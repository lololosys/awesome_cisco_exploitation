/* $Id: clns_parse.c,v 3.8.12.15 1996/09/12 00:16:57 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_parse.c,v $
 *------------------------------------------------------------------
 * clns_parse.c -- configure interface CLNS parameters
 *
 * February 1989, Jim Forster
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_parse.c,v $
 * Revision 3.8.12.15  1996/09/12  00:16:57  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.8.12.14  1996/08/08  14:50:26  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.8.12.13  1996/08/07  08:57:08  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.12.12  1996/08/02  02:47:42  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.8.12.11  1996/07/26  18:14:02  tylin
 * CSCdi59808:  clns ping fails between 7000 ethernet interfaces
 *    Fix dialer i/f checking by calling is_ddr().
 *    Update driver's filter table for all ES/IS multicast addresses
 *    whenever enable/disable CLNS routing.
 * Branch: California_branch
 *
 * Revision 3.8.12.10  1996/06/18  16:54:27  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Don't do adjacency holddown if no adjacencies are present (so we don't
 * do holddown when we first start up.)
 *
 * Revision 3.8.12.9  1996/06/18  16:31:14  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Force the protocol to go to sleep long enough for all neighbor
 * adjacencies to time out when the level is changed.  While we're
 * in there, do it for NET changes as well (since these can change
 * adjacency levels).
 *
 * Revision 3.8.12.8  1996/06/17  23:29:34  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.12.7  1996/05/26  06:45:51  dkatz
 * CSCdi58737:  ISIS/NLSP debug messages can be misleading
 * Branch: California_branch
 * Issue debug messages before transmitting packets.  Add instrumentation
 * for retransmissions.
 *
 * Revision 3.8.12.6  1996/05/26  06:31:48  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.8.12.5  1996/05/22  17:03:56  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.8.12.4  1996/05/04  20:04:00  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/04/29  10:02:04  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/04/06  00:36:00  asastry
 * CSCdi53023:  ISO-IGRP should poison redistributed routes if the
 * redistribution is disabled.
 *
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/03/18  19:02:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.6.4  1996/03/16  06:30:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.6.3  1996/03/13  01:13:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.6.2  1996/03/07  08:40:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.6.1  1996/02/20  00:21:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/20  18:06:32  dkatz
 * CSCdi47916:  ISIS/NLSP LSP checksum errors
 * Improve diagnostics.  Ignore errors if the holdtime is zero
 * (workaround).
 *
 * Revision 3.7  1996/02/08  20:10:04  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.6  1996/02/07  16:10:29  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/01/29  07:26:43  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/09  21:03:14  fred
 * CSCdi43225:  ipx ping fails with autonomous switching enabled on mip
 *              need to disable fair queuing on any interface that is
 *              autonomous switched, as SP/SSE don't do fancy queuin
 *
 * Revision 3.3  1995/11/17  08:52:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:51:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/23  21:37:22  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.6  1995/08/25  16:16:58  dkatz
 * CSCdi38585:  ISIS wont send serial IIHs if L2-only
 * Turn on the level-1 IIH timer for point-to-point interfaces, since
 * there's only one level of IIH for P2P lines.
 *
 * Revision 2.5  1995/08/25  14:59:34  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.4  1995/08/25  04:49:10  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Don't redistribute static routes as connected or ISIS routes.
 * Don't advertise ISIS routes twice if redistributing connected.
 * Make sure that the route table is updated properly when
 * the router level is changed.
 *
 * Revision 2.3  1995/08/23  18:25:46  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.2  1995/08/23  18:02:52  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:17:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../iprouting/route.h"
#include "../clns/parser_defs_clns.h"
#include "../clns/clns_registry.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "registry.h"
#include "sys_registry.h"
#include "../hes/sse_registry.h"
#include "../if/priority.h"
#include "../ui/common_strings.h"

/*
 * parse_nsap
 * Convert a string of the form xx.xxxx.xxxx...xxxx.xx to an nsap
 * Return FALSE if parse fails, TRUE otherwise with "result" updated.
 *
 * We now use the new style parsing routines.
 */

boolean parse_nsap (char *ptr, NSAP_T nsap)
{
    return(get_nsap_addr(ptr, NSAP_T_to_nsap_addr(nsap), NULL));
}

/*
 * force_hellos
 * poke ourselves to send out esh's.  We arrange to send them
 * out one second from now, so that if several interfaces are
 * inited from config mem, one esh will go out with both nsaps.
 */

void force_hellos (void)
{
    idbtype *idb;
    sys_timestamp shortly;

    TIMER_START(shortly, ONESEC);
    FOR_ALL_SWIDBS(idb) {
	if (idb->clns_enabled) {
	    /* don't delay an esh */
	    mgd_timer_set_soonest(&idb->clns_hello_timer, shortly);
	    idb->clns_hello_addr = NULL;  /* make sure we multicast */
	}
    }
}

/* clns_net_nsap_cmd
 *   process the NET command
 *   "clns net <nsap>"
 *	put the  nsap into an nrib entry and insert into nrib
 *	poke esis to send esh's on all interfaces in about a second (special
 *	attention to avoid sending several esh's within milliseconds).
 *   "no clns net <nsap>"
 *	delete the nsap from the nrib
 *	free its memory
 *
 * Note: the global "NSAP" command will also be processed here for
 * 	compatibility with previous release.
 */

static void clns_net_nsap_cmd (parseinfo *csb)
{

    clns_local_nettype *net;
    clns_ddbtype       *ddb;
    clns_pdbtype       *pdb;
    NSAP_T             nsap;
    boolean	       entry_exists;
    idbtype	       *ifp;
    int                i;

    if (csb->nvgen) {
	if (csb->interface) {

	    /* 
	     * Write interface specific net command 
	     */
	    nv_write((int) csb->interface->clns_net, "%s %n", csb->nv_command,
		     csb->interface->clns_net);
	    return;
	}

	/*
	 * Write out each NSAP configured for this system, if any. Write
	 * out global "clns net" commands if there are no pdb or interface
	 * specific NETs.
	 */
	for (net = clns_local_nets.qhead; net; net = net->next) {
	    pdb = clns_find_pdb(net->net);
	    if (pdb) {
		if (!net_equ(net->net, pdb->net)) {
		    for (i = 0; i < pdb->num_areas; i++) {
			if (net_equ(net->net, pdb->area_address[i]))
			    break;
		    }
		    if (i == pdb->num_areas)
			pdb = NULL;
		}
	    }
	    FOR_ALL_SWIDBS(ifp) {
		if (net_equ(net->net, ifp->clns_net)) {
		    pdb = (clns_pdbtype *) TRUE;
		    break;
		}
	    }
	    nv_write(!pdb, "%s %n", csb->nv_command, net->net);
	}
	return;
    }

    nsap_copy(GETOBJ(paddr,1)->clns_addr, nsap);
    if (!csb->sense) {
	entry_exists = FALSE;
	ddb = nsap_lookup(nsap, MATCH_EXACT);
	if (ddb) {
	    if ((ddb->entry_type != CLNS_OURNSAP_ENTRY) &&
		(ddb->entry_type != CLNS_OURNET_ENTRY)) {
		return;
	    } else {
		entry_exists = TRUE;
	    }
	}
	if (entry_exists) {

	    /*
	     * If global, change entry type to interface-specific.
	     * If not global, remove the pointer in the idb, if
	     * user has specified correct interface.
	     */
	    if (!csb->interface) {
		if (ddb->entry_type == CLNS_OURNSAP_ENTRY)
		    ddb->entry_type = CLNS_OURNET_ENTRY;
		else {
		    printf("\n%%CLNS: NET %n could not be deleted", nsap);
		    return;
		}
	    } else {
		if (ddb->destination == csb->interface->clns_net) {
		    csb->interface->clns_net = NULL;
		} else {
		}
	    }

	    /*
	     * Check if there any more pointers to the entry.  OURNSAP
	     * indicates a global reference and counts as one.  If no
	     * more references, delete the entry.
	     */
	    if (ddb->entry_type == CLNS_OURNSAP_ENTRY) return;
	    FOR_ALL_SWIDBS(ifp)
		if (ddb->destination == ifp->clns_net) return;

	    if (!nsap_delete(nsap)) {

		/*
		 * Should never happen.
		 */
		printf("\n%%CLNS: NET %n could not be deleted", nsap);
	    
	    }
	} else {
	    printf("\n%%CLNS: NET %n could not be deleted", nsap);
	}
    } else {

	/*
	 * If NSAP is interface specific, store in IDB.
	 */
	if (csb->interface) {
	    ddb = nsap_lookup(nsap, MATCH_EXACT);
	    if (ddb) {
		clns_route_update(nsap, NULL, csb->interface, ddb->entry_type,
				  ISO_STATIC, ISO_STATIC_DIST, 0,
				  timer_no_time, 0, NULL);
	    } else {
		clns_route_update(nsap, NULL, csb->interface, 
				  CLNS_OURNET_ENTRY, ISO_STATIC, 
				  ISO_STATIC_DIST, 0, timer_no_time, 0, NULL);
	    }
	    ddb = nsap_lookup(nsap, MATCH_EXACT);
	    if (ddb) {
		csb->interface->clns_net = ddb->destination;
	    }
	} else {
	    /*
	     * Insert NSAP in routing table, indicate it is global.
	     */
	    clns_route_update(nsap, NULL, NULL, CLNS_OURNSAP_ENTRY, 
			      ISO_STATIC, ISO_STATIC_DIST, 0,
			      timer_no_time, 0, NULL);
	}

	/*
	 * Poke ourselves to send out esh's.  We arrange to send them
	 * out one second from now, so that if several interfaces are
	 * inited from config mem, one esh will go out with both nsaps.
	 */
	force_hellos();
    }
}

/*
 * clns_poison_prefix_route
 *
 * Send poisoned information for a prefix route when the route is withdrawn.
 */
static void clns_poison_prefix_route (NSAP_T prefix, clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;
    idbtype      *idb;
    clns_ddbtype *ddb;

    ddb = nsap_lookup(prefix, MATCH_EXACT);

    /*
     * Send out poison info (out all relevant interfaces)
     * only if there is one path for this static route.
     */
    if (ddb && (ddb->routecount == 1)) {
        for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
 	    idb = idbqueue->idb;
		if (idb && (idb->clns_enabled) && interface_up(idb))
	            iso_igrp_sendupdate(pdb, idb, TRUE, NULL, ddb);  
	}
    }
}

/*
 * clns_route_cmd
 *
 * There are two forms of the "clns route" command. One form accepts
 * an NSAP for the next hop address.
 *
 *   [no] clns route <nsap-prefix> [<next-hop-nsap> | discard]
 *
 * The other form accepts an (interface, SNPA) pair.
 *
 *   [no] clns route <nsap-prefix> <interface> <unit> <snpa>
 *
 * Both forms will insert the nsap-prefix into the CLNS prefix routing table
 * as well as the adjacency table. Inserting in the adjacenecy table allows
 * routing protocols to redistribute the static route.
 *
 * In the first form, the <nsap-prefix> will be stored in the ddb, and
 * an rdb is built with the first_hop == <next-hop-nsap>. This is done so
 * the first_hop value can be used to index into the adjacency table where
 * all SNPA related information resides. If there is no mapping between
 * the first_hop NSAP (the system id portion) and an SNPA (either statically 
 * configured or dynamically determined), the route is invalid (cannot be used).
 *
 * In the second form, there is no next hop NSAP, so the <nsap-prefix> is
 * used as the first_hop in the rdb. And an adjacency entry is created 
 * (by this function), using first_hop as an index, and the supplied SNPA and
 * interface is stored within.
 */

