/* $Id: at_igrp2.c,v 3.10.18.14 1996/09/10 18:10:20 sluong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_igrp2.c,v $
 *------------------------------------------------------------------
 * IGRP2 AppleTalk specific routines.
 *
 * November 1992, David S.A. Stine, shamelessly stolen from Bob's IP.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2.c,v $
 * Revision 3.10.18.14  1996/09/10  18:10:20  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.10.18.13  1996/09/04  23:46:07  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.10.18.12  1996/08/28  12:36:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.18.11  1996/08/27  20:24:11  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.10.18.10  1996/06/29  00:06:50  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.10.18.9  1996/06/28  02:48:38  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.10.18.8  1996/06/28  02:39:57  dkatz
 * CSCdi61491:  EIGRP may lose routes
 * Branch: California_branch
 * Only ignore the routes we are supposed to ignore.
 *
 * Revision 3.10.18.7  1996/06/18  16:40:36  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.10.18.6  1996/06/17  23:28:57  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.10.18.5  1996/05/02  08:40:04  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.10.18.4  1996/04/07  05:19:45  dwong
 * CSCdi53892:  AT/EIGRP is broken
 * Branch: California_branch
 * Create a sub-idb block when it needs to.
 *
 * Revision 3.10.18.3  1996/04/02  05:35:38  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.10.18.2  1996/03/23  01:26:38  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.10.18.1  1996/03/18  18:52:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  01:06:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  08:28:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:17:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/08  03:18:36  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.9  1996/02/07  16:09:45  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  05:59:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/29  07:26:04  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1996/01/18  19:39:18  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/16  21:22:09  dwong
 * CSCdi46975:  Need to update cache entries when eigrp routes go away
 *
 * Revision 3.4  1995/12/07  22:27:17  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/22  02:53:46  dwong
 * CSCdi43029:  AT EIGRP active-time disabled
 *
 * Revision 3.2  1995/11/17  08:42:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  20:49:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/20  06:06:30  pst
 * CSCdi42495: Exorcise demons
 *
 * Revision 2.13  1995/10/17  00:47:00  kleung
 * CSCdi42168:  Router crash in dual_updatetopology() running AURP2 tests.
 *
 * Revision 2.12  1995/09/27  19:47:37  kleung
 * CSCdi41153:  read/write corruption in dci_memtest()
 *
 * Revision 2.11  1995/09/19  18:35:27  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.10  1995/09/07  17:02:19  mikel
 * CSCdi39889:  EIGRP: modularity violation
 * remove reference to IP specific variable
 *
 * Revision 2.9  1995/09/07  01:45:09  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers se
 *
 * Revision 2.8  1995/07/13  06:49:47  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.7  1995/07/05 22:24:20  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.6  1995/06/20  05:19:49  mikel
 * CSCdi36141:  EIGRP: random corruption caused by  linkdown event
 * failure to memlock the iidb during a linkdown_enq leads to corruption
 *
 * Revision 2.5  1995/06/19  06:43:17  bcole
 * CSCdi36032: IP-EIGRP: Connected route for shutdown passive-if stays in
 * topo tbl
 *
 * Revert CSCdi34952 changes.  During an if flap, consider
 * passive-interfaces before deciding that there must be no topology table
 * entries which need to be removed.
 *
 * Revision 2.4  1995/06/12  22:04:28  mikel
 * CSCdi35363:  EIGRP: ifcost_enq leaves connected entries in topology
 * table
 * Connected routes aren't properly cleaned up from the topology table
 *
 * Revision 2.3  1995/06/10  12:55:17  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.2  1995/06/09  12:58:31  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:08:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../iprouting/msg_igrp2.c"	/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "address.h"
#include <interface_private.h>
#include "config.h"
#include "../os/chunk.h"
#include "../iprouting/dual.h"
#include "../iprouting/route.h"
#include "atalk_private.h"
#include "../iprouting/igrp2-inlines.h"
#include "parser.h"
#include "parser_defs_atalk.h"
#include "../iprouting/parser_defs_igrp2.h"
#include "at_igrp2.h"
#include "at_registry.h"
#include "at_domain.h"
#include "at_igrp2_debug.h"

/*
 * Storage
 */
static watched_queue *atalkigrpQ;	/* appletalk IGRP2 protocol queue */

static ddbtype	*atalk_igrp2_ddb;	/* static ddb; we have only one AS */
static atalk_ipb_t *atalk_igrp2_ipb;	/* static ipb; similar to a IP pdb */

static void atigrp2_start (void);

/*
 * atigrp2_qevent
 * Enqueue work queue event, and
 */
static void atigrp2_qevent (atalk_ipb_t *ipb, atigrp2_worktype *work,
			    igrp2_eventtype type)
{
    work->type = type;
    process_enqueue(atalk_igrp2_ipb->workQ, work);
}

/*
 * atigrp2_PathUse
 *
 * This is called when the routing module wants to tell us whether or not
 * it is using a EIGRP path, rejecting the offered use of a path or is
 * done using a path.
 */
static boolean
atigrp2_PathUse (atalk_pdb_t *p, at_path_use_t usage)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    igrp2nettype dest;

    if (p == NULL || ddb == NULL)
	return (FALSE);

    dest.atalk_network.start = p->dstrngstr;
    dest.atalk_network.end = p->dstrngend;

    switch (usage) {
      case at_path_use:
	/*
	 * AppleTalk is using the path in question.
	 */
	break;
      case at_path_unuse:
	/*
	 * AppleTalk is no longer using the path in question.
	 */
	dual_lostroute(ddb, &dest);
	break;
      case at_path_reject:
	/*
	 * AppleTalk is rejecting the path in question.
	 */
	break;
      default:
	break;
    }
    return (TRUE);
}

/*
 * atigrp2_process_routes
 * Grab metric, compute composite metric.  Then get destinations/masks
 * from packet and fire off call to dual.  Return FALSE if we run out of
 * memory, or the packet has bogus data.
 */
static boolean atigrp2_process_routes (atigrp2_mpdecode *atd,
				       uchar *limit,
				       dual_newroute *route,
				       igrp2_mdecode *metric,
				       ddbtype *ddb,
				       ulong pkt_type,
				       boolean ignore_tlv,
				       ataddrtype address,
				       dual_peertype *peer,
				       void *extd)
{
    atigrp2_extdatatype *data;
    ataddrtype source;
    atalkidbtype *idb;
    ushort cable_high, cable_low;
    at_RemapTable *remap;
    ushort remap_net;

    source = peer->source.atalk_address;
    igrp2_metric_decode(&route->new_vecmetric, metric);
    route->new_metric = igrp2_compute_metric(ddb, &route->new_vecmetric,
					     &route->new_dest,
					     route->new_iidb,
					     &route->new_successor_metric);
    idb = atalk_getidb(dual_idb(route->new_iidb));
    if (idb->atalk_distance)
	route->new_metric += idb->atalk_distance;

    while (atd < (atigrp2_mpdecode *)limit) {
	route->new_ignore = ignore_tlv;
	cable_high = GETSHORT(&atd->cable_hi);
	cable_low  = GETSHORT(&atd->cable_lo);
 	if(idb->atalk_dminfo) {
	    
 	    if (idb->atalk_route_filter_in
 		&& !atalk_routecheck(cable_low, cable_high,
				     idb->atalk_route_filter_in))
		route->new_ignore = TRUE;
	    
	    
	    if (atdomain_FilterNet(idb,cable_low,FromDomain)) {
		remap_net = atdomain_FindBackwardRemap(idb->atalk_dminfo->out,
						       cable_low);
		if (remap_net) {
		    if (atd->cable_hi)
			cable_high += remap_net - cable_low;
		    cable_low = remap_net;
		}
		
		route->new_ignore = TRUE;
	    }
	    
	    
	    if (!route->new_ignore) {
		remap = atdomain_CreateRemap(idb, cable_low, cable_high);
		if (remap) {
		    cable_low = remap->remap_start;
		    if (cable_high)
			cable_high = remap->remap_end;
		}
	    }
	}    
	
	route->new_nexthop.atalk_address = (address == 0) ? source : address;
	route->new_dest.atalk_network.start = cable_low;
	route->new_dest.atalk_network.end = cable_high;

	/* Copy the external data for external destinations. */

	if (route->new_external) {
	    data = chunk_malloc(ddb->extdata_chunk);
	    if (data == NULL) {
		return (FALSE);
	    }
	    bcopy(extd, data, sizeof(atigrp2_extdatatype));
	    route->new_data = data;
	} else {
	    route->new_data = NULL;
	}

	/*
	 * At this point check to see if the ignore flag should be
	 * set in the topology table.  Only consider distance
	 * less than MAXDISTANCE and must pass input filtering.
	 */
	if (!route->new_ignore) {
	    if (idb->atalk_route_filter_in
		&& !atalk_routecheck(cable_low, cable_high,
				     idb->atalk_route_filter_in)) {
		route->new_ignore = TRUE;
	    }
	}

	/*
	 * If we have heard a EIGRP path which has a path which
	 * we can't use in the RTMP world, then mash the
	 * > 15 hopcount portion of the metric to METRIC_INACCESSIBLE.
	 */
	if (route->new_vecmetric.delay == METRIC_INACCESSIBLE ||
	    route->new_vecmetric.bandwidth == METRIC_INACCESSIBLE)
	    route->new_vecmetric.hopcount = DDP_POISON_HOPCOUNT;

	dual_rcvpkt(ddb, peer, route, pkt_type);
	atd++;
    }
    return(TRUE);
}

/*
 * atigrp2_receive
 * Transform update, query or reply packet into internal data structures.
 */
static void 
atigrp2_receive (ddbtype *ddb, dual_peertype *peer, paktype *pak1,
		 igrp2type *igrp2)

{
    paktype	*pak = pak1;
    atalk_ipb_t *ipb;
    igrp2_gentlvtype *tlv;
    int bytes;
    uchar *limit;
    ushort length;
    dual_newroute route;
    atigrp2_externtype *ate;
    atigrp2_metrictype *atm;
    boolean ignore_tlv;

    bzero(&route, sizeof(route));

    ipb = ddb->pdb;
    tlv = igrp2_packet_data(igrp2);

    /*
     * Note: atalk_datalen is the length of the DDP data _without_
     * the DDP header.
     */
    bytes = pak->atalk_datalen - IGRP2HEADERBYTES;
    route.new_infosource = peer->source;
    route.new_iidb = peer->iidb;
    route.new_origin = IGRP2_ORG_IGRP2;

    while (bytes > 0) {
	/*
	 * Let's inject a little sanity checking.  This is the same case
	 * as default in the switch statement.  Remember we need to poke
	 * the transport for an ack.
	 */
	length = GETSHORT(&tlv->length);
	if (length < sizeof(igrp2_gentlvtype)) {
	    if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK,
			 peer->iidb->idb))
		buginf("\nAT: atigrp2_receive, runt tlv %d", length);
	    goto done;
	}

	limit = (uchar *)tlv + length;
	switch (GETSHORT(&tlv->type)) {
	  case ATIGRP2_TLV_METRIC:
	    atm = (atigrp2_metrictype *) tlv;
	    route.new_external = FALSE;
	    route.new_data = NULL;
	    if (!atigrp2_process_routes(&atm->dest, limit, &route,
					(igrp2_mdecode *)&atm->delay, ddb,
					igrp2->opcode, FALSE,
					GETLONG(&atm->nexthop),
					peer, NULL)) {
		goto done;
	    }
	    break;
	  case ATIGRP2_TLV_EXTERNAL:
	    route.new_external = TRUE;
	    ate = (atigrp2_externtype *) tlv;

	    /*
	     * If we are the source of this advertisement then reject it.
	     */
	    ignore_tlv = (GETLONG(&ate->routerid) ==
			  (ataddrtype)ddb->routerid.atalk_address);

	    if (!atigrp2_process_routes(&ate->dest, limit, &route,
					(igrp2_mdecode *)&ate->delay, ddb,
					igrp2->opcode, ignore_tlv,
					GETLONG(&ate->nexthop),
					peer, &ate->routerid)) {
		goto done;
	    }
	    break;
	  default:
	    /*
	     * Skip the unknown TLV
	     */
	    break;
	}
	bytes -= length;
	tlv = (igrp2_gentlvtype *) ((uchar *)tlv + length);
    }

  done:

    /* Finish up any Updates we felt it necessary to send. */

    dual_finish_update_send(ddb);
}

/*
 * atigrp2_InterfaceUpForIGRP
 * Return a stricter concept of interface 'up' for IGRP uses...
 */
static boolean atigrp2_InterfaceUpForIGRP (atalkidbtype *idb)
{
    return (    atalkif_InterfaceUp(idb)
	     && (atalk_igrp_running)
	     && (idb->atalk_rp & ATALK_IGRP_ENABLED));
}

static boolean atigrp2_InterfaceUp (idbtype *idb)
{
    return (atalkif_InterfaceUp(atalk_getidb(idb)));
}


/*
 * atigrp2_hellohook
 *
 * We get called here before a hello packet is sent. At this point, we
 * can either add more information to the hello packet or adjust what
 * has already been added.
 *
 * V1 -- the only thing we need to add is the configured cable range
 *       info for this interface. The other stuff we will get from
 * 	 a ZIP GNI. Note that is we really wanted to be CLEVER,
 *       we could put the ZIP GNI stuff into the HELLO packet
 *       and bring up a new link _very_ quickly. However,
 *       one has to ask how much traffic you are going to put
 *       into these packets which were supposed to be itty-bitty
 *       little things, so as to not run up high traffic counts.
 *
 * Assumptions:
 * - pak->atalk_datalen is not set yet.
 * - the DDP header begins at the line pak->network_start
 * - there is a fixed IGRP2 header already installed on the packet
 * - there is at least one TLV (the parameter TLV) in the packet
 *   and we are passed a pointer to where our TLV should go.
 * - we have at most 17 extra bytes that we can insert into a Atalk DDP
 *   hello; see the definition of IGRP2_MAX_NET_LAYER_HDR. ???hack
 */
static boolean atigrp2_hellohook (ddbtype *ddb, igrp2type *igrp_hdr, int *len,
				  idbtype *idb, uchar *tlv_ptr)
{
    igrp2_gentlvtype	*param_tlv;
    atigrp2_cabletype	*cable;
    uchar *cable_p;
    atalkidbtype *atalkidb;

    /*
     * Make paranoid check for all parameters
     */
    if (!(ddb && igrp_hdr && len && idb && tlv_ptr))
	return(FALSE);
    param_tlv = igrp2_packet_data(igrp_hdr);

    /*
     * If the first TLV seen isn't a hello, bail out since our
     * assumptions will probably be invalid.
     */
    if (   igrp_hdr->opcode != IGRP_OPC_HELLO
	|| GETSHORT(&param_tlv->type) != IGRP2_PARAMETER) {
	if (at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK, idb))
	    buginf("\nAT: atigrp2_hellohook; wrong packtype/param param");
	return(FALSE);
    }
    /*
     * The cable config TLV was originally misimplemented,
     * requiring the PARAMETER TLV to be first, and the CABLE_CONFIG TLV
     * to be second.
     * If there are already multiple TLVs, we must wedge the CABLE_CONFIG
     * TLV in the middle to be backwards compatible...
     */
    cable_p = (uchar *)param_tlv + GETSHORT(&param_tlv->length);
 
    /*
     * Make room for cable config TLV
     */
    if (cable_p != (uchar *)tlv_ptr)
 	memmove(cable_p + sizeof(atigrp2_cabletype), cable_p,
 	        (uchar *)tlv_ptr - cable_p);
     
    cable = (atigrp2_cabletype *)cable_p;
    PUTSHORT(&cable->type, ATIGRP2_TLV_CABLE_CONFIG);
    PUTSHORT(&cable->length, sizeof(atigrp2_cabletype));
    atalkidb = atalk_getidb(idb);
    PUTSHORT(&cable->cable_lo, atalkidb->atalk_cablestart);
    PUTSHORT(&cable->cable_hi, atalkidb->atalk_cableend);
    PUTLONG(&cable->routerid, atalk_rtr_id);
    *len += sizeof(atigrp2_cabletype);
    return(TRUE);
}

