/* $Id: vines_services.c,v 3.6.18.3 1996/08/28 13:19:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_services.c,v $
 *------------------------------------------------------------------
 * vines_services.c - Support for the vines Application Layer Services.
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_services.c,v $
 * Revision 3.6.18.3  1996/08/28  13:19:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.2  1996/08/07  09:03:56  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.18.1  1996/03/18  22:32:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.3  1996/03/13  02:04:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.16.2  1996/03/07  11:05:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  21:47:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  20:14:12  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.5  1996/02/01  06:11:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/08  04:56:26  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/12/07  22:33:43  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:58:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/27  07:31:12  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.3  1995/06/23 18:48:55  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/20  20:11:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:15:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define __VINES_SERVICES_C__

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "registry.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "vsvc_st.h"
#include "parser_defs_vines.h"

/*
 * Constants
 */
const char vsvc_nomem[] = "\nCannot allocate memory for socket structure.";
const char vsvc_str[] = "VSVC";

/*
 * Local
 */
rbTree* vines_server_tree;

/*
 * Globals
 */
mgd_timer vines_svc_timer;
mgd_timer vsvc_tree_timer;
queuetype     vsvc_queue;		/* Queue of all supported services */


/**********************************************************************
 *
 *			   Miscellaneous
 *
 **********************************************************************/

/*
 * vsvc_create_address
 *
 * Create a "VINES Address" structure.  This is the 16 byte address
 * structure that Banyan services and programs pass around.
 */
vinesaddress *vsvc_create_address (
    vinesaddress *data,
    ushort port)
{
    memset(data, 0, sizeof(vinesaddress));
    data->family = VINES_ADDR_NATIVE;
    PUTLONG(data->f.vip.nnet, vines_network);
    data->f.vip.host = vines_host;
    data->f.vip.port = port;
    return(data);
}

#ifdef NOBODY_CALLS_ME
void vsvc_break_port (
    vinesaddress *address,
    ulong *net,
    ushort *host,
    ushort *port)
{
    if (address->family != VINES_ADDR_NATIVE) {
	*net = 0;
	*host = 0;
	*port = 0;
	return;
    }
    *net = GETLONG(address->f.vip.nnet);
    *host = address->f.vip.host;
    *port = address->f.vip.port;
}
#endif /* NOBODY_CALLS_ME */


/*
 * vsvc_socket_to_address
 *
 * Create a "VINES Address" structure from the remote address of a
 * socket.  This is the 16 byte address structure that Banyan services
 * and programs pass around.
 */
vinesaddress *vsvc_socket_to_address (
    vinesaddress *data,
    vinesipcsocket *socket)
{
    memset(data, 0, sizeof(vinesaddress));
    data->family = VINES_ADDR_NATIVE;
    PUTLONG(data->f.vip.nnet, socket->remote_net);
    data->f.vip.host = socket->remote_host;
    data->f.vip.port = socket->last_remote_port;
    return(data);
}

/*
 * vsvc_hostname
 *
 * Convert the router's hostname into a legal vines group name.
 */
char* vsvc_hostname (void)
{
    static char tmp[20];

    if (strlen(vines_hostname))
	return(vines_hostname);
    if (strlen(hostname) == 0)
	return("NoName");
    sstrncpy(tmp, hostname, VST_DOMAINLEN);
    lowercase(tmp);
    tmp[0] = toupper(tmp[0]);
    return(tmp);
}

/*
 * vsvc_create_svc_name
 *
 * Given the item part of a name, create a complete service name.
 */
static inline void vsvc_create_svc_name (vsvc_info *svc)
{
    char *hostname;

    hostname = vsvc_hostname();
    vsvc_make_name(svc->svc_name, svc->abbrev, hostname, "Servers");
}

/*
 * vsvc_create_nonce
 */
void vsvc_create_nonce (vinesnonce *nonce)
{
    uchar *bytes;

    bytes = (uchar *)nonce;
    PUTLONG(bytes+0, vines_network);
    PUTSHORT(bytes+4, vines_host);
    PUTLONG(bytes+6, 0);
    PUTLONG(bytes+10, -1);
}
/*
 * vsvc_print_nonce
 *
 * Print a VINES "nonce" in an easy to read format.  A nonce is the 14
 * byte session identifier/key, similar to a kerberos ticket.
 */
char *vsvc_print_nonce (vinesnonce *nonce)
{
    static char space[40];
    ulong net;
    ushort host;
    uchar *bytes;

    bytes = (uchar *)nonce;
    net = GETLONG(bytes+0);
    host = GETSHORT(bytes+4);
    sprintf(space, "%#z:%08x%08x", net, host, GETLONG(bytes+6),
	    GETLONG(bytes+10));
    return(space);
}

/*
 * vsvc_validate_nonce
 *
 * Validate a VINES "nonce" that was passed to us in a user query.  A
 * nonce is the 14 byte session identifier/key, similar to a kerberos
 * ticket.
 */
ushort vsvc_validate_nonce (vsvc_info *service, vinesnonce *nonce)
{
    ushort invalid = VSVC_NO_ERROR;

    if (*service->debug) {
	buginf("\nV%s:  user %s%s", service->abbrev, vsvc_print_nonce(nonce),
	       (invalid ? "(invalid)" : ""));
    }
    return(invalid);
}

void vsvc_break_nonce (vinesnonce *nonce, ulong *net, ushort *host)
{
    uchar *bytes;

    bytes = (uchar *)nonce;
    *net = GETLONG(bytes+0);
    *host = GETSHORT(bytes+4);
}

/*
 * vsvc_sprintf_address
 *
 * Print a vines address ("port") structure.
 */
char *vsvc_sprintf_address (char *space, vinesaddress *addr)
{
    switch (addr->family) {
      case VINES_ADDR_NATIVE:
	sprintf(space, vines_decimal_addresses ? "%#z:%u" : "%#z:%x",
		GETLONG(addr->f.vip.nnet), addr->f.vip.host,
		addr->f.vip.port);
	return(space);
      default:
	return("unknown");
    }
}

/*
 * vsvc_sprintf_version
 *
 * Print a vines version number as a string.
 */
char *vsvc_sprintf_version (char *string, ulong version)
{
    int minor, edit;

    edit = version % 100;
    version /= 100;
    minor = version % 100;
    version /= 100;

    if (edit >= 50) {
	sprintf(string, "%d.%02d(%d)", version, minor, edit-50);
    } else if (edit > 0) {
	sprintf(string, "%d.%02d(Beta-%d)", version, minor, edit);
    } else {
	sprintf(string, "%d.%02d", version, minor);
    }
    return(string);
}

/*
 * vsvc_oops_buginf
 *
 * Something is really screwed up.  The router can't find the data
 * structures for a routine that should exist.
 */
