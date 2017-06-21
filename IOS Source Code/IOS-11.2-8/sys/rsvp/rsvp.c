/* $Id: rsvp.c,v 1.1.4.13 1996/08/28 13:08:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp.c,v $
 * Revision 1.1.4.13  1996/08/28  13:08:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.12  1996/08/24  01:16:40  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.4.11  1996/08/20  10:24:22  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.10  1996/08/07  09:02:12  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 1.1.4.9  1996/07/19  06:09:23  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.8  1996/06/21  19:05:21  fred
 * CSCdi57507:  RSVP gets installed with Conversation number 0 on ethernet.
 *         Install registry callback from WFQ setup to RSVP to make sure
 *         that some reservable queues are allocated. Also, do not display
 *         weights and conversations unless WFQ invoked on interface
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/06/18  11:05:40  fred
 * CSCdi53943:  box crashes when removing RSVP from an interface
 *              Add defensive code to avoid null dereferences, and
 *              improve handling of interface and topology changes
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/18  08:37:12  fred
 * CSCdi60599:  RSVP interaction with PIM Sparse Mode has problems.
 *              Implement special code required for sparse mode route
 *              creation
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/05  21:11:41  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/05/17  11:40:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.18.1  1996/05/05  23:45:13  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 1.1.4.3  1996/05/13  16:42:25  fred
 * CSCdi57498:  rsvp data structures trashed - reverse order of destination
 *         and source when changing bcopy to memcpy calls
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/13  14:47:26  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:15  fred
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
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "string.h"
#include "../os/signal.h"
#include "../os/techsupport.h"
#include "../if/priority_private.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/ipoptions.h"
#include "../ip/ip_registry.h"
#include "../lib/cisco/include/ciscolib.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"

/***********************************************************
 * *
 * Process Variables *
 * *
 ***********************************************************/

#ifdef RSVP_MESSAGE_TEST
/*
 * Test Message
 */
struct rsvp_test_message_format {
	rsvp_header header;
	rsvp_session_ipv4 session;
	rsvp_hop_ipv4 hop;
	ulong refresh[3];
	rsvp_template_ipv4 template;
	rsvp_tspec tspec;
};
static const unsigned rsvp_test_message1[] = {
 0x10011d27 ,0x3e000050 ,0x000c0101 ,0xe0010101
,0x1101115c ,0x000c0301 ,0x83af030e ,0x00000000
,0x00080501 ,0x00007530 ,0x000c0b01 ,0x83af030e
,0x0000d903 ,0x001c0c01 ,0x00000005 ,0x05040000
,0x461c4000 ,0x461c4000 ,0x00000001 ,0x000003e8
};

static const unsigned rsvp_test_message2[] = {
 0x10051d23 ,0x3e000050 ,0x000c0101 ,0xe0010101
,0x1101115c ,0x000c0301 ,0x83af030e ,0x00000000
,0x00080501 ,0x00007530 ,0x000c0b01 ,0x83af030e
,0x0000d903 ,0x001c0c01 ,0x00000005 ,0x05040000
,0x461c4000 ,0x461c4000 ,0x00000001 ,0x000003e8
};
#endif
/*
 * note in following that UDP sockets always have a message type of 0
 */
enum rsvp_queue_types {
    UDP_PACKETS = 0, IP_PACKETS
    };
enum rsvp_messages {
    RSVP_IF_DOWN
};


static void rsvp_init(void);

int rsvp_debug = 0;
int rsvp_running = 0;

boolean rsvp_clr_ip_resv_signal = FALSE; 
boolean rsvp_clr_ip_sndr_signal = FALSE;
watched_boolean *rsvp_clr_cmd_over;

static ipsocktype *rsvp_udp;
mgd_timer rsvp_root_timer;
static watched_queue *rsvp_ip;

/*
 * extract precedence and return to RED
 */
