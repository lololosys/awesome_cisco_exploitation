/* $Id: ospf_adj.c,v 3.3.42.3 1996/08/28 12:53:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_adj.c,v $
 *------------------------------------------------------------------
 * ospf_adj.c -- OSPF adjaceny support 
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_adj.c,v $
 * Revision 3.3.42.3  1996/08/28  12:53:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.42.2  1996/08/26  22:28:35  myeung
 * CSCdi63803:  2nd type7 lsa is generated and translated when interface
 * is shut
 * Branch: California_branch
 *
 * Revision 3.3.42.1  1996/04/16  19:01:40  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/05  10:17:31  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  17:35:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/14  00:26:35  myeung
 * CSCdi41883:  OSPF LSA fragmentation crashes router
 * - Allow to send update containing huge LSA
 *
 * Revision 2.4  1995/09/19  20:53:46  myeung
 * CSCdi20080:  OSPF crashed when sending LSA in send_ls_update
 *
 * Revision 2.3  1995/06/19  01:08:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/13  17:16:50  aboonkon
 * CSCdi35774:  OSPF should accept Type 4 LSA received in a stub area
 *
 * Be more liberal in receiving.  It doesn't hurt to accept it, only cause
 * an extra LSA in the database.
 *
 * Revision 2.1  1995/06/07  21:09:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Routines exported from this mode
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"


/*
 * Storage
 */

#define IS_IT_LAST(nbr)	(nbr->nbr_dbd_list &&(!(nbr->nbr_dbd_list->list_next)))


/*
 * ls_req_insert
 * insert this request in
 * neighbor request list
 */
static void ls_req_insert (
    nbrtype *nbr,
    ls_req_list *req_ptr,
    uchar type)
{
   ls_req_list *temp_ptr;
   ls_req_list *temp1_ptr;

   temp1_ptr = temp_ptr = nbr->nbr_ls_req[type];

   req_ptr->req_next = NULL;
   req_ptr->req_prev = NULL;
   if (temp1_ptr) {
       req_ptr->req_next = temp1_ptr;
       temp1_ptr->req_prev = req_ptr;
   }
   req_ptr->req_flag = REQ_INQUEUE;
   nbr->nbr_ls_req[type] = req_ptr;

} /* end of ls_req_insert */


/*
 * ospf_send_req
 * build and send a db req packet 
 */
void ospf_send_req (idbtype *idb, nbrtype *nbr)
{
    paktype *pak;
    ls_req *req_ptr;
    uchar type;
    ls_req_list *entry_ptr;
    ushort length;
    ospf_idb_info *ospf_info = idb->ospf_info;

    /* check if there are any requests to send */
    if (NO_REQ(nbr))
	return;
    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE + MAX_OSPF_DATA);
    if (!pak)
	return;
    req_ptr = (ls_req *)OSPF_DATA_PTR(pak);
    length = 0;
    /* go through all request */
    for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	if (type == LSA_TYPE_IGNORE)
	    continue;
	for (entry_ptr = nbr->nbr_ls_req[type]; entry_ptr;
	    entry_ptr = entry_ptr->req_next) {
	    req_ptr->lsr_type = type;
	    req_ptr->lsr_id = entry_ptr->req_ls_id;
 	    req_ptr->lsr_adv_rtr = entry_ptr->req_adv_rtr;
	    entry_ptr->req_flag = REQ_SENT;
	    length += LS_REQ_SZ;
#ifdef NOTDEF
	    ospf_adj_buginf("\nOSPF: put LS request for LSID %i, type %d,"
			    " adv. rtr. %i in LS REQ packet",
			    req_ptr->lsr_id, req_ptr->lsr_type, 
			    req_ptr->lsr_adv_rtr);
#endif
	    if (length + LS_REQ_SZ > MAX_OSPF_DATA) {
		/*
		 * insert this packet in retransmit queue
		 * and start retransmit timer .
		 */
		mgd_timer_start(&nbr->nbr_poll_timer, 
				ospf_info->retr_interval*ONESEC);
		ospf_send_packet(pak, idb, OSPF_LS_REQ, length,
				 nbr->nbr_ipaddr, FALSE);
		return;
	    }
	    req_ptr++;
	}
    }
    if (length) {
	/*
	 * insert this packet in retransmit queue
	 * and start retransmit timer .
	 */
	mgd_timer_start(&nbr->nbr_poll_timer, ospf_info->retr_interval*ONESEC);
	ospf_send_packet(pak, idb, OSPF_LS_REQ, length,
			 nbr->nbr_ipaddr, FALSE); 

	ospf_adj_buginf("\nOSPF: Database request to %i ", nbr->nbr_id);
    } else
        retbuffer(pak);
    ospf_adj_buginf("\nOSPF: sent LS REQ packet to %i, length %d", 
 		    nbr->nbr_ipaddr, length);
} /* end of ospf_send_req */



