/* $Id: exec_show_ip_bgp.h,v 3.4.46.4 1996/08/21 08:03:58 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_bgp.h,v $
 *------------------------------------------------------------------
 * BGP show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_bgp.h,v $
 * Revision 3.4.46.4  1996/08/21  08:03:58  rchandra
 * CSCdi66662:  BGP: Route Reflector not enabled with peer-groups
 * Branch: California_branch
 * - set the reflector-id when peer-group members are configured
 * - in bgp_send_update do not dereference path if there is no best path
 * - all 'show ip bgp community <community>' to work in priv user mode.
 *
 * Revision 3.4.46.3  1996/08/12  16:04:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.46.2  1996/07/29  21:33:15  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.4.46.1  1996/04/16  19:01:29  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/11  07:43:11  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.3  1995/12/07  17:45:14  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  17:33:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:30:54  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/07/01  12:16:17  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:24:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip bgp [<address> [<mask>] [longer-prefixes]]
 * show ip bgp filter-list <list>
 * show ip bgp regexp <regexp>
 * show ip bgp neighbors [<address> [routes | paths | received-routes]]
 * show ip bgp summary
 * show ip bgp paths
 * show ip bgp cidr-only
 * show ip bgp inconsistent-as
 * show ip bgp community <upto 6 communities>
 * show ip bgp community-list <list>
 * 
 * (*OBJ(paddr,1)) = network
 * (*OBJ(paddr,2)) = mask
 * (OBJ(int,1)     = private passthrough
 *		     <list>
 * (OBJ(int,2)	   = flag
 * OBJ(string,1)   = <regexp>
 */
EOLS	(show_ip_bgp_eol, bgp_show, BGP_DEFAULT);


EOLS	(show_ip_bgp_damp_eol, bgp_show, BGP_DAMPENED_ROUTES);
KEYWORD (show_ip_bgp_damp, show_ip_bgp_damp_eol, show_ip_bgp_eol,
	 "dampened-paths", "Display paths suppressed due to dampening", 
	 PRIV_USER);

EOLS	(show_ip_bgp_comlist_eol, bgp_show, BGP_COMMUNITY_LIST);
KEYWORD_ID(show_ip_bgp_comlist_exact, show_ip_bgp_comlist_eol, 
	   show_ip_bgp_comlist_eol, OBJ(int, 2), TRUE, "exact-match",
	   "Exact match of the communities", PRIV_CONF);
NUMBER	(show_ip_bgp_comlist_num, show_ip_bgp_comlist_exact, no_alt,
	 OBJ(int,1), 1, MAX_EXT_COMLIST, "community-list number");
KEYWORD_MM(show_ip_bgp_comlist, show_ip_bgp_comlist_num, show_ip_bgp_damp, 
	 "community-list", 
	 "Display routes matching the community-list", PRIV_USER, 10);

pdecl	(show_ip_bgp_com_loop);
EOLS	(show_ip_bgp_com_eol, bgp_show, BGP_COMMUNITY);
KEYWORD_ID(show_ip_bgp_com_exact, show_ip_bgp_com_eol, 
	   show_ip_bgp_com_eol, OBJ(int, 14), TRUE, "exact-match",
	   "Exact match of the communities", PRIV_CONF);

KEYWORD_ID(show_ip_bgp_com_localas, show_ip_bgp_com_loop, 
	   show_ip_bgp_com_exact, OBJ(int, 13), COMMUNITY_LOCALAS, 
	   "local-AS",
	   "Do not send outside local AS (well-known community)", PRIV_USER);
KEYWORD_ID(show_ip_bgp_com_noadv, show_ip_bgp_com_loop, 
	   show_ip_bgp_com_localas, OBJ(int, 12), COMMUNITY_NOADV, 
	   "no-advertise",
	   "Do not advertise to any peer (well-known community)", PRIV_USER);
KEYWORD_ID(show_ip_bgp_com_noexp, show_ip_bgp_com_loop, 
	   show_ip_bgp_com_noadv, OBJ(int, 11), COMMUNITY_NOEXPORT, 
	   "no-export",
	   "Do not export to next AS (well-known community)", PRIV_USER);
NOP	(show_ip_bgp_com_loop, show_ip_bgp_com_noexp, NONE);

NUMBER	(show_ip_bgp_com_num5, show_ip_bgp_com_loop, show_ip_bgp_com_loop,
	 OBJ(int,5), 1, COMMUNITY_MAX, "community number");
NUMBER	(show_ip_bgp_com_num4, show_ip_bgp_com_num5, show_ip_bgp_com_loop,
	 OBJ(int,4), 1, COMMUNITY_MAX, "community number");
NUMBER	(show_ip_bgp_com_num3, show_ip_bgp_com_num4, show_ip_bgp_com_loop,
	 OBJ(int,3), 1, COMMUNITY_MAX, "community number");
NUMBER	(show_ip_bgp_com_num2, show_ip_bgp_com_num3, show_ip_bgp_com_loop,
	 OBJ(int,2), 1, COMMUNITY_MAX, "community number");
