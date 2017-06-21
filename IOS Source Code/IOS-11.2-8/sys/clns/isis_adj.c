/* $Id: isis_adj.c,v 3.6.6.9 1996/09/11 20:09:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_adj.c,v $
 *------------------------------------------------------------------
 * isis_adj.c - Functions for processing IS-IS adjacencies.
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_adj.c,v $
 * Revision 3.6.6.9  1996/09/11  20:09:22  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.6.6.8  1996/08/28  12:38:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.7  1996/08/08  14:50:43  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.6.6.6  1996/08/04  23:35:06  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.6.6.5  1996/05/26  06:42:27  dkatz
 * CSCdi58621:  ESH from router confuses ISIS
 * Branch: California_branch
 * Make sure ISIS adjacencies override ES adjacencies.
 *
 * Revision 3.6.6.4  1996/05/07  17:39:53  dkatz
 * CSCdi56547:  ISIS/NLSP have problems with more than 42 neighbors
 * Branch: California_branch
 * Allow neighbors to be spread across multiple TLVs (on both send
 * and receive.)
 *
 * Revision 3.6.6.3  1996/05/07  17:18:47  dkatz
 * CSCdi55147:  NLSP doesnt detect duplicate system IDs
 * Branch: California_branch
 * Detect duplicates in LSPs and IIHs.  Back off the irritating messages
 * to once per minute.
 *
 * Revision 3.6.6.2  1996/04/21  20:47:36  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.6.6.1  1996/03/18  19:03:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.4  1996/03/16  06:31:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.3  1996/03/13  01:13:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.2  1996/03/07  08:40:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  13:40:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/01  07:31:35  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.5  1996/02/20  22:54:36  asastry
 * CSCdi48461:  ISIS adjs can show up as type Phase IV if IV/V conversion
 * is on
 *
 * Revision 3.4  1995/12/22  21:58:10  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  08:53:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:52:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/23  23:14:10  dkatz
 * CSCdi39776:  ISIS can mislink backup chain
 * Dont leave a backup chain dangling from the heap.
 * Make sure that we run SPF when the second level adjacency comes up
 * if L1L2.
 *
 * Revision 2.5  1995/09/23  22:48:25  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.4  1995/08/25  15:19:49  dkatz
 * CSCdi38310:  ISIS LSP may not reflect all adjacencies
 * Always rebuild both L1 and L2 LSPs when an adjacency changes to Up state.
 *
 * Revision 2.3  1995/08/25  05:39:58  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.2  1995/08/23  18:03:06  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:18:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "mgd_timers.h"
#include "ieee.h"
#include "../if/network.h"
#include "config.h"
#include "clns.h"
#include "clns_route.h"
#include "isis.h"
#include "clns_adj.h"
#include "clns_externs.h"
#include "isis_externs.h"
#include "isis_inlines.h"
#include "isis_nlsp_debug.h"
#include "isis_registry.h"
#include "clns_registry.h"


/*
 * isis_log_adjacency_change
 * 
 * Produce a syslog message on an adjacency change, if enabled.
 */
void isis_log_adjacency_change (clns_pdbtype *pdb, clns_adjtype *adj,
				boolean going_up, const char *msg)
{
    char *interface_name;

    if (!pdb->log_adj_changes)		/* Punt if not configured. */
	return;

    if (adj->idb) {
	interface_name = adj->idb->namestring;
    } else {
	interface_name = "Unknown";
    }
    errmsg(&msgsym(ADJCHANGE, CLNS), pdb->errmsg_name,
	   station_string(&adj->key[1]), interface_name,
	   going_up ? "Up" : "Down", msg);
}

/*
 * isis_start_adj_timer
 *
 * Start the adjacency managed timer to match the adjacency expiration.
 *
 * This managed timer is used so that the adjacency will expire immediately
 * without waiting for adjacency_ager to find it.
 *
 * We initialize the timer to point to the PDB if it isn't running,
 * and then set it to expire with the adjacency.
 */
static void isis_start_adj_timer (clns_pdbtype *pdb, clns_adjtype *adj)
{
    if (!mgd_timer_running(&adj->isis_expiration_timer)) {
	mgd_timer_init_leaf(&adj->isis_expiration_timer, &pdb->timer,
			    ISIS_ADJ_TIMER, adj, FALSE);
    }
    mgd_timer_set_exptime(&adj->isis_expiration_timer, &adj->expiration_time);
}

/*
 *  isis_filter_adj
 *
 *    Returns true if the combination of the area address and system ID
 *    (plus a zero NSEL) matches the supplied address filter index.
 */
static boolean isis_filter_adj(isis_info_type *item, uchar *station_id,
			ulong filter_index, boolean match_all)
{
    nsap_addr address;
    uchar *addrptr;
    uchar *itemptr = item->value;
    uchar *itemend = itemptr + item->length;
    boolean result = match_all;
    boolean this_result;

    /* Loop for each area address in the item. */

    /* Build the address. */

    while (itemptr < itemend) {
	addrptr = address.addr;
	if (*itemptr + STATION_LENGTH + 1 > MAX_CLNS_ADDR_LEN) /* Too big! */
	    return(FALSE);
	address.length = *itemptr + STATION_LENGTH + 1;
	bcopy(itemptr + 1, addrptr, *itemptr);
	addrptr += *itemptr;
	itemptr += *itemptr + 1;
	bcopy(station_id, addrptr, STATION_LENGTH);
	addrptr += STATION_LENGTH;
	*addrptr = 0;
	this_result = reg_invoke_clns_filter_addr(filter_index, &address,
						  NULL);
	if (match_all)
	    result = result && this_result;
	else
	    result = result || this_result;
    }
    return(result);
}

/*
 * isis_nlsp_area_address_match
 *
 * Returns TRUE if at least one of the area addresses in the IIH
 * matches.  Returns FALSE if no match.  The option has already
 * been length-checked.
 */
static boolean isis_nlsp_area_address_match (isis_info_type *item,
					     clns_pdbtype *pdb)
{
    int area_count, pdb_ix, item_ix;
    ulong net, mask;
    isis_nlsp_area_addrs *area_item;
    boolean found;

    found = FALSE;
    area_count = item->length / ISIS_NLSP_AREA_ADDRS_LEN;
    area_item = (isis_nlsp_area_addrs *) item->value;
    for (item_ix = 0; item_ix < area_count; item_ix++, area_item++) {
	net = GETLONG(&area_item->net_number);
	mask = GETLONG(&area_item->net_mask);
	for (pdb_ix = 0; pdb_ix < pdb->num_nlsp_areas; pdb_ix++) {
	    if (pdb->nlsp_manual_area[pdb_ix] == net &&
		pdb->nlsp_manual_area_mask[pdb_ix] == mask) {
		found = TRUE;
		break;
	    }
	}
	if (found)
	    break;
    }
    return(found);
}

/*
 *  isis_area_address_match
 *
 *    Return true if at least one of the areas in the IIH packet
 *    matches one of the ISIS area addresses.
 */
static boolean isis_area_address_match (isis_info_type *item,
					clns_pdbtype   *pdb)
{

    isis_info_type *item_end;
    int   i;
    NSAP_T *area_address;

    area_address = (NSAP_T *)item->value;
    item_end = item;
    ADVANCE_ITEM(item_end, TRUE);
    while ((uchar *)area_address < (uchar *)item_end) {
	for (i = 0; i < pdb->num_areas; i++) {
	    if (nsap_match (*area_address, pdb->area_address[i])) {
		return(TRUE);
	    }
	}
	area_address = (NSAP_T *)(((uchar *)area_address) +
				  *area_address[0] + 1);
    }
    return(FALSE);
}

