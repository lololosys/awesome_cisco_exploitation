/* $Id: tarp2.c,v 3.4.56.3 1996/05/17 21:03:28 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp2.c,v $
 *------------------------------------------------------------------
 * TARP (Target ID Address Resolution Protocol)
 *
 * October, 1995 Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp2.c,v $
 * Revision 3.4.56.3  1996/05/17  21:03:28  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.4.56.2  1996/05/17  20:55:40  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 (decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.4.56.1  1996/03/18  22:11:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/07  10:51:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  18:51:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/20  22:18:33  snyder
 * CSCdi44202:  spellink errors
 *              unkown -> unknown
 *              adjaacency -> adjacency
 *
 * Revision 3.3  1995/11/20  19:19:19  asastry
 * CSCdi43154:  TARP corrupted memory pointer. Fix pdb corruption bug. In
 * addition, incorporate these other misc. fixes:
 *
 *
 *         - Fix spurious TARP interface command during NVGEN
 *         - Reset sequence number to zero before sending TARP Type 4 PDU
 *         - Accept TARP PDU with sequence number of zero, regardless of
 *           whether there is an entry in Loop Detection Buffer
 *         - Zero out NSAP field in TARP TID cache if CLNS NET is deleted
 *           from underneath TARP
 *         - create TID entry with a zero NSAP if a TARP TID is entered
 *           before assigning a CLNS NET
 *
 * Revision 3.2  1995/11/17  18:44:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "tarp_externs.h"
#include "tarp_debug.h"

uchar addr_buffer[TARP_MAX_TID_LEN];


/*
 * tarp_build_clns_packet
 *
 * Build a CLNP data PDU (containing TARP info). Returns a pointer to the
 * new packet if successful, NULL otherwise. Packet is absorbed by a
 * call to 'clns_write'.The args 'source_prot_info' and 'dest_prot_info'
 * are NSAP's that will be used in the CLNP header. The argument
 * 'tarp_source_prot_info', if non-NULL, will be the source protocol
 * information that will be inserted in the TARP data area as the source
 * protocol address. If it's NULL, we insert 'source_prot_info' into this
 * field.
 */
static paktype *tarp_build_clns_packet (tarp_pdutype *tarp_info,
				 uchar target_tid_len, uchar *target_tid,
				 uchar source_tid_len, uchar *source_tid,
				 uchar *source_prot_info,
				 uchar *dest_prot_info,
				 uchar *tarp_source_prot_info)
{
    paktype *pak;
    clnshdrtype *clns;
    clns_segpart_t *segptr;
    clns_pdbtype *clns_pdb;
    uchar *clns_net, *ptr;
    tarp_pdutype *tarp;
    ushort num_bytes;
    enum TARP_HOST_TYPE tarp_host_type;

    tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net);

    /*
     * Check if ISIS running. If it's not, check if we are an ES. If we
     * aren't an ES either, return.
     */
    if (tarp_host_type == TARP_NULL_HOST_TYPE) { 
	TARP_EVBUGINF("\nTARP-EV: TARP PDU not originated "
		      "(ISIS or ESIS not running)");
	return(NULL);
    }

    pak = clns_getbuffer(MEDDATA);
    if (!pak) {
	TARP_EVBUGINF("\nTARP-EV: Unable to allocate buffer "
		      "(tarp packet origination)");
	return(NULL);
    }

    clns = (clnshdrtype *)clnsheadstart(pak);

    clns->pid = NLP_ISO8473;
    clns->version = CLNS_VERSION;
    clns->sp = 1;
    clns->ms = clns->er = 0;
    clns->lifetime = clns_packet_lifetime;
    clns->type = CLNS_TYPE_DATA;

    /*
     * Point to address part.
     */
    ptr = PAK2ADDR(pak);

    /*
     * Fill in destination, then source. Also set the NSEL to TARP's
     * N-selector.
     */
    pak->clns_dstaddr = (uchar *)ptr;
    nsap_copy(dest_prot_info, ptr);
    ptr[ptr[0]] = tarp_global_nsel;
    ptr += ptr[0] + 1;

    pak->clns_srcaddr = (uchar *)ptr;
    nsap_copy(source_prot_info, ptr);
    ptr[ptr[0]] = tarp_global_nsel;
    ptr += ptr[0] + 1;

    /*
     * segmentation part
     */
    pak->clns_segpart = ptr;
    segptr = (clns_segpart_t*) ptr;
    PUTSHORT(segptr->duid, 0);
    segptr->segoffset[0] = 0;	/* this is the first segment */
    segptr->segoffset[1] = 0;
    ptr = ((uchar *) segptr) + sizeof(clns_segpart_t);

    *ptr++ = CLNS_OPT_QOS;
    *ptr++ = CLNS_OPT_GQOS_LEN;
    *ptr++ = CLNS_QOS_GLOBAL;

    /*
     * Header done -- fill in the header length 
     */
    clns->hlen = ptr - (uchar *)clnsheadstart(pak);

    /*
     * Data part (TARP info)
     */
    tarp = (tarp_pdutype *)ptr;
    PUTSHORT(&tarp->lifetime, tarp_info->lifetime);
    PUTSHORT(&tarp->sequence, tarp_info->sequence);
    tarp->prot_addr_type = tarp_info->prot_addr_type;
    tarp->tarp_urc = TARP_GET_URC(tarp_info);
    tarp->tarp_code = TARP_GET_CODE(tarp_info);
    tarp->target_len = target_tid_len;
    tarp->source_len = source_tid_len;

    /*
     * If we have a value to be inserted into the TARP protocol info, use
     * that instead of the CLNS source protocol address.
     */
    if (tarp_source_prot_info)
	tarp->prot_addr_len = tarp_source_prot_info[0];
    else
	tarp->prot_addr_len = pak->clns_srcaddr[0];

    ptr += TARP_PDU_HEADER_SIZE;

    /*
     * 'ptr' now points to the target TID field. Copy target TID
     * value, if applicable.
     */
    if (tarp->target_len) {
	bcopy(target_tid, ptr, tarp->target_len);
	ptr += tarp->target_len;
    }

    /*
     * 'ptr' now points to the source TID field
     */
    bcopy(source_tid, ptr, source_tid_len);
    ptr += source_tid_len;

    /*
     * 'ptr' now points to the source protocol address (NSAP) field. Note that
     * we don't store the NSAP length in the first byte (since this is already
     * stored in the field 'prot_addr_len'). The value stored here will be
     * 'tarp_source_prot_info' if it's non-NULL, or the CLNP source address.
     */
    if (tarp_source_prot_info) {
	bcopy(tarp_source_prot_info + 1, ptr, tarp_source_prot_info[0]);
	ptr += tarp_source_prot_info[0];
    }
    else {
	bcopy(source_prot_info + 1, ptr, pak->clns_srcaddr[0]);
	ptr += pak->clns_srcaddr[0];
    }

    num_bytes = ptr - (uchar *)clnsheadstart(pak);

    /*
     * Now fill in segment length value in header and segmentation
     * parts of packet.
     */
    PUTSHORT(segptr->length, num_bytes); 
    PUTSHORT(clns->clns_seglen, num_bytes);

    return (pak);
}

