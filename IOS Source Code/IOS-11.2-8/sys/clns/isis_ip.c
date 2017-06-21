/* $Id: isis_ip.c,v 3.7.26.4 1996/06/28 23:05:19 hampton Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_ip.c,v $
 *------------------------------------------------------------------
 * isis_ip.c -- IS-IS support for TCP/IP networks.
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_ip.c,v $
 * Revision 3.7.26.4  1996/06/28  23:05:19  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.26.3  1996/06/18  16:52:29  dkatz
 * CSCdi60661:  ISIS puts repeated interface routes in LSP
 * Branch: California_branch
 * Rework the handling of unnumbered interfaces--they are not equivalent
 * to the referenced numbered interfaces.
 *
 * Revision 3.7.26.2  1996/05/23  07:36:49  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.7.26.1  1996/03/18  19:03:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.1  1996/03/07  08:40:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/01/22  06:13:20  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/18  21:48:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/02  09:06:08  rchandra
 * CSCdi43948:  ISIS-IP LSP contain non-zero metric with I/E bit set
 * - prevent the parser from accepting more than 63 as metric for
 *   redistributed routes
 * - while adding the L1 learned routes into L2, do not allow the metric
 *   to be more than 63
 *
 * Revision 3.4  1995/12/01  02:48:30  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:53:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  20:52:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/03  22:58:24  rchandra
 * CSCdi39069:  Route entries all hash to same bucket
 * - Use the last 4 bytes of the NSAP for hashing
 * - include the code-review changes for CSCdi38153
 * - rebuild both L1 and L2 LSPs when redistribute command is entered
 *
 * Revision 2.16  1995/10/02  20:45:32  pst
 * CSCdi41383:  garbage text in show ip route
 *
 * Revision 2.15  1995/09/25  08:34:44  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.14  1995/09/23  23:14:16  dkatz
 * CSCdi39776:  ISIS can mislink backup chain
 * Dont leave a backup chain dangling from the heap.
 * Make sure that we run SPF when the second level adjacency comes up
 * if L1L2.
 *
 * Revision 2.13  1995/09/23  22:18:04  dkatz
 * CSCdi39389:  ISIS route changing L2->L1 does not update L2 LSP
 * Trigger L2 LSP generation if an installed L1 route is replacing a
 * non-L1 route.
 *
 * Revision 2.12  1995/09/23  21:37:41  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.11  1995/08/25  15:12:01  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.10  1995/08/25  04:49:12  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Don't redistribute static routes as connected or ISIS routes.
 * Don't advertise ISIS routes twice if redistributing connected.
 * Make sure that the route table is updated properly when
 * the router level is changed.
 *
 * Revision 2.9  1995/08/25  04:38:58  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Rework the circumstances under which level-2 LSPs are regenerated
 * for IP summarization.
 *
 * Revision 2.8  1995/08/23  18:26:04  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.7  1995/08/23  18:12:48  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Don't try to check the SPF instance on dummy NDBs.
 *
 * Display the time left for zero-age LSPs so we don't get confused.
 *
 * Revision 2.6  1995/08/23  18:03:11  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.5  1995/08/22  08:19:49  rchandra
 * CSCdi38153:  Route maps shouldnt default levels
 * - if route-map does not specify level and explicit level is configured,
 *   use the explicit level to decide if redistribution is allowed.
 *
 * Revision 2.4  1995/08/08  16:43:02  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.3  1995/06/21 08:53:52  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/18  19:39:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:18:45  hampton
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
#include "../if/network.h"
#include "config.h"
#include "subsys.h"
#include "../iprouting/route.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../iprouting/route_inlines.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "../clns/isis.h"
#include "isis_registry.h"


/*
 * isis_insert_ip_if_address
 *
 * This function inserts IP address(es) for a given interface in IIHs and LSPs.
 * This function does useful things only if IS-IS is configured to do IP.
 *
 * When input_idb is NULL, the first interface address is built.
 * Otherwise, only the specified interface address is built.
 * A single interface address is used in LSPs where the latter is used
 * in Hello packets.
 *
 * bytes_left should include space for the item header.
 */
static isis_info_type *isis_insert_ip_if_address (clns_pdbtype *pdb,
						  isis_info_type *item,
						  idbtype *input_idb,
						  int level,
						  int bytes_left)
{

    ipaddrtype   *ip_address;
    ipaddtype    *ptr;
    idbtype      *idb;
    idbqueuetype *idbq;
    isisidbtype  *isisidb;
    ipaddrtype   interface_address;

    /* We only allow a single TLV from this routine. */

    item->length = 0;
    item->code = ISIS_IP_IF_ADDRESSES;
    bytes_left = min(bytes_left, 
		     ISIS_INFO_TYPE_MAXDATA + ISIS_INFO_TYPE_OVERHEAD);
    if (bytes_left < ISIS_INFO_TYPE_OVERHEAD)
	return(item);
    bytes_left -= ISIS_INFO_TYPE_OVERHEAD;


    /*
     * If IS-IS not in IP mode or IP/IS-IS not enabled on interface, return.
     */
    if (!(pdb->isis_mode & ISIS_IP_MODE)) return(item);
    if (input_idb && !isis_checkqueue(&pdb->ip_idbq, input_idb)) return(item);
    for (idbq = pdb->ip_idbq.qhead; idbq; idbq = idbq->next) {
	idb = idbq->idb;
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb) continue;
	if (input_idb && input_idb != idb) continue;

	/*
	 * Conditions to ignore interface:
	 *   Interface is down
	 *   IP is disabled on interface
	 *   Level mismatch (if interface is level-2-only, don't include
	 *                   interface addresses in level-1 LSPs.
	 *   If unnumbered interface pointer does not exist.
	 */
	if (!interface_up (idb)) continue;
	if (!idb->ip_enabled) continue;
	if (!(isisidb->isis_circuit_type & level)) continue;
	interface_address = idb->ip_address;
	if (!interface_address) {
	    if (!idb->ip_unnumbered)
		continue;
	    interface_address = idb->ip_unnumbered->ip_address;
	}
	if (!idb) continue;

	/*
	 * Build item.
	 */
 	if (bytes_left < sizeof(ipaddrtype))
 	    break;
	ip_address = (ipaddrtype *) item->value;
    
 	PUTLONG(ip_address, interface_address);
 	ip_address++;
	item->length += sizeof(ipaddrtype);
 	bytes_left -= sizeof(ipaddrtype);

	if (!input_idb) break;

	/*
	 * Include secondary IP addresses.
	 */
	for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
 	    /*
 	     * If we're out of space, we'll catch it in the main loop
 	     * and get out eventually.
 	     */
 	    if (bytes_left < sizeof(ipaddrtype))
 		break;
	    PUTLONG(ip_address, ptr->address);
	    ip_address++;
 	    item->length += sizeof(ipaddrtype);
 	    bytes_left -= sizeof(ipaddrtype);
	}
    }
    ADVANCE_ITEM(item, FALSE);
    return(item);
}

/*
 * isis_insert_ip_lsp_if_address
 *
 * Insert IP addresses into LSP, if necessary.
 */
static void isis_insert_ip_lsp_if_address (isis_lspcontext *context)
{
    isis_info_type *item;
    int bytes_left;
    
    item = isis_new_item(context, ISIS_IP_IF_ADDRESSES, 0, NULL,
			 &bytes_left, FALSE);
    if (item == NULL)
	return;
    item = isis_insert_ip_if_address(context->pdb, item,
				     context->lsp_idb,
				     context->level,
				     bytes_left + ISIS_INFO_TYPE_OVERHEAD);
    isis_item_done(context, item);
}

/*
 * isis_valid_ip_if_address
 *
 * Check to see if any of the IP addresses advertised in an IIH is usable
 * as next hop when installing IP routes. If none of the IP addresses
 * matches any of our own prefixes on the incoming interface, we must
 * drop the IIH. Also check for mismatch between ISIS modes.
 */
static ipaddrtype isis_valid_ip_if_address (clns_pdbtype *pdb, idbtype *idb,
					    isis_TLV_parse *tlv_parse)
{
    isis_info_type *item, *next_item;
    isisidbtype *isisidb;
    ipaddrtype if_ipaddr;
    ipaddrtype *ipaddr_p;

    item = tlv_parse->TLV_array[ISIS_IP_IF_ADDRESSES];

    /* If we don't do ISIS-IP and there's no IP addresses in the IIH, fine. */

    if (!(pdb->isis_mode & ISIS_IP_MODE) && !item) {
	return(IPADDR_ZERO);
    }

    if (pdb->isis_mode & ISIS_IP_MODE) {
	if (item) {
	    /*
	     * Both routers run ISIS-IP. Loop over IP addresses to find the
	     * first one that's usable. If none found, don't accept hello.
	     */
	    ipaddr_p = (ipaddrtype *) &item->value;
	    next_item = item;
	    ADVANCE_ITEM(next_item, TRUE);

	    while ((char *)ipaddr_p < (char *)next_item) {
		if_ipaddr = GETLONG(ipaddr_p);
		if (ip_onifsubnet(idb, if_ipaddr)) {
		    return(if_ipaddr);
		}
		ipaddr_p++;
	    }
	    return(ISIS_INVALID_IP_NEXTHOP);
	} else {
	    /*
	     * We run ISIS-IP, but the other router seems not to run ISIS-IP.
	     * RFC1195 allows routers to omit IP interface addresses in IIHs
	     * on unnumbered links. Accept IIH if that might be the case.
	     */
	    if (!item && idb->ip_unnumbered) {
		return(IPADDR_ZERO);
	    }
	}
    }

    /*
     * Mismatch between ISIS mode of us and the adjacent router.
     * Accept adjacencies only if this is a level-2-only link.
     * This will enable users to run some areas CLNS only, some
     * IP only and some dual. Of course, proper routing over the
     * backbone for IP and CLNS is not guaranteed in this setup.
     */
    isisidb = get_isis_idb(pdb, idb);
    if (isisidb && (isisidb->isis_circuit_type == ISIS_CIRCUIT_L2)) {
	return(IPADDR_ZERO);
    }

    return(ISIS_INVALID_IP_NEXTHOP);
}

/*
 * isis_add_one_ip_entry
 *
 * Returns TRUE if successful.
 */
