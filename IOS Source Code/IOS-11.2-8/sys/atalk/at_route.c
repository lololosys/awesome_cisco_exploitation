/* $Id: at_route.c,v 3.4.18.10 1996/09/04 22:52:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_route.c,v $
 *------------------------------------------------------------------
 * atalk_route.c -- Appletalk routing
 *
 * September 1988, Bill Palmer
 * ATp2 support, August 1990, David Edwards
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_route.c,v $
 * Revision 3.4.18.10  1996/09/04  22:52:46  snyder
 * CSCdi68101:  declare things that should be const in sub_atalk
 *              Little one 36 from data, 8 from image
 * Branch: California_branch
 *
 * Revision 3.4.18.9  1996/07/16  22:56:05  dwong
 * CSCdi62796:  AT-EIGRP takes too long to age out
 * Branch: California_branch
 *
 * Revision 3.4.18.8  1996/07/10  22:18:45  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.18.7  1996/06/17  22:21:53  kleung
 * CSCdi60349:  AppleTalk Redist process name too long for show mem
 * Branch: California_branch
 *
 * Revision 3.4.18.6  1996/05/26  04:14:41  dwong
 * CSCdi58702:  Need to avoid a possible null pointer deference
 * Branch: California_branch
 *
 * Revision 3.4.18.5  1996/05/23  23:39:29  dwong
 * CSCdi57321:  AT-2-ASSERTFAILED in file /atalk/at_roure.c with traceback
 * Branch: California_branch
 * Warning messages should only be generated when debug apple event is on
 *
 * Revision 3.4.18.4  1996/05/02  08:40:24  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.4.18.3  1996/03/27  06:55:24  dwong
 * CSCdi51787:  Apple: Virtual-net config reloads router at __doprnt
 * Branch: California_branch
 * Avoid dereferencing the namestring when a null idb is passed.
 *
 * Revision 3.4.18.2  1996/03/23  01:27:21  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4.18.1  1996/03/18  18:52:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/07  08:29:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  00:17:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/08  23:08:07  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.3  1995/12/08  04:55:21  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.2  1995/11/17  08:43:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:50:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/19  18:35:33  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.6  1995/07/27  07:30:33  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.5  1995/07/13 06:50:00  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/07/05 22:24:43  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.3  1995/06/10  12:55:22  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.2  1995/06/09  12:58:39  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:09:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <interface.h>
#include "packet.h"
#include "plural.h"
#include "../os/chunk.h"
#include "atalk_private.h"
#include "at_registry.h"
#include "../os/free.h"
#include "at_domain.h"
#include "at_static.h"


/*
 * Forward declarations.
 */
static boolean route_periodic(treeLink*, void*);
static atalk_rdb_t *atroute_AddRoute(rbTree *, atalkidbtype *, ushort, ushort,
				     neighborentry *, ulong);
static void atroute_process_redist_queue(queuetype *);
static boolean route_GC(void);


/*
 * Constants
 */
static const ushort kMaxFreeRdbNodes = 25;
static const ushort ATALK_MIN_REDIST = 50;

/*
 * Route & path tree header. There is one of these per world.
 * We should probably hang the zone list off of this structure as well.
 */
typedef struct tree_list_ {
    rbTree	*r_tree;		/* route tree this entry describes */
    rbTree	*p_tree[AT_PATH_MAX];	/* path trees, one per type  */
    ulong	gcRuns;			/* number of route GC runs */
    ulong	rdbFreed;		/* # of RDB's free'd */
    ulong	rdbMallocs;		/* # of RDB's malloc'd */
    ulong	rdbRecycled;

    ulong	pdbFreed;		/* # of PDB's free'd */
    ulong	pdbMallocs;		/* # of PDB's malloc'd */
    ulong	pdbRecycled;

    struct tree_list_	*next;		/* the next tree */
} tree_list_t;

typedef struct atalk_redist_work_ {
    struct atalk_redist_work_	*next;
    at_path_t			type;
    at_path_action_t		action;
    ushort			rng_start;
    ushort			rng_end;
} atalk_redist_t;

typedef struct atalk_protocol_registry_ {
    struct atalk_protocol_registry_ *next;
    at_path_t	     type;		/* protocol type (from path type) */
    ulong	     leak_flags;	/* Flags indicate leak is needed */
    ulong	     unleak_flags;	/* Flags indicate unleak is needed */
    ulong	     leaked_flags;	/* Flags indicate leak has happened */
    atroute_leaker   leaker;		/* Leaking function */
    atroute_unleaker unleaker;		/* Unleaking function */
    atroute_all_leaker all_leaker; 	/* Leak all function */
    atroute_validate validate;		/* Validate a route */
} atalk_protocol_registry;


static tree_list_t  one_route_tree;	/* hard-wired until universes */
static tree_list_t* route_trees = NULL;	/* list of rb trees */
static queuetype    redist_queue;	/* Appletalk redistribute q */
static atalk_protocol_registry *atalk_redist_list = NULL;
static chunk_type   *redist_chunk;
static atalk_rdb_t  *atroute_GCPending;	/* list of rdb's needing cleaning */
static gccontext_t  *atroute_cleanup_gccontext;
static gccontext_t  *atroute_GC_gccontext;


inline treeKey IntervalKey (ushort low,ushort high)
{
    treeKey k;
    k.i.high = ((high == 0) ? low : high);
    k.i.low = low;
    return(k);
}

static inline void gcRedistQueue (void)
{

    /*
     * Wait until we have a function in chunk.c to collect
     * chunk siblings.
     */
}


#if DEAD
atalk_rdb_t *
atroute_GetRouteFromPath (atalk_pdb_t *p)
{
    return ((p != NULL) ? p->rdb : NULL);
}
#endif

static inline boolean
atroute_InsertRoute (rbTree *tree,ushort from,ushort to,atalk_rdb_t* r) {
    return (RBTreeIntInsert(from,to,tree,&r->link) != NULL);
}

static inline boolean
atroute_InsertPath (rbTree *tree,ushort from,ushort to,atalk_pdb_t* p) {
    return (RBTreeIntInsert(from,to,tree,&p->link) != NULL);
}

static void
route_Shutdown (void)
{
    int i;
    
    /* Clear all paths and routes. */
    atroute_ForAllRouteTreesDo(atroute_ClearPathsAndRoutes);

    /* GC all paths and routes */
    route_GC();
    for (i = 0; i < AT_PATH_MAX; i++)
	atroute_pathGC(i, atroute_rp_type(i));
}

/*
 * atroute_Initialize
 * Called to initialize the at_route.c module.
 */
void
atroute_Initialize (void)
{
    memset(&one_route_tree, 0, sizeof(one_route_tree));

    one_route_tree.r_tree = RBTreeCreate("Appletalk", "AT",
					 "Main AT RoutingTable",
					 at_print_key_interval,
					 &appletalk_debug->flag);
    one_route_tree.next = NULL;
    route_trees = &one_route_tree;
    atroute_InitPathTree(AT_PATH_PROXY, "AppleTalk proxy path tree");
    atroute_InitPathTree(AT_PATH_CONNECTED, "Connected AppleTalk Path Tree");
    atroute_GCPending = NULL;

    atroute_cleanup_gccontext = atmaint_AddGCContext(atroute_CleanUpRoute, 10);
    atroute_GC_gccontext = atmaint_AddGCContext(route_GC, 0);
    
    queue_init(&redist_queue, 5000);
    redist_chunk = chunk_create(sizeof(atalk_redist_t),
				ATALK_MIN_REDIST,
				CHUNK_FLAGS_DYNAMIC,
				NULL, 0,
				"Atalk Redist");
    /*
     * Create the chunks for pdb's and rdb's. These will be extensable
     * chunks...
     */
    if (redist_chunk == NULL) {
	errmsg(AT_ERR_NOMEM, "redist chunk", __LINE__, __FILE__);
	return;
    }
    /*
     * Add a free list for rdbs  and pdbs.  They tend to fragment 
     * memory otherwise.
     */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(atalk_rdb_t));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(atalk_pdb_t));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(ziptabletype));
    reg_add_atalk_shutdown(route_Shutdown, "route_Shutdown");
}


/*
 * atroute_InitPathTree
 *
 * Allocate and initialize a path tree.
 */
void
atroute_InitPathTree (ushort ptype, char *name)
{
    if (ptype > AT_PATH_MAX)
	return;
    
    one_route_tree.p_tree[ptype] = RBTreeCreate("Appletalk", "AT", name,
						at_print_key_interval,
						&appletalk_debug->flag);
}

/*
 * atroute_DeletePathTree
 *
 * Deallocate a path tree. NOTE -- you'd better make sure that
 * all nodes on the tree have been deleted before this function is called.
 */
void
atroute_DeletePathTree (ushort ptype)
{
    if (ptype > AT_PATH_MAX)
	return;
    
    if (one_route_tree.p_tree[ptype] != NULL) {
	free(one_route_tree.p_tree[ptype]);
	one_route_tree.p_tree[ptype] = NULL;
    }
}


/* 
 * atroute_StatusReport
 * Generate a status report for the Routing module.
 * We now dump stats about the main routing tree and all path
 * trees.
 */
void
atroute_StatusReport (void)
{
    rbTree	*tree;
    tree_list_t	*tl;
    ushort	ptype;

    for (tl = route_trees; tl != NULL; tl = tl->next) {
	tree = tl->r_tree;
	printf("\n  Routing statistics for %s:  %d GC run%s;"
	       "\n    Routes: %d new, %d freed, %d reused"
	       "\n    Paths:  %d new, %d freed, %d reused"
	       "\n    E-Paths:%d new, %d freed",
	       tree->tableName,
	       ArgAndPlural(tree->gcRuns,"","s"),
	       tl->rdbMallocs, tl->rdbFreed, tl->rdbRecycled,
	       tl->pdbMallocs, tl->pdbFreed, tl->pdbRecycled,
	       atalk_equalpathMallocs, atalk_equalpathFreed);
	RBTreeStats(tree, tree->tableName);
    }
    for (ptype = AT_PATH_CONNECTED; ptype < AT_PATH_MAX; ++ptype) {
	if (one_route_tree.p_tree[ptype]) {
	    tree = one_route_tree.p_tree[ptype];
	    printf("\n  RBtree statistics for path tree %s:  %d GC run%s;",
		   tree->tableName, ArgAndPlural(tree->gcRuns,"","s"));
	    RBTreeStats(tree, tree->tableName);
	}
    }
}

/*
 * route_ForAllRoutesDo
 *
 * Loop over all routes and apply the procedure, proc, to each route.
 * Relinquish the processor every SUSPEND_FREQUENCY routes.
 */
