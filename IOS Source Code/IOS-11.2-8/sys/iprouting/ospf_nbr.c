/* $Id: ospf_nbr.c,v 3.6.8.9 1996/09/08 08:12:13 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_nbr.c,v $
 *------------------------------------------------------------------
 * ospf.c -- Open Shortest Path First Interior Gateway Routing Protocol
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_nbr.c,v $
 * Revision 3.6.8.9  1996/09/08  08:12:13  myeung
 * CSCdi64631:  Router crashes on sh ip os int after fiddling with ip os
 * priority
 * Branch: California_branch
 *
 * Revision 3.6.8.8  1996/08/28  12:53:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.8.7  1996/08/07  09:00:57  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.8.6  1996/08/03  09:35:19  myeung
 * CSCdi64856:  Network LSA not removed after loss of neighbor
 * Branch: California_branch
 *
 * Revision 3.6.8.5  1996/07/19  07:27:09  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.6.8.4  1996/07/18  08:36:13  myeung
 * CSCdi62058:  DC-feature : loses neighbor with non DC code
 * Branch: California_branch
 * - Add ospf_dc_down_check() to bring P2P DC interface
 *   state to IFS_DOWN when nbr is lost
 *
 * Revision 3.6.8.3  1996/07/09  19:12:57  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.6.8.2  1996/04/30  23:14:19  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.6.8.1  1996/04/16  19:01:56  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/29  10:33:27  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.5  1996/02/08  03:05:47  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.4  1996/01/22  06:14:35  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/05  10:17:44  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  17:36:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/02  18:21:22  myeung
 * CSCdi39931:  OSPF should retransmit LSA immediately after receiving ACK
 *
 * Revision 2.7  1995/09/19  20:54:04  myeung
 * CSCdi20080:  OSPF crashed when sending LSA in send_ls_update
 *
 * Revision 2.6  1995/09/06  22:53:36  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Resolve problem for boot image
 *
 * Revision 2.5  1995/09/05  22:09:13  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.4  1995/06/30  05:21:22  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.3  1995/06/28 03:21:36  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/19 01:08:14  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:09:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * routines exported from this module
 *
 * rem_nbr_retrans_list:  removes db from neighbor retrans list
 * lsa_nbr_retrans:       checks if a db in neighbor retrans list
 * add_db_to_nbr_retrans: adds a db to neighbor retrans list
 * free_dbd_sum:          frees neighbor database description packets
 * free_ls_req_list:      frees link state request
 * ospf_nbr_delete:       deletes a neighbor
 * ospf_find_nbr:         find a neighbor on an interface
 *
 */
#include "master.h"
#include "../os/old_timer_callbacks.h"
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
#include "../util/random.h"
#include "ospf_inlines.h"

/*
 * ospf_log_adj_chg
 *
 * If enabled, generate a syslog message on adjacency change.
 */
static void ospf_log_adj_chg (idbtype *idb, nbrtype *nbr, uchar from, 
			      uchar to, const char *msg)
{
    pdbtype *pdb = idb->ospf_info->area_ptr->ar_pdb_ptr;

    if(!pdb->ospf_pdb->log_adj_changes)
	return;

    errmsg(&msgsym(ADJCHG, OSPF),
	   pdb->asystem, nbr->nbr_id, idb->namestring, 
	   ospf_nbr_state_string(from), ospf_nbr_state_string(to), 
	   msg);
}

/*
 * rem_nbr_retrans_list
 * removes a particular element
 * or all the elements from neighbor
 * retransmission queue.
 *
 */

void rem_nbr_retrans_list (nbrtype *nbr, lsdbtype *db)
{
    lsdb_list	*list;
    lsdb_list	*temp;

    SAVE_CALLER();

    list = nbr->nbr_retrans_queue.q_first;
    /* if db is non NULL, delete only that
     * element. otherwise delete all elements
     * in neighbor retransmission queue 
     */
    if (db) {
	for (; (list && list->list_db_ptr != db); list = list->list_next);
	if (list) {
	    lsdb_dequeue(&(nbr->nbr_retrans_queue), list);
	    nbr->nbr_retrans_count--;
	}
	if (nbr->nbr_retrans_queue.q_first == NULL) {
	    /*
	     * if there are no elements in retransmission
	     * queue, stop the retransmission timer
	     */
	    ospf_retrans_buginf("\nOSPF: Stop timer (1) for Nbr %i "
				"after removing %i, type %d %#x", 
				nbr->nbr_id, LSA_ID(db), LSA_TYPE(db),
				caller());
	    mgd_timer_stop(&nbr->nbr_lsa_retrs_timer);
	}
    } else {
	/*
	 * remove all elements in retransmission
	 * queue.
	 */
	for (; list ; list = list->list_next) {
	    if (list->list_db_ptr) {
		rem_db_retrans_list(list->list_db_ptr, nbr);
	    }
	}
	list = nbr->nbr_retrans_queue.q_first;
	for (; list ; list = temp) {
	    temp = list->list_next;
	    list->list_next = NULL;
	    ospf_lsdb_list_free(list);
	}
	nbr->nbr_retrans_count = 0;
	nbr->nbr_retrans_queue.q_first = NULL;
	nbr->nbr_retrans_queue.q_last  = NULL;
	nbr->nbr_retrans_queue.count   = 0;
	ospf_retrans_buginf("\nOSPF: Stop timer (2) for Nbr %i %#x", 
			   nbr->nbr_id, caller());
	mgd_timer_stop(&nbr->nbr_lsa_retrs_timer);
    }

} /* end of rem_nbr_retrans */


/*
 * is_db_in_nbr_retrans
 */
