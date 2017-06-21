/* $Id: dual.c,v 3.8.12.17 1996/09/10 18:09:01 sluong Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/dual.c,v $
 *------------------------------------------------------------------
 * DUAL - Diffusing update algorithm.  Core routines.
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dual.c,v $
 * Revision 3.8.12.17  1996/09/10  18:09:01  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.8.12.16  1996/09/05  23:07:35  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.8.12.15  1996/08/28  12:52:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.14  1996/08/27  20:23:24  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.8.12.13  1996/06/28  02:40:20  dkatz
 * CSCdi61491:  EIGRP may lose routes
 * Branch: California_branch
 * Only ignore the routes we are supposed to ignore.
 *
 * Revision 3.8.12.12  1996/06/28  01:20:39  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.8.12.11  1996/06/27  22:42:52  mikel
 * CSCdi61259:  EIGRP: crash in sh ip eigrp events when debug is on
 * Branch: California_branch
 * avoid NULL dereference in eigrp events log
 *
 * Revision 3.8.12.10  1996/06/18  16:46:22  dkatz
 * CSCdi60515:  EIGRP summaries stay in topo table
 * Branch: California_branch
 * Rework the decision process for doing topo table searches for
 * the min metric under a summary.  Get rid of the metric count
 * field.  Add a hook to suppress multiple enqueued requests for
 * a min metric search.
 *
 * Revision 3.8.12.9  1996/06/18  16:41:47  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.8.12.8  1996/06/02  18:27:35  dkatz
 * CSCdi59276:  Candidate Default can disappear from EIGRP
 * Branch: California_branch
 * If sinister forces try to make us send an update while active, defer
 * rather than stomping the topo table entry.
 *
 * Revision 3.8.12.7  1996/05/09  14:31:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.12.6  1996/05/07  17:26:32  dkatz
 * CSCdi55252:  EIGRP SAP queue is not emptied when last peer is gone
 * Branch: California_branch
 * Flush the SAP queue when the last peer goes down.
 *
 * Revision 3.8.12.5  1996/04/30  16:36:55  albright
 * CSCdi54781:  eigrp routers are SIA after static routes removed
 * Branch: California_branch
 *
 * Revision 3.8.12.4.10.1  1996/04/28  04:24:52  cakyol
 * use %CC instead of %C as printf format sequence.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.12.4  1996/04/02  07:32:32  dkatz
 * CSCdi52398:  EIGRP SAP defers too much to DUAL
 * Branch: California_branch
 * Interleave SAP and DUAL packets.  While we're in there, default SAP
 * split-horizon off again;  it's still messed up.
 *
 * Revision 3.8.12.3  1996/04/02  07:29:16  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.8.12.2  1996/04/02  05:36:50  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.8.12.1  1996/03/18  20:27:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  09:49:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:02:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  08:14:16  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1996/02/01  06:05:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:28:46  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/22  06:14:03  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/07  22:29:35  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  17:32:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:06:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/10/11  15:45:49  mikel
 * CSCdi41946:  EIGRP: NULL dereference due to missing protocol vector
 * (regress)
 * missing vector for IPX and Appletalk when  calling ddb->compare_externa
 *
 * Revision 2.9  1995/10/05  02:09:04  mikel
 * CSCdi40200:  EIGRP: incorrect redistributed routes in topology table
 * check to see an existing drdb already exists in the topology table.  If
 * so,  don't add overwrite the existing drdb.  This also eliminates
 * continuous updates seem when mutually redistributing
 * between different EIGRP processes
 *
 * Revision 2.8  1995/09/11  23:50:15  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.7  1995/08/04 19:42:32  albright
 * CSCdi35324:  IP EIGRP sending lots of UPDATE if serials connects to
 * itself
 *
 * Revision 2.6  1995/07/13  14:28:24  albright
 * CSCdi34641:  Crash in dual_getnext after adding passive interface to
 * EIGRP
 *
 * Revision 2.5  1995/07/07  05:26:29  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.4  1995/06/28 03:21:01  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.3  1995/06/19 01:07:24  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:06:09  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:05:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Topology Table/Transport Interface
 *
 * The key to the interface between DUAL and the transport is the organization
 * of the topology table.
 *
 * The DNDBs in the topology table are threaded chronologically by the time
 * of the last change;  as a DNDB is changed, it is moved to the end of the
 * thread.  Each DNDB receives a new Serial Number as it is changed;  the
 * serial numbers are monotonically increasing within a circular number
 * space (so we have to be careful about arithmetic comparison!)  Each
 * DNDB has embedded within it a Thread structure, which contains the
 * thread pointers and the serial number, among other things.
 *
 * Each interface has an independent state machine that crawls along this
 * chain, packetizing the contents of the changed DNDBs.  A packet is
 * canonically represented as a serial number range.  When an interface
 * becomes flow-ready, the state machine figures out what range of serial
 * numbers should go into the next packet, and calls the transport to enqueue
 * the packet descriptor.  When the transport is actually going to transmit
 * (it may be many milliseconds due to the vagaries of circuit pacing)
 * or retransmit (if a packet is lost), the transport calls back to actually
 * construct the packet from the contents of the range of DNDBs.  This
 * avoids sending information that has been changed, since a changed DNDB
 * will have been assigned a new serial number and shifted to the end of the
 * thread.
 *
 * For efficiency, pointers into the DNDB thread are kept in order to keep
 * track of the position of the state machines in the thread.  This has
 * obvious implications--a DNDB which is pointed to may need to be moved
 * and relinked, and it may be pointed to by many things.  A DNDB which
 * is pointed to is considered to be "anchored".  In fact, it's not really
 * the DNDB that's anchored, but the serial number (and the position in the
 * thread).  This is implemented with an auxiliary structure, known as an
 * "anchor entry."  This is a simple structure that maintains a count of
 * the number of things pointing at the entry (so we know when it can be
 * freed) and a pointer to the thread structure embedded in the DNDB behind
 * the anchor.  It is a separate structure in order to maintain a level of
 * indirection between the DNDB and the state machines.  This comes into play
 * if an anchored DNDB needs to change (and switch positions in the thread).
 * When this happens, a dummy thread entry is placed in the thread where
 * the DNDB used to be, at which point the DNDB is free to move.  The packet
 * builder will of course skip the dummy entry.
 *
 * The basic flow (starting at a quiescent state) is as follows.  In the
 * steady state, each interface state machine has transmitted everything, so
 * nothing is anchored, and the state machines all point off the end of the
 * thread.  When a change occurs, the changed DNDB is relinked to the end
 * of the thread.  Each quiescent interface is then anchored at the new
 * DNDB (a linked list of quiescent interfaces is kept on the DDB so that
 * this may be done efficiently).  Any interface that is already anchored
 * is ignored;  this means that the interface state machine is somewhere
 * up the thread working its way down, and will eventually get to our new
 * DNDB.  At this point a timer is started (the timer allows a bunch of
 * changed DNDBs to be rethreaded before we start packetizing;  the
 * additional DNDBs are moved to the end of the thread but the anchor points
 * stay where they are.)
 *
 * When the timer expires, the packetizer is run for the interface, which
 * examines the thread from the transmit anchor point, and determines how much
 * of the thread can be put into the next packet.  The transport is then called
 * to create a packet descriptor, and the serial number range is noted.  The
 * packet descriptor also contains an anchor for the first entry in the packet.
 * The transmit anchor is then advanced to point at the next entry after the
 * last one packetized (or is NULL if we hit the end of the chain).
 *
 * Eventually, the interface becomes flow control ready and the transport
 * calls back with this fact.  The interface state machine then packetizes
 * from the current position of the transmit anchor and advances it once
 * again.  If we were positioned at the end of the chain, the interface goes
 * idle.
 *
 * When a packet is acknowledged, the anchor in the packet descriptor is
 * used to find the place in the thread to clean up.  When the last peer
 * acknowledges a packet, the reference count in each DNDB (which tracks how
 * many interfaces still need to transmit the entry) is decremented, and if
 * that count goes to zero, the DNDB may be deleted (if it was going away).
 *
 * Note that all DNDBs are always threaded, even when everything is idle.
 * This makes it easy to bring up new peers, since the entire database must
 * be dumped to the new peer.  When a new peer comes up, the packetizer is
 * called repeatedly to run down the length of the chain up to and including
 * the last serial number in the interface state machine's current packet.
 * This is done because of transport sequencing considerations;  any
 * outstanding multicast packet will be ignored by the new peer, but the next
 * multicast must be sequenced *after* the last unicast update sent in the
 * peer bringup process (because the peer must be synchronized to the rest of
 * the peers on the interface).
 *
 * So the peer bringup process runs more or less as follows.  When the
 * new adjacency is recognized, the entire thread is packetized (up through
 * the current position of the interface transmit anchor) and the final
 * serial number is noted.  When a packet containing the final serial number
 * is acknowledged by the peer, the peer has caught up to the interface state
 * machine.
 *
 * Replies are handled a little differently than updates and queries, since
 * they are unicast and operate at a per-peer granularity, rather than a
 * per-interface granularity.  Replies are handled by threading DRDBs rather
 * than DNDBs.  The DRDBs are threaded onto the same thread as the DNDBs
 * used for Updates and Queries in order to maintain temporal ordering
 * between packet types for the same destination.  As replies are acknowledged,
 * the DRDBs are delinked from the thread (since they are not sent during
 * peer bringup).
 */
#include "master.h"
#include <string.h>
#include "ttysrv.h"
#include "mgd_timers.h"
#include "../os/chunk.h"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "dual.h"
#include "route.h"
#include "logger.h"
#include "../os/free.h"
#include "igrp2-inlines.h"
#include "igrp2_debug_flags.h"
#include "parser_defs_igrp2.h"
#include "exception.h"
#include "../os/clock.h"
#include "../os/clock_internal.h"
#include "msg_igrp2.c"			/* Not a typo, see logger.h */
#include "config.h"

/*
 * Forward definitions
 */
static char *dual_routetypestring(igrp2origintype);
static uchar *dual_show_event_param(ddbtype *, dual_etype_enum, uchar *,
			     uchar *, int *);

/*
 * Global storage
 */
queuetype ddbQ;


/*
 * dual_serno_earlier
 *
 * Returns TRUE if serial number A is before serial number B.
 *
 * This is just a simple unsigned compare.
 */
static inline boolean dual_serno_earlier (ulong serno_a, ulong serno_b)
{
    return(serno_a < serno_b);
}

/*
 * dual_serno_later
 *
 * Returns TRUE if serial number A is after serial number B.
 *
 * This is just a simple unsigned compare.
 */
static inline boolean dual_serno_later (ulong serno_a, ulong serno_b)
{
    return(serno_a > serno_b);
}

/*
 * dual_thread_to_drdb
 *
 * Returns a pointer to a DRDB, given a pointer to the embedded thread
 * entry.
 *
 * The caller is responsible for making sure that the thread entry is in
 * fact embedded in a DRDB.
 *
 * All this optimizes down to a simple indexed reference.
 */
static inline dual_rdbtype *dual_thread_to_drdb (igrp2_xmit_thread *thread)
{
    uchar *ptr;
    ulong offset;
    dual_rdbtype *drdb;

    offset = offsetof(dual_rdbtype, drdb_thread);
    ptr = ((uchar *) thread) - offset;
    drdb = (dual_rdbtype *) ptr;
    return(drdb);
}

/*
 * dual_thread_to_dndb
 *
 * Returns a pointer to a DNDB, given a pointer to the embedded thread
 * entry.
 *
 * The caller is responsible for making sure that the thread entry is in
 * fact embedded in a DNDB.
 *
 * All this optimizes down to a simple indexed reference.
 */
static inline dual_ndbtype *dual_thread_to_dndb (igrp2_xmit_thread *thread)
{
    uchar *ptr;
    ulong offset;
    dual_ndbtype *dndb;

    offset = offsetof(dual_ndbtype, dndb_xmit_thread);
    ptr = ((uchar *) thread) - offset;
    dndb = (dual_ndbtype *) ptr;
    return(dndb);
}

/*
 * dual_xmit_threaded
 *
 * Returns TRUE if the entry is on a thread, FALSE if not.
 */
static inline boolean dual_xmit_threaded (igrp2_xmit_thread *entry)
{
    return(entry->xmit_prev != NULL);
}

/*
 * dual_iidb_on_quiescent_list
 *
 * Returns TRUE if the IIDB is on the quiescent list.
 */
static inline boolean dual_iidb_on_quiescent_list (igrp2_idbtype *iidb)
{
    return (iidb->prev_quiescent != NULL);
}

/*
 * dual_remove_iidb_from_quiescent_list
 *
 * Remove the IIDB from the quiescent list if it's there.
 */
void dual_remove_iidb_from_quiescent_list (ddbtype *ddb, igrp2_idbtype *iidb)
{
    if (dual_iidb_on_quiescent_list(iidb)) {
	*iidb->prev_quiescent = iidb->next_quiescent; /* Delink forward */
	if (iidb->next_quiescent) {
	    iidb->next_quiescent->prev_quiescent = iidb->prev_quiescent;
	}
	iidb->next_quiescent = NULL;
	iidb->prev_quiescent = NULL;
    }
}

/*
 * dual_add_iidb_to_quiescent_list
 *
 * Add the IIDB to the quiescent list if it's not already there.
 */
