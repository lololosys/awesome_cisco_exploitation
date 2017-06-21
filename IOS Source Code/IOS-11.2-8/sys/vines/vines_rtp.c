/* $Id: vines_rtp.c,v 3.9.18.6 1996/08/07 09:03:54 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_rtp.c,v $
 *------------------------------------------------------------------
 *	       VINES ROUTING UPDATE PROTOCOL, Version 0
 *		       a.k.a. Non-Sequenced RTP
 *
 * February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_rtp.c,v $
 * Revision 3.9.18.6  1996/08/07  09:03:54  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.18.5  1996/07/23  13:30:28  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.18.4  1996/07/17  00:02:53  ehk
 * CSCdi59059:  Out of order packets when using vines route-cache
 * Branch: California_branch
 * invalidate a fast cache entry before passing to process switching
 * a packet with metric notification bit set and relearn a new cache
 * to maintain original packet ordering.
 *
 * Revision 3.9.18.3  1996/06/16  21:20:29  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.9.18.2  1996/05/11  16:17:41  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.9.18.1  1996/03/18  22:32:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  02:04:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  11:05:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:46:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/08  20:14:08  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/05  22:13:24  slin
 * CSCdi48180: Small buffer memory leak on downed serial interface with
 *             vines
 *
 * Revision 3.7  1996/01/22  07:44:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/02  00:15:13  slin
 * CSCdi44038:  Sending updates to networks that are unreachable
 * - Help Thy Neighbor only for WAN paths
 *
 * Revision 3.5  1995/12/01  21:39:18  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.4  1995/11/21  01:20:09  slin
 * CSCdi42431:  clear vines neighbor halts system
 * - Don't voluntarily suspend during processing of routing update
 *
 * Revision 3.3  1995/11/17  17:58:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/19  23:41:22  slin
 * CSCdi41922:  Vines Router process suspends too frequently
 *
 * Revision 2.7  1995/09/09  00:54:07  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.6  1995/08/22  16:54:08  slin
 * CSCdi38016:  Vines redirect sent to MAC broadcast and network broadcast
 * - Send redirect to last forwarding node
 *
 * Revision 2.5  1995/08/21  17:12:11  slin
 * CSCdi39054:  vines single-route sets wrong metric for routes
 * - Set route's metric to advertised metric, not actual metric
 *
 * Revision 2.4  1995/08/12  00:18:46  slin
 * CSCdi38606:  Vines f/s cache not invalidated under certain circumstances
 *
 * Revision 2.3  1995/08/07  05:44:46  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/07/07  20:40:30  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.1  1995/06/07 23:14:59  hampton
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
#include "address.h"
#include "../h/ieee.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/rif_util.h"
#include "vines_private.h"
#include "vinesfast.h"

/*
 * RTP Timer queues
 */
mgd_timer vrtp_rc_timer;		/* Roll Call for V0 WAN devices */

boolean waste_bandwidth_because_banyan_whines;

/*
 * Strings for multiple routines
 */
const char vsrtp_no_packet_msg[] = "\nVSRTP: No packet available for %s.";
const char vrtp_mac_rif_msg[] = "\nVRTP: Mac %e, Rif %s";
const char vines_network_metric_msg[] = "\n    network %#16Z metric %x (%s)";
const char vines_network_metric_seq_flags_msg[] =
    "\n    network %#16Z metric %04x, seq %08x, flags %02x";
const char vines_seconds_msg[] = "%s seconds";

/*
 * External function declarations
 */

/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vrtp_init
 *
 * Perform any initialization that is unique to RTP v0.
 */
void vrtp_init (void)
{
    mgd_timer_init_parent(&vrtp_rc_timer, &vines_timer);
}


/**********************************************************************
 *
 *		     INTERFACE TO VINES IP LAYER
 *
 **********************************************************************/

STATIC char const vrtp_upd_msg[] = "\nVRTP: received %s from %#z on %s";
STATIC char *const vrtp_names[] = {"0", "request", "2", "3", "update", "response", "redirect"};

/*
 * vrtp_send_pak
 *
 * Build the RTP header for a packet, and then send it to the VIP layer.
 */
void vrtp_send_pak (
    paktype *pak,
    vinespathtype *path,
    int opcode)
{
    vrtp_type *rtp;
    vinesidbtype *idb;

    if (reg_invoke_snapshot_update_denied(pak->if_output, FALSE)) {
	datagram_done(pak);
	return;
    }
    reg_invoke_snapshot_notice_update_pak(pak, pak->if_output);
    idb = vines_getidb(pak->if_output);
    rtp = (vrtp_type *)pak->transport_start;
    rtp->op = opcode;
    rtp->node = VRTP_NODE_ROUTER;
    rtp->controller = VRTP_CONT_MULTI;
    rtp->machine = VRTP_MACH_FAST;
    if (vines_vsrtp_enabled(idb))
	rtp->machine |= VRTP_COMPATABILITY;

    if (path) {
	if (do_vines_rtp_debug(vines_network, vines_host))
	    buginf("\nVRTP: sent %s to %#z on %s", vrtp_names[opcode],
		   path->neighbor->net, path->neighbor->host,
		   path->idb->phys_idb->namestring);
	vip_rtp_send_specific(pak, path, VINES_NOFLAGS);
	return;
    }

    idb = vines_getidb(pak->if_output);
    if (do_vines_rtp_debug(vines_network, vines_host))
	buginf("\nVRTP: sent %s to %#z on %s", vrtp_names[opcode],
	       VINES_ALLNETS, VINES_ALLHOSTS, idb->phys_idb->namestring);
    vip_send_broadcast(pak, idb, NULL, VINES_RTP, 0, 0);
    idb->tx_rtpv0_bcast++;
    vroute_bcast_out++;
}


/**********************************************************************
 *
 *				UPDATE
 *
 **********************************************************************/

/*
 * vrtp_route_flags
 *
 * A route was learned via an RTP neighbor.  What SRTP flags should be attached
 * to it.  Obviously none can come from the neighbor, since it doesn't support
 * SRTP.  The only thing that can be be determined a priori are the flags for
 * the neighbor itself.
 */
static inline ulong vrtp_route_flags (
    vinesnettype *server,
    vinesneighbortype *neighbor)
{
    if (server->net != neighbor->net)
	return(VSRTP_ENTRY_NO_SEQ);
    return(neighbor->srtp_flags);
}

/*
 * vrtp_route_update
 *
 * Perform updating of the VINES RTP routing tables.
 */
