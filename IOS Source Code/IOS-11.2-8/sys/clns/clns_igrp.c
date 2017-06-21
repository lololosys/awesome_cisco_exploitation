/* $Id: clns_igrp.c,v 3.5.12.6 1996/08/08 14:50:18 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_igrp.c,v $
 *------------------------------------------------------------------
 * clns_igrp.c -- Interior Gateway Routing Protocol for CLNS packets
 *
 * 5-December-1989, Paulina Knibbe
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_igrp.c,v $
 * Revision 3.5.12.6  1996/08/08  14:50:18  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.5.12.5  1996/07/11  18:29:16  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/06/18  18:55:54  asastry
 * CSCdi58806:  DEC "advertised" route does not work correctly if ISO-IGRP
 * is used.
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/05/04  20:03:55  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/04/06  00:35:55  asastry
 * CSCdi53023:  ISO-IGRP should poison redistributed routes if the
 * redistribution is disabled.
 *
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/03/18  19:02:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/13  01:12:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.2  1996/03/07  08:39:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  00:20:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  08:11:10  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1996/01/25  22:18:55  rchandra
 * CSCdi44688:  ISO-IGRP: 4k based images cannot interoperate with 68k
 * images
 * - do not super-impose structure on the byte stream while generating and
 *   parsing update
 *
 * Revision 3.3  1996/01/18  21:48:04  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  08:52:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/02  19:21:17  rchandra
 * CSCdi39191:  Memory leak in ISO-IGRP Router
 * - Before freeing the clns_ddb, clean the rdbs.
 *
 * Revision 2.4  1995/08/08  16:42:45  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.3  1995/06/18 19:39:37  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:58:55  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:17:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This module is an implementation of the cisco proprietary IGRP routing
 * protocol for ISO (CLNS) networks.
 * 
 * The basic algorithm is a modified Ford-Fulkerson algorithm employing
 * split horizon partitioning and hold-down to avoid routing loops.
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_adj.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../iprouting/route.h"
#include "../iprouting/igrp.h"


/*
 * clns_igrp_pdb_init
 * Initialize an clns_igrp Protocol Descriptor Block
 *     There will be one of these for each area this router is in
 *     and one for each domain this router is in
 *
 * Note that the pdb is created at the first reference to the pdb.
 * In some cases this is the 'router' command. In some places it
 * is the interface specific command. If theh pdb is created with
 * the interface specific command, we need to call this routine
 * again later to fill in the other fields (such as 'net').
 */
clns_pdbtype *clns_igrp_pdb_init (
    NSAP_T nsap,
    char *router_name,
    int level,
    boolean dynamic)
{
    clns_pdbtype *pdb = NULL;
    ulong flag, mask, index;
    boolean new_pdb;
    uint L1_pdb_count;
    /*
     * Allocate an unused router process flag
     */
    if (!dynamic)
	flag = ISO_STATIC;
    else {
	flag = ISO_IGRPMASK;
        L1_pdb_count = 0;
	for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    if (!strcasecmp(pdb->pname, router_name)) {
		if (!(pdb->typeflag & ISO_IGRPMASK)) {
		    printf("\nDuplicate router tag");
		    return (NULL);
		}
		flag = pdb->typeflag;
		break;
	    }
	    flag &= ~pdb->typeflag;
            if (pdb->level & CLNS_LEVEL1)
		++L1_pdb_count;
	}

	if ((L1_pdb_count > MAX_ISO_IGRP_PROCESSES) || (flag == 0)) {
	    printf("\nISO-IGRP: too many ISO IGRP routing processes");
	    return(NULL);
	}
    }

    if (flag & ISO_STATIC) 
	index = 0;
    else {
        mask = ISO_IGRP0;
        index = CLNS_PDBINDEX_IGRP0;
        while (mask)
        { 
	    if (flag & mask) {
	        flag &= mask;
	        break;
	    }

	    index++;
	    mask = mask << 1;
        }
    }

    /*
     * Now that a flag has been found, create the pdb.
     */
    new_pdb = FALSE;
    if (!pdb){
	pdb = malloc(sizeof(clns_pdbtype));
	new_pdb = TRUE;
    }
    if (pdb == NULL) return(NULL);
    if (dynamic){
	pdb->name = "iso-igrp";
    } else {
	pdb->name = "static";
    }
    if (!pdb->pname) setstring(&pdb->pname, router_name);
    queue_init(&pdb->clns_inputq,0);

    pdb->clns_pid = NO_PROCESS;
    pdb->level = level;
    pdb->family = PDB_OSI;
    pdb->typeflag = flag;
    pdb->redistallowed = flag;
    pdb->distance = ISO_IGRP_DIST;
    pdb->index = index;
    pdb->broadcasttime = pdb->def_broadcasttime = ISO_IGRP_CYCLETIME;
    pdb->invalidtime = pdb->def_invalidtime = ISO_IGRP_ROUTEFLUSHTIME;
    pdb->blindtime = pdb->def_blindtime = ISO_IGRP_BLINDTIME;
    pdb->hellotime = pdb->def_hellotime = ISO_IGRP_HELLOTIME;

    pdb->igrp_k1 = pdb->igrp_k1_def = ISO_IGRP_K1_DEFAULT;
    pdb->igrp_k2 = pdb->igrp_k2_def = ISO_IGRP_K2_DEFAULT;
    pdb->igrp_k3 = pdb->igrp_k3_def = ISO_IGRP_K3_DEFAULT;
    pdb->igrp_k4 = pdb->igrp_k4_def = ISO_IGRP_K4_DEFAULT;
    pdb->igrp_k5 = pdb->igrp_k5_def = ISO_IGRP_K5_DEFAULT;

    if (nsap[0]){
	nsap_copy(nsap, pdb->net);

	/*
	 * For now the domain part always starts at the beginning of the nsap
	 */
	pdb->domain = &pdb->net[1];
	pdb->domain_length = pdb->net[0] - AREA_LENGTH - STATION_LENGTH - 1;
    } else 
	pdb->domain_length = 0;

    /*
     * it used to matter that all of the level 1 pdbs came before all of
     * the level 2 pdbs. That is no longer important.
     */
    if (new_pdb)
	enqueue(&clns_protoQ, pdb);
    clns_set_pdb_running(pdb, (nsap[0] != 0));
    return(pdb);
}

/*
 * clns_igrp_pdb_initialize
 *
 * Called from router_parse. Used to initialize the ISO-IGRP processes
 * the new way. This function should become clns_igrp_pdb_init, once the older
 * commands are deprecated.
 */
clns_pdbtype *clns_igrp_pdb_initialize (char *router_name)
{
    clns_pdbtype *pdb;
    NSAP_T       nsap;

    pdb = NULL;
    nsap[0] = 0;

    if (clns_add_router(router_name, nsap, TRUE, ISO_IGRPMASK)) {
	pdb = clns_find_router_name(router_name);
    }
    return(pdb);
}

