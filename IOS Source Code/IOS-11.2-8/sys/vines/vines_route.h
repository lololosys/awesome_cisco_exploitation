/* $Id: vines_route.h,v 3.8.18.3 1996/08/07 09:03:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_route.h,v $
 *------------------------------------------------------------------
 * vines_route.h  --  Definitions for Vines RTP protocol.
 *
 * September 1992, David R. Hampton
 * from a module dated February 1990, David Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_route.h,v $
 * Revision 3.8.18.3  1996/08/07  09:03:52  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/05/11  16:22:20  thille
 * CSCdi51502:  Missed some duplicate strings in vines_route.h
 * Branch: California_branch
 * Save another 392 bytes.
 *
 * Revision 3.8.18.1  1996/03/18  22:32:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.2  1996/03/07  11:04:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:46:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/08  20:14:06  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.7  1996/01/22  07:44:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/08  04:56:24  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.5  1995/12/07  22:33:41  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/01  19:22:23  slin
 * CSCdi44499: vines RTS_AGE timer should be reset to 4 when an update is
 *             received
 *
 * Revision 3.3  1995/11/22  18:55:12  slin
 * CSCdi38892:  vines over X.25 requesting a full routing update on all
 *              PVCs
 * - Send only to newly established PVC
 *
 * Revision 3.2  1995/11/17  17:58:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern rbTree *vines_network_tree;
extern mgd_timer vrtp_rc_timer;

typedef struct vroute_fn_data_ {
    vinesnettype *server;
    vinesneighbortype *neighbor;
    ulong net;
    ulong flag;
    sys_timestamp timestamp;
    char *string;
} vroute_fn_data;

/*
 * Messages passed to vines_router process
 */
typedef struct {
    vinesidbtype *idb;
    addrtype addr;
} vroute_msg;

/*
 * Magic numbers for determining whether or not data structures have been
 * corrupted by other parts of the router software.
 */
#define VINES_MAGIC_PATH     0x07301963
#define VINES_MAGIC_NEIGHBOR 0x01111966
#define VINES_MAGIC_ROUTE    0x06061990
#define VINES_MAGIC_SERVER   0x05211994
#define VINES_MAGIC_INVALID  0xBAAAAAAD

/**********************************************************************
 *
 *			       METRICS
 *
 **********************************************************************/
#define V_METRIC_FDDI	(  1 * 16)	/* cisco defined */
#define V_METRIC_ETHER	(  2 * 16)
#define V_METRIC_16MBTR (  2 * 16)
#define V_METRIC_4MBTR	(  4 * 16)
#define V_METRIC_CUTOFF ( 30 * 16)
#define V_METRIC_T1	( 35 * 16)	/* cisco defined */
#define V_METRIC_56K	( 45 * 16)
#define V_METRIC_9600	( 90 * 16)
#define V_METRIC_4800	(150 * 16)
#define V_METRIC_2400	(250 * 16)	/* cisco defined */
#define V_METRIC_1200	(450 * 16)

#define V_METRIC_X25    ( 10 * 16)
#define V_METRIC_BRIDGE (  5 * 16)

#define VINES_INFINITY	0xFFFF


/**********************************************************************
 *
 *			       ROUTES
 *
 **********************************************************************/

/*
 * Route flags
 */
#define V_NET_ME		0x0001	/* net is this router */
#define V_NET_STATIC		0x0002	/* net was entered manually */
#define V_NET_RTP		0x0004	/* net discovered via RTP */
#define V_NET_IGRP		0x0008	/* net discovered via IGRP */
#define V_NET_TEST		0x0010	/* net entered by test command */
#define V_NET_REDIRECT		0x0020	/* net entered by a redirect */

/*
 * A 'route' to a server.  There is approximately one of these per server
 * per neighbor through wich it can be reached.  The metric value
 * retained is the metric _advertised_ by the neighbor associated with
 * this route.  It must be combined with the metric needed to reach the
 * neighbor in order to get the true metric to the server.  Only the best
 * metric routes (and static routes) are maintained in the tables.
 */
struct vinesroutetype_ {
    vinesroutetype *next;
    vinesroutetype *next_best;
    vinesnettype *server;
    ulong   magic;
    ulong   flags;
    ulong   snapshot_force_aging;
    ulong   metric;		/* metric in 0.125 second counts */
    ulong   uses;		/* times this route was used	*/
    sys_timestamp timer;	/* time entered or updated	*/
    sys_timestamp gctime;	/* time when garbage collected  */
    vinesneighbortype *gw;	/* how to get there		*/