/*
 * ospf_rcv_req
 * Receive a request
 */
void ospf_rcv_req (paktype *pak, ospftype *ospf, areatype *area)
{
    iphdrtype *ip;
    ls_req *req_ptr;
    lsdbtype *db;
    nbrtype *nbr;
    ushort length;
    idbtype *idb;
    ospf_idb_info *ospf_info;
    list_queue nbr_update_queue;

    nbr_update_queue.q_first = NULL;
    nbr_update_queue.q_last = NULL;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input; 
    ospf_info = idb->ospf_info;

    /* find the neighbour */
    nbr = ospf_find_nbr(idb, ospf->ospf_rtr_id, ip->srcadr,FALSE);
    if (nbr == NULL) {
	errmsg(&msgsym(NONEIGHBOR, OSPF), "database request", 
	       ospf->ospf_rtr_id);
        return;
    }
    /* 
     * ignore this packet if neighbor is in any
     * state less than NBR_EXCHANGE
     */
     if (nbr->nbr_state < NBR_EXCHANGE) 
	 return;

    /* make sure the packet is not empty */
    if (ospf->ospf_length <= OSPF_HDR_SIZE)
	return;
    req_ptr = (ls_req *) (ospf->ospf_data);

    length = ospf->ospf_length - OSPF_HDR_SIZE;

    /* Walk the request until we're out of data. */

    while (length >= LS_REQ_SZ) {
	db = NULL;
	if (INVALID_LSA_TYPE(req_ptr->lsr_type)) {
	    /*
	     * this request is invalid.
	     */
	    errmsg(&msgsym(BADLSATYPE, OSPF), "req", req_ptr->lsr_type,
		   req_ptr->lsr_id, req_ptr->lsr_adv_rtr,
		   ip->srcadr, idb->namestring);
	} else {
	    db = db_find_lsa(area,
			     req_ptr->lsr_id,
			     req_ptr->lsr_adv_rtr,
			     (uchar)(req_ptr->lsr_type));
	}
	if ((db == NULL) || (db->db_lsa_ptr == NULL)) {
	    /*
	     * some thing is wrong happend 
	     * free the packet and generate
	     * event BAD_LS_REQ
	     */
	    (*nbr_state_machine[NBR_EVENT_BAD_LS_REQ][nbr->nbr_state])
	                                      (idb, nbr);
	    break;
	}
	add_to_update_list(db, &nbr_update_queue, TRUE);
	length -= LS_REQ_SZ;
	req_ptr++;
    }
    if (nbr_update_queue.q_first) {
	send_ls_update(nbr_update_queue.q_first, idb, nbr, FALSE);
	free_db_list(nbr_update_queue.q_first);
    }
} /* end of ospf_rcv_req */

/* 
 * dbd_sum_alloc
 * allocate a packet for building 
 * summary packet 
 */
static listype *dbd_sum_alloc (
    idbtype *idb,
    ushort  size)
{
    paktype *pak;
    listype *head;

    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE + size);
    if (!pak) {
	return(NULL);
    }
    head = (listype *)(ipheadstart(pak) + IPHEADERBYTES(NOPT));
    head->list_pak_ptr = pak;
    head->list_next = NULL;
    return(head);

} /* end of dbd_sum_alloc */

/* 
 * build_dbdsum:
 * builds  database description summary packets 
 * which includes the ls_db for this rtr
 */
