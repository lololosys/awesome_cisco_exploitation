/* $Id: vines_arp.c,v 3.4.54.3 1996/08/07 09:03:32 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_arp.c,v $
 *------------------------------------------------------------------
 * vines_arp.c - support for assigning vines addresses
 * 
 * November 1992, David R. Hampton
 * from a module dated February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_arp.c,v $
 * Revision 3.4.54.3  1996/08/07  09:03:32  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.54.2  1996/06/17  23:38:48  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.54.1  1996/03/18  22:31:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/07  11:04:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  21:46:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/01  21:38:52  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/17  17:57:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:44:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "vines_private.h"
#include "mgd_timers.h"


/**********************************************************************
 *
 * ARP State Maintenance
 *
 **********************************************************************/
mgd_timer varp_timer;

static char *const varp_state_names[VARP_LAST_STATE+1] = {
    "off", "initializing", "learning", "service"};

char *varp_state_name (varp_states state)
{
    if (state > VARP_LAST_STATE)
	return("illegal");
    return(varp_state_names[state]);
}

void varp_new_state (vinesidbtype *idb, varp_states state)
{
    if (!idb)
	return;

    if (state > VARP_LAST_STATE)
	state = VARP_OFF;
    idb->arp_state = state;
    if (state == VARP_LEARNING) {
	mgd_timer_start(&idb->arp_timer, VARP_LEARNING_TIME);
    } else {
	mgd_timer_stop(&idb->arp_timer);
    }

    if (vinesarp_debug)
	buginf("\nVINES: ARP on %s changing to %s state",
	       idb->phys_idb->namestring, varp_state_name(state));
}

void varp_timer_expired (mgd_timer *expired_timer)
{
    vinesidbtype *idb;

    mgd_timer_stop(expired_timer);
    idb = mgd_timer_context(expired_timer);
    varp_new_state(idb, VARP_SERVICE);
}

void varp_idb_init (vinesidbtype *idb)
{
    mgd_timer_init_leaf(&idb->arp_timer, &varp_timer, VT_ARP, idb, FALSE);
    idb->arp_enabled = VARP_DYNAMIC;
    varp_new_state(idb, VARP_INITIALIZING);
}

void varp_init (void)
{
    mgd_timer_init_parent(&varp_timer, &vines_bkgnd_timer);
}


/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * varp_display_interface
 *
 * Print all ARP information that is maintained on a per interface basis.
 */
void varp_display_interface (vinesidbtype *idb)
{
    char *word;

    switch (idb->arp_enabled) {
      case VARP_ALWAYS:  word = "enabled";  break;
      case VARP_DYNAMIC: word = "dynamic";  break;
      case VARP_NEVER:   
      default:		 word = "disabled"; break;
    }
    printf("\n  ARP processing is %s", word);
    if (idb->arp_enabled == VARP_DYNAMIC) {
	printf(", currently %sactive", vines_servers_present(idb) ? "in" : "");
    }

    if (idb->arp_enabled == VARP_NEVER)
	return;
    printf(", state is %s", varp_state_name(idb->arp_state));
    if (idb->arp_state == VARP_LEARNING)
	printf("\n    Learning ends in %d seconds.",
	       mgd_timer_left_sleeping(&idb->arp_timer) / ONESEC);
}

/*
 * varp_display_timer
 *
 * Display all the timer queues maintained by ARP.
 */
void varp_display_timer (void)
{
    char temp[40];

    printf("\n    ARP: %s", vtimer_pretty_print(&varp_timer, "off", temp));
}

/*
 * varp_decode_packet
 *
 * Print out the RTP information in a packet.  This set of routines does
 * not use any of the pointers in the packet header other than the
 * network start pointer, so it may be called at any point in the
 * processing of a packet.
 */
uchar *varp_decode_packet (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    varp_header *arp;
    vsarp_header *sarp;
    boolean valid;

    arp = (varp_header *)data;
    sarp = (vsarp_header *)data;
    if (sarp->version == VARP_VER_NSARP) {
	valid = (arp->op == VARP_ASS_RESP);
	(*fn)("\n  ARP op %d, %saddress %z%s", arp->op,
	      valid ? "(" : "", arp->net, arp->host, valid ? ")" : "");
    } else {
	valid = (sarp->op == VARP_ASS_RESP);
	(*fn)("\n  ARP op %d, %saddress %z, sequence %d, metric %d%s",
	      sarp->op, valid ? "(" : "", sarp->net, sarp->host,
	      sarp->sequence, sarp->metric, valid ? ")" : "");
    }

    *limit = 0;
    return(NULL);
}

