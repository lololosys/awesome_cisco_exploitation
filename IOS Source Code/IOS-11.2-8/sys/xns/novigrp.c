/* $Id: novigrp.c,v 3.11.10.26 1996/09/10 18:10:48 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novigrp.c,v $
 *------------------------------------------------------------------
 * Novell IGRP(2) specific routines.
 *
 * November 1992, Joanne Boyle
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novigrp.c,v $
 * Revision 3.11.10.26  1996/09/10  18:10:48  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.11.10.25  1996/09/04  23:47:20  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.11.10.24  1996/08/28  13:25:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.10.23  1996/08/27  20:25:29  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.11.10.22  1996/08/07  09:05:31  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.10.21  1996/07/26  00:09:10  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.11.10.20  1996/07/10  18:40:38  akr
 * CSCdi61153:  Misaligned access during ipx eigrp routing tests
 * Branch: California_branch
 * (Fixes c-style issues in the previous fix)
 *
 * Revision 3.11.10.19  1996/07/05  07:52:17  akr
 * CSCdi61153:  Misaligned access during ipx eigrp routing tests
 * Branch: California_branch
 *
 * Revision 3.11.10.18  1996/06/28  02:49:30  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.11.10.17  1996/06/28  02:40:47  dkatz
 * CSCdi61491:  EIGRP may lose routes
 * Branch: California_branch
 * Only ignore the routes we are supposed to ignore.
 *
 * Revision 3.11.10.16  1996/06/21  16:50:04  sluong
 * CSCdi57042:  ipx floating static route kicks in after 4 min with EIGRP
 * Branch: California_branch
 *
 * Revision 3.11.10.15  1996/06/18  16:43:23  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.11.10.14  1996/06/17  23:40:36  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.10.13  1996/06/17  22:21:06  sluong
 * CSCdi60174:  crash in nov_get_backup_servers
 * Branch: California_branch
 *
 * Revision 3.11.10.12  1996/05/21  10:11:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.10.11  1996/05/18  00:05:34  sluong
 * CSCdi57728:  Crash in nov_filtercheck
 * Branch: California_branch
 *
 * Revision 3.11.10.10  1996/05/17  20:47:58  sluong
 * CSCdi57728:  Crash in nov_filtercheck
 * Branch: California_branch
 *
 * Revision 3.11.10.9  1996/05/07  17:26:52  dkatz
 * CSCdi55252:  EIGRP SAP queue is not emptied when last peer is gone
 * Branch: California_branch
 * Flush the SAP queue when the last peer goes down.
 *
 * Revision 3.11.10.8  1996/05/04  02:18:52  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.11.10.7  1996/05/03  02:19:23  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.11.10.6  1996/04/02  07:32:51  dkatz
 * CSCdi52398:  EIGRP SAP defers too much to DUAL
 * Branch: California_branch
 * Interleave SAP and DUAL packets.  While we're in there, default SAP
 * split-horizon off again;  it's still messed up.
 *
 * Revision 3.11.10.5  1996/04/02  07:26:05  dkatz
 * CSCdi52251:  IPX EIGRP announces route on down interface
 * Branch: California_branch
 * Don't forget to remove the connected route from the topo table
 * when the interface goes down.
 *
 * Revision 3.11.10.4  1996/04/02  07:23:52  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Fix a typo that left mondo weird stuff in the backup tables.  Get rid
 * of SAP CPUHOGs.  Turn split horizon back on by default.
 *
 * Revision 3.11.10.3  1996/04/02  07:16:06  dkatz
 * CSCdi51920:  EIGRP packets miscounted
 * Branch: California_branch
 * Clean up protocol-specific packet counters.
 *
 * Revision 3.11.10.2  1996/04/02  05:38:22  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.11.10.1  1996/03/18  22:53:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.2  1996/03/16  08:00:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.1  1996/03/07  11:22:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11  1996/02/21  22:23:20  sluong
 * CSCdi49360:  IPX EIGRP : bad tic metric redistributed into RIP
 *
 * Revision 3.10  1996/02/14  23:01:32  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.9  1996/01/22  07:53:06  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1995/12/22  20:38:29  boyle
 * CSCdi31767:  ipx router rip does not add networks once configured
 *
 * Revision 3.7  1995/12/19  20:07:10  boyle
 * CSCdi37965:  ipx eigrp-cannot remove AS
 *
 * Revision 3.6  1995/12/11  04:55:21  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.5  1995/12/07  22:35:05  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/20  17:55:27  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.3  1995/11/17  19:24:00  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:59:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:39:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/26  00:53:37  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.12  1995/10/25  15:53:55  boyle
 * CSCdi41675:  SAP entries not always marked as poisened when poison is
 * recieved
 *
 * Revision 2.11  1995/10/17  00:48:41  kleung
 * CSCdi42168:  Router crash in dual_updatetopology() running AURP2 tests.
 *
 * Revision 2.10  1995/09/07  01:46:03  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.9  1995/08/08  18:01:43  boyle
 * CSCdi38319:  IPX-EIGRP: redistribution of rip routes causes routing loop
 *
 * Revision 2.8  1995/08/04  03:08:33  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.7  1995/07/01  03:39:10  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.6  1995/06/20 21:54:05  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/20  05:19:21  mikel
 * CSCdi36141:  EIGRP: random corruption caused by  linkdown event
 * failure to memlock the iidb during a linkdown_enq leads to corruption
 *
 * Revision 2.4  1995/06/19  06:44:15  bcole
 * CSCdi36032: IP-EIGRP: Connected route for shutdown passive-if stays in
 * topo tbl
 *
 * Revert CSCdi34952 changes.  During an if flap, consider
 * passive-interfaces before deciding that there must be no topology table
 * entries which need to be removed.
 *
 * Revision 2.3  1995/06/19  04:51:12  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:26:11  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:29:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include <stdarg.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "mgd_timers.h"
#include "../os/chunk.h"
#include "access.h"
#include "parser.h"
#include "subsys.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../iprouting/dual.h"
#include "../iprouting/route.h"
#include "../iprouting/igrp2-inlines.h"
#include "logger.h"
#include "xns.h"
#include "novfast.h"
#include "novell.h"
#include "novroute.h"
#include "novnlsp.h"
#include "novigrp.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../iprouting/msg_igrp2.c"	/* Not a typo, see logger.h */
#include "../iprouting/msg_igrp.c"	/* Not a typo, see logger.h */
#include "../xns/msg_ipx.c"		/* Not a typo, see logger.h */
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_ipx.h"
#include "../clns/clns_registry.h"
#include "ipx_registry.h"
#include "../lane/lane.h"
#include "media_registry.h"
 
/*
 * Storage
 */
static chunk_type *novigrp_sapQ_chunks;
static ulong novigrp_sap_serial;

static char novigrp_buffer[128];
/*
 * novigrp_printaddr
 *
 * Return an IPX address in ASCII.
 */
static char *novigrp_printaddr (igrp2addrtype *addr)
{
    sprintf(novigrp_buffer, "%x.%e", addr->nov_address.net, 
	addr->nov_address.host);
    return(novigrp_buffer);
}

/*
 * novigrp_qevent
 * Enqueue work queue event
 */
static void novigrp_qevent (novpdbtype *pdb, novigrp_worktype *work,
		     igrp2_eventtype type)
{
    work->type = type;
    process_enqueue(pdb->watched_workQ, work);
}

/*
 * novigrp_process
 * Here we figure out which IGRP2 pdb gets the datagram.
 */
void novigrp_process (paktype *pak)
{
    novpdbtype *pdb;
    igrp2type *igrp;

    if (!novell_igrp_running) {
	retbuffer(pak);
	return;
    }	
    igrp = (igrp2type *)(XNSDATASTART(pak));
    pdb = find_novigrp_pdb(GETLONG(&igrp->asystem));
    if (pdb) {
	process_enqueue(pdb->watched_inputq, pak);
    } else {
	retbuffer(pak);
	return;	 
    }
}

/*
 * novigrp_metric_fudge
 * If offset-list is specified, fixup the delay factor.
 */
static ulong novigrp_metric_fudge (ddbtype *ddb, igrp2nettype *addr,
				   idbtype *idb, int direction)
{
    novpdbtype *pdb;

    pdb = ddb->pdb;

    if (pdb && pdb->mmetric_in && ((pdb->mmetric_list_in == 0) || 
				  nov_filtercheck(addr->nov_network,
						  pdb->mmetric_list_in)))
	return(pdb->mmetric_in);
    else
	return(0);
}

/*
 * novigrp_process_routes
 * Grab metric, compute composite metric.  Then get destinations/masks
 * from packet and fire off call to dual.  Return FALSE if we run out of
 * memory, or the packet has bogus data.
 */
static boolean novigrp_process_routes (ulong *destp,
				       uchar *limit,
				       dual_newroute *route,
				       igrp2_mdecode *metric,
				       ddbtype *ddb,
				       ulong pkt_type,
				       boolean ignore_tlv,
				       ulong *nexthop_net,
				       uchar *nexthop_host,
				       dual_peertype *peer,
				       void *extd)
{
    novpdbtype *pdb;
    int filter_index;
    novigrp_extdatatype *data;
    igrp2addrtype *infosource;
    nidbtype *nidb;
    idbtype *idb;

    idb = dual_idb(route->new_iidb);
    if (!idb) {
	errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	return(FALSE);
    }

    pdb = ddb->pdb;
    infosource = &peer->source;
    igrp2_metric_decode(&route->new_vecmetric, metric);
    route->new_metric = igrp2_compute_metric(ddb, &route->new_vecmetric,
					     &route->new_dest,
					     route->new_iidb,
					     &route->new_successor_metric);
    while (destp < (ulong *)limit) {
        if (GETLONG(nexthop_net) == 0L) {
            route->new_nexthop.nov_address.net = infosource->nov_address.net;
            ieee_copy(infosource->nov_address.host, 
		      route->new_nexthop.nov_address.host);
        } else {
       	    route->new_nexthop.nov_address.net = GETLONG(nexthop_net);
            ieee_copy(nexthop_host, route->new_nexthop.nov_address.host);
        }
	route->new_dest.nov_network = GETLONG(destp);

	/* Copy the external data for external destinations. */

	if (route->new_external) {
	    data = chunk_malloc(ddb->extdata_chunk);
	    if (!data)
		return(FALSE);
	    bcopy(extd, data, sizeof(novigrp_extdatatype));
	    route->new_data = data;

	} else {			/* Not external */
	    route->new_data = NULL;
	}

	/*
	 * At this point check to see if the ignore flag should be
	 * set in the topology table.  Only consider distance
	 * less than MAXDISTANCE and must pass input filtering.
	 */
	route->new_ignore = ignore_tlv;
	if (!route->new_ignore) {
            filter_index = pdb->ifltr_all;
            if (!filter_index) {
		nidb = IPX_PRIMARY_NIDB(idb);
	        filter_index = nidb->iprotofltr[pdb->index];
	    }
            if (filter_index) {
	        if (!nov_filtercheck(route->new_dest.nov_network,
				     filter_index)) {
	           route->new_ignore = TRUE;
	       }
	    }
	}

	if (NOVIGRP_DTARGET(route->new_dest.nov_network, ddb)) {
	    buginf("\nIPXEIGRP: %x via %s metric %u",
		   route->new_dest.nov_network,
		   novigrp_printaddr(&route->new_nexthop), route->new_metric);
	}

	dual_rcvpkt(ddb, peer, route, pkt_type);
	destp++;
    }
    return(TRUE);
}

/*
 * novigrp_receive
 * Transform update, query or reply packet into internal data structures.
 */
