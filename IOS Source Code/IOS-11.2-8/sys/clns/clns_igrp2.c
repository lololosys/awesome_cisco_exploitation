/* $Id: clns_igrp2.c,v 3.5.12.5 1996/08/08 14:50:20 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_igrp2.c,v $
 *------------------------------------------------------------------
 * clns_igrp_support.c -- Support routines for the CLNS IGRP process
 *
 * 12-January-1989, Paulina Knibbe
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_igrp2.c,v $
 * Revision 3.5.12.5  1996/08/08  14:50:20  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.5.12.4  1996/05/04  20:03:57  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/04/06  00:35:58  asastry
 * CSCdi53023:  ISO-IGRP should poison redistributed routes if the
 * redistribution is disabled.
 *
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/03/26  20:22:31  asastry
 * CSCdi50714:  ISO-IGRP does not install second (equal-cost) path in CLNS
 * prefix table.
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/03/18  19:02:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  08:40:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:20:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  08:11:12  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1996/01/25  22:18:57  rchandra
 * CSCdi44688:  ISO-IGRP: 4k based images cannot interoperate with 68k
 * images
 * - do not super-impose structure on the byte stream while generating and
 *   parsing update
 *
 * Revision 3.3  1995/11/17  08:52:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/05  22:22:53  myeung
 * CSCdi41579:  Misaligned accesses in clns_add_prefixroute/
 * clns_update_prefixroute
 * - Use GETLONG to read the validation time in the packet
 *
 * Revision 2.4  1995/10/04  20:06:24  rchandra
 * CSCdi37403:  clns/iso_igrp misaligned accesses in clns_setupprefix
 * - use PUTLONG to write validiation time
 *
 * Revision 2.3  1995/09/13  07:01:21  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.2  1995/07/18 23:14:24  asastry
 * CSCdi37284:  Redistribution of clns routes can cause crash.
 *
 * Revision 2.1  1995/06/07  20:17:19  hampton
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
#include "mgd_timers.h"
#include "../x25/x25.h"
#include "../if/network.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "../iprouting/route_map.h"
#include "../iprouting/igrp.h"
#include "../wan/dialer.h"

/*
 * iso_igrp_adj_filter_cmd
 *
 * Process adjacency-filter command
 *
 * OBJ(string,1) = filter name
 */
void iso_igrp_adj_filter_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    if (csb->nvgen) {
	nv_write(idb->iso_igrp_adj_filter, "%s %s", csb->nv_command,
		 filter_name(idb->iso_igrp_adj_filter));
	return;
    }
    if (!csb->sense) {
	clns_filter_destroy(idb->iso_igrp_adj_filter);
	idb->iso_igrp_adj_filter = 0;
    } else {
	idb->iso_igrp_adj_filter = clns_filter_create(GETOBJ(string,1),
						     idb->iso_igrp_adj_filter);
    }
}

/* 
 * getnextprefix
 *
 *    Return a pointer to the next prefix route we should advertise
 *    out this interface.
 */
clns_ddbtype *getnextprefix (clns_ddbtype *dest, idbtype *interface)
{
    clns_adjtype *adj;
    clns_rdbtype *rdb;
    boolean      sysid_flag;

    while ((dest = nsap_next(dest))) {

	/* 
	 * Don't advertise your own NSAP. If redistributing to
	 * another domain, the prefix route will be in route 
	 * table.
	 */
	if ((dest->entry_type != CLNS_ROUTE_ENTRY) &&
	    (dest->entry_type != CLNS_DISCARD_ENTRY)) {
	    continue;
	}

	/* 
	 * Which interface would we actually use.
	 */
	rdb = dest->rdb[0];
	if (!rdb)
	    continue;

	if (dest->metric == CLNS_METRIC_INACCESSIBLE)
	    return(dest);

	/*
	 * Imbedded system-id in rdb->first_hop (not solely system-id) are
	 * present in static next-hop prefix routes. Interface static
	 * routes have rdb->first_hop set to destination prefix and
	 * rdb->idb set. All other types of routes (i.e. ISO-IGRP and IS-IS)
	 * have only system-id stored in rdb->first_hop.
	 */
	sysid_flag = ((rdb->protocol_source & ISO_STATIC) && !rdb->idb);
	adj = find_adjacency(clns_adjacency_db, rdb->first_hop, sysid_flag,
			     rdb->idb, 0);
	if (!adj) {

	    /*
	     * Redistributing static next-hop prefixes.
	     */
	    if (!rdb->idb)
		return(dest);
	    continue;
	}

	/*
	 * Do split horizon check here. If the next-hop interface for
	 * the route is the same as the interface we are sending the
	 * advertisement on, don't advertise if split horizon is enabled
	 * for interface.
	 */
	if ((adj->idb == interface) && adj->idb->clns_split_enabled)
	    continue;
	return(dest);
    }
    return(NULL);
}