/* 
 * iso_igrp_destination_update
 *
 *    This routine is called to incorporate routing information
 *    about a particular destination. It adds destinations and
 *    puts destinations into hold-down. Destinations are only
 *    deleted in ???
 *
 */ 
clns_rdbtype *iso_igrp_destination_update (
    clns_pdbtype *pdb,
    ushort type,		/* level 1 or level 2 */
    NSAP_T id,
    idbtype *interface,
    NSAP_T gateway,		/* first hop gateway */
    hwaddrtype *snpa,
    ulong metric,
    ulong flags,
    int rib_type,		/* CLNS_*_ENTRY */
    ulong expiration_interval)
{
    clns_ddbtype *dest;
    int rdbindex, i;
    clns_rdbtype *rdb;

    /*
     * For simplex circuits we may need to map a receive-only interface
     * to its transmit-only counterpart.
     */
    if (interface)
	interface = interface->output_if;

    if (metric > CLNS_METRIC_INACCESSIBLE)
	metric = CLNS_METRIC_INACCESSIBLE;

    /* first find destination descriptor. Create it if necessary */
    dest = clns_find_dest(pdb, id);

    if (!dest) {
	/* Note that the following check for interface_up is NOT made when
	 * we add a route to an existing destination. It probably should be
	 */
	if (interface)
	    if ((!interface_up (interface)) && (flags != ISO_STATIC))
		return (NULL);
	if (metric >= CLNS_METRIC_INACCESSIBLE)
	    return (NULL);
	dest = dest_create(pdb, id);
	if (!dest)
	    return (NULL);

	dest->type = type;
	dest->metric = metric;
	for (i = 0; i < MAXROUTES; i++)
	    dest->rdb[i] = 0;
	dest->flags = 0;
	TIMER_STOP(dest->blindtimer);
	dest->routecount = 0;
    }

    /*
     * if this dest is ourselves, don't add a route
     *
     * never replace a static route with a dynamically learned route
     * (Later we should remember both and use the better route)
     *
     * otherwise, look for a previous rdb from this source.
     */
    if ((dest->entry_type == CLNS_OURNSAP_ENTRY) ||
	(dest->entry_type == CLNS_OURNET_ENTRY))
	return(NULL);

    rdbindex = -1;
    rdb = NULL;
    for (i = 0; i < dest->routecount; i++) {
	rdb = dest->rdb[i];
	if (rdb->protocol_source == ISO_STATIC) {
	    return(NULL);
	}
	if (nsap_equ(rdb->first_hop, gateway) && rdb->idb == interface) {
	    /*
	     * if the existing route was learned via igrp from the
	     * same gateway, don't replace it was a route learned via ESIS
	     */
	    if ((rdb->protocol_source & ISO_IGRPMASK) &&
		(flags == ISO_ESIS)){
		return(NULL);
	    }
	    rdbindex = i;
	    break;
	}
    }
    if (rdbindex == -1) 
	rdb = NULL;

    /*
     * If the destination is in hold-down, don't beleive
     * the new route unless from our current neighbor.
     */
    if (TIMER_RUNNING(dest->blindtimer) && !rdb) return(NULL);

    /* 
     * if the metric has gotten better, use the new route
     */
    if (metric < dest->metric) {
	clns_delete_all_rdbs(dest);
	clns_rbuginf("\nISO-IGRP-RT: using new route for: %n",
		      dest->destination);
	rdb = NULL;
	clns_invalidate_cache("iso_igrp_destination_update", FALSE);
	dest->metric = metric;
    } else {
	if (metric > dest->metric) {
	    if (!rdb || rdb->idb != interface){
		/* 
		 * The source of this route is different from the source
		 * of our current route. Discard the new route
		 */
		return(NULL);
	    } else {
		/*
		 * The source of our current route is telling us the
		 * route got worse. Maybe go into hold-down.
		 */
		if (iso_igrp_holddown(dest->metric, metric)){
		    clns_rbuginf ("\nISO-IGRP-RT: entering holddown:");
		    if (dest->type == CLNS_STATION_TYPE)
			clns_rbuginf("%s", station_string(&id [1]));
		    else
			clns_rbuginf("%s", area_string(&id [1]));
		    iso_igrp_route_delete(pdb, dest, rdbindex);
		    return(NULL);
		} else {
		    /*
		     * we are going to use this route anyways
		     * update the metric 
		     */
		    dest->metric = metric;

		    if (metric == CLNS_METRIC_INACCESSIBLE) {
			iso_igrp_route_delete (pdb, dest, rdbindex);
			return(NULL);
		    }
		}
	    }
	}
    }
    /* 
     * we've finished mucking with the various duplicate routes
     * now create a new rdb if necessary
     */
    if (rdb == NULL) {
	if (dest->routecount < MAXROUTES) {
	    if (metric >= CLNS_METRIC_INACCESSIBLE)
		return (NULL);
	    rdb = malloc(sizeof(clns_rdbtype));
	    if (!rdb) {
		if (dest->routecount == 0)
		    dest->metric = CLNS_METRIC_INACCESSIBLE;
		return(NULL);
	    }
	    dest->rdb[dest->routecount] = rdb;
	    dest->routecount += 1;
	    if (dest->routecount == 1){
		pdb->changed++;
		GET_TIMESTAMP(pdb->whenchanged);
	    }
	} else 
	    return (NULL);
    }

    dest->flags |= flags;
    dest->entry_type = rib_type;
    rdb->protocol_source = flags;
    rdb->idb = interface;

    if (gateway)
	nsap_copy(gateway, rdb->first_hop);

    if (flags == ISO_STATIC) {
	TIMER_STOP(rdb->expiration_time);
    } else if (metric && metric < CLNS_METRIC_INACCESSIBLE) {
	TIMER_START(rdb->expiration_time, expiration_interval);
	TIMER_STOP(dest->blindtimer);
    }
    return (rdb);
}

/*
 * iso_igrp_add_static_nbrs
 *
 * Add static neighbors to ISO-IGRP level-1 routing table. The neighbors
 * are obtained from the adjacency database.
 *
 * If idb != NULL, add static neighbors for the specified interface, otherwise
 * add for all interfaces.
 */
