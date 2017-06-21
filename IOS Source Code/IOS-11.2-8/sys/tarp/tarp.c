/* $Id: tarp.c,v 3.4.56.3 1996/08/28 13:16:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp.c,v $
 *------------------------------------------------------------------
 * TARP (Target ID Address Resolution Protocol)
 *
 * October, 1995 Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp.c,v $
 * Revision 3.4.56.3  1996/08/28  13:16:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.56.2  1996/05/17  21:03:26  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.4.56.1  1996/05/17  20:55:37  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.4  1995/11/28  18:58:48  asastry
 * CSCdi44538:  Configuring TARP TID causes crash if CLNS NET is missing
 *
 * Revision 3.3  1995/11/20  19:19:18  asastry
 * CSCdi43154:  TARP corrupted memory pointer. Fix pdb corruption bug. In
 * addition, incorporate these other misc. fixes:
 *
 *
 *         - Fix spurious TARP interface command during NVGEN
 *         - Reset sequence number to zero before sending TARP Type 4 PDU
 *         - Accept TARP PDU with sequence number of zero, regardless of
 *           whether there is an entry in Loop Detection Buffer
 *         - Zero out NSAP field in TARP TID cache if CLNS NET is deleted
 *           from underneath TARP
 *         - create TID entry with a zero NSAP if a TARP TID is entered
 *           before assigning a CLNS NET
 *
 * Revision 3.2  1995/11/17  18:44:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../clns/clns.h"
#include "../clns/clns_registry.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../if/network.h"
#include "tarp_externs.h"
#include "tarp_debug.h"

/*
 * Global variables
 */

watched_queue *tarpQ;
queuetype tarp_static_adjQ;          /* List of all adjacencies configured
					locally */
queuetype tarp_rqstQ;                /* Queue of outstanding requests */
boolean tarp_running;                /* True if TARP running */
boolean tarp_propagate_flag;         /* True if we're propagating TARP PDU's */
boolean tarp_originate_flag;         /* True if we're originating TARP PDU's */
boolean tarp_allow_cache_flag;     /* True if TID<->NET cache is active */
int tarpfork;                        /* Fork of TARP process */
ulong tarp_traffic[TARP_MAXTRAFFIC]; /* statistics */
char *tarp_my_tid;                   /* TID of this router */
ushort tarp_global_sequence_num;     /* sequence number used in PDU's
					originated by this system */