void vrtp_route_update (
    ulong dstnet,
    ushort advertised,
    vinespathtype *path,
    boolean *needsupdate)
{
    vinesneighbortype *current_gw;
    vinesneighbortype *neighbor;
    vinesnettype *server;
    vinesroutetype *route;
    int metric;
    boolean do_debug;

    if (!path || !path->neighbor)
	return;

    neighbor = path->neighbor;
    metric = min(VINES_INFINITY, advertised + neighbor->metric);
    server = vroute_FindEntry(dstnet);
    do_debug = do_vines_table_debug(dstnet, VINES_SERVER);

    /*
     * Not found, enter it.
     */
    if (server == NULL) {
	if (metric == VINES_INFINITY)
	    return;
	server = vines_add_server(dstnet, V_NET_RTP);
	route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	if (!route) {
	    vines_delete_server_entry(server);
	    return;
	}
	route->origin_timestamp = 0;
	route->local_timestamp = vines_sequence + 1;
	route->rtp_flags = vrtp_route_flags(server, neighbor);
	vines_new_server(server);
	if (do_debug)
	    buginf("\nVRTP: create route to %#Z via %#z (metric %04x)",
		   server->net, neighbor->net, neighbor->host, metric);
	return;
    }

    /*
     * Never be fooled by someone else advertising this routers address.
     */
    if (server->net == vines_network)
	return;

    /*
     * Better metric
     */
    if (metric < server->actual_metric) {
	if (do_debug)
	    buginf("\nVRTP: better route for %#Z (new %04x, old %04x, timestamp %08x)",
		   dstnet, metric, server->actual_metric, vines_sequence + 1);

	/*
	 * Does this change require suppression?  VPD 4-37 and 4-38.
	 */
	current_gw = server->best_route->gw;
	if ((server->actual_metric == VINES_INFINITY) || (current_gw == neighbor)) {
	    vines_set_suppresstime(server);
	    vines_set_changetime(server);
	}
	
	if (vines_onlyone) {
	    route = server->best_route;
	    if (route->flags & V_NET_STATIC)
		return;
	    route->gw = neighbor;
	    route->metric = advertised;
	} else {
	    /*
	     * Create the better route.  If there is no memory left to create
	     * it, then leave the existing route in place.
	     */
	    route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	    if (!route)
		return;
	    server = vines_delete_route_by_xxx(server, NULL, V_NET_RTP, metric,
					       timer_no_time);
	    if (!server)
		return;
	}
	
	route->origin_timestamp = 0;
	route->local_timestamp = vines_sequence + 1;
	route->rtp_flags = vrtp_route_flags(server, neighbor);
	vines_fixup_server(server, TRUE);
	vines_flush_cache_by_server(server);
    } else if (metric > server->actual_metric) {
	/*
	 * Worse metric.  See whether this neighbor currently has a route
	 * for the advertised server.  If so, mark that route as being
	 * unreachable.
	 */
	route = vines_get_route_by_xxx(server, neighbor, V_NET_RTP, NULL);
	if (route && (route->metric != VINES_INFINITY)) {
	    if (do_debug)
		buginf("\nVRTP: mark %#Z unreachable via %#z (timestamp %08x)",
		       dstnet, neighbor->net, neighbor->host, vines_sequence + 1);
	    route->origin_timestamp = 0;
	    route->local_timestamp = vines_sequence + 1;
	    route->rtp_flags = vrtp_route_flags(server, neighbor);
	    GET_TIMESTAMP(route->gctime);
	    route->metric = VINES_INFINITY;
	    route->flags  = V_NET_RTP;
	    vines_set_changetime(server);
	    vines_fixup_server(server, TRUE);
	}
	/*
	 * The router may know a better valid route, but not be
	 * advertising it yet.  Don't try to help the sender unless the
	 * better route is already being advertised.
	 */
	if (metric == VINES_INFINITY &&
	    server->adv_metric != VINES_INFINITY &&
	    path->srtp_flags & VSRTP_ENTRY_WAN) {
	    *needsupdate = TRUE;
	}
    } else {
	/*
	 * Same metric.  Either it is already in our tables, or we need
	 * to add another entry.  Add_route will fix up the server
	 * entry, so we don't have to worry about the FS cache here.
	 */
	if (metric == VINES_INFINITY)
	    return;
	route = vines_get_route_by_xxx(server, neighbor, V_NET_RTP, NULL);
	if (route) {
	    vines_set_route_timestamp(route);
	    route->flags  = V_NET_RTP;
	    return;
	}

	vines_flush_cache_by_server(server);
	
	if (vines_onlyone) {
	    route = vines_get_route_by_xxx(server, NULL, V_NET_RTP, NULL);
	    if (route == NULL) return;

	    /* Reuse this route entry */
	    route->gw = neighbor;
	    route->metric = advertised;
	    GET_TIMESTAMP(route->timer);
	    route->uses = 0;
	    route->flags = V_NET_RTP;
	    route->origin_timestamp = 0;
	    route->local_timestamp = vines_sequence + 1;
	    route->rtp_flags = vrtp_route_flags(server, neighbor);
	    vines_fixup_server(server, TRUE);
	    if (do_debug)
		buginf("\nRTP: fresher route to %#Z via %#z (metric %x, timestamp %x)",
		       dstnet, neighbor->net, neighbor->host, metric, route->local_timestamp);
	    return;
	}
	
	route = vines_add_route(server, neighbor, V_NET_RTP, advertised);
	if (route == NULL)
	    return;
	route->origin_timestamp = 0;
	route->local_timestamp = vines_sequence + 1;
	route->rtp_flags = vrtp_route_flags(server, neighbor);
	if (do_debug)
	    buginf("\nVRTP: alternate route to %#Z via %#z (metric %04x, timestamp %08x)",
		   dstnet, neighbor->net, neighbor->host, metric, vines_sequence + 1);
    }
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * vrtp_display_global
 *
 * Print all RTP information that is maintained on a per global basis.
 */
void vrtp_display_global (void)
{
    vinesneighbortype *neighbor;
    vinespathtype *path;
    mgd_timer *timer;
    char temp[40];

    printf("\n  Slowest update interval is %u seconds",
	   vines_slowest_update / ONESEC);
    if (mgd_timer_running(&vines_changed))
	printf("\n  Next change update in %u seconds",
	       mgd_timer_left_sleeping(&vines_changed) / ONESEC);

    printf("\n  Roll Call timer queue:");
    if (!mgd_timer_running(&vrtp_rc_timer)) {
	printf("  (empty)");
    } else {
	timer = &vrtp_rc_timer;
	while ((timer = mgd_timer_next_running(timer)) != NULL) {
	    path = mgd_timer_context(timer);
	    if (path == NULL) {
		printf("  (empty)");
		break;
	    }
	    neighbor = (vinesneighbortype *)path->neighbor;
	    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(timer));
	    printf("\n    Neighbor %s in %s.", vpath_id(path), temp);
	}
    }
}

/*
 * vrtp_display_interface
 *
 * Print all RTP information that is maintained on a per interface basis.
 */
