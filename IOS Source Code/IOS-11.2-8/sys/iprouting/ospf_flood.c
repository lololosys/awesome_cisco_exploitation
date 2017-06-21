/* $Id: ospf_flood.c,v 3.5.8.8 1996/08/28 12:53:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_flood.c,v $
 *------------------------------------------------------------------
 * ospf_flood.c -- Supports OSPF flooding Mechanism
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_flood.c,v $
 * Revision 3.5.8.8  1996/08/28  12:53:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.8.7  1996/08/26  22:28:38  myeung
 * CSCdi63803:  2nd type7 lsa is generated and translated when interface
 * is shut
 * Branch: California_branch
 *
 * Revision 3.5.8.6  1996/08/23  21:36:39  myeung
 * CSCdi66427:  LSA refresh is sent across OSPF demand circuit
 * Branch: California_branch
 *
 * Revision 3.5.8.5  1996/06/28  06:04:16  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.5.8.4  1996/06/04  21:52:18  myeung
 * CSCdi58058:  ospf nssa - summary address with not-advertise does not
 * work
 * Branch: California_branch
 *
 * Revision 3.5.8.3  1996/05/04  01:21:56  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.5.8.2  1996/04/16  19:01:46  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.5.8.1  1996/03/18  20:28:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/16  06:57:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.1  1996/03/07  09:49:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/02/29  10:33:15  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.4  1996/01/05  10:17:36  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/18  18:48:13  myeung
 * CSCdi43168:  Clean up OSPF flooding
 * - Consolidate flooding functions
 *
 * Revision 3.2  1995/11/17  17:35:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/10/14  00:26:40  myeung
 * CSCdi41883:  OSPF LSA fragmentation crashes router
 * - Allow to send update containing huge LSA
 *
 * Revision 2.13  1995/10/12  11:50:55  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.12  1995/10/08  23:44:27  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.11  1995/10/08  22:49:39  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.10  1995/10/02  18:21:18  myeung
 * CSCdi39931:  OSPF should retransmit LSA immediately after receiving ACK
 *
 * Revision 2.9  1995/09/25  11:49:48  myeung
 * CSCdi40976:  OSPF should send back most recent copy when old lsa is
 * received
 *
 * Revision 2.8  1995/09/19  20:53:49  myeung
 * CSCdi20080:  OSPF crashed when sending LSA in send_ls_update
 *
 * Revision 2.7  1995/09/14  22:22:08  klmj
 * CSCdi40192:  Spurious memory access found in
 * ospf_flood_further_neighbor()
 * Fixed in area_flood().
 *
 * Revision 2.6  1995/09/07  23:30:03  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Back out last patch
 *
 * Revision 2.5  1995/09/07  22:41:14  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Minor nits
 *
 * Revision 2.4  1995/09/06  22:53:30  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Resolve problem for boot image
 *
 * Revision 2.3  1995/09/05  22:09:04  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.2  1995/06/13  17:16:48  aboonkon
 * CSCdi35774:  OSPF should accept Type 4 LSA received in a stub area
 *
 * Be more liberal in receiving.  It doesn't hurt to accept it, only cause
 * an extra LSA in the database.
 *
 * Revision 2.1  1995/06/07  21:09:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Routines exported from this mode
 * ospf_rcv_update:	receives an update packet takes the proper action
 */

#include "master.h"
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "access.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "ospf_inlines.h"


/*
 * External declarations
 */
extern boolean verify_iso_checksum(uchar *, int, uchar *);
/*
 * Forward declarations
 */

/*
 * Storage
 */
boolean ospf_flood_acl_pass;

static inline boolean acl_ospf_flood_debug (ulong lsid) {
    return (ospf_debug_flood && 
	    (!ospf_flood_acl || fast_check(ospf_flood_acl, lsid)));
}

/*
 * free_db_list
 * free list elements 
 */
void free_db_list (lsdb_list *db_list)
{
    lsdb_list *list;
    lsdb_list *temp;

    for (list = db_list; list ; list = temp) {
	temp = list->list_next;
	list->list_next = NULL;
	ospf_lsdb_list_free(list);
    }
} /* end of free_db_list */


/*
 * ospf_rcv_ack
 */
