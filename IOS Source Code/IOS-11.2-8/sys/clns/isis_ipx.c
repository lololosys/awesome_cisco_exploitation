/* $Id: isis_ipx.c,v 3.11.6.23 1996/08/14 02:35:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_ipx.c,v $
 *------------------------------------------------------------------
 * IPX-specific support for IS-IS and NLSP
 *
 * May 1994, Dave Katz
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isis_ipx.c,v $
 * Revision 3.11.6.23  1996/08/14  02:35:50  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.11.6.22  1996/08/12  01:06:57  dkatz
 * CSCdi45425:  NLSP may not report RIP/SAP routes
 * Branch: California_branch
 * Don't let the DR timer get in the way of reporting adjacencies (and
 * remove a couple of other checks for it).  Rebuild both non-pseudonode
 * and pseudonode LSPs when an interface changes state.
 *
 * Revision 3.11.6.21  1996/08/11  15:13:22  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.11.6.20  1996/08/08  14:50:52  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.11.6.19  1996/08/08  05:17:48  mschaef
 * CSCdi31506:  NLSP Ager process remains when NLSP is disabled
 * Branch: California_branch
 *
 * Also:
 *
 *   - Convert to use new scheduler primitives
 *   - Remove cruft including unused variables/params
 *
 * Revision 3.11.6.18  1996/08/07  08:57:26  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.6.17  1996/08/04  23:35:19  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.11.6.16  1996/07/26  00:08:11  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.11.6.15  1996/07/24  01:13:53  john
 * CSCdi63771:  IPX SAP table unstable using NLSP in looped topology
 * Branch: California_branch
 *
 * Revision 3.11.6.14  1996/06/25  00:27:23  mschaef
 * CSCdi37797:  NLSP uses different next hop address than expected on WANs
 * Branch: California_branch
 *
 * Revision 3.11.6.13  1996/06/05  15:57:01  john
 * CSCdi58857:  Alignment error in NLSP LSP handling
 * Branch: California_branch
 *
 * Revision 3.11.6.12  1996/06/05  14:56:16  john
 * CSCdi58363:  ipx rip updates are not sent when nlsp is disabled
 * Branch: California_branch
 *
 * Revision 3.11.6.11  1996/05/26  06:46:13  dkatz
 * CSCdi58737:  ISIS/NLSP debug messages can be misleading
 * Branch: California_branch
 * Issue debug messages before transmitting packets.  Add instrumentation
 * for retransmissions.
 *
 * Revision 3.11.6.10  1996/05/22  17:04:14  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.11.6.9  1996/05/14  01:16:38  sluong
 * CSCdi52574:  NLSP LSP for statics lost after clear ipx nl *
 * Branch: California_branch
 *
 * Revision 3.11.6.8  1996/05/07  17:18:55  dkatz
 * CSCdi55147:  NLSP doesnt detect duplicate system IDs
 * Branch: California_branch
 * Detect duplicates in LSPs and IIHs.  Back off the irritating messages
 * to once per minute.
 *
 * Revision 3.11.6.7  1996/05/03  01:51:00  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.11.6.6  1996/04/21  20:47:42  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.11.6.5  1996/04/12  00:36:39  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.11.6.4  1996/04/09  22:31:44  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.11.6.3  1996/04/09  17:45:34  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.11.6.2  1996/04/02  05:36:05  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.11.6.1  1996/03/18  19:03:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  06:31:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/13  01:13:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.1  1996/03/07  08:40:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11  1996/03/05  23:10:09  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.10  1996/03/01  17:36:18  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix for compiler warning in obj-sp-solaris.
 *
 * Revision 3.9  1996/03/01  07:31:44  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.8  1996/02/20  18:06:39  dkatz
 * CSCdi47916:  ISIS/NLSP LSP checksum errors
 * Improve diagnostics.  Ignore errors if the holdtime is zero
 * (workaround).
 *
 * Revision 3.7  1996/01/18  21:48:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/11  04:45:30  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.5  1995/11/30  22:07:34  sluong
 * CSCdi44536:  isis/nlsp does not generate the non-pseudonode
 * Fix the broken message.
 *
 * Revision 3.4  1995/11/20  18:10:46  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.3  1995/11/17  08:53:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  20:52:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/26  00:49:17  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.16  1995/10/21  19:03:09  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.15  1995/10/09  17:17:58  sluong
 * CSCdi41778:  NLSP generates multiple pseudonodes with the same
 * management info
 *
 * Revision 2.14  1995/09/29  17:58:08  sluong
 * CSCdi41213:  NLSP leaks memory when its turned off
 *
 * Revision 2.5  1995/07/25  21:40:17  john
 * Revision 2.13  1995/09/23  23:00:41  dkatz
 * CSCdi31507:  NLSP hostname not cleaned up when NLSP is disabled
 * Remove the local host name from the name cache when turning off
 * NLSP.
 *
 * Revision 2.12  1995/09/23  22:57:16  dkatz
 * CSCdi33185:  NLSP nits with WAN pseudonodes
 * Always generate a zero node ID for a WAN pseudonode.
 *
 * Revision 2.11  1995/09/23  22:48:36  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.10  1995/09/23  21:37:46  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.9  1995/08/25  15:12:05  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.8  1995/08/23  18:26:08  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.7  1995/08/23  18:03:15  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.6  1995/08/08  16:43:08  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.5  1995/07/25 21:40:17  john
 * CSCdi37603:  NLSP LSP, Name matches ours for similar but not identical
 *              names
 *
 * Revision 2.4  1995/06/21  08:53:56  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/18  19:39:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:59:12  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:18:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "../if/network.h"
#include "config.h"
#include "sys_registry.h"
#include "name.h"
#include "../iprouting/route.h"
#include "../ip/ip.h"
#include "../iprouting/route_inlines.h"
#include "../xns/novell.h"
#include "../xns/novisis.h"
#include "../xns/novnlsp.h"
#include "../clns/parser_defs_isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"
#include "../clns/isis_inlines.h"
#include "bitlogic.h"

uchar isis_nlsp_pdbindex_bitfield[BITMASK_BYTES_IN_ARRAY(MAX_NOVPROCS)]; 


/*
 * ipx_nlsp_cache_name
 *
 * Store a name in the name cache, removing any duplicates of either
 * name or address.
 *
 * If "expire_time" is not running, the entry is permanent.
 */
static void ipx_nlsp_cache_name (char *name, ulong name_length,
				 uchar *system_id, sys_timestamp expire_time)
{
    nametype *old_name_ptr, *new_name_ptr;
    addrtype addr;

    /* Look up any existing name, and delete it if present. */

    old_name_ptr = name_cache_lookup(name, NULL, NAM_IPX);
    if (old_name_ptr) {
	name_delete(old_name_ptr->name, NAM_IPX);
    }

    /* Look up any existing address, and delete it if present. */

    addr.type = ADDR_NOVELL;
    addr.length = STATION_LENGTH;
    station_copy(system_id, addr.nov_addr.host);
    old_name_ptr = name_cache_lookup(NULL, &addr, NAM_IPX);
    if (old_name_ptr) {
	name_delete(old_name_ptr->name, NAM_IPX);
    }

    /* Set up the new name entry. */

    new_name_ptr = malloc_named(sizeof(nametype) + name_length + 1,
				"NLSP hostname");
    if (!new_name_ptr)
	return;

    bcopy(name, new_name_ptr->name, name_length);
    new_name_ptr->name[name_length] = '\0';
    GET_TIMESTAMP(new_name_ptr->lasttime);
    COPY_TIMESTAMP(expire_time, new_name_ptr->expiretime);
    new_name_ptr->flags = NAM_IPX | NAM_ADDRVALID;
    if (!TIMER_RUNNING(expire_time))
	new_name_ptr->flags |= NAM_PERMANENT;
    bcopy(system_id, new_name_ptr->v.ipx.adr.nov_addr.host, STATION_LENGTH);
    new_name_ptr->v.ipx.adr.type = ADDR_NOVELL;
    new_name_ptr->v.ipx.adr.length = STATION_LENGTH;
    enqueue(&nameQ, new_name_ptr);
}

/*
 * ipx_nlsp_cache_our_hostname
 *
 * Put our hostname into the cache, removing any previous entry with the
 * same name or ID.
 *
 * If "sense" is FALSE, remove the cache entry.
 *
 * We use a cheap hack to remove it;  we install it again with a zero
 * lifetime.
 */
static void ipx_nlsp_cache_our_hostname (clns_pdbtype *pdb, boolean sense)
{
    sys_timestamp dummy_timer;

    if (sense) {
	TIMER_STOP(dummy_timer);	/* Make it permanent. */
    } else {
	TIMER_START(dummy_timer, 0);	/* Make it expire right away. */
    }
    ipx_nlsp_cache_name(hostname, strlen(hostname), pdb->systemId,
			dummy_timer);
}

/*
 * ipx_nlsp_stationid_lookup
 *
 * Looks up the station ID in the name cache, if name caching is enabled.
 * If not, or the ID is not present in the cache, it returns a normal
 * station ID string instead.
 */
static char *ipx_nlsp_stationid_lookup (clns_pdbtype *pdb, uchar *stationid)
{
    nametype *name_entry;
    addrtype addr;

    if (!pdb || !pdb->name_lookup_disabled) {
	addr.type = ADDR_NOVELL;
	addr.length = STATION_LENGTH;

	station_copy(stationid, addr.nov_addr.host);
	name_entry = name_cache_lookup(NULL, &addr, NAM_IPX);
	if (name_entry) {
	    return(name_entry->name);
	}
    }
    return(station_string(stationid));
}

/*
 * ipx_nlsp_schedule_redist
 *
 * Schedule a redistribution.  We limit redistribution as follows:
 * No redistribution until REDIST_HOLDDOWN after the last
 * redistribution request.  Redistribute after REDIST_MAX_HOLDDOWN even
 * if the requests are still coming in.
 *
 * This has the effect that we kick NLSP only after things quiet down.
 *
 * Setting REDIST_HOLDDOWN to zero causes things to happen immediately.
 *
 * Note that L1_lsp_build_timer and redist_maxhold work in concert;  in
 * particular, there is an expectation that redist_maxhold is set the
 * first time that L1_lsp_build_timer is set.  If they get out of sync,
 * things could get quite ugly.
 */
#define REDIST_HOLDDOWN ONESEC		/* Quiet interval */
#define REDIST_MAX_HOLDDOWN ONEMIN	/* Do it anyhow if there's mayhem */

static void ipx_nlsp_schedule_redist (clns_pdbtype *pdb)
{
    if (REDIST_HOLDDOWN == 0) {		/* No holddown */
	isis_mark_L1_changed(pdb);
	return;
    }

    /* If timer isn't running, start it.  Note the max timeout. */

    if (!mgd_timer_running(&pdb->L1_lsp_build_timer)) {
	isis_schedule_lsp_build(pdb, CLNS_LEVEL1, REDIST_HOLDDOWN, TRUE);
	TIMER_START(pdb->ipx_redist_maxhold, REDIST_MAX_HOLDDOWN);
	locupdate_buginf(pdb, "Starting IPX redistribution holddown");
	return;
    }

    /* Timer running.  If we've hit the maximum holddown, don't restart it. */

    if (AWAKE(pdb->ipx_redist_maxhold))
	return;

    /* Not at max holddown yet.  Push out the expiration again. */

    isis_schedule_lsp_build(pdb, CLNS_LEVEL1, REDIST_HOLDDOWN, TRUE);
}


/*
 * ipx_nlsp_link_delay
 *
 * Returns the delay for the specified link
 */
static ulong ipx_nlsp_link_delay (nidbtype *nidb)
{
    ulong link_delay, link_throughput, ipx_ticks;

    reg_invoke_ipx_idb_delay_values(nidb->idb, &link_delay, 
				    &link_throughput, &ipx_ticks);
    return(link_delay);
}

/*
 * ipx_nlsp_link_bandwidth
 *
 * Returns the bandwidth for the specified link
 */
static ulong ipx_nlsp_link_bandwidth (nidbtype *nidb)
{
    ulong link_delay, link_throughput, ipx_ticks;

    reg_invoke_ipx_idb_delay_values(nidb->idb, &link_delay, 
				    &link_throughput, &ipx_ticks);
    return(link_throughput);
}

/*
 * isis_pseudonode_needed
 *
 * Returns TRUE if we should generate a pseudonode for this interface
 */
boolean ipx_isis_pseudonode_needed (clns_pdbtype *pdb, idbtype *idb)
{
    nidbtype *nidb;
    int route_count, static_count;
    isisidbtype *isisidb;

    if (!idb)
	return(FALSE);

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return(FALSE);

    /* Always true for IS-IS. */

    if (pdb->proc_type == PROC_TYPE_ISIS)
	return(TRUE);

    /* False if interface is down. */

    if (!isisidb->isis_interface_up)
	return(FALSE);

    /* True for all non-internal NIDBS. */

    nidb = idb->primary_nidb;
    if (!nidb)
	return(FALSE);

    if (!nidb->internal_network)
	return(TRUE);

    /* True for the internal NIDB if there are statics out there. */

    reg_invoke_ipx_return_static_counts(&route_count, &static_count);
    return(route_count || static_count);
}

/*
 * find_nlsp_pdb_by_instance - get CLNS PDB part of NLSP PDB pair
 * based on SNMP instance number (1 - based)
 */
clns_pdbtype *find_nlsp_pdb_by_instance (int instance)
{
    novpdbtype *pdb;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == NET_NLSP) && pdb->osi_pdb &&
	    ((pdb->osi_pdb->index - CLNS_PDBINDEX_NLSP0 + 1) == instance))
		return(pdb->osi_pdb);
    }
    return(NULL);
}

/*
 * find_nlsp_pdb - get CLNS PDB part of NLSP PDB pair.
 */
clns_pdbtype *find_nlsp_pdb (char *tag)
{
    novpdbtype *novpdb;
    clns_pdbtype *pdb;

    pdb = NULL;
    novpdb = novnlsp_find_pdb(tag);
    if (novpdb != NULL) {
	pdb = novpdb->osi_pdb;
    }
    return(pdb);
}

/*
 * isis_ipx_backup_callback
 *
 * An IPX route is subject for installation in the IPX routing table. This
 * occurs when the IPX route was previously attempted to be added, but failed 
 * because there was a route that existed with better administrative distance.
 * Now the route with better AD has gone unreachable.
 *
 * If the route is still valid, run Dijkstra.
 */
static void isis_ipx_backup_callback (novpdbtype *pdb,
			       ulong address,
			       ulong netmask,			       
			       ulong context,
			       ulong route_type)
{
    clns_pdbtype *clns_pdb;
    ushort index;
    ushort version;
    boolean debug = FALSE;

    if (!pdb)
	return;

    debug = (novell_private_debug && NOVNLSP_DEBUG(address, pdb));

    clns_pdb = pdb->osi_pdb;
    index = (context >> 16);
    version = (context & 0x0000ffff);

    if (debug) {
	buginf("\nIPX-NLSP: add route callback net %x, pdb @ %x (%d)"
	       ", lsp {%d/%d} ", address, pdb, pdb->index, index, version);
    }
    /*
     * If this route is from an old Dijkstra run, do not add it back
     * to routing table.
     */
    if (route_type == RTYPE_L1) {
	if (!isis_valid_lsp_version(clns_pdb, index, version)) {
	    if (debug) buginf("(lsp invalid)");
	    return;
	}
	if (debug) buginf("(lsp changed)");
	isis_mark_lsp_to_recalculate(clns_pdb, index);
    } else if (route_type == RTYPE_L2) {
	if (context != clns_pdb->current_L2_version) return; /* %%% */
	isis_trigger_spf(clns_pdb, CLNS_LEVEL2,
			 ISIS_SPF_TRIG_IPXBACKUP, NULL, TRUE);
    }
}

/*
 * isis_ipx_set_default_redist
 *
 * Set up default redistribution between NLSP and RIP
 *
 */
void ipx_nlsp_set_default_redist (novpdbtype *pdb)
{
    novpdbtype *dist;

    /*
     * Default redistribution of RIP <-> NLSP, static/connected -> NLSP,
     * and NLSPx <-> NLSPy.
     */
    novrip_pdbinit(); /* Static RIP pdb must be initialized */
    novrip_pdb.distflag |= pdb->mask;
    pdb->distflag |=
	(novrip_pdb.mask | novconnected_pdb.mask | novstatic_pdb.mask);
    for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	if (dist == pdb || dist->proctype != NET_NLSP)
	    continue;
	pdb->distflag |= dist->mask;
	pdb->sap_distflag |= dist->mask;
	dist->distflag |= pdb->mask;
	dist->sap_distflag |= pdb->mask;
    }
    /*
     * Default redistribution of SAP <-> NLSP and static SAP -> NLSP:
     */
    novrip_pdb.sap_distflag |= pdb->mask;
    pdb->sap_distflag |= (novrip_pdb.mask | novstatic_pdb.mask);
}

/*
 * isis_ipx_init
 *
 * Allocate an IPX pdb for the IS-IS protocol. There will not be an IPX
 * specific process running. The IS-IS protocol and state information
 * to operate IPX runs out of the CLNS pdb.
 */

static char *const nov_pdb_name[ISIS_NUM_PROC_TYPE] = {
    "isis", "nlsp"
    };

novpdbtype *isis_ipx_init (clns_pdbtype *clns_pdb, isis_proc_type proc_type)
{

    novpdbtype *pdb;
    
    /*
     * Create the pdb.
     */
    pdb = malloc_named(sizeof(novpdbtype), "NLSP IPX PDB");
    if (!pdb) {
	printf(nomemory);
	return(NULL);
    }

    /*
     * Allocate an unused router process flag.
     */
    if (allocate_novpdbindex(pdb) == FALSE) {
	free(pdb);
	printf("\n%%Unable to allocate index for %s pdb.", 
	       clns_pdb->errmsg_name);
	return(NULL);
    }	

    if (nov_urd_table == NULL)
	nov_urd_table = malloc_named(sizeof(nov_routetable), "IPX URD TBL");
    
    if (nov_urd_table == NULL) {
	free(pdb);
	printf(nomemory);
	return(NULL);
    }

    /*
     * Allocate memory for input queue.
     */
    queue_init(&pdb->inputq, 0);
    queue_init(&pdb->workQ, 0);

    /*
     * Fill in pdb fields related to IS-IS.
     */
    pdb->name = nov_pdb_name[proc_type];
    setstring(&pdb->pname, clns_pdb->pname);
    pdb->process = NULL;
    pdb->running = FALSE;
    pdb->restart = isis_ipx_restart;
    pdb->cleanup = isis_ipx_cleanup;
    pdb->holddown = NULL;
    pdb->redistribute = novnlsp_process_redistribute;
    pdb->redist_callback = isis_ipx_redist_callback;
    pdb->xroute_callback = isis_ipx_xroute_callback;
    pdb->sap_redist_callback = isis_ipx_sap_redist_callback;
    pdb->xsap_callback = isis_ipx_xsap_callback;
    pdb->backup_route_callback = novrip_process_backup_route;
    pdb->inject_backup_route_callback = novrip_inject_one_backup_route;
    pdb->designated_router_callback = novnlsp_dr_callback;
    pdb->proctype = (proc_type == PROC_TYPE_NLSP) ? NET_NLSP : NET_ISIS;
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->distance = pdb->def_distance = ISIS_DISTANCE;
    pdb->debug_set = FALSE;
    pdb->debug_net = 0;
    pdb->osi_pdb = clns_pdb;
    pdb->add_route = isis_ipx_backup_callback;
    pdb->rdb_changed_callback = isis_ipx_rdb_changed_callback;
    pdb->srdb_changed_callback = isis_ipx_srdb_changed_callback;
    pdb->malloc_backup = TRUE;

    ipx_nlsp_set_default_redist(pdb);

    /*
     * Put on IPX protocol queue:
     */
    enqueue(&novpdbQ, pdb);

    return(pdb);
}

/*
 * nlsp_pdb_init
 *
 * Creates and initializes both PDBs for NLSP.  The flow is a bit different
 * than for integrated IS-IS, since the process is driven from the IPX
 * side instead.
 *
 * If nov_pdb is non-null, we're restarting NLSP with an existing IPX PDB.
 * If it's null, we're firing it up for the first time.
 */
novpdbtype *nlsp_pdb_init (char *router_tag, novpdbtype *nov_pdb)
{
    clns_pdbtype *clns_pdb;
    novpdbtype *novpdb;
    ulong index, pdbs = 0;
    ushort checksum;

    for (novpdb = novpdbQ.qhead; novpdb; novpdb = novpdb->next) {       
	pdbs++;
	if (novpdb->proctype == NET_NLSP && novpdb->osi_pdb) {
	    if (!strcasecmp(novpdb->pname, strlen(router_tag) ? router_tag : 
			    NULL)) {
		return(NULL);
	    }
	}
    }
    
    if (pdbs >= MAX_NOVPROCS) {
	printf("\n%%Maximum of %d IPX processes are allowed", MAX_NOVPROCS);
	return (NULL);
    }

    if (!(BITMASK_FIND_FIRST_CLEAR(isis_nlsp_pdbindex_bitfield, MAX_NOVPROCS, 
				   &index))) {
	printf("\n%%Exceeded maximum of NLSP instances");
	return (NULL);
    }
    SETBIT(index, isis_nlsp_pdbindex_bitfield);
    clns_pdb = isis_pdb_init(router_tag, PROC_TYPE_NLSP);
    if (clns_pdb == NULL)
	return(NULL);

    clns_pdb->typeflag = ISO_NLSP;
    clns_pdb->index = index + CLNS_PDBINDEX_NLSP0;

    /* Set the stationid formatting routine. */

    clns_pdb->stationid_lookup = ipx_nlsp_stationid_lookup;

    /* Set the system ID from the default MAC address. */

    /* %% We'll need a callback if the defaddr is changed */

    ieee_copy(nov_defaddr, clns_pdb->systemId);

    /* Checksum system id. */
    checksum = isis_compute_checksum(clns_pdb->systemId, 
				     STATION_LENGTH, NULL, 0, 0);
    PUTSHORT(clns_pdb->systemId_checksum, checksum);

    if (!nov_pdb) {
	nov_pdb = isis_ipx_init(clns_pdb, clns_pdb->proc_type);
    } else {
	nov_pdb->osi_pdb = clns_pdb;
    }
    if (nov_pdb == NULL)
	return(NULL);

    clns_pdb->ipx_pdb = nov_pdb;
    clns_pdb->isis_mode |= ISIS_IPX_MODE;

    /* Launch the process. */

    isis_fork_processes(clns_pdb, clns_pdb->proc_type);

    /* Add all interfaces, in case they were configured first. */

    nlsp_add_all_ifs(nov_pdb);

    /* Register the hostname in the name cache. */

    ipx_nlsp_cache_our_hostname(clns_pdb, TRUE);

    return(nov_pdb);
}