void vrtp_display_interface (vinesidbtype *idb)
{
    if ((!idb->vines_update_delta_only) || vines_is_lan(idb) ||
	(idb->vines_update_count)) {
	printf("\n  Routing updates every %u seconds.",
	       idb->vines_wakeup_interval/ONESEC);
	if (interface_up(idb->phys_idb))
	    printf("  Next in %u seconds.",
		   mgd_timer_left_sleeping(&idb->wakeup_timer)/ONESEC);
    }
    if (idb->vines_update_delta_only) {
	printf("\n  Routing updates contain changes only.");
	if (idb->vines_update_count)
	    printf("\n    %d more full updates will be sent first.",
		   idb->vines_update_count);
    }
    if (mgd_timer_running(&idb->query_timer)) {
	printf("\n  RTP query will be sent in %d seconds.",
	       mgd_timer_left_sleeping(&idb->query_timer)/ONESEC);
    }
}

/*
 * vrtp_display_neighbor
 *
 * Display all the RTP counters kept on a per-neighbor basis.
 */
void vrtp_display_neighbor (vinesneighbortype *p)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    vinespathtype *path;

    printf("\n\n  RTP Counters:");

    for (path = p->available_paths; path; path = path->next) {
	char temp[40];
	swidb = path->idb->phys_idb;
	hwidb = swidb->hwptr;

	printf("\n\n    Interface %s, address %s", swidb->namestring,
	       vpath_id(path));

	printf("\n      Timers:");
	printf("\n        Roll Call: %s",
	       vtimer_pretty_print(&path->rc_timer, "not active", temp));

	printf("\n      Received counters:");
	printf("\n        Requests:   %08x", path->rtp_counts[VRTP_REQUEST]);
	printf("\n        Responses:  %08x", path->rtp_counts[VRTP_RESPONSE]);
	printf("\n        Updates:    %08x", path->rtp_counts[VRTP_UPDATE]);
	printf("\n        Redirects:  %08x", path->rtp_counts[VRTP_REDIRECT]);
	printf("\n        Unknown:    %08x", path->rtp_counts[0] + 
	       path->rtp_counts[2] + path->rtp_counts[3]);
    }
}

/*
 * vrtp_display_timer
 *
 * Display all the timer queues maintained by RTP.
 */
void vrtp_display_timer (void)
{
    vtimer_dump_path_queue(&vrtp_rc_timer, "      Roll Call: ", "empty");
}

/*
 * vrtp_decode_packet
 *
 * Print out the RTP information in a packet.  This set of routines does
 * not use any of the pointers in the packet header other than the
 * network start pointer, so it may be called at any point in the
 * processing of a packet.
 */
uchar *vrtp_decode_packet (
    uchar *data,
    int *limit,
    void (*fn)(const char *fmt, ...))
{
    vrtp_type *rtp;

    rtp = (vrtp_type *)data;
    (*fn)("\n   RTP op %02x node %02x, controller %02x, machine %02x",
	  rtp->op, rtp->node, rtp->controller, rtp->machine);
    *limit -= sizeof(vrtp_type);
    return(rtp->data);
}

/**********************************************************************
 *
 *		    RTP Generation and Processing
 *
 **********************************************************************/

static const char vrtp_bad_node[] = "\nVRTP: %s %#z's node type field indicates a %s.";
static const char vrtp_red_bad_node[] = "\nVRTP: Redirect shows %s %#z's as a %s.";

/*
 * vrtp_send_request
 *
 * Send an VINES RIP request for routing information.  This may be
 * directed to a single station or a single interface if desired.
 */
void vrtp_send_request (
    vinesidbtype *target,
    vinespathtype *path)
{
    vinesidbtype *idb;
    paktype *pak, *newpak;
    vrtp_type *rtp;
    vrtp_entry *entry;
    ulong net;
    ushort host;

    /*
     * Build a packet that will be used to broadcast on all interfaces.
     */
    pak = vrtp_getbuffer(sizeof(vrtp_entry));
    if (!pak)
	return;

    rtp = (vrtp_type *)pak->transport_start;
    rtp->op = VRTP_REQUEST;
    entry = (vrtp_entry *)rtp->data;
    PUT2WD(entry->nnet, VINES_ALLNETS);
    entry->metric = VINES_INFINITY;

    if (path) {
	target = path->idb;
	net = path->neighbor->net;
	host = path->neighbor->host;
    } else {
	net = VINES_ALLNETS;
	host = VINES_ALLHOSTS;
    }
    if (target) {
	/*
	 * send it to the target interface.
	 */
	if (vines_interface_up(target)) {
	    pak->if_output = target->phys_idb;	/* must set for bcast */
	    vrtp_send_pak(pak, path, VRTP_REQUEST);
	    target->tx_rtpv0_type[VRTP_REQUEST]++;
	    return;
	}
    } else {
	/*
	 * send it to each interface.
	 */
	FOR_ALL_VIDBS(idb) {
	    if (vines_interface_up(idb)) {
		newpak = pak_duplicate(pak);
		if (newpak != NULL) {
		    newpak->if_output = idb->phys_idb;	/* must set for bcast */
		    vrtp_send_pak(newpak, NULL, VRTP_REQUEST);
		    idb->tx_rtpv0_type[VRTP_REQUEST]++;
		}
	    }
	}
    }
    datagram_done(pak);
}

/*
 * vrtp_send_help_update 
 *
 * Receive and process a RTP route request
 */
static void vrtp_send_help_update (
    paktype *pak,
    vinesidbtype *idb,
    vinespathtype *path)
{
    vinesiptype *vip;
    vrtp_type *rtp, *newrtp;
    vrtp_entry *oldentry, *newentry;
    vinesnettype *server;
    paktype *newpak;
    int count, metric;
    ulong net;
    boolean do_debug;

    /*
     * Setup to process the original RTP message.
     */
    vip = (vinesiptype *)pak->network_start;
    rtp = (vrtp_type *)pak->transport_start;
    do_debug = do_vines_rtp_v_debug(vines_network, vines_host);

    /*
     * See if there is a packet available for a response.
     */
    newpak = vrtp_getbuffer(VRTP_MAX_ENTRIES * sizeof(vrtp_entry));
    if (!newpak) {
	if (do_debug)
	    buginf("\nVRTP: no packet available for helper response to %#z",
		   GET2WD(vip->ssrcnet), vip->srchost);
	return;
    }

    /*
     * Produce an entry in this response for each entry in the update metric
     * that was unreachable.  If the network requested is unknown or is being
     * filtered, then do not include it in the response.  Also, do not
     * include an entry for this router.  That is implicit in the response.
     */
    count = vip->len - sizeof(vinesiptype) - sizeof(vrtp_type);
    count = count / sizeof(vrtp_entry);
    oldentry = (vrtp_entry *)rtp->data;
    newrtp = (vrtp_type *)newpak->transport_start;
    newentry = (vrtp_entry *)newrtp->data;
    for ( ; count-- > 0; oldentry++) {
	if (oldentry->metric != VINES_INFINITY)
	    continue;
	net = GET2WD(oldentry->nnet);
	if (net == vines_network)
	    continue;
	server = vroute_FindEntry(net);
	if ((server == NULL) || (server->adv_metric == VINES_INFINITY))
	    continue;
	if (idb->output_network_filter &&
	    !vines_short_access_check(server->net, VINES_SERVER,
				      idb->output_network_filter)) {
	    continue;
	}
	PUT2WD(newentry->nnet, net);
	metric = server->adv_metric;
	newentry->metric = vmetric_to_rtp(metric);
	vrtp_buginf_metric(do_debug, net, metric);
	newentry++;
    }

    if ((uchar *)newentry - newrtp->data == 0) {
	retbuffer(newpak);
	return;
    }
    newpak->datagramsize = (uchar *)newentry - (uchar *)newrtp;
    newpak->if_output = pak->if_input;
    vrtp_send_pak(newpak, path, VRTP_RESPONSE);
    idb->tx_rtpv0_type[VRTP_RESPONSE]++;
}