static inline void
route_ForAllRoutesDo (rbTree *tree, routeProc proc)
{
    atalk_rdb_t *thisroute, *nextroute;
    int routecounter = 0;

    if (tree == NULL)
	return;

#define SUSPEND_FREQUENCY 100

    thisroute = KeepFirstRoute(tree);	/* Prime with first route */

    /* Loop over all routes. */
    while (thisroute) {

	nextroute = KeepNextRoute(tree, thisroute);

	(*proc)(thisroute);		/* Apply the procedure to route. */

	/* Processed SUSPEND_FREQUENCY routes? */
	if ((routecounter = (routecounter + 1) % SUSPEND_FREQUENCY) == 0)
	    process_suspend();		/* Simple processor relinquishment. */

	thisroute = nextroute;
    }
#undef SUSPEND_FREQUENCY
}

/*
 * Clear all paths to route, r, and delete it.  Assumes calling routine has
 * protected the route.
 */
void
atroute_ClearPathsAndRoutes (atalk_rdb_t *r)
{
    atroute_ClearAllPaths(r);
    ReleaseRoute(r);
    atroute_DeleteRoute(r);
}

/* 
 * Loop over all routes in all route trees and apply the procedure, proc,
 * to each route.
 */
void
atroute_ForAllRouteTreesDo (routeProc proc)
{
    tree_list_t *treelist;

    for (treelist = route_trees; treelist; treelist = treelist->next)
	route_ForAllRoutesDo(treelist->r_tree, proc);
}
    

char const *
atroute_rp_type (at_path_t type)
{
    switch (type) {
    case AT_PATH_CONNECTED:
	return ("CONNECTED");
    case AT_PATH_STATIC:
	return("STATIC");
    case AT_PATH_IGRP:
	return ("EIGRP");
    case AT_PATH_RTMP:
	return ("RTMP");
    case AT_PATH_PROXY:
	return ("PROXY");
    case AT_PATH_AURP:
	return ("AURP");
#if ATALK_NOT_USED_YET
    case AT_PATH_TDRP:			/* tunnel, dr. pepper */
	return ("DRPEPPER");
    case AT_PATH_TCAY:			/* tunnel, Cayman */
	return ("CAYMAN");
#endif
      default:
	return ("unknown");
    }
}

/*
 * atroute_NeedGCRun
 * Checks if we need a GC run to be scheduled.  Returns TRUE as long
 * as the number of nodes which are free and not-busy exceeds the threshold.
 *
 * Note: "tree" could be either a route tree or a path tree.
 */
static boolean
route_NeedGCRun (rbTree *tree)
{
    int cnt;
    if (tree == NULL)
	return (FALSE);

    cnt = RBTreeReleasedNodeCount(tree);

    if (atalk_running)
	return (cnt > kMaxFreeRdbNodes);
    return (cnt != 0); /* If AT is disabled, free the entire list */
}

/*
 * route_periodic
 *
 * Age a route and handle zone update query generation.
 *
 */
static boolean
route_periodic (treeLink* link, void* pdata)
{
    atalk_rdb_t	*r = route_Cast(link);

    /*
     * Age the Zones associated with this route.
     */
    if (r->zoneupdate && !SLEEPING(r->zonetimer)) {
	atalk_clear_zones(r);
	r->zoneupdate = FALSE;
	r->zoneretries++;
    }

    /*
     * Do redistribute functions for all paths on redist queue.
     */
    atroute_process_redist_queue(&redist_queue);

    /*
     * If there are no paths left, see if any routing protocol
     * (i.e. EIGRP, AURP) can tell us anything that it has left 
     * in its path database.
     */
    if (r->changed && r->pathcount == 0) {
	reg_invoke_atalk_validate_route(r->dstrngstr, r->dstrngend);

	if (r->pathcount == 0 && r->rpath.type != AT_PATH_STATIC)
	    at_static_validate_route(r->dstrngstr,r->dstrngend);
    }
    r->changed = FALSE;

    /*
     * Make sure that there are no paths still under this route
     * before we bother to check it.
     */
    if (   r->pathcount == 0
	&& (r->rpath.dqLink.flink == &r->rpath.dqLink
	    && r->rpath.dqLink.blink == &r->rpath.dqLink)) {
	if (r->action == at_delete_route) {
	    if (r->holddown > 0) {
		r->holddown--;
	    } else {
		atroute_DeleteRoute(r);
	    }
	}
	atroute_RoutePoisonSent(r);
    }
    return (TRUE);
}

/*
 * atroute_redist_all
 *
 * Walk the entire routing tree, calling the "unredist" protocol function
 * for all protocols other than the protocol which is listed in rpath.type.
 * At first, we're going to do just the RTMP path unredist and let the
 * IGRP2 routes which have been bled into RTMP age out the hard way...
 */
void
atroute_redist_all (boolean distflag)
{
    atalk_protocol_registry *protocol;
    rbTree *tree;

    for (protocol = atalk_redist_list; protocol != NULL;
	 protocol = protocol->next) {
	if (protocol->all_leaker)
	    (*protocol->all_leaker)(distflag);
    }    
    /*
     * There is no file of 'connected path' routines. This attempts to do the
     * job for them.
     */
    tree = atroute_GetPTree(AT_PATH_CONNECTED);
    if (tree != NULL)
        RBTreeForEachNode(atroute_path_redist_rbshell, (void *) distflag,
			  tree, FALSE);
    
    /*
     * In the future, we're going to have to poison IGRP routes which
     * we're pulling out of RTMP. For now, we can let 'em age out.
     * This might sound simple, but there are a couple of considerations
     * which must be addressed:
     * - if we send a poison packet per IGRP route removed on every RTMP
     *   enabled interface, we could end up splattering a whole lot of
     *   packets out there.
     * - if we try to create some sort of scheme to build large packets
     *   of poisons, then we're going to have a rather complex
     *   path removal function here.
     * - if we wait until the next 10 second interval to go off,
     *   then we're possibly increasing the complexity without
     *   really doing much to improve routing performance.
     *
     * As much as I hate it, I'm in favor of creating a solution where
     * we scan the routing table for all IGRP-derived routes which
     * we're removing; when all these routes fill up a RTMP
     * packet with poison updates, then send it out every interface
     * which is listed as a connected interface in the connected path
     * tree.
     */
}

/*
 * atroute_validate_all
 *
 * This function is called to "validate" a route. This is done
 * when a routing protocol has just learned that it is now
 * receiving an inferior metric for a path is has previously heard and
 * installed. This function will call all registered Atalk routing
 * protocols which introduce routes into the forwarding table
 * and cause them to compare and possibly install their path
 * into the forwarding table as the best path to the destination.
 */
void
atroute_validate_all (ushort rngstart, ushort rngend)
{
    atalk_protocol_registry *protocol;

    for (protocol = atalk_redist_list; protocol != NULL;
	 protocol = protocol->next) {
	if (protocol->validate)
	    (*protocol->validate)(rngstart, rngend);
    }
}

/*
 * atroute_RegisterProtocol
 *
 * Register a protocol for redistribution
 */
void  atroute_RegisterProtocol (at_path_t	    type,
				ulong		    leak_flags,
				ulong		    unleak_flags,
				ulong		    leaked_flags,
				atroute_leaker	    leaker,
				atroute_unleaker    unleaker,
				atroute_all_leaker  all_leaker,
				atroute_validate    validate)
{
    atalk_protocol_registry *new_protocol;

    new_protocol = malloc(sizeof(atalk_protocol_registry));

    ATASSERT(new_protocol != NULL);
    if (new_protocol == NULL)
	return;

    new_protocol->next         = atalk_redist_list;
    atalk_redist_list          = new_protocol;
    new_protocol->type         = type;
    new_protocol->leak_flags   = leak_flags;
    new_protocol->unleak_flags = unleak_flags;
    new_protocol->leaked_flags = leaked_flags;
    new_protocol->leaker       = leaker;
    new_protocol->unleaker     = unleaker;
    new_protocol->all_leaker   = all_leaker;
    new_protocol->validate     = validate;
}

/*
 * Unregister a protocol; ie, pull a protocol out of the redistribution
 * list.
 */
void
atroute_UnregisterProtocol (at_path_t type)
{
    atalk_protocol_registry	*protocol = NULL;
    atalk_protocol_registry	*prev = NULL;

    for (protocol = atalk_redist_list; protocol != NULL;
	 protocol = protocol->next) {
	if (protocol->type == type) {
	    if (prev == NULL) {
		atalk_redist_list = atalk_redist_list->next;
	    } else {
		prev->next = protocol->next;
	    }
	    free(protocol);
	    break;
	}
    }
}

/*
 * atroute_process_redist_queue
 *
 * Process redistribution requests
 */
static void
atroute_process_redist_queue (queuetype *q)
{
    atalk_pdb_t 	*p = NULL;
    atalk_pdb_t		tmp_path;
    atalk_redist_t	*w;
    rbTree		*p_tree;
    atalk_protocol_registry *protocol;


    while ((w = dequeue(q)) != NULL) {
	for (protocol = atalk_redist_list; protocol != NULL;
	     protocol = protocol->next) {

	    /*
	     * Check for leaking into our own type -- this is completely
	     * pointless, but slightly amusing.
	     */
	    if (protocol->type == w->type)
		continue;

	    p_tree = atroute_GetPTree(w->type);
	    if (p_tree == NULL) {
		if (at_debug(atalkerr_debug,0))
		    buginf("\nAT: atroute_process_redist_queue: queue elt w/"
			   "invalid type %d", w->type);
		free(w);
		return;
	    }
	    /*
	     * Look at the leak/unleak flags to see what (if anything)
	     * we have to do. Don't just blindly re-dist paths,
	     * since doing that on every single RTMP update will
	     * get you out of memory in a big hurry.
	     * and then the external metric changes...
	     */
	    p = atroute_LookupPath(p_tree, w->rng_start);
	    if (p == NULL) {
		if (at_debug(atalkerr_debug,0))
		    buginf("\nAT: atroute_process_redist_queue: NIL path for/"
			   "network %d, assuming unredist", w->rng_start);
		w->action = at_delete_path;
		memset(&tmp_path, 0, sizeof(tmp_path));
		tmp_path.dstrngstr = w->rng_start;
		tmp_path.dstrngend = w->rng_end;
		tmp_path.metric.metric = METRIC_INACCESSIBLE;
		tmp_path.metric.u.rtmp.hops = DDP_POISON_HOPCOUNT;
		tmp_path.metric.u.igrp.bandwidth = METRIC_INACCESSIBLE;
		tmp_path.metric.u.igrp.delay = METRIC_INACCESSIBLE;
		tmp_path.type = w->type;
		p = &tmp_path;
	    }

	    /* Leak it? */
	    switch (w->action) {
	      case at_delete_path:
		if (protocol->unleaker) {
		    if (at_debug(atalkredist_debug, 0))
			buginf("\nAT: Unredistribution of %s path, %#A to %s",
			       atroute_rp_type(p->type),
			       atutil_CableRange(p->dstrngstr, p->dstrngend),
			       atroute_rp_type(protocol->type));
		    (*protocol->unleaker)(p);
		}
		p->flags &= ~(protocol->leaked_flags);
		break;
	      case at_add_path:
	      case at_change_path:
		/*
		 * This is an odd case -- we might have already
		 * dist'ed the path into the other protocol, but
		 * it has now changed. The DUAL stuff doesn't
		 * differentiate between doing addition and modification,
		 * so we're just going to leak it again.
		 */
		if (protocol->leaker) {
		    if (at_debug(atalkredist_debug, 0))
			buginf("\nAT: Redistribution of %s path, %#A to %s",
			       atroute_rp_type(p->type),
			       atutil_CableRange(p->dstrngstr, p->dstrngend),
			       atroute_rp_type(protocol->type));
		    (*protocol->leaker)(p);
		}
		p->flags |= protocol->leaked_flags;
		break;
	      default:
		if (at_debug(atalkerr_debug,0))
		    buginf("\nAT: process_redist_queue: invalid action %d",
			   w->action);
		break;
	    }
	}
	chunk_free(redist_chunk, w);
    }
}

