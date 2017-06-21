/* $Id: igrp2-inlines.h,v 3.3.58.3 1996/04/02 07:29:21 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2-inlines.h,v $
 *------------------------------------------------------------------
 * IGRP2 inline routines.
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igrp2-inlines.h,v $
 * Revision 3.3.58.3  1996/04/02  07:29:21  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.3.58.2  1996/04/02  05:37:01  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.3.58.1  1996/03/18  20:27:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/07  09:49:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  14:29:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:33:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:07:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define IGRP2_ULONG_BITS 32
#define IGRP2_HIGH_BIT 0x80000000

typedef struct igrp2_longlong_ {
    ulong top;
    ulong bottom;
} igrp2_longlong;

#include <ciscolib.h>

/*
 * Divide a long long (64 bits) by a long (32 bits) yielding a 64 bit
 * result.  Throw away the remainder.
 */
static inline unsigned long long igrp2_divide (
     unsigned long long dividend,
     ulong divisor)
{
    int i;
    igrp2_longlong d, r;
    unsigned long long quotient;

    if (divisor > dividend)
	return(0);
    if (divisor == dividend)
	return(1);

    d.bottom = dividend & -1L;
    d.top = dividend >> IGRP2_ULONG_BITS;
    r.bottom = 0;
    r.top = 0;
    for (i = 0; i < 64; i++) {
	r.top <<= 1;
	if (r.bottom & IGRP2_HIGH_BIT)
	    r.top |= 1;
	r.bottom <<= 1;
	if (d.top & IGRP2_HIGH_BIT)
	    r.bottom |= 1;
	d.top <<= 1;
	if (d.bottom & IGRP2_HIGH_BIT)
	    d.top |= 1;
	d.bottom <<= 1;
	if (r.bottom < divisor) {
	    if (r.top > 0) {
		r.top -= 1;
		r.bottom -= divisor;
		d.bottom |= 1;
	    }
	} else {
	    r.bottom -= divisor;
	    d.bottom |= 1;
	}
    }
    quotient = d.top;
    quotient <<= IGRP2_ULONG_BITS;
    quotient |= d.bottom;
    return(quotient);
}
/*
 * This is the 32 bit version of the igrp metric computation.  It uses
 * 64 bit arithemetic (long long) to take care of the overflow problem.
 */
static inline ulong igrp2_computemetric (
     ulong bw,
     uchar load,
     ulong delay,
     uchar rely,
     uchar k1,
     uchar k2,
     uchar k3,
     uchar k4,
     uchar k5)
{
    unsigned long long m;
    ulong metric;

    m = 0;
    if (k1)
        m += bw * k1;
    if (k2)
        m += (bw * k2) / (256 - load);
    if (k3)
	m += delay * k3;
    if (k5) {
	m *= k5;
	m = igrp2_divide(m, rely + k4);
    }

    /* If the metric = 0 bump it to one.  metric = 0 is a special
     * case reserved by dual. 
     */
    if (m == 0)
	m = 1;

    if (m > METRIC_INACCESSIBLE)
	metric = METRIC_INACCESSIBLE;
    else
	metric = m;

    return(metric);
}

/*
 * igrp2_def_helloint
 * Determine the default interval between HELLO packets for a particular
 * interface and linktype.  Also consider interface bandwidth.  Note
 * bandwidth is stored inverted so check is also.
 */
static inline ulong igrp2_def_helloint (idbtype *swidb, enum LINK link)
{
    if (if_is_nbma(swidb, link) &&
 	(swidb->visible_bandwidth <= IGRP2_SLOW_VISIBLE_BANDWIDTH))
	return(IGRP2_HELLOTIME_NBMA);
    return(IGRP2_HELLOTIME);
}

/*
 * igrp2_def_holdint
 * Determine the default hold time for HELLO packets for a particular
 * interface and linktype.  Also consider interface bandwidth.  Note
 * bandwidth is stored inverted so check is also.
 */
static inline ulong igrp2_def_holdint (idbtype *swidb, enum LINK link)
{
    if (if_is_nbma(swidb, link) &&
 	(swidb->visible_bandwidth <= IGRP2_SLOW_VISIBLE_BANDWIDTH))
	return(IGRP2_HOLDTIME_NBMA);
    return(IGRP2_HOLDTIME);
}

/*
 * igrp2_kvalues_match
 * Verify that K-values of hello packet match our configured values.
 * Do comparison in such a way as to prevent the compiler from optimizing
 * the comparison into short/long memory references.  Such references lead
 * to misaligned accesses on some platforms.
 */

static inline boolean igrp2_kvalues_match (ddbtype *ddb,
					   igrp2_paramtype *param)
					 
{
    return(bcmp(&param->k1, &ddb->k1, IGRP2_KBYTES) == 0);
}

/*
 * igrp2_packet_data
 *
 * Returns a pointer to the data field in an EIGRP packet, given a pointer
 * to the packet itself.
 */
static inline void *igrp2_packet_data (igrp2type *igrp2)
{
    return((uchar *)igrp2 + IGRP2HEADERBYTES);
}

/*
 * igrp2_route_is_external
 *
 * Returns TRUE if the DNDB contains an external route.
 */
static inline boolean igrp2_route_is_external (dual_ndbtype *dndb)
{
    dual_rdbtype *drdb;

    /* No RDBs?  Shouldn't happen... */

    drdb = dndb->dndb_rdb;
    if (!drdb)
	return(FALSE);
    return(drdb->drdb_external);
}

