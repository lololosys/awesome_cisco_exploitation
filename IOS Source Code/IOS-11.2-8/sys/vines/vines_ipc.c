/* $Id: vines_ipc.c,v 3.7.12.5 1996/08/28 13:19:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_ipc.c,v $
 *------------------------------------------------------------------
 * vines_ipc.c - Support for the vines InterProcess Communications (IPC)
 *               protocol.
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_ipc.c,v $
 * Revision 3.7.12.5  1996/08/28  13:19:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.12.4  1996/08/07  09:03:38  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.12.3  1996/07/23  13:30:22  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.12.2  1996/03/27  18:34:43  slin
 * CSCdi51689:  SYS-3-MGDTIMER Uninitialized timer in VINES Protocols
 * Branch: California_branch
 *
 * - Initialize timers for pseudo-sockets
 *
 * Revision 3.7.12.1  1996/03/18  22:31:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/16  07:52:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.1  1996/03/07  11:04:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/02/14  01:05:57  ehk
 * CSCdi47766:  Vines-2-nomemory - service query crash
 * added IPC packet length check.
 *
 * Revision 3.6  1996/01/22  07:44:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/07  22:33:29  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/01  21:38:59  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/21  01:21:50  slin
 * CSCdi40757:  Vines: StreetTalk delays on serverless segments
 * - Send IPC error if StreetTalk not enabled
 *
 * Revision 3.2  1995/11/17  17:57:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/24  22:32:54  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.2  1995/07/07 20:40:04  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.1  1995/06/07 23:14:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
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
#include "vsvc_st.h"

/*
 * Globals
 */
ushort vipc_port_number;
queuetype vipc_portQ;

ushort vipc_connection_number;
mgd_timer vipc_timer;
queuetype vines_socketQ;

ulong vines_broadcast[VIPC_PORT_MAXCOUNTED];	/* received by router */
ulong vines_helpered[VIPC_PORT_MAXCOUNTED];	/* sent toward real server */
ulong vines_unicast[VIPC_PORT_MAXCOUNTED];	/* sent directly to me */
ulong vines_proxied[VIPC_PORT_MAXCOUNTED];	/* proxied to real server */
ulong vines_proxyback[VIPC_PORT_MAXCOUNTED];	/* proxy replies from real server */

static char *const vipc_types[] = {"Datagram", "Data", "Error", 
                                   "Disc", "Probe", "Ack"};
static uchar *const vipc_socket_states[] = {"null", "connecting", "connected",
			                    "idle", "dead"};

struct {
    ushort number;
    char *name;
} const vipc_port_names[] = {
    {0x0001, "Echo"},
    {0x0002, "Router"},
    {0x0003, "Back_End"},
    {0x0004, "Mail"},
    {0x0005, "Ftp"},
    {0x0006, "File"},
    {0x0007, "Server"},
    {0x000f, "Streettalk"}, 
    {0x0011, "Talk"},
    {0x0012, "NM"},
    {0x0013, "VS"},
    {0x0014, "IP"},
    {0x0015, "IP_ARP"},
    {0x0016, "STDA"},
    {0x0017, "STDA_Listen"},
    {0x0019, "NSM"},
    {0x001F, "Appletalk"},
    {0x0021, "IPX"},
    {0x015A, "VA"},
    {0x0176, "BIGEAR"},
    {0x0000, 0}
};


/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vines_init_ipc
 *
 * Perform initialization of IPC sockets, queues, etc.
 */
void vines_init_ipc (void)
{
    vipc_port_number = VIPC_PORT_FIRST_AVAIL;
    vipc_connection_number = 1;
    mgd_timer_init_parent(&vipc_timer, &vines_bkgnd_timer);
    queue_init(&vipc_portQ, 0);
    queue_init(&vines_socketQ, 0);
    vipc_create_port(VIPC_PORT_ECHO, "VINES Echo", FALSE, vines_echo_bounce);
    vipc_create_port(VIPC_PORT_CISCO_PROXY, "VINES Proxy", FALSE, vines_proxy_reply);
    vines_init_echo();
    vines_init_netrpc();
}

/*
 * vipc_traffic_init
 *
 * Initialize the VINES IPC protocol traffic array.
 */
void vipc_traffic_init (void)
{
    int i;

    FOR_ALL_VIPC_PORTS_COUNTED() {
	vines_broadcast[i] = 0;
	vines_helpered[i] = 0;
	vines_unicast[i] = 0;
	vines_proxied[i] = 0;
	vines_proxyback[i] = 0;
    }
}

/*
 * vipc_traffic_checkpoint
 *
 * Take a checkpoint of all IPC counters.  Since none of the IPC
 * counters are accessible via SNMP, they are simply cleared.
 */
void vipc_traffic_checkpoint (void)
{
    vipc_traffic_init();
}

/**********************************************************************
 *
 *			  TIMER MANIPULATION
 *
 **********************************************************************/

/*
 * vipc_bump_retrans_timer
 *
 * Increments the retransmission timer by the total delay for this
 * socket.  This routine is called when a retransmission takes place in
 * order to set the timer for the next retransmission.
 */
static inline sys_timestamp vipc_bump_retrans_timer (vinesipcsocket *socket)
{
    mgd_timer_start(&socket->next_retrans_time, socket->totalcost);
    return(mgd_timer_exp_time(&socket->next_retrans_time));
}

/*
 * vipc_set_retrans_timer
 *
 * Set the retransmission timer to a given value.  This routine is called
 * when a packet is transmitted for the first time.
 */
static inline sys_timestamp vipc_set_retrans_timer (vinesipcsocket *socket, sys_timestamp time)
{
    mgd_timer_set_soonest(&socket->next_retrans_time, time);
    return(mgd_timer_exp_time(&socket->next_retrans_time));
}

/*
 * vipc_clear_retrans_timer
 *
 * Clear the retransmission timer.
 */
static inline void vipc_clear_retrans_timer (vinesipcsocket *socket)
{
    mgd_timer_stop(&socket->next_retrans_time);
}
   
/*
 * vipc_set_idle_timer
 *
 * Set the idle timer to the current time plus thirty seconds.  This
 * routine is called every time a data is received on a socket.  Upon its
 * expiration, the connection will either be marked idle, marked dead, or
 * cleaned up.
 */
static inline void vipc_set_idle_timer (vinesipcsocket *socket)
{
    mgd_timer_start(&socket->next_idle_time, (30*ONESEC) + socket->totalcost);
}

/*
 * vipc_clear_idle_timer
 *
 * Clear the idle timer.
 */
static inline void vipc_clear_idle_timer (vinesipcsocket *socket)
{
    mgd_timer_stop(&socket->next_idle_time);
}
   
/*
 * vipc_set_ack_timer
 *
 * Set the acknowledgment timer to the current time plus the given
 * offset.  This routing is called every time a data is received on a
 * socket.  The offset should be 200 milliseconds in the normal case, or
 * zero if immediate acknowledgment was requested.
 */
static inline void vipc_set_ack_timer (vinesipcsocket *socket, ulong offset)
{
    mgd_timer_start(&socket->next_ack_time, offset);
}

/*
 * vipc_clear_ack_timer
 *
 * Clear the acknowledgement timer.
 */
static inline void vipc_clear_ack_timer (vinesipcsocket *socket)
{
    mgd_timer_stop(&socket->next_ack_time);
}
   

/**********************************************************************
 *
 *		   PORT AND CONNECTION MANIPULATION
 *
 **********************************************************************/

/*
 * vipc_allocate_port_number
 *
 * Provide a unique IPC port number.  This routine normally hands out
 * numerically increasing numbers, but it must check each number to
 * insure that it is not currently in use.  This could happen if the
 * number sequence were to wrap around from some reason, such as lots of
 * pings.
 */
static ushort vipc_allocate_port_number (void)
{
    vipc_port_info *port;
    ushort number, starting_port;

    starting_port = vipc_port_number;
    do {
	number = vipc_port_number++;
	if (vipc_port_number > VIPC_PORT_LAST_AVAIL)
	    vipc_port_number = VIPC_PORT_FIRST_AVAIL;
	
	/* Check to see if port is in use */
	FOR_ALL_VIPC_PORTS(port) {
	    if (port->number == number) {
		break;
	    }
	}
	if (port == NULL) {
	    return(number);
	}
    } while (starting_port != vipc_port_number);
    return(0);
}

/*
 * vipc_find_port
 *
 * Find an existing IPC port structure given its port number.  Returns
 * NULL if the port is not in use.
 */
vipc_port_info *vipc_find_port (ushort number)
{
    vipc_port_info *port;

    for (port = vipc_portQ.qhead; port; port = port->next) {
	if (port->number == number)
	    break;
    }
    return(port);
}

/*
 * vipc_create_port
 *
 * Create a new IPC port structure, and stick it onto the active port
 * queue.  Whenever an IPC data/datagram message is received, this queue
 * is checked to determine what routine to call with the packet data.
 */
vipc_port_info *vipc_create_port (
    ushort number,
    char *owner,
    boolean uses_netrpc,
    vipc_upcall *upcall)
{
    vipc_port_info *prev, *port, *newport;

    /*
     * Sanity
     */
    if (!owner || (!uses_netrpc && !upcall))
	return(NULL);
    if (number && vipc_find_port(number))
	return(NULL);

    /*
     * Create the new port structure.
     */
    newport = malloc_named(sizeof(vipc_port_info), "Vines IPC Port");
    if (!newport)
	return(NULL);
    if (number == 0)
	number = vipc_allocate_port_number();
    newport->number = number;
    newport->owner = owner;
    newport->uses_netrpc = uses_netrpc;
    newport->raw_ipc = upcall;
    if (vinesipc_debug)
	buginf("\nVIPC: Creating port %#x for %s", number, owner);

    /*
     * Insert into the list of active ports.
     */
    FOR_ALL_VIPC_PORTS_W_PREV(port, prev) {
	if (port->number > newport->number)
	    break;
    }
    insqueue(&vipc_portQ, newport, prev);
    return(newport);
}

