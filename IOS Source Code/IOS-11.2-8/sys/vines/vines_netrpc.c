/* $Id: vines_netrpc.c,v 3.5.12.3 1996/08/28 13:19:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_netrpc.c,v $
 *------------------------------------------------------------------
 * vines_netrpc.c - Support for the vines Network Remote
 *		    Procedure Call (NetRPC) protocol.
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_netrpc.c,v $
 * Revision 3.5.12.3  1996/08/28  13:19:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.2  1996/08/07  09:03:43  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/05/11  16:17:34  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.5  1996/02/14  18:30:55  ehk
 * CSCdi48247:  vines bluemail clients on serverless cannot get time
 * changes to avoid unaligned memory access and compiler padding in
 * VINES packet structures.
 *
 * Revision 3.4  1996/02/08  20:13:57  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1996/02/07  02:20:14  slin
 * CSCdi48252:  Vines: Spurious memory access in vsrtp_update_route
 * - Fix NULL pointer dereference in vsrtp_update_route() and unaligned
 *   longword write in vrpc_send_returnaddress()
 *
 * Revision 3.2  1995/11/17  17:58:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/25  23:53:35  slin
 * CSCdi42851:  Make replies to Vines Time requests unreliable
 * - Make replies to NetRPC Searches unreliable
 *
 * Revision 2.1  1995/06/07  23:14:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "../h/address.h"
#include "config.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"

/*
 * Globals
 */
const char bad_rpc_socket[]= "\nVRPC: send %s passed invalid socket.";
const char bad_rpc_packet[]= "\nVRPC: send %s passed/allocated invalid packet.";
const int vrpc_frame_lengths[] = {sizeof(vrpc_call), sizeof(vrpc_reject),
			    sizeof(vrpc_return), sizeof(vrpc_abort),
			    sizeof(vrpc_search), sizeof(vrpc_returnaddress),
			    sizeof(vrpc_search) };
char *const vrpc_names[] = {"call", "reject", "return", "abort", "search",
		      "return address", "search all", "unknown"};
static ulong vines_rpc_id;
const char vines_VINES_str[] = "VINES";
const char vines_Vines_str[] = "Vines";
const char vines_invalid_string_len_msg[] = 
    "\n%s: Invalid string length in data.";
const char vines_cannot_alloc_netrpc_packet_msg[] =
    ": cannot allocate packet for NetRPC frame.";


/**********************************************************************
 *
 *			 INITIALIZATION
 *
 **********************************************************************/

/*
 * vines_init_netrpc
 *
 * Initialize all session layer information.  There currently isn't much
 * here to initialize.
 */
void vines_init_netrpc (void)
{
    vines_rpc_id = 1;
    vines_init_services();
}


/**********************************************************************
 *
 *			   DEBUGGING
 *
 **********************************************************************/

/*
 * vrpc_log_receive
 *
 * Clean up the rest of the code by putting this common sequence into an
 * inline function.
 */
static void vrpc_log_receive (
    vinesipcsocket *socket,
    ushort type,
    ushort id,
    int numargs,
    ulong num1,
    ulong num2)
{
    char *portname, text[30];
    int dport, sport;

    if (!vinesnetrpc_debug)
	return;

    dport = socket->last_local_port;
    sport = socket->last_remote_port;
    if (sport <= VIPC_PORT_LAST_WKP) {
	portname = vipc_portname(sport);
    } else {
	portname = vipc_portname(dport);
    }
    switch (numargs) {
      case 2:
	sprintf(text, "(%u-%u)", num1, num2);
	break;
      case 1:
	sprintf(text, "(%u)", num1);
	break;
      default:
	text[0] = '\0';
	break;
    }

    buginf("\nVRPC: received %s RPC %s%s from %#z", portname, vrpc_names[type],
	   text, socket->remote_net, socket->remote_host);
    if (id != 0)
	buginf(", id %d", id);
}

/*
 * vrpc_log_send
 *
 * Clean up the rest of the code by putting this common sequence into an
 * inline function.
 */
static void vrpc_log_send (
    ulong net,
    ushort host,
    int dport,
    int sport,
    int type,
    uint numargs,
    ulong num1,
    ulong num2)
{
    char *portname, text[30];

    if (!vinesnetrpc_debug)
	return;

    if (dport <= VIPC_PORT_LAST_WKP) {
	portname = vipc_portname(dport);
    } else {
	portname = vipc_portname(sport);
    }
    switch (numargs) {
      case 2:
	sprintf(text, "(%u-%u)", num1, num2);
	break;
      case 1:
	sprintf(text, "(%u)", num1);
	break;
      default:
	text[0] = '\0';
	break;
    }

    buginf("\nVRPC: sending %s RPC %s%s to %#z", portname, vrpc_names[type],
	   text, net, host);
}