boolean is_db_in_nbr_retrans (nbrtype *nbr, lsdbtype *db)
{
    nbr_list  *list;

    if ((db == NULL)
	|| (nbr == NULL))
	return(TRUE);
    for (list = db->db_retrans; list; list = list->list_next){
	if (list->list_nbr == nbr)
	    return(TRUE);
    }
    return(FALSE);
}/* is_db_in_nbr_retrans */


/*
 * lsa_nbr_retrans
 * checks if a Link state is in a
 * neighbor retransmission list
 */

boolean lsa_nbr_retrans (nbrtype *nbr, lsdbtype *db)
{
    lsdb_list	*list;
    
    list = nbr->nbr_retrans_queue.q_first;
    for (; (list && list->list_db_ptr != db); list = list->list_next);
    if (list)
	return(TRUE);
    return(FALSE);

} /* end of lsa_nbr_retrans */

/*
 * add_db_to_nbr_retrans
 * adds a link state to neighbor 
 * retransmission list and starts
 * retransmission timer if necessary
 */

boolean add_db_to_nbr_retrans (nbrtype *nbr, lsdbtype *db, idbtype *idb)
{
    lsdb_list	*d_list;
    nbr_list *n_list;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    SAVE_CALLER();

    if (!nbr || !db)
        ospf_retrans_buginf("\nOSPF: add_db_retrans - nbr %#x, db %#x"
			   ", caller %#x",
			   nbr, db, caller());

    if (is_db_in_nbr_retrans(nbr, db) || 
	((idb->ospf_info->if_type == IF_VIRTUAL_LINK) && 
	 (LSA_TYPE(db) == LSA_TYPE_ASE)))
        return FALSE;
        
    /* 
     * allocate list elements and 
     * initialize the fields to appropriate
     * values.
     */
    d_list = ospf_lsdb_list_malloc();
    if (d_list) {
	n_list = ospf_nbr_list_malloc();	
	if (n_list) {
	    d_list->list_db_ptr = db;
	    d_list->list_next = NULL;
	    d_list->list_prev = NULL;
	    d_list->list_flood = TRUE;
	    GET_TIMESTAMP(d_list->list_time);
	    if (nbr->nbr_retrans_queue.q_first == NULL) {
		ospf_retrans_buginf("\nOSPF: Start timer for Nbr %i after "
				    "adding %i type %d caller %#x",
				    nbr->nbr_id, LSA_ID(db), LSA_TYPE(db),
				    caller());
		mgd_timer_start(&nbr->nbr_lsa_retrs_timer, 
				ospf_info->retr_interval*ONESEC);
	    }
	    /*
	     * queue the list in the nbr queue 
	     */
	    lsdb_enqueue(&(nbr->nbr_retrans_queue), d_list);
	    nbr->nbr_retrans_count++;
	    /*
	     * queue the list in the db queue
	     */
	    n_list->list_nbr = nbr;
	    n_list->list_next = db->db_retrans;
	    db->db_retrans = n_list;
	} else {
	    ospf_lsdb_list_free(d_list);
	    return FALSE;
	}
    } else {
	return FALSE;
    }
    return TRUE;
    
} /* end of add_db_to_nbr_retrans */

/*
 * ospf_nbr_dead
 * generates kill nbr event on the
 * neighbor and deallocates it if 
 * possible. It is called through
 * dead timer.
 */
void ospf_nbr_dead (nbrtype *nbr, boolean force)
{
    idbtype *idb = nbr->nbr_idb;
    ospf_idb_info *ospf_info = idb->ospf_info;
    ospf_event(OC_TIMER_EXPIRED, (ulong) "nbr_dead", nbr->nbr_id);
    /*
     * nbr inactivate timer is not required on demand circuit
     */
    if (OSPF_DC_IF_TYPE(ospf_info) && !force && 
	(nbr->nbr_state >= NBR_LOADING) && nbr->nbr_suppress_hello) {
	ospf_adj_buginf("\nOSPF: Dead event ignored for %i on "
			"demand circuit %s", nbr->nbr_id, idb->namestring); 
	return;
    }
    ospf_adj_buginf("\nOSPF: %i address %i on %s is dead", nbr->nbr_id, 
		    nbr->nbr_ipaddr, idb->namestring);
    (*(nbr_state_machine[NBR_EVENT_KILL_NBR][nbr->nbr_state]))(idb, nbr);
    ospf_nbr_delete(idb, nbr, FALSE);
    ospf_dc_down_check(idb);
} /* end of ospf_nbr_dead */

/*
 * free_dbd_sum
 * frees neighbor database
 * description packets.
 * called to cleanup the neighbor.
 */

void free_dbd_sum (nbrtype *nbr)
{
    listype	*list_ptr;
    listype	*temp_ptr;

    for (list_ptr = nbr->nbr_dbd_list; list_ptr;list_ptr = temp_ptr) {
	temp_ptr = list_ptr->list_next;
	retbuffer(list_ptr->list_pak_ptr); 
    }
    nbr->nbr_dbd_list = NULL;

} /* end of free_dbd_sum */

/*
 * ospf_nbr_hold_dbd
 * holds database describtion summary
 * for a while bore freeing it.
 * called through hold timer.
 */
void ospf_nbr_hold_dbd (nbrtype *nbr)
{
    ospf_event(OC_TIMER_EXPIRED, (ulong) "nbr_hold_dbd", nbr->nbr_id);
    free_dbd_sum(nbr);

} /* end of ospf_nbr_hold_dbd */

/*
 * ospf_nbr_poll
 * sends hello to a NONBROADCAST-MULTIACCESS
 * neighbor at a reduced frquency or retransmits
 * a request to a neighbor
 * called through poll timer.
 */
