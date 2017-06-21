/* $Id: decnet_route.c,v 3.5.18.5 1996/07/22 23:07:04 asastry Exp $ 
 * $Source: /release/112/cvs/Xsys/decnet/decnet_route.c,v $
 *------------------------------------------------------------------
 * D E C N E T _ R O U T E . C
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *
 * Support for 'dec route' and 'dec host' cmds
 *
 *------------------------------------------------------------------
 * $Log: decnet_route.c,v $
 * Revision 3.5.18.5  1996/07/22  23:07:04  asastry
 * CSCdi62165:  DECnet fast-switching should be able to use DECnet default
 * route
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/06/08  23:24:04  cheilek
 * CSCdi56786:  Interface Description Truncated
 * Branch: California_branch
 * Increase namestring display field width by 8.
 *
 * Revision 3.5.18.3  1996/05/22  20:54:34  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.5.18.2  1996/04/25  23:03:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.18.1.2.1  1996/03/24  03:46:53  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.5.18.1  1996/03/18  19:11:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  01:19:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  08:42:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:43:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:10:45  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:00:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/22  23:20:16  asastry
 * CSCdi46327:  'show decnet static' shows bad SNPA for non-ethernet
 * static routes
 *
 * Revision 3.2  1995/11/17  08:59:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/07  22:46:39  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.2  1995/06/18 19:40:22  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:23:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "decnet_externs.h"
#include "decnet_debug_flags.h"
#include "name.h"
#include "sys_registry.h"
#include "parser_defs_dec.h"
#include "parser.h"

/*
 * decnet_host_command
 *
 * Handle the 'dec host <string> <decnet-address> command
 */
void decnet_host_command(parseinfo *csb)
{
    nametype *ptr;
    int size;

    /*
     * Check for valid hostname
     */
    if (name_verify(GETOBJ(string,1)) != OK) {
	printf("%%DECnet: Bad hostname format");
	return;
    }
    uppercase(GETOBJ(string,1));   /* convert name to upper case */
    ptr = (nametype *) name_cache_lookup(GETOBJ(string,1), NULL, NAM_DECNET);
    if (ptr || !csb->sense) {
	name_delete(GETOBJ(string,1), NAM_DECNET);
	if (!csb->sense) {
	    return;
	}
    }
    size = sizeof(nametype) + strlen(GETOBJ(string,1));
    ptr = malloc(size);
    if (!ptr) {
	return;
    }
    strcpy(ptr->name, GETOBJ(string,1));
    GET_TIMESTAMP(ptr->lasttime);
    ptr->flags = NAM_PERMANENT + NAM_DECNET + NAM_ADDRVALID;
    bcopy(GETOBJ(paddr,1), &ptr->v.decnet.adr, sizeof(addrtype));

    enqueue(&nameQ, ptr);

}


/*
 * decnetname_show_hosts
 *
 * Display DECnet host information
 * Called by SERVICE_NAME_SHOW_HOSTS with NAM_DECNET index
 */

void decnet_name_show_hosts (nametype *ptr, char *continue_text)
{
    int node, area;

    printf(" DECnet ");

    /* 
     * Print out the DECnet style address.
     */
    area = (ptr->v.decnet.adr.addr.decnet_address) >> 10;
    node = (ptr->v.decnet.adr.addr.decnet_address) & 0x3ff;

    printf("  %d.%d", area, node);
}

/*
 * decnet_name_match_number
 *
 * Check if address matches one of addresses in this name cache entry
 *
 * Returns TRUE if match succeeds
 * Called by SERVICE_NAME_MATCH_NUMBER, index is ADDR_DECNET
 */

static boolean decnet_name_match_number (nametype *ptr, addrtype *address)
{
    if (address->length == ADDRLEN_DECNET) {
	if (ptr->v.decnet.adr.addr.decnet_address ==
	                                          address->addr.decnet_address)
	    return (TRUE);
    }
    return (FALSE);
}