void vsvc_oops_buginf (
    char *service,
    char *routine,
    char *msg_type)
{
    errmsg(&msgsym(RTNNOTFOUND, VINES), service, routine, msg_type);
}

/*
 * vsvc_c2b_iftype
 *
 * Convert a idb pointer into an identifier for the *interface* type.
 * Most routines use this conversion, but a few require a conversion from
 * idb pointer into the *protocol* types used on that interface.  Why is
 * beyond me, but thats how they do it.
 */
ushort vsvc_c2b_iftype (vinesidbtype *idb)
{
    hwidbtype *hwidb;

    hwidb = idb->phys_idb->hwptr;
    if (hwidb->status & IDB_ETHER)
	return(VSVC_IF_ETHER);
    if (hwidb->status & IDB_TR)
	return(VSVC_IF_TOKEN);
    if (hwidb->status & IDB_FDDI)
	return(VSVC_IF_FDDI);
    if (hwidb->status & IDB_ATM)
	return(VSVC_IF_ATM);
    if (hwidb->status & IDB_ISDN)
	return(VSVC_IF_ISDN);
    if (!(hwidb->status & IDB_SERIAL))
	return(0);

    if (hwidb->enctype == ET_HDLC)
	return(VSVC_IF_HDLC);
    if (is_x25(hwidb))
	return(VSVC_IF_X25);
    if (is_smds(hwidb))
	return(VSVC_IF_SMDS);
    if (is_frame_relay(hwidb))
	return(VSVC_IF_FRAME);
    return(0);
}

/*
 * vsvc_c2b_wantype
 *
 * Convert a idb pointer into an identifier for the *protocol* type used
 * on that interface.  Why Banyan does not always use *interface* types
 * is beyond me, but thats how they do it.  This routine is only called
 * for WAN interfaces.
 */
ushort vsvc_c2b_wantype (vinesidbtype *idb)
{
    hwidbtype *hwidb;

    if (!vines_is_wan(idb))
	return(0);
    hwidb = idb->phys_idb->hwptr;
    if (!(hwidb->status & IDB_SERIAL))
	return(0);
    if (hwidb->enctype == ET_HDLC)
	return(VINES_HDLC);
    if (is_x25(hwidb))
	return(VINES_X25);
    if (is_smds(hwidb))
	return(VINES_SMDS);
    if (is_frame_relay(hwidb))
	return(VINES_FRAME);
    return(0);
}


/**********************************************************************
 *
 *		    Service/Routine Manipulation
 *
 **********************************************************************/

/*
 * vsvc_find_service
 *
 * Given a port number, find the VINES service that has registered for
 * that port.  The port number can be either a well known port number, or
 * a transient port number.  It must be a port number on which the router
 * is expecting messages wrapped inside RPC headers.  Raw IPC ports are
 * found via the IPC port queue.
 */
vsvc_info *vsvc_find_service (ushort port)
{
    vsvc_info *service;

    FOR_ALL_SERVICES(service) {
	if (service->well_known_port == port)
	    return(service);
	if (service->transient_port == port)
	    return(service);
	if (service->temporary_port == port)
	    return(service);
    }
    return(NULL);
}

/*
 * vsvc_find_routine
 *
 * Given a service and a routine number, find the data structured
 * describing what to do with messages for that routine.
 */
vsvc_rtn_info *vsvc_find_routine (
    vsvc_info *service,
    ushort routine)
{
    int i;

    for (i = 0; i < service->routine_count; i++) {
	if (service->routines[i].number == routine)
	    return(&service->routines[i]);
    }
    return(NULL);
}

/*
 * vsvc_make_record
 *
 * Remember the procedure number that is about to be sent to a server.
 * This is necessary because Banyan doesn't include the called procedure
 * number with the results of the procedure.  (Gah.)  Thus this set of
 * routine.
 */
static vsvc_record *vsvc_make_record (
    vinesipcsocket *socket,
    vsvc_info *service,
    vsvc_rtn_info *routine)
{
    vsvc_record *record;

    if (socket == NULL) {
	service->last_broadcast = routine->number;
	return(NULL);
    }

    record = malloc(sizeof(vsvc_record));
    if (record == NULL)
	return(NULL);
    record->net  = socket->remote_net;
    record->host = socket->remote_host;
    record->procedure = routine->number;

    record->q.parent = record;
    lw_insert(&record->q, service->queries.blink);
    return(record);
}

/*
 * vsvc_delete_record
 *
 * We've received a response for this message.  Unqueue the note.
 */
static void vsvc_delete_record (vsvc_record *record)
{
    if (record == NULL)
	return;
    lw_remove(&record->q);
    free(record);
}

/*
 * vsvc_find_record
 *
 * Look up the last procedure called on this server/service.
 */
static ushort vsvc_find_record (
    vsvc_info *service,
    vinesipcsocket *socket)
{
    vsvc_record *record;
    dqueue_t *p;
    ushort proc;

    for (p = service->queries.flink; p != &service->queries; p = p->flink) {
	record = p->parent;
	if ((record->net  == socket->remote_net) &&
	    (record->host == socket->remote_host)) {
	    proc = record->procedure;
	    vsvc_delete_record(record);
	    return(proc);
	}
    }
    if (service->last_broadcast) {
	proc = service->last_broadcast;
	service->last_broadcast = 0;
	return(proc);
    }
    return(0);
}

/*
 * vsvc_install_svc
 *
 * Install a service record into the list of services that the router
 * knows about.
 */
void vsvc_install_svc (vsvc_info *service)
{
    enqueue(&vsvc_queue, service);
    mgd_timer_init_leaf(&service->timer, &vines_svc_timer, VT_SVC_SVC, service,
			FALSE);

    /*
     * Strings
     */
    vsvc_sprintf_version(service->version_string, service->version);

    service->queries.flink = &service->queries;
    service->queries.blink = &service->queries;
}

/*
 * vsvc_remove_svc
 *
 * Remove a service record from the list of services that the router
 * knows about.
 */
static inline void vsvc_remove_svc (vsvc_info *service)
{
    vipc_port_info *port_info;

    unqueue(&vsvc_queue, service);

    /*
     * Ports
     */
    port_info = vipc_find_port(service->well_known_port);
    vipc_destroy_port(port_info);
    port_info = vipc_find_port(service->transient_port);
    vipc_destroy_port(port_info);
    service->transient_port = VIPC_PORT_UNKNOWN;

    /*
     * Timers
     */
    mgd_timer_stop(&service->timer);
}

/*
 * vsvc_service_overview
 *
 * Print an overview of the services that the router knows anything about.
 */
