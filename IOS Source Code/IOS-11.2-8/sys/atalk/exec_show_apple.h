/* $Id: exec_show_apple.h,v 3.4.20.3 1996/06/24 22:31:03 perryl Exp $
 * $Source: /release/112/cvs/Xsys/atalk/exec_show_apple.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A P P L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_apple.h,v $
 * Revision 3.4.20.3  1996/06/24  22:31:03  perryl
 * CSCdi57969:  show ip interface not showing Vaccess
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/30  23:42:13  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/21  09:42:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/01  23:32:54  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/07  17:44:58  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  08:44:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:43:41  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:22:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_atalk_extend_here, no_alt);


/*****************************************************************
 * show apple static
 *
 */

EOLS    (show_apple_static_eol, show_appletalk, SHOW_APPLE_STATIC);
KEYWORD (show_apple_static,show_apple_static_eol, show_atalk_extend_here,
	"static", "AppleTalk static table", PRIV_USER);

/******************************************************************
 * show apple zone [<zone-name>]
 *
 * OBJ(int,1) = 	0 = no zone-name
 *         	1 = zone-name set
 * OBJ(string,1) = zone-name
 */
EOLS	(show_apple_zone_eol, show_appletalk, SHOW_APPLE_ZONES);
SET	(show_apple_zone_set1, show_apple_zone_eol, OBJ(int,1), 1);
TEXT	(show_apple_zone_name, show_apple_zone_set1, show_apple_zone_eol,
	 OBJ(string,1), "Zone name");
KEYWORD (show_apple_zone, show_apple_zone_name, show_apple_static,
	 "zone", "AppleTalk Zone table information", PRIV_USER);

/******************************************************************
 * show apple traffic
 *
 */
EOLS	(show_apple_traffic_eol, show_appletalk, SHOW_APPLE_TRAFFIC);
KEYWORD (show_apple_traffic, show_apple_traffic_eol, show_apple_zone,
	 "traffic", "AppleTalk protocol statistics", PRIV_USER);

/******************************************************************
 * show apple socket [<socket-number>]
 *
 * OBJ(int,1) = 	FALSE = no socket-number
 * 	 	TRUE = socket-number set
 * OBJ(int,2) = socket-number
 */
EOLS	(show_apple_socket_eol, show_appletalk, SHOW_APPLE_SOCKETS);
SET	(show_apple_socket_set1, show_apple_socket_eol, OBJ(int,1), TRUE);
NUMBER	(show_apple_socket_number, show_apple_socket_set1,
	 show_apple_socket_eol,
	 OBJ(int,2), 0, -1, "Socket Number");
KEYWORD (show_apple_socket, show_apple_socket_number, show_apple_traffic,
	 "sockets", "AppleTalk protocol processing information", PRIV_USER);

/******************************************************************
 * show apple route [<network>]
 * show apple route [<interface>]
 *
 * OBJ(int,1) = 	0 = neither
 * 	 	1 = network
 * 	 	2 = interface
 * OBJ(int,2) = network
 * OBJ(idb,1) = <interface>
 *
 */
EOLS	(show_apple_route_eol, show_appletalk, SHOW_APPLE_ROUTE);

