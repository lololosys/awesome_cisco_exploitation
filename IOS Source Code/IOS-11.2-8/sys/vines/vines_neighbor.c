/* $Id: vines_neighbor.c,v 3.7.18.4 1996/08/28 13:19:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_neighbor.c,v $
 *------------------------------------------------------------------
 * vines_neighbor.c - support for keeping Vines neighbor tables
 * 
 * September 1992, David R. Hampton
 * from a module dated February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_neighbor.c,v $
 * Revision 3.7.18.4  1996/08/28  13:19:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.18.3  1996/08/07  09:03:41  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/05/11  16:17:31  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.7.18.1  1996/03/18  22:31:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.16.3  1996/03/13  02:04:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.16.2  1996/03/07  11:04:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.16.1  1996/02/20  21:46:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/08  20:13:53  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.6  1995/12/08  04:56:17  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.5  1995/12/07  22:33:32  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/01  21:39:07  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/17  17:58:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:44:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/19  23:41:02  slin
 * CSCdi41922:  Vines Router process suspends too frequently
 *
 * Revision 2.5  1995/09/15  20:33:47  slin
 * CSCdi40388:  show vines int crashes router
 * - Convert yet another routine to gather info into local buffer before
 *   printing
 *
 * Revision 2.4  1995/07/27  07:31:04  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.3  1995/07/24 22:32:58  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.2  1995/07/07 20:40:10  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.1  1995/06/07 23:14:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "config.h"
#include "parser.h"
#include "../if/network.h"
#include "vines_private.h"
#include "vinesfast.h"

uint vines_paths, vines_neighbor_version;
uint vines_v0_count, vines_v0_servers;
uint vines_v1_count, vines_v1_servers;
boolean vines_reinit_debug;
const char vines_cannot_find_neighbor_msg[] = 
    "\nV%s: Cannot find neighbor entry for this router.";
const char vines_neighbor_malloc_str[] = "Vines Neighbor";

rbTree *vines_neighbor_tree;


/**********************************************************************
 *
 *			 VINES Neighbor Table
 *
 * This code supports the maintenance and display of the VINES neighbor
 * table.  A neighbor is any station, client or server, that shares a
 * physical network segment with the router.  This code maintains the
 * table of all neighbors, which interface or interfaces are used to
 * reach them, the addresses, and varius other information.
 *
 * This first section works exclusively with paths to a neighbor.  It
 * does not work with the nnrighbor as a whole.  Routines that work with
 * entire neighbor structures can be found in the next section.
 *
 **********************************************************************/


/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vines_neighbor_init
 *
 * Perform any neighbor table specific or path specific initialization.
 */
void vines_neighbor_init (void)
{
    vines_paths = 0;
    vines_neighbor_version = 0;
    vines_neighbor_tree = RBTreeCreate("Vines", "VN", "Neighbor Tree",
				       RBTreePrintKeyLongLong, &vinesrtp_debug);
    RBTreeNon32Bit(vines_neighbor_tree, RBTreeKeyMaxLongLong,
		   RBTreeKeyCompareLongLong);
}


/**********************************************************************
 *
 *		       VALIDITY AND CONSISTENCY
 *
 **********************************************************************/

/*
 * vines_set_path_flags
 *
 * Set the SRTP flags appropriate to this interface and version number.
 */
static void vines_set_path_flags (vinespathtype *path, int version)
{
    path->srtp_flags = VSRTP_ENTRY_NONE;
    
    if (version == VRTP_VERSION) {
	path->srtp_flags |= VSRTP_ENTRY_NO_SEQ;
    }

    if (!path->idb || !path->idb->phys_idb->hwptr) {
	path->srtp_flags |= VSRTP_ENTRY_INVALID;
	return;
    }

    if (vines_is_lan(path->idb)) {
	path->srtp_flags |= VSRTP_ENTRY_LAN;
    } else {
	path->srtp_flags |= VSRTP_ENTRY_WAN;
    }
}

/*
 * vines_set_neighbor_flags
 *
 * Run the list of all the best paths, choosing the best flags to put
 * into the neighbor flags entry.  This is done to indicate what, out of
 * all the choices, the router will actually use to reach the neighbor.
 * A LAN flags takes precedence over a WAN flag.
 */
static void vines_set_neighbor_flags (vinesneighbortype *neighbor)
{
    vinespathtype *path;
    ulong old_flags;

    old_flags = neighbor->srtp_flags;
    neighbor->srtp_flags = VSRTP_ENTRY_NONE;
    path = neighbor->best_path;
    if (path == NULL)
	return;
    do {
	if (path->srtp_flags & VSRTP_ENTRY_NO_SEQ) {
	    neighbor->srtp_flags |= VSRTP_ENTRY_NO_SEQ;
	}
	if (path->srtp_flags & VSRTP_ENTRY_LAN) {
	    neighbor->srtp_flags &= ~VSRTP_ENTRY_WAN;
	    neighbor->srtp_flags |= VSRTP_ENTRY_LAN;
	}
	if (path->srtp_flags & VSRTP_ENTRY_WAN) {
	    if ((neighbor->srtp_flags & VSRTP_ENTRY_LAN) == 0)
		neighbor->srtp_flags |= VSRTP_ENTRY_WAN;
	}
	path = path->next_best;
    } while (path != neighbor->best_path);
    if (neighbor->srtp_flags != old_flags) {
	if (vinesrtp_debug) {
	    buginf("\nVRTP: Neighbor flags changed from %x to %x",
		   old_flags, neighbor->srtp_flags);
	}
	vroute_update_flags(neighbor); 
    }
}

/*
 * vines_compute_neighbor_metric
 *
 * Scan the list of all currently valid paths looking for the one with
 * the lowest metric.  Record this metric as the overall metric value for
 * the neighbor.
 */
static void vines_compute_neighbor_metric (
    vinesneighbortype *neighbor)
{
    vinespathtype *path;
    ulong oldmetric, metric;
    sys_timestamp dummy;

    oldmetric = neighbor->metric;
    metric = VINES_INFINITY;
    FOR_ALL_PATHS(neighbor, path) {
	if (path->flags & V_N_PLACEHOLDER)
	    continue;
	metric = min(metric, path->metric);
    }
    neighbor->metric = metric;
    if (metric != oldmetric) {
	vines_fixup_servers_by_neighbor(neighbor);
	if (metric == VINES_INFINITY) {
	    TIMER_STOP(dummy);
	    vines_poison_routes_by_neighbor(neighbor, dummy);
	} 
    }
}

/*
 * vines_build_path_list
 *
 * Scan the list of all currently valid paths looking for the one(s)
 * with the same metric as the neighbor entry.  (The previous routine
 * should have already been called to insure that this is the lowest
 * metric.)  Build a circularly linked list of these paths for use by
 * get_next_path().
 *
 * Note: It is possible for the best path to be a static path through an
 * interface that is shutdown or has had vines disabled.  This is O.K. as
 * any packets using this path will be discarded as they code attempts to
 * transmit them.  This is one of the dangers of static routes.
 */
static void vines_build_path_list (vinesneighbortype *neighbor)
{
    vinespathtype *path, *last;

    neighbor->best_path = neighbor->primary_path = last = NULL;
    FOR_ALL_PATHS(neighbor, path) {
	if (path->flags & V_N_PLACEHOLDER)
	    continue;
	if (path->metric == neighbor->metric) {
	    if (neighbor->best_path == NULL) {
		neighbor->primary_path = path;
		neighbor->best_path = path;
	    } else {
		last->next_best = path;
	    }
	    last = path;
	} else {
	    path->next_best = NULL;
	}
    }
    if (last)
	last->next_best = neighbor->best_path;
}