void ospf_nbr_poll (nbrtype *nbr)
{
    idbtype *idb = nbr->nbr_idb;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "nbr_poll", nbr->nbr_id);
    /*
     * if the state of the neighbor is greater
     * than ATTEMPT, it is trying to form adjacency,
     * assume this timer event as request retransmission.
     */
    if (nbr->nbr_state > NBR_ATTEMPT) {
	ospf_adj_buginf("\nOSPF: Retransmitting request to %i on %s",
			nbr->nbr_id, idb->namestring);
	ospf_send_req(idb, nbr);
        return;
    }
    ospf_adj_buginf("\nOSPF: Sending poll to %i address %i on %s",
		    nbr->nbr_id, nbr->nbr_ipaddr, idb->namestring);
    ospf_send_hello(ospf_info->area_ptr, idb, nbr);
    mgd_timer_start(&nbr->nbr_poll_timer, nbr->nbr_poll_interval*ONESEC);

} /* end of ospf_nbr_poll */

/*
 * ospf_nbr_retrans_dbd
 * retransmits a database description
 * packet to a neighbor
 * called through dbd_retransmit timer.
 */
void ospf_nbr_retrans_dbd (nbrtype *nbr)
{
    idbtype *idb = nbr->nbr_idb;
    ospf_event(OC_TIMER_EXPIRED, (ulong) "retrans_dbd", nbr->nbr_id);
    ospf_adj_buginf("\nOSPF: Retransmitting DBD to %i on %s", 
		    nbr->nbr_id, idb->namestring);

    switch(nbr->nbr_state) {
      case NBR_EXSTART:
      case NBR_EXCHANGE:
	ospf_send_dbd(idb, nbr);
	break;
      default:
	break;
    }

} /* end of ospf_nbr_retrans_dbd */

/*
 * ospf_nbr_retrans_lsa
 * retransmits link states to 
 * a neighbor. Called through 
 * lsa_retransmit timer.
 */
void ospf_nbr_retrans_lsa (nbrtype *nbr)
{
    idbtype *idb = nbr->nbr_idb;
    ospf_idb_info *ospf_info = idb->ospf_info;
    lsdb_list *list;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "nbr_retrans_lsa", nbr->nbr_id);
    if (validmem(nbr)) {
	list = nbr->nbr_retrans_queue.q_first;
	if (ospf_info->oi_demand_circuit && 
	    (list->list_retrans_count >= 3) && 
	    !mgd_timer_running(&nbr->nbr_dead_timer)) {
	    /*
	     * A knob to kill dead nbr on demand circuit.
	     * It will be removed when the registry that inform
	     * OSPF about call failure is available.
	     */
	    ospf_adj_buginf("\nOSPF: Force to kill nbr %i on %s", nbr->nbr_id,
			    idb->namestring);
	    ospf_nbr_dead(nbr, TRUE);
	} else {
	    send_ls_update(nbr->nbr_retrans_queue.q_first, idb, nbr, TRUE);
	    mgd_timer_start(&nbr->nbr_lsa_retrs_timer, 
			    ospf_info->retr_interval*ONESEC);
	}
    } else {
	buginf("Invalid nbr pointer %x in lsa_retrans\n",nbr);
    }

} /* end of ospf_nbr_retrans_lsa */

/*
 * free_ls_req_list
 * frees either a request or all
 * requests from neighbor.
 */

void free_ls_req_list (
    idbtype	*idb,
    nbrtype	*nbr,
    ls_req_list	*list,
    uchar	lsa_type)
{
    uchar	type;
    ls_req_list	*temp;
    ls_req_list	*temp1;

    /*
     * if list is NULL, remove all request entries
     * from the neighbor or remove only the one requested.
     */
    if (list) {
	for (temp = nbr->nbr_ls_req[lsa_type]; (temp && temp != list);
	     temp = temp->req_next);
	if (temp) {
	    if (temp->req_prev == NULL) {
		/*
		 * it is the first element in
		 * the list.
		 */
		nbr->nbr_ls_req[lsa_type] = temp->req_next;
		if (temp->req_next)
		    temp->req_next->req_prev = NULL;
	    } else {
		if (temp->req_next == NULL) {
		    /*
		     * it is the last element
		     */
		    temp->req_prev->req_next = NULL;
		} else {
		    /*
		     * it is in the middle
		     */
		    temp->req_prev->req_next = temp->req_next;
		    temp->req_next->req_prev = temp->req_prev;
		}
	    }
	    free(temp);
	}
    } else {
	/*
	 * remove the complete list
	 */
	for (type = LSA_TYPE_RTR; type <= LSA_TYPE_ASE; type++) {
	    if (nbr->nbr_ls_req[type] != NULL) {
		for (temp = nbr->nbr_ls_req[type]; temp;) {
		    temp1 = temp->req_next;
		    free(temp);
		    temp = temp1;
		}
	    }
	    nbr->nbr_ls_req[type] = NULL;
	}
    }
    /*
     * If there are no requests in
     * in the neighbor request queue
     * stop the request retransmit timer.
     * if there is one, send it out.
     */
    if (NO_REQ(nbr))
	mgd_timer_stop(&nbr->nbr_poll_timer);
    else {
	if (REQ_TO_SEND(nbr))
	    ospf_send_req(idb, nbr);
    }
	    
} /* end of free_ls_req_list */

/*
 * ospf_delete_one_nbr
 * Clean up the nbr structure and free it.
 */
void ospf_delete_one_nbr (idbtype *idb, nbrtype *nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    free_ls_req_list(idb, nbr, NULL, 0);
    free_dbd_sum(nbr);
    mgd_timer_stop(&nbr->nbr_hello_master_timer);
    if (ospf_info->oi_old_dr == nbr)
	ospf_info->oi_old_dr = NULL;
    nbr->nbr_next = NULL;
    free(nbr);
}
/*
 * ospf_nbr_delete
 * deletes neighbor structure
 * if it is not manualy configured.
 */
