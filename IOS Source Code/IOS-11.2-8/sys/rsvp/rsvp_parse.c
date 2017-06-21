/* $Id: rsvp_parse.c,v 1.1.4.21 1996/08/24 01:16:47 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_parse.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_parse.c,v $
 * Revision 1.1.4.21  1996/08/24  01:16:47  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.4.20  1996/08/20  10:24:37  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.19  1996/08/07  23:19:04  fred
 * CSCdi65440:  Spurious memory access made at 0x6016B158 reading 0x1
 *         that happens when you use an integer as an address...
 * Branch: California_branch
 *
 * Revision 1.1.4.18  1996/08/07  09:02:14  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 1.1.4.17  1996/07/19  21:04:42  asastry
 * CSCdi63538:  Misc. display changes (make 'show ip rsvp neighbor' command
 * display interface number as well)
 * Branch: California_branch
 *
 * Revision 1.1.4.16  1996/07/19  20:00:20  fred
 * CSCdi63538:  change next hop to previous hop on ip rsvp sender display
 *              make the columns line up a little better
 * Branch: California_branch
 *
 * Revision 1.1.4.15  1996/07/19  19:54:26  fred
 * CSCdi63538:  change next hop to previous hop on ip rsvp sender display
 * Branch: California_branch
 *
 * Revision 1.1.4.14  1996/07/19  06:09:31  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.13  1996/07/16  23:29:41  fred
 * CSCdi63162:  RSVP diagnostic message needed in rejecting cases.
 *         Per code review comments, say "non-zero" instead of "!= 0"
 * Branch: California_branch
 *
 * Revision 1.1.4.12  1996/07/16  22:49:26  fred
 * CSCdi63162:  RSVP diagnostic message needed in rejecting cases
 *         Do not correct cases that fail the test in section 2.2 of the
 *         spec,
 *         reject the command.
 * Branch: California_branch
 *
 * Revision 1.1.4.11  1996/07/09  05:39:42  fred
 * CSCdi61493:  Cant remove statically configured reservation
 * Branch: California_branch
 *
 * Revision 1.1.4.10  1996/06/22  08:09:24  fred
 * CSCdi58184:  Reservation with bandwidth > flow max is incorrectly
 *         installed on first attempt. Correct analysis of first
 *         conversation's requirements.
 * Branch: California_branch
 *
 * Revision 1.1.4.9  1996/06/21  19:05:27  fred
 * CSCdi57507:  RSVP gets installed with Conversation number 0 on ethernet.
 *         Install registry callback from WFQ setup to RSVP to make sure
 *         that some reservable queues are allocated. Also, do not display
 *         weights and conversations unless WFQ invoked on interface
 * Branch: California_branch
 *
 * Revision 1.1.4.8  1996/06/18  11:05:48  fred
 * CSCdi53943:  box crashes when removing RSVP from an interface
 *              Add defensive code to avoid null dereferences, and
 *              improve handling of interface and topology changes
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/06/05  21:11:49  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/05/21  10:01:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.5  1996/05/17  11:40:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.18.1  1996/05/05  23:45:26  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 1.1.4.4  1996/05/13  16:53:08  fred
 * CSCdi57496:  ip rsvp reservation command parameter in wrong place
 *              while we're at it, correct oversight in command defensive
 *              code
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/13  16:39:00  fred
 * CSCdi57496:  ip rsvp reservation command parameter in wrong place
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/06  17:46:36  tylin
 * CSCdi55983:  Spurious messages seen with RSVP
 * Branch: California_branch
 *   Fix Null pointer reference.
 *
 * Revision 1.1.4.1  1996/04/17  13:57:23  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "logger.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../os/signal.h"
#include "../os/techsupport.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../ip/ip_registry.h"
#include "../if/priority_private.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"
#include "../ipmulticast/igmp.h"

pid_t rsvp_pid;
#define kbps(x) ((x + 999)/1000)

/*
 * display varying numbers
 */
static
void rsvp_kbps_display (ulong number, char *string)
{
    if (0 == number % 1000000)
	sprintf(string, "%dM", (number + 999999)/1000000);
    else if (0 == number % 1000)
	sprintf(string, "%dK", (number + 999)/1000);
    else
	sprintf(string, "%d", number);
}

/*
 * callback for fixing up WFQ interfaces for RSVP
 */

#define RSVP_EXPECTED_BPS_FOR_SESSION 32000
void rsvp_fixup_wfq (hwidbtype *hwidb)
{
    extern void shape_adjust_queue (hwidbtype *hwidb);
    idbtype *swidb;
    fair_queue_head *fq;
    rsvp_info_type *rsvp_info;
    rsvp_flow_ipv4 *conversation;
    ushort hashed_queues;
    ushort control_queues;
    ushort reserved_queues;
    ushort discard_level;
    short i;

    if (!hwidb)				/* if no hwidb, why are we here? */
	return;
    fq = hwidb->fair_queue_head;
    if (!fq)				/* if not WFQ, get out */
	return;
    swidb = firstsw_or_null(hwidb);
    if (!swidb)				/* if no swidb, we need to talk... */
	return;
    rsvp_info = swidb->rsvp_info;
    if (!rsvp_info)			/* if no RSVP, no need */
	return;

    hashed_queues = fq->fq_hash_mask + 1;
    control_queues = 1 + FAIR_LINK_TYPE_HASH;
    discard_level = fq->fq_max_enqueue;
    if (hashed_queues + control_queues == fq->fq_queue_count) {

	/*
	 * it's a WFQ interface in default configuration, and RSVP is
	 * to be enabled on it. Configure an appropriate number of queues
	 */
	reserved_queues = 
	    rsvp_info->maximum_bandwidth/RSVP_EXPECTED_BPS_FOR_SESSION;
	reserved_queues = max(1000, min(reserved_queues,1));
	delete_fair_queue(hwidb);
	build_fair_queue_head(hwidb, discard_level, hashed_queues, 
			      reserved_queues);
	for (i = RSVP_FLOW_HASH; --i >= 0; )
	    for (conversation = rsvp_info->flows[i];
		 conversation; conversation = conversation->next)
		conversation->conversation = fair_reserve_conversation(hwidb);
	rsvp_calculate_wfq_weights(swidb);
    }

    shape_adjust_queue(hwidb);
}

