/* $Id: igrp2.c,v 3.9.10.13 1996/09/10 18:09:29 sluong Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2.c,v $
 *------------------------------------------------------------------
 * IGRP2 generic routines.
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igrp2.c,v $
 * Revision 3.9.10.13  1996/09/10  18:09:29  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.9.10.12  1996/09/04  23:46:38  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.9.10.11  1996/08/28  12:52:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.10.10  1996/08/07  09:00:47  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.10.9  1996/07/06  05:54:56  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.10.8  1996/06/28  02:49:05  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.9.10.7  1996/06/21  16:51:43  sluong
 * CSCdi57042:  ipx floating static route kicks in after 4 min with EIGRP
 * Branch: California_branch
 *
 * Revision 3.9.10.6  1996/06/18  16:42:10  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.9.10.5  1996/04/03  00:57:19  dkatz
 * CSCdi53466:  EIGRP transport can hang
 * Branch: California_branch
 * Don't wait for acks that will never come.
 *
 * Revision 3.9.10.4  1996/04/02  07:37:30  dkatz
 * CSCdi52997:  IPX-EIGRP SAP actions may not happen for all neighbors
 * Branch: California_branch
 * Walk the correct thread.
 *
 * Revision 3.9.10.3  1996/04/02  07:23:36  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Fix a typo that left mondo weird stuff in the backup tables.  Get rid
 * of SAP CPUHOGs.  Turn split horizon back on by default.
 *
 * Revision 3.9.10.2  1996/04/02  05:37:04  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.9.10.1  1996/03/18  20:28:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.8.4  1996/03/16  06:56:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.8.3  1996/03/13  01:39:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.8.2  1996/03/07  09:49:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.8.1  1996/02/20  01:03:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/24  00:30:56  hampton
 * Implement suggested fix for negative EIGRP timer.  [CSCdi49676]
 *
 * Revision 3.8  1996/02/01  06:05:24  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/30  09:11:40  irfan
 * CSCdi47184:  EIGRP hellos go out on non-routing async interfaces
 * don't send hellos on or accept packets from static_routing interfaces.
 *
 * Revision 3.6  1996/01/08  23:03:48  bcole
 * CSCdi46504:  Static IP routes are not being installed
 *
 * Revision 3.5  1996/01/08  22:55:24  bcole
 * CSCdi46504:  Static IP routes are not being installed
 *
 * 64 bit timestamps are not transparent...
 * Change the way some timers are initialized to expire immediately so
 * that they work with 64 bit timestamps.
 *
 * Revision 3.4  1995/12/01  13:10:08  albright
 * CSCdi32436:  EIGRP routes lost when split horizon disabled; works with
 * it enabled
 *
 * Revision 3.3  1995/11/17  17:33:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:07:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/04  21:25:53  albright
 * CSCdi36672:  EIGRP neigbor tables not reflect correct uptime
 *
 * Add support for displaying a seconds based timestamp.
 *
 * Revision 2.10  1995/09/20  15:24:35  kramling
 * CSCdi38763:  CLI sh ip eigrp <x> needs to be truncated for any new IP
 *
 * Revision 2.9  1995/09/11  23:50:23  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.8  1995/09/07 17:02:41  mikel
 * CSCdi39889:  EIGRP: modularity violation
 * remove reference to IP specific variable and add check for
 * IGRP2_HOLDTIME
 *
 * Revision 2.7  1995/09/07  01:45:39  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.6  1995/08/14  15:07:26  albright
 * CSCdi35257:  router crashes, appears to be related to eigrp for ip
 *
 * Revision 2.5  1995/07/13  14:28:29  albright
 * CSCdi34641:  Crash in dual_getnext after adding passive interface to
 * EIGRP
 *
 * Revision 2.4  1995/06/28  03:21:13  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.3  1995/06/19 06:43:42  bcole
 * CSCdi36032: IP-EIGRP: Connected route for shutdown passive-if stays in
 * topo tbl
 *
 * Revert CSCdi34952 changes.  During an if flap, consider
 * passive-interfaces before deciding that there must be no topology table
 * entries which need to be removed.
 *
 * Revision 2.2  1995/06/19  01:07:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:06:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Transport protocol basics
 *
 * The EIGRP transport is a hybrid multicast/unicast beast.  It uses a single
 * sequence space across all interfaces and peers.
 *
 * Since only one sequence space is used, any single peer may receive only
 * certain packets in the sequence space.  This makes traditional windowing
 * schemes considerably more complicated, so the transport is stop-and-wait
 * (window size of 1).
 *
 * A reliable packet may be sent as either a multicast or a unicast.  The
 * receiving peer does not differentiate between the two.
 *
 * Multicast flow control is a bit funky.  A brute-force method would be to
 * send a single multicast and wait for all the unicast ACKs to arrive
 * before sending the next one.  This tends to make traffic flow suboptimal,
 * especially in the face of a recalcitrant (or deceased) peer.  To avoid
 * this problem, multicasts can be sent in a way that only flow-control-ready
 * peers can receive it (those that aren't ready will receive a unicast when
 * they are ready).  This mechanism is realized by preceding the packet with
 * a "Sequence TLV" packet that lists the peers that *must not* receive the
 * packet.  The multicast packet carries the "Conditional Receive" bit that
 * says that there must have been a preceding Sequence packet (to detect lost
 * sequence packets).  A new addition to the Sequence packet is the send
 * sequence number of the multicast packet;  this gives a two-way binding
 * between the packets and plugs a hole where a multicast could be received
 * by the wrong peers (due to a string of lost packets).
 *
 *
 * Implementation overview
 *
 * The implementation of this baroque protocol is itself rather baroque.
 *
 * First of all, there are two processes;  the Router process, and the
 * Hello process.  The Hello process does nothing but adjacency maintenance.
 * All received EIGRP packets first flow through the hello process.  Any
 * packet received from a peer will bump the adjacency holding timer back
 * up to the last time received in a hello.  No other processing of
 * received packets is done;  all received packets are requeued for the
 * Router process.  The Hello process also sends periodic multicast hello
 * packets out each interface.  These two functions reside in a separate
 * process so that the Router process can take large amounts of time to
 * do its thing without worrying about adjacencies dropping.
 *
 * The Router process does everything else.
 *
 * In general, packets are not built until just before being transmitted.
 * This is done in order to avoid sending data which is out of date (if
 * events are happening faster than packets can be sent).  Such packets
 * are represented canonically as a packet type and a serial number range
 * (representing a set of data in the topology database to be sent).  The
 * actual data is generated at the time the packet is to be transmitted.
 *
 * The transmission of packets is entirely timer-based.  This was done to
 * provide a rate control mechanism.  The transmission rate is controlled
 * on a per-interface basis (per subinterface if those are in use) and
 * is keyed on the configured interface bandwidth.  The packets are clocked
 * out a rate set to use no more than 50% of the bandwidth worst-case;
 * reliable packets are sent at a rate corresponding to a full MTU size,
 * whereas unreliable packets are sent at a rate corresponding to a minimum
 * packet size.  This gives a differential of about 37:1 between unreliable
 * and reliable packets (given a link slow enough to warrant a delay on
 * unreliable packets).
 *
 * Note that in cases where a packet is to be transmitted immediately, this
 * is done by setting the timer to expire in zero time.  This provides
 * consistency in terms of the interface;  it also provides a serialization
 * mechanism in case things are being driven from a different process thread;
 * the timer will "expire" when the owning process is ready to process it.
 *
 * Each interface has a pacing managed timer that controls the transmission
 * rate on that interface.  In addition, each peer has a send timer that
 * controls when the next unicast packet is to be transmitted to that peer
 * (subject to the restrictions of the interface pacing timer).
 *
 * Each interface has two transmit queues, one for reliable packets and
 * one for unreliable packets.  Similarly, each peer has two queues.
 * Each queue contains zero or more packet queue elements, which are
 * instantiations of a packet;  each queue element points at a packet
 * descriptor, which describes the packet (and possibly points to the
 * packet data itself).  The choice of which packet to be transmitted
 * at any given moment is determined partly by chance and partly by design;
 * unreliable packets are given priority over reliable packets, and
 * unicasts are given priority over multicasts, but limiters are used in
 * both cases to ensure that no queue is completely starved.
 *
 * All packets sent by the protocol engines are reliable;  unreliable packets
 * are only used by the transport engine for providing the reliable
 * service.
 *
 * Packet flow is somewhat twisty.  The process starts when the transport
 * indicates that it is flow control ready on an interface.  A callback
 * is made to the protocol module to alert it to this fact.  The protocol
 * module then decides what kind of packet to send, and calls igrp2_enqueue.
 * igrp2_enqueue creates a packet descriptor, enqueues it, and returns the
 * descriptor to the caller, who fills in the specifics (such as the
 * packet type and serial number range).  All reliable packets are
 * enqueued initially onto the interface reliable transmit queue, in order
 * to provide temporal ordering between unicast and multicast packets.
 * Sequence numbers are assigned at this time;  the transport uses a single
 * sequence number space for all peers on all interfaces (since it is by
 * necessity a sparse space anyhow).
 *
 * At some time in the future, the interface pacing timer fires, indicating
 * that it is time to send some kind of packet on the interface.  Assuming that
 * there are no unreliable packets to send, the next packet is taken from
 * the interface reliable queue.  If the packet is a unicast, it is moved
 * immediately to the peer unicast queue and the peer timer is kicked.
 * If it is a multicast, then a queue element is created for each peer on
 * the interface, representing the unicast packet that must be retransmitted
 * if the peer fails to acknowledge the unicast.  The queue element is set
 * to transmit after the retransmission interval.  A callback is then made
 * to the protocol module to actually build the packet, based on the descriptor
 * on the queue.  The packet, thus constructed, is transmitted as a multicast.
 *
 * If not all of the peers are flow-control ready when this process takes
 * place, the unicast queue element is set to transmit as soon as possible,
 * rather than after a retransmission interval, and the multicast is preceded
 * by a Sequence TLV to tell the laggard receivers not to listen to the packet.
 *
 * Under normal conditions, a series of uncast ACKs will arrive.  Each of
 * these is matched to the queue element enqueued on the peer transmit
 * queue.  The protocol module is called back with an indication that the
 * peer has acknowledged the packet (so that appropriate housekeeping can
 * be done) and the queue element is discarded.  When all peers have ack'ed
 * the multicast, the interface is made flow-control ready for multicast
 * again, the protocol module is called back with this indication, and
 * the process begins anew.
 *
 * If a peer timer expires, it means that a unicast packet is to be sent
 * (this could be either a standalone packet, or a retransmission of a
 * multicast).  If the interface pacing timer is running, the packet
 * is barred from being transmitted at that moment, so the peer timer is
 * deferred until the end of the pacing interval.  If the interface pacing
 * timer is stopped when the peer timer expires, the unicast packet is
 * built and transmitted (and the pacing timer is started).
 *
 * Flow control for each peer is on a stop-and-wait basis.  A new packet will
 * not be transmitted to a peer until the previous one is acknowledged.
 * If a peer has not acknowledged a previous packet and a multicast is to
 * be sent on an interface, the multicast will be marked so that the peer
 * will not receive it, and the packet will be added to the peer's unicast
 * queue.
 *
 * Flow control for multicasts is based on heuristics.  In theory there is
 * no need for multicast flow control, since any laggard peers will simply
 * get unicast copies later, but not having any flow control leads to
 * situations where a multicast is transmitted, nobody is allowed to receive
 * it, and then separate unicast copies are sent to each peer.  Instead,
 * a "best effort" flow control scheme is used.  When a multicast is
 * transmitted, the interface is marked as being flow-blocked for multicast.
 * Under normal circumstances, the interface will become flow-ready when
 * all peers have acknowledged the previous multicast (effectively slowing
 * the transmission rate to the capacity of the slowest peer).  If one
 * peer is excessively slow, however, this is unreasonable, so when a
 * multicast is sent, a timer is started;  if the timer expires and at
 * least one peer has acknowledged the packet, the interface will become
 * flow control ready again.  The setting of this timer is based on the
 * pacing interval and the mean SRTT of the peers on the interface.
 *
 * Multipoint NBMA interfaces are treated a bit differently.  Since such
 * interfaces really don't support native multicast, packets addressed to
 * a multicast address are replicated by the device driver.  If this were
 * done for EIGRP packets, it would subvert the pacing control mechanism,
 * so we don't use it.  Instead, when the protocol engine hands down a
 * multicast, the queue element is replicated for each peer as usual, but
 * rather than being scheduled to be sent after the retransmission interval,
 * they are scheduled to be sent at intervals equal to the pacing timer.
 * The multicast packet is never transmitted as such.
 *
 * Point-to-point interfaces are treated as the degenerate case of NBMA
 * interfaces;  the multicast is requeued as a unicast.
 *
 * See dual.c for a discussion of how the topology table is organized to
 * interface with the transport implementation.
 */

/*
 * Wacky Startup Sequence
 *
 * The lack of a three-way handshake leads to some interesting synchronization
 * consequences.  In particular, it is possible to become stuck in the peer
 * INIT state in the following way:  if one peer acknowledges its neighbor's
 * INIT packet without sending its own, and then the peer drops and
 * reacquires the neighbor again, the neighbor will have no way of telling
 * that the peer has lost its initial state.  The peer will receive packets
 * from the neighbor without the INIT bit and will discard them;  eventually
 * it will tear down the adjacency and hopefully things will sync up the
 * next time around.
 *
 * A workaround is to never send explicit acknowledgements while
 * waiting for your own INIT packet to be acknowledged, but rather to let
 * the acknowledgement be piggybacked with your outgoing INIT packets.
 * This makes it impossible for the above scenario to occur, since it cannot
 * be the case that one INIT can be acknowledged without the neighbor seeing
 * the INIT in the other direction.
 *
 * Sheesh.
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "../os/chunk.h"
#include "packet.h"
#include "address.h"
#include "../ip/ip.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "dual.h"
#include "route.h"
#include "igrp2-inlines.h"
#include "igrp2_debug.h"
#include "logger.h"
#include "subsys.h"
#include "parser_defs_igrp2.h"
#include "sys_registry.h"
#include "eigrp_registry.h"
#include "msg_igrp2.c"			/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "config.h"

/* Local storage */

static ddbtype *debug_ddb;		/* Filter debug info from this DDB */
static ulong igrp2_process_count;	/* Count of running EIGRP processes */
static chunk_type *igrp2_pakdesc_chunks; /* Packet descriptors */
static chunk_type *igrp2_pakdesc_qelm_chunks; /* Queue elements */
chunk_type *igrp2_anchor_chunks;	/* Anchors */
chunk_type *igrp2_dummy_chunks;		/* Dummies */
chunk_type *igrp2_inpak_chunks;		/* Input packet headers */

/*
 * Hook to induce errors (by dropping outgoing and incoming packets)
 */
#ifdef IGRP2_INDUCE_ERRORS
#include "../os/clock_internal.h"
#endif /* IGRP2_INDUCE_ERRORS */


/*
 * igrp2_pdebug
 * Decide if packet debugging output should be generated for this particular
 * peer.
 *
 * If the address pointer is NULL, we allow debugging *unless* a peer match
 * was specified.
 */

static boolean igrp2_pdebug (ddbtype *ddb, igrp2addrtype *addr)
{
    if (!igrp2_packet_debug)
	return(FALSE);
    if (debug_ddb && debug_ddb != ddb)
	return(FALSE);
    if (!addr && ddb->debug_peer_set)
	return(FALSE);
    if (addr && ddb->debug_peer_set &&
	!(*ddb->addrmatch)(addr, &ddb->debug_peer))
	return(FALSE);
    return(TRUE);
}

/*
 * igrp2_set_debug_ddb
 *
 * Set the target DDB for debug output filtering.
 */
void igrp2_set_debug_ddb (ddbtype *ddb)
{
    debug_ddb = ddb;
}

#ifdef IGRP2_INDUCE_ERRORS

/*
 * igrp2_induce_error
 *
 * Returns TRUE if an error should be induced, FALSE if not.
 */
static inline boolean igrp2_induce_error (ddbtype *ddb, igrp2_idbtype *iidb,
					  char *string)
{
    clock_epoch pick_a_card;
    clock_get_time_exact(&pick_a_card);
    if (((pick_a_card.epoch_frac >> 12)%128) >= iidb->iidb_error_rate) {
	return(FALSE);			/* We beat the odds */
    } else {
	if (igrp2_pdebug(ddb, NULL)) {
	    buginf("\nEIGRP: %s error induced", string);
	}
	return(TRUE);
    }
}

#else

static inline boolean igrp2_induce_error (ddbtype *ddb, igrp2_idbtype *iidb,
					  char *string)
{
    return(FALSE);
}

#endif


/*
 * igrp2_log_peer_change
 *
 * Log a change in peer status
 */
static void igrp2_log_peer_change (ddbtype *ddb, dual_peertype *peer,
				   boolean sense, char *msg)
{
    if (ddb->log_adj_change) {
	errmsg(&msgsym(NBRCHANGE, DUAL), ddb->name, ddb->asystem,
	       (*ddb->printaddr)(&peer->source), peer->iidb->idb->namestring,
	       sense ? "up" : "down", msg);
    }
}

/*
 * igrp2_xmit_debug_ok
 *
 * Returns TRUE if the transmit debug message should be emitted, given the
 * current flag setting.
 *
 * If the DETAIL bit is set in the message, it must also be set in the debug
 * flags.
 *
 * Otherwise, if there is any overlap between the message bits and the
 * debug flags, the message is sent.
 */
boolean igrp2_xmit_debug_ok (ulong flags)
{
    if ((flags & XDBDETAIL) && !(igrp2_xmit_debug_flags & XDBDETAIL))
	return(FALSE);			/* No detail */
    if (flags & igrp2_xmit_debug_flags & (~XDBDETAIL))	/* Some overlap */
	return(TRUE);
    return(FALSE);
}

/*
 * igrp2_detail_debug
 *
 * Returns TRUE if detailed packet debugging (terse or verbose) is enabled.
 */
static boolean igrp2_detail_debug (ddbtype *ddb, igrp2addrtype *addr)
{
    return (igrp2_pdebug(ddb, addr) &&
	    ((igrp2_packet_debug_flags & IGRP2_DEBUG_TERSE) ==
	     IGRP2_DEBUG_TERSE));
}

/*
 * igrp2_multicast_transmit_blocked
 *
 * Returns TRUE if the interface is blocked for transmitting multicast
 * packets.  This is the case if a multicast has been transmitted but it
 * has not yet been acknowledged by all of the receivers, and the multicast
 * flow timer has not expired.
 *
 * Note that this routine will return FALSE if there are multicasts queued
 * to be transmitted but no transmission is outstanding.  Don't confuse
 * this routine with igrp2_interface_is_flow_ready (below).
 */
static inline boolean igrp2_multicast_transmit_blocked (igrp2_idbtype *iidb)
{
    /* It's flow-blocked if there's a multicast packet lingering. */

    return (iidb->last_multi_pak != NULL);
}

/*
 * igrp2_peer_is_flow_ready
 *
 * Returns TRUE if the peer is flow-control ready, meaning that a new unicast
 * packet for this peer would be eligible to be transmitted immediately
 * (ignoring the pacing timer).  This is the case only if both the multicast
 * and unicast queues are empty.  This will be the case only if the interface
 * is completely quiescent, or if a single multicast was transmitted and the
 * peer in question has already acknowledged it (other peers may not have).
 *
 * If this routine returns false, it is guaranteed that a transmit_done
 * callback will happen at some point in the future.
 */
boolean igrp2_peer_is_flow_ready (dual_peertype *peer)
{
    /* Straightforward. */

    return(!(peer->xmitQ[RELIABLE_QUEUE].qhead ||
	     peer->iidb->xmitQ[RELIABLE_QUEUE].qhead ||
	     peer->going_down));
}

/*
 * igrp2_interface_is_flow_ready
 *
 * Returns TRUE if the interface is flow-control ready, meaning that a new
 * multicast packet would be eligible to be transmitted immediately on this
 * interface (ignoring the pacing timer).  This is the case only if there
 * are no outstanding multicasts on this interface, or if there is a single
 * outstanding multicast and the multicast flow control timer has expired.
 * This will allow a new multicast to be transmitted even though some peers
 * may be slow in acknowledging the last one.
 *
 * If this routine returns FALSE, it is guaranteed that a transmit_done
 * callback will happen at some point in the future.
 */
boolean igrp2_interface_is_flow_ready (igrp2_idbtype *iidb)
{
    /* Straightforward */

    return(!(iidb->xmitQ[RELIABLE_QUEUE].qhead ||
	     igrp2_multicast_transmit_blocked(iidb) ||
	     iidb->iidb_going_down ||
	     igrp2_peer_count(iidb) == 0));
}

/*
 * igrp2_defer_peer_timer
 *
 * Set the peer timer to expire at the same time as the IIDB circuit
 * pacing timer.  If the IIDB timer isn't running, don't touch the
 * peer timer.
 *
 * Returns TRUE if the timer was set, FALSE if not.
 */