void ospf_nbr_delete (idbtype *idb, nbrtype *nbr, boolean force)
{
    nbrtype *temp, *prev_nbr, *next_nbr;
    ospf_idb_info *ospf_info = idb->ospf_info;
  
    if (nbr) {
	if ((nbr->nbr_flags & NBRF_CONFIGURED) && (!force))
	    return;
	prev_nbr = NULL;
	for (temp = ospf_info->oi_nbr; temp; temp = temp->nbr_next) {
	    if (temp == nbr) {
		if (prev_nbr)
		    prev_nbr->nbr_next = temp->nbr_next;
		else
		    ospf_info->oi_nbr = temp->nbr_next;
		ospf_delete_one_nbr(idb, temp);
		break;
	    }
	    prev_nbr = temp;
	}
    } else {
	prev_nbr = NULL;
	for (temp = ospf_info->oi_nbr; temp ; temp = next_nbr) {
	    next_nbr = temp->nbr_next;
	    if ((temp->nbr_flags & NBRF_CONFIGURED) && !force) {
		prev_nbr = temp;
		continue;
	    }
	    if (prev_nbr)
		prev_nbr->nbr_next = next_nbr;
	    else
		ospf_info->oi_nbr = next_nbr;
	    ospf_delete_one_nbr(idb, temp);
	}
    }

} /* end of ospf_nbr_delete */

/*
 * ospf_create_one_nbr
 * Create a nbr single entry
 */
nbrtype *ospf_create_one_nbr (idbtype *idb, ulong nbr_rtr_id,
			      ipaddrtype nbr_addr)
{
    nbrtype *nbr;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if ((nbr = malloc( sizeof(nbrtype))) == NULL) {
	return(nbr);
    }
    nbr->nbr_id = nbr_rtr_id;
    nbr->nbr_ipaddr = nbr_addr;
    nbr->nbr_state = NBR_DOWN;
    nbr->nbr_pri = 0;
    nbr->nbr_ls_req[LSA_TYPE_RTR] = NULL;
    nbr->nbr_ls_req[LSA_TYPE_NET] = NULL;
    nbr->nbr_ls_req[LSA_TYPE_SUM_NET] = NULL;
    nbr->nbr_ls_req[LSA_TYPE_SUM_ASB] = NULL;
    nbr->nbr_ls_req[LSA_TYPE_ASE] = NULL;
    nbr->nbr_ls_req[LSA_TYPE_7_ASE] = NULL;
    nbr->nbr_poll_interval = POLL_INTERVAL;
    nbr->nbr_def_poll_interval = POLL_INTERVAL;
    nbr->nbr_dbd_list = NULL;
    nbr->nbr_retrans_queue.q_first = NULL;
    nbr->nbr_retrans_queue.q_last = NULL;
    nbr->nbr_retrans_count = 0;
    nbr->nbr_event_count = 0;
    nbr->nbr_flags |= NBRF_NEW;
    nbr->nbr_idb = idb;

    mgd_timer_init_parent(&nbr->nbr_hello_master_timer,
			  &ospf_info->oi_hello_master_timer);
    mgd_timer_init_leaf(&nbr->nbr_dead_timer,
			&nbr->nbr_hello_master_timer, 
			OSPF_NBR_DEAD_TIMER, nbr, FALSE);
    mgd_timer_init_leaf(&nbr->nbr_dbd_retrs_timer,
			&nbr->nbr_hello_master_timer,
			OSPF_NBR_DBD_RETRANS_TIMER, nbr, FALSE);
    mgd_timer_init_leaf(&nbr->nbr_dbd_hold_timer,
			&nbr->nbr_hello_master_timer,
			OSPF_NBR_DBD_HOLD_TIMER, nbr, FALSE);
    mgd_timer_init_leaf(&nbr->nbr_poll_timer,
			&nbr->nbr_hello_master_timer,
			OSPF_NBR_POLL_TIMER, nbr, FALSE);
    mgd_timer_init_leaf(&nbr->nbr_lsa_retrs_timer,
			&nbr->nbr_hello_master_timer,
			OSPF_NBR_LSA_RETRANS_TIMER, nbr, FALSE);
    return(nbr);
}


/*
 * ospf_find_nbr
 * find the neighbor structure.
 * If the interface is VIRTUAL link
 * ignore interface address.
 */
nbrtype *ospf_find_nbr (idbtype *idb, ulong nbr_rtr_id, 
			ipaddrtype nbr_addr, boolean create_nbr)
{
    nbrtype *nbr;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	if ((!nbr->nbr_id || !nbr_rtr_id || (nbr->nbr_id == nbr_rtr_id)) &&
	    (nbr->nbr_ipaddr == nbr_addr))
	    return(nbr);
	if (is_ospf_vl(idb->hwptr) && (nbr->nbr_id == nbr_rtr_id))
	    return(nbr);
    }

    if (!create_nbr)
	return(NULL);
    /* 
     * no neighbour present.
     * create one and return
     * it to caller
     */
    nbr = ospf_create_one_nbr(idb, nbr_rtr_id, nbr_addr);
    if (!nbr)
	return(NULL);
    /* 
     * insert it in neighbor list for this 
     * interface.
     */
    if (ospf_info->oi_nbr == NULL) {
	ospf_info->oi_nbr = nbr;
	nbr->nbr_next = NULL;
    } else {
	nbr->nbr_next = ospf_info->oi_nbr;
	ospf_info->oi_nbr = nbr;
    }
    return(nbr);

} /* end of ospf_find_nbr */	

/*
 * is_adj_ok
 * checks if it is ok to form adjacency
 * to neighbor.
 */