void ospf_rcv_ack (paktype *pak, ospftype *ospf, areatype *area)
{
    iphdrtype *ip;
    idbtype *idb;
    nbrtype *nbr;
    lsatype *lsa;
    lsdbtype *db;
    ushort length;
    pdbtype *pdb;
    boolean first_debug = TRUE;
    boolean fast_retrans = FALSE;
    lsdb_list *list;
    ospf_idb_info *ospf_info;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    nbr = ospf_find_nbr(idb, ospf->ospf_rtr_id, ip->srcadr, FALSE);
    if (nbr == NULL)
        return;
    pdb = area->ar_pdb_ptr;
    /* 
     * ignore this packet if neighbor is in any
     * state less than NBR_EXCHANGE
     */
     if (nbr->nbr_state < NBR_EXCHANGE) 
	 return;

    /* make sure the packet is not empty */
    if (ospf->ospf_length <= OSPF_HDR_SIZE)
	return;

    length = ospf->ospf_length - OSPF_HDR_SIZE;

    /* process the packet */
    lsa = (lsatype *)(ospf->ospf_data);

    while (length >= LINK_STATE_HEADER_SZ) {
	if (INVALID_LSA_TYPE(lsa->lsh_type)) {
	    /*
	     * this ack is invalid ignore it 
	     * and go for next one.
	     */
	    errmsg(&msgsym(BADLSATYPE, OSPF), "ack", lsa->lsh_type,
		   lsa->lsh_id, lsa->lsh_adv_rtr,
		   ip->srcadr, idb->namestring);
	    length -= LINK_STATE_HEADER_SZ;
	    lsa++;
	    continue;
	}
	db = db_find_lsa(area,
			 lsa->lsh_id,
			 lsa->lsh_adv_rtr,
			 lsa->lsh_type);
	
	ospf_flood_acl_pass = acl_ospf_flood_debug(lsa->lsh_id);
	if (first_debug && ospf_flood_acl_pass) {
	    ospf_flood_buginf("\nOSPF: Received ACK from %i",
			      ospf->ospf_rtr_id);
	    first_debug = FALSE;
	}
	ospf_flood_acl_buginf("\nOSPF: Rcv Ack Type %d, LSID %i, "
			      "Adv rtr %i, age %d, seq %#10x",
			      lsa->lsh_type, lsa->lsh_id,
			      lsa->lsh_adv_rtr, lsa->lsh_age,
			      lsa->lsh_seq);

	if ((db) && (is_db_in_nbr_retrans(nbr, db)) 
	    && SAME_INSTANCE(lsa,db->db_lsa_ptr, DB_ELAPSE(db)) ) {
	    ospf_flood_acl_buginf("\n      Remove LSA from "
				  "retransmission list");
	    /*
	     * This ACK does remove db from retransmission list, allow
	     * fast retransmission.
	     */
	    fast_retrans = TRUE;
	    rem_db_retrans_list(db, nbr);
	    rem_nbr_retrans_list(nbr, db);
	    if (db->db_in_maxageq) {
	        ospf_delete_maxage(pdb, db);
	        if (!db_free(pdb, db))
	            ospf_insert_maxage(pdb, db);
	    }
	} 
        length -= LINK_STATE_HEADER_SZ;
        lsa++;
    }
    ospf_info = idb->ospf_info;
    if (ospf_info && fast_retrans && 
	mgd_timer_running(&nbr->nbr_lsa_retrs_timer)) {
	list = nbr->nbr_retrans_queue.q_first;
	if (CLOCK_OUTSIDE_INTERVAL(list->list_time, 
				   ospf_info->retr_interval*ONESEC)) {
	    /*
	     * The retransmission interval already passed,
	     * so send retransmission once we get an ACK,
	     * it speeds up the retransmission process by
	     * more than 100%.
	     */
	    mgd_timer_start(&nbr->nbr_lsa_retrs_timer, 0);
	    nbr->nbr_fast_retrans = TRUE;
	}
    }
    ospf_flood_acl_pass = FALSE;
} /* end of ospf_rcv_ack */


/*
 * flush_req
 * free a request from neighbor
 * request list
 */
static void flush_req (
    idbtype *idb,
    nbrtype *nbr,
    lsatype *lsa)
{
    ls_req_list	*req_list;
    ls_req_list	*next_req;

    if (nbr->nbr_state == NBR_EXCHANGE || nbr->nbr_state==NBR_LOADING){
	/* check to see if any on this list are on request list */
	for (req_list = nbr->nbr_ls_req[lsa->lsh_type]; req_list;
	     req_list = next_req) { 
	    next_req = req_list->req_next;
	    if ((lsa->lsh_id == req_list->req_ls_id) &&
		(lsa->lsh_adv_rtr == req_list->req_adv_rtr)) {
		free_ls_req_list(idb,nbr,req_list, lsa->lsh_type);
	    }
	}
    }
} /* end of flush_req */


/*
 * ospf_validate_lsa
 *
 * Validate an LSA by type.  Returns TRUE if the LSA looks intact.
 */
