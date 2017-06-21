/* $Id: isis_update.c,v 3.5.26.11 1996/08/28 12:38:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_update.c,v $
 *------------------------------------------------------------------
 * isis_update.c -- IS-IS Update Process
 *
 * 24-Nov-1990, Paulina Knibbe
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_update.c,v $
 * Revision 3.5.26.11  1996/08/28  12:38:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.26.10  1996/08/12  01:07:05  dkatz
 * CSCdi45425:  NLSP may not report RIP/SAP routes
 * Branch: California_branch
 * Don't let the DR timer get in the way of reporting adjacencies (and
 * remove a couple of other checks for it).  Rebuild both non-pseudonode
 * and pseudonode LSPs when an interface changes state.
 *
 * Revision 3.5.26.9  1996/08/11  15:13:28  dkatz
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
 * Revision 3.5.26.8  1996/08/08  15:48:20  dkatz
 * CSCdi65479:  ISIS/NLSP mishandles purged packets
 * Branch: California_branch
 * Handle purged LSP flooding rules correctly (at last).  Keep purged
 * packets for a full LSP lifetime in all cases except when they expire
 * from natural causes.  Remove kludge that keeps SRM-set purged LSPs
 * in the database, since we now keep the LSPs long enough after purging.
 *
 * Revision 3.5.26.7  1996/08/08  14:50:56  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.5.26.6  1996/08/04  23:35:25  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.5.26.5  1996/08/02  02:47:58  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.5.26.4  1996/06/18  16:50:58  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.5.26.3  1996/05/26  06:40:46  dkatz
 * CSCdi58613:  ISIS/NLSP retransmission can get stuck
 * Branch: California_branch
 * Clear the RXM bits when changing the sequence number in place.
 *
 * Revision 3.5.26.2  1996/05/10  19:45:41  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.5.26.1  1996/03/18  19:04:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/13  01:13:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5  1996/01/18  21:48:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/12  23:01:06  dkatz
 * CSCdi45179:  ISIS LSPs get stuck after 24.8 days
 *
 * Revision 3.3  1995/11/17  08:54:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:52:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/23  22:48:45  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.7  1995/09/23  21:37:56  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.6  1995/08/25  15:24:30  dkatz
 * CSCdi38313:  ISIS may keep corrupted LSPs
 * Validate the LSP checksum before refreshing local LSPs.
 *
 * Revision 2.5  1995/08/25  04:59:20  dkatz
 * CSCdi37611:  ISIS SPF needs to suspend
 * Do the easy bits first--suspend between SPF runs, LSP generations, and
 * the like.
 *
 * Revision 2.4  1995/08/23  18:26:16  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.3  1995/08/23  18:03:24  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/06/09  12:59:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:19:08  hampton
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
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "mgd_timers.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_ipx.h"
#include "../clns/isis_inlines.h"
#include "../xns/ipx_registry.h"
#include "clns_registry.h"
#include "isis_registry.h"


/*
 * isis_trigger_update_process
 *
 * Wake up the update process.
 */
void isis_trigger_update_process (clns_pdbtype *pdb)
{
    if (pdb->update_wakeup) {
	process_set_boolean(pdb->update_wakeup, TRUE);
    }
}

/*
 * isis_lsp_timer_expiry
 *
 * Process an expired LSP timer.  This means that either the LSP has
 * just expired, or that its zero age lifetime has expired.
 *
 * If the LSP is to be purged but any SRM bits are still set, keep
 * it around for another zeroage_lifetime.  This keeps us from deleting
 * LSPs that haven't been sent (possible when there are a very large
 * number of them).
 */
static void isis_lsp_timer_expiry (clns_pdbtype *pdb, isis_lspdbtype *lsp)
{
    if (lsp->lsp_active) {		/* LSP expiring */
	update_pbuginf(pdb, "L%d LSP %s expired", 
		       lsp->level, lspid_string(&lsp->data->lspid));
	isis_purge_lsp(pdb, lsp, lsp->level, ISIS_ZEROAGE_LIFETIME);
	if (lsp->level == ISIS_CIRCUIT_L1) {
	    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_LSPEXPIRY,
			     &lsp->data->lspid, FALSE);
	} else {
	    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_LSPEXPIRY,
			     &lsp->data->lspid, FALSE);
	}
	spf_tbuginf(pdb, "L%d, LSP expiration", lsp->level);
    } else {				/* Zero-age lifetime expired */
	update_pbuginf(pdb, "L%d LSP %s deleted from database", 
		       lsp->level, lspid_string(&lsp->data->lspid));
	isis_delink_lsp(pdb, &lsp->data->lspid, lsp->level);
    }
}


/*
 * isis_refresh_one_lsp
 *
 * Refresh an LSP.  The sequence number is bumped, the holding time pushed
 * back up, and the refresh timer restarted.
 *
 * The checksum is validated before refreshing, to ensure that the data
 * wasn't corrupted in memory (since we recalculate the checksum when we
 * refresh).
 *
 * Returns TRUE if the checksum was valid and the LSP was regenerated;
 * false if the checksum failed (at which point the entire database is
 * blown up and all adjacencies are dropped).
 */
static boolean isis_refresh_one_lsp(clns_pdbtype *pdb, isis_lspdbtype *lsp)
{
    /* Bug out if for some reason there is no refresh timer. */

    if (!lsp->lsp_refresh_timer)
	return(TRUE);

    /* Sanity checks */

    if (!lsp->lsp_active ||
	!station_match(pdb->systemId, lsp->data->lspid.system_id)) {
	mgd_timer_stop(lsp->lsp_refresh_timer);	/* Be tidy. */
	return(TRUE);
    }

    update_pbuginf(pdb, "Refreshing L%d %s", lsp->level,
		   lspid_string(&lsp->data->lspid));
    if (!isis_verify_database_checksum(pdb, lsp))
	return(FALSE);
    isis_increment_seq(pdb, &lsp->data->sequence_number);

    /*
     * Wipe the rexmit bits, since they are invalidated by the change
     * in the sequence number.
     */
    memset(lsp->on_rexmit_queue.bits, 0,
	   lsp->on_rexmit_queue.bitfield_size / BITMAP_SIZE);
  
    mgd_timer_start(&lsp->lsp_timer, pdb->lsp_lifetime);
    mgd_timer_start_jittered(lsp->lsp_refresh_timer,
			     isis_lsp_refresh_interval(pdb),
			     ISIS_JITTER_PERCENT);
    isis_checksum_lsp(pdb, lsp);
    isis_set_all_srm(pdb, lsp, NULL);

    /* Keep track of how many LSPs we have refreshed */

    isis_traffic[pdb->proc_type][(lsp->level == ISIS_CIRCUIT_L1) ?
				 ISIS_L1_LSP_REFRESHED :
				 ISIS_L2_LSP_REFRESHED]++;

    /*
     * A lovely little kludge.  Stop the LSP generation limiter timestamp.
     * It is set when an LSP is generated and installed in the database;
     * if a network turns out to be extremely stable, the LSP may not
     * be regenerated for over 24.8 days, at which time the timer aliases and
     * life becomes uncomfortable.  This piece of code is the only one that
     * is guaranteed to be run regularly on all of our LSPs, so we take
     * this opportunity to zap the timer.  It isn't necessary to check it,
     * because the refresh timer gets restarted whenever the LSP is rebuilt
     * anyhow, so we are effectively guaranteed that the timer won't be
     * relevant right now.  (And even if we're wrong, the worst that can
     * happen is that the LSP might get sent twice too closely together.)
     */
    TIMER_STOP(lsp->next_gen);
    return(TRUE);
}

/*
 * isis_process_packets_inline
 *
 * Process all incoming packets.
 * We handle only a limited number of packets, to make sure
 * there is a nice interleaving between packets and timer events.
 * If we stop because there were no more packets to process
 * then we return TRUE, if we stop because we handled our limit
 * of packets, then return FALSE.
 */
