/* $Id: at_aurp.c,v 3.8.18.8 1996/08/28 12:36:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_aurp.c,v $
 *------------------------------------------------------------------
 * AppleTalk Update-Based Routing Protocol
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Support routines for AURP
 *------------------------------------------------------------------
 * $Log: at_aurp.c,v $
 * Revision 3.8.18.8  1996/08/28  12:36:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.18.7  1996/07/10  22:17:43  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.18.6  1996/06/27  09:21:08  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.8.18.5  1996/06/21  16:12:52  kleung
 * CSCdi53648:  Low memory modified by TTY Background
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/06/05  00:45:04  kleung
 * CSCdi57957:  Unsolicited message should use errmsg facility
 * Branch: California_branch
 *
 * Revision 3.8.18.3  1996/05/02  08:39:09  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.8.18.2  1996/03/23  01:25:49  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.8.18.1  1996/03/18  18:51:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.4  1996/03/13  20:49:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups; changes from reviews
 *
 * Revision 3.7.12.3  1996/03/13  01:06:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.2  1996/03/07  08:28:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.12.1  1996/02/20  00:16:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/08  23:08:01  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.7  1995/12/21  02:03:13  hampton
 * CSCdi45946:  Change routine names in at_aurp.c
 *
 * Revision 3.6  1995/12/08  04:55:12  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.5  1995/11/20  22:18:34  kleung
 * CSCdi43654:  Garbage debug message displayed - PathEntry GC run
 * complete ...
 *
 * Revision 3.4  1995/11/20  22:00:08  kleung
 * CSCdi42895:  Code can cause a segment violation crash
 *
 * Revision 3.3  1995/11/17  08:41:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:49:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/07/27  07:30:29  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.5  1995/07/21 01:22:28  kleung
 * CSCdi37111:  ATIP remap entries not cleaned up when range overflows.
 *
 * Revision 2.4  1995/07/13  06:49:24  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.3  1995/06/30 20:39:12  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.2  1995/06/10  12:55:07  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.1  1995/06/07  20:07:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <string.h>
#include "mgd_timers.h"
#include "sys_registry.h"
#include "packet.h"
#include "../os/signal.h"
#include "interface_private.h"
#include "address.h"
#include "plural.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "../ip/tunnel.h"
#include "../ip/ip_registry.h"
#include "subsys.h"
#include "atalk_private.h"
#include "at_aurp.h"
#include "ataurp_debug.h"
#include "aurp_fsm.h"
#include "at_registry.h"
#include "../os/free.h"
#include "at_domain.h"

enum {
    ATTIMER_AURP_FSM,
    ATTIMER_AURP_EVENTS,
    ATTIMER_AURP_ZONES,
    ATTIMER_AURP_PATHGC,
    ATTIMER_AURP_PATHENTRY,
    ATTIMER_AURP_PATHAGER
};

/* Forward declarations */
static void 	  aurp_ScheduleZoneRequest(atalk_rdb_t *);
static void 	  aurp_SendQueries(void);
static int        aurp_SendNeighborQueries(neighborentry *n);
static void 	  aurp_SendQuery(atalk_rdb_t *);
static boolean 	  aurp_validate(aurpOps_t const *);
static void    	  aurp_classify_packet(aurpOps_t *);
static void       copy_di(di_t const *from, di_t *to);
static void       aurp_queueRIpkt(paktype *pak, at_aurp_info *aurp_info);
static void       setup_di(tunnel_info *, at_aurp_info *);

static void    	  ip_to_di(ipaddrtype, di_t *);
static void 	  handle_aurp_packet(aurpOps_t *);
static void 	  aurp_HandlePorts(void);
static boolean 	  aurp_aurp2ops(paktype *, aurpOps_t *);
static boolean 	  isequal_di(di_t const *, di_t const *);
static int 	  aurp_ActiveConnections(void);
static void 	  NDC_event(at_aurp_info *, ushort, ushort, uchar);
static void 	  aurp_FreePathList(aurp_pathentry *);
static boolean 	  aurp_vencap(paktype *, long);
static boolean 	  aurp_domain_encaps(paktype *, aurp_pkttype);
static void 	  aurp_process(void);
static void       ataurp_init(subsystype *);

void              ataurp_debug_init(void);
void              ataurp_parser_init(void);

/* Global variables */
static pid_t      aurp_pid;

static mgd_timer  aurp_timer;	/* Master timer */

static mgd_timer  aurp_sched_time; /* Next time to run fsms */

static mgd_timer  aurp_pathGCTime; /* aurp path GC Timer */

static ulong	  aurp_pathGCInterval; /* aurp path GC Interval */

static connid_t	  next_id;	/* Next connection id */

static rbTree *   pending_events_by_net; /* Database of pending updates
					  * keyed by net number 
					  */

static EventQueue pending_events_by_count; /* Database of pending updates
					    * sorted by order of entry 
					    */

static ulong 	  pending_events_count; /* Ordinal rank of updates */

static rbTree *   aurp_path_db;	/* Private database of all AURP paths */

mgd_timer         aurp_update_eventsTimer; /* Next time to send updates */

ulong 	          aurp_eventUpdateInterval; /* Interval between sending
					     *   updates
					     */

static mgd_timer  aurp_pathentryTimer; /* Next time to clear empty
				        * pathentries
					*/

static mgd_timer  aurp_pathAgerTime; /* Next time to age AURP paths */

static watched_queue
                 *atalkaurpQ;	/* AURP incoming packet queue */

static mgd_timer  aurp_zoneRequestTime; /* Next time to send zone requests */

static int const  kMaxFreePathEntryNodes = 25; /* Maximum allowed free
					        * pathentries
						*/
static gccontext_t *aurp_EventEntry_gccontext;
static gccontext_t *aurp_PathEntry_gccontext;

static int const  kMaxFreeEventEntryNodes = 25;

/* Lookup table for updating a pending event.  AURP requires that each
 * route can only have one pending event.  Table is used by indexing 
 * the row with the old event and the column with the new event to get
 * the new pending event.
 */
static EventCode const 
                  event_table[6][6] = 
{ { NULL_EVENT, NA_EVENT, NULL_EVENT, NULL_EVENT, NULL_EVENT, ZC_EVENT },
  { NA_EVENT, NA_EVENT, NULL_EVENT, NULL_EVENT, NA_EVENT, NA_EVENT },
  { ND_EVENT, NDC_EVENT, ND_EVENT, NRC_EVENT, ND_EVENT, ND_EVENT },
  { NRC_EVENT, NDC_EVENT, NRC_EVENT, NRC_EVENT, NRC_EVENT, NRC_EVENT },
  { NDC_EVENT, NDC_EVENT, ND_EVENT, NRC_EVENT, NDC_EVENT, NDC_EVENT },
  { ZC_EVENT, ZC_EVENT, ZC_EVENT, ZC_EVENT, ZC_EVENT, ZC_EVENT }
};


/* Used to determine whether an RI-Upd contains events for which our peer has
 * requested information.
 */
static ushort     const event_array[] = { 0, NA_SUI_MASK, ND_SUI_MASK, 
				          NRC_SUI_MASK, NDC_SUI_MASK, 
				          ZC_SUI_MASK };
    
#define event_Cast(_l_) 	(EventEntry *)(_l_)
#define pathentry_Cast(_l_) 	(aurp_pathentry *)(_l_)

/*
 * AppleTalk AURP-service subsystem header
 */
#define ATAURP_MAJVERSION  1
#define ATAURP_MINVERSION  0
#define ATAURP_EDITVERSION 1

SUBSYS_HEADER(ataurp, ATAURP_MAJVERSION, ATAURP_MINVERSION, ATAURP_EDITVERSION,
	      ataurp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, atalk", "req: iphost, tunnel, atalk");

/*
 * ataurp_init
 *
 * Initialize the AppleTalk AURP-service.
 */
static void
ataurp_init (subsystype *subsys)
{

    ataurp_parser_init();
    ataurp_debug_init();
    aurp_init();
}

/*******************************************************************
 * Tunnel module routines
 *******************************************************************/

/*
 * void aurp_cstate
 *
 * Called by tunnel_setmode to change state of tunnel.
 */
static void
aurp_cstate (idbtype *swidb, int enable)
{
    hwidbtype *   hwidb;
    atalkidbtype *idb;
    tunnel_info * tinfo;

    hwidb = hwidb_or_null(swidb);
    idb = atalk_getidb(swidb);
    tinfo = hwidb ? hwidb->tunnel : NULL;

    if (idb == NULL) {
	atalk_init_idb(swidb);
	if ((idb = atalk_getidb(swidb)) == NULL) {
	    printf("\n%% Cannot create data structures for atalk interface "
		   "information.");
	    return;
	}
    }
    
    if (enable) {
	
	/* Change checksum default */
	if (tinfo)
	    tinfo->checksum = tinfo->checksum_default = TRUE;

	/* Replace the generic tunnel encapsulator routines with
         * AURP-specific ones.
	 */
	hwidb->vencap = aurp_vencap;
	if (atalk_DomainRouter && idb->atalk_dminfo)
	    atdomain_InitialAdjust(idb);
	
    } else {
	
	/* Revert to non-AURP checksums */
	if (tinfo != NULL)
	    tinfo->checksum = tinfo->checksum_default = FALSE;

	/* Restore generic tunnel encapsulator routines. */
	hwidb->vencap = tunnel_vencap;
	if(atalk_DomainRouter && idb->atalk_dminfo)
	    atdomain_DisablePort(idb);
    }
}

/*
 * aurp_udp_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated.
 */
static long
aurp_udp_type2link (paktype *pak)
{
    if (pak)
	return LINK_APPLETALK;
    else
	return LINK_ILLEGAL;
}

static inline void
aurp_enqueue (paktype *pak)
{
    if (atalkif_InterfaceUp(atalk_getidb(pak->if_input)))
	process_enqueue(atalkaurpQ, pak);
    else
	atalk_dispose(ATALK_NOT_OPERATIONAL, pak);
}

/*
 * aurp_udp_encaps
 *
 * Called by the tunnel_oqueue routine.  This spoofs the output queue
 * on the tunnel interface.  Instead of putting the packet on the
 * output hold queue, build a new packet from scratch, create a UDP
 * header, and copy the existing packet data into the new packet after
 * the UDP header.  Caller is responsible for laying in IP header.
 *
 * We are not responsible for releasing the old packet -- our caller
 * will do that (caller may need old packet for statistics).
 */
static paktype *
aurp_udp_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype *     new_pak;
    udptype *     udp;
    unsigned	  bytes;	/* Size of packet after encapsulation. */

    if (pak->linktype != LINK_APPLETALK)
	return NULL;

    /* bytes = sizeof(ipheader + udpheader + packet) */
    bytes = tinfo->header->datagramsize + UDPHEADERBYTES + pak->datagramsize;
    new_pak = getbuffer(bytes);
    if (!new_pak)
	return NULL;

    udp = (udptype *) (ipheadstart(new_pak) + tinfo->header->datagramsize);
    udp->sport    = udp->dport = AURP_PORT;
    udp->length   = UDPHEADERBYTES + pak->datagramsize;
    
    /* Checksum will be calculated in tunnel_oqueue() */
    udp->checksum = 0;

    /* copy old packet into place -- pak_encap sets DG start & size */
    bcopy(pak->datagramstart, (uchar *) udp->udpdata, pak->datagramsize);

    new_pak->datagramsize = bytes;

    return new_pak;
}

/*
 * aurp_udp_decaps
 *
 * Decapsulate a packet passed down from IP processing.
 *
 * Determine which tunnel packet is destined for.  If packet is good,
 * enqueue the packet for native processing.  pak->info_start will
 * point to start of domain header, pak->network_start will point to
 * the first byte after the domain header which will either be the
 * start of the AURP header or the AppleTalk DDP header, depending on
 * the type of the packet. 
 */
static boolean
aurp_udp_decaps (paktype *pak, udptype *udp)
{
    tunnel_info * tinfo;
    uchar *       datastart;
    hwidbtype *   hwidb;
    idbtype *     idb;
    iphdrtype *   ip;
    
    ip = (iphdrtype *)ipheadstart(pak);

    if (udp->dport != AURP_PORT)
	return (FALSE);

    FOR_ALL_TUNNELS(tinfo)
	if ((tinfo->mode == TUN_MODE_AURP_UDP) &&
	    (ip->srcadr == tinfo->destination) &&
	    (tinfo->hwidb->state == IDBS_UP) &&
	    (tinfo->aurp && tinfo->aurp->enabled))
	    break;

    if (!tinfo || !tinfo->hwidb || !tinfo->aurp) {
	datagram_done(pak);
	return (TRUE);
    }

    if (tunnel_debug)
	buginf("\n%s: AURP/UDP to decaps %i->%i (len=%d ttl=%d)",
	       pak->if_input->namestring, ip->srcadr, ip->dstadr,
	       ip->tl, ip->ttl);

    /* strip off IP header and move pkt to datagramstart */
    datastart = ((uchar *)udp + UDPHEADERBYTES);

    pak->enctype       = ET_TUNNEL;
    pak->datagramstart = datastart + IP_DOMAINHEADER_SIZE;
    pak->datagramsize  = udp->length - UDPHEADERBYTES;

    pak->network_start = pak->datagramstart;
    pak->info_start    = datastart;

    if (tunnel_debug)
	buginf("\n%s: AURP decapsulated (len=%d)",
	       pak->if_input->namestring, pak->datagramsize);

    pak->flags &= ~PAK_MCICHECKED;
    hwidb = tinfo->hwidb;
    idb = hwidb->firstsw;
    if (change_if_input(pak, idb)) {
	hwidb->counters.inputs++;
	hwidb->counters.rx_cumbytes += pak->datagramsize;

	/* queue onto AURP queue */
	aurp_enqueue(pak);
    } else
	retbuffer(pak);
    return (TRUE);
}

/*
 * aurp_udp_setmode
 *
 * Provide tunnel initialization with information for the tinfo structure.
 */
static void
aurp_udp_setmode (tunnel_info *tinfo, linktype_t *getlink, int *enctype)
{
    tinfo->encaps = aurp_udp_encaps;
    tinfo->resync = (resync_t) return_nothing;
    tinfo->cstate = aurp_cstate;

    tinfo->transport  = TUN_TRANS_IP;
    tinfo->protocol   = UDP_PROT;
    tinfo->modestring = "AURP";

    *getlink = aurp_udp_type2link;
    *enctype = ET_TUNNEL;
}

/*
 * aurp_vencap
 *
 * Replaces generic tunnel_vencap routine.  Add AURP domain header.
 */
static boolean
aurp_vencap (paktype *pak, long address)
{
    idbtype *     idb;
    at_aurp_info *aurp_info;
    tunnel_info * tinfo;

    pak->enctype = ET_NULL;

    idb = pak->if_output;
    if (!idb)
	return (FALSE);

    tinfo = idb->hwptr->tunnel;

    if (!tinfo)
	return (FALSE);

    aurp_info = tinfo->aurp;

    if (!aurp_info)
	return (FALSE);

    /* Put domain encapsulation */
    if (!aurp_domain_encaps(pak, ATALK_DATA))
	return (FALSE);

#if UNIMPLEMENTED
    /* If we haven't heard from the data sender for a while, send a Tickle
     * packet before sending data.
     */
    if (ELAPSED_TIME(aurp_info->r_lastheard) > AURP_TICKLE_BEFORE_DATA_TIME
	&& atfsm_get_state(&aurp_info->receiver_fsm) == aurp_fsm_DR_Connected)
	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_TIMEOUTLHFT);
#endif

    return (TRUE);
}

/*
 * aurp_domain_encaps
 *
 * Add a domain header to a packet.
 */
static boolean
aurp_domain_encaps (paktype *pak, aurp_pkttype pkttype)
{
    ip_domainheader_t *
                  domain;
    at_aurp_info *aurp_info;
    hwidbtype *   hwidb;

    if (!pak || !pak->if_output)
	return (FALSE);

    hwidb = hwidb_or_null(pak->if_output);

    if (!hwidb || !hwidb->tunnel)
	return (FALSE);

    aurp_info = hwidb->tunnel->aurp;
	  
    if (!aurp_info)
	return (FALSE);

    switch (aurp_info->ditype) {

    case AURP_IP_DI:
	pak->datagramstart -= IP_DOMAINHEADER_SIZE;
	pak->datagramsize += IP_DOMAINHEADER_SIZE;
	domain = (ip_domainheader_t *)pak->datagramstart;
	copy_di(&aurp_info->remote_di, (di_t *)&domain->dest_di);
	copy_di(&aurp_info->local_di, (di_t *)&domain->src_di);
	PUTSHORT(&domain->version, AURP_VERSION);
	PUTSHORT(&domain->reserved, 0);
	PUTSHORT(&domain->pkttype, pkttype);
	break;

    case AURP_NULL_DI:
    default:
    }

    return (TRUE);
}

/*
 * aurp_aurp2ops
 *
 * Break out domain and aurp header information from an incoming aurp packet
 * into an 'ops' structure.
 */
static boolean
aurp_aurp2ops (paktype *pak, aurpOps_t *ops)
{
    uchar *       ptr;

    if (!pak || !ops || !pak->if_input || !pak->if_input->hwptr->tunnel
	|| !pak->if_input->hwptr->tunnel->aurp)
	return (FALSE);

    ptr = pak->info_start;		/* Should point at domain header */

    /* Remember packet */
    ops->pak = pak;

    /* Remember aurp info */
    ops->aurp = pak->if_input->hwptr->tunnel->aurp;

    /* Copy destination DI */
    copy_di((di_t *)ptr, &ops->dest_di);
    ptr += ops->dest_di.generic.length + 1;
    
    /* Copy source DI */
    copy_di((di_t *)ptr, &ops->src_di);
    ptr += ops->src_di.generic.length + 1;

    /* Copy version */
    ops->version = GETSHORT(ptr);
    ptr += sizeof(ushort);

    /* Skip reserved byte */
    ptr += sizeof(ushort);

    /* Copy packet type */
    ops->paktype = GETSHORT(ptr);

    return (TRUE);
}

/*
 * aurp_validate
 *
 * Validate the domain and aurp header fields.
 */
static boolean
aurp_validate (aurpOps_t const *ops)
{
    at_aurp_info *aurp_info;

    if (!ops || !ops->pak || !ops->pak->if_input
	|| !ops->pak->if_input->hwptr->tunnel)
	return (FALSE);

    aurp_info = ops->aurp;

    /* Check destination DI */
    if (!isequal_di(&ops->dest_di, &aurp_info->local_di))
	return (FALSE);

    /* Check version */
    if (ops->version != AURP_VERSION)
	return (FALSE);
    
    return (TRUE);
}

/*
 * aurp_classify_packet
 *
 * Determine packet type.  If it's an AppleTalk data packet, put on AppleTalk's
 * input queue.  Otherwise pass it to aurp's packet handler for processing.
 */
static void
aurp_classify_packet (aurpOps_t *ops)
{
    switch(ops->paktype) {
    case ATALK_DATA:
	raw_enqueue(hwidb_or_null(ops->pak->if_input), ops->pak);
	break;
    case AURP_DATA:
	handle_aurp_packet(ops);
	break;
    default:
	/* Error */
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     ops->pak->if_input))
	    buginf("\nAT: %s: Illegal AURP packet type %d",
		   ops->pak->if_input->namestring, ops->paktype);
	atalk_dispose(ATALK_MARTIAN, ops->pak);
	break;
    }
}

/*
 * aurp_pak_create
 *
 * Allocate a packet buffer to hold requested size plus aurp header.
 */
static paktype *
aurp_pak_create (int size)
{
    paktype *     pak;
    
    size += AURP_HEADER_SIZE;		/* Account for AURP header */

    pak = getbuffer(size);
    
    if (!pak) {
	return (NULL);
    }

    pak->linktype = LINK_APPLETALK;
    pak->datagramsize = size;

    return pak;
}

/*
 * aurp_next_sequence
 *
 * Return the next expected sequence number.  0 is not a valid sequence
 * number.
 */
static ushort
aurp_next_sequence (ushort seq)
{
    return ((seq == AURP_MAX_SEQUENCE) ? 1 : seq + 1);
}

#if 0					/* Unused function */
/*
 * aurp_prev_sequence
 *
 * Return the previous sequence number.  0 is not a valid sequence number.
 */