void build_dbd_sum (idbtype *idb, nbrtype *nbr)
{
    areatype *area;
    lsdbtype *db;
    lsatype *lsa;
    lsatype *dbd_piece_ptr;
    dbdtype *dbd;
    listype *hdr_ptr;
    listype *last_ptr;
    listype *n_hdr_ptr;
    ushort length;
    uchar type;
    listype *blank;
    int count = 0;
    ospf_idb_info *ospf_info = idb->ospf_info;

    area = ospf_info->area_ptr;
    last_ptr = NULL;
    /*
     * Free summary list and request list
     * if there are any 
     */ 
    free_dbd_sum(nbr);
    free_ls_req_list(idb, nbr, NULL, 0);

    if ((hdr_ptr = dbd_sum_alloc(idb, MAX_OSPF_DATA)) == NULL) {
	buginf("\nOSPF: could not allocate summary packet\n");
	return;
    }
    /* get pointer to dbd header in the packet */
    dbd = (dbdtype *)OSPF_DATA_PTR(hdr_ptr->list_pak_ptr);
    dbd_piece_ptr = dbd->dbd_lsas;
    nbr->nbr_dbd_list = hdr_ptr;
    length = DBD_HDR_SIZE;
    hdr_ptr->list_seq = 0;
    /* grab all of the links */
    for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	if (type == LSA_TYPE_IGNORE)
	    continue;
	/* ignore all external into stub area */
	if ((area->ar_stub || area->ar_nssa) && 
	    ((type == LSA_TYPE_ASE) || (type == LSA_TYPE_SUM_ASB)))
	    continue;
	if (!area->ar_nssa && (type == LSA_TYPE_7_ASE))
	    continue;
	/* ignore externals into vitual links */
	if ((ospf_info->if_type == IF_VIRTUAL_LINK) && (type == LSA_TYPE_ASE))
	    continue;
	count = 0;
	for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	     db = db_get_next_db(db)) {

	    if (db->db_lsa_ptr == NULL) 
		continue;
	    /*
	    check_ospf_suspend_limit(area->ar_pdb_ptr, DBASE_ACTIVE, &count);
	    */
	    lsa = db->db_lsa_ptr;
	    if (ADV_AGE(db) >= MAXAGE) {
		add_db_to_nbr_retrans(nbr, db, idb);
		continue;
	    }
	    if (length + LINK_STATE_HEADER_SZ >= MAX_OSPF_DATA) {
		/*
		 * end of this packet. assume that more
		 * packets will follow for now
		 */
		dbd->dbd_i_m_ms = M_BIT;
		hdr_ptr->list_length = length;
		last_ptr = hdr_ptr;
		/*
		 * alloc next packet
		 */
		if ((n_hdr_ptr= dbd_sum_alloc(idb,MAX_OSPF_DATA))== NULL) {
		    errmsg(&msgsym(NOMEMORY, OSPF), "DBD summary");
		    free_dbd_sum(nbr);
		    return;
		}
		hdr_ptr->list_next = n_hdr_ptr;
		hdr_ptr = n_hdr_ptr;
		hdr_ptr->list_seq = 0;
		/* get pointer to dbd header in the packet */
		dbd = (dbdtype *)OSPF_DATA_PTR(hdr_ptr->list_pak_ptr);
                dbd_piece_ptr = dbd->dbd_lsas;
		length = DBD_HDR_SIZE;
	    }
	    dbd_piece_ptr->lsh_type = type;
	    dbd_piece_ptr->lsh_options = lsa->lsh_options;
	    dbd_piece_ptr->lsh_id = lsa->lsh_id;
	    dbd_piece_ptr->lsh_adv_rtr =   lsa->lsh_adv_rtr;
	    dbd_piece_ptr->lsh_seq = lsa->lsh_seq;
	    dbd_piece_ptr->lsh_chksum = lsa->lsh_chksum;
	    dbd_piece_ptr->lsh_length = lsa->lsh_length; 
	    dbd_piece_ptr->lsh_age = ADV_AGE(db);
#ifdef NOTDEF
	    ospf_adj_buginf("\nOSPF: put LSA header in DBD packet");
	    ospf_adj_buginf("\n      LSID %i, type %d, adv. rtr. %i"
			    ", seq %#10x, age %6d",
			    lsa->lsh_id, lsa->lsh_type, 
			    lsa->lsh_adv_rtr, lsa->lsh_seq, lsa->lsh_age);
#endif
	    length += LINK_STATE_HEADER_SZ;
	    dbd_piece_ptr++;
	}
    }
    /* put one blank packet at the end of database description 
     * packet list so that it would be sent at the end of database 
     * exchange when slave has no more DBD packet to send. 
     * This is to avoid the problem of master putting the same
     * LSA (which is in that last DBD packet) into LS REQ list
     * more than once. 
     */
    /*  check if the last packet is empty */
    if (length ==  DBD_HDR_SIZE) {
	hdr_ptr->list_next = NULL;
	hdr_ptr->list_length = length;
	dbd->dbd_i_m_ms = 0;
	return;
    } else {  /* allocate new blank packet */
	if ((blank = dbd_sum_alloc(idb, DBD_HDR_SIZE)) == NULL) {
	    buginf("\nOSPF: Could not allocate DBD");
	    return;
	}
	blank->list_length = DBD_HDR_SIZE;
	blank->list_next   = NULL;
	blank->list_seq    = 0;
	dbd = (dbdtype *)OSPF_DATA_PTR(blank->list_pak_ptr);
	dbd->dbd_i_m_ms = 0;
	hdr_ptr->list_next   = blank;
	hdr_ptr->list_length = length;
	dbd = (dbdtype *)OSPF_DATA_PTR(hdr_ptr->list_pak_ptr);
	dbd->dbd_i_m_ms = M_BIT;
    }
} /* end of build_dbdsum */