static void novigrp_receive (ddbtype *ddb,
			     dual_peertype *peer,
			     paktype *pak,
			     igrp2type *igrp2)
{
    ipxhdrtype *ipx;
    novpdbtype *pdb;
    igrp2_gentlvtype *tlv;
    int bytes;
    uchar *limit;
    idbtype *interface;
    igrp2_idbtype *iidb;
    ushort length;
    dual_newroute route;
    novigrp_externtype *nove;
    novigrp_metrictype *novm;
    boolean ignore_packet, ignore_tlv;
    
    bzero(&route, sizeof(route));

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    pdb = ddb->pdb;
    tlv = igrp2_packet_data(igrp2);
    route.new_infosource = peer->source;
    route.new_iidb = peer->iidb;
    route.new_origin = IGRP2_ORG_IGRP2;
    bytes = ipx->len - (XNSOVERHEADBYTES + IGRP2HEADERBYTES);
    iidb = peer->iidb;
    interface  = iidb->idb;
    ignore_packet = !nov_accesscheck(pak, interface->nov_router_filterlist);

    while (bytes > 0) {

	/*
	 * Let's inject a little sanity checking.  This is the same case
	 * as default in the switch statement.
	 */
	length = GETSHORT(&tlv->length);
	if (length < sizeof(igrp2_gentlvtype))
	    goto done;

	limit = (uchar *)tlv + length;
	switch (tlv->type) {
	  case NOVIGRP_METRIC:
	    novm = (novigrp_metrictype *) tlv;
	    route.new_external = FALSE;
	    route.new_data = NULL;
	    if (!novigrp_process_routes(&novm->dest, limit, &route,
					(igrp2_mdecode *)&novm->delay, ddb,
					igrp2->opcode, ignore_packet,
					&novm->nexthop_net,
					novm->nexthop_host, peer, NULL)) {
		goto done;
	    }
	    break;

	  case NOVIGRP_EXTERNAL:
	    route.new_external = TRUE;
	    nove = (novigrp_externtype *) tlv;

	    /*
	     * If we are the source of this advertisement then reject it.
	     */
	    ignore_tlv = ieee_equal(nove->routerid,
				    ddb->routerid.nov_address.host);

	    if (!novigrp_process_routes(&nove->dest, limit, &route,
					(igrp2_mdecode *)&nove->delay, ddb,
					igrp2->opcode,
					(ignore_packet || ignore_tlv),
					&nove->nexthop_net,
					nove->nexthop_host, peer,
					&nove->routerid)) {
		goto done;
	    }
	    break;

	  default:

	    /* Skip the unknown TLV. */

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
 * novigrp_write
 * Send a datagram.
 *
 * This routine does *not* free the supplied packet!
 */
static void novigrp_write (igrp2type *igrp2, int bytes, dual_peertype *peer,
		    igrp2_idbtype *iidb, boolean priority)
{
    ipxhdrtype *ipx;
    paktype *pak;
    idbtype *interface;

    interface = iidb->idb;

    pak = igrp2_getpak(XNSHEADERBYTES + bytes);
    if (!pak) {
	return;
    }
    bcopy(igrp2, ipxdatastart(pak), bytes);
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (peer) {
	PUT2WD(ipx->ddnet, peer->source.nov_address.net);
        ieee_copy(peer->source.nov_address.host, ipx->dhost);
    } else {
	PUT2WD(ipx->ddnet, IPX_PRIMARY_NIDB(interface)->novellnet);
	ieee_copy(baddr, ipx->dhost);
    }
    PUT2WD(ipx->ssnet, IPX_PRIMARY_NIDB(interface)->novellnet);
    ieee_copy(interface->novell_node, ipx->shost);
    pak->if_output = interface;
    pak->linktype = LINK_NOVELL;
    if (priority)
	pak->flags |= PAK_PRIORITY;

    ipx->tc = 0;
    ipx->pt = NOVIGRP_PROT;
    ipx->dsock = NOVELL_SOCK_IGRP;
    ipx->ssock = NOVELL_SOCK_IGRP;
    ipx->cksum = NOVELL_NO_CHECKSUM;
    ipx->len = XNSOVERHEADBYTES + bytes;


    novell_stat[NOVELL_IGRPOUT]++;
    switch (igrp2->opcode) {
      case IGRP_OPC_UPDATE:
	novell_stat[NOVELL_UPDATE_OUT]++;
	break;
      case IGRP_OPC_QUERY:
	novell_stat[NOVELL_QUERY_OUT]++;
	break;
      case IGRP_OPC_REPLY:
	novell_stat[NOVELL_REPLY_OUT]++;
	break;
      case IGRP_OPC_IPXSAP:
	novell_stat[NOVELL_IGRP_SAPOUT]++;
	break;
      default:
	break;
    }
    nov_send(pak);
}

/*
 * novigrp_cleanup
 * Here to cleanup after an IGRP router has been shutdown.
 * Release pdb storage.
 */

static void novigrp_cleanup (novpdbtype *pdb)
{
    ddbtype *ddb;

    ddb = pdb->ddb;

    /*
     * Delete all elements off of the work Q.
     */
    while (!process_is_queue_empty(pdb->watched_workQ)) {
	chunk_free(ddb->workq_chunk, process_dequeue(pdb->watched_workQ));
    }
    delete_watched_queue(&pdb->watched_workQ);

    igrp2_free_ddb(ddb);

    /*
     * Release all packets on the input queue.
     */
    while (!process_is_queue_empty(pdb->watched_inputq)) {
	datagram_done(process_dequeue(pdb->watched_inputq));
    }
    delete_watched_queue(&pdb->watched_inputq);

    unqueue(&novpdbQ, pdb);
    free_novpdbindex(pdb);
    free(pdb);
}

/*
 * novigrp_timers
 * Sets that maximum allowable time a route may remain in active state before
 * clearing
 */
void novigrp_timers (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    pdb = csb->protocol;
    ddb = pdb->ddb;

    if (csb->nvgen) {
	if (ddb->active_time == IGRP2_DEF_ACTIVE_TIME)
	    return;

	nv_write(TRUE, "%s", csb->nv_command);
	if (ddb->active_time != 0)
	    nv_add(TRUE, " %d", ddb->active_time / ONEMIN);
	else
	    nv_add(TRUE, " disabled");
	return;
    }

    if (csb->sense) {
	ddb->active_time = GETOBJ(int,1) * ONEMIN;
	if (GETOBJ(int,1) == 0) {
	    mgd_timer_stop(&ddb->ddb_active_timer);
	} else {
	    mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);
	}
    } else {
	ddb->active_time = IGRP2_DEF_ACTIVE_TIME;
	mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);
    }
}	

void
novigrp_event_logging (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    pdb = csb->protocol;
    ddb = pdb->ddb;

    if (csb->nvgen) {
	nv_write(ddb->log_events, "%s", csb->nv_command);
	return;
    }
    ddb->log_events = csb->sense;
}

/*
 * novigrp_event_log_size
 *
 * set/nvgen event log size
 */
void novigrp_event_log_size (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    pdb = csb->protocol;
    ddb = pdb->ddb;

    if (csb->nvgen) {
	nv_write(ddb->max_events != DUAL_DEFAULT_MAX_EVENTS, "%s %d",
		 csb->nv_command, ddb->max_events);
    } else {
	if (!dual_init_events(ddb, GETOBJ(int,1), csb->sense))
	    printf(nomemory);
    }
}

/*
 * novigrp_event_log_type
 *
 * set/nvgen event log types
 */
void novigrp_event_log_type (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    pdb = csb->protocol;
    ddb = pdb->ddb;
    dual_configure_logging(csb, ddb, GETOBJ(int,1), GETOBJ(int,2),
			   GETOBJ(int,3));
}

/*
 * novigrp_log_nbr_changes
 *
 * set/nvgen neighbor change logging
 */
void novigrp_log_nbr_changes (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    pdb = csb->protocol;
    ddb = pdb->ddb;

    if (csb->nvgen) {
	nv_write(ddb->log_adj_change != IGRP2_DEFAULT_LOG_ADJ_CHANGE,
		 "%s%s", ddb->log_adj_change ? "" : "no ", csb->nv_command);
    } else {
	ddb->log_adj_change = csb->sense;
    }
}

/*
 * novigrp_rdbupdate
 * Given a routing block, update the IGRP specific data
 */
static void novigrp_rdbupdate (dual_rdbtype *drdb, novrdbtype *rdb)
{
    idbtype *interface;
    igrp2metric *metric;

    if (!rdb || !drdb)
	return;
    interface = dual_idb(drdb->drdb_iidb);
    metric = &drdb->drdb_vecmetric;
    rdb->protocol_info.ripigrp_info.delay = metric->delay;
    rdb->protocol_info.ripigrp_info.bandwidth = metric->bandwidth;
    rdb->protocol_info.ripigrp_info.load = metric->load;
    rdb->protocol_info.ripigrp_info.reliability =
	metric->reliability ? metric->reliability :
	    interface->hwptr->reliability;
    rdb->protocol_info.ripigrp_info.mtu =
	metric->mtu ? metric->mtu : interface->hwptr->maxmtu;
    rdb->protocol_info.ripigrp_info.hopcount = metric->hopcount;
}

/*
 * novrt_update
 * Perform updating of the IPX routing table.
 * Only IGRP is using this routine to update the routing table today, in 
 * the future RIP should also be converted to use it.
 */

static novrdbtype *novrt_update (novpdbtype *pdb, ulong dstnet,
				 ulong snet, uchar *shost, idbtype *idb,
				 ushort hopcount, int delay, ulong metric,
				 ulong igrp_delay, int ext_hop, int ext_delay,
				 ulong flags, backuptype *backup_info)
{
    novndbtype *p;
    int i;
    novrdbtype *pp, *prev, *last_pp;
    novpdbtype *old_pdb;
    boolean interior;
    boolean debug, edebug;
    backuptype *old_backup_info;

    debug = NOVIGRP_DTARGET(dstnet, ((ddbtype *)pdb->ddb));
    edebug = NOVIGRP_DETARGET(dstnet, ((ddbtype *)pdb->ddb));
    interior = FALSE;
    old_pdb = NULL;
    if (debug)
	buginf("\nIPXEIGRP: Received update from %x.%e for net %x", 
		snet, shost, dstnet);

    /* Immediate bail if network 0/-1 (-2 allowed when default off) */
    if (NOV_ILLEGAL_NET(dstnet)) {
	if (debug || edebug)
	    buginf(", ignoring 0/-1");
	return(NULL);
    }
    
    if (idb == nullidb) {
	if (debug || edebug)
	    buginf(", ignoring, no interface");
	return(NULL);
    }

    p = novrip_find_rt(dstnet, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT);

    /*
     * Not found, enter it.
     */
    if (p == NULL) {
        if (metric == METRIC_INACCESSIBLE)
            return(NULL);
	/*
	 * Allow the few seconds of non-reliable back-wash. Will check
	 * for better routes, later in a validate check that is done
	 * when this ndb is ultimately freed...
	 */
	if (nov_nonreliable_ndb_on_free_queue(dstnet, NOV_NETMASK_ALL)) {
	    return(NULL);
	}
        p = nov_ndb_malloc();
        if (p == NULL)
            return(NULL);
        pp = malloc_named(sizeof(novrdbtype), "IPX RDB");
        if (pp == NULL) {
            nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	    return(NULL);
        }
        p->path_list = p->nextpath = pp;
        p->dstnet = dstnet;
        TIMER_STOP(p->holddownuntil);
        p->hopcount = hopcount; /* Is set to zero for igrp */
        p->delay = p->last_delay = delay; 	/* In clock ticks */
	p->igrp_delay = igrp_delay; /* In 10s of microseconds, zero for rip */
        p->ext_hop = ext_hop;
	p->ext_delay = ext_delay;
	p->path_metric = metric; /* set to METRIC_INACCESSIBLE for rip */
	p->options = IPX_NDB_OPTION_VALID;
	p->novpdbindex = pdb->index;
        pp->nidb = IPX_PRIMARY_NIDB(idb);

        pp->srcnet = snet;
        ieee_copy(shost, pp->srchost);
        GET_TIMESTAMP(pp->timer);
	/*
	 * Add the route to the hash table if netmask is -1.
	 */
	p->rt_mask = NOV_NETMASK_ALL;
	p->keylen = p->masklen = IPXRADIXKEYLEN;
	i = NOV_NETHASH(dstnet);
	p->next = (*novell_table)[i];
	(*novell_table)[i] = p;
        p->paths++;
        if (debug || edebug) 
            buginf("\nIPXEIGRP: create route to %x via %x.%e, metric %u",
                   dstnet, snet, shost, metric);
	/*
	 * Possibly invalidate existing cache entries which are
	 * currently reachable via the default route.
	 */
	novcache_increment_version(NULL, NOVCACHE_IGRP_NEW_ROUTE, dstnet, 
				   NOV_NETMASK_ALL);
        novell_routes++;
        p->flags = pdb->proctype | flags;            
	p->procmask = pdb->mask;
	pp->procmask = pdb->mask;
	nov_network_redist(pdb, p, ROUTE_UP);
	p->backup_info = backup_info;
	novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_CHANGE);
        return(pp);
    }
	
    /* Don't override static/connected/internal routes, ever. */

    if (p->flags & (NET_STATIC | NET_CONNECTED | NET_INTERNAL)) {
	if (!(p->flags & NET_FLOATING_STATIC)) {
	    if (debug || edebug)
	        buginf("\nIPXEIGRP: %x not added, entry in table is"
	   	      " static/connected/internal", dstnet);
	    return(NULL);
	}
    }
    /*
     * Exterior eigrp routes never override interior igrp routes.
     */
    if (p->flags & NET_IGRP) {
	if (!(p->flags & NET_DEXTERIOR) && (flags & NET_DEXTERIOR) &&
	    !TIMER_RUNNING(p->holddownuntil)) {
	    if (debug || edebug)
	        buginf("\nIPXEIGRP: %x not added, interior in table", dstnet);
	    return(NULL);
	}
	if ((p->flags & NET_DEXTERIOR) && !(flags & NET_DEXTERIOR))
	    interior = TRUE;
    }
    
    /* If RIP or NLSP route in the table and a new IGRP route is being 
     * received and the IGRP external hop is greater than the external
     * metric in the table, then don't accept the new IGRP route.
     */

    if ((p->flags & (NET_RIP | NET_NLSP)) && (ext_hop > p->hopcount)) {
	if (debug || edebug)
	   buginf("\nIPXEIGRP: %x not added, hop count in table %d ext hop in update %d",
			dstnet, p->hopcount, ext_hop);
        return(NULL);
    }
    
    /*
     * Equal metric, add multiple path. Also, update hopcount, delay and net
     * type fields, since they could theoretically fluctuate independent
     * of the metric. A metric equal to or less than the metric we had when 
     * we put a route into holddown will resurrect it. 
     */
    if (hopcount < novell_infinity && 
	NOVELL_EQUAL_ROUTE_INFO(hopcount, delay, p) &&
	(p->path_metric == metric)) {
        TIMER_STOP(p->holddownuntil);
        p->hopcount = hopcount;
        p->delay = p->last_delay = delay;
	/* Put best delay of all paths in p->igrp_delay */
	if (igrp_delay < p->igrp_delay)
	   p->igrp_delay = igrp_delay; 
	p->ext_hop = ext_hop;
 	p->ext_delay = ext_delay;
	p->path_metric = metric;
        last_pp = pp = p->path_list;
        while (pp) {
            last_pp = pp;
            /* One host, one vote */
            if (pp->srcnet == snet && ieee_equal(pp->srchost, shost) &&
		IPX_PRIMARY_NIDB(idb) == pp->nidb) {
                GET_TIMESTAMP(pp->timer);
                return(pp);
            } 
            pp = pp->next;
        }
        if (p->paths < novell_maxpaths) {
            pp = malloc_named(sizeof(novrdbtype), "IPX RDB");
            if (pp == NULL)
                return(NULL);
            last_pp->next = pp;
            p->paths++;
            p->flags = pdb->proctype | flags;             /* used as dynamic */
	    p->procmask = pdb->mask;
	    pp->procmask = pdb->mask;
            if (debug || edebug)
		buginf("\nIPXEIGRP: Adding %d multiple paths for Net %x",
                       p->paths, dstnet);
            pp->nidb = IPX_PRIMARY_NIDB(idb);
            pp->srcnet = snet;
            ieee_copy(shost, pp->srchost);
            GET_TIMESTAMP(pp->timer);
	    novcache_increment_version(NULL, NOVCACHE_IGRP_ADD_PATH, dstnet, 
				       NOV_NETMASK_ALL);
	    novell_mark_rdb_changed(p, pp, p->flags,
				    IPX_ROUTE_EVENT_CHANGE);
	    return(pp);
        } 
    }

    /*
     * Used in the better metric and worse metric cases.
     */
    old_pdb = nov_proctable[p->novpdbindex];

    /*
     * Better metric. If metric is METRIC_INACCESSIBLE, it can never be better.
     */
    if (((metric != METRIC_INACCESSIBLE) &&
	 (interior || NOVELL_BETTER_ROUTE_INFO(hopcount, delay, p))) ||
	(metric < p->path_metric)) {

        if (debug || edebug) {
            buginf("\nIPXEIGRP: better [%u/%d/%u] route for %x from %x.%e,\n         flushing old [%u/%d/%u] route/paths",
                   delay, hopcount, metric, dstnet, snet, shost,
                   p->delay, p->hopcount, p->path_metric);
	buginf("\n         external hop count in table %d, in update %d",
		p->ext_hop, ext_hop);
	} 
	if (old_pdb != pdb) {
	    if (old_pdb->lost_route)
		(*old_pdb->lost_route)(old_pdb, p);
	    nov_network_redist(old_pdb, p, ROUTE_DOWN);
	}

	novcache_increment_version(NULL, NOVCACHE_IGRP_BETTER_METRIC, dstnet,
				   NOV_NETMASK_ALL);
	p->options &= ~IPX_NDB_OPTION_VALID;
	novell_routes--;
	/*
	 * Absorb current route info into backup structure:
	 */
	ipx_ndb_self_backup(p);
	old_backup_info = p->backup_info;
	p->backup_info = NULL;
	novrip_del_rt(p->dstnet, p->rt_mask, 
		      IPX_ROUTE_DELETE_RETAIN_CONNECTED);	
	pp = novrt_update(pdb, dstnet, snet, shost, idb, hopcount, delay,
			  metric, igrp_delay, ext_hop, ext_delay, flags,
			  old_backup_info);
	return(pp);
    }
    if (NOVELL_WORSE_ROUTE_INFO(hopcount, delay, p) ||
         (hopcount >= novell_infinity) || (metric == METRIC_INACCESSIBLE)) {
        prev = (novrdbtype *) (&p->path_list);
        pp = p->path_list;
        while (pp) {
            /*
             * Worse metric from same host with same protocol
	     */
            if ((snet == pp->srcnet) && ieee_equal(pp->srchost, shost) &&
		(IPX_PRIMARY_NIDB(idb) == pp->nidb) &&
		(pdb->mask == pp->procmask)) {
                if (p->paths == 1) {
                    if (TIMER_RUNNING(p->holddownuntil))
                        return(NULL);
		    novcache_increment_version(NULL, 
					       NOVCACHE_IGRP_WORSE_METRIC_LAST, 
					       dstnet, NOV_NETMASK_ALL);
		    if (old_pdb != pdb)
		        nov_network_redist(old_pdb, p, ROUTE_DOWN);
                    p->delay = p->last_delay = delay;
		    p->igrp_delay = igrp_delay;
                    p->hopcount = hopcount;
		    p->ext_hop = ext_hop;
		    p->ext_delay = ext_delay;
		    p->path_metric = metric;
                    TIMER_START(p->holddownuntil,
				pp->nidb->idb->nov_update_interval);
		    if (old_pdb == pdb)
		        nov_network_redist(pdb, p, ROUTE_MODIF);
		    else
		        nov_network_redist(pdb, p, ROUTE_UP);
                    if (debug || edebug)
                        buginf("\nIPXEIGRP: poison route to %x via %e, hops %d delay %d",
			 dstnet, shost, hopcount, delay);
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_CHANGE);
		    return(pp);
                } else {
		    novcache_increment_version(NULL, 
					       NOVCACHE_IGRP_WORSE_METRIC,
					       dstnet, NOV_NETMASK_ALL);
                    p->paths--;
                    if (debug || edebug)
                        buginf("\nIPXEIGRP: delete path to %x via %e, paths remaining %d",
                               dstnet, shost, p->paths);
                    if ((prev->next = p->nextpath = pp->next) == NULL)
                        p->nextpath = p->path_list;
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_DELETED);
                    free (pp);
		    return(NULL);
                }
            }
            prev = pp;
            pp = pp->next;
        }
    }
    return(NULL); /* .... */
}

/*
 * novigrp_rtupdate
 * Attempt to install a route specified by dual.
 */