/*
 * tarp_originate_packet
 *
 * Originate a TARP PDU. 
 *
 * The value in 'pro_addr_type' indicates what kind of a protocol address it is
 * and currently can only be a CLNP NSAP. The spec keeps it general enough
 * to accommodate other kinds of addresses, so we need to carry the address
 * type info. 'dest_tid' is the TID of the destination, if applicable.
 * Similarly, 'dest_prot_addr' is the destination NSAP. If it's NULL,
 * it means we're originating a PDU that will be sent to all neighbors.
 */
void tarp_originate_packet (uchar pdu_type, uchar prot_addr_type,
				   uchar *source_tid, uchar *dest_tid,
				   uchar *source_prot_addr,
				   uchar *dest_prot_addr)
{
    paktype *pak;
    tarp_pdutype *tarp_out_pdu;
    ushort source_prot_addr_len, dest_tid_len, pdu_data_size;
    uchar level, *system_id;
    tarp_ldb_type *ldb;

    /*
     * Don't bother if the 'originate' flag is FALSE.
     */
    if (!tarp_originate_flag) {
	TARP_EVBUGINF("\nTARP-EV: Cannot originate TARP packet "
		      "- 'originate' flag is OFF");
	return;
    }

    /*
     * Only NSAP's supported right now. If we need to support other
     * protocols, we need to add another routine to build packets (like
     * what 'tarp_build_clns_packet' does).
     */
    if (!TARP_SUPPORTED_PROTOCOL(prot_addr_type)) {
	TARP_EVBUGINF("\nTARP-EV: Unable to originate TARP packet "
		      " - unknown protocol type %d", prot_addr_type);
	return;
    }

    if (!source_tid || !source_prot_addr)
	return;

    if (prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE) 
        source_prot_addr_len = source_prot_addr[0];
    else
	source_prot_addr_len = strlen(source_prot_addr);

    /*
     * dest_prot_addr will be NULL if we're originating a packet that
     * needs to be propagated to our neighbors (this will be the case
     * for type 1, 2 and 4 PDU's .
     */

    dest_tid_len = (dest_tid) ? strlen(dest_tid) : 0;
    pdu_data_size = strlen(source_tid) + dest_tid_len + source_prot_addr_len;

    tarp_out_pdu = malloc(TARP_PDU_HEADER_SIZE + pdu_data_size);
    if (!tarp_out_pdu) {
	tarp_traffic[TARP_NOMEM]++;
	return;
    }

    /*
     * Figure out which level we want to do propagation on. We'll
     * propagate only types 1,2 and 4 PDU's.
     */
    switch (pdu_type) {
        case TARP_RQST_TID_L1_PDU:
	    level = CLNS_L1_IS_ADJ_TYPE;
	    break;
	case TARP_RQST_TID_L2_PDU: 
	case TARP_NOTIFY_PDU:
	    level = CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE;
	    break;
	case TARP_RESPONSE_PDU:    
	case TARP_RQST_PROT_PDU:
	default:
	    level = 0;
    }

    /*
     * Fill in fixed part of tarp pdu info that will be sent out in the
     * new PDU. Note that we reset the sequence number if we are
     * sending out a TARP Type 4 PDU (TID or NET has been reset).
     */
    tarp_out_pdu->lifetime = tarp_lifetime;

    if (pdu_type == TARP_NOTIFY_PDU)
	tarp_global_sequence_num = 0;

    tarp_out_pdu->sequence = tarp_global_sequence_num++;
    tarp_out_pdu->prot_addr_type = prot_addr_type;
    tarp_out_pdu->tarp_urc = tarp_global_urc;
    tarp_out_pdu->tarp_code = pdu_type;
    tarp_out_pdu->target_len = dest_tid_len;
    tarp_out_pdu->source_len = strlen(source_tid);
    tarp_out_pdu->prot_addr_len = source_prot_addr_len;	    

    if (dest_tid)
	bcopy(dest_tid, TARGET_TID(tarp_out_pdu), dest_tid_len);

    bcopy(source_tid, SOURCE_TID(tarp_out_pdu), strlen(source_tid));

    /*
     * If address is an NSAP, fill in address field of PDU with the 
     * NSAP (and zero out the NSEL), and extract the system ID.
     */
    if (prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE)  {
        bcopy(&source_prot_addr[1], SOURCE_PROT_ADDR(tarp_out_pdu),
	      source_prot_addr_len);
	system_id = get_station(source_prot_addr);
    }
    else {
        bcopy(source_prot_addr, SOURCE_PROT_ADDR(tarp_out_pdu),
	      source_prot_addr_len);
	system_id = source_prot_addr;
    }

    /*
     * Create (or update) LDB entry for this station.
     */
    ldb = tarp_find_ldb_entry(system_id);
    if (ldb)
	ldb->sequence = tarp_out_pdu->sequence;
    else
	tarp_insert_ldb_entry(tarp_out_pdu->sequence, system_id);

    /*
     * If destination protocol address was specified, we need to 'unicast'
     * the packet. Else it is a propagation.
     */
    if (dest_prot_addr) {
	pak = tarp_build_clns_packet(tarp_out_pdu, 
				     dest_tid_len, dest_tid,
				     strlen(source_tid), source_tid,
				     source_prot_addr,
				     dest_prot_addr, NULL);
	if (pak) {
	    if (!clns_write(pak)){
		TARP_EVBUGINF("\nTARP-EV: Unable to send CLNS packet");
	    }		      
	    else {
		TARP_PABUGINF("\nTARP-PA: Originated TARP packet, type %d, to "
			      "\n\tdestination %n", pdu_type,
			      dest_prot_addr);
		if (dest_tid) 
		    TARP_PABUGINF(", TID %s", dest_tid);
		tarp_traffic[TARP_OUTPUT]++;
	    }
	}
	else {
	    TARP_EVBUGINF("\nTARP-EV: Unable to originate TARP packet "
			  "- type %d", pdu_type);
	}
    }
    else {
	tarp_propagate(tarp_out_pdu, level, TRUE, NULL);
    }

    /*
     * All done, free the PDU that we created.
     */
    free(tarp_out_pdu);

    return;
}