static boolean igrp2_defer_peer_timer (ddbtype *ddb,
				       igrp2_idbtype *iidb,
				       dual_peertype *peer)
{
    sys_timestamp exptime;

    if (mgd_timer_running(&iidb->iidb_send_timer)) {
	exptime = mgd_timer_exp_time(&iidb->iidb_send_timer);
	mgd_timer_set_exptime(&peer->peer_send_timer, &exptime);
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * igrp2_next_peer_queue
 *
 * Returns the type of the transmit queue (reliable/unreliable) from
 * which the next packet should be taken to send to the peer.
 *
 * The rule is that unrelilable packets take precedence until
 * peer->unrely_left_to_send is zero, at which point a reliable packet
 * will be sent (to keep the unreliables from starving the reliable queue).
 */
static xmit_queue_type igrp2_next_peer_queue (ddbtype *ddb,
					      dual_peertype *peer)
{
    igrp2_pakdesc_qelm *qelm;

    if (peer->xmitQ[UNRELIABLE_QUEUE].count) {

	/*
	 * Send unreliable packets if there are some present, and if we're
	 * still allowed to send them, or if there are no reliable packets
	 * to send, or if the reliable packet is waiting to be sent.
	 */
	qelm = peer->xmitQ[RELIABLE_QUEUE].qhead;
	if (peer->unrely_left_to_send || !qelm)
	    return(UNRELIABLE_QUEUE);
	if (TIMER_RUNNING_AND_SLEEPING(qelm->xmit_time)) {
	    return (UNRELIABLE_QUEUE);
	}
    }
    if (igrp2_detail_debug(ddb, &peer->source) &&
	!peer->unrely_left_to_send &&
	peer->xmitQ[UNRELIABLE_QUEUE].count &&
	peer->xmitQ[RELIABLE_QUEUE].count) {
	buginf("\nEIGRP:  Forcing reliable xmit to %s on %s",
	       (*ddb->printaddr)(&peer->source),
	       peer->iidb->idb->namestring);
    }
    return(RELIABLE_QUEUE);
}

/*
 * igrp2_start_peer_timer
 *
 * Start the transmit/retransmit timer for a peer.
 */
static void igrp2_start_peer_timer (ddbtype *ddb, igrp2_idbtype *iidb,
				    dual_peertype *peer)
{
    igrp2_pakdesc_qelm *qelm;
    queuetype *send_queue;

    /* If both transmit queues are empty, stop the timer and bail. */

    send_queue = &peer->xmitQ[igrp2_next_peer_queue(ddb, peer)];
    if (!send_queue->count) {
	if (mgd_timer_running(&peer->peer_send_timer))
	    mgd_timer_stop(&peer->peer_send_timer);
	return;
    }

    /* Grab the first element on the queue. */

    qelm = send_queue->qhead;

    /*
     * If there is no transmit time set on this packet, set it to expire
     * at the next pacing interval on the interface.  If the interface
     * timer isn't running, set the peer timer to expire now.
     */
    if (!TIMER_RUNNING(qelm->xmit_time)) {
	if (!igrp2_defer_peer_timer(ddb, iidb, peer)) {
	    mgd_timer_start(&peer->peer_send_timer, 0);
	}
    } else {

	/* Set the timer to the time when the packet is to be sent. */

	mgd_timer_set_exptime(&peer->peer_send_timer, &qelm->xmit_time);
    }
}

/*
 * igrp2_kick_interface
 *
 * Start the interface by kicking the flow control timer if it isn't already
 * running.  This will cause the upper levels to be kicked when the timer
 * expires (immediately).
 *
 * This routine is callable from other threads.
 */
static void igrp2_kick_interface (ddbtype *ddb, igrp2_idbtype *iidb)
{
    /* Kick it if the flow control timer isn't running. */

    if (!mgd_timer_running(&iidb->iidb_flow_control_timer)) {
	mgd_timer_start(&iidb->iidb_flow_control_timer, 0);
    }
}

/*
 * igrp2_kick_peer
 *
 * Start the peer by kicking the peer timer if it isn't already
 * running.  This will cause the upper levels to be kicked when the timer
 * expires (immediately).
 *
 * This routine is callable from other threads.
 */
static void igrp2_kick_peer (dual_peertype *peer)
{
    /* Kick it if the peer timer isn't running. */

    if (!mgd_timer_running(&peer->peer_send_timer)) {
	mgd_timer_start(&peer->peer_send_timer, 0);
    }
}

/*
 * igrp2_kick_pacing_timer
 *
 * Starts a pacing timer if it is stopped, but leaves it alone if it
 * is already running.
 */
static void igrp2_kick_pacing_timer (ddbtype *ddb, igrp2_idbtype *iidb,
				     ulong delay)
{
    mgd_timer *timer;

    timer = &iidb->iidb_send_timer;
    if (!mgd_timer_running(timer)) {
	mgd_timer_start(timer, delay);
    }
}

/*
 * igrp2_bind_pakdesc
 *
 * Bind a packet descriptor to a queue element.  Manipulates the reference
 * count in the packet descriptor.
 */
static void igrp2_bind_pakdesc(igrp2_pakdesc *pakdesc,
			       igrp2_pakdesc_qelm *qelm)
{
    qelm->pakdesc = pakdesc;
    pakdesc->refcount++;
}

/*
 * igrp2_free_qelm
 *
 * Free a queue element after unbinding it from the packet descriptor.
 * Manipulates the reference count in the packet descriptor.  Frees the
 * packet descriptor if the last reference is being unbound.
 *
 * This is also the locus of control for passing Transmit Ready indications
 * back to DUAL.  We will alert DUAL if the packet is reliable.
 */
static void igrp2_free_qelm (ddbtype *ddb, igrp2_pakdesc_qelm *qelm,
			     igrp2_idbtype *iidb, dual_peertype *peer)
{
    igrp2_pakdesc *pakdesc;

    if (qelm == NULL)
	return;

    pakdesc = qelm->pakdesc;
    if (pakdesc) {

	/* Drop the reference count. */

	pakdesc->refcount--;
	if (pakdesc->refcount < 0) {
	    errmsg(&msgsym(REFCOUNT, DUAL), pakdesc);
	}

	/* Notify DUAL if appropriate. */

	if (pakdesc->sequenced) {
	    dual_xmit_continue(ddb, iidb, pakdesc, peer);
	}

	/* If all references are gone, free the packet. */

	if (pakdesc->refcount <= 0) {
	    chunk_free(igrp2_pakdesc_chunks, pakdesc);
	}
    }
    chunk_free(igrp2_pakdesc_qelm_chunks, qelm);
}

/*
 * igrp2_cleanup_multipak
 *
 * Clean out the multicast packet pointer.  We need to coordinate things
 * carefully, so we do it in one place.  How unusual!
 *
 * Make sure that we zap last_multi_pak before freeing the queue element,
 * since DUAL may call back on that thread to see if we're flow control
 * ready.
 */
static void igrp2_cleanup_multipak (ddbtype *ddb, igrp2_idbtype *iidb)
{
    igrp2_pakdesc_qelm *qelm;

    qelm = iidb->last_multi_pak;
    if (qelm == NULL)
	return;

    iidb->last_multi_pak = NULL;
    iidb->mcast_start_refcount = 0;
    igrp2_free_qelm(ddb, qelm, iidb, NULL);
}

/*
 * igrp2_start_mcast_flow_timer
 *
 * Start the flow control timer.  We set it to a multiple of the average
 * SRTT, with bounds.  For non-multicast interfaces we include the
 * number of peers multiplied by the pacing interval, since these packets
 * are sent serially at the pacing interval.
 *
 */
static void igrp2_start_mcast_flow_timer (ddbtype *ddb, igrp2_idbtype *iidb)
{
    ulong timer_value;
    ulong mean_srtt;

    timer_value = 0;
    if (igrp2_peer_count(iidb) > 0) {

	mean_srtt = iidb->total_srtt / igrp2_peer_count(iidb);

	/* Set basic value based on average SRTT. */

	timer_value = mean_srtt * IGRP2_FLOW_SRTT_MULTIPLIER;

	/* Throw in a delay for NBMA interfaces. */

	if (!iidb->use_multicast) {
	    timer_value += iidb->iidb_send_interval[RELIABLE_QUEUE] *
		igrp2_peer_count(iidb);
	}
    }

    /* Place an absolute lower bound in case pacing is turned off. */
    
    timer_value = max(IGRP2_MIN_MCAST_FLOW_DELAY, timer_value);
    iidb->last_mcast_flow_delay = timer_value; /* For display purposes */
    mgd_timer_start(&iidb->iidb_flow_control_timer, timer_value);
}

/*
 * igrp2_update_mcast_flow_state
 *
 * Update the multicast flow control state.  Decides whether or not the
 * interface should remain flow-blocked, and updates the state as necessary.
 *
 * We make the interface flow-control ready if either all copies of the
 * packet have been acknowledged, or if the timer has expired and at
 * least one copy of the packet has been acknowledged.
 *
 * The multicast flow control timer is used to wake up the interface from
 * other threads (by forcing it to expire).  This particular condition is
 * noticable by virtue of the timer having expired but there being no
 * pending multicast packet.
 */
static void igrp2_update_mcast_flow_state (ddbtype *ddb, igrp2_idbtype *iidb)
{
    igrp2_pakdesc_qelm *qelm;
    igrp2_pakdesc *pakdesc;
    boolean just_became_ready;

    just_became_ready = FALSE;
    qelm = iidb->last_multi_pak;
    if (qelm) {
	pakdesc = qelm->pakdesc;
    } else {
	pakdesc = NULL;
    }

    /* If it's not blocked and the timer isn't running, we're already done. */

    if (!igrp2_multicast_transmit_blocked(iidb) &&
	!mgd_timer_running(&iidb->iidb_flow_control_timer)) {
	return;
    }

    /*
     * If all copies of the packet have been acknowledged, we are now flow
     * control ready.  We determine this by looking at the pakdesc refcount;
     * if it is == 1, ours is the only copy.
     */
    if (pakdesc && pakdesc->refcount == 1) {	/* Only us! */
	just_became_ready = TRUE;

    } else {

	/*
	 * If the timer has just expired, check to see if anybody has acked
	 * the last multicast packet or not.  If not, there's no reason to go
	 * on;  set the timer for another round.  Otherwise, stop the timer
	 * and continue.
	 */
	if (mgd_timer_expired(&iidb->iidb_flow_control_timer)) {
	    if (pakdesc && pakdesc->refcount == iidb->mcast_start_refcount) {
		igrp2_start_mcast_flow_timer(ddb, iidb);
		return;
	    }
	    mgd_timer_stop(&iidb->iidb_flow_control_timer);
	}

	/*
	 * If the timer has expired and there has been at least one
	 * acknowledgement, we are now flow ready.  If there is no packet
	 * pending, we are now flow ready (having been kicked).
	 */
	if (!mgd_timer_running(&iidb->iidb_flow_control_timer)) {
	    if (pakdesc) {
		if ((pakdesc->refcount < iidb->mcast_start_refcount) ||
		    iidb->mcast_start_refcount <= 1) { /* Just in case */
		    just_became_ready = TRUE;
		}
	    } else {			/* No packet pending */
		just_became_ready = TRUE;
	    }
	}
    }

    /*
     * Now ready.  Free the queue element (making us flow ready) and kick the
     * pacing timer.  Kill the flow control timer if it's still running.
     * Tell the upper levels.
     */
    if (just_became_ready) {
	if (igrp2_detail_debug(ddb, NULL)) {
	    buginf("\nEIGRP: %s multicast flow blocking cleared",
		   iidb->idb->namestring);
	}
	dual_log_xport_all(ddb, DUALEV_MCASTRDY, &iidb->idb, NULL);
	IGRP2_XBUGINF(XDBPACKETIZE|XDBDETAIL,
		      "\nFlow blocking cleared on %s", iidb->idb->namestring);
	igrp2_cleanup_multipak(ddb, iidb); /* May free pakdesc! */
	igrp2_kick_pacing_timer(ddb, iidb, 0);
	mgd_timer_stop(&iidb->iidb_flow_control_timer);
    }
}

/*
 * igrp2_flush_queue
 *
 * Flush all the elements out of a transmit queue, freeing the queue
 * elements.
 *
 * This is used to empty out the IIDB queues before we destroy it.
 */
static void igrp2_flush_queue (ddbtype *ddb, igrp2_idbtype *iidb,
			       queuetype *queue)
{
    igrp2_pakdesc_qelm *qelm;

    while ((qelm = dequeue(queue))) {
	igrp2_free_qelm(ddb, qelm, iidb, NULL);
    }
}

/*
 * igrp2_dequeue_qelm
 *
 * Dequeue and free the first queue element from a peer queue.  Unbinds (and
 * potentially frees) the corresponding packet descriptor.  May make
 * the associated interface multicast flow ready again.
 */
static void igrp2_dequeue_qelm (ddbtype *ddb, dual_peertype *peer,
				xmit_queue_type qtype)
{
    igrp2_pakdesc_qelm *qelm;
    igrp2_idbtype *iidb;
    ulong seq_num;

    /* Pull the entry from the queue. */

    qelm = dequeue(&peer->xmitQ[qtype]);
    if (qelm) {				/* Something is there? */
	seq_num = qelm->pakdesc->ack_seq_num; /* Grab it before we free! */
    
	/* Free the queue element and perhaps the packet as well. */
	
	igrp2_free_qelm(ddb, qelm, peer->iidb, peer);
	
	/*
	 * If the packet is sequenced and the sequence number matches the
	 * last multicast sent, update the flow control state.
	 */
	iidb = peer->iidb;
	if (qtype == RELIABLE_QUEUE && seq_num == iidb->iidb_last_mcast_seq) {
	    igrp2_update_mcast_flow_state(ddb, peer->iidb);
	}
    }
}

/*
 * igrp2_getpak
 * Moan if we can't get a packet buffer.
 */

paktype *igrp2_getpak (int size)
{
    paktype *pak;

    pak = getbuffer(size);
    if (!pak)
	errmsg(&msgsym(NOBUFFER, DUAL), size);
    return(pak);
}

/*
 * igrp2_getbuf
 * Get a buffer to build packet into.
 */

igrp2type *igrp2_getbuf (ushort nbytes)
{
    igrp2type *igrp2;

    igrp2 = malloc(IGRP2HEADERBYTES + nbytes);
    return(igrp2);
}

/*
 * igrp2_handle_to_peer
 *
 * Returns a pointer to the peer structure, given the peer handle number.
 */
dual_peertype *igrp2_handle_to_peer (ddbtype *ddb, int handle)
{
    if (handle == NO_PEER_HANDLE)
	return(NULL);
    if ((ulong) handle >= ddb->peer_array_size)	/* Covers negatives as well */
	return(NULL);
    return(ddb->peer_array[handle]);
}

/*
 * igrp2_adjust_peer_array
 *
 * Set the peer array entry to accommodate the new peer.  If the array isn't
 * big enough, reallocate it.
 *
 * Returns TRUE if success, FALSE if out of memory.
 */
#define PEER_TABLE_INCREMENT 10		/* Number of entries to add */
static boolean igrp2_adjust_peer_array (ddbtype *ddb, dual_peertype *peer,
				     int handle)
{
    dual_peertype **new_table;
    ulong new_table_size;

    /* If the array is too small, reallocate it. */

    if (handle >= ddb->peer_array_size) {
	new_table_size = max(ddb->peer_array_size + PEER_TABLE_INCREMENT,
			     handle + 1); /* Be robust. */
	new_table = malloc(sizeof(dual_peertype *) * new_table_size);
	if (!new_table)			/* Uhoh */
	    return(FALSE);
	if (ddb->peer_array) {
	    bcopy(ddb->peer_array, new_table,
		  sizeof(dual_peertype *) * ddb->peer_array_size);
	    free(ddb->peer_array);
	}
	ddb->peer_array = new_table;
	ddb->peer_array_size = new_table_size;
    }
    ddb->peer_array[handle] = peer;
    return(TRUE);
}

/*
 * igrp2_adjust_handle_array
 *
 * Ensures that the handle array is big enough for the new peer;  if it isn't,
 * it is reallocated.
 *
 * Returns TRUE if success, FALSE if out of memory.
 */
static boolean igrp2_adjust_handle_array (igrp2_handle_type *handles,
					  int handle_number)
{
    ulong *new_handle_array;
    ulong new_handle_array_size;

    if (handle_number == NO_PEER_HANDLE)
	return(FALSE);

    /* If the handle is within range, we're done. */

    if (handles->handle_array_size > HANDLE_TO_CELL(handle_number)) {
	return(TRUE);
    }

    /* We've gotten too big.  Allocate a new array. */

    new_handle_array_size = HANDLE_TO_CELL(handle_number) + 1;
    new_handle_array = malloc(HANDLE_MALLOC_SIZE(new_handle_array_size));
    if (!new_handle_array) {		/* Uhoh */
	return(FALSE);
    }

    /* Gotten the bigger and better one.  Copy over the old one, if present. */

    if (handles->handle_array) {
	bcopy(handles->handle_array, new_handle_array,
	      HANDLE_MALLOC_SIZE(handles->handle_array_size));
	free(handles->handle_array);
    }
    handles->handle_array = new_handle_array;
    handles->handle_array_size = new_handle_array_size;
    return(TRUE);
}

/*
 * igrp2_set_handle
 *
 * Set a peer's handle in a handle structure.
 */
static void igrp2_set_handle (ddbtype *ddb, igrp2_handle_type *handles,
			      int handle_number)
{
    if (handle_number == NO_PEER_HANDLE) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    if (handle_number >= CELL_TO_HANDLE(handles->handle_array_size)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    if (TEST_HANDLE(handles->handle_array, handle_number)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    SET_HANDLE(handles->handle_array, handle_number);
    handles->handles_used++;
}

/*
 * igrp2_clear_handle
 *
 * Clears a freed handle in a handle structure.
 */
void igrp2_clear_handle (ddbtype *ddb, igrp2_handle_type *handles,
			 int handle_number)
{
    if (handle_number == NO_PEER_HANDLE) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    if (handle_number >= CELL_TO_HANDLE(handles->handle_array_size)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    if (!TEST_HANDLE(handles->handle_array, handle_number)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    CLEAR_HANDLE(handles->handle_array, handle_number);
    if (!handles->handles_used) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    handles->handles_used--;
}

/*
 * igrp2_test_handle
 *
 * Tests to see if a handle is set in a handle structure.
 */
boolean igrp2_test_handle (ddbtype *ddb, igrp2_handle_type *handles,
			   int handle_number)
{
    if (handle_number == NO_PEER_HANDLE) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }
    if (handle_number >= CELL_TO_HANDLE(handles->handle_array_size))
	return(FALSE);			/* Out of range... */
    return(TEST_HANDLE(handles->handle_array, handle_number));
}

/*
 * igrp2_calculate_pacing_interval
 *
 * Calculates the pacing interval, given the bandwidth, packet size,
 * and whether the packet is sequenced or not.
 */
static ulong igrp2_calculate_pacing_interval (ddbtype *ddb,
					      igrp2_idbtype *iidb,
					      ulong length,
					      boolean sequenced)
{
    ulong interval, bandwidth, packet_size_bits_x_100, denominator, peer_count;

    bandwidth = iidb->idb->visible_bandwidth;
    if (bandwidth == 0) {
	bandwidth = 1;
    }

    /*
     * The calculation is effectively (data_size / (fraction * bandwidth)).
     * We reorder things to avoid truncation and to minimize the number
     * of multiplications and divisions necessary.
     */
    packet_size_bits_x_100 = (length + ddb->packet_overhead +
			IGRP2HEADERBYTES) * 8 * 100;
    denominator = bandwidth * iidb->igrp2_bandwidth_percentage;
    if (denominator == 0)
	denominator = 1;
    interval = packet_size_bits_x_100 / denominator;

    if (sequenced) {

	/*
	 * Sequenced packets are subject to a minimum interval, and are
	 * scaled upward according to the number of neighbors on
	 * multicast-capable interfaces.
	 */
	peer_count = igrp2_peer_count(iidb);
	if (iidb->use_multicast && (peer_count > 1))
	    interval *= peer_count / 2;
	interval = max(IGRP2_MIN_PACING_IVL, interval);

    } else {

	/* Unsequenced packets run at wire speed above T1. */

	if (bandwidth >= IGRP2_NO_UNRELIABLE_DELAY_BW) {
	    interval = 0;
	}
    }
    return(interval);
}

/*
 * igrp2_set_pacing_intervals
 *
 * Set the pacing intervals based on the interface bandwidth and peer count.
 */

static void igrp2_set_pacing_intervals (ddbtype *ddb, igrp2_idbtype *iidb)
{
    /*
     * The send intervals are calculated as a function of the bandwidth.
     * The target is to limit EIGRP to a fraction of the bandwidth.  This is
     * calculated based on the size of an ACK for the unreliable queue,
     * and the size of an MTU-sized update for the reliable queue.
     *
     * For links above 1Mbps, the interpacket gap for unreliable packets
     * is eliminated.
     *
     * The interpacket gap for reliable packets has an absolute minimum;
     * this is done because in practice the CPU load of processing the
     * incoming packets becomes the limiting factor.
     *
     * The interpacket gap for reliable packets also factors in the number
     * of peers on the interface, if the interface is multicast-capable.
     * This is done in an attempt to avoid flooding any receivers, since
     * an event on a LAN can cause everyone to transmit more or less
     * simultaneously.  If all transmitters back off when there are a lot
     * of them, then the receivers won't be quite as stressed.  We multiply
     * the pacing interval by half the number of neighbors.
     *
     * The interval calculated here is the base interval for the largest
     * packets.  When a packet is actually transmitted, the instantaneous
     * interval is based on the size of the packet.  The base value is used
     * in other calculations that include the pacing interval (such as the
     * multicast flow control timer).
     */

    /* First the reliable queue. */

    iidb->iidb_send_interval[RELIABLE_QUEUE] =
	igrp2_calculate_pacing_interval(ddb, iidb, iidb->max_packet_size, 
					TRUE);

    /* Now the unreliable queue. */

    iidb->iidb_send_interval[UNRELIABLE_QUEUE] =
	igrp2_calculate_pacing_interval(ddb, iidb, 0, FALSE);

    /* Use the base value as our current value. */

    iidb->iidb_last_send_interval[RELIABLE_QUEUE] =
	iidb->iidb_send_interval[RELIABLE_QUEUE];
    iidb->iidb_last_send_interval[UNRELIABLE_QUEUE] =
	iidb->iidb_send_interval[UNRELIABLE_QUEUE];
}

/*
 * igrp2_allochandle
 * Find first free handle, set it and adjust as necessary.  Adjusts peer
 * array as well.
 */
static void igrp2_allochandle (ddbtype *ddb, dual_peertype *peer)
{
    ulong bits, handle, cell;
    ulong *cell_ptr, *target;
    igrp2_idbtype *iidb;

    /* First search for a word with a clear bit. */

    cell_ptr = ddb->ddb_handles.handle_array;
    target = NULL;
    handle = NO_PEER_HANDLE;
    for (cell = 0; cell < ddb->ddb_handles.handle_array_size;  cell++) {
	if (*cell_ptr != ~0) {
	    target = cell_ptr;
	    break;
	}
	cell_ptr++;
    }

    /* If we found a word with a free bit, find the bit. */

    if (target) {
	bits = *target;
	handle = CELL_TO_HANDLE(cell);
	while (bits & 0x1) {		/* Better terminate! */
	    bits >>= 1;
	    handle++;
	}

    } else {

	/*
	 * No words with any free bits.  Grow the DDB handle array in
	 * reaction to this unfortunate turn of events.  We then use the
	 * first bit in the new word.
	 */
	handle = CELL_TO_HANDLE(ddb->ddb_handles.handle_array_size);
	if (!igrp2_adjust_handle_array(&ddb->ddb_handles, handle)) /* Uhoh */
	    handle = NO_PEER_HANDLE;
    }

    /* Now try to grow the IIDB handles array if necessary. */

    iidb = peer->iidb;
    if (!igrp2_adjust_handle_array(&iidb->iidb_handles, handle)) /* Uhoh */
	handle = NO_PEER_HANDLE;

    if (handle != NO_PEER_HANDLE) {	/* We've got all our memory */

	/* Add the peer array entry and set the bits in the handles. */

	if (igrp2_adjust_peer_array(ddb, peer, handle)) { /* It worked */

	    igrp2_set_handle(ddb, &ddb->ddb_handles, handle);
	    igrp2_set_handle(ddb, &iidb->iidb_handles, handle);
	    igrp2_set_pacing_intervals(ddb, iidb); /* This may change pacing */
	}
    }
    peer->peer_handle = handle;
}

/*
 * igrp2_freehandle
 *
 * Clear out handle bit assigned to this peer.
 */
void igrp2_freehandle (ddbtype *ddb, dual_peertype *peer)
{
    igrp2_idbtype *iidb;
    ulong handle;

    handle = peer->peer_handle;

    iidb = peer->iidb;
    igrp2_clear_handle(ddb, &ddb->ddb_handles, handle);
    igrp2_clear_handle(ddb, &iidb->iidb_handles, handle);
    igrp2_set_pacing_intervals(ddb, iidb); /* This may change pacing. */

    if (ddb->peer_array_size > handle)
	ddb->peer_array[handle] = NULL;
}

/*
 * igrp2_findhandle
 * Scan peercache looking for nexthop
 */

int igrp2_findhandle (
    ddbtype *ddb,
    igrp2addrtype *nexthop,
    idbtype *interface)
{
    dual_peertype *peer;
    ulong handle;

    peer = igrp2_find_peer(ddb, nexthop, interface);
    if (peer != NULL) {
	handle = peer->peer_handle;

	/*
	 * We probably should do something other than return the buggar
	 * if it is not already allocated to this peer.
	 */
	if (!igrp2_test_handle(ddb, &ddb->ddb_handles, handle))
	    errmsg(&msgsym(NOHANDLE, DUAL), handle);
	return(handle);
    } else {
	return(NO_PEER_HANDLE);
    }
}

/*
 * igrp2_cdelay
 * Convert the delay into nanoseconds
 */

ulong igrp2_cdelay (ulong delay)
{
    unsigned long long temp;

    temp = 0;               /* Stupid compiler */
    temp = (unsigned long long)delay * 10;
    temp = igrp2_divide(temp, 256);
    if (temp > METRIC_INACCESSIBLE)
	return(METRIC_INACCESSIBLE);
    else
	return(temp);
}

/*
 * igrp2_metric_decode
 * Pull igrp metric out of packet and salami it into the infotype structure.
 */

void igrp2_metric_decode (igrp2metric *im, igrp2_mdecode *metric)
{
    ulong mtu;

    bcopy(&metric->bandwidth, &im->bandwidth, 4);
    bcopy(&metric->delay, &im->delay, 4);
    bcopy(&metric->mtu, &mtu, 3);
    im->mtu = mtu >> 8;
    im->hopcount = metric->hopcount;
    im->reliability = metric->reliability;
    im->load = metric->load;
}

/*
 * igrp2_metric_encode
 *
 * Fill in the contents of a packet metric block from a DNDB.
 *
 * If "unreachable" is TRUE, the route is marked as being unreachable.
 */
void igrp2_metric_encode (ddbtype *ddb, dual_ndbtype *dndb,  
			  igrp2_idbtype *iidb, igrp2_mdecode *metric,  
			  boolean unreachable,  boolean connected)
{
    ulong mtu, delay, offset;
    igrp2metric *vecmetric;
    idbtype *interface;

    interface = iidb->idb;
    vecmetric = &dndb->dndb_vecmetric;
    
    /* Mark the route as unreachable if asked. */

    if (unreachable) {
	delay = METRIC_INACCESSIBLE;
    } else {
	delay = vecmetric->delay;
    }

    if (ddb->mmetric_fudge) {
 	offset = (*ddb->mmetric_fudge)(ddb, &dndb->dndb_dest, interface, OUT);
 	if (offset > METRIC_INACCESSIBLE - delay) {
 	    delay = METRIC_INACCESSIBLE;
 	} else {
 	    delay +=offset;
 	}
    }
    memset(metric, 0, sizeof(igrp2_mdecode));
    PUTLONG(&metric->bandwidth, vecmetric->bandwidth);
    PUTLONG(&metric->delay, delay);
    mtu = vecmetric->mtu << 8;
    bcopy(&mtu, &metric->mtu, 3);
    metric->hopcount = connected ? 0 : vecmetric->hopcount;
    metric->reliability = vecmetric->reliability;
    metric->load = vecmetric->load;
}

/*
 * igrp2_compute_metric
 * Give a metric entry and the incoming interface, return the composite metric
 * If interface is NULL, compute metric from just the metric entry data.
 *
 *  metric = [K1*bandwidth + (K2*bandwidth)/(256 - load) + K3*delay] *
 *				[K5/(reliability + K4)]
 *
 *  If K5 == 0, then there is no reliability term.
 *
 *  The default version of IGRP has K1 == K3 == 1, K2 == K4 == K5 == 0
 */

ulong igrp2_compute_metric (ddbtype *ddb,
			    igrp2metric *m,
			    igrp2nettype *dest,
			    igrp2_idbtype *iidb,
			    ulong *neighbor)
{
    ulong delay, bandwidth, offset;
    ulong reliability, metric;
    idbtype *interface;

    interface = iidb->idb;

    /*
     * Combine factors from update and interface.
     * Return the update factors in neighbor.
     */
    delay = m->delay;
    if (ddb->mmetric_fudge) {
	offset =  (*ddb->mmetric_fudge)(ddb, dest, interface, IN);
	if (offset > METRIC_INACCESSIBLE - delay) {
	    delay = METRIC_INACCESSIBLE;
	} else {
	    delay +=offset;
	}
    }
    /*
     * Figure out how to handle the inaccessible metric case since this 
     * is now exactly 32 bits in size. $$$
     */
    if (delay == METRIC_INACCESSIBLE || m->hopcount >= ddb->max_hopcount) {
	*neighbor = METRIC_INACCESSIBLE;
	m->delay = METRIC_INACCESSIBLE;
	return(METRIC_INACCESSIBLE);
    }
    bandwidth = m->bandwidth;

    /*
     * Compute the metric that our neighbor sees.
     */
    *neighbor = igrp2_computemetric(bandwidth, (ulong)m->load, delay,
				    m->reliability, ddb->k1, ddb->k2, ddb->k3,
				    ddb->k4, ddb->k5);

    if (interface != NULL) {
	delay = delay + (interface->delay << 8);
 	bandwidth = max(bandwidth, 
 			SCALED_BANDWIDTH(interface->visible_bandwidth) << 8);
	reliability = min((ulong)m->reliability, 
			  interface->hwptr->reliability);
    } else
	reliability = (ulong)m->reliability;

    /*
     * Compute the scalar metric for the current Type of Service.
     * We are currently assuming a default TOS of zero.
     */
    metric = igrp2_computemetric(bandwidth, (ulong)m->load, delay,
				 reliability, ddb->k1, ddb->k2, ddb->k3,
				 ddb->k4, ddb->k5);
    m->delay = delay;
    m->bandwidth = bandwidth;
    m->reliability = reliability;
    if (m->mtu > 0)
	m->mtu = min(m->mtu, interface->ip_mtu);
    else
	m->mtu = interface->ip_mtu;
    m->load = max(m->load, interface->hwptr->load);
    m->hopcount++;
    
    return(metric);
}

/*
 * igrp2_get_sw_version
 *
 * Store the software version in the specified location.
 *
 * This consists of two bytes of IOS version, and two bytes of EIGRP
 * revision number.  This gives a hook for changing the system behavior.
 */
static void igrp2_get_sw_version (igrp2_sw_version *verptr)
{
    verptr->ios_majorver = SYS_MAJORVERSION;
    verptr->ios_minorver = SYS_MINORVERSION;
    verptr->eigrp_majorrev = EIGRP_REVISION_MAJOR_LEVEL;
    verptr->eigrp_minorrev = EIGRP_REVISION_MINOR_LEVEL;
}

/*
 * igrp2_hello
 *
 * Build a hello packet, given a buffer to put it in.
 *
 * Returns a pointer to the end of the packet (for adding TLVs).
 */
static uchar *igrp2_hello (igrp2type *igrp2,
			   uchar k1,
			   uchar k2,
			   uchar k3,
			   uchar k4,
			   uchar k5,
			   ushort hold)
{
    igrp2_paramtype *param;
    igrp2_versiontype *vers_tlv;
    
    param = igrp2_packet_data(igrp2);
    igrp2->opcode = IGRP_OPC_HELLO;
    PUTSHORT(&param->type, IGRP2_PARAMETER);
    PUTSHORT(&param->length, sizeof(igrp2_paramtype));
    param->k1 = k1;
    param->k2 = k2;
    param->k3 = k3;
    param->k4 = k4;
    param->k5 = k5;
    param->pad = 0;
    PUTSHORT(&param->holdtime, hold);
    /*
     * Write out a software version TLV so that systems can tell what version
     * of software their neighbors are running.  This to dig us out of nasty
     * backwards compatibility problems...
     */
    vers_tlv = (igrp2_versiontype *)(((uchar *)param) +
				     sizeof(igrp2_paramtype));
    PUTSHORT(&vers_tlv->type, IGRP2_SW_VERSION);
    PUTSHORT(&vers_tlv->length, IGRP2_VERSION_TLV_LENGTH);
    igrp2_get_sw_version(&vers_tlv->version);
    return(((uchar *)vers_tlv) + IGRP2_VERSION_TLV_LENGTH);
}

/*
 * igrp2_otype
 * Decode an igrp opcode type.
 *
 * Note that the "ACK" opcode doesn't really exist at this time;  the caller
 * has to play the appropriate games with Hellos to find these.
 */

static char *const opcode_names[IGRP_NUM_OPCODES] = 
                                               {"UNKNOWN", "UPDATE", "REQUEST",
						"QUERY", "REPLY", "HELLO",
						"IPXSAP", "PROBE", "ACK"};
char *igrp2_otype (int opcode)
{
    if (opcode <= IGRP_NUM_OPCODES) {
	return(opcode_names[opcode]);
    } else {
	return("UNKNOWN");
    }
}

/*
 * igrp2_dual2opc
 * Translate dual internal packet type to igrp2 opcode type.
 */
int igrp2_dual2opc (int dual_ptype)
{
    switch (dual_ptype) {
      case DUAL_MULTUPDATE: return(IGRP_OPC_UPDATE);  break;
      case DUAL_MULTQUERY:  return(IGRP_OPC_QUERY);   break;
      case DUAL_SENDREPLY:  return(IGRP_OPC_REPLY);   break;
      default:              return(IGRP_OPC_ILLEGAL); break;
    }
}

/*
 * igrp2_bugpak
 * Display EIGRP packet debugging information.
 *
 * At least one of pakdesc and igrp2 must be non-NULL.
 */

void igrp2_bugpak (dual_peertype *peer, igrp2type *igrp2,
		   igrp2_pakdesc *pakdesc, char *msg, ddbtype *ddb,
		   idbtype *idb, igrp2_idbtype *iidb, igrp2addrtype *source,
		   ushort retry)
{
    ulong ack_seq;
    ulong send_seq;
    uchar opcode;

    /* First get the ACK seq number.  This is a bit funky for ACK packets. */

    if (pakdesc && pakdesc->ack_packet) { /* Ack packet? */
	ack_seq = 0;
	if (igrp2)
	    ack_seq = GETLONG(&igrp2->ack); /* Use the packet seq if there. */
	if (!ack_seq)
	    ack_seq = pakdesc->ack_seq_num;
    } else {
	if (igrp2)
	    ack_seq = GETLONG(&igrp2->ack);
	else
	    ack_seq = 0;
    }

    /* Get the rest of the parameters. */

    if (igrp2 && !pakdesc) {
	opcode = igrp2->opcode;
	send_seq = GETLONG(&igrp2->sequence);
    } else {
	opcode = pakdesc->opcode;
	send_seq = (pakdesc->ack_packet) ? 0 : pakdesc->ack_seq_num;
    }
    if (!igrp2 && pakdesc)
	igrp2 = pakdesc->pregen_pak;

    /* Fudge the opcode for acknowledgements. */

    if (opcode == IGRP_OPC_HELLO && ack_seq)
	opcode = IGRP_OPC_ACK;

    /* Bail if this packet type isn't being debugged. */

    if (!(igrp2_packet_debug_flags & (1 << opcode)))
	return;

    /* Bail if we're only debugging retries and this isn't one. */

    if ((!retry || !send_seq) &&	/* Only sequenced pkts are retries */
	(igrp2_packet_debug_flags & IGRP2_DEBUG_RETRY)) {
	return;
    }

    if (peer)
	source = &peer->source;
    if (!igrp2_pdebug(ddb, source))
	return;
    buginf("\nEIGRP: %s %s on %s", msg,
	   igrp2_otype(opcode), idb->namestring);
    if (source)
	buginf(" nbr %s", (*ddb->printaddr)(source));
    if (retry) {
	buginf(", retry %d", retry);
	if (peer) {
	    buginf(", RTO %d", peer->rto);
	}
    }
    if (igrp2) {
	buginf("\n  AS %d, Flags 0x%x, Seq %u/%u idbQ %d/%d",
	       GETLONG(&igrp2->asystem), GETLONG(&igrp2->flags),
	       send_seq, ack_seq, idb->hwptr->input_qcount,
	       idb->hwptr->outputq[DEFAULT_QUEUE(idb->hwptr)].count);
    } else if (pakdesc->ack_packet) {
	buginf("\n  Ack seq %u", ack_seq);
    }
    if (iidb)
	buginf(" iidbQ un/rely %d/%d", iidb->xmitQ[UNRELIABLE_QUEUE].count,
	       iidb->xmitQ[RELIABLE_QUEUE].count);
    if (peer)
	buginf(" peerQ un/rely %d/%d", peer->xmitQ[UNRELIABLE_QUEUE].count,
	       peer->xmitQ[RELIABLE_QUEUE].count);
    if (pakdesc && pakdesc->start_serno)
	buginf(" serno %d-%d", pakdesc->start_serno, pakdesc->end_serno);
}

/*
 * igrp2_pak_sanity
 * Sanity check an incoming EIGRP packet.
 * Return pointer to EIGRP header if packet passed checks, otherwise NULL.
 */

static igrp2type *igrp2_pak_sanity (ddbtype *ddb, paktype *pak,
				    igrp2addrtype *source, idbtype *source_idb,
				    ulong *paklen)
{
    igrp2type *igrp2;

    /*
     * Kill brain dead interfaces which are down but still receive.
     */
    if (!(*ddb->interface_up)(source_idb))
	return(NULL);
    /*
     * Get pointer to beginning of IGRP2 header.
     */	
    igrp2 = (*ddb->headerptr)(pak);
    /*
     * Check version number.  
     */
    if (igrp2->version != IGRP2_VERSION) {
	if (igrp2_detail_debug(ddb, source))
	    buginf("\nEIGRP: bad IGRP2 version from %s",
		   (*ddb->printaddr)(source));
	return(NULL);
    }
    /*
     * Verify checksum in datagram.
     */
    *paklen = (*ddb->paklen)(pak);
    if (ipcrc((ushort *)igrp2, *paklen)) {
	if (igrp2_detail_debug(ddb, source))
	    buginf("\nEIGRP: bad checksum from %s", 
		   (*ddb->printaddr)(source));
	return(NULL);
    }
    /*
     * Validate autonomous system number
     */
    if (GETLONG(&igrp2->asystem) != ddb->asystem) {
	if (igrp2_detail_debug(ddb, source))
	    buginf("\nEIGRP: system %d from %s, should be system %d",
		   GETLONG(&igrp2->asystem),
		   (*ddb->printaddr)(source), ddb->asystem);
	return(NULL);
    }
    return(igrp2);
}

/*
 * igrp2_compare_seq
 *
 * Compare two IGRP2 unsigned 32-bit sequence numbers.
 *
 * if seq1 is later than seq2, returns  1.
 * if seq1 is earlier than seq2, returns -1.
 * if seq1 equals seq2, returns 0.
 *
 * This scheme treats sequence numbers as a circular space;  the temporal
 * ordering of the two sequence numbers is based on which interpretation
 * puts them closer together on the circle.  This game is done by taking
 * advantage of the fact that subtraction is effectively circular.
 *
 * Lessons learned from the timer system...
 */

static int igrp2_compare_seq (ulong seq1, ulong seq2)
{
    long seq_diff;

    seq_diff = seq1 - seq2;
    if (seq_diff == 0)
	return(0);

    if (seq_diff > 0)
	return(1);

    return(-1);
}

/*
 * igrp2_send_ack
 *
 * Enqueue an acknowledgement for the given peer and sequence number.
 *
 * We will not enqueue the ACK if we are waiting for our INIT packet to
 * be acknowledged;  rather, we will carry the ACK in an upcoming data
 * packet (such as the retransmission of the INIT).
 */
static void igrp2_send_ack (ddbtype *ddb, dual_peertype *peer, ulong seq)
{
    igrp2_pakdesc *pakdesc;
    igrp2_idbtype *iidb;

    if (peer->need_init_ack) {		/* We don't send ACKs yet. */
	return;
    }

    iidb = peer->iidb;
    pakdesc = igrp2_enqueue(ddb, peer, iidb, FALSE);
    if (pakdesc) {
	pakdesc->opcode = IGRP_OPC_HELLO;
	pakdesc->ack_seq_num = seq;
	pakdesc->ack_packet = TRUE;
	igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
    }
}

/*
 * igrp2_cancel_cr_mode
 *
 * Cancel any pending Conditional Receive state.
 */
static inline void igrp2_cancel_cr_mode (dual_peertype *peer)
{
    peer->cr_mode = FALSE;
    peer->cr_sequence = 0;
}

/*
 * igrp2_should_accept_seq_packet
 *
 * Returns an indication as to whether the packet should be accepted (and
 * acknowledged), rejected with acknowledgement, or rejected without comment.
 */
typedef enum {ACCEPT_PACKET, REJECT_PACKET_WITH_ACK, REJECT_PACKET}
    packet_accept_type;

static packet_accept_type igrp2_should_accept_seq_packet (ddbtype *ddb,
							  dual_peertype *peer,
							  igrp2type *igrp2,
							  ulong sequence)
{
    /* Is the packet in sequence? */

    if (igrp2_compare_seq(sequence, peer->last_seq_number) > 0) {

	/* Yep.  Is the CR bit set? */

	if (GETLONG(&igrp2->flags) & IGRP2_CR_FLAG) {

	    /* Yep.  Are we in CR mode? */

	    if (peer->cr_mode) {

		/* Yep.  Did the Sequence TLV come with a sequence number? */

		if (peer->cr_sequence) {

		    /* Yep.  Is it correct? */

		    if (peer->cr_sequence == sequence) {

			/* Yep.  Accept the packet. */

			return(ACCEPT_PACKET);

		    } else {		/* Sequence mismatch */

			/* Bad sequence.  Don't accept. */

			if (igrp2_detail_debug(ddb, &peer->source))
			    buginf(", seq %d doesn't match SEQ TLV %d",
				   sequence, peer->cr_sequence);
			return(REJECT_PACKET);
		    }

		} else {		/* No sequence received */

		    /* Unsequenced CR packet.  Accept it. */

		    return(ACCEPT_PACKET);
		}

	    } else {			/* Not in CR mode */

		/* CR packet not in CR mode.  Don't accept. */

		if (igrp2_detail_debug(ddb, &peer->source))
		    buginf(", not in CR-mode, packet discarded");
		return(REJECT_PACKET);
	    }
	} else {			/* CR flag clear */

	    /* No CR flag on the packet.  Accept it. */

	    return(ACCEPT_PACKET);
	}

    } else {				/* Out of sequence */

	/* Out of sequence.  Don't accept it, but acknowledge it. */

	peer->iidb->out_of_sequence_rcvd++;
	if (igrp2_detail_debug(ddb, &peer->source)) {
	    buginf(", last received seq %d, out of sequence",
		   peer->last_seq_number);
	} else if (igrp2_packet_debug_flags & IGRP2_DEBUG_RETRY) {
	    /*
	     * Display if it is *seriously* out of sequence (as opposed to
	     * a duplicate.
	     */
	    if (sequence != peer->last_seq_number) {
		igrp2_bugpak(peer, igrp2, NULL, "Received", ddb,
			     peer->iidb->idb, peer->iidb, &peer->source,
			     1);	/* Fake a retry.  What a hack. */
		buginf(", ***last received seq %d, out of sequence***",
		       peer->last_seq_number);
	    }
	}
	return(REJECT_PACKET_WITH_ACK);
    }
}

/*
 * igrp2_accept_sequenced_packet
 *
 * Process sequence number from received IGRP2 packet. Return TRUE, if
 * packet is to be accepted, otherwise return FALSE.
 *
 * Tracks the expected setting of the CR bit as well, and updates the
 * peer last_seq_number field if the packet is accepted.
 */
static boolean igrp2_accept_sequenced_packet (
    ddbtype *ddb,
    dual_peertype *peer,
    igrp2type *igrp2)
{
    ulong sequence;
    boolean result;
    ulong opcode;

    sequence = GETLONG(&igrp2->sequence);
    opcode = igrp2->opcode;
    if (opcode == IGRP_OPC_HELLO && GETLONG(&igrp2->ack))
	opcode = IGRP_OPC_ACK;

    /* Do what our handler tells us to do. */

    switch (igrp2_should_accept_seq_packet(ddb, peer, igrp2, sequence)) {

      case ACCEPT_PACKET:
	peer->last_seq_number = sequence;
	result = TRUE;
	break;

      case REJECT_PACKET:
	result = FALSE;
	break;

      case REJECT_PACKET_WITH_ACK:
	igrp2_send_ack(ddb, peer, sequence);
	if (sequence != peer->last_seq_number) { /* Not a duplicate! */
	    dual_log_xport(ddb, DUALEV_BADSEQ, &peer->source, &peer->source,
			   &opcode);
	}
	result = FALSE;
	break;

      default:				/* Shouldn't happen. */
	result = FALSE;
	break;
    }

    /* Cancel CR mode if it was set. */

    if (peer->cr_mode)
	igrp2_cancel_cr_mode(peer);

    return(result);
}

/*
 * igrp2_update_srtt
 *
 * Update the peer SRTT.  Maintains the IIDB total SRTT field (which allows
 * us to calculate an average SRTT for multicast flow control purposes).
 */
static void igrp2_update_srtt (dual_peertype *peer, ulong new_value)
{
    igrp2_idbtype *iidb;

    iidb = peer->iidb;

    /* Remove the old value from total and replace it with the new value. */

    new_value = max(new_value, 1);	/* It takes some time... */
    if (iidb) {
	iidb->total_srtt -= peer->srtt;
	iidb->total_srtt += new_value;
    }
    peer->srtt = new_value;
}

/*
 * igrp2_calculate_rto
 *
 * Uses retransmission timeout estimation procedure from RFC 793 (TCP).
 * All times are in milliseconds.
 *
 * rtt = is calculated round trip time.
 * srtt = is a smoothed round trip time which is bounded.
 * rto = retransmission timeout.
 */

static void igrp2_calculate_rto (dual_peertype *peer, ulong rtt)
{
    if (!peer->srtt) {
	igrp2_update_srtt(peer, rtt);
    } else {

	/*
	 * srtt = (srtt * .8) + (rtt * .2)
	 */
	igrp2_update_srtt(peer, (peer->srtt * 8)/10 + (rtt * 2)/10);
    }

    /* The baseline RTO is IGRP2_RTO_MULTIPLIER * RTT. */

    peer->rto = peer->srtt * IGRP2_RTO_SRTT_MULTIPLIER;

    /* Apply an absolute lower bound. */

    peer->rto = max(peer->rto, IGRP2_RTO_LOWER);

    /* Apply a lower bound based on the interface pacing timer. */

    peer->rto = max(peer->rto,
		    peer->iidb->iidb_send_interval[RELIABLE_QUEUE] * 
		    IGRP2_RTO_PACING_MULTIPLIER);

    /* Apply an upper bound. */

    peer->rto = min(peer->rto, IGRP2_RTO_UPPER);
}

/*
 * igrp2_post_ack
 *
 * Perform cleanup after a packet has been acknowledged (or a unicast
 * transmission has been suppressed).
 */
static void igrp2_post_ack (ddbtype *ddb, dual_peertype *peer,
			    igrp2_pakdesc *pakdesc)
{
    IGRP2_XBUGINF(XDBPACKETIZE|XDBDETAIL,
		  "\nPacket acked from %s (%s), serno %d-%d",
		  (*ddb->printaddr)(&peer->source),
		  peer->iidb->idb->namestring,
		  pakdesc->start_serno, pakdesc->end_serno);

    /* Dequeue the packet and free the appropriate stuff. */

    igrp2_dequeue_qelm(ddb, peer, RELIABLE_QUEUE);
    peer->retry_count = 0;

    /* Kick the peer timer based on the next item on the queue (if any). */

    igrp2_start_peer_timer(ddb, peer->iidb, peer);
}

/*
 * igrp2_process_ack
 *
 * Process a received acknowledgement.  Potentially dequeue a packet
 * on the retransmission queue.  Alert the higher levels.
 */
static void igrp2_process_ack (
    ddbtype *ddb,
    dual_peertype *peer,
    ulong ack_seq)
{

    igrp2_pakdesc_qelm *qelm;
    igrp2_pakdesc *pakdesc;

    /* Get the first sequenced packet on the queue. */

    qelm = peer->xmitQ[RELIABLE_QUEUE].qhead;
    if (!qelm)				/* Nothing there. */
	return;
    pakdesc = qelm->pakdesc;

    /* If the acknowledgement doesn't match the packet, ignore it. */

    if (ack_seq != pakdesc->ack_seq_num)
	return;

    /* Calculate the RTT if we didn't retransmit. */

    if (!peer->retry_count) {
	if (TIMER_RUNNING(qelm->time_sent)) { /* Should be, but... */
	    igrp2_calculate_rto(peer, ELAPSED_TIME(qelm->time_sent));
	}
    }

    /* Clean up. */

    igrp2_post_ack(ddb, peer, pakdesc);
}

/*
 * igrp2_neighbor_down
 *
 * Take a neighbor down.  This starts the twisty passage to down-ness.
 *
 * We guarantee that we are on the router process thread, so we can do this
 * operation atomically.
 *
 * This calls the neighbor_down entry in the protocol-specific code, which
 * does some cleanup and then calls the DUAL code to clean up, and finally
 * calls the destroy_peer function.
 *
 * Note that the peer pointer will be invalid after calling this routine!
 */
static void igrp2_neighbor_down (ddbtype *ddb,
				 dual_peertype *peer)
{
    igrp2_idbtype *iidb;

    /* Inform the DUAL finite state machine. */

    peer->going_down = TRUE;		/* We're half-down */
    iidb = peer->iidb;
    dual_peerdown(ddb, peer);

    /* Fake an SIA so that everybody saves their log, if so desired. */

    if (ddb->kill_everyone)
	dual_sia_copy_log(ddb);
}

/*
 * igrp2_reinit_peer
 * Our neighbor claims it is not in init state, yet we are.  If it
 * repeatedly claims this, then probably we have dropped and reacquired
 * this adjacency subsequent to ACKing an update with init from our neighbor.
 *
 * It is a bug in EIGRP's peer processing state machine that
 * this can happen.  Once we've detected this bad state,
 * get out of it by dropping the neighbor and starting over.
 *
 * The peer pointer will be invalid after return from this routine.
 */

static void igrp2_reinit_peer (ddbtype *ddb, dual_peertype *peer)
{
    igrp2_idbtype *iidb;

    iidb = peer->iidb;
    if (igrp2_detail_debug(ddb, &peer->source) || igrp2_nbrs_debug)
	buginf("\nEIGRP: Neighbor %s in INIT state, out of sequence packet,"
	       " discarded", (*ddb->printaddr)(&peer->source));
    if (!TIMER_RUNNING(peer->reinit_start)) {
	GET_TIMESTAMP(peer->reinit_start);
    }
    if (CLOCK_IN_INTERVAL(peer->reinit_start, peer->last_holding_time))
	return;

    if (igrp2_detail_debug(ddb, &peer->source) || igrp2_nbrs_debug)
	buginf("\nEIGRP: Stuck in INIT state--dropping neighbor");
    dual_log_xport_all(ddb, DUALEV_INITSTUCK, &peer->source, &iidb->idb);

    igrp2_log_peer_change(ddb, peer, FALSE, "stuck in INIT state");
    igrp2_neighbor_down(ddb, peer);	/* Peer is now invalid! */

    /*
     * Make sure hello timer is going to go off soon, so that we expedite
     * getting in sync with this neighbor.
     */
    if (mgd_timer_left_sleeping(&iidb->iidb_hello_timer) >
				IGRP2_INIT_HELLO_DELAY) {
	mgd_timer_start(&iidb->iidb_hello_timer, IGRP2_INIT_HELLO_DELAY);
    }
}

/*
 * igrp2_find_sequence_tlv
 *
 * Find Sequence TLV. If found see if this system's address is in the
 * list. If so, we do not go into CR-mode, and the next packet received
 * with the CR-bit set, will not be accepted. If in CR-mode, it will be
 * accepted.
 *
 * A Sequnce TLV has following format:
 *
 * type (2 bytes)     IGRP2_SEQUENCE
 * length (2 bytes)   TLV length + 4
 * address-1:
 *   length (1 byte)   Length of address
 *   address (>1 byte) Value of address
 * address-n:
 *   length (1 byte)   Length of address
 *   address (>1 byte) Value of address
 */
static void igrp2_find_sequence_tlv (ddbtype *ddb,
				     dual_peertype *peer,
				     igrp2_gentlvtype *tlv_hdr,
				     int pak_len)
{
    uchar *addr_ptr;
    int   length;
    int   len;

    if (igrp2_detail_debug(ddb, &peer->source))
	buginf("\nEIGRP: Received Sequence TLV from %s",
	       (*ddb->printaddr)(&peer->source));

    /*
     * Point to data.
     */
    length = min(pak_len, GETSHORT(&tlv_hdr->length)) -
	sizeof(igrp2_gentlvtype);
    addr_ptr = (uchar *) (tlv_hdr + 1);

    /*
     * Go through each address in the Sequence TLV. If one of our
     * addresses is found, clear CR-mode, otherwise set it.
     * (addr_ptr+1) skips past the length byte.
     */
    while (length > 0) {
	if (igrp2_detail_debug(ddb, &peer->source))
	    buginf("\n       %s",
		   (*ddb->printaddr)((igrp2addrtype *)(addr_ptr+1)));
	if ((*ddb->localaddr)(ddb, (igrp2addrtype *)(addr_ptr+1),
			      peer->iidb->idb)) {
	    igrp2_cancel_cr_mode(peer);
	    if (igrp2_detail_debug(ddb, &peer->source))
		buginf("\n       clearing CR-mode");
	    return;
	}

	/*
	 * First byte of address contains length of address.	
	 */
	len = *addr_ptr + 1;
	length -= len;
	addr_ptr += len;
    }

    /*
     * Our address not found, go into CR-mode for respective neighbor.
     */
    peer->cr_mode = TRUE;
    if (igrp2_detail_debug(ddb, &peer->source))
	buginf("\n       setting CR-mode");
}

/*
 * igrp2_receive_hello
 *
 * Process received hello packets.
 *
 * For a Parameter TLV included in a packet, a verification of the K-values
 * are performed and the holdtime is stored in the peer cache.
 *
 * If this is a new peer, we will call DUAL to fashion the update stream
 * for the new peer.  In this case, we may suspend!
 */

static void igrp2_receive_hello (ddbtype *ddb,
				 dual_peertype *peer,
				 igrp2type *igrp2,
				 int paklen)
{

    igrp2_gentlvtype *tlv_hdr;
    igrp2_paramtype *param;
    igrp2_versiontype *version_tlv;
    igrp2_next_multi_seq_type *multiseq_tlv;
    boolean new_peer;
    ushort length;
    boolean seq_tlv_found;
    boolean cr_seq_found;
    boolean error_found;
    

    paklen -= IGRP2HEADERBYTES;
    if (paklen <= 0)
	return;

    /* We haven't seen any CR or SEQ TLV stuff so far. */

    seq_tlv_found = FALSE;
    cr_seq_found = FALSE;
    error_found = FALSE;

    /*
     * Point to data.
     */
    tlv_hdr = (igrp2_gentlvtype *) (igrp2 + 1);

    /*
     * Go through each TLV in hello packet.
     */
    new_peer = FALSE;
    while (paklen > 0) {
	switch (GETSHORT(&tlv_hdr->type)) {
	case IGRP2_SEQUENCE:
	    /*
	     * Process Sequence TLV.
	     */
	    igrp2_find_sequence_tlv(ddb, peer, tlv_hdr, paklen);
	    seq_tlv_found = TRUE;
	    break;

	case IGRP2_PARAMETER:
	    /*
	     * Process Parameter TLV.	
	     */
	    param = (igrp2_paramtype *) tlv_hdr;
	    if (!igrp2_kvalues_match(ddb, param)) {
		if (igrp2_packet_debug_flags & IGRP2_DEBUG_HELLO) {
		    if (igrp2_pdebug(ddb, &peer->source))
			buginf("\n       K-value mismatch");
		}
		igrp2_log_peer_change(ddb, peer, FALSE, "K-value mismatch");
		igrp2_neighbor_down(ddb, peer);
		return;
	    } else {
		new_peer = (!mgd_timer_running(&peer->holding_timer));
		peer->last_holding_time = GETSHORT(&param->holdtime) * ONESEC;
		mgd_timer_start(&peer->holding_timer, peer->last_holding_time);
	    }
	    break;

	case IGRP2_SW_VERSION:
	    /*
	     * Process software version number TLV.
	     */
	    version_tlv = (igrp2_versiontype *) tlv_hdr;
	    memcpy(&peer->peer_version, &version_tlv->version,
		   IGRP2_SW_VERSION_SIZE);
	    break;

	case IGRP2_NEXT_MCAST_SEQ:
	    /*
	     * Process next multicast sequence number TLV.
	     */
	    multiseq_tlv = (igrp2_next_multi_seq_type *) tlv_hdr;
	    peer->cr_sequence = GETLONG(&multiseq_tlv->seq_num);
	    cr_seq_found = TRUE;
	    if (igrp2_detail_debug(ddb, &peer->source)) {
		buginf("\nEIGRP: Received CR sequence TLV from %s,"
		       " sequence %d", (*ddb->printaddr)(&peer->source),
		       peer->cr_sequence);
	    }
	    break;

	default:
	    /*
	     * Skip the unknown TLV
	     */
	    break;
	}

	length = GETSHORT(&tlv_hdr->length);
	if (length < sizeof(igrp2_gentlvtype)) {
	    if (igrp2_detail_debug(ddb, &peer->source))
		buginf(", malformed TLV, length %d", length);
	    error_found = TRUE;
	    break;
	}
	paklen -= length;
	tlv_hdr = (igrp2_gentlvtype *) (((uchar *) tlv_hdr) + length);
    }

    /* Bail if the last TLV was short. */

    if (paklen < 0) {
	if (igrp2_detail_debug(ddb, &peer->source))
	    buginf(", malformed TLV, excess length %d", -paklen);
	error_found = TRUE;
    }

    /* If we got an error, clean up and bail out. */

    if (error_found) {
	igrp2_cancel_cr_mode(peer);
	return;
    }

    /*
     * If we got a CR sequence number without a Sequence TLV, cancel it.
     * If we got a Sequence TLV without a CR sequence number, we're talking
     * to an old-style peer;  clear any old CR sequence number for
     * completeness.
     */
    if (cr_seq_found && !seq_tlv_found) {
	igrp2_cancel_cr_mode(peer);
    } else if (seq_tlv_found && !cr_seq_found) {
	peer->cr_sequence = 0;
    }

    /* If this is a new peer, tell DUAL to send an update stream. */

    if (new_peer) {
	if (igrp2_nbrs_debug)
	    buginf("\nEIGRP: New peer %s", (*ddb->printaddr)(&peer->source));
	igrp2_log_peer_change(ddb, peer, TRUE, "new adjacency");
	dual_new_peer(ddb, peer);
    }
    return;
}

/*
 * igrp2_receive
 *
 * Process received IGRP2 packet. This function processes the fixed
 * part of the IGRP2 header. This function is network protocol 
 * independent.
 *
 * Returns a pointer to the peer structure, or NULL if the packet should
 * be ignored.
 */
dual_peertype *igrp2_receive (ddbtype *ddb, paktype *pak,
			      igrp2addrtype *source, idbtype *source_idb)
{
    dual_peertype *peer;
    igrp2_idbtype *iidb;
    igrp2type     *igrp2;
    ulong slot, paklen;
    ulong seq, ack_seq;
    ulong opcode;
    ulong flags;

    igrp2 = igrp2_pak_sanity(ddb, pak, source, source_idb, &paklen);
    if (igrp2 == NULL)
	return(NULL);

    /* Don't accept hello if this ddb is not configured on interface. */

    iidb = igrp2_find_iidb(ddb, source_idb);
    if (!iidb) {
	if (igrp2_detail_debug(ddb, source) || igrp2_nbrs_debug) {
	    buginf("\nEIGRP: ddb not configured on %s",
		   source_idb->namestring);
	}
	return(NULL);
    }

    /* Ignore the packet if we're passive. */

    if (iidb->passive_interface)
	return(NULL);

    seq = GETLONG(&igrp2->sequence);
    ack_seq = GETLONG(&igrp2->ack);
    flags = GETLONG(&igrp2->flags);
    opcode = igrp2->opcode;
    if (opcode == IGRP_OPC_HELLO && ack_seq)
	opcode = IGRP_OPC_ACK;

    peer = igrp2_find_peer(ddb, source, source_idb); /* May return NULL */
    /*
     * Print debug, if enabled.
     */
    igrp2_debug_rcvd_pkt(ddb, peer, igrp2, source_idb, source);
    igrp2_update_mib(ddb, igrp2, FALSE);

    if (peer) {

	/* Induce a receive error if desired. */
	
	if (igrp2_induce_error(ddb, peer->iidb, "Receive")) {
	    return(NULL);
	}

	/* If the peer is going down, ditch the packet. */
	
	if (peer->going_down)
	    return(NULL);

	if (opcode != IGRP_OPC_HELLO) {
	    dual_log_xport(ddb, DUALEV_PKTRCV, source, &source_idb,
			   source);
	    dual_log_xport(ddb, DUALEV_PKTRCV2, source, &opcode,
			   &flags);
	    dual_log_xport(ddb, DUALEV_PKTRCV3, source, &seq, &ack_seq);
	}
	
	/*
	 * If a packet was received with the INIT flag set, the peer
	 * wants a full topology table downloaded. The peer has gone down
	 * and come back up within the holdtime. We think the peer never
	 * went down. We need to reset the received sequence number for this 
	 * peer.
	 */
	if (flags & IGRP2_INIT_FLAG) {
	    if (!peer->need_init) {
		/*
		 * Perhaps this is simply a retransmission of the initial
		 * update packet?  If so, don't tear down the neighbor.
		 */
		if (seq != peer->last_seq_number) {
		    dual_log_xport_all(ddb, DUALEV_NOINIT, &source,
				       &source_idb);
		    igrp2_log_peer_change(ddb, peer, FALSE, "peer restarted");
		    igrp2_neighbor_down(ddb, peer);
		    return(NULL);
		}
	    } else {
		peer->need_init = FALSE;
	    }
	} else {

	    /*
	     * Packet has INIT bit clear.  If we're in INIT state and the
	     * packet is sequenced, and it is one that we would otherwise
	     * be expected to accept, go kick things to try to get the peer
	     * in sync with us.  If we wouldn't otherwise accept the packet,
	     * simply drop it.
	     */
	    if (peer->need_init && seq) {
		if (igrp2_should_accept_seq_packet(ddb, peer, igrp2,
						   seq) == ACCEPT_PACKET) {
		    igrp2_reinit_peer(ddb, peer);
		}
		return(NULL);
	    }
	}

	/*
	 * If the packet is carrying an acknowledgement, process it.
	 */
	if (ack_seq) {
	    igrp2_process_ack(ddb, peer, ack_seq);
	}

	/*
	 * If the packet is sequenced, see if it's acceptable.  If not,
	 * drop it.
	 */
	if (seq) {
	    if (!igrp2_accept_sequenced_packet(ddb, peer, igrp2)) {
		return(NULL);
	    }
	    TIMER_STOP(peer->reinit_start);
	}

    } else {				/* No peer */

	/* If the packet is from ourselves, drop it. */

	if ((*ddb->localaddr)(ddb, source, source_idb)) {
	    if (igrp2_detail_debug(ddb, source) || igrp2_nbrs_debug) {
		buginf("\nEIGRP: Packet from ourselves ignored");
	    }
	    return(NULL);
	}

	/* No peer.  If the packet isn't a hello, drop it. */

	if (opcode != IGRP_OPC_HELLO) {
	    if (igrp2_detail_debug(ddb, source) || igrp2_nbrs_debug) {
		buginf("\nEIGRP: Neighbor not yet found");
	    }
	    return(NULL);
	}

	/* New peer.  If we shouldn't accept it, ignore the hello. */

	if (ddb->accept_peer) {
	    if (!(*ddb->accept_peer)(ddb, source, iidb)) {
		return(NULL);
	    }
	}

	/* New peer.  Create an entry for it, with all the trimmings. */

	peer = malloc(sizeof(dual_peertype));
	if (!peer) {
	    return(NULL);
	}

	queue_init(&peer->xmitQ[UNRELIABLE_QUEUE], 0);
	queue_init(&peer->xmitQ[RELIABLE_QUEUE], 0);
	peer->iidb = iidb;
	igrp2_allochandle(ddb, peer);
	bcopy(source, &peer->source, sizeof(igrp2addrtype));
	peer->router_id = ddb->router_id;
	peer->srtt = 0;			/* total_srtt depends on this! */
	peer->rto = IGRP2_INIT_RTO;
	peer->need_init = TRUE;
	peer->need_init_ack = TRUE;
	peer->coming_up = TRUE;
	peer->protocol_data = NULL;
	GET_TIMESTAMP(peer->uptime);
	slot = (*ddb->peerbucket)(source);
	peer->next = ddb->peercache[slot];
	ddb->peercache[slot] = peer;
	peer->next_peer = ddb->peer_list;
	ddb->peer_list = peer;
	mgd_timer_init_leaf(&peer->peer_send_timer, &iidb->iidb_peer_timer,
			    IGRP2_PEER_SEND_TIMER, peer, FALSE);
	mgd_timer_init_leaf(&peer->holding_timer, &iidb->iidb_holding_timers,
			    IGRP2_PEER_HOLDING_TIMER, peer, FALSE);
	if (igrp2_peer_count(iidb) == 1) {	/* First one on this i/f */
	    dual_first_peer_added(ddb, iidb);
	}

	if (ddb->peerstatehook != NULL) {
	    (*ddb->peerstatehook)(ddb, peer, TRUE);
	}
    }

    /* Check for Sequence TLV. Only present in Hello packets. */

    if (igrp2->opcode == IGRP_OPC_HELLO) {
	igrp2_receive_hello(ddb, peer, igrp2, paklen);
    }

    /* Acknowledge sequenced packets.  The ACK may later be suppressed. */

    if (seq) {
	igrp2_send_ack(ddb, peer, peer->last_seq_number);
    }
    return(peer);
}

/*
 * igrp2_find_peer
 *
 * Find peer from ddb with matching address.
 */

dual_peertype *igrp2_find_peer (
    ddbtype *ddb,
    igrp2addrtype *address,
    idbtype *interface)
{

    dual_peertype *peer;
    int           slot;

    if (address == NULL)
	return(NULL);

    slot = (*ddb->peerbucket)(address);
    for (peer = ddb->peercache[slot]; peer; peer = peer->next) {
	if (peer->iidb->idb != interface) {
	    continue;
	}
	if ((*ddb->addrmatch)(&peer->source, address)) {
	    return(peer);
	}
    }
    return(NULL);
}

/*
 * igrp2_next_seq_number
 *
 * Increment sequence number in the IGRP2 32-bit sequence space.
 * If it wraps, set it to 1 (a value of 0 is illegal).
 */

static ulong igrp2_next_seq_number (ulong number)
{
    number++;

    /*
     * Reach upper limit of sequence space.
     */
    if (!number) number = 1;
    return(number);
}

/*
 * igrp2_set_seq_number
 *
 * Set the packet descriptor sequence number field to the next available
 * value.  Assumes that the packet is sequenced.
 *
 * Updates the DDB sequence number field.
 */
static void igrp2_set_seq_number (ddbtype *ddb, igrp2_pakdesc *pakdesc)
{
    ddb->send_seq_number = igrp2_next_seq_number(ddb->send_seq_number);
    pakdesc->ack_seq_num = ddb->send_seq_number;
}

/*
 * igrp2_enqueue
 *
 * Enqueue a packet descriptor to be sent.  The packet descriptor is
 * enqueued in the proper place, based on the settings of the peer,
 * iidb, and use_seq parameters.
 *
 * If use_seq is TRUE, the packet will be sent reliably.
 *
 * If peer is non-NULL, the packet will be unicasted;  if NULL, it will
 * be multicasted.
 *
 * The iidb parameter must always be non-NULL.
 */
igrp2_pakdesc *igrp2_enqueue (ddbtype *ddb, dual_peertype *peer,
			      igrp2_idbtype *iidb, boolean use_seq)
{
    igrp2_pakdesc *pakdesc;
    igrp2_pakdesc_qelm *qelm;
    queuetype *xmit_queue;
    xmit_queue_type qtype;
    boolean iidb_queue;

    /* Allocate a packet descriptor.  Fill it in. */

    pakdesc = chunk_malloc(igrp2_pakdesc_chunks);
    if (!pakdesc) {
	return(NULL);
    }
    pakdesc->pakdesc_multicast = (peer == NULL);

    /* If sequenced, set the bit and get a sequence number. */

    if (use_seq) {
	pakdesc->sequenced = TRUE;
	igrp2_set_seq_number(ddb, pakdesc);
    }

    /* Allocate a queue element for the packet.  Bind the descriptor to it. */

    qelm = chunk_malloc(igrp2_pakdesc_qelm_chunks);
    if (!qelm) {
	chunk_free(igrp2_pakdesc_chunks, pakdesc);
	return(NULL);
    }
    igrp2_bind_pakdesc(pakdesc, qelm);

    /*
     * Enqueue the packet.  Unreliable unicasts go directly to the peer
     * queue;  everything else goes via the IIDB queue for serialization.
     */
    qtype = use_seq ? RELIABLE_QUEUE : UNRELIABLE_QUEUE;
    iidb_queue = (use_seq || (!peer));
    pakdesc->peer = peer;
    if (iidb_queue) {
	xmit_queue = &iidb->xmitQ[qtype];
    } else {
	xmit_queue = &peer->xmitQ[qtype];
    }
    enqueue(xmit_queue, qelm);

    /*
     * Now kick the appropriate timer.
     */
    if (iidb_queue) {			/* On the IIDB */
	igrp2_kick_pacing_timer(ddb, iidb, 1); /* Jitter just a tad */
    } else {				/* On the peer queue */
	igrp2_start_peer_timer(ddb, iidb, peer);
    }
    return(pakdesc);
}

/*
 * igrp2_retransmit_packet
 *
 * Perform the housekeeping necessary when retransmitting a packet.
 *
 * Returns TRUE if we've punted the link due to excessive retransmissions,
 * or FALSE if we're still going.
 */
static boolean igrp2_retransmit_packet (ddbtype *ddb, dual_peertype *peer)
{
    /*
     * If we have been retransmitting this packet for at least the peer
     * holding time and the retry count has been exceeded, declare the
     * peer to be dead.
     */
    if (peer->retry_count >= IGRP2_RETRY_LIMIT && 
	CLOCK_OUTSIDE_INTERVAL(peer->pkt_first_send_time,
			       peer->last_holding_time)) {
	if (igrp2_detail_debug(ddb, &peer->source) || igrp2_nbrs_debug)
	    buginf("\nEIGRP: Retransmission retry limit exceeded");
	igrp2_take_peer_down(ddb, peer, FALSE, "retry limit exceeded");
	return(TRUE);
    }
    
    /* Note that we're retransmitting. */

    peer->retry_count++;
    
    /* Do exponential backoff. */

    peer->rto = (peer->rto * 3) / 2;
    peer->rto = min(peer->rto, IGRP2_RTO_UPPER);

    /* Bump the retransmission count. */

    peer->retransmit_count++;

    return(FALSE);
}

/*
 * igrp2_generate_packet
 *
 * Calls back to the protocol-specific module to generate a packet based
 * on the current descriptor, or uses the pregenerated packet.  Returns
 * a pointer to the packet, with the fixed header initialized.  Sets
 * *packet_suppressed TRUE if a packet should have been generated but all
 * data was overtaken by events (so the transport can fake an ACK).  Note
 * that we may return NULL even with packet_suppressed FALSE;  this indicates
 * that we ran out of memory, so the transport treats it as if the packet
 * was lost.
 *
 * If "peer" is NULL, the packet is a multicast.
 */

static igrp2type *igrp2_generate_packet (ddbtype *ddb,
					 igrp2_idbtype *iidb,
					 dual_peertype *peer,
					 igrp2_pakdesc *pakdesc,
					 boolean *packet_suppressed)
{
    igrp2type *igrp2;

    *packet_suppressed = FALSE;
    /*
     * Punt if we are going down.
     */
    if (peer && peer->going_down) {
        return (NULL);
    }

    igrp2 = pakdesc->pregen_pak;
    if (!igrp2) {

	/* If this is an ACK, build it. */
	
	if (pakdesc->ack_packet) {
	    igrp2 = malloc(IGRP2HEADERBYTES);
	    if (igrp2) {
		PUTLONG(&igrp2->ack, pakdesc->ack_seq_num);
		pakdesc->ack_seq_num = 0;	/* Don't send a nonzero SEQ! */
	    }
	    
	} else {
	    
	    /* Call back to generate the packet. */
	    
	    igrp2 = dual_buildpacket(ddb, iidb, peer, pakdesc,
				     packet_suppressed);
	}
    }
	
    if (!igrp2)
	return(NULL);

    /*
     * Set the ACK field (for non-ACKs;  we've already done ACKs above.)
     * For point-to-point hellos, don't fill in the ACK field;  this turns
     * them into ACKs.
     */
    if (!pakdesc->ack_packet) {
	if (peer && pakdesc->opcode != IGRP_OPC_HELLO) {
	    PUTLONG(&igrp2->ack, peer->last_seq_number);
	} else {
	    PUTLONG(&igrp2->ack, 0);
	}
    }

    /* Set INIT and sequence number (0 if unreliable packet). */
    
    if (pakdesc->set_init_flag) {
	PUTLONG(&igrp2->flags, IGRP2_INIT_FLAG);
    } else {
	PUTLONG(&igrp2->flags, 0);
    }
    PUTLONG(&igrp2->sequence, pakdesc->ack_seq_num);
    
    igrp2->version = IGRP2_VERSION;
    PUTLONG(&igrp2->asystem, ddb->asystem);
    igrp2->opcode = pakdesc->opcode;
    return(igrp2);
}

/*
 * igrp2_pacing_value
 *
 * Returns the value of the pacing timer for an interface, based on whether
 * the last packet sent was sequenced (reliable) or not, as well as the
 * length of the packet.
 */
static ulong igrp2_pacing_value (ddbtype *ddb, igrp2_idbtype *iidb,
				 igrp2_pakdesc *pakdesc)
{
    xmit_queue_type qtype;
    ulong interval;

    interval = igrp2_calculate_pacing_interval(ddb, iidb, pakdesc->length,
					       pakdesc->sequenced);
    qtype = pakdesc->sequenced ? RELIABLE_QUEUE : UNRELIABLE_QUEUE;
    iidb->iidb_last_send_interval[qtype] = interval; /* For display... */
    return(interval);
}

/*
 * igrp2_generate_checksum
 *
 * Build the checksum of an outgoing EIGRP packet.
 *
 * The length does *not* include the EIGRP header.
 */
static void igrp2_generate_checksum (igrp2type *igrp2, ulong length)
{
    ushort checksum;

    PUTSHORT(&igrp2->checksum, 0);
    checksum = ipcrc((ushort *)igrp2, length + IGRP2HEADERBYTES);
    PUTSHORT(&igrp2->checksum, checksum);
}

/*
 * igrp2_send_unicast
 *
 * Send the next unicast packet on the queue for this peer.
 *
 * Sets or stops the peer timer appropriately.
 */
static void igrp2_send_unicast (ddbtype *ddb, igrp2_idbtype *iidb,
				   dual_peertype *peer)
{
    igrp2_pakdesc_qelm *qelm;
    igrp2_pakdesc *pakdesc;
    igrp2type *igrp2;
    boolean sequenced;
    boolean retransmission;
    xmit_queue_type qtype;
    boolean suppress_ack;
    boolean packet_suppressed;
    ulong opcode, flags, seq, ack_seq;

    /* Stop the timer first. */

    if (mgd_timer_running(&peer->peer_send_timer)) {
	mgd_timer_stop(&peer->peer_send_timer);
    }

    /* If there's nothing here to send, skip it. */

    qtype = igrp2_next_peer_queue(ddb, peer);
    if (peer->xmitQ[qtype].qhead == NULL) {
	return;
    }

    /*
     * Attempt to do ACK suppression.  If the top of the unreliable queue
     * is an ACK of the last sequence number received from the peer, and
     * the top of the reliable queue is a packet that is ready to be
     * transmitted, delete the ACK and force the reliable packet to be
     * transmitted (regardless of whether a reliable or unreliable packet
     * was scheduled to be sent now).  The outgoing reliable packet will
     * carry the proper acknowledgement.
     *
     * If we don't find anything on the peer reliable queue, look to see
     * if there's a reliable unicast packet sitting on the top of the IIDB
     * queue;  that one's just about to move over here anyhow.
     */

    /* Anything on the unreliable queue? */

    qelm = peer->xmitQ[UNRELIABLE_QUEUE].qhead;
    if (qelm) {
	pakdesc = qelm->pakdesc;

	/* Yep.  Is it a suppressable ACK? */

	if (pakdesc->ack_packet &&
	    peer->last_seq_number == pakdesc->ack_seq_num) {

	    /* Yep.  Any reliable packets present? */

	    suppress_ack = FALSE;
	    qelm = peer->xmitQ[RELIABLE_QUEUE].qhead;
	    if (qelm) {			/* Something there? */
		pakdesc = qelm->pakdesc;

		/* Yep.  Is the reliable packet ready to go? */

		if (!TIMER_RUNNING_AND_SLEEPING(qelm->xmit_time)) {

		    /* Yep.  Delete the ACK. */

		    suppress_ack = TRUE;
		}

	    } else {			/* Nothing on reliable queue */

		/* Try the IIDB queue. */

		qelm = iidb->xmitQ[RELIABLE_QUEUE].qhead;
		if (qelm) {		/* Something there? */
		    pakdesc = qelm->pakdesc;

		    /* Yep.  Is it for us? */

		    if (pakdesc->peer == peer) {

			/* Yep.  Delete the ACK. */

			suppress_ack = TRUE;
		    }
		}
	    }

	    if (suppress_ack) {

		/* Do the deed. */

		igrp2_dequeue_qelm(ddb, peer, UNRELIABLE_QUEUE);
		dual_log_xport(ddb, DUALEV_ACKSUPPR, &peer->source,
			       &peer->source, &iidb->idb);
		if (igrp2_detail_debug(ddb, &peer->source))
		    buginf("\nEIGRP: Suppressed ACK %d to %s on %s",
			   peer->last_seq_number,
			   (*ddb->printaddr)(&peer->source),
			   iidb->idb->namestring);
		qtype = RELIABLE_QUEUE; /* Send the reliable one! */
		iidb->acks_suppressed++;
	    }
	}
    }

    /* Fetch the next packet without dequeueing it. */

    qelm = peer->xmitQ[qtype].qhead;

    /*
     * If there's no packet there, it may be because we just suppressed
     * an ACK.  Kick the pacing timer to keep the chain going.
     */
    if (!qelm) {
	igrp2_kick_pacing_timer(ddb, iidb, 0);
	return;
    }
    pakdesc = qelm->pakdesc;
    sequenced = pakdesc->sequenced;
    retransmission = sequenced && TIMER_RUNNING(qelm->time_sent);

    /*
     * If this is a retransmission, twiddle the right stuff, and bail if
     * the retransmission limit has been hit.
     */
    if (retransmission) {
	if (igrp2_retransmit_packet(ddb, peer)) {
	    return;
	}
	iidb->retransmissions_sent++;
    } else {
	
	/*
	 * If this is the initial transmission of a sequenced packet,
	 * set up some fields.
	 */
	if (sequenced) {
	    GET_TIMESTAMP(peer->pkt_first_send_time);
	}
    }

    /* Generate the packet. */

    packet_suppressed = FALSE;
    igrp2 = igrp2_generate_packet(ddb, iidb, peer, pakdesc,
				  &packet_suppressed);
    dual_log_xport(ddb, DUALEV_UCASTSENT, &peer->source, &iidb->idb,
		   &peer->source);
    dual_log_xport(ddb, DUALEV_PKTSENT2, &peer->source, &pakdesc->start_serno,
		   &pakdesc->end_serno);
    opcode = pakdesc->opcode;	/* Make it 32 bits. */
    if (pakdesc->ack_packet)
	opcode = IGRP_OPC_ACK;

    /* Fill in some important fixed-header fields. */

    if (igrp2) {

	/* Checksum the packet. */

	igrp2_generate_checksum(igrp2, pakdesc->length);
	
	/* Send the packet. */

	if (!igrp2_induce_error(ddb, iidb, "Transmit")) {
	    (*ddb->sendpacket)(igrp2, pakdesc->length + IGRP2HEADERBYTES,
			       peer, iidb, FALSE);
	}
	iidb->ucasts_sent[qtype]++;
	flags = GETLONG(&igrp2->flags);
	seq = GETLONG(&igrp2->sequence);
	ack_seq = GETLONG(&igrp2->ack);
	dual_log_xport(ddb, DUALEV_PKTRCV2, &peer->source, &opcode, &flags);
	dual_log_xport(ddb, DUALEV_PKTRCV3, &peer->source, &seq, &ack_seq);

	/*
	 * Print debug, if enabled.
	 */
	igrp2_debug_sent_pkt(ddb, peer, iidb, pakdesc, igrp2,
			     peer->retry_count);
	igrp2_update_mib(ddb, igrp2, TRUE);

	/* Free the buffer if we generated it locally. */

	if (!pakdesc->pregen_pak)
	    free(igrp2);

    } else {				/* No packet to send */

	/*
	 * No packet.  If the packet was suppressed, act as though it was
	 * sent and acknowledged.  Otherwise, act as though it was sent and
	 * lost.  Only sequenced packets are ever suppressed.
	 */
	if (packet_suppressed) {

	    dual_log_xport(ddb, DUALEV_PKTSUPPR, &peer->source, &opcode, NULL);

	    /* Dequeue and free the queue element. */

	    igrp2_post_ack(ddb, peer, pakdesc);
	    igrp2_kick_pacing_timer(ddb, iidb, 0); /* Don't stall the i/f! */
	    return;
	} else {
	    dual_log_xport(ddb, DUALEV_PKTNOSEND, &peer->source, &opcode,
			   NULL);
	}
    }
	
    /* Kick the interface pacing timer. */
    
    igrp2_kick_pacing_timer(ddb, iidb, igrp2_pacing_value(ddb, iidb, pakdesc));
    dual_log_xport(ddb, DUALEV_XMITTIME, &peer->source, &pakdesc->length,
		   &iidb->iidb_last_send_interval[qtype]);

    /* Make sure that we eventually send some multicasts. */

    if (iidb->unicasts_left_to_send)
	iidb->unicasts_left_to_send--;

    /* Do a few things depending on whether the packet was reliable or not. */

    if (sequenced) {

	/* Note the xmit time for the RTO if not a retransmission. */

	if (!retransmission)
	    GET_TIMESTAMP(qelm->time_sent);

	/* Note the retransmit time. */

	TIMER_START(qelm->xmit_time, peer->rto);

	/* Allow unreliable packets to be sent again. */

	peer->unrely_left_to_send = MAX_UNRELIABLE_PACKETS;

    } else {

	/* Unreliable.  Dequeue and free the queue element. */

	igrp2_dequeue_qelm(ddb, peer, UNRELIABLE_QUEUE);

	/* Charge for the unreliable packet. */

	if (peer->unrely_left_to_send) {
	    peer->unrely_left_to_send--;
	}
    }

    /* Kick the peer timer appropriately for the next packet. */

    igrp2_start_peer_timer(ddb, iidb, peer);
}

/*
 * igrp2_build_sequence_tlv
 *
 * Insert address of peers that should not receive the next multicast
 * packet to be transmitted.
 *
 * Adds the address to an existing structure, or creates one if it isn't
 * there yet.
 */

static igrp2_gentlvtype *igrp2_build_sequence_tlv (ddbtype *ddb,
						   dual_peertype *peer,
						   igrp2_gentlvtype *tlv)
{
    uchar *entry_ptr;
    int len;
    int entry_length;

    /*
     * Malloc TLV block if not already malloced.
     */
    if (!tlv) {
	if (igrp2_detail_debug(ddb, &peer->source))
	    buginf("\nEIGRP: Building Sequence TLV");
	tlv = malloc_named(IGRP2_MAX_SEQ_TLV, "EIGRP SEQ TLV");
	if (!tlv)
	    return(NULL);
	tlv->type = IGRP2_SEQUENCE;
	tlv->length = sizeof(igrp2_gentlvtype);
    }

    len = tlv->length;
    entry_ptr = ((uchar *) tlv) + len;

    entry_length = (*ddb->addrcopy)(&peer->source, entry_ptr);
    len += entry_length;

    /* If the next entry won't fit, bail now so we don't overrun it. */

    if ((IGRP2_MAX_SEQ_TLV - len) < entry_length) {
	free(tlv);
	return(NULL);
    }

    PUTSHORT(&tlv->length, len);
    return(tlv);
}
/*
 * igrp2_send_hello_packet
 *
 * Send a hello packet.  This should be used sparingly, since it bypasses
 * the circuit pacing mechanism by calling the protocol send routine
 * directly.
 *
 * This routine does not free the packet.
 */
static void igrp2_send_hello_packet (ddbtype *ddb, igrp2type *igrp2, 
				     igrp2_idbtype *iidb, int bytes,
				     boolean priority)
{
    igrp2->version = IGRP2_VERSION;
    igrp2->opcode = IGRP_OPC_HELLO;
    PUTLONG(&igrp2->asystem, ddb->asystem);
    PUTLONG(&igrp2->flags, 0);
    PUTLONG(&igrp2->ack, 0);
    PUTLONG(&igrp2->sequence, 0);
    igrp2_generate_checksum(igrp2, bytes);
    if (!igrp2_induce_error(ddb, iidb, "Transmit")) {
	(*ddb->sendpacket)(igrp2, bytes + IGRP2HEADERBYTES, NULL, iidb,
			   priority);
    }

    /* Do debug stuff. */

    igrp2_debug_sent_pkt(ddb, NULL, iidb, NULL, igrp2, 0);
    igrp2_update_mib(ddb, igrp2, TRUE);
}

/*
 * igrp2_send_seq_hello
 *
 * Send a Hello with a Sequence TLV and a Next Multicast Sequence TLV inside.
 * This is sent just before a multicast to limit the set of receiving peers.
 * The multicast has the CR bit set to reference this sequence TLV hello.
 * The sequence number of the multicast is added so that we don't have any
 * ambiguity in the face of lost packets.
 *
 * We call the send routine directly, skipping all queues, since we're
 * in the middle of peeling things off of the queue as we generate it.
 */
static void igrp2_send_seq_hello (ddbtype *ddb, igrp2_idbtype *iidb,
				  igrp2_pakdesc *pakdesc,
				  igrp2_gentlvtype *tlv)
{
    igrp2type *igrp2;
    int bytes;
    void *next_tlv;
    igrp2_next_multi_seq_type *multiseq;

    /* Allocate a buffer for this pile. */

    bytes = IGRP2HELLOHDRBYTES + tlv->length + IGRP2_MULTISEQ_TLV_LENGTH;
    igrp2 = igrp2_getbuf(bytes + IGRP2HEADERBYTES);
    if (!igrp2)
	return;

    /* Build the base hello packet. */

    next_tlv = igrp2_hello(igrp2, ddb->k1, ddb->k2, ddb->k3, ddb->k4, ddb->k5,
			iidb->holdtime/ONESEC);

    /* Add the SEQUENCE TLV. */

    bcopy(tlv, next_tlv, tlv->length);

    next_tlv = ((uchar *) next_tlv) + tlv->length;

    /* Add the multicast sequence number. */

    multiseq = next_tlv;
    PUTSHORT(&multiseq->type, IGRP2_NEXT_MCAST_SEQ);
    PUTSHORT(&multiseq->length, IGRP2_MULTISEQ_TLV_LENGTH);
    PUTLONG(&multiseq->seq_num, pakdesc->ack_seq_num);

    if (igrp2_detail_debug(ddb, NULL) &&
	(igrp2_packet_debug_flags & IGRP2_DEBUG_HELLO)) {
	buginf("\nEIGRP: Sending HELLO with Sequence TLV on %s, seq %d",
	       iidb->idb->namestring, pakdesc->ack_seq_num);
    }

    /* Send it. */

    igrp2_send_hello_packet(ddb, igrp2, iidb, bytes, FALSE);
    free(igrp2);
}

/*
 * igrp2_enqueue_peer_qelms
 *
 * Enqueue unicast peer queue elements for this multicast packet.
 *
 * Returns a pointer to a sequence TLV if the multicast is reliable and
 * some of the recipients are not flow-control ready.
 *
 * Returns TRUE if things generally went OK, FALSE if we couldn't generate
 * the sequence TLV but needed to (so that the caller doesn't try to send
 * the multicast packet).
 */
static boolean igrp2_enqueue_peer_qelms (ddbtype *ddb, igrp2_idbtype *iidb,
					 igrp2_pakdesc *pakdesc,
					 igrp2_gentlvtype **seq_tlv)
{
    dual_peertype *peer;
    igrp2_pakdesc_qelm *qelm;
    sys_timestamp transmit_time;
    boolean sequenced;
    xmit_queue_type qtype;

    sequenced = pakdesc->sequenced;

    /* Skip the whole thing if unreliable and multicast. */

    if (!sequenced && iidb->use_multicast)
	return(TRUE);

    GET_TIMESTAMP(transmit_time);	/* Send the first one now? */

    /* Loop over each peer on the interface. */

    peer = ddb->peer_list;
    while (peer) {

	/* Send on this peer if it is on the interface and isn't half-up. */

	if (peer->iidb == iidb && !peer->coming_up) {

	    /* Allocate a queue element. */

	    qelm = chunk_malloc(igrp2_pakdesc_qelm_chunks);
	    if (!qelm) {		/* No memory */
		return(FALSE);		/* Pretty hopeless, actually */
	    }
	    igrp2_bind_pakdesc(pakdesc, qelm);

	    /*
	     * If this is going to be multicast and the peer queue contains
	     * some sequenced packets (not flow control ready), set up a
	     * sequence TLV.  Note that we will only get here for sequenced
	     * packets, since the check at the top would have bailed out
	     * otherwise.
	     */
	    if (iidb->use_multicast) {
		if (peer->xmitQ[RELIABLE_QUEUE].count) {
		    *seq_tlv = igrp2_build_sequence_tlv(ddb, peer, *seq_tlv);
		    iidb->mcast_exceptions_sent++;
		    if (!*seq_tlv)
			return(FALSE);
		    /*
		     * Print debug, if enabled.
		     */
		    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
		} else {
		    
		    /*
		     * Going out as a multicast now, which means that the
		     * unicast copy is going to be a retransmission.  Note
		     * that we're sending it now (for RTT calculation) and
		     * note the retransmit time.
		     */
		    GET_TIMESTAMP(qelm->time_sent);
		    TIMER_START(qelm->xmit_time, peer->rto);
		    GET_TIMESTAMP(peer->pkt_first_send_time);
		}
	    } else {
		/*
		 * The packet will be unicast.  Set the transmit time
		 * to be one pacing time after the last peer's, since
		 * we can't send them all at the same time anyhow.  We use
		 * the base interval, since we don't know how big the packet
		 * is actually going to be.
		 */
		qelm->xmit_time = transmit_time;
		TIMER_UPDATE(transmit_time,
			     iidb->iidb_send_interval[RELIABLE_QUEUE]);

		/*
		 * Print debug, if enabled.
		 */
		igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	    }

	    /*
	     * Enqueue the element onto the peer's queue.
	     */
	    qtype = sequenced ? RELIABLE_QUEUE : UNRELIABLE_QUEUE;
	    enqueue(&peer->xmitQ[qtype], qelm);

	    /* Kick the peer timer appropriately. */

	    igrp2_start_peer_timer(ddb, iidb, peer);
	}
	peer = peer->next_peer;
    }
    return(TRUE);
}

/*
 * igrp2_flow_block_interface
 *
 * Mark an interface as flow-blocked for multicast.
 */
static void igrp2_flow_block_interface (ddbtype *ddb, igrp2_idbtype *iidb,
					igrp2_pakdesc_qelm **qelm)
{

    /*
     * Save a pointer to the packet, which makes the
     * interface flow-blocked for multicast.  Whimper if there's
     * already one there (there better not be).
     */
    if (iidb->last_multi_pak) {
	errmsg(&msgsym(MULTIFLOW, DUAL), "Qelm on flow_ready",
	       iidb->idb->namestring);
	igrp2_cleanup_multipak(ddb, iidb);
    }
    iidb->last_multi_pak = *qelm;
    if (*qelm)
	iidb->mcast_start_refcount = (*qelm)->pakdesc->refcount;
    *qelm = NULL;			/* Make sure it doesn't get freed. */

    /* Start the flow timer. */

    igrp2_start_mcast_flow_timer(ddb, iidb);
}

/*
 * igrp2_send_multicast
 *
 * Send the supplied multicast packet.
 *
 * Assumes that the interface timer has been stopped.  It may restart it.
 *
 * If the packet is reliable, or if this is an NBMA network (where we don't
 * actually try to send multicasts), the packet will be queued appropriately
 * on each peer's queue.
 *
 * This routine assumes that there is something in
 */
static void igrp2_send_multicast (ddbtype *ddb, igrp2_idbtype *iidb,
				  igrp2_pakdesc_qelm *qelm)
{
    igrp2_pakdesc *pakdesc;
    boolean sequenced;
    boolean tlv_success;
    igrp2_gentlvtype *seq_tlv;
    igrp2type *igrp2;
    boolean packet_suppressed;
    ulong opcode, flags, seq, ack_seq;
    xmit_queue_type qtype;

    /* Fetch the packet descriptor. */

    pakdesc = qelm->pakdesc;
    sequenced = pakdesc->sequenced;
    qtype = sequenced ? RELIABLE_QUEUE : UNRELIABLE_QUEUE;
    packet_suppressed = FALSE;
    opcode = pakdesc->opcode;	/* Make it 32 bits */
    dual_log_xport_all(ddb, DUALEV_MCASTSENT, &iidb->idb, NULL);
    dual_log_xport_all(ddb, DUALEV_PKTSENT2, &pakdesc->start_serno,
		       &pakdesc->end_serno);

    /* Generate the packet if we're actually sending a multicast. */

    if (iidb->use_multicast) {
	igrp2 = igrp2_generate_packet(ddb, iidb, NULL, pakdesc,
				      &packet_suppressed);
    } else {
	igrp2 = NULL;
    }

    /*
     * Go try to generate (re)transmission entries for this packet on
     * each peer on the interface.  If this is an NBMA or P2P interface, we
     * generate transmission entries regardless of whether the packet
     * is sequenced or not (because we don't actually transmit multicasts
     * on these interfaces).  Otherwise, we generate retransmission entries
     * if the packet is sequenced.
     *
     * We may be given a SEQ TLV hello packet in return, to tell which
     * of our neighbors should not receive the multicast.  We will
     * only get this back if multicasts are in use.
     *
     * Note that igrp2 may be NULL if the packet was suppressed.  We must
     * enqueue the peer qelms anyhow in order to ensure that packets are
     * acknowledged back to DUAL in the order that they were generated.
     */
    seq_tlv = NULL;
    tlv_success = igrp2_enqueue_peer_qelms(ddb, iidb, pakdesc, &seq_tlv);

    if (igrp2 || !iidb->use_multicast) { /* If there's something to send */
    
	if (igrp2) {
	    if (seq_tlv) {
		
		/* Got a SEQ TLV.  Send it in a Hello, and set the CR flag. */
		
		igrp2_send_seq_hello(ddb, iidb, pakdesc, seq_tlv);
		free(seq_tlv);
		PUTLONG(&igrp2->flags, GETLONG(&igrp2->flags) | IGRP2_CR_FLAG);
		iidb->cr_packets_sent++;
	    }
	    
	    /* Checksum the packet. */
	    
	    igrp2_generate_checksum(igrp2, pakdesc->length);
	}
	
	/*
	 * If not NBMA/P2P or unreliable, send the packet.
	 *
	 * If we ran out of TLV space, don't send the multicast (since
	 * otherwise it would fall into the wrong hands) and let the unicast
	 * retransmissions save our butt.
	 */
	if ((iidb->use_multicast && tlv_success) || !sequenced) {
	    if (!igrp2) {
		errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
		return;
	    }
	    if (!igrp2_induce_error(ddb, iidb, "Transmit")) {
		(*ddb->sendpacket)(igrp2, pakdesc->length + IGRP2HEADERBYTES,
				   NULL, iidb, FALSE);
	    }
	    iidb->mcasts_sent[qtype]++;
	    flags = GETLONG(&igrp2->flags);
	    seq = GETLONG(&igrp2->sequence);
	    ack_seq = GETLONG(&igrp2->ack);
	    dual_log_xport_all(ddb, DUALEV_PKTRCV2, &opcode, &flags);
	    dual_log_xport_all(ddb, DUALEV_PKTRCV3, &seq, &ack_seq);
	    
	    /*
	     * Print debug, if enabled.
	     */
	    igrp2_debug_sent_pkt(ddb, NULL, iidb, pakdesc, igrp2, 0);
	    igrp2_update_mib(ddb, igrp2, TRUE);
	    
	    /* Kick the interface pacing timer. */
	    
	    igrp2_kick_pacing_timer(ddb, iidb,
				    igrp2_pacing_value(ddb, iidb, pakdesc));
	    dual_log_xport_all(ddb, DUALEV_XMITTIME, &pakdesc->length,
			       &iidb->iidb_last_send_interval[qtype]);

	}
	
	/*
	 * If sequenced, mark the interface as flow-control blocked, and
	 * save the sequence number.
	 */
	if (sequenced) {
	    igrp2_flow_block_interface(ddb, iidb, &qelm);
	    iidb->iidb_last_mcast_seq = pakdesc->ack_seq_num;
	}
	
	/*
	 * If the packet was pregenerated, clear the CR bit now.  Otherwise,
	 * free the packet, since it will be regenerated later if necessary.
	 * This is safe to do, since ddb->sendpacket copies the packet we
	 * give it.
	 */
	if (igrp2) {
	    if (pakdesc->pregen_pak) {
		PUTLONG(&igrp2->flags,
			GETLONG(&igrp2->flags) & (~IGRP2_CR_FLAG));
	    } else {
		free(igrp2);
	    }
	}

    } else {				/* No packet to send */

	/*
	 * No packet to send.  This is usually because the contents of the
	 * packet have been overtaken by events (or suppressed for some other
	 * reason).  We kick the pacing timer to expire immediately.
	 *
	 * If this is due to having insufficient memory, we do the same thing.
	 */
	if (packet_suppressed) {
	    dual_log_xport_all(ddb, DUALEV_PKTSUPPR, &opcode, NULL);
	}
	free(seq_tlv);			/* Free it if it's there */
	IGRP2_XBUGINF(XDBBUILD, "\nPacket suppressed on %s, serno %d-%d",
		      iidb->idb->namestring, pakdesc->start_serno,
		      pakdesc->end_serno);
	igrp2_kick_pacing_timer(ddb, iidb, 0);
    }

    iidb->unicasts_left_to_send = MAX_UNICAST_PACKETS; /* We can send lots */

    /* Free the queue element. */

    if (qelm)
	igrp2_free_qelm(ddb, qelm, iidb, NULL);
}

/*
 * igrp2_peer_send_timer_expiry
 *
 * Process the expiration of a peer send timer.  This means that it's time to
 * (re)send the first packet on the queue.
 *
 * The peer timer will be stopped or restarted as necessary.
 */
static void igrp2_peer_send_timer_expiry (ddbtype *ddb, mgd_timer *timer)
{
    dual_peertype *peer;
    igrp2_idbtype *iidb;

    peer = mgd_timer_context(timer);
    iidb = peer->iidb;

    /* If the interface timer is running, punt until it is ready. */

    if (igrp2_defer_peer_timer(ddb, iidb, peer))
	return;

    /* The interface timer isn't running.  Time for us to send our packet. */

    igrp2_send_unicast(ddb, iidb, peer);
}

/*
 * igrp2_interface_pacing_timer_expiry
 *
 * Process the expiration of an interface pacing timer.  This means that
 * the pacing interval has expired and it's now safe to send another
 * packet.
 *
 * The timer will be stopped or restarted as necessary.
 *
 * We decide whether the next packet sent should be a multicast or a
 * unicast.  Given enough traffic, we will guarantee each of them some
 * traffic so as to not starve out either type.  If we don't have any of
 * one type, we'll send the other type instead.
 *
 * The IIDB reliable transmit queue contains all reliable multicasts and
 * initial unicasts.  When a multicast is pulled off the queue, it is
 * immediately transmitted.  When a unicast is pulled off the queue, it
 * is requeued on the peer queue.  This ensures that packets stay in
 * the order in which they were delivered to the transport.
 *
 * If it's time for a multicast, we send it now.  If not, we stop the
 * pacing timer in expectation that a peer timer will expire to send
 * a unicast.
 *
 * Unreliable multicasts are always sent ahead of reliable multicasts.
 * At this point, only outgoing Hello packets fit this description.
 * There is no mechanism to keep unreliable packets from hogging all
 * the bandwidth;  this should be added if there is ever a chance of
 * scads of unreliable multicasts being sent.
 */
static void igrp2_interface_pacing_timer_expiry (ddbtype *ddb,
						 mgd_timer *timer)
{
    igrp2_idbtype *iidb;
    igrp2_pakdesc_qelm *qelm;
    dual_peertype *peer;
    boolean requeued_unicast;

    iidb = mgd_timer_context(timer);
    mgd_timer_stop(timer);

    /* A loop, for picking off multiple unicasts in a row. */

    requeued_unicast = FALSE;
    while (TRUE) {

	/*
	 * If there are unicasts ready to send and it is legal to send
	 * them now (we haven't hit quota yet), simply bail and the
	 * peer timer will fire for the next peer.  Note that if we
	 * are blocked because the unicast quota has been exhausted but
	 * there are no multicasts to send, we will end up sending the
	 * next unicast anyhow.
	 */
	if (iidb->unicasts_left_to_send &&
	    mgd_timer_expired(&iidb->iidb_peer_timer)) {
	    break;
	}

	/* Note if we are forcing a multicast transmission. */

	if (igrp2_detail_debug(ddb, NULL) && !iidb->unicasts_left_to_send &&
	    mgd_timer_expired(&iidb->iidb_peer_timer)) {
	    buginf("\nEIGRP: Forcing multicast xmit on %s",
		   iidb->idb->namestring);
	}

	/*
	 * Can't send a unicast right now.  If there's an unreliable
	 * packet (multicast) on the IIDB queue, send it now.
	 */
	qelm = dequeue(&iidb->xmitQ[UNRELIABLE_QUEUE]);
	if (qelm) {			/* Got one */
	    igrp2_send_multicast(ddb, iidb, qelm);
	    break;
	}

	/*
	 * No unreliable packets available.  Go next to the reliable packet
	 * queue.  If the packet there is a multicast, send it.  If it is
	 * a unicast, requeue it for the peer and go around again.
	 */
	qelm = iidb->xmitQ[RELIABLE_QUEUE].qhead;
	if (qelm) { /* Got one */

	    peer = qelm->pakdesc->peer;

	    /* Separate out unicasts and multicasts. */

	    if (peer) {			/* It's a unicast */

		/*
		 * Unicast.  Queue onto the peer queue and kick
		 * the peer timer.  Go back to the top of the loop on
		 * the chance that there's another unicast waiting there.
		 */
		dequeue(&iidb->xmitQ[RELIABLE_QUEUE]);
		enqueue(&peer->xmitQ[RELIABLE_QUEUE], qelm);
		peer->coming_up = FALSE; /* It's ok to queue mcasts now */
		dual_log_xport(ddb, DUALEV_REQUEUEUCAST, &peer->source,
			       &peer->source, &qelm->pakdesc->ack_seq_num);
		igrp2_start_peer_timer(ddb, iidb, peer);
		requeued_unicast = TRUE;
		if (igrp2_detail_debug(ddb, NULL)) {
		    buginf("\nEIGRP:  Requeued unicast on %s",
			   iidb->idb->namestring);
		}
		continue;		/* Go around for more. */

	    } else {			/* It's a multicast */

		/*
		 * Multicast packet.  If we've already requeued a unicast,
		 * then bail out;  the unicast will be transmitted forthwith.
		 * Otherwise, if we're flow-blocked for multicast,
		 * bail out.  The pacing timer will be kicked when the
		 * flow control becomes ready.
		 */
		if (requeued_unicast)	/* Go send the unicast. */
		    break;

		if (igrp2_multicast_transmit_blocked(iidb)) { /* Blocked? */
		    if (igrp2_detail_debug(ddb, NULL)) {
			buginf("\nEIGRP: %s multicast flow blocked",
			       iidb->idb->namestring);
		    }
		    break;		/* Bail out */

		} else {		/* Flow-ready */

		    /*
		     * Multicast flow control ready.  Dequeue the packet
		     * and transmit it as a multicast (or pseudomulticast
		     * if this is an NBMA interface).
		     */
		    dequeue(&iidb->xmitQ[RELIABLE_QUEUE]);
		    igrp2_send_multicast(ddb, iidb, qelm);
		    break;		/* Exit the loop */
		}
	    }
	}
	break;				/* Get out of the loop. */
    }					/* Bottom of WHILE loop */
}

/*
 * igrp2_show_neighbor_q
 * Display the packets queued up for transmittal to this neighbor.
 */

static void igrp2_show_neighbor_q (ddbtype *ddb, dual_peertype *peer)
{
    igrp2_pakdesc_qelm *qelm;
    igrp2_pakdesc *pakdesc;
    int j;
    xmit_queue_type qtype;
    int opcode;

    for (qtype = 0; qtype < NUM_QUEUE_TYPES; qtype++) {
	for (j = 0; (qelm = queuepick(&peer->xmitQ[qtype].qhead, j)); j++) {
	    if (automore_quit())
		return;
	    pakdesc = qelm->pakdesc;
	    opcode = pakdesc->opcode;
	    if (opcode == IGRP_OPC_HELLO && pakdesc->ack_seq_num)
		opcode = IGRP_OPC_ACK;
	    printf("\n    %s seq %u ser %u-%u",
		   igrp2_otype(pakdesc->opcode),
		   pakdesc->ack_seq_num,
		   pakdesc->start_serno, pakdesc->end_serno);
	    if (pakdesc->pregen_pak)
		printf(" Len %d", pakdesc->length);
	    if (TIMER_RUNNING(qelm->time_sent))
		printf(" Sent %d", ELAPSED_TIME(qelm->time_sent));
	    if (pakdesc->set_init_flag)
		printf(" Init");
	    if (pakdesc->sequenced)
		printf(" Sequenced");
	}
    }
}

/*
 * igrp2_print_thread_serno
 *
 * Prints the serial number associated with a thread entry, or "none" if
 * no entry is present.
 */
static void igrp2_print_thread_serno (igrp2_xmit_thread *thread)
{
    if (thread) {
	printf("%d", thread->xt_serno);
    } else {
	printf("<none>");
    }
}

/*
 * igrp2_print_anchored_serno
 *
 * Prints the serial number associated with an anchored thread, or "none"
 * if none is present.
 */
static void igrp2_print_anchored_serno (igrp2_xmit_anchor *anchor)
{
    igrp2_xmit_thread *thread;
    thread = NULL;
    if (anchor)
	thread = anchor->xa_thread;
    igrp2_print_thread_serno(thread);
}

/*
 * igrp2_show_interface
 *
 * Display EIGRP interface-specific information.
 */
static const char if_banner[] = {
"\n                    Xmit Queue   Mean   Pacing Time   Multicast    Pending"
"\nInterface    Peers  Un/Reliable  SRTT   Un/Reliable   Flow Timer   Routes"
};

void igrp2_show_interface (ddbtype *ddb, idbtype *idb, boolean show_detail)
{
    igrp2_idbtype *iidb;
    ulong pending_routes;
    igrp2_xmit_thread *thread;

    automore_enable(if_banner);

    /* Loop over the interface queue. */

    for (iidb = ddb->iidbQ.qhead; iidb; iidb = iidb->next) {

	/* Skip this interface if it doesn't match the selected one. */

	if (idb && (iidb->idb != idb))
	    continue;

	/* Count the pending routes. */

	pending_routes = 0;
	if (iidb->iidb_xmit_anchor) {	/* Anchored? */
	    thread = iidb->iidb_xmit_anchor->xa_thread;
	    while (thread) {
		if (!thread->xt_dummy)
		    pending_routes++;
		thread = thread->xmit_next;
	    }
	}
	    
	/* Print out the good stuff. */

	mem_lock(iidb);
	printf("\n%12s %-3d      %-3d/%3d   %-5d    %-4d/%4d      %-4d       "
	       "%-5d",
	       iidb->idb->short_namestring, igrp2_peer_count(iidb),
	       iidb->xmitQ[UNRELIABLE_QUEUE].count,
	       iidb->xmitQ[RELIABLE_QUEUE].count,
	       iidb->total_srtt/max(1, igrp2_peer_count(iidb)),
	       iidb->iidb_send_interval[UNRELIABLE_QUEUE],
	       iidb->iidb_send_interval[RELIABLE_QUEUE],
	       iidb->last_mcast_flow_delay, pending_routes);

	/* Print detailed information if asked. */

	if (show_detail) {
	    printf("\n  Next xmit serial ");
	    igrp2_print_anchored_serno(iidb->iidb_xmit_anchor);
	    if (mgd_timer_running(&iidb->iidb_packetize_timer))
		printf(", packetize pending");
	    printf("\n  Un/reliable mcasts: %d/%d  "
		   "Un/reliable ucasts: %d/%d",
		   iidb->mcasts_sent[UNRELIABLE_QUEUE],
		   iidb->mcasts_sent[RELIABLE_QUEUE],
		   iidb->ucasts_sent[UNRELIABLE_QUEUE],
		   iidb->ucasts_sent[RELIABLE_QUEUE]);
	    printf("\n  Mcast exceptions: %d  CR packets: %d  "
		   "ACKs suppressed: %d",
		   iidb->mcast_exceptions_sent, iidb->cr_packets_sent,
		   iidb->acks_suppressed);
	    printf("\n  Retransmissions sent: %d  Out-of-sequence rcvd: %d",
		   iidb->retransmissions_sent, iidb->out_of_sequence_rcvd);
	    if (ddb->iidb_show_detail)
		(*ddb->iidb_show_detail)(ddb, iidb);
	}
	free(iidb);
    }
    automore_disable();
}

/*
 * igrp2_display_one_timer
 *
 * Callback routine for mgd_timer_display_tree
 */
static char *igrp2_display_one_timer (mgd_timer *timer, ushort type, char *buf)
{
    igrp2_idbtype *iidb;
    idbtype *idb;

    if (mgd_timer_is_parent(timer))
	return("(parent)");

    /* Determine the IDB. */

    switch (type) {
      case IGRP2_IIDB_PACING_TIMER:
      case IGRP2_IIDB_HELLO_TIMER:
      case IGRP2_FLOW_CONTROL_TIMER:
      case IGRP2_PACKETIZE_TIMER:
      case IGRP2_IIDB_ACL_CHANGE_TIMER:
	iidb = mgd_timer_context(timer);
	if (!validmem(iidb) || !validmem(iidb->idb))
	    return("Invalid");
	idb = iidb->idb;
	break;

      default:
	idb = NULL;
	break;
    }

    switch (type) {
      case IGRP2_IIDB_PACING_TIMER:
	sprintf(buf, "Interface pacing (%s)", idb->namestring);
	return(buf);

      case IGRP2_PEER_SEND_TIMER:
	return("Peer transmission");

      case IGRP2_IIDB_HELLO_TIMER:
	sprintf(buf, "Hello (%s)", idb->namestring);
	return(buf);

      case IGRP2_PEER_HOLDING_TIMER:
	return("Peer holding");

      case IGRP2_FLOW_CONTROL_TIMER:
	sprintf(buf, "Interface flow control (%s)", idb->namestring);
	return(buf);

      case IGRP2_ACTIVE_TIMER:
	return("Active check");

      case IGRP2_PACKETIZE_TIMER:
	sprintf(buf, "Packetization (%s)", idb->namestring);
	return(buf);

      case IGRP2_PDM_TIMER:
	return("Protocol dependent");

      case IGRP2_IIDB_ACL_CHANGE_TIMER:
	sprintf(buf, "Access list change delay (%s)", idb->namestring);
	return(buf);

      case IGRP2_DDB_ACL_CHANGE_TIMER:
	return("Access list change delay");

      default:
	return("Unknown");
    }
}

/*
 * igrp2_show_timers
 *
 * Called by IGRP2 protocol dependent functions.
 */
void igrp2_show_timers (ddbtype *ddb)
{
    printf("\n  Hello Process");
    automore_enable("    Expiration    Type");
    mgd_timer_display_tree(&ddb->ddb_hello_timer, igrp2_display_one_timer);
    automore_disable();
    printf("\n\n  Update Process");
    automore_enable("    Expiration    Type");
    mgd_timer_display_tree(&ddb->ddb_master_timer, igrp2_display_one_timer);
    automore_disable();
}

/*
 * igrp2_show_neighbors
 *
 * Called by IGRP2 protocol dependent functions.
 */
static const char peer_banner[] = {
  "H   Address                 Interface   Hold Uptime   SRTT   RTO  Q  Seq"
"\n                                        (sec)         (ms)       Cnt Num"};

void igrp2_show_neighbors (ddbtype *ddb, idbtype *idb, boolean show_pdm,
			   boolean show_detail)
{

    dual_peertype *peer, *next;
    boolean banner;
    ulong holdtime;
    
    banner = FALSE;
    for (peer = ddb->peer_list; peer; peer = next) {
	next = peer->next_peer;
	if (idb && peer->iidb->idb != idb) continue;
	if (!banner) {
	    automore_enable(peer_banner);
	    banner = TRUE;
	}
	holdtime = mgd_timer_left_sleeping(&peer->holding_timer) / ONESEC;
	
	mem_lock(peer);

	/*
	 * Print one line of peer info.
	 */
	printf("\n%3d %23s %12s %-3d %8TE %-4d %-5d %-2d  %d",
	       peer->peer_handle,
 	       (*ddb->printaddr)(&peer->source), 
 	       peer->iidb->idb->short_namestring,
	       holdtime, peer->uptime, peer->srtt, peer->rto,
	       peer->xmitQ[UNRELIABLE_QUEUE].count +
	       peer->xmitQ[RELIABLE_QUEUE].count,
	       peer->last_seq_number);
	if (show_detail) {
	    if (peer->last_startup_serno) {
		printf("\n   Last startup serial %d",
		       peer->last_startup_serno);
	    }
	    printf("\n   Version %d.%d/%d.%d, ",
		   peer->peer_version.ios_majorver,
		   peer->peer_version.ios_minorver,
		   peer->peer_version.eigrp_majorrev,
		   peer->peer_version.eigrp_minorrev);
	    printf("Retrans: %d, Retries: %d",
		   peer->retransmit_count, peer->retry_count);
	    if (peer->need_init)
		printf(", Waiting for Init");
	    if (peer->need_init_ack)
		printf(", Waiting for Init Ack");
	    if (TIMER_RUNNING(peer->reinit_start)) {
		printf(", reinit for %d msec",
		       ELAPSED_TIME(peer->reinit_start));
	    }
	    igrp2_show_neighbor_q(ddb, peer);
	}
	if (show_pdm && (ddb->peershowhook != NULL))
	    (*ddb->peershowhook)(ddb, peer);
	next = peer->next_peer;
	free(peer);
    }
    automore_disable();
}

/*
 * igrp2_print_send_flag
 *
 * Prints the contents of the DNDB/DRDB send_flag bits
 */
static void igrp2_print_send_flag (ushort send_flag)
{
    if (!send_flag)
	return;

    printf(", ");

    if (send_flag & DUAL_MULTUPDATE) {
	printf("U");
    }
    if (send_flag & DUAL_MULTQUERY) {
	printf("Q");
    }
    if (send_flag & DUAL_SENDREPLY) {
	printf("R");
    }
}

static const char topo_banner[] = {
"\nCodes: P - Passive, A - Active, U - Update, Q - Query, R - Reply,"
"\n       r - Reply status\n"};

/*
 * igrp2_show_topology
 *
 * Called by IGRP2 protocol dependent functions. Displays the DUAL topology
 * table.
 */

void igrp2_show_topology (ddbtype *ddb, int which)
{
    dual_ndbtype *dndb, *next;
    dual_rdbtype *drdb, *next_drdb;
    int i;
    boolean banner;
    boolean active;
    boolean connected;
    boolean redistributed;
    boolean summary;
    boolean FSonly;
    igrp2_idbtype *iidb;
    igrp2_xmit_thread *thread;
    ulong thread_count, drdb_count, dummy_count;
    boolean display_drdb;
    igrp2_handle_type reply_status;
    dual_peertype *peer;
    ulong handle_num;
    ulong tag;

    /* Allocate memory for a copy of the reply status. */

    reply_status.handle_array =
	malloc(HANDLE_MALLOC_SIZE(ddb->ddb_handles.handle_array_size));
    if (!reply_status.handle_array)
	return;

    if (which == IGRP2_TOP_SUMMARY) {
	printf("\nHead serial ");
	igrp2_print_thread_serno(ddb->ddb_thread_head);
	printf(", next serial %d", ddb->ddb_next_serno);
    }
    thread_count = 0;
    dummy_count = 0;
    drdb_count = 0;
    thread = ddb->ddb_thread_head;
    while (thread) {
	thread_count++;
	if (thread->xt_dummy) {
	    dummy_count++;
	} else if (thread->xt_drdb) {
	    drdb_count++;
	}
	thread = thread->xmit_next;
    }
    if (which == IGRP2_TOP_SUMMARY) {
	printf("\n%d routes, %d pending replies, %d dummies",
	       (thread_count - drdb_count - dummy_count), drdb_count,
	       dummy_count);
	printf("\n%s enabled on %d interfaces, neighbors present on %d "
	       "interfaces", ddb->name, ddb->iidbQ.count,
	       ddb->ddb_active_if_count);
	printf("\nQuiescent interfaces: ");
	iidb = ddb->quiescent_iidbs;
	while (iidb) {
	    printf(" %s", iidb->idb->short_namestring);
	    iidb = iidb->next_quiescent;
	}
	return;
    }
    
    banner = FALSE;
    for (i = 0; i < NETHASHLEN; i++) {
	for (dndb = ddb->topology[i]; dndb; dndb = next) {
	    active = dual_dndb_active(dndb);
	    reply_status.handles_used =
		dndb->dndb_reply_status.handles_used;
	    reply_status.handle_array_size =
		dndb->dndb_reply_status.handle_array_size;
	    if (active) {
		bcopy(dndb->dndb_reply_status.handle_array,
		      reply_status.handle_array,
		      HANDLE_MALLOC_SIZE(reply_status.handle_array_size));
	    }
	    FSonly = FALSE;
	    switch (which) {
	      case IGRP2_TOP_PASSIVE:
		if (active) {
		    next = dndb->dndb_next;
		    continue;
		}
		break;
	      case IGRP2_TOP_ACTIVE:
		if (!active) {
		    next = dndb->dndb_next;
		    continue;
		}
		break;
	      case IGRP2_TOP_ZERO:
		if (dndb->dndb_successors > 0) {
		    next = dndb->dndb_next;
		    continue;
		}
		break;
	      case IGRP2_TOP_PENDING:
		if (!dndb->dndb_send_flag) {
		    next = dndb->dndb_next;
		    continue;
		}
		break;
	      case IGRP2_TOP_FS:

		/*
		 * Print only links that have a reported distance less
		 * than FD.
		 */
		FSonly = TRUE;
		break;
	      case IGRP2_TOP_ALL:
		/*
		 * Fall through
		 */
	    }

	    if (!banner) {
		automore_enable(topo_banner);
		banner = TRUE;
	    }
	    
	    if (automore_quit())
		goto punt;
	    
	    mem_lock(dndb);
	    printf("\n%s %s, %d %successors, FD is ", (active) ? "A" : "P",
		   (*ddb->printnet)(&dndb->dndb_dest), dndb->dndb_successors,
		   dndb->dndb_succ ? "S" : "s");
	    if (dndb->dndb_old_metric == (ulong) -1) {
		printf("Inaccessible");
	    } else {
		printf("%u", dndb->dndb_old_metric);
	    }
 
 	    if (ddb->extract_tag) {
 		tag = (*ddb->extract_tag)(dndb->dndb_rdb);
 		if (tag) {
 		    printf(", tag is %d", tag);
 		}
 	    }
 
	    igrp2_print_send_flag(dndb->dndb_send_flag);

	    if (which == IGRP2_TOP_ALL) {
		printf(", serno %d", dndb->dndb_xmit_thread.xt_serno);
		if (dndb->dndb_xmit_thread.xt_refcount)
		    printf(", refcount %d",
			   dndb->dndb_xmit_thread.xt_refcount);
		if (dndb->dndb_xmit_thread.xt_anchor)
		    printf(", anchored");
	    }

	    /*
	     * Indicate number of outstanding replies.
	     */
	    if (reply_status.handles_used != 0) {
		printf("\n    %d replies", reply_status.handles_used);
	    }
	    if (active) {
		printf(", active ");
		print_dhms(dndb->dndb_active_time);
		printf(", query-origin: %s",
		       dual_query_origin_str(dndb->query_origin));
	    }

	    /*
	     * Print rdbs.
	     */
	    for (drdb = dndb->dndb_rdb; drdb; drdb = next_drdb) {
		display_drdb = !(FSonly &&
		    (drdb->drdb_successor_metric >= dndb->dndb_old_metric));
		mem_lock(drdb);

		/*
		 * Print next hop and metrics.
		 */
		connected = (drdb->drdb_origin == IGRP2_ORG_CONNECTED);
		redistributed = (drdb->drdb_origin == IGRP2_ORG_REDISTRIBUTED);
		summary = (drdb->drdb_origin == IGRP2_ORG_SUMMARY);

		if (display_drdb) {
		    printf("\n       %s via %s",
			   (dndb->dndb_succ == drdb) ? "*" : " ",
			   summary ? "Summary" : connected ? "Connected" :
			   redistributed ? "Redistributed" :
			   (*ddb->printaddr)(&drdb->drdb_nexthop));
		    
		    if (drdb->drdb_successor_metric == METRIC_INACCESSIBLE) {
			printf(" (Infinity/Infinity)");
		    } else if (drdb->drdb_metric == METRIC_INACCESSIBLE) {
			printf(" (Infinity/%u)", drdb->drdb_successor_metric);
		    } else if (!connected) {
			printf(" (%u/%u)", drdb->drdb_metric,
			       drdb->drdb_successor_metric);
		    }

		    /*
		     * Flag replay status waiting.
		     */
		    if (active && (drdb->drdb_handle != NO_PEER_HANDLE) &&
			igrp2_test_handle(ddb, &reply_status,
					  drdb->drdb_handle)) {
			printf(", r");
			igrp2_clear_handle(ddb, &reply_status,
					   drdb->drdb_handle);
		    }

		    /* Print update, query, or reply flags. */

		    igrp2_print_send_flag(drdb->drdb_send_flag);
		    
		    /*
		     * Print next hop interface.
		     */
		    if (drdb->drdb_iidb)
			printf(", %s", dual_idb(drdb->drdb_iidb)->namestring);
		    if (drdb->drdb_thread.xt_serno)
			printf(", serno %d", drdb->drdb_thread.xt_serno);
		    if (drdb->drdb_thread.xt_anchor)
			printf(", anchored");
		}
		next_drdb = drdb->drdb_next;
		free(drdb);
	    }

	    /*
	     * If active and there are any reply status bits left, find the
	     * associated peers and print them.
	     */
	    if (active && reply_status.handles_used) {
		printf("\n    Remaining replies:");
		for (handle_num = 0;
		     (handle_num <
		      CELL_TO_HANDLE(reply_status.handle_array_size));
		     handle_num++) {
		    if (igrp2_test_handle(ddb, &reply_status, handle_num)) {
			peer = igrp2_handle_to_peer(ddb, handle_num);
			if (peer) {
			    printf("\n         via %s, r, %s",
				   (*ddb->printaddr)(&peer->source),
				   peer->iidb->idb->namestring);
			} else {
			    printf("\n         via unallocated handle %d",
				   handle_num);
			}
			igrp2_clear_handle(ddb, &reply_status, handle_num);
		    }
		}
	    }
	    next = dndb->dndb_next;
	    free(dndb);
	}
    }
  punt:
    automore_disable();
    free(reply_status.handle_array);
}

/*
 * igrp2_find_iidb
 *
 * Find IGRP2 iidb structure for given ddb and idb.
 */

igrp2_idbtype *igrp2_find_iidb (ddbtype *ddb, idbtype *idb)
{
    
    igrp2_idblink *current_link;

    /* Walk the thread, looking for a DDB match. */

    current_link = idb->eigrp_info;

    while (current_link) {
	if (current_link->link_ddb == ddb)
	    break;
	current_link = current_link->link_next;
    }

    if (current_link) {			/* Found it! */
	return(current_link->link_iidb);
    } else {				/* Darn. */
	return(NULL);
    }
}

/*
 * igrp2_create_iidb
 *
 * Allocate and initialize an IIDB.
 *
 * Returns a pointer to the IIDB, or NULL if no memory.
 *
 * This routine does not link the IIDB into any data structures.  Note that
 * the nulliidb is never so linked.
 */
igrp2_idbtype *igrp2_create_iidb (ddbtype *ddb, idbtype *idb)
{
    igrp2_idbtype *iidb;

    /* Grab some memory. */

    iidb = malloc_named(sizeof(igrp2_idbtype), "EIGRP IIDB");
    if (!iidb) {
	return(NULL);
    }

    /* Initialize the queues. */

    queue_init(&iidb->xmitQ[UNRELIABLE_QUEUE], 0);
    queue_init(&iidb->xmitQ[RELIABLE_QUEUE], 0);
    queue_init(&iidb->pdm_queue, 0);

    /* Initialize random stuff. */

    iidb->idb = idb;
    iidb->hello_interval = igrp2_def_helloint(idb, ddb->linktype);
    iidb->holdtime = igrp2_def_holdint(idb, ddb->linktype);
    iidb->split_horizon = ddb->def_split_horizon;
    iidb->igrp2_bandwidth_percentage = IGRP2_DEFAULT_BANDWIDTH_PERCENT;
    iidb->holddown = EIGRP_HOLDDOWN_DEFAULT;

    /*
     * ISDN interfaces are marked as point-to-point, but act like multipoint,
     * so we need to treat them as multipoint (letting the driver do the
     * multicast replication).
     */
    iidb->use_multicast = (!if_is_nbma(idb, ddb->linktype) && !is_p2p(idb)) ||
	if_is_ddr(idb->hwptr);

    /* Initialize managed timers. */

    mgd_timer_init_parent(&iidb->iidb_peer_timer, &ddb->ddb_peer_timer);
    mgd_timer_init_parent(&iidb->iidb_holding_timers, &ddb->ddb_master_timer);
    mgd_timer_init_leaf(&iidb->iidb_send_timer, &ddb->ddb_intf_timer,
			IGRP2_IIDB_PACING_TIMER, iidb, FALSE);
    mgd_timer_init_leaf(&iidb->iidb_hello_timer, &ddb->ddb_hello_timer,
			IGRP2_IIDB_HELLO_TIMER, iidb, FALSE);
    mgd_timer_init_leaf(&iidb->iidb_flow_control_timer, &ddb->ddb_master_timer,
			IGRP2_FLOW_CONTROL_TIMER, iidb, FALSE);
    mgd_timer_init_leaf(&iidb->iidb_packetize_timer, &ddb->ddb_master_timer,
			IGRP2_PACKETIZE_TIMER, iidb, FALSE);
    mgd_timer_init_leaf(&iidb->iidb_acl_change_timer, &ddb->ddb_master_timer,
			IGRP2_IIDB_ACL_CHANGE_TIMER, iidb, FALSE);

    igrp2_set_pacing_intervals(ddb, iidb);

    return(iidb);
}

/*
 * igrp2_link_new_iidb
 *
 * Links a new IIDB into the appropriate data structures.
 */
static void igrp2_link_new_iidb (ddbtype *ddb, igrp2_idbtype *iidb)
{
    idbtype *idb;
    igrp2_idblink *idb_link;

    /* Link into the IDB. */

    idb = iidb->idb;
    idb_link = malloc(sizeof(igrp2_idblink));
    if (idb_link) {
	idb_link->link_next = idb->eigrp_info;
	idb->eigrp_info = idb_link;
	idb_link->link_ddb = ddb;
	idb_link->link_iidb = iidb;
    }
}


/*
 * igrp2_get_idb
 *
 * Get IGRP2 idb from idb queue. If it doesn't exist, it will create
 * one and put it on a temporary list.
 */

static igrp2_idbtype *igrp2_get_idb (ddbtype *ddb, idbtype *idb)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_find_iidb(ddb, idb);

    /*
     * iidb not created yet. Allocate and put on temporary
     * queue.
     */
    if (!iidb) {
	if (!(iidb = igrp2_find_temp_idb(ddb, idb))) {
	    iidb = igrp2_create_iidb(ddb, idb);
	    if (!iidb) {
		return(NULL);
	    }
	    enqueue(&ddb->temp_iidbQ, iidb);
	}
    }
    return(iidb);
}

/*
 * igrp2_find_temp_idb
 *
 * Find IGRP2 idb structure for given ddb on the temporary queue. This
 * queue is only used when an IGRP2 interface configuration parameter
 * is set but the interface is not yet configured to run IGRP2 over.
 * Once it is configured, the data structure is moved from the ddb->temp_iidbQ
 * to ddb->iidbQ.
 *
 * This function should be called by the protocol specific function that
 * NV writes configuration info for the interface.
 */

igrp2_idbtype *igrp2_find_temp_idb (ddbtype *ddb, idbtype *idb)
{
    
    igrp2_idbtype *iidb;

    for (iidb = ddb->temp_iidbQ.qhead; iidb; 
	 iidb = iidb->next) {
	if (iidb->idb == idb) break;
    }
    return(iidb);
}

/*
 * igrp2_flush_iidb_xmit_queues
 *
 * Flush the transmit queues on the IIDB, either because the interface
 * is being destroyed, or because the last peer has gone down.  Note
 * that DUAL will be called back as each packet is peeled off of the
 * reliable queue;  this will clean up the various DUAL transmit linkages.
 *
 * The last multicast flow control stuff is cleaned up as well.
 */
void igrp2_flush_iidb_xmit_queues (ddbtype *ddb, igrp2_idbtype *iidb)
{
    /* Flush each of 'em. */

    igrp2_flush_queue(ddb, iidb, &iidb->xmitQ[UNRELIABLE_QUEUE]);
    igrp2_flush_queue(ddb, iidb, &iidb->xmitQ[RELIABLE_QUEUE]);

    /* Flush the multicast flow control pointer. */

    igrp2_cleanup_multipak(ddb, iidb);
}

/*
 * igrp2_destroy_iidb
 *
 * Destroys an IIDB.
 *
 * This is called by the cleanup code to get rid of an IIDB when EIGRP
 * is deconfigured from an interface.
 *
 * The IIDB is mem_locked for every DRDB that points at it, so our freeing
 * it may not completely free it (a DRDB may still be active, for example).
 */
static void igrp2_destroy_iidb (ddbtype *ddb, igrp2_idbtype *iidb)
{
    igrp2_idblink *idb_link, **prev_link;
    idbtype *idb;

    /* Call the PDM to do any PDM-specific cleanup. */

    if (ddb->iidb_cleanup)
	(*ddb->iidb_cleanup)(ddb, iidb);

    /* Whimper if there are any peers there. */

    if (igrp2_peer_count(iidb)) {
	errmsg(&msgsym(PEERSEXIST, DUAL), igrp2_peer_count(iidb),
	       iidb->idb->namestring);
	return;
    }

    /* Turn off all timers on the interface. */
    
    mgd_timer_stop(&iidb->iidb_send_timer);
    mgd_timer_stop(&iidb->iidb_peer_timer);
    mgd_timer_stop(&iidb->iidb_holding_timers);
    mgd_timer_stop(&iidb->iidb_hello_timer);
    mgd_timer_stop(&iidb->iidb_flow_control_timer);
    mgd_timer_stop(&iidb->iidb_packetize_timer);
    mgd_timer_stop(&iidb->iidb_acl_change_timer);

    /* Flush the transmit queues. */

    igrp2_flush_iidb_xmit_queues(ddb, iidb);

    /* Delink it from the IDB. */

    idb = iidb->idb;
    idb_link = idb->eigrp_info;
    prev_link = &idb->eigrp_info;
    while (idb_link) {
	if (idb_link->link_ddb == ddb) {
	    *prev_link = idb_link->link_next; /* Delink */
	    free(idb_link);
	    break;
	}
	prev_link = &idb_link->link_next;
	idb_link = idb_link->link_next;
    }

    /* Toss it in the trash. */

    free(iidb->iidb_handles.handle_array);
    free(iidb);
}

/*
 * igrp2_process_starting
 *
 * Do common processing when an EIGRP process starts.
 */
void igrp2_process_starting (ddbtype *ddb)
{
    if (igrp2_process_count == 0) {	/* First one? */

	/* First the packet descriptors */
	
	igrp2_pakdesc_chunks = chunk_create(sizeof(igrp2_pakdesc),
					    IGRP2_MIN_PAKDESC,
					    CHUNK_FLAGS_DYNAMIC,
					    NULL, 0,
					    "EIGRP packet descriptors");
	
	/* Now the packet descriptor queue elements. */
	
	igrp2_pakdesc_qelm_chunks = chunk_create(sizeof(igrp2_pakdesc_qelm),
						 IGRP2_MIN_PAKDESC_QELM,
						 CHUNK_FLAGS_DYNAMIC,
						 NULL, 0,
						 "EIGRP queue elements");
	
	/* Now the anchor entries. */
	
	igrp2_anchor_chunks = chunk_create(sizeof(igrp2_xmit_anchor),
					   IGRP2_MIN_ANCHOR,
					   CHUNK_FLAGS_DYNAMIC, NULL, 0,
					   "EIGRP anchor entries");
	
	/* Now the dummy thread entries. */
	
	igrp2_dummy_chunks = chunk_create(sizeof(igrp2_xmit_thread),
					  IGRP2_MIN_THREAD_DUMMIES,
					  CHUNK_FLAGS_DYNAMIC,
					  NULL, 0,
					  "EIGRP dummy thread entries");

	/* Now the input packet headers. */

	igrp2_inpak_chunks = chunk_create(sizeof(igrp2_input_packet),
					  IGRP2_MIN_INPAK_CHUNKS,
					  CHUNK_FLAGS_DYNAMIC,
					  NULL, 0,
					  "EIGRP input packet headers");
    }
    igrp2_process_count++;
}

/*
 * igrp2_process_dying
 *
 * Do common cleanup when a process goes away.
 */
static void igrp2_process_dying (ddbtype *ddb)
{
    if (igrp2_process_count == 0)
	crashdump(0);			/* %% */

    igrp2_process_count--;		/* We're gone */

    if (igrp2_process_count == 0) {	/* Last one? */

	/* Get rid of the chunk memory allocated. */
	
	chunk_destroy(igrp2_pakdesc_chunks);
	igrp2_pakdesc_chunks = NULL;
	chunk_destroy(igrp2_pakdesc_qelm_chunks);
	igrp2_pakdesc_qelm_chunks = NULL;
	chunk_destroy(igrp2_anchor_chunks);
	igrp2_anchor_chunks = NULL;
	chunk_destroy(igrp2_dummy_chunks);
	igrp2_dummy_chunks = NULL;
	chunk_destroy(igrp2_inpak_chunks);
	igrp2_inpak_chunks = NULL;
    }
}
 
/*
 * igrp2_free_ddb
 *
 * A routing process is going away. Free resources associated with ddb.
 */
void igrp2_free_ddb (ddbtype *ddb)
{

    igrp2_idbtype *iidb, *next;
    dual_peertype *peer;
    igrp2_input_packet *inpak;

    ddb->ddb_going_down = TRUE;

    /*
     * Dequeue ddb from queue.
     */
    unqueue(&ddbQ, ddb);

    /*
     * Destroy any lingering peers.  Their death has been scheduled
     * (by virtue of the protocol effectively doing a "no network <foo>")
     * but the timers haven't had a chance to kick.  This will have
     * the side effect of finishing the destruction of a bunch of IIDBs as
     * well (those that have active peers).  IIDBs without any peers have
     * destruction timers running on them;  this is taken care of below.
     */
    while ((peer = ddb->peer_list)) {
	igrp2_neighbor_down(ddb, peer);	/* Deletes the peer from the DDB! */
    }

    /* Destroy any IIDBs still linked to the DDB.  They should all be gone. */

    while ((iidb = ddb->iidbQ.qhead)) {
	igrp2_deallocate_iidb(ddb, iidb);
    }
    while ((iidb = ddb->passive_iidbQ.qhead)) {
	igrp2_deallocate_iidb(ddb, iidb);
    }
    for (iidb = ddb->temp_iidbQ.qhead; iidb; iidb = next) {
	next = iidb->next;
	free(iidb);
    }

    /*
     * Wipeout topology table and associated data structures.
     */
    dual_cleanup(ddb);

    mgd_timer_stop(&ddb->ddb_master_timer);
    mgd_timer_stop(&ddb->ddb_hello_timer); /* It should already be stopped. */

   /* Free anything left in the input queue. */

    while ((inpak = process_dequeue(ddb->ddb_inputQ))) {
	datagram_done(inpak->packet);
	chunk_free(igrp2_inpak_chunks, inpak);
    }
    delete_watched_queue(&ddb->ddb_inputQ);

    /* Free the peer array table. */

    free(ddb->peer_array);

    /* Release the Null0 IIDB. */

    free(ddb->nulliidb);

    /* Toss the scratchpad area. */

    free(ddb->ddb_scratch);

    /* Free the handles. */

    free(ddb->ddb_handles.handle_array);

    /* Kill off the hello process. */

    process_kill(ddb->hello_pid);

    /* Free up some chunks. */

    chunk_destroy(ddb->extdata_chunk);
    chunk_destroy(ddb->workq_chunk);

    /* Possibly free up common resources. */

    igrp2_process_dying(ddb);

    /*
     * All allocated memory pointed to from ddb should be gone, get rid
     * of the whole block.
     */
    free(ddb);
}

/*
 * igrp2_allocate_iidb
 *
 * Allocate IIDB structure for ddb and enqueue it.  Pulls an IIDB from the
 * temp queue if appropriate.
 */
boolean igrp2_allocate_iidb (ddbtype *ddb, idbtype *idb, boolean passive_queue)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_find_iidb(ddb, idb);
    if (iidb)
	return (TRUE);
    
    /*
     * See if iidb is already on the temp queue. Move over
     * to active queue. Otherwise, allocate one for the active
     * queue.
     */
    iidb = igrp2_find_temp_idb(ddb, idb);
    if (iidb) {
	unqueue(&ddb->temp_iidbQ, iidb);
    } else {
	iidb = igrp2_create_iidb(ddb, idb);
	if (!iidb) {
	    return(FALSE);
	}
    }
    
    igrp2_link_new_iidb(ddb, iidb);
    if (passive_queue) {
	enqueue(&ddb->passive_iidbQ, iidb);
    } else {
	enqueue(&ddb->iidbQ, iidb);
    }
    iidb->passive_interface = passive_queue;
    
    /* Adjust the size of the scratchpad for DUAL. */
    
    dual_adjust_scratchpad(ddb);
    
    if (!passive_queue) {
	
	/* Set the hello timer to expire now. */
	
	mgd_timer_start(&iidb->iidb_hello_timer, 0);
    }
    if (ddb->set_mtu) {
	(*ddb->set_mtu)(iidb);
    }
    return (TRUE);
}

/*
 * igrp2_deallocate_iidb
 *
 * Get rid of an IIDB.
 *
 * This routine *must* be called on the process thread.
 */
void igrp2_deallocate_iidb (ddbtype *ddb, igrp2_idbtype *iidb)
{

    if (!iidb) /* Why are we here? */
	return;

    /* Take down any remaining neighbors. */

    igrp2_take_nbrs_down(ddb, iidb->idb, TRUE, "interface disabled");

    /* Dequeue the IIDB from the DDB. */
    
    if (iidb->passive_interface) {
	unqueue(&ddb->passive_iidbQ, iidb);
    } else {
	unqueue(&ddb->iidbQ, iidb);
    }
    
    /* Adjust the scratchpad area for DUAL. */
    
    dual_adjust_scratchpad(ddb);
    
    /* Remove the IIDB from the DDB quiescent list, if it's there. */
    
    dual_remove_iidb_from_quiescent_list(ddb, iidb);

    /* Clean up the topology table. */

    dual_ifdelete(ddb, iidb);
    
    /* Destroy the IIDB. */

    igrp2_destroy_iidb(ddb, iidb);
}

/*
 * igrp2_passive_interface
 *
 * Switch an interface from active to passive or vice versa.
 *
 * sense == TRUE if interface is becoming passive
 *
 * This *must* be called on the process thread.
 */
void igrp2_passive_interface (ddbtype *ddb, igrp2_idbtype *iidb, boolean sense)
{
    if (sense) {			/* Going passive */

	/* Take down any remaining neighbors. */

	igrp2_take_nbrs_down(ddb, iidb->idb, TRUE, "interface passive");

	/* Dequeue the IIDB from the DDB. */
    
	unqueue(&ddb->iidbQ, iidb);

	/* Kill the hello timer. */

	mgd_timer_stop(&iidb->iidb_hello_timer);

	/* Requeue the IIDB onto the passive queue. */

	enqueue(&ddb->passive_iidbQ, iidb);

    } else {				/* Going active */

	/* Requeue the IIDB appropriately. */

	unqueue(&ddb->passive_iidbQ, iidb);
	enqueue(&ddb->iidbQ, iidb);

	/* Start the hello timer. */

	mgd_timer_start(&iidb->iidb_hello_timer, 0);
    }
    iidb->passive_interface = sense;
}

/*
 * igrp2_take_all_nbrs_down
 * Take all neighbors down. This is called when global IGRP parameters are
 * changed.
 *
 * This routine can be safely called from other threads, since it schedules
 * the peers to be taken down;  the process does the real work.
 */
void igrp2_take_all_nbrs_down (ddbtype *ddb, char *reason)
{
    dual_peertype *peer, *next;

    for (peer = ddb->peer_list; peer; peer = next) {
	next = peer->next_peer;
	igrp2_take_peer_down(ddb, peer, FALSE, reason);
    }
}

/*
 * igrp2_take_nbrs_down
 *
 * Take all neighbors down for a given interface. This is called when
 * filtering, summarization, or split-horizon is configured on an interface.
 *
 * This routine can be safely called from other threads, since it schedules
 * the peers to be taken down;  the process does the real work.
 *
 * If "destroy_now" is TRUE, we do it all immediately.  This should only
 * be used when the process is being destroyed!
 */

void igrp2_take_nbrs_down (ddbtype *ddb, idbtype *idb, boolean destroy_now,
			   char *reason)
{

    dual_peertype *peer, *next;
    igrp2_idbtype *iidb;

    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;
    for (peer = ddb->peer_list; peer; peer = next) {
	next = peer->next_peer;
	if (peer->iidb->idb != idb)
	    continue;
	igrp2_take_peer_down(ddb, peer, destroy_now, reason);
    }
}

/*
 * igrp2_take_peer_down
 *
 * Take down a single peer.  
 *
 * This routine can be safely called from other threads, since it schedules
 * the peers to be taken down;  the process does the real work.
 *
 * However, if the "destroy_now" flag is set, the peer is destroyed
 * immediately.  This must only be called when the process is being destroyed.
 */
void igrp2_take_peer_down (ddbtype *ddb, dual_peertype *peer,
			   boolean destroy_now, char *reason)
{
    peer->going_down = TRUE;		/* Don't let a Hello sneak in there */
    peer->down_reason = reason;
    if (destroy_now) {
	igrp2_neighbor_down(ddb, peer);	/* Do it now. */
    } else {
	mgd_timer_start(&peer->holding_timer, 0); /* Defer it. */
    }
}

/*
 * igrp2_flush_peer_packets
 *
 * Flush all packets destined for a peer from the peer and IIDB queues.
 */
void igrp2_flush_peer_packets (ddbtype *ddb, dual_peertype *peer)
{
    xmit_queue_type qtype;
    igrp2_pakdesc_qelm *qelm;
    igrp2_pakdesc_qelm **prev_qelm;
    igrp2_pakdesc *pakdesc;
    queuetype *xmit_queue;
    igrp2_idbtype *iidb;
    
    /*
     * Deallocate resources on transmit queues.
     */
    for (qtype = 0; qtype < NUM_QUEUE_TYPES; qtype++) {
	while (peer->xmitQ[qtype].count) {
	    qelm = peer->xmitQ[qtype].qhead;
	    pakdesc = qelm->pakdesc;
	    if (pakdesc->sequenced) {
		IGRP2_XBUGINF(XDBPEERDOWN|XDBDETAIL,
			      "\n Freeing %d-%d, refcount %d from peer queue",
			      pakdesc->start_serno, pakdesc->end_serno,
			      pakdesc->refcount);
	    }
	    igrp2_dequeue_qelm(ddb, peer, qtype);
	}
    }

    /*
     * Walk the interface reliable transmit list and pull any packets
     * that are in flight there.  I hate type coercion, but we can't
     * automatically coerce from the void * below (because we are taking
     * a pointer to it, and void **'s don't get automatically assigned).
     */
    iidb = peer->iidb;
    xmit_queue = &iidb->xmitQ[RELIABLE_QUEUE];
    prev_qelm = (igrp2_pakdesc_qelm **) &xmit_queue->qhead;
    while ((qelm = *prev_qelm)) {
	pakdesc = qelm->pakdesc;
	if (pakdesc->peer == peer) {
	    unqueue(xmit_queue, qelm);
	    IGRP2_XBUGINF(XDBPEERDOWN|XDBDETAIL,
			  "\n  Freeing %d-%d, refcount %d from IIDB queue",
			  pakdesc->start_serno, pakdesc->end_serno,
			  pakdesc->refcount);
	    igrp2_free_qelm(ddb, qelm, iidb, peer);
	} else {
	    prev_qelm = &qelm->next;
	}
    }
}

/*
 * igrp2_destroy_peer
 *
 * IGRP2 neighbor has gone down, free resources.
 * NOTE:  This does NOT inform dual about the neighbor going away.  If this
 * is required, it is up to the caller to do so.  Alternatively, use
 * igrp2_neighbor_down() (from inside the router process) or
 * igrp2_take_nbr_down() (from anywhere).
 *
 * This should only be called at the very end of the circuitous neighbor down
 * process.
 */
void igrp2_destroy_peer (ddbtype *ddb, dual_peertype *peer)
{
    dual_peertype   **pptr, *prev;
    igrp2_idbtype *iidb;

    iidb = peer->iidb;

    if (ddb->peerstatehook != NULL) {

	/* Do PDM-specific cleanup */

	(*ddb->peerstatehook)(ddb, peer, FALSE);
    }

    /* Flush all packets for this guy. */

    igrp2_flush_peer_packets(ddb, peer);

    if (igrp2_nbrs_debug)
	buginf("\nEIGRP: Neighbor %s went down on %s", 
	       (*ddb->printaddr)(&peer->source), iidb->idb->namestring);

    /*
     * Delink and free peer.
     */
    pptr = &ddb->peercache[(*ddb->peerbucket)(&peer->source)];
    for ( ; *pptr; pptr = &(*pptr)->next) {
	if (*pptr == peer) {
	    *pptr = peer->next;
	    break;
	}
    }

    /* Take of single threaded list */

    if (ddb->peer_list == peer) {
	ddb->peer_list = peer->next_peer;
    } else {
	for (prev = ddb->peer_list; prev != NULL; prev = prev->next_peer) {
	    if (prev->next_peer == peer) {
		prev->next_peer = peer->next_peer;
		break;
	    }
	}
    }

    peer->next = NULL;
    peer->next_peer = NULL;
    peer->iidb = NULL;
    mgd_timer_stop(&peer->peer_send_timer);
    mgd_timer_stop(&peer->holding_timer);
    peer->peer_deleted = TRUE;		/* In case it's locked */

    if (igrp2_peer_count(iidb) == 0) {	/* Last one on the i/f */
	dual_last_peer_deleted(ddb, iidb);
    }
    iidb->total_srtt -= peer->srtt;
    free(peer);
}

/*
 * igrp2_send_hello
 *
 * Send (multicast) generic periodic hello packet.
 *
 * This is called from the Hello process, so we bypass all the fancy
 * queueing stuff and just blast it out.
 *
 * This is the only packet on which PAK_PRIORITY is set.
 */
static void igrp2_send_hello (ddbtype *ddb, igrp2_idbtype *iidb)
{
    uchar *tlv_ptr;
    igrp2type       *igrp2;
    int bytes, malloc_bytes;

    if ((*ddb->interface_up)(iidb->idb)) {
	bytes = malloc_bytes = IGRP2HELLOHDRBYTES;
	if (ddb->hellohook)
	    malloc_bytes += IGRP2HELLOHOOKBYTEKLUDGE;
	igrp2 = igrp2_getbuf(malloc_bytes);
	if (igrp2) {
	    tlv_ptr = igrp2_hello(igrp2, ddb->k1, ddb->k2, ddb->k3, ddb->k4,
				  ddb->k5, iidb->holdtime/ONESEC);
	    if (!ddb->hellohook ||
		((*ddb->hellohook)(ddb, igrp2, &bytes, iidb->idb, tlv_ptr))) {
		igrp2_send_hello_packet(ddb, igrp2, iidb, bytes, TRUE);
	    }
	    free(igrp2);
	}
    }

    /*
     * Kick the timer.
     */
    mgd_timer_start_jittered(&iidb->iidb_hello_timer, iidb->hello_interval,
			     IGRP2_JITTER_PCT);
}

/*
 * igrp2_set_hello_interval
 *
 * Add hello-interval to IGRP2 interface data structure.
 */

void igrp2_set_hello_interval (
    ddbtype *ddb,
    idbtype *idb,
    ulong hello_interval)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_get_idb(ddb, idb);
    if (!iidb) return;

    /*
     * Set the value.
     */
    iidb->hello_interval = hello_interval;
}

/*
 * igrp2_set_holdtime
 *
 * Add hold-time to IGRP2 interface data structure. If 0 is passed in, use
 * 3 times hello-interval.
 */

void igrp2_set_holdtime (ddbtype *ddb, idbtype *idb, ulong holdtime)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_get_idb(ddb, idb);
    if (!iidb)
	return;

    /*
     * Set the value.
     */
    iidb->holdtime = holdtime;
}

/*
 * igrp2_set_holddown
 *
 * Add hold-down to IGRP2 interface data structure. The default is 5 seconds.
 * 
 */

void igrp2_set_holddown (ddbtype *ddb, idbtype *idb, ulong holddown)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_get_idb(ddb, idb);
    if (!iidb)
	return;

    /*
     * Set the value.
     */
    iidb->holddown = holddown;
}

