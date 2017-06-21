/* $Id: clns_parse2.c,v 3.9.22.9 1996/08/16 04:43:37 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_parse2.c,v $
 *------------------------------------------------------------------
 * clns_parse2.c -- Routines to help in parsing command syntax
 *
 * Jan-15-1990 by Paulina Knibbe
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_parse2.c,v $
 * Revision 3.9.22.9  1996/08/16  04:43:37  dkatz
 * CSCdi66139:  Stopping CLNS IS-IS process causes a router crash
 * Branch: California_branch
 * Null out a couple of fields after freeing the memory pointed to
 * therefrom.
 *
 * Revision 3.9.22.8  1996/08/08  14:50:29  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.9.22.7  1996/08/07  08:57:11  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.22.6  1996/08/03  23:20:25  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.9.22.5  1996/07/26  18:14:04  tylin
 * CSCdi59808:  clns ping fails between 7000 ethernet interfaces
 *    Fix dialer i/f checking by calling is_ddr().
 *    Update driver's filter table for all ES/IS multicast addresses
 *    whenever enable/disable CLNS routing.
 * Branch: California_branch
 *
 * Revision 3.9.22.4  1996/05/22  17:03:59  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.9.22.3  1996/05/04  20:04:04  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.9.22.2  1996/03/29  00:36:29  rchandra
 * CSCdi50266:  Router didnt learn routing path via isis
 * Branch: California_branch
 * - when ISIS is disabled on an interface, delete the isisidb structure
 *   associated with the interface. If it is not deleted and if ISIS is
 *   enabled again, previous isisidb will get reused resulting in bogus
 *   interface number in the systemid
 *
 * Revision 3.9.22.1  1996/03/18  19:02:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/13  01:13:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.2  1996/03/07  08:40:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  00:21:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/01/23  01:39:37  asastry
 * CSCdi47322:  ISO-IGRP displays *Unknown SNPA* if CLNS default route is
 * configured
 *
 * Revision 3.8  1996/01/18  21:48:09  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/05  04:55:26  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/11  19:50:42  rchandra
 * CSCdi42468:  after clear clns route, static neighbors not advertised by
 * iso-igrp
 * - while doing 'clear clns route' do not delete the static neighbor
 *   routes
 *
 * Revision 3.5  1995/12/01  17:01:34  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.4  1995/12/01  02:48:15  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:52:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:51:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/21  19:02:55  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.5  1995/09/23  21:37:24  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.4  1995/08/23  18:25:51  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.3  1995/06/30  23:39:51  dbath
 * CSCdi35958:  show clns routes command fields not wide enough for slots
 * 10,11...
 * Make show 'clns route', 'clns neigh', 'clns is-neigh',
 * 'clns es-neigh' and 'isis route' all use short names for interfaces
 * to make room for 2 digit slot numbers (should also help for FEIP).
 *
 * Revision 2.2  1995/06/09  12:58:58  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:17:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "../ip/tunnel.h"
#include "../x25/x25.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../iprouting/route.h"
#include "../decnet/dnconversion.h"
#include "../clns/isis.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "../clns/isis_inlines.h"
#include "../os/chunk.h"
#include "bitlogic.h"

static char temp_domain_name[CLNS_MAX_TAG_LENGTH];


/*
 * clns_find_router_name
 *
 * Find the pdb for the routing process associated with this
 * router name. It will always be an area routing process.
 *
 */
clns_pdbtype *clns_find_router_name (char *router_name)
{
    clns_pdbtype *pdb;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (strlen(router_name) != strlen(pdb->pname)) {
	    continue;
	}
	if (!strcasecmp(router_name, pdb->pname)) {
	    break;
	}
    }
    return(pdb);
}

/*
 * Start CLNS processes if not already running, and force hellos.
 */
static void start_clns_processes (void)
{
    if (!clns_processes_setup) {
	mgd_timer_start(&esis_ager_timer, 0); /* Make it run right away */
	clns_processes_setup = TRUE;
	process_create(clns_input, "CLNS Input", IPROUTING_STACK, PRIO_HIGH);
	(void) cfork(esis_router, 0L, 2000, "ES-IS Routing", CONSOLE_LINENUM);
    }
    force_hellos();
}

/* 
 * clns_add_net
 *
 * Build NET for system and start CLNS and ES-IS processes if
 * not already started.
 */
void clns_add_net (NSAP_T nsap)
{
    clns_ddbtype *ddb;

    ddb = nsap_lookup(nsap, MATCH_EXACT);
    if (ddb) {
	/* NET already exists */
	if (ddb->entry_type == CLNS_OURNET_ENTRY) {
	    return;
	} else {
	    printf("\nCLNS: net %n already exists as %s", 
		   nsap, clns_hellostring(ddb->entry_type));
	}
	return;
    }

    clns_route_update(nsap, NULL, NULL, CLNS_OURNET_ENTRY, ISO_STATIC, 
		      ISO_STATIC_DIST, 0, timer_no_time, 0, NULL);
    /*
     * Initialize clns_myNET.
     * This should only be used if there are no iso igrp routing processes
     */
    nsap_copy(nsap, clns_myNET);

    /*
     * Start processes if necessary
     */
    clns_running = TRUE;
    start_clns_processes();
}

/*
 * build_domain_name
 *
 */
char *build_domain_name (char *area_name)
{
    bcopy((uchar *)"DOMAIN_", (uchar *)temp_domain_name, 8);
    if (area_name) {
	strcpy(&temp_domain_name[7], area_name);
    }
    return(temp_domain_name);
}

/*
 * clns_add_intfc_router
 *
 *    Add the specified interface to the specified clns router
 */
boolean clns_add_intfc_router (
    char    *router_name,
    idbtype *idb,
    int     protocol,
    int     level)
{
    clns_pdbtype *pdb, *pdb2;
    idbqueuetype *idbqueue;
    char         *domain_name;
    NSAP_T       nsap;
    isisidbtype  *isisidb;

    /*
     * Add to queue for this pdb first find the pdb queue for 
     * this area then find the pdb queue for this domain.
     */
    pdb = clns_find_router_name(router_name);
    domain_name = build_domain_name(router_name);
    pdb2 = NULL;
    if (!pdb) {
	nsap[0] = 0;
	if (protocol == ISO_ISISMASK) {
	    pdb = isis_pdb_init(router_name, PROC_TYPE_ISIS);
	} else {
	    if (CLNS_LEVEL1 & level) {
		pdb = clns_igrp_pdb_init(nsap, router_name, CLNS_LEVEL1, TRUE);
 		if (!pdb) return(FALSE);
	    }
	    if (CLNS_LEVEL2 & level) {
		pdb2 = clns_igrp_pdb_init(nsap, domain_name, CLNS_LEVEL2, TRUE);
 		if (!pdb2) return(FALSE);
	    }
	    /* 
	     * if level 1 router not asked for, copy level 2 pdb over to
	     * pdb so we can work with it
	     */
	    if (!pdb) pdb = pdb2;
	}
    }

    if (!pdb) return(FALSE);

    if (!(pdb->typeflag & protocol)) {
	printf("\nInvalid tag - it is assigned to %s", pdb->name);
	return(FALSE);
    }

    if (level != CLNS_LEVEL2) {
	/*
	 * check if this interface is already in this area
	 */
	for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	    if (idbqueue->idb == idb) return(FALSE);
	}
	
	if (!clns_add_if_to_pdbqueue(idb, pdb)) {
	    printf("\nCLNS: could not add interface; no memory");
	    return(FALSE);
	}

	/*
	 * Add static neighbors to ISO-IGRP level-1 routing table for
	 * this interface.
	 */
	if (pdb->net[0]) iso_igrp_add_static_nbrs(pdb, idb);
    }

    /*
     * Enable clns on this interface.
     */
    if (!idb->clns_enabled)
	clns_itf_enable(idb);

    clns_running = TRUE;
    clns_router_running = TRUE;

    /*
     * Do IS-IS initialization.
     */
    if (pdb->typeflag & ISO_ISISMASK) {
	clns_adjtype *adj;
	int bucket;
	isisidb = isis_alloc_isisidb(idb, PROC_TYPE_ISIS, CLNS_PDBINDEX_ISIS0);
	if (!isisidb)
	    return(FALSE);
	isis_start_interface(pdb, idb);
	if (pdb->net[0])
	    isis_add_static_nbrs(pdb, idb);
	/* Looks for Decnet adjacency */
	adj = NULL;
	while ((adj = next_neighbor(clns_adjacency_db, idb, 0, adj,
				    &bucket))) {
	    if (!(adj->protocol_source & DECNET_PHASEIV)) continue;
	    isis_new_es_adjacency(pdb, adj);
	    return (TRUE);
	}
	return(TRUE);
    }

    /*
     * If this is a static pdb, don't look for the domain.
     */
    if (pdb->typeflag == ISO_STATIC) return(TRUE);

    /* 
     * Now look for the pdb for the domain associated with this area.
     */
    if (!pdb->net[0] || !(pdb = clns_find_level2_pdb(pdb->net))) {
	if (!(pdb = pdb2)) {
	    pdb = clns_find_router_name(domain_name);
	    if (!pdb) return(TRUE);
	}
    }

    /*
     * Check if this interface is already in this domain
     * Note that it is NOT an error if it is. We may have an interface
     * in multiple areas that are in the same domain.
     */
    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	if (idbqueue->idb == idb) return(FALSE);
    }

    idbqueue = clns_add_if_to_pdbqueue(idb, pdb);
    if (!idbqueue){
	printf("\nCLNS: could not add interface; no memory");
	return(FALSE);
    }    
    /* In Level-2, there is a single pdb for each domain. There could be
     * may areas in it. Copy the router_name so to NVGEN the correct
     * process name.
     */
    setstring(&idbqueue->pname, router_name);

    return(TRUE);
}

