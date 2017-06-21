/* $Id: isis_flood.c,v 3.7.42.7 1996/08/28 12:38:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_flood.c,v $
 *------------------------------------------------------------------
 * isis_flood.c -- IS-IS flooding functions
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_flood.c,v $
 * Revision 3.7.42.7  1996/08/28  12:38:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.42.6  1996/08/12  01:06:53  dkatz
 * CSCdi45425:  NLSP may not report RIP/SAP routes
 * Branch: California_branch
 * Don't let the DR timer get in the way of reporting adjacencies (and
 * remove a couple of other checks for it).  Rebuild both non-pseudonode
 * and pseudonode LSPs when an interface changes state.
 *
 * Revision 3.7.42.5  1996/08/11  15:13:17  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.7.42.4  1996/08/04  23:35:13  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.7.42.3  1996/06/18  16:50:52  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.7.42.2  1996/05/26  06:46:06  dkatz
 * CSCdi58737:  ISIS/NLSP debug messages can be misleading
 * Branch: California_branch
 * Issue debug messages before transmitting packets.  Add instrumentation
 * for retransmissions.
 *
 * Revision 3.7.42.1  1996/03/18  19:03:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.2  1996/03/13  01:13:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.1  1996/03/07  08:40:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/01/05  04:55:35  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/22  21:58:13  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.5  1995/12/01  04:09:41  dkatz
 * CSCdi37675:  ISIS/NLSP can retransmit early
 *
 * Revision 3.4  1995/11/23  06:50:32  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/17  08:53:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:52:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/08  16:42:57  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.2  1995/06/21 08:53:50  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:18:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This module implements the sending and flooding LSP logic for the
 * ISO UPDATE process.
 *
 * The basic mechanism involves manipulation two sets of flags in each LSP:
 *   SRM - send routing message
 *   SSN - send sequence number
 *
 * There is a bit in the mask for each interface in the system (idb->number). 
 *
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "config.h"
#include "clns.h"
#include "clns_route.h"
#include "isis.h"
#include "clns_externs.h"
#include "isis_externs.h"
#include "isis_inlines.h"
#include "isis_ipx.h"
#include "bitlogic.h"
#include "../xns/ipx_registry.h"

/*
 * Infinite LSPID value. Used for CSNP end_id range.
 */
static const isis_lspid isis_infinite_lspid = 
{ {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, 0xff, 0xff };


/*
 * isis_initialize_lsp_transmission_fields
 *
 * Set up the fields to send a bunch of LSPs.
 */
static void isis_initialize_lsp_transmission_fields (clns_pdbtype *pdb,
						     isisidbtype *isisidb)
{
    isisidb->isis_sent_an_lsp = FALSE;
    lspid_copy(&isis_infinite_lspid, &isisidb->last_lsp_sent);
    if ((pdb->level & ISIS_CIRCUIT_L1) &&
	(isisidb->isis_circuit_type & ISIS_CIRCUIT_L1))
	isisidb->last_lsp_level_sent = ISIS_CIRCUIT_L1;
    else
	isisidb->last_lsp_level_sent = ISIS_CIRCUIT_L2;
}

/*
 * isis_wait_for_serial_csnp
 *
 * Returns TRUE if the interface is a serial line waiting for the initial
 * CSNP exchange (in such cases we don't want to send LSPs until after
 * the wait time has completed).  We set the pacing timer to go off just
 * after the wait timer if true (so we send LSPs at the end of the period).
 */
static boolean isis_wait_for_serial_csnp (clns_pdbtype *pdb, idbtype *idb,
				   isisidbtype *isisidb)
{
    sys_timestamp wakeup_time;

    if (mgd_timer_running(&isisidb->isis_dr_timer) && is_p2p(idb)) {
	wakeup_time = mgd_timer_exp_time(&isisidb->isis_dr_timer);
	TIMER_ADD_DELTA(wakeup_time, 1);
	mgd_timer_set_exptime(&isisidb->isis_lsp_pacing_timer, &wakeup_time);
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * isis_really_start_lsp_transmission
 *
 * This routine is called when we're not currently transmitting
 * LSPs on an interface.  We initialize things, fire off a timer,
 * and kick back.  The real work happens when the timer expires the
 * first time.  We delay the first LSP by 33 msec with 50% jitter.
 * Seems like a good number.
 *
 * If this is a point-to-point link and the DR timer is running (meaning
 * that the link just came up and we're holding the LSPs until the CSNP
 * exchange completes), set the first transmission time to the DR timer
 * expiry instead.
 */
void isis_really_start_lsp_transmission (clns_pdbtype *pdb, idbtype *idb,
					 isisidbtype *isisidb)
{
    isis_initialize_lsp_transmission_fields(pdb, isisidb);
    if (!isis_wait_for_serial_csnp(pdb, idb, isisidb)) {
	mgd_timer_start_jittered(&isisidb->isis_lsp_pacing_timer,
				 ISIS_MINBCASTLSPGENINTERVAL, 50);
    }
}

/*
 * isis_set_srm
 *
 * Set an SRM bit for an LSP.  Kicks the LSP transmitter on the interface.
 *
 * If the interface is down, we skip the whole thing.
 */
void isis_set_srm (clns_pdbtype *pdb, idbtype *idb, isis_lspdbtype *lsp)
{
    isisidbtype *isisidb;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb || !isisidb->isis_interface_up || !isisidb->isis_running)
	return;

    bitfield_set(isisidb->isis_if_number, &lsp->SRM);
    isis_start_lsp_transmission(pdb, idb, isisidb);
}

/*
 * isis_start_all_transmitters
 *
 * Kick the transmitter on all interfaces.
 */
void isis_start_all_transmitters (clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;
    idbtype      *idb;
    isisidbtype *isisidb;

    /* For each interface, kick the transmitter. */

    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);

	if (isisidb && isisidb->isis_running && isisidb->isis_interface_up) {
	    isis_start_lsp_transmission(pdb, idb, isisidb);
	}
    }
}

