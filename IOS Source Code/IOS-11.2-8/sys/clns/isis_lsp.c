/* $Id: isis_lsp.c,v 3.5.28.3 1996/08/28 12:38:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_lsp.c,v $
 *------------------------------------------------------------------
 * IS-IS routines for building LSPs
 *
 * June 1994, Dave Katz
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isis_lsp.c,v $
 * Revision 3.5.28.3  1996/08/28  12:38:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.28.2  1996/08/02  02:47:54  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.5.28.1  1996/03/18  19:04:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.4.1  1996/03/07  08:41:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/13  17:19:01  dkatz
 * CSCdi46860:  ISIS sets attached bit in too many LSPs
 * Be delicate.
 *
 * Revision 3.4  1995/12/01  17:48:52  dkatz
 * CSCdi44093:  ISIS crashes on purpose
 *
 * Revision 3.3  1995/12/01  02:48:32  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.2  1995/11/17  08:54:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/23  22:48:40  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.4  1995/09/23  22:13:51  dkatz
 * CSCdi39388:  ISIS validation too harsh
 * Crank out an error message with all available info and try not to crash.
 *
 * Revision 2.3  1995/09/23  21:37:48  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.2  1995/06/21  08:53:58  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:18:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "clns.h"
#include "clns_route.h"
#include "isis.h"
#include "clns_externs.h"
#include "isis_externs.h"
#include "isis_inlines.h"
#include "exception.h"
#include "../os/chunk.h"

/* Forward references */

static boolean isis_overloaded = FALSE;  /* %%% */

/*
 * This module contains routines to build and manipulate LSPs.
 *
 * The LSP building process is started by creating an initial LSP
 * fragment, and a "context block" which is used to carry the current
 * state of the construction:
 *
 *   context = isis_create_lsp(...)
 * 
 * Once this is done, options can be added.  An option entry is
 * initially created by the following:
 *
 *   item = isis_new_item(...)
 *
 * This call will initialize an item header and set up housekeeping
 * pointers.  Note that it sets the item to be zero length, so if
 * for some reason nothing is added to it, the item header will be
 * discarded when the LSP is completed.
 *
 * Once an item header has been created, there are basically two ways
 * to fill in an item.  In the first method, the caller keeps track
 * of the amount of space left in the LSP, and adds data to the
 * end of the item (isis_new_item returns the amount of space left,
 * and a pointer to where to add the data).  This is often used when
 * adding monolithic items to LSP0;  the out-of-space case is usually
 * fatal.  When the item is completed, the following call is made
 * to tie off the item:
 *
 *   isis_item_done(...)
 *
 * This updates the length field in the item based on the updated
 * data pointer maintained by the caller.  Note that this should
 * *not* be called if nothing was added to the item, because doing
 * so will add the null item header.
 *
 * Some callers are savvy to item headers, such as those that build
 * items that are used in both LSPs and IIHs;  the method is
 * essentially the same as above, except that the caller uses the
 * item pointer instead of the data pointer to build the item.
 *
 * The other method is used when building long lists of entries that
 * may span multiple items or multiple LSPs.  In this case, items
 * are added by calling:
 *
 *   entry = isis_add_item_entry(...)
 *
 * and terminated by calling:
 *
 *   isis_item_done(context, NULL)
 *
 * In this method, new items and/or new LSPs are automatically created
 * as necessary and the LSPs are installed in the database as they
 * fill up.
 *
 * In either case, when the LSP build process is complete, the final
 * LSP fragment should be installed by calling:
 *
 *   isis_lsp_done(context)
 *
 * and the context block disposed of by calling:
 *
 *   isis_destroy_context(context);
 */

/*
 * isis_free_lsp
 *
 * Free an LSP.
 *
 * Does some necessary cleanup first.
 */
void isis_free_lsp (clns_pdbtype *pdb, isis_lspdbtype *lsp)
{
    if (lsp == NULL)
	return;

    isis_delete_parents(lsp);		/* Maybe unlock any adjacencies! */
    isis_delink_from_tent(pdb, lsp);	/* Just in case. */
    mgd_timer_stop(&lsp->gen_limit_timer);
    mgd_timer_stop(&lsp->lsp_timer);
    if (lsp->lsp_refresh_timer) {
	mgd_timer_stop(lsp->lsp_refresh_timer);
	free(lsp->lsp_refresh_timer);
	lsp->lsp_refresh_timer = NULL;
    }
    bitfield_destroy(&lsp->SRM);
    bitfield_destroy(&lsp->SSN);
    bitfield_destroy(&lsp->on_rexmit_queue);
    lsp->lsp_freed = TRUE;		/* In case SPF has locked it. */
    free(lsp);
}

/*
 * isis_malloc_lspdb
 *
 * Allocate an LSP database entry.  Returns the address of the entry or NULL.
 *
 * The size includes the LSP database header (isis_lspdbtype).
 */
isis_lspdbtype *isis_malloc_lspdb(clns_pdbtype *pdb, ulong size)
{
    isis_lspdbtype *lsp_db;
    lsp_db = malloc_named(size, "ISIS LSP");
    if (lsp_db) {
	lsp_db->max_db_length = size;
	mgd_timer_init_leaf(&lsp_db->gen_limit_timer, &pdb->update_timer,
			    ISIS_LSP_GEN_TIMER, lsp_db, FALSE);
    }
    return(lsp_db);
}

