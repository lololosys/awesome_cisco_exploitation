/* $Id: vines_srtp_fsm.c,v 3.7.18.4 1996/07/31 22:37:30 ehk Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_srtp_fsm.c,v $
 *------------------------------------------------------------------
 * vines_srtp_fsm.c -- Vines Sequenced RTP State Machine
 *
 * July 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_srtp_fsm.c,v $
 * Revision 3.7.18.4  1996/07/31  22:37:30  ehk
 * CSCdi64769:  Vines SRTP crash while processing incoming RTP update
 * packet
 * Branch: California_branch
 * prevent modifying a freed memory bloc
 *
 * Revision 3.7.18.3  1996/07/19  22:47:08  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/03/28  22:42:35  ehk
 * CSCdi51823:  Vines recompute does not recompute.
 * Branch: California_branch
 * take all interfaces into account when computing vines network
 * number (CSCdi51823). if no interface MAC address is available,
 * use the system default MAC addres (CSCdi52195). to be consistent
 * with other protocols, disallow vines UI configuration unless
 * vines routing is enabled (CSCdi52009).
 *
 * Revision 3.7.18.1  1996/03/18  22:32:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  02:05:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  11:05:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:47:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/08  20:14:21  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.6  1996/01/22  07:44:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/01  21:39:25  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.4  1995/11/28  23:40:30  slin
 * CSCdi44517:  Vines:Incomplete Answer to a Vines Client SRTP Request
 * - Off by one error in calculating number of entries in request
 *
 * Revision 3.3  1995/11/17  17:58:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:44:51  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.1  1995/06/07  23:15:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Include Files
 */
#include "master.h"
#include <string.h>
#include "address.h"
#include "packet.h"
#include "interface_private.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/rif_util.h"
#include "../util/fsm.h"
#include "../vines/vines_private.h"

typedef ushort	action_t;	/* return value for state action routines */

boolean vines_state_debug;

/*****************************************************************************
 *
 *                          VSRTP FSM utilities
 *
 *****************************************************************************/

/*
 * v s r t p _ f s m _ d e c o d e
 *
 * Given a major and minor, produce the magic index that drives all
 * the different array indicies.
 */

int vsrtp_fsm_decode (vsrtp_fsm_data *data, int major, int minor)
{
    switch (major) {
      case vsrtp_rx_request:
	if (minor > vsrtp_rx_req_max)
	    return(FSM_FAULT);
	return(vsrtp_rx_req_base + minor - 1);

      case vsrtp_rx_update:
	if (minor > vsrtp_rx_upd_max)
	    return(FSM_FAULT);
	return(vsrtp_rx_upd_base + minor - 1);

      case vsrtp_rx_response:
	if (minor > vsrtp_rx_resp_max)
	    return(FSM_FAULT);
	return(vsrtp_rx_resp_base + minor - 1);

      case vsrtp_rx_redirect:
	return(vsrtp_rx_redir_base);

      case vsrtp_rx_reinit:
	return(vsrtp_rx_reinit_base);

      case vsrtp_timer:
	if (minor > vsrtp_timer_max)
	    return(FSM_FAULT);
	if (data->path->retry_count == 0)
	    return(vsrtp_rx_timer_base + vsrtp_timer_retry_final - 1);
	return(vsrtp_rx_timer_base + vsrtp_timer_retry - 1);

      default:
	return(FSM_FAULT);
    }
}

/*
 * vsrtp_decode_request_minor
 *
 * Convert a SRTP request header into a state machine minor index.
 */
int vsrtp_decode_request_minor (uchar type)
{
    switch (type) {
      case VSRTP_REQ_SPECIFIC: return(vsrtp_rx_req_specific);
      case VSRTP_REQ_CHANGES:  return(vsrtp_rx_req_changes);
      case VSRTP_REQ_FULL:     return(vsrtp_rx_req_full);
      case VSRTP_REQ_RESPONSE: return(vsrtp_rx_req_response);
      default:                 return(vsrtp_rx_req_invalid);
    }
}

/*
 * vsrtp_decode_update_major
 *
 * Convert a SRTP update header into a state machine major index.
 */
int vsrtp_decode_update_major (vsrtp_update *update)
{
    if (update->type == VSRTP_UPD_UPDATE)
	return(vsrtp_rx_update);
    return(vsrtp_rx_response);
}

/*
 * vsrtp_decode_update_minor
 *
 * Convert a SRTP update header into a state machine minor index.
 */
int vsrtp_decode_update_minor (uchar update_flags, int comparison)
{
    if (update_flags & VSRTP_UPD_FLAG_SYNC) {
	return(vsrtp_rx_upd_sync_less + comparison);
    } else if (update_flags & VSRTP_UPD_FLAG_FULL) {
	return(vsrtp_rx_upd_full_less + comparison);
    } else if (update_flags & VSRTP_UPD_FLAG_CHANGE) {
	return(vsrtp_rx_upd_change_less + comparison);
    } else {
	return(vsrtp_rx_upd_null_less + comparison);
    }
}

/*
 * vsrtp_prstate
 *
 * Return a string that describes the vsrtp fsm state.
 */
char *vsrtp_prstate(vsrtp_state state)
{
    switch(state) {
      case vsrtp_st_up:		return("up");
      case vsrtp_st_chgrqst:	return("change request");
      case vsrtp_st_fullrqst:	return("full request");
      case vsrtp_st_init:	return("init");
      default:			return("v0");
    }
}

/*
 * vsrtp_prinput
 *
 * Return a string that describes a major and minor input
 */
