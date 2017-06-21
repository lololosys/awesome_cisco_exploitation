/* $Id: clns_route.c,v 3.3.60.7 1996/08/08 14:50:31 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_route.c,v $
 *------------------------------------------------------------------
 * clns_route.c -- General prefix routing table support routines.
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_route.c,v $
 * Revision 3.3.60.7  1996/08/08  14:50:31  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.3.60.6  1996/06/18  18:55:57  asastry
 * CSCdi58806:  DEC "advertised" route does not work correctly if ISO-IGRP
 * is used.
 * Branch: California_branch
 *
 * Revision 3.3.60.5  1996/05/06  06:14:49  asastry
 * CSCdi40503:  CLNS routing problems caused by discard routes (DECnet
 * conversion)
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/05/04  20:04:08  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/04/05  21:51:21  asastry
 * CSCdi52752:  CPU load raises to 99/100% during big clns file transfers
 * - Part 2
 *
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/04/03  21:49:28  asastry
 * CSCdi52752:  High CPU utilization during large CLNS file transfers
 *
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:03:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:13:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:40:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:40:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:52:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  18:02:54  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/06/18  19:39:41  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:17:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_adj.h"
#include "config.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../iprouting/route_map.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "clns_inlines.h"
#include "exception.h"

/*
 * net_equal
 * Call net_equ, but use the new types.
 */

static inline boolean net_equal (nsap_addr *nsap1, nsap_addr *nsap2)
{
    return(net_equ((uchar *)nsap1, (uchar *)nsap2));
}


/*
 * clns_route_update
 *
 * This routine inserts a new prefix route. It is called for
 * static routes (and static discards) as well as dynamic prefix
 * routes.
 */
