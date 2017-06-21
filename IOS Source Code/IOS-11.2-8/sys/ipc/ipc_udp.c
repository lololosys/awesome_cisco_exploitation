/* $Id: ipc_udp.c,v 3.6.10.3 1996/07/06 05:54:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_udp.c,v $
 *------------------------------------------------------------------
 * Mainline placeholder file for IPC system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_udp.c,v $
 * Revision 3.6.10.3  1996/07/06  05:54:22  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.6.10.2  1996/05/21  09:52:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  20:18:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/16  07:00:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.2  1996/03/07  09:47:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  01:01:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:34:50  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/24  20:15:50  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/19  01:01:00  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:30:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:29:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:05:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  00:13:02  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.2  1995/06/28  09:27:09  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:03:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "media_registry.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"

#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_message.h"
#include "ipc_seats.h"
#include "ipc_ports.h"
#include "ipc_test.h"

#include "ipc_udp.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "ipc_udp_chain.h"
#include "ipc_debug.h"


ipsocktype *ipc_udp_comm_socket;
ipaddrtype ipc_udp_comm_address;

ipc_global_info *ipc_globals;

static parser_extension_request ipc_udp_chain_init_table[] = {
    { PARSE_ADD_CFG_LAST_CMD, &pname(config_ipc_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Send an IPC message out to the remote udp socket.
 * This routine is the send_vector for the seat we're transmitting to.
 */

static boolean ipc_udp_transmit (ipc_message *message, ipc_seat_data *seat)
{
    paktype *socket_pak;
    int offset, data_len;
    uchar *udp_data;

    data_len = message->header->size + IPC_MESSAGE_ENCAPS_SIZE;
    socket_pak = setup_ipsocket(seat->transport.udp.socket, data_len, &offset);

    if (socket_pak == NULL) return TRUE;

    udp_data = socket_pak->data_area + offset;
    socket_pak->datagramsize = data_len + offset - ENCAPBYTES;

    bcopy(message->header, udp_data, data_len);

    write_ipsocket(seat->transport.udp.socket, socket_pak, data_len);

    return FALSE;
}

/*
 * ipc_send_udp_seat_assignment
 *
 * Send a seat assignment request to the IPC master server.
 */

static void ipc_send_udp_seat_assignment (ipsocktype *socket)
{
    paktype *socket_pak;
    ipc_message_header *ipc_header;
    ipc_boot_request_v1 *bootstrap;

    int offset, data_len;

    data_len = IPC_MESSAGE_ENCAPS_SIZE + sizeof(ipc_boot_request_v1) +
	strlen(hostname);
    
    socket_pak = setup_ipsocket(socket, data_len, &offset);

    ipc_header = (ipc_message_header *) (socket_pak->data_area + offset);
    socket_pak->datagramsize = data_len + offset - ENCAPBYTES;

    memset(ipc_header, 0, IPC_MESSAGE_ENCAPS_SIZE);

    ipc_header->flags |= IPC_FLAG_BOOTSTRAP;
    ipc_header->type = IPC_BOOTSTRAP_REGISTER_REQUEST_V1;
    ipc_header->size = data_len - IPC_MESSAGE_ENCAPS_SIZE;

    bootstrap = (ipc_boot_request_v1 *)ipc_header->data;
    bootstrap->transport_type = IPC_SEAT_UDP;
    bootstrap->name_length = strlen(hostname);
    bcopy(hostname, bootstrap->name, bootstrap->name_length);

    write_ipsocket(socket, socket_pak, data_len);
}


/*
 * ipc_request_udp_seat_assignment:
 *
 * Contacts the IPC Master server at the specified IP address and
 * asks it to assign us a new "seat" id, because we are giving up control
 * and joining the IPC cloud.
 */

static void ipc_request_udp_seat_assignment (ipaddrtype remote_address)
{
    addrtype faddr;

    /*
     * If we're already using an open socket, we should close it.
     */    
    
    if (ipc_udp_comm_socket != NULL) {
	close_ipsocket(ipc_udp_comm_socket);
	ipc_udp_comm_address = 0L;
    }

    if (remote_address) {
	faddr.type = ADDR_IP;
	faddr.ip_addr = remote_address;
	ipc_udp_comm_socket = open_ipsocket(UDP_PROT,
					    NULL, /* Any local address */
					    &faddr,
					    IPC_PORT,
					    IPC_PORT,
					    0 /* No status flags */);

	if (ipc_udp_comm_socket == NULL) {
	    /*
	     * Print a somewhat nasty log message
	     */
	}
    }
    ipc_udp_comm_address = remote_address;

    /* Send the seat assignment (i.e. REGISTRATION) request. */

    ipc_send_udp_seat_assignment(ipc_udp_comm_socket);
}