static boolean isis_add_one_ip_entry (isis_lspcontext *context, 
				      ipaddrtype ip_address, 
				      ipaddrtype mask,
				      ulong metric)
{
    
    isis_ip_nbr_ad *entry;
    
    /* Create a spot for the next entry. */
    
    entry = isis_add_item_entry(context, sizeof(isis_ip_nbr_ad),
				FALSE, NULL);
    if (entry == NULL)
	return(FALSE);
    
    /*
     * Fill in metric, default is the only metric supported.
     */
    entry->metric.default_metric = metric;
    entry->metric.delay_metric = ISIS_METRIC_UNSUPPORTED;
    entry->metric.expense_metric = ISIS_METRIC_UNSUPPORTED;
    entry->metric.error_metric = ISIS_METRIC_UNSUPPORTED;
    
    /*
     * Fill in destination information.
     */
    PUTLONG(&(entry->ip_address), ip_address & mask);
    PUTLONG(&(entry->mask), mask);
    return(TRUE);
}


/*
 * isis_insert_ip_internal_info
 *
 * Add routes for all of the interfaces on which IS-IS is running.  These
 * are always advertised, whether or not connected routes are redistributed
 * into ISIS or not.
 */
static void isis_insert_ip_internal_info (isis_lspcontext *context)
{

    idbtype        *idb, *next;
    ipaddtype      *ptr;
    uchar          metric;
    pdbtype        *ip_pdb;
    clns_pdbtype   *pdb;
    int		   level;
    isisidbtype    *isisidb;

    pdb = context->pdb;
    level = context->level;

    /*
     * If IS-IS not in IP mode, return.
     */
    if (!(pdb->isis_mode & ISIS_IP_MODE)) return;
    if (!(ip_pdb = pdb->ip_pdb)) return;

    if (!isis_new_item(context, ISIS_IP_INTERNAL_ADDR, 0, NULL, NULL, TRUE))
 	return;
    
    /* Walk all interfaces looking for suitable candidates. */

    for (idb = SWIDBQ_HEAD; idb; idb = next) {
	next = IDB_NEXT(idb);
	if (!interface_up(idb)) continue;
	if (!idb->ip_enabled) continue;
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb) continue;

	/*
	 * If IS-IS is configured, check if address should be advertised
	 * at this level.  We advertise L1 and L1L2 circuits at level 1,
	 * and L2-only circuits at level 2 (L1 and L1L2 circuits will be
	 * summarized at level 2).
	 */
	if (isis_checkqueue(&pdb->ip_idbq, idb)) {
	    if (!(isisidb->isis_circuit_type & level)) continue;
	    if (level == ISIS_CIRCUIT_L2 &&
		isisidb->isis_circuit_type == ISIS_CIRCUIT_L1L2) continue;
	    metric = (level == ISIS_CIRCUIT_L1) ? isisidb->isis_L1_metric :
		isisidb->isis_L2_metric;
	} else if (idb->ip_passive_router & ip_pdb->mask) {
	    
	    /*
	     * If IS-IS is not running on interface, but it is passive,
	     * advertise address with metric 0.
	     */
	    metric = 0;
	} else {
	    continue;
	}
  
	/*
	 * Advertise the primary address.  If the interface is unnumbered,
	 * don't advertise the referenced numbered interface (since it
	 * will be advertised separately if configured).
	 */
	if (idb->ip_address) {
	    if (!isis_add_one_ip_entry(context, idb->ip_address,
				       idb->ip_nets_mask, metric)) {
		return;
	    }
	}

	/* Now for the secondaries... */

  	for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
 	    if (!isis_add_one_ip_entry(context, ptr->address,
 				       ptr->mask,  metric))
 		return;
  	}
    }
    isis_item_done(context, NULL);
}

/*
 * isis_insert_ip_lsp
 *
 * Insert interfaces and internal info into an LSP
 */
static void isis_insert_ip_lsp (isis_lspcontext *context)
{
    isis_insert_ip_lsp_if_address(context);
    isis_insert_ip_internal_info(context);
}

/*
 * isis_ip_passive_interface_command
 * A passive-interface command has been issued on a interface, rebuild the
 * LSPs.
 */
static void isis_ip_passive_interface_command (pdbtype *pdb, idbtype *idb, 
					       boolean passive)
{
    clns_pdbtype *clns_pdb;

    /*
     * An interface needs to have an isisidb to consider it for
     * any ISIS related operations. Create one for the passive
     * interface and do not use it.
     */
    if (!isis_alloc_isisidb(idb, PROC_TYPE_ISIS, CLNS_PDBINDEX_ISIS0))
	return;

    isis_update_interface_state(idb);	/* Get the state straight. */

    clns_pdb = pdb->osi_pdb;
    if (!clns_pdb || !clns_pdb->net[0])
	return;

    if (clns_pdb->level & ISIS_CIRCUIT_L1)
	isis_mark_L1_changed(clns_pdb);

    if (clns_pdb->level & ISIS_CIRCUIT_L2)
	isis_mark_L2_changed(clns_pdb);
}

/*
 * isis_ip_route_adjust
 *
 * A directly connected link has gone up or down. Determine if IS-IS
 * is running IP-mode on that interface. If IS-IS is enabled on the
 * interface or marked passive on that interface, build the
 * corresponding LSPs.  Also build LSPs when explicitly asked via the
 * adjust flag. 
 */
static void isis_ip_route_adjust (idbtype *idb, boolean adjust)
{

    clns_pdbtype *pdb;
    isisidbtype *isisidb;

    pdb = find_isis_pdb();
    if (!pdb || !pdb->net[0]) return;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /*
     * Not in IP-mode.
     */
    if (!(pdb->isis_mode & ISIS_IP_MODE)) return;

    /*
     * If IS-IS is not enabled on that interface or if that interface is
     * not passive under IS-IS.
     * If explictly specified as adjust, then proceed.
     */
    if (!(idb->ip_passive_router & pdb->ip_pdb->mask) && 
	!isis_checkqueue(&pdb->ip_idbq, idb) && !adjust)
	return;

    /*
     * Check if we need to build a new LSP. If we need to build a new LSP,
     * donot build them immediatly, delay it so that it allow the CLNS 
     * changes to take effect.
     */
    if ((pdb->level & ISIS_CIRCUIT_L1 &&
	 (isisidb->isis_circuit_type & ISIS_CIRCUIT_L1)) || adjust) {
	isis_schedule_lsp_build(pdb, CLNS_LEVEL1, ISIS_IP_LSP_BUILD_DELAY,
				FALSE);
	locupdate_buginf(pdb, "L1 IP route adjust");
    }
    if ((pdb->level & ISIS_CIRCUIT_L2 &&
	 (isisidb->isis_circuit_type & ISIS_CIRCUIT_L2)) || adjust) {
	isis_schedule_lsp_build(pdb, CLNS_LEVEL2, ISIS_IP_LSP_BUILD_DELAY,
				FALSE);
	locupdate_buginf(pdb, "L2 IP route adjust");
    }
    if (isis_nlsp_spf_statistics_debug[pdb->proc_type]) {
	buginf("\n%s-Stats: IP int down", pdb->errmsg_name);
    }
}


/*
 * isis_ip_address_change
 *
 * A directly connected link has changed IP addresses. Determine if IS-IS
 * is running IP-mode and if enabled on the specified interface.
 */
static void isis_ip_address_change (idbtype *idb, ipaddrtype address,
				    ipaddrtype mask, boolean secondary, 
				    boolean adding)
{
    isis_ip_route_adjust(idb, adding);
}


/*
 * isis_ip_update_summary_metric
 *
 * Update the summary metric.  Returns TRUE if the new metric was better
 * than the old one.
 */
static inline boolean isis_ip_update_summary_metric (isis_addrsumtype *sum,
						     ulong new_metric)
{
    if (new_metric < sum->sum_L2_metric) {
 	sum->sum_L2_metric = new_metric;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * isis_ip_get_summary
 *
 * Get summary entry for given address. A longest match will be performed,
 * therefore if the following summaries exist:
 *
 *   1) 10.0.0.0 255.0.0.0
 *   2) 10.1.0.0 255.255.0.0
 *
 *   10.1.2.3 matches 2)
 *   10.2.2.3 matches 1)
 *
 * Returns NULL if there is no summary match.
 */
static isis_addrsumtype *isis_ip_get_summary (clns_pdbtype *pdb,
					      ipaddrtype address)
{
    isis_addrsumtype *sum, *save_sum;

    save_sum = NULL;
    for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
	if ((address & sum->sum_mask) == sum->sum_address) {

	    /*
	     * Get best match.
	     */
	    if (!save_sum ||
		sum->sum_mask == LONGEST_MASK(sum->sum_mask,
					      save_sum->sum_mask)) {
		save_sum = sum;
	    }
	}
    }
    return(save_sum);
}

/*
 * isis_ip_process_summary
 *
 * Called when installing level 1 routes when running L1L2.
 *
 * This function will determine if a metric for an individual route
 * has changed or the metric for an address summary has changed.  In
 * either case, we have to regenerate the level 2 LSP.
 *
 * If there is no summary, we check to see if the old route was not at
 * level-1;  if it wasn't we regenerate the level 2 LSP.
 */
static void isis_ip_process_summary (clns_pdbtype *pdb,
				     ipaddrtype address,
				     ulong new_metric,
				     ulong old_metric,
				     ushort old_route_type)
{
    isis_addrsumtype *sum;
    ulong old_summary_metric;

    /* Fetch the summary, if any, corresponding to this route. */

    sum = isis_ip_get_summary(pdb, address);
    if (sum) {
	/*
	 * Summary exists.  If the metric is improving, try updating the
	 * summary metric, and if it improves, regenerate the LSP.
	 */
	if (new_metric < old_metric) { /* Better... */
	    old_summary_metric = sum->sum_L2_metric;
	    if (isis_ip_update_summary_metric(sum, new_metric)) {
		locupdate_buginf(pdb,
				 "summary metric change (%d->%d), %i",
				 old_summary_metric, new_metric, address);
		isis_mark_L2_changed(pdb);
	    }

	} else {

	    /*
	     * Metric is getting worse.  If the old metric is the same
	     * as the summary metric, we can't tell whether the summary
	     * metric should get worse or not, so we trigger a level 2
	     * LSP generation (which will cause the summary metric table
	     * to be up to date).  Note that there shouldn't be too many
	     * false positives in this case (he said hopefully).
	     */
	    if (sum->sum_L2_metric == old_metric) {
		locupdate_buginf(pdb,
				 "summarized metric change from old summary "
				 "metric(%d->%d), %i", old_metric, new_metric,
				 address);
		isis_mark_L2_changed(pdb);
	    }
	}

    } else {

	/*
	 * No summary present.  If the metric or route type is changing on
	 * this single route, regenerate the level 2 LSP.
	 */
	if (old_metric != new_metric || old_route_type != RTYPE_L1) {
	    locupdate_buginf(pdb,
			     "non-summarized metric change (%d->%d), %i",
			     old_metric, new_metric, address);
	    isis_mark_L2_changed(pdb);
	}
    }
}

