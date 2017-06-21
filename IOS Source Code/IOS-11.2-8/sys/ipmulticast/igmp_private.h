/* $Id: igmp_private.h,v 3.3.32.2 1996/05/04 01:08:25 wilber Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/igmp_private.h,v $
 *------------------------------------------------------------------
 * igmp_private.h - Private definitions for IGMP.
 *
 * Nov 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igmp_private.h,v $
 * Revision 3.3.32.2  1996/05/04  01:08:25  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.3.32.1  1996/03/27  23:30:03  mleelani
 * CSCdi52589:  UI misinformation of Igmp version
 * Branch: California_branch
 * Fix UI version bug.
 *
 * Revision 3.3  1996/01/12  22:56:01  mleelani
 * CSCdi46802:  IGMP needs faster leave response
 * Miscellaneous v2 enhancements.
 *
 * Revision 3.2  1995/11/30  04:42:06  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.1  1995/11/29  21:02:43  mleelani
 * Placeholder for private IGMP defines.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Static storage
 */
static void igmp_receive_leave(const igmptype *igmp, ipaddrtype source,
				   ipaddrtype dest, idbtype *idb);
static void igmp_process_last_member(igmp_cachetype *igmp);
static void igmp_elect_qr(const igmptype *igmp, ipaddrtype source, idbtype *idb);
static void igmp_switch_host_version(const idbtype *idb);
static void igmp_switch_version(idbtype *idb, int version);
static void igmp_switch_qr(const idbtype *idb);
static void igmp_do_mgd_timers(mgd_timer *master_timer, idbtype *idb);
static void igmp_report_delay(igmp_cachetype *cache, const idbtype *idb, uchar code);
static igmpidbtype *igmp_create_idb_block(idbtype *idb);