/*
 * isis_new_item
 *
 * Return a pointer to the next item space in the LSP.
 *
 * Also returns the remaining data space in the item and the start
 * of the data area.
 *
 * "bytes_left" says how much space is available for item data (the
 * item header has already been subtracted).
 *
 * "size" is the size of the item data that will be used;  if unknown,
 * a value of 0 should be passed, and it is up to the caller to deal
 * with spilling over.
 *
 * "spill" is TRUE if we should advance to the next LSP if there is
 * not enough room to add this item header to the end of the LSP.
 *
 * Returns NULL if there is insufficient space to add a new item,
 * or if we ran out of memory.
 */
isis_info_type *isis_new_item (isis_lspcontext *context,
			       int item_type,
			       int size,
			       void **data_start,
			       int *bytes_left,
			       boolean spill)
{
    isis_info_type *item;
    int local_bytes_left;
    isis_lspdbtype *lsp_db;

    /*
     * If there is insufficient room to add the item header, either
     * give up (if "spill" is FALSE) or move on to the next LSP
     * (if "spill" is TRUE).
     */
    while (TRUE) {
	lsp_db = context->lsp;
	if (lsp_db == NULL)
	    return(NULL);

	item = (isis_info_type *) context->lsp_end;
	local_bytes_left = context->usable_length -
	    ((uchar *)item - (uchar *) lsp_db) - ISIS_INFO_TYPE_OVERHEAD;
	
	if (local_bytes_left >= size)
	    break;
	if (!spill)
	    return(NULL);

	isis_lsp_done(context);
	if (!isis_next_lsp(context, FALSE))
	    return(NULL);		/* Out of memory */
    }

    context->lsp_item = item;

    item->length = size;

    item->code = item_type;
    context->item_type = item_type;
    context->lsp_end = item->value;	/* Point at beginning of data */

    if (bytes_left != NULL) {
	*bytes_left = min(local_bytes_left, ISIS_INFO_TYPE_MAXDATA);
    }

    if (data_start != NULL) {
	*data_start = ((uchar *) item) + ISIS_INFO_TYPE_OVERHEAD;
    }
    return(item);

}

/*
 * isis_item_done
 *
 * Update the context pointers when an item has been built, given the
 * new data end pointer.
 *
 * If the new end pointer is equal to the existing item pointer, it
 * means that the item has been abandoned.  If the item length is 0,
 * we consider the item to be abandoned, and push the end pointer
 * back.  This means that we do not support zero-length items, 
 * of course, but so far these have not been defined.
 *
 * If the end pointer is NULL, it means that the end pointer is
 * already up-to-date (due to calling isis_add_item_entry).
 */
void isis_item_done (isis_lspcontext *context, void *new_end)
{
    isis_info_type *item;

    if (context->lsp == NULL)
	return;

    item = context->lsp_item;
    if (new_end != NULL) {
	context->lsp_end = new_end;
    } else {
	new_end = context->lsp_end;
    }

    /* Update the length, except for abandoned items. */
   
    if (item != new_end) {
	uchar local_length;
	local_length = isis_item_length(item, new_end);
	item->length = local_length;

	/* If the item is NULL, back up the header. */

	if (local_length == 0)
	    context->lsp_end = (uchar *) item;
    }
}

/*
 * isis_next_lsp
 *
 * Allocate the next lsp fragment and update the context block.
 *
 * Returns FALSE if we couldn't complete it, or TRUE otherwise.
 *
 * We detect an overrun of the LSP space here, and return NULL if
 * we have a full fragment load.
 *
 * We will leave context->lsp NULL if we failed (it should have been NULL
 * upon entry to this routine).
 */