/*
 * decnet_address_lookup
 *
 * Look up given DECnet address in name cache. Return pointer to string
 * if found, else return NULL.
 */

char *decnet_address_lookup (int area, int node)
{
    addrtype  addr;
    nametype  *name;
    ushort    dn_addr;

    addr.type = ADDR_DECNET;
    addr.length = ADDRLEN_DECNET;
    dn_addr = (area << 10) + node;
    addr.decnet_addr = dn_addr;

    name = name_cache_lookup(NULL, &addr, NAM_DECNET);
    return(name ? name->name : NULL);

}

/*
 * decnet_static_route_command
 *
 * Handle the 'dec route' command
 */
void decnet_static_route_command (parseinfo *csb)
{
    idbtype      *idb;
    hwaddrtype   snpa;
    ushort       hops, cost;        
    ushort       dn_addr, next_hop_dn_addr;
    dnnet        *net;
    boolean      is_int_static_route;

    idb = GETOBJ(idb,1);

    bcopy((char *)&(GETOBJ(paddr,1)->decnet_addr), (char *)&dn_addr,
	  ADDRLEN_DECNET);
    hops = GETOBJ(int,3);
    cost = GETOBJ(int,4);
    net = (idb ? dn_nets[idb->dn_network] : dn_nets[0]);
    is_int_static_route = FALSE;

    switch (GETOBJ(int,2)) {

	case DC_ROUTE_DISCARD:
	    break;

	case DC_ROUTE_NAME:
	    /*
	     * decnet route a.b x.y [hops][cost]
	     * decnet route default x.y
	     *
	     * If a 'default' route was entered, dn_addr will be zero.
	     */
	    bcopy((char *)&(GETOBJ(paddr,2)->decnet_addr),
		  (char *)&next_hop_dn_addr, ADDRLEN_DECNET);
	    if (dn_addr == next_hop_dn_addr) {
		printf("\n%%Next hop address cannot be the same as route "
		       "destination");
		return;
	    }
	    
	    break;
       case DC_ROUTE_INTERFACE:
	    /*
	     * decnet route <a.b | default> <interface> <unit> <snpa>
	     */
	    bcopy((char *)&dn_addr, (char *)&next_hop_dn_addr, ADDRLEN_DECNET);
	    if (!is_p2p(GETOBJ(idb,1))) {
		bcopy((char *)GETOBJ(hwaddr,1), (char *)&snpa,
		       sizeof(hwaddrtype));
	    } else {
		snpa.length = 0;
	    }

	    is_int_static_route = TRUE;
	    break;
       default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
    /*
     * Add/delete static routes to DECnet static route queue.
     */
    decnet_staticQ_update(dn_addr, next_hop_dn_addr,
			  idb, &snpa, hops, cost, is_int_static_route, csb->sense);

    /*
     * Install/remove the static route. 
     */
    if (csb->sense)
        decnet_add_static_route(net, dn_addr, next_hop_dn_addr,
		 	        hops, cost, idb, &snpa, is_int_static_route);
    else
        decnet_remove_static_route (net, dn_addr);
}

/*
 * decnet_add_static_route
 *
 * Install a DECnet IV static route. 
 *
 */
void decnet_add_static_route (dnnet *net,
			      ushort dn_addr, ushort next_hop_dn_addr,
			      ushort hops, ushort cost,
			      idbtype *idb,
			      hwaddrtype *snpa,
			      boolean is_int_static_route) 
{
    dnroutetype **table, **routeptr, *thisroute, *route2;
    int area, node, next_hop_area, next_hop_node, index;

    /*
     * Value of 'dn_addr' is zero for a default route.
     */
    if (dn_addr == 0) {
       decnet_add_default_route(next_hop_dn_addr, hops, cost, idb, snpa,
				is_int_static_route);
       return;
    }

    area = dn_addr >> 10;
    node = dn_addr & 0x3ff;

    next_hop_area = next_hop_dn_addr >> 10;
    next_hop_node = next_hop_dn_addr & 0x3ff;

    if (net == NULL)
	return;

    if (!net->doareas) {
	/*
	 * If we are level 1
	 */
	if (node == 0) {
	    if (area != net->my_dn_area) {
	    /*
	     * Configuring a L2 static route does not make sense if we are
	     * running as a L1 router.
	     */

	    return;
	    }

	    /*
	     * Otherwise, this is a route to "x.0" where 'x' is our own area. This
	     * means we're configuring a route to the 'nearest L2 router'
	     */
	}
	else if (area != net->my_dn_area) {
	    /*
	     * Don't install L1 routes if we are L1 and the route is not in
	     * our area.
	     */
	    return;
	}
	else if (node == net->my_dn_node) {
	    /*
	     * Don't change our own address to point to somewhere else !!
	     */
	    return;
	}

        table = net->dnroutes;
	index = node;
    }
    else {
	/*
	 * else we are level 2
	 */
	if (area != net->my_dn_area) {
	    if (node != 0) {
	        /*
	         * Don't install L1 routes that are not in our area.
	         */
	        return;
	    }
	    table = net->dnareas;
	    index = area;
	}
	else {
	    /*
	     * We are a L2 router and installing a L1 route
	     */
	    if (node == 0) {
	        /*
	         * Don't install a route for our own area.
	         */
	        return;
	    }
	    else if (node == net->my_dn_node) {
	        /*
	         * Don't change our own address to point to somewhere else !!
	         */
	        return;
	    }

	    table = net->dnroutes;
	    index = node;
	}
    }

    /* 
     * Find the right entry in the routing table.
     */

    thisroute = table[index];
    routeptr = &table[index];

    /*
     * Do not overwrite 'dec advertise' route, if it exists.
     */
    if (thisroute && (thisroute->type == OSI_INJECTED)) {
	return;
    }

    /*
     * Do not overwrite a dynamically-learned adjacency with a static route.
     */
    route2 = (thisroute ? thisroute->nextadj : NULL);
    for ( ; route2; route2 = route2->nextadj) {
	if (route2->flags & DN_RT_HELLO) {
	    if ((net->doareas) && (node == 0)) {
	        dnet_robuginf("\nDNET-RT: Static route for area %d "
			      "not installed because an adjacency "
			      "entry was found", area);
            }
	    else {
	        dnet_robuginf("\nDNET-RT: Static route for %d.%d "
			      "not installed because an adjacency "
			      "entry was found", area, node);
	    }
	    return;
	}
    }

    /*
     * If the static route's cost is higher than that of the route that is
     * currently in the table, don't do anything.
     */
    if (thisroute && (thisroute->type == PHASEIV)) {
	if (cost > thisroute->cost)
	    return;

	dn_free_rt(&thisroute);
    }

    if (!thisroute) {
        thisroute = malloc(sizeof(dnroutetype));
        if (!thisroute) {
            dn_stat[DN_NOMEM]++;
	    return;
        }
        table[index] = thisroute;

	thisroute->interface = idb; 
	thisroute->hops = hops;
	thisroute->cost = cost;
	thisroute->prio = DN_DEF_PRIO;
	dn_addr(thisroute->gateway, next_hop_area, next_hop_node);
	bcopy(snpa, &thisroute->snpa, sizeof(hwaddrtype)); 	
	thisroute->type = PHASEIV_STATIC;
	thisroute->numroutes = 1;
	thisroute->nextsplit = NULL;
	thisroute->nextadj = NULL;    
        thisroute->areavector = NULL;
        thisroute->vector = NULL;
	thisroute->is_int_static_route = is_int_static_route;

	if ((net->doareas) && (node == 0)) {
	    dnet_robuginf("\nDNET-RT: Static route configured for area %d ",  area);
        }
	else {
	    dnet_robuginf("\nDNET-RT: Static route configured for %d.%d ",
			  area, node);
	}
    }
    else {
        /*
	 * This is a STATIC route..overwrite with new information.
	 */
        if (thisroute->hops != hops) {
	    thisroute->hops = hops;
	    dnet_robuginf("\nDNET-RT: Hop count for route %d.%d changed to %d", 
			  area, node, hops);
	}
	if (thisroute->cost != cost) {
	    thisroute->cost = cost;	
	    dnet_robuginf("\nDNET-RT: Cost for route %d.%d changed to %d", 
			  area, node, cost);
	}

	thisroute->interface = idb; 
	dn_addr(thisroute->gateway, next_hop_area, next_hop_node);
	bcopy(snpa, &thisroute->snpa, sizeof(hwaddrtype)); 	
    }

    if (!net->doareas) {
        TRIGGER_L1_ROUTES("static route added to routing table");
    }
    else {
	if (area != net->my_dn_area) {
           TRIGGER_L2_ROUTES("static route added to routing table");
       }
        else {
           TRIGGER_L1_ROUTES("static route added to routing table");
       }
    }		

    return;
}

/*
 * decnet_handle_static_route
 *
 * Handle the special case of a DECnet IV static route. This is called by
 * 'dn_forward'. Basically, we return a pointer to the correct route, so
 * that 'dn_forward' can process the packet going out on this route.
 *
 * If the route that we get upon look up is a static one with no outgoing
 * interface, recurse until we find the route with an outgoing interface.
 *
 */
dnroutetype *decnet_handle_static_route (dnnet *net, dnroutetype *static_route,
					 int depth)
{
    int next_hop_area, next_hop_node;
    dnroutetype *new_route;

    /*
     * Limit amount of routing table lookup recursion.
     */
    if (!depth)
	return(NULL);

    depth--;

    next_hop_area = dn_area(static_route->gateway);
    next_hop_node = dn_node(static_route->gateway);

    if (static_route->interface == NULL) {
	/*
	 * We have a static route of the form
	 *    decnet route a.b x.y
	 */
	if (next_hop_area != net->my_dn_area)  {  		
	    if (net->doareas) 
		new_route = net->dnareas[next_hop_area];
	    else 
		new_route = NULL;
	} else 
	    new_route = net->dnroutes[next_hop_node];
	
	/*
	 * If the new route is itself a static route that does not
	 * point to an outgoing interface, resolve it (recursively).
	 */
	if (new_route && (new_route->type == PHASEIV_STATIC) &&
	                 (new_route->interface == NULL))
	    new_route = decnet_handle_static_route(net, new_route, depth);
    }
    else {
	/*
	 * We have a static route of the form
	 *    decnet route a.b <interface> [snpa]
	 *
	 * In this case, the argument in 'static_route' will point to a route
	 * that has an snpa that we can use, so set new_route to static_route.
	 */
        new_route = static_route;
    }

    return (new_route);
}

/*
 * decnet_remove_static_route
 *
 * Remove a DECnet IV static route from the routing table.
 *
 */
void decnet_remove_static_route (dnnet *net, ushort dn_addr)
{
    dnroutetype **table, **routeptr, *thisroute;
    int area, node, index;

    area = dn_addr >> 10;
    node = dn_addr & 0x3ff;

    if (net == NULL)
	return;

    /*
     * Value of 'dn_addr' is zero for a default route.
     */
    if (dn_addr == 0) {
        decnet_remove_default_route();
        return;
    }

    if ((!net->doareas) || ((node != 0) && (area == net->my_dn_area))) {
        table = net->dnroutes;
	index = node;
    }
    else {
        table = net->dnareas;
	index = area;
    }

    /* 
     * Find the right entry in the routing table.
     */

    routeptr = &table[index];
    thisroute = table[index];

    if (!thisroute)
	return;

    /*
     * Do not touch non static routes !!
     */
    if (thisroute && (thisroute->type != PHASEIV_STATIC)) {
	return;
    }
    
    dnet_robuginf("\nDNET-RT: Static route for %d.%d removed", area, node);
    dn_unsplit(net, routeptr, thisroute);
    *routeptr = thisroute->nextadj;
    free(thisroute);

    if (routeptr)
        dn_recomp(routeptr, net);

    if ((!net->doareas) || ((node != 0) && (area == net->my_dn_area))) {
        TRIGGER_L1_ROUTES("static route deleted from routing table");
    }
    else {
        TRIGGER_L2_ROUTES("static route deleted from routing table");
    }		
}

/*
 * decnet_reinstate_static_routes
 *
 * Reinstate all static routes. Called when we start DECnet routing.
 *
 */
void decnet_reinstate_static_routes (dnnet *net)
{
    decnet_staticQtype *entry;

    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry;
	 entry = entry->next) {
	/*
	 * Default route has a 'dn_addr' value of 0.
	 */
	if (entry->dn_addr == 0) {
	    decnet_add_default_route(entry->next_hop_dn_addr, entry->hops,
				     entry->cost, entry->idb, &entry->snpa,
				     entry->is_int_static_route);
        }
	else {
	    decnet_add_static_route(net,
				     entry->dn_addr, entry->next_hop_dn_addr,
				     entry->hops, entry->cost, entry->idb,
				     &entry->snpa, entry->is_int_static_route);
	}
    }
}

