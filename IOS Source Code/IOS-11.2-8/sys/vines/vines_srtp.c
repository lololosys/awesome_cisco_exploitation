/* $Id: vines_srtp.c,v 3.11.6.5 1996/08/28 13:19:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_srtp.c,v $
 *------------------------------------------------------------------
 *	       VINES ROUTING UPDATE PROTOCOL, Version 1
 *		      a.k.a. Sequenced RTP (SRTP)
 *
 * July 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_srtp.c,v $
 * Revision 3.11.6.5  1996/08/28  13:19:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.6.4  1996/08/07  09:04:02  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.6.3  1996/07/30  07:02:43  ehk
 * CSCdi58129:  Vines SRTP: Router repeatedly advertises unreachable nets
 * Branch: California_branch
 * apply split-horizon when helping neighbors.
 *
 * Revision 3.11.6.2  1996/05/19  19:56:31  ehk
 * CSCdi58038:  Vines SRTP losing routes in some cases
 * Branch: California_branch
 * accept RTP updates with older sequence no. if the existing route
 * is unreachable i.e. in garbage collection state.
 *
 * Revision 3.11.6.1  1996/03/18  22:32:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  07:53:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  02:04:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  11:05:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:47:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/05  22:09:10  ehk
 * CSCdi50536:  Corrupted Vines SRTP redirects and invalid packet length
 * field
 * corrected SRTP redirect packet size and set the destination host id.
 *
 * Revision 3.10  1996/02/14  18:40:34  ehk
 * CSCdi45774:  Vines SRTP servers losing some routes
 * schedule an update to sync up routing table when an interface goes down.
 * changes to reject obsolete route information and always update sequence
 * number for existing routes.
 *
 * Revision 3.9  1996/02/08  20:14:16  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/07  02:20:17  slin
 * CSCdi48252:  Vines: Spurious memory access in vsrtp_update_route
 * - Fix NULL pointer dereference in vsrtp_update_route() and unaligned
 *   longword write in vrpc_send_returnaddress()
 *
 * Revision 3.7  1996/01/22  07:44:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/01  21:39:21  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.5  1995/12/01  19:22:26  slin
 * CSCdi44499: vines RTS_AGE timer should be reset to 4 when an update is
 *             received
 *
 * Revision 3.4  1995/11/21  01:24:10  slin
 * CSCdi43112:  SRTP goes into suppression when better metric learned
 * - Per VPD 5-109, set suppression interval only if already in suppression
 *
 * Revision 3.3  1995/11/17  17:58:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/10/19  23:41:30  slin
 * CSCdi41922:  Vines Router process suspends too frequently
 *
 * Revision 2.12  1995/09/15  20:33:53  slin
 * CSCdi40388:  show vines int crashes router
 * - Convert yet another routine to gather info into local buffer before
 *   printing
 *
 * Revision 2.11  1995/09/09  00:54:11  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.10  1995/08/25  11:45:39  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.9  1995/08/22 16:54:12  slin
 * CSCdi38016:  Vines redirect sent to MAC broadcast and network broadcast
 * - Send redirect to last forwarding node
 *
 * Revision 2.8  1995/08/22  00:53:40  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.7  1995/08/21  17:12:15  slin
 * CSCdi39054:  vines single-route sets wrong metric for routes
 * - Set route's metric to advertised metric, not actual metric
 *
 * Revision 2.6  1995/08/17  21:35:13  slin
 * CSCdi38846:  show vines int can halt system
 *
 * Revision 2.5  1995/08/12  00:18:50  slin
 * CSCdi38606:  Vines f/s cache not invalidated under certain circumstances
 *
 * Revision 2.4  1995/08/07  05:44:48  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.3  1995/07/24  22:33:02  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.2  1995/07/07 20:40:37  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.1  1995/06/07 23:15:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "../h/ieee.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../util/fsm.h"
#include "../vines/vines_private.h"
#include "vinesfast.h"
#include "../util/random.h"

boolean  vines_onlyone;
sys_timestamp vsrtp_reinit_check_time;
ulong    vsrtp_reinit_sequence;
ulong    vines_sequence;
ulong    vines_packet_id;
mgd_timer vsrtp_reassembly_timer;
mgd_timer vsrtp_retry_timer;
mgd_timer vsrtp_start_timer; /* When to start SRTP */
mgd_timer vsrtp_sync_timer;

/*
 * Counts kept solely for network management.  Vines NSM, specifically.
 * Some of them could be kept on a per interface basis, but it doesn't
 * seem like its worth the work.
 */
ulong vsrtp_rx_response_count;
ulong vsrtp_rx_request_lookup_count;
ulong vsrtp_rx_request_slr_count;
ulong vsrtp_rx_resync_count;
ulong vsrtp_rx_frag_rcvd;
ulong vsrtp_rx_frag_drop_dup;
ulong vsrtp_rx_frag_drop_oos;
ulong vsrtp_rx_frag_drop_old;
ulong vsrtp_reass_complete;
ulong vsrtp_reass_failed;
ulong vsrtp_rx_outdated;		/* msg has old sequence number */
ulong vsrtp_rx_oldnetinfo;		/* msg entry has old seq number */

ulong vsrtp_tx_response_count;
ulong vsrtp_tx_request_lookup_count;
ulong vsrtp_tx_request_slr_count;
ulong vsrtp_tx_resync_count;
ulong vsrtp_tx_frag_sent;		/* num frags created and sent */
ulong vsrtp_tx_frag_drop;		/* num frags created and not sent */
ulong vsrtp_frag_complete;		/* num messages fragmented */
ulong vsrtp_frag_failed;		/* num messages fragmentation failed */

/*
 *
 */
static char *const vsrtp_request_names[] = {"invalid", "specific", "change", 
                                            "full", "response"};
static char *const vsrtp_update_names[] = {"update", "response"};

/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vsrtp_init
 *
 * Perform any initialization that is unique to RTP v1.
 */
void vsrtp_init (void)
{
    mgd_timer_init_leaf(&vsrtp_start_timer, &vines_timer, VT_SRTP_START, NULL, FALSE);
    mgd_timer_init_parent(&vsrtp_reassembly_timer, &vines_timer);
    mgd_timer_init_parent(&vsrtp_retry_timer, &vines_timer);
    mgd_timer_init_parent(&vsrtp_sync_timer, &vines_timer);

    vsrtp_rx_response_count = 0;
    vsrtp_rx_request_lookup_count = 0;
    vsrtp_rx_request_slr_count = 0;
    vsrtp_rx_resync_count = 0;
    vsrtp_rx_frag_rcvd = 0;
    vsrtp_rx_frag_drop_dup = 0;
    vsrtp_rx_frag_drop_oos = 0;
    vsrtp_rx_frag_drop_old = 0;
    vsrtp_reass_complete = 0;
    vsrtp_reass_failed = 0;
    vsrtp_rx_outdated = 0;
    vsrtp_rx_oldnetinfo = 0;

    vsrtp_tx_response_count = 0;
    vsrtp_tx_request_lookup_count = 0;
    vsrtp_tx_request_slr_count = 0;
    vsrtp_tx_resync_count = 0;
    vsrtp_tx_frag_sent = 0;
    vsrtp_tx_frag_drop = 0;
    vsrtp_frag_complete = 0;
    vsrtp_frag_failed = 0;
}

/*
 * vsrtp_start
 *
 * Enable SRTP
 *
 */
void vsrtp_start (void)
{
    if (vinesrtp_debug)
	buginf("\nVSRTP: Starting Vines SRTP");
    
    vsrtp_enabled = TRUE;

    vines_sequence = random_gen();
    vsrtp_reinit_sequence = vines_sequence;
    TIMER_START(vsrtp_reinit_check_time, VSRTP_REINIT_CHECK);
    vines_neighbor_ager(TRUE);	/* Flush neighbor table */
    vroute_send_requests(NULL);	/* Send out SRTP full update request */
}

/*
 * vsrtp_schedule_start
 *
 * Attempt to start up SRTP.  Postpone if system recently came up.
 * This allows other systems to flush out information about us in the
 * advent of a system reboot. 
 */
void vsrtp_schedule_start (void)
{
    sys_timestamp wakeup_time;
    ulong minimum_uptime;
    
    minimum_uptime = VRTP_INVALIDTIME(VRTP_DEF_WAKEUP_INTERVAL) +
	VRTP_ROUTEEXPIRE(VRTP_DEF_WAKEUP_INTERVAL);
    if (!CLOCK_IN_STARTUP_INTERVAL(minimum_uptime)) {
	vsrtp_start();
	return;
    }

    TIMER_START_ABSOLUTE(wakeup_time, minimum_uptime);
    if (vinesrtp_debug)
	buginf("\nVSRTP: Scheduling Vines SRTP to start in %d seconds",
	       minimum_uptime/ONESEC);
    
    mgd_timer_start(&vsrtp_start_timer, TIME_LEFT_SLEEPING(wakeup_time));
}