uchar tarp_global_nsel;              /* NSEL used in TARP PDU's */
uchar tarp_global_urc;               /* value of update remote cache bit */
ushort tarp_lifetime;                /* lifetime value (hops) */
uchar tarp_global_protocol_type;     /* protocol type sent in outgoing pkts */
uint tarp_ldb_flush_timer;           /* loop detection buffer timer */
uint tarp_tid_cache_timer;           /* TID<->NET cache entry timer */
uint tarp_t1_resp_timer;             /* time to respond to type 1 PDU */
uint tarp_t2_resp_timer;             /* time to respond to type 2 PDU */
uint tarp_arp_timer;                 /* time to respond to TARP rqst */
uint tarp_post_t2_timer;             /* started when T2 expires */
uint tarp_zero_seq_timer;            /* Zero-sequence timer (used by LDB) */
uchar tarp_zero_nsap[CLNS_MAX_NSAP_LENGTH] = {CLNS_MAX_NSAP_LENGTH-1,
					      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					      0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* 
 * TARP timers
 */

static sys_timestamp tarp_ager_timer;   /* Timer for tarp_ager() */
sys_timestamp tarp_type4_timer;         /* TARP timer used to wait for
					   some period of time before
					   sending TARP type 4 PDUs */

/*
 * TARP subsystem header
 */

#define TARP_MAJVERSION 1
#define TARP_MINVERSION 0
#define TARP_EDITVERSION  1

SUBSYS_HEADER(tarp, TARP_MAJVERSION, TARP_MINVERSION, TARP_EDITVERSION,
	      tarp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: clns",
	      "req: clns");


/*
 * tarp_init_idb
 *
 * Initialize interface-related stuff for TARP.
 */
void tarp_init_idb (idbtype *idb, boolean setup_hwidb)
{
    idb->tarp_enabled = FALSE;
    idb->tarp_propagate = TRUE;
}

/*
 * tarp_add_registry_calls
 *
 * Do all registry-related stuff here. 
 */
static void tarp_add_registry_calls (void)
{
    reg_add_tarp_enqueue(tarp_enqueue, "tarp_enqueue");
    reg_add_tarp_running_status(tarp_running_status, "tarp_running_status");
    reg_add_tarp_start_type4_timer(tarp_start_type4_timer, 
				   "tarp_start_type4_timer");
    reg_add_sw_idb_init(tarp_init_idb, "tarp_init_idb");
}

/*
 * tarp_init
 *
 * Initialize all TARP-related stuff.
 */
void tarp_init (subsystype *subsys)
{
    int i;
    idbtype *idb;

    tarp_debug_init();
    tarp_parser_init();
    tarp_add_registry_calls();
    queue_init(&tarp_static_adjQ, 0);    
    queue_init(&tarp_rqstQ, 0);    
    GET_TIMESTAMP(tarp_ager_timer);
    TIMER_STOP(tarp_type4_timer);

    tarp_ldb_flush_timer = TARP_DEF_LDB_FLUSH_TIMER;
    tarp_tid_cache_timer = TARP_DEF_TID_CACHE_TIMER;
    tarp_t1_resp_timer = TARP_DEF_TYPE1_TIMEOUT;
    tarp_t2_resp_timer = TARP_DEF_TYPE2_TIMEOUT;
    tarp_arp_timer = TARP_DEF_ARP_RQST_TIMEOUT;
    tarp_post_t2_timer = TARP_DEF_POST_TYPE2_TIMEOUT;
    tarp_zero_seq_timer = TARP_DEF_LDB_ZERO_SEQ_TIMER;
    tarp_lifetime = TARP_DEF_LIFETIME;
    tarp_global_protocol_type = TARP_NSAP_PROT_ADDR_TYPE;
    tarp_propagate_flag = TRUE;
    tarp_originate_flag = TRUE;
    tarp_allow_cache_flag = TRUE;
    tarp_my_tid = NULL;
    FOR_ALL_SWIDBS(idb) {
	tarp_init_idb(idb, TRUE);
    }

    for (i = 0; i < TARP_MAXTRAFFIC; i++)
        tarp_traffic[i] = 0;

    /*
     * Sequence number space begins with zero.
     */
    tarp_global_sequence_num = 0;
    
    /*
     * Update remote cache (URC) bit is 0, by default (i.e. recipients of
     * packets should update their cache entries).
     */
    tarp_global_urc = 0;

    tarp_global_nsel = TARP_DEF_NSEL_TYPE;
}

/*
 * tarp_running_status
 *
 * Determine if TARP is running or not (called from CLNS)
 */
boolean tarp_running_status (void)
{
    return(tarp_running);
}

/*
 * tarp_enqueue
 *
 * Called from CLNS to put a packet on the TARP input queue
 */
void tarp_enqueue (paktype *pak)
{
    if (pak->if_input->tarp_enabled && tarp_running) 
 	process_enqueue_pak(tarpQ, pak);
    else
	protocol_discard(pak, FALSE);
}

/*
 * tarp_process_packet
 *
 * Process the packet after it has been sent through validity checks.
 * The flag 'we_are_es' will be set to TRUE if we're an ES.
 */
static void tarp_process_packet (paktype *pak, boolean we_are_es)
{
    uchar tid[TARP_MAX_TID_LEN], sender_addr[CLNS_MAX_NSAP_LENGTH];
    uchar nsap_len, area1[CLNS_MAX_NSAP_LENGTH], area2[CLNS_MAX_NSAP_LENGTH];
    boolean isis_running;
    clns_pdbtype *clns_pdb;
    tarp_tid_cache_type *cache_entry, *my_cache_entry = NULL;
    clns_adjtype *sender_adj;
    tarp_pdutype *tarp_pdu;
    enum TARP_HOST_TYPE tarp_host_type;
    uchar *clns_net;

    tarp_pdu = (tarp_pdutype *)clnsdatastart(pak);

    if (tarp_pkt_debug) {
	buginf("\nTARP-PA: Received TARP type %d PDU on "
	       "interface %s", TARP_GET_CODE(tarp_pdu), 
	                       pak->if_input->namestring);

	buginf("\n\t Lft = %d, Seq = %d, Prot type = 0x%X, URC = %s",
	       GETSHORT(&tarp_pdu->lifetime), GETSHORT(&tarp_pdu->sequence),
	       tarp_pdu->prot_addr_type, (!TARP_GET_URC(tarp_pdu)) ? 
	                                               "TRUE" : "FALSE");

	buginf("\n\t Ttid len = %d, Stid len = %d, Prot addr len = %d",
	       tarp_pdu->target_len, tarp_pdu->source_len,
	       tarp_pdu->prot_addr_len);

	buginf("\n\t Packet sent/propagated by %s",
		nsap_string(pak->clns_srcaddr));

	tarp_build_address_string(tarp_pdu, sender_addr);
	buginf("\n\t Originator's NSAP : %s", nsap_string(sender_addr));
    
	/*
	 * Extract destination TID, if it exists (applicable only to PDU types
	 * 1, 2 and 4).
	 */    
	if (tarp_pdu->target_len) {
	    tarp_build_tid_string(tarp_pdu, tid, TARP_DEST);	
	    buginf("\n\t Target %sTID : %s",
		   (TARP_GET_CODE(tarp_pdu) == TARP_NOTIFY_PDU) ?
		                                    "(old) " : "", tid);
	}

	/*
	 * Extract source TID.
	 */
	tarp_build_tid_string(tarp_pdu, tid, TARP_SOURCE);
	buginf("\n\t Originator's %sTID : %s",
	       (TARP_GET_CODE(tarp_pdu) == TARP_NOTIFY_PDU) ? 
	                                        "(new) " : "", tid);
    }

    tarp_traffic[TARP_INPUT]++;

    /*
     * See if ISIS is running.
     */
    tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net);
    isis_running = (tarp_host_type == TARP_IS_TYPE);

    if (!clns_pdb && !we_are_es) {
	TARP_EVBUGINF("\nTARP-EV: ISIS not running on this system "
		      "- can't process packet");
	return;
    }

    /*
     * Look up requested (and our own) TID in our TID cache. If no
     * cache entry is found for our own TID, create it.
     */
    cache_entry = tarp_find_tid_entry(TARGET_TID(tarp_pdu),
				      tarp_pdu->target_len, TARP_TID_KEY);

    if (tarp_my_tid) {
        my_cache_entry = tarp_find_tid_entry(tarp_my_tid, strlen(tarp_my_tid),
					     TARP_TID_KEY);
	if (!my_cache_entry) {
	    TARP_EVBUGINF("\nTARP-EV: Cache entry for our TID not "
			  "found - creating new entry");
	    tarp_modify_router_tid(tarp_my_tid, TRUE);
	    my_cache_entry = tarp_find_tid_entry(tarp_my_tid, 
						 strlen(tarp_my_tid),
						 TARP_TID_KEY);
	}	
    }
    else
	TARP_EVBUGINF("\nTARP-EV: Warning - system's TID has not been set");

    /*
     * Get adj entry of the guy who sent us this packet (clear the NSEL before
     * doing the look up). This is needed to avoid propagating the received 
     * PDU back to the sender. Note that this doesn't apply to packets that are
     * 'unicast' to us (i.e. types 3 and 5 PDU's). 
     */
    if ((TARP_GET_CODE(tarp_pdu) == TARP_RESPONSE_PDU) ||
	(TARP_GET_CODE(tarp_pdu) == TARP_RQST_PROT_PDU))
	sender_adj = NULL;
    else {
	/*
	 * Coerce NSEL to be 0.
	 */
	nsap_len = pak->clns_srcaddr[0];
	pak->clns_srcaddr[nsap_len] = 0;
	sender_adj = find_adjacency(clns_adjacency_db, pak->clns_srcaddr, TRUE,
				    NULL, 0);
    }

    /*
     * Now process the packet. 
     * The algorithm is as follows:
     *
     * If we are running as an ES:
     *
     * If we receive a Type 1 or 2 TARP PDU, we check if the packet is for us.
     * If it is, we process it. If it's not for us, we don't do anything else.
     *
     * If we are running as ISIS L1:
     * 
     * If we receive a Type 1 or 2 TARP PDU, we check if the packet is for us.
     * If it is, we process it. If it's not for us, we perform 'L1
     * propagation', i.e. propagate the packet to all our L1 neighbors (except
     * to the one who sent it). 
     *
     * If we are running as ISIS L2:
     *
     * If we receive a Type 1 TARP PDU, we check if the packet is for us. If
     * it is, we process it. If it's not for us, we check our TID cache, and
     * if we find a match, we process it. Otherwise, we perform 'L1
     * propagation'.
     *
     * If we receive a Type 2 TARP PDU, we check if the packet is for us. If
     * it is, we process it. If it's not for us, we check if the sender's area
     * is different from ours. If it is, we update (or create) the cache entry
     * for the sender . For the last two cases, we also perform
     * 'L1 and L2 propagation'.
     * 
     */

    /*
     * Extract destination TID.
     */
    tarp_build_tid_string(tarp_pdu, tid, TARP_DEST);

    switch (TARP_GET_CODE(tarp_pdu)) {
        case TARP_RQST_TID_L1_PDU:
	    /*
	     * Special case if we're an ES. If the PDU is for us, process
	     * it; else don't bother.
	     */
	    if (we_are_es) {
		if (tarp_my_tid && (strcmp(tarp_my_tid, tid) == 0))
		    tarp_handle_type1_2_pdu(tarp_pdu, my_cache_entry);

		break;
	    }

	    if (tarp_my_tid && (strcmp(tarp_my_tid, tid) == 0))
	        tarp_handle_type1_2_pdu(tarp_pdu, my_cache_entry);
	    else if ((clns_pdb->level & ISIS_CIRCUIT_L2) && cache_entry) {
		/*
		 * We've received a type 1 PDU, and we're running L2. Look
		 * up the L2 cache for a match. If there is a match, we
		 * send a type 3 (response) PDU. Otherwise, we behave as if
		 * we had no cache entry, and perform a L1 propagation. Since
		 * we don't maintain level information in our TID cache, we
		 * determine if a particular entry is a L2 entry by comparing
		 * the area field in the cache entry with our area. Note that
		 * the originator TID and originator protocol address in the
		 * response packet will be those corresponding to the
		 * requested TID (instead of the TID and protocol address
		 * of the originator of the type 3 PDU).
		 */
		tarp_extract_area(sender_addr, area1);
		tarp_extract_area(cache_entry->net, area2);		
		if (!nsap_equ(area1, area2))
		    tarp_handle_type1_2_pdu(tarp_pdu, cache_entry);
		else
		   tarp_propagate(tarp_pdu, CLNS_L1_IS_ADJ_TYPE, FALSE,
				  sender_adj);
	    }
	    else if (isis_running) 
		tarp_propagate(tarp_pdu, CLNS_L1_IS_ADJ_TYPE, FALSE,
			       sender_adj);

	    break;
	case TARP_RQST_TID_L2_PDU:  
	    /*
	     * Special case if we're an ES. If the PDU is for us, process
	     * it; else don't bother.
	     */
	    if (we_are_es) {
		if (tarp_my_tid && (strcmp(tarp_my_tid, tid) == 0))
		    tarp_handle_type1_2_pdu(tarp_pdu, my_cache_entry);

		break;
	    }

	    if (tarp_my_tid && (strcmp(tarp_my_tid, tid) == 0))
	        tarp_handle_type1_2_pdu(tarp_pdu, my_cache_entry);
	    else if (clns_pdb->level == ISIS_CIRCUIT_L1)
		 tarp_propagate(tarp_pdu, CLNS_L1_IS_ADJ_TYPE, FALSE,
				sender_adj);
	    else if (clns_pdb->level & ISIS_CIRCUIT_L2) {
		/*
		 * Check if the sender's area is different from ours. If
		 * it is, update the cache entry for the sender.
		 */
		tarp_build_address_string(tarp_pdu, sender_addr);
		tarp_extract_area(sender_addr, area1);
		tarp_extract_area(my_cache_entry->net, area2);		
		if (!nsap_equ(area1, area2)) {
		    TARP_PABUGINF("\nTARP-PA: Sender's area (%s) different "
				  "from our area (%s) - "
				  "\n\t updating cache entry ",
				  nsap_string(area1), nsap_string(area2));
		    /*
		     * Extract source TID.
		     */
		    tarp_build_tid_string(tarp_pdu, tid, TARP_SOURCE);
		    tarp_update_tid_entry(tid, strlen(tid),
			 		  tarp_pdu->prot_addr_type,
					  SOURCE_PROT_ADDR(tarp_pdu),
					  tarp_pdu->prot_addr_len, TRUE,
					  FALSE);
                }
		tarp_propagate(tarp_pdu,
			       CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE,
			       FALSE, sender_adj);
	    }

	    break;
	case TARP_RESPONSE_PDU:     
	    tarp_handle_type3_pdu(tarp_pdu);
	    break;
	case TARP_NOTIFY_PDU:       
	    tarp_handle_type4_pdu(tarp_pdu, sender_adj, we_are_es);
	    break;
	case TARP_RQST_PROT_PDU:    
	    tarp_handle_type5_pdu(tarp_pdu);
	    break;
	default:
	    tarp_build_address_string(tarp_pdu, sender_addr);
	    TARP_EVBUGINF("\nTARP-EV: Invalid TARP packet type seen from %s",
			  tarp_addr_string(tarp_pdu, sender_addr));
	    tarp_traffic[TARP_BADPACKET]++;
	    tarp_propagate(tarp_pdu, (clns_pdb->level == ISIS_CIRCUIT_L1) ?
			   CLNS_L1_IS_ADJ_TYPE :
			   CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE, FALSE,
			   sender_adj);
	    return; 
    }

    return;
}