static void clns_route_cmd (parseinfo *csb)
{
    clns_rdbtype *rdb;
    clns_pdbtype *pdb;
    clns_adjtype *adj;
    idbtype      *idb;
    NSAP_T       next_hop_nsap;
    NSAP_T       nsap_prefix;
    hwaddrtype   snpa;
    int          entry_type;

    idb = GETOBJ(idb,1);
    entry_type = CLNS_ROUTE_ENTRY;
    nsap_copy(GETOBJ(paddr,1)->clns_addr, nsap_prefix);

    switch (GETOBJ(int,10)) {

	case CLNS_ROUTE_DISCARD:
	    entry_type = CLNS_DISCARD_ENTRY;
	    clear_nsap(next_hop_nsap);
	    break;

	case CLNS_ROUTE_NAME:
	    nsap_copy(GETOBJ(paddr,2)->clns_addr, next_hop_nsap);
	    if (nsap_equ(nsap_prefix, next_hop_nsap)) {
		printf("\n%%Next hop NSAP cannot be the same as route "
		       "destination");
		return;
	    }
	    break;

	 case CLNS_ROUTE_INTERFACE:
	    /*
	     * clns route <nsap-prefix> <interface> <unit> <snpa>
	     */
	    nsap_copy(nsap_prefix, next_hop_nsap);
	    if (!is_p2p(GETOBJ(idb,1))) {
		bcopy((char *)GETOBJ(hwaddr,1), (char *)&snpa,
		       sizeof(hwaddrtype));
	    } else {
		snpa_clear(&snpa);
	    }
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }

    /*
     * Add/delete static routes to clns_staticQ.
     */
    clns_staticQ_update (nsap_prefix, next_hop_nsap, idb, &snpa, csb->sense);

    if (!csb->sense) {
	clns_invalidate_cache("clns_route_cmd", TRUE);

	/*
	 * If we're withdrawing a static route, make sure ISO-IGRP is made
	 * aware of it; we do this by sending out a flash update for just
	 * this route.
	 */
	for (pdb = (clns_pdbtype *)clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    /* 
	     * Worry about this only for an ISO-IGRP L2 pdb and only if
	     * it's redistributing static.
	     */
	    if ((pdb->typeflag & ISO_IGRPMASK) && (pdb->level & CLNS_LEVEL2) &&
		(pdb->redistallowed & ISO_STATIC)) {
	      clns_poison_prefix_route(nsap_prefix, pdb);
	    }
	}

	/*
	 * Delete from routing table.
	 */
	if (!nsap_delete_rdb(nsap_prefix, next_hop_nsap, idb)) {
	    printf("\n%%CLNS: route %n could not be deleted from routing table",
		   nsap_prefix);
	}

	/*
	 * Delete from adjacency table.
	 */
	adj = find_adjacency(clns_adjacency_db, nsap_prefix, FALSE, idb, 0);
	if (adj) {
	    delete_adjacency(clns_adjacency_db, adj);
	}

	/*
	 * Tell IS-IS about it.
	 */
	pdb = find_isis_pdb();
	if (pdb) {
	    isis_mark_L2_changed(pdb);
	}
	return;
    }

    /*
     * Add into routing table.
     */
    rdb = clns_route_update(nsap_prefix, next_hop_nsap, idb,
			    entry_type, ISO_STATIC, ISO_STATIC_DIST, 
			    0, timer_no_time, 0,
			    *clns_get_local_npai(NULL));

    /*
     * Add into adjacency table.
     */
    adj = add_adjacency(clns_adjacency_db, nsap_prefix, idb, NULL, -1,
			idb->clns_cluster_alias);
    if (adj) {
	adj->protocol_source |= ISO_STATIC;
 	adj->state = CLNS_ADJ_UP_STATE;
	adj->adjacencyType = CLNS_PREFIX_ADJ_TYPE;
	adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
	if (idb) {
	    adj->snpa = snpa;
	}

	pdb = find_isis_pdb();
	if (pdb) {
            /*
	     * Conditionally tell IS-IS about static route.
	     *
	     * This case handles a next-hop NET or a non-discard route.
             */
	    if (!idb && entry_type != CLNS_DISCARD_ENTRY) {
                adj->state = CLNS_ADJ_FAILED_STATE;
                adj = find_adjacency(clns_adjacency_db, next_hop_nsap,
				     TRUE, idb, 0);
                if (adj) clns_adjust_static_routes(adj, idb, TRUE, TRUE);
	    } else {
		
		/*
		 * This handles interface static routes and static discard
		 * entries.
		 */
		adj->state = (idb && !interface_up(idb)) ?
		    CLNS_ADJ_FAILED_STATE : CLNS_ADJ_UP_STATE;
                isis_mark_L2_changed(pdb);
            }
	}
    }
}

/*
 * clns_net_cmd
 *
 * Process "net" router subcommand.
 *
 */
void clns_net_cmd (parseinfo *csb)
{
    clns_pdbtype *pdb, *pdb2;
    idbqueuetype	 *idb;
    NSAP_T       net;
    int          i, n;
    ushort       checksum;

    pdb = (clns_pdbtype *) csb->protocol;
    if (csb->nvgen) {
	/* Don't write out the ISO-IGRP level-2 pdb or static routing. */
	if ((pdb->typeflag & ISO_IGRPMASK) && 
	    pdb->level == CLNS_LEVEL2) return;
	if (pdb->typeflag == ISO_STATIC) return;

	if (pdb->typeflag & ISO_ISISMASK) {
	    for (n = 0; n < pdb->num_areas; n++) {
		nv_write(TRUE, "%s %n", csb->nv_command,
			 pdb->area_address[n]);
	    }
	} else {
	    nv_write(pdb->net[0], "%s %n", csb->nv_command, pdb->net);
	}

	return;
    }


    if (pdb->family != PDB_OSI) {
	printf("\n%%net command only valid for OSI routing protocols");
	return;
    }

    nsap_copy(GETOBJ(paddr,1)->clns_addr, net);

    /*
     * See if NET exists.
     */
    for (i = 0; i < pdb->num_areas; i++) {
	if (nsap_match(net, pdb->area_address[i])) break;
    }
    if (i == pdb->num_areas) {
	/* Deleting non-existent entry. */
	if (!csb->sense) return;
    } else {
	/* Adding existent entry. */
	if (csb->sense) return;
    }
    
    /*
     * Doing a "no net <net value>" command.
     */
    if (!csb->sense) {
	pdb->num_areas--;
	nsap_copy(pdb->area_address[pdb->num_areas], 
		   pdb->area_address[i]);
	pdb->area_address[pdb->num_areas][0] = 0;

	/*
	 * If deleting last area address, clear out pdb->net.  Otherwise,
	 * copy the first area address into pdb->net.
	 */
	if (!pdb->num_areas) {
	    pdb->net[0] = 0;
	    clns_set_pdb_running(pdb, FALSE);
	} else {
	    nsap_copy(pdb->area_address[0], pdb->net);
	}
	clear_clns_neighbors(CLEAR_NBRS_ALL);
	nsap_delete(net);
	isis_mark_L1_changed(pdb);
	isis_mark_L2_changed(pdb);
	/*
	 * Inform TARP of the new NET
	 */
	reg_invoke_tarp_start_type4_timer();

	/*
	 * If this is ISIS :
	 * If any other area address present, update the sytemid and 
	 * re-initialize the interfaces to reflect the new sytemid.
	 * Rebuild the L1 LSP to reflect the removed area address and 
	 * rerun dijkstra to tell the world.  We also clear all adjacencies
	 * and hold down the protocol to make sure that all our old
	 * adjacencies expire.
	 */
	if (pdb == find_isis_pdb()) {
	    if (pdb->num_areas) {
		station_copy(get_station(pdb->net), pdb->systemId);
		checksum = isis_compute_checksum(pdb->systemId, 
						 STATION_LENGTH, NULL, 0, 0);
		PUTSHORT(pdb->systemId_checksum, checksum);

		for (idb = pdb->idbqueue; idb; idb = idb->next) {
		    isis_reset_idb_params(pdb, idb->idb, FALSE);
		    isis_start_interface(pdb, idb->idb);
		}
	    }
	    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_NEWSYSID, NULL,
			     TRUE);
	    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_NEWSYSID, NULL,
			     TRUE);
	    isis_adjacency_holddown(pdb);
	    clear_clns_neighbors(CLEAR_NBRS_ALL);
	}
	return;

    } else {
	/*
	* Doing a "net <net value>" command.
	*/
	if (pdb->num_areas == MAX_AREA_ADDRESSES) {
	    printf("\n%%The maximum allowed addresses already configured");
	    return;
	}
		
	/*
	 * Validate minimum length NET.
	 * IS-IS only, as ISO-IGRP does its own net checking.
	 */
	if (pdb->typeflag & ISO_ISISMASK) {
	    if (net[0] < CLNS_MIN_REASONABLE_NSAP_LENGTH) {
		printf("\n%%IS-IS: Invalid NET length");
		return;
	    }
	}

	if (!pdb->net[0]) {
	    /*
	     * This is the first NSAP configured for this protocol.
	     * IS-IS can have multiple NSAPs configured (though we only
	     * look at the areaID of secondary NSAPs, not the systemID).
	     * Now that we know our systemID and at least one area ID,
	     * start processes.
	     */
	    nsap_copy(net, pdb->net);
	    if (pdb->typeflag & ISO_ISISMASK) {
		station_copy(get_station(net), pdb->systemId);
		/* Checksum system id. */
		checksum = isis_compute_checksum(pdb->systemId, 
						 STATION_LENGTH, NULL, 0, 0);
		PUTSHORT(pdb->systemId_checksum, checksum);

		/*
		 * Start IS-IS processes.
		 */
		clns_add_isis_router(pdb);
		clns_set_pdb_running(pdb, TRUE);

		/*
		 * Have NET for first time. Initialize each interface
		 * IS-IS is configured over.
		 */
		for (idb = pdb->idbqueue; idb; idb = idb->next) {
		    isis_start_interface(pdb, idb->idb);
		}
		
		/*
		 * Inform TARP of the new NET
		 */
		reg_invoke_tarp_start_type4_timer();
	    } else if (pdb->typeflag & ISO_IGRPMASK) {
		if (!clns_add_router(pdb->pname, net, TRUE, ISO_IGRPMASK)) {
		    pdb->net[0] = 0;
		    return;
		}
		clns_set_pdb_running(pdb, TRUE);

		/*
		 * Copy interfaces from all level-1 pdbs in domain to
		 * level-2 domain pdb.
		 */
		for (pdb2 = clns_protoQ.qhead; pdb2; pdb2 = pdb2->next) {
		    if (!clns_domain_match(pdb->domain_length, pdb->domain,
			   pdb2->domain_length, pdb2->domain)) continue;
		    for (idb = pdb2->idbqueue; idb; idb = idb->next) {
			clns_add_intfc_router(pdb->pname, idb->idb, 
					      pdb->typeflag, CLNS_LEVEL2);
		    }
		}
	    }
	}
	    
	/* 
	 * IS-IS only supports multiple areas. 
	 */
	if (pdb->typeflag & ISO_ISISMASK) {
	    if (pdb->num_areas == MAX_AREA_ADDRESSES) {
		printf("\n%%CLNS: Maximum number of area addresses exceeded");
		return;
	    }
	    nsap_copy(net, pdb->area_address[pdb->num_areas]);
	    pdb->num_areas++;
	    pdb->num_active_areas = pdb->num_areas;
	    for (i=0; i<pdb->num_areas; i++) {
		area_copy(pdb->area_address[i], pdb->active_area_address[i]);
	    }
	    clns_route_update(net, NULL, NULL, CLNS_OURNET_ENTRY, ISO_STATIC,
			      ISO_STATIC_DIST, 0, timer_no_time, 0, NULL);
	    isis_mark_L1_changed(pdb);
	    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_NEWAREA, NULL,
			     TRUE);
	    isis_mark_L2_changed(pdb);
	}
    }

    /*
     * Check if this pdb is redistributed into another pdb. If so, now 
     * there is an NET to add to the routing table.
     */
    for (pdb2 = clns_protoQ.qhead; pdb2; pdb2 = pdb2->next) {
	if (pdb2 == pdb) continue;
	if (pdb2->redistallowed & pdb->typeflag) {
	    clns_redistribute(pdb2, pdb, csb->sense, NULL);
	}
    }
}