static ushort
aurp_prev_sequence (ushort seq)
{
    return ((seq == 1) ? AURP_MAX_SEQUENCE : seq - 1);
}
#endif

/*
 * aurp_check_sequence
 *
 * Verify if a sequence number is what we expected.
 */
static Seq_status
aurp_check_sequence (ushort reference, ushort comparison)
{
    Seq_status    ret_code;

    if (reference == comparison)
	return SEQOK;
    else if (reference == aurp_next_sequence(comparison))
	ret_code = SEQDELAYED;	/* Was previously ack'd sequence. */
    else if (comparison == aurp_next_sequence(reference))
	ret_code = SEQBADSYNC;	/* Skipped a sequence number. */
    else if ((short) (comparison - reference) > 0)
	ret_code = SEQBADSYNC;	/* Skipped more than one sequence number. */
    else
	ret_code = SEQIGNORE;	/* An old sequence number. */

    atalk_stat[ATALK_AURP_BAD_SEQ]++;
    return ret_code;
}

/*
 * aurp_next_connid
 *
 * Next connection id to use as a Data Receiver.
 */
connid_t
aurp_next_connid (void)
{
    next_id = (next_id == AURP_MAX_CONNECTION_ID) ? AURP_MIN_CONNECTION_ID
	: next_id + 1;
    return next_id;
}

/*
 * aurp_fill_header
 *
 * Fill an aurp header with the specified information.
 */
static void
aurp_fill_header (aurp_t *aurp, connid_t connid, ushort sequence, ushort cmd,
		  ushort flags)
{
    PUTSHORT(&aurp->connid, connid);
    PUTSHORT(&aurp->sequence, sequence);
    PUTSHORT(&aurp->command, cmd);
    PUTSHORT(&aurp->flags, flags);
}

/*
 * aurp_UpdateNeighborEntry
 *
 * Update the fields in a neighbor entry.
 */
void
aurp_UpdateNeighborEntry (at_aurp_info *aurp_info, neighborStatus status)
{
    sys_timestamp now;
    atalkidbtype *     idb;
    neighborentry *
                  n;

    if (!aurp_info)
	return;

    GET_TIMESTAMP(now);

    idb = aurp_info->idb;

    n = atmaint_FindNeighbor(0, idb);

    if (aurp_info->neighbor != n)
	aurp_info->neighbor = n;

    switch (status) {
    case neighborUp:
	if (!n) {
	    n = atmaint_CreateNeighbor(0, idb);
	    if (!n)
		return;
	    aurp_info->neighbor = n;
	} else {
	    COPY_TIMESTAMP(now, n->lastheard);
	    if (n->state == neighborDown)
		atmaint_RestartNeighbor(n, now);
	    n->state = neighborUp;
	}
	n->reachable_by |= ATALK_AURP_ENABLED;
	break;
    case neighborDown:
	if (n) {
	    n->state = neighborDown;
	    n->time_went_down = now;
	    n->reachable_by &= ~ATALK_AURP_ENABLED;
	}
	break;
    case neighborOverdue:
	if (n) {
	    n->state = neighborOverdue;
	}
	break;
    default:
	break;
    }
}

/*******************************************************************
 * Update events routines
 *******************************************************************/

/*
 * aurp_GetNextEvent
 *
 * Retrieve the next event from an RI-Upd packet.
 *
 */
static boolean
aurp_GetNextEvent (Event_tuple_t **tuple, int *nbytes, ushort *rngstart,
		   ushort *rngend, uchar *dist, EventCode *eventcode)
{
    uchar         t;

    if (*nbytes < EVENT_TUPLE_SIZE)
	return (FALSE);			/* No more events in packet */

    *eventcode = (*tuple)->event_code;	/* Get event type */

    if (*eventcode == NULL_EVENT) { /* Null Event */
	*tuple = (Event_tuple_t *) (((uchar *) *tuple) + 1);
	return (TRUE);
    }
    *rngstart = GETSHORT((*tuple)->net); /* Get starting range */
    t = (*tuple)->dist;			/* Get distance */
    *rngend = 0;
    *nbytes -= EVENT_TUPLE_SIZE;
    *tuple = (Event_tuple_t *) ((uchar *) (*tuple) + EVENT_TUPLE_SIZE);
    if (*rngstart >= ATALK_STARTUP_START) /* Fail if net is illegal */
	return (FALSE);
    if ((t & EXTENDED_TUPLE_MASK) == 0) { /* If net is not extended */
	*dist = t;
	return (TRUE);
    } else
	*dist = t & ~EXTENDED_TUPLE_MASK; /* Clear extended net bit */
					  /*     to get true distance */

    if (*nbytes < DDPNETBYTES)		/* Short update */
	return (FALSE);
    *rngend = GETSHORT(*tuple);
    *nbytes -= DDPNETBYTES;
    *tuple = (Event_tuple_t *) (((uchar *) *tuple) + DDPNETBYTES);
    if ((*rngstart <= *rngend) && (*rngend < ATALK_STARTUP_START))
	return (TRUE);

    return (FALSE);
}

/*
 * aurp_PutEventTuple
 *
 * Write an update event into an RI-Upd packet.
 */
static boolean
aurp_PutEventTuple (uchar **tuple, int *nbytes, EventCode type,
		    ushort rngstart, ushort rngend, uchar dist)
{
    if (type == NULL_EVENT)		/* Skip null events */
	return (FALSE);
    *(*tuple)++ = type;			/* Put event type. */
    PUTSHORT(*tuple, rngstart);		/* Put starting range */
    *tuple += DDPNETBYTES;
    if (type == ND_EVENT || type == NRC_EVENT) /* Put distance */
	**tuple = 0;
    else
	**tuple = dist;
    if (rngend) {
	**tuple |= EXTENDED_TUPLE_MASK;	/* Set extended net bit */
	(*tuple)++;
	PUTSHORT(*tuple, rngend);
	*tuple += DDPNETBYTES;
	*nbytes -= EXTENDED_EVENT_TUPLE_SIZE;
    } else {
	(*tuple)++;
	*nbytes -= NONEXTENDED_EVENT_TUPLE_SIZE;
    }
    return (TRUE);
}

/*
 * aurp_process_events
 *
 * Set up RI-Upds to be sent.  Postpone if a peer is requesting routing
 * information from us.
 */
static void
aurp_process_events (void)
{
    tunnel_info * tinfo;
    at_aurp_info *aurp_info;
    FSMFUNC       curstate;

    if (!atalk_running)
	return;

    if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	buginf("\nAT: Processing queued update events");

    FOR_ALL_AURP_TUNNELS(tinfo, aurp_info) {
	curstate = atfsm_get_state(&aurp_info->sender_fsm);
	if (curstate == aurp_fsm_DS_RI_Ack_Wait1
	    || curstate == aurp_fsm_DS_RI_Ack_Wait2
	    || curstate == aurp_fsm_DS_RI_Ack_Wait4) {
	    if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
		buginf("\nAT: Updates postponed");
	    return;		/* wait till this guy is finished */
	}
    }

    /* Build RI-Upds and queue onto private queues */
    FOR_ALL_AURP_TUNNELS(tinfo, aurp_info) {
	curstate = atfsm_get_state(&aurp_info->sender_fsm);
	if (curstate == aurp_fsm_DS_Connected
	    && aurp_info->s_informed) {

	    /* If there are packets to send */
	    if (aurp_Build_RIUpd(aurp_info)) {

		/* Notify fsm that timeout has occurred. */
		aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_TIMEOUTUPDATE);
	    }
	}
    }
}

/*
 * aurp_Build_RIUpd
 *
 * Build RI-Upds to be sent on a connection.
 */
boolean
aurp_Build_RIUpd (at_aurp_info *aurp_info)
{
    EventEntry *  e;
    EventQueue *  event;
    EventQueue *  nextevent;
    paktype *     pak;
    int           nbytes;
    uchar *       tuple;
    int           cnt;
    ushort        eventlist;
    atalkidbtype *     idb;
    atalk_rdb_t * route;
    zipnetentry * zn;
    rbTree *      tree;
    int           packets_built = 0;
    boolean       done = FALSE;
    uchar         hopcount;
    ushort        remap_start = 0;
    char const *  idb_name;
    
    tree = atroute_GetTree();

    if (aurp_info->pktqueue)
	aurp_FreeResources(aurp_info);

    idb = aurp_info->idb;

    idb_name = idb_get_namestring(idb->phys_idb);
    
    if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Building updates", idb_name);
    
    nextevent = (EventQueue *) pending_events_by_count.dqueue.flink;
    if (nextevent == &pending_events_by_count)
	done = TRUE;

    eventlist = 0;
    while (!done) {
	
	pak = aurp_pak_create(DDP_MAX_DATABYTES);
	if (!pak) {
	    if (packets_built == 0)
		return (FALSE);
	    else
		return (TRUE);
	}

	nbytes = pak->datagramsize - AURP_HEADER_SIZE;
	tuple = aurpdatastart(pak);
	cnt = 0;
	
	while (nextevent != &pending_events_by_count
	       && nbytes > EXTENDED_EVENT_TUPLE_SIZE ) {
	    event = nextevent;
	    nextevent = (EventQueue *) event->dqueue.flink;
	    
	    if (((long) (aurp_info->s_last_sent_update - event->count)) < 0) {
		
		e = event->event;
		
		/* Perform network number-based access filtering */
		if (idb->atalk_route_filter &&
		    !atalk_routecheck(e->dstrngstr, e->dstrngend,
				      idb->atalk_route_filter)) {
		    aurp_info->s_last_sent_update = event->count;
		}
		
		route = atroute_FindRoute(tree, e->dstrngstr, FALSE);
		if (route == NULL) {
		    if (e->eventtype != ND_EVENT) {
			aurp_info->s_last_sent_update = event->count;
			continue;
		    }
		} else {
		    
		    /* Don't send update if we're waiting for zone list */
		    if (route->zoneupdate) {
			if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK,
				     idb->phys_idb))
			    buginf("\nUpdate dropped to %#A because zone "
				   "list is being updated",
				   atalk_CableRange(route));
			aurp_info->s_last_sent_update = event->count;
			ReleaseRoute(route);
			continue;
		    }

		    zn = atalk_firstZone(route);
		    /* No updates if no zones */
		    if (zn == NULL && e->eventtype != ND_EVENT
			&& e->eventtype != NULL_EVENT
			&& e->eventtype != NRC_EVENT) {
			aurp_info->s_last_sent_update = event->count;
			if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK,
				     idb->phys_idb))
			    buginf("\nUpdate to %#A dropped because zone list "
				   "is NULL", atalk_CableRange(route));
			ReleaseRoute(route);
			continue;
		    }
		    
		    /* Perform zone-based access filtering */
		    if (idb->atalk_route_filter && e->eventtype != ND_EVENT
			&& e->eventtype != NULL_EVENT
			&& e->eventtype != NRC_EVENT) {
			if (atalk_permitPartialZones) {
			    while (zn != NULL) {
				if (atalk_rtmpzonecheck(zn->zone,
							idb->atalk_route_filter))
				    break;
				zn = atalkzn_nextZone(zn);
			    }
			    if (zn == NULL) {
				aurp_info->s_last_sent_update = event->count;
				ReleaseRoute(route);
				continue;
			    }
			} else {
			    while (zn != NULL) {
				if (!atalk_rtmpzonecheck(zn->zone,
							 idb->atalk_route_filter))
				    break;
				zn = atalkzn_nextZone(zn);
			    }
			    if (zn != NULL) {
				aurp_info->s_last_sent_update = event->count;
				ReleaseRoute(route);
				continue;
			    }
			}
		    }

		    /* Check for ZIP Reply filtering.  If all zones for this
		     * route are denied, then skip this route.  Prevents
		     * ZIP storms.
		     */
		    if (idb->atalk_zipreply_filter
			&& e->eventtype != ND_EVENT
			&& e->eventtype != NULL_EVENT
			&& e->eventtype != NRC_EVENT) {

			for (zn = atalk_firstZone(route); zn != NULL;
			     zn = atalkzn_nextZone(zn)) {

			    /* If at least one zone is permitted, we can
			     * stop checking.
			     */
			    if (atalk_zonecheck(zn->zone,
						idb->atalk_zipreply_filter))
				break;
			}
			if (zn == NULL) {
			    aurp_info->s_last_sent_update = event->count;
			    ReleaseRoute(route);
			    continue; /* All zones were denied, so skip. */
			}
			
		    }
		    
		}		    

		/* Keep track of event types we're sending */
		eventlist |= event_array[e->eventtype];
		
		hopcount = e->dist;

		
		if (atalk_DomainRouter_running) {

		    at_Domain *domain_src = NULL, *domain_dst = NULL;

		    if(route && route->rpath.idb) {

			domain_src = route->rpath.idb->atalk_dminfo;
			domain_dst = idb->atalk_dminfo;

                        if ((domain_dst || domain_src) &&
			    domain_dst == domain_src) {
                            ReleaseRoute(route);
                            continue;
			}

			if ((domain_src && domain_src->hop_rdctn) ||
			     (domain_dst && domain_dst->hop_rdctn)) {
			    if (hopcount != 0)
				hopcount = 1;
			}
		    }
		/* OK, and now let's create out remaps */
		    remap_start = 
			atdomain_GetRemapOut(idb,e->dstrngstr);
		    
		    if(remap_start) {
			if(e->dstrngend)
			    e->dstrngend = remap_start + 
				(e->dstrngend - e->dstrngstr);
			e->dstrngstr = remap_start;

		    }
		}
		
		if (aurp_PutEventTuple(&tuple, &nbytes, e->eventtype,
				       e->dstrngstr, e->dstrngend,
				       hopcount)) {
		    cnt++;
		    aurp_info->s_last_sent_update = event->count;
		}
		if (route)
		    ReleaseRoute(route);
	    }	
	}
	if (cnt) {
	    pak->datagramsize -= nbytes;
	    aurp_queueRIpkt(pak, aurp_info);
	    packets_built++;
	} else {
	    datagram_done(pak);
	    done = TRUE;
	}
	if (nextevent == &pending_events_by_count)
	    done = TRUE;
    }

    /* If RI-Upds were built and they contain events that peer requested,
     * return now.  Otherwise, destroy packets and return.
     */
    if (packets_built && (eventlist & aurp_info->s_sui)) {
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("\nAT: %s: Update%s queued",
		   idb_name, Plural(packets_built, "", "s"));
	return (TRUE);
    } else {
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("\nAT: %s: Null update suppressed", idb_name);
	aurp_FreeResources(aurp_info);
	return (FALSE);
    }
}

static boolean
aurp_NeedEventEntryGC (void)
{
    int           cnt;

    cnt = RBTreeReleasedNodeCount(pending_events_by_net);
    if (atalk_running)
	return (cnt > kMaxFreeEventEntryNodes);
    else
	return (cnt != 0);
}

static boolean
aurp_EventEntryGC (void)
{
    int           freed = 0;
    EventEntry *  e;

    while ((e = (EventEntry *) RBTreeGetFreeNode(pending_events_by_net))) {
	free(e);
	freed++;
	if (!aurp_NeedEventEntryGC())
	    break;
    }
    if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	buginf("\nAT: EventEntry GC run complete (%d entr%s freed)",
	       ArgAndPlural(freed, "y", "ies"));

    return (TRUE);
}

/*
 * aurp_gc_events
 *
 * Garbage collect update events that were sent.
 */
static void
aurp_gc_events (void)
{
    EventQueue *  e;
    EventQueue *  nexte;
    ulong         high;
    tunnel_info * tinfo;
    at_aurp_info *aurp_info;
    boolean       forcegc = FALSE;

    if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	buginf("\nAT: Garbage collecting update events queue");
    high = 0;

    /* If no tunnels are running, gc all queued events */
    if (aurp_ActiveConnections() == 0)
	forcegc = TRUE;

    FOR_ALL_AURP_TUNNELS(tinfo, aurp_info) {
	if (aurp_info->enabled && aurp_info->s_informed) {
	    if (aurp_info->s_last_sent_update == 0) {
		return;			/* Can't gc yet. */
	    }
	    if (high == 0)
		high = aurp_info->s_last_sent_update;
	    else if (((long) (high - aurp_info->s_last_sent_update)) >= 0)
		high = aurp_info->s_last_sent_update;
	}
    }

    e = (EventQueue *) pending_events_by_count.dqueue.flink;
    while (e != &pending_events_by_count) {
	nexte = (EventQueue *) e->dqueue.flink;
	if (!RBTreeNodeProtected(&e->event->link))
	    if ((((long) (high - e->count)) >= 0) || forcegc) {
                /* Free up all appropriate outbound remap entries */
                if (atalk_DomainRouter)
                    atdomain_DeletePathEntry(e->event->dstrngstr,
                        e->event->dstrngend);

		if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
		    buginf("\nAT: Deleting update event for %#A", 
			   atutil_CableRange(e->event->dstrngstr,
					     e->event->dstrngend));
		lw_remove(&e->dqueue);
		RBTreeDelete(pending_events_by_net, &e->event->link);
	    }
	e = nexte;
    }

    if (aurp_NeedEventEntryGC())
	atmaint_ScheduleGCRun(aurp_EventEntry_gccontext);
}

/*
 * aurp_FindEvent
 *
 * Look for a pending event in the database.
 */
static EventEntry *
aurp_FindEvent (ushort rngstart, ushort rngend)
{
    return (event_Cast(RBTreeIntSearch(pending_events_by_net,
				       IntervalKey(rngstart, rngend))));
}

static inline void
aurp_ProtectEvent (EventEntry *e)
{
    RBTreeNodeProtect(&e->link, TRUE);
}

static inline void
aurp_UnprotectEvent (EventEntry *e)
{
    RBTreeNodeProtect(&e->link, FALSE);
}

/* 
 * aurp_CreateEvent
 *
 * Create an event to be entered into the pending events database.
 */
static EventEntry *
aurp_CreateEvent (void)
{
    EventEntry *  e;

    e = event_Cast(RBTreeGetFreeNode(pending_events_by_net));
    if (e == NULL)
	e = malloc(sizeof(EventEntry));
    if (e == NULL)
	return (NULL);
    e->eqlink.event = e;
    e->eqlink.count = pending_events_count;
    if (++pending_events_count == 0)
	pending_events_count = 1;
    return (e);
}

/*
 * aurp_InsertEvent
 *
 * Insert a pending event into the pendings events database.  The includes
 * insertion into the sorted list and the tree.
 */
static boolean
aurp_InsertEvent (EventEntry *e)
{
    lw_insert(&e->eqlink.dqueue, pending_events_by_count.dqueue.blink);
    return (RBTreeIntInsert(e->dstrngstr, e->dstrngend, pending_events_by_net,
			    &e->link) != NULL);
}

/*
 * aurp_UpdatePendingEvent
 *
 * Update an existing pending event with a new event.  Lookup the final
 * event type as per p. 37.
 */
static void
aurp_UpdatePendingEvent (EventEntry *e, atalk_pdb_t const *p,
			 EventCode newevent)
{
    e->dist = atrtmp_GetPathMetric(p);
    e->idb = p->idb;
    e->eventtype = event_table[e->eventtype][newevent];
}

/*******************************************************************
 * Pathentry routines
 *******************************************************************/

static inline void
aurp_ProtectPathentry (aurp_pathentry *pathentry)
{
    RBTreeNodeProtect(&pathentry->link, TRUE);
}

static inline void
aurp_UnProtectPathentry (aurp_pathentry *pathentry)
{
    RBTreeNodeProtect(&pathentry->link, FALSE);
}

static inline aurp_pathentry *
KeepFirstPathentry (rbTree *tree)
{
    return ((aurp_pathentry *) atutil_KeepFirstEntry(tree));
}

static inline aurp_pathentry *
KeepNextPathentry (rbTree *tree, aurp_pathentry *pathentry)
{
    return ((aurp_pathentry *) atutil_KeepNextEntry(tree, &pathentry->link));
}

static inline EventEntry *
KeepFirstEvent (rbTree *tree)
{
    return ((EventEntry *) atutil_KeepFirstEntry(tree));
}

static inline EventEntry *
KeepNextEvent (rbTree *tree, EventEntry *event)
{
    return ((EventEntry *) atutil_KeepNextEntry(tree, &event->link));
}

/*
 * aurp_SetPathState
 *
 */