    /* Vines SRTP cells */
    ulong   origin_timestamp;
    ulong   local_timestamp;
    uchar   rtp_flags;
};

/*
 * A server.  There is one of these per server or router anywhere in the
 * vines internet.  If a server is available via more than one neighbor,
 * there is only one entry allocated for it.  The metric value reflects
 * the current best metric value of all routes to this server.
 */
struct vinesnettype_ {
    treeLink link;
    ulong   magic;
    ulong   flags;
    ulong   net;		/* the 'net number' in question */
    ulong   actual_metric;	/* metric in 0.125 second counts */
    ulong   adv_metric;		/* advertised metric in internal format */
    sys_timestamp suppresstime;	/* time suppression expires	*/
    sys_timestamp changetime;	/* when to stop including in change updates */
    dqueue_t by_metric;

    int     route_count;
    vinesroutetype *available_routes;
    vinesroutetype *best_route;
    vinesroutetype *primary_route;
};

struct routingdecision_ {
    vinesnettype      *server;
    vinesroutetype    *route;
    vinesneighbortype *neighbor;
    vinespathtype     *path;
};

/*
 * Hello and Routing Timers.
 *
 * We must not expire hellos before associated routes become invalid.
 * This allows the router to send two updates that the route has expired:
 * the flash when it expired and then one normal update.
 */
#define VRTP_DEF_WAKEUP_INTERVAL (90*ONESEC)
#define VRTP_DEF_UPDATE_COUNT    3

#define VRTP_HELLOTIME(idb)    (1 * idb->vines_wakeup_interval)
#define VRTP_HELLOINVALID(idb) (5 * idb->vines_wakeup_interval)
#define VRTP_HELLOEXPIRE(idb)  (2 * idb->vines_wakeup_interval) /* additional */

#define VRTP_QUERYINTERVAL(idb) (idb->vines_wakeup_interval / 3)

#define VRTP_SUPPRESSTIME      (120 * ONESEC)

#define VRTP_HOLDTIME(x)       (2 * x)
#define VRTP_MOVETIME(x)       (2 * x)
#define VRTP_ROUTEEXPIRE(x)    (2 * x)
#define VRTP_INVALIDTIME(x)    (4 * x)

/* VRTP_CHANGETIME must be longer than VRTP_HOLDTIME(x) */
#define VRTP_CHANGETIME        (3 * vines_slowest_update)

#define VRTP_FLASHUPDATE	(2*ONESEC)

/**********************************************************************
 *
 **********************************************************************/

/*
 * Vines Global Variables
 */
extern uint vines_routes;
extern uint vines_net_version, vines_neighbor_version;


/**********************************************************************
 *
 *	           Network Tree Interface Routines
 *
 **********************************************************************/

/*
 * vroute_EntryKey
 * Create a treeKey for a network which is always unique.
 */
static inline treeKey
vroute_EntryKey (ulong net)
{
    treeKey key;
    key.l = net;
    return key;
}

/*
 * vroute_RouteCast
 * Do data type conversion between links and networkss.
 */
static inline vinesnettype*
vrouteCast (treeLink* link) { return(vinesnettype*)link; }

/*
 * vroute_UnprotectNetwork
 * Allow a network to be freed or reused.
 */
static inline void
vroute_UnprotectNetwork (vinesnettype* s) 
{ 
    RBTreeNodeProtect(&s->link,FALSE); 
}

/*
 * vroute_ProtectNetwork
 * Protect a network from being freed.
 */
static inline void
vroute_ProtectNetwork (vinesnettype* s) 
{ 
    RBTreeNodeProtect(&s->link,TRUE); 
}

/*
 * vroute_NetworkProtected
 * Return protection status of network treeLink entry.
 */
static inline boolean
vroute_NetworkProtected (vinesnettype* s)
{
    return RBTreeNodeProtected(&s->link);
}

/*
 * vroute_GetNextNetwork - Search for the next network and return it w/o protection
 */
static inline vinesnettype*
vroute_GetNextNetwork (vinesnettype* network)
{
    vinesnettype* s = network;

    if (vines_network_tree == NULL)
	return NULL;
    if (s == NULL) {
	s = vrouteCast(RBTreeFirstNode(vines_network_tree));
    } else {
	s = vrouteCast(RBTreeNextNode(&s->link));
    }
    return s;
}