/*
 * dual_dndb_active
 *
 * Returns TRUE if the DNDB route is in active state
 */
static inline boolean dual_dndb_active (dual_ndbtype *dndb)
{
    return (dndb->dndb_reply_status.handle_array != NULL);
}

/*
 * igrp2_peer_count
 *
 * Returns the number of peers on an interface
 */
static inline ulong igrp2_peer_count (igrp2_idbtype *iidb)
{
    return(iidb->iidb_handles.handles_used);
}

/*
 * igrp2_debug_rcvd_pkt
 *
 * Issues a debug message for a received packet, if appropriate.
 */
static inline void igrp2_debug_rcvd_pkt (ddbtype *ddb, dual_peertype *peer,
					 igrp2type *igrp2, idbtype *idb,
					 igrp2addrtype *source)
{
    if (igrp2_packet_debug) {
	igrp2_bugpak(peer, igrp2, NULL, "Received", ddb, idb,
		     (peer ? peer->iidb : NULL), source, 0);
    }
}

/*
 * igrp2_debug_sent_pkt
 *
 * Issues a debug message for a sent packet, if appropriate.
 */
static inline void igrp2_debug_sent_pkt (ddbtype *ddb, dual_peertype *peer,
					 igrp2_idbtype *iidb,
					 igrp2_pakdesc *pakdesc,
					 igrp2type *igrp2, ulong retry_count)
{
    if (igrp2_packet_debug) {
	igrp2_bugpak(peer, igrp2, pakdesc, "Sending", ddb, iidb->idb,
		     iidb, NULL, retry_count);
    }
}

/*
 * igrp2_debug_enqueued_pkt
 *
 * Issues a debug message for an enqueued packet, if appropriate.
 */
static inline void igrp2_debug_enqueued_pkt (ddbtype *ddb, dual_peertype *peer,
					     igrp2_idbtype *iidb,
					     igrp2_pakdesc *pakdesc)
{
    if (igrp2_packet_debug) {
	igrp2_bugpak(peer, NULL, pakdesc, "Enqueueing", ddb, iidb->idb,
		     iidb, NULL, 0);
    }
}

/*
 * dual_log_dual_all
 *
 * Log a DUAL event if enabled, without a target check.
 */
static inline void dual_log_all (ddbtype *ddb, dual_event_code event,
				 void *param1, void *param2)
{
    if (ddb->dual_logging) {
	dual_log_event(ddb, event, param1, param2);
    }
}

/*
 * dual_log
 *
 * Log a DUAL event with a target check.
 */
static inline void dual_log (ddbtype *ddb, dual_event_code event,
			     igrp2nettype *target,
			     void *param1, void *param2)
{
    if (!ddb->debug_target_set || (*ddb->netmatch)(target, &ddb->debug_target))
	dual_log_all(ddb, event, param1, param2);
}

/*
 * dual_log_xport_all
 *
 * Log a transport event if enabled, without a target check.
 */
static inline void dual_log_xport_all (ddbtype *ddb, dual_event_code event,
				      void *param1, void *param2)
{
    if (ddb->dual_xport_logging) {
	dual_log_event(ddb, event, param1, param2);
    }
}

/*
 * dual_log_xport
 *
 * Log a transport event with a target check.
 */
static inline void dual_log_xport (ddbtype *ddb, dual_event_code event,
				  igrp2addrtype *target,
				  void *param1, void *param2)
{
    if (!ddb->debug_peer_set || (*ddb->addrmatch)(target, &ddb->debug_peer))
	dual_log_xport_all(ddb, event, param1, param2);
}

/*
 * dual_log_xmit_all
 *
 * Log a transmit event if enabled, without a target check.
 */
static inline void dual_log_xmit_all (ddbtype *ddb, dual_event_code event,
				      void *param1, void *param2)
{
    if (ddb->dual_xmit_logging) {
	dual_log_event(ddb, event, param1, param2);
    }
}

/*
 * dual_log_xmit
 *
 * Log a transmit event with a target check.
 */
static inline void dual_log_xmit (ddbtype *ddb, dual_event_code event,
				  igrp2nettype *target,
				  void *param1, void *param2)
{
    if (!ddb->debug_target_set || (*ddb->netmatch)(target, &ddb->debug_target))
	dual_log_xmit_all(ddb, event, param1, param2);
}

/*
 * dual_finish_update_send
 *
 * Go finish sending updates if the change queue has something in it.
 *
 * An inline because we unfortunately need to litter the process with
 * these calls.
 */
static inline void dual_finish_update_send (ddbtype *ddb)
{
    if (ddb->ddb_change_queue)
	dual_finish_update_send_guts(ddb);
}

/*
 * dual_idb
 *
 * Return the idb given the IIDB.
 */
static inline idbtype *dual_idb (igrp2_idbtype *iidb)
{
    if (!iidb)
	return(NULL);
    return(iidb->idb);
}

/*
 * igrp_metric_to_eigrp
 *
 * Convert a metric scaled for IGRP into one scaled for EIGRP.
 */
static inline ulong igrp_metric_to_eigrp (ulong igrp_metric)
{
    return (igrp_metric << IGRP_TO_IGRP2_SHIFT);
}