static boolean is_adj_ok (
    idbtype	*idb,
    nbrtype	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;
    /* check if we can for adjacency */
    if ((ospf_info->if_type == IF_VIRTUAL_LINK) || 
	(ospf_info->if_type == IF_POINT_TO_POINT) ||
	(ospf_info->if_type == IF_POINT_TO_M_POINT))
	/* yes we always form adjacency */
	return(TRUE);
    /* check if we are the designated router */
    if (ospf_info->dr 
	&& (ospf_info->dr->nbr_ipaddr == idb->ip_address))
	return(TRUE);
    /* check if we are backup designated router */
    if (ospf_info->bdr 
	&& (ospf_info->bdr->nbr_ipaddr == idb->ip_address))
	return(TRUE);
    /* check if the neighbour is the designated router */
    if (ospf_info->dr && (ospf_info->dr == nbr))
	return(TRUE);
    /* check if the neighbour is the backup designated router */
    if (ospf_info->bdr && (ospf_info->bdr == nbr))
	return(TRUE);
    return(FALSE);

} /* end of is_adj_ok */

/*
 * n_error:
 * Error routine in neighbour
 * state machine which indicates that
 * an invalid event occured.
 */

static void n_error (
    idbtype	*idb,
    nbrtype 	*nbr)
{

    ospf_event(OC_N_ERROR, nbr->nbr_id, nbr->nbr_state);
    nbr->nbr_event_count++;
    ospf_adj_buginf("\nOSPF: NBR state transition error: %d",nbr->nbr_state);

}/* end of n_error */

/*
 * n_hello - received a hello packet 
 */

static void n_hello (
    idbtype	*idb,
    nbrtype	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;
    uchar keyid;

    mgd_timer_start(&nbr->nbr_dead_timer, ospf_info->dead_interval*ONESEC);
    if (nbr->nbr_state < NBR_INIT) {
	ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_INIT, 
			 "Received Hello");
	nbr->nbr_state = NBR_INIT;
	nbr->nbr_event_count++;
	ospf_info->nbrcnt += 1;
	/*
	 * P2P demand circuit interface is up if and only if
	 * the neighbor is in 1 way or greater state.
	 */
	if ((ospf_info->if_state == IFS_DOWN) && 
	    (ospf_info->if_type == IF_POINT_TO_POINT) &&
	    ospf_info->oi_demand_circuit) {
	    ospf_info->if_state = IFS_POINT_TO_POINT;
	    mgd_timer_start(&ospf_info->oi_hello_timer,
			    ospf_info->hello_interval*ONESEC);
	}
	keyid = ospf_info->youngest_md_key ? 
	    ospf_info->youngest_md_key->id : 0;
	/*
	 * Neighbor startup with an old key, trigger rollover.
	 */
	if (nbr->nbr_message_digest_keyid != keyid)
	    ospf_info->old_md_nbr_count++;
    }
    if (nbr->nbr_state == NBR_INIT)
	mgd_timer_stop(&nbr->nbr_poll_timer);

}/* end of n_hello */

/*
 * n_start
 * Non-broadcast interface is up - current state NBR_DOWN
 * start inactivity timer and start sending hellos
 *
 * Note:
 * In our implementation, it is possible for us to generate 
 * START event on nbr which is already in ATTEMPT state.
 * It is because we use the START event to communicate our
 * priority to others.
 */

static void n_start (
    idbtype	*idb,
    nbrtype	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_START, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Starting %i address %i on %s", nbr->nbr_id,
		    nbr->nbr_ipaddr, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_ATTEMPT, "Start");
    nbr->nbr_state = NBR_ATTEMPT;
    nbr->nbr_event_count++;
    mgd_timer_start(&nbr->nbr_dead_timer, ospf_info->dead_interval*ONESEC);
    mgd_timer_stop(&nbr->nbr_poll_timer);

} /* end of n_start */

/*
 * n_2way
 * 2way connectivity is established between neighbors.
 * form the adjacency if necessary
 */

static void n_2way (
    idbtype	*idb,
    nbrtype	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_2WAY, nbr->nbr_id, nbr->nbr_state);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_2WAY, "2-Way Received");
    nbr->nbr_state = NBR_2WAY;
    nbr->nbr_event_count++;
    ospf_adj_buginf("\nOSPF: 2 Way Communication to %i on %s, state 2WAY", 
		    nbr->nbr_id, idb->namestring);
    /* 
     * a neighbour state changed to 2Way, generate IFE_NBRCHANGE
     * on the interface if this interfac needs to select designated
     * router. This event elects deg router if needed and brings up
     * adjucencies if necessary.
     */
    if ((ospf_info->if_type <= IF_NONBROADCAST)
	&& (ospf_info->if_state > IFS_POINT_TO_POINT))
	(*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))(idb, nbr);

    if (nbr->nbr_state == NBR_2WAY)
	(*(nbr_state_machine[NBR_EVENT_ADJ_OK][nbr->nbr_state]))(idb, nbr);
} /* end of n_2way */

/* n_rstad
 * reset an adjucency if it is in FULL state
 * and start it up again if needed.
 * This will be called if there is deg or bdeg
 * routers change or an area changes from transit
 * to stub area.
 */
static void n_rstad (
    idbtype	*idb,
    nbrtype	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_RSTAD, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Reset adjacency with %i on %s, state 2WAY", 
		    nbr->nbr_id, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_2WAY, "Reset Adjacency");

    /*
     * stop all timers except dead timer
     */
    mgd_timer_stop(&nbr->nbr_poll_timer);
    mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
    mgd_timer_stop(&nbr->nbr_dbd_hold_timer);
    
    /*
     * if the neighbor is trying to form adjacency
     * decrement neigbbor exchange count.
     */
    if ((nbr->nbr_state == NBR_EXCHANGE) || (nbr->nbr_state == NBR_LOADING)) {
	ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load--;
	ospf_info->oi_nbr_ex_load_count--;
    }
    /*
     * cleanup some variables in the 
     * structure and try to start the 
     * adjacency again.
     */
    if (nbr->nbr_state == NBR_FULL) 
	ospf_info->nbr_fcnt -= 1;
    nbr->nbr_state = NBR_2WAY;
    nbr->nbr_event_count++;
    nbr->nbr_mode = 0;
    nbr->nbr_seq = 0;
    free_dbd_sum(nbr); 
    rem_nbr_retrans_list(nbr, NULL);
    free_ls_req_list(idb, nbr, NULL, 0);
    (*(nbr_state_machine[NBR_EVENT_ADJ_OK][nbr->nbr_state]))
	(idb, nbr);

} /* end of n_rstad */