/*
 * atroute_set_path_redists
 *
 * Set a path to begin or end redistribution
 */
static boolean
atroute_set_path_redists (queuetype *q, atalk_pdb_t *p, boolean unredistribute)
{
    atalk_redist_t	*redist;

    if (!(atalk_redist_status & ATALK_REDIST_ENABLED))
	return (FALSE);

    if (p == NULL)
	return (FALSE);
    if ((p->state >= at_state_bad) && !unredistribute)
        return (FALSE);
    redist = chunk_malloc(redist_chunk);
    if (redist == NULL)
	return (FALSE);
    if (unredistribute) {
	redist->action = at_delete_path;
    } else {
	redist->action = at_add_path;
    }
    redist->rng_start = p->dstrngstr;
    redist->rng_end   = p->dstrngend;
    redist->type      = p->type;
    redist->next = NULL;
    enqueue(q, redist);
    return (TRUE);
}

/*
 * atroute_path_redistribute and atroute_path_unredistribute
 * Unredistribute a path from the protocols it is in
 */
boolean
atroute_path_redistribute (atalk_pdb_t *path)
{
    ATASSERT(path != NULL);
    if (path == NULL)
        return (FALSE);
    return (atroute_set_path_redists(&redist_queue, path, FALSE));
}

boolean
atroute_path_unredistribute (atalk_pdb_t *path)
{
    ATASSERT(path != NULL);
    if (path == NULL)
	return (FALSE);
    return (atroute_set_path_redists(&redist_queue, path, TRUE));
}

boolean
atroute_path_redist_rbshell (treeLink *p, void *unredistflag)
{
    return (atroute_set_path_redists(&redist_queue, (atalk_pdb_t *)p,
				     (boolean)unredistflag));
}

/* atroute_PathListInsert
 */
static void
atroute_PathListInsert(atalk_rdb_t *r, atalk_pdb_t *p)
{
    atalk_pdb_t *path;
    dqueue_t    *dq, *dqhead, *ndq;

    dqhead = dq = &r->rpath.dqLink;

    /* Find appropriate place to insert path.  dqhead is a sentinel node. */
    while ((ndq = dq->flink) != dqhead) {
	path = path_Cast(ndq->parent);
	if (atroute_MetricCompare(&p->metric, &path->metric,
				  ATALK_METRIC_LT))
	    break;
	dq = dq->flink;
    }
    lw_insert(&p->dqLink, dq);
    p->dqLink.parent = p;
}

/*
 * atroute_MetricCompare
 *
 * Compare two metrics. Return TRUE if metric of path A is in releation
 * requested to metric of path B
 *
 * EQ is TRUE iff type of route and metric are equivalent
 * GT is TRUE iff metric of A > metric of B, or A == B & type A > type B
 *
 *
 * When we are comparing a EIGRP path which is built on an external metric
 * introduced by RTMP and a RTMP path, prefer whichever has fewer
 * RTMP hops.
 */
boolean atroute_MetricCompare (atalk_metric_t *a, atalk_metric_t *b,
			       atalk_metric_compare_t rel)
{
    boolean gt, eq;

    gt = eq = FALSE;

    if (!(a->type > AT_PATH_MIN && a->type < AT_PATH_MAX)) {
	if (at_debug(atalkerr_debug, 0))
	    buginf("\natroute_MetricCompare: invalid metric type A %d B %d",
		   a->type, b->type);
	return (FALSE);
    }

    /*
     * Check the composite metric instance first, ie, a EIGRP-to-EIGRP
     * or RTMP-to-RTMP comparison.
     */
    if (a->metric > b->metric) {
	gt = TRUE;
    } else if (a->metric == b->metric) {
	if (a->type > b->type) {
	    gt = TRUE;
	} else if (a->type == b->type) { 
	    eq = TRUE;
	}
    }

    if (   (   a->type == AT_PATH_RTMP
	    && (b->type == AT_PATH_IGRP && b->u.igrp.external))
	|| (   (a->type == AT_PATH_IGRP && a->u.igrp.external)
	    && (b->type == AT_PATH_RTMP))) {
	if (a->type == AT_PATH_RTMP) {
	    gt = (a->u.rtmp.hops > b->u.igrp.ext_hops);
	    eq = (a->u.rtmp.hops == b->u.igrp.ext_hops);
	} else {
	    gt = (a->u.igrp.ext_hops > b->u.rtmp.hops);
	    eq = (a->u.igrp.ext_hops == b->u.rtmp.hops);
	}
    }


    if(a->metric == AT_METRIC_FLOATING)
	gt = TRUE;
    if(b->metric == AT_METRIC_FLOATING)
	gt = FALSE;

    switch (rel) {
      case ATALK_METRIC_EQ:
	return (eq);
      case ATALK_METRIC_NE:
	return (!eq);
      case ATALK_METRIC_GT:
	return (gt);
      case ATALK_METRIC_GE:
	return (gt || eq);
      case ATALK_METRIC_LT:
	return (!(eq || gt));
      case ATALK_METRIC_LE:
	return (!gt);
    }
    ATASSERT(FALSE);			/* shouldn't get here. */
    return (FALSE);
}

/*
 * atroute_BestPath
 *
 * This routine is called with a pointer to a path which has changed;
 * the path has had one of the following happen:
 * - path was added
 * - path's metric changed
 * - path is being/was deleted
 */
boolean
atroute_BestPath (atalk_pdb_t *p)
{
    if (p)
	return (atroute_BestPathOnRoute(p->rdb));
    else
	return (FALSE);
}
/*
 * atroute_BestPathOnRoute
 *
 * Find and elect the best path of all paths on the path list for a route.
 * We have to elect or re-elect a new path when one of three events happens:
 * 1. A path is created and added to a route.
 * 2. A path is modified (ie, its metric goes up or down)
 * 3. A path is deleted from a route.
 *
 * This function assumes that all the paths to be taken into consideration
 * in the selection of best (and equal paths) are currently attatched to
 * the path list.
 *
 * Special notes on connected (idb & proxy route) paths:
 * These are maintained in the rpath portion of the rdb just as
 * the updated information from a derived path would be, but the
 * flags are set to indicate that there would be no successive
 * paths on the path list following the rpath. This function
 * checks the connected/proxy flags and handles the rpath
 * accordingly, thereby removing gobs of special-case code from
 * throughout the rest of the routing module for connected routes.
 *
 */