clns_rdbtype *clns_route_update (
    NSAP_T  nsap_prefix,
    NSAP_T  first_hop,
    idbtype *first_hop_idb,
    int     entry_type,
    ulong   protocol_source,
    ulong   distance,
    ulong   validation_time,
    sys_timestamp expiration_time,
    int     metric,
    NSAP_T  source_NET)
{

    clns_ddbtype *ddb;
    clns_rdbtype *rdb = NULL;
    boolean      new_route, cache_refresh;
    int          i;
    clns_local_nettype *local_net;
    char         *msg;

    /*
     * Don't add routes with infinite distance.
     */
    if (distance == ISO_INFINITE_DIST) {
	return(NULL);
    }

    new_route = TRUE;
    cache_refresh = FALSE;
    ddb = nsap_lookup(nsap_prefix, MATCH_EXACT);
    if (ddb) {
	new_route = FALSE;
    } else {
	ddb = malloc(sizeof(clns_ddbtype));
	if (!ddb) {
	    return(NULL);
	} else {
	    nsap_copy(nsap_prefix, ddb->destination);
	    cache_refresh = TRUE;
	}
    }
    
    ddb->entry_type = entry_type;

    rdb = NULL;
    if ((entry_type != CLNS_OURNET_ENTRY) &&
	(entry_type != CLNS_OURNSAP_ENTRY)) {

	/*
	 * Check if new route should override existing route based
	 * on adminstrative distance.
	 */
	if (!new_route) {

	    /*
	     * Existing route has better distance.
	     */
	    if (distance > ddb->distance) {
		return(NULL);
	    }

	    /*
	     * New route is better. Delete rdb's for old route.
	     */
	    if (distance < ddb->distance) {
 		clns_delete_all_rdbs(ddb);
	    }
	}

	/*
	 * Look for rdb with matching first_hop, if any.
	 */
	for (i = 0; i < ddb->routecount; i++) {
	    rdb = ddb->rdb[i];
	    if (nsap_equ(first_hop, rdb->first_hop) &&
		(first_hop_idb == rdb->idb)) {
		break;
	    }
	}
 
 	/* If no matching first hop, add an rdb. */
 
 	if (i == ddb->routecount && ddb->routecount < MAXROUTES) {
	    rdb = malloc(sizeof(clns_rdbtype));
	    if (!rdb) {
		if (new_route) {
		    free(ddb);
		}
		return(NULL);
	    }
	    ddb->rdb[ddb->routecount] = rdb;
	    ddb->routecount++;
	    cache_refresh = TRUE;
	}
			       
	/*
	 * We either found an old one or allocated a new one.
	 * Fill it in.
	 */
	if (first_hop) {
	    nsap_copy(first_hop, rdb->first_hop);
	} else {
	    clear_nsap(rdb->first_hop);
	}
	rdb->idb = first_hop_idb;
	rdb->validation_time = validation_time;
	rdb->protocol_source = protocol_source;
	rdb->expiration_time = expiration_time;
	rdb->validation_time = validation_time;
	if (source_NET)
	    nsap_copy(source_NET, rdb->info_source);

	if (ddb->metric != metric)
	    cache_refresh = TRUE;
	ddb->metric = metric;

	/*
	 * Set adminstrative distance.
	 */
	ddb->distance = distance;
    
	/*
	 * If this is a new route and it is static, set pdb->changed
	 * in all pdbs that redistribute static. This will cause them to
	 * do flash updates.
	 */
	if (new_route && ddb->entry_type == CLNS_ROUTE_ENTRY) {
	    clns_flash_update(protocol_source);
	}
    } else if (new_route) {

	/*
	 * Store local NET in separate list too. Used for easy
	 * access when sending ISHes.
	 */
	local_net = malloc(sizeof(clns_local_nettype));
	if (local_net) {
	    nsap_copy(nsap_prefix, local_net->net);
	    enqueue(&clns_local_nets, local_net);
	}
	clns_refresh_my_net();
    }

    if (new_route && !nsap_insert(ddb)) {
	clns_rbuginf("\nCLNS-RT: could not insert route %n", 
		     nsap_prefix);
	free(ddb);
	return(NULL);
    }

    /*
     * Cache refresh should occur only if something changed.
     */
    if (cache_refresh) {
	clns_invalidate_cache("clns_route_update", FALSE);
    }
    else {
	clns_ibuginf("\nNo cache refresh for update of prefix %n",
		     nsap_prefix);
    }

    /*
     * If we added the default prefix, store in global for packet
     * forwarding decisions.
     */
    if (!nsap_prefix[0]) {
	clns_default_prefix = ddb;
    }

    if (clns_routing_debug) {
	if (first_hop) {
	    if (first_hop[0] == STATION_LENGTH)
		msg = station_string(&first_hop[1]);
	    else
		msg = nsap_string(first_hop);
	} else
	    msg = "local";

	buginf("\nCLNS-RT: Add %s to prefix table, next hop %s",
	       (nsap_prefix[0]) ? nsap_string(nsap_prefix) : "Default", 
	       msg);
    }

    /*
     * Check if this route is being redistributed into IS-IS. If so, add
     * a prefix neighbor adjacency to the adjacency database. This is not
     * done here for static routes. Static routes are always added to the
     * adjacency database when entered at keyboard.
     */
    if ((protocol_source != ISO_STATIC) && (protocol_source != DECNET_DISCARD)) {
	clns_manage_prefix_nbrs(ddb, protocol_source, TRUE);
    }

    return(rdb);
}

/*
 * nsap_insert
 * Insert the ddb into the CLNS prefix routing table.
 * Return FALSE if it already exists.
 */
boolean nsap_insert (clns_ddbtype *ddb)
{

    int h;

    if (nsap_lookup(ddb->destination, MATCH_EXACT)) {
	return(FALSE);
    }

    /*
     * Insert into hash table. 
     */
    h = nsap_hash(ddb->destination[0], &ddb->destination[1]);
    ddb->next = clns_routes[h];
    clns_routes[h] = ddb;

    return(TRUE);
}

/*
 * nsap_delete
 * 
 * Delete prefix routing table entry. This entry will free the ddb,
 * all associated rdb's and delink the ddb from the hash table.
 *
 */