static void dual_add_iidb_to_quiescent_list (ddbtype *ddb, igrp2_idbtype *iidb)
{
    if (iidb->iidb_xmit_anchor || dual_iidb_on_quiescent_list(iidb)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    iidb->next_quiescent = ddb->quiescent_iidbs;
    iidb->prev_quiescent = &ddb->quiescent_iidbs;
    ddb->quiescent_iidbs = iidb;
    if (iidb->next_quiescent)
	iidb->next_quiescent->prev_quiescent = &iidb->next_quiescent;
}

/*
 * dual_xmit_unthread_unanchored
 *
 * Delink a transmit thread entry that is guaranteed to be unanchored.
 *
 * Standard dual-linkage dance.
 */
static void dual_xmit_unthread_unanchored (ddbtype *ddb,
					   igrp2_xmit_thread *entry)
{
    /* Bail if not threaded. */

    if (!dual_xmit_threaded(entry))
	return;

    /* Die if anchored.  This is very bad. */

    if (entry->xt_anchor) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /*
     * This should twist your mind.  ;-)  The tail pointer (a ** pointer)
     * must always be non-null, as it points to either the head pointer,
     * or the next pointer in the last entry.  The contents of where the
     * tail pointer points to must always be null, since it will point
     * to the next pointer of the last guy on the list.
     */
    if (!ddb->ddb_thread_tail || *ddb->ddb_thread_tail)	{ /* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    if (ddb->ddb_thread_tail == &entry->xmit_next) { /* We're on the end */
	ddb->ddb_thread_tail = entry->xmit_prev;
    } else {			/* Tail doesn't match us */
	if (!entry->xmit_next) {	/* We better not be last */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
    }

    /* Adjust the pointers of our neighbors. */

    *entry->xmit_prev = entry->xmit_next; /* Prev->next = next */
    if (entry->xmit_next)
	entry->xmit_next->xmit_prev = entry->xmit_prev;	/* next->prev = prev */
    entry->xmit_prev = NULL;
    entry->xmit_next = NULL;
}

/*
 * dual_next_serno
 * 
 * Get next serial number.
 *
 * Serial number 0 is never used (we special-case this value elsewhere).
 *
 * The serial number space cannot wrap at this time;  in order to make
 * this work we have to renumber all of the thread entries, which is
 * a significant pain.
 *
 * We observe that if there are ten route changes per second, constantly,
 * it will take over 23 years before the serial number space wraps.  So
 * we punt.
 */
static ulong dual_next_serno (ddbtype *ddb)
{
    ulong retval;
    retval = ddb->ddb_next_serno;
#ifdef notdef
    ddb->ddb_next_serno = (ddb->ddb_next_serno + 1) %
	DUAL_SERNO_MODULUS; /* %%% */
#endif
    ddb->ddb_next_serno++;
    if (!ddb->ddb_next_serno) {
	crashdump(0);		/* The router won't stay up this long */
	ddb->ddb_next_serno++;
    }
    return(retval);
}

/*
 * dual_anchor_entry
 *
 * Anchors a thread entry.  Returns a pointer to the anchor entry.
 * If the entry is unanchored, an anchor entry will be created.
 */
static igrp2_xmit_anchor *dual_anchor_entry (ddbtype *ddb,
					     igrp2_xmit_thread *entry)
{
    igrp2_xmit_anchor *anchor;

    anchor = entry->xt_anchor;
    if (!anchor) {			/* No anchor there */
	anchor = chunk_malloc(igrp2_anchor_chunks);
	if (!anchor)
	    return(NULL);		/* Uhoh */
	anchor->xa_thread = entry;	/* Link 'em. */
	entry->xt_anchor = anchor;
    }

    /* Bump the ref count. */

    anchor->xa_anchor_count++;
    return(anchor);
}

/*
 * dual_unanchor_entry
 *
 * Releases an anchor.  If this is the last anchoring, we free the
 * anchor structure.  If we're anchoring a dummy entry, the dummy
 * is delinked and freed.
 */
void dual_unanchor_entry (ddbtype *ddb, igrp2_xmit_thread *entry)
{
    igrp2_xmit_anchor *anchor;

    anchor = entry->xt_anchor;
    if (!anchor)			/* We shouldn't even be here. */
	return;

    /* Drop the refcount. */

    anchor->xa_anchor_count--;
    if (anchor->xa_anchor_count < 0) {	/* Bad news */
	errmsg(&msgsym(ANCHORCOUNT, DUAL));
	anchor->xa_anchor_count = 0;
    }

    /* If it's the last reference, free the entry. */

    if (anchor->xa_anchor_count == 0) {
	entry->xt_anchor = NULL;
	anchor->xa_thread = NULL;	/* Keep things tidy. */
	chunk_free(igrp2_anchor_chunks, anchor);
	anchor = NULL;
    }

    /* If we freed the anchor and this is a dummy, free it. */

    if (anchor == NULL && entry->xt_dummy) {
	dual_xmit_unthread_unanchored(ddb, entry);
	chunk_free(igrp2_dummy_chunks, entry);
    }
}

/*
 * dual_xmit_unthread
 *
 * Unthreads an entry from the DDB thread.  If the entry is anchored, it
 * is replaced by a dummy entry.
 */
static void dual_xmit_unthread (ddbtype *ddb, igrp2_xmit_thread *entry)
{
    igrp2_xmit_anchor *anchor;
    igrp2_xmit_thread *thread_dummy;

    if (!dual_xmit_threaded(entry))
	return;				/* Nothing to do. */

    anchor = entry->xt_anchor;
    if (!anchor) {			/* Simple case, unanchored. */
	dual_xmit_unthread_unanchored(ddb, entry);
	return;
    }

    /* Allocate the dummy entry. */

    thread_dummy = chunk_malloc(igrp2_dummy_chunks);
    if (!thread_dummy) {		/* Sigh. */
	dual_xmit_unthread_unanchored(ddb, entry);
	return;
    }

    /* Copy the real entry into the dummy, including all pointers. */

    *thread_dummy = *entry;

    /* Mark the dummy as such.  Link the dummy and the anchor together. */

    thread_dummy->xt_dummy = TRUE;
    anchor->xa_thread = thread_dummy;

    /* Now clean up the original entry. */

    entry->xt_anchor = NULL;	/* It's not anchored any longer. */
    dual_xmit_unthread_unanchored(ddb, entry);

    /* Now link the neighbors to the dummy (the other way is already done). */
    
    *thread_dummy->xmit_prev = thread_dummy;
    if (thread_dummy->xmit_next) {
	thread_dummy->xmit_next->xmit_prev = &thread_dummy->xmit_next;
    } else {
	ddb->ddb_thread_tail = &thread_dummy->xmit_next;
    }
}


/*
 * dual_freedata
 * Check to see if the PDM data block needs to be freed
 */
inline static void dual_freedata (ddbtype *ddb, void **data)
{
    if (*data != NULL) {
	chunk_free(ddb->extdata_chunk, *data);
	*data = NULL;
    }
}

/*
 * dual_rdbdelete
 * Remove an rdb from the topology table.
 */
static const char dual_removerdb[] = "\nDUAL: Removing dest %s, nexthop %s";
static void dual_rdbdelete (ddbtype *ddb, dual_ndbtype *dndb,
			    dual_rdbtype *drdb)
{
    /* Freak if this DRDB is threaded to a peer. */

    if (dual_xmit_threaded(&drdb->drdb_thread)) { /* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    IGRP2_FABUGINF(&dndb->dndb_dest)(dual_removerdb,
				     (*ddb->printnet)(&dndb->dndb_dest),
		  (*ddb->printaddr)(&drdb->drdb_nexthop));
    DUAL_LOG(ddb, DUALEV_RDBDELETE, dndb->dndb_dest, dndb->dndb_dest,
	     drdb->drdb_nexthop);
    dual_freedata(ddb, &drdb->drdb_data);
    drdb->drdb_next = NULL;
    drdb->drdb_deleted = TRUE;		/* In case of a mem_lock */
    if (drdb->drdb_iidb)
	free(drdb->drdb_iidb);		/* Undo mem_lock */
    free(drdb);
}

/*
 * dual_rtdelete
 * Delete this entry from the routing table.
 */
static void dual_rtdelete (ddbtype *ddb, dual_ndbtype *dndb,
			   dual_rdbtype *drdb)
{
#ifdef notdef				/* Awfully verbose */
    DUAL_LOG(ddb, DUALEV_RTDELETE, dndb->dndb_dest, dndb->dndb_dest,
	     drdb->drdb_nexthop);
#endif
    (*ddb->rtdelete)(ddb, dndb, drdb);
}

/*
 * dual_zapdrdb
 * Used to remove routes from both the topology table and the routing table.
 * Notify signals the routing table of the event.
 */

static void dual_zapdrdb (ddbtype *ddb, dual_ndbtype *dndb,
			   dual_rdbtype *drdb, boolean notify)
{
    dual_rdbtype *cur_drdb;
    dual_rdbtype **prev;
    int successors;

    successors = dndb->dndb_successors;
    prev = &dndb->dndb_rdb;
    cur_drdb = *prev;
    while (cur_drdb) {
	if (cur_drdb == drdb) {
	    if (notify)
		dual_rtdelete(ddb, dndb, cur_drdb);
	    *prev = drdb->drdb_next;
	    if (dndb->dndb_succ == cur_drdb)
		dndb->dndb_succ = NULL;
	    dual_rdbdelete(ddb, dndb, cur_drdb);
	    if (successors > 0)
		dndb->dndb_successors--;
	    return;
	}
	prev = &cur_drdb->drdb_next;
	cur_drdb = *prev;
	successors--;
    }
}

/*
 * dual_dndbdelete
 * Remove an ndb from the topology table.  It's up to the caller to make
 * sure the rdb's have all been freed.
 *
 * Calls the ddb cleanup routine if present.
 *
 * NULLs out the supplied ddb pointer.
 */
static const char dual_removendb[] = "\nDUAL: No routes.  Flushing dest %s";
static void dual_dndbdelete (ddbtype *ddb, dual_ndbtype **dndbp)
{
    dual_ndbtype *dndb;
    dual_ndbtype **prev;
    ulong one = 1;
    ulong zero = 0;

    dndb = *dndbp;
    if (!dndb)
	return;

    /*
     * The thread refcount must be zero and the DNDB must not be on the
     * change queue.
     */
    if (dndb->dndb_xmit_thread.xt_refcount ||
	dndb->dndb_on_change_queue) { /* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /* Unthread the entry. */

    dual_xmit_unthread(ddb, &dndb->dndb_xmit_thread);

    IGRP2_FABUGINF(&dndb->dndb_dest)(dual_removendb,
				     (*ddb->printnet)(&dndb->dndb_dest));

    if (ddb->ndb_done != NULL) {
	(*ddb->ndb_done)(dndb);
    }

    prev = &ddb->topology[(*ddb->ndbbucket)(&dndb->dndb_dest)];
    while (*prev) {
	if (*prev == dndb) {
	    *prev = dndb->dndb_next;
	    dndb->dndb_next = NULL;
	    DUAL_LOG(ddb, DUALEV_NDBDELETE, dndb->dndb_dest, dndb->dndb_dest,
		     one);
	    dndb->dndb_deleted = TRUE;	/* In case of mem_lock */
	    free(dndb);
	    *dndbp = NULL;
	    return;
	}
	prev = &((*prev)->dndb_next);
    }
    
    /*
     * If we made it here we didn't find the ndb.  What should be 
     * done about it?  Return it, I guess.
     */
    DUAL_LOG(ddb, DUALEV_NDBDELETE, dndb->dndb_dest, dndb->dndb_dest, zero);
    return;
}

/*
 * dual_cleanup_dndb
 *
 * Clean up a DNDB after transmission has completed.  This consists of
 * clearing the appropriate sendflags mask, and if the destination is
 * unreachable, destroying the DNDB.
 *
 * The DNDB pointer will be nulled out if the DNDB is destroyed.
 */
static void dual_cleanup_dndb (ddbtype *ddb, dual_ndbtype **dndbp,
			       ulong send_mask)
{
    dual_ndbtype *dndb;
    dndb = *dndbp;
    dndb->dndb_send_flag &= ~send_mask;
    if (dndb->dndb_send_flag == 0 && dndb->dndb_rdb == NULL) {
	dual_dndbdelete(ddb, dndbp);
    }
}

/*
 * dual_move_xmit
 *
 * Move a xmit thread entry from its current position to the end of
 * the thread.  If the entry is currently anchored in its current
 * position, create a dummy thread entry before moving it.
 */
static void dual_move_xmit (ddbtype *ddb, igrp2_xmit_thread *entry)
{
    igrp2_xmit_thread *thread_dummy;

    thread_dummy = NULL;

    /* Unthread the entry.  A dummy will be created if anchored. */

    dual_xmit_unthread(ddb, entry);

    /* Now link the entry to the tail. */
    
    entry->xmit_prev = ddb->ddb_thread_tail; /* Set prev */
    *ddb->ddb_thread_tail = entry;		/* Set prev's next */
    ddb->ddb_thread_tail = &entry->xmit_next; /* Set tail */
    entry->xt_serno = dual_next_serno(ddb); /* Set serial # */
    entry->xmit_next = NULL;		/* Just to make sure. */
    entry->xt_anchor = NULL;		/* Ditto */
}

/*
 * dual_cleanup_drdb
 *
 * Clean up a DRDB after a reply is acknowledged (or the peer is being
 * deleted).  If zap_rdb is TRUE, the RDB will be deleted if conditions are
 * right;  if FALSE, only the send flag will be manipulated.
 *
 * Note that this may also delete the DNDB!  Callers beware!
 */
static void dual_cleanup_drdb (ddbtype *ddb, dual_rdbtype *drdb,
			       boolean zap_rdb)
{
    dual_ndbtype *dndb;
    boolean some_flag_set;

    drdb->drdb_send_flag &= ~DUAL_SENDREPLY;
    dndb = drdb->drdb_dndb;
    if (!dual_dndb_active(dndb) &&
	drdb->drdb_metric == METRIC_INACCESSIBLE &&
	drdb->drdb_send_flag == 0 &&
	zap_rdb) {
	dual_zapdrdb(ddb, dndb, drdb, TRUE);
    }
    
    /*
     * Now scan all DRDBs on this DNDB.  If none of them have
     * send_flag set, clear the DNDB send_flag bit, and try to
     * clean up the DNDB as well.
     */
    drdb = dndb->dndb_rdb;
    some_flag_set = FALSE;
    while (drdb) {
	if (drdb->drdb_send_flag) {
	    some_flag_set = TRUE;
	    break;
	}
	drdb = drdb->drdb_next;
    }
    
    if (!some_flag_set) {
	dual_cleanup_dndb(ddb, &dndb, DUAL_SENDREPLY);
    }
}

/*
 * dual_process_acked_reply
 *
 * Process the final acknowledgement of a reply.  Unthread the DRDBs
 * as appropriate.
 */
static void dual_process_acked_reply (ddbtype *ddb, igrp2_idbtype *iidb,
				      igrp2_pakdesc *pakdesc,
				      dual_peertype *peer)
{
    igrp2_xmit_thread *thread, *next_thread;
    igrp2_xmit_anchor *anchor;
    dual_rdbtype *drdb;

    IGRP2_XBUGINF(XDBACK, "\nReply acked from %s, serno %d-%d",
		  (*ddb->printaddr)(&peer->source),
		  pakdesc->start_serno, pakdesc->end_serno);

    /* Validity check. */

    if (!peer)
	peer = pakdesc->peer;
    if (!peer) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    anchor = pakdesc->pakdesc_anchor;
    if (!anchor) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /*
     * Walk the thread, unlinking entries until we hit an entry that
     * comes after the end of the packet.
     */
    thread = anchor->xa_thread;
    while (thread) {
	next_thread = thread->xmit_next;

	/* If we're past the end of the packet, bail. */

	if (dual_serno_later(thread->xt_serno,
			     pakdesc->end_serno)) {
	    break;
	}
	if (!thread->xt_dummy) {	/* It's real */

	    /* Unthread it. */

	    if (!thread->xt_drdb)
		crashdump(0);		/* Shouldn't be here! */
	    dual_xmit_unthread(ddb, thread);
	    IGRP2_XBUGINF(XDBACK|XDBDETAIL,
			  "\n  Found serno %d", thread->xt_serno);

	    /* Clear the DRDB send flag, and clean it up if it's all done. */

	    drdb = dual_thread_to_drdb(thread);
	    dual_log_xmit(ddb, DUALEV_REPLYACK, &drdb->drdb_dndb->dndb_dest,
			  &drdb->drdb_dndb->dndb_dest, &peer->source);
	    dual_cleanup_drdb(ddb, drdb, TRUE);
	}
	thread = next_thread;
    }

    /* Unanchor the packet start. */

    dual_unanchor_entry(ddb, anchor->xa_thread);
}

/*
 * dual_drop_dndb_refcount
 *
 * Decrement the refcount in the DNDB thread.  If it's gone to zero,
 * clear the send_flags and see about deleting the DNDB.
 *
 * Does nothing if the thread is a dummy.
 *
 * This may delete the DNDB from the chain and free it!  Do not reference
 * the thread entry or the DNDB in which it is embedded after calling this
 * routine!
 */
static void dual_drop_dndb_refcount (ddbtype *ddb, igrp2_xmit_thread *thread,
				     igrp2_pakdesc *pakdesc,
				     igrp2_idbtype *iidb)
{
    dual_ndbtype *dndb;
    dual_event_code event;

    if (!thread->xt_dummy) {
	if (thread->xt_drdb)
	    crashdump(0);		/* Shouldn't be here! */
	dndb = dual_thread_to_dndb(thread);
	if (pakdesc) {
	    if (pakdesc->opcode == IGRP_OPC_UPDATE)
		event = DUALEV_UPDATEACK;
	    else
		event = DUALEV_QUERYACK;
	} else {
	    if (dndb->dndb_send_flag & DUAL_MULTUPDATE)
		event = DUALEV_UPDATEACK;
	    else
		event = DUALEV_QUERYACK;
	}
	dual_log_xmit(ddb, event, &dndb->dndb_dest, &dndb->dndb_dest,
		      &iidb->idb);
	thread->xt_refcount--;
	if (thread->xt_refcount < 0) { /* Can't happen */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
	IGRP2_XBUGINF(XDBACK|XDBPEERDOWN|XDBDETAIL,
		      ", refcount now %d", thread->xt_refcount);
	if (thread->xt_refcount == 0) {
	    dndb->dndb_being_sent = FALSE; /* Everyone is done with it. */
	    dndb->dndb_split_horizon = TRUE; /* Ditto. */
	    if (dndb->dndb_on_change_queue) { /* Better not be. */
		errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
		return;
	    }

	    /*
	     * Clear the split-horizon suppression flag only if this is
	     * an update.  A bit of paranoia;  we want to make absolutely
	     * sure that we don't leave any old info behind after going
	     * passive.
	     */
	    if (pakdesc && pakdesc->opcode == IGRP_OPC_UPDATE)
		dndb->dndb_tell_everyone = FALSE; /* Done here as well. */
	    dual_cleanup_dndb(ddb, &dndb, DUAL_MULTMASK);
	}
    }
}

/*
 * dual_process_acked_multicast
 *
 * Process the acknowledgement of a multicast update or query.
 */
static void dual_process_acked_multicast (ddbtype *ddb, igrp2_idbtype *iidb,
					  igrp2_pakdesc *pakdesc,
					  dual_peertype *peer)
{
    igrp2_xmit_anchor *anchor;
    igrp2_xmit_thread *thread, *next_thread;

    if (!iidb) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /* It better be anchored! */

    anchor = pakdesc->pakdesc_anchor;
    if (!anchor) {			/* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    thread = anchor->xa_thread;
    if (thread->xt_serno != pakdesc->start_serno) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    IGRP2_XBUGINF(XDBACK, "\nMulticast acked from %s, serno %d-%d",
		  iidb->idb->namestring, pakdesc->start_serno,
		  pakdesc->end_serno);

    /*
     * Drop the refcount in each DNDB acknowledged by the packet.  If
     * the refcount goes to zero, clear the sendflags on the DNDB and
     * try cleaning it up.
     */
    while (thread) {
	next_thread = thread->xmit_next; /* We may free the DNDB! */

	/* Bail if we're past the end of the packet. */

	if (dual_serno_later(thread->xt_serno, pakdesc->end_serno))
	    break;

	if (!thread->xt_drdb) {		/* Ignore intermixed replies */
	    IGRP2_XBUGINF(XDBACK|XDBDETAIL,
			  "\n  Found serno %d", thread->xt_serno);
	    dual_drop_dndb_refcount(ddb, thread, pakdesc, iidb);
	}
	thread = next_thread;
    }

    /* Unanchor the start of the packet. */

    dual_unanchor_entry(ddb, anchor->xa_thread);
}

/*
 * dual_process_acked_startup
 *
 * Process the acknowledgement of a unicast update sent in the peer startup
 * stream.
 *
 * The fact that the startup process is still happening is tracked by
 * last_startup_serno;  if nonzero, we're still awaiting acknowledgement.
 */
static void dual_process_acked_startup (ddbtype *ddb, igrp2_idbtype *iidb,
					igrp2_pakdesc *pakdesc,
					dual_peertype *peer)
{
    igrp2_xmit_anchor *anchor;
    igrp2_xmit_thread *thread;

    /* There better be a peer! */

    if (!peer)
	peer = pakdesc->peer;
    if (!peer) {			/* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    IGRP2_XBUGINF(XDBACK|XDBSTARTUP,
		  "\nStartup update acked from %s, serno %d-%d",
		  (*ddb->printaddr)(&peer->source),
		  pakdesc->start_serno, pakdesc->end_serno);

    /* By definition our INIT packet has been acked;  duly note it. */

    peer->need_init_ack = FALSE;

    /* If the acknowledged packet is a dummy, we're done. */

    if (pakdesc->start_serno == 0)
	return;

    /* We better be expecting it. */

    if (!peer->last_startup_serno) {	/* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /* Get the anchored thread point. */

    anchor = pakdesc->pakdesc_anchor;
    if (!anchor) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }
    thread = anchor->xa_thread;
    if (thread->xt_serno != pakdesc->start_serno) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    IGRP2_XBUGINF(XDBACK|XDBSTARTUP,
		  ", thread ends %d", peer->last_startup_serno);

    /* Log it.  We do funny checks because we don't really have a dest. */

    if (!ddb->debug_peer_set ||
	(*ddb->addrmatch)(&peer->source, &ddb->debug_peer)) {
	dual_log_xmit_all(ddb, DUALEV_STARTUPACK, &peer->source,
			  &pakdesc->start_serno);
    }

    /*
     * If the end of the acknowledged packet is after what should have been
     * the end of the startup stream, there's a problem.
     */
    if (dual_serno_later(pakdesc->end_serno, peer->last_startup_serno)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /*
     * If the acknowledged packet ends at the end of the startup stream,
     * we're done.
     */
    if (pakdesc->end_serno == peer->last_startup_serno) {
	peer->last_startup_serno = 0;
    }

    /* Release the anchor. */

    if (anchor)
	dual_unanchor_entry(ddb, anchor->xa_thread);
}

/*
 * dual_packetize_thread
 *
 * Walk the thread, until one packet's worth of information has been seen, or
 * a target has been matched, or a packet type mismatch occurs.
 *
 * If target_serno is 0, no target match is done.
 *
 * If packet_type is NULL, this is a peer startup, and updates are sent
 * for all passive DNDBs found on the thread.  Otherwise, a group of packets
 * of the same type will be found and the packet type (in terms of a send_flag)
 * is returned.
 *
 * A pointer to the peer is returned if the packet type is a reply.
 *
 * Returns the ending serial number and a pointer to just past the last
 * entry put in the packet.  If nothing was put in the packet, the serial
 * number will be zero.
 *
 * This routine will size worst-case packets;  it is assumed that an
 * entry for every DNDB will be in the packet.  The application of filtering
 * and split horizon rules is done at packet generation time for simplicity.
 */
static igrp2_xmit_thread *dual_packetize_thread (ddbtype *ddb,
						 igrp2_idbtype *iidb,
						 dual_peertype **peer,
						 ulong *packet_type,
						 igrp2_xmit_thread *thread,
						 ulong target_serno,
						 ulong *end_serno)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    igrp2_xmit_thread *prev_thread;
    ulong bytes_left, item_size;
    boolean skip_entry;
    int peer_handle;

    bytes_left = iidb->max_packet_size - IGRP2HEADERBYTES; /* Max payload */
    prev_thread = NULL;
    peer_handle = NO_PEER_HANDLE;
    if (peer)
	*peer = NULL;
    while (thread) {

	/* Stop if we've hit the target. */
	
	if (target_serno &&
	    !dual_serno_earlier(thread->xt_serno, target_serno)) {
	    break;
	}

	if (!thread->xt_dummy) {	/* If it's for real */

	    /*
	     * Stop if we hit an entry with a different packet type to send.
	     * If it's the first time, note the packet type in the DNDB.
	     * Minor kludge--if no packet_type pointer is supplied, we will
	     * send all DNDBs, including ones with no send_flag set.
	     * This is done for startup updates.
	     */
	    if (thread->xt_drdb) {	/* It's a DRDB (reply) */
		drdb = dual_thread_to_drdb(thread);
		dndb = drdb->drdb_dndb;
	    } else {
		drdb = NULL;
		dndb = dual_thread_to_dndb(thread);
	    }
	    skip_entry = FALSE;
	    if (packet_type) {

		/*
		 * Skip entries with no sendflag.  DRDBs will always be sent
		 * since they are on the thread ephemerally.
		 */
		if (!drdb && !(dndb->dndb_send_flag & DUAL_MULTMASK)) {
		    skip_entry = TRUE;
		} else {
		    
		    /* Any packet type previously determined? */
		    
		    if (*packet_type) {
			
			/*
			 * If we have been sending Replies, the new entry
			 * must be a DRDB for the correct peer.
			 */
			if (*packet_type == DUAL_SENDREPLY) {
			    if (!drdb ||
				(drdb->drdb_handle != peer_handle)) {
				break;
			    }
			} else {
			    
			    /*
			     * We've been sending updates or queries.  If this
			     * entry is a reply (DRDB), stop if it's on our
			     * interface (because we have to send it), or
			     * skip it otherwise.
			     */
			    if (drdb) {
				if (drdb->drdb_iidb == iidb) {
				    break;
				} else {
				    skip_entry = TRUE;
				}
			    } else {

				/*
				 * Not a reply.  Stop if this entry is the
				 * wrong type.
				 */
				if ((dndb->dndb_send_flag & DUAL_MULTMASK) !=
				    *packet_type) {
				    break;
				}
			    }
			}
		    } else {
			
			/*
			 * No previous packet type found.  If this is a
			 * reply, skip it if we're on the wrong interface.
			 * If it's the right interface, note the packet
			 * type and peer.
			 *
			 * For non-replies, note the packet type.
			 */
			if (drdb) {	/* A reply */
			    if (drdb->drdb_iidb == iidb) {
				*packet_type = DUAL_SENDREPLY;
				peer_handle = drdb->drdb_handle;
				*peer = igrp2_handle_to_peer(ddb, peer_handle);
			    } else {	/* Wrong interface */
				skip_entry = TRUE;
			    }
			} else {
			    *packet_type = dndb->dndb_send_flag &
				DUAL_MULTMASK;
			}
		    }
		}
	    } else {			/* !packet_type */

		/*
		 * This is a peer startup situation.  Skip DRDBs but don't
		 * skip any DNDBs.
		 */
		if (drdb)
		    skip_entry = TRUE;
	    }

	    if (!skip_entry) {

		/* Got a live one.  Ask the PDM how big it will be. */
		
		item_size = (*ddb->item_size)(dndb);
		
		/* If it's too big to fit, bail.  Otherwise, charge for it. */
		
		if (item_size > bytes_left)
		    break;
		bytes_left -= item_size;

		/* Mark as being sent, since we're committed to sending it. */

		dndb->dndb_being_sent = TRUE;

		/*
		 * Log it if this is not a startup packet.  Startups are logged
		 * elsewhere.
		 */
		if (packet_type) {
		    switch (*packet_type) {
		      case DUAL_MULTUPDATE:
			dual_log_xmit(ddb, DUALEV_UPDATEPACK, &dndb->dndb_dest,
				      &dndb->dndb_dest, &iidb->idb);
			break;
		      case DUAL_MULTQUERY:
			dual_log_xmit(ddb, DUALEV_QUERYPACK, &dndb->dndb_dest,
				      &dndb->dndb_dest, &iidb->idb);
			break;
		      case DUAL_SENDREPLY:
			dual_log_xmit(ddb, DUALEV_REPLYPACK, &dndb->dndb_dest,
				      &dndb->dndb_dest, &(*peer)->source);
			break;
		    }
		}
	    }
	}

	/* Move to the next thread entry. */

	prev_thread = thread;
	thread = thread->xmit_next;
    }
    
    /* Note the ending serial number, if there is anything here. */

    if (prev_thread) {			/* The packet has contents */
	*end_serno = prev_thread->xt_serno;
    } else {				/* Nothing there */
	*end_serno = 0;
    }
    return(thread);
}

/*
 * dual_packetize_interface
 *
 * Figure out the contents of the next packet for this interface, and
 * call the transport to create and enqueue the packet descriptor for
 * it.  The packet will actually be built when the transport calls us
 * back through the buildpacket callback.
 *
 * We assume that the spot in the chain to packetize is pointed to by
 * iidb->iidb_xmit_anchor.
 */
static void dual_packetize_interface (ddbtype *ddb, igrp2_idbtype *iidb)
{
    ulong packet_type;
    igrp2_xmit_thread *thread, *first_entry;
    ulong start_serno, end_serno;
    ulong opcode;
    igrp2_pakdesc *pakdesc;
    igrp2_xmit_anchor *anchor;
    dual_event_code event;
    dual_peertype *peer;
    dual_rdbtype *drdb;

    /* Bail if we shouldn't be here. */

    anchor = iidb->iidb_xmit_anchor;
    if (!anchor) {			/* Can't happen */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    /*
     * We need to ignore multicast flow control if we're about to packetize
     * a reply packet, but not if we're going to send a multicast.  In order
     * to decide, we need to look at the type of the first entry that we're
     * going to packetize.  In order to do *that*, we have to skip any
     * dummy entries on the thread, as well as any DRDBs (replies) that
     * aren't for this interface.  So let's do that first.
     */
    thread = anchor->xa_thread;
    while (thread) {
	if (!thread->xt_dummy) {	/* Not a dummy */
	    if (!thread->xt_drdb)	/* Not a reply */
		break;
	    drdb = dual_thread_to_drdb(thread);
	    if (drdb->drdb_iidb == iidb) /* The right interface */
		break;
	}
	thread = thread->xmit_next;
    }
    if (thread) {			/* Something left? */
	if (!thread->xt_drdb) {		/* Not a reply */
	    if (!igrp2_interface_is_flow_ready(iidb)) { /* Not flow-ready */
		IGRP2_XBUGINF(XDBPACKETIZE, "\n  Interface not flow-ready");
		return;
	    }
	}
	
	/* Call the omnibus packetizer to do the work. */
	
	packet_type = 0;
	start_serno = thread->xt_serno;
	first_entry = thread;
	thread = dual_packetize_thread(ddb, iidb, &peer, &packet_type, thread,
				       0, &end_serno);
	
	/* Figure out the EIGRP opcode. */
	
	opcode = igrp2_dual2opc(packet_type);
	if (opcode == IGRP_OPC_ILLEGAL) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
	
	IGRP2_XBUGINF(XDBPACKETIZE, "\nIntf %s packetized %s %d-%d",
		      iidb->idb->namestring, igrp2_otype(opcode),
		      start_serno, end_serno);
	if (opcode == IGRP_OPC_REPLY)
	    IGRP2_XBUGINF(XDBPACKETIZE, ", peer %s",
			  (*ddb->printaddr)(&peer->source));
	
	/* Now ask the transport to enqueue the packet descriptor. */
	
	pakdesc = igrp2_enqueue(ddb, peer, iidb, TRUE);
	
	/* Write down the good stuff in the packet descriptor. */
	
	if (pakdesc) {
	    pakdesc->opcode = opcode;
	    pakdesc->start_serno = start_serno;
	    pakdesc->end_serno = end_serno;
	    pakdesc->pakdesc_anchor = dual_anchor_entry(ddb, first_entry);
	    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	    if (peer) {
		dual_log_xport(ddb, DUALEV_REPLYENQ, &peer->source,
			       &peer->source, &peer->iidb->idb);
	    } else {
		if (opcode == IGRP_OPC_UPDATE)
		    event = DUALEV_UPDATEENQ;
		else
		    event = DUALEV_QUERYENQ;
		dual_log_xport_all(ddb, event, &iidb->idb, NULL);
	    }
	}

	/*
	 * Charge for the packet so that deferring PDM code can eventually
	 * get a chance to send theirs.
	 */
	if (iidb->pdm_defer_count) {	/* Still deffering? */
	    iidb->pdm_defer_count--;	/* Another notch in the barrel */
	}
    }

    /* Re-anchor the IIDB at the new position. */

    if (thread) {			/* More to go on the thread */
	iidb->iidb_xmit_anchor = dual_anchor_entry(ddb, thread);
	IGRP2_XBUGINF(XDBPACKETIZE,
		      "\n  New anchor serno %d", thread->xt_serno);
    } else {				/* End of the line */
	iidb->iidb_xmit_anchor = NULL;
	dual_add_iidb_to_quiescent_list(ddb, iidb);
	IGRP2_XBUGINF(XDBPACKETIZE, "\n  Interface is now quiescent");
    }

    /* Release the old IIDB anchor. */

    dual_unanchor_entry(ddb, anchor->xa_thread);
}

/*
 * dual_process_acked_packet
 *
 * The final acknowledgement has arrived for a packet.  Clean up after the
 * packet as appropriate.  Returns TRUE if the acked packet was processed.
 */
static boolean dual_process_acked_packet (ddbtype *ddb, igrp2_idbtype *iidb,
					  igrp2_pakdesc *pakdesc,
					  dual_peertype *peer)
{
    /*
     * Cleanup is according to packet type.  Unknown types are the
     * purview of the PDM, and are handled elsewhere.
     */
    switch (pakdesc->opcode) {
      case IGRP_OPC_UPDATE:
	if (!pakdesc->pakdesc_multicast) { /* A startup update */
	    dual_process_acked_startup(ddb, iidb, pakdesc, peer);
	} else {			/* A multicast update */
	    dual_process_acked_multicast(ddb, iidb, pakdesc, peer);
	}
	break;

      case IGRP_OPC_QUERY:
	dual_process_acked_multicast(ddb, iidb, pakdesc, peer);
	break;

      case IGRP_OPC_REPLY:
	dual_process_acked_reply(ddb, iidb, pakdesc, peer);
	break;

      default:
	return(FALSE);
    }
    return(TRUE);
}

/*
 * dual_xmit_continue
 *
 * See if there's something to do on an interface as far as transmission
 * goes.
 *
 * If the PDM has any other data to send, we call the PDM first to see
 * if it wishes to send anything or not.  We tell the PDM whether or not 
 * there are DUAL packets waiting to go, so that it can make the
 * precedence decision.
 *
 * This routine is called when we get a "flow-ready" callback from
 * the transport, and when the interface packetization timer expires.
 *
 * If completed_packet is non-NULL, it is a points to packet that has
 * completed transmission.  If completed_peer is non-null, the completed
 * packet was acknowledged by that peer;  if it is NULL but completed_packet
 * is non-null, the interface is no longer blocked for multicast by the packet.
 * Note that this does not necessarily mean that the packet is done being
 * transmitted everywhere;  if the refcount in the packet descriptor is 0,
 * this means that the packet has been successfully been transmitted everywhere
 * and the resources behind that packet may be released.  (The refcount may
 * be greater than 0 if the multicast transmission timer expired, for
 * instance.)
 */
void dual_xmit_continue (ddbtype *ddb, igrp2_idbtype *iidb,
			 igrp2_pakdesc *completed_packet,
			 dual_peertype *completed_peer)
{
    boolean packets_pending;

    /*
     * Make sure that the DDB change queue has been flushed before going
     * on.  This is necessary because we may here due to a peer going
     * down (flushing out the peer transmit queue) in which case we
     * may have half-sent updates pending.
     */
    dual_finish_update_send(ddb);

    /* If this packet has been fully ACKed, do any necessary cleanup. */

    if (completed_packet && !completed_packet->refcount) {
	if (dual_process_acked_packet(ddb, iidb, completed_packet,
				      completed_peer)) {
	    completed_packet = NULL;
	}
    }

    /* See what's waiting. */

    packets_pending = (iidb->iidb_xmit_anchor != NULL);

    /* Give the PDM a chance to handle the ACK and send another packet. */

    if (ddb->transport_ready) {
	(*ddb->transport_ready)(ddb, iidb, completed_packet, completed_peer,
				packets_pending);
    }

    /*
     * If there's something pending, go call the packetizer.  It may do
     * nothing if flow-control conditions aren't conducive.
     */
    if (packets_pending) {
	IGRP2_XBUGINF(XDBPACKETIZE,
		      "\nPackets pending on %s", iidb->idb->namestring);
	dual_packetize_interface(ddb, iidb);
    }
}

/*
 * dual_xmit_drdb
 *
 * Thread a DRDB for transmission.  Links the DRDB onto the thread,
 * and links the peer onto the IIDB for transmission and anchors the chain
 * if this was the first unpacketized DRDB on the thread.
 */
static void dual_xmit_drdb (ddbtype *ddb, dual_rdbtype *drdb)
{
    dual_peertype *peer;
    igrp2_xmit_thread *thread;
    igrp2_idbtype *iidb;
    dual_ndbtype *dndb;

    dndb = drdb->drdb_dndb;
    peer = igrp2_handle_to_peer(ddb, drdb->drdb_handle);
    if (!peer) {			/* Uhoh */
	IGRP2_XBUGINF(XDBLINK|XDBSTRANGE, "\nNo peer for DRDB %s, handle %d",
		      (*ddb->printnet)(&dndb->dndb_dest),
		      drdb->drdb_handle);
	return;
    }
    iidb = peer->iidb;

    /* (Re)link it to the end of the thread. */

    thread = &drdb->drdb_thread;
    IGRP2_XBUGINF(XDBLINK, "\nDRDB %s REPLY, serno %d",
		  (*ddb->printnet)(&dndb->dndb_dest), thread->xt_serno);
    dual_move_xmit(ddb, thread);
    IGRP2_XBUGINF(XDBLINK, " to %d on peer %s (%s)", thread->xt_serno,
		  (*ddb->printaddr)(&peer->source), iidb->idb->namestring);

    /*
     * If the IIDB is quiescent, remove it from the quiescent list, anchor
     * it at this entry, and kick the packetizing timer.
     */
    if (dual_iidb_on_quiescent_list(iidb)) {
	if (iidb->iidb_xmit_anchor) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
	dual_remove_iidb_from_quiescent_list(ddb, iidb);
	iidb->iidb_xmit_anchor = dual_anchor_entry(ddb, thread);
	if (!mgd_timer_running(&iidb->iidb_packetize_timer)) {
	    IGRP2_XBUGINF(XDBLINK, ", IIDB timer started");
	    mgd_timer_start(&iidb->iidb_packetize_timer,
			    IGRP2_PACKETIZATION_DELAY);
	}
    }
}

/*
 * dual_xmit_dndb
 *
 * Thread a DNDB for transmission.  Links the DNDB into the thread.  Anchors
 * all quiescent IIDBs to this entry and starts their packetization timers.
 *
 * The DNDB pointer will be NULLed out if the DNDB is deleted.
 */
static void dual_xmit_dndb (ddbtype *ddb, dual_ndbtype **dndbp)
{
    igrp2_idbtype *iidb;
    igrp2_xmit_thread *thread;
    dual_ndbtype *dndb;

    dndb = *dndbp;
    thread = &dndb->dndb_xmit_thread;

    /* (Re)link it to the end of the thread. */

    IGRP2_XBUGINF(XDBLINK, "\nDNDB %s %s, serno %d",
		  igrp2_otype(igrp2_dual2opc(dndb->dndb_send_flag &
					     DUAL_MULTMASK)),
		  (*ddb->printnet)(&dndb->dndb_dest), thread->xt_serno);
    dual_move_xmit(ddb, thread);
    IGRP2_XBUGINF(XDBLINK, " to %d", thread->xt_serno);

    /* Set the refcount to the number of interfaces with live peers. */

    thread->xt_refcount = ddb->ddb_active_if_count;
    IGRP2_XBUGINF(XDBLINK, ", refcount %d", thread->xt_refcount);

    /* Proceed if there's somebody to send this route to. */

    if (thread->xt_refcount) {

	/*
	 * If the route is in the process of being sent in its old state
	 * and we don't know whether everyone has gotten it yet or not,
	 * set the "tell_everyone" flag to cause split horizon to be
	 * suppressed on the route.  Otherwise we could get into serious
	 * trouble if the new route is split horizoned on a different
	 * interface than the old one.
	 */
	if (dndb->dndb_being_sent) {
	    dndb->dndb_tell_everyone = TRUE;
	    dual_log(ddb, DUALEV_OBE, &dndb->dndb_dest, &dndb->dndb_dest,
		     &thread->xt_refcount);
	}

	/* Anchor all quiescent IIDBs to this entry. */

	while ((iidb = ddb->quiescent_iidbs)) {
	    if (iidb->iidb_xmit_anchor) {
		errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
		return;
	    }
	    iidb->iidb_xmit_anchor = dual_anchor_entry(ddb, thread);
	    IGRP2_XBUGINF(XDBLINK, "\n  Anchoring %s",
			  iidb->idb->namestring);
	    dual_remove_iidb_from_quiescent_list(ddb, iidb);
	    if (!mgd_timer_running(&iidb->iidb_packetize_timer)) {
		IGRP2_XBUGINF(XDBLINK,
			      ", starting %s timer", iidb->idb->namestring);
		mgd_timer_start(&iidb->iidb_packetize_timer,
				IGRP2_PACKETIZATION_DELAY);
	    }
	}

    } else {				/* No peers anywhere */

	/*
	 * Nobody to tell.  Wipe out the send flag.  Note that it is not
	 * critical that we relinked this DNDB, as the order of the thread
	 * doesn't matter if there are no peers anywhere.  I just like having
	 * the thread order reflect the temporal ordering of events.
	 */
	dual_cleanup_dndb(ddb, dndbp, DUAL_MULTMASK);
    }
}

/*
 * dual_finish_update_send_guts
 *
 * Install the contents of the change queue.  This is the final phase of
 * this kludge.  We have deferred rethreading DNDBs until an entire
 * packet's worth of changes has been processed.  This is done because
 * the receipt of a reply that makes us go passive may result in a whole
 * bunch of alternating reply/update pairs, resulting in a zillion little
 * packets.  Instead, dual_sendupdate stashes the changed DNDBs in the
 * change queue, and then at the end of packet processing they are all
 * installed in the thread.  (Of course this means that everyone that
 * calls dual_FC must play this game as well, but them's the breaks).
 */
void dual_finish_update_send_guts (ddbtype *ddb) {
    dual_ndbtype *dndb;
    ulong count;

    /* Walk the change queue, moving the DNDBs into the thread. */

    count = 0;
    while ((dndb = ddb->ddb_change_queue)) {
	if (!dndb->dndb_on_change_queue) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
	ddb->ddb_change_queue = dndb->dndb_next_chg;
	dndb->dndb_on_change_queue = FALSE;
	dndb->dndb_next_chg = NULL;	/* Nice 'n' paranoid */
	dual_xmit_dndb(ddb, &dndb);
	count++;
    }
    ddb->ddb_change_queue_tail = &ddb->ddb_change_queue; /* It's empty now */
    dual_log_all(ddb, DUALEV_EMPTYCHANGEQ, &count, NULL);
}

/*
 * dual_new_peer
 *
 * Process the creation of a new peer.  We enqueue the topology
 * database as unicast packets for the peer, up to the current anchor
 * point of the interface (everything after that will be sent as
 * multicast updates).
 *
 * This routine periodically suspends!  Beware.
 */
void dual_new_peer (ddbtype *ddb, dual_peertype *peer)
{
    igrp2_xmit_anchor *start_anchor;
    igrp2_xmit_thread *thread;
    ulong target_serno;
    igrp2_idbtype *iidb;
    idbtype *idb;
    ulong start_serno, end_serno;
    boolean first_packet;
    igrp2_pakdesc *pakdesc;

    iidb = peer->iidb;
    idb = iidb->idb;
    IGRP2_XBUGINF(XDBSTARTUP, "\nNew peer %s on %s",
		  (*ddb->printaddr)(&peer->source), idb->namestring);
    DUAL_LOG_ALL(ddb, DUALEV_PEERUP, peer->source, idb);
    thread = ddb->ddb_thread_head;	/* Go to the head of the class */

    /* Run from the head of the chain until the current IIDB anchor point. */

    if (iidb->iidb_xmit_anchor) {
	target_serno = iidb->iidb_xmit_anchor->xa_thread->xt_serno;
	IGRP2_XBUGINF(XDBSTARTUP,
		      "\n  IIDB anchored at serno %d", target_serno);
    } else {
	target_serno = 0;
	IGRP2_XBUGINF(XDBSTARTUP, "\n  No IIDB anchor");
    }

    /* Loop until we get to the end of the road for this interface. */

    first_packet = TRUE;
    start_anchor = NULL;
    while (thread &&
	   thread->xt_serno != target_serno) { /* Top of packet loop */

	/* Determine the serial number range for the next packet. */

	start_serno = thread->xt_serno;
	start_anchor = dual_anchor_entry(ddb, thread);
	thread = dual_packetize_thread(ddb, iidb, NULL, NULL, thread,
				       target_serno, &end_serno);
	IGRP2_XBUGINF(XDBSTARTUP|XDBPACKETIZE|XDBDETAIL,
		      "\n  Packetized serno %d-%d", start_serno, end_serno);

	/* Log it.  We do funny checks because we don't really have a dest. */
	
	if (!ddb->debug_peer_set ||
	    (*ddb->addrmatch)(&peer->source, &ddb->debug_peer)) {
	    dual_log_xmit_all(ddb, DUALEV_STARTUPPACK, &peer->source,
			      &start_serno);
	}

	/* Bail from the loop if the packetizer returned nothing. */

	if (!end_serno)
	    break;

	/*
	 * Note the ending serial number in the peer structure.  This will
	 * come in handy in order to decide when we've seen all of our
	 * startup packets acked.
	 */
	peer->last_startup_serno = end_serno;

	/* Enqueue the packet descriptor. */

	pakdesc = igrp2_enqueue(ddb, peer, iidb, TRUE);
	dual_log_xport(ddb, DUALEV_STARTUPENQ, &peer->source, &peer->source,
		       &iidb->idb);
	dual_log_xport(ddb, DUALEV_STARTUPENQ2, &peer->source,
		       &start_serno, &pakdesc->ack_seq_num);
	    
	/* Note the various parameters and set the INIT flag properly. */
	
	if (pakdesc) {
	    pakdesc->opcode = IGRP_OPC_UPDATE;
	    pakdesc->start_serno = start_serno;
	    pakdesc->end_serno = end_serno;
	    pakdesc->set_init_flag = first_packet;
	    pakdesc->pakdesc_anchor = start_anchor;
	    start_anchor = NULL;
	    first_packet = FALSE;
	    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	}

	/* Now the pause that refreshes. */

	process_may_suspend();
    }

    if (start_anchor)
	dual_unanchor_entry(ddb, start_anchor->xa_thread);

    /*
     * If we haven't managed to send a packet yet, do so now (send a
     * null UPDATE) as required by law.
     */
    if (first_packet) {
	IGRP2_XBUGINF(XDBSTARTUP, "\n  Sending null packet");
	pakdesc = igrp2_enqueue(ddb, peer, iidb, TRUE);
	if (pakdesc) {
	    pakdesc->opcode = IGRP_OPC_UPDATE;
	    pakdesc->start_serno = pakdesc->end_serno = 0;
	    pakdesc->set_init_flag = TRUE;
	    igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	}
	peer->last_startup_serno = 0;
    }

    /*
     * Final validity check.  We better have hit the target if there was one.
     */
    if (target_serno) {
	if (!thread || thread->xt_serno != target_serno) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
    } else {
	if (thread) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
    }
}

/*
 * dual_splithorizon
 * Determine if split horizon should be used on this interface.  Return TRUE
 * means this destination should be suppressed due to the splithorizon rule.
 */
static boolean dual_splithorizon (ddbtype *ddb, dual_ndbtype *dndb,
				  igrp2_idbtype *iidb)
{
    dual_rdbtype *drdb;
    int successors;

    /*
     * This is an indication that we are active for this destination and
     * the first entry may not be the successor before going active.
     */
    if (dndb->dndb_succ) {
	drdb = dndb->dndb_succ;
	if (drdb->drdb_iidb == iidb) {
	    return(iidb->split_horizon);
	}
	return(FALSE);
    }
	    
    successors = dndb->dndb_successors;
    drdb = dndb->dndb_rdb;
    while ((drdb != NULL) && (successors > 0)) {
	if (drdb->drdb_iidb == iidb) {
	    return(iidb->split_horizon);
	}
	drdb = drdb->drdb_next;
	successors--;
    }
    return(FALSE);
}

/*
 * dual_should_advertise
 *
 * Ask protocol dependent protocol if this destination should be 
 * advertised out the interface.
 */
static advertise_request_type dual_should_advertise (ddbtype *ddb,
						     dual_ndbtype *dndb,
						     igrp2_idbtype *iidb)
{

    if (!ddb->advertise_request)
	return(TRUE);

    return((*ddb->advertise_request)(ddb, &dndb->dndb_dest, iidb->idb));
}

/*
 * dual_buildpacket
 *
 * Build a packet to be transmitted, based on the information saved in
 * a packet descriptor.
 *
 * Called by the transport when it's actually time to send a packet.
 */
typedef enum thread_kind_ {STARTUP_THREAD, MCAST_THREAD, UCAST_THREAD}
    thread_kind;
igrp2type *dual_buildpacket (ddbtype *ddb,
			     igrp2_idbtype *iidb,
			     dual_peertype *peer,
			     igrp2_pakdesc *pakdesc,
			     boolean *packet_suppressed)
{
    igrp2_xmit_thread *thread;
    igrp2type *igrp2;
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    uchar *packet_ptr;
    thread_kind thread_type;
    ulong bytes_left;
    ulong item_size;
    ulong payload_size;
    boolean advertise_unreachable;
    boolean advertise_item;
    void *log_param;
    dual_event_code event;

    log_param = NULL;			/* Stupid compiler */
    event = DUALEV_UNKNOWN;
    *packet_suppressed = FALSE;
    payload_size = 0;

    /* Decide what to do based on the packet type. */

    switch (pakdesc->opcode) {

	/* First the DNDB-derived packets. */

      case IGRP_OPC_UPDATE:
      case IGRP_OPC_QUERY:
	if (pakdesc->opcode == IGRP_OPC_UPDATE &&
	    !pakdesc->pakdesc_multicast) {
	    
	    /* A startup Update packet. */

	    IGRP2_XBUGINF(XDBBUILD|XDBSTARTUP,
			  "\nBuilding startup packet for %s",
			  (*ddb->printaddr)(&peer->source));
	    thread_type = STARTUP_THREAD;

	} else {

	    /* A generic Update or Query. */

	    IGRP2_XBUGINF(XDBBUILD, "\nBuilding multicast %s packet for %s",
			  igrp2_otype(pakdesc->opcode), iidb->idb->namestring);
	    thread_type = MCAST_THREAD;
	}
	break;


	/* Now the DRDB-derived packets. */

      case IGRP_OPC_REPLY:

	/* Standard unicast reply. */

	IGRP2_XBUGINF(XDBBUILD, "\nBuilding REPLY packet for %s",
		      (*ddb->printaddr)(&peer->source));
	if (pakdesc->start_serno == 0)
	    IGRP2_XBUGINF(XDBBUILD, " (no data)");
	thread_type = UCAST_THREAD;
	break;

	/* Anything else must belong to the PDM.  Call them. */

      default:

	if (!ddb->buildpacket) {	/* Uhoh */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    *packet_suppressed = TRUE;
	    return(NULL);
	}
	IGRP2_XBUGINF(XDBBUILD, "\nBuilding PDM-specific packet on %s",
		      iidb->idb->namestring);
	igrp2 = (*ddb->buildpacket)(ddb, peer, pakdesc, packet_suppressed);
	return(igrp2);
    }

    IGRP2_XBUGINF(XDBBUILD,
		  ", serno %d-%d", pakdesc->start_serno, pakdesc->end_serno);

    if (pakdesc->pakdesc_anchor) {
	thread = pakdesc->pakdesc_anchor->xa_thread;
    } else {
	thread = NULL;
    }
    
    /*
     * If there's nothing there, suppress the packet, unless this is
     * the first packet of the startup thread (we have to send a null
     * packet in that case).
     */
    if (!pakdesc->set_init_flag && !thread) {
	*packet_suppressed = TRUE;
	return(NULL);
    }
    
    /* Now construct the packet. */

    igrp2 = igrp2_getbuf(iidb->max_packet_size);
    if (igrp2) {
	packet_ptr = igrp2_packet_data(igrp2);
	bytes_left = iidb->max_packet_size - IGRP2HEADERBYTES;
	if (thread) {
	    IGRP2_XBUGINF(XDBBUILD|XDBDETAIL, "\n  Items: ");
	} else {
	    IGRP2_XBUGINF(XDBBUILD|XDBDETAIL, "\n  No items in range");
	}
	
	/* Troll the thread until we're past the end of the packet. */

	while (thread) {
	    if (!thread->xt_dummy) {	/* It's real */

		/* Get the DNDB pointer. */

		if (thread->xt_drdb) {
		    drdb = dual_thread_to_drdb(thread);
		    dndb = drdb->drdb_dndb;
		} else {
		    dndb = dual_thread_to_dndb(thread);
		}

		/* Apply the appropriate test. */

		advertise_unreachable = FALSE;
		advertise_item = FALSE;
		log_param = NULL;

		switch (thread_type) {

		    /* The peer startup update thread. */

		  case STARTUP_THREAD:

		    /* Send it if passive and not otherwise blocked. */

		    if (!thread->xt_drdb &&
			!dual_dndb_active(dndb) &&
			dndb->dndb_rdb &&
			(dual_should_advertise(ddb, dndb, iidb) ==
			 ADVERTISE_ROUTE) &&
			!dual_splithorizon(ddb, dndb, iidb)) {
			advertise_item = TRUE;
			log_param = &peer->source;
			event = DUALEV_STARTUPXMIT;
		    }
		    break;


		    /* Multicast updates and queries. */

		  case MCAST_THREAD:

		    if (!thread->xt_drdb) {

			/*
			 * If split horizon is enabled on the interface,
			 * don't send anything, except when the DNDB says
			 * to ignore split horizon (in which case we send
			 * an unreachable).
			 */
			if (dual_splithorizon(ddb, dndb, iidb)) {
			    if (!dndb->dndb_split_horizon ||
				dndb->dndb_tell_everyone) {
				advertise_item = TRUE;
				advertise_unreachable = TRUE;
			    }
			} else {
			    /*
			     * No split horizon.  Always advertise queries,
			     * but allow updates to be suppressed by filters.
			     */
			    switch (dual_should_advertise(ddb, dndb, iidb)) {
			      case ADVERTISE_ROUTE:
				advertise_item = TRUE;
				break;

			      case POISON_ROUTE:
				advertise_item = TRUE;
				advertise_unreachable = TRUE;
				break;

			      case SUPPRESS_ROUTE:
				if (pakdesc->opcode == IGRP_OPC_QUERY) {
				    advertise_item = TRUE;
				    advertise_unreachable = TRUE;
				}
				break;
			    }
			}

			/*
			 * Last-ditch stupidity avoidance.  If this is a query,
			 * enforce the split-horizoning determination we made
			 * when we went active.  In particular, if this is the
			 * split horizon interface, we must never send any
			 * query on it.  Conversely, if this is not the split
			 * horizon interface we must *always* send a query on
			 * it.
			 */
			if (pakdesc->opcode == IGRP_OPC_QUERY) {
#ifdef ACTIVE_METRIC_KLUDGE
			    if (bcmp(&dndb->dndb_vecmetric,
				     &dndb->query_metric,
				     sizeof(igrp2metric))) {
				buginf("\nUnequal, %s, dndb=%d, query=%d",
				       (*ddb->printnet)(&dndb->dndb_dest),
				       dndb->dndb_vecmetric.delay,
				       dndb->query_metric.delay);
			    }
#endif
			    if (dndb->dndb_query_sh_if == iidb) {
				if (advertise_item) {
				    advertise_item = FALSE;
				    dual_log_xmit(ddb, DUALEV_QSHON,
						  &dndb->dndb_dest,
						  &dndb->dndb_dest,
						  &iidb->idb);
				}
			    } else {
				if (!advertise_item) {
				    advertise_item = TRUE;
				    advertise_unreachable = TRUE;
#ifdef DUAL_SPLIT_HORIZON_QUERIES	/* Don't whimper otherwise. */
				    dual_log_xmit(ddb, DUALEV_QSHOFF,
						  &dndb->dndb_dest,
						  &dndb->dndb_dest,
						  &iidb->idb);
#endif
				}
			    }
			    event = DUALEV_QUERYXMIT;
			} else {
			    event = DUALEV_UPDATEXMIT;
			    if (dual_dndb_active(dndb)) {
				advertise_item = FALSE;
				dual_log(ddb, DUALEV_UPDATESQUASH,
					 &dndb->dndb_dest, &dndb->dndb_dest,
					 &iidb->idb);
			    }
			}
			log_param = &iidb->idb;
		    }
		    break;


		    /* Unicast replies */

		  case UCAST_THREAD:

		    /*
		     * Always advertise, but make it unreachable if we are
		     * suppressing updates on this interface.
		     */
		    if (thread->xt_drdb) {
			advertise_item = TRUE;
			if ((dual_should_advertise(ddb, dndb, iidb) !=
			     ADVERTISE_ROUTE) ||
			    dual_splithorizon(ddb, dndb, iidb)) {
			    advertise_unreachable = TRUE;
			}
			log_param = &peer->source;
			event = DUALEV_REPLYXMIT;
		    }
		    break;
		}

		/*
		 * Whew.  If we're supposed to advertise this item, call the
		 * PDM to actually build the item.  If we're advertising
		 * a query and the active timer hasn't been started, do it
		 * now.  (This keeps us from starting the active timer until
		 * we've at least had an opportunity to transmit the packet.)
		 */
		if (advertise_item) {
		    if (advertise_unreachable) {
			IGRP2_XBUGINF(XDBBUILD|XDBDETAIL,
				      " U%d", thread->xt_serno);
		    } else {
			IGRP2_XBUGINF(XDBBUILD|XDBDETAIL,
				      " %d", thread->xt_serno);
		    }
		    item_size = (*ddb->add_item)(ddb, iidb, dndb, packet_ptr,
						 bytes_left,
						 advertise_unreachable);
		    if (!item_size) {	/* Overflow! */
			free(igrp2);
			errmsg(&msgsym(INTERNAL, DUAL), ddb->name,
			       ddb->asystem);
			return(NULL);
		    }
		    packet_ptr += item_size;
		    bytes_left -= item_size;
		    payload_size += item_size;
		    if (pakdesc->opcode == IGRP_OPC_QUERY &&
			!TIMER_RUNNING(dndb->dndb_active_time)) {
			GET_TIMESTAMP(dndb->dndb_active_time);
		    }
		    dual_log_xmit(ddb, event, &dndb->dndb_dest,
				  &dndb->dndb_dest, log_param);
		    dual_log_xmit(ddb, event + 1, &dndb->dndb_dest,
				  &dndb->dndb_vecmetric.delay,
				  &advertise_unreachable);
		} else {
		    IGRP2_XBUGINF(XDBBUILD|XDBDETAIL,
				  " S%d", thread->xt_serno);
		}
	    } else {			/* Dummy */
		IGRP2_XBUGINF(XDBBUILD|XDBDETAIL, " D%d", thread->xt_serno);
	    }
	    
	    /*
	     * Advance to the next item on the thread.  Bail if it's past
	     * the end of our packet.
	     */
	    thread = thread->xmit_next;
	    if (!thread)
		break;
	    if (dual_serno_later(thread->xt_serno, pakdesc->end_serno))
		break;
	}				/* while (thread) */

	/*
	 * If nothing was added to the packet, and it's not the initial
	 * packet, suppress it.
	 */
	if (!payload_size && !pakdesc->set_init_flag) {
	    IGRP2_XBUGINF(XDBBUILD, "\n  Suppressed.");
	    free(igrp2);
	    igrp2 = NULL;
	    *packet_suppressed = TRUE;
	}
	pakdesc->length = payload_size;
    }
    return(igrp2);
}

/*
 * dual_last_peer_deleted
 *
 * Handle the deletion of the last peer on an interface.
 * All of the peers have already been cleaned up, and with them all of
 * the outstanding unicast packets.
 *
 * We need to clean up the IIDB queues, which will in turn undo all the
 * linkages.  Once we do that, all that should remain is the transmit
 * anchor (pointing at unpacketized DNDBs).  We then walk the thread,
 * dropping the refcount on everything to show that we're not going to
 * send 'em.
 */
void dual_last_peer_deleted (ddbtype *ddb, igrp2_idbtype *iidb)
{
    igrp2_xmit_thread *thread, *next_thread;
    igrp2_xmit_anchor *anchor;
    dual_ndbtype *dndb;

    IGRP2_XBUGINF(XDBPEERDOWN, "\nLast peer deleted from %s",
		  iidb->idb->namestring);
    dual_log_xmit_all(ddb, DUALEV_LASTPEER, &iidb->idb, NULL);

    /* Make sure there are no straggling half-sent Updates. */

    dual_finish_update_send(ddb);

    /* Clean up the IIDB queues. */

    igrp2_flush_iidb_xmit_queues(ddb, iidb);
    if (ddb->last_peer_deleted)
	(*ddb->last_peer_deleted)(ddb, iidb);

    /*
     * Walk down the thread from the transmit anchor, reducing the
     * refcount by 1 on each entry.  This may clear the DNDB sendflag
     * and in turn free the DNDB.
     */
    anchor = iidb->iidb_xmit_anchor;
    if (anchor) {
	thread = anchor->xa_thread;
	while (thread) {
	    next_thread = thread->xmit_next;
	    if (!thread->xt_dummy && !thread->xt_drdb) {
		dndb = dual_thread_to_dndb(thread);
		IGRP2_XBUGINF(XDBPEERDOWN|XDBDETAIL,
			      "\n  Dropping refcount on %s",
			      (*ddb->printnet)(&dndb->dndb_dest));
		dual_drop_dndb_refcount(ddb, thread, NULL, iidb);
	    }
	    thread = next_thread;
	}

	/* Free the anchor whilst here. */

	dual_unanchor_entry(ddb, anchor->xa_thread);
	iidb->iidb_xmit_anchor = NULL;
    }

    /* Note that the interface is now inactive. */

    ddb->ddb_active_if_count--;

    /* Remove the interface from the quiescent list. */

    dual_remove_iidb_from_quiescent_list(ddb, iidb);
}

/*
 * dual_first_peer_added
 *
 * Handle the addition of the first peer on an interface.
 */
void dual_first_peer_added (ddbtype *ddb, igrp2_idbtype *iidb)
{
    dual_log_xmit_all(ddb, DUALEV_FIRSTPEER, &iidb->idb, NULL);
    ddb->ddb_active_if_count++;		/* Note that the interface is active */
    dual_add_iidb_to_quiescent_list(ddb, iidb);	/* Show that it's ready */
}

/*
 * dual_successor
 * Extract the successor from the ndb.
 */
static inline dual_rdbtype *dual_successor (dual_ndbtype *dndb)
{
    return(dndb->dndb_succ ? dndb->dndb_succ : dndb->dndb_rdb);
}


static const char state_transition[] = "\nDUAL: Going from state %d to state %d";

/*
 * dual_state
 * Routine to change the state of a dual destination.
 */

static void dual_state (ddbtype *ddb, dual_ndbtype *dndb, dual_qo_t state)
{
    IGRP2_FABUGINF(&dndb->dndb_dest)(state_transition, dndb->query_origin,
				     state);
    DUAL_LOG(ddb, DUALEV_STATE, dndb->dndb_dest, dndb->query_origin, state);
    dndb->query_origin = state;
}

/*
 * dual_clearhandle
 * Clear the bit in the reply-status table when we receive a reply.
 */
static void dual_clearhandle (ddbtype *ddb, dual_ndbtype *dndb, int handle)
{
    ulong handle_count;

    if (handle == NO_PEER_HANDLE)
	return;

    if (igrp2_test_handle(ddb, &dndb->dndb_reply_status, handle)) {
	igrp2_clear_handle(ddb, &dndb->dndb_reply_status, handle);
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Clearing handle %d, "
					 "count is now %d", handle,
					 dndb->dndb_reply_status.handles_used);
	handle_count = dndb->dndb_reply_status.handles_used;
	DUAL_LOG(ddb, DUALEV_CLEARHANDLE1, dndb->dndb_dest, dndb->dndb_dest,
		 handle_count);
	DUAL_LOG(ddb, DUALEV_CLEARHANDLE2, dndb->dndb_dest, handle,
		 dndb->dndb_reply_status.handle_array[0]);
    }
}

/*
 * This is only a temporary message for debugging purposes.
 */
static const char dualnoentry[] =
    "\nDUAL: No entry for destination %s in topology table.";

/*
 * dual_promote
 * Promote the passed in rdb in the dndb's rdb chain.
 */

static void dual_promote (ddbtype *ddb, dual_ndbtype *dndb, dual_rdbtype *drdb)
{
    dual_rdbtype *cur_rdb, **prev;
    ulong count;

    /*
     * Find the DRDB and delink it from its current position.
     */
    prev = &dndb->dndb_rdb;
    while (*prev && *prev != drdb)
	prev = &((*prev)->drdb_next);
    if (*prev == NULL) {
	IGRP2_FABUGINF(&dndb->dndb_dest)(dualnoentry,
					 (*ddb->printnet)(&dndb->dndb_dest));
	return;
    }
    *prev = drdb->drdb_next;

    /*
     * Now insert the entry into the chain.  Entries are sorted by
     * reported distance.  All external entries come after all internal
     * entries.
     */
    prev = &dndb->dndb_rdb;
    cur_rdb = dndb->dndb_rdb;
    if (dndb->dndb_successors) {	/* Be paranoid. */
	count = dndb->dndb_successors - 1; /* Count of *other* successors */
	while (count && cur_rdb) {	/* Walk all successors. */
	    
	    /*
	     * If we're internal and the next is external, we're there.
	     * Otherwise, if external flag matches and ours is closer than
	     * the next, we're there.
	     */
	    if ((!drdb->drdb_external && cur_rdb->drdb_external) ||
		((drdb->drdb_external == cur_rdb->drdb_external) &&
		 (drdb->drdb_successor_metric <=
		  cur_rdb->drdb_successor_metric))) {
		break;			/* Go link it in. */
	    }
	    prev = &cur_rdb->drdb_next;
	    cur_rdb = cur_rdb->drdb_next;	/* Move along. */
	    count--;
	}
    }

    /* We're at our link point.  Link it in. */

    drdb->drdb_next = *prev;
    *prev = drdb;
}

/*
 * dual_rtupdate
 * Update routing table.  Update successors cell in the ndb of the topology
 * table.  Possibly promote the newly installed successor in the rdb chain.
 */
static void dual_rtupdate (ddbtype *ddb, dual_ndbtype *dndb,
			   dual_rdbtype *drdb)
{
    dual_rdbtype *first_rdb;
    boolean success, promote;

    /*
     * Don't update routing table if it is a directly connected or a
     * redistributed route destination.  It was already done
     * long before we got here.  Also, don't let anything override a
     * connected route.  True we shouldn't have more than one of these
     * in the rdb chain at once, but at least this keeps us from doing
     * something stupid like sending perpetual updates fedback by a
     * neighboring router.
     *
     * Summary rdbs and redistributed rdbs may coexist in the rdb chain,
     * and could also lead to perpetual update feedback.
     */
    success = FALSE;
    first_rdb = dndb->dndb_rdb;
    switch (drdb->drdb_origin) {
    case IGRP2_ORG_CONNECTED:
	dndb->dndb_successors = 1;
	/*
	 * Let's avoid the problem of more than one connected path to
	 * a destination with different metrics causing an update
	 * storm because of a changing metric.  Keep the lowest metric
	 * at the top of the list.  The same goes for redistrubuted
	 * below.
	 */
	if (first_rdb->drdb_origin == IGRP2_ORG_CONNECTED) {
	    if (dndb->dndb_metric <= drdb->drdb_metric) {
		success = FALSE;
		break;
	    }
	}
	success = TRUE;
	break;
    case IGRP2_ORG_REDISTRIBUTED:
	dndb->dndb_successors = 1;
	/*
	 * Promote redistributed rdbs, except over connected rdbs
	 */
	if (first_rdb->drdb_origin != IGRP2_ORG_CONNECTED) {
	    /*
	     * N.B. See the comment in the connected case above.
	     */
	    if (first_rdb->drdb_origin == IGRP2_ORG_REDISTRIBUTED) {
		if (dndb->dndb_metric <= drdb->drdb_metric) {
		    success = FALSE;
		    break;
		}
	    }
	    success = TRUE;
	} else {
	    success = FALSE;
	}
	break;
    case IGRP2_ORG_SUMMARY:
    case IGRP2_ORG_IGRP2:
	success = (*ddb->rtupdate)(ddb, dndb, drdb, &promote);
	success = success && promote;
	break;
    }

    /*
     * We need to determine if *_rtupdate() indicated it should
     * be promoted in the rdb chain.
     */
    if (success) {
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: RT installed %s via %s",
				    (*ddb->printnet)(&dndb->dndb_dest),
				    (*ddb->printaddr)(&drdb->drdb_nexthop));
	DUAL_LOG(ddb, DUALEV_RTINSTALL, dndb->dndb_dest, dndb->dndb_dest,
		 drdb->drdb_nexthop);
	dual_promote(ddb, dndb, drdb);
    }
}

/*
 * dual_rdbclear
 * Silently remove the rdb entry from the topology table.
 *
 * If the DNDB is deleted, the supplied pointer will be nulled out.
 */
static void dual_rdbclear (ddbtype *ddb, dual_ndbtype **dndbp,
			   dual_rdbtype *drdb)
{
    dual_ndbtype *dndb;

    dndb = *dndbp;
    dual_zapdrdb(ddb, dndb, drdb, FALSE);
    if ((dndb->dndb_rdb == NULL) && (dndb->dndb_send_flag == 0)) {
	dual_dndbdelete(ddb, dndbp);
    }
}

/*
 * dual_active
 * Create a mini reply-status table for this destination only.  Bump
 * the count in the ndb and flag multicast queries to be sent.
 *
 * query_drdb points to the DRDB for the guy that sent us a query if we're
 * going active upon receipt of a query.
 *
 * Returns TRUE if we've gone active, FALSE if it was suppressed for
 * some reason.
 */
static const char dual_activestring[] = "\nDUAL: Dest %s %sentering active state.";

static boolean dual_active (ddbtype *ddb, dual_ndbtype *dndb,
			    dual_rdbtype *query_drdb)
{
    dual_rdbtype *drdb;
    int peer_count;
    igrp2_idbtype *iidb, *split_horizon_iidb;
    igrp2_handle_type *iidb_handles, *dndb_handles, *ddb_handles;
    boolean split_horizon;
    ulong zero = 0;
    ulong one = 1;
    ulong reply_status_bytes;
    ulong index;

    ddb_handles = &ddb->ddb_handles;
    dndb_handles = &dndb->dndb_reply_status;

    if (!ddb_handles->handles_used) { /* No peers at all */
	IGRP2_FABUGINF(&dndb->dndb_dest)(dual_activestring,
					 (*ddb->printnet)(&dndb->dndb_dest),
					 "not ");
	DUAL_LOG(ddb, DUALEV_NOTACTIVE, dndb->dndb_dest, dndb->dndb_dest,
		 zero);
	return(FALSE);
    }

#ifdef DUAL_SPLIT_HORIZON_QUERIES
    /*
     * If we were about to send an update, AND we were going to
     * temporarily disable split horizon, then we MUST continue to
     * disable it, even in this case.  Failure to do so would result
     * in old stale information in some neighboring topology tables.
     * This is the same case that shows up in _routeupdate().  See
     * that function for more info.
     */
    if (dndb->dndb_send_flag & DUAL_MULTMASK) {
#ifdef notdef				/* dkatz */
	/*
	 * Clear the update flag when we know for certain we aren't going
	 * active.  That's about the time we set the query flag.
	 */
	split_horizon = dndb->dndb_split_horizon;
#else
	/* Always suppress split horizon if anything's happening. */

	split_horizon = FALSE;
#endif
    } else {
	split_horizon = TRUE;
    }
#else					/* DUAL_SPLIT_HORIZON_QUERIES */
    split_horizon = FALSE;
#endif					/* DUAL_SPLIT_HORIZON_QUERIES */
    drdb = dual_successor(dndb);

    /*
     * Figure out how many peers need to receive this query.  We check
     * the "dndb_being_sent" flag in anticipation of the "dndb_tell_everyone"
     * flag being set at the end of this routine.  We want to suppress
     * split horizon in this case.
     */
    peer_count = ddb_handles->handles_used; /* Total peer count */
    split_horizon_iidb = NULL;
    if (drdb && !dndb->dndb_being_sent && split_horizon) {
	iidb = drdb->drdb_iidb;
	if (iidb) {
	
	    /* Is split horizon enabled on this interface? */
	    
	    if (iidb->split_horizon) {
		
		/*
		 * Are we processing a query from our successor?
		 */
		if (query_drdb) {
		    if (query_drdb == drdb) {
			split_horizon_iidb = iidb;
		    }
		} else {
		    
		    /*
		     * We aren't processing a query.  Are we in multiple
		     * diffusing computations for this dest and did we
		     * receive a query from our successor while active?
		     */
		    if (dndb->query_origin == DUAL_QOMULTI) {
			split_horizon_iidb = iidb;
		    }
		}
	    }
	}
    }

    /* If there's a split horizoned interface, subtract those peers. */

    if (split_horizon_iidb) {
	peer_count -= igrp2_peer_count(split_horizon_iidb);
    }
    dndb->dndb_query_sh_if = split_horizon_iidb; /* Remember it for later. */

    /*
     * Return false if split horizon nailed this destination
     */
    if (peer_count == 0) {
	IGRP2_FABUGINF(&dndb->dndb_dest)(dual_activestring,
					 (*ddb->printnet)(&dndb->dndb_dest),
					 "not ");
	DUAL_LOG(ddb, DUALEV_NOTACTIVE, dndb->dndb_dest, dndb->dndb_dest, one);
	return(FALSE);
    }

    /* Alright, we've got somebody to query.  Create a reply status table. */

    if (dual_dndb_active(dndb)) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }

    reply_status_bytes = HANDLE_MALLOC_SIZE(ddb_handles->handle_array_size);
    dndb_handles->handle_array = malloc_named(reply_status_bytes,
					      "Dual reply status");
    if (dndb_handles->handle_array == NULL) {
	return(FALSE);
    }
    dndb_handles->handle_array_size = ddb_handles->handle_array_size;

    /* First set bits for all of the peers. */

    bcopy(ddb_handles->handle_array, dndb_handles->handle_array,
	  reply_status_bytes);
    dndb_handles->handles_used = ddb_handles->handles_used;

    /*
     * Now clear all of the bits for peers on the split horizoned interface,
     * if there is one.
     */
    if (split_horizon_iidb) {
	iidb_handles = &split_horizon_iidb->iidb_handles;
	if (iidb_handles->handle_array_size >
	    dndb_handles->handle_array_size) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return(FALSE);
	}
	for (index = 0; index < iidb_handles->handle_array_size; index++) {
	    dndb_handles->handle_array[index] &=
		~(iidb_handles->handle_array[index]);
	}
	dndb_handles->handles_used -= iidb_handles->handles_used;
    }

    /*
     * Guess we ARE going active after all.  We may have a pending update
     * that needs to be cleared.
     */
    dndb->dndb_send_flag &= ~DUAL_MULTMASK;
    dndb->dndb_send_flag |= DUAL_MULTQUERY;
    dual_xmit_dndb(ddb, &dndb);
    if (dndb == NULL) {			/* Can't happen; we have peers */
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }
    IGRP2_FABUGINF(&dndb->dndb_dest)(dual_activestring,
				(*ddb->printnet)(&dndb->dndb_dest), "");
    DUAL_LOG(ddb, DUALEV_ACTIVE, dndb->dndb_dest, dndb->dndb_dest, peer_count);

    /*
     * There is an implicit assumption elsewhere that we only have 1
     * former feasible successor while active.  Specifically, in
     * dual_splithorizon.  This is of course assuming this successor
     * isn't just now in the process of going away.  The caller
     * dual_peerdown() has set this to zero in this case and please
     * don't touch it.
     */
    if (dndb->dndb_successors != 0)
	dndb->dndb_successors = 1;

    /*
     * We need to have this here so that when we get ready to send the
     * query and we have changed the query-origin flag from one of the
     * two local states to multiple successor origin before the query
     * goes out we loose track of whether we should split horizon.
     * This flag ensures this does not happen.
     *
     * Note: A little history behind the above comment.  At one time,
     * _getnext() used to use the query-origin flag to determine when
     * to disable split horizon.  The problem with that is explained
     * above.  Therefore we needed an additional flag (stack variable
     * split_horizon) to force this state regardless of the state
     * changes between now and when we eventually get around to
     * sending the query.
     */
    dndb->dndb_split_horizon = (split_horizon_iidb != NULL);

    /*
     * IGRP2_FBUGINF()
     */
    if (IGRP2_FTARGET(&dndb->dndb_dest)) {
	boolean local_split_horizon;	/* So dual_log can take a pointer */
	buginf("\nDUAL: Set reply-status table. Count is %d.",
	       dndb_handles->handles_used);
	DUAL_LOG(ddb, DUALEV_SETREPLY, dndb->dndb_dest,
		 dndb_handles->handles_used, dndb_handles->handle_array[0]);
	buginf("\nDUAL: %soing split horizon", dndb->dndb_split_horizon ?
	       "D" : "Not d");
	if (dndb->dndb_split_horizon && split_horizon_iidb)
	    buginf(" on %s", split_horizon_iidb->idb->namestring);
	local_split_horizon = dndb->dndb_split_horizon;
	DUAL_LOG(ddb, DUALEV_SPLIT, dndb->dndb_dest, 
		 local_split_horizon,  split_horizon_iidb);
    }

    return(TRUE);
} 

/*
 * dual_ndblookup
 * Find the matching ndb in the topology table.
 */

dual_ndbtype *dual_ndblookup (ddbtype *ddb, igrp2nettype *dest)
{
    dual_ndbtype *dndb;

    dndb = ddb->topology[(*ddb->ndbbucket)(dest)];
    while (dndb) {
	if ((*ddb->netmatch)(dest, &dndb->dndb_dest))
	    return(dndb);
	dndb = dndb->dndb_next;
    }
    return(NULL);
}

/*
 * dual_rdblookup
 * Find the matching rdb in the topology table.  Return NULL if not found
 * or the ndb is NULL.
 */

static dual_rdbtype *dual_rdblookup (ddbtype *ddb,
				     dual_ndbtype *dndb,
				     igrp2_idbtype *iidb,
				     igrp2addrtype *nexthop)
{
    dual_rdbtype *drdb;

    if (dndb == NULL)
	return(NULL);
    drdb = dndb->dndb_rdb;
    while (drdb) {
	if ((iidb == drdb->drdb_iidb) &&
	    (*ddb->addrmatch)(nexthop, &drdb->drdb_nexthop))
	    return(drdb);
	drdb = drdb->drdb_next;
    }
    return(NULL);
}

/*
 * dual_sendreply
 * Get set for a reply
 */

static void dual_sendreply (ddbtype *ddb, dual_ndbtype *dndb,
			    dual_rdbtype *drdb)
{
    if ((dndb == NULL) || (drdb == NULL))
	return;

    drdb->drdb_send_flag |= DUAL_SENDREPLY;
    dndb->dndb_send_flag |= DUAL_SENDREPLY; /* Don't let it get freed! */
    dual_xmit_drdb(ddb, drdb);
    IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Send reply about %s to %s",
				(*ddb->printnet)(&dndb->dndb_dest),
				(*ddb->printaddr)(&drdb->drdb_nexthop));
    DUAL_LOG(ddb, DUALEV_SENDREPLY, dndb->dndb_dest, dndb->dndb_dest,
	     drdb->drdb_nexthop);
}

/*
 * dual_setFD
 * Return FD, not ever allowing zero.  This way connected routes pass
 * the feasibility condition test without a special check.
 *
 * This is not the case any longer.  We now make certain that the computed
 * metrics are never less than one.  This seems like a safer bet anyway.
 */
static inline void dual_setFD (dual_ndbtype *dndb, ulong metric)
{
    dndb->dndb_old_metric = metric;
}

/*
 * dual_maxRD
 * Set RD to a metric of infinity
 */
static inline void dual_maxRD (ddbtype *ddb, dual_ndbtype *dndb)
{
    dndb->dndb_metric = METRIC_INACCESSIBLE;
    dndb->dndb_vecmetric.delay = METRIC_INACCESSIBLE;
    dual_log(ddb, DUALEV_SETRD, &dndb->dndb_dest, &dndb->dndb_dest,
	     &dndb->dndb_metric);
}

/*
 * dual_setRD
 * Set the reported distance so that we report the correct metric when
 * the next update, query, or reply is sent.
 */
static inline ulong dual_setRD (ddbtype *ddb, dual_ndbtype *dndb)
{
    ulong metric;

    metric = dndb->dndb_rdb->drdb_metric;
    dndb->dndb_metric = metric;
    dndb->dndb_vecmetric = dndb->dndb_rdb->drdb_vecmetric;
    dual_log(ddb, DUALEV_SETRD, &dndb->dndb_dest, &dndb->dndb_dest,
	     &dndb->dndb_metric);
    return(metric);
}

/*
 * dual_compare_drdb
 * compare incoming drdb
 *
 * Returns FALSE if the incoming route is identical to the DRDB, or TRUE
 * if it is not (meaning that we need to take action).
 */
static boolean dual_compare_drdb (ddbtype *ddb, dual_newroute *route,
				  dual_rdbtype *drdb)
{
    if (ddb->compare_external != NULL) {
	if (!(*ddb->compare_external)(route->new_data, drdb->drdb_data))
	    return (TRUE);
    }

    if ((drdb->drdb_metric == route->new_metric) &&  
	(drdb->drdb_vecmetric.delay == route->new_vecmetric.delay) &&
	(drdb->drdb_vecmetric.bandwidth == route->new_vecmetric.bandwidth) &&
	(drdb->drdb_vecmetric.mtu == route->new_vecmetric.mtu) &&
	(drdb->drdb_vecmetric.hopcount == route->new_vecmetric.hopcount) &&
	(drdb->drdb_vecmetric.reliability ==
	 route->new_vecmetric.reliability) &&
	(drdb->drdb_vecmetric.load == route->new_vecmetric.load) &&
	(drdb->drdb_external == route->new_external) &&
	(drdb->drdb_opaque_flags == route->new_opaque_flags) &&
	(drdb->drdb_origin == route->new_origin) &&
	(drdb->drdb_keep == route->new_keep) &&
	(drdb->drdb_ignore == route->new_ignore)) {
	return (FALSE);
    }
    return (TRUE);
}


/*
 * dual_updatetopology
 * Update the topology table with (possibly) new information.  Return
 * neighbors old metric or inaccessible if this is a new entry.  Also, return
 * the old metric (calculated by our point of view) in route->new_metric.
 * Note: route->new_data may contain malloc'd data whose pointer needs to be
 * saved in a drdb, or the data should be freed.
 *
 * Returns a pointer to the dndb and drdb, if any.
 *
 * createjunk is TRUE if we should create a new DNDB even if the destination
 * is to be ignored or is unreachable.
 *
 * If update_needed is non-NULL, it points to a boolean which we will set
 * to be TRUE if an update needs to be sent in response to this destination.
 *
 * If a DRDB is created for this route, created_drdb will be returned TRUE.
 */
static ulong dual_updatetopology (ddbtype *ddb, boolean createjunk,
				  boolean *update_needed, dual_newroute *route,
				  dual_ndbtype **new_dndb,
				  dual_rdbtype **new_drdb,
				  boolean *created_drdb)
#define DUAL_MAXCOPYCHAR 80
{
    char dual_scratch[DUAL_MAXCOPYCHAR];
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    dual_rdbtype **prev;
    ulong old_metric;
    int slot;
    int successors;
    int peer_handle;

    *new_dndb = NULL;
    *new_drdb = NULL;
    if (created_drdb)
	*created_drdb = FALSE;

    /*
     * This little sanity check should be pulled out once the infosource
     * field is fully implemented.  For now they need to be the same.
     */
    if (!(*ddb->addrmatch)(&route->new_nexthop, &route->new_infosource)) {
	sstrncpy(dual_scratch, (*ddb->printaddr)(&route->new_nexthop),
		DUAL_MAXCOPYCHAR - 1);
	errmsg(&msgsym(NOMATCH, DUAL), dual_scratch,
	       (*ddb->printaddr)(&route->new_infosource));
    }

    dndb = dual_ndblookup(ddb, &route->new_dest);
    if (dndb == NULL) {
	if (!createjunk &&
	    (route->new_ignore ||
	     (route->new_metric == METRIC_INACCESSIBLE))) {

	    /*
	     * Received a useless update - ignore it.  Route may have been
	     * administratively prohibited.
	     */
	    dual_log(ddb, DUALEV_IGNORE, &route->new_dest, &route->new_dest,
		     &route->new_metric);
	    dual_freedata(ddb, &route->new_data);
	    return(METRIC_INACCESSIBLE);
	}
	dndb = malloc_named(sizeof(dual_ndbtype), "Dual NDB");
	if (dndb == NULL) {
	    dual_freedata(ddb, &route->new_data);
	    return(METRIC_INACCESSIBLE);
	}

	/*
	 * Fill in the destination address into the ndb.
	 * Set FD and RD to infinity.
	 */
	dndb->dndb_dest = route->new_dest;
	dual_setFD(dndb, METRIC_INACCESSIBLE);
	dual_maxRD(ddb, dndb);
	dndb->dndb_successors = 0;
	dndb->query_origin = DUAL_QOLOCAL;

	/*
	 * This field is used to determine when to send out an update
	 * in the event the metric has not changed, but the {in,ex}ternal
	 * disposition has.  It is set here (initialization), and when 
	 * it is checked (should be in _scanupdate).
	 */
	dndb->dndb_external = route->new_external;

	/*
	 * If the client protocol wants to know that we have created a
	 * new network, tell it now.
	 */
	if (ddb->ndb_new != NULL) {
	    (*ddb->ndb_new)(dndb, dual_idb(route->new_iidb));
	}
	/*
	 * Setup hash bucket pointing to dndb and link it into the chain.
	 */
	slot = (*ddb->ndbbucket)(&route->new_dest);
	dndb->dndb_next = ddb->topology[slot];
	ddb->topology[slot] = dndb;
    } else {
	/*
	 * Write down what the opaque flags used to be set to before
	 * revising topology table.
	 */
	if (ddb->exterior_flag)
	    dndb->dndb_opaque_flags_old = (*ddb->exterior_flag)(dndb);
    }
    *new_dndb = dndb;

    drdb = dual_rdblookup(ddb, dndb, route->new_iidb, &route->new_nexthop);
    if (drdb == NULL) {
	drdb = malloc_named(sizeof(dual_rdbtype), "Dual RDB");
	if (drdb == NULL) {
	    dual_freedata(ddb, &route->new_data);
	    return(METRIC_INACCESSIBLE);
	}
	if (created_drdb)
	    *created_drdb = TRUE;
	drdb->drdb_thread.xt_drdb = TRUE;
	drdb->drdb_dndb = dndb;
	drdb->drdb_iidb = route->new_iidb;
	if (drdb->drdb_iidb)
	    mem_lock(drdb->drdb_iidb);
	drdb->drdb_successor_metric = METRIC_INACCESSIBLE;
	drdb->drdb_handle = NO_PEER_HANDLE;

	/*
	 * Fill in next hop and infosource.
	 */
	drdb->drdb_nexthop = route->new_nexthop;
	drdb->drdb_infosource = route->new_infosource;

	/*
	 * Now link it into the chain and set the old metric correctly.
	 * Be careful not to insert ahead of our current best cost
	 * successor(s) if any exists.  Also, if we are active, we may
	 * not be able to depend on the setting of successors.  This
	 * being the case, insert this new information after the first
	 * entry if we are active.
	 */
	prev = &dndb->dndb_rdb;
	if (!dual_dndb_active(dndb)) {
	    successors = dndb->dndb_successors;
	    while (successors--) {
		if (!*prev) {
		    errmsg(&msgsym(BADCOUNT, DUAL),
			   (*ddb->printnet)(&route->new_dest),
			   dndb->dndb_successors,
			   dndb->dndb_successors - successors - 1);
		    break;
		}
		prev = &((*prev)->drdb_next);
	    }
	} else {
	    if (dndb->dndb_rdb != NULL) {
		prev = &((*prev)->drdb_next);
	    } else if (dndb->dndb_successors > 0) {
		errmsg(&msgsym(BADCOUNT, DUAL),
		       (*ddb->printnet)(&route->new_dest),
		       dndb->dndb_successors, 0);
	    }
	}

	drdb->drdb_next = *prev;
	*prev = drdb;
    }

    /*
     * Update the peer handle.  It should only be changing if the DRDB
     * handle was previously NO_PEER_HANDLE (either it's new, or it's
     * an old DRDB that was still active when our peer went down, and who
     * has now risen from the grave).
     */
    if (route->new_iidb) {
	peer_handle = igrp2_findhandle(ddb, &route->new_nexthop,
				       dual_idb(route->new_iidb));
	if (drdb->drdb_handle != peer_handle) {
	    if (drdb->drdb_handle != NO_PEER_HANDLE) {
		errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
		return(0);
	    }
	    drdb->drdb_handle = peer_handle;
	}
    } else {
	if (drdb->drdb_handle != NO_PEER_HANDLE) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return(0);
	}
    }

    *new_drdb = drdb;

    /* 
     * A comparison of the pending drdb.  If it is an exact copy
     * of an existing drdb in the topology table, don't bother
     * linking it in - This is also fix for CSCdi40200.
     */
    if (update_needed && *update_needed) {
	*update_needed = dual_compare_drdb(ddb, route, drdb);
    }

    /*
     * Indicate whether we should ignore this peer in dual computations.
     */
    drdb->drdb_ignore = route->new_ignore;

    /*
     * Fill in composite metrics, igrp vector, and external data, if any.
     */
    drdb->drdb_metric = route->new_metric;
    old_metric = drdb->drdb_successor_metric;
    drdb->drdb_successor_metric = route->new_successor_metric;
    drdb->drdb_vecmetric = route->new_vecmetric;
    drdb->drdb_external = route->new_external;
    drdb->drdb_opaque_flags = route->new_opaque_flags;
    drdb->drdb_origin = route->new_origin;
    drdb->drdb_keep = route->new_keep;
    /*
     * Replace any existing drdb->drdb_data with any new data which has been
     * temporarily hung off the ddb.
     */
    dual_freedata(ddb, &drdb->drdb_data);
    drdb->drdb_data = route->new_data;

    return(old_metric);
}

/*
 * dual_scanupdate
 * Scan the rdb chain and attempt to install into the routing table.
 */
static void dual_scanupdate (ddbtype *ddb,  dual_ndbtype *dndb)
{
    dual_rdbtype *drdb;
    dual_rdbtype *succ;
    dual_rdbtype *next;
    idbtype *interface;
    boolean up;
    uchar opaque_flags_new;
    uchar *update_reason;

    /*
     * Initialize successor count, to avoid relying upon protocol dependent
     * update routine to set it when no paths are in the routing table.  Also,
     * remember who our old successor was so before updating the routing
     * table so in case the routing table selects a non feasible successor
     * we keep the SDAG intact when going active.
     */
    drdb = dndb->dndb_rdb;
    succ = dndb->dndb_rdb;
    dndb->dndb_successors = 0;
    interface = NULL;
    up = TRUE;
    while (drdb) {

	/*
	 * Remember next rdb in case we promote this route when we
	 * attempt to install it.
	 */
	next = drdb->drdb_next;
	if (up && !drdb->drdb_ignore) {
	    dual_rtupdate(ddb, dndb, drdb);
	}
	drdb = next;
    }

    if (dndb->dndb_successors == 0) {
	/*
	 * No successors?  If we have a summary rdb, consider that to be
	 * a feasible successor even though it wasn't accepted by the
	 * protocol specific routing table.
	 */
	for (drdb = dndb->dndb_rdb; drdb; drdb = drdb->drdb_next) {
	    if ((drdb->drdb_origin == IGRP2_ORG_SUMMARY) &&
		(drdb->drdb_metric != METRIC_INACCESSIBLE)) {
		dndb->dndb_successors = 1;
		IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Forced promotion of"
						 " summary %s",
						 (*ddb->printnet)
						 (&dndb->dndb_dest));
		DUAL_LOG(ddb, DUALEV_FPROMOTE, dndb->dndb_dest,
			 dndb->dndb_dest,
			 drdb->drdb_metric);
		dual_promote(ddb, dndb, drdb);
		break;
	    }
	}
    }
    /*
     * See if the PDM has a reason for us to send an update.
     */
    if (ddb->exterior_check != NULL)
	(*ddb->exterior_check)(ddb, dndb, &opaque_flags_new);
    else
	opaque_flags_new = 0;
#ifdef notdef				/* Bob says this isn't necessary */
    /*
     * If the new routing table best metric is not a feasible
     * successor, we must set FD to zero order to go active.  We must
     * check for FD set to infinity otherwise feasibility condition
     * check will return true.  This is NOT what we want in this case.
     * There is no other way to increase the value of FD.  Set succ to
     * the old successor so that when we transition from active to
     * passive, we know where to send the last reply.
     */
    if ((dndb->dndb_successors > 0) &&
	(dndb->dndb_old_metric != METRIC_INACCESSIBLE)) {
	drdb = dndb->dndb_rdb;
	if (drdb && (drdb->drdb_metric != METRIC_INACCESSIBLE) &&
	    (drdb->drdb_origin == IGRP2_ORG_IGRP2) && 
	    (drdb->drdb_successor_metric >= dndb->dndb_old_metric)) {

	    /*
	     * Force active state by setting FD to zero.
	     */
	    dual_setFD(dndb, 0);
	    dndb->dndb_succ = succ;
	    return;
	}
    }
#endif

    update_reason = NULL;
    /*
     * This is the case where the destination is about to go away and
     * we are going to tell the world about it.
     */
    if (dndb->dndb_rdb == NULL) {
	update_reason = "rt gone";
    } else {

	/*
	 * The external field in the ndb is only checked here to
	 * determine when to send an update in the case of the path
	 * changing {in,ex}ternal disposition.  It is also checked in
	 * the caller _routeupdate().  It is only set upon
	 * initialization (should be in _updatetopology) and here
	 * after the destination is updated.
	 */
	if (dndb->dndb_rdb->drdb_external != dndb->dndb_external) {
	    dndb->dndb_external = dndb->dndb_rdb->drdb_external;
	    if (dndb->dndb_external)
		update_reason = "rt now ext";
	    else
		update_reason = "rt now int";
	} else {
	    /*
	     * Check for other reasons why we need to send an update.
	     *  - the metric has changed
	     *  - PDM flag information has changed
	     *  - we used to have a feasible successor but now we don't
	     */
	    if ((dndb->dndb_old_metric != METRIC_INACCESSIBLE) &&
		(dndb->dndb_successors == 0)) {
		update_reason = "rt now infea";
	    } else if (dndb->dndb_rdb->drdb_metric != dndb->dndb_metric) {
		update_reason = "metric chg";
	    } else if (dndb->dndb_opaque_flags_old != opaque_flags_new) {
		if (opaque_flags_new)
		    update_reason = "rt now ext2";
		else
		    update_reason = "rt now int2";
	    }
	}
    }
    if (update_reason)
	dual_sendupdate(ddb, dndb, update_reason);
}

/*
 * dual_routeupdate
 *
 * Do some cleanup and attempt to install destination in routing table.
 * This only happens when we've found a feasible successor and
 * remain in passive state.  Send an update about this destination if the
 * old best metric is different than the new best metric.
 *
 * The DNDB pointer is nulled out if the DNDB is freed.
 *
 * This routine needs to be to fixed to cleanup the routing
 * table routes before installing new routes due to the problem
 * created when the metric increases.  Try to figure out a way to avoid
 * scanning the rdb's twice, once to delete and once to add. $$$
 */
static void dual_routeupdate (ddbtype *ddb, dual_ndbtype **dndbp)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    dual_rdbtype **prev_drdb;
    int successors;
    int iidb_count;
    int iidb_ix;
    boolean diddle_split_horizon;
    boolean drdb_deleted;

    dndb = *dndbp;

    if (!ddb->ddb_scratch)
	return;				/* We're so hosed it's not funny */

    /*
     * This function sets the split_horizon flag in the dndb.  It is
     * used by _getnext() to determine when to ignore split horizon.
     * We must send updates with infinity metric over split horizoned
     * interfaces when we acquire a new interface.  This function first
     * saves interfaces which are currently in use, then checks it against
     * interfaces in use once the routing table has been updated.  If
     * there are any new interfaces that weren't used before, split horizon
     * is temporarily disabled.  However, if we find ourselves running 
     * through this routine more than once before the update is sent,
     * we could find ourselves in a position of nullifying this effect.
     * Take care to leave split_horizon disabled if the update send_flag
     * is set when we enter this routine.
     *
     * Yet another condition for disabling split horizon.  If dndb->dndb_succ
     * is non-NULL, we may have changed interfaces before going active.
     * Now that we are passive that is the only indication that this has
     * occured and we must disable split horizon without even having to
     * scan all the interfaces for it.  Set the diddle flag accordingly.
     */
    if (dndb->dndb_succ != NULL) {
	diddle_split_horizon = FALSE;
	dndb->dndb_split_horizon = FALSE;
	dndb->dndb_succ = NULL;
    } else {
	if ((dndb->dndb_send_flag & DUAL_MULTMASK) &&
	    !dndb->dndb_split_horizon) {
	    diddle_split_horizon = FALSE;
	} else {
	    diddle_split_horizon = TRUE;
	}
    }

    drdb = dndb->dndb_rdb;
    prev_drdb = &dndb->dndb_rdb;
    successors = dndb->dndb_successors;
    iidb_count = 0;
    while (drdb) {
	drdb_deleted = FALSE;

	/*
	 * First, squirrel away the known interfaces used to get
	 * to successors.
	 */
	if ((successors-- > 0)) {
	    
	    /* Look for an existing match. */
	    
	    for (iidb_ix = 0; iidb_ix < iidb_count; iidb_ix++) {
		if (ddb->ddb_scratch[iidb_ix].iidb == drdb->drdb_iidb)
		    break;
	    }
	    
	    /* If no match, add the new one. */
	    
	    if (iidb_ix == iidb_count) {
		ddb->ddb_scratch[iidb_count].iidb = drdb->drdb_iidb;
		ddb->ddb_scratch[iidb_count].found = FALSE;
		iidb_count++;
	    }
	}

	/*
	 * See if this route is marked with infinity.  If so,
	 * remove it and move on.
	 */
	if (drdb->drdb_metric == METRIC_INACCESSIBLE) {

	    /*
	     * But only remove the routing table entry if we have something
	     * to say to this link about this destination.
	     */
	    if (drdb->drdb_send_flag == 0) {
		dual_zapdrdb(ddb, dndb, drdb, TRUE);
		drdb_deleted = TRUE;
	    } else {
		dual_rtdelete(ddb, dndb, drdb);
	    }

	    /*
	     * Only delete entries when the metric gets worse OR the route
	     * changes internal/external disposition.  This minimizes the
	     * number of times routing table caches get invalidated.  If
	     * it is a simple matter of a metric decrease from the same
	     * nexthop then it makes no sense to upset the fast switching
	     * path.
	     */
        } else if (((drdb->drdb_origin == IGRP2_ORG_IGRP2) ||
		    (drdb->drdb_origin == IGRP2_ORG_SUMMARY))) {
#if(0)
		   /*
		    * We need to rethink this part again.  It is
		    * BROKEN!
		   && ((drdb->drdb_metric > dndb->dndb_metric) ||
		    (dndb->dndb_rdb->external != dndb->dndb_external))
		    */
#endif
	    if (ddb->rtmodify)
		(*ddb->rtmodify)(ddb, dndb, drdb);
	}
	if (!drdb_deleted) {
	    prev_drdb = &drdb->drdb_next;
	}
	drdb = *prev_drdb;
    }
    dual_scanupdate(ddb, dndb);
    if ((dndb->dndb_rdb == NULL) && (dndb->dndb_send_flag == 0)) {
	dual_dndbdelete(ddb, dndbp);
	dndb = *dndbp;			/* May be gone! */
    } else {
	/*
	 * If we are forced to go active, then bail.
	 */
	if (dndb->dndb_succ != NULL)
	    return;

	/*
	 * Now, scan the successors to see if we have acquired a new
	 * interface that we haven't used before.  We could find
	 * ourselves having not set send_flag in the dndb yet.  This
	 * can happen because there is information _scanupdate() is
	 * not privy to.  That is when everything remains the same
	 * (metric, flags, etc.) except we change which interface we
	 * are using to get to the destination.  Two cases are
	 * covered, the first case in the loop, as stated above, we
	 * added a new interface.  The second case is when we lose an
	 * interface.  Losing an interface case we don't have to
	 * disable split horizon.  Simply send the update.
	 */
	if (diddle_split_horizon)
	    dndb->dndb_split_horizon = TRUE;
	drdb = dndb->dndb_rdb;
	successors = dndb->dndb_successors;
	while (drdb && (successors-- > 0)) {
	    for (iidb_ix = 0; iidb_ix < iidb_count; iidb_ix++) {
		if (drdb->drdb_iidb == ddb->ddb_scratch[iidb_ix].iidb) {
		    ddb->ddb_scratch[iidb_ix].found = TRUE;
		    break;
		}
	    }

	    /*
	     * Didn't find this interface.  Disable split horizon
	     * and exit the loop.  If we haven't already set the
	     * update flag, do so.  This is the first case described
	     * above.
	     */
	    if (iidb_ix == iidb_count) {
		dndb->dndb_split_horizon = FALSE;
		dual_sendupdate(ddb, dndb, "new if");
		return;
	    }
	    drdb = drdb->drdb_next;
        }
	/*
	 * And this is the second case.  See above.
	 */
	for (iidb_ix = 0; iidb_ix < iidb_count; iidb_ix++) {
	    if (ddb->ddb_scratch[iidb_ix].found == FALSE) {
		dndb->dndb_split_horizon = FALSE; /* Added by DMK. %%% */
		dual_sendupdate(ddb, dndb, "lost if");
		break;
	    }
	}
    }
}