/*
 * n_adjok
 * check if it is ok to
 * form adjacency and start it
 * if necessary
 */
static void n_adjok (
    idbtype *idb,
    nbrtype *nbr)
{
    ospf_event(OC_N_ADJOK, nbr->nbr_id, nbr->nbr_state);

    if (nbr->nbr_state == NBR_2WAY) {
	if (is_adj_ok(idb, nbr)) {
	    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, 
			     NBR_EXSTART, "AdjOK?");
	    nbr->nbr_state = NBR_EXSTART;
	    /* set initial seq number */
	    /* if it is the first time we are trying to
	     * bring up adjuceny set the sequence number
	     * to msclock.
	     */
	    if (nbr->nbr_seq == 0)
		nbr->nbr_seq = random_gen();
	    nbr->nbr_i_ms = (I_BIT | MS_BIT);
	    nbr->nbr_mode = NBR_ADJ_MASTER;
	    ospf_send_dbd(idb, nbr);
	}
    } else if (nbr->nbr_state >= NBR_EXSTART) {
	if (!is_adj_ok(idb, nbr))
	    n_rstad(idb, nbr);
    }
} /* end of n_adjok */

/*
 * n_negdone
 * called after Negotiation of
 * Master/SLAVE relationship to
 * build database description.
 */

static void n_negdone (
    idbtype *idb,
    nbrtype *nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_NEGDONE, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: NBR Negotiation Done. We are the %s", 
		    (nbr->nbr_mode == NBR_ADJ_MASTER) ? "MASTER" : "SLAVE");
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_EXCHANGE,
		     "Negotiation Done");
    nbr->nbr_event_count++;
    nbr->nbr_state = NBR_EXCHANGE;
    ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load++;
    ospf_info->oi_nbr_ex_load_count++;
    /* stop sending db negotiation pkt
     * and start sending summary packets
     */
    mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
    build_dbd_sum(idb, nbr);
} /* end of n_negdone */

/* n_exdone
 * This routine is called through neighbor state machine. 
 * Called after comleting database exchange to
 * start loading the database.
 */

static void n_exdone (
    idbtype *idb,
    nbrtype *nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_EXDONE, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Exchange Done with %i on %s",
		    nbr->nbr_id, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_LOADING,
		     "Exchange Done");
    /* if master free last dbsum else leave it hanging around for a while */
    if (nbr->nbr_mode == NBR_ADJ_MASTER) {
	free_dbd_sum(nbr);
    } else  {
	/* we are in SLAVE mode */
	/* put nbr in SLAVE_HOLDING mode until timer has expired */
	nbr->nbr_mode = NBR_ADJ_SLAVE_HOLD;
	mgd_timer_start(&nbr->nbr_dbd_hold_timer, 
			ospf_info->dead_interval*ONESEC);
    }
    nbr->nbr_state = NBR_LOADING;
    nbr->nbr_event_count++;
    /* if there is no requests pending go to full state */
    if (NO_REQ(nbr))
	(*(nbr_state_machine[NBR_EVENT_LOAD_DONE][nbr->nbr_state]))
		                              (idb, nbr);
    else 
	/* start sending requests */
	if (!mgd_timer_running(&nbr->nbr_poll_timer))
	    ospf_send_req(idb, nbr);
} /* end of n_exdone */

 
/* n_badreq:
 * cleans the adj and start it allover again.
 * It is called through Iinterface state table
 * when an invalid request is received during adjuceny
 * bringing up process. Since This exchange can occure 
 * while DBD are being exchanged.
 */
static void n_badreq (
    idbtype *idb,
    nbrtype *nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;
    ospf_event(OC_N_BADREQ, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Bad request received from %i on %s",
		    nbr->nbr_id, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_EXSTART, "BadLSReq");
    /* we use poll timer for retransmitting the
     * request packet since ospf_retr_timer timer might
     * be used for retransmitting dbd packets.
     */
    mgd_timer_stop(&nbr->nbr_poll_timer);
    mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
    free_dbd_sum(nbr);	 	
    free_ls_req_list(idb, nbr, NULL, 0);	
    rem_nbr_retrans_list(nbr, NULL);	

    if (nbr->nbr_state == NBR_FULL) 
	ospf_info->nbr_fcnt -= 1;
    
    /* start it again */
    if ((nbr->nbr_state == NBR_EXCHANGE) || (nbr->nbr_state == NBR_LOADING)) {
	ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load--;
	ospf_info->oi_nbr_ex_load_count--;
    }
    nbr->nbr_state = NBR_EXSTART;
    nbr->nbr_event_count++;
    nbr->nbr_seq = random_gen();
    nbr->nbr_i_ms =  (I_BIT | MS_BIT);
    nbr->nbr_mode = NBR_ADJ_MASTER;
    ospf_send_dbd(idb, nbr);

} /* end of n_badreq */

/* n_badseq
 * This routine is called through neighbor state machine. 
 * Called when an unexpected sequence number
 * is received to restart adjacency.
 */