/*
 * getnextdest
 *
 *    Return a pointer to the next destination we should advertise
 *    out this interface. rdb will contain a pointer to the route
 *    descriptor block for this destination.
 *    
 */
clns_ddbtype *getnextdest (
    clns_pdbtype *pdb,
    int *hash_index,
    clns_ddbtype *dest,
    idbtype *interface,
    clns_rdbtype **rdb)
{
    int i;
    clns_adjtype *adj;

    if (*hash_index >= CLNS_NETHASHLEN) {
	return(NULL);
    }

    if (dest) {
	dest = dest->next;
	if (!dest) {
	    *hash_index += 1;
	    if (*hash_index >= CLNS_NETHASHLEN) {
		*rdb = NULL;
		return(NULL);
	    }
	}
    }
    while (TRUE) {
	while (dest) {
	    /*
	     * for each dest, check if we should advertise it
	     * DON'T advertise if:
	     *      there is no route
	     *      there is no interface for the route
	     *      this update packet is going out the same
	     *            interface the route would use (split horizon)
	     *      this route was learned over this interface
	     *
	     * NOTE if there are multiple routes for this destination
	     * we should advertise out this interface if ANY of the
	     * routes passes the checks.
	     *
	     * If there are NO routes (we are in hold-down) advertise
	     * that the destination is no longer reachable.
	     */
	    if (TIMER_RUNNING_AND_SLEEPING(dest->blindtimer)) {
		*rdb = NULL;
		return(dest);
            }
	    for (i = 0; i < dest->routecount; i++) {
		*rdb = dest->rdb[i];
		/*
		 * The following check will force us to advertise
		 * our own NETs and NSAPs in the routing updates.
		 * This is helpful is we are in more than one area
		 * and/or domain.
		 */
		if (((dest)->entry_type == CLNS_OURNSAP_ENTRY)
		    || ((dest)->entry_type == CLNS_OURNET_ENTRY))
		    return(dest);
		    
		/*
		 * is there an available route which passes the
		 * split horizon check ?
		 */
		if (!(*rdb))
		    continue;
		adj = find_adjacency(clns_adjacency_db, (*rdb)->first_hop,
				     FALSE, 0, 0);
		if (!adj)
		    continue;

		/*
		 * Do split horizon check. Only perform if configured on
		 * interface.
		 */
		if ((adj->idb == interface) && adj->idb->clns_split_enabled) {
		    continue;
		}
		return(dest);
	    }
	    dest = dest->next;
	    if (!dest){
		*hash_index += 1;
		if (*hash_index >= CLNS_NETHASHLEN){
		    *rdb = NULL;
		    return(NULL);
		}
	    }
	}
	/*
	 * first time through (and when a hash bucket is exhausted)
	 * find the next hash value which has a destination in it
	 */
	while ((dest = pdb->dest_routing_table[*hash_index]) == 0){
	    *hash_index += 1;
	    if (*hash_index >= CLNS_NETHASHLEN){
		*rdb = NULL;
		return(NULL);
	    }
	}
    }
}

/* clns_setupprefix
 *
 *    This routine is called when we are sending an update. 
 *    It builds a prefix route advertisement at *ptr. If the flag
 *    "poison_prefix_routes" is TRUE, advertise this prefix route
 *    with a metric of infinity.
 */