/*
 * decnet_add_default_route
 *
 * Install a DECnet IV default route. Called from 'decnet_add_static_route'.
 *
 */
void decnet_add_default_route (ushort next_hop_dn_addr,
			       ushort hops, ushort cost,
			       idbtype *idb,
			       hwaddrtype *snpa,
			       boolean is_int_static_route) 
{
    int next_hop_area, next_hop_node;
    char buffer[3*ADDRLEN_MAX];

    next_hop_area = next_hop_dn_addr >> 10;
    next_hop_node = next_hop_dn_addr & 0x3ff;

    /*
     * Create entry for default route if none exists.
     */
    if (!decnet_default_route) {
        decnet_default_route = malloc(sizeof(dnroutetype));
        if (!decnet_default_route) {
            dn_stat[DN_NOMEM]++;
	    return;
        }
    }

    decnet_default_route->interface = idb; 
    decnet_default_route->hops = hops;
    decnet_default_route->cost = cost;
    decnet_default_route->prio = DN_DEF_PRIO;
    dn_addr(decnet_default_route->gateway, next_hop_area, next_hop_node);
    bcopy(snpa, &decnet_default_route->snpa, sizeof(hwaddrtype)); 	
    decnet_default_route->type = PHASEIV_STATIC;
    decnet_default_route->numroutes = 1;
    decnet_default_route->nextsplit = NULL;
    decnet_default_route->nextadj = NULL;    
    decnet_default_route->areavector = NULL;
    decnet_default_route->vector = NULL;
    decnet_default_route->is_int_static_route = is_int_static_route;

    if (decnet_rt_debug) {
        buginf("\nDNET-RT: Default route configured with ");
        if (!is_int_static_route)
            buginf("next hop address of %d.%d ", next_hop_area, next_hop_node);    
        else {
            if (idb)
                buginf("next hop interface of %s ", idb->namestring);
            if (snpa->length != 0)
                buginf("with snpa %s", print_snpa_string(buffer, snpa));
        }
    }
}

