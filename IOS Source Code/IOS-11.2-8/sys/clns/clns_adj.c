/* $Id: clns_adj.c,v 3.2.60.5 1996/08/08 14:50:11 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_adj.c,v $
 *------------------------------------------------------------------
 * clns_adj.c - Routines for adding, deleting and accessing adjacencies
 *
 * 14-September-1990 by Paulina Knibbe
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_adj.c,v $
 * Revision 3.2.60.5  1996/08/08  14:50:11  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.2.60.4  1996/05/07  21:06:44  dkatz
 * CSCdi56815:  Removing CLNS static route can cause a crash
 * Branch: California_branch
 * Careful not to dereference non-existent data structures.
 *
 * Revision 3.2.60.3  1996/05/04  20:03:51  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/04/21  20:47:18  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.2.60.1  1996/03/18  19:02:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:12:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:39:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:51:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:51:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/23  22:48:13  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.5  1995/08/25  14:10:22  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.4  1995/08/23 18:02:49  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.3  1995/07/24  23:13:44  widmer
 * CSCdi35486:  CLNS: keyword misparsed as NSAP
 *
 * Revision 2.2  1995/06/21  08:53:40  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:16:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "../xns/novell.h"
#include "../xns/ipx_registry.h"
#include "../clns/clns_registry.h"

/* Global storage */

clns_adjtype **clns_adjacency_db;
clns_adjtype *clns_default_is_adj;


/*
 * adjacency_ager
 *
 *    This routine will run the adjacency database and get rid
 *    of any adjacencies which have expired. It should be run
 *    periodically regardless of what routing protocol we are using.
 *    I think that means that it needs to be run by the esis process.
 *
 */
void adjacency_ager (clns_adjtype **adj_db)
{
    clns_adjtype *adj, *next, *next_alias;
    int          bucket;

    if (!adj_db)
	crashdump(0);

    for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
	for (adj = adj_db[bucket]; adj; adj = next) {
	    next = adj->next;
	    for ( ; adj; adj = next_alias) {
		next_alias = adj->next_alias;
		if (adj->protocol_source & ISO_STATIC) continue;
		if (adj->protocol_source & DECNET_DISCARD) continue;
		if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time)) {
		    adj_ebuginf("\nCLNS-Adj: Aging Adjacency %s",
				station_string((uchar *)&adj->key[1]));
		    if (!delete_adjacency(adj_db, adj)) {
			adj_ebuginf("\nCLNS-Adj: Adjacency %n deletion failed",
				    adj->key);
		    }
		}
	    }
	}
    }
}

/*
 * link_adjacency
 *
 * Insert the adjacency into the database.
 */
static void link_adjacency (clns_adjtype **adj_db, int bucket,
			    clns_adjtype *adjacency)
{
    if (!adj_db)
	crashdump(0);

    adjacency->next = adj_db[bucket];
    adj_db[bucket] = adjacency;
    return;
}

/*
 * add_adjacency
 *
 *    add or refresh the specified adjacencies. Note that MANY fields
 *    of the adjacency are left for the caller to fill in.
 *    NOTE that this does not check for the case where the snpa
 *    for an adjacency has changed. ???
 *
 *    A holding time of -1 means "infinity".  This results in the
 *    adjacency timer not running.  A holding time of 0 is legal,
 *    and means that the adjacency has already expired (a waste, but
 *    we gotta do it).
 *
 *    %%% This will find CLNS adjacencies when adding NLSP adjacencies.
 *
 */