/*
 * isis_ipx_restart
 *
 * Do common restart as appropriate ... called from "ipx router nlsp"
 * if "no ipx router nlsp" was issued. Also called by "ipx routing"
 * if "no ipx routing" had caused ipx to be shutdown. All context
 * required to start/restart nlsp should be stored in the pdb. Note
 * that we key on pdb->running to figure out if we are already running.
 */
void isis_ipx_restart (novpdbtype *pdb)
{
    boolean debug = novellnlsp_debug;

    /*
     * Immediate bail if are already restarted:
     */
    if (pdb->running) {
	return;
    }
    pdb->running = TRUE;

    if (debug) buginf("\nNLSP: restarting ...");

    /*
     * Ensure at least partial RIP/SAP functionality:
     */
    novell_common_restart();

    /*
     * Note that we get away with default redistribution for now because
     * we do not currently allow any changes to default settings.
     *
     * THIS WILL CHANGE!
     */
    ipx_nlsp_set_default_redist(pdb);

    nlsp_pdb_init(pdb->pname, pdb);	

    novell_nlsp_running++;

    novnlsp_ager_create(); /* Create ager process if appropriate */

    /*
     * Possibly trim maxpaths since NLSP supports only 4:
     */
    novell_paths_command(FALSE, TRUE, novell_cfg_maxpaths);
}

/*
 * nov_same_lsp
 *
 * If this idb matches any idb in the lsp desc list return TRUE.
 */
static boolean nov_same_lsp (novndbtype *ndb, idbtype *idb)
{
    novlsptype *desc;

    if (ndb) {
	for (desc = ndb->lsp_list; desc; desc = desc->next) {
	    if (desc->idb == idb) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * ipx_nlsp_enqueue_lsp_desc
 *
 * Add an element for "idb" to the lsp descriptor list for the aggregated
 * (drop) route described by "ndb". This indicates that a route or routes
 * learned from "idb" match an area address or summary access list that
 * has generated this aggregated route. This list keeps track of *all* the
 * idb's with matching routes that have contributed to this summary. A
 * drop route should not be removed from the route table until this list
 * becomes empty.
 */
static void ipx_nlsp_enqueue_lsp_desc (novndbtype *ndb, idbtype *idb)
{
    novlsptype *desc;

    if (!nov_same_lsp(ndb, idb)) {
	desc = malloc_named(sizeof(novlsptype), "IPX AGG LSP");
	if (desc == NULL) {
	    return;
	}
	desc->idb = idb;
	desc->next = ndb->lsp_list;
	ndb->lsp_list = desc;
    }
}

/*
 * ipx_nlsp_dequeue_lsp_desc
 *
 * Dequeue and deallocate the lsp descriptor for "idb" if present. If
 * the descriptor is present it means that a route or routes learned
 * on this idb matched an area address or summary access list and
 * triggered the aggregated (drop) route described by "ndb". Return
 * TRUE if and only if the list is now empty (which means the drop
 * route should be deleted).
 */
static boolean ipx_nlsp_dequeue_lsp_desc (novndbtype *ndb, idbtype *idb)
{
    novlsptype *desc, *last_desc;

    if (ndb == NULL)
	return(FALSE);

    last_desc = (novlsptype *)&ndb->lsp_list;
    desc = ndb->lsp_list;

    for (last_desc = (novlsptype *)&ndb->lsp_list, desc = ndb->lsp_list;
	 desc; last_desc = desc, desc = desc->next) {
	if (desc->idb == idb) {
	    last_desc->next = desc->next;
	    free(desc);
	    return(ndb->lsp_list == NULL);
	}
    }
    return(FALSE);
}

/* 
 * ipx_nlsp_delete_all_summaries_patricia
 *
 * Remove all summaries generated by the process in the radix tree.
 */
static int ipx_nlsp_delete_all_summaries_patricia (rntype *rnnode, va_list ap)
{
    novpdbtype *pdb = va_arg(ap, novpdbtype *);
    nov_sumroutetype *net;
    novndbtype *p;		/* Route data for a network */
    novrdbtype *pp;		/* Path entry in chain attached to a "p" */
    novrdbtype *lastpp; 	/* Save pp for deletion */

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;    

    p = net->ndb;
    if (p && p->path_list && p->path_list->nidb == nullnidb &&
	p->novpdbindex == pdb->index) {
	if (!rn_delete(&p->keylen, &p->masklen, nov_rnhead))
	    buginf("\nIPX: reset_patricia did not delete %x %x correctly",
		   p->dstnet, p->rt_mask);    
	/* Start by throwing away all the path entries */
	pp = p->path_list;
	while (pp != NULL) {
	    lastpp = pp;
	    pp = pp->next;
	    novell_mark_rdb_changed(p, lastpp, p->flags,
				    IPX_ROUTE_EVENT_DELETED);
	    novrdb_free(lastpp);
	}
	p->options |= IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP;
	nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, FALSE);
	free(net);
    }

    return NOV_WALK_CONTINUE;
}

/* 
 * ipx_nlsp_route_adjust_patricia
 *
 * Check to see if any drop routes need to be deleted following an
 * idb down or nidb down event.
 */
static int ipx_nlsp_route_adjust_patricia (rntype *rnnode, va_list ap)
{
    novpdbtype *pdb = va_arg(ap, novpdbtype *);
    idbtype    *idb = va_arg(ap, idbtype *);
    nov_sumroutetype *net;
    novndbtype *ndb;

    net = (nov_sumroutetype *)rnnode;

    if (!net)
	return NOV_WALK_ABORT;

    ndb = net->ndb;

    if (ndb && ndb->path_list && (ndb->path_list->nidb == nullnidb) &&
	(ndb->novpdbindex == pdb->index)) {
	if (ipx_nlsp_dequeue_lsp_desc(ndb, idb)) {
	    novrip_del_rt(ndb->dstnet, ndb->rt_mask,
			  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	}
    }
    return NOV_WALK_CONTINUE;
}

/*
 * isis_ipx_cleanup
 *
 * Do teardown as appropriate ... invoked by nov_router_erase().
 *
 */
void isis_ipx_cleanup (novpdbtype *pdb)
{
    clns_pdbtype *cpdb;
    idbtype *idb;
    nidbtype *nidb;
    paktype *pak;
    boolean debug;

    debug = novellnlsp_debug;

    if (debug) buginf("\nNLSP: going down, cleanup");

    cpdb = pdb->osi_pdb;
    if (!cpdb)
	return;

    /* Flag that we're not running, in case we suspend during any of this. */

    clns_set_pdb_running(cpdb, FALSE);

    /*
     * Clean up each interface.  Note that novnlsp_change_interface causes
     * the IDB to be dequeued from the idbqueue, so this loop will 
     * in fact terminate!
     */

    while (cpdb->idbqueue) {
	idb = cpdb->idbqueue->idb;
	nidb = idb->primary_nidb;
	novnlsp_change_interface(pdb, nidb, FALSE, FALSE);
    }

    /* Kill the processes. */

    if (cpdb->clns_pid != NO_PROCESS) {
	process_kill(cpdb->clns_pid);
	cpdb->clns_pid = NO_PROCESS;
    }
    if (cpdb->update_pid != NO_PROCESS) {
	process_kill(cpdb->update_pid);
	cpdb->update_pid = NO_PROCESS;
    }

    /* Clean up the LSP database (aging all LSPs). */

    isis_clear_database(cpdb, cpdb->level1_LSPs);

    /* Free the LSP index table. */

    if (cpdb->lspix_table) {
	free(cpdb->lspix_table);
	cpdb->lspix_table = NULL;
    }

    /* Free any lingering input packets. */

    while (process_queue_size(cpdb->clns_watched_inputq)){
	pak = process_dequeue(cpdb->clns_watched_inputq);
	retbuffer(pak);
    }
    while (process_queue_size(cpdb->update_inputq)){
	pak = process_dequeue(cpdb->update_inputq);
	retbuffer(pak);
    }
    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
			 ipx_nlsp_delete_all_summaries_patricia, pdb);

    /* Make sure any remaining timers are dead. */

    mgd_timer_stop(&cpdb->update_timer);
    mgd_timer_stop(&cpdb->timer);

    /* Remove the hostname from the name cache. */

    ipx_nlsp_cache_our_hostname(cpdb, FALSE);

    /*
     * Delete any unreachable route data:
     */
    nov_clean_urd_table(pdb);

    /* Clear our index. */
 
    CLEARBIT(cpdb->index - CLNS_PDBINDEX_NLSP0, isis_nlsp_pdbindex_bitfield);

    /* Can't think of much else to do.  Free the PDB. */

    pdb->osi_pdb = NULL;
    isis_cleanup_pdb(cpdb);
    free(cpdb);

    /*
     * Inject backup data if only NLSP is shutting down. Always delete
     * anything that goes unused. NOTE: Allow suspend or risk CPUHOG!
     */
    if (!novell_shutting_down) {
	novrip_inject_all_backup_routes(TRUE, pdb);
	novrip_inject_all_backup_services(TRUE, pdb);
    }
    novrip_delete_all_backup_data(0, 0);

    /*
     * Shut down Ager if appropriate. Note that novell_nlsp_running
     * was decremented before the current call to cleanup.
     */
    novnlsp_ager_kill();

    /*
     * Shut down RIP/SAP if superfluous:
     */
    novell_common_cleanup();
    return;
}

/*
 * isis_ipx_get_pdb
 *
 * Gets IS-IS IPX pdb from clns pdb. If it does not exist, an IPX
 * pdb is created. Note that the IPX backup route table is created
 * to support the potential pseudonode LSP.
 */
novpdbtype *isis_ipx_get_pdb (clns_pdbtype *clns_pdb)
{
    novpdbtype *pdb;

    /* 
     * First check that CLNS pdb is owned by IS-IS.
     */
    if (!(clns_pdb->typeflag & ISO_ISISMASK)) {
	return(NULL);
    }

    pdb = clns_pdb->ipx_pdb;
    if (!pdb) {
	/* Create backup route table if necessary */
	if (!novrip_init_backup_tables())
	    return(NULL);
	pdb = isis_ipx_init(clns_pdb, PROC_TYPE_ISIS);
	clns_pdb->ipx_pdb = pdb;
	clns_pdb->isis_mode |= ISIS_IPX_MODE;
    }

    return(pdb);
}

/*
 * isis_add_ipx_route
 *
 * Called by Dijkstra to add an IPX route
 */
void isis_add_ipx_route (clns_pdbtype *pdb, 
			 ulong net_number, 
			 ulong netmask,
			 isis_lspdbtype *lsp_zero,
			 isis_lspdbtype *current_lsp,
			 isis_lsp_parent *next_hops, 
			 isis_lsp_parent *leaf_params,
			 int metric,
			 boolean external)
{
    idbtype *next_idb;
    ulong next_net;
    uchar *next_node;
    nov_cost_param_type nov_cost;
    int ix;
    isis_lspix_type *lspix_entry;
    isisidbtype *isisidb;

    /* Iterate for each path. */

    nov_cost.metric = metric;		/* Invariant */
    for (ix = 0; ix < lsp_zero->parent_count; ix++, next_hops++) {
	/*
	 * Record order of preference detail:
	 */
	nov_cost.delay = next_hops->ticks;
	if (netmask == NOV_NETMASK_ALL)
	    nov_cost.hops = next_hops->hops;
	else {
	    /*
	     * Special handling for aggregated route metric. Set hop
	     * count to 254 (not 255) to be picked up later by services
	     * learned via aggregated route. Such a service will be
	     * reachable, but of lower precedence than services learned
	     * via an explicit route with the same ticks (as per the
	     * spec). In other words, services learned via aggregated
	     * routes will show a route metric of "ticks/254" and GNS
	     * will do its thing accordingly.
	     */
	    nov_cost.hops = (NOVELL_MAX_HOPS_ALLOWED-1);
	    nov_cost.area_count = next_hops->hops;
	}
	nov_cost.ext_delay = leaf_params->ticks;
	nov_cost.ext_hops = leaf_params->hops;
	nov_cost.oop.total_throughput = next_hops->throughput;
	nov_cost.oop.total_delay = next_hops->delay;
	nov_cost.oop.mtu = next_hops->mtu;
	if (next_hops->next_hop) {
	    ieee_copy(&next_hops->next_hop->key[1], nov_cost.oop.system_id);
	    ieee_copy(next_hops->next_hop->snpa.addr, nov_cost.oop.mac_addr);
	    isisidb = get_isis_idb(pdb, next_hops->next_hop->idb);
	    if (isisidb)
		nov_cost.oop.circuit_id = isisidb->isis_local_ckt_number;
	} else {
	    ieee_zero(nov_cost.oop.system_id);
	    ieee_zero(nov_cost.oop.mac_addr);
	    nov_cost.oop.circuit_id = 0;
	}
	if (isis_nlsp_spf_event_debug[pdb->proc_type] &&
	    !isis_nlsp_terse_spf_event_debug[pdb->proc_type]) {
	    buginf("\n%s: SPF route to %x %x [M%d/T%d/H%d][Ext T%d/H%d] via ",
		   pdb->errmsg_name, net_number, netmask,
		   nov_cost.metric, nov_cost.delay, nov_cost.hops, 
		   nov_cost.ext_delay, nov_cost.ext_hops);
	    if (next_hops->next_hop == NULL) {
		buginf("local");
	    } else {
		buginf("%x.%e", next_hops->next_hop->ipx_net,
		       next_hops->next_hop->ipx_node);
	    }
	}
	if (next_hops->next_hop == NULL) {
	    next_idb = NULL;
	    next_net = 0;
	    next_node = NULL;
	} else {
	    next_idb = next_hops->next_hop->idb;
	    next_net = next_hops->next_hop->ipx_net;
	    next_node = next_hops->next_hop->ipx_node;
	}
	/*
	 * If IDB is null, this is a directly connected route and they already
	 * know about it, so just punt.
	 */
	if (next_idb == NULL) {
	    return;
	}
	if (pdb->proc_type == PROC_TYPE_NLSP && pdb->lspix_table) {
	    lspix_entry =
		isis_lsp_index_to_lspix_entry(pdb, current_lsp->lsp_index);
	    if (lspix_entry) {
		if (netmask != NOV_NETMASK_ALL) {
		    if (next_hops->next_hop->ipx_version < NLSP_VERSION) {
			/*
			 * Next hop router is an old router. It won't  
			 * understand aggregate routes. Can't use it.
			 */
			if (isis_nlsp_spf_event_debug[pdb->proc_type]) {
			    buginf("\n%s: SPF route to %x %-8x is unusable",
				   pdb->errmsg_name, net_number, netmask);
			}
			novrip_add_rt(pdb->ipx_pdb, nullnidb, net_number, 
				      netmask, 0, 0, 
				      NOVELL_DEFAULT_CONNECTED_TICKS,
				      NOVELL_DEFAULT_RIP_HOPS,
				      NET_SUMMARY | NET_NLSP, 
				      IPX_ROUTE_ADD_NORMAL, 
				      IPX_NDB_OPTION_STATIC_NO_NVGEN,
				      IPX_STATIC_TYPE_HOST,
				      IPX_FLOATING_STATIC_EXISTS_NO);
			continue;
		    }
		}
		novnlsp_update(pdb->ipx_pdb,		/* ipx pdb */
			       net_number,		/* dnet */
			       netmask,                 /* dnet mask */
			       next_net,		/* next hop net */
			       next_node, 		/* next hop host */
			       IPX_PRIMARY_NIDB(next_idb), /* nidb */
			       &nov_cost, 		/* cost summary */
			       ((netmask != NOV_NETMASK_ALL) ? NET_SUMMARY :
				(external ? NET_XROUTE : 0)), /* TRUE if xrt */
			       &lspix_entry->chains[ISIS_ROUTE_IPX],
			       lspix_entry->lsp_version,
			       current_lsp->lsp_index);
	    }
	}
    }
}

/*
 * isis_add_ipx_service
 *
 * Called by Dijkstra to add an IPX service advertisement
 */
void isis_add_ipx_service (clns_pdbtype *pdb,
			   isis_nlsp_svcs_info *svc,
			   isis_lspdbtype *lsp_zero,
			   isis_lspdbtype *current_lsp,
			   isis_lsp_parent *next_hops,
			   isis_lsp_parent *leaf_params,
			   int name_length)
{
    uchar svc_name[ISIS_NLSP_SVC_MAXLEN + 1];
    ulong net;
    ushort sock, type;
    idbtype *next_idb;
    nov_cost_param_type nov_cost;
    int ix;
    ulong hops;
    isis_lspix_type *lspix_entry;
    isisidbtype *isisidb;

    net = GETLONG(&svc->net_number);
    sock = GETSHORT(&svc->socket);
    type = GETSHORT(&svc->type);
    
    /* Iterate for each equal-cost path. */

    nov_cost.metric = 0;		/* Invariant */
    for (ix = 0; ix < lsp_zero->parent_count; ix++, next_hops++) {
	if (next_hops->next_hop == NULL) {
	    next_idb = NULL;
	} else {
	    next_idb = next_hops->next_hop->idb;
	}

	/*
	 * Decrement the overall hop count by one.  It ends up one too
	 * high as a result of the SPF calculation--the pseudonode (to
	 * which the external is adjacent) must be viewed as one hop
	 * away from its previous hop, but that hop doesn't count for
	 * externals advertised from there.
	 */

	hops = next_hops->hops;
	if (hops > 0)
	    hops--;
	/*
	 * Record order of preference detail:
	 */
	nov_cost.delay = next_hops->ticks;
	nov_cost.hops = hops;
	nov_cost.ext_delay = leaf_params->ticks;
	nov_cost.ext_hops = leaf_params->hops;
	nov_cost.oop.total_throughput = next_hops->throughput;
	nov_cost.oop.total_delay = next_hops->delay;
	nov_cost.oop.mtu = next_hops->mtu;
	if (next_hops->next_hop) {
	    ieee_copy(&next_hops->next_hop->key[1], nov_cost.oop.system_id);
	    ieee_copy(next_hops->next_hop->snpa.addr, nov_cost.oop.mac_addr);
	    isisidb = get_isis_idb(pdb, next_hops->next_hop->idb);
	    if (isisidb)
		nov_cost.oop.circuit_id = isisidb->isis_local_ckt_number;
	} else {
	    ieee_zero(nov_cost.oop.system_id);
	    ieee_zero(nov_cost.oop.mac_addr);
	    nov_cost.oop.circuit_id = 0;
	}
	if (isis_nlsp_spf_event_debug[pdb->proc_type] &&
	    !isis_nlsp_terse_spf_event_debug[pdb->proc_type]) {
	    bcopy(svc->name, svc_name, name_length);
	    svc_name[name_length] = '\0';
	    buginf("\n%s: SPF service %s at %x.%e [S%d/T%d/H%d] via %s",
		   pdb->errmsg_name, svc_name, net, svc->node_number,
		   sock, type, hops, 
		   next_idb ? next_idb->namestring : "local");
	}
	if (pdb->lspix_table) {
	    lspix_entry =
		isis_lsp_index_to_lspix_entry(pdb, current_lsp->lsp_index);
	    if (lspix_entry) {
		novnlsp_svc_update(pdb->ipx_pdb,
				   next_idb,
				   type, svc->name, name_length,
				   net, (uchar*)&svc->node_number, sock,
				   &nov_cost,
				   &lspix_entry->chains[ISIS_ROUTE_IPX_SVC],
				   lspix_entry->lsp_version,
				   current_lsp->lsp_index);
	    }
	}
    }
}

/*
 * isis_insert_ipx_areas
 *
 * Insert the IPX areas into this packet (LSP or hello)
 *
 * If there are no areas, this leaves item->length zero.
 *
 * Updates bytes_left.
 */
static isis_info_type *isis_insert_ipx_areas (clns_pdbtype *pdb, 
					      isis_info_type *item,
					      int *bytes_left)
{
    isis_nlsp_area_addrs *area_addr;
    ulong area_num;
    int length;

    length = pdb->num_nlsp_areas * ISIS_NLSP_AREA_ADDRS_LEN;
    if (*bytes_left < length + ISIS_INFO_TYPE_OVERHEAD)
	return(item);

    item->code = ISIS_NLSP_AREA_ADDRS;
    item->length = length;
    *bytes_left -= length + ISIS_INFO_TYPE_OVERHEAD;
    area_addr = (isis_nlsp_area_addrs *) item->value;
    
    for (area_num = 0; area_num < pdb->num_nlsp_areas; area_num++) {
	PUTLONG(&area_addr->net_number, pdb->nlsp_manual_area[area_num]);
	PUTLONG(&area_addr->net_mask, pdb->nlsp_manual_area_mask[area_num]);
	ADVANCE_POINTER(area_addr, ISIS_NLSP_AREA_ADDRS_LEN);
    }
    ADVANCE_ITEM(item, FALSE);
    return(item);
}
	
/*
 * Insert options into the hello.
 *
 * "item" is expected to point just beyond the current end of the PDU.
 *
 * Returns the updated item pointer.
 */
isis_info_type *isis_insert_ipx_hello (clns_pdbtype *pdb, isis_info_type *item,
				       idbtype *idb, int bytes_left)
{
    item->length = 0;

    if (!(pdb->isis_mode & ISIS_IPX_MODE) || !idb) {
	return(item);
    }

    /* Add the node address as management info for IS-IS only. */

    if (pdb->proc_type == PROC_TYPE_ISIS && idb->primary_nidb) {
	isis_nlsp_mgmt_info *info;

	if (bytes_left < ISIS_NLSP_MGMT_INFO_LEN + ISIS_INFO_TYPE_OVERHEAD)
	    return(item);
	item->code = ISIS_NLSP_MGMT_INFO;
	item->length = ISIS_NLSP_MGMT_INFO_LEN;
	info = (isis_nlsp_mgmt_info *) item->value;
	PUTLONG(&info->net_number, idb->primary_nidb->novellnet);
	bcopy(idb->novell_node, info->node_number, IEEEBYTES);
	info->ipx_version_number = NLSP_IPX_VERSION_NUMBER;
	info->router_name_length = 0;
	ADVANCE_ITEM(item, TRUE);	/* Length guaranteed nonzero */
    }
	
    /* Add the area addresses. */

    item = isis_insert_ipx_areas(pdb, item, &bytes_left);

    /* Add in the link MTU. */
    
    if (bytes_left < ISIS_NLSP_LOCAL_MTU_LEN + ISIS_INFO_TYPE_OVERHEAD)
	return(item);

    item->code = ISIS_NLSP_LOCAL_MTU;
    item->length = ISIS_NLSP_LOCAL_MTU_LEN;
    PUTLONG(item->value, max_proto_mtu(idb, LINK_NOVELL));
    ADVANCE_ITEM(item, TRUE);		/* Length guaranteed nonzero */

    return (item);
}

/*
 * isis_insert_ipx_lsp_areas
 *
 * Insert IPX area addresses into the LSP, if appropriate.
 */
void isis_insert_ipx_lsp_areas (isis_lspcontext *context)
{
    isis_info_type *item;
    int bytes_left;

    if (!(context->pdb->isis_mode & ISIS_IPX_MODE)) {
	return;
    }

    /* Add the area addresses if necessary. */

    if (!isis_lspid_is_pseudonode(&context->lspid)) {
	item = isis_new_item(context, ISIS_NLSP_AREA_ADDRS, 0, NULL,
			     &bytes_left, FALSE);
	if (item == NULL)
	    return;
	item = isis_insert_ipx_areas(context->pdb, item, &bytes_left);
	isis_item_done(context, item);
    }
}


/*
 * isis_write_nlsp_link_info
 *
 * Writes out a link info entry
 */
static inline void isis_write_nlsp_link_info (isis_nlsp_link_info *entry,
					      ulong metric, uchar *id, 
					      ulong mtu_size, ulong thruput,
					      ulong delay, ushort media)
{
    entry->cost = metric;
    systemid_copy(id, entry->neighbor_id);
    PUTLONG(&entry->mtu_size, mtu_size);
    PUTLONG(&entry->throughput, thruput);
    PUTLONG(&entry->delay, delay);
    PUTSHORT(&entry->media_type, media);
}

/*
 * isis_add_one_nlsp_link_ad
 *
 * Add a link info entry to an existing LSP (used for incremental adds)
 *
 * Pseudo_id is nonzero if we are adding to a pseudonode LSP
 */
void isis_add_one_nlsp_link_ad (isis_info_type *item, uchar *id, idbtype *idb,
				ulong metric, uchar pseudo_id)
{
    isis_nlsp_link_info *entry;
    ulong mtu;
    ulong bandwidth;
    ulong delay;
    nidbtype *nidb;

    nidb = idb->primary_nidb;
    if (nidb == NULL)
	return;

    item->code = ISIS_NLSP_LINK_INFO;
    item->length = ISIS_NLSP_LINK_INFO_LEN;
    entry = (isis_nlsp_link_info *) item->value;
    mtu = max_proto_mtu(idb, LINK_NOVELL);
    bandwidth = ipx_nlsp_link_bandwidth(nidb);
    delay = ipx_nlsp_link_delay(nidb);

    if (pseudo_id) {			/* It's the pseudonode */
	metric = 0;
	mtu = 0;
	bandwidth = 0;
	delay = 0;
    }
    isis_write_nlsp_link_info(entry, metric, id, mtu, bandwidth, delay,
			      nidb->nov_nlsp_media);
}

/*
 * isis_ipx_add_link_item
 *
 * Creates a LINK INFO TLV item
 *
 * Returns TRUE if success, FALSE if out of memory
 */
static boolean isis_ipx_add_link_item (isis_lspcontext *context,
				       ulong metric, uchar *id, 
				       ulong mtu_size, ulong thruput,
				       ulong delay, ushort media)
{
    isis_info_type *item;
    isis_nlsp_link_info *entry;

    item = isis_new_item(context, ISIS_NLSP_LINK_INFO, 
			 ISIS_NLSP_LINK_INFO_LEN, (void **) &entry,
			 NULL, TRUE);
    if (item == NULL)
	return(FALSE);
    isis_write_nlsp_link_info(entry, metric, id, mtu_size, thruput, delay, 
			      media);
    ADVANCE_ITEM(item, TRUE);
    isis_item_done(context, item);
    return(TRUE);
}

/*
 * isis_ipx_add_is_adj
 *
 * Add an IS adjacency to an LSP in progress if the adjacency is up.
 *
 * Returns TRUE if success, FALSE if out of memory
 */
static boolean isis_ipx_add_is_adj (clns_adjtype *adj, 
				    isis_lspcontext *context,
				    boolean pseudonode)
{
    boolean result;
    idbtype *adj_idb;
    ulong metric;
    ulong mtu, bandwidth, delay;
    nidbtype *nidb;
    isisidbtype *isisidb;

    result = TRUE;

    if (adj->state == CLNS_ADJ_UP_STATE && 
	!TIMER_RUNNING_AND_AWAKE(adj->expiration_time)) {
	adj_idb = adj->idb;
	nidb = adj_idb->primary_nidb;
	if (nidb == NULL)
	    return(FALSE);
	isisidb = get_isis_idb(context->pdb, adj_idb);
	if (isisidb == NULL)
	    return(FALSE);

	metric = isisidb->isis_L1_metric;
	mtu = max_proto_mtu(adj_idb, LINK_NOVELL);
	bandwidth = ipx_nlsp_link_bandwidth(nidb);
	delay = ipx_nlsp_link_delay(nidb);
	if (pseudonode) {
	    metric = 0;
	    mtu = 0;
	    bandwidth = 0;
	    delay = 0;
	}
	result = isis_ipx_add_link_item(context, metric,
					&adj->key[1], mtu, bandwidth, delay,
					nidb->nov_nlsp_media);
    }
    return(result);
}


/*
 * isis_add_nlsp_links
 *
 * Add link info to the LSP
 *
 * If context->lsp_idb is non-null, this is a pseudonode LSP
 */
static void isis_add_nlsp_links (isis_lspcontext *context)
{
    clns_pdbtype *pdb;
    idbtype *idb;
    clns_adjtype *adj, *cur_adj;
    int bucket;
    nidbtype *nidb;
    idbtype *cur_idb;
    isisidbtype *isisidb;
    int route_count, server_count;

    pdb = context->pdb;			/* Cheaper than on the stack */
    idb = context->lsp_idb;
    if (idb && (idb->primary_nidb == NULL)) /* Something's wrong */
	return;

    /*
     * For non-pseudonode LSPs, add all WAN adjacencies, the pseudonode
     * for each LAN, and the pseudonode for each RIP/SAP stub.
     */
    if (idb == NULL) {
	reg_invoke_ipx_return_static_counts(&route_count, &server_count);

	adj = NULL;
	cur_adj = NULL;
	while ((adj = next_neighbor(pdb->adjacency_db, NULL,
				    CLNS_NLSP_IS_ADJ_TYPE, 
				    cur_adj, &bucket))) {
	    cur_adj = adj;  /* Save a copy of where we are in the adj table */
	    for (; adj; adj = adj->next_alias) {
		if (!is_p2p(adj->idb))
		    continue;
		if (!isis_ipx_add_is_adj(adj, context, FALSE))
		    return;
	    }
	}
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->nov_lsp_enabled && 
		nidb->nov_lsp_isis) /* IS-IS running on this i/f */
		continue;
	    cur_idb = nidb->idb;
	    isisidb = get_isis_idb(pdb, cur_idb);
	    if (!isisidb)
		continue;
	    if (!isisidb->isis_interface_up)
		continue;

	    /*
	     * Skip WAN links on which we are not the DR, since there is
	     * only a WAN pseudonode if we are the DR (this happens if
	     * there are no NLSP adjacencies on the link).
	     */
	    if (is_p2p(cur_idb) && !isis_we_are_dr(pdb, cur_idb, CLNS_LEVEL1))
		continue;

	    /*
	     * If statics are present, add an adjacency to the internal
	     * network.  Make it look like an ethernet.
	     */
	    if (nidb->internal_network) {
		if (route_count || server_count) {
		    if (!isis_ipx_add_link_item(context, NLSP_STATIC_COST,
						isisidb->isis_L1_circuit_id,
						NLSP_STATIC_MTU,
						NLSP_STATIC_BANDWIDTH,
						NLSP_STATIC_DELAY,
						NLSP_TYPE_GENERIC_LAN)) {
			return;
		    }
		}
	    } else {
		if (!isis_ipx_add_link_item(context, isisidb->isis_L1_metric,
					    isisidb->isis_L1_circuit_id, 
					    max_proto_mtu(cur_idb,
							  LINK_NOVELL),
					    ipx_nlsp_link_bandwidth(nidb),
					    ipx_nlsp_link_delay(nidb),
					    nidb->nov_nlsp_media)) {
		    return;
		}
	    }
	}
    } else {

	/*
	 * Pseudonode LSP.  Add the router itself, plus all adjacencies
	 * if this is a LAN running NLSP.
	 */
	isisidb = get_isis_idb(pdb, idb);
	if (isisidb) {
	    if (!isis_ipx_add_link_item(context, 0, pdb->systemId, 0, 0, 0,
					idb->primary_nidb->nov_nlsp_media)) {
		return;
	    }
	    if (!is_p2p(idb) && isisidb->isis_running) {
		adj = NULL;
		cur_adj = NULL;
		while ((adj = next_neighbor(pdb->adjacency_db, idb,
					    CLNS_NLSP_IS_ADJ_TYPE, 
					    cur_adj, &bucket))) {
		    cur_adj = adj;  /* Save a copy */
		    for (; adj; adj = adj->next_alias) {
			if (!isis_ipx_add_is_adj(adj, context, TRUE))
			    return;
		    }
		}
	    }
	}
    }
}