/**********************************************************************
 *
 *			 INDIVIDUAL MESSAGES
 *
 **********************************************************************/

/**************************************************
 *
 *		    CALL MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_long_call
 *
 * Send a NetRPC "call" message over a reliable IPC connection.  This
 * mesage should elicit a "reject", "abort", or "return" message from the
 * other entity.  Unfortunately, Banyan does not use the transaction
 * field in the NetRPC header, so the application level has to remember
 * which procedure it called last and assume that any response is from
 * that procedure.  This routine takes care of remembering the procedure
 * being called.
 */
void vrpc_send_long_call (
    vinesipcsocket *socket,
    paktype *pak,
    ulong program,
    ushort version,
    ushort procedure,
    ushort dport,
    ushort sport)
{
    vrpc_call *rpc;

    if (vrpc_check_args(socket, pak, VRPC_CALL) == BAD)
	return;
    pak->session_start = pak->application_start - sizeof(vrpc_call);
    pak->datagramsize += sizeof(vrpc_call);
    rpc = (vrpc_call *)pak->session_start;
    rpc->type = VRPC_CALL;
    rpc->id = vines_rpc_id++;
    PUTLONG(&rpc->program, program);
    rpc->version = version;
    rpc->procedure = procedure;

    vrpc_log_send(socket->remote_net, socket->remote_host, dport, sport,
		  VRPC_CALL, 1, procedure, -1);
    vipc_send_data(socket, pak, dport, sport, VINES_HOPS);
}

/*
 * vrpc_send_broadcast_call
 *
 * Send a broadcast NetRPC "call" message.  This is done over unreliable
 * IPC since it is a broadcast message.  This mesage should not elicit
 * any response.
 */
void vrpc_send_broadcast_call (
    paktype *pak,
    vinesidbtype *use,
    vinesidbtype *exclude,
    ulong program,
    ushort version,
    ushort procedure,
    ushort dport,
    ushort sport,
    ushort flags,
    ushort ttl)
{
    vrpc_call *rpc;

    pak->session_start = pak->application_start - sizeof(vrpc_call);
    pak->datagramsize += sizeof(vrpc_call);

    rpc = (vrpc_call *)pak->session_start;
    rpc->type = VRPC_CALL;
    rpc->id = vines_rpc_id++;
    PUTLONG(&rpc->program, program);
    rpc->version = version;
    rpc->procedure = procedure;

    vrpc_log_send(VINES_ALLNETS, VINES_ALLHOSTS, dport, sport, VRPC_CALL,
		  1, procedure, -1);
    vipc_send_broadcast_datagram(pak, use, exclude, dport, sport, flags, ttl);
}

/*
 * vrpc_rcv_call
 *
 * Receive a NetRPC call message from another entity.  If this message
 * was directed specificly to this station, then it should respond with a
 * "reject", "abort", or "return" message.  If this message was a
 * broadcast, then there should be no response to this message.
 */
static boolean vrpc_rcv_call (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_call *rpc;

    /*
     * Call the appropriate service routine
     */
    rpc = (vrpc_call *)data;
    length -= sizeof(vrpc_call);
    socket->last_netrpc_id = rpc->id;
    vrpc_log_receive(socket, VRPC_CALL, rpc->id, 1, rpc->procedure, -1);
    if (vinesnetrpc_debug)
	buginf("\n        program %d, version %d", GETLONG(&rpc->program),
	       rpc->version);
    vsvc_dispatch_call(socket, service, rpc, rpc->data, length);
    return(TRUE);
}	

/**************************************************
 *
 *		   REJECT MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_reject
 *
 * Send a NetRPC "reject" message back to the source of the last "call"
 * messge.  A reject is sent when the recipient of a message does not
 * support one of the values in the header of the call.  The most common
 * usage of this routine is for procedures that are not implemented in
 * the router.
 */