/*
 * dual_setdistance
 * Feasibility condition has been met.  Set FD and RD.  Also, run through
 * the rdb chain zapping entries that are not feasible successors.
 */
static void dual_setdistance (ddbtype *ddb, dual_ndbtype *dndb)
{
    ulong metric;
    dual_rdbtype *drdb;
    
    if (dndb->dndb_succ != NULL) {
	/*
	 * Ouch!  The routing table selected a successor that does
	 * not meet the feasibility condition.  Simply bail and let
	 * the caller do the FC check again.  Of course it will fail
	 * because _rtupdate() has set the old successors to ignore.
	 */
	return;
    }

    drdb = dndb->dndb_rdb;
    if ((drdb != NULL) && (dndb->dndb_successors > 0)) {

	/*
	 * Update RD and FD.
	 */
	metric = dual_setRD(ddb, dndb);
	if (metric < dndb->dndb_old_metric) {
	    dual_setFD(dndb, metric);
	}
    } else {
		 
	/*
	 * At this point, two possibilities exist.  We have no
	 * successors in the routing table or we are about to
	 * delete this route.  Therefore, we probably have
	 * something to send.  If so, set RD to infinity since we
	 * have either no neighbors or no successors to the
	 * destination.  Also set FD to infinity as well for dual.
	 */
	dual_maxRD(ddb, dndb);
#ifdef notdef				/* Bozo no-no: FD increased w/o DC */
	dual_setFD(dndb, METRIC_INACCESSIBLE);
#endif
    }
}

