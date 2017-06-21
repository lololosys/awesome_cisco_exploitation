/* $Id: vsvc_vs.c,v 3.3.20.1 1996/03/18 22:33:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/vines/vsvc_vs.c,v $
 *------------------------------------------------------------------
 * vsvc_vs.c - Support for the `VINES Security' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_vs.c,v $
 * Revision 3.3.20.1  1996/03/18  22:33:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:05:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:48:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:11:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:00:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/23  18:49:03  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/20  20:11:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:16:25  hampton
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
#include "config.h"
#include "parser.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "parser_defs_vines.h"

#include "vsvc_vs.h"
#include "vsvc_vs.rpch"
#include "vsvc_vs.rpcc"
#include "vsvc_ss.h"
#include "vsvc_st.h"
#include <ciscolib.h>


/**********************************************************************
 *
 *			Vines Security Service
 *
 **********************************************************************/

/**************************************************
 *
 *		VS - Helper Routines
 *
 **************************************************/

/*
 * v_port_to_path
 *
 * Given a vines 16 byte address (aka a service port), look up the
 * corresponding path structure for that address.
 *
 * This code assumes that the neighbor entry for the client has only one
 * path specified.  This assumption is valid, given the way that the
 * vines client code works.  Should Banyan ever change the client code to
 * support multiple interfaces, this routine will have to change.
 */
static inline vinespathtype *v_port_to_path (
    vinesaddress *port,
    ulong *net,
    ushort *host)
{
    vinesneighbortype *neighbor;

    *net = GETLONG(port->f.vip.nnet);
    *host = port->f.vip.host;
    neighbor = vneigh_FindEntry(*net, *host);
    return(neighbor ? neighbor->best_path : NULL);    
}


/**************************************************
 *
 *		VS - Get Neighbor (15)
 *
 **************************************************/

/*
 * vvs_get_neighbor_rcv_call
 *
 * This routine is invoked when a user with login location restrictions
 * attempts to log into the vines network.
 *
 * This code assumes that the neighbor entry for the client has only one
 * path specified.  This assumption is valid, given the way that the
 * vines client code works.  Should Banyan ever change the client code to
 * support multiple interfaces, this routine will have to change.
 */