void vrpc_send_reject (
    vinesipcsocket *socket,
    ushort reason)
{
    paktype *pak;
    vrpc_reject *rpc;
    int dport, sport;

    pak = vrpc_getbuffer(VRPC_REJECT, 0);
    if (vrpc_check_args(socket, pak, VRPC_REJECT) == BAD)
	return;
    rpc = (vrpc_reject *)pak->session_start;
    rpc->id = socket->last_netrpc_id;
    rpc->reason = reason;

    dport = socket->last_remote_port;
    sport = socket->last_local_port;
    vrpc_log_send(socket->remote_net, socket->remote_host, dport, sport,
		  VRPC_REJECT, 1, reason, -1);
    vipc_send_data(socket, pak, dport, sport, VINES_HOPS);
}

/*
 * vrpc_rcv_reject
 *
 * Since this code does not currently support sending directed "call"
 * messages, this routine should never be called.  The calling routine
 * prints a message that a "reject" frame was received, so that it is
 * obvious if this point is reached.
 */
static void vrpc_rcv_reject (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_reject *rpc;

    /*
     * Call the appropriate service routine.  This currently does
     * nothing.
     */
    rpc = (vrpc_reject *)data;
    vrpc_log_receive(socket, VRPC_REJECT, rpc->id, 1, rpc->reason, -1);
    vsvc_dispatch_reject(socket, service, rpc->reason);
}	

/**************************************************
 *
 *		   ABORT MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_abort
 *
 * Send a NetRPC "abort" message back to the source of the last "call"
 * messge.  An abort is sent when the recipient of a message cannot
 * complete the processing of a request.
 */
void vrpc_send_abort (
    vinesipcsocket *socket,
    ushort reason)
{
    paktype *pak;
    vrpc_abort *rpc;
    int dport, sport;

    pak = vrpc_getbuffer(VRPC_ABORT, 0);
    if (vrpc_check_args(socket, pak, VRPC_ABORT) == BAD)
	return;
    rpc = (vrpc_abort *)pak->session_start;
    rpc->id = socket->last_netrpc_id;
    rpc->reason = reason;

    dport = socket->last_remote_port;
    sport = socket->last_local_port;
    vrpc_log_send(socket->remote_net, socket->remote_host, dport, sport,
		  VRPC_ABORT, 1, reason, -1);
    vipc_send_data(socket, pak, dport, sport, VINES_HOPS);
}

/*
 * vrpc_rcv_abort
 *
 * Since this code does not currently support sending directed "call"
 * messages, this routine should never be called.  The calling routine
 * prints a message that a "abort" frame was received, so that it is
 * obvious if this point is reached.
 */
static void vrpc_rcv_abort (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_abort *rpc;

    /*
     * Call the appropriate service routine.  This currently does
     * nothing.
     */
    rpc = (vrpc_abort *)data;
    vrpc_log_receive(socket, VRPC_ABORT, rpc->id, 1, rpc->reason, -1);
    vsvc_dispatch_abort(socket, service, rpc->reason);
}	

/**************************************************
 *
 *		   RETURN MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_return
 *
 * Send a NetRPC "return" message back to the source of the last "call"
 * messge.  A return is sent when the recipient of a message is able to
 * completely process the request, and contains any information that
 * results from this processing.  This is the normal response to a call
 * request message.
 */
void vrpc_send_return (
    vinesipcsocket *socket,
    paktype *pak)
{
    vrpc_return *rpc;
    int dport, sport;

    if (vrpc_check_args(socket, pak, VRPC_RETURN) == BAD)
	return;
    pak->session_start = pak->application_start - sizeof(vrpc_return);
    pak->datagramsize += sizeof(vrpc_return);
    rpc = (vrpc_return *)pak->session_start;
    rpc->type = VRPC_RETURN;
    rpc->id = socket->last_netrpc_id;

    dport = socket->last_remote_port;
    sport = socket->last_local_port;
    vrpc_log_send(socket->remote_net, socket->remote_host, dport, sport,
		  VRPC_RETURN, 0, -1, -1);
    vipc_send_data(socket, pak, dport, sport, VINES_HOPS);
}

/*
 * vrpc_rcv_return
 *
 * This routine is called when a valid response to a router's query
 * arrives back at the source.  Unfortunately, Banyan does not use the
 * transaction field in the NetRPC header, so the application level has
 * to remember which procedure it called last and assume that any
 * response is from that procedure.
 */
static boolean vrpc_rcv_return (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_return *rpc;

    /*
     * Call the appropriate service routine
     */
    rpc = (vrpc_return *)data;
    length -= sizeof(vrpc_return);
    vrpc_log_receive(socket, VRPC_RETURN, rpc->id, 0, -1, -1);
    vsvc_dispatch_return(socket, service, rpc->data, length);
    return(TRUE);
}	

