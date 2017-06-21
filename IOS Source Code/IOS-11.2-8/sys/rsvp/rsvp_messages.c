/* $Id: rsvp_messages.c,v 1.1.4.18 1996/08/24 01:16:44 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_messages.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_messages.c,v $
 * Revision 1.1.4.18  1996/08/24  01:16:44  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.4.17  1996/08/20  10:24:32  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.16  1996/08/16  17:21:04  asastry
 * CSCdi65504:  Make 'debug IP RSVP detail' command dump packet contents
 * in the event of an error condition.
 *
 * Branch: California_branch
 *
 * Revision 1.1.4.15  1996/08/09  00:18:14  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.14  1996/07/19  22:40:47  fred
 * CSCdi63487:  RSVP blocked reservation sent upstream
 *         reservation message created for zero bandwidth flow
 * Branch: California_branch
 *
 * Revision 1.1.4.13  1996/07/19  06:09:28  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.12  1996/07/04  01:12:26  fred
 * CSCdi61495:  Reservation requesting more than available bandwidth is
 *         installed. Divide responses from "rsvp_has_bitrate" into three
 *         cases: bandwidth is unavailable, it's available, or the request
 *         would reduce the bit rate, so another handler needs to think
 *         about that.
 * Branch: California_branch
 *
 * Revision 1.1.4.11  1996/06/22  10:39:26  fred
 * CSCdi58399:  Lockup on zero length message.
 *         In debug code, only display message after checking validity,
 *         to avoid zero length objects locking up display
 * Branch: California_branch
 *
 * Revision 1.1.4.10  1996/06/22  08:09:22  fred
 * CSCdi58184:  Reservation with bandwidth > flow max is incorrectly
 *         installed on first attempt. Correct analysis of first
 *         conversation's requirements.
 * Branch: California_branch
 *
 * Revision 1.1.4.9  1996/06/21  19:05:26  fred
 * CSCdi57507:  RSVP gets installed with Conversation number 0 on ethernet.
 *         Install registry callback from WFQ setup to RSVP to make sure
 *         that some reservable queues are allocated. Also, do not display
 *         weights and conversations unless WFQ invoked on interface
 * Branch: California_branch
 *
 * Revision 1.1.4.8  1996/06/18  11:05:45  fred
 * CSCdi53943:  box crashes when removing RSVP from an interface
 *              Add defensive code to avoid null dereferences, and
 *              improve handling of interface and topology changes
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/06/18  08:37:16  fred
 * CSCdi60599:  RSVP interaction with PIM Sparse Mode has problems.
 *              Implement special code required for sparse mode route
 *              creation
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/11  20:30:53  fred
 * CSCdi60143:  add interface to buginfs
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/11  16:15:39  fred
 * CSCdi60110:  draft 12 change - remove integrity object from messages
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/05  21:11:46  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/17  11:40:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.18.1  1996/05/05  23:45:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 1.1.4.2  1996/05/13  16:45:55  fred
 * CSCdi57500:  update rsvp from draft 8 to draft 11 objects
 *         This is done with an ifdef at this point as some EFT sites
 *         need draft 8 code; these will be removed later
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:22  fred
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
#include "subsys.h"
#include "../os/signal.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../ip/ip_registry.h"
#include "../if/priority_private.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"

static rsvp_refresh rsvp_default_refresh =
{
    {sizeof(rsvp_refresh), RSVP_CLASS_TIME_VALUES, 1},
    RSVP_REFRESH_MS
};

static rsvp_filter_ipv4 wild_card_filter =
{
    {sizeof(rsvp_filter_ipv4), RSVP_CLASS_FILTER_SPEC, RSVP_TYPE_IPV4},
    0, 0, 0
};

/*
 * merge flow specifications
 */
static boolean
rsvp_merge_flow_specs (rsvp_flowspec *new, rsvp_flowspec *old) 
{
    enum qos_service_type qos_service;
    rsvp_flowspec merge;

    qos_service = rsvp_get_flow_qos_service(old);
    if (QOS_GUARANTEED != qos_service)
	qos_service = rsvp_get_flow_qos_service(new);

    rsvp_build_flowspec(&merge,
			qos_service,
			max(rsvp_get_flow_maximum_burst(old),
			    rsvp_get_flow_maximum_burst(new)),
			max(rsvp_get_flow_average_bit_rate(old),
			    rsvp_get_flow_average_bit_rate(new)),
			min(rsvp_get_flow_maximum_e2e_delay(old),
			    rsvp_get_flow_maximum_e2e_delay(new)),
			min(rsvp_get_flow_service_level(old),
			    rsvp_get_flow_service_level(new)),
			min(rsvp_get_flow_min_unit(old),
			    rsvp_get_flow_min_unit(new)),
			max(rsvp_get_flow_max_unit(old),
			    rsvp_get_flow_max_unit(new)),
			max(rsvp_get_flow_peak(old),
			    rsvp_get_flow_peak(new)),
			max(rsvp_get_flow_requested_rate(old),
			    rsvp_get_flow_requested_rate(new)),
			min(rsvp_get_flow_slack(old),
			    rsvp_get_flow_slack(new)),
			min(rsvp_get_flow_composed_mtu(old),
			    rsvp_get_flow_composed_mtu(new)));

    if (bcmp(old, &merge, merge.base.length)) {
	memcpy(old, &merge, sizeof(merge));
	return TRUE;
    }
    return FALSE;
}

/*
 * calculate weights for guaranteed flows
 */