/*
 * tarp_receive_packet
 *
 * Handle the packet that we just received. Packet is disposed of by caller.
 */
static void tarp_receive_packet (paktype *pak)
{
    tarp_pdutype *tarp_pdu;
    tarp_ldb_type *ldb;
    uchar sender_addr[CLNS_MAX_NSAP_LENGTH];
    uchar *system_id;
    boolean we_are_es = FALSE;
    clns_pdbtype *clns_pdb;
    uchar *clns_net;
    enum TARP_HOST_TYPE tarp_host_type;

    tarp_pdu = (tarp_pdutype *)clnsdatastart(pak);

    tarp_build_address_string(tarp_pdu, sender_addr);

    /*
     * Perform validity checks on incoming packet.
     */

    /*
     * Don't bother if packet's lifetime is over.
     */
    if (GETSHORT(&tarp_pdu->lifetime) == 0) {
	TARP_EVBUGINF("\nTARP-EV: Packet from %s discarded",
		      tarp_addr_string(tarp_pdu, sender_addr));
	TARP_EVBUGINF("\n\t  - packet lifetime exceeded");
	tarp_traffic[TARP_LIFETIME_EXCEEDED]++;
	return;
    }

    /*
     * Don't bother if the protocol type is not a supported one.
     */
    if (!TARP_SUPPORTED_PROTOCOL(tarp_pdu->prot_addr_type)) {
	TARP_EVBUGINF("\nTARP-EV: Packet from %s discarded"
		      "\n\t  - unsupported protocol type %X",
		      tarp_addr_string(tarp_pdu, sender_addr),
		      tarp_pdu->prot_addr_type);
	tarp_traffic[TARP_BADPACKET]++;
	return;
    }

    /*
     * PDU types 1, 2 and 4 should have a target ID.
     */
    if ((TARP_GET_CODE(tarp_pdu) == TARP_RQST_TID_L1_PDU) ||
        (TARP_GET_CODE(tarp_pdu) == TARP_RQST_TID_L2_PDU) ||
	(TARP_GET_CODE(tarp_pdu) == TARP_NOTIFY_PDU)) {
	if (tarp_pdu->source_len == 0) {
	    TARP_EVBUGINF("\nTARP-EV: Packet from %s (PDU type %d) discarded"
			  "\n\t  - target ID is missing in packet",
			  tarp_addr_string(tarp_pdu, sender_addr),
			  TARP_GET_CODE(tarp_pdu));
	    return;
	}
    }

    /*
     * Determine if we're an ES. The flag 'we_are_es' will be set to TRUE
     * if we are, and in this case, we don't run through the loop detection
     * check.
     */
    tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net);
    we_are_es = (tarp_host_type == TARP_ES_TYPE);

    if (we_are_es) {
	tarp_process_packet(pak, we_are_es);
	return;
    }	

    system_id = (tarp_pdu->prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE) ?
	         get_station(sender_addr) : sender_addr;

    /*
     * Search in loop detection buffer to see if we have an entry for this
     * system.
     *
     * The algorithm for loop detection is as follows:
     *
     * If cache entry exists in loop detection buffer (LDB):
     * If packet's sequence # is <= that in cache, and is not zero, we
     * discard the packet. If the packet's sequence # is > that in cache,
     * we update the cache entry (and process the packet). If the packet's
     * sequence # is zero, and the zero sequence timer is not running, we 
     * treat it as new info and update the cache entry (and process the 
     * packet). If the packet's sequence # is zero, and the zero sequence 
     * timer is running, we discard the packet (this will avoid packets
     * looping around).
     *
     * Else..
     *
     * If no cache entry exists in the loop detection buffer (LDB), create
     * an entry. If the sequence number is zero, we also start the
     * zero sequence timer.
     */
    ldb = tarp_find_ldb_entry(system_id);
    if (ldb) {
        if ((GETSHORT(&tarp_pdu->sequence) <= ldb->sequence) &&
	    (GETSHORT(&tarp_pdu->sequence) != 0)) {
	    if (tarp_event_debug) {
		buginf("\nTARP-EV: Packet from %s discarded - sequence ",
		       tarp_addr_string(tarp_pdu, sender_addr));
		buginf("\n\t number (%d) <= that in LDB cache entry (%d)",
		       GETSHORT(&tarp_pdu->sequence), ldb->sequence);
	    }
	    return;
	}
	else if (GETSHORT(&tarp_pdu->sequence) > ldb->sequence) {
	    if (tarp_event_debug) {
		buginf("\nTARP-EV: Packet from %s has a ",
		       tarp_addr_string(tarp_pdu, sender_addr));
		buginf("\n\t sequence number (%d) > that in LDB cache entry "
		       "(%d) ", GETSHORT(&tarp_pdu->sequence), ldb->sequence);
		buginf("\n\t - updating cache entry");
	    }
	    tarp_update_ldb_entry(ldb, GETSHORT(&tarp_pdu->sequence),
				  tarp_ldb_flush_timer);
	}
	else {
	    /*
	     * Incoming PDU has a sequence number of zero. If the
	     * 'zero sequence' timer is running, discard packet. Else, process
	     * the packet.
	     */
	    if (TIMER_RUNNING(ldb->zero_seq_time)) {
		if (tarp_event_debug) {
		    buginf("\nTARP-EV: Packet from %s discarded - ",
		       tarp_addr_string(tarp_pdu, sender_addr));
		    buginf("\n\t incoming packet's sequence number is zero,"
		       "\n\t and LDB entry's zero sequence timer "
		       "\n\t is still running");
		}
		return;
	    }
	    else {
		if (tarp_event_debug) {
		    buginf("\nTARP-EV: Packet from %s has a "
			   "\n\t sequence number of zero "
			   " - updating LDB cache entry",
			   tarp_addr_string(tarp_pdu, sender_addr));
		    buginf("\n\tStarting zero sequence timer "
			   "(value : %d seconds)",
		           tarp_zero_seq_timer/ONESEC);
		}
		tarp_update_ldb_entry(ldb, GETSHORT(&tarp_pdu->sequence),
				      tarp_ldb_flush_timer);
		TIMER_START(ldb->zero_seq_time, tarp_zero_seq_timer);
	    }
	}
    }
    else {
	/*
	 * No entry was found in the loop detection cache. Make an entry in
	 * the cache. If we're installing an LDB entry with sequence number
	 * of zero, start the LDB zero-sequence timer.
	 *
	 */
	if ((ldb = tarp_insert_ldb_entry(GETSHORT(&tarp_pdu->sequence), 
					 system_id)) == NULL)
	    return;
	if (GETSHORT(&tarp_pdu->sequence) == 0) {
	    if (tarp_event_debug) {
		buginf("\nTARP-EV: Creating new LDB entry for %s",
			  tarp_addr_string(tarp_pdu, sender_addr));
		buginf("\n\tStarting zero sequence timer with %d seconds",
		       tarp_zero_seq_timer/ONESEC);
	    }
	    TIMER_START(ldb->zero_seq_time, tarp_zero_seq_timer);
	}
    }

    /*
     * Now process the packet.
     */
    tarp_process_packet(pak, FALSE);

    return;
}