/*
 * isis_update_adj_count
 * 
 * Update the adjacency count for the particular adjacency type on the given
 * interface. If 'increment' is true, update the count, else decrement it.
 */
void isis_update_adj_count (clns_pdbtype *pdb, clns_adjtype *adj,
			    boolean increment)
{
   idbtype *idb;
   isisidbtype *isisidb;

   idb = adj->idb;
   if (!idb)
       return;
   isisidb = get_isis_idb(pdb, idb);
   if (!isisidb)
       return;
   switch (adj->adjacencyType) {
   case CLNS_L2_IS_ADJ_TYPE:
       increment ? (isisidb->isis_adj_count[1]++) : 
	   (isisidb->isis_adj_count[1]--);
       if (adj->L2only) break;
   case CLNS_NLSP_IS_ADJ_TYPE:
   case CLNS_L1_IS_ADJ_TYPE:
       increment ? (isisidb->isis_adj_count[0]++) : 
	   (isisidb->isis_adj_count[0]--);
       break;
   }
}

/*
 * isis_validate_basic_nlsp_hello
 *
 * Validates area address and MTU info from any NLSP hello
 *
 * Returns TRUE if OK, FALSE if bogus.  Deletes any adjacency 
 * on area mismatch.
 */
static boolean isis_validate_basic_nlsp_hello (clns_pdbtype *pdb,
					       isis_TLV_parse *tlv_parse,
					       clns_adjtype *adj)
{
    isis_info_type *mtu_item;
    isis_info_type *area_item;
    isis_nlsp_local_mtu *mtu_value;

    /* Make sure the right TLVs are present and well-formed. */

    area_item = tlv_parse->TLV_array[ISIS_NLSP_AREA_ADDRS];
    mtu_item = tlv_parse->TLV_array[ISIS_NLSP_LOCAL_MTU];

    for (;;) {				/* Not really a loop */
	if (area_item == NULL || mtu_item == NULL) {
	    break;
	}
	
	if (mtu_item->length != ISIS_NLSP_LOCAL_MTU_LEN) {
	    break;
	}
	mtu_value = (isis_nlsp_local_mtu *) mtu_item->value;
	if (GETLONG(&(mtu_value->mtu)) < NOVELL_MINMTU) {
	    break;
	}
	
	/* Punt if there's no area address match. */
	
	if (!isis_nlsp_area_address_match(area_item, pdb)) {
	    if (adj) {
		isis_log_adjacency_change(pdb, adj, FALSE, "area mismatch");
		delete_adjacency(pdb->adjacency_db, adj);
	    }
	    adj_pbuginf(pdb, "Area mismatch");
	    return(FALSE);
	}
	return(TRUE);
    }
    adj_pbuginf(pdb, "Improperly formatted IIH");
    return(FALSE);
}
/*
 * isis_compute_p2p_circuit_id
 *
 * Computes the point-to-point circuit ID, given the received IIH and
 * the local information.
 *
 * Returns TRUE if the new circuit ID is different than before.
 */
static boolean isis_compute_p2p_circuit_id (clns_pdbtype *pdb, idbtype *idb,
					    isisidbtype *isisidb,
					    isis_pt_IIH_hdrtype *isis)
{
    uchar circuit_id[SYSTEMID_LENGTH];
    boolean id_changed;
    
    /*
     * Set the larger of the two circuit IDs as the new one.
     */
    if (compare_stationid(pdb->systemId, isis->source_id) >= 0) {
	
	/* Our system id is larger than neighbors. */
	
	station_copy(pdb->systemId, circuit_id);
	circuit_id[ISIS_PSEUDO_ID] = isisidb->isis_local_ckt_number &
	    ISIS_P2P_CKT_MASK;
	
    } else {
	
	/* His is bigger. */
	
	station_copy(isis->source_id, circuit_id);
	circuit_id[ISIS_PSEUDO_ID] = isis->local_circuit_id;
    }
    
    /* Note if the circuit ID is different. */
    
    id_changed = !systemid_match(circuit_id, isisidb->isis_L1_circuit_id);

    /* Update the circuit ID. */

    systemid_copy(circuit_id, isisidb->isis_L1_circuit_id);

    return(id_changed);
}


/*
 * isis_nlsp_area_copy
 *
 * Copy area addresses from IIH into adjacency
 */
static void isis_nlsp_area_copy (clns_adjtype *adj, isis_info_type *area_item)
{
    isis_nlsp_area_addrs *areaptr;
    int area_count;
    int area_ix;

    area_count = area_item->length / ISIS_NLSP_AREA_ADDRS_LEN;
    areaptr = (isis_nlsp_area_addrs *) area_item->value;
    for (area_ix = 0; area_ix < area_count; area_ix++, areaptr++) {
	adj->ipx_area_net[area_ix] = GETLONG(&areaptr->net_number);
	adj->ipx_area_mask[area_ix] = GETLONG(&areaptr->net_mask);
    }
    adj->ipx_area_count = area_count;
}

/*
 * isis_nlsp_accept_adjacency
 *
 * Accept adjacency for NLSP WAN interfaces.
 *
 * Returns new circuit state.
 */
static int isis_nlsp_accept_adjacency (clns_adjtype *adj, idbtype *idb,
				       clns_pdbtype *pdb, NSAP_T nbr_id, 
				       paktype *pak,
				       isis_pt_IIH_hdrtype *isis,
				       isis_info_type *area_item,
				       boolean state_changed)
{
    ulong          hold_time;
    hwaddrtype     snpa;
    boolean        new_adj;
    ipxhdrtype     *ipx;
    isisidbtype    *isisidb;
    boolean	   id_changed;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return(CLNS_ADJ_FAILED_STATE);

    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);
    hold_time = GETSHORT(isis->ht);

    /* 
     * If adjacency not in database, create entry. Otherwise,
     * update expiration time and metric.
     */
    if (!adj) {
	adj = add_adjacency(pdb->adjacency_db, nbr_id, idb, &snpa, hold_time,
			    TRUE);
	if (!adj) return(CLNS_ADJ_FAILED_STATE);
	adj->num_areas = 0;
	adj->state = CLNS_ADJ_FAILED_STATE;
	adj_pbuginf(pdb, "New serial adjacency");
	new_adj = TRUE;
	adj->adjacencyType = CLNS_NLSP_IS_ADJ_TYPE;
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
    } else {
	TIMER_START(adj->expiration_time, hold_time * ONESEC);
	new_adj = FALSE;
    }
    isis_start_adj_timer(pdb, adj);
    adj->protocol_source |= pdb->typeflag;

    /*
     * Update the adjacency count on this interface. If it is a new
     * adjacency or the existing adjacency went from down to up, increment the
     * count.  Purge any pseudonode LSP for this interface, since we
     * now have an active adjacency.
     */
    if (adj->state != CLNS_ADJ_UP_STATE) {
	isis_update_adj_count(pdb, adj, TRUE);
	isis_log_adjacency_change(pdb, adj, TRUE,
				  new_adj ? "new adjacency" : "restarted");
	GET_TIMESTAMP(adj->create_time); /* Show that it cycled. */
	isis_reset_circuit_id(pdb, idb);
	isis_purge_dr(ISIS_CIRCUIT_L1, pdb, isisidb);
    }
    /*
     * Set adjacency state.
     */
    adj->state = CLNS_ADJ_UP_STATE;
    adj->L1_circuit_id[0] = isis->local_circuit_id;

    /* Update the circuit ID.  Blow the adjacency if it changed. */

    id_changed = isis_compute_p2p_circuit_id(pdb, idb, isisidb, isis);

    if (!state_changed && id_changed) {
	isis_log_adjacency_change(pdb, adj, FALSE, "circuit ID changed");
	delete_adjacency(pdb->adjacency_db, adj);
	return(CLNS_ADJ_FAILED_STATE);
    }
    isis_nlsp_area_copy(adj, area_item);

    /* Get the source address for next-hop purposes. */

    ipx = (ipxhdrtype *) ipxheadstart(pak);
    ieee_copy(ipx->shost, adj->ipx_node);
    adj->ipx_net = GETLONG(ipx->ssnet);
    adj->ipx_version = isis->version_pid;

    /* %%% We're supposed to save the min MTU on the circuit. */

    if (state_changed) {

	/*
	 * We want to minimize the possibility of flooding large numbers
	 * of LSPs across this link.  We do a couple of things to make
	 * this happen.  First, we delay the initial CSNP for a few
	 * seconds.  This allows us to receive the latest LSPs on the LAN
	 * side if we are just starting up, so that our CSNP shows that
	 * we have half a clue.  Then we hold down the flooding of LSPs
	 * for a few seconds after we send our CSNP in order for the other
	 * side to have a chance to do the same thing.  Receipt of his
	 * CSNP will likely turn off the SRM bits for a bunch of the LSPs.
	 * This works best between two ciscos, and won't hurt anything
	 * if the other side isn't a cisco.
	 */
	mgd_timer_start(&isisidb->csnp_ctl[0].isis_csnp_timer,
			ISIS_P2P_CSNP_WAIT);
	mgd_timer_start(&isisidb->isis_dr_timer,
			ISIS_P2P_CSNP_WAIT + ISIS_P2P_LSP_WAIT);

	/* Set SRM bits in the appropriate database. */

	isis_mark_L1_changed(pdb);
	isis_recursive_walk(NULL, ISIS_WALK_ALL, pdb->level1_LSPs,
			    isis_rw_set_srm, idb, pdb, NULL, NULL);
    }
    return(CLNS_ADJ_UP_STATE);
}