/*
 * isis_insert_mgmt_info
 *
 * Build the management info item.
 *
 * Returns FALSE if out of memory, TRUE otherwise.
 */
static boolean isis_insert_mgmt_info (isis_lspcontext *context)
{
    isis_nlsp_mgmt_info *mgmt_info;
    isisidbtype *isisidb;
    char *server_name;
    ulong server_name_len;
    isis_info_type *item;
    clns_pdbtype *pdb;
    idbtype *idb;
    nidbtype *nidb;

    pdb = context->pdb;			/* Cheaper than on the stack */
    idb = context->lsp_idb;
    nidb = NULL;
    if (idb) {
	nidb = idb->primary_nidb;
	if (!nidb)
	    return(TRUE);		/* Weird. */
    }

    isisidb = get_isis_idb(pdb, idb);
    if (idb && !isisidb)
	return(TRUE);

    /* Add management info if the interface is up. */

    if (!idb || isisidb->isis_interface_up) {
	if (idb) {				/* DR LSP */
	    if (nidb->internal_network) {
		server_name = "Statics";
	    } else {
		server_name = idb->description;
		if (server_name == NULL)
		    server_name = idb->namestring;
	    }
	} else {
	    server_name = hostname;
	}
	if (server_name != NULL) {
	    server_name_len = min(strlen(server_name), ISIS_NLSP_SVC_MAXLEN);
	} else {
	    server_name_len = 0;
	}
	
	item = isis_new_item(context, ISIS_NLSP_MGMT_INFO,
			     ISIS_NLSP_MGMT_INFO_LEN + server_name_len, 
			     (void **) &mgmt_info, NULL, TRUE);
	if (!item)
	    return(FALSE);
	
	if (idb) {	/* Pseudonode */
	    
	    /*
	     * For pseudonode LSPs, put the net and node for this interface
	     * into the management info block.  WAN pseudonodes (and the
	     * static pseudonode) get a zero node number.  Unnumbered
	     * WAN pseudonodes get a zero net number as well.
	     */
	    if (nidb->internal_network || is_p2p(idb)) {
		ieee_zero(mgmt_info->node_number);
	    } else {
		ieee_copy(idb->novell_node, mgmt_info->node_number);
	    }

	    if (NOVELL_NIDB_UNNUMBERED(nidb) || nidb->internal_network) {
		PUTLONG(&mgmt_info->net_number, 0);
	    } else {
		PUTLONG(&mgmt_info->net_number, nidb->novellnet);
	    }

	} else {				/* non-Pseudonode */
	    
	    /* Non-pseudonode LSP.  Put the internal address in. */
	    
	    PUTLONG(&mgmt_info->net_number, ipx_internal_net);
	    ieee_copy(ipx_internal_hostid, mgmt_info->node_number);
	}
	
	mgmt_info->ipx_version_number = NLSP_IPX_VERSION_NUMBER;
	mgmt_info->router_name_length = server_name_len;
	if (server_name != NULL) {
	    bcopy(server_name, mgmt_info->router_name, server_name_len);
	}
	ADVANCE_ITEM(item, TRUE);		/* Length guaranteed nonzero */
	isis_item_done(context, item);
    }
    return(TRUE);
}

/*
 * ipx_nlsp_check_summary
 *
 * See if this net falls into the ranges of any configured
 * summary routes. If so, do not inject this route.
 */
static boolean ipx_nlsp_check_summary (novndbtype *ndb, novpdbtype *pdb)
{
    nov_sumlisttype *item;
    novpdbtype *src_pdb;
    int i;

    if (!pdb->osi_pdb->enable_summary) 
	return TRUE;
    if (!pdb->sumlist[ndb->novpdbindex]) {
	/*
	 * No manual summary, check automatic summary using the the area 
	 * addresses if the route is learned via another NLSP instances.
	 * Area address is defined as the union of both the manual area 
	 * addresses and the computed area addresses.
	 */
	src_pdb = NOV_INDEX2PDB(ndb->novpdbindex);
	if (!src_pdb->osi_pdb || src_pdb->proctype != NET_NLSP)
	    return TRUE;
	for (i = 0; i < src_pdb->osi_pdb->num_nlsp_union_areas; i++) {
	    if (src_pdb->osi_pdb->nlsp_union_area[i] == 
		(src_pdb->osi_pdb->nlsp_union_area_mask[i] & ndb->dstnet))
		return FALSE;
	}
    }
    item = ipx_get_sum_acl(pdb->sumlist[ndb->novpdbindex]); 
    if (!item)
	return TRUE;
    do {
	if ((item->net != -1L) && ((ndb->dstnet & item->mask) != item->net))
	    continue;
	if ((item->idb != NULL) && !nov_same_idb(item->idb, ndb->path_list))
	    continue;
	return item->grant;
    } while ((item = item->next));
    return FALSE;
}

/*
 * ipx_nlsp_inject_sroute_patricia
 *
 */
static int ipx_nlsp_inject_sroute_patricia (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    isis_nlsp_summary_routes *sumentry;
    nidbtype *nidb;
    idbtype *idb;
    clns_pdbtype *pdb;
    isis_lspcontext *context = va_arg(ap, isis_lspcontext *);    
    novndbtype *cur_ndb;

    if (context->lsp == NULL)
	return NOV_WALK_ABORT;

    pdb = context->pdb;

    if (!(pdb->isis_mode & ISIS_IPX_MODE)) {
	return NOV_WALK_ABORT;
    }

    nidb = NULL;
    idb = context->lsp_idb;
    if (idb) {
	nidb = idb->primary_nidb;
	if (!nidb)
	    return NOV_WALK_ABORT;
    }

    sumroute = (nov_sumroutetype *)rnnode;
 
    if (sumroute && !TIMER_RUNNING(sumroute->ndb->holddownuntil)) {
	cur_ndb = sumroute->ndb;

	/* Don't pick up "drop" routes. */
	if (nidb == nullnidb)
	    return NOV_WALK_CONTINUE;
	/*
	 * Pick up only routes from other (dynamic) protocols learned
	 * via the this interface. (For the internal net interface, pick
	 * up only statics.) Do this only when redistribution allows.
         */
	if (!(INDEX2MASK(cur_ndb->novpdbindex) & pdb->ipx_pdb->distflag) ||
	    (pdb->ipx_pdb->index == cur_ndb->novpdbindex))
	    return NOV_WALK_CONTINUE;
	    
	if (nidb->internal_network) {
	    if (!(cur_ndb->flags & NET_STATIC))
		return NOV_WALK_CONTINUE;
	} else {
	    if (!(cur_ndb->flags & (NET_RIP | NET_IGRP | NET_NLSP)))
		return NOV_WALK_CONTINUE;
	    /*
	     * Only import summary if learned on this IDB:
	     */
	    if (!nov_same_idb(idb, cur_ndb->path_list))
		return NOV_WALK_CONTINUE;
	}
	/*
	 * Reject any external summary that will be subsumed by any
	 * configured summaries (area address or summary access list).
	 */
	if (!ipx_nlsp_check_summary(cur_ndb, pdb->ipx_pdb))
	    return NOV_WALK_CONTINUE;

	/*
	 * Are we about to cross an NLSP area boundary? If so, only pass
	 * this address summary to the other area
	 * if our count is at least one.
	 */
	if (NOV_INDEX2PDB(cur_ndb->novpdbindex)->proctype == NET_NLSP)
	    if (cur_ndb->area_count == 0)
		return NOV_WALK_CONTINUE;

	if ((isis_nlsp_spf_event_debug[pdb->proc_type] &&
	    !isis_nlsp_terse_spf_event_debug[pdb->proc_type]) ||
	    NOVNLSP_DEBUG(cur_ndb->dstnet, pdb->ipx_pdb)) {
	    buginf("\n%s-SPF:  Import route %x %x to LSP SROUTE list "
		   "for idb %s", pdb->errmsg_name, cur_ndb->dstnet, 
		   cur_ndb->rt_mask, idb->namestring);
	}

	/*
	 * Create a new LSP if we are hashing and we have not yet done so:
	 */
	if (pdb->lsp_agg_frag) {
	    if (!isis_create_specific_lsp(context, pdb->lsp_agg_frag)) {
		return NOV_WALK_ABORT;
	    }
	    if (!isis_new_item(context, ISIS_NLSP_SUMMARY_ROUTES,
			       0, NULL, NULL, TRUE)) {
		return NOV_WALK_ABORT;
	    }
	    pdb->lsp_agg_frag = 0;
	}

	sumentry = isis_add_item_entry(context, ISIS_NLSP_SUMMARY_ROUTES_LEN,
				       FALSE, NULL);
	if (!sumentry)
	    return NOV_WALK_ABORT;
	/*
	 * Allow this LSP to be suppressed if it is identical
	 * to an active LSP. This determination will be made
	 * later by isis_lsp_avl_insert()
	 */
	context->lsp->lsp_suppress = pdb->lsp_hash_enable; /* %%% */

	sumentry->area_count = cur_ndb->area_count - 1;
	/*
	 * Count the number of leading ones to get the mask length 
         * NOTE: all leading zeros are counted as one.
	 */
	sumentry->masklen = ipx_bitsinmask(cur_ndb->rt_mask);
	PUTLONG(&sumentry->address, cur_ndb->dstnet);
	PUTSHORT(&sumentry->ticks, cur_ndb->delay);
    }
    return NOV_WALK_CONTINUE;
}	    

/*
 * ipx_nlsp_area_address_summary_depend
 *
 */
static void ipx_nlsp_area_address_summary_depend (novndbtype *ndb, idbtype *idb)
{
    novpdbtype *src_pdb;
    int match = -1;
    int i;

    src_pdb = NOV_INDEX2PDB(ndb->novpdbindex);
    if (!src_pdb->osi_pdb || src_pdb->proctype != NET_NLSP)
	return;

    if ((idb != NULL) && (!nov_same_idb(idb, ndb->path_list))) {
	if (novellredist_debug)
	    buginf("\nIPX-DIST: net %x area reject, lsp %s",
		   ndb->dstnet, idb->short_namestring);
	return;
    }

    for (i = 0; i < src_pdb->osi_pdb->num_nlsp_union_areas; i++) {
	if (src_pdb->osi_pdb->nlsp_union_area[i] == 
	    (src_pdb->osi_pdb->nlsp_union_area_mask[i] & ndb->dstnet)) {
	    if ((match == -1) || 
		(src_pdb->osi_pdb->nlsp_union_area_mask[match] <
		 src_pdb->osi_pdb->nlsp_union_area_mask[i]))
		match = i;
	}
    }
    if (match != -1)
	src_pdb->osi_pdb->nlsp_union_area_match_count[match]++;
}

/*
 * ipx_nlsp_filtercheck
 * Check if the specified network number is permitted.
 */