/*
 * ipc_config_master_udp:
 *
 * Parser hook for specifying ipc master server.
 * "[no] ipc master <ip-address>"
 */

void ipc_config_master_udp (parseinfo *csb)
{
    if (csb->nvgen) {
	if (ipc_udp_comm_address)
	    nv_write(ipc_udp_comm_address, "%s %i",
		 csb->nv_command,
		 ipc_udp_comm_address);
    } else {
	if (csb->sense) {
	    ipc_udp_comm_address = GETOBJ(paddr,1)->ip_addr;
	    if (ipc_debug_events) {
		buginf("\nIPC: master set to %i", ipc_udp_comm_address);
	    }
	    ipc_request_udp_seat_assignment(ipc_udp_comm_address);
	}
	else
	    ipc_udp_comm_address = 0;
    }
}

/*
 * ipc_respond_udp:
 *
 * Send a registration response to the (newly created) seat in question.
 *
 * Used by IPC Master to respond to slave bootstrap request.  The master has
 * already created a "seat" 
 */

static void ipc_udp_respond_with_assignment (ipc_seat_data *the_seat)
{
    ipc_message *the_message;
    ipc_boot_response_v1 *response;

    the_message = ipc_get_message(sizeof(ipc_boot_response_v1),
				  0,
				  IPC_BOOTSTRAP_REGISTER_RESPONSE_V1);

    if (the_message == NULL) return;

    the_message->header->flags |= IPC_FLAG_BOOTSTRAP;

    response = the_message->data;

    response->error_code = IPC_OK;
    response->seat_id = the_seat->seat;
    response->master_control_port = ipc_globals->my_server_port;
    
    ipc_udp_transmit(the_message, the_seat);
    
    ipc_return_message(the_message);
}

/*
 * ipc_udp_process_bootstrap_resp_v1
 *
 * Process a bootstrap response sent by the IPC master server.
 * It has our new seat number, so we have to renumber our local
 * seat datastructure and create a new seat datastructure
 * for the IPC master server.
 */

static void ipc_udp_process_bootstrap_resp_v1 (paktype *pak,
					ipc_message_header *ipc_header)
{
    ipc_seat_data *our_seat;
    ipc_seat_data *new_master;
    ipc_boot_response_v1 *response;
    char control_port_name[IPC_PORT_NAME_MAX];
    ipc_seat seat_id;
    ushort error_code;
    uint   control_port;

    response = (ipc_boot_response_v1 *)ipc_header->data;
    
    error_code = response->error_code;
    seat_id = response->seat_id;
    control_port = response->master_control_port;

    datagram_done(pak);

    if (ipc_debug_seats) {
	buginf("\nIPC: Got registration response: %s seat=%d",
	       ipc_decode_error(response->error_code),
	       seat_id);
    }

    if (error_code != IPC_OK) return;

    /*
     * Get our current seat and renumber/rename it, keeping the other
     * fields intact.   Renumber the currently open ports so they'll still
     * work. :-)
     */

    our_seat = ipc_get_seat_info();
    if (our_seat == NULL) return;

    ipc_renumber_seat(our_seat, seat_id, hostname);

    /*
     * Make a new master seat 
     */

    new_master = ipc_create_seat(IPC_MASTER_SEAT,
				 IPC_MASTER_NAME);

    new_master->type = IPC_SEAT_UDP;
    new_master->interrupt_ok = FALSE;
    new_master->transport.udp.socket = ipc_udp_comm_socket;
    new_master->send_vector = ipc_udp_transmit;

    ipc_add_seat(new_master);

    sprintf(control_port_name,"%s%c%s", IPC_MASTER_NAME,
	    IPC_PORT_NAME_SEPARATOR, IPC_CONTROL_PORT_NAME);

    ipc_create_port_entry(control_port_name, control_port);

    ipc_globals->is_ipc_master = FALSE;
    ipc_globals->local_seat_addr = seat_id;

    /*
     * That was painful. :-)
     */
}

/*
 * ipc_udp_process_bootstrap_reg_v1
 *
 * Process bootstrap registration requests from a server wishing to become
 * a slave.  We assign a new seat number (after checking to see that he's not
 * already registered) and send a boot response message back.
 */