/*
 * vrtp_send_update
 *
 * Send an VINES RTP update ("response") message.  If 'update' is TRUE,
 * then we should send a full update, not just items that have changed.
 * If 'split' is TRUE, then we should do split horizioning for this
 * routing update.
 */
void vrtp_send_update (
    vinesidbtype *idb,
    vinespathtype *path,
    boolean fullupdate,
    boolean always_send,
    boolean split,
    uchar type)
{
    paktype *pak;
    vrtp_type *rtp;
    vrtp_entry *p;
    vinesnettype *entry;
    int count;
    boolean sent_any, done, do_debug, do_v_debug;
    ulong net;
    ushort host;

    sent_any = done = FALSE;
    entry = NULL;
    do_debug = do_vines_rtp_debug(vines_network, vines_host);
    do_v_debug = do_debug && (vinesrtp_debug & VERBOSE_DEBUG);

    if (path) {
	net = path->neighbor->net;
	host = path->neighbor->host;
    } else {
	net = VINES_ALLNETS;
	host = VINES_ALLHOSTS;
    }

    /*
     * Banyan is claiming that cisco is "non-compliant" with the VINES
     * RTP specification.  The specific areas that they are complaining
     * about are 1) split horizon, and 2) flash updates only containing
     * changed information.  According to the specification, a router
     * must always announce all routes that it knows, even though this is
     * a waste of network and processor bandwidth.  Conserving this
     * bandwidth and is the reason the enhancements were originally
     * implemented.
     */
    if (waste_bandwidth_because_banyan_whines) {
	if (!idb->vines_update_delta_only)
	    fullupdate = TRUE;
	split = FALSE;
    }
    
    /*
     * Keep grabbing a packet and stufing it with routes until the entire
     * table has been walked.
     */
    while (!done) {
	pak = vrtp_getbuffer(VRTP_MAX_ENTRIES * sizeof(vrtp_entry));
	if (!pak)
	    return;
	if (do_v_debug) {
	    buginf("\nVRTP: generating %s for %#z on %s", vrtp_names[type],
		   net, host, idb->phys_idb->namestring);
	}

	count = 0;
	rtp = (vrtp_type *)pak->transport_start;
	p = (vrtp_entry *)rtp->data;
	while((entry = vroute_GetNextNetwork(entry)) != NULL) {
	    if (entry->net == vines_network)
		continue;
	    if (!fullupdate) { 
		/*
		 * Partial Update.  Put this network in if it has had
		 * changes recently, and those changes still need to be
		 * announced.
		 */
		if (!TIMER_RUNNING_AND_SLEEPING(entry->changetime))
		    continue;
	    }
	    if (split && vines_server_uses_idb(entry, idb))
		continue;
	    if (idb->output_network_filter &&
		!vines_short_access_check(entry->net, VINES_SERVER,
					  idb->output_network_filter)) {
		continue;
	    }

	    PUT2WD(p->nnet, entry->net);
	    p->metric = vmetric_to_rtp(entry->adv_metric);
	    vrtp_buginf_metric(do_v_debug, entry->net, entry->adv_metric);
	    p++;
	    if (++count == VRTP_MAX_ENTRIES)
		break;
	    process_suspend_if_req();
	}
	if (!entry)
	    done = TRUE;

	/*
	 * Did we put anything into the packet?  If so, send it.  At
	 * least one update frame (even if empty) must be sent as this is
	 * how the router advertises its existance to other routers and
	 * servers.
	 */
	pak->if_output = idb->phys_idb;	/* required for bcast */
	pak->datagramsize = (uchar *)p - (uchar *)rtp;
	if (count || always_send || (fullupdate && !sent_any)) {
	    sent_any = TRUE;
	    vrtp_send_pak(pak, path, type);
	    idb->tx_rtpv0_type[type]++;
	} else {
	    if (do_debug) {
		buginf("\nVRTP: sending %s to %#z on %s (squelched)", vrtp_names[type],
		       net, host, idb->phys_idb->namestring);
	    }
	    datagram_done(pak);
	}
	process_suspend_if_req();
    }
}

/*
 * vrtp_rcv_request
 *
 * Receive and process a RTP route request
 */
void vrtp_rcv_request (
    paktype *pak,
    vinespathtype *path)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    vrtp_entry *oldentry, *newentry;
    vinesnettype *server;
    vinesidbtype *idb;
    paktype *newpak;
    vrtp_type *newrtp;
    int count, metric;
    ulong net;
    boolean do_debug;

    vip = (vinesiptype *)pak->network_start;
    rtp = (vrtp_type *)pak->transport_start;
    idb = vines_getidb(pak->if_input);
    do_debug = do_vines_rtp_v_debug(vines_network, vines_host);

    if (vip->len == sizeof(vinesiptype) + sizeof(vrtp_type)) {
	/*
	 * Send a full update.
	 */
	vrtp_send_update(idb, path, TRUE, TRUE, !idb->vines_split_disabled,
			 VRTP_RESPONSE);
	return;
    }

    /*
     * Was this a request for a full update?
     */
    count = vip->len - sizeof(vinesiptype) - sizeof(vrtp_type);
    count = count / sizeof(vrtp_entry);
    oldentry = (vrtp_entry *)rtp->data;
    if ((count == 1) && (GET2WD(oldentry->nnet) == VINES_ALLNETS)) {
	/*
	 * Send a full update.
	 */
	vrtp_send_update(idb, path, TRUE, TRUE, !idb->vines_split_disabled,
			 VRTP_RESPONSE);
	return;
    }

    /*
     * See if there is a packet available for a response.
     */
    newpak = vrtp_getbuffer(VRTP_MAX_ENTRIES * sizeof(vrtp_entry));
    if (!newpak) {
	if (do_debug)
	    buginf("\nVRTP: no packet available for response to %#z",
		   GET2WD(vip->ssrcnet), vip->srchost);
	return;
    }

    /*
     * Produce an entry in the response for each entry in the request.
     * If the network requested is unknown or is being filtered, then
     * return a metric of infinity instead of ignoring it.  Also, do not
     * include an entry for this router.  That is implicit in the response.
     */
    newrtp = (vrtp_type *)newpak->transport_start;
    newentry = (vrtp_entry *)newrtp->data;
    for ( ; count-- > 0; oldentry++) {
	net = GET2WD(oldentry->nnet);
	if (net == vines_network)
	    continue;
	server = vroute_FindEntry(net);
	if (server && idb->output_network_filter &&
	    !vines_short_access_check(server->net, VINES_SERVER,
				      idb->output_network_filter)) {
	    server = NULL;
	}
	PUT2WD(newentry->nnet, net);
	metric = server ? server->adv_metric : VINES_INFINITY;
	newentry->metric = vmetric_to_rtp(metric);
	vrtp_buginf_metric(do_debug, net, metric);
	newentry++;
    }

    newpak->datagramsize = (uchar *)newentry - (uchar *)newrtp;
    newpak->if_output = pak->if_input;
    vrtp_send_pak(newpak, path, VRTP_RESPONSE);
    idb->tx_rtpv0_type[VRTP_RESPONSE]++;
}