static const char dual_found[] = " found";

/*
 * dual_testFC
 * Check to see if we have a feasible successor for this destination.
 * Either we are about to to active for this dest (depending on the
 * outcome of this routine) or we are comming out of active state.
 * This is the SNC (source node condition) case.
 *  1. Find the minimum metric to the destination.
 *  2. Find all neighbor metrics that are less than FD.
 *  3. Of those neighbors, if our metric through them is equal to the
 *     minimum metric found in 1. above, we have met the feasibility
 *     condition.
 * This seems a bit restrictive.  Why not simply use step two as the
 * feasibility condition?  Because you may not select the minimum
 * distance to the destination.
 */
static boolean dual_testFC (ddbtype *ddb, dual_ndbtype *dndb)
{
    ulong Dmin;
    boolean fc;
    dual_rdbtype *drdb, *best;
    boolean debug;

    debug = IGRP2_FTARGET(&dndb->dndb_dest);

    if (debug) {
	buginf("\nDUAL: Find FS for dest %s. FD is %u, RD is %u",
	       (*ddb->printnet)(&dndb->dndb_dest), dndb->dndb_old_metric,
	       dndb->dndb_metric);
    }
    DUAL_LOG(ddb, DUALEV_SEARCHFS, dndb->dndb_dest, dndb->dndb_dest,
	     dndb->dndb_old_metric);

    if (dndb->dndb_old_metric == METRIC_INACCESSIBLE) {

	/*
	 * Short circuit the test.  If FD is infinity then we can never
	 * fail the condition.
	 */
	if (debug)
	    buginf(dual_found);
	return(TRUE);
    }

    best = NULL;
    drdb = dndb->dndb_rdb;
    fc = FALSE;
    Dmin = METRIC_INACCESSIBLE;
    while (drdb != NULL) {

	/*
	 * Don't consider a feasible successor if the interface to it
	 * is down.  This check is probably done in the protocol dependent
	 * routing table update routines anyway.  We must be consistent.
	 */
	if (!drdb->drdb_ignore && (drdb->drdb_metric <= Dmin)) {
	    if (drdb->drdb_successor_metric < dndb->dndb_old_metric) {
		fc = TRUE;
		if (drdb->drdb_metric < Dmin) {
		    Dmin = drdb->drdb_metric;
		    best = drdb;
		} else if (best == NULL)
		    best = drdb;	/* make sure it points to something */
	    } else {
		if (drdb->drdb_metric < Dmin) {
		    Dmin = drdb->drdb_metric;
		    fc = FALSE;
		}
	    }
	}
	if (debug) {
	    buginf("\nDUAL: \t%s metric %u/%u",
		   (*ddb->printaddr)(&drdb->drdb_nexthop),
		   drdb->drdb_metric, drdb->drdb_successor_metric);
	}
	drdb = drdb->drdb_next;
    }
    if (fc) {
	DUAL_LOG(ddb, DUALEV_FCSAT1, dndb->dndb_dest, best->drdb_nexthop,
		 dndb->dndb_metric);
	DUAL_LOG(ddb, DUALEV_FCSAT2, dndb->dndb_dest, best->drdb_metric,
		 best->drdb_successor_metric);
    } else {
	DUAL_LOG(ddb, DUALEV_FCNOT, dndb->dndb_dest, Dmin, dndb->dndb_metric);
    }
    if (debug) {
	if (fc == FALSE)
	    buginf("not");
	buginf(dual_found);
	buginf(" Dmin is %u", Dmin);
    }
    return(fc);
}