static boolean ospf_validate_lsa(lsatype *lsa)
{
    rtr_lsatype *rtr_lsa;
    link_info *link;
    ulong linkcount;
    ulong remaining_length = lsa->lsh_length;

    /* Do type-specific validation. */

    switch (lsa->lsh_type) {
    case LSA_TYPE_RTR:

	/*
	 * Router LSA.  Walk the LSA, validating the various lengths
	 * and counts.  Yuck.
	 */
	if (remaining_length < RTR_LSA_HDR_SIZE) /* Too small? */
	    return(FALSE);

	remaining_length -= RTR_LSA_HDR_SIZE;
	rtr_lsa = (rtr_lsatype *) lsa;
	linkcount = rtr_lsa->la_lnk_cnt;
	link = &(rtr_lsa->la_link);
	while (linkcount) {

	    /* Enough space for this link? */

	    if (remaining_length < LINK_INFO_SIZE)
		return(FALSE);
	    remaining_length -= LINK_INFO_SIZE;

	    /* Enough space for this link's TOS info? */

	    if (remaining_length < LINK_METRIC_SIZE * link->lnk_tos_count)
		return(FALSE);
	    remaining_length -= LINK_METRIC_SIZE * link->lnk_tos_count;

	    linkcount--;
	    link = (link_info *) (((uchar *) link ) + LINK_INFO_SIZE +
				  (LINK_METRIC_SIZE *
				   link->lnk_tos_count));
	}
	break;

    case LSA_TYPE_NET:
    case LSA_TYPE_SUM_NET:
    case LSA_TYPE_SUM_ASB:
    case LSA_TYPE_ASE:
    case LSA_TYPE_7_ASE:
	break;

      default:
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ospf_rcv_update:
 * Receives The Link Update packet and floods it
 * if necessary.
 */
void ospf_rcv_update (paktype *pak, ospftype *ospf, areatype *area)
{
    iphdrtype *ip;
    idbtype *idb;
    nbrtype *nbr;
    lsatype *lsa;
    lsdbtype *db;
    list_queue nbr_ack_queue;
    ls_upd *ls_update;
    boolean ack_on_idb;
    list_queue flood_queue;
    list_queue src_flood_queue;
    list_queue ex_flood_queue;
    list_queue src_ex_flood_queue;
    list_queue nbr_update_queue;
    ushort adv_count;
    ls_req_list *req_list;
    ls_req_list *next_req;
    ushort age;
    ulong length;
    ospf_idb_info *ospf_info;
    boolean first_debug = TRUE;
    boolean do_not_age;

    ip = (iphdrtype *)ipheadstart(pak);

    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL; 
    src_flood_queue.q_first = NULL;
    src_flood_queue.q_last = NULL;
    ex_flood_queue.q_first = NULL;
    ex_flood_queue.q_last = NULL;
    src_ex_flood_queue.q_first = NULL;
    src_ex_flood_queue.q_last = NULL;
    nbr_ack_queue.q_first = NULL;
    nbr_ack_queue.q_last = NULL;
    nbr_update_queue.q_first = NULL;
    nbr_update_queue.q_last = NULL;

    idb = pak->if_input;
    ospf_info = idb->ospf_info;
    nbr = ospf_find_nbr(idb, ospf->ospf_rtr_id, ip->srcadr, FALSE);
    if (nbr == NULL)
        return;
    /* 
     * ignore this packet if neighbor is in any
     * state less than NBR_EXCHANGE
     */
     if (nbr->nbr_state < NBR_EXCHANGE) 
	 return;

    /* make sure the packet is not empty */
    if (ospf->ospf_length <= OSPF_HDR_SIZE)
	return;

    /* process the packet */

    ls_update = (ls_upd *)(ospf->ospf_data);
    lsa = ls_update->lsu_lsas;
    length = ospf->ospf_length - OSPF_HDR_SIZE;

    adv_count = ls_update->lsu_adv_count;

    /*
     * assume we will not 
     * flood on same interface
     */
    ack_on_idb = TRUE;
    if ((ospf_info->if_state == IFS_DR) && (ospf_info->bdr != nbr))
	/* 
	 * floods on the same interface
	 * needs no ack.
	 */
	ack_on_idb = FALSE;
 
    /*
     * Walk the LSAs in the LSU.  We track the count, the amount of
     * info left in the LSU, and the current LSA pointer.
     */
    for (; adv_count; adv_count--, length -= lsa->lsh_length, 
	 lsa = (lsatype *)(((uchar *)lsa) + lsa->lsh_length)) {

	/*
	 * Supposedly there is another advertisement there.  First see
	 * if there's enough memory for an LSA header.
	 */
	if (length < LINK_STATE_HEADER_SZ) {
	    errmsg(&msgsym(ERRRCV, OSPF), "Bad LSU adv count",
			 ip->srcadr);
	    return;
	}

	/*
	 * OK, the header is there.  Now see if the whole LSA is there.
	 */
	if (length < lsa->lsh_length) {
	    errmsg(&msgsym(ERRRCV, OSPF), "Bad LSU length",
			 ip->srcadr);
	    return;
	}

	/* It's all here.  Now validate the LSA. */

	if (!ospf_validate_lsa(lsa)) {
 	    errmsg(&msgsym(BADLSATYPE, OSPF), "Bad LSA data", 
 		   lsa->lsh_type, lsa->lsh_id, lsa->lsh_adv_rtr,
 		   ip->srcadr, idb->namestring);
	    return;
	}

	/* Burp out some info. */

	ospf_flood_acl_pass = acl_ospf_flood_debug(lsa->lsh_id);
	if (first_debug && ospf_flood_acl_pass) {
	    ospf_flood_buginf("\nOSPF: received update from %i, %s",
			      ospf->ospf_rtr_id, idb->namestring);
	    first_debug = FALSE;
	}
	/* Verify the checksum. */

	age = lsa->lsh_age;
	lsa->lsh_age = 0;
	/* 
	 * Store DoNotAge bit and clear DoNotAge bit of the LSA
	 */
	do_not_age = ((age & DO_NOT_AGE) == DO_NOT_AGE);
	age &= ~DO_NOT_AGE;

	ospf_flood_acl_buginf("\nOSPF: Rcv Update Type %d, LSID %i, "
			      "Adv rtr %i, age %d%s, seq %#10x",
			      lsa->lsh_type, lsa->lsh_id,
			      lsa->lsh_adv_rtr, age, 
			      do_not_age ? " (DNA)" : "",  
			      lsa->lsh_seq);
	if ( (lsa->lsh_type > LSA_TYPE_RTR) && 
	    (lsa->lsh_type <= LSA_TYPE_7_ASE) ) {
	    net_lsatype *net_lsa = (net_lsatype *) lsa;
	    ospf_flood_acl_buginf("\n      Mask%#m", net_lsa->la_mask);
	}

 	if (!verify_iso_checksum((uchar *)lsa, lsa->lsh_length,
				 (uchar *)&(lsa->lsh_chksum))) {
  	    /* this advertisement is invalid
  	     * ignore it and go for next one.
  	     */
 	    errmsg(&msgsym(BADLSATYPE, OSPF), "Bad LSA chksum", 
 		   lsa->lsh_type, lsa->lsh_id, lsa->lsh_adv_rtr,
 		   ip->srcadr, idb->namestring);
  	    continue;
  	}
  	lsa->lsh_age = age;

 	if (INVALID_LSA_TYPE(lsa->lsh_type) ||
 	    ((lsa->lsh_type == LSA_TYPE_ASE) && 
	     (area->ar_stub || area->ar_nssa)) ||
	    ((lsa->lsh_type == LSA_TYPE_7_ASE) && !area->ar_nssa) ||
	    ((lsa->lsh_type == LSA_TYPE_SUM_ASB) && 
	     (area->ar_stub || area->ar_nssa))) {
 	    errmsg(&msgsym(BADLSATYPE, OSPF), "Bad LSA type", 
 		   lsa->lsh_type, lsa->lsh_id, lsa->lsh_adv_rtr,
 		   ip->srcadr, idb->namestring);
 	    continue;
 	}

	/* Look up the LSA in the database. */

	db = db_find_lsa(area,
			 lsa->lsh_id,
			 lsa->lsh_adv_rtr,
			 lsa->lsh_type);
	
	/* If the LSA is maxaged and we don't know about it, Ack it. */

	if ((lsa->lsh_age >= MAXAGE) && (!db)) {
            /* 
             * if we're exchanging our respective databases, create an
             * entry in our database for this ancient lsa anyway.
             */
	    if (nbr->nbr_state == NBR_EXCHANGE || 
		nbr->nbr_state == NBR_LOADING) {
		db_install(&db,
			   lsa,
			   do_not_age,
			   area,
			   idb, 
			   nbr,
			   &src_flood_queue,
			   &flood_queue,
			   &ex_flood_queue,
			   &src_ex_flood_queue);
                /*
                 * if db is not valid at this point, we could be running out
                 * of memory and couldn't install it - go on to the next lsa
                 * in this packet and hope things get better soon.
                 * The neighbor will re-send this particular lsa because we
                 * didn't ack it, and we won't have removed it from our
                 * request list, either.
                 */
                if (!db) {
                    continue;
                }

		db->db_freeme = TRUE;
		ospf_insert_maxage(area->ar_pdb_ptr, db);
	    }
	    /*
  	     * acknowledge the receipt of the advertisement to nbr
	     * p:71-4
	     */ 
	    add_ack_list(&nbr_ack_queue, NULL, lsa);
	    /* 
	     * purge all outstanding requests for equal or previous 
	     * instances of the advertisement from sending neighbor's
	     * Link State Request List.
	     */
	    for (req_list = nbr->nbr_ls_req[lsa->lsh_type]; req_list;
		 req_list = next_req) {
                next_req = req_list->req_next;
		if ((lsa->lsh_id == req_list->req_ls_id) && 
		    (lsa->lsh_adv_rtr == req_list->req_adv_rtr)) {
		    if (!(REQ_MORE_RECENT(req_list, lsa)))
		       /* the lsa is either same instance or more recent */
		       free_ls_req_list(idb, nbr, req_list, lsa->lsh_type);
		}
	    }
	    continue;
	}

	/* New and not maxaged.  Flood it. */

	if ((!db)
	    || ((MORE_RECENT(lsa, db->db_lsa_ptr, DB_ELAPSE(db))))) {
	    if ((db) && (HOLD_TIMER_CHECK(db->db_time, MINLSINTERVAL))) {
		ospf_flood_acl_buginf("\nOSPF: Hold time check fail");
		continue;
	    }
	    rem_db_retrans_list(db, NULL);
	    if (ack_on_idb){
		/* 
		 * we will not flood on same interface
		 */
		if (ospf_info->if_state == IFS_BACKUP) {
		    if (ospf_info->dr == nbr)
			add_ack_list(&ospf_info->ack_queue, NULL, lsa);
		} else
		    add_ack_list(&ospf_info->ack_queue, NULL, lsa);
	    }
	    /* install new copy */;
	    db_install(&db,
		       lsa, 
		       do_not_age,
		       area,
		       idb, 
		       nbr,
		       &src_flood_queue,
		       &flood_queue,
		       &ex_flood_queue,
		       &src_ex_flood_queue);
	} else {

 	    /* Not new and not maxaged.  Look it over. */

	    for (req_list = nbr->nbr_ls_req[lsa->lsh_type]; req_list;
		 req_list = next_req) {
		next_req = req_list->req_next;
		if ((lsa->lsh_id == req_list->req_ls_id) && 
		    (lsa->lsh_adv_rtr == req_list->req_adv_rtr)) {
		    (*nbr_state_machine[NBR_EVENT_BAD_LS_REQ]
		     [nbr->nbr_state]) (idb, nbr);
		    ospf_flood_acl_buginf("\nOSPF: Error in exchange process");
		    goto abort_process;
		}
	    }

	    if (SAME_INSTANCE(lsa, db->db_lsa_ptr,DB_ELAPSE(db))) {
		/*
		 * This could be implied ack or a duplicate
		 * If it is a implied ack remove the retransmit
		 * entry from the retrans list. If it is a duplicate
		 * send a direct ack to neighbor.
		 */
		ospf_flood_acl_buginf("\nOSPF: Received same lsa");
		flush_req(idb, nbr, lsa);
		if (is_db_in_nbr_retrans(nbr, db)) {
		    /* we treat this as an implied ack */
		    ospf_flood_acl_buginf("\n      Remove LSA from "
					  "retransmission list");
		    rem_db_retrans_list(db, nbr);
		    rem_nbr_retrans_list(nbr, db);
		    /* 
		     * If the interface state is BACKUP, send
		     * Delayed Ack if lsa is received from Designated
		     * router.
		     */
		    if ((ospf_info->if_state == IFS_BACKUP) &&
			(ospf_info->dr == nbr))
			/* yes send a Delayed ack */
			add_ack_list(&ospf_info->ack_queue, NULL, lsa);
		} else
		    /* we treat it as a duplicate */
		    add_ack_list(&nbr_ack_queue, NULL, lsa);
	    } else {
		if ((ADV_AGE(db) >= MAXAGE) && 
		    (db->db_lsa_ptr->lsh_seq == OSPF_MAXSEQ)) {
		    /* 
		     * The sequence number is wrapping.
		     */
		    continue;
		}
		/*
		 * The database copy is more recent.
		 * Send a copy back to the neighbor.
		 */
		ospf_flood_acl_buginf("\nOSPF: Send more recent copy "
				      "lsid %i type %d seq %x back to %i",
				      LSA_ID(db),
				      LSA_TYPE(db),
				      db->db_lsa_ptr->lsh_seq,
				      lsa->lsh_adv_rtr);

		add_to_update_list(db, &nbr_update_queue, TRUE);

		ospf_flood_acl_buginf("\nOSPF: Received a old lsa "
				      "recv %x have %x "
				      "age %x have %x "
				      "chksum %x have %x",
				      lsa->lsh_seq, db->db_lsa_ptr->lsh_seq,
				      lsa->lsh_age, db->db_lsa_ptr->lsh_age,
				      lsa->lsh_chksum, 
				      db->db_lsa_ptr->lsh_chksum);
	    }
	}
    }

    if (nbr->nbr_state == NBR_LOADING && NO_REQ(nbr))
	(*(nbr_state_machine[NBR_EVENT_LOAD_DONE][nbr->nbr_state]))
	    (idb, nbr);
    /* now completed processing the advertisements.
     * Now flood the advertisements and send directed
     * acks if necessary. 
     */
  abort_process:
    flood_area(area, &src_flood_queue, NULL, NULL);
    ex_flood_area(area->ar_pdb_ptr, &src_ex_flood_queue, NULL, NULL);
    flood_area(area, &flood_queue, idb, nbr);
    ex_flood_area(area->ar_pdb_ptr, &ex_flood_queue, idb, nbr);
    if (nbr_ack_queue.q_first != NULL) {
	send_ack_list(&nbr_ack_queue, idb, nbr);
    }
    if (nbr_update_queue.q_first) {
	send_ls_update(nbr_update_queue.q_first, idb, nbr, FALSE);
	free_db_list(nbr_update_queue.q_first);
    }
    /* set delay ack timer, if it is not active */
    if (!mgd_timer_running(&ospf_info->oi_ack_timer) &&
	(ospf_info->ack_queue.q_first)) {
	mgd_timer_start(&ospf_info->oi_ack_timer,
			(ospf_info->retr_interval * ONESEC) >> 1);
    }
    ospf_flood_acl_pass = FALSE;
    return;
} /* end of ospf_rcv_update */	


/*
 * send_ls_update:
 * go through db_list and send ls update pkt
 */
void send_ls_update (
    lsdb_list *db_list,
    idbtype *idb,
    nbrtype *nbr,
    boolean retrans)
{
    paktype *pak;
    lsatype *lsa;
    lsdbtype *db;
    lsdb_list *list;
    ulong *adv_count_ptr;
    ushort length;
    lsatype *db_lsa;
    ipaddrtype	dest_addr;
    boolean do_not_age;
    ospf_idb_info *ospf_info = idb->ospf_info;
    boolean first_debug = TRUE;
    boolean fast_retrans = FALSE;

    if (db_list == NULL || ospf_info->if_state == IFS_DOWN)
	return;
    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE + MAX_OSPF_DATA);
    if (!pak)
	return;
    adv_count_ptr = (ulong *)OSPF_DATA_PTR(pak);
    *adv_count_ptr = 0;
    lsa = (lsatype *) (((uchar *)adv_count_ptr) + 4);
    /* 
     * ospf data should not exceed MAX_OSPF_DATA 
     * add size of the adv count field
     */
    length = 4;
    /* if a neighbor is specified, send it to him */
    if (nbr) {
	dest_addr = nbr->nbr_ipaddr;
	if (retrans) {
	    fast_retrans = nbr->nbr_fast_retrans;
	    nbr->nbr_fast_retrans = FALSE;
	}
    } else {
	/* decide on what destination to use in this packet */
	switch (ospf_info->if_type) {
	  case IF_BROADCAST:
	    /* if the interface state is IFS_DR or
	     * IFS_BACKUP, send this packet to ALLSPFROUTER
	     * otherwise send it to ALLSPFDROUTERS.
	     */
	    if ((ospf_info->if_state == IFS_DR) 
		|| (ospf_info->if_state == IFS_BACKUP))
		dest_addr = ALLSPFROUTERS;
	    else
		dest_addr = ALLSPFDROUTERS;
	    break;
	  case IF_POINT_TO_POINT:
	  case IF_POINT_TO_M_POINT:
	    dest_addr =  ALLSPFROUTERS;
	    break;
	  case IF_VIRTUAL_LINK:
	    /* send it to all Virtual neighbors */
	    dest_addr = ALL_VIRTUAL_NBRS;
	    break;
	  case IF_NONBROADCAST:
	    dest_addr = ALL_EXCH_NBRS;
	    break;
	  default:
	    return;
	}
    }
    for (list = db_list; list ; list = list->list_next) {
	/* check if we need to flood this adv */
	db = list->list_db_ptr;
	if (!(list->list_flood) || (db->db_lsa_ptr == NULL))
	    continue;
	    
	/*
	 * Don't flood external LSA on virtual links.
	 */
	if ((ospf_info->if_type == IF_VIRTUAL_LINK) && 
	    (LSA_TYPE(db) == LSA_TYPE_ASE))
	    continue;

	db_lsa = db->db_lsa_ptr;
	/* check if this lsa fits in this packet */
	if ((length + db_lsa->lsh_length) > MAX_OSPF_DATA) {
	    if (*adv_count_ptr) {
		ospf_send_packet(pak,
				 idb,
				 OSPF_LS_UPD,
				 length,
				 dest_addr,
				 FALSE);
		/* if we are retransmitting, just send one packet */
		if (retrans) {
		    ospf_flood_acl_pass = FALSE;
		    return;
		}
	    } else {
		retbuffer(pak);
	    }
	    /* allocate new packet */
	    if ((db_lsa->lsh_length + LS_UPD_SZ) > MAX_OSPF_DATA) {
		/*
		 * It is possible for RTR LSA get bigger than
		 * MAX_OSPF_DATA. In which case, get a bigger buffer
		 * that hold this huge LSA.
		 */
		pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE +
				db_lsa->lsh_length + LS_UPD_SZ);
	    } else {		
		pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE +
				MAX_OSPF_DATA);
	    }
	    if (!pak) {
		ospf_flood_acl_pass = FALSE;
		return;
	    }
	    adv_count_ptr = (ulong *)OSPF_DATA_PTR(pak);
	    *adv_count_ptr = 0;
	    lsa = (lsatype *) (((uchar *)adv_count_ptr) + LS_UPD_SZ);
	    length = LS_UPD_SZ; /* 4 for adv_cnt */
	    first_debug = TRUE;
	}
	if (retrans)
	    list->list_retrans_count++;
	/* Now plug in the adv in the packet */
	bcopy((uchar *)db_lsa, (uchar *)lsa, db_lsa->lsh_length);
	lsa->lsh_age = (db->db_do_not_age ? db_lsa->lsh_age : ADV_AGE(db)) + 
	    ospf_info->if_transdly;
	if (lsa->lsh_age > MAXAGE)
	    lsa->lsh_age = MAXAGE;

	do_not_age = FALSE;
	if ((ospf_info->oi_demand_circuit || db->db_do_not_age) &&
	    (lsa->lsh_age < MAXAGE) && 
	    !ospf_info->area_ptr->ar_dcbitless_lsa_count) {
	    do_not_age = TRUE;
	}
	    
	ospf_flood_acl_pass = acl_ospf_flood_debug(lsa->lsh_id);
	if (first_debug && ospf_flood_acl_pass) {
	    ospf_flood_buginf("\nOSPF: %s update on %s to %i",
			      retrans ? (fast_retrans ? 
					 "Fast retransmitting" : 
					 "Retransmitting") : "Sending",
			      idb->namestring, 
			      dest_addr);
	    first_debug = FALSE;
	}
	ospf_flood_acl_buginf("\nOSPF: Send Type %d, LSID %i, "
			      "Adv rtr %i, age %d%s, seq %#10x",
			      lsa->lsh_type, lsa->lsh_id,
			      lsa->lsh_adv_rtr, lsa->lsh_age,
			      do_not_age ? " (DNA)" : "",
			      lsa->lsh_seq);

	if (do_not_age)
	    lsa->lsh_age |= DO_NOT_AGE;
	GET_TIMESTAMP(list->list_time);
	length += lsa->lsh_length;
	*adv_count_ptr += 1;
	lsa = (lsatype *)((uchar *)lsa + lsa->lsh_length);
    }
    /* send the last packet if necessary */
    if (*adv_count_ptr) { 
	ospf_send_packet(pak,
			 idb,
			 OSPF_LS_UPD,
			 length,
			 dest_addr,
			 FALSE);
    } else
	retbuffer(pak);
    ospf_flood_acl_pass = FALSE;
} /* end of send_ls_update */
	    

