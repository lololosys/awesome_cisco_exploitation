/* $Id: tarp_parse.c,v 3.8.18.5 1996/09/04 23:55:03 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp_parse.c,v $
 *------------------------------------------------------------------
 * TARP (Target ID Address Resolution Protocol) UI support
 *
 * October, 1995 Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_parse.c,v $
 * Revision 3.8.18.5  1996/09/04  23:55:03  snyder
 * CSCdi68120:  save a little memory here and there
 *              declare things that are const, const
 *              savings 300 out of data section
 *                       64 out of image
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/06/17  23:38:13  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.18.3  1996/05/17  21:03:32  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/05/17  20:55:43  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  22:11:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:51:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  18:52:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/08  20:13:30  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.7  1996/02/07  16:16:00  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/01  06:10:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/30  20:34:44  asastry
 * CSCdi47719:  Add help entry for TARP exec command. Also make other
 * trivial UI fixes.
 *
 * Revision 3.4  1996/01/29  07:30:16  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/20  19:19:21  asastry
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
 * Revision 3.2  1995/11/17  18:44:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "ieee.h"
#include "config.h"
#include "address.h"
#include "parser.h"
#include "../if/network.h"
#include "parser_defs_tarp.h"
#include "../ui/debug.h"
#include "tarp_debug.h"
#include "tarp_externs.h"
#include "../ui/common_strings.h"


/*
 * tarp_update_static_adjQ
 *
 * Add to / delete from the adj queue. This is a list of all adjacencies and
 * can be:
 *
 * TARP_BLACKLIST - NET of adjacency to whom we will not propagate TARP PDU's.
 * TARP_STATIC_MAP - statically configured NET<->TID mapping. The 'tid' for
 *                   such entries will be non-NULL.
 * TARP_STATIC_ADJ - statically configured TARP adjacencies. This will be
 *                   used to propagate TARP PDU's to a host that does not
 *                   have a CLNS adjacency to this host. 
 * 
 *
 * It's assumed that 'addr' contains an NSAP.
 */
void tarp_update_static_adjQ (uchar *addr, enum TARP_ADJ_ENTRY_TYPE type,
			      uchar *tid, boolean sense)
{
    tarp_static_adjQ_type *entry;
    uchar nsap_len;
    uchar *blank = "";
    static const char *const tarp_q_type[TARP_MAX_QUEUE] =
	   {"BLACKLIST", "MAP", "ADJ"};

    /*
     * Zero NSEL in addr before searching. This will also ensure that 
     * the table will contain entries with NSEL of zero.
     */
    nsap_len = addr[0];
    addr[nsap_len] = 0;

    entry = tarp_search_static_adjQ(addr, type);

    /*
     * Add new entry or return if it exists.
     */
    if (sense) {
	if (entry)
	    return;

        entry = malloc(sizeof(tarp_static_adjQ_type));
	if (!entry) 
	    return;

	/*
	 * Allocate space for TID if we need to store a TID.
	 */
	if (tid) {
	    entry->tid = malloc(strlen(tid) + 1);
	    if (!entry->tid) {
		free(entry);
		return;
	    }
	}

	enqueue(&tarp_static_adjQ, entry);
	
	/*
	 * Fill in entry fields. If this is a static map, also create an
	 * entry in the TID cache.
	 */
	nsap_copy(addr, entry->nsap);

	if (tid) {
	    bcopy(tid, entry->tid, strlen(tid) + 1);
	    if (type == TARP_STATIC_MAP) {
		tarp_update_tid_entry(entry->tid, strlen(entry->tid),
				      tarp_global_protocol_type,
				      &entry->nsap[1], entry->nsap[0],
				      TRUE, TARP_STATIC_FLAG);
	    }
	}
	else
	    entry->tid = NULL;

	entry->type = type;

	TARP_EVBUGINF("\nTARP-EV: Added %n, (type %s), %s%s to static "
			  "queue", addr, tarp_q_type[type],
		          tid ? "TID " : "",
		          tid ? tid : blank);
    }
    else {
	/*
	 * Delete entry. If entry does not exist, complain and return.
	 */
	if (!entry) {
	    TARP_EVBUGINF("\nTARP-EV: Could not remove %n, (type %s) from "
			  "static queue", addr,
		          tarp_q_type[type]);
	    return;
	}
	
	unqueue(&tarp_static_adjQ, entry);

	TARP_EVBUGINF("\nTARP-EV: Removed %n, (type %s) from static "
			  "queue", addr, tarp_q_type[type]);
	
	/*
	 * Delete the TID of entry, if it exists.
	 */
	if (entry->tid) {
	    /*
	     * If this is a static map, get rid of the entry in the TID cache
	     * as well.
	     */
	    if (type == TARP_STATIC_MAP) {
		tarp_delete_one_tid_entry(entry->tid);
	    }

	    free(entry->tid);
        }

	free(entry);
    }
    
    return;
}

/*
 * tarp_show_adjacencyQ
 *
 * Show all entries in the adj queue, based on type (TARP_BLACKLIST,
 * TARP_STATIC_MAP, TARP_STATIC_ADJ).
 */