/*
 * dual_FCsatisfied
 * Come here when the feasibility condition has been satisfied.
 *
 * rcv_query is TRUE if we're processing a query
 *
 * The DNDB pointer is NULLed out if the DNDB is freed.
 */
static void dual_FCsatisfied (ddbtype *ddb, dual_ndbtype **dndbp,
			      dual_rdbtype *drdb, boolean rcv_query)
{
    dual_ndbtype *dndb;

    dndb = *dndbp;

    /*
     * Feasibility condition is satisfied.
     */
    switch (dndb->query_origin) {
      case DUAL_QOCLEAR:
	break;
      case DUAL_QOLOCAL:
	if (rcv_query)
	    dual_sendreply(ddb, dndb, drdb);
	break;
      case DUAL_QOMULTI:
      case DUAL_QOSUCCS:
	dual_sendreply(ddb, dndb, dual_successor(dndb));
	break;
      default:
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: query_origin = %d at %d"
				    " for %s from %s",
				    dndb->query_origin, __LINE__,
				    (*ddb->printnet)(&dndb->dndb_dest),
				    (*ddb->printaddr)(&drdb->drdb_nexthop));
	break;
    }

    /*
     * We are simply updating the routing table with probable new
     * information.  We may be able to be smarter about calling
     * routeupdate.
     */
    dual_routeupdate(ddb, dndbp);
    if (*dndbp) {
	dual_setdistance(ddb, *dndbp);
    }
}