void iso_igrp_add_static_nbrs (clns_pdbtype *pdb, idbtype *idb)
{

    clns_adjtype *adj;
    clns_rdbtype *rdb;
    uchar        *ptr;
    NSAP_T       nsap;
    int          bucket;

    adj = NULL;
    while ((adj = next_neighbor(clns_adjacency_db, idb, 0, adj, &bucket))) {
	if (!(adj->protocol_source & ISO_STATIC)) continue;
	if (!itf_in_pdb (adj->idb, pdb)) continue;
	
	/*
	 * If static adjacency in same area as this pdb. Need to	
	 * to build NSAP from adjacency information.
	 */
	BUILD_NSAP(adj, nsap);
	if (clns_find_pdb (nsap) == pdb) {
	    if (pdb->level == CLNS_LEVEL1) {
		rdb = iso_igrp_destination_update(pdb, CLNS_STATION_TYPE, 
						  adj->key, adj->idb, 
						  adj->key, &adj->snpa, 
						  iso_igrp_compute_metric(pdb,
								  adj->idb), 
						  ISO_STATIC, 
						  CLNS_ROUTE_ENTRY, 0);
	    } else {
		ptr = get_area (nsap);
		nsap [0] = AREA_LENGTH;
		nsap [1] = *ptr++;
		nsap [2] = *ptr;
		rdb = iso_igrp_destination_update(pdb, CLNS_AREA_TYPE, 
						  nsap, adj->idb, adj->key, 
						  &adj->snpa, 
						  iso_igrp_compute_metric(pdb,
								  adj->idb), 
						  ISO_STATIC,
						  CLNS_ROUTE_ENTRY, 0);
	    }
	    iso_igrp_setup_metric(rdb, adj->idb);
	}
    }
}

/*
 * iso_igrp_sendhello
 *
 *    this routine sends a router hello on the specified interface.
 *    this hello will contain an entry for each area this router is
 *    in (in this domain). this hello is sent out each interface
 *    that is in this domain. (this routine should only be called
 *    by the level 2 iso igrp process).
 */
void iso_igrp_sendhello (
    clns_pdbtype *pdb,
    idbtype *interface,
    hwaddrtype *snpa)
{
    iso_igrp_info_type *item;
    iso_igrp_area_addr_type *area_addr;
    clns_pdbtype *pdb2;
    paktype *pak;
    iso_igrp_hello_hdrtype *iso_igrp;
    int nbytes;
    uint total_length;
    int area_pos;
    iso_igrp_param_type *params;

    /*
     * is the interface up ?
     */
    if (!interface_up(interface))
	return;

    /*
     * first check if this interface is in this domain.
     */
    if (!itf_in_pdb(interface, pdb) || !(interface_up(interface))
	                            || !(interface->clns_enabled)){
	return;
    }
    nbytes = interface->clns_mtu;
    pak = clns_getbuffer(MEDDATA);
    if (!pak)
	return;
    pak->flags |= PAK_PRIORITY;
    pak->if_output = interface;

    iso_igrp = (iso_igrp_hello_hdrtype *)iso_igrp_headstart(pak);

    iso_igrp->pid = NLP_ISO_IGRP;
    iso_igrp->hlen = ISO_IGRP_HELLO_HDRSIZE;
    iso_igrp->version = ISO_IGRP_VERSION;
    iso_igrp->reserved = 0;
    iso_igrp->opcode = ISO_IGRP_OPCODE_HELLO;
    iso_igrp->intf_type = ISO_IGRP_LEVEL1_LEVEL2;

    PUTSHORT(iso_igrp->holdingtime, ISO_IGRP_HELLO_DEF_HOLD_TIME/ONESEC);
	
    item = (iso_igrp_info_type *)iso_igrp->item;
    item->code = ISO_IGRP_AREALIST;
    item->length = 0;
    iso_igrp->source_net [0] = 0;

    area_addr = (iso_igrp_area_addr_type *)&item->value;
    for (pdb2 = clns_protoQ.qhead; pdb2; pdb2=pdb2->next){
	if ((clns_domain_match (pdb->domain_length, pdb->domain,
				pdb2->domain_length, pdb2->domain)) &&
	    (pdb2->level & CLNS_LEVEL1)){
	    if (!itf_in_pdb (interface, pdb2)) continue;

	    /*
	     * Source NET should be the one that the pdb uses for
	     * this interface. 
	     */
	    if (!iso_igrp->source_net [0]) {
		nsap_copy(*clns_get_local_npai(pdb2), iso_igrp->source_net);
		clns_pad_net(iso_igrp->source_net);
	    }

	    /*
	     * Build area addresses in hello packet.
	     */
	    area_pos = pdb2->net[0] - STATION_LENGTH - AREA_LENGTH;
	    build_nsap(pdb2->domain_length, pdb2->domain,
		       &pdb2->net[area_pos],
		       NULL, NULL,
		       ((NSAP_T *)area_addr));
	    area_addr = (iso_igrp_area_addr_type *)
		((uchar *)area_addr + area_addr->addr_length + 1);
	}
	item->length = (ulong)area_addr - (ulong)item;
    }

    /*
     * If no level-1 pdbs assigned to this interface, use info from level-2
     * pdb to build source NET and area address.
     */
    if (!iso_igrp->source_net [0]) {
	nsap_copy(*clns_get_local_npai(pdb), iso_igrp->source_net);
	clns_pad_net(iso_igrp->source_net);
	nsap_copy (pdb->net, *(NSAP_T *)area_addr);
	area_addr->addr_length -= (STATION_LENGTH + SELECTOR_LENGTH);
	area_addr = (iso_igrp_area_addr_type *)
	    ((char *)area_addr + area_addr->addr_length + 1);
	item->length = (ulong) area_addr - (ulong) item;
    }

    total_length = (ulong)area_addr - (ulong)iso_igrp_headstart(pak);

    item = (iso_igrp_info_type *)((char *)item + item->length);
    item->code = ISO_IGRP_PARAMS;
    item->length = sizeof(iso_igrp_param_type) + sizeof(iso_igrp_info_type);
    /* ??? - 2 */

    params = (iso_igrp_param_type *)&item->value;
    params->cycletime = pdb->broadcasttime / ONESEC;
    params->routeflushtime = pdb->invalidtime / ONESEC;
    params->blindtime = pdb->blindtime / ONESEC;
    params->hellotime = pdb->hellotime / ONESEC;
    params->k1 = pdb->igrp_k1;
    params->k2 = pdb->igrp_k2;
    params->k3 = pdb->igrp_k3;
    params->k4 = pdb->igrp_k4;
    params->k5 = pdb->igrp_k5;

    total_length += item->length;
    PUTSHORT(iso_igrp->len, total_length);
    /*
     * if the caller specified a particular destination, use it
     * otherwise send it to all iss
     */
    if (!snpa)
	snpa = &all_iss;

    if (clns_encaps(pak, total_length, interface, snpa, NULL)){
	clns_ibuginf("\nISO-IGRP: Hello sent on %s for %s",
		     interface->namestring, pdb->pname);
	clns_traffic[ISO_IGRP_H_OUT]++;
	datagram_out(pak);
    } else {
	datagram_done(pak);
	if (!is_x25(interface->hwptr)){
	    clns_ibuginf("\nISO-IGRP: %s encapsulation of router hello failed",
			 interface->namestring);
	}
    }
}


/*
 * iso_igrp_rcvhello
 *
 *    We have received an IGRP router hello.
 *
 *    For each area that is in the hello,
 *        if we are also in that area
 *             form/update a level 1 neighbor
 *        else
 *             form/update a level 2 neighbor
 *       
 */
