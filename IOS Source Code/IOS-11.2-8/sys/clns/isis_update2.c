/* $Id: isis_update2.c,v 3.9.12.14 1996/09/12 00:17:20 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_update2.c,v $
 *------------------------------------------------------------------
 * isis_update2.c -- IS-IS Update Process
 *
 * 24-Nov-1990, Paulina Knibbe
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_update2.c,v $
 * Revision 3.9.12.14  1996/09/12  00:17:20  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.9.12.13  1996/08/28  12:38:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.12  1996/08/12  01:07:11  dkatz
 * CSCdi45425:  NLSP may not report RIP/SAP routes
 * Branch: California_branch
 * Don't let the DR timer get in the way of reporting adjacencies (and
 * remove a couple of other checks for it).  Rebuild both non-pseudonode
 * and pseudonode LSPs when an interface changes state.
 *
 * Revision 3.9.12.11  1996/08/11  15:13:34  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.9.12.10  1996/08/08  15:48:24  dkatz
 * CSCdi65479:  ISIS/NLSP mishandles purged packets
 * Branch: California_branch
 * Handle purged LSP flooding rules correctly (at last).  Keep purged
 * packets for a full LSP lifetime in all cases except when they expire
 * from natural causes.  Remove kludge that keeps SRM-set purged LSPs
 * in the database, since we now keep the LSPs long enough after purging.
 *
 * Revision 3.9.12.9  1996/08/08  14:51:00  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.9.12.8  1996/08/07  08:57:31  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/08/04  23:46:14  dkatz
 * CSCdi65078:  IS-IS Duplicate System ID detection still too sensitive
 * Branch: California_branch
 * Only perform duplicate detection on fragment zero of the non-pseudonode
 * LSP.
 *
 * Revision 3.9.12.6  1996/08/04  23:35:31  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.9.12.5  1996/07/30  06:13:27  hsmit
 * CSCdi64266:  ISIS should make difference between spurious LSP purges
 * and DUPSYS
 * Branch: California_branch
 * Purged LSPs should not fire the duplicate system ID warning that
 * was introduced in CSCdi57308.
 *
 * Revision 3.9.12.4  1996/07/29  23:10:58  hsmit
 * CSCdi64255:  Show isis spf-log should not confuse when running l1-only
 * or l2-only
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/05/26  06:40:52  dkatz
 * CSCdi58613:  ISIS/NLSP retransmission can get stuck
 * Branch: California_branch
 * Clear the RXM bits when changing the sequence number in place.
 *
 * Revision 3.9.12.2  1996/05/10  19:45:48  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.9.12.1  1996/03/18  19:04:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.4  1996/03/16  06:31:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.12.3  1996/03/13  01:14:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.12.2  1996/03/07  08:41:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  00:21:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/20  18:06:45  dkatz
 * CSCdi47916:  ISIS/NLSP LSP checksum errors
 * Improve diagnostics.  Ignore errors if the holdtime is zero
 * (workaround).
 *
 * Revision 3.8  1996/02/19  21:32:39  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.7  1996/01/29  07:26:53  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1995/12/22  21:58:16  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.5  1995/12/01  04:09:44  dkatz
 * CSCdi37675:  ISIS/NLSP can retransmit early
 *
 * Revision 3.4  1995/12/01  02:48:37  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:54:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:10:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:52:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/04  23:31:15  myeung
 * CSCdi41388:  ISIS debug message is misleading
 *
 * Revision 2.8  1995/09/23  23:14:20  dkatz
 * CSCdi39776:  ISIS can mislink backup chain
 * Dont leave a backup chain dangling from the heap.
 * Make sure that we run SPF when the second level adjacency comes up
 * if L1L2.
 *
 * Revision 2.7  1995/09/23  22:48:53  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.6  1995/09/23  21:37:59  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.5  1995/08/23  18:26:20  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.4  1995/08/23  18:09:18  dkatz
 * CSCdi37888:  Better debugging of SPF trigger events needed
 * Remove special-case check for ISIS, and let the fancy LSP comparison
 * debugging do its job.
 *
 * Revision 2.3  1995/08/23  18:03:28  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/06/21  08:54:04  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:19:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This module is an implementation of the update process in
 * the ISO ISIS routing protocol.
 *
 * This process is responsible for building link state packets from the
 * adjacency database. It is also responsible for flooding the link
 * state packets it receives.
 */
#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "mgd_timers.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"
#include "bitlogic.h"
#include "../clns/parser_defs_isis.h"

typedef enum age_relation_ {OLD_NEWER, NEW_OLDER = OLD_NEWER, SAME_AGE,
				NEW_NEWER, OLD_OLDER = NEW_NEWER} age_relation;
typedef enum spf_change_ {SPF_NO_CHANGE, SPF_PARTIAL, SPF_FULL} spf_change;


/*
 * isis_safe_option_code
 *
 * Returns TRUE if the option code is "safe," i.e., requires only a
 * partial route update if its contents are different.
 */
static inline boolean isis_safe_option_code (uchar code)
{
    switch (code) {
      case ISIS_NLSP_MGMT_INFO:
      case ISIS_NLSP_SVCS_INFO:
      case ISIS_NLSP_EXT_ROUTES:
      case ISIS_NLSP_SUMMARY_ROUTES:
      case ISIS_IP_IF_ADDRESSES:
      case ISIS_IP_INTERNAL_ADDR:
      case ISIS_IP_EXTERNAL_ADDR:
	return(TRUE);			/* These guys are OK */
	
      default:
	return(FALSE);			/* Nope. */
    }
}
/*
 * Compare old and new LSPs to see if a full SPF run is necessary or not.
 *
 * If the two LSPs are exactly alike, do nothing.  Do an SPF run if
 * important fields or options are different.  Missing TLVs are fine
 * so long as the missing ones are unimportant.  A missing LSP is a
 * special case of missing TLVs.
 *
 * When an LSP changes from expired to unexpired, or vice versa, the LSP
 * is treated as if all of its fields have changed.  If the LSP contains
 * only "unimportant" fields then a partial route update is only necessary;
 * otherwise, a full SPF is in order.
 */
static spf_change isis_check_spf (clns_pdbtype *pdb, isis_lspdbtype *old_lsp,
			   isis_lspdbtype *new_lsp, int level,
			   int *trigger_type)
{
    isis_info_type *old_item, *new_item;
    uchar *old_end, *new_end;
    boolean old_spent;			/* TRUE if we hit end of old LSP */
    boolean new_spent;			/* TRUE if we hit end of new LSP */
    boolean birth_or_death;		/* TRUE if one is expired, one isn't */
    char *lsp_id_string;

    old_spent = FALSE;
    new_spent = FALSE;
    if (isis_nlsp_spf_trigger_debug[pdb->proc_type]) {
	lsp_id_string = lspid_string(&new_lsp->data->lspid);
    } else {
	lsp_id_string = NULL;
    }

    if (!old_lsp) {

	/* Brand new LSP.  If LSP zero, force a full SPF. */

	update_pbuginf(pdb, "New LSP");
	if (new_lsp->data->lspid.lsp_number == 0) {
	    spf_tbuginf(pdb, "L%d, new LSP 0 %s", level, lsp_id_string);
	    *trigger_type = ISIS_SPF_TRIG_NEWLSP0;
	    return(SPF_FULL);
	}

	/* Not LSP 0.  Mark the old LSP as spent. */

	old_spent = TRUE;

	birth_or_death = TRUE;		/* No old LSP. */

    } else {

	/*
	 * Note whether one is expired and one isn't.  The use of the XOR
	 * bit operator works below because the lsp_active field is a single
	 * bit.  If it were a boolean, it would also work if the values TRUE
	 * and FALSE were always used.  So either way, it's relatively safe.
	 */
	birth_or_death = old_lsp->lsp_active ^ new_lsp->lsp_active;
    }

    /* If important stuff in LSP 0 changed, we need an SPF. */

    if (old_lsp && new_lsp->data->lspid.lsp_number == 0) {
	if ((old_lsp->data->p != new_lsp->data->p) ||
	    (old_lsp->data->att != new_lsp->data->att) ||
	    (old_lsp->data->hippity != new_lsp->data->hippity) ||
	    (old_lsp->data->is_type != new_lsp->data->is_type)) {
	    update_pbuginf(pdb, "Important fields changed");
	    spf_tbuginf(pdb, "L%d, LSP fields changed %s", level,
			lsp_id_string);
	    *trigger_type = ISIS_SPF_TRIG_FIELDS;
	    return(SPF_FULL);
	}
    }

    /*
     * If the TLV data in the LSP is identical, there is no change.
     */
    if (old_lsp && old_lsp->db_length == new_lsp->db_length &&
	!bcmp(old_lsp->data->item, new_lsp->data->item,
	      isis_lspdb_tlv_length(new_lsp)) &&
	!birth_or_death) {
	return(SPF_NO_CHANGE);
    }

    /* Set up for the LSP comparison. */

    if (old_spent) {
	old_end = NULL;			/* So the compiler doesn't complain */
	old_item = NULL;
    } else {
	old_end = ((uchar *) old_lsp) + old_lsp->db_length;
	old_item = (isis_info_type *) &old_lsp->data->item;
    }
    new_end = ((uchar *) new_lsp) + new_lsp->db_length;
    new_item = (isis_info_type *) &new_lsp->data->item;

    /*
     * Walk the LSPs, comparing contents.  For a partial update, only
     * "safe" options can have changed.  In all other cases we must do a
     * full SPF run.
     */
    while (TRUE) {
	boolean different;

	/* See if we've hit the end of either or both LSPs. */

	if (!old_spent)
	    old_spent = (uchar *) old_item >= old_end;
	if (!new_spent)
	    new_spent = (uchar *) new_item >= new_end;

	if (old_spent && new_spent)	/* All done! */
	    break;

	/*
	 * If types are different and at least one of them is "unsafe",
	 * do a full SPF.
	 */
	if (!old_spent && !new_spent && old_item->code != new_item->code) {
	    update_pbuginf(pdb, "TLV code mismatch (%x, %x)",
			   old_item->code, new_item->code);
	    if (!isis_safe_option_code(old_item->code) ||
		!isis_safe_option_code(new_item->code)) {
		spf_tbuginf(pdb, "L%d, %s TLV code mismatch (%x, %x)",
			    level, lsp_id_string, old_item->code,
			    new_item->code);
		*trigger_type = ISIS_SPF_TRIG_CODE;
		return(SPF_FULL);
	    }
	}

	/*
	 * See if the TLVs have the same data.  An LSP expiration or renewal
	 * is the same as a difference in data.
	 */
	different = TRUE;		/* If only one is present */
	if (!old_spent && !new_spent && !birth_or_death) {
	    different = (old_item->length != new_item->length);
	    if (!different) {
		different = bcmp(old_item->value, new_item->value, 
				 old_item->length);
	    }
	}

	/* If the two items differ, check for "safe" types. */

	if (different) {
	    uchar code;
	    code = old_spent ? new_item->code : old_item->code;
	    update_pbuginf(pdb, "TLV contents different, code %x", code);
	    if (!isis_safe_option_code(code)) {
		spf_tbuginf(pdb, "L%d, %s TLV contents changed, code %x",
			    level, lsp_id_string, code);
		*trigger_type = ISIS_SPF_TRIG_TLVCHG;
		return(SPF_FULL);	/* Something important changed! */
	    }
	}

	/* Advance the pointers. */

	if (!old_spent) {
	    ADVANCE_ITEM(old_item, TRUE);
	    old_spent = (uchar *) old_item >= old_end;
	}
	if (!new_spent) {
	    ADVANCE_ITEM(new_item, TRUE);
	    new_spent = (uchar *) new_item >= new_end;
	}
    }
    return(SPF_PARTIAL);
}

