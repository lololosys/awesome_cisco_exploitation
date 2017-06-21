/* $Id: cfg_routemap_set_bgp.h,v 3.5.46.3 1996/07/29 21:33:11 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_set_bgp.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_set_bgp.h -- BGP specific route map set commands
 *
 * Mar. 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set_bgp.h,v $
 * Revision 3.5.46.3  1996/07/29  21:33:11  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.5.46.2  1996/05/22  17:16:12  rchandra
 * CSCdi55633:  BGP: set dampening route-map command not accepted
 * Branch: California_branch
 *
 * Revision 3.5.46.1  1996/04/16  19:01:21  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.5  1995/12/11  07:43:06  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.4  1995/11/28  19:50:36  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.3  1995/11/25  12:44:57  rchandra
 * CSCdi43016:  BGP: need knob to selectively drop community attribute
 * - add 'set community none' route-map subcommand
 *
 * Revision 3.2  1995/11/17  17:31:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:16:00  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:45  widmer
 * Cscdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * set as-path prepend <as> ... <as>
 * set as-path tag
 * set community <value> | additive
 * set local-preference <value>
 * set origin {igp | egp <as> | incomplete }
 * set weight <value>
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(int,2) = metric, IGRP bandwidth, local-pref, or weight, or origin.
 * OBJ(int,3) = IGRP delay metric, or origin_as.
 * OBJ(int,4) = IGRP reliability metric.
 * OBJ(int,5) = IGRP loading metric.
 * OBJ(int,6) = IGRP mtu metric.
 * OBJ(int,2) = 0 for type-1, 1 for type-2, 2 for internal, 3 for external.
 * OBJ(int,2) = tag
 * OBJ(int,2) = 0 for level-1, 1 for level-2, 2 for level-1-2, 3 for stub-area,
 *              4 for backbone.
 * OBJ(paddr,1) = IP or OSI address.
 * OBJ(paddr,2) = If IP address in OBJ(paddr,1) than mask here.
 * OBJ(int, 1)  = community number for set community
 * OBJ(int, 2)  = TRUE if it is set community additive.
 */

pdecl(routemap_set_community_number);
pdecl(routemap_set_community_func);
pdecl(routemap_set_community_add);

LINK_EXIT(routemap_set_bgp_exit, no_alt);

/*
 * set as-path [prepend | tag]
 */
/*
 * set as-path prepend
 */
pdecl(routemap_set_as_path_pnd_number);

FUNC	(routemap_set_as_path_tag_func,
	 routemap_set_bgp_exit,
	 route_map_set_as_path_tag_command);
KEYWORD	(routemap_set_as_path_tag, routemap_set_as_path_tag_func, no_alt,
	 "tag",	"Set the tag as an AS-path attribute", PRIV_CONF);


FUNC	(routemap_set_as_path_pnd_func,
	 routemap_set_as_path_pnd_number,
	 route_map_set_as_path_prepend_command);
NUMBER	(routemap_set_as_path_pnd_number, routemap_set_as_path_pnd_func,
	 routemap_set_bgp_exit,
	 OBJ(int,1), 1, MAXUSHORT, "AS number");
FUNC	(routemap_set_as_path_pnd_no_func,
	 routemap_set_bgp_exit,
	 route_map_set_as_path_prepend_command);
FUNC    (routemap_set_as_path_fresh_cmd,
         routemap_set_as_path_pnd_number,
         route_map_set_as_path_prepend_fresh_command);
NOPREFIX(routemap_set_as_path_pnd_noprefix, routemap_set_as_path_fresh_cmd,
	 routemap_set_as_path_pnd_no_func);
KEYWORD	(routemap_set_as_path_pnd, routemap_set_as_path_pnd_noprefix,
	 routemap_set_as_path_tag,
	 "prepend", "Prepend to the as-path", PRIV_CONF);

KEYWORD	(routemap_set_as_path, routemap_set_as_path_pnd, ALTERNATE,
	 "as-path", "Prepend string for a BGP AS-path attribute",
	 PRIV_CONF);

/*
 * set dampening
 */
FUNC    (routemap_set_damp_func, routemap_set_bgp_exit, 
	 route_map_set_dampening_command);
NUMBER	(routemap_set_damp_msuppress, routemap_set_damp_func, no_alt, 
	 OBJ(int,5), 1, 0xFF, "Maximum duration to suppress a stable route"); 
NUMBER	(routemap_set_damp_suppress, routemap_set_damp_msuppress, no_alt, 
	 OBJ(int,4), 1,BGP_MAX_PENALTY,"penalty to start suppressing a route");
NUMBER	(routemap_set_damp_reuse, routemap_set_damp_suppress, no_alt, 
	 OBJ(int,3), 1, BGP_MAX_PENALTY,"penalty to start reusing a route");
NUMBER	(routemap_set_damp_halflife, routemap_set_damp_reuse, no_alt, 
	 OBJ(int,2), 1, BGP_MAX_HALFLIFE, "half-life time for the penalty");
NOPREFIX(routemap_set_damp_noprefix, routemap_set_damp_halflife,
	 routemap_set_damp_func);
KEYWORD	(routemap_set_damp, routemap_set_damp_noprefix, routemap_set_as_path,
	 "dampening", "Set BGP route flap dampening parameters", PRIV_CONF);

/*
 * set community
 */
FUNC	(routemap_set_community_add_func,
	 routemap_set_bgp_exit,
	 route_map_set_community_command);