/*
 * vrtp_rcv_response
 *
 * Process an VINES RTP packet
 */
void vrtp_rcv_response (
    paktype *pak,
    vinespathtype *path)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    vrtp_entry *p;
    int nentries;
    vinesidbtype *idb;
    vinesneighbortype *neighbor;
    ulong srcnet, net;
    boolean needsupdate, do_debug;
    int metric;

    /*
     * Reset the Roll Call timer for this neighbor.
     */
    if (path->srtp_flags & VSRTP_ENTRY_WAN) {
	vrtp_rc_timer_enqueue(path);
    }
    idb = vines_getidb(pak->if_input);

    /*
     * The fact that an routing response was received from a system
     * indicates that the route to that system is available.
     */
    needsupdate = FALSE;
    vip = (vinesiptype *)pak->network_start;
    rtp = (vrtp_type *)pak->transport_start;
    srcnet = GET2WD(vip->ssrcnet);
    do_debug = do_vines_rtp_v_debug(srcnet, vip->srchost);
    if (do_debug)
	buginf("\n    network %#Z from the server", srcnet);
    neighbor = vneigh_FindEntry(srcnet, vip->srchost);
    vrtp_route_update(srcnet, 0, path, &needsupdate);

    /*
     * Now iterate through all routes explicitly specified in the routing
     * response packet.  The sender of an update should never be listed
     * explicitly in the update.
     */
    nentries = (vip->len - sizeof(vinesiptype) - sizeof(vrtp_type)) /
	sizeof(vrtp_entry);
    for (p = (vrtp_entry *)rtp->data; nentries-- > 0; p++) {
	net = GET2WD(p->nnet);
	metric = vmetric_from_rtp(p->metric);
	vrtp_buginf_metric(do_debug, net, metric);
	if (net == srcnet) {
	    if (do_debug)
		buginf(" (invalid entry)");
	    continue;
	}
	if (idb->input_network_filter &&
	    !vines_short_access_check(net, VINES_SERVER,
				      idb->input_network_filter)) {
	    if (do_debug)
		buginf(" (failed filter)");
	    continue;
	}
	if (vines_valid_net(net) && vmetric_valid(metric))
	    vrtp_route_update(net, metric, path, &needsupdate);
    }
    if (needsupdate) {
	vrtp_send_help_update(pak, idb, path);
    }
}

/*
 * vrtp_send_redirect
 *
 * Process an VINES RTP packet
 */
static const char vrtp_no_redirect[] = "\nVRTP: not sending redirect for %#z (%s)";