boolean nsap_delete (NSAP_T nsap)
{
    clns_ddbtype *ddb;
    clns_ddbtype *prev_ddb;
    int          h;
    boolean      local, status;
    clns_local_nettype *local_net;

    status = FALSE;
    local = FALSE;

    /*
     * Remove this from the cache just in case its there.
     */
    nsap_cache_delete(nsap);

    /*
     * Search for destination ddb.
     */
    prev_ddb = NULL;
    h = nsap_hash(nsap[0], &nsap[1]);
    for (ddb = clns_routes[h]; ddb; prev_ddb = ddb, ddb = ddb->next) {
	if (nsap_equ(nsap, ddb->destination)) {
	    local = (ddb->entry_type == CLNS_OURNSAP_ENTRY ||
		     ddb->entry_type == CLNS_OURNET_ENTRY);
	    clns_delete_all_rdbs(ddb);
	    if (prev_ddb) {
		prev_ddb->next = ddb->next;
	    } else {
		clns_routes[h] = ddb->next;
	    }
	    if (!ddb->destination[0]) {
		clns_default_prefix = NULL;
	    }
	    ddb->next = NULL;
	    free(ddb);
	    status = TRUE;
	    break;
	}
    }

    /*
     * If deleting a local NET, remove from local NET list.
     */
    if (local) {
	for (local_net = clns_local_nets.qhead; 
	     local_net; local_net = local_net->next) {
	    if (nsap_equ(nsap, local_net->net)) {
		unqueue(&clns_local_nets, local_net);
		free(local_net);
		break;
	    }
	}
    }

    /* Update clns_myNET in case it was deleted. */

    clns_refresh_my_net();

    return(status);
}

/*
 * nsap_delete_rdb
 * 
 * Delete a specific next hop entry for a given prefix route.
 * This function is available for static routes only.
 * Returns TRUE if the rdb was successfully deleted.
 *
 * If prefix == next_hop, this is an interface route.
 */