void vsrtp_stop (void)
{
    mgd_timer_stop(&vsrtp_start_timer);

    if (vsrtp_enabled) {
	vsrtp_enabled = FALSE;
	vines_neighbor_ager(TRUE);
    }
}


/*****************************************************************************
 *
 *                          SRTP interface to VIP
 *
 *****************************************************************************/

/*
 * vsrtp_send_broadcast
 *
 * Send an VINES Sequenced RTP packet.  This may be directed to a single
 * station or a single interface if desired.
 */
boolean vsrtp_send_broadcast (
    paktype *pak,
    enum vsrtp_update_infotype type,
    vinesidbtype *target)
{
    vinesidbtype *idb;
    paktype *newpak;
    vsrtp_header *rtp;
    boolean sent_any = FALSE;

    pak->transport_start = pak->session_start - VSRTP_HEADER_BYTES;
    pak->datagramsize += VSRTP_HEADER_BYTES;

    rtp = (vsrtp_header *)pak->transport_start;
    rtp->version = VSRTP_VERSION;
    rtp->operation = type;
    rtp->node_type = VSRTP_NODE_ROUTER;
    rtp->flags = 0;
    rtp->reserved = 0;

    /*
     * Send it.
     */
    FOR_ALL_VIDBS(idb) {
	if (target && (target != idb))
	    continue;
	if (!vines_interface_up(idb))
	    continue;
	if ((newpak = pak_duplicate(pak)) == NULL)
	    continue;
	rtp = (vsrtp_header *)newpak->transport_start;
	rtp->flags = 0;
	if (vines_rtp_present(idb))
	    rtp->flags |= VSRTP_FLAG_OLD_PRESENT;
	if (do_vines_rtp_debug(vines_network, vines_host))
	    buginf("\nVSRTP: Sending %s to %#z on %s", vsrtp_names[type],
		   VINES_ALLNETS, VINES_ALLHOSTS, idb->phys_idb->namestring);
	vip_send_broadcast(newpak, idb, NULL, VINES_RTP, VINES_NOFLAGS, VINES_TTL0);
	idb->tx_rtpv1_type[type]++;
	idb->tx_rtpv1_bcast++;
	vroute_bcast_out++;
	sent_any = TRUE;
    }
    datagram_done(pak);
    return(sent_any);
}

/*
 * vsrtp_send_specific
 *
 * Send an VINES Sequenced RTP packet to a specific station.
 */
vinesidbtype *vsrtp_send_specific (
    paktype *pak,
    int type,
    vinespathtype *path)
{
    vinesidbtype *idb;
    vsrtp_header *rtp;

    pak->transport_start = pak->session_start - VSRTP_HEADER_BYTES;
    pak->datagramsize += VSRTP_HEADER_BYTES;

    rtp = (vsrtp_header *)pak->transport_start;
    rtp->version = VSRTP_VERSION;
    rtp->operation = type;
    rtp->node_type = VSRTP_NODE_ROUTER;
    rtp->flags = 0;
    if (vines_rtp_present(path->idb))
	rtp->flags |= VSRTP_FLAG_OLD_PRESENT;
    rtp->reserved = 0;

    /*
     * Send it.
     */
    if (do_vines_rtp_debug(vines_network, vines_host)) {
	vinesneighbortype *neighbor = path->neighbor;
	buginf("\nVSRTP: Sending %s to %#z on %s", vsrtp_names[type],
	       neighbor->net, neighbor->host, path->idb->phys_idb->namestring);
    }
    idb = vip_rtp_send_specific(pak, path, VINES_NOFLAGS);
    if (idb)
	idb->tx_rtpv1_type[type]++;
    return(idb);
}

/**********************************************************************
 *
 *		VSRTP Specific Timer Queue Manipulation
 *
 **********************************************************************/

/*
 * vsrtp_retry_timer_expired
 *
 * Process all events on a queue whose time has arrived.  Call the finite
 * state machine to handle each event.
 */
void vsrtp_retry_timer_expired (vinespathtype *path)
{
    vsrtp_fsm(NULL, path, vsrtp_timer, vsrtp_timer_retry);
}

/*
 * vsrtp_reassembly_timer_expired
 *
 * Process all events on the reassembly queue whose time has arrived.
 * Call the finite state machine to handle each event.
 */
void vsrtp_reassembly_timer_expired (vinespathtype *path)
{
    paktype *pak;
    vsrtp_header *srtp;
    vsrtp_update *update;
    int major, minor;
    ulong first, last;

    pak = path->reassemblyQ.qhead;
    srtp = (vsrtp_header *)pak->transport_start;
    update = (vsrtp_update *)srtp->data;
    major = vsrtp_decode_update_major(update);
    minor = vsrtp_decode_update_minor(update->flags, -1);
    vsrtp_fsm(NULL, path, major, minor);
    vsrtp_reass_failed++;
    while ((pak = dequeue(&path->reassemblyQ)) != NULL) {
	first = pak->unspecified[0];
	last = pak->unspecified[1];
	vsrtp_rx_frag_drop_old += (last - first)/VSRTP_UPD_FRAGENTRIES;
	datagram_done(pak);
    }
}


/**********************************************************************
 *
 * Fragmentation and Reassembly
 *
 **********************************************************************/

/*
 * vsrtp_fragment_update
 *
 * Given a monolithic routing update, fragment it into sendable packets
 * and enqueue those packets onto a queue.  This routine returns TRUE if
 * it was able to obtain enough buffers to completely fragment the
 * packet, FALSE otherwise.  This routine is calleld for all outgoing
 * updates, but it should  only increment the fragmentation counts if
 * the update requires more than one packet to be sent.
 */
boolean vsrtp_fragment_update (
    queuetype *queue,
    paktype *pak,
    enum vsrtp_update_infotype type,
    ushort metric,	/* internal form */
    uchar flags)
{
    paktype *fragment;
    vsrtp_update *update, *frag_update;
    int i, size, size_left;
    ulong id;
    uchar *data;

    memset(queue, 0, sizeof(queuetype));
    size_left = pak->datagramsize - VSRTP_UPDATE_BYTES;
    update = (vsrtp_update *)pak->session_start;
    data = update->data;
    id = ++vines_packet_id;
    i = 0;

    do {
	fragment = vsrtp_getbuffer("update fragment", VSRTP_UPDATE_BYTES +
				   VSRTP_UPD_TOTALBYTES);
	if (!fragment) {
	    if (queue->count > 1)
		vsrtp_frag_failed++;
	    while ((fragment = dequeue(queue)) != NULL)
		retbuffer(fragment);
	    retbuffer(pak);
	    return(FALSE);
	}

	frag_update = (vsrtp_update *)fragment->session_start;
	frag_update->type        = type;
	frag_update->flags       = flags;
	if (i == 0)
	    frag_update->flags  |= VSRTP_UPD_FLAG_BOM;
	if (size_left <= VSRTP_UPD_FRAGBYTES)
	    frag_update->flags  |= VSRTP_UPD_FLAG_EOM;
	frag_update->packet_id   = id;
	frag_update->data_offset = i * VSRTP_UPD_FRAGENTRIES;
	PUTLONG(&frag_update->sequence, vines_sequence);
	frag_update->metric      = vmetric_to_srtp(metric);
	size = min(size_left, VSRTP_UPD_FRAGBYTES);
	fragment->datagramsize = VSRTP_UPDATE_BYTES + size;
	bcopy(data, frag_update->data, size);
	enqueue(queue, fragment);

	size_left -= size;
	data += size;
	i++;
    } while (size_left > 0);
    if (queue->count > 1)
	vsrtp_frag_complete++;
    retbuffer(pak);
    return(TRUE);
}

/*
 * vsrtp_send_fragments
 *
 * Given a queue of packet fragments, send them with the appropriate
 * IP layer header.  Make sure to count them too.
 */