/*
 *------------------------------------------------------------------
 * Walking Routines for Show Commands
 *------------------------------------------------------------------
 */

/*
 * show reservations
 */
static void
rsvp_show_reservation_rsb (parseinfo *csb, rsbtype * rsb)
{
    char rate[20];
    char burst[20];
    rsvp_session_head *head = rsb->head;
    char protocol[4];
    char delay_service[8];
    char *service;
    char *style;

    switch (head->session.protocol) {
    default:
	sprintf(protocol, "%3d", head->session.protocol);
	break;
    case TCP_PROT:
	strcpy(protocol, "TCP");
	break;
    case UDP_PROT:
	strcpy(protocol, "UDP");
	break;
    }

    switch (rsb->style.reservation_type) {
    case RSVP_FF_OPTION:
	style = "FF";
	break;
    case RSVP_SE_OPTION:
	style = "SE";
	break;
    case RSVP_WF_OPTION:
	style = "WF";
	break;
    default:
	style = "UNK";
	break;
    }

    switch (rsvp_get_flow_qos_service(&rsb->flowspec)) {
    case QOS_CONTROLLED_LOAD:
	service = "LOAD";
	break;
    case QOS_GUARANTEED:
	service = "RATE";
	break;
    case QOS_PREDICTIVE:
	service = "PRDC";
	break;
    case QOS_CONTROLLED_DELAY:
	sprintf(delay_service, "DE %d", rsvp_get_flow_service_level(&rsb->flowspec));
	service = delay_service;
	break;
    default:
	if (0 == rsvp_get_flow_average_bit_rate(&rsb->flowspec))
	    service = "BLKD";
	else
	    service = "UNKN";
	break;
    }

    if (!csb) {
	rsvp_kbps_display(rsvp_get_flow_average_bit_rate(&rsb->flowspec), rate),
	rsvp_kbps_display(rsvp_get_flow_maximum_burst(&rsb->flowspec)/8, burst);
	printf("\n%13i %13i %3s %5d %5d %13i %5s %2s %s %5s %s",
	       head->session.destination,
	       rsb->filter.source,
	       protocol,
	       head->session.udp_dest_port,
	       rsb->filter.udp_source_port,
	       rsb->hop.neighbor,
	       rsb->if_input->short_namestring,
	       style,
	       service,
	       rate,
	       burst);
    } else {
	nv_write(TRUE,
		 "%s %i %i %s %d %d %i %s %s %s %d %d",
		 csb->nv_command,
		 head->session.destination,
		 rsb->filter.source,
		 protocol,
		 head->session.udp_dest_port,
		 rsb->filter.udp_source_port,
		 rsb->hop.neighbor,
		 rsb->if_input->short_namestring,
		 style,
		 service,
		 kbps(rsvp_get_flow_average_bit_rate(&rsb->flowspec)),
		 kbps(rsvp_get_flow_maximum_burst(&rsb->flowspec))/8);
    }
}

static void
rsvp_show_reservation_head (parseinfo *csb, wavl_handle * head)
{
    rsbtype *rsb;

    for (rsb = rsvp_session_first_reservation(head);
	 rsb && (!csb || !automore_quit());
	 rsb = rsvp_session_next_reservation(head, rsb)) {
	rsvp_show_reservation_rsb(csb, rsb);
    }
}

/*
 * show path state information
 */
static void
rsvp_show_sender_psb (parseinfo *csb, psbtype * psb)
{
    char rate[20];
    char burst[20];
    rsvp_session_head *head = psb->head;
    char protocol[4];

    switch (head->session.protocol) {
    default:
	sprintf(protocol, "%3d", head->session.protocol);
	break;
    case TCP_PROT:
	strcpy(protocol, "TCP");
	break;
    case UDP_PROT:
	strcpy(protocol, "UDP");
	break;
    }

    if (!csb) {
	rsvp_kbps_display(rsvp_get_average_bit_rate(&psb->traffic_specification), rate);
	rsvp_kbps_display(rsvp_get_maximum_burst(&psb->traffic_specification)/8, burst);
	printf("\n%15i %15i %3s %5d %5d %15i %5s %6s %s",
	       head->session.destination,
	       psb->template.source,
	       protocol,
	       head->session.udp_dest_port,
	       psb->template.udp_source_port,
	       psb->hop.neighbor,
	       psb->if_input->short_namestring,
	       rate,
	       burst);
    } else
	nv_write(TRUE,
		 "%s %i %i %s %d %d %i %s %d %d",
		 csb->nv_command,
		 head->session.destination,
		 psb->template.source,
		 protocol,
		 head->session.udp_dest_port,
		 psb->template.udp_source_port,
		 psb->hop.neighbor,
		 psb->if_input->short_namestring,
		 kbps(rsvp_get_average_bit_rate(&psb->traffic_specification)),
		 kbps(rsvp_get_maximum_burst(&psb->traffic_specification))/8);
}

static void rsvp_show_sender_head (parseinfo *csb, wavl_handle * head)
{
    psbtype *psb;

    for (psb = rsvp_session_first_path(head);
	 psb && (!csb || !automore_quit());
	 psb = rsvp_session_next_path(head, psb)) {
	rsvp_show_sender_psb(csb, psb);
    }
}

/*
 *------------------------------------------------------------------
 * API
 *------------------------------------------------------------------
 *
 *    These routines are written to be used by the command
 *    responders *and* by processes that want RSVP to make
 *    reservations on their behalf
 */

/*
 *	rsvp_add_ip_resv_api
 *
 *	Add a reservation as though it had been received
 */
