/* $Id: cfg_routemap_set_ip.h,v 3.3 1995/11/28 19:50:37 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_routemap_set_ip.h,v $
 *---------------------------------------------------------------
 * cfg_routemap_set_ip.h -- IP specific route-map clauses
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set_ip.h,v $
 * Revision 3.3  1995/11/28  19:50:37  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:06:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/21  18:16:01  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:46  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * set ip destination <ip-address> <mask>
 * set ip next-hop <ip-address> [... <ip-address>]
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

LINK_EXIT(routemap_set_ip_return, no_alt);

LINK_TRANS(routemap_set_ip_extend_here, no_alt);

/*
 * set ip next-hop
 */
pdecl(routemap_set_ip_nexthop_ip);

FUNC	(routemap_set_ip_nexthop_func,
	 routemap_set_ip_nexthop_ip,
	 route_map_set_next_hop_command);
IPADDR	(routemap_set_ip_nexthop_ip, routemap_set_ip_nexthop_func,
	 routemap_set_ip_return,
	 OBJ(paddr,1), "IP address of next hop");
KEYWORD	(routemap_set_ip_nexthop, routemap_set_ip_nexthop_ip,
	 routemap_set_ip_extend_here,
	 "next-hop", "Next hop address", PRIV_CONF);

/*
 * set ip destination
 */
FUNC	(routemap_set_ip_dest_func,
	 routemap_set_ip_return,
	 route_map_set_dest_command);
IPADDR_MASK (routemap_set_ip_getip, routemap_set_ip_dest_func, no_alt,
	     OBJ(paddr,1), OBJ(paddr,2), "IP prefix of summary");
NOPREFIX(routemap_set_ip_dest_no, routemap_set_ip_getip,
	 routemap_set_ip_dest_func);
KEYWORD	(routemap_set_ip_dest, routemap_set_ip_dest_no,
	 routemap_set_ip_nexthop,
	 "destination", "Summary address to advertise", PRIV_CONF|PRIV_HIDDEN);

/*
 * ip
 */
KEYWORD_ID (routemap_set_ip, routemap_set_ip_dest, ALTERNATE,
	    OBJ(int,1), 0,
	    "ip", "IP specific information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set_ip