static void tarp_show_adjacencyQ (enum TARP_ADJ_ENTRY_TYPE type)
{
    tarp_static_adjQ_type *entry;

    switch (type) {
        case TARP_BLACKLIST:
	    automore_enable("\n\tAdjacencies that we won't propagate TARP PDU's to:\n");
	    break;
	case TARP_STATIC_MAP:
	    automore_enable("\n\tStatic MAP entries:\n");
	    break;
        case TARP_STATIC_ADJ:
	    automore_enable("\n\tManual (static) TARP adjacencies:\n");
	    break;
	default:
	    return;
    }

    for (entry = (tarp_static_adjQ_type *)tarp_static_adjQ.qhead;
	 entry; entry = entry->next) {
	if (entry->type != type)
	    continue;
	
	printf("\n");
	if ((type == TARP_STATIC_MAP) && entry->tid)
	    printf("%25s", entry->tid);

        printf("\t%s", nsap_string(entry->nsap));
    }

    automore_disable();

    return;
}

/*
 * tarp_search_static_adjQ
 *
 * Search for a particular entry in the static adj queue, based on type. It's
 * assumed that 'addr' is an NSAP. The search key is an NSAP.
 */
tarp_static_adjQ_type *tarp_search_static_adjQ (uchar *addr, 
					        enum TARP_ADJ_ENTRY_TYPE type)
{
    tarp_static_adjQ_type *entry;

    for (entry = (tarp_static_adjQ_type *)tarp_static_adjQ.qhead;
	 entry; entry = entry->next) {
	if (entry->type != type)
	    continue;
	
	if (nsap_equ(entry->nsap, addr))
	    return (entry);
    }

    return (NULL);
}

/*
 * tarp_rqstBLOCK
 *
 * Block, awaiting a response for a request we have originated. The block
 * will be aborted if we receive a response, get interrupted or time out.
 */
static boolean tarp_rqstBLOCK (ulong arg)
{
    tarp_rqstQ_type *entry;
    
    entry = (tarp_rqstQ_type *)arg;

    /*
     * The status is set to TARP_RESP_SUCCESS when we receive a TARP
     * type 3 PDU.
     */
    if (entry->status == TARP_RESP_SUCCESS)     /* response received */
	return (FALSE);
    if (psipending2(entry->tty)) {              /* interrupted */
	entry->status = TARP_RESP_INTERRUPTED;
	return (FALSE);
    }
    if (TIMER_RUNNING_AND_AWAKE(entry->expiration_time)) { /* timed out */
	entry->status = TARP_RESP_TIMEOUT;
	return (FALSE);
    }

    return (TRUE);
}

/*
 * tarp_resolve_command
 *
 * Handles the TARP resolve commands (PDU types 1, 2 and 5). Makes an entry
 * in the TARP request queue, and then originates the appropriate PDU. Returns
 * TRUE upon success, FALSE otherwise. 'info' can be a TID (TARP PDU's 1 and 2)
 * or an NET (TARP PDU 5).
 */
static enum TARP_RESPONSE_TYPE tarp_resolve_command (uchar pdu_type, uchar *info)
{
    tarp_rqstQ_type *entry;
    ushort info_len;
    long time_out;
    clns_pdbtype *clns_pdb;
    uchar *clns_net;
    enum TARP_RESPONSE_TYPE status;
    enum TARP_HOST_TYPE tarp_host_type;

    tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net);
    if ((tarp_host_type != TARP_IS_TYPE) && (tarp_host_type != TARP_ES_TYPE)){
	printf("\n%% Need to have ISIS/ESIS running on this system");
        return(TARP_RESP_FAILURE);
    }

    if ((pdu_type != TARP_RQST_TID_L1_PDU) &&
	(pdu_type != TARP_RQST_TID_L2_PDU) &&
	(pdu_type != TARP_RQST_PROT_PDU)) {
	printf("\nInvalid TARP PDU type");
        return(TARP_RESP_FAILURE);
    }

    /* 
     * If we're originating a TARP type 5 PDU, the info being passed in is
     * an NET. Else (it's a type 1 or 2 PDU), it is a TID.
     */
    info_len = (pdu_type == TARP_RQST_PROT_PDU) ? info[0]+1 : strlen(info)+1;

    entry = malloc(sizeof(tarp_rqstQ_type));
    if (!entry) {
	return (TARP_RESP_FAILURE);
    }
    entry->tid_or_net = malloc(info_len);
    if (!entry->tid_or_net) {
	free(entry);
	return (TARP_RESP_FAILURE);
    }
    
    /*
     * Fill in the values for this request queue element.
     */
    bcopy(info, entry->tid_or_net, info_len);

    entry->tty = stdio;
    entry->type = pdu_type;
    entry->status = TARP_RESP_FAILURE;     /* The eternal pessimist ... */

    time_out = 0;
    switch(pdu_type) {
        case TARP_RQST_TID_L1_PDU:
	    time_out = tarp_t1_resp_timer;
	    break;
        case TARP_RQST_TID_L2_PDU:
	    time_out = tarp_t2_resp_timer;
	    break;
        case TARP_RQST_PROT_PDU:
	    time_out = tarp_arp_timer;
	    break;
    }

    /*
     * Start the timer for this request, enqueue it, and originate the
     * appropriate request.
     */
    TIMER_START(entry->expiration_time, time_out);
    enqueue(&tarp_rqstQ, entry);
    if ((pdu_type == TARP_RQST_TID_L1_PDU) ||
	(pdu_type == TARP_RQST_TID_L2_PDU))
	tarp_originate_packet(pdu_type, tarp_global_protocol_type,
			      tarp_my_tid, info,
			      clns_net, NULL);
    else if (pdu_type == TARP_RQST_PROT_PDU)
	tarp_originate_packet(pdu_type, tarp_global_protocol_type,
			      tarp_my_tid, NULL,
			      clns_net, info);

    psion(stdio);
    printf("\n%s", type_escape_seq_abort_msg);

    printf("\nSending TARP type %d PDU, timeout %d seconds ...",
	   pdu_type, time_out/ONESEC);
    flush();
    edisms((blockproc *)tarp_rqstBLOCK, (ulong)entry);
    status = entry->status;
    p_unqueue(&tarp_rqstQ, entry);
    free(entry->tid_or_net);
    free(entry);

    return(status);
}