/*
 * tarp_handle_type1_2_pdu
 *
 * Handle receipt of a TARP type 1 or type 2 PDU. Basically entails
 * originating a Type 3 TARP PDU as a response. The information that will
 * be contained in the new PDU will come from 'cache_info'.
 */
void tarp_handle_type1_2_pdu (tarp_pdutype *tarp_pdu,
			      tarp_tid_cache_type *cache_info)
{
    /*
     * Get protocol address of sender.
     */
    tarp_build_address_string(tarp_pdu, addr_buffer);

    /*
     * If 'update remote cache' bit is zero, update the cache entry
     * we have for the sender.
     */
    if (!TARP_GET_URC(tarp_pdu))
	tarp_update_tid_entry(SOURCE_TID(tarp_pdu), tarp_pdu->source_len,
			      tarp_pdu->prot_addr_type,
			      SOURCE_PROT_ADDR(tarp_pdu),
			      tarp_pdu->prot_addr_len, TRUE, 0);

    /*
     * Originate a TARP TYPE 3 response PDU. Note that 'addr_buffer' will
     * contain the destination address (NSAP).
     */
    tarp_originate_packet(TARP_RESPONSE_PDU, tarp_pdu->prot_addr_type,
			  cache_info->tid, NULL, 
			  cache_info->net, addr_buffer);
    
    return;
}