/*
 * dual_FC
 *
 * Calculate feasibility condition.
 *
 * rcv_query is TRUE if we're processing a query.
 *
 * Returns TRUE if the condition is satisfied.
 *
 * The DNDB pointer is NULLed out if the DNDB is freed.
 */
static boolean dual_FC (ddbtype *ddb, dual_ndbtype **dndbp, dual_rdbtype *drdb,
			boolean rcv_query)
{
    boolean fc;
    ulong metric;
    dual_ndbtype *dndb;

    dndb = *dndbp;

    fc = dual_testFC(ddb, dndb);
    if (fc == FALSE) {

	/* Try going active. */

	if (!dual_active(ddb, dndb, (rcv_query ? drdb : NULL))) {

	    /*
	     * Didn't go active.  Accept the best cost successor at this point.
	     */
	    /*
	     * Bob thinks that maybe we should just call dual_transition
	     * here.
	     */
	    dual_setFD(dndb, METRIC_INACCESSIBLE);
	    dual_FCsatisfied(ddb, dndbp, drdb, rcv_query);
	    return(TRUE);
	}

	/*
	 * Feasibility condition is not satisfied.
	 */
	if (dndb->dndb_successors == 0) {
	    dual_maxRD(ddb, dndb);
	} else {

	    /*
	     * If FD is set to zero, this is an indication that the
	     * caller wanted to force active state, so here we are.
	     * Therefore, we need to set RD to infinity before
	     * continuing.  This usually happens in lostroute and when
	     * the routing table accepts a non-feasible successor.
	     */
	    if (dndb->dndb_old_metric == 0) {
		dual_maxRD(ddb, dndb);
		metric = dndb->dndb_rdb->drdb_metric;
	    } else {
		metric = dual_setRD(ddb, dndb);
	    }

	    /*
	     * Temp-FD
	     */
	    dual_setFD(dndb, metric);
	}

#ifdef ACTIVE_METRIC_KLUDGE
	dndb->query_metric = dndb->dndb_vecmetric;
#endif

	/*
	 * If we are processing a query from other than our successor,
	 * send a reply.
	 */
	if (drdb && (drdb != dual_successor(dndb)) && rcv_query) {
	    dual_sendreply(ddb, dndb, drdb);
	}
    } else {
	dual_FCsatisfied(ddb, dndbp, drdb, rcv_query);
	dndb = *dndbp;			/* It might be gone. */

	/*
	 * Check to see if the routing table accepted a successor
	 * that does not meet the FC.  If so, run through the
	 * diffusing computation.  Recursion is okay at this point
	 * since we are returning immediately.
	 */
	if (dndb && (dndb->dndb_succ != NULL)) {
	    return(dual_FC(ddb, dndbp, drdb, rcv_query));
	}
    }
    return(fc);
}

/*
 * dual_transition
 *
 * We have no outstanding replies.  Figure out if we should go
 * passive.  In two cases we need to run the feasibility condition
 * again to make a determination whether we should go passive.  If the
 * condition fails, we need to restart the diffusing computation (i.e.,
 * remain active).
 *
 * Note:  Be very careful.  drdb could be set to NULL.
 *
 * The DNDB may be freed;  if so, the pointer is NULLed.
 */
static void dual_transition (ddbtype *ddb, dual_ndbtype **dndbp,
			     dual_rdbtype *drdb)
{
    dual_ndbtype *dndb;
    boolean fc;

    dndb = *dndbp;
    switch (dndb->query_origin) {
      case DUAL_QOCLEAR:
	dual_FC(ddb, dndbp, drdb, FALSE);
	dndb = *dndbp;			/* Maybe freed. */
	if (dndb)
	    dual_state(ddb, dndb, DUAL_QOLOCAL);
	break;

      case DUAL_QOLOCAL:

	/*
	 * Simply select the minimum metric.  Short circuit the FC test
	 * so it passes by setting old_metric to infinity.
	 */
	dual_setFD(dndb, METRIC_INACCESSIBLE);
	dual_FC(ddb, dndbp, drdb, FALSE);
	dndb = *dndbp;
	break;

      case DUAL_QOMULTI:
	fc = dual_FC(ddb, dndbp, drdb, FALSE);
	dndb = *dndbp;
	if (dndb) {
	    if (fc == FALSE) {
		dual_state(ddb, dndb, DUAL_QOSUCCS);
	    } else {
		dual_state(ddb, dndb, DUAL_QOLOCAL);
	    }
	}
	break;

      case DUAL_QOSUCCS:

	/*
	 * Simply select the minimum metric.  Short circuit the FC test
	 * so it passes by setting FD to infinity.
	 */
	dual_setFD(dndb, METRIC_INACCESSIBLE);
	dual_FC(ddb, dndbp, drdb, FALSE);
	dndb = *dndbp;
	if (dndb) {
	    dual_state(ddb, dndb, DUAL_QOLOCAL);
	}
	break;
      default:
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: query_origin = %d at %d"
				    " for %s from %s",
				    dndb->query_origin, __LINE__,
				    (*ddb->printnet)(&dndb->dndb_dest),
				    (*ddb->printaddr)(&drdb->drdb_nexthop));
	break;
    }
}

/*
 * dual_cleanup_reply_status
 *
 * Check reply status table to see if it is empty;  if so, free it and
 * possibly go passive.
 *
 * The DNDB may be freed;  if so, the pointer is NULLed.
 */
static void dual_cleanup_reply_status (ddbtype *ddb, dual_ndbtype **dndbp,
				       dual_rdbtype *drdb)
{
    dual_ndbtype *dndb;

    dndb = *dndbp;
    if (!dndb->dndb_reply_status.handles_used) { /* No replies left */

	/*
	 * Received last reply.  Possibly send reply to old successor.
	 * Go to passive state for this destination.  Stop the active
	 * timer.
	 */
	TIMER_STOP(dndb->dndb_active_time);
	free(dndb->dndb_reply_status.handle_array);
	dndb->dndb_reply_status.handle_array = NULL;
	dndb->dndb_reply_status.handle_array_size = 0;
	dndb->dndb_query_sh_if = NULL;
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Freeing reply status table");
	dual_log(ddb, DUALEV_REPLYFREE, &dndb->dndb_dest, &dndb->dndb_dest,
		 NULL);
	dual_transition(ddb, dndbp, drdb);
    }
}

/*
 * dual_validate_ndb_route
 * Update routing table with information from an ndb, provided we are in
 * passive state.
 */

static void dual_validate_ndb_route (ddbtype *ddb, dual_ndbtype *dndb)
{
    /*
     * Do NOT update the routing table if we are active!
     */
    if (dual_dndb_active(dndb))
	return;
    dual_FC(ddb, &dndb, dndb->dndb_rdb, FALSE);
}

/*
 * The following N routines are the entry points into the dual state
 * machine.  They are used to receive update, query, and reply packets.
 * In addition, they are for detecting one of interface cost changes,
 * link to a neighbor going down and pertinent changes to the routing
 * table.
 */


/*
 * dual_sendupdate
 *
 * Flag a DNDB to be sent as an Update.  Sticks the DNDB onto the change
 * queue.
 *
 * ***NOTE*** dual_finish_update_send() **MUST** be called after this
 * routine is called to flush the change queue.  This is done by littering
 * the PDM with calls to that routine after every call into DUAL (sigh).
 */
void dual_sendupdate (ddbtype *ddb, dual_ndbtype *dndb, uchar *reason)
{

#if (0)
    if ((dndb->dndb_successors == 0) &&
	(dndb->dndb_old_metric == METRIC_INACCESSIBLE)) {
	/*
	 * If FD was already infinity, and the protocol specific routing
	 * table rejected us, then we have no new information to tell our
	 * neighbors.  Supress sending an update.
	 */
	return;
    }
#endif

    /*
     * We may still have the MULTQUERY bit set in the DNDB even though
     * we're passive, since the reply status bits are cleared on the
     * arrival of replies, whereas the MULTQUERY bit is cleared when
     * the query is acknowledged (if the ACK is lost, it may take a bit).
     * Since we can't have both the MULTUPDATE and MULTQUERY bits set,
     * we clear the MULTQUERY bit.
     *
     * If the route is active, however, we just bail, since we will be
     * sending an update when the route goes passive again.
     */
    if (dndb->dndb_send_flag & DUAL_MULTQUERY) {
	if (dual_dndb_active(dndb)) {	/* Active? */
	    return;			/* Wait for it to go passive again. */
	}
	dndb->dndb_send_flag &= ~DUAL_MULTMASK;
    }
    dndb->dndb_send_flag |= DUAL_MULTUPDATE;

    /*
     * Instead of calling dual_xmit_dndb here to thread the DNDB, we instead
     * stick the DNDB onto the change queue;  it will be unqueued shortly
     * when we finish processing the current packet.  (If we're not processing
     * a packet this will happen right away).  This kludge is to group
     * all of the updates together in the case where we're going passive
     * and we need to send a whole buttload of replies and updates (otherwise
     * we'd alternate and it would take a zillion packets).
     */
    if (!dndb->dndb_on_change_queue) {
	*ddb->ddb_change_queue_tail = dndb; /* Add to the tail. */
	ddb->ddb_change_queue_tail = &dndb->dndb_next_chg;
	dndb->dndb_next_chg = NULL;
	dndb->dndb_on_change_queue = TRUE;
    }
    IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Send update about %s.  "
				     "Reason: %s",
				     (*ddb->printnet)(&dndb->dndb_dest),
				     reason);
    dual_log(ddb, DUALEV_UPDATE_SEND, &dndb->dndb_dest, &dndb->dndb_dest,
	     &dndb->dndb_metric);
    dual_log(ddb, DUALEV_UPDATE_SEND2, &dndb->dndb_dest, reason,
	     &dndb->dndb_vecmetric.delay);
}

/*
 * dual_validate_route
 *
 * Update routing table with current information in the topology table,
 * if it exists and we are in passive state.
 */
void dual_validate_route (ddbtype *ddb, igrp2nettype *dest)
{
    dual_ndbtype *dndb;

    dndb = dual_ndblookup(ddb, dest);
    if (dndb == NULL)
	return;
    dual_validate_ndb_route(ddb, dndb);	/* May free the DNDB! */
}

/*
 * dual_reload_proto_table
 * Protocol requests re-populating of its routing table.  Walk through
 * topology table and attempt to re-establish routes.
 */

void dual_reload_proto_table (ddbtype *ddb)
{
    dual_ndbtype *dndb, *next;
    int i;

    if (ddb == NULL)
	return;
    for (i = 0; i < NETHASHLEN; i++) {
	process_may_suspend();
	for (dndb = ddb->topology[i]; dndb; dndb = next) {
	     next = dndb->dndb_next;
	    /*
	     * validate the ndb and see if we need to update the routing
	     * table
	     */
	    dual_validate_ndb_route(ddb, dndb);	/* May free DNDB! */
	}
    }
}

/*
 * dual_rcvupdate
 *
 * Process an update, or a simulation thereof (such as a redistribution
 * event).
 */
static void dual_rcvupdate (ddbtype *ddb, dual_newroute *route)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    ulong CD;			/* Neighbor's old metric */
    boolean debug;
    boolean igrp2_origin;
    boolean update_needed;

    update_needed = TRUE;
    CD = dual_updatetopology(ddb, FALSE, &update_needed, route, &dndb, &drdb,
			     NULL);
    if (!update_needed)
	return;
    if ((dndb == NULL) || (drdb == NULL))
	return;
    debug = IGRP2_FTARGET(&dndb->dndb_dest);
    igrp2_origin = (route->new_origin == IGRP2_ORG_IGRP2);
    if (debug) {
	buginf("\nDUAL: dual_rcvupdate(): %s via %s metric %u/%u",
	       (*ddb->printnet)(&route->new_dest),
	       igrp2_origin ? (*ddb->printaddr)(&route->new_nexthop) :
	       dual_routetypestring(route->new_origin), route->new_metric,
	       route->new_successor_metric);
    }
    if (igrp2_origin) {
	DUAL_LOG(ddb, DUALEV_RCVUP1, route->new_dest, route->new_dest,
		 route->new_nexthop);
    } else {
	DUAL_LOG(ddb, DUALEV_RCVUP3, route->new_dest, route->new_dest,
		 route->new_origin);
    }
    DUAL_LOG(ddb, DUALEV_RCVUP2, route->new_dest, route->new_metric,
	     route->new_successor_metric);

    if (!dual_dndb_active(dndb)) {

	/*
	 * Passive state.  Don't run the FC check if all paths to the
	 * destination are unreachable.  However, if this newly learned
	 * path is not unreachable, then do the FC check anyway.
	 */
	if ((dndb->dndb_old_metric != METRIC_INACCESSIBLE) ||
	    (drdb->drdb_successor_metric != METRIC_INACCESSIBLE)) {
	    dual_FC(ddb, &dndb, drdb, FALSE); /* May free DNDB! */
	} else {
	    if (drdb->drdb_send_flag == 0) {
		dual_rdbclear(ddb, &dndb, drdb);
	    }
	}
    } else {

	/*
	 * Active state.
	 */
	if ((dndb->dndb_successors > 0) && (drdb == dual_successor(dndb))) {

	    /*
	     * Update came from successor.
	     */
	    if (CD < drdb->drdb_successor_metric) {

		/*
		 * Metric increase.
		 */
		switch (dndb->query_origin) {
		  case DUAL_QOLOCAL:
		    dual_state(ddb, dndb, DUAL_QOCLEAR);
		    break;
		  case DUAL_QOSUCCS:
		    dual_state(ddb, dndb, DUAL_QOMULTI);
		    break;
		  default:
		    if (debug)
			buginf("\nDUAL: query_origin = %d at %d"
			       " for %s from %s",
			       dndb->query_origin, __LINE__,
			       (*ddb->printnet)(&route->new_dest),
			       (*ddb->printaddr)(&route->new_nexthop));
		    break;
		}
	    }
	}
    }
}

/*
 * dual_rcvquery
 * Process a query.
 */
static void dual_rcvquery (ddbtype *ddb, dual_newroute *route)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    boolean fc;

    dual_updatetopology(ddb, TRUE, NULL, route, &dndb, &drdb, NULL);
    if ((dndb == NULL) || (drdb == NULL))
	return;

    IGRP2_FABUGINF(&dndb->dndb_dest)
	("\nDUAL: dual_rcvquery():%s via %s metric %u/%u, RD is %u",
	 (*ddb->printnet)(&route->new_dest),
	 (*ddb->printaddr)(&route->new_nexthop),
	 route->new_metric, route->new_successor_metric, dndb->dndb_metric);
    DUAL_LOG(ddb, DUALEV_RCVQ1, route->new_dest, route->new_dest,
	     route->new_nexthop);
    DUAL_LOG(ddb, DUALEV_RCVQ2, route->new_dest, route->new_metric,
	     route->new_successor_metric);

    if (!dual_dndb_active(dndb)) {

	/*
	 * Passive state.
	 */
	if ((dndb->dndb_old_metric == METRIC_INACCESSIBLE) &&
	    (drdb->drdb_successor_metric == METRIC_INACCESSIBLE)) {
	    dual_sendreply(ddb, dndb, drdb);
	} else {
	    fc = dual_FC(ddb, &dndb, drdb, TRUE); /* May free DNDB! */
	    if (dndb) {
		if (!fc) {
		    if (drdb == dual_successor(dndb)) {
			dual_state(ddb, dndb, DUAL_QOSUCCS);
		    } else {
			dual_state(ddb, dndb, DUAL_QOLOCAL);
		    }
		}
	    }
	}
    } else {

	/*
	 * Active state
	 */
	if ((dndb->dndb_successors > 0) && (drdb == dual_successor(dndb))) {
	    dual_state(ddb, dndb, DUAL_QOMULTI);
	} else {
	    dual_sendreply(ddb, dndb, drdb);
	}
    }
}


/*
 * dual_rcvreply
 * Process a reply.  This could be a big problem if we receive a reply
 * out of the blue when we arent expecting one.  For example, if our
 * successor sends a reply with METRIC_INACCESSIBLE, then we will
 * silently delete the entry from the topology table without ever
 * testing the FC or sending an update or notifying the routing table.
 * We probably need a check for this.
 */

static void dual_rcvreply (ddbtype *ddb, dual_newroute *route)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    boolean debug;
    boolean created_drdb;

    dual_updatetopology(ddb, TRUE, NULL, route, &dndb, &drdb, &created_drdb);
    if ((dndb == NULL) || (drdb == NULL))
	return;

    mem_lock(drdb);			/* Don't let it slip away just yet */
    debug = IGRP2_FTARGET(&dndb->dndb_dest);
    if (debug)
	buginf("\nDUAL: dual_rcvreply(): %s via %s metric %u/%u",
	       (*ddb->printnet)(&route->new_dest),
	       (*ddb->printaddr)(&route->new_nexthop),
	       route->new_metric, route->new_successor_metric);
    DUAL_LOG(ddb, DUALEV_RCVR1, route->new_dest, route->new_dest,
	     route->new_nexthop);
    DUAL_LOG(ddb, DUALEV_RCVR2, route->new_dest, route->new_metric,
	     route->new_successor_metric);

    if (dndb->dndb_reply_status.handles_used > 0) {
	if (debug) {
	    buginf("\nDUAL: Count is %d",
		   dndb->dndb_reply_status.handles_used);
	}
	dual_clearhandle(ddb, dndb, drdb->drdb_handle);
	dual_cleanup_reply_status(ddb, &dndb, drdb); /* May free DRDB! */

	/*
	 * If the DRDB didn't exist before we got the reply, and the route
	 * is still active, and the metric is infinity, delete the DRDB
	 * so we don't suck up hideous amounts of memory waiting to go
	 * passive.
	 */
	if (created_drdb && dual_dndb_active(dndb) &&
	    drdb->drdb_successor_metric == METRIC_INACCESSIBLE &&
	    !drdb->drdb_deleted) {
	    dual_zapdrdb(ddb, dndb, drdb, FALSE);
	}
    } else {
	if (drdb->drdb_successor_metric == METRIC_INACCESSIBLE) {
	    if (drdb->drdb_send_flag == 0) {
		dual_rdbclear(ddb, &dndb, drdb);
	    }
	}
    }
    free(drdb);				/* Undo the mem_lock */
}

/*
 * dual_rcvpkt
 *
 * Process a received packet
 *
 * It is assumed that all the good stuff has been stuffed into the
 * appropriate DDB variables.  This will change when we rip them out
 * of the DDB, of course.
 */
void dual_rcvpkt (ddbtype *ddb, dual_peertype *peer, dual_newroute *route,
		  ulong pkt_type)
{
    switch (pkt_type) {
      case IGRP_OPC_UPDATE:
	dual_rcvupdate(ddb, route);
	break;
      case IGRP_OPC_QUERY:
	dual_rcvquery(ddb, route);
	break;
      case IGRP_OPC_REPLY:
	dual_rcvreply(ddb, route);
	break;
    }
}

/*
 * dual_peerdown
 *
 * Called by protocol-specific peerdown routines (toward the end of the spindly
 * thread) when a peer goes away.
 */
void dual_peerdown (ddbtype *ddb, dual_peertype *peer)
{
    int slot;
    dual_ndbtype *dndb, *next_dndb;
    dual_rdbtype *drdb;
    idbtype *interface;
    igrp2_idbtype *iidb;
    igrp2addrtype infosource;
    int handle;

    iidb = peer->iidb;
    interface = iidb->idb;
    handle = peer->peer_handle;
    infosource = peer->source;
    IGRP2_FBUGINF("\nDUAL: linkdown(): start - %s via %s",
		  (*ddb->printaddr)(&infosource), interface->namestring);
    DUAL_LOG_ALL(ddb, DUALEV_PEERDOWN1, infosource, interface);
    IGRP2_XBUGINF(XDBPEERDOWN,
		  "\nPeer %s going down", (*ddb->printaddr)(&peer->source));

    /* Free the peer handle.  This avoids setting reply status bits for us! */

    igrp2_freehandle(ddb, peer);

    /* Now clean out any topology table entries for this peer. */

    for (slot = 0; slot < NETHASHLEN; slot++) {
	process_may_suspend();

	/* Top of this hash line.  Walk the DNDBs on it. */

	for (dndb = ddb->topology[slot];  dndb;  dndb = next_dndb) {
	    next_dndb = dndb->dndb_next; /* It may be freed! */
	    drdb = dndb->dndb_rdb;
	    IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: Destination %s",
					(*ddb->printnet)(&dndb->dndb_dest));

	    /* Walk each DRDB on this DNDB looking for a match. */

	    while (drdb &&
		   (!(*ddb->addrmatch)(&infosource, &drdb->drdb_infosource) ||
		   (drdb->drdb_iidb != iidb))) {
		drdb = drdb->drdb_next;
	    }

	    /* It gets interesting if we matched our dying peer. */

	    if (drdb) {

		/*
		 * Unthread the DRDB if it was being transmitted.  We
		 * don't want to free the DRDB just yet, as we need it
		 * for the FC checks and such that follow.
		 */
		if (drdb->drdb_send_flag) {
		    dual_xmit_unthread(ddb, &drdb->drdb_thread);
		    dual_cleanup_drdb(ddb, drdb, FALSE);
		}

		/* Disassociate from the peer. */

		if (drdb->drdb_handle != NO_PEER_HANDLE) {
		    if (drdb->drdb_handle != handle) {
			errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
			return;
		    }
		    drdb->drdb_handle = NO_PEER_HANDLE;
		}

		/*
		 * Set D_jk^i to infinity.
		 */
		drdb->drdb_successor_metric = METRIC_INACCESSIBLE;

		/*
		 * Set D_j^i to infinity.
		 */
		drdb->drdb_vecmetric.delay = METRIC_INACCESSIBLE;
		drdb->drdb_metric = METRIC_INACCESSIBLE;
		if (!dual_dndb_active(dndb)) {

		    /*
		     * Passive state.
		     */
		    if (dndb->dndb_successors == 1 && drdb == dndb->dndb_rdb) {
			dndb->dndb_successors = 0;

			/*
			 * D_j^i is already set to infinity above.
			 */
			dual_FC(ddb, &dndb, drdb, FALSE); /* May free DNDB! */
		    } else {

			/*
			 * Wipe out the DRDB and possibly the DNDB if nobody's
			 * still trying to send it anywhere.
			 */
			if (drdb->drdb_send_flag == 0) {
			    dual_zapdrdb(ddb, dndb, drdb, TRUE);
			    if ((dndb->dndb_rdb == NULL) &&
				(dndb->dndb_send_flag == 0)) {
				dual_dndbdelete(ddb, &dndb);
			    }
			}
		    }
		} else {
		    dual_clearhandle(ddb, dndb, handle);
		    if ((dndb->dndb_successors > 0) &&
			(drdb == dual_successor(dndb))) {
			dndb->dndb_successors = 0;

			/*
			 * D_j^i is already set to infinity above.
			 */
			dual_state(ddb, dndb, DUAL_QOCLEAR);
		    }
		    dual_cleanup_reply_status(ddb, &dndb, drdb);
		}
	    } else {			/* No matching RDB found */
		if (dual_dndb_active(dndb)) {

		    /*
		     * Active state.  We didn't find an rdb for this
		     * destination.  However, we may still have to clear
		     * the bit in the reply-status table.  The handle
		     * was passed in by the caller.
		     */
		    dual_clearhandle(ddb, dndb, handle);
		    dual_cleanup_reply_status(ddb, &dndb, NULL);
		}
	    }
	}
    }

    /* Get rid of the peer structure. */

    igrp2_destroy_peer(ddb, peer);

    IGRP2_FBUGINF("\nDUAL: linkdown(): finish");
    dual_log_all(ddb, DUALEV_PEERDOWN2, &handle, NULL);
    dual_finish_update_send(ddb);
}