/*
 * isis_schedule_psnp
 *
 * Schedule a PSNP to be sent in one second on this circuit, if there
 * isn't already one scheduled.  We do this to batch up acknowledgements
 * to multiple LSPs.
 */
static void isis_schedule_psnp (clns_pdbtype *pdb, idbtype *idb)
{
    isisidbtype *isisidb;

    isisidb = get_isis_idb(pdb, idb);

    if (!isisidb)
	return;

    if (!isisidb->isis_running)
	return;

    /* Bail if timer is already running. */

    if (mgd_timer_running(&isisidb->isis_psnp_timer))
	return;

    /* Otherwise, set it to fire in one second. */

    mgd_timer_start(&isisidb->isis_psnp_timer, ONESEC);
}

/*
 * Compare the relative ages of two sets of LSP information.  Sequence
 * number is highest precedence, then expiration status, then checksum.
 */
static age_relation isis_compare_age (ulong old_seq, ulong new_seq,
				      boolean old_running, boolean new_running,
				      ushort old_cksum, ushort new_cksum)
{
    /* Test the sequence numbers. */
    
    if (old_seq < new_seq) {
	return(NEW_NEWER);
    } else if (old_seq > new_seq) {
	return(OLD_NEWER);
    } else {
	
	/*
	 * Sequence numbers equal.  If one is expired, that one's newer.
	 * If both are expired, they're equal.
	 */
	if (old_running && !new_running) {
	    return(NEW_NEWER);
	} else if (!old_running && new_running) {
	    return(OLD_NEWER);
	} else if (!old_running && !new_running) {
	    return(SAME_AGE);
	} else {
	    
	    /*
	     * Both are unexpired.  Compare checksums.
	     * The one with the higher checksum is newer.
	     */
	    if (old_cksum < new_cksum) {
		return(NEW_NEWER);
	    } else if (old_cksum > new_cksum) {
		return(OLD_NEWER);
	    } else {
		return(SAME_AGE);
	    }
	}
    }
}


/*
 * ISIS fast acknowledgment routines.
 * We try to save CPU when we receive multiple copies of the same
 * LSP over different interfaces. This should be beneficial especially
 * in NBMA environments with large full meshes.
 * We keep a small cache with the lsp index of the most recently received
 * LSPs. We x-or the LSP checksum bytes as the hashfunction.
 * As we will always send a PSNP for an LSP we have in our database,
 * there should be no problem when we receive a corrupted LSP, but
 * recognize it anyway.
 * The fast acknowledgement prevents a LSP tree-lookup, the validation
 * of the checksum and validation of most of the content.
 */
static isis_lspdbtype *isis_try_fastpsnp (clns_pdbtype *pdb, int level,
					paktype *pak, isis_LSP_hdrtype *isis)
{
    isis_fastpsnp_type *fpsnp_cache;
    isis_fastpsnp_type *fpsnp_entry;
    ushort lspix;
    isis_lspdbtype *lsp;
    ushort cache_index;
    boolean same_lsp;

    fpsnp_cache = pdb->fastpsnp_cache;

    /*
     * Do necessary checks to make sure the cache entry is valid.
     * Check whether the FastPSNP cache is enabled, whether we have
     * received a runt LSP, and whether we received a purged LSP.
     * In any of these cases we use the old-fashioned robust path.
     */
    if (!fpsnp_cache) {
	return(NULL);
    }

    if (isis_pak_length(pak, pdb) < ISIS_MINIMAL_HEADER_SIZE) {
	return(NULL);
    }

    if (GETSHORT(isis->ht) == 0) {
	return(NULL);
    }

    /*
     * Get the right entry from the cache.
     * A zero lspix indicates that the entry is not in use.
     * Check to see if the lspix points to a valid and the correct LSP.
     */

    cache_index = isis_fastpsnp_hash(isis->checksum);
    fpsnp_entry = &fpsnp_cache[cache_index];
    lspix = fpsnp_entry->lspix;

    lsp = isis_lspix_to_lsp(pdb, lspix);
    if (!lsp) {
	return(NULL);
    }

    /*
     * Compare LSPid, sequence number, checksum and level. This is what
     * identifies an LSP.
     */

    if (level != lsp->level) { 
	return(NULL);
    }

    same_lsp = (memcmp(&isis->lspid, &lsp->data->lspid,
		       ISIS_FASTPSNP_ID_SIZE) == 0);
    if (!same_lsp) {
	return(NULL);
    }

    /*
     * We can now safely assume we already have this LSP in our LSDB.
     * Increment counters. Return the lspix of the LSP we have found.
     */

    isis_traffic[pdb->proc_type][ISIS_FASTPSNP_CACHEHITS]++;

    return(lsp);
}

/*
 * Populate an entry of the fast psnp cache.
 */
static void isis_populate_fastpsnp_entry (clns_pdbtype *pdb, ushort lspix,
				   uchar *checksum)
{
    isis_fastpsnp_type *fpsnp_cache;
    isis_fastpsnp_type *fpsnp_entry;
    uchar cache_index;

    fpsnp_cache = pdb->fastpsnp_cache;
    if (!fpsnp_cache) {
	return;
    }

    cache_index = isis_fastpsnp_hash(checksum);
    fpsnp_entry = &fpsnp_cache[cache_index];
    fpsnp_entry->lspix = lspix;

    update_pbuginf(pdb,
		   "Populating FastPSNP cache (index %d lspix %d chksm %x)",
		     cache_index, lspix, GETSHORT(checksum));
}


/*
 * isis_regenerate_lsp
 *
 * The function processes a received LSP with an LSPID for this system.
 * Write the received sequence number into the corresponding LSP fragment
 * and rebuild the lsp from scratch. This has the net effect of regenerating
 * the received LSP fragment with received sequence number + 1.
 *
 * We keep track of how often the non-pseudonode LSP gets regenerated as
 * a means of detecting duplicate system IDs.  If it happens too many times
 * in too short of a time, we will issue a warning message.
 */
static boolean isis_regenerate_lsp (clns_pdbtype *pdb,
				    isis_lspdbtype *rcv_lsp,
				    int level)
{

    idbqueuetype *idbq;
    idbtype      *idb;
    isisidbtype  *isisidb;
    isis_lspid *lspid;

    /*
     * Non-pseudo node LSP.
     */
    lspid = &rcv_lsp->data->lspid;
    if (!isis_lspid_is_pseudonode(lspid)) {
	if (level == ISIS_CIRCUIT_L1) {
	    locupdate_buginf(pdb, "L1 regeneration");
	    isis_build_L1_pdu(pdb);
	} else {
	    locupdate_buginf(pdb, "L2 regeneration");
	    isis_build_L2_pdu(pdb);
	}

	/*
	 * Perform duplicate system ID detection on fragment zero of the
	 * non-pseudonode LSP.
	 */
	if (rcv_lsp->lsp_active &&
	    (lspid->pseudo_id == 0 && lspid->lsp_number == 0)) {
	    pdb->regen_count++;
	    if (mgd_timer_running(&pdb->systemid_dup_timer)) {
		if (pdb->regen_count >= ISIS_DUPID_COUNT) {
		    errmsg(&msgsym(DUPSYSTEM, CLNS), pdb->errmsg_name,
			   station_string(pdb->systemId));
	        }
	    } else {
		mgd_timer_start(&pdb->systemid_dup_timer, ISIS_DUPID_INTERVAL);
	    }
	}
		       
	return(TRUE);
    } else {

	/*
	 * Pseudo-node LSP.
	 */
	for (idbq = pdb->idbqueue; idbq; idbq = idbq->next) {
	    idb = idbq->idb;
	    isisidb = get_isis_idb(pdb, idb);
	    if (!isisidb) continue;
	    if (level == ISIS_CIRCUIT_L1) {
		if (systemid_match(lspid->system_id, 
				   isisidb->isis_L1_dr_id)) {
		    if (isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L1)) {
			locupdate_buginf(pdb, "L1 DR regeneration");
			isis_build_L1_dr_pdu(pdb, idb); 
			return(TRUE);
		    }
		}
	    } else {
		if (systemid_match(lspid->system_id, isisidb->isis_L2_dr_id)) {
		    if (isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L2)) {
			locupdate_buginf(pdb, "L2 DR regeneration");
			isis_build_L2_dr_pdu(pdb, idb); 
			return(TRUE);
		    }
		}
	    }
	}
    }
    return(FALSE);
}