int clns_setupprefix (
    clns_pdbtype *pdb,
    char *ptr,
    clns_ddbtype *dest,
    boolean poison_prefix_routes)
{
    char         *start_ptr;
    clns_rdbtype *rdb;
    clns_ddbtype *ddb;
    ulong         time;

      /*
       * Remember the start of the prefix. We need this to calculate 
       * the length of the prefix
       */
    start_ptr = ptr;

    /*
     * if we are the originator of this prefix route 
     * we should get it a unique identifier each time we generate it.
     * also set the hold time
     *
     * if we are propagating this prefix route, propagate the unique id
     */
    rdb = dest->rdb[0];

    if (!rdb || !ptr)
	return (0);

    if (rdb->validation_time == 0) {
	GET_TIMESTAMP32(time);
 	PUTLONG(ptr, time);
 	ptr += sizeof(ulong);
 	PUTSHORT(ptr, pdb->invalidtime/ONESEC);
 	ptr += sizeof(ushort);
    } else {
	PUTLONG(ptr, rdb->validation_time);
 	ptr += sizeof(ulong);
   	if (TIMER_RUNNING_AND_SLEEPING(rdb->expiration_time))
  	    PUTSHORT(ptr, pdb->invalidtime/ONESEC);
   	else
  	    PUTSHORT(ptr, 0);
 	ptr += sizeof(ushort);
    }

    /*
     * Check to see if next hop is this system. If so, this route
     * is in the prefix table for redistribution. Set metrics to
     * zeroes.
     */
    ddb = NULL;
    if (rdb)
	ddb = nsap_lookup(rdb->first_hop, MATCH_EXACT);
    if (ddb && ((ddb->entry_type == CLNS_OURNET_ENTRY) ||
		(ddb->entry_type == CLNS_OURNSAP_ENTRY))) {
	clns_setupmetric(pdb, (iso_igrp_metrictype *)ptr, ddb, rdb, FALSE);
	ptr += ISO_IGRP_METRIC_HOPCOUNT_LENGTH;
    } else {
	clns_setupmetric(pdb, (iso_igrp_metrictype *)ptr, dest, rdb,
                         poison_prefix_routes);
	ptr += ISO_IGRP_METRIC_HOPCOUNT_LENGTH;
    }

    /*
     * Copy destination address prefix and originating source NET
     * into packet.
     */
    bcopy(dest->destination, ptr, dest->destination[0]+1);
    ptr += (dest->destination[0] + 1);

    /*
     * IS-IS routes don't have source NET stored in entry. Use
     * NET of pdb doing the redistribution.
     */
    if (rdb->protocol_source & ISO_ISISMASK) {
	bcopy(pdb->net, ptr, pdb->net[0] + 1);
	ptr += (pdb->net[0] + 1);
    } else {
	bcopy(rdb->info_source, ptr, rdb->info_source[0] + 1);
	ptr += (rdb->info_source[0] +1);
    }

    return(ptr - start_ptr);
}


/*
 * clns_setupmetric
 *
 *    This routine is called when we are sending an update,
 *    It builds the metric at *ptr.  It works for dynamic, static and
 *    unreachable routes. If the flag "poison_prefix_routes" is TRUE, 
 *    advertise this route with a metric of infinity. This flag applies to
 *    perfix routes ONLY.
 *
 */    