void rsvp_calculate_wfq_weights (idbtype * idb)
{
    rsvp_info_type	*rsvp_info = idb->rsvp_info;
    hwidbtype		*hwidb = hwidb_or_null(idb);
    ulong		 greatest_bandwidth = 0;
    rsvp_flow_ipv4	*flow;
    int			 index;

    /*
     * find the greatest bandwidth
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	rsvp_info = idb->rsvp_info;
	if (!rsvp_info)
	    continue;
	for (index = RSVP_FLOW_HASH; --index >= 0;) {
	    for (flow = rsvp_info->flows[index]; flow; flow = flow->next) {
		if (greatest_bandwidth < flow->bandwidth)
		    greatest_bandwidth = flow->bandwidth;
	    }
	}
    }

    /*
     * calculate the weights as (largest bandwidth*4)/bandwidth to
     * get a weight that gets each flow its relative bandwidth the
     * reason for multiplying by four is to get the weights for flows
     * close to the max bandwidth reasonable weights.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	rsvp_info = idb->rsvp_info;
	if (!rsvp_info)
	    continue;
	for (index = RSVP_FLOW_HASH; --index >= 0;) {
	    for (flow = rsvp_info->flows[index]; flow; flow = flow->next) {
		flow->weight = (4*greatest_bandwidth)/flow->bandwidth;
	    }
	}
    }
}

/*
 * rsvp_delete_conversation
 *
 * delete a conversation from an interface idb
 */
static void
rsvp_delete_conversation (idbtype * idb, rsvp_flow_ipv4 * conversation)
{
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    rsvp_flow_ipv4 *previous;

    if(!rsvp_info)
	return;

    previous = rsvp_info->flows[nethash(conversation->destination)];
    if(previous == conversation)
	rsvp_info->flows[nethash(conversation->destination)]
	    = conversation->next;
    else {
	while(previous)
	    if(conversation != previous->next)
		previous = previous->next;
	    else
		break;
	if(previous)
	    previous->next = conversation->next;
	else {
	    return;
	}
    }
    if(previous) {
	fair_release_conversation(hwidb_or_null(idb),
				   conversation->conversation);
	free(conversation);
    }
}

/*
 * rsvp_ip_conversation
 *
 * routine called in ip_determine_fair_queue_flow_id to classify packet
 */
ushort 
rsvp_ip_conversation (paktype * pak)
{
    iphdrtype *ip;
    ipaddrtype destination;
    ipaddrtype source;
    uchar ip_protocol;
    udptype *ports;
    ushort udp_source_port;
    ushort udp_dest_port;
    rsvp_info_type *rsvp_info;
    rsvp_flow_ipv4 *conversation;
    ulong time_elapsed;
    ushort n_intervals;

    ip = iphdrtype_start(pak);
    destination = GETLONG(&ip->dstadr);
    source = GETLONG(&ip->srcadr);
    ip_protocol = ip->prot;

    ports =(udptype *) ipdatastart_iph(ip);
    udp_source_port = ports->sport;
    udp_dest_port = ports->dport;

    rsvp_info = pak->if_output->rsvp_info;
    if(!rsvp_info)
	return(0);		/*
				 *  return "no reserved flow" 
				 */
    conversation = rsvp_info->flows[nethash(destination)];

    for(; conversation; conversation = conversation->next) {
	if(ip_protocol != conversation->ip_protocol)
	    continue;
	if(destination != conversation->destination)
	    continue;
	if(conversation->source && source != conversation->source)
	    continue;
	if(!conversation->udp_dest_port)
	    break;
	else if(udp_dest_port != conversation->udp_dest_port)
	    continue;
	else if(!conversation->udp_source_port)
	    break;
	else if(udp_source_port != conversation->udp_source_port)
	    continue;
	else
	    break;
    }
    if(!conversation)
	return(0);		/* return "it's best effort" */

    time_elapsed = ELAPSED_TIME(conversation->last_event);
    if (conversation->interval == 0)
	conversation->interval = 1;
    n_intervals = time_elapsed/conversation->interval;
    if (n_intervals) {
	TIMER_UPDATE(conversation->last_event,
		     conversation->interval * n_intervals);
	conversation->bytes_this_interval -=
	    conversation->bytes_per_interval * n_intervals;
	if (conversation->bytes_this_interval < 0)
	    conversation->bytes_this_interval = 0;
    }

    /*
     * allow accumulated slop of doubling traffic load in an interval
     * followed by a subsequent empty interval or several under-rate
     * intervals. But if the guy is too fast for very long, whack him
     * off at the knees.
     */
    if (2*conversation->bytes_per_interval < conversation->bytes_this_interval) {
	/*
	 * treat as best effort 
	 */
	pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    } else {
	/*
	 * treat better than that 
	 */
	conversation->bytes_this_interval += ip->tl;
	pak->fair_weight = conversation->weight;
    }
    return(conversation->conversation);	/* return conversation */
}

/*
 * rsvp_add_conversation
 *
 * put a new conversation on an interface
 */
static rsvp_flow_ipv4 *
rsvp_add_conversation (idbtype * idb, rsbtype * rsb)
{
    ipaddrtype destination = rsb->head->session.destination;
    ipaddrtype source = rsb->filter.source;
    ushort udp_dest_port = rsb->head->session.udp_dest_port;
    ushort udp_source_port = rsb->filter.udp_source_port;
    uchar ip_protocol = rsb->head->session.protocol;
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    rsvp_flow_ipv4 *conversation;
    rsvp_flow_ipv4 **head;
    ushort conversation_id;

    head = &rsvp_info->flows[nethash(destination)];
    conversation = *head;
    for(; conversation; conversation = conversation->next) {
	if(destination != conversation->destination)
	    continue;
	if(udp_dest_port != conversation->udp_dest_port)
	    continue;
	if(source != conversation->source)
	    continue;
	if(udp_source_port != conversation->udp_source_port)
	    continue;
	if(ip_protocol != conversation->ip_protocol)
	    continue;
	return(conversation);
    }
    conversation_id = fair_reserve_conversation(hwidb_or_null(idb));

    conversation = malloc(sizeof(rsvp_flow_ipv4));
    if(conversation) {
	conversation->destination = destination;
	conversation->udp_dest_port = udp_dest_port;
	conversation->source = source;
	conversation->udp_source_port = udp_source_port;
	conversation->ip_protocol = ip_protocol;
	conversation->bandwidth = 0;
	conversation->weight = FAIR_QUEUE_RSVP_WEIGHT;
	conversation->conversation = conversation_id;
	conversation->service_level =
	    rsvp_get_flow_service_level(&rsb->flowspec);
	conversation->qos_service = rsvp_get_flow_qos_service(&rsb->flowspec);
	GET_TIMESTAMP(conversation->last_event);
	conversation->next = *head;
	*head = conversation;
    }
    return(conversation);
}

/*
 * rsvp_largest_conversation
 *
 * find the largest matching conversation on an interface
 */
static rsvp_flow_ipv4 *
rsvp_largest_conversation (idbtype * idb, rsbtype * rsb)
{
    ipaddrtype destination = rsb->head->session.destination;
    ipaddrtype source = rsb->filter.source;
    ushort udp_dest_port = rsb->head->session.udp_dest_port;
    ushort udp_source_port = rsb->filter.udp_source_port;
    uchar ip_protocol = rsb->head->session.protocol;
    rsvp_flow_ipv4 *conversation;
    rsvp_flow_ipv4 *largest = NULL;

    conversation = idb->rsvp_info->flows[nethash(destination)];

    for(; conversation; conversation = conversation->next) {
	if(destination != conversation->destination)
	    continue;
	if(udp_dest_port != conversation->udp_dest_port)
	    continue;
	if(ip_protocol != conversation->ip_protocol)
	    continue;
	if(rsb->style.reservation_type & RSVP_SHARED) {
	    if(!largest ||
		    largest->bandwidth < conversation->bandwidth)
		largest = conversation;
	    continue;
	} else {
	    if(source != conversation->source)
		continue;
	    if(udp_source_port != conversation->udp_source_port)
		continue;
	    return(conversation);
	}
    }
    return(largest);
}

/*
 * rsvp_update_conversation
 *
 * change the bandwidth of a conversation
 */
static void
rsvp_update_conversation (rsbtype * rsb)
{
    idbtype *idb;
    rsvp_info_type *rsvp_info;
    rsvp_flow_ipv4 *conversation;
    int delta;
    ulonglong temp;

    idb = rsb->if_input;
    if(!idb)
	return;

    rsvp_info = idb->rsvp_info;
    if(!rsvp_info)
	return;
    conversation = rsvp_add_conversation(idb, rsb);
    if(!conversation) {
	return;
    }
    delta = rsvp_get_flow_requested_rate(&rsb->flowspec) - conversation->bandwidth;
    conversation->bandwidth += delta;
    if(!conversation->bandwidth)
	rsvp_delete_conversation(idb, conversation);
    else {
	conversation->qos_service = rsvp_get_flow_qos_service(&rsb->flowspec);
	conversation->service_level =
	    rsvp_get_flow_service_level(&rsb->flowspec);

	/*
	 * calculate interval in milliseconds
	 *
	 * use ulonglong temporary to enable large intermediate result,
	 * assuming 100 ms interval at megabit rates (and therefore fairly
	 * large numbers - > 4.0*10^9) normal.
	 *
	 * we are combining bits/second with bytes per burst to get
	 * milliseconds per interval.
	 */
	conversation->bytes_per_interval = temp =
	    rsvp_get_flow_maximum_burst(&rsb->flowspec)/8;
	temp = 1000 * 8*temp;
	conversation->interval = temp/conversation->bandwidth;
	if (!conversation->interval)
	    conversation->interval = 1;
    }
    rsvp_info->allocated_bitrate += delta;

    if(is_subinterface(idb) && hwidb_or_null(idb)) {
	idb = firstsw_or_null(hwidb_or_null(idb));
	if(idb) {
	    rsvp_info = idb->rsvp_info;
	    if (rsvp_info)
		idb->rsvp_info->allocated_bitrate += delta;
	}
    }
}

/*
 * clear reservations
 */

/*
 * routine is applied to all path state blocks to delete them
 */
static boolean
rsvp_flush_psb (wavl_node_type * node, va_list argptr)
{
    psbtype *psb =(psbtype *) node;

    TIMER_START(psb->cleanup_timer, 0);
    rsvp_psb_expiry(psb->threads, NULL);
    return(TRUE);
}

/*
 * routine is applied to all reservation state blocks to delete them
 */
static boolean
rsvp_flush_rsb_incoming (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;

    TIMER_START(rsb->cleanup_timer, 0);
    rsvp_rsb_expiry(rsb->threads, NULL);
    return(TRUE);
}

/*
 * routine is applied to all reservation state blocks to delete them
 */
static wavl_handle *rsvp_flush_rsb_request_handle;
static boolean
rsvp_flush_rsb_request (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    wavl_handle *handle;

    handle = rsvp_flush_rsb_request_handle;

    rsvp_stop_resv_refresh(rsb);
    rsvp_rsb_delete(handle, rsb);
    rsb->head->rsb_outbound_count--;
    free(rsb);
    return(TRUE);
}

/*
 * clear all reservation state
 */
static boolean
rsvp_flush_session (wavl_node_type * node, va_list argptr)
{
    rsvp_session_head *head =(rsvp_session_head *) node;

    mgd_timer_stop(&head->master);	/*
					 * stop all timers 
					 */

    wavl_walk(&head->static_psb_database, 0, rsvp_flush_psb);
    wavl_walk(&head->psb_database, 0, rsvp_flush_psb);
    wavl_walk(&head->rsb_incoming_database, 0, rsvp_flush_rsb_incoming);
    rsvp_flush_rsb_request_handle = &head->rsb_fixed_database;
    wavl_walk(&head->rsb_fixed_database, 0, rsvp_flush_rsb_request);
    rsvp_flush_rsb_request_handle = &head->rsb_shared_database;
    wavl_walk(&head->rsb_shared_database, 0, rsvp_flush_rsb_request);
    wavl_delete(&rsvp_session_database, head->threads);
    free(head);
    return TRUE;
}

void rsvp_clear_reservations (void)
{
    idbtype *idb;
    rsvp_info_type *rsvp_info;

    wavl_walk(&rsvp_session_database, 0, rsvp_flush_session);

    FOR_ALL_SWIDBS(idb) {
	rsvp_info = idb->rsvp_info;
	if(rsvp_info) {
	    rsvp_shutdown_interface(idb);
	}
    }
}

/*
 * shut down an RSVP interface
 */
void
rsvp_shutdown_interface (idbtype * idb)
{
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    rsvp_neighbor_type *nbr;
    rsvp_neighbor_type *next_nbr;
    rsvp_flow_ipv4 *flow;
    rsvp_flow_ipv4 *next_flow;
    int i;

    if(rsvp_info) {
	idb->rsvp_info = NULL;
	for(nbr = rsvp_info->rsvp_nbr; nbr; nbr = next_nbr) {
	    next_nbr = nbr->next;
	    free(nbr);
	}
	for(i = 0; i < RSVP_FLOW_HASH; i++) {
	    for(flow = rsvp_info->flows[i]; flow; flow = next_flow) {
		next_flow = flow->next;
		fair_release_conversation(hwidb_or_null(idb),
					   flow->conversation);
		free(flow);
	    }
	}
	free(rsvp_info);
	rsvp_running--;
    }
}

/*
 * There has been a change in an interface. Clear all reservations
 * and allow them to be reinstalled.
 */
static hwidbtype *rsvp_expire_interface;
static boolean
rsvp_expire_hwidb_resv (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;

    if(rsb->if_input && hwidb_or_null(rsb->if_input) == rsvp_expire_interface) {
	TIMER_START(rsb->cleanup_timer, 0);
	(void) rsvp_rsb_expiry(rsb->threads, argptr);
    }
    return TRUE;
}

static boolean
rsvp_expire_hwidb (wavl_node_type * node, va_list argptr)
{
    rsvp_session_head *head =(rsvp_session_head *) node;

    wavl_walk(&head->rsb_incoming_database, 0, rsvp_expire_hwidb_resv);
    return TRUE;
}

void 
rsvp_modify_interface_reservations (hwidbtype * hwidb)
{
    rsvp_expire_interface = hwidb;
    wavl_walk(&rsvp_session_database, 0, rsvp_expire_hwidb);
}


static psbtype *path_propagate_psb;

static void 
rsvp_path_sparse_install (void)
{
    psbtype *psb = path_propagate_psb;
    rsvp_refresh time_values;
    int minimum_interval;
    int rate_control;
    int maximum_interval;

    /*
     * in sparse mode, if there is no route, and we are colocated
     * with the sender, try to create one
     */
    if (samecable(psb->template.source) == psb->if_input) {
	minimum_interval = RSVP_REFRESH_MS;
	rate_control = (12*psb->time_values.refresh_ms)/10;
	maximum_interval = 1000*psb->head->psb_count;

	rsvp_build_refresh(&time_values,
			   max(minimum_interval,
			       min(rate_control, maximum_interval)));

	/*
	 * Cause the packet to be looped back and forwarded using IP multicast
	 * routing. This is the case where we have to encapsulate a PATH
	 * message inside a PIM Register packet so we can get the PATH
	 * message to the RP. The Router-Alert option will not be inserted.
	 * This PATH message travels down the tree to setup multicast state
	 * without RSVP seeing it. After so many tries, then PATH messages
	 * will start being sent with the Router-Alert option. 
	 *
	 * This allows PATH messages to travel on a already setup multicast
	 * sparse-mode path.
	 */
	rsvp_send_path(psb->if_input, psb->ttl, &psb->head->session,
		       &time_values, &psb->template, 
		       &psb->traffic_specification, &psb->policy,
		       &psb->advertisement, FALSE);
    }
}

/*
 * rsvp_path_propagate/rsvp_refresh_path
 *
 * timer driven routine - refresh downstream path state.
 *
 * triggered updates are done by triggering the routine to do an update
 */

static void 
rsvp_path_propagate (idbtype * idb)
{
    psbtype *psb = path_propagate_psb;
    rsvp_refresh time_values;
    int minimum_interval;
    int rate_control;
    int maximum_interval;

    if(idb && idb->rsvp_info) {
	minimum_interval = RSVP_REFRESH_MS;
	rate_control = (12*psb->time_values.refresh_ms)/10;
	maximum_interval = 1000*psb->head->psb_count;

	rsvp_build_refresh(&time_values,
			   max(minimum_interval,
			       min(rate_control, maximum_interval)));

	rsvp_send_path(idb, psb->ttl, &psb->head->session,
		       &time_values, &psb->template, 
		       &psb->traffic_specification, &psb->policy,
		       &psb->advertisement, TRUE);
    }
}

/*	
 * rsvp_populate_if_output
 *
 * Go look to see if there are any new outgoing interfaces that have 
 * been added to the IP multicast outgoing interface list.
 *
 * This function assumes that psb->if_output.count is 0. Otherwise, duplicate
 * interfaces will occur in list.
 */
static void rsvp_populate_if_output (psbtype *psb)
{
    queuetype  *queue;
    idbtype    *idb;
    ipaddrtype next_hop;

    if (!rsvp_downstream_lookup(psb->template.source, 
				psb->head->session.destination,
				&queue, &idb, &next_hop)) return;

    if (idb) {
	/* unicast case */
	data_enqueue(&psb->if_output, idb);
	return;
    }

    /* multicast case */
    while (rsvp_get_downstream_interface(queue, idb, &idb, &next_hop, 255)) {
	if (!idb) break;
	data_enqueue(&psb->if_output, idb);
    }
}

void
rsvp_refresh_path (psbtype * psb)
{

    if(TIMER_RUNNING_AND_AWAKE(psb->cleanup_timer))
	return;
    if(!psb->if_input->rsvp_info)
	return;
    if (!interface_up_simple(psb->if_input)) {
	rsvp_psb_expiry(psb->threads, NULL);
	return;
    }

    /*
     * Set short timer if we are sending PATH messages with no Router-Alert
     * option to set up multicast state in network. If we are approaching
     * the end of the retry count, set a little longer timer so we can
     * grab the outgoing interface state and send a PATH message with a
     * Router-Alert option right away.
     */
    if (psb->no_ra_retry_count <= 1) {
	rsvp_schedule_path_refresh(psb);
    } else {
	if (psb->no_ra_retry_count == 2) {
	    mgd_timer_start(&psb->refresh_timer, RA_QUICK_INTERVAL);
	} else {
	    mgd_timer_start(&psb->refresh_timer, NO_RA_QUICK_INTERVAL);
	}
    }
    if (psb->no_ra_retry_count)	psb->no_ra_retry_count--;
    path_propagate_psb = psb;

    /*
     * See if there needs some sparse-mode PIM attention. If not and there
     * is still no psb->if_output and we almost reached the retry count,
     * go get some new interfaces from multicast, Joins may have arrived
     * while we were retrying no-RA path messages.
     */
    if (rsvp_needs_sm_attention(psb)) {
	rsvp_path_sparse_install();
    } else if (!psb->if_output.count) {
	rsvp_populate_if_output(psb);
    }
    
    /*
     * Normal case, multicast routing has state and RSVP is in sync with it.
     */
    if (psb->if_output.count) {
	data_walklist(&psb->if_output, rsvp_path_propagate);
    }
}

/*
 * valid that the style of this reservation is mergable with all
 * other extant reservations
 *
 * observation: this is a transitive property. If it is true of the
 * first entry in a database, it is true of all the members of that database,
 * as all have been compared to it.
 */
static boolean
rsvp_incompatible_reservation_style (rsvp_session_head * head,
				     rsvp_style * style)
{
    rsbtype *rsb;

    rsb = rsvp_session_first_reservation(&head->rsb_fixed_database);
    if(rsb)
	return(rsb->style.reservation_type != style->reservation_type);

    rsb = rsvp_session_first_reservation(&head->rsb_shared_database);
    if(rsb)
	return(rsb->style.reservation_type != style->reservation_type);

    return(FALSE);
}

/*
 *  rsvp_calculate_max_matching
 *
 * merge incoming reservations on idb to determine current need
 */
static rsbtype *rsvp_calculate_max_matching_rsb;

static boolean
rsvp_calculate_max_matching (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    rsbtype *lub;

    lub = rsvp_calculate_max_matching_rsb;

    if(rsb->filter.source && lub->filter.source &&
       rsb->filter.source != lub->filter.source)
	return(TRUE);
    if(rsb->filter.udp_source_port && lub->filter.udp_source_port &&
       rsb->filter.udp_source_port != lub->filter.udp_source_port)
	return(TRUE);

    if(rsb->if_input && lub->if_input &&
       rsb->if_input != lub->if_input)
	return(TRUE);

    rsvp_merge_flow_specs(&rsb->flowspec, &lub->flowspec);
    return(TRUE);
}

/*
 * find largest possible(merged) flowspec, one or all interfaces
 */
static void
rsvp_find_largest_matching_flowspec (rsbtype * rsb, boolean all_interfaces)
{
    idbtype *idb = rsb->if_input;

    if(all_interfaces)
	rsb->if_input = NULL;
    bzero(&rsb->flowspec, sizeof(rsb->flowspec));

    rsvp_calculate_max_matching_rsb = rsb;
    wavl_walk(&rsb->head->rsb_incoming_database, 0, rsvp_calculate_max_matching);
    rsb->if_input = idb;
}

/*
 *  rsvp_remove_similar_requests
 *
 * remove potentially merged requests
 */
static rsbtype *rsvp_remove_similar_requests_rsb;
static wavl_handle *rsvp_remove_similar_requests_handle;

static boolean
rsvp_remove_similar_requests (wavl_node_type * node, va_list argptr)
{
    rsbtype *request =(rsbtype *) node;
    rsbtype *rsb;
    wavl_handle *handle;
    boolean result = TRUE;
    rsvp_flowspec flowspec;

    rsb = rsvp_remove_similar_requests_rsb;
    handle = rsvp_remove_similar_requests_handle;

    if(request->filter.source && rsb->filter.source &&
       request->filter.source != rsb->filter.source)
	return(result);
    if(request->filter.udp_source_port && rsb->filter.udp_source_port &&
       request->filter.udp_source_port != rsb->filter.udp_source_port)
	return(result);

    /* We had a matching sender spec, now check to see if there
     * are other reservations supporting this request
     */

    flowspec = request->flowspec;
    rsvp_find_largest_matching_flowspec(request, TRUE);
    if(rsvp_get_flow_requested_rate(&request->flowspec)) {
      /*
       * there are: update the reservation, don't tear it down
       */
      RSVP_RESV_BUGINF(request->head->session.destination,
		       ("\nRSVP: update %s RESV request %i(%d) <- %i(%d:%d)",
			request->if_input->namestring,
			request->head->session.destination,
			request->head->session.udp_dest_port,
			request->filter.source,
			request->head->session.protocol,
			request->filter.udp_source_port));
      rsvp_trigger_resv_refresh(request);
      return(result);
    }
    

    /* We didn't find any other supporting reservations, so we
     * need to teardown the request
     */

    request->flowspec = flowspec;
    result = FALSE;
    /*
     * tear down this reservation request
     */
    RSVP_RESV_BUGINF(request->head->session.destination,
		     ("\nRSVP: remove %s RESV request %i(%d) <- %i(%d:%d)",
		      request->if_input->namestring,
		      request->head->session.destination,
		      request->head->session.udp_dest_port,
		      request->filter.source,
		      request->head->session.protocol,
		      request->filter.udp_source_port));

    rsvp_stop_resv_refresh(request);
    rsvp_send_resv_teardown(request->if_input, &request->hop,
			    &request->head->session, request->scope,
			    &request->style, &request->flowspec,
			    &request->filter);
    rsvp_rsb_delete(handle, request);
    request->head->rsb_outbound_count--;
    free(request);
    return(result);
}

/*
 * hunt and destroy subsuming requests
 */
static inline void
rsvp_remove_matching_requests (wavl_handle * handle, rsbtype * rsb)
{
    rsvp_remove_similar_requests_rsb = rsb;
    rsvp_remove_similar_requests_handle = handle;
    wavl_walk(handle, 0, rsvp_remove_similar_requests);
}

/*
 * rsvp_refresh_reservation
 *
 * timer driver routine - refresh reservation state upstream
 * triggered updates are done by triggering the routine to do an update
 */
void
rsvp_refresh_reservation (rsbtype * rsb)
{
    int minimum_interval;
    int rate_control;
    int maximum_interval;

    if (!rsb->if_input->rsvp_info || !interface_up_simple(rsb->if_input)) {
        rsvp_remove_matching_requests(rsvp_request_database(rsb), rsb);
        return;
    }

    if(TIMER_RUNNING_AND_AWAKE(rsb->cleanup_timer))
	return;

    minimum_interval = RSVP_REFRESH_MS;
    rate_control = (12*rsb->time_values.refresh_ms)/10;
    maximum_interval = 1000*rsb->head->rsb_outbound_count;
    rsb->time_values.refresh_ms = max(minimum_interval,
				      min(rate_control, maximum_interval));
    rsvp_schedule_resv_refresh(rsb);

    RSVP_RESV_BUGINF(rsb->head->session.destination,
		     ("\nRSVP: Sending RESV message for %i",
		      rsb->head->session.destination));
    rsvp_send_resv(rsb->if_input, &rsb->hop,
		   &rsb->head->session, &rsb->time_values,
		   &rsb->policy, &rsb->confirm, rsb->scope, &rsb->style,
		   &rsb->flowspec, &rsb->filter);

    rsb->confirm.base.length = 0;
}

/*
 *  rsvp_install_matching_requests
 *
 * find a matching entry(may be more than one) in the outbound rsb database.
 * if one or both flow spec parameters are equal, we may be reducing our
 * allocation; if so, determine what allocation should obtain.
 *
 * if no allocation obtains, tear down the reservation
 */
static boolean
rsvp_install_matching_requests (psbtype * psb, rsbtype * rsb)
{
    rsbtype *request;
    wavl_handle *handle;

    if (ip_ouraddress(psb->hop.neighbor))
	return TRUE;
    if (0 == rsvp_get_flow_requested_rate(&rsb->flowspec) ||
	QOS_TSPEC == rsvp_get_flow_qos_service(&rsb->flowspec))
	return TRUE;
    if (!interface_up_simple(rsb->if_input))
	return TRUE;
    if (!interface_up_simple(psb->if_input))
	return TRUE;
    if (psb->if_input == rsb->if_input)
	return TRUE;
    if(psb->template.source && rsb->filter.source &&
       psb->template.source != rsb->filter.source)
	return(TRUE);
    if(psb->template.udp_source_port && rsb->filter.udp_source_port &&
       psb->template.udp_source_port != rsb->filter.udp_source_port)
	return(TRUE);
    handle = rsvp_request_database(rsb);
    switch(rsvp_session_request_search(handle, &request,
				       psb->if_input,
				       psb->hop.neighbor,
				       rsb->style.reservation_type,
				       psb->template.source,
				       psb->template.udp_source_port)) {
    case RSVP_NO_MEMORY:
	return TRUE;

    case RSVP_NOT_FOUND:
	request->head = rsb->head;
	request->if_input = psb->if_input;
	request->hop = psb->hop;
	request->style = rsb->style;
	request->filter = rsb->filter;
	request->flowspec = rsb->flowspec;

	if(!rsvp_rsb_insert(handle, request)) {

	    /*
	     * insert failed
	     */
	    free(request);
	    return TRUE;
	}
	request->head->rsb_outbound_count++;
	mgd_timer_init_leaf(&request->refresh_timer, &request->head->master,
			    TIMER_TYPE_REFRESH_RESERVATION, request, FALSE);
	break;

    case RSVP_FOUND:
	if (!rsvp_merge_flow_specs(&rsb->flowspec, &request->flowspec))
	    return TRUE;
	break;
    }
    rsvp_build_refresh(&request->time_values,
		       max(RSVP_REFRESH_MS,
			   max(psb->time_values.refresh_ms,
			       rsb->time_values.refresh_ms)));

    if(request->filter.udp_source_port ||
       request->head->session.udp_dest_port) {

	RSVP_RESV_BUGINF(rsb->head->session.destination,
		   ("\nRSVP: start requesting %d kbps %s reservation for "
		     "%i(%d) %s-> %i(%d) on %s neighbor %i",
		     rsvp_get_flow_requested_rate(&request->flowspec) / 1000,
		     request->style.reservation_type == RSVP_WF_OPTION ? "WF" :
		     request->style.reservation_type == RSVP_SE_OPTION ? "SE" : "FF",
		     request->filter.source,
		     request->filter.udp_source_port,
		     request->head->session.protocol == UDP_PROT ? "UDP" : "TCP",
		     request->head->session.destination,
		     request->head->session.udp_dest_port,
		     request->if_input->namestring,
		     request->hop.neighbor));
    } else {
	RSVP_RESV_BUGINF(rsb->head->session.destination,
			 ("\nRSVP: start requesting %d kbps %s reservation "
			  "for %i %d-> %i on %s neighbor %i",
			  rsvp_get_flow_requested_rate(&request->flowspec)/1000,
			  request->style.reservation_type == RSVP_WF_OPTION ? 
			  "WF" :
			  request->style.reservation_type == RSVP_SE_OPTION ? 
			  "SE" : "FF",
			  request->filter.source,
			  request->head->session.protocol,
			  request->head->session.destination,
			  request->if_input->namestring,
			  request->hop.neighbor));
    }
    /*
     * triggered update of reservation message
     */
    if (rsb->confirm.base.length) {
	request->confirm = rsb->confirm;
	rsb->confirm.base.length = 0;
    }
    rsvp_trigger_resv_refresh(request);

    return(TRUE);
}

/*
 * change reservations for all senders
 */
static rsbtype *rsvp_install_matching_rsb;
static psbtype *rsvp_install_matching_psb;

static boolean
rsvp_install_matching_requests_rsb (wavl_node_type * node, va_list argptr)
{
    psbtype *psb =(psbtype *) node;

    return rsvp_install_matching_requests(psb, rsvp_install_matching_rsb);
}

static boolean
rsvp_install_matching_requests_psb (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;

    return rsvp_install_matching_requests(rsvp_install_matching_psb, rsb);
}

static inline void
rsvp_scan_paths_for_path (psbtype * psb)
{
    rsvp_install_matching_psb = psb;
    wavl_walk(&psb->head->rsb_incoming_database, 0, rsvp_install_matching_requests_psb);
}

static inline void
rsvp_scan_paths_for_resv (rsbtype * rsb)
{
    rsvp_install_matching_rsb = rsb;
    wavl_walk(&rsb->head->psb_database, 0, rsvp_install_matching_requests_rsb);
}

/*
 * rsb expiration routine
 */
boolean
rsvp_rsb_expiry (wavl_node_type * node, va_list argptr)
{
    rsvp_flowspec flowspec;
    rsbtype *rsb =(rsbtype *) node;

    if (interface_up_simple(rsb->if_input) &&
	TIMER_RUNNING_AND_SLEEPING(rsb->cleanup_timer))
	return(TRUE);

    RSVP_RESV_BUGINF(rsb->head->session.destination,
		     ("\nRSVP: remove %s RESV %i(%d) <- %i(%d:%d)",
		      rsb->if_input->namestring,
		      rsb->head->session.destination,
		      rsb->head->session.udp_dest_port,
		      rsb->filter.source,
		      rsb->head->session.protocol,
		      rsb->filter.udp_source_port));

    flowspec = rsb->flowspec;
    rsvp_rsb_delete(&rsb->head->rsb_incoming_database, rsb);
    rsvp_find_largest_matching_flowspec(rsb, FALSE);
    rsvp_update_conversation(rsb);
    rsvp_remove_matching_requests(rsvp_request_database(rsb), rsb);
    rsb->head->rsb_incoming_count--;
    free(rsb);
    return(TRUE);
}

/*
 * rsvp_expire_matching_flows
 *
 * expire reservations that match the incoming psb.
 * note that the spec has us expire only those exclusively matching;
 * here, we expire all INCLUSIVELY matching. Reservations that should
 * not have gone away will be reinstalled over the next 30 seconds.
 *
 * good area to revisit...
 */
static psbtype *rsvp_expire_psb_matching_rsb;
static boolean
rsvp_expire_matching_rsb (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    psbtype *psb;

    psb = rsvp_expire_psb_matching_rsb;

    if(rsb->filter.source && psb->template.source &&
       rsb->filter.source != psb->template.source)
	return(TRUE);
    if(rsb->filter.udp_source_port && psb->template.udp_source_port &&
       rsb->filter.udp_source_port != psb->template.udp_source_port)
	return(TRUE);

    TIMER_START(rsb->cleanup_timer, 0);
   (void) rsvp_rsb_expiry(rsb->threads, argptr);

    return(TRUE);
}

/*
 * find largest possible(merged) flowspec, one or all interfaces
 */
static inline void
rsvp_expire_matching_flows (psbtype * psb)
{
    rsvp_expire_psb_matching_rsb = psb;
    wavl_walk(&psb->head->rsb_incoming_database, 0, rsvp_expire_matching_rsb);
}

/*
 * psb expiration routine
 */
boolean
rsvp_psb_expiry (wavl_node_type * node, va_list argptr)
{
    psbtype *psb =(psbtype *) node;
    idbtype *idb;

    if (interface_up_simple(psb->if_input) && 
	TIMER_RUNNING_AND_SLEEPING(psb->cleanup_timer))
	return(TRUE);

    RSVP_PATH_BUGINF(psb->head->session.destination,
		     ("\nRSVP: remove %s PATH %i(%d) <- %i(%d:%d)",
		      psb->if_input->namestring,
		      psb->head->session.destination,
		      psb->head->session.udp_dest_port,
		      psb->template.source,
		      psb->head->session.protocol,
		      psb->template.udp_source_port));

    rsvp_psb_delete(&psb->head->psb_database, psb);
    psb->head->psb_count--;
    rsvp_stop_path_refresh(psb);

    /*
     * forward the message to each next hop
     */
    while((idb = data_dequeue(&psb->if_output))) {

	/*
	 * forward to multicast neighbors
	 */
	rsvp_send_path_teardown(idb, psb->ttl,
				&psb->head->session, &psb->template,
				&psb->traffic_specification,
			        &psb->advertisement);
    }
    rsvp_expire_matching_flows(psb);
    free(psb);
    return(TRUE);
}

/*
 * rsvp_clean_interface
 *
 * Remove all path and reservation information for a down interface
 */
static idbtype *rsvp_idb_to_clean;
static boolean
rsvp_clean_idb_psb (wavl_node_type * node, va_list argptr)
{
    psbtype *psb =(psbtype *) node;
    idbtype *idb = rsvp_idb_to_clean;

    if(idb == psb->if_input) {
	TIMER_START(psb->cleanup_timer, 0);
	rsvp_psb_expiry(psb->threads, NULL);
    }
    return(TRUE);
}

static boolean
rsvp_clean_idb_incoming (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    idbtype *idb = rsvp_idb_to_clean;

    if(idb == rsb->if_input) {
	TIMER_START(rsb->cleanup_timer, 0);
	rsvp_rsb_expiry(rsb->threads, NULL);
    }
    return(TRUE);
}

static boolean
rsvp_clean_interface_session (wavl_node_type * node, va_list arglst)
{
    rsvp_session_head *head =(rsvp_session_head *) node;

    wavl_walk(&head->psb_database, 0, rsvp_clean_idb_psb);
    wavl_walk(&head->rsb_incoming_database, 0, rsvp_clean_idb_incoming);
    return TRUE;
}

void
rsvp_clean_interface (idbtype * idb)
{
    rsvp_idb_to_clean = idb;
    wavl_walk(&rsvp_session_database, 0, rsvp_clean_interface_session);
}

/*
 * walking routine to list merged reservations for an interface
 *
 * returns TRUE(stop search) when
 *
 * action effected:
 */
static rsvp_confirm_ipv4 *rsvp_rsb_apply_confirm_confirm;
static rsvp_flowspec *rsvp_rsb_apply_confirm_flowspec;
static rsvp_filter_ipv4 *rsvp_rsb_apply_confirm_filter;
static rsvp_error_ipv4 *rsvp_rsb_apply_confirm_spec;
static boolean
rsvp_rsb_apply_confirm (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    rsvp_flowspec *flowspec;
    rsvp_filter_ipv4 *filter;
    rsvp_confirm_ipv4 *confirm_spec;
    rsvp_error_ipv4 *error;
    idbtype *upstream_idb;
    ipaddrtype source;

    flowspec = rsvp_rsb_apply_confirm_flowspec;
    filter = rsvp_rsb_apply_confirm_filter;
    confirm_spec = rsvp_rsb_apply_confirm_confirm;
    error = rsvp_rsb_apply_confirm_spec;

    if(rsb->filter.source && filter->source &&
       rsb->filter.source != filter->source)
	return TRUE;
    if(rsb->filter.udp_source_port && filter->udp_source_port &&
       rsb->filter.udp_source_port != filter->udp_source_port)
	return TRUE;

    source = confirm_spec->receiver;
    upstream_idb =
	reg_invoke_ip_first_hop(&source, source,
				(IP_PATH_ROUNDROBIN | IP_PATH_FAIL_FORUS));
    if (upstream_idb != rsb->if_input)
	return TRUE;

    /*
     * OK, this is one of the RSBs that is forcing an confirm;
     * propagate the confirm
     */

    rsvp_send_resv_confirm(rsb->if_input, rsb->hop.neighbor,
			   &rsb->head->session, error, confirm_spec,
			   &rsb->style, &rsb->flowspec, &rsb->filter);
    return(TRUE);
}

/*
 * propagate a reservation confirm PDU to any relevant folks
 */
static inline void
rsvp_forward_resv_confirm (rsvp_session_head * head,
			   rsvp_confirm_ipv4 *confirm_spec,
			   rsvp_flowspec *flowspec,
			   rsvp_filter_ipv4 *filter,
			   rsvp_error_ipv4 *error)
{

    rsvp_rsb_apply_confirm_flowspec = flowspec;
    rsvp_rsb_apply_confirm_filter = filter;
    rsvp_rsb_apply_confirm_confirm = confirm_spec;
    rsvp_rsb_apply_confirm_spec = error;

    wavl_walk(&head->rsb_incoming_database, 0, rsvp_rsb_apply_confirm);
}

/*
 * walking routine to list merged reservations for an interface
 *
 * returns TRUE(stop search) when
 *
 * action effected:
 */
static rsvp_scope_ipv4 *rsvp_rsb_apply_error_scope;
static rsvp_flowspec *rsvp_rsb_apply_error_flowspec;
static rsvp_filter_ipv4 *rsvp_rsb_apply_error_filter;
static rsvp_error_ipv4 *rsvp_rsb_apply_error_spec;
static boolean
rsvp_rsb_apply_error (wavl_node_type * node, va_list argptr)
{
    rsbtype *rsb =(rsbtype *) node;
    rsvp_scope_ipv4 *scope;
    rsvp_flowspec *flowspec;
    rsvp_filter_ipv4 *filter;
    rsvp_error_ipv4 *error_spec;

    scope = rsvp_rsb_apply_error_scope;
    flowspec = rsvp_rsb_apply_error_flowspec;
    filter = rsvp_rsb_apply_error_filter;
    error_spec = rsvp_rsb_apply_error_spec;

    if(rsb->filter.source && filter->source &&
       rsb->filter.source != filter->source)
	return TRUE;
    if(rsb->filter.udp_source_port && filter->udp_source_port &&
       rsb->filter.udp_source_port != filter->udp_source_port)
	return TRUE;

    /*
     * OK, this is one of the RSBs that is forcing an error;
     * propagate the error
     */

    rsvp_send_resv_error(rsb->if_input, rsb->hop.neighbor, &rsb->hop,
			 &rsb->head->session, error_spec, rsb->scope,
			 &rsb->policy, &rsb->style, &rsb->flowspec,
			 &rsb->filter);

    return(TRUE);
}

/*
 * propagate a reservation error PDU to any relevant folks
 */
static inline void
rsvp_forward_resv_error (rsvp_session_head * head,
			 rsvp_scope_ipv4 * scope,
			 rsvp_flowspec * flowspec,
			 rsvp_filter_ipv4 * filter,
			 rsvp_error_ipv4 * error_spec)
{
    rsvp_rsb_apply_error_scope = scope;
    rsvp_rsb_apply_error_flowspec = flowspec;
    rsvp_rsb_apply_error_filter = filter;
    rsvp_rsb_apply_error_spec = error_spec;

    wavl_walk(&head->rsb_incoming_database, 0, rsvp_rsb_apply_error);
}

/*
 * rsvp_right_incoming_interface()
 *
 * verify that a path, path error, or path tear is coming from the
 * indicated sender
 */
static boolean rsvp_right_incoming_interface (paktype *pak,
					      ipaddrtype sender,
					      ipaddrtype destination,
					      ipaddrtype neighbor,
					      ipaddrtype *next_hop,
					      idbtype **idb,
					      char *mtype)
{

    /*
     * check the route by which we received this, looking for routing
     * flaps.
     */
    if(!rsvp_upstream_lookup(sender, destination, idb, next_hop)) {

	/*
	 * route lookup failed, ignore message
	 */
	RSVP_BUGINF(destination, ("\nRSVP %s: no route to sender %i ",
				  mtype, sender));
	return FALSE;
    };

    if (IPMULTICAST(destination)) {
        if (*idb && pak->if_input != *idb) {
	    /*
	     * Lookup resulted in different interface or neighbor
	     */
	    RSVP_BUGINF(destination,
			("\nRSVP %s: not on route {%i, %i}",
			 mtype, sender, destination));
	    return FALSE;
	}
    }
    return TRUE;
}

/*
 * rsvp_incoming_path_message()
 *
 * we are in receipt of a path message with a valid header.
 *
 * PATH messages carry information from senders to receivers along
 * the paths used by the data packets. The IP destination address
 * of a PATH message is the DestAddress for the session; the
 * source address is an address of the node that sent the message
 *(preferably the address of the interface through which it was
 * sent). The PHOP(i.e., the RSVP_HOP) object of each PATH
 * message must contain the address of the interface through which
 * the PATH message was sent.
 */
static void
rsvp_incoming_path_message (rsvp_header * rsvp_header_ptr,
			    rsvp_session_head * head, paktype * pak,
			    void **objects)
{
    ushort length;
    rsvp_object * object;
    queuetype *queue_ptr;
    psbtype *psb;
    iphdrtype *ip = iphdrtype_start(pak);
    rsvp_error_ipv4 error_spec;
    rsvp_hop_ipv4 *hop;
    rsvp_refresh *refresh;
    rsvp_template_ipv4 *template;
    rsvp_tspec *traffic_specification;
    rsvp_policy *policy;
    rsvp_adspec *advertisement;

    hop = objects[RSVP_CLASS_HOP];
    refresh = objects[RSVP_CLASS_TIME_VALUES]?
	objects[RSVP_CLASS_TIME_VALUES]: &rsvp_default_refresh;
    policy = objects[RSVP_CLASS_POLICY_DATA];
    template = objects[RSVP_CLASS_SENDER_TEMPLATE];
    traffic_specification = objects[RSVP_CLASS_SENDER_TSPEC];
    advertisement = objects[RSVP_CLASS_ADSPEC];

    /*
     * parse initial objects of PATH message
     */

    if(!hop || !rsvp_valid_hop_ipv4(hop, pak->if_input)) {
	RSVP_PATH_BUGINF(head->session.destination,
 			 ("\nRSVP: Couldn't get HOP information "
 			  "from PATH message"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);

	return;
    }
    if(!rsvp_valid_refresh(refresh)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP: Couldn't get REFRESH information "
			  "from PATH message"));
	goto dump_object;
    }
    if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	/* check for looping messages */
        return;
    if(policy && !rsvp_valid_policy(policy)) {
	/* note that this can only occur if policy is present */
	rsvp_build_error_ipv4(&error_spec, rsvp_ip_address(pak->if_input), 0,
			      RSVP_ERROR_ADMINISTRATIVE, 0);

	rsvp_send_path_error(pak->if_input, ip->srcadr,
			     &head->session, &error_spec, template,
			     traffic_specification, policy,
			     advertisement);
	goto dump_object;
    }
    if(!template || 
       !rsvp_valid_template_ipv4(template, &head->session)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP: Couldn't get TEMPLATE information "
			  "from PATH message"));
	goto dump_object;
    }
    if(!traffic_specification || !rsvp_valid_tspec(traffic_specification)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP: Couldn't get SENDER TSPEC information "
			  "from PATH message"));
	goto dump_object;
    }

    RSVP_PATH_BUGINF(head->session.destination,
		     ("\nRSVP: PATH message for %i(%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      hop->neighbor));
    /*
     * Each sender descriptor object sequence in the message defines a
     * sender. Process each sender as follows, starting the
     * Path_Refresh_Needed and Resv_Refresh_Needed flags off.
     */
    length = rsvp_header_ptr->rsvp_length -
	(((uchar *) template) - ((uchar *) rsvp_header_ptr));
    object = &template->base;
    while(length) {
	boolean Check_Reservations_Needed = FALSE;
	boolean Refresh_Needed = FALSE;
	idbtype *idb = NULL;
	ipaddrtype next_hop = 0;

	template = NULL;
	traffic_specification = NULL;
	advertisement = NULL;
	/*
	 * parse initial per-sender objects of PATH message
	 */
	if(!rsvp_extract_template_ipv4(&length, &object, &template,
				       &head->session) ||
	   !template) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP: Bad template info in PATH message!"));
	    goto dump_object;
	}
	if(!rsvp_extract_tspec_ipv4(&length, &object,
				    &traffic_specification) ||
	   !traffic_specification) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH ERROR: invalid traffic "
			      "specification object"));
	    goto dump_object;
	}
	if(!rsvp_extract_adspec(&length, &object, &advertisement)) {
	    return;
	};

	/*
	 * Search for a path state block(PSB) whose(SESSION,
	 * SENDER_TEMPLATE) pair matches the corresponding objects in
	 * the message.
	 */
    reinstall_psb:
	switch(rsvp_session_path_search(&head->psb_database, &psb,
					template->source,
					template->udp_source_port)) {
	case RSVP_NO_MEMORY:
	    return;

	case RSVP_NOT_FOUND:

	    /*
	     * Create a new PSB.
	     */
	    psb->head = head;
	    Refresh_Needed = TRUE;

	    psb->if_input = pak->if_input;
	    rsvp_object_copy(&psb->hop, hop);
	    rsvp_object_copy(&psb->template, template);
	    rsvp_object_copy(&psb->time_values, refresh);
	    if(traffic_specification)
		rsvp_object_copy(&psb->traffic_specification,
				 traffic_specification);
	    if(policy)
		rsvp_object_copy(&psb->policy, policy);
	    if(advertisement)
		rsvp_object_copy(&psb->advertisement, advertisement);

	    mgd_timer_init_leaf(&psb->refresh_timer, &head->master,
                                TIMER_TYPE_REFRESH_PATH, psb, FALSE);

	    queue_ptr = NULL;
	    idb = NULL;
	    next_hop = 0;
	    if (!rsvp_right_incoming_interface(pak, template->source,
					       head->session.destination,
					       hop->neighbor,
					       &next_hop, &idb, "PATH")) {
		free(psb);
		continue;
	    }

	    if(!rsvp_psb_insert(&head->psb_database, psb)) {

		/*
		 * insert failed
		 */
		free(psb);
		continue;
	    }
	    head->psb_count++;
	    queue_init(&psb->if_output, 0);

	    queue_ptr = NULL;
	    idb = NULL;
	    next_hop = 0;
	    if(!rsvp_downstream_lookup(template->source,
				       head->session.destination,
				       &queue_ptr, &idb, &next_hop)) {

		/*
		 * route lookup failed, ignore any need to send a refresh
		 * We can also fall here if we are the DR for a newly
		 * created sparse group.  In that case we want to trigger
		 * a refresh.
		 */

	      if (rsvp_needs_sm_attention(psb)) {
		psb->no_ra_retry_count = PSB_NO_RA_RETRIES;
		rsvp_refresh_path(psb);
		Refresh_Needed = FALSE;
	      } else {
		RSVP_PATH_BUGINF(head->session.destination,
				 ("\nRSVP Couldn't find route for %i",
				  head->session.destination));
		Refresh_Needed = FALSE;
	      }
	    } else if(idb) {

		/*
		 * unicast route
		 */
		data_enqueue(&psb->if_output, idb);
		Check_Reservations_Needed = TRUE;
	    } else if(queue_ptr) {

		/*
		 * multicast route
		 */
		while(rsvp_get_downstream_interface(queue_ptr, idb, &idb,
						    &next_hop,
						    rsvp_header_ptr->sending_ttl)) {
		    if(!idb)
			break;
		    data_enqueue(&psb->if_output, idb);
		    Check_Reservations_Needed = TRUE;
		}

		/*
		 * Send PATH message through multicast routing to set up state.
		 * Don't use Router-Alert option.
		 */
		if (!psb->if_output.count) {
		    Refresh_Needed = TRUE;
		    if (!psb->no_ra_retry_count) {
			psb->no_ra_retry_count = PSB_NO_RA_RETRIES;
		    }
		}
	    } else {
		Refresh_Needed = FALSE;
	    }

	    if(refresh->refresh_ms) {
		if(!mgd_timer_running(&head->master)) {
		    /*
		     * if not running, start cleanup timer 
		     */
		    mgd_timer_start(&head->cleanup, 4 * refresh->refresh_ms);
		} else if(4 * refresh->refresh_ms <
			  mgd_timer_left_sleeping(&head->cleanup)) {

		    /*
		     * if not first in cleanup period, restart cleanup period
		     */
		    mgd_timer_start(&head->cleanup, 4 * refresh->refresh_ms);
		}
	    }
	    break;

	case RSVP_FOUND:

	    /*
	     * If there is a match considering wildcards in the
	     * SENDER_TEMPLATE objects, but the two SENDER_TEMPLATEs
	     * differ, build and send a "Ambiguous Path" PERR message,
	     * drop the PATH message, and return.
	     */
	    if(!rsvp_same_path(&head->session, template, psb)) {
		RSVP_PATH_BUGINF(head->session.destination,
				 ("\nRSVP PATH: ambiguous path information"));
		rsvp_build_error_ipv4(&error_spec, psb->if_input->ip_address,
				      0, RSVP_ERROR_AMBIGUOUS_PATH, 0);

		rsvp_send_path_error(psb->if_input, psb->hop.neighbor,
				     &head->session, &error_spec, template,
				     traffic_specification, policy,
				     advertisement);
		continue;
	    }
	    
	    if(advertisement) {
		if(bcmp(advertisement, &psb->advertisement,
			advertisement->base.length)) {
 	            
	            rsvp_object_copy(&psb->advertisement, advertisement);
		    Refresh_Needed = TRUE;
		}
	    }
	    if(traffic_specification) {
		if (bcmp(traffic_specification, &psb->traffic_specification,
			traffic_specification->base.length)) {

		    rsvp_object_copy(&psb->traffic_specification,
		    		     traffic_specification);
		    Refresh_Needed = TRUE;
		    Check_Reservations_Needed = TRUE;
		}
	    }

	    /*
	     * if recieved on a different path than before
	     * and this is the right path, then the old (and therefore
	     * its reservations) must be wrong. Change them.
	     */
	    if ((pak->if_input != psb->if_input) ||
		(hop->neighbor != psb->hop.neighbor)) {

		queue_ptr = NULL;
		idb = NULL;
		next_hop = 0;
		if (!rsvp_right_incoming_interface(pak, template->source,
						   head->session.destination,
						   hop->neighbor,
						   &next_hop, &idb, "PATH")) {

		    /*
		     * if not right path, ignore this information
		     */
		    continue;
		} else {

		    /*
		     * Tear down the old one and install a new one.
		     */
		    RSVP_PATH_BUGINF(head->session.destination,
				     ("\nRSVP PATH: path changed from %s:%i "
				      "to %s:%i",
				      psb->if_input->namestring,
				      psb->hop.neighbor,
				      pak->if_input->namestring,
				      hop->neighbor));
		    TIMER_START(psb->cleanup_timer, 0);
		    rsvp_psb_expiry(psb->threads, NULL);
		    Refresh_Needed = TRUE;
		    Check_Reservations_Needed = TRUE;
		    goto reinstall_psb;
		}
	    }

	    queue_ptr = NULL;
	    idb = NULL;
	    next_hop = 0;
	    if(!rsvp_downstream_lookup(template->source,
				       head->session.destination,
				       &queue_ptr, &idb, &next_hop)) {

		/*
		 * route lookup failed, ignore any need to send a refresh
		 */
		RSVP_BUGINF(head->session.destination,
			    ("\nRSVP Couldn't find route for %i",
			     head->session.destination));
		goto flush_paths;
	    } else if(idb) {
	        /*
                 * unicast reservation.
                 */
		if(psb->if_output.count != 1 ||
		   idb !=(idbtype *) pak_queue_get_head(&psb->if_output)) {
		    while(data_dequeue(&psb->if_output));
		    data_enqueue(&psb->if_output, idb);
		    Refresh_Needed = TRUE;
		    Check_Reservations_Needed = TRUE;
		}
	    } else if(queue_ptr) {
		int idb_count = 0;

		/*
		 * make sure all interfaces are the same
		 */
		while(rsvp_get_downstream_interface(queue_ptr, idb,
						    &idb, &next_hop,
						    rsvp_header_ptr->sending_ttl)) {
		    if(!idb)
			break;
		    if(!checkqueue(&psb->if_output, idb))
			goto lookup_failed;
		    idb_count++;
		};

		/*
		 * list of output interfaces differs; reconstruct
		 */
		if(psb->if_output.count != idb_count) {
		lookup_failed:
		    while(data_dequeue(&psb->if_output));
		    Refresh_Needed = TRUE;
		    idb = NULL;
		    while(rsvp_get_downstream_interface(queue_ptr, idb,
							&idb, &next_hop,
							rsvp_header_ptr->sending_ttl)) {
			data_enqueue(&psb->if_output, idb);
			Check_Reservations_Needed = TRUE;
		    }
		}
	    } else {
	    flush_paths:
		Refresh_Needed = FALSE;
		while((idb = data_dequeue(&psb->if_output)))
		    rsvp_send_path_teardown(idb, psb->ttl,
					    &psb->head->session,
					    &psb->template,
					    &psb->traffic_specification,
					    &psb->advertisement);
	    }
	    break;
	}
	TIMER_START(psb->cleanup_timer, 4 * refresh->refresh_ms);
	psb->ttl = rsvp_header_ptr->sending_ttl;

	if(Refresh_Needed) {
	    rsvp_trigger_path_refresh(psb);
	}
	if(Check_Reservations_Needed) {
	    rsvp_scan_paths_for_path(psb);
	}
    }
    /*
     *  end of scan through senders 
     */
}