/*
 * decnet_remove_default_route
 *
 * Remove a DECnet IV default route. Called from 'decnet_remove_static_route',
 * and from 'decnet.c' when DECnet routing is turned off.
 *
 */
void decnet_remove_default_route (void)
{
    if (decnet_default_route) {
	free(decnet_default_route);
	decnet_default_route = NULL;
    }

    return;
}

/*
 * decnet_staticQ_update
 *
 * Add or delete from the DECnet static route queue.
 */
void decnet_staticQ_update (
			    ushort dn_addr,
			    ushort next_hop_dn_addr,
			    idbtype *idb,
			    hwaddrtype *snpa,
			    ushort hops,
			    ushort cost,
			    boolean is_int_static_route,
			    boolean sense)
{
    decnet_staticQtype *entry;

    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; 
	 entry = entry->next) {

	if (dn_addr != entry->dn_addr)
	    continue;

	/*
	 * Handle 'default' routes specially. Since we can have only one default
	 * route, we should quit as soon as we find an entry with address of
	 * zero, and overwrite this entry with the new default route info.
	 */
	if (dn_addr == 0)
	    break;

	if (!idb) {
	    if (next_hop_dn_addr != entry->next_hop_dn_addr)
		continue;
	} else {
	    if (idb != entry->idb) {
		continue;
	    }
	    if (snpa->length != entry->snpa.length) {
		continue;
	    }
	    if (bcmp(snpa->addr, entry->snpa.addr, snpa->length)) {
		continue;
	    }
	}
	break;
    }

    /*
     * Add new entry or update existing entry.
     */
    if (sense) {
	if (!entry) {
	    entry = malloc(sizeof(decnet_staticQtype));
	    if (!entry) {
		return;
	    }
	    enqueue(&decnet_staticQ, entry);
	}

	/*
	 * Fill in data. 'dn_addr' will be zero for default routes.
	 */
	entry->idb = idb;
	entry->dn_addr = dn_addr;
	entry->hops = hops;
	entry->cost = cost;
	entry->is_int_static_route = is_int_static_route;
	entry->next_hop_dn_addr = next_hop_dn_addr;

	/*
	 * If this was an interface static route and an SNPA was specified
	 * copy the info to the static Q entry.
	 */
	if (is_int_static_route && snpa->length) {
	    bcopy(snpa, &entry->snpa, sizeof(hwaddrtype));
	}

    } else {

	/*
	 * Delete existing entry. If entry does not exist, return quietly.
	 */
	if (!entry) {
	    return;
	}
	unqueue(&decnet_staticQ, entry);
	free(entry);
    }
}