/*
 * clns_istype_cmd
 *
 * Process "is-type" router subcommand.
 */
void clns_istype_cmd (parseinfo *csb)
{
    clns_pdbtype *pdb;
    int          old_level;
    
    pdb = (clns_pdbtype *) csb->protocol;

    if (csb->nvgen) {
	if (pdb->typeflag & ISO_ISISMASK) {
	    nv_write((pdb->level != ISIS_CIRCUIT_L1L2) &&
		     (pdb->level == GETOBJ(int,10)), csb->nv_command);
	}
	return;
    }

    old_level = pdb->level;

    if (pdb->typeflag & ISO_ISISMASK) {
	if (csb->sense) {
	    pdb->level = GETOBJ(int,10);
	}else{
	    pdb->level = ISIS_CIRCUIT_L1L2;
	}
    }else{
	printf("\nis-type only valid for routing ISIS");
	return;
    }

    /*
     * Purge database if changing levels.
     * This applies going from level-1-2    to level-1 or
     *                         level-2-only to level-1
     */
    if ((old_level & ISIS_CIRCUIT_L2) && !(pdb->level & ISIS_CIRCUIT_L2)) {
	isis_clear_database(pdb, pdb->level2_LSPs);
	pdb->level2_LSPs = NULL;
	pdb->current_L2_version++;
	pdb->num_L2_routes = 0;
	pdb->redistributed_L2_routes = 0;
 	if (pdb->ip_pdb) age_all_routes(pdb->ip_pdb);
	isis_mark_L1_changed(pdb);
	isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_NEWLEVEL, NULL, TRUE);
	isis_adjacency_holddown(pdb);
	clear_clns_neighbors(CLEAR_NBRS_ALL);
    }

    /*
     * This applies going from level-1-2 to level-2-only
     *                         level-1   to level-2-only
     */
    if ((old_level & ISIS_CIRCUIT_L1) && !(pdb->level & ISIS_CIRCUIT_L1)) {
	isis_clear_database(pdb, pdb->level1_LSPs);
	pdb->level1_LSPs = NULL;
	pdb->current_L1_version++;
 	if (pdb->ip_pdb) {
	    age_all_routes(pdb->ip_pdb);
	}
	isis_mark_L2_changed(pdb);
	pdb->L1_routes_changed = TRUE;
	isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_NEWLEVEL, NULL, TRUE);
	isis_adjacency_holddown(pdb);
	clear_clns_neighbors(CLEAR_NBRS_ALL);
    }
}

/* clns_routing_cmd
 *   "[no] clns routing"
 *   process the routing command.  This command enables operation as 
 *   as IS. When routing is enabled, turn bridging OFF. When routing
 *   is disabled, turn bridging ON.
 *
 *   "no clns routing" should delete all routing pdbs and clean up
 *   all of the data structures.
 */
static void clns_routing_cmd (parseinfo *csb)
{
    NSAP_T *net;
    clns_pdbtype *pdb;
    idbtype *idb;
    hwidbtype *hwidb;
    int bucket;
    clns_adjtype *next, *adj;

    /* 
     * There is no NVGEN processing here.  It's handled in clns_commands.
     */

    if (csb->sense) {
	/*
 	 * check if there is a <net> - if so, process it
	 */
	if (GETOBJ(paddr,1)->type == ADDR_CLNS) {
	    net = (NSAP_T *) &(GETOBJ(paddr,1)->clns_addr);
	    clns_add_net(*net);
	}
	if (!clns_running) {
	    clns_running = TRUE;
	}
	FOR_ALL_SWIDBS(idb)
	    clns_rxtypes_update(idb);
	clns_router_running = TRUE;
	force_hellos();		/* send out hellos */

	FOR_ALL_HWIDBS(hwidb) {
	    clns_fastsetup(hwidb);
	}
    }else{
	FOR_ALL_SWIDBS(idb)
	    clns_rxtypes_update(idb);
	clns_router_running = FALSE;
	pdb = clns_protoQ.qhead;
	while (pdb) {
	    if (!(pdb->typeflag & ISO_ISISMASK) && 
		(pdb->level & CLNS_LEVEL1)) {
		clns_delete_router(pdb->pname);
		pdb = clns_protoQ.qhead;
	    }else{
		pdb = pdb->next;
	    }
	}
	if (clns_myNET[0] == 0) {
	    /* we are not an end system */
	    clns_running = FALSE;
	    FOR_ALL_SWIDBS(idb) {
		if (idb->clns_enabled)
		    clns_itf_disable(idb);
	    }
	    FOR_ALL_HWIDBS(hwidb) {
		clns_fastsetup(hwidb);
	    }
	}
	FOR_ALL_HWIDBS(hwidb) {
	    hwidb->clns_routecache &= ~CLNS_SSE_ROUTECACHE;
	}
	for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
	    for (adj = clns_adjacency_db[bucket]; adj; adj = next) {
		next = adj->next;
		delete_adjacency(clns_adjacency_db, adj);
	    }
	}
    }
}

/*
 * clns_xs_neighbor_cmd
 * parse the interface commands:
 *	[no] clns es-neighbor <nsap> <snpa>
 * 	[no] clns is-neighbor <nsap> <snpa>
 */
static void clns_xs_neighbor_cmd (parseinfo *csb)
{
    idbtype         *idb;
    clns_adjtype    *adj;
    clns_rdbtype    *rdb;
    clns_pdbtype    *isis_pdb, *pdb;
    uchar           *station_id;
    char            *buffer;
    NSAP_T          ns;
    NSAP_T          key;
    hwaddrtype      snpa;
    addrtype        addrs;

    idb = csb->interface;

    if (csb->nvgen) {
	int bucket;
	NSAP_T	nsap;

	adj = NULL;
	while ((adj = next_neighbor(clns_adjacency_db, idb, 0, adj,
				    &bucket))) {
	    if (!(adj->protocol_source & ISO_STATIC)) {
		continue;
	    }
	    /*
	     * Don't write prefix neighbors.
	     */
	    if (adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE) {
		continue;
	    }
	    
	    switch (csb->which) {
	      case CLNS_ES_NEIGHBOR:
		if (adj->adjacencyType != CLNS_ES_ADJ_TYPE) {
		    continue;
		}
		nv_write(TRUE, csb->nv_command);
		break;
	      case CLNS_IS_NEIGHBOR:
		if ((adj->adjacencyType != CLNS_IS_ADJ_TYPE) &&
		    (adj->adjacencyType != CLNS_L1_IS_ADJ_TYPE) &&
		    (adj->adjacencyType != CLNS_L2_IS_ADJ_TYPE)){
		    continue;
		}
		nv_write(TRUE, csb->nv_command);
		break;
	      default:
		continue;
	    }
	    if (adj->num_areas) {
		BUILD_NSAP(adj, nsap);
	    }else{
		nsap_copy(adj->key, nsap);
	    }
	    
	    /*
	     * Static neighbors need SNPA in non-P2P interface and on
	     * subinterfaces for X25 and SMDS.
	     */
	    nv_add(TRUE, " %n %s", nsap,
		   is_p2p(idb) ? " " : snpa_string(&adj->snpa));
	}
	return;
    }

    bcopy((char *)GETOBJ(paddr,1)->clns_addr, (uchar *)ns,
	  GETOBJ(paddr,1)->length+1);
    memset(&addrs, 0, sizeof(addrs));
    memset(&snpa, 0, sizeof(hwaddrtype));
    
    /* 
     * Smallest NSAP allowed is an AFI, station id, and N-selector
     * (8 bytes).
     */
    if (ns[0] < (AFI_LENGTH + STATION_LENGTH + SELECTOR_LENGTH)) {
	printf("\nNSAP address too short, must include N-selector");
	return;
    }
    station_id = get_station(ns);
    station_copy(station_id, &key[1]);
    key[0] = STATION_LENGTH;

    /*
     * Deleting a neighbor.
     */
    if (!csb->sense) {
	adj = find_adjacency(clns_adjacency_db, key, FALSE, idb, 0);
	if (!adj) {
	    return;
	}
	if (adj->protocol_source & ISO_STATIC) {
	    delete_adjacency(clns_adjacency_db, adj);
	}

	return;
    }

    /*
     * Get data link address information.
     */
    if (is_p2p(idb)) {
	snpa.type = idb->hwptr->enctype;
    } else {
	bcopy((char *)GETOBJ(hwaddr,1), (char *)&snpa, sizeof(hwaddrtype));
    }

    if (!reg_invoke_clns_parse_addr_map(idb->hwptr->enctype, csb, &snpa, 
					&addrs)) {
	/*
	 *  failed
	 */
	return;
    }

    adj = add_adjacency(clns_adjacency_db, key, idb, &snpa, -1, 
			idb->clns_cluster_alias);

    /*
     * Fill in the rest of the fields in the adjacency.
     */
    isis_pdb = find_isis_pdb();
    adj->protocol_source = ISO_STATIC;
    adj->state = CLNS_ADJ_UP_STATE;
    switch (csb->which) {
      case CLNS_ES_NEIGHBOR:
	adj->adjacencyType = CLNS_ES_ADJ_TYPE;
	if (isis_pdb && itf_in_pdb(idb, isis_pdb)) {
	    isis_mark_L1_changed(isis_pdb);
	} else {
	    isis_pdb = NULL;
	}
	break;
      case CLNS_IS_NEIGHBOR:
	adj->adjacencyType = CLNS_IS_ADJ_TYPE;
	if (isis_pdb && itf_in_pdb(idb, isis_pdb)) {
	    isis_mark_L1_changed(isis_pdb);
	    isis_mark_L2_changed(isis_pdb);
	} else {
	    isis_pdb = NULL;
	}
	break;
    }
    adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;

    pdb = clns_find_pdb(ns);
    if (pdb && (pdb->typeflag & ISO_IGRPMASK) && itf_in_pdb(adj->idb, pdb)) {

	/*
	 * If NSAP is in one of our areas, report system id. Otherwise,
	 * report area address.
	 */
	if (pdb->level == CLNS_LEVEL1) {
	    rdb = iso_igrp_destination_update(pdb, CLNS_STATION_TYPE, adj->key,
		      idb, adj->key, &adj->snpa, 
		      iso_igrp_compute_metric(pdb, idb), ISO_STATIC, 
		      CLNS_ROUTE_ENTRY, 0);
	} else {
	    key[0] = AREA_LENGTH;
	    buffer = (char *) get_area(ns);
	    key[1] = *buffer++;
	    key[2] = *buffer;
	    rdb = iso_igrp_destination_update(pdb, CLNS_AREA_TYPE, key,
		      idb, adj->key, &adj->snpa, 
		      iso_igrp_compute_metric(pdb, idb), ISO_STATIC, 
		      CLNS_ROUTE_ENTRY, 0);
	}
	iso_igrp_setup_metric(rdb, idb);
    }

    /*
     * Fill in area address.
     */
    ns[0] -= (STATION_LENGTH + SELECTOR_LENGTH);
    add_area_to_adj(ns, adj);

    if (isis_pdb) {
	clns_adjust_static_routes (adj, NULL, TRUE, TRUE);
    }
}

/*
 * clns_intfc_enable_cmd
 * process the clns interface enable disable command
 * interface clns enable:
 * 	must have specified NET already
 *	clns_intfc_enabled flag in the idb set to true
 * interface clns disable:
 *	set a flag in the idb to false
 */