/*
 * rsvp_incoming_path_error_message()
 *
 * we are in receipt of a path error message with a valid header.
 *
 * PATH messages carry information from senders to receivers along
 * the paths used by the data packets. The IP destination address
 * of a PATH message is the DestAddress for the session; the
 * source address is an address of the node that sent the message
 *(preferably the address of the interface through which it was
 * sent). The PHOP(i.e., the RSVP_HOP) object of each PATH
 * message must contain the address of the interface through which
 * the PATH message was sent.
 */
static void
rsvp_incoming_path_error_message (rsvp_header *rsvp_header_ptr,
				  rsvp_session_head * head, paktype * pak,
			    	  void **objects)
{
    ushort length;
    rsvp_object * object;
    iphdrtype *ip = iphdrtype_start(pak);
    psbtype *psb;
    rsvp_error_ipv4 *error_spec;
    idbtype *idb = NULL;
    ipaddrtype next_hop = 0;
    rsvp_policy *policy;
    rsvp_template_ipv4 *template;
    rsvp_tspec *traffic_specification;
    rsvp_adspec *advertisement;

    error_spec = objects[RSVP_CLASS_ERROR_SPEC];
    template = objects[RSVP_CLASS_SENDER_TEMPLATE];
    traffic_specification = objects[RSVP_CLASS_SENDER_TSPEC];
    policy = objects[RSVP_CLASS_POLICY_DATA];
    advertisement = objects[RSVP_CLASS_ADSPEC];

    /*
     * parse initial objects of PATH message
     */

    if(!error_spec || !rsvp_valid_error_ipv4(error_spec)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP PATH ERROR: invalid error spec"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }

    if(policy && !rsvp_valid_policy(policy)) {
	RSVP_BUGINF(head->session.destination,
		    ("\nRSVP PATH ERROR: invalid policy object"));
	goto dump_object;
    }

    if(advertisement && !rsvp_valid_adspec(advertisement)) {
	RSVP_BUGINF(head->session.destination,
		    ("\nRSVP PATH ERROR: invalid adspec object"));
	goto dump_object;
    }

    if(!template || 
       !rsvp_valid_template_ipv4(template, &head->session)) {
	RSVP_BUGINF(head->session.destination,
		    ("\nRSVP PATH ERROR: invalid template object"));
	goto dump_object;
    }

    if(!traffic_specification || !rsvp_valid_tspec(traffic_specification)) {
	RSVP_BUGINF(head->session.destination,
		    ("\nRSVP PATH ERROR: invalid sender tspec object"));
	goto dump_object;
    }

    RSVP_PATH_BUGINF(head->session.destination,
		     ("\nRSVP: PATH ERROR message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      ip->srcadr));

    /*
     * Each sender descriptor object sequence in the message defines a
     * sender. Process each sender as follows, starting the
     * Path_Refresh_Needed and Resv_Refresh_Needed flags off.
     */
    length = rsvp_header_ptr->rsvp_length -
	(((uchar *) template) - ((uchar *) rsvp_header_ptr));
    object = &template->base;
    while(length) {

	/*
	 * parse initial per-sender objects of PATH message
	 */
	template = NULL;
	traffic_specification = NULL;
	advertisement = NULL;
	if(!rsvp_extract_template_ipv4(&length, &object, &template,
				       &head->session) ||
	   !template) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH ERROR: invalid sender template"));
	    goto dump_object;
	}
	if(!rsvp_extract_tspec_ipv4(&length, &object,
				    &traffic_specification) ||
	   !traffic_specification) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH ERROR: invalid traffic "
			      "specification object"));
	    goto dump_object;
	}
	if(!rsvp_extract_adspec(&length, &object, &advertisement)) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH ERROR: invalid advertisement "
			      "object"));
	    goto dump_object;
	}
	if (!rsvp_right_incoming_interface(pak,
					   template->source,
					   head->session.destination,
					   ip->srcadr,
					   &next_hop, &idb, "PATH ERROR"))
	    continue;

	switch(rsvp_session_path_search(&head->psb_database, &psb,
					template->source,
					template->udp_source_port)) {
	case RSVP_NO_MEMORY:
	    return;

	case RSVP_NOT_FOUND:
	    free(psb);
	    continue;

	case RSVP_FOUND:
	    if(rsvp_same_path(&head->session, template, psb)) {
		TIMER_START(psb->cleanup_timer, 0);
		rsvp_psb_expiry(psb->threads, NULL);
	    }
	    if(psb->if_input) {
		rsvp_send_path_error(psb->if_input, psb->hop.neighbor,
				     &head->session, error_spec, template,
				     traffic_specification, policy,
				     advertisement);
	    }
	    break;
	}
    };
}

