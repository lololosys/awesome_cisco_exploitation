/* $Id: vines_route.c,v 3.12.12.4 1996/08/28 13:19:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_route.c,v $
 *------------------------------------------------------------------
 * vines_route.c - Generic routing support for VINES
 *
 * February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_route.c,v $
 * Revision 3.12.12.4  1996/08/28  13:19:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.12.3  1996/08/07  09:03:49  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.12.12.2  1996/05/11  16:22:18  thille
 * CSCdi51502:  Missed some duplicate strings in vines_route.h
 * Branch: California_branch
 * Save another 392 bytes.
 *
 * Revision 3.12.12.1  1996/03/18  22:32:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.4  1996/03/16  07:53:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.2.3  1996/03/13  02:04:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.10.2.2  1996/03/07  11:04:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  21:46:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/02/14  18:40:29  ehk
 * CSCdi45774:  Vines SRTP servers losing some routes
 * schedule an update to sync up routing table when an interface goes down.
 * changes to reject obsolete route information and always update sequence
 * number for existing routes.
 *
 * Revision 3.11  1996/02/08  20:14:03  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.10  1996/01/22  07:44:44  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1995/12/14  23:47:46  slin
 * CSCdi45347:  Clear vine rout * clears the routes for ever
 * - Reset router sequence number so that we can relearn routes from SRTP
 *   neighbors
 *
 * Revision 3.8  1995/12/08  04:56:22  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.7  1995/12/07  22:33:38  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/01  21:39:14  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.5  1995/11/22  18:55:07  slin
 * CSCdi38892:  vines over X.25 requesting a full routing update on all
 *              PVCs
 * - Send only to newly established PVC
 *
 * Revision 3.4  1995/11/21  01:22:56  slin
 * CSCdi43012:  Route should stay suppressed while its metric changes
 *
 * Revision 3.3  1995/11/17  17:58:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/19  23:41:13  slin
 * CSCdi41922:  Vines Router process suspends too frequently
 *
 * Revision 2.8  1995/09/09  00:54:02  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.7  1995/08/11  20:26:58  slin
 * CSCdi35167:  Vines BADTIMER error
 *
 * - Issue errmsg only if Vines thinks idb is up
 *
 * Revision 2.6  1995/07/27  07:31:08  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.5  1995/07/26 22:40:35  slin
 * CSCdi37704:  Wrong Vines timers stopped
 *
 * Revision 2.4  1995/07/07  20:40:24  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.3  1995/06/23 18:48:53  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/13  18:31:44  slin
 * CSCdi35622:  Vines not correctly notified of FR PVC state changes
 *
 * Revision 2.1  1995/06/07  23:14:52  hampton
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
#include "logger.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "../if/network.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vinesfast.h"
#include "../os/signal.h"
#include "media_registry.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */

uint vines_routes, vines_net_version;
mgd_timer vines_timer, vines_idb_timer, vines_changed, vines_periodic_timer;
ulong vines_slowest_update;
rbTree *vines_network_tree;
boolean vsrtp_enabled;
dqueue_t servers_by_metric;

/*
 * String for inline routine in vines_route.h
 */
const char vsrtp_network_suppression_msg[] = 
    "\nVRTP: network %#Z went into suppression";
const char vines_cannot_find_network_msg[] = 
    "\nV%s: Cannot find network entry for this router.";
const char vines_vines_network_str[] = "Vines Network";

/*
 * Counts kept solely for network management.  Vines NSM, specifically.
 * Some of them could be kept on a per interface basis, but it doesn't
 * seem like its worth the work.
 */
ulong vroute_no_buffer_in;
ulong vroute_no_buffer_out;
ulong vroute_bcast_in;
ulong vroute_bcast_out;
ulong vroute_redirect_modified;
ulong vroute_redirect_created;
ulong vroute_lookup_success;
ulong vroute_lookup_failed;

#define VINT_TIME_ROUTE_START	 0
#define VINT_TIME_ROUTE_END	 1
#define VINT_TIME_NBR_START	 2
#define VINT_TIME_NBR_END	 3
#define VINT_TIME_CACHE_START	 4
#define VINT_TIME_CACHE_END	 5
#define VINT_TIME_PROXY_START	 6
#define VINT_TIME_PROXY_END	 7
#define VINT_TIME_RTP_START	 8
#define VINT_TIME_RTP_END	 9
#define VINT_TIME_MAX		10
sys_timestamp v_interval_time[VINT_TIME_MAX];

#define VINT_COUNT_RTP	0
#define VINT_COUNT_MAX		1
ulong v_interval_count[VINT_COUNT_MAX];

extern inline void vint_record_time (uint index)
{
    if (index >= VINT_TIME_MAX)
	return;
    GET_TIMESTAMP(v_interval_time[index]);
}

extern inline void vint_record_value (uint index, ulong value)
{
    if (index >= VINT_COUNT_MAX)
	return;
    v_interval_count[index] = value;
}


/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vines_route_init
 *
 * Perform any server table specific or route specific initialization.
 */
void vines_route_init (void)
{
    int i;

    vines_routes = 0;
    vines_net_version = 0;
    vines_slowest_update = VRTP_DEF_WAKEUP_INTERVAL;
    vines_network_tree = RBTreeCreate("Vines", "VN", "Network Tree",
				      NULL, &vinesrtp_debug);
    vrtp_init();
    vsrtp_init();
    vsrtp_enabled = FALSE;
    o_init(&servers_by_metric, NULL);
    mgd_timer_init_leaf(&vines_periodic_timer, &vines_timer, VT_PERIODIC,
			NULL, FALSE);
    mgd_timer_start(&vines_periodic_timer, VRTP_DEF_WAKEUP_INTERVAL);

    for (i = 0; i < VINT_TIME_MAX; i++)
	TIMER_STOP(v_interval_time[i]);
    for (i = 0; i < VINT_COUNT_MAX; i++)
	v_interval_count[i] = 0;

    /*
     * Counters kept for NSM
     */
    vroute_no_buffer_in = 0;
    vroute_no_buffer_out = 0;
    vroute_bcast_in = 0;
    vroute_bcast_out = 0;
    vroute_redirect_modified = 0;
    vroute_redirect_created = 0;
    vroute_lookup_success = 0;
    vroute_lookup_failed = 0;
}

/**********************************************************************
 *
 *		       VALIDITY AND CONSISTENCY
 *
 **********************************************************************/

/*
 * vines_unlock_nodes
 *
 * Decrement the reference count on server and neighbor data structures.
 * This allows them to be freed now that there are no longer any explicit
 * routes pointed at them.
 */
void vines_unlock_nodes (routingdecision *explicitroute)
{
    if (!explicitroute)
	return;
    if (explicitroute->server)
	vroute_UnprotectNetwork(explicitroute->server);
    if (explicitroute->neighbor)
	vneigh_UnprotectNeighbor(explicitroute->neighbor);
}

/*
 * vines_lock_nodes
 *
 * Increment the reference count on server and neighbor data structures.
 * This insures that they will never be freed while there is still an
 * explicit route pointed at them.
 */
void vines_lock_nodes (routingdecision *explicitroute)
{
    if (!explicitroute)
	return;
    if (explicitroute->server)
	vroute_ProtectNetwork(explicitroute->server);
    if (explicitroute->neighbor)
	vneigh_ProtectNeighbor(explicitroute->neighbor);
}

/*
 * vines_compute_server_metric
 *
 * Scan the list of all currently valid routes looking for the one with
 * the lowest metric.  Record this metric as the overall metric value for
 * the server.
 */
void vines_compute_server_metric (vinesnettype *server, boolean bump)
{
    vinesroutetype *route;
    vinesnettype *psvr;
    int oldmetric, metric = VINES_INFINITY;
    dqueue_t *qptr, *prev;

    oldmetric = server->actual_metric;
    FOR_ALL_ROUTES(server, route) {
	metric = min(metric, route->metric + route->gw->metric);
    }

    vines_set_server_metrics(server, metric);
    if (metric != oldmetric) {
	if (do_vines_table_debug(server->net, VINES_SERVER))
	    buginf("\nVRTP: server %#Z metric changed from %x to %x",
		   server->net, oldmetric, metric);

	/* Restart supression interval, if necessary */
	if (TIMER_RUNNING_AND_SLEEPING(server->suppresstime))
	    vines_set_suppresstime(server);
	
	/*
	 * bump up the vines_changed timer (VT_CHANGED) or 
	 * simply record server's changed time.
	 */
	if (bump)
	    vines_set_changetime(server);
	else
	    TIMER_START(server->changetime, VRTP_CHANGETIME);
	vines_process_connections();
	lw_remove(&server->by_metric);
	if (server->net != vines_network) {
	    RUN_DQUEUE_W_PREV(servers_by_metric, qptr, prev) {
		psvr = qptr->parent;
		if (psvr->actual_metric < metric)
		    continue;
		if (psvr->actual_metric > metric)
		    break;
		if (psvr->net > server->net)
		    break;
	    }
	    lw_insert(&server->by_metric, prev);
	}
    }
}

/*
 * vines_build_route_list
 *
 * Scan the list of all currently valid routes looking for the one(s)
 * with the same metric as the server entry.  (The previous routine
 * should have already been called to insure that this is the lowest
 * metric.)  Build a circularly linked list of these routes for use by
 * get_next_route().
 */
