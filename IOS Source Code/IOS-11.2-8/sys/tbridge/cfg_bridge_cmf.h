/* $Id: cfg_bridge_cmf.h,v 3.1.8.3 1996/05/11 07:34:00 speakman Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/cfg_bridge_cmf.h,v $
 *------------------------------------------------------------------
 * tbridge/cfg_bridge_cmf.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Parse chains for Constrained Multicast Flooding (CMF) config commands
 *------------------------------------------------------------------
 * $Log: cfg_bridge_cmf.h,v $
 * Revision 3.1.8.3  1996/05/11  07:34:00  speakman
 * CSCdi57444:  CMF UI needs appropriate keyword
 * Branch: California_branch
 * Change the show/clear keyword from 'cmf' to 'multicast'.
 * Extend 'clear' flexibility.
 *
 * Revision 3.1.8.2  1996/05/08  00:23:50  speakman
 * CSCdi56968:  CMF accounting for flooded packets is incorrect
 * Branch: California_branch
 *  - account for IP multicast rx packets in the SA's bte
 *  - show CMF rx and tx packet counts per-interface
 *  - constrain the initial IGMP report on a given interface
 *    to router ports only to prevent suppression and sequential
 *    joining behaviour
 *  - show IGMP timers only when CMF debugging is enabled
 *  - clear the multicast-router-port state when the multicast-group
 *    state is cleared
 *  - flood (unconstrained) IP multicast data packets in the absence of
 *    any multicast-router ports
 *
 * Revision 3.1.8.1  1996/05/04  05:08:43  wilber
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
 * Revision 3.1  1996/04/21  23:06:42  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(cfg_bridge_cmf_command_exit, NONE);

/***************************************************************************
 * bridge cmf <cgmp>
 *
 */

EOLS	(bridge_cmf_eol, tbridge_cmf_command, TBRIDGE_CFG_CMF);

EOLS	(bridge_cmf_cgmp_eol, tbridge_cmf_command, TBRIDGE_CFG_CMF_CGMP);

KEYWORD (bridge_cmf_cgmp, bridge_cmf_cgmp_eol, bridge_cmf_eol,
	 "cgmp", "Cisco Group Management Protocol", PRIV_CONF|PRIV_HIDDEN);

KEYWORD (bridge_cmf, bridge_cmf_cgmp, cfg_bridge_cmf_command_exit,
	 "cmf", "Constrained multicast flooding", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	bridge_cmf