/*
 * tarp_print_response
 *
 * Print appropriate response, based on PDU type and status.
 */
static void tarp_print_response (enum TARP_RESPONSE_TYPE status,
				 uchar pdu_type, tarp_tid_cache_type *entry)
{
    switch (status) {
        case TARP_RESP_INTERRUPTED:
	    printf("\n\n** Interrupted **\n");
	    return;
	case TARP_RESP_FAILURE:
	    printf("\n\n** General failure **\n");
	    return;
	case TARP_RESP_TIMEOUT:
	    printf("\n\n ** Request timed out for TARP PDU type %d **\n",
		   pdu_type);
	    return;
	case TARP_RESP_SUCCESS:
	    if (!entry) {
		printf("\n\n ** Could not find cache entry**\n");
		return;
	    }

	    if ((pdu_type == TARP_RQST_TID_L1_PDU) ||
		(pdu_type == TARP_RQST_TID_L2_PDU)) {
		printf("\n\n NET corresponding to TID %s is %n\n",
		       entry->tid, entry->net);
	    }
	    else if (pdu_type == TARP_RQST_PROT_PDU) {
	        printf("\n\n TID corresponding to NET %n is %s\n",
		       entry->net, entry->tid);
            }
	    return;
    }

    return;
}

/*
 * tarp_handle_resolve_command
 *
 * Originate a TARP packet to resolve a TID.
 */
static void tarp_handle_resolve_command (parseinfo *csb)
{
    tarp_tid_cache_type *entry;
    enum TARP_RESPONSE_TYPE status;
    uchar tarp_pdu_type;

    /*
     * By default, we'll assume that we're originating a TARP Level 1 PDU.
     */
    tarp_pdu_type = GETOBJ(int,1) ? GETOBJ(int,1) : TARP_RQST_TID_L1_PDU;

    /*
     * If we have the requested information in the cache, give it.
     * Don't bother searching the cache if cacheing has been disabled.
     */
    if (tarp_allow_cache_flag) {
	entry = tarp_find_tid_entry(GETOBJ(string,1),
				    strlen(GETOBJ(string,1)),
				    TARP_TID_KEY);
	if (entry) {
	    tarp_print_response(TARP_RESP_SUCCESS,
				TARP_RQST_TID_L1_PDU, entry);
	    return;
	}
	else {
	    /*
	     * special case...if we're trying to resolve our
	     * own TID and no cache entry was found, create a
	     * cache entry and return the info to the user.
	     */
	    if (!strcmp(tarp_my_tid, GETOBJ(string,1))) {
		tarp_modify_router_tid(GETOBJ(string, 1),
				       TRUE);
	        entry = tarp_find_tid_entry(GETOBJ(string,1),
					    strlen(GETOBJ(string,1)),
					    TARP_TID_KEY);
	        tarp_print_response(TARP_RESP_SUCCESS,
				    TARP_RQST_TID_L1_PDU, entry);
	        return;
	    }
	}
    }

    /*
     * Types 1 or 2 PDU's are sent when we need an NET corresponding
     * to a given TID. The packet is flooded to all neighbors, and the
     * destination NSAP address will be NULL. 
     */
     switch (tarp_pdu_type) {
         case TARP_RQST_TID_L1_PDU:
	     status = tarp_resolve_command(TARP_RQST_TID_L1_PDU,
					   GETOBJ(string,1));
	     entry = tarp_find_tid_entry(GETOBJ(string,1),
					 strlen(GETOBJ(string,1)),
					 TARP_TID_KEY);
	     if ((status == TARP_RESP_SUCCESS) ||
		 (status == TARP_RESP_INTERRUPTED) ||
		 (status == TARP_RESP_FAILURE)) {
	         tarp_print_response(status, TARP_RQST_TID_L1_PDU,
				     entry);
		     return;
	     }
	     else {
	         /*
		  * Else the L1 request timed out. Initiate a L2
	 	  * request. To do this, we deliberately fall into the
		  * next case statement.
		  */
		  printf("\n\nL1 request timed out, initiating L2 "
			 "request ...");
	     }

	 case TARP_RQST_TID_L2_PDU:
	     status = tarp_resolve_command(TARP_RQST_TID_L2_PDU,
					   GETOBJ(string,1));
	     entry = tarp_find_tid_entry(GETOBJ(string,1),
					 strlen(GETOBJ(string,1)),
					 TARP_TID_KEY);
	     tarp_print_response(status, TARP_RQST_TID_L2_PDU, entry);
	     break;
    }
} 

/*
 * tarp_handle_query_command
 *
 * Originate a TARP packet to resolve an NET.
 */