/*
 * isis_set_all_srm
 *
 * Set all SRM bits according to the mesh group configured on the input
 * interface, if any.  The rule is that we flood to all interfaces except
 * those that are a member of the same mesh group as the input interface and
 * those that are labelled as blocked.
 *
 * If no receive interface is specified, the LSP will be flooded on
 * all interfaces except those marked "blocked."
 *
 * Note that SRM_mask has its bits set and cleared as the interface goes
 * up and down.  We rely on the fact that no SRM bit will be left set,
 * since purged LSPs will never be deleted from the LSP database otherwise.
 * To accommodate this fact, we note that isis_interface_up and the
 * corresponding SRM_mask bit are set atomically, so if an interface is
 * up, the SETBITMASK will do something for that interface, and
 * isis_start_all_transmitters will start the transmitter.  Conversely,
 * if the interface is down, we will not set the bit and will not start
 * the process.
 *
 * Note also that if an interface goes down in the middle of transmitting
 * LSPs, the process will continue to run and clear all the corrsponding
 * SRM bits.  Whew.
 */
void isis_set_all_srm (clns_pdbtype *pdb, isis_lspdbtype *lsp,
		       isisidbtype *rcv_isisidb)
{
    idbqueuetype *idbqueue;
    idbtype      *idb;
    isisidbtype *isisidb;

    /*
     * Set all the SRM bits, and then clear the bits associated with the
     * mesh group.  If there are any interfaces in the "blocked" mesh group,
     * clear those too.
     */
    bitfield_setmask(&pdb->SRM_mask, &lsp->SRM);
    if (rcv_isisidb && rcv_isisidb->isis_mesh_group) {
	bitfield_clearmask(&rcv_isisidb->isis_mesh_group->mesh_group_bits,
			   &lsp->SRM);
    }
    if (pdb->mesh_group_blocked) {
	bitfield_clearmask(&pdb->mesh_group_blocked->mesh_group_bits,
			   &lsp->SRM);
    }

    /* Now start all transmitters for which there's a bit set. */

    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);

	if (isisidb && isisidb->isis_running && isisidb->isis_interface_up
	    && bitfield_check(isisidb->isis_if_number, &lsp->SRM)) {
	    isis_start_lsp_transmission(pdb, idb, isisidb);
	}
    }
}

/*
 * isis_rw_set_srm
 *
 * Set SRM (send routing message) flag in LSP for given idb.
 */
boolean isis_rw_set_srm (isis_tree_type *element, void *idb_parm,
			 void *pdb_parm, void *dummy1, void *dummy2)
{
    idbtype *idb;
    clns_pdbtype *pdb;

    idb = idb_parm;
    pdb = pdb_parm;
    isis_set_srm(pdb, idb, element->lsp);
    return(TRUE);
}    

/*
 * isis_add_to_rexmit_q
 *
 * Add the specified lspid to the re-transmit queue.
 */
static void isis_add_to_rexmit_q (clns_pdbtype *pdb, idbtype *idb,
				 isis_lspdbtype *lsp_db, int level)
{
    isis_rexmit_entry *element;
    isisidbtype *isisidb;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * If this pdu is already on the queue, don't put it on again.
     */
    if (!bitfield_check(isisidb->isis_if_number, &lsp_db->on_rexmit_queue)) {

	/*
	 * Allocate timer memory, store values and enqueue.
	 */
	element = malloc_named(sizeof(isis_rexmit_entry), "ISIS Rexmit");
	if (!element) return;
	
	element->idb = idb;
	element->lsp_seq = GETLONG(&lsp_db->data->sequence_number);
	lspid_copy(&lsp_db->data->lspid, &element->timer_id);
	element->level = level;
 	TIMER_START(element->rexmit_timer, isisidb->isis_retrans_interval);
 	enqueue(&isisidb->isis_rexmit_queue, element);
	bitfield_set(isisidb->isis_if_number, &lsp_db->on_rexmit_queue);

	/* If the interface rexmit timer isn't running, start it. */

	if (!mgd_timer_running(&isisidb->isis_rexmit_timer)) {
	    mgd_timer_set_exptime(&isisidb->isis_rexmit_timer,
				  &element->rexmit_timer);
	}
    }
}

/*
 * isis_build_lsp_packet
 *
 *    construct an LSP packet for the specified level
 *
 */
#ifdef TEST_BAD_CHECKSUMS
static int errored_packet_interval = 0;	/* How often we make a bad packet */
static int current_packet_number = 0;	/* Our interval counter */
#endif

static paktype *isis_build_lsp_packet (
    clns_pdbtype *pdb,
    isis_lspdbtype *lsp_db,
    int level)
{
    paktype          *pak;
    isis_LSP_hdrtype *isis;
    int               copy_bytes, pdu_bytes;

    pdu_bytes = isis_lspdb_tlv_length(lsp_db) + ISIS_LSP_HEADER_SIZE;
    pak = getbuffer(pdu_bytes + pdb->packet_overhead);
    if (!pak) return (NULL);

    isis = isis_pak_start(pak, pdb);
    memset(isis, 0, pdu_bytes);

    isis->pid = NLP_ISIS;
    isis->hlen = ISIS_LSP_HEADER_SIZE;
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis->version_pid = NLSP_VERSION;
    } else {
	isis->version_pid = ISIS_VERSION;
    }
    isis->id_length = 0;
    isis->type = (level == ISIS_CIRCUIT_L1) ? ISIS_L1_LSP : ISIS_L2_LSP;
    isis->version_pdu = ISIS_VERSION_LSP;
    isis->max_area_addresses = 0;
    PUTSHORT(isis->pdu_length, pdu_bytes);

    /*
     * Set hold-time in units of seconds within PDU.
     */
    if (lsp_db->lsp_active) {
	PUTSHORT(isis->ht,
		 mgd_timer_left_sleeping(&lsp_db->lsp_timer) / ONESEC);
    } else {

	/* This LSP has expired. */

	PUTSHORT(isis->ht, 0);
    }

    /*
     * Now copy the data from the lsp_db.
     */
    copy_bytes = pdu_bytes - ISIS_LSP_HEADER_SIZE + ISIS_LSP_CORE_SIZE;
    bcopy(&lsp_db->data, (uchar *) &isis->isis_LSP_hdr_copy_start, copy_bytes);

#ifdef TEST_BAD_CHECKSUMS
    /*
     * If we're supposed to induce checksum errors, do so now.
     */
    if (errored_packet_interval) {
	current_packet_number++;
	if (current_packet_number >= errored_packet_interval) {
	    isis->checksum[0]++;	/* Make it bad. */
	    current_packet_number = 0;
	}
    }
#endif
    return(pak);
}