boolean nsap_delete_rdb (NSAP_T prefix, NSAP_T next_hop, idbtype *idb)
{
    clns_ddbtype *ddb;
    clns_rdbtype *rdb;
    int          i;
    
    ddb = nsap_lookup(prefix, MATCH_EXACT);
    if (!ddb)
	return(FALSE);

    /*
     * If there is only one next-hop, call the traditional
     * nsap_delete() to remove the prefix route.
     */
    if (ddb->routecount == 1) {
	return(nsap_delete(prefix));
    }

    /*
     * Find the correct rdb and delete it.
     */
    for (i = 0; i < ddb->routecount; i++) {
	rdb = ddb->rdb[i];
	if (!nsap_equ(next_hop, rdb->first_hop) || rdb->idb != idb) {
	    continue;
	}
	clns_delete_rdb(ddb, i);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nsap_next
 *
 * This function is used to traverse the CLNS prefix routing table. If
 * ddb is NULL upon entry, the search begins from the start of routing
 * table. If non-NULL, it continues from hash position of current ddb.
 */
clns_ddbtype *nsap_next (clns_ddbtype *ddb)
{
    int h;

    if (!ddb) {
	h = 0;
    } else {
	if (ddb->next) {
	    return(ddb->next);
	}
	h = nsap_hash(ddb->destination[0], &ddb->destination[1]) + 1;
    }

    for (; h < CLNS_NETHASHLEN; h++) {
	if (!(ddb = clns_routes[h])) {
	    continue;
	}
	return(ddb);
    }

    return(NULL);
}

/*
 * nsap_lookup
 *
 * This function will perfrom a table lookup on an NSAP address in the
 * prefix routing table. Best match is always performed unless an exact
 * match is requested. If an exact match is requested, and there is not
 * one in the table, a NULL value is returned.
 */
clns_ddbtype *nsap_lookup (NSAP_T nsap, uchar match_type)
{
    clns_ddbtype *ddb;
    clns_ddbtype *best_ddb;
    int          best_val, this_val, h;

    /*
     * If exact match is requested, use hash function to get to hash table
     * location.
     */
    if (match_type == MATCH_EXACT) {
	h = nsap_hash(nsap[0], &nsap[1]);
	for (ddb = clns_routes[h]; ddb; ddb = ddb->next) {
	    if (nsap_equ(ddb->destination, nsap)) {
		break;
	    }
	}
	return(ddb);
    }

    /*
     * If closest match requested, traverse prefix routing table for 
     * best match.
     */
    best_val = 0;
    best_ddb = NULL;
    for (h = 0; h < CLNS_NETHASHLEN; h++) {
	for (ddb = clns_routes[h]; ddb; ddb = ddb->next) {
	    this_val = nsap_match(ddb->destination, nsap);
	    if (this_val > best_val) {
		best_val = this_val;
		best_ddb = ddb;
	    }
	}
    }
    return((best_ddb) ? best_ddb : clns_default_prefix);
}

/*
 * nsap_cache_delete
 *
 * delete the specified nsap from the cache
 */
void nsap_cache_delete (NSAP_T nsap)
{
    clns_cache_adjtype *adj, *next;
    clns_cachetype     *cache, *prev;
    int                h;
    
    prev = NULL;
    h = nsap_hash(nsap[0], &nsap[1]);

    for (cache = clns_cache[h]; cache; cache = cache->next) {
	if (net_equ(nsap, cache->destination)) {
	    break;
	}
	prev = cache;
    }

    if (cache) {
	clns_rbuginf("\nCLNS-RT: Deleting cache entry for: %n", nsap);
	if (prev) {
	    prev->next = cache->next;
	} else {
	    clns_cache[h] = cache->next;
	}

	adj = cache->cache_adj;
	while (adj) {
	    next = adj->next;
	    adj->next = NULL;
	    adj->adj = NULL;
	    free(adj);
	    if (next == cache->cache_adj) {
		break;
	    }
	    adj = next;
	}
	cache->next = NULL;
	free (cache);
	clns_cache_changes++;
    }
}    

/*
 * clns_make_cache_adj
 *
 * Allocate and fill in a cache entry with next hop adjacency 
 * information.
 */
void clns_make_cache_adj (clns_cachetype *cache, clns_adjtype *adj)
{

    clns_cache_adjtype *cache_adj, *temp_adj;    

    if (!cache)
	return;

    /*
     * Process for each cluster alias this adjacency has.
     */
    for ( ; adj; adj = adj->next_alias) {

	/*
	 * Allocate memory for adjacency cache entry.
	 */
	cache_adj = malloc(sizeof(clns_cache_adjtype));
	if (!cache_adj)
	    return;

	cache_adj->adj = adj;
	if (adj->snpa.length > 0) {
	    /*
	     * Copy destination SNPA to CLNS route cache, and convert
	     * to native bit order
	     */
	    bcopy(adj->snpa.addr, cache_adj->dst_snpa, adj->snpa.length);
	    if (adj->idb->hwptr->make_snpa_native)
		adj->idb->hwptr->make_snpa_native(adj->idb->hwptr,
						  cache_adj->dst_snpa);
	}

	/*
	 * Cache adjacencies are linked in a circular link list. Find
	 * last one in chain and have it point to the new entry.
	 */
	for (temp_adj = cache->cache_adj; 
	     temp_adj && temp_adj->next != cache->cache_adj; 
	     temp_adj = temp_adj->next) {
	}

	/*
	 * Insert to link list.
	 */
	if (!temp_adj) {
	    /* Point to self. */
	    cache_adj->next = cache_adj;
	    /* Set head of list. */
	    cache->cache_adj = cache_adj;
	} else {
	    /* Last element points to new first. */
	    temp_adj->next = cache_adj;
	    /* Set new first to point to old first. */
	    cache_adj->next = cache->cache_adj;
	    /* Set head of list. */
	    cache->cache_adj = cache_adj;
	}
    }
     
    return;
}

/*
 * clns_make_route_cache
 *
 * Build a linked list of route cache entries for the destination.
 *
 *    route_nrib - the route descriptor block from the 
 *                 conforming or non-conforming databases 
 */
clns_cachetype *clns_make_route_cache (NSAP_T dest_nsap, clns_adjtype *adj)
{
    clns_cachetype *entry;
    int            h, depth;

    /*
     * Do not create cache entries for DECNET Phase IV adjacencies. This
     * allows us to do DECNET conversion on the last hop to a Phase IV
     * node. If we did build one, CLNS fast-switching blindly tries to
     * send an OSI packet to a Phase IV node.
     */
    if (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) {
	return(NULL);
    }

    depth = 0;
    h = nsap_hash(dest_nsap[0], &dest_nsap[1]);
    for (entry = clns_cache[h]; entry; entry = entry->next) {
	if (net_equ(dest_nsap, entry->destination)) {
	    break;
	}
    }

    if (entry) {

	/* 
	 * We found an existing cache entry for this destination
	 * This must be an expired entry that hasn't been removed yet 
	 * Should I just remove it ???
	 */
	clns_rbuginf("\nCLNS-RT: Deleting obsolete cache entry for: %n",
		     entry->destination);
	nsap_cache_delete(entry->destination);
	entry = NULL;
    }

    /*
     * Fill in the nametype stuff.
     */
    if (!entry) {
	entry = malloc(sizeof(clns_cachetype));
	if (entry) {
	    nsap_copy(dest_nsap, entry->destination);
	    entry->cache_version = clns_cache_version;
	    TIMER_START(entry->cache_expiration,
			CLNS_ROUTE_CACHE_HOLDING_TIMER);
	    entry->cache_adj = NULL;
	} else {
	    /* could not malloc cache space */
	    return(NULL);
	}
    }

    if (adj)
	clns_make_cache_adj(entry, adj);

    /*
     * Hook this into the actual cache structure.
     */
    entry->next = clns_cache[h];
    clns_cache[h] = entry;
    clns_cache_changes++;

    return(entry);
}

/* 
 * nsap_cache_search
 *
 *    This routine will search the clns_cache for the specified nsap
 */
clns_cache_adjtype *nsap_cache_search (NSAP_T nsap)
{
    clns_cachetype     *cache;
    clns_cache_adjtype *cache_adj;
    int                hash;

    hash = nsap_hash(nsap[0], &nsap[1]);
    for (cache = clns_cache[hash]; cache; cache = cache->next) {
	if (net_equ(nsap, cache->destination)) {
	    if (clns_cache_version == cache->cache_version) {
		break;
	    }
	}
    }

    if (cache) {
	cache_adj = cache->cache_adj;
	cache->cache_adj = cache_adj->next;
    } else {
	cache_adj = NULL;
    }

    return(cache_adj);
}


/*
 * clns_route_search
 *
 * Jacket routine (called by DECnet) to determine the existence of a
 * CLNS route.
 */
boolean clns_route_search (uchar *prefix)
{
    uchar domain_length;
    NSAP_T area;
    clns_pdbtype *pdb;

    if (nsap_lookup(prefix, MATCH_EXACT))
	return (TRUE);

    /* 
     * no prefix route - is there an area route in ISO-IGRP?? First
     * get an ISO-IGRP pdb that matches the conversion prefix. The
     * prefix that's passed in will consist of the 'domain' and the
     * area.
     */
    domain_length = prefix[0] - AREA_LENGTH;
    area[0] = AREA_LENGTH;
    bcopy(&prefix[domain_length+1], &area[1], AREA_LENGTH);
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->typeflag & ISO_IGRPMASK) {
	    if (clns_domain_match(pdb->domain_length, pdb->domain,
				  domain_length, prefix+1)) {
		if (clns_find_dest(pdb, area)) {
		    return(TRUE);
		}
            }
	}
    }
    
    return(FALSE);
}