static inline boolean isis_process_packets_inline (clns_pdbtype *pdb)
{
    int		limiter;
    paktype	*pak;
    isishdrtype	*isis;
    isisidbtype	*isisidb;

    for (limiter = 0; limiter < ISIS_PACKET_LIMITER; limiter++) {

	pak = process_dequeue(pdb->update_inputq);
	if (!pak) {
	    return(FALSE);
	}

	process_may_suspend();

	isis = isis_pak_start(pak, pdb);
	isisidb = get_isis_idb(pdb, pak->if_input);

	/* Only accept the packet if there's an ISIS IDB. */

	if (!isisidb) {
	    update_pbuginf(pdb, "No ISISIDB for packet on %s",
			   pak->if_input->namestring);
	} else {
	    switch (isis->type) {
	    case ISIS_L1_LSP:
		isis_traffic[pdb->proc_type][ISIS_L1_LSP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L1_LSP_IN]++;
		isis_rcv_lsp(pdb, pak, ISIS_CIRCUIT_L1);
		break;
	    case ISIS_L2_LSP:
		isis_traffic[pdb->proc_type][ISIS_L2_LSP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L2_LSP_IN]++;
		isis_rcv_lsp(pdb, pak, ISIS_CIRCUIT_L2);
		break;
	    case ISIS_L1_CSNP:
		isis_traffic[pdb->proc_type][ISIS_L1_CSNP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L1_CSNP_IN]++;
		isis_rcv_csnp(pdb, pak, ISIS_CIRCUIT_L1);
		break;
	    case ISIS_L2_CSNP:
		isis_traffic[pdb->proc_type][ISIS_L2_CSNP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L2_CSNP_IN]++;
		isis_rcv_csnp(pdb, pak, ISIS_CIRCUIT_L2);
		break;
	    case ISIS_L1_PSNP:
		isis_traffic[pdb->proc_type][ISIS_L1_PSNP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L1_PSNP_IN]++;
		isis_rcv_psnp(pdb, pak, ISIS_CIRCUIT_L1);
		break;
	    case ISIS_L2_PSNP:
		isis_traffic[pdb->proc_type][ISIS_L2_PSNP_IN]++;
		isisidb->isisidb_traffic[ISISIDB_L2_PSNP_IN]++;
		isis_rcv_psnp(pdb, pak, ISIS_CIRCUIT_L2);
		break;
	    default:
		break;
	    }
	}
	datagram_done(pak);
    }
    return(TRUE);
}

/*
 * isis_process_timers_inline
 *
 * Process timer events.
 * We handle only a limited number of events, to make sure
 * there is a nice interleaving between packets and timer event.
 * If we stop because there were no more timers to process
 * then we return TRUE, if we stop because we handle our limit
 * of timers, then return FALSE.
 * As a side effect the variables ipx_redist_timer_expired and
 * prc_holddown_expired can be changed.
 */
static inline boolean isis_process_timers_inline (clns_pdbtype *pdb,
				    boolean *ipx_redist_timer_expired,
				    boolean *prc_holddown_expired)
{
    int			limiter;
    mgd_timer		*expired_timer;
    idbtype		*idb;
    isisidbtype		*isisidb;
    isis_lspdbtype	*lsp;

    for (limiter = 0; limiter < ISIS_TIMER_LIMITER; limiter++) {

	expired_timer = mgd_timer_first_expired(&pdb->update_timer);
	if (!expired_timer) {
	    return(FALSE);
	}

	process_may_suspend();
	switch (mgd_timer_type(expired_timer)) {
		
	case ISIS_PSNP_TIMER:

	    /* Try to send a PSNP on this interface. */

	    idb = mgd_timer_context(expired_timer);
	    if (!idb)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    isis_psnp(pdb, idb);
	    break;

	case ISIS_L1_CSNP_TIMER:

	    /* Send an L1 CSNP on this interface. */

	    idb = mgd_timer_context(expired_timer);
	    isisidb = get_isis_idb(pdb, idb);
	    if (!isisidb)
		goto timer_error;
	    isis_csnp_timer_expired(pdb, idb, isisidb, ISIS_CIRCUIT_L1);
	    break;


	case ISIS_L2_CSNP_TIMER:

	    /* Send an L2 CSNP on this interface. */

	    idb = mgd_timer_context(expired_timer);
	    isisidb = get_isis_idb(pdb, idb);
	    if (!isisidb)
		goto timer_error;
	    isis_csnp_timer_expired(pdb, idb, isisidb, ISIS_CIRCUIT_L2);
	    break;


	case ISIS_AGE_TIMER:

	    /* Run the ager. */

	    isis_ager(pdb);
	    break;


	case ISIS_REXMIT_TIMER:

	    /* Potentially retransmit a packet. */

	    idb = mgd_timer_context(expired_timer);
	    isis_rexmit_packet(pdb, idb);
	    break;


	case ISIS_LSP_PACING_TIMER:

	    /* Perhaps go send another LSP on the interface. */

	    idb = mgd_timer_context(expired_timer);
	    isisidb = get_isis_idb(pdb, idb);
	    if (!isisidb)
		goto timer_error;
	    isis_lsp_send_timer_expiry(pdb, idb, isisidb);
	    break;


	case ISIS_LSP_GEN_TIMER:

	    /* Go fire up all of the interfaces again. */

	    mgd_timer_stop(expired_timer);
	    isis_start_all_transmitters(pdb);
	    break;


	case ISIS_LSP_TIMER:

	    /* LSP timer expiry.  Go process it. */
		 
	    lsp = mgd_timer_context(expired_timer);
	    if (!lsp)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    isis_lsp_timer_expiry(pdb, lsp);
	    break;


	case ISIS_LSP_REFRESH_TIMER:

	    /* LSP refresh timer expiry.  Go refresh it. */

	    lsp = mgd_timer_context(expired_timer);
	    if (!lsp)
		goto timer_error;
	    isis_refresh_one_lsp(pdb, lsp);
	    break;


	case ISIS_L1_LSP_BUILD_TIMER:

	    /*
	     * Time to rebuild an L1 LSP.  For NLSP, stop the maximum
	     * holddown timer, and set the flag saying that we may
	     * need to rebuild DR LSPs as well.
	     */
	    mgd_timer_stop(expired_timer);
	    isis_mark_L1_changed(pdb);
	    if (pdb->proc_type == PROC_TYPE_NLSP) {
		TIMER_STOP(pdb->ipx_redist_maxhold);
		*ipx_redist_timer_expired = TRUE;
	    }
	    break;


	case ISIS_L2_LSP_BUILD_TIMER:

	    /* Time to rebuild an L2 LSP. */

	    mgd_timer_stop(expired_timer);
	    isis_mark_L2_changed(pdb);
	    break;


	case ISIS_L1_SPF_TIMER:

	    /*
	     * L1 SPF timer expired.  If the run_spf flag is set,
	     * compute SPF and trigger the timer again (to act as
	     * holddown);  otherwise just cancel the timer.
	     */
	    if (pdb->run_L1_spf) {
		pdb->run_L1_spf = FALSE;
		isis_compute_spt(pdb, ISIS_CIRCUIT_L1);
		mgd_timer_start(expired_timer, pdb->spf_interval[0]);
		process_may_suspend();
	    } else {
		mgd_timer_stop(expired_timer);
	    }
	    break;


	case ISIS_L2_SPF_TIMER:

	    /*
	     * L2 SPF timer expired.  If the run_spf flag is set,
	     * compute SPF and trigger the timer again (to act as
	     * holddown);  otherwise just cancel the timer.
	     */
	    if (pdb->run_L2_spf) {
		pdb->run_L2_spf = FALSE;
		isis_compute_spt(pdb, ISIS_CIRCUIT_L2);
		mgd_timer_start(expired_timer, pdb->spf_interval[1]);
		process_may_suspend();
	    } else {
		mgd_timer_stop(expired_timer);
	    }
	    break;
		
		
	case ISIS_L1_PRC_TIMER:
		
	    /*
	     * L1 PRC timer expired.  If the some_lsps_changed flag is
	     * set, call for a PRC (see below). Always stop the timer.
	     */
	    *prc_holddown_expired = pdb->some_lsps_changed;
	    mgd_timer_stop(expired_timer);
	    break;


	case ISIS_DUPID_TIMER:

	    /* Duplicate ID detection timer expired.  Clean up. */

	    pdb->regen_count = 0;
	    mgd_timer_stop(expired_timer);
	    break;

		
	default:
	timer_error:
	    mgd_timer_stop(expired_timer);
	}
    }
    return(TRUE);
}

