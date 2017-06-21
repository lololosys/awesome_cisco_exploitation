/* $Id: isis_spf.c,v 3.5.6.3 1996/08/28 12:38:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_spf.c,v $
 *------------------------------------------------------------------
 * isis_spf.c -- SPF algorithm for IS-IS
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file includes the routines necessary to run the SPF
 * algorithm over the IS-IS database.
 *------------------------------------------------------------------
 * $Log: isis_spf.c,v $
 * Revision 3.5.6.3  1996/08/28  12:38:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.6.2  1996/05/04  20:04:17  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/03/18  19:04:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  06:31:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/07  08:41:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  13:41:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/05  23:10:18  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.4  1995/12/01  17:48:54  dkatz
 * CSCdi44093:  ISIS crashes on purpose
 *
 * Revision 3.3  1995/12/01  02:48:34  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.2  1995/11/17  08:54:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:52:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/23  21:37:52  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.8  1995/08/25  15:12:12  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.7  1995/08/25  04:39:01  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Rework the circumstances under which level-2 LSPs are regenerated
 * for IP summarization.
 *
 * Revision 2.6  1995/08/23  18:26:11  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.5  1995/08/23  18:03:20  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.4  1995/08/08  16:43:14  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.3  1995/06/21 08:54:00  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/09  12:59:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:19:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * A note about suspending during SPF:
 *
 * The SPF process may suspend at the bottom of the loop.  Critical resources
 * that SPF uses include:
 *	The adjacency database
 *	The LSP database
 *	The TENT list
 *
 * Consistency is maintained as follows:
 *
 * Each LSP may point to one or more adjacencies (the next hops toward the
 * router that issued the LSP).  These adjacencies are maintained across
 * SPF runs, since partial route updates use them.  Protection is maintained
 * by mem_lock-ing the adjacencies when they are written down in the LSP
 * database.  If the adjacency is deleted in the middle of an SPF run,
 * the pointers will stay valid for the remainder of the run, and another
 * SPF will have been scheduled to run immediately afterward (due to the
 * topology change).  At the initialization of that SPF run, the memory will
 * be unlocked and the adjacency will finally be free to go.
 *
 * The LSP database is normally not changed structurally during an SPF run.
 * The one exception is when an external command clears the database.  If
 * this occurs, the act of freeing the LSPs will delink the TENT list and
 * decrement the tent_count.  The SPF suspends just before pulling the
 * next entry from the TENT list, so the SPF will never pick up a bogus
 * LSP pointer from the TENT list (in fact, it will probably bail immediately,
 * as tent_count will have gone to zero).
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "../clns/isis_ipx.h"
#include "../xns/novisis.h"
#include "../xns/ipx_registry.h"
#include "../xns/novnlsp.h"
#include "clns_registry.h"
#include "isis_registry.h"


/*
 * isis_viable_link
 *
 * If there is no link in the child's LSP which identifies the parent,
 * don't use node for a link. Otherwise, return TRUE.
 *
 */