/*
 * atigrp2_write
 * Send a datagram. Assume all routes have been inserted into packet
 * and that pak->atalk_datalen has been set.
 *
 * We assume the following:
 * 1) the packet's checksum needs to be computed.
 * 2) interface != 0
 * 3) when peer == 0, then we are being called to send a
 *    broadcast packet; when whe have a peer, an explicit choice
 *    as to bcast/unicast will be made.
 *
 * Note that this routine does not free the supplied packet.
 */
static void 
atigrp2_write (igrp2type *igrp2, int bytes, dual_peertype *peer,
	       igrp2_idbtype *iidb, boolean priority)
{
    ataddrtype	dest;
    ushort	dstnet;
    uchar	dstnode;
    ddplong	*ddp;
    neighborentry *n;
    boolean	packet_accounting = TRUE; /* Assume this is a non-bcast pak */
    paktype *pak;
    idbtype *idb;
    atalkidbtype *atalkidb;

    if (iidb == NULL) {
	if (at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK, NULL))
	    buginf("\nAT: atigrp2_write, NULL iidb");
    }
    idb = iidb->idb;
    atalkidb = atalk_getidb(idb);

    /*
     * This is here because IGRP2's reliable transport can queue up packets
     * on the output queue and send these even after a state transition.
     * For instance, a hello packet could be waiting on the output queue
     * and we've just reset the interface and we're now in the startup
     * range. This will result in bogus neighbor entries with 1 hello
     * packet to their traffic count.
     */
    if (!atigrp2_InterfaceUpForIGRP(atalkidb)) {
	return;
    }
    pak = igrp2_getpak(DDP_OVERHEADBYTES + bytes);
    if (!pak) {
	return;
    }
    bcopy(igrp2, pak->network_start + DDP_OVERHEADBYTES, bytes);
    ddp = (ddplong *)pak->network_start;
    dest = (ataddrtype)((peer) ? peer->source.atalk_address : ATALK_BROAD_ADDR);
    dstnet = atalk_netonly(dest);
    dstnode = atalk_nodeonly(dest);

    if (priority)
	pak->flags |= PAK_PRIORITY;
    pak->linktype = LINK_APPLETALK;
    pak->atalk_dstmcast = ATALK_UNICAST;
    pak->transport_start = (uchar *)ddp + DDP_OVERHEADBYTES;
    if (dstnode == ATALK_BROAD_ADDR) {
	atalk_pak_setbroadcast(pak);
    }
    igrp2 = (igrp2type *)(pak->transport_start);
    if (packet_accounting) {
	n = atmaint_FindNeighbor(dest, atalkidb);
	if (n != NULL) {
	    switch (igrp2->opcode) {
	      case IGRP_OPC_REPLY:
		atalk_stat[ATALK_IGRP_REPLY_OUT]++;
		n->igrp_replies_sent++;
		break;
	      case IGRP_OPC_UPDATE:
		atalk_stat[ATALK_IGRP_UPDATE_OUT]++;
		n->igrp_updates_sent++;
		break;
	      case IGRP_OPC_QUERY:
		atalk_stat[ATALK_IGRP_QUERY_OUT]++;
		n->igrp_queries_sent++;
		break;
	      case IGRP_OPC_HELLO:
		atalk_stat[ATALK_IGRP_HELLO_OUT]++;
		n->igrp_hellos_sent++;
		break;
	      default:
		if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb))
		    buginf("\nAT: atigrp2_write, unknown packet opcode %d",
			   igrp2->opcode);
		break;
	    }
	}
    }
    /*
     * Note that the output idb has not been set up until now. We didn't have
     * the output idb information before this point.
     */
    pak->if_input = idb;
    atalk_pak_newInit(pak, atalkidb, bytes, dstnet, dstnode,
		      ATIGRP2_DDPTYPE,ATIGRP2_DDPTYPE,ATIGRP2_DDPTYPE);
    atalk_pak_setlength(pak, bytes);
    atalk_DDPcksum(pak);
    if (at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK, idb)) {
	buginf("\nAT: atigrp2_write: about to send packet");
	at_pak_printOhdr(pak, atalkidb, TRUE);
    }
    if (atalk_send(pak)) {
	atalk_stat[ATALK_IGRP_OUT]++;
	if (at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK, idb)) {
	    buginf("\nAT: atigrp2, src=%s, dst=%s, size=%d, EIGRP pkt sent",
		   atalkif_nodename(atalkidb), atalkif_netname(atalkidb),
		   bytes);
	}
    } else {
	atalk_stat[ATALK_IGRP_OUT_FAILED]++;
	if (at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK, idb))
	    buginf("\nAT: atigrp2, write failed to: %d.%d",dstnet,dstnode);
	/* NOTE: atalk_send pitches packet for us */
    }
}

/*
 * atigrp2_hello_valid
 * Return TRUE if HELLO packet coming from a router that is 'non-operational'
 */
static atigrp2_cabletype *
atigrp2_hello_valid (paktype* pak, idbtype *phys_idb)
{
    atigrp2_cabletype	*cable;
    igrp2_gentlvtype	*tlv, *parameter;
    int			bytes;
    ataddrtype		srcaddr;
    ushort		snet,enet;	/* starting and ending net #'s */
    ushort length;
    atalkidbtype	*idb;

    if (pak->linktype != LINK_APPLETALK)
	return (NULL);
    idb = atalk_getidb(phys_idb);
    bytes = pak->atalk_datalen - IGRP2HEADERBYTES;

    tlv = (igrp2_gentlvtype *)(pak->transport_start + IGRP2HEADERBYTES);
    cable = NULL;
    parameter = NULL;
    /*
     * Search for the cable configuration TLV, and parameter TLV in same pass
     */
    while (bytes > 0) {
	length = GETSHORT(&tlv->length);
	if (length < sizeof(igrp2_gentlvtype)) {
	    if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\nAT: atigrp2_hello_valid, runt tlv %d", length);
	    return(NULL);
	}
	switch (GETSHORT(&tlv->type)) {
	case ATIGRP2_TLV_CABLE_CONFIG:
	    cable = (atigrp2_cabletype *) tlv;
	    break;
	case IGRP2_PARAMETER:
	    parameter = tlv;
	    break;
	default:
	    /*
	     * Skip the unknown TLV
	     */
	    break;
	}
	bytes -= length;
	tlv = (igrp2_gentlvtype *) ((uchar *)tlv + length);
    }
    /*
     * Return if we couldn't find cable configuration TLV.
     * Sequenced TLV hellos do NOT have a cable configuration.
     * They're not bad, they're just of no use here. We must
     * have both a cable TLV *and* a paramter TLV to want to use the
     * information.
     */
    if ((cable == NULL) || (parameter == NULL))
	return (NULL);
    snet  = GETSHORT(&cable->cable_lo);		/* get starting net # */
    enet  = GETSHORT(&cable->cable_hi);		/* get ending net # */
    srcaddr = atalk_pak_srcaddr(pak);

    if (GETSHORT(&cable->length) != sizeof(atigrp2_cabletype)) {
	atalk_stat[ATALK_IGRP_BAD]++;
	return (NULL);
    }

    /*
     * check to see if the router in question is sending
     * discovery hello's...
     */
    if (snet == 0) {
	if (!atalkif_HalfRouter(idb) && !atalkif_CaymanTunnel(idb)) {
	    atalk_pak_printbad(pak,"EIGRP disc source net invalid",idb);
	    atalk_stat[ATALK_IGRP_BAD]++;
	    return (NULL);
	}
    } else {
	if (!atalk_validSrcAddress(idb, srcaddr)) {
	    if (atalkif_operational(idb) || !idb->atalk_discovery) {
		atalk_pak_printbad(pak,"EIGRP disc source not valid",idb);
		phys_idb->hwptr->atalk_inputerrs++;
		atalk_stat[ATALK_IGRP_BAD]++;
		return (NULL);
	    }
	}
	if ((idb->atalk_enctype == ET_ETHERTALK) && 
	    (idb->aarp_enctype == ET_NULL) &&
	    (atalk_nodeonly(srcaddr) == idb->atalknode)) {
	    errmsg(AT_ERR_ADDRINUSE, idb_get_namestring(phys_idb), srcaddr);
	    idb->atalknode ^= idb->atalknode;
	    atalkif_RestartPort(idb);
	    return (NULL);
	} else 	if (atalkif_IsMyAddress(idb,srcaddr)) {
	    atalk_pak_printbad(pak,"EIGRP disc src from us?",idb);
	    atalk_stat[ATALK_IGRP_BAD]++;
	    return (NULL);
	}
    }
    return (cable);
}

/*
 * atigrp2_AcquirePortInfo
 *
 * Like the RTMP version, but we expect a IGRP2  HELLO packet.
 * The HELLO packet should have a cable-range TLV at the end of
 * the packet. If it doesn't, then pitch it.
 *
 * Hello packets which don't have a valid cable range should not be
 * passed into DUAL; especially hello packets which have a range
 * of 0-0. (Pure discovery). Packet passed in has already been checked
 * for a 'PARAMETER' TLV.
 */
static void
atigrp2_AcquirePortInfo (paktype* pak, atigrp2_cabletype *cable,
			 atalkidbtype *idb)
{
    ataddrtype		srcaddr;
    ushort		snet,enet;	/* starting and ending net */

    srcaddr = atalk_pak_srcaddr(pak);
    if (!atalkif_InterfaceUp(idb))
	return;

    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	buginf("\nAT: igrp2, in AcquirePortInfo for %#a", srcaddr);
    }
    snet  = GETSHORT(&cable->cable_lo);		/* get starting net # */
    enet  = GETSHORT(&cable->cable_hi);		/* get ending net # */


    /*
     * Did we hear a hello packet from a router with an identical router
     * ID? If so, raise an error and bail.
     */
    if (GETLONG(&cable->routerid) == atalk_rtr_id) {
	errmsg(AT_ERR_COMPATERR4, srcaddr, atalk_rtr_id);
	atalkif_ConfigError(idb, "router ID conflict");
	atalk_stat[ATALK_IGRP_BAD]++;
	return;
    }
    /*
     * At this point, the EIGRP hello packet source address 
     * has been verified, so regardless of other actions taken
     * we save this address as the GW of last resort.
     */
    idb->atalk_defaultgw = srcaddr;
    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	buginf("\natigrp2_AcquirePortInfo: acquiring cable info for "
	       "%d-%d from %#a", snet, enet, srcaddr);
    }
    atalkif_CableInfoReport(idb,snet,enet,srcaddr);
}

static boolean
atigrp2_update_equalpaths (treeLink *link, void *pdata)
{
    atalk_pdb_t	*p = (atalk_pdb_t *)link;

    if (p == NULL)
        return (FALSE);

    atigrp2_validate_route(p->dstrngstr, p->dstrngend);
    return (TRUE);
}

static void
atigrp2_forceupdate (void)
{
    rbTree *p_tree;
 
    p_tree = atroute_GetPTree(AT_PATH_IGRP);
    if (p_tree)
 	RBTreeForEachNode(atigrp2_update_equalpaths, NULL, p_tree, FALSE);
}
  

/*
 * atigrp2_rtupdate
 *
 * Update the metric information in the routing table.
 * Called from various places in dual.c via the ddb->rtupdate callback
 * when we are processing a update/query packet from a neighbor.
 *
 * The source of the packet which caused us to be called will be
 * in ddb->ddb_infosource.
 *
 */