/*
 * vroute_GetFirstNetwork
 * Get the first network from the tree.
 */
static inline vinesnettype*
vroute_GetFirstNetwork (void)
{ 
    return vroute_GetNextNetwork(NULL); 
}

/*
 * vroute_GetBestNetwork
 * Return the specified network OR the next greater network.
 */
static inline vinesnettype*
vroute_GetBestNetwork (treeKey key)
{
    return vrouteCast(RBTreeLexiNode(vines_network_tree,key));
}

/*
 * vroute_InsertEntry
 * Add a network to our table.
 */
static inline void
vroute_InsertEntry (vinesnettype* s) 
{
    treeKey key = vroute_EntryKey(s->net);
    if (vines_network_tree != NULL)
	RBTreeInsert(key,vines_network_tree,&s->link);
}

/*
 * vroute_DeleteEntry
 * Delete a network from our table.
 */
static inline void
vroute_DeleteEntry (vinesnettype* s)
{
    if ((vines_network_tree != NULL)&&(s!=NULL))
	RBTreeDelete(vines_network_tree,&s->link);
}

/*
 * vroute_ReinitNetwork
 * Release any resources in use for this network
 */
static inline void
vroute_ReinitNetwork (vinesnettype* s)
{
	/* Currently no special resources */
}

/*
 * vroute_CreateEntry
 * initialize a new network
 */
static inline vinesnettype*
vroute_CreateEntry (ulong net)
{
    vinesnettype* s;

    if (vines_network_tree == NULL)
	return NULL;

    s = (vinesnettype*) RBTreeGetFreeNode(vines_network_tree);
    if (s != NULL)
	vroute_ReinitNetwork(s);
    else
	s = malloc_named(sizeof(vinesnettype), vines_vines_network_str);
    if (s == NULL)
	return NULL;
    memset(s, 0, sizeof(vinesnettype)); /* clear node */
    s->net = net;
#ifdef notdef
    if (vinestree_debug)
	errmsg(&msgsym(NEWNETWORK, VINES), s->net);
#endif
    vroute_InsertEntry(s);
    return s;
}

/*
 * vroute_FindEntry
 *
 * Find a network for the given address. No NULL parameters allowed.
 */
static inline vinesnettype*
vroute_FindEntry (ulong net)
{
    vinesnettype* s;
    treeKey key;

    key = vroute_EntryKey(net);
    if (vines_network_tree == NULL)
	return (NULL);
    s = vrouteCast(RBTreeSearch(vines_network_tree,key));
    return (s);
}

/*
 * vroute_WalkTree
 *
 * Call the specified function for each node in the tree.
 */
static inline boolean
vroute_WalkTree (treeProc proc, vroute_fn_data *data)
{
    return(RBTreeForEachNode(proc, (void *)data, vines_network_tree, TRUE));
}

/*
 * vroute_WalkPartialTree
 *
 * Call the specified function for each node in the tree.
 */
static inline boolean
vroute_WalkPartialTree (treeProc proc, vinesnettype *s, void *data)
{
    if (s)
	return(RBTreeForEachNodeTilFalse(proc, data, vines_network_tree,
					 &s->link, TRUE));
    return(RBTreeForEachNodeTilFalse(proc, data, vines_network_tree,
				     NULL, TRUE));
}

static inline vinesnettype *
vroute_FindSelf (boolean debug, char *tag)
{
    vinesnettype *self;

    self = vroute_FindEntry(vines_network);
    if (!self && debug)
	buginf(vines_cannot_find_network_msg, tag);
    return(self);
}


/**********************************************************************
 *
 *			   General Inlines
 *
 **********************************************************************/

/*
 * Macros to traverse all routes in a network entry
 */
#define FOR_ALL_ROUTES(server, route) \
    for ((route) = (server)->available_routes; (route); (route) = (route)->next)

#define FOR_ALL_ROUTES_W_PREV(server, route, prev) \
    prev = (vinesroutetype *)&(server)->available_routes; \
    for ((route) = (server)->available_routes; \
         (route); \
         (prev) = (route), (route) = (route)->next)

#define FOR_ALL_SERVERS(server) \
    for ((server) = vroute_GetFirstNetwork(); \
	 (server); \
	 (server) = vroute_GetNextNetwork((server)))