/*
 * isis_backup_ndb
 *
 * Create a backup chain containing all of the current RDBs for an
 * NDB in anticipation of its deletion.
 *
 * Moves the current ISIS backup chain to the destination backup chain
 * pointer before adding the active RDB entries to it.
 *
 * Note that this may create multiple backup entries for a single LSP
 * if the deleted route had multiple next-hops to the node that issued
 * the LSP;  this is easier to ignore than it is to fix.  It won't hurt
 * anything.
 */
static void isis_backup_ndb (clns_pdbtype *pdb, ndbtype *ndb,
			     isis_backuptype **backup_chain)
{
    ulong rdb_ix;
    rdbtype *rdb;

    /* Grab the existing backup chain. */
    
    isis_attach_backup_chain(&ndb->ndb_isis_backup, backup_chain);
		
    /* Examine each RDB in the NDB. */

    for (rdb_ix = 0; rdb_ix < ndb->routecount; rdb_ix++) {
	rdb = ndb->rdb[rdb_ix];
	if (rdb) {

	    /* RDB is there.  Build the backup entry. */

	    isis_install_backup(pdb, ndb->metric,
				rdb->isis_rdb_linkage.lsp_index,
				ISIS_ROUTE_IP, backup_chain);
	}
    }
}

/*
 * isis_install_one_ip_route
 *
 * Install a single next hop into the forwarding table.
 */
static rdbtype *isis_install_one_ip_route(clns_pdbtype *pdb,
					  ipaddrtype address,
					  ipaddrtype mask,
					  clns_adjtype *adj,
					  ulong metric,
					  ushort route_type,
					  ushort lsp_index)
{
    rdbtype *rdb;
    isis_lspix_type *lspix_entry;
    isis_lsp_linkage *linkage;

    /* Never try to install a recursive route. */

    if (!adj || !adj->idb || !ip_onifsubnet(adj->idb, adj->ip_nexthop)) {
	return(NULL);
    }

    /* Add the route. */

    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp_index);
    rdb = network_update(pdb->ip_pdb, address, mask, adj->ip_nexthop,
			 adj->idb, metric, 0, adj->ip_nexthop, 0, NULL,
			 route_type);

    /*
     * If the route was installed, link it to the LSP.
     * We are paranoid. If the rdb seems to have existed before we called
     * network_update, we will not link it in again. Aging of this rdb
     * still depends on the other LSP that installed it first.
     */
    if (rdb) {
	linkage = &rdb->isis_rdb_linkage;
	if ((linkage->lsp_index == 0) && (linkage->lsp_prev == NULL)) {
	    isis_add_entry_to_lsp_chain(linkage,
					&lspix_entry->chains[ISIS_ROUTE_IP]);
	    linkage->lsp_index = lsp_index;
	    linkage->lsp_version = lspix_entry->lsp_version;
	} else {
	    /* We got an old rdb. Don't link in. Cross our fingers. */
	    rdb = NULL;
	}
    }
    spf_ebuginf(pdb, TRUE, "Next hop %s/%i (%s) (%s)", 
		station_string(&adj->key[1]), adj->ip_nexthop,
		adj->idb->namestring, rdb ? "accepted" : "rejected");
    return(rdb);
}

/*
 * isis_install_ip_route
 *
 * Install each of the next hops of this route into the forwarding
 * table.
 *
 * If the route cannot be installed, the backuptype entry created by
 * network_update will be updated with the provided backup chain, if
 * any, with the addition of the new route.
 *
 * The supplied backup chain, if any, will be linked into the NDB.
 * This should only be done if we know that the NDB is brand new.
 */
static void isis_install_ip_route (clns_pdbtype *pdb,
				   ipaddrtype address,
				   ipaddrtype mask,
				   isis_lsp_parent *next_hops,
				   ulong metric,
				   ushort route_type,
				   ushort lsp_index,
				   ulong old_metric,
				   ushort old_route_type,
				   isis_backuptype **backup_chain)
{
    clns_adjtype *adj;
    boolean success;
    ulong nexthop_ix;
    rdbtype *rdb;
    ndbtype *ndb;
    backuptype *backup;

    /*
     * Add route to IP forwarding table
     */
    success = FALSE;
    if (next_hops[0].next_hop) {
	spf_ebuginf(pdb, TRUE, "Add %i/%i to IP route table, metric %d",
		    address, mask, metric);
    }

    for (nexthop_ix = 0; nexthop_ix < MAXROUTES; nexthop_ix++) {
	adj = next_hops[nexthop_ix].next_hop;
	if (!adj)
	    break;
	rdb = isis_install_one_ip_route(pdb, address, mask, adj,
					metric, route_type, lsp_index);

	/*
	 * Look up the route to the destination.  It will either be our
	 * new one, or an existing one if our install failed.
	 */
	ndb = ip_rtlookup(address, mask);

	/*
	 * If the install succeeded, link the backup chain, if any, to
	 * the NDB.  If a chain is given to us, there should not be one
	 * in the NDB already;  we free it if we find it.
	 */
	if (rdb) {			/* Installation succeeded */
	    success = TRUE;
	    if (backup_chain) {
		if (ndb->ndb_isis_backup) {	/* Uhoh */
		    spf_ebuginf(pdb, TRUE, "NDB backup chain found on %x",
				ndb);
		    isis_delete_backup_chain(pdb, &ndb->ndb_isis_backup);
		}
		isis_attach_backup_chain(backup_chain, &ndb->ndb_isis_backup);
	    }

	} else {

	    /* Route install failed.  See if there's a backup entry. */

	    ndb = ip_rtlookup(address, mask);
	    if (ndb) {
		backup = check_backup_info(pdb->ip_pdb, ndb);
		if (backup) {

		    /*
		     * Backup entry there.  First attach the remaining backup
		     * chain that we were passed, if any.  Then build a new
		     * backup entry for our failed installation.
		     */
		    if (backup_chain) {
			isis_attach_backup_chain(backup_chain,
					    &backup->isis_backup_context_info);
		    }
		    isis_install_backup(pdb, metric, lsp_index, ISIS_ROUTE_IP,
					&backup->isis_backup_context_info);
		}
	    }
	}
    }

    /* If we haven't disposed of the backup chain yet, just dump it. */

    if (backup_chain && *backup_chain) {
	isis_delete_backup_chain(pdb, backup_chain);
    }

    if (success) {
	/*
	 * If this is a level 1 route and we're running L1L2, we
	 * may need to update a summary (or the entire route) in
	 * a level-2 LSP.
	 */
	if (pdb->level == ISIS_CIRCUIT_L1L2 && route_type == RTYPE_L1) {
	    isis_ip_process_summary(pdb, address, metric, old_metric,
				    old_route_type);
	}
    }
}


/*
 * isis_add_ip_route
 *
 * Add routes to IP forwarding table.
 */
static void isis_add_ip_route (clns_pdbtype *pdb, ipaddrtype *dest,
			       isis_lsp_parent *next_hops, ulong metric,
			       int level, isis_lspdbtype *current_lsp)
{

    ndbtype      *ndb;
    ipaddrtype   ip_address;
    ipaddrtype   mask;
    ulong        old_metric;
    ushort	 route_type, old_route_type;
    boolean      install_backup, matched_nexthop;
    rdbtype	 *rdb;
    isis_lspix_type *lspix_entry;
    ulong	 rdb_ix, nexthop_ix;
    clns_adjtype *nexthop_adj;
    isis_backuptype *backup_chain;

    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, current_lsp->lsp_index);

    /*
     * Store IP destination and mask in local stack storage.
     */
    ip_address = GETLONG(dest);
    dest++;
    mask = GETLONG(dest);

    route_type = (level == ISIS_CIRCUIT_L1) ? RTYPE_L1 : RTYPE_L2;

    /* Look up any existing route (exact match). */

    ndb = ip_rtlookup(ip_address, mask);
    if (ndb) {

	/* See if the existing route is an IS-IS route. */

	old_metric = ndb->metric;
	old_route_type = ndb->route_type;
	if (ip_own_ndb(ndb, pdb->ip_pdb)) {

	    /*
	     * Find matching next hop/metric/LSP index RDBs and update the
	     * LSP version number therein.  If all of our next-hops are
	     * already in the RDB, we're done.
	     */

	    if (metric == old_metric && route_type == old_route_type) {
		install_backup = FALSE;

		/* Check each next hop. */

		for (nexthop_ix = 0; nexthop_ix < MAXROUTES; nexthop_ix++) {
		    nexthop_adj = next_hops[nexthop_ix].next_hop;

		    if (nexthop_adj == NULL)	/* All done. */
			break;

		    /* Check each RDB. */

		    matched_nexthop = FALSE;
		    for (rdb_ix = 0; rdb_ix < MAXROUTES; rdb_ix++) {
			rdb = ndb->rdb[rdb_ix];
			if (!rdb)
			    break;

			/* Next hop IP address and interface the same? */

			if ((nexthop_adj->ip_nexthop == rdb->gateway) &&
			    (nexthop_adj->idb == rdb->idb)) {

			    matched_nexthop = TRUE;

			    /* LSP index the same? */

			    if (current_lsp->lsp_index ==
				rdb->isis_rdb_linkage.lsp_index) {

				/* Yep.  Update the version. */
				
				rdb->isis_rdb_linkage.lsp_version =
				    lspix_entry->lsp_version;

			    } else {	/* LSP index mismatch */

				/*
				 * The next hop is the same but the LSP
				 * index didn't match.  This means that we
				 * have multiple LSPs that have the same
				 * destination with the same metric and
				 * next-hop.  Install a backup entry for
				 * this one.
				 */
				install_backup = TRUE;
			    }
			}		/* if (same nexthop) */
		    }			/* for (each rdb) */

		    /* If we didn't match this next-hop, install it. */

		    if (!matched_nexthop) {
			spf_ebuginf(pdb, TRUE, "Add next hop to IP route "
				    "%i/%i, metric %d", ip_address, mask,
				    metric);
			isis_install_one_ip_route(pdb, ip_address, mask,
						  nexthop_adj, metric,
						  route_type,
						  current_lsp->lsp_index);
		    }
		}			/* for (each nexthop) */

		/* If we need to install a backup entry, do so. */

		spf_ebuginf(pdb, TRUE, "Redundant IP route %i/%i, metric %d, "
			    "not added", ip_address, mask, metric);

		if (install_backup) {
		    spf_ebuginf(pdb, TRUE, "Backup installed");
		    isis_install_backup(pdb, metric, current_lsp->lsp_index,
					ISIS_ROUTE_IP, &ndb->ndb_isis_backup);
		}

	    } else {			/* Route isn't the same */

		/* Create some booleans so that the code is clearer. */

		boolean metric_better, level_better, level_same;
		boolean default_better;

		/* Determine metric improvement. */

		metric_better = (metric < old_metric);

		/* Determine level improvement. */

		level_better = (route_type == RTYPE_L1 &&
				old_route_type == RTYPE_L2);
		level_same = (route_type == old_route_type);

		/* Determine a better kind of default route. */

		default_better = (ip_address == 0 && mask == 0 &&
				  (!ndb->rdb[0] ||
				   ndb->rdb[0]->isis_rdb_linkage.lsp_index ==
				   0));

		/* Is the new route better? */

		if (level_better || default_better ||
		    (level_same && metric_better) ||
		    (metric_better && old_metric == METRIC_INACCESSIBLE)) {

		    /*
		     * New route is "better" than the old route.  Level-1
		     * routes take precedence, regardless of metric.  A
		     * nearest-level-2 default is always worse than a specific
		     * default route.  If the level is the same, the better
		     * metric wins.  If the old metric was infinity, the
		     * better metric wins regardless of level.
		     */
		    spf_ebuginf(pdb, TRUE, "IP route to %i/%i has better "
				"metric (%d/%d) or route type (%d/%d)",
				ip_address, mask, metric, old_metric,
				route_type, old_route_type);
		    
		    backup_chain = NULL;
		    
		    /* Install a backup entry for the current route. */
		    
		    isis_backup_ndb(pdb, ndb, &backup_chain);
		    
		    /* Delete the route. */
		    
		    ip_route_delete(pdb->ip_pdb, ndb->netnumber, ndb->netsmask,
				    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
		    
		    /* Install the new one. */
		    
		    isis_install_ip_route(pdb, ip_address, mask, next_hops,
					  metric, route_type,
					  current_lsp->lsp_index,
					  old_metric, old_route_type,
					  &backup_chain);
		    
		} else {			/* New route is worse */

		    /*
		     * New route is "worse" than the old route.  We create a
		     * backup entry for it.
		     */
		    spf_ebuginf(pdb, TRUE, "IP route to %i/%i has worse "
				"metric (%d/%d) or route type (%d/%d)",
				ip_address, mask, metric, old_metric,
				route_type, old_route_type);
		    isis_install_backup(pdb, metric, current_lsp->lsp_index,
					ISIS_ROUTE_IP, &ndb->ndb_isis_backup);
		}
	    }

	} else {			/* Not an ISIS route */

	    /*
	     * There's an existing route, and it's not an ISIS route.
	     * Attempt to install the new one.  If the install fails
	     * because the existing route has a better admin distance,
	     * isis_install_ip_route will create the appropriate backup
	     * entry.
	     */
	    isis_install_ip_route(pdb, ip_address, mask, next_hops,
				  metric, route_type, current_lsp->lsp_index,
				  METRIC_INACCESSIBLE, RTYPE_OTHER, NULL);
	}

    } else {				/* No existing route */

	/*
	 * No existing route.  Install our route.
	 */
	isis_install_ip_route(pdb, ip_address, mask, next_hops,
			      metric, route_type, current_lsp->lsp_index,
			      METRIC_INACCESSIBLE, RTYPE_OTHER, NULL);
    }
}

