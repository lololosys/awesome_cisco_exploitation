/* $Id: rsvp_database.c,v 1.1.4.3 1996/08/20 10:24:23 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_database.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_database.c,v $
 * Revision 1.1.4.3  1996/08/20  10:24:23  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/07/19  06:09:25  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:16  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"
/***********************************************************
 * *
 * Process Variables *
 * *
 ***********************************************************/

wavl_handle rsvp_session_database;

/*
 * wavl_search
 *
 * Search the path state database for the passed goal. If no matching
 * node exists, return NULL. Goal is a dummy node which the caller
 * must allocate and salt with the desired search value.
 */
enum rsvp_search_result
rsvp_session_path_search (wavl_handle * handle,
			  psbtype ** result,
			  ipaddrtype source,
			  ushort udp_source_port)
{
    psbtype *goal;
    psbtype *wavl_return;

    /*
     * wavl lookup requires a goal of the same data type;
     * allocate one temporarily
     */
    goal = malloc(sizeof(psbtype));
    if(!goal) {
	*result = NULL;
	return(RSVP_NO_MEMORY);
    }
    /*
     * seed with search values
     */
    goal->template.source = source;
    goal->template.udp_source_port = udp_source_port;

    /*
     * search
     */
    wavl_return = 
	rsvp_wavl_to_psbtype(wavl_search(handle,
					 rsvp_psbtype_to_wavl(goal),
					 PSB_SESSION_SENDER_THREAD));
    if(wavl_return)
	goto goal_found;

    /*
     * seed with search imprecise values
     */

    if(udp_source_port) {
	goal->template.udp_source_port = 0;

	/*
	 * search
	 */
	wavl_return =
	    rsvp_wavl_to_psbtype(wavl_search(handle,
					     rsvp_psbtype_to_wavl(goal),
					     PSB_SESSION_SENDER_THREAD));
	if(wavl_return)
	    goto goal_found;
    }
    /*
     * return the goal data structure - we can initialize and use it
     */
    *result = goal;
    return(RSVP_NOT_FOUND);

 goal_found: /*
	      * toss the goal and return the real one
	      */
    free(goal);
    *result = wavl_return;
    return(RSVP_FOUND);
}

/*
 * initialize session head
 */
void rsvp_session_head_init(rsvp_session_head *head,
			    ipaddrtype destination,
			    uchar protocol,
			    ushort udp_dest_port) 
{
    rsvp_psb_init(&head->static_psb_database, rsvp_psb_compare);
    rsvp_psb_init(&head->static_rsb_database, rsvp_rsb_compare);
    rsvp_psb_init(&head->psb_database, rsvp_psb_compare);
    rsvp_rsb_init(&head->rsb_incoming_database, rsvp_rsb_compare);
    rsvp_rsb_init(&head->rsb_fixed_database, rsvp_rsb_compare);
    rsvp_rsb_init(&head->rsb_shared_database, rsvp_rsb_shared_compare);

    mgd_timer_init_parent(&head->master, &rsvp_root_timer);
    mgd_timer_init_leaf(&head->cleanup, &head->master,
			TIMER_TYPE_CLEANUP, head, FALSE);

    rsvp_build_session_ipv4(&head->session.base, 
			    destination, protocol, 0,  udp_dest_port);
}

/*
 * initialize session head
 */
void rsvp_session_done(rsvp_session_head *head)
{
    if (head->static_psb_count || head->static_rsb_count ||
	head->psb_count || head->rsb_incoming_count || 
	head->rsb_outbound_count)
	return;

    mgd_timer_stop(&head->master);

    wavl_finish(&head->static_psb_database);
    wavl_finish(&head->static_rsb_database);
    wavl_finish(&head->psb_database);
    wavl_finish(&head->rsb_incoming_database);
    wavl_finish(&head->rsb_fixed_database);
    wavl_finish(&head->rsb_shared_database);

    wavl_delete(&rsvp_session_database, head->threads);
    free(head);
}

/*
 * wavl_search
 *
 * Search the path state database for the passed goal. If no matching
 * node exists, return NULL. Goal is a dummy node which the caller
 * must allocate and salt with the desired search value.
 */