/*
 * isis_update_own_lsps
 *
 * If one of our adjecencies changed, or there was some other
 * reason to recreate any of our non-pseudonode LSPs, do so.
 */
void isis_update_own_lsps (clns_pdbtype *pdb)
{
    if (pdb->L1_changed) {
	pdb->L1_changed = FALSE;
	if (pdb->level & ISIS_CIRCUIT_L1) {
	    isis_build_L1_pdu(pdb);
	    process_may_suspend();
	}
    }
    if (pdb->L2_changed) {
	pdb->L2_changed = FALSE;
	if (pdb->level & ISIS_CIRCUIT_L2) {
	    isis_build_L2_pdu(pdb);
	    process_may_suspend();
	}
    }
}

/*
 * isis_update_own_pseudonode_lsps
 *
 * Do the interface specific stuff. If we are the DR on any interface,
 * send out pseudo-node LSPs.
 */
void isis_update_own_pseudonode_lsps (clns_pdbtype *pdb,
				      boolean ipx_redist_timer_expired)
{
    idbtype      *idb;
    isisidbtype  *isisidb;
    idbqueuetype *idbqueue;

    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb) continue;
	if (mgd_timer_running(&isisidb->isis_dr_timer)) continue;

	/*
	 * If some significant info has changed, send the pseudo-node 
	 * LSPs.  Note that this routine can suspend, during which
	 * time more changes can be flagged--we note this by clearing
	 * the dr_changed flag *before* rebuilding the LSP, then at
	 * the end we set the l1_changed flag if the dr_changed flag
	 * has become set again (so the update routine will run again
	 * soon).
	 */
	if (isis_we_are_dr(pdb, idb, CLNS_LEVEL1)) {
	    if (isisidb->isis_L1_dr_changed ||
		(isisidb->isis_ipx_xroutes_changed &&
		 ipx_redist_timer_expired)) {
		isisidb->isis_L1_dr_changed = FALSE;
		isisidb->isis_ipx_xroutes_changed = FALSE;
		if (idb && (pdb->proc_type == PROC_TYPE_ISIS ||
			    reg_invoke_ipx_isis_pseudonode_needed(pdb,
								  idb))) {
		    if (pdb->level & ISIS_CIRCUIT_L1) {
			isis_build_L1_dr_pdu(pdb, idb);
			process_may_suspend();
		    }
		}
		if (isisidb->isis_L1_dr_changed ||
		    isisidb->isis_ipx_xroutes_changed) { /* More changes */
		    isis_mark_L1_changed(pdb);
		}
	    }
	}
	if (isis_we_are_dr(pdb, idb, CLNS_LEVEL2)) {
	    if (isisidb->isis_L2_dr_changed) {
		isisidb->isis_L2_dr_changed = FALSE;
		if (pdb->proc_type == PROC_TYPE_ISIS ||
		    reg_invoke_ipx_isis_pseudonode_needed(pdb, idb)) {
		    /* Only if we need one */
		    if (pdb->level & ISIS_CIRCUIT_L2) {
			isis_build_L2_dr_pdu(pdb, idb);
			process_may_suspend();
		    }
		}
		if (isisidb->isis_L2_dr_changed) { /* More changes! */
		    isis_mark_L2_changed(pdb);
		}
	    }
	}
    }
}

/*
 * isis_do_partial_route_calculation
 *
 * Determine if a partial update should be done.  Note that we
 * loop, since computing routes on one LSP may trigger the
 * recomputation of routes in other LSPs (due to backup routes
 * kicking in).
 */
void isis_do_partial_route_calculation (clns_pdbtype *pdb,
					       boolean *prc_holddown_expired )
{
    if (*prc_holddown_expired) {
	    
	*prc_holddown_expired = FALSE;
	    
	/*
	 * Changes pending, holddown expired. Do partial SPF:
	 */
	while (pdb->some_lsps_changed) {
	    pdb->some_lsps_changed = FALSE;
	    isis_compute_some_routes(pdb);
	    process_may_suspend();
	}
	/*
	 * Start/restart holddown timer:
	 */
	mgd_timer_start(&pdb->L1_prc_timer, pdb->prc_interval[0]);
    }
}

/*
 * isis_update
 *
 * Main ISIS routing process. 
 *
 */
process isis_update (void)
{
    clns_pdbtype	*pdb;
    boolean		ipx_redist_timer_expired;
    boolean		prc_holddown_expired = FALSE;
    boolean		more_timers, more_packets;
    ulong		major, minor;


    /* Initialization gorp. */

    if (!process_get_arg_ptr((void **) &pdb))
	process_kill(THIS_PROCESS);

    process_wait_on_system_init();

    /* Set up the event predicates. */

    process_watch_queue(pdb->update_inputq, ENABLE, RECURRING);
    process_watch_mgd_timer(&pdb->update_timer, ENABLE);
    process_watch_boolean(pdb->update_wakeup, ENABLE, RECURRING);

    /* Now the real work can start. */

    while (TRUE) {

	process_wait_for_event();

	/* If the "running" flag is clear, wedge the process. */

	if (!clns_pdb_running(pdb)) {
	    process_push_event_list(NULL); /* Save the state. */
	    process_watch_boolean(pdb->watched_running, ENABLE, ONE_SHOT);
	    process_wait_for_event();	/* We'll wake up when it's time. */
	    process_watch_boolean(pdb->watched_running, DISABLE, ONE_SHOT);
	    process_pop_event_list(NULL);
	}

	/* Empty the event queue.  We process events in our own order. */

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
	      case QUEUE_EVENT:
	      case BOOLEAN_EVENT:
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
	process_set_boolean(pdb->update_wakeup, FALSE);

	/* Validate the LSP index array.  We're paranoid. %%% */

	isis_validate_lspix_array(pdb);

	/*
	 * We process all expired timers and packets in the queue before
	 * doing anything else. This is to prevent unprocessed expired
	 * timers for the next process_wait_for_event().
	 */

	more_packets = TRUE;
	more_timers = TRUE;
	ipx_redist_timer_expired = FALSE;

	while (more_packets || more_timers) {
	    more_packets = isis_process_packets_inline(pdb);
	    more_timers = isis_process_timers_inline(pdb,
						     &ipx_redist_timer_expired,
						     &prc_holddown_expired);
	}

	/*
	 * Rebuild the LSPs if we've been told to.
	 * See if we have become DIS, or need to change our pseudonode LSPs.
	 * See if we need to do a partial route calculation.
	 */
	isis_update_own_lsps(pdb);
	isis_update_own_pseudonode_lsps(pdb, ipx_redist_timer_expired);
	isis_do_partial_route_calculation(pdb, &prc_holddown_expired);
	
 	/*
 	 * Now clean up the cleanup queue.  These are routes that had all
 	 * of their next-hops removed by the LSP ager, but had some backup
 	 * entries present.  Such routes are left with no next-hops, but
 	 * the backup LSPs are triggered to recalculate.  If none of the
 	 * backup routes turn out to be available, the route would be stranded
 	 * with no next-hops;  the queue cleaner finds these stranded
 	 * routes and finally deletes them.
 	 */
	isis_empty_cleanup_queue(pdb);
    }
}

/*
 * isis_find_L1_lsp
 *
 *   This routine will find the level1 LSP for the specified LSP id
 */
isis_lspdbtype *isis_find_L1_lsp (clns_pdbtype *pdb, isis_lspid *lspid)
{

    isis_tree_type *element;
    int            h;

    element = isis_get_avl_element(lspid, &pdb->level1_LSPs, 
				   &h, NULL, NULL, NULL, 0, NULL, NULL);

    return ((element) ? element->lsp : NULL);
}