/*
 * isis_ip_show_interfaces
 *
 * Display interfaces that IS-IS is enabled on for IP.
 */
static void isis_ip_show_interfaces (pdbtype *pdb)
{

    clns_pdbtype *clns_pdb;
    idbqueuetype *idbq;

    clns_pdb = pdb->osi_pdb;
    if (!clns_pdb) return;

    for (idbq = clns_pdb->ip_idbq.qhead; idbq; idbq = idbq->next) {
	printf("\n    %s", idbq->idb->namestring);
    }
}



/*
 * isis_ip_add_default_route
 *
 * Add network 0.0.0.0 to the IP routing table, pointing at the nearest
 * level-2 router(s).
 * 
 * Note that a default route generated here has the special property of
 * having an LSP index of 0 in the RDB.  We use this to discriminate
 * our old routes generated herein from ISIS routes installed because
 * somebody else announced default in their LSPs.
 */
static void isis_ip_add_default_route (clns_pdbtype *pdb)
{

    ndbtype      *ndb;
    clns_adjtype *adj;
    pdbtype	 *ip_pdb;
    isis_lspdbtype *lsp;
    int          lsp_ix, adj_ix;
    int		 next_hops_installed;

    /* Look up any existing default route. */

    ip_pdb = pdb->ip_pdb;
    ndb = ip_rtlookup(0, 0);
    if (ndb) {				/* There's one present */

	/* Is it an IS-IS route? */
	
	if (ip_own_ndb(ndb, ip_pdb)) {

	    /*
	     * ISIS route.  If it isn't one of ours (it came from a real
	     * LSP), it has precedence.
	     */
	    if (ndb->rdb[0] && (ndb->rdb[0]->isis_rdb_linkage.lsp_index != 0))
		return;			/* An explicit route, bail! */

	    /*
	     * It's one of ours.  Delete it.  This will only happen during
	     * a full SPF run, which doesn't happen very often any more.
	     */
	    ip_route_delete(pdb->ip_pdb, IPADDR_ZERO,
			    IPADDR_ZERO, IPADDR_ZERO,
			    IPADDR_ZERO, NULL, 0);
	}
    }

    /*
     * Bail out if we aren't going to install a new one.
     * Could happen because we haven't found a router with the ATT bit set.
     * Or because we are a L1L2 router that is attached itself.
     */
    if (!pdb->isis_nr_best_L2s)
	return;

    if ((pdb->level != ISIS_CIRCUIT_L1) && isis_is_L2_attached(pdb))
	return;

    if (pdb->ignore_att_bit_ip)
	return;

    /*
     * Attempt to install the new one.  If there's something in the routing
     * table with better administrative distance, our route will be rejected
     * (no big deal).  network_update will leave the LSP linkage
     * information (including the LSP index) as all zeroes, which is how
     * we want it.
     */
    spf_ebuginf(pdb, TRUE, "Add 0.0.0.0 (%d L2 router(s)) to IP route table, "
		"metric %d", pdb->isis_nr_best_L2s, pdb->isis_best_L2_cost);

    next_hops_installed = 0;
    for (lsp_ix = 0; lsp_ix < pdb->isis_nr_best_L2s; lsp_ix++) {
	lsp = isis_lspix_to_lsp(pdb, pdb->isis_best_L2s[lsp_ix]);
	if (!lsp) continue;

	for (adj_ix = 0; adj_ix < lsp->parent_count; adj_ix++) {
	    adj = lsp->parents[adj_ix].next_hop;
	    if (!adj) continue;

	    /*
	     * Don't try to install more the MAXROUTES next-hops.
	     * If different L1L2 routers share the same next-hops
	     * we have bad luck, and will install less then MAXROUTES
	     * next-hops. So be it.
	     */
	    if (next_hops_installed >= MAXROUTES) return;

	    spf_ebuginf(pdb, TRUE, "Next hop %s/%i (%s)", 
			station_string(&adj->key[1]), adj->ip_nexthop,
			adj->idb->namestring);
	    if (network_update(pdb->ip_pdb, IPADDR_ZERO, IPADDR_ZERO,
			       adj->ip_nexthop, adj->idb,
			       pdb->isis_best_L2_cost, 0, adj->ip_nexthop,
			       0, NULL, RTYPE_L1)) {
		next_hops_installed++;
		spf_ebuginf(pdb, TRUE, "(accepted)");
	    } else {
		spf_ebuginf(pdb, TRUE, "(rejected)");
	    }
	}
    }
}


/*
 * isis_summary_address_command
 *
 * Process:
 * [no] isis summary-address <ip-address> <ip-mask> [level-1|level-2|level1-2]
 *
 * Add/Delete address summary entries. These entries configure how IS-IS does
 * route aggregation. If level-2 is specified, level-1 learned IP routes or
 * redistributed routes (from other protocols into level-2) are summarized 
 * into the level-2 backbone with the configured address/mask. If level-1 is 
 * specified, IP routes that are redistributed into a level-1 area use the 
 * configured address/mask.
 *
 * level-2 routes are not summarized into level-1.
 */
void isis_summary_address_command (parseinfo *csb)
{

    isis_addrsumtype *summary;
    clns_pdbtype     *pdb;
    ipaddrtype       address, mask;
    int              level, sumlevel;

    if (!(pdb = (clns_pdbtype *) csb->protocol)) {
	printf("\nMust specify a routing protocol - isis");
	return;
    }
    if (pdb->family != PDB_OSI) return;

    /*	
     * Get IP address.
     */
    address = GETOBJ(paddr, 1)->ip_addr;
    level = 0;
    sumlevel = 0;
    mask = 0;

    /*
     * Don't process rest of parameter list if a "no" was entered.
     */
    if (csb->sense) {

	/*
	 * Get IP mask.
	 */
	mask = GETOBJ(paddr,2)->ip_addr;

	/*
	 * Get level, default ISIS_CIRCUIT_L2.
	 */
	level = GETOBJ(int,1);
	if (!level) level = ISIS_CIRCUIT_L2;
    }

    /*
     * Find existing entry.
     */
    for (summary = pdb->ip_address_sumq.qhead; summary;
	 summary = summary->next) {
	if (summary->sum_address == address) break;
    }

    if (csb->sense) {
	if (!summary) {
	    if (!(summary = malloc_named(sizeof(isis_addrsumtype),
					 "ISIS IP addrsum"))) return;
	    enqueue(&pdb->ip_address_sumq, summary);
	}
	summary->sum_address = address & mask;
	summary->sum_mask = mask;
	sumlevel = (level | summary->sum_level); /* might be change in level*/
	summary->sum_level = level;
	summary->sum_L2_metric = METRIC_INACCESSIBLE;
	summary->sum_metric = METRIC_INACCESSIBLE;
    } else {
	if (summary) {
	    sumlevel = summary->sum_level;
	    unqueue(&pdb->ip_address_sumq, summary);
	    free(summary);
	}
    }

    /*
     * Rebuild level-2 LSP, after running level-1 Dijkstra. Set change
     * flag to indicate that a level-2 LSP must be built.
     */
    if (sumlevel & ISIS_CIRCUIT_L1)
	isis_mark_L1_changed(pdb);

    if ((pdb->level & ISIS_CIRCUIT_L2) && (sumlevel & ISIS_CIRCUIT_L2))
	isis_mark_L2_changed(pdb);
}