static boolean
rsvp_add_ip_resv_api (ipaddrtype destination,
		      ipaddrtype source,
		      uchar ip_protocol,
		      ushort udp_dest_port,
		      ushort udp_source_port,
		      ipaddrtype neighbor,
		      idbtype *idb,
		      ulong reservation_type,
		      ulong average_bit_rate /* kilobits per second */,
		      ulong maximum_burst /* kbytes */,
		      enum qos_service_type qos_service,
		      ulong service_level)
{
    rsvp_session_head *head;
    rsbtype *rsb;

    if (!idb->rsvp_info)
	return FALSE;

    if (!neighbor)
	neighbor = rsvp_ip_address(idb);

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol,
				    udp_dest_port)) {
    case RSVP_NO_MEMORY:
        return FALSE;			/* the ultimate end case */

    case RSVP_FOUND:
	if (head->session.udp_dest_port != udp_dest_port)
	    return FALSE;
        break;				/* head-> session context */

    case RSVP_NOT_FOUND:
        rsvp_session_head_init(head, destination,
                               ip_protocol, udp_dest_port);
        if(!wavl_insert(&rsvp_session_database, head->threads)) {
            free(head);
            return FALSE;
        }
        break;
    }

    switch (rsvp_session_reservation_search(&head->rsb_incoming_database,
					    &rsb, idb, neighbor,
					    reservation_type, source,
					    udp_source_port)) {
    case RSVP_NO_MEMORY:
	return FALSE;
    case RSVP_NOT_FOUND:
	rsb->head = head;
	rsb->if_input = idb;
	rsvp_build_refresh(&rsb->time_values, RSVP_REFRESH_MS);
	if (reservation_type != RSVP_WF_OPTION)
	    rsvp_build_filter_ipv4(&rsb->filter, source, udp_source_port);
	maximum_burst *= 8*1000;	/* convert from kbytes to bits */
	average_bit_rate *= 1000;	/* convert from kbps to bps */
	rsvp_build_flowspec(&rsb->flowspec, qos_service, maximum_burst,
			    average_bit_rate, 500000, service_level, 0,
			    idb->hwptr->maxmtu, average_bit_rate,
			    average_bit_rate, 0, idb->hwptr->maxmtu);
	rsvp_build_hop_ipv4(&rsb->hop, neighbor, NULL);
	rsvp_build_style(&rsb->style, reservation_type);
	mgd_timer_init_leaf(&rsb->refresh_timer, &head->master,
			    TIMER_TYPE_STATIC_RESV, rsb, FALSE);
        if(!rsvp_rsb_insert(&head->static_rsb_database, rsb)) {
            /*
             * insert failed
             */
            free(rsb);
            return FALSE;
        }
	head->static_rsb_count++;
	rsvp_trigger_resv_refresh(rsb);
	break;
    case RSVP_FOUND:
	return FALSE;
    }
    return TRUE;
}

/*
 * remove a permanent reservation
 */
static boolean
rsvp_delete_ip_resv_api (ipaddrtype destination,
			 ipaddrtype source,
			 uchar ip_protocol,
			 ushort udp_dest_port,
			 ushort udp_source_port,
			 ipaddrtype neighbor,
			 idbtype *idb,
			 ulong reservation_type)
{
    rsvp_session_head *head;
    rsbtype *rsb;

    if (!idb->rsvp_info)
	return FALSE;

    if (!neighbor)
	neighbor = rsvp_ip_address(idb);

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol,
				    udp_dest_port)) {
    case RSVP_NO_MEMORY:
        return FALSE;			/* the ultimate end case */

    case RSVP_FOUND:
	if (head->session.udp_dest_port != udp_dest_port)
	    return FALSE;
        break;				/* head-> session context */

    case RSVP_NOT_FOUND:
	free(head);
	return TRUE;
    }

    /*
     * delete static rsb
     */
    switch (rsvp_session_reservation_search(&head->static_rsb_database,
					    &rsb, idb, neighbor,
					    reservation_type, source,
					    udp_source_port)) {
    case RSVP_NO_MEMORY:
	break;

    case RSVP_NOT_FOUND:
	free(rsb);
	break;

    case RSVP_FOUND:
	rsvp_stop_resv_refresh(rsb);
	rsvp_rsb_delete(&head->static_rsb_database, rsb);
	free(rsb);
	head->static_rsb_count--;
	break;
    }

    /*
     * delete active reservation if one exists
     */
    switch (rsvp_session_reservation_search(&head->rsb_incoming_database,
					    &rsb, idb, neighbor,
					    reservation_type, source,
					    udp_source_port)) {
    case RSVP_NO_MEMORY:
	return FALSE;
    case RSVP_NOT_FOUND:
	free(rsb);
	return TRUE;
	break;
    case RSVP_FOUND:
	TIMER_START(rsb->cleanup_timer, 0);
	rsvp_rsb_expiry(rsb->threads, NULL);
	break;
    }

    rsvp_session_done(head);
    return TRUE;
}

static boolean
rsvp_add_ip_path_api (ipaddrtype destination,
		      ipaddrtype source,
		      uchar ip_protocol,
		      ushort udp_dest_port,
		      ushort udp_source_port,
		      ipaddrtype neighbor,
		      idbtype *idb,
		      ulong average_bit_rate /* kilobits per second */,
		      ulong maximum_burst /* kbytes */)
{
    rsvp_session_head *head;
    psbtype *psb;

    if (!idb->rsvp_info)
	return FALSE;

    if (!neighbor)
	neighbor = rsvp_ip_address(idb);

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol,
				    udp_dest_port)) {
    case RSVP_NO_MEMORY:
        return FALSE;			/* the ultimate end case */

    case RSVP_FOUND:
	if (head->session.udp_dest_port != udp_dest_port)
	    return FALSE;
        break;				/* head-> session context */

    case RSVP_NOT_FOUND:
        rsvp_session_head_init(head, destination,
                               ip_protocol, udp_dest_port);
        if(!wavl_insert(&rsvp_session_database, head->threads)) {
            free(head);
            return FALSE;
        }
        break;
    }

    switch (rsvp_session_path_search(&head->static_psb_database, &psb,
				     source, udp_source_port)) {
    case RSVP_NO_MEMORY:
	return FALSE;
    case RSVP_NOT_FOUND:
	psb->head = head;
	psb->if_input = idb;
	rsvp_build_refresh(&psb->time_values, RSVP_REFRESH_MS);
	rsvp_build_template_ipv4(&psb->template, source, udp_source_port);
	rsvp_build_hop_ipv4(&psb->hop, neighbor, NULL);
	rsvp_build_tspec(&psb->traffic_specification,
			 average_bit_rate*1000, maximum_burst*8000,
			 idb->visible_bandwidth*1000,
			 0, max_proto_mtu(idb, LINK_IP));
	rsvp_build_adspec(&psb->advertisement);
        if(!rsvp_psb_insert(&head->static_psb_database, psb)) {

            /*
             * insert failed
             */
            free(psb);
            return FALSE;
        }
	mgd_timer_init_leaf(&psb->refresh_timer, &head->master,
			    TIMER_TYPE_STATIC_PATH, psb, FALSE);
	psb->no_ra_retry_count = PSB_NO_RA_RETRIES;
	rsvp_trigger_path_refresh(psb);
        head->static_psb_count++;
	break;
    case RSVP_FOUND:
	return FALSE;
    }
    return TRUE;
}