static boolean novigrp_rtupdate (ddbtype *ddb, dual_ndbtype *dndb,
				 dual_rdbtype *drdb, boolean *promote)
{
    long flags;
    novpdbtype *pdb;
    novndbtype *ndb;
    novrdbtype *rdb;
    ulong metric;
    ulong tag;
    ulong addr, mask;
    int extm, extdelay;
    ulong delay;
    int count;

    *promote = FALSE;
    pdb = ddb->pdb;

    if (drdb->drdb_data) {
	flags = NET_DEXTERIOR;
	tag = ((novigrp_extdatatype *)drdb->drdb_data)->tag;
        extm = ((novigrp_extdatatype *)drdb->drdb_data)->ext_metric;
	extdelay = ((novigrp_extdatatype *)drdb->drdb_data)->ext_delay;
    } else {
	flags = 0;
	tag = 0;
	extm = 0;
	extdelay = 0;
    }

    addr = dndb->dndb_dest.nov_network;
    mask = NOV_NETMASK_ALL;

    ndb = novrip_find_rt(addr, mask, IPX_ROUTE_MATCH_EXACT);
    if (ndb == NULL) {
	metric = METRIC_INACCESSIBLE;
	delay = 0;
    } else { 
	metric = ndb->path_metric;
	delay = ndb->igrp_delay;
    }

    pdb->arg1 = drdb->drdb_successor_metric;
    pdb->route_type = (drdb->drdb_data) ? RTYPE_IGRP2_EXT : RTYPE_IGRP2_INT;

    rdb = novrt_update(pdb, addr,
		       drdb->drdb_metric ? drdb->drdb_nexthop.nov_address.net:
		       drdb->drdb_infosource.nov_address.net,
                       drdb->drdb_metric ?
		       drdb->drdb_nexthop.nov_address.host :
		       drdb->drdb_infosource.nov_address.host,
		       dual_idb(drdb->drdb_iidb), 0,
		       0, drdb->drdb_metric, drdb->drdb_vecmetric.delay,
		       extm, extdelay, flags, NULL);

    if (rdb) {
	novigrp_rdbupdate(drdb, rdb);
	/*
	 * Successful update to the routing table. Set the successors
	 * field in the ddb correctly.
	 */
	if (ndb == NULL || ndb->path_metric < metric)
	    dndb->dndb_successors = 1;
        else {
            /*
             * No easy way to glean it in this case.  Use the brute force
             * method looking for number of best cost successors learned
             * from this instance of igrp2.
             */
            count = 0;
	    rdb = ndb->path_list;
	    while (rdb) {
                if (rdb->procmask == pdb->mask)
                    count++;
		rdb = rdb->next;
            }
            dndb->dndb_successors = count;
        }
 	/*
 	 * We could have move paths around if maxpaths > 1. This is the
 	 * only way to get all the servers that we should have.
 	 */
#if 0
 	if (novell_maxpaths > 1)
 	    novell_posit_backup_servers_needed();
#endif
	*promote = TRUE;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * novigrp_netmatch
 * Perform novell network address match.
 */
static boolean novigrp_netmatch (
    igrp2nettype *net1,
    igrp2nettype *net2)
{
  return(net1->nov_network == net2->nov_network);
}

/*
 * novigrp_addrmatch
 * Perform IPX address match.
 */
static boolean novigrp_addrmatch (igrp2addrtype *addr1, igrp2addrtype *addr2)
{
    return((addr1->nov_address.net == GETLONG(&addr2->nov_address.net)) &&
	   ieee_equal(addr1->nov_address.host, addr2->nov_address.host));
}

char novigrp_netbuffer[20];
/*
 * novigrp_printnet
 *
 * Return an IPX net in ASCII.
 */
static char *novigrp_printnet (igrp2nettype *addr)
{
    sprintf(novigrp_netbuffer, "%x", addr->nov_network);
    return(novigrp_netbuffer);
}


/*
 * novigrp_localaddr
 *
 * Check to see if supplied address is one of the system's addresses.
 */

static boolean novigrp_localaddr (ddbtype *ddb, igrp2addrtype *address,
				  idbtype *idb)
{
    ulong novell_network;

    novell_network = GETLONG(&address->nov_address.net);
    FOR_ALL_SWIDBS(idb) {
	if (IPX_PRIMARY_NIDB(idb) &&
	    (IPX_PRIMARY_NIDB(idb)->novellnet == novell_network) &&
	    ieee_equal(idb->novell_node, address->nov_address.host))
	    return(TRUE);
    }
    return(FALSE);

}

/*
 * novigrp_addrcopy
 *
 * Copy an IPX address from source to dest. First byte specifies length
 * of address. Length returned is (length of address + 1).
 */
static ulong novigrp_addrcopy (igrp2addrtype *source, uchar *dest)
{
    int len;

    len = sizeof(ulong) + IEEEBYTES;
    *dest++ = len;
    bcopy(&source->nov_address.net, dest, len);
    return(len + 1);
}

/*
 * novigrp_rtdelete
 * Delete an rdb from the routing table, if it exists.
 */
static void novigrp_rtdelete (ddbtype *ddb, dual_ndbtype *dndb,
			      dual_rdbtype *drdb)
{
    novpdbtype *pdb;
    novndbtype *ndb;
    novndbtype *pndb;
    novrdbtype *rdb, *rdb_to_be_freed = NULL;
    novrdbtype *prdb;

    pdb = ddb->pdb;
    pndb = (novndbtype *)&(*novell_table)[NOV_NETHASH(dndb->dndb_dest.nov_network)];
    ndb = pndb->next;
    while (ndb) {
      if (ndb->dstnet == dndb->dndb_dest.nov_network)
	break;
      pndb = ndb;
      ndb = ndb->next;
    }

    if ((ndb != NULL) && (pdb->mask & ndb->procmask) && 
	!(ndb->flags & NET_CONNECTED)) {
        rdb = ndb->path_list;
	prdb = (novrdbtype *) (&ndb->path_list);
	while (rdb) {
	    /* Delete only if from same host, with same protocol */
	    if ((rdb->srcnet == drdb->drdb_nexthop.nov_address.net) &&
		(ieee_equal(rdb->srchost, drdb->drdb_nexthop.nov_address.host)) &&
		(rdb->procmask == pdb->mask) &&
		(rdb->nidb->idb == dual_idb(drdb->drdb_iidb))) { 
		novcache_increment_version(NULL, NOVCACHE_IGRP_RT_DEL,
					   rdb->nidb->novellnet,
					   NOV_NETMASK_ALL);
		if (ndb->paths == 1) { 
		    /* 
		     * Mark route as down. Put in 'IGRP holddown'.
		     */ 
		    if (!TIMER_RUNNING(ndb->holddownuntil)) { 
			ndb->delay = ndb->ext_delay = NOVELL_INFDELAY; 
			ndb->igrp_delay = NOT_IBM_TICKS(ndb->delay); 
			ndb->hopcount = ndb->ext_hop = XNS_INFINITY; 
			TIMER_START(ndb->holddownuntil, drdb->drdb_iidb ? 
				    drdb->drdb_iidb->holddown :
				    EIGRP_HOLDDOWN_DEFAULT);
			nov_network_redist(pdb, ndb, ROUTE_DOWN); 
		    } 
		} else { 
		    if ((prdb->next = ndb->nextpath = rdb->next) == NULL) 
		      ndb->nextpath = ndb->path_list; 
 		    rdb->next = rdb_to_be_freed; 
 		    rdb_to_be_freed = rdb;
		    rdb = prdb; 
		    ndb->paths--; 
		} 
	    }
	    prdb = rdb;
	    if (prdb)
	      rdb = rdb->next;
 	}
 	for (prdb = rdb_to_be_freed; prdb != NULL; prdb = rdb) {
 	    rdb = prdb->next;
 	    novell_mark_rdb_changed(ndb, prdb, ndb->flags,
 				    IPX_ROUTE_EVENT_DELETED);
 	    free(prdb);
	}
	if (ndb->path_list == NULL) {
	    pndb->next = ndb->next;
	    ndb->next = NULL;
	    nov_network_redist(pdb, ndb, ROUTE_DOWN);
	    nov_ndb_free(ndb, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
	}
    }
}

/*
 * novigrp_holddown
 * Let the caller know if this rdb should be deleted.
 */
static boolean novigrp_holddown (
    novpdbtype *pdb,
    novndbtype *ndb,
    novrdbtype *rdb,
    ulong newmetric)
{
    /*
     * If we have new information, or this information is about a route
     * that is not optimal, then perform a feasible successor check.
     * If it's a feasible successor, return FALSE.  It's feasible if:
     * - our neighbor's metric is better than our best metric, and
     * - our new metric is less than the variance times the best metric
     */ 
    if (!rdb || rdb->protocol_info.ripigrp_info.metric != ndb->path_metric) {
	if ((pdb->arg1 < ndb->path_metric)
	    && (newmetric < pdb->igrp_variance * ndb->path_metric))
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * novigrp_rtgetmetric
 * Get best metric from routing table for this destination.  If the
 * destination doesn't exist or the metrics aren't comparable then
 * return inaccessible.
 */
static ulong novigrp_rtgetmetric (igrp2nettype *dest, ddbtype *ddb)
{
    novndbtype *ndb;

    ndb = novrip_find_rt(dest->nov_network, NOV_NETMASK_ALL,
			 IPX_ROUTE_MATCH_EXACT);
    if (ndb && (NOVELL_ROUTE_SOURCE(ndb->flags) ==
 		NOVELL_ROUTE_SOURCE(((novpdbtype *)ddb->pdb)->proctype)))
	return(ndb->path_metric);
    return(METRIC_INACCESSIBLE);
}

/*
 * nov_redistribute
 *
 * A redistribute command was entered . Notify DUAL of routes that belong 
 * to the process being redistributed. DUAL tells IPX what to include in 
 * routing updates, so IPX must tell DUAL here which routes need to be 
 * redisributed.
 */
void nov_redistribute (novpdbtype *pdb, novpdbtype *dist, boolean sense)
{
    novigrp_worktype *work;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.rea.pdbindex = dist->index;
    work->c.rea.sense = sense;
    novigrp_qevent(pdb, work, IGRP2_WORK_REDISTALL);
}

/*
 * novigrp_get_prot_id
 *
 * Map an ndb->proctype into an IPX-IGRP2 external protocol id.
 */
static int novigrp_get_prot_id (ulong proctype)
{
    enum NOVIGRP_PROT_ID prot_id;

    switch (proctype) {
    case NET_STATIC:
      prot_id = STATIC_PROTID;
      break;
    case NET_CONNECTED:
      prot_id = CONNECTED_PROTID;
      break;
    case NET_RIP:
      prot_id = RIP_PROTID;
      break;
    case NET_IGRP:
      prot_id = IGRP_PROTID;
      break;
    case NET_ISIS:
      prot_id = ISIS_PROTID;
      break;
    case NET_NLSP:
      prot_id = NLSP_PROTID;
      break;
    case NET_INTERNAL:
      prot_id = INTERNAL_PROTID;
      break;
    default:
      prot_id = NULL_PROTID;
      break;
     }
     return(prot_id);
 }

/*
 * novigrp_build_external_info
 *
 * Build ddb->ddb_data for external destinations. Get information from either
 * the routing table entry or policy configuration.
 */
static void novigrp_build_external_info (novpdbtype *pdb, ddbtype *ddb,
					 novpdbtype *dist, novndbtype *ndb,
					 dual_newroute *route, int event)
{

    novigrp_extdatatype *ext_info;

    route->new_external = TRUE;
    route->new_origin = IGRP2_ORG_REDISTRIBUTED;
    ext_info = chunk_malloc(ddb->extdata_chunk);
    if (!ext_info) return;

    ieee_copy(ddb->routerid.nov_address.host, ext_info->routerid);
    ext_info->asystem = dist->asystem;
    ext_info->tag = ndb->protocol_info.ripigrp_info.tag;
    ext_info->ext_metric = min(ndb->hopcount + 1, novell_infinity);
    ext_info->ext_delay = min(ndb->delay, NOVELL_INFDELAY);
    ext_info->protocol = novigrp_get_prot_id(dist->proctype);
    ext_info->flag = 0;
    route->new_data = ext_info;
}

/*
 * novigrp_rtchange
 *
 * Inform DUAL about redistributed route. This inserts/deletes the
 * route from the topology table. Returns FALSE, if the IGRP metric
 * has not been set in the rdb. Otherwise, return TRUE.
 */
static boolean novigrp_rtchange (novpdbtype *pdb, novndbtype *ndb, int event)
{
    ddbtype *ddb;
    novpdbtype *pdb2, *src_pdb; 
    novrdbtype *rdb;
    idbtype *interface;
    boolean reachable;
    int list;
    dual_newroute route;
    dual_rtev_type event_type;

    bzero(&route, sizeof(route));

    /* If no path list, the route is DOWN... Avoid crashes! */
    if (ndb->path_list == NULL) {
      event = ROUTE_DOWN;
    }

    reachable = (event == ROUTE_UP || event == ROUTE_MODIF);
    src_pdb = NULL;
    if (reachable) {
        src_pdb = NOV_INDEX2PDB(ndb->novpdbindex);
    }
    if (src_pdb == NULL) {
        /*
         * The ndb's owning pdb may no longer exist, in which case src_pdb
         * is NULL.  Make sure we don't dereference src_pdb.
         */
        reachable = FALSE;
    }
    if (reachable) {
	list = pdb->ofltr_proto[src_pdb->index];
	if (list && !nov_filtercheck(ndb->dstnet, list))
            reachable = FALSE;
    }

    pdb2 = nov_proctable[ndb->novpdbindex];
    if (pdb2->proctype & NET_IGRP)
	rdb = ndb->path_list;
    else
	rdb = NULL;
    
    /*
     * Inform DUAL that route has changed.
     */
    ddb = pdb->ddb;
    route.new_dest.nov_network = ndb->dstnet;
    route.new_nexthop.nov_address.net = 0;
    ieee_zero(route.new_nexthop.nov_address.host);
    route.new_infosource.nov_address.net = 0;
    ieee_zero(route.new_infosource.nov_address.host);
    route.new_iidb = NULL;
    route.new_ignore = FALSE;

    if (pdb == pdb2)
	dual_lostroute(ddb, &route.new_dest);

    /*
     * Set up metric for DUAL call.
     */
    if (reachable) {
	route.new_successor_metric = 0;
    	if (rdb) {
            route.new_vecmetric.delay = rdb->protocol_info.ripigrp_info.delay;
            route.new_vecmetric.bandwidth =
		rdb->protocol_info.ripigrp_info.bandwidth;
            route.new_vecmetric.reliability =
		rdb->protocol_info.ripigrp_info.reliability;
            route.new_vecmetric.load = rdb->protocol_info.ripigrp_info.load;
            route.new_vecmetric.mtu = rdb->protocol_info.ripigrp_info.mtu;
            route.new_vecmetric.hopcount =
		rdb->protocol_info.ripigrp_info.hopcount;
	} else {
            /*
             * Vector values for redistributed routes use the external delay
             * (ticks) as computed in RIP and then translated to an EIGRP
             * style delay. This allows a modicum of granularity when
             * choosing between two redistributed EIGRP paths. For
	     * redistribution to work correctly, bandwidth must be set to the
             * same value for all instances of EIGRP on a network. Do not
             * be tempted to set this value to the interface bandwidth!
             */
	    interface = ndb->path_list->nidb->idb;
	    if (ndb->flags & NET_CONNECTED) {
		route.new_vecmetric.bandwidth = 
		    SCALED_BANDWIDTH(interface->visible_bandwidth) << 8;
		route.new_vecmetric.delay = interface->delay << 8;
	    } else {
		route.new_vecmetric.delay = ndb->igrp_delay;
		route.new_vecmetric.bandwidth = BPS_TO_IGRP_BW(9600) << 8;
	    }
	    route.new_vecmetric.reliability = interface->hwptr->reliability;
	    route.new_vecmetric.load = interface->hwptr->load;
	    route.new_vecmetric.mtu = interface->hwptr->maxmtu;
	    route.new_vecmetric.hopcount = 0;
	}
	route.new_successor_metric = 0;
	route.new_metric = igrp2_computemetric(route.new_vecmetric.bandwidth,
					       route.new_vecmetric.load,
					       route.new_vecmetric.delay,
					       route.new_vecmetric.reliability,
					       ddb->k1, ddb->k2,
					       ddb->k3, ddb->k4, ddb->k5);
        event_type = (event == ROUTE_UP) ? DUAL_RTUP : DUAL_RTCHANGE;
	/* Don't try to 'bring up' inaccesible paths to DUAL. Confuses it. */
	if (route.new_metric == METRIC_INACCESSIBLE)
	    return(TRUE);
    } else {
        route.new_metric = METRIC_INACCESSIBLE;
	route.new_successor_metric = METRIC_INACCESSIBLE;
	route.new_vecmetric.delay = METRIC_INACCESSIBLE;
	event_type = DUAL_RTDOWN;
    }

    /*
     * Build the external information for a specific redistributed
     * route. 
     */
    novigrp_build_external_info(pdb, ddb, pdb2, ndb, &route, event);

    /*
     * Inform DUAL.
     */
    route.new_opaque_flags = 0;
    dual_rtchange(ddb, &route, event_type);
    return(TRUE);
}

/*
 * novigrp_process_redistribute
 * Process a redistribute everyting queue entry.
 */
static void novigrp_process_redistribute (novpdbtype *pdb,
					  novigrp_worktype *work)
{
    int i;
    novndbtype *ndb;
    novpdbtype *dist;
    boolean sense;
    ulong mask;
    ddbtype *ddb = pdb->ddb;

    dist = NOV_INDEX2PDB(work->c.rea.pdbindex);
    if (dist == NULL)
	return;
    sense = work->c.rea.sense;

    if (pdb->running) {
	/*
	 * For each network created by routing process "dist", see if we
	 * should redistribute into "pdb".
	 */
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    for (ndb = (*novell_table)[i]; ndb; ndb = ndb->next) {

		mask = INDEX2MASK(ndb->novpdbindex);
		/*
		 * Is this destination owned by dist?
		 * Is the protocol the destination was learned via being 
		 *  redist'd to this pdb? 
		 * make sure not same protocol.
		 */
		if ((dist->mask & mask) &&
		    (pdb->distflag & mask) &&
		    !(pdb->mask & mask)) {
		    /*
		     * If this is a connected route, don't redistribute into
		     * EIGRP if it one of EIGRP's routes... which means it
		     * is a primary NIDB on an interface which EIGRP knows
		     * about.
		     */
		    if ((ddb == NULL) ||
			(!(ndb->flags & NET_CONNECTED)) ||
			(ndb->path_list == NULL) ||
			(ndb->path_list->nidb == NULL) ||
			IPX_NIDB_IS_SECONDARY(ndb->path_list->nidb) ||
			(igrp2_find_iidb(ddb, ndb->path_list->nidb->idb) ==
			 NULL)) {
			novigrp_rtchange(pdb, ndb, sense ? ROUTE_UP : ROUTE_DOWN);
		    }
		}
	    }
	}
    }
    /*
     * Finally, set the distflag for this pdb. This flag must be
     * set after the call to redist_callback for "no redist" case.
     * It was already set in novell_redist_command for "redist" case.
     * Distflag needs to get set even if the process isn't running (yet).
     */
    if (!sense)
	pdb->distflag &= ~dist->mask;
}

/*
 * novigrp_redist_enq
 * Enqueue a redist work event
 */
static void novigrp_redist_enq (novpdbtype *pdb, ulong net, int event)
{
    novigrp_worktype *work;
    
    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    /*
     * Queue redistribution request and process later.
     */
    if (!(work = chunk_malloc(pdb->ddb->workq_chunk))) {
	return;
    }
    work->c.red.net = net;
    work->c.red.event = event;
    novigrp_qevent(pdb, work, IGRP2_WORK_REDIST);
}

/*
 * novigrp_redist_callback
 * Routine used to inform dual when a route we are concerned with 
 * has changed state.
 */
static void novigrp_redist_callback (novpdbtype *pdb, novndbtype *ndb,
				     int event)
{
    if (!pdb->running)
	return;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    /* Ignore prefix routes. */

    if (ndb->rt_mask != NOV_NETMASK_ALL)
	return;

    /* Don't advertise connected nets that IGRP cares about */
    if (event != ROUTE_DOWN) {
	if (ndb->flags & NET_CONNECTED) {
	    if (novigrp_net_in_pdb(pdb, ndb->dstnet))
		return;
	}
    }

    /* if (!pdb_outfilter(pdb, NULL, ndb->pdbindex, ndb->netnumber))
       return;
       */

    novigrp_redist_enq(pdb, ndb->dstnet, event);
}

/*
 * novigrp_process_redist
 *
 * Process each element on the redistribution queue. As each element
 * is inserted in the topology table, it is then unqueued. 
 */
static void novigrp_process_redist (novpdbtype *pdb, novigrp_worktype *work)
{

    novndbtype          *ndb, temp_ndb;

    ndb = novrip_find_rt(work->c.red.net, NOV_NETMASK_ALL,
			 IPX_ROUTE_MATCH_EXACT);

    /*
     * Route has been deleted, process ROUTE_DOWN event with use of
     * temporary db on stack. temp_ndb->pdbindex = 0 down below does
     * not get used in novigrp_rtchange().
     */
    if ((!ndb) || (work->c.red.event == ROUTE_DOWN)) {
	temp_ndb.dstnet = work->c.red.net;
	temp_ndb.path_list = NULL;
	temp_ndb.novpdbindex = 0;
	temp_ndb.procmask = pdb->distflag;
	ndb = &temp_ndb;
	work->c.red.event = ROUTE_DOWN;
    }

    (void)novigrp_rtchange(pdb, ndb, work->c.red.event);
}

/*
 * novigrp_process_lost
 * Process each element on the lost route queue.
 */
static void novigrp_process_lost (novpdbtype *pdb, novigrp_worktype *work)
{
    ddbtype *ddb;
    igrp2nettype dest;

    /*
     * Inform DUAL that route has changed.
     */
    ddb = pdb->ddb;

    dest.nov_network = work->c.d.net;
    dual_lostroute(ddb, &dest);
}

/*
 * novigrp_show_traffic
 *
 * show ipx eigrp traffic
 */
void novigrp_show_traffic (parseinfo *csb)
{

    novpdbtype *pdb;
    ddbtype *ddb;
    ulong   asystem;

    asystem = GETOBJ(int,1);

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	ddb = pdb->ddb;
        if (!ddb)
            continue;
        if (!pdb->running)
            continue;
        if (asystem && pdb->asystem != asystem)
            continue;
	printf("\nIP-EIGRP Traffic Statistics for process %d", pdb->asystem);
        igrp2_show_traffic(ddb);
    }
}

/*
 * novigrp_show_event
 *
 * show ipx eigrp event
 */
void novigrp_show_event (parseinfo *csb)
{

    novpdbtype *pdb;
    ddbtype *ddb;
    ulong   asystem;

    asystem = GETOBJ(int,1);

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	ddb = pdb->ddb;
        if (!ddb)
            continue;
        if (!pdb->running)
            continue;
        if (asystem && pdb->asystem != asystem)
            continue;
	printf("\nEvent information for AS %d:", pdb->asystem);
        dual_show_events(ddb, csb->which, 0, 0);
    }
}

/*
 * novigrp_protid_string
 *
 * Look at IPX-IGRP2 external protocol id and return string for
 * printing.
 */
static char *novigrp_protid_string (int prot_id)
{

    char *str;

    switch (prot_id) {
    case IGRP_PROTID:
      str = "EIGRP";
      break;
    case STATIC_PROTID:
      str = "Static";
      break;
    case CONNECTED_PROTID:
      str = "Connected";
      break;
    case RIP_PROTID:
       str = "RIP";
      break;
    case ISIS_PROTID:
      str = "ISIS";
      break;
    case NLSP_PROTID:
      str = "NLSP";
      break;
    case INTERNAL_PROTID:
      str = "INTERNAL";
      break;
    default:
    case NULL_PROTID:
      str = "Unknown protocol";
      break;
    }
    return(str);
}

/*
 * novigrp_show_topology_route
 */
static void novigrp_show_topology_route (ddbtype *ddb, int address)
{
    dual_ndbtype        *dndb = NULL;
    dual_rdbtype        *drdb;
    dual_rdbtype	*drdb_next;
    novigrp_extdatatype *ext_info;
    boolean		connected;
    boolean		redistributed;
    igrp2nettype	dest;
    igrp2metric		*metric;

    if (ddb == NULL)
	return;
    dest.nov_network = address;

    dndb = dual_ndblookup(ddb, &dest);
    if (dndb == NULL) {
      printf("\n%% Route not in topology table");
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
    automore_enable(NULL);

    printf("\nIPX-EIGRP topology entry for %s",
	   (*ddb->printnet)(&dndb->dndb_dest));
    printf("\n  State is %s, Query origin flag is %d, %d Successor(s)",
         dual_dndb_active(dndb) ? "Active" : "Passive",
         dndb->query_origin, dndb->dndb_successors);
    printf("\n  Routing Descriptor Blocks:");

    drdb = dndb->dndb_rdb;
    while (drdb) {
      mem_lock(drdb);
      connected = (drdb->drdb_origin == IGRP2_ORG_CONNECTED);
      redistributed = (drdb->drdb_origin == IGRP2_ORG_REDISTRIBUTED);
      printf("\n  Next hop is %s", connected ? "Connected" :
                redistributed ? "Redistributed" :
		(*ddb->printaddr)(&drdb->drdb_nexthop));
      if (drdb->drdb_iidb) {
          printf(" (%s)", dual_idb(drdb->drdb_iidb)->namestring);
      }
      printf(", from %x.%e", drdb->drdb_infosource.nov_address.net, 
		drdb->drdb_infosource.nov_address.host);
      printf("\n      Composite metric is ");
      if (drdb->drdb_successor_metric == METRIC_INACCESSIBLE) {
	  printf("(Infinity/Infinity)");
      } else if (drdb->drdb_metric == METRIC_INACCESSIBLE) {
	  printf("(Infinity/%u)", drdb->drdb_successor_metric);
      } else {
	  printf("(%u/%u)", drdb->drdb_metric, drdb->drdb_successor_metric);
      }
      printf(", Send flag is 0x%x, Route is %s", 
	     drdb->drdb_send_flag, (drdb->drdb_external) ?
	     "External" : "Internal");
      if (drdb->drdb_ignore) {
	  printf("\n      This is an ignored route");
      }
      metric = &drdb->drdb_vecmetric;
      printf("\n      Vector metric:");
      printf("\n        Minimum bandwidth is %d Kbit", (metric->bandwidth) ?
            IGRP2_BANDWIDTH_SCALE/metric->bandwidth : 0);
      printf("\n        Total delay is %d microseconds",
	     igrp2_cdelay(metric->delay));
      printf("\n        Reliability is %d/255", metric->reliability);
      printf("\n        Load is %d/255", metric->load);
      printf("\n        Minimum MTU is %d", metric->mtu);
      printf("\n        Hop count is %d", metric->hopcount);
      if (drdb->drdb_data) {
	  ext_info = chunk_malloc(ddb->extdata_chunk);
	  if (ext_info) {
	      bcopy(drdb->drdb_data, ext_info, sizeof(*ext_info));
	      printf("\n      External data:");
	      printf("\n        Originating router is %e %s", 
		     ext_info->routerid,
		     (!ieee_equal(ext_info->routerid, 
				  ddb->routerid.nov_address.host))
		     ? " " : "(this system)");
	      if (ext_info->protocol == IGRP_PROTID) 
		  printf("\n        AS number of route is %d",
			 ext_info->asystem);
	      printf("\n        External protocol is %s, metric is %u, "
		     "delay %u", novigrp_protid_string(ext_info->protocol),
		     ext_info->ext_metric, ext_info->ext_delay);
	      printf("\n        Administrator tag is %d (0x%08x)",
		     ext_info->tag, ext_info->tag);
	      printf("\n        Flag is 0x%08x", ext_info->flag);
	      chunk_free(ddb->extdata_chunk, ext_info);
	  }
      }
      drdb_next = drdb->drdb_next;
      free(drdb);
      drdb = drdb_next;
    }
    free(dndb);
    free(ddb);
    automore_disable();
}

/*
 * novigrp_posit_sap_update_peer
 *
 * Posit an update for the peer specified. Called by igrp2_for_each_peer.
 */
static void novigrp_posit_sap_update_peer (ddbtype *ddb, dual_peertype *peer,
				    void *data)
{
    idbtype *idb;

    idb = data;
    if (peer->iidb->idb == idb) {
	novell_posit_sap_update(IPX_PRIMARY_NIDB(idb),
				peer->source.nov_address.net,
				peer->source.nov_address.host, 0,
				IPX_FULL_NEEDED,
				IPX_DESTINATION_UNICAST,
				NOVELL_EVERYONE,
				FALSE, FALSE);
    }
}

/*
 * novigrp_if_commands
 *
 * Process IPX-IGRP2 interface subcommands. 
 *
 * [no] ipx hello-interval eigrp <as-number> <value> 
 * [no] ipx hold-time eigrp <as-number> <value>
 * [no] ipx sap-incremental eigrp <as-number>
 * [no] ipx split-horizon eigrp <as-number>
 *
 */
void novigrp_if_commands (parseinfo *csb)
{

    ddbtype       *ddb;
    novpdbtype    *pdb;
    idbtype       *idb;
    igrp2_idbtype *iidb;
    boolean take_neighbors_down;
    ulong interval;
    boolean all_nets_deleted;
    nidbtype *nidb;

    if (!(idb = csb->interface) || !IPX_PRIMARY_NIDB(idb)) 
	return;

    /*
     * NV write all interface subcommands for IGRP2 related pdbs.
     */
    if (csb->nvgen) {
 	/* Always bail if IPX not enabled */
 	if (!novell_on_idb(idb)) {
 	    return;
 	}
 	/*
 	 * Also bail if the network(s) associated with this interface have
 	 * been deleted. Note that networks must hang around until all
 	 * poisons/reverse poisons have been sent. This could be a very
 	 * long time if the internet is large and/or significant output
 	 * rip/sap delays have been configured. We do not want to write
 	 * deleted network state to nvram while this is happening. On the
 	 * other hand, only a subset of networks on an interface may be
 	 * deleted. In this case we want to update nvram with everything
 	 * except the deleted network(s).
 	 */
 	all_nets_deleted = TRUE;
 	FOR_ALL_NIDBS(nidb) {
 	    if ((nidb->idb == idb) && (!NOVELL_NIDB_DELETED_MODE(nidb))) {
 		all_nets_deleted = FALSE;
 		break;
 	    }
 	}
 	if (all_nets_deleted)
 	    return;
 
	for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
            if (pdb->proctype != NET_IGRP || (pdb->proctype == NET_IGRP &&
		!pdb->entered)) 
		continue;
          ddb = pdb->ddb;
          if (!(iidb = igrp2_find_iidb(ddb, idb))) {
              if (!(iidb = igrp2_find_temp_idb(ddb, idb))) continue;
          }

	  switch (csb->which) {
	    case IPXIGRP_HELLO:
	      nv_write((iidb->hello_interval !=
			igrp2_def_helloint(idb, ddb->linktype)), "%s %s %d %d",
                       csb->nv_command, pdb->name, pdb->asystem,
                       iidb->hello_interval/ONESEC);
	      break;
	    case IPXIGRP_HOLDTIME:
	      nv_write((iidb->holdtime !=
			igrp2_def_holdint(idb, ddb->linktype)),
		       "%s %s %d %d", csb->nv_command, pdb->name,
		       pdb->asystem, iidb->holdtime/ONESEC);
	      break;
	    case IPXIGRP_HOLDDOWN:
	      nv_write(iidb->holddown != 
		       EIGRP_HOLDDOWN_DEFAULT,
		       "%s %s %d %d", csb->nv_command, pdb->name,
		       pdb->asystem, iidb->holddown/ONESEC);
	      break;
	    case IPXIGRP_BW_PCT:
	      nv_write((iidb->igrp2_bandwidth_percentage !=
			IGRP2_DEFAULT_BANDWIDTH_PERCENT),
		       "%s %s %d %d", csb->nv_command, pdb->name,
		       pdb->asystem, iidb->igrp2_bandwidth_percentage);
	      break;
	    case IPXIGRP_SAPUPDATE:
	      if (IPX_PRIMARY_NIDB(idb)->igrp_sap_updates) {
		  if (idb_is_lan(idb) ||
		      IPX_PRIMARY_NIDB(idb)->igrp_sap_only) {
		      nv_write(TRUE, "%s %s %d", 
			       csb->nv_command, pdb->name, pdb->asystem);
		      nv_add((IPX_PRIMARY_NIDB(idb)->igrp_sap_only),
			     " rsup-only");
		  }
	      } else {
		  if (!idb_is_lan(idb) ||
		      IPX_PRIMARY_NIDB(idb)->igrp_sap_only) {
		      nv_write(TRUE, "no %s %s %d", 
			       csb->nv_command, pdb->name, pdb->asystem);
		      nv_add((IPX_PRIMARY_NIDB(idb)->igrp_sap_only),
			     " rsup-only");
		  }
	      }
	      break;
	    case IPXIGRP_SPLIT:
	      if (ddb->def_split_horizon)
		  nv_write(!iidb->split_horizon, "no %s %s %d",
			   csb->nv_command, pdb->name, pdb->asystem);
	      else
		  nv_write(iidb->split_horizon, "%s %s %d", csb->nv_command,
			   pdb->name, pdb->asystem);
	      break;
	  }
      }
      return;
    }

    if (!partialmatch("eigrp", (char *) csb->router_name.myname)) {
      printf("\nInvalid routing process");
      return;
    }

    /*
     * Get pdb from parse block.
     */
    pdb = find_novigrp_pdb(csb->router_name.as);

    /*
     * If no pdb is found, create one. 
     */
    if (!pdb) {
	pdb = novigrp_start(csb->router_name.as);
	if (!pdb)
	    return;
    }

    /*
     * Get DUAL descriptor block.
     */
    ddb = pdb->ddb;
    if (!ddb)
	return;

    switch (csb->which) {
      case IPXIGRP_HELLO:
	if (csb->sense)
	    interval = GETOBJ(int,1)*ONESEC;
	else
	    interval = igrp2_def_helloint(idb, ddb->linktype);
        igrp2_set_hello_interval(ddb, idb, interval);
	break;
      case IPXIGRP_HOLDDOWN:
	if (csb->sense)
	    interval = GETOBJ(int,1)*ONESEC;
	else
	    interval = EIGRP_HOLDDOWN_DEFAULT;
	igrp2_set_holddown(ddb, idb, interval);
	break;
      case IPXIGRP_HOLDTIME:
	if (csb->sense)
	    interval = GETOBJ(int,1)*ONESEC;
	else
	    interval = igrp2_def_holdint(idb, ddb->linktype);
	igrp2_set_holdtime(ddb, idb, interval);
	break;
      case IPXIGRP_BW_PCT:
	igrp2_set_bandwidth_percent(ddb, idb, GETOBJ(int,1));
	break;
      case IPXIGRP_SAPUPDATE:
	take_neighbors_down = FALSE;
	if (csb->set_to_default)
	    csb->sense = !idb_is_lan(idb);
	/* If command has no effect, do nothing. */
	if (IPX_PRIMARY_NIDB(idb)->igrp_sap_updates != csb->sense) {
	    /*
	     * If command disables the SAP incremental packets, well, each
	     * peer has to die to reset the remote server table. Sorry, but
	     * this is the only method that is absolutely guaranteed to work,
	     * and people won't change the value of this varioble too often
	     * in the NEGATIVE direction (enabled->disabled) to make anything
	     * more complex, but as effective, worthwhile.
	     */
	    if (csb->sense == FALSE) {
		if (novellsap_debug || novellsap_event_debug)
		  buginf("\nIPXIGRP: Disabling incremental SAPs. "
			 "Resetting IPX neighbors on interface");
		
		take_neighbors_down = TRUE;
		IPX_PRIMARY_NIDB(idb)->igrp_sap_updates = csb->sense;
		IPX_PRIMARY_NIDB(idb)->igrp_sap_only = csb->sense;
	    } else {
		IPX_PRIMARY_NIDB(idb)->igrp_sap_updates = csb->sense;
		/* Force some IGRP updates out if new... or delete if old */
		igrp2_for_each_peer(ddb, novigrp_posit_sap_update_peer, idb);
	    }
	}
	/* If changing sap-only status, disable peers */
	if (csb->sense &&
	    (IPX_PRIMARY_NIDB(idb)->igrp_sap_only != (GETOBJ(int,3)))) {
	    if (novellsap_debug || novellsap_event_debug)
	      buginf("\nIPXIGRP: Changing SAP only status. "
		     "Resetting IPX neighbors on interface");
	    IPX_PRIMARY_NIDB(idb)->igrp_sap_only = GETOBJ(int,3);
	    take_neighbors_down = TRUE;
	}
	if (take_neighbors_down) {
	    igrp2_take_nbrs_down(ddb, idb, FALSE, "Reliable SAP configured");
	}
	break;
      case IPXIGRP_SPLIT:
	if (csb->set_to_default)
	    csb->sense = ddb->def_split_horizon;
        igrp2_set_split_horizon(ddb, idb, csb->sense);
        break;

    }
}

/*
 * novell_eigrp_sap_split_horizon_command
 */
static void novell_eigrp_sap_split_horizon_command (boolean enabling) 
{ 
    if (!novell_running || novell_shutting_down) 
      return; 
    if (enabling != ipx_eigrp_sap_split_horizon) {
	ipx_eigrp_sap_split_horizon = enabling;
	novigrp_take_neighbors_down(NULL, "SAP split horizon changed");
    }
} 

void novigrp_command (parseinfo *csb)
{
    if (!novell_running || novell_shutting_down) {
	if (csb->nvgen || (csb->which != NOVELL_ADDRESS)) {
	    if (!csb->nvgen) {
		printf("\n%%IPX not running");
	    }
	    return;
	}
    }

    if (csb->nvgen) {
	switch (csb->which) {
	  case NOVELL_EIGRP_SAP_SPLIT_HORIZON: 
	    nv_write(ipx_eigrp_sap_split_horizon !=
		     IPX_EIGRP_SAP_SPLIT_HORIZON_DEFAULT,
		     "%s%s", ipx_eigrp_sap_split_horizon ? "" : "no ",
		     csb->nv_command); 
	    break; 
	    
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NOVELL_EIGRP_SAP_SPLIT_HORIZON:
	novell_eigrp_sap_split_horizon_command(csb->sense);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/* 
 * novigrp_lost_route
 *
 * Called when an EIGRP route is overwritten in the routing table
 * by a Connected, Static or RIP route. 
 */
static void novigrp_lost_route (
    novpdbtype *pdb,
    novndbtype *ndb)
{
    novigrp_worktype *work;

    if (pdb->running == FALSE)
	return;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    /* Ignore prefix routes for now. */

    if (ndb->rt_mask != NOV_NETMASK_ALL)
	return;

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.d.net = ndb->dstnet;
    novigrp_qevent(pdb, work, IGRP2_WORK_LOST);
}

/*
 * novigrp_process_backup
 * Process each element on the backup queue.
 */
static void novigrp_process_backup (novpdbtype *pdb, novigrp_worktype *work)
{
    ddbtype *ddb;
    igrp2nettype dest;

    ddb = pdb->ddb;

    dest.nov_network = work->c.d.net;
    dual_validate_route(ddb, &dest);
}

/*
 * novigrp_callbackup_routes
 *
 * Called when a backup route is candidate for promotion to the primary
 * route. This occurs when better administratively distance route has
 * gone unreachable.
 */
static void novigrp_callbackup_routes (novpdbtype    *pdb,
				ulong      address,
				ulong	   netmask,
				ulong      context,
				ulong      route_type)
{
    novigrp_worktype *work;

    if (pdb->running == FALSE)
	return;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    /* Ignore masked routes for now. */

    if (netmask != NOV_NETMASK_ALL)
	return;

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.d.net = address;
    novigrp_qevent(pdb, work, IGRP2_WORK_BACKUP);
}

/*
 * novigrp_peerbucket
 * Return slot based on IPX address.
 */
static int novigrp_peerbucket (igrp2addrtype *address)
{
    return(nethash(GETLONG(&address->nov_address.net)));
}

/*
 * novigrp_ndbbucket
 * Return slot based on IPX address.
 */
static int novigrp_ndbbucket (igrp2nettype *address)
{
    return(nethash(address->nov_network));
}

/*
 * novigrp_headerptr
 *
 * Return pointer to beginning of IGRP2 fixed length header.
 */
static igrp2type *novigrp_headerptr (paktype *pak)
{
    /*
     * Assumes a fixed length IPX header.
     */
    return((igrp2type *)(XNSDATASTART(pak)));
}

/*
 * novigrp_paklen
 *
 * Return length of IGRP2 packet minus the IPX header length.
 */
static ulong novigrp_paklen (paktype *pak)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    return(ipx->len - XNSHEADERBYTES);
}    

/*
 * IGRP2/server Support.
 * 
 * 	Theory of operation:
 * 
 * 	1) No 'mainline' Novell code is changed to support IGRP2/SAP.
 * 	   Therefore, regression is not an issue -- the SAP code stays
 * 	   the same.
 * 
 * 	2) All that is attempted is to provide a mechanism to allow
 * 	   update-only SAP packets to be maintained between IGRP2 peers.
 * 
 * 	3) To do this:
 * 		a) SAP entries sent between IGRP2 peers are encapsulated in
 * 		   IGRP2, providing for the 'reliable transport' between peers.
 * 		b) server entries in the main table are marked as from 'IGRP2'
 * 		   and these entries do not age.
 * 		c) Interfaces enabled for IGRP2/SAP support do NOT send
 * 		   out periodic updates.
 * 		d) The SAP information received from an IGRP2 peer is stored
 * 		   (hopefully, efficiently) in a 'backup server table' maintained
 * 		   for each peer. The names/types of each server are common
 * 		   to all peers advertising that server, so these are kept in
 * 		   a common single 'server name/type table' that is associated
 * 	           with each IGRP process. Per-peer advertisements point into
 * 		   that common table to reference the name/type. This saves
 * 		   48 bytes per peer server entry.
 * 		e) When a server is deleted or made worse cost from the
 *		   main table, or the routing table has a route changed, this
 *		   means previously ignored servers from a peer now might be
 *		   wanted in the main table. To easily do this, the
 * 		   'backup server table' for all IGRP2 peers is processed as
 * 		   if it were a set of normal SAP updates coming into the
 * 		   main table. This processing is throttled to happen at most
 * 		   one every 'novell_backup_servers_timer' seconds, to avoid
 * 		   too much CPU overhead. This value is configurable by the
 * 		   user. By sending these 'backup server tables' to the main
 *		   server table, if the IGRP information is useful, it will
 *		   be entered. This serves as a simple 'redistribution'
 *		   mechanism.
 * 		f) When an IGRP2 encapsulated SAP packet arrives, it is passed
 * 		   to the main Novell code for 'mainline table' processing,
 * 		   as well as to a function that will update the information
 * 		   in the backup table.
 * 		g) When a peer goes down, or comes up, (as told to us by DUAL)
 * 		   the server backup table associated with that peer is deleted
 * 		   (as are any of its entries in the mainline table), and then
 * 		   (if coming up) a new server backup table is created.
 *		   Finally, the main Novell code is told to pass a FULL list of
 * 		   servers to the newly-arriving peer. This guarantees peer
 * 		   server synchronization.
 * 		h) Poisons are sent out links that have IGRP2/SAP enabled,
 * 		   even when split horizon is violated. This guarantees all
 * 		   IGRP/SAP peers know about a server that the local router
 * 		   is poisoning.
 *		i) The user can configure the use of the IGRP2/SAP feature
 *		   through configuration commands.
 * 
 * The data structures
 * 
 * 	The backup server data structures are maintained as follows:
 * 
 * 	per peer... peer->protocol_data casts into a
 * 			(nov_server_per_source_bynametable *)
 * 			A hashtable, for this peer, of all servers known via
 * 			this peer. It contains the 'source specific server
 * 			data' of server name, server type, dest net, dest host,
 *                      dest sock, and nethops. THE NOVELL SPEC 
 * 			ALLOWS YOU TO ONLY CONSIDER NAME AND TYPE AS UNIQUELY
 * 			IDENTIFYING A SERVER. THEREFORE, MULTIPLE ROUTERS COULD
 * 			ADVERTISE MULTIPLE 'SERVER LOCATIONS' AND GIVE
 * 			DIFFERENT INFORMATION FOR THESE THINGS.
 * 
 *   The functions:
 * 
 * 	novigrp_free_sap_backup_block -- Free's a peer's specific server
 * 					 entry.
 * 
 * 	novigrp_advertise_peer_saps -- 'Advertise' all of the servers known
 * 				       by this peer to the 'mainline' server
 * 				       table. If told to poison these, then
 * 				       send poisons to the mainline table,
 * 				       and delete all the servers associated
 * 				       with this peer.
 * 
 * 	nov_get_backup_servers -- Get any backup servers that are known
 * 				  with any routing protocol (currently,
 * 				  only IGRP2) Written with a 'nov' prefix
 * 				  as this could support other routing
 * 				  protocols.
 * 
 * 	novigrp_peershowhook -- A hook used by IGRP2 when displaying the
 * 				list of peers. It prints all the servers
 * 				associated with this peer.
 * 
 * 	novigrp_peerstatehook -- On peer creation or deletion, deletes and
 * 				 poisons existing server entries, then if
 * 				 peer is starting up, creates a clean new
 * 				 table. (Even when 'restarting' the old must
 * 				 be deleted, and poisoned, as we are assuming
 * 			         NO KNOWLEDGE about the new peer). This also
 * 				 will cause this router to send a full list
 * 				 of servers to this new peer.
 * 
 * 	novigrp_allocate_sap_backup_block -- Create a new peer-specific
 * 				 backup server entry.
 * 
 * 	novigrp_accept_server_change -- Called as a 'callback' by 
 * 				novell_addserver -- this will cause a SAP
 * 				advertisement from a remote peer to be
 * 				reflected in the peer's backup server table.
 * 
 * 	novigrp_send_sap -- Send a SAP packet in IGRP2's reliable
 * 			    transport protocol.
 * 
 * 	novigrp_rcv_sap	 -- Receive a SAP packet sent in IGRP2's reliable
 * 			    transport protocol.
 */
/*
 * novigrp_free_sap_backup_block
 * Free a server backup block.
 */
static void novigrp_free_sap_backup_block (novell_server_per_source_list *sl,
				    novell_server_per_source_list *prev_sl,
				    nov_server_per_source_bynametable
				    *peer_sap_table, int hash_entry)
{
    /*
     * If entry exists, delete it when poisoned
     */
    if (prev_sl == NULL) {
	(*peer_sap_table)[hash_entry] = sl->next;
    } else {
	prev_sl->next = sl->next;
    }
    free(sl);
}

/*
 * novigrp_advertise_peer_saps
 *
 * Advertise the servers associated with this peer. Called by
 * igrp2_for_each_peer.
 */
static void novigrp_advertise_peer_saps (ddbtype *ddb, dual_peertype *peer,
				  void *data)
{
    int i;
    boolean *do_poison;
    boolean poison;
    novell_serveridtype server;
    nov_server_per_source_bynametable *peer_sap_table;
    novell_server_per_source_list *sl, *next_sl, *prev_sl;

    do_poison = data;

    poison = *do_poison;

    peer_sap_table = peer->protocol_data;
    if (peer_sap_table == NULL) {
	return;
    }
    for (i = 0; i < NOVELL_SAP_HASHLEN; i++) {
	prev_sl = NULL;

	/* Sleep for a bit.  Bail if our peer went away. */

	process_may_suspend();
	if (peer->peer_deleted)		/* Overtaken by events. */
	    return;

	for (sl = (*peer_sap_table)[i]; sl != NULL; sl = next_sl) {
	    /* next_sl must ALWAYS move forward */
	    next_sl = sl->next;
	    if (poison) {
		sl->nethops = NOVELL_MAX_HOPS_ALLOWED;
	    }

	    /* Copy the server info together in a single block */
 	    bcopy((uchar *) &(sl->server_type), 
		  (uchar *) &(server.server_type),
		  sizeof(novell_serveridtype));

	    /* Let the system know about it... */
	    novell_addserver(ddb->pdb, &server,
			     IPX_PRIMARY_NIDB(peer->iidb->idb),
			     peer->source.nov_address.host,
			     peer->source.nov_address.net,
 			     NET_IGRP, server.nethops, NULL, NULL, 0, 0, TRUE);
	    if (poison) {
		novigrp_free_sap_backup_block(sl, prev_sl, peer_sap_table, i);
		/* prev_sl stays the same */
	    } else {
		/* prev_sl must move forward */
		prev_sl = sl;
	    }
	}
    }
}

/*
 * nov_get_backup_servers
 *
 * Retrieve any backup servers available now through 'pseudo-updates'
 */
void nov_get_backup_servers (void)
{
    novpdbtype *pdb, *pdb_next;
    ddbtype *ddb;
    boolean do_poisons_false = FALSE;

    novell_backup_servers_needed = FALSE;
    TIMER_START(novell_backup_servers_timer,
		novell_backup_server_interval * ONESEC);
    /* Find Server offset */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb_next) {
        pdb_next = pdb->next;
        ddb = pdb->ddb;
	if (ddb == NULL)
	    continue;
	igrp2_for_each_peer(ddb, novigrp_advertise_peer_saps,
			    &do_poisons_false);
    }
}

/*
 * novigrp_peershowhook
 * Detailed show hook for IGRP neighbors
 */

static void novigrp_peershowhook (ddbtype *ddb, dual_peertype *peer)
{
    int bucket;
    nov_server_per_source_bynametable *peer_sap_table;
    novell_server_per_source_list *sl;
    boolean printed_headers = FALSE;
    int count, locki;
    ptable *slp;                  /* source list pointers */

    peer_sap_table = peer->protocol_data;
    if (peer_sap_table) {
	/*
	 * Lock the table to avoid it's being free.
	 */
	mem_lock(peer_sap_table);
	for (bucket = 0; bucket < NOVELL_SAP_HASHLEN; bucket++) {
	    /*
	     * Count the number of entry per bucket.
	     */
	    count = 0;	    
	    for (sl = (*peer_sap_table)[bucket]; sl != NULL; sl = sl->next) {
		count++;
	    }
	    if (count == 0)
		continue;
	    slp = malloc(count * sizeof(novell_server_per_source_list *));
	    if (slp == NULL) {
		break;
	    }
	    /* 
	     * Lock the entire bucket before displaying to avoid pointer
	     * corruption.
	     */
	    for (sl = (*peer_sap_table)[bucket], locki = 0; locki < count;
		 locki++, sl = sl->next) {
		mem_lock(sl);
		*slp[locki] = sl;
	    }
	    /*
	     * It's now safe to display everything in this bucket using the
	     * pointer that's been locked.
	     */
	    for (locki = 0, sl = *slp[locki]; locki < count; 
		 locki++, sl = *slp[locki]) {
		if (!printed_headers) {
		    printf("\nServer table for this peer:");
		    printf("\n         Type Name                    Address               Port  Hops");
		    printed_headers = TRUE;
		}
		printf("\n\t %-4x %21s %-8x.%e:%04x %-2d",
		       sl->server_type, sl->server_name,
		       sl->dnet, sl->dhost, sl->dsock, sl->nethops);
		free(sl);
	    }
	    free(slp);
	}
	free(peer_sap_table);
    }
}

/*
 * novigrp_accept_peer
 *
 * Returns TRUE if we should accept the peer.
 */
static boolean novigrp_accept_peer (ddbtype *ddb, igrp2addrtype *source,
				    igrp2_idbtype *iidb)
{
    /*
     * Don't allow neighbors to form if sourced from a subnet other than
     * the primary one.
     */
    if (IPX_PRIMARY_NIDB(iidb->idb)->novellnet != 
	GETLONG(&source->nov_address.net)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * novigrp_peerstatehook
 * Change the state of a peer
 * Return an indication of whether or not the peer was accepted.
 */
static void novigrp_peerstatehook (ddbtype *ddb, dual_peertype *peer,
				   boolean bringing_up)
{
    boolean do_poisons_true = TRUE;
    novigrp_sapq_entry *prev_entry, *current_entry, *next_entry;
    igrp2_idbtype *iidb;
    boolean debug = NOVIGRP_DTARGET(peer->source.nov_address.net, ddb);

    /* 1: Poison everything in the existing table, delete it all */
    novigrp_advertise_peer_saps(ddb, peer, &do_poisons_true);

    /* 2: Free the existing table */
    free(peer->protocol_data);
    peer->protocol_data = NULL;

    /* Do this only if bringing the peer up now. */
    if (bringing_up) {

	/* 3: Create a new table */
	peer->protocol_data =
	    malloc(sizeof(nov_server_per_source_bynametable));
	if (peer->protocol_data == NULL) {
	    return;
	}

	/* 4: Posit the need to send a full update to this peer */
	novigrp_posit_sap_update_peer(ddb, peer, peer->iidb->idb);
    } else {
	if (!(iidb = peer->iidb))
            return;
	prev_entry = NULL;
	for (current_entry = iidb->pdm_queue.qhead; current_entry; 
	     current_entry = next_entry) {
	    next_entry = current_entry->next;
	    if (novigrp_addrmatch(&peer->source, &current_entry->dest)) {
		if (debug)
		    buginf("\nIPXEIGRP: Removing sap entry from sap transmit "
			   "queue for peer %s", 
			   novigrp_printaddr(&peer->source));
		free(current_entry->sappak);
		chunk_free(novigrp_sapQ_chunks, remqueue(&iidb->pdm_queue, 
							 current_entry, 
							 prev_entry));
	    } else {
		prev_entry = current_entry;
	    }
	}
    }
}

/*
 * novigrp_advertise_request
 *
 * Called by DUAL to ask if a destination should be advertised out
 * the supplied interface.
 */
static advertise_request_type novigrp_advertise_request (ddbtype *ddb,
							 igrp2nettype *dest,
							 idbtype *interface)
{
    novpdbtype *pdb;

    pdb = ddb->pdb;

    /*
     * Check outbound interface filters.
     */
    if (interface) {
	if (IPX_PRIMARY_NIDB(interface)->igrp_sap_only) {
	    return(SUPPRESS_ROUTE);
	}
	if (!nov_filtercheck(
		     dest->nov_network,
		     IPX_PRIMARY_NIDB(interface)->oprotofltr[pdb->index])) {
	    return(SUPPRESS_ROUTE);
	}
    }
    if (!nov_filtercheck(dest->nov_network, pdb->ofltr_all)) {
        return(SUPPRESS_ROUTE);
    }
    return(ADVERTISE_ROUTE);
}

/*
 * novigrp_allocate_sap_backup_block
 *
 * Allocate a server backup block for the server, peer, and pdb given
 */
static novell_server_per_source_list *novigrp_allocate_sap_backup_block (
	     nov_server_per_source_bynametable *peer_sap_table,
  	     int hash_entry,
	     novpdbtype *pdb,
	     novell_serveridtype *server)
{
    novell_server_per_source_list *sl;

    /* If not in table right now, add it... */
    sl = malloc(sizeof(novell_server_per_source_list));
    if (sl == NULL) {
	return(NULL);
    }
    sl->next = (*peer_sap_table)[hash_entry];
    (*peer_sap_table)[hash_entry] = sl;
    return(sl);
}


/*
 * novigrp_accept_server_change
 *
 * Reflect the current server data in our 'shadow table'
 */
static void novigrp_accept_server_change (novell_serveridtype *server,
				   dual_peertype *peer_mem,
				   novpdbtype *pdb_mem)
{
    int i;
    nov_server_per_source_bynametable *peer_sap_table;
    novell_server_per_source_list *sl, *prev_sl;
    novpdbtype *pdb = pdb_mem;

    /* First, see if we have a table... */
    peer_sap_table = peer_mem->protocol_data;
    if (peer_sap_table == NULL) {
	return;
    }

    /* We have a table. Find the entry. */
    i = nov_sap_hash(server);

    prev_sl = NULL;
    for (sl = (*peer_sap_table)[i]; sl != NULL; prev_sl = sl, sl = sl->next) {
	if ((sl->server_type == server->server_type) &&
	    (!strcmp(sl->server_name, server->server_name))) {
	    break;
	}
    }
    if (server->nethops >= novell_infinity) {
	/* We are poisoned */
	if (sl != NULL) {
	    novigrp_free_sap_backup_block(sl, prev_sl, peer_sap_table, i);
	}
    } else {
	/* We are NOT poisoned */
	if (sl == NULL) {
	    /* Now we have the data. Now add it to the main server table */
	    if ((sl = novigrp_allocate_sap_backup_block(peer_sap_table, i, pdb,
							server)) == NULL)
		return;
	}
	/* Copy current server information into our table */
	bcopy((uchar *) &(server->server_type), (uchar *) &(sl->server_type),
	      sizeof(novell_serveridtype));
    }
}

/*
 * Return TRUE if interface up
 */
static boolean novigrp_interface_up (idbtype *idb)
{
    return((IPX_PRIMARY_NIDB(idb) == NULL) ? FALSE : 
	   NOVELL_NIDB_IDB_UP(IPX_PRIMARY_NIDB(idb)));
}

/*
 * free_novpdbindex
 * Free a pdb index dynamically.
 */
void free_novpdbindex (novpdbtype *pdb)
{
    nov_proctable[pdb->index] = NULL;
    pdb->mask = 0;
    pdb->index = -1;
}

/*
 * novigrp_process_ifdown
 *
 * Process an ifdown event.  Tear down all neighbors, and delete the
 * IIDB if so instructed.
 */
static void novigrp_process_ifdown (novpdbtype *pdb, novigrp_worktype *work)
{
    ddbtype *ddb;
    igrp2_idbtype *iidb;
    idbtype *idb;

    ddb = pdb->ddb;
    if (!ddb)
	return;
    idb = work->c.ifd.interface;
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
 * novigrp_connstate
 *
 * Process a connstate event from the work queue.  This adds our connected
 * route to the topology table.
 */
static void novigrp_connstate (novpdbtype *pdb, novigrp_worktype *work)
{
    ddbtype *ddb;
    idbtype *interface;
    igrp2_idbtype *iidb;
    dual_newroute route;

    ddb = pdb->ddb;
    interface = work->c.ifc.interface;
    iidb = igrp2_find_iidb(ddb, interface);
    if (!iidb)
	return;

    /*
     * Setup protocol specific parameters for call.
     */
    bzero(&route, sizeof(route));
    route.new_iidb = iidb;
    route.new_dest.nov_network = work->c.ifc.net;
    route.new_vecmetric.mtu = interface->hwptr->maxmtu;
    dual_conn_rtchange(ddb, &route,
		       work->c.ifc.sense ? DUAL_RTUP : DUAL_RTDOWN);
}

/*
 * novigrp_reload_proto_table
 * Reload an EIGRP protocol table, because the IPX routing table has been
 * reloaded.
 */
void novigrp_reload_proto_table (novpdbtype *pdb)
{
    ddbtype *ddb;

    ddb = pdb->ddb;
    if (ddb == NULL)
	return;
    dual_reload_proto_table(ddb);
}

/*
 * novigrp_sendsap_wakeup
 *
 * Wake things up because someone is trying to send a SAP.
 *
 * This is done to wake the slumbering transport process if nothing else
 * is happening, since we don't actually call igrp2_enqueue when the SAP
 * is first presented.
 */
static void novigrp_sendsap_wakeup (novpdbtype *pdb, novigrp_worktype *work)
{
    igrp2_idbtype *iidb;

    /* Call DUAL to make the transport move. */

    iidb = igrp2_find_iidb(pdb->ddb, work->c.send_sap.idb);
    if (iidb) {
	dual_xmit_continue(pdb->ddb, iidb, NULL, NULL);
    }
}

/*
 * novigrp_process_workq
 *
 * Process the work queue.
 *
 * Returns TRUE if we bailed because we hit our event limiter.
 */
#define WORKQ_LIMITER 20		/* Max 20 per pass */
static boolean novigrp_process_workq (novpdbtype *pdb)
{
    novigrp_worktype *work;
    ulong workq_limiter;

    workq_limiter = WORKQ_LIMITER;
    while (!process_is_queue_empty(pdb->watched_workQ)) {

	/* Bail if we should go do something else. */

	workq_limiter--;
	if (!workq_limiter)
	    return(TRUE);

	process_may_suspend();
	work = process_dequeue(pdb->watched_workQ);

	switch (work->type) {
	  case IGRP2_WORK_REDIST:
	  case IGRP2_WORK_UNREDIST:
	    novigrp_process_redist(pdb, work);
	    break;
	  case IGRP2_WORK_IFDOWN:
	    novigrp_process_ifdown(pdb, work);
	    break;
	  case IGRP2_WORK_REDISTALL:
	    novigrp_process_redistribute(pdb, work);
	    break;
	  case IGRP2_WORK_LOST:
	    novigrp_process_lost(pdb, work);
	    break;
	  case IGRP2_WORK_BACKUP:
	    novigrp_process_backup(pdb, work);
	    break;
	  case IGRP2_WORK_CONNSTATE:
	    novigrp_connstate(pdb, work);
	    break;
	  case IGRP2_WORK_SENDSAP:
	    novigrp_sendsap_wakeup(pdb, work);
	    break;
	  case IGRP2_WORK_SUMMARY:
	  case IGRP2_WORK_INTEXTSTATE:
	  case IGRP2_WORK_PASSIVEINTF:

	    /*
	     * Do nothing
	     */
	    break;
	  case IGRP2_WORK_TABLERELOAD:
	    novigrp_reload_proto_table(pdb);
	    break;
	}
	chunk_free(pdb->ddb->workq_chunk, work);
	dual_finish_update_send(pdb->ddb);
    }
    return(FALSE);
}

/*
 * novigrp_rcv_sap
 *
 * Strip off EIGRP headers and send to SAP process.
 *
 * Currently, the only SAP packets supported over EIGRP are general queries
 * and general responses.
 */
static void novigrp_rcv_sap (paktype *pak, dual_peertype *peer, ddbtype *ddb)
{
    ipxhdrtype *ipx;
    uchar *src, *dst;
    novell_sappacket *sapp;
    
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (novellsap_debug || novellsap_event_debug)
      buginf("\nIPXEIGRP: Received EIGRP SAP from %x.%e",
	     peer->source.nov_address.net, peer->source.nov_address.host);

    /* Strip IGRP2 header bytes */
    dst = (uchar *)XNSDATASTART(pak);
    src = dst + IGRP2HEADERBYTES;
    bcopy(src, dst, ipx->len - (IGRP2HEADERBYTES + XNSHEADERBYTES));
    /* Make the packet look perfectly 'valid' from IPX's point of view */
    ipx->len -= IGRP2HEADERBYTES;
    pak->datagramsize -= IGRP2HEADERBYTES;
    sapp = (novell_sappacket *)(XNSDATASTART(pak));
    switch (sapp->response_type) {
      case NOVELL_SAP_GENQUERY:
	if (((novell_querytype *) sapp)->server_type != NOVELL_EVERYONE) {
	    errmsg(&msgsym(BADIGRPSAPTYPE, IPX), "received",
		   sapp->response_type);
	} else {
	    novell_posit_sap_update(pak->nif_input,
				    (GET2WD(ipx->ssnet) ?
				     GET2WD(ipx->ssnet) :
				     pak->nif_input->novellnet),
				    ipx->shost, ipx->ssock,
				    IPX_FULL_NEEDED,
				    IPX_DESTINATION_UNICAST,
				    NOVELL_EVERYONE,
				    FALSE, FALSE);
	}
	break;
      case NOVELL_SAP_GENRESPONSE:
	novell_accept_sap_data(ddb->pdb, pak, sapp, NET_IGRP, "EIGRP",
			       novigrp_accept_server_change,
			       peer, ddb->pdb);
	break;
      default:
	errmsg(&msgsym(BADIGRPSAPTYPE, IPX), "received", sapp->response_type);
	break;
    }
}

/*
 * novigrp_enable_process_wait
 *
 * Enable process blocking predicate for Hello process
 *
 * The process is set up to wait on the watched input queue.
 */
static void novigrp_enable_process_wait (ddbtype *ddb)
{
    novpdbtype *pdb;

    pdb = ddb->pdb;
    if (!pdb)
	return;
    process_watch_queue(pdb->watched_inputq, ENABLE, RECURRING);
}

/*
 * novigrp_input_packet
 *
 * Returns the next input packet, or NULL if it is not there.
 * Returns the source of the packet as well.
 */
static paktype *novigrp_input_packet (ddbtype *ddb, igrp2addrtype *source)
{
    paktype *pak;
    novpdbtype *pdb;
    ipxhdrtype *ipx;

    pdb = ddb->pdb;
    if (!pdb)
	return(NULL);
    pak = process_dequeue(pdb->watched_inputq);
    if (pak) {
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	source->nov_address.net = GETLONG(&ipx->ssnet);
	bcopy(ipx->shost, source->nov_address.host, IEEEBYTES);
    }
    return(pak);
}

/*
 * novigrp_router
 * Main IGRP2 routing process.  One per major network.
 */

#define PACKET_LIMITER 20		/* Max packets per pass */
static forktype novigrp_router (novpdbtype *pdb)
{
    paktype *pak;
    ipxhdrtype *ipx;
    igrp2type *igrp2;
    ddbtype *ddb;
    dual_peertype *peer;
    ulong packet_limiter;
    igrp2_input_packet *inpak;
    ulong major, minor;
    boolean once_around;

    process_wait_on_system_init();

    ddb = pdb->ddb;

    /* Wake up the hello process. */

    process_set_boolean(ddb->run_hello_process, TRUE);

    /* Set up the event predicates. */

    process_watch_queue(ddb->ddb_inputQ, ENABLE, RECURRING);
    process_watch_queue(pdb->watched_workQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&ddb->ddb_master_timer, ENABLE);

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
		pak = inpak->packet;
		if (novigrp_interface_up(inpak->if_input)) {
		    novell_stat[NOVELL_IGRPIN]++;
		    ipx   = (ipxhdrtype *)ipxheadstart(pak);
		    igrp2 = (igrp2type *)(XNSDATASTART(pak));
		    
		    /*
		     * Process fixed header.
		     */
		    peer = igrp2_receive(ddb, pak, 
					 (igrp2addrtype *) &ipx->ssnet, 
					 inpak->if_input);
		    /*
		     * Switch on packet type.  Routines must check version
		     * number.
		     */
		    if (peer && !inpak->if_input->static_routing) {
			
			switch (igrp2->opcode) {
			  case IGRP_OPC_UPDATE:
			    novell_stat[NOVELL_UPDATE_IN]++;
			    novigrp_receive(ddb, peer, pak, igrp2);
			    break;
			  case IGRP_OPC_QUERY:
			    novell_stat[NOVELL_QUERY_IN]++;
			    novigrp_receive(ddb, peer, pak, igrp2);
			    break;
			  case IGRP_OPC_REPLY:
			    novell_stat[NOVELL_REPLY_IN]++;
			    novigrp_receive(ddb, peer, pak, igrp2);
			    break;
			  case IGRP_OPC_IPXSAP:
			    novell_stat[NOVELL_IGRP_SAPIN]++;
			    novigrp_rcv_sap(pak, peer, ddb);
			    break;
			  default:
			    break;
			}
		    } else if (peer &&
			       (novelligrp_debug || novelligrp_event_debug)) {
			buginf("\nIPXEIGRP: Input packet while routing "
			       "disabled on %s", inpak->if_input->namestring);
		    }
		}
		datagram_done(pak);
		chunk_free(igrp2_inpak_chunks, inpak);
		
		/* Do something else if we've done lots of packets. */
		
		packet_limiter--;
		if (!packet_limiter) {
		    once_around = TRUE;
		    break;
		}
	    }
	    /* Process the work queue. */
	    
	    if (!process_is_queue_empty(pdb->watched_workQ))
		once_around = once_around || novigrp_process_workq(pdb);

	    /* Process any timer expirations. */
	    
	    if (mgd_timer_expired(&ddb->ddb_master_timer))
		once_around = once_around || igrp2_process_managed_timers(ddb);
	    
	} while (once_around);
    }
}

/*
 * novigrp_pdbinit
 * Initialize the pdb data structure.
 */
static novpdbtype *novigrp_pdbinit (int asystem)
{
    novpdbtype *pdb;

    /*
     * Attempt to initialize the rip pdb if it hasen't already been
     * initialized.  This is done so we get a mask when we set the
     * igrp distflag.
     */
    novrip_pdbinit();

    /*
     * Create the pdb.
     */
    pdb = malloc_named(sizeof(novpdbtype), "IPX PDB");
    if (pdb == NULL) {
	return(NULL);
    }
    /*
     * Allocate an unused router process flag
     */
    if (allocate_novpdbindex(pdb) == FALSE) {
	free(pdb);
	return(NULL);
    }	
    pdb->name = "eigrp";
    pdb->pname = "IPX EIGRP";
    pdb->process = novigrp_router;
    pdb->cleanup = novigrp_cleanup;
    pdb->default_include_net = FALSE;
    pdb->entered = FALSE;
    pdb->holddown = novigrp_holddown;
    /*
     * pdb->query = novigrp_sendrequest;
     */
    pdb->holdokay = FALSE;
    pdb->proctype = NET_IGRP;
    pdb->distflag = pdb->mask;

    /*
     * Automatic redistribution of rip <---> igrp
     */
    novrip_pdb.distflag |= pdb->mask;
    pdb->distflag |= novrip_pdb.mask;

    /*
     * And don't forget connected and static. Floating static is not
     * redistributed by default.
     */
    pdb->distflag |= novconnected_pdb.mask | novstatic_pdb.mask;

    pdb->redistallowed = PROC_REDISTMASK;
    pdb->redistribute = nov_redistribute;
    pdb->redist_callback = novigrp_redist_callback;
    pdb->add_route = novigrp_callbackup_routes;
    pdb->lost_route = novigrp_lost_route;
    pdb->exteriors_out = NET_DEXTERIOR;
    pdb->exteriors_in = NET_DEXTERIOR;
    pdb->broadcasttime = pdb->def_broadcasttime = 5 * ONESEC;
    pdb->invalidtime = pdb->def_invalidtime = 0;
    pdb->holdtime = pdb->def_holdtime = 0;
    pdb->flushtime = pdb->def_flushtime = 0;
    pdb->distance = pdb->def_distance = IGRP_DISTANCE;
    pdb->igrp_variance = IGRP_DEFVARIANCE;
    pdb->asystem = asystem;
    pdb->igrp_k1 = pdb->igrp_k1_def = IGRP_K1_DEFAULT;
    pdb->igrp_k2 = pdb->igrp_k2_def = IGRP_K2_DEFAULT;
    pdb->igrp_k3 = pdb->igrp_k3_def = IGRP_K3_DEFAULT;
    pdb->igrp_k4 = pdb->igrp_k4_def = IGRP_K4_DEFAULT;
    pdb->igrp_k5 = pdb->igrp_k5_def = IGRP_K5_DEFAULT;
    pdb->igrp_maxhops = pdb->igrp_def_maxhops = IGRP_MAXHOPS;

    /* Initialize the input queue and work queue */

    pdb->watched_inputq = create_watched_queue("IPX-EIGRP input queue", 0, 0);
    pdb->watched_workQ = create_watched_queue("IPX-EIGRP work queue", 0, 1);

    enqueue(&novpdbQ, pdb);
    return(pdb);
}

static const char novigrp_nei_banner[] = {"\n\nIPX EIGRP Neighbors for process %d"};
static const char novigrp_int_banner[] = {"\n\nIPX EIGRP Interfaces for process %d"};

/*
 * novigrp_show_neighbors
 *
 * Process "show nov igrp {neighbors|interface} <asystem> [<interface><unit>]
 *
 * OBJ(int,1) = asystem
 * OBJ(int,2) = "servers"
 * OBJ(int,3) = "detail"
 * OBJ(int,4) = TRUE if "neighbors", FALSE if common_str_interface
 * OBJ(idb,1) = interface or NULL
 */
void novigrp_show_neighbors (parseinfo *csb)
{
    ddbtype *ddb;
    novpdbtype *pdb;
    int     asystem;

    if (!novell_igrp_running) {
        printf ("\n%% IPX EIGRP not running");
        return;
    }

    asystem = GETOBJ(int,1);
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	ddb = pdb->ddb;
	if (!ddb || !pdb->asystem) 
	    continue;
	if (pdb->asystem == GETOBJ(int,1) || !asystem) {
	    if (GETOBJ(int,4)) {
		printf(novigrp_nei_banner, pdb->asystem);
		igrp2_show_neighbors(ddb, GETOBJ(idb,1), GETOBJ(int,2),
				     GETOBJ(int,3));
	    } else {
		printf(novigrp_int_banner, pdb->asystem);
		igrp2_show_interface(ddb, GETOBJ(idb,1), GETOBJ(int,3));
	    }
	    if (asystem) 
		break;
	}
    }
}

static const char novigrp_top_banner[] = {"\nIPX EIGRP Topology Table for process %d"};

/*
 * novigrp_show_topology
 *
 * Process "show nov eigrp topology <asystem>
 */
void novigrp_show_topology (parseinfo *csb)
{
    ddbtype *ddb;
    novpdbtype *pdb;
    int     asystem;

    if (!novell_igrp_running) {
	printf ("\n%% IPX EIGRP not running");
	return;
    }
    asystem = GETOBJ(int,1);
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
        ddb = pdb->ddb;
	if (ddb == NULL)
	    continue;
	if (!pdb->running)
	    continue;
	if (asystem && (pdb->asystem != asystem))
	    continue;

	if  (GETOBJ(int,2)) {
	    novigrp_show_topology_route(ddb, GETOBJ(int,2));
	} else {
	    printf(novigrp_top_banner, pdb->asystem);
	    igrp2_show_topology(ddb, csb->which);
	    if (asystem) 
		break;
	}
    }
}

/*
 * novigrp_conn_rtchange
 * Queue a connstate event on the work queue.
 */
static void novigrp_conn_rtchange (novpdbtype *pdb, idbtype *interface,
				   ulong net, boolean sense)
{
    novigrp_worktype *work;

    if (pdb->running == FALSE)
	return;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    work = chunk_malloc(pdb->ddb->workq_chunk);
    if (work == NULL) {
	return;
    }

    work->c.ifc.interface = interface;
    work->c.ifc.net = net;
    work->c.ifc.sense = sense;
    novigrp_qevent(pdb, work, IGRP2_WORK_CONNSTATE);
}

/*
 * novigrp_ifdown_enq
 * Queue an ifdown event on the work queue.
 */
static void novigrp_ifdown_enq (novpdbtype *pdb, idbtype *interface,
				boolean delete)
{
    novigrp_worktype *work;
    igrp2_idbtype *iidb;
    ddbtype *ddb;

    ddb = pdb->ddb;
    if (pdb->running == FALSE)
	return;

    /*
     * We are shutting down.  Ignore requests.
     */
    if (pdb->shutdown) {
	return;
    }

    iidb = igrp2_find_iidb(ddb, interface);
    if (!iidb)
	return;

    work = chunk_malloc(ddb->workq_chunk);
    if (work == NULL) {
	return;
    }
    iidb->iidb_going_down = delete;
    work->c.ifd.interface = interface;
    novigrp_qevent(pdb, work, IGRP2_WORK_IFDOWN);
}

/*
 * novigrp_redist_conn_state
 *
 * Update redistributed connected routes based on the current state of
 * the interface and of EIGRP.
 */
static void novigrp_redist_conn_state(novpdbtype *pdb, idbtype *idb, ulong net,
				      boolean sense)
{
    novpdbtype *dist;
    boolean disted;
    novndbtype *ndb;
    
    ndb = novrip_find_rt(net, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT);
    if (ndb != NULL) {
	dist = NOV_INDEX2PDB(ndb->novpdbindex);
	disted = ((dist->proctype == NET_CONNECTED) &&
		  (dist->mask & pdb->distflag));
    } else {
	disted = FALSE;
    }
    if (!disted)
	return;

    /*
     * Redistribute in the opposite sense of the setting of the network
     * command (but if the interface is down, don't redistribute the
     * route if we would otherwise).
     */
    if (sense) {
	novigrp_redist_enq(pdb, net, ROUTE_DOWN);
    } else if (NOVELL_NIDB_IDB_UP(IPX_PRIMARY_NIDB(idb))) {
	novigrp_redist_enq(pdb, net, ROUTE_UP);
    }
}

/*
 * novigrp_set_mtu
 */
static void novigrp_set_mtu (igrp2_idbtype *iidb)
{
    idbtype *idb;

    if (iidb) {
	idb = iidb->idb;
	iidb->max_packet_size = min(max_proto_mtu(idb, LINK_NOVELL),
				    MAXIGRP2BYTES) - XNSHEADERBYTES;
    }
}

/*
 * novigrp_mtu_change
 */
void novigrp_mtu_change (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    idbtype *idb;
    novpdbtype *pdb;
    igrp2_idbtype *iidb;
    ddbtype *ddb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	    if (pdb->proctype != NET_IGRP)
		continue;
	    ddb = pdb->ddb;
	    if (!ddb)
		continue;
	    iidb = igrp2_find_iidb(pdb->ddb, idb);
	    if (ddb->set_mtu)
		(*ddb->set_mtu)(iidb);
	    igrp2_take_nbrs_down(pdb->ddb, idb, FALSE, "mtu change");
	}
    }
}