/*
 * rsvp_incoming_path_tear_message()
 *
 * we are in receipt of a path teardown message with a valid header.
 */
static void
rsvp_incoming_path_tear_message (rsvp_header *rsvp_header_ptr,
				 rsvp_session_head * head, paktype * pak,
			    	 void **objects)
{
    ushort length;
    rsvp_object * object;
    iphdrtype *ip = iphdrtype_start(pak);
    psbtype *psb;
    rsvp_hop_ipv4 *hop;
    rsvp_template_ipv4 *template;
    rsvp_tspec *traffic_specification;
    rsvp_adspec *advertisement;

    hop = objects[RSVP_CLASS_HOP];
    template = objects[RSVP_CLASS_SENDER_TEMPLATE];
    traffic_specification = objects[RSVP_CLASS_SENDER_TSPEC];
    advertisement = objects[RSVP_CLASS_ADSPEC];

    /*
     * parse initial objects of PATH message
     */

    if(!hop || !rsvp_valid_hop_ipv4(hop, pak->if_input)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP PATH TEARDOWN: invalid upstream "
			  "hop object"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }

    if(!template || 
       !rsvp_valid_template_ipv4(template, &head->session)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP PATH TEARDOWN: invalid template "
			  "object"));
	goto dump_object;
    }

    if(!traffic_specification || !rsvp_valid_tspec(traffic_specification)) {
	RSVP_PATH_BUGINF(head->session.destination,
			 ("\nRSVP PATH TEARDOWN: invalid sender tspec "
			  "object"));
	goto dump_object;
    }

    if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	/* check for looping messages */
        return;

    RSVP_PATH_BUGINF(head->session.destination,
		     ("\nRSVP: PATH TEAR message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      ip->srcadr));

    /*
     * Each sender descriptor object sequence in the message defines a
     * sender. Process each sender as follows
     */
    length = rsvp_header_ptr->rsvp_length -
	(((uchar *) template) - ((uchar *) rsvp_header_ptr));
    object = &template->base;
    while(length) {
	template = NULL;
	traffic_specification = NULL;
	advertisement = NULL;

	/*
	 * parse initial per-sender objects of PATH message
	 */
	if(!rsvp_extract_template_ipv4(&length, &object, &template,
				       &head->session) ||
	   !template) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH TEARDOWN: invalid sender "
			      "template"));
	    goto dump_object;
	}
	if(!rsvp_extract_tspec_ipv4(&length, &object,
				    &traffic_specification) ||
	   !traffic_specification) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH TEARDOWN: invalid traffic "
			      "specification object"));
	    goto dump_object;
	}
	if(!rsvp_extract_adspec(&length, &object, &advertisement)) {
	    RSVP_PATH_BUGINF(head->session.destination,
			     ("\nRSVP PATH TEARDOWN: invalid advertisement "
			      "object"));
	    goto dump_object;
	};

	/*
	 * Search for a path state block(PSB) whose(SESSION,
	 * SENDER_TEMPLATE) pair matches the corresponding objects in
	 * the message.
	 */
	switch(rsvp_session_path_search(&head->psb_database, &psb,
					template->source,
					template->udp_source_port)) {
	case RSVP_NO_MEMORY:
	    break;

	case RSVP_NOT_FOUND:
	    free(psb);
	    break;

	case RSVP_FOUND:
	    if(rsvp_same_path(&head->session, template, psb)) {
		TIMER_START(psb->cleanup_timer, 0);
		rsvp_psb_expiry(psb->threads, NULL);
	    }
	    break;
	}
    };
    /*
     *  end of scan through senders 
     */
}