static boolean
rsvp_delete_ip_path_api (ipaddrtype destination,
			 ipaddrtype source,
			 uchar ip_protocol,
			 ushort udp_dest_port,
			 ushort udp_source_port)
{
    rsvp_session_head *head;
    psbtype *psb;

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol, udp_dest_port)) {
    case RSVP_NO_MEMORY:
        return FALSE;			/* the ultimate end case */

    case RSVP_FOUND:
	if (head->session.udp_dest_port != udp_dest_port)
	    return FALSE;
        break;				/* head-> session context */

    case RSVP_NOT_FOUND:
	free(head);
	return TRUE;
    }

    switch (rsvp_session_path_search(&head->static_psb_database,
				     &psb, source, udp_source_port)) {
    case RSVP_NO_MEMORY:
	return FALSE;
    case RSVP_NOT_FOUND:
	free(psb);
	break;
    case RSVP_FOUND:
	rsvp_psb_delete(&head->static_psb_database, psb);
	head->static_psb_count--;
	rsvp_stop_path_refresh(psb);
	break;
    }

    switch (rsvp_session_path_search(&head->psb_database,
				     &psb, source, udp_source_port)) {
    case RSVP_NO_MEMORY:
	return FALSE;
    case RSVP_NOT_FOUND:
	free(psb);
	return TRUE;
	break;
    case RSVP_FOUND:
	TIMER_START(psb->cleanup_timer, 0);
	rsvp_psb_expiry(psb->threads, NULL);
	rsvp_session_done(head);
	break;
    }
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * Command Handlers
 *------------------------------------------------------------------
 */

/************************************************************************
 * ip rsvp neighbor <access-list>
 * no ip rsvp neighbor <access-list>
 *
 * OBJ(int,1) = Access List
 */
void rsvp_nbr_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    rsvp_info_type *rsvp_info = idb->rsvp_info;

    if (!rsvp_info) {

	/*
	 * RSVP not configured - bail
	 */
	if (!csb->nvgen && csb->sense)
	    printf("\nRSVP: must configure RSVP Bandwidth first");

    } else if (csb->nvgen) {

	/*
	 * store configuration
	 */
	if (rsvp_info->nbr_acl)
	    nv_write(TRUE, "%s %d", csb->nv_command, rsvp_info->nbr_acl);

    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	rsvp_info->nbr_acl = 0;

    } else {

	/*
	 * install active configuration
	 */
	rsvp_info->nbr_acl = GETOBJ(int, 1);
    }
}

/************************************************************************
 * ip rsvp udp-multicasts [<multicast-address>]
 * no ip rsvp udp-multicasts [<multicast-address>]
 *
 * OBJ(paddr,1) = UDP Multicast Address
 */

void rsvp_udp_rsvp_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    rsvp_info_type *rsvp_info = idb->rsvp_info;

    if (!rsvp_info) {

	/*
	 * RSVP not configured - bail
	 */
	if (!csb->nvgen && csb->sense)
	    printf("\nRSVP: must configure RSVP Bandwidth first");

    } else if (csb->nvgen) {

	/*
	 * store configuration
	 */
	nv_write(rsvp_info->udp_required, "%s %i", 
		 csb->nv_command, rsvp_info->udp_group);

    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	if (IPMULTICAST_RSVD(rsvp_info->udp_group))
	    ip_multicast_ctl(idb, rsvp_info->udp_group, FALSE);
	else
	    igmp_delete_group(rsvp_info->udp_group, idb);
	rsvp_info->udp_required = FALSE;
	rsvp_info->udp_group = RSVP_ENCAP_1;
	ip_multicast_ctl(idb, rsvp_info->udp_group, TRUE);

    } else {

	/*
	 * Validate IP Multicast Address
	 */
	if (GETOBJ(paddr,1)->ip_addr && 
	    !IPMULTICAST(GETOBJ(paddr,1)->ip_addr)) {
	    printf("\n the address %i is not an IP Multicast Address", 
		   GETOBJ(paddr,1)->ip_addr);
	    return;
	}

	/*
	 * install active configuration
	 */
	rsvp_info->udp_required = TRUE;
	if (IPMULTICAST_RSVD(rsvp_info->udp_group))
	    ip_multicast_ctl(idb, rsvp_info->udp_group, FALSE);
	else
	    igmp_delete_group(rsvp_info->udp_group, idb);
	rsvp_info->udp_group = 
	    GETOBJ(paddr,1)->ip_addr? GETOBJ(paddr,1)->ip_addr : RSVP_ENCAP_1;
	if (IPMULTICAST_RSVD(rsvp_info->udp_group))
	    ip_multicast_ctl(idb, rsvp_info->udp_group, TRUE);
	else
	    igmp_add_group(rsvp_info->udp_group, idb, 0, IGMP_LOCAL_FLAG, 0);
    }
}