void vsrtp_send_fragments (
    queuetype *queue,
    vinespathtype *path,
    vinesidbtype *bcast_idb,
    boolean response)
{
    boolean fragmented, sent;
    vsrtp_update *update;
    vsrtp_header *rtp;
    vinesidbtype *idb;
    paktype *pak;
    boolean do_debug;

    do_debug = do_vines_rtp_v_debug(vines_network, vines_host);
    fragmented = (queue->count > 1);
    while ((pak = dequeue(queue)) != NULL) {
	if (do_debug) {
	    rtp = (vsrtp_header *)pak->transport_start;
	    update = (vsrtp_update *)rtp->data;
	    buginf("\n    update: type %02x, flags %02x, id %04x, ofst %04x, seq %08x, met %04x",
		   update->type, update->flags, update->packet_id,
		   update->data_offset, update->sequence,
		   vmetric_from_srtp(update->metric));
	}
	if (path) {
	    idb = vsrtp_send_specific(pak, VSRTP_UPDATE, path);
	    sent = (idb != NULL);
	    if (response)
		path->srtp_tx_resp_count++;
	} else {
	    sent = vsrtp_send_broadcast(pak, VSRTP_UPDATE, bcast_idb);
	}
	if (fragmented) {
	    if (sent) {
		vsrtp_tx_frag_sent++;
	    } else {
		vsrtp_tx_frag_drop++;
	    }
	}
    }
}

/*
 * vsrtp_reassemble1
 *
 * Given a path instance and a fragment of a routing update, put it onto
 * the reassembly queue in the proper location.  This routine also merges
 * adjacent sets of tuples into a single larger set.
 */
paktype *vsrtp_reassemble1 (
    vinespathtype *path,
    paktype *pak,
    vsrtp_update *update,
    vsrtp_reass_type *result)
{
    vinesiptype *vip, *saved_vip;
    queuetype *queue;
    uchar *from, *to;
    int first, last, size;
    paktype *saved, *prev, *next, *temp;
    vsrtp_header *saved_rtp, *next_rtp;
    vsrtp_update *saved_update, *next_update;
    boolean do_debug;

    /*
     * Initialize
     */
    vip = (vinesiptype *)pak->network_start;
    queue = &path->reassemblyQ;
    do_debug = do_vines_rtp_debug(GETLONG(vip->ssrcnet), vip->srchost);

    /*
     * Record the first and last tuple in a location of the packet that
     * is easy to access.
     */
    first = update->data_offset;
    last = first + ((vip->len - (update->data - (uchar *)vip)) /
		    VSRTP_ENTRY_BYTES);
    pak->unspecified[0] = first;
    pak->unspecified[1] = last;

    /*
     * First packet is an easy one.
     */
    if (!queue->count) {
	if (do_debug)
	    buginf("\nVSRTP: Enqueueing fragment, tuples %d-%d.",
		   first, last);
	enqueue(queue, pak);
	if (!mgd_timer_running(&path->reassembly_timer))
	    mgd_timer_start(&path->reassembly_timer, VSRTP_REASSEMBLY_TIME);
	*result = VSRTP_REASS_ENQUEUED;
	return(NULL);
    }

    /*
     * Scan the reassebly queue and add this fragment in the correct
     * location for the fragments that it contains.  If this fragment
     * is a duplicate, signal the higher layer so it is handled
     * appropriately.
     */
    prev = NULL;
    for (saved = queue->qhead; saved; prev = saved, saved = saved->next) {
	if (first < saved->unspecified[0])
	    break;
	if ((first >= saved->unspecified[0]) && 
	    (last  <= saved->unspecified[1])) {
	    if (do_debug)
		buginf("\nVSRTP: Duplicate fragment discarded, tuples %d-%d.",
		       first, last);
	    vsrtp_rx_frag_drop_dup++;
	    *result = VSRTP_REASS_DUPLICATE;
	    return(NULL);
	}
    }
    insqueue(queue, pak, prev);
    if (do_debug)
	buginf("\nVSRTP: Enqueueing fragment, tuples %d-%d.",
	       first, last);

    /*
     * Now rescan the queue and merge any packets containing adjacent
     * sets of tuplets.
     */
    prev = NULL;
    for (saved = queue->qhead; saved && saved->next;
	 prev = saved, saved = saved->next) {
	next = saved->next;
	if (saved->unspecified[1] != next->unspecified[0])
	    continue;

	size = (next->unspecified[1] - next->unspecified[0]) *
	    VSRTP_ENTRY_BYTES;
	
	/*
	 * Unqueue the two fragments and requeue a single new larger fragment.
	 */
	if (do_debug)
	    buginf("\nVSRTP: Merging fragments %d-%d and %d-%d.",
		   saved->unspecified[0], saved->unspecified[1],
		   next->unspecified[0], next->unspecified[1]);
	unqueue(queue, next);
	unqueue(queue, saved);
	temp = pak_grow(saved, saved->datagramsize, saved->datagramsize + size);
	if (!temp) {
	    if (do_debug)
		buginf("\nVSRTP: Cannot get buffer.  Flushing reassembly queue.");
	    datagram_done(next);
	    datagram_done(saved);
	    vines_flush_queue(queue);
	    mgd_timer_stop(&path->reassembly_timer);
	    vroute_no_buffer_in++;
	    *result = VSRTP_REASS_FAILED;
	    return(NULL);
	}
	insqueue(queue, temp, prev);
	saved = temp;
	
	/*
	 * Now do all the data manipulation to join the data from the
	 * next packet to the current one.
	 */
	saved_vip = (vinesiptype *)saved->network_start;
	saved_rtp = (vsrtp_header *)saved->transport_start;
	saved_update = (vsrtp_update *)saved_rtp->data;
	to = saved_update->data + VSRTP_ENTRY_BYTES *
	    (saved->unspecified[1] - saved->unspecified[0]);
	
	next_rtp = (vsrtp_header *)next->transport_start;
	next_update = (vsrtp_update *)next_rtp->data;
	from = next_update->data;

	bcopy(from, to, size);
	saved->unspecified[1] = next->unspecified[1];
	saved_update->flags |= next_update->flags;
	saved_vip->len += size;
	saved->datagramsize += size;
	datagram_done(next);
	
	/*
	 * Is this a complete update?  If so, return the fully
	 * reassembled packet.  This packet must still have a packet lock
	 * on it, because the original reference was discarded a long
	 * time ago.
	 */
	if ((saved_update->flags & VSRTP_UPD_FLAG_BOMEOM) ==
	    VSRTP_UPD_FLAG_BOMEOM) {
	    unqueue(queue, saved);
	    if (do_debug)
		buginf("\nVSRTP: Reassembly finished, tuples %d-%d.",
		       saved->unspecified[0], saved->unspecified[1]);

	    if (queue->count) {
		if (do_debug)
		    buginf("\nVSRTP: Flushing fragments still on queue.  Say what!?!");
		vines_flush_queue(queue);
	    }
	    mgd_timer_stop(&path->reassembly_timer);
	    vsrtp_reass_complete++;
	    *result = VSRTP_REASS_DONE;
	    return(saved);
	}
    }

    /*
     * on slow links such as 2400 baud, it takes longer than
     * 5 seconds to send a 1500 byte RTP packet. so increased
     * the reassembly time up to 10 seconds per packet and in 
     * addition, restart the reassembly timer in case an RTP 
     * update consists of multiple fragments.
     */
    mgd_timer_start(&path->reassembly_timer, VSRTP_REASSEMBLY_TIME);
    *result = VSRTP_REASS_ENQUEUED;
    return(NULL);
}

/*
 * vsrtp_reassemble 
 *
 * Top level reassembly routine.  Check to make sure this update fragment
 * is part of the complete update that is currently being reassebmled,
 * and flush the reassembly queue if it is not.  Next, check to see if
 * this is the only fragment of an update and bail early if it is.
 * Otherwise, pass the fragment to the reassembly worker routine.
 */
paktype *vsrtp_reassemble (
    vinespathtype *path,
    paktype *pak,
    vsrtp_header *rtp,
    vsrtp_reass_type *result)
{
    paktype *saved;
    ulong first, last;
    boolean complete;
    vinesiptype *vip;
    vsrtp_header *saved_rtp;
    vsrtp_update *update, *saved_update;
    boolean do_debug;

    vip = (vinesiptype *)pak->network_start;
    update = (vsrtp_update *)rtp->data;
    complete = ((update->flags & VSRTP_UPD_FLAG_BOMEOM) ==
		VSRTP_UPD_FLAG_BOMEOM);
    do_debug = do_vines_rtp_debug(GETLONG(vip->ssrcnet), vip->srchost);

    if (path->reassemblyQ.count) {
	saved = (paktype *)path->reassemblyQ.qhead;
	saved_rtp = (vsrtp_header *)saved->transport_start;
	saved_update = (vsrtp_update *)saved_rtp->data;
	if (saved_update->packet_id > update->packet_id) {
	    /*
	     * This fragment is from a previous update and should be
	     * ignored.
	     */
	    vsrtp_rx_frag_drop_oos++;
	    *result = VSRTP_REASS_IGNORED;
	    return(NULL);
	} else if (saved_update->packet_id < update->packet_id) {
	    /*
	     * This fragment is either the first fragment of a new
	     * update, or a complete new update.
	     */
	    if (do_debug)
		buginf("\nVSRTP: Flushing fragments of update %x for fragment of update %x.",
		       saved_update->packet_id, update->packet_id);
	    vsrtp_reass_failed++;
	    while ((saved = dequeue(&path->reassemblyQ)) != NULL) {
		first = saved->unspecified[0];
		last = saved->unspecified[1];
		vsrtp_rx_frag_drop_old += (last - first)/VSRTP_UPD_FRAGENTRIES;
		datagram_done(saved);
	    }
	} else if (complete) {
	    /*
	     * This handles the corner case.  We have a fragment of an
	     * update, but then we receive a complete update with the
	     * same packet ID number.  This should never happen, but...
	     */
	    if (do_debug)
		buginf("\nVSRTP: Flushing fragments of update %x for update %x.",
		       saved_update->packet_id, update->packet_id);
	    vines_flush_queue(&path->reassemblyQ);
	    mgd_timer_stop(&path->reassembly_timer);
	}
    }

    if (complete) {
	*result = VSRTP_REASS_DONE;
	return(pak);
    }

    vsrtp_rx_frag_rcvd++;
    return(vsrtp_reassemble1(path, pak, update, result));
}