/*
 * isis_ip_show_summary
 *
 * Display address summary configuration in "show ip protocol" display.
 */
static void isis_ip_show_summary (pdbtype *ip_pdb)
{

    isis_addrsumtype *sum;
    clns_pdbtype *pdb;
    
    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    printf("\n  Address Summarization:");
    if (!pdb->ip_address_sumq.count) {
	printf("\n    None");
	return;
    }

    for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
	printf("\n    %i/%i into ", sum->sum_address, sum->sum_mask);
	if (sum->sum_level != ISIS_CIRCUIT_L1L2) {
	    printf("%s ", (sum->sum_level == ISIS_CIRCUIT_L1) ? isis_L1_str :
		    isis_L2_str);
	} else {
	    printf("level-1 and level-2");
	}
    }
}    

/*
 * isis_ip_routemap_level_check
 *
 * Check level against route-map. If not set, we should default to
 * level-2.
 */
static boolean isis_ip_routemap_level_check (int level, route_maptype *map,
					     int redist_level)
{

    /*
     * When there is a route-map with no set-level, we need
     * to consider the level specified via the redist command.
     * 
     * As the default level for both the route-map and redist
     * are same, the next best thing to do if set-level is
     * not specified *and* redist-level is specified is to 
     * check with the redist-level.
     *
     * If there is no set-level and redist-level, check the
     * default set-level (ie L2 only). This will happen when
     * we are checking for default route injection.
     */
    if (!(map->set_list.set_flags & ROUTE_MAP_LEVEL_SET)) {
	if (redist_level) {
	    if (redist_level & level)
		return(TRUE);
	} else if (level != ISIS_CIRCUIT_L1)
	    return(TRUE);

	return(FALSE);
    }

    if (level == ISIS_CIRCUIT_L1 && 
	(map->set_list.level == ROUTE_MAP_SET_LEVEL1 ||
	 map->set_list.level == ROUTE_MAP_SET_LEVEL12)) return(TRUE);

    if (level == ISIS_CIRCUIT_L2 && 
	(map->set_list.level == ROUTE_MAP_SET_LEVEL2 ||
	 map->set_list.level == ROUTE_MAP_SET_LEVEL12)) return(TRUE);

    return(FALSE);
}


/*
 * isis_construct_ip_default
 *
 * Add 0.0.0.0 to an level-2 LSP. A default route can be added to a level-1
 * LSP if a route-map is used.
 *
 * This is done in reaction to the "default-information originate" command.
 */

static void isis_construct_ip_default (isis_lspcontext *context)
{
    
    route_maptype  *map;
    ulong	metric;
    pdbtype   *pdb;
    
    pdb = context->pdb->ip_pdb;
    
    if (!pdb->default_originate) return;
    
    /*
     * Check for conditional default generation.
     */
    if (!pdb_routemap_for_default(pdb)) return;
    metric = 0;
    map = pdb->route_map;
    
    if (map) {
 	if (!isis_ip_routemap_level_check(context->level, map, 0)) return;
	
 	if (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) {
	    metric = map->set_list.metric.value.scalar;
	    if (metric > ISIS_MAX_LINK_METRIC)
		metric = ISIS_MAX_LINK_METRIC;
 	}
 	if (map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) {
 	    if (map->set_list.metric_type == ROUTE_MAP_SET_EXTERNAL) {
 		metric |= 0x80;
 	    }
 	}
    } else {
 	if (context->level == ISIS_CIRCUIT_L1) return;
    }
    
    if (!isis_add_one_ip_entry(context, 0, 0, metric))
	return;
}
/*
 * isis_ip_set_adv_type
 *
 * Set the advertisement type in the LSP if it needs to change.
 *
 * Returns TRUE if succeeded, FALSE if out of memory.
 */
static boolean isis_ip_set_adv_type (isis_lspcontext *context,
				     boolean building_externals,
				     boolean external_route)
{
    /* If the type is changing, close off the last item and start a new one. */

    if ((external_route && !building_externals) ||
	(!external_route && building_externals)) {
	isis_item_done(context, NULL);
	if (!isis_new_item(context, external_route ?
			   ISIS_IP_EXTERNAL_ADDR : ISIS_IP_INTERNAL_ADDR,
			   0, NULL, NULL, TRUE)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * isis_construct_ip_adv
 *
 * At level 1, advertise all routes redistributed into level-1.  Note
 * that this does not include ISIS interface routes.
 *
 * At level 2, advertise all routes redistributed into level-2, plus
 * summaries (or explicit routes if there are no summaries) of
 * all level-1 ISIS routes, including level-1 ISIS interface routes.
 *
 * The default route is added here as required.
 */
static void isis_construct_ip_adv (isis_lspcontext *context)
{
    ndbtype          *db, *ndbptr, *sdbptr;
    int		     ndb_bucket, sdb_bucket;
    isis_addrsumtype *sum;
    pdbtype          *ip_pdb;
    ulong            metric;
    route_maptype    *map;
    pdbtype 	     *src_pdb;
    clns_pdbtype     *pdb;
    int		     level, redist_level;
    boolean	     building_externals;
    boolean	     external_route;
    boolean	     isis_route;
    boolean	     interface_route;
    isisidbtype      *isisidb;
    
    level = context->level;
    pdb = context->pdb;
    
    if (!isis_new_item(context, ISIS_IP_EXTERNAL_ADDR, 0, NULL, NULL, TRUE))
	return;
    building_externals = TRUE;
    
    /*
     * Clean up all summary metrics, since we will recalculate them all
     * on the fly.
     */
    for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
	sum->sum_metric = METRIC_INACCESSIBLE;
	if (level == CLNS_LEVEL2)	/* Only on L2 builds! */
	    sum->sum_L2_metric = METRIC_INACCESSIBLE;
    }
    
    ip_pdb = pdb->ip_pdb;
    
    /*
     * Insert 0.0.0.0 into LSP, if "default-information originate" is
     * configured.
     */
    isis_construct_ip_default(context);
    
    ndbptr = NULL;
    sdbptr = NULL;
    ndb_bucket = 0;
    sdb_bucket = 0;
    
    /*
     * Scan through IP routing table.
     */
    while ((db = geteachroute(&ndbptr, &ndb_bucket, &sdbptr,
			      &sdb_bucket, FALSE)) != NULL) {
	/*
	 * Skip default route.
	 */
	if (!db->netnumber) continue;
	
	/*
	 * Don't consider routes that are in holddown.
	 */
	if (db->metric == METRIC_INACCESSIBLE) continue;

	/*
	 * Don't consider routes that aren't to be redistributed.
	 */
	if (db->attrflags & NET_NOREDIST)
	    continue;

	/*
	 * If it's an L1 ISIS route, or an L1 or L1L2 interface route,
	 * and this is level 2, update the summary metric, or advertise
	 * the route if there is no summary.
	 */
	isis_route = ip_own_ndb(db, ip_pdb);
	interface_route = ((db->attrflags & NET_ATTACHED) &&
			   (NDB2PROCTYPE(db) == PROC_CONNECTED) &&
			   isis_checkqueue(&pdb->ip_idbq, db->lastinterface));
	
	if (isis_route || interface_route) {
	    if (level != CLNS_LEVEL2)
		continue;		/* Ignore these at level 1 */
	    if (isis_route && db->route_type != RTYPE_L1)
		continue;		/* Ignore L2 ISIS routes */

	    /*
	     * Determine if there is an address summary associated
	     * with this destination.  If so, update the metric of the
	     * summary for later advertisement.  If there is no summary,
	     * advertise the route now.
	     *
	     * Make sure that the L1 metric is not more than 
	     * MAX_LINK_METRIC.
	     */
	    isisidb = get_isis_idb(pdb, db->lastinterface);
	    metric = ((db->attrflags & NET_ATTACHED) && isisidb) ? 
		isisidb->isis_L1_metric : db->metric;
	    if (metric > ISIS_MAX_LINK_METRIC)
		metric = ISIS_MAX_LINK_METRIC;

	    sum = isis_ip_get_summary(pdb, db->netnumber);
	    if (sum && (sum->sum_level & ISIS_CIRCUIT_L2)) {
		isis_ip_update_summary_metric(sum, metric);
		continue;		/* Don't advertise now. */
	    }
	    external_route = FALSE;	/* Advertise this one as internal. */

	} else {

	    /*
	     * Only consider routes derived from protocols we are
	     * redistributing for.  (IS-IS is already excluded).
	     */
	    if (!(ip_redist_flag(db, ip_pdb))) continue;
	    
	    /*
	     * Get source pdb for route. It is INDEX2PDB(db->pdbindex) unless 
	     * it is attached, then it is the best distance pdb.
	     */
	    src_pdb = ip_get_src_pdb(ip_pdb, db, FALSE);
	    
	    /*
	     * Get metric from one on redistribute command.
	     */
	    metric = 0;
	    if (ip_def_metric_configured(ip_pdb, src_pdb->index))
		metric = ip_get_def_metric_scalar(ip_pdb, src_pdb->index);
	    
	    /*
	     * Check if route-maps have allowed the route to be advertised.
	     * If so, use the set values associated with the route-map.
	     */
	    redist_level = ip_pdb->metric[src_pdb->index].type;
	    if (ip_pdb->routemap[src_pdb->index]) {
		if (!pdb_routemap(ip_pdb, src_pdb, db)) continue;
		map = ip_pdb->route_map;
		if (!isis_ip_routemap_level_check(level, map, redist_level))
		    continue;

		if (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) {
		    metric = map->set_list.metric.value.scalar;
		    if (metric > ISIS_MAX_LINK_METRIC)
			metric = ISIS_MAX_LINK_METRIC;
		}
		if (map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) {
		    if (map->set_list.metric_type == ROUTE_MAP_SET_EXTERNAL) {
			metric |= 0x80;
		    }
		}
	    } else {
		if (!(redist_level & level)) 
		    continue;
	    }

	    /*
	     * The redistributed route survived.  If it is part of a summary,
	     * update the summary metric.  Otherwise, advertise it now.
	     */
	    sum = isis_ip_get_summary(pdb, db->netnumber);
	    if (sum && (sum->sum_level & level)) {
		if (metric < sum->sum_metric) sum->sum_metric = metric;
		continue;
	    }

	    external_route = TRUE;	/* Advertise this one as external. */
	}

	/*
	 * We have a route to advertise.  Set the advertisement type and
	 * advertise the route.
	 */
	if (!isis_ip_set_adv_type(context, building_externals, external_route))
	    return;
	building_externals = external_route;
	if (!isis_add_one_ip_entry(context, db->netnumber, db->netsmask,
				   metric))
	    return;
    }

    /*
     * Made it through the routing table.  Now advertise all of the
     * redistributed summaries we calculated.  If this is level 2 and
     * there is an L2 summary metric calculated, skip the advertisement
     * (since we're about to advertise the summary in the next chunk of code).
     */
    if (!isis_ip_set_adv_type(context, building_externals, TRUE))
	return;
    building_externals = TRUE;
    for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
  	if ((sum->sum_metric != METRIC_INACCESSIBLE) &&
	    (level == CLNS_LEVEL1 ||
	     sum->sum_L2_metric == METRIC_INACCESSIBLE)) {
 	    if (!isis_add_one_ip_entry(context, sum->sum_address,
				       sum->sum_mask, sum->sum_metric))
 		return;
  	}
    }
    /*
     * Now advertise the L2 summaries if we're at level 2.
     */
    if (level == CLNS_LEVEL2) {
	if (!isis_ip_set_adv_type(context, building_externals, FALSE))
	    return;
	building_externals = FALSE;
	for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
	    if (sum->sum_L2_metric != METRIC_INACCESSIBLE) {
		if (!isis_add_one_ip_entry(context, sum->sum_address,
					   sum->sum_mask, sum->sum_L2_metric))
		    return;
	    }
	}
    }
    isis_item_done(context, NULL);
}