static short rsvp_red_precedence (paktype *pak)
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

    ip = iphdrtype_start(pak);
    destination = ip->dstadr;
    source = ip->srcadr;
    ip_protocol = ip->prot;

    ports =(udptype *) ipdatastart_iph(ip);
    udp_source_port = ports->sport;
    udp_dest_port = ports->dport;

    rsvp_info = pak->if_output->rsvp_info;
    if(rsvp_info) {
	conversation = rsvp_info->flows[nethash(destination)];
    
	for(; conversation; conversation = conversation->next) {
	    if(ip_protocol != conversation->ip_protocol)
		continue;
	    if(destination != conversation->destination)
		continue;
	    if(conversation->source && source != conversation->source)
		continue;
	    if(!conversation->udp_dest_port)
		return(RED_RSVP_PRECEDENCE);
	    else if(udp_dest_port != conversation->udp_dest_port)
		continue;
	    else if(!conversation->udp_source_port)
		return(RED_RSVP_PRECEDENCE);
	    else if(udp_source_port != conversation->udp_source_port)
		continue;
	    else
		return(RED_RSVP_PRECEDENCE);
	}
    }
    return(ip->tos >> IP_PRECEDENCE_SHIFT);	/* return 0..7 */
}

/*
 * rsvp_acl_debug
 *
 * Print debug message contingent upon 'debug_flag' being TRUE, and the acl
 * permitting (if it exists).
 */
boolean rsvp_acl_debug (boolean debug_flag, ipaddrtype address) 
{
    return(debug_flag && fast_check(rsvp_acl, address));
}

/*
 * enqueue IP messages to RSVP.
 */
static void
rsvp_enqueue (paktype * pak)
{
    idbtype *idb;
    iphdrtype *ip;
    rsvp_info_type	*rsvp_info;

    ip =(iphdrtype *) ipheadstart(pak);

    idb = pak->if_input;
    rsvp_info = idb->rsvp_info;
    if(rsvp_info &&(rsvp_running))
	process_enqueue(rsvp_ip, pak);
    else {
	RSVP_BUGINF(0,("\nRSVP: Input packet while RSVP disabled on %s",
		       pak->if_input->namestring));
	retbuffer(pak);
    }
}

/*
 * enqueue IP messages to RSVP.
 */
static void
rsvp_alert_handler (paktype * pak, boolean * absorbed)
{
    idbtype *idb;

    idb = pak->if_input;
    if(idb->rsvp_info && rsvp_running && !(pak->flags & PAK_SENDSELF)) {
	ip_enqueue(pak);
	*absorbed = TRUE;
    }
    else {
	*absorbed = FALSE;
    }
}

#ifdef RSVP_MESSAGE_TEST
static void
rsvp_generate_message (idbtype *idb, void *test_message)
{
    paktype *pak;
    iphdrtype *ip;
    struct rsvp_test_message_format *header;

    if (!interface_up_simple(idb))
	return;

    header = test_message;
    pak = getbuffer(sizeof(iphdrtype) + header->header.rsvp_length + 10);
    if(pak) {
	ip = iphdrtype_start(pak);
	ipbuild_head(pak, sizeof(iphdrtype) + header->header.rsvp_length,
		     RSVP_PROT, header->hop.neighbor,
		     header->session.destination, NOPT, &ipdefaultparams);
	memcpy(ipdatastart_iph(ip), header, header->header.rsvp_length);
	pak->datagramsize = ip->tl;
	pak->if_input = idb;
	pak->flags |= PAK_SENDSELF;
	rsvp_input_message(pak, &header->header, header->header.rsvp_length);
	datagram_done(pak);
    }
}
#endif

/*
 * build a path message and recieve it.
 * this implements static path messages through the API
 */
static void
rsvp_receive_static_path (psbtype * psb)
{
    ushort length;
    ushort pak_length;
    paktype *pak;
    iphdrtype *ip;
    rsvp_header *header;
    rsvp_object *object;

    rsvp_schedule_path_refresh(psb);

    if (!interface_up_simple(psb->if_input))
	return;

    length = sizeof(rsvp_header) + psb->head->session.base.length +
	     psb->hop.base.length + psb->time_values.base.length +
	     psb->template.base.length + psb->advertisement.base.length +
	     psb->traffic_specification.base.length;

    pak_length = sizeof(iphdrtype) + length;
    pak = getbuffer(pak_length);
    if(pak) {
	bzero(pak->network_start, pak_length);
	ip = iphdrtype_start(pak);
	ipbuild_head(pak, pak_length, RSVP_PROT,
		     psb->hop.neighbor, psb->head->session.destination,
		     NOPT, &ipdefaultparams);
	header =(rsvp_header *) ipdatastart_iph(ip);
	object = rsvp_build_header(header, RSVP_MSG_PATH, TTL_DEFAULT/4);
	header->rsvp_length = length;

	rsvp_object_copy(object, &psb->head->session);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &psb->hop);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &psb->time_values);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &psb->template);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &psb->traffic_specification);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &psb->advertisement);

	pak->flags |= PAK_SENDSELF;
	pak->if_input = psb->if_input;
	rsvp_input_message(pak, header, header->rsvp_length);
	datagram_done(pak);
    }
}