static void tarp_handle_query_command (parseinfo *csb)
{
    tarp_tid_cache_type *entry;
    enum TARP_RESPONSE_TYPE status;

    /*
     * If we have the requested information in the cache, give it.
     * Don't bother searching the cache if cacheing has been disabled.
     */
     if (tarp_allow_cache_flag) {
	 entry = tarp_find_tid_entry(GETOBJ(paddr,1)->clns_addr,
					    0, TARP_NET_KEY);
	 if (entry) {
	     tarp_print_response(TARP_RESP_SUCCESS, TARP_RQST_PROT_PDU, entry);
	     return;
	 }
	else {
	    clns_pdbtype *clns_pdb;
	    uchar *clns_net;
	    tarp_get_host_status(&clns_pdb, &clns_net);
	    /*
	     * special case...if we're trying to query our
	     * own NET and no cache entry was found, check if we
	     * have a TID defined for this system. If there is, create a
	     * cache entry and return the info to the user.
	     */
	    if (tarp_my_tid && clns_pdb && 
		       net_equ(clns_net, GETOBJ(paddr,1)->clns_addr)) {
		tarp_modify_router_tid(tarp_my_tid, TRUE);
	        entry = tarp_find_tid_entry(tarp_my_tid, 
					    strlen(tarp_my_tid),
					    TARP_TID_KEY);
	        tarp_print_response(TARP_RESP_SUCCESS,
				    TARP_RQST_TID_L1_PDU, entry);
	        return;
	    }
	}
     }

     /*
      * Type 5 PDU is sent when we need a TID corresponding to an
      * NET. The packet is 'unicast' to the specified NSAP, and the
      * target TID will be NULL.
      */
     status = tarp_resolve_command(TARP_RQST_PROT_PDU,
				   GETOBJ(paddr,1)->clns_addr);
     entry = tarp_find_tid_entry(GETOBJ(paddr,1)->clns_addr,
				 0, TARP_NET_KEY);
     tarp_print_response(status, TARP_RQST_PROT_PDU, entry);
}

/*
 * tarp_originate_command
 *
 * Originate a TARP PDU. The PDU type is sent in through OBJ(int,1). Before
 * we originate the PDU, we queue a request entry and match the received
 * response against this entry; this will prevent duplicate responses.
 */
void tarp_originate_command (parseinfo *csb)
{
    if (!tarp_my_tid) {
        printf("\n%% Need to assign a target ID for this system");
        return;
    }

    switch (csb->which) {
        case CMD_TARP_RESOLVE:
	    tarp_handle_resolve_command(csb);
	    break;

        case CMD_TARP_QUERY:
	    tarp_handle_query_command(csb);
	    break;

        case TARP_RESPONSE_PDU:
        case TARP_NOTIFY_PDU:
	default:
	    printf("%% Not yet implemented");
    }
}

/*
 * tarp_modify_router_tid
 *
 * Add/delete this router's TID. 
 */
void tarp_modify_router_tid (uchar *name, boolean add)
{
    int str_len;
    clns_pdbtype *clns_pdb;
    uchar *clns_net;
    uchar old_tid[TARP_MAX_TID_LEN];
    boolean first_time;
    enum TARP_HOST_TYPE tarp_host_type;

    if (!tarp_running) {
        printf("\n%% TARP not running");
        return;
    }

    first_time = TRUE;

    if (add) {
	/*
	 * If there is already a TID, save it in another buffer and then
	 * get rid of the TID and the corresponding cache entry.
	 */
	if (tarp_my_tid) {
	    first_time = FALSE;
	    bcopy(tarp_my_tid, old_tid, strlen(tarp_my_tid));
	    old_tid[strlen(tarp_my_tid)] = '\0';
	    tarp_delete_one_tid_entry(tarp_my_tid);
	}

	str_len = strlen(name);
	tarp_my_tid = malloc(str_len+1);
	if(!tarp_my_tid) {
	    tarp_traffic[TARP_NOMEM]++;
	    return;
	}

	/*
	 * Ensure that null terminator is also copied.
	 */
	bcopy(name, tarp_my_tid, str_len + 1);
	tarp_host_type = tarp_get_host_status(&clns_pdb, &clns_net);

	/*
	 * Update TID cache entry.
	 */
	if ((tarp_host_type == TARP_IS_TYPE) || 
	    (tarp_host_type == TARP_ES_TYPE)) {
	    
	    /*
	     * If the system has no NET, use the null NSAP for the TID
	     * entry.
	     */
	    if (clns_net)
		tarp_update_tid_entry(tarp_my_tid, str_len,
				      TARP_NSAP_PROT_ADDR_TYPE, clns_net+1,
				      *clns_net, TRUE, TARP_MY_ENTRY_FLAG);
	    else
		tarp_update_tid_entry(tarp_my_tid, str_len,
				      TARP_NSAP_PROT_ADDR_TYPE, 
				      &tarp_zero_nsap[1], tarp_zero_nsap[0],
				      TRUE, TARP_MY_ENTRY_FLAG);
	    
	    /*
	     * If we had a cache entry (for us) previously and now was changed,
	     * propagate this info to all our neighbors to inform them of the
	     * change.
	     */
	    if (!first_time) {
		tarp_propagate_new_info(old_tid, tarp_my_tid, clns_net);
	    }
	}
    }
    else {
	/* 
	 * We are deleting the TID. Delete the cache entry for this
	 * before freeing up the space.
	 */
	tarp_delete_one_tid_entry(tarp_my_tid);
	free(tarp_my_tid);
	tarp_my_tid = NULL;
    }
}

/*
 * tarp_nvgen_static_commands
 *
 * NVGEN the TARP static commands, based on queue element type.
 */