/**********************************************************************
 *
 *			       COMMANDS
 *
 **********************************************************************/

/*
 * varp_command
 *
 * Process VINES interface keywords
 */
void varp_command (parseinfo *csb)
{
    idbtype *phys_idb;
    vinesidbtype *idb;

    phys_idb = csb->interface;
    idb = vines_getidb(phys_idb);
    if (!vines_running || !idb)
	return;

    if (csb->nvgen) {
	if (!vines_running)
	    return;
	switch (idb->arp_enabled) {
	  case VARP_NEVER:
	    nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	  case VARP_ALWAYS:
	    nv_write(TRUE, "%s", csb->nv_command);
	    break;
	  case VARP_DYNAMIC:
	    break;
	}
	return;
    }
    
    if (csb->set_to_default)
	idb->arp_enabled = VARP_DYNAMIC;
    else if (!csb->sense) {
	if (GETOBJ(int,1)) {
	    idb->arp_enabled = VARP_ALWAYS;
	} else  {
	    idb->arp_enabled = VARP_NEVER;
	}
    } else {
	if (GETOBJ(int,1)) {
	    idb->arp_enabled = VARP_DYNAMIC;
	} else  {
	    idb->arp_enabled = VARP_ALWAYS;
	}
    }
}
/*
 * varp_notify
 *
 * Called when vines is enabled or disabled on an interface.  This is the
 * trigger for starting or stopping ARP on an interface.  This function
 * will also be called if the metric on an interface is changed, so make
 * sure to check for that case.
 */
void varp_notify (vinesidbtype *idb)
{
    if (idb->vines_metric) {
	if (idb->arp_state != VARP_SERVICE)
	    varp_new_state(idb, VARP_LEARNING);
    } else {
	varp_new_state(idb, VARP_OFF);
    }
}


/**********************************************************************
 *
 *			      Vines ARP
 *	       Address Resolution (Assignment) Protocol
 *
 * This section contains all the routines to support the Vines Client
 * address assignment protocol (called ARP for some reason).  This
 * protocol allows a newly powered on client to discover his network
 * level address.
 *
 **********************************************************************/

/*
 * varp_getbuffer
 */
static inline paktype *varp_getbuffer (void)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(varp_header));
    if (pak) {
	pak->transport_start = pak->network_start + sizeof(vinesiptype);
	pak->datagramsize = sizeof(varp_header);
    }
    return(pak);
}

/*
 * vsarp_getbuffer
 */
static inline paktype *vsarp_getbuffer (void)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(vsarp_header));
    if (pak) {
	pak->transport_start = pak->network_start + sizeof(vinesiptype);
	pak->datagramsize = sizeof(vsarp_header);
    }
    return(pak);
}

/*
 * vines_client_inc
 *
 * Increment the client number and check for the wrap condition.
 */
static void vines_client_inc (void)
{
    vines_client++;
    if (vines_client > VINES_LAST_CLIENT) {
	vines_client = VINES_1ST_CLIENT;
	if (vinesarp_debug)
	    buginf("\nVINES: client number wrapped to %0x",
		   VINES_1ST_CLIENT);
    }
}

/*
 * vines_find_client_address
 *
 * Find a client address that is not currently in use.  Note that there
 * is an inherent ninety second window in this protocol when the router
 * first comes up.  Existing clients (from before the reboot) may be
 * using a address but the router has not yet heard a hello message from
 * them.  There is no way for the router to detect this condition and
 * prevent a duplicate assignment.  After the router has been up for
 * ninety seconds, it should have heard from all existing clients and
 * know not to reuse their addresses.
 */
static boolean vines_find_client_address (void)
{
    ushort starting_client;

    starting_client = vines_client;
    while (vneigh_FindEntry(vines_network, vines_client)) {
	if (vinesarp_debug)
	    buginf("\nVINES: client number %#x in use.", vines_client);
	vines_client_inc();
	if (vines_client == starting_client) {
	    if (vinesarp_debug)
		buginf("\nVINES: client number space is full.");
	    return(FALSE);
	}
    }
    return(TRUE);
}

/**********************************************************************
 *
 * ARP frame processing
 *
 **********************************************************************/