/*
 * igrp2_set_split_horizon
 *
 * Set split horizon flag in IGRP2 interface data structure.
 */

void igrp2_set_split_horizon (ddbtype *ddb, idbtype *idb, boolean split_enable)
{

    igrp2_idbtype *iidb;

    iidb = igrp2_get_idb(ddb, idb);
    if (!iidb)
	return;

    /*
     * Set the value.
     */
    if (iidb->split_horizon != split_enable) {
	iidb->split_horizon = split_enable;
	igrp2_take_nbrs_down(ddb, idb, FALSE, "split horizon changed");
    }
}

/*
 * igrp2_set_bandwidth_percent
 *
 * Set percentage of bandwidth.  If zero, set it to the default.
 */
void igrp2_set_bandwidth_percent (ddbtype *ddb, idbtype *idb, ulong percent)
{
    igrp2_idbtype *iidb;

    iidb = igrp2_get_idb(ddb, idb);
    if (!iidb)
	return;

    /*
     * Set the value.
     */
    if (percent == 0)
	percent = IGRP2_DEFAULT_BANDWIDTH_PERCENT;
    iidb->igrp2_bandwidth_percentage = percent;
    igrp2_set_pacing_intervals(ddb, iidb);
}

/*
 * igrp2_update_mib
 *
 * Update MIB counters based on opcode.
 */