char *vsrtp_prinput (vsrtp_major major, int minor)
{
    static char buffer[30];

    switch (major) {
      case vsrtp_rx_request:
	switch(minor) {
	  case vsrtp_rx_req_specific: return("request specific");
	  case vsrtp_rx_req_changes:  return("request changes");
	  case vsrtp_rx_req_full:     return("request full");
	  case vsrtp_rx_req_response: return("request response");
	  default:		      return("unknown request");
	}
	break;

      case vsrtp_rx_update:
	switch(minor) {
	  case vsrtp_rx_upd_null_failed:    return("null update, failed");
	  case vsrtp_rx_upd_null_less:	    return("null update, less");
	  case vsrtp_rx_upd_null_equal:	    return("null update, equal");
	  case vsrtp_rx_upd_null_onemore:   return("null update, onemore");
	  case vsrtp_rx_upd_null_greater:   return("null update, greater");
	  case vsrtp_rx_upd_change_failed:  return("change update, failed");
	  case vsrtp_rx_upd_change_less:    return("change update, less");
	  case vsrtp_rx_upd_change_equal:   return("change update, equal");
	  case vsrtp_rx_upd_change_onemore: return("change update, onemore");
	  case vsrtp_rx_upd_change_greater: return("change update, greater");
	  case vsrtp_rx_upd_full_failed:    return("full update, failed");
	  case vsrtp_rx_upd_full_less:	    return("full update, less");
	  case vsrtp_rx_upd_full_equal:	    return("full update, equal");
	  case vsrtp_rx_upd_full_onemore:   return("full update, onemore");
	  case vsrtp_rx_upd_full_greater:   return("full update, greater");
	  case vsrtp_rx_upd_sync_failed:    return("sync update, failed");
	  case vsrtp_rx_upd_sync_less:	    return("sync update, less");
	  case vsrtp_rx_upd_sync_equal:	    return("sync update, equal");
	  case vsrtp_rx_upd_sync_onemore:   return("sync update, onemore");
	  case vsrtp_rx_upd_sync_greater:   return("sync update, greater");
	  default:
	    sprintf(buffer, "unknown update (%d)", minor);
	    return(buffer);
	}
      break;

      case vsrtp_rx_response:
	switch(minor) {
	  case vsrtp_rx_resp_null_failed:    return("null response, failed");
	  case vsrtp_rx_resp_null_less:	     return("null response, less");
	  case vsrtp_rx_resp_null_equal:     return("null response, equal");
	  case vsrtp_rx_resp_null_onemore:   return("null response, onemore");
	  case vsrtp_rx_resp_null_greater:   return("null response, greater");
	  case vsrtp_rx_resp_change_failed:  return("change response, failed");
	  case vsrtp_rx_resp_change_less:    return("change response, less");
	  case vsrtp_rx_resp_change_equal:   return("change response, equal");
	  case vsrtp_rx_resp_change_onemore: return("change response, onemore");
	  case vsrtp_rx_resp_change_greater: return("change response, greater");
	  case vsrtp_rx_resp_full_failed:    return("full response, failed");
	  case vsrtp_rx_resp_full_less:	     return("full response, less");
	  case vsrtp_rx_resp_full_equal:     return("full response, equal");
	  case vsrtp_rx_resp_full_onemore:   return("full response, onemore");
	  case vsrtp_rx_resp_full_greater:   return("full response, greater");
	  case vsrtp_rx_resp_sync_failed:    return("sync response, failed");
	  case vsrtp_rx_resp_sync_less:	     return("sync response, less");
	  case vsrtp_rx_resp_sync_equal:     return("sync response, equal");
	  case vsrtp_rx_resp_sync_onemore:   return("sync response, onemore");
	  case vsrtp_rx_resp_sync_greater:   return("sync response, greater");
	  default:
	    sprintf(buffer, "unknown response (%d)", minor);
	    return(buffer);
	}
      break;

      case vsrtp_rx_redirect: return("redirect");
      case vsrtp_rx_reinit:   return("reinit");

      case vsrtp_timer:
	switch(minor) {
	  case vsrtp_timer_retry:    return("timer retry");
	  default:		     return("timer unknown");
	}
	break;

      default: return("unknown major");
    }
}

/*****************************************************************************
 *
 *                  V S R T P   A c t i o n s
 *
 *****************************************************************************/

/*
 * v s r t p _ a c t i o n _ a
 *
 * state:  several
 * event:  net rx request specific routes
 * action: Send an RTP response containing the requested routes.
 */