static const char varp_illegal[]= "\n%s: received illegal ARP type (%d) from %e on %s.";
static const char varp_received[] = "\n%s: received ARP type %d from %e on %s.";
static const char varp_nobuffers[]= "\n%s: discarding ARP from %e on %s (no packets).";
static const char varp_notenabled[]= "\n%s: discarding ARP from %e on %s (not enabled).";
static const char varp_nonieee[]  = "\n%s: discarding ARP on %s (non IEEE interface).";
static const char varp_learning[] = "\n%s: discarding ARP from %e on %s (not in service).";
static const char varp_send[]     = "\n%s: sending ARP type %d to %e";
static const char varp_assign[]   = " assigning address %x:%x";
static const char varp_assign2[]  = " assigning address %x:%x, sequence %x, metric %x";

/*
 * vines_process_arp
 *
 * Build a response to the incoming v0 arp frame.  Return the length of
 * the response, or zero if there is no response.
 */
static paktype *vines_process_nsarp (paktype *pak, uchar *address)
{
    paktype *newpak;
    varp_header *arp, *response;
    idbtype *phys_idb;
    vinesidbtype *idb;

    phys_idb = pak->if_input;
    idb = vines_getidb(phys_idb);
    arp = (varp_header *)pak->transport_start;

    if (!(idb_is_ieeelike(phys_idb))) {
	idb->rx_arpv0_discarded++;
	if (vinesarp_debug)
	    buginf(varp_nonieee, "VNSARP", phys_idb->namestring);
	return(NULL);
    }

    if ((idb->arp_enabled == VARP_NEVER) ||
	((idb->arp_enabled == VARP_DYNAMIC) && vines_servers_present(idb))) {
	idb->rx_arpv0_discarded++;
	if (vinesarp_debug)
	    buginf(varp_notenabled, "VNSARP", address, phys_idb->namestring);
	return(NULL);
    }

    if (idb->arp_state != VARP_SERVICE) {
	idb->rx_arpv0_discarded++;
	if (vinesarp_debug)
	    buginf(varp_learning, "VNSARP", address, phys_idb->namestring);
	return(NULL);
    }

    if (arp->op >= VARP_LAST) {
	idb->rx_arpv0_illegal++;
	if (vinesarp_debug)
	    buginf(varp_illegal, "VNSARP", arp->op, address, phys_idb->namestring);
	return(NULL);
    }

    idb->rx_arpv0_type[arp->op]++;
    if (vinesarp_debug)
	buginf(varp_received, "VNSARP", arp->op, address, phys_idb->namestring);
    if ((arp->op != VARP_SVC_REQ) && (arp->op != VARP_ASS_REQ))
	return(NULL);

    newpak = varp_getbuffer();
    if (!newpak) {
	if (vinesarp_debug)
	    buginf(varp_nobuffers, "VNSARP", address, phys_idb->namestring);
	return(NULL);
    }

    response = (varp_header *)newpak->transport_start;
    switch (arp->op) {
      case VARP_SVC_REQ:
	response->op = VARP_SVC_RESP;
	break;

      case VARP_ASS_REQ:
	if (!vines_find_client_address()) {
	    retbuffer(newpak);
	    return(NULL);
	}
	response->op = VARP_ASS_RESP;
	response->net = vines_network;
	response->host = vines_client;
	vines_client_inc();
	break;
    }

    if (vinesarp_debug) {
	buginf(varp_send, "VNSARP", response->op, address);
	if (response->op == VARP_ASS_RESP)
	    buginf(varp_assign, response->net, response->host);
    }
    idb->tx_arpv0_type[response->op]++;
    return(newpak);
}

/*
 * vines_process_sarp
 *
 * Build a response to the incoming v1 arp frame.  Return the length of
 * the response, or zero if there is no response.
 */