static boolean ipx_nlsp_filtercheck (ulong net, ulong mask, int accesslist)
{
    nov_sumlisttype *item;

    if (!accesslist)
	return(TRUE);
    /*
     * 800 list? ipx code should handle it.
     */
    if (accesslist >= MINNOVACC && accesslist <= MAXNOVACC)
	return (nov_filtercheck(net, accesslist));
    /*
     * List out of range? No good!
     */
    if (accesslist < MINNOVSUM || accesslist > MAXNOVSUM)
	return TRUE;

    item = ipx_get_sum_acl(accesslist);

    if (item == NULL)
	return(TRUE);

    do {
	if (item->mask != mask)
	    continue;
	if ((item->net != -1L) &&
	    (item->net != (net & item->mask)))
	    continue;
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}

/*
 * ipx_nlsp_distribute_list_check
 */
static boolean ipx_nlsp_distribute_list_check (novndbtype *ndb, novpdbtype *pdb)
{
    novpdbtype *src_pdb;

    if (!(ndb->flags & (NET_RIP | NET_IGRP | NET_STATIC | NET_NLSP | 
			NET_XROUTE)))
	return FALSE;

    if (pdb->proctype != NET_NLSP)
	return FALSE;
    
    if (!ipx_route_valid(ndb)) {
	return FALSE;
    } 
    /* In filter check */
    if (!ipx_nlsp_filtercheck(ndb->dstnet, ndb->rt_mask, pdb->ifltr_all)) 
	return FALSE;
    
    /* Out filter check */
    src_pdb = NOV_INDEX2PDB(ndb->novpdbindex);
    if (!ipx_nlsp_filtercheck(ndb->dstnet, ndb->rt_mask, src_pdb->ofltr_all))
	return FALSE;
    if (!ipx_nlsp_filtercheck(ndb->dstnet, ndb->rt_mask,
			 ndb->path_list->nidb->oprotofltr[pdb->index]))
	return FALSE;
    if (!ipx_nlsp_filtercheck(ndb->dstnet, ndb->rt_mask,
			 src_pdb->ofltr_proto[pdb->index]))
	return FALSE;
    
    if (!(INDEX2MASK(ndb->novpdbindex) & pdb->distflag) ||
	(pdb->index == ndb->novpdbindex))
	return FALSE;

    return TRUE;
}

/* 
 * ipx_nlsp_summary_depend
 *
 * cruise the summaries and bump the match count if the route falls
 * in the the range of any configured aggregate addresses.
 */
static void ipx_nlsp_summary_depend (novndbtype *ndb, novpdbtype *pdb, idbtype *idb)
{
    nov_sumlisttype *sum, *match;

    if (!ipx_nlsp_distribute_list_check(ndb, pdb))
	return;

    if (!pdb->osi_pdb->enable_summary) 
	return;
    if (!pdb->sumlist[ndb->novpdbindex]) {
	/*
	 * Check automatic summary using the the area 
	 * addresses if the route is learned via another NLSP instances.
	 * Area address is defined as the union of both the manual area 
	 * addresses and the computed area addresses.
	 */
	ipx_nlsp_area_address_summary_depend(ndb, idb);
	return;
    }

    match = NULL;
    for (sum = ipx_get_sum_acl(pdb->sumlist[ndb->novpdbindex]);
	 sum; sum = sum->next) {

	/*
	 * Check for match:
	 *  - Access list must match summary.
	 *  - If summary is interface specific, interface must match.
	 *  - If summary is for pseudonode lsp, pseudonode idb must match.
	 */
	if (sum->net != (ndb->dstnet & sum->mask))
	    continue;

	if ((sum->idb != NULL) && (!nov_same_idb(sum->idb, ndb->path_list))) {
	    if (novellredist_debug)
		buginf("\nIPX-DIST: net %x reject sum %x %x sum->idb %s",
		       ndb->dstnet, sum->net, sum->mask,
		       sum->idb->short_namestring);
	    continue;
	}

	if ((idb != NULL) && (!nov_same_idb(idb, ndb->path_list))) {
	    if (novellredist_debug)
		buginf("\nIPX-DIST: net %x reject sum %x %x lsp idb %s",
		       ndb->dstnet, sum->net, sum->mask,
		       idb->short_namestring);
	    continue;
	}

	/* save the longest match */
	if ((match == NULL) || (match->mask < sum->mask))
	    match = sum;
    }
    /*
     * We have a match. Bump count so later we know to generate
     * an aggregated route.
     */
    if (match) {
	match->match_count++;	
	if (novellredist_debug)
	    buginf("\nIPX-DIST: net %x matches sum %x %x "
		   " %d matches, match->idb %s, lsp %s",
		   ndb->dstnet, match->net, match->mask, match->match_count,
		   (match->idb ? match->idb->short_namestring : "NULL"),
		   (idb ? idb->short_namestring : "NULL"));
    }
}

/* 
 * ipx_nlsp_summary_depend_patricia 
 *
 * Cruise the summaryQ and bump the area address or summary access
 * list match count if the summary falls in the the range of any configured
 * aggregated addresses.
 */
static int ipx_nlsp_summary_depend_patricia (rntype *rnnode, va_list ap)
{
    novpdbtype *pdb = va_arg(ap, novpdbtype *);
    nov_sumlisttype *sum, *match = NULL;
    nov_sumroutetype *net;
    novndbtype *ndb;
    isis_lspcontext *context = va_arg(ap, isis_lspcontext *);    
    idbtype *idb;

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;    

    ndb = net->ndb;

    idb = context->lsp_idb;

    if (!ipx_nlsp_distribute_list_check(ndb, pdb))
	return NOV_WALK_CONTINUE;

    if (!pdb->osi_pdb->enable_summary) 
	return NOV_WALK_CONTINUE;

    if (!pdb->sumlist[ndb->novpdbindex]) {
	/*
	 * Check automatic summary using the the area addresses
	 * if the route is learned via another NLSP instance. Area
	 * address is defined as the union of both the manual area 
	 * addresses and the computed area addresses.
	 */
	ipx_nlsp_area_address_summary_depend(ndb, NULL);
	return NOV_WALK_CONTINUE;
    }

    for (sum = ipx_get_sum_acl(pdb->sumlist[ndb->novpdbindex]); sum;
	 sum = sum->next) {

	if (sum->net != (ndb->dstnet & sum->mask))
	    continue;

	if ((idb != NULL) && (!nov_same_idb(idb, ndb->path_list)))
	    continue;

	/* save the longest match */
	if ((match == NULL) || (match->mask < sum->mask))
	    match = sum;
    }
    /*
     * We have a match. Bump count so later we know to generate
     * an aggregated route.
     */
    if (match) {
	match->match_count++;	
	if (novellredist_debug)
	    buginf("\nIPX-DIST: net %x matches sum %x %x "
		   " %d matches, match->idb %s, lsp %s",
		   ndb->dstnet, match->net, match->mask, match->match_count,
		   (match->idb ? match->idb->short_namestring : "NULL"),
		   (idb ? idb->short_namestring : "NULL"));
    }
    return NOV_WALK_CONTINUE;
}

/*
 * ipx_nlsp_add_summary_tlv
 *
 * For each item in the current configuration which might generate
 * an aggregated route, check the match count. (This is the count
 * of routes which which matched an area address or explicit access
 * list entry corresponding to a possible aggregated route.)
 *
 * If the match count is non-zero, add the corresponding aggregated
 * route to the current lsp. Also add an element to the lsp match
 * list to remember the reference.
 *
 * If the match count is zero, remove the corresponding reference in the
 * lsp match list. If the list was non-NULL but is now NULL, remove the
 * corresponding drop route from the route tables.
 */
static void ipx_nlsp_add_summary_tlv (int net, int mask, int count, int area_count, 
			       int ticks, isis_lspcontext *context)
{
    novndbtype *ndb;
    isis_nlsp_summary_routes *sumentry;
    clns_pdbtype *pdb;
    idbtype *idb;
    boolean debug;

    pdb = context->pdb;
    idb = context->lsp_idb;

    debug = ((isis_nlsp_spf_event_debug[pdb->proc_type] &&
	      !isis_nlsp_terse_spf_event_debug[pdb->proc_type]) ||
	     NOVNLSP_DEBUG(net, pdb->ipx_pdb));

    if (debug)
	buginf("\n%s-SPF:  Summary route %x %x @[%d/%d], %d refs (%s)",
	       pdb->errmsg_name, net, mask, ticks, area_count, count,
	       (idb ? idb->short_namestring : "non-pseudonode"));

    if (!count) {
	/*
	 * Remove the "drop" route if there is an aggregate
	 * in our table and now there is no more specific 
	 * that falls into this range.
	 */
	ndb = novrip_find_rt(net, mask, IPX_ROUTE_MATCH_EXACT);
	if (ndb && ndb->path_list && ndb->path_list->nidb == nullnidb &&
	    ndb->novpdbindex == pdb->ipx_pdb->index) {
	    if (ipx_nlsp_dequeue_lsp_desc(ndb, idb)) {
		novrip_del_rt(net, mask, IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	}
    } else {
	if (debug)
	    buginf("\n%s-SPF:  Adding Route %x %x to LSP SROUTE list",
		   pdb->errmsg_name, net, mask);
	/*
	 * Install a summary address into our forwarding table having 
	 * a nullnidb to indicate "drop"
	 */
	novrip_add_rt(pdb->ipx_pdb, nullnidb, net, mask, 0, 0, 
		      NOVELL_DEFAULT_CONNECTED_TICKS,
		      NOVELL_DEFAULT_RIP_HOPS,
		      NET_SUMMARY | NET_NLSP, IPX_ROUTE_ADD_NORMAL, 
		      IPX_NDB_OPTION_STATIC_NO_NVGEN, IPX_STATIC_TYPE_HOST,
		      IPX_FLOATING_STATIC_EXISTS_NO);

	ndb = novrip_find_rt(net, mask, IPX_ROUTE_MATCH_EXACT);
	if (ndb && ndb->path_list && ndb->path_list->nidb == nullnidb &&
	    ndb->novpdbindex == pdb->ipx_pdb->index) {
	    ipx_nlsp_enqueue_lsp_desc(ndb, idb);
	}
	/*
	 * Create a new LSP if we are hashing and we have not yet done so:
	 */
	if (pdb->lsp_agg_frag) {
	    if (!isis_create_specific_lsp(context, pdb->lsp_agg_frag)) {
		return;
	    }
	    if (!isis_new_item(context, ISIS_NLSP_SUMMARY_ROUTES,
			       0, NULL, NULL, TRUE)) {
		return;
	    }
	    pdb->lsp_agg_frag = 0;
	}
	/*
	 * Insert the aggregated route into the current lsp:
	 */
	sumentry = isis_add_item_entry(context, ISIS_NLSP_SUMMARY_ROUTES_LEN,
				       FALSE, NULL);
	if (!sumentry)
	    return;
	/*
	 * Allow this LSP to be suppressed if it is identical
	 * to an active LSP. This determination will be made
	 * later by isis_lsp_avl_insert()
	 */
	context->lsp->lsp_suppress = pdb->lsp_hash_enable; /* %%% */
	
	sumentry->area_count = area_count;
	/*
	 * Count the number of leading ones to get the mask 
	 * length.
	 * NOTE: all leading zeros are counted as one.
	 */
	sumentry->masklen = ipx_bitsinmask(mask);
	PUTLONG(&sumentry->address, net);
	PUTSHORT(&sumentry->ticks, ticks);
    }
}

/*
 * isis_generate_summaries
 * 
 * Insert aggregated routes into the current lsp.
 *
 * We assume that the match counts for all area addresses and summary
 * access lists have been incremented by all (permmitted) aggregated
 * and external routes which match a possible summary.
 *
 * We cruise both the area addresses (implicit triggers) and summary access
 * lists (explicit triggers) and either add a summary (if the match count
 * is non-zero) or delete an existing summary (if the match count is zero
 * and this lsp is the last reference matching that summary). (See
 * ipx_nlsp_add_summary_tlv() for details.)
 *
 * As the match count check is completed for each data structure, we
 * reset the match counts for the next sweep (either a different lsp
 * or a later SPF).
 */
static void isis_generate_summaries (isis_lspcontext *context)
{
    novpdbtype *dist;
    int accnum, i;
    nov_sumlisttype *list;
    clns_pdbtype *pdb;

    pdb = context->pdb;

    for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	accnum = pdb->ipx_pdb->sumlist[dist->index]; 
	if (!accnum) {
	    if (!dist->osi_pdb || dist->osi_pdb == pdb ||
		dist->proctype != NET_NLSP)
		continue;
	    for (i = 0; i < dist->osi_pdb->num_nlsp_union_areas; i++) {
		ipx_nlsp_add_summary_tlv
		    (dist->osi_pdb->nlsp_union_area
		     [i], dist->osi_pdb->nlsp_union_area_mask[i], 
		     dist->osi_pdb->nlsp_union_area_match_count[i], 
		     NOVELL_DEFAULT_AREA_COUNT, 1, context);
		dist->osi_pdb->nlsp_union_area_match_count[i] = 0;
	    }
	    continue;
	}
	for (list = ipx_get_sum_acl(accnum); list; list = list->next) {
	    /*
	     * Don't do anything with the permit or the "all"
	     * network entry access-lists.
	     */
	    if (list->grant || list->net == -1L) 
		continue;
	    /*
	     * Generate summary (or delete drop route):
	     */
	    ipx_nlsp_add_summary_tlv(list->net, list->mask, list->match_count,
				     list->area_count, list->ticks, context);
	}
	/*
	 * Sweep the entire list clean before processing next idb:
	 */
	for (list = ipx_get_sum_acl(accnum); list; list = list->next) {
	    list->match_count = 0;
	}	
    }
}

/*
 * isis_insert_ipx_lsp
 * 
 * Insert IPX info into the LSP.
 *
 * context->lsp_idb is a pointer to the IDB if this is a pseudonode LSP,
 * or NULL otherwise.
 *
 * Note--this routine may suspend!
 */
void isis_insert_ipx_lsp (isis_lspcontext *context)
{
    idbtype *idb;
    nidbtype *nidb;
    clns_pdbtype *pdb;
    isis_info_type *item;
    novndbtype *cur_ndb = NULL;
    novell_sapentrytype *cur_svc;
    int table_index;
    isis_nlsp_ext_routes *entry;
    int name_length;
    isis_nlsp_svcs_info *sentry;
    boolean debug;

    if (context->lsp == NULL)
	return;

    pdb = context->pdb;
    if (!pdb || !pdb->ipx_pdb)
	return;

    if (!(pdb->isis_mode & ISIS_IPX_MODE)) {
	return;
    }

    /*
     * Hash if enabled:
     */
    if (pdb->lsp_hash_enable) {
	isis_insert_ipx_lsp_hashed(context);
	return;
    }
    
    nidb = NULL;
    idb = context->lsp_idb;
    if (idb) {
	nidb = idb->primary_nidb;
	if (!nidb)
	    return;
    }

    debug = ((isis_nlsp_spf_event_debug[pdb->proc_type] &&
	      !isis_nlsp_terse_spf_event_debug[pdb->proc_type]));

    /* Add management info. */

    if (!isis_insert_mgmt_info(context))
	return;

    /* Add link info. */

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis_add_nlsp_links(context);
    }

    /* Add Xroutes. */

    if (idb) {				/* pseudonode */

	/* Start the new TLV. */

	if (!isis_new_item(context, ISIS_NLSP_EXT_ROUTES, 0, NULL,
			   NULL, TRUE))
	    return;

	/* Loop over the entire IPX route table. */

	for (;;) {
	    cur_ndb = nov_get_each_route(cur_ndb, &table_index, TRUE);
	    if (cur_ndb == NULL) break;

	    if (!ipx_nlsp_distribute_list_check(cur_ndb, pdb->ipx_pdb))
		continue;
	    /*
	     * For real interfaces, pick up RIP/EIGRP and other pdb's NLSP 
	     * routes for our interface. For the internal net interface, 
	     * pick up only statics.
	     */

	    if (nidb->internal_network) {
		if (!(cur_ndb->flags & NET_STATIC)) 
		    continue; 
	    } else {
		if (!(cur_ndb->flags & (NET_RIP | NET_IGRP | NET_NLSP)))
		    continue;
		
		/* Only advertise RIP if learned on our IDB */

		if (!nov_same_idb(idb, cur_ndb->path_list))
		    continue;
	    }
	    /*
	     * Only add XROUTE if not subsumed by a summary:
	     */
	    if (ipx_nlsp_check_summary(cur_ndb, pdb->ipx_pdb)) {
		if (debug || NOVNLSP_DEBUG(cur_ndb->dstnet, pdb->ipx_pdb)) {
		    buginf("\n%s-SPF:  Adding Route %x %x to LSP XROUTE list "
			   "for idb %s", pdb->errmsg_name, cur_ndb->dstnet, 
			   cur_ndb->rt_mask, idb->short_namestring);
		}
		entry = isis_add_item_entry(context,
					    ISIS_NLSP_EXT_ROUTES_LEN,
					    FALSE, NULL);
		if (!entry)
		    return;			/* Deep trouble! */
		if (cur_ndb->flags & NET_IGRP) {
		    if (cur_ndb->flags & NET_DEXTERIOR)
			entry->hops = cur_ndb->ext_hop;
		    else 
			entry->hops = cur_ndb->ext_hop + 1;
		    PUTSHORT(&entry->ticks, 
			     (cur_ndb->ext_delay == NOVELL_INFDELAY ?
			      NOVELL_INFDELAY : (cur_ndb->ext_delay + 
				 NOVELL_IDB_TICKS(IPX_PRIMARY_NIDB(idb)))));
		} else {
		    entry->hops = cur_ndb->hopcount;
		    PUTSHORT(&entry->ticks, cur_ndb->delay);
		}
		PUTLONG(&entry->net_number, cur_ndb->dstnet);
	    }
	}
	isis_item_done(context, NULL);
    }

    /* Add Sroutes. */

    pdb->lsp_agg_frag = 0; /* Just in case ... used only when hashing */

    if (idb) {				/* pseudonode */

	if (!isis_new_item(context, ISIS_NLSP_SUMMARY_ROUTES,
			   0, NULL, NULL, TRUE)) {
	    return;
	}
	/*
	 * Walk the patricia tree importing summary
	 * routes from other areas, as appropriate:
	 */
	rn_walktree_blocking(nov_rnhead->rnh_treetop, 
			     ipx_nlsp_inject_sroute_patricia, context);

	if (!pdb->non_pnode_summary) {
	    /*
	     * Loop over the entire IPX route table, counting each
	     * route that matches an implicit (area address) or
	     * explicit (access list) summary.
	     */
	    for (;;) {	    
		cur_ndb = nov_get_each_route(cur_ndb, &table_index, TRUE);
		if (cur_ndb == NULL) break;
		ipx_nlsp_summary_depend(cur_ndb, pdb->ipx_pdb, idb);
	    }
	    /*
	     * Walk the patricia tree again and count each
	     * summary route that matches an implicit or explicit
	     * summary (triggering a more inclusive summary).
	     */
	    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
				 ipx_nlsp_summary_depend_patricia,
				 pdb->ipx_pdb, context);
	    /*
	     * Finally, generate aggregated routes in
	     * this pseudonode lsp as appropriate:
	     *
	     * Include the deny summary lists that have non-zero route count if
	     * manual summaries are configured, include the area addresses 
	     * that have the non-zero route count (automatic summaries)
	     */
	    isis_generate_summaries(context);
	}
	isis_item_done(context, NULL);
    }
    if (!idb) {                         /* non-pseudonode */

	if (pdb->non_pnode_summary) {
	    /*
	     * Start the new TLV.
	     */
	    if (!isis_new_item(context, ISIS_NLSP_SUMMARY_ROUTES,
			       0, NULL, NULL, TRUE)) {
		return;
	    }
	    
	    /* Loop over the entire IPX route table. */
	    for (;;) {	    
		cur_ndb = nov_get_each_route(cur_ndb, &table_index, TRUE);
		if (cur_ndb == NULL) break;
		ipx_nlsp_summary_depend(cur_ndb, pdb->ipx_pdb, NULL);
	    }
	    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
				 ipx_nlsp_summary_depend_patricia,
				 pdb->ipx_pdb, context);
	    
	    isis_generate_summaries(context);

	    isis_item_done(context, NULL);
	}
    }
    
    /* Add services info. */

    if (idb) {				/* pseudonode */
	cur_svc = NULL;

	/* Loop over the entire IPX service table. */

	for (;;) {
	    cur_svc = nov_get_each_service(cur_svc, &table_index, TRUE);
	    if (cur_svc == NULL) break;
	    if (cur_svc->server.nethops == novell_infinity)
		continue;
	    /*
	     * Do report all other services, with the exception
	     * of those static services which are not reachable.
	     */
	    
	    if (cur_svc->flags & NET_STATIC) {
		if (!novrip_find_rt(cur_svc->server.dnet,
				    NOV_NETMASK_ALL,
				    IPX_ROUTE_MATCH_FUZZ)) {
		    continue;
		}
	    }
	    /* Don't pick up services learned via this NLSP instance */
	    if (!(INDEX2MASK(cur_svc->novpdbindex) & pdb->ipx_pdb->distflag) ||
		(pdb->ipx_pdb->index == cur_svc->novpdbindex))
		continue;

	    if (!ipx_distribute_sap_list_check(cur_svc, pdb->ipx_pdb, 
					       IPX_PRIMARY_NIDB(idb)))
		continue;
	    /*
	     * For real interfaces, pick up only SAP services for our
	     * interface.  For the internal net interface, pick up only 
	     * statics.  Ignore infinitely distant services.
	     */
	    if (nidb->internal_network) {
		if (!(cur_svc->flags & NET_STATIC)) 
		    continue;
	    } else {
		if (!(cur_svc->flags & (NET_RIP | NET_IGRP | NET_NLSP)))
		    continue;
		if (pdb->ipx_pdb->index == cur_svc->novpdbindex)
		    continue;
		if (!nov_srdb_same_idb(idb, cur_svc->paths))
		    continue;
	    }

	    name_length = min(strlen(cur_svc->server.server_name),
			      ISIS_NLSP_SVC_MAXLEN);
	    item = isis_new_item(context, ISIS_NLSP_SVCS_INFO, 
				 ISIS_NLSP_SVCS_INFO_LEN +
				 name_length, 
				 (void **) &sentry, NULL, TRUE);
	    if (item == NULL)
		return;
	    sentry->hops = cur_svc->server.nethops;
	    PUTLONG(&sentry->net_number, GETLONG(&cur_svc->server.dnet));
	    bcopy(cur_svc->server.dhost, sentry->node_number, IEEEBYTES);
	    PUTSHORT(&sentry->socket, GETSHORT(&cur_svc->server.dsock));
	    PUTSHORT(&sentry->type, GETSHORT(&cur_svc->server.server_type));
	    bcopy(cur_svc->server.server_name, sentry->name, name_length);
	    ADVANCE_ITEM(item, TRUE);
	    isis_item_done(context, item);
	}
    }
}