/*
 * isis_lsp_no_change
 *
 */
static boolean isis_lsp_no_change (isis_lspdbtype *old_lsp,
				   isis_lspdbtype *new_lsp,
				   int level, clns_pdbtype *pdb)
{

    spf_change   check_result;
    int trigger_type;

    if ((old_lsp == NULL) || (new_lsp == NULL) || (pdb == NULL))
	return(FALSE);

    trigger_type = 0;
    check_result = isis_check_spf(pdb, old_lsp, new_lsp, level, &trigger_type);

    return(check_result == SPF_NO_CHANGE);
}



/*
 * isis_lsp_avl_insert
 *
 *    This routine is called at the bottom of an avl insertion.
 *    It will check whether or not the new data should replace the
 *    old data.
 *
 *    If idb == NULL, it means that the LSP is locally generated.
 *
 *    If this is LSP 0 of a non-pseudonode LSP and this is NLSP, add
 *    the hostname to the cache.
 */
static void isis_lsp_avl_insert (int *h, isis_lspid *key,
				 isis_lspdbtype *new_lsp,
				 isis_tree_type **element, idbtype *idb,
				 int level, clns_pdbtype *pdb,
				 isis_TLV_parse *tlv_parse)
{

    isis_lspdbtype *old_lsp;
    boolean      this_system;
    age_relation relative_age;
    ulong        old_seq, new_seq;
    ushort	 old_cksum, new_cksum;
    spf_change   check_result;
    isisidbtype  *isisidb;
    int trigger_type;
    ulong parent_index;
    boolean rogue_purge;

    isisidb = get_isis_idb(pdb, idb);
    if (idb && !isisidb) {
	isis_free_lsp(pdb, new_lsp);
	return;
    }

    *h = FALSE;
    new_seq = GETLONG(&new_lsp->data->sequence_number);

    if (*element) {			/* There's one in the DB already */
	old_lsp = (*element)->lsp;
	old_seq = GETLONG(&old_lsp->data->sequence_number);
    } else {
	old_lsp = NULL;
	old_seq = 0;
    }

    relative_age = SAME_AGE;
    rogue_purge = FALSE;

    this_system = (station_match(pdb->systemId,
				 new_lsp->data->lspid.system_id));

    /* Determine the relative age.  If there's no old one, the new is newer. */

    if (!old_lsp) {
	relative_age = NEW_NEWER;
    } else {

	/* Otherwise, call the omnibus comparator. */

	old_cksum = GETSHORT(old_lsp->data->checksum);
	new_cksum = GETSHORT(new_lsp->data->checksum);

	relative_age = isis_compare_age(old_seq, new_seq,
					old_lsp->lsp_active,
					new_lsp->lsp_active,
					old_cksum, new_cksum);
	/*
	 * Screen for *locally generated* LSPs which are identical
	 * to a previously installed LSP.
	 *
	 * NOTE: %%% Restrict this behavior to NLSP for now, and only
	 *       %%% when LSP data is unpacked (hashed). Someday this
	 *       %%% may want to change.
	 */

	if ((pdb->proc_type == PROC_TYPE_NLSP) && pdb->lsp_hash_enable) {
	    if (new_lsp->lsp_suppress && !old_lsp->lsp_force_insert) {
		if (isis_lsp_no_change(old_lsp, new_lsp, level, pdb)) {
		    update_pbuginf
			(pdb, "No change, suppress L%d LSP %s, seq %x",
			 level, lspid_string(&new_lsp->data->lspid), 
			 GETLONG(&new_lsp->data->sequence_number));
		    isis_free_lsp(pdb, new_lsp); /* %%% */
		    return;
		}
	    }
	}
	new_lsp->lsp_suppress = FALSE;
    }

    /* Now do the right thing based on the relative age. */
    
    switch (relative_age) {

	/*
	 * Received LSP is either not in the database or is newer. An LSP
	 * with HoldTime of 0 with the same sequence number as the stored LSP
	 * (with non-zero HoldTime) is considered new. This logic also
	 * processes locally originated LSPs (identified when !idb).
	 */

      case NEW_NEWER:

	/*
	 * Copy interesting fields from the old LSP (if any) to the new.
	 * Explicitly mem_lock all of the parent adjacencies, so that when
	 * we free the old LSP the refcount will end up the same.
	 */

	if (old_lsp) {
	    sys_timestamp gen_limit_expiry;
	    gen_limit_expiry = mgd_timer_exp_time(&old_lsp->gen_limit_timer);
	    mgd_timer_stop(&old_lsp->gen_limit_timer);
	    bcopy(old_lsp->copy_start, new_lsp->copy_start,
		  old_lsp->copy_end - old_lsp->copy_start);
	    if (TIMER_RUNNING(gen_limit_expiry)) {
		mgd_timer_set_exptime(&new_lsp->gen_limit_timer,
				      &gen_limit_expiry);
	    }
	    for (parent_index = 0; parent_index < new_lsp->parent_count;
		 parent_index++) {
		if (new_lsp->parents[parent_index].next_hop) {
		    mem_lock(new_lsp->parents[parent_index].next_hop);
		}
	    }

	} else {

	    /*
	     * Brand new LSP.  Allocate an index; if we never use it, it
	     * remains free.
	     */
	    new_lsp->lsp_index = isis_alloc_lspix(pdb);
	    if (!new_lsp->lsp_index) {	/* Out of memory! */
		isis_free_lsp(pdb, new_lsp);
		return;
	    }
	}

	/*
	 * Never set SRM bits for an LSP with sequence number 0.
	 */
	if (!new_seq) {
	    isis_avl_insert(pdb, h, key, new_lsp, element);
	    return;
	}

	/*
	 * Originating a new LSP.  Set SRM on all interfaces (SSN is clear).
	 * If any PSNP timers are running, they'll eventually expire and do
	 * nothing--this is cheaper than looking to find them all.  If
	 * the generation limit timestamp isn't set, set it now.  If
	 * it is set and we're in the limit period, set the timer.  This
	 * causes the LSP send routines to refuse to send the LSP.
	 */
	if (!idb) {			/* Local LSP */
	    isis_set_all_srm(pdb, new_lsp, NULL);

	    /* Start refresh timer.  Create it if necessary. */

	    if (!new_lsp->lsp_refresh_timer) {
		new_lsp->lsp_refresh_timer = malloc_named(sizeof(mgd_timer),
							  "LSP refresh timer");
		if (new_lsp->lsp_refresh_timer) {
		    mgd_timer_init_leaf(new_lsp->lsp_refresh_timer,
					&pdb->lsp_timers,
					ISIS_LSP_REFRESH_TIMER,
					new_lsp, FALSE);
		}
	    }
	    if (new_lsp->lsp_refresh_timer) {
		mgd_timer_start_jittered(new_lsp->lsp_refresh_timer,
					 isis_lsp_refresh_interval(pdb),
					 ISIS_JITTER_PERCENT);
	    }

	    if (!TIMER_RUNNING(new_lsp->next_gen) ||
		AWAKE(new_lsp->next_gen)) {
		TIMER_START(new_lsp->next_gen,
			    pdb->lsp_gen_interval[level - 1]);
	    } else {			/* Uhoh */
		if (!mgd_timer_running(&new_lsp->gen_limit_timer)) {
		    mgd_timer_set_exptime(&new_lsp->gen_limit_timer,
					  &new_lsp->next_gen);
		    update_pbuginf(pdb,
				   "Rate limiting L%d LSP %s, seq %x",
				   level, lspid_string(&new_lsp->data->lspid), 
				   GETLONG(&new_lsp->data->sequence_number));
		}
	    }
	} else {			/* idb != NULL; not local LSP */

	    /* Not originating;  rather, we received it. */

	    update_pbuginf(pdb, "LSP newer than database copy");

	    /*
	     * If an LSP for this system was received, originate new
	     * one after overwriting the existing sequence number.
	     * This will cause the sequence number to be incremented.
	     * Never overwrite the sequence number of a dummy LSP.
	     */
	    if (this_system) {
		if (old_lsp && !isis_dummy_lsp(old_lsp)) {
		    PUTLONG(&old_lsp->data->sequence_number, new_seq);

		    /*
		     * Do not suppress these, even if lsp-data-hashing
		     * is in effect. The lsp may *appear* to be unchanged
		     * but it must be flooded anyway.
		     */
		    old_lsp->lsp_force_insert = TRUE; /* %%% */

		    /*
		     * Wipe the rexmit bits, since they are invalidated by
		     * the change in the sequence number.
		     */
 		    memset(old_lsp->on_rexmit_queue.bits, 0,
			   old_lsp->on_rexmit_queue.bitfield_size /
			   BITMAP_SIZE);
  
		    if (isis_regenerate_lsp(pdb, new_lsp, level)) {
			update_pbuginf(pdb, "Regenerated LSP");
			isis_free_lsp(pdb, new_lsp);
			return;
		    }
		}

		/*
		 * Couldn't regenerate our LSP for one reason or another.
		 * Install this one, purged.
		 */
		update_pbuginf(pdb, "Couldn't regenerate LSP, purging");
		isis_purge_lsp(pdb, new_lsp, level, pdb->lsp_lifetime);
		isis_avl_insert(pdb, h, key, new_lsp, element);
		return;
	    }
		
	    /*
	     * Set SRM for all circuits.  We may clear SRM for the received
	     * circuit below.  All SSN bits are already clear.  We may set SSN
  	     * for the received circuit below.  We don't worry about
  	     * PSNP timers running; they'll expire and do nothing if their
  	     * SSN bits are clear.
	     *
	     * Don't set SRM for a purged (HT=0) LSP that didn't previously
	     * exist (now officially known as a "rogue purge".)
	     */
	    rogue_purge = (!old_lsp && !new_lsp->lsp_active);

	    if (!rogue_purge) {
		isis_set_all_srm(pdb, new_lsp, isisidb);
	    }

	    /*
	     * If the sequence number matches an LSP in the database
	     * but the checksums don't match, purge the LSP by setting
	     * its hold time to zero.
	     */
	    if (old_lsp) {
		if (old_seq == new_seq && old_seq &&
		    GETSHORT(new_lsp->data->checksum) != 
		    GETSHORT(old_lsp->data->checksum)) {
		    if (new_lsp->lsp_active) {
			isis_mark_lsp_purged(new_lsp, pdb->lsp_lifetime);
		    }
		}
	    }

	    /*
	     * For rogue LSP purges and non-purges, clear SRM on this circuit
	     * and set SSN (for P2P links).  For non-rogue purges, all SRMs
	     * will already be set and all SSNs will already be clear.
	     * (Actually, no SRM bits will be set for the rogue purge, but
	     * we'll waste an extra few cycles here.  Tant pis.)
	     */
	    if (new_lsp->lsp_active || rogue_purge) {
 		bitfield_clear(isisidb->isis_if_number, &new_lsp->SRM);
  		if (is_p2p(idb)) {
 		    bitfield_set(isisidb->isis_if_number, &new_lsp->SSN);
		    isis_schedule_psnp(pdb, idb);
  		}
  	    }
  	}

	/*
	 * Determine whether we need to do a full SPF, a partial update,
	 * or nothing, before we install the new LSP (since it will free
	 * the old one).
	 */
	trigger_type = 0;

	/* 
	 * If we see a purge LSP for an LSP that is not in our data base
	 * (i.e. 'rogue_purge' is TRUE), do not bother running SPF. 
	 */
	if (rogue_purge) {
	    update_pbuginf(pdb, "Purge LSP received for an LSP not in "
			   "data base");
	    check_result = SPF_NO_CHANGE;
	}
	else {
	    check_result = isis_check_spf(pdb, old_lsp, new_lsp, level,
				          &trigger_type);
	}

	/*
	 * Install new LSP in database.
	 */
	isis_avl_insert(pdb, h, key, new_lsp, element);

	/*
	 * Install an entry for this LSP in the FastPSNP cache.
	 */
	isis_populate_fastpsnp_entry(pdb, new_lsp->lsp_index,
				     new_lsp->data->checksum);

	/*
	 * For NLSP, possibly install the hostname from this LSP in the
	 * name cache.
	 */
	if (pdb->proc_type == PROC_TYPE_NLSP) {
	    reg_invoke_ipx_nlsp_cache_hostname(pdb, new_lsp, tlv_parse);
	}
	
	/* Now take care of what kind of update we need to do. */

	switch (check_result) {

	  case SPF_NO_CHANGE:
	    update_pbuginf(pdb, "No change");
	    break;

	  case SPF_PARTIAL:
	    isis_mark_lsp_to_recalculate(pdb, new_lsp->lsp_index);
	    update_pbuginf(pdb, "Leaf routes changed");
	    break;

	  case SPF_FULL:
	    isis_trigger_spf(pdb, level, trigger_type,
			     &new_lsp->data->lspid, FALSE);
	    update_pbuginf(pdb, "Full SPF required");
	    break;
	}
	break;

      case SAME_AGE:

	/*
	 * New LSP is same as database copy.
	 */
	if (idb) {
	    update_pbuginf(pdb, "LSP same as database copy");
 	    bitfield_clear(isisidb->isis_if_number, &old_lsp->SRM);
  	    if (is_p2p(idb)) {
 		bitfield_set(isisidb->isis_if_number, &old_lsp->SSN);
		isis_schedule_psnp(pdb, idb);
	    }
	}
	isis_free_lsp(pdb, new_lsp);
	break;

      case OLD_NEWER:
	/*
	 * Received LSP is older than database copy.
	 */
	if (idb) {
	    update_pbuginf(pdb, "LSP older than database copy");
	    isis_set_srm(pdb, idb, old_lsp);
	    bitfield_clear(isisidb->isis_if_number, &old_lsp->SSN);
	}
	isis_free_lsp(pdb, new_lsp);
	break;

    }
}