/**********************************************************************
 *
 *			  GENERATE AN UPDATE
 *
 **********************************************************************/

/*
 * vsrtp_generate_update
 *
 * Generate one gigantic packet containing all of the information to be
 * transmitted.  Another routine is used to fragment this monolithic
 * update into smaller size packets that can actually be transmitted.
 */
paktype *vsrtp_generate_update (
    enum vsrtp_update_type type,
    char *name,
    int size,
    int change_id,
    vinesidbtype *idb)
{
    paktype *pak;
    vinesnettype *server;
    vinesroutetype *route;
    vsrtp_update *update;
    vsrtp_entry *entry;
    boolean do_debug;

    /*
     * Is there a large enough buffer available?
     */
    do_debug = do_vines_rtp_v_debug(vines_network, vines_host);
    if (do_debug) {
	buginf("\nVSRTP: generating %s update", name);
	if (type == VSRTP_CHANGE_UPDATE)
	    buginf(", entries since %08x", change_id);
    }
    pak = vsrtp_getbuffer(name, VSRTP_UPDATE_BYTES + size);
    if (!pak)
	return(NULL);

    /*
     * Populate it with information.
     */
    if (type == VSRTP_NULL_UPDATE)
	return(pak);
    update = (vsrtp_update *)pak->session_start;
    entry = (vsrtp_entry *)update->data;
    server = NULL;
    while((server = vroute_GetNextNetwork(server)) != NULL) {
	if (server->net == vines_network)
	    continue;
	if (idb && idb->output_network_filter) {
	    if (!vines_short_access_check(server->net, VINES_SERVER,
					  idb->output_network_filter))
		continue;
	}
	if (type == VSRTP_CHANGE_UPDATE) {
	    FOR_ALL_ROUTES(server, route) {
		if (route->local_timestamp >= change_id)
		    break;
	    }
	    if (!route)
		continue;
	} else {
	    route = server->best_route;
	}
	entry->net      = server->net;
	entry->metric   = vmetric_to_srtp(server->adv_metric);
	PUTLONG(&entry->sequence, route->origin_timestamp);
	entry->flags    = route->rtp_flags;
	entry->reserved = 0;
	if (do_debug)
	    buginf(vines_network_metric_seq_flags_msg,
		   server->net, server->adv_metric, route->origin_timestamp,
		   route->rtp_flags);
	entry++;
	process_suspend_if_req();
    }

    pak->datagramsize = (uchar *)entry - pak->session_start;
    return(pak);
}


/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * vsrtp_display_global
 *
 * Print all SRTP information that is maintained on a per global basis.
 */
void vsrtp_display_global (void)
{
    vinesneighbortype *neighbor;
    vinespathtype *path;
    mgd_timer *timer;
    uint DisplayBufSize;
    char *displaybuf;

    if (mgd_timer_running(&vsrtp_start_timer))
	printf("\n  SRTP scheduled to start in %d seconds",
	       mgd_timer_left_sleeping(&vsrtp_start_timer)/ONESEC);
    
    if (!vsrtp_enabled)
	return;

    displaybuf = malloc(VINESBUFLEN);
    if (displaybuf == NULL)
	return;
    DisplayBufSize = VINESBUFLEN;
    
    printf("\n  Sequence: %08x, Packet ID: %08x",
	   vines_sequence, vines_packet_id);
    if (TIMER_RUNNING(vsrtp_reinit_check_time)) {
	printf("\n  Reinit check in %d seconds, starting sequence %08x",
	       TIME_LEFT_SLEEPING(vsrtp_reinit_check_time)/ONESEC,
	       vsrtp_reinit_sequence);
    }

    printf("\n  Reassembly timer queue:");
    if (!mgd_timer_running(&vsrtp_reassembly_timer)) {
	printf("  (empty)");
    } else {
	*displaybuf = '\0';
	timer = &vsrtp_reassembly_timer;
	while ((timer = mgd_timer_next_running(timer)) != NULL) {
	    path = mgd_timer_context(timer);
	    neighbor = (vinesneighbortype *)path->neighbor;
	    safe_sprintf(&displaybuf, &DisplayBufSize,
			 "\n    Neighbor %s in %u seconds.", vpath_id(path),
			 mgd_timer_left_sleeping(timer)/ONESEC);
	}
	printf("%s", displaybuf);
    }

    printf("\n  Retry timer queue:");
    if (!mgd_timer_running(&vsrtp_retry_timer)) {
	printf("  (empty)");
    } else {
	*displaybuf = '\0';
	timer = &vsrtp_retry_timer;
	while ((timer = mgd_timer_next_running(timer)) != NULL) {
	    path = mgd_timer_context(timer);
	    neighbor = (vinesneighbortype *)path->neighbor;
	    safe_sprintf(&displaybuf, &DisplayBufSize,
			 "\n    Neighbor %s in %u seconds, %d more attempts.",
			 vpath_id(path),
			 mgd_timer_left_sleeping(timer)/ONESEC,
			 path->retry_count);
	}
	printf("%s", displaybuf);
    }
    free(displaybuf);
}

/*
 * vsrtp_display_interface
 *
 * Print all SRTP information that is maintained on a per interface basis.
 */
void vsrtp_display_interface (vinesidbtype *idb)
{
    char temp[40];

    if (!vines_vsrtp_enabled(idb))
	return;

    if (vines_rtp_present(idb))
	printf("\n  VINES compatibility mode is enabled");

    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&idb->sync_timer));
    printf("\n  Next synchronization update in %s.", temp);
}

/*
 * vsrtp_display_neighbor
 *
 * Display all the SRTP counters kept on a per-neighbor basis.
 */
static const char vsrtp_cnts_fmt[] = "\n          %8s  %08x  %08x  %08x  %08x  %08x";