/*
 * clns_add_isis_router
 */
void clns_add_isis_router (clns_pdbtype *pdb)
{
    /* Fork the processes. */

    isis_fork_processes(pdb, PROC_TYPE_ISIS);

    /*
     * Finally - check if we need to create clnp and esis.
     */
    start_clns_processes();
}

/*
 * clns_add_router
 *
 *    Create a new domain and/or area clns igrp router
 */
boolean clns_add_router (
    char *router_name,
    NSAP_T prefix,
    boolean dynamic,
    int protocol)
{

    clns_pdbtype *pdb, *pdb2;
    char         *domain_name;
    uchar        *area;
    int          i, have_net;
    uchar        nsap[CLNS_MAX_NSAP_LENGTH];
    uchar        station_id[STATION_LENGTH+1];
    NSAP_T       area_id;

    have_net = (prefix[0]);
    nsap[0] = 0;
    if (have_net) {
	/*
	 * do validity checking on the prefix
	 * when checking to see if we need to create an area process
	 *     only look at the domain and area so that more than one
	 *     net in an area will not create more than one process !
	 */
	if ((dynamic) &&
	   (prefix[0] < (MIN_DOMAIN_LENGTH+AREA_LENGTH+STATION_LENGTH+1))){
	    printf("\nCLNS: invalid net; too short");
	    return(FALSE);
	}
	if ((dynamic) && 
	    (prefix[0] > (MAX_DOMAIN_LENGTH+AREA_LENGTH+STATION_LENGTH+1))){
	    printf("\nCLNS: invalid net; too long");
	    return(FALSE);
	}

	if ((dynamic) && (prefix[prefix[0]] != 0)){
	    printf("\nCLNS: invalid selector field. Must be zero.");
	    return(FALSE);
	}
	nsap_copy(prefix, nsap);
    }

    /* 
     * Now add the pdb for the area use the full net here.
     */
    pdb = clns_find_router_name(router_name);
    if (!pdb) {
	pdb = clns_igrp_pdb_init(*(NSAP_T *)nsap, router_name,
				 CLNS_LEVEL1, dynamic);
	if (!pdb) return(FALSE);
    }

    if (have_net) {

	/*
	 * Add the NET for this area to the non-conforming database.
	 */
	clns_add_net(nsap);

	/*
	 * Add the station id to the conforming database.
	 */
	pdb->domain = &pdb->net[1];
	pdb->domain_length = pdb->net[0] - AREA_LENGTH - STATION_LENGTH - 1;
	station_id[0] = STATION_LENGTH;
	station_copy(get_station(nsap), &station_id[1]);
	if (!iso_igrp_destination_update(pdb, CLNS_STATION_TYPE, station_id, 
					 NULL, NULL, NULL, 0, ISO_STATIC, 
					 CLNS_OURNET_ENTRY, 0)) {
	    return(FALSE);
	}

	/*
	 * Add static ES/IS neighbors to level-1 routing table.
	 */
	iso_igrp_add_static_nbrs(pdb, NULL);

	/*
	 * Actually create the process for this area.
	 */
	if (dynamic) {
	    pdb->clns_pid = cfork((forkproc *)clns_igrp_router, (long)pdb,1500,
				  "ISO-IGRP Router", CONSOLE_LINENUM);
	}
    }

    /*
     * if we haven't already got a clns and esis process - add them now
     */
    start_clns_processes();
    clns_router_running = TRUE;

    clns_running = TRUE;
    if (!dynamic) return(TRUE);

    if (!have_net) return(TRUE);

    /*
     * If there isn't already a pdb for the domain, add it.
     */
    domain_name = build_domain_name(router_name);
    if (!(pdb2 = clns_find_level2_pdb(pdb->net))) {

	/*
	 * There is no domain pdb with an address. There might
	 * be a domain pdb from a "clns router" interface subcommand
	 * forward reference. Use this pdb.
	 */
	if (!(pdb2 = clns_find_router_name(domain_name))) {
	    pdb2 = clns_igrp_pdb_init(*(NSAP_T *)nsap, domain_name, CLNS_LEVEL2,
				      dynamic); 
	    if (!pdb2) {

		/*
		 * We could not allocate a pdb for this domain.
		 * Go back and DELETE the pdb for this area !
		 */
		clns_delete_router(router_name);
		return(FALSE);
	    }
	}

	nsap_copy(pdb->net, pdb2->net);
	pdb2->domain = &pdb2->net[1];
	pdb2->domain_length = pdb2->net[0] - AREA_LENGTH - 
	    STATION_LENGTH - 1;
	pdb2->distance = pdb->distance;
	pdb2->igrp_k1 = pdb->igrp_k1;
	pdb2->igrp_k2 = pdb->igrp_k2;
	pdb2->igrp_k3 = pdb->igrp_k3;
	pdb2->igrp_k4 = pdb->igrp_k4;
	pdb2->igrp_k5 = pdb->igrp_k5;
	pdb2->redistallowed |= (pdb->redistallowed & ~pdb->typeflag);
	for (i = 0; i < CLNS_MAXROUTERS; i++) {
	    pdb2->routemap[i] = pdb->routemap[i];
	}

	if (dynamic) {
	    pdb2->clns_pid = cfork((forkproc *)clns_igrp_router,(long)pdb2, 1500, 
			      "ISO-IGRP Router", CONSOLE_LINENUM);
	    clns_set_pdb_running(pdb2, TRUE);
	}
    } else {
	pdb = clns_find_router_name(domain_name);
	if (pdb && (pdb != pdb2)) {
	    clns_make_common_domain_pdb(pdb2, pdb);
	}
    }

    /*
     * Add a destination entry for this area.
     */
    area_id[0] = AREA_LENGTH;
    area = get_area(nsap);
    for (i = 1; i <= AREA_LENGTH; i++) {
	area_id[i] = area[i-1];
    }

    if (!iso_igrp_destination_update(pdb2, CLNS_AREA_TYPE,
	     area_id, NULL, NULL, NULL, 0, ISO_STATIC, CLNS_OURNET_ENTRY, 0)) {
	return(FALSE);
    }

    /*
     * Add static areas from ES/IS neighbors that are in our domain but
     * not in any of our areas.
     */
    iso_igrp_add_static_nbrs(pdb2, NULL);

    return(TRUE);
}

/*
 * clns_make_common_domain_pdb
 *
 * There is already a domain pdb with an NET assigned. Since
 * this newly added pdb is in the same domain, use a common domain
 * pdb. Delete the bogus domain pdb that could have been
 * generated from a "clns router" interface subcommand reference.
 * Don't forget to copy the interfaces that it was enabled for to the
 * common pdb.
 */