static void iso_igrp_rcvhello (
    clns_pdbtype *pdb,
    paktype *pak)
{
    ulong holdingtime;

    iso_igrp_hello_hdrtype *iso_igrp;
    iso_igrp_info_type *item;
    iso_igrp_area_addr_type *area;
    uchar *station_id;
    int num_areas;
    hwaddrtype dest_snpa, source_snpa;
    boolean new_one;
    clns_pdbtype *pdb2;
    idbtype *idb;
    hwaddrtype snpa;
    clns_adjtype *adjacency;
    NSAP_T *area_from_packet, key;
    char *item_end;

    iso_igrp = (iso_igrp_hello_hdrtype *)iso_igrp_headstart(pak);

    new_one = FALSE;
    idb = pak->if_input;
    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);
    holdingtime = GETSHORT(iso_igrp->holdingtime);
    clns_ibuginf("\nISO-IGRP: Received hello from %n, (%s), ht %d",
		 iso_igrp->source_net, idb->namestring, holdingtime);

    /*
     * Filter the packet.
     */
    if (idb->iso_igrp_adj_filter) {
	if (!clns_filter_addr(idb->iso_igrp_adj_filter, 
			      (nsap_addr *) &iso_igrp->source_net,
			      NULL)) {
	    clns_ibuginf("\nISO-IGRP: filter rejected hello");
	    return;
	}
    }

    /* 
     * Do validity checking on length 
     * (NOTE that this is done BEFORE accepting any data from packet)
     */
    num_areas = 0;

    item = (iso_igrp_info_type *)iso_igrp->item;
    if (item->code == ISO_IGRP_AREALIST){
	area = (iso_igrp_area_addr_type *)item->value;
	while ((ulong)area < ((ulong)item + item->length)){
	    if (area->addr_length == 0){
		clns_traffic[CLNS_HDRSYNTAX]++;
		clns_ibuginf("\nISO_IGRP: Invalid header"
			     "(zero length area)");
		clns_ibuginf("\n          from %s:%s",
			     pak->if_input->namestring,
			     snpa_string(&snpa));
		return;
	    }
	    area = (iso_igrp_area_addr_type *)
		((char *)area + area->addr_length+1);
	num_areas += 1;
	}
    } else {
	/* 
	 * this is a code we don't understand yet.
	 */
	return;
    }
    /*
     * packet looks valid. now process it.
     */
    key[0] = STATION_LENGTH;
    station_id = get_station(iso_igrp->source_net);
    station_copy (station_id, &key [1]);

    adjacency = find_adjacency(clns_adjacency_db, key, FALSE, idb, 0);
    if (!adjacency) {
	adjacency = add_adjacency(clns_adjacency_db, key, idb, &snpa,
				  holdingtime, idb->clns_cluster_alias);
	if (!adjacency) return;
	adjacency->state = CLNS_ADJ_UP_STATE;
	adjacency->adjacencyType = CLNS_IS_ADJ_TYPE;
    }

    adjacency->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
    snpa_copy(&snpa, &adjacency->snpa);
    TIMER_START(adjacency->expiration_time, holdingtime * ONESEC);
    adjacency->L2only = (pdb->level == CLNS_LEVEL2) ? TRUE : FALSE;
    adjacency->protocol_source |= pdb->typeflag;

    /*
     * fill in list of area addresses
     */
    item = (iso_igrp_info_type *)iso_igrp->item;
    if (item->code == ISO_IGRP_AREALIST){
	/* 
	 * should check that each area in the packet is
	 * in the adjacency. should also check that
	 * there are no areas in the adjacency that are NOT
	 * in the packet.
	 */
	area_from_packet = (NSAP_T *)item->value;
	item_end = (char *)item + item->length;
	while ((char *)area_from_packet < (char *)item_end){
	    if (!area_in_adj(item->value, adjacency)){
		add_area_to_adj(item->value, adjacency);
		new_one = TRUE;
	    }
	    area_from_packet = (NSAP_T *)(char *)area_from_packet[0] + 1;
	}
    }

    /*
     * if this is a new neighbor entry and it was sent with
     * a broadcast, send out pt to pt hello right now
     */
    (*idb->hwptr->extract_snpa)(pak,&dest_snpa,DESTINATION);
    (*idb->hwptr->extract_snpa)(pak,&source_snpa,SOURCE);
    if (new_one && ((dest_snpa.target_type == TARGET_OSI_ALL_IS) ||
	(!(pak->if_input->hwptr->status & IDB_BCAST)))) {
	pdb2 = clns_find_level2_pdb(pdb->net);
	if (pdb2) iso_igrp_sendhello(pdb2, pak->if_input, &source_snpa);
    }
}

/*
 * process routing metrics
 *
 * walk through the packet. for each item,
 *    1) use the metric info to build a composite metric
 *    2) update the destinations we know about
 *           a) add new ones
 *           b) put unreachables into hold-down
 *    3) update route information
 */
static void process_routing_metrics (
    paktype *pak,
    int nbytes,
    char *ptr,
    int ptr_increment,
    idbtype *interface,
    clns_pdbtype *pdb,
    uchar type,
    NSAP_T source)
{
    iso_igrp_hdrtype *iso_igrp;
    hwaddrtype snpa;
    iso_igrp_metrictype *metric_item;
    ulong metric;
    clns_rdbtype *rdb;
    NSAP_T key;
    int i;

    iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);

    (*interface->hwptr->extract_snpa)(pak, &snpa, SOURCE);
    while (nbytes > ptr_increment) {
 	metric = iso_igrp_metric((iso_igrp_metrictype *)ptr, interface, pdb);
	metric_item = (iso_igrp_metrictype *)ptr;
	if (type == CLNS_LEVEL1)
	    key[0] = STATION_LENGTH;
	else
	    key[0] = AREA_LENGTH;

	for (i = 1; i <= key[0]; i++)
	    key[i] = metric_item->id[i-1];

	clns_ibuginf("\nISO-IGRP: Received level %d adv for %s metric %d",
		     type, (type == 1) ? station_string(&key [1]) :
		     area_string (&key [1]), metric);

	rdb = iso_igrp_destination_update
	        (pdb, type, key, interface, source, &snpa, metric,
		 pdb->typeflag, CLNS_ROUTE_ENTRY, pdb->invalidtime);
	iso_igrp_route_update(rdb,(iso_igrp_metrictype *)ptr, interface);
	ptr += ptr_increment;
	nbytes -= ptr_increment;
    }
}

/*
 * clns_prefix_has_idb_up
 *
 * Check to see if a prefix route has an outgoing interface that is up.
 */
boolean clns_prefix_has_idb_up (clns_ddbtype *ddb)
{
    clns_rdbtype *rdb;
    int i;

    for (i = 0; i < ddb->routecount; i++) {
        rdb = ddb->rdb[i];
	if (rdb->idb && interface_up(rdb->idb))
	    return(TRUE);

	/*
	 * If the next hop is the system then return true.
	 * For simplicity, check if metric is 0 and idb is null.
	 */
	if (!rdb->metric && !rdb->idb)
	    return(TRUE);
    }

    return(FALSE);
}

