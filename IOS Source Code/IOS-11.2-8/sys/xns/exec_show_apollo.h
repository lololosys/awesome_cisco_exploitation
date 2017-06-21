/* $Id: exec_show_apollo.h,v 3.2.62.2 1996/05/30 23:48:06 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_apollo.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A P O L L O . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_apollo.h,v $
 * Revision 3.2.62.2  1996/05/30  23:48:06  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/21  10:11:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  19:21:35  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:45  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:22:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




/******************************************************************
 * show apollo traffic
 *
 */
EOLS	(show_apollo_traffic_eol, show_apollo, SHOW_APOLLO_TRAFFIC);
KEYWORD (show_apollo_traffic, show_apollo_traffic_eol, no_alt,
	"traffic", "Apollo network statistics", PRIV_USER);



/******************************************************************
 * show apollo route [<net>]
 *
 * OBJ(int,1) =	<net> (zero means all nets)
 *
 */
EOLS	(show_apollo_route_eol, show_apollo, SHOW_APOLLO_ROUTE);
NUMBER	(show_apollo_route_num, show_apollo_route_eol, show_apollo_route_eol,
	OBJ(int,1), 1, -1, "Routes to the specified apollo network");
KEYWORD (show_apollo_route, show_apollo_route_num, show_apollo_traffic,
	"route", "Apollo routing table", PRIV_USER);



/******************************************************************
 * show apollo interface [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_apollo_interface_eol, show_apollo, SHOW_APOLLO_INTERFACE);
INTERFACE (show_apollo_get_interface, show_apollo_interface_eol, show_apollo_interface_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_apollo_interface, show_apollo_get_interface, show_apollo_route,
	common_str_interface, "Apollo interface status and configuration", PRIV_USER);



/******************************************************************
 * show apollo arp
 *
 */
EOLS	(show_apollo_arp_eol, show_apollo, SHOW_APOLLO_ARP);
KEYWORD (show_apollo_arp, show_apollo_arp_eol, show_apollo_interface,
	 "arp", "Apollo Arp table", PRIV_USER);

/******************************************************************
 * show apollo access-lists [<list-number>]
 *
 * OBJ(int,1) = <list-number>
 */
EOLS	(show_apollo_access_eol, show_apollo, SHOW_APOLLO_ACCESS);
KEYWORD (show_apollo_access, show_apollo_access_eol, show_apollo_arp,
	 "access-lists", "Apollo access lists", PRIV_USER);



/******************************************************************/

KEYWORD (show_apollo, show_apollo_access, ALTERNATE,
	 "apollo", "Apollo network information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_apollo