void vrtp_send_redirect (
    paktype *original,
    routingdecision *thewaythere)
{
    vinesneighbortype *neighbor;
    vrtp_type *rtp;
    vrtp_redirecttype *redirect;
    vinespathtype *path;
    paktype *pak;
    vinesiptype *vip;
    riftype sender_rif, gw_rif, *temp_rif;
    srbroute_t *data;
    vinesidbtype *idb;
    ulong dstnet;
    leveltype level;
    uchar sender_mac[IEEEBYTES];
    ulong dstmetric, nbrmetric, do_debug;
    vinespathtype thewayback;
    vinesneighbortype dummyneighbor;
    
    /*
     * Do not send a redirect if we have sent one recently, if we turned
     * off redirects by setting the interval to zero, or if we are
     * filtering information about that network.
     */
    vip = (vinesiptype *)original->network_start;
    idb = vines_getidb(original->if_input);
    dstnet = GET2WD(vip->ddstnet);
    if (idb->output_network_filter &&
	!vines_short_access_check(dstnet, VINES_SERVER,
				  idb->output_network_filter)) {
	return;
    }
    if (SLEEPING(idb->vines_redirect_time) || !idb->vines_redirect_interval)
	return;
    TIMER_START(idb->vines_redirect_time, idb->vines_redirect_interval);
    do_debug = do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost);
    do_debug |= do_vines_rtp_debug(GETLONG(vip->ssrcnet), vip->srchost);

    find_802addr(original, sender_mac, GET_SOURCE);

    /*
     * Are there any RIFs associated with the stations invloved in this
     * redirect?  There are four cases, and two sub-cases:
     *
     * 	Source	Dest	Response
     * 	------	----	--------
     * 	no rif	no rif	send redirect with no rif
     * 	rif	no rif	send redirect with no rif
     * 	no rif	rif	send redirect with rif
     * 	rif	rif	a) if source and dest are on same ring,
     * 			   send redirect with rif 
     * 			b) if source and dest are on different
     * 			   rings, do not send redirect
     *
     * I do not know why Banyan implemented the last case this way.  It
     * is possible, but painful, to concatenate RIFs.  We should not send
     * a redirect in this case to be compatible with Banyan.  They will
     * do the wrong thing upon receipt of this packet.
     *
     * Rifs are copied to local storage because they can change at
     * interrupt level.  If this happens while the actual rif part of the
     * entry is being copied, you can form a circular rif.
     */
    if (!using_multiring(original->if_input, LINK_VINES)) {
	memset(&gw_rif, 0, sizeof(riftype));
	memset(&sender_rif, 0, sizeof(riftype));
    } else {
	level = raise_interrupt_level(NETS_DISABLE);
	temp_rif = rif_lookup(thewaythere->path->dlc_addr.addr,
			      hwidb_or_null(original->if_input), NULL, 0);
	if (temp_rif)
	    bcopy(temp_rif, &gw_rif, sizeof(riftype));
	else
	    memset(&gw_rif, 0, sizeof(riftype));
	temp_rif = rif_lookup(sender_mac, 
			      hwidb_or_null(original->if_input), NULL, 0);
	if (temp_rif)
	    bcopy(temp_rif, &sender_rif, sizeof(riftype));
	else
	    memset(&sender_rif, 0, sizeof(riftype));
	reset_interrupt_level(level);

	if ((gw_rif.length >= RC_MIN_RIF)
	    && (sender_rif.length >= RC_MIN_RIF) &&
	    (RIF_DEST_RING((srbroute_t *)gw_rif.rif) !=
	     RIF_DEST_RING((srbroute_t *)sender_rif.rif))) {
	    if (do_debug)
		buginf(vrtp_no_redirect, dstnet, vip->dsthost,
		       "src and dst have rifs");
	    return;
	}
    }
    
    /*
     * Can we get a packet to create a redirect.
     */
    pak = vrtp_getbuffer(sizeof(vrtp_redirecttype) + V_REDIR_DATAMAX);
    if (!pak) {
	if (do_debug)
	    buginf(vrtp_no_redirect, dstnet, vip->dsthost, "no packet");
	return;
    }
    pak->datagramsize -= V_REDIR_DATAMAX;
    rtp = (vrtp_type *)pak->transport_start;
    redirect = (vrtp_redirecttype *)rtp->data;
    memset(redirect, 0, sizeof(vrtp_redirecttype));
    redirect->version = 0;
    redirect->macaddrlen = IEEEBYTES;

    /*
     * Fill in destination specific fields.  Banyan uses a network level
     * broadcast here for some reason.  It's a royal pain, so send a
     * specific.
     */
    PUTLONG(redirect->dstnet, dstnet);
    redirect->dsthost = vip->dsthost;
    neighbor = thewaythere->neighbor;
    if ((neighbor->net == dstnet) &&
	(neighbor->host == vip->dsthost)) {
	dstmetric = thewaythere->path->metric;
	redirect->dstmetric     = vmetric_to_rtp(dstmetric);
	redirect->dstnode       = thewaythere->neighbor->node;
	redirect->dstcontroller = thewaythere->path->controller;
	redirect->dstmachine    = thewaythere->path->machine;
    } else {
	dstmetric = min(VINES_INFINITY, thewaythere->route->metric + neighbor->metric);
	redirect->dstmetric     = vmetric_to_rtp(dstmetric);
    }

    /*
     * Fill in the next hop specific fields.
     */
    path = vines_get_path_by_idb(neighbor, idb);
    nbrmetric = path->metric;
    PUTLONG(redirect->nbrnet, neighbor->net);
    redirect->nbrhost       = neighbor->host;
    redirect->nbrmetric     = vmetric_to_rtp(nbrmetric);
    redirect->nbrnode       = neighbor->node;
    redirect->nbrcontroller = path->controller;
    redirect->nbrmachine    = path->machine;
    ieee_copy(path->dlc_addr.addr, redirect->mac);

    if (gw_rif.length >= RC_MIN_RIF) {
	pak->datagramsize += gw_rif.length;
	redirect->riflen = gw_rif.length;
	bcopy(gw_rif.rif, redirect->rif, gw_rif.length);
	data = (srbroute_t *)redirect->rif;
	data->rc_dlf ^= RC_D;
    } else {
	redirect->riflen = 0;
    }

    /* Build a dummy path back to the last forwarding node, which can
     * be either the source of the packet of a router of the packet.
     */
    dummyneighbor.net = VINES_ALLNETS;
    dummyneighbor.host = VINES_ALLHOSTS;
    thewayback.neighbor = &dummyneighbor;
    thewayback.idb = idb;
    thewayback.enctype = original->enctype;
    ieee_copy(sender_mac, thewayback.dlc_addr.addr);
    
    if (do_debug)
	buginf("\nVRTP: sent redirect for %#z(%04x) via %#z(%04x)",
	       dstnet, vip->dsthost, dstmetric,
	       neighbor->net, neighbor->host, nbrmetric);
    pak->if_output = original->if_input;
    vrtp_send_pak(pak, &thewayback, VRTP_REDIRECT);
    idb->tx_rtpv0_type[VRTP_REDIRECT]++;
}

/*
 * vrtp_rcv_redirect
 *
 * Process an VINES RTP packet.  This will probably only happen on a
 * token ring network that is source route bridged.  Banyan's recommended
 * settings for this are for the servers to run with SRB enabled, and the
 * clients to run with SRB disabled.  This means that the router will
 * hear updated from the servers, but will not hear the hellos of the
 * clients.  It will receive redirects from a server for those clients
 * that are on the same bridged token ring as the router.
 */