/*
 * rsvp_incoming_resv_confirm_message()
 *
 * we are in receipt of a path teardown message with a valid header.
 */
static void
rsvp_incoming_resv_confirm_message (rsvp_header *rsvp_header_ptr,
				    rsvp_session_head * head, paktype * pak,
			    	    void **objects)
{
    ushort length;
    rsvp_object * object;
    iphdrtype *ip = iphdrtype_start(pak);
    psbtype *first_psb;
    rsvp_error_ipv4 *error_spec;
    rsvp_style *style;
    rsvp_flowspec *flowspec;
    rsvp_filter_ipv4 *filter;
    rsvp_confirm_ipv4 *confirm;

    error_spec = objects[RSVP_CLASS_ERROR_SPEC];
    style = objects[RSVP_CLASS_STYLE];
    flowspec = objects[RSVP_CLASS_FLOWSPEC];
    filter = objects[RSVP_CLASS_FILTER_SPEC];
    confirm = objects[RSVP_CLASS_RESV_CONFIRM];

    /*
     * parse initial objects of RESV message
     */

    if(!error_spec || !rsvp_valid_error_ipv4(error_spec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid error spec"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }
    if(!confirm || !rsvp_valid_confirm_ipv4(confirm)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid confirm object"));
	goto dump_object;
    }
    if(!style || !rsvp_valid_style(style)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid style object"));
	goto dump_object;
    }
    if (!flowspec || !rsvp_valid_flowspec(flowspec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid flowspec object"));
	goto dump_object;
    }
    if(style->reservation_type != RSVP_WF_OPTION) {
	if(!filter || !rsvp_valid_filter_ipv4(filter)) {
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV CONFIRM: invalid filter object"));
	    goto dump_object;
	}
    }
    RSVP_RESV_BUGINF(head->session.destination,
		     ("\nRSVP: RESV CONFIRM message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      ip->srcadr));

    /*
     * locate first session path block
     */
    first_psb = rsvp_session_first_path(&head->psb_database);
    if(!first_psb) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: no path information for %i",
			  head->session.destination));
	return;
    };

    /*
     * parse the flow/filter part of the message
     */
    length = rsvp_header_ptr->rsvp_length -
    	(((uchar *) flowspec) - ((uchar *) rsvp_header_ptr));
    object = &flowspec->base;
    switch(style->reservation_type) {
    case RSVP_FF_OPTION:
	/*
	 * for each flow descriptor, confirm a reservation
	 */
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_flowspec(&length, &object, &flowspec))
		goto bad_ff;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_ff;
	    if(!flowspec || !filter) {
	    bad_ff:
		RSVP_RESV_BUGINF(head->session.destination,
			         ("\nRSVP RESV CONFIRM: Fixed Filter "
				  "incorrectly formatted"));
		return;
	    }
	    rsvp_forward_resv_confirm
		(head, confirm, flowspec, filter, error_spec);
	}
	break;

    case RSVP_WF_OPTION:
	/*
	 * there is one flow descriptor; confirm a reservation
	 */
	filter = &wild_card_filter;	/* default filter spec */
	if(!rsvp_extract_flowspec(&length, &object, &flowspec))
	    goto bad_wf;
	if(!flowspec || !filter || length) {
	bad_wf:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV CONFIRM: Wildcard Filter "
			      "incorrectly formatted"));
	    return;
	}
	rsvp_forward_resv_confirm
	    (head, confirm, flowspec, filter, error_spec);
	break;

    case RSVP_SE_OPTION:
	/*
	 * for each set of flow descriptors, confirm a reservation
	 */
	flowspec = NULL;
	if(!rsvp_extract_flowspec(&length, &object, &flowspec)) {
	bad_se:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV CONFIRM: Shared Filter "
			      "incorrectly formatted"));
	    return;
	}
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_se;
	    if(!flowspec || !filter)
		goto bad_se;
	    rsvp_forward_resv_confirm
		(head, confirm, flowspec, filter, error_spec);
	}
	break;
    }
}