/*
 * isis_add_virtual_flag
 *
 * Add the "virtual link" flag to the IS_NEIGHBORS option.
 *
 * Assumes that we're in the middle of building this option.
 */
static void isis_add_virtual_flag (isis_lspcontext *context)
{
    uchar *ptr;
    ptr = isis_add_item_entry(context, 1, FALSE, NULL);
    if (!ptr)
	return;
    *ptr = FALSE;			/* Not virtual */
}

/*
 * isis_allocate_is_adv
 *
 * Allocate space in the current item for another IS advertisement.
 * Advances to new items/LSPs, and takes care of adding the "virtual"
 * flag as necessary.
 *
 * Returns a pointer to the advertisement location, or NULL if no
 * memory.
 */
static isis_is_nbr_ad *isis_allocate_is_adv (isis_lspcontext *context)
{
    isis_is_nbr_ad *nbr;
    
    /*
     * We loop in case it doesn't fit, so we can
     * add the virtual flag as necessary.  We won't loop more
     * than twice.
     */
    while (TRUE) {
	boolean new_flag;
	nbr = isis_add_item_entry(context, ISIS_ONE_IS_AD_SIZE,
				  TRUE, &new_flag);
	if (nbr == NULL)
	    break;
	if (!new_flag)		/* It fit */
	    break;
	isis_add_virtual_flag(context);
    }
    return(nbr);
}

/*
 * isis_write_is_ad
 *
 *    Fill in an IS advertisement, with metric.
 */
static void isis_write_is_ad (isis_is_nbr_ad *nbr_ad, uchar *key, ulong metric)
{
    /*
     * Note that for DR metric should be zero
     */
    nbr_ad->metric.default_metric = metric;
    nbr_ad->metric.delay_metric = ISIS_METRIC_UNSUPPORTED;
    nbr_ad->metric.expense_metric = ISIS_METRIC_UNSUPPORTED;
    nbr_ad->metric.error_metric = ISIS_METRIC_UNSUPPORTED;
    
    systemid_copy (key, nbr_ad->neighbor_id);
}

/*
 * isis_add_dr_adv
 *
 * Add advertisements of adjacency to ourselves as DR to the LSP.
 */
static void isis_add_dr_adv (isis_lspcontext *context)
{
    uchar metric;
    uchar *sys_id;
    idbtype *idb;
    clns_pdbtype *pdb;
    isis_is_nbr_ad *nbr;
    idbqueuetype *idbq;
    isisidbtype *isisidb;
    
    if (!context->lsp)
  	return;
    
    pdb = context->pdb;
    
    /*
     * If we are the designated router on any interface, insert a link to
     * the pseudo-node.
     */
    for (idbq = pdb->idbqueue; idbq; idbq = idbq->next) {
	idb = idbq->idb;
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb)
	    continue;
	if (isis_we_are_dr(pdb, idb, context->level)) {
	    
	    /* Found one.  Create space for it. */
	    
	    nbr = isis_allocate_is_adv(context);
	    if (!nbr)
		return;
	    
	    if (context->level == ISIS_CIRCUIT_L1) {
		metric = isisidb->isis_L1_metric;
		sys_id = isisidb->isis_L1_circuit_id;
	    } else {
		metric = isisidb->isis_L2_metric;
		sys_id = isisidb->isis_L2_circuit_id;
	    }
	    isis_write_is_ad(nbr, sys_id, metric);
	}
    }
}

/*
 * isis_build_is_neighbor
 *
 * Build an IS neighbor entry.  Returns TRUE if successful, FALSE if
 * no memory.
 */
boolean isis_build_is_neighbor (isis_lspcontext *context,
				int metric,
				uchar *sys_id,
				uchar last_byte)
{
    isis_is_nbr_ad *nbr;
    
    /* 
     * Build an advertisment for this IS. 
     */
    nbr = isis_allocate_is_adv(context);
    if (!nbr)
	return(FALSE);
    
    isis_write_is_ad(nbr, sys_id, metric);
    
    /*
     * Fill in last byte.
     */
    nbr->neighbor_id[STATION_LENGTH] = last_byte;
    
    return(TRUE);
}

/*
 * isis_add_L1_is_neighbors
 *
 * Add all L1 IS neighbors to the LSP.
 */
static void isis_add_L1_is_neighbors (isis_lspcontext *context)
{
    clns_adjtype   *adj;
    int            bucket;
    char    last_byte;
    isisidbtype    *isisidb;
    clns_pdbtype *pdb;
    
    /* Walk all adjacencies. */
    
    pdb = context->pdb;
    adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, NULL, 0, adj,
				&bucket)) != NULL) {
	
	isisidb = get_isis_idb(context->pdb, adj->idb);
	if (!isisidb)
	    continue;

	/* Skip adjacencies of the wrong pursuasion. */
	
	if (adj->state != CLNS_ADJ_UP_STATE) continue;
	if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
	    continue;
	if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE && adj->L2only) continue;
	if (!is_p2p(adj->idb) &&
	    !station_match(&adj->key[1], isisidb->isis_L1_circuit_id)) 
	    continue;
	
  	/*
	 * Don't advertise non IS-IS adjacencies.
  	 */
	if (!(adj->protocol_source & ISO_ISISMASK)) continue;
	
	/* 
	 * Build an advertisment for this IS. 
	 */
	last_byte = 0;
	if (!is_p2p(adj->idb))
	    last_byte = isisidb->isis_L1_circuit_id[ISIS_PSEUDO_ID];
	
	if (!isis_build_is_neighbor(context, isisidb->isis_L1_metric,
				    &adj->key[1], last_byte))
	    return;
    }
}

/*
 * isis_write_es_metric
 *
 * Does the work of writing the ES metric fields to the ES 
 * neighbors option.
 */
static void isis_write_es_metric (isis_metric_type *metric, 
			   uchar metric_val)
{
    metric->default_metric = metric_val;
    metric->delay_metric = ISIS_METRIC_UNSUPPORTED;
    metric->expense_metric = ISIS_METRIC_UNSUPPORTED;
    metric->error_metric = ISIS_METRIC_UNSUPPORTED;
}

/*
 * isis_append_one_es_ad
 *
 */
static void isis_append_one_es_ad(isis_info_type *item, uchar *key)
{
    
    uchar *one_neighbor;
    
    one_neighbor = ((uchar *)item) + item->length + ISIS_INFO_TYPE_OVERHEAD;
    station_copy(key, one_neighbor);
    item->length += ISIS_ONE_ES_AD_SIZE;
}

/*
 * isis_add_one_es_ad
 *
 *    Add one ES advertisement
 */
void isis_add_one_es_ad(item, key, metric)
    isis_info_type *item;
    uchar           *key;
    ulong          metric;
{
    isis_es_nbr_ad *ad;
    
    ad = (isis_es_nbr_ad *)&item->value;
    
    /*
     * Note that for DR metric should be zero
     */
    isis_write_es_metric(&ad->metric, metric);
    
    item->length = sizeof (isis_metric_type);
    item->code = ISIS_ES_NEIGHBORS;
    isis_append_one_es_ad(item, key);
}

/*
 * isis_add_es_neighbors
 *
 * Add ES neighbors to the LSP.
 */