static boolean 
atigrp2_rtupdate (ddbtype *ddb, dual_ndbtype *dndb, dual_rdbtype *drdb,
		  boolean *promote)
{
    atalk_ipb_t	*ipb;
    long	flags;
    ataddrtype	srcaddr, nhsrcaddr;
    boolean	valid = FALSE;
    atalk_rdb_t *r;			/* existing route, if any */
    atalk_pdb_t *p;
    atalk_metric_t old_metric, new_metric;
    neighborentry *n, *old_metric_neighbor;
    ushort	rngstart, rngend;	/* start and end of network range */
    rbTree	*p_tree, *r_tree;
    atalkidbtype *idb;
    idbtype	*phys_idb;
    
    ipb       = ddb->pdb;
    phys_idb = dual_idb(drdb->drdb_iidb);
    if (!phys_idb) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }
    idb = atalk_getidb(phys_idb);
    srcaddr   = drdb->drdb_infosource.atalk_address;
    nhsrcaddr = drdb->drdb_nexthop.atalk_address;
    /* The next hop for the direct connected net is itself... */
    if (drdb->drdb_origin == IGRP2_ORG_CONNECTED)
	nhsrcaddr = srcaddr;
    rngstart  = dndb->dndb_dest.atalk_network.start;
    rngend    = dndb->dndb_dest.atalk_network.end;
    *promote = FALSE;
    if (at_debug(atalkigrp_update_debug, ATALK_INTERFACE_MASK, phys_idb)) {
	buginf("\nAT: atigrp2_rtupdate, metric %u succ %d for %d-%d, is: %a, "
	        "nh: %a, idb: %s",
	       drdb->drdb_metric, dndb->dndb_successors, rngstart,rngend,
	       srcaddr, nhsrcaddr, idb_get_namestring(phys_idb));
    }

    flags = 0;

    /*
     * Belt and suspenders.
     */
    if ((drdb->drdb_data != NULL) &&
	(((atigrp2_extdatatype *)drdb->drdb_data)->flag & IGRP2_CD)) {
	flags = NET_DEXTERIOR;
    }

    r_tree = atroute_GetTree();
    p_tree = atroute_GetPTree(AT_PATH_IGRP);
    bzero(&new_metric, sizeof(new_metric));
    bzero(&old_metric, sizeof(old_metric));
    old_metric_neighbor = NULL;
    p = atroute_GetValidPath(p_tree,rngstart,rngend,&valid,FALSE);
    if (!valid)
        return (FALSE);

    r = atroute_GetValidRoute(r_tree,rngstart,rngend,&valid,FALSE);
    /*
     * If either of these routines return !valid, it means that the
     * cable range that they specify isn't valid. Don't bother
     * trying to put it into the routing table. Matter of fact,
     * we should probably consider ignoring this tuple on a
     * permanent basis.
     */
    if (!valid) {
	return (FALSE);
    }

    if (p && ((atroute_GetEqualPathCount(p) + 1) > atalk_MaxPaths)){
        atalk_InvalidateCache("atigrp2_rtupdate");
	atroute_DeleteEqualPathList(p);
    }

    /* Build a temporary IGRP path */
    new_metric.metric             = drdb->drdb_metric;
    new_metric.type               = AT_PATH_IGRP;
    new_metric.u.igrp.hops        = drdb->drdb_vecmetric.hopcount;
    new_metric.u.igrp.bandwidth   = drdb->drdb_vecmetric.bandwidth;
    new_metric.u.igrp.delay       = drdb->drdb_vecmetric.delay;
    new_metric.u.igrp.mtu         = drdb->drdb_vecmetric.mtu;
    new_metric.u.igrp.reliability = drdb->drdb_vecmetric.reliability;
    new_metric.u.igrp.load        = drdb->drdb_vecmetric.load;

    /*
     * Is this an external path? If so, then pull out the info
     * about same from the data block hung off the side of the ddb.
     */
    if (drdb->drdb_data) {
	atigrp2_extdatatype	*data  = drdb->drdb_data;
	new_metric.u.igrp.external     = TRUE;
	new_metric.u.igrp.ext_source   = GETLONG(&data->routerid);
	new_metric.u.igrp.ext_hops     = data->ext_metric;
	new_metric.u.igrp.ext_proto_id = data->protocol;
	new_metric.u.igrp.ext_flag     = data->flag;
    }

    /*
     * Offset list logic might go here, but ask Bob.
     */

    if (r != NULL) {
	old_metric = r->rpath.metric;
	if (r->rpath.type == AT_PATH_IGRP) {
	    old_metric_neighbor = r->rpath.gwptr;
	} else {
	    old_metric_neighbor = NULL;
	}
  } else {
	old_metric.metric = METRIC_INACCESSIBLE;
	old_metric.type   = AT_PATH_IGRP;
	old_metric_neighbor = NULL;
	old_metric.u.igrp.ext_hops = DDP_POISON_HOPCOUNT;
    }
    n = atmaint_GetNeighbor(nhsrcaddr, idb, FALSE);
    if (n)
	atmaint_ReleaseNeighbor(n);

    
    /*
     * Are we getting a 'NO-OP' call from EIGRP, asking to reinstall a path
     * we already HAVE? If so, set successors to 1, indicate we accept it
     * and RETURN.
     */
    if ((n == old_metric_neighbor) &&
	(atroute_MetricCompare(&new_metric, &old_metric, ATALK_METRIC_EQ))) {
	dndb->dndb_successors = 1;
	*promote = TRUE;
	n->igrp_state = n->state = neighborUp;
	n->igrp_updates_heard++;
	return(TRUE);
    }

    /*
     * If the new metric is better than the existing one, (ie, the new
     * metric is numerically less than the existing metric) then
     * accept the path we are being handed. Or, if it is from the
     * same neighbor as before and the new metric is WORSE IGRP, accept
     * the path we are being handed. (if it is the same, it is a wash).
     */
    if ((atroute_MetricCompare(&new_metric, &old_metric, ATALK_METRIC_LT)) ||
	((n == old_metric_neighbor) &&
	 (atroute_MetricCompare(&new_metric, &old_metric, ATALK_METRIC_GT)))) {
	if (drdb->drdb_data
	    && at_debug(atalkigrp_external_debug, ATALK_INTERFACE_MASK,
			phys_idb)) {
	    buginf("\natigrp2_rtupdate: updating %d-%d, metric: %u"
		   " ext: %d from %d",
		   rngstart,rngend, new_metric.metric,
		   new_metric.u.igrp.ext_hops,
		   new_metric.u.igrp.ext_source);
	}
	if (p != NULL) {		/* If path there, delete it! */
	    if (at_debug(atalkigrp_external_debug, ATALK_INTERFACE_MASK,
			 phys_idb)) {
		buginf("\natigrp2_rtupdate: deleting existing path, metric:"
		       " %u learned from %a",
		       p->metric.metric, p->gwptr->address);
	    }
	    atalk_InvalidateCache("atigrp2_rtupdate - path del");
	    atroute_DeletePath(p);
	    p = NULL;
	}
	/* At this point, nothing previously from EIGRP is in the table... */ 
	dndb->dndb_successors = 0;
	/*
	 * If the hop count is greater than DDP_MAXHOPCOUNT, then simply
	 * return false -- we don't want to put a > 15 hop path into the
	 * forwarding table. ???
	 */
	if (   new_metric.u.igrp.hops > DDP_MAX_HOPCOUNT
	    || new_metric.metric == METRIC_INACCESSIBLE)
	    return (FALSE);
	/*
	 * If 'n' is NULL, meaning that this is the first path from
	 * a brand-new neighbor, we're going to have to create the neighbor.
	 */
	if (n == NULL) {
	    n = atmaint_CreateNeighbor(nhsrcaddr, idb);
	    if (n) {
		GET_TIMESTAMP(n->igrp_firstheard);
		GET_TIMESTAMP(n->igrp_lastheard);
	    } else {
		if (at_debug(atalkerr_debug,0))
		    buginf("\nAT: atigrp2_rtupdate -- unable to create nbr for"
			   "%#a from %s", nhsrcaddr,
			   idb_get_namestring(phys_idb));
		return (FALSE);
	    }
	}
	if (n == NULL) {
	    if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\nAT: atigrp2_rtupdate -- unable to create nbr for"
		       "%#a from %s", nhsrcaddr, idb_get_namestring(phys_idb));
	    *promote = FALSE;	/* we didn't use this path */
	    return (FALSE);
	}
	p = atroute_AddPath(p_tree, idb, rngstart, rngend, n, 
			    atroute_DefaultPathAction,
			    atigrp2_PathUse,
			    0, AT_PATH_IGRP, &new_metric);
	if (p == NULL) {
	    buginf("\nAT: atigrp2_rtupdate, not able to add path");
	    return (FALSE);
	}
	dndb->dndb_successors = 1;	/* since we're adding a new path */
	*promote = TRUE;
	n->igrp_state = n->state = neighborUp;
	n->igrp_updates_heard++;
	return (TRUE);
    }
    else {
        if (p) {
            return (atroute_AddEqualPath(p, idb->phys_idb, rngstart, rngend, n,
				         AT_PATH_IGRP, &new_metric));
        }
    }
    return (FALSE);
}

/*
 * atigrp2_advertise_request
 *
 * Returns TRUE if we are allowing the network (route) in question
 * to be propagated to the neighbor in question.
 *
 * Distribute lists are a criteria to check before sending a route out
 * an interface.
 *
 * In the future, we should check to see if we have the zone name for the
 * route in question. If we do not, then we should not advertise said route.
 * What is preventing us from making this check as of now (94.04.01) is that
 * there isn't currently a way to cleaning signal DUAL that the route
 * in question should be advertised when we have obtained the zone.
 */
static advertise_request_type
atigrp2_advertise_request (ddbtype *ddb, igrp2nettype *dest, idbtype *phys_idb)
{
    atalk_rdb_t		*r;
    rbTree		*r_tree;
    boolean		advertise_it = FALSE;
    atalkidbtype	*idb;


    if (ddb != NULL && dest != NULL && phys_idb != NULL) {
	idb = atalk_getidb(phys_idb);
	r_tree = atroute_GetTree();
	r = atroute_LookupRoute(r_tree, dest->atalk_network.start, FALSE);
	if (r != NULL) {
	    /*
	     * We've passed the zone check, now check to see if there is an
	     * output distribution list on this interface.
	     */
	    if (idb->atalk_route_filter) {
		advertise_it = atalk_routecheck(r->dstrngstr,
						r->dstrngend,
						idb->atalk_route_filter);
	    } else {
		advertise_it = TRUE;	/* no dist-list, assume true */
	    }
	    
	    /* We have to check whether this net is inside
	     * our OUT remap range on thsi domain, then we 
	     * will not propagate it
	     * since we do split horizon based on the domain
	     */
	    if(atalk_DomainRouter_running) {
		if(atdomain_FilterNet(idb, r->dstrngstr, ToDomain))
		    advertise_it = FALSE;
		
	    }
	}
	
	if (at_debug(atalkigrp_update_debug, ATALK_INTERFACE_MASK, phys_idb)) {
	    buginf("\nAT: atigrp2_advertise_request,%sadvertising %d-%d out %s",
		   (advertise_it ? " " : " not "),
		   dest->atalk_network.start, dest->atalk_network.end,
		   idb_get_namestring(phys_idb));
	}
    }
    return (advertise_it ? ADVERTISE_ROUTE : SUPPRESS_ROUTE);
}

/*
 * atigrp2_rtdelete
 * Delete an entry from the routing table only if it belongs to us.
 *
 * ??? OK, this will be tricky.
 * We simply can't nuke the route, we have to keep the route around long
 * enough to do the RTMP NotifyNeighbor stuff. (ie, 3 RTMP intervals).
 * MUST, MUST, MUST do this. Otherwise, we're going to be counting
 * to infinity all overagain...
 *
 * NN for a deleted interface will be different that NN for a deleted
 * route.
 */
static void 
atigrp2_rtdelete (ddbtype *ddb, dual_ndbtype *dndb, dual_rdbtype *drdb)
{
    atalk_pdb_t	*p;
    rbTree	*p_tree, *r_tree;
    boolean	valid = FALSE;
    idbtype     *phys_idb;
    atalkidbtype *idb;
    ataddrtype	srcaddr;
    atalk_rdb_t *r;
    neighborentry *n;


    phys_idb = dual_idb(drdb->drdb_iidb);
    idb = atalk_getidb(phys_idb);
    if (at_debug(atalkigrp_update_debug, ATALK_INTERFACE_MASK, phys_idb)) {
	buginf("\nAT: atigrp2_rtdelete, deleting %d-%d from %a",
	       dndb->dndb_dest.atalk_network.start,
	       dndb->dndb_dest.atalk_network.end,
	       drdb->drdb_nexthop.atalk_address);
	if (phys_idb)
	    buginf(" on %s", idb_get_namestring(phys_idb));
    }
    r_tree = atroute_GetTree();
    p_tree = atroute_GetPTree(AT_PATH_IGRP);
    if (!(p_tree && r_tree))
	return;
    r = atroute_GetValidRoute(r_tree,dndb->dndb_dest.atalk_network.start,
			      dndb->dndb_dest.atalk_network.end,&valid,FALSE);
    if (r == NULL || !valid)
	return;
    srcaddr =  drdb->drdb_nexthop.atalk_address;
    /*
     * Because of some baroque circumstances introduced by interface
     * flappage and DUAL's handling of same, we will get called at this
     * entry point twice for each route being deleted.
     * Fortunately, it is easy to determine when the second (bogus) call
     * is being made: the source address of the routing information will
     * be 0.0 and we will not have a neighbor entry for same.
     * In the cases where the source address is 0.0, we will have
     * no neighbor. Bail.
     */
    if ((n = atmaint_GetNeighbor(srcaddr, idb, FALSE)) == NULL)
	return;
    atmaint_ReleaseNeighbor(n);
    p = atroute_GetMatchingPath(p_tree,dndb->dndb_dest.atalk_network.start,
				dndb->dndb_dest.atalk_network.end,idb,n,
				&valid);
    if (valid && p != NULL) {
        atalk_InvalidateCache("atigrp2_rtdelete");
  	atroute_DeletePath(p);
    }
    else {
        if (r->eigrp_equalcount) {
	    p = atroute_GetValidPath(p_tree, dndb->dndb_dest.atalk_network.start,
				     dndb->dndb_dest.atalk_network.end,
				     &valid, FALSE);
	    if (valid && p) {
	        atalk_InvalidateCache("atigrp2_rtdelete");
	        atroute_DeleteEqualPath(p, idb->phys_idb);
	    }
	}
    }
}

/*
 * atigrp2_rtgetmetric
 * Get best metric from routing table for this destination.  If the
 * destination doesn't exist or the metrics aren't comparable then
 * return inaccessible.
 */
static ulong 
atigrp2_rtgetmetric (igrp2nettype *dest, ddbtype *ddb)
{
    atalk_rdb_t	*r;
    rbTree	*r_tree;
    ulong	metric;

    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, dest->atalk_network.start, FALSE);
    metric = (r == NULL) ? METRIC_INACCESSIBLE : r->rpath.metric.metric;
    return (metric);
}

/*
 * atigrp2_peerbucket
 * Return slot based on atalk_address.
 * For this one, we'll use the nethash, since we're going to be dealing
 * with the 24-bit address.
 */
static int 
atigrp2_peerbucket (igrp2addrtype *address)
{
    return (nethash((ipaddrtype)address->atalk_address));
}

/*
 * atigrp2_peerstatehook
 *
 * Called when a peer is being activated or deactivated.
 */
static void
atigrp2_peerstatehook (ddbtype *ddb, dual_peertype *peer, boolean new_peer)
{
    idbtype *idb;
    idb = peer->iidb->idb;
    if (new_peer) {
	if (at_debug(atalkigrp_events, ATALK_INTERFACE_MASK, idb)) {
	    errmsg(AT_ERR_EIGRPPEERUP, peer->source.atalk_address,
		   peer->router_id, idb->namestring);
	}
    } else {
	if (at_debug(atalkigrp_events, ATALK_INTERFACE_MASK, idb)) {
	    errmsg(AT_ERR_EIGRPPEERDN, peer->source.atalk_address,
		   peer->router_id, idb->namestring);
	}
    }
}

/*
 * atigrp2_peershowhook
 * Detailed show hook for AT/IGRP neighbors
 */
static void
atigrp2_peershowhook (ddbtype *ddb, dual_peertype *peer)
{
    if (peer == NULL)
	return;

    printf("\n   PeerID: %10u", peer->router_id);
}

/*
 * atigrp2_ndbbucket
 * Return slot based on AppleTalk network number.
 * Note that this is the same hash method used in the AppleTalk fast-cache
 * hashing.
 */
static int 
atigrp2_ndbbucket (igrp2nettype *address)
{
    int bucket;

    bucket = (address->atalk_network.start & 0xff);
    bucket ^= (address->atalk_network.start >> 8);
    return (bucket);
}

/*
 * atigrp2_headerptr
 *
 * Return pointer to beginning of IGRP2 fixed length header.
 * This will occur after the long DDP header.
 * ??? Note for future: Might have to insert broadcast proto header here.
 *
 * NOTE:
 *
 * AppleTalk is not IP. In AppleTalk, when a packet is created for output, 
 * the packet header is initialized so that pak->atalk_dataptr is initialized.
 * In the AppleTalk stack, this is done in the call atalk_pak_create().
 * For the purpose of finding where the
 * EIGRP packet begins, all we need to do is point to the first byte
 * past the DDP header. pak->atalk_dataptr always does exactly this in the
 * AppleTalk stack. We didn't put that field into the pak structure
 * just because we like typing.
 *
 * pak->atalk_datalen is also initialized, as well as things like
 * pak->datagramstart and pak->datagramsize are also initialized.
 *
 * On input, these fields are initialized in atalk_inithdrptr().
 * We AppleTalk people do this for a reason: so we don't have to copy
 * encapsulation hacks from protocols which assume that the only good
 * encap is a fixed encap. Making [incorrect] assumptions about fixed
 * encapsulations and where packets start in AppleTalk DDP, ie, thinking
 * frames begin "on the line", is a good way to break all Phase-I AppleTalk
 * encapuslations.
 */
static igrp2type 
*atigrp2_headerptr (paktype *pak)
{
    return ((igrp2type *) (pak->transport_start));
}

/*
 * atigrp2_paklen
 *
 * Return length of IGRP2 packet minus the DDP long header length.
 */
static ulong 
atigrp2_paklen (paktype *pak)
{
    return (pak->datagramsize - DDP_OVERHEADBYTES);
}    

/*
 * atigrp2_ndb_new
 *
 * Called when DUAL first allocates a new ndb (network descriptor block).
 *
 */
static void
atigrp2_ndb_new (dual_ndbtype *ndb, idbtype *phys_idb)
{
    ushort  Start,End;
    atalkidbtype *idb;

    idb = atalk_getidb(phys_idb);
    if (!ndb || !phys_idb || !idb)
	return;
    if (!atalk_DomainRouter && !idb->atalk_dminfo)
	return;

    Start = ndb->dndb_dest.atalk_network.start;
    End   = ndb->dndb_dest.atalk_network.end;

    /* OK, find this remapping entry */

    atdomain_ndb_new(idb, Start, End);
}

/*
 * atigrp2_ndb_done
 *
 * Called when DUAL is about to de-allocate a ndb (network descriptor block.
 *
 */
static void
atigrp2_ndb_done (dual_ndbtype *ndb)
{
    ushort  Start,End;

    if(!ndb)
	return;
    Start = ndb->dndb_dest.atalk_network.start;
    End   = ndb->dndb_dest.atalk_network.end;
    
    atdomain_ndb_done(Start, End);
}

/*
 * atigrp2_ipbinit
 * Initialize the ipb data structure given a new AS number.
 */
static void
atigrp2_ipbinit (atalk_ipb_t *ipb, int asystem)
{
    if (ipb == NULL)
	return;
    ipb->name     = "at/eigrp";
    ipb->pname    = "AT/EIGRP Router";
    ipb->holdokay = FALSE;

    /*
     * This should probably be set to PROC_IGRP2 once the metrics are no 
     * longer directly comparable.  But for now, this is fine.
     */

    ipb->distance = ipb->def_distance = IGRP_DISTANCE;
    ipb->asystem = asystem;
    ipb->igrp_k1 = ipb->igrp_k1_def = IGRP_K1_DEFAULT;
    ipb->igrp_k2 = ipb->igrp_k2_def = IGRP_K2_DEFAULT;
    ipb->igrp_k3 = ipb->igrp_k3_def = IGRP_K3_DEFAULT;
    ipb->igrp_k4 = ipb->igrp_k4_def = IGRP_K4_DEFAULT;
    ipb->igrp_k5 = ipb->igrp_k5_def = IGRP_K5_DEFAULT;

    /*
     * This is the other place where we need to divorce ourselves from
     * the base-level IP data structures... ???
     */
    ipb->at_socket = ATALK_SOCK_IGRP2;
}