static void vsvc_service_overview (void)
{
    vsvc_info *service;
    char time1[30], time2[30], time3[30];
    char minver[20], maxver[20];
    automore_enable(NULL);
    FOR_ALL_SERVICES(service) {
	printf("\n\n%s Service:", service->name);
	printf("\n    Name:     %s  (%s)", service->svc_name, service->abbrev);
	printf("\n    Ports:    Well Known %d, Transient %d",
	       service->well_known_port, service->transient_port);
	if (service->temporary_port)
	    printf(", Data %d", service->temporary_port);
	if (service->version) {
	    printf("\n    Emulates: %s, ", service->version_string);
	    vsvc_sprintf_version(minver, service->min_acceptable);
	    vsvc_sprintf_version(maxver, service->max_acceptable);
	    printf(" Supports: %s - %s", minver, maxver);
	}
	if (mgd_timer_running(&service->timer)) {
	    sprint_ticks_in_dhms(time1, mgd_timer_left_sleeping(&service->timer));
	    sprint_ticks_in_dhms(time2, service->timer_period_init);
	    sprint_ticks_in_dhms(time3, service->timer_period_normal);
	    printf("\n    Timer:    %s, initial %s, interval %s",
		   time1, time2, time3);
	} else {
	    printf("\n    Timer:    not running");
	}
    }
    automore_disable();
}

/*
 * vsvc_display_timer
 *
 * Display all the timer queues maintained by the Applications Layer.
 */
void vsvc_display_timer (void)
{
    vsvc_info *service;
    char temp[40];
    

    printf("\n    Services:");
    printf("\n      Master timer: %s",
	   vtimer_pretty_print(&vines_svc_timer, "off", temp));
    printf("\n      Service tree ager: %s",
	   vtimer_pretty_print(&vsvc_tree_timer, "off", temp));

    FOR_ALL_SERVICES(service) {
	printf("\n      %s Service: %s", service->name,
	       vtimer_pretty_print(&service->timer, "off", temp));
    }
}

/*
 * vsvc_dump
 *
 * Print the contents of the application layer.  This is used in
 * development to see raw message data, but should never be called in a
 * production router (not that it would hurt anything.)
 */
void vsvc_dump (
    vsvc_info *service,
    char *tag,
    uchar *data,
    int length)
{
    int i, j;
    uchar p;

    if (length == 0) {
	buginf("\nV%s: %s (no data)", service->abbrev, tag);
	return;
    }

    for (i = 0; i < length; i += 16) {
	if ((i % 16) == 0)
	    buginf("\nV%s: %s", service->abbrev,
		   (i == 0) ? tag : "     ");
	for (j = 0; (j < 16) && (i + j < length); j++)
	    buginf(" %02x", data[i+j]);
	for ( ; j < 16 ; j++)
	    buginf("   ");
	buginf("   ");
	for (j = 0; (j < 16) && (i + j < length); j++) {
	    p = data[i+j] & 0x7F;
	    if (p <= 0x1F) p = '.';
	    if (p == 0x7F) p = '.';
	    buginf("%c", p);
	}
    }
}

/**********************************************************************
 *
 *		    Servers Tree Maintenance Routines
 *
 **********************************************************************/

static boolean vsvc_delete_server_info_entry (
    treeLink *x,
    void *dummy)
{
    serverentry* server;

    server = vsvc_ServerCast(x);
    if (server->serverid == vines_network)
	return(TRUE);
    vsvc_DeleteServer(server);
    return(TRUE);
}

static boolean vsvc_age_server_info_entry (
    treeLink *x,
    void *dummy)
{
    vsvc_info *service;
    serverentry* server;
    boolean keep_entry;

    server = vsvc_ServerCast(x);
    keep_entry = FALSE;
    for (service = vsvc_queue.qhead; service; service = service->next) {
	if (service->age_service_info)
	    keep_entry |= (*service->age_service_info)(server);
    }

    if (server->serverid == vines_network)
	return(TRUE);
    if (!keep_entry) {
	vsvc_DeleteServer(server);
    }
    return(TRUE);
}

static void vines_server_tree_ager (boolean flush)
{
    if (flush) {
	vsvc_WalkServerTree(vsvc_delete_server_info_entry, NULL);
    } else {
	vsvc_WalkServerTree(vsvc_age_server_info_entry, NULL);
    }
}


/**********************************************************************
 *
 *			   PROXY SERVICE
 *
 **********************************************************************/

/*
 * vsvc_proxy_call
 *
 * A client addressed a query directly to the router for which it has no
 * support.  Address the exact same query to a _Banyan_ server.  When it
 * responds, the response will be sent to the client as if the router had
 * originated it.
 */
static boolean vsvc_proxy_call (
    vinesipcsocket *socket,
    vrpc_call *call,
    uchar *data_to_proxy,
    int data_length)
{
    vinesipcsocket *newsocket;
    paktype *pak;
    ushort port;
    ulong net;

    /*
     * Is this something that should be forwarded?
     */
    if (socket->remote_host == VINES_SERVER)
	return(FALSE);
    net = vines_nearest_server_net();
    if (net == VINES_UNKNOWNNET) {
	if (vines_proxy_debug)
	    buginf("\nVSVC: no servers present to receive proxy");
	return(FALSE);
    }

    /*
     * Perform all setup.
     */
    newsocket = vipc_create_conn(net, VINES_SERVER, 0, 0, TRUE);
    if (!newsocket) {
	if (vines_proxy_debug)
	    buginf("\nVSVC: could not allocate socket for proxy");
	return(TRUE);
    }
    pak = vrpc_getbuffer(VRPC_CALL, data_length);
    if (!pak) {
	if (vines_proxy_debug)
	    buginf("\nVSVC: could not allocate packet for proxy");
	return(TRUE);
    }
    if (!vines_create_proxy(socket, newsocket)) {
	if (vines_proxy_debug)
	    buginf("\nVSVC: could not allocate proxy structure");
	retbuffer(pak);
	return(TRUE);
    }

    /*
     * Off it goes.
     */
    bcopy(data_to_proxy, pak->application_start, data_length);
    port = socket->last_local_port;
    if (vines_proxy_debug)
	buginf("\nVSVC: proxying %s procedure %d to %#Z", vipc_portname(port),
	       call->procedure, net);
    port = (port <= VIPC_PORT_MAXCOUNTED) ? port : 0;
    vines_proxied[port]++;
    vrpc_send_long_call(newsocket, pak, call->program, call->version,
			call->procedure, socket->last_local_port,
			VIPC_PORT_CISCO_PROXY);
    return(TRUE);
}


/**********************************************************************
 *
 *	                 INTERFACE TO/FROM NetRPC
 *
 **********************************************************************/

/**************************************************
 *
 *                  NetRPC CALL
 *
 **************************************************/

/*
 * vsvc_call_default_debug
 *
 * Print a message that the service default action for a routine was
 * taken.
 */
static inline void vsvc_call_default_debug (
    vsvc_info *service,
    ushort procedure,
    const char *text)
{
    if (*service->debug)
	buginf("\nV%s: %s call for procedure %d (svc default)", service->abbrev,
	       text, procedure);
}