static void clns_intfc_enable_cmd (parseinfo *csb)
{
    idbtype *idb;
    clns_pdbtype *pdb;
    boolean clns_routing_on_interface = FALSE, ip_routing_on_interface = FALSE;

    idb = csb->interface;
    if (csb->sense) {
	if (!idb->clns_enabled) {
	    clns_itf_enable(idb);
	}
	idb->clns_level = CLNS_LEVEL1 | CLNS_LEVEL2;

    } else if (idb->clns_enabled) {
	/*
	 * Search thru' PDBs to see if this interface has a dynamic protocol
	 * enabled.
	 */
        for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    if (itf_in_pdb (idb, pdb)) {
		if (pdb->typeflag & ISO_IGRPMASK) {
		    clns_routing_on_interface = TRUE;
		    break;
		} else {
		    /* ISIS process */
		    if (isis_checkqueue(&pdb->osi_idbq, idb)) {
		        clns_routing_on_interface = TRUE;
			break;
		    } else
		        ip_routing_on_interface = TRUE;
		}
	    }
	}

        /*
	 * Disable only if no dynamic routing protocols are enabled for this
	 * interface.
	 */
	if (!clns_routing_on_interface && !ip_routing_on_interface) {
	    clns_itf_disable(idb);
 
 	    /* invalidate the cache just in case we have entries
 	     * which point at this interface.
 	     */
	    clns_invalidate_cache("clns_intfc_enable_cmd", TRUE);

	    /*
	     * Delete all adjacencies on this interface.
	     */
	    clns_remove_idb_depend(idb);
	} else if (!clns_routing_on_interface && ip_routing_on_interface)
	    /* Only "ip router isis" is configured, clear clns_level to 
	     * avoid sending ISH over broadcast network
	     */
	    idb->clns_level = 0;
    }
    clns_fastsetup(idb->hwptr);
}

/*
 * clnshost_command
 *
 *    host foo <net> ...
 *
 */
static const char badhost[] ="\nBad clns host command format";

static void clns_host_command (parseinfo *csb)
{
    nametype *ptr;
    int i, size;
    NSAP_T nsap;
    addrtype *addr;

    /*
     * Check for valid hostname
     */
    if (name_verify(GETOBJ(string,1)) != OK) {
	printf("%%CLNS: Bad hostname format");
	return;
    }
    ptr = name_cache_lookup(GETOBJ(string,1), NULL, NAM_CLNS);
    if (ptr || !csb->sense) {
	name_delete(GETOBJ(string,1),NAM_CLNS);
	if (!csb->sense) {
	    return;
	}
    }
    size = sizeof(nametype) + strlen(GETOBJ(string,1));
    ptr = malloc(size);
    if (!ptr) {
	return;
    }
    strcpy(ptr->name, GETOBJ(string,1));
    GET_TIMESTAMP(ptr->lasttime);
    ptr->flags = NAM_PERMANENT + NAM_CLNS + NAM_ADDRVALID;

    i = 0;
    clear_nsap(nsap);
    addr = GETOBJ(paddr,1);
    while ((addr->type == ADDR_CLNS) &&
	   (i < MAXCLNSADDRESSES)) {
	ptr->v.clns.adr[i].length = ADDRLEN_CLNS;
	ptr->v.clns.adr[i].type = ADDR_CLNS;
	nsap_copy(addr->clns_addr, ptr->v.clns.adr[i].clns_addr);
	i++;
	switch (i) {
	  case 1:	addr = GETOBJ(paddr,2); break;
	  case 2:	addr = GETOBJ(paddr,3); break;
	  case 3:	addr = GETOBJ(paddr,4); break;
	  case 4:	addr = GETOBJ(paddr,5); break;
	  case 5:	addr = GETOBJ(paddr,6); break;
	  case 6:	addr = GETOBJ(paddr,7); break;
	  case 7:	addr = GETOBJ(paddr,8); break;
	  case 8:	i = MAXCLNSADDRESSES; break;
	}
    }
    if (ptr->v.clns.adr[0].length == 0) {
	free(ptr);
	printf(badhost);
	printf("\n%%CLNS: No addresses specified");
    }else{
	enqueue(&nameQ, ptr);
    }
}

/*
 * clns_int_process_name
 * Get the interface process name
 */
static char *clns_int_process_name(clns_pdbtype *pdb, idbtype *idb)
{
    idbqueuetype *idbqueue;
    if (pdb->level != CLNS_LEVEL2)
	return (pdb->pname);
    
    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next)
	if (idbqueue->idb == idb) 
	    return(idbqueue->pname);
    
    /* This should never happen. But as a precaution... */
    return (pdb->pname + L2PREFIX_LENGTH);
}

/*
 * clns_router_intfc_cmd
 * The interface version of the clns router command.
 * Syntax:
 *    [no] clns router iso-igrp <router-tag> [level 2]
 *    [no] clns router isis <router-tag>
 */
static void clns_router_intfc_cmd (parseinfo *csb)
{

    idbtype      *idb;
    clns_pdbtype *pdb;
    char         *name;
    idbqueuetype *idbq;
    char         router_name[TOKLEN];
    boolean      dynamic;
    int          protocol, level;
    
    idb = csb->interface;
    protocol = 0;		/* shut gcc up */

    
    if (csb->nvgen) {
	boolean no_pdb;

	no_pdb = TRUE;
	for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    if (itf_in_pdb(idb, pdb)) {
		no_pdb = FALSE;
		if (pdb->typeflag & ISO_IGRPMASK) {
		    if (pdb->level == CLNS_LEVEL2 &&
			idb->clns_level != CLNS_LEVEL2) continue;

		    nv_write(TRUE, "%s iso-igrp %s", csb->nv_command,
			     clns_int_process_name(pdb, idb));

		    if (idb->clns_level == CLNS_LEVEL2) {
			nv_add(TRUE, " level 2");
		    }
		} else if (pdb->typeflag & ISO_ISISMASK) {
		    if (isis_checkqueue(&pdb->osi_idbq, idb)) {
			nv_write(TRUE, "%s isis %s", csb->nv_command, 
				 pdb->pname);
		    }
		}
	    }
	}
	return;
    }

    dynamic = TRUE;
    switch (GETOBJ(int,1)) {
      case PROTOCOL_ISIS:
	protocol = ISO_ISISMASK;
	break;
      case PROTOCOL_ISO_IGRP:
	protocol = ISO_IGRPMASK;
	break;
    }
    if (GETOBJ(string,1)[0] != '\0') {
	strcpy(router_name, GETOBJ(string,1));
	name = router_name;
    }else{
	router_name[0] = '\b';
	router_name[1] = '\0';
	name = NULL;
    }

    level = CLNS_LEVEL1 | CLNS_LEVEL2;
    if ((protocol != ISO_ISISMASK) && (GETOBJ(int,2) != 0)) {
	/*
	 * Check if the user specified a priority or a level designator
	 */
	level = GETOBJ(int,2);
    }
    if (csb->sense) {
	idb->clns_level = level;
	clns_add_intfc_router(name, idb, protocol, level);
 	pdb = clns_find_router_name(name);
    } else {
 	/*
	 * see if this idb is attached to a level 2 router
	 */
 	for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
 	    if (itf_in_pdb(idb, pdb)) break;
 	}
 	if (!pdb || !(pdb->typeflag & protocol)) {
	    printf("\n%%CLNS: could not remove router %s from interface %s",
		   router_name, idb->namestring);
	    return;
	}
 	if (protocol != ISO_ISISMASK) {
 	    if (!clns_delete_intfc_router(pdb, idb)) {
	    printf("\n%%CLNS: could not remove router %s from interface %s",
		   router_name, idb->namestring);
		return;
	    }
  	}
    }

    /*
     * Identify which interfaces are running IS-IS for OSI.
     */
    if (pdb && protocol == ISO_ISISMASK) {
	idbq = isis_checkqueue(&pdb->osi_idbq, idb);
	if (idbq) {
	    if (!csb->sense) {
		unqueue(&pdb->osi_idbq, idbq);
		free(idbq);
		if (!pdb->osi_idbq.count) {
		    pdb->isis_mode &= ~ISIS_OSI_MODE;
		}
		clns_delete_intfc_router(pdb, idb);
	    }
	} else {
	    if (csb->sense) {
		idbq = malloc(sizeof(idbqueuetype));
		if (idbq) {
		    idbq->idb = idb;
		    enqueue(&pdb->osi_idbq, idbq);
		    if (pdb->osi_idbq.count) {
			pdb->isis_mode |= ISIS_OSI_MODE;
		    }
		}
	    }
	}
    }
}

/*
 * clns_intfc_command
 *
 * Process CLNS interface commands.
 */