static boolean
aurp_SetPathState (atalk_pdb_t *p, at_state_t state)
{
    if (p == NULL || p->type != AT_PATH_AURP)
	return (FALSE);

    if (state >= at_state_bad)
	atroute_path_unredistribute(p);

    switch (state) {
    case at_state_good:
	p->state = state;
	TIMER_STOP(p->wentbad);
	GET_TIMESTAMP(p->timer);
	break;

    case at_state_bad:
	if (p->state  >= at_state_bad)
	    return (FALSE);
	p->state = state;
	GET_TIMESTAMP(p->wentbad);
	atrtmp_SetMetric(&p->metric, DDP_POISON_HOPCOUNT);
	atroute_NewPathMetric(p->rdb, p);
	break;
	
    default:
	break;
    }
    atroute_BestPath(p);
    return (TRUE);
}

/*
 * aurp_AddPathEntry
 *
 * Create a new path entry and enter it into the aurp path database.
 */
static aurp_pathentry *
aurp_AddPathEntry (ushort rngstart, ushort rngend)
{
    aurp_pathentry *
                  p;

    p = pathentry_Cast(RBTreeGetFreeNode(aurp_path_db));

    if (p == NULL) {
	p = malloc(sizeof(aurp_pathentry));
	if (p == NULL)
	    return NULL;
    }

    p->pathlist.dqueue.flink = &p->pathlist.dqueue;
    p->pathlist.dqueue.blink = &p->pathlist.dqueue;

    p->dstrngstr = rngstart;
    p->dstrngend = rngend;

    if (!RBTreeIntInsert(rngstart, rngend, aurp_path_db, &p->link)) {
	free(p);
	return NULL;
    }
    return p;
}

/*
 * aurp_needPathEntryGC
 *
 * Returns TRUE if garbage of freed path entries is needed.
 */
static boolean
aurp_needPathEntryGC (void)
{
    int           cnt;

    cnt = RBTreeReleasedNodeCount(aurp_path_db);
    if (atalk_running)
	return (cnt > kMaxFreePathEntryNodes);
    else
	return (cnt != 0);
}

/*
 * aurp_PathEntryGC
 *
 * Garbage collect freed path entries.
 */
static boolean
aurp_PathEntryGC (void)
{
    int           freed = 0;
    aurp_pathentry *
                  pathentry;

    while ((pathentry = pathentry_Cast(RBTreeGetFreeNode(aurp_path_db)))) {
	aurp_FreePathList(pathentry);
	free(pathentry);
	freed++;
	if (!aurp_needPathEntryGC())
	    break;
    }
    if (at_debug(atalkaurp_connection_debug, ATALK_NULL_MASK))
	buginf("\nAT: PathEntry GC run complete (%d pathentr%s freed)",
	       ArgAndPlural(freed, "y", "ies"));

    return (TRUE);
}

/*
 * aurp_DeletePathEntry
 *
 * Remove a path entry from the aurp path database.
 */
static void
aurp_DeletePathEntry (aurp_pathentry *pathentry)
{
    if (RBTreeNodeProtected(&pathentry->link))
	return;
    RBTreeDelete(aurp_path_db, &pathentry->link);
}

/*
 * aurp_NextPdb
 *
 * Return the next pdb in a list of pdbs.
 */
static inline aurp_pdb_t *
aurp_NextPdb (aurp_pdb_t const *p)
{
    return ((aurp_pdb_t *)p->dqueue.flink);
}

/*
 * aurp_PathListInsert
 *
 * Insert a pdb into a list of pdbs, sorted in ascending order by metric.
 */
static void
aurp_PathListInsert (aurp_pdb_t *queue_ptr, aurp_pdb_t *node_ptr)
{
    aurp_pdb_t *  cur;

    if (queue_ptr == NULL)
	return;

    cur = aurp_NextPdb(queue_ptr);
    while (cur != queue_ptr) {
	if (atroute_MetricCompare(&node_ptr->metric, &cur->metric,
				  ATALK_METRIC_LT))
	    break;
	cur = aurp_NextPdb(cur);
    }
    /* Insert in front of cur */
    lw_insert(&node_ptr->dqueue, cur->dqueue.blink);
}

/*
 * aurp_PathListRemove
 *
 * Remove a pdb from a list of pdbs.
 */
static inline void
aurp_PathListRemove (aurp_pdb_t *p)
{
    lw_remove(&p->dqueue);
}

/*
 * aurp_InitPdb
 *
 * Initialize a pdb, in particular setup the double-ended queue pointers.
 */
static inline void
aurp_InitPdb (aurp_pdb_t *p)
{
    dqueue_t *    ptr;

    ptr = &p->dqueue;

    ptr->flink = ptr;
    ptr->blink = ptr;
}

/*
 * aurp_PathListEmpty
 *
 * Return TRUE if a path entry's list of pdbs is empty.
 */
static inline boolean
aurp_PathListEmpty (aurp_pathentry const *pathentry)
{
    dqueue_t const *
                  dq = &pathentry->pathlist.dqueue;
    return (dq->flink == dq);
}

/*
 * aurp_AddNewBestPath
 *
 * Find the best path(s) for the given cable-range and inform the main routing
 * module.  Routing module will decide whether or not to accept it(them).
 * Returns true if module accepted one or more paths from us.
 */
static void
aurp_AddNewBestPath (ushort rngstart, ushort rngend)
{
    aurp_pathentry *
                  pathentry;
    aurp_pdb_t *  p;
    aurp_pdb_t *  nextp;
    rbTree *      path_tree;
    boolean       valid;
    aurp_pdb_t *  sentinel;
    atalk_pdb_t * path;

    pathentry = pathentry_Cast(RBTreeIntSearch(aurp_path_db,
					       IntervalKey(rngstart, rngend)));
    if (pathentry == NULL)
	return;

    path_tree = atroute_GetPTree(AT_PATH_AURP);

    sentinel = &pathentry->pathlist;
    p = aurp_NextPdb(sentinel);

    /* Inform routing module of all our best paths */
    while (p != sentinel) {
	
	nextp = aurp_NextPdb(p);

	if (atroute_MetricCompare(&pathentry->best_metric, &p->metric,
				  ATALK_METRIC_EQ)) {

	    /* First make sure he doesn't already know about it */
	    if (!atroute_GetMatchingPath(path_tree, pathentry->dstrngstr,
					 pathentry->dstrngend, p->idb, p->n,
					 &valid)) {
		path = atroute_AddPath(path_tree, p->idb, pathentry->dstrngstr,
				       pathentry->dstrngend, p->n,
				       atroute_DefaultPathAction, NULL, 0,
				       AT_PATH_AURP, &p->metric);
		if (path)
		    aurp_ScheduleZoneRequest(path->rdb);
	    }
	} else
	    break;		/* No need to check rest of paths. */
				/* They're worse. */
	
	p = nextp;
    }
}

static inline void
aurp_RemovePdb (aurp_pdb_t *p)
{
    aurp_pathentry *
                  pathentry;

    if (p == NULL)
	return;
    
    pathentry = p->pathentry;

    aurp_PathListRemove(p);
    pathentry->best_metric = aurp_NextPdb(&pathentry->pathlist)->metric;
}

/*
 * aurp_DeletePath
 * 
 * Delete a path from our private database.  Inform main routing module,
 * too.
 */
static void
aurp_DeletePath (aurp_pdb_t *p)
{
    atalk_pdb_t * q;
    aurp_pathentry *
                  pathentry;
    rbTree *      path_tree;
    boolean       valid;
    
    pathentry = p->pathentry;

    aurp_RemovePdb(p);

    path_tree = atroute_GetPTree(AT_PATH_AURP);

    q = atroute_GetMatchingPath(path_tree, pathentry->dstrngstr,
				pathentry->dstrngend, p->idb, p->n, &valid);

    if (q && valid) {
	aurp_SetPathState(q, at_state_bad);
    }
    
    free(p);
}

/*
 * aurp_FreePathList
 *
 * Unconditionally free a pathentry's list of pdbs.
 */
static void
aurp_FreePathList (aurp_pathentry *pathentry)
{
    while (!aurp_PathListEmpty(pathentry)) {
	free(aurp_NextPdb(&pathentry->pathlist));
    }
}

/*
 * aurp_ClearPathsFor
 *
 * Clear all the pdbs learned through the specified idb.
 */
void
aurp_ClearPathsFor (atalkidbtype *idb)
{
    aurp_pathentry *
                  pathentry;
    aurp_pathentry *
                  nextentry;
    aurp_pdb_t *  p;
    aurp_pdb_t *  sentinel;
    aurp_pdb_t *  nextp;

    pathentry = pathentry_Cast(RBTreeFirstNode(aurp_path_db));

    if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		 idb->phys_idb))
	buginf("\nAT: %s: Clearing paths",
	       idb_get_namestring(idb->phys_idb));

    while (pathentry != NULL) {
	nextentry = pathentry_Cast(RBTreeNextNode(&pathentry->link));
	sentinel = &pathentry->pathlist;
	p = aurp_NextPdb(sentinel);
	while (p != sentinel) {
	    nextp = aurp_NextPdb(p);
	    if (p->idb == idb) {
		aurp_DeletePath(p);	
		aurp_AddNewBestPath(pathentry->dstrngstr,
				    pathentry->dstrngend);
	    }
	    p = nextp;			
	}				
	pathentry = nextentry;
    }
}

aurp_pathentry *
aurp_FindPathentry (ushort rngstart, ushort rngend)
{
    if (!aurp_path_db)
        return(NULL);
    return (pathentry_Cast(RBTreeIntSearch(aurp_path_db,
					   IntervalKey(rngstart, rngend))));
}

static boolean
clear_emptyPathentries (treeLink *link, void *pdata)
{
    aurp_pathentry *
                  p = pathentry_Cast(link);

    if (aurp_PathListEmpty(p))
	aurp_DeletePathEntry(p);

    return (TRUE);
}

static void
pathentry_Ager (void)
{
    RBTreeForEachNode(clear_emptyPathentries, NULL, aurp_path_db, FALSE);

    if (aurp_needPathEntryGC())
	atmaint_ScheduleGCRun(aurp_PathEntry_gccontext);
}
/*
 * aurp_AddPath
 *
 * Add a pdb and, if necessary, a path entry to the aurp path database.
 */
static void
aurp_AddPath (atalkidbtype *idb, ushort rngstart, ushort rngend,
	      neighborentry *n, uchar dist)
{
    aurp_pathentry *
                  pathentry;
    aurp_pdb_t *  p;
    atalk_rdb_t * r;
    rbTree *      path_tree;
    rbTree *      route_tree;
    atalk_pdb_t * at_p;
    boolean       valid;

    pathentry = aurp_FindPathentry(rngstart, rngend);
    if (pathentry == NULL) {
	pathentry = aurp_AddPathEntry(rngstart, rngend);
	if (pathentry == NULL) {
	    errmsg(AT_ERR_NOMEM, "pathentry", __LINE__, __FILE__);
	    return;
	}
    } else if (rngstart != pathentry->dstrngstr ||
	       rngend != pathentry->dstrngend) {
	errmsg(AT_ERR_PATHREJECTED, atutil_CableRange(rngstart, rngend));
	return;
    }
    
    /* Check for conflicting routes in table */
    route_tree = atroute_GetTree();
    r = atroute_GetValidRoute(route_tree, rngstart, rngend, &valid, FALSE);
    if (!valid) {
	errmsg(AT_ERR_PATHREJECTED, atutil_CableRange(rngstart, rngend));
	return;
    }
        
    p = malloc(sizeof(aurp_pdb_t));
    if (p == NULL) {
	errmsg(AT_ERR_NOMEM, "aurp_pdb", __LINE__, __FILE__);
	return;
    }
    aurp_InitPdb(p);

    p->pathentry = pathentry;		/* Join pdb to path entry. */
    p->idb = idb;
    p->n = n;

    p->metric.type = AT_PATH_AURP;
    atrtmp_SetMetric(&p->metric, dist + 1);
    
    aurp_PathListInsert(&pathentry->pathlist, p); /* Join path entry to pdb. */
    pathentry->best_metric = aurp_NextPdb(&pathentry->pathlist)->metric;

    path_tree = atroute_GetPTree(AT_PATH_AURP);

    at_p = atroute_GetMatchingPath(path_tree, rngstart, rngend, idb, n,
				   &valid);

    /* If we find a path in the main routing module, it must be in the 
     * BAD state.  Otherwise, we would never be in this routine. 
     */
    if (valid) {
	if (at_p) {
	    atrtmp_SetMetric(&at_p->metric, dist + 1);
	    atroute_NewPathMetric(at_p->rdb, at_p);
	    aurp_SetPathState(at_p, at_state_good);
	} else {
	    at_p = atroute_AddPath(path_tree, idb, rngstart, rngend, n,
				   aurp_SetPathState, NULL, 0, AT_PATH_AURP,
				   &p->metric);
	}

	/* If main routing module accepted the path, schedule a zone
         * request. 
	 */
	if (at_p)
	    aurp_ScheduleZoneRequest(at_p->rdb);
    }
}

/*
 * aurp_GetMatchingPath
 *
 * Find the pdb with the matching cable range, idb and neighborentry.
 */
static aurp_pdb_t *
aurp_GetMatchingPath (ushort rngstart, ushort rngend, atalkidbtype *idb,
		      neighborentry *n)
{
   aurp_pathentry *
                 pathentry;
   aurp_pdb_t *  sentinel;
   aurp_pdb_t *  cur;

   pathentry = aurp_FindPathentry(rngstart, rngend);

   if (!pathentry)
       return NULL;

   sentinel = &pathentry->pathlist;
   cur = aurp_NextPdb(sentinel);

   while (cur != sentinel) {
       if (cur->idb == idb && cur->n == n)
	   return cur;
       else
	   cur = aurp_NextPdb(cur);
   }
   return NULL;
}

#if 0					/* Unused function */
aurp_pdb_t *
aurp_GetBestPath (ushort rngstart, ushort rngend)
{
    aurp_pathentry *
                  best_path;

    best_path = pathentry_Cast(RBTreeIntSearch(aurp_path_db,
					       IntervalKey(rngstart, rngend)));
    if (!best_path)
	return NULL;

    return (&best_path->pathlist);
}
#endif

static char const *
event_Str (EventCode code)
{
    switch (code) {
    case NULL_EVENT:	return ("NULL EVENT");
    case NA_EVENT:	return ("NA EVENT");
    case ND_EVENT:	return ("ND EVENT");
    case NRC_EVENT:	return ("NRC EVENT");
    case NDC_EVENT:	return ("NDC EVENT");
    case ZC_EVENT:	return ("ZC EVENT");
    default:		return ("UNKNOWN EVENT");
    }
}

/*
 * queue_event
 *
 * Queue an event for the specified pdb onto the pending events queue.
 */
static void
queue_event (atalk_pdb_t *p, EventCode code)
{
    EventEntry *  e;

    if (!pending_events_by_net || aurp_ActiveConnections() == 0 || !p)
	return;

    if (atrtmp_GetPathMetric(p) > DDP_MAX_HOPCOUNT)
	if (code == NA_EVENT || code == NDC_EVENT)
	    return;

    /* Don't send out routes that comes from a domain that contains
       any AURP tunnels to ensure 1) inter-domain routing only and
       2) route consistency among all AURP tunnels.
     */
    if (atalk_DomainRouter_running) {
        if (atdomain_MatchAnyAURPTunnel(p->idb))
           return;
        /* Don't send out routes that conflict with inbound ranges */
        if (atdomain_PathInAURPInbound(p))
           return;
        /* Lock up all appropriate outbound remap entries */
        atdomain_QueueEvent(p->idb,p->dstrngstr, p->dstrngend);
    }
    
    /* First, see if an event for this path is present */
    if ((e = aurp_FindEvent(p->dstrngstr, p->dstrngend)) != NULL) {
	aurp_ProtectEvent(e);

	if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Updating %s for %#A, was %s,", event_Str(code),
		   atutil_CableRange(p->dstrngstr, p->dstrngend),
		   event_Str(e->eventtype));

	aurp_UpdatePendingEvent(e, p, code);

	if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	    buginf(" now %s", event_Str(e->eventtype));

	aurp_UnprotectEvent(e);
    } else {
	if (at_debug(atalkaurp_update_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Creating %s for %#A",
		   event_Str(code), atutil_CableRange(p->dstrngstr,
						      p->dstrngend));
	/* If not present, get an EventEntry, fill it in, and insert it. */
	e = aurp_CreateEvent();
	if (e == NULL)
	    return;
	e->dstrngstr = p->dstrngstr;
	e->dstrngend = p->dstrngend;
	e->dist = atrtmp_GetPathMetric(p);
	e->idb = p->idb;
	e->eventtype = code;
	aurp_InsertEvent(e);
    }
}

/*
 * NA_event
 *
 * Process a Net Add event.
 */
static void
NA_event (at_aurp_info *aurp_info, ushort rngstart, ushort rngend, uchar dist)
{
    aurp_pdb_t *  p;
    rbTree *      path_tree;
    atalkidbtype *     idb;
    neighborentry *
                  n;
    atalk_rdb_t * r;
    rbTree *      r_tree;
    EventEntry *  e;
    
    at_RemapTable *remap;


    if (!aurp_info)
	return;

    idb = aurp_info->idb;
    n = aurp_info->neighbor;


    /* Put domain stuff right here */
    if(idb->atalk_dminfo) {
	remap = atdomain_CreateRemap(idb,rngstart, rngend);
	if(remap) {
	    rngstart = remap->remap_start;
	    if(rngend)
		rngend   = remap->remap_end;
	}
    }
    path_tree = atroute_GetPTree(AT_PATH_AURP);

    if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Net Add event for %#A, %d hops",
	       idb_get_namestring(idb->phys_idb),
	       atutil_CableRange(rngstart, rngend), dist);

    p = aurp_GetMatchingPath(rngstart, rngend, idb, n);

    if (p) {
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("...treating as Net Distance Change event");
	NDC_event(aurp_info, rngstart, rngend, dist); /* AURP spec.  p.46 */
    } else {
	if (dist < DDP_MAX_HOPCOUNT) {
	    aurp_AddPath(idb, rngstart, rngend, n, dist);
	
	    r_tree = atroute_GetTree();
	    r = atroute_LookupRoute(r_tree, rngstart, FALSE);
	    if (r && r->pathcount > 0 && r->rpath.type == AT_PATH_AURP &&
		r->rpath.state == at_state_good) {
		e = aurp_FindEvent(r->dstrngstr, r->dstrngend);
		if (e && e->eventtype == ND_EVENT) {
		    /* Ok, this means that the route just changed to
		     * a tunnel.
		     */
		    aurp_ProtectEvent(e);
		    aurp_UpdatePendingEvent(e, r->best_path, NRC_EVENT);
		    aurp_UnprotectEvent(e);
		}
	    }
	} else if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
			    idb->phys_idb))
	    buginf("...ignoring");
    }
}

/*
 * aurp_AgePath
 *
 * Age a path.
 */
static boolean
aurp_AgePath (treeLink *link, void *pdata)
{
    atalk_pdb_t * p = (atalk_pdb_t *) link;

    if (p == NULL)
	return (FALSE);
    if (TIMER_RUNNING(p->wentbad) &&
	CLOCK_OUTSIDE_INTERVAL(p->wentbad, 2 * atalk_rtmpValidTime))
	atroute_DeletePath(p);
    return (TRUE);
}
    
/*
 * aurp_pathAger
 *
 * For each path, call path ager routine.
 */
static void
aurp_pathAger (void)
{
    rbTree *      p_tree;

    p_tree = atroute_GetPTree(AT_PATH_AURP);
    if (p_tree)
	RBTreeForEachNode(aurp_AgePath, NULL, p_tree, FALSE);
}

/*
 * ND_event
 *
 * Process a Net Down event.
 */
