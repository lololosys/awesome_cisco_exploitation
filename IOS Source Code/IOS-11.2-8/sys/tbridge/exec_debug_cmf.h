/* $Id: exec_debug_cmf.h,v 3.1.8.1 1996/05/04 05:08:45 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/exec_debug_cmf.h,v $
 *------------------------------------------------------------------
 * tbridge/exec_debug_cmf.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Parse chains for Constrained Multicast Flooding (CMF) debug commands
 *------------------------------------------------------------------
 * $Log: exec_debug_cmf.h,v $
 * Revision 3.1.8.1  1996/05/04  05:08:45  wilber
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
 * Revision 3.1  1996/04/21  23:06:44  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(debug_tbridge_cmf_eol, tbridge_debug_command, DEBUG_TBRIDGE_CMF);
KEYWORD (debug_tbridge_cmf, debug_tbridge_cmf_eol, NONE,
	 "cmf", "Constrained multicast flooding", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tbridge_cmf