void clns_intfc_command (parseinfo *csb)
{
    int bits;
    idbtype *idb;
    hwidbtype *hwidb;

    idb = csb->interface;
    hwidb = idb->hwptr;
    bits = GETOBJ(int,1);

    if (csb->nvgen) {
	clns_pdbtype *pdb;
	boolean       print_clns_enable;

	switch (csb->which) {
	case CLNS_ROUTEC:
	    if (bits) {
		if (hwidb->clns_routecache & bits)
		    nv_write(TRUE, csb->nv_command);
	    } else
		if (!hwidb->clns_routecache)
		    nv_write(TRUE, "no %s", csb->nv_command);
	    break;

	case CLNS_CLUSTER_CMD:
	    nv_write(idb->clns_cluster_alias, csb->nv_command);
	    break;

	case CLNS_CHECKSUM_CMD:
	    nv_write(! idb->clns_checksum, "no %s", csb->nv_command);
	    break;

	case CLNS_CNGST_THRESH:
	    nv_write(idb->hwptr->clns_congestion != CLNS_DEF_CONGESTION,
		     "%s %d", csb->nv_command, idb->hwptr->clns_congestion);
	    break;

	case CLNS_ENABLE:
	    print_clns_enable = TRUE;
	    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
		if (itf_in_pdb(idb, pdb)) {
		    if (pdb->typeflag & ISO_IGRPMASK) {
		        print_clns_enable = FALSE;
		        break;
		    } else if (isis_checkqueue(&pdb->osi_idbq, idb)) {
		        print_clns_enable = FALSE;
		        break;
		    }
		}
	    }
	    nv_write(idb->clns_enabled && print_clns_enable && 
		     idb->clns_level, csb->nv_command);
	    break;

	case CLNS_ERPDU_INTRVL:
	    nv_write(idb->clns_er_interval != CLNS_DEF_ERPDU_INTERVAL,
		     "%s %d", csb->nv_command, idb->clns_er_interval);
	    break;

	case CLNS_ES_NEIGHBOR:
	case CLNS_IS_NEIGHBOR:
	    csb->which = GETOBJ(int,1);
	    clns_xs_neighbor_cmd(csb);
	    break;

	case CLNS_MTU:
	    nv_write(idb->clns_mtu != def_proto_mtu(idb, LINK_CLNS),
		     "%s %d", csb->nv_command, idb->clns_mtu);
	    break;

	case CLNS_NSAP:
	case CLNS_NET:
	    clns_net_nsap_cmd(csb);
	    break;

	case CLNS_RDPDU_INTRVL:
	    nv_write(idb->clns_rd_interval != CLNS_DEF_RDPDU_INTERVAL,
		     "%s %d", csb->nv_command, idb->clns_rd_interval);
	    break;

	case CLNS_RDPDU_MASK:
	    nv_write(!idb->clns_send_rdpdumasks, "no %s", csb->nv_command);
	    break;

	case CLNS_ROUTER:
	    clns_router_intfc_cmd(csb);
	    break;

	case CLNS_SEND_ERPDU:
	    nv_write(! idb->clns_send_erpdus, "no %s", csb->nv_command);
	    break;

	case CLNS_SEND_RDPDU:
	    nv_write(! idb->clns_send_rdpdus, "no %s", csb->nv_command);
	    break;

	case CLNS_DEC_COMPAT:
	    nv_write(idb->clns_dec_compatible, csb->nv_command);
	    break;

	case CLNS_ACCESS_GROUP:
	    nv_write(idb->clns_filter_in, "%s %s in", csb->nv_command, 
		     filter_name(idb->clns_filter_in));
	    nv_write(idb->clns_filter_out, "%s %s out", csb->nv_command,
		     filter_name(idb->clns_filter_out));
	    break;

	case CLNS_SPLIT_CMD:
	    if (idb->clns_split_enabled != idb->clns_split_enabled_def) {
		nv_write(idb->clns_split_enabled, "clns split-horizon");
		nv_write(!idb->clns_split_enabled, "no clns split-horizon");
	    }
	    break;
	    
	case CLNS_ADJ_FILTER:
	    nv_write(idb->clns_es_adj_filter, "%s es %s", csb->nv_command, 
		     filter_name(idb->clns_es_adj_filter));
	    nv_write(idb->clns_is_adj_filter, "%s is %s", csb->nv_command,
		     filter_name(idb->clns_is_adj_filter));
	    break;
	    
	case CLNS_ESCT_TIME:
	    nv_write(idb->clns_esct_time, "%s %d", csb->nv_command,
		     idb->clns_esct_time);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Process command.
     */
    switch (csb->which) {
    case CLNS_ROUTEC:
	if (GETOBJ(int,1) == CLNS_SSE_ROUTECACHE) {
	    if (hwidb->status & IDB_CBUS) {
		if (reg_invoke_sse_present() && !is_smds(hwidb)) {
		    if (csb->sense) {
			/*
			 * Enable clns on this interface.
			 */
			if (!idb->clns_enabled)
			    clns_itf_enable(idb);
			
			clns_running = TRUE;
			clns_router_running = TRUE;
			hwidb->clns_routecache |= CLNS_FAST_ROUTECACHE |
			    CLNS_SSE_ROUTECACHE;
			delete_fair_queue(hwidb);
		    } else 
			hwidb->clns_routecache &= ~CLNS_SSE_ROUTECACHE;
		} else {
		    if (csb->sense)
			printf("SSE switching not supported");
		    else
			hwidb->clns_routecache &= ~CLNS_SSE_ROUTECACHE;
		    return;
		}
	    } else {
		printf("%s not cBus interface", idb->namestring);
		return;
	    }
	} else
	    idb->hwptr->clns_routecache = csb->set_to_default || csb->sense;
	clns_fastsetup(idb->hwptr);
	break;

    case CLNS_CLUSTER_CMD:
	idb->clns_cluster_alias = csb->sense;
	break;

    case CLNS_CHECKSUM_CMD:
	idb->clns_checksum = csb->set_to_default || csb->sense;
	break;

    case CLNS_CNGST_THRESH:
	if (csb->sense) {
	    idb->hwptr->clns_congestion = GETOBJ(int,1);
	} else {
	    idb->hwptr->clns_congestion = CLNS_DEF_CONGESTION;
	}
	break;

    case CLNS_ENABLE:
	clns_intfc_enable_cmd(csb);
	break;

    case CLNS_ERPDU_INTRVL:
	if (csb->sense) {
	    idb->clns_er_interval = GETOBJ(int,1);
	} else {
	    idb->clns_er_interval = CLNS_DEF_ERPDU_INTERVAL;
	}
	break;

    case CLNS_ES_NEIGHBOR:
    case CLNS_IS_NEIGHBOR:
	csb->which = GETOBJ(int,1);
	clns_xs_neighbor_cmd(csb);
	break;

    case CLNS_MTU:
	if (csb->sense) {
	    if (GETOBJ(int,1) > max_proto_mtu(idb, LINK_CLNS)) {
		printf("\n%%CLNS: Invalid CLNS mtu.  Must be <= %d",
		       max_proto_mtu(idb, LINK_CLNS));
	    } else {
		idb->clns_mtu = GETOBJ(int,1);
	    }
	} else {
	    idb->clns_mtu = def_proto_mtu(idb, LINK_CLNS);
	}
	break;

    case CLNS_NSAP:
    case CLNS_NET:
	clns_net_nsap_cmd(csb);
	break;

    case CLNS_RDPDU_INTRVL:
	if (csb->sense) {
	    idb->clns_rd_interval = GETOBJ(int,1);
	} else {
	    idb->clns_rd_interval = CLNS_DEF_RDPDU_INTERVAL;
	}
	break;

    case CLNS_RDPDU_MASK:
	idb->clns_send_rdpdumasks = csb->set_to_default || csb->sense;
	break;

    case CLNS_ROUTER:
	clns_router_intfc_cmd(csb);
	break;

    case CLNS_SEND_ERPDU:
	idb->clns_send_erpdus = csb->set_to_default || csb->sense;
	break;

    case CLNS_SEND_RDPDU:
	idb->clns_send_rdpdus = csb->set_to_default || csb->sense;
	break;

    case CLNS_DEC_COMPAT:
	idb->clns_dec_compatible = csb->sense;
	break;

    case CLNS_ACCESS_GROUP:
	clns_set_access_group(idb, csb->sense ? GETOBJ(string,1) : NULL, 
			      GETOBJ(int,1));
	break;

    case CLNS_SPLIT_CMD:
	if (csb->set_to_default)
	    idb->clns_split_enabled = idb->clns_split_enabled_def;
	else
	    idb->clns_split_enabled = csb->sense;
	break;

    case CLNS_ADJ_FILTER:
	clns_set_adj_filter(idb, csb->sense ? GETOBJ(string,1) : NULL, 
			    GETOBJ(int,1));
	break;

    case CLNS_ESCT_TIME:
	idb->clns_esct_time = (csb->sense) ? GETOBJ(int,1) : 0;
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;

    }
}

/*
 * clns_commands
 * Handle the CLNS commands.
 * CLNS Interface commands are handled by calling clns_intfc_commands.
 */


void clns_commands (parseinfo *csb)
{
    if (csb->nvgen) {
	nametype *name;
	int i;
	
	switch (csb->which) {

	  case CLNS_CONFIG_TIMER:
	    nv_write(clns_config_timer != CLNS_DEF_CONFIG_TIMER,
		     "%s %d", csb->nv_command,
		     clns_config_timer/ONESEC);
	    break;

	  case CLNS_HOST:
	    name = nameQ.qhead;
	    while (name) {
		if ((name->flags & NAM_PERMANENT) &&
		    (name->flags & NAM_CLNS)) {
		    nv_write(TRUE, "%s %s", csb->nv_command,
			     name->name);
		    for (i=0; i < MAXCLNSADDRESSES; i++) {
			nv_add(name->v.clns.adr[i].length," %n",
			       name->v.clns.adr[i].clns_addr);
		    }
		}
		name = name->next;
	    }
	    break;

	  case CLNS_HOLDING_TIMER:
	    nv_write(clns_holding_timer != CLNS_DEF_HOLDING_TIMER,
		     "%s %d", csb->nv_command,
		     clns_holding_timer/ONESEC);
	    break;

	  case CLNS_NSAP:
	  case CLNS_NET:
	    clns_net_nsap_cmd(csb);
	    break;

	  case CLNS_PACKET_LIFETIME:
	    nv_write(clns_packet_lifetime != CLNS_DEF_LIFETIME,
		     "%s %d", csb->nv_command,
		     clns_packet_lifetime);
	    break;

	  case CLNS_ROUTE:
	    nv_clns_route(csb);
	    break;

	  case CLNS_ROUTING:
	    nv_write(clns_router_running, csb->nv_command);
	    break;

/* XXX CLNS_SOURCEROUTE is commented out in cisco sources as of 25may92 - tcs
	  case CLNS_SOURCEROUTE:
	    nv_write(clns_source_route_allowed,
		     "%s", csb->nv_command);
	    break;
*/

	  case CLNS_WANT_ERPDU:
	    nv_write(!clns_want_erpdus, "no %s", csb->nv_command);
	    break;

	  case CLNS_SECURITY_PASSTHRU:
	    nv_write(clns_security_passthru, "%s", csb->nv_command);
	    break;

	  case CLNS_CACHE_INVALIDATE:
	    nv_clns_cache_write(csb);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    csb->interface = NULL;
    switch (csb->which) {
      case CLNS_CONFIG_TIMER:
	if (!csb->sense) {
	    clns_config_timer = CLNS_DEF_CONFIG_TIMER;
	}else{
	    ulong config_time;
	    /* 
	     * Issue a warning if config timer is being configured to a
	     * value that is greater than half the holding timer.
	     */
	    config_time = GETOBJ(int,1)*ONESEC;
	    if ((config_time*2) > clns_holding_timer) {
	        printf("\n"
	                "**Warning** - configuration timer value should be\n"
 	                "     no more than twice that of the holding timer");
	    }

	    clns_config_timer = GETOBJ(int,1) * ONESEC;
	}
	break;
      case CLNS_HOST:
	clns_host_command(csb);
	break;
      case CLNS_HOLDING_TIMER:
	if (!csb->sense) {
	    clns_holding_timer = CLNS_DEF_HOLDING_TIMER;
	}else{
	    ulong hold_time;
	    /* 
	     * Issue a warning if hold timer is being configured to a
	     * value that is less than twice the config timer.
	     */
	    hold_time = GETOBJ(int,1)*ONESEC;
	    if (hold_time < (2*clns_config_timer)) {
		printf("\n"
		       "**Warning** - holding timer value should be\n"
		       "     at least twice that of the configuration timer");
	    }
	    clns_holding_timer = hold_time;
	}
	break;
      case CLNS_NSAP:
      case CLNS_NET:
	clns_net_nsap_cmd(csb);
	break;
      case CLNS_PACKET_LIFETIME:
	if (!csb->sense) {
	    clns_packet_lifetime = CLNS_DEF_LIFETIME;
	}else{
	    clns_packet_lifetime = GETOBJ(int,1);
	}
	break;
      case CLNS_ROUTE:
	clns_route_cmd(csb);
	break;
      case CLNS_ROUTING:
	clns_routing_cmd(csb);
	break;
/*  Commented out in cisco 9.1 sources of 25May92 - tcs
      case CLNS_SOURCEROUTE:
        printf("\n%%CLNS: sorry, source routing not supported yet");
	clns_source_route_allowed = csb->sense;
	break;
*/
      case CLNS_WANT_ERPDU:
	clns_want_erpdus = csb->set_to_default || csb->sense;
	break;

      case CLNS_SECURITY_PASSTHRU:
	clns_security_passthru = csb->sense;
	break;

      case CLNS_CACHE_INVALIDATE:
	if (csb->sense) {
	    /* If no parameters, use the defaults. */
	    if (!GETOBJ(int,2)) {
		clns_cache_inval_init(CLNS_CACHE_MIN_INVAL_DEFAULT,
				      CLNS_CACHE_MAX_INVAL_DEFAULT,
				      CLNS_CACHE_QUIET_IVL_DEFAULT,
				      CLNS_CACHE_QUIET_THRESH_DEFAULT);
	    } else {
		ulong min = GETOBJ(int,1) * ONESEC;
		ulong max = GETOBJ(int,2) * ONESEC;

		if (min > max) {
		    printf("\nCLNS:  Max cannot be less than min");
		    return;
		}
		clns_cache_inval_init(min, max, GETOBJ(int,3), GETOBJ(int,4));
	    }
	} else
	    clns_cache_inval_init(0,0,0,0);	/* Turn it off */

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * clear_clns_command
 * handle the exec command "clear clns xxx".
 *	clear clns route [<tag> | <nsap>]
 *	clear clns cache
 *	clear clns [is-|es-]neighbors [<net> | <tag>]
 * called from exec.c
 */

void clear_clns_command (parseinfo *csb)
{
    switch (csb->which) {
      case CLEAR_CLNS_CACHE:
	clns_invalidate_cache("clear_clns_command", TRUE);
	break;
      case CLEAR_CLNS_ROUTE:
	clear_clns_routecommand();
	break;
      case CLEAR_CLNS_NEIGHBORS:
	clear_clns_neighbors(CLEAR_NBRS_ALL);
	break;
      case CLEAR_CLNS_IS_NEIGHBORS:
	clear_clns_neighbors(CLEAR_NBRS_IS);
	break;
      case CLEAR_CLNS_ES_NEIGHBORS:
	clear_clns_neighbors(CLEAR_NBRS_ES);
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * clns_nv_write_redist
 */
void clns_nv_write_redist (clns_pdbtype *to_pdb, char *nv_command)
{
    clns_pdbtype *pdb;
    char         *str;
    boolean      write;

  for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
      if (to_pdb == pdb) continue;
      str = to_pdb->routemap[pdb->index];

      /*
       * Don't print DOMAIN_ if we are looking at a level2 pdb.
       */
      if ((pdb->typeflag & ISO_IGRPMASK) && pdb->level == CLNS_LEVEL2)
          continue;

      write = (to_pdb->redistallowed & pdb->typeflag);

	/*
	 * IS-IS routes default to redistributing static. Don't NVGEN unless
	 * there is a route-map specified.
	 */
	if (pdb->typeflag == ISO_STATIC && (to_pdb->typeflag & ISO_ISISMASK)) {
	    if (write && !str) continue;
	    if (!write) {
		nv_write(TRUE, "no %s static", nv_command);
		continue;
	    }
	}

	if (write) {
	    nv_write(TRUE, "%s %s", nv_command, pdb->name);
	    if (pdb->typeflag != ISO_STATIC && pdb->pname) {
		nv_add(TRUE, " %s", pdb->pname);
	    }
	}
	nv_add((boolean) str, " route-map %s", str);
    }
}

/*
 * show_clns_redirects
 * show redirect entries in the nrib
 */

void show_clns_redirects (void)
{
    clns_ddbtype *ddb, *nddb;
    clns_rdbtype *rdb;
    clns_adjtype *adj;
    int          t, i;

    automore_enable(NULL);
    for (ddb = nsap_next (NULL); ddb;) {
	if (ddb->entry_type == CLNS_REDIRECT_ENTRY) {
	    mem_lock(ddb);
	    for (i = 0; i < ddb->routecount; i++) {
		rdb = ddb->rdb[i];
		adj = find_adjacency(clns_adjacency_db, rdb->first_hop,
				     FALSE, NULL, 0);
		if (!adj) {
		    continue;
		}
		t = 0;
		if (TIMER_RUNNING(rdb->expiration_time)) {
		    t = TIME_LEFT_SLEEPING(rdb->expiration_time);
		}
        	printf("\nRD %n->%n@%s:%s, HT %d",
		       ddb->destination, rdb->first_hop,
		       adj->idb->namestring, snpa_string(&adj->snpa),
		       t/ONESEC);
	    }
	    nddb = nsap_next(ddb);
	    free(ddb);
	    ddb = nddb;
	}
	else
	    ddb = nsap_next(ddb);
    }
    automore_disable();
}

/*
 * show_clns_routes
 *
 * Display the CLNS prefix routing table. This function can display the
 * whole table or a single entry.
 *
 * show clns route [<nsap> | <clns host name>]
 *
 */

static void show_clns_routes (parseinfo *csb)
{

    clns_ddbtype *ddb, *nddb;
    clns_pdbtype *pdb;
    boolean      first;
    NSAP_T       ns;
    int		 i;

    *ns = 0;
    nsap_copy(GETOBJ(paddr,1)->clns_addr, ns);

    /*
     * Display single route.
     */
    if (*ns) {
	if (!(ddb = nsap_lookup(ns, MATCH_CLOSEST))) return;
	printf("\nCLNS Prefix Routing Table");	    
	mem_lock(ddb);
	clns_show_one_route(ddb);
	free(ddb);
	return;
    }

    automore_enable(NULL);
    /*
     * Display ISO-IGRP routes.
     */
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(pdb->typeflag & ISO_IGRPMASK)) {
	    continue;
	}
	if (clns_show_igrp_routes(pdb, pdb->level)) {
	    printf("\n");
	}
    }

    /* 
     * Display CLNS prefix routing table.
     */
    first = TRUE;

    for (i = 0; i < CLNS_NETHASHLEN; i++) {
        for (ddb = clns_routes [i]; ddb; ddb = nddb) {
	    if (first) {
	        printf ("\nCLNS Prefix Routing Table");	    
	        first = FALSE;
	    }
	    mem_lock(ddb);
	    clns_show_one_route (ddb);
            nddb = ddb->next;
	    free(ddb);
	}
    }
    automore_disable();


}

/*
 * show_clns_interface
 *
 * Show the CLNS configuration of a single network interface
 */

void show_clns_interface (idbtype *idb)
{

    clns_pdbtype *pdb;
    idbqueuetype *idbqueue;
    int          when;
    isisidbtype  *isisidb;

    printf(if_is_line_proto_is_msg, idb->namestring,
	   print_idbstate(idb), interface_up(idb) ? "up" : "down");

    if (!idb->clns_enabled) {
	printf("\n  CLNS protocol processing disabled");
	return;
    }

    printf("\n  Checksums %sabled, MTU %d, Encapsulation %s",
	   idb->clns_checksum ? "en" : "dis", idb->clns_mtu, 
	   encstring((idb->hwptr->enctype == ET_ARPA || 
		      idb->hwptr->enctype == ET_SNAP) ? 
		     ET_SAP : idb->hwptr->enctype));

    printf("\n  ERPDUs %sabled, min. interval %d msec.",
	   idb->clns_send_erpdus ? "en" : "dis", idb->clns_er_interval);
    if (TIMER_RUNNING(idb->clns_er_timer)) {
	printf(", last sent ");
	print_dhms(idb->clns_er_timer);
    }

    if (clns_router_running) {
	printf("\n  RDPDUs %sabled, min. interval %d msec., Addr Mask %sabled",
	       idb->clns_send_rdpdus ? "en" : "dis", idb->clns_rd_interval,
	       idb->clns_send_rdpdumasks ? "en" : "dis");
	if (TIMER_RUNNING(idb->clns_rd_timer)) {
	    printf(", last sent ");
	    print_dhms(idb->clns_rd_timer);
	}
	printf("\n  Congestion Experienced bit set at %d packets",
	   idb->hwptr->clns_congestion);
    }

    printf("\n  CLNS fast switching %sabled",
	   idb->hwptr->fast_switch_flag & FSF_CLNS ? "en" : "dis");
    printf("\n  CLNS SSE switching %sabled",
	   idb->hwptr->clns_routecache & CLNS_SSE_ROUTECACHE ? "en" : "dis");

    if (idb->clns_dec_compatible) {
	printf("\n  DEC compatibility mode ON for this interface");
    } else {
	printf("\n  DEC compatibility mode OFF for this interface");
    }
    
    if (idb->clns_cluster_alias) {
	printf("\n  CLNS cluster alias enabled on this interface");
    }

    if (idb->clns_net)
	printf("\n  NET on this interface: %n", idb->clns_net);

    when = mgd_timer_left_sleeping(&idb->clns_hello_timer) / ONESEC;
    printf("\n  Next ESH/ISH in %d seconds", when);

    if (idb->clns_filter_in)
	printf("\n  Input filter %s", filter_name(idb->clns_filter_in));
    if (idb->clns_filter_out)
	printf("\n  Output filter %s", filter_name(idb->clns_filter_out));
    if (idb->clns_es_adj_filter)
	printf("\n  ES adjacency filter %s", 
	       filter_name(idb->clns_es_adj_filter));
    if (idb->clns_is_adj_filter)
	printf("\n  IS adjacency filter %s", 
	       filter_name(idb->clns_is_adj_filter));

    /* 
     * Show routing protocol interface related data. 
     */
    for (pdb = clns_protoQ.qhead; pdb; pdb=pdb->next) {
	for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	    if (idb != idbqueue->idb) continue;
	    if (pdb->typeflag & ISO_ISISMASK) {
		isisidb = get_isis_idb(pdb, idb);
		if (!isisidb) continue;
		printf("\n  Routing Protocol: IS-IS");
		printf("\n    Circuit Type: %s", 
		       (isisidb->isis_circuit_type == ISIS_CIRCUIT_L1) ? 
		       isis_L1_str :
		       (isisidb->isis_circuit_type == ISIS_CIRCUIT_L1L2) ? 
		       isis_L1L2_str : isis_L2only_str);
		printf("\n    Interface number %#x, local circuit ID %#x",
		       isisidb->isis_if_number,
		       isisidb->isis_local_ckt_number);

		if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L1) {
		    printf("\n    Level-1 Metric: %d, Priority: %d, "
			   "Circuit ID: %s",
			   isisidb->isis_L1_metric, isisidb->isis_L1_cur_prio,
			   systemid_name_string(pdb,
						isisidb->isis_L1_circuit_id));
		    printf("\n    Number of active level-1 adjacencies: %d", 
			   isisidb->isis_adj_count[0]);
		}
		if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L2) {
		    printf("\n    Level-2 Metric: %d, Priority: %d, "
			   "Circuit ID: %s",
			   isisidb->isis_L2_metric, isisidb->isis_L2_prio,
			   systemid_name_string(pdb,
						isisidb->isis_L2_circuit_id));
		    printf("\n    Number of active level-2 adjacencies: %d", 
			   isisidb->isis_adj_count[1]);
		}

		if (is_p2p(idb)) {
		    when = mgd_timer_left_sleeping(&isisidb->
						   isis_hello_timer) / 
						       ONESEC;
			printf("\n    Next IS-IS Hello in %d seconds", 
			       when);
		
		} else {

		    if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L1) {
			when = mgd_timer_left_sleeping(&isisidb->
						       isis_hello_timer) / 
							   ONESEC;
			printf("\n    Next IS-IS LAN Level-1 Hello in "
			       "%d seconds", 
			       when);
		    }

		    if (isisidb->isis_circuit_type & ISIS_CIRCUIT_L2) {
			when = mgd_timer_left_sleeping(&isisidb->
						       isis_hello2_timer)/ 
							   ONESEC;
			printf("\n    Next IS-IS LAN Level-2 Hello in "
			       "%d seconds",  when);
		    }
		}

	    } else {
		printf("\n  ISO-IGRP split horizon %sabled",
		       (idb->clns_split_enabled) ? "en" : "dis");

		if (pdb->level & CLNS_LEVEL1) {
		    printf("\n  Routing Protocol: ISO-IGRP");
		    printf("\n    Routing Domain/Area: %s/%s",
			   domain_string(pdb->domain_length, pdb->domain),
			   area_string(get_area(pdb->net)));
		} else if (idb->clns_level == CLNS_LEVEL2) {
		    printf("\n  Routing Protocol: ISO-IGRP");
		    printf("\n    Routing Domain: <%s>",
			   domain_string(pdb->domain_length, pdb->domain));
		    printf("   Level 2 only");
		}
	    }
	}
    }
}