static void isis_add_es_neighbors (isis_lspcontext *context)
{
    isis_info_type *item;
    int bucket;
    clns_adjtype *neighbor;
    clns_pdbtype *pdb;
    
    pdb = context->pdb;
    
    /* Always include this system's station id. */
    
    item = isis_new_item(context, ISIS_ES_NEIGHBORS,
			 ISIS_MIN_ES_AD_SIZE,
			 NULL, NULL, TRUE);
    if (!item)
	return;
    isis_add_one_es_ad(item, pdb->systemId, 0);
    ADVANCE_ITEM(item, TRUE);		/* Length guaranteed nonzero */
    isis_item_done(context, item);
    
    /* Now loop for all static ES neighbors. */
    
    neighbor = NULL;
    while ((neighbor = next_neighbor(pdb->adjacency_db, NULL, 
				     CLNS_ES_ADJ_TYPE | CLNS_IS_ADJ_TYPE,
				     neighbor, &bucket)) != NULL) {
	isisidbtype *isisidb;

	isisidb = get_isis_idb(pdb, neighbor->idb);
	if (!isisidb)
	    continue;
	
	/* Skip it if we don't like it. */
	
	if (neighbor->state != CLNS_ADJ_UP_STATE) continue;
	if (!(neighbor->protocol_source & ISO_STATIC) && 
	    !is_p2p(neighbor->idb)) continue;
	if (!itf_in_pdb(neighbor->idb, pdb)) continue;
	
  	/*
	 * Only advertise systems in IS-IS learned areas.
  	 */
	if (!isis_area_in_pdb(pdb, neighbor)) continue;
	
  	/*
	 * Advertise static end system adjacencies. Since I create
	 * a different adjacency for each different station id,
	 * I don't need to worry about looking for lots of station ids
	 * for one physical neighbor.
  	 */
	item = isis_new_item(context, ISIS_ES_NEIGHBORS,
			     ISIS_MIN_ES_AD_SIZE,
			     NULL, NULL, TRUE);
	if (!item)
	    return;
	isis_add_one_es_ad(item, &neighbor->key[1], 
			   isisidb->isis_L1_metric);
	ADVANCE_ITEM(item, TRUE);	/* Length guaranteed nonzero */
	isis_item_done(context, item);
    }
}

/*
 * isis_build_L1_pdu
 *
 * It may generate multiple packets if there is too much information for one.
 *
 * In general, it will attempt to leave 10% of each packet empty to allow
 * for future growth.
 *
 */
void isis_build_L1_pdu (pdb)
    clns_pdbtype   *pdb;
{
    isis_lspcontext  *context;
    
    if (isis_nlsp_local_update_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-Update: Building L1 LSP", pdb->errmsg_name);
    }
    
    context = isis_create_lsp(pdb, ISIS_CIRCUIT_L1, 0, NULL);
    if (context == NULL) return;

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	/* First field must be the manual area addresses.  */
	
	if (!isis_construct_areaAddress(context)) {
	    update_pbuginf(pdb, "No room for area addrs");
	    goto bail_out;
	}
	
	/* Insert NLPID in packet, if necessary. */
	
	isis_insert_lsp_nlpid(context);

	/* Fill in IP information. */

	if (pdb->ip_pdb)
	    reg_invoke_isis_insert_ip_lsp(context);
    }

    if (pdb->ipx_pdb) {

	/* Insert IPX area addresses, if necessary. */
	
	reg_invoke_ipx_isis_insert_ipx_lsp_areas(context);
	
	/* Insert IPX info in LSP, if any. */
	
	reg_invoke_ipx_isis_insert_ipx_lsp(context);
    }
    
    if (pdb->proc_type == PROC_TYPE_ISIS) {

	/* Start the IS neighbors option. */
	
	if (!isis_new_item(context, ISIS_IS_NEIGHBORS, 0, NULL, NULL, TRUE))
	    goto bail_out;
	isis_add_virtual_flag(context);
	
	/* Fill in any IS neighbor adjacencies to our own DR. */
	
	isis_add_dr_adv(context);
	
	/* Fill in other IS neighbor adjacencies. */
	
	isis_add_L1_is_neighbors(context);
	
	/* Close up the item. */
	
	isis_item_done(context, NULL);
	
	/* Add the ES neighbors. */
	
	isis_add_es_neighbors(context);
	
	/* Add IP externals if appropriate. */
	
	if (pdb->ip_pdb &&
	    (pdb->ip_pdb->distflag != pdb->ip_pdb->mask || 
	     pdb->ip_pdb->default_originate)) {
	    reg_invoke_isis_construct_ip_adv(context);
	}

    }
    
    /* Send off any remaining bits and clean up. */
    
    isis_lsp_done(context);
    
    /*
     * Check to see if there are any old LSP fragments. If so,
     * prematurely age them. However, Do not do this if NLSP
     * is hashing, rather than packing, data.
     */
    if (!pdb->ipx_pdb || !pdb->lsp_hash_enable) {
	isis_flush_lsp_fragments(pdb, &context->lspid, ISIS_CIRCUIT_L1);
    }

  bail_out:    
    isis_destroy_context(context);
}

/*
 * isis_construct_areaAddress
 *
 * Build an area address advertisement. If constructing area addresses
 * for a level-1 LSP, use the manually configured area addresses. 
 * Otherwise, use the new area addresses learned for the level-2 LSPs.
 *
 * Returns TRUE if the area addresses were successfully added.
 */
boolean isis_construct_areaAddress(isis_lspcontext *context)
{
    
    uchar    *start;
    uchar   *area_addr;
    int     i, size, num_areas, length_comp;
    boolean active_area_flag;
    int	    bytes_left;
    clns_pdbtype *pdb;
    
    pdb = context->pdb;
    
    if (!isis_new_item(context, ISIS_AREA_ADDRESSES, 0, (void **) &start, 
		       &bytes_left, FALSE))
	return(FALSE);
    
    num_areas = pdb->num_areas;
    active_area_flag = FALSE;
    length_comp = SYSTEMID_LENGTH;
    if (context->level == ISIS_CIRCUIT_L2) {
	num_areas = pdb->num_active_areas;
	active_area_flag = TRUE;
	length_comp = 0;
    }
    for (i = 0; i < num_areas; i++) {
	area_addr = (active_area_flag) ? pdb->active_area_address[i] :
	    pdb->area_address[i];
	
	if (bytes_left < (*area_addr - length_comp + 1)) return(FALSE);
	if (active_area_flag) {
	    nsap_copy (area_addr, *(NSAP_T *)start);
	} else {
	    area_copy (area_addr, *(NSAP_T *)start);
	}
	size = start[0] + 1;
	start += size;
	bytes_left -= size;
    }
    
    isis_item_done(context, start);
    return(TRUE);
}

/*
 * isis_add_L2_is_neighbors
 *
 * Add all L2 IS neighbors to the LSP.
 */
static void isis_add_L2_is_neighbors (isis_lspcontext *context)
{
    clns_adjtype   *adj;
    int            bucket;
    char    last_byte;
    uchar   *key;
    clns_pdbtype *pdb;
    
    /* Walk all adjacencies. */
    
    pdb = context->pdb;
    adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, NULL,
				CLNS_L2_IS_ADJ_TYPE, adj, &bucket))) {
	isisidbtype *isisidb;

	/* Skip adjacencies of the wrong pursuasion. */
	
	if (adj->state != CLNS_ADJ_UP_STATE)
	    continue;
	if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
	    continue;
	isisidb = get_isis_idb(context->pdb, adj->idb);
	if (!isisidb)
	    continue;
	
  	/*
  	 * Advertise all L2 adjacencies on serial links.
  	 * Advertise only the designated router on LANs.
  	 */
  	key = 0;
	if (is_p2p(adj->idb)) {
	    key = &adj->key[1];
	} else if (station_match(&adj->key[1],
				 isisidb->isis_L2_circuit_id)) {
	    key = isisidb->isis_L2_circuit_id;
  	}
	
  	if (key) {
	    
  	    /* Build an advertisement for this IS. */
	    
	    last_byte = 0;
	    if (!is_p2p(adj->idb))
		last_byte = isisidb->isis_L2_circuit_id[ISIS_PSEUDO_ID];
	    if (!isis_build_is_neighbor(context, isisidb->isis_L2_metric,
					key, last_byte))
		return;
  	}
    }
}


/*
 * isis_get_next_item
 *
 *    This routine will iteratively return all of the 'items' advertised
 *    by a given IS (including ourselves). 
 *
 */