/**********************************************************************
 *
 *		     VINES General Metric Manipulation
 *
 **********************************************************************/

static inline boolean vmetric_valid (ulong metric)
{
    return(metric > 0L);
}

/*
 *
 */
static inline char *vmetric_to_seconds (ushort metric)
{
    static char buffer[20];
    sprintf(buffer, "%u.%04u", metric/80, (metric%80)*125);
    return(buffer);
}

/*
 *
 */
static inline char *vmetric_to_config (ushort metric)
{
    static char buffer[20];
    sprintf(buffer, "%u", metric/16);
    if ((metric%16) != 0)
	sprintf(buffer+strlen(buffer), " %4u", (metric%16)*625);
    return(buffer);
}

/*
 *
 */
static inline ushort vmetric_from_config (ushort whole, ushort fractional)
{
    return((whole << 4) + (fractional/625));
}

/*
 *
 */
static inline void vines_set_server_metrics(vinesnettype *server, int metric)
{
    server->actual_metric = metric;
    if (TIMER_RUNNING_AND_SLEEPING(server->suppresstime)) {
	server->adv_metric = VINES_INFINITY;
    } else {
	server->adv_metric = metric;
    }
}


/**********************************************************************
 *
 *		 VINES Generic Timer Queue Manipulation
 *
 **********************************************************************/

static inline void vines_bump_changed_timer (void)
{
    if (mgd_timer_running(&vines_changed))
	return;
    mgd_timer_start(&vines_changed, VRTP_FLASHUPDATE);
}

static inline void vines_set_changetime (vinesnettype *server)
{
    TIMER_START(server->changetime, VRTP_CHANGETIME);
    vines_bump_changed_timer();
}

static inline void vines_set_suppresstime (vinesnettype *server)
{
    if (do_vines_rtp_v_debug(server->net, VINES_SERVER) ||
	do_vines_table_debug(server->net, VINES_SERVER))
	buginf(vsrtp_network_suppression_msg, server->net);
    TIMER_START(server->suppresstime, VRTP_SUPPRESSTIME);
}

/**********************************************************************
 *
 *			Routing Table Inlines
 *
 **********************************************************************/

/*
 * vines_get_route
 */
static inline vinesroutetype *vines_get_route (
    vinesnettype *server)
{
    vinesroutetype *route;

    route = server->best_route;
    server->best_route = route->next_best;
    route->uses++;
    return(route);
}

extern vinesroutetype *vines_get_route_by_xxx(vinesnettype *, vinesneighbortype *, ulong, vinesidbtype *);

/*
 * vines_get_route_by_idb 
 *
 * Find a route to a server that uses the given interface.
 */
static inline vinesroutetype *vines_get_route_by_idb (
    vinesnettype *server,
    vinesidbtype *idb)
{
    return(vines_get_route_by_xxx(server, NULL, 0, idb));
}

/*
 * vines_get_route_by_gw 
 *
 * Find a route to a server that uses the given neighbor router.
 */
static inline vinesroutetype *vines_get_route_by_gw (
    vinesnettype *server,
    vinesneighbortype *gw)
{
    return(vines_get_route_by_xxx(server, gw, 0, NULL));
}

/*
 * vines_server_total_routes
 *
 * Count all routes associated with a server data structure.
 */
static inline int vines_server_total_paths (
    vinesnettype *server)
{
    vinesroutetype *route;
    int count = 0;

    if (!server || !server->available_routes)
	return(0);
    FOR_ALL_ROUTES(server, route)
	count += route->gw->path_count;
    return(count);
}

/**********************************************************************
 *
 *                      External Definitions
 *
 **********************************************************************/
 
extern char *const vsrtp_names[];
extern uint vines_routes, vines_net_version;
extern ulong vsrtp_reinit_sequence, vines_sequence, vines_packet_id;
extern sys_timestamp vsrtp_reinit_check_time;
extern mgd_timer vsrtp_start_timer;
extern ulong vroute_no_buffer_in;
extern ulong vroute_no_buffer_out;
extern ulong vroute_bcast_in;
extern ulong vroute_bcast_out;
extern ulong vroute_redirect_modified;
extern ulong vroute_redirect_created;
extern dqueue_t servers_by_metric;


/**********************************************************************
 *
 *			      PROTOTYPES
 *
 **********************************************************************/