/*
 * show_clns_traffic
 * show clns traffic counters
 */

void show_clns_traffic (void)
{
    clns_pdbtype *isis_pdb;

    /*
     * Display CLNS related statisitics. 
     */
    automore_enable(NULL);
    printf("\nCLNS & ESIS Output: %d, Input: %d",
	   clns_traffic[CLNS_OUTPUT], clns_traffic[CLNS_INPUT]);
    printf("\nCLNS Local: %d, Forward: %d",
	   clns_traffic[CLNS_LOCAL], clns_traffic[CLNS_FORWARD]);
    printf("\nCLNS Discards:");
    printf("\n  Hdr Syntax: %d, Checksum: %d, Lifetime: %d, Output cngstn: %d",
	   clns_traffic[CLNS_HDRSYNTAX], clns_traffic[CLNS_CHECKSUM],
	   clns_traffic[CLNS_LIFETIME], 
	   clns_traffic[CLNS_OUTPUTCNGSTN_DROPPED]);
    printf("\n  No Route: %d, Dst Unreachable %d, Encaps. Failed: %d", 
	   clns_traffic[CLNS_NOROUTE], clns_traffic[CLNS_UNREACHDST],
	   clns_traffic[CLNS_ENCAPSFAILED]);
    printf("\n  NLP Unknown: %d, Not an IS: %d",
	   clns_traffic[CLNS_NLPUNKNOWN], clns_traffic[CLNS_NOT_AN_IS]);
    printf("\nCLNS Options: Packets %d, total %d, bad %d, GQOS %d, cngstn exprncd %d",
	   clns_traffic[CLNS_OPTIONS_PACKETS],
	   clns_traffic[CLNS_OPTIONS_TOTAL],
	   clns_traffic[CLNS_BADOPTION],
	   clns_traffic[CLNS_OPTIONS_GQOS],
	   clns_traffic[CLNS_OUTPUTCNGSTN_EXPRNCED]);
    printf("\nCLNS Segments:  Segmented: %d, Failed: %d",
	   clns_traffic[CLNS_SEGMENTED], clns_traffic[CLNS_SEGFAILED]);
    printf("\nCLNS Broadcasts: sent: %d, rcvd: %d",
	   clns_traffic[CLNS_BCASTIN], clns_traffic[CLNS_BCASTOUT]);
    printf("\nEchos: Rcvd %d requests, %d replies",
	    clns_traffic[CLNS_ECHOREQIN], clns_traffic[CLNS_ECHOREPIN]);
    printf("\n      Sent %d requests, %d replies",
	    clns_traffic[CLNS_ECHOREQOUT], clns_traffic[CLNS_ECHOREPOUT]);
    printf("\nESIS(sent/rcvd): ESHs: %d/%d, ISHs: %d/%d, RDs: %d/%d, QCF: %d/%d",
	   clns_traffic[ESIS_ESHSSENT], clns_traffic[ESIS_ESHSRCVD],
	   clns_traffic[ESIS_ISHSSENT], clns_traffic[ESIS_ISHSRCVD],
	   clns_traffic[ESIS_RDUSSENT], clns_traffic[ESIS_RDUSRCVD],
	   clns_traffic[ESIS_QCFSENT], clns_traffic[ESIS_QCFRCVD]);

    /*
     * Display ISO-IGRP related staistics.
     */
    printf("\nISO-IGRP: Querys (sent/rcvd): %d/%d",
	   clns_traffic[ISO_IGRP_QOUT], clns_traffic[ISO_IGRP_QIN]);
    printf(" Updates (sent/rcvd): %d/%d", 
	   clns_traffic[ISO_IGRP_OUT], clns_traffic[ISO_IGRP_IN]);
    printf("\nISO-IGRP: Router Hellos: (sent/rcvd): %d/%d",
	   clns_traffic[ISO_IGRP_H_OUT],
	   clns_traffic[ISO_IGRP_H_IN]);
    printf("\nISO-IGRP Syntax Errors: %d", clns_traffic[ISO_IGRP_SYNTAX_ERROR]);

    /*
     * Display IS-IS related statistics.
     */
    printf("\nIS-IS: Level-1 Hellos (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_HELLO_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_HELLO_IN]);
    printf("\nIS-IS: Level-2 Hellos (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_HELLO_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_HELLO_IN]);
    printf("\nIS-IS: PTP Hellos     (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_PTP_HELLO_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_PTP_HELLO_IN]);
    printf("\nIS-IS: Level-1 LSPs sourced (new/refresh): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_LSP_BUILD],
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_LSP_REFRESHED]);
    printf("\nIS-IS: Level-2 LSPs sourced (new/refresh): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_LSP_BUILD],
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_LSP_REFRESHED]);
    printf("\nIS-IS: Level-1 LSPs flooded (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_LSP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_LSP_IN]);
    printf("\nIS-IS: Level-2 LSPs flooded (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_LSP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_LSP_IN]);
    printf("\nIS-IS: LSP Retransmissions: %d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_RETRANSMITS]);
    printf("\nIS-IS: FastPSNP cache hits: %d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_FASTPSNP_CACHEHITS]);
    printf("\nIS-IS: Level-1 CSNPs (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_CSNP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_CSNP_IN]);
    printf("\nIS-IS: Level-2 CSNPs (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_CSNP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_CSNP_IN]);
    printf("\nIS-IS: Level-1 PSNPs (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_PSNP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_PSNP_IN]);
    printf("\nIS-IS: Level-2 PSNPs (sent/rcvd): %d/%d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_PSNP_OUT], 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_PSNP_IN]);
    printf("\nIS-IS: Level-1 DR Elections: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_DR_ELECTS]);
    printf("\nIS-IS: Level-2 DR Elections: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_DR_ELECTS]);
    printf("\nIS-IS: Level-1 SPF Calculations: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_SPF_DONE]);
    printf("\nIS-IS: Level-2 SPF Calculations: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_SPF_DONE]);
    printf("\nIS-IS: Level-1 Partial Route Calculations: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L1_PARTIALS]);
    printf("\nIS-IS: Level-2 Partial Route Calculations: %d", 
	   isis_traffic[PROC_TYPE_ISIS][ISIS_L2_PARTIALS]);
    printf("\nIS-IS: LSP checksum errors received: %d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_CKSUM_ERR]);
    printf("\nIS-IS: LSP HT=0 checksum errors received: %d",
	   isis_traffic[PROC_TYPE_ISIS][ISIS_CKSUM_ERR_HT0]);
    isis_pdb = find_isis_pdb();
    if (isis_pdb) {
	printf("\nIS-IS: Update process queue depth: %d/%d",
	       process_queue_size(isis_pdb->update_inputq),
	       process_queue_max_size(isis_pdb->update_inputq));
	printf("\nIS-IS: Update process packets dropped: %d",
	       isis_pdb->input_drops);
    }
    automore_disable();
    
}

/*
 * show_clns_globals
 * display global clns information
 */

void show_clns_globals (void)
{
    clns_local_nettype *local_net;
    clns_pdbtype *pdb;

    automore_enable(NULL);
    printf("\nGlobal CLNS Information:");
    printf("\n  %d Interfaces Enabled for CLNS", clns_nnets);

    for (local_net = clns_local_nets.qhead; local_net;
	 local_net = local_net->next) {
	printf("\n  NET: %n", local_net->net);
    }

    printf("\n  Configuration Timer: %d, Default Holding Timer: %d, Packet Lifetime %d",
	   clns_config_timer/ONESEC,
	   clns_holding_timer/ONESEC,
	   clns_packet_lifetime);

    printf("\n  ERPDU's %srequested on locally generated packets",
	   clns_want_erpdus ? "" : "not ");

    if (!clns_router_running) {
	printf("\n  End system operation only (forwarding not allowed)");
	automore_disable();
	return;
    }

    /* 
     * Print out info for each domain/area that this router is in.
     */
    printf("\n  Intermediate system operation enabled (forwarding allowed)");

    if (!clns_protoQ.qhead || clns_protoQ.qhead == clns_protoQ.qtail) {
	printf("\n  Static Routing Enabled");
	automore_disable();
	return;
    }

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->typeflag == ISO_STATIC) continue;

	if (pdb->typeflag & ISO_ISISMASK) {
	    printf("\n  IS-IS %s Router: %s",
	       (pdb->level == ISIS_CIRCUIT_L1) ?  isis_L1_str :
	       (pdb->level == ISIS_CIRCUIT_L2) ? isis_L2only_str :
	       isis_L1L2_str, pdb->pname);
	    printf("\n    Routing for Area: %s", 
		   (pdb->net[0]) ? long_area_string(pdb->net) : "");
	} else if (pdb->typeflag & ISO_IGRPMASK) {
	    printf("\n  ISO-IGRP %s Router: %s", (pdb->level == CLNS_LEVEL1) ?
		   isis_L1_str : isis_L2_str, pdb->pname);
	    printf("\n    Routing for Domain: %s", 
		   domain_string(pdb->domain_length, pdb->domain));
	    if (pdb->level == CLNS_LEVEL1)
		printf(", Area: %s", area_string(get_area(pdb->net)));
	}
    }
}

/*
 * show_one_cache_entry
 *
 * 'cache_entry' is locked before this function is called.
 *
 */
static void show_one_cache_entry (
    clns_cachetype     *cache_entry,
    clns_cache_adjtype *cache_adj,
    clns_cache_adjtype *first_cache_adj,
    int                version,
    int		       cache_line)
{

    clns_adjtype *adj;
    boolean      valid;
    uchar        *str;
    int		 j;
    hwaddrtype cache_addr;

    valid = (cache_entry->cache_version == version);
    if (first_cache_adj == cache_adj) {
	str = clns_address_lookup(cache_entry->destination);
	printf("\n[%02x] %s%n %s", cache_line, (valid) ? "*" : " ",
	       cache_entry->destination, str);
    }

    adj = cache_adj->adj;
    if (!adj || !validmem(adj)) return;
  
    if (valid) {
	mem_lock(adj);
	if (adj->key[0]) {
	    printf("\n  -> %s @ %s", (adj->key[0] == STATION_LENGTH) ? 
		   station_string(&adj->key[1]) : nsap_string(adj->key),
		   adj->idb->namestring);
	    if (!is_p2p(adj->idb)) {
		/*
		 * Construct hwaddrtype using snpa address from cache and
		 * addrtype info from adjacency.
		 */
		snpa_copy(&adj->snpa, &cache_addr);
		bcopy(cache_adj->dst_snpa, &cache_addr.addr, cache_addr.length);
		printf(" : %s", snpa_string(&cache_addr));
	    }
	}
	free(adj);
    }
    if (cache_adj->length) {
	printf("\n  => ");
	for (j = 0; j < cache_adj->length; j++) {
	    printf("%-02x", cache_adj->rewrite[j]);
	    if (j == 37)
		printf("\n%5c",' ');
	}
	printf(" / %d", cache_adj->length);
    }
}

/*
 * show_clns_cache
 * show clns cache information about routes we've used recently
 *
 * Message Format:
 * [cache line] <nsap> <name>
 *   -> <next-hop-sys-id> @ <intfc-name> : <snpa-addr>, 
 *   => <rewrite> / <length>
 */
void show_clns_cache (void)
{
    clns_cachetype     *cache_entry, *next_cache;
    clns_cache_adjtype *cache_adj, *next_adj, *first_cache_adj;
    int                i;

    automore_enable(NULL);
    printf("\nCLNS routing cache version %d", clns_cache_version);
    printf("\n[Hash] Destination -> Next hop @ Interface : SNPA Address"
	   " => Rewrite / Length");

    for (i = 0; i < CLNS_CACHE_SIZE; i++) {
	for (cache_entry = clns_cache[i]; cache_entry; 
	     cache_entry = next_cache) {
	    mem_lock(cache_entry);
	    first_cache_adj = cache_entry->cache_adj;
 	    for (cache_adj = cache_entry->cache_adj; cache_adj; 
 		 cache_adj = next_adj) {
 		mem_lock(cache_adj);
		show_one_cache_entry(cache_entry, cache_adj, first_cache_adj,
				     clns_cache_version, i);
		next_adj = cache_adj->next;
		free(cache_adj);
 		if (next_adj == first_cache_adj) break;
	    }
	    next_cache = cache_entry->next;
	    free(cache_entry);
	}
    }

    automore_disable();
}

/*
 * show_clns
 * Display clns information.
 */
void show_clns (parseinfo *csb)
{
    if (!clns_running) {
	printf("%% CLNS is not enabled");
	return;
    }

    switch (csb->which) {
      case SHOW_CLNS_GLOBAL:
	show_clns_globals();
	break;
      case SHOW_CLNS_CACHE:
	if (GETOBJ(int,2)) {
	    show_clns_cache_delay();
        }
	else if (GETOBJ(int,1)) {
	    show_clns_cache_invalidations();
	} else {
	    show_clns_cache();
	}
	break;
      case SHOW_CLNS_ES_NEIGHBORS:
	show_clns_neighbors(NULL, clns_adjacency_db, 'E', GETOBJ(idb,1),
			    GETOBJ(int,1));
	break;
      case SHOW_CLNS_INTERFACE:
	automore_enable(NULL);
	show_swidb_chain(GETOBJ(idb,1), show_clns_interface, 0);
	automore_disable();
        break;
      case SHOW_CLNS_IS_NEIGHBORS:
	show_clns_neighbors(NULL, clns_adjacency_db, 'I', GETOBJ(idb,1),
			    GETOBJ(int,1));
	break;
      case SHOW_CLNS_NEIGHBORS:
	show_clns_neighbors(NULL, clns_adjacency_db, 'A', GETOBJ(idb,1),
			    GETOBJ(int,1));
	break;
      case SHOW_CLNS_REDIRECTS:
	show_clns_redirects();
	break;
      case SHOW_CLNS_ROUTE:
	show_clns_routes(csb);
	break;
      case SHOW_CLNS_TRAFFIC:
 	show_clns_traffic();
	break;
      case SHOW_CLNS_PROTOCOL:
	clns_show_pdb_chain(GETOBJ(string,1), clns_show_router);
	break;
      case SHOW_CLNS_FILTER_EXPR:
	show_filter_expr(GETOBJ(string,1), GETOBJ(int,1), GETOBJ(int,2), 
			 FILTER_PROTO_CLNS);
	break;
      case SHOW_CLNS_FILTER_SET:
	clns_show_filter_set(GETOBJ(string,1), GETOBJ(int,1));
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * clns_redistribute
 *
 * Process redistribute router subcommand for OSI routing protocols. 
 * Valid forms are:
 *
 *  [no] redistribute isis <tag>
 *  [no] redistribute iso-igrp <tag>
 *  [no] redistribute static
 *     
 */
void clns_redistribute (clns_pdbtype *pdb, clns_pdbtype *rpdb,
			int sense, char *map_name)
{

    NSAP_T prefix;
    char   *str;
    idbqueuetype *idbqueue;
    idbtype *idb;

    /*
     * Add/Delete route-map from pdb.
     */
    str = clns_redistribute_routemap(pdb, rpdb, map_name, sense);

    /*
     * Set redistribution flags. 
     */
    if (sense) {
	pdb->redistallowed |= rpdb->typeflag;
    } else {
	pdb->redistallowed &= ~rpdb->typeflag;
    }

    /*
     * If redistributing from ISO-IGRP, get domain pdb.
     */
    if (rpdb->typeflag & ISO_IGRPMASK) {
	nsap_copy(rpdb->net, prefix);
	if (!(prefix[0] = rpdb->domain_length)) return;
	if (!(rpdb = clns_find_level2_pdb(rpdb->net))) return;
	pdb->routemap[rpdb->index] = str;
	if (str) mem_lock(str);
    } 

    /*
     * If redistributing from IS-IS, get area address.
     */
    if (rpdb->typeflag & ISO_ISISMASK) {
	nsap_copy(rpdb->net, prefix);
	prefix[0] -= (STATION_LENGTH + SELECTOR_LENGTH);
    }

    /*
     * If redistributed into ISO-IGRP, set flags in both pdbs. The 
     * level-1 pdb (done above) is used to flag redistribution to
     * keep state if the level-2 pdb does not have a domain prefix
     * defined yet as well as for "write terminal". The level-2 pdb 
     * is used for the actual redistribution.
     */
    if (pdb->typeflag & ISO_IGRPMASK) {
	if (!pdb->net[0]) return;
	if (!(pdb = clns_find_level2_pdb(pdb->net))) return;
    }
    if (sense) {
	pdb->redistallowed |= rpdb->typeflag;
    } else {
	/*
         * If it's ISO-IGRP that we were redistributing into, make sure that
	 * we poison all routes that were learnt from the redistributing
	 * protocol.
	 */
	if (pdb->typeflag & ISO_IGRPMASK) {
	    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue=idbqueue->next){
		idb = idbqueue->idb;
		if ((idb) && (idb->clns_enabled) && (interface_up(idb))){
		    iso_igrp_sendupdate(pdb, idb, TRUE, rpdb, NULL);
	        }
	    }    
        }

	pdb->redistallowed &= ~rpdb->typeflag;
    }

    /*
     * Enter prefix into routing table so respective protocol can
     * redistribute it.
     */
    if (rpdb->typeflag != ISO_STATIC) {
	if (sense) {

	    /*
	     * If NET configured for process, add to routing table.
	     */
	    if (prefix[0]) {
		clns_route_update(prefix, pdb->net, NULL, CLNS_ROUTE_ENTRY,
				  rpdb->typeflag, rpdb->distance, 0,
				  timer_no_time, 0, pdb->net);
	    }
	} else {
	    nsap_delete(prefix);
	}
    }

    /*
     * If redistributing into IS-IS, build new level-2 LSP.
     */
    if (pdb->typeflag & ISO_ISISMASK) {

	/*
	 * If redistributing from ISO-IGRP, add each prefix as a
	 * prefix neighbor to the adjacency database.
	 */
	if (rpdb->typeflag & ISO_IGRPMASK) {
	    if (!clns_manage_all_prefix_nbrs(pdb, rpdb, sense)) return;
	}
	isis_mark_L2_changed(pdb);
    }
}

/*
 * clns_redistribute_routemap
 *
 * Add/delete route-map name from a pdb.
 */
char *clns_redistribute_routemap (
    clns_pdbtype *pdb,
    clns_pdbtype *rpdb,
    char *map_name,
    boolean sense)
{

    char *str;
    int  len, index;

    str = NULL;
    if (sense) {
	if (!map_name || !*map_name) 
	    return(NULL);
	len = strlen(map_name) + 1;
	str = malloc(len);
	if (!str) return(NULL);
	bcopy(map_name, str, len);
    }

    index = rpdb->index;
    if (pdb->routemap[index]) {
	free(pdb->routemap[index]);
    }
    pdb->routemap[index] = str;
    return(str);
}

/*
 * clns_distance
 *
 * Process distance command for CLNS routing protocols.
 */
void clns_distance (clns_pdbtype *pdb, int distance)
{

    clns_ddbtype *ddb, *next_ddb;
    clns_rdbtype *rdb;
    clns_pdbtype *temppdb;
    int          i;

    pdb->distance = distance;

    /*
     * If distance has changed for an ISO-IGRP pdb, get level-2 pdb
     * as well.
     */
    if (pdb->typeflag & ISO_IGRPMASK) {
	temppdb = clns_find_level2_pdb(pdb->net);
	if (!temppdb) return;
	pdb = temppdb;
	pdb->distance = distance;
    }

    /*
     * Clear routing table for routes owned by this process. 
     */
    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	for (ddb = clns_routes[i]; ddb; ddb = next_ddb) {
	    next_ddb = ddb->next;
	    if (ddb->entry_type != CLNS_ROUTE_ENTRY) continue;
	    rdb = ddb->rdb[0];
	    if (rdb && (rdb->protocol_source & pdb->typeflag)) {
		nsap_delete(ddb->destination);
	    }
	}
    }

    /*
     * If ISO-IGRP routes get cleared and IS-IS is running, run level-2
     * SPF to get new routes.
     */
    for (temppdb = clns_protoQ.qhead; temppdb; temppdb = temppdb->next) {
	if (temppdb->typeflag & ISO_ISISMASK) {
	    isis_mark_L2_changed(temppdb);
	    isis_trigger_spf(temppdb, CLNS_LEVEL2, ISIS_SPF_TRIG_DISTANCE,
			     NULL, TRUE);
	}
    }
}

/*
 * nv_clns_route
 *
 * Perform NV generation for CLNS static prefix routes "clns route" commands.
 */
void nv_clns_route (parseinfo *csb)
{

    clns_staticQtype *entry;
 
    nv_write(TRUE, "!");

    for (entry = clns_staticQ.qhead; entry; entry = entry->next) {
 
 	nv_write(TRUE, "%s %n ", csb->nv_command, entry->prefix);
 
 	/*
	 * 0-length next-hop and NULL idb means a discard entry.
	 */
 	if (!entry->idb) {
 	    nv_add(TRUE, "%s", (!entry->next_hop[0]) ? "discard" :
 		   nsap_string(entry->next_hop));
 	} else {
 	    /*
	     * Interface static routes (includes SNPA and interface).
	     */
 	    nv_add (TRUE, "%s %s", entry->idb->namestring,
		    is_p2p(entry->idb) ? " " : snpa_string(&entry->snpa));
  	}
    }
}

/*
 *
 */
void ip_clns_redistribute_level_nvgen (pdbtype *proto, pdbtype *pdb)
{
    const char *str;

    if (proto->proctype & PROC_ISIS) {
	str = NULL;
	if (pdb->criteria[proto->index].routetype == RTYPE_L1) {
	    str = isis_L1_str;
	} else if (pdb->criteria[proto->index].routetype == RTYPE_L2) {
	    str = isis_L2_str;
	} else if (pdb->criteria[proto->index].routetype ==
		   (RTYPE_L1|RTYPE_L2)) {
	    str = isis_L1L2_str;
	}
	nv_add((boolean) str, " %s", str);
    }
}

/*
 *
 */
void ip_clns_redistribute_level_printf (pdbtype *proto, pdbtype *pdb)
{
    const char *str;

    if (proto->proctype & PROC_ISIS) {
	str = NULL;
	if (pdb->criteria[proto->index].routetype == RTYPE_L1) {
	    str = isis_L1_str;
	} else if (pdb->criteria[proto->index].routetype == RTYPE_L2) {
	    str = isis_L2_str;
	} else if (pdb->criteria[proto->index].routetype ==
		   (RTYPE_L1|RTYPE_L2)) {
	    str = isis_L1L2_str;
	}
	if (str)
	  printf(" %s", str);
    }
}

/*
 *
 */
void ip_clns_redistribute_metric_nvgen (pdbtype *proto, pdbtype *pdb)
{
    const char *str;
    
    if (!(pdb->proctype & PROC_ISIS))
	return;

    nv_add(TRUE, " metric-type %s",
	   (pdb->metric[proto->index].metric.scalar & 0x80) ?
	   "external" : "internal");
    str = NULL;
    if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L1) {
	str = isis_L1_str;
    } else if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L2) {
	str = isis_L2_str;
    } else if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L1L2) {
	str = isis_L1L2_str;
    }
    nv_add((boolean) str, " %s", str);
}

/*
 *
 */
void ip_clns_redistribute_metric_printf (pdbtype *proto, pdbtype *pdb)
{
    const char *str;
    
    if (!(pdb->proctype & PROC_ISIS))
	return;

    printf(" metric-type %s",
	   (pdb->metric[proto->index].metric.scalar & 0x80) ?
	   "external" : "internal");
    str = NULL;
    if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L1) {
	str = isis_L1_str;
    } else if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L2) {
	str = isis_L2_str;
    } else if (pdb->metric[proto->index].type == ISIS_CIRCUIT_L1L2) {
	str = isis_L1L2_str;
    }
    if (str)
        printf(" %s", str);
}