/************************************************************************
 * ip rsvp bandwidth [<available-bandwidth> [<largest-flow>]]
 * no ip rsvp bandwidth [<available-bandwidth> [<largest-flow>]]
 * no ip rsvp
 *
 * OBJ(int,1) = available bandwidth
 * OBJ(int,2) = available bandwidth to one flow
 */ 
void
rsvp_bandwidth_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb;
    rsvp_info_type *rsvp_info = idb->rsvp_info;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	if (rsvp_info)
	    nv_write(TRUE, "%s %d %d", csb->nv_command,
		     kbps(rsvp_info->maximum_bandwidth),
		     kbps(rsvp_info->maximum_flow_bandwidth));

    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	if (rsvp_info) {
	    if (IPMULTICAST_RSVD(rsvp_info->udp_group))
		ip_multicast_ctl(idb, rsvp_info->udp_group, FALSE);
	    else
		igmp_delete_group(rsvp_info->udp_group, idb);
	    rsvp_clean_interface(idb);
	    free(rsvp_info);
	    idb->rsvp_info = NULL;
	    rsvp_running--;
	    if (!rsvp_running)
		process_kill(rsvp_pid);
	}
    } else {

	/*
	 * install active configuration
	 */
	if (!rsvp_info) {
	    idb->rsvp_info = rsvp_info = malloc(sizeof(rsvp_info_type));
	    if (rsvp_info) {
		rsvp_info->udp_group = RSVP_ENCAP_1;
	        ip_multicast_ctl(idb, rsvp_info->udp_group, TRUE);
	    } else {
		printf("\n configuration not accepted");
		return;
	    }
	    if (!rsvp_running)
		rsvp_pid = process_create(rsvp_process, "rsvp",
					  HUGE_STACK, PRIO_NORMAL);
	    rsvp_running++;
	}
	if (rsvp_info) {
	    rsvp_info->maximum_bandwidth = GETOBJ(int, 1) * 1000;

	    if (!rsvp_info->maximum_bandwidth)
		rsvp_info->maximum_bandwidth = 3000/4 * idb->visible_bandwidth;
	    rsvp_info->maximum_flow_bandwidth = GETOBJ(int, 2) * 1000;

	    if (!rsvp_info->maximum_flow_bandwidth ||
		rsvp_info->maximum_flow_bandwidth
		> rsvp_info->maximum_bandwidth)
		rsvp_info->maximum_flow_bandwidth =
		    rsvp_info->maximum_bandwidth;

	    /*
	     * now make sure that there's enough bandwidth around
	     */
	    hwidb = hwidb_or_null(idb);
	    reg_invoke_rsvp_fixup_wfq(hwidb);

	    if (is_subinterface(idb)) {

		/*
		 * if this is a subinterface, make sure that the
		 * main interface has a similar rsvp data structure
		 */
		idb = hwidb->firstsw;
		rsvp_info = idb->rsvp_info;
		if (!rsvp_info) {
		    idb->rsvp_info = rsvp_info = malloc(sizeof(rsvp_info_type));
		
		    if (rsvp_info) {
			rsvp_running++;
			rsvp_info->udp_group = RSVP_ENCAP_1;
			rsvp_info->maximum_flow_bandwidth = 
			rsvp_info->maximum_bandwidth =
				3000/4 * idb->visible_bandwidth;
		    }
		}
	    }
	}
    }
}

/******************************************************************
 * [no] ip rsvp reservation
 *      <session ip> <sender ip> udp/tcp
 *      <session dport> <sender sport>
 *      <next hop ip> <next hop interface>
 *      FF|SE|WF
 *      <average-kbps> <burst-size>
 *
 * [no] ip rsvp reservation
 *      <session ip> <sender ip> any|<sender protocol>
 *      <next hop ip> <next hop interface>
 *      FF|SE|WF
 *      <average-kbps> <burst-size>
 *
 *      OBJ(paddr,1) = <session ip>
 *      OBJ(paddr,2) = <sender ip>
 *      OBJ(paddr,3) = <next hop ip>
 *      OBJ(idb,1) = <next hop interface>
 *      OBJ(int,1) = <session dport>
 *      OBJ(int,2) = <sender protocol>
 *      OBJ(int,3) = <sender sport>
 *      OBJ(int,4) = <style flags>
 *      OBJ(int,5) = <average kbps>
 *      OBJ(int,6) = <burst size kbytes>
 *      OBJ(int,7) = <QOS Service>
 *      OBJ(int,8) = <Service Level>
 */ 
void
rsvp_ip_resv_command (parseinfo *csb)
{
    boolean result;
    rsvp_session_head *head;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	for (head = rsvp_first_session(&rsvp_session_database);
	     head;
	     head = rsvp_next_session(&rsvp_session_database, head)) {
	    rsvp_show_reservation_head(csb, &head->static_rsb_database);
	} 

    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	result = rsvp_delete_ip_resv_api(GETOBJ(paddr,1)->ip_addr, 
					 GETOBJ(paddr,2)->ip_addr,
					 GETOBJ(int,2), GETOBJ(int,1),
					 GETOBJ(int,3), 
					 GETOBJ(paddr,3)->ip_addr,
					 GETOBJ(idb,1), GETOBJ(int,4));
    } else {

	if (!GETOBJ(paddr,1)->ip_addr) {
	    printf("\nDestination Address 0.0.0.0 Illegal");
	    return;
	}
	if (!GETOBJ(paddr,2)->ip_addr && GETOBJ(int,3)) {
	    printf("\nSource Port non-zero illegal when Source Address 0.0.0.0");
	    return;
	}
	if (GETOBJ(int,2)) {
	    if (!GETOBJ(int,1) && GETOBJ(int,3)) {
	        printf("\nSource Port non-zero illegal when Destination Port 0");
	        return;
	    }
	} else {
	    if (GETOBJ(int,1)) {
	        printf("\nDestination Port non-zero illegal with IP Protocol 0");
	        return;
	    }
	    if (GETOBJ(int,3)) {
	        printf("\nSource Port non-zero illegal with IP Protocol 0");
	        return;
	    }
	}

	/*
	 * install active configuration
	 */
	result = rsvp_add_ip_resv_api(GETOBJ(paddr,1)->ip_addr, 
				      GETOBJ(paddr,2)->ip_addr,
				      GETOBJ(int,2), GETOBJ(int,1),
				      GETOBJ(int,3), GETOBJ(paddr,3)->ip_addr,
				      GETOBJ(idb,1), GETOBJ(int,4),
				      GETOBJ(int,5), GETOBJ(int,6),
				      GETOBJ(int,7), GETOBJ(int,8));
	if (!result)
	    printf("\nRSVP: reservation not accepted");
    }
}