/**************************************************
 *
 *		   SEARCH MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_search
 *
 * This routine is used to find an instance of a particular well known
 * service.  The service type is defined by the port number, and the
 * revisions allowed are specified in the NetRPC header itself.
 */
void vrpc_send_search (
    ushort revision,
    ulong  min,
    ulong  max,
    ushort user_num,
    char   *user_string,
    char   *string1,
    char   *string2,
    char   *string3,
    ushort dport,
    ushort sport,
    int hops)
{
    vrpc_search *rpc;
    paktype *pak;
    int length;
    uchar *c;

    length = strlen(user_string) + strlen(string1) +
	strlen(string2) + strlen(string3);
    pak = vrpc_b_getbuffer(length);
    if (!pak)
	return;

    rpc = (vrpc_search *)pak->session_start;
    rpc->type           = VRPC_SEARCH;
    rpc->revision       = revision;
    rpc->min_s          = min & 0xFFFF;
    rpc->max_s          = max & 0xFFFF;
    rpc->userdef_number = user_num;
    rpc->min_l          = min;
    rpc->max_l          = max;
    c = (uchar *)&rpc->userdef_length;
    c = vines_insert_smstring(c, user_string, 255);
    c = vines_insert_smstring(c, string1, 255);
    c = vines_insert_smstring(c, string2, 255);
    c = vines_insert_smstring(c, string3, 255);
    pak->datagramsize = c - (uchar *)rpc;

    if (vinesnetrpc_debug) {
	vrpc_log_send(VINES_ALLNETS, VINES_ALLHOSTS, dport, sport,
		      VRPC_SEARCH, 2, min, max);
	buginf("\n        args: %d, <%s>, <%s>, <%s>, <%s>",
	       user_num, user_string, string1, string2, string3);
	buginf(" : %u bytes", pak->datagramsize);
    }
    vipc_send_broadcast_datagram(pak, NULL, NULL, dport, sport,
				 VINES_B_SERVERS, hops);
}

/*
 * vrpc_rcv_search
 *
 * Someone is looking for a single instance of a well known service.
 * Since the router does not provide any complete services, it should
 * never respond to a search message.  However, this might be a good
 * place to short circuit searches in a future release.
 */
static boolean vrpc_rcv_search (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_search *rpc;
    char *info;
    uchar *c;

    /*
     * Print some debugging.  Includes figuring out where the strings
     * live in the data field.
     */
    rpc = (vrpc_search *)data;
    vrpc_log_receive(socket, VRPC_SEARCH, 0, 2, rpc->min_l, rpc->max_l);

    info = malloc(1024 /* 4 strings of max 255 plus terminator */);
    if (info == FALSE)
	return(FALSE);
    c = (uchar *)&rpc->userdef_length;
    c = vines_extract_smstring(c, info + 0, 255);
    c = vines_extract_smstring(c, info + 256, 255);
    c = vines_extract_smstring(c, info + 512, 255);
    c = vines_extract_smstring(c, info + 768, 255);
    if (vinesnetrpc_debug) {
	buginf("\n        args: <%d>, <%s>, <%s>, <%s>, <%s>",
	       rpc->userdef_number, info, info+256, info+512,
	       info+768);
	if ((c - data) != length)
	    buginf("\nVRPC: Malformed packet, parsed %u bytes, %u actual.",
		   (c - data), length);
    }

    /*
     * Now make sure that this service falls in the range of services
     * requested by the sender.  If the router version of the service is
     * not supposed to answer broadcasts, then make sure to return false
     * so that the serverless code will helper this.
     */
    if (!service->answer_search) {
	free(info);
	return(FALSE);
    }

    /*
     * time search request from severless segment,
     * answer it. Otherwise, let real servers respond.
     */
    if (socket->remote_host != VINES_SERVER &&
	vines_servers_present(socket->last_idb_in)) {
	free(info);
	return(FALSE);
    }

    if (service->version &&
	(service->version >= rpc->min_l) &&
	(service->version <= rpc->max_l)) {
	vsvc_dispatch_search(socket, service, rpc->userdef_number,
			     info, info+256, info+512, info+768);
    }
    free(info);
    return(TRUE);
}
	

/**************************************************
 *
 *		 SEARCH ALL MESSAGES
 *
 **************************************************/

/*
 * vrpc_rcv_searchall
 *
 * Someone is looking for all instances of a well known service.  Since
 * the router does not provide any complete services, it should never
 * respond to a search message.  However, this might be a good place to
 * short circuit searches in a future release.
 */