/*
 * tarp_handle_type3_pdu
 *
 * A type 3 TARP PDU is basically a response to a type 1,2 or 5 TARP PDU.
 * Upon receipt, we check to see if we have an outstanding request for this
 * response. If we don't we treat the response as a duplicate, and ignore 
 * it. Otherwise, we set the status of the request entry in the queue to
 * SUCCESS, and update the info in our cache. Setting the status to SUCCESS
 * will awaken us from our trance and make us print the received information.
 */
void tarp_handle_type3_pdu (tarp_pdutype *tarp_pdu)
{
    tarp_rqstQ_type *entry;
    NSAP_T nsap_buffer;

    /*
     * Retrieve sender's TID and NSAP address.
     */
    tarp_build_address_string(tarp_pdu, nsap_buffer);
    tarp_build_tid_string(tarp_pdu, addr_buffer, TARP_SOURCE);

    /*
     * See if we have an entry for the sender in the queue.
     * If we do not, it means this is a duplicate response, and so we don't
     * need to process this PDU. If we find it, we set the status field
     * in this entry to TARP_RESP_SUCCESS and update our cache entry.
     */

    for (entry = (tarp_rqstQ_type *)tarp_rqstQ.qhead; entry;
	 entry = entry->next) {

	/*
	 * If we'd queued up a type 1 or 2 PDU request, search based on
	 * TID.
	 */
	if ((entry->type == TARP_RQST_TID_L1_PDU) ||
	    (entry->type == TARP_RQST_TID_L2_PDU)) {
	    if (strcmp(entry->tid_or_net, addr_buffer) == 0) {
	        break;
	    }
	}
	/*
	 * If we'd queued up a type 5 PDU request, search based on NET.
	 */
	else if (entry->type == TARP_RQST_PROT_PDU)
	    if (net_equ(entry->tid_or_net, nsap_buffer)) {
		break;
	    }
	/*
	 * Shouldn't happen - but just in case...
	 */
	else
	    continue;
    }
    
    if (entry) {
	/*
	 * Found an entry - update our cache entry, and set the flag in
	 * the entry to SUCCESS.
	 */
	tarp_update_tid_entry(SOURCE_TID(tarp_pdu), tarp_pdu->source_len,
			      tarp_pdu->prot_addr_type,
			      SOURCE_PROT_ADDR(tarp_pdu),
			      tarp_pdu->prot_addr_len, TRUE, 0);
	entry->status = TARP_RESP_SUCCESS;
	return;
    }
    else {
	/*
	 * We didn't find an entry - discard this info.
	 */
	TARP_EVBUGINF("\nTARP-EV: Duplicate response from %s discarded",
		       addr_buffer);
	}

    return;
}

/*
 * tarp_handle_type4_pdu
 *
 * Update our cache entry with the sender's info, if we have an entry for the
 * target TID. Also propagate the info if we are an IS (i.e. the flag
 * 'we_are_es' is FALSE). 'sender_adj' is the adjacency of the sender of
 * this PDU, and tells us that we should not propagate this PDU back to
 * this sender.
 */
void tarp_handle_type4_pdu (tarp_pdutype *tarp_pdu, clns_adjtype *sender_adj,
			    boolean we_are_es)
{
    /*
     * Extract target TID.
     */
    tarp_build_tid_string(tarp_pdu, addr_buffer, TARP_DEST);

    /*
     * If we have a cache entry for the target TID, delete the entry
     * and add a new one containing the new information.
     */
    if (tarp_find_tid_entry(TARGET_TID(tarp_pdu), tarp_pdu->target_len,
			    TARP_TID_KEY)) {
	tarp_delete_one_tid_entry(addr_buffer);
	tarp_update_tid_entry(SOURCE_TID(tarp_pdu), tarp_pdu->source_len,
			      tarp_pdu->prot_addr_type,
			      SOURCE_PROT_ADDR(tarp_pdu),
			      tarp_pdu->prot_addr_len, TRUE, 0);
    }

    /*
     * Propagate to all our neighbors if we're not an ES.
     */
    if (!we_are_es)
	tarp_propagate(tarp_pdu, CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE,
		       FALSE, sender_adj);

    return;
}

