/* $Id: cfg_routemap_match_ip.h,v 3.3.52.1 1996/05/04 01:21:11 wilber Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_match_ip.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_match_ip.h -- IP specific route-map clauses
 *
 * March 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_match_ip.h,v $
 * Revision 3.3.52.1  1996/05/04  01:21:11  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3  1995/11/28  19:50:33  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:15:59  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:44  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * match ip address <access-list> [<access-list> ... <access-list>]
 * match ip next-hop <access-list> [<access-list> ... <access-list>]
 * match ip route-source <access-list> [<access-list> ... <access-list>]
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(idb,1) = interface (for "match interface" command only)
 * OBJ(int,2) = IP access-list or as-path access list.
 * OBJ(int,3) = Is set to TRUE when a named list is parsed.
 */

pdecl(routemap_match_ip_routesource_num);
pdecl(routemap_match_ip_nexthop_num);
pdecl(routemap_match_ip_address_num);

LINK_EXIT(routemap_match_ip_exit, no_alt);

/*
 * match ip route-source
 */
FUNC	(routemap_match_ip_routesource_func,
	 routemap_match_ip_routesource_num,
	 route_map_match_routesource_command);
SET      (routemap_match_ip_routesource_alpha,
	  routemap_match_ip_routesource_func,
	  OBJ(int,3), TRUE);
GENERAL_STRING (routemap_match_ip_routesource_accname,
		routemap_match_ip_routesource_alpha, 
		routemap_match_ip_exit, OBJ(string,1),
		"IP standard access-list name", STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS(routemap_match_ip_routesource_stest,
		    routemap_match_ip_routesource_accname, NONE, no_alt);
SET      (routemap_match_ip_routesource_numeric,
	  routemap_match_ip_routesource_func, 
	  OBJ(int,3), FALSE);
NUMBER	(routemap_match_ip_routesource_num,
	 routemap_match_ip_routesource_numeric,
	 routemap_match_ip_routesource_stest, 
	 OBJ(int,2), 1, MAXFASTACC, "IP access-list number");
EVAL	(routemap_match_ip_routesource_free, routemap_match_ip_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ROUTESOURCE, GETOBJ(int,1)));
NOPREFIX(routemap_match_ip_routesource_noprefix, routemap_match_ip_exit,
	 routemap_match_ip_routesource_free);
IFELSE	(routemap_match_ip_routesource_ifelse,
	 routemap_match_ip_routesource_noprefix,
	 routemap_match_ip_routesource_num, eol_check(csb));
KEYWORD	(routemap_match_ip_routesource, routemap_match_ip_routesource_ifelse,
	 no_alt, "route-source",
	 "Match advertising source address of route", PRIV_CONF);

/*
 * match ip next-hop
 */
FUNC	(routemap_match_ip_nexthop_func,
	 routemap_match_ip_nexthop_num,
	 route_map_match_nexthop_command);
SET      (routemap_match_ip_nexthop_alpha, routemap_match_ip_nexthop_func,
	  OBJ(int,3), TRUE);
GENERAL_STRING (routemap_match_ip_nexthop_accname,
		routemap_match_ip_nexthop_alpha, 
		routemap_match_ip_exit, OBJ(string,1),
		"IP standard access-list name", STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS (routemap_match_ip_nexthop_stest,
		    routemap_match_ip_nexthop_accname, NONE, no_alt);
SET      (routemap_match_ip_nexthop_numeric,
	  routemap_match_ip_nexthop_func, 
	  OBJ(int,3), FALSE);
NUMBER	(routemap_match_ip_nexthop_num, routemap_match_ip_nexthop_numeric,
	 routemap_match_ip_nexthop_stest,
	 OBJ(int,2), 1, MAXFASTACC, "IP access-list number");
EVAL	(routemap_match_ip_nexthop_free, routemap_match_ip_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_NEXTHOP, GETOBJ(int,1)));
NOPREFIX(routemap_match_ip_nexthop_noprefix, routemap_match_ip_exit,
	 routemap_match_ip_nexthop_free);
IFELSE	(routemap_match_ip_nexthop_ifelse, routemap_match_ip_nexthop_noprefix,
	 routemap_match_ip_nexthop_num, eol_check(csb));
KEYWORD	(routemap_match_ip_nexthop, routemap_match_ip_nexthop_ifelse,
	 routemap_match_ip_routesource,
	 "next-hop", "Match next-hop address of route", PRIV_CONF);

/*
 * match ip address
 */
FUNC	(routemap_match_ip_address_func,
	 routemap_match_ip_address_num,
	 route_map_match_address_command);
SET      (routemap_match_ip_address_alpha, routemap_match_ip_address_func,
	  OBJ(int,3), TRUE);
GENERAL_STRING (routemap_match_ip_address_accname,
		routemap_match_ip_address_alpha, 
		routemap_match_ip_exit, OBJ(string,1),
		"IP access-list name", STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS(routemap_match_ip_address_stest,
		    routemap_match_ip_address_accname, NONE, no_alt);
SET      (routemap_match_ip_address_numeric,
	  routemap_match_ip_address_func, 
	  OBJ(int,3), FALSE);
NUMBER	(routemap_match_ip_address_num, routemap_match_ip_address_numeric,
	 routemap_match_ip_address_stest,
	 OBJ(int,2), 1, MAXSLOWACC, "IP access-list number");
EVAL	(routemap_match_ip_address_free, routemap_match_ip_exit,
	 route_map_match_free_address(csb->route_map,
				      ROUTEMAP_ADDRESS, GETOBJ(int,1)));
NOPREFIX(routemap_match_ip_address_noprefix, routemap_match_ip_exit,
	 routemap_match_ip_address_free);
IFELSE	(routemap_match_ip_address_ifelse, routemap_match_ip_address_noprefix,
	 routemap_match_ip_address_num, eol_check(csb));
KEYWORD	(routemap_match_ip_address, routemap_match_ip_address_ifelse,
	 routemap_match_ip_nexthop,
	 "address", "Match address of route or match packet", PRIV_CONF);

/*
 * ip keyword
 */
KEYWORD_ID (routemap_match_ip, routemap_match_ip_address, ALTERNATE,
	    OBJ(int,1), 0,
	    "ip", "IP specific information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_match_ip