/*
 * isis_process_nlsp_serial_iih
 *
 * An NLSP hello packet was received on a serial link.
 */

/*				  adjacency state    received state  */
static const uchar isis_p2p_adj_state[CLNS_ADJ_NUMSTATES][CLNS_ADJ_NUMSTATES] =

/* Adjacency state == up: */
/*    Up                  Init              Down          */
{{CLNS_ADJ_UP_STATE, CLNS_ADJ_UP_STATE, CLNS_ADJ_INIT_STATE},

/* Adjacency state == init: */
/*    Up                  Init              Down          */
 {CLNS_ADJ_UP_STATE, CLNS_ADJ_UP_STATE, CLNS_ADJ_INIT_STATE},

/* Adjacency state == down: */
/*    Up                  Init              Down          */
 {CLNS_ADJ_FAILED_STATE, CLNS_ADJ_INIT_STATE, CLNS_ADJ_INIT_STATE}};

static void isis_process_nlsp_serial_iih (paktype *pak, 
					  clns_pdbtype *pdb, 
					  idbtype *idb, 
					  isis_pt_IIH_hdrtype *isis, 
					  isis_info_type *item,
					  char *packet_end, 
					  isis_TLV_parse *tlv_parse)
{
    NSAP_T         key;
    clns_adjtype   *adj;
    int		   oldstate, newstate;
    isisidbtype    *isisidb;
    boolean	   state_changed;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /* Look up any adjacency for the neighbor. */

    station_copy(isis->source_id, &key[1]);
    key[0] = STATION_LENGTH;
    adj = find_adjacency(pdb->adjacency_db, key, FALSE, idb,
			 CLNS_NLSP_IS_ADJ_TYPE);

    /* Check for L2-only circuit type.  If there, tear down any adjacency. */

    if (isis->circuit_type == ISIS_CIRCUIT_L2) {
	if (adj) {
	    isis_log_adjacency_change(pdb, adj, FALSE, "level mismatch");
	    delete_adjacency(pdb->adjacency_db, adj);
	}
	return;
    }

    /* Do basic validation. */

    if (!isis_validate_basic_nlsp_hello(pdb, tlv_parse, adj)) {
	return;
    }

    /* Croak if state variable is out of range. */

    if (isis->state > CLNS_ADJ_MAXSTATE) {
	adj_pbuginf(pdb, "Improperly formatted IIH");
	return;
    }

    /*
     * Now we go through our state table, based on the provided state
     * in the hello and our circuit state.
     */
    oldstate = isisidb->isis_if_state;
    newstate = isis_p2p_adj_state[oldstate][isis->state];
    adj_pbuginf(pdb, "old state %d, new state %d",
		oldstate, newstate);

    /* If the state changed, send another hello. */

    state_changed = (oldstate != newstate);
    if (state_changed)
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);

    /* If the new state is "up", do the adjacency acceptance dance. */

    if (newstate == CLNS_ADJ_UP_STATE) {
	isisidb->isis_if_state =
	    isis_nlsp_accept_adjacency(adj, idb, pdb, key, pak, isis,
				       tlv_parse->
				       TLV_array[ISIS_NLSP_AREA_ADDRS],
				       state_changed);
    } else {
	isisidb->isis_if_state = newstate;
    }
}


/*
 * isis_copy_hello_tlvs
 *
 * Update an adjacency based on information in an IIH.
 *
 * Parameters:	item = pointer to first TLV in packet
 *		packet_end = pointer to end+1 of packet
 *		adj = adjacency entry for this neighbor
 *		pdb = our PDB
 *		if_input = interface hello received upon
 */
static void isis_copy_hello_tlvs (clns_adjtype *adj,
				  clns_pdbtype *pdb,
				  idbtype *if_input,
				  isis_TLV_parse *tlv_parse)
{
    isis_info_type *item;
    NSAP_T         *area_from_packet;
    ipaddrtype     *ipaddr;
    int		   i;
    isis_nlsp_mgmt_info *info;

    /* Clear IP and IPX addresses for adjacency. */

    for (i = 0; i < MAXROUTES; i++) {
	adj->ip_address[i] = 0;
    }
    adj->ipx_net = 0;
    
    /* Area addresses.  Write 'em down. */

    item = tlv_parse->TLV_array[ISIS_AREA_ADDRESSES];
    if (item) {
	isis_info_type *next_item;
	next_item = item;
	ADVANCE_ITEM(next_item, TRUE);

	area_from_packet = (NSAP_T *)item->value;
	while ((char *)area_from_packet < (char *)next_item) {
	    if (!area_in_adj(*(NSAP_T *) area_from_packet, adj)) {
		add_area_to_adj(*(NSAP_T *) area_from_packet, adj);
	    }
	    area_from_packet = (NSAP_T *)
		(((char *) area_from_packet) + *area_from_packet[0]+1);
	}		    
    }

    /* IP interface addresses.  Write 'em down. */

    item = tlv_parse->TLV_array[ISIS_IP_IF_ADDRESSES];
    if (item) {
	isis_info_type *next_item;
	next_item = item;
	ADVANCE_ITEM(next_item, TRUE);

	ipaddr = (ipaddrtype *) &item->value;
	for (i = 0; i < MAXROUTES; i++) {
	    adj->ip_address[i] = GETLONG(ipaddr);
	    ipaddr++;
	    if ((char *)ipaddr >= (char *)next_item) break;
	}
    }

    /* IPX management info.  Write it down. */
	    
    item = tlv_parse->TLV_array[ISIS_NLSP_MGMT_INFO];
    if (item) {
	info = (isis_nlsp_mgmt_info *) item->value;
	adj->ipx_net = GETLONG(&info->net_number);
	bcopy(info->node_number, adj->ipx_node, IEEEBYTES);
    }
}


