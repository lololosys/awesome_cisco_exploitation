/* $Id: cfg_routemap_set_clns.h,v 3.3 1995/11/28 19:50:23 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/cfg_routemap_set_clns.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_set_clns.h -- CLNS specific route-map clauses
 *
 * March 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set_clns.h,v $
 * Revision 3.3  1995/11/28  19:50:23  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  08:51:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:15:55  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:08  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:16:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * set clns destination <clns-prefix>
 * set clns next-hop <clns-prefix> [... <clns-prefix>]
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

LINK_EXIT(routemap_clnp_exit, no_alt);

/*
 * set next-hop
 */
pdecl(routemap_set_clnp_nexthop_clns);

FUNC	(routemap_set_clnp_nexthop_func,
	 routemap_set_clnp_nexthop_clns,
	 route_map_set_next_hop_command);
NSAPADDR(routemap_set_clnp_nexthop_clns, routemap_set_clnp_nexthop_func,
	 routemap_clnp_exit,
	 OBJ(paddr,1), "CLNS summary prefix");
KEYWORD	(routemap_set_clnp_nexthop, routemap_set_clnp_nexthop_clns, no_alt,
	 "next-hop", "Next hop address", PRIV_CONF);

/*
 * set destination
 */
FUNC 	(routemap_set_clnp_dest_func,
	 routemap_clnp_exit,
	 route_map_set_dest_command);
NSAPADDR(routemap_set_clnp_getclns, routemap_set_clnp_dest_func, no_alt,
	 OBJ(paddr,1), "CLNS summary prefix");
NOPREFIX(routemap_set_clnp_dest_no, routemap_set_clnp_getclns,
	 routemap_set_clnp_dest_func);
KEYWORD	(routemap_set_clnp_dest, routemap_set_clnp_dest_no,
	 routemap_set_clnp_nexthop,
	 "destination", "Summary address to advertise", PRIV_CONF|PRIV_HIDDEN);

/*
 * clns keyword.
 */
KEYWORD_ID (routemap_set_clnp, routemap_set_clnp_dest, ALTERNATE,
	    OBJ(int,1), 1,
	    "clns", "OSI summary address", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set_clnp