static paktype *vines_process_sarp (paktype *pak, uchar *address)
{
    paktype *newpak;
    vsarp_header *arp, *response;
    vinespathtype path;
    idbtype *phys_idb;
    vinesidbtype *idb;

    phys_idb = pak->if_input;
    idb = vines_getidb(phys_idb);
    arp = (vsarp_header *)pak->transport_start;

    if (!(idb_is_ieeelike(phys_idb))) {
	idb->rx_arpv1_discarded++;
	if (vinesarp_debug)
	    buginf(varp_nonieee, "VSARP", phys_idb->namestring);
	return(NULL);
    }

    if (!vines_vsrtp_enabled(idb)) {
	if (vinesarp_debug)
	    buginf("\nVSARP: discarding ARP from %e on %s (not supported).",
		   address, phys_idb->namestring);
	return(NULL);
    }

    if ((idb->arp_enabled == VARP_NEVER) ||
	((idb->arp_enabled == VARP_DYNAMIC) && vines_servers_present(idb))) {
	idb->rx_arpv1_discarded++;
	if (vinesarp_debug)
	    buginf(varp_notenabled, "VSARP", address, phys_idb->namestring);
	return(NULL);
    }

    if (idb->arp_state != VARP_SERVICE) {
	idb->rx_arpv1_discarded++;
	if (vinesarp_debug)
	    buginf(varp_learning, "VSARP", address, phys_idb->namestring);
	return(NULL);
    }

    if (arp->op >= VARP_LAST) {
	idb->rx_arpv1_illegal++;
	if (vinesarp_debug)
	    buginf(varp_illegal, "VSARP", arp->op, path.dlc_addr.addr,
		   phys_idb->namestring);
	return(NULL);
    }

    idb->rx_arpv1_type[arp->op]++;
    if (vinesarp_debug)
	buginf(varp_received, "VSARP", arp->op, address, phys_idb->namestring);
    if ((arp->op != VARP_SVC_REQ) && (arp->op != VARP_ASS_REQ))
	return(NULL);

    newpak = vsarp_getbuffer();
    if (!newpak) {
	if (vinesarp_debug)
	    buginf(varp_nobuffers, "VSARP", address, phys_idb->namestring);
	return(NULL);
    }

    response = (vsarp_header *)newpak->transport_start;
    switch (arp->op) {
      case VARP_SVC_REQ:
	response->op = VARP_SVC_RESP;
	break;

      case VARP_ASS_REQ:
	if (!vines_find_client_address()) {
	    retbuffer(newpak);
	    return(NULL);
	}
	response->op = VARP_ASS_RESP;
	response->net = vines_network;
	response->host = vines_client;
	vines_client_inc();
	break;
    }
    response->version = VARP_VER_SARP;
    response->sequence = vines_sequence;
    response->metric = vmetric_to_srtp(vines_compute_metric(pak));

    if (vinesarp_debug) {
	buginf(varp_send, "VSARP", response->op, address);
	if (response->op == VARP_ASS_RESP)
	    buginf(varp_assign2, response->net, response->host,
		   response->sequence, response->metric);
    }
    idb->tx_arpv1_type[response->op]++;
    return(newpak);
}

/*
 * vines_arp
 *
 * Vines Address Resolution Protocol.  This is address resolution in it's
 * most basic sense.  "Hi!  I'm new here.  What should my address be?"
 *
 * This is a two step process.  The initializing client first sends a
 * query asking for all network servers to respond, and accepts all
 * responses.  It then sends an address assignment request to the first
 * server to answer it's previous query.  The server then assigns an
 * address, and sends an address assignment response back to the client.
 * The Banyan specification claims that a state should be kept for this
 * entire sequence.  Maintaining state didn't seem necessary, so this
 * implementation is stateless.
 */
void vines_arp (paktype *pak)
{
    paktype *newpak;
    vsarp_header *arp;
    vinesidbtype *idb;
    vinespathtype path;
    vinesneighbortype neigh;

    memset(&neigh, 0, sizeof(vinesneighbortype));
    memset(&path, 0, sizeof(vinespathtype));
    (*pak->if_input->hwptr->extract_snpa)(pak, &path.dlc_addr, SOURCE);
    arp = (vsarp_header *)pak->transport_start;
    
    switch (arp->version) {
      case VARP_VER_NSARP:
	newpak = vines_process_nsarp(pak, path.dlc_addr.addr);
	break;
	
      case VARP_VER_SARP:
	newpak = vines_process_sarp(pak, path.dlc_addr.addr);
	break;

      default:
	newpak = NULL;
	break;
    }
    if (!newpak)
	return;

    path.idb = vines_getidb(pak->if_input);
    path.next_best = &path;
    path.enctype = pak->enctype;
    neigh.net = VINES_ALLNETS;
    neigh.host = VINES_ALLHOSTS;
    neigh.best_path = &path;
    neigh.primary_path = &path;
    neigh.available_paths = &path;

    idb = vip_send_specific(newpak, &neigh, &path, VINES_ARP, 0);
    if (idb)
	idb->tx_unicast++;
}
