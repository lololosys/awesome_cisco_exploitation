/* $Id: exec_show_dhcp.h,v 3.1.40.1 1996/05/21 09:50:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_dhcp.h,v $
 *------------------------------------------------------------------
 * exec_show_dhcp.h - DHCP proxy client   show parse chain
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_dhcp.h,v $
 * Revision 3.1.40.1  1996/05/21  09:50:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1  1996/02/01  04:58:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:47:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:55  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show dhcp [interface <interface>]
 * 
 * OBJ(idb,1) = <interface>
 *
 * The 'interface' keyword is not necessary.  The action routine
 * doesn't use it, but we must parse it for compatability.
 */

EOLS	(show_dhcp_lease_eol, show_dhcpc_leases, SHOW_DHCP_LEASE);

INTERFACE_KEYWORD(show_dhcp_if, show_dhcp_lease_eol,
		  show_dhcp_lease_eol,
		  OBJ(idb,1), IFTYPE_P2P,
		  common_str_interface, "Show DHCP information on one interface");

KEYWORD (show_dhcp_lease, show_dhcp_if, no_alt,
	 "lease", "Show DHCP Addresses leased from a server",
	 PRIV_ROOT);

EOLS	(show_dhcp_server_eol, show_dhcp, SHOW_DHCP_SERVER);

KEYWORD (show_dhcp_server, show_dhcp_server_eol, show_dhcp_lease,
	 "server", "Show DHCP Servers we know about",
	 PRIV_ROOT);

KEYWORD (show_dhcp, show_dhcp_server, ALTERNATE,
	 "dhcp", "Dynamic Host Configuration Protocol status",
	 PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_dhcp