void clns_make_common_domain_pdb (
    clns_pdbtype *domain_pdb,
    clns_pdbtype *bogus_pdb)
{
    idbqueuetype *b_idbq, *d_idbq, *next;

    /*
     * Copy interfaces from bogus pdb into common domain pdb.
     */
    for (b_idbq = bogus_pdb->idbqueue; b_idbq; b_idbq = next) {
	next = b_idbq->next;
	for (d_idbq = domain_pdb->idbqueue; d_idbq; d_idbq = d_idbq->next) {
	    if (d_idbq->idb == b_idbq->idb)
		break;
	}
	if (d_idbq) {
	    /*
	     * Throw away node from bogus_pdb's linked list
	     */
	    free(b_idbq);
	} else {
	    /*
	     * Copy node from bogus_pdb's linked list to domain_pdb's
	     */
	    b_idbq->next = domain_pdb->idbqueue;
	    domain_pdb->idbqueue = b_idbq;
	}
    }

    /*
     * Delete bogus pdb from clns_protoQ link list.
     */
    unqueue(&clns_protoQ, bogus_pdb);
    
    /*
     * Deallocate the name string and pdb.
     */
    free(bogus_pdb->pname);
    free(bogus_pdb);
}

/*
 * clns_remove_idb_depend
 *
 *    Remove any information which depends on the
 *    specified interface
 */
void clns_remove_idb_depend (idbtype *idb)
{
    clns_ddbtype *ddb;
    clns_rdbtype *rdb;
    clns_adjtype *adj = NULL; 
    clns_pdbtype *pdb;
    int          i;

    /*
     * Delete prefix routes for adjacencies on downed interface.
     */
    for (ddb = nsap_next(NULL); ddb; ddb = nsap_next(ddb)) {
	for (i = 0; i < ddb->routecount; i++) {
	    rdb = ddb->rdb[i];
	    adj = find_adjacency(clns_adjacency_db, rdb->first_hop, FALSE,
				 idb, 0);
	    if (!adj) continue;
 	    if (rdb->protocol_source & ISO_IGRPMASK) {
 		ddb->metric = CLNS_METRIC_INACCESSIBLE;
 		rdb->metric = CLNS_METRIC_INACCESSIBLE;
 		clns_flash_update(rdb->protocol_source);
	    }
	}
    }
 
    delete_interface_adjacencies(clns_adjacency_db, idb);

    pdb = find_isis_pdb();
    if (pdb && pdb->net[0]) {
	isis_reset_circuit_id(pdb, idb);
    }
}

/*
 * clns_delete_intfc_router
 *
 */
boolean clns_delete_intfc_router (clns_pdbtype *pdb, idbtype *idb)
{
    boolean      iso_igrp_in_use = FALSE;
    boolean      clns_isis_in_use = FALSE, ip_isis_in_use = FALSE;
    NSAP_T       net;
    clns_pdbtype *temp_pdb; 
    isisidbtype  *isisidb;

    /*
     * Search thru' PDBs to see if there is any iso-igrp protocol
     * enabled.
     */
    for (temp_pdb = clns_protoQ.qhead; temp_pdb; temp_pdb = temp_pdb->next) {
        if ((pdb->typeflag & ISO_IGRPMASK) && itf_in_pdb(idb, temp_pdb))
            iso_igrp_in_use = TRUE;
            break;
    }
    /*
     * If IS-IS, check to see if all network layer protocols disabled
     * for interface. Assume there is only one ISIS pdb.
     */
    if (pdb->typeflag & ISO_ISISMASK) {
	if (isis_checkqueue(&pdb->osi_idbq, idb)) return(FALSE);
	if (isis_checkqueue(&pdb->ipx_idbq, idb)) return(FALSE);
	if (!iso_igrp_in_use)
	    idb->clns_level = 0;
	if (isis_checkqueue(&pdb->ip_idbq, idb)) return(FALSE);
    }

    /*
     * Invalidate the cache just in case we have entries which point
     * at this interface.
     */
    clns_invalidate_cache("clns_delete_intfc_router", TRUE);

    /*
     * If there are any route or neighbor entries which
     * use this interface, remove them, too.
     */
    clns_remove_idb_depend(idb);
    clns_remove_idb_from_queue(pdb, idb);

    /*
     * If this is the isis pdb, turn off clns on this interface
     * LATER - remove all neighbors learned over this interface.
     */
    if (pdb->typeflag & ISO_ISISMASK) {

	/*
	 * remove all the information about isis from the idb.
	 */
	isis_reset_idb_params(pdb, idb, TRUE);
	if (idb->isis_idb) {
	    isisidb =
		idb->isis_idb[isis_pdbindex_to_procindex(CLNS_PDBINDEX_ISIS0)];
	    free(isisidb);
	    idb->isis_idb[isis_pdbindex_to_procindex(CLNS_PDBINDEX_ISIS0)] =
		NULL;
	}

        if (!iso_igrp_in_use) {
	    if (idb->clns_enabled)
		clns_itf_disable(idb);
	}

	/*
         * Reset clns_myNET to 1st entry in local_nets Q. This will prevent
	 * clns_myNET from containing stale NET value. 
	 */
	clns_refresh_my_net();
	return(TRUE);
    }

    /*
     * Should we remove this itf from the domain pdb ?
     * (If this itf is in any other area in this domain, leave it
     *
     * If this itf is not in ANY area or domain, disable clns processing
     * on the interface.
     */
    iso_igrp_in_use = FALSE;
    nsap_copy(pdb->net, net);
    for (temp_pdb = clns_protoQ.qhead; temp_pdb; temp_pdb = temp_pdb->next) {
        if (temp_pdb->typeflag  & ISO_IGRPMASK) {
	    if (temp_pdb->level & CLNS_LEVEL1) {
	        if (clns_domain_match(temp_pdb->domain_length,
				      temp_pdb->domain,
				     net[0] - STATION_LENGTH - AREA_LENGTH - 1,
				      &net[1])){
		    /* 
		     * This is another area in the same domain.
		     */
		    if (itf_in_pdb(idb, temp_pdb)) return(TRUE);
	        } else {
		    /* 
		     * This is an area from another domain
		     * (Note that all interfaces that are in a domain
		     * will be in one of the areas so we don't need to
		     * check the domain level pdbs).
		     */
		    if (itf_in_pdb(idb, temp_pdb)) {
		        iso_igrp_in_use = TRUE;
			break;
		    }
	        }
	    } else {
                if (temp_pdb != pdb && itf_in_pdb(idb, temp_pdb)) {
                    iso_igrp_in_use = TRUE;
                    break;
                }
            } 
        } else {
	    if (itf_in_pdb(idb, temp_pdb) && (temp_pdb != pdb)) {
                if (isis_checkqueue(&pdb->osi_idbq, idb)) {
		    clns_isis_in_use = TRUE;
	            break;
		} else
		    ip_isis_in_use = TRUE;
	    }
        }
    }

    /*
     * Yes, we need to remove the itf from the domain pdb.
     */
    pdb = clns_find_level2_pdb(net);
    if (pdb){
	clns_remove_idb_from_queue(pdb, idb);
	clns_remove_idb_depend(idb);
	/*
	 * Do we need to disable clns processing on this interface ???
	 */
	if (!iso_igrp_in_use && !clns_isis_in_use && !ip_isis_in_use) {
	    if (idb->clns_enabled)
		clns_itf_disable(idb);
	} else if (!iso_igrp_in_use && !clns_isis_in_use && ip_isis_in_use)
	    idb->clns_level = 0;
    }
    clns_refresh_my_net();

    return(TRUE);
}

/*
 * clns_clear_prefix
 *
 * Clear all of the prefix routes. The 'what_to_clear' mask specifies
 * the protocol source for the prefix routes to clear.
 */