clns_adjtype *add_adjacency (clns_adjtype **adj_db, NSAP_T key,
			     idbtype *idb, hwaddrtype *snpa, long hold_time,
			     boolean multiple_snpas)
{
    clns_adjtype *adjacency, *new_adj;

    if (!adj_db)
	crashdump(0);

    adjacency = find_adjacency(adj_db, key, FALSE, idb, 0);

    if (!adjacency || multiple_snpas) {
	new_adj = malloc_named(sizeof(clns_adjtype), "CLNS Adjacency");
	if (!new_adj) {
	    return(NULL);
	}
	/*
	 * Do basic initialization of the ISIS managed timer, so if this
	 * doesn't turn out to be an ISIS timer, the mgd_timer_stop in
	 * delete_adjacency won't hurt anybody.
	 */
	mgd_timer_init_leaf(&new_adj->isis_expiration_timer, NULL, 0, NULL,
			    FALSE);
	nsap_copy(key, new_adj->key);
	new_adj->idb = idb;
	snpa_copy(snpa, &new_adj->snpa);

	/*
	 * If cluster aliasing is enabled on this interface and there
	 * is an existing adjacency with the same system id, link along
	 * next_alias linked list.
	 */
	if (adjacency && multiple_snpas) {
	    new_adj->next_alias = adjacency->next_alias;
	    adjacency->next_alias = new_adj;
	    reg_invoke_clns_invalidate_cache("add_adjacency", FALSE);
	} else {
	    link_adjacency(adj_db, 
			   nsap_hash(new_adj->key[0], &new_adj->key[1]), 
			   new_adj);
	}
	adjacency = new_adj;
	GET_TIMESTAMP(adjacency->create_time);
    }
    
    if (hold_time == -1) {
	TIMER_STOP(adjacency->expiration_time);
    } else {
	TIMER_START(adjacency->expiration_time, hold_time * ONESEC);
    }

    return(adjacency);
}

/*
 *  compare_stationid
 *
 *     This routine is passed two 6 digit station ids.
 *     It returns -1 if the first id is less than the second.
 *                0                     equal to
 *                +1                    greater than
 */
int compare_stationid (uchar id1[STATION_LENGTH], uchar id2[STATION_LENGTH])
{
    int i;

    for (i = 0; i < STATION_LENGTH; i++) {
	if (id1[i] < id2[i])
	    return(-1);
	if (id1[i] > id2[i])
	    return(+1);
    }
    return(0);
}

/*
 * compare_area_address
 *
 */
int compare_area_address (uchar *area1, uchar *area2)
{
    int i, len, len1, len2;

    len1 = *area1++;
    len2 = *area2++;

    len = min(len1, len2);
    for (i = 0; i < len; i++, area1++, area2++) {
	if (*area1 < *area2)
	    return(-1);
	if (*area1 > *area2)
	    return(1);
    }
    if (len1 == len2)
	return(0);
    return(len1 - len2);
}

/*
 * delete_adjacency_alias
 *
 */
static void delete_adjacency_alias (clns_adjtype *head,
				    clns_adjtype *adj_to_delete)
{
    clns_adjtype *prev, *adj;
    
    prev = head;
    for (adj = head->next_alias; adj; prev = adj, adj = adj->next_alias)  {
	if (adj == adj_to_delete) {
	    prev->next_alias = adj->next_alias;
	    adj_ebuginf (
		 "\nCLNS-Adj: Deleting cluster alias adjacency %s, %s (%s)",
			 station_string(&adj->key[1]), snpa_string(&adj->snpa),
			 adj->idb ? adj->idb->namestring : NULL);
	    adj->next_alias = NULL;
	    mgd_timer_stop(&adj->isis_expiration_timer); /* We're paranoid */
	    reg_invoke_clns_invalidate_cache("delete_adjacency_alias", TRUE);
	    free(adj);
	    return;
	}
    }
}

/*
 * delete_adjacency
 *
 *    Remove the adjacency from the database
 */