/*
 * atigrp2_redist_all
 * Stub to do nothing -- all of IGRP2 are already redistributed
 */
static void atigrp2_redist_all (boolean distflag)
{
    return;
}

static const char atigrp2_nei_banner[] = {"\nAT/EIGRP Neighbors for process %d, router id %u"};

/*
 * atigrp2_show_neighbors
 *
 * Process "show apple neighbors <asystem> [<interface><unit>]
 */
void 
atigrp2_show_neighbors (parseinfo *csb)
{
    ddbtype *ddb;
    atalk_ipb_t *ipb;
    
    ddb = atalk_igrp2_ddb;
    ipb = ddb->pdb;
    printf(atigrp2_nei_banner, ipb->asystem, ddb->routerid.atalk_address);
    
    /*
     * In the future, we will have to pass in the real asystem
     * we want to view; for now (v1) there is only on asystem.
     */
    if (GETOBJ(int,3)) {
	igrp2_show_neighbors(ddb, GETOBJ(idb,1), GETOBJ(int,2), GETOBJ(int,2));
    } else {
	igrp2_show_interface(ddb, GETOBJ(idb,1), GETOBJ(int,2));
    }
}

/*
 * atigrp2_connstate_enq
 * Enqueue a connstate event onto the work queue.
 */
static void atigrp2_connstate_enq (ddbtype *ddb, atalkidbtype *idb,
				   boolean sense)
{
    atalk_ipb_t *ipb;
    atigrp2_worktype *work;

    if (ddb == NULL)
	return;

    ipb = ddb->pdb;
    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.ifc.interface = idb;
    work->c.ifc.start = idb->atalk_cablestart;
    work->c.ifc.end = idb->atalk_cableend;
    work->c.ifc.sense = sense;
    atigrp2_qevent(atalk_igrp2_ipb, work, IGRP2_WORK_CONNSTATE);
}

/*
 * atigrp2_ifdown_enq
 * Enqueue an ifdown event onto the work queue.
 */
static void atigrp2_ifdown_enq (ddbtype *ddb, atalkidbtype *idb,
				boolean delete)
{
    atalk_ipb_t *ipb;
    atigrp2_worktype *work;
    igrp2_idbtype *iidb;

    if (ddb == NULL)
	return;

    iidb = igrp2_find_iidb(ddb, idb->phys_idb);
    if (!iidb)
	return;

    ipb = ddb->pdb;
    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    iidb->iidb_going_down = delete;
    work->c.ifd.interface = idb;
    atigrp2_qevent(atalk_igrp2_ipb, work, IGRP2_WORK_IFDOWN);
}

/*
 * atigrp2_route_adjust
 *
 * Called when a directly connected atalk network has changed status.
 * (ie, has been shut down, started up, etc.)
 *
 * Inform the DUAL finite state machine about change, assuming that EIGRP
 * is actually running on this interface.
 *
 * tear_down is TRUE if we should disable the interface for EIGRP.
 */
static void atigrp2_route_adjust (atalkidbtype *idb, boolean tear_down)
{
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (atalk_igrp_running && igrp2_find_iidb(ddb, idb->phys_idb)) {
	/*
	 * Add network to topology table.
	 */
	if (atalkif_InterfaceUp(idb) && !tear_down) {
	    atigrp2_connstate_enq(ddb, idb, TRUE);
	} else {
	    atigrp2_connstate_enq(ddb, idb, FALSE);
	    atigrp2_ifdown_enq(ddb, idb, tear_down);
	}
    }
}

/*
 * atigrp2_if_change_callback
 *
 * Called by the system for interface changes
 */
static void atigrp2_if_change_callback (atalkidbtype *idb)
{
    atigrp2_route_adjust(idb, FALSE);
}

/*
 * atigrp2_process_ifdown
 *
 * Process an interface down event.  We need to delete the connected
 * route associated with this interface, and then tear down all peers.
 *
 * If "delete" is set, we must then destroy the IIDB.
 */
static void atigrp2_process_ifdown (atigrp2_worktype *work)
{
    igrp2_idbtype *iidb;
    idbtype *idb;
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (!ddb)
	return;
    idb = work->c.ifd.interface->phys_idb;
    if (!idb)
	return;
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;

    /* Tear down all peers. */

    igrp2_take_nbrs_down(ddb, idb, TRUE, "interface down");

    /* If we're deleting the IIDB, do so now. */

    if (iidb->iidb_going_down)
	igrp2_deallocate_iidb(ddb, iidb);
}

/*
 * atigrp2_process_connstate
 *
 * Process a connstate event from the work queue
 *
 * Install/delete the connected route.
 */
static void atigrp2_process_connstate (atigrp2_worktype *work)
{
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;
    dual_newroute route;

    ddb = atalk_igrp2_ddb;
    idb = work->c.ifc.interface->phys_idb;
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return;

    /*
     * Setup protocol specific parameters for call.
     */
    bzero(&route, sizeof(route));
    route.new_iidb = iidb;
    route.new_dest.atalk_network.start = work->c.ifc.start;
    route.new_dest.atalk_network.end   = work->c.ifc.end;
    route.new_vecmetric.mtu = max_proto_mtu(idb, LINK_APPLETALK);
    dual_conn_rtchange(ddb, &route,
		       work->c.ifc.sense ? DUAL_RTUP : DUAL_RTDOWN);
}

/*
 * atigrp2_item_size
 *
 * Return the size of the packet item that would be necessary to carry
 * the supplied route.  We return the worst-case value, since the
 * item may change from internal to external between packetization
 * and transmission.
 */
static ulong atigrp2_item_size (dual_ndbtype *dndb)
{
    return(max(sizeof(atigrp2_externtype), sizeof(atigrp2_metrictype)));
}

/*
 * atigrp2_add_item
 *
 * Build an item from the supplied DNDB and add it to the packet.
 *
 * Returns the length of the item, or 0 if there wasn't enough room to
 * add the item.
 *
 * Note that we do not put multiple destinations into a TLV any more.
 * This could still be done as an optimization to make the packets smaller.
 * Maybe Mikel will do this.
 */
static ulong atigrp2_add_item (ddbtype *ddb, igrp2_idbtype *iidb,
			       dual_ndbtype *dndb, void *packet_ptr,
			       ulong bytes_left, boolean unreachable)
{
    ulong item_length;
    dual_rdbtype *drdb;
    boolean external;
    atigrp2_metrictype *atm;
    atigrp2_externtype *ate;
    atigrp2_mpdecode *dest;
    ulong origin;
    ulong flags;
    ushort remap_net;
    atalk_rdb_t *route;
    rbTree *tree;
    atalkidbtype *interface;

    interface = atalk_getidb(iidb->idb);
    flags = 0;
    external = igrp2_route_is_external(dndb);
    atm = packet_ptr;
    ate = packet_ptr;

    /* Bail if there's no room for the item.  (There better be!) */

    if (external) {
	item_length = sizeof(atigrp2_externtype);
    } else {
	item_length = sizeof(atigrp2_metrictype);
    }
    if (item_length > bytes_left)
	return(0);			/* Uhoh! */

    drdb = dndb->dndb_rdb;
    
    /* Build the item depending on the type. */

    if (external) {			/* External route */
	PUTSHORT(&ate->type, ATIGRP2_TLV_EXTERNAL);
	PUTSHORT(&ate->length, item_length);
	PUTLONG(&ate->nexthop, 0);
	if (drdb) {			/* It has to be there, but we check */
	    bcopy(drdb->drdb_data, &ate->routerid,
		  sizeof(atigrp2_extdatatype));
	} else {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return(0);
	}
	dest = &ate->dest;
	igrp2_metric_encode(ddb, dndb, iidb, (igrp2_mdecode *)&ate->delay,
			    unreachable, FALSE);

    } else {				/* Internal route */

	if (drdb) {
	    origin = drdb->drdb_origin;
	} else {
	    origin = IGRP2_ORG_IGRP2;
	}
	PUTSHORT(&atm->type, ATIGRP2_TLV_METRIC);
	PUTSHORT(&atm->length, item_length);
	PUTLONG(&atm->nexthop, 0);
	dest = &atm->dest;
	igrp2_metric_encode(ddb, dndb, iidb, (igrp2_mdecode *)&atm->delay,
			    unreachable,(origin == IGRP2_ORG_CONNECTED));
    }
    
    PUTSHORT(&dest->cable_lo, dndb->dndb_dest.atalk_network.start);
    PUTSHORT(&dest->cable_hi, dndb->dndb_dest.atalk_network.end);
 
    if(atalk_DomainRouter_running) {
 	at_Domain *domain_src, *domain_dst;
	
 	remap_net = atdomain_GetRemapOut(interface,
					 dndb->dndb_dest.atalk_network.start);
 	if(remap_net) {
 	    if(dndb->dndb_dest.atalk_network.end) {
 		PUTSHORT(&dest->cable_hi,
 			 remap_net + dndb->dndb_dest.atalk_network.end -
 			 dndb->dndb_dest.atalk_network.start);
	    }
 	    PUTSHORT(&dest->cable_lo, remap_net);
 	}
	
 	tree = atroute_GetTree();
 	route = atroute_LookupRoute(tree, dndb->dndb_dest.atalk_network.start,
 				    FALSE);
 	if (route && route->pathcount > 0) {
 	    domain_src = route->rpath.idb->atalk_dminfo;
 	    domain_dst = interface->atalk_dminfo;
	    
 	    if (((domain_src && domain_src->hop_rdctn) ||
 		 (domain_dst && domain_dst->hop_rdctn)) &&
 		domain_src != domain_dst) {
 		
 		if (external) {
 		    if (ate->hopcount != 0)
 			ate->hopcount = 1;
 		    if (GETSHORT(&ate->ext_metric) != 0)
 			PUTSHORT(&ate->ext_metric, 1);
 		} else if (atm->hopcount != 0) {
 		    atm->hopcount = 1;
		}
 	    }
 	}
    }
    return(item_length);
}

/*
 * atigrp2_addrmatch
 * Perform an AppleTalk address match.
 */
static boolean 
atigrp2_addrmatch (igrp2addrtype *addr1, igrp2addrtype *addr2)
{
    return (addr1->atalk_address == addr2->atalk_address);
}

/*
 * atigrp2_printaddr
 *
 * Return an AT address in ASCII.
 */
static char*
atigrp2_printaddr (igrp2addrtype *addr)
{
    static char atigrp2_buffer[98];
    
    sprintf(atigrp2_buffer, "%#a", addr->atalk_address);
    return (atigrp2_buffer);
}

/*
 * atigrp2_printnet
 *
 * Return an atalk network # string in ASCII.
 */
static char*
atigrp2_printnet (igrp2nettype *addr)
{
    static char atigrp2_buffer[12];
    sprintf(atigrp2_buffer, "%d-%d",
	    addr->atalk_network.start, addr->atalk_network.end);
    return (atigrp2_buffer);
}

/*
 * atigrp2_netmatch
 * Perform atalk network number match.
 */
static boolean 
atigrp2_netmatch (igrp2nettype *net1, igrp2nettype *net2)
{
    return (   (   (net1->atalk_network.end != 0)
		&& (   net1->atalk_network.start == net2->atalk_network.start
		    && net1->atalk_network.end == net2->atalk_network.end)
		)
	    || (net1->atalk_network.start == net2->atalk_network.start));
 }

/*
 * atigrp2_addrcopy
 *
 * Copy an atalk address from source to dest. First byte specifies length
 * of address. Length returned is (length of address + 1).
 */
static ulong 
atigrp2_addrcopy (igrp2addrtype *source, uchar *dest)
{
    *dest++ = sizeof(ataddrtype);
    PUTLONG(((ataddrtype *) dest), source->atalk_address);
    return(sizeof(ataddrtype) + 1);
}

/*
 * atigrp2_localaddr
 *
 * Check to see if supplied address is one of the system's addresses.
 */
static boolean 
atigrp2_localaddr (ddbtype *ddb, igrp2addrtype *address, idbtype *idb)
{
    atalkidbtype *atalkidb = atalk_getidb(idb);

    return (atalkidb &&
 	    GETLONG(&address->atalk_address) == atalkif_MyAddress(atalkidb));
}

/*
 * atigrp2_UpdateNeighborEntry
 *
 * Update the igrp2 portions of an AppleTalk neighbor entry.
 */
static void
atigrp2_UpdateNeighborEntry (ddbtype *ddb, dual_peertype *peer, uchar opcode)
{
    sys_timestamp	 now;
    atalkidbtype	*idb;		/* idb to peer */
    ataddrtype		srcaddr;	/* peer's atalk address */
    neighborentry	*n;		/* atalk neighborentry ptr */

    if (!(ddb && peer))
	return;

    GET_TIMESTAMP(now);
    idb = atalk_getidb(peer->iidb->idb);
    srcaddr = peer->source.atalk_address;

    n = atmaint_FindNeighbor(srcaddr, idb);
    if (   (n == NULL)
	|| (atalk_igrp_running && !(n->reachable_by & ATALK_IGRP_ENABLED))) {
	if (n == NULL)			/* didn't find it, so create */
	    n = atmaint_CreateNeighbor(srcaddr, idb);
	if (n == NULL)
	    return;
 	COPY_TIMESTAMP(now, n->igrp_firstheard);
 	COPY_TIMESTAMP(now, n->igrp_lastheard);
 	n->igrp_state = n->state = neighborUp;
 	COPY_TIMESTAMP(now, n->igrp_restarted);
 	n->igrp_rtr_id = peer->router_id;
 	n->reachable_by |= ATALK_IGRP_ENABLED;
    }
    n->port = idb;
    COPY_TIMESTAMP(now, n->igrp_lastheard);
    COPY_TIMESTAMP(now, n->lastheard);
    switch (opcode) {
      case IGRP_OPC_HELLO:
 	n->igrp_hellos_heard++;
 	COPY_TIMESTAMP(now, n->igrp_last_hello);
 	if (n->igrp_state == neighborDown) {
 	    n->igrp_overdues++;
 	}
 	break;
      case IGRP_OPC_REPLY:
 	n->igrp_replies_heard++;
	COPY_TIMESTAMP(now, n->igrp_last_reply);
 	break;
      case IGRP_OPC_QUERY:
 	n->igrp_queries_heard++;
	COPY_TIMESTAMP(now, n->igrp_last_query);
 	break;
      case IGRP_OPC_UPDATE:
 	n->igrp_updates_heard++;
	COPY_TIMESTAMP(now, n->igrp_last_update);
 	break;
      default:
	n->igrp_bogus++;
	break;
    }
    n->igrp_state = n->state = neighborUp;
    return;
}

static void atigrp2_age_neighbor (neighborentry *n)
{
    igrp2_idbtype	*iidb;
    ddbtype		*ddb;

    if (   (n == NULL)
	|| (!(atalk_igrp_running && n->reachable_by & ATALK_IGRP_ENABLED)))
	return;

    ddb = atalk_igrp2_ddb;
    iidb = igrp2_find_iidb(ddb, n->port->phys_idb);
    /*
     * If we have an iidb, then assume that IGRP2 is enabled on this
     * interface; note that the hello and hold times are interface-specific,
     * but in reality they will all be set to the same value.
     */
    if (iidb) {
  	n->igrp_state = n->state = neighborUp;	/* Assume he's OK */
 	if (CLOCK_OUTSIDE_INTERVAL(n->igrp_last_hello,
 				   (3 * iidb->hello_interval))) {
 	    /* the IGRP neighbor has gone away */
  	    n->igrp_state = neighborDown;
 	} else if (CLOCK_OUTSIDE_INTERVAL(n->igrp_last_hello,	
 					  iidb->hello_interval + 1)) {
  	    n->igrp_state = neighborOverdue;
  	}
    } else {
	/* The neighbor should just be considered DOWN for IGRP purposes */
	n->igrp_state = neighborDown;
    }
    if (n->igrp_state == neighborDown)
 	n->reachable_by &= ~ATALK_IGRP_ENABLED;
}