enum rsvp_search_result
rsvp_session_head_search (rsvp_session_head ** result,
			  ipaddrtype destination,
			  uchar protocol,
			  ushort udp_dest_port)
{
    rsvp_session_head *goal;
    rsvp_session_head *wavl_return;

    /*
     * wavl lookup requires a goal of the same data type;
     * allocate one temporarily
     */
    goal = malloc(sizeof(rsvp_session_head));
    if(!goal) {
	*result = NULL;
	return(RSVP_NO_MEMORY);
    }
    /*
     * seed with search values
     */
    rsvp_build_session_ipv4 (&goal->session, destination, protocol,
			     0, udp_dest_port);

    /*
     * search
     */
    wavl_return =
	(rsvp_session_head *) wavl_search(&rsvp_session_database,
					  goal->threads, SESSION_PORT_THREAD);

    if(wavl_return) {
	free(goal);
	*result = wavl_return;
	return(RSVP_FOUND);
    }
    else {
	*result = goal;
	return(RSVP_NOT_FOUND);
    }
}

/*
 * wavl_search
 *
 * Search the path state database for the passed goal. If no matching
 * node exists, return NULL. Goal is a dummy node which the caller
 * must allocate and salt with the desired search value.
 */
enum rsvp_search_result
rsvp_session_reservation_search (wavl_handle * handle,
				 rsbtype ** result,
				 idbtype * if_input,
				 ipaddrtype neighbor,
				 ulong style,
				 ipaddrtype source,
				 ushort udp_source_port)
{
    rsbtype *goal;
    rsbtype *wavl_return;

    /*
     * wavl lookup requires a goal of the same data type;
     * allocate one temporarily
     */
    goal = malloc(sizeof(rsbtype));
    if(!goal) {
	*result = NULL;
	return(RSVP_NO_MEMORY);
    }
    /*
     * seed with search values
     */
    goal->if_input = if_input;
    goal->hop.neighbor = neighbor;
    goal->style.reservation_type = style;
    if (style != RSVP_WF_OPTION) {
	goal->filter.source = source;
	goal->filter.udp_source_port = udp_source_port;
    }

    /*
       search
       */
    wavl_return =
	rsvp_wavl_to_rsbtype(wavl_search(handle,
					 rsvp_rsbtype_to_wavl(goal),
					 RSB_SESSION_RECEIVER_THREAD));

    if(wavl_return) {
	free(goal);
	*result = wavl_return;
	return(RSVP_FOUND);
    }
    else {
	*result = goal;
	return(RSVP_NOT_FOUND);
    }
}

/*
 * wavl_search
 *
 * Search the path state database for the passed goal. If no matching
 * node exists, return NULL. Goal is a dummy node which the caller
 * must allocate and salt with the desired search value.
 */
enum rsvp_search_result
rsvp_session_request_search (wavl_handle * handle,
			     rsbtype ** result,
			     idbtype * if_input,
			     ipaddrtype neighbor,
			     ulong style,
			     ipaddrtype source,
			     ushort udp_source_port)
{
    rsbtype *goal;
    rsbtype *wavl_return;

    /*
     * wavl lookup requires a goal of the same data type;
     * allocate one temporarily
     */
    goal = malloc(sizeof(rsbtype));
    if(!goal) {
	*result = NULL;
	return(RSVP_NO_MEMORY);
    }
    /*
     * seed with search values
     */
    goal->if_input = if_input;
    goal->hop.neighbor = neighbor;
    goal->style.reservation_type = style;
    if (style != RSVP_WF_OPTION) {
	goal->filter.source = source;
	goal->filter.udp_source_port = udp_source_port;
    }

    /*
     * search
     */
    wavl_return =
	rsvp_wavl_to_rsbtype(wavl_search(handle,
					 rsvp_rsbtype_to_wavl(goal),
					 RSB_SESSION_RECEIVER_THREAD));

    if(wavl_return) {
	free(goal);
	*result = wavl_return;
	return(RSVP_FOUND);
    }
    else {
	*result = goal;
	return(RSVP_NOT_FOUND);
    }
}

/***********************************************************
 * *
 * Path State Support Routines *
 * *
 ***********************************************************/

/*
 * Comparison routine for Session Header Blocks;
 */