/*
 * isis_ip_redist_callback
 *
 * This function is called by network_update() when a route has been
 * created or an existing route with better distance or metrics is
 * created. If we are redistributing this protocol, set a flag to
 * rebuild appropriate LSP.
 */
static void isis_ip_redist_callback (pdbtype *pdb, ndbtype *ndb, int event,
				     int flag)
{
    clns_pdbtype  *clns_pdb;
    route_maptype *map;
    pdbtype 	  *src_pdb;
    ulong      	  type;

    /*
     * Processing a route for redistribution.
     */
    if (flag != DEFAULT_ROUTE) {
        if (!ndb) return;
	src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);

	if (!src_pdb)
	    return;

	/*
	 * Check distribute-lists.
	 */
	if (!pdb_outfilter(pdb, NULL, src_pdb, ndb->netnumber))
	    return;

	/*
	 * Check route-maps.
	 */
	if (!pdb_routemap(pdb, src_pdb, ndb)) return;
	type = pdb->metric[ndb->pdbindex].type;

	/*
	 * Set redistribution flag in the ndb. This says some calling overhead
	 * of this function.
	 */
	if ((event == ROUTE_UP) || (event == ROUTE_MODIF)) {
	    ndb->advertisingmask |= pdb->mask;
	} else {
	    ndb->advertisingmask &= ~pdb->mask;
	}
    } else {
	if (!pdb->default_originate) return;
	type = ISIS_CIRCUIT_L2;
	
	/*
	 * Processing default origination.
	 */
	if (event != ROUTE_DOWN && !pdb_routemap_for_default(pdb)) return;
    }
    map = pdb->route_map;

    clns_pdb = pdb->osi_pdb;
    if (!clns_pdb)
	return;

    /*
     * Set redistribution timer to send a single LSP when a lot of
     * new routes have come in.
     *
     * If no route-map, the redist level is used to decide the level,
     * or else use the route_map_level_check to decide the level.
     */
    if ((!map && (type & ISIS_CIRCUIT_L1)) ||
	(map && isis_ip_routemap_level_check(ISIS_CIRCUIT_L1, map, type))) {
	isis_schedule_lsp_build(clns_pdb, CLNS_LEVEL1,
				ISIS_IP_LSP_BUILD_DELAY, FALSE);
	locupdate_buginf(clns_pdb, "L1 IP redist callback");
    }

    if ((!map && (type & ISIS_CIRCUIT_L2)) || 
	(map && isis_ip_routemap_level_check(ISIS_CIRCUIT_L2, map, type))) {
	    isis_schedule_lsp_build(clns_pdb, CLNS_LEVEL2,
				    ISIS_IP_LSP_BUILD_DELAY, FALSE);
	    locupdate_buginf(clns_pdb, "L2 IP redist callback");
	}
}

/*
 * isis_ip_redistribute
 *
 * Process a "redistribute" command for IS-IS.
 */
static void isis_ip_redistribute (pdbtype *pdb, pdbtype *dist, boolean sense)
{

    clns_pdbtype *clns_pdb;
    int          type;


    type = 0;
    if (sense) {
	pdb->redist_callback = isis_ip_redist_callback;
    } else if (pdb->distflag == pdb->mask && !pdb->default_originate) {
	pdb->redist_callback = NULL;
    }

    if (dist) {
	pdb->metric[dist->index].flag = sense;
	type = pdb->metric[dist->index].type; 
    } else {
        type = ISIS_CIRCUIT_L1 | ISIS_CIRCUIT_L2;
    }

    if (sense) {
	if (!type) {
	    type = ISIS_CIRCUIT_L2;
	    if (dist) pdb->metric[dist->index].type = type;
	}
    } else {
	if (dist) pdb->metric[dist->index].type = 0;
    }

    clns_pdb = pdb->osi_pdb;
    if (!clns_pdb)
	return;
    
    /*
     * Always rebuild both the L1 and L2 LSPs. We might be 
     * changing the redistribution from some other level to
     * the current level or might be using a route-map.
     */
    isis_mark_L1_changed(clns_pdb);
    locupdate_buginf(clns_pdb, "L1 IP redist");
    isis_mark_L2_changed(clns_pdb);
    locupdate_buginf(clns_pdb, "L2 IP redist");
}


/*
 * isis_ip_recalculate_all_backup_routes
 *
 * Flag all LSPs on the backup chain to be recalculated.  Delete the
 * backup chain.
 *
 * Returns TRUE if there was an overflow entry on the chain (meaning that
 * the caller must trigger a full SPF).
 */
static boolean isis_ip_recalculate_backup_routes (clns_pdbtype *pdb,
					  isis_backuptype **backup_chain)
{
    isis_backuptype *current_backup;
    boolean full_spf;

    current_backup = *backup_chain;
    full_spf = FALSE;
    while (current_backup) {
	if (current_backup->lsp_index == 0) {
	    full_spf = TRUE;
	} else {
	    isis_mark_lsp_to_recalculate(pdb, current_backup->lsp_index);
	}
	current_backup = current_backup->ndb_next;
    }
    isis_delete_backup_chain(pdb, backup_chain);
    return(full_spf);
}

/*
 * isis_ip_backup_callback
 *
 * An IP route is subject for installation in the IP routing table. This
 * occurs when the IP route was previously attempted to be added, but failed 
 * because there was a route that existed with better administrative distance.
 * Now the route with better AD has gone unreachable.
 *
 * If the route is still valid, run Dijkstra.
 */
static void isis_ip_backup_callback (pdbtype *pdb, ipaddrtype address,
				     ipaddrtype mask, backuptype *backup)
{

    clns_pdbtype *clns_pdb;
    boolean full_spf;
    isis_backuptype *current_backup;

    clns_pdb = pdb->osi_pdb;
    if (!clns_pdb)
	return;

    full_spf = FALSE;

    /*
     * If there's no backup information, trigger a full SPF.
     * This shouldn't happen, but oh well.
     */
    current_backup = backup->isis_backup_context_info;
    if (!current_backup) {
	full_spf = TRUE;
    } else {

	/*
	 * Backup info is there.  Trigger a recalculation of every LSP
	 * on the backup chain.  Then delink the chain.
	 */
	full_spf = isis_ip_recalculate_backup_routes(clns_pdb,
					    &backup->isis_backup_context_info);
    }

    /*
     * Now trigger a full SPF if so indicated.  We do it at both levels,
     * because there may have been lost backup routes from both levels.
     */
    if (full_spf) {
	if (clns_pdb->level & CLNS_LEVEL1) {
	    isis_trigger_spf(clns_pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_IPBACKUP,
			     NULL, TRUE);
	}
	if (clns_pdb->level & CLNS_LEVEL2) {
	    isis_trigger_spf(clns_pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_IPBACKUP,
			     NULL, TRUE);
	}
    }
}

/*
 * isis_ip_redistribute_command_exception
 *
 * If an IS-IS routing process tag is specified in a "redistribute",
 * this function need not be used. Otherwise, it does. Here is the
 * problem.
 *
 *	router rip
 *      redistribute isis level-1
 *
 * The above specifies to redistribute IS-IS level-1 IP routes into
 * RIP. However the routing process tag is NULL. The parser parses
 * this as a tag equal to "level-1". This function handles this exception
 * case. This is important since most people run a single IS-IS process
 * and use a NULL tag.
 *
 * This function process the tag value as the "level" keyword. If it
 * is not found, nothing is done. If found, it sets OBJ(int,10) to the
 * same value the parser strings would have set if the keyword was found
 * somewhere else on the line.
 */
static void isis_ip_redistribute_command_exception (parseinfo *csb)
{
    if (!strcasecmp(isis_L1_str, csb->router_name.tag)) {
	SETOBJ (int, 10) = ISIS_CIRCUIT_L1;
    } else if (!strcasecmp(isis_L2_str, csb->router_name.tag)) {
	SETOBJ (int, 10) = ISIS_CIRCUIT_L2;
    } else if (!strcasecmp(isis_L1L2_str, csb->router_name.tag)) {
	SETOBJ (int, 10) = ISIS_CIRCUIT_L1L2;
    }
}

/*
 * isis_ip_query
 *
 * IP routing table reload has occured. Rerun Dijkstra to place routes
 * back in routing table.
 */
static void isis_ip_query (pdbtype *ip_pdb, idbtype *idb)
{

    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    /*
     * Cause both levels of Dijkstra to be run.
     */
    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_IPQUERY, NULL, TRUE);
    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_IPQUERY, NULL, TRUE);
}

/*
 * isis_ip_rdb_cleanup
 *
 * Clean up an RDB that's about to be freed.
 */
static void isis_ip_rdb_cleanup (pdbtype *ip_pdb, rdbtype *rdb)
{
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    /* Make sure the RDB is delinked from the LSP. */

    isis_delete_entry_from_lsp_chain(&rdb->isis_rdb_linkage);
    rdb->rdb_flags &= ~IP_RDB_ISIS;
    rdb->isis_rdb_ndb = NULL;
}

/*
 * isis_ip_ndb_cleanup
 *
 * Clean up an NDB that's about to be freed.
 */