void vrtp_rcv_redirect (
    paktype *pak,
    vinespathtype *path)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *gw, *source;
    vrtp_redirecttype *redirect;
    vinesidbtype *idb;
    srbroute_t *newrif;
    riftype *rif;
    hwaddrtype dlc_address, sender;
    ulong nbrnet, dstnet;
    ulong nbrmetric, dstmetric;
    boolean do_debug;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
    uchar local[SRB_MAXRIF];

    vip = (vinesiptype *)pak->network_start;
    rtp = (vrtp_type *)pak->transport_start;
    redirect = (vrtp_redirecttype *)rtp->data;
    nbrmetric = vmetric_from_rtp(redirect->nbrmetric);
    dstmetric = vmetric_from_rtp(redirect->dstmetric);
    nbrnet = GETLONG(redirect->nbrnet);
    dstnet = GETLONG(redirect->dstnet);
    do_debug = do_vines_rtp_debug(GETLONG(vip->ssrcnet), vip->srchost);
    if (do_debug) {
	buginf("\nVRTP: redirecting data for %#z(%04x) to %#z(%04x)",
	       dstnet, redirect->dsthost, dstmetric,
	       nbrnet, redirect->nbrhost, nbrmetric);
	buginf(vrtp_mac_rif_msg, redirect->mac,
	       redirect->riflen ? get_rif_str(redirect->rif, rif_string) : "none");
    }
    source = vneigh_FindEntry(GETLONG(vip->ssrcnet), vip->srchost);
    gw = vneigh_FindEntry(nbrnet, redirect->nbrhost);
    idb = vines_getidb(pak->if_input);

    /*
     * Check for a redirect sent by me, redirecting someone to me, or
     * about me.  This happens because a VINES server sends a redirect
     * messages as a MAC layer broadcast.  This means that any redirect
     * send to another station redirecting them to this router will also
     * be heard by this router.
     */
    if (idb->input_network_filter &&
	!vines_short_access_check(dstnet, redirect->dsthost,
				  idb->input_network_filter)) {
	if (do_debug)
	    buginf("\nVRTP: Redirect discarded, failed filter");
	return;
    }
    if (!source || (source->newflags & V_N_SELF)) {
	if (do_debug)
	    buginf("\nVRTP: Redirect discarded, bad source address");
	return;
    }
    if (gw && (gw->newflags & V_N_SELF)) {
	if (do_debug)
	    buginf("\nVRTP: Redirect discarded, bad gateway address");
	return;
    }
    if ((dstnet == vines_network) && (redirect->dsthost == vines_host)) {
	if (do_debug)
	    buginf("\nVRTP: Redirect discarded, bad target address");
	return;
    }

    /*
     * Make noise about Banyan bugs and then correct them.
     */
    if ((redirect->nbrhost == VINES_SERVER) &&
	(redirect->nbrnode != VRTP_NODE_ROUTER)) {
	redirect->nbrnode = VRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vrtp_red_bad_node, "server", nbrnet,
		   redirect->nbrhost, "client");
    } else if ((redirect->nbrhost != VINES_SERVER) &&
	       (redirect->nbrnode != VRTP_NODE_CLIENT)) {
	redirect->nbrnode = VRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vrtp_red_bad_node, "client", nbrnet,
		   redirect->nbrhost, "server");
    }
    if ((redirect->dsthost == VINES_SERVER) &&
	(redirect->dstnode != VRTP_NODE_ROUTER)) {
	redirect->dstnode = VRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vrtp_red_bad_node, "server", nbrnet,
		   redirect->nbrhost, "client");
    } else if ((redirect->dsthost != VINES_SERVER) &&
	       (redirect->dstnode != VRTP_NODE_CLIENT)) {
	redirect->dstnode = VRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vrtp_red_bad_node, "client", nbrnet,
		   redirect->nbrhost, "server");
    }

    /*
     * Create the neighbor entry if necessary.
     */
    if (!gw) {
	gw = vines_add_neighbor(nbrnet, redirect->nbrhost,
				(V_N_DYNAMIC | V_N_REDIRECT), VINES_INFINITY,
				VRTP_VERSION, redirect->nbrnode);
	if (!gw) {
	    return;
	}
	memset(&dlc_address, 0, sizeof(hwaddrtype));
	dlc_address.type = STATION_IEEE48;
	dlc_address.length = STATIONLEN_IEEE48;
	dlc_address.target_type = TARGET_UNICAST;
	ieee_copy(redirect->mac, dlc_address.addr);
	path = vines_add_path(gw, (V_N_DYNAMIC | V_N_REDIRECT), idb,
			      idb->vines_enctype, &dlc_address, nbrmetric,
			      VRTP_VERSION, redirect->nbrnode);
	if (!path || !gw->available_paths) {
	    vines_delete_neighbor_entry(gw);
	    return;
	}
	path->controller = redirect->nbrcontroller;
	path->machine    = redirect->nbrmachine;
	ieee_copy(redirect->mac, path->dlc_addr.addr);
	if (using_multiring(pak->if_input, LINK_VINES)) {
	    rif = rif_lookup(path->dlc_addr.addr, 
			     hwidb_or_null(pak->if_input), NULL, 0);
	    if (!rif) {
		/*
		 * Don't already have a rif.  Merge the rif for the
		 * source address and the rif included inside of the
		 * redirect frame.
		 */
		(*pak->if_input->hwptr->extract_snpa)(pak, &sender, SOURCE);
		rif = rif_lookup(sender.addr, hwidb_or_null(pak->if_input), 
				 NULL, 0);
		if (!redirect->riflen) {
		    /*
		     * No rif in redirect, so use the same rif as for the
		     * sender.  The rif_update routine flips the D bit on
		     * all AGED rifs that it creates because it assumes
		     * that they are being gleaned from a received frame.
		     * Make sure to flip the D bit here so that when
		     * rif_update does do its flip, what ends up in the
		     * rif tables for the new entey is identical to what
		     * is already there for the sender.
		     */
		    if (!rif || (rif->length < RC_MIN_RIF)) {
			newrif = NULL;
		    } else {
			bcopy(rif->rif, &local, rif->length);
			newrif = (srbroute_t *)&local;
			newrif->rc_dlf ^= RC_D;
		    }
		} else {
		    /*
		     * Rif in redirect.  What to do?
		     */
		    if (!rif || (rif->length < RC_MIN_RIF)) {
			newrif = (srbroute_t *)redirect->rif;
		    } else {
			/*
			 * If you need a rif to reach the sender, and there
			 * is a rif included in the redirect, then the target
			 * should be on the local ring.  Banyan will not send
			 * a redirect unless this is true.
			 */
			newrif = NULL;
			path->metric = idb->vines_metric;
		    }
		}
		rif_update(hwidb_or_null(pak->if_input), 0, 
			   path->dlc_addr.addr, NULL, newrif, RIF_AGED);
	    }
	}
    }

    /*
     * Was this a client redirect?  If so, we are done.  Also protect
     * against adding an entry for the local router's address.
     */
    if (redirect->dsthost != VINES_SERVER)  /* about a network */
	return;
    if (redirect->nbrhost != VINES_SERVER)  /* via a router */
	return;
    if ((dstnet == vines_network) || (nbrnet == vines_network))
	return;

    /*
     * Add the new route for the server.  Calculate the metric value that
     * should be passed to the add_route subroutine to insure the metric
     * value that ends up in the route table is the same as the
     * redirecting router's metric to the destination.  Once the new
     * route is installed, delete any old routes that point at the
     * neibhbor that sent the redirect.  They are abviously no longer
     * good, or we wouldn't have received a redirect in the first place.
     * The only purpose for the first lookup is so that the correct
     * counter gets incremented.
     */
    server = vines_add_server(dstnet, (V_NET_RTP | V_NET_REDIRECT));
    route = vines_get_route_by_idb(server, path->idb);
    if (route) {
	vroute_redirect_modified++;
    } else {
	vroute_redirect_created++;
    }
    route = vines_add_route(server, gw, (V_NET_RTP | V_NET_REDIRECT),
			    dstmetric - gw->metric);
    if (route) {
	server = vines_delete_route_by_xxx(server, NULL, V_NET_RTP,
					   dstmetric, timer_no_time);
    }
    if (server && server->available_routes == NULL)
	vines_delete_server_entry(server);
}

/*
 * vrtp_send_periodic
 *
 * On a broadcast media, this will send one broadcast packet.  On a multi
 * point-to-point media, this will send one packet per destination if
 * there are any known destinations, otherwise it will send a broadcast.
 */