/*
 * isis_send_one_lsp
 *
 * Called at the bottom of a recursive walk to send an LSP.  Returns FALSE
 * if packet was sent (to terminate the walk) and TRUE otherwise (to
 * continue).
 *
 * Also manipulates various flags to keep track of the state of the
 * transmission on this interface.
 *
 * If the interface is down or there are no adjacencies present, we
 * simply clear the SRM bit and go on.  This will cause the state machine
 * to very rapidly clear all SRM bits for a particular interface if it
 * dies for some reason.
 */
static boolean isis_send_one_lsp (isis_tree_type *element, void *pdb_parm,
				  void *idb_parm, void *level_parm,
				  void *dummy)
{
    paktype          *pak;
    isis_LSP_hdrtype *isis;
    int              nbytes;
    hwaddrtype       snpa;
    boolean	     result;
    isisidbtype	     *isisidb;
    isis_lspdbtype   *lsp_db;
    boolean	     serial;
    ulong	     lsp_size, link_mtu;
    clns_pdbtype *pdb;
    idbtype *idb;
    int *level_ptr;
    int level;

    pdb = pdb_parm;
    idb = idb_parm;
    level_ptr = level_parm;
    level = *level_ptr;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return(FALSE);
    lsp_db = element->lsp;

    /* Don't send it if SRM is clear. */

    if (!bitfield_check(isisidb->isis_if_number, &lsp_db->SRM)) 
	return(TRUE);

    /* Don't send it if it's on the retransmission queue. */

    if (bitfield_check(isisidb->isis_if_number, &lsp_db->on_rexmit_queue))
	return(TRUE);			/* It'll get sent later. */

    /* Don't send it if it's being rate limited. */

    if (mgd_timer_running(&lsp_db->gen_limit_timer))
	return(TRUE);			/* It'll go when it expires. */

    /*
     * If there are no adjacencies for given level, or the interface is
     * down, or the LSP is a placeholder, clear the SRM bit and go on.
     */
    if (!isisidb->isis_adj_count[level - 1] || !isisidb->isis_interface_up ||
	isis_dummy_lsp(lsp_db)) {
	bitfield_clear(isisidb->isis_if_number, &lsp_db->SRM);
	return(TRUE);
    }

    serial = is_p2p(idb);

    memset(&snpa, 0, sizeof(snpa));
    snpa.type = STATION_ILLEGAL;
    snpa.length = 0;

    if (level == ISIS_CIRCUIT_L1) {
	snpa.target_type = TARGET_OSI_ALL_L1;
    } else {
	snpa.target_type = TARGET_OSI_ALL_L2;
    }

    /*
     * Add to retransmission queue for serial links.
     */
    if (serial) {
	isis_add_to_rexmit_q(pdb, idb, lsp_db, level);
    }

    /* 
     * Construct a packet. 
     */
    pak = isis_build_lsp_packet(pdb, lsp_db, level);
    if (!pak) return(FALSE);

    isis = isis_pak_start(pak, pdb);
    nbytes = GETSHORT(isis->pdu_length);

    /* Complain bitterly if we can't transmit the LSP on this circuit. */

    lsp_size = nbytes + pdb->packet_overhead;
    link_mtu = max_proto_mtu(idb, pdb->proc_type == PROC_TYPE_NLSP ?
			     LINK_NOVELL : LINK_CLNS);
    if (lsp_size > link_mtu) {
	errmsg(&msgsym(LSPTOOBIG, CLNS), pdb->errmsg_name, level,
	       lspid_string(&isis->lspid), lsp_size, idb->namestring, 
	       link_mtu);
	retbuffer(pak);
	return(FALSE);
    }

    /* 
     * Send it. 
     */
    pak->if_output = idb;

    update_pbuginf(pdb, "Sending L%d LSP %s, seq %x, ht %d on %s",
		   level, lspid_string(&isis->lspid), 
		   GETLONG(&isis->sequence_number), 
		   GETSHORT(isis->ht), idb->namestring);
    result = isis_send_packet(pdb, pak, nbytes, idb, &snpa);
    if (result) {
	isis_traffic[pdb->proc_type][(level == ISIS_CIRCUIT_L1) ? 
				     ISIS_L1_LSP_OUT : ISIS_L2_LSP_OUT]++;
	isisidb->isisidb_traffic[(level == ISIS_CIRCUIT_L1) ?
				 ISISIDB_L1_LSP_OUT :
				 ISISIDB_L2_LSP_OUT]++;
    } else {
	if (!is_x25(idb->hwptr)) {
	    update_pbuginf(pdb, "LSP encaps failed on %s", 
			   idb->namestring);
	}
    }

    /*
     * On LANs - clear the SRM bit. On serial interfaces, wait for ack to
     * before clearing SRM bit.
     */
    if (!serial) {
	bitfield_clear(isisidb->isis_if_number, &lsp_db->SRM);
    }

    /* Update the various fields and restart the timer. */

    lspid_copy(&element->lsp->data->lspid, &isisidb->last_lsp_sent);
    isisidb->isis_sent_an_lsp = TRUE;
    mgd_timer_start(&isisidb->isis_lsp_pacing_timer, isisidb->pacing_interval);
    return(FALSE);			/* Done for this pass. */
}


/*
 * isis_rexmit_packet
 *
 * Potentially retransmit a packet
 *
 * Called from the update routine whenever a retransmission timer expires.
 */