/*
 * isis_insert_ipx_lsp_hashed
 * 
 * Insert IPX info into the LSP.
 *
 * context->lsp_idb is a pointer to the IDB if this is a pseudonode LSP,
 * or NULL otherwise.
 *
 * Note--this routine may suspend!
 */
void isis_insert_ipx_lsp_hashed (isis_lspcontext *context)
{
    idbtype *idb;
    nidbtype *nidb;
    clns_pdbtype *pdb;
    isis_info_type *item;
    novndbtype *cur_ndb = NULL;
    novell_sapentrytype *cur_svc;
    int table_index;
    uchar lsp_index = 0;
    int ndb_item_count;
    int svc_item_count;
    isis_nlsp_ext_routes *entry;
    int name_length;
    isis_nlsp_svcs_info *sentry;
    isis_lspdbtype *old_lsp;
    boolean debug;
    
    if (context->lsp == NULL)
  	return;
    
    pdb = context->pdb;
    if (!pdb || !pdb->ipx_pdb)
  	return;
    
    if (!(pdb->isis_mode & ISIS_IPX_MODE)) {
  	return;
    }
    
    nidb = NULL;
    idb = context->lsp_idb;
    if (idb) {
  	nidb = idb->primary_nidb;
  	if (!nidb)
  	    return;
    }
    
    debug = ((isis_nlsp_spf_event_debug[pdb->proc_type] &&
  	      !isis_nlsp_terse_spf_event_debug[pdb->proc_type]));
    
    /* Add management info. */
    
    if (!isis_insert_mgmt_info(context))
  	return;
    
    /* Add link info. */
    
    if (pdb->proc_type == PROC_TYPE_NLSP) {
  	isis_add_nlsp_links(context);
    }
    
    /* Add Xroutes. */
    
    if (idb) {				/* pseudonode */
 	
  	table_index = 0;
  	ndb_item_count = 0;
  	cur_ndb = NULL;
 	
  	/* Loop over the entire IPX route table. */
 	
  	for (;;) {
  	    for (;;) {
  		cur_ndb = nov_get_each_route_per_index(cur_ndb,
  						       &table_index,
  						       TRUE);
  		if (cur_ndb == NULL) break;
		
 		/* Ignore routes in holddown. */
 		
 		if (!ipx_route_valid(cur_ndb))
 		    continue;

		if (!ipx_nlsp_distribute_list_check(cur_ndb, pdb->ipx_pdb))
		    continue;
		/*
		 * For real interfaces, pick up RIP/EIGRP and other pdb's NLSP 
		 * routes for our interface. For the internal net interface, 
		 * pick up only statics.
		 */
  		if (nidb->internal_network) {
  		    if (!(cur_ndb->flags & NET_STATIC)) 
  			continue; 
  		} else {
  		    if (!(cur_ndb->flags & (NET_RIP | NET_IGRP | NET_NLSP)))
  			continue;
  		    
  		    /* Only advertise RIP if learned on our IDB */
  		    
  		    if (!nov_same_idb(idb, cur_ndb->path_list))
  			continue;
  		}
		/*
		 * Only add XROUTE if not subsumed by a summary:
		 */
		if (ipx_nlsp_check_summary(cur_ndb, pdb->ipx_pdb)) {
		    if (debug || NOVNLSP_DEBUG(cur_ndb->dstnet, pdb->ipx_pdb)) {
			buginf("\n%s-SPF:  Adding Route %x %x to LSP XROUTE list "
			       "for idb %s", pdb->errmsg_name, cur_ndb->dstnet, 
			       cur_ndb->rt_mask, idb->short_namestring);
		    }
		    /*
		     * If first item in this bucket, create the
		     * fragment and the TLV (exception is LSP 0):
		     */
		    if (ndb_item_count == 0) {
			if (lsp_index != 0) {
			    if (!isis_create_specific_lsp(context, lsp_index))
				return;
			}
			if (!isis_new_item(context, ISIS_NLSP_EXT_ROUTES,
					   0, NULL, NULL, TRUE))
			    return;
		    }
		    
		    entry = isis_add_item_entry(context,
						ISIS_NLSP_EXT_ROUTES_LEN,
						FALSE, NULL);
		    if (!entry)
			return;			/* Deep trouble! */
		    
		    ndb_item_count++;
		    
		    /*
		     * Allow this LSP to be suppressed if it is identical
		     * to an active LSP. This determination will be made
		     * later by isis_lsp_avl_insert()
		     */
		    context->lsp->lsp_suppress = TRUE; /* %%% */
		    
		    if (cur_ndb->flags & NET_IGRP) {
			if (cur_ndb->flags & NET_DEXTERIOR)
			    entry->hops = cur_ndb->ext_hop;
			else 
			    entry->hops = cur_ndb->ext_hop + 1;
			PUTSHORT(&entry->ticks, 
				 (cur_ndb->ext_delay == NOVELL_INFDELAY ?
				  NOVELL_INFDELAY : (cur_ndb->ext_delay + 
				 NOVELL_IDB_TICKS(IPX_PRIMARY_NIDB(idb)))));   
		    } else {
			entry->hops = cur_ndb->hopcount;
			PUTSHORT(&entry->ticks, cur_ndb->delay);
		    }
		    PUTLONG(&entry->net_number, cur_ndb->dstnet);
		}
	    }
	    /*
  	     * Move on to next table index. Possibly tie off the current
  	     * LSP and move on to the next bucket.
  	     */
  	    table_index++;
 	    
  	    if ((table_index % (NOV_NETHASHLEN/pdb->lsp_rt_frags) == 0) ||
  		(table_index >= NOV_NETHASHLEN)) {
  		
  		if ((lsp_index == 0) || (ndb_item_count > 0)) {
  		    isis_item_done(context, NULL);
  		    isis_lsp_done(context);
  		}
  		/*
  		 * If we have generated a NULL LSP, purge
  		 * any old LSP that is still active.
  		 */
  		if ((lsp_index != 0) && (ndb_item_count == 0)) {
  		    context->lspid.lsp_number = lsp_index;
  		    old_lsp = isis_find_L1_lsp(pdb, &context->lspid);
  		    if (old_lsp && old_lsp->lsp_active) {
  			isis_purge_lsp(pdb, old_lsp, context->level,
  				       pdb->lsp_lifetime);
 			
  		    }
  		}
 		
  		lsp_index++;
 		
  		if (lsp_index <= context->lspid.lsp_number) {
  		    lsp_index = context->lspid.lsp_number + 1;
  		}
 		
  		if ((table_index >= NOV_NETHASHLEN) || (lsp_index == 0)) {
  		    break;
  		}
  		ndb_item_count = 0;
  	    }
  	}
    }
    
    /* Add services info. */
    
    if (idb) {				/* pseudonode */
 	
  	table_index = 0;
  	svc_item_count = 0;
  	cur_svc = NULL;
 	
  	/* Loop over the entire IPX service table. */
 	
  	for (;;) {
  	    for (;;) {
  		cur_svc = nov_get_each_service_per_index(cur_svc,
  							 &table_index,
  							 TRUE);
  		if (cur_svc == NULL) break;
 		
  		/* Don't report poisoned services */
  		if (cur_svc->server.nethops >= novell_infinity)
  		    continue;
		
 		/*
 		 * Do report all other services, with the exception
 		 * of those static services which are not reachable.
 		 */
		
 		if (cur_svc->flags & NET_STATIC) {
 		    if (!novrip_find_rt(cur_svc->server.dnet,
					NOV_NETMASK_ALL,
 					IPX_ROUTE_MATCH_FUZZ)) {
 			continue;
 		    }
 		}
		/* Don't pick up services learned via this NLSP instance */
		if (!(INDEX2MASK(cur_svc->novpdbindex) & pdb->ipx_pdb->distflag) ||
		    (pdb->ipx_pdb->index == cur_svc->novpdbindex))
		    continue;
		
		if (!ipx_distribute_sap_list_check(cur_svc, pdb->ipx_pdb, 
						   IPX_PRIMARY_NIDB(idb)))
		    continue;
  		/*
  		 * For real interfaces, pick up only SAP services for our
  		 * interface.  For the internal net interface, pick up only 
  		 * statics.  Ignore infinitely distant services.
  		 */
  		if (nidb->internal_network) {
  		    if (!(cur_svc->flags & NET_STATIC)) 
  			continue;
  		} else {
  		    if (!(cur_svc->flags & (NET_RIP | NET_IGRP | NET_NLSP)))
  			continue;
  		    if (pdb->ipx_pdb->index == cur_svc->novpdbindex)
  			continue;
  		    if (!nov_srdb_same_idb(idb, cur_svc->paths))
  			continue;
  		}
  		
  		name_length = min(strlen(cur_svc->server.server_name),
  				  ISIS_NLSP_SVC_MAXLEN);
  		/*
  		 * If first item in this bucket, create the fragment:
  		 */
  		if (svc_item_count == 0) {
  		    if (!isis_create_specific_lsp(context, lsp_index))
  			return;
  		}
  		item = isis_new_item(context, ISIS_NLSP_SVCS_INFO, 
  				     ISIS_NLSP_SVCS_INFO_LEN +
  				     name_length, 
  				     (void **) &sentry, NULL, TRUE);
  		if (item == NULL)
  		    return;
		
  		svc_item_count++;
		
  		/*
  		 * Allow this LSP to be suppressed if it is identical
  		 * to an active LSP. This determination will be made
  		 * later by isis_lsp_avl_insert()
  		 */
  		context->lsp->lsp_suppress = TRUE; /* %%% */
		
		sentry->hops = cur_svc->server.nethops;
		PUTLONG(&sentry->net_number, GETLONG(&cur_svc->server.dnet));
		bcopy(cur_svc->server.dhost, sentry->node_number, IEEEBYTES);
		PUTSHORT(&sentry->socket, GETSHORT(&cur_svc->server.dsock));
		PUTSHORT(&sentry->type, GETSHORT(&cur_svc->server.server_type));
		bcopy(cur_svc->server.server_name, sentry->name, name_length);
		ADVANCE_ITEM(item, TRUE);
		isis_item_done(context, item);
  	    }
  	    /*
  	     * Move on to next table index. Possibly tie off the current
  	     * LSP and move on to the next bucket. If nothing is in this
  	     * fragment, free it and move on to next.
  	     */
  	    table_index++;
 	    
  	    if ((table_index % (NOV_NETHASHLEN/pdb->lsp_svc_frags) == 0) ||
  		(table_index >= NOV_NETHASHLEN)) {
  		
  		if (svc_item_count > 0) {
  		    isis_lsp_done(context);
  		} else {
  		    /*
  		     * If we have generated a NULL LSP, purge
  		     * any old LSP that is still active.
  		     */
  		    context->lspid.lsp_number = lsp_index;
  		    old_lsp = isis_find_L1_lsp(pdb, &context->lspid);
  		    if (old_lsp && old_lsp->lsp_active) {
  			isis_purge_lsp(pdb, old_lsp, context->level,
  				       pdb->lsp_lifetime);
  		    }
  		}
 		
  		lsp_index++;
 		
  		if (lsp_index <= context->lspid.lsp_number) {
  		    lsp_index = context->lspid.lsp_number + 1;
  		}
 		
  		if ((table_index >= NOV_NETHASHLEN) || (lsp_index == 0)) {
  		    break;
  		}
  		
  		svc_item_count = 0;
  	    }
  	}
    }

    /* Add Sroutes. */

    /*
     * Init table index to rewalk the entire route table. Get the
     * next LSP index ready just in case we need to build an LSP.
     * Note that the index will do double duty as a boolean. When
     * set it indicates that an LSP must be created if summaries
     * are to be generated. When cleared, it indicates that one
     * or more LSPs were created containing summaries.
     */
    table_index = 0;
    pdb->lsp_agg_frag = max((pdb->lsp_rt_frags + pdb->lsp_svc_frags),
			    context->lspid.lsp_number+1);

    if (idb) {				/* pseudonode */
	
	/*
	 * Walk the patricia tree importing summary
	 * routes from other areas, as appropriate:
	 */
	rn_walktree_blocking(nov_rnhead->rnh_treetop, 
			     ipx_nlsp_inject_sroute_patricia, context);

	if (!pdb->non_pnode_summary) {
	    /*
	     * Loop over the entire IPX route table, counting each
	     * route that matches an implicit (area address) or
	     * explicit (access list) summary.
	     */
	    for (;;) {	    
		cur_ndb = nov_get_each_route(cur_ndb, &table_index, TRUE);
		if (cur_ndb == NULL) break;
		ipx_nlsp_summary_depend(cur_ndb, pdb->ipx_pdb, idb);
	    }
	    /*
	     * Walk the patricia tree again and count each
	     * summary route that matches an implicit or explicit
	     * summary (triggering a more inclusive summary).
	     */
	    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
				 ipx_nlsp_summary_depend_patricia,
				 pdb->ipx_pdb, context);
	    /*
	     * Finally, generate aggregated routes in
	     * this pseudonode lsp as appropriate:
	     *
	     * Include the deny summary lists that have non-zero route count if
	     * manual summaries are configured, include the area addresses 
	     * that have the non-zero route count (automatic summaries)
	     */
	    isis_generate_summaries(context);
	}
    }

    if (!idb) {                         /* non-pseudonode */

	if (pdb->non_pnode_summary) {
	    
	    /* Loop over the entire IPX route table. */
	    for (;;) {	    
		cur_ndb = nov_get_each_route(cur_ndb, &table_index, TRUE);
		if (cur_ndb == NULL) break;
		ipx_nlsp_summary_depend(cur_ndb, pdb->ipx_pdb, NULL);
	    }
	    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
				 ipx_nlsp_summary_depend_patricia,
				 pdb->ipx_pdb, context);

	    isis_generate_summaries(context);
	}
    }
    /*
     * Tie off current summary LSP if anything generated:
     */
    if (pdb->lsp_agg_frag == 0) {
	isis_item_done(context, NULL);
    }
}

/*
 * isis_ipx_lsp_hash_enable_command
 *
 * OBJ(int,1) = route fragments
 * OBJ(int,2) = svc fragments
 */
void isis_ipx_lsp_hash_enable_command (parseinfo *csb, clns_pdbtype *pdb)
{
    
    if (csb->nvgen) {
 	if (pdb->lsp_hash_enable) {
 	    nv_write(TRUE, csb->nv_command);
 	    if ((pdb->lsp_rt_frags != ISIS_NLSP_RT_FRAGS_DEFAULT) ||
 		(pdb->lsp_svc_frags != ISIS_NLSP_SVC_FRAGS_DEFAULT)) {
 		nv_add(TRUE, " %d %d", pdb->lsp_rt_frags, pdb->lsp_svc_frags);
 	    }
 	}
  	return;
    }
    
    if ((pdb->lsp_hash_enable == csb->sense) &&
 	(pdb->lsp_rt_frags == GETOBJ(int,1)) &&
 	(pdb->lsp_svc_frags == GETOBJ(int,2))) {
  	return;
    }
    
    pdb->lsp_hash_enable = csb->sense;
    pdb->lsp_rt_frags  = GETOBJ(int,1);
    pdb->lsp_svc_frags = GETOBJ(int,2);
    
    /* Force regen of all LSPs */
    ipx_nlsp_clear_all(pdb);
}

/*
 * ipx_isis_compute_union_areas
 *
 * Join the computed set and the manual set to make a new set. 
 */
static void ipx_isis_compute_union_areas (clns_pdbtype *pdb)
{
    boolean found_it;
    int area, i;

    pdb->num_nlsp_union_areas = 0;   /* Start with 0 element */
    /* 
     * Add the manual set.
     */
    for (area = 0; area < pdb->num_nlsp_areas; area++) {
	pdb->nlsp_union_area[area] = pdb->nlsp_manual_area[area];
	pdb->nlsp_union_area_mask[area] = pdb->nlsp_manual_area_mask[area];
	pdb->num_nlsp_union_areas++;
    }
    /* 
     * Add the computed set 
     */
    for (area = 0; area < pdb->num_nlsp_computed_areas; area++) {
	/* Try to find it. */
	found_it = FALSE;
	for (i = 0; i < pdb->num_nlsp_union_areas; i++) {
	    if (pdb->nlsp_union_area[i] == pdb->nlsp_computed_area[area] &&
		pdb->nlsp_union_area_mask[i] == 
		pdb->nlsp_computed_area_mask[area]) {
		found_it = TRUE;
		break;
	    }
	}
	/* Don't add a duplicated entry */
	if (found_it)
	    continue;
	pdb->nlsp_union_area[pdb->num_nlsp_union_areas] = 
	    pdb->nlsp_computed_area[area];
	pdb->nlsp_union_area_mask[pdb->num_nlsp_union_areas] = 
	    pdb->nlsp_computed_area_mask[area];
	pdb->num_nlsp_union_areas++;
    }
}

/*
 * ipx_isis_union_areas
 *
 * Add a learned area to the set of active areas.
 *
 */
void ipx_isis_union_areas (clns_pdbtype *pdb, long area, long area_mask)
{

    long  largest, largest_mask;
    int   i, index;
    boolean found_it;
    novpdbtype *dist;
    novndbtype *ndb;

    /*
     * Area already in list of acceptable areas.
     */
    for (i = 0; i < pdb->num_nlsp_computed_areas; i++) {
	if (area == pdb->nlsp_computed_area[i] && 
	    area_mask == pdb->nlsp_computed_area_mask[i]) 
	    return;
    }

    index = pdb->num_nlsp_computed_areas;

    /*
     * Go do tie breaking. Largest area address gets dropped from 
     * list.  Complain bitterly if this happens.
     */
    if (pdb->num_nlsp_computed_areas == MAX_AREA_ADDRESSES) {
	largest = area;
	largest_mask = area_mask;
	index = -1;
	for (i = 0; i < pdb->num_active_areas; i++) {
	    if (largest < pdb->nlsp_computed_area[i] || 
		(largest == pdb->nlsp_computed_area[i] &&
		 largest_mask < pdb->nlsp_computed_area_mask[i])) {
		largest = pdb->nlsp_computed_area[i];
		largest_mask = pdb->nlsp_computed_area_mask[i];
		index = i;
	    }
	}

	/* The area address on this call is the largest, ignore it. */
	if (index == -1) return;

	/* Going to replace an entry. Keep number of entries the same. */
	pdb->num_nlsp_computed_areas--;
    }

    found_it = FALSE;
    for (i = 0; i < pdb->num_nlsp_areas; i++) {
	if (pdb->nlsp_manual_area[i] == area &&
	    pdb->nlsp_manual_area_mask[i] == area_mask) {
	    found_it = TRUE;
	    break;
	}
    }

    /*
     * If we're replacinng a learned area address, remove it from our
     * forwarding table.
     */
    if (!found_it) {
	ndb = novrip_find_rt(pdb->nlsp_computed_area[index], 
			     pdb->nlsp_computed_area_mask[index], 
			     IPX_ROUTE_MATCH_EXACT);
	if (ndb && ndb->path_list && ndb->path_list->nidb == nullnidb &&
	    ndb->novpdbindex == pdb->ipx_pdb->index)
	    novrip_del_rt(pdb->nlsp_computed_area[index], 
			  pdb->nlsp_computed_area_mask[index], 
			  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
    }

    pdb->nlsp_computed_area[index] = area;
    pdb->nlsp_computed_area_mask[index] = area_mask;
    pdb->num_nlsp_computed_areas++;
    /*
     * Our area address set change. Notify other NLSP instances if they are
     * doing automatic summary.
     */
    for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	if (!dist->osi_pdb || dist->osi_pdb == pdb ||
	    dist->proctype != NET_NLSP)
	    continue;
	if (!dist->sumlist[pdb->ipx_pdb->index]) {
	    ipx_mark_dr_info_change(dist, NULL);
	    isis_mark_L1_changed(dist->osi_pdb);
	}
    }
    /*
     * Recompute the union of computed set and manual set.
     */
    ipx_isis_compute_union_areas(pdb);
}

/*
 * isis_ipx_enable_summary_command
 *
 * Set the "enable summary" flag.
 */