/*
 * State tables for ISIS serial adjacency establishment
 *
 * The state tables are indexed first by circuit type and then by
 * adjacency type (L1, L2, L1L2 in both cases).  A non-existent adjacency
 * is considered to be of type NONE (0).  The circuit type is offset by 1,
 * since there is no NONE circuit type.
 *
 * There are four tables, three for use with neighbors that have matching
 * area addresses (for L1-only, L2-only, and L1L2 routers) and one for
 * use with neighbors that have no matching area addresses (L2-only or L1L2).
 */

typedef struct isis_p2p_trans_ {
    uchar action;			/* down/up/accept */
    uchar adj_type;			/* L1, L2, L1L2 */
} isis_p2p_trans;

enum {ACTION_DOWN, ACTION_UP, ACTION_ACCEPT};

/* Some defines to make the tables look prettier */

#define A_DOWN ACTION_DOWN
#define A_UP   ACTION_UP
#define A_ACC  ACTION_ACCEPT

#define C_1 ISIS_CIRCUIT_L1
#define C_2 ISIS_CIRCUIT_L2
#define C_12 ISIS_CIRCUIT_L1L2

/* Tables for neighbors with matching area addresses */

/*
 * L1-only routers
 *                                circuit type           adj type */
static const isis_p2p_trans L1_only[ISIS_NUM_CIRC_TYPES-1][ISIS_NUM_CIRC_TYPES] =
{
    /*
     * Circuit type = L1 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_1 }, {A_ACC , 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L2 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L1L2
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_1 }, {A_ACC , 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }}};

/*
 * L2-only routers
 *                                circuit type           adj type */
static const isis_p2p_trans L2_only[ISIS_NUM_CIRC_TYPES-1][ISIS_NUM_CIRC_TYPES] =
{
    /*
     * Circuit type = L1 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L2 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_2 }, {A_DOWN, 0   }, {A_ACC , 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L1L2
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_2 }, {A_DOWN, 0   }, {A_ACC , 0   }, {A_DOWN, 0  }}};

/*
 * L1L2 routers
 *                                circuit type           adj type */
static const isis_p2p_trans L1_L2[ISIS_NUM_CIRC_TYPES-1][ISIS_NUM_CIRC_TYPES] =
{
    /*
     * Circuit type = L1 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_1 }, {A_ACC , 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L2 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_2 }, {A_DOWN, 0   }, {A_ACC , 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L1L2
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_12}, {A_DOWN, 0   }, {A_DOWN, 0   }, {A_ACC , 0  }}};


/* Tables for neighbors with non-matching area addresses */

/*
 * L1L2 or L2-only routers, please.
 *                                circuit type           adj type */
static const isis_p2p_trans no_match[ISIS_NUM_CIRC_TYPES-1][ISIS_NUM_CIRC_TYPES] =
{
    /*
     * Circuit type = L1 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L2 only
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_2 }, {A_DOWN, 0   }, {A_ACC , 0   }, {A_DOWN, 0  }},
    /*
     * Circuit type = L1L2
     *  no adj          L1 adj          L2 adj          L1L2 adj */
    {{A_UP  , C_2 }, {A_DOWN, 0   }, {A_ACC , 0   }, {A_DOWN, 0  }}};

/*
 * Table to look up proper table indexed by router type
 */
static const isis_p2p_trans (*trans_tables[])[][] =
    {NULL, &L1_only, &L2_only, &L1_L2};


/*
 *isis_process_isis_serial_iih
 *
 * An IS-IS hello packet was received on a serial link.
 *
 */
static void isis_process_isis_serial_iih (paktype *pak, 
					  clns_pdbtype *pdb, 
					  idbtype *idb, 
					  isis_pt_IIH_hdrtype *isis, 
					  isis_info_type *item,
					  char *packet_end, 
					  isis_TLV_parse *tlv_parse)
{
    clns_adjtype   *adj;
    boolean	   adjacency_allowed;
    boolean	   area_match;
    NSAP_T         key;
    isisidbtype    *isisidb;
    const isis_p2p_trans (*table)[ISIS_NUM_CIRC_TYPES-1][ISIS_NUM_CIRC_TYPES];
    ulong action;
    ulong adj_level;
    ulong new_adj_type;
    ulong pdb_type;
    hwaddrtype     snpa;
    ipaddrtype	nexthop_ipaddr;
    ulong oldstate, newstate;
    boolean state_changed, going_up, going_down;
    char *log_string;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * Verify authentication, if any.
     */
    if (!isis_check_auth(item, packet_end, NULL, isisidb, 
			 isis->circuit_type)) {
	adj_pbuginf(pdb, "Authentication failed");
	return;
    }
    if (tlv_parse->TLV_array[ISIS_AREA_ADDRESSES] == NULL) {
	adj_pbuginf(pdb, "Improperly formatted serial IIH");
	return;
    }

    station_copy(isis->source_id, &key[1]);
    key[0] = STATION_LENGTH;

    /*
     * Walk all of the area addresses in the hello packet.  Run each effective
     * NET of our neighbor through the adjacency filter and note the
     * results.  If we must match all area addresses, AND the results
     * together;  otherwise, OR the results.
     */
    adjacency_allowed = (isisidb->match_all_areas || 
			 !isisidb->isis_adj_filter);
    item = tlv_parse->TLV_array[ISIS_AREA_ADDRESSES];
    area_match = isis_area_address_match(item, pdb);
    if (isisidb->isis_adj_filter) {
	boolean filter_result = isis_filter_adj(item, isis->source_id, 
						isisidb->isis_adj_filter,
						isisidb->match_all_areas);
	if (isisidb->match_all_areas)
	    adjacency_allowed = adjacency_allowed && filter_result;
	else
	    adjacency_allowed = adjacency_allowed || filter_result;
    }
    if (!adjacency_allowed) {
	adj_pbuginf(pdb, "Adjacency disallowed by filter");
	return;
    }

    /*
     * Check ISIS mode. Check the IP interface addresses in the IIH.
     * If none of them is usable as next hop, drop the IIH. 
     */
    nexthop_ipaddr = reg_invoke_isis_valid_ip_if_address(pdb, idb, tlv_parse);
    if (nexthop_ipaddr == ISIS_INVALID_IP_NEXTHOP) {
	adj_pbuginf(pdb, "No usable IP interface addresses in serial IIH");
	return;
    }

    /* 
     * Attempt to find neighbor in adjacency database.
     */
    adj = find_adjacency(pdb->adjacency_db, key, FALSE, idb,
			 CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE);

    /*
     * If a Neighbor adjacency state option is present, perform the
     * three-way handshake, a la NLSP.
     */

    item = tlv_parse->TLV_array[ISIS_SERIAL_CIRC_STATE];
    oldstate = isisidb->isis_if_state;
    newstate = CLNS_ADJ_UP_STATE;	/* In case it's not there */
    if (item) {

	/*
	 * Now we go through our state table, based on the provided state
	 * in the hello and our circuit state.
	 */
	newstate = isis_p2p_adj_state[oldstate][item->value[0]];
	adj_pbuginf(pdb, "rcvd state %d, old state %d, new state %d",
		    item->value[0], oldstate, newstate);
    }
    isisidb->isis_if_state = newstate;
    state_changed = (oldstate != newstate);
    going_up = (newstate == CLNS_ADJ_UP_STATE && state_changed);
    going_down = (oldstate == CLNS_ADJ_UP_STATE && state_changed);

    /* If the state changed, send another hello. */

    if (state_changed)
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);

    /*
     * Next, determine the proper state table to use.  If there are matching
     * area addresses, look up the table based on the router type.
     */
    new_adj_type = 0;
    action = ACTION_DOWN;
    log_string = "incompatible level";
    pdb_type = pdb->level & isisidb->isis_circuit_type;
    if (!pdb_type || (pdb_type > ISIS_CIRCUIT_L1L2)) {
	table = NULL;
	adj_pbuginf(pdb, "Invalid PDB level");
    } else {
	if (area_match) {
	    table = trans_tables[pdb_type];
	} else {
	    /*
	     * No area match.  If this is an L1-only router, punt;  otherwise,
	     * use the no-match table.
	     */
	    if (pdb_type == ISIS_CIRCUIT_L1) {
		table = NULL;
		adj_pbuginf(pdb, "No matching areas");
	    } else {
		table = &no_match;
	    }
	}
    }
    
    /*
     * OK, now determine the action to take.  If there is no table, the
     * action is is DOWN; otherwise, look it up.  The adjacency level
     * is zero if there is no adjacency; otherwise we get it from the
     * adjacency.  An adjacency going from half-up to up counts as no
     * adjacency in this case.  An adjacency going from up to any other
     * state gets an action of DOWN.
     */
    adj_level = 0;
    if (going_down) {
	log_string = "neighbor forgot us";
    } else {
	if (table) {
	    if (adj && newstate == CLNS_ADJ_UP_STATE && !going_up) {
		if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) {
		    if (adj->L2only) {
			adj_level = ISIS_CIRCUIT_L2;
		    } else {
			adj_level = ISIS_CIRCUIT_L1L2;
		    }
		} else if (adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE) {
		    adj_level = ISIS_CIRCUIT_L1;
		}
	    }
	    action = (*table)[isis->circuit_type - 1][adj_level].action;
	    new_adj_type =
		(*table)[isis->circuit_type - 1][adj_level].adj_type;
	}
    }
    adj_pbuginf(pdb, "Action = %d, new_type = %d", action, new_adj_type);
    
    /*
     * We have an action, and if the action is UP, we have an adjacency
     * type.  Perform the proper action.
     */
    switch (action) {

      case ACTION_DOWN:

	/* Down.  If there's an adjacency, delete it. */

	if (adj) {
	    isis_log_adjacency_change(pdb, adj, FALSE, log_string);
	    delete_adjacency(pdb->adjacency_db, adj);
	    adj = NULL;
	}
	break;

      case ACTION_UP:

	/* Compute the circuit id. */

	isis_compute_p2p_circuit_id(pdb, idb, isisidb, isis);

	/* Create an adjacency for the circuit. */

	if (!adj) {
	    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);
	    adj = add_adjacency(pdb->adjacency_db, key, idb, &snpa, 0, FALSE);
	    if (adj) {
		adj->num_areas = 0;
		adj_pbuginf(pdb, "New serial adjacency");
	    }
	}
	if (adj) {
	    if (new_adj_type & ISIS_CIRCUIT_L2) {
		adj->adjacencyType = CLNS_L2_IS_ADJ_TYPE;
	    } else {
		adj->adjacencyType = CLNS_L1_IS_ADJ_TYPE;
	    }
	    adj->L2only = (new_adj_type == ISIS_CIRCUIT_L2);
	    adj->protocol_source |= pdb->typeflag;
	    adj->state = newstate;
	    adj->L1_circuit_id[0] = isis->local_circuit_id;
	    adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;

	    if (going_up) {
		isis_log_adjacency_change(pdb, adj, TRUE, "new adjacency");

		/* Update the adjacency count. */
		
		isis_update_adj_count(pdb, adj, TRUE);
		GET_TIMESTAMP(adj->create_time); /* Show that it cycled. */
		
		/*
		 * Use isis_dr_timer to determine when to flood all LSPs to
		 * neighbor. This leaves some time for us to receive his 
		 * CSNP packet, which may reduce the number of LSPs that
		 * require flooding on the point-to-point link.
		 */
		mgd_timer_start(&isisidb->isis_dr_timer,
				ISIS_P2P_CSNP_WAIT + ISIS_P2P_LSP_WAIT);
		
		/*
		 * Set SRM bits from appropriate database. Then send CSNP so we
		 * can get his latest database.  Delay sending the CSNP a
		 * bit so the adjacency has time to come up.
		 */
		if (new_adj_type && ISIS_CIRCUIT_L1) {
		    isis_mark_L1_changed(pdb);
		    isis_recursive_walk(NULL, ISIS_WALK_ALL, pdb->level1_LSPs,
					isis_rw_set_srm, idb, pdb, NULL, NULL);
		    mgd_timer_start(&isisidb->csnp_ctl[0].isis_csnp_timer,
				    ISIS_P2P_CSNP_WAIT);
		}
		if (new_adj_type && ISIS_CIRCUIT_L2) {
		    isis_mark_L2_changed(pdb);
		    isis_recursive_walk(NULL, ISIS_WALK_ALL, pdb->level2_LSPs,
					isis_rw_set_srm, idb, pdb, NULL, NULL);
		    mgd_timer_start(&isisidb->csnp_ctl[1].isis_csnp_timer,
				    ISIS_P2P_CSNP_WAIT);
		}
	    }
	}
	break;

      case ACTION_ACCEPT:

	/* Accept.  Make sure the circuit ID hasn't changed. */

	if (isis_compute_p2p_circuit_id(pdb, idb, isisidb, isis)) {
	    isis_log_adjacency_change(pdb, adj, FALSE, "circuit ID changed");
	    delete_adjacency(pdb->adjacency_db, adj);
	    adj = NULL;
	}
    }

    /* Bail out if we're done. */

    if (!adj)
	return;

    /* Update the adjacency timer. */
    
    TIMER_START(adj->expiration_time, GETSHORT(isis->ht) * ONESEC);
    isis_start_adj_timer(pdb, adj);

    /*
     * Copy area addresses from PDU to adjacency entry. 
     */
    isis_copy_hello_tlvs(adj, pdb, idb, tlv_parse);
    adj->ip_nexthop = nexthop_ipaddr;
    adj->ipx_version = isis->version_pid;
}