ushort vvs_get_neighbor_rcv_call (
    vinesipcsocket *socket,
    vvs_get_neighbor_query *query,
    vvs_get_neighbor_reply *reply)
{
    vvs_get_neighbor_entry *entry;
    vinespathtype *path;
    vinesidbtype *idb;
    char address[20];
    idbtype *swidb;
    ushort host;
    ulong net;
    int i;

    /*
     * Gather the local information
     */
    path = v_port_to_path(&query->client_port, &net, &host);
    if (path == NULL) {
	if (vvs_debug)
	    buginf("\nVVS: client %#z is unknown", net, host);
	return(VVS_ERROR_NO_SUCH_CLIENT);
    }
    idb = path->idb;
    swidb = idb->phys_idb;
    memset(address, 0, 16);
    if (idb_is_ieeelike(swidb)) {
	uchar *mac = path->dlc_addr.addr;
	sprintf(address, "%-02x%-02x%-02x%-02x%-02x%-02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    hostname = vsvc_hostname();

    /*
     * Now scan the login location list to see if there is a match.
     * This isn't necessary, but it does save bandwidth if we return a
     * failure here.
     */
    for (i = 0; i < query->entry_count; i++) {
	entry = &query->entry[i];
	if (vvs_debug & VERBOSE_DEBUG) {
	    char sl[10];
	    if (entry->slot_no || entry->line_no)
		sprintf(sl, "%d-%d", entry->slot_no, entry->line_no);
	    else
		sprintf(sl, "*");
	    buginf("\nVVS:   checking against %s,%s,%s",
		   entry->server_name, sl,
		   entry->address ? entry->address : "*");
	}
	if (strcasecmp(entry->server_name, hostname) != 0)
	    continue;
	if (entry->slot_no && (entry->slot_no != idb->svc_slot))
	    continue;
	if (entry->line_no && (entry->line_no != idb->svc_line))
	    continue;
	if (strlen(entry->address) && (strcasecmp(entry->address, address) != 0))
	    continue;

	strcpy(reply->server_name, vsvc_hostname());
	if (swidb->description)
	    vsvc_insert_description(reply->network_name, swidb->description);
	reply->slot_no = idb->svc_slot;
	reply->line_no = idb->svc_line;
	strcpy(reply->mac_address, address);

	if (vvs_debug)
	    buginf("\nVVS: reporting on client %#z", net, host);
	return(VSVC_NO_ERROR);
    }
    return(VVS_ERROR_NO_SUCH_CLIENT);
}


/**************************************************
 *
 *	    VS - Check Neighbor (16)
 *
 **************************************************/

/*
 * vvs_check_neighbor_rcv_call
 *
 * This routine is invoked when someone attempts to log in from a client.
 *
 * This code assumes that the neighbor entry for the client has only one
 * path specified.  This assumption is valid, given the way that the
 * vines client code works.  Should Banyan ever change the client code to
 * support multiple interfaces, this routine will have to change.
 */
ushort vvs_check_neighbor_rcv_call (
    vinesipcsocket *socket,
    vvs_check_neighbor_query *query,
    vvs_check_neighbor_reply *reply)
{
    vinespathtype *path;
    vinesidbtype *idb;
    idbtype *swidb;
    ulong net;
    ushort host;

    path = v_port_to_path(&query->client_port, &net, &host);
    if (path == NULL) {
	if (vvs_debug)
	    buginf("\nVVS: client %#z is unknown", net, host);
	return(VVS_ERROR_NO_SUCH_CLIENT);
    }

    idb = path->idb;
    swidb = idb->phys_idb;
    strcpy(reply->server_name, vsvc_hostname());
    if (swidb->description)
	vsvc_insert_description(reply->network_name, swidb->description);
    reply->slot_no = idb->svc_slot;
    reply->line_no = idb->svc_line;
    memset(reply->mac_address, 0, 16);
    if (idb_is_ieeelike(swidb)) {
	uchar *mac = path->dlc_addr.addr;
	sprintf(reply->mac_address, "%-02x%-02x%-02x%-02x%-02x%-02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    reply->user_id = 100;
    if (vvs_debug)
	buginf("\nVVS: reporting on client %#z", net, host);
    return(VSVC_NO_ERROR);
}


/**************************************************
 *
 *	    VS - Validate Nonce (28)
 *
 **************************************************/

/*
 * vvs_validate_rcv_call
 */
ushort vvs_validate_rcv_call (
    vinesipcsocket *socket,
    vvs_validate_query *query,
    vvs_validate_reply *reply)
{
    sstrncpy(reply->username, "Admin@Dtp-2@Servers", VST_TOTALLEN);
    reply->version = 55255;
    reply->unknown1 = 0x00000001;
    reply->unknown2 = 0x0001;
    reply->unknown3 = 0x00000004;
    reply->unknown4 = 0xFFFFFFFF;
    reply->unknown5 = 0x0003;
    reply->unknown6 = 0xFE310100;
    return(VSVC_NO_ERROR);
/*    return(VVS_ERROR_BAD_NONCE); */
}

/*
 * vvs_validate_send_call
 */
void vvs_validate_send_call (
    vinesipcsocket *client_socket,
    vinesnonce *client_nonce)
{
    vvs_validate_query *query;
    vinesipcsocket *server_socket;
    vinesaddress address;
    ulong net;
    ushort host;

    query = malloc(sizeof(vvs_validate_query));
    if (query == NULL) {
	return;
    }

    vsvc_break_nonce(client_nonce, &net, &host);
    server_socket = vipc_create_conn(net, host, 0, 0, TRUE);
    if (!server_socket) {
	free(query);
	buginf(vsvc_nomem);
	return;
    }

    bcopy(client_nonce, query->client_nonce, VINES_NONCE_LEN);
    vsvc_socket_to_address(&address, client_socket);
    bcopy(&address, &query->client_port, sizeof(vinesaddress));
    query->unknown1 = 0x0001;
    query->unknown2 = 0x0065;
    query->unknown3[0] = '\0';
    vvs_validate_send_call_(server_socket, query);
    free(query);
}

/*
 * vvs_validate_rcv_reply
 */
void vvs_validate_rcv_reply (
    vinesipcsocket *socket,
    vvs_validate_reply *reply)
{
    vst_user_validated(0, reply->username);
}

/*
 * vvs_validate_rcv_abort
 */
void vvs_validate_rcv_abort (
    vinesipcsocket *socket,
    ushort reason)
{
    vst_user_validated(reason, NULL);
}


/**************************************************
 *
 *            VS - Search / ReturnAddress
 *
 **************************************************/

/*
 * vvs_rcv_search
 *
 * A NetRPC search message has been received on the VINES Security port.
 */
void vvs_rcv_search (
    vinesipcsocket *socket,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3)
{
    /* Do nothing */
}

/*
 * vvs_rcv_returnaddress
 *
 * A NetRPC returnaddress message has been received on the VINES Security
 * port.  
 */
void vvs_rcv_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /* Do nothing */
}


/**************************************************
 *
 *	VS - Routines Common Across Services
 *
 **************************************************/

void vvs_new_neighbor (ulong net, vinesidbtype *idb)
{
}

/*
 * vvs_periodic
 *
 * Perform any periodic processing for the VINES Security service.  This
 * routine is called once every thirty seconds.
 */
void vvs_periodic (void)
{
}

/*
 * vvs_init
 *
 * Boot time initializion of information that is maintained by the VINES
 * Security Service.  This routine will only ever be called once by the
 * router.
 */
void vvs_init (void)
{
    vsvc_install_svc(&vsvc_vs_info);
}

/*
 * vvs_start
 *
 * Run time initialize of information that is maintained by the VINES
 * Security Service.  This routine will be called each time the 'vines
 * routing' command is issued.
 */
void vvs_start (void)
{
}

/*
 * vvs_stop
 *
 * Purge all information that is maintained by the VINES Security
 * Service.  This routine will be called each time the 'no vines routing'
 * command is issued.
 */
void vvs_stop (void)
{
}

/*
 * vvs_display
 *
 * Display VS service specific information.
 */
void vvs_display (void)
{
}