static void isis_ip_ndb_cleanup (pdbtype *ip_pdb, ndbtype *ndb)
{
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    /* Make sure the backup chain is delinked from the NDB. */

    isis_delete_backup_chain(pdb, &ndb->ndb_isis_backup);
}

/*
 * isis_ip_backup_cleanup
 *
 * Clean up a backup entry that's about to be freed.
 */
static void isis_ip_backup_cleanup (pdbtype *ip_pdb, backuptype *backup)
{
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    /* Make sure the backup chain is delinked from the backup entry. */

    isis_delete_backup_chain(pdb, &backup->isis_backup_context_info);
}

/*
 * isis_ip_build_backup_info
 *
 * Called by network_update when a backup entry is being built for an
 * ISIS route.  If "replaced" is TRUE, an ISIS route is being replaced
 * by a route from another protocol with better administrative distance.
 * If "replaced" is FALSE, an attempt by ISIS to install a route failed
 * because of the presence of a route with better administrative distance.
 */
static void isis_ip_build_backup_info (pdbtype *ip_pdb, ndbtype *ndb,
				       backuptype *backup, ushort route_type,
				       boolean replaced)
{
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    /*
     * If replacing our route, build a backup entry from the NDB contents.
     * This will detach any existing backup chain from the NDB.
     *
     * If our route installation fails, we don't do anything because we don't
     * have enough information to do it (but we catch this case after calling
     * network_update and take care of it there).
     */
    if (replaced) {
	isis_backup_ndb(pdb, ndb, &backup->isis_backup_context_info);
    }
}

/*
 * isis_ip_init_ndb
 * 
 * Initialize ISIS-specific NDB fields
 *
 * Only do it on a new NDB;  we want to preserve the backup chains when we're
 * doing things like adding a next hop.
 */
static void isis_ip_init_ndb (pdbtype *pdb, ndbtype *ndb, boolean new_ndb)
{
    if (new_ndb) {
	ndb->ndb_isis_backup = NULL;
    }
}

/*
 * isis_ip_init_rdb
 *
 * Initialize ISIS-specific RDB fields
 */
static void isis_ip_init_rdb (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb)
{
    rdb->rdb_flags |= IP_RDB_ISIS;
    rdb->isis_rdb_ndb = ndb;
}

/*
 * isis_ip_ndb_copy
 *
 * Perform ISIS-specific NDB copy procedures
 *
 * Assumes that the entire NDB has already been copied
 */
static void isis_ip_ndb_copy (pdbtype *ip_pdb, ndbtype *oldndb,
			      ndbtype *newndb)
{
    ulong rdb_ix;
    rdbtype *rdb;

    isis_attach_backup_chain(&oldndb->ndb_isis_backup,
			     &newndb->ndb_isis_backup);
    for (rdb_ix = 0; rdb_ix < newndb->routecount; rdb_ix++) {
	rdb = newndb->rdb[rdb_ix];
	if (rdb->rdb_flags & IP_RDB_ISIS) {
	    rdb->isis_rdb_ndb = newndb;
	}
    }
}

/*
 * isis_ip_ndb_pdbchange
 *
 * Perform protocol-specific actions when owner of NDB changes
 */
static void isis_ip_ndb_pdbchange (pdbtype *ip_pdb, ndbtype *ndb, rdbtype *rdb,
				   boolean now_ours)
{
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    if (now_ours) {

	/* New one is ISIS. */
	
	isis_ip_init_ndb(ip_pdb, ndb, TRUE);
	if (rdb)
	    memset(&rdb->proto_rdb, 0, sizeof(rdb->proto_rdb));

    } else {

	/* Old one is ISIS. */

	isis_delete_backup_chain(pdb, &ndb->ndb_isis_backup);
	if (rdb)
	    isis_delete_entry_from_lsp_chain(&rdb->isis_rdb_linkage);
    }
}

/*
 * isis_ip_ndb_print
 *
 * Print the contents of the backup chain for this NDB
 */
#define BACKUPS_PER_LINE 4		/* This many before a new line */
static void isis_ip_ndb_print (pdbtype *ip_pdb, ndbtype *ndb)
{
    isis_backuptype *backup;
    ulong backup_count;
    clns_pdbtype *pdb;

    pdb = ip_pdb->osi_pdb;
    if (!pdb)
	return;

    if (!pdb->long_route_display)
	return;

    isis_validate_ndb_backup_chain(&ndb->ndb_isis_backup);
    backup = ndb->ndb_isis_backup;
    if (backup) {
	printf("\n        Backup ix/lvl/metric:");
	backup_count = 0;
	while (backup) {
	    if (backup_count && (!(backup_count % BACKUPS_PER_LINE))) {
		printf("\n                             ");
	    }
	    if (backup->lsp_index) {
		printf("  %d", backup->lsp_index);
	    } else {
		printf("  *");
	    }
	    printf("/L%d/%d", ISIS_BACKUP_LEVEL(backup->metric),
		   ISIS_BACKUP_BASE_METRIC(backup->metric));
	    backup = backup->ndb_next;
	    backup_count++;
	}
    }
}

/*
 * isis_ip_rdb_print
 *
 * sprintf the ISIS-specific information for an RDB.
 */
static int isis_ip_rdb_print (pdbtype *ip_pdb, rdbtype *rdb, char *bp)
{
    clns_pdbtype *pdb;

    *bp = '\0';
    pdb = ip_pdb->osi_pdb;

    if (pdb && pdb->long_route_display)
	return(sprintf(bp, ", from LSP %d", rdb->isis_rdb_linkage.lsp_index));
    else
	return(0);
}

/*
 * isis_ip_lsp_link_to_rdb
 *
 * Returns an RDB pointer, given a pointer to the LSP link information
 * embedded therein
 */
static inline rdbtype *isis_ip_lsp_link_to_rdb (isis_lsp_linkage *link)
{
    rdbtype *rdb;
    if (link == NULL)
	return(NULL);
    rdb = (rdbtype *) ((uchar *)link - offsetof(rdbtype, isis_rdb_linkage));
    return(rdb);
}

/*
 * ip_isis_age_routes
 *
 * Age the IP routes on an LSP
 *
 * Called after the routes associated with an LSP are calculated
 */
static void ip_isis_age_routes (clns_pdbtype *pdb, ushort lsp_index)
{
    isis_lspix_type *lspix_entry;
    isis_lsp_linkage *link_entry;
    isis_lsp_linkage **prev_link;
    rdbtype *rdb;
    ndbtype *ndb;
    isis_backuptype *current_backup;
    boolean backup_found;
    int rdb_index;
    ulong old_metric, metric;
    ushort route_type;
    ipaddrtype address;
    isis_cleanup_queue *cleanup;

    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp_index);
    if (!lspix_entry)			/* Can't happen */
	return;

    /* Traverse the RDB chain looking for expired entries. */

    link_entry = lspix_entry->chains[ISIS_ROUTE_IP];

    while (link_entry) {

	/* Check to see if the entry is expired. */

	if (link_entry->lsp_version != lspix_entry->lsp_version) {

	    /*
	     * Entry is expired.  Extrapolate the RDB pointer from the
	     * linkage pointer.
	     */
	    rdb = isis_ip_lsp_link_to_rdb(link_entry);
	    ndb = rdb->isis_rdb_ndb;
	    old_metric = ndb->metric;
	    address = ndb->netnumber;
	    route_type = ndb->route_type;

	    spf_ebuginf(pdb, TRUE, "Aging IP %i/%i, next hop %i",
			ndb->netnumber, ndb->netsmask, rdb->gateway);

	    /* Delink the RDB from the LSP chain. */

	    prev_link = link_entry->lsp_prev;
	    isis_delete_entry_from_lsp_chain(link_entry);

	    /* Delete the RDB. */

	    rdb_index = ip_find_rdb_index(ndb, rdb);
	    if (rdb_index >= 0) {	/* This better have worked... */
		rdb_delete(pdb->ip_pdb, ndb, rdb_index);
	    } else {
		spf_ebuginf(pdb, TRUE, "Can't find RDB index");
		rdb_free(rdb, TRUE);		/* Don't lose it, at least. */
	    }

	    /*
	     * If there are any next-hops left in the NDB, try to find
	     * backup entries at the same metric (extra next-hops would
	     * have ended up here).  If there are no next-hops left,
	     * find all backup entries at the next higher metric.  Trigger
	     * a route recalculation for those backup entries and delete
	     * them.  If the backup entry is the overflow entry, trigger
	     * an SPF.
	     */
	    if (ndb->routecount) {
		metric = ndb->metric;
		if (route_type == RTYPE_L2)
		    metric |= ISIS_BACKUP_L2_METRIC_BIT;
	    } else {
		if (ndb->ndb_isis_backup) {
		    metric = ndb->ndb_isis_backup->metric;
		} else {
		    metric = 0;		/* For completeness */
		}
	    }
	    backup_found = FALSE;
	    while ((current_backup = ndb->ndb_isis_backup)) {
		backup_found = TRUE;
		if (current_backup->metric != metric)
		    break;		/* No more at this metric */
		if (current_backup->lsp_index) { /* A real backup entry */
		    isis_mark_lsp_to_recalculate(pdb,
						 current_backup->lsp_index);
		} else {		/* An overflow entry */
		    if (pdb->level & CLNS_LEVEL1) {
			isis_trigger_spf(pdb, CLNS_LEVEL1,
					 ISIS_SPF_TRIG_OVERFLOWHIT,
					 NULL, TRUE);
		    }
		    if (pdb->level & CLNS_LEVEL2) {
			isis_trigger_spf(pdb, CLNS_LEVEL2,
					 ISIS_SPF_TRIG_OVERFLOWHIT,
					 NULL, TRUE);
		    }
		}
		isis_delete_backup_entry(pdb, current_backup);
	    }
	    
	    /*
	     * If all RDBs are gone but some backups were found,
	     * create a cleanup entry.  If all RDBs are gone and no backups
	     * were found, delete the NDB.
	     */
	    if (!ndb->routecount) {
		if (backup_found) {
		    spf_ebuginf(pdb, TRUE, "Created cleanup queue entry");
		    cleanup = isis_create_cleanup_queue_entry(pdb);
		    if (cleanup) {
			cleanup->route_type = ISIS_ROUTE_IP;
			cleanup->cleanup_ip_addr = ndb->netnumber;
			cleanup->cleanup_ip_mask = ndb->netsmask;
		    }
		} else {
		    spf_ebuginf(pdb, TRUE, "Deleted NDB");
		    ip_route_delete(pdb->ip_pdb, ndb->netnumber, ndb->netsmask,
				    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
		}
	    }

	    /*
	     * If we're L1L2 and this is an L1 route going away, update
	     * the level 2 summary metrics/routes as appropriate.
	     */
	    if (pdb->level == ISIS_CIRCUIT_L1L2 && route_type == RTYPE_L1) {
		isis_ip_process_summary(pdb, address, METRIC_INACCESSIBLE,
					old_metric, RTYPE_L1);
	    }
	    link_entry = *prev_link;
	} else {			/* Version is fine */
	    link_entry = link_entry->lsp_next;
	}
    }
}