/*
 * vipc_destroy_port
 *
 * An IPC port is no longer being used.  Remove it from the active queue
 * and free up the memory used.
 */
void vipc_destroy_port (vipc_port_info *port)
{
    if (port == NULL)
	return;

    if (vinesipc_debug)
	buginf("\nVIPC: Destroying port %#x for %s",
	       port->number, port->owner);
    unqueue(&vipc_portQ, port);
    free(port);
}

/*
 * vipc_destroy_port_number
 *
 * An IPC port is no longer being used.  Remove it from the active queue
 * and free up the memory used.
 */
void vipc_destroy_port_number (ushort port_number)
{
    vipc_port_info *port;

    port = vipc_find_port(port_number);
    vipc_destroy_port(port);
}

/*
 * vipc_connection_in_use
 *
 * Check to see if the given connection number is in use.  It's not
 * likely, but it would cause havoc if it happened.
 */
static boolean vipc_connection_in_use (ushort id)
{
    vinesipcsocket *socket;

    FOR_ALL_VIPC_SOCKETS(socket) {
	if (socket->local_id == id)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * vipc_connection_in_use
 *
 * Allocate a unique ID number for this side of an IPC connection.
 */
static ushort vipc_allocate_connection (void)
{
    boolean done;
    ushort connection, start;

    done = FALSE;
    start = vipc_connection_number;
    do {
	connection = vipc_connection_number++;
	if (vipc_connection_number == VIPC_CONN_LAST)
	    vipc_connection_number = VIPC_CONN_FIRST;
	if (!vipc_connection_in_use(connection))
	    return(connection);
    } while (vipc_connection_number != start);
    errmsg(&msgsym(CONSISTENCY, VINES), "all connection numbers in use.");
    return(-1);
}

/*
 * vipc_create_conn
 *
 * Create an IPC 'socket'.  This data structure contains all information
 * necessary to communicate with another VINES station.  The local id
 * number must be unique to this router.  No port numbers are assigned,
 * as an IPC connection carries data for more than one IPC port.
 */
vinesipcsocket *vipc_create_conn (
    ulong  r_net,
    ushort r_host,
    ushort r_id,
    ushort l_id,
    boolean outgoing)
{
    vinesipcsocket *socket;
    ushort metric;

    /*
     * Sockets can only exist between two individual hosts.
     */
    if (vines_is_bcast_addr(r_net, r_host)) {
	if (vinesipc_debug) {
	    buginf("\nVIPC: Attempt to create a connection to the broadcast %z.",
		   r_net, r_host);
	}
	return(NULL);
    }

    /*
     * Look for an existing socket that can be used.
     */
    FOR_ALL_VIPC_SOCKETS(socket) {
	if ((socket->remote_net == r_net) && (socket->remote_host == r_host)) {
	    if (!outgoing && (r_id != socket->remote_id))
		continue;
	    if (socket->state == SS_DEAD)
		continue;
	    if ((socket->state == SS_IDLE) && outgoing)
		continue;
	    return(socket);
	}
    }

    /*
     * No such luck.  Build a new one.
     */
    socket = malloc_named(sizeof(vinesipcsocket), "Vines IPC Socket");
    if (!socket)
	return(NULL);

    /* Connection information */
    socket->state            = (r_id ? SS_CONNECTED : SS_CONNECTING);
    socket->remote_net       = r_net;
    socket->remote_host      = r_host;
    socket->remote_id        = r_id;
    socket->local_id         = l_id ? l_id : vipc_allocate_connection();
    socket->last_send_seq    = 0;
    socket->next_send_ack    = 0;
    socket->last_send_ack    = 0;
    socket->last_rcvd_seq    = 0;

    /* Metric information */
    socket->need_metric = VIPC_NEED_NOTHING;
    if (r_net == vines_network) {
	socket->servermetric = 0;
	socket->lasthop = vines_get_neighbor_metric(r_net, r_host);
    } else {
	metric = vines_get_server_metric(r_net);
	if ((metric == 0) || (metric == VINES_INFINITY)) {
	    socket->need_metric |= VIPC_NEED_VALID_METRIC;
	    socket->servermetric = VIPC_DEFAULT_METRIC;
	} else {
	    socket->servermetric = metric;
	}
	socket->lasthop = 0;
	if (socket->remote_host != VINES_SERVER) {
	    socket->need_metric |= VIPC_NEED_LAST_HOP;
	}
    }
    socket->bias             = 0;
    socket->totalcost        = vmetric_to_ipc(socket->servermetric +
					      socket->lasthop + socket->bias);

    /* Retransmission and reassembly information */
    socket->reassembly = NULL;
    queue_init(&socket->retransQ, 0);
    
    /* Timer information */
    mgd_timer_init_leaf(&socket->next_ack_time, &vipc_timer, VT_IPC_ACK,
			socket, FALSE);
    mgd_timer_init_leaf(&socket->next_idle_time, &vipc_timer, VT_IPC_IDLE,
			socket, FALSE);
    mgd_timer_init_leaf(&socket->next_retrans_time, &vipc_timer, VT_IPC_RETRANS,
			socket, FALSE);
    vipc_set_idle_timer(socket);

    /* Outta here */
    enqueue(&vines_socketQ, socket);
    return(socket);
}

/*
 * vipc_get_conn
 *
 * Find an existing socket based upon the connection number.  Idle
 * sockets may not be used for transmission, but they may be received
 * upon.
 */
vinesipcsocket *vipc_get_conn (
    ushort l_id,
    ushort r_id,
    boolean outgoing)
{
    vinesipcsocket *socket;

    if (l_id == 0)
	return(NULL);

    FOR_ALL_VIPC_SOCKETS(socket) {
	if (socket->local_id == l_id) {
	    if (r_id && socket->remote_id && (r_id != socket->remote_id))
		continue;
	    if (socket->state == SS_DEAD)
		continue;
	    if ((socket->state == SS_IDLE) && outgoing)
		continue;
	    if (!socket->remote_id)
		socket->remote_id = r_id;
	    break;
	}
    }
    return(socket);
}

/*
 * vipc_get_conn_by_address
 *
 * Find an existing socket based upon the source address.  This should
 * only be used for received datagram messages that do not contain
 * connection numbers.  Idle sockets may not be used for transmission,
 * but they may be received upon.
 */
vinesipcsocket *vipc_get_conn_by_address (
    ulong r_net,
    ushort r_host)
{
    vinesipcsocket *socket;

    FOR_ALL_VIPC_SOCKETS(socket) {
	if ((socket->remote_net == r_net) && (socket->remote_host == r_host)) {
	    if ((socket->state == SS_DEAD) || (socket->state == SS_IDLE))
		continue;
	    break;
	}
    }
    return(socket);
}

/*
 * vipc_kill_conn
 *
 * The connection is done for.  Clear all queues, timers, etc.
 */
void vipc_kill_conn (vinesipcsocket *socket, char *reason, boolean erase)
{
    paktype *pak;

    if (vinesipc_debug) {
	buginf("\nVIPC: Connection to %z killed by %s.", socket->remote_net,
	       socket->remote_host, reason);
    }
    if (socket->reassembly)
	datagram_done(socket->reassembly);
    while ((pak = pak_dequeue(&socket->retransQ)) != NULL)
	datagram_done(pak);
    vipc_clear_retrans_timer(socket);
    vipc_clear_idle_timer(socket);
    vipc_clear_ack_timer(socket);
    vines_flush_proxy_queue(socket);
    socket->state = SS_DEAD;

    if (erase) {
	p_unqueue(&vines_socketQ, socket);
	free(socket);
    }
}

/*
 * vipc_user_clear_conn
 */
void vipc_user_clear_conn (parseinfo *csb)
{
    vinesipcsocket *socket, *prev;
    ushort conn_id;

    conn_id = GETOBJ(int, 1);
    socket = vines_socketQ.qhead;
    FOR_ALL_VIPC_SOCKETS_W_PREV(socket, prev) {
	if (socket->local_id != conn_id)
	    continue;
	vipc_kill_conn(socket, "user clear", TRUE);
	socket = prev;
    }
}

/*
 * vipc_network_error
 *
 * The network layer is reporting an error message to IPC.
 */
void vipc_network_error (vinesiptype *vip, vinesipctype *ipc)
{
    vinesipcsocket *socket, *prev;
    ushort conn_id;

    conn_id = ipc->local_id;
    socket = vines_socketQ.qhead;
    FOR_ALL_VIPC_SOCKETS_W_PREV(socket, prev) {
	if (socket->local_id != conn_id)
	    continue;
	vsvc_report_error(socket);
	vipc_kill_conn(socket, "network error", TRUE);
	socket = prev;
    }
}

/**********************************************************************
 *
 *		     INTERFACE TO VINES IP LAYER
 *
 **********************************************************************/

/*
 * vipc_send_socket
 *
 * This routine takes a packet and a socket structure as input.  It
 * extracts the VINES address , puts it into the packet, and then sends
 * the packet.
 */
static vinesidbtype *vipc_send_socket (
    vinesipcsocket *socket,
    paktype *pak,
    int flags,
    int ttl)
{
    vinesipctype *ipc;
    vinesidbtype *idb;

    if (vinesipc_debug) {
	ipc = (vinesipctype *)pak->transport_start;
	if (ipc->type > VIPC_LAST) {
	    buginf("\nVIPC: sending IPC unknown(%x) to %#z port %#x from port %#x",
		   ipc->type, socket->remote_net, socket->remote_host,
		   ipc->dstport, ipc->srcport);
	} else {
	    buginf("\nVIPC: sending IPC %s to %#z port %#x from port %#x",
		   vipc_types[ipc->type], socket->remote_net,
		   socket->remote_host, ipc->dstport, ipc->srcport);
	    buginf("\n      r_cid %d, l_cid %d, seq %d, ack %d, %s %d",
		   ipc->remote_id, ipc->local_id, ipc->sequence, ipc->ack,
		   (ipc->type == VIPC_ERROR) ? "error" : "length",
		   ipc->length);
	}
    }

    idb = vip_send_pak(pak, socket->remote_net, socket->remote_host,
		       VINES_IPC, flags, ttl);
    if (idb)
	idb->tx_ipc++;

    /*
     * Update connection information
     */
    socket->last_send_ack = socket->next_send_ack;
    vipc_clear_ack_timer(socket);
    vipc_set_idle_timer(socket);

    return(idb);
}

/*
 * vipc_send_nosocket_common
 *
 * This routine takes a packet and an address as input.  It puts it into
 * the packet, and then sends the packet.
 */
static inline vinesidbtype *vipc_send_nosocket_common (
    paktype *pak,
    ulong  dnet,
    ushort dhost,
    ulong  snet,
    ushort shost,
    int flags,
    int ttl)
{
    vinesidbtype *idb;
    vinesipctype *ipc;

    if (vinesipc_debug) {
	ipc = (vinesipctype *)pak->transport_start;
	if (ipc->type > VIPC_LAST) {
	    buginf("\nVIPC: sending IPC unknown(%x) to %#z port %#x from port %#x",
		   ipc->type, dnet, dhost, ipc->dstport, ipc->srcport);
	} else {
	    buginf("\nVIPC: sending IPC %s to %#z port %#x from local port %#x",
		   vipc_types[ipc->type], dnet, dhost, ipc->dstport,
		   ipc->srcport);
	}
    }

    idb = vip_send_pak_common(pak, dnet, dhost, snet, shost,
			      VINES_IPC, flags, ttl);
    if (idb)
	idb->tx_ipc++;
    return(idb);
}

static vinesidbtype *vipc_send_nosocket (
    paktype *pak,
    ulong  dnet,
    ushort dhost,
    int flags,
    int ttl)
{
    return(vipc_send_nosocket_common(pak, dnet, dhost, vines_network,
				     vines_host, flags, ttl));
}

static vinesidbtype *vipc_send_nosocket_alt_src (
    paktype *pak,
    ulong  dnet,
    ushort dhost,
    ulong  snet,
    ushort shost,
    int flags,
    int ttl)
{
    return(vipc_send_nosocket_common(pak, dnet, dhost, snet, shost,
				     flags, ttl));
}

/*
 * vipc_send_broadcast
 *
 * This routine takes a packet and an interface or two as input.  It puts
 * the broadcast address into the packet, and then sends it out the
 * requested interfaces, or out all interfaces other thatn the one that
 * should be excluded.
 */
static void vipc_send_broadcast (
    paktype *pak,
    vinesidbtype *use,
    vinesidbtype *exclude,
    int flags,
    int ttl)
{
    vinesipctype *ipc;

    if (vinesipc_debug) {
	ipc = (vinesipctype *)pak->transport_start;
	if (ipc->type > VIPC_LAST) {
	    buginf("\nVIPC: sending IPC unknown(%x) to %#z port %#x from port %#x",
		   ipc->type, VINES_ALLNETS, VINES_ALLHOSTS, ipc->dstport,
		   ipc->srcport);
	} else {
	    buginf("\nVIPC: sending IPC %s to %#z port %#x from port %#x",
		   vipc_types[ipc->type], VINES_ALLNETS, VINES_ALLHOSTS,
		   ipc->dstport, ipc->srcport);
	}
    }

    vip_send_broadcast(pak, use, exclude, VINES_IPC, flags, ttl);
}

/**********************************************************************
 *
 *		VINES IPC CONNECTION ORIENTED MESSAGES
 *
 **********************************************************************/

/**************************************************
 *
 *		    ACK PACKETS
 *
 **************************************************/

/*
 * vipc_purge_acked
 *
 * Scan the retransmit queue for this socket looking for packets that
 * have been acknowledged.  Free any packets that are discovered.  The
 * last packet freed is marked with an error code if this routine was
 * called from vipc_rcv_error().
 */
static void vipc_purge_acked (vinesipcsocket *socket, ushort ack, ushort error)
{
    paktype *pak;
    vinesipctype *ipc;

    if (QUEUEEMPTY(&socket->retransQ))
	return;
    while ((pak = pak_dequeue(&socket->retransQ)) != NULL) {
	ipc = (vinesipctype *)pak->transport_start;
	if (ipc->sequence > ack) {
	    pak_requeue(&socket->retransQ, pak);
	    return;
	}
	pak->retransmit_count = (ipc->sequence == ack) ? error : 0;
	TIMER_STOP(pak->retransmit_time);
	datagram_done(pak);
    }
    if (QUEUEEMPTY(&socket->retransQ))
	vipc_clear_retrans_timer(socket);    
}

/*
 * vipc_send_ack
 *
 * Send an ACK packet to the remote host.
 *
 * VPD p4-18.  The entity generates an acknowledgment packet if both of
 * the following are met:
 *
 *     o  The acknowledgment timer expires.
 *
 *     o The last acknowledgment number sent variable is not equal to the
 *       next acknowledgment number to send variable.
 */
static vinesidbtype *vipc_send_ack (vinesipcsocket *socket)
{
    paktype *pak;
    vinesipctype *ipc;
    
    if (socket->next_send_ack == socket->last_send_ack)
	return(NULL);

    pak = vipc_getbuffer(0);
    if (!pak)
	return(NULL);

    ipc = (vinesipctype *)pak->transport_start;
    ipc->srcport   = 0;
    ipc->dstport   = 0;
    ipc->type      = VIPC_ACK;
    ipc->control   = 0;
    ipc->local_id  = socket->local_id;
    ipc->remote_id = socket->remote_id;
    ipc->sequence  = socket->last_send_seq;
    ipc->ack       = socket->next_send_ack;
    ipc->length    = 0;

    return(vipc_send_socket(socket, pak, 0, VINES_HOPS));
}

/*
 * vipc_rcv_ack
 *
 * Release any packets on the retransmit queue.
 */
static void vipc_rcv_ack (vinesipcsocket *socket, paktype *pak)
{
    vinesipctype *ipc;

    /*
     * Check received sequence number
     */
    ipc = (vinesipctype *)pak->transport_start;
    if (socket->last_rcvd_seq) {
	if (ipc->sequence != socket->last_rcvd_seq) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Discarding out of sequence packet");
	    return;
	}
    }

    /*
     * Update connection information
     */
    socket->last_rcvd_seq = ipc->sequence;
    socket->next_send_ack = ipc->sequence;
    vipc_purge_acked(socket, ipc->ack, 0);
    if (ipc->control & VIPC_CTL_ACK)
	vipc_set_ack_timer(socket, 0);
    vipc_set_idle_timer(socket);
}

/**************************************************
 *
 *		    ERROR PACKETS
 *
 **************************************************/

/*
 * vipc_send_error
 *
 * Handle the sending of a VINES IPC level error packet.  For a full
 * listing of VINES networking error codes see the file vines.errors.
 *
 * VPD p4-17.  An error packet requires a sequence number and an
 * acknowledgment.  The IPC entity sets the acknowledgment number field
 * in the header of an error packet to the sequence number of the packet
 * in error.
 */
static vinesidbtype *vipc_send_error (vinesipcsocket *socket, int code)
{
    paktype *pak;
    vinesipctype *ipc;

    pak = vipc_getbuffer(0);
    if (!pak)
	return(NULL);

    ipc = (vinesipctype *)pak->transport_start;
    ipc->srcport   = 0;
    ipc->dstport   = 0;
    ipc->type      = VIPC_ERROR;
    ipc->control   = 0;
    ipc->local_id  = socket->local_id;
    ipc->remote_id = socket->remote_id;
    ipc->sequence  = ++socket->last_send_seq;
    ipc->ack       = socket->next_send_ack;
    ipc->length    = code;
    
    return(vipc_send_socket(socket, pak, 0, VINES_HOPS));
}

/*
 * vipc_rcv_error
 *
 * Process an incoming error IPC packet.  There is one case where these
 * frames are actually expected.  This is when a ping or a traceroute is
 * in progress.
 *
 * VPD p4-17.  When the entity received an error packet in the correct
 * order, the entity releases all data packets on the acknowledgment
 * queue with sequence numbers less than or equal to the value of the
 * acknowledgment number in the error packet.  The entity returns and
 * user send data requests associated with the released data packets.
 *
 * The IPC entity returns the user send request associated with the
 * packet that has a sequence number equal to the value of the
 * acknowledgment number in the error packet.  The send request in
 * returned with an error.  The error code returned is equal to the value
 * of the error field of the error packet header.
 */
static void vipc_rcv_error (vinesipcsocket *socket, paktype *pak)
{
    vinesipctype *ipc;

    /*
     * Check received sequence number
     */
    ipc = (vinesipctype *)pak->transport_start;
    if (socket->last_rcvd_seq) {
	if (ipc->sequence != (socket->last_rcvd_seq + 1)) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Discarding out of sequence packet");
	    vipc_send_probe(socket);
	    return;
	}
    }

    /*
     * Update connection information
     */
    socket->last_rcvd_seq = ipc->sequence;
    socket->next_send_ack = ipc->sequence;
    vipc_purge_acked(socket, ipc->ack, ipc->length /* error code */);
    vsvc_report_error(socket);
    vipc_set_ack_timer(socket, (ipc->control & VIPC_CTL_ACK) ? 0 : ONETICK);
    vipc_set_idle_timer(socket);
    if (socket->last_remote_port == VIPC_PORT_ECHO)
	vines_ipc_echo_sink(socket, ipc->data, ipc->length);
}