boolean
atroute_BestPathOnRoute (atalk_rdb_t *r)
{
    atalk_pdb_t	*p;
    atalk_pdb_t	*old_bp;
    boolean	changed_route = FALSE;
#if ATALK_NOT_IMPLEMENTED_YET
    atalk_pdb_t	*np,*first_path;	/* path pointer for walking equals */
    dqueue_t	*dq, *dqhead;
#endif

    ATASSERT(r != NULL);
    if (r == NULL)
	return (FALSE);
    /*
     * Here we determine if the change on the path list should be leaked.
     * There are three conditions under which the route needs to be leaked:
     * 1. This is a new path (connected or not) and is the only path
     *    on the route's path list. best_path will be NULL.
     * 2. The current best path is no longer the best path; ie, we have
     *    a new best path to the location. (metric change) best_path
     *    doesn't point to the first path on the list.
     * 3. The last path on this route has been deleted; best_path will
     *    be NULL and pathcount will be 0.
     * 4. There is only one path on the path list, but its metric has
     *    changed.
     */
    old_bp = r->best_path;

    if (old_bp == NULL) {
	changed_route = TRUE;
	/*
	 * just added or last path deleted -- figure out which.
	 */
	if (r->pathcount == 0 && (r->rpath.dqLink.flink == &r->rpath.dqLink)){
	    /*
	     * There are no paths left on this route. Mark the path as
	     * impossible to reach, put it into notify and let other protocols
	     * notify their neighbors as necessary.
	     */
	    r->action = at_notify0_route;

	    r->rpath.idb = NULL;
	    atrtmp_SetMetric(&r->rpath.metric, DDP_POISON_HOPCOUNT);
	    r->best_path = NULL;
	    GET_TIMESTAMP(r->wentbad);
	    /*
	     * We want to age in one minute, and the ager runs once
	     * every rtmpUpdateTime mseconds.
	     */
	    if (r->rpath.type == AT_PATH_IGRP) {
	        r->holddown = AT_IGRP_HOLDDOWN / (atalk_rtmpUpdateTime / ONESEC);
	    }
	    return (TRUE);
	} else {
	    /*
	     * Else, a path was just added to this route and we're going
	     * to use it.
	     */
	    r->action = at_add_route;
	}
    } else {
	/*
	 * Pick up a pointer to the first path on the route. The
	 * first path should be the one with the best cost.
	 */
	p = path_Cast(r->rpath.dqLink.flink->parent);

	/*
	 * Pay attention here. If you want to change this code,
	 * you should understand what you are doing.
	 *
	 * When a better path than the one we currently have in the
	 * best_path slot appears, we have to unredist said path and
	 * tell the routing protocol in question that we are no longer
	 * using the path.
	 * This is done by comparing the new path to the existing best path;
	 * if the new path's metric is less than the old path's metric,
	 * then unredist the old path and redist the new path.
	 *
	 * We check the type of the new path and old path; if they
	 * are the same, then we simply compare the composite metric.
	 * If the paths are dissimilar, then we use the expensive
	 * atroute_MetricCompare() function.
	 */
	if (r->rpath.metric.type == p->metric.type) {
	    if ((r->rpath.metric.metric > p->metric.metric) || (old_bp != p)) {
		if (old_bp->path_use)
		    (*old_bp->path_use)(old_bp, at_path_unuse);
		atroute_path_unredistribute(old_bp);
	    } else if (   (p == old_bp)
		       && (r->rpath.metric.metric != p->metric.metric)) {
		/* The same path has CHANGED somehow, dudes... */
		r->action = at_change_route;
 		changed_route = TRUE;
	    }
	} else {			/* dissimilar path types */
	    if (   atroute_MetricCompare(&r->rpath.metric,&p->metric,
					 ATALK_METRIC_GT)
		|| old_bp != p) {
		if (old_bp->path_use)
		    (*old_bp->path_use)(old_bp, at_path_unuse);
		atroute_path_unredistribute(old_bp);
	    } else if ((p == old_bp) &&
		       (atroute_MetricCompare(&r->rpath.metric, &p->metric,
					      ATALK_METRIC_NE))) {
		/* The same path has CHANGED somehow, dudes... */
		r->action = at_change_route;
		changed_route = TRUE;
	    }
	}
    }
    /*
     * If we are here, we have a path. Clear holddown.
     */
    r->holddown = 0;

    /*
     * The best path should be the first path on the list.
     * For connected & proxy paths, this will turn out to the
     * the rpath entry, since when the list is empty, it points
     * to itself.
     */
    p = r->best_path = path_Cast(r->rpath.dqLink.flink->parent);    
    if (p->path_use)
	(*p->path_use)(p, at_path_use);	/* we're going to use it... */

    /*
     * If we have changed paths or the only path we had changed
     * (the metric changed, the gateway changed, etc), then
     * copy all the information out of the new best path into the
     * route 'path'. This will be used throughout the code rather
     * than a second dereference from ->best_path...
     */
    atroute_UpdateRoute(r,p);

    if (changed_route) {
	r->changed = TRUE;

	/* Now let the domain module know that there is a path change */
	atdomain_PathUpdate(r);

	/* Redistribute the path only if zone list is complete */
	if (!r->zoneupdate && atalk_firstZone(r))
	    atroute_set_path_redists(&redist_queue, r->best_path, FALSE);
    }

    if (r->best_path) {
        if (r->best_path->equalpath == NULL)
            r->best_path->equalpath = r->best_path;
    }

    return (TRUE);
}


void
atroute_UpdateRoute(atalk_rdb_t *r, atalk_pdb_t *p)
{
    /*
     * Copy all the information out of the path into the
     * route 'path'. This will be used throughout the code rather
     * than a second dereference from ->best_path...
     */
    r->rpath.idb      = p->idb;
    r->rpath.gwptr     = p->gwptr;
    r->rpath.gateway   = p->gateway;
    r->rpath.metric    = p->metric;
    r->timer           = p->timer;
    r->rpath.type      = p->type;
    r->rpath.state     = p->state;
}

/*
 * atroute_GetValidEqualPath - 
 *
 *   For load balancing, we have the luxury to pick only
 *   the path is in good health since we have multiple
 *   paths.
 */
static atalk_pdb_t*
atroute_GetValidEqualPath (atalk_pdb_t *p)
{
   atalk_pdb_t *next;

   next = p->equalpath;
   while (next != p) {
       if (next->state == at_state_good)
	   break;

       if (at_debug(atalklb_debug, ATALK_NULL_MASK))
           buginf("GetValidEqualPath::idb = %s, state = %c, %u-%u\n",
                  idb_get_namestring(next->idb->phys_idb), 
		  atroute_PathState(next),
                  next->dstrngstr, next->dstrngend);

       next = next->equalpath;
   }

   return (next);
}

/*
 * atroute_SwitchEqualPath -
 * 
 *   Here we do the magic work, switching path etc...
 */
idbtype*
atroute_SwitchEqualPath (atalk_rdb_t *r, ataddrtype *nexthop)
{
    if (r->best_path == NULL) {
        if (r->rpath.idb && at_debug(atalklb_debug, ATALK_NULL_MASK))
            buginf("SwitchEqualPath::idb = %s, next hop = %d.%d\n",
                   idb_get_namestring(r->rpath.idb->phys_idb), 
		   atalk_netonly(r->rpath.gateway), 
		   atalk_nodeonly(r->rpath.gateway));
	*nexthop = r->rpath.gateway;
	return (r->rpath.idb ? r->rpath.idb->phys_idb : NULL);
    }

    if (r->next_equalpath == NULL) {
        r->next_equalpath = r->best_path;
    }
    else {
        if (r->next_equalpath->type != r->best_path->type)
	    r->next_equalpath = r->best_path;
	else
            r->next_equalpath = atroute_GetValidEqualPath(r->next_equalpath);
    }

    *nexthop = r->next_equalpath->gateway;

    if (at_debug(atalklb_debug, ATALK_NULL_MASK))
        buginf("SwitchEqualPath::idb = %s, next hop = %d.%d, %u-%u\n",
               idb_get_namestring(r->next_equalpath->idb->phys_idb), 
               atalk_netonly(*nexthop), atalk_nodeonly(*nexthop),
	       r->next_equalpath->dstrngstr, r->next_equalpath->dstrngend);

    return (r->next_equalpath->idb->phys_idb);
}

/*
 * Modify a path's metric. This means it may move in the list of paths
 * for this route.
 */
void 
atroute_NewPathMetric (atalk_rdb_t *r, atalk_pdb_t *p)
{
    dqueue_t    *dq;

    if (!(p && r))
	return;
    /*
     * First, delete it...
     * if no other path on the list, then this route will disappear.
     */
    dq = p->dqLink.blink;
    if (dq == &r->rpath.dqLink)
	r->best_path = NULL;	/* no other path on the list */
    else
	r->best_path = path_Cast(dq->parent);
    
    lw_remove(&p->dqLink);			/* unlink from route's path list */
    /*
     * Now, insert it back...
     *
     * ??? should insert at the head of the list:
     * - if cost is equal && our pathtype is less than the next pathtype
     * - if new cost is less than the previous first node cost.
     */
    atroute_PathListInsert(r, p);

    atroute_BestPathOnRoute(r);
}

/*
 * atalk_route_adjust
 * fix up the appletalk routing table when an interface flaps.
 * This is called from 'route_adjust' in if/network.c when an
 * interface changes state.
 *
 * Since this routine will fire 'atroute_DisablePort' via the StateHandler,
 * the best place to handle marking the paths 'invalid' is in
 * atroute_DisablePort, then handle the adjustment of the routes to use new
 * paths (if any exist) in the routing function, called from the maint
 * process.
 */
void
atalk_route_adjust (idbtype *phys_idb)
{
    atalkidbtype *idb;

    idb = atalk_getidb(phys_idb);
    
    if (!(idb && idb->atalk_enabled && atalk_running))
	return;

    /*
     * If the interface just came up, but the atalk state is still down,
     * restart the port.
     */
    if (interface_up(phys_idb)) {
	if (atalkif_opstatus(idb) == opNetDown)
	    atalkif_RestartPort(idb);
    } else {
	/*
	 * If the interface is down and our state doesn't show same, then
	 * begin restarting the port, which will take it down.
	 */
	if (atalkif_opstatus(idb) != opNetDown)
	    atalkif_RestartPort(idb);
    }
}

/*
 * atroute_AddProxyPath
 * Add a proxy route to our routing tables.
 *
 * A proxy is a 'special-case' route. There is no 'real' path created
 * for a proxy, but due to the way the path/route blocks are divied up,
 * we have to create a pdb for the route.
 */
boolean
atroute_AddProxyPath (ushort net,int len,uchar* buf, boolean internal)
{
    atalk_rdb_t *r = NULL;
    atalk_pdb_t *p = NULL;
    rbTree	*p_tree;
    rbTree	*r_tree;
    ziptabletype* z = NULL;
    atalk_metric_t metric;

    /*
     * If there is already a proxy for the specified net, then
     * don't add it.
     */
    p_tree = atroute_GetPTree(AT_PATH_PROXY);
    p = atroute_LookupPath(p_tree, net);
    if (p != NULL) {
	printf("%% Net %d is already in use as a proxy\n",net);
	return (FALSE);
    }
    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, net, FALSE);
    if (r != NULL) {
	printf("%% Net %d is already in use\n", net);
	return (FALSE);
    }
    memset(&metric, 0, sizeof(metric));
    metric.metric = AT_METRIC_PROXY;
    metric.type = AT_PATH_PROXY;
    p = atroute_AddPath(p_tree, atalk_idbQ->qtail,
			net, 0,
			NULL,		/* no neighbor */
			atroute_DefaultPathAction,
			NULL,
			0, AT_PATH_PROXY,
			&metric);
    if (p == NULL) {
	printf("%% Unable to add net %d\n",net);
	return (FALSE);
    }
    p->internal = internal;
    if (!atroute_BestPath(p)) {
	printf("\n%%Unable to compute best path in AddProxy");
	return (FALSE);
    }

    r = atroute_LookupRoute(r_tree, net, FALSE);
    if (r == NULL) {
	printf("%% Net %d cannot be defined\n", net);
	return (FALSE);
    }
    z = atzip_AddZone(r,buf,len);
    if (z == NULL) {
	atroute_DeletePath(p);
	printf("%% Unable to associate zone to net %d",net);/*???*/
	return (FALSE);
    }
    
    /* Now redistribute proxy path */
    atroute_path_redist_rbshell((treeLink *) p, FALSE);

    if (atalk_events)
	errmsg(AT_ERR_ZONEPROXY, z->pname, atutil_CableRange(net, 0)); 
    return (TRUE);
}

/*
 * atroute_DeleteProxyPath
 * Delete a proxy path. The route ager will delete the route once
 * the path has disappeared.
 */
void
atroute_DeleteProxyPath (ushort net)
{
    atalk_pdb_t	*p;
    atalk_rdb_t	*r;
    ziptabletype *z;

    if ((p = atroute_LookupPath(atroute_GetPTree(AT_PATH_PROXY), net))) {
	r = p->rdb;
	z = atalk_PrimaryZone(r);
	r->flags = 0;
	atroute_DeletePath(p);		/* let maint delete route */
	if (atalk_events && (z!=NULL))
	    errmsg(AT_ERR_NOPROXY, z->pname, atutil_CableRange(net, 0));
    }
}
	