/*
 * novigrp_network_command
 *
 * Process "network" command for an IGRP2 routing process. This function
 * will obtain the idbs associated with the network number configured.
 * Each idb will be stored in an igrp2_idbtype data structure that resides
 * in the ddb.
 */
void novigrp_network_command (novpdbtype *pdb, ulong net, boolean sense)
{
    idbtype    *idb, *next;
    ddbtype    *ddb;
    igrp2_idbtype *iidb;

    ddb = pdb->ddb;

    for (idb = SWIDBQ_HEAD; idb; idb = next) {
	next = IDB_NEXT(idb);
	if (!IPX_PRIMARY_NIDB(idb) || 
	    !IPX_PRIMARY_NIDB(idb)->novellnet)
	    continue;
	
	if (IPX_PRIMARY_NIDB(idb)->novellnet == net) {
	    iidb = igrp2_find_iidb(ddb, idb);

	    /* If we need to create an IIDB, do it. */

	    if (sense) {
		if (!iidb) {
		    igrp2_allocate_iidb(ddb, idb, FALSE);
		} else {
		    iidb->iidb_going_down = FALSE; /* Cancel any deletion */
		}
	    }

	    /* Twiddle the route.  Remove the old before adding the new. */

	    if (!sense)
		novigrp_conn_rtchange(pdb, idb, net, sense);
	    novigrp_redist_conn_state(pdb, idb, net, sense);
	    if (sense)
		novigrp_conn_rtchange(pdb, idb, net, sense);

	    /* If we need to delete an IIDB, do it. */

	    if (!sense && iidb) {
		novigrp_ifdown_enq(ddb->pdb, idb, TRUE);
	    }
	}

    }
}