/*
 * build a reservation message and recieve it
 * this implements static reservation messages through the API
 */
static void
rsvp_receive_static_reservation (rsbtype * rsb)
{
    ushort length;
    ushort pak_length;
    paktype *pak;
    iphdrtype *ip;
    rsvp_header *header;
    rsvp_object *object;

    rsvp_schedule_resv_refresh(rsb);

    if (!interface_up_simple(rsb->if_input))
	return;

    length = sizeof(rsvp_header) + rsb->head->session.base.length +
	rsb->hop.base.length + rsb->time_values.base.length +
	rsb->confirm.base.length +
	rsb->policy.base.length + rsb->style.base.length +
	rsb->flowspec.base.length + rsb->filter.base.length;
    if (rsb->scope)
	length += rsb->scope->base.length;

    pak_length = sizeof(iphdrtype) + length;
    pak = getbuffer(pak_length);
    if(pak) {
	bzero(pak->network_start, pak_length);
	ip = iphdrtype_start(pak);
	ipbuild_head(pak, pak_length, RSVP_PROT,
		     rsb->hop.neighbor, rsb->head->session.destination,
		     NOPT, &ipdefaultparams);
	header =(rsvp_header *) ipdatastart_iph(ip);
	object = rsvp_build_header(header, RSVP_MSG_RESV, TTL_DEFAULT/4);
	header->rsvp_length = length;

	rsvp_object_copy(object, &rsb->head->session);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->hop);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->time_values);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->confirm);
	object = rsvp_next_object(object);
	if (rsb->scope) {
	    rsvp_object_copy(object, rsb->scope);
	    object = rsvp_next_object(object);
	}
	rsvp_object_copy(object, &rsb->policy);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->style);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->flowspec);
	object = rsvp_next_object(object);
	rsvp_object_copy(object, &rsb->filter);

	rsb->confirm.base.length = 0;	/* no need to confirm again */
	pak->flags |= PAK_SENDSELF;
	pak->if_input = rsb->if_input;
	rsvp_input_message(pak, header, header->rsvp_length);
	datagram_done(pak);
    }
}

/*
 * rsvp_change_if_state
 *
 * notify on interface state changes
 */
static void
rsvp_change_if_state (idbtype *swidb, hwidbtype *hwidb)
{
    if (!hwidb) {
	/* handle subinterfaces */
	hwidb = hwidb_or_null(swidb);
	if (!hwidb)
	    return;
    }
    if (!swidb) {
	swidb = firstsw_or_null(hwidb);
	if (!swidb) 
	    return;
    }

    /* now have a swidb AND an hwidb */

    if (swidb->rsvp_info && !interface_up_simple(swidb))
	    process_send_message(rsvp_pid, RSVP_IF_DOWN, swidb, 0);
}

/***********************************************************
 * *
 * Process Context *
 * *
 ***********************************************************/
/*
 * rsvp_timers
 *
 * Process expired refresh timers. A refresh timer triggering is on a
 * PATH, and results in:
 * updated path information propagation toward receivers
 * updated reservation information toward sender(s)
 * if the dead man timer for any reservation or psb has expired,
 * cleanup of that state block.
 */ 