/*
 * isis_rcv_serial_iih
 *
 * An IS-IS hello packet was recevied on a serial link.
 *
 */
void isis_rcv_serial_iih (paktype *pak, clns_pdbtype *pdb, 
			  idbtype *idb, isishdrtype *isis_hdr)
{
    isis_pt_IIH_hdrtype *isis;
    char           *packet_end;
    hwaddrtype     source_snpa;
    isis_TLV_parse *tlv_parse;
    isis_info_type *item;

    isis = (isis_pt_IIH_hdrtype *) isis_hdr;

    if (isis_nlsp_adj_packet_debug[pdb->proc_type]) {
	(*idb->hwptr->extract_snpa)(pak, &source_snpa, SOURCE);
	buginf("\n%s-Adj: Rec serial IIH from %s on %s, "
	       "cir type %d, cir id %02x", 
	       pdb->errmsg_name, snpa_string (&source_snpa), idb->namestring,
	       isis->circuit_type, isis->local_circuit_id);
    }

    /*
     * If IIH received from this system, ignore.
     */
    if (station_match(pdb->systemId, isis->source_id)) {
	adj_pbuginf(pdb, "Duplicate system id");
	return;
    }

    /* Validate packet. */

    tlv_parse = isis_valid_packet(pak, isis, pdb);
    if (tlv_parse == NULL)
	return;

    item = (isis_info_type *)isis->item;
    packet_end = ((char *)isis) + GETSHORT(isis->pdu_length);

    /* Split off for NLSP versus ISIS. */

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis_process_nlsp_serial_iih(pak, pdb, idb, isis, item,
				     packet_end, tlv_parse);
    } else {
	isis_process_isis_serial_iih(pak, pdb, idb, isis, item,
				     packet_end, tlv_parse);
    }
    free(tlv_parse);
}