/*
 * novigrp_address_command
 *
 * Called when an IPX address is added or deleted from an interface.
 */
void novigrp_address_command (idbtype *idb, ulong net, boolean adding)
{

    novpdbtype       *pdb;
    igrp2_idbtype    *iidb;
    boolean	     eigrp_net;

    eigrp_net = FALSE;

    /*
     * Inform all IGRP2 speakers that are configured on this interface
     * there is an address change.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP)
	    continue;

	iidb = igrp2_find_iidb(pdb->ddb, idb);
	eigrp_net = novigrp_net_in_pdb(pdb, net);

	/*
	 * If the network is not the primary nidb and we are trying
	 * to add it, return.
	 */
        if (IPX_PRIMARY_NIDB(idb)->novellnet != net) {
	    if (adding && eigrp_net) {
		printf("\n%% Can't configure IPX-EIGRP on secondary network %x",
		       net);
	    }
            return;
        }

	/*
	 * If adding a new address, determine if IGRP should be configured
	 * on interface (based on a previous 'network' command given in a
	 * strange order.) If so, allocate the resources to do so... , or
	 * delete them if removing interface from Novell and it was in IGRP.
	 */
	if (adding) {
	    if (eigrp_net) {		/* A net we care about? */
		if (!iidb) {
		    igrp2_allocate_iidb(pdb->ddb, idb, FALSE);
		} else {
		    iidb->iidb_going_down = FALSE; /* Cancel any deletion */
		}
		novigrp_conn_rtchange(pdb, idb, net, adding);
	    }
	} else {			/* Deleting */
	    if (eigrp_net) {		/* It's ours */
		novigrp_conn_rtchange(pdb, idb, net, adding);
	    }
	    if (iidb) {
		novigrp_ifdown_enq(pdb, idb, TRUE);
	    }
	}
    }
}