boolean isis_next_lsp (isis_lspcontext *context, boolean first_time)
{
    isis_lspdbtype *lsp;
    isis_lsp_core *lsp_core;
    isis_lspdbtype *old_lsp;
    int seq_num;
    isis_info_type *item;
    int bytes_left;
    clns_pdbtype *pdb;

    pdb = context->pdb;

    /* Check to see if we're at the limit. */

    if (context->lspid.lsp_number == ISIS_MAX_LSP_FRAGMENT) {
	errmsg(&msgsym(LSPFULL, CLNS), pdb->errmsg_name, context->level);
	return(FALSE);
    }

    /* Allocate the LSP. */

    lsp = isis_malloc_lspdb(pdb, pdb->packet_mtu + ISIS_LSPDB_OVERHEAD);
    context->lsp = lsp;
    if (lsp == NULL) {		/* No memory */
	return(FALSE);
    }
    context->usable_length = lsp->max_db_length; /* Use the whole thing. */
    context->lsp_item = NULL;
    mgd_timer_init_leaf(&lsp->lsp_timer, &pdb->lsp_timers, ISIS_LSP_TIMER,
			lsp, FALSE);
    mgd_timer_start(&lsp->lsp_timer, pdb->lsp_lifetime);
    lsp->lsp_active = TRUE;
    lsp->level = context->level;

    /* Update the context block.  Bump the LSP number. */

    lsp_core = lsp->data;
    context->lsp_end = lsp_core->item;
    if (!first_time)
	context->lspid.lsp_number++;
    lspid_copy(&context->lspid, &lsp_core->lspid);

    /* Determine the LSP sequence number. */

    if (context->level == ISIS_CIRCUIT_L1) {
	old_lsp = isis_find_L1_lsp(pdb, &lsp_core->lspid);
    } else {
	old_lsp = isis_find_L2_lsp(pdb, &lsp_core->lspid);
    }
    if (old_lsp != NULL) {
	seq_num = GETLONG(&old_lsp->data->sequence_number) + 1;
    } else {
	seq_num = 1;
    }
    PUTLONG(&lsp_core->sequence_number, seq_num);

    /*
     * Set IS type field to value from PDB. 
     */
    lsp_core->is_type = (pdb->level == ISIS_CIRCUIT_L2) ? 
	ISIS_CIRCUIT_L1L2 : pdb->level;

    /*
     * For now we don't support partition repair.
     */
    lsp_core->p = FALSE;

    /*
     * For now we support only the default metric. Set attached flag if
     * there is any other reachable area and this is fragment zero of a
     * non-pseudonode level-1 LSP.
     */
    if (context->level == ISIS_CIRCUIT_L1 &&
	!context->lspid.lsp_number && !context->lspid.pseudo_id &&
	!context->pdb->force_hippity && !isis_overloaded &&
	!pdb->never_set_att_bit && isis_is_L2_attached(pdb)) {
	lsp_core->att = ISIS_LSP_L2_ATTACHED;
    } else {
	lsp_core->att = 0;
    }
   
    /*
     * Hippity - set to one if this router is in an unreliable state.
     * Typically this means it has run out of memory at an inopportune time.
     * No traffic should be routed through this router until its
     * hippity cost goes to zero.
     * Currently we will set the hippity bit only when the user explicitly
     * wants this to happen (so he can use the router as a flooding server).
     * We should never set the hippity bit on pseudonode LSPs. The proper
     * thing to do would probably be to lower our priority so we won't become
     * DIS when we are in trouble. %%%
     */
    if (context->lspid.pseudo_id || context->lspid.lsp_number) {
	lsp_core->hippity = 0;
    } else {
	lsp_core->hippity = (context->pdb->force_hippity || isis_overloaded);
    }

    /* Add authentication, if appropriate. */

    item = isis_new_item(context, ISIS_AUTHENTICATION, 0, NULL, 
			 &bytes_left, FALSE);
    if (item == NULL)
	return(FALSE);
    item = isis_insert_auth(item, pdb, NULL, context->level, 
			    bytes_left);
    isis_item_done(context, item);

    return(TRUE);
}

/*
 * isis_create_specific_lsp
 *
 * Allocate a specific lsp fragment and update the context block.
 *
 * Returns FALSE if we couldn't complete it, or TRUE otherwise.
 *
 * We detect an overrun of the LSP space here, and return NULL if
 * we have a full fragment load.
 *
 * We will leave context->lsp NULL if we failed (it should have been NULL
 * upon entry to this routine).
 */
boolean isis_create_specific_lsp (isis_lspcontext *context, uchar lsp_number)
{
    if (context->lsp != NULL)
	return(FALSE);

    context->lspid.lsp_number = lsp_number;
    return(isis_next_lsp(context, TRUE));
}

/*
 * isis_create_lsp
 *
 * Create an LSP context block and LSP 0.
 *
 * idb is set for pseudonodes, NULL otherwise.
 *
 * Returns pointer to context block or NULL.
 */
isis_lspcontext *isis_create_lsp (clns_pdbtype *pdb,
				  int level,
				  int pseudo_id,
				  idbtype *idb)
{
    isis_lspcontext *context;

    /* Create the context block. */

    context = malloc_named(sizeof(isis_lspcontext), "ISIS LSP context");
    if (context != NULL) {
	context->pdb = pdb;
	context->level = level;
	context->lsp_idb = idb;

	/* Initialize the LSP ID. */
	
	station_copy(pdb->systemId, context->lspid.system_id);
	context->lspid.pseudo_id = pseudo_id;
	
	/* Create the rest of the LSP. */
	
	if (!isis_next_lsp(context, TRUE)) {
	    free(context);			/* No memory! */
	    context = NULL;

	}
    }
    return(context);
}

/*
 * isis_destroy_context
 *
 * Destroy the context structure and the partial LSP carried therein.
 */
void isis_destroy_context (isis_lspcontext *context)
{
    if (context->lsp != NULL)
	isis_free_lsp(context->pdb, context->lsp);
    free(context);
}

/*
 * isis_lsp_done
 *
 * Tie off the current LSP and install it in the database.
 */
void isis_lsp_done (isis_lspcontext *context)
{
    isis_lspdbtype *lsp;

    lsp = context->lsp;
    if (lsp == NULL)
	return;

    lsp->db_length = context->lsp_end - (uchar *) lsp;
    isis_checksum_lsp(context->pdb, lsp);
    isis_link_lsp(context->pdb, lsp, context->level, NULL, NULL);
    context->lsp = NULL;
    context->usable_length = 0;

    /* Keep track of how many LSPs we have created */

    isis_traffic[context->pdb->proc_type][(context->level == ISIS_CIRCUIT_L1) ?
					  ISIS_L1_LSP_BUILD :
					  ISIS_L2_LSP_BUILD]++;

}