void igrp2_update_mib (ddbtype *ddb, igrp2type *igrp2, boolean send_flag)
{

    ulong *ptr = NULL;
    
    switch (igrp2->opcode) {
    case IGRP_OPC_HELLO:
	ptr = (GETLONG(&igrp2->ack)) ? &ddb->mib.acks_sent :
	    &ddb->mib.hellos_sent;
	break;
    case IGRP_OPC_QUERY:
	ptr = &ddb->mib.queries_sent;
	break;
    case IGRP_OPC_REPLY:
	ptr = &ddb->mib.replies_sent;
	break;
    case IGRP_OPC_UPDATE:
	ptr = &ddb->mib.updates_sent;
	break;
    }

    if (ptr) {
	if (!send_flag) ptr++;
	(*ptr) += 1;
    }
}

/*
 * igrp2_show_traffic
 *
 * Show MIB variables from ddb.
 */

void igrp2_show_traffic (ddbtype *ddb)
{
    
    printf("\n  Hellos sent/received: %d/%d", ddb->mib.hellos_sent, 
	   ddb->mib.hellos_rcvd);
    printf("\n  Updates sent/received: %d/%d", ddb->mib.updates_sent, 
	   ddb->mib.updates_rcvd);
    printf("\n  Queries sent/received: %d/%d", ddb->mib.queries_sent, 
	   ddb->mib.queries_rcvd);
    printf("\n  Replies sent/received: %d/%d", ddb->mib.replies_sent, 
	   ddb->mib.replies_rcvd);
    printf("\n  Acks sent/received: %d/%d", ddb->mib.acks_sent, 
	   ddb->mib.acks_rcvd);
    printf("\n  Input queue high water mark %d, %d drops",
	   ddb->ddb_maxQdepth, ddb->ddb_Qdrops);
    printf("\n");
}