/*
 * novigrp_route_adjust
 *
 * Called when a directly connected IPX network has changed status.
 * Inform the DUAL finite state machine about change.
 */
void novigrp_route_adjust (novpdbtype *inpdb, idbtype *idb)
{
    novpdbtype   *pdb, *next;
    ulong net;

    /*
     * Process loop once if a pdb was passed in. Otherwise, process
     * all pdbs associated with interface.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = next) {
	if (inpdb) {
	    pdb = inpdb;
	    next = NULL;
	} else {
            next = pdb->next;
	}
	if (pdb->proctype != NET_IGRP) 
	    continue;

	net = IPX_PRIMARY_NIDB(idb)->novellnet;
	if (!net)
	    continue; 

	if (!igrp2_find_iidb(pdb->ddb, idb)) 
	    continue;

	if (!novigrp_net_in_pdb(pdb, net))
	    continue;

	if (novigrp_interface_up(idb)) { /* It's up */
	    novigrp_conn_rtchange(pdb, idb, net, TRUE);
	} else {			/* It's down */
	    novigrp_conn_rtchange(pdb, idb, net, FALSE);
	    novigrp_ifdown_enq(pdb, idb, FALSE);
	}
    }
}

/*
 * novigrp_add_item
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
static ulong novigrp_add_item (ddbtype *ddb, igrp2_idbtype *iidb,
			       dual_ndbtype *dndb, void *packet_ptr,
			       ulong bytes_left, boolean unreachable)
{
    ulong item_length;
    dual_rdbtype *drdb;
    boolean external;
    novigrp_metrictype *novm;
    novigrp_externtype *nove;
    ulong *dest;
    ulong origin;
    ulong flags;
    boolean debug;
    novigrp_extdatatype *ext_info;

    debug = NOVIGRP_DTARGET(dndb->dndb_dest.nov_network, ddb);
    flags = 0;
    external = igrp2_route_is_external(dndb);

    /* Bail if there's no room for the item.  (There better be!) */

    if (external) {
	item_length = sizeof(novigrp_externtype);
    } else {
	item_length = sizeof(novigrp_metrictype);
    }
    if (item_length > bytes_left)
	return(0);			/* Uhoh! */

    drdb = dndb->dndb_rdb;
    
    /* Build the item depending on the type. */

    if (external) {			/* External route */
	nove = packet_ptr;
	dest = &nove->dest;
	PUTSHORT(&nove->type, NOVIGRP_EXTERNAL);
	PUTSHORT(&nove->length, item_length);
	PUTLONG(&nove->nexthop_net, 0);
	if (drdb) {			/* It has to be there, but we check */
	    if (!drdb->drdb_data) {
		errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
		return(0);
	    }
	    bcopy(drdb->drdb_data, &nove->routerid,
		  sizeof(novigrp_extdatatype));
	    ext_info = (novigrp_extdatatype *)&nove->routerid;
	    ext_info->ext_delay = ext_info->ext_delay + 
		NOVELL_IDB_TICKS(IPX_PRIMARY_NIDB(iidb->idb));
	} else {			/* Uhoh */
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return(0);
	}
	igrp2_metric_encode(ddb, dndb, iidb, (igrp2_mdecode *)&nove->delay,
			    unreachable, FALSE);
	if (debug)
	    buginf("\nIPXEIGRP: External %x metric %u hop %d delay %u", 
		   dndb->dndb_dest.nov_network, dndb->dndb_metric, 
		   ((novigrp_extdatatype *)drdb->drdb_data)->ext_metric,
		   ((novigrp_extdatatype *)drdb->drdb_data)->ext_delay);

    } else {				/* Internal route */

	/*
	 * If route is marked as a candidate default, consider
	 * performing output filtering on it.
	 */
	novm = packet_ptr;
	dest = &novm->dest;
	PUTSHORT(&novm->type, NOVIGRP_METRIC);
	PUTSHORT(&novm->length, item_length);
	PUTLONG(&novm->nexthop_net, 0);
	if (drdb) {
	    origin = drdb->drdb_origin;
	} else {
	    origin = IGRP2_ORG_IGRP2;
	}
	igrp2_metric_encode(ddb, dndb, iidb, (igrp2_mdecode *)&novm->delay,
			    unreachable, (origin == IGRP2_ORG_CONNECTED));
	if (debug)
	    buginf("\nIPXEIGRP: Internal %x metric %u",
		   dndb->dndb_dest.nov_network, dndb->dndb_vecmetric.delay +
		   dndb->dndb_vecmetric.bandwidth);
    }
    PUTLONG(dest, dndb->dndb_dest.nov_network);
    return(item_length);
}