/*
 * vines_fixup_neighbor
 *
 * Insure that the neighbor data structure is consistent for the current
 * path metrics.
 */
void vines_fixup_neighbor (vinesneighbortype *neighbor)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    vines_compute_neighbor_metric(neighbor);
    vines_build_path_list(neighbor);
    vines_set_neighbor_flags(neighbor);
    reset_interrupt_level(level);
    vines_flush_cache_by_neighbor(neighbor);
}

/*
 * vines_check_path_list
 *
 * Follow a linked list of paths to make sure that the structures and
 * links are still valid.  Complain loudly if they are not, and attempt
 * to minimize the damage by truncating the list before the error.
 */
boolean vines_check_path_list (vinesneighbortype *neighbor, vinespathtype **origin)
{
    vinespathtype *p, *prev;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    for (prev = (vinespathtype *)origin, p = *origin; p; prev = p, p = p->next) {
	if (p->magic != VINES_MAGIC_PATH) {
	    errmsg(&msgsym(INVALIDPATH, VINES), "magic number", 
		   neighbor->net, neighbor->host);
	    prev->next = NULL;
	    reset_interrupt_level(level);
	    return(TRUE);
	}
	if (!validlink(p->next)) {
	    errmsg(&msgsym(INVALIDPATH, VINES), "link",
		   neighbor->net, neighbor->host);
	    p->next = NULL;
	    reset_interrupt_level(level);
	    return(TRUE);
	}
    }
    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * vines_verify_neighbor_structure
 *
 * This routine verifies the contents of the data structure, looking for
 * corrupted data or corrupted pointers.  It corrects problems with the
 * data structure wherever possible.  This routine returns FALSE if the
 * neighbor structure is unusable.  It returns TRUE is the neighbor data
 * structure was valid, or if it has been repaired so that it can be
 * used.
 */
boolean vines_verify_neighbor_structure (
    vinesneighbortype *neighbor)
{
    leveltype level;

    /*
     * Check neighbor and first path.
     */
    if (!validmem(neighbor)) {
	buginf("\nvines_verify_neighbor_structure passed invalid pointer");
	return(FALSE);
    }
    if (!neighbor->available_paths)
	return(TRUE);
    if (!validlink(neighbor->available_paths)) {
	neighbor->available_paths = neighbor->best_path = NULL;
	errmsg(&msgsym(CORRUPTENTRY, VINES));
	return(TRUE);
    }
    
    /*
     * Walk both the in use path list and the free path list, checking
     * each one.  Remember that if the in use routes get zapped, the
     * best_path pointer must also be reset.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (vines_check_path_list(neighbor, &neighbor->available_paths))
	vines_fixup_neighbor(neighbor);
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

ulong vneigh_get_path_count (void)
{
    return(vines_paths);
}

ulong vneigh_get_neighbor_count (void)
{
    return(vines_neighbor_tree->activeNodes);
}


/**********************************************************************
 *
 *			INSERTION AND DELETION
 *
 **********************************************************************/

/*
 * vneigh_inc_type_counts
 *
 * Given a vines neighbor, determine what type it is and increment
 * the appropriate counter.
 */
static void vneigh_inc_type_counts (
    vinesneighbortype *neighbor,
    vinesidbtype *idb)
{
    if (!neighbor || !idb)
	return;

    switch (neighbor->version) {
      case 0:
	vines_v0_count++;
	switch (neighbor->node) {
	  case VRTP_NODE_CLIENT:
	    idb->v0_clients++;
	    break;
	  case VRTP_NODE_ROUTER:
	    if (server_is_banyan(neighbor->net)) {
		vines_v0_servers++;
		idb->v0_servers++;
	    } else {
		idb->v0_routers++;
	    }
	    break;
	}
	break;
      case 1:
	vines_v1_count++;
	switch (neighbor->node) {
	  case VSRTP_NODE_CLIENT:
	    idb->v1_clients++;
	    break;
	  case VSRTP_NODE_ROUTER:
	    if (server_is_banyan(neighbor->net)) {
		vines_v1_servers++;
		idb->v1_servers++;
	    } else {
		idb->v1_routers++;
	    }
	    break;
	}
	break;
    }
}

/*
 * vneigh_dec_type_counts
 *
 * Given a vines neighbor, determine what type it is and decrement
 * the appropriate counter.
 */
static void vines_dec_type_counts (
    vinesneighbortype *neighbor,
    vinesidbtype *idb)
{
    if (!neighbor || !idb)
	return;

    switch (neighbor->version) {
      case 0:
	vines_v0_count--;
	switch (neighbor->node) {
	  case VRTP_NODE_CLIENT:
	    idb->v0_clients--;
	    break;
	  case VRTP_NODE_ROUTER:
	    if (server_is_banyan(neighbor->net)) {
		vines_v0_servers--;
		idb->v0_servers--;
	    } else {
		idb->v0_routers--;
	    }
	    break;
	}
	break;
      case 1:
	vines_v1_count--;
	switch (neighbor->node) {
	  case VSRTP_NODE_CLIENT:
	    idb->v1_clients--;
	    break;
	  case VSRTP_NODE_ROUTER:
	    if (server_is_banyan(neighbor->net)) {
		vines_v1_servers--;
		idb->v1_servers--;
	    } else {
		idb->v1_routers--;
	    }
	    break;
	}
	break;
    }
}

/*
 * vines_insert_new_path
 *
 * Given a neighbor node and a path node, insert the new node into the
 * sorted list of nodes currently in use.  The sorting is done based upon
 * the dlc address block.  Sorting in necessary to guarantee that any
 * parallel routers maintain the exact same view of the universe.  This
 * is important in preventing streettalk broadcast loops.
 */
void vines_insert_new_path (
    vinesneighbortype *neighbor,
    vinespathtype *newpath)
{
    vinespathtype *p, *pred;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    pred = (vinespathtype *)&neighbor->available_paths;
    p = neighbor->available_paths;
    for ( ; p; pred = p, p = p->next) {
	if (memcmp(&newpath->dlc_addr, &p->dlc_addr, sizeof(hwaddrtype)) < 0)
	    break;
    }
    newpath->next = p;
    pred->next = newpath;
    reset_interrupt_level(level);
}

/*
 * vines_make_new_path
 *
 * Find or create a path entry for this server.  This routine will first
 * look in the free list of paths associated with this server.  If there
 * are no path structures there, it will malloc enough memory to hold
 * all the current path structures, plus one more.  It then copies all
 * of the old path structures into the new memory block, and frees the
 * old memory block.  A pointer to the remaining space in the new memory
 * block is returned as a place to put the new path.
 */
static inline void vines_init_dqueue(vinespathtype *path, dqueue_t *timer)
{
    timer->flink = timer->blink = timer;
    timer->parent = path;
}

vinespathtype *vines_make_new_path (
    vinesneighbortype *neighbor)
{
    vinespathtype *new;

    /*
     * Allocate a memory block for the new path entry, and fill in some
     * of the values.
     */
    if (!(new = malloc_named(sizeof(vinespathtype), "Vines Path")))
	return(NULL);
    mgd_timer_init_leaf(&new->reassembly_timer, &vsrtp_reassembly_timer,
			VT_REASSEMBLY, new, FALSE);
    mgd_timer_init_leaf(&new->retry_timer, &vsrtp_retry_timer, VT_RETRY, new,
			FALSE);
    mgd_timer_init_leaf(&new->rc_timer, &vrtp_rc_timer, VT_RC, new, FALSE);
    vines_init_dqueue(new, &new->by_idb);
    return(new);
}

/*
 * vines_set_path_timestamp
 *
 * Set the timestamp on the path given, setting the 'snapshot' bit in the
 * path as well..
 */
static void vines_set_path_timestamp (vinespathtype *p)
{
    GET_TIMESTAMP(p->timer);
    if (p->flags & (V_N_STATIC | V_N_PLACEHOLDER))
        return;
    p->snapshot_no_age = 
        reg_invoke_snapshot_aging_denied(p->idb->phys_idb);
}

/*
 * vines_add_path
 *
 * Add a new path to an existing neighbor entry.  This routine first
 * checks to see if the path already exists.  If not, it calls another
 * routine to make space for the new entry, and then copies in all the
 * data.  A second routine takes care of inserting the path into the
 * sorted linked list.
 */
vinespathtype *vines_add_path (
    vinesneighbortype *neighbor,
    ulong flags,
    vinesidbtype *idb,
    int enctype,
    hwaddrtype *dlc_addr,
    int metric,
    int version,
    uchar type)
{
    vinespathtype *existing, *new;

    if (!neighbor || !vines_verify_neighbor_structure(neighbor))
	return(NULL);
    existing = vines_get_path_by_xxx(neighbor, flags, idb, enctype, dlc_addr);
    if (existing) {
	if (metric) {
	    /*
	     * Override any old metric.
	     */
	    existing->metric = metric;
	    vines_set_path_timestamp(existing);
	    vines_fixup_neighbor(neighbor);
	}
	return(existing);
    }
    if (!(new = vines_make_new_path(neighbor)))
	return(NULL);

    /*
     * Fill in the new information.
     */
    new->neighbor      = neighbor;
    new->magic         = VINES_MAGIC_PATH;
    new->metric        = metric;
    new->uses          = 0;
    new->flags         = flags;
    new->metric        = metric;
    new->idb           = idb;
    vines_set_path_timestamp(new);
    new->enctype       = enctype;
    memset(&new->dlc_addr, 0, sizeof(hwaddrtype));
    if (dlc_addr)
	bcopy(dlc_addr, &new->dlc_addr, sizeof(hwaddrtype));

    /* Set up interface path chain */
    lw_insert(&new->by_idb, &idb->path_list);

    /* SRTP info */
    new->state = 0;
    new->reflected_timestamp = 0;
    new->origin_timestamp = 0;
    new->local_timestamp = vines_sequence + 1;
    new->packet_id = 0;
    vines_set_path_flags(new, version);
    queue_init(&new->reassemblyQ, 0);

    vines_insert_new_path(neighbor, new);
    vines_fixup_neighbor(neighbor);

    /*
     * Housekeeping
     */
    vines_paths++;
    neighbor->path_count++;
    vines_neighbor_version++;
    vneigh_inc_type_counts(neighbor, idb);
    if (do_vines_table_debug(neighbor->net, neighbor->host))
	buginf("\nVRTP: create path to %#z via %s, timestamp %x",
	       neighbor->net, neighbor->host,
	       ((idb != vinesnullidb) ? idb->phys_idb->namestring : "self"),
	       vines_sequence + 1);
    return(new);
}


/*
 * vines_unlink_path
 *
 * Unlink a specific path from the neighbor entry, and relink it on the
 * list of free paths.  This routine takes the responsibility of insuring
 * neighbor node consistency after moving the path entry.
 */
static inline void vines_unlink_path (
    vinesneighbortype *neighbor,
    vinespathtype *prev,
    vinespathtype *path)
{
    leveltype level;

    /* Delete the old queue entries */
    vines_flush_queue(&path->reassemblyQ);
    mgd_timer_stop(&path->reassembly_timer);
    mgd_timer_stop(&path->retry_timer);
    mgd_timer_stop(&path->rc_timer);
    lw_remove(&path->by_idb);

    /* Update counts */
    vines_paths--;
    neighbor->path_count--;
    vines_dec_type_counts(neighbor, path->idb);

    /* Finally remove it and rebuild best_path list */
    level = raise_interrupt_level(NETS_DISABLE);
    path->neighbor = NULL;
    prev->next = path->next;
    path->next = NULL;
    vines_fixup_neighbor(neighbor);
    reset_interrupt_level(level);
    free(path);
}

/*
 * vines_delete_path
 *
 * Delete an individual path from this neighbor structure.  If all paths
 * are deleted, then the server table must be checked to find any routes
 * the are using this neighbor as a gateway.  The unlink_path routine
 * (via fixup_neighbor) takes care of flushing the route and FS cache tables.
 */
vinesneighbortype *vines_delete_path (
    vinesneighbortype *p,
    ulong flags,
    vinesidbtype *idb,
    int enctype,
    hwaddrtype *dlc_addr)
{
    vinespathtype *path, *prev;
    boolean changed;

    changed = FALSE;
    FOR_ALL_PATHS_W_PREV(p, path, prev) {
	if (flags && !(flags & path->flags))
	    continue;
	if (idb && (idb != path->idb))
	    continue;
	if (enctype && (enctype != path->enctype))
	    continue;
	if (dlc_addr && (bcmp(dlc_addr, &path->dlc_addr, sizeof(hwaddrtype)) != 0))
	    continue;
	if (idb && do_vines_table_debug(p->net, p->host))
	    buginf("\nVRTP: delete path to neighbor %#z via %s",
		   p->net, p->host, idb->phys_idb->namestring);
	vines_unlink_path(p, prev, path);
	path = prev;
	changed = TRUE;
    }
    if (changed) {
	vines_neighbor_version++;
	if (p->available_paths == NULL) {
	    vines_delete_neighbor_entry(p);
	    return(NULL);
	}
    }
    return(p);
}

/*
 * vines_flush_paths_by_flag
 *
 * Given a neighbor entry, flush all paths given a certain criteria.
 * This is generally used to flush all dynamic paths associated with a
 * neighbor, because a better path to that neighbor was discovered.
 */
vinesneighbortype *vines_flush_paths_by_flag (
    vinesneighbortype *neighbor,
    int flag,
    vinespathtype *except)
{
    vinespathtype *path, *prev;
    leveltype level;
    boolean changed, keep_routes;
    sys_timestamp dummy;

    if (!neighbor)
	return(NULL);
    changed = FALSE;
    keep_routes = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);
    FOR_ALL_PATHS_W_PREV(neighbor, path, prev) {
	if (except && (path == except)) {
	    keep_routes = TRUE;
	    continue;
	}
	if (path->flags & flag) {
	    vines_unlink_path(neighbor, prev, path);
	    path = prev;
	    changed = TRUE;
	} else if ((path->flags & V_N_PLACEHOLDER) == 0) {
	    keep_routes = TRUE;
	}
    }
    reset_interrupt_level(level);
    if (changed) {
	vines_neighbor_version++;
	vines_flush_cache_by_neighbor(neighbor);
	if (!keep_routes) {
	    TIMER_STOP(dummy);
	    vines_flush_routes_by_neighbor(neighbor, V_NET_RTP|V_NET_TEST, dummy);
	}
	if (!neighbor->available_paths) {
	    vines_delete_neighbor_entry(neighbor);
	    return(NULL);
	}
    }
    return(neighbor);
}

/*
 * vines_flush_dynamic_paths
 *
 * Flush all dynamically learned paths to a given network:host pair.
 * This routine is called from the 'clear vines' command code.
 */
void vines_flush_dynamic_paths (ulong net, ushort host)
{
    vinesneighbortype *gw;

    if (!(gw = vneigh_FindEntry(net, host)))
	return;
    gw = vines_flush_paths_by_flag(gw, (V_N_DYNAMIC|V_N_TEST), NULL);
}

/*
 * vines_flush_all_paths_by_xxx
 *
 * This routine scans the entire neighbor table, flushing all dynamically
 * learned paths.  It leaves only the statically configured paths. The
 * unlink_path routine takes care of flushing the route and FS cache tables.
 */
static boolean vines_flush_one_entry (treeLink *link, void *dummy)
{
    vinesneighbortype *p;
    vinespathtype *path, *prev;
    vneigh_fn_data *data;
    boolean entry_changed;

    p = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    if (!vines_verify_neighbor_structure(p)) {
	vines_delete_neighbor_entry(p);
	return(TRUE);
    }

    entry_changed = FALSE;
    FOR_ALL_PATHS_W_PREV(p, path, prev) {
	if (data->idb && (path->idb != data->idb))
	    continue;
	if (data->flag && !(path->flags & data->flag))
	    continue;
	if (do_vines_table_debug(p->net, p->host))
	    buginf("\nVRTP: delete path to neighbor %#z via %s",
		   p->net, p->host, path->idb->phys_idb->namestring);
	vines_unlink_path(p, prev, path);
	path = prev;
	entry_changed = TRUE;
    }
    if (entry_changed) {
	if (!p->available_paths) {
	    vines_delete_neighbor_entry(p);
	    return(TRUE);
	}
	return(TRUE);
    }
    return(FALSE);
}

/*
 * vines_flush_all_paths_by_xxx
 *
 * This routine scans the entire neighbor table, flushing all dynamically
 * learned paths.  It leaves only the statically configured paths.
 */
void vines_flush_all_paths_by_xxx (int flag, vinesidbtype *idb)
{
    vneigh_fn_data data;
    sys_timestamp oldtime;

    if (idb)
	flag &= ~V_N_STATIC;

    data.flag = flag;
    data.idb = idb;
    oldtime = mgd_timer_exp_time(&vines_changed);
    vneigh_WalkTree(vines_flush_one_entry, &data);
    if (TIMERS_NOT_EQUAL(oldtime, mgd_timer_exp_time(&vines_changed))) {
	vines_neighbor_version++;
	if (idb) {
	    vines_flush_cache_by_idb(hwidb_or_null(idb->phys_idb));
	}
    }
}

/*
 * vines_flush_static_paths
 *
 * This routine scans the entire neighbor table, flushing all statically
 * learned paths ona given interface.  It is called when the
 * encapsulation changes on an interface.
 */
void vines_flush_static_paths (idbtype *swidb, int old_encap)
{
    vinesidbtype *idb;
    vneigh_fn_data data;
    sys_timestamp oldtime;

    if (!vines_on_idb(swidb))
	return;
    idb = vines_getidb(swidb);
    data.flag = V_N_STATIC;
    data.idb = idb;
    oldtime = mgd_timer_exp_time(&vines_changed);
    vneigh_WalkTree(vines_flush_one_entry, &data);
    if (TIMERS_NOT_EQUAL(oldtime, mgd_timer_exp_time(&vines_changed))) {
	vines_neighbor_version++;
	if (idb) {
	    vines_flush_cache_by_idb(hwidb_or_null(swidb));
	}
    }
}

/*
 * vines_poison_one_entry
 *
 * An interface has gone down.  Search through the neighbor table looking
 * for any entries that use this interface.  Mark all paths that use this
 * interface as unreachable.  If all paths to a neighbor have become
 * unreachable, then the routing table needs to be scanned to see if any
 * route have become unreachable.
 */
static boolean vines_poison_one_entry (treeLink *link, void *dummy)
{
    vinesneighbortype *p;
    vinespathtype *path;
    vneigh_fn_data *data;
    boolean entry_changed, do_debug;

    p = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    if (!vines_verify_neighbor_structure(p)) {
	vines_delete_neighbor_entry(p);
	return(TRUE);
    }

    do_debug = do_vines_rtp_debug(p->net, p->host);
    do_debug |= do_vines_table_debug(p->net, p->host);
    entry_changed = FALSE;
    FOR_ALL_PATHS(p, path) {
	if (path->flags & V_N_STATIC)
	    continue;
	if (data->idb && (path->idb != data->idb))
	    continue;
	TIMER_START(path->gctime, VRTP_HELLOINVALID(path->idb));
	path->metric = VINES_INFINITY;
	entry_changed = TRUE;
	if (do_debug)
	    buginf("\nVRTP: marking neighbor %#z unreachable via %s",
		   p->net, p->host, path->idb->phys_idb->namestring);
    }
    /*
     * Do not call delete_entry() here.  Nothing was deleted, only
     * metrics were changed.Fixup_neighbor will insure that
     * the FS cache is flushed.
     */
    if (entry_changed) {
	vines_fixup_neighbor(p);
	return(TRUE);
    }
    return(FALSE);
}

void vines_poison_all_paths_by_idb (vinesidbtype *idb)
{
    vneigh_fn_data data;
    sys_timestamp oldtime;

    data.idb = idb;
    oldtime = mgd_timer_exp_time(&vines_changed);
    vneigh_WalkTree(vines_poison_one_entry, &data);

    /*
     * Force a flash update of the entries that were changed.
     */
    if (!mgd_timer_running(&vines_changed))
	mgd_timer_start(&vines_changed, VRTP_FLASHUPDATE);
}

/*
 * vines_add_neighbor
 *
 * Add a new neighbor entry to the tables.  This routine first looks to
 * see if the neighbor already exists, and if so, returns it.  Otherwise
 * it allocates space for a new entry, fills it in, and links it into the
 * approptiate place in the table.
 */
vinesneighbortype *vines_add_neighbor (
    ulong net,
    ushort host,
    ulong flags,
    int metric,
    int version,
    uchar type)
{
    vinesneighbortype *p;

    /*
     * See if an entry exists for this neighbor.  If not, create a new
     * entry for it.
     */
    p = vneigh_FindEntry(net, host);
    if (p)
	return(p);

    p = vneigh_CreateEntry(net, host);
    if (p == NULL)
	return(NULL);

    p->magic = VINES_MAGIC_NEIGHBOR;
    p->newflags = flags;		/* for the V_N_ME flag */
    p->node = type;
    p->metric = metric;
    p->is_router = (type == VRTP_NODE_ROUTER);
    p->version = version;

    if (do_vines_table_debug(p->net, p->host))
	buginf("\nVRTP: create neighbor %#z", p->net, p->host);
    return(p);
}

/*
 * vines_delete_neighbor_entry
 *
 * Given a VINES neighbor structure, eradicate it from the internal tables.
 */
void vines_delete_neighbor_entry (vinesneighbortype *neigh)
{
    vinespathtype *path, *prev;

    if (!neigh)
	return;

    /*
     * Get rid of all usage of this neighbor in the route tables
     */
    vines_flush_routes_by_neighbor(neigh, 0, timer_no_time);
    vines_flush_cache_by_neighbor(neigh);
    if (do_vines_table_debug(neigh->net, neigh->host))
	buginf("\nVRTP: delete neighbor %#z", neigh->net, neigh->host);
    vneigh_DeleteEntry(neigh);

    /*
     * Now delete all the paths.
     */
    FOR_ALL_PATHS_W_PREV(neigh, path, prev) {
	vines_unlink_path(neigh, prev, path);
	path = prev;
    }
}

/*
 * vines_delete_neighbor
 *
 * Given a network number and host number, look up the neighbor structure
 * and remove it from the tables.
 */
void vines_delete_neighbor (ulong net, ushort host)
{
    vinesneighbortype *p;

    p = vneigh_FindEntry(net, host);
    vines_delete_neighbor_entry(p);
}    

/**********************************************************************
 *
 *				LOOKUP
 *
 **********************************************************************/

static void vines_dump_neighbor (vinesneighbortype *n)
{
    vinespathtype *p;
    int depth;

    buginf("\nNeighbor         Parent   Left     Right    Free     nextl    prevl    busy");
    buginf("\n%16z %08x %08x %08x %08x %08x %08x %d", n->net, n->host, n->link.parent,
	   n->link.left, n->link.right, n->link.freeList, n->link.nextl, n->link.prevl,
	   n->link.busy);

    buginf("\n              Neighbor Magic    cnt avail    best     primary");
    buginf("\n              %08x %08x %03d %08x %08x %08x", n, n->magic, n->path_count,
	   n->available_paths, n->best_path, n->primary_path);

    buginf("\n              Path     next     nextb    magic    idb");
    if (validlink(n->available_paths)) {
	depth = 0;
	FOR_ALL_PATHS(n, p) {
	    buginf("\n           %2d %08x %08x %08x %08x %08x", depth, p, p->next,
		   p->next_best, p->magic, p->idb);
	    if (!validlink(p->next))
		break;
	    depth++;
	}
    }
}

/*
 * vines_get_path_by_xxx
 *
 * Given a neighbor entry and a set of criteria, find the first entry that
 * matches the criteria.
 */
vinespathtype *vines_get_path_by_xxx (
    vinesneighbortype *neighbor,
    ulong flags,
    vinesidbtype *idb,
    int enctype,
    hwaddrtype *dlc_addr)
{
    vinespathtype *path;

    if (neighbor == NULL)
	return(NULL);

    if (!validlink(neighbor->available_paths)) {
	errmsg(&msgsym(BADPARAM,VINES), "get_path_by_xxx", "neighbor", "free memory");
	vines_dump_neighbor(neighbor);
	neighbor->available_paths = NULL;
	return(NULL);
    }	
    FOR_ALL_PATHS(neighbor, path) {
	if (!validlink(path->next)) {
	    char foo[40];
	    sprintf(foo, "%x", path->next);
	    errmsg(&msgsym(BADPARAM,VINES), "get_path_by_xxx", "path->next", foo);
	    vines_dump_neighbor(neighbor);
	    path->next = NULL;
	    return(NULL);
	}
	if (flags && !(path->flags & flags))
	    continue;
	if (idb && (idb != path->idb))
	    continue;
	if (enctype && (enctype != path->enctype))
	    continue;
	if (dlc_addr && (bcmp(dlc_addr, &path->dlc_addr, sizeof(hwaddrtype)) != 0))
	    continue;
	return(path);
    }
    return(FALSE);
}

/*
 * vines_best_path_uses_idb
 *
 * Given a neighbor entry and an interface, see if any of the best
 * paths to this neighbor use the given interface.
 */
boolean vines_best_path_uses_idb (
    vinesneighbortype *neighbor,
    vinesidbtype *idb)
{
    vinespathtype *path;

    if (!neighbor || !neighbor->best_path)
	return(FALSE);
    path = neighbor->best_path;
    do {
	if (idb == path->idb)
	    return(TRUE);
	path = path->next_best;
    } while (path != neighbor->best_path);
    return(FALSE);
}


/*
 * vines_best_path_from_delta_or_snapshot
 *
 * Check all paths to see if one of them is from an interface that uses
 * delta updates or snapshot updates.  This routine is called from the route
 * ager routine to see whether or not an old route should be expired.
 */
boolean vines_best_path_from_delta_or_snapshot (vinesroutetype *route)
{
    vinespathtype *path;
    vinesneighbortype *neighbor = (route == NULL) ? NULL : route->gw;
    
    if (!neighbor)
	return(FALSE);
    path = neighbor->best_path;
    do {
	if ((path->idb->vines_update_delta_only) ||
	    ((path->snapshot_no_age) &&
	     (!(route->snapshot_force_aging))))
	    return(TRUE);
	path = path->next_best;
    } while (path != neighbor->best_path);
    return(FALSE);
}


/*
 * vines_get_neighbor_metric
 *
 * Return the metric value for this neighbor.  This routine is called by
 * the transport layer to deterime the appropriate initial setting for
 * timeouts.
 */
int vines_get_neighbor_metric (ulong net, ushort host)
{
    vinesneighbortype *neighbor;

    neighbor = vneigh_FindEntry(net, host);
    return(neighbor ? neighbor->metric : 0);
}

/*
 * vines_get_slowest_update_interval
 *
 * Return the slowest update interval of all the interfaces through which
 * this neighbor is known.  This routine is called by the route ading
 * routines to deterime when a route should be considered invalid.
 */
int vines_get_slowest_update_interval (vinesneighbortype *neighbor)
{
    vinespathtype *path;
    int interval = 0;

    FOR_ALL_PATHS(neighbor, path) {
	if (path->idb->vines_update_delta_only)
	    return(-1L);
	interval = max(interval, path->idb->vines_wakeup_interval);
    }
    return(interval);
}

/**********************************************************************
 *
 *				UPDATE
 *
 **********************************************************************/

/*
 * vines_neighbor_update
 *
 * Perform updating of the VINES RTP routing tables.  Return TRUE if the
 * entry was actually updated, FALSE otherwise.  This indication is used
 * to see if the routes in this packet should be used or discarded.
 */
boolean vines_neighbor_update (
    paktype *pak,
    vinesneighbortype **this_neighbor,
    vinespathtype **this_path,
    char **error,
    int version,
    uchar type)
{
    vinesiptype *vip;
    vinesneighbortype *p;
    vinespathtype *path, *savepath;
    ulong net, interval;
    ushort host;
    int metric_in;
    vinesidbtype *idb;
    hwaddrtype dlc_address;
    boolean do_debug;

    vip = (vinesiptype *)pak->network_start;
    net = GET2WD(vip->ssrcnet);
    host = vip->srchost;
    do_debug = do_vines_rtp_debug(net, host);

    *error = NULL;

    /* 
     * Pitch illegal addresses, and see if the station exists.
     */
    if (!vines_valid_net(net) || !vines_valid_host(host)) {
	*error = "invalid address";
	return(FALSE);
    }
    idb = vines_getidb(pak->if_input);
    metric_in = vines_compute_metric(pak);
    memset(&dlc_address, 0, sizeof(hwaddrtype));
    if (!(*pak->if_input->hwptr->extract_snpa)(pak, &dlc_address, SOURCE)) {
	*error = "invalid dlc address";
	return(FALSE);
    }

    /*
     * If it was not found, enter it and fill in the information for this
     * interface.  If this station is multiply attached, the code will
     * worry about that when it receives the update from the other
     * interface(s).
     */
    p = vneigh_FindEntry(net, host);
    if (p == NULL) {
	p = vines_add_neighbor(net, host, V_N_DYNAMIC, metric_in, version, type);
	path = vines_add_path(p, V_N_DYNAMIC, idb, pak->enctype, &dlc_address,
			      metric_in, version, type);
	if (!p || !path) {
	    vines_delete_neighbor_entry(p);
	    return(FALSE);
	}
	vines_new_neighbor(p, path);
	*this_neighbor = p;
	*this_path = path;
	return(TRUE);
    }

    /*
     * Well, He's still there.  See if the specific entry for this
     * interface and this MAC address are in the table.  If it is, update
     * it and return.  If an entry is found for the right interface but
     * the wrong address, either the station has two interface cards on
     * the same network, or someone swapped an interface card.  Use twice
     * the routing interval to decide which case it is.  The metric and
     * encapsulation values must be updated for every update so that any
     * changes will be noticed. Fixup_neighbor should only be called
     * if the interface or metric has changed.
     *
     * Do not, I repeat, do not blithely change the flags on the neighbor
     * or path entry.  If this is a static neighbor, it needs to remain a
     * static neighbor.  The only changes that should ever be made it to
     * the flags field of a path is to indicate that an RTP update
     * overrides an RTP redirect
     */
    savepath = NULL;
    FOR_ALL_PATHS(p, path) {
	if (idb != path->idb)
	    continue;
	if (bcmp(&dlc_address, &path->dlc_addr, sizeof(hwaddrtype)) != 0) {
	    savepath = path;
	    continue;
	}
	if (path->flags & V_N_REDIRECT)
	    path->flags = V_N_DYNAMIC;
	path->enctype = pak->enctype;	/* Just in case it changed */
	vines_set_path_timestamp(path);
	if ((version == VRTP_VERSION) && (path->metric != metric_in)) {
	    /*
	     * Don't update here for SRTP neighbors.  SRTP will take
	     * care of this after checking for non-equal metrics
	     * between neighbors.
	     */
	    path->metric = metric_in;
	    vines_fixup_neighbor(p);
	}
	*this_neighbor = p;
	*this_path = path;
	return(TRUE);
    }
    if (savepath) {
	interval = vines_get_slowest_update_interval(p);
	if (interval == -1L)
	    interval = VRTP_DEF_WAKEUP_INTERVAL;
	if (CLOCK_OUTSIDE_INTERVAL(savepath->timer, VRTP_MOVETIME(interval))) {
	    bcopy(&dlc_address, &savepath->dlc_addr, sizeof(hwaddrtype));
	    if (do_debug && (host == 1))
		buginf(" (new hardware address)");
	    if (savepath->flags & V_N_REDIRECT)
		savepath->flags = V_N_DYNAMIC;
	    savepath->enctype = pak->enctype;	/* Just in case it changed */
	    vines_set_path_timestamp(savepath);
	    savepath->metric = metric_in;
	    vines_fixup_neighbor(p);
	    *this_neighbor = p;
	    *this_path = savepath;
	    return(TRUE);
	}
    }

    /*
     * At this point, this is a neighbor that already has an entry in the
     * tables, but it has multiple interfaces.  Add a new path to the
     * structure for the new interface card.  If the new path cannot be
     * added, just bail.  There is no need to destroy the neighbor
     * because we were trying to add a second or subsequent path, not the
     * first path.  Add_path will fix up the neighbor entry, so we
     * don't have to worry about the FS cache here. 
     */
    path = vines_add_path(p, V_N_DYNAMIC, idb, pak->enctype, &dlc_address,
			  metric_in, version, type);
    if (path == NULL)
	return(FALSE);
    if (do_vines_table_debug(p->net, p->host))
	buginf("\nVRTP: alternate path to neighbor %#z, interface %s",
	       p->net, p->host, idb->phys_idb->namestring);
    *this_neighbor = p;
    *this_path = path;
    return(TRUE);
}

/*
 * vines_route_adjust
 *
 * Handle interface up/down transitions.  When an interface comes up,
 * this routine sends out a RTP request.  When an interface goes down,
 * this routine poisons all routes through that interface, forces an
 * immediate flash update, and the flushes those routes.
 */
void vines_route_adjust (idbtype *swidb)
{
    vinesidbtype *idb;

    /*
     * Do we really want to do anything?
     */
    if (!vines_running)
	return;
    idb = vines_getidb(swidb);
    if (!idb || !idb->vines_metric)
	return;

    /*
     * Do different things depending on whether the interface came up or
     * went down.
     */
    if (vines_interface_up(idb) && !idb->vines_lineup) {
	idb->vines_lineup = TRUE;
	if (vinesrtp_debug || vinestable_debug)
	    buginf("\nVRTP: Interface %s came up.", swidb->namestring);
	vroute_interface_came_up(idb);
    } else if (!vines_interface_up(idb) && idb->vines_lineup) {
	idb->vines_lineup = FALSE;
	if (vinesrtp_debug || vinestable_debug)
	    buginf("\nVRTP: Interface %s went down.", swidb->namestring);
	vroute_interface_went_down(idb);
    }
}

/**********************************************************************
 *
 *			     HOUSEKEEPING
 *
 **********************************************************************/
#define VNEIGH_FLAG_FLUSH        0x01
#define VNEIGH_FLAG_CHECK_REINIT 0x02
#define VNEIGH_FLAG_SEND_REINIT  0x04

/*
 * vines_neighbor_ager
 *
 * Age each path in the VINES RTP neighbor tables.  Do each hash bucket
 * by running a pair of pointers through all the entries on the list.
 * One of the pointers will initially point to the address of the hash
 * bucket.  This makes life simpler by by not requiring a special case
 * for a delete at the head of the bucket.
 *
 * On an interface where only delta updates are send, this routine needs
 * to age out entries whose metrics are infinity.  It should never set a
 * metric to infinity on one of these interfaces.
 */
static boolean vneigh_age_entry (treeLink *link, void *dummy)
{
    vinesneighbortype *p;
    vneigh_fn_data *data;
    vinesidbtype *idb;
    vinespathtype *path, *prev;
    boolean entry_changed, result, do_debug;

    p = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    if (p->available_paths == NULL) {
	vines_delete_neighbor_entry(p);
	return(TRUE);
    }

    do_debug = do_vines_rtp_debug(p->net, p->host);
    do_debug |= do_vines_table_debug(p->net, p->host);
    entry_changed = FALSE;
    FOR_ALL_PATHS_W_PREV(p, path, prev) {
	if (path->flags & (V_N_STATIC | V_N_PLACEHOLDER))
	    continue;
	if (data->flag & VNEIGH_FLAG_FLUSH) {
	    if (do_debug)
		buginf("\nVRTP: flushing host entry for %#z on %s",
		       p->net, p->host, path->idb->phys_idb->namestring);
	    vines_unlink_path(p, prev, path);
	    path = prev;
	    entry_changed = TRUE;
	    continue;
	}
	if (path->metric == VINES_INFINITY) {
	    if (CLOCK_OUTSIDE_INTERVAL(path->gctime, VRTP_HELLOEXPIRE(path->idb))) {
		if (do_debug)
		    buginf("\nVRTP: garbage collecting host entry for %#z on %s",
			   p->net, p->host, path->idb->phys_idb->namestring);
		vines_unlink_path(p, prev, path);
		path = prev;
		entry_changed = TRUE;
	    }
	    continue;
	}
	if (path->snapshot_no_age)
	    continue;
	if ((p->version == VRTP_VERSION) && vines_no_updates(path->idb))
	    continue;
	if (CLOCK_OUTSIDE_INTERVAL(path->timer, VRTP_HELLOINVALID(path->idb))) {
	    if (do_debug)
		buginf("\nVRTP: timed out host entry for %#z on %s",
		       p->net, p->host, path->idb->phys_idb->namestring);
	    path->gctime = TIMER_ADD_DELTA(path->timer,
					   VRTP_HELLOINVALID(path->idb));
	    path->metric = VINES_INFINITY;
	    path->local_timestamp = vines_sequence + 1;
	    entry_changed = TRUE;
	    continue;
	}
	if (p->is_router && (p->version == VSRTP_VERSION) &&
	    (data->flag & VNEIGH_FLAG_CHECK_REINIT)) {
	    if (vsrtp_compare_3seqno(vsrtp_reinit_sequence, vines_sequence,
				     path->reflected_timestamp) != EQUAL) {
		/*
		 * Announce all out of sync neighbors.
		 */
		if (vines_reinit_debug)
		    buginf("\nVSRTP: neighbor %#z(%s) has old seq. number %08x",
			   p->net, p->host, path->idb->phys_idb->namestring,
			   path->reflected_timestamp);
		/*
		 * But only send one reinit message.
		 */
		if (data->flag & VNEIGH_FLAG_SEND_REINIT) {
		    data->flag &= ~VNEIGH_FLAG_SEND_REINIT;
		    TIMER_START(vsrtp_reinit_check_time, 2*VSRTP_REINIT_CHECK);
		    FOR_ALL_VIDBS(idb) {
			if (!vines_interface_up(idb) || !vines_srtp_present(idb))
			    continue;
			mgd_timer_update(&idb->wakeup_timer, VSRTP_REINIT_PAUSE);
			vsrtp_send_reinit(idb);
		    }
		}
	    }
	}
    }
    if (!p->available_paths) {
	vines_delete_neighbor_entry(p);
	result = TRUE;
    } else if (entry_changed) {
	vines_fixup_neighbor(p);
	result = TRUE;
    } else {
	result = FALSE;
    }
    process_suspend_if_req();
    return(result);
}

void vines_neighbor_ager (boolean flushflag)
{
    vneigh_fn_data data;
    sys_timestamp oldtime;

    data.flag = 0;
    if (flushflag)
	data.flag |= VNEIGH_FLAG_FLUSH;
    if (TIMER_RUNNING_AND_AWAKE(vsrtp_reinit_check_time)) {
	TIMER_STOP(vsrtp_reinit_check_time);
	data.flag |= (VNEIGH_FLAG_CHECK_REINIT | VNEIGH_FLAG_SEND_REINIT);
	if (vines_reinit_debug)
	    buginf("\nVSRTP: performing reinit check, initial seq. %08x, current seq. %08x",
		   vsrtp_reinit_sequence, vines_sequence);
    }
    oldtime = mgd_timer_exp_time(&vines_changed);
    vneigh_WalkTree(vneigh_age_entry, &data);
    if (TIMERS_NOT_EQUAL(oldtime, mgd_timer_exp_time(&vines_changed)))
	vines_neighbor_version++;
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * vneigh_ShowStats
 *
 * Show statistical information on the vines neighbor tree.
 */
void vneigh_ShowStats (parseinfo *csb)
{
    if (vines_neighbor_tree)
	RBTreeStats(vines_neighbor_tree, "VINES neighbor");
    else
	printf("\nVINES neighbor tree does not exist.");
}


static const char vines_neighbor_header[] = "
Address           Hardware Address     Type  Int       Flag Age  Metric    Uses
";
/* 
1234567890123456 12345678901234567890 VINES To3/4.255 E5*. 123   1234  1234567 */

static const char vines_path_self_line[] =  "-%*c -     -         %c      -      -        -";
static const char vines_path_place_line[] = "-%*c -     -         %c      -      -  %-7d";

/*
 * vines_neighbortype
 *
 * Return a single character identifying how this neighbor address was
 * learned.
 */
static char vines_neighbortype (ulong flags)
{
    if (flags & V_N_SELF)
        return('C');
    if (flags & V_N_PLACEHOLDER)
        return('P');
    if (flags & V_N_STATIC)
        return('S');
    if (flags & V_N_REDIRECT)
	return('D');
    if (flags & V_N_DYNAMIC)
	return('R');
    if (flags & V_N_IGRP)
	return('I');
    if (flags & V_N_TEST)
	return('T');
    return('?');
}

/*
 * vines_display_paths
 *
 * Display all paths in a given linked list.  This routine is called from
 * vines_display_neighbors().
 */
static void vines_display_paths (
    vinesneighbortype *p,
    vinesidbtype *target)
{
    vinespathtype *path, *next;
    vinesidbtype *idb;
    idbtype *swidb;
    boolean first = TRUE;
    ulong time;
    char flag1, flag2, flag3, buffer[40];

    for (path = p->available_paths; path; path = next) {
	/*
	 * Check command line qualifiers
	 */
	if (target && (target != path->idb)) {
	    next = path->next;
	    continue;
	}
	mem_lock(path);

	/*
	 * Print common information.
	 */
	if (first) {
	    printf("\n%#16z  ", p->net, p->host);
	    first = FALSE;
	} else {
	    printf("\n %#16z ", p->net, p->host);
	}

	/*
	 * There are a couple of entries that are not associated with an
	 * interface.  Check for these now.
	 */
	if (path->idb == vinesnullidb) {
	    if (path->flags & V_N_SELF) {
		printf(vines_path_self_line,
		       STATIONLEN_X121-1, ' ', 'C');
	    } else if (path->flags & V_N_PLACEHOLDER) {
		printf(vines_path_place_line,
		       STATIONLEN_X121-1, ' ', 'P',
		       path->uses);
	    }
	    next = path->next;
	    free(path);
	    continue;
	}

	/*
	 * Print one line for this path in the entry.
	 */
	idb = path->idb;
	swidb = idb->phys_idb;
	vines_dlc(buffer, &path->dlc_addr);
	printf("%*s %5s %9s", STATIONLEN_X121, buffer,
	       encstring(path->enctype), swidb->short_namestring);

	flag1 = vines_neighbortype(path->flags);
	if (path == p->best_path) {
	    flag2 = '*';
	    flag3 = (path->metric != p->metric) ? 'h' : ' ';
	} else {
	    flag2 = (path->metric == p->metric) ? '.' : ' ';
	    flag3 = ' ';
	}
	printf(" %c%d%c%c ", flag1, p->version, flag2, flag3);
	if (path->flags & V_N_STATIC) {
	    printf("  -");
	} else if ((p->version == VRTP_VERSION) && vines_no_updates(idb)) {
	    printf("n/a");
	} else {
	    time = ELAPSED_TIME(path->timer)/ONESEC;
	    time = min(time, 999);
	    printf("%-3d", time);
	}
	printf("   %04x %-8u", path->metric, path->uses);
	next = path->next;
	free(path);
    }
}

/*
 * vines_display_neighbors
 *
 * Display part or all of the VINES neighbor table.  This routine may
 * display multiple lines of output per neighbor.  It will be displayed
 * as one line for each path associated with that neighbor.
 */
static boolean vneigh_display_entry (treeLink *link, void *dummy)
{
    vinesneighbortype *p;
    vneigh_fn_data *data;

    p = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    if (!vines_verify_neighbor_structure(p)) {
	vines_delete_neighbor_entry(p);
	return(!automore_quit());
    }
    
    if (data->net) {
	if (data->net != p->net)
	    return(TRUE);
	if (data->host && (data->host != p->host))
	    return(TRUE);
    }

    vines_display_paths(p, data->idb);
    if (p->newflags & V_N_ME)
	return(!automore_quit());

    /*
     * If one specific neighbor was requested, provide all the detailed counts.
     */
    if (data->net && data->host) {
	automore_header(NULL);
	switch (p->version) {
	  case VRTP_VERSION:  vrtp_display_neighbor(p);  break;
	  case VSRTP_VERSION: vsrtp_display_neighbor(p); break;
	  default:
	    printf("\n  Illegal version number %d encountered.", p->version);
	    break;
	}
    }

    return(!automore_quit());
}

/*
 * vines_display_neighbors
 *
 * Display part or all of the VINES neighbor table.  This routine may
 * display multiple lines of output per neighbor.  It will be displayed
 * as one line for each path associated with that neighbor.
 */
void vines_display_neighbors (ulong net, ushort host, vinesidbtype *idb)
{
    vneigh_fn_data data;

    data.idb = idb;
    data.net = net;
    data.host = host;

    automore_enable(NULL);
    printf("\n%d neighbors, %d paths, version %d, next update %d seconds",
	   vines_neighbor_tree->activeNodes,vines_paths,vines_neighbor_version,
	   mgd_timer_left_sleeping(&vines_idb_timer) / ONESEC);
    automore_header(vines_neighbor_header);
    vneigh_WalkPartialTree(vneigh_display_entry, NULL, &data);
    automore_disable();
}

/*
 * vines_display_paths_on_idb
 *
 * Display any vines neighbor paths that point to the specified idb.
 * These are all already in a queue hanging off the idb, so there is no
 * need to run the entire routing tree.  This routine is called from the
 * routines to display generic interface information.
 */
static void vines_display_paths_on_idb (vinesidbtype *idb)
{
    vinesneighbortype *neighbor;
    vinespathtype *path;
    dqueue_t *q;
    int count;
    uint DisplayBufSize;
    char *displaybuf;
    
    printf("\n  Neighbors: ");
    if (idb->path_list.flink == &idb->path_list) {
	printf(" none.");
	return;
    }

    displaybuf = malloc_named(VINESBUFLEN, "Vines Display Temp");
    if (displaybuf == NULL)
	return;
    DisplayBufSize = VINESBUFLEN;
    
    count = 0;
    for (q = idb->path_list.flink; q != &idb->path_list; q = q->flink) {
	path = (vinespathtype *)q->parent;
	neighbor = path->neighbor;

	if ((count++ % 4) == 0)
	    safe_sprintf(&displaybuf, &DisplayBufSize, "\n    ");
	safe_sprintf(&displaybuf, &DisplayBufSize, "%d %#16z  ",
		     neighbor->version, neighbor->net, neighbor->host);
    }
    printf("%s", displaybuf);
    free(displaybuf);
}

/*
 * vneigh_display_interface
 *
 * Print all neighbor specific information that is maintained on a per
 * interface basis.
 */
void vneigh_display_interface (vinesidbtype *idb)
{
    printf("\n  Nodes present: %d 5.5x servers, %d 5.5x routers, %d 5.5x clients",
	   idb->v1_servers, idb->v1_routers, idb->v1_clients);
    printf("\n                 %d 4.11 servers, %d 4.11 routers, %d 4.11 clients",
	   idb->v0_servers, idb->v0_routers, idb->v0_clients);
    vines_display_paths_on_idb(idb);
}

/**********************************************************************
 *
 *			    MISCELLANEOUS
 *
 **********************************************************************/

int vines_get_enctype (long ptr)
{
    routingdecision *p = (routingdecision *)ptr;

    return(p->path->enctype);
}

uchar *vines_get_mac_address (long ptr)
{
    routingdecision *p = (routingdecision *)ptr;

    return(p->path->dlc_addr.addr);
}

ulong vines_get_net (long ptr)
{
    return(((routingdecision *)ptr)->neighbor->net);
}

ushort vines_get_host (long ptr)
{
    return(((routingdecision *)ptr)->neighbor->host);
}

/*
 * vines_new_neighbor
 *
 * A new neighbor station has been detected.  First perform any special
 * activities here for new neighbors that are servers, then special
 * activities for any new neighbor.
 */
void vines_new_neighbor (vinesneighbortype *neighbor, vinespathtype *path)
{
    vinesidbtype *idb;

    idb = path->idb;
    if (neighbor->host == VINES_SERVER) {
	if (neighbor->version == VSRTP_VERSION) {
	    /*
	     * If the new neighbor is the first neighbor that performs
	     * routing, send a broadcast, otherwise send a unicast.  A
	     * broadcast is sent for the first neighbor because it might
	     * be that this router has just joined an existing network.
	     * In that is the case, the first update would be the first
	     * of many needed if they were sent as unicasts.  VPD 5-95.
	     */
	    if ((idb->v1_servers + idb->v1_routers) == 1) {
		vsrtp_send_update(VSRTP_FULL_UPDATE, idb, NULL);
	    } else {
		vsrtp_send_update(VSRTP_FULL_UPDATE, idb, path);
	    }
	}
	vsvc_new_neighbor(neighbor->net, path->idb);
    }

    /*
     * Things to do for all neighbors.
     */
}

/*
 * vines_start_aging_one_entry
 *
 * Start the aging of a single entry, usually due to changes in snapshot
 * routing.
 */
static boolean vines_start_aging_one_entry (treeLink *link, void *dummy)
{
    vinesneighbortype *p;
    vinespathtype *path;
    vneigh_fn_data *data;

    p = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    if (!vines_verify_neighbor_structure(p)) {
	vines_delete_neighbor_entry(p);
	return(TRUE);
    }

    /* If not the neighbor we are interested in, continue... */ 
    if (data->net != 0L) {
	if ((p->net != data->net) || (p->host != data->host))
	  return(FALSE);
    }
    
    /* Set the 'SNAPSHOT_AGE' bit in all routes learned via this neighbor */
    vines_start_aging_routes_by_neighbor(p);

    /* Turn off snapshot no-aging for all paths learned from this neighbor */
    FOR_ALL_PATHS(p, path) {
	if (!path->snapshot_no_age)
	    continue;
	if (path->metric == VINES_INFINITY) {
	    path->snapshot_no_age = FALSE;
	    continue;
	}
	if (path->idb->phys_idb != data->phys_idb)
	    continue;
	path->snapshot_no_age = FALSE;
	vines_set_path_timestamp(path);
    }
    return(FALSE);
}
  
/*
 * vines_start_aging_paths
 *
 * Called by Snapshot to start aging paths on this hwidb, from the address
 * given in this VINES packet, if a packet is passed in.
 */
void vines_start_aging_paths (idbtype *phys_idb, paktype *pak)
{
    vinesiptype *vip;
    vneigh_fn_data data;

    if (!vines_running)
        return;

    if (pak != NULL) {
	vip = (vinesiptype *)pak->network_start;
	data.net = GETLONG(vip->ssrcnet);
	data.host = vip->srchost;
    } else {
	data.net = 0L;
	data.host = 0;
    }
    data.phys_idb = phys_idb;
    vneigh_WalkTree(vines_start_aging_one_entry, &data);
}
