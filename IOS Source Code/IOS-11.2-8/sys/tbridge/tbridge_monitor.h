/* $Id: tbridge_monitor.h,v 3.1.8.1 1996/05/04 05:09:10 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_monitor.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_monitor.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Declarations used in the tbridge_monitor process.
 *------------------------------------------------------------------
 * $Log: tbridge_monitor.h,v $
 * Revision 3.1.8.1  1996/05/04  05:09:10  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/04/21  23:06:48  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * These are the current queues and timers upon which this
 * process waits, and which it handles.
 * This is all the constrained IP multicasting stuff,
 * but other features may be added in the future.
 */

enum queue_types {
    TBM_IGMP_QUEUE,
    TBM_OSPF_QUEUE,
    TBM_CGMP_QUEUE,
    TBM_LAST_QUEUE
};

enum timer_types {
    TBM_IGMP_QRY_TIMER,
    TBM_IGMP_RPT_TIMER,
    TBM_MROUTER_TIMER,
    TBM_LAST_TIMER
};

/*
 * Global declarations:
 */
extern watched_queue *tbm_igmpQ;
extern watched_queue *tbm_ospfQ;
extern watched_queue *tbm_cgmpQ;

extern mgd_timer tbm_igmp_qry_tmr;
extern mgd_timer tbm_igmp_rpt_tmr;
extern mgd_timer tbm_mrouter_tmr;