/*
 * novigrp_item_size
 *
 * Return the size of the packet item that would be necessary to carry
 * the supplied route.  We return the worst-case value, since the
 * item may change from internal to external between packetization
 * and transmission.
 */
static ulong novigrp_item_size (dual_ndbtype *dndb)
{
    return(max(sizeof(novigrp_externtype), sizeof(novigrp_metrictype)));
}

/*
 * novigrp_send_sap
 *
 * Send a SAP packet to an EIGRP neighbor
 *
 * Currently, the only SAP packets supported over EIGRP are general queries
 * and general responses.
 */
boolean novigrp_send_sap (paktype *sappak,
			  enum IPX_DESTINATION destination_type,
			  nidbtype *nidb)
{
    novpdbtype *pdb;
    ipxhdrtype *sapipx;
    uchar *sap_data;
    ddbtype *ddb;
    dual_peertype  *peer;
    igrp2_idbtype  *iidb;
    igrp2addrtype igrp_addr;
    novell_sappacket *sapp;
    igrp2type *igrp2;
    ushort sap_pdulen;
    novigrp_worktype *work;
    novigrp_sapq_entry *sapq_entry;
    
    sapipx = (ipxhdrtype *)ipxheadstart(sappak);
    sapp = (novell_sappacket *)(XNSDATASTART(sappak));

    if (((sapp->response_type != NOVELL_SAP_GENQUERY) &&
	 (sapp->response_type != NOVELL_SAP_GENRESPONSE)) ||
	((sapp->response_type == NOVELL_SAP_GENQUERY) &&
	 (((novell_querytype *) sapp)->server_type != NOVELL_EVERYONE))) {
	errmsg(&msgsym(BADIGRPSAPTYPE, IPX), "sent", sapp->response_type);
	datagram_done(sappak);
	return(FALSE);
    }
    bcopy(sapipx->ddnet, &(igrp_addr.nov_address.net), XNS_NETBYTES);
    ieee_copy(sapipx->dhost, igrp_addr.nov_address.host); 

    sap_pdulen = sapipx->len - XNSHEADERBYTES;
    sap_data = ipxdatastart(sappak);

    /*	
     * Enqueue this packet for each process on this interface.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP)
	    continue;

	/*
	 * We are shutting down.  Ignore requests.
	 */
	if (pdb->shutdown)
	    continue;

	ddb = pdb->ddb;
        if (!(iidb = igrp2_find_iidb(ddb, nidb->idb))) 
	    continue;

	/*
	 * If there are no peers on this interface don't bother
	 * sending the packet.
	 */
	if (!igrp2_peer_count(iidb)) {
	    if (novellsap_event_debug ||novellsap_debug)
		buginf("\nIPXEIGRP: EIGRP SAP not sent - no peers");
	    continue;
	}

	if (destination_type == IPX_DESTINATION_BROADCAST) {
	   peer = NULL;
	} else {
	    peer = igrp2_find_peer(ddb, &igrp_addr, nidb->idb);
	    if (peer == NULL)
	        continue;
	}
	igrp2 = igrp2_getbuf(sap_pdulen);
	if (igrp2 == NULL) {
	    errmsg(&msgsym(BADIGRPSAP, IPX), nidb->idb->namestring);
	    datagram_done(sappak);
	    return(FALSE);
	}
	/*
	 * Copy the SAP header and server data to the beginning of IGRP data.
	 */
	bcopy(sap_data, igrp2_packet_data(igrp2), sap_pdulen);
	if (novellsap_event_debug ||novellsap_debug)
	    buginf("\nIGRP SAP sent");

	/* Set up the chunk handler if it isn't already. */

	if (!novigrp_sapQ_chunks) {
	    novigrp_sapQ_chunks = chunk_create(sizeof(novigrp_sapq_entry),
					       NOVIGRP_SAPQ_CHUNK_COUNT,
					       CHUNK_FLAGS_DYNAMIC,
					       NULL, 0,
					       "IPX EIGRP SAP headers");
	}

	/* Grab a header and enqueue it. */

	sapq_entry = chunk_malloc(novigrp_sapQ_chunks);
	if (sapq_entry) {
	    sapq_entry->sappak = igrp2;
	    sapq_entry->length = sap_pdulen;
	    sapq_entry->destination_type = destination_type;
	    sapq_entry->dest = igrp_addr;
	    enqueue(&iidb->pdm_queue, sapq_entry);
	} else {			/* Out of memory! */
	    errmsg(&msgsym(BADIGRPSAP, IPX), nidb->idb->namestring);
	    free(igrp2);
	    datagram_done(sappak);
	    return(FALSE);
	}

	/* If this is the first entry on the queue, send a work queue item. */

	if (iidb->pdm_queue.count == 1) {
	    if (!(work = chunk_malloc(ddb->workq_chunk))) {
		return(FALSE);
	    }
	    work->c.send_sap.idb = nidb->idb;
	    novigrp_qevent(pdb, work, IGRP2_WORK_SENDSAP);
	}
    }
    datagram_done(sappak);
    return(TRUE);
}

/*
 * novigrp_transport_ready
 *
 * Called when the transport is ready to send a packet (and perhaps has
 * completed one).  We process the ACK if it's one of ours, and then
 * try enqueuing another SAP if we have one.
 */
static void novigrp_transport_ready (ddbtype *ddb,
				     igrp2_idbtype *iidb,
				     igrp2_pakdesc *pakdesc,
				     dual_peertype *peer,
				     boolean others_pending)
{
    novigrp_sapq_entry *sapq_entry;
    boolean multicast;

    /* Process the acknowledgement, if any. */

    if (pakdesc && !pakdesc->refcount) {
	if (pakdesc->opcode != IGRP_OPC_IPXSAP) {
	    errmsg(&msgsym(INTERNAL, DUAL), ddb->name, ddb->asystem);
	    return;
	}
	free(pakdesc->pregen_pak);	/* Free our packet. */
    }

    /* If we have nothing to say, bail. */

    if (!iidb->pdm_queue.qhead)
	return;

    /*
     * If DUAL has something to say, and we're still deferring to DUAL,
     * bail.
     */
    if (others_pending && iidb->pdm_defer_count) {
	return;
    }

    /* It's OK to send something.  Find something to send. */

    while (TRUE) {
	if (iidb->pdm_queue.qhead) {
	    
	    /* Take a look at the top of the queue without dequeueing. */
	    
	    sapq_entry = iidb->pdm_queue.qhead;
	    multicast =
		(sapq_entry->destination_type == IPX_DESTINATION_BROADCAST);
	    
	    /* Bail if we're not flow-ready for this guy. */
	    
	    if (multicast) {
		peer = NULL;
		if (!igrp2_interface_is_flow_ready(iidb))
		    return;
	    } else {
		peer = igrp2_find_peer(ddb, &sapq_entry->dest, iidb->idb);
		
		/* If the peer is gone, toss the packet and start over. */
		
		if (!peer) {
		    sapq_entry = dequeue(&iidb->pdm_queue);
		    free(sapq_entry->sappak);
		    chunk_free(novigrp_sapQ_chunks, sapq_entry);
		    continue;
		}
		if (!igrp2_peer_is_flow_ready(peer))
		    return;
	    }

	    /* Send the packet as a multicast or to the peer as appropriate. */

	    sapq_entry = dequeue(&iidb->pdm_queue);
	    pakdesc = igrp2_enqueue(ddb, peer, iidb, TRUE);
	    if (pakdesc) {
		pakdesc->opcode = IGRP_OPC_IPXSAP;
		novigrp_sap_serial++;
		if (!novigrp_sap_serial) /* Rollover! */
		    novigrp_sap_serial = 1;
		pakdesc->start_serno = novigrp_sap_serial;
		pakdesc->end_serno = novigrp_sap_serial;
		pakdesc->pregen_pak = sapq_entry->sappak;
		pakdesc->length = sapq_entry->length;
		igrp2_debug_enqueued_pkt(ddb, peer, iidb, pakdesc);
	    } else {			/* Out of memory! */
		free(sapq_entry->sappak);
	    }
	    chunk_free(novigrp_sapQ_chunks, sapq_entry);

	    /* We sent one, so we'll have to defer for awhile. */

	    iidb->pdm_defer_count = IPX_EIGRP_SAP_DEFER_COUNT;
	}
	break;
    }
}

/*
 * novigrp_iidb_cleanup
 *
 * IPX-specific IIDB cleanup
 *
 * Clean out the SAP queue.
 */