/*
 * clns_my_own_prefix 
 *
 * Determine if this prefix route is one that was sourced locally
 */
static boolean clns_my_own_prefix (iso_igrp_prefixroute *prefix, clns_ddbtype *ddb)
{
    clns_pdbtype *pdb;
    NSAP_T *source_NET;

    source_NET = &prefix->source_NET;
    for (pdb = (clns_pdbtype *) clns_protoQ.qhead; pdb; pdb=pdb->next) {
        if (nsap_equ(*source_NET, pdb->net))
	        return(TRUE);
    }

    return(FALSE);
}

/*
 * process prefix routes
 *
 */
static void process_prefix_routes (
    iso_igrp_info_type *info,
    clns_pdbtype *pdb,
    idbtype *interface,
    NSAP_T source,
    hwaddrtype *snpa)
{
    char *item_end, *ptr;
    int length;
    iso_igrp_prefixroute dummy, *one_prefix;
    iso_igrp_metrictype *metric;
    clns_ddbtype *dest;

    one_prefix = &dummy;
    length = 0;
    item_end = (char *)info + info->length;
    ptr = info->value;
    while (ptr < item_end) {
	memset(one_prefix, 0, sizeof(iso_igrp_prefixroute));

	/* 
	 * get validation info 
	 */
	one_prefix->validation.time = GETLONG(ptr);
	ptr += sizeof(ulong);
	one_prefix->validation.hold_time[0] = *ptr++;
	one_prefix->validation.hold_time[1] = *ptr++;

	/* 
	 * get metric and hop count info 
	 */
	metric = &(one_prefix->metric);
	bcopy(ptr, metric, ISO_IGRP_METRIC_HOPCOUNT_LENGTH);
	ptr += ISO_IGRP_METRIC_HOPCOUNT_LENGTH;

	/* 
	 * get the prefix 
	 */
	if (ptr[0] > CLNS_MAX_NSAP_LENGTH) {
	    clns_ibuginf("\nISO-IGRP: Invalid routing update(1) rcvd from %s",
			 interface->namestring);
	    clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	    return;
	}
	bcopy(ptr, metric->id, ptr[0] + 1);
	ptr += (ptr[0] + 1);

	/* 
	 * get the source NET 
	 */
	if ((ptr[0] == 0) || 
	    (ptr[0] > CLNS_MAX_NSAP_LENGTH)){
	    clns_ibuginf("\nISO-IGRP: Invalid routing update(2) rcvd from %s",
			 interface->namestring);
	    clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	    return;
	}
	bcopy(ptr, one_prefix->source_NET, ptr[0]+1);
	ptr += (ptr[0] + 1);

	if (ptr > item_end) {
	    clns_ibuginf("\nISO-IGRP: Invalid routing update(3) rcvd from %s",
			 interface->namestring);
	    clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	    return;
	}
	clns_ibuginf("\nISO-IGRP: Received prefix adv for %s metric %d",
		     (!one_prefix->metric.id [0]) ? "Default Prefix" :
		     nsap_string(one_prefix->metric.id), 
		     iso_igrp_metric (&one_prefix->metric, interface, pdb));

	dest = nsap_lookup(one_prefix->metric.id, MATCH_EXACT);
	if (dest) {

	    /*
	     * This pdb has better administrative distance than the
	     * route in the routing table.
	     */
	    if (pdb->distance < dest->distance) {
		if (dest->rdb [0]) {
		    clns_manage_prefix_nbrs (dest,
			     dest->rdb [0]->protocol_source, FALSE);
		}
		dest = NULL;
	    }
	    else if (pdb->distance == dest->distance) {
		clns_update_prefixroute(dest, one_prefix, pdb, interface,
					source, snpa);
	    }
	    else if ((dest->distance == ISO_STATIC_DIST) && 
		     !clns_prefix_has_idb_up(dest) && 
		     !clns_my_own_prefix(one_prefix, dest)) { 
	        clns_ibuginf("\nISO-IGRP: Overwriting entry for prefix %s "
			     "metric %d (route with lower distance is down)",
			     (!one_prefix->metric.id [0]) ? "Default Prefix" :
			     nsap_string(one_prefix->metric.id), 
			     iso_igrp_metric (&one_prefix->metric,
					      interface, pdb));
		clns_update_prefixroute(dest, one_prefix, pdb, interface,
					source, snpa);
		dest->distance = pdb->distance;
	    }
	}
	if (!dest) {
	    clns_add_prefixroute(one_prefix, interface, source, pdb, snpa);
	}
    }
}

/*
 * iso_igrp_rcvupdate
 *
 *    this routine receives a routing update and incorporates
 *    the data into the routing database.
 *
 */
static void iso_igrp_rcvupdate (
    clns_pdbtype *pdb,
    paktype *pak)
{
    iso_igrp_hdrtype *iso_igrp;
    idbtype *interface;
    int ptr_increment,nbytes;
    uchar type;
    NSAP_T source;
    uchar *source_station;
    iso_igrp_info_type *info;
    char *packet_end;
    hwaddrtype snpa;

    iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);

    interface = pak->if_input;
    (*interface->hwptr->extract_snpa)(pak, &snpa, SOURCE);

    source_station = get_station(iso_igrp->source_net);
    source[0] = STATION_LENGTH;
    station_copy(source_station, &source[1]);

    clns_ibuginf("\nISO-IGRP: Received update from %s (%s)",
		 station_string (&source [1]), pak->if_input->namestring);

    clns_ibuginf("\nISO-IGRP: Opcode: ");
    switch (iso_igrp->opcode){
      case ISO_IGRP_OPCODE_STATION:
	clns_ibuginf("station");
	break;
      case ISO_IGRP_OPCODE_AREA:
	clns_ibuginf("area");
	break;
      case ISO_IGRP_OPCODE_HELLO:
	clns_ibuginf("hello");
	break;
      case ISO_IGRP_OPCODE_QUERY:
	clns_ibuginf("query");
	break;
      default:
	clns_ibuginf("unrecognized update type");
    }

    ptr_increment = 0;
    info = (iso_igrp_info_type *)iso_igrp->item;
    if (iso_igrp->opcode & ISO_IGRP_OPCODE_STATION) {
	/*
	 * if we got a level 1 update but we are not in that area
	 *     throw away the update
	 */
	if (pdb->level & CLNS_LEVEL2)
	    return;
	/*
	 * station updates should only contain station routes
	 */
	if (info->code != ISO_IGRP_STATIONROUTE)
	    return;
	type = CLNS_STATION_TYPE;
	ptr_increment = sizeof(iso_igrp_metrictype) - 
	                       (CLNS_MAX_NSAP_LENGTH - STATION_LENGTH);
    } else {
	if (iso_igrp->opcode & ISO_IGRP_OPCODE_AREA) {
	    type = CLNS_AREA_TYPE;
	    ptr_increment = sizeof(iso_igrp_metrictype) -
		                         (CLNS_MAX_NSAP_LENGTH - AREA_LENGTH);
	    pdb = clns_find_level2_pdb(iso_igrp->source_net);
	    if (!pdb)
		return;
	    /*
	     * area updates may contain prefix routes, too
	     */
	    if ((info->code != ISO_IGRP_AREAROUTE) &&
		(info->code != ISO_IGRP_PREFIXROUTE))
		return;
	} else {
	    return;
	}
    }
    /*
     * don't accept the update if we haven't got a neighbor entry
     * for the source. 
     */
    if (!find_adjacency(clns_adjacency_db, source, FALSE, interface, 0))
	return;

    /*
     * walk through the various types of info in the update
     */
    info = (iso_igrp_info_type *)iso_igrp->item;
    packet_end = (char *)iso_igrp_headstart(pak) + GETSHORT(iso_igrp->len);
    while ((char *)info < (char *)packet_end) {
	/*
	 * handle area or station metrics
	 */
	nbytes = info->length;

	if ((info->code == ISO_IGRP_AREAROUTE) ||
	    (info->code == ISO_IGRP_STATIONROUTE))
	    process_routing_metrics(pak, nbytes, info->value, ptr_increment,
				    interface, pdb, type, source);
	else if (info->code == ISO_IGRP_PREFIXROUTE) {
	    process_prefix_routes(info, pdb, interface, source, &snpa);
	}
	if (info->length == 0){
	    clns_ibuginf("\nISO IGRP: Invalid routing update rcvd from %s",
			 interface->namestring);
	    clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	    return;
	}
	info = (iso_igrp_info_type *)((char *)info + info->length);
    }
}