static boolean isis_viable_link (clns_pdbtype *pdb,
				 isis_lspid *parent,
				 isis_lspdbtype *child,
				 int level)
{
    isis_lspdbtype  *child_lsp;
    isis_info_type  *item;
    isis_generic_ad *ad;
    uchar	    *id;

    /*
     * In child LSP, search for a link pointing back to parent.
     *
     * Note that although pdb->L1_adj_type will equal CLNS_L1_IS_ADJ_TYPE,
     * this will also work for level 2 (since the item code is the same,
     * ISIS_IS_NEIGHBORS, for both level 1 and level 2).
     */
    ad = NULL;
    item = NULL;
    child_lsp = child;
    while ((ad = isis_walk_lsp(pdb, &child_lsp, &item, ad, 
			       pdb->L1_adj_type, level))) {
	if (pdb->proc_type == PROC_TYPE_NLSP)
	    id = ad->ipx_link_info.neighbor_id;
	else
	    id = ad->is_neighbor.neighbor_id;
	if (systemid_match(parent->system_id, id)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}


/*
 * isis_lock_parent
 *
 * Mem_lock the next_hop adjacency of a parent entry, if it is there.
 */
static inline void isis_lock_parent (isis_lsp_parent *parent)
{
    if (parent) {
	if (parent->next_hop) {
	    mem_lock(parent->next_hop);
	}
    }
}


/*
 * isis_unlock_parent
 *
 * Unlock the next_hop adjacency of a parent entry, if it is there.
 */
static inline void isis_unlock_parent (isis_lsp_parent *parent)
{
    if (parent) {
	if (parent->next_hop) {
	    free(parent->next_hop);
	}
    }
}


/*
 * isis_adj_tie_break
 *
 * Given a full set of next-hops and a new potential one, compare the new
 * one to all existing ones, and if the new one is "better" than an existing
 * next-hop, replace the old one.
 *
 * If maintain_locks is TRUE, we must lock and unlock the adjacencies as
 * we tinker.
 */
static void isis_adj_tie_break (clns_pdbtype *pdb, 
				isis_lsp_parent existing[],
				isis_lsp_parent *new,
				int level,
				boolean maintain_locks)
{

    clns_adjtype *adj, *new_adj;
    int          i, compare;
    isisidbtype  *adj_isisidb, *newadj_isisidb;

    new_adj = new->next_hop;
    
    for (i = 0; i < ISIS_MAX_PATHS; i++) {
	if (!(adj = existing[i].next_hop)) return;

	/*
	 * First, prefer ES and PR adjacencies over anything else.
	 */
	if (new_adj->adjacencyType == CLNS_ES_ADJ_TYPE &&
	    adj->adjacencyType != CLNS_ES_ADJ_TYPE) goto override;

	if (new_adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE &&
	    adj->adjacencyType != CLNS_PREFIX_ADJ_TYPE) goto override;

	if (adj->adjacencyType == CLNS_ES_ADJ_TYPE ||
	    adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE) continue;

	/*
	 * AdjacencyType same, now try system ids.
	 */
	compare = compare_stationid(&new_adj->key[1], &adj->key[1]);
	if (compare < 0) goto override;
	if (compare > 0) continue;

	/*
	 * System IDs same, now try circuit ids.
	 */
	adj_isisidb = get_isis_idb(pdb, adj->idb);
	if (!adj_isisidb)
	    continue;
	newadj_isisidb = get_isis_idb(pdb, new_adj->idb);
	if (!newadj_isisidb)
	    continue;
	if (level == ISIS_CIRCUIT_L1) {
	    compare = compare_stationid(newadj_isisidb->isis_L1_circuit_id,
					adj_isisidb->isis_L1_circuit_id);
	    if (!compare) {
		if (newadj_isisidb->isis_L1_circuit_id[ISIS_PSEUDO_ID] <
		    adj_isisidb->isis_L1_circuit_id[ISIS_PSEUDO_ID]) {
		    compare = -1;
		}
	    }
	} else {
	    compare = compare_stationid(newadj_isisidb->isis_L2_circuit_id,
					adj_isisidb->isis_L2_circuit_id);
	    if (!compare) {
		if (newadj_isisidb->isis_L2_circuit_id[ISIS_PSEUDO_ID] <
		    adj_isisidb->isis_L2_circuit_id[ISIS_PSEUDO_ID]) {
		    compare = -1;
		}
	    }
	}
	if (compare < 0) goto override;
	if (compare > 0) continue;

	/*
	 * Circuit IDs same. Last tie breaker, compare SNPA addresses.
	 */
	compare = compare_stationid(new_adj->snpa.addr, adj->snpa.addr);
	if (compare < 0) goto override;
    }

    return;

 override:
    if (maintain_locks) {
	isis_lock_parent(new);
	isis_unlock_parent(&existing[i]);
    }
    existing[i] = *new;
}

/*
 * update_nlsp_params
 *
 * Combines this hop's NLSP metrics with the root path's set
 */
static inline void update_nlsp_params (clns_pdbtype *pdb,
				       isis_lsp_parent *result,
				       isis_lsp_parent *leaf,
				       isis_lsp_parent *root,
				       ulong netmask)
{
    result->throughput = min(leaf->throughput, root->throughput);
    result->delay = leaf->delay + root->delay;
    result->mtu = min(leaf->mtu, root->mtu);
    if (netmask == NOV_NETMASK_ALL)
	result->hops = leaf->hops + root->hops;
    else 
	result->hops = leaf->hops;
    result->ticks = leaf->ticks + root->ticks;
    spf_ebuginf(pdb, TRUE, "thru %d/%d/%d, delay %d/%d/%d, mtu %d/%d/%d, "
		"hops %d/%d/%d, ticks %d/%d/%d",
		root->throughput, leaf->throughput, result->throughput,
		root->delay, leaf->delay, result->delay,
		root->mtu, leaf->mtu, result->mtu,
		root->hops, leaf->hops, result->hops,
		root->ticks, leaf->ticks, result->ticks);
}

/*
 * isis_install_potential_next_hop
 *
 * Attempt to install a next_hop entry for this IS
 */
static void isis_install_potential_next_hop (clns_pdbtype *pdb, 
					     isis_lsp_parent *potential,
					     isis_lspdbtype *child,
					     int level)
{
    int j;

    /* Compare against the next_hops already present.  Punt if there. */
    
    for (j=0; j<child->parent_count; j++) {
	if (potential->next_hop == child->parents[j].next_hop)
	    return;
    }
    
    /* Not present.  Add it if there's room. */
    
    if (child->parent_count < ISIS_MAX_PATHS) {
	child->parents[child->parent_count] = *potential;
	isis_lock_parent(&child->parents[child->parent_count]);
	child->parent_count++;
    } else {
	
	/* No room.  Replace an existing entry if plausible. */
	
	isis_adj_tie_break(pdb, child->parents, potential, level, TRUE);
    }
}

/*
 * isis_calculate_is_next_hops
 *
 * Determine the next hop values for a node being added to TENT.
 */
static void isis_calculate_is_next_hops (clns_pdbtype *pdb,
					 int level,
					 isis_lspdbtype *parent,
					 isis_lspdbtype *child,
					 isis_lsp_parent *leaf_params,
					 ulong netmask)
{
    int i;
    isis_lsp_parent try_this;
    NSAP_T key;
    clns_adjtype *adj, *next;

    /*
     * If the child is adjacent to the root and is not a pseudonode,
     * go to the adjacency database and pick up all adjacencies to
     * the child at the lowest metric.
     *
     * Note that we will lose the fact that there are differing delay/bandwidth
     * via two different links to the same system, since in general the
     * two adjacencies advertised may look the same.
     */
    if (parent->parents[0].next_hop == NULL &&
	!isis_lspid_is_pseudonode(&child->data->lspid)) { /* At root */

	/*
	 * Look for adjacencies with a cost equal to the root_distance.
	 * These are our least-cost adjacencies, assuming that nobody
	 * cheated and used non-zero costs in the pseudonode!
	 */
	station_copy(child->data->lspid.system_id, &key[1]);
	key[0] = STATION_LENGTH;
	adj = find_adjacency(pdb->adjacency_db, key, FALSE, NULL, 0);
	update_nlsp_params(pdb, &try_this, leaf_params, &parent->parents[0],
			   netmask);

	for ( ; adj; adj = next) {
	    next = adj->next;
	    for (; adj; adj = adj->next_alias) {
		isisidbtype *isisidb;
		isisidb = get_isis_idb(pdb, adj->idb);
		if (!isisidb) continue;
		if (station_match (&key[1], &adj->key[1]) &&
		    adj->state == CLNS_ADJ_UP_STATE &&
		    (!TIMER_RUNNING_AND_AWAKE(adj->expiration_time) ||
		     debugger_active()) &&	/* So we can breakpoint! */
		    adj->adjacencyType != CLNS_IS_ADJ_TYPE && adj->idb) {
		    spf_ebuginf(pdb, FALSE, "considering adj to %s (%s) metric %d",
				station_string(child->data->lspid.system_id),
				adj->idb->namestring, isisidb->isis_L1_metric);
		    if (child->root_distance != ((level == ISIS_CIRCUIT_L1) ?
						 isisidb->isis_L1_metric :
						 isisidb->isis_L2_metric))
			continue;
		    spf_ebuginf(pdb, FALSE, "  (accepted)");
		    try_this.next_hop = adj;
		    isis_install_potential_next_hop(pdb, &try_this, child, 
						    level);
		}
	    }
	}
    } else {

	/* Not at root.  Just pick up the parent's next_hop entries. */

	for (i=0; i<parent->parent_count; i++) {
	    update_nlsp_params(pdb, &try_this, leaf_params, 
			       &parent->parents[i], netmask);
	    try_this.next_hop = parent->parents[i].next_hop;
	    isis_install_potential_next_hop(pdb, &try_this, child, level);
	}
    }
}
		

/*
 * isis_calculate_leaf_next_hops
 *
 * Determine the next hop values for a leaf node.
 */
static void isis_calculate_leaf_next_hops (clns_pdbtype *pdb,
					   isis_lspdbtype *paths_lsp,
					   uchar *dest,
					   int level,
					   int metric,
					   isis_lsp_parent next_hops[])
{

    clns_adjtype *adj, *next;
    NSAP_T       key;
    int          i, j;

    for (i=0; i<ISIS_MAX_PATHS; i++)
	next_hops[i].next_hop = NULL;

    /* If we're adjacent to the root, go to the adjacency database. */

    if (!paths_lsp->parents[0].next_hop) {
	station_copy(dest, &key[1]);
	key[0] = STATION_LENGTH;
	adj = find_adjacency(pdb->adjacency_db, key, FALSE, NULL, 0);

	/*
	 * This loop handles the case where there are multiple
	 * adjacencies (on different interfaces) with same system id.
	 * Only the ones with the best cost are chosen as next hops.
	 * All adjacencies with same key will hash to same location, just 
	 * traverse the link list.
	 */
	if (adj) {

	    i = 0;
	    for ( ; adj; adj = next) {
		next = adj->next;
		for (; adj; adj = adj->next_alias) {
		    
		    isisidbtype  *isisidb;
		    isisidb = get_isis_idb(pdb, adj->idb);
		    if (!isisidb)
			continue;
		    
		    /*
                     * Don't use ISO-IGRP adjacency types.
		     */
		    if (adj->protocol_source & ISO_IGRPMASK) 
			continue;
		    
		    /*
		     * Don't store DECNET Phase IV adjacencies in IS-IS 
		     * level-1 routing table.
		     */
		    if (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) 
			continue;

		    if (station_match (&key[1], &adj->key[1]) &&
			adj->state == CLNS_ADJ_UP_STATE &&
			!TIMER_RUNNING_AND_AWAKE(adj->expiration_time) &&
			adj->adjacencyType != CLNS_IS_ADJ_TYPE) {
			if (level == ISIS_CIRCUIT_L1) {
			    if (metric != isisidb->isis_L1_metric) 
				continue;
			} else {
			    if (metric != isisidb->isis_L2_metric) 
				continue;
			}
			for (; i < ISIS_MAX_PATHS; i++) {	
			    if (!next_hops[i].next_hop) {
				next_hops[i].next_hop = adj;
				break;
			    }
			}
		    }
		}
	    }
	    return;
	}
    }

    /*
     * Inherit nodes from last node put on PATHs (paths_lsp).
     */
    for (i = 0; i < paths_lsp->parent_count; i++) {
	if (!paths_lsp->parents[i].next_hop) break;
	for (j = 0; j < ISIS_MAX_PATHS; j++) {
	    if (paths_lsp->parents[i].next_hop == next_hops[j].next_hop) {
		break;
	    } else {
		if (!next_hops[j].next_hop) {
		    next_hops[j].next_hop = paths_lsp->parents[i].next_hop;
		    break;
		} else if (j == (ISIS_MAX_PATHS-1)) {
		    isis_adj_tie_break(pdb, next_hops, &paths_lsp->parents[i],
				       level, FALSE);
		    break;
		}
	    }
	}
    }
}


/*
 * isis_delete_parents
 *
 * Remove all parent entries from this LSP.  We do this centrally and
 * carefully because we mem_lock the adjacencies contained therein.
 * Parent adjacencies are always locked.
 */
void isis_delete_parents (isis_lspdbtype *lsp)
{
    int i;
    isis_lsp_parent *parent;

    for (i=0; i < lsp->parent_count; i++) {
	parent = &lsp->parents[i];
	if (parent->next_hop) {
	    free(parent->next_hop);
	    parent->next_hop = NULL;
	}
    }
    lsp->parent_count = 0;
}

/*
 * isis_initialize_root
 *
 */
static isis_lspdbtype *isis_initialize_root (clns_pdbtype *pdb, int level)
{
    isis_lspdbtype *lsp;
    isis_lspid lspid;

    /*
     * Put this system on TENT. This system is the root of the 
     * SPF tree.
     */
    systemid_copy(pdb->systemId, lspid.system_id);
    lspid.lsp_number = 0;
    pdb->isis_tent_count = 0;

    if (level == ISIS_CIRCUIT_L1) {
	lsp = isis_find_L1_lsp(pdb, &lspid);
    } else {
	lsp = isis_find_L2_lsp(pdb, &lspid);
    }

    /*
     * Put this system (root) on TENT list.
     */
    if (lsp) {
	lsp->parent_count = 1;
	lsp->parents[0].next_hop = NULL;
	lsp->parents[0].throughput = MAXINT;
	lsp->parents[0].delay = 0;
	lsp->parents[0].mtu = MAXINT;
	lsp->parents[0].hops = 0;
	lsp->parents[0].ticks = 0;
	lsp->root_distance = 0;

	lsp->parent_index = 0;		/* No parent */

	lsp->db_on_tent = TRUE;
	pdb->isis_tent[0] = lsp;
	lsp->prev_tent = &pdb->isis_tent[0];
	lsp->next_tent = NULL;

	pdb->isis_tent_count = 1;
    }
    pdb->min_tent = pdb->max_tent = 0;
    return(lsp);
}

/*
 * isis_notice_other_areas
 *
 * If we run ISIS for IP only, we don't maintain a CLNS routing table.
 * However, when we run as a L1L2 router, we need to know if we saw
 * other areas to determine whether we are attached to the backbone.
 */
static void isis_notice_other_areas (clns_pdbtype *pdb, char *area_address,
				     int level)
{
    int i;

    /* Is it really necessary to look for other areas ? */

    if ((level == ISIS_CIRCUIT_L1) || (!pdb->isis_mode & ISIS_IP_MODE) ||
	(pdb->num_L2_routes > 0)) {
	return;
    }

    /* Check this area address agains our own area addresses. */

    for(i = 0; i < pdb->num_active_areas; i++) {
	if (nsap_match(area_address, pdb->active_area_address[i])) {
	    /* This system is in the same area as we are. */
	    return;
	}
    }
    pdb->num_L2_routes = 1;
}

/*
 * isis_nlsp_inject_default
 *
 * Inject an IPX default route based on an attached node.
 */
static void isis_nlsp_inject_default (clns_pdbtype *pdb)

{
    isis_lsp_parent dest_params;
    isis_lspdbtype *lsp;

    dest_params.hops = 1;		/* Dummy up some parameters */
    dest_params.ticks = 1;

    /*
     * For the moment NLSP does L1 only. So we will never see the attached
     * bit set, and we will never need to install a default.
     *
     * %%% Should remove default first. Just like IP does. %%%
     */
    /*
     * If we are not going to install a new default, bail now.
     * Can happen if we haven't found a L1L2 router with ATT bit.
     * Or we are an attached L1L2 router ourselves.
     */
    if (!pdb->isis_nr_best_L2s)
	return;

    if ((!pdb->level == ISIS_CIRCUIT_L1) && isis_is_L2_attached(pdb))
	return;

     /*
      * Install one default route.
      * Once we truely support L2 for NLSP, we should install multiple
      * next-hops to do load balancing. Just like IP does.
      */
    lsp = isis_lspix_to_lsp(pdb, pdb->isis_best_L2s[0]);
    if (!lsp) return;

    reg_invoke_ipx_isis_add_ipx_route(pdb, NOV_RIP_DEFAULT_ROUTE, 
				      NOV_NETMASK_ALL,
				      lsp,
				      lsp,
				      lsp->parents,
				      &dest_params,
				      lsp->root_distance,
				      TRUE);
}

/*
 * isis_nlsp_calculate_ticks
 *
 * Calculate tick count using the awful formulae on page 7-7 of the spec.
 */
static inline ulong isis_nlsp_calculate_ticks (ulong throughput, ulong delay,
					       boolean wan_link)
{
    ulong delay_part, thruput_part;
    
    /*
     * We note that very often the delay and throughput parts of the
     * calculation will turn out to be zero (for LAN links > 82Kbps and
     * < 66 msec delay, and for WAN links > 663Kbps and < 28 msec delay)
     * so we skip the expensive multiplication and division in those
     * cases.
     */
    if (wan_link) {			/* If it's a WAN */
	if (throughput <= (576 * 64 * 18)) {
	    thruput_part = (576 * 64 * 18) / throughput;
	} else {
	    thruput_part = 0;
	}
	
	if (delay >= (1000000 / (2 * 18))) {
	    delay_part = (2 * delay * 18) / 1000000;
	} else {
	    delay_part = 0;
	}

    } else {		/* It's a LAN */

	if (throughput <= (576 * 8 * 18)) {
	    thruput_part = (576 * 8 * 18) / throughput;
	} else {
	    thruput_part = 0;
	}
	
	if (delay >= (1000000 / 18)) {
	    delay_part = (delay * 18) / 1000000;
	} else {
	    delay_part = 0;
	}
    }
    
    return(max(1, delay_part + thruput_part));
}


/*
 * isis_delink_from_tent
 *
 * Delink an LSP from the tent list
 */
void isis_delink_from_tent (clns_pdbtype *pdb, isis_lspdbtype *lsp)
{
    /* Don't bother if we're not on the TENT list. */

    if (!lsp->db_on_tent)
	return;

    /* Complain if we think we are but there is no tent list. */

    if (!pdb->isis_tent) {
	errmsg(&msgsym(TENT, CLNS), pdb->errmsg_name, "no tent list");
	return;
    }

    /* Do the standard doubly-linked list stuff. */

    if (lsp->next_tent) {
	lsp->next_tent->prev_tent = lsp->prev_tent; /* Relink backward */
    }
    *lsp->prev_tent = lsp->next_tent;		    /* Relink forward */
    lsp->next_tent = NULL;
    lsp->prev_tent = NULL;

    /* Drop the flag as well. */

    lsp->db_on_tent = FALSE;

    /* And drop the count as well. */

    pdb->isis_tent_count--;
    if (pdb->isis_tent_count < 0) {
	errmsg(&msgsym(TENT, CLNS), pdb->errmsg_name, "tent count negative");
	pdb->isis_tent_count = 0;
    }
}

/*
 * isis_add_lsp_routes
 *
 * Walk an LSP, adding the leaves to the forwarding table.  If "partial"
 * is FALSE, we're doing a full SPF run, and examine all LSP fragments,
 * and add neighbor LSPs to the tentative list.  If "partial" is TRUE,
 * we're updating only a single LSP fragment, and don't do anything with
 * router neighbors.
 *
 * If "partial" is TRUE, lsp_fragment points at the fragment we're
 * interested in.  If false, lsp_fragment is ignored.
 *
 * Returns the number of links traversed on this pass.
 *
 * Note that we try suspending between fragments.  This may result in
 * our fragment being ripped out from under us.  If this happens, we
 * will bail out and the caller will do any necessary recovery.  (In
 * such cases there is guaranteed to be another SPF coming soon, so
 * as long as we don't leave any dangling pointers or other such rot
 * we should be fine.)
 */
static ulong isis_add_lsp_routes (clns_pdbtype *pdb, int level,
				  isis_lspdbtype *lsp_zero,
				  isis_lspdbtype *lsp_fragment,
				  boolean partial, isis_lspix_type *entry)
{
    isis_lspdbtype   *neighbor_lsp, *current_lsp;
    isis_generic_ad  *ad;
    isis_info_type   *item;
    isis_lspdbtype   **tent_ptr;
    uchar            *dest;
    int              root_distance, distance, ix;
    ulong	     metric;
    ushort	     media;
    isis_lspid       lspid;
    int              format;
    NSAP_T           prefix;
    isis_lsp_parent  next_hops[ISIS_MAX_PATHS], leaf_params;
    ulong            netmask;
    int              i;
    isis_lspix_type  *lspix_entry;
    ulong	     link_count;
    boolean	     fragment_freed;

    /* Walk all items in the LSP. */

    link_count = 0;
    ad = NULL;
    item = NULL;
    if (partial) {
	current_lsp = lsp_fragment;
    } else {
	current_lsp = lsp_zero;
	lsp_fragment = lsp_zero;
    }
    while ((ad = isis_walk_lsp(pdb, &current_lsp, &item, ad, 0, level))) {

	/* Have we come to a new fragment? */

	if (current_lsp != lsp_fragment) {

	    /* New fragment.  Bail if this is only a partial update. */

	    if (partial)
		break;

	    /*
	     * Not a partial.  Lock the new fragment and try to suspend.
	     * If the fragment was freed in the meantime, bail.
	     */
	    mem_lock(current_lsp);
	    if (process_may_suspend()) {
		pdb->suspend_count++;
	    }
	    fragment_freed = current_lsp->lsp_freed;
	    free(current_lsp);
	    if (fragment_freed) {
		break;
	    }
	    lsp_fragment = current_lsp;
	}

	/*
	 * Never consider LSPs with 0 holdtime.
	 */
	if (!current_lsp->lsp_active)
	    continue;

	neighbor_lsp = NULL;
	format = ISIS_NULL_FORMAT;
	prefix[0] = 0;
	leaf_params.delay = 0;
	leaf_params.throughput = MAXINT;
	leaf_params.mtu = MAXINT;
	leaf_params.hops = 0;
	leaf_params.ticks = 0;
	metric = ((isis_metric_type *) item->value)->default_metric;
	netmask = NOV_NETMASK_ALL;

	/*
	 * Examine each entry in the LSP.  Add stubs to the forwarding
	 * table.  Add neighbors to TENT as appropriate.
	 */
	switch (item->code) {

	case ISIS_IS_NEIGHBORS:
	    if (partial)
		continue;
	    format = ISIS_SYSTEMID_FORMAT;
	    dest = ad->is_neighbor.neighbor_id;
	    metric = ad->is_neighbor.metric.default_metric;
	    systemid_copy(dest, lspid.system_id);
	    lspid.lsp_number = 0;
	    if (level == ISIS_CIRCUIT_L1) {
		neighbor_lsp = isis_find_L1_lsp(pdb, &lspid);
	    } else {
		neighbor_lsp = isis_find_L2_lsp(pdb, &lspid);
	    }
	    
	    /*
	     * Don't consider not found LSPs or LSPs with either 0 
	     * holdtime or no LSP number 0 present in database.
	     */
	    if (!neighbor_lsp || !neighbor_lsp->lsp_active || 
		(neighbor_lsp->data->lspid.lsp_number != 0)) {
		continue;
	    }
	    break;
	    
	case ISIS_ES_NEIGHBORS:
	    format = ISIS_STATIONID_FORMAT;
	    dest = ad->es_neighbor;
	    break;

	case ISIS_PREFIX_NEIGHBORS:
	    if (!lsp_zero->root_distance) {
		pdb->redistributed_L2_routes++;
		continue;
	    }
	    format = ISIS_NSAP_FORMAT;
	    dest = ad->prefix_neighbor;
	    prefix_copy(dest, prefix);
	    dest = prefix;
	    break;
	    
	case ISIS_AREA_ADDRESSES:
	    if (partial)
		continue;
	    format = ISIS_NSAP_FORMAT;
	    metric = 0;
	    if (level == ISIS_CIRCUIT_L1) {
		isis_union_areas (pdb, ad->area_address);
		continue;
	    } else {
		dest = ad->area_address;
	    }
	    break;

	case ISIS_IP_INTERNAL_ADDR:
	case ISIS_IP_EXTERNAL_ADDR:
	    if (!lsp_zero->root_distance) continue;
	    format = ISIS_IP_FORMAT;
	    metric = ad->ip_info.metric.default_metric;
	    dest = (uchar *) &ad->ip_info.ip_address;
	    break;

	case ISIS_NLSP_AREA_ADDRS:
	    if (partial)
		continue;
	    format = ISIS_IPX_FORMAT;
	    metric = 0;
	    if (level == ISIS_CIRCUIT_L1) {
		reg_invoke_ipx_isis_union_areas(pdb, 
                                      GETLONG(&ad->ipx_areas.net_number),
				      GETLONG(&ad->ipx_areas.net_mask));
	    }
	    continue;
	    
	case ISIS_NLSP_MGMT_INFO:
	    format = ISIS_IPX_FORMAT;
	    metric = 0;
	    leaf_params.hops = 0;		/* It's within us all */

	    /* Connected networks get 0 ticks, internals get 1. */

	    if (isis_lspid_is_pseudonode(&lsp_zero->data->lspid)) {
		leaf_params.ticks = 0;	/* Connected network */
	    } else {
		leaf_params.ticks = 1;	/* Internal network */
	    }
	    dest = (uchar *) &ad->ipx_mgmt_info.net_number;
	    break;
	    
	case ISIS_NLSP_LINK_INFO:
	    if (partial)
		continue;
	    format = ISIS_SYSTEMID_FORMAT;
	    metric = ad->ipx_link_info.cost;
	    dest = ad->ipx_link_info.neighbor_id;
	    leaf_params.delay = GETLONG(&ad->ipx_link_info.delay);
	    leaf_params.throughput = GETLONG(&ad->ipx_link_info.throughput);
	    media = GETSHORT(&ad->ipx_link_info.media_type);
	    leaf_params.mtu = GETLONG(&ad->ipx_link_info.mtu_size);
	    if (leaf_params.throughput == 0) {
		leaf_params.throughput = MAXINT; /* Really means "infinite" */
		leaf_params.ticks = 0;	/* No tick charge to pseudonode */
	    } else {
		leaf_params.ticks = 
		    isis_nlsp_calculate_ticks(leaf_params.throughput,
					      leaf_params.delay,
					      media & NLSP_TYPE_WAN_MASK);

	    }
	    
	    if (leaf_params.mtu == 0)
		leaf_params.mtu = MAXINT;
	    leaf_params.hops = 0;
	    if (!isis_lspid_is_pseudonode(&lsp_zero->data->lspid))
		leaf_params.hops = 1;

	    systemid_copy(dest, lspid.system_id);
	    lspid.lsp_number = 0;
	    if (level == ISIS_CIRCUIT_L1) {
		neighbor_lsp = isis_find_L1_lsp(pdb, &lspid);
	    } else {
		neighbor_lsp = isis_find_L2_lsp(pdb, &lspid);
	    }
	    
	    /*
	     * Don't consider not found LSPs or LSPs with either 0 
	     * holdtime or no LSP number 0 present in database.
	     */
	    if (!neighbor_lsp || !neighbor_lsp->lsp_active || 
		(neighbor_lsp->data->lspid.lsp_number != 0)) {
		continue;
	    }
	    break;

	case ISIS_NLSP_SVCS_INFO:
	    format = ISIS_IPX_SVC_FORMAT;
	    dest = (uchar *) &ad->ipx_svcs_info; /* %%% Yuck. */
	    leaf_params.hops = ad->ipx_svcs_info.hops;
	    metric = 0;
	    break;

	case ISIS_NLSP_EXT_ROUTES:
	    if (!lsp_zero->root_distance) continue;
	    format = ISIS_IPX_EXT_FORMAT;
	    metric = 0;
	    leaf_params.hops = ad->ipx_ext_rts.hops;
	    leaf_params.ticks = GETSHORT(&ad->ipx_ext_rts.ticks);
	    dest = (uchar *) &ad->ipx_ext_rts.net_number;
	    break;

	case ISIS_NLSP_SUMMARY_ROUTES:
	    if (!lsp_zero->root_distance) continue;
	    format = ISIS_IPX_FORMAT;
	    metric = 0;
	    leaf_params.hops = ad->ipx_sum_rts.area_count;
	    leaf_params.ticks = GETSHORT(&ad->ipx_sum_rts.ticks);
	    dest = (uchar *) &ad->ipx_sum_rts.address;
	    if (ad->ipx_sum_rts.masklen == 0) {
		netmask = 0;
	    } else {
		for (i = 32 - ad->ipx_sum_rts.masklen; i; i--)
		    netmask = netmask << 1;
	    }
	    break;

	default:
	    continue;
	}

	/*
	 * Calculate distance for potential candidate of TENT. It
	 * is the sum of the link metric and the root distance of the
	 * last node put on PATHs.
	 */
	root_distance = lsp_zero->root_distance + metric;

	/*
	 * If metric is "infinite", continue.
	 */
	if (root_distance >= ISIS_MAX_PATH_METRIC) continue;

	/*
	 * Leaf nodes can go in routing table now since the transit
	 * node is on PATHs, if there are other shortest paths to
	 * the leafs, we save state in the routing table.
	 */
	if (!neighbor_lsp) {		/* It's a leaf */
	    if (pdb->proc_type == PROC_TYPE_NLSP) {
		int hop_delta, tick_delta;
		isis_lspdbtype *grandparent_lsp;

		/*
		 * For NLSP, we use the parent's cost with our local
		 * cost additions, unless this is a pseudonode LSP, in
		 * which case we use the grandparent's cost.  This
		 * is done because we should not incur the cost of the
		 * link to the pseudonode for these routes.  We need
		 * to adjust the root distance as well.
		 *
		 * We don't do this funny game for services.
		 *
		 * We do this by figuring out how many extra hops and
		 * ticks were added in the last hop and then subtracting
		 * that amount for each next-hop entry.  There may be
		 * less next-hops for the grandparent than the parent,
		 * but we can use a single delta, since (in theory) all
		 * nodes should have the same cost (that we're subtracting)
		 * to the pseudonode.
		 */
		hop_delta = 0;
		tick_delta = 0;
		grandparent_lsp = isis_lspix_to_lsp(pdb,
						    lsp_zero->parent_index);
		if (isis_lspid_is_pseudonode(&lsp_zero->data->lspid) &&
		    grandparent_lsp && format != ISIS_IPX_SVC_FORMAT) {
		    root_distance -= lsp_zero->root_distance -
			grandparent_lsp->root_distance;
		    hop_delta = lsp_zero->parents[0].hops -
			grandparent_lsp->parents[0].hops;
		    if (hop_delta < 0)	/* Don't be too crazy */
			hop_delta = 0;
		    tick_delta = lsp_zero->parents[0].ticks -
			grandparent_lsp->parents[0].ticks;
		    if (tick_delta < 0)	/* Ditto */
			tick_delta = 0;
		}
		for (ix=0; ix < lsp_zero->parent_count; ix++) {
		    update_nlsp_params(pdb, &next_hops[ix], &leaf_params,
				       &lsp_zero->parents[ix], netmask);
		    if (netmask == NOV_NETMASK_ALL)
			next_hops[ix].hops -= hop_delta;
		    next_hops[ix].ticks -= tick_delta;
		    next_hops[ix].next_hop =
			lsp_zero->parents[ix].next_hop;
		}
	    } else {

		/* For ISIS we root around for ES adjacencies and such. */

		isis_calculate_leaf_next_hops(pdb, lsp_zero, dest, level, 
					      root_distance, next_hops);
	    }
	    if (format == ISIS_IP_FORMAT) {
		if (pdb->ip_idbq.count) {
		    reg_invoke_isis_add_ip_route(pdb, (ipaddrtype *) dest,
						 next_hops, root_distance,
						 level, current_lsp);
		}
	    } else if (format == ISIS_IPX_FORMAT || 
		       format == ISIS_IPX_EXT_FORMAT) {
		if (pdb->isis_mode & ISIS_IPX_MODE) {
		    ulong net_number;

		    /* If net 0, don't bother. */

		    net_number = GETLONG(dest);
		    if (net_number || ZERO_PREFIX(net_number, netmask)) {
			reg_invoke_ipx_isis_add_ipx_route(pdb, net_number, 
							  netmask,
							  lsp_zero,
							  current_lsp,
							  next_hops, 
							  &leaf_params,
							  root_distance,
							  format ==
							  ISIS_IPX_EXT_FORMAT);
		    }
		}
	    } else if (format == ISIS_IPX_SVC_FORMAT) {
		if (pdb->isis_mode & ISIS_IPX_MODE) {
		    reg_invoke_ipx_isis_add_ipx_service(pdb,
							(isis_nlsp_svcs_info *)
							dest, lsp_zero,
							current_lsp,
							next_hops,
							&leaf_params,
							item->length -
						ISIS_NLSP_SVCS_INFO_LEN);
		}
	    } else {
		if (pdb->isis_mode & ISIS_OSI_MODE) {
		    reg_invoke_isis_add_osi_route(pdb, dest, lsp_zero,
						  next_hops, root_distance,
						  level);
		} else {
		    /*
		     * Make sure we can figure out whether to set ATT bit
		     * or not, even when we are not routing CLNS.
		     */
		    if (item->code == ISIS_AREA_ADDRESSES) {
			isis_notice_other_areas(pdb, dest, level);
		    }
		}
	    }
	    continue;
	}

	lspix_entry =
	    isis_lsp_index_to_lspix_entry(pdb, neighbor_lsp->lsp_index);
	if (!lspix_entry || (lspix_entry->lsp != neighbor_lsp)) {
	    errmsg(&msgsym(LSPDB, CLNS), pdb->errmsg_name, neighbor_lsp,
		   neighbor_lsp->lsp_index, lspix_entry,
		   lspix_entry ? lspix_entry->lsp : NULL);
	    break;
	}
	/* 	
	 * Don't consider node if already on PATHs.
	 */
	if (neighbor_lsp->db_on_paths)
	    continue;

	/*
	 * Don't consider node if it is a non-routing server (NLSP).
	 */
	if (lsp_zero->non_routing)
	    continue;

	/*
	 * Don't consider node if it signals it is overloaded.
	 */
	if (lsp_zero->data->hippity) {
	    /*
	     * Make an exception if we are this node ourself, and
	     * we have set the hippity bit by force. In that case
	     * we need to use this LSP, or we won't find any routes.
	     */
	    if (!(pdb->force_hippity &&
		station_match(lsp_zero->data->lspid.system_id,
			      pdb->systemId))) {
		continue;
	    }
	}

	/*
	 * Check if IS has a link to the last node put on PATHs.
	 */
	if (!isis_viable_link(pdb, &lsp_zero->data->lspid, neighbor_lsp, 
			      level)) continue;

	/* Special handling if the LSP is already on TENT. */

	if (neighbor_lsp->db_on_tent) {
	    
	    distance = neighbor_lsp->root_distance;

	    /*
	     * Shorter distance found, don't process this one, go to next 
	     * link.
	     */
	    if (distance < root_distance) continue;
	    
	    /*
	     * Longer distance found, delink from TENT list and replace new
	     * TENT entry in list in root_distance position.
	     */
	    if (distance > root_distance) {
		isis_delink_from_tent(pdb, neighbor_lsp);
		
		spf_ebuginf(pdb, FALSE, "Remove %s from TENT, metric %d",
			    lspid_string(&neighbor_lsp->data->lspid), 
			    distance);
		isis_delete_parents(neighbor_lsp);
		
		/* Note if we removed the highest one. */
		if (pdb->max_tent == distance &&
		    pdb->isis_tent[distance] == NULL) {
		    pdb->max_tent--;
		}
	    }
	}

	/*
	 * If no entry found in TENT, build one and put on TENT. 
	 * Otherwise, use existing equal-cost entry.
  	 */
	if (!neighbor_lsp->db_on_tent) {
	    tent_ptr = &(pdb->isis_tent[root_distance]);
	    neighbor_lsp->next_tent = *tent_ptr; /* Link forward */
	    if (neighbor_lsp->next_tent) {
		neighbor_lsp->next_tent->prev_tent =
		    &(neighbor_lsp->next_tent);	/* Link forward back */
	    }
	    neighbor_lsp->prev_tent = tent_ptr;	/* Link backward */
	    *tent_ptr = neighbor_lsp;	/* Link backward front */
	    pdb->isis_tent_count++;
	    neighbor_lsp->root_distance = root_distance;
	    neighbor_lsp->db_on_tent = TRUE;

	    /* Update the distance range. */

	    if (pdb->isis_tent_count == 1) {
		pdb->min_tent = pdb->max_tent = root_distance;
	    } else {
		if (pdb->min_tent > root_distance)
		    pdb->min_tent = root_distance;
		if (pdb->max_tent < root_distance)
		    pdb->max_tent = root_distance;
	    }
	}

	/*
	 * Add next hops for existing/new entry. 
	 */
	neighbor_lsp->parent_index = lsp_zero->lsp_index;
	isis_calculate_is_next_hops(pdb, level, lsp_zero, neighbor_lsp,
				    &leaf_params, netmask);
	link_count++;

	if (isis_nlsp_spf_event_debug[pdb->proc_type]) {
	    buginf("\n%s-SPF: Add %s to TENT, metric %d", 
		   pdb->errmsg_name, lspid_string(&neighbor_lsp->data->lspid), 
		   root_distance);
	    for (ix = 0; ix < neighbor_lsp->parent_count; ix++) {
		clns_adjtype *next_hop_adj;
		next_hop_adj = neighbor_lsp->parents[ix].next_hop;
		if (next_hop_adj) {
		    buginf("\n%s-SPF:   Next hop %s (%s)", 
			   pdb->errmsg_name, 
			   station_string(&next_hop_adj->key[1]),
			   next_hop_adj->idb->namestring);
		} else {
		    buginf("\n%s-SPF:  Next hop local", pdb->errmsg_name);
		}
	    }
	}
    }
    return(link_count);
}


/*
 * isis_add_to_paths
 *
 * Take shortest distance entry off the TENT list and put on the shortest
 * path tree (PATHS).
 */
static isis_lspdbtype *isis_add_to_paths (clns_pdbtype *pdb, int level)
{

    isis_lspdbtype *lsp = NULL, **tent_ptr;
    int         metric;

    if (pdb->isis_tent_count == 0)
	return(NULL);

    /* Walk each valid metric looking for an entry. */

    for (metric = pdb->min_tent, tent_ptr = &(pdb->isis_tent[pdb->min_tent]); 
	 metric <= pdb->max_tent; 
	 metric++, tent_ptr++) {

	if (*tent_ptr == NULL) /* Nothing at this distance */
	    continue;
	
	pdb->min_tent = metric;		/* Found the first one here */

	/* 
	 * If more than one entry at distance, give preference to
	 * pseudo-nodes.
	 */
	for (lsp = *tent_ptr; lsp; lsp = lsp->next_tent) {
	    if (isis_lspid_is_pseudonode(&lsp->data->lspid))
		break;
	}

	if (!lsp) {			/* Didn't find a pseudonode. */
	    lsp = *tent_ptr;
	}

	/*
	 * Delink from list of same distances.
	 */
	isis_delink_from_tent(pdb, lsp);

	/* If we took the last entry at this distance, update min_tent. */

	if (*tent_ptr == NULL)
	    pdb->min_tent++;

	break;
    }
    
    if (!lsp) {				/* Can't happen, but so what */
	spf_ebuginf(pdb, FALSE, "add_to_paths found no LSP");
	return (NULL);
    }

    lsp->db_on_paths = TRUE;
    spf_ebuginf(pdb, FALSE, "Move %s to PATHS, metric %d",
		lspid_string(&lsp->data->lspid), lsp->root_distance);
    return (lsp);
}

/*
 * isis_age_one_lsp_chain
 *
 * Age one LSP chain.
 *
 */
void isis_age_one_lsp_chain (clns_pdbtype *pdb, ushort index)
{
    isis_lspix_type *entry;
    isis_lsp_linkage **chain_ptr;
    isis_backuptype **backup_ptr;
    int chain_ix;

    /* Call the ager. */

    entry = isis_lsp_index_to_lspix_entry(pdb, index);
    if (!entry)				/* Shouldn't happen */
	return;
    spf_ebuginf(pdb, FALSE, "Aging L%d LSP %d (%s), version %d",
		entry->level, index, entry->lsp ? 
		lspid_string(&entry->lsp->data->lspid) : "deleted",
		entry->lsp_version);
    if (pdb->ipx_pdb) {
	reg_invoke_ipx_nlsp_age_ipx_routes_and_services(pdb, index, entry);
    }
    if (pdb->ip_pdb) {
	reg_invoke_ip_isis_age_routes(pdb, index);
    }

    /* If we're all done with this chain, free the index entry. */

    if (entry->lsp == NULL) {	/* Last time through */
	chain_ptr = entry->chains;
	backup_ptr = entry->backup_chains;
	for (chain_ix = 0; chain_ix < NUM_ISIS_ROUTE_TYPES;
	     chain_ix++, chain_ptr++, backup_ptr++) {
	    if (*chain_ptr) {		/* Better be gone */
		errmsg(&msgsym(LSPCHAIN, CLNS), pdb->errmsg_name, "chain",
		       *chain_ptr);
	    }
	    if (*backup_ptr) {		/* Better be gone */
		errmsg(&msgsym(LSPCHAIN, CLNS), pdb->errmsg_name, "backup",
		       *backup_ptr);
	    }
	}
	entry->lspix_in_use = FALSE;
    }
}

/*
 * isis_age_lsp_chains
 *
 * Age the routes attached to each modified LSP.  This cleans up all of
 * the now-defunct routes.
 *
 * We also free up index entries when they are through.  This is signalled
 * by lspix_in_use being set, but lsp being NULL.  This can happen when
 * an LSP is withdrawn;  we need to keep the index entry alive long enough
 * to flush out all the routes.
 */
static void isis_age_lsp_chains (clns_pdbtype *pdb)
{
    ulong ix;
    isis_lspix_type *entry;

    if (!pdb->lspix_table)
	return;

    entry = &pdb->lspix_table[1];
    for (ix = 1; ix < pdb->lspix_count; ix++, entry++) {
	if (entry->lspix_in_use) {
	    isis_age_one_lsp_chain(pdb, ix);
	    process_may_suspend();
	}
    }
}
/*
 * isis_flush_lsp_backup_chains
 *
 * Flush the backup chains attached to an LSP.
 */
void isis_flush_lsp_backup_chains (clns_pdbtype *pdb, isis_lspix_type *entry)
{
    isis_backuptype **backup_ptr;
    ulong chain_ix;

    backup_ptr = entry->backup_chains;
    for (chain_ix = 0; chain_ix < NUM_ISIS_ROUTE_TYPES;
	 chain_ix++, backup_ptr++) {
	isis_delete_backup_chain(pdb, backup_ptr);
    }
}

/*
 * isis_init_all_lsps
 *
 * Clear all SPF flag bits in all LSPs. Clean out the parent entries.
 * Flush the backup entries.
 */
static void isis_init_all_lsps (clns_pdbtype *pdb, int level)
{
    ulong lspix_count;
    isis_lspix_type *entry;
    isis_lspdbtype *lsp;

    if (!pdb->lspix_table)
	return;

    lspix_count = pdb->lspix_count - 1;	/* We don't use index 0 */
    entry = &pdb->lspix_table[1];
    while (lspix_count--) {
	if (entry->level == level && entry->lspix_in_use) {

	    /* Found an entry.  Mark it unchanged and purge the parents. */

	    entry->lsp_changed = FALSE;
	    lsp = entry->lsp;
	    if (lsp) {
		lsp->db_on_paths = FALSE;
		isis_delete_parents(lsp);
	    }

	    /* Bump the version number. */

	    entry->lsp_version++;

	    /* Flush the backup entry chains. */

	    isis_flush_lsp_backup_chains(pdb, entry);
	}
	entry++;
    }
}

/*
 * isis_compute_spt
 *
 * This function performs the Dijkstra Algorithm on the IS-IS link
 * state database. The function works as follows. This system is the
 * first node put on the shortest path tree (SPT). It is the root.
 * Then all the links (adjacencies) of the root are examined, if there
 * is two-way confirmation of the link, the overload bit is not set, and
 * the metric is not infinite, the node (adjacency) is put on a tentative
 * list (TENT). Once all adjacencies of an LSP are examined, the entry
 * on TENT with the smallest distance is removed and put on the shortest
 * path tree (PATHS). Now the this node's adjacencies are examined and
 * the algorithm iterates. An adjanency is only put on TENT if it is not
 * on PATHS and either it is not on TENT or the distance is better or equal
 * than he current entry on TENT. The algorithm is complete when TENT is
 * empty.
 *
 * As nodes are put on PATHS, any leaf nodes associated with the node are
 * added to the routing table. The transit nodes themselves will not be
 * added to the routing table. For Level-1 routing this is OK because ISs
 * always advertise themselves as ESs. For Level-2, one only cares about
 * leaf nodes (area addresses and prefix routes).
 *
 */
void isis_compute_spt (clns_pdbtype *pdb, int level)
{

    isis_lspdbtype     *lsp;
    int              i;
    boolean          was_attached;
    ulong            paths_count, spt_elapsed_time, spt_start_time;
    isis_lspix_type  *lspix_entry;
    ulong	     link_count;
    isis_spf_log     *log;
    isis_spf_log_entry *log_entry;
    ulong	     log_index;

    /*
     * Determine if we should run the algorithm.
     */
    if (level == ISIS_CIRCUIT_L1) {
	pdb->L1_routes_changed = TRUE;

	/*
	 * Rebuild active area set.  When done with L1 SPT, check if
         * anything changed.  %%% Do this for NLSP areas as well.
	 */
	pdb->old_num_active_areas = pdb->num_active_areas;
	for (i=0; i<pdb->num_active_areas; i++) {
	  nsap_copy(pdb->active_area_address[i], 
		    pdb->old_active_area_address[i]);
	}
	pdb->num_active_areas = pdb->num_areas;
	for (i=0; i<pdb->num_areas; i++) {
	  area_copy(pdb->area_address[i], pdb->active_area_address[i]);
	}
    }

    was_attached = isis_is_L2_attached(pdb);
    if (level == ISIS_CIRCUIT_L2) {
	pdb->num_L2_routes = 0;
	pdb->redistributed_L2_routes = 0;
	if (pdb->proc_type == PROC_TYPE_NLSP)	/* Don't bother for NLSP */
	    return;
    }

    link_count = 0;
    paths_count = 0;
    pdb->suspend_count = 0;
    spt_start_time = runtime();

    /*
     * Print initial debugging output, if enabled.
     */
    spf_ebuginf(pdb, FALSE, "Compute L%d SPT", level);
    if (isis_nlsp_spf_statistics_debug[pdb->proc_type]) {
	buginf("\n%s-Stats: Compute L%d SPT", pdb->errmsg_name, level);
    }

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	reg_invoke_clns_invalidate_cache("isis_compute_spt", FALSE);
    }

    /*
     * Clear closest level-2 router to find new ones.
     */
    if (level == ISIS_CIRCUIT_L1) {
	    pdb->isis_best_L2 = NULL;	/* For CLNS */
	    pdb->isis_nr_best_L2s = 0;	/* For IP and IPX */
	    pdb->isis_best_L2_cost = ISIS_MAX_PATH_METRIC;
    }

    /*
     * Increment route table version number so new entries are
     * identified and old ones are invalidated.
     */
    if (level == ISIS_CIRCUIT_L1) {
	pdb->current_L1_version++;
    } else {
	pdb->current_L2_version++;
    }
    
    /* Allocate memory for the TENT list. */
    
    pdb->isis_tent = malloc_named(sizeof(isis_lspdbtype *) *
				  ISIS_MAX_PATH_METRIC, "ISIS TENT");
    if (pdb->isis_tent == NULL)
	return;
    
    /* Initialize the LSP database. */

    isis_init_all_lsps(pdb, level);

    /* 
     * Put this system on TENT.
     */
    if (!isis_initialize_root(pdb, level)) {
	free(pdb->isis_tent);
	pdb->isis_tent = NULL;
	return;
    }
    
    /* 
     * ---------- Algorithm starts iteration. ----------
     *
     * Ends when TENT is empty. When TENT is empty, all viable nodes 
     * from the link state database have been put on PATHS (SPT).
     */
    do {

	/*
	 * Remove the node from the TENT list with minimum distance
	 * and place on PATHS. The new node on PATHS is the new lsp.
	 */
	lsp = isis_add_to_paths(pdb, level);
	if (!lsp) break;
	lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp->lsp_index);
	if (!lspix_entry || (lspix_entry->lsp != lsp)) { /* Bad news */
	    errmsg(&msgsym(LSPDB, CLNS), pdb->errmsg_name, lsp,
		   lsp->lsp_index, lspix_entry,
		   lspix_entry ? lspix_entry->lsp : NULL);
	    break;
	}
	paths_count++;

	/*
	 * If the node just added is the best (or one of the best) L1L2
	 * routers, note it, so IP/IPX can later point default at it.
	 */
	if ((level == ISIS_CIRCUIT_L1) &&
	    (lsp->data->att & ISIS_LSP_L2_ATTACHED) &&
	    (lsp->root_distance <= pdb->isis_best_L2_cost) &&
	    (pdb->isis_nr_best_L2s < ISIS_MAX_PATHS)) {

	    pdb->isis_best_L2s[pdb->isis_nr_best_L2s] = lsp->lsp_index;
	    pdb->isis_best_L2_cost = lsp->root_distance;
	    pdb->isis_nr_best_L2s++;
	    spf_ebuginf(pdb, FALSE, "A nearest L2 router is %s (%d)", 
			station_string(lsp->data->lspid.system_id),
			lsp->root_distance);
	}

	/* 
	 * Attempt to add each adjacency of this node to TENT. 
	 */
	link_count += isis_add_lsp_routes(pdb, level, lsp, NULL, FALSE,
					  lspix_entry);
	
	/*
	 * Bottom of the SPF loop.  This is a nice time to suspend.  If we
	 * did actually suspend, count it for statistics.  Note that
	 * the database may be ripped out from underneath us while we
	 * are suspended;  if this happens, the TENT list will have been
	 * cleaned up for us, and tent_count will be zero, so we'll
	 * fall out of the loop.  Another SPF will have been scheduled in
	 * that case.
	 */
	if (process_may_suspend()) {
	    pdb->suspend_count++;	/* For statistics. */
	}
    } while (pdb->isis_tent_count);

    if (pdb->isis_tent_count) {
	spf_ebuginf(pdb, FALSE, "tent_count NZ after SPF");
    }

    /* Clean up. */

    free(pdb->isis_tent);
    pdb->isis_tent = NULL;

    isis_traffic[pdb->proc_type][(level == ISIS_CIRCUIT_L1) ? 
	  ISIS_L1_SPF_DONE : ISIS_L2_SPF_DONE]++;

    /*
     * Do IP and IPX related post-Dijkstra processing.
     * In particular this means if the system is level-1-only, (or a L1L2
     * which is not attached to the backbone), insert (or remove) default
     * route(s) in IP or IPX routing table pointing in direction of closest
     * attached level-2 routers.
     *
     * NB. CLNS has already taken care of default routing while adding
     * routes to the CLNS routing table. We don't want to use the LSPs
     * of closest L2 routers, as we need a real ndb, because CLNS uses
     * that ndb for packet forwarding directly.
     */
    if (level == ISIS_CIRCUIT_L1) {
	if (pdb->proc_type == PROC_TYPE_NLSP) {
	    isis_nlsp_inject_default(pdb);
	} else if (pdb->ip_pdb) {
	    reg_invoke_isis_ip_add_default_route(pdb);
	}
    }

    /* Age the route chains on each LSP. */

    isis_age_lsp_chains(pdb);

    /*
     * Print final statistics info, if enabled.
     */
    spt_elapsed_time = runtime() - spt_start_time;
    if (isis_nlsp_spf_statistics_debug[pdb->proc_type]) {
	buginf("\n%s-Stats: Complete L%d SPT, ", pdb->errmsg_name, level);
	buginf("Compute time %d.%03d, %d nodes, %d links on SPT, %d suspends",
	       spt_elapsed_time/ONESEC, spt_elapsed_time%ONESEC, paths_count,
	       link_count, pdb->suspend_count);
    }

    /* Update the SPF log entry, and prepare the next one. */

    if (level == CLNS_LEVEL1)
	log = pdb->L1_spf_log;
    else
	log = pdb->L2_spf_log;
    if (log) {
	log_entry = &log->log_entry[log->log_index];
	GET_TIMESTAMP(log_entry->run_time);
	log_entry->spf_duration = spt_elapsed_time;
	log_entry->node_count = paths_count;
	log_index = log->log_index + 1;
	if (log_index >= ISIS_SPF_LOG_SIZE) {
	    log_index = 0;
	}
	log_entry = &log->log_entry[log_index];
	TIMER_STOP(log_entry->run_time);
	log_entry->trigger_count = 0;
	log_entry->trigger_type = 0;
	log_entry->spf_duration = 0;
	log_entry->node_count = 0;
	bzero(&log_entry->lspid, ISIS_LSPID_LENGTH);
	log->log_index = log_index;
    }

    /*
     * Additional IPX post processing. If we just become DR, inject
     * potential pseudonode. Note that this must follow LSP aging.
     */

    if ((pdb->ipx_pdb != NULL) && (level == ISIS_CIRCUIT_L1)) {
	if (pdb->inject_backups) {
	    if (pdb->ipx_pdb->designated_router_callback) {
		(*pdb->ipx_pdb->designated_router_callback)(TRUE, 
							    pdb->ipx_pdb);
	    }
	    pdb->inject_backups = FALSE;
	}
    }

    /*
     * If we just ran level-2, check if we are still attached to other areas.
     */
    if (level == ISIS_CIRCUIT_L2) {
	if (was_attached != isis_is_L2_attached(pdb)) {
	    if (!pdb->never_set_att_bit) {
		isis_mark_L1_changed(pdb);
		isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_ATTACH, NULL,
				 TRUE);
	    }
	}
    } else {
	/* If level-1 only router. */
	if (pdb->level == ISIS_CIRCUIT_L1) return;

	/*
	 * If we just ran level-1, check if we learned new area addresses and
	 * if so, report in level-2 LSP.
	 */
	if (pdb->num_active_areas != pdb->old_num_active_areas) {
	    isis_mark_L2_changed(pdb);
	    isis_trigger_spf(pdb, CLNS_LEVEL2,
			     ISIS_SPF_TRIG_AREASET, NULL, TRUE);
	} else {
	    for (i=0; i<pdb->num_active_areas; i++) {
		if (!nsap_match(pdb->active_area_address[i], 
				pdb->old_active_area_address[i])) {
		    isis_mark_L2_changed(pdb);
		    isis_trigger_spf(pdb, CLNS_LEVEL2,
				     ISIS_SPF_TRIG_AREASET, NULL, TRUE);
		}
	    }
	}
    }
}