/*
 * atigrp2_show_guts
 *
 * show good, deep, greasy info for things like the ddb and all the ndb's and
 * rdb's hanging off of it.
 *
 * In the future, this routing should take the AS number as an argument.
 */
static void
atigrp2_show_guts (void)
{
    igrp2_idbtype	*iidb, *iidb_next;
    ddbtype		*ddb = atalk_igrp2_ddb;


    if (ddb == NULL)
	return;
    mem_lock(ddb);
    printf("\n  Atalk/EIGRP information for AS %d, router id %d (0x%08X):",
	   ddb->asystem,
	   ddb->routerid.atalk_address, ddb->routerid.atalk_address);
    printf("\n  DDB name: %s, index %d", ddb->name, ddb->index);
    printf("\n  K-values: K1=%d, K2=%d, K3=%d, K4=%d, K5=%d",
	   ddb->k1, ddb->k2, ddb->k3, ddb->k4, ddb->k5);
    printf("\n  Index: %d");
    printf("\n  AppleTalk/EIGRP Interface information:");
    iidb = ddb->iidbQ.qhead;
    while (iidb) {
	mem_lock(iidb);
	printf("\n    %s", iidb->idb->namestring);
	printf("\n      state %s, hello interval %d sec, holdtime %d sec,"
	       " next hello %d sec",
	       atalkif_InterfaceUp(atalk_getidb(iidb->idb)) ? "up" : "down",
	       iidb->hello_interval/ONESEC, iidb->holdtime/ONESEC,
	       mgd_timer_left_sleeping(&iidb->iidb_hello_timer));
	printf("\n      split horizon processing is %s, # of neighbors is %d",
	       (iidb->split_horizon) ? "on, " : "off,",
	       igrp2_peer_count(iidb));
	iidb_next = iidb->next;
	free(iidb);
	iidb = iidb_next;
    }
    free(ddb);				/* mem_unlock */
}

/*
 * atigrp2_build_ext_info
 *
 * Build ddb->ddb_data for external destinations. Get information from either
 * the routing table entry or policy configuration.
 */
static atigrp2_extdatatype *
atigrp2_build_ext_info (ddbtype *ddb, atalk_metric_t *metric)
{
    atigrp2_extdatatype *ext_info;

    if (!(metric && ddb))
	return (NULL);

    ext_info = chunk_malloc(ddb->extdata_chunk);
    if (ext_info == NULL) {
	return (NULL);
    }
    PUTLONG(&ext_info->tag, ddb->routerid.atalk_address);
    PUTLONG(&ext_info->routerid, ddb->routerid.atalk_address);
    PUTLONG(&ext_info->universe, ddb->asystem);
    ext_info->universe   = ddb->asystem;
    ext_info->ext_metric = metric->u.rtmp.hops;
    ext_info->protocol   = metric->type;
    ext_info->flag       = IGRP2_EXTERNAL;
    return (ext_info);
}

/*
 * atigrp2_unredist
 *
 * Un re-distribute a path from the EIGRP routing protocol's notion of
 * a routing table.
 */
static void
atigrp2_unredist (atalk_pdb_t *p)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    dual_newroute route;

    /*
     * Don't unredist things that don't need it
     */
    if ((p == NULL) || (p->type == AT_PATH_IGRP)) {
	return;
    }

    bzero(&route, sizeof(route));
    route.new_dest.atalk_network.start = p->dstrngstr;
    route.new_dest.atalk_network.end = p->dstrngend;
    route.new_iidb = NULL;

    route.new_nexthop.atalk_address = 0;
    route.new_infosource.atalk_address = 0;
    route.new_ignore = FALSE;
    route.new_metric = METRIC_INACCESSIBLE;
    route.new_vecmetric.delay = METRIC_INACCESSIBLE;
    route.new_data = atigrp2_build_ext_info(ddb, &p->metric);
    route.new_external = TRUE;
    route.new_origin = IGRP2_ORG_REDISTRIBUTED;
    if (at_debug(atalkigrp_redist_debug,0)) {
	buginf("\natigrp2_unredist_path: undist'ing %s path to %d-%d",
	       atroute_rp_type(p->type), p->dstrngstr, p->dstrngend);
    }
    route.new_opaque_flags = 0;
    dual_rtchange(ddb, &route, DUAL_RTDOWN);
}

/*
 * atigrp2_redist
 *
 * Redist a single externally-derived path.
 */
static boolean
atigrp2_redist (atalk_pdb_t *p)
{

    idbtype	*idb;
    ddbtype	*ddb = atalk_igrp2_ddb;
    rbTree	*p_tree;
    atalk_pdb_t *real_p;
    boolean	valid;
    dual_newroute route;

    /* Check to see if route is still there */
    p_tree = atroute_GetPTree(p->type);
    real_p = atroute_GetValidPath(p_tree,p->dstrngstr,p->dstrngend,&valid,FALSE);
    if (!real_p) {
	atigrp2_unredist(p);
	return(FALSE);
    }

    /* Don't redist connected that are enabled for IGRP or IGRP paths */

    if ((p == NULL) || 
	((p->type == AT_PATH_CONNECTED) &&
	 (p->rdb->rpath.idb->atalk_rp & ATALK_IGRP_ENABLED)) ||
	(p->type == AT_PATH_IGRP)) {
	return(FALSE);
    }
    bzero(&route, sizeof(route));
    route.new_dest.atalk_network.start = p->dstrngstr;
    route.new_dest.atalk_network.end   = p->dstrngend;
    if (atalk_path_internal(p)) {
	/* if this is an internal path there is no idb.  Use the first one
		off the idb chain, as the parameters set below won't be 
		used for much anyways. */
	idb = SWIDBQ_HEAD;
    } else {
	idb = p->idb->phys_idb;
    }
    route.new_iidb = NULL;
    route.new_nexthop.atalk_address = 0;
    route.new_infosource.atalk_address = 0;
    route.new_ignore = FALSE;
    route.new_successor_metric = 0;
    route.new_vecmetric.delay = idb->delay << 8;
    route.new_vecmetric.reliability = idb->hwptr ? idb->hwptr->reliability : 0;
    route.new_vecmetric.load = idb->hwptr ? idb->hwptr->load : 0;
    route.new_vecmetric.mtu = idb->ip_mtu;
    route.new_vecmetric.hopcount = p->metric.u.rtmp.hops;
    route.new_vecmetric.bandwidth = p->metric.metric;
    route.new_metric = igrp2_computemetric(route.new_vecmetric.bandwidth,
					   route.new_vecmetric.load,
					   route.new_vecmetric.delay,
					   route.new_vecmetric.reliability,
					   ddb->k1, ddb->k2, ddb->k3, ddb->k4,
					   ddb->k5);
    if (route.new_metric == METRIC_INACCESSIBLE) {
	/* Don't redistribute infinity routes */ 
	return (TRUE);
    }
    if (at_debug(atalkigrp_redist_debug, 0)) {
	buginf("\nAT: atigrp2: disting into AT/EIGRP route %#A from %s,"
	       " %d hops metric",
	       atutil_CableRange(p->dstrngstr, p->dstrngend),
	       atroute_rp_type(p->type), p->metric.u.rtmp.hops,
	       p->metric.metric);
    }

    route.new_data = atigrp2_build_ext_info(ddb, &p->metric);
    route.new_external = TRUE;
    route.new_origin = IGRP2_ORG_REDISTRIBUTED;
    route.new_opaque_flags = 0;
    dual_rtchange(ddb, &route, DUAL_RTCHANGE);
    return (TRUE);
}

/*
 * atigrp2_redist_path
 * Enqueue a destination for redistribution on the work queue.
 */
static boolean atigrp2_redist_path (atalk_pdb_t *p)
{
    atigrp2_worktype *work;
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (ddb == NULL)
	return(FALSE);
    if (p == NULL)
	return(FALSE);
    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return(FALSE);
    }

    bcopy(p, &(work->c.red.p), sizeof(atalk_pdb_t));
    atigrp2_qevent(atalk_igrp2_ipb, work, IGRP2_WORK_REDIST);
    return(TRUE);
}

/*
 * atigrp2_unredist_path
 * Enqueue a destination for redistriibution on the work queue.
 */
static void atigrp2_unredist_path (atalk_pdb_t *p)
{
    atigrp2_worktype *work;
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (ddb == NULL)
	return;
    if (p == NULL)
	return;
    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    bcopy(p, &(work->c.red.p), sizeof(atalk_pdb_t));
    atigrp2_qevent(atalk_igrp2_ipb, work, IGRP2_WORK_UNREDIST);
}

/*
 * atigrp2_process_redist
 * Process a redistributed entry from the work queue.
 */
static void atigrp2_process_redist (atigrp2_worktype *work)
{
    atigrp2_redist(&(work->c.red.p));
}

/*
 * atigrp2_process_unredist
 * Process a redistributed entry from the work queue.
 */
static void atigrp2_process_unredist (atigrp2_worktype *work)
{
    atalk_pdb_t *p;

    p = &(work->c.red.p);
    atigrp2_unredist(p);
}

/*
 * atigrp2_validate_route
 *
 * This function is called when the AppleTalk routing engine wishes
 * to ask EIGRP/DUAL if it has any backup path for the cable-range
 * described by the parameters.
 */
void atigrp2_validate_route (ushort dstrngstr, ushort dstrngend)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    atigrp2_worktype *work;

    if (ddb == NULL)
	return;

    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.d.start = dstrngstr;
    work->c.d.end = dstrngend;
    atigrp2_qevent(atalk_igrp2_ipb, work, IGRP2_WORK_BACKUP);
}

static void 
atigrp2_process_validate (atigrp2_worktype *work)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    igrp2nettype dest;

    if (ddb == NULL)
	return;

    dest.atalk_network.start = work->c.d.start;
    dest.atalk_network.end = work->c.d.end;
    /*
     * Ask DUAL if route is still active and validate if available.
     */
    dual_validate_route(ddb, &dest);
}

static void 
atigrp2_process_lost (atigrp2_worktype *work)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    igrp2nettype dest;

    if (ddb == NULL)
	return;

    dest.atalk_network.start = work->c.d.start;
    dest.atalk_network.end = work->c.d.end;
    /*
     * Ask DUAL if route is still active and validate if available.
     */
    dual_lostroute(ddb, &dest);
}

/*
 * atigrp2_process_redistribute
 * Process a redistribute event from the work queue.  (this is also a skeleton)
 */
static void atigrp2_process_redistribute (atigrp2_worktype *work)
{
    return;
}

/*
 * atigrp2_process_workq
 *
 * Process the work queue.  Returns TRUE if we bailed because of the limiter.
 */
#define WORKQ_LIMITER 20		/* Max 20 per pass */
static boolean atigrp2_process_workq (atalk_ipb_t *ipb)
{
    atigrp2_worktype *work;
    ulong workq_limiter;

    workq_limiter = WORKQ_LIMITER;
    while (!process_is_queue_empty(ipb->workQ)) {

	/* Bail if we should go do something else. */

	workq_limiter--;
	if (!workq_limiter)
	    return(TRUE);

	process_may_suspend();

	work = process_dequeue(ipb->workQ);
	switch (work->type) {
	  case IGRP2_WORK_REDIST:
	    atigrp2_process_redist(work);
	    break;
	  case IGRP2_WORK_UNREDIST:
	    atigrp2_process_unredist(work);
	    break;
	  case IGRP2_WORK_IFDOWN:
	    atigrp2_process_ifdown(work);
	    break;
	  case IGRP2_WORK_REDISTALL:
	    atigrp2_process_redistribute(work);
	    break;
	  case IGRP2_WORK_BACKUP:
	    atigrp2_process_validate(work);
	    break;
	  case IGRP2_WORK_LOST:
	    atigrp2_process_lost(work);
	    break;
	  case IGRP2_WORK_CONNSTATE:
	    atigrp2_process_connstate(work);
	    break;
	  case IGRP2_WORK_SUMMARY:
	  case IGRP2_WORK_TABLERELOAD:
	  case IGRP2_WORK_INTEXTSTATE:
	  default:
	    /*
	     * Do nothing.
	     */
	    break;
	}
	chunk_free(atalk_igrp2_ddb->workq_chunk, work);
	dual_finish_update_send(ipb->ddb);
    }
    return(FALSE);
}

static void 
atigrp2_show_topology_route (ddbtype *ddb, ushort start)
{
    dual_ndbtype        *dndb;
    dual_rdbtype        *drdb, *drdb_next;
    atigrp2_extdatatype *ext_info;
    char		*origin_str;
    igrp2nettype	dest;
    igrp2metric		*metric;

    if (ddb == NULL)
	return;
    /*
     * Since we don't want to trouble the user with entering the whole
     * cable range, use just the starting number. This will require that
     * we fill in the ending range below upon a successful lookup, lest
     * we see extended nets displayed as non-extended nets in the
     * actual output.
     */
    dest.atalk_network.start = start;
    dest.atalk_network.end   = 0;

    dndb = dual_ndblookup(ddb, &dest);
    if (!dndb) {
	printf("\n% Route not in topology table");
	return;
    }
    /*
     * Lock both the ddb and the neighbor descriptor block for the rest
     * of this function. This will guarantee that we can make the 
     * through the printing of the route chain without someone
     * freeing the root of the route chain out from beneath us.
     */
    mem_lock(ddb);
    mem_lock(dndb);
    dest = dndb->dndb_dest;		/* fill in end of cable range */
    printf("\nAT/EIGRP topology entry for %s", (*ddb->printnet)(&dest));
    printf("\n  State is %s, Query origin flag is %s, %d Successor(s)",
	   dual_dndb_active(dndb) ? "Active" : "Passive", 
	   dual_query_origin_str(dndb->query_origin), dndb->dndb_successors);
    printf("\n  Routing Descriptor Blocks:");

    drdb = dndb->dndb_rdb;
    while (drdb) {
	mem_lock(drdb);
	metric = &drdb->drdb_vecmetric;
	switch (drdb->drdb_origin) {
	  case IGRP2_ORG_CONNECTED:
	    origin_str = "Connected";
	    break;
	  case IGRP2_ORG_REDISTRIBUTED:
	    origin_str = "Redistributed";
	    break;
	  case IGRP2_ORG_IGRP2:
	    origin_str = (*ddb->printaddr)(&drdb->drdb_nexthop);
	  default:
	    origin_str = "Unknown origin";
	}
	printf("\n  NextHop is %s", origin_str);
	if (drdb->drdb_iidb) {
	    printf(" (%s)", drdb->drdb_iidb->idb->namestring);
	}
	printf(", heard from %s", (*ddb->printaddr)(&drdb->drdb_infosource));
	printf("\n      Composite metric is (%u/%u)"
	       "\n      Send flag is 0x%x, Route is %s", 
	       drdb->drdb_metric, drdb->drdb_successor_metric,
	       drdb->drdb_send_flag,
	       (drdb->drdb_external) ? "External" : "Internal");
	if (drdb->drdb_ignore) {
	    printf("\n      **This is an ignored route**");
	}
	printf("\n      Vector metric:");
	printf("\n        Minimum bandwidth is %u Kbit", (metric->bandwidth) ?
		   IGRP2_BANDWIDTH_SCALE/metric->bandwidth : 0);
	printf("\n        Total delay is %u nanoseconds",
	       igrp2_cdelay(metric->delay));
	printf("\n        Reliability is %u/255", metric->reliability);
	printf("\n        Load is %u/255", metric->load);
	printf("\n        Minimum MTU is %u", metric->mtu);
	printf("\n        Hop count is %u", metric->hopcount);
	if (drdb->drdb_data) {
	    ext_info = chunk_malloc(ddb->extdata_chunk);
	    bcopy(drdb->drdb_data, ext_info, sizeof(atigrp2_extdatatype));
	    printf("\n      External data:");
	    printf("\n        Originating router is %d (0x%08x) %s",
		   GETLONG(&ext_info->routerid), GETLONG(&ext_info->routerid),
		   (GETLONG(&ext_info->routerid) ==
		    ddb->routerid.atalk_address) ? "(this system)" : " ");
	    printf("\n        AS number of route is %d",
		   GETLONG(&ext_info->universe));
	    printf("\n        External protocol is %s, external metric is %d", 
		   atroute_rp_type(ext_info->protocol), ext_info->ext_metric);
	    printf("\n        Administrator tag is %d (0x%08x)",
		   GETLONG(&ext_info->tag), GETLONG(&ext_info->tag));
	    printf("\n        Flag is 0x%08x", ext_info->flag);
	    chunk_free(ddb->extdata_chunk, ext_info);
	}
	drdb_next = drdb->drdb_next;
	free(drdb);
	drdb = drdb_next;
    }
    free(dndb);
    free(ddb);
}