/*
 * atroute_IsProxyRoute
 * Quickly check for a proxy route entry derived from a proxy path.
 */
boolean
atroute_IsProxyRoute (rbTree *tree, ushort net)
{
    atalk_rdb_t* r;
    r = atroute_LookupRoute(tree,net, FALSE);
    return (r && atalk_net_proxy(r));
}

/*
 * atroute_FirstHop
 * return first hop for given datagram based on dest. address
 *
 * OK, for universes and WANTalk, we're going to need to determine which
 * universe this packet is destined for as well as the other grungy details.
 * The universe determination has to be done first.
 *
 * Return Value Interpretations (if idb->output == NULL):
 *		 0 = no route to destination
 *		-1 = bad destination
 * if idb->output != NULL:
 *		 0 = destination is for me or last router or marshal
 *		-1 = destination is broadcast
 * last router is used only when we are not operational and destination is
 * not on the output cable.
 *  
 */
ataddrtype
atroute_FirstHop (paktype* pak)
{
    atalk_rdb_t *r;
    atalkidbtype	*idb = NULL;
    ataddrtype	dstaddr;
    rbTree	*tree;
    addrTypes	destination_type;
    ushort	net;
    ataddrtype  nexthop;

    dstaddr = atutil_DestAddr(pak);

    /* Make a special check for packets with dst nets in the startup range */
    net = atalk_netonly(dstaddr);
    if (net >= ATALK_STARTUP_START && net <= ATALK_STARTUP_END) {
	/* If pak->if_input is NULL, atalk_send1 will drop packet, so we're
	 * covered.
	 */
	pak->if_output = pak->if_input;
	return (dstaddr);
    }

    idb = atalk_getidb(pak->if_output);
    if (idb != NULL) {
	/* We have overridden routing, so check the address and handle. */
	destination_type = atalkif_DstAddrType(idb, dstaddr);
#ifdef ATYPE_DEBUG
	atalk_PrintAddrType(idb, dstaddr, destination_type);
#endif
	switch (destination_type) {
	  case dstCableBroadcast:
	    return (-1);
	  case dstForCableRouter:
	  case dstIsMyAddress:
	    return (0);
	  case dstForThisCable:
	    return (dstaddr);
	  case dstNotForCable:
	    if (idb->atalk_defaultgw != 0) {
		atalk_stat[ATALK_USED_DEFGW]++;
		return (idb->atalk_defaultgw);
	    }
	    /* Fall through */
	case addrIsInvalid:
	    if (atalkif_CaymanTunnel(idb))
		return (-1);
	default:
	    atalk_stat[ATALK_DADDR_INVALID]++;
	    pak->if_output = NULL;
	    return (-1);
	}
    }
    tree = atroute_GetTree();
    r = atroute_LookupRoute(tree, atalk_netonly(dstaddr), FALSE);
    if ((r == NULL) || (r->pathcount == 0))
	return (0);		/* No route indication */
    if (at_debug(atalk_events, ATALK_NULL_MASK))
        ATASSERT(r->rpath.idb  != NULL);
    r->uses++;

    if (atroute_GetEqualPathCount(r->best_path)) {
        pak->if_output = atroute_SwitchEqualPath(r, &nexthop);
    }
    else {
        pak->if_output = (r->rpath.idb ? r->rpath.idb->phys_idb : NULL);
	nexthop = r->rpath.gateway;
    }
    idb = atalk_getidb(pak->if_output);

    destination_type = atalkif_DstAddrType(idb, dstaddr);
#ifdef ATYPE_DEBUG
    atalk_PrintAddrType(idb, dstaddr, destination_type);
#endif
    switch (destination_type) {
      case dstCableBroadcast:
	return (-1);
      case dstIsMyAddress:
	return (0);
      case dstNotForCable:
	return (nexthop);
      case dstForThisCable:
	return (dstaddr);
      case dstForCableRouter:
	return (0);
      case addrIsInvalid:
      default:
	atalk_stat[ATALK_DADDR_INVALID]++;
	pak->if_output = NULL;
	return (-1);
    }
}

/*
 * atroute_AddRoute
 * Add an appletalk route to the table
 */
static atalk_rdb_t*
atroute_AddRoute (rbTree *tree,
		  atalkidbtype * idb, ushort dstrngstr,ushort dstrngend,
		  neighborentry* n,
		  ulong flags)
{
    tree_list_t	*tl = route_trees;
    register atalk_rdb_t *r;
    
    if (tree == NULL)
	return (NULL);
    /*
     * If the route being added is fully-extended in a compatibility-mode
     * internet, this is an error.
     */
    if (dstrngend != 0 && (idb->atalk_cableend != ATALK_RANGE_UNKNOWN)) {
	if ((dstrngstr != dstrngend) && (dstrngstr != 0)) {
	    atalk_extendedInternet = TRUE;
	    if (atalk_obsoleteRouterActive && !atalk_extendedInternet) {
		errmsg(AT_ERR_COMPATERR3,
		       atutil_CableRange(dstrngstr,dstrngend));
		atalkif_ConfigError(idb,"internet in compatibility mode");
		return (NULL);
	    }
	}
    }
    r = route_Cast(RBTreeGetFreeNode(tree));
    if (r != NULL) {
	memset(r, 0, sizeof(atalk_rdb_t));
	tl->rdbRecycled++;
    } else {
	if ((r = malloc(sizeof(atalk_rdb_t))) != NULL) {
	    tl->rdbMallocs++;
	}
	if (r == NULL)
	    return (NULL);
    }

    r->tree = tree;
    r->znext = NULL;
    GET_TIMESTAMP(r->started);
    COPY_TIMESTAMP(r->started, r->timer);
    TIMER_STOP(r->wentbad);		/*???*/
    r->flags = 0;			/* Temporarily cleared */
    r->dstrngstr = dstrngstr;
    r->dstrngend = dstrngend;
    r->pathcount = 0;
    r->rtmp_equalcount = 0;
    r->eigrp_equalcount = 0;
    r->next_equalpath = NULL;
    r->best_path = NULL;
    r->uses = 0L;
    r->zoneupdate = FALSE;
    r->zoneaddr = 0L;
    r->zonecnt = 0;
    r->zonelist = NULL;

    InitListItem(&r->maintLink);
    /*
     * Init the rpath flink/blink to point to NULL.
     */
    r->rpath.dqLink.parent = &r->rpath;
    r->rpath.dqLink.flink = r->rpath.dqLink.blink = &r->rpath.dqLink;
    r->rpath.rdb = r;

    if(idb)
	r->domain = idb->atalk_domain;

    if (!atroute_InsertRoute(tree,dstrngstr,dstrngend,r)) {
	if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK)) {
	    errmsg(AT_ERR_DEBUGMSG,"Unable to insert route into tree");
	}
	free(r);
	return (NULL);
    }
    /* Set the flags now, since we are assured OK completion */
    r->flags = flags;
    return (r);
}

boolean 
atroute_AddEqualPath (atalk_pdb_t *p,
		      idbtype *phys_idb,
		      ushort dstrngstr, ushort dstrngend,
		      neighborentry *n,
		      ushort type,
		      atalk_metric_t *metric)
{
    atalk_pdb_t *next, *prev, *np;
    short count;
    atalkidbtype *idb;

    if (p->state != at_state_good)
        return (FALSE);

    if (p->type != AT_PATH_RTMP && p->type != AT_PATH_IGRP)
        return (FALSE);

    count = atroute_GetEqualPathCount(p) + 1;
    if (count >= atalk_MaxPaths)
        return (FALSE);

    prev = next = p;
    while (count--) {
        if (!atroute_MetricCompare(&next->metric, metric, ATALK_METRIC_EQ) ||
	    (next->idb->phys_idb == phys_idb))
	    return (FALSE);

        prev = next;
        next = next->equalpath;
    }

    np = malloc(sizeof(atalk_pdb_t));
    if (np == NULL) {
        if (at_debug(atalklb_debug, ATALK_NULL_MASK))
	    buginf("Unable to create equal-cost path.\n");
	return (FALSE);
    }
    bzero(np, sizeof(atalk_pdb_t));
    idb = atalk_getidb(phys_idb);
    memcpy(np, idb->atalk_path, sizeof(atalk_pdb_t));
    memcpy(&np->metric, metric, sizeof(atalk_metric_t));
    np->idb = idb;
    np->state = at_state_good;
    np->type = type;
    prev->equalpath = np;
    np->equalpath = p;

    if (n != NULL) {
        atmaint_ProtectNeighbor(n);
	np->gateway = n->address;
	np->gwptr = n;
    } else {
	np->gateway = 0;
	np->gwptr = NULL;
    }
    
    np->dstrngstr = dstrngstr;
    np->dstrngend = dstrngend;
    memcpy(&np->metric, metric, sizeof(atalk_metric_t));
    atalk_rtmp_set_timer(np);
    np->started = np->timer;
    atroute_UpdateEqualPathCount(p, TRUE);
    atalk_InvalidateCache("atroute_AddEqualPath");
    atalk_equalpathMallocs++;
    return (TRUE);
}

/*
 * Add a path. Most routing protocol modules will add paths, not
 * routes. The EXEC will add routes, because there are proxy routes
 * and connected routes, which are not learned thru the net.
 *
 * There are a zillion parameters to this function. They are:
 *
 * - tree	the path tree in which the PDB will be inserted.
 * - idb	the idb on which this path is active.
 * - dstrngstr, dstrngend 	start/end of the cable range
 * - n		neighbor which told us this path
 * - path_action	ptr to function used to set path state (may be NULL)
 * - path_use		ptr to function use to notify RP of path usage
 *			(may be NULL)
 * - flags
 * - type	AT_PATH_{CONNECTED,PROXY,STATIC,IGRP,RTMP,AURP}
 * - metric	path's metric
 */