/*
 * isis_process_nlsp_lan_iih
 *
 * Process LAN IIHs for ISIS
 */
static void isis_process_nlsp_lan_iih(paktype *pak, clns_pdbtype *pdb,
				      idbtype *idb, 
				      isis_lan_IIH_hdrtype *isis, 
				      isis_info_type *item, int level,
				      boolean he_knows_us,
				      hwaddrtype *source_snpa,
				      uchar *packet_end, 
				      isis_TLV_parse *tlv_parse)
{
    clns_adjtype *adj, *loop_adj;
    uchar key[SYSTEMID_LENGTH];
    int hold_time;
    nidbtype *nidb;
    ipxhdrtype *ipx;
    int new_state;
    isisidbtype *isisidb;
    boolean going_up;
    const char *updown_string;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * Get source system id. 
     */
    station_copy(isis->source_id, (uchar *) &key[1]);
    key[0] = STATION_LENGTH;

    /*
     * Source net nummber must match the interface net address.
     */

    nidb = IPX_PRIMARY_NIDB(idb);
    ipx = (ipxhdrtype *) ipxheadstart(pak);
    if (GETLONG(ipx->ssnet) != nidb->novellnet) {
	adj_pbuginf(pdb, "Source IPX net mismatch");
	return;
    }
    
    /* Source host must match MAC address. */
    
    if (bcmp(ipx->shost, source_snpa->addr, IEEEBYTES)) {
	adj_pbuginf(pdb, "Source IPX node mismatch");
	return;
    }
    
    /* Do basic validation. */
    
    adj = find_adjacency(pdb->adjacency_db, key, FALSE, idb,
			 CLNS_NLSP_IS_ADJ_TYPE);
    if (!isis_validate_basic_nlsp_hello(pdb, tlv_parse, adj))
	return;
    hold_time = GETSHORT(isis->ht);

    for (loop_adj = adj; loop_adj; loop_adj = loop_adj->next_alias) {
	
	/*
	 * Validate the SNPA against the one in the adjacency.  Loop
	 * through the next_alias list to find the matching SNPA.
	 * If a match does not exist, adj is assigned to NULL and
	 * this adjacency will be added to the next_alias list.
	 */
	
	if (snpa_equ(source_snpa, &loop_adj->snpa)) {
	    break;
	}
    }
    adj = loop_adj;

    if (adj) {
	
	TIMER_START(adj->expiration_time, hold_time * ONESEC);
	
    } else {

	/* 
	 * There wasn't an adjacency yet.  Create one.
	 */
	adj = add_adjacency(pdb->adjacency_db, key, idb, source_snpa,
			    hold_time, TRUE);
	if (!adj)
	    return;
	adj->adjacencyType = CLNS_NLSP_IS_ADJ_TYPE;
	adj->protocol_source |= pdb->typeflag;
	station_copy(isis->source_id, &adj->key[1]);
	adj->key[0] = STATION_LENGTH;
	snpa_copy(source_snpa, &adj->snpa);
	adj->state = CLNS_ADJ_INIT_STATE;
	adj_pbuginf(pdb, "New adjacency for %s",
		    snpa_string(source_snpa));
	adj->L1_lan_priority = ISIS_MAX_PRIO + 1;
	adj->L2_lan_priority = ISIS_MAX_PRIO + 1; /* So we detect a change */
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
    }

    isis_start_adj_timer(pdb, adj);

    /* Pick up the IPX address from the packet. */

    ieee_copy(ipx->shost, adj->ipx_node);
    adj->ipx_net = GETLONG(ipx->ssnet);
    adj->ipx_version = isis->version_pid;

    if (adj->ipx_version != NLSP_VERSION) {
	adj_pbuginf(pdb, "IIH minor version %d", adj->ipx_version);
    }

    /* Pick up the area addresses. */
    
    isis_nlsp_area_copy(adj, tlv_parse->TLV_array[ISIS_NLSP_AREA_ADDRS]);

    /*
     * Figure out whether the state changed.  If he knows us, we're
     * up;  if not, we're init.  Flag DR election if there was a change.
     */
    new_state = (he_knows_us) ? CLNS_ADJ_UP_STATE : CLNS_ADJ_INIT_STATE;
    if (new_state != adj->state) {
	going_up = (new_state == CLNS_ADJ_UP_STATE);
	isis_update_adj_count(pdb, adj, going_up);
	if (going_up) {
	    updown_string = "new adjacency";
	    GET_TIMESTAMP(adj->create_time); /* Show that it cycled. */
	} else {
	    updown_string = "neighbor forgot us";
	}
	isis_log_adjacency_change(pdb, adj, going_up, updown_string);
	adj->state = new_state;
	isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L1);
	isis_mark_L1_dr_changed(pdb, isisidb);
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
	isis_mark_L1_changed_delayed(pdb);
	adj_pbuginf(pdb, "Adjacency changed to %s",
		    (new_state == CLNS_ADJ_UP_STATE) ? "up" : "init");
	/*
	 * If we're hearing from the DR, abort the DR election holddown
	 * (NLSP allows this).  This will more rapidly bring us up.
	 */
	if (mgd_timer_running(&isisidb->isis_dr_timer) &&
	    station_match(isis->source_id, isis->lan_id)) {
	    mgd_timer_start(&isisidb->isis_dr_timer, 0);  /* Do it now. */
	    adj_pbuginf(pdb, "DR adjacency up, election holddown aborted");
	}
	/*
	 * If we're the DR and the adjacency is up, send a CSNP so the guy can
	 * rapidly figure out what LSPs he's missing.
	 */
	if (new_state == CLNS_ADJ_UP_STATE && !is_p2p(idb) &&
	    isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L1)) {
	    mgd_timer_start(&isisidb->csnp_ctl[0].isis_csnp_timer, 0);
	}
    }

    /* Flag DR election if the other guy changed priority and he's up. */

    if (new_state == CLNS_ADJ_UP_STATE && adj->L1_lan_priority != isis->prio) {
	adj->L1_lan_priority = isis->prio;
	isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L1);
    }

    /*
     * Save the circuit id.  If it changed, force DR election.  This is not
     * in the spec, but is necessary because if we elected this guy DR
     * before he did so himself, we do not know his pseudonode ID (he
     * won't report it until he elects himself).  This check will make
     * us run DR election a second time when he gets around to it.
     */
    if (!systemid_match(isis->lan_id, adj->L1_circuit_id)) {
	isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L1);
    }
    systemid_copy(isis->lan_id, adj->L1_circuit_id);
}


/*
 * isis_process_isis_lan_iih
 *
 * Process LAN IIHs for ISIS
 */