static const char atigrp2_top_banner[] = {"\nAT/EIGRP Topology Table for process %d, router id %u"};

/*
 * atigrp2_show_topology
 *
 * Process "show appletalk igrp2 topology <asystem>
 */
void 
atigrp2_show_topology (parseinfo *csb)
{
    ddbtype *ddb;
    atalk_ipb_t *ipb;
    boolean	detail;
    ushort	net;

    detail = GETOBJ(int,1);
    net = GETOBJ(int,2);

    ddb = atalk_igrp2_ddb;
    ipb = ddb->pdb;
    if (detail) {
	atigrp2_show_topology_route(ddb, net);
    } else {
	printf(atigrp2_top_banner, ipb->asystem, ddb->routerid.atalk_address);
	igrp2_show_topology(ddb, GETOBJ(int,3));
    }
}

static void
atigrp2_set_splithorizon (atalkidbtype *idb, boolean split_enable)
{
    if (atalk_igrp2_ddb == NULL)
        return;

    igrp2_set_split_horizon(atalk_igrp2_ddb, idb->phys_idb, split_enable);
}

static boolean
atigrp2_get_splithorizon (idbtype *idb)
{
    ddbtype	  *ddb = atalk_igrp2_ddb;
    igrp2_idbtype *iidb;

    if (ddb == NULL)
        return(FALSE);
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return (FALSE);

    return (iidb->split_horizon);
}

/*
 * atigrp2_set_mtu
 */
static void atigrp2_set_mtu (igrp2_idbtype *iidb)
{
    idbtype *idb;

    if (iidb) {
	idb = iidb->idb;
	iidb->max_packet_size = DDP_MAX_DATABYTES;
    }
}

/*
 * atigrp2_config_idb -- configure an IDB to run AT/IGRP2.
 *
 * This is called to enable or disable IGRP2 on an AppleTalk interface.
 *
 * If enable == TRUE, then we are enabling IGRP2 on this interface;
 * otherwise, we are disabling IGRP2 on this interface.
 */
static void
atigrp2_config_idb (idbtype *idb, boolean enable)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    atalk_ipb_t	*ipb;
    atalk_pdb_t *idb_path;
    igrp2_idbtype *iidb;
    atalkidbtype *atalkidb;

    atalkidb = atalk_getidb(idb);
    if (!idb || !ddb || !atalkidb)
	return;
    
    iidb = igrp2_find_iidb(ddb, idb);

    /* Countervene any enqueued request to delete the IIDB. */

    if (iidb && enable)
	iidb->iidb_going_down = FALSE;

    /* If this command has no effect, do nothing */

    if (iidb ? (enable) : (!enable)) {
	return;
    }
    idb_path = atalkif_GetIdbPath(atalkidb);
    /* Remove current redistributed, if enabling */
    if (enable && idb_path) {
	atigrp2_unredist_path(idb_path);
    }
    ipb = atalk_igrp2_ipb;
    if (enable) {
	if (!(igrp2_allocate_iidb(ipb->ddb, idb, FALSE)))
	    return;
	atigrp2_set_splithorizon(atalkidb, TRUE); /* turn on S/H by default */
	atigrp2_config_hello(atalkidb, 0);
	atigrp2_config_holdtime(atalkidb, 0);
	/*
	 * Let DUAL know about the directly connected route.
	 */
	atigrp2_route_adjust(atalkidb, FALSE);

    } else {				/* Disabling */

	atigrp2_route_adjust(atalkidb, TRUE); /* Tear down and clean up. */
        if (idb_path)
	    atigrp2_redist_path(idb_path); /* Redistribute other path */
    }
}

/*
 * sets the IGRP2 hello interval for a IGRP-configured interface.
 * The hello interval is specified in seconds, defaulted if 0.
 */
void 
atigrp2_config_hello (atalkidbtype *atalkidb, ulong hello_int)
{
    ddbtype *ddb;
    idbtype *idb;

    idb = atalkidb->phys_idb;

    ddb = atalk_igrp2_ddb;
    if (hello_int == 0)
	hello_int = igrp2_def_helloint(idb, ddb->linktype);
    else
	hello_int *= ONESEC;
    igrp2_set_hello_interval(ddb, idb, hello_int);
}

/*
 * Returns the IGRP2 hello interval, in seconds.
 * Also return an indication as to whether or not the hello time has been
 * configured.
 */
static ulong
atigrp2_idb_hello_int (atalkidbtype *atalkidb, boolean *configd)
{
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;
    ulong hello_int, def_int;

    idb = atalkidb->phys_idb;
    *configd = FALSE;
    ddb = atalk_igrp2_ddb;
    def_int = igrp2_def_helloint(idb, ddb->linktype) / ONESEC;
    if (idb == NULL)
	return(def_int);
    iidb = igrp2_find_iidb(ddb, idb);
    if (iidb == NULL)
	return(def_int);
    hello_int = iidb->hello_interval / ONESEC;
    if (hello_int != def_int)
	*configd = TRUE;
    return(hello_int);
}

/*
 * sets the IGRP2 holdtime interval for a IGRP-configured interface.
 * The hold interval is specified in seconds, defaulted if 0.
 */
void 
atigrp2_config_holdtime (atalkidbtype *atalkidb, ulong holdtime)
{
    ddbtype *ddb;
    idbtype *idb;

    idb = atalkidb->phys_idb;
    ddb = atalk_igrp2_ddb;
    if (holdtime == 0)
	holdtime = igrp2_def_holdint(idb, ddb->linktype);
    else
	holdtime *= ONESEC;
    igrp2_set_holdtime(ddb, idb, holdtime);
}

/*
 * Returns the IGRP2 holdtime interval, in seconds.
 * Also return an indication as to whether or not the holdtime has been
 * configured.
 */
static ulong
atigrp2_idb_holdtime (atalkidbtype *atalkidb, boolean *configd)
{
    ddbtype *ddb;
    idbtype *idb;
    igrp2_idbtype *iidb;
    ulong holdtime, def_int;

    idb = atalkidb->phys_idb;
    *configd = FALSE;
    ddb = atalk_igrp2_ddb;
    def_int = igrp2_def_holdint(idb, ddb->linktype) / ONESEC;
    if (idb == NULL)
	return(def_int);
    iidb = igrp2_find_iidb(ddb, idb);
    if (iidb == NULL)
	return(def_int);
    holdtime = iidb->holdtime / ONESEC;
    if (holdtime != def_int)
	*configd = TRUE;
    return(holdtime);
}

/*
 * atigrp2_AgePath and atigrp2_PathAger
 *
 * These two functions exist because of how we do our path block locking.
 * If we should happen to try to delete a path while some other thread
 * has it locked, we will mark it as "ready for deletion" and then
 * return from atroute_DeletePath. (eg, someone has the pdb locked
 * from the "show apple route" thread)
 *
 * These functions will periodically scan the IGRP path tree and
 * call atroute_DeletePath on pdb's which have been previously marked for
 * deletion. If a pdb previously marked as 'deleted' is now no longer locked
 * we will now delete it.
 */
static boolean
atigrp2_AgePath (treeLink *link, void *pdata)
{
    atalk_pdb_t	*p = (atalk_pdb_t *)link;
    boolean deleteallpaths = (boolean) pdata;
    
    if (!deleteallpaths)
      return(TRUE);

    ATASSERT(p != NULL);
    if (p == NULL)
	return (FALSE);

    atalk_InvalidateCache("atigrp2_AgePath");
    atroute_DeletePath(p);
    return (TRUE);
}

static void
atigrp2_PathAger (boolean deleteallpaths)
{
    rbTree	*p_tree;

    p_tree = atroute_GetPTree(AT_PATH_IGRP);
    if (p_tree)
	RBTreeForEachNode(atigrp2_AgePath, (void *) deleteallpaths, p_tree,
			  FALSE);
}

/*
 * Stop all EIGRP processing and only EIGRP processing.
 */
static void atigrp2_stop (void)
{
    atalkidbtype 	*idb;
    ddbtype 	*ddb;
    atalk_ipb_t	*ipb;
    neighborentry	*n = NULL;
    neighborentry	*prev_n = NULL;

    ddb = atalk_igrp2_ddb;
    if (ddb == NULL)
	return;
    ipb = atalk_igrp2_ipb;
    if (ipb == NULL)
	return;

    /*
     * Unbind the EIGRP socket first, so that we don't have any packets
     * pending in the EIGRP input queue.
     */
    atalk_UnBindSocket(ipb->at_socket);
    /* Delete all interfaces from IGRP's table */
    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_enabled) {
	    atigrp2_config_idb(idb->phys_idb, FALSE);
	    idb->atalk_rp &= ~ATALK_IGRP_ENABLED;
	    /*
	     * If there is no routing protocol at all left on the interface
	     * now, then disable all AppleTalk processing on the interface.
	     */
	    if ((idb->atalk_rp & ~(ATALK_IGRP_CONFIGURED)) == 0) {
		atalkif_clear_zones(idb);
		atalkif_shutdown(idb);
		atalkif_init(idb, TRUE, FALSE);
	    }
	}
    }

    atalk_redist_status &= ~ATALK_REDIST_ENABLED;

    /* Delete all EIGRP paths from the route table */
    atigrp2_PathAger(TRUE);

    atroute_UnregisterProtocol(AT_PATH_IGRP);
    process_kill(ipb->router_pid);		/* kill the routing process */
    atalk_igrp_running = FALSE;
    atalk_rtr_id = 0;
    /*
     * Delete all EIGRP-only neighbors. No EIGRP routing makes EIGRP
     * neighbors a non-sequitur.
     */
    do {
	prev_n = n;
	n = atmaint_GetNextNeighbor(prev_n);
	if (prev_n == NULL)
	    continue;

	if (prev_n->reachable_by & ATALK_IGRP_ENABLED) {
	    if (!(prev_n->reachable_by & ATALK_RTMP_ENABLED)) {
		prev_n->state = neighborDown;
		errmsg(AT_ERR_NBRDELETED, prev_n->address);
		atmaint_ReleaseNeighbor(prev_n);
		atmaint_DeleteNeighbor(prev_n);
	    } else {
		prev_n->reachable_by &= ~ATALK_IGRP_ENABLED;
	    }
	}
    } while (n != NULL);

    while (!process_is_queue_empty(ipb->workQ))
	chunk_free(ddb->workq_chunk, process_dequeue(ipb->workQ));
    delete_watched_queue(&ipb->workQ);
    free(ipb);
    igrp2_free_ddb(ddb);
    atalk_igrp2_ddb = NULL;
    atalk_igrp2_ipb = NULL;
    atalk_igrp_started = FALSE;
}

static void
atigrp2_set_bandwidth_percent (atalkidbtype *idb, ulong percent)
{
    if (atalk_igrp2_ddb == NULL)
        return;

    igrp2_set_bandwidth_percent(atalk_igrp2_ddb, idb->phys_idb, percent);
}

static ulong
atigrp2_get_bandwidth_percent (idbtype *idb)
{
    ddbtype	  *ddb = atalk_igrp2_ddb;
    igrp2_idbtype *iidb;

    if (ddb == NULL)
        return(0);
    iidb = igrp2_find_iidb(ddb, idb);
    if (!iidb)
	return (0);

    if (iidb->igrp2_bandwidth_percentage == IGRP2_DEFAULT_BANDWIDTH_PERCENT)
	return(0);

    return (iidb->igrp2_bandwidth_percentage);
}

/*
 * atigrp2_ddb_debug
 *
 * Handle AT-EIGRP debugging commands which need to touch the ddb or ipb.
 */
void 
atigrp2_ddb_debug (parseinfo *csb)
{
    ushort	net;
    uchar	node;
    char       *str;

    if (atalk_igrp2_ddb == NULL) {
	printf("\n%%Appletalk EIGRP not running"); 
	return;
    }
    if (csb->sense) {
	switch (csb->which) {
	  case DEBUG_ATALKIGRP_TARGET:
	    if ((GETOBJ(int,1) == 0) || (GETOBJ(int,2) == 0)) {
		printf("\nMalformed or nonexistent cable range specified");
		return;
	    }
	    atalk_igrp2_ddb->debug_target.atalk_network.start = GETOBJ(int,1);
	    atalk_igrp2_ddb->debug_target.atalk_network.end = GETOBJ(int,2);
	    atalk_igrp2_ddb->debug_target_set = TRUE;
	    printf("\nAppletalk target enabled for %d-%d",
		   GETOBJ(int,1), GETOBJ(int,2));
	    break;
	    
	  case DEBUG_ATALKIGRP_PEER:
	    net  = GETOBJ(paddr,1)->apple_addr.net;
	    node = GETOBJ(paddr,1)->apple_addr.host;
	    atalk_igrp2_ddb->debug_peer.atalk_address = atalk_address(net,node);
	    atalk_igrp2_ddb->debug_peer_set = TRUE;
	    break;
	    
	  default:
	    break;
	}
    } else {
	str = NULL;
	switch (csb->which) {
	  case DEBUG_ATALKIGRP_TARGET:
	    atalk_igrp2_ddb->debug_target_set = FALSE;
	    str = "target";
	    break;
	  case DEBUG_ATALKIGRP_PEER:
	    atalk_igrp2_ddb->debug_peer_set = FALSE;
	    str = "peer";
	    break;
	  default:
	    break;
	}
	if (str) {
	    printf("\nAppletalk EIGRP %s debugging disabled", str);
	}
    }
}

int
atigrp2_get_active_time (void)
{
   if (atalk_igrp2_ddb == NULL || atalk_igrp2_ipb == NULL)
       return (0);

   return (atalk_igrp2_ddb->active_time / ONEMIN);
}

/*
 * atigrp2_clear_event_logging
 *
 * Stop event logging.
 */
void atigrp2_clear_event_logging (parseinfo *csb)
{
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (!ddb)
	return;
    dual_clear_event_logging(ddb);
}

/*
 * atigrp2_clear_log
 *
 * Clear out the EIGRP event log (and restart logging if it has been
 * stopped).
 */
void atigrp2_clear_log (parseinfo *csb)
{
    ddbtype *ddb;

    ddb = atalk_igrp2_ddb;
    if (!ddb)
	return;
    dual_clear_log(ddb);
}

/*
 * atigrp2_clear_neighbors
 *
 * Process exec command:
 * 	clear apple eigrp neighbors [<appletalk-address> | <interface>]
 */