/*
 * vsvc_call_action_debug
 *
 * Print a message telling what action was taken for a specific routine.
 */
static inline void vsvc_call_action_debug (
    vsvc_info *service,
    vsvc_rtn_info *routine,
    const char *text)
{
    if (*service->debug)
	buginf("\nV%s: %s call for procedure `%s'", service->abbrev, text,
	       routine->name);
}

/*
 * vsvc_send_call
 *
 * Send a message for an application layer service.  This routine has the
 * responsibility of converting unpacked data structures, used by the
 * code for the services, into packed data structures that can be sent
 * across the network.  It differentiates between directed and broadcast
 * messages, and makes the appropriate calls for each.
 */
void vsvc_send_call (
    vinesipcsocket *socket,
    vsvc_info *service,
    vsvc_rtn_info *routine,
    uchar *data)
{
    vsvc_record *record;
    paktype *query;
    uchar *c;

    /*
     * Don't forget to record the call for when the response comes back.
     * Boy did Banyan goof here.
     */
    record = vsvc_make_record(socket, service, routine);
    if (socket && (record == NULL)) {
	if (service->debug)
	    buginf("\nV%s: call record failed.", service->abbrev);
	return;
    }

    /*
     * Get a packet to send.
     */
    if (socket) {
	query = vrpc_getbuffer(VRPC_CALL, routine->query_len);
    } else {
	query = vrpc_b_getbuffer(routine->query_len);
    }
    if (!query) {
	errmsg(&msgsym(NOBUFFERS, VINES), "sending service query");
	vsvc_delete_record(record);
	return;
    }

    /*
     * Call the packing routine that was automatically generated from the
     * service description.
     */
    if (routine->send_query_action & VSVC_SEND_DATA) {
	if (routine->query_pack) {
	    c = (*routine->query_pack)(query->application_start, data);
	    query->datagramsize = (c - query->application_start);
	} else {
	    errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
		   routine->name, "query pack");
	    vsvc_delete_record(record);
	    retbuffer(query);
	    return;
	}
    } else {
	query->datagramsize = 0;
    }

    /*
     * Count it.
     */
    service->msgs_out++;
    service->bytes_out += query->datagramsize;

    /*
     * Print some debugging and send the packet.
     */
    if (*service->debug) {
	ulong  net  = (socket) ? socket->remote_net  : VINES_ALLNETS;
	ushort host = (socket) ? socket->remote_host : VINES_ALLHOSTS;
	buginf("\nV%s: sending %s call to %#z", service->abbrev,
	       routine->name, net, host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Query", query->application_start,
		      query->datagramsize);
    }
    if (socket) {
	vrpc_send_long_call(socket, query, 0, 1, routine->number,
			    service->well_known_port, service->transient_port);
    } else {
	vrpc_send_broadcast_call(query, NULL, NULL, 0, 1, routine->number,
				 service->well_known_port, 
				 service->transient_port, 0, 0);
    }
}

/*
 * vsvc_dispatch_call
 *
 * A message has been received for an application layer service. This
 * routine has the responsibility of converting the packed data
 * structures received from the network into an unpacked data structures
 * that can be used by the services code.  It is also responsible for
 * packing the response message and sending it back to the requester, or
 * for sending back error messages.
 */