/*
 * rsvp_has_bitrate
 *
 * admit new(increased, potentially) reservation
 */
enum rsvp_bitrate_responses {NO_BITS, YES_BITS, REDUCES_BITRATE};
static enum rsvp_bitrate_responses
rsvp_has_bitrate (rsbtype * rsb, rsvp_flowspec * flowspec)
{
    idbtype *idb;
    rsvp_info_type *rsvp_info;
    rsvp_flow_ipv4 *conversation;
    int delta;

    /*
     * are we in the right place?
     */
    idb = rsb->if_input;
    if(!idb) {
	return(NO_BITS);
    }
    rsvp_info = idb->rsvp_info;
    if(!rsvp_info) {
	return(NO_BITS);
    }

    /* 
     * is there a conversation to work with?
     */
    conversation = rsvp_largest_conversation(idb, rsb);
    if(conversation)
	delta = rsvp_get_flow_requested_rate(flowspec) - conversation->bandwidth;
    else
	delta = rsvp_get_flow_requested_rate(flowspec);
    if(delta <= 0) {
	return(REDUCES_BITRATE);
    }

    /*
     * admission - do we have sufficient bandwidth?
     */
    if(rsvp_info->maximum_flow_bandwidth < rsvp_get_flow_requested_rate(flowspec)) {
	return(NO_BITS);
    }
    if(rsvp_info->maximum_bandwidth < rsvp_info->allocated_bitrate + delta) {
	return(NO_BITS);
    }

    if(is_subinterface(idb) && hwidb_or_null(idb)) {
	
	/*
	 * sub-interface - make sure there's room on the interface
	 */
	idb = firstsw_or_null(hwidb_or_null(idb));
	if(idb) {
	    rsvp_info = idb->rsvp_info;
	    if (rsvp_info) {
		if(rsvp_info->maximum_flow_bandwidth < rsvp_get_flow_requested_rate(flowspec)) {
		    return(NO_BITS);
		}
		if(rsvp_info->maximum_bandwidth <
		   rsvp_info->allocated_bitrate + delta) {
		    return(NO_BITS);
		}
	    }
	}
    }
    return(YES_BITS);
}

/*
 * rsvp_update_reservation
 *
 * we are in receipt of a resv message with a valid header.
 *
 * RESV messages carry reservation requests hop-by-hop from
 * receivers to senders, along the reverse paths of data flow for
 * the session. The IP destination address of a RESV message is
 * the unicast address of a previous-hop node, obtained from the
 * path state. The IP source address is an address of the node
 * that sent the message. The NHOP(i.e., the RSVP_HOP) object
 * must contain the IP address of the (incoming) interface through
 * which the RESV message is sent.
 */
static void
rsvp_update_reservation (psbtype * first_psb,
			 rsvp_session_head * head,
			 rsvp_hop_ipv4 * hop,
			 idbtype * if_input,
			 rsvp_refresh * refresh,
			 rsvp_policy * policy,
			 rsvp_scope_ipv4 * scope,
			 rsvp_style * style,
			 rsvp_flowspec * flowspec,
			 rsvp_filter_ipv4 * filter,
			 rsvp_policy * flow_policy,
			 rsvp_confirm_ipv4 *confirm)
{
    rsbtype *rsb;
    rsvp_error_ipv4 error_spec;
    char *change = NULL;
    boolean new;

    switch(rsvp_session_reservation_search(&head->rsb_incoming_database,
					   &rsb, if_input, hop->neighbor,
					   style->reservation_type,
					   filter->source,
					   filter->udp_source_port)) {
    case RSVP_NO_MEMORY:
    default:
	return;

    case RSVP_NOT_FOUND:
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP: Reservation is new"));
    treat_as_new:
	new = TRUE;
	bzero(rsb, sizeof(rsb));
	rsb->head = head;
	rsb->if_input = if_input;
	rsb->hop = *hop;
	rsb->time_values = *refresh;
	rsb->style = *style;
	rsb->filter = *filter;
	if(policy)
	    rsb->policy = *policy;
	if(flow_policy)
	    rsb->policy = *flow_policy;
	bzero(&rsb->flowspec, sizeof(rsb->flowspec));

	/*
	 * note that the scope and flowspec are *not* built now; they will
	 * be found later as a "change" and added in
	 */

	if(!rsvp_rsb_insert(&head->rsb_incoming_database, rsb)) {
	    free(rsb);
	    return;
	}
	head->rsb_incoming_count++;
	TIMER_START(rsb->cleanup_timer, 4 * refresh->refresh_ms);
	if(!mgd_timer_running(&head->master) ||
	   4 * refresh->refresh_ms < mgd_timer_left_sleeping(&head->cleanup)) {

	    /*
	     * if not first in cleanup period, or not running at all,
	     * restart cleanup period
	     */
	    mgd_timer_start(&head->cleanup, 4 * refresh->refresh_ms);
	}
	break;

    case RSVP_FOUND:
	new = FALSE;
	rsb->time_values = *refresh;
	TIMER_START(rsb->cleanup_timer, 4 * refresh->refresh_ms);
	if(!mgd_timer_running(&head->master) ||
	   4 * refresh->refresh_ms < mgd_timer_left_sleeping(&head->cleanup)) {

	    /*
	     * if not first in cleanup period, or not running at all,
	     * restart cleanup period
	     */
	    mgd_timer_start(&head->cleanup, 4 * refresh->refresh_ms);
	}
	if(rsb->if_input != if_input) {
	    change = "Next Hop Interface";
	    break;
	}
	if(bcmp(&rsb->hop, hop, hop->base.length)) {
	    change = "Next Hop Address";
	    break;
	}
	if(bcmp(&rsb->flowspec, flowspec, flowspec->base.length)) {
	    change = "Flow Specification";
	    break;
	}
	if(bcmp(refresh, &rsb->time_values, refresh->base.length)) {
	    change = "Refresh Timing";
	    break;
	}
	if(policy) {
	    if(!rsb->policy.base.length ||
	       bcmp(policy, &rsb->policy, policy->base.length)) {
		change = "Policy";
		break;
	    }
	} else {
	    if(rsb->policy.base.length) {
		change = "Policy";
		break;
	    }
	}
	if(scope) {
	    if(!rsb->scope ||
	       bcmp(scope, rsb->scope, scope->base.length)) {
		change = "Scope";
		break;
	    }
	} else {
	    if(rsb->scope) {
		change = "Scope";
		break;
	    }
	}
	/*
	 * no change in the specification
	 */
	if (confirm) {
	    rsvp_build_error_ipv4(&error_spec, if_input->ip_address, 0,
			          RSVP_ERROR_CONFIRM, 1);
	    rsvp_forward_resv_confirm(head, confirm, flowspec,
				      filter, &error_spec);
	}
	return;
    };
    /*
     * at this point we know that the specification is new or changed;
     * schedule an update and figure out what the update should comprise
     */
    if (confirm)
	rsb->confirm = *confirm;
    if(change) {
	RSVP_RESV_BUGINF(head->session.destination,
		         ("\nRSVP: change in %s", change));
    }

    if(if_input != rsb->if_input ||
       hop->neighbor != rsb->hop.neighbor ||
       style->reservation_type != rsb->style.reservation_type) {

	/*
	 * remove it from this interface
	 */
	RSVP_RESV_BUGINF(head->session.destination,
		         ("\nRSVP: Reservation changed direction"));
	bzero(&rsb->flowspec, sizeof(*flowspec));
	rsvp_update_conversation(rsb);

	rsvp_rsb_delete(&head->rsb_incoming_database, rsb);
	head->rsb_incoming_count--;
	goto treat_as_new;
    } else
	switch (rsvp_has_bitrate(rsb, flowspec)) {
	case NO_BITS:
	    RSVP_RESV_BUGINF(head->session.destination, 
			     ("\nallocation failed"));
	    if (!new) {

		/*
		 * send an error on the first installation of this,
		 * but then treat it as in BLOCKADE state per the spec;
		 *
		 * it will leave blockade state when the reservation is
		 * honored or it is removed by timeout or teardown.
		 */
		rsvp_build_error_ipv4(&error_spec, if_input->ip_address, 0,
				      RSVP_ERROR_ADMISSION, 2);

		rsvp_send_resv_error(if_input, hop->neighbor, hop, 
				     &head->session, &error_spec, scope, 
				     policy, style, flowspec, filter);
	    }
	    return;

	case YES_BITS:

	yes_bits:
	    if(bcmp(&rsb->flowspec, flowspec, flowspec->base.length)) {
		/*
		 * this one changed: use rsb to find the largest on the 
		 * interface.
		 * Of course, "largest" must reflect the new flow 
		 * specification.
		 */
		rsvp_find_largest_matching_flowspec(rsb, FALSE);
		if (QOS_TSPEC == rsvp_get_flow_qos_service(&rsb->flowspec))
		    memcpy(&rsb->flowspec, flowspec, flowspec->base.length);
		/* 
		 * the "largest" may be no reservation at all, 
		 * or may need to increase 
		 */
		rsvp_merge_flow_specs(flowspec, &rsb->flowspec);
		rsvp_update_conversation(rsb);
		rsvp_calculate_wfq_weights(rsb->if_input);
		/* now remember what *this* one is, for the future */
		memcpy(&rsb->flowspec, flowspec, flowspec->base.length);

		if (new)
		  rsvp_scan_paths_for_resv(rsb);
		else
		  rsvp_remove_matching_requests(rsvp_request_database(rsb), rsb);
	    }
	    break;

	case REDUCES_BITRATE:

	  if (new)
	    goto yes_bits;

	  memcpy(&rsb->flowspec, flowspec, flowspec->base.length);
	  rsvp_update_conversation(rsb);
	  rsvp_calculate_wfq_weights(rsb->if_input);
	  rsvp_remove_matching_requests(rsvp_request_database(rsb), rsb);

	  break;
	}

    if (rsb->confirm.base.length) {
	rsvp_build_error_ipv4(&error_spec, if_input->ip_address, 0,
			      RSVP_ERROR_CONFIRM, 1);
	rsvp_forward_resv_confirm(head, confirm, flowspec,
				  filter, &error_spec);
    }
}