void atigrp2_clear_neighbors (parseinfo *csb)
{
    ddbtype	  *ddb = atalk_igrp2_ddb;
    idbtype       *idb, *tidb;
    igrp2_idbtype *iidb;
    dual_peertype *peer;
    ushort	net;
    uchar	node;
    ataddrtype 	addr;
    atalk_rdb_t *r = NULL;
    atalk_pdb_t	*p;
    rbTree	*tree;
    igrp2addrtype address;

    if ((ddb == NULL) || (csb == NULL))
        return;

    idb = GETOBJ(idb,1);
    net  = GETOBJ(paddr,1)->apple_addr.net;
    node = GETOBJ(paddr,1)->apple_addr.host;
    addr = atalk_address(net,node);
    address.atalk_address = addr;

    /*
     * Scan through all IGRP2 pdbs.
     */
    if (!(idb || addr)) {
	/*
	 * Take all neigbhors down.	
	 */
	for (iidb = ddb->iidbQ.qhead;
	     iidb != NULL; 
	     iidb = iidb->next) {
	    igrp2_take_nbrs_down(ddb, iidb->idb, FALSE, "manually cleared");
	}
    } else if (idb != NULL) {
	/*
	 * Take neighbors down on a specific interface.
	 */
	igrp2_take_nbrs_down(ddb, idb, FALSE, "manually cleared");
    } else {
	/*
	 * Take a single neighbor down.
	 */
	tree = atroute_GetTree();
	if ((net != 0) &&
	    ((r = atroute_LookupRoute(tree, net, FALSE)) != NULL) &&
	    atalk_net_connected(r) &&
	    ((p = r->best_path) != NULL) &&
	    ((tidb = p->idb->phys_idb) != NULL) &&
	    ((peer = igrp2_find_peer(ddb, &address, tidb)) != NULL)) {
	    igrp2_take_peer_down(ddb, peer, FALSE, "manually cleared");
	} else {
	    errmsg(AT_ERR_NOSUCHNBR, addr);
	}
    }
}

/*
 * atigrp2_show_traffic
 *
 * show appletalk eigrp traffic
 */
void atigrp2_show_traffic (parseinfo *csb)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    if (ddb == NULL)
	return;
    printf("\nAT-EIGRP Traffic Statistics");
    igrp2_show_traffic(ddb);
}

/*
 * atigrp2_show_event
 *
 * show appletalk eigrp event
 */
void atigrp2_show_event (parseinfo *csb)
{
    ddbtype	*ddb = atalk_igrp2_ddb;
    int		show_type = 0;

    if (ddb == NULL)
	return;
    switch (csb->which) {
      case SHOW_APPLE_EIGRP_EVENT:
	show_type = IGRP2_EVENT_CUR;
	printf("\n  Atalk/EIGRP current event information router id %d"
	       " (0x%08X):",
	       ddb->routerid.atalk_address, ddb->routerid.atalk_address);
	break;
      case SHOW_APPLE_EIGRP_SIAEVENT:
	show_type = IGRP2_EVENT_SIA;
	printf("\n  Atalk/EIGRP SIA event information router id %d (0x%08X):",
	       ddb->routerid.atalk_address, ddb->routerid.atalk_address);
	break;
      default:
	return;
    }
    dual_show_events(ddb, show_type, 0, 0);
}

/*
 * atigrp2_config_command
 *
 * EIGRP Configuration global config command entry.
 *
 * Eventually, all EIGRP global configuration commands will flow into
 * this function. This will take some more extraction from the
 * at_parse.c file and will require the EIGRP components of AppleTalk
 * to register a dynamic parse chain.
 */
void
atigrp2_config_command (parseinfo *csb)
{
    ddbtype	*ddb;
    atalk_ipb_t	*ipb;

    if (csb == NULL)
	return;
    if (csb->which != ATALK_EIGRP_COMMAND)
	return;
    if ((ipb = atalk_igrp2_ipb) == NULL)
	return;
    if ((ddb = ipb->ddb) == NULL)
	return;

    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case ATALK_EIGRP_TIMERS:
	    if (ddb->active_time == IGRP2_DEF_ACTIVE_TIME)
		break;
	    if (atalk_igrp_running) {
		nv_write(TRUE, "%s", csb->nv_command);
		if (ddb->active_time)
		    nv_add(TRUE, " %d", (ddb->active_time / ONEMIN));
		else
		    nv_add(TRUE, " disabled");
	    }
	    break;

	  case ATALK_EIGRP_LOG:
	    /* Store apple eigrp-logging only if it was explicitly specified */
	    if (atalk_eigrp_logged) {	
		nv_write(atalkigrp_events->flag != 0,  csb->nv_command);
	    }
	    break;
	    
	  case ATALK_EIGRP_LOG_SIZE:
	    nv_write(ddb->max_events != DUAL_DEFAULT_MAX_EVENTS, "%s %d",
		     csb->nv_command, ddb->max_events);
	    break;

	  case ATALK_EIGRP_LOG_TYPE:
	    dual_configure_logging(csb, ddb, FALSE, FALSE, FALSE);
	    break;

	  case ATALK_EIGRP_LOG_NBR:
	    nv_write(ddb->log_adj_change != IGRP2_DEFAULT_LOG_ADJ_CHANGE,
		     "%s%s", ddb->log_adj_change ? "" : "no ",
		     csb->nv_command);
	    break;

	  default:
	    break;
	}
	return;
    }

    if (!atalk_igrp_running) {
	printf(atalk_badcmd, "AppleTalk/EIGRP must be enabled first");
	return;
    }
    switch (GETOBJ(int,1)) {
      case ATALK_EIGRP_TIMERS:
	if (csb->sense) {
	    ddb->active_time = GETOBJ(int,2) * ONEMIN;
	    if (GETOBJ(int,2) == 0) {
		mgd_timer_stop(&ddb->ddb_active_timer);
	    } else {
		mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);
	    }
	} else {
	    ddb->active_time = IGRP2_DEF_ACTIVE_TIME;
	    mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);
	}
	break;
	
      case ATALK_EIGRP_LOG:
	atalk_debug_setflag(csb, atalkigrp_events);
	atalk_eigrp_logged = csb->sense;
	ddb->log_events = csb->sense;
	break;

      case ATALK_EIGRP_LOG_SIZE:
	if (!dual_init_events(ddb, GETOBJ(int,2), csb->sense))
	    printf(nomemory);
	break;

      case ATALK_EIGRP_LOG_TYPE:
	dual_configure_logging(csb, ddb, GETOBJ(int,2), GETOBJ(int,3),
			       GETOBJ(int,4));
	break;

      case ATALK_EIGRP_LOG_NBR:
	ddb->log_adj_change = csb->sense;
	break;

      default:
	break;
    }
}

void
atigrp2_if_command (parseinfo *csb)
{
    atalkidbtype	*idb;
    ulong	hello_int, holdtime, bw_percent;
    boolean	hello_configd, hold_configd;

    if (csb == NULL)
	return;

    if (!atalk_running && !csb->nvgen) {
	printf(atalk_not_enabled);
	return;
    }
    idb = atalk_getidb(csb->interface);

    if (csb->nvgen) {
	if (!(idb && idb->atalk_enabled)) {
	    return;
	}
	if (!(atalk_igrp_running && (idb->atalk_rp & ATALK_IGRP_ENABLED))) {
	    return;
	}
	
	switch (csb->which) {
	  case ATALK_IGRP_SPLIT:
	    /*
	     * Write out this interface command only if
	     * split-horizon processing is turned off (on is default)
	     */
	    if (atalk_running && atalk_igrp_running
		&& (idb->atalk_rp & ATALK_IGRP_ENABLED)) {
		nv_write(!atigrp2_get_splithorizon(idb->phys_idb), "no %s",
			 csb->nv_command);
	    }
	    break;

	  case ATALK_IGRP_BW_PCT:
	    if (atalk_running && atalk_igrp_running
		&& (idb->atalk_rp & ATALK_IGRP_ENABLED)) {
		bw_percent = atigrp2_get_bandwidth_percent(idb->phys_idb);
		nv_write(bw_percent != 0, "%s %d", csb->nv_command,
			 bw_percent);
	    }
	    break;

	  case ATALK_IGRP_TIMERS:
	    if (atalk_running && atalk_igrp_running
		&& (idb->atalk_rp & ATALK_IGRP_ENABLED)) {
		hello_int = atigrp2_idb_hello_int(idb, &hello_configd);
		holdtime  = atigrp2_idb_holdtime(idb, &hold_configd);
		nv_write((hello_configd || hold_configd), "%s %d %d",
			 csb->nv_command, hello_int, holdtime);
	    }
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }					/* end of NV gen */

    if (!(idb && idb->atalk_enabled)) {
	printf(atalk_badcmd, atalk_not_on_idb);
	return;
    }
    if (!(atalk_igrp_running && (idb->atalk_rp & ATALK_IGRP_ENABLED))) {
	printf(atalk_badcmd, "AT/EIGRP not enabled on this interface");
	return;
    }

    switch (csb->which) {
      case ATALK_IGRP_SPLIT:
	/* For AT-IGRP, the default is to enable split horizon */
	atigrp2_set_splithorizon(idb, csb->set_to_default || csb->sense);
	break;

      case ATALK_IGRP_BW_PCT:
 	atigrp2_set_bandwidth_percent(idb, GETOBJ(int,1));
	break;

      case ATALK_IGRP_TIMERS:
	atigrp2_config_hello(idb, csb->sense ? GETOBJ(int,1) : 0);
	atigrp2_config_holdtime(idb, csb->sense ? GETOBJ(int,2) : 0);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void
atigrp2_routing_command (parseinfo *csb)
{
    if (csb == NULL)
	return;

    if (csb->nvgen) {
	nv_write(atalk_igrp_running, "appletalk routing eigrp %d",
		 atalk_rtr_id);
	return;
    }

    if (csb->sense == FALSE) {
	atigrp2_stop();
    } else {
	if (!atalk_running)
	    atalk_start();
	atalk_rtr_id = GETOBJ(int,2);
	if (atalk_rtr_id) {
	    atigrp2_start();
	    atalk_redist_status |=
		(ATALK_REDIST_ENABLED|ATALK_REDIST_CONFIGURED);
	    atroute_redist_all(FALSE);
	}
    }
}

/*
 * atigrp2_protocol_command
 *
 * Here is where we handle the "eigrp" option to the interface-specific
 * "appletalk protocol" command.
 */
void
atigrp2_protocol_command (parseinfo *csb)
{
    int		which;			/* which routing protocol */
    atalkidbtype	*idb;

    if (csb == NULL)
	return;
    
    which = GETOBJ(int,1);
    idb = atalk_getidb(csb->interface);

    if (csb->nvgen) {
        if (idb == NULL)
	    return;
	switch (which) {
	  case ATALK_RP_IGRP:
	    nv_write((   atalk_igrp_running
		      && (idb->atalk_rp & ATALK_IGRP_CONFIGURED)),
		     "%s", csb->nv_command);
	    break;
	  default:
	    break;
	}
	return;
    }

    /*
     * Create a sub-idb if one hasn't allocated.
     */
    if (idb == NULL) {
	atalk_init_idb(csb->interface);
	if ((idb=atalk_getidb(csb->interface)) == NULL) {
	    printf("\n%% Unable to create data structures for atalk interface "
		   "information.");
	    return;
	}
	atalk_rxtypes_update(csb->interface);
    }
    
    if (!idb->atalk_enabled) {
	printf(atalk_badcmd, atalk_not_enabled);
	return;
    }
    
    switch (which) {
      case ATALK_RP_IGRP:
	if (atalk_igrp_running) {
	    if (csb->sense) {
		idb->atalk_rp |= (ATALK_IGRP_ENABLED | ATALK_IGRP_CONFIGURED);
		atigrp2_config_idb(idb->phys_idb, csb->sense);
	    } else {
		if (!(idb->atalk_rp & ATALK_RTMP_ENABLED))
		    printf(atalk_badcmd, "Disabling EIGRP without enabling "
			   "RTMP");
		else {
		    idb->atalk_rp &= ~(ATALK_IGRP_ENABLED |
				       ATALK_IGRP_CONFIGURED);
		    atigrp2_config_idb(idb->phys_idb, csb->sense);
		}
	    }
	} else {
	    printf(atalk_badcmd, "AppleTalk/EIGRP must be enabled first");
	}
	break;
      default:
	printf("\n%%Error in route-protocol subcommand in %s @ %d",
	       __FILE__,__LINE__);
	return;
    }
}

/*
 * atigrp2_encap_fixup
 * set igrp2 timers to proper values after an encapsulation change
 */ 

static void atigrp2_encap_fixup (idbtype *swidb, int old_encap)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;

    ddb = atalk_igrp2_ddb;
    if (ddb == NULL)
        return;

    iidb = igrp2_find_iidb(ddb, swidb);
    if (iidb && igrp2_default_timers(iidb)) {
	iidb->hello_interval = igrp2_def_helloint(swidb, ddb->linktype);
	iidb->holdtime = igrp2_def_holdint(swidb, ddb->linktype);
    }
}

/*
 * atigrp2_enable_process_wait
 *
 * Enable process blocking predicate for Hello process
 *
 * The process is set up to wait on the input queue.
 */
static void atigrp2_enable_process_wait (ddbtype *ddb)
{
    atalk_ipb_t *ipb;

    ipb = ddb->pdb;
    if (!ipb)
	return;
    process_watch_queue(ipb->pktQ, ENABLE, RECURRING);
}

/*
 * atigrp2_input_packet
 *
 * Returns the next input packet, or NULL if it is not there.
 * Returns the source of the packet as well.
 */
static paktype *atigrp2_input_packet (ddbtype *ddb, igrp2addrtype *source)
{
    paktype *pak;
    atalk_ipb_t *ipb;

    ipb = ddb->pdb;
    if (!ipb)
	return(NULL);
    pak = process_dequeue(ipb->pktQ);
    if (pak) {
	source->atalk_address = atalk_pak_srcaddr(pak);
    }
    return(pak);
}

/*
 * atigrp2_process
 *
 * pre-process an incoming EIGRP packet.
 *
 * We return FALSE if the packet hasn't been completely handled (ie,
 * dropped) and TRUE if we completely processed the packet in here.
 */
static boolean atigrp2_process (paktype *pak)
{
    if (!pak)
	return (TRUE);			/* pretend we swallowed a null pak */

    atalk_stat[ATALK_IGRP_IN]++;
    if (!atalk_check(pak, DDP_TYPE_Invalid, ATALK_IGRP_IGNORED, FALSE))
	return (TRUE);			/* we swallowed the packet */
    return(FALSE);
}

/*
 * atigrp2_handle_packet
 *
 * Handle a received packet.
 *
 * Disposes of the packet properly.
 */
static void atigrp2_handle_packet (ddbtype *ddb, idbtype *phys_idb,
				   paktype *pak)
{
    dual_peertype *peer;
    ataddrtype	srcaddr;
    igrp2type	*igrp2;
    atigrp2_cabletype *cable;
    ushort	srcnet;
    tinybool	idb_up;
    atalkidbtype *idb;
    char const * idb_name;
    tinybool     do_debug;

    idb = atalk_getidb(phys_idb);
    idb_name = idb_get_namestring(phys_idb);
    do_debug = at_debug(atalkigrp_packet_debug, ATALK_INTERFACE_MASK,
			phys_idb);
    /*
     * rcvd a packet on an interface which does not have IGRP
     * enabled, pitch it. ???
     */
    if (!(atalk_igrp_running && (idb->atalk_rp & ATALK_IGRP_ENABLED))){
	if (do_debug)
	    buginf("\nAT: %s pitching EIGRP packet received; "
		   "EIGRP not enabled", idb_name);
	atalk_dispose(ATALK_IGRP_IGNORED, pak);
	return;
    }
    
    /*
     * Process this packet if we are operational or on a
     * point-to-point link.
     */
    idb_up = atalkif_InterfaceUp(idb);
    if (!idb_up) {
	datagram_done(pak);
	return;
    }
    atalk_stat[ATALK_IGRP_IN]++;
    
    if (do_debug) {
	atalk_pak_printgood(pak, "EIGRP pak", idb);
    }
    
    /*
     * Process fixed header.
     */
    srcaddr = atalk_pak_srcaddr(pak);
    igrp2 = (igrp2type *)pak->transport_start;
    cable = NULL;
    
    if (igrp2->opcode == IGRP_OPC_HELLO) {
	/*
	 * We can't trust hellos from non-operational routers if we
	 * are a discovery port. However, as a seed port, they can kick
	 * us out of the 'acquire' mode. Either way, never pass non-
	 * operational hellos to IGRP to form a peer adjacency.
	 *
	 * The other hello packet which should never make it into
	 * the DUAL/EIGRP engine are hello packets which have
	 * router id's the same as ours. This is a big no-no.
	 */
	cable = atigrp2_hello_valid(pak, phys_idb);
	if (cable != NULL) {	/* hello is valid & has cable-TLV */
	    if (!atalkif_operational(idb)) {
		atigrp2_AcquirePortInfo(pak, cable, idb);
		datagram_done(pak);
		return;
	    } else {
		if (GETLONG(&cable->routerid) == atalk_rtr_id) {
		    errmsg(AT_ERR_COMPATERR4, srcaddr, atalk_rtr_id);
		    atalkif_ConfigError(idb, "router ID conflict");
		    atalk_stat[ATALK_IGRP_BAD]++;
		    datagram_done(pak);
		    return;
		}
	    }
	} else {		/* invalid hello or hello/ack */
	    if (GETLONG(&igrp2->ack) == 0) { /* non-ack hello */
		datagram_done(pak);
		return;
	    }
	}
    } else {			/* not a hello packet */
	if (!idb_up) {
	    atalk_stat[ATALK_IGRP_IGNORED]++;
	    datagram_done(pak);
	    return;
	}
    }
    
    /*
     * Validate the incoming packet.
     */
    if (!atalk_validSrcAddress(idb, srcaddr)) {
	if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, phys_idb))
	    buginf("\nAT: %s pitching EIGRP packet from invalid "
		   "source address %a", idb_name, srcaddr);
	atalk_dispose(ATALK_IGRP_BAD, pak);
	return;
    }
    
    /*
     * Only accept packets from our source network
     */
    srcnet = atalk_netonly(srcaddr);
    if ((srcnet < idb->atalk_cablestart) ||
	(srcnet > ((idb->atalk_cableend == 0) ?
		   (idb->atalk_cablestart) : (idb->atalk_cableend)))) {
	if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, phys_idb))
	    buginf("\nAT: %s pitching EIGRP packet from remote net "
		   "source address %a", idb_name, srcaddr);
	atalk_dispose(ATALK_IGRP_BAD, pak);
	return;
    }
    
    /*
     * Otherwise, we know that the AppleTalk configuration
     * info for this cable is correct, let's introduce the packet
     * into the DUAL engine. This routine will do the sequence
     * # checking, the peer discovery and peer addition. The
     * actual meat in a packet more substantial than a hello will
     * be processed in atigrp2_receive and the various dual
     * routines called.
     */
    if (cable != NULL) {
	ddb->router_id = GETLONG(&cable->routerid);
    }
    peer = igrp2_receive(ddb, pak, (igrp2addrtype *) &srcaddr, phys_idb);
    if (do_debug) {
	if (peer)
	    buginf("\natigrp2_router: peer is %a",
		   peer->source.atalk_address);
	else
	    buginf("\natigrp2_router: NULL peer after igrp2_receive");
    }
    
    /*
     * Switch on packet type.  Routines must check version number.
     */
    if (peer) {
	switch (igrp2->opcode) {
	  case IGRP_OPC_UPDATE:
	    atalk_stat[ATALK_IGRP_UPDATE_IN]++;
	    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\natigrp2_router: received UPDATE from %a",
		       peer->source.atalk_address);
	    atigrp2_receive(ddb, peer, pak, igrp2);
	    break;
	    
	  case IGRP_OPC_QUERY:
	    atalk_stat[ATALK_IGRP_QUERY_IN]++;
	    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\natigrp2_router: received QUERY from %a",
		       peer->source.atalk_address);
	    atigrp2_receive(ddb, peer, pak, igrp2);
	    break;
	    
	  case IGRP_OPC_REPLY:
	    atalk_stat[ATALK_IGRP_REPLY_IN]++;
	    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\natigrp2_router: received REPLY from %a",
		       peer->source.atalk_address);
	    atigrp2_receive(ddb, peer, pak, igrp2);
	    break;
	    
	  case IGRP_OPC_HELLO:
	    atalk_stat[ATALK_IGRP_HELLO_IN]++;
	    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\natigrp2_router: received HELLO from %a",
		       peer->source.atalk_address);
	    /*
	     * Hellos not processed by atigrp2_receive(), but we
	     * should update the appletalk neighbor info
	     */
	    if (cable && peer)
		peer->router_id = GETLONG(&cable->routerid);
	    break;
	    
	  default:
	    atalk_stat[ATALK_IGRP_BAD]++;
	    if (at_debug(atalkigrp_debug, ATALK_INTERFACE_MASK, phys_idb))
		buginf("\natigrp2_router: %s received invalid pak fm %#a",
		       idb_name, peer->source.atalk_address);
	    break;
	}
	atigrp2_UpdateNeighborEntry(ddb, peer, igrp2->opcode);
    }
    datagram_done(pak);
}