/**************************************************
 *
 *		    PROBE PACKETS
 *
 **************************************************/

/*
 * vipc_send_probe
 *
 * Something was out of sequence.  Notify the sender of the last packet
 * received in sequence.
 *
 * VPD p4-17.  The sending IPC entity issues a probe packet to inform the
 * receiving ipc entity that a data or error packet was missed.  The
 * sending entity sets the sequence number field in the probe packet
 * header to the last sequence number received.
 *
 * VPD p6-27.  This routine should only be called when a data or error
 * packet is received out of sequence.  Other out of sequence packets
 * should not elicit a probe response.
 */
vinesidbtype *vipc_send_probe (vinesipcsocket *socket)
{
    paktype *pak;
    vinesipctype *ipc;

    pak = vipc_getbuffer(0);
    if (!pak)
	return(NULL);

    ipc = (vinesipctype *)pak->transport_start;
    ipc->srcport   = 0;
    ipc->dstport   = 0;
    ipc->type      = VIPC_PROBE;
    ipc->control   = 0;
    ipc->local_id  = socket->local_id;
    ipc->remote_id = socket->remote_id;
    ipc->sequence  = socket->last_send_seq;
    ipc->ack       = socket->next_send_ack;
    ipc->length    = 0;
    
    return(vipc_send_socket(socket, pak, 0, VINES_HOPS));
}

