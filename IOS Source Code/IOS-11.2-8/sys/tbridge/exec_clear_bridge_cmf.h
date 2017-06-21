/* $Id: exec_clear_bridge_cmf.h,v 3.1.8.4 1996/08/12 16:07:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/exec_clear_bridge_cmf.h,v $
 *------------------------------------------------------------------
 * tbridge/exec_clear_bridge_cmf.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Parse chains for Constrained Multicast Flooding (CMF) clear commands
 *------------------------------------------------------------------
 * $Log: exec_clear_bridge_cmf.h,v $
 * Revision 3.1.8.4  1996/08/12  16:07:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.8.3  1996/05/11  07:34:01  speakman
 * CSCdi57444:  CMF UI needs appropriate keyword
 * Branch: California_branch
 * Change the show/clear keyword from 'cmf' to 'multicast'.
 * Extend 'clear' flexibility.
 *
 * Revision 3.1.8.2  1996/05/08  00:23:51  speakman
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
 * Revision 3.1.8.1  1996/05/04  05:08:44  wilber
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
 * Revision 3.1  1996/04/21  23:06:43  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(clear_bridge_cmf_command_exit, NONE);

/******************************************************************
 * clear bridge [<bridge-group>] multicast
 *              [{<router-ports> | <groups> | <counts>}]
 *              [<group-address>] [<interface-unit>] [<counts>]
 *
 * OBJ(int,1)   = <bridge-group>
 * OBJ(int,2)   = <clear_routers>
 * OBJ(int,3)   = <clear_groups>
 * OBJ(int,4)   = <clear_counts>
 * OBJ(paddr,1) = <group-address>
 * OBJ(idb,1)   = <interface-unit>
 */

EOLNS (clear_bridge_multicast_eol, tbridge_clear_bridge_multicast_command);

KEYWORD_ID (clear_bridge_multicast_counts, clear_bridge_multicast_eol,
	    clear_bridge_multicast_eol, OBJ(int,4), TRUE, "counts",
	    "Multicast packet counts", PRIV_OPR);

INTERFACE (clear_bridge_multicast_idb, clear_bridge_multicast_counts,
	   clear_bridge_multicast_counts, OBJ(idb,1), IFTYPE_ANYSUB);

IPADDR (clear_bridge_multicast_addr, clear_bridge_multicast_idb,
	clear_bridge_multicast_idb, OBJ(paddr,1), "group address");

KEYWORD_ID (clear_bridge_multicast_routers, clear_bridge_multicast_idb,
	    clear_bridge_multicast_counts, OBJ(int,3), TRUE,
	    "router-ports", "Multicast router ports", PRIV_OPR);

KEYWORD_ID (clear_bridge_multicast_groups, clear_bridge_multicast_addr,
	    clear_bridge_multicast_routers, OBJ(int,2), TRUE,
	    "groups", "Multicast groups", PRIV_OPR);

KEYWORD (clear_bridge_multicast_kwd, clear_bridge_multicast_groups,
	 clear_bridge_cmf_command_exit,
	 "multicast", "Multicast-group state", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_bridge_multicast_kwd