/*
 * atigrp2_router
 * Main IGRP2 routing process. 
 */
#define PACKET_LIMITER 20		/* Max packets per pass */
static process atigrp2_router (void)
{
    paktype	*pak;
    ddbtype	*ddb;
    atalk_ipb_t	*ipb;
    idbtype	*idb;			/* interface we received packet on */
    ulong	packet_limiter;
    igrp2_input_packet *inpak;
    ulong	major, minor;
    boolean	once_around;

    process_wait_on_system_init();
    ipb = atalk_igrp2_ipb;

    atalkigrpQ = create_watched_queue("AT IGRP packets", 0, 0);
    ddb = ipb->ddb;
    ddb->routerid.atalk_address = atalk_rtr_id;
    ipb->pktQ = atalkigrpQ;
    atalk_BindSocket(ipb->pktQ, "EIGRP", ATALK_SOCK_IGRP2, atigrp2_process);

    /* Wake up the hello process. */

    process_set_boolean(ddb->run_hello_process, TRUE);

    /* Set up the event predicates. */

    process_watch_queue(ddb->ddb_inputQ, ENABLE, RECURRING);
    process_watch_queue(ipb->workQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&ddb->ddb_master_timer, ENABLE);

    mgd_timer_start(&ddb->ddb_pdm_timer, atalk_igrpGCInterval);

    while (TRUE) {
	if (ddb->ddb_change_queue) {	/* Better be empty now! */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	}
	process_wait_for_event();

	/* Empty the event queue.  We process events in our own order. */

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
	      case QUEUE_EVENT:
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}

	/*
	 * Top of inner loop.  We need to bleed off all of the event-causing
	 * stuff before we go to sleep, even if we hit our limiters.  We
	 * exit this loop if we actually exhausted everything.
	 */
	do {
	    once_around = FALSE;
	    /*
	     * First empty the IGRP packet queue
	     */
	    packet_limiter = PACKET_LIMITER;
	    while ((inpak = process_dequeue(ddb->ddb_inputQ))) {
		process_may_suspend();

		/* Process the packet. */

		pak = inpak->packet;
		idb = inpak->if_input;
		atigrp2_handle_packet(ddb, idb, pak);
		chunk_free(igrp2_inpak_chunks, inpak);
		
		/* Do something else if we've done lots of packets. */
		
		packet_limiter--;
		if (!packet_limiter) {
		    once_around = TRUE;
		    break;
		}
	    }

	    /* Process the work queue. */
	    
	    if (!process_is_queue_empty(ipb->workQ))
		once_around = once_around || atigrp2_process_workq(ipb);

	    /* Process any timer expirations. */
	    
	    if (mgd_timer_expired(&ddb->ddb_master_timer))
		once_around = once_around || igrp2_process_managed_timers(ddb);
	    
	} while (once_around);
    }
}

/*
 * atigrp2_pdm_timer_expiry
 *
 * Handle the expiration of the PDM timer.  For us, this means that it's
 * time to run the ager.
 */
static void atigrp2_pdm_timer_expiry (ddbtype *ddb, mgd_timer *timer)
{
    atigrp2_PathAger(FALSE);	/* delete any pending D paths */
    atroute_pathGC(AT_PATH_IGRP, "EIGRP"); /* GC deleted paths */
    mgd_timer_start(timer, atalk_igrpGCInterval);
}

/*
 * atigrp2_interface_change
 *
 * Process a change in metric on an interface.
 */
static void atigrp2_interface_change (ddbtype *ddb, igrp2_idbtype *iidb)
{
    atalkidbtype *idb;

    idb = atalk_getidb(iidb->idb);
    if (idb)
	atigrp2_route_adjust(idb, FALSE);
}

/*
 * atigrp2_ddbinit
 * Initialize the ddb data structure.
 */
static void
atigrp2_ddbinit (ddbtype *ddb, atalk_ipb_t *ipb)
{
    if (!(ddb && ipb))
	return;

    ddb->pdb = ipb;
    ipb->ddb = ddb;

    ddb->name = "AT/EIGRP";
    ddb->linktype = LINK_APPLETALK;
    ddb->k1 = IGRP_K1_DEFAULT;
    ddb->k2 = IGRP_K2_DEFAULT;
    ddb->k3 = IGRP_K3_DEFAULT;
    ddb->k4 = IGRP_K4_DEFAULT;
    ddb->k5 = IGRP_K5_DEFAULT;
    ddb->max_hopcount = IGRP_MAXHOPS;
    ddb->active_time = IGRP2_DEF_ACTIVE_TIME;
    /* ??? Set the asystem to 1 for now. */
    ddb->asystem = 1;			/* ipb->asystem */

    /* Set up the transfer vectors. */

    ddb->add_item	= atigrp2_add_item;
    ddb->addrcopy 	= atigrp2_addrcopy;
    ddb->addrmatch	= atigrp2_addrmatch;
    ddb->advertise_request = atigrp2_advertise_request;
    ddb->buildpacket	= NULL;
    ddb->compare_external = NULL;
    ddb->enable_process_wait = atigrp2_enable_process_wait;
    ddb->exterior_check	= NULL;
    ddb->exterior_flag	= NULL;
    ddb->headerptr	= atigrp2_headerptr;
    ddb->hellohook	= atigrp2_hellohook;
    ddb->iidb_cleanup	= NULL;
    ddb->iidb_show_detail = NULL;
    ddb->input_packet	= atigrp2_input_packet;
    ddb->interface_change = atigrp2_interface_change;
    ddb->interface_up	= atigrp2_InterfaceUp;
    ddb->item_size	= atigrp2_item_size;
    ddb->localaddr	= atigrp2_localaddr;
    ddb->mmetric_fudge	= NULL;
    ddb->ndbbucket	= atigrp2_ndbbucket;
    ddb->ndb_done	= atigrp2_ndb_done;
    ddb->ndb_new	= atigrp2_ndb_new;
    ddb->netmatch	= atigrp2_netmatch;
    ddb->paklen		= atigrp2_paklen;
    ddb->pdm_timer_expiry = atigrp2_pdm_timer_expiry;
    ddb->peerbucket	= atigrp2_peerbucket;
    ddb->peershowhook	= atigrp2_peershowhook;
    ddb->peerstatehook	= atigrp2_peerstatehook;
    ddb->printaddr	= atigrp2_printaddr;
    ddb->printnet	= atigrp2_printnet;
    ddb->rtdelete	= atigrp2_rtdelete;
    ddb->rtgetmetric	= atigrp2_rtgetmetric;
    ddb->rtmodify = atigrp2_rtdelete;
    ddb->rtupdate	= atigrp2_rtupdate;
    ddb->sendpacket	= atigrp2_write;
    ddb->transport_ready = NULL;
    ddb->set_mtu        = atigrp2_set_mtu;
    ddb->extract_tag    = NULL;

    /* Set up random detritus. */

    ddb->packet_overhead = DDP_OVERHEADBYTES;
    ddb->routerid.atalk_address = atalk_rtr_id;
    dual_allocate_free_lists();
    dual_init_ddb(ddb, "AT-EIGRP Hello", sizeof(atigrp2_extdatatype),
		  sizeof(atigrp2_worktype));
    mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);
    enqueue(&ddbQ, ddb);
}

/*
 * Start all IGRP processing.
 */
static void
atigrp2_start (void)
{
    atalkidbtype	*idb;
    atalk_ipb_t	*ipb;
    ddbtype	*ddb;

    if (atalk_igrp_running || atalk_igrp_started) { 
	atalk_igrp_running = atalk_igrp_started = TRUE; 
	return; 
    } 
    if ((ipb = malloc_named(sizeof(atalk_ipb_t), "Atalk IPB")) == NULL) {
	return;
    }
    if ((ddb = malloc_named(sizeof(ddbtype), "Atalk DDB")) == NULL) {
	free(ipb); 
	return; 
    } 
    atigrp2_ipbinit(ipb, 1); 
    atalk_igrp2_ipb = ipb; 
    atigrp2_ddbinit(ddb, ipb);
    atalk_igrp2_ddb = ddb;
    ipb->workQ = create_watched_queue("AT-EIGRP work queue", 0, 1);

    atroute_InitPathTree(AT_PATH_IGRP, "AT/EIGRP Path Tree");
    atroute_RegisterProtocol(AT_PATH_IGRP,
			     AT_ROUTE_LEAK_IGRP,
			     AT_ROUTE_UNLEAK_IGRP,
			     AT_ROUTE_LEAKED_IGRP,
			     atigrp2_redist_path,
			     atigrp2_unredist_path,
			     atigrp2_redist_all,
			     atigrp2_validate_route);
    reg_add_atalk_validate_route(atigrp2_validate_route,
				 "atigrp2_validate_route");
    reg_add_encap_fixup(atigrp2_encap_fixup, "atigrp2_encap_fixup");
    /* Delete all interfaces from IGRP's table */
    ipb->router_pid = process_create(atigrp2_router, "AT EIGRP", NORMAL_STACK,
				     PRIO_NORMAL);
    if (ipb->router_pid == NO_PROCESS) { 
	free(ipb); 
	free(ddb);
	atroute_UnregisterProtocol(AT_PATH_IGRP);
	atroute_DeletePathTree(AT_PATH_IGRP);
	return;
    }
    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_rp & ATALK_IGRP_CONFIGURED) {
	    idb->atalk_rp |= ATALK_IGRP_ENABLED;
	    atigrp2_config_idb(idb->phys_idb, TRUE);
	}
    }
    atalk_igrp_running = atalk_igrp_started = TRUE; 
}

static void atigrp2_register_thyself (void)
{
    reg_add_atalk_shutdown(atigrp2_stop, "atigrp2_stop");
    reg_add_atalk_age_neighbor(atigrp2_age_neighbor, "atigrp2_age_neighbor");
    reg_add_atalk_if_config(atigrp2_config_idb, "atigrp2_config_idb");
    reg_add_atalk_if_operational(atigrp2_if_change_callback,
				 "atigrp2_if_change_callback");
    reg_add_atalk_if_disable(atigrp2_if_change_callback,
			     "atigrp2_if_change_callback");
    reg_add_atalk_show_globals(atigrp2_show_globals_add,
			       "atigrp2_show_globals_add");
    reg_add_atalk_show_private(atigrp2_show_guts, "atigrp2_show_guts");
    reg_add_atalk_show_neighbor(atigrp2_show_nbr_add, "atigrp2_show_nbr_add");
    reg_add_atalk_show_traffic(atigrp2_show_tfc_add, "atigrp2_show_tfc_add");
    reg_add_atalk_forceupdate(atigrp2_forceupdate, "atigrp2_forceupdate");
}

static void atigrp2_init (subsystype *subsys)
{
    atigrp2_register_thyself();

    atigrp2_parser_init();
    atigrp2_debug_init();
    atalk_igrp2_ipb = NULL;
    atalk_igrp2_ddb = NULL;
}

/*
 * Appletalk subsystem header
 */

#define ATIGRP2_MAJVERSION	1
#define ATIGRP2_MINVERSION	0
#define ATIGRP2_EDITVERSION	1

SUBSYS_HEADER(atigrp2, ATIGRP2_MAJVERSION, ATIGRP2_MINVERSION, 
	      ATIGRP2_EDITVERSION,
	      atigrp2_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk, eigrp", "req: atalk, eigrp");