boolean delete_adjacency (clns_adjtype **adj_db, clns_adjtype *adj)
{
    clns_adjtype *prev, *find_one;
    clns_pdbtype *pdb;
    int          bucket;
    int		 old_protocol_source = adj->protocol_source;
    nidbtype     *nidb;

    if (!adj_db)
	crashdump(0);

    /* If this is the default IS adjacency, clear it out. */

    if (clns_default_is_adj == adj)
	clns_default_is_adj = NULL;

    bucket = nsap_hash(adj->key[0], &adj->key[1]);
    prev = NULL;

    find_one = adj_db[bucket];
    for ( ; find_one; prev = find_one, find_one = find_one->next) {
	pdb = NULL;
	if (adj != find_one) {
	    if (find_one->next_alias) {
		delete_adjacency_alias(find_one, adj);
	    }
	    continue;
	}

	adj_ebuginf("\nCLNS-Adj: Deleting adjacency %s (%s)",
		    station_string(&adj->key[1]),
		    adj->idb ? adj->idb->namestring : NULL);
	mgd_timer_stop(&adj->isis_expiration_timer);

	/*
	 * If adjacency created by ISO-IGRP, call it to delete routes.
	 */
	if (adj->protocol_source & ISO_IGRPMASK) {
	    reg_invoke_clns_delete_igrp_adj(pdb, adj);
	}

 	/*
 	 * IS-IS and static routes
 	 */
 	if (adj->protocol_source & (ISO_NLSPMASK | ISO_ISISMASK | 
				    ISO_STATIC | ISO_ESIS | 
				    DECNET_DISCARD | DECNET_PHASEIV)) {
	    if (adj->protocol_source & ISO_NLSPMASK) {
		nidb = IPX_PRIMARY_NIDB(adj->idb);
		pdb = reg_invoke_ipx_find_nlsp_pdb(nidb->pname);
	    } else {
		pdb = find_isis_pdb();
	    }
 	    if (pdb) {
		isisidbtype *isisidb;
		isisidb = get_isis_idb(pdb, adj->idb);

 		/* 
 		 * Decrement the adjacency count for this interface.
  		 */
 		if (adj->state == CLNS_ADJ_UP_STATE)
 		    isis_update_adj_count(pdb, adj, FALSE);
 		
  		/*
 		 * Clear out the circuit id from the serial idb.
  		 */
 		if (is_p2p(adj->idb)) {
		    isis_reset_circuit_id(pdb, adj->idb);

		    /*
		     * Note that the interface is down.  For NLSP, mark the
		     * pseudonode LSP for rebuilding (only NLSP has WAN
		     * pseudonodes).
		     */
		    if (isisidb) {
			isisidb->isis_if_state = CLNS_ADJ_FAILED_STATE;
			if (pdb->proc_type == PROC_TYPE_NLSP) {
			    isis_mark_L1_dr_changed(pdb, isisidb);
			}
		    }
 		    switch (adj->adjacencyType) {
 		    case CLNS_ES_ADJ_TYPE:
 		    case CLNS_L1_IS_ADJ_TYPE:
		    case CLNS_NLSP_IS_ADJ_TYPE:
 			isis_mark_L1_changed(pdb);
 			break;

 		    case CLNS_L2_IS_ADJ_TYPE:
 			isis_mark_L2_changed(pdb);
 			if (!adj->L2only) isis_mark_L1_changed(pdb);
 			break;
 			
 		    case CLNS_PREFIX_ADJ_TYPE:
 			isis_mark_L2_changed(pdb);
 			break;
 		    }
 		} else {
 		    
 		    /*
 		     * If this adjacency was DR, set flag to elect new DR, and 
 		     * delete reference to it in LSP.
 		     */
		    if (isisidb) {
			if (isis_system_is_dr(pdb, &adj->key[1], isisidb,
					      ISIS_CIRCUIT_L1)) {
			    isis_trigger_dr_election(pdb, isisidb,
						     ISIS_CIRCUIT_L1);
			    isis_mark_L1_changed_delayed(pdb);
			} 
			if (isis_system_is_dr(pdb, &adj->key[1], isisidb,
					      ISIS_CIRCUIT_L2)) {
			    isis_trigger_dr_election(pdb, isisidb,
						     ISIS_CIRCUIT_L2);
			    isis_mark_L2_changed_delayed(pdb);
			}
 		    
			/*
			 * If we are the DR, remove adjacency from the 
			 * pseudo-node LSP.
			 */
			if (isis_we_are_dr(pdb, adj->idb, CLNS_LEVEL1)) {
			    isis_mark_L1_dr_changed(pdb, isisidb);
			    isis_mark_L1_changed(pdb);
			} 
			if (isis_we_are_dr(pdb, adj->idb, CLNS_LEVEL2)) {
			    isis_mark_L2_dr_changed(pdb, isisidb);
			    isis_mark_L2_changed(pdb);
			}
			if (adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE) {
			    isis_mark_L2_changed(pdb);
			}
		    }
  		}
 		adj->protocol_source &= ~pdb->typeflag;
  	    }
  	}

	/*
	 * An ES-IS or IS-IS adjacency went down. There might be a static
	 * route pointing to it.
	 */
	if (((old_protocol_source & ISO_ESIS) && 
	    adj->adjacencyType == CLNS_IS_ADJ_TYPE) ||
	    (old_protocol_source & ISO_ISISMASK)){
	    reg_invoke_clns_adjust_static_routes(adj, adj->idb, FALSE, TRUE);
	}

	/*
	 * If this adjacency is both statically configured and learned
	 * via a routing protocol, don't free memory.
	 */
	if ((adj->protocol_source & ISO_STATIC) &&
	    old_protocol_source != adj->protocol_source)
	    return(TRUE);

	/*
	 * If there are aliases associated with the adjacency, have the
	 * next alias point to the next element in the linked list.
	 */
	if (adj->next_alias) {
	    adj->next_alias->next = adj->next;
	    if (prev) {
		prev->next = adj->next_alias;
	    } else {
		adj_db[bucket] = adj->next_alias;
	    }
	} else {
	    if (prev) {
		prev->next = adj->next;
	    } else {
		adj_db[bucket] = adj->next;
	    }
	}

	if (!pdb || pdb->proc_type == PROC_TYPE_ISIS) {
	    reg_invoke_clns_invalidate_cache("delete_adjacency", TRUE);
	}
	adj->next = NULL;
	free(adj);
	return(TRUE);
    }

    return(FALSE);
}