/*
 * decnet_search_staticQ
 *
 * Search for an entry in the queue. If the flag 'lowest_cost' is TRUE search
 * entire queue to see if there's any other entry with lower cost. Otherwise
 * return the first entry that we find.
 *
 * Key is DECnet address.
 */
decnet_staticQtype *decnet_search_staticQ (ushort dn_addr, boolean lowest_cost)
{
    decnet_staticQtype *entry, *temp_entry;

    temp_entry = NULL;
    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; 
	 entry = entry->next) {

	if (dn_addr == entry->dn_addr) {
	    if (!lowest_cost) {
		temp_entry = entry;
	        break;
	    }
	    else {
	        if (!temp_entry)
		    temp_entry = entry;
		else {
		    if (entry->cost < temp_entry->cost)
			temp_entry = entry;
		}
	    }
	}
    }
    return (temp_entry);
}			   

/*
 * decnet_show_static
 *
 * Display all configured static routes (i.e. all the routes in the static
 * queue. If 'addr' contains a non-zero DECnet address, then display static
 * route info only for that address.
 */
void decnet_show_static (addrtype *addr)
{
    decnet_staticQtype *entry, *temp_entry;
    int node, area, show_node, show_area;
    char buffer[3*ADDRLEN_MAX];

    show_area = (addr->decnet_addr >> 10) & 0x3f;
    show_node = (addr->decnet_addr & 0x3ff);

    printf("\n");
    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; ) {
	mem_lock(entry);

	if (show_area || show_node) {
	    /*
	     * Check if we need to show entry/entries for a specific address. If
	     * we're looking at entries for a specific address, then skip this
	     * entry if this entry's address doesn't match the address we're looking
	     * for.
	     */
	    if (addr->decnet_addr != entry->dn_addr) {
		temp_entry = entry->next;
	        free(entry);
	        entry = temp_entry;
		continue;
	    }
	}

	printf("\n ");
	if (entry->dn_addr == 0) 
	    sprintf(buffer, "%s", "DEFAULT");
	else {
	    node = (entry->dn_addr) & 0x3ff;
	    area = (entry->dn_addr) >> 10;
	    if (node != 0)
    	        sprintf(buffer, "%d.%d", area, node);
	    else
	        sprintf(buffer, "%d", area);
	}

	printf("%9s%-6d%-8d", buffer, entry->cost, entry->hops);

	if (!entry->is_int_static_route) {
	    node = (entry->next_hop_dn_addr) & 0x3ff;
	    area = (entry->next_hop_dn_addr) >> 10;
	    printf("%-18d.%d", area, node);
        }
	else {
	    if (entry->idb) 
		printf("%-22s", entry->idb->namestring);
	    if (entry->snpa.length != 0)
		printf("\t%s", print_snpa_string(buffer, &entry->snpa));
	}

	temp_entry = entry->next;
	free(entry);
	entry = temp_entry;
    }
    printf("\n");
}