/*
 * area_flood
 * flood into this area 
 */
static void area_flood (
    areatype 	*area,
    lsdb_list 	*flood_list,
    idbtype 	*src_idb,
    nbrtype 	*src_nbr)
{

    idbtype 	*idb;
    nbrtype 	*nbr;
    lsdb_list 	*list;
    lsatype	*lsa;
    ls_req_list	*req_list;
    ls_req_list	*next_req;
    boolean	floodit, req_match, req_flood, retrans, dc_no_flood;
    ospf_idb_info *ospf_info;

    for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next) {
        ospf_info = idb->ospf_info;
	if ((ospf_info->enabled == FALSE) || (!(ospf_interface_up(idb))))
	    continue;
 	if (!ospf_info->oi_nbr)
 	    continue;
        for (list = flood_list; list ;list = list->list_next)
            list->list_flood = FALSE;

	floodit = FALSE;
	for (list = flood_list; list ;list = list->list_next) {
	    lsa = list->list_db_ptr->db_lsa_ptr;
	    dc_no_flood = FALSE;
	    if (ospf_info->oi_demand_circuit &&
		!list->list_db_ptr->db_lsa_changed &&
		!area->ar_dcbitless_lsa_count) {
		ospf_flood_acl_pass = acl_ospf_flood_debug(lsa->lsh_id);
		ospf_flood_acl_buginf("\nOSPF: Ignore refresh Type %d, "
				      "LSID %i Adv rtr %i age %d%s, seq %#10x "
				      "for demand circuit %s", lsa->lsh_type, 
				      lsa->lsh_id, lsa->lsh_adv_rtr, 
				      lsa->lsh_age,
				      list->list_db_ptr->db_do_not_age ? 
				      " (DNA)" : "", lsa->lsh_seq, 
				      idb->namestring); 
		ospf_flood_acl_pass = FALSE;
		dc_no_flood = TRUE;
		if (!idb->ospf_info->oi_nbr_ex_load_count)
		    continue;
		/*
		 * We have exchange/load neighbor on this interface,
		 * so let it go through and check the request queue.
		 * Otherwise, we would unnecessarily restart the exchange
		 * process when we request our own (older) LSA again.
		 */
	    }
	    retrans = FALSE;
	    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
		/* 
		 * If the neighbors is in a lesser state than
		 * NBR_EXCHANGE, it does not participate in flooding.
		 */
		if (nbr->nbr_state < NBR_EXCHANGE)
		    continue;
		/*
		 * Assume no match on request queue and we don't have
		 * more recent copy.
		 */
		req_match = FALSE;
		req_flood = FALSE;
		if (nbr->nbr_state == NBR_EXCHANGE 
		    || nbr->nbr_state == NBR_LOADING) {
		    /* check to see if any on this list 
		     * are on request list 
		     */
		    for (req_list = nbr->nbr_ls_req[lsa->lsh_type];
			 req_list; req_list = next_req) { 
			next_req = req_list->req_next;
			if ((lsa->lsh_id == req_list->req_ls_id) &&
			    (lsa->lsh_adv_rtr == req_list->req_adv_rtr)) {
			    req_match = TRUE;
			    if (!REQ_MORE_RECENT(req_list, lsa)) {
				/*
				 * Nbr has same or least recent copy, 
				 * remove from list.
				 */
				free_ls_req_list(idb, nbr, req_list, 
						 lsa->lsh_type);
				if (!REQ_SAME_INSTANCE(req_list, lsa)) {
				    /* 
				     * More recent has come in - flood to nbr 
				     */
				    req_flood = TRUE;
				}
			    } 
			}	
		    }
		    if (nbr->nbr_state == NBR_LOADING && NO_REQ(nbr))
			(*(nbr_state_machine[NBR_EVENT_LOAD_DONE]
			   [nbr->nbr_state]))(idb, nbr);
		}
		/*
		 * It is DC interface and there is no change in LSA
		 */
		if (dc_no_flood)
		    continue;
		if (req_match && !req_flood) {
		    /*
		     * Nbr have same or more recent entry
		     */
		    continue;
		}
		if (nbr == src_nbr)
		    continue;
		/*
		 * At this point we are not positive that nbr
		 * has up-to-date LSA.
		 */
		add_db_to_nbr_retrans(nbr, list->list_db_ptr, idb);
		retrans = TRUE;
	    } /* end of nbr */
	    if (!retrans) {
		/*
		 * Not on retrans list, no need to flood.
		 */
		continue;
	    }
	    if ((ospf_info->if_type == IF_BROADCAST) 
		|| (ospf_info->if_type == IF_NONBROADCAST)) {
		if (idb == src_idb) {
		    /*
		     * If LSA received from this idb and
		     * it is from DR/BDR or we are BDR,
		     * no need to flood.
		     * Note that we verify if the neighbor has
		     * also declared itself as BDR even we think 
		     * it is. This is used to solve the problem
		     * in which the old DR has died and the neighbor 
		     * has not elect itself as BDR yet and so floods 
		     * its new router LSA to ALLSPFDROUTERS instead 
		     * of ALLSPFROUTERS.
		     * If we mistake our neighor as BDR and do not
		     * flood the new router LSA to other neighbors,
		     * the new router LSA will not reach other
		     * neighbors until retransmission. It can
		     * results in route loss if other neighbors
		     * happen to run SPF before receiving the
		     * retransmission.
		     */
		    if ((ospf_info->dr == src_nbr) ||
			((ospf_info->bdr == src_nbr) &&
			 (src_nbr->nbr_bdr == src_nbr->nbr_ipaddr)) ||
			(ospf_info->if_state == IFS_BACKUP))
		    continue;
		}
	    }
	    /*
	     * Flood this LSA.
	     */
	    list->list_flood = TRUE;
	    floodit = TRUE;
	} /* end of list */
	if (floodit) 
	    send_ls_update(flood_list, idb, NULL, FALSE);
    } /* end of idb */
} /* end of area_flood */