/*
 * isis_link_lsp
 *
 * Install an LSP in the database.  If idb is NULL, the LSP is locally
 * generated;  otherwise it is received from another system.
 */
void isis_link_lsp (clns_pdbtype *pdb, isis_lspdbtype *lsp_db,
		    int level, idbtype *idb, isis_TLV_parse *tlv_parse)
{

    isis_tree_type **element;
    int            h;

    element = (level == ISIS_CIRCUIT_L1) ? &pdb->level1_LSPs :
	&pdb->level2_LSPs;
		
    isis_get_avl_element(&lsp_db->data->lspid, element, &h, 
			 isis_lsp_avl_insert, lsp_db, idb, level, pdb,
			 tlv_parse);
}

/*
 * isis_delink_lsp
 *
 */
void isis_delink_lsp (
    clns_pdbtype *pdb,
    isis_lspid *lspid,
    int level)
{

    isis_tree_type **element;
    int            h;

    element = (level == ISIS_CIRCUIT_L1) ? &pdb->level1_LSPs :
	&pdb->level2_LSPs;

    isis_lsp_avl_delete(pdb, lspid, element, &h);
}

/*
 * isis_validate_nlsp_lsp
 *
 * Validate an NLSP LSP.
 *
 * Returns TRUE if OK, FALSE if bad.
 *
 * A placeholder right now;  we'll add more stuff as we finish fixing
 * conformance test failures.
 *
 * A lot of what used to be here is now in isis_ipx.c.
 */
static boolean isis_validate_nlsp_lsp (clns_pdbtype *pdb, 
				       isis_TLV_parse *tlv_parse,
				       isis_LSP_hdrtype *isis)
{
    ushort ht;

    /* Punt if hold-time zero. */

    ht = GETSHORT(isis->ht);
    if (ht == 0)
	return(TRUE);

    return(TRUE);
}

/*
 * isis_rcv_lsp
 *
 */
void isis_rcv_lsp (clns_pdbtype *pdb, paktype *pak, int level)
{
    isis_LSP_hdrtype *isis;
    isis_lspdbtype *lsp_db, *lsp_in_lsdb;
    hwaddrtype   snpa;
    ulong        hold_time;
    int          total_tlv_length, db_length, copy_length;
    idbtype      *idb;
    isis_TLV_parse *tlv_parse;
    boolean lsp_valid;
    isisidbtype  *isisidb;
    boolean bad_checksum;
    ushort       old_checksum;

    isis = isis_pak_start(pak, pdb);
    idb = pak->if_input;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * Try to do a fast acknowledgment to save lots of the checks
     * done normally. This should save CPU when many copies of the
     * same LSP are received at about the same time via different
     * interfaces.
     */

    lsp_in_lsdb = isis_try_fastpsnp(pdb, level, pak, isis);
    if (lsp_in_lsdb) {
 	bitfield_clear(isisidb->isis_if_number, &lsp_in_lsdb->SRM);
  	if (is_p2p(idb)) {
 	    bitfield_set(isisidb->isis_if_number, &lsp_in_lsdb->SSN);
	    isis_schedule_psnp(pdb, idb);
	}
	/*
	 * Should we do a debug here ? Maybe not doing so will increase
	 * usability of "debug isis/nlsp update" ?
	 */
	return;
    }

    lsp_valid = TRUE;

    /* Parse the TLVs. */

    tlv_parse = isis_valid_packet(pak, isis, pdb);
    if (!tlv_parse)
	return;

    if (isis_nlsp_update_packet_debug[pdb->proc_type]) {
	(*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);
	buginf ("\n%s-Update: Rec L%d LSP %s, seq %x, ht %d,", 
		pdb->errmsg_name, level, 
		lspid_string(&isis->lspid),
		GETLONG(&isis->sequence_number), GETSHORT(isis->ht));
	buginf ("\n%s-Update: from SNPA %s (%s)", 
	       pdb->errmsg_name, snpa_string (&snpa), idb->namestring);
    }

    /* Check incoming level. */

    if (!(pdb->level & level) || 
	!(isisidb->isis_circuit_type & level)) {
	update_pbuginf(pdb, "level mismatch");
	goto exit_early;
    }

    if (!isis_valid_adjacency(pdb, pak, level)) {
	update_pbuginf(pdb, "Invalid adjacency");
	goto exit_early;
    }

    total_tlv_length = GETSHORT(isis->pdu_length) - ISIS_LSP_HEADER_SIZE;

    /* Validate fields for NLSP. */

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	lsp_valid = isis_validate_nlsp_lsp(pdb, tlv_parse, isis);
    }

    /* Punt if the LSP is broken. */

    if (!lsp_valid)
	goto exit_early;

    /*
     * Verify LSP authentication, if any.
     */
    if (pdb->proc_type == PROC_TYPE_ISIS) {
	if (!isis_check_auth ((isis_info_type *)isis->item, 
			      (char *)isis->item + total_tlv_length, pdb, 
			      NULL, level)) {
	    update_pbuginf(pdb, "LSP authentication failed");
	    goto exit_early;
	}
    }

    db_length = isis_lspdb_private_length() + ISIS_LSP_CORE_SIZE +
	total_tlv_length;

    /*
     * If LSP has previously originated by this system, create reserve
     * room in memory to add links.
     */
    if (station_match(pdb->systemId, isis->lspid.system_id) &&
	(pdb->packet_mtu + ISIS_LSPDB_OVERHEAD > db_length)) {
	lsp_db = isis_malloc_lspdb(pdb, pdb->packet_mtu + ISIS_LSPDB_OVERHEAD);
    } else {
	lsp_db = isis_malloc_lspdb(pdb, db_length);
    }
    if (!lsp_db)
	goto exit_early;

    mgd_timer_init_leaf(&lsp_db->lsp_timer, &pdb->lsp_timers, ISIS_LSP_TIMER,
			lsp_db, FALSE);
    lsp_db->db_length = db_length;
    hold_time = GETSHORT(isis->ht);
    lsp_db->level = level;
    copy_length = total_tlv_length + ISIS_LSP_CORE_SIZE;

    /*
     * In NLSP, if received checksum is 0, treat as if an LSP was received
     * with a hold time of 0.  For ISIS, we ignore these, since we flood
     * them that way;  a zero checksum is considered to be good.
     */
    if (pdb->proc_type == PROC_TYPE_NLSP && !GETSHORT(isis->checksum) &&
	hold_time) {
	hold_time = 0;
	isis_traffic[pdb->proc_type][ISIS_CKSUM_ERR]++;
    }

    /* 
     * Check the checksum.  ISIS says purge entries with bad checksums;
     * NLSP says to simply ignore them.  We allow the user to configure
     * the box to ignore them for ISIS as well, so we use our PDB variable.
     *
     * Checksums on purges do not need to be correct. See note 32 in 10589.
     * Cisco routers always recompute the checksum over the LSP header
     * when purging an LSP. Some implementations just keep the old
     * checksum in the header of the purged LSP. We used to complain
     * about this, but we should not have done that.
     *
     * Another way to handle this issue would be to zero out the checksum on
     * purges. Those LSPs are always accepted. Some other implementations
     * do this.
     */
    bad_checksum = FALSE;
    if (!verify_iso_checksum(&isis->lspid, copy_length, isis->checksum)) {
	if (hold_time == 0) {
	    isis_traffic[pdb->proc_type][ISIS_CKSUM_ERR_HT0]++;
	    PUTSHORT(isis->checksum, 0);
	} else {
	    isis_traffic[pdb->proc_type][ISIS_CKSUM_ERR]++;
	    update_pbuginf(pdb, "Checksum failed for LSP %s",
			   lspid_string(&isis->lspid));

	    old_checksum = GETSHORT(isis->checksum);
	    compute_iso_checksum(&isis->lspid, copy_length, isis->checksum);
	    errmsg(&msgsym(LSPCKSUM, CLNS), pdb->errmsg_name, level,
		   lspid_string(&isis->lspid), old_checksum,
		   pak->if_input->namestring, copy_length,
		   GETSHORT(isis->checksum),
		   isis_traffic[pdb->proc_type][ISIS_CKSUM_ERR]);
	    if (isis_nlsp_cksum_error_debug[pdb->proc_type]) {
		buginf("\n%s: LSP checksum failed for LSP %s, length %d, "
		       "checksum %04x, should be %04x", pdb->errmsg_name,
		       lspid_string(&isis->lspid), copy_length, old_checksum,
		       GETSHORT(isis->checksum));
		isis_dump_packet(pak);
	    }

	    bad_checksum = TRUE;
	    PUTSHORT(isis->checksum, 0);
	    hold_time = 0;
	    if (pdb->ignore_cksum_errs) {
		isis_free_lsp(pdb, lsp_db);
		goto exit_early;
	    }
	}
    }

    if (hold_time <= 1) {		/* Expired or about to be */
	lsp_db->lsp_active = FALSE;
	mgd_timer_start(&lsp_db->lsp_timer, pdb->lsp_lifetime);
    } else {
	hold_time--;			/* Decrement before flooding. */
	lsp_db->lsp_active = TRUE;
	mgd_timer_start(&lsp_db->lsp_timer, hold_time * ONESEC);
    }

    bcopy(&isis->lspid, &lsp_db->data, copy_length);
    if (pdb->proc_type == PROC_TYPE_NLSP) { /*Copy non-routing bit from hdr */
	lsp_db->non_routing = isis->non_routing;
    }

    /*
     * Attempt to store received LSP in database.
     */
    isis_link_lsp(pdb, lsp_db, level, idb, tlv_parse);

    /*
     * If the checksum was bad, purge the LSP (IS-IS only).
     */
    if (bad_checksum) {
	if (level == ISIS_CIRCUIT_L1) {
	    lsp_db = isis_find_L1_lsp(pdb, &isis->lspid);
	} else {
	    lsp_db = isis_find_L2_lsp(pdb, &isis->lspid);
	}
	if (lsp_db) {
	    isis_purge_lsp(pdb, lsp_db, level, pdb->lsp_lifetime);
	}
    }
  exit_early:
    free(tlv_parse);
}