/*
 * dual_ifdelete
 *
 * Adjust for an interface being deleted.  Everything for this interface
 * should be gone except for active successors;  we zap their IIDB pointers
 * so we don't have any dangles, and whimper if we find anything else.
 */
void dual_ifdelete (ddbtype *ddb, igrp2_idbtype *iidb)
{
    idbtype *interface;
    int slot;
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    boolean active;

    if (iidb == NULL)
	return;
    interface = iidb->idb;
    IGRP2_FBUGINF("\nDUAL: dual_ifdelete(): %s is being deleted",
		  interface->namestring);
    dual_log_all(ddb, DUALEV_IFDEL1, &interface, NULL);

    /* Walk the topology table. */

    for (slot = 0; slot < NETHASHLEN; slot++) {
	process_may_suspend();

	/* Walk each DNDB in the table slot. */

	for (dndb = ddb->topology[slot]; dndb; dndb = dndb->dndb_next) {
	    active = (dual_dndb_active(dndb));

	    /* Walk each DRDB on the DNDB. */

	    for (drdb = dndb->dndb_rdb; drdb; drdb = drdb->drdb_next) {

		/* Look for a matching IIDB pointer. */

		if (drdb->drdb_iidb == iidb) {

		    /*
		     * Got a match.  If this DRDB isn't on an active DNDB,
		     * whine.  No whining if the process is dying, however.
		     */
		    if (!active && !ddb->ddb_going_down) {
			errmsg(&msgsym(RDBLEFT, DUAL),
			       (*ddb->printnet)(&dndb->dndb_dest),
			       (*ddb->printaddr)(&drdb->drdb_nexthop),
			       iidb->idb->namestring,
			       dual_routetypestring(drdb->drdb_origin));
			dual_log_all(ddb, DUALEV_IFDEL3, &dndb->dndb_dest,
				     &drdb->drdb_nexthop);
		    }
		}
	    }
	}
    }
    dual_log_all(ddb, DUALEV_IFDEL2, NULL, NULL);
    IGRP2_FBUGINF("\nDUAL: dual_ifdelete(): finish");
}

/*
 * dual_conn_rtchange
 * Yet another entry point into dual.  This is used to revise connected
 * metric routes in the topology table.
 */

void dual_conn_rtchange (ddbtype *ddb, dual_newroute *route,
			 dual_rtev_type event)
{
    idbtype *interface;

    interface = dual_idb(route->new_iidb);
    if (!interface) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return;
    }

    route->new_external = FALSE;
    route->new_origin = IGRP2_ORG_CONNECTED;
    route->new_data = NULL;
    route->new_ignore = FALSE;
    if (event == DUAL_RTDOWN) {
	route->new_successor_metric = METRIC_INACCESSIBLE;
	route->new_metric = METRIC_INACCESSIBLE;
	route->new_vecmetric.delay = METRIC_INACCESSIBLE;
	DUAL_LOG(ddb, DUALEV_CONNDOWN, route->new_dest, route->new_dest,
		 interface);
	dual_rcvupdate(ddb, route);
	return;
    }
    /*
     * We assume the caller has filled in nexthop, dest, and mtu.
     */
    route->new_vecmetric.delay = igrp_metric_to_eigrp(interface->delay);
    route->new_vecmetric.bandwidth =
	igrp_metric_to_eigrp(SCALED_BANDWIDTH(interface->visible_bandwidth));
    route->new_vecmetric.reliability = interface->hwptr->reliability;
    route->new_vecmetric.load = interface->hwptr->load;
    route->new_vecmetric.hopcount = 0;
    route->new_metric = igrp2_computemetric(route->new_vecmetric.bandwidth,
					    route->new_vecmetric.load,
					    route->new_vecmetric.delay,
					    route->new_vecmetric.reliability,
					    ddb->k1, ddb->k2, ddb->k3, ddb->k4,
					    ddb->k5);
    route->new_successor_metric = 0;
    DUAL_LOG(ddb, DUALEV_CONNCHANGE, route->new_dest, route->new_dest,
	     interface);
    dual_rcvupdate(ddb, route);
}

/*
 * dual_rtchange
 * Process a routing table change.  This happens when one of the following
 * occurs.
 *  1) A route comes up or goes down from another process we are
 *     redistributing.
 *  2) The metric changes for a route we are redistributing from another
 *     process and it has a comparable metric (equal administrative
 *     distance in IP speak)
 *  Note: Do not accept a redistributed route that has a computed metric
 *        of zero.  This occurs when a static route points to Null 0.
 *        The workaround for this is to set a default-metric.  This
 *        is a protocol dependent observation.
 */
void dual_rtchange (ddbtype *ddb, dual_newroute *route,
		    dual_rtev_type event)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;

    dndb = dual_ndblookup(ddb, &route->new_dest);

    switch (event) {
      case DUAL_RTUP:
      case DUAL_RTCHANGE:

	/*
	 * If they are trying to change/add an infinity metric, they
	 * are not doing the correct actions. Add robustness and ignore
	 * this route.
	 */
	if (route->new_metric == METRIC_INACCESSIBLE) {
	    dual_freedata(ddb, &route->new_data);
	    return;
	}
	route->new_successor_metric = 0;
	break;
      case DUAL_RTDOWN:
	if (dndb == NULL) {
	    dual_freedata(ddb, &route->new_data);
	    return;
	}
	drdb = dual_rdblookup(ddb, dndb, route->new_iidb,
			      &route->new_nexthop);
	if (drdb == NULL) {
	    dual_freedata(ddb, &route->new_data);
	    return;
	}

	/*
	 * These three assignments were probably already done by the caller,
	 * but just in case...
	 */
	route->new_successor_metric = METRIC_INACCESSIBLE;
	route->new_metric = METRIC_INACCESSIBLE;
	route->new_vecmetric.delay = METRIC_INACCESSIBLE;
	break;

      default:
	break;
    }
    dual_log(ddb, DUALEV_REDIST1, &route->new_dest, &route->new_dest,
	     &route->new_origin);
    dual_log(ddb, DUALEV_REDIST2, &route->new_dest, &event, NULL);
    dual_rcvupdate(ddb, route);
}

/*
 * dual_lostroute
 * A route derived from this process gets overwritten by another
 * process.
 */
void dual_lostroute (ddbtype *ddb, igrp2nettype *dest)
{
    dual_ndbtype *dndb;
    ulong zero = 0;
    ulong one = 1;

    dndb = dual_ndblookup(ddb, dest);
    if (dndb != NULL) {

	/*
	 * Let's put a little sanity check in here.  If we already
	 * have a non igrp2 derived destination, then don't do anything.
	 * That just means we were needlessly called.  Not to mention,
	 * it can break the FC check if called in the wrong order.
	 * Also, please, please, do not send an update or futz with
	 * metrics if we are active for this destination.
	 */
	if ((dual_dndb_active(dndb)) ||
	    (dndb->dndb_rdb == NULL) ||
	    (dndb->dndb_rdb->drdb_origin != IGRP2_ORG_IGRP2)) {
	    IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: lostroute(): do nothing");
	    dual_log(ddb, DUALEV_LOST, dest, dest, &zero);
	    return;
	}

	/*
	 * Force active state.  It would be nice if we could know which
	 * successors were tossed out by the routing table so that we
	 * could ignore them and check the feasibility condition.  But
	 * unfortunately, we don't.  Therefore we have to force active
	 * state.
	 */
	IGRP2_FABUGINF(&dndb->dndb_dest)("\nDUAL: lostroute(): "
					 "force active state");
	dual_log(ddb, DUALEV_LOST, dest, dest, &one);
	dual_setFD(dndb, 0);
	dual_FC(ddb, &dndb, dndb->dndb_rdb, FALSE); /* May free DNDB! */
    }
}

/*
 * dual_table_walk
 *
 * Traverse topology table for caller. Return next dndb in table.
 */

dual_ndbtype *dual_table_walk (ddbtype *ddb, dual_ndbtype *dndb, int *index)
{

    dual_ndbtype *dndb2;
    int          i;

    /*
     * If still on linked list, return next pointer.
     */
    if (dndb && dndb->dndb_next)
	return(dndb->dndb_next);

    /*
     * Start down hash table.
     */
    i = (dndb) ? *index + 1 : *index;

    for ( ; i < NETHASHLEN; i++) {
	process_may_suspend();
	for (dndb2 = ddb->topology[i]; dndb2; dndb2 = dndb2->dndb_next) {

	    /*
	     * Found a non-NULL entry, update index and return pointer.
	     */
	    *index = i;
	    return(dndb2);
	}
    }
    return(NULL);
}

/*
 * dual_cleanup
 * Clear out topology table completely.  This process is going away.
 */
void dual_cleanup (ddbtype *ddb)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;
    int index;

    for (index = 0; index < NETHASHLEN; index++) {
	process_may_suspend();
	while ((dndb = ddb->topology[index]) != NULL) {
	    /*
	     * First, scan rdb chain clearing it out and any external
	     * data structures.
	     */
	    while ((drdb = dndb->dndb_rdb) != NULL) {
		dual_zapdrdb(ddb, dndb, drdb, TRUE);
	    }

	    /*
	     * Clear out the reply-status table if it exists, wipe out
	     * the ndb, and move on.
	     */
	    if (dual_dndb_active(dndb)) {
		free(dndb->dndb_reply_status.handle_array);
		dndb->dndb_reply_status.handle_array = NULL;
		dndb->dndb_reply_status.handles_used = 0;
		dndb->dndb_reply_status.handle_array_size = 0;
	    }
	    dual_dndbdelete(ddb, &dndb);
	}
    }
    if (ddb->events)
	free(ddb->events);
    if (ddb->sia_events)
	free(ddb->sia_events);
}

/*
 * dual_log_copy
 * Take a snapshot of the active dual event log by copying it to a new buffer.
 */

static void dual_log_copy (ddbtype *ddb, dual_event_type *src,
			   dual_event_type *src_ptr, dual_event_type **buf,
			   dual_event_type **bufptr)
{
    if (src == NULL)
	return;
    if (*buf == NULL) {
	*buf = malloc_named(sizeof(dual_event_type) * ddb->max_events,
			    "Dual Events");
	if (*buf == NULL) {
	    return;
	}
    }
    bcopy(src, *buf, (sizeof(dual_event_type) * ddb->max_events));
    *bufptr = (src_ptr - src) + *buf;
}

/*
 * dual_sia_copy_log
 *
 * Copy the event log in an SIA situation.
 */
void dual_sia_copy_log (ddbtype *ddb)
{

    /* If we found any stuck peers, copy the log, but only once. */

    if (!ddb->sia_events) {
	dual_log_copy(ddb, ddb->events, ddb->event_curptr, &ddb->sia_events,
		      &ddb->sia_eventptr);

	/* Kill all peers to trigger a pseudo-SIA on our neighbors if asked. */

	if (ddb->kill_everyone)
	    igrp2_take_all_nbrs_down(ddb, "all neighbors killed");
    }
}

/*
 * dual_unstick_dndb
 *
 * Clears a DNDB that's stuck in active.  The DNDB may be freed, along
 * with other DNDBs in the topology table.  Peers may be freed.  Basically,
 * we will leave things a mess.
 */
static void dual_unstick_dndb (ddbtype *ddb, dual_ndbtype *dndb)
{
    dual_rdbtype *drdb, *next_drdb;
    dual_peertype *peer, *next;
    ulong handle_count;
    ulong stuck_time;
    
    /* Uhoh, we're stuck.  Duly note it. */
    
    mem_lock(dndb);
    errmsg(&msgsym(SIA, DUAL),
	   (*ddb->printnet)(&dndb->dndb_dest),
	   ddb->name, ddb->asystem);
    handle_count = dndb->dndb_reply_status.handles_used;
    stuck_time = ELAPSED_TIME(dndb->dndb_active_time);
    
    DUAL_LOG_ALL(ddb, DUALEV_SIA, dndb->dndb_dest, stuck_time);
    DUAL_LOG_ALL(ddb, DUALEV_SIA4, handle_count,
		 dndb->dndb_reply_status.handle_array[0]);
    
    /*
     * Walk all of the DRDBs.  If we find one associated with an active peer,
     * knock it off of the list.  If we find a DRDB with no peer associated
     * with it, whimper now but clean it up later.  Be careful in case the
     * DNDB is freed or goes passive in the process.
     */
    for (drdb = dndb->dndb_rdb; (drdb && !dndb->dndb_deleted &&
				 dual_dndb_active(dndb));
	 drdb = next_drdb) {
	next_drdb = drdb->drdb_next; /* It may go away! */
	if (drdb->drdb_handle == NO_PEER_HANDLE)
	    continue;
	if (!igrp2_test_handle(ddb, &dndb->dndb_reply_status,
			       drdb->drdb_handle)) {
	    continue;
	}
	peer = igrp2_handle_to_peer(ddb, drdb->drdb_handle);
	if (peer) {
	    dual_log_all(ddb, DUALEV_NORESP, &drdb->drdb_nexthop,
			 &drdb->drdb_origin);
	    igrp2_clear_handle(ddb, &dndb->dndb_reply_status,
			       drdb->drdb_handle);
	    igrp2_take_peer_down(ddb, peer, TRUE, "stuck in active");
	} else {	/* No peer! */
	    dual_log_all(ddb, DUALEV_SIA5, &drdb->drdb_nexthop,
			 &drdb->drdb_origin);
	}
    }
    
    /*
     * Now walk all of the peers to see if any of them has a bit set.
     * These would correspond to active peers with no DRDBs (shouldn't
     * happen--but then none of this should.)  Note that the DNDB may have
     * been deleted above, or may be deleted in the process, or may have
     * gone passive in either case.
     */
    peer = ddb->peer_list;
    while (peer && !dndb->dndb_deleted && dual_dndb_active(dndb)) {
	next = peer->next_peer;
	if (igrp2_test_handle(ddb, &dndb->dndb_reply_status,
			      peer->peer_handle)) {
	    DUAL_LOG_ALL(ddb, DUALEV_SIA3, peer->source, peer->iidb->idb);
	    igrp2_clear_handle(ddb, &dndb->dndb_reply_status,
			       peer->peer_handle);
	    igrp2_take_peer_down(ddb, peer, TRUE, "stuck in active");
	}
	peer = next;
    }

    /*
     * If we have any bits left at this point, whimper one more time.
     * This means that the peer corresponding to the SIA bit has
     * mysteriously disappeared.  If the DNDB has been deleted above, skip it.
     */
    if (!dndb->dndb_deleted && dual_dndb_active(dndb)) {
	if (dndb->dndb_reply_status.handles_used) {
	    errmsg(&msgsym(SIANOHANDLE, DUAL),
		   dndb->dndb_reply_status.handles_used,
		   dndb->dndb_reply_status.handle_array[0]);
	    dual_log_all(ddb, DUALEV_SIA2,
			 &dndb->dndb_reply_status.handles_used,
			 dndb->dndb_reply_status.handle_array);
	    dndb->dndb_reply_status.handles_used = 0;
	    dual_cleanup_reply_status(ddb, &dndb, NULL);
	}
    }
    free(dndb);
}

/*
 * dual_scan_active
 * This function scans the topology table looking for destinations
 * that are stuck in active.  It does this by looking for any active
 * state routes with timestamps older than allowed.  If one is found,
 * then it will log an error and clean up the mess.
 */
void dual_scan_active (ddbtype *ddb)
{
    int index;
    dual_ndbtype *dndb, *next_dndb;
    boolean found_sia;
    boolean restart_hash_line;

    /* Bail if we're not doing SIA checking. */

    if (ddb->active_time == 0)
	return;

    /* Walk the topology table. */

    found_sia = FALSE;
    for (index = 0; index < NETHASHLEN; index++) {
	process_may_suspend();

	/*
	 * If we hit a DDB that is stuck in active, we have to restart
	 * from the beginning of the hash line.  This is because when
	 * we clear the neighbor, it may make other DNDBs in the hash
	 * line disappear as well (if there were multiple DNDBs stuck).
	 * We may process some DNDBs more than once, but that's OK (if a
	 * little wasteful).
	 *
	 * We finess this by using a do/while loop that brings us back
	 * the beginning of the hash line if we find an active entry.
	 */
	do {				/* Restart loop */
	    restart_hash_line = FALSE;
	    dndb = ddb->topology[index];
	    while (dndb != NULL && !restart_hash_line) {
		next_dndb = dndb->dndb_next;
		if (dual_dndb_active(dndb)) {
		    
		    /* DNDB is active.  Has it been active for too long? */
		    
		    if (TIMER_RUNNING(dndb->dndb_active_time) && 
			CLOCK_OUTSIDE_INTERVAL(dndb->dndb_active_time,
					       ddb->active_time)) {
			dual_unstick_dndb(ddb, dndb);
			found_sia = TRUE;
			restart_hash_line = TRUE;
		    }
		}
		dndb = next_dndb;
	    }
	} while (restart_hash_line);
    }
    dual_finish_update_send(ddb);

    /* If we found any stuck peers, copy the log, but only once. */

    if (found_sia){
	dual_sia_copy_log(ddb);
    }
}

/*
 * dual_routelookup
 * Return the drdb for the feasible successor which matches those parameters
 * currently set in the ddb.
 */

dual_rdbtype *dual_routelookup (ddbtype *ddb, igrp2nettype *dest,
				igrp2_idbtype *iidb, igrp2addrtype *nexthop)
{
    dual_ndbtype *dndb;
    dual_rdbtype *drdb;

    dndb = dual_ndblookup(ddb, dest);
    if (dndb == NULL)
	return(NULL);
    drdb = dual_rdblookup(ddb, dndb, iidb, nexthop);
    if (drdb == NULL)
	return(NULL);
    if (drdb->drdb_successor_metric >= dndb->dndb_old_metric)
	return(NULL);
    return(drdb);
}