static void n_badseq (
    idbtype *idb,
    nbrtype *nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_BADSEQ, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Bad seq received from %i on %s", 
		    nbr->nbr_ipaddr, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_EXSTART,
		     "SeqNumberMismatch");
    mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
    mgd_timer_stop(&nbr->nbr_dbd_hold_timer);
    free_dbd_sum(nbr);		/* summary of area db */
    rem_nbr_retrans_list(nbr, NULL);
    free_ls_req_list(idb, nbr, NULL, 0);/* list of requests from this nbr */

    if (nbr->nbr_state == NBR_FULL) 
	ospf_info->nbr_fcnt -= 1;
    
    if ((nbr->nbr_state == NBR_EXCHANGE) || (nbr->nbr_state == NBR_LOADING)) {
	ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load--;
	ospf_info->oi_nbr_ex_load_count--;
    }
    nbr->nbr_event_count++;
    nbr->nbr_state = NBR_EXSTART;
    nbr->nbr_seq = random_gen();
    nbr->nbr_i_ms = (I_BIT | MS_BIT);
    nbr->nbr_mode = NBR_ADJ_MASTER;
    ospf_send_dbd(idb, nbr);

} /* n_badseq */

/* n_loaddone
 * This routine is called through neighbor state machine 
 * after a neighbor synchronizes its database. This routine
 * changes the neighbor state to FULL and reissues Router and
 * Net Links if necessary.
 */

static void n_loaddone (
    idbtype	*idb,
    nbrtype	*nbr)
{
    areatype *area;
    list_queue flood_queue;
    ushort change;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_LOADDONE, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Synchronized with %i on %s, state FULL",
		    nbr->nbr_id, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_FULL, "Loading Done");
    area = ospf_info->area_ptr;
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    change = 0;
    /* 
     * decrement the count of neighbors 
     * synchronizing the database at this time
     * and set the neighbor state to FULL.
     */
    area->ar_pdb_ptr->ospf_nbr_ex_load--;
    ospf_info->oi_nbr_ex_load_count--;
    nbr->nbr_event_count++;
    nbr->nbr_state = NBR_FULL;
    ospf_info->nbr_fcnt +=1;

    /*
     * Check to see if we need to reissue
     * router and net Link States.
     */
    if (build_rtr_lsa(area, &flood_queue, FALSE))
	change = SPF_SCHD_RTR;

    if (ospf_info->if_state == IFS_DR) {
	if (build_net_lsa(idb, &flood_queue, FALSE))
	    change |= SPF_SCHD_NET;
    }

    /* 
     * flood the link states if there are any
     */
    if (flood_area(area, &flood_queue, NULL, NULL)) { 
	/*
	 * schedule to run SPF if there is a 
	 * change in the topology
	 */
	spf(area, change, 0);
    }
} /* end of n_loaddone */

/*
 * n_1way 
 * can not see ourselfs in hello received from this
 * neighbour. we reset back to NBR_INIT. It will be
 * called in any state greater than NBR_INIT and it
 * might cause NBR_EVENT_NBRCHANGE event.
 */

static void n_1way (
    idbtype	*idb,
    nbrtype 	*nbr)
{
    areatype	*area;
    list_queue	flood_queue;
    ushort	change;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_N_1WAY, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_buginf("\nOSPF: Cannot see ourself in hello from %i on %s, "
		    "state INIT", nbr->nbr_id, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_INIT, "1-Way");
    /*
     * stop all timers except dead timer
     */
    mgd_timer_stop(&nbr->nbr_poll_timer);
    mgd_timer_stop(&nbr->nbr_dbd_retrs_timer);
    mgd_timer_stop(&nbr->nbr_dbd_hold_timer);

    /*
     * if the neighbor is trying to form adjacency
     * decrement neigbbor exchange count.
     */
    if ((nbr->nbr_state == NBR_EXCHANGE) || (nbr->nbr_state == NBR_LOADING)) {
	ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load--;
	ospf_info->oi_nbr_ex_load_count--;
    }

    /*
     * decrement number of adjucent nbr and reissue the
     * the router and net link states if necessary
     */
    if (nbr->nbr_state == NBR_FULL) {
	area = ospf_info->area_ptr;
	flood_queue.q_first = NULL;
	flood_queue.q_last = NULL;
	change = 0;
	ospf_info->nbr_fcnt -= 1;
	nbr->nbr_state = NBR_INIT;
	if (build_rtr_lsa(ospf_info->area_ptr,&flood_queue,FALSE))
	    change = SPF_SCHD_RTR;
	if (ospf_info->if_state == IFS_DR) {
	    if (build_net_lsa(idb, &flood_queue, FALSE))
		change |= SPF_SCHD_NET;
	}
	if (flood_area(area, &flood_queue, NULL, NULL)) 
	    spf(area, change, 0);
    }

    /*
     * clean some variables in structure
     */
    nbr->nbr_event_count++;
    nbr->nbr_state = NBR_INIT;
    nbr->nbr_mode = 0;
    nbr->nbr_seq = 0;
    nbr->nbr_dr = 0;
    nbr->nbr_bdr = 0;

    /*
     * free database describtion and
     * retransmission entries if necessary
     */
    free_dbd_sum(nbr);
    rem_nbr_retrans_list(nbr, NULL);
    free_ls_req_list(idb, nbr, NULL, 0); 

    /*
     * report neighbour change which will cause 
     * reelection of deg and bdeg routers. This event need to
     * be generated only on BROADCAST and NONBROADCAST
     * interfaces in valid state. 
     */
    if ((ospf_info->if_type <= IF_NONBROADCAST) 
	&& (ospf_info->if_state > IFS_POINT_TO_POINT))
	(*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))
		                             (idb, nbr);
} /* end of n_1way */

/*
 * n_down
 * Neighbor has gone away, clean
 * up the neighbor structure.
 */