void vsrtp_display_neighbor (vinesneighbortype *p)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    vinespathtype *path;
    char textaddr[40];

    if (!vsrtp_enabled)
	return;

    printf("\n\n  SRTP Counters:");

    FOR_ALL_PATHS(p, path) {
	char temp[40];

	swidb = path->idb->phys_idb;
	hwidb = swidb->hwptr;

	vines_dlc(textaddr, &path->dlc_addr);
	printf("\n\n    Interface %s, address %s, state %s",
	       swidb->namestring, textaddr, vsrtp_prstate(path->state));

	printf("\n      Origin %08x, Local %08x(%08x), Flags %04x, ID %04x",
	       path->origin_timestamp, path->local_timestamp,
	       path->reflected_timestamp,
	       path->srtp_flags, path->packet_id);

	printf("\n      Timers:");
	printf("\n        Reassembly: %s",
	       vtimer_pretty_print(&path->reassembly_timer, "not active", temp));
	printf("\n        Retry request: %s",
	       vtimer_pretty_print(&path->retry_timer, "not active", temp));
	if (mgd_timer_running(&path->retry_timer) && path->retry_count)
	    printf(", %d more tries", path->retry_count);

	printf("\n      Received counters:");
	printf("\n        Requests:   specific   changes      full      null   unknown");
	printf(vsrtp_cnts_fmt, " ", path->srtp_rx_req_counts[0],
	       path->srtp_rx_req_counts[1], path->srtp_rx_req_counts[2],
	       path->srtp_rx_req_counts[3], path->srtp_rx_req_counts[4]);
	printf("\n        Updates:      failed      less     equal  one more   greater");
	printf(vsrtp_cnts_fmt, "null:", path->srtp_rx_upd_counts[0],
	       path->srtp_rx_upd_counts[1], path->srtp_rx_upd_counts[2],
	       path->srtp_rx_upd_counts[3], path->srtp_rx_upd_counts[4]);
	printf(vsrtp_cnts_fmt, "change:", path->srtp_rx_upd_counts[5],
	       path->srtp_rx_upd_counts[6], path->srtp_rx_upd_counts[7],
	       path->srtp_rx_upd_counts[8], path->srtp_rx_upd_counts[9]);
	printf(vsrtp_cnts_fmt, "full:", path->srtp_rx_upd_counts[10],
	       path->srtp_rx_upd_counts[11], path->srtp_rx_upd_counts[12],
	       path->srtp_rx_upd_counts[13], path->srtp_rx_upd_counts[14]);
	printf(vsrtp_cnts_fmt, "sync:", path->srtp_rx_upd_counts[15],
	       path->srtp_rx_upd_counts[16], path->srtp_rx_upd_counts[17],
	       path->srtp_rx_upd_counts[18], path->srtp_rx_upd_counts[19]);
	printf("\n        Redirects:  %08x", path->srtp_rx_redir_count);
	printf("\n        Reinits:    %08x", path->srtp_rx_reinit_count);

	printf("\n      Transmitted counters:");
	printf("\n        Requests:   specific   changes      full      null   unknown");
	printf(vsrtp_cnts_fmt, " ", path->srtp_tx_req_counts[0],
	       path->srtp_tx_req_counts[1], path->srtp_tx_req_counts[2],
	       path->srtp_tx_req_counts[3], path->srtp_tx_req_counts[4]);
	printf("\n        Updates:    %08x", path->srtp_tx_upd_count);
	printf("\n        Responses:  %08x", path->srtp_tx_resp_count);
	printf("\n        Redirects:  %08x", path->srtp_tx_redir_count);
	printf("\n        Reinits:    %08x", path->srtp_tx_reinit_count);
    }
}

/*
 * vrtp_display_timer
 *
 * Display all the timer queues maintained by SRTP.
 */
void vsrtp_display_timer (void)
{
    if (!vsrtp_enabled)
	return;

    vtimer_dump_path_queue(&vsrtp_reassembly_timer,
			   "      Reassembly: ", "empty");
    vtimer_dump_path_queue(&vsrtp_retry_timer,
			   "      Retry: ", "empty");
}

/*
 * vsrtp_decode_packet
 *
 * Print out the SRTP information in a packet.  This set of routines does
 * not use any of the pointers in the packet header other than the
 * network start pointer, so it may be called at any point in the
 * processing of a packet.
 */
uchar *vsrtp_decode_packet (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vsrtp_header *srtp;
    vsrtp_request *request;
    vsrtp_update *update;
    char *s, buffer[20];
    int temp;

    srtp = (vsrtp_header *)data;
    (*fn)("\n   RTP version %04x, op %s, node %02x, flags %02x, reserved %02x",
	  srtp->version, vsrtp_names[srtp->operation],
	  srtp->node_type, srtp->flags, srtp->reserved);
    *limit -= VSRTP_HEADER_BYTES;

    switch (srtp->operation) {
      case VSRTP_REQUEST:
	request = (vsrtp_request *)srtp->data;
	temp = request->type;
	if (temp > VSRTP_REQ_LAST) {
	    sprintf(buffer, "invalid (%x)", temp);
	    s = buffer;
	} else {
	    s = vsrtp_request_names[temp];
	}
	(*fn)("\n       type %s, reserved %02x, data %04x", s,
	      request->reserved, request->data[0]);
	*limit -= VSRTP_REQUEST_BYTES;
	return((uchar *)&request->data[1]);
      case VSRTP_UPDATE:
	update = (vsrtp_update *)srtp->data;
	temp = update->type;
	if (temp > VSRTP_UPD_LAST) {
	    sprintf(buffer, "invalid (%x)", temp);
	    s = buffer;
	} else {
	    s = vsrtp_update_names[temp];
	}
	(*fn)("\n       type %s, flags %02x, pid %04x, ofst %04x, seq %08x, metric %04x",
	      s, update->flags, update->packet_id, update->data_offset,
	      GETLONG(&update->sequence), update->metric);
	*limit -= VSRTP_UPDATE_BYTES;
	return(update->data);
      case VSRTP_REDIRECT:
      case VSRTP_REINIT:
      default:
	return(srtp->data);
    }
}

/**********************************************************************
 *
 *
 *
 **********************************************************************/

/*
 * vsrtp_update_route
 *
 * Perform updating of the VINES RTP routing tables for a 5.50 update.
 * According to VPD p5-17, the local sequence number is always updated
 * for a neighbor's route entry, but is only updated upon a network
 * change for a remote server's route entry.
 */