#ifdef NOBODY_CALLS_ME
isis_info_type *isis_get_next_item (
    clns_pdbtype *pdb,
    isis_lspdbtype **lsp_db,
    isis_info_type *item,
    int level)
{

    char           *ptr, *data_end;
    isis_tree_type *avl_element;
    isis_tree_type *lsp_tree;

    ptr = (char *)*lsp_db;
    data_end = ptr + (*lsp_db)->db_length;

    if (!item) {
	item = (isis_info_type *)&(*lsp_db)->data->item;
	return((char *)item < data_end ? item : NULL);
    } 
    ADVANCE_ITEM(item, TRUE);
    if ((char *)item >= data_end) {
	/*
	 * Go get the next element from the avl tree.
	 * If it is from the same station, walk through its items, too
	 * ??? This won't handle the case where a PDU has no items ???
	 */
	avl_element = 0;
	if (level == ISIS_CIRCUIT_L1) {
	    lsp_tree = pdb->level1_LSPs;
	} else {
	    lsp_tree = pdb->level2_LSPs;
	}

	isis_next_avl(&(*lsp_db)->data->lspid, lsp_tree, &avl_element);

	if (avl_element) {
	    *lsp_db = avl_element->lsp;
	    ptr = (char *)*lsp_db;
	    data_end = ptr + (*lsp_db)->db_length;
	    item = (isis_info_type *)&(*lsp_db)->data->item;
	    if ((char *)item < data_end) return(item);
	}
	return(NULL);
    }
    return(item);
}
#endif


/*
 * isis_walk_item
 *
 *  Return a pointer to the next advertisement.
 *
 */
static isis_generic_ad *isis_walk_item (isis_info_type *item,
					isis_generic_ad *adv)
{
    isis_info_type *next_item;
    int  size, overhead;

    /*
     * Each item will contain advertisements for one of the following:
     *   end systems
     *   intermediate systems
     *   prefixes
     *   and then some
     * of course (!) the format for each is different.
     */
    switch (item->code) {
    case ISIS_ES_NEIGHBORS:
	size = ISIS_ONE_ES_AD_SIZE;
	overhead = 4;
	break;

    case ISIS_IS_NEIGHBORS:
	size = ISIS_ONE_IS_AD_SIZE;
	overhead = 1;
	break;

    case ISIS_PREFIX_NEIGHBORS:
	if (adv) {
	    /* Stored in LSP with length in semi-octet units. */
	    size = ((*(char *)adv) >> 1) + 1;
	    if (*((char *) adv) & 1) size++;
	    overhead = 0;
	} else {
	    size = 0;
	    overhead = 4;
	}
        break;

    case ISIS_AREA_ADDRESSES:
	if (adv) {
	    size = (*(char *)adv) + 1;
	} else {
	    size = 0;			/* Size doesn't matter */
	}
	overhead = 0;
	break;

    case ISIS_IP_INTERNAL_ADDR:
    case ISIS_IP_EXTERNAL_ADDR:
 	size = sizeof(isis_ip_nbr_ad);
 	overhead = 0;
 	break;
 
    case ISIS_IP_IF_ADDRESSES:
 	size = sizeof(ipaddrtype);
 	overhead = 0;
 	break;
 
    case ISIS_NLPID:
 	size = 1;
 	overhead = 0;
 	break;
 
    case ISIS_IDR_INFO:
 
 	/*
 	 * Handle a single item at a time, therefore the size is
 	 * set to max.
 	 */
 	overhead = 0;
 	size = item->length + ISIS_INFO_TYPE_OVERHEAD;
 	break;

    case ISIS_NLSP_AREA_ADDRS:
	size = ISIS_NLSP_AREA_ADDRS_LEN;
	overhead = 0;
	break;

    case ISIS_NLSP_MGMT_INFO:
	size = item->length + ISIS_INFO_TYPE_OVERHEAD;
	overhead = 0;
	break;

    case ISIS_NLSP_LINK_INFO:
	size = item->length + ISIS_INFO_TYPE_OVERHEAD;
	overhead = 0;
	break;

    case ISIS_NLSP_SVCS_INFO:
	size = item->length + ISIS_INFO_TYPE_OVERHEAD;
	overhead = 0;
	break;

    case ISIS_NLSP_EXT_ROUTES:
	size = ISIS_NLSP_EXT_ROUTES_LEN;
	overhead = 0;
	break;

    case ISIS_NLSP_SUMMARY_ROUTES:
 	size = ISIS_NLSP_SUMMARY_ROUTES_LEN;
 	overhead = 0;
 	break;

    default:
	overhead = 0;
	size = item->length + ISIS_INFO_TYPE_OVERHEAD;
	break;
    }

    if (!adv) {
	adv = (isis_generic_ad *)(((uchar *)&item->value) + overhead);
    } else {
	adv = (isis_generic_ad *)(((uchar *)adv) + size);
    }

    next_item = item;
    ADVANCE_ITEM(next_item, TRUE);
    if ((uchar *) adv < (uchar *) next_item) 
	return(adv);

    return(NULL);
}


/* 
 * isis_walk_lsp_data
 *
 *   Return a pointer to the next advertisement;
 *
 *   This routine will walk an individual LSP data PDU and return the
 *   next advertised item.
 *
 *   It will also update the value of 'item' if it needs to proceed
 *   to another item in this PDU.
 *
 *   Advertisement type of 0 will match all types.
 */    
static isis_generic_ad *isis_walk_lsp_data (isis_lspdbtype *lsp_db,
					    isis_info_type **item,
					    isis_generic_ad *adv,
					    int type)
{
    char    *data_end;
    boolean walk_item;

    data_end = ((char *) lsp_db) + lsp_db->db_length;

    /*
     * If we aren't already positioned at an 'item', find one.
     */
    if (!*item) {
	*item = (isis_info_type *)&lsp_db->data->item;
	adv = NULL;
    }

    while ((char *)*item < data_end) {
	walk_item = TRUE;
	if (type != 0) {
	    switch (type) {
	    case CLNS_L1_IS_ADJ_TYPE:
	    case CLNS_L2_IS_ADJ_TYPE:
		if ((*item)->code != ISIS_IS_NEIGHBORS)
		    walk_item = FALSE;
		break;

	    case CLNS_ES_ADJ_TYPE:
		if ((*item)->code != ISIS_ES_NEIGHBORS)
		    walk_item = FALSE;
		break;

	    case CLNS_PREFIX_ADJ_TYPE:
		if ((*item)->code != ISIS_PREFIX_NEIGHBORS)
		    walk_item = FALSE;
		break;

	    case CLNS_NLSP_IS_ADJ_TYPE:
		if ((*item)->code != ISIS_NLSP_LINK_INFO)
		    walk_item = FALSE;
		break;

	    case CLNS_NLSP_SVC_ADJ_TYPE:
		if ((*item)->code != ISIS_NLSP_SVCS_INFO)
		    walk_item = FALSE;
		break;

	    case CLNS_NLSP_XROUTE_ADJ_TYPE:
		if ((*item)->code != ISIS_NLSP_EXT_ROUTES)
		    walk_item = FALSE;
		break;

 	    case CLNS_NLSP_SROUTE_ADJ_TYPE:
 		if ((*item)->code != ISIS_NLSP_SUMMARY_ROUTES)
 		    walk_item = FALSE;
 		break;

	    }
	}

	if (walk_item) {
	    adv = isis_walk_item(*item, adv);
	    if (adv) {
		return(adv);
	    }
	}

	/* mal-formed LSP */
	if (!(*item)->length) return(NULL);
	
	ADVANCE_ITEM(*item, TRUE);	/* Length guaranteed nonzero */
	adv = NULL;
    }
    return(NULL);
}


/* 
 * isis_walk_lsp
 *
 *    This routine will iteratively return all of the neighbors advertised
 *    by a specific IS. It takes 'LSP_ptr' as the header. 'LSP_ptr, 'item'
 *    and 'adv' are used to keep track of where we are in the link state info.
 * 
 *    It is specifically intended to work for all types of advertisements:
 *    level1 IS, level2 IS, ES, and prefixes.
 *
 *    If the advertisement type is 0, it will match all advertisements.
 */