/*
 * nv_decnet_host_cmd
 *
 * Handle nv generation of 'decnet host' commands.
 */
void nv_decnet_host_cmd (parseinfo *csb)
{
    int area, node;
    nametype *name;

    name = (nametype *) nameQ.qhead;
    while (name) {
	if ((name->flags & NAM_PERMANENT) && (name->flags & NAM_DECNET)) {
	    area = (name->v.decnet.adr.addr.decnet_address) >> 10;
	    node = (name->v.decnet.adr.addr.decnet_address) & 0x3ff;
	    nv_write(TRUE, "%s %s", csb->nv_command, name->name);
	    nv_add(TRUE, " %d.%d", area, node);
	}
	name = name->next;
    }

    return;
}

/*
 * nv_decnet_route_cmd
 *
 * Handle nv generation of 'decnet route' commands.
 */
void nv_decnet_route_cmd (parseinfo *csb)
{
    decnet_staticQtype *entry;
    int area, node;
    char buffer[3*ADDRLEN_MAX];

    nv_write(TRUE, "!");

    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; 
 	 entry = entry->next) {

	area = (entry->dn_addr) >> 10;
	node = (entry->dn_addr) & 0x3ff;
	/*
	 * Generate "dec route a.b .. ", or "dec route default .. "
	 */
	if (entry->dn_addr != 0)
 	    nv_write(TRUE, "%s %d.%d ", csb->nv_command, area, node);
	else
 	    nv_write(TRUE, "%s default ", csb->nv_command);
 
 	/*
	 * 0-length next-hop and NULL idb means a discard entry.
	 */
 	if (!entry->idb) {
	    if (entry->next_hop_dn_addr == 0) {
 	        nv_add(TRUE, "discard");
	    }
	    else {
	        area = (entry->next_hop_dn_addr) >> 10;
	        node = (entry->next_hop_dn_addr) & 0x3ff;
		nv_add(TRUE, "%d.%d", area, node);
	    }
 	} else {
 	    /*
	     * Interface static routes (includes SNPA and interface).
	     */
	    nv_add (TRUE, "%s %s", entry->idb->namestring,
		    is_p2p(entry->idb) ? " " :
		    print_snpa_string(buffer, &entry->snpa));
  	}

	if (entry->hops || entry->cost) {
 	    nv_add (TRUE, " %d %d", entry->hops, entry->cost);
	}
    }

    return;
}

/*
 * register_dec_route_routines
 *
 * Enter any service calls (relevant to this module) to registry.  Called by
 * 'dn_register_thyself' in decnet.c.
 */

void register_dec_route_routines (void)
{
    reg_add_name_show_hosts(NAM_DECNET, decnet_name_show_hosts,
			    "decnet_name_show_hosts");
    reg_add_name_match_number(ADDR_DECNET, decnet_name_match_number,
			      "decnet_name_match_number");

}