/* vines_route.c */
extern void vines_route_init(void);
extern void vines_unlock_nodes(routingdecision *);
extern void vines_lock_nodes(routingdecision *);
extern void vines_compute_server_metric(vinesnettype *server, boolean bump);
extern void vines_build_route_list(vinesnettype *);
extern void vines_fixup_server(vinesnettype *server, boolean bump);
extern boolean vines_fixup_one_server(treeLink *, void *);
extern void vines_display_times(void);
extern void vines_fixup_servers_by_neighbor(vinesneighbortype *neighbor);
extern boolean vines_check_route_list(vinesnettype *, vinesroutetype **);
extern boolean vines_verify_server_structure(vinesnettype *);
extern void vines_insert_new_route(vinesnettype *, vinesroutetype *);
extern vinesroutetype *vines_make_new_route(vinesnettype *);
extern vinesroutetype *vines_add_route(vinesnettype *, vinesneighbortype *, ulong, ushort);
extern vinesnettype *vines_delete_this_route(vinesnettype *, vinesroutetype *);
extern vinesnettype *vines_delete_route_by_xxx(vinesnettype *, vinesneighbortype *, ulong, ulong, sys_timestamp);
extern void vines_flush_dynamic_routes(ulong);
extern boolean vines_flush_one_route(treeLink *, void *);
extern void vines_flush_routes_by_neighbor(vinesneighbortype *, ulong, sys_timestamp);
extern boolean vines_poison_one_route(treeLink *, void *);
extern void vines_poison_routes_by_neighbor(vinesneighbortype *, sys_timestamp);
extern void vines_start_aging_routes_by_neighbor(vinesneighbortype *);
extern vinesnettype *vines_add_server(ulong, ulong);
extern void vines_delete_server_entry(vinesnettype *);
extern void vines_delete_server(ulong);
extern boolean vines_server_uses_idb(vinesnettype *, vinesidbtype *);
extern int vines_get_server_metric(ulong);
extern boolean vines_first_hop(ulong, ushort, routingdecision *);
extern boolean vines_first_hop_test(ulong, ushort);
extern boolean vroute_age_one_net(treeLink *, void *);
extern void vroute_age_all_nets(boolean);
extern void vroute_interface_came_up(vinesidbtype *);
extern void vroute_interface_went_down(vinesidbtype *);
extern void vroute_vc_added(vinesidbtype *, addrtype *);
extern void vroute_vc_removed(vinesidbtype *);
extern void vines_delete_if(vinesidbtype *);
extern void vroute_interface_came_up_work(vinesidbtype *);
extern void vroute_interface_went_down_work(vinesidbtype *);
extern void vroute_vc_added_work(vinesidbtype *, addrtype *);
extern void vroute_vc_removed_work(vinesidbtype *);
extern boolean vines_reload_entry(treeLink *, void *);
extern void vines_reload_imminent(void);
extern char vroute_nettype(ulong);
extern void vines_display_route_list(vinesnettype *, vinesneighbortype *, int, boolean);
extern boolean vroute_display_one_net(treeLink *, void *);
extern void vroute_display_all_nets(long, vinesneighbortype *);
extern void vroute_display_nets_by_metric(long, vinesneighbortype *);
extern void vroute_ShowStats(parseinfo *);
extern boolean vines_from_reverse_path(paktype *);
extern vinesidbtype *vines_get_reverse_idb(paktype *);
extern boolean vines_nearest_server(routingdecision *, boolean *);
extern void vines_new_server(vinesnettype *);
extern void vines_ager(boolean);
extern void vroute_process_messages(void);
extern process vines_router(void);
extern void vroute_send_full_updates(vinesidbtype *);
extern void vroute_send_change_updates(vinesidbtype *);
extern void vroute_send_requests(vinesidbtype *);
extern void vroute_send_redirects(paktype *, routingdecision *);
extern uchar *vroute_decode_packet(uchar *, int *, void (*fn )(const char *, ...));
extern void vroute_all_use_neighbor(vinesneighbortype *);
extern void vroute_update_flags(vinesneighbortype *);
extern void vines_set_route_timestamp(vinesroutetype *);
extern ulong vines_nearest_server_net(void);
extern ulong vroute_get_route_count(void);
extern ulong vroute_get_lookup_success(void);
extern ulong vroute_get_lookup_failed(void);