atalk_pdb_t*
atroute_AddPath (rbTree *p_tree,
		 atalkidbtype *idb,
		 ushort dstrngstr, ushort dstrngend,
		 neighborentry *n,
		 boolean (*path_action)(atalk_pdb_t *, at_state_t),
		 boolean (*path_use)(atalk_pdb_t *, at_path_use_t),
		 ulong flags,
		 ushort type,
		 atalk_metric_t *metric)
{
    atalk_pdb_t *p;
    atalk_rdb_t	*r;
    boolean	new_route = FALSE;
    rbTree	*r_tree;
    tree_list_t *tl = route_trees;

    if (!p_tree)
	return (NULL);
    /*
     * See if we are adding a path to an existing route; if no route
     * currently exists, then create one.
     */
    r_tree = atroute_GetTree();		/* get route tree */
    if ((r = atroute_LookupRoute(r_tree, dstrngstr, TRUE)) == NULL) {
	r = atroute_AddRoute(r_tree, idb, dstrngstr, dstrngend, n, flags);
	if (r == NULL)
	    return (NULL);
	new_route = TRUE;
    } else {
	/*
	 * If we already have a route with a lower-cost path, don't
	 * bother adding this path.
	 */
	if (atroute_MetricCompare(&r->rpath.metric, metric, ATALK_METRIC_LT)) {
	    return (NULL);
	}

	/*
	 * If we are better cost than this path, then delete the path
	 * and unredist it.
	 */
	atroute_DeletePath(r->best_path);
    }

    p = path_Cast(RBTreeGetFreeNode(p_tree));
    if (p == NULL) {
	p = malloc(sizeof(atalk_pdb_t));
	if (p == NULL) {
	    errmsg(AT_ERR_NOMEM, "pdb", __LINE__, __FILE__);
	    if (new_route)
		atroute_DeleteRoute(r);	/* no point in keeping it around */
	    return (NULL);
	}
	tl->pdbMallocs++;
    } else
	tl->pdbRecycled++;

    p->metric.type               = type;
    p->metric.metric             = metric->metric;
    p->metric.u.igrp.hops        = metric->u.igrp.hops;
    p->metric.u.igrp.bandwidth   = metric->u.igrp.bandwidth;
    p->metric.u.igrp.delay       = metric->u.igrp.delay;
    p->metric.u.igrp.reliability = metric->u.igrp.reliability;
    p->metric.u.igrp.mtu         = metric->u.igrp.mtu;
    p->metric.u.igrp.load        = metric->u.igrp.load;
    p->metric.u.igrp.external    = metric->u.igrp.external;
    if (p->metric.u.igrp.external) {
	p->metric.u.igrp.ext_source   = metric->u.igrp.ext_source;
	p->metric.u.igrp.ext_hops     = metric->u.igrp.ext_hops;
	p->metric.u.igrp.ext_proto_id = metric->u.igrp.ext_proto_id;
	p->metric.u.igrp.ext_flag     = metric->u.igrp.ext_flag;
    }

    /* Got everything we need; start adding. */
    p->rdb = r;
    p->idb = idb;
    if (n != NULL) {
	atmaint_ProtectNeighbor(n);
	p->gateway = n->address;
	p->gwptr = n;
    } else {
	/*
	 * Note: Any path that requires all traffic to be consumed by us,
	 *       the gateway address must be zero.  Otherwise, the address
	 *       MUST be local to the path's IDB.
	 */
	p->gateway = 0;
	p->gwptr = NULL;
    }
    p->equalpath = NULL;
    p->internal = FALSE;
    GET_TIMESTAMP(p->timer);
    COPY_TIMESTAMP(p->timer, p->started);
    p->state = at_state_good;
    p->type = type;
    p->dstrngstr = dstrngstr;
    p->dstrngend = dstrngend;
    p->holddown = 0;
    p->path_action = path_action;
    p->path_use    = path_use;
    p->flags = 0;
    if (type == AT_PATH_CONNECTED) {
	atalkif_SetIdbPath(idb, p);
    }
    /*
     * If it is a new route, then we have to set up the dummy path block
     * to point to our path block. If this is a pre-existing route, then
     * we can simply hang out path in the list where it belongs.
     * Either way, BestPath must be called after inserting a new path
     * to update the best path and equal pointers.
     *
     * ??? should insert at the head of the list:
     * - if cost is equal && our pathtype is less than the next pathtype
     * - if new cost is less than the previous first node cost.
     */
    atroute_PathListInsert(r, p);

    /*
     * Now insert path into path tree.
     */
    p->tree = p_tree;			/* remember parent path tree */
    (void)atroute_InsertPath(p_tree, dstrngstr, dstrngend, p);
    r->pathcount++;			/* account for the path added */

    if (p->path_action)
	(*p->path_action)(p, at_state_good);
    if (!atroute_BestPathOnRoute(r)) {
	errmsg(AT_ERR_NOBESTPATH, dstrngstr, dstrngend);
    }
    if (at_debug(atalk_events, ATALK_NULL_MASK) && idb) {
	errmsg(AT_ERR_PATHADD, idb_get_namestring(idb->phys_idb),
	       atroute_rp_type(type),
	       atutil_CableRange(dstrngstr, dstrngend),
	       (p->type == AT_PATH_CONNECTED) ? atalkif_MyAddress(p->idb)
                                              : n->address,
	       metric->metric);
    }

    /*
     * The zone request scheduling has been moved here because having it
     * down in the atroute_AddRoute() function means that rdb->rpath
     * hasn't been set yet. We have to check for the connected
     * path state the hard way... atalk_net_connected wants to look at
     * a rdb.
     */
    if (metric->metric > AT_METRIC_STATIC
	&& p->type != AT_PATH_CONNECTED && p->type != AT_PATH_PROXY
	&& p->type != AT_PATH_STATIC && p->type != AT_PATH_AURP)
	atmaint_ScheduleZoneRequest(p->rdb);
    return (p);
}

short
atroute_GetEqualPathCount (atalk_pdb_t *p)
{
    atalk_rdb_t *r;

    if (!p)
        return (0);

    r = p->rdb;
    switch (p->type) {
        case AT_PATH_RTMP:
            return (r->rtmp_equalcount);

	case AT_PATH_IGRP:
	    return (r->eigrp_equalcount);

        default:
	    return (0);
    }
}

void
atroute_ClearEqualPathCount (atalk_pdb_t *p)
{
    atalk_rdb_t *r;

    r = p->rdb;
    switch (p->type) {
        case AT_PATH_RTMP:
            r->rtmp_equalcount = 0;
            break;

        case AT_PATH_IGRP:
	    r->eigrp_equalcount = 0;
            break;

	default:
	    break;
    }
}

void
atroute_UpdateEqualPathCount (atalk_pdb_t *p, boolean increment)
{
    atalk_rdb_t *r;

    r = p->rdb;
    switch (p->type) {
        case AT_PATH_RTMP:
            if (increment)
                r->rtmp_equalcount++;
	    else
	        r->rtmp_equalcount--;
            break;

	case AT_PATH_IGRP:
	    if (increment)
	        r->eigrp_equalcount++;
	    else
	        r->eigrp_equalcount--;
	    break;

	default:
	    break;
    }
}


/*
 *  atroute_DeleteEqualPathList - Remove equal cost paths that
 *  are linked to the primary path
 */
void
atroute_DeleteEqualPathList (atalk_pdb_t *p)
{
    atalk_pdb_t *next, *prev;
    atalk_rdb_t *r;
    short count;

    next = prev = p->equalpath;
    r = p->rdb;
    count = atroute_GetEqualPathCount(p);
    while (count--) {
        if (next->gwptr)
	    atmaint_UnprotectNeighbor(next->gwptr);
	next = next->equalpath;
	prev->equalpath = NULL;
	free(prev);
	prev = next;
	atalk_equalpathFreed++;
    }
    r->next_equalpath = p;
    p->equalpath = p;
    atroute_ClearEqualPathCount(p);
}

void
atroute_DeleteEqualPath (atalk_pdb_t *p, idbtype *phys_idb)
{
    atalk_pdb_t *prev, *next;
    short count;

    prev = p;
    next = p->equalpath;
    count = atroute_GetEqualPathCount(p);
    while (count--) {
	if (phys_idb == next->idb->phys_idb) {
	    atalk_InvalidateCache("atroute_DeleteEqualpath");
	    if (p->rdb->next_equalpath == next)
	        p->rdb->next_equalpath = next->equalpath;
	    prev->equalpath = next->equalpath;
	    if (next->gwptr)
	        atmaint_UnprotectNeighbor(next->gwptr);
	    next->equalpath = NULL;
	    free(next);
	    atroute_UpdateEqualPathCount(p, FALSE);
	    atalk_equalpathFreed++;
	    return;
	}
	prev = next;
	next = next->equalpath;
    }
}


/*
 * atroute_DeletePath
 *
 * Oooo, here's alot of work:
 * - Unlink path from parent rdb's path list
 * - update the rdb's best_path pointer, if this path was the current path;
 *   notify all routing modules of a change in the status of this route.
 * - decrement parent rdb's pathcount
 * - unlink path from idb list; if there are no more paths on idb
 * - notify path-type protocol module that we're deleting this path
 * - update
 */
void
atroute_DeletePath (atalk_pdb_t *p)
{
    atalk_rdb_t	*r;
    dqueue_t *dq;

    if (p == NULL)
	return;
    if (!(p->dqLink.flink && p->dqLink.blink)) /* both must be != NIL */
	return;
    r = p->rdb;				/* pick up parent route */
    if (r == NULL)
	return;
    /*
     * Most the "best path" pointer up the list of paths. If there is
     * no other path on the list, then this route will disappear.
     */
    dq = p->dqLink.blink;
    if (dq == &r->rpath.dqLink)
	r->best_path = NULL;	/* no other path on the list */
    else
	r->best_path = path_Cast(dq->parent);

    /*
     * If this is a connected path, then NIL out the atalk_path pointer
     * in the IDB. If this isn't done, things will blow up later.
     */
    if (p->type == AT_PATH_CONNECTED) {
	atalkif_SetIdbPath(p->idb, NULL);
    }
    /*
     * Since we protect neighbor entries every time a path is added from
     * a neighbor, we should unprotect here every time we remove a
     * path.
     */
    if (p->gwptr)
	atmaint_UnprotectNeighbor(p->gwptr);
    /*
     * Protect against the instance where someone is trying to delete
     * the fixed path contained in the RDB.
     */
    if (at_debug(atalk_events, ATALK_NULL_MASK) && p->idb) {
	errmsg(AT_ERR_PATHDEL, idb_get_namestring(p->idb->phys_idb),
	       atroute_rp_type(p->type),
	       atalk_CableRange(r), p->gateway);
    }
    if (atroute_GetEqualPathCount(p))
	atroute_DeleteEqualPathList(p);
    /*
     * The following two must be done _before_ the call to the path_action
     * routine, which will call BestPath. If they are not performed
     * before path_action, then the route owning the path may
     * never be deleted.
     */
    lw_remove(&p->dqLink);			/* unlink from route's path list */
    r->pathcount--;
    if (p->path_action)
	(*p->path_action)(p, at_state_delete);
    RBTreeDelete(p->tree, &p->link);
}

