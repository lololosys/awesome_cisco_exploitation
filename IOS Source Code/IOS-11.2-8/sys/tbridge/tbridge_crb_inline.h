/* $Id: tbridge_crb_inline.h,v 3.2.62.2 1996/05/04 05:08:55 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_crb_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_crb_inline.h
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Generic inline functions for Concurrent Routing and Bridging (CRB)
 *------------------------------------------------------------------
 * $Log: tbridge_crb_inline.h,v $
 * Revision 3.2.62.2  1996/05/04  05:08:55  wilber
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
 * Revision 3.2.62.1  1996/03/27  09:03:28  speakman
 * CSCdi51444:  Blocked bridge ports do not respond to ARP broadcasts.
 * Branch: California_branch
 * Move the check for non-blocking interfaces to FOLLOW the onerous
 * type-specific duties.  Add receive packet debugging.
 *
 * Revision 3.2  1995/11/17  18:44:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/30  22:44:18  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Exchange the safety of data hiding for a 7% performance improvement
 * in the 64 byte pps no-drop rate.  Use an inline to do the SMF match
 * instead of a function call, and forgo the pointer safety checks in
 * ieee_equal in the bte locator to do a direct MAC address comparison.
 *
 * Revision 2.3  1995/06/23  21:42:12  speakman
 * CSCdi36377:  Alignment error in tbridge
 * Do the right thing.
 *
 * Revision 2.2  1995/06/23  04:12:42  speakman
 * CSCdi36302:  IGMP fails when bridging is configured
 * Identify IP multicasts for reception in tbridge_crb_inline
 * instead of tbridge_forward_inline.
 *
 * Revision 2.1  1995/06/07  23:03:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