static void
ND_event (at_aurp_info *aurp_info, ushort rngstart, ushort rngend)
{
    aurp_pdb_t *  q;
    atalkidbtype *     idb;
    neighborentry *
                  n;
    atalk_rdb_t * r;
    rbTree *      r_tree;
    EventEntry *  e;
    at_RemapTable *remap = NULL;
    at_SubDomain  *sd;

    idb = aurp_info->idb;
    n = aurp_info->neighbor;

    if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Net Down event for %#A",
	       idb_get_namestring(idb->phys_idb),
	       atutil_CableRange(rngstart, rngend));

    if(idb->atalk_dminfo) {
	remap = atdomain_CreateRemap(idb,rngstart, rngend);
	if(remap) {
	    rngstart = remap->remap_start;
	    if(rngend)
		rngend   = remap->remap_end;
	}
    }

    q = aurp_GetMatchingPath(rngstart, rngend, idb, n);
    if (q) {
	aurp_DeletePath(q);
	aurp_AddNewBestPath(rngstart, rngend);

	/* Check if NRC_EVENT is pending.  If so, see if we need to 
	 * change it to a ND_EVENT.
	 */
	r_tree = atroute_GetTree();
	r = atroute_LookupRoute(r_tree, rngstart, FALSE);
	if (!r || r->pathcount == 0 || r->rpath.type != AT_PATH_AURP ||
	    r->rpath.state >= at_state_bad) {
	    /* Ok, this means that network is no longer accessible through
	     * a tunnel.
	     */
	    e = aurp_FindEvent(r->dstrngstr, r->dstrngend);
	    if (e && e->eventtype == NRC_EVENT) {
		aurp_ProtectEvent(e);
		e->eventtype = ND_EVENT;
		aurp_UnprotectEvent(e);
	    }
	}
    } else {
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("...no path in database");
        /* Remove remap if path is not in database */
        if (idb->atalk_dminfo && remap) {
            sd = idb->atalk_dminfo->in;
            if (atdomain_DeleteRemap(sd,remap, FALSE)) {
	        if (sd->RemapListHdr != NULL)
		    atdomain_UnLockAvCell(sd,rngstart,rngend);
            }
	}
    }
}

/*
 * NRC_event
 *
 * Process a Net Route Change.
 */
static void
NRC_event (at_aurp_info *aurp_info, ushort rngstart, ushort rngend)
{
    atalkidbtype *     idb;

    idb = aurp_info->idb;

    if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Net Route Change event for %#A...treating as "
	       "Net Down", idb_get_namestring(idb->phys_idb),
	       atutil_CableRange(rngstart, rngend));
    ND_event(aurp_info, rngstart, rngend); /* AURP spec.  p.46 */
}

/*
 * NDC_event
 *
 * Process a Net Distance Change event.
 */
static void
NDC_event (at_aurp_info *aurp_info, ushort rngstart, ushort rngend, uchar dist)
{
    aurp_pdb_t *  p;
    atalk_pdb_t * q;
    rbTree *      path_tree;
    atalkidbtype *     idb;
    boolean       valid;
    neighborentry *
                  n;
    aurp_pathentry *
                  pathentry;
    aurp_pdb_t *  sentinel;
    at_RemapTable  *remap;

    if (!aurp_info)
	return;

    idb = aurp_info->idb;
    n = aurp_info->neighbor;

    if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Net Distance Change event for %#A, %d hops",
	       idb_get_namestring(idb->phys_idb),
	       atutil_CableRange(rngstart, rngend), dist);

    /* Put domain stuff right here */
    if(idb->atalk_dminfo) {
	remap = atdomain_CreateRemap(idb,rngstart, rngend);
	if(remap) {
	    rngstart = remap->remap_start;
	    if(rngend)
		rngend   = remap->remap_end;
	}
    }
    
    if (dist == DDP_MAX_HOPCOUNT) {	/* AURP spec.  p.47 */
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("...treating as Net Down");
	ND_event(aurp_info, rngstart, rngend);
	return;
    }

    path_tree = atroute_GetPTree(AT_PATH_AURP);

    p = aurp_GetMatchingPath(rngstart, rngend, idb, n);
    if (p) {
	atrtmp_SetMetric(&p->metric, dist + 1);

	/* now reinsert */
	pathentry = p->pathentry;
	sentinel = &pathentry->pathlist;
	aurp_PathListRemove(p);
	aurp_PathListInsert(sentinel, p);
	pathentry->best_metric = aurp_NextPdb(sentinel)->metric;

	/* See if main routing module has copy of this path.  We need
	 * to update that path, too.
	 */
	q = atroute_GetMatchingPath(path_tree, rngstart, rngend, idb, n,
				    &valid);
	if (valid && q) {
	    atrtmp_SetMetric(&q->metric, dist + 1);
	    GET_TIMESTAMP(q->timer); /* Path was updated.  Make note. */

	    /* Inform main routing module of change in metric */
	    atroute_NewPathMetric(q->rdb, q);
	}

	/* Change in metric may have caused other paths in private db
	 * to become better than current path.  Inform main routing module
	 * about these paths.
	 */
	aurp_AddNewBestPath(rngstart, rngend);

    } else {
	if (at_debug(atalkaurp_update_debug, ATALK_INTERFACE_MASK,
		     idb->phys_idb))
	    buginf("...treating as Net Add event");
	NA_event(aurp_info, rngstart, rngend, dist);
    }

}

/*
 * aurp_GetNextRoute
 *
 * Return the next route in the main routing table that isn't an AURP path.
 */
static atalk_rdb_t *
aurp_GetNextRoute (rbTree *tree, atalkidbtype *idb, atalk_rdb_t *prev)
{
    atalk_rdb_t * r;
    boolean       connected = FALSE;
    atalkidbtype *     r_idb;

    if (idb == NULL)
	return (NULL);

    for (r = KeepNextRoute(tree, prev); r != NULL;
	 r = KeepNextRoute(tree, r)) {
	connected = atalk_net_connected(r);
	r_idb = r->rpath.idb;
	if (idb != r_idb) {
	    if (connected && (!rtmp_NetIsValid(r)))
		continue;
	    if (r->rpath.type == AT_PATH_AURP)
		continue;
	    return (r);
	}
    }
    return (NULL);
}

/*
 * aurp_FreeResources
 *
 * Free any packets on the connection's private packet queue.
 */
void
aurp_FreeResources (at_aurp_info *aurp_info)
{
    paktype *     pak;
    paktype *     nextpak;

    pak = aurp_info->pktqueue;

    while (pak != NULL) {
	nextpak = pak->next;
	pak->next = NULL;
	datagram_done(pak);
	pak = nextpak;
    }
    aurp_info->pktqueue = NULL;
}

/*
 * aurp_PutTuple
 *
 * Write a routing tuple to tuple.  Returns number of bytes written.
 */
static int
aurp_PutTuple (atalk_rdb_t *r, rtmptuple *tuple, atalkidbtype *idb_out)
{
    uchar hopcount = atrtmp_GetRouteMetric(r);
    ushort remap_start = 0;

    if (atalk_DomainRouter_running) {
	at_Domain *domain_src = NULL, *domain_dst = NULL;

	if(r->rpath.idb)
	    domain_src = r->rpath.idb->atalk_dminfo;
	domain_dst = idb_out->atalk_dminfo;
	if (((domain_src && domain_src->hop_rdctn) ||
	     (domain_dst && domain_dst->hop_rdctn)) &&
	      domain_src != domain_dst) {

	    if (hopcount != 0)
		hopcount = 1;
	}
	/* Now let's do out remaps */
	remap_start = atdomain_GetRemapOut(idb_out,r->dstrngstr);

    }
    
    if (atalk_net_extended(r)) {
	if(remap_start)
	    PUTSHORT(tuple->net, remap_start);
	else
	    PUTSHORT(tuple->net, r->dstrngstr);
	tuple->dist = RTMPTUPLERANGE | hopcount;
	tuple = (rtmptuple *) (((uchar *) tuple) + RTMPTUPLEBYTES);
	if(remap_start)
	    PUTSHORT(tuple->net, remap_start + (r->dstrngend - r->dstrngstr));
	else
	    PUTSHORT(tuple->net, r->dstrngend);
	tuple->dist = AURPTUPLEVERSION;
	return (2*RTMPTUPLEBYTES);
    } else {
	if(remap_start)
	    PUTSHORT(tuple->net, remap_start);
	else
	    PUTSHORT(tuple->net, r->dstrngstr);

	tuple->dist = hopcount;
	return (RTMPTUPLEBYTES);
    }
}

/*
 * aurp_Build_RIRsps
 *
 * Builds RI-Rsp packets to be sent to peer
 */
void
aurp_Build_RIRsps (at_aurp_info *aurp_info)
{
    rbTree *      tree;
    boolean       done = FALSE;
    paktype *     pak;
    int           nbytes;		/* Number of bytes for tuples. */
    atalk_rdb_t * route;
    rtmptuple *   tuple;
    zipnetentry * zn;
    int           rtcnt;
    int           pkt;
    atalkidbtype *     idb;
    EventEntry *  event;
    int           offset;

    tree = atroute_GetTree();
    pkt = 0;
    route = NULL;
    idb = aurp_info->idb;
    rtcnt = 0;

    if (aurp_info->pktqueue)
	aurp_FreeResources(aurp_info);

    while (!done) {
	pak = aurp_pak_create(DDP_MAX_DATABYTES);

	if (pak == NULL) {
	    return;
	}

	nbytes = pak->datagramsize - AURP_HEADER_SIZE;
	tuple = (rtmptuple *) aurpdatastart(pak);

	while (nbytes >= RTMPTUPLEBYTES * 2) {
	    route = aurp_GetNextRoute(tree, idb, route);

	    if (route == NULL) {
		done = TRUE;
		break;
	    }

            /* Don't send out routes that comes from a domain that contains
               any AURP tunnels to ensure 1) inter-domain routing only and
               2) route consistency among all AURP tunnels.
             */
            if (atalk_DomainRouter_running) {
                if (atdomain_MatchAnyAURPTunnel(route->rpath.idb))
                    continue;
                /* Don't send out routes that conflict with inbound ranges */
                if (atdomain_PathInAURPInbound(route->best_path))
                    continue;
            }
    
	    event = aurp_FindEvent(route->dstrngstr, route->dstrngend);

	    /* Don't send routes that have pending updates unless those
	     * updates are Net Distance Change events.
	     */
	    if (event && event->eventtype != NDC_EVENT)
		continue;

	    /* Check for access control. */
	    if (idb->atalk_route_filter && 
		!atalk_routecheck(route->dstrngstr, route->dstrngend, 
				  idb->atalk_route_filter))
	        continue;

	    /* Zonelist is being updated.  Skip. */
	    if (route->zoneupdate)
		continue;

	    zn = atalk_firstZone(route);

	    /* No zonelist?  Skip if atalk_routeZones is enabled (default). */
	    if ((zn == NULL) && atalk_routeZones)
		continue;

	    if (idb->atalk_route_filter) {
		while (zn != NULL) {
		    if (!atalk_rtmpzonecheck(zn->zone,idb->atalk_route_filter))
			break;
		    zn = atalkzn_nextZone(zn);
		}
		if (zn != NULL)
		    continue;
	    }

	    /* Check for ZIP Reply filtering.  If all zones for this route
	     * are denied, then skip this route.  Prevents ZIP storms.
	     */
	    if (idb->atalk_zipreply_filter) {

		for (zn = atalk_firstZone(route); zn != NULL;
		     zn = atalkzn_nextZone(zn)) {

		    /* If at least one zone is permitted, we can stop
		     * checking.
		     */
		    if (atalk_zonecheck(zn->zone, idb->atalk_zipreply_filter))
			break;
		}
		if (zn == NULL)
		    continue;	/* All zones were denied, so skip. */
	    }
	    
	    /* Don't send routes with illegal hop counts. */
	    if (route->best_path
		&& atrtmp_GetPathMetric(route->best_path) > DDP_MAX_HOPCOUNT)
		continue;

	    rtcnt++;
	    
	    offset = aurp_PutTuple(route, tuple, idb);

	    nbytes -= offset;
	    tuple = (rtmptuple *) (((uchar *) tuple) + offset);
	}
	if (rtcnt || (pkt == 0)) {
	    pak->datagramsize -= nbytes;
	    aurp_queueRIpkt(pak, aurp_info);
	    pkt++;
	} else {
	    datagram_done(pak);
	}
    }
}

/*******************************************************************
 * Packet Send Routines
 *******************************************************************/

/*
 * aurp_send
 *
 * Take an AURP packet, add a domain header, and send it on its way.
 * AppleTalk data packets do not use this routine.
 */
static boolean
aurp_send (paktype *pak, atalkidbtype *atalkidb)
{
    idbtype *idb = atalkidb->phys_idb;
    
    if (!idb || !interface_up(idb)) {
	atalk_dispose(ATALK_NOROUTE, pak);    
	return (FALSE);
    }

    pak->if_output = idb;

    if (aurp_domain_encaps(pak, AURP_DATA)) {
	datagram_out(pak);
	return (TRUE);
    } else {
	idb->hwptr->atalk_outputerrs++;
	atalk_dispose(ATALK_ENCAPSFAILED, pak);
	return (FALSE);
    }
}

/*
 * aurp_Send_nullRI
 *
 * Send a null Routing Information Update packet.  Used by data senders
 * to check for the existence of their peers.
 */
void
aurp_Send_nullRI (at_aurp_info *aurp_info)
{
    paktype *     pak;

    pak = aurp_pak_create(0);	/* Null RIs have no data. */

    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->s_connid,
		     aurp_info->s_sequence, RI_UPD, 0);
    if (aurp_send(pak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_RI_OUT]++;
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Null RI-Upd sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
    }
}

/*
 * aurp_Send_RI_Upd
 *
 * Send a Routing Information Update packet.
 */
void
aurp_Send_RI_Upd (at_aurp_info *aurp_info)
{
    paktype *     pak;
    paktype *     newpak;

    pak = aurp_info->pktqueue;

    if (!pak)
	return;

    newpak = pak_duplicate(pak);
    if (!newpak)
        return;

    newpak->next = NULL;

    aurp_fill_header(aurphdrtype_start(newpak), aurp_info->s_connid,
		     aurp_info->s_sequence, RI_UPD, 0);

    if (aurp_send(newpak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_RI_OUT]++;
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: RI-Upd sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
    }
}

/*
 * aurp_Send_RI_Rsp
 * 
 * Send a Routing Information Response packet.
 */
void
aurp_Send_RI_Rsp (at_aurp_info *aurp_info)
{
    paktype *     pak;
    paktype *     newpak;
    boolean       last = FALSE;
    
    pak = aurp_info->pktqueue;

    if (!pak)
	return;

    newpak = pak_duplicate(pak);

    if (!newpak)
	return;

    if (pak->next == NULL)	/* Is this last RI-Rsp packet? */
	last = TRUE;

    newpak->next = NULL;

    aurp_fill_header(aurphdrtype_start(newpak), aurp_info->s_connid,
		     aurp_info->s_sequence, RI_RSP, last ? LASTFLAG_MASK : 0);

    if (aurp_send(newpak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_RI_OUT]++;
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: RI-Rsp sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
    }

}

/*
 * aurp_Send_Open_Req
 *
 * Send an Open Request packet.
 */
void
aurp_Send_Open_Req (at_aurp_info *aurp_info)
{
    paktype *     pak;
    Open_Req_t *  aurp_data;
   
    pak = aurp_pak_create(OPEN_REQ_SIZE);

    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->r_connid, 0, OPEN_REQ,
		     aurp_info->r_sui);

    aurp_data = (Open_Req_t *) aurpdatastart(pak);
    PUTSHORT(&aurp_data->version, AURP_VERSION);
    aurp_data->option_count = 0; /* No options */

    if (aurp_send(pak, aurp_info->idb))
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Open-Req sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));

}

/*
 * aurp_Send_Open_Rsp
 *
 * Send an Open Resposne packet.
 */
static void
aurp_Send_Open_Rsp (at_aurp_info *aurp_info, ErrorCode code)
{
    paktype *     pak;
    Open_Rsp_t *  aurp_data;
    
    pak = aurp_pak_create(OPEN_RSP_SIZE);

    if (!pak)
	return;
    
    aurp_fill_header(aurphdrtype_start(pak), aurp_info->s_connid, 0, OPEN_RSP,
		     0);
    
    aurp_data = (Open_Rsp_t *) aurpdatastart(pak);
    PUTSHORT(&aurp_data->upd_rate, code);
    aurp_data->option_count = 0;		/* no options */
    
    if (aurp_send(pak, aurp_info->idb))
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Open-Rsp sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));

}

/*
 * aurp_Send_RI_Req
 *
 * Send a Routing Information Request packet.
 */
void
aurp_Send_RI_Req (at_aurp_info *aurp_info)
{
    paktype *     pak;

    pak = aurp_pak_create(0);	/* Empty data packet */

    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->r_connid, 0, RI_REQ,
		     aurp_info->r_sui);

    if (aurp_send(pak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_RI_OUT]++;
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: RI-Req sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
    }
}

/*
 * aurp_Send_RI_Ack
 *
 * Send a Routing Information Acknowledgement packet.
 */
static void
aurp_Send_RI_Ack (at_aurp_info *aurp_info, ushort sequence, ushort flags)
{
    paktype *     pak;
    
    pak = aurp_pak_create(0);	/* Empty data packet */

    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->r_connid, sequence,
		     RI_ACK, flags);

    if (aurp_send(pak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_RI_OUT]++;
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: RI-Ack sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
    }
}

/*
 * aurp_Send_RD
 *
 * Send a Router Down packet.
 */
void
aurp_Send_RD (at_aurp_info *aurp_info, ErrorCode code, boolean isreceiver)
{
    paktype *     pak;
    RD_t *        aurp_data;
    connid_t      connid;
    ushort        sequence;

    pak = aurp_pak_create(RD_SIZE);

    if (!pak)
	return;

    connid = isreceiver ? aurp_info->r_connid : aurp_info->s_connid;
    sequence = isreceiver ? 0 : aurp_info->s_sequence;

    aurp_fill_header(aurphdrtype_start(pak), connid, sequence, RD, 0);

    aurp_data = (RD_t *) aurpdatastart(pak);
    PUTSHORT(&aurp_data->error, code);

    if (aurp_send(pak, aurp_info->idb))
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: RD sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
}

/*
 * aurp_Send_Tickle
 *
 * Send a Tickle packet.
 */
void
aurp_Send_Tickle (at_aurp_info *aurp_info)
{
    paktype *     pak;

    pak = aurp_pak_create(0);	/* Empty data packet */
    
    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->r_connid, 0, TICKLE,
		     0);

    if (aurp_send(pak, aurp_info->idb))
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Tickle sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
}


/*
 * aurp_Send_Tickle_Ack
 *
 * Send a Tickle Acknowledgement packet.
 */
static void
aurp_Send_Tickle_Ack (at_aurp_info *aurp_info)
{
    paktype *     pak;

    pak = aurp_pak_create(0);	/* Empty data packet */

    if (!pak)
	return;

    aurp_fill_header(aurphdrtype_start(pak), aurp_info->s_connid, 0,
		     TICKLE_ACK, 0);

    if (aurp_send(pak, aurp_info->idb))
	if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Tickle-Ack sent",
		   idb_get_namestring(aurp_info->idb->phys_idb));
}

/*******************************************************************
 * Packet handling routines
 *******************************************************************/

static void
handle_RD (aurpOps_t *ops)
{
    at_aurp_info *aurp_info;
    aurp_t *      aurp;
    char const *  idb_name;
    tinybool      do_debug;
    
    aurp_info = ops->aurp;
    aurp = (aurp_t *) ops->pak->network_start;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: RD received", idb_name);
    atalk_stat[ATALK_AURP_RD]++;

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags field in RD", idb_name);
	return;
    }

    if (GETSHORT(&aurp->sequence) == 0) {

	/* Data receiver just sent us RD pkt */

	if (GETSHORT(&aurp->connid) != aurp_info->s_connid) {
	    if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
			 aurp_info->idb->phys_idb))
		buginf("\nAT: %s: Wrong connection id",
		       idb_get_namestring(aurp_info->idb->phys_idb));
	    return;
	}
	aurp_Send_RI_Ack(aurp_info, 0, 0);
	aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_RD);
    } else {

	/* Data sender sent us RD pkt. */

	switch (aurp_check_sequence(aurp_info->r_sequence,
				    GETSHORT(&aurp->sequence))) {
	case SEQOK:
	    aurp_Send_RI_Ack(ops->aurp, GETSHORT(&aurp->sequence), 0);
	    /* Fall through. */
	case SEQBADSYNC:
	    errmsg(AT_ERR_OUTOFSYNC, idb_name);
	    aurp_Reset(&aurp_info->sender_fsm);
	    aurp_Reset(&aurp_info->receiver_fsm);
	    aurp_ClearPathsFor(aurp_info->idb);
	    aurp_UpdateNeighborEntry(aurp_info, neighborDown);
	    break;
	case SEQDELAYED:
	    if (do_debug)
		buginf("\nAT: %s: Delayed packet...ignoring", idb_name);
	    break;
	case SEQIGNORE:
	    break;
	default:
	    break;
	}
    }
}

