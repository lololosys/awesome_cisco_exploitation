/* $Id: cfg_routemap_match.h,v 3.3.52.2 1996/08/12 16:03:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_match.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_match.h
 *
 * Process route-map match subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_match.h,v $
 * Revision 3.3.52.2  1996/08/12  16:03:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.52.1  1996/05/21  09:53:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/11/28  19:50:30  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/15  15:19:12  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:12:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * match interface <name-unit> [<name-unit> ... <name-unit>]
 * match metric <metric-value> [<metric-value>] ... <metric-value>]
 * match route-type [local | internal | external [type-1 | type-2] |
 *                  level-1 | level-2] [....]
 * match tag <tag-value> [<tag-value> ... <tag-value>]
 *
 * Obsolete and hidden:
 * match address <access-list> [<access-list> ... <access-list>]
 * match next-hop <access-list> [<access-list> ... <access-list>]
 * match route-source <access-list> [<access-list> ... <access-list>]
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(idb,1) = interface (for "match interface" command only)
 * OBJ(int,2) = IP access-list or as-path access list.
 * OBJ(string,1) = OSI access-list.
 * OBJ(int,2) = number for metric or tag.
 * OBJ(int,2) = 0 is internal, 1 is external, 2 is level-1, 3 is level-2.
 */

pdecl(routemap_match_routesource_num);
pdecl(routemap_match_nexthop_num);
pdecl(routemap_match_address_num);
pdecl(routemap_match_routetype_internal);
pdecl(routemap_match_tag_number);
pdecl(routemap_match_metric_number);
pdecl(routemap_match_interface_idb);

LINK_TRANS(routemap_match_extend_here, no_alt);

EOLNS (routemap_match_eols, route_map_eols);
LINK_TRANS(routemap_match_return, routemap_match_eols);

/*
 * match route-source
 */
FUNC	(routemap_match_routesource_func,
	 routemap_match_routesource_num,
	 route_map_match_routesource_command);
NUMBER	(routemap_match_routesource_num, routemap_match_routesource_func,
	 routemap_match_eols,
	 OBJ(int,2), 1, MAXFASTACC, "IP access-list number");
EVAL	(routemap_match_routesource_free, routemap_match_eols,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ROUTESOURCE, GETOBJ(int,1)));
NOPREFIX(routemap_match_routesource_noprefix, routemap_match_eols,
	 routemap_match_routesource_free);
IFELSE	(routemap_match_routesource_ifelse, routemap_match_routesource_noprefix,
	 routemap_match_routesource_num, eol_check(csb));
KEYWORD	(routemap_match_routesource, routemap_match_routesource_ifelse,
	 routemap_match_extend_here,
	 "route-source", "Match advertising source address of route",
	 PRIV_CONF | PRIV_HIDDEN);

/*
 * match next-hop
 */
FUNC	(routemap_match_nexthop_func,
	 routemap_match_nexthop_num,
	 route_map_match_nexthop_command);
NUMBER	(routemap_match_nexthop_num, routemap_match_nexthop_func,
	 routemap_match_eols,
	 OBJ(int,2), 1, MAXFASTACC, "IP access-list number");
EVAL	(routemap_match_nexthop_free, routemap_match_eols,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_NEXTHOP, GETOBJ(int,1)));
NOPREFIX(routemap_match_nexthop_noprefix, routemap_match_eols,
	 routemap_match_nexthop_free);
IFELSE	(routemap_match_nexthop_ifelse, routemap_match_nexthop_noprefix,
	 routemap_match_nexthop_num, eol_check(csb));
KEYWORD	(routemap_match_nexthop, routemap_match_nexthop_ifelse,
	 routemap_match_routesource,
	 "next-hop", "Match next-hop address of route",
	 PRIV_CONF | PRIV_HIDDEN);

/*
 * match address
 */
FUNC	(routemap_match_address_func,
	 routemap_match_address_num,
	 route_map_match_address_command);
NUMBER	(routemap_match_address_num, routemap_match_address_func,
	 routemap_match_eols,
	 OBJ(int,2), 1, MAXSLOWACC, "IP access-list number");
EVAL	(routemap_match_address_free, routemap_match_eols,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ADDRESS, GETOBJ(int,1)));
NOPREFIX(routemap_match_address_noprefix, routemap_match_eols,
	 routemap_match_address_free);
IFELSE	(routemap_match_address_ifelse, routemap_match_address_noprefix,
	 routemap_match_address_num, eol_check(csb));
KEYWORD	(routemap_match_address, routemap_match_address_ifelse,
	 routemap_match_nexthop,
	 "address", "Match address of route or match packet",
	 PRIV_CONF | PRIV_HIDDEN);

/*
 * match route-type
 */
FUNC	(routemap_match_routetype_func,
	 routemap_match_routetype_internal,
	 route_map_match_routetype_command);