void clns_setupmetric (
    clns_pdbtype        *pdb,
    iso_igrp_metrictype *ptr,
    clns_ddbtype        *dest,
    clns_rdbtype        *rdb,
    boolean             poison_prefix_routes)
{
    ulong         temp;
    route_maptype *map;

    /*
     * Always advertise ourselves with metric zero.
     * This handles advertising our own reachability and 
     * redistribution of IGRP into IGRP
     */
    if ((dest->entry_type == CLNS_OURNET_ENTRY) ||
	(dest->entry_type == CLNS_OURNSAP_ENTRY)) {
	temp = 0;
	bcopy((uchar *)&temp, ptr->delay, 3);
	bcopy((uchar *)&temp, ptr->bandwidth, 3);
	PUTSHORT(ptr->mtu, 0);
	ptr->reliability = 0;
	ptr->load = 0;
	ptr->hopcount = 0;
	return;
    }

    if (rdb && (dest->metric != CLNS_METRIC_INACCESSIBLE) && 
	!poison_prefix_routes) {

	/*
	 * Check if route-map is present, use IGRP configured metric
	 * for redistribution, if present.
	 */
	map = pdb->route_map;
	if (map) {
	    if (!(map->set_list.set_flags & ROUTE_MAP_METRIC_SET) ||
		!map->set_list.metric.igrp_metric) {
		map = NULL;
	    }
	}

	/*
	 * Redistribution of IS-IS --> IGRP and all static routes
	 * should be advertised with metric 0.
	 */
	if (rdb->protocol_source & (ISO_ISISMASK | ISO_STATIC)) {
	    if (!map) {
		temp = 0;
		bcopy((uchar *)&temp, ptr->delay, 3);
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		PUTSHORT(ptr->mtu, 0);
		ptr->reliability = 0;
		ptr->load = 0;
		ptr->hopcount = 0;
	    } else {
		temp = (map->set_list.metric.value.vector.delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = (map->set_list.metric.value.vector.bandwidth << 8);
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = map->set_list.metric.value.vector.loading;
		ptr->reliability = 
		    map->set_list.metric.value.vector.reliability;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu, map->set_list.metric.value.vector.mtu);
	    }
	    return;
	}

	/*
	 * Use metric from routing table if next-hop interface is up
	 */
	if (rdb->idb && interface_up(rdb->idb)) {
	    temp = (!map) ? (rdb->delay << 8) :
		(map->set_list.metric.value.vector.delay << 8);
	    bcopy((uchar *)&temp, ptr->delay, 3);
	    temp = (!map) ? (rdb->bandwidth << 8) :
		(map->set_list.metric.value.vector.bandwidth << 8);
	    bcopy((uchar *)&temp, ptr->bandwidth, 3);
	    if (!map) {
		PUTSHORT(ptr->mtu, rdb->mtu);
	    } else {
		PUTSHORT(ptr->mtu, map->set_list.metric.value.vector.mtu);
	    }
	    ptr->reliability = (!map) ? rdb->reliability :
		map->set_list.metric.value.vector.reliability;
	    ptr->load = (!map) ? rdb->load :
		map->set_list.metric.value.vector.loading;
	    ptr->hopcount = rdb->hopcount + 1;
	    return;
	}
    }

    /* 
     * if no other case worked, use an infinite metric
     */
    temp = (unsigned) -1;
    bcopy((uchar *)&temp, ptr->delay, 3);
    temp = 0;
    bcopy((uchar *)&temp, ptr->bandwidth, 3);
    PUTSHORT(ptr->mtu, 0);
    ptr->reliability = 0;
    ptr->load = 0;
    ptr->hopcount = 0;
}

/*
 * iso_igrp_metric
 *
 *    This routine is called when a packet comes in. It builds
 *    a composite metric from the values in the metric field
 *    which is pointed to by ptr. It adds in the appropriate
 *    information from the 'interface' if it is non-null.
 *
 *    NOTE that this routine should use the same algorithm
 *    as iso_igrp_route_update which computes the new metric
 *    to advertise when a route update is received.
 *
 */
ulong iso_igrp_metric (
    iso_igrp_metrictype *ptr,
    idbtype *interface,
    clns_pdbtype *pdb)
{
    ulong delay, bandwidth, metric;
    uchar reliability, load;

    bcopy((uchar *)ptr->delay, (uchar *)&delay, 3);
    delay >>= 8;
    if (delay >= CLNS_METRIC_INACCESSIBLE) 
	return(CLNS_METRIC_INACCESSIBLE);
    bcopy((uchar *)ptr->bandwidth, (uchar *)&bandwidth, 3);
    bandwidth >>=8;
    
    if (interface != NULL) {
	delay = delay + interface->delay;
	bandwidth = max(bandwidth,
			SCALED_BANDWIDTH(interface->visible_bandwidth));
	if (ptr->reliability)
	    reliability = min(ptr->reliability,interface->hwptr->reliability);
	else
	    reliability = interface->hwptr->reliability;
        if (ptr->load) 
            load = max(ptr->load, interface->hwptr->load);
        else 
            load = interface->hwptr->load;
    } else {
	reliability = ptr->reliability;
	load = ptr->load;
    }
    

    metric = 0;
    if (pdb->igrp_k1)
	metric += bandwidth * pdb->igrp_k1;
    if (pdb->igrp_k2)
	metric += bandwidth * pdb->igrp_k2 / (256 - load);
    if (pdb->igrp_k3)
	metric += delay * pdb->igrp_k3;
    if (pdb->igrp_k5) {
	metric *= pdb->igrp_k5;
	metric /= (reliability + pdb->igrp_k4);
    }
    return(metric);
}

/*
 * iso_igrp_route_update
 *
 *    This routine is called when new routing information is
 *    discovered in a routing update.
 *    The new information will be extracted from the packet
 *    and stored in the route descriptor block
 *
 *    NOTE that this is where the metrics are modified for
 *    the 'next hop'.
 */