enum avl_compare
rsvp_session_compare (avl_node_type * node1, avl_node_type * node2)
{
    rsvp_session_head *head1 =(rsvp_session_head *) node1;
    rsvp_session_head *head2 =(rsvp_session_head *) node2;

    if(head1->session.destination < head2->session.destination)
	return(AVL_LT);
    if(head2->session.destination < head1->session.destination)
	return(AVL_GT);
    if(head1->session.protocol < head2->session.protocol)
	return(AVL_LT);
    if(head2->session.protocol < head1->session.protocol)
	return(AVL_GT);
    if (head1->session.udp_dest_port && head2->session.udp_dest_port) {
	if(head1->session.udp_dest_port < head2->session.udp_dest_port)
	    return(AVL_LT);
	if(head2->session.udp_dest_port < head1->session.udp_dest_port)
	    return(AVL_GT);
    }
    return(AVL_EQ);
}

/*
 * Comparison routine for Path State Blocks;
 */
enum avl_compare
rsvp_psb_compare (avl_node_type * node1, avl_node_type * node2)
{
    psbtype *psb1 =(psbtype *) node1;
    psbtype *psb2 =(psbtype *) node2;

    if(psb1->template.source < psb2->template.source)
	return(AVL_LT);
    if(psb2->template.source < psb1->template.source)
	return(AVL_GT);

    if(psb1->template.udp_source_port < psb2->template.udp_source_port)
	return(AVL_LT);
    if(psb2->template.udp_source_port < psb1->template.udp_source_port)
	return(AVL_GT);
    return(AVL_EQ);
}

/***********************************************************
 * *
 * Reservation State Support Routines *
 * *
 ***********************************************************/

/*
 * Comparison routine for session-reciever thread in RSB
 */
enum avl_compare
rsvp_rsb_compare (avl_node_type * node1, avl_node_type * node2)
{
    rsbtype *rsb1 =(rsbtype *) node1;
    rsbtype *rsb2 =(rsbtype *) node2;

    if(rsb1->filter.source < rsb2->filter.source)
	return(AVL_LT);
    if(rsb2->filter.source < rsb1->filter.source)
	return(AVL_GT);
    if(rsb1->filter.udp_source_port < rsb2->filter.udp_source_port)
	return(AVL_LT);
    if(rsb2->filter.udp_source_port < rsb1->filter.udp_source_port)
	return(AVL_GT);

    if(rsb1->if_input && rsb2->if_input) {
	/*
	 *   allow for wild card match on a search
	 */
	if(rsb1->if_input->snmp_if_index < rsb2->if_input->snmp_if_index)
	    return(AVL_LT);
	if(rsb2->if_input->snmp_if_index < rsb1->if_input->snmp_if_index)
	    return(AVL_GT);
    }
    if(rsb1->hop.neighbor && rsb2->hop.neighbor) {
	/*
	 *  allow for wild card match on a search
	 */
	if(rsb1->hop.neighbor < rsb2->hop.neighbor)
	    return(AVL_LT);
	if(rsb2->hop.neighbor < rsb1->hop.neighbor)
	    return(AVL_GT);
    }
    return(AVL_EQ);
}

/*
 * Comparison routine for session-reciever thread in shared RSB
 */
enum avl_compare
rsvp_rsb_shared_compare (avl_node_type * node1, avl_node_type * node2)
{
    rsbtype *rsb1 =(rsbtype *) node1;
    rsbtype *rsb2 =(rsbtype *) node2;

    if(rsb2->filter.source) {
	if(rsb1->filter.source < rsb2->filter.source)
	    return(AVL_LT);
	if(rsb2->filter.source < rsb1->filter.source)
	    return(AVL_GT);
    }
    if(rsb2->filter.udp_source_port) {
	if(rsb1->filter.udp_source_port < rsb2->filter.udp_source_port)
	    return(AVL_LT);
	if(rsb2->filter.udp_source_port < rsb1->filter.udp_source_port)
	    return(AVL_GT);
    }

    if(rsb1->if_input && rsb2->if_input) {
	/*
	 * allow for wild card match on a search
	 */
	if(rsb1->if_input->snmp_if_index < rsb2->if_input->snmp_if_index)
	    return(AVL_LT);
	if(rsb2->if_input->snmp_if_index < rsb1->if_input->snmp_if_index)
	    return(AVL_GT);
    }
    if(rsb1->hop.neighbor && rsb2->hop.neighbor) {
	/*
	 *  allow for wild card match on a search
	 */
	if(rsb1->hop.neighbor < rsb2->hop.neighbor)
	    return(AVL_LT);
	if(rsb2->hop.neighbor < rsb1->hop.neighbor)
	    return(AVL_GT);
    }
    return(AVL_EQ);
}