/*
 * Delete all paths to route, r.  If route is directly connected, 
 * disassociate directly connected path with idb.
 */
void
atroute_ClearAllPaths (atalk_rdb_t *r)
{
    atalk_pdb_t	*p, *pp;
    ushort	pc;

    if (r == NULL)
	return;

    /* If route has directly connected path, disassociate the path with idb. 
          If it's a proxy route than the idb will be NULL, so do nothing */
    if ((atalk_net_connected(r)) && (r->rpath.idb != NULL))
	atalkif_SetIdbPath(r->rpath.idb, NULL);

    /*
     * Just keep deleting paths off the bottom until we have no more.
     */
    pc = r->pathcount;
    p = path_Cast(r->rpath.dqLink.blink->parent);
    while (pc > 0) {
	pp = path_Cast(p->dqLink.blink->parent); /* walk back */
	atroute_DeletePath(p);
	pc--;
	p = pp;
    }
}

/*
 * route_GC - Clean up the routing tree(s) free list as requested.
 *
 * This function doesn't put pdb's or rdb's onto the rbTree internal
 * list to be free'd -- other functions do that. This function
 * merely reclaims memory used by rdb's and pdb's. That is _it_.
 *
 */
static boolean
route_GC (void)
{
    atalk_rdb_t *r;
    ulong	r_freed = 0;
    rbTree	*tree;
    tree_list_t	*tl = route_trees;

    gcRedistQueue();

    /*
     * Walk all routing trees...
     */
    for (tl = route_trees; tl != NULL; tl = tl->next) {
	tree = tl->r_tree;
	ATASSERT(tree != NULL);
	tree->gcRuns++;
	if (!RBTreeValid(tree, TRUE, "route_GC")) {
	    if (!TIMER_RUNNING_AND_SLEEPING(tree->errTimer)) {
		errmsg(SYS_ERR_TABLEERR, "Appletalk", tree->tableName,
		       tree->lastErr);
		TIMER_START(tree->errTimer, 5 * ONEMIN);
	    }
	}
	while ((r = route_Cast(RBTreeGetFreeNode(tree))) != NULL) {
#if 0
	    atalk_FreeZoneEntries(r);	/* free any zone name entries */
#endif
	    free(r);			/* free the route */
	    r_freed++;
	    if (!route_NeedGCRun(tree))
		break;
	}
	tl->rdbFreed += r_freed;
    }

    if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	buginf("\nAT: Route DB GC run complete (%d RDB%s freed)",
	       ArgAndPlural(r_freed,"","s"));

    return (TRUE);
}

/*
 * atroute_CleanUpRoute
 *
 * This function is used to clean up a rdb and all data structures
 * hung off a rdb. This includes zipnet's and zones.
 *
 */
boolean
atroute_CleanUpRoute (void)
{
    atalk_rdb_t	*rdb;
    rbTree	*tree;
    ushort	count = 0;

    if (atalk_InRouteZoneWeb > 0)
	return (FALSE);			/* we didn't run; re-schedule */

    if (at_debug(atalkerr_debug, ATALK_NULL_MASK)) {
	buginf("\nAT: atroute_CleanUpRoute, cleaning up... ");
    }
    /*
     * OK, we have control of the route-zone-zipnet web. _Don't_
     * dismiss inside the following loop.
     */
    while (atroute_GCPending != NULL) {
	rdb = atroute_GCPending;
	atroute_GCPending = rdb->free;	/* unlink from free thread */
	tree = rdb->tree;
	rdb->free = NULL;
	atalk_FreeZoneEntries(rdb);
	RBTreeAddToFreeList(tree, (treeLink *)rdb);
	++count;
    }
    if (at_debug(atalkerr_debug, ATALK_NULL_MASK)) {
	buginf("\nAT: atroute_CleanUpRoute, cleaned up %u rdb's", count);
    }

    return (TRUE);			/* we're done; don't re-schedule */
}  

/*
 * atroute_DeleteRoute
 * remove an appletalk route from the table
 */
void
atroute_DeleteRoute (atalk_rdb_t* r)
{
    rbTree	*tree = NULL;
    at_path_t	type;
    ushort	dstrngstr, dstrngend;

    if (r == NULL)
	return;
    type = r->rpath.type;
    dstrngstr = r->dstrngstr;
    dstrngend = r->dstrngend;
    atmaint_CancelZoneRequest(r);
    if (RouteProtected(r))		/* if it's still locked, bail */
	return;
    if (r->pathcount > 0)		/* any active paths left? bail */
	return;
    /*
     * Proxy nets should not be deleted here; they should call
     * atroute_DeleteProxyNet().
     */
    ATASSERT(r->pathcount == 0);
    if (r->link.busy != 0) {
	buginf("\nAT: atroute_DeleteRoute: busy: %d on route %d-%d",
	       r->link.busy, r->dstrngstr, r->dstrngend);
    }

    if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK)
	|| at_debug(atalk_events, ATALK_NULL_MASK)) {
	errmsg(AT_ERR_DELROUTE,atalk_CableRange(r));
    }

    tree = r->tree;
    /*
     * Oh, Great Almighty Murphy, forgive me the sins which I am about
     * to commit.
     *
     * We have a conflicting set of requirements here.
     * 1. We have to delete the rdb in question from the route RBTree,
     *    so that any attempts to lookup the rdb will fail. This is
     *    necessary because EIGRP will delete a route, then re-add it
     *    without dismissing. 
     * 2. But we need to use the route deletion event to consistently
     *    delete any zipnets and zone entries. EIGRP's actions be damned.
     * 3. And doing all the rdb deletion, zipnet deletion, zone deletion
     *    immediately when atroute_DeleteRoute is called will result
     *    in anyone sitting at a "More" prompt in a "show apple route",
     *    "show apple zone" or other such show command which holds pointers
     *    into the route/zone/zipnet web to probably crash.
     *
     * So, we delete the rdb from the route tree, then scavenge the node
     * back from the RBTree's freelist so it won't be immediately
     * re-used when EIGRP tries to add a route using the same RDB
     * immediately after deleting it. We then schedule the RDB for
     * zone & zipnet cleanup, after which it will _then_ be added
     * back onto the RBtree's freelist.
     *
     * Did you hurl your cookies yet? If not, then finish following the
     * thread for the deletion of a rdb.
     */
    if (RBTreeDelete(tree, &r->link) != &r->link) {
	if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
	    buginf("AT: atroute_DeleteRoute: RBTreeDelete didn't "
		   "delete route node");
    }

    if (atalk_DomainRouter)
	atdomain_route_done(dstrngstr,dstrngend);

    RBTreeScavengeFreeNode(tree, (treeLink *)r); /* gimme that node back */
    r->free = atroute_GCPending;
    atroute_GCPending = r;
    if (atalk_running) {
	reg_invoke_atalk_validate_route(dstrngstr, dstrngend);
	if(r->pathcount == 0 && type != AT_PATH_STATIC)
	    at_static_validate_route(dstrngstr,dstrngend);
    }

    atmaint_ScheduleGCRun(atroute_cleanup_gccontext);
}

/*
 * atroute_GetValidRoute - Search routing tree and validate range.
 */
atalk_rdb_t*
atroute_GetValidRoute (rbTree *tree, ushort from, ushort to,boolean* valid, boolean poison)
{
    atalk_rdb_t* r;

    *valid = FALSE;
    if ((from == 0) || ((to != 0) && (to < from)))
	return (NULL);
    if (   (from >= ATALK_STARTUP_START)
	|| ((to != 0) && (to >= ATALK_STARTUP_START)))
	return (NULL);
    r = route_Cast(RBTreeIntSearch(tree,IntervalKey(from,to)));
    if (r == NULL) {
	*valid = TRUE;
	return (NULL);
    }
    if ((from == to) && (r->dstrngend == 0) && (r->dstrngstr == from)) {
        if (poison == FALSE)
	    r->dstrngend = to;	/* prefer extended info when heard */
	*valid = TRUE;
    } else if ((r->dstrngend == r->dstrngstr) && (to == 0) && 
	       (from == r->dstrngstr)) {
	*valid = TRUE;		/* allow non-extended compatible entries */
    } else if ((r->dstrngstr == from) && (r->dstrngend == to)) {
	*valid = TRUE;
    }
    return (r);
}

/*
 *
 * A path is considered unique by 4 factors:
 * - net number/net range
 * - idb
 * - neighbor/next hop
 * - type of routing protcol responsible for creation
 *
 * We need the neighbor _and_ the idb since in the situation of parallel
 * links to the neighboring router, the neighbor's address might not be
 * unique in the case of half-router links and unnumbered (ie, no specific
 * address) interfaces.
 */
atalk_pdb_t*
atroute_GetMatchingPath (rbTree *tree, ushort from, ushort to,
			 atalkidbtype *idb, neighborentry *n, boolean *valid)
{
    atalk_pdb_t	*p;

    *valid = FALSE;
    if ((from == 0) || ((to != 0) && (to < from)))
	return (NULL);
    if (   (from >= ATALK_STARTUP_START)
	|| ((to != 0) && (to >= ATALK_STARTUP_START)))
	return (NULL);

    *valid = TRUE;
    if (tree == NULL)
	return (NULL);
    p = path_Cast(RBTreeIntSearch(tree,IntervalKey(from,to)));
    while (p) {
	if (!(p->rdb->dstrngstr == from && p->rdb->dstrngend == to))
	    break;
	if (p->idb == idb && p->gwptr == n)
	    return (p);
	p = path_Cast(RBTreeNextNode(&p->link));
    }
    return (NULL);
}

/*
 * atroute_GetValidPath
 *
 * Get a (as in any) valid path for the network range specified.
 * note: 'valid' is set to TRUE if the net number range specified is valid.
 *       'tree' is a path tree, not a route tree.
 */
atalk_pdb_t*
atroute_GetValidPath (rbTree *tree, ushort from, ushort to, boolean* valid, boolean poison)
{
    atalk_rdb_t* r;
    atalk_pdb_t* p;

    *valid = FALSE;
    if ((from == 0) || ((to != 0) && (to < from)))
	return (NULL);
    if (   (from >= ATALK_STARTUP_START)
	|| ((to != 0) && (to >= ATALK_STARTUP_START)))
	return (NULL);

    if (tree == NULL)
	return (NULL);
    p = path_Cast(RBTreeIntSearch(tree,IntervalKey(from,to)));

    /*
     * If we found no entry, and we made it past the preliminary checks,
     * then the specified range is valid and there is no route.
     */
    if (p == NULL) {
	*valid = TRUE;
	return (NULL);
    }

    r = p->rdb;
    if ((from == to) && (r->dstrngend == 0) && (r->dstrngstr == from)) {
        if (poison == FALSE)
	    r->dstrngend = to;		/* prefer extended info when heard */
	*valid = TRUE;
	return (p);
    }
    if ((r->dstrngend == r->dstrngstr)&&(to == 0) && (from == r->dstrngstr)) {
	*valid = TRUE;		/* allow non-extended compatible entries */
	return (p);
    }
    if ((r->dstrngstr == from) && (r->dstrngend == to)) {
	*valid = TRUE;
	return (p);
    }
    return (NULL);
}