void isis_rexmit_packet (clns_pdbtype *pdb, idbtype *idb)
{
    isis_lspdbtype    *lsp_db;
    isisidbtype	    *isisidb;
    isis_rexmit_entry *rexmit_entry;
    boolean sent_one;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb) {
	mgd_timer_stop(&isisidb->isis_rexmit_timer);
	return;
    }

    /* If the rexmit queue is empty, stop the timer. */

    rexmit_entry = isisidb->isis_rexmit_queue.qhead;
    if (!rexmit_entry) {
	mgd_timer_stop(&isisidb->isis_rexmit_timer); /* Nothing to do. */
	return;
    }

    /*
     * If the head of the rexmit queue isn't ready to go yet, restart
     * the timer and bail.
     */
    if (SLEEPING(rexmit_entry->rexmit_timer)) {
	mgd_timer_set_exptime(&isisidb->isis_rexmit_timer,
			      &rexmit_entry->rexmit_timer);
	return;
    }

    dequeue(&isisidb->isis_rexmit_queue); /* Pull it off now. */
    sent_one = FALSE;

    /* Look up the LSP. */

    if (rexmit_entry->level == ISIS_CIRCUIT_L1) {
	lsp_db = isis_find_L1_lsp(pdb, &rexmit_entry->timer_id);
    } else {
	lsp_db = isis_find_L2_lsp(pdb, &rexmit_entry->timer_id);
    }
    
    if (lsp_db) {

  	/*
 	 * Check the LSP sequence number.  If it doesn't match, it means that
 	 * a newer version of the LSP has been installed in the database.
 	 * If the new version needs to be retransmitted, there will be
 	 * another retransmit entry coming along eventually.
 	 */
 	if (rexmit_entry->lsp_seq == GETLONG(&lsp_db->data->sequence_number)) {
	    
	    /* Note that it's been dequeued. */
	    
	    bitfield_clear(isisidb->isis_if_number, &lsp_db->on_rexmit_queue);
	    
	    /*
	     * If there are no adjacencies of a given level for the interface,
	     * don't retransmit the packet.
	     */
	    if (!isisidb->isis_adj_count[rexmit_entry->level - 1]) {
		bitfield_clear(isisidb->isis_if_number, &lsp_db->SRM);
	    }

	    /*
	     * If LSP still needs to be sent, kick the transmitter.
	     */
	    if (bitfield_check(isisidb->isis_if_number, &lsp_db->SRM)) {
 		update_pbuginf(pdb, "Retransmitting L%d LSP %s on %s",
 			       rexmit_entry->level,
 			       lspid_string(&lsp_db->data->lspid),
 			       idb->namestring);
		isis_start_lsp_transmission(pdb, idb, isisidb);
 		isis_traffic[pdb->proc_type][ISIS_RETRANSMITS]++;
		sent_one = TRUE;
	    }
	}
    }
    free(rexmit_entry);

    /*
     * Now handle the timer.  If we sent one, set the timer to expire
     * after the appropriate holddown interval.  If not, set it to expire
     * at the retransmission time of the next entry on the queue (or stop
     * the timer if there are no more entries).
     */
    if (sent_one && isisidb->isis_min_rexmit_interval) {
	mgd_timer_start(&isisidb->isis_rexmit_timer,
			isisidb->isis_min_rexmit_interval);
    } else {
	rexmit_entry = isisidb->isis_rexmit_queue.qhead;
	if (rexmit_entry) {
	    mgd_timer_set_exptime(&isisidb->isis_rexmit_timer,
				  &rexmit_entry->rexmit_timer);
	} else {
	    mgd_timer_stop(&isisidb->isis_rexmit_timer);
	}
    }
}


/* 
 * isis_increment_seq
 *
 *     Increment the sequence number in the lsp. If it has exceeded the
 *     range, disable the routing module until all old info is gone. LATER???
 */
void isis_increment_seq (clns_pdbtype *pdb, ulong *sequence_number_ptr)
{
    ulong sequence_number = GETLONG(sequence_number_ptr);
    if (sequence_number < ISIS_SEQUENCE_MODULUS)
	sequence_number += 1;
    else{
	update_pbuginf(pdb, "Sequence number wrap");
	sequence_number = 1;
    }
    PUTLONG(sequence_number_ptr, sequence_number);
}

/*
 * isis_psnp
 *
 *   This routine is called by the update process when the 
 *   'partial sequence number packet' timer goes off. There is a 
 *   separate timer for each interface.
 */
void isis_psnp (clns_pdbtype *pdb, idbtype *idb)
{
    hwaddrtype   snpa;
    int          level;
    isisidbtype  *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;
    if (!isisidb->isis_interface_up)
	return;

    memset(&snpa, 0, sizeof(snpa));
    snpa.type = STATION_ILLEGAL;
    snpa.length = 0;
    level = 0;

    if (!is_p2p(idb)) {

	/* 
	 * LAN - only send the psnp if we are NOT the designated router.
	 */
	if (!isis_we_are_dr(pdb, idb, CLNS_LEVEL1) &&
	    isisidb->isis_circuit_type & ISIS_CIRCUIT_L1) {
	    level |= ISIS_CIRCUIT_L1;
	}
	if (!isis_we_are_dr(pdb, idb, CLNS_LEVEL2) &&
	    isisidb->isis_circuit_type & ISIS_CIRCUIT_L2) {
	    level |= ISIS_CIRCUIT_L2;
	}
    } else {

	/* 
	 * Serial lines - send the appropriate snp. 
	 */
	if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L1) {
	    level |= ISIS_CIRCUIT_L1;
	} 
	if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L2) {
	    level |= ISIS_CIRCUIT_L2;
	}
    }

    level &= pdb->level;

    if (level & ISIS_CIRCUIT_L1) {
	snpa.target_type = TARGET_OSI_ALL_L1;
	isis_send_psnps(pdb, idb, ISIS_L1_PSNP, pdb->level1_LSPs, &snpa);
    } 
    if (level & ISIS_CIRCUIT_L2) {
	snpa.target_type = TARGET_OSI_ALL_L2;
	isis_send_psnps(pdb, idb, ISIS_L2_PSNP, pdb->level2_LSPs, &snpa);
    }

}


/*
 * isis_init_snp
 *
 */
static void isis_init_snp (clns_pdbtype *pdb, paktype *pak, long snp_type,
			   int header_len)
{
    isis_CSNP_hdrtype *csnp_pdu;

    csnp_pdu = isis_pak_start(pak, pdb);

    csnp_pdu->pid = NLP_ISIS;
    csnp_pdu->hlen = header_len;
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	csnp_pdu->version_pid = NLSP_VERSION;
    } else {
	csnp_pdu->version_pid = ISIS_VERSION;
    }
    csnp_pdu->id_length = 0;
    csnp_pdu->type = snp_type;
    csnp_pdu->version_pdu = ISIS_VERSION;
    csnp_pdu->max_area_addresses = 0;
    systemid_copy(pdb->systemId, csnp_pdu->source_id);
}


/* 
 * isis_insert_snp_entry
 *
 * Insert an SNP entry into a PSNP or CSNP packet.  This routine assumes
 * that there is already an item created, and that the item has sufficient
 * space for the entry.
 */