static boolean vrpc_rcv_searchall (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_search *rpc;
    char *info;
    uchar *c;

    /*
     * Print some debugging.  Includes figuring out where the strings
     * live in the data field.
     */
    rpc = (vrpc_search *)data;
    vrpc_log_receive(socket, VRPC_SEARCHALL, 0, 2, rpc->min_l, rpc->max_l);

    info = malloc(1024 /* 4 strings of max 255 plus terminator */);
    if (info == FALSE)
	return(FALSE);
    c = (uchar *)&rpc->userdef_length;
    c = vines_extract_smstring(c, info + 0, 255);
    c = vines_extract_smstring(c, info + 256, 255);
    c = vines_extract_smstring(c, info + 512, 255);
    c = vines_extract_smstring(c, info + 768, 255);
    if (vinesnetrpc_debug) {
	buginf("\n        args: <%d>, <%s>, <%s>, <%s>, <%s>",
	       rpc->userdef_number, info, info+256, info+512,
	       info+768);
	if ((c - data) != length)
	    buginf("\nVRPC: Malformed packet, parsed %u bytes, %u actual.",
		   (c - data), length);
    }

    /*
     * Now make sure that this service falls in the range of services
     * requested by the sender.  If the router version of the service is
     * not supposed to answer broadcasts, then make sure to return false
     * so that the serverless code will helper this.
     */
    if (!service->answer_search) {
	free(info);
	return(FALSE);
    }
    if (service->version &&
	(service->version >= rpc->min_l) &&
	(service->version <= rpc->max_l)) {
	vsvc_dispatch_search(socket, service, rpc->userdef_number,
			     info, info+256, info+512, info+768);
    }
    free(info);
    return(TRUE);
}

/**************************************************
 *
 *	       RETURN ADDRESS MESSAGES
 *
 **************************************************/

/*
 * vrpc_send_returnaddress_common
 *
 *
 */
static inline paktype *vrpc_send_returnaddress_common (
    vinesipcsocket *socket,
    ulong network,
    ushort host,
    ushort proto_rev,
    ulong  service_rev,
    ushort dport,
    ushort sport)
{
    vrpc_returnaddress *rpc;
    paktype *pak;

    pak = vrpc_getbuffer(VRPC_RETURNADDRESS, 0);
    if (vrpc_check_args(socket, pak, VRPC_RETURNADDRESS) == BAD) {
	if (pak)
	    retbuffer(pak);
	return(NULL);
    }
    rpc = (vrpc_returnaddress *)pak->session_start;
    rpc->proto_rev = proto_rev;
    rpc->service_rev_s = service_rev & 0xFFFF;
    PUTLONG(&rpc->service_rev_l, service_rev);
    rpc->address.family = VINES_ADDR_NATIVE;
    PUT2WD(rpc->address.f.vip.nnet, network);
    rpc->address.f.vip.host = host;
    rpc->address.f.vip.port = sport;
    memset(rpc->address.f.vip.fill, 0, sizeof(rpc->address.f.vip.fill));
    vrpc_log_send(socket->remote_net, socket->remote_host, dport, sport,
		  VRPC_RETURNADDRESS, 1, service_rev, -1);
    return(pak);
}

/*
 * vrpc_send_returnaddress
 *
 *
 */
void vrpc_send_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    ushort dport,
    ushort sport)
{
    paktype *pak;

    pak = vrpc_send_returnaddress_common(socket, vines_network, vines_host,
					 proto_rev, service_rev, dport, sport);
    if (pak == NULL)
	return;
    vipc_send_datagram(pak, socket->remote_net, socket->remote_host,
		       dport, sport, VINES_HOPS);
}

/*
 * vrpc_rcv_returnaddress
 *
 *
 */
static void vrpc_rcv_returnaddress (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *data,
    int length)
{
    vrpc_returnaddress *rpc;

    /*
     * Call the appropriate service routine
     */
    rpc = (vrpc_returnaddress *)data;
    vrpc_log_receive(socket, VRPC_RETURNADDRESS, 0, 1, rpc->service_rev_l, -1);

    vsvc_dispatch_returnaddress(socket, service, rpc->proto_rev,
				rpc->service_rev_l, &rpc->address);
}	

/**********************************************************************
 *
 *		    INTERFACE FROM VINES IPC LAYER
 *
 **********************************************************************/

/*
 * vines_netrpc_receive
 *
 * Receive a frame that contains a netrpc header.  Copy off onformation
 * the may need to be used later, and then call a routine that handles
 * that individual type of NetRPC frame.
 */