boolean vsrtp_update_route (
    ulong dstnet,
    ushort advertised,	/* internal form */
    ulong sequence,
    uchar flags,
    vinespathtype *path)
{
    vinesneighbortype *neighbor, *current_gw;
    vinesnettype *server;
    vinesroutetype *route;
    int metric;
    boolean do_debug, changed;

    if (!path || !path->neighbor)
	return(FALSE);

    neighbor = path->neighbor;
    metric = min(VINES_INFINITY, advertised + neighbor->metric);
    server = vroute_FindEntry(dstnet);
    do_debug = do_vines_table_debug(dstnet, VINES_SERVER);
    /*
     * Not found, enter it.
     */
    if (server == NULL) {
	if (metric == VINES_INFINITY)
	    return(FALSE);
	server = vines_add_server(dstnet, V_NET_RTP);
	route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	if (!server || !route) {
	    vines_delete_server_entry(server);
	    return(FALSE);
	}
	route->origin_timestamp = sequence;
	route->local_timestamp = vines_sequence + 1;
	route->rtp_flags = flags;
	vines_new_server(server);
	if (do_debug)
	    buginf("\nVSRTP: create route to %#Z via %#z (metric %04x, timestamp %08x)",
		   server->net, neighbor->net, neighbor->host, metric,
		   route->local_timestamp);
	return(TRUE);
    }

    /*
     * Never be fooled by someone else advertising this routers address.
     */
    if (server->net == vines_network) {
	path->reflected_timestamp = sequence;
	return(FALSE);
    }

    /*
     * do not accept older information unless the existing route 
     * is unreachable i.e. in garbage collection state.
     */
    route = vines_get_route_by_xxx(server, NULL, V_NET_RTP, NULL);
    if (route &&
	!(route->rtp_flags & VSRTP_ENTRY_NO_SEQ) &&
	!(flags & VSRTP_ENTRY_NO_SEQ) &&
	vsrtp_compare_seqno(route->origin_timestamp, sequence) == LESS) {

	if (route->metric != VINES_INFINITY) {
	    if (do_debug)
		buginf("\nVSRTP: existing route to %#Z via %#z is more recent "
		       "%x v %x", dstnet, route->gw->net, route->gw->host,
		       sequence, route->origin_timestamp);

	    /*
	     * We'll ignore this information but see if this neighbor needs help.
	     */
	    changed = FALSE;
	    if (metric == VINES_INFINITY) {
		FOR_ALL_ROUTES(server, route) {
		    if (route->metric != VINES_INFINITY) {
			route->local_timestamp = vines_sequence + 1;
			changed = TRUE;
		    }
		}
	    }
	    vsrtp_rx_oldnetinfo++;
	    return (changed);
	}
    }
    
    /*
     * Better metric
     */
    if (metric < server->actual_metric) {
	if (do_debug)
	    buginf("\nVSRTP: better route for %#Z (new %04x, old %04x, timestamp %08x)",
		   dstnet, metric, server->actual_metric, vines_sequence + 1);

	/*
	 * Does this change require suppression?  VPD 5-109 and 5-110
	 */
	current_gw = server->best_route->gw;
	if ((server->actual_metric == VINES_INFINITY) ||
	    (TIMER_RUNNING_AND_SLEEPING(server->suppresstime) &&
	    current_gw != neighbor)) {
	    vines_set_suppresstime(server);
	    if (vines_any_rtp_present()) {
		TIMER_START(server->changetime, VRTP_CHANGETIME);
	    }
	}
	
	/*
	 * One route only, or multiple routes?  If the addition of a
	 * second or subsequent routes fails, there is no need to delete
	 * the server entry.  Just bail out and leave the existing routes
	 * intact.
	 */
	if (vines_onlyone) {
	    route = server->best_route;
	    if (route->flags & V_NET_STATIC)
		return(FALSE);
	    route->gw = neighbor;
	    route->metric = advertised;
	    vines_fixup_server(server, FALSE);
	} else {
	    route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	    if (!route)
		return(FALSE);
	    server = vines_delete_route_by_xxx(server, NULL, V_NET_RTP, metric,
					       timer_no_time);
	    if (!server)
		return(FALSE);
	}
	
	/*
	 * Common code
	 */
	route->origin_timestamp = sequence;
	route->local_timestamp = vines_sequence + 1;
	route->rtp_flags = flags;
	vines_flush_cache_by_server(server);
    } else if (metric > server->actual_metric) {
	/*
	 * Worse metric.  See whether this neighbor currently has a route
	 * for the advertised server.  If so, mark that route as being
	 * unreachable.
	 */
	route = vines_get_route_by_xxx(server, neighbor, V_NET_RTP, NULL);
	if (route && (route->metric != VINES_INFINITY)) {
	    if (do_debug)
		buginf("\n\nVSRTP: mark %#Z unreachable via %#z (timestamp %08x)",
		       dstnet, neighbor->net, neighbor->host, vines_sequence + 1);
	    route->origin_timestamp = sequence;
	    route->local_timestamp = vines_sequence + 1;
	    route->rtp_flags = flags;
	    GET_TIMESTAMP(route->gctime);
	    route->metric = VINES_INFINITY;
	    route->flags  = V_NET_RTP;
	    vines_fixup_server(server, FALSE);
	    if (vines_any_rtp_present()) {
		TIMER_START(server->changetime, VRTP_CHANGETIME);
	    }
	}
	if ((metric == VINES_INFINITY) && 
	    !TIMER_RUNNING_AND_SLEEPING(server->suppresstime)) {
	    /*
	     * Bump the timestamp on any other routes to this server.
	     * This will force them to be sent in the next scheduled
	     * routing update.  This is how the "Help Thy Neighbor"
	     * scenario works for SRTP.  There are no extra packets
	     * generated as in the non-SRTP case.
	     */
	    changed = FALSE;
	    FOR_ALL_ROUTES (server, route) {
		if ((route->metric != VINES_INFINITY) &&
		    !(vines_best_path_uses_idb(route->gw, path->idb))) {
		    route->local_timestamp = vines_sequence + 1;
		    changed = TRUE;
		}
	    }
	    return (changed);
	}
	return(TRUE);
    } else if (metric == VINES_INFINITY) {
	/*
	 * This neighbor is unreachable.  An unreachable indication of a
	 * neighbor that is already unreachable should cause that entry
	 * to be deleted from the tables.  VPD 5-93.
	 */
	route = vines_get_route_by_gw(server, neighbor);
	server = vines_delete_this_route(server, route);
	return(FALSE);
    } else {
	/*
	 * Same metric.  Either it is already in our tables, or we need
	 * to add another entry.  Add_route will fix up the server
	 * entry, so we only have to worry about the FS cache for the
	 * single route case.
	 */
	route = vines_get_route_by_xxx(server, neighbor, V_NET_RTP, NULL);
	if (route) {
	    route->origin_timestamp = sequence;
	    vines_set_route_timestamp(route);
	    route->flags  = V_NET_RTP;
	    route->rtp_flags = flags;
	    return(FALSE);
	}

	vines_flush_cache_by_server(server);
	
	if (vines_onlyone) {
	    route = vines_get_route_by_xxx(server, NULL, V_NET_RTP, NULL);
	    if (route == NULL) return(FALSE);

	    /* Reuse this route entry */
	    route->gw = neighbor;
	    route->metric = advertised;
	    vines_set_route_timestamp(route);
	    route->uses = 0;
	    route->flags = V_NET_RTP;
	    route->origin_timestamp = sequence;
	    route->local_timestamp = vines_sequence;
	    route->rtp_flags = flags;
	    vines_fixup_server(server, FALSE);
	    if (do_debug)
		buginf("\nVSRTP: fresher route to %#Z via %#z (metric %x, timestamp %x)",
		       dstnet, neighbor->net, neighbor->host, metric, route->local_timestamp);
	    return(TRUE);
	}

	route = vines_get_route_by_xxx(server, NULL, V_NET_RTP, NULL);
	if (route &&
	    !(flags & VSRTP_ENTRY_NO_SEQ) &&
	    !(route->rtp_flags & VSRTP_ENTRY_NO_SEQ) &&
	    vsrtp_compare_seqno(sequence, route->origin_timestamp) == LESS) {
	    if (vines_delete_route_by_xxx(server, NULL, V_NET_RTP, 0, timer_no_time) == NULL)
		server = vines_add_server(dstnet, V_NET_RTP);
	}
	
	route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	if (route == NULL)
	    return(FALSE);
	route->origin_timestamp = sequence;
	route->local_timestamp = vines_sequence;
	route->rtp_flags = flags;
	if (do_debug)
	    buginf("\nVSRTP: alternate route to %#Z via %#z (metric %04x, timestamp %08x)",
		dstnet, neighbor->net, neighbor->host, metric, route->local_timestamp);
    }
    return(TRUE);
}

/**********************************************************************
 *
 * SEND SRTP messages
 *
 **********************************************************************/
STATIC const char vsrtp_msg[] = "\nVSRTP: received %s from %#z on %s";
STATIC char *const vsrtp_names[] = {"0", "request", "update", "redirect", "reinit"};

/*
 * vsrtp_send_full_request
 *
 * Send an VINES SRTP request for routing information.  This may be
 * directed to a single station or a single interface if desired.
 */
void vsrtp_send_full_request (
    vinesidbtype *target,
    vinespathtype *path)
{
    paktype *pak;
    vsrtp_request *request;

    if (!vines_vsrtp_enabled(target))
	return;

    /*
     * Build a packet that will be used to broadcast on all interfaces.
     */
    pak = vsrtp_getbuffer("full request", VSRTP_REQUEST_BYTES);
    if (!pak)
	return;
    request = (vsrtp_request *)pak->session_start;
    request->type = VSRTP_REQ_FULL;
    request->reserved = 0;

    /*
     * Check inputs
     */
    if (path) {
	vsrtp_send_specific(pak, VSRTP_REQUEST, path);
    } else {
	vsrtp_send_broadcast(pak, VSRTP_REQUEST, target);
    }
}

/*
 * vsrtp_send_update
 *
 * Generate and send an VINES Sequenced RTP update message.  The type
 * field determines the amount of information that it put into the
 * update.
 */
void vsrtp_send_update (
    enum vsrtp_update_type type,
    vinesidbtype *idb,
    vinespathtype *path)
{
    paktype *pak;
    queuetype queue;
    char *name;
    int size, flags;

    if (!vines_vsrtp_enabled(idb))
	return;

   switch (type) {
      case VSRTP_SYNC_UPDATE:
	name  = "sync";
	size  = VSRTP_UPD_TOTALBYTES;
	flags = (VSRTP_UPD_FLAG_SYNC | VSRTP_UPD_FLAG_FULL);
	break;
      case VSRTP_FULL_UPDATE:
	name  = "full";
	size  = VSRTP_UPD_TOTALBYTES;
	flags = VSRTP_UPD_FLAG_FULL;
	break;
      case VSRTP_CHANGE_UPDATE:
	name  = "change";
	size  = VSRTP_UPD_TOTALBYTES;
	flags = VSRTP_UPD_FLAG_CHANGE;
	break;
      case VSRTP_NULL_UPDATE:
	name  = "null";
	size  = 0;
	flags = VSRTP_UPD_FLAG_NULL;
	break;
      case VSRTP_INVALID_UPDATE:
      default:
	return;
    }

    pak = vsrtp_generate_update(type, name, size, vines_sequence, idb);
    if (!pak)
	return;
    if (!vsrtp_fragment_update(&queue, pak, VSRTP_UPD_UPDATE, idb->vines_metric, flags))
	return;
    if (type == VSRTP_SYNC_UPDATE)
	vsrtp_tx_resync_count++;
    vsrtp_send_fragments(&queue, path, idb, FALSE);
}

/*
 * vsrtp_send_redirect
 *
 * Generate and send a VINES Sequenced RTP redirect packet.  There is no
 * way of knowing what station forwarded this packet other than
 * extracting the senders MAC address and working from there.  The
 * simplest thing to do is to send this packet as a broadcast.  This will
 * also prime any other station that might be about to send a packet to
 * the same destination via this router.
 */
static const char vsrtp_no_redirect[] = "\nVSRTP: not sending redirect for %#z (%s)";