static void ipc_udp_process_bootstrap_reg_v1 (paktype *pak,
				       ipc_message_header *ipc_header,
				       ipaddrtype source_ip_addr,
				       ushort source_ip_port)
{
    char *seat_name;
    ipc_seat_data *the_seat;
    ipc_error_code error;
    ipsocktype *socket;
    addrtype faddr;

    ipc_boot_request_v1 *request;

    request = (ipc_boot_request_v1 *)ipc_header->data;

    seat_name = ipc_extract_string_copy(request->name_length,
					request->name);

    if (ipc_debug_events) {
	buginf("\nIPC: Registration request for seat '%s'",seat_name);
    }

    /*
     * If the seat hasn't been registered before, let's assign the seat id
     * and respond.  If not, check to make sure the IP addresses are the same.
     * otherwise we may have a duplicate seat, or two transports trying to
     * register at the same time.
     */

    the_seat = ipc_assign_seat(seat_name, &error);

    if (error == IPC_SEAT_IN_USE) {
	if (ipc_debug_seats) {
	    buginf("\nIPC: Tried to register with a name already in use.");
	}
	/* Check IP addresses to make sure we don't have a duplicate name */
	if (the_seat->type == IPC_SEAT_UDP &&
	    the_seat->transport.udp.dest_udp_addr == source_ip_addr) {
	    ipc_udp_respond_with_assignment(the_seat);
	    if (ipc_debug_seats) {
		buginf("\nIPC: Got duplicate seat registration.");
	    }
	}
	return;
    }

    if (error != IPC_OK) return;

    /* 
     * The seat isn't in use, so set up the fields and open a socket
     * back to the sender and hook it in to the transmit vector for this
     * seat.
     */


    faddr.type = ADDR_IP;
    faddr.ip_addr = source_ip_addr;
    socket = open_ipsocket(UDP_PROT,
			   NULL,
			   &faddr,
			   IPC_PORT,
			   source_ip_port,
			   0);

    if (socket == NULL) {
	return;
    }
    
    the_seat->type = IPC_SEAT_UDP;
    the_seat->transport.udp.dest_udp_addr = source_ip_addr;
    the_seat->transport.udp.socket = socket;
    the_seat->send_vector = ipc_udp_transmit;
    the_seat->interrupt_ok = FALSE;

    ipc_add_seat(the_seat);
    ipc_udp_respond_with_assignment(the_seat);
}

/*
 * We received a bootstrap packet from our udp port, we should
 * de-mux it and hand it off the to appropriate processing routing so
 * it can be answered or discarded. 
 */

static void ipc_udp_process_bootstrap(paktype *pak, udptype *udp)
{
    ipc_message_header *ipc_header;
    iphdrtype *ip_header;
    ipaddrtype source_ip_addr;

    ipc_header = (ipc_message_header *)udp->udpdata;
    ip_header = iphdrtype_start(pak);
    source_ip_addr = ip_header->srcadr;

    switch (ipc_header->type) {
    case IPC_BOOTSTRAP_REGISTER_REQUEST_V1:
	if (ipc_debug_seats)
	    buginf("\nIPC: UDP Bootstrap registration request from %i.",
		   source_ip_addr);

	ipc_udp_process_bootstrap_reg_v1(pak, ipc_header,
					 source_ip_addr, udp->sport);
	break;
    case IPC_BOOTSTRAP_REGISTER_RESPONSE_V1:
	if (ipc_debug_seats)
	    buginf("\nIPC: UDP Bootstrap response from %i.",
		   source_ip_addr);
	ipc_udp_process_bootstrap_resp_v1(pak, ipc_header);
	break;
    default:
	if (ipc_debug_seats)
	    buginf("\nIPC: Unknown bootstrap request %d from %i.",
		   ipc_header->type, source_ip_addr);
	datagram_done(pak);
    }
}

static boolean ipc_udp_input (paktype *pak, udptype *udp)
{
    ipc_message_header *header;

    if (udp->dport != IPC_PORT)
	return FALSE;

    clear_if_input(pak);

    header = (ipc_message_header *)udp->udpdata;

    if (header->flags & IPC_FLAG_BOOTSTRAP)
	ipc_udp_process_bootstrap(pak, udp);
    else {
	pak->datagramstart = (uchar *)udp->udpdata;
	ipc_process_raw_pak(pak);
    }    
    return TRUE;
}

static void ipc_udp_subsys_init (subsystype *subsys)
{
    ipc_udp_comm_socket = NULL;

    /* Register a listener for our UDP traffic. */

    reg_add_ip_udp_input(ipc_udp_input, "ipc_udp_input");

    /* Hook in parse chains */

    parser_add_command_list(ipc_udp_chain_init_table,"ipc udp xport cmds");
}

#define IPC_UDP_MAJOR_VERSION 1
#define IPC_UDP_MINOR_VERSION 0
#define IPC_UDP_EDIT_VERSION 0

SUBSYS_HEADER(ipc_udp,
	      IPC_UDP_MAJOR_VERSION,
	      IPC_UDP_MINOR_VERSION,
	      IPC_UDP_EDIT_VERSION,
	      ipc_udp_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, 
	      "req: iphost ipc");