/* 
 * flood area: flood area and clean the list queue passed
 * return TRUE if we really do the flooding
 */
inline boolean flood_area (areatype *area, list_queue *flood_list,
			   idbtype *src_idb, nbrtype *src_nbr)
{
    if (flood_list->q_first) {
	area_flood(area, flood_list->q_first, src_idb, src_nbr);
	free_db_list(flood_list->q_first);
 	flood_list->q_first = NULL;
 	flood_list->q_last  = NULL;
 	return (TRUE);
    }
    return (FALSE);
} /* end of flood_area */


/*
 * ex_flood_area
 * floods externals to all areas except stub area
 */
boolean ex_flood_area (pdbtype *pdb, list_queue *flood_list, idbtype *src_idb,
		       nbrtype *src_nbr)
{
    areatype *ar;
      
    for (ar = pdb->ospf_area_list; ar;ar = ar->ar_next ) {
	if (ar->ar_stub)
	    continue;
	if (ar->ar_nssa) {
	    ospf_flood_buginf("\nSend NSSA update to area %s %x", 
			      ar->ar_id_str, ar->ar_nssa_flood_list.q_first);
	    /*
	     * We use per area flood list to handle NSSA because it
	     * is equally ugly to explicitly pass flood list 
	     * particular for NSSA as the number of flood list
	     * depend on the number of area which is a variable.
	     * With the upcoming OSPF packet pacing code, all flood list
	     * will be eliminated so this ugly piece of code will not stay
	     * long.
	     */
	    if (ar->ar_nssa_flood_list.q_first) {
		area_flood(ar, ar->ar_nssa_flood_list.q_first, src_idb, 
			   src_nbr);
		free_db_list(ar->ar_nssa_flood_list.q_first);
	    }
	    ar->ar_nssa_flood_list.q_first = NULL;
	    ar->ar_nssa_flood_list.q_last = NULL;
	    continue;
	}
	if (flood_list->q_first)
	    area_flood(ar, flood_list->q_first, src_idb, src_nbr);
    }
    if (flood_list->q_first) {
	free_db_list(flood_list->q_first);
	flood_list->q_first = NULL;
	flood_list->q_last = NULL;
	return (TRUE);
    }
    return (FALSE);
} /* end of ex_flood_area */