void vines_build_route_list (vinesnettype *server)
{
    vinesroutetype *route, *last;
    int metric;

    server->best_route = server->primary_route = last = NULL;
    FOR_ALL_ROUTES(server, route) {
	metric = min(VINES_INFINITY, route->metric + route->gw->metric);
	if (metric == server->actual_metric) {
	    if (server->best_route == NULL) {
		server->best_route = route;
		server->primary_route = route;
	    } else {
		last->next_best = route;
	    }
	    last = route;
	} else {
	    route->next_best = NULL;
	}
    }
    if (last)
	last->next_best = server->best_route;
}

/*
 * vines_fixup_server
 *
 * Insure that the server data structure is consistent for the current
 * route metrics. bump up the vines_changed alarm (VT_CHANGED) if bump
 * is TRUE. otherwise, record the server's changed time.
 */
void vines_fixup_server (vinesnettype *server, boolean bump)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    vines_compute_server_metric(server, bump);
    vines_build_route_list(server);
    reset_interrupt_level(level);
}

/*
 * vines_fixup_servers_by_neighbor
 *
 * Insure that all servers data structures are consistent for the current
 * route metrics.  This is generally called after a neighbors metric
 * changed because it heard a better path, or because there is a new path
 * available.
 */
boolean vines_fixup_one_server (treeLink *link, void *dummy)
{
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *neighbor;
    vroute_fn_data *data;

    server = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    neighbor = data->neighbor;

    for (route = server->available_routes; route; route = route->next) {
	if (route->gw != neighbor)
	    continue;
	vines_fixup_server(server, TRUE);
	break;
    }
    return(FALSE);
}

void vines_fixup_servers_by_neighbor (vinesneighbortype *neighbor)
{
    vroute_fn_data data;

    if (!neighbor->is_router)
	return;

    data.neighbor = neighbor;
    vroute_WalkTree(vines_fixup_one_server, &data);
}

/*
 * vines_check_route_list
 *
 * Follow a linked list of routes to make sure that the structures and
 * links are still valid.  Complain loudly if they are not, and attempt
 * to minimize the damage by truncating the list before the error.
 */
