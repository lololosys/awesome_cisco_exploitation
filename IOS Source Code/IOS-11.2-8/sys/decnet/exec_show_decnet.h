/* $Id: exec_show_decnet.h,v 3.2.60.3 1996/06/24 22:31:18 perryl Exp $
 * $Source: /release/112/cvs/Xsys/decnet/exec_show_decnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ D E C N E T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_decnet.h,v $
 * Revision 3.2.60.3  1996/06/24  22:31:18  perryl
 * CSCdi57969:  show ip interface not showing Vaccess
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/30  23:42:55  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/21  09:44:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  08:59:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:23:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show decnet 
 */
EOLS	(show_decnet_traffic_global, show_decnet, SHOW_DECNET_GLOBAL);

/******************************************************************
 * show decnet traffic
 * 
 */

EOLS	(show_decnet_traffic_eol, show_decnet, SHOW_DECNET_TRAFFIC);
KEYWORD (show_decnet_traffic, show_decnet_traffic_eol, 
         show_decnet_traffic_global, "traffic", "DECnet traffic statistics", 
         PRIV_USER);

/******************************************************************
 * show decnet route [<decnet-addr>]
 * 
 */

EOLS	(show_decnet_route_eol, show_decnet, SHOW_DECNET_ROUTE);
DNADDR (show_decnet_route_addr, show_decnet_route_eol, show_decnet_route_eol,
	OBJ(paddr,1),"DECnet Address");
KEYWORD (show_decnet_route, show_decnet_route_addr, show_decnet_traffic,
	 "route", "DECnet routing table", PRIV_USER);

/******************************************************************
 * show decnet neighbors
 * 
 */

EOLS	(show_decnet_neighbors_eol, show_decnet, SHOW_DECNET_NEIGHBORS);
KEYWORD (show_decnet_neighbors, show_decnet_neighbors_eol, show_decnet_route,
	 "neighbors", "DECnet adjacent neighbors", PRIV_USER);

/******************************************************************
 * show decnet map
 * 
 */

EOLS	(show_decnet_map_eol, show_decnet, SHOW_DECNET_MAP);
KEYWORD (show_decnet_map, show_decnet_map_eol, show_decnet_neighbors,
	 "map", "DECnet Address Translation Gateway table", PRIV_USER);

/******************************************************************
 * show decnet interface [<interface>]
 * 
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_decnet_interface_eol, show_decnet, SHOW_DECNET_INTERFACE);
INTERFACE (show_decnet_get_interface, show_decnet_interface_eol, show_decnet_interface_eol,
	   OBJ(idb,1), (IFTYPE_ANYSUB | IFTYPE_VACCESS));
KEYWORD (show_decnet_interface, show_decnet_get_interface, show_decnet_map,
	 common_str_interface, "DECnet interface status and configuration", PRIV_USER);


/*****************************************************************
 * show decnet static [<decnet-addr>]
 */
EOLS	(show_decnet_static_eol, show_decnet, SHOW_DECNET_STATIC);
DNADDR  (show_decnet_static_addr, show_decnet_static_eol, show_decnet_static_eol,
	 OBJ(paddr,1),"DECnet Address");
KEYWORD (show_decnet_static, show_decnet_static_addr, show_decnet_interface,
	"static", "Display all static routes in static route queue",
	 PRIV_USER);

/*****************************************************************
 * show decnet debug
 */
EOLS	(show_decnet_debug_eol, show_decnet, SHOW_DECNET_DEBUG);
KEYWORD (show_decnet_debug, show_decnet_debug_eol, show_decnet_static,
	"debug", "Incomplete network structures for each network", PRIV_USER);

/******************************************************************
 * show decnet access-lists [<list-number>]
 *
 * OBJ(int,1) = <list-number>
 */
EOLS	(show_decnet_access_eol, show_decnet, SHOW_DECNET_ACCESS);
ACCESS_LIST(show_decnet_access_list, show_decnet_access_eol, show_decnet_access_eol,
	   OBJ(int,2), MINDECACC, MAXDECACC,
	   DN_MIN_ACL, DN_MAX_ACL, "Access list number");
KEYWORD (show_decnet_access, show_decnet_access_list, show_decnet_debug,
	 "access-lists", "Decnet access lists", PRIV_USER);

/*****************************************************************
 *
 * Optional ATG network number.  The default is -1, indicating no number.
 */
SET	(show_decnet_atg_default, show_decnet_access, OBJ(int,1), -1);
NUMBER	(show_decnet_atg, show_decnet_debug, show_decnet_atg_default,
	 OBJ(int,1), 0, 3, "Optional ATG network number");

/******************************************************************
 * show decnet
 */
EOLS	(show_decnet_eol, show_decnet, SHOW_DECNET);
KEYWORD (show_decnet, show_decnet_atg, ALTERNATE,
	 "decnet", "DECnet information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_decnet