/*
 * isis_cleanup_ip_route
 *
 * Clean up an IP route if we have stranded it with no next hops.
 *
 * Called at the end of route calculation to pick up stragglers that
 * were supposed to have backup entries but ultimately did not.
 */
static void isis_cleanup_ip_route (clns_pdbtype *pdb, ipaddrtype address,
			    ipaddrtype mask)
{
    ndbtype *ndb;

    /* Look up the route. */

    ndb = ip_rtlookup(address, mask);
    if (ndb) {

	/* If the entry has no next hops, delete it. */

	if (!ndb->routecount) {
	    spf_ebuginf(pdb, TRUE, "Deleted IP NDB %i/%i via cleanup entry",
			address, mask);
	    ip_route_delete(pdb->ip_pdb, address, mask,
			    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
	}
    }
}

/*
 * isis_ip_init
 *
 * Allocate an IP pdb for the IS-IS protocol. There will not be
 * an IP related process running. The IS-IS protocol and state information
 * to operate runs out of the CLNS pdb.
 */
static pdbtype *isis_ip_init (clns_pdbtype *clns_pdb)
{

    pdbtype *pdb;

    /*
     * Create the pdb.
     */
    pdb = malloc_named(sizeof(pdbtype), "ISIS IP PDB");
    if (!pdb) {
	printf("\nunable to allocate memory for IS-IS pdb.");
	return(NULL);
    }

    /*
     * Allocate memory for input queue.
     */
    pdb->inputq = malloc_named(sizeof(queuetype), "ISIS IP queue");
    if (!pdb->inputq) {
	printf(nomemory);
	free(pdb);
	return(NULL);
    }

    /*
     * Allocate an unused router process flag.
     */
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb->inputq);
	free(pdb);
	return(NULL);
    }	

    /*
     * Fill in pdb fields related to IS-IS.
     */
    pdb->name = "isis";
    pdb->family = PDB_IP;
    pdb->pname = "IS-IS Router";
    pdb->process = NULL;
    pdb->running = TRUE;
    pdb->cleanup = NULL;
    pdb->majorupdate = NULL;
    pdb->holddown = NULL;
    pdb->query = isis_ip_query;
    pdb->redistribute = isis_ip_redistribute;
    pdb->add_route = isis_ip_backup_callback;
    pdb->build_backupinfo = isis_ip_build_backup_info;
    pdb->show_summary = isis_ip_show_summary;
    pdb->malloc_backup = TRUE;
    pdb->proctype = PROC_ISIS;
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->atomic_update = TRUE;
    pdb->multipath = pdb->def_multipath = DEF_MAXROUTES;
    pdb->distance = pdb->def_distance = ISIS_DISTANCE;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->rdb_cleanup = isis_ip_rdb_cleanup;
    pdb->ndb_cleanup = isis_ip_ndb_cleanup;
    pdb->backup_cleanup = isis_ip_backup_cleanup;
    pdb->ndb_init = isis_ip_init_ndb;
    pdb->rdb_init = isis_ip_init_rdb;
    pdb->ndb_copy = isis_ip_ndb_copy;
    pdb->ndb_pdbchange = isis_ip_ndb_pdbchange;
    pdb->ndb_print = isis_ip_ndb_print;
    pdb->rdb_print = isis_ip_rdb_print;

    /*
     * Put on IP protocol queue.
     */
    enqueue(&protoQ, pdb);

    /*
     * Set appropriate fields in CLNS pdb.
     */
    clns_pdb->isis_mode |= ISIS_IP_MODE;
    pdb->osi_pdb = clns_pdb;

    return(pdb);
}

/*
 * isis_ip_get_pdb
 *
 * Gets IS-IS IP pdb from clns pdb. If it does not exist, an IP
 * pdb is created.
 */
static pdbtype *isis_ip_get_pdb (clns_pdbtype *clns_pdb)
{
    pdbtype *pdb;

    /* 
     * First check that CLNS pdb is owned by IS-IS.
     */
    if (!(clns_pdb->typeflag & ISO_ISISMASK)) {
	return(NULL);
    }

    pdb = clns_pdb->ip_pdb;
    if (!pdb) {
	pdb = isis_ip_init(clns_pdb);
	clns_pdb->ip_pdb = pdb;
    }
    return(pdb);
}

/*
 * ip_router_command
 *
 * Process [no] ip router isis <tag> command.
 *
 * This command enables/disables IS-IS for IP on the specified interface.
 */

void ip_router_command (parseinfo *csb)
{

    clns_pdbtype *pdb;
    idbqueuetype *idbq;
    idbtype      *idb;
    char	 *name;
    isisidbtype  *isisidb;

    idb = csb->interface;

    /*
     * NVRAM generation.
     */
    if (csb->nvgen) {
	for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    if (!pdb->ip_pdb) continue;
	    if (isis_checkqueue(&pdb->ip_idbq, idb)) {
		nv_write(TRUE, "%s isis %s", csb->nv_command, pdb->pname);
	    }
	}
	return;
    }

    /*
     * Get router tag string.
     */
    name = GETOBJ(string,1);
    if (strlen(name) == 0)
	name = NULL;

    pdb = clns_find_router_name(name);

    /*
     * Process "no ip router isis <tag>" command.
     */
    if (!csb->sense) {
	if (!pdb || !(pdb->typeflag & ISO_ISISMASK)) return;

	idbq = isis_checkqueue(&pdb->ip_idbq, idb);
	if (!idbq) return;

	unqueue(&pdb->ip_idbq, idbq);
	free(idbq);
    
	if (!pdb->ip_idbq.count) {
	    pdb->isis_mode &= ~ISIS_IP_MODE;
	    ip_router_erase(pdb->ip_pdb);
	    free_pdbindex(pdb->ip_pdb);
	    pdb->ip_pdb = NULL;
	}

	clns_delete_intfc_router(pdb, idb);

    } else {
	if (pdb && !(pdb->typeflag & ISO_ISISMASK)) return;
    
	/*
	 * Enable ISIS-IP only if the interface has an ip address or
	 * if it is an unnumbered interface.
	 */
	if (!idb->ip_address && !idb->ip_unnumbered) {
	    printf("\nCannot enable ISIS-IP. "
		   "Configure IP address or make the interface unnumbered");
	    return;
	}
	/*
	 * Process "ip router isis <tag>" command.
	 */
	clns_add_intfc_router(name, idb, ISO_ISISMASK, ISIS_CIRCUIT_L1L2);

	if (!(pdb = clns_find_router_name(name))) {
	    printf("\nCould not enable IS-IS on interface");
	    return;
	}

	/*
	 * Check if IP pdb is available.
	 */
	if (!isis_ip_get_pdb(pdb)) return;

	/*
	 * Tag interface as running IS-IS for IP.
	 */
	if (isis_checkqueue(&pdb->ip_idbq, idb))  return;
	idbq = malloc_named(sizeof(idbqueuetype), "ISIS IP IDB queue");
	if (!idbq) {
	    printf(nomemory);
	    return;
	}    
	idbq->idb = idb;
	enqueue(&pdb->ip_idbq, idbq);
    }

    /*
     * If a system id is configured, send out new LSP.
     */
    if (pdb->net[0]) {
	isisidb = get_isis_idb(pdb, idb);
	if (isisidb) {
	    if (pdb->level & ISIS_CIRCUIT_L1 &&
		isisidb->isis_circuit_type & ISIS_CIRCUIT_L1) {
		isis_mark_L1_changed(pdb);
	    }
	    if (pdb->level & ISIS_CIRCUIT_L2 &&
		isisidb->isis_circuit_type & ISIS_CIRCUIT_L2) {
		isis_mark_L2_changed(pdb);
	    }
	}
    }
}

/*
 * isisip_init
 *
 * Called at subsystem link time
 */
static void isisip_init (subsystype *subsys)
{
    reg_add_ip_isis_get_pdb(isis_ip_get_pdb, "isis_ip_get_pdb");
    reg_add_ip_isis_redistribute(isis_ip_redistribute, "isis_ip_redistribute");
    reg_add_ip_isis_redistribute_command_exception(
				     isis_ip_redistribute_command_exception,
				    "isis_ip_redistribute_command_exception");
    reg_add_ip_interface_route_adjust(isis_ip_route_adjust,
				     "isis_ip_route_adjust");
    reg_add_ip_passive_cmd_route_adjust(PROC_ISIS, 
					isis_ip_passive_interface_command,
					"isis_ip_passive_interface_command");
    reg_add_ip_isis_show_interfaces(isis_ip_show_interfaces,
				    "isis_ip_show_interfaces");
    reg_add_ip_isis_age_routes(ip_isis_age_routes, "ip_isis_age_routes");
    reg_add_ip_address_change(isis_ip_address_change,
			      "isis_ip_address_change");
    reg_add_isis_insert_ip_if_address(isis_insert_ip_if_address,
				      "isis_insert_ip_if_address");
    reg_add_isis_valid_ip_if_address(isis_valid_ip_if_address,
				      "isis_valid_ip_if_address");
    reg_add_isis_insert_ip_lsp(isis_insert_ip_lsp, "isis_insert_ip_lsp");
    reg_add_isis_construct_ip_adv(isis_construct_ip_adv,
				  "isis_construct_ip_adv");
    reg_add_isis_add_ip_route(isis_add_ip_route, "isis_add_ip_route");
    reg_add_isis_cleanup_ip_route(isis_cleanup_ip_route,
				  "isis_cleanup_ip_route");
    reg_add_isis_ip_add_default_route(isis_ip_add_default_route,
				      "isis_ip_add_default_route");
}


/*
 * ISIS-IP subsystem header
 */
#define ISISIP_MAJVERSION 1
#define ISISIP_MINVERSION 0
#define ISISIP_EDITVERSION  1

SUBSYS_HEADER(isisip, ISISIP_MAJVERSION, ISISIP_MINVERSION, ISISIP_EDITVERSION,
	      isisip_init, SUBSYS_CLASS_PROTOCOL,
	      "req: isis",
	      "seq: isis");