/*
 * aurp_FinishZIPRsp
 *
 * Actually send out ZIP Response packet.
 */
static void
aurp_FinishZIPRsp (paktype *pak, atalkidbtype *idb, ushort cnt, int nbytes,
		   ushort subcode, connid_t connid)
{
    ZI_Rsp_t *    aurp_data;

    if (pak == NULL)
	return;

    pak->datagramsize -= nbytes;
    aurp_data = (ZI_Rsp_t *) aurpdatastart(pak);
    PUTSHORT(&aurp_data->subcode, subcode);
    PUTSHORT(&aurp_data->tuple_count, cnt);
    aurp_fill_header(aurphdrtype_start(pak), connid, 0, ZIP_RSP, 0);
    if (aurp_send(pak, idb)) {
	atalk_stat[ATALK_AURP_ZI_OUT]++;
	atalk_stat[ATALK_ZIP_OUT]++;
    }
}

/*
 * aurp_send_xreply
 *
 * Send an extended ZIP Reply packet for a network.  If zonelist doesn't fit
 * in a single packet, multiple packets are sent.
 */
static void
aurp_send_xreply (at_aurp_info *aurp_info, atalk_rdb_t *route)
{
    paktype *     pak;
    zipnetentry * tzn;
    int           NumZones;
    int           nbytes;
    zipentry *    ze;
    int           count;
    atalkidbtype *     idb;
    int           filter_list;
    ushort        remap_net;
    char const *  idb_name;
    
    idb = aurp_info->idb;
    filter_list = idb->atalk_zipreply_filter;

    idb_name = idb_get_namestring(idb->phys_idb);
    
    NumZones = atalk_CountAndFilterZones(route, filter_list);
    
    pak = NULL;
    count = 0;
    nbytes = 0;
    ze = NULL;

    for (tzn = atalk_firstZone(route); tzn; tzn = atalkzn_nextZone(tzn)) {
	
	if (!atalk_zonecheck(tzn->zone, filter_list))
	    continue;
	
	if ((nbytes < (tzn->zone->length + ZIPENTRYBYTES)) && (pak != NULL)) {
	    aurp_FinishZIPRsp(pak, aurp_info->idb, NumZones, nbytes, XZIRSP,
			     aurp_info->s_connid);
	    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, aurp_info->idb->phys_idb))
		buginf("\nAT: %s: Sent Extended ZIP reply for %#A with %d of "
		       "%d zones",
		       idb_name, atalk_CableRange(route), count, NumZones);
	    pak = NULL;
	    nbytes = 0;
	    count = 0;
	}
	if (pak == NULL) {
	    pak = aurp_pak_create(DDP_MAX_DATABYTES);

	    if (pak == NULL) {
		return;
	    }
	    nbytes = pak->datagramsize - AURP_HEADER_SIZE - ZI_RSP_SIZE;
	    ze = (zipentry *) (aurpdatastart(pak) + ZI_RSP_SIZE);
	}

	if(idb->atalk_dminfo) {
	    remap_net = atdomain_GetRemapOut(idb,route->dstrngstr);
	    if(remap_net)
		PUTSHORT(ze->net, remap_net);
	    else
		PUTSHORT(ze->net, route->dstrngstr);
	}
	else
	    PUTSHORT(ze->net, route->dstrngstr);
	ze->len = tzn->zone->length;
	bcopy(tzn->zone->name, ze->name, tzn->zone->length);
	count++;
	nbytes -= ZIPENTRYBYTES + ze->len - 1;
	ze = (zipentry *) (((uchar *) ze) + ZIPENTRYBYTES + ze->len - 1);
    }
    if (pak == NULL)
	return;
    if (count == 0) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT:	No zones in extended reply!");
	datagram_done(pak);
	return;
    }

    aurp_FinishZIPRsp(pak, aurp_info->idb, NumZones, nbytes, XZIRSP, 
		      aurp_info->s_connid);
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %s: Sent Extended ZIP reply for %#A with %d of %d "
	       "zones", idb_name, atalk_CableRange(route), count, NumZones);
}

/*
 * aurp_FinishZIPReq
 *
 * Actually, send out ZIP Query packet.
 */
static boolean
aurp_FinishZIPReq(paktype *pak, atalkidbtype *idb, int nbytes, int subcode,
		  connid_t connid)
{
    ZI_Req_t *    aurp_data;
    
    if (pak == NULL)
	return (FALSE);

    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: %s: Sending ZIP-REQ", idb_get_namestring(idb->phys_idb));

    pak->datagramsize -= nbytes;
    aurp_data = (ZI_Req_t *) aurpdatastart(pak);
    PUTSHORT(&aurp_data->subcode, subcode);
    aurp_fill_header(aurphdrtype_start(pak), connid, 0, ZIP_REQ, 0);
    if (aurp_send(pak, idb)) {
	atalk_stat[ATALK_AURP_ZI_OUT]++;
	atalk_stat[ATALK_ZIP_OUT]++;
	return (TRUE);
    }
    return (FALSE);
}

/*
 * aurp_add_query
 * 
 * Add a net to a ZIP Query packet, pak.  If no packet is supplied or
 * there is no room left, one is created.
 */
static void
aurp_add_query (at_aurp_info *aurp_info, paktype **pak, int *nbytes,
		ushort net)
{
    paktype *     rpak;
    static uchar *ze;		/* Where to put next net. */
    ushort remap_net;

    rpak = *pak;

    if (*nbytes < DDPNETBYTES && rpak) {
	aurp_FinishZIPReq(rpak, aurp_info->idb, *nbytes, ZIREQ,
			  aurp_info->r_connid);
	*pak = rpak = NULL;
	*nbytes = 0;
    }

    if (rpak == NULL) {
	rpak = aurp_pak_create(DDP_MAX_DATABYTES);
	if (rpak == NULL) {
	    return;
	}
	*pak = rpak;
	*nbytes = rpak->datagramsize - AURP_HEADER_SIZE - ZI_REQ_SIZE;
	ze = aurpdatastart(rpak) + ZI_REQ_SIZE;
    }

    if(aurp_info && aurp_info->idb->atalk_dminfo) {
	remap_net = atdomain_FindBackwardRemap(aurp_info->idb->atalk_dminfo->in,net);
	if (remap_net)
	    net = remap_net;
    }

    PUTSHORT(ze, net);
    ze += DDPNETBYTES;
    *nbytes -= DDPNETBYTES;
}

/*
 * aurp_add_reply
 *
 * Add a zonelist for network, net, to pak.  Create packet if none is 
 * supplied.  If zonelist doesn't fit in packet, send an extended reply.
 */
static void
aurp_add_reply (at_aurp_info *aurp_info, paktype **pak, int *cnt,
		int *nbytes, ushort net)
{
    atalk_rdb_t * route;
    rbTree *      tree;
    zipnetentry * zn;
    paktype *     rpak;
    static zipentry *
                  ze;		/* Where to put next zone tuple. */
    int           AllZonesLen;	/* Total length of all zones. */
    int           NumZones;
    int           ZoneListLen;	/* Amt. of packet space needed to store list */
    int           filter_list;
    atalkidbtype *     idb;
    ushort        remap_net;

    tree = atroute_GetTree();

    
    idb = aurp_info->idb;
    if (idb->atalk_dminfo) {
	remap_net = atdomain_FindBackwardRemap(idb->atalk_dminfo->out, net);
	if (remap_net)
	    net = remap_net;
    }
    route = atroute_FindRoute(tree, net, FALSE);

    if (route == NULL)
	return;


    filter_list = idb->atalk_zipreply_filter;

    /* If zonelist is being updated or route has no zonelist, abort. */
    if (route->zoneupdate || (atalk_firstZone(route) == NULL)) {
	ReleaseRoute(route);
	return;
    }

    /* Find total length and number of zones. */
    atalk_FilteredZoneListLengthAndCount(route, &AllZonesLen, &NumZones,
					 filter_list);

    if (NumZones == 0 || AllZonesLen == 0) {
	ReleaseRoute(route);
	return;
    }

    /* Calculate space needed to put zonelist into packet */
    ZoneListLen = AllZonesLen + NumZones * (ZIPENTRYBYTES - 1);
    
    if (ZoneListLen > DDP_MAX_DATABYTES) {
	aurp_send_xreply(aurp_info, route);
	ReleaseRoute(route);
	return;
    }

    rpak = *pak;

    if (rpak && *nbytes < ZoneListLen) {
	/* No room left in packet to fit zonelist for this route,
	 * so kick off this packet.
	 */
	aurp_FinishZIPRsp(rpak, aurp_info->idb, *cnt, *nbytes, ZIRSP,
			  aurp_info->s_connid);
	*pak = rpak = NULL;
	*nbytes = 0;
	*cnt = 0;
    }

    if (rpak == NULL) {
	rpak = aurp_pak_create(DDP_MAX_DATABYTES);
	if (rpak == NULL) {
	    ReleaseRoute(route);
	    return;
	}
	*pak = rpak;
	*nbytes = rpak->datagramsize - AURP_HEADER_SIZE - ZI_RSP_SIZE;
	*cnt = 0;
	ze = (zipentry *) (aurpdatastart(rpak) + ZI_RSP_SIZE);
    }

    for (zn = atalk_firstZone(route); zn; zn = atalkzn_nextZone(zn)) {
    
	/* check for access filtering */
	if (!atalk_zonecheck(zn->zone, filter_list))
	    continue;

	if(idb->atalk_dminfo) {
	    remap_net = atdomain_GetRemapOut(idb,route->dstrngstr);
	    if(remap_net) 
		PUTSHORT(ze->net, remap_net);
	    else
		PUTSHORT(ze->net, route->dstrngstr);
	}
	else
	    PUTSHORT(ze->net, route->dstrngstr);
	ze->len = zn->zone->length;
	bcopy(zn->zone->name, ze->name, zn->zone->length);
	(*cnt)++;
	*nbytes -= ZIPENTRYBYTES + ze->len - 1;
	ze = (zipentry *) (((uchar *) ze) + ZIPENTRYBYTES + ze->len - 1);
    }
    ReleaseRoute(route);
}

static void
handle_SZIforRIUpd (at_aurp_info *aurp_info, paktype *pak)
{
    paktype *     newpak;
    int           nbytes;
    Event_tuple_t *
                  tuple;
    ushort        rngstart;
    ushort        rngend;
    uchar         dist;
    EventCode     eventtype;
    int           cnt;
    int           sbytes;

    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, aurp_info->idb->phys_idb))
	buginf("\nAT: %s: Processing SZI in RI-Upd",
	       idb_get_namestring(aurp_info->idb->phys_idb));
    
    tuple = (Event_tuple_t *) aurpdatastart(pak);
    nbytes = pak->datagramsize - AURP_HEADER_SIZE;
    cnt = 0;
    newpak = NULL;
    sbytes = 0;

    while (nbytes > 0) {
	aurp_GetNextEvent(&tuple, &nbytes, &rngstart, &rngend, &dist,
			  &eventtype);
	if (eventtype == NA_EVENT)
	    aurp_add_reply(aurp_info, &newpak, &cnt, &sbytes, rngstart);
    }

    if (pak) {
	aurp_FinishZIPRsp(newpak, aurp_info->idb, cnt, sbytes, ZIRSP,
			  aurp_info->s_connid);
    }
}

static void
handle_SZIforRIRsp (at_aurp_info *aurp_info, paktype *pak)
{
    paktype *     newpak;
    int           nbytes;
    rtmptuple *   tuple;
    ushort        rngstart;
    ushort        rngend;
    uchar         dist;
    int           cnt;
    int           sbytes;
    
    if (pak == NULL)
	return;

    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, aurp_info->idb->phys_idb))
	buginf("\nAT: %s: Processing SZI in RI-Rsp",
	       idb_get_namestring(aurp_info->idb->phys_idb));

    tuple = (rtmptuple *) aurpdatastart(pak);
    nbytes = pak->datagramsize - AURP_HEADER_SIZE;
    cnt = 0;
    newpak = NULL;
    sbytes = 0;

    while (nbytes > 0) {
	if (!atrtmp_GetNextTuple(&tuple, &nbytes, &rngstart, &rngend, &dist,
				 AURPTUPLEVERSION)) {
	    break;
	}
	
	aurp_add_reply(aurp_info, &newpak, &cnt, &sbytes, rngstart);

    }

    if (pak) {
	aurp_FinishZIPRsp(newpak, aurp_info->idb, cnt, sbytes, ZIRSP,
			  aurp_info->s_connid);
    }
}

static void
handle_RIAck (aurpOps_t *ops)
{
    at_aurp_info *aurp_info;
    aurp_t *      aurp;
    FSMFUNC       curstate;
    paktype *     pak;
    paktype *     nextpak;
    char const *  idb_name;
    tinybool      do_pkt_debug, do_conn_debug;
    
    aurp_info = ops->aurp;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_pkt_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			    aurp_info->idb->phys_idb);
    do_conn_debug = at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
			     aurp_info->idb->phys_idb);
    
    if (do_pkt_debug)
	buginf("\nAT: %s: RI-Ack received", idb_name);
    atalk_stat[ATALK_AURP_RI_IN]++;
    aurp = (aurp_t *) ops->pak->network_start;

    if (GETSHORT(&aurp->sequence) == 0) {
	/* Must be RI-Ack in response to the RD pkt we sent */
	if (GETSHORT(&aurp->connid) != aurp_info->r_connid) {
	    if (do_conn_debug)
		buginf("\nAT: %s: Wrong connection id", idb_name);
	    return;
	}
	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_RI_ACK_DONE);
	return;
    } else {
	if (GETSHORT(&aurp->connid) != aurp_info->s_connid) {
	    if (do_conn_debug)
		buginf("\nAT: %s: Wrong connection id", idb_name);
	    return;
	}
	switch (aurp_check_sequence(aurp_info->s_sequence,
				    GETSHORT(&aurp->sequence))) {
	case SEQOK:
	    aurp_info->s_sequence = aurp_next_sequence(aurp_info->s_sequence);
	    break;
	case SEQDELAYED:
	    if (do_pkt_debug)
		buginf("\nAT: %s: Delayed packet...ignoring", idb_name);
	    /* Do nothing */
	    break;
	case SEQBADSYNC:
	    errmsg(AT_ERR_OUTOFSYNC, idb_name);
	    aurp_info->s_error = LOSTSYNC; /* Error code to pass in RD pkt */
	    aurp_Disconnect(&aurp_info->sender_fsm);
	    return;
	    break;
	case SEQIGNORE:
	default:
	    return;
	    break;
	}

	curstate = atfsm_get_state(&aurp_info->sender_fsm);

	if (curstate == aurp_fsm_DS_RI_Ack_Wait1) {
	    if ((pak = aurp_info->pktqueue) != NULL) {
		nextpak = pak->next;
		pak->next = NULL;
		if (GETSHORT(&aurp->flags) & SZI_MASK)
		    handle_SZIforRIRsp(aurp_info, pak);
		datagram_done(pak);
		aurp_info->pktqueue = nextpak;
		if (nextpak) {
		    aurp_info->s_retry_count = 0;
		    TIMER_START(aurp_info->s_retry_timer,
				RIRspIntervals[0] * ONESEC);
		    aurp_Send_RI_Rsp(aurp_info);
		} else {
		    aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_RI_ACK_DONE);
		    aurp_info->s_informed = TRUE;
		}
	    }
	} else if (curstate == aurp_fsm_DS_RI_Ack_Wait2) {
	    if ((pak = aurp_info->pktqueue) != NULL) {
		nextpak = pak->next;
		pak->next = NULL;
		if (GETSHORT(&aurp->flags) & SZI_MASK)
		    handle_SZIforRIUpd(aurp_info, pak);
		datagram_done(pak);
		aurp_info->pktqueue = nextpak;
		if (nextpak) {
		    aurp_info->s_retry_count = 0;
		    TIMER_START(aurp_info->s_retry_timer,
				RIUpdIntervals[0] * ONESEC);
		    aurp_Send_RI_Upd(aurp_info);
		} else {
		    aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_RI_ACK_DONE);
		}
	    }
	} else if ((curstate == aurp_fsm_DS_RI_Ack_Wait3) ||
		   (curstate == aurp_fsm_DS_RI_Ack_Wait4)) {
	    aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_RI_ACK_DONE);
	} else 
	    if (do_pkt_debug)
		buginf("\nAT: %s: Unexpected RI-Ack", idb_name);
	return;
    }
}

static void
handle_Tickle (aurpOps_t *ops)
{
    at_aurp_info *aurp_info;
    aurp_t *      aurp;
    char const *  idb_name;
    tinybool      do_debug;
    
    aurp_info = ops->aurp;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: Tickle received", idb_name);

    aurp = (aurp_t *) ops->pak->network_start;

    if (GETSHORT(&aurp->connid) != aurp_info->s_connid) {
	if (do_debug)
	    buginf("\nAT: %s: Wrong connection id in Tickle", idb_name);
	return;
    }

    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero sequence number in Tickle", idb_name);
	return;
    }

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags field in Tickle", idb_name);
	return;
    }

    if (atfsm_get_state(&aurp_info->sender_fsm) != aurp_fsm_DS_Connected) {
	if (do_debug)
	    buginf("\nAT: %s: Unexpected Tickle", idb_name);
	return;
    }

    aurp_Send_Tickle_Ack(aurp_info);
}

static void
handle_TickleAck (aurpOps_t *ops)
{
    at_aurp_info *
                aurp_info;
    aurp_t *    aurp;
    char const *idb_name;
    tinybool    do_debug;

    aurp_info = ops->aurp;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: Tickle-Ack received", idb_name);

    aurp = (aurp_t *) ops->pak->network_start;

    if (GETSHORT(&aurp->connid) != aurp_info->r_connid) {
	if (do_debug)
	    buginf("\nAT: %s: Wrong connection id in Tickle-Ack", idb_name);
	return;
    }

    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero sequence number in Tickle-Ack",
		   idb_name);
	return;
    }

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags field in Tickle-Ack", idb_name);
	return;
    }

    aurp_UpdateNeighborEntry(aurp_info, neighborUp);
    if (atfsm_get_state(&aurp_info->receiver_fsm)
	== aurp_fsm_DR_Tickle_Ack_Wait) {
	GET_TIMESTAMP(aurp_info->r_lastheard); /* Reset last-heard-from time */
	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_TICKLE_ACK);
    }
}