/*
 * rsvp_incoming_resv_message()
 *
 * we are in receipt of a resv message with a valid header.
 *
 * RESV messages carry reservation requests hop-by-hop from
 * receivers to senders, along the reverse paths of data flow for
 * the session. The IP destination address of a RESV message is
 * the unicast address of a previous-hop node, obtained from the
 * path state. The IP source address is an address of the node
 * that sent the message. The NHOP(i.e., the RSVP_HOP) object
 * must contain the IP address of the(incoming) interface through
 * which the RESV message is sent.
 */
static void
rsvp_incoming_resv_message (rsvp_header *rsvp_header_ptr,
			    rsvp_session_head * head, paktype * pak,
			    void **objects)
{
    ushort length;
    rsvp_object * object;
    psbtype *first_psb;
    rsvp_error_ipv4 error_spec;
    rsvp_hop_ipv4 *hop;
    rsvp_refresh *refresh;
    rsvp_policy *policy;
    rsvp_confirm_ipv4 *confirm;
    rsvp_scope_ipv4 *scope;
    rsvp_style *style;
    rsvp_flowspec *flowspec;
    rsvp_filter_ipv4 *filter;

    hop = objects[RSVP_CLASS_HOP];
    refresh = objects[RSVP_CLASS_TIME_VALUES]?
	objects[RSVP_CLASS_TIME_VALUES]: &rsvp_default_refresh;
    policy = objects[RSVP_CLASS_POLICY_DATA];
    confirm = objects[RSVP_CLASS_RESV_CONFIRM];
    scope = objects[RSVP_CLASS_SCOPE];
    style = objects[RSVP_CLASS_STYLE];
    flowspec = objects[RSVP_CLASS_FLOWSPEC];
    filter = objects[RSVP_CLASS_FILTER_SPEC];

    /*
     * parse initial objects of RESV message
     */

    if(!hop || !rsvp_valid_hop_ipv4(hop, pak->if_input)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid upstream hop object"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }
    if(!rsvp_valid_refresh(refresh)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid timing object"));
	goto dump_object;
    }

    if(confirm && !rsvp_valid_confirm_ipv4(confirm)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid confirm object"));
	goto dump_object;
    }

    if(scope && !rsvp_valid_scope_ipv4(scope)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid scope object"));
	goto dump_object;
    }

    if(policy && !rsvp_valid_policy(policy)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid policy object"));
	goto dump_object;
    }
    if(!style || !rsvp_valid_style(style)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid style object"));
	goto dump_object;
    }
    if (!flowspec || !rsvp_valid_flowspec(flowspec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid flowspec object"));
	goto dump_object;
    }
    if(style->reservation_type != RSVP_WF_OPTION) {
	if(!filter || !rsvp_valid_filter_ipv4(filter)) {
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV CONFIRM: invalid filter object"));
	    goto dump_object;
	}
    }
    if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	/* check for looping messages */
        return;

    RSVP_RESV_BUGINF(head->session.destination,
		     ("\nRSVP: RESV message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      hop->neighbor));
    /*
     * locate first session path block
     */
    first_psb = rsvp_session_first_path(&head->psb_database);
    if(!first_psb) {
	if(!(style->reservation_type & RSVP_SCOPE_WILD)) {
	    rsvp_build_error_ipv4(&error_spec, rsvp_ip_address(pak->if_input),
				  0, RSVP_ERROR_NO_PATH_INFORMATION, 0);
	    rsvp_send_resv_error(pak->if_input, hop->neighbor, hop,
				 &head->session, &error_spec,
				 scope, policy, style, flowspec, filter);
	}
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: no path information for %i",
			  head->session.destination));
	goto dump_object;
    };

    /*
     * valid styles for all existing reservations that match
     */
    if(rsvp_incompatible_reservation_style(head, style)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: incompatible style"));
	rsvp_build_error_ipv4(&error_spec, rsvp_ip_address(pak->if_input), 0,
			      RSVP_ERROR_INCOMPATIBLE_STYLE, 0);
	rsvp_send_resv_error(pak->if_input, hop->neighbor, hop, 
			     &head->session, &error_spec, scope, 
			     policy, style, flowspec, filter);
	return;
    }
    length = rsvp_header_ptr->rsvp_length -
    	(((uchar *) flowspec) - ((uchar *) rsvp_header_ptr));
    object = &flowspec->base;
    switch(style->reservation_type) {
    case RSVP_FF_OPTION:
	/*
	 * for each flow descriptor, install a reservation
	 */
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_flowspec(&length, &object, &flowspec))
		goto bad_ff;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_ff;
	    if(!flowspec || !filter) {
	    bad_ff:
		RSVP_RESV_BUGINF(head->session.destination,
				 ("\nRSVP RESV: Fixed Reservation incorrectly "
				  "formatted"));
		goto dump_object;
	    }
	    rsvp_update_reservation(first_psb, head, hop, pak->if_input,
				    refresh, policy, scope, style,
				    flowspec, filter, policy, confirm);
	}
	break;

    case RSVP_WF_OPTION:
	/*
	 * there is one flow descriptor; install a reservation
	 */
	filter = &wild_card_filter;	/* required filter spec */
	if(!rsvp_extract_flowspec(&length, &object, &flowspec) &&
	   !flowspec)
	    goto bad_wf;
	if(length) {
	bad_wf:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV: Wildcard Reservation incorrectly "
			      "formatted"));
	    goto dump_object;
	}
	rsvp_update_reservation(first_psb, head, hop, pak->if_input,
				refresh, policy, scope,
				style, flowspec, filter, policy, confirm);
	break;

    case RSVP_SE_OPTION:
	/*
	 * for each set of flow descriptors, install a reservation
	 */
	if(!rsvp_extract_flowspec(&length, &object, &flowspec) ||
	   !flowspec) {
	bad_se:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV: Shared Explicit Reservation "
			      "incorrectly formatted"));
	    goto dump_object;
	}
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_se;
	    if(!filter)
		goto bad_se;
	    rsvp_update_reservation(first_psb, head, hop, pak->if_input,
				    refresh, policy, scope,
				    style, flowspec, filter, policy, confirm);
	}
	break;
    }
}

/*
 * rsvp_incoming_resv_error_message()
 *
 * we are in receipt of a resv error message with a valid header.
 */
static void
rsvp_incoming_resv_error_message (rsvp_header *rsvp_header_ptr,
				  rsvp_session_head * head, paktype * pak,
			    	  void **objects)
{
    ushort length;
    rsvp_object * object;
    iphdrtype *ip = iphdrtype_start(pak);
    psbtype *first_psb;
    rsvp_error_ipv4 *error_spec;
    rsvp_scope_ipv4 *scope;
    rsvp_style *style;
    rsvp_flowspec *flowspec;
    rsvp_filter_ipv4 *filter;
    rsvp_policy *policy;
    rsvp_hop_ipv4 *hop;

    error_spec = objects[RSVP_CLASS_ERROR_SPEC];
    scope = objects[RSVP_CLASS_SCOPE];
    style = objects[RSVP_CLASS_STYLE];
    flowspec = objects[RSVP_CLASS_FLOWSPEC];
    filter = objects[RSVP_CLASS_FILTER_SPEC];
    policy = objects[RSVP_CLASS_POLICY_DATA];
    hop = objects[RSVP_CLASS_HOP];

    /*
     * parse initial objects of RESV message
     */
    if(!hop || !rsvp_valid_hop_ipv4(hop, pak->if_input)) {
	RSVP_BUGINF(head->session.destination,
		    ("\nRSVP RESV: invalid upstream hop object"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }
    if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	/* check for looping messages */
        return;
    if(!error_spec || !rsvp_valid_error_ipv4(error_spec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV ERROR: invalid error spec"));
	goto dump_object;
    }
    if(scope && !rsvp_valid_scope_ipv4(scope)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV: invalid scope object"));
	goto dump_object;
    }
    if(policy && !rsvp_valid_policy(policy)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV ERROR: invalid policy object"));
	goto dump_object;
    }
    if(!style || !rsvp_valid_style(style)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV ERROR: invalid style object"));
	goto dump_object;
    }
    if (!flowspec || !rsvp_valid_flowspec(flowspec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV CONFIRM: invalid flowspec object"));
	goto dump_object;
    }
    if(style->reservation_type != RSVP_WF_OPTION) {
	if(!filter || !rsvp_valid_filter_ipv4(filter)) {
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV CONFIRM: invalid filter object"));
	    goto dump_object;
	}
    }
    RSVP_RESV_BUGINF(head->session.destination,
		     ("\nRSVP: RESV ERROR message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      ip->srcadr));

    /*
     * locate first session path block
     */
    first_psb = rsvp_session_first_path(&head->psb_database);
    if(!first_psb) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV ERROR: no path information for %i",
			  head->session.destination));
	goto dump_object;
    };

    /*
     * parse the flow/filter part of the message
     */
    length = rsvp_header_ptr->rsvp_length -
    	(((uchar *) flowspec) - ((uchar *) rsvp_header_ptr));
    object = &flowspec->base;
    switch(style->reservation_type) {
    case RSVP_FF_OPTION:
	/*
	 * for each flow descriptor, install a reservation
	 */
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_flowspec(&length, &object, &flowspec))
		goto bad_ff;
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_ff;
	    if(!flowspec || !filter) {
	    bad_ff:
		RSVP_RESV_BUGINF(head->session.destination,
				 ("\nRSVP RESV ERROR: Fixed Filter "
				  "incorrectly formatted"));
		goto dump_object;
	    }
	    rsvp_forward_resv_error(head, scope, flowspec, filter, error_spec);
	}
	break;

    case RSVP_WF_OPTION:
	/*
	 * there is one flow descriptor; install a reservation
	 */
	filter = &wild_card_filter;	/* default filter spec */
	if(!rsvp_extract_flowspec(&length, &object, &flowspec))
	    goto bad_wf;
	if(!flowspec || !filter || length) {
	bad_wf:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV ERROR: Wildcard Filter incorrectly "
			      "formatted"));
	    goto dump_object;
	}
	rsvp_forward_resv_error(head, scope, flowspec, filter, error_spec);
	break;

    case RSVP_SE_OPTION:
	/*
	 * for each set of flow descriptors, install a reservation
	 */
	flowspec = NULL;
	if(!rsvp_extract_flowspec(&length, &object, &flowspec)) {
	bad_se:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV ERROR: Shared Filter incorrectly "
			      "formatted"));
	    goto dump_object;
	}
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_se;
	    if(!flowspec || !filter)
		goto bad_se;
	    rsvp_forward_resv_error(head, scope, flowspec, filter, error_spec);
	}
	break;
    }
}

/*
 * rsvp_incoming_resv_tear_message()
 *
 * we are in receipt of a resv teardown message with a valid header.
 */