boolean vsvc_dispatch_call (
    vinesipcsocket *socket,
    vsvc_info *service,
    vrpc_call *call,
    uchar *packed_data,
    int packed_length)
{
    ushort procedure = call->procedure;
    vsvc_rtn_info *routine;
    uchar *query_data, *reply_data, *c;
    int bytes_parsed;
    ulong result_code;
    paktype *response;

    /*
     * See if the service is supported.
     */
    if (!service) {
	if (vinesnetrpc_debug)
	    buginf("\nVSVC: Service %#04x not supported (call)",
		   socket->last_local_port);
	return(FALSE);
    }

    /*
     * Count it.
     */
    service->msgs_in++;
    service->bytes_in += packed_length;

    /*
     * Now see if this transaction request is supported by the router (in
     * which case a routine exists).  Handle the default responses for a
     * service here if the routine doesn't exist.  This can be either a
     * REJECT, IGNORE, or PROXY response.
     */
    routine = vsvc_find_routine(service, procedure);
    if (!routine) {
	switch (service->routine_default) {
	  case VSVC_IGNORE:
	    vsvc_call_default_debug(service, procedure, "Ignoring");
	    if (*service->debug & VERBOSE_DEBUG)
		vsvc_dump(service, "Query", packed_data, packed_length);
	    return(TRUE);
	  case VSVC_PROXY:
	    if (socket->last_was_broadcast) {
		vsvc_call_default_debug(service, procedure, "Ignoring broadcast");
		return(TRUE);
	    }
	    vsvc_call_default_debug(service, procedure, "Proxying");
	    if (*service->debug & VERBOSE_DEBUG)
		vsvc_dump(service, "Query", packed_data, packed_length);
	    vsvc_proxy_call(socket, call, packed_data, packed_length);
	    return(TRUE);
	  default:
	    vsvc_call_default_debug(service, procedure, "Rejecting");
	    if (*service->debug & VERBOSE_DEBUG)
		vsvc_dump(service, "Query", packed_data, packed_length);
	    vrpc_send_abort(socket, service->reject_code);
	    return(TRUE);
	}
    }

    /*
     * The router has been explicitly programmed for this transaction
     * request.  Perform the specified response for this routine.  This
     * can be either a REJECT, IGNORE, or PROXY response.
     */
    switch (routine->rcv_query_action) {
      case VSVC_ACCEPT_DATA:
      case VSVC_ACCEPT_NODATA:
	break;
      case VSVC_IGNORE:
	vsvc_call_action_debug(service, routine, "Ignoring");
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Query", packed_data, packed_length);
	return(TRUE);
      case VSVC_PROXY:
	if (socket->last_was_broadcast) {
	    vsvc_call_action_debug(service, routine, "Ignoring");
	    return(TRUE);
	}
	vsvc_call_action_debug(service, routine, "Proxying");
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Query", packed_data, packed_length);
	vsvc_proxy_call(socket, call, packed_data, packed_length);
	return(TRUE);
      case VSVC_REJECT:
      default:
	vsvc_call_action_debug(service, routine, "Rejecting");
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Query", packed_data, packed_length);
	vrpc_send_abort(socket, routine->rcv_reject_code);
	return(TRUE);
    }

    /*
     * Now that we know this is a keeper, dump the contents.
     */
    if (*service->debug) {
	buginf("\nV%s: received %s call from %#z", service->abbrev,
	       routine->name, socket->remote_net, socket->remote_host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Query", packed_data, packed_length);
    }

    /*
     * The programmed response was to accept this transaction request.
     * Check, parse, and unpack all of the arguments to the call.
     */
    query_data = NULL;
    if (routine->rcv_query_action & VSVC_ACCEPT_NODATA) {
	if (packed_length != 0) {
	    if (*service->debug)
		buginf("\nV%s: %s not expecting call data, received %d bytes",
		       service->abbrev, routine->name, packed_length);
	    vrpc_send_reject(socket, VRPC_REJ_UNSPECIFIED);
	    return(TRUE);
	}
    } else if (routine->query_unpack) {
	query_data = malloc(routine->query_len);
	if (query_data == NULL) {
	    vrpc_send_abort(socket, VSVC_ERR_NO_MEMORY);
	    return(TRUE);
	}
	c = (*routine->query_unpack)(packed_data, query_data);
	bytes_parsed = (c - packed_data);
	if ((bytes_parsed > packed_length) ||
	    (!routine->query_padded && (bytes_parsed < packed_length))) {
	    free(query_data);
	    if (*service->debug)
		buginf("\nV%s: %s call parse failed, %d bytes rcvd, %d bytes parsed",
		       service->abbrev, routine->name, packed_length,
		       bytes_parsed);
	    vrpc_send_reject(socket, VRPC_REJ_UNSPECIFIED);
	    return(TRUE);
	}
    } else {
	errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
	       routine->name, "query unpack");
	vrpc_send_reject(socket, VRPC_REJ_UNSPECIFIED);
	return(TRUE);
    }

    /*
     * Get memory for the response (if there should be one)
     */
    reply_data = NULL;
    if (routine->send_reply_action & VSVC_SEND_DATA) {
	reply_data = malloc(routine->reply_len);
	if (reply_data == NULL) {
	    if (query_data)
		free(query_data);
	    vrpc_send_abort(socket, VSVC_ERR_NO_MEMORY);
	    return(TRUE);
	}
    }

    /*
     * Now that all that is finally done, call the routine to process the
     * call.  This routine should return TRUE indicating that it
     * successfully processed the CALL, but there are cases where it may
     * fail to process the CALL and return FALSE.  Check for both cases
     * and send either a RETURN (with or without data) an ABORT back to
     * the requester.
     */
    result_code = (*routine->rcv_query_fn)(socket, query_data, reply_data);
    if (result_code == VSVC_DEFERRED_REPLY) {
	/* Do nothing */
    } else if (result_code != VSVC_NO_ERROR) {
	if (*service->debug) {
	    buginf("\nV%s: sending abort (%d) to %#z", service->abbrev,
		   result_code, socket->remote_net, socket->remote_host);
	}
	vrpc_send_abort(socket, result_code);
	service->msgs_out++;
    } else if (routine->send_reply_action & VSVC_SEND_DATA) {
	if (routine->reply_pack) {
	    response = vrpc_getbuffer(VRPC_RETURN, routine->reply_len);
	    if (response) {
		c = (*routine->reply_pack)(response->application_start, reply_data);
		response->datagramsize = (c - response->application_start);
		service->bytes_out += response->datagramsize;
		service->msgs_out++;
		if (*service->debug) {
		    buginf("\nV%s: sending %s return to %#z", service->abbrev,
			   routine->name, socket->remote_net, socket->remote_host);
		    if (*service->debug & VERBOSE_DEBUG)
			vsvc_dump(service, "Reply", response->application_start,
				  response->datagramsize);  
		}
		vrpc_send_return(socket, response);
	    } else {
		errmsg(&msgsym(NOBUFFERS, VINES), "preparing service reply");
		vrpc_send_reject(socket, VRPC_REJ_UNSPECIFIED);
	    }
	} else {
	    errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
		   routine->name, "reply pack");
	    vrpc_send_reject(socket, VRPC_REJ_UNSPECIFIED);
	}
    }

    /*
     * Clean up.
     */
    if (query_data)
	free(query_data);
    if (reply_data)
	free(reply_data);
    return(result_code == 0);
}

/**************************************************
 *
 *                  NetRPC ABORT
 *
 **************************************************/

/*
 * vsvc_failure_debug
 *
 * Print a message that the reason a routine failed.
 */
static void vsvc_failure_debug (
    vsvc_info *service,
    vsvc_rtn_info *routine,
    const char *action,
    ushort reason)
{
    if (*service->debug == FALSE)
	return;
    if (routine) {
	buginf("\nV%s: %s %s with reason %d.", service->abbrev,
	       routine->name, action, reason);
	return;
    }
    buginf("\nV%s: unknown procedure %#04x %s with reason %d.",
	   service->abbrev, action, reason); 
}

/*
 * vsvc_dispatch_abort
 *
 * A message has been received for an application layer service. This
 * routine has the responsibility of removing the corresponding call
 from the outstanding transactions list.
 */
void vsvc_dispatch_abort (
    vinesipcsocket *socket,
    vsvc_info *service,
    ushort reason)
{
    vsvc_rtn_info *routine;
    ushort procedure;

    /*
     * See if the service is supported.
     */
    if (!service)
	return;

    /*
     * Now find the record of the last call that was sent to this
     * particular instance of the service.  This is necessary because the
     * 'abort' packet carries no indication of its contents.
     */
    procedure = vsvc_find_record(service, socket);
    if (procedure == 0) {
	if (*service->debug) {
	    buginf("\nV%s: No outstanding query, but abort from %#z, code %d",
		   service->abbrev, socket->remote_net, socket->remote_host,
		   reason);
	}
	return;
    }
    routine = vsvc_find_routine(service, procedure);
    vsvc_failure_debug(service, routine, "aborted", reason);
    if (routine->abort_fn)
	(*routine->abort_fn)(socket, reason);
}

/**************************************************
 *
 *                  NetRPC REJECT
 *
 **************************************************/

/*
 * vsvc_dispatch_reject
 *
 * A message has been received for an application layer service. This
 * routine has the responsibility of removing the corresponding call
 from the outstanding transactions list.
 */
void vsvc_dispatch_reject (
    vinesipcsocket *socket,
    vsvc_info *service,
    ushort reason)
{
    vsvc_rtn_info *routine;
    ushort procedure;

    /*
     * See if the service is supported.
     */
    if (!service)
	return;

    /*
     * Now find the record of the last call that was sent to this
     * particular instance of the service.  This is necessary because the
     * 'reject' packet carries no indication of its contents.
     */
    procedure = vsvc_find_record(service, socket);
    if (procedure == 0) {
	if (*service->debug) {
	    buginf("\nV%s: No outstanding query, but reject from %#z, code %d",
		   service->abbrev, socket->remote_net, socket->remote_host,
		   reason); 
	}
	return;
    }
    routine = vsvc_find_routine(service, procedure);
    vsvc_failure_debug(service, routine, "rejected", reason);
    if (routine->reject_fn)
	(*routine->reject_fn)(socket, reason);
}

/**************************************************
 *
 *                  NetRPC RETURN
 *
 **************************************************/

