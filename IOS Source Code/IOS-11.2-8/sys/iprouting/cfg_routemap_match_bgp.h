/* $Id: cfg_routemap_match_bgp.h,v 3.3 1995/11/28 19:50:31 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_routemap_match_bgp.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_match_bgp.h -- BGP specific route map match clauses
 *
 * March 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_match_bgp.h,v $
 * Revision 3.3  1995/11/28  19:50:31  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:15:58  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:43  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * match as-path <as-path-list>
 * match community <community-list> [exact-match]
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(idb,1) = interface (for "match interface" command only)
 * OBJ(int,2) = IP access-list or as-path access list.
 * OBJ(string,1) = OSI access-list.
 * OBJ(int,2) = number for metric or tag.
 * OBJ(int,2) = 0 is internal, 1 is external, 2 is level-1, 3 is level-2.
 */

pdecl(routemap_match_aspath_number);
pdecl(routemap_match_std_community_number);
pdecl(routemap_match_ext_community_number);

LINK_EXIT(routemap_match_bgp_exit, no_alt);

/*
 * match community
 */
FUNC	(routemap_match_community_ex_func,
	 routemap_match_bgp_exit,
	 route_map_match_community_command);
KEYWORD_ID(routemap_match_community_exact, routemap_match_community_ex_func,
	   routemap_match_bgp_exit,
	   OBJ(int,3), TRUE,
	   "exact-match", "Do exact matching of communities", PRIV_CONF);
FUNC    (routemap_match_community_func,
         routemap_match_std_community_number,
         route_map_match_community_command);
NUMBER  (routemap_match_ext_community_number, routemap_match_community_func,
         routemap_match_community_exact, OBJ(int,2), MAX_STD_COMLIST+1,
         MAX_EXT_COMLIST, "Community-list number (extended)");
NUMBER  (routemap_match_std_community_number,
         routemap_match_community_func, routemap_match_ext_community_number,
         OBJ(int,2), 1, MAX_STD_COMLIST, "Community-list number (standard)");
SET	(routemap_match_community_args, routemap_match_std_community_number,
	 OBJ(int,4), ROUTEMAP_MATCH_COMMARGS);
FUNC	(routemap_match_community_init,
	 routemap_match_community_args,
	 route_map_match_community_command);
SET	(routemap_match_community_begin, routemap_match_community_init,
	 OBJ(int,4), ROUTEMAP_MATCH_COMMARGS_BEGIN);
FUNC	(routemap_match_community_free,
	 routemap_match_bgp_exit,
	 route_map_free_community);
NOPREFIX(routemap_match_community_noprefix, routemap_match_bgp_exit,
	 routemap_match_community_free);
IFELSE	(routemap_match_community_ifelse, routemap_match_community_noprefix,
	 routemap_match_community_begin, eol_check(csb));
KEYWORD	(routemap_match_community, routemap_match_community_ifelse, ALTERNATE,
	 "community", "Match BGP community list", PRIV_CONF);

/*
 * match as-path
 */
FUNC	(routemap_match_aspath_func,
	 routemap_match_aspath_number,
	 route_map_match_aspath_command);
NUMBER	(routemap_match_aspath_number, routemap_match_aspath_func,
	 routemap_match_bgp_exit,
	 OBJ(int,2), MINREGEXPACC+1, MAXREGEXPACC, "AS path access-list");
EVAL	(routemap_match_aspath_free, routemap_match_bgp_exit,
	 route_map_free_item(&(csb->route_map)->match_list.as_path));
NOPREFIX(routemap_match_aspath_noprefix, routemap_match_bgp_exit,
	 routemap_match_aspath_free);
IFELSE	(routemap_match_aspath_ifelse, routemap_match_aspath_noprefix,
	 routemap_match_aspath_number, eol_check(csb));
KEYWORD	(routemap_match_aspath, routemap_match_aspath_ifelse,
	 routemap_match_community,
	 "as-path", "Match BGP AS path list", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_match_aspath