/*
 * clns_discard_route_adjust
 *
 * Set the state of the static discard route with the given prefix to
 * the value indicated by the parameter "up_state". If "up_state" is TRUE,
 * set state of the route to UP, else set it to DOWN.
 */
void clns_discard_route_adjust (uchar *prefix, boolean up_state)
{
    clns_ddbtype       *ddb;
    clns_rdbtype       *rdb;
    clns_pdbtype       *pdb;
    clns_adjtype       *adj;
    NSAP_T             next_hop_nsap;
    boolean            inform_isis;

    ddb = nsap_lookup(prefix, MATCH_EXACT);
    pdb = find_isis_pdb();
    inform_isis = (pdb && pdb->net[0] && (pdb->level & ISIS_CIRCUIT_L2) &&
		   (pdb->redistallowed & DECNET_DISCARD));

    if (!up_state) {	
        if (!ddb)
	    return;    

	/*
	 * Don't bother if it's not a DECNET DISCARD entry
	 */
	if (ddb->entry_type != CLNS_DECNET_DISCARD_ENTRY)
	    return;
	if (!nsap_delete(prefix)) {
	    clns_rbuginf("\nCLNS-RT: Could not delete %s", nsap_string(prefix));
	}

	/*
	 * Delete from adjacency table.
	 */
	adj = find_adjacency(clns_adjacency_db, prefix, FALSE, NULL, 0);
	if (adj) {
	    delete_adjacency(clns_adjacency_db, adj);
	}

	/*
	 * Tell IS-IS about it.
	 */
	if (inform_isis)
	    isis_mark_L2_changed(pdb);

	return;
    }

    /*
     * Else if the route exists, set its state to ON or OFF, otherwise
     * create the route.
     */

    if (ddb) {
        adj = find_adjacency(clns_adjacency_db, prefix, FALSE, NULL, 0);
        if (adj) {
	    /*
	     * If no change, just return.
	     */
	    if ((up_state && adj->state == CLNS_ADJ_UP_STATE) ||
	        (!up_state && adj->state == CLNS_ADJ_FAILED_STATE))
		return;

            adj->state = (up_state) ? CLNS_ADJ_UP_STATE : CLNS_ADJ_FAILED_STATE;
            clns_rbuginf("\nCLNS-RT: Turned prefix route %s ",
			 nsap_string(prefix)); 
	    clns_rbuginf("to state %s", (up_state) ? "ON" : "OFF");

        }
        if (inform_isis)
	    isis_mark_L2_changed(pdb);
    }
    else {
        clear_nsap(next_hop_nsap);

        /*
         * Add into routing table.
         */
        rdb = clns_route_update(prefix, next_hop_nsap, NULL,
			        CLNS_DECNET_DISCARD_ENTRY, DECNET_DISCARD,
			        ISO_DECNET_DISCARD_DIST, 0, timer_no_time, 0,
				*clns_get_local_npai(NULL));
        /*
         * Add into adjacency table.
         */
        adj = add_adjacency(clns_adjacency_db, prefix, NULL, NULL, 0, TRUE);
        if (adj) {
	    adj->protocol_source |= DECNET_DISCARD;
	    adj->state = CLNS_ADJ_UP_STATE;
	    adj->adjacencyType = CLNS_PREFIX_ADJ_TYPE;
	    adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
	}
        if (inform_isis)
	    isis_mark_L2_changed(pdb);
    }

    return;
    
}