/*
 * isis_snp_data_left
 *
 * Returns TRUE if there's some data left to parse.
 */
static inline boolean isis_snp_data_left (isis_csnp_parse_context *context)
{
    return((uchar *) context->item < context->packet_end);
}

/*
 * isis_snp_advance_item
 *
 * Advances to the next item.
 */
static void isis_snp_advance_item (isis_csnp_parse_context *context)
{
    context->item = context->item_end;
    ADVANCE_ITEM(context->item_end, TRUE);
}

/*
 * isis_snp_next_item
 *
 * Advance to the next valid item in the packet if the current item
 * isn't the right type.  Sets current_entry to the first entry of
 * the valid item.
 */
static void isis_snp_next_item (isis_csnp_parse_context *context)
{
    while (TRUE) {

	/* If we're done, bail. */
	
	if (!isis_snp_data_left(context))
	    return;
	
	/* If the current item is valid, we're done. */
	
	if (context->item->code == ISIS_SNP) {
	    context->current_entry = (isis_snp_entry *) context->item->value;
	    return;
	}

	/* Wrong type.  Advance. */

	isis_snp_advance_item(context);
    }
}

/*
 * isis_snp_next_entry
 *
 * Advance to the next SNP entry in the packet.  Returns the entry pointer,
 * or NULL if we're through.
 */
static isis_snp_entry *isis_snp_next_entry (isis_csnp_parse_context *context)
{
    while (TRUE) {

	/* If we're done, bail. */
	
	if (!isis_snp_data_left(context))
	    return(NULL);

	/*
	 * If first time in this item, use the first entry of the first
	 * valid item.
	 */
	if ((uchar *) context->current_entry <= (uchar *) context->item) {
	    isis_snp_next_item(context);
	} else {

	    /* Not the first time in the item.  Bump the pointer. */
	    
	    context->current_entry =
		(isis_snp_entry *) (((uchar *) context->current_entry) +
				    ISIS_SNP_ENTRY_LENGTH);
	}
	
	/* If this entry ends within the item, we're all set. */

	if (((uchar *) context->current_entry + ISIS_SNP_ENTRY_LENGTH) <=
	    (uchar *) context->item_end)
	    return(context->current_entry);

	/* Past the end.  Advance the item and go around again. */

	isis_snp_advance_item(context);
    }
}

/*
 * Build null LSP.
 */
static isis_lspdbtype *isis_build_null_lsp (clns_pdbtype *pdb, 
					    isis_lspid *lspid, int level)
{
    isis_lspdbtype *lsp_db;
    isis_lsp_core   *core;

    lsp_db = isis_malloc_lspdb(pdb, isis_lspdb_private_length() +
			       ISIS_LSP_CORE_SIZE);
    if (!lsp_db) return(NULL);

    mgd_timer_init_leaf(&lsp_db->lsp_timer, &pdb->lsp_timers, ISIS_LSP_TIMER,
			lsp_db, FALSE);
    mgd_timer_start(&lsp_db->lsp_timer, pdb->lsp_lifetime);
    lsp_db->lsp_active = TRUE;
    lsp_db->db_length = lsp_db->max_db_length;
    lsp_db->level = level;
    core = lsp_db->data;
    lspid_copy(lspid, &core->lspid);
    PUTLONG(&core->sequence_number,0);
    core->is_type = level;

    return(lsp_db);
}

/*
 * isis_install_dummy_lsps
 *
 * Install the queue of dummy LSPs built by isis_add_dummy_lsp
 */
static void isis_install_dummy_lsps(clns_pdbtype *pdb,
				    int level,
				    isis_lspdbtype *dummy_lsp)
{
    isis_tree_type **element;
    int            h;
    isis_lspdbtype *next_dummy;

    /* Flush out the queue of LSPs. */

    while (dummy_lsp) {
	next_dummy = dummy_lsp->next_lsp;
	dummy_lsp->next_lsp = NULL;
	h = 0;
	element = (level == ISIS_CIRCUIT_L1) ? &pdb->level1_LSPs :
	    &pdb->level2_LSPs;
	
	isis_get_avl_element(&dummy_lsp->data->lspid, element, &h, 
			     isis_lsp_avl_insert, dummy_lsp, NULL, level, pdb,
			     NULL);
	dummy_lsp = next_dummy;
    }
}

/*
 * isis_add_dummy_lsp
 *
 *    When we hear a SNP for a LSP we don't have - add a LSP with
 *    sequence number 0 and send a PSNP.  This will cause someone to
 *    send us the correct LSP.
 *
 *    We create them here but add them to a queue to be installed later--
 *    otherwise we confuse the tree walking we're probably doing right now.
 */
static void isis_add_dummy_lsp (clns_pdbtype *pdb, isis_lspid *lspid,
				idbtype *idb, int level,
				isis_lspdbtype **dummy_queue)
{
    isis_lspdbtype   *lsp_db;
    isisidbtype	     *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (idb && !isisidb) {
	return;
    }
    lsp_db = isis_build_null_lsp(pdb, lspid, level);
    if (!lsp_db)
	return;

    bitfield_set(isisidb->isis_if_number, &lsp_db->SSN);
    isis_schedule_psnp(pdb, idb);
    lsp_db->next_lsp = *dummy_queue;
    *dummy_queue = lsp_db;
}

/*
 * isis_no_lsp_for_snp
 *
 * Install a dummy LSP and send a PSNP for it if appropriate.
 */
static void isis_no_lsp_for_snp (clns_pdbtype *pdb, idbtype *idb,
				 isis_snp_entry *snp_entry, int level,
				 isis_lspdbtype **dummy_queue)
{
    /*
     * Only do this if the SNP entry has nonzero lifetime, checksum,
     * and sequence number.
     */
    if (GETSHORT(snp_entry->lifetime) &&
	GETSHORT(snp_entry->checksum) &&
	GETLONG(&snp_entry->sequence_number)) {
	snp_buginf(pdb, "Entry %s, seq %x not in LSP database, adding dummy",
		   lspid_string(&snp_entry->lspid),
		   GETLONG(&snp_entry->sequence_number));
	isis_add_dummy_lsp(pdb, &snp_entry->lspid, idb, level, dummy_queue);
    }
}