void isis_ipx_enable_summary_command (parseinfo *csb, clns_pdbtype *pdb)
{
    int i;
    novndbtype *ndb;
    novpdbtype *dist;

    if (csb->nvgen) {
	nv_write(pdb->enable_summary, csb->nv_command);
	return;
    }
    if (pdb->enable_summary == csb->sense)
	return;
    pdb->enable_summary = csb->sense;
    isis_mark_L1_changed(pdb);
    if (csb->sense == FALSE) {
	/* Turning off auto summary. Remove all area address routes */
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	    if (!dist->osi_pdb || dist->proctype != NET_NLSP)
		continue;
	    ipx_mark_dr_info_change(dist, NULL);
	    isis_mark_L1_changed(dist->osi_pdb);
	    for (i = 0; i < dist->osi_pdb->num_nlsp_areas; i++) {
		if (dist->osi_pdb->nlsp_manual_area_mask[i] == 0)
		    continue;
		ndb = novrip_find_rt(dist->osi_pdb->nlsp_manual_area[i], 
				     dist->osi_pdb->nlsp_manual_area_mask[i],
				     IPX_ROUTE_MATCH_EXACT);
		if (ndb && ndb->path_list && 
		    ndb->path_list->nidb == nullnidb &&
		    ndb->novpdbindex == pdb->ipx_pdb->index)
		    novrip_del_rt(dist->osi_pdb->nlsp_manual_area[i], 
				  dist->osi_pdb->nlsp_manual_area_mask[i], 
				  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	    for (i = 0; i < dist->osi_pdb->num_active_areas; i++) {
		if (dist->osi_pdb->nlsp_computed_area_mask[i] == 0)
		    continue;
		ndb = novrip_find_rt(dist->osi_pdb->nlsp_computed_area[i], 
				     dist->osi_pdb->nlsp_computed_area_mask[i],
				     IPX_ROUTE_MATCH_EXACT);
		if (ndb && ndb->path_list && 
		    ndb->path_list->nidb == nullnidb &&
		    ndb->novpdbindex == pdb->ipx_pdb->index)
		    novrip_del_rt(dist->osi_pdb->nlsp_computed_area[i], 
				  dist->osi_pdb->nlsp_computed_area_mask[i], 
				  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	}
	/* 
	 * print a friendly message if there is any redistribute command 
	 * with access-lists.
	 */
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	    if (!dist->osi_pdb || dist->proctype != NET_NLSP)
		continue;
	    if (pdb->ipx_pdb->sumlist[dist->index] != 0) {
		printf("\n%%redistribute commands for doing route aggregation are now deactivated");
		break;
	    }
	}
    } else {
	/* Rebuild all LSP's */
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	    if (!dist->osi_pdb || dist->proctype != NET_NLSP)
		continue;
	    ipx_mark_dr_info_change(dist, NULL);
	    isis_mark_L1_changed(dist->osi_pdb);
	}
    }
}

/*
 * isis_ipx_enable_lsp_summary_command
 *
 * Set the "enable summary" flag.
 */
void isis_ipx_enable_lsp_summary_command (parseinfo *csb, clns_pdbtype *pdb)
{
    boolean previous_sense;

    if (csb->nvgen) {
	nv_write(pdb->non_pnode_summary, csb->nv_command);
	return;
    }

    if (pdb->non_pnode_summary == csb->sense) {
	return;
    }
    previous_sense = pdb->non_pnode_summary;
    pdb->non_pnode_summary = csb->sense;
    csb->sense = FALSE;
    isis_ipx_enable_summary_command(csb, pdb);
    csb->sense = TRUE;
    isis_ipx_enable_summary_command(csb, pdb);
    csb->sense = pdb->non_pnode_summary;
}

/*
 * isis_ipx_area_cmd
 *
 * Handle the IPX and NLSP area configuration commands
 *
 * [no] ipx area-address <net> <mask> (router ISIS subcommand)
 * [no] area-address <net> <mask> (ipx router NLSP subcommand)
 *
 * pdb          clns pdb
 * interactive  did we come here from the parser or elsewhere
 * enabling     csb->sense  true if enabling
 * nvgen        csb->nvgen  true to write NVM commands
 * cmd          the command name csb->command
 * narea        OBJ(int,1) = network
 * nmask        OBJ(int,2) = mask
 */
void isis_ipx_area_cmd (clns_pdbtype *pdb, boolean interactive, boolean sense,
			boolean nvgen, char *cmd, int narea, int nmask)
{
    ulong area_num;
    boolean found_it;
    novndbtype *ndb;
    novpdbtype *dist;

    /* NVGENs first. */

    if (nvgen) {
	for (area_num = 0; area_num < pdb->num_nlsp_areas; area_num++) {
	    nv_write(TRUE, "%s %x %x", cmd,
		     pdb->nlsp_manual_area[area_num],
		     pdb->nlsp_manual_area_mask[area_num]);
	}
	return;
    }

    /* Complain if the command is malformed. */

    if (!ipx_validate_contiguous_mask(nmask)) {
	if (narea || nmask) {
	  if (interactive)
	    printf("\n%%%s: Non-contiguous mask %8x is not supported", 
		   pdb->errmsg_name, nmask);
	    return;
	}
    }

    if ((narea & nmask) != narea) {
      if (interactive)
	printf("\n%%%s: Mask inconsistent with network", pdb->errmsg_name);
    } else {

	/* Try to find it. */

	found_it = FALSE;
	for (area_num = 0; area_num < pdb->num_nlsp_areas; area_num++) {
	    if (pdb->nlsp_manual_area[area_num] == narea &&
		pdb->nlsp_manual_area_mask[area_num] == nmask) {
		found_it = TRUE;
		break;
	    }
	}

	/* Check for the "no" form. */

	if (!sense) {		/* "no" */
	    if (nmask != 0) {
		ndb = novrip_find_rt(narea, nmask,
				     IPX_ROUTE_MATCH_EXACT);
		if (ndb && ndb->path_list && ndb->path_list->nidb == nullnidb
		    && ndb->novpdbindex == pdb->ipx_pdb->index)
		    novrip_del_rt(narea, nmask,
				  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	    if (!found_it) {
	      if (interactive)
		printf("\n%%%s:  Area not found", pdb->errmsg_name);
	    } else {

		/* Found it.  Shuffle everything else up. */

		pdb->num_nlsp_areas--;
		while (area_num < pdb->num_nlsp_areas) {
		    pdb->nlsp_manual_area[area_num] =
			pdb->nlsp_manual_area[area_num + 1];
		    pdb->nlsp_manual_area_mask[area_num] =
			pdb->nlsp_manual_area_mask[area_num + 1];
		    area_num++;
		}

		/* Do the same for the computed set */
		found_it = FALSE;
		for (area_num = 0; area_num < pdb->num_nlsp_computed_areas; 
		     area_num++) {
		    if (pdb->nlsp_computed_area[area_num] == narea &&
			pdb->nlsp_computed_area_mask[area_num] == 
			nmask) {
			found_it = TRUE;
			break;
		    }
		}
		if (found_it) {
		    pdb->num_nlsp_computed_areas--;
		    while (area_num < pdb->num_nlsp_computed_areas) {
			pdb->nlsp_computed_area[area_num] =
			    pdb->nlsp_computed_area[area_num + 1];
			pdb->nlsp_computed_area_mask[area_num] =
			    pdb->nlsp_computed_area_mask[area_num + 1];
			area_num++;
		    }
		}
		/* Rebuild the union set */
		ipx_isis_compute_union_areas(pdb);
		if (pdb->proc_type == PROC_TYPE_NLSP &&
		    pdb->num_nlsp_areas == 0) {
		    clns_set_pdb_running(pdb, FALSE);
		}
	    }
	} else {
	    if (!found_it) {		/* Not already there */
		pdb->isis_mode |= ISIS_IPX_MODE;
		
		/* Add the new one if there's room. */
		
		if (pdb->num_nlsp_areas < MAX_AREA_ADDRESSES) {
		    pdb->nlsp_manual_area[pdb->num_nlsp_areas] = narea;
		    pdb->nlsp_manual_area_mask[pdb->num_nlsp_areas] = 
			nmask;
		    pdb->num_nlsp_areas++;
		} else {
		  if (interactive)
		    printf("\n%%%s:  Too many area addresses",
			   pdb->errmsg_name);
		}
		ipx_isis_union_areas(pdb, narea, nmask);
 		if (pdb->proc_type == PROC_TYPE_NLSP) {
 		    clns_set_pdb_running(pdb, TRUE); /* Fire up NLSP... */
 		}
	    }
	}	
	isis_mark_L1_changed(pdb);
	/*
	 * Our area address set change. Notify other NLSP instances if they are
	 * doing automatic summary.
	 */
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	    if (!dist->osi_pdb || dist->osi_pdb == pdb ||
		dist->proctype != NET_NLSP)
		continue;
	    if (!dist->sumlist[pdb->ipx_pdb->index]) {
		ipx_mark_dr_info_change(dist, NULL);
		isis_mark_L1_changed(dist->osi_pdb);
	    }
	}
    }
}

/*
 * ipx_nlsp_mark_all_L1_dr_changed
 *
 * Walk all the nidbs and mark L1 dr change for each.
 */
void ipx_nlsp_mark_all_L1_dr_changed (clns_pdbtype *pdb)
{
    isisidbtype *isisidb;
    nidbtype *nidb;

    if (pdb && clns_pdb_running(pdb)) {
	FOR_ALL_NIDBS(nidb) {
	    isisidb = get_isis_idb(pdb, nidb->idb);
	    if (isisidb)
   	       isis_mark_L1_dr_changed(pdb, isisidb);
	}
    }
}

/*
 * ipx_mark_dr_info_change
 *
 * Mark that the info generated in the DR LSP for this interface has changed.
 * Null idb pointer means to mark dr change for all.
 */
void ipx_mark_dr_info_change (novpdbtype *pdb, idbtype *idb)
{
    isisidbtype *isisidb;
    clns_pdbtype *osi_pdb;
    nidbtype *nidb;

    if (pdb) {
	osi_pdb = pdb->osi_pdb;
	if (osi_pdb && clns_pdb_running(osi_pdb)) {
	    ipx_nlsp_schedule_redist(pdb->osi_pdb);
	    if (idb) {
		isisidb = get_isis_idb(osi_pdb, idb);
		if (isisidb)
		    isisidb->isis_ipx_xroutes_changed = TRUE;
	    } else {
		FOR_ALL_NIDBS(nidb) {
		    isisidb = get_isis_idb(osi_pdb, nidb->idb);
		    if (isisidb)
			isisidb->isis_ipx_xroutes_changed = TRUE;
		}
	    }
	}
    }
}

/*
 * ipx_mark_non_dr_info_change
 *
 * Mark that the info generated for the non-DR LSP for this system has changed.
 * We ignore NULL pointers.
 */
void ipx_mark_non_dr_info_change (novpdbtype *pdb)
{
    if (pdb && pdb->osi_pdb && clns_pdb_running(pdb->osi_pdb)) {
	ipx_nlsp_schedule_redist(pdb->osi_pdb);
    }
}

/*
 * Table mapping bandwidth to default link cost, from spec (p. 5-12)
 */
struct bw_cost {
    ulong bandwidth;			/* Bandwidth in Kbps */
    ulong cost;				/* Cost */
};

static const struct bw_cost nlsp_costs[] = {
    {16, 61},
    {48, 55},
    {128, 45},
    {256, 40},
    {512, 35},
    {1000, 30},
    {2000, 27},
    {4000, 26},
    {8000, 25},
    {10000, 23},
    {16000, 20},
    {32000, 19},
    {64000, 15},
    {128000, 14},
    {512000, 9},
    {4000000, 6},
    {0xffffffff, 3}};

#define NUM_NLSP_COSTS (sizeof(nlsp_costs) / sizeof(struct bw_cost))

/*
 * ipx_nlsp_set_cost
 *
 * Set the cost for this interface, given the current bandwidth
 * setting and possible explicit cost setting.
 *
 * We reissue the non-DR LSP to reflect the change.
 */
void ipx_nlsp_set_cost (nidbtype *nidb, clns_pdbtype *pdb)
{
    idbtype *idb;
    isisidbtype *isisidb;
    int i;
    ulong bandwidth;

    if (!nidb)
	return;
    idb = nidb->idb;
    if (!idb)
	return;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;
    /*
     * Set pacing interval if configured explicitly:
     */
    if (isisidb->cfg_pacing_interval) {
	isisidb->pacing_interval = isisidb->cfg_pacing_interval;
    }
    /* If explicitly set, bail out. */
    
    if (isisidb->nlsp_explicit_cost)
	return;
    
    /*
     * It's being defaulted.  Pick it up from the table. While we're
     * at it, compute appropriate pacing interval, unless configured.
     */
	    
    bandwidth = ipx_nlsp_link_bandwidth(nidb);
    if (bandwidth && !isisidb->cfg_pacing_interval) {
	isisidb->pacing_interval = max(min(10000000/bandwidth, ONESEC),
				       NLSP_MINIMUM_PACING_INTERVAL);
    }
    bandwidth /= 1000; /* Convert bps to Kbps */
    
    for (i=0; i<NUM_NLSP_COSTS; i++) {
	if (bandwidth < nlsp_costs[i].bandwidth) {
	    isisidb->isis_L1_metric = nlsp_costs[i].cost;
	    break;
	}
    }
    /* Notify the authorities. */
    
    ipx_mark_non_dr_info_change(pdb->ipx_pdb);
}

/*
 * ipx_nlsp_set_media
 *
 * Set the media type in the NIDB given the interface type, encapsulation,
 * etc.
 */
static void ipx_nlsp_set_media (nidbtype *nidb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    ushort media;

    if (nidb == NULL)
	return;
    idb = nidb->idb;
    if (idb == NULL)
	return;
    hwidb = idb->hwptr;
    if (hwidb == NULL)
	return;

    /* Start easy--set generic LAN/WAN. */

    if (is_p2p(idb))
	media = NLSP_TYPE_GENERIC_WAN;
    else
	media = NLSP_TYPE_GENERIC_LAN;

    /* Now start picking them apart. */

    if (idb_is_ether_like(idb)) {
	switch (nidb->enctype) {
	  case ET_NOVELL_ETHER:
	    media = NLSP_TYPE_802_3_RAW;
	    break;
	  case ET_ARPA:
	    media = NLSP_TYPE_ETHERNET;
	    break;
	  case ET_SAP:
	    media = NLSP_TYPE_802_3_LLC;
	    break;
	  case ET_SNAP:
	    media = NLSP_TYPE_802_3_SNAP;
	    break;
	}
    } else if (is_tokenring(hwidb)) {
	switch (nidb->enctype) {
	  case ET_SAP:
	    media = NLSP_TYPE_802_5_LLC;
	    break;
	  case ET_SNAP:
	    media = NLSP_TYPE_802_5_SNAP;
	    break;
	}
    } else if (is_fddi(hwidb)) {
	switch (nidb->enctype) {
	  case ET_SAP:
	    media = NLSP_TYPE_FDDI_LLC;
	    break;
	  case ET_SNAP:
	    media = NLSP_TYPE_FDDI_SNAP;
	    break;
	  case ET_NOVELL_FDDI:
	    media = NLSP_TYPE_GENERIC_LAN; /* %%% Not defined by Novell %%% */
	    break;
	}
    } else if (is_tunnel(hwidb)) {
	media = NLSP_TYPE_IP_TUNNEL;
    } else if (is_isdn(hwidb)) {
	media = NLSP_TYPE_ISDN;
    } else if (is_serial(hwidb)) {
	switch (hwidb->enctype) {
	  case ET_PPP:
	    media = NLSP_TYPE_PPP;
	    break;
	  case ET_X25:
	    media = NLSP_TYPE_X_25;
	    break;
	  case ET_FRAME_RELAY:
	    media = NLSP_TYPE_FRAME_RELAY;
	    break;
	  default:
	    break;
	}
    }
    nidb->nov_nlsp_media = media;
}
		
/*
 * ipx_nlsp_interface_init
 *
 * Initialize NLSP for this interface.  Called from several places, depending
 * on command ordering and such.
 *
 * "sense" is TRUE if we're turning the interface on, and FALSE if we're
 * turning the interface off.
 *
 * "active" is TRUE if we're actually running NLSP on the interface, or
 * FALSE if it's effectively passive (necessary to generate pseudonode
 * LSPs for RIP-only interfaces).
 *
 * These two booleans are combined to give us three possible states:
 *
 *  !sense == NLSP should forget about this i/f entirely
 *  sense && !active == NLSP should generate a pseudonode for this i/f
 *  sense && active == NLSP should run full-bore on this i/f
 */
void ipx_nlsp_interface_init (nidbtype *nidb, novpdbtype *npdb, boolean sense,
			      boolean active)
{
    clns_pdbtype *pdb;
    idbtype *idb;
    isisidbtype *isisidb;
    boolean was_active;

    pdb = npdb->osi_pdb;
    if (pdb == NULL)
	return;				/* Shouldn't happen */

    idb = nidb->idb;
    if (idb == NULL)
	return;				/* Shouldn't happen either. */

    /*
     * If other NLSP instance is enable on this interface, we don't want
     * to fire up the hello stuff!
     */
    if (nidb != ipx_internal_network_nidb && 
	strcasecmp(nidb->pname, pdb->pname))
	active = FALSE;

    was_active = FALSE;
    isisidb = get_isis_idb(pdb, idb);
    if (isisidb) {
	was_active = isisidb->isis_running;
    }

    if (sense) {			/* Get some storage if not there */
	isisidb = isis_alloc_isisidb(idb, PROC_TYPE_NLSP, pdb->index);
	if (!isisidb)
	    return;
    } else {
	active = FALSE;			/* Don't be fooled by bad callers */
    }

    /* If we are turning off NLSP on the interface, clean up. */

    if (itf_in_pdb(idb, pdb)) {
	if (was_active && !active) {
	    delete_interface_adjacencies(pdb->adjacency_db, idb);
	    isis_reset_circuit_id(pdb, idb);
	    isis_reset_idb_params(pdb, idb, FALSE);
	}

	if (!sense) {		/* Remove all traces */
	    isis_release_idb_number(pdb, isisidb);
	    clns_remove_idb_from_queue(pdb, idb);
	    if (isisidb) {
		free(isisidb);
		idb->isis_idb[isis_pdbindex_to_procindex(pdb->index)] = NULL;
	    }
	}
    }

    /* Enable stuff as necessary. */

    if (sense) {

	/* Get the interface state. */

	isisidb->isis_interface_up = (interface_up(idb) &&
				      NOVELL_NIDB_ISUP(nidb));

	/* Set up the media type. */

	ipx_nlsp_set_media(nidb);
	ipx_nlsp_set_cost(nidb, pdb);

	/* Find the interface on the PDB queue.  If not there, add it. */

	if (!itf_in_pdb(idb, pdb)) {	/* It's not there */

	    /* Not there. add it. */
	    
	    if (!clns_add_if_to_pdbqueue(idb, pdb)) {
		printf("\nNLSP: could not add interface; no memory");
		return;
	    }
	}

	/* Do more stuff if NLSP will actually be running on this i/f. */

	if (active) {
	    if (!was_active) {
		
		/* Fire up the hello stuff. */
		
		pdb->net[0] = 1;		/* %%% Heinous! */
		isis_start_interface(pdb, idb);	/* %%% Need NLSP version */
	    }

	} else {			/* Inactive */

	    isis_reset_circuit_id(pdb, idb);
	}
    }

    isis_mark_L1_changed(pdb);
    if (isisidb)
	isis_mark_L1_dr_changed(pdb, isisidb);
}

/*
 * ipx_router_isis_command
 *
 * Process the "ipx router isis" command.
 */
#ifdef NOBODY_CALLS_ME
void ipx_router_isis_command (parseinfo *csb)
{
#if 0
    nidbtype *nidb = csb->interface->primary_nidb;
    clns_pdbtype *pdb;
    char *name;

    if (csb->nvgen) {
	nv_write(nidb && nidb->nov_lsp_enabled && nidb->nov_lsp_isis, "%s",
		 csb->nv_command/*, pdb->pname*/);
	return;
    }

    name = GETOBJ(string,1);
    if (strlen(name) == 0)
 	name = NULL;
    pdb = clns_find_router_name(name);
    if (pdb == NULL)
	return;

    /* Bitch if IPX isn't running on this interface. */

    if (!nidb) {
	printf("\n%%%s:  Must specify an IPX network first", pdb->errmsg_name);
	return;
    }
    /*
     * If we are enabling IPX ISIS on this interface, set compatibility
     * mode parameters to reflect the most recently configured state.
     * If disabling, reset state to force RIP/SAP to simple ON.
     */
    nidb->nov_lsp_enabled = csb->sense;
    nidb->nov_lsp_isis = csb->sense;

    if (nidb->nov_lsp_enabled) {
	nidb->nov_lsp_rip_auto = nidb->nov_lsp_cfg_rip_auto;
	nidb->nov_lsp_sap_auto = nidb->nov_lsp_cfg_sap_auto;
	nidb->nov_lsp_rip_on = nidb->nov_lsp_cfg_rip_on;
	nidb->nov_lsp_sap_on = nidb->nov_lsp_cfg_sap_on;
	nidb->nov_lsp_rip_timestamp = 0;
	nidb->nov_lsp_sap_timestamp = 0;
    } else {
	nidb->nov_lsp_rip_auto = FALSE;
	nidb->nov_lsp_sap_auto = FALSE;
	nidb->nov_lsp_rip_on = TRUE;
	nidb->nov_lsp_sap_on = TRUE;
	nidb->nov_lsp_rip_timestamp = 0;
	nidb->nov_lsp_sap_timestamp = 0;
    }

    if (csb->sense) {

	/*
	 * %%% Need to verify placement of this %%%
	 * Check if IPX pdb is available:
	 */
	if (!isis_ipx_get_pdb(pdb))
	    return;

	pdb->isis_mode |= ISIS_IPX_MODE;
	if (clns_pdb_running(pdb)) {
	    isis_mark_L1_changed(pdb);
	    isis_mark_L1_dr_changed(pdb, isisidb);
	}
    }
#endif
}
#endif

/*
 * ipx_get_corresponding_idb
 *
 * Get the IDB corresponding to a route or service (given its flag bits and
 * an NIDB pointer).
 *
 * If the STATIC flag is set, we return the NIDB of the internal net;
 * otherwise, we return the NIDB from the route.
 *
 * Returns NULL if there is no internal net defined.
 */
static idbtype *ipx_get_corresponding_idb (nidbtype *route_nidb, ulong flags)
{
    nidbtype *nidb;

    if (flags & NET_STATIC) {
	nidb = ipx_internal_network_nidb;
    } else {
	nidb = route_nidb;
    }

    if (nidb && nidb->idb)
	return(nidb->idb);
    else
	return(NULL);
}

/*
 * isis_ipx_redist_callback
 *
 * Handle redistribution from NLSP/RIP/EIGRP into NLSP.
 *
 */
void isis_ipx_redist_callback (novpdbtype *pdb,
			       novndbtype *ndb,
			       int event)
{
    boolean accepted = FALSE;
    clns_pdbtype *cpdb;
    boolean redist_debug;
    nidbtype *route_nidb;

    /*
     * No joy if not running:
     */

    cpdb = pdb->osi_pdb;
    if (!cpdb || !clns_pdb_running(cpdb))
	return;

    redist_debug = (novellredist_debug &&
		    (pdb->debug_set ? (pdb->debug_net == ndb->dstnet): TRUE));

    /*
     * Reject route if not RIP or STATIC or EIGRP or NLSP:
     */
    if (ndb->flags & (NET_RIP | NET_IGRP | NET_STATIC | NET_NLSP | 
		      NET_XROUTE)) {
	accepted = TRUE;
    }

    if (!accepted) {
	if (redist_debug)
	    buginf("\n%s: reject net %x for redistribution",
		   cpdb->errmsg_name, ndb->dstnet);
    } else {
	/*
	 * Additional screening:
	 */
	if (novellredist_debug)
	    buginf("\n%s: redistribute %s net %x into %s",
		   cpdb->errmsg_name,
		   novdbg_event2string(event),
		   ndb->dstnet,
		   pdb->name);
	/*
	 * Need to determine if this is best RIP-only route currently known.
	 */
	if (ndb->path_list) {
	    route_nidb = ndb->path_list->nidb;
	    ipx_mark_dr_info_change(pdb, 
				    ipx_get_corresponding_idb(route_nidb, 
							      ndb->flags));
	    isis_mark_L1_changed(pdb->osi_pdb);
	} 
    }
}

/*
 * isis_ipx_rdb_changed_callback
 * Notice an RDB has changed, cause DR LSPs to be generated for all affected
 * interfaces
 */
void isis_ipx_rdb_changed_callback (novpdbtype *pdb,
				    novndbtype *ndb,
				    novrdbtype *rdb,
				    enum IPX_ROUTE_EVENT event,
				    ulong flags)
{
    novrdbtype *loop_rdb;
    nidbtype *route_nidb;
    
    /* Ignore our own routes. */

    if (flags & pdb->proctype)
        return;

    /*
     * Advertise item out all interfaces listed if event is UP. If event
     * is DOWN, mark ALL LSP's as needing info.
     */

    /* If already known out this interface, or still known, don't bother */
    /* Or... if event is change, just accept */

    if (event != IPX_ROUTE_EVENT_CHANGE) {
	for (loop_rdb = ndb->path_list; loop_rdb != NULL;
	     loop_rdb = loop_rdb->next) {
	    if ((loop_rdb != rdb) && (loop_rdb->nidb == rdb->nidb))
		return;
	}
    }
    route_nidb = rdb->nidb;
    if (route_nidb) {
	ipx_mark_dr_info_change(pdb, 
				ipx_get_corresponding_idb(route_nidb, flags));
    }
}

/*
 * isis_ipx_srdb_changed_callback
 * Notice an SRDB has changed, cause DR LSPs to be generated for all affected
 * interfaces
 */
void isis_ipx_srdb_changed_callback (novpdbtype *pdb,
				     novell_sapentrytype *sap,
				     novell_sapentry_rdb *srdb,
				     enum IPX_ROUTE_EVENT event,
				     ulong flags)
{
    novell_sapentry_rdb *loop_srdb;
    
    /* We don't care about NLSP RDBs changing. NLSP takes care of it */
    if (flags & pdb->proctype)
      return;
    /*
     * If the entry's srdb has a NULL nidb pointer, this is because
     * it is a static sap... deal with it here in the future, more
     * globally as dealing with 'flags & NET_STATIC'...
     */
    if (srdb->nidb == NULL)
      return;
    /*
     * Advertise item out all interfaces listed if event is UP. If event
     * is DOWN, mark ALL LSP's as needing info.
     */
    /* If already known out this interface, or still known, don't bother */
    /* Or... if event is change, just accept */
    if (event != IPX_ROUTE_EVENT_CHANGE) {
	for (loop_srdb = sap->paths; loop_srdb != NULL;
	     loop_srdb = loop_srdb->next) {
	    if ((loop_srdb != srdb) && (loop_srdb->nidb == srdb->nidb))
	      return;
	}
    }

    ipx_mark_dr_info_change(pdb, 
			    ipx_get_corresponding_idb(srdb->nidb, flags));
}

/*
 * isis_ipx_sap_redist_callback
 *
 * Handle redistribution from SAP into NLSP/ISIS.
 *
 */
void isis_ipx_sap_redist_callback (novpdbtype *pdb,
				   novell_sapentrytype *sap,
				   int event)
{
    clns_pdbtype *cpdb;
    nidbtype *nidb;
    boolean redist_debug = FALSE;

    /*
     * No joy if not running:
     */
    cpdb = pdb->osi_pdb;
    if (!cpdb || !clns_pdb_running(cpdb))
	return;

    redist_debug = (novellredist_debug &&
		    (pdb->debug_set ?
		     (pdb->debug_net == sap->server.dnet): TRUE));
    /*
     * Reject service if not vanilla SAP:
     */
    if (!sap->flags & (NET_RIP | NET_STATIC)) {
	if (redist_debug)
	    buginf("\n%s: reject non-SAP router service"
		   "%x:%s, net %x for redistribution",
		   cpdb->errmsg_name,
		   sap->server.server_type,
		   sap->server.server_name,
		   sap->server.dnet);

	return;
    }

    /*
     * Additional screening:
     */

    if (redist_debug)
	buginf("\n%s: redistribute %s service %x:%s, net %x"
	       "@%d hops into NLSP",
	       cpdb->errmsg_name,
	       novdbg_event2string(event),
	       sap->server.server_type,
	       sap->server.server_name,
	       sap->server.dnet,
	       sap->server.nethops);
    /*
     * Need to determine if this is best SAP service currently known.
     */
    if (sap->paths) {
	nidb = sap->paths->nidb;
	ipx_mark_dr_info_change(pdb, 
				ipx_get_corresponding_idb(nidb, sap->flags));
    }
}

/*
 * isis_ipx_xroute_callback
 *
 * Determine if incoming RIP route is best XROUTE in area. If so,
 * the caller will remove the current XROUTE and replace it with
 * a bone fide RIP route.
 *
 */
boolean isis_ipx_xroute_callback (novpdbtype *pdb,
				  novndbtype *ndb,
				  ushort ticks,
				  ushort hops)
{
    clns_pdbtype *cpdb;
    boolean debug;
    ulong ndb_ext_delay;
    ulong ndb_ext_hops;

    ndb_ext_delay =
	((ndb->path_list != NULL) ? ndb->ext_delay : NOVELL_INFDELAY);
    ndb_ext_hops =
	((ndb->path_list != NULL) ? ndb->ext_hop : novell_infinity);

    debug = (novell_verbose_debug &&
	     (((pdb->proctype == NET_NLSP) &&
	      NOVNLSP_DEBUG(ndb->dstnet, pdb))));
    /*
     * If not running, caller's route is always preferred:
     */
    cpdb = pdb->osi_pdb;
    if (!cpdb || !clns_pdb_running(cpdb)) {
	return(TRUE);
    }
    /*
     * Reject request not NLSP:
     */
    if (!(ndb->flags & NET_NLSP)) {
	if (debug)
	    buginf("\n%s: reject XROUTE callback on non-NLSP net %x",
		   cpdb->errmsg_name, ndb->dstnet);
	return(FALSE);
    }

    if (debug) {
	buginf("\n%s: Compare %s for net %x @ [Ext T%d/H%d] to"
	       " RIP route @ [T%d/H%d]",
	       cpdb->errmsg_name,
	       ((ndb->flags & NET_XROUTE) ? "XROUTE" : "NLSP route"),
	       ndb->dstnet, ndb_ext_delay, ndb_ext_hops, ticks, hops);
    }
    /*
     * If NLSP route (pure or XROUTE) is no longer valid
     * (but not yet purged) then always prefer RIP:
     */
    if (TIMER_RUNNING(ndb->holddownuntil) ||
	(ndb->path_metric == METRIC_INACCESSIBLE) ||
	(ndb->delay == NOVELL_INFDELAY) ||
	(ndb->hopcount >= novell_infinity)) {
	if (debug) buginf("(in holddown, prefer RIP)");
	return(TRUE);
    }
    /*
     * Reject request if not XROUTE (never replace pure NLSP):
     */
    if (!(ndb->flags & NET_XROUTE)) {
	if (debug)
	    buginf("\n%s: reject XROUTE callback on non-XROUTE net %x",
		   cpdb->errmsg_name, ndb->dstnet);
	return(FALSE);
    }
    /*
     * Need to determine if this RIP route is best (or tied for best)
     * entry route in the area:
     */
    if ((ticks < ndb_ext_delay) ||
	((ticks == ndb_ext_delay) &&
	 (hops <= ndb_ext_hops))) {
	    if (debug) buginf("(prefer RIP)");
	    return(TRUE);
	} else {
	    if (debug) buginf("(prefer XROUTE)");
	    return(FALSE);
	}
}

/*
 * isis_ipx_xsap_callback
 *
 * Determine if incoming SAP service is best service in area. If so,
 * the caller will remove the current service entry and replace it with
 * the new service entry.
 *
 *     sap  -> new (incoming) service entry
 *
 */
boolean isis_ipx_xsap_callback (novpdbtype *pdb,
				novell_sapentrytype *sap,
				ushort hops)
{
    clns_pdbtype *cpdb;
    boolean debug;

    if (!pdb) {
	return(TRUE);
    }

    debug = (novell_verbose_debug &&
	     (((pdb->proctype == NET_NLSP) &&
	      NOVNLSP_DEBUG(sap->server.dnet, pdb))));
    /*
     * If not running always prefer caller's service:
     */
    cpdb = pdb->osi_pdb;
    if (!cpdb || !clns_pdb_running(cpdb)) {
	return(TRUE);
    }
    /*
     * Additional screening TBS:
     */
    if (debug)
	buginf("\n%s: compare svc %x:%s, net %x"
	       " @ [H%d][Ext H%d] to SAP @ [H%d]",
	       cpdb->errmsg_name,
	       sap->server.server_type,
	       sap->server.server_name,
	       sap->server.dnet,
	       sap->server.nethops,
	       sap->ext_hop,
	       hops);
    /*
     * Need to determine if this SAP service is best (or tied for best)
     * entry service in the area:
     */

    if (sap->poisoned || (hops <= sap->ext_hop)) {
	if (debug) buginf("(prefer SAP)");
	return(TRUE);
    } else {
	if (debug) buginf("(prefer XROUTE)");
	return(FALSE);
    }
}

/*
 * nlsp_ipx_sched_spf
 *
 * Utility routine to force SPF (if and only if NLSP is running). In
 * general this should be called only as a side effect of an administrative
 * request (e.g., clear ipx route *).
 */
boolean nlsp_ipx_sched_spf (clns_pdbtype *clns_pdb)
{
    if ((clns_pdb != NULL) && (clns_pdb_running(clns_pdb)) &&
	(clns_pdb->ipx_pdb != NULL) && (clns_pdb->ipx_pdb->running)) {
	isis_trigger_spf(clns_pdb, CLNS_LEVEL1,
			 ISIS_SPF_TRIG_IPXQUERY, NULL, TRUE);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nlsp_common_send
 *
 * pak  -> packet buffer
 * len   = length of data area
 * swidb = output interface
 * nidb  = primary nidb on output interface
 * len   = current data length
 * pt    = packet type for output
 * dnet/dhost/dsock = destination address/socket
 * snet/shost/ssock = source address/socket
 *
 * Note that it is assumed that the data is at ipxdatastart(pak) ...
 * the network layer header will be built at ipxheadstart(pak) before
 * transmission.
 *
 * This routine will dispose of the packet passed to it, either by
 * transmitting it or discarding it.
 */
static boolean nlsp_common_send (paktype *pak,
				 idbtype *swidb,
				 nidbtype *nidb,
				 ushort len,
				 uchar *machost,
				 ushort pt,
				 ulong dnet,
				 uchar *dhost,
				 ulong dsock,
				 ulong snet,
				 uchar *shost,
				 ulong ssock)
{
    ipxhdrtype *ipx;

    if ((nidb == NULL) || (pak == NULL) || (len == 0)) {
	if (pak != NULL)
	    datagram_done(pak);
	return(FALSE);
    }

    /*
     * Setup IPX header:
     */
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    ipx->cksum = NOVELL_NO_CHECKSUM;
    ipx->len = len + sizeof(ipxhdrtype);
    ipx->tc = 0;
    ipx->pt = pt;

    PUT2WD(ipx->ddnet, dnet);
    ieee_copy(dhost, ipx->dhost);
    ipx->dsock = dsock;

    PUT2WD(ipx->ssnet, snet);
    ieee_copy(shost, ipx->shost);
    ipx->ssock = ssock;

    pak->if_output = swidb;
    pak->nif_output = nidb;
    pak->datagramsize = ipx->len;
    pak->linktype = LINK_NOVELL;
    pak->flags |= PAK_PRIORITY;
    return(nov_send1(pak, machost, ipx->len));
}

static uchar nlsp_multicast_ether[IEEEBYTES] = {0x09, 0x00, 0x1b, 0xff, 0xff, 0xff};
static uchar nlsp_multicast_token[IEEEBYTES] = {0xc0, 0x00, 0x10, 0x00, 0x00, 0x00};
static uchar nlsp_multicast_fddi [IEEEBYTES] = {0x09, 0x00, 0x1b, 0xff, 0xff, 0xff};
static uchar nlsp_internal_host  [IEEEBYTES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

/*
 * nlsp_ipx_send_pkt - send a packet
 *
 * Returns TRUE if successful.
 *
 * Note that it is assumed that the data is at ipxdatastart(pak) ...
 * the network layer header will be built at ipxheadstart(pak) before
 * transmission.
 *
 * This routine will dispose of the packet passed to it, by either sending
 * it or discarding it.
 */
boolean nlsp_ipx_send_pkt (clns_pdbtype *pdb, paktype *pak, int length,
			   idbtype *swidb)
{
    boolean result;
    uchar *target_snpa;
    nidbtype *nidb;
    ulong srcnet;
    uchar *srchost;
    isisidbtype *isisidb;

    if (swidb == NULL || ipx_internal_net == 0) {
	datagram_done(pak);
	return(FALSE);
    }
    nidb = IPX_PRIMARY_NIDB(swidb);
    if (nidb == NULL) {
	datagram_done(pak);
	return(FALSE);
    }

    isisidb = get_isis_idb(pdb, swidb);
    if (!isisidb) {
      datagram_done(pak);
      return(FALSE);
    }

    if (!isisidb->isis_interface_up) {        /* Interface is down */
      datagram_done(pak);
      return(TRUE);                   /* Don't complain about encaps! */
    }

    target_snpa = novell_allhosts;
    srcnet = ipx_internal_net;
    srchost = nlsp_internal_host;

    if (!is_p2p(swidb)) {
      /*
       * If it's a LAN, update the target SNPA based on whether all
       * machines on the LAN are multicast capable.
       */
      if (nidb->nov_mcast_capable) {
	if (idb_is_ether_like(swidb))
	  target_snpa = nlsp_multicast_ether;
	else if (is_tokenring(swidb->hwptr))
	  target_snpa = nlsp_multicast_token;
	else if (is_fddi(swidb->hwptr))
	  target_snpa = nlsp_multicast_fddi;
      }
      srcnet = nidb->novellnet;
      srchost = swidb->novell_node;
    }
    
    result = nlsp_common_send(pak, swidb, nidb, length,
			      target_snpa,
			      NOVELL_TYPE_NLSP,
			      0,
			      novell_allhosts,
			      NOVELL_SOCK_NLSP,
			      srcnet,
			      srchost,
			      NOVELL_SOCK_NLSP);
    return(result);
}


/*
 * show_nlsp_neigh
 *
 * show ipx nlsp <tag-id> neighbors [<interface>] [detail]
 */
void show_nlsp_neigh (parseinfo *csb)
{
    char *name;
    clns_pdbtype *pdb;
    novpdbtype *ipx_pdb;

    name = GETOBJ(string,2);

    if (strlen(name)) {
	pdb = find_nlsp_pdb(!strcmp(name, "notag") ? '\0' : name);
	if (pdb) {
	    printf("\nNLSP Level-1 Neighbors: Tag Identifier = %s", name);
	    show_clns_neighbors(pdb, pdb->adjacency_db, 'N', GETOBJ(idb,1), 
				GETOBJ(int,1));
	}
    } else {
	for (ipx_pdb = novpdbQ.qhead; ipx_pdb; ipx_pdb = ipx_pdb->next) {
	    if ((ipx_pdb->proctype == NET_NLSP) && ipx_pdb->osi_pdb) {
		pdb = ipx_pdb->osi_pdb;
		printf("\nNLSP Level-1 Neighbors: Tag Identifier = %s", 
		       strlen(pdb->pname) ? pdb->pname : "notag");
		show_clns_neighbors(pdb, pdb->adjacency_db, 'N', 
				    GETOBJ(idb,1), GETOBJ(int,1));
	    }
	}
    }
}

/*
 * nlsp_ipx_we_are_designated
 * Return TRUE if we are designated router on this interface.
 */
static boolean nlsp_ipx_we_are_designated (idbtype *idb)
{
    novpdbtype *nlsp_pdb;
    nidbtype *nidb = IPX_PRIMARY_NIDB(idb);

    if (!novell_nlsp_enabled)
      return(FALSE);
    if (!nidb)
	return(FALSE);
    nlsp_pdb = novnlsp_find_pdb(nidb->pname);
    if (nlsp_pdb == NULL) {
	return(FALSE);
    }
    return(isis_we_are_dr(nlsp_pdb->osi_pdb, idb, CLNS_LEVEL1));
}

/*
 * isis_age_ipx_routes_and_services
 *
 * Age IPX routes and services attached to an LSP.  Called when an LSP
 * changes, and after all of its associated info has been installed.
 */
static void isis_age_ipx_routes_and_services (clns_pdbtype *pdb, ushort index, 
					      isis_lspix_type *entry)
{
    novpdbtype *novpdb;

    novpdb = pdb->ipx_pdb;
    if (novpdb) {
	novnlsp_newage_routes(novpdb, &entry->chains[ISIS_ROUTE_IPX],
			      entry->lsp_version, index);
	novnlsp_newage_services(novpdb, &entry->chains[ISIS_ROUTE_IPX_SVC],
				entry->lsp_version, index);
	/*
	 * Always run the ager. This is important as what's in the
	 * route/sap tables may be invalid and long gone, yet with
	 * valid information in the urd table. This is the only way
	 * to make sure it gets cleared out.
	 */
	novnlsp_ager_schedule();
    }
}

/*
 * ipx_nlsp_nidb_network_changed
 *
 * Called through registry when an NIDB is coming or going
 */
static void ipx_nlsp_nidb_network_changed (nidbtype *nidb,
					   enum IPX_ROUTE_EVENT event)
{
    novpdbtype *novpdb;
    idbtype *idb;

    idb = nidb->idb;
    if (idb == NULL)
	return;

    for (novpdb = novpdbQ.qhead; novpdb; novpdb = novpdb->next) {

	if (novpdb->proctype != NET_NLSP)
	    continue;

	switch (event) {
	case IPX_ROUTE_EVENT_DELETED:
	    ipx_nlsp_interface_init(nidb, novpdb, FALSE, FALSE);
	    break;
	    
	case IPX_ROUTE_EVENT_ADD:
	    ipx_nlsp_interface_init(nidb, novpdb, TRUE, 
				    (nidb->nov_lsp_enabled && 
				     !nidb->nov_lsp_isis));
	    break;
	    
	default:
	    break;
	}
    }
}

/*
 * ipx_nlsp_hostname_changed
 *
 * Notification when the hostname changes
 *
 * We want to reissue our non-pseudonode LSP in this case, as well
 * as cacheing our hostname.
 */
static void ipx_nlsp_hostname_changed (boolean non_default)
{
    novpdbtype *pdb;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_NLSP && pdb->osi_pdb) {
	    ipx_nlsp_cache_our_hostname(pdb->osi_pdb, TRUE);
	    ipx_mark_non_dr_info_change(pdb);
	}
    }
}

/*
 * ipx_nlsp_bw_delay_changed
 *
 * Notification when the IPX bandwidth or delay changes
 *
 * We want to reissue the non-DR LSP in this case.  We set the cost,
 * in case the bandwidth changed;  this has the side effect of reissuing
 * the non-DR LSP and updating the bandwidth and delay info.
 */
void ipx_nlsp_bw_delay_changed (nidbtype *nidb)
{
    novpdbtype *pdb;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_NLSP && pdb->osi_pdb) {
	    ipx_nlsp_set_cost(nidb, pdb->osi_pdb);
	}
    }
}

/*
 * ipx_nlsp_encap_fixup
 *
 * Called by the system when the encapsulation changes
 */
static void ipx_nlsp_encap_fixup (idbtype *idb, int old_encap)
{
    clns_pdbtype *pdb;
    nidbtype *nidb;

    /* Skip it if this interface has no NLSP interest. */

    nidb = idb->primary_nidb;
    if (!nidb)
	return;

    /* Skip it if no NLSP right now. */

    pdb = find_nlsp_pdb(nidb->pname);
    if (!pdb)
	return;

    /* Blow away any adjacencies. */
    
    delete_interface_adjacencies(pdb->adjacency_db, idb);

    /* Reset the media type. */

    ipx_nlsp_set_media(nidb);

    isis_mark_L1_changed(pdb);
}


/*
 * ipx_nlsp_track_interface_state
 *
 * Track the interface state, based on the physical interface state and
 * the NIDB state.
 */
static void ipx_nlsp_track_interface_state (idbtype *idb, isisidbtype *isisidb,
					    nidbtype *nidb, clns_pdbtype *pdb)
{
    boolean new_state;

    /* Skip it if the state hasn't changed since the last time. */

    new_state = (interface_up(idb) && NOVELL_NIDB_ISUP(nidb));
    if (new_state == isisidb->isis_interface_up)
	return;

    /* It changed.  Note it and start causing trouble. */

    isisidb->isis_interface_up = new_state;

    if (new_state) {			/* It came up */

	ipx_nlsp_set_cost(nidb, pdb);	/* Pick up any new cost from IPXWAN */
	if (isisidb->isis_running)
	    isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);

    } else {				/* It went down */

	/* Delete all adjacencies. */

	delete_interface_adjacencies(pdb->adjacency_db, idb);

	/* Purge any pseudonode LSP of ours for this interface. */

	if (isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L1))
	    isis_purge_dr(ISIS_CIRCUIT_L1, pdb, isisidb);

	/* Zap the circuit ID. */

	isis_reset_circuit_id(pdb, idb);

	/*
	 * Possibly remove drop routes:
	 */
	rn_walktree_blocking(nov_rnhead->rnh_treetop,
			     ipx_nlsp_route_adjust_patricia, 
			     pdb->ipx_pdb, idb);
    }

    /* Note the interface state in the SRM mask in the PDB. */

    isis_manipulate_srm_mask(pdb, isisidb);

    isis_mark_L1_changed(pdb);
    isis_mark_L1_dr_changed(pdb, isisidb);
}