/*
 * vipc_rcv_probe
 *
 * The router is being notified that something was out of sequence.
 *
 * VPD p4-17.  Then the receiving entity receives a probe packet, the
 * receiving entity can update the acknowledgment queue if the sequence
 * number in the probe packet header matches the last sequence number
 * received.  The receiving entity retransmits all packets in the
 * acknowledgment queue with sequence numbers greater than the
 * acknowledgment field of the received probe packet.
 *
 */
static void vipc_rcv_probe (vinesipcsocket *socket, paktype *pak)
{
    vinesipctype *ipc;

    /*
     * Check received sequence number
     */
    ipc = (vinesipctype *)pak->transport_start;
    if (socket->last_rcvd_seq) {
	if (ipc->sequence != socket->last_rcvd_seq) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Discarding out of sequence packet");
	    return;
	}
    }

    /*
     * Update connection information.  This sets the connection so that a
     * retransmission will be attempted immediately.
     */
    socket->last_rcvd_seq = ipc->sequence;
    socket->next_send_ack = ipc->sequence;
    vipc_purge_acked(socket, ipc->ack, 0);
    vipc_set_ack_timer(socket, (ipc->control & VIPC_CTL_ACK) ? 0 : ONETICK);
    vipc_set_idle_timer(socket);
}

/**************************************************
 *
 *		    DISC PACKETS
 *
 **************************************************/

/*
 * vipc_send_disc_nosocket
 *
 * Tell the other guy to get lost.  This is called when another entity
 * tries to communicate with a nonexistent connection on this router.
 */
static vinesidbtype *vipc_send_disc_nosocket (
    ulong net,
    ushort host,
    vinesipctype *ipc)
{
    paktype *pak;
    vinesipctype *newipc;

    pak = vipc_getbuffer(0);
    if (!pak)
	return(NULL);

    newipc = (vinesipctype *)pak->transport_start;
    newipc->srcport   = ipc->dstport;
    newipc->dstport   = ipc->srcport;
    newipc->type      = VIPC_DISC;
    newipc->control   = 0;
    newipc->local_id  = ipc->remote_id;
    newipc->remote_id = ipc->local_id;
    newipc->sequence  = newipc->ack + 1;
    newipc->ack       = newipc->sequence;
    newipc->length    = 0;
    
    return(vipc_send_nosocket(pak, net, host, 0, VINES_HOPS));
}

/*
 * vipc_send_disc
 *
 * Kill the connection.  Shut down all timers, free saved packets, etc.
 */
static vinesidbtype *vipc_send_disc (vinesipcsocket *socket)
{
    paktype *pak;
    vinesipctype *ipc;

    pak = vipc_getbuffer(0);
    if (!pak)
	return(NULL);

    ipc = (vinesipctype *)pak->transport_start;
    ipc->srcport   = 0;
    ipc->dstport   = 0;
    ipc->type      = VIPC_DISC;
    ipc->control   = 0;
    ipc->local_id  = socket->local_id;
    ipc->remote_id = socket->remote_id;
    ipc->sequence  = socket->last_send_seq;
    ipc->ack       = socket->next_send_ack;
    ipc->length    = 0;
    
    socket->state = SS_DEAD;
    return(vipc_send_socket(socket, pak, 0, VINES_HOPS));
}

/*
 * vipc_rcv_disc
 *
 * The other side killed the connection.  Shut down all timers, free
 * saved packets, etc.
 */
static void vipc_rcv_disc (vinesipcsocket *socket, paktype *pak)
{
    vinesipctype *ipc;

    ipc = (vinesipctype *)pak->transport_start;
    vipc_kill_conn(socket, "disconnect", FALSE);
    vipc_set_idle_timer(socket);
    if (socket->last_remote_port == VIPC_PORT_ECHO)
	vines_ipc_echo_sink(socket, ipc->data, ipc->length);
}


/**************************************************
 *
 *		    DATA PACKETS
 *
 **************************************************/

/*
 * vipc_send_data
 *
 * Send a sequenced, reliable message.  An IPC header is attached to the
 * incoming information, and the message is passed on to the VINES IP
 * layer for transmission.  The packet is also stuck onto a
 * retransmission queue, and a retransmission timer started.
 */
vinesidbtype *vipc_send_data (
    vinesipcsocket *socket,
    paktype *pak,
    ushort dport,
    ushort sport,
    int ttl)
{
    vinesidbtype *idb;
    vinesipctype *ipc;
    ulong flags;
    int length = pak->datagramsize;

    if (!socket) {
	if (vinesipc_debug)
	    buginf("\nVIPC: send data passed invalid socket.");
	return(NULL);
    }

    /*
     * Is this a pseudo-socket created by the receipt of a datagram?  If
     * so, then a real socket must be created before this message can be
     * sent.  The alternative would have been to create a real socket for
     * every datagram received, but that would chew through the socket
     * number space extremely quickly.
     */
    if (socket->local_id == 0) {
	socket = vipc_create_conn(socket->remote_net, socket->remote_host,
				  0, 0, TRUE);
	if (socket == NULL) {
	    if (vinesipc_debug)
		buginf("\nVIPC: cannot create socket to send.");
	    datagram_done(pak);
	    return(NULL);
	}
    }

    pak->transport_start = pak->session_start - sizeof(vinesipctype);
    pak->datagramsize += sizeof(vinesipctype);

    ipc = (vinesipctype *)pak->transport_start;
    ipc->srcport   = sport ? sport : socket->last_local_port;
    ipc->dstport   = dport ? dport : socket->last_remote_port;
    ipc->type      = VIPC_DATA;
    ipc->control   = VIPC_CTL_BOM | VIPC_CTL_EOM;
    ipc->local_id  = socket->local_id;
    ipc->remote_id = socket->remote_id;
    ipc->sequence  = ++socket->last_send_seq;
    ipc->ack       = socket->next_send_ack;
    ipc->length    = length;

    socket->last_remote_port = ipc->dstport;
    socket->last_local_port = ipc->srcport;

    /*
     * Send it.
     */
    pak_lock(pak);
    flags = VINES_ERROR;
    if (socket->need_metric & VIPC_NEED_LAST_HOP)
	flags |= VINES_METRIC;
    idb = vipc_send_socket(socket, pak, flags, ttl);
    if (!idb) {
	if (vinesipc_debug)
	    buginf("\nVIPC: could not send data message.");
	vipc_network_error((vinesiptype *)pak->network_start, ipc);
	return(NULL);
    }

    /*
     * Add to retransmit queue.
     */
    if (ipc->dstport == VIPC_PORT_ECHO) {
	datagram_done(pak);
	return(idb);
    }
    pak->retransmit_count = VIPC_RETRY_COUNT;
    TIMER_START(pak->retransmit_time, socket->totalcost);
    vipc_set_retrans_timer(socket, pak->retransmit_time);
    if (!pak_enqueue(&socket->retransQ, pak)) {
	if (vinesipc_debug)
	    buginf("\nVIPC: send data cannot add to retransQ.");
	datagram_done(pak);
	return(NULL);
    }
    return(idb);
}

/*
 * vipc_process_data
 *
 * Call the appropriate higher level routine to process the data from a
 * received IPC Data message.
 */