/*
 * iso_igrp_send_router_hello
 *
 *    send all of the router to router hellos for this domain
 */
static void iso_igrp_send_router_hellos (
    clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;

    if (pdb->level & CLNS_LEVEL1)
	return;

    for (idbqueue=pdb->idbqueue; idbqueue; idbqueue=idbqueue->next){
	iso_igrp_sendhello(pdb, idbqueue->idb, (hwaddrtype *)NULL);
    }
}


/*
 * iso_igrp_sendupdate
 *
 *    This routine sends out a routing  update on the specified interface
 *    The routing update will contain all relevant entries from the
 *    pdbs database. We will do split-horizon checking for this interface.
 *    
 *    If the flag 'poison_prefix_routes' is TRUE, this routine will send
 *    out prefix route updates with a metric of infinity. The argument
 *    'rpdb' is the PDB that we're redistributing from, and it is these
 *    prefix routes that we need to poison.
 */
void iso_igrp_sendupdate (
    clns_pdbtype *pdb,
    idbtype *interface,
    boolean poison_prefix_routes,
    clns_pdbtype *rpdb,
    clns_ddbtype *specific_prefix)    
{
    int nbytes,maxbytes;
    boolean dest_done,prefix_done;
    paktype *pak;
    iso_igrp_hdrtype *iso_igrp;
    iso_igrp_metrictype *ptr;
    clns_ddbtype *dest;
    int hash_index, length;
    clns_rdbtype *rdb;
    ushort total_length;
    iso_igrp_info_type *dest_adverts, *prefix_adverts;
    ushort itemlength;
    int itemcount;
    clns_adjtype *adj;
    clns_pdbtype *pdb2;
    uchar        *source_net;

    source_net = pdb->net;

    /*
     * If sending updates for area/domain routes, make sure we use the
     * correct source NET address that matches the interface. 
     */
    if (pdb->level & CLNS_LEVEL2) {
	for (pdb2 = clns_protoQ.qhead; pdb2; pdb2=pdb2->next) {
	    if ((clns_domain_match (pdb->domain_length, pdb->domain,
				    pdb2->domain_length, pdb2->domain)) &&
		(pdb2->level & CLNS_LEVEL1)) {
		if (!itf_in_pdb (interface, pdb2)) continue;

		/*
		 * Source NET should be the one that the pdb uses for
		 * this interface. 
		 */
		source_net = pdb2->net;
	    }
	}
    }

    dest_done = prefix_done = FALSE;
    hash_index = 0;
    dest = NULL;
    rdb = NULL;
    if (pdb->level == CLNS_LEVEL1) 
	prefix_done = TRUE;
    else {
	/*
	 * If we're poisoning prefix routes, then we don't need to
	 * bother about sending other routes.
	 */
	if (poison_prefix_routes) {
	    if (rpdb || specific_prefix)
	        dest_done = TRUE;
	}
    }

    while ((!dest_done) || (!prefix_done)){
	/*
	 * we want to make each routing update as big as it can be
	 * without fragmenting. 
	 */
	nbytes = interface->clns_mtu - sizeof(iso_igrp_hdrtype) - 
	    ISO_IGRP_TLV_HDR_LENGTH;

	pak = clns_getbuffer(interface->clns_mtu);
	if (!pak) return;

	pak->if_output = interface;

	iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);
	iso_igrp->pid = NLP_ISO_IGRP;
	iso_igrp->hlen = sizeof(iso_igrp_hdrtype);
	iso_igrp->version = ISO_IGRP_VERSION;
	iso_igrp->edition = pdb->edition;

	nsap_copy(source_net, iso_igrp->source_net);
	clns_pad_net(iso_igrp->source_net);
	/* 
	 * fill in the fields that are different for level 1 and level 2
	 */
	prefix_adverts = 0;
	dest_adverts = (iso_igrp_info_type *)iso_igrp->item;
	dest_adverts->length = 0;
	if (pdb->level & CLNS_LEVEL2){
	    iso_igrp->opcode = ISO_IGRP_OPCODE_AREA;
	    dest_adverts->code = ISO_IGRP_AREAROUTE;
	    itemlength = sizeof(iso_igrp_metrictype)
		          - (CLNS_MAX_NSAP_LENGTH - AREA_LENGTH);
	    length = AREA_LENGTH;
	} else {
	    iso_igrp->opcode = ISO_IGRP_OPCODE_STATION;
	    dest_adverts->code = ISO_IGRP_STATIONROUTE;
	    itemlength = sizeof(iso_igrp_metrictype)
		          - (CLNS_MAX_NSAP_LENGTH - STATION_LENGTH);
	    length = STATION_LENGTH;
	}
	/* 
	 *  build an entry for each dest we know how to reach
	 * stop when there are no more dests or we have filled the
	 * maximum packet size
  	 */
  	ptr = (iso_igrp_metrictype *)dest_adverts->value;
  	dest_adverts->length = 0;
 	if (nbytes > 255)
 	    nbytes = 255;
 
 	nbytes = nbytes - ISO_IGRP_TLV_HDR_LENGTH;
  	nbytes = (nbytes / itemlength) * itemlength;
  	maxbytes = nbytes;
  	itemcount = 0;
 	if (!dest_done) {
 	    while (nbytes > 0) {
 		dest = getnextdest(pdb, &hash_index, dest, interface, &rdb);
 		if (!dest) {
 		    dest_done = TRUE;
 		    break;
 		}
		if (rdb) {
		    adj = find_adjacency(clns_adjacency_db, rdb->first_hop,
					 FALSE, rdb->idb, 0);
		    if (!adj)
			rdb = NULL;
		}

 		clns_ibuginf("\nISO-IGRP: Advertise dest: %s",
		     (pdb->level == CLNS_LEVEL1) ? 
		     station_string(&dest->destination [1]) :
		     area_string (&dest->destination [1]));

 		clns_setupmetric(pdb, ptr, dest, rdb, FALSE);
 		bcopy((uchar *)&dest->destination[1], ptr->id, length);
 		nbytes -= itemlength;
 		ptr = (iso_igrp_metrictype *)((char *)ptr+itemlength);
 		itemcount++;
 	    }
 	    rdb = NULL;
 	    if (itemcount == 0)
 		ptr = (iso_igrp_metrictype *)iso_igrp->item;
 	    else
 		dest_adverts->length = (uchar)(((uchar *)ptr) - iso_igrp->item);
 	}
 	if (dest_adverts->length == 0)
 	    ptr = (iso_igrp_metrictype *)iso_igrp->item;
 	/*
 	 * check if we should put any prefix routes in this update
 	 */
 	if ((!prefix_done) && (nbytes >= sizeof(iso_igrp_prefixroute))){
 	    prefix_adverts = (iso_igrp_info_type *)ptr;
 	    prefix_adverts->length = 0;
 	    if (pdb->level == CLNS_LEVEL2){
 		prefix_done = clns_build_prefix_routes(pdb, interface,
		   (iso_igrp_info_type **)&ptr, &nbytes, &dest, 
		   poison_prefix_routes, rpdb, specific_prefix);
 	    } else
 		prefix_done = TRUE;
 	}
  	/*
  	 * if packet has no destinations or prefix routes, discard it
  	 */
  	if (nbytes == maxbytes){
 	    retbuffer(pak);
  	    return;
  	}
  	/* 
	 * finally send the packet
	 */
	total_length = iso_igrp->hlen;
	if (itemcount != 0)
	    total_length += dest_adverts->length;
	if (prefix_adverts)
	    total_length += prefix_adverts->length;

	PUTSHORT(iso_igrp->len, total_length);
	if (clns_encaps(pak, total_length, interface, &all_iss, NULL)){
	    clns_ibuginf("\nISO-IGRP: Sending update on interface: %s",
			 pak->if_output->namestring);
	    clns_traffic[ISO_IGRP_OUT]++;
	    datagram_out(pak);
	} else {
	    datagram_done(pak);
	    if (!is_x25(interface->hwptr)){
		clns_ibuginf("\nISO-IGRP: Encap failed on update. if: %s",
			     interface->namestring);
	    }
	}
    }
}

