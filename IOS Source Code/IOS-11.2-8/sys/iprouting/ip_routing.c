/* $Id: ip_routing.c,v 3.2 1995/11/17 17:34:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ip_routing.c,v $
 *------------------------------------------------------------------
 * ip_routing.c -- Generic IP routing functions
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_routing.c,v $
 * Revision 3.2  1995/11/17  17:34:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "interface_private.h"
#include "route_map.h"


/*
 * ip_routing_route_map_set_nvgen
 * NVgen IP routing route map clauses.
 */

void ip_routing_route_map_set_nvgen (parseinfo *csb,
				     route_maptype *map)
{
    route_map_addrtype *aptr;
    boolean first;

    if ((map->set_list.set_flags & ROUTE_MAP_DEST_SET) &&
	(map->set_list.destination.type == ADDR_IP)) {
	nv_write(TRUE, "%s ip destination %i %i", csb->nv_command,
		 map->set_list.destination.ip_addr,
		 map->set_list.mask);
    }

    if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	first = TRUE;
	for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
	    if (aptr->address.type != ADDR_IP)
		continue;
	    if (first) {
		first = FALSE;
		nv_write(TRUE, "%s ip next-hop", csb->nv_command);
	    }
	    nv_add(TRUE, " %i", aptr->address.ip_addr);
	}
    }
}

/*
 * ip_routing_route_map_match_nvgen
 * NVgen IP routing route map clauses.
 */

void ip_routing_route_map_match_nvgen (parseinfo *csb,
				       route_maptype *map)
{
}

/*
 * ip_routing_show_route_map_set
 * Show IP routing route map set clauses.
 */

void ip_routing_show_route_map_set (route_maptype *map)
{
    boolean first;
    route_map_addrtype *aptr;

    if ((map->set_list.set_flags & ROUTE_MAP_DEST_SET) &&
	(map->set_list.destination.type == ADDR_IP)) {
	printf("\n    ip destination %i%m",
	       map->set_list.destination.ip_addr,
	       map->set_list.mask);
    }
    
    if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	first = TRUE;
	for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
	    if (aptr->address.type != ADDR_IP)
		continue;
	    if (first) {
		first = FALSE;
		printf("\n    ip next-hop");
	    }
	    printf(" %i", aptr->address.ip_addr);
	}
    }
}

/*
 * ip_routing_show_route_map_match
 * Show IP routing route map match clauses.
 */

void ip_routing_show_route_map_match (route_maptype *map)
{
}