/*
 * isis_compare_lsp_snp
 * 
 * Compare an LSP and an SNP entry, and do the right thing with the result.
 */
static void isis_compare_lsp_snp (clns_pdbtype *pdb, idbtype *idb,
				  isisidbtype *isisidb,
				  isis_lspdbtype *lsp_db,
				  isis_snp_entry *snp_entry,
				  boolean serial)
{
    /* Compare the ages and do the right thing. */
    
    switch (isis_compare_age(GETLONG(&lsp_db->data->sequence_number),
			     GETLONG(&snp_entry->sequence_number),
			     lsp_db->lsp_active,
			     (GETSHORT(snp_entry->lifetime) != 0),
			     GETSHORT(lsp_db->data->checksum),
			     GETSHORT(snp_entry->checksum))) {
      case SAME_AGE:
	
	/* Synchronized - stop sending this LSP on this interface. */
	
	if (serial)
	    bitfield_clear(isisidb->isis_if_number, &lsp_db->SRM);
	snp_buginf(pdb, "Same entry %s, seq %x",
		   lspid_string(&snp_entry->lspid),
		   GETLONG(&snp_entry->sequence_number));
	break;
	
      case NEW_OLDER:
	
	/* They have an older version - update them. */
	
	bitfield_clear(isisidb->isis_if_number, &lsp_db->SSN);
	isis_set_srm(pdb, idb, lsp_db);
	snp_buginf(pdb, "Entry %s, seq %x is older than ours "
		   "(seq %x), flooding",
		   lspid_string(&snp_entry->lspid),
		   GETLONG(&snp_entry->sequence_number),
		   GETLONG(&lsp_db->data->sequence_number));
	break;
	
      case NEW_NEWER:
	
	/* They have a new version - ask them to send it. */
	
	bitfield_set(isisidb->isis_if_number, &lsp_db->SSN);
	isis_schedule_psnp(pdb, idb);
	if (serial)
	    bitfield_clear(isisidb->isis_if_number, &lsp_db->SRM);
	snp_buginf(pdb, "Entry %s, seq %x is newer than ours "
		   "(seq %x), sending PSNP",
		   lspid_string(&snp_entry->lspid),
		   GETLONG(&snp_entry->sequence_number),
		   GETLONG(&lsp_db->data->sequence_number));
	break;
    }
}

/* 
 * isis_rw_process_csnp
 *
 *   This routine is called at the bottom of a recursive walk.
 *   It will compare the current SNP entry with the specified LSP.
 *
 *   The parse context is updated.
 */
static boolean isis_rw_process_csnp (isis_tree_type *element,
				     void *context_parm, void *pdb_parm,
				     void *queue_parm, void *dummy)
{
    isis_CSNP_hdrtype *csnp;
    isis_lspdbtype   *lsp_db;
    isis_snp_entry *snp_entry;
    idbtype        *idb;
    isis_csnp_parse_context *context;
    clns_pdbtype *pdb;
    isis_lspdbtype **dummy_queue;

    context = context_parm;
    pdb = pdb_parm;
    dummy_queue = queue_parm;

    idb = context->idb;
    csnp = context->csnp_pkt;
    lsp_db = element->lsp;

    /*
     * If we're before the start of the range, continue the traversal.
     * If we're past the end of the range, bail out.
     */
    if (isis_match_lspid(&element->lspid, &csnp->start_lsp) < 0) {
	return(TRUE);		/* Too early */
    }
    if (isis_match_lspid(&element->lspid, &csnp->end_lsp) > 0) {
	return(FALSE);		/* Too late */
    }

    while (TRUE) {
	
	snp_entry = context->current_entry;

	/*
	 * Check for end of CSNP packet.  Set SRM for any LSP database
	 * entries that are within the range of the CSNP.  If we're past
	 * the end of the range, terminate the traversal.
	 *
	 * The calling routine handles the case where the database entries
	 * run out before the CSNP entries.
	 */
	if (!isis_snp_data_left(context)) {
	    if (isis_match_lspid(&element->lspid, &csnp->end_lsp) <= 0) {
		
		/* In range.  Flood it. */
		
		if (lsp_db->lsp_active && !isis_dummy_lsp(lsp_db)) {
		    isis_set_srm(pdb, idb, lsp_db);
		    snp_buginf(pdb, "LSP not in CSNP, flooding %s, seq %x",
			       lspid_string(&lsp_db->data->lspid),
			       GETLONG(&lsp_db->data->sequence_number));
		}
		return (TRUE);
	    } else {
		
		/* Past the end.  Hang it up. */
		
		return(FALSE);
	    }
	}
	
	/*
	 * OK, we've got a CSNP and an LSPID and we're in range.  Compare
	 * the LSPID to the current SNP value and Do The Right Thing.
	 */
	switch (isis_match_lspid(&element->lspid, &snp_entry->lspid)) {
	    
	  case -1:   /* Database LSPID less than packet LSPID. */
	    
	    /*
	     * Our neighbor does NOT have one of the LSPs that we have.
	     * Set SRM for the LSP so we can send it.
	     */
	    if (lsp_db->lsp_active && !isis_dummy_lsp(lsp_db)) {
		isis_set_srm(pdb, idb, lsp_db);
		snp_buginf(pdb, "LSP not in CSNP (next entry %s, seq %x), "
			   "flooding %s, seq %x",
			   lspid_string(&snp_entry->lspid),
			   GETLONG(&snp_entry->sequence_number),
			   lspid_string(&lsp_db->data->lspid),
			   GETLONG(&lsp_db->data->sequence_number));
	    }
	    return(TRUE);
	    
	  case 0:   /* Database LSPID is equal to packet LSPID. */

	    /* Compare the LSP to the SNP and do the right thing. */

	    isis_compare_lsp_snp(pdb, idb, context->isisidb, lsp_db, snp_entry,
				 context->serial);

	    /*
	     * Update the pointer to the next SNP entry.
	     */
	    isis_snp_next_entry(context);
	    return(TRUE);		/* Move on to the next LSP. */
	    
	  case 1:    /* Database LSPID is greater than packet LSPID. */
	    
	    /*
	     * Our neighbor has an LSP that we DON'T have. Insert a
	     * dummy lsp and ask him for the real one.
	     */
	    isis_no_lsp_for_snp(pdb, idb, snp_entry, context->level,
				dummy_queue);
	    break;
	}
	
	/*
	 * Update the pointer to the next SNP, and then go around again
	 * with the same LSP.
	 */
	isis_snp_next_entry(context);
    }					/* while(TRUE) */
}

/*
 * isis_rcv_csnp
 *
 * Process a CSNP packet.  We do a linear scan of the LSP database,
 * comparing it to the contents of the CSNP.  This scales as O(N).
 */
void isis_rcv_csnp (clns_pdbtype *pdb, paktype *pak, int level)
{
    isis_CSNP_hdrtype *isis;
    isis_info_type    *snp_item;
    uchar	      *packet_end;
    idbtype           *idb;
    ulong             length;
    boolean           serial;
    isis_TLV_parse    *tlv_parse;
    isisidbtype       *isisidb;
    isis_csnp_parse_context context;
    isis_tree_type    *lsp_tree;
    isis_lspdbtype    *dummy_queue;
    
    dummy_queue = NULL;

    /* Get various interesting pointers. */

    isis = isis_pak_start(pak, pdb);
    idb = pak->if_input;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (isis_nlsp_snp_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-SNP: Rec L%d CSNP from %s (%s)", pdb->errmsg_name,
	       level, station_string(isis->source_id), idb->namestring);
    }

    /* Validate the packet. */

    tlv_parse = isis_valid_packet(pak, isis, pdb);
    if (!tlv_parse)
	return;
    free(tlv_parse);			/* We don't use it here */

    if (!(pdb->level & level) || 
	!(isisidb->isis_circuit_type & level)) {
	update_pbuginf(pdb, "Level mismatch");
	return;
    }

    serial = is_p2p(idb);

    /*
     * Accept CSNPs from non-fully adjacent neighbor on point-to-point links.
     * This allows the neighbor to avoid flooding LSPs if not necessary.
     */
    if (!serial && !isis_valid_adjacency (pdb, pak, level)) {
	update_pbuginf(pdb, "Invalid adjacency");
	return;
    }

    /* 
     * Check if CSNP from DR, if not ignore.
     */
    if (!serial &&
	!station_match(isis->source_id, (level == ISIS_CIRCUIT_L1) ?
		       isisidb->isis_L1_dr_id : 
		       isisidb->isis_L2_dr_id)) {
	if (isis_nlsp_snp_debug[pdb->proc_type] ||
	    isis_nlsp_update_packet_debug[pdb->proc_type]) {
	    buginf("\n%s-SNP: CSNP discarded--received from non-DR",
		   pdb->errmsg_name);
	}
	return;
    }
    
    length = GETSHORT(isis->pdu_length);
    snp_item = (isis_info_type *)isis->item;
    snp_buginf(pdb, "CSNP range %s to %s", lspid_string(&isis->start_lsp),
	       lspid_string(&isis->end_lsp));

    /* Set up the context block. */

    context.idb = idb;
    context.isisidb = isisidb;
    context.csnp_pkt = isis;
    context.serial = serial;
    context.level = level;
    context.item = snp_item;
    context.item_end = snp_item;
    ADVANCE_ITEM(context.item_end, TRUE); /* Point past end of current item */
    context.current_entry = (isis_snp_entry *) isis; /* Gotta be before item */
    packet_end = (char *)isis + length;
    context.packet_end = packet_end;

    /* Advance to the first item. */

    isis_snp_next_entry(&context);

    /* Now walk the LSP database. */

    if (level == ISIS_CIRCUIT_L1) {
	lsp_tree = pdb->level1_LSPs;
    } else {
	lsp_tree = pdb->level2_LSPs;
    }
    isis_recursive_walk(NULL, ISIS_WALK_ALL, lsp_tree, isis_rw_process_csnp,
			&context, pdb, &dummy_queue, NULL);

    /*
     * Check for end condition - we ran out of LSPs in database before the
     * SNP ran out of advertisements. Process the trailing advertisements.
     */
    while (isis_snp_data_left(&context)) {
	isis_no_lsp_for_snp(pdb, idb, context.current_entry, level,
			    &dummy_queue);
	isis_snp_next_entry(&context);
    }

    /* Now install all the created dummy entries. */

    isis_install_dummy_lsps(pdb, level, dummy_queue);
}