/*
 * iso_igrp_ager
 *
 *    age the appropriate routes. some destinations may
 *    go into holddown.
 */
static void iso_igrp_ager(
    clns_pdbtype *pdb)
{
    int j, route;
    clns_ddbtype *dest, *prev;
    clns_rdbtype *rdb;
    char null_selector;

    null_selector = 0;
    for (j = 0; j < CLNS_NETHASHLEN; j++) {

	prev = (clns_ddbtype *)&pdb->dest_routing_table[j];
	dest = prev->next;

	while (dest) {

	    /*
	     * the destination is still reachable. Check if any of
	     * the routes have expired
	     */
	    if (!TIMER_RUNNING(dest->blindtimer)) {
		for (route = 0; route < dest->routecount; route++) {
		    rdb=dest->rdb[route];
		    /* 
		     * only age out an entry if we learned it via esis or igrp
		     * get rid of routes where:
		     *  1) the interface is down
		     *       or
		     *  2) the route has expired
		     */
		    if (TIMER_RUNNING_AND_AWAKE(rdb->expiration_time)) {
			clns_rbuginf("\nISO-IGRP-RT: Aging rdb for %s via %s",
			     ((pdb->level == CLNS_LEVEL1) ?     
				     station_string (&dest->destination [1]) :
				     area_string (&dest->destination [1])),
			     station_string(&rdb->first_hop [1]));

			iso_igrp_route_delete(pdb,dest,route);
		    }
		}
	    }

	    /*
	     * If the destination was in hold-down but the timer
	     * has expired - delete it
	     * Clean the leftover rdb structures before deletion.
	     */
	    if (TIMER_RUNNING_AND_AWAKE(dest->blindtimer)) {
		clns_rbuginf("\nISO-IGRP-RT: flushing destination: %s",
		     ((pdb->level == CLNS_LEVEL1) ?     
		      station_string (&dest->destination [1]) :
			     area_string (&dest->destination [1])));

		/*
		 * If an area route was deleted, see if there is a matching
		 * DECnet "advertised" type route that we need to mark as
		 * "down". Of course, this is necessary only if we're
		 * doing conversion.
		 */
		if (reg_invoke_dnconversion_report())
		    reg_invoke_decnet_trigger_L2_routes();

		clns_invalidate_cache("iso_igrp_ager", FALSE);
		prev->next = dest->next;
		dest->next = NULL;
 		for (route = 0; route < dest->routecount; route++)
 		    if (dest->rdb[route]) {
 			free(dest->rdb[route]);
 			dest->rdb[route] = NULL;
 		    }
		free(dest);
		dest = prev;
	    }
	    prev = dest;
	    dest = dest->next;
	} /* end of while (dest) */
    }
}

/*
 * iso_igrp_majorupdate
 *
 *    send a routing update out each appropriate interface
 */
static void iso_igrp_majorupdate (
    clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;
    idbtype *idb;

    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next){
	idb = idbqueue->idb;
	if ((idb) && (idb->clns_enabled) && (interface_up(idb))){
	    iso_igrp_sendupdate(pdb, idb, FALSE, NULL, NULL);
	}
    }
}
	

/*
 * check_iso_igrp_packet
 *
 *    do validity checking on the incoming iso igrp packet 
 *
*/
static boolean check_iso_igrp_packet (paktype *pak)
{
    iso_igrp_hdrtype *iso_igrp;
    uchar hlen;
    ulong length;
    iso_igrp_info_type *info;
    char *packet_end;

    iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);

    if (iso_igrp->version != ISO_IGRP_VERSION) {
	return(FALSE);
    }
    hlen = sizeof(iso_igrp_hdrtype);
    if (hlen != iso_igrp->hlen) {
	clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	return(FALSE);
    }
    /*
     * see if the total length of the packet agrees with the
     * sum of the parts
     */
    packet_end = (char *)iso_igrp_headstart(pak) + GETSHORT(iso_igrp->len);
    info = (iso_igrp_info_type *)iso_igrp->item;
    length = hlen;
    while ((char *)info < (char *)packet_end){
	if (info->length == 0){
	    clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	    return(FALSE);
	    break;
	}
	length += info->length;
	info = (iso_igrp_info_type *)((char *)info + info->length);
    }
    if (length != GETSHORT(iso_igrp->len)) {
	clns_traffic[ISO_IGRP_SYNTAX_ERROR]++;
	return(FALSE);
    }	
    return(TRUE);
}