/*
 * ospf_send_dbd
 * send a dbd packet
 */
void ospf_send_dbd (idbtype *idb, nbrtype *nbr)
{
    ushort length = 0;
    listype *list;
    paktype *pak;
    dbdtype *dbd;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if (nbr->nbr_dbd_list == NULL) {
	if ((nbr->nbr_state == NBR_EXSTART)
	    || (nbr->nbr_state == NBR_EXCHANGE)) {
	    if ((list = dbd_sum_alloc(idb, DBD_HDR_SIZE)) == NULL) {
		buginf("\nOSPF: Could not allocate DBD");
	        return;
	    }
	    nbr->nbr_dbd_list = list;
	    list->list_length = DBD_HDR_SIZE;
	} else
	    return;
    } else
	list = nbr->nbr_dbd_list;
    pak = list->list_pak_ptr;
    dbd = (dbdtype *)OSPF_DATA_PTR(pak);
    length  = list->list_length;
    dbd->dbd_options = ospf_info->area_ptr->ar_options; 
    if (ospf_info->oi_demand_circuit)
	dbd->dbd_options |= OPTION_DC_BIT;
    dbd->dbd_reserved = 0;
    if (nbr->nbr_state == NBR_EXSTART)
	dbd->dbd_i_m_ms = (I_BIT | M_BIT | MS_BIT);
    else 
	if (length == DBD_HDR_SIZE)
	    dbd->dbd_i_m_ms = nbr->nbr_i_ms;
	else
	    dbd->dbd_i_m_ms |= nbr->nbr_i_ms;
    dbd->dbd_seq = nbr->nbr_seq;
    list->list_seq = nbr->nbr_seq;
    ospf_adj_buginf("\nOSPF: Send DBD to %i on %s seq %#x opt %#x "
		    "flag %#x len %d",
		    nbr->nbr_id, idb->namestring, dbd->dbd_seq,
		    dbd->dbd_options, dbd->dbd_i_m_ms, length + OSPF_HDR_SIZE);
    ospf_send_packet(pak, 
		     idb, 
		     OSPF_DB_DES,
		     length,
		     nbr->nbr_ipaddr,
		     TRUE	/* dupe the packet */
		     );
    /*
     * start the retransmission timer only
     * if we are in MASTER mode.
     */
    if (nbr->nbr_mode == NBR_ADJ_MASTER)
	mgd_timer_start(&nbr->nbr_dbd_retrs_timer, 
			ospf_info->retr_interval*ONESEC);

} /* end of ospf_send_dbd */


/*
 * ospf_rcv_dbds
 * receive a dbd from
 * neighbor
 */