/*
 * clns_adjust_static_routes
 *
 * An interface or adjacency has gone down. Set status in prefix neighbor
 * entries of adjacency database. Inform IS-IS of change. ISO-IGRP will
 * find out next update interval.
 *
 * If down_adj != NULL, process a single adjacency up/down rather than the 
 * interface.
 *
 * The build_lsp flag indicates whether the L2 LSP should be built.
 */
boolean clns_adjust_static_routes (clns_adjtype *down_adj, idbtype *idb,
				boolean up_state, boolean build_lsp)
{
    clns_pdbtype     *pdb;
    clns_rdbtype     *rdb;
    clns_ddbtype     *ddb;
    clns_adjtype     *adj;
    clns_staticQtype *sre;
    boolean          inform_isis;

    inform_isis = FALSE;

    /*
     * Scan through each configured static prefix route. Find the 	
     * corresponding prefix neighbor in the adjacency database. Set
     * the new status.
     */
    for (sre = clns_staticQ.qhead; sre; sre = sre->next) {

	/*
	 * A single adjacency has gone up or down.
	 */
	if (down_adj) {

	    /*
	     * Ignore static interface routes when a single adjacency goes
	     * down.
	     */
	    if (sre->idb)
		continue;

	    /*
	     * Next-hop system-ids don't match.
	     */
	    if (!station_match(get_station(sre->next_hop), 
			       &down_adj->key[1])) continue;
	} else {
	    if (sre->idb) {
		if (sre->idb != idb)
		    continue;
	    } else {
		down_adj = find_adjacency(clns_adjacency_db, sre->next_hop,
					  TRUE, idb, 0);
		if (!down_adj)
		    continue;
	    }
	}

	/*
	 * This static route is affected by adjacency/interface state change.
	 */
	adj = find_adjacency(clns_adjacency_db, sre->prefix, FALSE, NULL, 0);
	if (adj) {
	    adj->state = (up_state) ? CLNS_ADJ_UP_STATE : 
		CLNS_ADJ_FAILED_STATE;
	    inform_isis = TRUE;
	}
	
	/*
	 * For ISO-IGRP, check to see if we need to reinstate any interface
	 * static routes after the interface came back up. If there is
	 * a prefix route that was learnt via ISO-IGRP, we should overwrite
	 * it with the static one that we have configured (locally.)
	 *
	 * We won't send a flash update, and let the next ISO-IGRP update
	 * send the new route information to all the neighbors.
	 */
	if (up_state) {
	    ddb = nsap_lookup(sre->prefix, MATCH_EXACT);
	    if (ddb->distance == ISO_STATIC_DIST) 
	        continue;
	    rdb = ddb->rdb[0];
	    if (!rdb) 
	        continue;
	    /*
	     * Don't bother if the route in the table is already a 
	     * static route. 
	     */
	    if (rdb->protocol_source & ISO_STATIC)
	        continue;
	    /*
	     * Do not touch any routes other than ISO-IGRP routes!!
	     */
	    if (!(rdb->protocol_source & ISO_IGRPMASK))
	        continue;

	    clns_ibuginf("\nISO-IGRP:Reinstating static route for %s",
			 nsap_string(sre->prefix));
	    clns_ibuginf("\nISO-IGRP:(next hop interface %s)", 
			 sre->idb->namestring);
	    clns_route_update(sre->prefix, 
			      idb ? sre->prefix : sre->next_hop, 
			      sre->idb, 
			      CLNS_ROUTE_ENTRY, ISO_STATIC, ISO_STATIC_DIST, 
			      0, timer_no_time, 0, *clns_get_local_npai(NULL));
	    /*
	     * Add into adjacency table.
	     */
	    adj = add_adjacency(clns_adjacency_db, sre->prefix, 
				sre->idb, NULL, -1, 
				sre->idb->clns_cluster_alias);
	    if (adj) {
	        adj->protocol_source |= ISO_STATIC;
		adj->state = CLNS_ADJ_UP_STATE;
		adj->adjacencyType = CLNS_PREFIX_ADJ_TYPE;
		adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
		adj->idb = sre->idb;
		if (idb) {
		    bcopy(&sre->snpa, &adj->snpa, sizeof(hwaddrtype));
		}
	    }
	}
    }

    /*
     * If the interface state change affected any routes, tell IS-IS.
     * If not allowed to rebuild the lsp, just return.
     */
    if (!build_lsp)
	return (inform_isis);

    if (inform_isis) {
	pdb = find_isis_pdb();
       if (!pdb || !pdb->net[0]) {
	    return (FALSE);
	}
	if ((pdb->level & ISIS_CIRCUIT_L2) &&
	    (pdb->redistallowed & ISO_STATIC)) {
	    isis_mark_L2_changed(pdb);
	}
    }
    return (FALSE);
}