KEYWORD_ID(routemap_set_community_add, routemap_set_community_add_func,
	   routemap_set_bgp_exit,
	   OBJ(int, 6), TRUE,
	   "additive", "Add to the existing community", PRIV_CONF);
KEYWORD_ID(routemap_set_community_localas, routemap_set_community_func,
 	   routemap_set_community_add, OBJ(int, 3), COMMUNITY_LOCALAS,
 	   "local-AS", 
 	   "Do not send outside local AS (well-known community)", PRIV_CONF);
KEYWORD_ID(routemap_set_community_noadv, routemap_set_community_func,
	   routemap_set_community_localas, OBJ(int, 3), COMMUNITY_NOADV,
	   "no-advertise",
	   "Do not advertise to any peer (well-known community)", PRIV_CONF);
KEYWORD_ID(routemap_set_community_noexp, routemap_set_community_func,
	   routemap_set_community_noadv, OBJ(int, 3), COMMUNITY_NOEXPORT,
	   "no-export",
	   "Do not export to next AS (well-known community)", PRIV_CONF);
FUNC	(routemap_set_community_func,
	 routemap_set_community_number,
	 route_map_set_community_command);

SET	(routemap_set_community_null, routemap_set_community_func,
	 OBJ(int,3), 0);
BGP_COMMUNITY(routemap_set_community_community, routemap_set_community_null,
	      routemap_set_community_noexp, OBJ(int, 4), OBJ(int, 5),
	      "community number in aa:nn format");

NUMBER	(routemap_set_community_number, routemap_set_community_func,
	 routemap_set_community_community,
	 OBJ(int,3), 1, COMMUNITY_MAX, "community number");

KEYWORD_ID(routemap_set_community_none, routemap_set_community_add_func,
	   routemap_set_community_number,
	   OBJ(int, 2), 1,
	   "none", "No community attribute", PRIV_CONF);
NOPREFIX(routemap_set_community_noprefix, routemap_set_community_none,
	 routemap_set_community_add_func);
KEYWORD	(routemap_set_community, routemap_set_community_noprefix,
	 routemap_set_damp,
	 "community", "BGP community attribute", PRIV_CONF);

/*
 * set origin
 */
FUNC	(routemap_set_origin_func,
	 routemap_set_bgp_exit,
	 route_map_set_origin_command);

KEYWORD_ID(routemap_set_origin_incomplete, routemap_set_origin_func, no_alt,
	   OBJ(int,2), ORIGIN_INCOMPLETE,
	   "incomplete", "unknown heritage", PRIV_CONF);

NUMBER	(routemap_set_origin_egp_as, routemap_set_origin_func, no_alt,
	 OBJ(int,3), 0, MAXUSHORT, "remote AS");
KEYWORD_ID(routemap_set_origin_egp, routemap_set_origin_egp_as,
	   routemap_set_origin_incomplete,
	   OBJ(int,2), ORIGIN_EGP,
	   "egp", "remote EGP", PRIV_CONF);

KEYWORD_ID(routemap_set_origin_igp, routemap_set_origin_func,
	   routemap_set_origin_egp,
	   OBJ(int,2), ORIGIN_IGP,
	   "igp", "local IGP", PRIV_CONF);

NOPREFIX(routemap_set_origin_no, routemap_set_origin_igp,
	 routemap_set_origin_func);
KEYWORD	(routemap_set_origin, routemap_set_origin_no, routemap_set_community,
	 "origin", "BGP origin code", PRIV_CONF);

/*
 * set weight
 */
FUNC	(routemap_set_weight_func,
	 routemap_set_bgp_exit,
	 route_map_set_weight_command);
NUMBER	(routemap_set_weight_number, routemap_set_weight_func,
	 routemap_set_bgp_exit,
	 OBJ(int,2), 0, 0xffffffff, "Weight value");
NOPREFIX(routemap_set_weight_no, routemap_set_weight_number,
	 routemap_set_weight_func);
KEYWORD	(routemap_set_weight, routemap_set_weight_no, routemap_set_origin,
	 "weight", "BGP weight for routing table", PRIV_CONF);
/*
 * set destination-preference 
 */
FUNC	 (routemap_set_destpref_func, 
	  routemap_set_bgp_exit,
	  route_map_set_destpref_command);
NUMBER (routemap_set_destpref_number, routemap_set_destpref_func, 
	routemap_set_bgp_exit, 
	OBJ(int,2), 0, 0xffffffff, "Preference value");
NOPREFIX (routemap_set_destpref_no, routemap_set_destpref_number, 
	  routemap_set_destpref_func);
KEYWORD	(routemap_set_destpref, routemap_set_destpref_no, 
	 routemap_set_weight, "destination-preference", 
	 "BGP destination preference path attribute", PRIV_CONF);

/*
 * set local-preference
 */
FUNC	(routemap_set_localpref_func,
	 routemap_set_bgp_exit,
	 route_map_set_localpref_command);
NUMBER	(routemap_set_localpref_number, routemap_set_localpref_func,
	 routemap_set_bgp_exit,
	 OBJ(int,2), 0, 0xffffffff, "Preference value");
NOPREFIX(routemap_set_localpref_no, routemap_set_localpref_number,
	 routemap_set_localpref_func);
KEYWORD	(routemap_set_localpref, routemap_set_localpref_no,
	 routemap_set_destpref,
	 "local-preference", "BGP local preference path attribute", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set_localpref