void iso_igrp_route_update (
    clns_rdbtype *rdb,
    iso_igrp_metrictype *ptr,
    idbtype *interface)
{
    ulong temp;

    if (!rdb)
	return;
    bcopy((uchar *)ptr->delay, (uchar *)&temp, 3);
    temp >>= 8;
    rdb->delay = temp + interface->delay;
    bcopy((uchar *)ptr->bandwidth, (uchar *)&temp, 3);
    temp >>= 8;
    rdb->bandwidth = max(temp, SCALED_BANDWIDTH(interface->visible_bandwidth));
    temp = GETSHORT(ptr->mtu);
    rdb->mtu = (ushort)(min(temp, interface->clns_mtu));

    if (ptr->reliability)
        rdb->reliability = min(interface->hwptr->reliability,ptr->reliability);
    else 
	rdb->reliability = interface->hwptr->reliability;

    if (ptr->load)
        rdb->load = max(ptr->load, interface->hwptr->load);
    else
        rdb->load = interface->hwptr->load;

    rdb->hopcount = ptr->hopcount + 1;

    /* ??? is ip igrp doubling the hopcount ??? */
}

/*
 * dest_create
 *
 *    This routine will create a new station descriptor and
 *    link it into the appropropriate known destination array
 */
clns_ddbtype *dest_create (clns_pdbtype *pdb, NSAP_T id)
{
    clns_ddbtype *dest;
    int bucket;

    bucket = nsap_hash(id[0], &id[1]);
    dest = malloc(sizeof (clns_ddbtype));
    if (!dest)
	return(NULL);
				     
    nsap_copy(id, dest->destination);
    dest->next = pdb->dest_routing_table[bucket];
    pdb->dest_routing_table[bucket] = dest;

    return(dest);
}

/*
 * iso_igrp_holddown
 *
 */
boolean iso_igrp_holddown (ulong old_metric, ulong new_metric)
{

    /* 
     * if the route is more than 10% worse than our current best,
     * go into holddown
     */
    return((old_metric + old_metric / 10) < new_metric ? TRUE : FALSE);
}

/*
 * iso_igrp_route_delete
 *
 *    This routine will delete the specified route.
 *    The cache is always invalidated.
 *    If  there are no more routes to the specified destination,
 *    put it into hold-down.
 *
 *    if this route is actually a neighbor entry, remove all of
 *    the routes which we learned from this neighbor
 */
void iso_igrp_route_delete (
    clns_pdbtype *pdb,
    clns_ddbtype *dest,
    short index)
{
    int i;

    clns_invalidate_cache("iso_igrp_route_delete", FALSE);

    if (dest->routecount > 1) {
	clns_delete_rdb(dest,index);
	dest->metric = CLNS_METRIC_INACCESSIBLE;
	for (i = 0; i < dest->routecount; i++)
	    if (dest->rdb[i]->metric < dest->metric)
		dest->metric = dest->rdb[i]->metric;
    } else if (dest->routecount == 1) {
	clns_rbuginf("\nISO-IGRP-RT: %n entering holddown",
		     dest->destination);
	TIMER_START(dest->blindtimer, pdb->blindtime);
	dest->metric = CLNS_METRIC_INACCESSIBLE;
	pdb->changed++;
	GET_TIMESTAMP(pdb->whenchanged);
    }
}

/*
 * iso_pdbBLOCK
 *
 *    ??? This routine may not belong in this file
 * Return TRUE if the specified process should stay blocked
 * Return FALSE if it should run
 *
 */
boolean iso_pdbBLOCK (ulong foo)
{
    clns_pdbtype *pdb;

    pdb = (clns_pdbtype *)foo;
    if (!clns_pdb_running(pdb))
	return(TRUE);
    if (pdb->clns_inputq.qhead)
	return(FALSE);           /* input available */
    if (mgd_timer_expired(&pdb->timer))
	return(FALSE);           /* timer expired */
    if (!SLEEPING(pdb->hello_timer)) /* time to send another router hello */
	return(FALSE);
    if (pdb->changed)
	return(FALSE);           /* topology changed */
    return(TRUE);
}