static void isis_process_isis_lan_iih (paktype *pak, clns_pdbtype *pdb,
				       idbtype *idb, 
				       isis_lan_IIH_hdrtype *isis,
				       isis_info_type *item, int level,
				       boolean he_knows_us,
				       hwaddrtype *source_snpa, 
				       uchar *packet_end,
				       isis_TLV_parse *tlv_parse)
{
    boolean adjacency_allowed;
    clns_adjtype *adj;
    uchar key[SYSTEMID_LENGTH];
    int hold_time;
    isisidbtype *isisidb;
    ipaddrtype nexthop_ipaddr;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * Verify authentication, if any.
     */
    if (!isis_check_auth(item, packet_end, NULL, isisidb, level)) {
	adj_pbuginf(pdb, "Authentication failed");
	return;
    }

    /*
     * Get source system id. 
     */
    station_copy(isis->source_id, (uchar *) &key[1]);
    key[0] = STATION_LENGTH;

    /* Bitch if there's no area address option. */

    if (tlv_parse->TLV_array[ISIS_AREA_ADDRESSES] == NULL) {
	adj_pbuginf(pdb, "Improperly formatted LAN IIH");
	return;
    }
    /*
     * Match on area address if Level 1 hello received.
     *
     * Walk all of the area addresses in the hello packet.  Note the
     * first area address that we have in common.  Run each effective
     * NET of our neighbor through the adjacency filter and note the
     * results.  If we must match all area addresses, AND the results
     * together;  otherwise, OR the results.
     */
    adjacency_allowed = (isisidb->match_all_areas || 
			 !isisidb->isis_adj_filter);
    item = tlv_parse->TLV_array[ISIS_AREA_ADDRESSES];
    if (level == ISIS_CIRCUIT_L1 && !isis_area_address_match(item, pdb)) {
	adj_pbuginf(pdb, "Area mismatch, level %d IIH on %s", 
		    level, idb->namestring);
	return;
    }
    if (isisidb->isis_adj_filter) {
	boolean filter_result = isis_filter_adj(item, isis->source_id, 
						isisidb->isis_adj_filter,
						isisidb->match_all_areas);
	if (isisidb->match_all_areas)
	    adjacency_allowed = adjacency_allowed && filter_result;
	else
	    adjacency_allowed = adjacency_allowed || filter_result;
    }
    if (!adjacency_allowed) {
	adj_pbuginf(pdb, "Adjacency disallowed by filter");
	return;
    }

    /*
     * Check ISIS mode. Check the IP interface addresses in the IIH.
     * If none of them is usable as next hop, drop the IIH. 
     */
    nexthop_ipaddr = reg_invoke_isis_valid_ip_if_address(pdb, idb, tlv_parse);
    if (nexthop_ipaddr == ISIS_INVALID_IP_NEXTHOP) {
	adj_pbuginf(pdb, "No usable IP interface addresses in LAN IIH");
	return;
    }

    adj = find_adjacency(pdb->adjacency_db, key, FALSE, idb,
			 CLNS_ALL_ADJ_TYPE);
    if (adj) {

	/*
	 * Validate the SNPA against the one in the adjacency.  If the
	 * SNPA is not the same, ignore the IIH.  This is probably because
	 * two LANs are bridged or wired together and we want to preserve
	 * the existing adjacency.  If in fact the user was able to rapidly
	 * swap an ethernet card, then we will toss the IIHs until the old
	 * adjacency times out, at which point we will accept the new one.
	 */

	if (!snpa_equ(source_snpa, &adj->snpa)) {
	    adj_pbuginf(pdb, "new SNPA %s doesn't match "
			"old SNPA %s--possible bridged LANs", 
			snpa_string(source_snpa), snpa_string(&adj->snpa));
	    return;
	}

	/*
	 * If adjacency level has changed or adjacency created by ES-IS.
	 */
	if (level == ISIS_CIRCUIT_L1) {
	    if ((adj->ipx_version != isis->version_pid) ||
		((adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) && adj->L2only) ||
 		(adj->adjacencyType == CLNS_IS_ADJ_TYPE) ||
 		(adj->adjacencyType == CLNS_ES_ADJ_TYPE)) {
		isis_mark_L1_changed(pdb);
		isis_mark_L1_dr_changed(pdb, isisidb);
		goto new_adj;
	    }
	}
	if ((level == ISIS_CIRCUIT_L2) &&
	    (adj->adjacencyType != CLNS_L2_IS_ADJ_TYPE)) {
	    isis_mark_L2_changed(pdb);
	    isis_mark_L2_dr_changed(pdb, isisidb);
	    goto new_adj;
	}

	/*
	 * Existing adjacency is legit. Update the holding timer, LAN Priority
	 * and neighbor areas.
	 */
	TIMER_START(adj->expiration_time, GETSHORT(isis->ht) * ONESEC);
	isis_start_adj_timer(pdb, adj);

	/*
	 * Fill in area addresses and IP addresses from packet.
	 */
	isis_copy_hello_tlvs(adj, pdb, idb, tlv_parse);
	adj->ip_nexthop = nexthop_ipaddr;

	/*
	 * If the other side had seen us, we need to check for change in
	 * priority and the DR advertised by the other side.
	 * The following is the rule to trigger DR election :
	 * a) When a new adj comes set the priority to ISIS_MAX_PRIO+1.
	 *    This will trigger an DR election once the adj goes up.
	 * b) When the adj goes from up->init state, set the lan priority
	 *    for both L1 and L2 to ISIS_MAX_PRIO+1. This will trigger
	 *    DR election once we go up.
	 * c) When he had seen us, if there is a change in the DR advertised
	 *    by the other side, it could indicate that he had elected 
	 *    himself as DR. We must run DR election.
	 */
	if (he_knows_us) {
	    if ((level == ISIS_CIRCUIT_L1) &&
		((adj->L1_lan_priority != isis->prio) ||
		 !systemid_match(adj->L1_circuit_id, isis->lan_id))) {
		adj->L1_lan_priority = isis->prio;
		isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L1);
	    }
	    if ((level== ISIS_CIRCUIT_L2) && 
		((adj->L2_lan_priority != isis->prio) ||
		 !systemid_match(adj->L2_circuit_id, isis->lan_id))){
		adj->L2_lan_priority = isis->prio;
		isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L2);
	    }

	    /*
	     * Save the local circuit id. May need it for the
	     * lan id later if this will end up being the DR.
	     */
	    systemid_copy(isis->lan_id, (level == ISIS_CIRCUIT_L1) ?
			  adj->L1_circuit_id : adj->L2_circuit_id);
	 

	    /*
	     * If the adjacency is moving to the Up state, rebuild the
	     * appropriate LSPs to reflect the new adjacency.  Because
	     * the adjacency database is overloaded with L1 and L2 state
	     * in a single adjacency, we're never quite sure what state
	     * was present before, so we build both L1 and L2 LSPs if the
	     * adjacency is L1L2.
	     *
	     * Since the LSP generation rate limiting code may make us
	     * not notice the change, explicitly trigger an SPF run at
	     * the appropriate levels to make sure that it happens.
	     */
	    if (adj->state != CLNS_ADJ_UP_STATE) {
		if (level == ISIS_CIRCUIT_L1) {
		    isis_mark_L1_changed(pdb);
		    isis_mark_L1_dr_changed(pdb, isisidb);
		    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_NEWADJ,
				     NULL, TRUE);
		    if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) { /* L1L2 */
			isis_mark_L2_dr_changed(pdb, isisidb);
			isis_mark_L2_changed(pdb);
			isis_trigger_spf(pdb, CLNS_LEVEL2,
					 ISIS_SPF_TRIG_NEWADJ, NULL, TRUE);
		    }
		} else {
		    isis_mark_L2_changed(pdb);
		    isis_mark_L2_dr_changed(pdb, isisidb);
		    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_NEWADJ,
				     NULL, TRUE);
		    if (!adj->L2only) {	/* L1L2 */
			isis_mark_L1_dr_changed(pdb, isisidb);
			isis_mark_L1_changed(pdb);
			isis_trigger_spf(pdb, CLNS_LEVEL1,
					 ISIS_SPF_TRIG_NEWADJ, NULL, TRUE);
		    }
		}
		adj->state = CLNS_ADJ_UP_STATE;
		isis_update_adj_count(pdb, adj, TRUE); 
		adj_pbuginf(pdb, "Adjacency state goes to Up");
		isis_log_adjacency_change(pdb, adj, TRUE, "new adjacency");
		GET_TIMESTAMP(adj->create_time); /* Show that it cycled. */

		/* %%% Remove the possible ES adj in L1 pseudo lsp, if dr */
	    }

	} else if (adj->state != CLNS_ADJ_INIT_STATE) {

	    if (adj->state == CLNS_ADJ_UP_STATE) {
	        isis_update_adj_count(pdb, adj, FALSE);
		isis_log_adjacency_change(pdb, adj, FALSE,
					  "neighbor forgot us");
	    }
	    adj->state = CLNS_ADJ_INIT_STATE;
	    adj_pbuginf(pdb, "Adjacency state goes to Init");

	    if (adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE || !adj->L2only) {
		isis_mark_L1_changed(pdb);
		adj->L1_lan_priority = ISIS_MAX_PRIO + 1;
		isis_mark_L1_dr_changed(pdb, isisidb);
	    }

	    if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) {
		isis_mark_L2_changed(pdb);		
		adj->L1_lan_priority = ISIS_MAX_PRIO + 1;
		adj->L2_lan_priority = ISIS_MAX_PRIO + 1;
		isis_mark_L1_dr_changed(pdb, isisidb);
		isis_mark_L2_dr_changed(pdb, isisidb);
	    }
	}
	return;

    } else {

	/* 
	 * There wasn't an adjacency yet.
	 */
	hold_time = GETSHORT(isis->ht);
	adj = add_adjacency(pdb->adjacency_db, key, idb, source_snpa,
			    hold_time, FALSE);
	if (!adj)
	    return;
	adj->state = CLNS_ADJ_INIT_STATE;
    }

 new_adj:


    isis_schedule_hello(pdb, isisidb, level);
    if (adj->state == CLNS_ADJ_UP_STATE) {
        isis_update_adj_count(pdb, adj, FALSE);
	isis_log_adjacency_change(pdb, adj, FALSE, "adjacency type changed");
    }

    /*
     * New adjacency - set adjacency type, priority, state, and get area 
     * addresses.
     */
    if (level == ISIS_CIRCUIT_L1) {
	adj->L1_lan_priority = ISIS_MAX_PRIO + 1;
	if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) {
	    adj->L2only = FALSE;
	} else {
	    if (adj->adjacencyType == CLNS_IS_ADJ_TYPE)
		adj->L2only = FALSE;
	    adj->adjacencyType = CLNS_L1_IS_ADJ_TYPE;
	}
    } else {
	adj->L2_lan_priority = ISIS_MAX_PRIO + 1;
	adj->L2only = (!adj->adjacencyType || 
		       adj->adjacencyType == CLNS_IS_ADJ_TYPE ||
		       adj->adjacencyType ==  CLNS_L2_IS_ADJ_TYPE);
	adj->adjacencyType = CLNS_L2_IS_ADJ_TYPE;
    }

    adj->protocol_source |= pdb->typeflag;
    TIMER_START(adj->expiration_time, GETSHORT(isis->ht) * ONESEC);
    isis_start_adj_timer(pdb, adj);
    station_copy(isis->source_id, &adj->key[1]);
    adj->key[0] = STATION_LENGTH;
    systemid_copy(isis->lan_id, (level == ISIS_CIRCUIT_L1) ?
		  adj->L1_circuit_id : adj->L2_circuit_id);

    /*
     * Fill in area addresses and IP and IPX info.
     */
    isis_copy_hello_tlvs(adj, pdb, idb, tlv_parse);
    adj->ipx_version = isis->version_pid;
    adj->ip_nexthop = nexthop_ipaddr;

    snpa_copy(source_snpa, &adj->snpa);
    adj->state = CLNS_ADJ_INIT_STATE;
    adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;

    adj_pbuginf(pdb, "New adjacency, level %s for %s",
		(adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE) ? "1" : 
		(adj->L2only) ? "2" : "1 & 2", snpa_string(source_snpa));
}