/*
 * isis_compute_some_routes
 *
 * Does a partial route computation when some LSPs change, but not
 * enough to cause us to want to run a full SPF.
 */
void isis_compute_some_routes (clns_pdbtype *pdb)
{
    isis_lspid lspid;
    isis_lspdbtype *lsp, *lsp_zero;
    isis_lspix_type *entry;
    ulong ix;

    /*
     * Walk the LSP array looking for changed LSPs.
     */
    entry = &pdb->lspix_table[1];
    for (ix=1; ix < pdb->lspix_count; ix++, entry++) {
	if (entry->lsp_changed) {

	    /* Found a changed entry.  First sanity check. */

	    entry->lsp_changed = FALSE;

	    lsp = entry->lsp;
	    if (!entry->lspix_in_use)
		continue;

	    /* Bump the version number. */

	    entry->lsp_version++;

	    /* Flush the backup entry chains. */

	    isis_flush_lsp_backup_chains(pdb, entry);

	    if (lsp) {

		/* Now look up fragment zero corresponding to this LSP. */

		if (lsp->data->lspid.lsp_number != 0) { /* Not LSP zero */
		    lspid_copy(&lsp->data->lspid, &lspid);
		    lspid.lsp_number = 0;
		    if (lsp->level == CLNS_LEVEL1)
			lsp_zero = isis_find_L1_lsp(pdb, &lspid);
		    else
			lsp_zero = isis_find_L2_lsp(pdb, &lspid);
		} else {
		    lsp_zero = lsp;
		}
		if (!lsp_zero)
		    continue;
		    
		/* Do this only if the LSP was on the SPF tree. */

		if (lsp_zero->db_on_paths) {
		    
		    /* Looking good.  Now update the routes for this LSP. */
		    
		    spf_ebuginf(pdb, FALSE,
				"Calculating routes for L%d LSP %d (%s)",
				lsp->level, lsp->lsp_index,
				lspid_string(&lsp->data->lspid));
		    isis_add_lsp_routes(pdb, entry->level, lsp_zero, lsp, 
					TRUE, entry);
		    if (entry->level == CLNS_LEVEL1)
			isis_traffic[pdb->proc_type][ISIS_L1_PARTIALS]++;
		    else 
			isis_traffic[pdb->proc_type][ISIS_L2_PARTIALS]++;
		}
	    }

	    /* Age the entry. */

	    isis_age_one_lsp_chain(pdb, ix);
	    process_may_suspend();
	}
    }
}

/*
 * isis_empty_cleanup_queue
 *
 * Empty out the cleanup queue.  Each cleanup queue entry points to a
 * route.  The protocol-specific entry needs to look up the route.  If
 * there are no next-hops there, the route should be deleted.
 */
void isis_empty_cleanup_queue (clns_pdbtype *pdb)
{
    isis_cleanup_queue *cleanup, **prev_cleanup;

    prev_cleanup = &pdb->cleanup_queue;
    while ((cleanup = pdb->cleanup_queue)) {
	switch (cleanup->route_type) {
	  case ISIS_ROUTE_IP:
	    reg_invoke_isis_cleanup_ip_route(pdb, cleanup->cleanup_ip_addr,
					     cleanup->cleanup_ip_mask);
	    break;
	  default:
	    break;
	}
	pdb->cleanup_queue = cleanup->next_cleanup;
	free(cleanup);
    }
}