KEYWORD_ID (routemap_match_routetype_local, routemap_match_routetype_func,
	    routemap_match_eols,
	    OBJ(int,2), ROUTE_MAP_LOCAL,
	    "local", "locally generated route", PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_level2, routemap_match_routetype_func,
	    routemap_match_routetype_local,
	    OBJ(int,2), ROUTE_MAP_LEVEL2,
	    "level-2", "IS-IS level-2 route", PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_level1, routemap_match_routetype_func,
	    routemap_match_routetype_level2,
	    OBJ(int,2), ROUTE_MAP_LEVEL1,
	    "level-1", "IS-IS level-1 route", PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_type2, routemap_match_routetype_func,
	    routemap_match_routetype_func,
	    OBJ(int,2), ROUTE_MAP_EX_TYPE2,
	    "type-2", "OSPF external type 2 route", PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_type1, routemap_match_routetype_func,
	    routemap_match_routetype_type2,
	    OBJ(int,2), ROUTE_MAP_EX_TYPE1,
	    "type-1", "OSPF external type 1 route", PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_external, routemap_match_routetype_type1,
	    routemap_match_routetype_level1,
	    OBJ(int,2), ROUTE_MAP_EXTERNAL,
	    "external", "external route (BGP, EIGRP and OSPF type 1/2)",
	    PRIV_CONF);
KEYWORD_ID (routemap_match_routetype_internal, routemap_match_routetype_func,
	    routemap_match_routetype_external,
	    OBJ(int,2), ROUTE_MAP_INTERNAL,
	    "internal", "internal route (including OSPF intra/inter area)",
	    PRIV_CONF);
EVAL	(routemap_match_routetype_free, routemap_match_eols,
	 route_map_free_item(&(csb->route_map)->match_list.route_type));
NOPREFIX(routemap_match_routetype_noprefix, routemap_match_eols,
	 routemap_match_routetype_free);
IFELSE	(routemap_match_routetype_ifelse, routemap_match_routetype_noprefix,
	 routemap_match_routetype_internal, eol_check(csb));
KEYWORD	(routemap_match_routetype, routemap_match_routetype_ifelse,
	 routemap_match_address,
	 "route-type", "Match route-type of route", PRIV_CONF);

/*
 * match tag
 */
FUNC	(routemap_match_tag_func,
	 routemap_match_tag_number,
	 route_map_match_tag_command);
NUMBER	(routemap_match_tag_number, routemap_match_tag_func,
	 routemap_match_eols,
	 OBJ(int,2), 0, MAXUINT, "Tag value");
EVAL	(routemap_match_tag_free, routemap_match_eols,
	 route_map_free_item(&(csb->route_map)->match_list.tag));
NOPREFIX(routemap_match_tag_noprefix, routemap_match_eols,
	 routemap_match_tag_free);
IFELSE	(routemap_match_tag_ifelse, routemap_match_tag_noprefix,
	 routemap_match_tag_number, eol_check(csb));
KEYWORD	(routemap_match_tag, routemap_match_tag_ifelse,
	 routemap_match_routetype,
	 "tag", "Match tag of route", PRIV_CONF);

/*
 * match metric
 */
FUNC	(routemap_match_metric_func,
	 routemap_match_metric_number,
	 route_map_match_metric_command);
NUMBER (routemap_match_metric_number, routemap_match_metric_func,
	routemap_match_eols,
	OBJ(int,2), 0, 0xffffffff, "Metric value");
EVAL	(routemap_match_metric_free, routemap_match_eols,
	 route_map_free_item(&(csb->route_map)->match_list.metric));
NOPREFIX(routemap_match_metric_noprefix, routemap_match_eols,
	 routemap_match_metric_free);
IFELSE	(routemap_match_metric_ifelse, routemap_match_metric_noprefix,
	 routemap_match_metric_number, eol_check(csb));
KEYWORD	(routemap_match_metric, routemap_match_metric_ifelse,
	 routemap_match_tag,
	 "metric", "Match metric of route", PRIV_CONF);

/*
 * match interface
 */
FUNC	(routemap_match_interface_func,
	 routemap_match_interface_idb,
	 route_map_match_interface_command);
INTERFACE (routemap_match_interface_idb, routemap_match_interface_func,
	   routemap_match_eols, OBJ(idb,1), IFTYPE_ANYSUB);
EVAL	(routemap_match_interface_free, routemap_match_eols,
	 route_map_free_item((route_map_match_value **)
			     &(csb->route_map)->match_list.interface));
NOPREFIX(routemap_match_interface_noprefix, routemap_match_eols,
	 routemap_match_interface_free);
IFELSE	(routemap_match_interface_ifelse, routemap_match_interface_noprefix,
	 routemap_match_interface_idb, eol_check(csb));
KEYWORD	(routemap_match_interface, routemap_match_interface_ifelse,
	 routemap_match_metric,
	 common_str_interface, "Match first hop interface of route", PRIV_CONF);

NVGENNS (routemap_match_nvgen, routemap_match_interface,
	route_map_match_nvgen);
KEYWORD	(routemap_match, routemap_match_nvgen, ALTERNATE,
	 "match", "Match values from routing table", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_match