static void clns_clear_prefix (
    ulong what_to_clear)
{
    clns_ddbtype *ddb;
    clns_rdbtype *rdb;
    int          i;
    boolean      freed;
    
    freed = FALSE;
    for (ddb = nsap_next(NULL); ddb; ddb = nsap_next(ddb)) {
	if (ddb->entry_type == CLNS_ROUTE_ENTRY) {
	    for (i = 0; i < ddb->routecount; i++) {
		rdb = ddb->rdb[i];
		if (rdb->protocol_source & what_to_clear) {
		    clns_delete_rdb(ddb, i);
		    freed = TRUE;
		    i--;	/* Since we shuffled... */
		}
	    }
	    if (freed) {
		freed = FALSE;
		if (what_to_clear & ISO_IGRPMASK) {
		    clns_flash_update(what_to_clear);
		}
	    }
	}
    }
}

/*
 * clns_clear_routing
 *
 * Clear routes for a particular ISO-IGRP process.
 */
static void clns_clear_routing (
    clns_pdbtype *pdb,
    boolean exclude_static)
{
    clns_ddbtype *ddb, *next, *prev;
    int          i;

    for (i = 0; i < NETHASHLEN; i++) {
	prev = NULL;
	for (ddb = pdb->dest_routing_table[i]; ddb; ddb = next) {
            next = ddb->next;

	    /*
	     * Don't delete our own DDBs
	     */
	    
	    if ((ddb->metric == 0) && 
                ((ddb->entry_type == CLNS_OURNET_ENTRY) ||
                 (ddb->entry_type == CLNS_OURNSAP_ENTRY))) {
		prev = ddb;
		continue;
	    }

	    if (exclude_static &&
		ddb->rdb[0] && 
		(ddb->rdb[0]->protocol_source == ISO_STATIC))
		continue;

	    /* 
	     * DDB is not ours - delete it
	     */
	    clns_delete_all_rdbs(ddb);

	    if (prev == NULL) 
		pdb->dest_routing_table[i] = ddb->next;
	    else 
		prev->next = ddb->next;

            ddb->next = NULL;
	    free(ddb);		
	}
    }
}


/* 
 * clns_delete_router
 *
 *  Delete this specified router process.
 *   1) Find pdb and remove it from the pdbq.
 *   2) Remove this area from all of the interfaces.
 *   3) Scan the idb queue and remove the interface CLNS NET if the NET
 *      matches the NET being deleted.
 *   4) Kill the process.
 *   5) Deallocate all of the space associated with the pdb.
 *   6) Deallocate the pdb.
 */
boolean clns_delete_router (char *router_name)
{
    clns_pdbtype *pdb, *pdb2, *rpdb;
    idbqueuetype *idbqueue, *next;
    paktype      *pak;
    idbtype      *idb;
    boolean      isis;
    int          i;
    NSAP_T       id;
    uchar        *area;
    clns_ddbtype *dest;
    boolean free_pdb2;
    free_pdb2 = FALSE;			/* Argh, this code is so hosed. */

    pdb = (clns_pdbtype *)clns_find_router_name(router_name);
    if (!pdb) {
	printf("\nCannot find router %s", router_name);
	return(FALSE);
    }

    isis = (pdb->typeflag & ISO_ISISMASK);

    if (!isis && (pdb->level & CLNS_LEVEL2)) {
	printf("\nCannot delete level 2 IGRP router: %s",router_name);
	return(FALSE);
    }

    clns_set_pdb_running(pdb, FALSE);

    /*
     * Remove interfaces from protocol specific queues.
     */
    if (isis) {
	while ((idbqueue = dequeue(&pdb->ip_idbq))) {
	    free(idbqueue);
	}
	while ((idbqueue = dequeue(&pdb->osi_idbq))) {
	    free(idbqueue);
	}
	while ((idbqueue = dequeue(&pdb->ipx_idbq))) {
	    free(idbqueue);
	}
    }

    /* 
     * Remove this area from all of the interfaces.
     */
    idbqueue = pdb->idbqueue;
    while (idbqueue) {
	next = idbqueue->next;
	clns_delete_intfc_router(pdb, idbqueue->idb);
	idbqueue = next;
    }    

    /* 
     * Any interface NET that matches the global NET being deleted
     * must be set to NULL
     */
    FOR_ALL_SWIDBS(idb) {
        if (!idb->clns_net) continue;
	if (pdb->net[0] && nsap_equ(idb->clns_net, pdb->net))
	    idb->clns_net = NULL;  
    }

    /*
     * Remove pdb from the queue and kill the process.
     */
    unqueue(&clns_protoQ, pdb);
    if (pdb->clns_pid != NO_PROCESS) {
	process_kill(pdb->clns_pid);
	pdb->clns_pid = NO_PROCESS;
    }

    /*
     * Kill the main timer and any subordinates.  We cheat and check to
     * see if the timer is initialized before doing so.  This is because
     * the "no router xxx" command actually creates a PDB before calling
     * the delete routine!  Too big of a mess to undo.  Meantime, we
     * detect this condition so that we don't generate a message.
     */
    if (mgd_timer_initialized(&pdb->timer))
	mgd_timer_stop(&pdb->timer);

    if (isis) {

	/*
	 * Deallocate memory used for link state databases as well as
	 * the level-1 routing table. The IS-IS routes in the level-2 routing 
         * table will age naturally.
	 */
	isis_free_resources(pdb);

	/*
	 * If IP pdb exists, free it.
	 */
	if (pdb->ip_pdb) {
	    ip_router_erase(pdb->ip_pdb);
	    unqueue(&protoQ, pdb->ip_pdb);
	    free_pdbindex(pdb->ip_pdb);
	    pdb->ip_pdb = NULL;
	}

	if (pdb->update_pid != NO_PROCESS) {
	    process_kill(pdb->update_pid);
	    pdb->update_pid = NO_PROCESS;
	}

	/* Stop any lingering timers. */

	mgd_timer_stop(&pdb->update_timer);

    } else {

	/*
	 * Clear routing information from this pdb.
	 */
	clns_clear_routing(pdb, FALSE);
    }

    /*
     * Deallocate the name string.
     */
    free(pdb->pname);
    pdb->pname = NULL;

    /*
     * Deallocate strings in route-maps.
     */
    for (i = 0; i < CLNS_MAXROUTERS; i++) {
	if (pdb->routemap[i]) free(pdb->routemap[i]);
    }

    /*
     * Remove this NET from the prefix routing table.
     */
    if (isis) {
        for (i = 0; i < pdb->num_areas; i++) {
	    nsap_delete(pdb->area_address[i]);
        }
    } else {
	if (pdb->net[0]) nsap_delete(pdb->net);
    }

    /*
     * Remove any stray input packets.
     */
    while (pdb->clns_inputq.qhead){
	pak = dequeue(&pdb->clns_inputq);
	retbuffer(pak);
    }
    if (pdb->clns_watched_inputq) {	/* New style? */
	while (!process_is_queue_empty(pdb->clns_watched_inputq)) {
	    pak = process_dequeue(pdb->clns_watched_inputq);
	    retbuffer(pak);
	}
    }
    if (isis) {
	while (!process_is_queue_empty(pdb->update_inputq)) {
	    pak = process_dequeue(pdb->update_inputq);
	    retbuffer(pak);
	}
    }
    /* 
     * Do we need to delete the process for the domain, too ???
     */
    if (isis) {
	isis_cleanup_pdb(pdb);
	free(pdb);
	return(TRUE);
    }

    pdb2 = clns_find_level2_pdb(pdb->net);
    if (pdb2 && !clns_found_level1_pdb(pdb2)) {
	unqueue(&clns_protoQ, pdb2);
	if (pdb2->clns_pid != NO_PROCESS) {
	    process_kill(pdb2->clns_pid);
	    pdb2->clns_pid = NO_PROCESS;
	}
	free_pdb2 = TRUE;
	clns_clear_routing(pdb2, FALSE);
	clns_clear_prefix(pdb2->typeflag);
	/*
	 * remove this pdb->typeflag from any redistribute lists
	 */
	for (rpdb = clns_protoQ.qhead; rpdb; rpdb = rpdb->next){
	    rpdb->redistallowed &= ~pdb2->typeflag;
	}
	free(pdb2->pname);
	pdb2->pname = NULL;
    } else {

	/*
	 * The domain process needs to stick around - delete our route to
	 * the area we just deleted.
	 */
	if (pdb2 && pdb && pdb->net[0]) {
	    id[0] = AREA_LENGTH;
	    area = get_area(pdb->net);
	    for (i = 1; i <= AREA_LENGTH; i++) {
		id[i] = area[i-1];
	    }
	    dest = clns_find_dest(pdb2, id);
	    if (dest && dest->entry_type == CLNS_OURNET_ENTRY) {
	        for (i = 0; i < dest->routecount; i++) {
		    iso_igrp_route_delete(pdb, dest, i);
		}
	    }
	}
    }

    /*
     * If there aren't any more routing processes, clear the
     * clns_router_running flag.
     */
    if (clns_protoQ.qhead == NULL) clns_router_running = FALSE;

    /*
     * Should I turn off clns, too ???
     */
    free(pdb);
    if (free_pdb2)
	free(pdb2);
    return(TRUE);
}