/*
 * igrp2_for_each_peer
 *
 * Execute the function given for each peer in the ddb.  Squirrel away
 * next pointers in case (*func)() deletes the peer entry.
 *
 * This routine may be called from threads other than EIGRP;  it may
 * suspend, and the routine called may also suspend.  Be careful!
 *
 * It is possible at additional peers will be added while this code is
 * suspended;  it is up to the caller of this routine to find those new
 * peers later.
 *
 * The routine called by this routine must respect the peer->peer_deleted
 * flag (by skipping the peer) and must check it if it suspends.
 */
void igrp2_for_each_peer (ddbtype *ddb, igrp2_peerfunction func, void *data)
{
    dual_peertype *peer, **peer_list;
    ulong peer_count, peer_number;

    /* Count the peers. */

    peer_count = 0;
    for (peer = ddb->peer_list; peer != NULL; peer = peer->next_peer) {
	peer_count++;
    }

    /* Save pointers to all of the peers;  lock each peer structure as well. */

    peer_list = malloc(peer_count * sizeof(dual_peertype *));
    if (peer_list == NULL)		/* Uhoh */
	return;

    peer_number = 0;
    for (peer = ddb->peer_list; peer != NULL; peer = peer->next_peer) {
	mem_lock(peer);
	peer_list[peer_number] = peer;
	peer_number++;
    }

    /* Now process each peer. */

    for (peer_number = 0; peer_number < peer_count; peer_number++) {
	peer = peer_list[peer_number];
	if (!peer->peer_deleted) {
	    (*func)(ddb, peer, data);
	    process_may_suspend();
	}
	free(peer);			/* Undo mem_lock */
    }

    /* Toss the list. */

    free(peer_list);
}