NUMBER	(show_ip_bgp_com_num1, show_ip_bgp_com_num2, show_ip_bgp_com_loop,
	 OBJ(int,1), 1, COMMUNITY_MAX, "community number");

BGP_COMMUNITY (show_ip_bgp_com5, show_ip_bgp_com_loop, show_ip_bgp_com_loop,
	       OBJ(int,9), OBJ(int,10), "community number");
BGP_COMMUNITY (show_ip_bgp_com4, show_ip_bgp_com5, show_ip_bgp_com_loop,
	       OBJ(int,7), OBJ(int,8), "community number");
BGP_COMMUNITY (show_ip_bgp_com3, show_ip_bgp_com4, show_ip_bgp_com_loop,
	       OBJ(int,5), OBJ(int,6), "community number");
BGP_COMMUNITY (show_ip_bgp_com2, show_ip_bgp_com3, show_ip_bgp_com_loop,
	       OBJ(int,3), OBJ(int,4), "community number");
BGP_COMMUNITY (show_ip_bgp_com1, show_ip_bgp_com2, show_ip_bgp_com_loop,
	       OBJ(int,1), OBJ(int,2), "community number");

IFELSE	(show_ip_bgp_com_ifelse, show_ip_bgp_com1, show_ip_bgp_com_num1,
	 new_community_format);
KEYWORD (show_ip_bgp_com, show_ip_bgp_com_ifelse, show_ip_bgp_comlist, 
	 "community", "Display routes matching the communities", PRIV_USER);


EOLS	(show_ip_bgp_inconsistent_eol, bgp_show, BGP_BOGUS);
KEYWORD (show_ip_bgp_inconsistent, show_ip_bgp_inconsistent_eol,
	 show_ip_bgp_com, "inconsistent-as",
	 "Display only routes with inconsistent origin ASs", PRIV_USER);

EOLS	(show_ip_bgp_cidr_eol, bgp_show, BGP_CIDR);
KEYWORD (show_ip_bgp_cidr, show_ip_bgp_cidr_eol, show_ip_bgp_inconsistent,
	 "cidr-only", "Display only routes with non-natural netmasks",
	 PRIV_USER);

EOLS	(show_ip_bgp_fstat, bgp_show, BGP_FSTAT);
TESTVAR	(show_ip_bgp_fstat_check, show_ip_bgp_fstat, show_ip_bgp_cidr,
	 NONE, NONE, NONE, OBJ(int,2), TRUE);

EOLS	(show_ip_bgp_regexp_eol, bgp_show, BGP_REGEXP);
TEXT	(show_ip_bgp_regexp, show_ip_bgp_regexp_eol, no_alt,
	 OBJ(string,1), "A regular-expression to match the BGP AS paths");
KEYWORD (show_ip_bgp_apply_regexp, show_ip_bgp_regexp, 
	 show_ip_bgp_fstat_check,
	 "regexp", "Display routes matching the AS path regular expression",
	 PRIV_USER);

EOLS	(show_ip_bgp_filter_list_num_eol, bgp_show, BGP_LIST);	
NUMBER	(show_ip_bgp_filter_list_num, show_ip_bgp_filter_list_num_eol, no_alt,
	 OBJ(int,1), MINREGEXPACC+1, MAXREGEXPACC, 
	 "Regular expression access list number");
KEYWORD (show_ip_bgp_apply_filter_list, show_ip_bgp_filter_list_num, 
	 show_ip_bgp_apply_regexp,
	 "filter-list", "Display routes conforming to the filter-list",
	 PRIV_USER);

EOLS	(show_ip_bgp_addr_eol, bgp_show, BGP_ADDRESS);

EOLS    (show_ip_bgp_subnets_eol, bgp_show, BGP_ADDRESS_SUBNETS);

/* The subnets keyword is made hidden, the longer-prefixes keyword is used
 * so that it is consistent with the longer-prefixes keyword in 
 * show ip route command. Don't bother to change the subnets related 
 * identifiers here.
 */ 
KEYWORD (show_ip_bgp_subnets, show_ip_bgp_subnets_eol, show_ip_bgp_addr_eol,
	 "subnets", "Display route and more specific routes", 
	 PRIV_USER|PRIV_HIDDEN);

KEYWORD (show_ip_bgp_prefix, show_ip_bgp_subnets_eol, show_ip_bgp_subnets,
	 "longer-prefixes", "Display route and more specific routes", 
	 PRIV_USER);
IPADDR	(show_ip_bgp_netmask, show_ip_bgp_prefix, show_ip_bgp_addr_eol,
	 OBJ(paddr,2), "Network mask");

IPADDR	(show_ip_bgp_network, show_ip_bgp_netmask, 
	 show_ip_bgp_apply_filter_list, OBJ(paddr,1), 
	 "Network in the BGP routing table to display");

KEYWORD_ID(show_ip_bgp_fstat_kw, show_ip_bgp_network, show_ip_bgp_network,
	   OBJ(int, 2), TRUE, 
	   "flap-statistics", "Display flap statistics of routes", PRIV_USER);

EOLNS	(show_ip_bgp_pgrp_eol, bgp_show_pgrp);