/*
 * clns_build_prefix_routes
 *
 * This routine adds the appropriate prefix routes to the routing update
 * packet. It checks that the data in the packet does not get larger than
 * 'nbytes'.
 *
 * This routine will return when it has either filled the packet
 * or advertised all of the prefix routes. It returns true if has
 * advertised all of the routes.
 *
 * The argument 'poison_prefix_routes' is TRUE if we need to poison the
 * prefix routes that were being redistributed from 'rpdb'.
 */
boolean clns_build_prefix_routes (
    clns_pdbtype *pdb,
    idbtype *interface,
    iso_igrp_info_type **ptr,
    int *nbytes,
    clns_ddbtype **dest,
    boolean poison_prefix_routes,
    clns_pdbtype *rpdb,
    clns_ddbtype *specific_prefix)
{
    clns_rdbtype         *rdb;
    char                 *prefixptr;
    int                  length;
    boolean              should_poison;

    /*
     * Set up code and final length.
     */
    (*ptr)->code = ISO_IGRP_PREFIXROUTE;
    (*ptr)->length = 2;

    if ((*nbytes -= 2) < sizeof(iso_igrp_prefixroute)){
	return(FALSE);
    }

    prefixptr = (*ptr)->value;

    rdb = 0;

    if (specific_prefix) 
      *dest = specific_prefix;
    else
      *dest = getnextprefix(*dest, interface);

    for ( ; *dest; *dest = getnextprefix(*dest, interface)) {
	rdb = (*dest)->rdb[0];
	if (!rdb) {
	  if (specific_prefix)
	    return(TRUE);
	  else
	    continue;
	}
	if (pdb->redistallowed & rdb->protocol_source) {
	    if (!clns_pdb_routemap(pdb, *dest)) continue;

	    /* 
	     * Special case: if we are sourcing this prefix route
	     * and it matches the net for this pdb - don't
	     * advertise it. (This is effectively split-horizon).
	     * This prevents us from advertising the 'special domain prefix'
	     * inside its own domain.
	     */
	    if ((rdb->validation_time != 0) || 
		!clns_domain_match((*dest)->destination[0], 
				   &(*dest)->destination[1],
				   pdb->domain_length, pdb->domain)) {

		/*
		 * If we're poisoning prefix routes, make sure that we
		 * poison only those prefix routes that were being
		 * redistributed from 'rpdb'. If we determine that this
		 * particular prefix route should be poisoned, we
		 * set the flag "should_poison" to TRUE, so that
		 * the function "clns_setupprefix" will send a metric of
		 * infinity for this particular prefix route.
		 *
		 * Also if the prefix route has no outgoing interfaces
		 * that are UP, advertise it with an infinite metric.
		 */
		if (poison_prefix_routes && rpdb &&
		   (rdb->protocol_source == rpdb->typeflag))
		  should_poison = TRUE;
		else if (poison_prefix_routes && specific_prefix)
		  should_poison = TRUE;
		else if (!clns_prefix_has_idb_up(*dest))
		  should_poison = TRUE;
		else
		  should_poison = FALSE;
		length = clns_setupprefix(pdb, prefixptr, *dest,
					  should_poison);

		if (length) {
		    clns_ibuginf("\nISO-IGRP: Advertise prefix: %s, metric %d",
				 (!(*dest)->destination[0]) ? "Default" :
				 nsap_string((*dest)->destination),
				 (should_poison) ? CLNS_METRIC_INACCESSIBLE : 
				                   (*dest)->metric);
		}

		(*ptr)->length += length;
		prefixptr += length;
		*nbytes -= length;
		if (*nbytes < sizeof (iso_igrp_prefixroute)) {
		    *ptr += (*ptr)->length;
		    (*ptr)->code = ISO_IGRP_PREFIXROUTE;
		    (*ptr)->length = 2; /*for backwards compatibility*/
		    return(FALSE);
		}
		
		/*
		 * If we're looking at only a specific prefix, we are
		 * done.
		 */
		if (specific_prefix)
		  break;
	    }
	}
    }

    /*
     * Set new poistion of packet pointer.	
     */
    *ptr += (*ptr)->length;

    return(TRUE);
}

/*
 * clns_pad_net
 *
 *    Utility routine. The 'net' field in the packet headers is always
 *    the maximum length (this makes parsing easier). When we fill it
 *    in, pad the necessary trailing bytes.
 */
void clns_pad_net (NSAP_T nsap)
{
    int i;
    
    for (i = nsap[0]; i < CLNS_MAX_NSAP_LENGTH-1; i++)
	nsap[i+1] = 0;
}