boolean vines_check_route_list (vinesnettype *server, vinesroutetype **origin)
{
    vinesroutetype *p, *prev;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    for (prev = (vinesroutetype *)origin, p = *origin; p; prev = p, p = p->next) {
	if (p->magic != VINES_MAGIC_ROUTE) {
	    errmsg(&msgsym(INVALIDROUTE, VINES), "magic number", server->net);
	    prev->next = NULL;
	    reset_interrupt_level(level);
	    return(TRUE);
	}
	if (!validlink(p->next)) {
	    errmsg(&msgsym(INVALIDROUTE, VINES), "link", server->net);
	    p->next = NULL;
	    reset_interrupt_level(level);
	    return(TRUE);
	}
    }
    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * vines_verify_server_structure
 *
 * This routine verifies the contents of the data structure, looking for
 * corrupted data or corrupted pointers.  It corrects problems with the
 * data structure wherever possible.  This routine returns FALSE if the
 * server structure is unusable.  It returns TRUE is the server data
 * structure was valid, or if it has been repaired so that it can be
 * used.
 */
boolean vines_verify_server_structure (vinesnettype *server)
{
    leveltype level;

    /*
     * Check server and first route.
     */
    if (!validmem(server)) {
	buginf("\nvines_verify_server_structure passed invalid pointer");
	return(FALSE);
    }
    if (!server->available_routes)
	return(TRUE);
    if (!validlink(server->available_routes)) {
	server->available_routes = server->best_route = NULL;
	errmsg(&msgsym(CORRUPTENTRY, VINES));
	return(TRUE);
    }
    
    /*
     * Walk both the in use route list and the free route list, checking
     * each one.  Remember that if the in use routes get zapped, the
     * next_best pointer must also be reset.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (vines_check_route_list(server, &server->available_routes))
	vines_fixup_server(server, TRUE);
    reset_interrupt_level(level);
    return(TRUE);
}

/**********************************************************************
 *
 *			  EXTERNAL INTERFACE
 *
 * This section should expand over time.
 *
 **********************************************************************/

ulong vroute_get_route_count (void)
{
    return(vines_routes);
}
ulong vroute_get_lookup_success (void)
{
    return(vroute_lookup_success);
}
ulong vroute_get_lookup_failed (void)
{
    return(vroute_lookup_failed);
}

/**********************************************************************
 *
 *			INSERTION AND DELETION
 *
 **********************************************************************/

/*
 * vines_insert_new_route
 *
 * Given a server node and a route node, insert the new node into the
 * sorted list of nodes currently in use.  The sorting is done based upon
 * the network level address of the gateway for each route. Sorting in
 * necessary to guarantee that any parallel routers maintain the exact
 * same view of the universe.
 */
void vines_insert_new_route (
    vinesnettype *server,
    vinesroutetype *newroute)
{
    vinesroutetype *p, *pred;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    pred = (vinesroutetype *)&server->available_routes;
    p = server->available_routes;
    for ( ; p; pred = p, p = p->next) {
	if (newroute->gw->net < p->gw->net)
	    break;
    }
    newroute->next = p;
    pred->next = newroute;
    server->route_count++;
    reset_interrupt_level(level);
}

/*
 * vines_make_new_route
 *
 * Find or create a route entry for this server.  This routine will first
 * look in the free list of routes associated with this server.  If there
 * are no route structures there, it will malloc enough memory to hold
 * all the current route structures, plus one more.  It then copies all
 * of the old route structures into the new memory block, and frees the
 * old memory block.  A pointer to the remaining space in the new memory
 * block is returned as a place to put the new route.
 */
vinesroutetype *vines_make_new_route (vinesnettype *server)
{
    vinesroutetype *new;

    /*
     * Allocate a memory block for the new route entry, and fill in some
     * of the values.
     */
    if (!(new = malloc_named(sizeof(vinesroutetype), "Vines Route")))
	return(NULL);
    return(new);
}

/*
 * vines_add_route
 *
 * Add a new route to an existing server entry.  This routine first
 * checks to see if the route already exists.  If not, it calls another
 * routine to make space for the new entry, and then copies in all the
 * data.  A second routine takes care of inserting the route into the
 * sorted linked list.
 */
vinesroutetype *vines_add_route (
    vinesnettype *server,
    vinesneighbortype *gw,
    ulong flags,
    ushort metric)
{
    vinesroutetype *existing, *new;

    if (!server || !vines_verify_server_structure(server))
	return(NULL);
    existing = vines_get_route_by_xxx(server, gw, flags, NULL);
    if (existing) {
	existing->metric = metric;
	vines_set_route_timestamp(existing);
	existing->uses = 0;
	existing->flags = flags;
	vines_fixup_server(server, TRUE);
	return(existing);
    }
    if (!(new = vines_make_new_route(server)))
	return(NULL);

    /*
     * Fill in the new information.
     */
    new->magic = VINES_MAGIC_ROUTE;
    vines_set_route_timestamp(new);
    new->uses = 0;
    new->flags = flags;
    new->gw = gw;
    new->metric = metric;
    vines_insert_new_route(server, new);
    vines_fixup_server(server, TRUE);

    /*
     * Housekeeping
     */
    vines_routes++;
    vines_net_version++;
    if (do_vines_table_debug(gw->net, gw->host))
	buginf("\nVRTP: add route to %#Z via %#z",
	       server->net, gw->net, gw->host);
    return(new);
}

/*
 * vines_unlink_route
 *
 * Unlink a specific route from the server entry, and relink it on the
 * list of free routes.  This routine takes the responsibility of
 * insuring server node consistency after moving the route entry.
 */
static inline vinesnettype *vines_unlink_route (
    vinesnettype *server,
    vinesroutetype *prev,
    vinesroutetype *route)
{
    /*
     * Link around this route and then delete it
     */
    prev->next = route->next;
    route->next = NULL;
    route->gw = NULL;
    free(route);

    /* rebuild best_route list here */
    vines_fixup_server(server, TRUE);
    server->route_count--;
    vines_routes--;

    if (server->available_routes == NULL) {
	vines_delete_server_entry(server);
	return(NULL);
    }
    return(server);
}

/*
 * vines_delete_this_route
 *
 * Delete all routes from this server structure but the one specified.
 */
vinesnettype *vines_delete_this_route (
    vinesnettype *server,
    vinesroutetype *delete_this_route)
{
    vinesroutetype *route, *prev;
    boolean changed;

    if (!server)
	return(NULL);
    changed = FALSE;
    FOR_ALL_ROUTES_W_PREV(server, route, prev) {
	if (route != delete_this_route)
	    continue;
	server = vines_unlink_route(server, prev, route);
	if (server == NULL)
	    return(NULL);
	route = prev;
	changed = TRUE;
    }
    if (changed) {
	vines_net_version++;
	vines_fixup_server(server, TRUE);
	vines_flush_cache_by_server(server);
    }
    return(server);
}

/*
 * vines_delete_route_by_xxx
 *
 * Delete an individual route from this server structure.  The
 * unlink_route routine takes care of flusing the FS cache table.
 */
vinesnettype *vines_delete_route_by_xxx (
    vinesnettype *server,
    vinesneighbortype *neighbor,
    ulong flags,
    ulong metric,
    sys_timestamp time)
{
    vinesroutetype *route, *prev;
    boolean changed;

    if (!server)
	return(NULL);
    changed = FALSE;
    FOR_ALL_ROUTES_W_PREV(server, route, prev) {
	if (neighbor && (route->gw != neighbor))
	    continue;
	if (flags && !(route->flags & flags))
	    continue;
	if (metric && (metric >= (route->metric + route->gw->metric)))
	    continue;
	if (TIMER_RUNNING(time) && TIMER_EARLIER(time, route->timer))
	    continue;
	server = vines_unlink_route(server, prev, route);
	if (server == NULL)
	    return(NULL);
	route = prev;
	changed = TRUE;
    }
    if (changed)
	vines_net_version++;
    return(server);
}

static inline vinesnettype *vines_delete_routes_by_flags (
    vinesnettype *server,
    ulong flags)
{
    return(vines_delete_route_by_xxx(server, NULL, flags, 0, timer_no_time));
}

static inline vinesnettype *vines_delete_routes_by_neighbor (
    vinesnettype *server,
    vinesneighbortype *neighbor)
{
    if (!neighbor->is_router)
	return(server);
    return(vines_delete_route_by_xxx(server, neighbor, 0, 0,timer_no_time));
}

static inline vinesnettype *vines_delete_routes (
    vinesnettype *server)
{
    return(vines_delete_route_by_xxx(server, NULL, 0, 0, timer_no_time));
}

/*
 * vines_flush_dynamic_routes
 *
 * Flush all dynamically learned routes to a given network number.  This
 * routine is called from the 'clear vines' command code.
 */
void vines_flush_dynamic_routes (ulong net)
{
    vinesnettype *server;
    vinesroutetype *route;
    vinespathtype *path;
    
    if (!(server = vroute_FindEntry(net)))
	return;
    if (vines_vsrtp_enabled(NULL)) {
	FOR_ALL_ROUTES(server, route) {
	    if (route->gw->version & VSRTP_VERSION) {
		FOR_ALL_PATHS(route->gw, path) {
		    path->origin_timestamp = 0;
		    vsrtp_send_full_request(path->idb, path);
		}
	    }
	}
    }
    server = vines_delete_routes_by_flags(server, (V_NET_RTP|V_NET_TEST));
}

/*
 * vines_flush_routes_by_neighbor
 *
 * Remove all routes that go through the given neighbor node.  This
 * routine is called when a neighbor entry times out due to the lack of
 * hello messages.
 */

boolean vines_flush_one_route (treeLink *link, void *dummy)
{
    vinesnettype *server;
    vinesneighbortype *neighbor;
    vroute_fn_data *data;

    server = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    neighbor = data->neighbor;

    server = vines_delete_route_by_xxx(server, neighbor, data->flag, 0, data->timestamp);
    return(TRUE);
}

void vines_flush_routes_by_neighbor (
    vinesneighbortype *neighbor,
    ulong flags,
    sys_timestamp time)
{
    vroute_fn_data data;

    if (!neighbor->is_router)
	return;
    data.neighbor = neighbor;
    data.flag = flags;
    data.timestamp = time;
    vroute_WalkTree(vines_flush_one_route, &data);
}

/*
 * vines_poison_routes_by_neighbor
 *
 * Remove all routes that go through the given neighbor node.  This
 * routine is called when a neighbor entry times out due to the lack of
 * hello messages.  Note the use of "!TIMER_EARLIER".  This is
 * necessary because the comparison wanted is "equal or later".
 */
boolean vines_poison_one_route (treeLink *link, void *dummy)
{
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *neighbor;
    vroute_fn_data *data;
    boolean entry_changed;

    server = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    neighbor = data->neighbor;

    entry_changed = FALSE;
    FOR_ALL_ROUTES(server, route) {
	if (route->gw != neighbor)
	    continue;
	if (route->flags & V_NET_STATIC)
	    continue;
	if (TIMER_RUNNING(data->timestamp))
	    if (!TIMER_EARLIER(route->timer, data->timestamp)) /* route more recent */
		continue;
	GET_TIMESTAMP(route->gctime);
	route->metric = VINES_INFINITY;
	route->local_timestamp = vines_sequence + 1;
	vines_set_changetime(server);
	if (do_vines_rtp_v_debug(server->net, VINES_SERVER) ||
	    do_vines_table_debug(server->net, VINES_SERVER))
	    buginf("\nVRTP: poisoning entry for %#Z via %#z",
		   server->net, neighbor->net, neighbor->host);
	entry_changed = TRUE;
    }
    if (entry_changed)
	vines_fixup_server(server, TRUE);
    return(TRUE);
}

void vines_poison_routes_by_neighbor (
    vinesneighbortype *neighbor,
    sys_timestamp time)
{
    vroute_fn_data data;

    data.neighbor = neighbor;
    data.timestamp = time;
    vroute_WalkTree(vines_poison_one_route, &data);
}

/*
 * vines_start_aging_one_route
 *
 * Start aging on all routes learned by this neighbor. This bit will
 * be cleared whenever a route is relearned.
 */
static boolean vines_start_aging_one_route (treeLink *link, void *dummy)
{
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *neighbor;
    vroute_fn_data *data;

    server = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    neighbor = data->neighbor;

    FOR_ALL_ROUTES(server, route) {
	if (route->gw != neighbor)
	    continue;
	if (route->flags & V_NET_STATIC)
	    continue;
	/* The order of the following calls is very important... */
	vines_set_route_timestamp(route);
	route->snapshot_force_aging = TRUE;
	if ((vinesrtp_debug & VERBOSE_DEBUG) || vinestable_debug)
	    buginf("\nVRTP: setting entry for aging %#Z via %#z",
		   server->net, neighbor->net, neighbor->host);
    }
    return(TRUE);
}

void vines_start_aging_routes_by_neighbor (
    vinesneighbortype *neighbor)
{
    vroute_fn_data data;

    data.neighbor = neighbor;
    vroute_WalkTree(vines_start_aging_one_route, &data);
}

/*
 * vines_add_server
 *
 * Add a new server entry to the tables.  This routine first looks to see
 * if the server already exists, and if so, returns it.  Otherwise it
 * allocates space for a new entry, fills it in, and links it into the
 * approptiate place in the table.
 */
vinesnettype *vines_add_server (ulong net, ulong flags)
{
    vinesnettype *server;

    /*
     * See if an entry exists for this server.  If not, create a new
     * entry for it.
     */
    server = vroute_FindEntry(net);
    if (server)
	return(server);

    server = vroute_CreateEntry(net);
    if (server == NULL)
	return(NULL);
    server->magic = VINES_MAGIC_SERVER;
    server->flags = flags;
    o_init(&server->by_metric, server);
    TIMER_START(server->changetime, VRTP_CHANGETIME);
    vines_set_server_metrics(server, VINES_INFINITY);

    if (do_vines_table_debug(server->net, VINES_SERVER))
	buginf("\nVRTP: add server %#Z", server->net);
    return(server);
}

/*
 * vines_delete_server_entry
 *
 * Given a VINES server structure, eradicate it from the internal tables.
 */
void vines_delete_server_entry (vinesnettype *server)
{
    vinesroutetype *route;

    if (!server)
	return;

    /*
     * Get rid of all usage of this server in the routing tables
     */
    lw_remove(&server->by_metric);
    vines_flush_cache_by_server(server);
    if (do_vines_table_debug(server->net, VINES_SERVER))
	buginf("\nVRTP: delete server %#Z", server->net);
    vroute_DeleteEntry(server);

    /*
     * Now adjust all the various counts to reflect the deletion of the
     * node.  The paths will be left hanging off of this node in case
     * someone still has a pointer to it.
     */
    FOR_ALL_ROUTES(server, route)
	vines_routes--;
    vines_net_version++;
}

/*
 * vines_delete_server
 *
 * Given a network number, look up the server structure and remove it
 * from the routing tables.
 */
void vines_delete_server (ulong net)
{
    vinesnettype *p;

    p = vroute_FindEntry(net);
    vines_delete_server_entry(p);
}    

/*
 * vroute_update_flags
 *
 * A neighbor's SRTP flags have changed because a new path was learned.
 * Update the flags on only that neighbor's entry in the routing table.
 * Do not update the flags on any other routes reachable via that
 * neighbor, because those flags should have been set by the neighbor.
 */
void vroute_update_flags (vinesneighbortype *neighbor)
{
    vinesnettype *server;
    vinesroutetype *route;

    server = vroute_FindEntry(neighbor->net);
    if (server == NULL)
	return;
    FOR_ALL_ROUTES(server, route) {
	if (route->gw == neighbor)
	    route->rtp_flags = neighbor->srtp_flags;
    }
}


/**********************************************************************
 *
 *				LOOKUP
 *
 **********************************************************************/

static void vines_dump_server (vinesnettype *s)
{
    vinesroutetype *r;
    int depth;

    buginf("\nServer     Parent   Left     Right    Free     nextl    prevl    busy");
    buginf("\n%10Z %08x %08x %08x %08x %08x %08x %d", s->net, s->link.parent, s->link.left,
	   s->link.right, s->link.freeList, s->link.nextl, s->link.prevl, s->link.busy);

    buginf("\n         Server   Magic    cnt avail    best     primary");
    buginf("\n         %08x %08x %03d %08x %08x %08x", s, s->magic, s->route_count,
	   s->available_routes, s->best_route, s->primary_route);

    buginf("\n         Route    next     nextb    magic    gw");
    if (validlink(s->available_routes)) {
	depth = 0;
	FOR_ALL_ROUTES(s, r) {
	    buginf("\n      %2d %08x %08x %08x %08x %08x", depth, r, r->next,
		   r->next_best, r->magic, r->gw);
	    if (!validlink(r->next))
		break;
	    depth++;
	}
    }
}

/*
 * vines_get_route_by_xxx
 *
 * Given a server entry and a set of criteria, find the first entry that
 * matches the criteria.
 */
vinesroutetype *vines_get_route_by_xxx (
    vinesnettype *server,
    vinesneighbortype *gw,
    ulong flags,
    vinesidbtype *idb)
{
    vinesroutetype *route;

    if (server == NULL)
	return(NULL);

    if (!validlink(server->available_routes)) {
	errmsg(&msgsym(BADPARAM,VINES), "get_route_by_xxx", "server", "free memory");
	vines_dump_server(server);
	server->available_routes = NULL;
	return(NULL);
    }	
    FOR_ALL_ROUTES(server, route) {
	if (!validlink(route->next)) {
	    char foo[40];
	    sprintf(foo, "%x", route->next);
	    errmsg(&msgsym(BADPARAM,VINES), "get_route_by_xxx", "route->next", foo);
	    vines_dump_server(server);
	    route->next = NULL;
	    return(NULL);
	}
	if (flags && !(route->flags & flags))
	    continue;
	if (idb && !vines_get_path_by_xxx(route->gw, 0, idb, 0, NULL))
	    continue;
	if (gw && (route->gw != gw))
	    continue;
	return(route);
    }
    return(NULL);
}

/*
 * vines_server_uses_idb
 *
 * See if any of the possible paths to this server go through a specific
 * interface.  This routine checks each route one at a time, calling
 * another routine to check whether or not any of the paths to that
 * neighbor node use this interface.  This routine is called to determine
 * whether or not a redirect packet needs to be sent when a frame is
 * forwarded.  This routine intentionally looks only at route that are in
 * the best path list.
 */
boolean vines_server_uses_idb (
    vinesnettype *server,
    vinesidbtype *idb)
{
    vinesroutetype *route;

    if (!server)
	return(FALSE);
    route = server->best_route;
    do {
	if (vines_best_path_uses_idb(route->gw, idb))
	    return(TRUE);
	route = route->next_best;
    } while (route != server->best_route);
    return(FALSE);
}

/*
 * vines_get_server_metric
 *
 * Return the metric value for this server.  This routine is called by
 * the transport layer to deterime the appropriate initial setting for
 * timeouts.
 */
int vines_get_server_metric (ulong net)
{
    vinesnettype *server;

    server = vroute_FindEntry(net);
    return(server ? server->actual_metric : 0);
}

/*
 * vines_first_hop
 *
 * Determine first hop for the given datagram based upon its destination
 * address.  This routine returns an explicit address that includes not
 * only a pointer to the destination server entry, but a pointer to
 * which route should be used, which neighbor is the next hop, and which
 * specific path to that neighbor should be used.  This information must
 * all be nailed down at the same time so that the best_route and
 * best_path pointers may be moved.  This routine also locks the data
 * structures so that they will not be freed from under it.
 */
boolean vines_first_hop (
    ulong dstnet,
    ushort dsthost,
    routingdecision *howtogetthere)
{
    vinesnettype *server;
    vinesneighbortype *neighbor;

    /*
     * May or may not be local, so assume it isn't and look for a route.
     * Also look for a neighbor entry.  This will use this if possible.
     */
    neighbor = vneigh_FindEntry(dstnet, dsthost);
    if (neighbor && neighbor->newflags & V_N_SELF) {
	howtogetthere->server   = NULL;
	howtogetthere->route    = NULL;
	howtogetthere->neighbor = neighbor;
	howtogetthere->path     = NULL;
	vines_lock_nodes(howtogetthere);
	vroute_lookup_success++;
	return(TRUE);
    }
    if (neighbor && !(neighbor->newflags & V_N_USE_ROUTE)) {
	howtogetthere->server   = NULL;
	howtogetthere->route    = NULL;
	howtogetthere->neighbor = neighbor;
	howtogetthere->path     = vines_get_path(neighbor);
	if (!howtogetthere->path) {
	    vroute_lookup_failed++;
	    return(FALSE);
	}
	if (howtogetthere->path->flags & V_N_PLACEHOLDER) {
	    vroute_lookup_failed++;
	    return(FALSE);
	}
	vines_lock_nodes(howtogetthere);
	vroute_lookup_success++;
	return(TRUE);
    }

    server = vroute_FindEntry(dstnet);
    if (server) {
	howtogetthere->server   = server;
	howtogetthere->route    = vines_get_route(server);
	if (!howtogetthere->route) {
	    vroute_lookup_failed++;
	    return(FALSE);
	}
	howtogetthere->neighbor = howtogetthere->route->gw;
	howtogetthere->path     = vines_get_path(howtogetthere->neighbor);
	if (!howtogetthere->path) {
	    vroute_lookup_failed++;
	    return(FALSE);
	}
	if (howtogetthere->path->flags & V_N_PLACEHOLDER) {
	    vroute_lookup_failed++;
	    return(FALSE);
	}
	vines_lock_nodes(howtogetthere);
	vroute_lookup_success++;
	return(TRUE);
    }
    vroute_lookup_failed++;
    return(FALSE);
}

/*
 * vines_first_hop_test
 *
 * This is essentially the same routine as vines_first_hop(), but it
 * doesn't lock any data structures, or adjust the 'next' pointers.  It
 * returns a simpole boolean value on whether or not there is an
 * available path to the destination.
 */
boolean vines_first_hop_test (ulong dstnet, ushort dsthost)
{
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *neighbor;
    vinespathtype *path;

    neighbor = vneigh_FindEntry(dstnet, dsthost);
    if (neighbor && !(neighbor->newflags & V_N_USE_ROUTE)) {
	path = neighbor->best_path;
	if (path == NULL)
	    return(FALSE);
	if (path->flags & V_N_PLACEHOLDER)
	    return(FALSE);
	return(TRUE);
    }

    server = vroute_FindEntry(dstnet);
    if (server) {
	route = server->best_route;
	if (!route)
	    return(FALSE);
	path = route->gw->best_path;
	if (path == NULL)
	    return(FALSE);
	if (path->flags & V_N_PLACEHOLDER)
	    return(FALSE);
	return(TRUE);
    }
    return(FALSE);
}


/**********************************************************************
 *
 *			     HOUSEKEEPING
 *
 **********************************************************************/

/*
 * vroute_age_one_net
 *
 * Determine if this VINES net entry has any routes that need to be aged.
 * This routine is called by the generic tree code for each node in the
 * VINES network tree.
 */
boolean vroute_age_one_net (treeLink *link, void *dummy)
{
    vinesnettype *p;
    vinesroutetype *prev, *route;
    vroute_fn_data *data;
    boolean entry_changed, result, do_debug;
    ulong interval, gcinterval;
    int metric;

    p = vrouteCast(link);
    data = (vroute_fn_data *)dummy;

    if (p->flags & (V_NET_ME))
	return(TRUE);

    if (data->flag) {
	p = vines_delete_routes_by_flags(p, V_NET_RTP|V_NET_TEST);
	return(TRUE);
    }
    do_debug = do_vines_rtp_v_debug(p->net, VINES_SERVER);
    do_debug |= do_vines_table_debug(p->net, VINES_SERVER);

    if (TIMER_RUNNING_AND_AWAKE(p->changetime))
	TIMER_STOP(p->changetime);
    entry_changed = FALSE;
    FOR_ALL_ROUTES_W_PREV(p, route, prev) {
	if (route->flags & V_NET_STATIC)
	    continue;
	if (route->gw->version == VRTP_VERSION)
	    interval = vines_get_slowest_update_interval(route->gw);
	else
	    interval = -1L;
	metric = min(VINES_INFINITY, route->metric + route->gw->metric);
	if (metric == VINES_INFINITY) {
	    gcinterval = (interval == -1L) ? VRTP_DEF_WAKEUP_INTERVAL : interval;
	    if (CLOCK_OUTSIDE_INTERVAL(route->gctime, VRTP_ROUTEEXPIRE(gcinterval))) {
		if (do_debug)
		    buginf("\nVRTP: garbage collecting entry for %#Z via %#z",
			   p->net, route->gw->net, route->gw->host);
		p = vines_unlink_route(p, prev, route);
		if (p == NULL)
		    return(TRUE);
		route = prev;
		entry_changed = TRUE;
	    }
	    continue;
	}
	if ((route->gw->version == VSRTP_VERSION) || (interval == -1L))
	    continue;
	if (CLOCK_OUTSIDE_INTERVAL(route->timer, VRTP_INVALIDTIME(interval))) {
	    if (vines_best_path_from_delta_or_snapshot(route))
		continue;
	    if (do_debug)
		buginf("\nVRTP: timed out entry for %#Z via %#z",
		       p->net, route->gw->net, route->gw->host);
	    route->gctime = TIMER_ADD_DELTA(route->timer,
					    VRTP_INVALIDTIME(interval));
	    route->metric = VINES_INFINITY;
	    route->local_timestamp = vines_sequence + 1;
	    vines_set_changetime(p);
	    entry_changed = TRUE;
	}
    }

    /*
     * Is it time to come out of suppression?
     */
    if (TIMER_RUNNING_AND_AWAKE(p->suppresstime)) {
	TIMER_STOP(p->suppresstime);
	if (do_debug) {
	    buginf("\nVRTP: network %#Z came out of suppression", p->net);
	}
	FOR_ALL_ROUTES(p, route) {
	    route->local_timestamp = vines_sequence + 1;
	}
	vines_set_changetime(p);
	entry_changed = TRUE;
    }
    
    if (entry_changed) {
	vines_fixup_server(p, TRUE);
	result = TRUE;
    } else {
	result = FALSE;
    }
    process_suspend_if_req();
    return(result);
}

/*
 * vroute_age_all_nets
 *
 * Age each route in the VINES RTP network tree.  This routine simply
 * calls the generic tree routine and lets another function do all the
 * real work.
 */
void vroute_age_all_nets (boolean flushflag)
{
    vroute_fn_data data;
    
    data.flag = flushflag;
    vroute_WalkTree(vroute_age_one_net, &data);
}

/*
 * vroute_interface_came_up
 *
 * This routine collects all the actions that must be performed when an
 * already "up" interface is configured for vines, or an interface
 * already configured for vines comes "up".
 */
void vroute_interface_came_up_work (vinesidbtype *idb)
{
    varp_new_state(idb, VARP_LEARNING);
    vroute_send_full_updates(idb);
    vroute_send_requests(idb);
    idb->vines_update_count = VRTP_DEF_UPDATE_COUNT;
    if (vines_vsrtp_enabled(idb))
	mgd_timer_start(&idb->sync_timer, VSRTP_SYNC_INTERVAL);
    if (vines_is_wan(idb))
	mgd_timer_start(&idb->query_timer, VRTP_QUERYINTERVAL(idb));
    vines_set_wakeup_time(idb);
}

void vroute_interface_came_up (vinesidbtype *idb)
{
    if (!idb->deletion_pending)
	process_send_message(vines_rtr_pid, VINES_SIGNAL_IDB_UP, idb, 0);
}

/*
 * vroute_interface_went_down
 *
 * This routine collects all the actions that must be performed when an
 * already "up" interface is configured for vines, or an interface
 * already configured for vines comes "up".
 */
void vroute_interface_went_down_work (vinesidbtype *idb) {
    vinesidbtype *vidb;
    
    varp_new_state(idb, VARP_OFF);
    vines_flush_cache_by_idb(hwidb_or_null(idb->phys_idb));
    vcache_locked++;
    vines_poison_all_paths_by_idb(idb);
    vroute_send_change_updates(FALSE);

    /* Schedule an update to insure that all neighbors sync up properly. */
    FOR_ALL_VIDBS(vidb) {
	if (!vines_interface_up(vidb))
	    continue;
	mgd_timer_start(&vidb->sync_timer,
		    VRTP_INVALIDTIME(VRTP_DEF_WAKEUP_INTERVAL));
    }

    vines_flush_all_paths_by_idb(idb);
    vcache_locked--;
    mgd_timer_stop(&idb->sync_timer);
    mgd_timer_stop(&idb->query_timer);
    mgd_timer_stop(&idb->wakeup_timer);
    TIMER_STOP(idb->update_start_time);
    TIMER_STOP(idb->update_end_time);
}

void vroute_interface_went_down (vinesidbtype *idb) {
    if (!idb->deletion_pending)
	process_send_message(vines_rtr_pid, VINES_SIGNAL_IDB_DOWN, idb, 0);
}

/*
 * vroute_vc_added
 *
 * This routine collects all the common actions that must be performed
 * when a Frame Relay VC becomes active, or an X.25 PVC or SVC is opened.
 */
void vroute_vc_added_work (vinesidbtype *idb, addrtype *addr)
{
    vinespathtype dummypath;
    vinesneighbortype dummyneighbor;

    if (addr && addr->type == ADDR_VINES) {
	dummyneighbor.net = addr->vines_addr.net;
	dummyneighbor.host = addr->vines_addr.host;
	dummypath.neighbor = &dummyneighbor;
	dummypath.idb = idb;
	vsrtp_send_full_request(idb, &dummypath);
	vrtp_send_request(idb, &dummypath);
    } else {
	vsrtp_send_full_request(idb, NULL);
	vrtp_send_request(idb, NULL);
    }
    idb->vines_update_count = VRTP_DEF_UPDATE_COUNT;
    if (vines_is_wan(idb))
	mgd_timer_start(&idb->query_timer, VRTP_QUERYINTERVAL(idb));
}

void vroute_vc_added (vinesidbtype *idb, addrtype *addr)
{
    vroute_msg *msg;

    if (!idb->deletion_pending) {
	if ((msg = malloc(sizeof(vroute_msg))) == NULL)
	    return;
	msg->idb = idb;
	if (addr != NULL && addr->type != ADDR_ILLEGAL)
	    address_copy(&msg->addr, addr);
	else
	    msg->addr.type = ADDR_ILLEGAL;
	
	process_send_message(vines_rtr_pid, VINES_SIGNAL_VC_ADDED, msg, 0);
    }
}

/*
 * vroute_vc_removed
 *
 * This routine collects all the common actions that must be performed
 * when a Frame Relay VC becomes inactive, or an X.25 PVC or SVC is
 * closed.
 */
void vroute_vc_removed_work (vinesidbtype *idb)
{
    /*
     * Does nothing right now.
     */
}

void vroute_vc_removed (vinesidbtype *idb)
{
    if (!idb->deletion_pending)
	process_send_message(vines_rtr_pid, VINES_SIGNAL_VC_REMOVED, idb, 0);
}

/*
 * Do the work to bring down configuration of an interface (no metric)
 */
static void vines_delete_if_work (vinesidbtype *idb)
{
    vroute_interface_went_down_work(idb);
    idb->vines_lineup = FALSE;
    idb->vines_metric = 0;
    idb->metric_is_default = FALSE;
    idb->svc_slot = 0;
    idb->svc_line = 0;
}

void vines_delete_if (vinesidbtype *idb)
{
    process_send_message(vines_rtr_pid, VINES_SIGNAL_IDB_DELETION_PENDING, idb, 0);
}

/*
 * vines_reload_imminent
 *
 * The router is being reloaded.  Mark *all* routes as unreachable, and
 * send out routing updates.  This should cause the network to
 * reconfigure instead of letting it send into a black hole and waiting
 * for timeouts to reconfigure the network.
 */
boolean vines_reload_entry (treeLink *link, void *dummy)
{
    vinesnettype *server;

    server = vrouteCast(link);
    vines_set_server_metrics(server, VINES_INFINITY);
    /* The next line is to insure that we do not relearn any
       routes before the router actually goes down. */
    TIMER_START(server->suppresstime, 10*ONEMIN);
    return(TRUE);
}

void vines_reload_imminent (void)
{
    if (!vines_running)
	return;

    vroute_WalkTree(vines_reload_entry, NULL);
    vines_running = FALSE;	/* Ignore any responses */
    vroute_send_full_updates(NULL);
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

static const char vines_route_header[]="
Network            Neighbor          Flags     Age      Metric     Uses
";
/*
1234567890123456   1234567890123456  R5*NS.    123       1234  1234567
 1234567890123456  1234567890123456  R5*NS.    123       1234  1234567
*/
static const char vines_self_route[]=
        "\n%#16Z   -                 C%d          -           -        -";

static const char vines_route_header2[]="
Network            Neighbor          Flags     Age      Metric     Uses
                                     Origin    Local     Flags
";
/*
1234567890123456  1234567890123456  R5*NS.    123       1234  1234567
                                    12345678  12345678  1234 */

/*
 * vroute_nettype
 *
 * Return a single character identifying how this network number was
 * learned.
 */
char vroute_nettype (ulong flags)
{
    if (flags & V_NET_ME)
        return('C');
    if (flags & V_NET_STATIC)
        return('S');
    if (flags & V_NET_REDIRECT)
	return('D');
    if (flags & V_NET_RTP)
	return('R');
    if (flags & V_NET_IGRP)
	return('I');
    if (flags & V_NET_TEST)
	return('T');
    return('?');
}

/*
 * vines_display_route_list
 *
 * Display all routes in a given linked list.  This routine is called from
 * vines_display_severs().
 */
void vines_display_route_list (
    vinesnettype *server,
    vinesneighbortype *neighbor,
    int flag,
    boolean verbose)
{
    vinesroutetype *route, *next;
    vinesneighbortype *gw;
    boolean first = TRUE;
    int metric, version;
    ulong time;

    for (route = server->available_routes ; route; route = next) {
	/*
	 * Check command line qualifiers
	 */
	gw = route->gw;
	if (neighbor && (neighbor != gw)) {
	    next = route->next;
	    continue;
	}
	if (gw)
	    vneigh_ProtectNeighbor(gw);
	mem_lock(route);

	/*
	 * Check for degenerate cases.
	 */
	if (route->flags & V_NET_ME) {
	    printf(vines_self_route, server->net, gw->version);
	    if (gw)
		vneigh_UnprotectNeighbor(gw);
	    next = route->next;
	    free(route);
	    continue;
	}

	/*
	 * Print one line for this route in the entry.
	 */
	if (first) {
	    first = FALSE;
	    printf("\n%#16Z ", server->net);
	} else {
	    printf("\n %#16Z", server->net);
	}
	if (gw) {
	    metric = min(VINES_INFINITY, route->metric + gw->metric);
	    version = gw->version;
	    printf("  %#16z", gw->net, gw->host);
	} else {
	    metric = VINES_INFINITY;
	    version = 0;
	    printf("  %16s", "Invalid");
	}
	printf("  %c%d%c%c%c%c    ",
	       /* 1 */ vroute_nettype(route->flags),
	       /* 2 */ version,
	       /* 3 */ ((route == server->best_route) ? '*' : ' '),
	       /* 4 */ flag,
	       /* 5 */ (TIMER_RUNNING_AND_SLEEPING(server->suppresstime) ? 'S' : ' '),	
	       /* 6 */ ((metric != server->actual_metric) ? 'h' : ' '));
	flag = ' ';
	if (route->flags & V_NET_STATIC) {
	    printf("  -");
	} else if ((gw->version == VSRTP_VERSION) ||
		   vines_best_path_from_delta_or_snapshot(route)) {
	    printf("n/a");
	} else {
	    time = ELAPSED_TIME(route->timer)/ONESEC;
	    time = min(time,999);
	    printf("%-3d", time);
	}
	printf("        %04x  %-7d", metric, route->uses);

	/*
	 * If we're being verbose, print a second line with more
	 * information.
	 */
	if (verbose)
	    printf("\n%35s  %08x  %08x   %04x", " ",
		   route->origin_timestamp, route->local_timestamp,
		   route->rtp_flags);

	if (gw)
	    vneigh_UnprotectNeighbor(gw);
	next = route->next;
	free(route);
    }
}

/*
 * vroute_display_one_net
 *
 * Display one entry from the VINES network tree.  This routine is called
 * by the generic tree code for each node in the VINES network tree.
 */
boolean vroute_display_one_net (treeLink *link, void *dummy)
{
    vinesneighbortype *neighbor;
    vinesnettype *p;
    char flag;
    vroute_fn_data *data;
    boolean verbose;

    p = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    neighbor = data->neighbor;

    if (data->net > 0 && (data->net != p->net))
	return(TRUE);
    if (neighbor && !vines_get_route_by_gw(p, neighbor))
	return(TRUE);
    
    flag = (p == data->server) ? (data->flag ? 'N' : 'n') : ' ';
    verbose = (data->net) ? TRUE : FALSE;
    vines_display_route_list(p, neighbor, flag, verbose);
    return(!automore_quit());
}

/*
 * vroute_display_all_nets
 *
 * Display part or all of the VINES network tree.  This routine calls the
 * generic tree code which does all the dirty work of walking the tree
 * and calling the printing subroutine.
 */
void vroute_display_all_nets (
    long net,
    vinesneighbortype *neighbor)
{
    routingdecision waytobanyan;
    vroute_fn_data data;

    vines_nearest_server(&waytobanyan, (boolean *)&data.flag);
    data.server = waytobanyan.server;
    data.net = net;
    data.neighbor = neighbor;

    automore_enable(NULL);
    printf("\n%d servers, %d routes, version %d, next update %d seconds",
	   vines_network_tree->activeNodes, vines_routes, vines_net_version,
	   mgd_timer_left_sleeping(&vines_idb_timer) / ONESEC);
    if (net) {
	automore_header(vines_route_header2);
    } else {
	automore_header(vines_route_header);
    }
    vroute_WalkPartialTree(vroute_display_one_net, NULL, &data);
    automore_disable();
    vines_unlock_nodes(&waytobanyan);
}

/*
 * vroute_display_nets_by_metric
 *
 * Display part or all of the VINES network tree.  This routine calls the
 * generic tree code which does all the dirty work of walking the tree
 * and callin ghte printing subroutine.
 */
void vroute_display_nets_by_metric (
    long net,
    vinesneighbortype *neighbor)
{
    vroute_fn_data data;
    vinesnettype *server;
    dqueue_t *qptr;

    data.server = servers_by_metric.flink->parent;
    data.net = net;
    data.neighbor = neighbor;

    automore_enable(NULL);
    printf("\n%d servers, %d routes, version %d, next update %d seconds",
	   vines_network_tree->activeNodes, vines_routes, vines_net_version,
	   mgd_timer_left_sleeping(&vines_idb_timer) / ONESEC);
    automore_header(vines_route_header);
    RUN_DQUEUE(servers_by_metric, qptr) {
	server = qptr->parent;
	vroute_display_one_net(&server->link, &data);
    }
    automore_disable();
}

void vines_display_times (void)
{
    vinesidbtype *idb;
    idbtype *phys_idb;
    static char *fmt = "\n      %20s: %-4u ms";
    
    printf(fmt, "rcvd rtp",
	   CLOCK_DIFF_UNSIGNED(v_interval_time[VINT_TIME_RTP_START],
			       v_interval_time[VINT_TIME_RTP_END]));
    printf(", %u packets", v_interval_count[VINT_COUNT_RTP]);
    printf(fmt, "age routes",
	   CLOCK_DIFF_UNSIGNED(v_interval_time[VINT_TIME_ROUTE_START],
			       v_interval_time[VINT_TIME_ROUTE_END]));
    printf(fmt, "age neighbors",
	   CLOCK_DIFF_UNSIGNED(v_interval_time[VINT_TIME_NBR_START],
			       v_interval_time[VINT_TIME_NBR_END]));
    printf(fmt, "age cache",
	   CLOCK_DIFF_UNSIGNED(v_interval_time[VINT_TIME_CACHE_START],
			       v_interval_time[VINT_TIME_CACHE_END]));
    printf(fmt, "age proxy",
	   CLOCK_DIFF_UNSIGNED(v_interval_time[VINT_TIME_PROXY_START],
			       v_interval_time[VINT_TIME_PROXY_END]));

    FOR_ALL_VIDBS(idb) {
	phys_idb = idb->phys_idb;
	if (idb == vinesnullidb)
	    continue;
	if (!interface_up(phys_idb) || !idb->vines_metric)
	    continue;
	if (!TIMER_RUNNING(idb->update_start_time))
	    continue;
	printf(fmt, phys_idb->namestring,
	       CLOCK_DIFF_UNSIGNED(idb->update_start_time,
				   idb->update_end_time));
    }
}

/*
 * vroute_ShowStats
 *
 * Show statistical information on the vines network tree.
 */
void vroute_ShowStats (parseinfo *csb)
{
    if (vines_network_tree)
	RBTreeStats(vines_network_tree, "VINES network");
    else
	printf("\nVINES network tree does not exist.");
}

/*
 * vroute_decode_packet
 *
 * Print out the RPC header information in a packet.
 */
uchar *vroute_decode_packet (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vsrtp_header *srtp;

    srtp = (vsrtp_header *)data;
    if (srtp->version > VSRTP_VERSION_THRESH) {
	return(vrtp_decode_packet(data, limit, fn));
    } else if (srtp->version == VSRTP_VERSION) {
	return(vsrtp_decode_packet(data, limit, fn));
    } else {
	(*fn)("\n   RTP version %x", srtp->version);
	*limit -= sizeof(ushort);
	return((uchar *)&srtp->operation);
    }
}

/**********************************************************************
 *
 *			    MISCELLANEOUS
 *
 **********************************************************************/

/*
 * vines_from_reverse_path
 *
 * Did this packet come in on what this router considers the primary path
 * to the source of this packet.  By definition, we will consider the
 * primary path to be the first path that is currently eligible for
 * forwarding traffic to this station.
 */
boolean vines_from_reverse_path (paktype *pak)
{
    vinesiptype *vip;
    vinesneighbortype *neighbor;
    vinespathtype *path;
    vinesnettype *server;
    vinesidbtype *idb_back;
    hwaddrtype dlc_address;
    
    vip = (vinesiptype *)pak->network_start;
    neighbor = vneigh_FindEntry(GET2WD(vip->ssrcnet), vip->srchost);

    if (!neighbor) {
	server = vroute_FindEntry(GET2WD(vip->ssrcnet));
	if (!server || !server->primary_route || !server->primary_route->gw)
	    return(FALSE);
	neighbor = server->primary_route->gw;
    }
    if (!neighbor->primary_path)
	return(FALSE);
    path = neighbor->primary_path;
    idb_back = path->idb;

    if (!idb_back || (idb_back == vinesnullidb))
	return(FALSE);
    if (idb_back->phys_idb != pak->if_input)
	return(FALSE);
    memset(&dlc_address, 0, sizeof(hwaddrtype));
    if (!((*pak->if_input->hwptr->extract_snpa)(pak, &dlc_address, SOURCE)))
	return(FALSE);
    if (bcmp(&path->dlc_addr, &dlc_address, sizeof(hwaddrtype)) != 0)
	return(FALSE);
    return(TRUE);
}

/*
 * vines_get_reverse_idb
 *
 * Did this packet come in on what this router considers the primary path
 * to the source of this packet.  By definition, we will consider the
 * primary path to be the first path that is currently eligible for
 * forwarding traffic to this station.
 */
vinesidbtype *vines_get_reverse_idb (paktype *pak)
{
    vinesiptype *vip;
    vinesneighbortype *neighbor;
    vinesnettype *server;
    
    vip = (vinesiptype *)pak->network_start;
    neighbor = vneigh_FindEntry(GET2WD(vip->ssrcnet), vip->srchost);
    if (neighbor)
	return(neighbor->primary_path->idb);
    server = vroute_FindEntry(GET2WD(vip->ssrcnet));
    if (server) {
	neighbor = server->primary_route->gw;
	return(neighbor->primary_path->idb);
    }
    return(NULL);
}

/*
 * vines_nearest_server
 *
 * Scan the VINES RTP routing table looking for the nearest "real" banyan
 * server.  We can tell by looking at the manufacturer part of the
 * network number.  Banyan is manufacturer number 1.  If this server is
 * on a directly connected network, we want to set the local flag as
 * well.  We can determine this by seeing if the next hop network is the
 * same as the destination network number.  This code onlyy checks the
 * metric for route zero, because it only maintains routes of equal
 * metric.
 */
boolean vines_nearest_server (
    routingdecision *thewaythere,
    boolean *local)
{
    vinesnettype *server, *nearest;
    vinesroutetype *route;
    ulong metric;

    metric = VINES_INFINITY;
    nearest = NULL;
    FOR_ALL_SERVERS(server) {
	if (metric <= server->actual_metric)
	    continue;
	if ((server->net & VINES_NET_MFG) <= VINES_NET_BANYAN) {
	    nearest = server;
	    metric = server->actual_metric;
	}
    }

    /*
     * Did we find one?
     */
    if (!nearest) {
	memset(thewaythere, 0, sizeof(routingdecision));
	return(FALSE);
    }

    thewaythere->server = nearest;
    FOR_ALL_ROUTES(nearest, route) {
	if (nearest->net == route->gw->net) {
	    *local = TRUE;
	    thewaythere->route = route;
	    thewaythere->neighbor = thewaythere->route->gw;
	    thewaythere->path = thewaythere->neighbor->primary_path;
	    vines_lock_nodes(thewaythere);
	    return(TRUE);
	}
    }

    *local = FALSE;
    thewaythere->route = nearest->primary_route;
    thewaythere->neighbor = thewaythere->route->gw;
    thewaythere->path = thewaythere->neighbor->primary_path;
    vines_lock_nodes(thewaythere);
    return(TRUE);
}

ulong vines_nearest_server_net (void)
{
    routingdecision thewaythere;
    boolean dummy;
    ulong net;

    if (vines_nearest_server(&thewaythere, &dummy)) {
	net = thewaythere.server->net;
	vines_unlock_nodes(&thewaythere);
	return(net);
    }
    return(VINES_UNKNOWNNET);
}

/*
 * vines_new_server
 *
 * A new server has been detected somewhere on the network.  Perform any
 * special activities here.
 */
void vines_new_server (vinesnettype *server)
{
}

/*
 * vroute_all_use_neighbor
 *
 * This is a routine used to set all routes to point to the same
 * neighbor with a very low metric.  This can be used in an emergency
 * to force circular routes to all point to this router so that they
 * will then disappear when the neighbor entry ages out.
 */
void vroute_all_use_neighbor (vinesneighbortype *neighbor)
{
    vinesnettype *server;
    vinesroutetype *route;

    FOR_ALL_SERVERS(server) {
	if (server->net == vines_network)
	    continue;
	FOR_ALL_ROUTES(server, route) {
	    route->metric = 0;
	    route->gw = neighbor;
	}
    }
}


/**********************************************************************
 *
 *		       ROUTING UPDATE PROTOCOL
 *
 * Routines in this section understand the basic premises of the routing
 * update protocol.
 *
 **********************************************************************/

/*
 * vines_ager
 *
 * Call the routines to age both the VINES neighbor table and the VINES
 * server table.  Also wipe the proxy queue to make sure we don't lose
 * memory there.  If it hasn't been answered in 90 seconds, it's not
 * going to be answered.  This routine is quickly called multiple time
 * from the route timer routine, so prevent it from doing unnecessary work.
 */
void vines_ager (boolean flushflag)
{
    vint_record_time(VINT_TIME_ROUTE_START);
    vroute_age_all_nets(flushflag);
    vint_record_time(VINT_TIME_ROUTE_END);
    process_may_suspend();

    vint_record_time(VINT_TIME_NBR_START);
    vines_neighbor_ager(flushflag);
    vint_record_time(VINT_TIME_NBR_END);
    process_may_suspend();

    vint_record_time(VINT_TIME_CACHE_START);
    vinescache_ager(flushflag);
    vint_record_time(VINT_TIME_CACHE_END);
    process_may_suspend();

    vint_record_time(VINT_TIME_PROXY_START);
    while (vines_proxyQ.qhead)
	(void)p_dequeue(&vines_proxyQ);
    vint_record_time(VINT_TIME_PROXY_END);
    process_may_suspend();
}

/*
 * vroute_process_queues
 *
 * Process any incoming RTP messages that are sitting on the queue.  All
 * valid messages are passed top the neighbor update routine to keep that
 * table current.  Only messages received from primary paths to a
 * neighbor (as determined by the neighbor update routine) are used to
 * update the route table.
 */

static void vroute_process_queues (void)
{
    paktype *pak;
    vinesiptype *vip;
    vsrtp_header *rtp;
    vinesidbtype *idb;
    ulong srcnet, count, processed;
    ushort srchost;
    uchar *error;
    boolean do_debug;
    idbtype *swidb;

    /*
     * Work on the RTP packet queue
     */
    vint_record_time(VINT_TIME_RTP_START);
    processed = 0;
    count = PAK_SWITCHCOUNT;
    while (TRUE) {
	/*
	 * Process only a limited number of packets per invocation
	 */
	if (--count == 0)
	    break;
	
	/*
	 * Dequeue a VINES RTP message
	 */
	pak = process_dequeue(vinesrtpQ);
	if (!pak)
	    break;
	
	processed++;
	vip = (vinesiptype *)pak->network_start;
	srcnet = GETLONG(vip->ssrcnet);
	srchost = vip->srchost;
	do_debug = do_vines_rtp_debug(srcnet, vip->srchost);
	swidb = pak->if_input;
	idb = vines_getidb(swidb);

	/*
	 * A wee bit o counting.
	 */
	if ((swidb->hwptr->broadcast)(swidb->hwptr, pak) == FUZ_TRUE) {
	    vroute_bcast_in++;
	}

	/*
	 * Perform sanity checking and filtering on each routing protocol
	 * packet.  Make noise if anything fails.
	 */
	error = NULL;
	if (!interface_up(pak->if_input)) {
	    error = "down";
	} else if (!vines_valid_net(srcnet)) {
	    error = "bad source";
	} else if ((vip->tc & VINES_HOPS) != 0) {
	    error = "bad hop count";
	} else if (!vines_short_access_check(srcnet, srchost, idb->input_router_filter)) {
	    error = "filtered";
	}
	if (error) {
	    if (do_debug)
		buginf("\nVINES: Discarding RTP/SRTP from %#z on %s (%s)",
		       srcnet, vip->srchost, idb->phys_idb->namestring, error);
	    datagram_done(pak);
	    continue;
	}

	/*
	 * Range check the operation field.
	 */
	rtp = (vsrtp_header *)pak->transport_start;
	if (rtp->version > VSRTP_VERSION_THRESH) {
	    reg_invoke_snapshot_update_received(pak->if_input,
						(paktype *) pak);
	    vrtp_process_message(pak);
	} else if (rtp->version == VSRTP_VERSION) {
	    if (vines_vsrtp_enabled(idb)) {
		vsrtp_process_message(pak);
	    } else {
		if (do_debug)
		    buginf("\nVSRTP: Discarding SRTP from %#z on %s (not supported)",
			   srcnet, vip->srchost, idb->phys_idb->namestring);
	    }
	} else if (do_debug) {
	    buginf("\nVRTP: invalid RTP version from %#z on %s",
		   srcnet, vip->srchost, idb->phys_idb->namestring);
	}
	datagram_done(pak);
	process_suspend_if_req();
    }
    vint_record_time(VINT_TIME_RTP_END);
    vint_record_value(VINT_COUNT_RTP, processed);
}

/*
 * vroute_query_timer_expired
 *
 * An interface's query timer expired.  See if there are any known
 * neighbors on this interface.  If so, queries are no longer needed, so
 * cancel them.  This does leave a hole for multipoint interfaces, but
 * that can be worked around by using point-to-point sub-interfaces.
 */
static void vroute_query_timer_expired (vinesidbtype *idb)
{
    if (vines_routers_present(idb)) {
	mgd_timer_stop(&idb->query_timer);
    } else {
	vrtp_send_request(idb, NULL);
	mgd_timer_update(&idb->query_timer, VRTP_QUERYINTERVAL(idb));
    }
}

/*
 * vroute_wakeup_timer_expired
 *
 * An interface's wakeup timer expired.  Send the appropriate updates.
 */
static void vroute_wakeup_timer_expired (vinesidbtype *idb)
{
    GET_TIMESTAMP(idb->update_start_time);
    if (vines_srtp_present(idb))
	vsrtp_send_periodic(idb);
    if (vines_rtp_present(idb))
	vrtp_send_periodic(idb);
    GET_TIMESTAMP(idb->update_end_time);
    mgd_timer_update(&idb->wakeup_timer, idb->vines_wakeup_interval);
}

/*
 * vroute_sync_timer_expired
 *
 * An interface's sync timer expired.  Send the appropriate updates.
 */
static void vroute_sync_timer_expired (vinesidbtype *idb)
{
    GET_TIMESTAMP(idb->update_start_time);
    vsrtp_send_periodic(idb);
    GET_TIMESTAMP(idb->update_end_time);
    mgd_timer_update(&idb->sync_timer, VSRTP_SYNC_INTERVAL);
}

/*
 * vines_periodic_if_check
 *
 * This is a paranoia check to insure that if an interface is up and
 * configured for vines, that its timer is really running.
 */
static void vines_periodic_if_check (void)
{
    vinesidbtype *idb;

    FOR_ALL_VIDBS(idb) {
	/* configured? */
	if (!idb->vines_metric)
	    continue;
	if (!mgd_timer_running(&idb->wakeup_timer) && idb->vines_lineup &&
	    interface_up(idb->phys_idb)) {
	    errmsg(&msgsym(BADTIMER,VINES), idb->phys_idb->namestring);
	    mgd_timer_start(&idb->wakeup_timer, 0);
	}
    }
}

/*
 * vroute_do_timers
 *
 * Handle any expired timers for this process.
 */
static void vroute_do_timers (void)
{
    mgd_timer *expired_timer;
    vinespathtype *path;
    vinesidbtype *idb;

    while(mgd_timer_expired(&vines_timer)) {
	expired_timer = mgd_timer_first_expired(&vines_timer);
	switch (mgd_timer_type(expired_timer)) {
	  case VT_SRTP_START:
	    mgd_timer_stop(&vsrtp_start_timer);
	    vsrtp_start();
	    break;
	    
	  case VT_RETRY:
	    path = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    vsrtp_retry_timer_expired(path);
	    break;
	    
	  case VT_REASSEMBLY:
	    path = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    vsrtp_reassembly_timer_expired(path);
	    break;
	    
	  case VT_RC:
	    path = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    vrtp_rc_timer_expired(path);
	    break;
	    
	  case VT_CHANGED:
	    /*
	     * Send out any batched changes that have accumulated up to this
	     * point.  There is a 2% change that this information may be
	     * duplicated in a full update that immediately follows.
	     */
	    vroute_send_change_updates(NULL);
	    mgd_timer_stop(&vines_changed);
	    break;
	    
	  case VT_QUERY:
	    idb = mgd_timer_context(expired_timer);
	    vroute_query_timer_expired(idb);
	    break;
	    
	  case VT_WAKEUP:
	    vinescache_debug_count = 0;
	    idb = mgd_timer_context(expired_timer);
	    vroute_wakeup_timer_expired(idb);
	    break;
	    
	  case VT_SYNC:
	    idb = mgd_timer_context(expired_timer);
	    vroute_sync_timer_expired(idb);
	    break;
	    
	  case VT_PERIODIC:
	    vines_periodic_if_check();
	    vines_ager(FALSE);
	    mgd_timer_update(&vines_periodic_timer, VRTP_DEF_WAKEUP_INTERVAL);
	    break;

	  default:
	    errmsg(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/*
 * vroute_process_messages
 *
 * Handle any received packets for this process.
 */
void vroute_process_messages (void)
{
    vinesidbtype *idb;
    idbtype *swidb;
    ulong message, value;
    void *pointer;
    vroute_msg *msg;
    
    while (process_get_message(&message, &pointer, &value)) {
	switch (message) {
	  case MSG_ROUTE_ADJUST:
	    swidb = pointer;
	    vines_route_adjust(swidb);
	    break;
	  case VINES_SIGNAL_IDB_UP:
	    idb = pointer;
	    if (!idb->deletion_pending)
		vroute_interface_came_up_work(idb);
	    break;
	  case VINES_SIGNAL_IDB_DOWN:
	    idb = pointer;
	    if (!idb->deletion_pending)
		vroute_interface_went_down_work(idb);
	    break;
	  case VINES_SIGNAL_VC_ADDED:
	    msg = pointer;
	    if (msg != NULL) {
		idb = msg->idb;
		if (!idb->deletion_pending)
		    vroute_vc_added_work(idb, &msg->addr);
		free(msg);
	    }
	    break;
	  case VINES_SIGNAL_VC_REMOVED:
	    idb = pointer;
	    if (!idb->deletion_pending)
		vroute_vc_removed_work(idb);
	    break;
	  case VINES_SIGNAL_IDB_DELETION_PENDING:
	    idb = pointer;
	    vines_delete_if_work(idb);
	    break;
	case MSG_FR_PVC_ACTIVE:
	    swidb = pointer;
	    if (vines_on_idb(swidb)) {
		idb = vines_getidb(swidb);
		if (!idb->deletion_pending)
		    vroute_vc_added_work(idb, NULL);
	    }
	    break;
	case MSG_FR_PVC_INACTIVE:
	    swidb = pointer;
	    if (vines_on_idb(swidb)) {
		idb = vines_getidb(swidb);
		if (!idb->deletion_pending)
		    vroute_vc_removed_work(idb);
	    }
	    break;
	  default:
	    errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), message, pointer, value);
	    break;
	}
    }
}

/*
 * vines_router_teardown 
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
static void vines_router_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&vines_timer, DISABLE);
    process_watch_queue(vinesrtpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(vinesrtpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&vinesrtpQ);
    reg_delete_route_adjust_msg(vines_rtr_pid);
    reg_delete_media_fr_pvc_active(vines_rtr_pid);
    reg_delete_media_fr_pvc_inactive(vines_rtr_pid);
    vines_rtr_pid = 0;
}

/*
 * vines_router
 *
 * VINES RTP router process. Perform routing maintaince functions.
 */
process vines_router (void)
{
    ulong major, minor;

    /*
     * Set up this processes world.
     */
    signal_permanent(SIGEXIT, vines_router_teardown);
    vinesrtpQ = create_watched_queue("VINES RTP packets", 0, 0);
    process_watch_queue(vinesrtpQ, ENABLE, RECURRING);
    reg_add_route_adjust_msg(vines_rtr_pid, "vines_router");
    reg_add_media_fr_pvc_active(vines_rtr_pid, "vines_router");
    reg_add_media_fr_pvc_inactive(vines_rtr_pid, "vines_router");
    process_watch_mgd_timer(&vines_timer, ENABLE);

    /*
     * Process spin-loop.
     */
    while (TRUE) {
	/*
	 * Wait for the next event to occur.
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		vroute_do_timers();
		break;
	      case QUEUE_EVENT:
		vroute_process_queues();
		break;
	      case MESSAGE_EVENT:
		vroute_process_messages();
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/**********************************************************************
 *
 *		     VINES GENERIC SEND ROUTINES
 *
 **********************************************************************/

/*
 * vroute_send_full_updates
 *
 * Perform an VINES RIP general broadcast.
 */

void vroute_send_full_updates (vinesidbtype *target)
{
    vinesidbtype *idb;

    /*
     * Bump the SRTP sequence number.
     */
    if (vsrtp_enabled) {
	vines_sequence++;
	if (vines_state_debug)
	    buginf("\nVSRTP: Sequence number incremented to %x in send full updates",
		   vines_sequence);
    }

    /*
     * Send both RTP and SRTP updates
     */
    FOR_ALL_VIDBS(idb) {
	if ((target && (target != idb)) || !vines_interface_up(idb))
	    continue;
	if (vines_srtp_present(idb))
	    vsrtp_send_update(VSRTP_FULL_UPDATE, idb, NULL);
	if (vines_rtp_present(idb))
	    vrtp_send_update(idb, NULL, TRUE, TRUE, !idb->vines_split_disabled,
			     VRTP_UPDATE);
    }
}

/*
 * vroute_send_change_updates
 */
void vroute_send_change_updates (vinesidbtype *target)
{
    vinesidbtype *idb;

    /*
     * Bump the SRTP sequence number.
     */
    if (vsrtp_enabled) {
	vines_sequence++;
	if (vines_state_debug)
	    buginf("\nVSRTP: Sequence number incremented to %x in send change updates",
		   vines_sequence);
    }

    /*
     * Send both RTP and SRTP updates
     */
    FOR_ALL_VIDBS(idb) {
	if ((target && (target != idb)) || !vines_interface_up(idb))
	    continue;
	if (vines_srtp_present(idb))
	    vsrtp_send_update(VSRTP_CHANGE_UPDATE, idb, NULL);
	if (vines_rtp_present(idb))
	    vrtp_send_update(idb, NULL, FALSE, FALSE,
			     !idb->vines_split_disabled, VRTP_UPDATE);
    }
}

/*
 * vroute_send_requests
 */
void vroute_send_requests (vinesidbtype *target)
{
    vinesidbtype *idb;
    
    FOR_ALL_VIDBS(idb) {
	if ((target && (target != idb)) ||!vines_interface_up(idb))
	    continue;
	vsrtp_send_full_request(idb, NULL);
	vrtp_send_request(idb, NULL);
    }
}

/*
 * vroute_send_redirects
 */
void vroute_send_redirects (
    paktype *original,
    routingdecision *thewaythere)
{
    vinesidbtype *idb;
    
    idb = thewaythere->path->idb;
    if (!idb)
	return;
    if (vines_srtp_present(idb))
	vsrtp_send_redirect(original, thewaythere);
    if (vines_rtp_present(idb))
	vrtp_send_redirect(original, thewaythere);
}

/*
 * vines_set_route_timestamp
 * Set the current timer on the given route, and indicate to Snapshot that
 * it has been seen.
 */
void vines_set_route_timestamp (vinesroutetype *route)
{
    if (route == NULL)
      return;

    GET_TIMESTAMP(route->timer);
    route->snapshot_force_aging = FALSE;
}