/*
 * isis_find_us
 *
 * Search in TLV for this system's address. The hardware address is
 * used for LAN media. The network layer system-id is used for WAN
 * media used as a multi-access network (X.25, SMDS, and Frame Relay).
 */
static boolean isis_find_us (clns_pdbtype *pdb, isis_lan_IIH_hdrtype *isis,
			     uchar *end, idbtype *idb)
{

    uchar *address;
    uchar *curptr;
    uchar *tlv_end;
    uchar addr[IEEEBYTES];
    isis_info_type *item;

    item = (isis_info_type *)isis->item;

    /*
     * Get media address for this interface.
     */
    address = isis_adj_addr(pdb, idb, idb->hwptr->hardware, pdb->systemId,
			    addr);

    /*
     * Go through each address in the hello packet.
     */
    while ((uchar *) item < end) {
	if (item->code == ISIS_IS_NEIGHBORS_IIH) {
	    curptr = item->value;
	    tlv_end = curptr + item->length;
	    while (curptr < tlv_end) {
		if (!bcmp(address, curptr, STATION_LENGTH)) return (TRUE);
		curptr += STATION_LENGTH;
	    }
	}
	ADVANCE_ITEM(item, TRUE);
    }
    return (FALSE);
}

/*
 * isis_rcv_lan_iih
 *
 * Process received LAN hello packet.
 */
void isis_rcv_lan_iih (paktype *pak, clns_pdbtype *pdb, idbtype *idb,
		       isishdrtype *isis_hdr, int level)
{
    isis_lan_IIH_hdrtype *isis;
    isis_info_type *item;
    char           *packet_end;
    hwaddrtype     source_snpa;
    isis_TLV_parse *tlv_parse;
    isisidbtype    *isisidb;
    char	   *packet_type;
    boolean	   found_us;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    isis = (isis_lan_IIH_hdrtype *) isis_hdr;

    (*idb->hwptr->extract_snpa)(pak, &source_snpa, SOURCE);

    adj_pbuginf(pdb, "Rec L%d IIH from %s (%s), cir type %d, cir id %s",
		level, snpa_string(&source_snpa), idb->namestring,
		isis->circuit_type, systemid_string(isis->lan_id));

    /*
     * If IIH received from this system, ignore.
     */
    if (station_match(pdb->systemId, isis->source_id)) {
	if (isis->type == ISIS_LAN_L1_HELLO) {
	    packet_type = "LAN L1 hello";
	} else {
	    packet_type = "LAN L2 hello";
	}
	errmsg(&msgsym(BADPACKET, CLNS), pdb->errmsg_name, packet_type,
	       "Duplicate system ID detected", "", snpa_string(&source_snpa),
	       pak->if_input->namestring);
	adj_pbuginf(pdb, "Duplicate system id");
	return;
    }

    if (!(isisidb->isis_circuit_type & level)) {
	adj_pbuginf(pdb, "Interface level mismatch, %s", 
		    idb->namestring);
	return;
    } else if (!(pdb->level & level)) {
	adj_pbuginf(pdb, "is-type mismatch");
	return;
    }

    /* Validate packet. */

    tlv_parse = isis_valid_packet(pak, isis, pdb);
    if (tlv_parse == NULL)
	return;

    item = (isis_info_type *)isis->item;
    packet_end = ((char *) isis) + GETSHORT(isis->pdu_length);

    /* Note whether we're present in the sender's neighbor list(s). */

    found_us = isis_find_us(pdb, isis, packet_end, idb);

    /* Split off for NLSP versus ISIS. */

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis_process_nlsp_lan_iih(pak, pdb, idb, isis, item, level, found_us,
				  &source_snpa, packet_end, tlv_parse);
    } else {
	isis_process_isis_lan_iih(pak, pdb, idb, isis, item, level, found_us,
				  &source_snpa, packet_end, tlv_parse);
    }
    free(tlv_parse);
}