/*
 * next_neighbor
 *
 * Scan the adjacency database for all (or a selected set of) adjacencies
 *
 * If neighbor is NULL, start from the beginning
 * If idb is non-null, match on IDB
 * If adj_type is non-zero, match on adjacency type (as a bitmask)
 *
 */
clns_adjtype *next_neighbor (clns_adjtype **adj_db, idbtype *idb,
			     int adj_type, clns_adjtype *neighbor, int *bucket)
{
    int index;

    if (!adj_db)
	crashdump(0);

    index = *bucket;

    if (neighbor) {
	neighbor = neighbor->next;
    } else {
	index = -1;			/* Mildly scary */
    }

    for (;;) {

	/* If the current has line has run out, use the next one. */

	if (!neighbor) {
	    index++;
	    if (index == CLNS_ADJ_DB_SIZE)
		break;
	    neighbor = adj_db[index];
	    continue;			/* Back to the top */
	}

	if ((idb == NULL) || (idb == neighbor->idb)) { /* Potential match */
	    if ((adj_type == 0 || (adj_type & neighbor->adjacencyType))) {
		break;			/* Got it */
	    }
	}
	neighbor = neighbor->next;
    }

    *bucket = index;
    return(neighbor);
}

/*
 * find_adjacency
 *
 * Search for adjacency entry in the CLNS adjacency database. The 
 * supplied key can be in station format or nsap format. In most cases
 * adjacencies are stored in 6-byte station id format. The nsap format
 * is used when a route uses a next hop as an NSAP address (static and
 * ISO-IGRP routes.
 *
 * If idb is non-NULL, the idb of the adjacency is compared with the one
 * in the entry. This is also true for level, when it is not 0.
 */