isis_generic_ad *isis_walk_lsp (
    clns_pdbtype *pdb,
    isis_lspdbtype **lsp_db,
    isis_info_type **item,
    isis_generic_ad *adv,
    int type,
    int level)
{

    isis_tree_type *avl, *lsp_tree;
    
    if (level == ISIS_CIRCUIT_L1) {
	lsp_tree = pdb->level1_LSPs;
    } else {
	lsp_tree = pdb->level2_LSPs;
    }

    while (*lsp_db) {
	adv = isis_walk_lsp_data(*lsp_db, item, adv, type);
	if (adv) return (adv);

	isis_next_avl(&(*lsp_db)->data->lspid, lsp_tree, &avl);
	if (!avl) break;

	*lsp_db = avl->lsp;
	*item = NULL;

    }
    return (NULL);
}


/*
 * isis_rw_refresh_lsp
 *
 * This function will send out new LSPs that were previously generated
 * by this system. The sequence number will be incremented and the 
 * hold time set to MAX_AGE. This function processes both non-pseudo and
 * pseudo node LSPs and associated fragments.
 *
 */
static boolean isis_rw_refresh_lsp (isis_tree_type *element, void *pdb_parm,
				    void *dummy1, void *dummy2, void *dummy3)
{

    isis_lspdbtype *lsp;
    clns_pdbtype *pdb;

    pdb = pdb_parm;
    lsp = element->lsp;
    return(isis_refresh_one_lsp(pdb, lsp));
}


/*
 * isis_refresh_lsp_level
 *
 * Refresh LSPs by reflooding when periodic timer expires.
 *
 */
static void isis_refresh_lsps_level (clns_pdbtype *pdb, int level)
{

    isis_tree_type *element;
    idbqueuetype   *idbq;
    isis_lspid     lspid;
    isisidbtype	   *isisidb;


    if (level == ISIS_CIRCUIT_L1) {
	element = pdb->level1_LSPs;
    } else {
	element = pdb->level2_LSPs;
    }
    if (!element) return;

    systemid_copy (pdb->systemId, lspid.system_id);
    lspid.lsp_number = 0;

    /*
     * Do non-pseudo node LSP.
     */
    isis_recursive_walk(&lspid, ISIS_WALK_MATCH_SYSTEM, element, 
			isis_rw_refresh_lsp, pdb, NULL, NULL, NULL);

    for (idbq = pdb->idbqueue; idbq; idbq = idbq->next) {
	isisidb = get_isis_idb(pdb, idbq->idb);
	if (isisidb == NULL) {
	    return;
	}
	lspid.pseudo_id = isisidb->isis_local_ckt_number;
	if (level == ISIS_CIRCUIT_L1 &&
	    !isis_we_are_dr(pdb, idbq->idb, CLNS_LEVEL1)) {
	    continue;
	}
	if (level == ISIS_CIRCUIT_L2 &&
	    !isis_we_are_dr(pdb, idbq->idb, CLNS_LEVEL2)) {
	    continue;
	}

	/*
	 * Do pseudo-node LSP.
	 */
	isis_recursive_walk(&lspid, ISIS_WALK_MATCH_SYSTEM, element, 
			    isis_rw_refresh_lsp, pdb, NULL, NULL, NULL);
    }
}
/*
 * isis_refresh_lsps
 *
 * Refresh LSPs by reflooding each of them.
 *
 */
void isis_refresh_lsps (clns_pdbtype *pdb)
{
    isis_refresh_lsps_level(pdb, ISIS_CIRCUIT_L1);
    if (pdb->level & ISIS_CIRCUIT_L2)
	isis_refresh_lsps_level(pdb, ISIS_CIRCUIT_L2);
}


/*
 * isis_find_L2_lsp
 * 
 *    This routine will find the level2 LSP for the specified system Id
 */
isis_lspdbtype *isis_find_L2_lsp (clns_pdbtype *pdb, isis_lspid *lspid)
{
    isis_tree_type *element;
    int            h;

    element = isis_get_avl_element(lspid, &pdb->level2_LSPs, 
				   &h, NULL, NULL, NULL, 0, NULL, NULL);

    return ((element) ? element->lsp : NULL);
}


/*
 * isis_build_L2_pdu
 *
 * This routine will actual build all of the current adjacency info into
 * a Link state pdu. This may actually span more than one packet if 
 * necessary.
 *
 * In general, this routine will attempt to avoid filling all of the
 * link state packets in order to leave room for future growth.
 * 
 * Level 2 link state pdus contain level 2 is neighbors and prefix 
 * neighbors
 */
void isis_build_L2_pdu (pdb)
    clns_pdbtype *pdb;
{
    isis_lspcontext  *context;
    
    if (isis_nlsp_local_update_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-Update: Building L2 LSP", pdb->errmsg_name);
    }
    
    context = isis_create_lsp(pdb, ISIS_CIRCUIT_L2, 0, NULL);
    if (context == NULL) return;

    /*
     * First field must be the area addresses. Note that this is not
     * exactly the same as 'area addresses' in level1. This is supposed
     * to merge the manual area addresses from all of the received level 1
     * lsps and send those.
     */
    if (!isis_construct_areaAddress(context)) {
	update_pbuginf(pdb, "No room for area addrs");
	goto bail_out;
    }
    
    /* Insert NLPID in packet, if necessary. */
    
    isis_insert_lsp_nlpid(context);
    
    /* Fill in IP information. */

    if (pdb->ip_pdb)
	reg_invoke_isis_insert_ip_lsp(context);
    
    /* Start the IS neighbors option. */
    
    if (!isis_new_item(context, ISIS_IS_NEIGHBORS, 0, NULL, NULL, TRUE))
	goto bail_out;
    isis_add_virtual_flag(context);
    
    /* Fill in any IS neighbor adjacencies to our own DR. */
    
    isis_add_dr_adv(context);
    
    /* Fill in other IS neighbor adjacencies. */
    
    isis_add_L2_is_neighbors(context);
    
    /* Close up the item. */
    
    isis_item_done(context, NULL);
    
    /* Fill in prefix neighbors. */
    
    if (pdb->proc_type == PROC_TYPE_ISIS)
	reg_invoke_isis_add_prefix_neighbors(context);
    
    /* Fill in IP externals and summaries, if appropriate. */
    
    if (pdb->ip_pdb) {
	reg_invoke_isis_construct_ip_adv(context);
    }
    
    /* Send off any remaining bits and clean up. */
    
    isis_lsp_done(context);
    
    /*
     * Check to see if there are any old LSP fragments. If so,
     * prematurely age them.
     */
    isis_flush_lsp_fragments(pdb, &context->lspid, ISIS_CIRCUIT_L2);

  bail_out:    
    isis_destroy_context(context);
}

/*
 * isis_construct_dr
 *
 * Build a Designated Router advertisement. Include only
 * non-Pseudo node advertisments.
 */
static void isis_construct_dr (isis_lspcontext *context)
{
    clns_adjtype   *adj;
    int            bucket;
    idbtype	   *idb;
    int		   level;
    clns_pdbtype   *pdb;
    
    idb = context->lsp_idb;
    pdb = context->pdb;
    level = context->level;

    /* Walk all adjacencies. */
    
    adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, idb, 0, adj, &bucket))) {
	
	/* Skip adjacencies of the wrong pursuasion. */
	
	if (adj->state != CLNS_ADJ_UP_STATE)
	    continue;
	if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
	    continue;
	if (adj->protocol_source == ISO_STATIC)
	    continue;
	if ((level == ISIS_CIRCUIT_L1 && 
	     ((adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE) ||
	      (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE &&  !adj->L2only))) ||
	    (level == ISIS_CIRCUIT_L2 && 
	     (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE))) {
	    
	    /* Build the entry. */
	    
	    if (!isis_build_is_neighbor(context, 0, &adj->key[1], 0))
		return;
	}
    }
}

/*
 * isis_add_es_metric
 *
 * Add the ES metric field to the LSP.
 *
 * Assumes that we're in the middle of building this option.
 */