/* clns_found_level1_pdb
 *
 * Find a Level 1 pdb that is in the same domain as the Level 2 pdb passed in. 
 * If one is found, return TRUE.
 */
boolean clns_found_level1_pdb (clns_pdbtype *pdb)
{
    clns_pdbtype *pdb_temp;

    /* 
     * Don't bother if we are passed in a Level 1 pdb.
     */
    if (pdb->level & CLNS_LEVEL1) {
        return(FALSE);
    }

    for (pdb_temp = clns_protoQ.qhead; pdb_temp; pdb_temp = pdb_temp->next) {

	if (pdb_temp->level & CLNS_LEVEL2) {
	    continue;
	}
	
	if (clns_domain_match(pdb->domain_length, pdb->domain,
			      pdb_temp->domain_length, pdb_temp->domain)) {
	    return(TRUE);
	}
    }

    return(FALSE);
}
	    

/*
 * clns_remove_neighbor_depend
 *
 *    We are about to remove a neighbor adjacency.
 *    First delete all of the routes that we learned from this neighbor
 *
 */
void clns_remove_neighbor_depend (clns_pdbtype *pdb, NSAP_T next_hop)
{
    clns_ddbtype *ddb;
    clns_rdbtype *rdb;
    boolean      found_one;
    int          i, j;

    found_one = FALSE;

    /*
     * Do system id and area routes from pdb.
     */
    if (pdb) {
	for (i = 0; i < CLNS_NETHASHLEN; i++) {
	    for (ddb = pdb->dest_routing_table[i]; ddb; ddb = ddb->next) {
		for (j = 0; j < ddb->routecount; j++) {
		    rdb = ddb->rdb[j];
		    if (nsap_equ(rdb->first_hop, next_hop)) {
			found_one = TRUE;
			if (ddb->entry_type == CLNS_ROUTE_ENTRY) {
			    clns_flash_update(rdb->protocol_source);
			}
			iso_igrp_route_delete(pdb, ddb, j);
		    }
		}
	    }
	}
	return;
    }

    /*
     * Do prefix routes.
     */
    for (ddb = nsap_next(NULL); ddb; ddb = nsap_next(ddb)) {
   	for (i = 0; i < ddb->routecount; i++) {
	    rdb = ddb->rdb[i];
	    if (nsap_equ(rdb->first_hop, next_hop)) {
		found_one = TRUE;
		if (ddb->entry_type == CLNS_ROUTE_ENTRY) {
		    clns_flash_update(rdb->protocol_source);
		}
		if (ddb->entry_type != CLNS_IS_ENTRY &&
		    (rdb->protocol_source & ISO_IGRPMASK)) {
		    if (ddb->routecount == 1) {
			ddb->metric = CLNS_METRIC_INACCESSIBLE;
			TIMER_START(ddb->blindtimer, ISO_IGRP_BLINDTIME);
			clns_manage_prefix_nbrs(ddb, rdb->protocol_source,
						FALSE);
		    }
		    continue;
		}

		/*
		 * This free is for IS-IS routes only.
		 */
		clns_delete_rdb(ddb, i);
		i--;		/* We shuffled... */
	    }
	}
    }

    if (found_one) {
	clns_invalidate_cache("clns_remove_neighbor_depend", TRUE);
    }
}

/*
 * clns_itf_enable
 *
 * Enable an interface for CLNS.
 */
void clns_itf_enable (idbtype *idb)
{
    /*
     * Note:clns_macaddrs_update() will check if (clns_running && idb->clns_enabled),
     *      so set them to TRUE before calling the function.
     */
    idb->clns_enabled = TRUE;
    clns_running = TRUE;
    clns_router_running = TRUE;
    if (idb->hwptr->afilter_reset)
	(*idb->hwptr->afilter_reset)(idb->hwptr);
    else
	clns_macaddrs_update(idb);
    /*
     * Enable routing of CLNS by MCI hardware, and reset the interface
     * so the CLNS ethernet multicasts are enabled.
     */
    if (max_proto_mtu(idb, LINK_CLNS) < idb->clns_mtu) {
	idb->clns_mtu = max_proto_mtu(idb, LINK_CLNS);
    }

    clns_rxtypes_update(idb);
    clns_nnets++;

    /*
     * Start processes if not already started.
     */
    start_clns_processes();

    /* 
     * Store for use in case we never hear from an IS, and have to use
     * configuration request method. 
     */
    dummy_adj.idb = idb; 
    clns_fastsetup(idb->hwptr);

    /* Start the hello timer. */

    mgd_timer_start_jittered(&idb->clns_hello_timer, ONESEC, CLNS_JITTER_PCT);
    idb->clns_sched_hello_time = mgd_timer_exp_time(&idb->clns_hello_timer);
}

/*
 * clns_itf_disable
 *
 * Disable an interface for CLNS.
 */
void clns_itf_disable (idbtype *idb)
{
    /*
     * Note:clns_macaddrs_update() will check if (clns_running && idb->clns_enabled),
     *      so reset idb->clns_enabled before calling the function.
     */
    idb->clns_enabled = FALSE;
    if (idb->hwptr->afilter_reset)
	(*idb->hwptr->afilter_reset)(idb->hwptr);
    else
	clns_macaddrs_update(idb);
    clns_rxtypes_update(idb);
    idb->clns_level = 0;
    clns_nnets--;
}


/*
 * clns_show_protocol_source
 *
 */
void clns_show_protocol_source (ulong protocol_source)
{
    if (protocol_source & ISO_STATIC) {
	printf("Static");
    } else if ((protocol_source & ISO_ISISMASK) && 
	       (protocol_source & ISO_IGRPMASK)) {
	printf("IS-IS/IGRP");
    } else if (protocol_source & ISO_ISISMASK) {
	printf("IS-IS");
    } else if (protocol_source & ISO_IGRPMASK) {
	printf("ISO-IGRP");
    } else if (protocol_source & (DECNET_PHASEIV | ISO_ESIS)) {
 	if (protocol_source == DECNET_PHASEIV) {
 	    printf ("Decnet");
 	} else if (protocol_source == ISO_ESIS) {
	    printf ("ES-IS");
 	} else {
 	    printf ("ES-IS/Decnet");
 	}
    } else {
	printf("%10s", "Unknown");
    }
}

/*
 * clns_show_one_route
 *
 * Called by show_clns_routes. ddb is locked before this routine is called.
 *
 */