void vsrtp_send_redirect (
    paktype *original,
    routingdecision *thewaythere)
{
    vinesiptype *vip;
    vinesneighbortype *neighbor;
    vinespathtype *path;
    paktype *pak;
    vsrtp_redirect *redirect;
    vinesidbtype *idb;
    ulong dstnet;
    ushort dsthost;
    riftype sender_rif, gw_rif, *temp_rif;
    srbroute_t *data;
    uchar sender_mac[IEEEBYTES];
    leveltype level;
    ulong dst_metric, nbr_metric;
    boolean do_debug;
    vinespathtype thewayback;
    vinesneighbortype dummyneighbor;
    
    idb = vines_getidb(original->if_input);
    if (!vines_vsrtp_enabled(idb))
	return;

    /*
     * Do not send a redirect if we have sent one recently, if we turned
     * off redirects by setting the interval to zero, or if we are
     * filtering information about that network.
     */
    vip = (vinesiptype *)original->network_start;
    dstnet = GET2WD(vip->ddstnet);
    if (idb->output_network_filter &&
	!vines_short_access_check(dstnet, VINES_SERVER,
				  idb->output_network_filter)) {
	return;
    }
    if (SLEEPING(idb->vines_redirect_time) || !idb->vines_redirect_interval)
	return;
    TIMER_START(idb->vines_redirect_time, idb->vines_redirect_interval);
    do_debug = do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost);
    do_debug |= do_vines_rtp_debug(GETLONG(vip->ssrcnet), vip->srchost);

    find_802addr(original, sender_mac, GET_SOURCE);

    /*
     * Are there any RIFs associated with the stations invloved in this
     * redirect?  There are four cases, and two sub-cases:
     *
     * 	Source	Dest	Response
     * 	------	----	--------
     * 	no rif	no rif	send redirect with no rif
     * 	rif	no rif	send redirect with no rif
     * 	no rif	rif	send redirect with rif
     * 	rif	rif	a) if source and dest are on same ring,
     * 			   send redirect with rif 
     * 			b) if source and dest are on different
     * 			   rings, do not send redirect
     *
     * I do not know why Banyan implemented the last case this way.  It
     * is possible, but painful, to concatenate RIFs.  We should not send
     * a redirect in this case to be compatible with Banyan.  They will
     * do the wrong thing upon receipt of this packet.
     *
     * Rifs are copied to local storage because they can change at
     * interrupt level.  If this happens while the actual rif part of the
     * entry is being copied, you can form a circular rif.
     */
    if (!using_multiring(original->if_input, LINK_VINES)) {
	memset(&gw_rif, 0, sizeof(riftype));
	memset(&sender_rif, 0, sizeof(riftype));
    } else {
	level = raise_interrupt_level(NETS_DISABLE);
	temp_rif = rif_lookup(thewaythere->path->dlc_addr.addr,
			      hwidb_or_null(original->if_input), NULL, 0);
	if (temp_rif)
	    bcopy(temp_rif, &gw_rif, sizeof(riftype));
	else
	    memset(&gw_rif, 0, sizeof(riftype));
	temp_rif = rif_lookup(sender_mac, 
			      hwidb_or_null(original->if_input), NULL, 0);
	if (temp_rif)
	    bcopy(temp_rif, &sender_rif, sizeof(riftype));
	else
	    memset(&sender_rif, 0, sizeof(riftype));
	reset_interrupt_level(level);

	if ((gw_rif.length >= RC_MIN_RIF)
	    && (sender_rif.length >= RC_MIN_RIF) &&
	    (RIF_DEST_RING((srbroute_t *)gw_rif.rif) !=
	     RIF_DEST_RING((srbroute_t *)sender_rif.rif))) {
	    if (do_debug)
		buginf(vsrtp_no_redirect, dstnet, vip->dsthost,
		       "src and dst have rifs");
	    return;
	}
    }
    
    /*
     * Can we get a packet to create a redirect.
     */
    pak = vsrtp_getbuffer("redirect", VSRTP_REDIRECT_BYTES+SRB_MAXRIF);
    if (!pak) {
	if (do_debug)
	    buginf(vsrtp_no_redirect, dstnet, vip->dsthost, "no packet");
	return;
    }
    redirect = (vsrtp_redirect *)pak->session_start;

    /*
     * Fill in destination specific fields.  If the ultimate destination
     * is a neighbor, we will have complete information.  If not, simply
     * fill in as much as possible.
     */
    dstnet  = GET2WD(vip->ddstnet);
    dsthost = vip->dsthost;
    PUTLONG(redirect->dst_net, dstnet);
    redirect->dst_host = dsthost;
    neighbor = thewaythere->neighbor;
    if ((neighbor->net == dstnet) && (neighbor->host == dsthost)) {
	dst_metric = thewaythere->path->metric;
	redirect->dst_metric   = vmetric_to_srtp(dst_metric);
	redirect->dst_node     = thewaythere->neighbor->node;
	redirect->dst_flags    = thewaythere->neighbor->srtp_flags;
	PUTLONG(redirect->dst_sequence, thewaythere->path->origin_timestamp);
    } else {
	dst_metric = thewaythere->route->metric + thewaythere->neighbor->metric;
	dst_metric = min(dst_metric, VINES_INFINITY);
	redirect->dst_metric   = vmetric_to_srtp(dst_metric);
	redirect->dst_node     = 0;
	redirect->dst_flags    = thewaythere->route->rtp_flags;
	PUTLONG(redirect->dst_sequence, 0);
    }

    /*
     * Fill in the next hop specific fields.  We do not know if the
     * previous route lookup gave us a path to the neighbor on the same
     * interface as the incoming datagram.  Request that specific path
     * here to make sure that we have the right path.
     */
    path = vines_get_path_by_idb(neighbor, idb);
    nbr_metric = path->metric;
    PUTLONG(redirect->nbr_net, neighbor->net);
    redirect->nbr_host       = neighbor->host;
    redirect->nbr_metric     = vmetric_to_srtp(nbr_metric);
    redirect->nbr_node       = neighbor->node;
    redirect->nbr_flags      = neighbor->srtp_flags;
    PUTLONG(redirect->nbr_sequence, path->origin_timestamp);
    redirect->nbr_macaddrlen = IEEEBYTES;
    ieee_copy(path->dlc_addr.addr, redirect->nbr_mac);

    if (gw_rif.length >= RC_MIN_RIF) {
	pak->datagramsize -= (SRB_MAXRIF - gw_rif.length);
	redirect->nbr_riflen = gw_rif.length;
	bcopy(gw_rif.rif, redirect->nbr_rif, gw_rif.length);
	data = (srbroute_t *)redirect->nbr_rif;
	data->rc_dlf ^= RC_D;
    } else {
	pak->datagramsize -= SRB_MAXRIF;
	redirect->nbr_riflen = 0;
    }

    /* Build a dummy path back to the last forwarding node, which can
     * be either the source of the packet or a router of the packet.
     */
    dummyneighbor.net = VINES_ALLHOSTS;
    dummyneighbor.host = VINES_ALLHOSTS;
    thewayback.neighbor = &dummyneighbor;
    thewayback.idb = idb;
    thewayback.enctype = original->enctype;
    ieee_copy(sender_mac, thewayback.dlc_addr.addr);
    
    pak->if_output = original->if_input;
    if (do_debug)
	buginf("\nVSRTP: sent redirect for %#z(%04x) via %#z(%04x)",
	       dstnet, dsthost, dst_metric,
	       neighbor->net, neighbor->host, nbr_metric);
    vsrtp_send_specific(pak, VSRTP_REDIRECT, &thewayback);
    idb->tx_rtpv1_type[VSRTP_REDIRECT]++;
}

/*
 * vsrtp_send_reinit
 *
 * Send an VINES RIP request for routing information.  This may be
 * directed to a single station or a single interface if desired.
 */
void vsrtp_send_reinit (vinesidbtype *target)
{
    paktype *pak;

    if (!vines_vsrtp_enabled(target))
	return;

    /*
     * Build a packet that will be used to broadcast on all interfaces.
     */
    pak = vsrtp_getbuffer("reinit", 0);
    if (!pak)
	return;
    vsrtp_send_broadcast(pak, VSRTP_REINIT, target);
}

/*
 * vsrtp_send_periodic
 *
 * On a broadcast media, this will send one broadcast packet.  On a multi
 * point-to-point media, this will send one packet per destination if
 * there are any known destinations, otherwise it will send a broadcast.
 * The packet sent will either be a NULL update or a SYNC update.
 */