/******************************************************************
 * [no] ip rsvp sender
 *      <session ip> <sender ip> UDP/TCP
 *      <session dport> <sender sport>
 *      <next hop ip> <next hop interface>
 *      <average kbps> <burst size kbytes>
 *
 * [no] ip rsvp sender
 *      <session ip> <sender ip> <sender protocol>
 *      <next hop ip> <next hop interface>
 *      <average kbps> <burst size kbytes>
 *
 *      OBJ(paddr,1) = <session ip>
 *      OBJ(paddr,2) = <sender ip>
 *      OBJ(paddr,3) = <next hop ip>
 *      OBJ(idb,1) = <next hop interface>
 *      OBJ(int,1) = <session dport>
 *      OBJ(int,2) = <sender protocol>
 *      OBJ(int,3) = <sender sport>
 *      OBJ(int,5) = <average kbps>
 *      OBJ(int,6) = <burst size kbytes>
 */ 
void
rsvp_ip_path_command (parseinfo *csb)
{
    rsvp_session_head *head;
    boolean result;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	for (head = rsvp_first_session(&rsvp_session_database);
	     head;
	     head = rsvp_next_session(&rsvp_session_database, head)) {
	    rsvp_show_sender_head(csb, &head->static_psb_database);
	}

    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	result = rsvp_delete_ip_path_api(GETOBJ(paddr,1)->ip_addr, 
					 GETOBJ(paddr,2)->ip_addr,
					 GETOBJ(int,2), GETOBJ(int,1),
					 GETOBJ(int,3));
    } else {

	if (!GETOBJ(paddr,1)->ip_addr) {
	    printf("\nDestination Address 0.0.0.0 Illegal");
	    return;
	}
	if (!GETOBJ(paddr,2)->ip_addr && GETOBJ(int,3)) {
	    printf("\nSource Port non-zero illegal when Source Address 0.0.0.0");
	    return;
	}
	if (GETOBJ(int,2)) {
	    if (!GETOBJ(int,1) && GETOBJ(int,3)) {
	        printf("\nSource Port non-zero illegal when Destination Port 0");
	        return;
	    }
	} else {
	    if (GETOBJ(int,1)) {
	        printf("\nDestination Port non-zero illegal with IP Protocol 0");
	        return;
	    }
	    if (GETOBJ(int,3)) {
	        printf("\nSource Port non-zero illegal with IP Protocol 0");
	        return;
	    }
	}
	    
	/*
	 * install active configuration
	 */
	result = rsvp_add_ip_path_api(GETOBJ(paddr,1)->ip_addr, 
				      GETOBJ(paddr,2)->ip_addr,
				      GETOBJ(int,2), GETOBJ(int,1),
				      GETOBJ(int,3), GETOBJ(paddr,3)->ip_addr,
				      GETOBJ(idb,1), GETOBJ(int,5),
				      GETOBJ(int,6));
	if (!result)
	    printf("\nRSVP: sender not accepted");

    }
}

/******************************************************************
 * show ip rsvp installed [<interface-unit>]
 *
 *      OBJ(idb,1) = optional interface name.
 */
static void
rsvp_show_install_idb (idbtype *idb)
{
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    hwidbtype *hwidb = hwidb_or_null(idb);
    rsvp_flow_ipv4 *flow;
    short i;
    static const char wfq_format1[] = "\n%6s %15i %15i %6s %6d %6d %6d %6d";
    static const char wfq_format2[] = "\n%6s %15i %15i %6d %6d %6d %6d %6d";
    static const char red_format1[] = "\n%6s %15i %15i %6s %6d %6d";
    static const char red_format2[] = "\n%6s %15i %15i %6d %6d %6d";
    const char *format1;
    const char *format2;
    char speed[16];

    if (!rsvp_info)
	return;
    if (rsvp_info->allocated_bitrate) {
	if (hwidb && hwidb->fair_queue_head) {
	    format1 = wfq_format1;
	    format2 = wfq_format2;
	    printf("\nRSVP: %s\n%6s %15s %15s %6s %6s %6s %6s %s",
		   idb->namestring, "BPS", "To", "From",
		   "Protocol", "DPort", "Sport", "Weight", "Conversation");
	} else {
	    format1 = red_format1;
	    format2 = red_format2;
	    printf("\nRSVP: %s\n%6s %15s %15s %6s %6s %6s",
		   idb->namestring, "BPS", "To", "From",
		   "Protocol", "DPort", "Sport");
	}

	for (i = RSVP_FLOW_HASH; --i >= 0;)
	    for (flow = rsvp_info->flows[i]; flow; flow = flow->next) {
		if (automore_quit())
		    return;

		rsvp_kbps_display(flow->bandwidth, speed);
		switch (flow->ip_protocol) {
		case UDP_PROT:
		    printf(format1,
			   speed,
			   flow->destination,
			   flow->source,
			   "UDP",
			   flow->udp_dest_port,
			   flow->udp_source_port,
			   flow->weight,
			   flow->conversation);
		    break;

		case TCP_PROT:
		    printf(format1,
			   speed,
			   flow->destination,
			   flow->source,
			   "TCP",
			   flow->udp_dest_port,
			   flow->udp_source_port,
			   flow->weight,
			   flow->conversation);
		    break;

		default:
		    printf(format2,
			   speed,
			   flow->destination,
			   flow->source,
			   flow->ip_protocol,
			   flow->udp_dest_port,
			   flow->udp_source_port,
			   flow->weight,
			   flow->conversation);
		    break;
		}
	    }
    } else {
	printf("\nRSVP: %s has no installed reservations",
	       idb->namestring);
    }
}