clns_adjtype *find_adjacency (clns_adjtype **adj_db, NSAP_T  key,
			      boolean extract_stationid, idbtype *idb,
			      int type)
{
    clns_adjtype *adj;
    int          bucket, i;

    if (!adj_db)
	crashdump(0);

    /*
     * Check to see if the station id should be extracted from the
     * NSAP formatted address.
     */
    if (extract_stationid) {
	bucket = nsap_hash(STATION_LENGTH, get_station(key));
    } else {
	bucket = nsap_hash(key[0], &key[1]);
    }
    
    for (adj = adj_db[bucket]; adj; adj = adj->next) {
	
	/* 
	 * If requested key is in station id format (6 bytes).
	 */
	if (!extract_stationid) {
	    if (!nsap_equ(key, adj->key))
		continue;
	} else {
	    if (!station_match(get_station(key), &adj->key[1]))
		continue;
	    
	    /*
	     * Now match areas.
	     */
	    for (i = 0; i < adj->num_areas; i++) {
		if (nsap_match(adj->area_address[i], key))
		    break;
	    }
	    if (i == adj->num_areas)
		continue;
	}

	/*
	 * Found adjacency, check if idb and type should be compared.
	 */
	if (idb) {
	    if (idb != adj->idb)
		continue;
	    if (type && (!(type & adj->adjacencyType)))
		continue;
	    break;
	} else {
	    break;
	}
    }

    return(adj);
}


/*
 * area_in_adj
 *
 *    This routine returns true if the specified area is already in
 *    this adjacency
 */
boolean area_in_adj (NSAP_T area_address, clns_adjtype *adj)
{
    int i;

    for (i = 0; i < adj->num_areas; i++) {
	if (nsap_equ(adj->area_address[i], area_address)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}


/*
 * add_area_to_adj
 *
 *   Add the specified area address to the adjacency.
 *   LATER: should drop the 'correct' area address if there are too many
 */
void add_area_to_adj(NSAP_T area_address, clns_adjtype *adj)
{
    if (adj->num_areas < MAX_AREA_ADDRESSES) {
	nsap_copy(area_address, adj->area_address[adj->num_areas++]);
    } else {
	adj_ebuginf("\nCLNS-Adj: too many area addresses for adjacency");
    }
}

/*
 * delete_interface_adjacencies
 *
 * Delete all adjacencies on this interface
 */
void delete_interface_adjacencies (clns_adjtype **adj_db, idbtype *idb)
{
    int i;
    clns_adjtype *adj, *next, *next_alias;

    if (!adj_db)
	crashdump(0);

    for (i = 0; i < CLNS_ADJ_DB_SIZE; i++) {
	for (adj = adj_db[i]; adj; adj = next) {
	    next = adj->next;
	    for (; adj; adj = next_alias) {
		next_alias = adj->next_alias;
		if (adj->idb != idb) continue;
		if (adj->protocol_source & ISO_STATIC) continue;
		if (adj->protocol_source & DECNET_DISCARD) continue;
		delete_adjacency(adj_db, adj);
	    }
	}
    }
}

/*
 * clns_create_adjacency_db
 *
 * Create an adjacency database.
 *
 */
clns_adjtype **clns_create_adjacency_db (const char *name)
{
    clns_adjtype **db;

    /* Straightforward--malloc the proper memory hunk. */

    db = malloc_named(sizeof(clns_adjtype *) * CLNS_ADJ_DB_SIZE, name);
    return(db);
}

/*
 * clns_mark_default_is_adj
 *
 * Mark an adjacency to be the default IS adjacency (in the case where we
 * are an ES).
 */
void clns_mark_default_is_adj (clns_adjtype *adj)
{
    clns_default_is_adj = adj;
}

/*
 * clns_get_default_is_adj
 *
 * Return a pointer to the default IS adjacency, if any (for use when we are
 * an ES).
 *
 * Returns NULL if there is none.
 */
clns_adjtype *clns_get_default_is_adj (void)
{
    return(clns_default_is_adj);
}