/*
 * clns_delete_rdb - delete an RDB
 *
 * Shuffles all later RDBs down one.
 * Frees the memory.
 * Decrements ddb->routecount.
 * Returns TRUE if all went OK, FALSE if things are wacky.
 */
boolean clns_delete_rdb(clns_ddbtype *ddb, int rdb_number)
{
    int i;
    clns_rdbtype *rdb;

    if (ddb->routecount <= 0 || ddb->routecount > MAXROUTES)
	return (FALSE);

    if (rdb_number < 0 || rdb_number >= ddb->routecount)
	return (FALSE);
    rdb = ddb->rdb[rdb_number];
    if (rdb == NULL)
	return (FALSE);

    ddb->routecount--;
    for (i = rdb_number; i < ddb->routecount; i++) {
	ddb->rdb[i] = ddb->rdb[i+1];
    }
    ddb->rdb[ddb->routecount] = NULL;
    free (rdb);
    return (TRUE);
}

/*
 * clns_delete_all_rdbs - delete all rdbs for a route
 */
void clns_delete_all_rdbs(clns_ddbtype *ddb)
{
    while (clns_delete_rdb(ddb, 0)) {};
}

/*
 * clns_pdb_routemap
 *
 * Examine route-map for redistributing protocol. If one is define,
 * examine match conditions and return result. pdb->route_map holds
 * map for later retrieval of the set parameters.
 */
boolean clns_pdb_routemap (clns_pdbtype *pdb, clns_ddbtype *ddb)
{
    clns_rdbtype *rdb;
    int          index;
    int          typeflag;
    uchar        *map_name;
    
    rdb = ddb->rdb[0];
    if (!rdb) {
	return(TRUE);
    }
    
    typeflag = rdb->protocol_source;
    for (index = 0; index < CLNS_MAXROUTERS; index++, typeflag >>= 1) {
	if (typeflag & 1)
	    break;
    }
    
    map_name = pdb->routemap[index];
    if (map_name) {
	pdb->route_map = route_map_clns_check(map_name, ddb, rdb);
	if (!pdb->route_map)
	    return(FALSE);
    } else 
	pdb->route_map = NULL;
    return(TRUE);
}