void vsrtp_send_periodic (vinesidbtype *idb)
{
    vinespathtype *path;
    dqueue_t *q;

    if (!vines_vsrtp_enabled(idb))
	return;

    if ((idb->phys_idb->hwptr->status & IDB_VINES_BCAST) ||
	(idb->path_list.flink == &idb->path_list)) {
	if (mgd_timer_expired(&idb->sync_timer)) {
	    vsrtp_send_update(VSRTP_SYNC_UPDATE, idb, NULL);
	    mgd_timer_update(&idb->sync_timer, VSRTP_SYNC_INTERVAL);
	} else {
	    vsrtp_send_update(VSRTP_NULL_UPDATE, idb, NULL);
	}
	return;
    }

    for (q = idb->path_list.flink; q != &idb->path_list; q = q->flink) {
	path = (vinespathtype *)q->parent;
	if (path->neighbor->version != VSRTP_VERSION)
	    continue;

	if (mgd_timer_expired(&idb->sync_timer)) {
	    vsrtp_send_update(VSRTP_SYNC_UPDATE, idb, path);
	    mgd_timer_update(&idb->sync_timer, VSRTP_SYNC_INTERVAL);
	} else {
	    vsrtp_send_update(VSRTP_NULL_UPDATE, idb, path);
	}
    }
}

/**********************************************************************
 *
 *		     Top Level Periodic Routines
 *
 **********************************************************************/

static const char vsrtp_bad_node[] = "\nVSRTP: %s %#z's node type field indicates a %s.";

/*
 * vsrtp_process_message
 *
 * Process any incoming SRTP messages that just been removed from the
 * routing queue.  All valid messages are passed top the neighbor update
 * routine to keep that table current.
 */
void vsrtp_process_message (paktype *pak)
{
    vinesiptype *vip;
    vsrtp_header *rtp;
    vsrtp_update *update;
    vsrtp_request *request;
    vinesidbtype *idb;
    ulong srcnet;
    char *error;
    int major, minor, comparison;
    vinesneighbortype *neighbor;
    vinespathtype *path;
    boolean do_debug;
    ulong sequence;
    paktype *reassembled_pak;
    vsrtp_reass_type result;
    uchar update_flags;

    /*
     * Range check the operation field.
     */
    idb = vines_getidb(pak->if_input);
    vip = (vinesiptype *)pak->network_start;
    rtp = (vsrtp_header *)pak->transport_start;
    srcnet = GET2WD(vip->ssrcnet);
    do_debug = do_vines_rtp_debug(srcnet, vip->srchost);
    if ((srcnet == vines_network) && (vip->srchost == vines_host)) {
	if (do_debug)
	    buginf("\nVSRTP: ignoring RTP type (%d), my source address.",
		   rtp->operation);
	return;
    }
    if (rtp->operation >= VSRTP_LAST) {
	if (do_debug)
	    buginf("\nVSRTP: illegal RTP type (%d) from %#z.",
		   rtp->operation, srcnet, vip->srchost);
	idb->rx_rtpv1_illegal++;
	return;
    }
    
    /*
     * Count it and print some debugging.
     */
    idb->rx_rtpv1_type[rtp->operation]++;
    if ((pak->if_input->hwptr->broadcast)(pak->if_input->hwptr, pak)==FUZ_TRUE)
	idb->rx_rtpv1_bcast++;
    if (do_debug)
	buginf(vsrtp_msg, vsrtp_names[rtp->operation], srcnet,
	       vip->srchost, idb->phys_idb->namestring);
    
    /*
     * Make noise about Banyan bugs and then correct them.
     */
    if ((vip->srchost == VINES_SERVER) &&
	(rtp->node_type != VSRTP_NODE_ROUTER)) {
	rtp->node_type = VSRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vsrtp_bad_node, "Server", srcnet, vip->srchost, "client");
    } else if ((vip->srchost != VINES_SERVER) &&
	       (rtp->node_type != VSRTP_NODE_CLIENT)) {
	rtp->node_type = VSRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vsrtp_bad_node, "Client", srcnet, vip->srchost, "server");
    }
    
    /*
     * Update the neighbor entry.  Note: For SRTP, this will not modify
     * any existing metrics.  We can't afford to do that here, because we
     * don't know whether or not this is an SRTP update which include
     * information on the metric that the neighbor is using.  The SRTP
     * update processing routine will take care of modifying the metric
     * if necessary.
     */
    if (!vines_neighbor_update(pak, &neighbor, &path, &error,
			       VSRTP_VERSION, rtp->node_type)) {
	if (do_debug)
	    buginf(" (%s)", error);
	return;
    }	
    if (!neighbor || !path) {
	if (do_debug)
	    buginf("\nVSRTP: Could not create neighbor table entry.  Flushing packet.");
	return;
    }

    /*
     * Update some version specific fields in the neighbor structure.
     */
    path->controller = 0;
    path->machine    = 0;
    if (path->state == 0) {
	if (rtp->node_type == VSRTP_NODE_ROUTER) {
	    path->state = vsrtp_st_init;
	    /*
	     * Don't set up the retry counts here.  The first packet will
	     * either be a full update and we won't need to send a query,
	     * or it will cause a query to be sent and everything will be
	     * set up correctly.
	     */
	} else {
	    path->state = vsrtp_st_up;
	}
    }
   
    /*
     * Determine the correct major and minor inputs to the state machine.
     * This section also performs packet reassembly for RTP Update
     * packets and aborts if the update is not completely reassembled.
     * The packet must have an extra lock in this section in case it gets
     * left on the reassembly queue.
     */
    switch (rtp->operation) {
       case VSRTP_REDIRECT:
	vsrtp_fsm(pak, path, vsrtp_rx_redirect, 0);
	path->srtp_rx_redir_count++;
	break;
	
      case VSRTP_REINIT:
	vsrtp_fsm(pak, path, vsrtp_rx_reinit, 0);
	path->srtp_rx_reinit_count++;
	break;
	
      case VSRTP_REQUEST:
	request = (vsrtp_request *)rtp->data;
	minor = vsrtp_decode_request_minor(request->type);
	vsrtp_fsm(pak, path, vsrtp_rx_request, minor);
	path->srtp_rx_req_counts[minor-1]++;
	break;
	
      case VSRTP_UPDATE:
	/*
	 * Save some data from the packet before doing reassembly.  This
	 * way we don't have to find the headers again in the reassembled
	 * packet.  We also may not have a pak left if reassembly fails,
	 * but we're still going to need information from it.
	 */
	update = (vsrtp_update *)rtp->data;
	update_flags = update->flags;
	major = vsrtp_decode_update_major(update);
	sequence = GETLONG(&update->sequence);
	if (vip->srchost == VINES_SERVER) {
	    comparison = vsrtp_compare_seqno(path->origin_timestamp, sequence);
	} else {
	    comparison = EQUAL;
	}

	/*
	 * Keep a separate count of responses for NSM.  Interface
	 * granularity is not needed.
	 */
	if (update->type == VSRTP_UPD_RESPONSE)
	    vsrtp_rx_response_count++;
	if (comparison == LESS)
	    vsrtp_rx_outdated++;

	/*
	 * Reassemble it and test the results.  The reassemble routine
	 * may return a different packet that the packet passed in.  This
	 * will happen any time a reassembly completes.  The packet
	 * locking must remain here around the reassembly and processing
	 * of the reassembled update.  This maintains the integrity of
	 * the fragments being assembled after the network layer has
	 * released them.
	 */
	pak_lock(pak);
	result = VSRTP_REASS_FAILED;
	reassembled_pak = vsrtp_reassemble(path, pak, rtp, &result);
	switch (result) {
	  case VSRTP_REASS_FAILED:
	    minor = vsrtp_decode_update_minor(update_flags, -1);
	    path->srtp_rx_upd_counts[minor-1]++;
	    vsrtp_fsm(NULL, path, major, minor);
	    return;
	  case VSRTP_REASS_IGNORED:
	  case VSRTP_REASS_DUPLICATE:
	    datagram_done(pak);
	    return;
	  case VSRTP_REASS_ENQUEUED:
	    return;
	  case VSRTP_REASS_DONE:
	    break;
	  default:
	    errmsg(&msgsym(REASSEMBLY, VINES), result);
	    datagram_done(pak);
	    return;
	}

	/*
	 * Throw it at the state machine.
	 */
	minor = vsrtp_decode_update_minor(update_flags, comparison);
	path->srtp_rx_upd_counts[minor-1]++;
	vsrtp_fsm(reassembled_pak, path, major, minor);
	datagram_done(reassembled_pak);
	break;
	
      default:
	vsrtp_fsm(pak, path, vsrtp_unknown, 0);
	break;
    }
}


/*
 * vines_vsrtp_enabled
 *
 * Return TRUE if SRTP can be considered enabled on this interface.
 * Checks to see if SNAPSHOT is enabled on the interface as well; we can't
 * run SRTP if SNAPSHOT is on an interface...
 *
 * If phys_idb is NULL, just consider the global flag
 */
boolean vines_vsrtp_enabled (vinesidbtype *idb)
{
    if (idb == NULL)
        return(vsrtp_enabled);
    else
        return(vsrtp_enabled &&
	       !(reg_invoke_snapshot_enabled(idb->phys_idb)));
}