static void
handle_RIRsp (aurpOps_t *ops)
{
    aurp_t     *  aurp;
    paktype    *  pak;
    FSMFUNC 	  curstate;
    int 	  nbytes;
    rtmptuple  *  rt;
    ushort 	  rngstart, rngend;
    uchar	  dist;
    at_aurp_info *aurp_info;
    atalkidbtype    *  idb;
    char const *  idb_name;
    tinybool      do_debug;
    
    aurp_info = ops->aurp;
    pak = ops->pak;
    idb = aurp_info->idb;
    aurp = (aurp_t *) pak->network_start;

    idb_name = idb_get_namestring(idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: RI-Rsp received", idb_name);
    atalk_stat[ATALK_AURP_RI_IN]++;

    /* See if we're in the right state to receive */
    curstate = atfsm_get_state(&aurp_info->receiver_fsm);
    if (curstate != aurp_fsm_DR_RI_Rsp_Wait &&
	curstate != aurp_fsm_DR_Connected &&
	curstate != aurp_fsm_DR_Tickle_Ack_Wait) {
	if (do_debug)
	    buginf("\nAT: %s: Unexpected RI-Rsp", idb_name);
	return;
    }

    if (aurp_info->r_connid != GETSHORT(&aurp->connid)) {
	if (do_debug)
	    buginf("\nAT: %s: Wrong connection id in RI-Rsp", idb_name);
	return;
    }

    switch (aurp_check_sequence(aurp_info->r_sequence,
				GETSHORT(&aurp->sequence))) {
    case SEQOK:
	aurp_info->r_sequence = aurp_next_sequence(aurp_info->r_sequence);
	aurp_Send_RI_Ack(ops->aurp, GETSHORT(&aurp->sequence), SZI_MASK);
	break;
    case SEQDELAYED:
	if (do_debug)
	    buginf("\nAT: %s: Delayed packet...ignoring", idb_name);
	aurp_Send_RI_Ack(ops->aurp, GETSHORT(&aurp->sequence), SZI_MASK);
	return;
    case SEQBADSYNC:
	errmsg(AT_ERR_OUTOFSYNC, idb_name);
	aurp_info->r_error = LOSTSYNC;	/* Error code to pass in RD pkt */
	aurp_Disconnect(&aurp_info->receiver_fsm);
	aurp_ClearPathsFor(aurp_info->idb);
	aurp_UpdateNeighborEntry(aurp_info, neighborDown);
	return;
	break;
    case SEQIGNORE:
	if (do_debug)
	    buginf("\nAT: %s: Unexpected sequence...ignoring", idb_name);
	return;
    default:
	return;
    }

    aurp_UpdateNeighborEntry(aurp_info, neighborUp);
    GET_TIMESTAMP(aurp_info->r_lastheard); /* Reset last-heard-from time */
    nbytes = pak->datagramsize - (pak->network_start - pak->info_start)
	- AURP_HEADER_SIZE;
    rt = (rtmptuple *) aurpdatastart_aurph(aurp);

    while (nbytes > 0) {
	if (!atrtmp_GetNextTuple(&rt, &nbytes, &rngstart, &rngend, &dist,
				 AURPTUPLEVERSION)) {
	    return;			/* We'll eventually resend RI-Req. */
	}
	
	/* Perform access filtering */
	if (idb->atalk_route_filter_in &&
	    !atalk_routecheck(rngstart, rngend, idb->atalk_route_filter_in))
	    continue;

	if (dist < DDP_MAX_HOPCOUNT) {
	    NA_event(aurp_info, rngstart, rngend, dist);
	} else {
	    ND_event(aurp_info, rngstart, rngend);
	}
    }

    if (GETSHORT(&aurp->flags) & LASTFLAG_MASK) { /* Last RI-Rsp packet? */
	aurp_info->r_informed = TRUE;	/* Ok, we're fully updated. */
	if (curstate == aurp_fsm_DR_RI_Rsp_Wait ||
	    curstate == aurp_fsm_DR_Tickle_Ack_Wait) {
	    aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_RI_RSP);
	}
    }

}

static void
handle_RIUpd (aurpOps_t *ops)
{
    aurp_t *      aurp;
    paktype *     pak;
    at_aurp_info *aurp_info;
    int           nbytes;
    rbTree *      path_tree;
    Event_tuple_t *
                  tuple;
    ushort        rngstart;
    ushort        rngend;
    uchar         dist;
    EventCode     eventcode;
    atalkidbtype *     idb;
    char const *  idb_name;
    tinybool      do_debug;
    

    pak = ops->pak;
    aurp_info = ops->aurp;
    idb = aurp_info->idb;
    aurp = (aurp_t *) pak->network_start;

    idb_name = idb_get_namestring(idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: RI-Upd received", idb_name);
    atalk_stat[ATALK_AURP_RI_IN]++;
    if (aurp_info->r_connid != GETSHORT(&aurp->connid)) {
	if (do_debug)
	    buginf("\nAT: %s: Wrong connection id in RI-Upd", idb_name);
	return;
    }

    switch (aurp_check_sequence(aurp_info->r_sequence,
				GETSHORT(&aurp->sequence))) {
    case SEQOK:
	aurp_info->r_sequence = aurp_next_sequence(aurp_info->r_sequence);
	aurp_Send_RI_Ack(ops->aurp, GETSHORT(&aurp->sequence), SZI_MASK);
	break;
    case SEQDELAYED:
	if (do_debug)
	    buginf("\nAT: %s: Delayed packet...resending RI-Ack", idb_name);
	aurp_Send_RI_Ack(ops->aurp, GETSHORT(&aurp->sequence), SZI_MASK);
	return;
    case SEQBADSYNC:
	errmsg(AT_ERR_OUTOFSYNC, idb_name);
	aurp_info->r_error = LOSTSYNC;
	aurp_Disconnect(&aurp_info->receiver_fsm);
	aurp_ClearPathsFor(aurp_info->idb);
	aurp_UpdateNeighborEntry(aurp_info, neighborDown);
	return;
	break;
    case SEQIGNORE:
	if (do_debug)
	    buginf("\nAT: %s: Unexpected sequence...ignoring", idb_name);
	return;
    default:
	return;
    }

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags field in RI-Upd", idb_name);
	return;
    }

    aurp_UpdateNeighborEntry(aurp_info, neighborUp);
    GET_TIMESTAMP(aurp_info->r_lastheard); /* Reset last-heard-from time */

    aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_RI_RSP);

    nbytes = pak->datagramsize - (pak->network_start - pak->info_start)
	- AURP_HEADER_SIZE;
    tuple = (Event_tuple_t *) aurpdatastart_aurph(aurp);
    path_tree = atroute_GetPTree(AT_PATH_AURP);

    while (nbytes >= EVENT_TUPLE_SIZE) {
	if (!aurp_GetNextEvent(&tuple, &nbytes, &rngstart, &rngend, &dist,
			       &eventcode)) {
	    if (do_debug)
		errmsg(AT_ERR_DEBUGMSG, "AURP bad event tuple");
	    break;
	}

	/* Perform access filtering */
	if (idb->atalk_route_filter_in &&
	    eventcode != ZC_EVENT && eventcode != NULL_EVENT &&
	    !atalk_routecheck(rngstart, rngend, idb->atalk_route_filter_in))
	    continue;

	switch (eventcode) {

	case NA_EVENT:
	    NA_event(aurp_info, rngstart, rngend, dist);
	    break;
	case ND_EVENT:
	    ND_event(aurp_info, rngstart, rngend);
	    break;
	case NRC_EVENT:
	    NRC_event(aurp_info, rngstart, rngend);
	    break;
	case NDC_EVENT:
	    NDC_event(aurp_info, rngstart, rngend, dist);
	    break;
	case ZC_EVENT:
	    /* Not handled */
	    break;
	case NULL_EVENT:
	    /* Do nothing */
	    break;
	default:
	    errmsg(AT_ERR_DEBUGMSG, "AURP bad event code");
	    break;
	}
    }
}

static void
handle_RIReq (aurpOps_t *ops)
{
    aurp_t *      aurp;
    paktype *     pak;
    at_aurp_info *aurp_info;
    char const *  idb_name;
    tinybool      do_debug;
    
    pak = ops->pak;
    aurp = (aurp_t *) pak->network_start;
    aurp_info = ops->aurp;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: RI-Req received", idb_name);
    atalk_stat[ATALK_AURP_RI_IN]++;
    if (GETSHORT(&aurp->connid) != aurp_info->s_connid) {
	if (do_debug)
	    buginf("\nAT: %s: Connection ID mismatch", idb_name);
	return;
    }

    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug) {
	    atalk_stat[ATALK_AURP_BAD_SEQ]++;
	    buginf("\nAT: %s: Nonzero sequence number", idb_name);
	}
	return;
    }

    if (atfsm_get_state(&aurp_info->sender_fsm) != aurp_fsm_DS_Connected) {
	if (do_debug)
	    buginf("\nAT: %s: Ignoring RI-Req", idb_name);
	return;
    }

    aurp_info->s_sui = GETSHORT(&aurp->flags);
    aurp_info->s_informed = FALSE;

    aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_RI_REQ);
}

static void
handle_OpenRsp (aurpOps_t *ops)
{
    aurp_t *      aurp;
    Open_Rsp_t *  aurp_data;
    paktype *     pak;
    at_aurp_info *aurp_info;
    char const *  idb_name;
    tinybool      do_debug;
    

    pak = ops->pak;
    aurp_info = ops->aurp;

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);

    if (do_debug)
	buginf("\nAT: %s: Open-Rsp received", idb_name);

    if (atfsm_get_state(&aurp_info->receiver_fsm)
	!= aurp_fsm_DR_Open_Rsp_Wait) {
	
	if (do_debug)
	    buginf("\nAT: %s: Unexpected Open-Rsp packet", idb_name);
	return;
    }
    aurp = (aurp_t *) pak->network_start;
    aurp_data = (Open_Rsp_t *) aurpdatastart_aurph(aurp);

    if (GETSHORT(&aurp->connid) != aurp_info->r_connid) {
	if (do_debug)
	    buginf("\nAT: %s: Connection ID mismatch", idb_name);
	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_OPEN_RSPBAD);
	return;
    }
    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Nonzero sequence number", idb_name);
	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_OPEN_RSPBAD);
	return;
    }
    if ((short) GETSHORT(&aurp_data->upd_rate) < 0) {
	if (do_debug)
	    buginf("\nAT: %s: Error opening connection", idb_name);

	aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_OPEN_RSPBAD);
	return;
    }
    
    aurp_info->r_remapping = (GETSHORT(&aurp->flags) & REMAPPING_MASK);
    aurp_info->r_hopreduce = (GETSHORT(&aurp->flags) & HOPREDUCE_MASK);
    aurp_info->r_update_rate = GETSHORT(&aurp_data->upd_rate);

    aurp_fsm(&aurp_info->receiver_fsm, AURP_FSM_OPEN_RSPGOOD);
    aurp_UpdateNeighborEntry(aurp_info, neighborUp);
}

static void
handle_OpenReq (aurpOps_t *ops)
{
    aurp_t *      aurp;
    Open_Req_t *  options;
    at_aurp_info *aurp_info;
    paktype *     pak;
	
    pak = ops->pak;
    aurp = (aurp_t *)pak->network_start;
    options = (Open_Req_t *) aurpdatastart_aurph(aurp);
    aurp_info = ops->aurp;

    if (at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK, aurp_info->idb->phys_idb))
	buginf("\nAT: %s: Open-Req received",
	       idb_get_namestring(aurp_info->idb->phys_idb));

    atalk_stat[ATALK_AURP_OPEN_REQ]++;

    if (GETSHORT(&options->version) != AURP_VERSION
	|| GETSHORT(&aurp->sequence) != 0) {
	aurp_Send_Open_Rsp(ops->aurp, INVALIDVERSION);
	return;
    }

    if (atfsm_get_state(&aurp_info->sender_fsm) != aurp_fsm_DS_Unconnected) {
	if (GETSHORT(&aurp->connid) != aurp_info->s_connid) {

	    if (atfsm_get_state(&aurp_info->sender_fsm) ==
		aurp_fsm_DS_Connected) {
		aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_OPENREQBAD);
	    }
	    return;
	}

	/* Round nominal update rate to nearest multiple of 10 seconds. */
	aurp_Send_Open_Rsp(ops->aurp,
			   (aurp_eventUpdateInterval + 10) / 10 - 1);
	return;
    }
    
    aurp_info->s_connid = GETSHORT(&aurp->connid);
    aurp_info->s_sui = GETSHORT(&aurp->flags);

    /* Round nominal update rate to nearest multiple of 10 seconds. */
    aurp_Send_Open_Rsp(ops->aurp, (aurp_eventUpdateInterval + 10) / 10 - 1);
    aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_INITIATE);

    /* Prod Data receiver */
    if (atfsm_get_state(&aurp_info->receiver_fsm) ==
	aurp_fsm_DR_Open_Rsp_Wait) {
	aurp_Send_Open_Req(aurp_info);
    }
}

static void
process_ZIRsp (ZI_Rsp_t *aurp_data, int length, atalkidbtype *idb)
{
    int           cnt;
    rbTree *      tree;
    rbTree *      path_tree;
    atalk_rdb_t * zlist;	/* List of routes which we updated */
    atalk_pdb_t * path;
    ushort        net;
    uchar *       curptr;	/* Points at zone tuple in packet */
    uchar *       baseptr;	/* Points at head of list of zones in */
				/* packet.  Used to calculate offsets */
				/* for optimized tuples. */
    aurp_zipentry *zip;
    sys_timestamp now;
    atalk_rdb_t *r;
    int ZipTupleSize;
    boolean valid;
    ushort  remap_start;
    char const *  idb_name;
    
    idb_name = idb_get_namestring(idb->phys_idb);
    
    GET_TIMESTAMP(now);
    tree = atroute_GetTree();
    path_tree = atroute_GetPTree(AT_PATH_AURP);
    zlist = NULL;
    cnt = GETSHORT(&aurp_data->tuple_count);

    if (cnt == 0)
	return;

    length -= ZI_RSP_SIZE;
    curptr = ((uchar *) aurp_data + ZI_RSP_SIZE);
    baseptr = curptr + sizeof(net);

    while (cnt > 0 || length > DDPNETBYTES) {

	zip = (aurp_zipentry *) curptr;

	net = GETSHORT(&zip->net);

	if (idb && idb->atalk_dminfo) {
	    remap_start = atdomain_FindForwardRemap(idb->atalk_dminfo->in,net);
	    if (remap_start)
		net = remap_start;
	}
	if (zip->zone_info.long_tuple.length & OPTIMIZED_TUPLE) {
	    /* Tuple uses optimized format.  Compute location of zone name. */
	    zip = (aurp_zipentry *) (baseptr
				     + (GETSHORT(&zip->zone_info.offset)
					& OFFSET_MASK) - sizeof(net));
	    /* Compute size of tuple.  Allows us to find next tuple. */
	    ZipTupleSize = sizeof(net) + sizeof(zip->zone_info.offset);
	} else {
	    /* Compute size of tuple.  Allows us to find next tuple. */
	    ZipTupleSize = sizeof(net) +
		sizeof(zip->zone_info.long_tuple.length) +
		zip->zone_info.long_tuple.length;
	}

	/* If tuple is illegal, then abort the whole process. */
	if (zip->zone_info.long_tuple.length > ZONELEN
	    || zip->zone_info.long_tuple.length == 0)
	    break;

	r = atroute_FindRoute(tree, net, FALSE);

	if (r) {
	    
	    path = atroute_GetValidPath(path_tree, r->dstrngstr, r->dstrngend,
					&valid, FALSE);
	    if (!valid || !path) {
		ReleaseRoute(r);
		r = NULL;
	    }
	}
	
	if (r != NULL) {
	    
	    if (!idb->atalk_route_filter_in
		|| atalk_routecheck(r->dstrngstr, r->dstrngend,
				    idb->atalk_route_filter_in)) {
		if (r->zoneupdate) {
		    /*
		     * This case releases the route for additional zones in
		     * the same packet.  Otherwise we would 
		     * lock it too many times and not know how many calls to
		     * release to make when we finish processing.
		     */
		    ReleaseRoute(r);
		} else {
		    atalk_clear_zones(r);
		    r->zoneupdate = TRUE;
		    r->zoneaddr = 0;
		    r->zonetimer = now;
		    r->zonecnt = 0;
		    r->znext = zlist;
		    zlist = r;
		}
		atzip_AddZone(r, zip->zone_info.long_tuple.name,
			      zip->zone_info.long_tuple.length);
		if (at_debug(atalk_events, ATALK_NULL_MASK))
		    errmsg(AT_ERR_NEWZONE, zip->zone_info.long_tuple.length,
			   zip->zone_info.long_tuple.name);
	    } else {
		/* This case releases the route because we are ignoring it. */

		ReleaseRoute(r);
		if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		    buginf("AT: %s: Skipping zone update for %#A because "
			   "route is access controlled",
			   idb_name,
			   atalk_CableRange(r));
	    }
	} else {
	    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\nAT: %s: Received zone for no route", idb_name);
	}
	cnt--;
	curptr += ZipTupleSize;
	length -= ZipTupleSize;
    }
    while (zlist != NULL) {
	r = zlist;
	zlist = zlist->znext;
	r->znext = NULL;
	zip_FinishMerge(r, now);
	/* 
	 * For now, only redist best_path.  Eventually, we will have to
         * redist all equalpaths, too.
	 */
	atroute_path_redist_rbshell((treeLink *)r->best_path, FALSE);
	ReleaseRoute(r);
    }
}

static void
process_XZIRsp (ZI_Rsp_t *aurp_data, int length, atalkidbtype *idb)
{
    int           cnt;
    rbTree *      tree;
    rbTree *      path_tree;
    atalk_pdb_t * path;
    ushort        net;
    aurp_zipentry *
                  zip;
    atalk_rdb_t * r;
    sys_timestamp now;
    boolean       err = FALSE;
    int           x;
    boolean       valid;
    ushort        remap_start;

    GET_TIMESTAMP(now);
    tree = atroute_GetTree();
    path_tree = atroute_GetPTree(AT_PATH_AURP);
    cnt = GETSHORT(&aurp_data->tuple_count);
    zip = (aurp_zipentry *) ((uchar *) aurp_data + ZI_RSP_SIZE);
    length -= ZI_RSP_SIZE;
    net = GETSHORT(&zip->net);

    if (idb && idb->atalk_dminfo) {
	remap_start = atdomain_FindForwardRemap(idb->atalk_dminfo->in, net);
	if (remap_start)
	    net = remap_start;
    }
    r = atroute_FindRoute(tree, net, FALSE);
    if (r == NULL)
	return;
    
    path = atroute_GetValidPath(path_tree, r->dstrngstr, r->dstrngend, &valid, FALSE);
    if (!valid || !path) {
	ReleaseRoute(r);
	return;
    }
    
    if (idb->atalk_route_filter_in
	&& !atalk_routecheck(r->dstrngstr, r->dstrngend,
			     idb->atalk_route_filter_in)) {
	ReleaseRoute(r);
	return;
    }

    if (r->zoneupdate && (r->zonecnt != 0)) {
	if (idb != r->rpath.idb) {
	    ReleaseRoute(r);
	    return;
	}
    } else {
	atalk_clear_zones(r);
	r->zoneupdate = TRUE;
	r->zonecnt = cnt;
    }
    r->zonetimer = now;
    
    while (length > ZIPENTRYBYTES && (r->zonecnt > 0)) {
	if (zip->net != net || zip->zone_info.long_tuple.length == 0 ||
	    zip->zone_info.long_tuple.length > ZONELEN) {
	    err = TRUE;
	    break;
	}
	atzip_AddZone(r, zip->zone_info.long_tuple.name,
		      zip->zone_info.long_tuple.length);
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_NEWZONE, zip->zone_info.long_tuple.length,
		   zip->zone_info.long_tuple.name);
	r->zonecnt--;
	x = ZIPENTRYBYTES + zip->zone_info.long_tuple.length - 1;
	length -= x;
	zip = (aurp_zipentry *) (((uchar *)zip) + x);
    }
    if (err) {
	atalk_clear_zones(r);
    }
    if ((r->zonecnt <= 0) || err) {
	zip_FinishMerge(r, now);
    }
    /* 
     * For now, only redist best_path.  Eventually, we will have to
     * redist all equalpaths, too.
     */
    if (r->zonecnt <= 0 && r->zoneupdate == FALSE)
	atroute_path_redist_rbshell((treeLink *)r->best_path, FALSE);
    ReleaseRoute(r);
}

static ziptabletype *
aurp_GDZLGetNextZone (ziptabletype *zone, atalkidbtype *idb, int *bucket)
{
    zipnetentry * zn;

    while (1) {
	zone = zip_GetNextZone(idb, zone, bucket);
	if (zone == NULL)		/* no more zones? stop looking. */
	    break;

	if (idb->atalk_route_filter != 0)
	    if (!atalk_gzlzonecheck(zone, idb->atalk_route_filter)
		|| !atalk_rtmpzonecheck(zone, idb->atalk_route_filter)) {
		continue;
	    }
	if ((zn = atalk_firstNet(zone)) != NULL) {
	    while (zn != NULL) {
		if (!atalkif_AURPTunnel(zn->net->rpath.idb))
		    break;		/* zone has net in our local */
					/* internet, so return it */
		zn = atalkzn_nextNet(zn);
	    }
	    if (zn != NULL)
		break;		
	}
    }
    return (zone);
	    
}