void
rsvp_show_install (parseinfo *csb)
{
    idbtype *idb = GETOBJ(idb, 1);

    if (!idb) {
	automore_enable(NULL);
	FOR_ALL_SWIDBS(idb) {
	    if (automore_quit())
		break;
	    rsvp_show_install_idb(idb);
	} automore_disable();
    } else if (!idb->rsvp_info) {

	/*
	 * RSVP not configured - bail
	 */
	printf("\nRSVP: not an RSVP Interface");

    } else {
	automore_enable(NULL);
	rsvp_show_install_idb(idb);
	automore_disable();
    }
}

/******************************************************************
 * show ip rsvp interface [<interface-unit>]
 *
 *      OBJ(idb,1) = optional interface name.
 */

static void
rsvp_show_interface_idb (idbtype *idb)
{
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    char rate[16];
    char bw[16];
    char flow_bw[16];

    if (rsvp_info) {
	rsvp_kbps_display(rsvp_info->allocated_bitrate, rate);
	rsvp_kbps_display(rsvp_info->maximum_bandwidth, bw);
	rsvp_kbps_display(rsvp_info->maximum_flow_bandwidth, flow_bw);
	printf("\n%8s %8s %8s %8s %3d/255 %4d %4d %8d %8d",
	       idb->short_namestring, rate, bw, flow_bw,
	       rsvp_info->allocated_bitrate/(rsvp_info->maximum_bandwidth/256),
	       rsvp_info->udp_neighbors, rsvp_info->ip_neighbors,
	       rsvp_info->duplex_neighbors, rsvp_info->udp_required);
    }
}

void
rsvp_show_interface (parseinfo *csb)
{
    idbtype *idb = GETOBJ(idb, 1);

    if (!idb) {
	automore_enable(NULL);
	printf("\n%8s %8s %8s %8s %3s/255 %4s %4s %8s %8s",
	       common_str_interface, "allocated", "i/f max", "flow max",
	       "percent", "UDP", "IP", "UDP_IP", "UDP M/C");
	FOR_ALL_SWIDBS(idb) {
	    if (automore_quit())
		break;
	    rsvp_show_interface_idb(idb);
	}
	automore_disable();
    } else if (!idb->rsvp_info) {

	/*
	 * RSVP not configured - bail
	 */
	printf("\nRSVP: not an RSVP Interface");

    } else {
	automore_enable(NULL);
	printf("\n%8s %8s %8s %8s %3s/255 %4s %4s %8s %8s",
	       common_str_interface, "allocated", "i/f max", "flow max",
	       "percent", "UDP", "IP", "UDP_IP", "UDP M/C");
	rsvp_show_interface_idb(idb);
	automore_disable();
    }
}

/******************************************************************
 * show ip rsvp reservation [<group-name> | <group-address>]
 *
 *      OBJ(paddr,1) = IP group address
 */
void
rsvp_show_reservation (parseinfo *csb)
{
    ipaddrtype group_address = GETOBJ(paddr,1)->ip_addr;
    rsvp_session_head *head;

    automore_enable(NULL);
    printf("\n%13s %13s %3s %5s %5s %13s %5s %2s %4s %3s %s",
	   "To", "From", "Protocol", "DPort", "Sport",
	   "Next Hop", "I/F", "Filter", "Service", "BPS", "Bytes", "Service");
    for (head = rsvp_first_session(&rsvp_session_database);
	 head;
	 head = rsvp_next_session(&rsvp_session_database, head)) {
	if (automore_quit())
	    break;
	if (!group_address || group_address == head->session.destination) {
	    rsvp_show_reservation_head(NULL, &head->rsb_incoming_database);
	}
    } 
    automore_disable();
}

/******************************************************************
 * show ip rsvp request [<group-name> | <group-address>]
 *
 *      OBJ(paddr,1) = IP group address
 */
void rsvp_show_request (parseinfo *csb) 
{
    ipaddrtype group_address = GETOBJ(paddr,1)->ip_addr;
    rsvp_session_head *head;

    automore_enable(NULL);
    printf("\n%13s %13s %3s %5s %5s %13s %5s %2s %4s %3s %s",
	   "To", "From", "Protocol", "DPort", "Sport",
	   "Next Hop", "I/F", "Filter", "Service", "BPS", "Bytes", "Service");
    for (head = rsvp_first_session(&rsvp_session_database);
	 head;
	 head = rsvp_next_session(&rsvp_session_database, head)) {
	if (automore_quit())
	    break;
	if (!group_address || group_address == head->session.destination) {
	    rsvp_show_reservation_head(NULL, &head->rsb_fixed_database);
	    rsvp_show_reservation_head(NULL, &head->rsb_shared_database);
	}
    } 
    automore_disable();
}

/******************************************************************
 * show ip rsvp sender [<group-name> | <group-address>]
 *
 *      OBJ(paddr,1) = IP group address
 */ 
void rsvp_show_path (parseinfo *csb) 
{
    ipaddrtype group_address = GETOBJ(paddr,1)->ip_addr;
    rsvp_session_head *head;

    automore_enable(NULL);
    printf("\n%15s %15s %3s %5s %5s %15s %4s %4s %s",
	   "To", "From", "Protocol", "DPort", "Sport",
	   "Prev Hop", "I/F", "BPS", "Bytes");
    for (head = rsvp_first_session(&rsvp_session_database);
	 head;
	 head = rsvp_next_session(&rsvp_session_database, head)) {
	if (automore_quit())
	    break;
	if (!group_address || group_address == head->session.destination) {
	    rsvp_show_sender_head(NULL, &head->psb_database);
	}
    }
    automore_disable();
}