/*
 * isis_rcv_psnp
 *
 * Process a received PSNP packet.  We look up each individual entry
 * in the LSP database.  This scales as O(M log N).
 */
void isis_rcv_psnp (clns_pdbtype *pdb, paktype *pak, int level)
{
    isis_PSNP_hdrtype *psnp;
    isis_info_type    *snp_item, *item_end;
    isis_snp_entry    *snp_entry;
    uchar	      *packet_end;
    idbtype           *idb;
    ulong             length;
    boolean           serial;
    isis_TLV_parse    *tlv_parse;
    isisidbtype       *isisidb;
    isis_lspdbtype    *lsp_db;
    isis_lspdbtype    *dummy_queue;

    dummy_queue = NULL;

    /* Get various interesting pointers. */

    psnp = isis_pak_start(pak, pdb);
    idb = pak->if_input;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (isis_nlsp_snp_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-SNP: Rec L%d PSNP from %s (%s)", pdb->errmsg_name,
	       level, station_string(psnp->source_id), idb->namestring);
    }

    /* Validate the packet. */

    tlv_parse = isis_valid_packet(pak, psnp, pdb);
    if (!tlv_parse)
	return;
    free(tlv_parse);			/* We don't use it here */

    if (!(pdb->level & level) || 
	!(isisidb->isis_circuit_type & level)) {
	update_pbuginf(pdb, "Level mismatch");
	return;
    }

    serial = is_p2p(idb);
    length = GETSHORT(psnp->pdu_length);
    
    /*
     * Do not process PSNPs if received on LAN and not DR.
     */
    if (!serial && !isis_we_are_dr(pdb, idb, level)) {
	if (isis_nlsp_snp_debug[pdb->proc_type] ||
	    isis_nlsp_update_packet_debug[pdb->proc_type]) {
	    buginf("\n%s-SNP: PSNP discarded - I'm not the DR",
		   pdb->errmsg_name);
	}
	return;
    }
	    
    snp_item = (isis_info_type *)psnp->item;
    packet_end = (uchar *)psnp + length;

    /* Now walk the PSNP, looking for SNP entry items. */

    while ((uchar *) snp_item < packet_end) {

	if (snp_item->code == ISIS_SNP) {

	    /* Found an item.  Walk the item until it is spent. */

	    item_end = snp_item;
	    ADVANCE_ITEM(item_end, TRUE); /* Get the end of the item */
	    snp_entry = (isis_snp_entry *) snp_item->value;
	    while ((uchar *) snp_entry < (uchar *) item_end) {
	
		if (isis_nlsp_snp_debug[pdb->proc_type] ||
		    isis_nlsp_update_packet_debug[pdb->proc_type]) {
		    buginf("\n%s-SNP: PSNP entry %s, seq %x, ht %d",
			   pdb->errmsg_name,
			   lspid_string(&snp_entry->lspid), 
			   GETLONG(&snp_entry->sequence_number), 
			   GETSHORT(snp_entry->lifetime));
		}

		/* Got an SNP entry.  Look up the corresponding LSP. */
		
		if (level == ISIS_CIRCUIT_L1) {
		    lsp_db = isis_find_L1_lsp(pdb, &snp_entry->lspid);
		} else {
		    lsp_db = isis_find_L2_lsp(pdb, &snp_entry->lspid);
		}
		
		/* If there's an LSP, compare it with the SNP and do the right
		 * thing.  If there isn't an LSP, create a dummy and request
		 * the real one as appropriate.
		 */
		if (lsp_db) {			/* We have the LSP */
		    isis_compare_lsp_snp(pdb, idb, isisidb, lsp_db, snp_entry,
					 serial);
		} else {				/* No LSP */
		    isis_no_lsp_for_snp(pdb, idb, snp_entry, level,
					&dummy_queue);
		}
		snp_entry = (isis_snp_entry *) (((uchar *) snp_entry) +
						ISIS_SNP_ENTRY_LENGTH);
	    }
	}

	/* Advance to the next item. */

	ADVANCE_ITEM(snp_item, TRUE);
    }

    /* Install any dummy LSPs built. */

    isis_install_dummy_lsps(pdb, level, dummy_queue);
}

/*
 * isis_valid_adjacency
 *
 * Determine if adjacency is valid for received PDU.
 */
boolean isis_valid_adjacency (clns_pdbtype *pdb, paktype *pak, int level)
{

    idbtype      *idb;
    clns_adjtype *adj, *cur_adj;
    hwaddrtype   snpa;
    int          bucket;
    
    idb = pak->if_input;
    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);

    /*
     * Scan through adjacency database to find adjacency by matching
     * interface and SNPA. Validate that the adjacency is the appropriate
     * level for received PDU.
     */
    adj = NULL;
    cur_adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, idb, 0, cur_adj, 
				&bucket))) {
	cur_adj = adj;  /* Save a copy of where we are in the adj table */
	for (; adj; adj = adj->next_alias) {
	    if (adj->state != CLNS_ADJ_UP_STATE)
		continue;
	    if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
		continue;
	    if (is_p2p(idb) ||
		!bcmp (adj->snpa.addr, snpa.addr, snpa.length)) {
		if (level == ISIS_CIRCUIT_L2 && 
		    adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) return (TRUE);
		
		if (level == ISIS_CIRCUIT_L1) {
		    if (adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE ||
			adj->adjacencyType == CLNS_NLSP_IS_ADJ_TYPE ||
			(adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE && 
			 !adj->L2only)) { 
			return (TRUE);
		    }
		}
	    }
	}
    }
    return (FALSE);
}

/*
 * isis_schedule_lsp_build
 *
 * Schedule an LSP to be built after a delay.  This is used to reduce the
 * number of times the LSPs are built in the face of things like mass
 * numbers of route redistributions.
 *
 * The LSP in question is scheduled to be rebuilt after the specified
 * delay if the timer isn't already running, or if we've been explicitly
 * told to.
 */
void isis_schedule_lsp_build (clns_pdbtype *pdb, int level, ulong delay,
			      boolean restart_timer)
{
    mgd_timer *timer;

    if (level == CLNS_LEVEL1) {
	timer = &pdb->L1_lsp_build_timer;
    } else {
	timer = &pdb->L2_lsp_build_timer;
    }

    if (restart_timer || !mgd_timer_running(timer)) {
	mgd_timer_start(timer, delay);
    }
}

/*
 * isis_mark_L1_changed_delayed
 *
 * Mark level 1 as being changed so that we will regenerate our LSPs after
 * a delay.
 */
void isis_mark_L1_changed_delayed (clns_pdbtype *pdb)
{
    SAVE_CALLER();
    locupdate_buginf(pdb, "L1 delayed changed from %#x", caller());
    isis_schedule_lsp_build(pdb, CLNS_LEVEL1, ISIS_LSP_BUILD_DELAY, FALSE);
}

/*
 * isis_mark_L2_changed_delayed
 *
 * Mark level 2 as being changed so that we will regenerate our LSPs after
 * a delay.
 */
void isis_mark_L2_changed_delayed (clns_pdbtype *pdb)
{
    SAVE_CALLER();
    locupdate_buginf(pdb, "L2 delayed changed from %#x", caller());
    isis_schedule_lsp_build(pdb, CLNS_LEVEL2, ISIS_LSP_BUILD_DELAY, FALSE);
}

/*
 * isis_mark_L1_changed
 *
 * Mark level 1 as being changed so that we will regenerate our LSPs.
 * Don't touch it if a delayed regeneration is in effect.
 */
void isis_mark_L1_changed (clns_pdbtype *pdb)
{
    SAVE_CALLER();
    if (!mgd_timer_running(&pdb->L1_lsp_build_timer)) {
	pdb->L1_changed = TRUE;
	isis_trigger_update_process(pdb);
	locupdate_buginf(pdb, "L1 changed from %#x", caller());
    }
}

/*
 * isis_mark_L2_changed
 *
 * Mark level 2 as being changed so that we will regenerate our LSPs.
 * Don't touch it if a delayed regeneration is in effect.
 */
void isis_mark_L2_changed (clns_pdbtype *pdb)
{
    SAVE_CALLER();
    if (!mgd_timer_running(&pdb->L2_lsp_build_timer)) {
	pdb->L2_changed = TRUE;
	isis_trigger_update_process(pdb);
	locupdate_buginf(pdb, "L2 changed from %#x", caller());
    }
}

/*
 * isis_mark_L1_dr_changed
 *
 * Mark level 1 DR as being changed so that we will regenerate our LSPs.
 */
void isis_mark_L1_dr_changed (clns_pdbtype *pdb, isisidbtype *isisidb)
{
    SAVE_CALLER();
    isisidb->isis_L1_dr_changed = TRUE;
    isis_trigger_update_process(pdb);
    locupdate_buginf(pdb, "L1 DR changed from %#x", caller());
}

/*
 * isis_mark_L2_dr_changed
 *
 * Mark level 2 DR as being changed so that we will regenerate our LSPs.
 */
void isis_mark_L2_dr_changed (clns_pdbtype *pdb, isisidbtype *isisidb)
{
    SAVE_CALLER();
    isisidb->isis_L2_dr_changed = TRUE;
    isis_trigger_update_process(pdb);
    locupdate_buginf(pdb, "L2 DR changed from %#x", caller());
}

/*
 * isis_trigger_partial_route_calculation
 *
 * Trigger a partial route calculation.
 */
