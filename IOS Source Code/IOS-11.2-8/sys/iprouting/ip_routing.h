/* $Id: ip_routing.h,v 3.2 1995/11/17 17:34:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ip_routing.h,v $
 *------------------------------------------------------------------
 * ip_routing.h -- Generic IP routing declarations
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_routing.h,v $
 * Revision 3.2  1995/11/17  17:34:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ip_routing.c
 */ 
void ip_routing_route_map_set_nvgen(parseinfo *csb, route_maptype *map);
void ip_routing_route_map_match_nvgen(parseinfo *csb, route_maptype *map);
void ip_routing_show_route_map_set(route_maptype *map);
void ip_routing_show_route_map_match(route_maptype *map);