/*
 * igrp2_flow_control_timer_expiry
 *
 * Process the expiration of an interface multicast flow control timer.
 */
static void igrp2_flow_control_timer_expiry (ddbtype *ddb, mgd_timer *timer)
{
    igrp2_idbtype *iidb;

    iidb = mgd_timer_context(timer);

    /* Update the flow control state. */

    igrp2_update_mcast_flow_state(ddb, iidb);
}

/*
 * igrp2_process_managed_timers
 *
 * Called by the protocol-specific processes to service generic managed
 * timers.
 *
 * Returns TRUE if we bailed because we hit our limit, or FALSE if not.
 */
#define TIMER_LIMITER 20		/* No more than 20 timers per pass */
boolean igrp2_process_managed_timers (ddbtype *ddb)
{
    mgd_timer *timer;
    ushort timer_type;
    igrp2_idbtype *iidb;
    dual_peertype *peer;
    ulong timer_limiter;

    /* Loop until we've run out of timers or we decide to do something else. */

    timer_limiter = TIMER_LIMITER;
    while (TRUE) {
	process_may_suspend();

	/* Bail out of the loop if all the timers have been processed. */

	if (!mgd_timer_expired(&ddb->ddb_master_timer))
	    return(FALSE);

	/* Bail if we should do something else for awhile. */

	timer_limiter--;
	if (!timer_limiter)
	    return(TRUE);

	/*
	 * Give priority to the interface timers.  This is because the peer
	 * timers may have been set to expire at the same time as the interface
	 * timers, and they expect that the interface timers will have been
	 * stopped or reset to expire further in the future when they
	 * actually expire.  Since the managed timer system does not sort
	 * entries that have equal expiration time, we impose order ourselves
	 * by examining the interface subtrees first.
	 */
	timer = mgd_timer_first_expired(&ddb->ddb_intf_timer);
	if (!timer)			/* No interface timers */
	    timer = mgd_timer_first_expired(&ddb->ddb_peer_timer);

	if (timer) {
	    timer_type = mgd_timer_type(timer);
	    switch (timer_type) {

	      case IGRP2_IIDB_PACING_TIMER:

		/* Pacing timer expired. */

		igrp2_interface_pacing_timer_expiry(ddb, timer);
		break;

	      case IGRP2_PEER_SEND_TIMER:

		/* Peer transmit timer expired. */

		igrp2_peer_send_timer_expiry(ddb, timer);
		break;

	      default:
		mgd_timer_stop(timer);
		errmsg(&msgsym(UNKTIMER, DUAL), timer_type);
		break;
	    }
	} else {

	    /* Not an interface or peer timer.  Process it. */

	    timer = mgd_timer_first_expired(&ddb->ddb_master_timer);
	    if (timer) {
		timer_type = mgd_timer_type(timer);
		switch (timer_type) {

		  case IGRP2_PEER_HOLDING_TIMER:

		    /* Peer holding timer expired. */

		    peer = mgd_timer_context(timer);
		    mgd_timer_stop(timer);
		    if (igrp2_nbrs_debug)
			buginf("\nEIGRP: Holdtime expired");
		    dual_log_xport_all(ddb, DUALEV_HOLDTIMEEXP, &peer->source,
				       &peer->iidb->idb);
		    if (peer->going_down) { /* It's being torn down */
			igrp2_log_peer_change(ddb, peer, FALSE,
					      peer->down_reason);
		    } else { /* It really timed out */
			igrp2_log_peer_change(ddb, peer, FALSE,
					      "holding time expired");
		    }
		    igrp2_neighbor_down(ddb, peer);
		    break;

		  case IGRP2_FLOW_CONTROL_TIMER:
		    
		    /* Flow control timer expired. */
		    
		    igrp2_flow_control_timer_expiry(ddb, timer);
		    break;

		  case IGRP2_ACTIVE_TIMER:

		    /* SIA timer expired. */

		    dual_scan_active(ddb);
		    mgd_timer_start(timer, ONEMIN);
		    break;

		  case IGRP2_PACKETIZE_TIMER:

		    /* The hysteresis for packetizing just expired. */

		    mgd_timer_stop(timer);
		    iidb = mgd_timer_context(timer);
		    IGRP2_XBUGINF(XDBPACKETIZE|XDBDETAIL,
				  "\nPacketizing timer expired on %s",
				  iidb->idb->namestring);
		    dual_xmit_continue(ddb, iidb, NULL, NULL);
		    break;


		  case IGRP2_PDM_TIMER:

		    /* The PDM timer expired.  Call the PDM to handle it. */

		    (*ddb->pdm_timer_expiry)(ddb, timer);
		    break;


		  case IGRP2_IIDB_ACL_CHANGE_TIMER:

		    /*
		     * The ACL change timer for an interface expired.  Tear
		     * down the neighbors on that interface.
		     */
		    mgd_timer_stop(timer);
		    iidb = mgd_timer_context(timer);
		    igrp2_take_nbrs_down(ddb, iidb->idb, FALSE,
					 "route filter changed");
		    break;


		  case IGRP2_DDB_ACL_CHANGE_TIMER:

		    /*
		     * The ACL change timer for the process expired.  Tear
		     * down all neighbors.
		     */
		    mgd_timer_stop(timer);
		    igrp2_take_all_nbrs_down(ddb, "route filter changed");
		    break;


		  default:
		    mgd_timer_stop(timer);
		    errmsg(&msgsym(UNKTIMER, DUAL), timer_type);
		    break;
		}
	    }
	}
    }
}

