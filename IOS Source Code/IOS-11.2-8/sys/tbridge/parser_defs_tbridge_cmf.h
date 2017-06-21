/* $Id: parser_defs_tbridge_cmf.h,v 3.1.2.1 1996/05/04 05:08:47 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/parser_defs_tbridge_cmf.h,v $
 *------------------------------------------------------------------
 * tbridge/parser_defs_tbridge_cmf.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Parser defines for Constrained Multicast Flooding (CMF)
 *------------------------------------------------------------------
 * $Log: parser_defs_tbridge_cmf.h,v $
 * Revision 3.1.2.1  1996/05/04  05:08:47  wilber
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
 * Revision 3.1  1996/05/03  01:21:03  speakman
 * One last placehoder for CMF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define TBRIDGE_CFG_CMF		0
#define TBRIDGE_CFG_CMF_CGMP	1