static void
rsvp_incoming_resv_tear_message (rsvp_header *rsvp_header_ptr,
				 rsvp_session_head * head, paktype * pak,
			    	 void **objects)
{
    ushort length;
    rsvp_object * object;
    iphdrtype *ip = iphdrtype_start(pak);
    rsvp_hop_ipv4 *hop;
    rsvp_scope_ipv4 *scope;
    rsvp_style *style;
    rsvp_filter_ipv4 *filter;
    rsvp_flowspec *flowspec;
    rsbtype *rsb;

    hop = objects[RSVP_CLASS_HOP];
    scope = objects[RSVP_CLASS_SCOPE];
    style = objects[RSVP_CLASS_STYLE];
    filter = objects[RSVP_CLASS_FILTER_SPEC];
    flowspec = objects[RSVP_CLASS_FLOWSPEC];

    /*
     * parse initial objects of RESV TEAR message
     */

    if(!hop || !rsvp_valid_hop_ipv4(hop, pak->if_input)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV TEARDOWN: invalid upstream hop "
                          "object"));
    dump_object:
	RSVP_DUMP_OBJECT(rsvp_header_ptr, rsvp_detail_flag,
			 rsvp_header_ptr->rsvp_length);
	return;
    }
    if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	/* check for looping messages */
        return;
    if(scope && !rsvp_valid_scope_ipv4(scope)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV TEARDOWN: invalid scope object"));
	goto dump_object;
    }
    if(!style || !rsvp_valid_style(style)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV TEARDOWN: invalid style object"));
	goto dump_object;
    }
    if (flowspec && !rsvp_valid_flowspec(flowspec)) {
	RSVP_RESV_BUGINF(head->session.destination,
			 ("\nRSVP RESV TEARDOWN: invalid flowspec object"));
	goto dump_object;
    }
    if(style->reservation_type != RSVP_WF_OPTION) {
	if(!filter || !rsvp_valid_filter_ipv4(filter)) {
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV TEARDOWN: invalid filter object"));
	    goto dump_object;
	}
    }
    RSVP_RESV_BUGINF(head->session.destination,
		     ("\nRSVP: RESV TEAR message for %i (%s) from %i",
		      head->session.destination, pak->if_input->namestring,
		      ip->srcadr));

    if (flowspec) {
	length = rsvp_header_ptr->rsvp_length -
	    (((uchar *) flowspec) - ((uchar *) rsvp_header_ptr));
	object = &flowspec->base;
    } else {
	length = rsvp_header_ptr->rsvp_length -
	    (((uchar *) filter) - ((uchar *) rsvp_header_ptr));
	object = &filter->base;
    }
    switch(style->reservation_type) {
    case RSVP_FF_OPTION:
	/*
	 * for each flow descriptor, install a reservation
	 */
	while(length) {
	    flowspec = NULL;
	    if(!rsvp_extract_flowspec(&length, &object, &flowspec))
		goto bad_ff;
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_ff;
	    if(!filter) {
	      bad_ff:
		RSVP_RESV_BUGINF(head->session.destination,
				 ("\nRSVP RESV TEARDOWN: Fixed Filter "
				  "incorrectly formatted"));
		goto dump_object;
	    }
	    switch(rsvp_session_reservation_search(&head->rsb_incoming_database,
						   &rsb,
						   pak->if_input,
						   hop->neighbor,
						   style->reservation_type,
						   filter->source,
						   filter->udp_source_port)) {
	    case RSVP_NO_MEMORY:
		break;

	    case RSVP_NOT_FOUND:
		free(rsb);
		break;

	    case RSVP_FOUND:
		/*
		 * force timer expiration
		 */
		TIMER_START(rsb->cleanup_timer, 0);
		rsvp_rsb_expiry(rsb->threads, NULL);
		break;
	    }
	}
	break;

    case RSVP_WF_OPTION:
	filter = &wild_card_filter;	/* default filter spec */
	/*
	 * there is one flow descriptor; install a reservation
	 */
	if(!rsvp_extract_flowspec(&length, &object, &flowspec))
	    goto bad_wf;
	if (!(pak->flags & PAK_SENDSELF) && ip_ouraddress(hop->neighbor))
	    /* check for looping messages */
	    return;
	rsvp_extract_filter_ipv4(&length, &object, &filter);
	if(!filter || length) {
	bad_wf:

	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV TEARDOWN: Wildcard Filter "
			      "incorrectly formatted"));
	    goto dump_object;
	}
	switch(rsvp_session_reservation_search(&head->rsb_incoming_database,
					       &rsb,
					       pak->if_input, hop->neighbor,
					       style->reservation_type,
					       filter->source,
					       filter->udp_source_port)) {
	case RSVP_NO_MEMORY:
	    break;

	case RSVP_NOT_FOUND:
	    free(rsb);
	    break;

	case RSVP_FOUND:
	    /*
	     * force timer expiration
	     */
	    TIMER_START(rsb->cleanup_timer, 0);
	    (void) rsvp_rsb_expiry(rsb->threads, NULL);
	    break;
	}
	break;

    case RSVP_SE_OPTION:
	/*
	 * for each set of flow descriptors, install a reservation
	 */
	flowspec = NULL;
	if(!rsvp_extract_flowspec(&length, &object, &flowspec)) {
	bad_se:
	    RSVP_RESV_BUGINF(head->session.destination,
			     ("\nRSVP RESV TEARDOWN: Shared Filter "
			      "incorrectly formatted"));
	    goto dump_object;
	}
	while(length) {
	    filter = NULL;
	    if(!rsvp_extract_filter_ipv4(&length, &object, &filter))
		goto bad_se;
	    if(!filter)
		goto bad_se;
	    switch(rsvp_session_reservation_search(&head->rsb_incoming_database,
						   &rsb,
						   pak->if_input,
						   hop->neighbor,
						   style->reservation_type,
						   filter->source,
						   filter->udp_source_port)) {
	    case RSVP_NO_MEMORY:
		break;

	    case RSVP_NOT_FOUND:
		free(rsb);
		break;

	    case RSVP_FOUND:
		/*
		 * force timer expiration
		 */
		TIMER_START(rsb->cleanup_timer, 0);
		rsvp_rsb_expiry(rsb->threads, NULL);
		break;
	    }
	}
	break;
    }
}

/*
 * valid header and message
 */
static boolean
rsvp_valid_message (rsvp_header *header, ushort length,
		    iphdrtype *ip, void **objects)
{
    ushort checksum;
    rsvp_object *object;
    boolean print_detail_flag;

    print_detail_flag = rsvp_print_detail(header->message_type,
					  rsvp_detail_flag);

    if(rsvp_debug && print_detail_flag) {
	buginf("\nRSVP: message received from %i", ip->srcadr);
    }
    /*
     * valid the RSVP header
     */
    if(length != header->rsvp_length) {
	RSVP_BUGINF(0,("\nRSVP: Data Field Length != RSVP Message Length"));
	return FALSE;
    }
    if(RSVP_VERSION != header->version) {
	RSVP_BUGINF(0,("\nRSVP: incorrect version"));
	RSVP_DUMP_OBJECT(header, rsvp_detail_flag, header->rsvp_length);
	return FALSE;
    }
    /*
     * Checksum the message
     */
    checksum = header->cksum;
    if(checksum) {
	ushort foo;

	header->cksum = 0;
	if(checksum !=(foo = ipcrc((ushort *) header, length))) {
	    RSVP_BUGINF(0,("\nRSVP: invalid checksum %x s/b %x", 
			   checksum, foo));
	    return FALSE; 
	}
    }
    if(header->flag0 || header->flag1 || header->flag2 || header->flag3 ||
       header->reserved_byte) {
	RSVP_BUGINF(0,("\nRSVP: reserved field not zero"));
	RSVP_DUMP_OBJECT(header, rsvp_detail_flag, header->rsvp_length);
	return FALSE;
    }
    if(header->message_type < RSVP_MSG_PATH ||
       RSVP_MSG_CONFIRM < header->message_type) {
	RSVP_BUGINF(0,("\nRSVP: undefined message type"));
        RSVP_DUMP_OBJECT(header, rsvp_detail_flag, header->rsvp_length);
	return FALSE;
    }
#ifdef DRAFT10
    /*
     * for now, disable fragmentation
     */
    if(header->fragment) {
	RSVP_BUGINF(0,("\nRSVP: fragmentation unimplemented"));
	return FALSE;
    }
#endif
    /*
     * check that
     * - there is at least one object
     * - the lengths of the objects are positive,
     * - the lengths of the objects add up, and
     * - the objects are multiples of 32 bit words
     */
    length -= sizeof(rsvp_header);
    if(length <= 0) {
	return FALSE;
    }
    object = header->rsvp_first_object;
    while(length > 0) {
	if(object->length == 0) {
	    RSVP_BUGINF(0,("\nRSVP: zero length object"));
	    return FALSE;
	}
	if(object->length % 4 != 0) {
	    RSVP_BUGINF(0,("\nRSVP: object not a multiple of four bytes"));
	    return FALSE;
	}
	if(object->length > length) {
	    RSVP_BUGINF(0,("\nRSVP: last object not fully contained in "
			   "message"));
	    return FALSE;
	}
	if(RSVP_CLASS_MAX < object->class) {
	    RSVP_BUGINF(0,("\nRSVP: unrecognized object class"));
	    return FALSE;
	}
	if (!objects[object->class])
	    objects[object->class] = object;
	length -= object->length;
	object = rsvp_next_object(object);
    }

    RSVP_DUMP_OBJECT(header, rsvp_detail_flag,
		     header->rsvp_length - sizeof(rsvp_header));
    return TRUE;
}

/*
 * basic incoming message validation and discriminiation.
 *
 * make sure the message is valid, and then pass it to whoever wants it
 */
void
rsvp_input_message (paktype * pak, rsvp_header * rsvp, ushort length)
{
    iphdrtype *ip = iphdrtype_start(pak);
    rsvp_object *object;
    rsvp_session_head *head;
    rsvp_session_ipv4 *session = NULL;
    void *objects[RSVP_CLASS_MAX+1];

    bzero(objects, sizeof(objects));
    if(!rsvp_valid_message(rsvp, length, ip, objects)) {
	return;
    }
    rsvp_neighbor(pak);
    object = rsvp_first_object(rsvp, &length);

    session = objects[RSVP_CLASS_SESSION];
    if(!session || !rsvp_valid_session_ipv4(session))
	return;

    switch(rsvp_session_head_search(&head, session->destination,
				    session->protocol,
				    session->udp_dest_port)) {
    case RSVP_NO_MEMORY:
	return;				/* the ultimate end case */

    case RSVP_FOUND:
	if (head->session.udp_dest_port != session->udp_dest_port)
	    return;
	break;				/* head-> session context */

    case RSVP_NOT_FOUND:
	rsvp_session_head_init(head, session->destination,
			       session->protocol,
			       session->udp_dest_port);
	if(!wavl_insert(&rsvp_session_database, head->threads)) {
	    free(head);
	    return;
	}
	break;
    }
    switch(rsvp->message_type) {
    case RSVP_MSG_PATH:
	rsvp_incoming_path_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_RESV:
	rsvp_incoming_resv_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_PERR:
	rsvp_incoming_path_error_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_RERR:
	rsvp_incoming_resv_error_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_PTEAR:
	rsvp_incoming_path_tear_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_RTEAR:
	rsvp_incoming_resv_tear_message(rsvp, head, pak, objects);
	break;
    case RSVP_MSG_CONFIRM:
	rsvp_incoming_resv_confirm_message(rsvp, head, pak, objects);
	break;
    }
    rsvp_session_done(head);
}

/*
 * rsvp_print_detail
 *
 * Return TRUE if we need to print detailed debugging info. 
 *
 * The function will return TRUE if the appropriate debugging flag
 * for the particular message type is on.
 */
boolean rsvp_print_detail (rsvp_message_types msg_type, boolean detail_flag)
{
    boolean print_detail_flag, is_path_msg;

    is_path_msg = (msg_type == RSVP_MSG_PATH) || 
	          (msg_type == RSVP_MSG_PERR) ||
	          (msg_type == RSVP_MSG_PTEAR);
		  
    /*
     * See if appropriate debugging has been enabled.
     *
     * Assume that anything that's not a PATH-related message is a
     * RESV-related message,
     */
    print_detail_flag = (detail_flag == RSVP_DEBUG_ENABLE) ||
	                 ((detail_flag == RSVP_RESV_DEBUG_FLAG) && 
		           !is_path_msg) || 
	                 ((detail_flag == RSVP_PATH_DEBUG_FLAG) &&
		           is_path_msg);

    return(print_detail_flag);
}

void
rsvp_clean_session_heads(void)
{
    rsvp_session_head *head, *prev_head = NULL;

    /*
     * Note that invoking rsvp_session_done might not remove the session 
     * head from the rsvp_session_database, because all databases hanging
     * off of this structure may be non-empty. This precludes the use of
     * a loop of the rsvp_first_session kind. And just in case it gets 
     * removed, we need to mem_lock it so that we can use that as a dummy
     * to get the next session head
     */
    head = rsvp_first_session(&rsvp_session_database);
    if (head == NULL)
	return;
    prev_head = head;
    mem_lock(head);
    rsvp_session_done(head);
    while ((head = rsvp_next_session(&rsvp_session_database, prev_head)) 
	   != NULL) {
	free(prev_head);
	prev_head = head;
	mem_lock(head);
	rsvp_session_done(head);
    }
    free(prev_head);
    return;
}

boolean
rsvp_clr_ip_resv(wavl_node_type * node, va_list argptr)
{
    rsvp_session_head *head =(rsvp_session_head *) node;
    rsbtype *rsb;
    
    /*
     * Note that we cannot use the wavl_walk function here since it does
     * not allow deletion of all elements of the tree during a single walk
     */
    while ((rsb = rsvp_session_first_reservation(&head->rsb_incoming_database))
	   != NULL)
	rsvp_flush_rsb_incoming((wavl_node_type *)rsb, argptr);
    process_may_suspend();
    return TRUE;
}

boolean
rsvp_clr_ip_sndr(wavl_node_type * node, va_list argptr)
{
    rsvp_session_head *head =(rsvp_session_head *) node;
    psbtype *psb;

    /*
     * Note that we cannot use the wavl_walk function here since it does
     * not allow deletion of all elements of the tree during a single walk
     */
    while ((psb = rsvp_session_first_path(&head->psb_database)) !=NULL)
	rsvp_flush_psb((wavl_node_type *)psb, argptr);
    process_may_suspend();
    return TRUE;
}