/*
 * tarp_process_incoming_packets
 *
 * Handle any received packets for TARP process. Incoming packet is
 * disposed of here.
 */
static void tarp_process_incoming_packets (void)
{
    paktype *pak;
    int count;

    /*
     * Process only a limit number of packets per pass.
     */
    count = PAK_SWITCHCOUNT;
    while (--count > 0) {	
	pak = process_dequeue(tarpQ); 
	if (pak == NULL)
	    break;

	if (!interface_up(pak->if_input)) {
	    datagram_done(pak);
	    continue;
	}

	tarp_receive_packet(pak);
	datagram_done(pak);
    }
}

/*
 * tarp_prot
 * 
 * Main TARP (Target ID Address Resolution Protocol) process
 */

static forktype tarp_prot (void)
{
    ulong major, minor;
    paktype *pak;

    process_wait_on_system_init();

    tarpQ = create_watched_queue("TARP packets", 0, 0);
    process_watch_queue(tarpQ, ENABLE, RECURRING);
    process_watch_timer(&tarp_ager_timer, ENABLE);
    while (tarp_running) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	        case QUEUE_EVENT:
		    tarp_process_incoming_packets();
		    break;
		case TIMER_EVENT:
		    tarp_ldb_ager();
		    tarp_tid_ager();
		    tarp_check_type4_timer();
		    TIMER_START(tarp_ager_timer, TARP_PERIODIC_TIMER);
		    break;
		default:
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		    break;
	    }
	}
    }

    /* Clean up ... */
    tarp_clean_up();

    /* Kill the main timer and any subordinates. */

    process_watch_timer(&tarp_ager_timer, DISABLE);
    process_watch_queue(tarpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(tarpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&tarpQ);

    /* Die ... */
    tarpfork = 0;
    TARP_EVBUGINF("\nTARP-EV: Killing TARP process");
    process_kill(THIS_PROCESS);
}

/*
 * tarp_start_process
 *
 * Start the TARP process.
 */
void tarp_start_process (void)
{
    if (!tarpfork) {
	tarp_running = TRUE;
        tarpfork = process_create(tarp_prot, "TARP Protocol", 
				  NORMAL_STACK, PRIO_NORMAL);
    }
}