void clns_show_one_route (clns_ddbtype *ddb)
{
    clns_pdbtype *pdb;
    clns_rdbtype *rdb;
    clns_adjtype *adj;
    char         *str;
    int          i;

    printf("\n%s", (ddb->destination[0]) ?
	   nsap_string(ddb->destination) : "Default Prefix");

    switch (ddb->entry_type) {
    case CLNS_NULL_ENTRY:
	printf(", Null");
	return;
    case CLNS_OURNSAP_ENTRY:
	printf(", Local NSAP Entry");
	return;
    case CLNS_OURNET_ENTRY:
	printf(", Local NET Entry");
	return;
    case CLNS_DISCARD_ENTRY:
	printf(", Discard Entry");
	adj = find_adjacency(clns_adjacency_db, ddb->destination, FALSE,
			     NULL, 0);
	if (adj) {
	    printf(", %s", (adj->state == CLNS_ADJ_UP_STATE) ? "Up" : "Down");
        }
	return;
    case CLNS_DECNET_DISCARD_ENTRY:
	printf(", DECnet discard Entry");
	adj = find_adjacency(clns_adjacency_db, ddb->destination, FALSE,
			     NULL, 0);
	if (adj) {
	    printf(", %s", (adj->state == CLNS_ADJ_UP_STATE) ? "Up" : "Down");
        }
	return;

    case CLNS_REDIRECT_ENTRY:
	printf(", Redirect Entry");
	return;
    case CLNS_ROUTE_ENTRY:
	if (ddb->metric == CLNS_METRIC_INACCESSIBLE) {
	    printf(" [%d/Infinity]", ddb->distance);
	} else {
	    printf(" [%d/%d]", ddb->distance, ddb->metric);
	}
	break;
    case CLNS_ES_ENTRY:
	printf(" ES-IS Entry");
	break;
    default:
	printf(", Unknown Entry Type");
	return;
    }

    if (ddb->routecount > MAXROUTES)
	printf("\nInvalid route count %d", ddb->routecount);

    for (i = 0; i < MAXROUTES; i++) {
	rdb = ddb->rdb[i];
	if (!rdb) {
	    if (ddb->routecount > i)
		printf("\n   [invalid null entry]");
	    continue;
	} else {
	    if (ddb->routecount <= i)
		printf("\n   [invalid non-null entry]:");
	}
	mem_lock(rdb);

	printf("\n  ");

	/*
	 * Print next hop address.
	 */
	adj = find_adjacency(clns_adjacency_db, rdb->first_hop, FALSE,
			     rdb->idb, 0);
	if (nsap_equ(ddb->destination, rdb->first_hop)) {
	    if (adj) {
		if (is_p2p(adj->idb)) {
		    printf("via %s, Static%s", adj->idb->namestring,
			   interface_up(adj->idb) ? "" : " (Down)");
		} else {
		    printf("via %s, %s, Static%s", snpa_string (&adj->snpa),
			   adj->idb->namestring,
			   interface_up(adj->idb) ? "" : " (Down)");
		}
	    }
	    free(rdb);
	    continue;
	} else {
	    if (rdb->first_hop[0] == STATION_LENGTH) {
		str = clns_stationid_lookup(NULL, &rdb->first_hop[1]);
	    } else {
		str = clns_address_lookup(rdb->first_hop);
		if (!str) str = nsap_string(rdb->first_hop);
	    }
	    printf("via %s, ", str); 
	}

	/*
	 * Print protocol source and state, if applicable.
	 */
	if (rdb->protocol_source == ISO_STATIC) {
	    printf("Static");
	} else if (rdb->protocol_source & ISO_IGRPMASK) {
	    printf("ISO-IGRP");
	} else if (rdb->protocol_source & ISO_ISISMASK) {
	    printf("IS-IS");
	} else if (rdb->protocol_source & ISO_ESIS) {
	    printf("ES-IS");
	} else {
	    printf("?");
	}

	/*
	 * Print state and next hop interface.
	 */
	if (rdb->protocol_source & ISO_ISISMASK) {
	    if (!(pdb = find_isis_pdb())) return;
	    printf(", %s", (pdb->current_L2_version == rdb->route_version) ?
		   "Up" : "Down");
	} else if (rdb->protocol_source & (ISO_IGRPMASK | ISO_ESIS)) {
	    printf(", %s", !TIMER_RUNNING(ddb->blindtimer) ? "Up" :
		   ((SLEEPING(ddb->blindtimer)) ? "HoldDown" : "Down"));
	}
	if (adj && adj->idb) {
	    printf(", %s", adj->idb->namestring);
	}
	free(rdb);
    }

}

/*
 * clns_show_pdb_chain
 * Walk the pdb chain, showing all iso igrp processes or a particular
 * process. This is an interactive routine intended for "show" commands.
 *
 */
void clns_show_pdb_chain (char *target, boolean (*rtn)(clns_pdbtype *))
{
    clns_pdbtype *pdb;
    boolean      onceflag;

    if (*target) uppercase(target);
    onceflag = FALSE;

    automore_enable(NULL);
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (!validmem(pdb)) return;

	if (*target && (strcasecmp(target, pdb->pname))) continue;

	if (onceflag) {
	    if (*target != '\0') {
		automore_disable();
		return;
	    }
	    automore_conditional(0);
	} else {
	    onceflag = TRUE;
	}
	onceflag = (*rtn)(pdb);
    }
    automore_disable();
}

/*
 * clns_display_redistribute
 *
 *    Display the <tag> for each set of prefix routes we are redistributing
 */
static void clns_display_redistribute (
    ulong redistallowed)
{
    clns_pdbtype *pdb;

    printf("\n  Redistribute:");
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(redistallowed & pdb->typeflag)) continue;
	printf("\n    %s (%s)", pdb->name, 
	       (pdb->pname) ? pdb->pname : "Null Tag");
    }
}

/*
 * clns_show_isis
 *
 */
static void clns_show_isis (
    clns_pdbtype *pdb)
{

    idbqueuetype *idbqueue;
    clns_pdbtype *pdb2;
    int          i;
    isisidbtype  *isisidb;

    printf("\nIS-IS Router: %s", (pdb->pname) ? pdb->pname : "<Null Tag>");
    printf("\n  System Id: %s  ", systemid_string((uchar *)pdb->systemId));
    printf("IS-Type: %s", (pdb->level == ISIS_CIRCUIT_L1L2) ? isis_L1L2_str :
	   (pdb->level == ISIS_CIRCUIT_L1) ? isis_L1_str : isis_L2only_str);

    /* Display areas. */

    printf("\n  Manual area address(es): ");
    for (i = 0; i < pdb->num_areas; i++) {
	printf("\n\t%s", long_area_string(pdb->area_address[i]));
    }	
    printf("\n  Routing for area address(es): ");
    for (i = 0; i < pdb->num_active_areas; i++) {
	printf("\n\t%n", pdb->active_area_address[i]);
    }	

    /* Display interfaces. */

    if (pdb->idbqueue) {
	printf("\n  Interfaces supported by IS-IS:");
	for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	    printf("\n\t%s", idbqueue->idb->namestring);
	    if (isis_checkqueue(&pdb->osi_idbq, idbqueue->idb)) {
		printf(" - OSI");
	    }
	    if (isis_checkqueue(&pdb->ip_idbq, idbqueue->idb)) {
		printf(" - IP");
	    }
	    if (isis_checkqueue(&pdb->ipx_idbq, idbqueue->idb)) {
		printf(" - IPX");
	    }
	    isisidb = get_isis_idb(pdb, idbqueue->idb);
	    if (isisidb && isisidb->isis_adj_filter) {
		printf("  adj filter %s%s", 
		       filter_name(isisidb->isis_adj_filter),
		       isisidb->match_all_areas ? " (match all)" : "");
	    }
	}
    } else {
	printf("\n  No interfaces in domain/area.");
    }

    /* Display redistribution data. */

    if (pdb->redistallowed == pdb->typeflag) {
	printf("\n  No redistribution performed");
    } else {
	printf("\n  Redistributing: ");
	for (pdb2 = clns_protoQ.qhead; pdb2; pdb2 = pdb2->next) {
	    if (pdb2 == pdb) continue;
	    if ((pdb2->typeflag & ISO_IGRPMASK) && 
		pdb2->level == CLNS_LEVEL2) continue;
	    if (pdb->redistallowed & pdb2->typeflag) {
		printf("\n    %s", pdb2->name);
		if (pdb2->typeflag != ISO_STATIC) {
		    printf(" (%s)", (pdb2->pname) ? pdb2->pname : "Null Tag");
		}
	    }
	}
    }

    /*
     * Display administrative distance.
     */
    printf("\n  Distance: %d", pdb->distance);

    /* Display partition avoidance state. */

    if (pdb->partition_avoid_on) {
	printf("\n  Partition avoidance enabled (%sactive)",
	       pdb->partition_avoid_ok ? "" : "in");
    }
}

/*
 * clns_show_router
 *
 * This is actually the 'show clns protocol' command.
 */