void isis_trigger_partial_route_calculation (clns_pdbtype *pdb)
{
    if (!pdb->some_lsps_changed && !mgd_timer_running(&pdb->L1_prc_timer)) {
 	mgd_timer_start(&pdb->L1_prc_timer,
 			min(pdb->prc_interval[0],
 			    ISIS_NLSP_SPF_INITIAL_WAIT));
    }
    pdb->some_lsps_changed = TRUE;
    
    /* PRC logging is TBS */
}

/*
 * isis_trigger_spf
 *
 * Trigger an SPF run at the specified level.  Log the triggering event.
 * Issue a debug message if requested.
 */
static char *const trigger_strings[ISIS_SPF_TRIG_NUMTYPES][2] = {
    {"periodic SPF", "PERIODIC"},
    {"new system ID", "NEWSYSID"},
    {"new area", "NEWAREA"},
    {"new level", "NEWLEVEL"},
    {"routes cleared", "RTCLEARED"},
    {"new metric", "NEWMETRIC"},
    {"IP backup", "IPBACKUP"},
    {"IP query", "IPQUERY"},
    {"IPX backup", "IPXBACKUP"},
    {"IPX query", "IPXQUERY"},
    {"L2 attach changed", "ATTACHFLAG"},
    {"admin distance changed", "ADMINDIST"},
    {"area set changed", "AREASET"},
    {"new adjacency", "NEWADJ"},
    {"database changed", "DBCHANGED"},
    {"backup overflow entry found", "BACKUPOVFL"},
    {"new LSP 0", "NEWLSP"},
    {"LSP expired", "LSPEXPIRED"},
    {"LSP header fields changed", "LSPHEADER"},
    {"TLV code mismatch", "TLVCODE"},
    {"TLV contents changed", "TLVCONTENT"}
};

void isis_trigger_spf (clns_pdbtype *pdb, int level, int trigger_type,
		       isis_lspid *lspid, boolean generate_debug)
{
    isis_spf_log *log;
    isis_spf_log_entry *log_entry;

    /* Set the SPF flag and maybe kick the holddown timer. */

    if (level == CLNS_LEVEL1) {
	if (!pdb->run_L1_spf && !mgd_timer_running(&pdb->L1_spf_timer)) {
	    mgd_timer_start(&pdb->L1_spf_timer,
			    min(pdb->spf_interval[0],
				ISIS_NLSP_SPF_INITIAL_WAIT));
	}
	pdb->run_L1_spf = TRUE;
	log = pdb->L1_spf_log;
    } else {

	/*
	 * Let level-2 lag a bit so that it doesn't run before level 1
	 * if both are triggered at the same time.
	 */
	if (!pdb->run_L2_spf && !mgd_timer_running(&pdb->L2_spf_timer)) {
	    mgd_timer_start(&pdb->L2_spf_timer,
			    min(pdb->spf_interval[1],
				ISIS_NLSP_SPF_INITIAL_WAIT) + 1);
	}
	pdb->run_L2_spf = TRUE;
	log = pdb->L2_spf_log;
    }

    if (trigger_type >= ISIS_SPF_TRIG_NUMTYPES)	/* Bad trigger type */
	return;

    /* Generate the debug message, if asked. */

    if (generate_debug) {
	spf_tbuginf(pdb, "L%d, %s", level, trigger_strings[trigger_type][0]);
    }

    /* Log the event. */

    if (log) {
	log_entry = &log->log_entry[log->log_index];
	log_entry->trigger_type |= (1 << trigger_type);
	log_entry->trigger_count++;
	if (lspid) {
	    lspid_copy(lspid, &log_entry->lspid);
	}
    }
}

/*
 * isis_display_spf_log
 *
 * Display the SPF trigger log
 */
static void isis_display_spf_log (clns_pdbtype *pdb, int level,
				  isis_spf_log *log)
{
    isis_spf_log_entry *log_entry;
    int last_entry, current_entry;
    ulong trigger_number;
    char elapsed_time[20];

    if (log) {

	/* Walk the log. */

	last_entry = log->log_index;
	current_entry = last_entry + 1;
	printf("\n\n   Level %d SPF log", level);
	printf("\n  When   Duration  Nodes  Count     "
	       "Last trigger LSP   Triggers");
	while (TRUE) {
	    if (current_entry >= ISIS_SPF_LOG_SIZE)
		current_entry = 0;
	    log_entry = &log->log_entry[current_entry];

	    /* Print the entry. */

	    if (log_entry->trigger_count) {

		/*
		 * First, the time elapsed since the last SPF run, the
		 * duration, and the number of nodes seen.
		 */
		if (TIMER_RUNNING(log_entry->run_time)) {
		    sprint_dhms(elapsed_time, log_entry->run_time);
		    printf("\n%10s%-6d%-7d", elapsed_time,
			   log_entry->spf_duration, log_entry->node_count);
		} else {
		    printf("%c%23s",'\n', "(pending)");
		}

		/* Now the trigger count. */

		printf("%-7d  ", log_entry->trigger_count);

		/*
		 * If we know the last LSP that trigger this, print it.
		 * All triggers larger then ISIS_SPF_TRIG_SAVED_LSPIDS
		 * have filled in the lspid field in the log.
		 */

		if (log_entry->trigger_type >= ISIS_SPF_TRIG_SAVES_LSPID) {
		    printf("%-20s ", lspid_name_string(pdb,
						       &log_entry->lspid));
		} else {
		    printf("%-20s ", " ");
		}

		/* Now interpret the trigger bits. */

		for (trigger_number = 0;
		     trigger_number < ISIS_SPF_TRIG_NUMTYPES;
		     trigger_number++) {
		    if (log_entry->trigger_type & (1 << trigger_number)) {
			printf(" %s", trigger_strings[trigger_number][1]);
		    }
		}
	    }

	    /* Bail if done. */

	    if (current_entry == last_entry)
		break;

	    /* Go on. */

	    current_entry++;
	}
    }
}

/*
 * show_isis_spf_log
 *
 * Display the SPF trigger log.
 */
void show_isis_spf_log (parseinfo *csb)
{
    char *name;
    clns_pdbtype *pdb;

    switch (csb->which) {
      case SHOW_ISIS_SPFLOG:
	pdb = find_isis_pdb();
	break;

      case SHOW_NLSP_SPFLOG:
	name = GETOBJ(string,2);
	pdb = reg_invoke_ipx_find_nlsp_pdb(name);
	break;

      default:
	pdb = NULL;
	break;
    }
    if (!pdb)
	return;

    automore_enable(NULL);
    if (pdb->level & CLNS_LEVEL1) {
        isis_display_spf_log(pdb, CLNS_LEVEL1, pdb->L1_spf_log);
        automore_conditional(0);
    }
    if (pdb->level & CLNS_LEVEL2) {
        isis_display_spf_log(pdb, CLNS_LEVEL2, pdb->L2_spf_log);
    }
    automore_disable();
}

/*
 * isis_display_timer
 *
 * Callback to display a single timer
 */
static char *isis_display_timer (mgd_timer *timer, ushort type, char *buf)
{
    idbtype *idb;

    /* Don't say much if it's a parent. */

    if (mgd_timer_is_parent(timer))
	return("(Parent)");

    /* Break out based on the timer type. */

    switch (type) {
      case ISIS_L1_HELLO_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "L1 Hello (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_L2_HELLO_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "L2 Hello (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_DR_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "DR delay (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_ADJ_TIMER:
	return("Adjacency");
	break;

      case ISIS_PSNP_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "PSNP (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_L1_CSNP_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "L1 CSNP (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_L2_CSNP_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "L2 CSNP (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_AGE_TIMER:
	return("Ager");
	break;

      case ISIS_REXMIT_TIMER:
	return("LSP rexmit");
	break;

      case ISIS_LSP_PACING_TIMER:
	idb = mgd_timer_context(timer);
	if (validmem(idb)) {
	    sprintf(buf, "LSP pacing (%s)", idb->namestring);
	} else {
	    return("(Invalid)");
	}
	break;

      case ISIS_LSP_GEN_TIMER:
	return("LSP generation");
	break;

      case ISIS_LSP_TIMER:
	return("LSP lifetime");
	break;

      case ISIS_LSP_REFRESH_TIMER:
	return("LSP refresh");
	break;

      case ISIS_L1_LSP_BUILD_TIMER:
	return("L1 LSP build");
	break;

      case ISIS_L2_LSP_BUILD_TIMER:
	return("L2 LSP build");
	break;

      case ISIS_L1_SPF_TIMER:
	return("L1 SPF");
	break;

      case ISIS_L2_SPF_TIMER:
	return("L2 SPF");
	break;

      case ISIS_L1_PRC_TIMER:
	return("L1 PRC");
	break;

      case ISIS_DUPID_TIMER:
	return("Dup sysID detect");
	break;

      default:
	return("(Unknown type)");
	break;
    }
    return(buf);
}

/*
 * show_isis_mesh_groups
 *
 * Display mesh groups
 */
void show_isis_mesh_groups (parseinfo *csb)
{
    clns_pdbtype *pdb;
    switch (csb->which) {
      case SHOW_ISIS_MESHGRPS:
	pdb = find_isis_pdb();
	break;

      default:
	pdb = NULL;
	break;
    }
    if (!pdb)
	return;
    automore_enable(NULL);
    isis_display_mesh_groups(pdb);
    automore_disable();
}

/*
 * show_isis_timers
 *
 * Display the timer tree.
 */
void show_isis_timers (parseinfo *csb)
{
    clns_pdbtype *pdb;
    char *name;

    switch (csb->which) {
      case SHOW_ISIS_TIMERS:
	pdb = find_isis_pdb();
	break;

      case SHOW_NLSP_TIMERS:
	name = GETOBJ(string,2);
	pdb = reg_invoke_ipx_find_nlsp_pdb(name);
	break;

      default:
	pdb = NULL;
	break;
    }
    if (!pdb)
	return;

    printf("\n  Hello Process");
    automore_enable("    Expiration    Type");
    mgd_timer_display_tree(&pdb->timer, isis_display_timer);
    automore_disable();
    printf("\n\n  Update Process");
    automore_enable("    Expiration    Type");
    mgd_timer_display_tree(&pdb->update_timer, isis_display_timer);
    automore_disable();
}