static const ulong dual_event_length[DUAL_ETYPE_COUNT] = {
    0,					/* DUAL_NONE */
    sizeof(ulong),			/* DUAL_NUMBER */
    sizeof(ulong),			/* DUAL_HEXNUMBER */
    sizeof(igrp2nettype),		/* DUAL_NET */
    sizeof(igrp2addrtype),		/* DUAL_ADDR */
    DUAL_EVENTS_BUFFER_SIZE/2,		/* DUAL_STRING */
    sizeof(idbtype *),			/* DUAL_SWIF */
    sizeof(boolean),			/* DUAL_BOOLEAN */
    sizeof(ulong),			/* DUAL_RTORIGIN */
    sizeof(ulong),			/* DUAL_QUERYORIGIN */
    sizeof(ulong),			/* DUAL_RTEVENT */
    sizeof(ulong)			/* DUAL_PKTTYPE */
};
static const dual_event_decode dual_event_table[DUALEV_COUNT] = {
    { "Not used", DUAL_NONE, DUAL_NONE },
    { "Generic", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_GENERIC */
    { "Update sent, RD", DUAL_NET, DUAL_NUMBER }, /* DUALEV_UPDATE_SEND */
    { "Update reason, delay", DUAL_STRING, DUAL_NUMBER }, /* DUALEV_UPDATE_SEND2 */
    { "State change", DUAL_QUERYORIGIN, DUAL_QUERYORIGIN }, /* DUALEV_STATE */
    { "Clr handle dest/cnt", DUAL_NET, DUAL_NUMBER }, /* DUALEV_CLEARHANDLE1 */
    { "Clr handle num/bits", DUAL_NUMBER, DUAL_HEXNUMBER }, /* DUALEV_CLEARHANDLE2 */
    { "Free reply status", DUAL_NET, DUAL_NONE }, /* DUALEV_REPLYFREE */
    { "Send reply", DUAL_NET, DUAL_ADDR }, /* DUALEV_SENDREPLY */
    { "Forced promotion", DUAL_NET, DUAL_NUMBER }, /* DUALEV_FPROMOTE */
    { "Route install", DUAL_NET, DUAL_ADDR }, /* DUALEV_RTINSTALL */
    { "Route delete", DUAL_NET, DUAL_ADDR }, /* DUALEV_RTDELETE */
    { "NDB delete", DUAL_NET, DUAL_NUMBER }, /* DUALEV_NDBDELETE */
    { "RDB delete", DUAL_NET, DUAL_ADDR }, /* DUALEV_RDBDELETE */
    { "Active net/peers", DUAL_NET, DUAL_NUMBER }, /* DUALEV_ACTIVE */
    { "Not active net/1=SH", DUAL_NET, DUAL_NUMBER }, /* DUALEV_NOTACTIVE */
    { "Set reply-status", DUAL_NUMBER, DUAL_HEXNUMBER }, /* DUALEV_SETREPLY */
    { "Split-horizon set", DUAL_BOOL, DUAL_SWIF }, /* DUALEV_SPLIT */
    { "Find FS", DUAL_NET, DUAL_NUMBER }, /* DUALEV_SEARCHFS */
    { "FC sat nh/ndbmet", DUAL_ADDR, DUAL_NUMBER }, /* DUALEV_FCSAT1 */
    { "FC sat rdbmet/succmet", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_FCSAT2 */
    { "FC not sat Dmin/met", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_FCNOT */
    { "Rcv update dest/nh", DUAL_NET, DUAL_ADDR }, /* DUALEV_RCVUP1 */
    { "Rcv update met/succmet", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_RCVUP2 */
    { "Rcv update dest/orig", DUAL_NET, DUAL_RTORIGIN }, /* DUALEV_RCVUP3 */
    { "Rcv query dest/nh", DUAL_NET, DUAL_ADDR }, /* DUALEV_RCVQ1 */
    { "Rcv query met/succ met", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_RCVQ2 */
    { "Rcv reply dest/nh", DUAL_NET, DUAL_ADDR }, /* DUALEV_RCVR1 */
    { "Rcv reply met/succ met", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_RCVR2 */
    { "Peer down", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_PEERDOWN1 */
    { "Peer down end, handle", DUAL_NUMBER, DUAL_NONE }, /* DUALEV_PEERDOWN2 */
    { "Peer up", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_PEERUP */
    { "i/f delete", DUAL_SWIF, DUAL_NONE },	/* DUALEV_IFDEL1 */
    { "i/f delete finish", DUAL_NONE, DUAL_NONE }, /* DUALEV_IFDEL2 */
    { "i/f del lurker", DUAL_NET, DUAL_ADDR }, /* DUALEV_IFDEL3 */
    { "Lost route 1=forceactv", DUAL_NET, DUAL_NUMBER }, /* DUALEV_LOST */
    { "Conn rt down", DUAL_NET, DUAL_SWIF }, /* DUALEV_CONNDOWN */
    { "Conn rt change", DUAL_NET, DUAL_SWIF }, /* DUALEV_CONNCHANGE */
    { "Redist rt change", DUAL_NET, DUAL_RTORIGIN }, /* DUALEV_REDIST1 */
    { "Redist rt event", DUAL_RTEVENT, DUAL_NONE }, /* DUALEV_REDIST2 */
    { "SIA dest/time", DUAL_NET, DUAL_NUMBER }, /* DUALEV_SIA */
    { "SIA lost handles", DUAL_NUMBER, DUAL_HEXNUMBER }, /* DUALEV_SIA2 */
    { "SIA peer with no DRDB", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_SIA3 */
    { "SIA count, bits", DUAL_NUMBER, DUAL_HEXNUMBER }, /* DUALEV_SIA4 */
    { "SIA DRDB with no peer", DUAL_ADDR, DUAL_RTORIGIN }, /* DUALEV_SIA5 */
    { "SIA peer with DRDB", DUAL_ADDR, DUAL_RTORIGIN }, /* DUALEV_NORESP */
    { "Intf mcast ready", DUAL_SWIF, DUAL_NONE }, /* DUALEV_MCASTRDY */
    { "Peer stuck in INIT", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_INITSTUCK */
    { "Peer should be in INIT", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_NOINIT */
    { "Hold time expired", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_HOLDTIMEEXP */
    { "ACK suppressed", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_ACKSUPPR */
    { "Unicast sent", DUAL_SWIF, DUAL_ADDR }, /* DUALEV_UCASTSENT */
    { "Multicast sent", DUAL_SWIF, DUAL_NONE }, /* DUALEV_MCASTSENT */
    { "Serno range", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_PKTSENT2 */
    { "Suppressed", DUAL_PKTTYPE, DUAL_NONE }, /* DUALEV_PKTSUPPR */
    { "Not sent", DUAL_PKTTYPE, DUAL_NONE }, /* DUALEV_PKTNOSEND */
    { "Packet received", DUAL_SWIF, DUAL_ADDR }, /* DUALEV_PKTRCV */
    { "Opcode/Flags", DUAL_PKTTYPE, DUAL_HEXNUMBER }, /* DUALEV_PKTRCV2 */
    { "Seq/AckSeq", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_PKTRCV3 */
    { "Bad sequence", DUAL_ADDR, DUAL_PKTTYPE }, /* DUALEV_BADSEQ */
    { "Reply ACK", DUAL_NET, DUAL_ADDR }, /* DUALEV_REPLYACK */
    { "Reply packetized", DUAL_NET, DUAL_ADDR }, /* DUALEV_REPLYPACK */
    { "Reply enqueued", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_REPLYENQ */
    { "Reply transmitted", DUAL_NET, DUAL_ADDR }, /* DUALEV_REPLYXMIT */
    { "Reply delay/poison", DUAL_NUMBER, DUAL_BOOL }, /* DUALEV_REPLYXMIT2 */
    { "Update ACK", DUAL_NET, DUAL_SWIF }, /* DUALEV_UPDATEACK */
    { "Update packetized", DUAL_NET, DUAL_SWIF }, /* DUALEV_UPDATEPACK */
    { "Update enqueued", DUAL_SWIF, DUAL_NONE }, /* DUALEV_UPDATEENQ */
    { "Update transmitted", DUAL_NET, DUAL_SWIF }, /* DUALEV_UPDATEXMIT */
    { "Update delay/poison", DUAL_NUMBER, DUAL_BOOL }, /* DUALEV_UPDATEXMIT2 */
    { "Query ACK", DUAL_NET, DUAL_SWIF }, /* DUALEV_QUERYACK */
    { "Query packetized", DUAL_NET, DUAL_SWIF }, /* DUALEV_QUERYPACK */
    { "Query enqueued", DUAL_SWIF, DUAL_NONE }, /* DUALEV_QUERYENQ */
    { "Query transmitted", DUAL_NET, DUAL_SWIF }, /* DUALEV_QUERYXMIT */
    { "Query delay/poison", DUAL_NUMBER, DUAL_BOOL }, /* DUALEV_QUERYXMIT2 */
    { "Startup ACK", DUAL_ADDR, DUAL_NUMBER }, /* DUALEV_STARTUPACK */
    { "Startup packetized", DUAL_ADDR, DUAL_NUMBER }, /* DUALEV_STARTUPPACK */
    { "Startup enqueued", DUAL_ADDR, DUAL_SWIF }, /* DUALEV_STARTUPENQ */
    { "serno/seq #", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_STARTUPENQ2 */
    { "Startup transmitted", DUAL_NET, DUAL_ADDR }, /* DUALEV_STARTUPXMIT */
    { "Startup delay/poison", DUAL_NUMBER, DUAL_BOOL }, /* DUALEV_STARTUPXMIT2 */
    { "Last peer down", DUAL_SWIF, DUAL_NONE }, /* DUALEV_LASTPEER */
    { "First peer up", DUAL_SWIF, DUAL_NONE }, /* DUALEV_FIRSTPEER */
    { "Query split-horizon forced on", DUAL_NET, DUAL_SWIF }, /* DUALEV_QSHON */
    { "Query split-horizon forced off", DUAL_NET, DUAL_SWIF }, /* DUALEV_QSHOFF */
    { "Metric set", DUAL_NET, DUAL_NUMBER }, /* DUALEV_SETRD */
    { "Route OBE net/refcount", DUAL_NET, DUAL_NUMBER }, /* DUALEV_OBE */
    { "Update squashed", DUAL_NET, DUAL_SWIF }, /* DUALEV_UPDATESQUASH */
    { "Change queue emptied, entries", DUAL_NUMBER, DUAL_NONE}, /* DUALEV_EMPTYCHANGEQ */
    { "Requeue ucast", DUAL_ADDR, DUAL_NUMBER }, /* DUALEV_REQUEUEUCAST */
    { "Xmit size/gap", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_XMITTIME */
    { "Revise summary", DUAL_ADDR, DUAL_ADDR }, /* DUALEV_SUMREV */
    { "added/metric", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_SUMREV2 */
    { "min metric/result code", DUAL_NUMBER, DUAL_NUMBER }, /* DUALEV_SUMREV3 */
    { "queued for recalculation", DUAL_NONE, DUAL_NONE }, /* DUALEV_SUMDEP */
    { "Get sum metric", DUAL_ADDR, DUAL_ADDR }, /* DUALEV_GETSUM */
    { "min metric", DUAL_NUMBER, DUAL_NONE }, /* DUALEV_GETSUM2 */
    { "Process summary", DUAL_ADDR, DUAL_ADDR }, /* DUALEV_PROCSUM */
    { "added/metric ptr", DUAL_NUMBER, DUAL_HEXNUMBER }, /* DUALEV_PROCSUM2 */
    { "Ignored route, metric", DUAL_ADDR, DUAL_NUMBER }, /* DUALEV_IGNORE */
    { "Unknown", DUAL_NUMBER, DUAL_NUMBER } /* DUALEV_UNKNOWN */
};

static void dual_format_event_record (ddbtype *ddb, char *buffer,
				      dual_event_type *evt_ptr,
				      int counter, boolean print_time)
{
    dual_etype_enum	data1_type, data2_type;
    uchar		*outbuf;
    char		*type;
    uchar		*buf;
    int			numchar;
    const dual_event_decode *decodep;

    decodep = &dual_event_table[evt_ptr->code];
    type = decodep->message;
    data1_type = decodep->data1;
    data2_type = decodep->data2;
    outbuf = buffer;
    if (print_time) {
	numchar = sprintf(outbuf, "%4d %CC %s: ", counter, "%T.%k",
			  &evt_ptr->timestamp, type);
    } else {
	numchar = sprintf(outbuf, "%4d %s: ", counter, type);
    }
    outbuf += numchar;
    buf = &evt_ptr->buf[0];
    buf = dual_show_event_param(ddb, data1_type, buf, outbuf, &numchar);
    outbuf += numchar;
    (void)dual_show_event_param(ddb, data2_type, buf, outbuf, &numchar);
}

/*
 * dual_log_event
 *
 * Log each event.  This routine will be called from some other ipigrp2
 * routine with useful data.  The data will be stored for later display
 */

void dual_log_event (ddbtype *ddb, dual_event_code code, void *data1,
		     void *data2)
{
    dual_event_type *curptr;
    uchar *buf;
    ulong len1, len2;
    char log_string[120];

    /* If the system has crashed, forget it. */

    if (process_get_boolean(system_crashed))
	return;

    /* If logging is disabled, forget it. */

    if (!ddb->ddb_logging_enabled)
	return;

    /* If no memory, forget it. */

    if (ddb->events == NULL)
	return;

    curptr = ddb->event_curptr;
    curptr->code = code;
    buf = &curptr->buf[0];
    len1 = dual_event_length[dual_event_table[code].data1];
    len2 = dual_event_length[dual_event_table[code].data2];
    if ((len1 + len2 <= DUAL_EVENTS_BUFFER_SIZE)) {
	if (len1 && data1)
	    bcopy(data1, buf, len1);
	if (len2 && data2)
	    bcopy(data2, buf + len1, len2);
    } else {
	memset(buf, 0, DUAL_EVENTS_BUFFER_SIZE);
    }
    clock_get_time(&curptr->timestamp);

    /* Bump and circle. */

    if (ddb->log_events) {
	dual_format_event_record(ddb, log_string, curptr,
				 ddb->event_count++, FALSE);
	errmsg(&msgsym(EVENT, DUAL), ddb->name, log_string);
    }
    ddb->event_curptr++;

    if (ddb->event_curptr >= (ddb->events + ddb->max_events)) {
	ddb->event_curptr = ddb->events;
    }
}

/*
 * dual_allocate_free_lists
 *
 * Create custom free lists for data structures with lots of churn.
 */
void dual_allocate_free_lists (void)
{
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(dual_ndbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(dual_rdbtype));
}

/*
 * dual_init_events
 * allocate memory for igrp2 event logging queue
 *
 * If sense is FALSE, the size is set to the default.
 *
 * Returns TRUE if OK, FALSE if malloc failed.
 */
boolean dual_init_events (ddbtype *ddb, ulong event_buffer_size, boolean sense)
{

    if (!sense)
	event_buffer_size = DUAL_DEFAULT_MAX_EVENTS;

    ddb->max_events = event_buffer_size;

    /* If it's already there, then start over. */

    if (ddb->events) {
	free(ddb->events);
	ddb->events = NULL;
	ddb->event_curptr = NULL;
    }

    /* Free the SIA buffer, in case we're resizing. */

    if (ddb->sia_events) {
	free(ddb->sia_events);
	ddb->sia_events = NULL;
	ddb->sia_eventptr = NULL;
    }

    /* If the size is zero, don't waste the memory. */

    if (event_buffer_size == 0)
	return(TRUE);

    ddb->event_curptr = ddb->events = malloc_named((sizeof(dual_event_type) * 
						    event_buffer_size),
						   "Dual Events");
    return (ddb->events != NULL);
}

/*
 * dual_clear_event_logging
 *
 * Turn off event logging.
 */
void dual_clear_event_logging (ddbtype *ddb)
{
    ddb->ddb_logging_enabled = FALSE;
}

/*
 * dual_clear_log
 *
 * Empties out all event logs, and reenabled event logging.
 */
void dual_clear_log (ddbtype *ddb)
{
    dual_init_events(ddb, ddb->max_events, TRUE);
    ddb->ddb_logging_enabled = TRUE;
}

/*
 * dual_adjust_scratchpad
 *
 * Adjust the size of the scratchpad area to match the number of IIDBs
 * that are active for DUAL.  We should only adjust upward,
 * because life is rife with race conditions when IIDBs get
 * deleted (since some DRDBs received on that interface may linger).
 */
void dual_adjust_scratchpad (ddbtype *ddb)
{
    if (!ddb->scratch_table_size ||
	(ddb->scratch_table_size < ddb->iidbQ.count)) {
	ddb->scratch_table_size = max(ddb->iidbQ.count, 1);

	/* Free any old one. */
	
	free(ddb->ddb_scratch);
	
	/* Allocate the new one.  It may be NULL. */
	
	ddb->ddb_scratch = malloc_named(ddb->scratch_table_size *
					sizeof(dual_iidb_scratch),
					"Dual IIDB scratch table");
    }
}

/*
 * dual_init_timers
 *
 * Initialize DDB managed timers.
 */
static void dual_init_timers (ddbtype *ddb)
{
    /* Initialize the master timer for the Router process. */

    mgd_timer_init_parent(&ddb->ddb_master_timer, NULL);

    /* Initialize the master timer for the Hello process. */

    mgd_timer_init_parent(&ddb->ddb_hello_timer, NULL);

    /* Initialize the children of the master timer. */

    mgd_timer_init_parent(&ddb->ddb_intf_timer, &ddb->ddb_master_timer);
    mgd_timer_init_parent(&ddb->ddb_peer_timer, &ddb->ddb_master_timer);
    mgd_timer_init_leaf(&ddb->ddb_active_timer, &ddb->ddb_master_timer,
			IGRP2_ACTIVE_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&ddb->ddb_pdm_timer, &ddb->ddb_master_timer,
			IGRP2_PDM_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&ddb->ddb_acl_change_timer, &ddb->ddb_master_timer,
			IGRP2_DDB_ACL_CHANGE_TIMER, NULL, FALSE);
}

/*
 * dual_init_ddb
 *
 * Set up common DDB information
 *
 * Returns TRUE if successful
 */
boolean dual_init_ddb (ddbtype *ddb, char *hello_process_name,
		       ulong extdata_size, ulong workq_size)
{
    ddb->ddb_thread_tail = &ddb->ddb_thread_head;
    ddb->ddb_change_queue_tail = &ddb->ddb_change_queue;
    ddb->ddb_next_serno = 1;		/* zero is reserved */
    if (!igrp2_init_hello_process(ddb, hello_process_name))
	return(FALSE);
    ddb->ddb_inputQ = create_watched_queue(hello_process_name,
					   IGRP2_MAX_QDEPTH, RECURRING);
    if (!ddb->ddb_inputQ) {
	process_kill(ddb->hello_pid);
	return(FALSE);
    }
    ddb->extdata_chunk = chunk_create(extdata_size, DUAL_EXTDATA_CHUNKSIZE,
				      CHUNK_FLAGS_DYNAMIC, NULL, 0,
				      "EIGRP ExtData");
    ddb->workq_chunk = chunk_create(workq_size, DUAL_WORKQ_CHUNKSIZE,
				    CHUNK_FLAGS_DYNAMIC, NULL, 0,
				    "EIGRP WorkQ");
    igrp2_process_starting(ddb);
    queue_init(&ddb->iidbQ, 0);
    queue_init(&ddb->temp_iidbQ, 0);
    queue_init(&ddb->passive_iidbQ, 0);
    dual_init_events(ddb, DUAL_DEFAULT_MAX_EVENTS, TRUE);
    dual_init_timers(ddb);
    ddb->ddb_logging_enabled = TRUE;
    ddb->dual_logging = DUAL_LOGGING_DUAL_DEFAULT;
    ddb->dual_xport_logging = DUAL_LOGGING_XPORT_DEFAULT;
    ddb->dual_xmit_logging = DUAL_LOGGING_XMIT_DEFAULT;
    ddb->log_adj_change = IGRP2_DEFAULT_LOG_ADJ_CHANGE;
    dual_adjust_scratchpad(ddb);	/* Ensure that it has at least one. */
    return(TRUE);
}

/*
 * dual_query_origin_str
 *
 * Return a string describing the query_origin value
 */
static char *const dual_qo_str[DUAL_QOCOUNT] =
    {"Clear", "Local origin", "Multiple Origins", "Successor Origin"};
char *dual_query_origin_str (dual_qo_t qo)
{
    if (qo >= DUAL_QOCOUNT)
	return (NULL);
    return (dual_qo_str[qo]);
}

/*
 * dual_routetypestring
 *
 * Return the string associated with a particular DUAL route type
 */

static char *dual_routetypestring (igrp2origintype rtorigin)
{
    switch (rtorigin) {
      case IGRP2_ORG_CONNECTED:
	return("Connected");
      case IGRP2_ORG_IGRP2:
	return("Internal");
      case IGRP2_ORG_REDISTRIBUTED:
	return("Redistributed");
      case IGRP2_ORG_SUMMARY:
	return("Summary");
      default:
	return("Unknown");
    }
}

/*
 * dual_route_event_string
 *
 * Return the string associated with a particular DUAL route event
 */
static char *dual_route_event_string (dual_rtev_type type)
{
    switch (type) {
      case DUAL_RTUP:
	return("Route Up");
      case DUAL_RTDOWN:
	return("Route Down");
      case DUAL_RTCHANGE:
	return("Route Changed");
      default:
	return("Unknown");
    }
}

/*
 * dual_show_event_param
 * Decode event type, and show a particular parameter from an event.
 * Print into a supplied buffer for caller's convenience.
 */

static uchar *
dual_show_event_param (ddbtype *ddb, dual_etype_enum type, uchar *bufin,
		       uchar *bufout, int *numcharout)
{
    ulong data;
    int   charcnt = 0;

    data = *(ulong *)bufin;
    switch (type) {
      case DUAL_NONE:
	charcnt = 0;
	bufout[0] = '\0';
	break;
      case DUAL_BOOL:
	charcnt = sprintf(bufout, "%s ", (data ? "TRUE" : "FALSE"));
	break;
      case DUAL_SWIF:
	if ((idbtype *)data != NULL)
	    charcnt = sprintf(bufout, "%s ", ((idbtype *)data)->namestring);
	break;
      case DUAL_NUMBER:
	charcnt = sprintf(bufout, "%u ", data);
	break;
      case DUAL_HEXNUMBER:
	charcnt = sprintf(bufout, "%#x ", data);
	break;
      case DUAL_NET:
	charcnt = sprintf(bufout, "%s ",
			  (*ddb->printnet)((igrp2nettype *)bufin));
	break;
      case DUAL_ADDR:
	charcnt = sprintf(bufout, "%s ",
			  (*ddb->printaddr)((igrp2addrtype *)bufin));
	break;
      case DUAL_STRING:
	charcnt = sprintf(bufout, "%s ", bufin);
	break;
      case DUAL_RTORIGIN:
	charcnt = sprintf(bufout, "%s ", dual_routetypestring(data));
	break;
      case DUAL_QUERYORIGIN:
	charcnt = sprintf(bufout, "%s ", dual_query_origin_str(data));
	break;
      case DUAL_RTEVENT:
	charcnt = sprintf(bufout, "%s ", dual_route_event_string(data));
	break;
      case DUAL_PKTTYPE:
	charcnt = sprintf(bufout, "%s ", igrp2_otype(data));
	break;
      default:
	charcnt = sprintf(bufout, "0x%x ", data);
	break;
    }
    *numcharout = charcnt;
    return (bufin + dual_event_length[type]);
}

/*
 * dual_free_show_blocks
 *
 * Free up the list of blocks allocated for dual_show_events.  Uneventful.
 */
static void dual_free_show_blocks (dual_show_event_block *block_head)
{
    dual_show_event_block *next_block;

    while (block_head) {
	next_block = block_head->next_block;
	free(block_head);
	block_head = next_block;
    }
}

/*
 * dual_show_events
 * This routine shows logged events.  It starts at the bottom
 * and works its way up.  Event types are defined above, and
 * determine how the data will be displayed.
 *
 * In order to avoid having to allocate another giant contiguous block
 * of memory, we copy the log into a series of smaller blocks so that
 * we can print it even if memory has become somewhat fragmented.
 */

void dual_show_events (ddbtype *ddb, int which, ulong start, ulong end)
{
    dual_event_type *copy_ptr, *src_events, *src_curptr, *src_ptr;
    dual_show_event_block *block_head, *block_cur, **block_prev;
    int block_counter, entry_counter;
    char print_buffer[120];
    boolean copy_done;

    switch (which) {
    case IGRP2_EVENT_CUR:
	src_events = ddb->events;
	src_curptr = ddb->event_curptr;
	break;
    case IGRP2_EVENT_SIA:
	/*
	 * Show SIA event log
	 */
	src_events = ddb->sia_events;
	src_curptr = ddb->sia_eventptr;
	break;
    default:
	return;
    }
    if ((src_events == NULL) || (src_events->code == 0)) {
	printf("\n  Event log is empty.");
	return;
    }

    /*
     * Copy the log into a linked list of smaller blocks, so that we have
     * a prayer of being able to do this even when memory is fragmented even
     * a little and we're using a mondo log size.  We copy in reverse order
     * so that our list can be traversed front-to-back.
     */
    block_head = NULL;
    block_prev = &block_head;
    src_ptr = src_curptr;
    copy_done = FALSE;
    while (!copy_done) {

	/* Create the next block.  If we run out of space, free and bail. */

	block_cur = malloc_named(sizeof(dual_show_event_block),
				 "Dual event display");
	if (block_cur == NULL) {	/* Uhoh */
	    dual_free_show_blocks(block_head);
	    printf(nomemory);
	    return;
	}

	/* Link the new block into the chain. */

	*block_prev = block_cur;
	block_prev = &block_cur->next_block;

	/* Copy in the elements. */

	entry_counter = DUAL_SHOW_EVENT_ENTRY_COUNT;
	copy_ptr = &block_cur->event_entry[0];
	while (entry_counter--) {

	    /* Back-bump the log pointer, and circle as necessary. */

	    src_ptr--;
	    if (src_ptr < src_events)
		src_ptr = src_events + (ddb->max_events - 1);
	    bcopy(src_ptr, copy_ptr, sizeof(dual_event_type));
	    copy_ptr++;
	    if (src_ptr == src_curptr) { /* We hit the end! */
		copy_done = TRUE;
		break;
	    }
	}
    }

    /* Got it.  Now run down it and print it. */

    automore_enable(NULL);
    block_cur = block_head;
    copy_ptr = &block_head->event_entry[0];
    entry_counter = 0;
    for (block_counter = 1;
	 block_counter <= ddb->max_events;
	 block_counter++, entry_counter++, copy_ptr++) {

	/*
	 * If we're past the end of the current block, advance to the
	 * next one.
	 */
	if (entry_counter >= DUAL_SHOW_EVENT_ENTRY_COUNT) {
	    block_cur = block_cur->next_block;
	    if (block_cur == NULL) {
		printf("\n Internal error!!"); /* Can't happen... */
		break;
	    }
	    copy_ptr = &block_cur->event_entry[0];
	    entry_counter = 0;
	}

	/* Bail if we're done. */

	if (copy_ptr->code == 0)
	    break;

	/* Be sane. */

	if (copy_ptr->code >= DUALEV_UNKNOWN)
	    copy_ptr->code = DUALEV_UNKNOWN;

	/* Format the item if we're supposed to show this one. */

	if (!start || (block_counter >= start && block_counter <= end)) {
	    dual_format_event_record(ddb, print_buffer, copy_ptr,
				     block_counter, TRUE);
	    printf("\n%s", print_buffer);
	}

	/* Bail out if things are getting ugly. */

	if (automore_quit() || modemcheck(stdio) ||
	    (start && block_counter > end)) {
	    break;
	}
    }
    dual_free_show_blocks(block_head);
    automore_disable();
}

/*
 * dual_configure_logging
 *
 * Configure the set of event types to log (or do NVGEN as appropriate)
 *
 * The parameters are only valid if csb->nvgen is FALSE.
 */
void dual_configure_logging (parseinfo *csb, ddbtype *ddb, boolean dual_flag,
			     boolean xmit_flag, boolean xport_flag)
{
    if (csb->nvgen) {
	if (ddb->dual_logging != DUAL_LOGGING_DUAL_DEFAULT ||
	    ddb->dual_xport_logging != DUAL_LOGGING_XPORT_DEFAULT ||
	    ddb->dual_xmit_logging != DUAL_LOGGING_XMIT_DEFAULT) {
	    nv_write(TRUE, "%s", csb->nv_command);
	    nv_add(ddb->dual_logging, " dual");
	    nv_add(ddb->dual_xmit_logging, " xmit");
	    nv_add(ddb->dual_xport_logging, " transport");
	}
    } else {
    	ddb->dual_logging = dual_flag;
	ddb->dual_xmit_logging = xmit_flag;
	ddb->dual_xport_logging = xport_flag;
    }
}