/*
 * igrp2_default_timers
 * Returns true if iidb has default hellotime and hold interval
 */

boolean igrp2_default_timers (igrp2_idbtype *iidb)
{

    if ((iidb->hello_interval == IGRP2_HELLOTIME || 
	  iidb->hello_interval == IGRP2_HELLOTIME_NBMA) && 
	(iidb->holdtime == IGRP2_HOLDTIME || 
	iidb->holdtime == IGRP2_HOLDTIME_NBMA))
	return(TRUE);

    return(FALSE);
}

/*
 * igrp2_delay_changed
 *
 * Callback when the interface delay changes.  Update the metric if there
 * is a connected route on this interface.
 */
static void igrp2_delay_changed (idbtype *idb) {
    ddbtype *ddb;
    igrp2_idbtype *iidb;

    /* Walk the DDB queue and update this interface on every DDB. */

    for (ddb = ddbQ.qhead; ddb; ddb = ddb->next) {
	iidb = igrp2_find_iidb(ddb, idb);
	if (iidb) {

	    /* Update the interface route. */

	    (*ddb->interface_change)(ddb, iidb);

	    /* Zap the neighbors so that the change takes effect. */

	    igrp2_take_nbrs_down(ddb, iidb->idb, FALSE,
				 "interface delay changed");
	}
    }
}