static void
handle_GDZLReq (at_aurp_info *aurp_info, ushort index)
{
    GDZL_Rsp_t *  aurp_data;
    paktype *     pak;
    int           cnt;
    ziptabletype *zone;
    int           bucket;
    uchar *       p;
    boolean       last = FALSE;
    atalkidbtype *     idb;
    int           nbytes;

    atalk_stat[ATALK_AURP_GDZL_IN]++;
    pak = aurp_pak_create(DDP_MAX_DATABYTES);
    if (!pak)
	return;

    idb = aurp_info->idb;
    aurp_data = (GDZL_Rsp_t *) aurpdatastart(pak);
    nbytes = pak->datagramsize - AURP_HEADER_SIZE - GDZL_RSP_SIZE;
    cnt = 0;
    zone = NULL;
    bucket = 0;
    p = aurpdatastart(pak) + GDZL_RSP_SIZE;
    
    while (1) {
	zone = aurp_GDZLGetNextZone(zone, idb, &bucket);
	if (zone == NULL)
	    break;
	if (nbytes < zone->length + 1)
	    break;
	cnt++;
	if (cnt < index)
	    continue;
	*p++ = zone->length;
	bcopy(zone->name, p, zone->length);
	p += zone->length;
	nbytes -= zone->length + 1;
    }
    pak->datagramsize -= nbytes;
    PUTSHORT(&aurp_data->subcode, GDZLRSP);
    PUTSHORT(&aurp_data->index, index);
    if (zone == NULL || aurp_GDZLGetNextZone(zone, idb, &bucket) == NULL)
	last = TRUE;
    aurp_fill_header(aurphdrtype_start(pak), aurp_info->s_connid, 0, ZIP_RSP,
		     last ? LASTFLAG_MASK : 0);
    if (aurp_send(pak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_GDZL_OUT]++;
    }
}

static void
handle_GZNReq (at_aurp_info *aurp_info, paktype *pak)
{
    GZN_Rsp_t *aurp_reply_data;
    uchar *p;
    rtmptuple *tuple;
    uchar zone[ZONELEN];
    uchar zonelen;
    int cnt = 0;
    paktype *rpak;
    int nbytes;
    ziptabletype *zt;
    zipnetentry *zn;
    int TupleSize;

    atalk_stat[ATALK_AURP_GZN_IN]++;
    p = aurpdatastart(pak) + GZN_REQ_SIZE;

    zonelen = *p++;
    if (zonelen > ZONELEN)
	return;
    bcopy(p, zone, zonelen);

    rpak = aurp_pak_create(DDP_MAX_DATABYTES);
    if (!rpak)
	return;
    nbytes = rpak->datagramsize - AURP_HEADER_SIZE - GZN_RSP_SIZE;

    p = aurpdatastart(rpak) + GZN_RSP_SIZE;
    *p++ = zonelen;
    bcopy(zone, p, zonelen);
    p += zonelen;
    tuple = (rtmptuple *)(p + sizeof(short));/* Leave room for network count */
    nbytes -= zonelen + 3;	/* Account for zonename and tuple count */

    zt = atzip_FindZone(zone, zonelen);
    if (zt == NULL) {
	PUTSHORT(p, 0);			/* No networks. */
    } else {
	zn = atalk_firstNet(zt);
	while (zn != NULL && nbytes > (2 * RTMPTUPLEBYTES)) {
	    TupleSize = aurp_PutTuple(zn->net, tuple, aurp_info->idb);
	    nbytes -= TupleSize;
	    tuple = (rtmptuple *) ((uchar *) tuple + TupleSize);
	    cnt++;
	    zn = atalkzn_nextNet(zn);
	}
	PUTSHORT(p, cnt);
    }

    rpak->datagramsize -= nbytes;
    aurp_reply_data = (GZN_Rsp_t *) aurpdatastart(rpak);
    PUTSHORT(&aurp_reply_data->subcode, GZNRSP);
    aurp_fill_header(aurphdrtype_start(rpak), aurp_info->s_connid, 0, ZIP_RSP,
		     0);
    if (aurp_send(rpak, aurp_info->idb)) {
	atalk_stat[ATALK_AURP_GZN_OUT]++;
    }
}

    
static void
handle_ZIReq (at_aurp_info *aurp_info, paktype *pak)
{
    aurp_t *      aurp_head;
    uchar *       net;
    int           nbytes;
    paktype *     newpak;
    int           cnt;
    int           sbytes;

    aurp_head = (aurp_t *) pak->network_start;
    net = aurpdatastart_aurph(aurp_head) + ZI_REQ_SIZE;
    nbytes = pak->datagramsize - (net - pak->info_start);
    newpak = NULL;
    cnt = 0;
    sbytes = 0;

    while (nbytes > 1) {
	aurp_add_reply(aurp_info, &newpak, &cnt, &sbytes, GETSHORT(net));
	net += DDPNETBYTES;
	nbytes -= DDPNETBYTES;
    }
    if (newpak) {
	aurp_FinishZIPRsp(newpak, aurp_info->idb, cnt, sbytes, ZIRSP,
			  aurp_info->s_connid);
    }
}

static void
handle_ZIPResponse (aurpOps_t *ops)
{
    aurp_t *      aurp;
    ZI_Rsp_t *    aurp_data;
    at_aurp_info *aurp_info;
    paktype *     pak;
    int           nbytes;
    char const *  idb_name;
    tinybool      do_debug;

    pak = ops->pak;
    aurp = (aurp_t *) pak->network_start;
    aurp_data = (ZI_Rsp_t *) aurpdatastart_aurph(aurp);
    aurp_info = ops->aurp;
    nbytes = pak->datagramsize - (pak->network_start - pak->info_start) -
	AURP_HEADER_SIZE;
    GET_TIMESTAMP(aurp_info->r_lastheard);

    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: ZIPResponse received", idb_name);

    atalk_stat[ATALK_AURP_ZI_IN]++;
    atalk_stat[ATALK_ZIP_IN]++;

    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero sequence in ZI Response", idb_name);
	return;
    }

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags in ZI Response", idb_name);
	return;
    }

    switch (GETSHORT(&aurp_data->subcode)) {
    case ZIRSP:
	process_ZIRsp(aurp_data, nbytes, aurp_info->idb);
	break;
    case XZIRSP:
	process_XZIRsp(aurp_data, nbytes, aurp_info->idb);
	break;
    default:
	if (aurp_info->neighbor)
	    aurp_info->neighbor->zipunknown++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: %s: ZI Response unknown subcode (%d), ignored",
		   idb_name, GETSHORT(&aurp_data->subcode));
	break;
    }
}

static void
handle_ZIPRequest (aurpOps_t *ops)
{
    aurp_t *      aurp;
    ZI_Req_t *    aurp_data;
    paktype *     pak;
    at_aurp_info *aurp_info;
    int           nbytes;
    char const *  idb_name;
    tinybool      do_debug;
    
    pak = ops->pak;
    aurp = (aurp_t *) pak->network_start;
    aurp_data = (ZI_Req_t *) aurpdatastart_aurph(aurp);
    aurp_info = ops->aurp;
    nbytes = pak->datagramsize - (pak->network_start - pak->info_start) -
	AURP_HEADER_SIZE;
    idb_name = idb_get_namestring(aurp_info->idb->phys_idb);
    do_debug = at_debug(atalkaurp_packet_debug, ATALK_INTERFACE_MASK,
			aurp_info->idb->phys_idb);
    
    if (do_debug)
	buginf("\nAT: %s: ZIPRequest received", idb_name);

    atalk_stat[ATALK_AURP_ZI_IN]++;
    atalk_stat[ATALK_ZIP_IN]++;

    if (GETSHORT(&aurp->sequence) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero sequence in ZI Request", idb_name);
	return;
    }

    if (GETSHORT(&aurp->flags) != 0) {
	if (do_debug)
	    buginf("\nAT: %s: Non-zero flags in ZI Request", idb_name);
	return;
    }

    switch (GETSHORT(&aurp_data->subcode)) {
    case ZIREQ:
	handle_ZIReq(aurp_info, pak);
	break;
    case GDZLREQ:
	handle_GDZLReq(aurp_info, GETSHORT(&aurp_data->index));
	break;
    case GZNREQ:
	handle_GZNReq(aurp_info, pak);
	break;
    default:
	if (aurp_info->neighbor)
	    aurp_info->neighbor->zipunknown++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: %s: ZI Request unknown subcode (%d), ignored",
		   idb_name, GETSHORT(&aurp_data->subcode));
	break;
    }
}

static void
handle_aurp_packet (aurpOps_t *ops)
{
    aurp_t *      aurp = (aurp_t *) ops->pak->network_start;

    switch(GETSHORT(&aurp->command)) {
    case RI_REQ:
	handle_RIReq(ops);
	break;
    case RI_RSP:
	handle_RIRsp(ops);
	break;
    case RI_ACK:
	handle_RIAck(ops);
	break;
    case RI_UPD:
	handle_RIUpd(ops);
	break;
    case RD:
	handle_RD(ops);
	break;
    case ZIP_REQ:
	handle_ZIPRequest(ops);
	break;
    case ZIP_RSP:
	handle_ZIPResponse(ops);
	break;
    case OPEN_REQ:
	handle_OpenReq(ops);
	break;
    case OPEN_RSP:
	handle_OpenRsp(ops);
	break;
    case TICKLE:
	handle_Tickle(ops);
	break;
    case TICKLE_ACK:
	handle_TickleAck(ops);
	break;
    default:
    }
    datagram_done(ops->pak);
}

/*
 * aurp_HandlePorts
 *
 * Runs port fsms as long as underlying tunnel interface is up.
 */
static void
aurp_HandlePorts (void)
{
    tunnel_info * tinfo;
    at_aurp_info *aurp;

    FOR_ALL_AURP_TUNNELS(tinfo, aurp) {
	if (atalkif_InterfaceUp(aurp->idb)) {
	    if (aurp->enabled) {
		/* We're running.  Nothing special to do. */
	    } else {
		if (!tinfo->destination || !(tinfo->source ||
					     tinfo->source_idb))
		    continue;

		/* Set up di's */
		setup_di(tinfo, aurp);

		aurp->enabled = TRUE;
		errmsg(AT_ERR_INTUP, idb_get_namestring(aurp->idb->phys_idb));
	    }
	    aurp_fsm(&aurp->receiver_fsm, ATFSM_NILEVENT);
	    aurp_fsm(&aurp->sender_fsm, ATFSM_NILEVENT);
	} else {		/* Interface is not up */
	    if (aurp->enabled) {
		aurp_ClearPathsFor(aurp->idb);
		aurp_Reset(&aurp->receiver_fsm);
		aurp_Reset(&aurp->sender_fsm);
		aurp_UpdateNeighborEntry(aurp, neighborDown);
		aurp->enabled = FALSE;
	    } else {

	    }
	}
    }
}

void
aurp_Disconnect (at_fsm *fsm)
{
    atfsm_clear_stateQ(fsm);
    aurp_fsm(fsm, AURP_FSM_DISCONNECT);
}

void
aurp_Reset (at_fsm *fsm)
{
    atfsm_clear_stateQ(fsm);
    aurp_fsm(fsm, AURP_FSM_RESET);
}

static void 
aurp_coming_up (idbtype *swidb)
{
    atalkidbtype *idb;

    idb = atalk_getidb(swidb);
    if (!idb)
        return;
    
    if (atalk_DomainRouter && idb->atalk_dminfo) {
	idb->atalk_operational = TRUE;
	atdomain_InitialAdjust(idb);    
    }
}

static void 
aurp_going_down (idbtype *swidb)
{
    atalkidbtype *idb;

    idb = atalk_getidb(swidb);
    if (!idb)
        return;

    if (atalk_DomainRouter && idb->atalk_dminfo) {
	idb->atalk_operational = FALSE;
	atdomain_DisablePort(idb);    
    }
}


/*
 * aurpif_clear
 *
 * Called whenever an interface is cleared.
 */
static void
aurpif_clear (idbtype *idb)
{
    at_aurp_info *aurp_info;
    hwidbtype *   hwidb;
    tunnel_info * tinfo;
    atalkidbtype *atalkidb;
    

    hwidb = hwidb_or_null(idb);
    if (!hwidb)
	return;

    tinfo = hwidb->tunnel;
    if (!tinfo)
	return;

    aurp_info = tinfo->aurp;
    if (!aurp_info)
	return;

    atalkidb = aurp_info->idb;
    
    errmsg(AT_ERR_INTCLEARED, idb_get_namestring(atalkidb->phys_idb));
    aurp_info->r_error = NORMALCLOSE;
    aurp_info->s_error = NORMALCLOSE;
    aurp_Disconnect(&aurp_info->sender_fsm);
    aurp_Disconnect(&aurp_info->receiver_fsm);
    aurp_ClearPathsFor(atalkidb);
    setup_di(tinfo, aurp_info);

    if (atalk_DomainRouter && atalkidb->atalk_dminfo &&
	atalkidb->atalk_operational)
	atdomain_InitialAdjust(atalkidb);
}

static boolean
ataurp_redist_path (atalk_pdb_t *p)
{
    /* Never redist our own paths to ourselves. */
    if ((p == NULL) || p->type == AT_PATH_AURP)
	return (FALSE);

    if (p->flags & AT_ROUTE_LEAKED_AURP) {
	/* Since path was already leaked to us, it's distance must have
	 * changed.  Queue NDC and check AURP database for any paths
	 * that should be put into the main routing table.
	 */
	queue_event(p, NDC_EVENT);
	aurp_AddNewBestPath(p->dstrngstr, p->dstrngend);
    } else {
	queue_event(p, NA_EVENT);
    }
    return (TRUE);
}

static void
ataurp_unredist_path (atalk_pdb_t *p)
{
    atalk_rdb_t * r;
    rbTree *      r_tree;

    /* Never redist our own paths to ourselves. */
    if ((p == NULL) || (p->type == AT_PATH_AURP))
	return;

    /* Path is going down so check our database for an alternative path
     * and inform main routing module.
     */
    aurp_AddNewBestPath(p->dstrngstr, p->dstrngend);

    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, p->dstrngstr, FALSE);
    if (r && r->pathcount > 0 && r->rpath.type == AT_PATH_AURP)
	queue_event(p, NRC_EVENT);
    else
	queue_event(p, ND_EVENT);
}

static void
ataurp_redist_all (boolean distflag)
{
    rbTree	* tree;
    
    tree = atroute_GetPTree(AT_PATH_AURP);
    if (tree == NULL)
	return;

    RBTreeForEachNode(atroute_path_redist_rbshell, (void *)distflag,
		      tree, FALSE);
}

/*******************************************************************
 * Show commands support routines
 *******************************************************************/

static boolean
aurp_show_interface (atalkidbtype *idb)
{
    at_aurp_info *aurp_info;

    if (!atalkif_AURPTunnel(idb))
	return (FALSE);

    aurp_info = idb->phys_idb->hwptr->tunnel->aurp;

    printf("\n  AppleTalk port is an AURP Tunnel");

    printf("\n    AURP Data Sender state is %s",
	   aurp_showstatusStr(atfsm_get_state(&aurp_info->sender_fsm)));
    printf("\n    AURP Data Receiver state is %s",
	   aurp_showstatusStr(atfsm_get_state(&aurp_info->receiver_fsm)));
    printf("\n    AURP Tickle Time interval is %d seconds",
	   aurp_info->r_tickletime / ONESEC);

    return (TRUE);
}

static void
aurp_show_topology_detail (aurp_pathentry *pathentry)
{
    aurp_pdb_t *p, *np, *sentinel;
    boolean       done = FALSE;

    printf("\n%#A", atutil_CableRange(pathentry->dstrngstr,
				      pathentry->dstrngend));

    sentinel = &pathentry->pathlist;
    p = aurp_NextPdb(sentinel);

    if (p == sentinel)
	return;

    while (!done) {
	inline_mem_lock(p);

	printf("\n             via %s, %d hop%s",
	       idb_get_namestring(p->idb->phys_idb),
	       ArgAndPlural(p->metric.u.rtmp.hops, "", "s"));

	np = aurp_NextPdb(p);

	if (np == sentinel || np == p || np == NULL)
	    done = TRUE;
	inline_mem_unlock_or_free(p);
	p = np;
    }
}
    
void
aurp_show_topology (ushort net)
{
    aurp_pathentry *
                  pathentry;

    if (net) {
	pathentry = aurp_FindPathentry(net, 0);
	if (pathentry) {
	    aurp_ProtectPathentry(pathentry);
	    aurp_show_topology_detail(pathentry);
	    aurp_UnProtectPathentry(pathentry);
	}
	return;
    } else {

	for (pathentry = KeepFirstPathentry(aurp_path_db); pathentry;
	     pathentry = KeepNextPathentry(aurp_path_db, pathentry)) {
	    aurp_show_topology_detail(pathentry);
	}

    }
}

/*
 * aurp_show_event_detail
 *
 * Called by aurp_show_events.  Prints out information about a pending
 * update event. 
 */
static void
aurp_show_event_detail (EventEntry *event)
{
    EventCode     type;

    type = event->eventtype;

    printf("\n%#A, %s pending", atutil_CableRange(event->dstrngstr,
						  event->dstrngend),
	   event_Str(type));
    if (type != ND_EVENT && type != NRC_EVENT && type != ZC_EVENT)
	printf(", %d hops", event->dist);
}

/*
 * aurp_show_events
 *
 * Show information about pending update event, net.  If net == 0,
 * show all pending update events.
 */
void
aurp_show_events (ushort net)
{
    EventEntry *  event;

    if (!pending_events_by_net)
        return;

    if (net) {
	event = event_Cast(RBTreeIntSearch(pending_events_by_net,
					   IntervalKey(net, 0)));
	if (event) {
	    aurp_ProtectEvent(event);
	    aurp_show_event_detail(event);
	    aurp_UnprotectEvent(event);
	}
    } else {
	for (event = KeepFirstEvent(pending_events_by_net); event;
	     event = KeepNextEvent(pending_events_by_net, event)) {
	    aurp_show_event_detail(event);
	}
				    
    }
}

static void
show_aurp_globals (void)
{
    printf("\n  AURP updates will be sent every %d seconds",
	   aurp_eventUpdateInterval);
}

void
static aurp_show_traffic (void)
{
    printf("\n  AURP: %d Open Requests, %d Router Downs",
	   atalk_stat[ATALK_AURP_OPEN_REQ], atalk_stat[ATALK_AURP_RD]);
    printf("\n        %d Routing Information sent, %d Routing Information "
	   "received", atalk_stat[ATALK_AURP_RI_OUT],
	   atalk_stat[ATALK_AURP_RI_IN]);
    printf("\n        %d Zone Information sent, %d Zone Information received",
	   atalk_stat[ATALK_AURP_ZI_OUT], atalk_stat[ATALK_AURP_ZI_IN]);
    printf("\n        %d Get Zone Nets sent, %d Get Zone Nets received",
	   atalk_stat[ATALK_AURP_GZN_OUT], atalk_stat[ATALK_AURP_GZN_IN]);
    printf("\n        %d Get Domain Zone List sent, %d Get Domain Zone List "
	   "received", atalk_stat[ATALK_AURP_GDZL_OUT],
	   atalk_stat[ATALK_AURP_GDZL_IN]);
    printf("\n        %d bad sequence",
	   atalk_stat[ATALK_AURP_BAD_SEQ]);
}   

static void
aurp_show_private_interface (atalkidbtype *idb)
{
    at_aurp_info *aurp_info;
    
    if (!atalkif_AURPTunnel(idb))
	return;
    
    aurp_info = idb->phys_idb->hwptr->tunnel->aurp;
    
    printf("\n           AURP: DR: id %#x, seq %#x, lastheard %#Ta(%d secs ago)",
	   aurp_info->r_connid, aurp_info->r_sequence, aurp_info->r_lastheard,
	   ELAPSED_TIME(aurp_info->r_lastheard)/ONESEC);
    printf("\n                 DS: id %#x, seq %#x",
	   aurp_info->s_connid, aurp_info->s_sequence);
    
}

static void
aurp_register_thyself (void)
{
    reg_add_tunnel_setmode(TUN_MODE_AURP_UDP, aurp_udp_setmode,
			   "aurp_udp_setmode");
    reg_add_ip_udp_input(aurp_udp_decaps, "aurp_udp_decaps");
    reg_add_atalk_aurp_FindPathentry((void *) aurp_FindPathentry, 
						"aurp_FindPathentry");
    reg_add_atalk_aurpif_clear(aurpif_clear, "aurpif_clear");
    reg_add_atalk_show_traffic(aurp_show_traffic, "aurp_show_traffic");
}

