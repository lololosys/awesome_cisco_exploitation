/* $Id: cfg_routemap_match_clns.h,v 3.3 1995/11/28 19:50:22 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/cfg_routemap_match_clns.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_match_clns.h -- CLNS specific route-map match commands
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_match_clns.h,v $
 * Revision 3.3  1995/11/28  19:50:22  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  08:51:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:15:54  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:08  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:16:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * match clns address <access-list> [<access-list> ... <access-list>]
 * match clns next-hop <access-list> [<access-list> ... <access-list>]
 * match clns route-source <access-list> [<access-list> ... <access-list>]
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(idb,1) = interface (for "match interface" command only)
 * OBJ(int,2) = IP access-list or as-path access list.
 * OBJ(string,1) = OSI access-list.
 * OBJ(int,2) = number for metric or tag.
 * OBJ(int,2) = 0 is internal, 1 is external, 2 is level-1, 3 is level-2.
 */

pdecl(routemap_match_clnp_routesource_string);
pdecl(routemap_match_clnp_nexthop_string);
pdecl(routemap_match_clnp_address_string);

LINK_EXIT(routemap_match_clnp_exit, no_alt);

/*
 * match clns route-source
 */
FUNC	(routemap_match_clnp_routesource_func,
	 routemap_match_clnp_routesource_string,
	 route_map_match_routesource_command);
STRING	(routemap_match_clnp_routesource_string,
	 routemap_match_clnp_routesource_func,
	 routemap_match_clnp_exit, OBJ(string,1), "OSI filter set");
EVAL	(routemap_match_clnp_routesource_free, routemap_match_clnp_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ROUTESOURCE, GETOBJ(int,1)));
NOPREFIX(routemap_match_clnp_routesource_noprefix, routemap_match_clnp_exit,
	 routemap_match_clnp_routesource_free);
IFELSE	(routemap_match_clnp_routesource_ifelse,
	 routemap_match_clnp_routesource_noprefix,
	 routemap_match_clnp_routesource_string, eol_check(csb));
KEYWORD	(routemap_match_clnp_routesource,
	 routemap_match_clnp_routesource_ifelse, no_alt,
	 "route-source", "Match advertising source address of route",
	 PRIV_CONF);

/*
 * match clns next-hop
 */
FUNC	(routemap_match_clnp_nexthop_func,
	 routemap_match_clnp_nexthop_string,
	 route_map_match_nexthop_command);
STRING	(routemap_match_clnp_nexthop_string,
	 routemap_match_clnp_nexthop_func,
	 routemap_match_clnp_exit, OBJ(string,1), "OSI filter set");
EVAL	(routemap_match_clnp_nexthop_free, routemap_match_clnp_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_NEXTHOP, GETOBJ(int,1)));
NOPREFIX(routemap_match_clnp_nexthop_noprefix, routemap_match_clnp_exit,
	 routemap_match_clnp_nexthop_free);
IFELSE	(routemap_match_clnp_nexthop_ifelse,
	 routemap_match_clnp_nexthop_noprefix,
	 routemap_match_clnp_nexthop_string, eol_check(csb));
KEYWORD	(routemap_match_clnp_nexthop, routemap_match_clnp_nexthop_ifelse,
	 routemap_match_clnp_routesource,
	 "next-hop", "Match next-hop address of route", PRIV_CONF);

/*
 * match clns address
 */
FUNC	(routemap_match_clnp_address_func,
	 routemap_match_clnp_address_string,
	 route_map_match_address_command);
STRING	(routemap_match_clnp_address_string,
	 routemap_match_clnp_address_func,
	 routemap_match_clnp_exit, OBJ(string,1), "OSI filter set");
EVAL	(routemap_match_clnp_address_free, routemap_match_clnp_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ADDRESS, GETOBJ(int,1)));
NOPREFIX(routemap_match_clnp_address_noprefix, routemap_match_clnp_exit,
	 routemap_match_clnp_address_free);
IFELSE	(routemap_match_clnp_address_ifelse,
	 routemap_match_clnp_address_noprefix,
	 routemap_match_clnp_address_string, eol_check(csb));
KEYWORD	(routemap_match_clnp_address, routemap_match_clnp_address_ifelse,
	 routemap_match_clnp_nexthop,
	 "address", "Match address of route or match packet", PRIV_CONF);

/*
 * clns keyword.
 */
KEYWORD_ID (routemap_match_clnp, routemap_match_clnp_address, ALTERNATE,
	    OBJ(int,1), 1,
	    "clns", "CLNS information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_match_clnp
