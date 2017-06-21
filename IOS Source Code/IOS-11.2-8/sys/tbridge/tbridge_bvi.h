/* $Id: tbridge_bvi.h,v 3.1.4.1 1996/05/04 05:08:50 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_bvi.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_bvi.h
 *
 * March, 1996 - Wilber Su
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Transparent Bridging Bridge-group Virtual Interface
 * For Integrated Routing and Bridging(IRB) support
 *------------------------------------------------------------------
 * $Log: tbridge_bvi.h,v $
 * Revision 3.1.4.1  1996/05/04  05:08:50  wilber
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
 * Revision 3.1  1996/03/09  03:04:08  wilber
 * Placeholders for Integrated Routing and Bridging(IRB)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean tbridge_bvi_parser_createidb(uint64 *, parseinfo *);
extern boolean tbridge_bvi_stay_shutdown(idbtype *);
extern void tbridge_bvi_swif_comingup(idbtype *);
extern void tbridge_bvi_swif_erase(idbtype *);
extern void tbridge_bvi_macaddrs_update(idbtype *);
extern void tbridge_bvi_initialize(idbtype *);