SET	(show_apple_route_int_set, show_apple_route_eol, OBJ(int,1), 2);
INTERFACE (show_apple_route_int, show_apple_route_int_set,
	   show_apple_route_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

SET	(show_apple_route_network_set, show_apple_route_eol, OBJ(int,1), 1);
NUMBER	(show_apple_route_network, show_apple_route_network_set,
	 show_apple_route_int,
	 OBJ(int,2), 1, 65279, "Network number");

KEYWORD (show_apple_route, show_apple_route_network, show_apple_socket,
	 "route", "AppleTalk routing table", PRIV_USER);

/******************************************************************
 * show apple private [{globals,interfaces,route-tree,aarp-tree,cache}]
 *
 * Default is to show everything.
 *
 */
EOLS	(show_apple_private_eol, show_appletalk, SHOW_APPLE_PRIVATE);

KEYWORD_ID (show_apple_private_all, show_apple_private_eol,
	    show_apple_private_eol,
	    OBJ(int,1), ATALK_SHOW_PVT_ALL,
	    "all", "All private/detailed AppleTalk information",
	    PRIV_USER|PRIV_HIDDEN);

SET (show_apple_private_set1, show_apple_private_eol, OBJ(int,2), TRUE);
ATADDR	(show_apple_private_addr, show_apple_private_set1,
	 show_apple_private_eol,
	 OBJ(paddr,1), "AppleTalk address argument");

KEYWORD_ID (show_apple_private_cache, show_apple_private_addr,
	    show_apple_private_all,
	    OBJ(int,1), ATALK_SHOW_PVT_CACHE,
	    "cache", "Detailed fast-switching cache information",
	    PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_igrp, show_apple_private_eol,
	    show_apple_private_cache,
	    OBJ(int,1), ATALK_SHOW_PVT_IGRP,
	    "eigrp", "Detailed AT/EIGRP information", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_rtmp, show_apple_private_eol,
  	    show_apple_private_igrp,
	    OBJ(int,1), ATALK_SHOW_PVT_RTMP,
	    "rtmp", "Detailed AT/RTMP information", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_remap, show_apple_private_eol,
  	    show_apple_private_rtmp,
	    OBJ(int,1), ATALK_SHOW_PVT_REMAP,
	    "remap", "AT remap list", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_aarp, show_apple_private_eol,
	    show_apple_private_remap,
	    OBJ(int,1), ATALK_SHOW_PVT_AARP,
	    "aarp", "Detailed AARP tree display", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_route, show_apple_private_eol,
	    show_apple_private_aarp,
	    OBJ(int,1), ATALK_SHOW_PVT_ROUTES,
	    "routes", "Detailed route tree display", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_int, show_apple_private_eol,
	    show_apple_private_route,
	    OBJ(int,1), ATALK_SHOW_PVT_INTERFACES,
	    "interfaces", "Detailed AppleTalk interfaces display",
	    PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (show_apple_private_global, show_apple_private_eol,
	    show_apple_private_int,
	    OBJ(int,1), ATALK_SHOW_PVT_GLOBALS,
	    "globals", "Detailed parameter display", PRIV_USER|PRIV_HIDDEN);

/*
 * Set OBJ(int,1) to a default of the "old" behavior.
 */
KEYWORD_ID(show_apple_private, show_apple_private_global, show_apple_route,
	   OBJ(int,1), ATALK_SHOW_PVT_DEFAULT,
	   "private", NULL, PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show apple neighbor [<neighbor-addr>]
 *
 * OBJ(int,1) =	FALSE = no neighbor-addr
 * 		TRUE = neighbor-addr set
 * (*OBJ(paddr,1)) = neighbor-addr
 */

EOLS	(show_apple_neighbor_eol, show_appletalk, SHOW_APPLE_NEIGHBORS);
SET	(show_apple_neighbor_set1, show_apple_neighbor_eol, OBJ(int,1), TRUE);
ATADDR	(show_apple_neighbor_addr, show_apple_neighbor_set1,
	 show_apple_neighbor_eol,
	 OBJ(paddr,1), "AppleTalk address of neighbor");
KEYWORD (show_apple_neighbor, show_apple_neighbor_addr, show_apple_private,
	 "neighbors", "AppleTalk Neighboring router status", PRIV_USER);

/******************************************************************
 * show apple nbp
 *
 */
EOLS	(show_apple_nbp_eol, show_appletalk, SHOW_APPLE_NBP);
KEYWORD (show_apple_nbp, show_apple_nbp_eol, show_apple_neighbor,
	 "nbp", "AppleTalk NBP name table", PRIV_USER);

/******************************************************************
 * show apple name-cache
 *
 */
EOLS	(show_apple_names_eol, show_appletalk, SHOW_APPLE_NAMES);
KEYWORD (show_apple_names, show_apple_names_eol, show_apple_nbp,
	 "name-cache", "AppleTalk name cache", PRIV_USER);

/******************************************************************
 * show apple map
 *
 */
EOLS	(show_apple_map_eol, show_appletalk, SHOW_APPLE_MAP);
KEYWORD (show_apple_map, show_apple_map_eol, show_apple_names,
	 "map", "AppleTalk WANTalk maps", PRIV_HIDDEN);

/******************************************************************
 * show apple interface [brief] [<interface>]
 *
 * OBJ(int,1) =	FALSE = no interface
 * 		TRUE = interface set
 * OBJ(idb,1) = interface
 */

EOLS	(show_apple_interface_eol, show_appletalk, SHOW_APPLE_INTERFACE);
SET	(show_apple_interface_set1, show_apple_interface_eol,
	 OBJ(int,1), TRUE);
INTERFACE(show_apple_interface_interface, show_apple_interface_set1,
	  show_apple_interface_eol,
	  OBJ(idb,1), (IFTYPE_ANYSUB | IFTYPE_VACCESS));
SET     (show_apple_int_set_brief, show_apple_interface_interface,
         OBJ(int,2), TRUE);
KEYWORD (show_apple_interface_brief_short, show_apple_int_set_brief,
         show_apple_interface_interface,
         "brief",
         "Brief summary of AppleTalk interface status and configuration",
         PRIV_USER);
KEYWORD_MM (show_apple_interface_brief_long, show_apple_int_set_brief,
         show_apple_interface_interface,
         "brief",
         "Brief summary of AppleTalk interface status and configuration",
         PRIV_USER, 4);
TEST_INT(show_apple_interface_brief, show_apple_interface_brief_long,
	 show_apple_interface_brief_short, NONE, nbrinets);
KEYWORD (show_apple_interface, show_apple_interface_brief,
	 show_apple_map,
	 common_str_interface, "AppleTalk interface status and configuration", PRIV_USER);

/*
 * show apple globals
 *
 */
EOLS	(show_apple_globals_eol, show_appletalk, SHOW_APPLE_GLOBALS);
KEYWORD (show_apple_globals, show_apple_globals_eol, show_apple_interface,
	 "globals", "AppleTalk global parameters", PRIV_USER);

/******************************************************************
 * show apple cache
 *
 */
EOLS	(show_apple_cache_eol, show_appletalk, SHOW_APPLE_CACHE);
KEYWORD (show_apple_cache, show_apple_cache_eol, show_apple_globals,
	 "cache", "AppleTalk fast-switching cache", PRIV_USER);

/******************************************************************
 * show apple arp
 *
 */
EOLS	(show_apple_arp_eol, show_appletalk, SHOW_APPLE_ARP);
SET	(show_apple_arp_set1, show_apple_arp_eol, OBJ(int,1), TRUE);
ATADDR	(show_apple_arp_addr, show_apple_arp_set1, show_apple_arp_eol,
	 OBJ(paddr,1), "AppleTalk address");
KEYWORD (show_apple_arp, show_apple_arp_addr, show_apple_cache,
	 "arp", "AppleTalk arp table", PRIV_USER);

/******************************************************************
 * show apple adjacent [<net>]
 *
 * OBJ(int,1) =	FALSE = no network number
 *		TRUE = network number set
 * OBJ(int,2) = <net>
 */
EOLS	(show_apple_adjacent_eol, show_appletalk, SHOW_APPLE_ADJACENT);
SET	(show_apple_adjacent_set, show_apple_adjacent_eol, OBJ(int,1), TRUE);
NUMBER	(show_apple_adjacent_num, show_apple_adjacent_set,
	 show_apple_adjacent_eol,
	 OBJ(int,2), 1, 65279, "Network number");

KEYWORD (show_apple_adjacent, show_apple_adjacent_num, show_apple_arp,
	 "adjacent-routes", "AppleTalk adjacent routes", PRIV_USER);

/******************************************************************
 * show apple access-lists [<list-number>]
 *
 * OBJ(int,1) = <list-number>
 */
EOLS	(show_apple_access_eol, show_appletalk, SHOW_APPLE_ACCESS);
ACCESS_LIST(show_apple_access_list, show_apple_access_eol, show_apple_access_eol,
	   OBJ(int,1), MINATALKACC, MAXATALKACC,
	   AT_MIN_ACL, AT_MAX_ACL, "Access list number");
KEYWORD (show_apple_access, show_apple_access_list, show_apple_adjacent,
	 "access-lists", "AppleTalk access lists", PRIV_USER);

/******************************************************************/
KEYWORD (show_apple, show_apple_access, ALTERNATE,
	 "appletalk", "AppleTalk information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_apple