void ospf_rcv_dbd (paktype *pak, ospftype *ospf, areatype *area)
{
    iphdrtype *ip;
    dbdtype *dbd;
    lsatype *lsa;
    ls_req_list  *ls_req_ptr;
    nbrtype *nbr;
    idbtype *idb;
    listype *tlist_ptr;
    ushort length;
    ospf_idb_info *ospf_info;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    ospf_info = idb->ospf_info;
    dbd = (dbdtype *)(ospf->ospf_data);
    /* find the neighbour */
    nbr = ospf_find_nbr(idb, ospf->ospf_rtr_id, ip->srcadr, FALSE);
    if (nbr == NULL) {
	errmsg(&msgsym(NONEIGHBOR, OSPF), "database description", 
	       ospf->ospf_rtr_id);
        return;
    }

    ospf_adj_buginf("\nOSPF: Rcv DBD from %i on %s seq %#x "
		    "opt %#x flag %#x len %d state %s", 
		    ospf->ospf_rtr_id, idb->namestring, dbd->dbd_seq,
		    dbd->dbd_options, dbd->dbd_i_m_ms, ospf->ospf_length,
		    ospf_nbr_state_string(nbr->nbr_state));
    /* 
     * ignore this packet if neigbour is in any
     * state less than NBR_INIT
     */
    if (nbr->nbr_state < NBR_INIT) {
	ospf_adj_buginf("\nOSPF: Nbr state is less than INIT");
	return;
    }

    /* if it is in INIT state generate
     * 2WAY event.
     */
    if (nbr->nbr_state == NBR_INIT)
	(*nbr_state_machine[NBR_EVENT_2WAY][nbr->nbr_state])
	                                      (idb, nbr);

    /* if the neighbour ends up with 2WAY
     * state ignore this packet as there is
     * no need to bring up the adjuceny
     */
    if (nbr->nbr_state == NBR_2WAY) {
	ospf_adj_buginf("\nOSPF: Nbr state is 2WAY");
	return;
    }

    /*
     * Handle hello suppression negotiation
     */
    if (GET_DC_BIT(dbd->dbd_options)) {
	if (OSPF_DC_IF_TYPE(ospf_info)) {
	    if (!nbr->nbr_suppress_hello) {
		nbr->nbr_suppress_hello = TRUE;
		ospf_info->oi_suppress_hello_count++;
	    }
	    ospf_info->oi_demand_circuit = TRUE;
	}
    } else {
	if (OSPF_DC_IF_TYPE(ospf_info)) {
	    if (nbr->nbr_suppress_hello) {
		nbr->nbr_suppress_hello = FALSE;
		ospf_info->oi_suppress_hello_count--;
	    }
	    ospf_info->oi_demand_circuit = 
		ospf_info->oi_cfg_demand_circuit;
	}
    }

    /* processing this packet depends on
     * neighbour state.
     */
    switch (nbr->nbr_state) {
      case NBR_EXSTART:
	if (IS_I_M_MS_SET(dbd->dbd_i_m_ms) && (ospf->ospf_length 
				       == (OSPF_HDR_SIZE + DBD_HDR_SIZE))) {
	    if (ospf->ospf_rtr_id > area->ar_pdb_ptr->ospf_rtr_id) {
		nbr->nbr_mode = NBR_ADJ_SLAVE;
		nbr->nbr_seq = dbd->dbd_seq;
		nbr->nbr_options = dbd->dbd_options;
		nbr->nbr_i_ms = 0;
		(*(nbr_state_machine[NBR_EVENT_NEGO_DONE][nbr->nbr_state]))
	                                      (idb, nbr);
	    } else {
		ospf_adj_buginf("\nOSPF: First DBD and we are not SLAVE");
		return;
	    }
	} else {
	    if ((IS_I_MS_OFF(dbd->dbd_i_m_ms))
		&& (dbd->dbd_seq == nbr->nbr_seq)
		&& (ospf->ospf_rtr_id < area->ar_pdb_ptr->ospf_rtr_id)){
		/* yes we are the master */
		nbr->nbr_options = dbd->dbd_options;
		nbr->nbr_i_ms = MS_BIT;
		(*(nbr_state_machine[NBR_EVENT_NEGO_DONE][nbr->nbr_state]))
	                                      (idb, nbr);
	    } else {
		ospf_adj_buginf("\nOSPF: Unrecognized dbd for EXSTART");
		return;
	    }
	}
	break;
      case NBR_EXCHANGE:
	if (((nbr->nbr_mode == NBR_ADJ_SLAVE) && (!(dbd->dbd_i_m_ms & MS_BIT)))
	    ||((nbr->nbr_mode == NBR_ADJ_MASTER)&&(dbd->dbd_i_m_ms & MS_BIT))){
	    /* inconsistency in MASTER and SLAVE relationship */
	    ospf_adj_buginf("\nOSPF: EXCHANGE - inconsistent in MASTER/SLAVE");
	    (*(nbr_state_machine[NBR_EVENT_SEQ_MISMATCH][nbr->nbr_state]))
	                                      (idb, nbr);
	    return;
	}
	/* if init bit is set generate seq mismatch */
	if (IS_I_BIT_SET(dbd->dbd_i_m_ms) || (nbr->nbr_options !=
						  dbd->dbd_options)) {
	    ospf_adj_buginf("\nOSPF: EXCHANGE - OPTIONS/INIT not match");
	    (*(nbr_state_machine[NBR_EVENT_SEQ_MISMATCH][nbr->nbr_state]))
		                        (idb, nbr);
	    return;
	}
	if (nbr->nbr_mode == NBR_ADJ_MASTER) {
	    if (dbd->dbd_seq == nbr->nbr_seq) {
		/* yes accept this packet and process it */
		break;
	    } else if (dbd->dbd_seq == (nbr->nbr_seq - 1))
		/* it is a duplicate discord this packet */
		return;
	} else {
	    /*
	     * we are a slave
	     */
	    if (dbd->dbd_seq == (nbr->nbr_seq + 1))
		/* accept this packet */
		break;
	    else if (dbd->dbd_seq == nbr->nbr_seq) {
		/*
		 * it is a duplicate packet and slave must
		 * resend its previous packet.
		 */
		ospf_send_dbd(idb, nbr);
		return;
	    }
	}
	/*
	 * if the processing reaches this point generate
	 * Seq Mismatch event.
	 */
	(*(nbr_state_machine[NBR_EVENT_SEQ_MISMATCH][nbr->nbr_state]))
	                                      (idb, nbr);
	return;
	break;
      case NBR_LOADING:
      case NBR_FULL:
	/* 
	 * The only packets we should receive are duplicates 
	 */
	if ((nbr->nbr_mode == NBR_ADJ_MASTER) &&
	    (dbd->dbd_seq == (nbr->nbr_seq - 1))) {
	    /* It is a duplicate.  Discard this packet. */
	    return;
	} else {
	    if ( ((nbr->nbr_mode == NBR_ADJ_SLAVE) 
                  || (nbr->nbr_mode == NBR_ADJ_SLAVE_HOLD)) 
                 && (dbd->dbd_seq == nbr->nbr_seq) ) {
		/*
		 * it is a duplicate packet and slave must
		 * resend its previous packet.
		 */
		if (nbr->nbr_dbd_list) {
		    ospf_send_dbd(idb, nbr);
		    return;
		}
	    }
	}
	/*
	 * if the processing reaches this point generate
	 * Seq Mismatch event.
	 */
	(*(nbr_state_machine[NBR_EVENT_SEQ_MISMATCH][nbr->nbr_state]))
	                                      (idb, nbr);
	return;
	break;
      default:
	errmsg(&msgsym(UNKNOWNSTATE, OSPF));
	return;
    } /* end of the switch */

    /* 
     * now we accepted the packet.
     * start processing the
     * packet.
     */
    lsa = dbd->dbd_lsas;
    length = (ospf->ospf_length - (OSPF_HDR_SIZE + DBD_HDR_SIZE));
    for (; length >= LINK_STATE_HEADER_SZ; 
	 lsa++, length -= LINK_STATE_HEADER_SZ) {
	if (INVALID_LSA_TYPE(lsa->lsh_type) || 
	    ((lsa->lsh_type == LSA_TYPE_ASE) && 
	     (area->ar_stub || area->ar_nssa)) ||
	    ((lsa->lsh_type == LSA_TYPE_7_ASE) && !area->ar_nssa) ||
	    ((lsa->lsh_type == LSA_TYPE_SUM_ASB) && 
	     (area->ar_stub || area->ar_nssa))) {

	    /* yes it is an invalid LSA */
	    (*(nbr_state_machine[NBR_EVENT_SEQ_MISMATCH][nbr->nbr_state]))
		                             (idb, nbr);
 	    errmsg(&msgsym(BADLSATYPE, OSPF), "dbd", lsa->lsh_type,
 		   lsa->lsh_id, lsa->lsh_adv_rtr,
 		   ip->srcadr, idb->namestring);
  	    return;
  	}
	if (db_lookup_lsa(area, lsa)) {
	    /* we have most recent copy */
#ifdef NOTDEF
 	    ospf_adj_buginf("\nOSPF: Received DBD from nbr %i and"
 			    " think that we have"
 			    " a more recent copy"
 			    "\n   LSID %i, type %d, adv. rtr. %i"
 			    ", seq %#10x, age %6d",
 			    nbr->nbr_id, lsa->lsh_id, lsa->lsh_type, 
 			    lsa->lsh_adv_rtr, lsa->lsh_seq, lsa->lsh_age);
#endif
	    continue;
        }
	/* we do not have a copy of it
	 * insert it in request list.
	 */
	ls_req_ptr = malloc(sizeof(ls_req_list));
	if (ls_req_ptr == NULL) {
	    return;
	}
	ls_req_ptr->req_age = lsa->lsh_age;
	ls_req_ptr->req_type = lsa->lsh_type;
	ls_req_ptr->req_ls_id = lsa->lsh_id;
	ls_req_ptr->req_adv_rtr = lsa->lsh_adv_rtr;
	ls_req_ptr->req_seq = lsa->lsh_seq;
	ls_req_ptr->req_chksum = lsa->lsh_chksum;
	ls_req_insert(nbr, ls_req_ptr, lsa->lsh_type);
    } /* end of processing dbd */

    /* decide if we need to send a dbd */
    if (nbr->nbr_mode == NBR_ADJ_MASTER) {
	mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
	nbr->nbr_seq++;
	tlist_ptr = nbr->nbr_dbd_list;
	if (tlist_ptr && (tlist_ptr->list_seq == (nbr->nbr_seq - 1))) {
	    nbr->nbr_dbd_list = tlist_ptr->list_next;
	    retbuffer(tlist_ptr->list_pak_ptr);
        }
	/* check if the dbd queue is empty */
	if (nbr->nbr_dbd_list == NULL) {
	    if (!(IS_M_BIT_SET(dbd->dbd_i_m_ms))) {
		/* this is the end of exchange */
		(*(nbr_state_machine[NBR_EVENT_EXCH_DONE][nbr->nbr_state]))
	                                 (idb, nbr);
		return;
	    }
	}
    } else {
	nbr->nbr_seq = dbd->dbd_seq;
	tlist_ptr = nbr->nbr_dbd_list;
	if (tlist_ptr && (tlist_ptr->list_seq == (nbr->nbr_seq - 1))
	    && (tlist_ptr->list_next)) {
	    nbr->nbr_dbd_list = tlist_ptr->list_next;
	    retbuffer(tlist_ptr->list_pak_ptr);
	}
	if ((IS_IT_LAST(nbr)) && (!(IS_M_BIT_SET(dbd->dbd_i_m_ms))))
	    /* this is the end of exchange */
	    (*(nbr_state_machine[NBR_EVENT_EXCH_DONE][nbr->nbr_state]))
	                                 (idb, nbr);
    }
    ospf_send_dbd(idb, nbr);
    /* Now check if we can send a req */
    if ((nbr->nbr_state > NBR_EXSTART) && (nbr->nbr_state < NBR_FULL) &&
	(!(NO_REQ(nbr))) && !mgd_timer_running(&nbr->nbr_poll_timer))
	ospf_send_req(idb, nbr);

} /* end of ospf_rcv_dbd */


/* end of ospf_adj.c */