/*
 * clns_flash_update
 *
 *    For each pdb that redistributes 'type_flag', set pdb->changed in
 *    order to force a flash update.
 */
void clns_flash_update (ulong type_flag)
{
    clns_pdbtype *pdb;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next){
	if (pdb->redistallowed & type_flag)
	    pdb->changed ++;
	}
}

/*
 * clns_add_prefixroute
 *
 *    Add a new prefix route learned from a routing update
 */
void clns_add_prefixroute (
    iso_igrp_prefixroute *prefixroute,
    idbtype *idb,
    NSAP_T first_hop,
    clns_pdbtype *pdb,
    hwaddrtype *snpa)
{
    clns_rdbtype *rdb;
    NSAP_T *source_NET;
    sys_timestamp expiration_time;
    ulong metric;
    clns_pdbtype *temppdb;

    metric = iso_igrp_metric(&prefixroute->metric, idb, pdb);
    if (metric >= CLNS_METRIC_INACCESSIBLE)
	return;

    source_NET = &prefixroute->source_NET;

    /*
     * never accept an adv that you were the original source for
     */
    for (temppdb = clns_protoQ.qhead; temppdb; temppdb = temppdb->next) {
	if (nsap_equ(*source_NET, temppdb->net))
	    return;
    }

    TIMER_START(expiration_time,
		GETSHORT(prefixroute->validation.hold_time) * ONESEC);
    rdb = clns_route_update(prefixroute->metric.id, first_hop, idb,
			    CLNS_ROUTE_ENTRY, pdb->typeflag,
			    pdb->distance, 
			    GETLONG(&prefixroute->validation.time),
			    expiration_time, metric,
			    *source_NET);

    /* metric   ??? probably should do metric inside previous routine */
    iso_igrp_route_update(rdb, &prefixroute->metric, idb);
}

/* 
 * clns_update_prefixroute
 *
 * This function decides if an existing route should be updated with new
 * next-hop and metric information from a received prefix route update.
 *
 * This function also decides if the change requires a flash update to
 * be sent.
 *
 * This function only applies to prefix routes.
 */
#define CLNS_TIMER_LATER(ts1,ts2) (((long)ts2 - (long)ts1) < 0)