boolean clns_show_router (clns_pdbtype *pdb)
{

    idbqueuetype *idbqueue;
    boolean      domain_pdb;
    int          area_pos;

    if (pdb->typeflag == ISO_STATIC) return(FALSE);

    /*
     * Display information in pdb for respective protocol.
     */
    if (pdb->typeflag & ISO_ISISMASK) {
	clns_show_isis(pdb);
	return(TRUE);
    } else if (pdb->typeflag & ISO_IGRPMASK) {
	if (pdb->level & CLNS_LEVEL2) {
	    printf("\nISO-IGRP Level 2 Router: %s", pdb->pname);
	    printf("\n  Routing for domain: %s",
		   (uchar *)domain_string(pdb->domain_length, pdb->domain));
	    domain_pdb = TRUE;
	    /*
	     * if this pdb is redistributing any 'extra' routes 
	     * display that fact
	     */
	    if ((pdb->redistallowed - pdb->typeflag) != 0) {
		clns_display_redistribute(pdb->redistallowed - pdb->typeflag);
	    }
	} else {
	    area_pos = pdb->net[0] - STATION_LENGTH - AREA_LENGTH;
	    printf("\nISO-IGRP Level 1 Router: %s", pdb->pname);
	    printf("\n  Routing for domain: %s area: %s",
		   domain_string(pdb->domain_length, pdb->domain),
		   area_string(&pdb->net[area_pos]));
	    domain_pdb = FALSE;
	}

	printf("\n  Sending Updates every %d seconds",
	       pdb->broadcasttime/ONESEC);
	if (mgd_timer_running(&pdb->timer)) {
	    printf(". Next due in %d seconds",
		   mgd_timer_left_sleeping(&pdb->timer)/ONESEC);
	}
	printf("\n  Invalid after %d seconds, Hold down for %d seconds",
	       pdb->invalidtime/ONESEC, pdb->blindtime/ONESEC);

	printf("\n  Sending Router Hellos every %d seconds",
	       pdb->hellotime/ONESEC);
	if (pdb->hellotime){
	    printf(", Next due in %d seconds",
		   TIME_LEFT_SLEEPING(pdb->hello_timer)/ONESEC);
	}
	printf("\n  Invalid after %d seconds",
	       ISO_IGRP_HELLO_DEF_HOLD_TIME/ONESEC);

	printf("\n  IGRP metric weight ");
	printf("K1=%u, K2=%u, K3=%u, K4=%u, K5=%u",
	       pdb->igrp_k1, pdb->igrp_k2, pdb->igrp_k3, 
	       pdb->igrp_k4, pdb->igrp_k5);

    } else {
	printf("\nUnrecognized pdb type");
	return(TRUE);
    }

    /*
     * Display what interfaces this protocol is running over.
     */
    if (pdb->idbqueue) {
	printf("\n  Interfaces in domain/area:");
	for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	    printf("\n\t%s", idbqueue->idb->namestring);
	    if (idbqueue->idb->iso_igrp_adj_filter)
		printf("  adj filter %s", 
		       filter_name(idbqueue->idb->iso_igrp_adj_filter));
	}
    } else {
	printf("\n  No interfaces in domain/area.");
    }

    /*
     * Display administrative distance.
     */
    printf("\n  Distance: %d", pdb->distance);

    return(TRUE);
}

/*
 * clns_walk_ddb
 *
 *    This routine will return a pointer to each entry in the ddb
 *    regardless of its status
 */
clns_ddbtype *clns_walk_ddb (
    clns_pdbtype *pdb,
    int          *hash_index,
    clns_ddbtype *dest)
{
    if (dest){
	dest = dest->next;
	if (!dest)
	    *hash_index += 1;
    }
    while (TRUE){
	if (dest)
	    return(dest);
	while ((dest = pdb->dest_routing_table[*hash_index]) == 0){
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return(NULL);
	}
    }	
}

/*
 * clear_clns_routecommand
 *
 *    this routine is called when the administrator has asked
 *    to clear the entire dynamic routing database
 */
void clear_clns_routecommand (void)
{
    clns_ddbtype *ddb, *next;
    clns_pdbtype *pdb;
  
    /*
     * Delete all of the non-static info in all of the pdbs.
     */
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
 	clns_clear_routing(pdb, TRUE);
    }

    /*
     * Delete the non-static prefix routes.
     */
    for (ddb = nsap_next(NULL); ddb; ddb = next) {
 	next = nsap_next(ddb);
 	if (ddb->entry_type != CLNS_ROUTE_ENTRY) continue;

	/*
	 * Don't delete redistribution entries. ISO-IGRP.
	 */
	if (!ddb->metric) continue;

	/*
	 * Don't delete static routes.
	 */
	if (ddb->rdb[0] && 
	    ddb->rdb[0]->protocol_source == ISO_STATIC) continue;

	nsap_delete(ddb->destination);
    }

    /* Make sure IS-IS runs Dijkstra to repopulate its routes. */

    pdb = find_isis_pdb();
    if (pdb) {
	isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_RTCLEAR, NULL, TRUE);
	isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_RTCLEAR, NULL, TRUE);
    }
}

/*
 * clear_clns_neighbors
 *
 * This routine deletes all of the dynamic adjacencies. Values for
 * type mean:
 *
 * CLEAR_NBRS_ALL = Clear all adjacencies
 * CLEAR_NBRS_IS  = Clear IS adjacencies only
 * CLEAR_NBRS_ES  = Clear ES adjacencies only
 */
void clear_clns_neighbors (int type)
{
    clns_adjtype *adj, *next;
    int          bucket;

    for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
	for (adj = clns_adjacency_db[bucket]; adj; adj = next) {
	    next = adj->next;
	    if (type == CLEAR_NBRS_ES &&
		adj->adjacencyType != CLNS_ES_ADJ_TYPE)
		continue;
	    if (type == CLEAR_NBRS_IS && 
		adj->adjacencyType != CLNS_IS_ADJ_TYPE && 
		adj->adjacencyType != CLNS_L1_IS_ADJ_TYPE && 
		adj->adjacencyType != CLNS_L2_IS_ADJ_TYPE) {
		continue;
	    }
	    if (type == CLEAR_NBRS_ALL &&
		adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE)
		continue;

	    if (adj->protocol_source & DECNET_DISCARD)
		continue;
	    if (!(adj->protocol_source & ISO_STATIC))
		delete_adjacency(clns_adjacency_db, adj);
	}
    }
}

/*
 * clns_show_igrp_routes
 *
 * Display ISO-IGRP routes from either the system id or area routing table.
 *
 */
static const char double_dash[] = {"--"};
static const char systemid_str[] = "\nSystem Id       ";
static const char areaid_str[] = "\nArea Id         ";
static const char route_hdr[] = {"Next-Hop        SNPA                Interface   Metric   State"};
int clns_show_igrp_routes (clns_pdbtype *pdb, int level)
{
    clns_ddbtype *ddb, *nddb;
    clns_rdbtype *rdb;
    clns_adjtype *adj;
    char         *str;
    int          index, i;
    boolean      banner = FALSE;
    boolean      our_entry;

    index = 0;
    ddb = clns_walk_ddb(pdb, &index, NULL);
    while (ddb) {
	mem_lock(ddb);
        our_entry = (ddb->entry_type == CLNS_OURNSAP_ENTRY) ||
		    (ddb->entry_type == CLNS_OURNET_ENTRY);

	for (i = 0; i < MAXROUTES; i++) {
	    if (!banner) {
		printf("\nISO-IGRP Routing Table for Domain %s", 
		       domain_string(pdb->domain_length, pdb->domain));
		if (pdb->level == CLNS_LEVEL1) {
		    printf(", Area %s",
			   area_string(&pdb->net[pdb->domain_length+1]));
		    printf(systemid_str);
		} else {
		    printf(areaid_str);
		}
		printf(route_hdr);
		banner = TRUE;
	    }
	    if (i) {
		str = " ";
	    } else {
		str = (level == CLNS_LEVEL1) ? 
		    clns_stationid_lookup(NULL, &ddb->destination[1]) :
			area_string(&ddb->destination[1]);
	    }

	    if (!(rdb = ddb->rdb[i])) {
		if (i) continue;
		printf("\n%16s%16s%20s%12s", str, double_dash, 
		       double_dash, double_dash);
	    } else {
 		/*
 		 * Print SNPA only if we have any adjacency, and the ddb
		 * is not that of our own entry. This check will take care 
 		 * of the case where we have a static default configured, 
 		 * and we end up printing a bogus SNPA string corresponding to
 		 * our own entry.
 		 */

		adj = find_adjacency(clns_adjacency_db, rdb->first_hop,
				     FALSE, rdb->idb, 0);
		printf("\n%16s%16s%20s%12s", str,
		       clns_stationid_lookup(NULL, &rdb->first_hop[1]), 
 		       (adj && !our_entry) ? snpa_string(&adj->snpa) :
 		                                                  double_dash,
 		       (adj && !our_entry) ? adj->idb->namestring : 
 		                                                  double_dash);
	    }
	    
	    if (ddb->metric >= CLNS_METRIC_INACCESSIBLE) {
		printf("%9s", "Infinity");
	    } else {
		printf("%9d", ddb->metric);
	    }
	    printf("%s", !TIMER_RUNNING(ddb->blindtimer) ? "Up" :
		    ((SLEEPING(ddb->blindtimer)) ? "HoldDown" : "Down"));
	}
	nddb = clns_walk_ddb(pdb, &index, ddb);
	free(ddb);
	ddb = nddb;
    }
    return(banner);
}