void vrtp_send_periodic (vinesidbtype *idb)
{
    vinespathtype *path;
    boolean full_update, always_send;
    dqueue_t *q;

    if (!idb->vines_update_delta_only) {
	full_update = TRUE;
	always_send = TRUE;
    } else if (idb->vines_update_count) {
	idb->vines_update_count--;
	full_update = TRUE;
	always_send = TRUE;
    } else {
	full_update = FALSE;
	always_send = vines_is_lan(idb);
    }

    if ((idb->phys_idb->hwptr->status & IDB_VINES_BCAST) ||
	(idb->path_list.flink == &idb->path_list)) {
	vrtp_send_update(idb, NULL, full_update, always_send,
			 !idb->vines_split_disabled, VRTP_UPDATE);
	return;
    }

    for (q = idb->path_list.flink; q != &idb->path_list; q = q->flink) {
	path = (vinespathtype *)q->parent;
	if (path->neighbor->version != VRTP_VERSION)
	    continue;
	vrtp_send_update(idb, path, full_update, always_send,
			 !idb->vines_split_disabled, VRTP_UPDATE);
    }
}


/**********************************************************************
 *
 *		     Top Level Periodic Routines
 *
 **********************************************************************/

/*
 * vrtp_process_message
 *
 * Process an incoming RTP messages that just been removed from the
 * routing queue.  All valid messages are passed top the neighbor update
 * routine to keep that table current.
 */
void vrtp_process_message (paktype *pak)
{
    vinesiptype *vip;
    vinesneighbortype *neighbor;
    vinespathtype *path;
    vrtp_type *rtp;
    vinesidbtype *idb;
    ulong srcnet;
    ushort srchost;
    boolean do_debug;
    char *error;

    /*
     * Range check the operation field.
     */
    vip = (vinesiptype *)pak->network_start;
    idb = vines_getidb(pak->if_input);
    srcnet = GET2WD(vip->ssrcnet);
    srchost = vip->srchost;
    rtp = (vrtp_type *)vip->data;
    do_debug = do_vines_rtp_debug(srcnet, srchost);
    if ((srcnet == vines_network) && (srchost == vines_host)) {
	if (do_debug)
	    buginf("\nVRTP: ignoring RTPv0 type (%d), my source address.",
		   rtp->op);
	return;
    }
    if (rtp->op >= VRTP_LAST) {
	if (do_debug)
	    buginf("\nVINES: illegal RTPv0 type (%d) from %#z.",
		   rtp->op, srcnet, srchost);
	idb->rx_rtpv0_illegal++;
	return;
    }
    
    /*
     * Check for compatibility mode.  If this is a compatability mode RTP
     * message, and the current neighbor entry is marked having been
     * learned via RTP, then it should be deleted and a SRTP request
     * should be sent to the senders address on that interface.
     *
     * Also check for a router/server that has been downgraded and no
     * longer supports SRTP.  This could happen if a router rebooted.
     */
    if (vines_vsrtp_enabled(idb)) {
	neighbor = vneigh_FindEntry(srcnet, srchost);
	if (rtp->machine & VRTP_COMPATABILITY) {
	    if (do_debug)
		buginf("\nVRTP: ignoring compatibility %s from %#z.",
		       vrtp_names[rtp->op], srcnet, srchost);
	    if ((neighbor == NULL) || (neighbor->version == VSRTP_VERSION))
		return;
	    path = vines_get_path_by_idb(neighbor, idb);
	    if (path)
		vsrtp_send_full_request(idb, path);
	    vines_flush_dynamic_paths(srcnet, srchost);
	    vines_neighbor_update(pak, &neighbor, &path, &error,
				  VSRTP_VERSION, rtp->node);
	    if (neighbor && path) {
		neighbor->version = VSRTP_VERSION;
		vines_new_neighbor(neighbor, path);
	    }
	    return;
	} else if (neighbor && (neighbor->version == VSRTP_VERSION)) {
	    vines_flush_dynamic_paths(srcnet, srchost);
	    neighbor = vneigh_FindEntry(srcnet, srchost);
	    if (neighbor) {
		neighbor->version = VRTP_VERSION;
	    }
	}
    }

    /*
     * Count it and consume it.
     */
    idb->rx_rtpv0_type[rtp->op]++;
    if ((pak->if_input->hwptr->broadcast)(pak->if_input->hwptr, pak)==FUZ_TRUE)
	idb->rx_rtpv0_bcast++;
    if (do_debug) {
	buginf(vrtp_upd_msg, vrtp_names[rtp->op], srcnet,
	       srchost, idb->phys_idb->namestring);
    }
    
    /*
     * Make noise about Banyan bugs and then correct them.
     */
    if ((srchost == VINES_SERVER) &&
	(rtp->node != VRTP_NODE_ROUTER)) {
	rtp->node = VRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vrtp_bad_node, "Server", srcnet, srchost, "client");
    } else if ((srchost != VINES_SERVER) &&
	       (rtp->node != VRTP_NODE_CLIENT)) {
	rtp->node = VRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vrtp_bad_node, "Client", srcnet, srchost, "server");
    }
    
    if (!vines_neighbor_update(pak, &neighbor, &path, &error,
			       VRTP_VERSION, rtp->node)) {
	if (do_debug)
	    buginf(" (%s)", error);
	return;
    }
    if (!neighbor || !path) {
	if (do_debug)
	    buginf("\nVRTP: Could not create neighbor table entry.  Flushing packet.");
	return;
    }
    path->rtp_counts[rtp->op]++;

    /*
     * Update some version specific fields in the neighbor structure.
     */
    path->controller = rtp->controller;
    path->machine = rtp->machine;
    if ((path->srtp_flags & VSRTP_ENTRY_WAN) && !mgd_timer_running(&path->rc_timer))
	vrtp_rc_timer_initial_enqueue(path);

    /*
     * Call the appropriate routine to process this packet.
     */
    switch (rtp->op) {
      case VRTP_REQUEST:
	vrtp_rcv_request(pak, path);
	break;
	
      case VRTP_UPDATE:
      case VRTP_RESPONSE:
	if (rtp->node == VRTP_NODE_ROUTER)
	    vrtp_rcv_response(pak, path);
	break;
	
      case VRTP_REDIRECT:
	vrtp_rcv_redirect(pak, path);
	break;
	
      default:
	break;
    }
}

/*
 * vrtp_rc_timer_expired
 *
 * Process all RTP timers to see if there are any events that must be
 * processed.
 */
void vrtp_rc_timer_expired (vinespathtype *path)
{
    path->rc_state++;
    if (path->rc_state == VRTP_RC_GIVE_UP) {
	path->gctime = TIMER_ADD_DELTA(path->timer,
				       VRTP_HELLOINVALID(path->idb));
	path->metric = VINES_INFINITY;
	path->local_timestamp = vines_sequence + 1;
	vines_fixup_neighbor(path->neighbor);
	vines_flush_cache_by_neighbor(path->neighbor);
    } else {
	vrtp_send_request(path->idb, path);
	vrtp_rc_timer_enqueue_retry(path);
    }
}


