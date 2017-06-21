/* $Id: tbridge_debug_flags.h,v 3.3.56.4 1996/05/14 04:30:17 speakman Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_debug_flags.h,v $
 *------------------------------------------------------------------
 * Transparent Bridging debug flags.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tbridge_debug_flags.h,v $
 * Revision 3.3.56.4  1996/05/14  04:30:17  speakman
 * CSCdi57584:  CGMP packets not seen by RSP
 * Branch: California_branch
 * Calculate network_start when copying RSP packets for flooding.
 *
 * Revision 3.3.56.3  1996/05/09  14:47:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.56.2  1996/05/04  05:08:56  wilber
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
 * Revision 3.3.56.1  1996/03/27  09:03:29  speakman
 * CSCdi51444:  Blocked bridge ports do not respond to ARP broadcasts.
 * Branch: California_branch
 * Move the check for non-blocking interfaces to FOLLOW the onerous
 * type-specific duties.  Add receive packet debugging.
 *
 * Revision 3.3  1995/11/25  00:48:59  rbadri
 * CSCdi44433:  Transparent bridging debug flags should be off
 * Move TBRIDGE_DEBUG within the comment.
 *
 * Revision 3.2  1995/11/17  18:45:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:31:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:03:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from TBRIDGE code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(tbridge_debug_arr)

DEBUG_FLAG(tbridge_debug_pak_0, DEBUG_TBRIDGE_PAK_0,
	   "Transparently bridged packets (rx interrupt)")

DEBUG_FLAG(tbridge_debug_pak_1, DEBUG_TBRIDGE_PAK_1,
	   "Transparently bridged packets (rx functions)")
DEBUG_FLAG(tbridge_debug_rcv_1, DEBUG_TBRIDGE_RCV_1,
	   "Transparently bridged received packets (rx functions)")
DEBUG_FLAG(tbridge_debug_dis_1, DEBUG_TBRIDGE_DIS_1,
	   "Transparently bridged discards (rx functions)")

DEBUG_FLAG(tbridge_debug_pak_2, DEBUG_TBRIDGE_PAK_2,
	   "Transparently bridged packets (fast-switched)")
DEBUG_FLAG(tbridge_debug_mcast_2, DEBUG_TBRIDGE_MCAST_2,
	   "Transparently bridged multicasts (fast-switched)")

DEBUG_FLAG(tbridge_debug_pak_3, DEBUG_TBRIDGE_PAK_3,
	   "Transparently bridged packets (process-switched)")

DEBUG_FLAG(tbridge_debug_ipm, DEBUG_TBRIDGE_IPM,
	   "IP multicast flooding")
DEBUG_FLAG(tbridge_debug_cmf, DEBUG_TBRIDGE_CMF,
	   "Constrained multicast flooding")
DEBUG_FLAG(tbridge_debug_crb, DEBUG_TBRIDGE_CRB,
	   "Concurrent routing and bridging events")
DEBUG_FLAG(tbridge_debug_cbus, DEBUG_TBRIDGE_CBUS,
	   "Autonomous bridging statistics")
DEBUG_FLAG(tbridge_debug_cbus_flood, DEBUG_TBRIDGE_CBUS_FLOOD,
	   "Autonomously flooded packets")

DEBUG_ARRDONE

/*
 * Defining TBRIDGE_DEBUG will enable a bunch of tbridging debugging code
 * that should never be included in customer images of any description.
 * It will impair performance and produce a potentially huge number of
 * debug messages.  Define this ONLY for debugging development images.
 * The commands to enable the different levels of debugging are hidden.

#define TBRIDGE_DEBUG

 */