/*
 * vsvc_return_default_debug
 *
 * Print a message that the service default action for a routine was
 * taken.
 */
static inline void vsvc_return_default_debug (
    vsvc_info *service,
    ushort procedure,
    const char *text)
{
    if (*service->debug)
	buginf("\nV%s: %s reply from procedure %#04x (svc default)", service->abbrev,
	       text, procedure); 
}

/*
 * vsvc_return_action_debug
 *
 * Print a message telling what action was taken for a specific routine.
 */
static inline void vsvc_return_action_debug (
    vsvc_info *service,
    vsvc_rtn_info *routine,
    const char *text)
{
    if (*service->debug)
	buginf("\nV%s: %s reply from procedure `%s'", service->abbrev, text,
	       routine->name);
}

/*
 * vsvc_send_reply
 *
 * Send a message for an application layer service.  This routine has the
 * responsibility of converting unpacked data structures, used by the
 * code for the services, into packed data structures that can be sent
 * across the network.  Responses are always send as directed messages.
 */
void vsvc_send_reply (
    vinesipcsocket *socket,
    vsvc_info *service,
    vsvc_rtn_info *routine,
    uchar *data)
{
    paktype *response;
    uchar *c;

    /*
     * Get a packet to send.
     */
    response = vrpc_getbuffer(VRPC_RETURN, routine->reply_len);
    if (response == NULL) {
	errmsg(&msgsym(NOBUFFERS, VINES), "sending service reply");
	return;
    }

    /*
     * Call the packing routine that was automatically generated from the
     * service description.
     */
    c = (*routine->reply_pack)(response->application_start, data);
    response->datagramsize = (c - response->application_start);

    /*
     * Print some debugging and send the packet.
     */
    if (*service->debug) {
	buginf("\nV%s: sending %s return to %#z", service->abbrev,
	       routine->name, socket->remote_net, socket->remote_host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Reply", response->application_start,
		      response->datagramsize);  
    }
    vrpc_send_return(socket, response);
}

/*
 * vsvc_dispatch_return
 *
 * A message has been received for an application layer service. This
 * routine has the responsibility of converting the packed data
 * structures received from the network into an unpacked data structures
 * that can be used by the services code.
 */
boolean vsvc_dispatch_return (
    vinesipcsocket *socket,
    vsvc_info *service,
    uchar *packed_data,
    int packed_length)
{
    vsvc_rtn_info *routine;
    uchar *reply_data, *c;
    int bytes_parsed;
    ushort procedure;

    /*
     * See if the service is supported.
     */
    if (!service) {
	if (vinesnetrpc_debug)
	    buginf("\nVSVC: Service %#04x not supported (return)",
		   socket->last_local_port);
	return(FALSE);
    }

    /*
     * Count it.
     */
    service->msgs_in++;
    service->bytes_in += packed_length;

    /*
     * Now find the record of the last call that was sent to this
     * particular instance of the service.  This is necessary because the
     * 'return' packet carries no indication of its contents.
     */
    procedure = vsvc_find_record(service, socket);
    if (procedure == 0) {
	if (*service->debug) {
	    buginf("\nV%s: No outstanding query, but return from %#z",
		   service->abbrev, socket->remote_net, socket->remote_host);
	    if (*service->debug & VERBOSE_DEBUG)
		vsvc_dump(service, "Reply", packed_data, packed_length);  
	}
	return(TRUE);
    }

    /*
     * Now see if the routine is supported by the router.  Since this is
     * a reply, the only two cases will be "yes" it is supported (in
     * which case a routine exists), or "no" it isn't and how the hell
     * did we get here.
     */
    routine = vsvc_find_routine(service, procedure);
    if (!routine) {
	vsvc_return_default_debug(service, procedure, "Ignoring");
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Reply", packed_data, packed_length);
	return(TRUE);
    }

    /*
     * The router has been explicitly programmed for this transaction
     * reply.  Since this is a reply, the only two cases will be "yes" it
     * is supported, or "no" it isn't and how the hell did we get here.
     */
    switch (routine->rcv_reply_action) {
      case VSVC_ACCEPT_DATA:
      case VSVC_ACCEPT_NODATA:
	break;
      case VSVC_IGNORE:
      default:
	vsvc_return_action_debug(service, routine, "Ignoring");
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Reply", packed_data, packed_length);
	return(TRUE);
    }

    /*
     * The programmed response was to accept this transaction reply.
     * Check, parse, and unpack all of the arguments to the reply.
     */
    if (*service->debug) {
	buginf("\nV%s: received %s return from %#z", service->abbrev,
	       routine->name, socket->remote_net, socket->remote_host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Reply", packed_data, packed_length);  
    }
    reply_data = NULL;
    if (routine->rcv_reply_action & VSVC_ACCEPT_NODATA) {
	if (packed_length != 0) {
	    if (*service->debug)
		buginf("\nV%s: %s not expecting reply data, received %d bytes",
		       service->abbrev, routine->name, packed_length);
	    return(TRUE);
	}
    } else if (routine->reply_unpack) {
	reply_data = malloc(routine->reply_len);
	if (reply_data == NULL) {
	    return(TRUE);
	}
	c = (*routine->reply_unpack)(packed_data, reply_data);
	bytes_parsed = (c - packed_data);
	if (bytes_parsed != packed_length) {
	    free(reply_data);
	    if (*service->debug)
		buginf("\nV%s: %s reply parse failed, %d bytes rcvd, %d bytes parsed",
		       service->abbrev, routine->name, packed_length,
		       bytes_parsed);
	    return(TRUE);
	}
    } else {
	errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
	       routine->name, "reply unpack");
	return(TRUE);
    }

    /*
     * Now that all that is finally done, call the routine to process the
     * data.  The routine should return TRUE indicating that it
     * successfully processed the CALL, but there are cases where it may
     * fail to process the CALL and return FALSE.  This routine must
     * check for that contingency and send either a REJECT or an ABORT
     * back to the requester.
     */
    (*routine->rcv_reply_fn)(socket, reply_data);

    /*
     * Clean up.
     */
    if (reply_data)
	free(reply_data);
    return(TRUE);
}


/**************************************************
 *
 *                  NetRPC SEARCH
 *
 **************************************************/

/*
 * vsvc_send_search
 *
 * Send a message for an application layer service.  This routine has the
 * responsibility of packing the search parameters into the message that
 * will be sent across the network.  Searches are always send as
 * broadcast messages.
 */
void vsvc_send_search (
    vinesipcsocket *socket,
    vsvc_info *service,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3,
    uchar hops)
{
    if (*service->debug)
	buginf("\nV%s: sending search message (%d hops)", service->abbrev, hops);
    vrpc_send_search(service->version, service->min_acceptable, service->max_acceptable, 
		     user_num, user_string, info1, info2, info3,
		     service->well_known_port, service->transient_port, hops);
    
}