static void tarp_nvgen_static_commands (enum TARP_ADJ_ENTRY_TYPE type)
{
    tarp_static_adjQ_type *entry;

    for (entry = (tarp_static_adjQ_type *)tarp_static_adjQ.qhead;
	 entry; entry = entry->next) {

	if (entry->type != type)
	    continue;

	switch (type) {
	    case TARP_BLACKLIST:
	        nv_write (TRUE, "tarp blacklist-adjacency %n", entry->nsap);
		break;
	    case TARP_STATIC_ADJ:
		nv_write (TRUE, "tarp route-static %n", entry->nsap);
		break;
	    case TARP_STATIC_MAP:
		if (entry->tid)
		    nv_write (TRUE, "tarp map %s %n", entry->tid, entry->nsap);

		break;
	    default:
		break;
	}
    }

    return;
}

/*
 * tarp_command 
 *
 * Handle tarp global config commands
 */
void tarp_command (parseinfo *csb)
{
    tarp_tid_cache_type *my_cache_entry;

    if (csb->nvgen) {
	switch (csb->which) {
	    case TARP_RUNNING:
	        nv_write (tarp_running, csb->nv_command);
		break;
	    case TARP_LDB_TIMER:
	        nv_write (tarp_ldb_flush_timer != TARP_DEF_LDB_FLUSH_TIMER,
			  "%s %d", csb->nv_command, tarp_ldb_flush_timer/ONESEC);
		break;
	    case TARP_TID_TIMER:
	        nv_write (tarp_tid_cache_timer != TARP_DEF_TID_CACHE_TIMER,
			  "%s %d", csb->nv_command, tarp_tid_cache_timer/ONESEC);
		break;
	    case TARP_T1_RESP_TIMER:
	        nv_write (tarp_t1_resp_timer != TARP_DEF_TYPE1_TIMEOUT,
			  "%s %d", csb->nv_command, tarp_t1_resp_timer/ONESEC);
		break;
	    case TARP_T2_RESP_TIMER:
	        nv_write (tarp_t2_resp_timer != TARP_DEF_TYPE2_TIMEOUT,
			  "%s %d", csb->nv_command, tarp_t2_resp_timer/ONESEC);
		break;
	    case TARP_LDB_ZERO_SEQ_TIMER:
		nv_write (tarp_zero_seq_timer != TARP_DEF_LDB_ZERO_SEQ_TIMER,
			  "%s %d", csb->nv_command, tarp_zero_seq_timer/ONESEC);
		break;
	    case TARP_ARP_TIMER:
	        nv_write (tarp_arp_timer != TARP_DEF_ARP_RQST_TIMEOUT,
			  "%s %d", csb->nv_command, tarp_arp_timer/ONESEC);
		break;
	    case TARP_POST_TYPE2_TIMER:
	        nv_write (tarp_post_t2_timer != TARP_DEF_POST_TYPE2_TIMEOUT,
			 "%s %d",csb->nv_command, tarp_post_t2_timer/ONESEC);
		break;
	    case TARP_URC:
	        nv_write (tarp_global_urc != 0, "%s %d", csb->nv_command,
			  tarp_global_urc);
	        break;	    
	    case TARP_PROTOCOL_TYPE:
		nv_write(tarp_global_protocol_type != TARP_NSAP_PROT_ADDR_TYPE,
			 "%s %X", csb->nv_command, tarp_global_protocol_type);
		break;
	    case TARP_NSEL_TYPE:
		nv_write(tarp_global_nsel != TARP_DEF_NSEL_TYPE,
			 "%s %X", csb->nv_command, tarp_global_nsel);
		break;
	    case TARP_LIFETIME:
	        nv_write (tarp_lifetime != TARP_DEF_LIFETIME,
			  "%s %d", csb->nv_command, tarp_lifetime);
		break;
	    case TARP_SEQUENCE:
		/*
		 * No need to nvgen TARP sequence number command because
		 * we will always start with sequence # 0 upon startup.
		 */
		break;		
	    case TARP_MAP:
		tarp_nvgen_static_commands(TARP_STATIC_MAP);
		break;
	    case TARP_BLACKLIST_CMD:
		tarp_nvgen_static_commands(TARP_BLACKLIST);
		break;
	    case TARP_STATIC_ADJ_CMD:
		tarp_nvgen_static_commands(TARP_STATIC_ADJ);
		break;
	    case TARP_PROPAGATE_CMD:
		nv_write (!tarp_propagate_flag, "no %s", csb->nv_command);
		break;
	    case TARP_ORIGINATE_CMD:
		nv_write (!tarp_originate_flag, "no %s", csb->nv_command);
		break;
	    case TARP_DISABLE_CACHE_CMD:
		nv_write (!tarp_allow_cache_flag, "no %s", csb->nv_command);
		break;
	    case TARP_TID:
		nv_write (tarp_my_tid != NULL, "%s %s", csb->nv_command, tarp_my_tid);
		break;
	    default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}

	return;
    }

    switch (csb->which) {
        case TARP_RUNNING:
	    
	    if (csb->sense) {
		if (!tarp_running) 
		    tarp_start_process();
	    }
	    else 
		tarp_running = FALSE;

	    break;
	case TARP_LDB_TIMER:
	    tarp_ldb_flush_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) :
		                                 TARP_DEF_LDB_FLUSH_TIMER;
	    break;
	case TARP_TID_TIMER:
	    tarp_tid_cache_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) :
		                                 TARP_DEF_TID_CACHE_TIMER;
	    break;
	case TARP_T1_RESP_TIMER:
	    tarp_t1_resp_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) :
		                                 TARP_DEF_TYPE1_TIMEOUT;
	    break;
	case TARP_T2_RESP_TIMER:
	    tarp_t2_resp_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) : 
		                                 TARP_DEF_TYPE2_TIMEOUT;
	    break;
	case TARP_LDB_ZERO_SEQ_TIMER:
	    tarp_zero_seq_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) : 
		                                 TARP_DEF_LDB_ZERO_SEQ_TIMER;
	    break;
	case TARP_ARP_TIMER:
	    tarp_arp_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) : 
		                                 TARP_DEF_ARP_RQST_TIMEOUT; 
	    break;
	case TARP_POST_TYPE2_TIMER:
	    tarp_post_t2_timer = (csb->sense) ? (GETOBJ(int,1) * ONESEC) :
		                                 TARP_DEF_POST_TYPE2_TIMEOUT;
	    break;
	case TARP_STATIC_ADJ_CMD:
	    tarp_update_static_adjQ(GETOBJ(paddr,1)->clns_addr,
				    TARP_STATIC_ADJ, NULL, csb->sense);
	    break;
        case TARP_PROPAGATE_CMD:
	    tarp_propagate_flag = csb->set_to_default || csb->sense;
	    break;
        case TARP_ORIGINATE_CMD:
	    tarp_originate_flag = csb->set_to_default || csb->sense;
	    break;
	case TARP_DISABLE_CACHE_CMD:
	    tarp_allow_cache_flag = csb->set_to_default || csb->sense;
	    /*
	     * Get rid of TID<->NET cache if we're disabling the cacheing
	     * capability.
	     */
	    if (!csb->sense)
		tarp_clear_tid_table(TRUE);
	    else {
		/*
		 * If we are enabling cacheing, add our own TID entry to the
		 * cache if there is no cache entry already.
		 */
		if (tarp_my_tid) {
		    my_cache_entry = tarp_find_tid_entry(tarp_my_tid, 
							 strlen(tarp_my_tid),
							 TARP_TID_KEY);
		    if (!my_cache_entry) {
			tarp_modify_router_tid(tarp_my_tid, TRUE);
			TARP_EVBUGINF("\nTARP-EV: Added cache entry for our TID");
		    }
		}
	    }
	    break;
	case TARP_BLACKLIST_CMD:
	    tarp_update_static_adjQ(GETOBJ(paddr,1)->clns_addr, TARP_BLACKLIST,
				    NULL, csb->sense);
	    break;
	case TARP_MAP:
	    tarp_update_static_adjQ(GETOBJ(paddr,1)->clns_addr,
				    TARP_STATIC_MAP, GETOBJ(string,1),
				    csb->sense);
	    break;
	case TARP_SEQUENCE:
	    tarp_global_sequence_num = (csb->sense) ? GETOBJ(int,1) : 0;
	    break;
	case TARP_LIFETIME:
	    tarp_lifetime = (csb->sense) ? GETOBJ(int,1) : TARP_DEF_LIFETIME;
	    break;
	case TARP_NSEL_TYPE:
	    tarp_global_nsel = (csb->sense) ? GETOBJ(int,1) :
		                                    TARP_DEF_NSEL_TYPE;
	    break;
	case TARP_PROTOCOL_TYPE:    
	    tarp_global_protocol_type = (csb->sense) ? GETOBJ(int,1) :
		                                    TARP_NSAP_PROT_ADDR_TYPE;
	    break;	
	case TARP_URC:
	    tarp_global_urc = (csb->sense) ? GETOBJ(int,1) : 0;
	    break;	
	case TARP_TID:
	    tarp_modify_router_tid(GETOBJ(string, 1), csb->sense);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}