static void
rsvp_timers (void)
{
    mgd_timer *expired_timer;
    psbtype *psb;
    rsbtype *rsb;
    rsvp_session_head *head;

    while(mgd_timer_expired(&rsvp_root_timer)) {
	expired_timer = mgd_timer_first_expired(&rsvp_root_timer);
	switch(mgd_timer_type(expired_timer)) {

	case TIMER_TYPE_STATIC_PATH:
	    psb = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    rsvp_receive_static_path(psb);
	    break;

	case TIMER_TYPE_REFRESH_PATH:
	    psb = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    rsvp_refresh_path(psb);
	    break;

	case TIMER_TYPE_REFRESH_RESERVATION:
	    rsb = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    rsvp_refresh_reservation(rsb);
	    break;

	case TIMER_TYPE_CLEANUP:
	    head = mgd_timer_context(expired_timer);
	    mgd_timer_start(expired_timer, RSVP_REFRESH_MS);
	    wavl_walk(&head->psb_database, 0, rsvp_psb_expiry);
	    wavl_walk(&head->rsb_incoming_database, 0, rsvp_rsb_expiry);
	    rsvp_session_done(head);
	    break;

	case TIMER_TYPE_STATIC_RESV:
	    rsb = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    rsvp_receive_static_reservation(rsb);
	    break;

	default:
	    errmsg(&msgsym(UNEXPECTEDTIMER, SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/*
 * rsvp_queues
 *
 * pull messages out of queues and operate on them
 */
static void
rsvp_queues (ulong minor)
{
    udptype *udp;
    iphdrtype *ip;
    rsvp_header *rsvp;
    paktype *pak;
    rsvp_info_type *rsvp_info = NULL;

    switch(minor) {
    case IP_PACKETS:
	pak = process_dequeue(rsvp_ip);
        if (pak->if_input)
	    rsvp_info = pak->if_input->rsvp_info;
	if (!(pak->flags & PAK_SENDSELF) && rsvp_info) {
	    ip = iphdrtype_start(pak);
	    rsvp =(rsvp_header *) ipdatastart(pak);
            if (reg_invoke_access_check(pak->linktype,
                                        pak, rsvp_info->nbr_acl)) {
	        rsvp_input_message(pak, rsvp, ip->tl - ipheadsize(ip));
	    } else {
		RSVP_BUGINF(0, ("\nRSVP: RSVP Message from %i rejected",
				ip->srcadr));
	    }
	}
	datagram_done(pak);
	break;

    case UDP_PACKETS:
	pak = read_ipsocket(rsvp_udp, 0, 0, &udp, &rsvp);
	if (pak) {
            if (pak->if_input)
	        rsvp_info = pak->if_input->rsvp_info;
	    if (!(pak->flags & PAK_SENDSELF) && rsvp_info) {
		ip = iphdrtype_start(pak);
	        rsvp->cksum = 0;
                if (reg_invoke_access_check(pak->linktype,
                                            pak, rsvp_info->nbr_acl)) {
	            rsvp_input_message(pak, rsvp, udp->length - UDPHEADERBYTES);
	        } else {
		    RSVP_BUGINF(0, ("\nRSVP: RSVP Message from %i rejected",
				    ip->srcadr));
	        }
	    }
	    datagram_done(pak);
	}
	break;

    default:
	errmsg(&msgsym(UNEXPECTEDQUEUE, SCHED), minor);
	break;
    }
}

/*
 * rsvp_messages
 *
 * this is more an error handler thn anything else; there are no messages
 */
static void
rsvp_messages (void)
{
    ulong message,
    value;
    void *pointer;

    while(process_get_message(&message, &pointer, &value)) {
	switch(message) {
	case RSVP_IF_DOWN:
	    rsvp_clean_interface(pointer);
	    break;
	default:
	    errmsg(&msgsym(UNEXPECTEDMESSAGE, SCHED), message, pointer, value);
	    break;
	}
    }
}

/*
 * rsvp_teardown
 *
 * This process is being killed. Remove any data structures that this
 * process set up when it started.
 */
static void
rsvp_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    /*
     * stop ALL timers
     */
    mgd_timer_stop(&rsvp_root_timer);

    rsvp_clear_reservations();
    wavl_finish(&rsvp_session_database);
    process_watch_mgd_timer(&rsvp_root_timer, DISABLE);

    process_watch_queue(rsvp_ip, DISABLE, RECURRING);
    process_watch_queue(rsvp_udp->inq, DISABLE, RECURRING);
    while((pak = process_dequeue(rsvp_ip)) != NULL)
	retbuffer(pak);
    while((pak = process_dequeue(rsvp_udp->inq)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&rsvp_ip);
    close_ipsocket(rsvp_udp);
    delete_watched_boolean(&rsvp_clr_cmd_over);

    rsvp_running = 0;
    rsvp_init();		/* put in initial condition */
}

/*
 * rsvp_process
 *
 * The "main" process for the RSVP Protocol
 */
void
rsvp_process (void)
{
    ulong major;
    ulong minor;

    /*
     * Setup process private structures
     */
    reg_add_ip_rsvp_classify(0, rsvp_ip_conversation, "rsvp_ip_conversation");
    reg_add_determine_RED_precedence(LINK_IP, rsvp_red_precedence, "rsvp_red_precedence");
    reg_add_if_statechange_complete(rsvp_change_if_state, "rsvp_change_if_state");
    reg_add_ip_enqueue(RSVP_PROT, rsvp_enqueue, "rsvp_enqueue");
    rsvp_ip = create_watched_queue("RSVP/IP", 0, IP_PACKETS);
    rsvp_udp = open_ipsocket(UDP_PROT, 0, 0, RSVP_PORT_ROUTER, 0, SOCK_ANYSRC);

    process_watch_queue(rsvp_ip, ENABLE, RECURRING);
    process_watch_queue(rsvp_udp->inq, ENABLE, RECURRING);
    process_watch_mgd_timer(&rsvp_root_timer, ENABLE);
    signal_permanent(SIGEXIT, rsvp_teardown);
    rsvp_clr_cmd_over = create_watched_boolean("RSVP clear cmds", 0);

    /*
     * Initialize Databases
     */
#ifdef RSVP_MESSAGE_TEST
    {
	idbtype *idb;
	FOR_ALL_SWIDBS(idb)
	    if (hwidb_or_null(idb) && idb->rsvp_info && hwidb_or_null(idb)->enctype == ET_ARPA) {
	rsvp_generate_message(idb, rsvp_test_message1);
	rsvp_generate_message(idb, rsvp_test_message2);
	}
    }
#endif
    while(rsvp_running) {
	/*
	 * Wait for the next event to occur.
	 */
	process_wait_for_event();
	while(process_get_wakeup(&major, &minor)) {
	    switch(major) {
	    case TIMER_EVENT:
		rsvp_timers();
		break;

	    case QUEUE_EVENT:
		rsvp_queues(minor);
		break;

	    case MESSAGE_EVENT:
		rsvp_messages();
		break;

	    case DIRECT_EVENT:
		if (rsvp_clr_ip_resv_signal) {
		    wavl_walk(&rsvp_session_database, 0, 
			      rsvp_clr_ip_resv);
		    rsvp_clean_session_heads();
		    rsvp_clr_ip_resv_signal = FALSE;
		    process_set_boolean(rsvp_clr_cmd_over, TRUE);
		}
		if (rsvp_clr_ip_sndr_signal) {
		    wavl_walk(&rsvp_session_database, 0, 
			      rsvp_clr_ip_sndr);
		    rsvp_clean_session_heads();
		    rsvp_clr_ip_sndr_signal = FALSE;
		    process_set_boolean(rsvp_clr_cmd_over, TRUE);
		}
		break;

	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

/***********************************************************
 * *
 * Subsystem Context *
 * *
 ***********************************************************/
static void rsvp_init(void)
{
    rsvp_debug = 0;
    rsvp_running = 0;

    rsvp_udp = NULL;
    bzero(&rsvp_root_timer, sizeof(rsvp_root_timer));
    rsvp_ip = NULL;

    bzero(&rsvp_session_database, sizeof(rsvp_session_database));
    rsvp_session_database_init(&rsvp_session_database, rsvp_session_compare);

    rsvp_routing_alert_init();
    rsvp_parser_init();
    rsvp_debug_init();

    mgd_timer_init_parent(&rsvp_root_timer, NULL);
}

static void rsvp_init_thyself (subsystype * subsystem)
{
    rsvp_init();
    reg_add_rsvp_fixup_wfq(rsvp_fixup_wfq, "rsvp_fixup_wfq");
    reg_add_interface_cfg_modified(
	rsvp_modify_interface_reservations, 
	"rsvp_modify_interface_reservations");
    reg_add_iprouting_alert(RSVP_PROT, rsvp_alert_handler,
			    "rsvp_alert_handeler");

    /*
     * Register the tech-support command.
     */
    reg_add_show_techsupport_protocol(TECHSUPPORT_IP_RSVP,
	rsvp_show_techsupport_commands, "rsvp_show_techsupport_commands");
}
/*
 * RSVP subsystem header
 */
#define RSVP_MAJVERSION 1
#define RSVP_MINVERSION 0
#define RSVP_EDITVERSION 1
SUBSYS_HEADER (rsvp, RSVP_MAJVERSION, RSVP_MINVERSION, RSVP_EDITVERSION,
	       rsvp_init_thyself, SUBSYS_CLASS_PROTOCOL,
	       "seq: iprouting", "req: iphost ipmulticast iprouting");