/*
 * vsvc_dispatch_search
 *
 * A search message has been received for an application layer service.
 */
boolean vsvc_dispatch_search (
    vinesipcsocket *socket,
    vsvc_info *service,
    ushort user_num,
    char *user_string,
    char *info1,
    char *info2,
    char *info3)
{
    /*
     * Call the service specific routine to check the optional arguments.  This
     * routine is responsible for sending a `returnaddress' message if all the
     * arguments pass.  Remember, the version check has already taken place.
     */
    if (*service->debug)
	buginf("\nV%s: received search from %#z", service->abbrev,
	       socket->remote_net, socket->remote_host);
    (*service->search_fn)(socket, user_num, user_string, info1, info2, info3);
    return(TRUE);
}

/**************************************************
 *
 *             NetRPC RETURNADDRESS
 *
 **************************************************/

/*
 * vsvc_send_returnaddress
 *
 * Send a message for an application layer service.  There are no
 * parameters to a search response other than the version number, so no
 * packing is necessary.  Search responses are always send as directed
 * messages.
 */
void vsvc_send_returnaddress (
    vinesipcsocket *socket,
    vsvc_info *service)
{
    if (*service->debug)
	buginf("\nV%s: sending return address message", service->abbrev);
    vrpc_send_returnaddress(socket, 0, service->version,
			    socket->last_remote_port, service->well_known_port); 
    
}

/*
 * vsvc_dispatch_returnaddress
 *
 * A search response message has been received for an application layer
 * service.
 */
void vsvc_dispatch_returnaddress (
    vinesipcsocket *socket,
    vsvc_info *service,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /*
     * Call the service specific routine to check the optional arguments.  This
     * routine is responsible for sending a `returnaddress' message if all the
     * arguments pass.  Remember, the version check has already taken place.
     */
    if (*service->debug)
	buginf("\nV%s: received return address from %#z", service->abbrev,
	       socket->remote_net, socket->remote_host);
    (*service->retadd_fn)(socket, proto_rev, service_rev, service_address);
}

/**************************************************
 *
 *               RAW IPC (aka "other")
 *
 **************************************************/

/*
 * vsvc_send_other
 *
 * Send a non-RPC message for an application layer service.  This routine
 * has the responsibility of converting unpacked data structures, used by
 * the code for the services, into packed data structures that can be
 * sent across the network.  "Other" data messages are always sent as
 * directed messages.
 */
void vsvc_send_other (
    vinesaddress *address,
    vsvc_info *service,
    vsvc_rtn_info *routine,
    uchar *data)
{
    vinesipcsocket *socket;
    paktype *other;
    uchar *c;

    if (address->family != VINES_ADDR_NATIVE)
	return;
    socket = vipc_create_conn(GETLONG(address->f.vip.nnet), address->f.vip.host, 0, 0, TRUE);
    if (!socket)
	return;
    other = vipc_getbuffer(routine->other_len);
    if (!other)
	return;

    if (routine->other_pack) {
	c = (*routine->other_pack)(other->session_start, data);
	other->datagramsize = (c - other->session_start);
    } else {
	errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
	       routine->name, "other pack");
	retbuffer(other);
	return;
    }

    if (*service->debug) {
	buginf("\nV%s: sending %s data to %#z", service->abbrev,
	       routine->name, socket->remote_net, socket->remote_host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Other", other->session_start,
		      other->datagramsize);
    }
    vipc_send_data(socket, other, address->f.vip.port, service->transient_port, VINES_HOPS);
}

/*
 * vsvc_unpack_other
 *
 * A non-RPC message has been received for an application layer
 * service. This routine has the responsibility of converting the packed
 * data structures received from the network into an unpacked data
 * structures that can be used by the services code.
 *
 * This routine is called from the routine that will process the data.
 * This is because IPC vectors directly to that routine, and not through
 * the RPC and SVC layers.
 */
void *vsvc_unpack_other (
    vinesipcsocket *socket,
    vsvc_info *service,
    vsvc_rtn_info *routine,
    uchar *packed_data,
    int packed_length)
{
    uchar *other_data, *c;
    int bytes_parsed;

    /*
     * Check, parse, and unpack all of the arguments to the data packet.
     */
    if (*service->debug) {
	buginf("\nV%s: received %s raw data from %#z", service->abbrev,
	       routine->name, socket->remote_net, socket->remote_host);
	if (*service->debug & VERBOSE_DEBUG)
	    vsvc_dump(service, "Other", packed_data, packed_length);  
    }
    other_data = NULL;
    if (routine->other_unpack) {
	other_data = malloc(routine->other_len);
	c = (*routine->other_unpack)(packed_data, other_data);
	bytes_parsed = (c - packed_data);
	if (bytes_parsed != packed_length) {
	    free(other_data);
	    if (*service->debug)
		buginf("\nV%s: %s other parse failed, %d bytes rcvd, %d bytes parsed",
		       service->abbrev, routine->name, packed_length,
		       bytes_parsed);
	    return(NULL);
	}
    } else {
	errmsg(&msgsym(FNNOTFOUND, VINES), service->name,
	       routine->name, "other unpack");
	return(NULL);
    }

    /*
     * Now that all that is finally done, return the unpacked data to
     * the routine that called us.
     */
    return(other_data);
}

/**********************************************************************
 *
 *			    MISCELLANEOUS
 *
 **********************************************************************/

/*
 * vsvc_start_services
 *
 * "Start" all services that the router knows about.  This routine
 * handles the things that are common to all services.  This mainly
 * consists of enqueing the service description block, allocating data
 * structures, starting timers, etc.  The last action of this routine is
 * to call a service specific start routine.
 */
void vsvc_start_services (void)
{
    vsvc_info *service;
    vipc_port_info *port;
    serverentry *s;

    if (vines_network == VINES_UNKNOWNNET)
	return;

    if (vines_server_tree == NULL)
	vines_server_tree = RBTreeCreate("Vines", "VN", "Server Table",
					 RBTreePrintKeyLong, &vst_debug);
    mgd_timer_start(&vsvc_tree_timer, VSVC_TREE_AGE_INTERVAL);
    s = vsvc_FindServer(vines_network);
    if (s == NULL)
	s = vsvc_CreateServer(vines_network);
    if (s == NULL)
	if (vst_debug)
	    buginf("\nVSVC: Cannot find or create entry for this router.");

    FOR_ALL_SERVICES(service) {
	/*
	 * Ports
	 */
	if (service->well_known_port)
	    vipc_create_port(service->well_known_port, service->name, TRUE, NULL);
	port = vipc_create_port(VIPC_PORT_UNKNOWN, service->name, TRUE, NULL);
	service->transient_port = port->number;
	
	/*
	 * Timers
	 */
	if (service->periodic && service->timer_period_init) {
	    mgd_timer_start(&service->timer, service->timer_period_init);
	}

	/*
	 * Other
	 */
	vsvc_create_svc_name(service);
	service->msgs_in = 0;
	service->msgs_out = 0;
	service->bytes_in = 0;
	service->bytes_out = 0;

	/*
	 * Service specific startup
	 */
	(*service->start)();
    }
}