/*
 * which_route_command
 *
 * Display next-hop information for a given NSAP destination.
 *
 */
void which_route_command (parseinfo *csb)
{

    clns_adjtype *adj;
    clns_pdbtype *pdb;
    clns_ddbtype *ddb;
    NSAP_T       nsap;
    char         *str;

    nsap_copy(GETOBJ(paddr,1)->clns_addr, nsap);

    ddb = NULL;
    adj = NULL;
    printf("\nRoute look-up for destination %n", nsap);
    str = clns_address_lookup(nsap);
    if (str) printf(", %s", str);

    /*
     * This code duplicates the code from clns_route(). This allows us
     * to get protocol specific information about the route.
     */
    pdb = clns_find_pdb(nsap);
    if (pdb) {
	if ((pdb->typeflag & ISO_ISISMASK) && (pdb->level & ISIS_CIRCUIT_L1)) {
	    adj = isis_route(pdb, nsap);
	    if (adj) {
		printf("\n  Found route in IS-IS level-1 routing table");
	    }
	} else if (pdb->typeflag & ISO_IGRPMASK) {
	    adj = iso_igrp_route(pdb, nsap, ROUTINGDEPTH);
	    if (adj) {
		printf("\n  Found route in ISO-IGRP routing table");
	    }
	}
    } else {
	
	/*
	 * Destination is not in any of our areas. If IS-IS is running,
	 * get closest level-2 router. The closest level-2 router is built 
	 * only when the system is a level-1 only router.
	 *
	 * NOTE: this will have to change when multiple IS-IS processes are 
	 * supported.
	 */
	pdb = find_isis_pdb();
	if (pdb && pdb->isis_best_L2) {
	    adj = isis_make_route_cache(nsap, pdb->isis_best_L2);
	    if (adj) {
		printf("\n  Using route to closest IS-IS level-2 router");
	    }
	}
    }

    /*
     * Do level-2 routing by performing a best match lookup in
     * the CLNS prefix routing table.
     */
    if (!adj) {
	ddb = nsap_lookup(nsap, MATCH_CLOSEST);
	if (ddb) {
	    adj = clns_get_route(nsap, ddb, ROUTINGDEPTH);
	    if (adj) {
		printf("\n  Found route in CLNS prefix routing table");
	    }
	}
    }

    /*
     * Last shot, try the adjacency database, the destination might
     * be directly connected.
     */
    if (!adj) {
	adj = find_adjacency(clns_adjacency_db, nsap, TRUE, NULL, 0);
	if (adj) {
	    printf("\n  Found route in Adjacency database");
	}
    }

    /*	
     * Display adjacency information.
     */
    if (!adj) {
	printf("\n  Route not found");
	return;
    }

    /*
     * Destination is one of the addresses for this system.
     */
    if ((int) adj == -1) {
	printf(" - destination is local");
	return;
    }

    /*
     * If route learned from prefix routing table, display one liner from
     * "show clns route" display.
     */
    if (ddb) {
	printf("\n\nRoute entry used:");
	clns_show_one_route(ddb);

	/*
	 * Interface static route, there is no real adjacency entry.
	 */
	if (nsap_equ(ddb->destination, adj->key))
	    return;
    }

    printf("\n\nAdjacency entry used:");
    printf(nbr_hdr);
    clns_show_one_neighbor(NULL, adj, 'A', TRUE, FALSE);
}

/*
 * clns_metric_command
 *
 * Process router sub-command "metric weights" command.
 *
 * [no] metric weights <tos> <k1> <k2> <k3> <k4> <k5>
 */
void clns_metric_command (clns_pdbtype *pdb, parseinfo *csb)
{
    clns_pdbtype *pdb2;

    /*
     * Get level-2 pdb, and store the same values in it.
     */
    if (!pdb->net[0]) return;
    if (!(pdb2 = clns_find_level2_pdb(pdb->net))) return;

    if (!csb->sense) {
	pdb->igrp_k1 = pdb->igrp_k1_def;
	pdb->igrp_k2 = pdb->igrp_k2_def;
	pdb->igrp_k3 = pdb->igrp_k3_def;
	pdb->igrp_k4 = pdb->igrp_k4_def;
	pdb->igrp_k5 = pdb->igrp_k5_def;
	pdb2->igrp_k1 = pdb->igrp_k1_def;
	pdb2->igrp_k2 = pdb->igrp_k2_def;
	pdb2->igrp_k3 = pdb->igrp_k3_def;
	pdb2->igrp_k4 = pdb->igrp_k4_def;
	pdb2->igrp_k5 = pdb->igrp_k5_def;
	return;
    }

    if (GETOBJ(int,6)) {
	printf("only default QOS (0) supported");
	return;
    }

    pdb->igrp_k1 = GETOBJ(int,1);
    pdb2->igrp_k1 = GETOBJ(int,1);

    pdb->igrp_k2 = GETOBJ(int,2);
    pdb2->igrp_k2 = GETOBJ(int,2);

    pdb->igrp_k3 = GETOBJ(int,3);
    pdb2->igrp_k3 = GETOBJ(int,3);

    pdb->igrp_k4 = GETOBJ(int,4);
    pdb2->igrp_k4 = GETOBJ(int,4);

    pdb->igrp_k5 = GETOBJ(int,5);
    pdb2->igrp_k5 = GETOBJ(int,5);
}

/*
 * clns_basic_timers
 *
 * Process ISO-IGRP basic timers router sub-command.
 *
 * router iso-igrp <tag>
 * basic timers <update-interval> <invalid-time> <holddown-time>
 *
 */
void clns_basic_timers (clns_pdbtype *pdb, parseinfo *csb)
{
    clns_pdbtype *pdb2;

    if (!(pdb->typeflag & ISO_IGRPMASK)) {
	printf("\nBasic timers valid only for ISO-IGRP");
	return;
    }

    /*
     * Get level-2 pdb, and store the same values in it.
     */
    if (!pdb->net[0]) return;
    if (!(pdb2 = clns_find_level2_pdb(pdb->net))) return;

    if (!csb->sense) {
	pdb->broadcasttime = pdb->def_broadcasttime;
	pdb2->broadcasttime = pdb2->def_broadcasttime;
	pdb->invalidtime = pdb->def_invalidtime;
	pdb2->invalidtime = pdb2->def_invalidtime;
	pdb->blindtime = pdb->def_blindtime;
	pdb2->blindtime = pdb2->def_blindtime;
	return;
    }

    pdb->broadcasttime = 
	GETOBJ(int,1) ? (GETOBJ(int,1)*ONESEC) : pdb->def_broadcasttime;
    pdb2->broadcasttime = pdb->broadcasttime;

    pdb->invalidtime =
	GETOBJ(int,2) ? (GETOBJ(int,2)*ONESEC) : pdb->def_invalidtime;
    pdb2->invalidtime = pdb->invalidtime;

    pdb->blindtime = 
	GETOBJ(int,3) ? (GETOBJ(int,3)*ONESEC) : pdb->def_blindtime;
    pdb2->blindtime = pdb->blindtime;
}