static void isis_add_es_metric (isis_lspcontext *context)
{
    isis_metric_type *metric;
    metric = isis_add_item_entry(context, sizeof(isis_metric_type), 
				 FALSE, NULL);
    if (!metric)
	return;
    isis_write_es_metric(metric, 0);
}

/*
 * isis_allocate_es_adv
 *
 * Allocate space in the current item for another ES advertisement.
 * Advances to new items/LSPs, and takes care of adding the metric
 * field as necessary.
 *
 * Returns a pointer to the advertisement location, or NULL if no
 * memory.
 */
static uchar *isis_allocate_es_adv (isis_lspcontext *context)
{
    uchar *nbr;
    
    /*
     * We loop in case it doesn't fit, so we can
     * add the metric as necessary.  We won't loop more
     * than twice.
     */
    while (TRUE) {
	boolean new_flag;
	nbr = isis_add_item_entry(context, ISIS_ONE_ES_AD_SIZE,
				  TRUE, &new_flag);
	if (nbr == NULL)
	    break;
	if (!new_flag)		/* It fit */
	    break;
	isis_add_es_metric(context);
    }
    return(nbr);
}
/*
 * isis_construct_dr_es
 *
 * Build end-system links in pseudo-node LSP with metric 0.
 *
 */
static void isis_construct_dr_es (isis_lspcontext *context)
{
    clns_pdbtype *pdb;
    uchar *adv;
    clns_adjtype *neighbor;
    int bucket;
    idbtype *idb;
    
    pdb = context->pdb;
    idb = context->lsp_idb;
    
    /* Create the item. */
    
    if (!isis_new_item(context, ISIS_ES_NEIGHBORS, 0, NULL, NULL, TRUE))
	return;
    
    /* Add the metric field. */
    
    isis_add_es_metric(context);
    
    /* Now loop for all ES neighbors. */
    
    neighbor = NULL;
    while ((neighbor = next_neighbor(pdb->adjacency_db, idb,
				     CLNS_ES_ADJ_TYPE | CLNS_IS_ADJ_TYPE,
				     neighbor, &bucket)) != NULL) {
	if (neighbor->protocol_source == ISO_STATIC) continue;
	if (!isis_area_in_pdb(pdb, neighbor)) continue;
	
	adv = isis_allocate_es_adv(context);
	if (adv == NULL)
	    return;
	station_copy(&neighbor->key[1], adv);
    }
    isis_item_done(context, NULL);
}

/*
 * isis_build_L1_dr_pdu
 *
 *    This routine will build a level 1 designated router pdu.
 *    It may span multiple packets.
 *
 */
void isis_build_L1_dr_pdu (clns_pdbtype *pdb, idbtype *idb)
{
    isis_lspcontext *context;
    isisidbtype     *isisidb;
    
    /* Don't do it if we shouldn't be here. */

    isisidb = get_isis_idb(pdb, idb);
    if (isisidb == NULL) {
	return;
    }
    if (!isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L1))
	return;

    if (isis_nlsp_local_update_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-Update: Building L1 pseudonode LSP for %s",
	       pdb->errmsg_name, idb->namestring);
    }
    
    context = isis_create_lsp(pdb, ISIS_CIRCUIT_L1,
			      isisidb->isis_local_ckt_number, idb);
    if (context == NULL)
	return;

    if (pdb->ipx_pdb) {

	/* Add the IPX stuff as appropriate. */
	
	reg_invoke_ipx_isis_insert_ipx_lsp(context);
    }
    
    if (pdb->proc_type == PROC_TYPE_ISIS) {

	/* Start the IS neighbors option. */
	
	if (!isis_new_item(context, ISIS_IS_NEIGHBORS, 0, NULL, NULL, TRUE))
	    goto bail_out;
	isis_add_virtual_flag(context);
	
	/*
	 * Always include my own system id.
	 */
	isis_build_is_neighbor(context, 0, pdb->systemId, 0);
	
	/* Add the IS neighbors. */
	
	isis_construct_dr(context);
	
	/* Close up the item. */
	
	isis_item_done(context, NULL);
	
	/* Add the ES neighbors. */
	
	isis_construct_dr_es(context);
    }
    
    /* Send off any remaining bits and clean up. */
    
    isis_lsp_done(context);
    
    /*
     * Check to see if there are any old LSP fragments. If so,
     * prematurely age them.
     */
    isis_flush_lsp_fragments(pdb, &context->lspid, ISIS_CIRCUIT_L1);

  bail_out:
    isis_destroy_context(context);
}

/*
 * isis_build_L2_dr_pdu
 *
 */
void isis_build_L2_dr_pdu (clns_pdbtype *pdb, idbtype *idb)
{
    isis_lspcontext *context;
    isisidbtype	    *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (isisidb == NULL) {
	return;
    }

    /* Don't do it if we shouldn't be here. */
    if (!isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L2))
	return;

    if (isis_nlsp_local_update_debug[pdb->proc_type] ||
	isis_nlsp_update_packet_debug[pdb->proc_type]) {
	buginf("\n%s-Update: Building L2 pseudonode LSP for %s",
	       pdb->errmsg_name, idb->namestring);
    }
    
    context = isis_create_lsp(pdb, ISIS_CIRCUIT_L2,
			      isisidb->isis_local_ckt_number, idb);
    if (context == NULL) return;
    
    /* Start the IS neighbors option. */
    
    if (!isis_new_item(context, ISIS_IS_NEIGHBORS, 0, NULL, NULL, TRUE))
	goto bail_out;
    isis_add_virtual_flag(context);
    
    /*
     * Always include my own system id.
     */
    isis_build_is_neighbor(context, 0, pdb->systemId, 0);
    
    /* Add the IS neighbors. */
    
    isis_construct_dr(context);
    
    /* Close up the item. */
    
    isis_item_done(context, NULL);
    
    /* Send off any remaining bits and clean up. */
    
    isis_lsp_done(context);
    
    /*
     * Check to see if there are any old LSP fragments. If so,
     * prematurely age them.
     */
    isis_flush_lsp_fragments(pdb, &context->lspid, ISIS_CIRCUIT_L2);

  bail_out:
    isis_destroy_context(context);
}

/*
 * isis_flush_lsp_fragments
 *
 * Prematurely age all LSP fragments that come after the supplied LSPID.
 */
void isis_flush_lsp_fragments (
    clns_pdbtype *pdb,
    isis_lspid *lsp_id,
    int level)
{
    isis_lspid lspid;
    isis_tree_type *lsp_tree;

    if (level == ISIS_CIRCUIT_L1) {
	lsp_tree = pdb->level1_LSPs;
    } else {
	lsp_tree = pdb->level2_LSPs;
    }

    lspid_copy(lsp_id, &lspid);
    if (lspid.lsp_number == 0xFF)
	return;
    lspid.lsp_number++;
    isis_recursive_walk(&lspid, ISIS_WALK_MATCH_SYSTEM, lsp_tree,
			isis_rw_purge_lsp, &level, pdb,
			&pdb->lsp_lifetime, NULL);
}

/*
 * isis_area_in_pdb
 *
 * Determines if any of the adjacencies area addresses are in the list of 
 * IS-IS learned area addresses.
 */
boolean isis_area_in_pdb (clns_pdbtype *pdb, clns_adjtype *adj)
{

    int i, j;

    for (i = 0; i < pdb->num_active_areas; i++) {
	for (j = 0; j < adj->num_areas; j++) {
	    if (nsap_match (adj->area_address[j], 
			    pdb->active_area_address[i])) return(TRUE);
	}
    }
    return(FALSE);
}


/*
 *
 */
void clns_route_map_inform (char *map_name, int event)
{
    clns_pdbtype *clns_pdb;

    for (clns_pdb = clns_protoQ.qhead; clns_pdb; clns_pdb = clns_pdb->next) {
	if (!(clns_pdb->typeflag & ISO_ISISMASK)) {
	    continue;
	}
	isis_mark_L2_changed(clns_pdb);
    }
}