/*
 * vsvc_stop_services
 *
 * "Stop" all services that the router knows about.  This routine first
 * calls a service specific stop routine.  It then handles the things
 * that are common to all services.
 */
void vsvc_stop_services (void)
{
    vsvc_info *service;

    FOR_ALL_SERVICES(service) {
	(*service->stop)();

	/*
	 * Ports
	 */
	vipc_destroy_port_number(service->well_known_port);
	vipc_destroy_port_number(service->transient_port);
	service->transient_port = 0;
	mgd_timer_stop(&service->timer);
    }

    /*
     * Destroy the entire server tree.  No need to specifically delete
     * the entry for this server.
     */
    if (vines_server_tree)
	vines_server_tree = RBTreeDestroy(vines_server_tree, &vst_debug);
    mgd_timer_stop(&vsvc_tree_timer);
}

/*
 * vsvc_report_ipc_error
 *
 * Pass a lower level error message along to any services that might be
 * interested.  This should eventually be a registry, or a vector built
 * from the service description.
 */
void vsvc_report_error (vinesipcsocket *socket)
{
    vnsm_report_error(socket);
}

/*
 * vsvc_new_neighbor
 *
 * Pass the routing code's notification of a new neighbor along to any
 * services that might be interested.  This should eventually be a
 * registry, or a vector built from the service description.
 */
void vsvc_new_neighbor (ulong net, vinesidbtype *idb)
{
    vst_new_neighbor(net, idb);
    vvs_new_neighbor(net, idb);
}

/*
 * vsvc_action_name
 *
 * Given a routine action, return the textual name for that action.
 */
static char *vsvc_action_name (vsvc_rcv_action action)
{
    switch (action) {
      case VSVC_ACCEPT_DATA:	return("Accept");
      case VSVC_ACCEPT_NODATA:	return("Accept");
      case VSVC_REJECT:		return("Reject");
      case VSVC_IGNORE:		return("Ignore");
      case VSVC_PROXY:		return("Proxy");
    }
    return("unknown");
}

/*
 * vsvc_display_one_proc
 *
 * Formatting routine to display a single service routine and its disposition.
 */
static void vsvc_display_one_proc (
    int number,
    vsvc_rcv_action action,
    int reject_code,
    char *name)
{
    char reject_string[20], number_string[20];

    if (number != -1) {
	sprintf(number_string, "%d", number);
    } else {
	sprintf(number_string, "-");
    }
    if (reject_code != 0) {
	sprintf(reject_string, "%d", reject_code);
    } else {
	reject_string[0] = '\0';
    }
    printf("\n     %-5s  %-6s  %-5s  %30s", number_string,
	   vsvc_action_name(action), reject_string, name);
}
/*
 * vsvc_display_procs
 *
 * Display a list of all routines that are known for a specific
 * service.
 */
static void vsvc_display_procs (vsvc_info *svc)
{
    vsvc_rtn_info *rtn;
    int number;

    automore_enable(NULL);
    printf("\n%s Service", svc->name);
    printf("\n  Routine Information:");

    automore_header("    Number  Action   Code  Name");
    vsvc_display_one_proc(-1, svc->routine_default, svc->reject_code, "Default");
    for (number = 0; number < svc->routine_count; number++) {
	rtn = &svc->routines[number];
	vsvc_display_one_proc(rtn->number, rtn->rcv_query_action,
			      rtn->rcv_reject_code, rtn->name);
    }
    automore_disable();
}

/*
 * vines_display_services
 *
 * Display information about all of the vines application level services.
 */
void vsvc_show_services (parseinfo *csb)
{
    vsvc_info *service;
    char text[20];

    automore_enable(NULL);
     switch (GETOBJ(int, 1)) {
      case SH_V_S_TREE:
	printf("\n\nServer Tree Information:");
	RBTreeStats(vines_server_tree,"  Servers");
	vst_display_group_tree();
	break;

      case SH_V_S_DATA:
	if (GETOBJ(int, 2) == 0) {
	    vsvc_service_overview();
	    break;
	}
	service = vsvc_find_service(GETOBJ(int,2));
	if (!service) {
	    printf("*** Service %d not found ***\n");
	    break;
	}
	if (GETOBJ(int, 3)) {
	    vsvc_display_procs(service);
	    break;
	}
	printf("\n\n%s Service:", service->name);
	if (mgd_timer_running(&service->timer)) {
	    sprint_ticks_in_dhms(text, mgd_timer_left_sleeping(&service->timer));
	    printf("\n  Next wakeup in %s.", text);
	} else {
	    printf("\n  Periodic timer not running.");
	}
	(*service->display)();
    }
    automore_disable();
}

/*
 * vines_process_services
 *
 * The application level timer has expired.  Check each of the services
 * to see if they need to be run at this time.  After calling the
 * service, reset the application level timer, but only if the service
 * hasn't already done this.  Some services, like Streettalk, need to
 * awaken at odd intervals to do things like delayed packet forwarding,
 * etc.
 */
void vsvc_process_service (mgd_timer *expired_timer)
{
    vsvc_info *service;

    service = mgd_timer_context(expired_timer);
    if (service->periodic == NULL) {
	mgd_timer_stop(expired_timer);
	return;
    }

    if (service->timer_period_normal) {
	mgd_timer_update(expired_timer, service->timer_period_normal);
    } else {
	mgd_timer_stop(expired_timer);
    }
    (*service->periodic)();
}

void vsvc_process_tree (void)
{
    vines_server_tree_ager(FALSE);
    mgd_timer_update(&vsvc_tree_timer, VSVC_TREE_AGE_INTERVAL);
}

/*
 * vsvc_hostname_changed
 *
 * The hostname changes.  Change the names of all the services to use the
 * new hostname.
 */
void vsvc_hostname_changed (boolean non_default)
{
    vsvc_info *service;

    FOR_ALL_SERVICES(service) {
	vsvc_create_svc_name(service);
    }
    vst_hostname_changed();
}

/*
 * vines_init_services
 *
 * Initialize all application layer information.
 */
void vines_init_services (void)
{
    reg_add_hostname_changed(vsvc_hostname_changed, "vsvc_hostname_changed");

    queue_init(&vsvc_queue, 0);
    mgd_timer_init_parent(&vines_svc_timer, &vines_bkgnd_timer);
    mgd_timer_init_leaf(&vsvc_tree_timer, &vines_svc_timer, VT_SVC_TREE, NULL,
			FALSE);
    vfs_init();
    vnsm_init();
    vss_init();
    vst_init();
    vvs_init();
}