/*
 * send_ack_list:
 * Builds ack packet out of a list
 * sends it out to a neighbor or
 * to a multicast address.
 */
void send_ack_list(list_queue *ack_list_queue, idbtype *idb, nbrtype *nbr)
{
    paktype *pak;
    lsatype *lsa;
    lsatype *ack_lsa;
    ipaddrtype dest_addr;
    lsdb_list *list, *temp;
    ushort length;
    ushort ack_pak_count = 0;
    boolean rate_control = TRUE;
    ospf_idb_info *ospf_info = idb->ospf_info;
    boolean first_debug = TRUE;

    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE + MAX_OSPF_DATA);
    if (!pak)
	return;
    lsa = (lsatype *)OSPF_DATA_PTR(pak);

    if (nbr) {
	/* send unicast ack
	 * to this neighbor 
	 */
	dest_addr = nbr->nbr_ipaddr;
	rate_control = FALSE;
    } else {
	/* send ack on this interface
	 * Decide what type of address
	 * to use
	 */
	switch (ospf_info->if_type) {
	  case IF_BROADCAST:
	    if ((ospf_info->if_state == IFS_DR) 
		|| (ospf_info->if_state == IFS_BACKUP))
		dest_addr = ALLSPFROUTERS;
	    else
		dest_addr = ALLSPFDROUTERS;
	    break;
	  case IF_NONBROADCAST:
	    dest_addr = ALL_EXCH_NBRS;
	    break;
	  case IF_POINT_TO_POINT:
	  case IF_POINT_TO_M_POINT:
	    dest_addr = ALLSPFROUTERS;
	    break;
	  case IF_VIRTUAL_LINK:
	    dest_addr = ALL_VIRTUAL_NBRS;
	    break;
	  default:
	    return;
	}
    }

    /* 
     * ospf data should not exceed MAX_OSPF_DATE 
     */
    length = 0;

    for (list = ack_list_queue->q_first; list ; list = temp) {
	temp = list->list_next;
	if (list->list_db_ptr)
	    ack_lsa = list->list_db_ptr->db_lsa_ptr;
	else
	    ack_lsa = list->list_lsa_ptr;
	if (ack_lsa == NULL)
	    ospf_buginf("OSPF: Invalid ack list \n");

	/* check if this lsa fits in this packet */
	if ((length + LINK_STATE_HEADER_SZ) >= MAX_OSPF_DATA) {
	    ospf_send_packet(pak,
			     idb,
			     OSPF_LS_ACK,
			     length,
			     dest_addr,
			     FALSE);

	    ack_pak_count++;
	    if (rate_control && (ack_pak_count >= ACK_PACKET_LIMIT_NUM) ) {
		mgd_timer_start(&ospf_info->oi_ack_timer, 
				DELAYED_ACK_INTERVAL);
		ospf_flood_acl_pass = FALSE;
		return;
	    }

	    /* allocate new packet */
	    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE +
			    MAX_OSPF_DATA);
	    if (!pak) {
		ospf_flood_acl_pass = FALSE;
		return;
	    }
	    lsa = (lsatype *)OSPF_DATA_PTR(pak);
	    length = 0;
	    first_debug = TRUE;
	}
	bcopy((uchar *)ack_lsa, (uchar *)lsa, LINK_STATE_HEADER_SZ);
	ospf_flood_acl_pass = acl_ospf_flood_debug(ack_lsa->lsh_id);
	if (first_debug && ospf_flood_acl_pass) {
	    if (nbr) {
		ospf_flood_buginf("\nOSPF: Sending direct ACK on %s to %i", 
				  idb->namestring, nbr->nbr_id);
	    } else {
		ospf_flood_buginf("\nOSPF: Sending delayed ACK on %s", 
				  idb->namestring);
	    }
	    first_debug = FALSE;
	}
	ospf_flood_acl_buginf("\nOSPF: Ack Type %d, LSID %i, "
			      "Adv rtr %i, age %d, seq %#10x",
			      ack_lsa->lsh_type, ack_lsa->lsh_id,
			      ack_lsa->lsh_adv_rtr, ack_lsa->lsh_age,
			      ack_lsa->lsh_seq);


	length += LINK_STATE_HEADER_SZ;
	lsa = (lsatype *)((uchar *)lsa + LINK_STATE_HEADER_SZ);
	if (list->list_db_ptr)
	    list->list_db_ptr->db_ackcnt--;
	else
	    free(list->list_lsa_ptr);
	list->list_next = NULL;
	ospf_lsdb_list_free(list);
	ack_list_queue->q_first = temp;
	ack_list_queue->count--;
    }
    ospf_flood_acl_pass = FALSE;
    /* send the last packet if necessary */
    if (length) { 
	ospf_send_packet(pak,
			 idb,
			 OSPF_LS_ACK,
			 length,
			 dest_addr,
			 FALSE);
	ack_list_queue->q_first = NULL;
	ack_list_queue->q_last = NULL;	
    } else
	retbuffer(pak);

}/* end of send_ack_list */

/* end of ospf_flood.c */
