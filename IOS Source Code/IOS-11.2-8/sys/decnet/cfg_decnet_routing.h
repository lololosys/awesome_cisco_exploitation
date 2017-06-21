/* $Id: cfg_decnet_routing.h,v 3.2.60.2 1996/05/30 23:42:31 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/cfg_decnet_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ D E C N E T _ R O U T I N G . H
 *
 * Parse chain info for DECnet static route and host * commands.
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_decnet_routing.h,v $
 * Revision 3.2.60.2  1996/05/30  23:42:31  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:11:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:19:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:44:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:58:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/24  06:17:29  asastry
 * CSCdi36662:  Add a knob to override the behavior of the "L2 attached
 * bit". This is needed to allow L2 routers that are also doing IV/V
 * conversion to advertise the route to the "nearest L2 router" even when
 * they have no Phase IV area reachability.
 *
 * Revision 2.1  1995/07/02  01:44:24  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(conf_decnet_routing_extend_here, no_alt);

/******************************************************************************
 * dec route <addr> | default <next-hop_addr> [hops][cost]
 * dec route <addr> | default <interface> [hops][cost]
 * dec route <addr> | default discard
 */

EOLS	(decnet_route_eol, decnet_command, DC_ROUTE);

NUMBER  (decnet_route_cost, decnet_route_eol, decnet_route_eol,
         OBJ(int,4), 0, DN_MAX_AREA_COST, "Cost for static route");
NUMBER  (decnet_route_hops, decnet_route_cost, decnet_route_eol,
         OBJ(int,3), 0, DN_MAX_AREA_HOPS, "Hop count for static route");

SET	(decnet_route_name_set, decnet_route_hops, OBJ(int,2), DC_ROUTE_NAME);
DNADDR(decnet_route_name, decnet_route_name_set, no_alt,
	 OBJ(paddr,2), "DECnet address of next hop router");

KEYWORD_ID(decnet_route_discard, decnet_route_eol, decnet_route_name,
	   OBJ(int,2), DC_ROUTE_DISCARD,
	   "discard", "Discard packets with the DECnet address", PRIV_CONF);

SNPAADDR(decnet_route_int_snpa, decnet_route_hops, no_alt,
	 OBJ(hwaddr,1), "SNPA address");
IFELSE	(decnet_route_int_eval, decnet_route_hops, decnet_route_int_snpa,
	 is_p2p(GETOBJ(idb,1)));
SET	(decnet_route_int_set, decnet_route_int_eval, OBJ(int,2),
	 DC_ROUTE_INTERFACE);
INTERFACE(decnet_route_int, decnet_route_int_set, decnet_route_discard,
	  OBJ(idb,1), IFTYPE_ANYSUB);

DNADDR(decnet_route_addr, decnet_route_int, NONE, OBJ(paddr,1),
       "DECnet address");
EVAL	(decnet_route_default_eval, decnet_route_int,
	 (GETOBJ(paddr,1)->addr.decnet_address = 0,
	  GETOBJ(paddr,1)->type = ADDR_DECNET,
	  GETOBJ(paddr,1)->length = ADDRLEN_DECNET)); 
KEYWORD	(decnet_route_default, decnet_route_default_eval,
         decnet_route_addr, "default", "Default prefix", PRIV_CONF);
NVGENS	(decnet_route_nvgen, decnet_route_default, decnet_command, DC_ROUTE); 
KEYWORD	(decnet_route, decnet_route_nvgen, conf_decnet_routing_extend_here,
	 "route", "Define a DECnet static route", PRIV_CONF);

/******************************************************************************
 * decnet propagate static
 * no  decnet propagate static
 *
 */
EOLS	(decnet_redis_eol, decnet_command, DC_PROPAGATE);
KEYWORD	(decnet_redis_kwd, decnet_redis_eol, no_alt,
	 "static", "Propagate DECnet static routes", PRIV_CONF);
NVGENS	(decnet_redis_nvgen, decnet_redis_kwd, decnet_command, DC_PROPAGATE);
KEYWORD	(decnet_redistribute, decnet_redis_nvgen, decnet_route,
	 "propagate", "Allow DECnet static routes to be sent in routing updates",
	 PRIV_CONF);

/******************************************************************************
 * decnet attach override
 * no decnet attach override
 *
 */
EOLS	(decnet_attach_eol, decnet_command, DC_ATTACH_OVERRIDE);
KEYWORD	(decnet_attach_kwd, decnet_attach_eol, no_alt,
	 "override", "Override the 'unattached' state", PRIV_CONF);
NVGENS	(decnet_attach_nvgen, decnet_attach_kwd, decnet_command, DC_ATTACH_OVERRIDE);
KEYWORD	(decnet_attach_override, decnet_attach_nvgen, decnet_redistribute,
	 "attach", "Allow an area router to act 'attached'",
	 PRIV_CONF);

/******************************************************************************
 * decnet host <name> <OBJ(paddr,1)> 
 * no decnet host <name>
 *
 */
EOLS	(decnet_host_eol, decnet_command, DC_HOST);

DNADDR(decnet_host_addr, decnet_host_eol, no_alt,
	 OBJ(paddr,1), "DECnet address to assign name to");
NOPREFIX(decnet_host_noprefix, decnet_host_addr, decnet_host_eol);
STRING	(decnet_host_name, decnet_host_noprefix, no_alt,
	 OBJ(string,1), "Name to give to a DECnet address");
NVGENS	(decnet_host_nvgen, decnet_host_name, decnet_command, DC_HOST);
KEYWORD	(decnet_host, decnet_host_nvgen, decnet_attach_override,
	 "host", "Define a name mapping for a DECnet address", PRIV_CONF);

KEYWORD (cfg_decnet_routing, decnet_host, ALTERNATE,
	 "decnet", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cfg_decnet_routing