/*
 * atroute_RouteCount
 * Return the number of routes that are active.
 */
ulong
atroute_RouteCount (void)
{
    rbTree	*tree;

    tree = atroute_GetTree();
    return (tree->activeNodes);
}
#if DEAD

int
atroute_RoutePathCount (atalk_rdb_t *r)
{
    return (r->pathcount);
}
#endif

/*
 * atroute_IsRouteValid
 * Check if a route entry of the specified values would be valid.
 */
boolean
atroute_IsRouteValid (rbTree *tree, ushort low, ushort high)
{
    boolean valid = FALSE;
    (void)atroute_GetValidRoute(tree, low,high,&valid,FALSE);
    return (valid);
}

/*
 * atroute_ager - Age all routes as appropriate. Note that paths are not
 *                touched; those are the property of their respective
 *                processes.
 */
void
atroute_Ager (void)
{
    tree_list_t	*tl;
    rbTree	*tree;			/* current routing tree being aged */

    for (tl = route_trees; tl != NULL; tl = tl->next) {
	tree = tl->r_tree;
	ATASSERT(tree != NULL);
	if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Route ager starting on %s (%d active node%s)",
		   tree->tableName, ArgAndPlural(tree->activeNodes,"","s"));

	/* Age the route. */
	RBTreeForEachNode(route_periodic, NULL, tree, FALSE);

	if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Route ager finished on %s (%d active node%s)",
		   tree->tableName, ArgAndPlural(tree->activeNodes,"","s"));

	/* Schedule garbage collection, if needed. */
	if (route_NeedGCRun(tree))
	    atmaint_ScheduleGCRun(atroute_GC_gccontext);
    }
}

/* 
 * atroute_WriteNV
 * appletalk routing nvram support
 */
void 
atroute_WriteNV (char *nv_command)
{
    atalk_rdb_t	*r = NULL;
    atalk_pdb_t	*p = NULL;
    zipnetentry	*z;
    rbTree	*p_tree;
    
    /*
     * For current, we only have to worry about writing out proxy-nbp routes.
     */
    p_tree = atroute_GetPTree(AT_PATH_PROXY);
    for (p = KeepFirstPath(p_tree); p != NULL; p = KeepNextPath(p_tree,p)) {
	if (!atalk_path_internal(p)) {
	    r = p->rdb;
	    z = atalk_firstZone(r);
	    nv_write(TRUE, "%s %d ", nv_command, r->dstrngstr);
	    atalk_nv_printzone(z->zone->pname);
	}
    }
}

/*
 * atroute_FindRoute - Lookup a route for a net
 *
 * Returns the route WITH protection.
 */
atalk_rdb_t*
atroute_FindRoute (rbTree *r_tree, ushort net, boolean force)
{
    atalk_rdb_t* r;

    r = route_Cast(RBTreeIntSearch(r_tree,IntervalKey(net,0)));
    if ((r != NULL) && ((r->pathcount > 0) || force))
	ProtectRoute(r);
    return (r);
}
/*
 * atroute_LookupPath - Lookup a route for a net
 *
 * Returns the route WITHOUT protection.
 */
atalk_pdb_t*
atroute_LookupPath (rbTree *p_tree, ushort net)
{
    atalk_pdb_t* p;

    p = path_Cast(RBTreeIntSearch(p_tree,IntervalKey(net,0)));
    return (p);
}

/*
 * atroute_LookupRoute - Lookup a route for a net
 *
 * Returns the route WITHOUT protection.
 */
atalk_rdb_t *
atroute_LookupRoute (rbTree *r_tree, ushort net, boolean force)
{
    atalk_rdb_t* r;

    r = route_Cast(RBTreeIntSearch(r_tree,IntervalKey(net,0)));
    if ((r != NULL) && ((r->pathcount > 0) || (force)))
	return (r);
    return NULL;
}

/*
 * atroute_treeprint
 * Print the routing table RB trees and their path trees.
 *
 */
void
atroute_printtrees (void)
{
    /*
     * There is a multiple-access problem in the RBTreePrint
     * code as it stands now. In the future, we can fix this, but
     * for now, dumping all the route/path trees serves a rather limited
     * purpose. Disabled to prevent router crashes from "show apple priv all"
     */
#ifdef NOT_USED
    tree_list_t	*tl;
    rbTree	*r_tree;
    rbTree	*p_tree;
    at_path_t	path_type;

    for (tl = route_trees; tl != NULL; tl = tl->next) {
	r_tree = tl->r_tree;
	printf("\n% routing tree internals", r_tree->tableName);
	RBTreePrint(r_tree->root, 0, r_tree);
	for (path_type = AT_PATH_DUMMY; path_type < AT_PATH_MAX; path_type++) {
	    p_tree = tl->p_tree[path_type];
	    if (p_tree) {
		printf("\n%s path tree from %s routing tree",
		       p_tree->tableName, r_tree->tableName);
		RBTreePrint(p_tree->root, 0, p_tree);
	    }
	}
    }
#endif
}

/*
 * atroute_GetTree
 *
 * Get a routing tree. parameters TBD. This will return a routing tree.
 */
rbTree *
atroute_GetTree (void)
{
    return (route_trees->r_tree);
}
/*
 * atroute_GetPTree -- return a path tree. input: path type.
 */
rbTree *
atroute_GetPTree (ushort ptype)
{
    if (ptype > AT_PATH_MAX)
	return (NULL);
    return (route_trees->p_tree[ptype]);
}

char
atroute_TypeInitial (atalk_rdb_t *r)
{
    static char	const type_vector[AT_PATH_MAX] =
	{'@', 'd','C','P','S','E','R','D','A'};

    if (r) {
	if (r->rpath.type > AT_PATH_MIN && r->rpath.type < AT_PATH_MAX)
	    return (type_vector[r->rpath.type]);
    }
    return '?';
}

char
atroute_PathState (atalk_pdb_t *p)
{
    static const char state_vector[at_state_poisoned+1] =
	{ '?', 'G', 'S', 'B', 'B', 'P' };

    if (p) {
	if (p->state > at_state_invalid && p->state <= at_state_poisoned)
	    return (state_vector[p->state]);
    }
    return ('B');
}

atalk_rdb_t*
atroute_GetIdbRoute (atalkidbtype *idb)
{
    atalk_pdb_t *p;
    
    if (idb) {
	p = atalkif_GetIdbPath(idb);
	if (p)
	    return (p->rdb);
    } 
    return (NULL);
}

/*
 * atroute_pathGC
 *
 * Walk the specified path tree and free up unused paths.
 */
void
atroute_pathGC (at_path_t which_p_tree, char const *path_proc)
{
    atalk_pdb_t *p;
    rbTree	*p_tree;
    ulong	 p_freed = 0;
    ulong	 p_waiting = 0;
    tree_list_t *tl = route_trees;

    if (!(   (which_p_tree > AT_PATH_DUMMY && which_p_tree < AT_PATH_MAX)
	  && path_proc))
	return;

    if ((p_tree = atroute_GetPTree(which_p_tree)) != NULL) {
	if (!RBTreeValid(p_tree, TRUE, path_proc)) {
	    if (!TIMER_RUNNING_AND_SLEEPING(p_tree->errTimer)) {
		errmsg(SYS_ERR_TABLEERR, "Appletalk", p_tree->tableName,
		       p_tree->lastErr);
		TIMER_START(p_tree->errTimer, 5 * ONEMIN);
	    }
	}
	while ((p = path_Cast(RBTreeGetFreeNode(p_tree))) != NULL) {
	    free(p);			/* free the route */
	    p_freed++;
	    if (!route_NeedGCRun(p_tree))
		break;
	}
	p_waiting = p_tree->freeNodes;
	tl->pdbFreed += p_freed;
    }
    if (at_debug(appletalk_debug, ATALK_NULL_MASK))
	buginf("\nAT: %s GC complete (%d PDB%s freed, %d PDB%s waiting)",
	       path_proc, ArgAndPlural(p_freed,"","s"),
	       ArgAndPlural(p_waiting,"","s"));
}

/*
 * atroute_RoutePoisonSent
 *
 * Called to flag that a routing protocol has poisoned a route.
 */
void
atroute_RoutePoisonSent (atalk_rdb_t *r)
{
    boolean emit_debugging_notice;

    if (r == NULL)
	return;
    switch (r->action) {
      case at_notify0_route:
	r->action = at_notify1_route;
	emit_debugging_notice = TRUE;
	break;
      case at_notify1_route:
	r->action = at_delete_route;
	/* Fall through */
      case at_delete_route:
	emit_debugging_notice = TRUE;
	break;
      default:
	emit_debugging_notice = FALSE;
    }
    if (emit_debugging_notice && at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	printf("\nAT: Time to notify neighbor for route %d-%d",
	       r->dstrngstr, r->dstrngend);
}

boolean
atroute_DefaultPathAction (atalk_pdb_t *p, at_state_t state)
{
    at_state_t ostate;

    if (p == NULL)
	return (FALSE);

    ostate = p->state;
    /*
     * Unredistributing an already unredistributed route has no ill effect
     * We want to cover ALL cases of a redistributed path needing
     * unredistribution.
     */
    if (state >= at_state_bad) {
	atroute_path_unredistribute(p);
    }
    /* Similarly, bring it back to redistribution if previously gone */
    if ((state <= at_state_suspect) && (p->state >= at_state_bad)) {
	atroute_path_redistribute(p);
    }
    atroute_BestPath(p);

    /* If it came from Notify Neighbor or Deleted state,
     * then assume the new route and therefore delete 
     *  the zone list
     */  

    if(ostate > at_state_bad && !atalk_net_connected(p->rdb))
	atmaint_ResetRequestInfo(p->rdb);
    return (TRUE);
}

void
atroute_clear_redisted_paths (void)
{
    atrtmp_redist_all(TRUE);
    /*
     * When other routing protocols are added, perform their unredist here.
     */
}