/*
 * aurp_init
 *
 * Perform aurp registration.
 *
 */
void
aurp_init (void)
{
    aurp_register_thyself();

    aurp_eventUpdateInterval = AURP_DEFAULT_UPDATE_RATE;

    pending_events_count = 1;

    mgd_timer_init_parent(&aurp_timer, NULL);
    mgd_timer_init_leaf(&aurp_sched_time, &aurp_timer, ATTIMER_AURP_FSM,
			NULL, FALSE);
    mgd_timer_init_leaf(&aurp_update_eventsTimer, &aurp_timer,
			ATTIMER_AURP_EVENTS, NULL, FALSE);
    mgd_timer_init_leaf(&aurp_zoneRequestTime, &aurp_timer,
			ATTIMER_AURP_ZONES, NULL, FALSE);
    mgd_timer_init_leaf(&aurp_pathGCTime, &aurp_timer,
			ATTIMER_AURP_PATHGC, NULL, FALSE);
    mgd_timer_init_leaf(&aurp_pathentryTimer, &aurp_timer,
			ATTIMER_AURP_PATHENTRY, NULL, FALSE);
    mgd_timer_init_leaf(&aurp_pathAgerTime, &aurp_timer,
			ATTIMER_AURP_PATHAGER, NULL, FALSE);

    aurp_EventEntry_gccontext = atmaint_AddGCContext(aurp_EventEntryGC, 0);
    aurp_PathEntry_gccontext = atmaint_AddGCContext(aurp_PathEntryGC, 0);
}

/*
 * aurp_start
 * 
 * Start the aurp process if it's not already running.  Bulk of routine
 * is run only once.
 */
static void
aurp_start (void)
{
    static boolean 
                  aurp_started = FALSE; 

    if (aurp_started)
	return;

    aurp_started = TRUE;
    pending_events_by_net = RBTreeCreate("Appletalk", "AT",
					 "Pending AURP Events",
					 at_print_key_interval,
					 &appletalk_debug->flag);
    aurp_path_db = RBTreeCreate("AppleTalk", "AT", "AURP Path database",
				at_print_key_interval,
				&appletalk_debug->flag);
    atroute_InitPathTree(AT_PATH_AURP, "AURP Path Tree");
    atroute_RegisterProtocol(AT_PATH_AURP,
			     AT_ROUTE_LEAK_AURP,
			     AT_ROUTE_UNLEAK_AURP,
			     AT_ROUTE_LEAKED_AURP,
			     ataurp_redist_path,
			     ataurp_unredist_path,
			     ataurp_redist_all,
			     aurp_AddNewBestPath);
    pending_events_by_count.dqueue.flink = &pending_events_by_count.dqueue;
    pending_events_by_count.dqueue.blink = &pending_events_by_count.dqueue;

    /* Seed connection id */
    next_id = atalk_random(AURP_MAX_CONNECTION_ID + 1);

    mgd_timer_start(&aurp_update_eventsTimer,
		    aurp_eventUpdateInterval * ONESEC);

    aurp_pathGCInterval = ONEMIN;
    mgd_timer_start(&aurp_pathGCTime, aurp_pathGCInterval);

    mgd_timer_start(&aurp_pathentryTimer, AURP_PATHENTRY_INTERVAL);

    mgd_timer_start(&aurp_pathAgerTime, AURP_PATHAGER_INTERVAL);

    /* These registrations only make sense after the AURP process has 
     * started.
     */
    reg_add_reset_if(aurpif_clear, "aurpif_clear");
    reg_add_atalk_show_globals(show_aurp_globals, "show_aurp_globals");
    reg_add_atalk_show_interface_special(aurp_show_interface,
					 "aurp_show_interface");
    reg_add_atalk_show_private_interface(aurp_show_private_interface,
					 "aurp_show_private_interface");
    reg_add_atalk_validate_route(aurp_AddNewBestPath, "aurp_AddNewBestPath");
    
    reg_add_swif_comingup(aurp_coming_up, "No shut for aurp tunnel");
    reg_add_swif_goingdown(aurp_going_down, "shut for aurp tunnel");
    aurp_pid = process_create(aurp_process, "AURP Service", NORMAL_STACK,
			      PRIO_NORMAL);
}

/*
 * Start/stop AURP on a tunnel.
 */
boolean
aurp_config_tunnel (atalkidbtype *idb, boolean enable, di_type type)
{
    at_aurp_info *aurp_info;
    tunnel_info * tinfo = idb->phys_idb->hwptr->tunnel;
    
    if (!tinfo)
	return (FALSE);

    if (enable) {

	if (!tinfo->aurp) {
	    if ((aurp_info = malloc(sizeof(at_aurp_info))) == NULL) {
		printf(nomemory);
		return (FALSE);
	    }
	    aurp_info->idb = idb;
	    aurp_info->ditype = type;
	    aurp_info->enabled = FALSE;
	    aurp_info->r_tickletime = AURP_DEFAULT_TICKLETIME;

	    aurp_info->s_last_sent_update = 0;
	    tinfo->aurp = aurp_info;

	    /* Initialize state machines */
	    aurp_fsm_init(&aurp_info->sender_fsm, (char *) aurp_info);
	    aurp_fsm_init(&aurp_info->receiver_fsm, (char *) aurp_info);
	}

	aurp_start();			/* Start aurp process, if necessary. */

	/* configure for AppleTalk */
	idb->atalk_cablestart = idb->atalk_cableend = ATALK_NET_UNNUMBERED;
	idb->atalk_enabled = TRUE;
	idb->atalk_operational = TRUE;
	idb->aarp_tentative = AARP_ADDR_SET;
	idb->atalk_counter = -1;	/* Don't run AT FSM */

	if (atalk_DomainRouter && idb->atalk_dminfo)
	    atdomain_InitialAdjust(idb);
    } else {

	if (!tinfo->aurp)
	    return (FALSE);		/* AURP not configured on interface! */
	
	aurp_info = tinfo->aurp;
	aurp_Disconnect(&aurp_info->sender_fsm);
	aurp_Disconnect(&aurp_info->receiver_fsm);
	aurp_ClearPathsFor(aurp_info->idb);
	aurp_FreeResources(tinfo->aurp);
	free(tinfo->aurp);
	tinfo->aurp = NULL;
	/* reset everything */
	idb->atalk_enabled = FALSE;
	idb->atalk_operational = FALSE;
	if (atalk_DomainRouter && idb->atalk_dminfo)
	    atdomain_DisablePort(idb);
    }
    return (TRUE);
}

static void
aurp_process_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&aurp_timer, DISABLE);
    process_watch_queue(atalkaurpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalkaurpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalkaurpQ);
    aurp_pid = 0;
}

/*
 * aurp_process 
 *
 * Process AURP packets, execute interface state machines, handle path
 * aging garbage collection, etc.
 */
static process
aurp_process (void)
{
    paktype    *  pak;
    aurpOps_t	  ops;
    ulong         major, minor;
    mgd_timer  *  expired_timer;
    
    /* wait for system initialization */
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, aurp_process_teardown);
    atalkaurpQ = create_watched_queue("AURP packets", 0, 0);
    process_watch_queue(atalkaurpQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&aurp_timer, ENABLE);
    
    /* Defer first fsm run */
    mgd_timer_start(&aurp_sched_time, AURP_SCHED_DELAY);
    
    while (TRUE) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		while (mgd_timer_expired(&aurp_timer)) {
		    expired_timer = mgd_timer_first_expired(&aurp_timer);
		    switch (mgd_timer_type(expired_timer)) {
		    case ATTIMER_AURP_FSM:
			/* Run state machines */
			aurp_HandlePorts();
			mgd_timer_start(expired_timer, AURP_SCHED_DELAY);
			break;
		    case ATTIMER_AURP_EVENTS:
			aurp_process_events(); /* Send any pending update events */
			aurp_gc_events(); /* Garbage collect old updates */
			mgd_timer_start(expired_timer,
					aurp_eventUpdateInterval * ONESEC);
			break;
		    case ATTIMER_AURP_ZONES:
			aurp_SendQueries(); /* Send zone requests */
			break;
		    case ATTIMER_AURP_PATHGC:
			atroute_pathGC(AT_PATH_AURP, "AURP");
			mgd_timer_start(expired_timer, aurp_pathGCInterval);
			break;
		    case ATTIMER_AURP_PATHENTRY:
			pathentry_Ager(); /* Age pathentries */
			mgd_timer_start(expired_timer, AURP_PATHENTRY_INTERVAL);
			break;
		    case ATTIMER_AURP_PATHAGER:
			aurp_pathAger(); /* Age AURP paths in main routing table */
			mgd_timer_start(expired_timer, AURP_PATHAGER_INTERVAL);
			break;
		    default:
			mgd_timer_stop(expired_timer);
			break;
		    }
		}
		break;
	    case QUEUE_EVENT:
		/* Dequeue packets and process */
		while ((pak = process_dequeue(atalkaurpQ)) != NULL) {
		    if (!aurp_aurp2ops(pak, &ops)) {
			atalk_dispose(ATALK_MARTIAN, pak);
			continue;
		    }

		    if (!aurp_validate(&ops)) {
			atalk_dispose(ATALK_MARTIAN, pak);
			continue;
		    }

		    /* Routine is expected to dipose of packet after use. */
		    aurp_classify_packet(&ops);
		}
		
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * aurp_fsm
 *
 * Inform finite state machine, fsm, of the event, evt. 
 */
void
aurp_fsm (at_fsm *fsm, atfsmEvent evt)
{
    atfsmResult   result;

    if (evt != ATFSM_NILEVENT)
	atfsm_add_event(fsm, evt, 0);
    result = atfsm_exec_fsm(fsm);

    switch (result) {
    case ATFSM_EXEC_OK:
	break;
    case ATFSM_HALTED:
	errmsg(AT_ERR_SUBROUTINE_ERROR, "AURP: fsm halted");
	break;
    case ATFSM_BAD_EVENT:
	errmsg(AT_ERR_SUBROUTINE_ERROR, "AURP: illegal event %d",
	       evt);
    default:
	errmsg(AT_ERR_SUBROUTINE_ERROR,
	       "AURP: unknown fsm return code %d for %s", result,
	       aurp_eventStr(evt));
    }
}

/*******************************************************************
 * Zone request handling
 *******************************************************************/

/*
 * aurp_ScheduleZoneRequest
 *
 * Queue a zone request to be sent for this net.  A timer is set indicating 
 * when the actual request should be sent.  For routes learned via a
 * RI-Rsp or RI-Upd, the request will very likely never be sent since
 * the zone list will usually be acquired from the SZI piggybacked
 * onto the RI-Ack. 
 */
static void
aurp_ScheduleZoneRequest (atalk_rdb_t *r)
{
    atmaint_ScheduleZoneRequestShell(r, &maint_pendingZoneRequests);

    /* Set time to send query for this route.  Hopefully, request will
     * have already been fulfilled by response to piggy backed ZI-Req in
     * the RI-Ack.
     */
    TIMER_START(r->zonetimer, atalk_zipQueryInterval);
    if (!mgd_timer_running(&aurp_zoneRequestTime))
	mgd_timer_start(&aurp_zoneRequestTime, atalk_zipQueryInterval);
}

/*
 * aurp_SetRequestTime
 *
 * Mark the next time a zone request should be sent for this net.
 * Also, sets or updates the next time the zone request processing
 * should be done.
 */
static void
aurp_SetRequestTime (atalk_rdb_t *r)
{
    int           t;
    
    t = ++r->zoneretries;
    if (t > MAX_ZIP_RETRY_MULTIPLIER)
	t = MAX_ZIP_RETRY_MULTIPLIER;

    TIMER_START(r->zonetimer, t * atalk_zipQueryInterval);

    /* Set aurp_zoneRequestTime to the sooner of itself, if running,
     * or r->zonetimer. 
     */
    mgd_timer_set_soonest(&aurp_zoneRequestTime, r->zonetimer);
}

/*
 * aurp_SendQueries
 *
 * Scan pending zone request list and send out ZIP Requests for all
 * nets without a zone.  
 */
static void
aurp_SendQueries (void)
{
    neighborentry *
                  n;
    atalk_rdb_t * r;
    atalk_rdb_t * nr;
    int           pcnt;
    int           stat;

    pcnt = 0;
    mgd_timer_stop(&aurp_zoneRequestTime);
    r = atmaint_GetFirstRequest();
    while (r != NULL) {
	nr = atmaint_GetNextRequest(r);

	if (!atalkif_AURPTunnel(r->rpath.gwptr->port)) {
	    r = nr;
	    continue;
	}

	if ((!r->zoneupdate && ((atalk_firstZone(r) != NULL)))) {
	    atmaint_CancelZoneRequest(r);
	    r = nr;
	    continue;
	}

	if (r->zoneupdate && !SLEEPING(r->zonetimer)) {
	    if ((r->zonecnt != 0) && (r->zoneretries <= 1)) {
		aurp_SendQuery(r);
		r = nr;
		continue;
	    }
	    atmaint_ResetRequestInfo(r);
	}
	if (!r->zoneupdate && (r->zoneretries > MAX_ZIP_GWTRIES))
	    aurp_SendQuery(r);
	else if (!r->zoneupdate)
	    pcnt++;
	r = nr;
    }
    if (pcnt == 0)
	return;

    for (n = atmaint_GetNextNeighbor(NULL); n != NULL;
	 n = atmaint_GetNextNeighbor(n)) {
	if (!atalkif_AURPTunnel(n->port))
	    continue;
	if (n->state != neighborUp)
	    continue;
	if (TIMER_RUNNING_AND_SLEEPING(n->qtimer))
	    continue;
	TIMER_STOP(n->qtimer);
	pcnt = 0;
	atmaint_ProtectNeighbor(n);
	while (pcnt < NEIGHBOR_PACKET_LIMIT) {
	    stat = aurp_SendNeighborQueries(n);
	    if (stat != 0)
		pcnt++;
	    if (stat <= 0)
		break;
	}
	if (pcnt) {
	    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb))
		buginf("\nAT: %s: Sent %d query packets to AURP neighbor",
		       idb_get_namestring(n->port->phys_idb), pcnt);
	}
	atmaint_UnprotectNeighbor(n);
    }

    /* If there are still pending zone request, schedule another run */
    if (maint_pendingZoneRequests &&
	!mgd_timer_running_and_sleeping(&aurp_zoneRequestTime))
	mgd_timer_start(&aurp_zoneRequestTime, atalk_zipQueryInterval);
}

/*
 * aurp_SendNeighborQueries
 *
 * Send a ZI Request to a neighbor.  The ZI Request contains
 * NEIGHBOR_QUERIES_PER_PACKET requests.
 */
static int
aurp_SendNeighborQueries (neighborentry *n)
{
    paktype *     pak;
    atalk_rdb_t * p;
    at_aurp_info *aurp_info;
    int           qcnt;
    sys_timestamp timeo;
    int           bytes;
    hwidbtype *   hwidb;

    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nSending neighbor ZIP Queries");

    hwidb = hwidb_or_null(n->port->phys_idb);
    if (!hwidb)
	return (FALSE);

    aurp_info = hwidb->tunnel->aurp;

    p = atmaint_NextNeighborZipQuery(n, NULL);
    if (!p)
	return (FALSE);

    pak = NULL;
    qcnt = 0;
    timeo = n->qtimer;
    bytes = 0;

    while ((p != NULL) && (qcnt++ < NEIGHBOR_QUERIES_PER_PACKET)) {
	aurp_add_query(aurp_info, &pak, &bytes, p->dstrngstr);
	aurp_SetRequestTime(p);

	/* Set timeo to the sooner of itself, if running, or p->zonetimer. */
	timeo = TIMER_SOONEST(timeo, p->zonetimer);

	p = atmaint_NextNeighborZipQuery(n, p);
    }
    if (aurp_FinishZIPReq(pak, aurp_info->idb, bytes, ZIREQ,
			  aurp_info->r_connid)) {
	n->qcount += qcnt;
	n->qtimer = timeo;	/* Set next time to query this neighbor */
	return (p == NULL) ? -1 : 1;
    }
    return (FALSE);
}

/*
 * aurp_SendQuery
 *
 * Send a ZI Request for a single net.
 */
static void
aurp_SendQuery (atalk_rdb_t *r)
{
    paktype *     pak;
    hwidbtype	 *idb;
    int           bytes;
    at_aurp_info *aurp_info;

    pak = NULL;
    bytes = 0;

    if (!atalkif_AURPTunnel(r->rpath.idb))
	return;

    idb = hwidb_or_null(r->rpath.idb->phys_idb);
    if (idb == NULL)
	return;
    aurp_info = idb->tunnel->aurp;

    aurp_add_query(aurp_info, &pak, &bytes, r->dstrngstr);

    aurp_FinishZIPReq(pak, aurp_info->idb, bytes, ZIREQ, aurp_info->r_connid);

    aurp_SetRequestTime(r);
}

static int
aurp_ActiveConnections (void)
{
    int count = 0;
    tunnel_info *tinfo;
    at_aurp_info *aurp;
    FSMFUNC curstate;

    FOR_ALL_AURP_TUNNELS(tinfo, aurp) {
	curstate = atfsm_get_state(&aurp->sender_fsm);
	if (curstate == aurp_fsm_DS_Connected
	    || curstate == aurp_fsm_DS_RI_Ack_Wait1
	    || curstate == aurp_fsm_DS_RI_Ack_Wait2
	    || curstate == aurp_fsm_DS_RI_Ack_Wait4)
	    count++;
    }
    return (count);
}

/*******************************************************************
 * Miscellaneous routines
 *******************************************************************/

/* 
 * copy_di
 *
 * Copy a Domain Identifier.
 */
static void
copy_di (di_t const *from, di_t *to)
{
    switch (from->generic.authority) {
    case AURP_IP_DI:
	memcpy(&to->ip, &from->ip, sizeof(di_t));	/* structure copy */
	break;
    case AURP_NULL_DI:
	to->null = from->null;
	break;
    default:
	break;
    }
}

static void
ip_to_di (ipaddrtype ipaddr, di_t *di)
{
    di->ip.length = DI_IP_LENGTH;
    di->ip.authority = AURP_IP_DI;
    PUTSHORT(&di->ip.reserved, 0);
    di->ip.addr = ipaddr;
}

static boolean
isequal_di (di_t const *x, di_t const *y)
{
    switch (x->generic.authority) {
    case AURP_IP_DI:
	if (x->ip.length != y->ip.length)
	    return (FALSE);
	if (x->ip.authority != y->ip.authority)
	    return (FALSE);
	if (x->ip.addr != y->ip.addr)
	    return (FALSE);
	break;
    case AURP_NULL_DI:
	if (x->null.length != y->null.length)
	    return (FALSE);
	if (x->null.authority != y->null.authority)
	    return (FALSE);
	break;
    default:
	errmsg(AT_ERR_SUBROUTINE_ERROR, "Unknown Domain Identifier Authority");
	return (FALSE);
	break;
    }
    return (TRUE);
}

static void
setup_di (tunnel_info *tinfo, at_aurp_info *aurp)
{
    switch (aurp->ditype) {
    case AURP_IP_DI:
	ip_to_di(tinfo->source_idb
		 ? idb_sw_get_ipaddress(tinfo->source_idb)
		 : tinfo->source, &aurp->local_di);
	ip_to_di(tinfo->destination, &aurp->remote_di);
	break;
    case AURP_NULL_DI:
	aurp->local_di.null.length = DI_NULL_LENGTH;
	aurp->local_di.null.authority = AURP_NULL_DI;
	aurp->remote_di.null.length = DI_NULL_LENGTH;
	aurp->remote_di.null.authority = AURP_NULL_DI;
	break;
    default:
	errmsg(AT_ERR_SUBROUTINE_ERROR,
	       "Unrecognized Domain Identifier type");
    }
}


/*
 * aurp_queueRIpkt
 *
 * Queue packet onto end of connection's private queue.
 */
static void
aurp_queueRIpkt (paktype *pak, at_aurp_info *aurp_info)
{
    paktype *     nextpak;

    pak->next = NULL;
    nextpak = aurp_info->pktqueue;
    if (nextpak == NULL) {
	aurp_info->pktqueue = pak;
	return;
    } else {
	/* Put packet on end of queue */
	while (nextpak->next != NULL)
	    nextpak = nextpak->next;
	nextpak->next = pak;
    }
}