static action_t vsrtp_action_a (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak, *newpak;
    vinesiptype *vip;
    vsrtp_header *rtp;
    vsrtp_request *request;
    ulong *entry;
    vsrtp_update *response;
    vsrtp_entry *newentry;
    vinesnettype *server;
    queuetype queue;
    int i, nentries;
    vinespathtype *path;
    boolean do_debug;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;
    pak = data->pak;
    do_debug = do_vines_rtp_v_debug(vines_network, vines_host);

    /*
     * Get set to read the list of queries from the received request.
     */
    vip = (vinesiptype *)pak->network_start;
    rtp = (vsrtp_header *)pak->transport_start;
    request = (vsrtp_request *)rtp->data;
    entry = (ulong *)request->data;
    nentries = (vip->len - sizeof(vinesiptype) - VSRTP_HEADER_BYTES
		- VSRTP_REQUEST_HEADER_BYTES) / sizeof(ulong);
    vsrtp_rx_request_lookup_count++;

    /*
     * Setup a packet for the response.
     */
    newpak = vsrtp_getbuffer("response", VSRTP_UPDATE_BYTES +
			     VSRTP_ENTRY_BYTES * nentries);
    if (!newpak)
	return(FSM_OK);
    response = (vsrtp_update *)newpak->session_start;
    newentry = (vsrtp_entry *)response->data;

    /*
     * Look up and fill in the information requested.  If the network
     * requested is unknown or is being filtered, then return a metric of
     * infinity instead of ignoring it.
     */
    for (i = 0; i < nentries; i++) {
	newentry->net = GET2WD(&entry[i]);
	if (newentry->net == vines_network)
	    continue;
	server = vroute_FindEntry(newentry->net);
	if (path->idb->output_network_filter &&
	    !vines_short_access_check(server->net, VINES_SERVER,
				      path->idb->output_network_filter)) {
	    server = NULL;
	}
	if (server) {
	    newentry->metric   = vmetric_to_srtp(server->adv_metric);
	    PUTLONG(&newentry->sequence, server->best_route->origin_timestamp);
	    newentry->flags    = server->best_route->rtp_flags;
	} else {
	    newentry->metric   = vmetric_to_srtp(VINES_INFINITY);
	    PUTLONG(&newentry->sequence, 0);
	    newentry->flags    = 0;
	}
	newentry->reserved = 0;
	vrtp_buginf_metric(do_debug, newentry->net, newentry->metric);
	newentry++;
    }
    vsrtp_tx_request_lookup_count++;
    newpak->datagramsize = (uchar *)newentry - newpak->session_start;
    if (!vsrtp_fragment_update(&queue, newpak, VSRTP_UPD_RESPONSE, path->metric,
			       VSRTP_UPD_FLAG_CHANGE))
	return(FSM_OK);
    vsrtp_send_fragments(&queue, path, NULL, FALSE);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ b
 *
 * state:  several
 * event:  net rx request all changes
 * action: Send an RTP response containing all changes since the specified index.
 */

static action_t vsrtp_action_b (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak, *newpak;
    vsrtp_header *rtp;
    vsrtp_request *request;
    ulong change_id;
    queuetype queue;
    vinespathtype *path;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;
    pak = data->pak;

    /*
     * Determine the change ID for which records should be sent.
     */
    rtp = (vsrtp_header *)pak->transport_start;
    request = (vsrtp_request *)rtp->data;
    change_id = GETLONG((ulong *)request->data);

    /*
     * Generate, fragment, and send a response.
     */
    newpak = vsrtp_generate_update(VSRTP_CHANGE_UPDATE, "change",
				   VSRTP_UPD_TOTALBYTES, change_id, path->idb);
    if (!newpak)
	return(FSM_OK);
    if (!vsrtp_fragment_update(&queue, newpak, VSRTP_UPD_RESPONSE,
			       path->metric, VSRTP_UPD_FLAG_CHANGE))
	return(FSM_OK);
    vsrtp_send_fragments(&queue, path, NULL, FALSE);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ c
 *
 * state:  several
 * event:  net rx request full topology update
 * action: Send an RTP response containing full routing tables
 */

static action_t vsrtp_action_c (vsrtp_fsm_data *data, const *dummy)
{
    paktype *newpak;
    queuetype queue;
    vinespathtype *path;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;

    /*
     * Generate, fragment, and send a response.
     */
    newpak = vsrtp_generate_update(VSRTP_FULL_UPDATE, "full",
				   VSRTP_UPD_TOTALBYTES, 0, path->idb);
    if (!newpak)
	return(FSM_OK);
    if (!vsrtp_fragment_update(&queue, newpak,VSRTP_UPD_RESPONSE, path->metric,
			       VSRTP_UPD_FLAG_FULL))
	return(FSM_OK);
    vsrtp_send_fragments(&queue, path, NULL, FALSE);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ d
 *
 * state:  several
 * event:  net rx request response only
 * action: Send an RTP response containing no information
 */

static action_t vsrtp_action_d (vsrtp_fsm_data *data, const *dummy)
{
    paktype *newpak;
    queuetype queue;
    vinespathtype *path;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;
    vsrtp_rx_request_slr_count++;

    /*
     * Generate, fragment, and send a response.
     */
    newpak = vsrtp_generate_update(VSRTP_NULL_UPDATE, "null", 0, 0, path->idb);
    if (!newpak)
	return(FSM_OK);
    if (!vsrtp_fragment_update(&queue, newpak,VSRTP_UPD_RESPONSE, path->metric,
			       VSRTP_UPD_FLAG_NULL))
	return(FSM_OK);
    vsrtp_send_fragments(&queue, path, NULL, FALSE);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ e
 *
 * state:  up
 * event:  net rx null/change/full update in sequence
 * action: Record the sending server's sequence number, and process all
 *         the information tuples contained in the update.
 */

static action_t vsrtp_action_e (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinesiptype *vip;
    vsrtp_header *rtp;
    vsrtp_update *update;
    vsrtp_entry *entry, *end;
    vinesneighbortype *neighbor;
    vinespathtype *path;
    sys_timestamp start_time;
    ulong net;
    ushort metric, metric1;
    boolean do_debug, changes_made;
    uchar flags;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;
    pak = data->pak;
    neighbor = path->neighbor;

    /*
     * Extract information from the received SRTP update message.
     */
    vip = (vinesiptype *)pak->network_start;
    rtp = (vsrtp_header *)pak->transport_start;
    update = (vsrtp_update *)rtp->data;
    if (vines_is_lan(path->idb))
	flags = VSRTP_ENTRY_LAN;
    else
	flags = VSRTP_ENTRY_WAN;

    /*
     * Now update the local data structures.  The local timestamp is
     * updated after the data in the frame is processed.  We won't know
     * until then if it needs to be changed.  The neighbor's metric based
     * upon the information contained in the update.  His view of the
     * metric maybe different from our view of the network, and a
     * consistent view is necessary.  This is common on X.25 networks
     * where the line speeds of the connected devices are independent.
     * The difference in metrics can be huge.  For example a 64K bps X.25
     * connection has a metric of 55, while a 1200 bps cinnection as a
     * metric of 460.
     */
    GET_TIMESTAMP(path->timer);
    path->srtp_flags = flags;
    path->origin_timestamp = GETLONG(&update->sequence);
    path->packet_id = update->packet_id;
    metric = vmetric_from_srtp(update->metric);
    metric1 = vines_compute_metric(pak);
    metric = max(metric, metric1);
    if (metric != path->metric) {
	path->metric = metric;
	vines_fixup_neighbor(neighbor);
    }

    /*
     * Get set to read the entries from the received update.
     */
    GET_TIMESTAMP(start_time);
    entry = (vsrtp_entry *)update->data;
    end = (vsrtp_entry *)((uchar *)vip + vip->len);
    do_debug = do_vines_rtp_v_debug(neighbor->net, neighbor->host);

    /*
     * Suck the information out of the routing update and use it to
     * update the entries in our internal tables.
     */
    if (do_debug) {
	buginf("\n    update: type %02x, flags %02x, id %04x, ofst %04x, seq %08x, met %04x",
	       update->type, update->flags, update->packet_id,
	       update->data_offset, GETLONG(&update->sequence),
	       update->metric);
    }

    /*
     * Stop here for client updates.  Everything else is server
     * specific.
     */
    if (neighbor->is_router == FALSE)
	return(FSM_OK);

    /*
     * Extract routing information from this message.  The sender of an
     * update should never be listed explicitly in the update.
     */
    if (do_debug)
	buginf("\n    network %#Z from the server", neighbor->net);
    changes_made = vsrtp_update_route(neighbor->net, 0,
				      GETLONG(&update->sequence),
				      flags, path);
    mem_lock(path);
    vneigh_ProtectNeighbor(neighbor);
    for ( ; entry < end; entry++) {
	net = entry->net;
	metric = vmetric_from_srtp(entry->metric);
	if (do_debug)
	    buginf(vines_network_metric_seq_flags_msg,
		   entry->net, metric, GETLONG(&entry->sequence),
		   entry->flags);
	if (net == neighbor->net) {
	    if (do_debug)
		buginf(" (invalid entry)");
	    continue;
	}
	if (path->idb->input_network_filter &&
	    !vines_short_access_check(net, VINES_SERVER,
				      path->idb->input_network_filter)) {
	    if (do_debug)
		buginf(" (failed filter)");
	    continue;
	}
	if ((entry->metric != 0) && vines_valid_net(net)) {
	    changes_made |= vsrtp_update_route(net, metric,
					       GETLONG(&entry->sequence),
					       entry->flags, path);
	}
	process_may_suspend();
    }
    vneigh_UnprotectNeighbor(neighbor);
 
    /*
     * If any changes were made to the database (other than origin
     * timestamps) as a result of this update, then the timestamp needs
     * to be updated.  Also clear the retry timer.
     */
    if (changes_made) {
	path->local_timestamp = vines_sequence + 1;
	vines_bump_changed_timer();
    }
    path->retry_count = 0;
    mgd_timer_stop(&path->retry_timer);

    /*
     * decrement the path memory block reference count or
     * free it if no one is using it.
     */
    free(path);

    /*
     * Now purge any routes via this neighbor that haven't been updated
     * since we started processing this packet.
     */
    if (update->flags & VSRTP_UPD_FLAG_SYNC)
	vsrtp_rx_resync_count++;
    if (update->flags & VSRTP_UPD_FLAG_FULL)
	vines_poison_routes_by_neighbor(neighbor, start_time);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ f
 *
 * state:  several
 * event:  net rx redirect message
 * action: Process this information in this redirect
 */

static const char vsrtp_red_bad_node[] = "\nVSRTP: Redirect shows %s %#z's as a %s.";

static action_t vsrtp_action_f (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinesiptype *vip;
    vsrtp_header *rtp;
    vsrtp_redirect *redirect;
    vinesneighbortype *neighbor, *destination, *sender;
    vinespathtype *new_path, *sender_path;
    hwaddrtype dlc_address;
    vinesidbtype *idb;
    srbroute_t *newrif;
    riftype *rif;
    vinesroutetype *route;
    vinesnettype *server;
    uchar local[SRB_MAXRIF];
    ulong nbr_net, dst_net;
    ulong nbr_metric, dst_metric;
    int nbr_version;
    boolean do_debug;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);

    /*
     * Get set to read the redirect.
     */
    sender_path = data->path;
    pak = data->pak;
    idb = vines_getidb(pak->if_input);
    vip = (vinesiptype *)pak->network_start;
    rtp = (vsrtp_header *)pak->transport_start;
    redirect = (vsrtp_redirect *)rtp->data;

    /*
     * Make sure the redirect does not reference this router.  If so, it
     * needs to be dumped.  This happens because a VINES server sends a
     * redirect messages as a MAC layer broadcast.  This means that any
     * redirect send to another station redirecting them to this router
     * will also be heard by this router.
     */
    nbr_metric = vmetric_from_srtp(redirect->nbr_metric);
    dst_metric = vmetric_from_srtp(redirect->dst_metric);
    nbr_net = GETLONG(redirect->nbr_net);
    dst_net = GETLONG(redirect->dst_net);
    do_debug = do_vines_rtp_v_debug(GETLONG(vip->ssrcnet), vip->srchost);
    if (do_debug) {
	buginf("\nVSRTP: redirecting data for %#z(%04x) to %#z(%04x)",
	       dst_net, redirect->dst_host, dst_metric,
	       nbr_net, redirect->nbr_host, nbr_metric);
	buginf("\nVRTP: Mac %e, Rif %s", redirect->nbr_mac,
	       redirect->nbr_riflen ? get_rif_str(redirect->nbr_rif, rif_string) : "none");
    }
    sender = vneigh_FindEntry(GETLONG(vip->ssrcnet), vip->srchost);
    destination = vneigh_FindEntry(dst_net, redirect->dst_host);
    neighbor = vneigh_FindEntry(nbr_net, redirect->nbr_host);
    if (destination && (destination->newflags & V_N_SELF))
	return(FSM_OK);
    if (neighbor && (neighbor->newflags & V_N_SELF))
	return(FSM_OK);
    if (sender_path->idb->input_network_filter &&
	!vines_short_access_check(dst_net, redirect->dst_host,
				  sender_path->idb->input_network_filter)) {
	if (do_debug)
	    buginf("\nVSRTP: Redirect discarded, failed filter");
	return(FSM_OK);
    }

    /*
     * Make noise about Banyan bugs and then correct them.
     */
    if ((redirect->nbr_host == VINES_SERVER) &&
	(redirect->nbr_node != VSRTP_NODE_ROUTER)) {
	redirect->nbr_node = VSRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vsrtp_red_bad_node, "server", nbr_net,
		   redirect->nbr_host, "client");
    } else if ((redirect->nbr_host != VINES_SERVER) &&
	       (redirect->nbr_node != VSRTP_NODE_CLIENT)) {
	redirect->nbr_node = VSRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vsrtp_red_bad_node, "client", nbr_net,
		   redirect->nbr_host, "server");
    }
    if ((redirect->dst_host == VINES_SERVER) &&
	(redirect->dst_node != VSRTP_NODE_ROUTER)) {
	redirect->dst_node = VSRTP_NODE_ROUTER;
	if (banyan_bugs_debug)
	    buginf(vsrtp_red_bad_node, "server", nbr_net,
		   redirect->nbr_host, "client");
    } else if ((redirect->dst_host != VINES_SERVER) &&
	       (redirect->dst_node != VSRTP_NODE_CLIENT)) {
	redirect->dst_node = VSRTP_NODE_CLIENT;
	if (banyan_bugs_debug)
	    buginf(vsrtp_red_bad_node, "client", nbr_net,
		   redirect->nbr_host, "server");
    }

    /*
     * Create the neighbor entry if necessary.
     */
    nbr_version = (redirect->nbr_flags & VSRTP_ENTRY_NO_SEQ) ? VRTP_VERSION : VSRTP_VERSION;
    if (!neighbor) {
	neighbor = vines_add_neighbor(nbr_net, redirect->nbr_host,
				      (V_N_DYNAMIC | V_N_REDIRECT), VINES_INFINITY,
				      nbr_version, redirect->nbr_node);  
	memset(&dlc_address, 0, sizeof(hwaddrtype));
	dlc_address.type = STATION_IEEE48;
	dlc_address.length = STATIONLEN_IEEE48;
	dlc_address.target_type = TARGET_UNICAST;
	ieee_copy(redirect->nbr_mac, dlc_address.addr);
	new_path = vines_add_path(neighbor, (V_N_DYNAMIC | V_N_REDIRECT), idb,
				  idb->vines_enctype, &dlc_address, nbr_metric,
				  nbr_version, redirect->nbr_node);
	if (!new_path || !neighbor->available_paths) {
	    vines_delete_neighbor_entry(neighbor);
	    return(FSM_OK);
	}
	if (nbr_version == VSRTP_VERSION) {
	    if (redirect->nbr_node == VSRTP_NODE_ROUTER) {
		new_path->state = vsrtp_st_init;
	    } else {
		new_path->state = vsrtp_st_up;
	    }
	    new_path->srtp_flags = redirect->nbr_flags;
	    new_path->origin_timestamp = GETLONG(redirect->nbr_sequence);
	    new_path->local_timestamp = vines_sequence + 1;
	}
	ieee_copy(redirect->nbr_mac, new_path->dlc_addr.addr);
	if (using_multiring(pak->if_input, LINK_VINES)) {
	    rif = rif_lookup(new_path->dlc_addr.addr, 
			     hwidb_or_null(pak->if_input), NULL, 0);
	    if (!rif) {
		/*
		 * Don't already have a rif.  Merge the rif for the
		 * source address and the rif included inside of the
		 * redirect frame.
		 */
		rif = rif_lookup(sender_path->dlc_addr.addr,
				 hwidb_or_null(pak->if_input), NULL, 0);
		if (!redirect->nbr_riflen) {
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
			newrif = (srbroute_t *)redirect->nbr_rif;
		    } else {
			/*
			 * If you need a rif to reach the sender, and there
			 * is a rif included in the redirect, then the target
			 * should be on the local ring.  Banyan will not send
			 * a redirect unless this is true.
			 */
			newrif = NULL;
		    }
		}
		rif_update(hwidb_or_null(pak->if_input), 0, 
			   new_path->dlc_addr.addr, NULL,
			   newrif, RIF_AGED);
	    }
	}
	vines_fixup_neighbor(neighbor);
    }

    /*
     * Was this a client redirect?  If so, we are done.  Also protect
     * against adding an entry for the local router's address.
     */
    if (redirect->dst_host != VINES_SERVER)  /* about a network */
	return(FSM_OK);
    if (redirect->nbr_host != VINES_SERVER)  /* via a router */
	return(FSM_OK);
    if ((dst_net == vines_network) || (nbr_net == vines_network))
	return(FSM_OK);

    /*
     * Add the new route for the server.  Calculate the metric value that
     * should be passed to the add_route subroutine to insure the metric
     * value that ends up in the route table is the same as the
     * redirecting router's metric to the destination.  Once the new
     * route is installed, delete any old routes that point at the
     * neighbor that sent the redirect.  They are abviously no longer
     * good, or we wouldn't have received a redirect in the first place.
     * The only purpose for the first lookup is so that the correct
     * counter gets incremented.
     */
    server = vines_add_server(dst_net, (V_NET_RTP | V_NET_REDIRECT));
    route = vines_get_route_by_idb(server, idb);
    if (route) {
	vroute_redirect_modified++;
    } else {
	vroute_redirect_created++;
    }
    route = vines_add_route(server, neighbor, (V_NET_RTP | V_NET_REDIRECT),
			    dst_metric - neighbor->metric);
    if (route) {
	route->rtp_flags = redirect->dst_flags;
	route->origin_timestamp = GETLONG(redirect->dst_sequence);
	route->local_timestamp = vines_sequence + 1;
	server = vines_delete_route_by_xxx(server, NULL, V_NET_RTP,
					   dst_metric, timer_no_time);
    }
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ g
 *
 * state:  several
 * event:  net rx reinit message
 * action: Purge all previously known information from the sender.
 */

static action_t vsrtp_action_g (vsrtp_fsm_data *data, const *dummy)
{
    vinespathtype *path;
    vinesneighbortype *neighbor;
    sys_timestamp null_time;

    /*
     * Check inputs
     */
    if (!data || !data->pak || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;
    neighbor = path->neighbor;

    /*
     * "Purge" any routes via this neighbor.  Mark them all as
     * unreachable and then force a change update.  This keeps the rest
     * of the network in sync with the changes caused by the reinit
     * packet.
     */
    TIMER_STOP(null_time);
    vines_poison_routes_by_neighbor(neighbor, null_time);
    path->origin_timestamp = 0;
    path->retry_count = VSRTP_REQUEST_COUNT;
    vines_flush_queue(&path->reassemblyQ);
    mgd_timer_stop(&path->reassembly_timer);
    mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ h
 *
 * state:  several
 * event:  out of sequence message, failed reassembly of change update
 * action: Send a request for changes.  Set up fields to retry request.
 */

static action_t vsrtp_action_h (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinespathtype *path;
    vsrtp_request *request;
    vinesidbtype *idb;

    /*
     * Check inputs.  Data->pak may or may not point to a packet.  It
     * will be NULL if this routine was called because a packet failed to
     * reassemble.
     */
    if (!data || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;

    /*
     * General housekeeping
     */
    if (mgd_timer_running(&path->retry_timer))
	return(FSM_OK);
    path->retry_count = VSRTP_REQUEST_COUNT;
    mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);

    /*
     * Build and send the request
     */
    pak = vsrtp_getbuffer("change request", VSRTP_REQUEST_BYTES);
    if (!pak)
	return(FSM_OK);
    request = (vsrtp_request*)pak->session_start;
    request->type = VSRTP_REQ_CHANGES;
    request->reserved = 0;
    request->data[0] = path->origin_timestamp;
    idb = vsrtp_send_specific(pak, VSRTP_REQUEST, path);
    path->srtp_tx_req_counts[VSRTP_REQ_CHANGES-1]++;
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ i
 *
 * state:  several
 * event:  out of sequence message, failed reassembly of full update
 * action: Send a request for full.  Set up fields to retry request.
 */

static action_t vsrtp_action_i (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinespathtype *path;
    vsrtp_request *request;
    vinesidbtype *idb;

    /*
     * Check inputs  Data->pak may or may not point to a packet.  It
     * will be NULL if this routine was called because a packet failed to
     * reassemble.
     */
    if (!data || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;

    /*
     * General Houskeeping
     */
    if (mgd_timer_running(&path->reassembly_timer) ||
	mgd_timer_running(&path->retry_timer))
	return(FSM_OK);
    path->retry_count = VSRTP_REQUEST_COUNT;
    mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);

    /*
     * Build and send the request
     */
    pak = vsrtp_getbuffer("full request", VSRTP_REQUEST_BYTES);
    if (!pak)
	return(FSM_OK);
    request = (vsrtp_request*)pak->session_start;
    request->type = VSRTP_REQ_FULL;
    request->reserved = 0;
    PUTLONG(&request->data[0], path->origin_timestamp);
    idb = vsrtp_send_specific(pak, VSRTP_REQUEST, path);
    path->srtp_tx_req_counts[VSRTP_REQ_FULL-1]++;
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ j
 *
 * state:  several
 * event:  timer expired
 * action: Re-send a request for changes.
 */

static action_t vsrtp_action_j (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinespathtype *path;
    vsrtp_request *request;
    vinesidbtype *idb;

    /*
     * Check inputs
     */
    if (!data || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;

    /*
     * Set up for next time.
     */
    path->retry_count--;
    mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);

    /*
     * Build and send the request
     */
    pak = vsrtp_getbuffer("change request", VSRTP_REQUEST_BYTES);
    if (!pak)
	return(FSM_OK);
    request = (vsrtp_request*)pak->session_start;
    request->type = VSRTP_REQ_CHANGES;
    request->reserved = 0;
    request->data[0] = path->origin_timestamp;
    idb = vsrtp_send_specific(pak, VSRTP_REQUEST, path);
    path->srtp_tx_req_counts[VSRTP_REQ_CHANGES-1]++;
    return(FSM_OK);
}

/*
 * v s r t p _ a c t i o n _ k
 *
 * state:  several
 * event:  timer expired
 * action: Re-send a request for all changes (aka full update).
 */

static action_t vsrtp_action_k (vsrtp_fsm_data *data, const *dummy)
{
    paktype *pak;
    vinespathtype *path;
    vsrtp_request *request;
    vinesidbtype *idb;

    /*
     * Check inputs
     */
    if (!data || !data->path)
	return(FSM_RC_DATA_ERROR);
    path = data->path;

    /*
     * Set up for next time.
     */
    if (mgd_timer_running(&path->reassembly_timer)) {
	mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);
	return(FSM_OK);
    }
    path->retry_count--;
    mgd_timer_start(&path->retry_timer, VSRTP_REQUEST_TIME);

    /*
     * Build and send the request
     */
    pak = vsrtp_getbuffer("full request", VSRTP_REQUEST_BYTES);
    if (!pak)
	return(FSM_OK);
    request = (vsrtp_request*)pak->session_start;
    request->type = VSRTP_REQ_FULL;
    request->reserved = 0;
    request->data[0] = path->origin_timestamp;
    idb = vsrtp_send_specific(pak, VSRTP_REQUEST, path);
    path->srtp_tx_req_counts[VSRTP_REQ_FULL-1]++;
    return(FSM_OK);
}

/******************************************************************************
 *
 *                              V S R T P _ F S M 
 *
 * 		     	      S t a t e   T a b l e s
 *
 *****************************************************************************/

/*
 * Damn casting!
 */
#define VSRTP_ACTION_a FSM_ACTION_ROUTINE(vsrtp_action_a)
#define VSRTP_ACTION_b FSM_ACTION_ROUTINE(vsrtp_action_b)
#define VSRTP_ACTION_c FSM_ACTION_ROUTINE(vsrtp_action_c)
#define VSRTP_ACTION_d FSM_ACTION_ROUTINE(vsrtp_action_d)
#define VSRTP_ACTION_e FSM_ACTION_ROUTINE(vsrtp_action_e)
#define VSRTP_ACTION_f FSM_ACTION_ROUTINE(vsrtp_action_f)
#define VSRTP_ACTION_g FSM_ACTION_ROUTINE(vsrtp_action_g)
#define VSRTP_ACTION_h FSM_ACTION_ROUTINE(vsrtp_action_h)
#define VSRTP_ACTION_i FSM_ACTION_ROUTINE(vsrtp_action_i)
#define VSRTP_ACTION_j FSM_ACTION_ROUTINE(vsrtp_action_j)
#define VSRTP_ACTION_k FSM_ACTION_ROUTINE(vsrtp_action_k)

#define VSRTP_FSM_DECODE FSM_DECODE_ROUTINE(vsrtp_fsm_decode)

/*
 *  VSRTP_FSM State = "vsrtp_st_up"
 */

const static struct fsm_state vsrtp_state_up[VSRTP_MAX_EVENT_NUM] =
{
/*  Action        Arguments  Next State           Index  Decoded Input               */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Request Events                */
{VSRTP_ACTION_a,  FSM_NOARG, FSM_NOCHANGE},     /*  0, vsrtp_rx_req_specific         */
{VSRTP_ACTION_b,  FSM_NOARG, FSM_NOCHANGE},     /*  1, vsrtp_rx_req_changes          */
{VSRTP_ACTION_c,  FSM_NOARG, FSM_NOCHANGE},     /*  2, vsrtp_rx_req_full             */
{VSRTP_ACTION_d,  FSM_NOARG, FSM_NOCHANGE},     /*  3, vsrtp_rx_req_response         */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Update Events                 */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  4, vsrtp_rx_upd_null_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  5, vsrtp_rx_upd_null_less        */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /*  6, vsrtp_rx_upd_null_equal       */
{VSRTP_ACTION_h,  FSM_NOARG, vsrtp_st_chgrqst}, /*  7, vsrtp_rx_upd_null_onemore     */
{VSRTP_ACTION_h,  FSM_NOARG, vsrtp_st_chgrqst}, /*  8, vsrtp_rx_upd_null_greater     */
{VSRTP_ACTION_h,  FSM_NOARG, vsrtp_st_chgrqst}, /*  9, vsrtp_rx_upd_change_failed    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 10, vsrtp_rx_upd_change_less      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 11, vsrtp_rx_upd_change_equal     */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 12, vsrtp_rx_upd_change_onemore   */
{VSRTP_ACTION_h,  FSM_NOARG, vsrtp_st_chgrqst}, /* 13, vsrtp_rx_upd_change_greater   */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 14, vsrtp_rx_upd_full_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 15, vsrtp_rx_upd_full_less        */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 16, vsrtp_rx_upd_full_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 17, vsrtp_rx_upd_full_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 18, vsrtp_rx_upd_full_greater     */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 19, vsrtp_rx_upd_sync_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 20, vsrtp_rx_upd_sync_less        */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 21, vsrtp_rx_upd_sync_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 22, vsrtp_rx_upd_sync_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 23, vsrtp_rx_upd_sync_greater     */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Response Events               */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 24, vsrtp_rx_resp_null_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 25, vsrtp_rx_resp_null_less       */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 26, vsrtp_rx_resp_null_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 27, vsrtp_rx_resp_null_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 28, vsrtp_rx_resp_null_greater    */
{VSRTP_ACTION_h,  FSM_NOARG, vsrtp_st_chgrqst}, /* 29, vsrtp_rx_resp_change_failed   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 30, vsrtp_rx_resp_change_less     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 31, vsrtp_rx_resp_change_equal    */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 32, vsrtp_rx_resp_change_onemore  */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 33, vsrtp_rx_resp_change_greater  */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 34, vsrtp_rx_resp_full_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 35, vsrtp_rx_resp_full_less       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 36, vsrtp_rx_resp_full_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 37, vsrtp_rx_resp_full_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 38, vsrtp_rx_resp_full_greater    */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 39, vsrtp_rx_resp_sync_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 40, vsrtp_rx_resp_sync_less       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 41, vsrtp_rx_resp_sync_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 42, vsrtp_rx_resp_sync_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, FSM_NOCHANGE},     /* 43, vsrtp_rx_resp_sync_greater    */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Redirect Events               */
{VSRTP_ACTION_f,  FSM_NOARG, FSM_NOCHANGE},     /* 44, vsrtp_rx_redirect             */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP ReInit Events                 */
{VSRTP_ACTION_g,  FSM_NOARG, vsrtp_st_init},    /* 45, vsrtp_rx_reinit               */
/*-----------------------------------------------------------------------------------*/
                                                /* Timer events                      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 46, vsrtp_timer_retry             */
{FSM_NOACTION,    FSM_NOARG, vsrtp_st_init}     /* 47, vsrtp_timer_retry_final       */
/*-----------------------------------------------------------------------------------*/
};


/*
 *  VSRTP_FSM State = "vsrtp_st_chgrqst"
 */

const static struct fsm_state vsrtp_state_chgrqst[VSRTP_MAX_EVENT_NUM] =
{
/*  Action        Arguments  Next State           Index  Decoded Input               */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Request Events                */
{VSRTP_ACTION_a,  FSM_NOARG, FSM_NOCHANGE},     /* 0,  vsrtp_rx_req_specific         */
{VSRTP_ACTION_b,  FSM_NOARG, FSM_NOCHANGE},     /* 1,  vsrtp_rx_req_changes          */
{VSRTP_ACTION_c,  FSM_NOARG, FSM_NOCHANGE},     /* 2,  vsrtp_rx_req_full             */
{VSRTP_ACTION_d,  FSM_NOARG, FSM_NOCHANGE},     /* 3,  vsrtp_rx_req_response         */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Update Events                 */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  4, vsrtp_rx_upd_null_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  5, vsrtp_rx_upd_null_less        */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  6, vsrtp_rx_upd_null_equal       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  7, vsrtp_rx_upd_null_onemore     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  8, vsrtp_rx_upd_null_greater     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  9, vsrtp_rx_upd_change_failed    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 10, vsrtp_rx_upd_change_less      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 11, vsrtp_rx_upd_change_equal     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 12, vsrtp_rx_upd_change_onemore   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 13, vsrtp_rx_upd_change_greater   */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 14, vsrtp_rx_upd_full_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 15, vsrtp_rx_upd_full_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 16, vsrtp_rx_upd_full_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 17, vsrtp_rx_upd_full_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 18, vsrtp_rx_upd_full_greater     */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 19, vsrtp_rx_upd_sync_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 20, vsrtp_rx_upd_sync_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 21, vsrtp_rx_upd_sync_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 22, vsrtp_rx_upd_sync_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 23, vsrtp_rx_upd_sync_greater     */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Response Events               */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 24, vsrtp_rx_resp_null_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 25, vsrtp_rx_resp_null_less       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 26, vsrtp_rx_resp_null_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 27, vsrtp_rx_resp_null_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 28, vsrtp_rx_resp_null_greater    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 29, vsrtp_rx_resp_change_failed   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 30, vsrtp_rx_resp_change_less     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 31, vsrtp_rx_resp_change_equal    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 32, vsrtp_rx_resp_change_onemore  */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 33, vsrtp_rx_resp_change_greater  */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 34, vsrtp_rx_resp_full_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 35, vsrtp_rx_resp_full_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 36, vsrtp_rx_resp_full_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 37, vsrtp_rx_resp_full_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 38, vsrtp_rx_resp_full_greater    */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 39, vsrtp_rx_resp_sync_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, vsrtp_st_fullrqst},/* 40, vsrtp_rx_resp_sync_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 41, vsrtp_rx_resp_sync_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 42, vsrtp_rx_resp_sync_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 43, vsrtp_rx_resp_sync_greater    */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Redirect Events               */
{VSRTP_ACTION_f,  FSM_NOARG, FSM_NOCHANGE},     /* 44, vsrtp_rx_redirect             */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP ReInit Events                 */
{VSRTP_ACTION_g,  FSM_NOARG, vsrtp_st_init},    /* 45, vsrtp_rx_reinit               */
/*-----------------------------------------------------------------------------------*/
                                                /* Timer events                      */
{VSRTP_ACTION_j,  FSM_NOARG, FSM_NOCHANGE},     /* 46, vsrtp_timer_retry             */
{FSM_NOACTION,    FSM_NOARG, vsrtp_st_init}     /* 47, vsrtp_timer_retry_final       */
/*-----------------------------------------------------------------------------------*/
};


/*
 *  VSRTP_FSM State = "vsrtp_st_fullrqst"
 */

const static struct fsm_state vsrtp_state_fullrqst[VSRTP_MAX_EVENT_NUM] =
{
/*  Action        Arguments  Next State           Index  Decoded Input               */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Request Events                */
{VSRTP_ACTION_a,  FSM_NOARG, FSM_NOCHANGE},     /* 0,  vsrtp_rx_req_specific         */
{VSRTP_ACTION_b,  FSM_NOARG, FSM_NOCHANGE},     /* 1,  vsrtp_rx_req_changes          */
{VSRTP_ACTION_c,  FSM_NOARG, FSM_NOCHANGE},     /* 2,  vsrtp_rx_req_full             */
{VSRTP_ACTION_d,  FSM_NOARG, FSM_NOCHANGE},     /* 3,  vsrtp_rx_req_response         */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Update Events                 */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  4, vsrtp_rx_upd_null_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  5, vsrtp_rx_upd_null_less        */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  6, vsrtp_rx_upd_null_equal       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  7, vsrtp_rx_upd_null_onemore     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  8, vsrtp_rx_upd_null_greater     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /*  9, vsrtp_rx_upd_change_failed    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 10, vsrtp_rx_upd_change_less      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 11, vsrtp_rx_upd_change_equal     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 12, vsrtp_rx_upd_change_onemore   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 13, vsrtp_rx_upd_change_greater   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 14, vsrtp_rx_upd_full_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 15, vsrtp_rx_upd_full_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 16, vsrtp_rx_upd_full_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 17, vsrtp_rx_upd_full_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 18, vsrtp_rx_upd_full_greater     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 19, vsrtp_rx_upd_sync_failed      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 20, vsrtp_rx_upd_sync_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 21, vsrtp_rx_upd_sync_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 22, vsrtp_rx_upd_sync_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 23, vsrtp_rx_upd_sync_greater     */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Response Events               */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 24, vsrtp_rx_resp_null_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 25, vsrtp_rx_resp_null_less       */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 26, vsrtp_rx_resp_null_equal      */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 27, vsrtp_rx_resp_null_onemore    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 28, vsrtp_rx_resp_null_greater    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 29, vsrtp_rx_resp_change_failed   */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 30, vsrtp_rx_resp_change_less     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 31, vsrtp_rx_resp_change_equal    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 32, vsrtp_rx_resp_change_onemore  */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 33, vsrtp_rx_resp_change_greater  */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 34, vsrtp_rx_resp_full_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 35, vsrtp_rx_resp_full_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 36, vsrtp_rx_resp_full_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 37, vsrtp_rx_resp_full_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 38, vsrtp_rx_resp_full_greater    */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 39, vsrtp_rx_resp_sync_failed     */
{FSM_NOACTION,    FSM_NOARG, FSM_NOCHANGE},     /* 40, vsrtp_rx_resp_sync_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 41, vsrtp_rx_resp_sync_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 42, vsrtp_rx_resp_sync_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 43, vsrtp_rx_resp_sync_greater    */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Redirect Events               */
{VSRTP_ACTION_f,  FSM_NOARG, FSM_NOCHANGE},     /* 44, vsrtp_rx_redirect             */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP ReInit Events                 */
{VSRTP_ACTION_g,  FSM_NOARG, vsrtp_st_init},    /* 45, vsrtp_rx_reinit               */
/*-----------------------------------------------------------------------------------*/
                                                /* Timer events                      */
{VSRTP_ACTION_k,  FSM_NOARG, FSM_NOCHANGE},     /* 46, vsrtp_timer_retry             */
{FSM_NOACTION,    FSM_NOARG, vsrtp_st_init}     /* 47, vsrtp_timer_retry_final       */
/*-----------------------------------------------------------------------------------*/
};


/*
 *  VSRTP_FSM State = "vsrtp_st_init"
 */

const static struct fsm_state vsrtp_state_init[VSRTP_MAX_EVENT_NUM] =
{
/*  Action        Arguments  Next State           Index  Decoded Input               */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Request Events                */
{VSRTP_ACTION_a,  FSM_NOARG, FSM_NOCHANGE},     /* 0,  vsrtp_rx_req_specific         */
{VSRTP_ACTION_b,  FSM_NOARG, FSM_NOCHANGE},     /* 1,  vsrtp_rx_req_changes          */
{VSRTP_ACTION_c,  FSM_NOARG, FSM_NOCHANGE},     /* 2,  vsrtp_rx_req_full             */
{VSRTP_ACTION_d,  FSM_NOARG, FSM_NOCHANGE},     /* 3,  vsrtp_rx_req_response         */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Update Events                 */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  4, vsrtp_rx_upd_null_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  5, vsrtp_rx_upd_null_less        */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  6, vsrtp_rx_upd_null_equal       */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  7, vsrtp_rx_upd_null_onemore     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  8, vsrtp_rx_upd_null_greater     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /*  9, vsrtp_rx_upd_change_failed    */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 10, vsrtp_rx_upd_change_less      */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 11, vsrtp_rx_upd_change_equal     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 12, vsrtp_rx_upd_change_onemore   */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 13, vsrtp_rx_upd_change_greater   */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 14, vsrtp_rx_upd_full_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 15, vsrtp_rx_upd_full_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 16, vsrtp_rx_upd_full_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 17, vsrtp_rx_upd_full_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 18, vsrtp_rx_upd_full_greater     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 19, vsrtp_rx_upd_sync_failed      */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 20, vsrtp_rx_upd_sync_less        */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 21, vsrtp_rx_upd_sync_equal       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 22, vsrtp_rx_upd_sync_onemore     */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 23, vsrtp_rx_upd_sync_greater     */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Response Events               */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 24, vsrtp_rx_resp_null_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 25, vsrtp_rx_resp_null_less       */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 26, vsrtp_rx_resp_null_equal      */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 27, vsrtp_rx_resp_null_onemore    */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 28, vsrtp_rx_resp_null_greater    */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 29, vsrtp_rx_resp_change_failed   */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 30, vsrtp_rx_resp_change_less     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 31, vsrtp_rx_resp_change_equal    */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 32, vsrtp_rx_resp_change_onemore  */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 33, vsrtp_rx_resp_change_greater  */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 34, vsrtp_rx_resp_full_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 35, vsrtp_rx_resp_full_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 36, vsrtp_rx_resp_full_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 37, vsrtp_rx_resp_full_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 38, vsrtp_rx_resp_full_greater    */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 39, vsrtp_rx_resp_sync_failed     */
{VSRTP_ACTION_i,  FSM_NOARG, FSM_NOCHANGE},     /* 40, vsrtp_rx_resp_sync_less       */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 41, vsrtp_rx_resp_sync_equal      */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 42, vsrtp_rx_resp_sync_onemore    */
{VSRTP_ACTION_e,  FSM_NOARG, vsrtp_st_up},      /* 43, vsrtp_rx_resp_sync_greater    */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP Redirect Events               */
{VSRTP_ACTION_f,  FSM_NOARG, FSM_NOCHANGE},     /* 44, vsrtp_rx_redirect             */
/*-----------------------------------------------------------------------------------*/
                                                /* RTP ReInit Events                 */
{VSRTP_ACTION_g,  FSM_NOARG, vsrtp_st_init},    /* 45, vsrtp_rx_reinit               */
/*-----------------------------------------------------------------------------------*/
                                                /* Timer events                      */
{VSRTP_ACTION_k,  FSM_NOARG, FSM_NOCHANGE},     /* 46, vsrtp_timer_retry             */
{FSM_NOACTION,    FSM_NOARG, vsrtp_st_init}     /* 47, vsrtp_timer_retry_final       */
/*-----------------------------------------------------------------------------------*/
};



const struct finite_state_machine vsrtp_fsm_table[vsrtp_fsm_max] = 
{
    /*  State			Decoder			State Value */
        {vsrtp_state_up,	VSRTP_FSM_DECODE},	/*  VSRTP_Up         */
        {vsrtp_state_chgrqst,	VSRTP_FSM_DECODE},	/*  VSRTP_ChangeRqst */
	{vsrtp_state_fullrqst,	VSRTP_FSM_DECODE},	/*  VSRTP_FullRqst   */
	{vsrtp_state_init,	VSRTP_FSM_DECODE}	/*  VSRTP_Init       */
};

/******************************************************************************
 *
 *                              V S R T P _ F S M 
 *
 * 		     	        E x e c u t i o n
 *
 *****************************************************************************/

/*
 * vsrtp_fsm
 *
 * Interface to the vsrtp finite state machine
 *
 * major and minor indicate the event that has occured that we are
 * calling the fsm with.
 */

ushort vsrtp_fsm (
    paktype           *pak,
    vinespathtype     *path,
    vsrtp_major       major,
    int		      minor)
{
    int fsm_rc;
    vsrtp_state	old_state;
    vsrtp_fsm_data fn_data;
    boolean do_debug;

    fn_data.pak = pak;
    fn_data.path = path;
    old_state = path->state;
    do_debug = do_vines_rtp_debug(path->neighbor->net, path->neighbor->net);
    do_debug |= vines_state_debug;
    fsm_rc = (int)fsm_execute("VSRTP", vsrtp_fsm_table, &path->state,
			      vsrtp_fsm_max, &fn_data, major, minor, FALSE);
    if (fsm_rc < 0) {
	switch (fsm_rc) {
	  case FSM_RC_ERROR_INPUT:
	  case FSM_RC_INVALID_INPUT:
	    if (do_debug)
		buginf("\nVSRTP: Invalid input: %s %s %s",
		       vpath_id(path), vsrtp_prstate(old_state),
		       vsrtp_prinput(major, minor));
	    break;
	  case FSM_RC_UNKNOWN_STATE:
	    if (do_debug)
		buginf("\nVSRTP: illegal state: %s %s %s states: %d %d",
		       vpath_id(path), vsrtp_prstate(old_state),
		       vsrtp_prinput(major, minor), old_state, path->state);
	    break;
	  case FSM_RC_UNKNOWN_INPUT:
	    if (do_debug)
		buginf("\nVSRTP: illegal input: %s %s %s input: %d %d",
		       vpath_id(path), vsrtp_prstate(old_state),
		       vsrtp_prinput(major, minor), major, minor);
	    break;
	  case FSM_RC_DATA_ERROR:
	    if (do_debug)
		buginf("\nVSRTP: invalid data: %s %s %s input: %d %d",
		       vpath_id(path), vsrtp_prstate(old_state),
		       vsrtp_prinput(major, minor), major, minor);
	    break;
	  default:
	    if (do_debug)
		buginf("\nVSRTP: unknown error rtn code %d: %s %s %s input: %d %d  states: %d %d",
		       fsm_rc, vpath_id(path), vsrtp_prstate(old_state),
		       vsrtp_prinput(major, minor), major, minor, old_state, path->state);
	    break;
	}
    } else {
	if (do_debug)
	    buginf("\nVSRTP: %s %s -> %s, %s", vpath_id(path),
		   vsrtp_prstate(old_state), vsrtp_prstate(path->state),
		   vsrtp_prinput(major, minor));
    }
    return(fsm_rc);
}