EOLNS	(show_ip_bgp_pgrp_summary_eol, bgp_show_pgrp_summary);
KEYWORD	(show_ip_bgp_pgrp_summary, show_ip_bgp_pgrp_summary_eol,
	 show_ip_bgp_pgrp_eol,
	 "summary", "Summary of peer-group member status", PRIV_USER);

SET	(show_ip_bgp_pgrp_tag_set, show_ip_bgp_pgrp_summary, 
	 OBJ(int, 1), TRUE);
STRING	(show_ip_bgp_pgrp_tag, show_ip_bgp_pgrp_tag_set, show_ip_bgp_pgrp_eol, 
	 OBJ(string, 1), "peer-group name");
KEYWORD	(show_ip_bgp_pgrp, show_ip_bgp_pgrp_tag, show_ip_bgp_fstat_kw,
	 "peer-group", "Display information on peer-groups", PRIV_USER);


EOLNS	(show_ip_bgp_neighbors_eol, bgp_show_neighbors);

EOLNS	(show_ip_bgp_neighborpaths_eol, bgp_show_neighbor_paths);
TEXT	(show_ip_bgp_neighborpaths_regexp, show_ip_bgp_neighborpaths_eol,
	 show_ip_bgp_neighborpaths_eol, OBJ(string, 1),
	 "A regular-expression to match the BGP AS paths");
KEYWORD (show_ip_bgp_neighbors_paths, show_ip_bgp_neighborpaths_regexp,
	 show_ip_bgp_neighbors_eol, "paths",
	 "Display AS paths learned from neighbor", PRIV_USER);

EOLS	(show_ip_bgp_neighbors_damproute_eol, bgp_show_neighbor_routes, 
	 BGP_NEIGH_DAMPROUTES);
KEYWORD (show_ip_bgp_neighbors_damproute, show_ip_bgp_neighbors_damproute_eol,
	 show_ip_bgp_neighbors_paths, 
	 "dampened-routes",
	 "Display the dampened routes received from neighbor",PRIV_USER);

EOLS	(show_ip_bgp_neighbors_fstat_eol, bgp_show_neighbor_routes, 
	 BGP_NEIGH_FSTAT);
KEYWORD	(show_ip_bgp_neighbors_fstat, show_ip_bgp_neighbors_fstat_eol,
	 show_ip_bgp_neighbors_damproute,
	 "flap-statistics", "Display flap statistics of the routes learned "
	 "from neighbor", PRIV_USER);

EOLS	(show_ip_bgp_neighbor_advroutes_eol, bgp_show_neighbor_routes,
	 BGP_NEIGH_ADVROUTES);
KEYWORD	(show_ip_bgp_neighbors_advroutes, show_ip_bgp_neighbor_advroutes_eol,
	 show_ip_bgp_neighbors_fstat, 
	 "advertised-routes", 
	 "Display the routes advertised to an EBGP neighbor", PRIV_USER);

EOLS	(show_ip_bgp_neighbor_rcvdroutes_eol, bgp_show_neighbor_routes,
	 BGP_NEIGH_RCVDROUTES);
KEYWORD	(show_ip_bgp_neighbors_rcvdroutes, show_ip_bgp_neighbor_rcvdroutes_eol,
	 show_ip_bgp_neighbors_advroutes, 
	 "received-routes", 
	 "Display the received routes from neighbor", PRIV_USER);


EOLS	(show_ip_bgp_neighborroutes_eol, bgp_show_neighbor_routes, 
	 BGP_NEIGH_ROUTES);
KEYWORD (show_ip_bgp_neighbors_routes, show_ip_bgp_neighborroutes_eol,
	 show_ip_bgp_neighbors_rcvdroutes, "routes",
	 "Display routes learned from neighbor", PRIV_USER);

IPADDR 	(show_ip_bgp_neighbors_network, show_ip_bgp_neighbors_routes,
	 show_ip_bgp_neighbors_eol, OBJ(paddr,1),
	 "Neighbor to display information about"); 

KEYWORD (show_ip_bgp_neighbors, show_ip_bgp_neighbors_network,
	 show_ip_bgp_pgrp, "neighbors",
	 "Detailed information on TCP and BGP neighbor connections",
	 PRIV_USER);

EOLNS	(show_ip_bgp_paths_eol, bgp_show_paths);
TEXT	(show_ip_bgp_paths_regexp, show_ip_bgp_paths_eol, 
	 show_ip_bgp_paths_eol, OBJ(string, 1),
	 "A regular-expression to match the BGP AS paths");
KEYWORD (show_ip_bgp_paths, show_ip_bgp_paths_regexp, show_ip_bgp_neighbors,
	 "paths", "Path information", PRIV_USER);

EOLNS	(show_ip_bgp_summary_eol, bgp_show_summary);
KEYWORD (show_ip_bgp_summary, show_ip_bgp_summary_eol, show_ip_bgp_paths,
	 "summary", "Summary of BGP neighbor status", PRIV_USER);

KEYWORD (show_ip_bgp, show_ip_bgp_summary, ALTERNATE,
	 "bgp", "BGP information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_bgp