static void vipc_process_data (vinesipcsocket *socket, paktype *pak)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    vipc_port_info *port_info;
    ushort port;
    vinesidbtype *vidb_in;
    char disposition[] = "\nVIPC: %s data from %#z for unknown port (%#x).";

    vip = (vinesiptype *)pak->network_start;
    port = socket->last_local_port;
    vidb_in = vines_getidb(pak->if_input);

    /* Pass up StreetTalk packets if not enabled */
    if (port == VIPC_PORT_ST && !streettalk_enabled) {
	vidb_in->rx_ipc_unknown++;
	vipc_send_error(socket, VINES_ERR_NO_SOCKET);
	return;
    }
    
    port_info = vipc_find_port(port);
    if (!port_info) {
	if (vines_unknown_debug)
	    buginf_vinespak(pak);
	if (vipc_proxy_packet(socket, pak)) {
	    if (vinesipc_debug || vines_unknown_debug)
		buginf(disposition, "Proxied", GET2WD(vip->ssrcnet),
		       vip->srchost, port);
	    return;
	}
	if (vinesipc_debug || vines_unknown_debug)
	    buginf(disposition, "Discarded", GET2WD(vip->ssrcnet),
		   vip->srchost, port);
	if (socket->last_was_broadcast == FALSE) {
	    vidb_in->rx_ipc_unknown++;
	    vipc_send_error(socket, VINES_ERR_NO_SOCKET);
	}
	return;
    }

    /*
     * Dispatch to the appropriate handler routine.
     */
    ipc = (vinesipctype *)pak->transport_start;
    if (port_info->uses_netrpc) {
	vines_netrpc_receive(socket, ipc->data, ipc->length);
	return;
    }
    if (port_info->raw_ipc) {
	(*port_info->raw_ipc)(socket, ipc->data, ipc->length);
	return;
    }
    errmsg(&msgsym(IPCNOROUTINE, VINES), port, "data");
}

/*
 * vipc_rcv_data
 *
 * Receive a sequenced, reliable message.  Process all of the fields in
 * the IPC header, and then pass the data off to another routine to
 * handle the demultiplexing and processing.
 */
static void vipc_rcv_data (vinesipcsocket *socket, paktype *pak)
{
    paktype *rpak;
    vinesipctype *ipc, *ripc;

    /*
     * Get and set pointers to the various VINES headers.
     */
    ipc = (vinesipctype *)pak->transport_start;
    pak->session_start = ipc->data;

    /*
     * Check received sequence number.  Make sure to mask the test to
     * sixteen bits so that wrap around from 65535 to 0 works properly.
     */
    if (ipc->sequence != ((socket->last_rcvd_seq + 1) & 0xFFFF)) {
	if (vinesipc_debug)
	    buginf("\nVIPC: Discarding out of sequence packet");
	vipc_send_probe(socket);
	return;
    }

    /*
     * Partially process the control information.  This makes it possible
     * to easily ignore the packet if there are no buffers available for
     * reassembly.
     */
    if (ipc->control & VIPC_CTL_ABORT) {
	if (socket->reassembly)
	    datagram_done(socket->reassembly);
	socket->reassembly = NULL;
    }
    switch (ipc->control & VIPC_CTL_BOMEOM) {
      case VIPC_CTL_BOMEOM:
	break;

      case VIPC_CTL_BOM:
	if (socket->reassembly)
	    datagram_done(socket->reassembly);
	socket->reassembly = vipc_getbuffer(VIPC_MAXDATA);
	if (!socket->reassembly) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Cannot allocate buffer for reassembly");
	    return;
	}
	break;
	
      case VIPC_CTL_EOM:
      case VIPC_CTL_NONE:
	if (!socket->reassembly) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Middle fragment but no packet in reassembly");
	    return;
	}
    }

    /*
     * Update connection information
     */
    socket->last_rcvd_seq = ipc->sequence;
    socket->next_send_ack = ipc->sequence;
    vipc_purge_acked(socket, ipc->ack, 0);
    vipc_set_ack_timer(socket, (ipc->control & VIPC_CTL_ACK) ? 0 : ONETICK);
    vipc_set_idle_timer(socket);
    
    /*
     * Process the data
     */
    switch (ipc->control & VIPC_CTL_BOMEOM) {
      case VIPC_CTL_BOMEOM:
	vipc_process_data(socket, pak);
	return;

      case VIPC_CTL_BOM:
	rpak = socket->reassembly;
	rpak->datagramstart = rpak->data_area +
	    (pak->datagramstart - pak->data_area);
	rpak->datagramsize = pak->datagramsize;
	bcopy(pak->datagramstart, rpak->datagramstart, pak->datagramsize);
	break;

      case VIPC_CTL_EOM:
      case VIPC_CTL_NONE:
	rpak = socket->reassembly;
	ripc = (vinesipctype *)rpak->transport_start;

	/*
	 * safeguard against incorrect IPC packet length.
	 * length validation is required to prevent router crash.
	 */
	if ((rpak->datagramsize + ipc->length) > VIPC_MAXDATA) {
	   if (vinesipc_debug)
		buginf("\nVIPC: Packet too long. Disc session");
	   vipc_send_disc(socket);
	   socket->reassembly = NULL;
	   datagram_done(rpak);
	   return;
	}

	bcopy(ipc->data, ripc->data + ripc->length, ipc->length);
	rpak->datagramsize += ipc->length;
	ripc->length += ipc->length;

	if (ipc->control & VIPC_CTL_EOM) {
	    socket->reassembly = NULL;
	    vipc_process_data(socket, rpak);
	    datagram_done(rpak);
	    return;
	}
    }
}

/**************************************************
 *
 *		  DATAGRAM PACKETS
 *
 **************************************************/

/*
 * vipc_send_datagram_common
 *
 * Send an unsequenced, unacknowledged message.  This routine is normally
 * used to send broadcast messages.
 */
vinesidbtype *vipc_send_datagram_common (
    paktype *pak,
    ulong  dnet,
    ushort dhost,
    ulong  snet,
    ushort shost,
    ushort dport,
    ushort sport,
    int ttl)
{
    vinesipc2type *ipc;

    if (!sport) {
	if (vinesipc_debug)
	    buginf("\nVIPC: Send datagram to port %#x called with no source port.", dport);
	return(NULL);
    }

    pak->transport_start = pak->session_start - sizeof(vinesipc2type);
    pak->datagramsize += sizeof(vinesipc2type);

    ipc = (vinesipc2type *)pak->transport_start;
    ipc->srcport   = sport;
    ipc->dstport   = dport;
    ipc->type      = VIPC_DATAGRAM;
    ipc->control   = VIPC_CTL_BOMEOM;

    if (vines_is_bcast_addr(dnet, dhost)) {
	vipc_send_broadcast(pak, NULL, NULL, 0, ttl);
	return(NULL);
    }
    return(vipc_send_nosocket_alt_src(pak, dnet, dhost, snet, shost, 0, ttl));
}

/*
 * vipc_send_broadcast_datagram
 *
 * Send an unsequenced, unacknowledged message.  This routine is normally
 * used to send broadcast messages.
 */
void vipc_send_broadcast_datagram (
    paktype *pak,
    vinesidbtype *use,
    vinesidbtype *exclude,
    ushort dport,
    ushort sport,
    int flags,
    int ttl)
{
    vinesipc2type *ipc;

    if (!sport) {
	if (vinesipc_debug)
	    buginf("\nVIPC: Send datagram to port %#x called with no source port.", dport);
	return;
    }

    pak->transport_start = pak->session_start - sizeof(vinesipc2type);
    pak->datagramsize += sizeof(vinesipc2type);

    ipc = (vinesipc2type *)pak->transport_start;
    ipc->srcport   = sport;
    ipc->dstport   = dport;
    ipc->type      = VIPC_DATAGRAM;
    ipc->control   = VIPC_CTL_BOMEOM;

    vipc_send_broadcast(pak, use, exclude, flags, ttl);
}

/*
 * vipc_rcv_datagram
 *
 * Receive an unsequenced, unacknowledged message.  This routine is normally
 * called when a broadcast messages is received.
 */
static boolean vipc_rcv_datagram (
    paktype *pak,
    int length,
    boolean broadcast)
{
    vinesiptype *vip;
    vinesipc2type *ipc;
    vinesipcsocket *socket, pseudo_socket;
    vipc_port_info *port_info;
    ushort port;
    vinesidbtype *vidb_in;
    
    /*
     * Get and set pointers to the various VINES headers.
     */
    vip = (vinesiptype *)pak->network_start;
    ipc = (vinesipc2type *)pak->transport_start;
    pak->session_start = ipc->data;
    length -= sizeof(vinesipc2type);
    vidb_in = vines_getidb(pak->if_input);
    
    /*
     * Find an existing socket if one exists.  If not, create a
     * pseudo-socket instead of a real one.  If this packet does elicit a
     * response, a real socket will be created at that time.  This
     * prevents chewing up socket numbers at ablingingly fast rate, one
     * per IPC broadcast message.
     */
    socket = vipc_get_conn_by_address(GETLONG(vip->ssrcnet), vip->srchost);
    if (socket == NULL) {
	socket = &pseudo_socket;
	memset(socket, 0, sizeof(vinesipcsocket));
	socket->remote_net = GET2WD(vip->ssrcnet);
	socket->remote_host = vip->srchost;
	socket->reassembly = NULL;
	queue_init(&socket->retransQ, 0);
	/* Set up timers but don't link them to the vipc_timer */
	mgd_timer_init_leaf(&socket->next_ack_time, NULL, VT_IPC_ACK,
			    socket, FALSE);
	mgd_timer_init_leaf(&socket->next_idle_time, NULL, VT_IPC_IDLE,
			    socket, FALSE);
	mgd_timer_init_leaf(&socket->next_retrans_time, NULL, VT_IPC_RETRANS,
			    socket, FALSE);
    }
    socket->last_local_port = ipc->dstport;
    socket->last_remote_port = ipc->srcport;
    socket->last_idb_in = vidb_in;
    socket->last_hop_count = vip->tc & VINES_HOPS;
    socket->last_was_broadcast = broadcast;

    port = ipc->dstport;

    /* Pass up StreetTalk packets if not enabled */
    if (port == VIPC_PORT_ST && !streettalk_enabled) {
	if (broadcast == FALSE) {
	    vidb_in->rx_ipc_unknown++;
	    vipc_send_error(socket, VINES_ERR_NO_SOCKET);
	}
	return (FALSE);
    }
    
    port_info = vipc_find_port(port);
    if (!port_info) {
	if (vines_unknown_debug)
	    buginf_vinespak(pak);
	if (vinesipc_debug || vines_unknown_debug)
	    buginf("\nVIPC: Discarding datagram from %#z for unknown port (%#x).",
		   GET2WD(vip->ssrcnet), vip->srchost, port);
	if (broadcast == FALSE) {
	    vidb_in->rx_ipc_unknown++;
	    vipc_send_error(socket, VINES_ERR_NO_SOCKET);
	}
	return(FALSE);
    }

    if (vinesipc_debug)
	buginf(" (hop count %d)", socket->last_hop_count);
    if (port_info->uses_netrpc) {
	return(vines_netrpc_receive(socket, ipc->data, length));
    }
    if (port_info->raw_ipc) {
	(*port_info->raw_ipc)(socket, ipc->data, length);
	return(TRUE);
    }
    errmsg(&msgsym(IPCNOROUTINE, VINES), port, "datagram");
    return(TRUE);
} 