/*
 * ipx_nlsp_update_interface_state
 *
 * Called by the system when the interface state changes
 */
static void ipx_nlsp_update_interface_state (idbtype *idb, clns_pdbtype *pdb)
{
    isisidbtype *isisidb;
    nidbtype *nidb;

    nidb = idb->primary_nidb;
    if (!nidb)
	return;

    if (!pdb)
	return;

    /* Skip it if this interface has no NLSP interest. */

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /* Try manipulating it. */
 
    ipx_nlsp_track_interface_state(idb, isisidb, nidb, pdb);
}

/*
 * ipx_nlsp_route_adjust
 */
static void ipx_nlsp_route_adjust (idbtype *idb)
{
    nidbtype *nidb;
    novpdbtype *pdb;

    nidb = idb->primary_nidb;
    if (!nidb)
	return;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_NLSP) {
	    ipx_nlsp_update_interface_state(idb, pdb->osi_pdb);
	}
    }
}

/*
 * ipx_nlsp_nidb_state_change
 *
 * Called by IPX when the NIDB state changes
 */
static void ipx_nlsp_nidb_state_change (nidbtype *nidb)
{
    clns_pdbtype *pdb;
    idbtype *idb;
    isisidbtype *isisidb = NULL;
    novpdbtype *novpdb;

    idb = nidb->idb;
    for (novpdb = novpdbQ.qhead; novpdb; novpdb = novpdb->next) {
	if (novpdb->proctype == NET_NLSP) {
	    pdb = novpdb->osi_pdb;

	    /* Skip it if no NLSP right now. */

	    if (!pdb)
		continue;

	    /* Skip it if this interface has no NLSP interest. */
	    if (!isisidb)
		isisidb = get_isis_idb(pdb, idb);
	    if (!isisidb)
		continue;
	    
	    /* Try manipulating it. */
	    
	    ipx_nlsp_track_interface_state(idb, isisidb, nidb, pdb);
	}
    }
    if (isisidb)
	isisidb->isis_interface_up = (interface_up(idb) && NOVELL_NIDB_ISUP(nidb));
}

