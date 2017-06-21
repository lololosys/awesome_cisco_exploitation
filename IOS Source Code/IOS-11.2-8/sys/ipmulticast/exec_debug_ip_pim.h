/* $Id: exec_debug_ip_pim.h,v 3.2.60.2 1996/09/13 22:52:41 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_debug_ip_pim.h,v $
 *------------------------------------------------------------------
 * PIM Debug Commands
 *
 * November 1993.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_pim.h,v $
 * Revision 3.2.60.2  1996/09/13  22:52:41  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/09/10  05:31:35  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:29:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/19  14:48:08  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [un]debug ip mcache [<group-name-or-address>]
 *
 * OBJ(paddr,1) - group name or address
 */
EOLS (debug_ip_mcache_eol, ipmulticast_debug_commands, DEBUG_MCACHE);
IPADDR_NAME (debug_ip_mcache_addr, debug_ip_mcache_eol, 
	     debug_ip_mcache_eol, OBJ(paddr,1), "IP group address");
KEYWORD (debug_ip_mcache, debug_ip_mcache_addr, ALTERNATE,
	 "mcache", "IP multicast cache operations", PRIV_OPR);

/******************************************************************
 * [un]debug ip mrouting [<group-name-or-address>]
 *
 * OBJ(paddr,1) - group name or address
 */
EOLS (debug_ip_mrouting_eol, ipmulticast_debug_commands, DEBUG_MROUTE);
IPADDR_NAME (debug_ip_mrouting_addr, debug_ip_mrouting_eol, 
	     debug_ip_mrouting_eol, OBJ(paddr,1), "IP group address");
KEYWORD (debug_ip_mrouting, debug_ip_mrouting_addr, debug_ip_mcache,
	 "mrouting", "IP multicast routing table activity", PRIV_OPR);

/******************************************************************
 * [un]debug ip pim auto-rp 
 *
 */
EOLS	(debug_ip_pim_autorp_eols, ipmulticast_debug_commands, DEBUG_AUTORP);

/******************************************************************
 * [un]debug ip pim [<group-name-or-address>]
 *
 * OBJ(paddr,1) - group name or address
 */
EOLS	(debug_ip_pim_eol, ipmulticast_debug_commands, DEBUG_PIM);
IPADDR_NAME (debug_ip_pim_addr, debug_ip_pim_eol, debug_ip_pim_eol,
	     OBJ(paddr,1), "IP group address");
TEST_MULTIPLE_FUNCS(debug_ip_pim_test, debug_ip_pim_addr,
	     debug_ip_pim_eol, NONE);
KEYWORD_MM (debug_ip_pim_autorp, debug_ip_pim_autorp_eols, debug_ip_pim_test,
	 "auto-rp", "Auto-RP", PRIV_OPR, 4);
KEYWORD (debug_ip_pim, debug_ip_pim_autorp, debug_ip_mrouting,
	 "pim", "PIM protocol activity", PRIV_OPR);

/******************************************************************
 * [un]debug ip sd
 *
 */
EOLS	(debug_ip_sd_eols, ipmulticast_debug_commands, DEBUG_SD);
KEYWORD (debug_ip_sd, debug_ip_sd_eols, debug_ip_pim,
	 "sd", "Session Directory (SD)", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_sd