static void n_down (
    idbtype	*idb,
    nbrtype 	*nbr)
{
    areatype	*area;
    list_queue	flood_queue;
    ospf_idb_info *ospf_info = idb->ospf_info;
    uchar keyid;

    ospf_event(OC_N_DOWN, nbr->nbr_id, nbr->nbr_state);
    ospf_adj_spec_buginf("\nOSPF: %i address %i on %s is dead, state DOWN", 
			 nbr->nbr_id, nbr->nbr_ipaddr, idb->namestring);
    ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_DOWN, "Neighbor Down");

    area = ospf_info->area_ptr;
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    /*
     * stop all neighbor timers
     */
    mgd_timer_stop(&nbr->nbr_hello_master_timer);
    
    /*
     * free dbd and retransmission
     * lists
     */
    free_dbd_sum(nbr);
    rem_nbr_retrans_list(nbr, NULL);
    free_ls_req_list(idb, nbr, NULL, 0);	  

    if ((nbr->nbr_state == NBR_EXCHANGE) || (nbr->nbr_state == NBR_LOADING)) {
	ospf_info->area_ptr->ar_pdb_ptr->ospf_nbr_ex_load--;
	ospf_info->oi_nbr_ex_load_count--;
    }

    /*
     * clean some variables in the structure
     */
    if (nbr != ospf_info->oi_myself) {
	if (nbr->nbr_state > NBR_ATTEMPT)
	    ospf_info->nbrcnt -= 1;
	if (nbr->nbr_state == NBR_FULL) 
	    ospf_info->nbr_fcnt -= 1;
	if (nbr->nbr_suppress_hello) {
	    nbr->nbr_suppress_hello = FALSE;
	    ospf_info->oi_suppress_hello_count--;
	}
    }
    nbr->nbr_mode = 0;
    nbr->nbr_seq = 0;
    nbr->nbr_message_digest_seq = 0;
    nbr->nbr_state = NBR_DOWN;
    if ((ospf_info->if_type == IF_POINT_TO_POINT) && 
	ospf_info->oi_demand_circuit) {
	mgd_timer_start(&ospf_info->oi_hello_timer,
			ospf_info->oi_poll_interval*ONESEC);
    }
    if (ospf_info->old_md_nbr_count) { 
        /*
	 * Remove down neighbor from rollover.
	 */
        keyid = ospf_info->youngest_md_key ? 
	  ospf_info->youngest_md_key->id : 0;
	if (nbr->nbr_message_digest_keyid != keyid)
            ospf_info->old_md_nbr_count--;
    }
    nbr->nbr_event_count++;
    nbr->nbr_dr = 0;
    nbr->nbr_bdr = 0;
    nbr->nbr_i_ms = 0;
    if (nbr->nbr_flags & NBRF_CONFIGURED)
	nbr->nbr_id = 0;
    /*
     * if this neighbor is on nonbroadcast and
     * multiaccess network and if it is manualy
     * configured, start the poll timer and start
     * sending hellos at reduced frequency.
     */
    if ((ospf_info->if_type == IF_NONBROADCAST) 
	&& (nbr->nbr_flags & NBRF_CONFIGURED)) {
	mgd_timer_start(&nbr->nbr_poll_timer, nbr->nbr_poll_interval*ONESEC);
	ospf_log_adj_chg(idb, nbr, nbr->nbr_state, NBR_ATTEMPT, "NBMA Start");
	nbr->nbr_state = NBR_ATTEMPT;
    }

    /*
     * Issue neighbor change event on this interface
     * for deg and bdeg router election if necessary.
     */
    if (ospf_info->if_type < IF_POINT_TO_POINT)
	(*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))(idb, nbr);

    build_rtr_lsa(ospf_info->area_ptr, &flood_queue, FALSE);
    /*
     * Need to rebuild network LSA to remove dead nbr
     */
    if (ospf_info->if_state == IFS_DR)
	build_net_lsa(idb, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of n_down */

/*
 * Neighbor finite State Machine table.
 */

void (*const nbr_state_machine[NBR_EVENTS][NBR_STATES])(idbtype *, nbrtype *) = {

/* DOWN  ATTEMPT     INIT     2WAY  EXSTART EXCH    LOAD    FULL    SCVL */ 

/* HELLO_RCV */ 
{n_hello, n_hello, n_hello, n_hello, n_hello,n_hello,n_hello,n_hello,n_hello},
/* START     */ 
{n_start, n_start, n_error, n_error, n_error,n_error,n_error,n_error,n_error},
/* 2WAY      */	
{n_error, n_error, n_2way,  n_error, n_error,n_error,n_error,n_error,n_error},
/* ADJ_OK    */
{n_error, n_error, n_error, n_adjok, n_adjok,n_adjok,n_adjok,n_adjok,n_error},
/* NEGO_DONE */
{n_error, n_error, n_error,n_error, n_negdone,n_error,n_error,n_error,n_error},
/* EXCH_DONE */
{n_error, n_error, n_error,n_error, n_error, n_exdone,n_error,n_error,n_error},
/* SEQ_MSMTCH*/
{n_error,n_error,n_error,n_badseq,n_badseq,n_badseq,n_badseq,n_badseq,n_badseq},
/* BAD_LSREQ */
{n_error,n_error,n_error,n_error, n_error, n_badreq,n_badreq,n_badreq,n_error},
/* LOAD_DONE */
{n_error,n_error,n_error,n_error, n_error, n_error,n_loaddone,n_error,n_error},
/* 1WAY      */	
{n_error, n_error, n_error, n_1way,  n_1way, n_1way, n_1way, n_1way, n_1way},
/* RST_ADJ   */
{n_error, n_error, n_error, n_error, n_rstad,n_rstad,n_rstad,n_rstad,n_rstad},
/* KILL_NBR  */	
{n_down,  n_down,  n_down,  n_down,  n_down, n_down, n_down, n_down, n_down},
/* INACT_TMR */	
{n_down,  n_down,  n_down,  n_down,  n_down, n_down, n_down, n_down, n_down},
/* LLDOWN    */	
{n_down,  n_down,  n_down,  n_down,  n_down, n_down, n_down, n_down, n_down},
};



/* end of ospf_nbr.c */