/**********************************************************************
 *
 *		    INTERFACE FROM VINES IP LAYER
 *
 **********************************************************************/

/*
 * vipc_demux_message
 *
 * Demultiplex based upon the IPC message type.  Handles debugging, and
 * calling the appropriate handler routines.
 */
static boolean vipc_demux_message (
    paktype *pak,
    boolean broadcast)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    vinesipc2type *shortipc;
    vinesipcsocket *socket;
    int length;
    char *word;

    vip = (vinesiptype *)pak->network_start;
    ipc = (vinesipctype *)pak->transport_start;
    if (ipc->type == VIPC_DATAGRAM) {
	shortipc = (vinesipc2type *)pak->transport_start;
	if (vinesipc_debug)
	    buginf("\nVIPC: Rcvd %s from %#z port %#x to port %#x",
		   vipc_types[shortipc->type], GET2WD(vip->ssrcnet),
		   vip->srchost, shortipc->srcport, shortipc->dstport);
	length = vip->len - sizeof(vinesiptype);
	return(vipc_rcv_datagram(pak, length, broadcast));
    }

    if (vinesipc_debug) {
	if (ipc->type > VIPC_LAST) {
	    buginf("\nVIPC: rcvd unknown(%x) from %#z port %#x to port %#x",
		   ipc->type, GET2WD(vip->ssrcnet), vip->srchost,
		   ipc->srcport, ipc->dstport);
	} else {
	    buginf("\nVIPC: rcvd %s from %#z port %#x to port %#x",
		   vipc_types[ipc->type], GET2WD(vip->ssrcnet),
		   vip->srchost, ipc->srcport, ipc->dstport);
	    switch (ipc->type) {
		case VIPC_ERROR:	word = "error";		break;
		case VIPC_ACK:		word = "filler";	break;
		default:		word = "length";	break;
	    }
	    buginf("\n      r_cid %d, l_cid %d, seq %d, ack %d, %s %d",
		   ipc->local_id, ipc->remote_id, ipc->sequence, ipc->ack,
		   word, ipc->length);
	}
    }
    
    if (ipc->remote_id) {
	socket = vipc_get_conn(ipc->remote_id, ipc->local_id, FALSE);
    } else {
	socket = vipc_create_conn(GET2WD(vip->ssrcnet), vip->srchost,
				  ipc->local_id, ipc->remote_id, FALSE);
    }
    if (!socket) {
	if (ipc->type != VIPC_DISC) {
	    if (vinesipc_debug)
		buginf("\nVIPC: Cannot find or create socket. Sending disconnect.");
	    vipc_send_disc_nosocket(GET2WD(vip->ssrcnet), vip->srchost, ipc);
	}
	return(TRUE);
    }

    if (ipc->srcport)
	socket->last_remote_port = ipc->srcport;
    if (ipc->dstport)
	socket->last_local_port = ipc->dstport;
    socket->last_idb_in = vines_getidb(pak->if_input);
    socket->last_hop_count = vip->tc & VINES_HOPS;
    socket->last_was_broadcast = broadcast;
    switch (ipc->type) {
      case VIPC_DATA:
	vipc_rcv_data(socket, pak);
	socket->state = SS_CONNECTED;
	break;
	
      case VIPC_ERROR:
	vipc_rcv_error(socket, pak);
	socket->state = SS_CONNECTED;
	break;
	
      case VIPC_DISC:
	vipc_rcv_disc(socket, pak);
	socket->state = SS_DEAD;
	break;
	
      case VIPC_PROBE:
	vipc_rcv_probe(socket, pak);
	socket->state = SS_CONNECTED;
	break;
	
      case VIPC_ACK:
	vipc_rcv_ack(socket, pak);
	socket->state = SS_CONNECTED;
	break;
	
      default:
	break;
    }
    return(TRUE);
}

/*
 * vines_ipc_receive
 *
 * Upcall from the VINES IP layer.  Hand off connectionless messages
 * almost immediately.  For connection oriented messages, find or create
 * the connection, and then call the appropriate routine for the type of
 * message received.
 */
void vines_ipc_receive (paktype *pak, vinesidbtype *idb)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    int recordedtype;
    boolean broadcast, processed, helper = FALSE, bcast_helper = FALSE;

    /*
     * Count it before anything else.
     */
    vip = (vinesiptype *)pak->network_start;
    ipc = (vinesipctype *)pak->transport_start;
    recordedtype = (ipc->dstport < VIPC_PORT_MAXCOUNTED) ? ipc->dstport : 0;
    broadcast = ((GET2WD(vip->ddstnet) != vines_network) ||
		 (vip->dsthost != vines_host));
    if (broadcast)
	vines_broadcast[recordedtype]++;
    else
	vines_unicast[recordedtype]++;

    /*
     * Process it.  Pass up streettalk packets unless the
     * streettalk support has been explicitly enabled.
     */
    processed = vipc_demux_message(pak, broadcast);

    /*
     * Was it something that needs to be handled in a special manner?
     * All special packets are zero hop broadcast packets that contain an
     * IPC Datagram.
     *
     * StreetTalk broadcasts from servers need to be forwarded to
     * maintain connectivity.  Broadcasts received from serverless
     * networks also need to be forwarded.  The sending routines will
     * take care of the reverse path forwarding tests.
     */
    if (processed || !broadcast || ((vip->tc & VINES_HOPS) != 0) ||
	(ipc->type != VIPC_DATAGRAM))
	return;
    if (!streettalk_enabled && (ipc->dstport == VIPC_PORT_ST) && (vip->srchost == VINES_SERVER)) {
	if (!vines_is_st_welcome(pak)) {
	    if (vinesipc_debug || vst_debug)
		buginf("\nVIPC: flooding streettalk broadcast from %#z",
		       GET2WD(vip->ssrcnet), vip->srchost);
	    vines_flood_bcast(pak, FALSE, FALSE, NULL);
	} else if (vinesipc_debug || vst_debug) {
	    buginf("\nVIPC: filtering streettalk broadcast from %#z",
		   GET2WD(vip->ssrcnet), vip->srchost);
	}
	return;
    }
    switch (idb->serverless) {
      case VSVLESS_NEVER:        helper = FALSE; bcast_helper = FALSE; break;
      case VSVLESS_ALWAYS:       helper = TRUE;  bcast_helper = FALSE; break;
      case VSVLESS_ALWAYS_BCAST: helper = TRUE;  bcast_helper = TRUE;  break;
      case VSVLESS_DYNAMIC:
	helper = !vines_servers_present(idb);
	bcast_helper = FALSE;
	break;
    }
    if (helper) {
	vines_helpered[recordedtype]++;
	if (bcast_helper) {
	    if (vines_proxy_debug) {
		if (ipc->type > VIPC_LAST)
		    buginf("\nVIPC: flooding IPC unknown(%x)");
		else
		    buginf("\nVIPC: flooding IPC %s", vipc_types[ipc->type]);
	    }
	    vines_flood_bcast(pak, FALSE, FALSE, NULL);
	} else {
	    vines_forward_nearest(pak);
	}
	return;
    }
}

/*
 * vipc_set_metric
 *
 * The router has received an IPC metric notification.  This should only
 * occur if the router requested metric information for a newly created
 * socket.  Look for the socket in question, and record the metric
 * information as part of the path cost.
 */
void vipc_set_metric (ulong net, ushort host, ushort metric)
{
    vinesipcsocket *socket;
    ushort svr_metric;

    if (metric == VINES_INFINITY)
	return;
    FOR_ALL_VIPC_SOCKETS(socket) {
	if ((net != socket->remote_net) || (host != socket->remote_host))
	    continue;
	if (socket->state == SS_DEAD)
	    continue;
	if (socket->need_metric & VIPC_NEED_VALID_METRIC) {
	    svr_metric = vines_get_server_metric(socket->remote_net);
	    if ((svr_metric != 0) && (svr_metric != VINES_INFINITY)) {
		socket->need_metric &= ~(VIPC_NEED_VALID_METRIC);
		socket->servermetric = svr_metric;
	    }
	}
	if (!socket->lasthop) {
	    socket->need_metric &= ~(VIPC_NEED_LAST_HOP);
	    socket->lasthop = metric;
	}
	socket->totalcost = vmetric_to_ipc(socket->servermetric +
					   socket->lasthop + socket->bias);
    }
}

