/* $Id: exec_show_xns.h,v 3.3.20.2 1996/05/30 23:48:12 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_xns.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ X N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_xns.h,v $
 * Revision 3.3.20.2  1996/05/30  23:48:12  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/21  10:11:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/01  23:35:13  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:21:44  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show xns traffic
 */
EOLS	(show_xns_traffic_eol, show_xns,        SHOW_XNS_TRAFFIC);
KEYWORD (show_xns_traffic, show_xns_traffic_eol, no_alt,
            "traffic", "XNS protocol statistics", PRIV_USER);

/******************************************************************
 * show xns route [<network>]
 *
 * OBJ(int,1) = <network>
 *		0 if no network
 */
EOLS	(show_xns_route_eol, show_xns, SHOW_XNS_ROUTE);
NUMBER	(show_xns_route_addr, show_xns_route_eol, show_xns_route_eol,
	 OBJ(int,1), 1, -1, "Show route to a specific XNS network");
KEYWORD (show_xns_route, show_xns_route_addr, show_xns_traffic,
	 "route", "XNS routing table", PRIV_USER);

/******************************************************************
 * show xns interface [<interface>]
 *
 * OBJ(int,1) = TRUE if <interface> exists
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_xns_interface_eol, show_xns, SHOW_XNS_INTERFACE);
SET       (show_xns_ifname_set, show_xns_interface_eol,
           OBJ(int,1), TRUE);
INTERFACE (show_xns_ifname, show_xns_ifname_set, show_xns_interface_eol,
           OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_xns_interface, show_xns_ifname, show_xns_route,
            common_str_interface, "XNS interface status and configuration", PRIV_USER);

/******************************************************************
 * show xns cache
 */
EOLS	(show_xns_cache_eol, show_xns, SHOW_XNS_CACHE);
KEYWORD (show_xns_cache, show_xns_cache_eol, show_xns_interface,
            "cache", "XNS fast-switching cache", PRIV_USER);

/******************************************************************
 * show xns access-lists [<list-number>]
 *
 * OBJ(int,1) = <list-number>
 */
EOLS	(show_xns_access_eol, show_xns, SHOW_XNS_ACCESS);
ACCESS_LIST(show_xns_access_list, show_xns_access_eol, show_xns_access_eol,
	   OBJ(int,1), MINXNSACC, MAXEXNSACC,
	   XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL, "Access list number");
KEYWORD (show_xns_access, show_xns_access_list, show_xns_cache,
	 "access-lists", "IPX access lists", PRIV_USER);

/******************************************************************
 * show xns
 */
KEYWORD (show_xns_kwd, show_xns_access, ALTERNATE,
	 "xns", "XNS information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_xns_kwd