/* 
 * tarp_intf_enable 
 * 
 * Enable TARP on interface. If TARP process is not running, start it.
 *
 */
static void tarp_intf_enable (idbtype *idb)
{
    if (!tarp_running) 
	tarp_start_process();

    idb->tarp_enabled = TRUE;
    return;
}

/* 
 * tarp_intf_disable
 * 
 * Disable TARP on interface
 *
 */
static void tarp_intf_disable (idbtype *idb)
{
    idb->tarp_enabled = FALSE;
    return;
}

/*
 * tarp_intfc_command
 *
 * Handle interface commands for TARP
 */
void tarp_intfc_command (parseinfo *csb)
{
    idbtype *idb;

    idb = csb->interface;

    if (csb->nvgen) {
	switch (csb->which) {
	    case TARP_ENABLE:
	        nv_write (idb->tarp_enabled, csb->nv_command);
		break;
	    case TARP_PROPAGATE_CMD:
	        nv_write (!idb->tarp_propagate, "no %s", csb->nv_command);
		break;
	    default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}

	return;
    }

    switch (csb->which) {
        case TARP_ENABLE:
	    if (csb->sense) 
		tarp_intf_enable(idb);
	    else 
		tarp_intf_disable(idb);
	    break;
        case TARP_PROPAGATE_CMD:
	    idb->tarp_propagate = csb->set_to_default || csb->sense;
	    break;	    
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}

/*
 * display_tarp_int_info 
 *
 * Display TARP-relevant interface information.
 */

static void display_tarp_int_info (idbtype *idb)
{
    if (idb->tarp_enabled) {
        printf("\n%s is %s, line protocol is %s, encapsulation is %s", 
               idb->namestring, print_idbstate(idb), 
               interface_up(idb) ? "up" : "down",
	       encstring(idb->hwptr->enctype));
	printf("\nTARP propagation is %sabled on this interface",
	       (idb->tarp_propagate) ? "en" : "dis");
    }

    return;
}

/*
 * show_tarpglobal
 *
 * Display all global TARP information
 */
static void show_tarpglobal (void)
{
    printf("\n\nGlobal TARP information:");
    printf("\n  TID of this station is \"%s\"", tarp_my_tid != NULL ?
	                                     tarp_my_tid : "");
    printf("\n  Timer T1 (timer for response to TARP Type 1 PDU) is %d "
	       "seconds", tarp_t1_resp_timer/ONESEC);
    printf("\n  Timer T2 (timer for response to TARP Type 2 PDU) is %d "
	       "seconds", tarp_t2_resp_timer/ONESEC);
    printf("\n  Timer T3 (timer for response to ARP request) is %d "
	       "seconds", tarp_arp_timer/ONESEC);
    printf("\n  Timer T4 (timer that starts when T2 expires) is %d "
	       "seconds", tarp_t1_resp_timer/ONESEC);
    printf("\n  Loop Detection Buffer entry timeout : %d "
	       "seconds", tarp_ldb_flush_timer/ONESEC);
    printf("\n  Loop Detection Buffer zero sequence timer is %d "
	        "seconds", tarp_zero_seq_timer/ONESEC);
    printf("\n  TID cache entry timeout : %d "
	       "seconds", tarp_tid_cache_timer/ONESEC);
    printf("\n  This station will %spropagate TARP PDUs",
	        (tarp_propagate_flag && clns_router_running) ? "" : "not ");
    printf("\n  This station will %soriginate TARP PDUs",
	        tarp_originate_flag ? "" : "not ");
    printf("\n  TID<->NET cache is %sabled",
	        tarp_allow_cache_flag ? "en" : "dis");
    printf("\n  Sequence number that next packet originated by this station "
	       "will have : %d", tarp_global_sequence_num);
    printf("\n  Update remote cache (URC) bit is %d", tarp_global_urc);
    printf("\n  Packet lifetime : %d hops", tarp_lifetime);
    printf("\n  Protocol type used in outgoing packets : \"%X\"",
	        tarp_global_protocol_type);
    printf("\n  N-Selector used in TARP PDU's : \"%X\"", tarp_global_nsel);
    printf("\n");
  
    return;
}


/*
 * show_tarp_counters
 *
 * Display all TARP counters
 */
static void show_tarp_counters (void)
{
    printf("\nTARP counters :");
    printf("\n\tPackets output: %d, Input: %d", tarp_traffic[TARP_OUTPUT],
	   tarp_traffic[TARP_INPUT]);
    printf("\n\tHdr syntax: %d", tarp_traffic[TARP_HDRSYNTAX]);
    printf("\n\tNo memory: %d, ", tarp_traffic[TARP_NOMEM]);
    printf("Invalid packet: %d ", tarp_traffic[TARP_BADPACKET]);
    printf("\n\tLifetime exceeded: %d ", tarp_traffic[TARP_LIFETIME]);    
    return;
}

/*
 * show_one_tid_cache_entry
 *
 * Display info for a particular TARP cache entry. Static cache entries are
 * preceded by an "*". The cache entry for this host is preceded by an "&".
 * If 'detail' is TRUE, show additional information.
 *
 * Cache entry is locked prior to calling this routine.
 */
static void show_one_tid_cache_entry (tarp_tid_cache_type *cache_entry,
			       boolean detail)
{
    int t;

    if (cache_entry->flags & TARP_STATIC_FLAG)
	printf("\n* ");
    else if (cache_entry->flags & TARP_MY_ENTRY_FLAG)
	printf("\n& ");
    else
	printf("\n  ");

    printf("%25s", cache_entry->tid);
    printf("   %s", nsap_string(cache_entry->net));

    if (detail) {
	/*
	 * Static entries or our own entry don't expire.
	 */
	if (cache_entry->flags & (TARP_STATIC_FLAG | TARP_MY_ENTRY_FLAG))
	    printf("\n  Expiration time : NONE");
	else {
	    t = 0;
	    if (TIMER_RUNNING(cache_entry->expiration_time))
		t = TIME_LEFT_SLEEPING(cache_entry->expiration_time);

	    printf("\n  Expiration time : %d seconds ", t/ONESEC);
	}
	printf("\n");
    }
}

/*
 * show_tarp_host
 *
 * Display info on a specific TARP host.
 */
static void show_tarp_host (uchar *host)
{
    tarp_tid_cache_type *cache_entry, *temp_cache_entry;
    int index, t;

    if (!host)
	return;
    
    index = 0;
    cache_entry = tarp_walk_tid_table(&index, NULL);

    while (cache_entry) {
	mem_lock(cache_entry);
	if (strcmp(cache_entry->tid, host) == 0) {
	    printf("\n\nTID of entry : %s", cache_entry->tid);
	    printf("\nNET of entry : %n", cache_entry->net);
	    printf("\nEntry type: %s", tarp_entry_type(cache_entry));

	    if (cache_entry->flags & (TARP_STATIC_FLAG | TARP_MY_ENTRY_FLAG)) 
		printf("\nExpiration time : NONE");
	    else {
		t = 0;
		if (TIMER_RUNNING(cache_entry->expiration_time))
		    t = TIME_LEFT_SLEEPING(cache_entry->expiration_time);
		printf("\nExpiration time : %d seconds", t/ONESEC);
	    }
		
	    printf("\n");
	    free(cache_entry);
	    break;
        }
	
        temp_cache_entry = tarp_walk_tid_table(&index, cache_entry);
        free(cache_entry);
        cache_entry = temp_cache_entry;
    }
}

/*
 * show_one_ldb_cache_entry
 *
 * Display info for a particular LDB entry. Cache entry is locked before
 * this routine is called.
 */
static void show_one_ldb_cache_entry (tarp_ldb_type *cache_entry)
{
    int t;

    printf("\n %20s", station_string(cache_entry->sys_id));
    printf("%-10d", cache_entry->sequence);

    if (TIMER_RUNNING(cache_entry->expiration_time))
	t = TIME_LEFT_SLEEPING(cache_entry->expiration_time);
    else
	t = 0;

    printf("%-15d", t/ONESEC);

    if (TIMER_RUNNING(cache_entry->zero_seq_time))
	t = TIME_LEFT_SLEEPING(cache_entry->zero_seq_time);
    else
	t = 0;

    printf("%-15d", t/ONESEC);    
}

/*
 * show_tarp_ldb_cache
 *
 * Display the TARP LDB (Loop Detection Buffer).
 */
void show_tarp_ldb_cache (void)
{
    char *str ={ "\n   System ID        Sequence Number    Expiration (sec)    Zero-sequence timer"};

    tarp_ldb_type *entry, *temp_entry;
    int index;

    automore_enable(str);
    index = 0;
    entry = tarp_walk_ldb_table(&index, NULL);
    
    while (entry) {
	mem_lock(entry);
	show_one_ldb_cache_entry(entry);
        temp_entry = tarp_walk_ldb_table(&index, entry);
        free(entry);
        entry = temp_entry;
    }

   automore_disable();
}

/*
 * show_tarp_tid_cache
 *
 * Display the TARP TID  cache. If 'detail' flag is TRUE, show additional
 * information.
 */
void show_tarp_tid_cache (boolean detail)
{
    char *str ={ "\nTID ('*' : static; & : local)                   NSAP\n"};

    tarp_tid_cache_type *entry, *temp_entry;
    int index;

    /*
     * Don't bother if the 'tarp_allow_cache' flag is FALSE.
     */
    if (!tarp_allow_cache_flag) {
	printf("\n%% TID<->NET cache disabled");
	return;
    }

    automore_enable(str);
    index = 0;
    entry = tarp_walk_tid_table(&index, NULL);
    
    while (entry) {
	mem_lock(entry);
	show_one_tid_cache_entry(entry, detail);
        temp_entry = tarp_walk_tid_table(&index, entry);
        free(entry);
        entry = temp_entry;
    }

   automore_disable();
}

/*
 * tarp_clear_counters
 *
 * Clear all TARP counters
 */
void tarp_clear_counters (void)
{
    int i;

    for (i = 0; i < TARP_MAXTRAFFIC; i++)
	tarp_traffic[i] = 0;

    return;
}

/*
 * tarp_handle_tid_table_delete
 *
 * Basically a jacket routine called from the parser to delete the TID
 * cache. It calls the routine 'tarp_clear_tid_table' with the right
 * argument.
 */
void tarp_handle_tid_table_delete (parseinfo *csb)
{
    tarp_clear_tid_table(GETOBJ(int,1));
}

/*
 * show_tarp 
 *
 * Show global TARP information
 */
void show_tarp (parseinfo *csb)
{

    idbtype *idb;

    if (!tarp_running) {
	printf("%% TARP is not enabled");
	automore_disable();
	return;
    }

    idb = GETOBJ(idb, 1);
    switch (csb->which) {

        case SHOW_TARP_GLOBAL:
	    automore_enable(NULL);
	    show_tarpglobal();
	    automore_disable();
	    break;

        case SHOW_TARP_COUNTERS:
	    automore_enable(NULL);
	    show_tarp_counters();
	    automore_disable();
	    break;

        case SHOW_TARP_HOST:
	    automore_enable(NULL);
	    /*
	     * GETOBJ(string,1) is the TID of the entry to be displayed
	     */
	    show_tarp_host(GETOBJ(string,1));
	    automore_disable();
	    break;
	    
        case SHOW_TARP_TID_CACHE:
	    /*
	     * OBJ(int,1) is TRUE if "detail" keyword was specified.
	     */
	    show_tarp_tid_cache(GETOBJ(int,1));
	    break;

        case SHOW_TARP_LDB_CACHE:
	    show_tarp_ldb_cache();
	    break;

	case SHOW_TARP_BLACKLIST_ADJ:
	    tarp_show_adjacencyQ(TARP_BLACKLIST);
	    break;

	case SHOW_TARP_STATIC_ADJ:
	    tarp_show_adjacencyQ(TARP_STATIC_ADJ);
	    break;

	case SHOW_TARP_STATIC_MAP:
	    tarp_show_adjacencyQ(TARP_STATIC_MAP);
	    break;

        case SHOW_TARP_INTERFACE:
	    automore_enable(NULL);
	    /*
	     * If no interface specified, display list of interfaces
	     * on which TARP has been enabled.
	     */
	    if (!idb) {
		FOR_ALL_SWIDBS(idb) {
		    display_tarp_int_info(idb);
		}
		return;
	    }
	    show_swidb_chain(idb, display_tarp_int_info, 0);
	    automore_disable();
	    break;
    }
}