/**********************************************************************
 *
 *			 TIMER DRIVEN EVENTS
 *
 **********************************************************************/

/*
 * vipc_resend_packets
 *
 * Look at the retransmit queue for this socket, and resend any packets
 * whose time has come.  If the retransmit count on a frame ever hits
 * zero, purge the retransmit queue and kill the socket.
 */
static void vipc_resend_packets (vinesipcsocket *socket)
{
    paktype *pak, *newpak;
    vinesipctype *ipc;
    elementtype *element;

    if (QUEUEEMPTY(&socket->retransQ)) {
	vipc_clear_retrans_timer(socket);    
	return;
    }

    /*
     * Walk the retransmit queue, sending packets.  The packets need to
     * remain on the retransmit queue unless the connection is actually
     * being killed.
     */
    for (element = socket->retransQ.qhead; element; element = element->next) {
	pak = element->pakptr;
	/*
	 * Time?
	 */
	if (SLEEPING(pak->retransmit_time)) {
	    vipc_set_retrans_timer(socket, pak->retransmit_time);
	    return;
	}

	/*
	 * Too many attempts?
	 */
	ipc = (vinesipctype *)pak->transport_start;
	if (--pak->retransmit_count == 0) {
	    while ((pak = pak_dequeue(&socket->retransQ)) != NULL)
		datagram_done(pak);
	    vipc_set_idle_timer(socket);
	    socket->state = SS_DEAD;
	    return;
	}
	pak->retransmit_time = vipc_bump_retrans_timer(socket);

	newpak = pak_duplicate(pak);
	if (!newpak) {
	    return;
	}

	/*
	 * Send it again.
	 */
	vines_send(newpak);
    }
}

/*
 * vipc_check_metric
 *
 * Check the connection to see if there is enough information
 * available to compute the complete round trip time.
 */
static void vipc_check_metric (vinesipcsocket *socket)
{
    ushort svr_metric;

    if ((socket->need_metric & VIPC_NEED_VALID_METRIC) == 0)
	return;
    
    svr_metric = vines_get_server_metric(socket->remote_net);
    if ((svr_metric != 0) && (svr_metric != VINES_INFINITY)) {
	socket->need_metric &= ~(VIPC_NEED_VALID_METRIC);
	socket->servermetric = svr_metric;
	socket->totalcost = (socket->servermetric + socket->lasthop +
			     socket->bias) * ONETICK;
	if (mgd_timer_running(&socket->next_retrans_time))
	    mgd_timer_start(&socket->next_retrans_time, 0); /* fire immediately */
    }
}

/*
 * vipc_ack_timer_expired
 *
 * The ACK timer has expired.  Send a lone ACK packet.
 */
void vipc_ack_timer_expired (mgd_timer *expired_timer)
{
    vinesipcsocket *socket;

    socket = mgd_timer_context(expired_timer);
    mgd_timer_stop(expired_timer);
    vipc_check_metric(socket);
    vipc_send_ack(socket);
}

/*
 * vipc_idle_timer_expired
 *
 * The IDLE timer has expired.  The connection may now move on to the
 * next plane of existence.
 */
void vipc_idle_timer_expired (mgd_timer *expired_timer)
{
    vinesipcsocket *socket;

    socket = mgd_timer_context(expired_timer);
    mgd_timer_stop(expired_timer);
    if ((socket->state == SS_CONNECTING) ||
	(socket->state == SS_CONNECTED)) {
	socket->state = SS_IDLE;
	vipc_set_idle_timer(socket);
    } else if (socket->state == SS_IDLE) {
	socket->state = SS_DEAD;
	vipc_set_idle_timer(socket);
    } else if (socket->state == SS_DEAD) {
	vipc_kill_conn(socket, "timeout", TRUE);
    }
}

/*
 * vipc_retrans_timer_expired
 *
 * The RETRANS timer has expired.  Resend any unacknowledged messages.
 */
void vipc_retrans_timer_expired (mgd_timer *expired_timer)
{
    vinesipcsocket *socket;

    socket = mgd_timer_context(expired_timer);
    mgd_timer_stop(expired_timer);
    vipc_check_metric(socket);
    vipc_resend_packets(socket);
}

/*
 * vines_process_connections
 *
 * Check each connection to see if ots complete round trip time is
 * known.  If not, go compute it.
 */