/*
 * igrp2_bandwidth_changed
 *
 * Callback when the interface bandwidth changes.  Recalculate the pacing
 * intervals.
 */
static void igrp2_bandwidth_changed (idbtype *idb)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;

    /* Walk the DDB queue and update this interface on every DDB. */

    for (ddb = ddbQ.qhead; ddb; ddb = ddb->next) {
	iidb = igrp2_find_iidb(ddb, idb);
	if (iidb) {

	    /* Update the interface route. */

	    (*ddb->interface_change)(ddb, iidb);

	    /* Update the pacing interval. */

	    igrp2_set_pacing_intervals(ddb, iidb);

	    /* Zap the neighbors so that the change takes effect. */

	    igrp2_take_nbrs_down(ddb, iidb->idb, FALSE,
				 "interface bandwidth changed");

	    /*
	     * Change the hold time and hello interval to the possibly new
	     * defaults (we vary by bandwidth) if they were previously
	     * defaulted.
	     */
	    if (igrp2_default_timers(iidb)) {
		iidb->hello_interval = igrp2_def_helloint(idb, ddb->linktype);
		iidb->holdtime = igrp2_def_holdint(idb, ddb->linktype);
	    }
	}
    }
}

/*
 * igrp2_probe_peer
 *
 * Start/stop the probe on a peer.
 */
void igrp2_probe_peer (ddbtype *ddb, dual_peertype *peer, boolean stop_now)
{
    /* Stop it if we're asked. */

    if (stop_now) {
	peer->send_probe_sequence = 0;
    } else {				/* Start now. */
	peer->send_probe_sequence = 1;
	igrp2_kick_peer(peer);		/* Kick the peer */
    }
}

/*
 * igrp2_probe_interface
 *
 * Start/stop the probe on an interface.
 */
void igrp2_probe_interface (ddbtype *ddb, igrp2_idbtype *iidb,
			    boolean stop_now)
{
    /* Stop it if we're asked. */

    if (stop_now) {
	iidb->send_probe_sequence = 0;
    } else {				/* Start now. */
	iidb->send_probe_sequence = 1;
	igrp2_kick_interface(ddb, iidb); /* Kick the interface */
    }
}

/*
 * igrp2_start_acl_timer
 *
 * (re)start the ACL change timer.  If the timer is already running, it is
 * restarted.  This means that as long as the user enters at least one
 * access list change every 10 seconds, the neighbors won't be cleared
 * until they're all done.
 */
void igrp2_start_acl_timer (ddbtype *ddb, idbtype *idb)
{
    igrp2_idbtype *iidb;

    iidb = NULL;
    if (idb) {
	iidb = igrp2_find_iidb(ddb, idb);
	if (!iidb)
	    return;
	mgd_timer_start(&iidb->iidb_acl_change_timer, IGRP2_ACL_CHANGE_DELAY);
    } else {
	mgd_timer_start(&ddb->ddb_acl_change_timer, IGRP2_ACL_CHANGE_DELAY);
    }
}

/*
 * igrp2_hello_process
 *
 * Process for handling EIGRP adjacency management
 */
#define IGRP2_HELLO_PACKET_LIMIT 40	/* Max number of packets per pass */
static process igrp2_hello_process (void)
{
    paktype *pak;
    igrp2addrtype source;
    igrp2_input_packet *inpak;
    dual_peertype *peer;
    mgd_timer *timer;
    igrp2_idbtype *iidb;
    ulong packet_limit;
    ddbtype *ddb;
    ulong major, minor;

    if (!process_get_arg_ptr((void **) &ddb))
	process_kill(THIS_PROCESS);

    /* Wait for the router process to take off. */

    process_watch_boolean(ddb->run_hello_process, ENABLE, ONE_SHOT);
    process_wait_for_event();
    delete_watched_boolean(&ddb->run_hello_process);

    /* Now set up our event predicates. */

    if (ddb->enable_process_wait)
	(*ddb->enable_process_wait)(ddb);
    process_watch_mgd_timer(&ddb->ddb_hello_timer, ENABLE);

    /* Top o' the loop to you. */

    while (TRUE) {
	process_wait_for_event();

	/* Process the events. */

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		
		/* Process managed timers. */
		
		while (TRUE) {
		    timer = mgd_timer_first_expired(&ddb->ddb_hello_timer);
		    if (!timer)
			break;
		    switch (mgd_timer_type(timer)) {
			
		      case IGRP2_IIDB_HELLO_TIMER:
			
			/*
			 * Peer hello timer expired.  The send routine resets
			 * the timer.
			 */
			iidb = mgd_timer_context(timer);
			igrp2_send_hello(ddb, iidb);
			
			break;
			
		      default:
			mgd_timer_stop(timer);
			break;
		    }
		}
		break;

	      case QUEUE_EVENT:
	      case DIRECT_EVENT:

		/*
		 * Process incoming packets next.  Any packet at all is
		 * sufficient to keep an existing adjacency alive.  We
		 * then pass all packets on to the router process for
		 * further handling.
		 */
		packet_limit = IGRP2_HELLO_PACKET_LIMIT;
		while (TRUE) {
		    
		    /* Bail if we've done enough for one pass. */
		    
		    if (packet_limit-- == 0)
			break;
		    
		    /*
		     * Grab the next packet.  Bail if there's nothing to be
		     * had.
		     */
		    pak = (*ddb->input_packet)(ddb, &source);
		    if (!pak)
			break;
		    
		    /* Got a packet.  Look for a peer. */
		    
		    peer = igrp2_find_peer(ddb, &source, pak->if_input);
		    
		    if (peer) {
			
			/* If the peer is going down, burn the packet. */
			
			if (peer->going_down) {
			    datagram_done(pak);
			    continue;
			}
			
			/* Push the holding time back up. */
			
			mgd_timer_start(&peer->holding_timer,
					peer->last_holding_time);
		    }
		    
		    /*
		     * Now queue the packet for the router process.  We first
		     * credit the input interface for the packet, but since we
		     * do so we have to enforce the maximum queue depth on the
		     * DDB queue, or we could run the box out of buffers.  By
		     * using this approach, we will keep adjacencies alive,
		     * even if we are dropping packets.
		     */
		    inpak = chunk_malloc(igrp2_inpak_chunks);
		    if (inpak) {
			inpak->packet = pak;
			inpak->if_input = pak->if_input;
			clear_if_input(pak);
			if (!process_is_queue_full(ddb->ddb_inputQ)) {
			    process_enqueue(ddb->ddb_inputQ, inpak);
			    
			    /* Track the high water mark. */
			    
			    if (process_queue_size(ddb->ddb_inputQ) >
				ddb->ddb_maxQdepth) {
				ddb->ddb_maxQdepth =
				    process_queue_size(ddb->ddb_inputQ);
			    }
			} else {
			    datagram_done(pak);
			    chunk_free(igrp2_inpak_chunks, inpak);
			    ddb->ddb_Qdrops++;	/* Count the drop. */
			}
		    } else {			/* Couldn't malloc inpak */
			datagram_done(pak);
		    }
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
 * igrp2_init_hello_process
 *
 * Fire up the Hello process for this router process.
 *
 * Returns TRUE if successful.
 */
boolean igrp2_init_hello_process (ddbtype *ddb, char *name)
{
    ddb->hello_pid = process_create(igrp2_hello_process, name, NORMAL_STACK,
				    PRIO_NORMAL);
    if (ddb->hello_pid == NO_PROCESS) {
	return(FALSE);
    } else {
	ddb->run_hello_process = create_watched_boolean(name, 0);
	if (!ddb->run_hello_process) {
	    process_kill(ddb->hello_pid);
	    return(FALSE);
	}
	return(process_set_arg_ptr(ddb->hello_pid, ddb));
    }
}

/*
 * IGRP2 initialization
 */
static void igrp2_init (subsystype* subsys)
{
    igrp2_debug_init();
    igrp2_parser_init();
    reg_add_bandwidth_changed(igrp2_bandwidth_changed,
			      "igrp2_bandwidth_changed");
    reg_add_delay_changed(igrp2_delay_changed, "igrp2_delay_changed");
    reg_add_igrp2_cdelay(igrp2_cdelay, "igrp2_cdelay");
}

/*
 * EIGRP subsystem header
 */
#define EIGRP_MAJVERSION 1
#define EIGRP_MINVERSION 0
#define EIGRP_EDITVERSION 1

SUBSYS_HEADER (eigrp, EIGRP_MAJVERSION, EIGRP_MINVERSION, EIGRP_EDITVERSION,
               igrp2_init, SUBSYS_CLASS_PROTOCOL,
               subsys_seq_iphost,
               NULL);