static void isis_insert_snp_entry(clns_pdbtype *pdb,
				  isis_snp_build_context *context,
				  isis_tree_type *element)
{
    isis_info_type *item;
    isis_lspdbtype   *lsp_db;
    isis_snp_entry *snp;
    ulong          lifetime;

    lsp_db = element->lsp;

    if (lsp_db->lsp_active) {
	lifetime = mgd_timer_left_sleeping(&lsp_db->lsp_timer) / ONESEC;
    } else {
	lifetime = 0;
    }

    /*
     * Build entry.
     */
    item = context->item;
    snp = (isis_snp_entry *) (item->value + item->length);
    PUTSHORT(snp->lifetime, lifetime);
    lspid_copy(&lsp_db->data->lspid, &snp->lspid);
    PUTLONG(&snp->sequence_number,GETLONG(&lsp_db->data->sequence_number));
    snp->checksum[0] = lsp_db->data->checksum[0];
    snp->checksum[1] = lsp_db->data->checksum[1];
    item->length += ISIS_SNP_ENTRY_LENGTH;
    context->current_length += ISIS_SNP_ENTRY_LENGTH;
}


/*
 * isis_increment_lspid
 *
 * Add 1 to an LSPID, with carry.
 */
static void isis_increment_lspid (isis_lspid *lspid)
{
    uchar *lsp_array;
    ulong current_byte;
    int byte_number;

    lsp_array = (uchar *) lspid;

    /* We could be fancy, but we'll just be boring. */

    byte_number = ISIS_LSPID_LENGTH - 1;
    while (byte_number >= 0) {
	current_byte = lsp_array[byte_number] + 1;
	lsp_array[byte_number] = current_byte; /* Truncate if we hit 0x100 */
	if (current_byte <= 0xff)	/* In range */
	    break;
	byte_number--;
    }
}

/*
 * isis_rw_send_csnp
 *
 * This routine is called at the bottom of recursive walk.
 * It will format and send as many csnp packets as are necessary.
 */