void vines_process_connections (void)
{
    vinesipcsocket *socket, *prev;

    FOR_ALL_VIPC_SOCKETS_W_PREV(socket, prev) {
	vipc_check_metric(socket);
    }
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

char *vipc_portname (ushort port)
{
    static char scratch[10];
    int i;
    
    for (i = 0; vipc_port_names[i].number != 0; i++)
	if (port == vipc_port_names[i].number)
	    return(vipc_port_names[i].name);
    sprintf(scratch, "%d", port);
    return(scratch);
}

/*
 * vipc_display
 *
 * Display all IPC related information.  This includes traversing the
 * connection list and displaying all information about each active
 * connection.
 */
void vipc_display (void)
{
    vipc_port_info *port;
    vinesipcsocket *socket, *next;

    automore_enable(NULL);
    printf("\nVINES IPC Status:\n");
    printf("\n  Next Port: %u", vipc_port_number);
    printf("\n  Next Connection: %u", vipc_connection_number);
    printf("\n  Next check in: %d sec",
	   mgd_timer_left_sleeping(&vipc_timer)/ONESEC);

    printf("\n\n  Port  NetRPC  Owner");
    FOR_ALL_VIPC_PORTS(port) {
	printf("\n  %04x   %4s   %s", port->number,
	       (port->uses_netrpc ? "Yes" : "No"), port->owner);
    }

    FOR_ALL_VIPC_SOCKETS_W_NEXT(socket, next) {
	mem_lock(socket);

	printf("\n");
	printf("\n  Connection %u, state: %s", socket->local_id,
	       vipc_socket_states[socket->state]);
	printf("\n    Local address: %#z, id %04x, last port: %04x",
	       vines_network, vines_host,
	       socket->local_id, socket->last_local_port);
	printf("\n    Remote address: %#z, id %04x, last port: %04x",
	       socket->remote_net, socket->remote_host,
	       socket->remote_id, socket->last_remote_port);
	printf("\n    Last send seq: %04x, Last rcvd seq: %04x",
	       socket->last_send_seq, socket->last_rcvd_seq);
	printf("\n    Next send ack: %04x, Last sent ack: %04x",
	       socket->next_send_ack, socket->last_send_ack);
	printf("\n    Server metric %04x%s, last hop %04x%s, bias %04x, total %d (ms)",
	       socket->servermetric,
	       (socket->need_metric & VIPC_NEED_VALID_METRIC) ? "*" : "",
	       socket->lasthop,
	       (socket->need_metric & VIPC_NEED_LAST_HOP) ? "*" : "",
	       socket->bias, socket->totalcost);
	printf("\n    Send ACK in %d ms, Retransmit in %d ms",
	       mgd_timer_left_sleeping(&socket->next_ack_time),
	       mgd_timer_left_sleeping(&socket->next_retrans_time));
	printf("\n    Idle check in %d sec", 
	       mgd_timer_left_sleeping(&socket->next_idle_time)/ONESEC);
	printf("\n    Retransmit queue contains %d packets",
	       socket->retransQ.count);
	printf("\n    %sacket in reassembly",
	       socket->reassembly ? "P" : "No p");
	next = socket->next;
	free(socket);
	if (automore_quit())
	    break;
    }

    automore_disable();
}

/*
 * vipc_display_timer
 *
 * Display all the timer queues maintained by IPC.
 */
void vipc_display_timer (void)
{
    char temp[40];
    
    printf("\n    IPC: %s", vtimer_pretty_print(&vipc_timer, "off", temp));
}

/*
 * vipc_display_traffic
 *
 * Display the various activities that have been performed on received
 * packets, on a per port basis.
 */
void vipc_display_traffic (void)
{
    int i;
    ulong *p;
    char temp[40];

    p = vines_broadcast;
    printf("\nBroadcast: Other:%08d", *p++);
    FOR_ALL_VIPC_PORTS_COUNTED() {
	printf("%s %02x:%08x", ((i % 5) ? "," : "\n             "), i, *p++);
    }
    p = vines_helpered;
    printf("\n Helpered: Other:%08d", *p++);
    FOR_ALL_VIPC_PORTS_COUNTED() {
	printf("%s %02x:%08x", ((i % 5) ? "," : "\n             "), i, *p++);
    }
    p = vines_unicast;
    printf("\n  Unicast: Other:%08d", *p++);
    FOR_ALL_VIPC_PORTS_COUNTED() {
	printf("%s %02x:%08x", ((i % 5) ? "," : "\n             "), i, *p++);
    }
    p = vines_proxied;
    printf("\n Proxied:  Other:%08d", *p++);
    FOR_ALL_VIPC_PORTS_COUNTED() {
	printf("%s %02x:%08x", ((i % 5) ? "," : "\n             "), i, *p++);
    }
    p = vines_proxyback;
    printf("\nP_Replies: Other:%08d", *p++);
    FOR_ALL_VIPC_PORTS_COUNTED() {
	printf("%s %02x:%08x", ((i % 5) ? "," : "\n             "), i, *p++);
    }

    printf("\n    IPC: %s", vtimer_pretty_print(&vipc_timer, "off", temp));
}

/*
 * display_vinespak_ipc
 *
 * Print out the IPC information in a packet.
 */
uchar *display_vinespak_ipc (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vinesipctype *ipc;
    vinesipc2type *ipc2;

    ipc = (vinesipctype *)data;
    (*fn)("\n   IPC sport %04x, dport %04x, type   %02x, ctl   %02x",
	  ipc->srcport, ipc->dstport, ipc->type, ipc->control);

    if (ipc->type == VIPC_DATAGRAM) {
	ipc2 = (vinesipc2type *)data;
	*limit -= sizeof(vinesipc2type);
	return(display_vinespak_netrpc(ipc2->data, limit, fn));
    }

    (*fn)("\n       lid   %04x, rid   %04x, seq  %04x, ack %04x, len %04x",
	  ipc->local_id, ipc->remote_id, ipc->sequence, ipc->ack, ipc->length);
    if (ipc->type == VIPC_DATA) {
	*limit = ipc->length;
	return(display_vinespak_netrpc(ipc->data, limit, fn));
    }
    *limit = 0;
    return(ipc->data);
}

/*
 * display_vinespak_spp
 *
 * Print out the SPP information in a packet.
 */
uchar *display_vinespak_spp (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vinesspptype *spp;

    spp = (vinesspptype *)data;
    (*fn)("\n   SPP sport %04x, dport %04x, type   %02x, ctl   %02x",
	  spp->srcport, spp->dstport, spp->type, spp->control);
    (*fn)("\n       lid   %04x, rid   %04x, seq  %04x, ack %04x, win %04x",
	  spp->local_id, spp->remote_id, spp->sequence, spp->ack,
	  spp->window);
    *limit -= sizeof(vinesspptype);
    return(spp->data);
}

/**********************************************************************
 *
 *			  MESSAGE FORWARDING
 *
 **********************************************************************/

/*
 * vines_forward_nearest
 *
 * Locate the nearest _Banyan_ server and forward the frame in it's
 * direction.  This routine is at the heart of the serverless support
 * code.
 */
boolean vines_forward_nearest (paktype *pak)
{
    paktype *newpak;
    vinesiptype *newvip;
    routingdecision thewaythere;
    vinesipctype *ipc;
    vinesneighbortype *gw;
    boolean broadcast;
    vinesidbtype *outidb;

    broadcast = FALSE;
    ipc = (vinesipctype *)pak->transport_start;

    /*
     * Lookup the router that is the next step closer to a real Banyan
     * server.  Make sure that there is a valid path and interface given,
     * and that it is not the same interface the the packet was received
     * on.
     */
    if (!vines_from_reverse_path(pak))
	return(FALSE);
    if (!(vines_nearest_server(&thewaythere, &broadcast)))
	return(FALSE);
    if ((!thewaythere.path || !thewaythere.path->idb) ||
	(pak->if_input == thewaythere.path->idb->phys_idb)) {
	vines_unlock_nodes(&thewaythere);
	return(TRUE);
    }

    newpak = pak_duplicate(pak);
    if (!newpak) {
	vines_unlock_nodes(&thewaythere);
	return(TRUE);
    }
	
    gw = thewaythere.neighbor;
    newpak->if_output = thewaythere.path->idb->phys_idb;

    newvip = (vinesiptype *)newpak->network_start;
    PUT2WD(newvip->ddstnet, VINES_ALLNETS);
    newvip->dsthost = VINES_ALLHOSTS;
    newvip->tc &= ~VINES_HOPS;
    if (vinesipc_debug || vines_proxy_debug) {
	if (ipc->type > VIPC_LAST)
	    buginf("\nVIPC: forwarding IPC unknown(%x) to ", ipc->type);
	else
	    buginf("\nVIPC: forwarding IPC %s to ", vipc_types[ipc->type]);
	if (broadcast)	
	    buginf("%s", thewaythere.path->idb->phys_idb->namestring);
	else
	    buginf("%#z", gw->net, gw->host);
    }
    vines_getidb(pak->if_input)->rx_bcast_helpered++;
    outidb = vines_send1(newpak, broadcast ? NULL : &thewaythere);
    if (outidb) {
	if (broadcast) {
	    outidb->tx_bcast++;
	} else {
	    outidb->tx_unicast++;
	}
    }
    vines_unlock_nodes(&thewaythere);
    return(TRUE);
}

/**********************************************************************
 *
 *			    PROXY SERVICE
 *
 **********************************************************************/

/*
 * vines_create_proxy
 *
 * Create the data structures so that a clients query can be associated
 * with the response coming back from the Banyan server.
 */
vinesproxytype *vines_create_proxy (vinesipcsocket *from, vinesipcsocket *to)
{
    vinesproxytype *proxy;

    if (!(proxy = malloc_named(sizeof(vinesproxytype), "Vines Proxy")))
	return(FALSE);
    proxy->proxy_socket = to;
    proxy->query_socket = from;
    proxy->query_from_port = from->last_remote_port;
    proxy->query_to_port = from->last_local_port;
    p_enqueue(&vines_proxyQ, proxy);
    return(proxy);
}

/*
 * vines_flush_proxy_queue
 *
 * Run the proxy queue flushing any entries that point to the specified
 * socket.
 */
void vines_flush_proxy_queue (vinesipcsocket *socket)
{
    vinesproxytype *proxy, *next = NULL;

    for (proxy = vines_proxyQ.qhead; proxy; proxy = next) {
	next = proxy->next;
	if ((proxy->proxy_socket == socket) ||
	    (proxy->query_socket == socket)) {
	    p_unqueue(&vines_proxyQ, proxy);
	    free(proxy);
	}
    }
}

/*
 * vipc_proxy_packet
 *
 * A client addressed a query directly to the router for which it has no
 * support.  Address the exact same query to a _Banyan_ server.  When it
 * responds, the response will be sent to the client as if the router had
 * originated it.  We send all of these from a single source port, so
 * they are easy to detect when they come back from the server.
 */
boolean vipc_proxy_packet (
    vinesipcsocket *socket,
    paktype *pak)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    vinesipcsocket *newsocket;
    vinesidbtype *idb;
    paktype *proxypak;
    routingdecision thewaythere;
    char *type;
    boolean local;
    ushort procedure;

    /*
     * Is this something that should be forwarded?
     */
    vip = (vinesiptype *)pak->network_start;
    ipc = (vinesipctype *)pak->transport_start;
    if (vip->srchost == VINES_SERVER)
	return(FALSE);
    idb = vines_getidb(pak->if_input);
    switch (idb->serverless) {
      case VSVLESS_NEVER:       	return(FALSE);
      case VSVLESS_ALWAYS:		break;
      case VSVLESS_ALWAYS_BCAST:	break;
      case VSVLESS_DYNAMIC:
	if (vines_servers_present(idb))
	    return(FALSE);
	break;
    }
    if (!vines_nearest_server(&thewaythere, &local))
	return(FALSE);

    /*
     * Perform all setup.
     */
    newsocket = vipc_create_conn(thewaythere.server->net, VINES_SERVER,
				   0, 0, TRUE);
    if (!newsocket) {
	if (vinesipc_debug)
	    buginf("\nVIPC: could not allocate socket for proxy");
	return(TRUE);
    }
    proxypak = vipc_getbuffer(ipc->length);
    if (!proxypak)
	return(TRUE);
    if (!vines_create_proxy(socket, newsocket)) {
	datagram_done(proxypak);
	return(TRUE);
    }

    /*
     * Off it goes.
     */
    bcopy(pak->session_start, proxypak->session_start, ipc->length);
    if (vinesipc_debug || vines_proxy_debug) {
	type = vipc_portname(ipc->dstport);
	procedure = vines_netrpc_extract_procedure(pak);
	buginf("\nVIPC: proxying %s procedure %d to %#z", type,
	       procedure, thewaythere.server->net, VINES_SERVER);
	if (vines_unknown_debug)
	    buginf_vinespak(pak);
    }
    vines_proxied[ipc->dstport]++;
    idb = vipc_send_data(newsocket, proxypak, ipc->dstport,
			 VIPC_PORT_CISCO_PROXY, VINES_HOPS);
    if (idb)
	idb->tx_proxy++;
    return(TRUE);
}

/*
 * vines_proxy_reply
 *
 * A server has sent us the response to a query that was forwarded by
 * this router.  Take the data portion of this message and package it up
 * for shipping back the original requester.  Note that we are here
 * simply because a message was received on the "proxy" port.
 */
void vines_proxy_reply (
    vinesipcsocket *socket,
    uchar *data,
    int length)
{
    vinesipcsocket *original;
    vinesproxytype *proxy;
    paktype *newpak;

    /*
     * Find it.
     */
    proxy = vines_proxyQ.qhead;
    for ( ; proxy; proxy = proxy->next) {
	if (proxy->proxy_socket == socket) {
	    p_unqueue(&vines_proxyQ, proxy);
	    break;
	}
    }
    if (!proxy)
	return;
    original = proxy->query_socket;
    if (!original) {
	if (vinesipc_debug)
	    buginf("\nVIPC: cannot find original socket.");
	free(proxy);
	return;
    }
    socket->last_idb_in->rx_proxy_reply++;

    /*
     * Repackage and ship.
     */
    newpak = vipc_getbuffer(length);
    if (!newpak) {
	free(proxy);
	return;
    }
    bcopy(data, newpak->session_start, length);
    if (vinesipc_debug || vines_proxy_debug) {
	buginf("\nVIPC: sending proxy reply packet to %#z",
	       original->remote_net, original->remote_host);
	if (vines_unknown_debug)
	    display_vinespak_ipc(data, &length, buginf);
    }
    vines_proxyback[socket->last_remote_port]++;
    vipc_send_data(original, newpak, proxy->query_from_port,
			     proxy->query_to_port, VINES_HOPS);
    free(proxy);
    return;
}