static void novigrp_iidb_cleanup (ddbtype *ddb, igrp2_idbtype *iidb)
{
    novigrp_sapq_entry *sapq_entry;

    /* Flush 'em out. */

    while ((sapq_entry = dequeue(&iidb->pdm_queue))) {
	free(sapq_entry->sappak);
	chunk_free(novigrp_sapQ_chunks, sapq_entry);
    }
}

/*
 * novigrp_have_peer
 * See if an IPX IGRP peer exists out this interface
 */
boolean novigrp_have_peer (nidbtype *nidb)
{
    novpdbtype *pdb;
    ddbtype *ddb;
    igrp2_idbtype  *iidb;


    /* Only primary NIDBs have peers... */
    if (IPX_NIDB_IS_SECONDARY(nidb))
        return(FALSE);
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
        if (pdb->proctype != NET_IGRP)
            continue;
        ddb = pdb->ddb;
        if (!(iidb = igrp2_find_iidb(ddb, nidb->idb)))
            continue;
        if (igrp2_peer_count(iidb)) 
	    return(TRUE);	
    }
    return(FALSE);
}

/*
 * novigrp_rt_access_list_change
 *
 * An access list either changed its contents, or an access list
 * usage on an interface was changed.
 */
void novigrp_rt_access_list_change (ulong list)
{
    novpdbtype *pdb;
    ddbtype *ddb;
    idbtype *idb;
    boolean done;
    int i;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	done = FALSE;
        if (pdb->proctype != NET_IGRP)
            continue;
        ddb = pdb->ddb;
	for (i = 0; i < MAX_NOVPROCS; i++) {
	    if ((pdb->ofltr_proto[i]  == list) ||
		(pdb->ofltr_all == list) ||  
		(pdb->ifltr_all == list)) {
	        FOR_ALL_SWIDBS(idb)
	            igrp2_take_nbrs_down(ddb, idb, FALSE,
					 "route filter changed");
		done = TRUE;
	        break;
	    }
        }
	if (!done) {
            FOR_ALL_SWIDBS(idb) {
                if (IPX_PRIMARY_NIDB(idb)) {
                    if ((IPX_PRIMARY_NIDB(idb)->oprotofltr[pdb->index] == 
			list) ||
                       (IPX_PRIMARY_NIDB(idb)->iprotofltr[pdb->index] == list))
		        igrp2_take_nbrs_down(ddb, idb, FALSE,
					     "route filter changed");
                }
            }
        }
    }
}

/*
 * novigrp_sap_access_list_work
 * novigrp_sap_access_list_change
 *
 * An access list either changed its contents, or an access list
 * usage on an interface was changed.
 * If inidb is non-NULL, clear neighbors only for that IDB.
 * If inidb is NULL, clear all neighbors on interfaces using access list 'list'
 */
void novigrp_sap_access_list_work (ulong list, idbtype *inidb)
{
    novpdbtype *pdb;
    ddbtype *ddb;
    idbtype *idb;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
        if (pdb->proctype != NET_IGRP)
            continue;
        ddb = pdb->ddb;
	if (inidb != NULL) {
	    igrp2_take_nbrs_down(ddb, inidb, FALSE, "SAP filter changed");
	} else {
	    FOR_ALL_SWIDBS(idb) {
		if ((idb->nov_sap_infilterlist == list) ||
		    (idb->nov_sap_outfilterlist == list) ||
		    (idb->nov_sap_routerfilterlist == list)) {
		    igrp2_take_nbrs_down(ddb, idb, FALSE,
					 "SAP filter changed");
		}
	    }
	}
    }
}

/*
 * novigrp_sap_access_list_change
 *
 * Function to call when a SAP access list changes for any reason
 */
void novigrp_sap_access_list_change (ulong list)
{
    novpdbtype *pdb;

    novigrp_sap_access_list_work(list, NULL);
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_NLSP && pdb->osi_pdb) {
	    reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
	    reg_invoke_isis_mark_L1_changed(pdb->osi_pdb);
	}
    }
}

/*
 * novigrp_test_command
 * This isn't really a command, it is a helper routine to find the ddb
 * and fill in the ipx address for the generic test command.
 * OBJ(int,1) has the type
 * OBJ(int,2) has the as number.
 * OBJ(int,3) has the ipx address.
 */
void novigrp_test_command (parseinfo *csb)
{
#ifdef notdef
    int as;
    ddbtype *ddb;
    novpdbtype *pdb;

    as = GETOBJ(int,2);
    if (as == 0) {
	printf("\nNo as number");
	return;
    }

    ddb = NULL;
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next)
	if ((pdb->proctype == NET_IGRP) && (pdb->asystem == as)) {
	    ddb = pdb->ddb;
	    break;
	}
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }

    ddb->ddb_dest.nov_network = GETOBJ(int,3);
    dual_test_command(ddb, GETOBJ(int,1));
#endif /* notdef */
}

/*
 * novigrp_debug_command
 * Parse an ipx address for the fsm debug command
 * OBJ(int,1) AS number
 * OBJ(int,2) ipx address
 * OBJ(paddr,1) ipx neighbor address
 */
void novigrp_debug_command (parseinfo *csb)
{
    int as;
    ddbtype *ddb;
    novpdbtype *pdb;

    as = GETOBJ(int,1);
    ddb = NULL;
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next)
	if ((pdb->proctype == NET_IGRP) && (pdb->asystem == as)) {
	    ddb = pdb->ddb;
	    break;
	}
    if (ddb == NULL) {
	printf("\nCouldn't find process");
	return;
    }

    if (csb->sense) {
	switch (csb->which) {
	case IPXIGRP_DEB_FSM:
	    if (GETOBJ(int,2) == 0) {
		printf("\nNo IPX network number specified");
		return;
	    }
	    ddb->debug_target.nov_network = GETOBJ(int,2);
	    ddb->debug_target_set = TRUE;
	    printf("\nIPX Target enabled on AS %d for %x", as, GETOBJ(int,2));
	    break;
	case IPXIGRP_DEB_NEI:
	    ieee_copy(GETOBJ(paddr,1)->nov_addr.host,
		      ddb->debug_peer.nov_address.host);
	    ddb->debug_peer.nov_address.net = GETOBJ(paddr,1)->nov_addr.net;
	    ddb->debug_peer_set = TRUE;
	    printf("\nIPX Neighbor target enabled on AS %d for %x.%e", as,
		   ddb->debug_peer.nov_address.net,
		   ddb->debug_peer.nov_address.host);
	    break;
	}
    } else {
	switch (csb->which) {
	case IPXIGRP_DEB_FSM:
	    ddb->debug_target_set = FALSE;
	    printf("\nIPX Target disabled on AS %d", as);
	    break;
	case IPXIGRP_DEB_NEI:
	    ddb->debug_peer_set = FALSE;
	    printf("\nIPX Neighbor target disabled on AS %d", as);
	    break;
	}
    }
}

/*
 * novigrp_clear_neighbors
 *
 * Process exec command:
 * 	clear ipx eigrp neighbors [<ipx-address> | <interface>]
 */
void novigrp_clear_neighbors (parseinfo *csb)
{
    idbtype       *idb, *tidb;
    novpdbtype    *pdb;
    ddbtype       *ddb;
    igrp2_idbtype *iidb;
    dual_peertype *peer;
    ulong	  net;
    uchar host[IEEEBYTES];
    igrp2addrtype address;
    novndbtype 	  *ndb;
    novrdbtype	  *rdb;
    nidbtype	  *nidb;

    ieee_copy(GETOBJ(paddr,1)->nov_addr.host, host);
    net = GETOBJ(paddr,1)->nov_addr.net;
    idb = GETOBJ(idb,1);
    address.nov_address.net = net;
    ieee_copy(host, address.nov_address.host);

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP) continue;
	ddb = pdb->ddb;
	if (!ddb) continue;

	if (!(idb || net)) {

	    /*
	     * Take all neighbors down.	
	     */
	    for (iidb = ddb->iidbQ.qhead; iidb; 
		 iidb = iidb->next) {
		igrp2_take_nbrs_down(ddb, iidb->idb, FALSE,
				     "manually cleared");
	    }
	} else if (idb) {
	    /*
	     * Take neighbors down on a specific interface.
	     */
	    igrp2_take_nbrs_down(ddb, idb, FALSE, "manually cleared");
	} else {
	    /*
	     * Take a single neighbor down.
	     */
	    ndb = novrip_find_rt(net, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT);
	    if (ndb == NULL)
	      continue;
	    if (!(ndb->flags & NET_CONNECTED))
	      continue;
	    if ((rdb = ndb->path_list) == NULL)
	      continue;
	    if ((nidb = rdb->nidb) == NULL)
	      continue;
	    if ((tidb = nidb->idb) == NULL)
	      continue;
	    if ((peer = igrp2_find_peer(ddb, &address, tidb)) == NULL)
	      continue;
	    igrp2_take_peer_down(ddb, peer, FALSE, "manually cleared");
	}
    }
}

/*
 * novigrp_clear_event_logging
 *
 * Stop event logging.
 */
void novigrp_clear_event_logging (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP) continue;
	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	dual_clear_event_logging(ddb);
    }
}

/*
 * novigrp_clear_log
 *
 * Clear out the EIGRP event log (and restart logging if it has been
 * stopped).
 */
void novigrp_clear_log (parseinfo *csb)
{
    novpdbtype *pdb;
    ddbtype *ddb;

    /*
     * Scan through all IGRP2 pdbs.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP) continue;
	ddb = pdb->ddb;
	if (!ddb)
	    continue;
	dual_clear_log(ddb);
    }
}

/*
 * novigrp_take_neighbors_down
 * Take the neighbors down on the given inpdb, or all pdbs if inpdb is NULL
 */
void novigrp_take_neighbors_down (novpdbtype *inpdb, char *reason)
{
    novpdbtype *pdb;

    if (inpdb != NULL) {
	if (inpdb->ddb != NULL) {
	    igrp2_take_all_nbrs_down(inpdb->ddb, reason);
	}
    } else {
	for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	    if ((pdb->proctype == NET_IGRP) && (pdb->ddb != NULL)) {
		igrp2_take_all_nbrs_down(pdb->ddb, reason);
	    }
	}
    }
}

/*
 * novigrp_encap_fixup
 * set igrp2 timers to proper values after an encapsulation change
 */ 

void novigrp_encap_fixup (idbtype *swidb, int old_encap)
{
    novpdbtype *pdb;
    ddbtype *ddb;
    igrp2_idbtype *iidb;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP)
	    continue;
	if (!pdb->running)
	    continue;
	ddb = pdb->ddb;
	iidb = igrp2_find_iidb(ddb, swidb);
	if (iidb == NULL)
	    continue;
	if (!igrp2_default_timers (iidb))
	    continue;
	iidb->hello_interval = igrp2_def_helloint(swidb, ddb->linktype);
	iidb->holdtime = igrp2_def_holdint(swidb, ddb->linktype);
    }
}

/*
 * novigrp_iidb_show_detail
 *
 * Display IPX-specific IIDB information
 */
static void novigrp_iidb_show_detail (ddbtype *ddb, igrp2_idbtype *iidb)
{
    printf("\n  %d entries in SAP transmit queue", iidb->pdm_queue.count);
}

/*
 * novigrp_interface_change
 *
 * Process a change in metric on an interface.
 */
static void novigrp_interface_change (ddbtype *ddb, igrp2_idbtype *iidb)
{
    novpdbtype *pdb;

    pdb = ddb->pdb;
    if (!pdb)
	return;
    novigrp_route_adjust(pdb, iidb->idb);
}

/*
 * novigrp_ddbinit
 * Initialize the ddb data structure.
 */
static boolean novigrp_ddbinit (novpdbtype *pdb)
{
    ddbtype *ddb;
    
    ddb = malloc_named(sizeof(ddbtype), "IPX DDB");
    if (ddb == NULL) {
	return(FALSE);
    }

    dual_allocate_free_lists();
    dual_init_ddb(ddb, "IPX-EIGRP Hello", sizeof(novigrp_extdatatype),
		  sizeof(novigrp_worktype));

    ddb->pdb = pdb;
    pdb->ddb = ddb;

    ddb->name = "IPX-EIGRP";
    ddb->linktype = LINK_NOVELL;
    ddb->k1 = pdb->igrp_k1;
    ddb->k2 = pdb->igrp_k2;
    ddb->k3 = pdb->igrp_k3;
    ddb->k4 = pdb->igrp_k4;
    ddb->k5 = pdb->igrp_k5;
    ddb->max_hopcount = IGRP_MAXHOPS;
    ddb->asystem = pdb->asystem;

    /*
     * Turn on split horizon by default
     */
    ddb->def_split_horizon = TRUE;

    /* Set up transfer vectors. */

    ddb->accept_peer	= novigrp_accept_peer;
    ddb->add_item	= novigrp_add_item;
    ddb->addrcopy	= novigrp_addrcopy;
    ddb->addrmatch	= novigrp_addrmatch;
    ddb->advertise_request = novigrp_advertise_request;
    ddb->buildpacket	= NULL;
    ddb->compare_external = NULL;
    ddb->enable_process_wait = novigrp_enable_process_wait;
    ddb->exterior_check	= NULL;
    ddb->exterior_flag	= NULL;
    ddb->headerptr	= novigrp_headerptr;
    ddb->hellohook	= NULL;
    ddb->iidb_cleanup	= novigrp_iidb_cleanup;
    ddb->iidb_show_detail = novigrp_iidb_show_detail;
    ddb->input_packet	= novigrp_input_packet;
    ddb->interface_change = novigrp_interface_change;
    ddb->interface_up	= novigrp_interface_up;
    ddb->item_size	= novigrp_item_size;
    ddb->last_peer_deleted = novigrp_iidb_cleanup;
    ddb->localaddr	= novigrp_localaddr;
    ddb->mmetric_fudge	= novigrp_metric_fudge;
    ddb->ndbbucket	= novigrp_ndbbucket;
    ddb->ndb_done	= NULL;
    ddb->ndb_new	= NULL;
    ddb->netmatch	= novigrp_netmatch;
    ddb->paklen		= novigrp_paklen;
    ddb->pdm_timer_expiry = NULL;
    ddb->peerbucket	= novigrp_peerbucket;
    ddb->peershowhook	= novigrp_peershowhook;
    ddb->peerstatehook	= novigrp_peerstatehook;
    ddb->printaddr	= novigrp_printaddr;
    ddb->printnet	= novigrp_printnet;
    ddb->rtdelete	= novigrp_rtdelete;
    ddb->rtgetmetric	= novigrp_rtgetmetric;
    ddb->rtmodify       = NULL;
    ddb->rtupdate	= novigrp_rtupdate;
    ddb->sendpacket	= novigrp_write;
    ddb->transport_ready = novigrp_transport_ready;
    ddb->set_mtu        = novigrp_set_mtu;
    ddb->extract_tag    = NULL;

    /* Set up other miscellaneous stuff. */

    ddb->packet_overhead = XNSHEADERBYTES;
    ddb->routerid.nov_address.net = 0; 
    ieee_copy(default_mac_addr, ddb->routerid.nov_address.host);
    ddb->active_time = IGRP2_DEF_ACTIVE_TIME;
    mgd_timer_start(&ddb->ddb_active_timer, ONEMIN);

    enqueue(&ddbQ, ddb);
    return(TRUE);
}

/*
 * novigrp_start
 * Get novigrp revved up.
 */
novpdbtype *novigrp_start (ushort asystem)
{
    novpdbtype *pdb;

    pdb = novigrp_pdbinit(asystem);
    if (pdb) {
	if (novigrp_ddbinit(pdb)) 
	    return(pdb);
	else {
	    free(pdb);
	    pdb = NULL;
	}
    }
    return(pdb);
}

extern void novigrp_parser_init(void);

/*
 * ipxeigrp_init
 *
 * Initialize novell EIGRP support
 */
static void ipxeigrp_init (subsystype *subsys)
{
    novigrp_enabled = TRUE;	/* Give IPX a quick way of finding this */
    
    reg_add_media_change_mtu(novigrp_mtu_change, "novigrp_mtu_change");
    reg_add_novigrp_address_command(novigrp_address_command,
				    "novigrp_address_command");
    reg_add_nov_get_backup_servers(nov_get_backup_servers,
				   "nov_get_backup_servers");
    reg_add_nov_redistribute(nov_redistribute, "nov_redistribute");
    reg_add_novigrp_send_sap(novigrp_send_sap, "novigrp_send_sap");
    reg_add_novigrp_have_peer(novigrp_have_peer, "novigrp_have_peer");
    reg_add_novigrp_reload_proto_table(novigrp_reload_proto_table,
				       "novigrp_reload_proto_table");
    reg_add_novigrp_network_command(novigrp_network_command,
				    "novigrp_network_command");
    reg_add_novigrp_sap_access_list_work(novigrp_sap_access_list_work,
					 "novigrp_sap_access_list_work");
    reg_add_novigrp_route_adjust(novigrp_route_adjust, "novigrp_route_adjust");
    reg_add_novigrp_process(novigrp_process, "novigrp_process");
    reg_add_novigrp_start(novigrp_start, "novigrp_start");
    reg_add_encap_fixup(novigrp_encap_fixup, "novigrp_encap_fixup");
    reg_add_ipx_igrp2_exists_iidb(ipx_igrp2_exists_iidb,
				  "ipx_igrp2_exists_iidb");
    reg_add_ipx_igrp2_take_nbrs_down(ipx_igrp2_take_nbrs_down,
				     "ipx_igrp2_take_nbrs_down");
    access_create_callback(novigrp_sap_access_list_change, MINNOVSAPACC,
			   MAXNOVSAPACC);
    access_create_callback(novigrp_rt_access_list_change, MINNOVACC,
			   MAXENOVACC);
    novigrp_parser_init();
}
 
/*
 * IPX EIGRP subsystem header
 */

#define NOVIGRP_MAJVERSION	1
#define NOVIGRP_MINVERSION	0
#define NOVIGRP_EDITVERSION	1

SUBSYS_HEADER(ipxeigrp, NOVIGRP_MAJVERSION, NOVIGRP_MINVERSION, 
	      NOVIGRP_EDITVERSION,
	      ipxeigrp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx, eigrp", "req: ipx, eigrp");