static boolean isis_rw_send_csnp (isis_tree_type *element,
				  void *context_parm, void *pdb_parm,
				  void *dummy1, void *dummy2)
{
    isis_CSNP_hdrtype *csnp;
    ulong snp_type;
    isis_snp_build_context *context;
    clns_pdbtype *pdb;

    context = context_parm;
    pdb = pdb_parm;

    context->packet_full = FALSE;

    /* Skip null placeholder LSPs. */

    if (isis_dummy_lsp(element->lsp))
	return(TRUE);

    snp_type = context->type;
    csnp = isis_pak_start(context->pak, pdb);

    /* If there's no item pointer, create one. */

    if (!context->item) {
	context->item = (isis_info_type *) 
	    (((uchar *) isis_pak_start(context->pak, pdb)) + 
	     context->current_length);
	context->item->code = ISIS_SNP;
	context->item->length = 0;
	context->current_length += ISIS_INFO_TYPE_OVERHEAD;
    }

    /*
     * Insert the information about the LSP into the CSNP.  We're guaranteed
     * to have enough room, since we check at the end.
     */
    isis_insert_snp_entry(pdb, context, element);

    /* If the current item is full, tie it off. */

    if (context->item->length >
	ISIS_INFO_TYPE_MAXDATA - ISIS_SNP_ENTRY_LENGTH) {
	context->item = NULL;
    }

    /* If the packet is full, bail out. */

    if (pdb->packet_mtu - context->current_length <
	ISIS_SNP_ENTRY_LENGTH + ISIS_INFO_TYPE_OVERHEAD) {

	/* Mark the end of the range carried by this CSNP. */

	lspid_copy(&element->lsp->data->lspid, &csnp->end_lsp);

	/* Flag that we've filled a packet. */

	context->packet_full = TRUE;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * isis_send_csnp
 *
 * This routine is called by the update process when the Complete
 * Sequence Number Packet timer expires.  There is a separate timer for each 
 * interface per level.  CSNPs are also sent during point-to-point link
 * initialization.
 *
 * This routine will send a single CSNP packet.  If the LSP database is big
 * enough to spill over into multiple CSNPs, we will be called back later.
 * The building_packets flag will be TRUE if we are in the middle of building
 * multiple packets.
 */
static boolean isis_send_csnp (clns_pdbtype *pdb, idbtype *idb,
			       isisidbtype *isisidb, int level)
{
    hwaddrtype snpa;
    int        serial;
    int        csnp_target;
    int        csnp_type;
    isis_tree_type *lsp_db;
    paktype           *pak;
    isis_CSNP_hdrtype *csnp;
    boolean	      result;
    isis_snp_build_context context;
    int level_ix;
    csnp_control *csnp_ctl;

    level_ix = level - 1;		/* Index by level */

    if (!isisidb->isis_interface_up)
	return(FALSE);

    serial = is_p2p(idb);
    csnp_ctl = &isisidb->csnp_ctl[level_ix];

    /* Don't do it if we're at the wrong level. */

    if (!(pdb->level & level) || !(isisidb->isis_circuit_type & level))
	return(FALSE);

    /* Don't do it if not DR or point-to-point. */

    if (!serial && !isis_we_are_dr(pdb, idb, level))
	return(FALSE);

    /* If there are no adjacencies at this level, skip it. */

    if (!isisidb->isis_adj_count[level_ix])
	return(FALSE);

    /* Don't do it if we're waiting for our initial DR holddown. */

    if (!serial && mgd_timer_running(&isisidb->isis_dr_timer))
	return(FALSE);

    /* All set to go with it.  Set up a bunch of junk. */

    memset(&snpa, 0, sizeof(snpa));
    snpa.type = STATION_ILLEGAL;
    snpa.length = 0;
    if (level == ISIS_CIRCUIT_L1) {
	csnp_target = TARGET_OSI_ALL_L1;
	csnp_type = ISIS_L1_CSNP;
	lsp_db = pdb->level1_LSPs;
    } else {
	csnp_target = TARGET_OSI_ALL_L2;
	csnp_type = ISIS_L2_CSNP;
	lsp_db = pdb->level2_LSPs;
    }
    
    /* Initialize the context block. */
    
    memset(&context, 0, sizeof(context));
    context.type = csnp_type;
    context.snpa = &snpa;

    /* Get a buffer.  Hang it up 'til next time if we can't get one. */

    pak = getbuffer(pdb->packet_mtu + pdb->packet_overhead);
    if (!pak)
	return(FALSE);
    context.pak = pak;
    csnp = isis_pak_start(context.pak, pdb);
    memset(csnp, 0, ISIS_CSNP_HEADER_SIZE);
    isis_init_snp(pdb, context.pak, csnp_type, ISIS_CSNP_HEADER_SIZE);

    /* If this is the first packet, initialize the start ID. */

    if (!csnp_ctl->building_packets) {
	memset(&csnp_ctl->next_snp_id, 0, ISIS_LSPID_LENGTH);
    }

    lspid_copy(&csnp_ctl->next_snp_id, &csnp->start_lsp);
    context.current_length = ISIS_CSNP_HEADER_SIZE;
    context.item = NULL;
    
    snpa.target_type = csnp_target;
    
    /*
     * Walk all of the LSPs, filling in the CSNP packet as we go.  We
     * get control back when either a packet fills, or we run out
     * of LSPs.  The packet_full flag tells us which.
     */
    isis_recursive_walk(&csnp_ctl->next_snp_id, ISIS_WALK_FIND_STARTID, lsp_db,
			isis_rw_send_csnp, &context, pdb, NULL, NULL);
	
    /*
     * Update the ID for the next SNP.  If at the end, overwrite the
     * packet end ID with all FFs.
     */
    if (context.packet_full) {
	lspid_copy(&csnp->end_lsp, &csnp_ctl->next_snp_id);
	isis_increment_lspid(&csnp_ctl->next_snp_id);
    } else {
	lspid_copy(&isis_infinite_lspid, &csnp->end_lsp);
    }
    
    PUTSHORT(csnp->pdu_length, context.current_length);
    update_pbuginf(pdb, "Sending L%d CSNP on %s",
		   csnp->type == ISIS_L1_CSNP ? 1 : 2, idb->namestring);
    result = isis_send_packet(pdb, pak, context.current_length,
			      idb, &snpa);
    if (result) {
	isis_traffic[pdb->proc_type][(csnp->type == ISIS_L1_CSNP) ? 
				     ISIS_L1_CSNP_OUT : 
				     ISIS_L2_CSNP_OUT]++;
	isisidb->isisidb_traffic[(csnp->type == ISIS_L1_CSNP) ?
				 ISIS_L1_CSNP_OUT :
				 ISIS_L2_CSNP_OUT]++;
    } else {
	if (!is_x25(idb->hwptr)) {
	    update_pbuginf(pdb, "Encapsulation failed for "
			   "CSNP on %s",
			   idb->namestring);
	}
    }
    return(context.packet_full);
}

/*
 * isis_csnp_timer_expired
 *
 * Called when a CSNP timer expires.
 *
 * An attempt is made to send a CSNP on the interface, and the timer is
 * reset.  The timeout value is based on whether we are in the middle of
 * a series of CSNPs or not;  if so, the timer is set to the interpacket
 * time, and if not, it is set to the CSNP interval.
 */
void isis_csnp_timer_expired (clns_pdbtype *pdb, idbtype *idb,
			      isisidbtype *isisidb, int level)
{
    boolean more_packets;
    csnp_control *csnp_ctl;

    /* Try to send a CSNP.  Reset the flag and timer as appropriate. */

    more_packets = isis_send_csnp(pdb, idb, isisidb, level);

    csnp_ctl = &isisidb->csnp_ctl[level - 1];
    csnp_ctl->building_packets = more_packets;
    if (more_packets) {			/* Not quite done yet */
	mgd_timer_start(&csnp_ctl->isis_csnp_timer,
			NLSP_MINIMUM_PACING_INTERVAL);
    } else {

	/*
	 * If there is a CSNP interval, restart the timer;  otherwise, shut
	 * it down.  Normally P2P interfaces have a zero interval (CSNPs
	 * are not normally sent) but mesh groups may configure nonzero
	 * CSNP intervals on P2P links for robustness.
	 */
	if (csnp_ctl->csnp_interval) {
	    mgd_timer_start_jittered(&csnp_ctl->isis_csnp_timer,
				     csnp_ctl->csnp_interval,
				     ISIS_JITTER_PERCENT);
	} else {				/* No interval */
	    mgd_timer_stop(&csnp_ctl->isis_csnp_timer);
	}
    }
}

/*
 * isis_rw_send_psnp
 *
 *    This routine is called at the bottom of recursive walk.
 *    It will format and send as many psnp packets as are necessary.
 *    Note that it is EXTREMELY similar to isis_rw_send_csnp.
 */
static boolean isis_rw_send_psnp (isis_tree_type *element,
				  void *idb_parm, void *context_parm,
				  void *pdb_parm, void *dummy)
{
    isis_PSNP_hdrtype *psnp;
    boolean	   result;
    ulong	   snp_type;
    isis_lspdbtype *lsp_db;
    isisidbtype    *isisidb;
    idbtype *idb;
    isis_snp_build_context *context;
    clns_pdbtype *pdb;

    idb = idb_parm;
    context = context_parm;
    pdb = pdb_parm;

    /* Bail if we don't owe an SNP on this LSP. */

    lsp_db = element->lsp;

    isisidb = get_isis_idb(pdb, idb);
    if (isisidb == NULL)
	return (FALSE);

    if (!(bitfield_check(isisidb->isis_if_number, &lsp_db->SSN)))
	return(TRUE);

    snp_type = context->type;

    update_pbuginf(pdb, "Build L%d PSNP entry for %s, seq %x",
		   snp_type == ISIS_L1_PSNP ? 1 : 2, 
		   lspid_string(&lsp_db->data->lspid), 
		   GETLONG(&lsp_db->data->sequence_number));

    /* If there's no current packet, allocate one. */

    if (!context->pak) {
        context->pak = getbuffer(pdb->packet_mtu + pdb->packet_overhead);
	if (!context->pak)
	    return (FALSE);

	psnp = isis_pak_start(context->pak, pdb);
	memset(psnp, 0, ISIS_CSNP_HEADER_SIZE);

	isis_init_snp(pdb, context->pak, snp_type, ISIS_PSNP_HEADER_SIZE);
	context->current_length = ISIS_PSNP_HEADER_SIZE;
	context->item = NULL;
    } else {
	psnp = isis_pak_start(context->pak, pdb);
    }

    /* If there's no item pointer, create one. */

    if (!context->item) {
	context->item = (isis_info_type *) 
	    (((uchar *) isis_pak_start(context->pak, pdb)) + 
	     context->current_length);
	context->item->code = ISIS_SNP;
	context->item->length = 0;
	context->current_length += ISIS_INFO_TYPE_OVERHEAD;
    }

    /*
     * Insert the information about the LSP into the PSNP.  We're guaranteed
     * to have enough room, since we check at the end.  Note that we've
     * sent it.
     */
    isis_insert_snp_entry(pdb, context, element);
    bitfield_clear(isisidb->isis_if_number, &(element->lsp->SSN));

    /* If the current item is full, tie it off. */

    if (context->item->length >
	ISIS_INFO_TYPE_MAXDATA - ISIS_SNP_ENTRY_LENGTH) {
	context->item = NULL;
    }

    /* If the packet is full, send it. */

    if (pdb->packet_mtu - context->current_length <
	ISIS_SNP_ENTRY_LENGTH + ISIS_INFO_TYPE_OVERHEAD) {

	PUTSHORT(psnp->pdu_length, context->current_length);
	update_pbuginf(pdb, "Sending L%d PSNP on %s",
		       snp_type == ISIS_L1_PSNP ? 1 : 2, idb->namestring);
	result = isis_send_packet(pdb, context->pak, context->current_length,
				  idb, context->snpa);
	context->pak = NULL;
	if (result) {
	    isis_traffic[pdb->proc_type][(snp_type == ISIS_L1_PSNP) ? 
					 ISIS_L1_PSNP_OUT : 
					 ISIS_L2_PSNP_OUT]++;
	    if (isisidb) {
		isisidb->isisidb_traffic[(snp_type == ISIS_L1_PSNP) ?
					 ISISIDB_L1_PSNP_OUT :
					 ISISIDB_L2_PSNP_OUT]++;
	    }
	} else {
	    if (!is_x25(idb->hwptr)) {
		update_pbuginf(pdb, "Encapsulation failed for PSNP on %s",
			       idb->namestring);
	    }
	}
    }
    return(TRUE);
}

/* 
 * isis_send_psnps
 */
void isis_send_psnps (clns_pdbtype *pdb, idbtype *idb,
		      long snp_type, isis_tree_type *tree_head,
		      hwaddrtype *snpa)
{
    paktype           *pak;
    isis_PSNP_hdrtype *psnp;
    boolean	      result;
    isis_snp_build_context context;

    /* Initialize the context block. */

    memset(&context, 0, sizeof(context));
    context.type = snp_type;
    context.snpa = snpa;

    /*
     * Walk all of the LSPs, filling in PSNP packets as we go.  We may
     * fill one up and start a new one along the way.
     */
    isis_recursive_walk(NULL, ISIS_WALK_ALL, tree_head, isis_rw_send_psnp,
			idb, &context, pdb, NULL);

    /* Finish up the last fragment. */

    pak = context.pak;
    if (pak) {
	psnp = isis_pak_start(pak, pdb);
	PUTSHORT(psnp->pdu_length, context.current_length);
	update_pbuginf(pdb, "Sending L%d PSNP on %s",
		       snp_type == ISIS_L1_PSNP ? 1 : 2, idb->namestring);
	result = isis_send_packet(pdb, pak, context.current_length,
				  idb, snpa);
	if (result) {
	    isis_traffic[pdb->proc_type][(snp_type == ISIS_L1_PSNP) ? 
					 ISIS_L1_PSNP_OUT : 
					 ISIS_L2_PSNP_OUT]++;
	} else {
	    if (!is_x25(idb->hwptr)) {
		update_pbuginf(pdb, "Encapsulation failed for PSNP on %s",
			       idb->namestring);
	    }
	}
    }
}

/*
 * isis_lsp_send_timer_expiry
 *
 * Called when an LSP send timer expires.  We basically look for another
 * LSP to send on this interface, and if there aren't any, we close up
 * shop on this interface.
 */
void isis_lsp_send_timer_expiry (clns_pdbtype *pdb, idbtype *idb,
				 isisidbtype *isisidb)
{
    isis_tree_type *tree;

    /*
     * If this is a serial line that is waiting for CSNPs to be exchanged,
     * restart the timer to wait for that event.
     */
    if (isis_wait_for_serial_csnp(pdb, idb, isisidb))
	return;

    /* Kill the timer. */

    mgd_timer_stop(&isisidb->isis_lsp_pacing_timer);

    /*
     * If this is the first time we're sending an LSP, clear the request
     * flag.
     */
    if (!isisidb->isis_sent_an_lsp) {	/* Just starting. */
	isisidb->isis_lsp_send_requested = FALSE;
    }

    /* Determine which tree we're looking in. */

    if (isisidb->last_lsp_level_sent == ISIS_CIRCUIT_L1)
	tree = pdb->level1_LSPs;
    else
	tree = pdb->level2_LSPs;

    /*
     * Increment the last LSPID sent to determine the next one.  If this
     * is the first time, it's set to all FFs, so the increment will
     * set it to all zeros.
     */
    isis_increment_lspid(&isisidb->last_lsp_sent);

    /* Do the recursive walk, looking for the start ID. */

    isis_recursive_walk(&isisidb->last_lsp_sent, ISIS_WALK_FIND_STARTID,
			tree, isis_send_one_lsp, pdb, idb,
			&isisidb->last_lsp_level_sent, NULL);

    /*
     * If the timer isn't running, it means that we didn't send anything.
     * If this is the case, we're done with this level.  Advance to the
     * next level.  If we're out of levels, we're all done;  however,
     * if the send_requested bit is set, it means that another LSP has
     * been marked for transmission, and we should start all over again.
     */
    if (!mgd_timer_running(&isisidb->isis_lsp_pacing_timer)) {

	if (isisidb->last_lsp_level_sent == ISIS_CIRCUIT_L2 ||
	    !(pdb->level & ISIS_CIRCUIT_L2)) { /* All levels done */

	    if (isisidb->isis_lsp_send_requested) { /* We need to go again */
		isis_initialize_lsp_transmission_fields(pdb, isisidb);
		mgd_timer_start(&isisidb->isis_lsp_pacing_timer,
				isisidb->pacing_interval);
	    }

	} else {			/* Need to do the next level (2) */
	    isisidb->last_lsp_level_sent = ISIS_CIRCUIT_L2;
	    lspid_copy(&isis_infinite_lspid, &isisidb->last_lsp_sent);
	    mgd_timer_start(&isisidb->isis_lsp_pacing_timer,
			    isisidb->pacing_interval);
	}
    }
}

/*
 * isis_manipulate_srm_mask
 *
 * Set or clear the pdb's SRM mask according to the interface state.
 */
void isis_manipulate_srm_mask (clns_pdbtype *pdb, isisidbtype *isisidb)
{
    /* Screen for bogus pointers or isisidb that has been released */
    if (!pdb || !isisidb || (isisidb->isis_if_number < 0)) {
	errmsg(&msgsym(BADIFNUM, CLNS), pdb, isisidb,
	       isisidb ? isisidb->isis_if_number : 0);
	return;
    }
    if (isisidb->isis_interface_up && isisidb->isis_running) {
	bitfield_set(isisidb->isis_if_number, &pdb->SRM_mask);
    } else {
	bitfield_clear(isisidb->isis_if_number, &pdb->SRM_mask);
    }
}

/*
 * isis_find_mesh_group
 *
 * Scan the PDB list looking for a particular mesh group.  Returns
 * a pointer, or NULL.
 */
static isis_mesh_group_entry *isis_find_mesh_group (clns_pdbtype *pdb,
						    ulong group_number)
{
    isis_mesh_group_entry *cur_mesh;

    cur_mesh = pdb->pdb_mesh_groups.qhead;
    while (cur_mesh) {
	if (cur_mesh->mesh_group_number == group_number)
	    break;
	cur_mesh = cur_mesh->next;
    }
    return(cur_mesh);
}

/*
 * isis_configure_mesh_group
 *
 * Add a mesh group to an interface.  Sets up all appropriate data
 * structures.  Returns FALSE if we ran out of memory.
 *
 * Note that any previous mesh group on the interface must be first
 * deconfigured.
 *
 * Mesh group zero is special;  that's the "blocked" mesh group, which
 * earns itself a special PDB pointer.
 */
boolean isis_configure_mesh_group (clns_pdbtype *pdb, isisidbtype *isisidb,
				   ulong group_number)
{
    isis_mesh_group_entry *mesh_entry;

    /* Find any existing mesh entry.  Create one if not there. */

    mesh_entry = isis_find_mesh_group(pdb, group_number);
    if (!mesh_entry) {
	mesh_entry = malloc_named(sizeof(isis_mesh_group_entry),
				  "ISIS mesh group entry");
	if (!mesh_entry)
	    return(FALSE);

	mesh_entry->mesh_group_number = group_number;
	enqueue(&pdb->pdb_mesh_groups, mesh_entry);
	if (group_number == 0)		/* "blocked" group */
	    pdb->mesh_group_blocked = mesh_entry;
    }

    /* OK, we've got an entry.  Set the bit for the interface. */

    bitfield_set(isisidb->isis_if_number, &mesh_entry->mesh_group_bits);
    mesh_entry->mesh_group_intf_count++;

    /* Set up the ISISIDB as well. */

    isisidb->isis_mesh_group = mesh_entry;
    return(TRUE);
}

/*
 * isis_deconfigure_mesh_group
 *
 * Remove a mesh group from an interface.  Clean up the detritus.
 */
void isis_deconfigure_mesh_group (clns_pdbtype *pdb, isisidbtype *isisidb)
{
    isis_mesh_group_entry *mesh_entry;

    /* Find the mesh entry. */

    mesh_entry = isisidb->isis_mesh_group;
    if (!mesh_entry)
	return;

    /* Clear the interface bit and drop the ref count. */

    bitfield_clear(isisidb->isis_if_number, &mesh_entry->mesh_group_bits);
    mesh_entry->mesh_group_intf_count--;
    isisidb->isis_mesh_group = NULL;

    /* If we removed the last reference, delete the group. */

    if (mesh_entry->mesh_group_intf_count == 0) {
	unqueue(&pdb->pdb_mesh_groups, mesh_entry);
	if (pdb->mesh_group_blocked == mesh_entry)
	    pdb->mesh_group_blocked = NULL;
	bitfield_destroy(&mesh_entry->mesh_group_bits);
	free(mesh_entry);
    }
}

/*
 * isis_display_mesh_groups
 */
void isis_display_mesh_groups (clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;
    idbtype      *idb;
    isisidbtype *isisidb;
    isis_mesh_group_entry *mesh_entry;
    isis_mesh_group_display *cur_entry, *new_entry, *next_entry, **prev_entry;

    /*
     * Walk the list of interfaces.  For each interface, find the
     * matching mesh group entry and hang the interface name from it.
     */
    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);
	mesh_entry = isisidb->isis_mesh_group;
	if (mesh_entry) {

	    /* Create a new entry. */

	    new_entry = malloc_named(sizeof(isis_mesh_group_display),
				     "ISIS mesh group display");
	    if (!new_entry)
		goto cleanup;
	    new_entry->intf_name = idb->namestring;

	    /* Skip to the end of the current list and add the new one. */

	    prev_entry = &mesh_entry->display_list;
	    while (*prev_entry) {
		prev_entry = &(*prev_entry)->next;
	    }
	    *prev_entry = new_entry;
	}
    }

    /* OK, the list is built.  Now display the mess. */

    for (mesh_entry = pdb->pdb_mesh_groups.qhead;  mesh_entry;
	 mesh_entry = mesh_entry->next) {
	printf("\n\nMesh group ");
	if (mesh_entry->mesh_group_number)
	    printf("%d:", mesh_entry->mesh_group_number);
	else
	    printf("(blocked)");
	for (cur_entry = mesh_entry->display_list;  cur_entry;
	     cur_entry = cur_entry->next) {
	    printf("\n  %s", cur_entry->intf_name);
	}
    }

  cleanup:

    /* Now flush out the memory. */

    for (mesh_entry = pdb->pdb_mesh_groups.qhead;  mesh_entry;
	 mesh_entry = mesh_entry->next) {
	cur_entry = mesh_entry->display_list;
	mesh_entry->display_list = NULL;
	while (cur_entry) {
	    next_entry = cur_entry->next;
	    free(cur_entry);
	    cur_entry = next_entry;
	}
    }
}