/*
 * show_nlsp_traffic
 *
 * Display NLSP traffic statistics
 */
static void show_nlsp_traffic (void)
{
    printf("\nNLSP:   Level-1 Hellos received %d, sent %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_HELLO_IN],
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_HELLO_OUT]);
    printf("\n        PTP Hello received %d, sent %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_PTP_HELLO_IN],
	   isis_traffic[PROC_TYPE_NLSP][ISIS_PTP_HELLO_OUT]);
    printf("\n        Level-1 LSPs received %d, sent %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_LSP_IN],
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_LSP_OUT]);
    printf("\n        LSP Retransmissions: %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_RETRANSMITS]);
    printf("\n        LSP checksum errors received: %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_CKSUM_ERR]);
    printf("\n        LSP HT=0 checksum errors received: %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_CKSUM_ERR_HT0]);
    printf("\n        Level-1 CSNPs received %d, sent %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_CSNP_IN],
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_CSNP_OUT]);
    printf("\n        Level-1 PSNPs received %d, sent %d",
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_PSNP_IN],
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_PSNP_OUT]);
    printf("\n        Level-1 DR Elections: %d", 
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_DR_ELECTS]);
    printf("\n        Level-1 SPF Calculations: %d", 
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_SPF_DONE]);
    printf("\n        Level-1 Partial Route Calculations: %d", 
	   isis_traffic[PROC_TYPE_NLSP][ISIS_L1_PARTIALS]);
}

/*
 * show_nlsp_interface
 *
 * Display NLSP interface information
 */
static void show_nlsp_interface (idbtype *idb)
{
    isisidbtype *isisidb;
    clns_pdbtype *pdb;
    nidbtype *nidb;


    if (NULL == (nidb = IPX_PRIMARY_NIDB(idb)))
	return;

    if (!nidb->nov_lsp_enabled)
	return;
    pdb = find_nlsp_pdb(nidb->pname);
    if (!pdb)
	return;

    isisidb = get_isis_idb(pdb, idb);
    if (isisidb) {
	printf("\n    Level 1 Hello interval %d sec"
	       "\n    Level 1 Designated Router Hello interval %d sec",
	       isisidb->isis_L1_hello_interval/ONESEC,
	       isisidb->isis_L1_dr_hello_interval/ONESEC);
	printf("\n    Level 1 CSNP interval %d sec",
	       isisidb->csnp_ctl[0].csnp_interval/ONESEC);
	printf("\n    Level 1 LSP retransmit interval %d sec, "
	       "LSP (pacing) interval %d mSec",
	       isisidb->isis_retrans_interval/ONESEC,
	       isisidb->pacing_interval);
	printf("\n    Level 1 adjacency count is %d",
	       isisidb->isis_adj_count[0]);
	printf("\n    Level 1 circuit ID is %s",
	       systemid_name_string(pdb, isisidb->isis_L1_circuit_id));
	if (!is_p2p(idb)) {
	    printf("\n    Level 1 Designated Router is %s",
		   (*pdb->stationid_lookup)(pdb, isisidb->isis_L1_dr_id));
	}
    }
}

/*
 * ipx_nlsp_clear_all
 *
 * Clear the LSP database and all adjacencies.
 */
void ipx_nlsp_clear_all (clns_pdbtype *pdb)
{
    if (pdb) {
	isis_restart(pdb);
    } else {
	printf("\n%%NLSP not running");
    }
}

/*
 * isis_clear_nlsp_neighbors
 *
 * Clear all NLSP neighbors
 */
void isis_clear_nlsp_neighbors (clns_pdbtype *pdb)
{
    clns_adjtype *adj, *next, *next_alias;
    int          bucket;

    for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
	for (adj = pdb->adjacency_db[bucket]; adj; adj = next) {
	    next = adj->next;
	    for (; adj; adj = next_alias) {
		next_alias = adj->next_alias;
		delete_adjacency(pdb->adjacency_db, adj);
	    }
	}
    }
}

/*
 * ipx_nlsp_name_match_number
 *
 * Say whether or not a name matches an address.
 *
 * We only match the host part, since this is all that gets stored
 * (it's really a system ID).
 */
static boolean ipx_nlsp_name_match_number (nametype *ptr, addrtype *address)
{
    return(bcmp(ptr->v.ipx.adr.nov_addr.host, address->nov_addr.host,
		IEEEBYTES) == 0);
}

/*
 * ipx_nlsp_cache_hostname
 *
 * Add an NLSP router hostname to the name cache
 */
static void ipx_nlsp_cache_hostname (clns_pdbtype *pdb, isis_lspdbtype *lsp_db,
				     isis_TLV_parse *tlv_parse)
{
    isis_nlsp_mgmt_info *mgmt_info;

    /* Bail if it's not all there. */

    if (!pdb || !lsp_db || !tlv_parse)
	return;

    /* Bail if it's not fragment zero of a non-pseudonode LSP. */

    if (lsp_db->data->lspid.pseudo_id || lsp_db->data->lspid.lsp_number)
	return;

    /* Bail if the LSP is expired. */

    if (!lsp_db->lsp_active)
	return;

    /* Bail if there's no management info TLV. */

    mgmt_info = (isis_nlsp_mgmt_info *) 
	tlv_parse->TLV_array[ISIS_NLSP_MGMT_INFO]->value;
    if (!mgmt_info)
	return;

    /* Store the hostname in the cache. */

    ipx_nlsp_cache_name(mgmt_info->router_name, mgmt_info->router_name_length,
			lsp_db->data->lspid.system_id, 
			mgd_timer_exp_time(&lsp_db->lsp_timer));
}

/*
 * ipx_nlsp_show_hosts
 *
 * Display a host entry for "show hosts"
 */
static void ipx_nlsp_show_hosts (nametype *name_ptr, char *continue_text)
{
    printf("  NLSP   %e", name_ptr->v.ipx.adr.nov_addr.host);
}

/*
 * ipx_nlsp_validate_tlv
 *
 * Called from packet validity processing to validate a TLV in an
 * NLSP packet
 */
static boolean ipx_nlsp_validate_tlv (clns_pdbtype *pdb,
				      isis_info_type *opt_ptr,
				      void *packet,
				      char *error_type)
{
    isishdrtype *isis;
    isis_LSP_hdrtype *lsp;
    isis_nlsp_area_addrs *area_addrs;
    isis_nlsp_mgmt_info *mgmt_info;
    isis_nlsp_link_info *link_info;
    boolean pseudonode_lsp;
    uchar *data_ptr;
    int i;
    char router_name_string[ISIS_NLSP_SVC_MAXLEN + 1];

    isis = packet;
    switch (opt_ptr->code) {


	/* Area addresses must be well-formed. */
	
      case ISIS_NLSP_AREA_ADDRS:
	if ((opt_ptr->length % ISIS_NLSP_AREA_ADDRS_LEN) != 0 ||
	    opt_ptr->length < ISIS_NLSP_AREA_ADDRS_LEN ||
	    opt_ptr->length > 
	    ISIS_NLSP_AREA_ADDRS_LEN * ISIS_NLSP_MAX_AREA_ADDRS) {
	    sprintf(error_type, "Bad area addr length %d",
		    opt_ptr->length);
	    return(FALSE);
	}
	
	data_ptr = opt_ptr->value;
	for (i=0; i<opt_ptr->length / ISIS_NLSP_AREA_ADDRS_LEN; i++) {
	    ulong net, mask;
	    area_addrs = (isis_nlsp_area_addrs *) data_ptr;
	    net = GETLONG(&area_addrs->net_number);
	    mask = GETLONG(&area_addrs->net_mask);
	    if ((net & mask) != net) {
		sprintf(error_type, "Bad net/mask combo %x/%x",
			net, mask);
		return(FALSE);
	    }
	    data_ptr += ISIS_NLSP_AREA_ADDRS_LEN;
	}
	break;
	
	
	/* NLSP management info must be well-formed as well. */
	
      case ISIS_NLSP_MGMT_INFO:

	/* Better be an LSP. */
	
	if (isis->type != ISIS_L1_LSP) {
	    sprintf(error_type, "Mgmt info in wrong packet type");
	    return(FALSE);
	}

	lsp = packet;
	pseudonode_lsp = isis_lspid_is_pseudonode(&lsp->lspid);
	
	mgmt_info = (isis_nlsp_mgmt_info *) opt_ptr->value;
	if (opt_ptr->length < ISIS_NLSP_MGMT_INFO_LEN ||
	    opt_ptr->length <
	    ISIS_NLSP_MGMT_INFO_LEN + mgmt_info->router_name_length) {
	    sprintf(error_type, "Bad mgmt info length %d",
		    opt_ptr->length);
	    return(FALSE);
	}

#ifdef notdef				/* Disabled by dkatz for the moment */

	/* Validate name length;  pseudonode can be zero, others must be NZ. */

	if (mgmt_info->router_name_length > ISIS_NLSP_SVC_MAXLEN ||
	    (!pseudonode_lsp && mgmt_info->router_name_length == 0)) {
	    sprintf(error_type, "Bad router name length %d",
		    mgmt_info->router_name_length);
	    return(FALSE);
	}

#endif
	
	/* The name and net number must be different than ours. */
	
	if (!station_match(pdb->systemId, lsp->lspid.system_id)) { /* Not us */
	    if (!pseudonode_lsp) {	/* Only applies to non-pseudonode */
		if (strlen(hostname) == mgmt_info->router_name_length) {
		    if (!bcmp(hostname, mgmt_info->router_name,
			      mgmt_info->router_name_length)) {
			sprintf(error_type, "Name matches ours");
			return(FALSE);
		    }
		}
		if (GETLONG(&mgmt_info->net_number) == ipx_internal_net) {
		    sprintf(error_type, "Internal net matches ours");
		    return(FALSE);
		}
	    }
	} else {

	    /*
	     * Same system ID.  If the name doesn't match in the non-pseudonode
	     * LSP, there might be someone with the same system ID out there!
	     */
	    if (!pseudonode_lsp) {
		if (strlen(hostname) != mgmt_info->router_name_length ||
		    bcmp(hostname, mgmt_info->router_name,
			 mgmt_info->router_name_length)) {
		    bcopy(mgmt_info->router_name, router_name_string,
			  mgmt_info->router_name_length);
		    router_name_string[mgmt_info->router_name_length] = '\0';
		    sprintf(error_type, "Possible duplicate system ID, "
			    "other system name is %s", router_name_string);
		    return(FALSE);
		}
	    }
	}
	break;
	
	
	/* NLSP link info has a bunch of requirements. */
	
      case ISIS_NLSP_LINK_INFO:
	
	/* Better be an LSP. */
	
	if (isis->type != ISIS_L1_LSP) {
	    sprintf(error_type, "Link info in wrong packet type");
	    return(FALSE);
	}
	
	lsp = packet;
	pseudonode_lsp = isis_lspid_is_pseudonode(&lsp->lspid);
	
	/* Length must be reasonable. */
	
	if (opt_ptr->length < ISIS_NLSP_LINK_INFO_LEN) {
	    sprintf(error_type, "Bad link info length %d",
		    opt_ptr->length);
	    return(FALSE);
	}
	
	/* Stupid special bits must be zero. */
	
	link_info = (isis_nlsp_link_info *) opt_ptr->value;
	if (link_info->S1 || link_info->external) {
	    sprintf(error_type, "S1/extern bits set");
	    return(FALSE);
	}

	if (pseudonode_lsp) {		/* Pseudonode LSP */
	    
	    /* Cost must be zero in pseudonode LSP. */
	    
	    if (link_info->cost != 0) {
		sprintf(error_type, "Nonzero cost in DR LSP");
		return(FALSE);
	    }
	} else {			/* Non-pseudonode LSP */

	    /* Metrics must be nonzero if non-pseudonode LSP. */
	    
	    if (!GETLONG(&link_info->mtu_size) ||
		!GETLONG(&link_info->delay) ||
		!GETLONG(&link_info->throughput) ||
		!link_info->cost) {
		sprintf(error_type, "Zero metrics in non-DR LSP");
		return(FALSE);
	    }
	}
	break;
    }
    return(TRUE);
}

void ipx_nlsp_display_all_database (boolean long_display, 
				    boolean internal_display)
{
    clns_pdbtype *isis_pdb;
    novpdbtype   *pdb;
    int call_level;

    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == NET_NLSP) && pdb->osi_pdb) {
	    isis_pdb = pdb->osi_pdb;
	    printf("\nNLSP Level-1 Link State Database: Tag Identifier = %s", 
		   strlen(isis_pdb->pname) ? isis_pdb->pname : "notag");
	    printf(isis_data_str);
 	    call_level = CLNS_LEVEL1;
  	    isis_recursive_walk(NULL, ISIS_WALK_ALL,
  				isis_pdb->level1_LSPs, 
 				isis_rw_display_lsp, 
 				&call_level, &long_display,
 				&internal_display, isis_pdb);
	}
    }
}

/*
 * isis_ipx_system_init
 *
 * Initialize registries, etc. for IPX NLSP
 */
void isis_ipx_system_init (void)
{
    reg_add_ipx_nlsp_mark_all_L1_dr_changed(ipx_nlsp_mark_all_L1_dr_changed, 
					    "ipx_nlsp_mark_all_L1_dr_changed");
    reg_add_ipx_nlsp_display_all_database(ipx_nlsp_display_all_database,
					  "ipx_nlsp_display_all_database");
    reg_add_ipx_isis_union_areas(ipx_isis_union_areas, "ipx_isis_union_areas");
    reg_add_nlsp_pdb_init(nlsp_pdb_init, "nlsp_pdb_init");
    reg_add_ipx_find_nlsp_pdb(find_nlsp_pdb, "find_nlsp_pdb");
    reg_add_ipx_isis_add_ipx_route(isis_add_ipx_route, "isis_add_ipx_route");
    reg_add_ipx_isis_add_ipx_service(isis_add_ipx_service,
				     "isis_add_ipx_service");
    reg_add_ipx_isis_insert_ipx_hello(isis_insert_ipx_hello,
				      "isis_insert_ipx_hello");
    reg_add_ipx_isis_insert_ipx_lsp_areas(isis_insert_ipx_lsp_areas,
					  "isis_insert_ipx_lsp_areas");
    reg_add_ipx_isis_add_one_nlsp_link_ad(isis_add_one_nlsp_link_ad,
					  "isis_add_one_nlsp_link_ad");
    reg_add_ipx_isis_insert_ipx_lsp(isis_insert_ipx_lsp,
				    "isis_insert_ipx_lsp");
    reg_add_ipx_isis_pseudonode_needed(ipx_isis_pseudonode_needed,
				       "ipx_isis_pseudonode_needed");
    reg_add_ipx_nlsp_ipx_sched_spf(nlsp_ipx_sched_spf, "nlsp_ipx_sched_spf");
    reg_add_ipx_nlsp_ipx_send_pkt(nlsp_ipx_send_pkt, "nlsp_ipx_send_pkt");
    reg_add_ipx_nlsp_we_are_designated(nlsp_ipx_we_are_designated,
				       "nlsp_ipx_we_are_designated");
    reg_add_ipx_mark_dr_info_change(ipx_mark_dr_info_change,
				    "ipx_mark_dr_info_change");
    reg_add_ipx_nidb_network_changed(ipx_nlsp_nidb_network_changed,
				     "ipx_nlsp_nidb_network_changed");
    reg_add_hostname_changed(ipx_nlsp_hostname_changed, 
			     "ipx_nlsp_hostname_changed");
    reg_add_ipx_show_nlsp_traffic(show_nlsp_traffic, "show_nlsp_traffic");
    reg_add_ipx_nidb_bw_delay_changed(ipx_nlsp_bw_delay_changed, 
				   "ipx_nlsp_bw_delay_changed");
    reg_add_route_adjust(ipx_nlsp_route_adjust, "ipx_nlsp_route_adjust");
    reg_add_ipx_nidb_state_change(ipx_nlsp_nidb_state_change,
				  "ipx_nlsp_nidb_state_change");
    reg_add_encap_fixup(ipx_nlsp_encap_fixup, "ipx_nlsp_encap_fixup");

    reg_add_ipx_show_interface(show_nlsp_interface, "show_nlsp_interface");
    reg_add_ipx_nlsp_age_ipx_routes_and_services
	(isis_age_ipx_routes_and_services, "isis_age_ipx_routes_and_services");
    reg_add_ipx_nlsp_set_default_redist
	(ipx_nlsp_set_default_redist, "ipx_nlsp_set_default_redist");
    reg_add_ipx_nlsp_clear_neighbors
	(isis_clear_nlsp_neighbors, "isis_clear_nlsp_neighbors");
    reg_add_name_match_number(ADDR_NOVELL, ipx_nlsp_name_match_number,
			      "ipx_nlsp_name_match_number");
    reg_add_name_show_hosts(NAM_IPX, ipx_nlsp_show_hosts,
			    "ipx_nlsp_show_hosts");
    reg_add_ipx_nlsp_cache_hostname(ipx_nlsp_cache_hostname,
				    "ipx_nlsp_cache_hostname");
    reg_add_ipx_nlsp_validate_tlv(ipx_nlsp_validate_tlv,
				  "ipx_nlsp_validate_tlv");
    reg_add_ipx_nlsp_update_interface_state(ipx_nlsp_update_interface_state,
					    "ipx_nlsp_update_interface_state");
}