/*
 * iso_igrp_check_hello
 *
 */
static boolean iso_igrp_check_hello (paktype *pak)
{
    uchar hlen;
    iso_igrp_hello_hdrtype *iso_igrp;

    iso_igrp = (iso_igrp_hello_hdrtype *)iso_igrp_headstart(pak);

    if (iso_igrp->version != ISO_IGRP_VERSION)
	return(FALSE);

    hlen = ISO_IGRP_HELLO_HDRSIZE;
    if (hlen != iso_igrp->hlen){
	hwaddrtype snpa;

	clns_traffic[CLNS_HDRSYNTAX]++;
	(*pak->if_input->hwptr->extract_snpa)(pak, &snpa, SOURCE);
	clns_ibuginf("\nISO_IGRP: Invalid header length (%d vs %d)",
		     iso_igrp->hlen, hlen);
	clns_ibuginf("\n          from %s:%s",
		     pak->if_input->namestring,
		     snpa_string(&snpa));
	return(FALSE);
    }
    return(TRUE);
}


/*
 * clns_igrp_router
 * Main IGRP routing process.  One for each area and one for each domain.
 *    This process runs periodically and processes all messages in
 *    its input queue. It may send out routing updates.
 */
forktype clns_igrp_router (clns_pdbtype *pdb)
{
    paktype *pak;
    iso_igrp_hdrtype *iso_igrp;
    boolean sendupdate;

    edisms (systeminitBLOCK, 0);
    sendupdate = TRUE;
    iso_igrp_send_router_hellos(pdb);
    mgd_timer_init_leaf(&pdb->timer, NULL, 0, NULL, FALSE);/* Runs by itself */
    mgd_timer_start(&pdb->timer, 2 * ONESEC);
    pdb->hello_timer = mgd_timer_exp_time(&pdb->timer);

    while (TRUE) {
	/* 
	 * send our routing table out all of the interfaces
	 * which are in this domain/area
	 */
	if (TIMER_RUNNING_AND_AWAKE(pdb->hello_timer)){
	    iso_igrp_send_router_hellos(pdb);
	    TIMER_UPDATE(pdb->hello_timer, pdb->hellotime);
	}
	if (sendupdate || pdb->changed) {
	    clns_ibuginf("\nISO-IGRP: Originating level %d %s update",
			 pdb->level, (sendupdate) ? "periodic" : "flash");
	    iso_igrp_majorupdate(pdb);
	    sendupdate = FALSE;
	}
	pdb->changed = FALSE;
	edisms((blockproc *)iso_pdbBLOCK, (ulong)pdb);

	/* 
	 * go read and process each incoming packet 
	 */
	pak = dequeue(&pdb->clns_inputq);
	while (pak){ 

	    iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);

	    if (!verify_iso_checksum(iso_igrp_headstart(pak),
				     iso_igrp->hlen,
				     iso_igrp->checksum)){
		clns_traffic[CLNS_CHECKSUM]++;
	    } else {
		switch (iso_igrp->opcode){
		  case ISO_IGRP_OPCODE_HELLO:
		    /* 
		     * Note that the length of the hello must be
		     * checked as the hello is parsed
		     */
		    if (iso_igrp_check_hello(pak)){
			clns_traffic[ISO_IGRP_H_IN]++;
			iso_igrp_rcvhello(pdb, pak);
		    }
		    break;
		  case (ISO_IGRP_OPCODE_QUERY | ISO_IGRP_OPCODE_STATION):
		  case (ISO_IGRP_OPCODE_QUERY | ISO_IGRP_OPCODE_AREA):
		      if (check_iso_igrp_packet(pak)){
			  clns_traffic[ISO_IGRP_QIN]++;
			  clns_ibuginf
			  ("\nISO-IGRP: Originating update in response to query");
			  iso_igrp_sendupdate(pdb, pak->if_input, FALSE, NULL,
					      NULL);
		      }
		    break;
		  case ISO_IGRP_OPCODE_STATION:
		  case ISO_IGRP_OPCODE_AREA:
		    if (check_iso_igrp_packet(pak)){
			clns_traffic[ISO_IGRP_IN]++;
			iso_igrp_rcvupdate(pdb, pak);
		    }
		    break;
		  default:
		    break;
		}
	    }
	    datagram_done(pak);
	    pak = dequeue(&pdb->clns_inputq);
	}
	/*
	 * if we don't need to go back to sleep now,
	 * age the routing table
	 */
	if (mgd_timer_expired(&pdb->timer)) {
	    iso_igrp_ager(pdb);
	    sendupdate = TRUE;
	    mgd_timer_update(&pdb->timer, pdb->broadcasttime);
	    (void)process_may_suspend();
	}
	if (pdb->changed)
	    pdb->edition++;
    }
}

/*
 * iso_igrp_route
 *
 */
clns_adjtype *iso_igrp_route (
    clns_pdbtype *pdb,
    uchar *destination,
    int depth)
{

    clns_ddbtype *ddb;
    clns_adjtype *adj;
    NSAP_T key;
    uchar        *id;
    int i;

    if (pdb->level == CLNS_LEVEL1) {
	id = get_station (destination);
	key[0] = STATION_LENGTH;
	for (i = 1; i <= STATION_LENGTH; i++)
	    key[i] = id[i-1];
    } else {
	id = get_area (destination);
	key[0] = AREA_LENGTH;
	for (i = 1; i <= AREA_LENGTH; i++)
	    key[i] = id[i-1];
    }
    ddb = clns_find_dest (pdb, key);
    adj = NULL;
    if (ddb) {
	adj = clns_get_route (destination, ddb, depth);
    }

    return (adj);
}

/*
 * iso_igrp_compute_metric
 *
 *    build a composite metric for a neighbor end system or intermediate system
 */
ulong iso_igrp_compute_metric (clns_pdbtype *pdb, idbtype *interface)
{

    ulong delay, bandwidth;
    ulong reliability, metric;
    uchar load;

    bandwidth = SCALED_BANDWIDTH(interface->visible_bandwidth);
    delay = interface->delay;
    reliability = interface->hwptr->reliability;
    load = interface->hwptr->load;

    metric = 0;
    if (pdb->igrp_k1)
	metric += bandwidth * pdb->igrp_k1;
    if (pdb->igrp_k2)
	metric += bandwidth * pdb->igrp_k2 / (256 - (ulong)load);
    if (pdb->igrp_k3)
	metric += delay * pdb->igrp_k3;
    if (pdb->igrp_k5){
	metric *= pdb->igrp_k5;
	metric /= (reliability + pdb->igrp_k4);
    }
    return(metric);
}