/*
 * isis_add_item_entry
 *
 * Allocate space to add a new entry under the current item.
 * If there is insufficient space in the current LSP to add the
 * item, install the current LSP fragment and move on to a new one.
 *
 * Returns a pointer to the data area for the new item, or NULL if
 * we're out of memory.
 *
 * If stop_on_new_item is TRUE and we've advanced to a new item
 * (or a new LSP), create the new item header but don't add the
 * new entry.  This is used when the beginning of an item has a
 * different format than the rest of the item.
 *
 * If new_flag is supplied, it is set to TRUE if we've moved on to
 * a new item.
 */
void *isis_add_item_entry (isis_lspcontext *context, int size, 
			   boolean stop_on_new_item, boolean *new_flag)
{
    void *entry;
    boolean local_new_flag = FALSE;

    /* Bomb if it can never fit. */

    if (size > ISIS_INFO_TYPE_MAXDATA)
	crashdump(0);

    if (context->lsp == NULL)
	return(NULL);

    /*
     * Loop trying to find enough space for the new item.  We are 
     * guaranteed that the loop will terminate eventually because a
     * new LSP will have more than ISIS_INFO_TYPE_MAXDATA space.
     */
    while (TRUE) {
	int bytes_left;
	int item_size;

	/*
	 * Figure out whether there's enough space in the lsp to add 
	 * the new entry.
	 */
	bytes_left = context->usable_length -
	    (context->lsp_end - (uchar *) context->lsp) - 
		ISIS_INFO_TYPE_OVERHEAD;

	if (bytes_left < size) {
	    uchar item_type;

	    /*
	     * Not enough space.  Finish off the current LSP and move on
	     * to the next one.
	     */
	    item_type = context->item_type; /* It'll get smashed */
	    isis_item_done(context, NULL);
	    isis_lsp_done(context);
	    if (!isis_next_lsp(context, FALSE))
		return(NULL);
	    if (!isis_new_item(context, item_type, 0, NULL, NULL, FALSE))
		return(NULL);
	    local_new_flag = TRUE;
	    continue;
	}

	/*
	 * Figure out whether there's enough space in the current item
	 * to add the new entry.
	 */
	item_size = context->lsp_end - (uchar *) context->lsp_item;
	if ((item_size + size) <=
	    (ISIS_INFO_TYPE_MAXDATA + ISIS_INFO_TYPE_OVERHEAD))
	    break;			/* Enough room */

	/* Not enough room.  Tie off the current item and start a new one. */
	
	isis_item_done(context, NULL);
	if (!isis_new_item(context, context->item_type, 0, NULL, NULL, TRUE))
	    return(NULL);
	local_new_flag = TRUE;
    }

    /* Space available.  Just add it here. */

    entry = context->lsp_end;
    if (!local_new_flag || !stop_on_new_item)
	context->lsp_end += size;
    if (new_flag)
	*new_flag = local_new_flag;
    return(entry);
}

/*
 * isis_alloc_lspix
 *
 * Allocate an LSP index for a new LSP
 *
 * Returns the LSP index, or 0 if no memory.
 *
 * Index 1 is never used.
 *
 * If the allocated entry is never used, it remains free.
 */
ushort isis_alloc_lspix (clns_pdbtype *pdb)
{
    isis_lspix_type *old_table, *new_table, *new_entry, *adjust_entry;
    int ix, adjust_ix, chain_ix;
    ushort hold_version;
    isis_lsp_linkage **chain_ptr;
    isis_backuptype **backup_ptr;

    new_entry = NULL;
    ix = 0;
    old_table = pdb->lspix_table;

    /* Search for a free entry.  Brute force, maybe it will matter some day. */

    if (old_table) {
	for (ix=1; ix<pdb->lspix_count; ix++) {
	    new_entry = &old_table[ix];
	    if (!new_entry->lspix_in_use) {
		break;
	    }
	}
    }
    
    if (ix >= pdb->lspix_count) {

	/* No free entries or very first time. */
	
	new_table = malloc_named(sizeof(isis_lspix_type) * 
				 (pdb->lspix_count + ISIS_LSPIX_INCREMENT),
				 "ISIS LSP index");
	if (!new_table)
	    return(0);
	
	/* Copy over the old entries. */
	
	if (old_table) {
	    bcopy(old_table, new_table,
		  sizeof(isis_lspix_type) * pdb->lspix_count);
	    free(old_table);
	}
	
	/* Adjust the prev pointers of each protocol chain. */

	adjust_entry = new_table;
	for (adjust_ix = 0; adjust_ix < pdb->lspix_count; adjust_ix++) {
	    chain_ptr = adjust_entry->chains;
	    backup_ptr = adjust_entry->backup_chains;
	    for (chain_ix = 0; chain_ix < NUM_ISIS_ROUTE_TYPES; chain_ix++) {
		if (*chain_ptr) {
		    (*chain_ptr)->lsp_prev = chain_ptr;
		}
		if (*backup_ptr) {
		    (*backup_ptr)->lsp_prev = backup_ptr;
		}
		chain_ptr++;
		backup_ptr++;
	    }
	    adjust_entry++;
	}
	
	ix = pdb->lspix_count;
	if (ix == 0)			/* Don't use the first entry. */
	    ix = 1;

	pdb->lspix_count += ISIS_LSPIX_INCREMENT;
	pdb->lspix_table = new_table;
	new_entry = &new_table[ix];
    }

    /*
     * Wipe out the new entry, but preserve the version number (to help
     * protect against old stray references somewhere).
     */
    hold_version = new_entry->lsp_version;
    memset(new_entry, 0, sizeof(*new_entry));
    new_entry->lsp_version = hold_version + 1; /* What the heck. */
    return(ix);
}