/*
 * tarp_handle_type5_pdu
 *
 * A type 5 PDU is sent when the requestor wants a TID that corresponds an
 * NET. If we get this PDU, we construct a TYPE 3 TARP PDU (with our
 * information) and send it off.
 */
void tarp_handle_type5_pdu (tarp_pdutype *tarp_pdu)
{
    tarp_tid_cache_type *my_cache_entry;

    /*
     * Look up our entry in our TID cache.
     */
    my_cache_entry = tarp_find_tid_entry(tarp_my_tid, strlen(tarp_my_tid),
					 TARP_TID_KEY);
    if (my_cache_entry) {
	/*
	 * Get sender's protocol (NSAP) address.
	 */
	tarp_build_address_string(tarp_pdu, addr_buffer);
	tarp_originate_packet(TARP_RESPONSE_PDU,
			      my_cache_entry->prot_addr_type,
			      my_cache_entry->tid, NULL, 
			      my_cache_entry->net, addr_buffer);
    }
    else {
	TARP_EVBUGINF("\nTARP-EV: Unable to process Type 5 TARP PDU - no "
		      "\n\t cache entry found for our TID");
    }

    return;
}

/*
 * tarp_send_packet
 *
 * Send the TARP packet to the given destination. The routine builds a CLNP
 * packet, including the TARP part. Since the TARP header info in the packet
 * that we're going to propagate might be different from that in the packet
 * we received, we get passed in the new header info in 'new_tarp_header_info'.
 * The original TARP data part is contained in 'old_tarp_info', and we just
 * copy this into the new CLNP packet. The argument 'originator_net' will
 * be NULL if this packet was locally sourced. If the packet came from someone
 * else, this argument will contain the NET of the packet's originator. 
 * The argument 'idb' is the outgoing interface, and will be NULL if we're
 * sending this packet to a static TARP adjacency.
 */
static void tarp_send_packet (tarp_pdutype *new_tarp_header_info,
			      tarp_pdutype *old_tarp_info, uchar *source_net,
			      uchar *dest_net, uchar *originator_net,
			      idbtype *idb)
{
    paktype *pak;
    idbtype *out_idb = NULL;
    clns_adjtype *adj;

    /*
     * If we don't have an outgoing interface for this packet, find it
     * based on destination NET. If we aren't successful, return. If we
     * find one, but it's to our address, just return.
     */
    if (!idb) {
	adj = clns_route(dest_net, ROUTINGDEPTH);
	if (!adj) {
	    TARP_EVBUGINF("\nTARP-EV: No route found for TARP static "
			  "adjacency\n\t %n - packet not sent", dest_net);
	    return;
	}
	else if ((int) adj == -1) {
	    TARP_EVBUGINF("\nTARP-EV: TARP static adjacency pointing to us "
			  " - packet not sent");
	    return;
	}
	
	out_idb = adj->idb;
    }

    pak = tarp_build_clns_packet(new_tarp_header_info,
				 old_tarp_info->target_len,
				 TARGET_TID(old_tarp_info),
				 old_tarp_info->source_len,
			         SOURCE_TID(old_tarp_info),
				 source_net, dest_net, originator_net);
    if (!pak) {
	TARP_EVBUGINF("\nTARP-EV: Unable to build TARP packet (type %d)"
	              " for propagation", TARP_GET_CODE(old_tarp_info));
	return;
    }

    /*
     * Call 'clns_write' to send the packet.
     */
    if (!clns_write(pak)) {
	if (idb)
	    TARP_EVBUGINF("\nTARP-EV: Unable to send CLNS packet to %n "
		      "(via %s)", dest_net, idb->namestring);
	else
	    TARP_EVBUGINF("\nTARP-EV: Unable to send CLNS packet to "
			  "TARP static adjacency %n", dest_net);
	return;
    }

    if (tarp_pkt_debug) {
	if (idb) {
	    buginf("\nTARP-PA: Propagated TARP packet, "
		   "type %d, out on %s", TARP_GET_CODE(old_tarp_info), 
		   idb->namestring);
	}
	else {
	    buginf("\nTARP-PA: Propagated TARP packet, "
		   "type %d, to TARP static adjacency "
		   "\n\t %n (via %s)", TARP_GET_CODE(old_tarp_info),
		   dest_net, out_idb->namestring);
	}
	
	buginf("\n\t Lft = %d, Seq = %d, Prot type = 0x%X, "
	       "URC = %s", new_tarp_header_info->lifetime, 
	       new_tarp_header_info->sequence,
	       new_tarp_header_info->prot_addr_type,
	       (!TARP_GET_URC(new_tarp_header_info)) ? "TRUE" : "FALSE");
	buginf("\n\t Dtid len = %d, Stid len = %d, "
      	       "Prot addr len = %d", old_tarp_info->target_len,
       	       old_tarp_info->source_len,
	       old_tarp_info->prot_addr_len);

	if (idb)
	    buginf("\n\t Destination NSAP : %n", dest_net);
	else
	    buginf("\n\t Destination NSAP : %n"
		   "\n\t (Static TARP adjacency)", dest_net);

	buginf("\n\t Originator's NSAP : %n",
	       originator_net ? originator_net : source_net);

	if (old_tarp_info->target_len) {
	    tarp_build_tid_string(old_tarp_info, addr_buffer, TARP_DEST);
	    buginf("\n\t Target %sTID : %s",
		   (TARP_GET_CODE(old_tarp_info) == TARP_NOTIFY_PDU) ?
		   "(old) " : "",
		   addr_buffer);
	}

	tarp_build_tid_string(old_tarp_info, addr_buffer, TARP_SOURCE);
	buginf("\n\t Originator's %sTID : %s",
	       (TARP_GET_CODE(old_tarp_info) == TARP_NOTIFY_PDU) ? 
	       "(new) " : "",
	       addr_buffer);
    }

    tarp_traffic[TARP_OUTPUT]++;

    return;
}