/******************************************************************
 * show ip rsvp neighbor [<interface-unit>]
 *
 *      OBJ(idb,1) = optional interface name.
 */
static void rsvp_show_neighbor_idb (idbtype *idb)
{
    rsvp_info_type *rsvp_info = idb->rsvp_info;
    rsvp_neighbor_type *nbr;
    if (rsvp_info) {
	for (nbr = rsvp_info->rsvp_nbr; nbr; nbr = nbr->next) {
	    if (automore_quit())
		return;
	    printf("\n%8s %15i %s", idb->short_namestring, nbr->neighbor,
		   nbr->ip_protocol == UDP_PROT ? "UDP" :
		   nbr->ip_protocol == RSVP_PROT ? "RSVP" :
		   "BOTH");
	}
    }
}

void rsvp_show_neighbor (parseinfo *csb)
{
    idbtype *idb = GETOBJ(idb, 1);

    if (!idb) {
	automore_enable(NULL);
	printf("\n%8s %15s %s", "Interface", "Neighbor", "Encapsulation");
	FOR_ALL_SWIDBS(idb) {
	    if (automore_quit())
		break;
	    rsvp_show_neighbor_idb(idb);
	}
	automore_disable();
    } else if (!idb->rsvp_info) {

	/*
	 * RSVP not configured - bail
	 */
	printf("\nRSVP: not an RSVP Interface");

    } else {
        printf("\n%8s %15s %s", "Interface", "Neighbor", "Encapsulation");
        automore_enable(NULL);
        rsvp_show_neighbor_idb(idb);
        automore_disable();
    }
}

/******************************************************************
 * clear ip rsvp sender
 *	{ <session ip> <sender ip> UDP/TCP/<ip protocol>
 *	<session dport> <sender sport> | '*' }
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,4) = TRUE if <session ip> is given, FALSE if '*'
 */
void
rsvp_clear_ip_sender_command (parseinfo *csb)
{
    rsvp_session_head *head;
    psbtype *psb;
    ipaddrtype destination;
    ipaddrtype source;
    int dest_port;
    int ip_protocol;
    int source_port;
    boolean clear_one_sender = GETOBJ(int,4);

    if (!clear_one_sender) {
	rsvp_clr_ip_sndr_signal = TRUE;
	process_watch_boolean(rsvp_clr_cmd_over, ENABLE, ONE_SHOT);
	process_wakeup(rsvp_pid);
	process_wait_for_event();
	process_watch_boolean(rsvp_clr_cmd_over, DISABLE, 0);
	return;
    }
    
    destination = GETOBJ(paddr,1)->ip_addr;
    source = GETOBJ(paddr,2)->ip_addr;
    dest_port = GETOBJ(int,1); 
    ip_protocol = GETOBJ(int,2);
    source_port = GETOBJ(int,3);

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol, dest_port)) {
    case RSVP_NO_MEMORY:
	return;

    case RSVP_FOUND:
	if (head->session.udp_dest_port != dest_port)
	    return;
	break;

    case RSVP_NOT_FOUND:
	free(head);
	return;
    }

    switch(rsvp_session_path_search(&head->psb_database, &psb,
				    source, source_port)) {
    case RSVP_NO_MEMORY:
	return;
    case RSVP_NOT_FOUND:
	free(psb);
	break;
    case RSVP_FOUND:
	TIMER_START(psb->cleanup_timer, 0);
	rsvp_psb_expiry(psb->threads, NULL);
	rsvp_session_done(head);
	return;
    }
}

/******************************************************************
 * clear ip rsvp reservation
 *	{ <session ip> <sender ip> TCP/UDP/<ip protocol>
 *	<session dport> <sender sport> | '*' }
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,4) = TRUE if <session ip> is given; FALSE if '*'
 */
void
rsvp_clear_ip_resv_command (parseinfo *csb)
{
    rsvp_session_head *head;
    rsbtype *rsb;
    ipaddrtype destination;
    ipaddrtype source;
    int dest_port; 
    int ip_protocol;
    int source_port;
    boolean clear_one_resv = GETOBJ(int,4);

    if (!clear_one_resv) {
	rsvp_clr_ip_resv_signal = TRUE;
	process_watch_boolean(rsvp_clr_cmd_over, ENABLE, ONE_SHOT);
	process_wakeup(rsvp_pid);
	process_wait_for_event();
	process_watch_boolean(rsvp_clr_cmd_over, DISABLE, 0);
	return;
    }

    destination = GETOBJ(paddr,1)->ip_addr;
    source = GETOBJ(paddr,2)->ip_addr;
    dest_port = GETOBJ(int,1); 
    ip_protocol = GETOBJ(int,2);
    source_port = GETOBJ(int,3);

    switch(rsvp_session_head_search(&head, destination,
				    ip_protocol, dest_port)) {
    case RSVP_NO_MEMORY:
	return;

    case RSVP_FOUND:
	if (head->session.udp_dest_port != dest_port)
	    return;
	break;

    case RSVP_NOT_FOUND:
	return;
    }

    switch(rsvp_session_reservation_search(&head->rsb_incoming_database, &rsb,
				    (idbtype *)NULL, 0, 0, 
				    source, source_port)) {
    case RSVP_NO_MEMORY:
	return;
    case RSVP_NOT_FOUND:
	free(rsb);
	break;
    case RSVP_FOUND:
	TIMER_START(rsb->cleanup_timer, 0);
	rsvp_rsb_expiry(rsb->threads, NULL);
	rsvp_session_done(head);
	break;
    }
}

/*
 * rsvp_show_techsupport_commands
 *
 * Display useful IP RSVP related information.
 */
void rsvp_show_techsupport_commands (parseinfo *csb)
{
    show_techsupport_command(csb, "show ip rsvp interface");
    show_techsupport_command(csb, "show ip rsvp sender");
    show_techsupport_command(csb, "show ip rsvp reservation");
    show_techsupport_command(csb, "show ip rsvp installed");
    show_techsupport_command(csb, "show ip rsvp request");
    show_techsupport_command(csb, "show ip rsvp neighbor");
}
