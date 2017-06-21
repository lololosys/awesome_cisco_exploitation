/* $Id: exec_debug_dhcp.h,v 3.1 1996/02/01 04:58:29 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ip/exec_debug_dhcp.h,v $
 *------------------------------------------------------------------
 * exec_debug_dhcp.h - DHCP proxy client   debug parse chain
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_debug_dhcp.h,v $
 * Revision 3.1  1996/02/01  04:58:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:47:45  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug dhcp
 *
 */
EOLS	(debug_dhcp_eol, dhcp_debug_command, DEBUG_DHCP);

INTERFACE (debug_dhcp_get_interface, debug_dhcp_eol, no_alt,
	   OBJ(idb,1), IFTYPE_P2P);

KEYWORD (debug_dhcp_int, debug_dhcp_get_interface, debug_dhcp_eol,
	 "Interface", "Debug DHCP Activity related to one interface",
	 PRIV_OPR);

KEYWORD_DEBUG(debug_dhcp_cmds, debug_dhcp_int, ALTERNATE,
	      OBJ(pdb,1), dhcp_debug_arr,
	      "dhcp", "DHCP client activity", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_dhcp_cmds