/*
 * isis_backup_valid_error
 *
 * Complain about a validation error in a backup entry.
 */
static void isis_backup_valid_error (isis_backuptype *backup, char *msg,
				     char *string, void *value)
{
    errmsg(&msgsym(BKUPERR, CLNS), msg, backup, backup->ndb_next,
	   backup->ndb_prev, backup->lsp_next, backup->lsp_prev,
	   backup->metric, backup->lsp_index, string, value);
}

/*
 * isis_link_valid_error
 *
 * Complain about a validation error in a linkage entry.
 */
static void isis_link_valid_error (isis_lsp_linkage *link, char *msg,
				     char *string, void *value)
{
    errmsg(&msgsym(LINKERR, CLNS), msg, link, link->lsp_next, link->lsp_prev,
	   link->lsp_index, link->lsp_version, string, value);
}

#define CHAIN_DEBUG
#ifdef CHAIN_DEBUG
/*
 * isis_validate_backup_entry
 *
 * Validates the linkages of a single backup entry.
 *
 * If nonzero, the LSP index value will be validated as well.
 */
static boolean isis_validate_backup_entry (isis_backuptype *backup,
					   ushort lsp_index)
{
    /* There's gotta be an NDB back pointer.  Make sure it's consistent. */

    if (!backup->ndb_prev) {
	isis_backup_valid_error(backup, "No NDB back pointer", "", 0);
	return(FALSE);
    }
    if (*backup->ndb_prev != backup) {
	isis_backup_valid_error(backup,	"NDB back/front inconsistent",
				"front", *backup->ndb_prev);
	return(FALSE);
    }

    /* If there's an NDB forward pointer, make sure that it's consistent. */

    if (backup->ndb_next) {
	if (backup->ndb_next->ndb_prev != &backup->ndb_next) {
	    isis_backup_valid_error(backup, "NDB front/back inconsistent",
				  "back", backup->ndb_next->ndb_prev);
	    return(FALSE);
	}
    }

    /* If an LSP index was specified, validate that. */

    if (lsp_index) {
	if (backup->lsp_index != lsp_index) {
	    isis_backup_valid_error(backup, "LSP index bad", "should be",
				    (void *) (ulong) lsp_index);
	    return(FALSE);
	}
    }

    /* If lsp index is nonzero, there's gotta be LSP linkages. */

    if (backup->lsp_index) {
	if (!backup->lsp_prev) {
	    isis_backup_valid_error(backup, "No LSP back pointer", "", 0);
	    return(FALSE);
	}
	if (*backup->lsp_prev != backup) {
	    isis_backup_valid_error(backup, "LSP back/front inconsistent",
				    "front", *backup->lsp_prev);
	    return(FALSE);
	}
	if (backup->lsp_next) {
	    if (backup->lsp_next->lsp_prev != &backup->lsp_next) {
		isis_backup_valid_error(backup, "LSP front/back inconsistent",
					"back", backup->lsp_next->lsp_prev);
		return(FALSE);
	    }
	}
    } else {				/* No LSP linkages */
	if (backup->lsp_prev || backup->lsp_next) {
	    isis_backup_valid_error(backup, "LSP linkages non-NULL", "", 0);
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * isis_validate_ndb_backup_chain_inline
 *
 * Validates a backup chain along the NDB thread.
 */
static inline void
    isis_validate_ndb_backup_chain_inline (isis_backuptype **chain_head)
{
    isis_backuptype *backup;

    backup = *chain_head;
    if (backup) {
	if (backup->ndb_prev != chain_head) {
	    isis_backup_valid_error(backup, "NDB prev doesn't point at head",
				    "head", chain_head);
	    return;
	}
    }

    while (backup) {

	/* Validate the entry. */

	isis_validate_backup_entry(backup, 0);
	backup = backup->ndb_next;
    }
}

/*
 * isis_validate_lsp_backup_chain
 *
 * Validates a backup chain along the LSP thread.
 *
 * The LSP index is optional;  if present, it will be compared to the
 * index in the backup entries.
 */
static void isis_validate_lsp_backup_chain (isis_backuptype **chain_head,
					    ushort lsp_index)
{
    isis_backuptype *backup;

    backup = *chain_head;
    if (backup) {
	if (backup->lsp_prev != chain_head) {
	    isis_backup_valid_error(backup, "LSP prev doesn't point at head",
				    "head", chain_head);
	    return;
	}
    }
    backup = *chain_head;

    while (backup) {

	/* Validate the entry. */

	isis_validate_backup_entry(backup, lsp_index);
	backup = backup->lsp_next;
    }
}

/*
 * isis_validate_lsp_route_entry
 *
 * Validate a single route entry
 */
static boolean isis_validate_lsp_route_entry (isis_lsp_linkage *linkage)
{
    if (!linkage->lsp_prev) {
	isis_link_valid_error(linkage, "No back pointer", "", 0);
	return(FALSE);
    }
    if (*linkage->lsp_prev != linkage) {
	isis_link_valid_error(linkage, "LSP back/front inconsistent",
			      "front", *linkage->lsp_prev);
	return(FALSE);
    }
    if (linkage->lsp_next) {
	if (linkage->lsp_next->lsp_prev != &linkage->lsp_next) {
	    isis_link_valid_error(linkage, "LSP front/back inconsistent",
				  "back", linkage->lsp_next->lsp_prev);
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * isis_validate_lsp_route_chain
 *
 * Validate a chain of LSP routes
 */
static void isis_validate_lsp_route_chain (isis_lsp_linkage **linkhead)
{
    isis_lsp_linkage *link;

    link = *linkhead;
    if (link) {
	if (link->lsp_prev != linkhead) {
	    isis_link_valid_error(link, "LSP prev doesn't point at head",
				  "head", linkhead);
	    return;
	}
    }

    while (link) {
	isis_validate_lsp_route_entry(link);
	link = link->lsp_next;
    }
}

#else					/* ifdef CHAIN_DEBUG */

/* Get rid of all the overhead. */

static inline void isis_validate_ndb_backup_chain_inline
    (isis_backuptype **chain_head) {}
static inline void isis_validate_lsp_backup_chain
    (isis_backuptype **chain_head, ushort lsp_index) {}
static inline boolean isis_validate_backup_entry (isis_backuptype *backup,
						  ushort lsp_index)
{return(TRUE);}
static inline boolean isis_validate_lsp_route_entry (isis_lsp_linkage *linkage)
{return(TRUE);}
static inline void isis_validate_lsp_route_chain
    (isis_lsp_linkage **linkhead) {}

#endif

/*
 * isis_validate_ndb_backup_chain
 *
 * Validates a backup chain along the NDB thread.  This version exists
 * to be externally-callable.
 */
void isis_validate_ndb_backup_chain (isis_backuptype **chain_head)
{
    isis_validate_ndb_backup_chain_inline(chain_head);
}

/*
 * isis_validate_lspix_array
 *
 * Validate the LSPIX array and its attached chains.
 */
void isis_validate_lspix_array (clns_pdbtype *pdb)
{
    ulong ix, chain_ix;
    isis_lspix_type *entry;
    isis_lsp_linkage **chain_ptr;
    isis_backuptype **backup_ptr;

    if (pdb->lspix_table) {
	if (pdb->lspix_table[0].lsp) {
	    errmsg(&msgsym(LSPIXARRNULL, CLNS), pdb->errmsg_name, 0,
		   pdb->lspix_table[0].lsp);
	}
	entry = &(pdb->lspix_table[1]);
	for (ix = 1; ix < pdb->lspix_count; ix++, entry++) {
	    if (!entry->lspix_in_use) {
		if (entry->lsp) {
		    errmsg(&msgsym(LSPIXARRNULL, CLNS), pdb->errmsg_name, ix,
			   entry->lsp);
		}
		continue;
	    }
	    if (!entry->lsp)
		continue;
	    if (entry->lsp->lsp_index != ix) {
		errmsg(&msgsym(LSPIXARRMATCH, CLNS), pdb->errmsg_name, ix,
		       entry->lsp, "index", entry->lsp->lsp_index, ix);
	    }
	    if (entry->lsp->level != entry->level) {
		errmsg(&msgsym(LSPIXARRMATCH, CLNS), pdb->errmsg_name, ix,
		       entry->lsp, "level", entry->lsp->level, entry->level);
	    }
	    chain_ptr = entry->chains;
	    backup_ptr = entry->backup_chains;
	    for (chain_ix = 0; chain_ix < NUM_ISIS_ROUTE_TYPES;
		 chain_ix++, chain_ptr++, backup_ptr++) {
		if (*chain_ptr) {
		    if ((*chain_ptr)->lsp_prev != chain_ptr) {
			errmsg(&msgsym(LSPIXHEAD, CLNS), pdb->errmsg_name,
			       "chain", chain_ix, chain_ptr,
			       (*chain_ptr)->lsp_prev);
		    }
		    isis_validate_lsp_route_chain(chain_ptr);
		}
		if (*backup_ptr) {
		    if ((*backup_ptr)->lsp_prev != backup_ptr) {
			errmsg(&msgsym(LSPIXHEAD, CLNS), pdb->errmsg_name,
			       "backup", chain_ix, backup_ptr,
			       (*backup_ptr)->lsp_prev);
		    }
		    isis_validate_lsp_backup_chain(backup_ptr, ix);
		}
	    }
	}
    }
}


/*
 * isis_valid_lsp_version
 *
 * Returns TRUE if the specified LSP index/version number pair is up-to-date
 */
boolean isis_valid_lsp_version(clns_pdbtype *pdb, ushort index, ushort version)
{
    isis_lspix_type *entry;

    entry = isis_lsp_index_to_lspix_entry(pdb, index);

    if (!entry)
	return(FALSE);			/* Bogus index or NULL PDB */

    if (!entry->lspix_in_use)		/* Bogus index */
	return(FALSE);

    if (entry->lsp_version != version)	/* Out of date */
	return(FALSE);

    return(TRUE);			/* All is proper with the world. */
}

/*
 * isis_attach_backup_chain
 *
 * Delink a backup chain from one pointer, and link it to another.
 */
void isis_attach_backup_chain (isis_backuptype **oldhead,
			       isis_backuptype **newhead)
{
    isis_validate_ndb_backup_chain_inline(oldhead);

    *newhead = *oldhead;
    if (*newhead) {
	(*newhead)->ndb_prev = newhead;
    }
    *oldhead = NULL;
}

/*
 * isis_add_entry_to_lsp_chain
 *
 * Link something into an LSP chain.
 */
void isis_add_entry_to_lsp_chain (isis_lsp_linkage *entry,
				   isis_lsp_linkage **head)
{
    if ((entry == NULL) || (head == NULL)) {
	return;
    }
    if (entry->lsp_prev != NULL) {
	errmsg(&msgsym(ONLIST, CLNS), entry->lsp_prev, entry->lsp_next,
	       entry->lsp_index, entry->lsp_version);
    }
    isis_validate_lsp_route_chain(head);
    entry->lsp_next = *head;
    *head = entry;
    if (entry->lsp_next != NULL) {
	entry->lsp_next->lsp_prev = &entry->lsp_next;
    }
    entry->lsp_prev = head;
}

/*
 * isis_delete_entry_from_lsp_chain
 *
 * Delinks an entry from a chain.
 *
 * Does nothing if the entry is not on a chain.
 */
void isis_delete_entry_from_lsp_chain (isis_lsp_linkage *entry)
{
    if (entry == NULL)			/* Bail */
	return;

    if (entry->lsp_prev == NULL)	/* Not on a chain */
	return;

    isis_validate_lsp_route_entry(entry);
    if (entry->lsp_next != NULL) {
	entry->lsp_next->lsp_prev = entry->lsp_prev;
    }
    *entry->lsp_prev = entry->lsp_next;
    entry->lsp_next = NULL;
    entry->lsp_prev = NULL;
}

/*
 * isis_detach_backup_entry
 *
 * Detach a backup entry from its threads.  Does not free the entry!
 */
static void isis_detach_backup_entry (isis_backuptype *backup)
{
    isis_validate_backup_entry(backup, 0);

    /* First disentangle the NDB/backup thread. */

    if (backup->ndb_prev) {
	*(backup->ndb_prev) = backup->ndb_next;
    }
    if (backup->ndb_next) {
	backup->ndb_next->ndb_prev = backup->ndb_prev;
    }

    /* Now the LSP thread. */

    if (backup->lsp_prev) {
	*(backup->lsp_prev) = backup->lsp_next;
    }
    if (backup->lsp_next) {
	backup->lsp_next->lsp_prev = backup->lsp_prev;
    }
}

/*
 * isis_delete_backup_entry
 *
 * Delete a backup entry from all of its threads.  Free it.
 */
void isis_delete_backup_entry (clns_pdbtype *pdb, isis_backuptype *backup)
{
    isis_detach_backup_entry(backup);
    chunk_free(pdb->backup_chunks, backup);
}

/*
 * isis_delete_backup_chain
 *
 * Deletes a chain of backup entries from either the NDB or LSP threads.
 */
void isis_delete_backup_chain (clns_pdbtype *pdb, isis_backuptype **threadhead)
{
    if (!threadhead)
	return;

    /* Keep deleting entries until the chain is empty. */

    while (*threadhead) {
	isis_delete_backup_entry(pdb, *threadhead);
    }
}

/*
 * isis_add_backup_entry
 *
 * Add a backup entry to the appropriate chains.  Either of the chain head
 * pointers may be null, in which case the item is assumed to not be
 * linked to that chain.
 */
void isis_add_backup_entry (isis_backuptype *backup, isis_backuptype **ndbhead,
			    isis_backuptype **lsphead)
{
    /* Thread the NDB chain if present. */

    if (ndbhead) {
	isis_validate_ndb_backup_chain_inline(ndbhead);
	backup->ndb_next = *ndbhead;
	backup->ndb_prev = ndbhead;
	*ndbhead = backup;
	if (backup->ndb_next) {
	    backup->ndb_next->ndb_prev = &backup->ndb_next;
	}
    }

    /* Thread the LSP chain if present. */

    if (lsphead) {
	isis_validate_lsp_backup_chain(lsphead, 0);
	backup->lsp_next = *lsphead;
	backup->lsp_prev = lsphead;
	*lsphead = backup;
	if (backup->lsp_next) {
	    backup->lsp_next->lsp_prev = &backup->lsp_next;
	}
    }
}

/*
 * isis_install_overflow_backup
 *
 * Trim the chain and install an overflow entry here.
 */
static void isis_install_overflow_backup (clns_pdbtype *pdb,
					  isis_backuptype **prev_backup,
					  ulong metric)
{
    isis_backuptype *orphan_chain;
    isis_backuptype *overflow_entry;

    /* First delink the rest of the chain. */

    isis_attach_backup_chain(prev_backup, &orphan_chain);

    /* Now free the chain. */

    isis_delete_backup_chain(pdb, &orphan_chain);

    /* Now create an overflow entry and link it in. */

    overflow_entry = chunk_malloc(pdb->backup_chunks);
    if (!overflow_entry)
	return;

    overflow_entry->metric = metric;
    isis_add_backup_entry(overflow_entry, prev_backup, NULL);
}

/*
 * isis_install_backup
 *
 * Install an ISIS backup entry for a route.  The backup entry
 * will be added to the chain provided.  If an excessive number
 * of different metric backup entries are present, an entry will be created
 * that indicates that the backup list overflowed, and will cause a full SPF
 * to be done if it is ever encountered when backup routes need
 * to be installed.
 *
 * The net result is a chain in order of increasing metric, with no more
 * than ISIS_BACKUP_MAX_METRIC_DEPTH different metrics on the chain, and
 * potentially an overflow entry with LSP index = 0 on the end of the
 * chain (which does not count against the metric limit).
 *
 * We make no assumptions about the previous number of metrics on the chain,
 * so that we can perhaps change the maximum number on the fly.
 *
 * The overflow entry carries the metric of the lowest metric that was
 * truncated from the backup chain.  We can insert entries directly in
 * front of this entry if they have a lower metric than the overflow
 * entry, since we know that there aren't any routes with a better metric
 * out there that were lost.  If the new entry has an equal or worse
 * metric than the overflow entry, we drop it.
 */
void isis_install_backup (clns_pdbtype *pdb,
			  ulong metric,
			  ushort lsp_index,
			  ulong lspix_chain_index,
			  isis_backuptype **ndbhead)
{
    isis_backuptype *backup, *current_backup, **prev_backup, **lsphead;
    isis_lspix_type *lspix_entry;
    ulong prev_metric, metric_count;

    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp_index);
    if (lspix_entry) {
	lsphead = &lspix_entry->backup_chains[lspix_chain_index];

	/* Set the high order metric bit if this is an L2 LSP. */
	
	if (lspix_entry->level == CLNS_LEVEL2)
	    metric |= ISIS_BACKUP_L2_METRIC_BIT;
    } else {
	lsphead = NULL;
    }

    /* Walk the chain looking for the place to sort in our metric entry. */

    isis_validate_ndb_backup_chain_inline(ndbhead);
    if (lsphead)
	isis_validate_lsp_backup_chain(lsphead, lsp_index);
    prev_backup = ndbhead;
    current_backup = *ndbhead;
    metric_count = 0;
    prev_metric = 0;			/* Just to keep compiler quiet */
    while (current_backup) {
	if (metric < current_backup->metric) /* We found our spot */
	    break;
	if (current_backup->lsp_index == 0) /* The overflow entry? */
	    break;
	if (!metric_count || current_backup->metric != prev_metric) {
	    prev_metric = current_backup->metric;
	    metric_count++;
	}
	prev_backup = &current_backup->ndb_next;
	current_backup = *prev_backup;
    }

    /*
     * We've found the spot.  Bail out if the next entry is an overflow and
     * our metric is equal or worse than it.
     */
    if (current_backup && current_backup->lsp_index == 0 &&
	metric >= current_backup->metric)
	return;

    /* Build and link in the new entry. */

    backup = chunk_malloc(pdb->backup_chunks);
    if (!backup)
	return;
    backup->metric = metric;
    backup->lsp_index = lsp_index;
    isis_add_backup_entry(backup, prev_backup, lsphead);

    /*
     * Now walk the chain from here on and see if we've exceeeded the
     * max number of metrics allowed on the chain.  If so, trim the chain
     * and install an overflow entry.  Note that we may delete our own
     * entry in doing so, if we added the last one.  Note that we may
     * also redundantly delete and re-add an overflow entry, but
     * it doesn't cost much and it makes the code cleaner.
     */
    current_backup = backup;
    while (current_backup) {
	if (!metric_count || current_backup->metric != prev_metric) {
	    prev_metric = current_backup->metric;
	    metric_count++;
	}
	if (metric_count > ISIS_BACKUP_MAX_METRIC_DEPTH) {
	    if (current_backup->lsp_index == 0)
		break;			/* It's the overflow entry */
	    spf_ebuginf(pdb, TRUE,
			"Overflow entry added, truncate L%d metric %d",
			ISIS_BACKUP_LEVEL(prev_metric),
			ISIS_BACKUP_BASE_METRIC(prev_metric));
	    isis_install_overflow_backup(pdb, prev_backup, prev_metric);
	    break;
	}
	prev_backup = &current_backup->ndb_next;
	current_backup = *prev_backup;
    }
}

/*
 * isis_mark_lsp_to_recalculate
 *
 * Mark an LSP to cause its routes to be recalculated (a partial route
 * calculation).
 *
 * Sets the global flag as well to trigger the route recalculation.
 */
void isis_mark_lsp_to_recalculate (clns_pdbtype *pdb, ushort lsp_index)
{
    isis_lspix_type *lspix_entry;
    isis_lspdbtype *lsp;

    SAVE_CALLER();

    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp_index);
    if (lspix_entry)		/* Should always be valid */
	lspix_entry->lsp_changed = TRUE;
    isis_trigger_partial_route_calculation(pdb);
    lsp = lspix_entry->lsp;
    spf_ebuginf(pdb, FALSE,
		"L%d LSP %d (%s) flagged for recalculation from %x",
		lspix_entry->level, lsp_index,
		lsp ? lspid_string(&lsp->data->lspid) : "deleted", caller());
}

/*
 * isis_create_cleanup_queue_entry
 *
 * Create and enqueue a cleanup queue entry.
 *
 * Returns a pointer to the entry, or NULL if no memory.
 *
 * Links the entry to the PDB.
 */
isis_cleanup_queue *isis_create_cleanup_queue_entry (clns_pdbtype *pdb)
{
    isis_cleanup_queue *cleanup;

    cleanup = malloc_named(sizeof(isis_cleanup_queue), "ISIS cleanup");
    if (cleanup) {
	cleanup->next_cleanup = pdb->cleanup_queue;
	pdb->cleanup_queue = cleanup;
    }
    return(cleanup);
}