void clns_update_prefixroute (
    clns_ddbtype         *dest,
    iso_igrp_prefixroute *prefixroute,
    clns_pdbtype         *pdb,
    idbtype              *idb,
    NSAP_T               first_hop,
    hwaddrtype           *snpa)
{
    NSAP_T       *source_NET;
    clns_pdbtype *temppdb;
    clns_rdbtype *rdb;
    ulong        new_metric, old_metric;
    boolean      same_next_hop, overwrite_static_route;
    
    /*
     * Note - today we only support a single rdb for each ddb for
     * prefix routes
     */
    rdb = dest->rdb[0];
    if (!rdb) {
	return;
    }

    /*
     * We will overwrite a static route if all of its outgoing interfaces
     * are DOWN.
     */
    overwrite_static_route = (rdb->protocol_source == ISO_STATIC) &&
                             (dest->distance == ISO_STATIC_DIST) &&
                             (!clns_prefix_has_idb_up(dest));

    /*
     * If we have a static prefix route - never replace it with a
     * dynamic prefix route. If, however, this static route is down,
     * then go ahead and update with a dynamic prefix route.
     */
    if ((rdb->protocol_source == ISO_STATIC) && !overwrite_static_route) 
      return;

    /*
     * Original source of prefix route. This is the router that is
     * directly connected to the domain.
     */
    source_NET = &prefixroute->source_NET;

    /*
     * Never accept an advertisement that you were the original source for.
     * Never override one of your own sourced routes with a learned one.
     */
    for (temppdb = clns_protoQ.qhead; temppdb; temppdb = temppdb->next) {
	if (nsap_equ(*source_NET, temppdb->net) ||
	         (!overwrite_static_route && 
	         net_equ(rdb->info_source, temppdb->net))) {
	    clns_ibuginf ("\nISO-IGRP: Received our own route");
	    return;
	}
    }

    /*
     * If this is stale information, don't refresh it.
     */
    if (net_equ(rdb->info_source, *source_NET) &&
	!CLNS_TIMER_LATER(prefixroute->validation.time,
		     rdb->validation_time)) {
	if (dest->metric == CLNS_METRIC_INACCESSIBLE) {
	    clns_ibuginf 
		("\nISO-IGRP: Received stale info for inaccessible route");
	    return;
	}
    }

    /* 
     * Build the composite metric that was advertised. Determine if the
     * advertisement is from the next-hop that we are currently using.
     */
    new_metric = iso_igrp_metric(&prefixroute->metric, idb, pdb);
    clns_ibuginf ("\nISO-IGRP: Routing table metric = %d", dest->metric);

    same_next_hop = (nsap_equ(first_hop, rdb->first_hop) && rdb->idb == idb);

    /*
     * We have just received an unreachability adv. If it is not from
     * our current next_hop, ignore it.
     */
    if (new_metric == CLNS_METRIC_INACCESSIBLE && !same_next_hop) return;
	    
    /*
     * We have received a metric that is worse than our current
     * metric. If it is not from the current source, ignore it.
     */
    if ((new_metric > dest->metric) && 
	!same_next_hop && !overwrite_static_route) 
        return;

    /*
     * If advertised metric is same as routing table metric, we have a new
     * next-hop that is equal-cost.
     */
    if (new_metric == dest->metric && !same_next_hop) {
	clns_add_prefixroute(prefixroute, idb, first_hop, pdb, snpa);
	return;
    }

    /*
     * At this point we know that either: 
     *
     * 1) The advertised metric is better than the metric in the routing table.
     * 2) The advertised metric from our current next hop has gotten worse.
     * 3) The advertised metric from our current next hop is the same as the
     *    routing table metric, so we update route timers.
     */

    /*
     * If metric changed from good to inaccessible or vice-versa,
     * delete/add prefix neighbor if redistributing into IS-IS.
     */
    if (new_metric == CLNS_METRIC_INACCESSIBLE && 
	dest->metric < CLNS_METRIC_INACCESSIBLE) {
	clns_manage_prefix_nbrs(dest, rdb->protocol_source, FALSE);
	TIMER_START(dest->blindtimer, pdb->blindtime);
    } else if (new_metric < CLNS_METRIC_INACCESSIBLE && 
	       dest->metric == CLNS_METRIC_INACCESSIBLE) {
	clns_manage_prefix_nbrs(dest, rdb->protocol_source, TRUE);
	TIMER_STOP(dest->blindtimer);	/* Make it look "up". */
    }

    /*
     * If current next-hop is advertising infinity and routing table entry
     * has infinity metric, don't consider flash updating.
     */
    if (new_metric == CLNS_METRIC_INACCESSIBLE &&
	dest->metric == CLNS_METRIC_INACCESSIBLE) return;

    /*
     * Update routing table with new information from update packet.
     */
    iso_igrp_route_update(rdb, &prefixroute->metric, idb);
    rdb->protocol_source = pdb->typeflag;
    old_metric = dest->metric;
    dest->metric = new_metric;
    nsap_copy(*source_NET, rdb->info_source);
    if (!same_next_hop || rdb->idb != idb) {
	nsap_copy(first_hop, rdb->first_hop);
	clns_invalidate_cache("clns_update_prefixroute", FALSE);
    }
    rdb->idb = idb;

    rdb->validation_time = GETLONG(&prefixroute->validation.time);
    TIMER_START(rdb->expiration_time,
		GETSHORT(prefixroute->validation.hold_time)*ONESEC);

    /* 
     * If the new metric is 10% better than the old metric or the 
     * new metric is unreachable - cause a flash update for all pdbs that 
     * redistribute this domain.
     */
    if ((new_metric < (old_metric - old_metric / 10)) ||
	new_metric == CLNS_METRIC_INACCESSIBLE) {
	clns_flash_update(pdb->typeflag);
    }
}

/*
 * iso_igrp_setup_metric
 * 
 * Store interface metric information in ISO-IGRP rdb.
 */
void iso_igrp_setup_metric (clns_rdbtype *rdb, idbtype *idb)
{
    if (!rdb)
	return;

    rdb->delay = idb->delay;
    rdb->bandwidth = SCALED_BANDWIDTH(idb->visible_bandwidth);
    rdb->mtu = idb->clns_mtu;
    rdb->reliability = idb->hwptr->reliability;
    rdb->load = idb->hwptr->load;
    rdb->hopcount = 1;
}