/*
 * clns_add_cache_rewrite
 * Add a rewrite entry to a cache entry.  First walk the cache to find the
 * destination in the cache, then walk the adjacencies to find the same
 * adjacency.
 */

void clns_add_cache_rewrite (paktype *pak, ulong dest_addr)
{
    nsap_addr *dest = (nsap_addr *)pak->clns_dstaddr;
    nsap_addr *nexthop = (nsap_addr *)pak->clns_nexthopaddr;
    nsap_addr temp_nsap;
    clns_cachetype *cache;
    clns_cache_adjtype *cache_adj;
    hwidbtype *hwidb;
    int len, hash;
    trrif_t *trh;

    if (pak->if_input == NULL)
	return;
    /*
     * Update the cache with transit (routed) packets, not those sourced by us.
     */

    if (!dest) return;

    /*
     * Check for multicast and incorrect encapsulation type
     */
    hwidb = pak->if_output->hwptr;
    if (hwidb->status & IDB_ETHER) {	/* if ethernet */
	if ((*pak->datagramstart & 0x01) == 1)	/* then forbid multicast */
	    return;
	len = ETHER_SAP_ENCAPBYTES;
    } else if (hwidb->status & IDB_FDDI) {
	switch (hwidb->type) {
	  case IDBTYPE_FDDIT:
	    if (pak->datagramstart[1] & 0x01)
		return;
	    break;
	  case IDBTYPE_BSI:
	  case IDBTYPE_FDDI:
	    if (pak->datagramstart[1] & 0x80)
		return;
	    break;
	  default:
	    crashdump(0);
	}
	len = FDDI_SAP_ENCAPBYTES;
    } else if (hwidb->status & IDB_TR) {
	if (pak->enctype != ET_SAP)
	    return;
	trh = (trrif_t *)pak->datagramstart;
	if (trh->saddr[0] & TR_RII)
	    len = TRING_SAP_ENCAPBYTES +(trh->rc_blth & RC_LTHMASK);
	else
	    len = TRING_SAP_ENCAPBYTES;
    } else if (hwidb->status & IDB_SERIAL) {
	len = pak->encsize;
    } else if (is_atm(hwidb)) {
	len = pak->encsize;
    } else
	return;

    /*
     * Find the destination in the cache. Make sure the NSEL is zeroed out 
     * before the cache look-up. Use a temporary buffer to do the hash
     * look-up, so that we preserve the original NSAP.
     */
    bcopy(dest, &temp_nsap, dest->length+1);
    temp_nsap.addr[dest->length - 1] = 0;
    hash = nsap_hash(temp_nsap.length, temp_nsap.addr);
    cache = clns_cache[hash];		/* stupid compiler */
    for ( ; cache; cache = cache->next) {
	if (net_equal(dest, (nsap_addr *)cache->destination))
	    break;
    }
    
    if (!cache)
	return;
    if (clns_cache_version != cache->cache_version)
	return;

    /*
     * Find the adjacency.
     */
    cache_adj = cache->cache_adj;
    while (TRUE) {
	if (!cache_adj)
	    return;

	/*
	 * Check NETs and check the interface, just in case we're load balancing.
	 */
	if ((cache_adj->adj->idb == pak->if_output) &&
	    net_equal(nexthop, (nsap_addr *)cache_adj->adj->key))
	    break;
	cache_adj = cache_adj->next;

	/*
	 * If we're back at the head of the list, we've had a match
	 * failure.  Just bail out.
	 */
	if (cache_adj == cache->cache_adj)
	    return;
    }

    /*
     * Sanity check to make sure the rewrite fits, and then copy it.  If
     * there's an entry present, don't overwrite it.
     */
    if (len > MAXMACSTRING)
	return;
    if (cache_adj->length)
	return;
    bcopy(pak->datagramstart, cache_adj->rewrite, len);
    cache_adj->length = len;
    clns_cache_changes++;
}