/*
 * tarp_propagate
 *
 * Propagate information in PDU that we received. Can be L1, L2, or L1L2.
 * L1 indicates propagation to all L1 ISIS adjacencies, L2 is for all L2 ISIS
 * adjacencies, and L1L2 is for all ISIS adjacencies.
 *
 * If this is an end-system, we propagate a packet only if it was originated
 * locally.
 *
 * Information from a source is never propagated back to the source. If the
 * propagate packet originated here (e.g. propagating a change notification in
 * a type 4 PDU), the flag 'from_us' will be true.
 */
void tarp_propagate (tarp_pdutype *tarp_pdu, uchar level, boolean from_us,
		     clns_adjtype *sender_adj)
{
    clns_adjtype *adj;
    clns_pdbtype *clns_pdb;
    uchar *clns_net;
    int i, j, bucket;
    tarp_pdutype tarp_out_pdu;
    NSAP_T adj_nsap;
    boolean adj_is_L1L2, es_adj_L1, adj_ok, es_adj_ok;
    tarp_static_adjQ_type *tarp_static_entry;
    enum TARP_HOST_TYPE tarp_host_type;
    uchar temp_nsap_buffer[CLNS_MAX_NSAP_LENGTH];
    uchar area1[CLNS_MAX_NSAP_LENGTH], area2[CLNS_MAX_NSAP_LENGTH];

    /*
     * Don't bother if the 'propagate' flag is FALSE.
     */
    if (!tarp_propagate_flag && tarp_event_debug) {
	tarp_build_tid_string(tarp_pdu, addr_buffer, TARP_SOURCE);
	buginf("\nTARP-EV: Cannot propagate TARP packet received from "
	       "%s \n\t ('propagate' flag is set to OFF)", addr_buffer);
	return;
    }

    /*
     * Check if ISIS running. If it's not, check if we're running as an ES.
     * Note that an ES will only propagate TARP PDU's that are originated
     * locally. So if the flag 'from_us' is FALSE and we're an ES, just return.
     */
    tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net); 
    if (tarp_host_type != TARP_IS_TYPE) {
	if ((tarp_host_type == TARP_ES_TYPE) && from_us) {
	    if (!clns_net) {
		TARP_EVBUGINF("\nTARP-EV: TARP PDU not propagated "
			      "(no NET configured on this box)");
		return;
	    }
	}
	else
	    return;
    }

    /*
     * Fill in fixed part of tarp pdu info that will be sent out in the
     * new PDU. When propagating, we use a packet lifetime of 1 less than the
     * value we received (however, if the packet was originated locally, we use
     * the lifetime value as is). The sequence number is propagated unchanged.
     */
    if (from_us)
	tarp_out_pdu.lifetime = tarp_pdu->lifetime;
    else
	tarp_out_pdu.lifetime = tarp_pdu->lifetime - 1;

    /*
     * If the new lifetime value of the TARP PDU is zero, we might at well
     * not propagate the packet, since it will be discarded by all the
     * receivers anyway - this is an optimization as recommended in the 
     * amended TARP spec.
     */
    if (tarp_out_pdu.lifetime == 0) {
	TARP_EVBUGINF("\nTARP-EV: TARP PDU not propagated "
		      "(decremented lifetime value is zero)");
	return;
    }

    tarp_out_pdu.sequence = tarp_pdu->sequence;
    tarp_out_pdu.prot_addr_type = tarp_pdu->prot_addr_type;
    tarp_out_pdu.tarp_urc = TARP_GET_URC(tarp_pdu);
    tarp_out_pdu.tarp_code = TARP_GET_CODE(tarp_pdu);

    /*
     * Get source protocol address in the TARP PDU. We use this to fill
     * in the 'source_prot_addr' field in the case where we are
     * propagating a TARP PDU that we received from someone else. If
     * the PDU was originated locally, we use the same value as the
     * CLNP source address.
     */
    tarp_build_address_string(tarp_pdu, temp_nsap_buffer);

    for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
        for (adj = clns_adjacency_db[bucket]; adj; adj = adj->next) {
	    /*
	     * Don't propagate packet to the original sender of this info.
	     * If we're originating this packet and we're sending it
	     * to all our neighbors, 'sender_adj' will be NULL, so this
	     * check will still work correctly.
	     */
	    if (adj == sender_adj)
		continue;

	    if (adj->idb == NULL) {
		TARP_EVBUGINF("\nNull IDB for adj %n - skip it", adj->key);
		continue;
	    }

	    /*
	     * Don't bother if adjacency is not up. However, if it is a
	     * Type 4 PDU, we will relax this condition since an adjacency
	     * might have gone to 'Init' if we changed our NET.
	     */
	    if ((adj->state != CLNS_ADJ_UP_STATE) &&
		(TARP_GET_CODE(&tarp_out_pdu) != TARP_NOTIFY_PDU)) {
		TARP_EVBUGINF("\nTARP-EV: Packet not propagated to %n "
                              "on interface "
			      "\n\t %s (adjacency is not in UP state)",
			      adj->key, adj->idb->namestring);
		continue;
	    }

	    /*
	     * Don't bother if adjacency's outgoing interface is not up.
	     */
	    if (!interface_up(adj->idb)) {
		TARP_EVBUGINF("\nTARP-EV: Packet not propagated to %n "
                              "on interface "
			      "\n\t %s (outgoing interface is not UP)",
			      adj->key, adj->idb->namestring);
		continue;
	    }

	    /*
	     * Don't propagate if adj's outgoing interface has TARP 
	     * disabled.
	     */
	    if (!adj->idb->tarp_enabled) {
		TARP_EVBUGINF("\nTARP-EV: Packet not propagated to %n "
			      "on interface "
			      "\n\t %s (TARP is disabled on this interface)",
			      adj->key, adj->idb->namestring);
		continue;
	    }

	    /*
	     * Don't propagate if adj's outgoing interface has TARP propagation
	     * disabled.
	     */
	    if (!adj->idb->tarp_propagate) {
		TARP_EVBUGINF("\nTARP-EV: Packet not propagated to %n "
                              "on interface "
			      "\n\t %s (TARP propagation disabled on this "
			      "interface)",
			      adj->key, adj->idb->namestring);
		continue;
	    }
	    
	    /*
	     * If we're running as an ES, we will maintain only L1
	     * adjacencies. If we're sending Type 2 PDU's send them
	     * only to L1L2 adjacencies. 
	     * The flag 'adj_ok' will determine if we can send
	     * to this adjacency.
	     */
	    es_adj_L1 = es_adj_ok = adj_ok = FALSE;
	    if (tarp_host_type == TARP_ES_TYPE) {
		if (!clns_net)
		    return;

		adj_ok = ((adj->adjacencyType & level) ||
		          (adj->adjacencyType == CLNS_ES_ADJ_TYPE));
			  
	    }
	    else {
		/*
		 * We're running as an IS...
		 *
		 * Propagate only to adjacencies of the right level, e.g. don't
		 * propagate to L1-only adjacencies if we're doing a L2
		 * propagation. If we have an ES adjacency, first determine
		 * whether it's L1 or L2, and then determine whether it's
		 * compatible with the propagation level. The flag 'es_adj_ok'
		 * will determine if we can send to this ES adjacency.
		 */
		adj_is_L1L2 = ((adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) &&
			       (!adj->L2only));
		adj_ok = (adj->adjacencyType & level) ||
		         ((level == CLNS_L1_IS_ADJ_TYPE) && adj_is_L1L2);

		if (adj->adjacencyType == CLNS_ES_ADJ_TYPE) {
		    for (i = 0; i < clns_pdb->num_areas; i++) {
			if (es_adj_L1)
			    break;

			for (j = 0; j < adj->num_areas; j++) {
			    if (nsap_match(clns_pdb->area_address[i],
					   adj->area_address[j])) {
				es_adj_L1 = TRUE;
				break;
			    }
			}
		    }
		    es_adj_ok =((es_adj_L1 && (level & CLNS_L1_IS_ADJ_TYPE)) ||
				(!es_adj_L1 && (level & CLNS_L2_IS_ADJ_TYPE)));
		}
	    }

	    if ((!adj_ok) && (!es_adj_ok))
		continue;

	    /*
	     * Build the full NSAP for this adjacency.
	     */
	    
	    nsap_copy(adj->area_address[0], adj_nsap);
	    bcopy(&adj->key[1], &adj_nsap[adj_nsap[0]+1], adj->key[0]);
	    adj_nsap[0] += adj->key[0];
	    adj_nsap[0] += SELECTOR_LENGTH;
	    adj_nsap[adj_nsap[0]] = 0;

	    /*
	     * Check if this adjacency is 'blacklisted' i.e. it should not
	     * receive this PDU.
	     */
	    
	    if (tarp_search_static_adjQ(adj_nsap, TARP_BLACKLIST)) {
		TARP_EVBUGINF("\nTARP-EV: Not propagating to %n on interface "
			      "%s \n\t (propagation disallowed for this "
                              "adjacency)",
			      adj_nsap, adj->idb->namestring);
		continue;
	    }

	    /*
	     * Adjacency is eligible to receive this TARP PDU, so send it.
	     *
	     * If packet has been locally originated, we use our CLNP
	     * source address, else we use the source address in the tarp_pdu
	     * (which has been extracted into 'temp_nsap_buffer').
	     */

	    tarp_send_packet(&tarp_out_pdu, tarp_pdu,
			     clns_net, adj_nsap, 
			     from_us ? NULL : temp_nsap_buffer, 
			     adj->idb);
	}
    }

    /* 
     * Done sending to all CLNS adjacencies. Now send to all TARP static
     * adjacencies (if any).
     */

    for (tarp_static_entry = (tarp_static_adjQ_type *)tarp_static_adjQ.qhead;
	 tarp_static_entry; tarp_static_entry = tarp_static_entry->next) {

	if (tarp_static_entry->type != TARP_STATIC_ADJ)
	    continue;

	/*
	 * Check if this static adjacency is 'blacklisted' i.e. it should not
	 * receive this PDU.
	 */
	    
	if (tarp_search_static_adjQ(tarp_static_entry->nsap, TARP_BLACKLIST)) {
	    TARP_EVBUGINF("\nTARP-EV: Not propagating to static adjacency %n "
			   "\n\t (propagation disallowed for this adjacency)",
			   tarp_static_entry->nsap);
	    continue;
	}

	/*
	 * Don't send Type 1 PDU's to Level-2 static adjacencies. We
	 * should send Type 2 PDU's to Level-1 static adjacencies
	 * because we cannot tell whether the static adjacency is L1L2
	 * or L1-only.
	 */
	tarp_extract_area(clns_net, area1);
	tarp_extract_area(tarp_static_entry->nsap, area2);
	if ((TARP_GET_CODE(&tarp_out_pdu) == TARP_RQST_TID_L1_PDU) && 
	    !nsap_equ(area1, area2)) {
	    TARP_EVBUGINF("\nTARP-EV: Not propagating Type 1 TARP PDU "
                          "to static adjacency \n\t%n "
			  "\n\t (adjacency is Level-2)",
			  tarp_static_entry->nsap);
	    continue;
	}

        /*
	 * If packet has been locally originated, we use our CLNP
	 * source address, else we use the source address in the tarp_pdu
	 * (which has been extracted into 'temp_nsap_buffer').
	 */
	tarp_send_packet(&tarp_out_pdu, tarp_pdu,
			 clns_net, tarp_static_entry->nsap,
			 from_us ? NULL : temp_nsap_buffer, NULL);
    }
    
    return;
}

/*
 * tarp_propagate_new_info
 *
 * Send a TARP type 4 PDU if some information about us changed (i.e. TID
 * or NET change). If 'old_tid' is NULL, it means our NET was changed.
 * in which case the target TID will contain the current TID. Otherwise, it
 * means our TID was changed, in which case the target TID will contain the
 * old TID.
 */
void tarp_propagate_new_info (uchar *old_tid, uchar *current_tid,
			      uchar *net)
{
    if (!current_tid || !net)
	return;

    /*
     * Send a type 4 PDU to all our neighbors. Note that the target TID for
     * a type 4 TARP PDU will be our old TID, if the TID was changed, or
     * the current TID if the TID was not changed (in which case it was
     * our NET that got changed).
     */
    tarp_originate_packet(TARP_NOTIFY_PDU, tarp_global_protocol_type,
			  current_tid, old_tid ? old_tid : current_tid,
			  net, NULL);

    return;
}