boolean vines_netrpc_receive (
    vinesipcsocket *socket,
    uchar *data,
    int length)
{
    vinesnetrpctype *rpc;
    boolean processed = FALSE;
    vsvc_info *service;

    service = vsvc_find_service(socket->last_local_port);
    if (!service) {
	errmsg(&msgsym(RPCNOSERVICE, VINES), socket->last_local_port);
        return(TRUE);
    }

    rpc = (vinesnetrpctype *)data;
    switch (rpc->m.call.type) {
      case VRPC_CALL:
	processed = vrpc_rcv_call(socket, service, data, length);
	break;
	
      case VRPC_REJECT:
	vrpc_rcv_reject(socket, service, data, length);
	processed = TRUE;
	break;
	
      case VRPC_RETURN:
	processed = vrpc_rcv_return(socket, service, data, length);
	break;
	
      case VRPC_ABORT:
	vrpc_rcv_abort(socket, service, data, length);
	processed = TRUE;
	break;
	
      case VRPC_SEARCH:
	processed = vrpc_rcv_search(socket, service, data, length);
	break;
	
      case VRPC_RETURNADDRESS:
	vrpc_rcv_returnaddress(socket, service, data, length);
	processed = TRUE;
	break;
	
      case VRPC_SEARCHALL:
	processed = vrpc_rcv_searchall(socket, service, data, length);
	break;
	
      default:
	vrpc_log_receive(socket, rpc->m.call.type, 0, 0, -1, -1);
	processed = FALSE;
	break;
    }
    return(processed);
}

/*
 * vines_netrpc_extract_procedure
 *
 * Given an RPC message, make sure it is a "call" message and then
 * extract the procedure number.  This is used by lower level helper
 * routines when debugging is turned on.
 */
ushort vines_netrpc_extract_procedure (paktype *pak)
{
    vinesnetrpctype *rpc;

    rpc = (vinesnetrpctype *)pak->session_start;
    if (rpc->m.call.type != VRPC_CALL)
	return(-1);
    return(rpc->m.call.procedure);
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * display_vinespak_netrpc
 *
 * Print out the NetRPC information in a packet.
 */
uchar *display_vinespak_netrpc (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vinesnetrpctype *rpc;
    ulong *ptr;

    rpc = (vinesnetrpctype *)data;
    switch (rpc->m.call.type) {
      case VRPC_CALL:
	(*fn)("\n   RPC call, id %d, program %d, vers %d, procedure %d",
	      rpc->m.call.id, rpc->m.call.program, rpc->m.call.version,
	      rpc->m.call.procedure);
	data = rpc->m.call.data;
	*limit -= 12;
	break;
	
      case VRPC_REJECT:
	(*fn)("\n   RPC reject, id %d, reason %d",
	      rpc->m.reject.id, rpc->m.reject.reason);
	*limit = 0;
	break;
	
      case VRPC_RETURN:
	(*fn)("\n   RPC return, id %d", rpc->m.retn.id);
	data = rpc->m.retn.data;
	*limit -= 4;
	break;
	
      case VRPC_ABORT:
	(*fn)("\n   RPC abort, id %d, reason %d",
	      rpc->m.reject.id, rpc->m.reject.reason);
	*limit = 0;
	break;
	
      case VRPC_SEARCH:
	(*fn)("\n   RPC search, revision %02x, min %04x, max %04x, user %02x",
	      rpc->m.search.revision, rpc->m.search.min_l,
	      rpc->m.search.max_l, rpc->m.search.userdef_number);
	*limit = 0;
	break;
	
      case VRPC_RETURNADDRESS:
	ptr = (ulong *) &rpc->m.retnaddress.address;
	(*fn)("\n   RPC return addr, proto %02x, service %04x, address %08x%08x",
	      rpc->m.retnaddress.proto_rev, rpc->m.retnaddress.service_rev_l,
	      ptr[0], ptr[1]);
	*limit = 0;
	break;
	
      case VRPC_SEARCHALL:
	(*fn)("\n   RPC searchall, revision %02x, min %04x, max %04x, user %02x",
	      rpc->m.searchall.revision, rpc->m.searchall.min_l,
	      rpc->m.searchall.max_l, rpc->m.searchall.userdef_number);
	*limit = 0;
	break;
	
      default:
	(*fn)("\n   RPC type %04x", rpc->m.unknown.type);
	data = rpc->m.unknown.data;
	*limit -= 2;
	break;
    }
    return(data);
}
