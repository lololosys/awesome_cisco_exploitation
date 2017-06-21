/* $Id: isis.c,v 3.13.12.22 1996/09/12 00:17:08 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis.c,v $
 *------------------------------------------------------------------
 * isis.c -- IS to IS Dynamic Routing Protocol
 *
 * 5-October-1990, Paulina Knibbe
 *
 * NLSP support added September 1994 by Dave Katz
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This code supports both ISO 10589 IS-IS and the Netware Link
 * Services Protocol (NLSP).
 *------------------------------------------------------------------
 * $Log: isis.c,v $
 * Revision 3.13.12.22  1996/09/12  00:17:08  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.13.12.21  1996/08/28  12:38:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.12.20  1996/08/16  04:43:44  dkatz
 * CSCdi66139:  Stopping CLNS IS-IS process causes a router crash
 * Branch: California_branch
 * Null out a couple of fields after freeing the memory pointed to
 * therefrom.
 *
 * Revision 3.13.12.19  1996/08/11  15:12:59  dkatz
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
 * Revision 3.13.12.18  1996/08/08  15:48:14  dkatz
 * CSCdi65479:  ISIS/NLSP mishandles purged packets
 * Branch: California_branch
 * Handle purged LSP flooding rules correctly (at last).  Keep purged
 * packets for a full LSP lifetime in all cases except when they expire
 * from natural causes.  Remove kludge that keeps SRM-set purged LSPs
 * in the database, since we now keep the LSPs long enough after purging.
 *
 * Revision 3.13.12.17  1996/08/08  14:50:40  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.13.12.16  1996/08/07  08:57:17  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.12.15  1996/08/04  23:35:01  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.13.12.14  1996/08/02  02:47:47  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.13.12.13  1996/06/18  16:50:36  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.13.12.12  1996/06/18  16:31:23  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Force the protocol to go to sleep long enough for all neighbor
 * adjacencies to time out when the level is changed.  While we're
 * in there, do it for NET changes as well (since these can change
 * adjacency levels).
 *
 * Revision 3.13.12.11  1996/05/26  06:31:59  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.13.12.10  1996/05/22  17:04:04  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.13.12.9  1996/05/14  01:16:26  sluong
 * CSCdi52574:  NLSP LSP for statics lost after clear ipx nl *
 * Branch: California_branch
 *
 * Revision 3.13.12.8  1996/05/10  19:45:30  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.13.12.7  1996/05/07  17:39:47  dkatz
 * CSCdi56547:  ISIS/NLSP have problems with more than 42 neighbors
 * Branch: California_branch
 * Allow neighbors to be spread across multiple TLVs (on both send
 * and receive.)
 *
 * Revision 3.13.12.6  1996/05/07  17:15:47  dkatz
 * CSCdi54576:  ISIS LSP pacing rate needs to be configurable
 * Branch: California_branch
 * Provide a knob to alter the LSP pacing timer;  the timer is already
 * there.
 *
 * Revision 3.13.12.5  1996/05/03  01:50:36  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.13.12.4  1996/04/21  20:47:23  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.13.12.3  1996/04/12  00:36:20  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.13.12.2  1996/04/09  17:45:27  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.13.12.1  1996/03/18  19:03:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  06:30:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/13  01:13:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.11.2.2  1996/03/07  08:40:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  00:21:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/19  21:32:30  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.12  1996/01/29  07:26:47  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.11  1996/01/18  21:48:18  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/05  04:55:30  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.9  1995/12/15  21:30:41  asastry
 * CSCdi45586:  Configuring ISIS 'net' multiple times results in multiple
 * ISIS update and adjacency processes
 *
 * Revision 3.8  1995/12/11  04:45:23  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.7  1995/12/01  17:01:40  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.6  1995/12/01  09:21:02  dkatz
 * CSCdi43081:  Detailed display of single LSP broken (regress)
 *
 * Revision 3.5  1995/12/01  02:48:20  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.4  1995/11/30  21:47:25  sluong
 * CSCdi44536:  isis/nlsp does not generate the non-pseudonode
 *
 * Revision 3.3  1995/11/17  08:53:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  20:52:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/21  19:03:01  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.16  1995/09/23  22:53:10  dkatz
 * CSCdi39582:  ISIS/NLSP LSP flooding may get stuck
 * Make sure that the lsp send flag stays in sync with the state of
 * the LSP pacing timer.
 *
 * Revision 2.15  1995/09/23  22:48:17  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.14  1995/09/23  21:49:15  dkatz
 * CSCdi38298:  Display of numeric ISIS LSP ID causes name lookup
 * Do numeric lookup before the name lookup.
 *
 * Revision 2.13  1995/09/23  21:37:31  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.12  1995/08/25  16:17:03  dkatz
 * CSCdi38585:  ISIS wont send serial IIHs if L2-only
 * Turn on the level-1 IIH timer for point-to-point interfaces, since
 * there's only one level of IIH for P2P lines.
 *
 * Revision 2.11  1995/08/25  16:02:47  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.10  1995/08/25  15:24:22  dkatz
 * CSCdi38313:  ISIS may keep corrupted LSPs
 * Validate the LSP checksum before refreshing local LSPs.
 *
 * Revision 2.9  1995/08/25  14:59:41  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.8  1995/08/25  05:39:55  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.7  1995/08/23  23:24:47  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Remember Pleasant Paul's Mantra.
 *
 * Revision 2.6  1995/08/23  18:25:56  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.5  1995/08/23  18:12:44  dkatz
 * CSCdi37879:  ISIS L2 LSPs needlessly flooded
 * Don't try to check the SPF instance on dummy NDBs.
 *
 * Display the time left for zero-age LSPs so we don't get confused.
 *
 * Revision 2.4  1995/08/23  18:03:02  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.3  1995/06/21  08:53:43  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/09  12:59:07  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:18:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This module is an implementation of the ISIS routing protocol.
 */
#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "mgd_timers.h"
#include "subsys.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_ipx.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "../iprouting/route.h"
#include "parser.h"
#include "../clns/parser_defs_isis.h"
#include "isis_inlines.h"
#include "bitlogic.h"
#include "../xns/ipx_registry.h"
#include "clns_registry.h"
#include "isis_registry.h"
#include "../os/chunk.h"

/*
 * ISIS related string definitions.
 */
const char isis_L1_str[] = { "level-1" };
const char isis_L1L2_str[] = { "level-1-2" };
const char isis_L2_str[] = { "level-2" };
const char isis_L2only_str[] = { "level-2-only" };
ulong isis_traffic[ISIS_NUM_PROC_TYPE][ISIS_MAX_TRAFFIC]; /* Traffic info */

/*
 * Support for more than 255 interfaces
 *
 * The IS-IS specification implies that a router can have no more than 255
 * interfaces.  Upon careful inspection of the spec, it turns out that
 * the real limitation is to have no more than 255 pseudonode LSPs generated
 * by a single system (since the pseudonode ID is eight bits wide).  In
 * particular, point-to-point links can never have associated pseudonodes
 * in IS-IS (they can in NLSP) so the real limit is 255 LAN interfaces.
 *
 * Note that point-to-point interfaces do have eight bit circuit numbers
 * announced in hello packets, but these are used only for adjacency
 * negotiation and are never announced.
 *
 * So the strategy is to allocate circuit numbers from separate pools for
 * LAN and P2P interfaces, and to simply truncate the circuit numbers on
 * P2P interfaces to eight bits when announced in the protocol.  We assign
 * circuit numbers on P2P interfaces starting at 0x100 just to keep from
 * getting confused.
 *
 * We implement this situation with arrays of bitfields.  isis_idb_bitfield
 * is used to assign internal IDB numbers to each interface.  These are
 * the ordinals associated with the SRM, etc., bits.  LAN and P2P
 * interfaces will be intermixed, and the number space will be compact.
 *
 * isis_lan_bitfield is used to allocate circuit numbers for LAN interfaces.
 * This is bound to 255 at most.  We explicitly set bit 0 when we create
 * the bitfield to disallow circuit number 0;  legal values are 1-255.
 *
 * isis_p2p_bitfield is used to allocate circuit numbers for P2P interfaces.
 * We add 0x100 to the bit number to get the circuit number.
 */
static dynamic_bitfield *isis_idb_bitfield; /* Array per process */
static dynamic_bitfield *isis_lan_bitfield; /* Ditto, for LAN i/fs */
static dynamic_bitfield *isis_p2p_bitfield; /* Ditto, for P2P i/fs */


/*
 * get_isis_idb
 *
 * Returns the ISIS IDB given the PDB and IDB, or NULL
 *
 * Tolerates null IDB and PDB pointers.
 */
isisidbtype *get_isis_idb (clns_pdbtype *pdb, idbtype *idb)
{
    int isisindex;

    if (!idb)
	return(NULL);
    if (!pdb)
	return(NULL);
    if (!idb->isis_idb)
	return(NULL);
    if (!isis_pdbindex_valid(pdb->index))
	return(NULL);
    isisindex = isis_pdbindex_to_procindex(pdb->index);
    return (idb->isis_idb[isisindex]);
}

/*
 * isis_trigger_dr_election
 *
 * Trigger the hello process to run a DR election.
 */
void isis_trigger_dr_election (clns_pdbtype *pdb, isisidbtype *isisidb,
			       ulong level)
{
    if (!pdb || !isisidb)
	return;

    /* Set the process flag. */

    pdb->run_dr_election = TRUE;

    /* Set the appropriate level flag. */

    if (level & ISIS_CIRCUIT_L1)
	isisidb->isis_compute_L1_dr = TRUE;
    if (level & ISIS_CIRCUIT_L2)
	isisidb->isis_compute_L2_dr = TRUE;

    /* Wake up the process. */

    if (pdb->router_wakeup)
	process_set_boolean(pdb->router_wakeup, TRUE);
}

/*
 * isis_lsp_refresh_interval
 *
 * Returns the active value of the LSP refresh interval.  This is either
 * the configured value, or thirty seconds shy of the LSP lifetime if
 * the configured value is higher than this.
 *
 * This allows things to be configured in any order, a problem that we've
 * had to fix twice now...
 */

ulong isis_lsp_refresh_interval (clns_pdbtype *pdb)
{
    ulong value;

    if (pdb->lsp_lifetime <= ISIS_LSP_REFRESH_OFFSET) {
	value = ONESEC;		/* Unworkable, but hey... */
    } else {
	value = min(pdb->lsp_refresh_interval,
		    pdb->lsp_lifetime - ISIS_LSP_REFRESH_OFFSET);
    }
    return(value);
}

/*
 * isis_we_are_dr - returns TRUE if we are DR at the given level on an
 * interface
 *
 * P2P interfaces are DR in NLSP if NLSP isn't actually running on
 * them (!), or is running but has no adjacencies.
 */
boolean isis_we_are_dr (clns_pdbtype *pdb, idbtype *idb, ulong level)
{
    isisidbtype *isisidb;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return(FALSE);

    if (is_p2p(idb)) {

	/* ISIS point-to-point links are never DR. */

	if (pdb->proc_type == PROC_TYPE_ISIS) {
	    return(FALSE);
	}

	/*
	 * Point-to-point links are DR for NLSP if NLSP isn't running there,
	 * or if NLSP is running but there are no adjacencies.
	 */
	if (isisidb->isis_running && isisidb->isis_adj_count[level - 1]) {
	    return(FALSE);		/* Active NLSP, so no DR. */
	} else {
	    return(TRUE);		/* Inactive NLSP. */
	}
    }

    /* LAN links.  We're DR if the DR ID says so. */

    return(isis_system_is_dr(pdb, pdb->systemId, isisidb, level));
}

/*
 * lspid_copy
 *
 * Copy an LSP ID.
 */
void lspid_copy (const isis_lspid *lsp1, isis_lspid *lsp2)
{
    bcopy ((uchar *) lsp1, (uchar *) lsp2, ISIS_LSPID_LENGTH);
}

/*
 * lspid_name_string
 *
 * Display an LSPID. If a name is defined for the station id it will
 * be prefixed to the pseudo-id and LSP number.
 *
 * No name:   0000.0c00.1111.00-00
 * With name: cisco.00-00
 */

char *lspid_name_string (clns_pdbtype *pdb, isis_lspid *lspid)
{
    char *str, *buf;

    str = (*pdb->stationid_lookup)(pdb, lspid->system_id);
    buf = next_buf();
    sstrncpy(buf, str, STATION_STRING_LENGTH+1);
    sprintf(buf + strlen(buf), ".%02x-%02x", lspid->pseudo_id,
	    lspid->lsp_number);

    return(buf);
}

/*
 * lspid_string
 *
 * Produces a string in the form xxxx.xxxx.xxxx.yy-zz which correpsonds
 * to an IS-IS LSP id.
 *
 */
char *lspid_string (isis_lspid *lspid)
{
    char  *ptr, *p2;
    int   i;
    uchar nibble;
    uchar *lspid_ptr;

    lspid_ptr = (uchar *) lspid;	/* Treat it as all bytes. */

    ptr = next_buf();
    p2 = ptr;
    for (i = 0; i < SYSTEMID_LENGTH; i++) {
	if (i && !(i % 2)) {
	    *ptr++ = '.';
	}
	nibble = lspid_ptr[i] >> 4;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	nibble = lspid_ptr[i] & 0x0f;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
    }
    *ptr++ = '-';
    nibble = lspid->lsp_number >> 4;
    *ptr++ = nibble + (nibble < 10 ? '0' : '7');
    nibble = lspid->lsp_number & 0x0f;
    *ptr++ = nibble + (nibble < 10 ? '0' : '7');
    *ptr = '\0';
    return(p2);
}

/*
 * find_isis_pdb
 *
 */
clns_pdbtype *find_isis_pdb (void)
{
    clns_pdbtype *pdb;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->typeflag & ISO_ISISMASK) break;
    }
    return(pdb);
}
    

/*
 * isis_default_csnp_timer
 *
 * Returns the default CSNP timer value in milliseconds for this interface
 * type and process type.
 */
ulong isis_default_csnp_timer (idbtype *idb, isis_proc_type proc_type)
{
    if (is_p2p(idb)) {
	return(0);
    } else {
	if (proc_type == PROC_TYPE_ISIS) {
	    return(ISIS_DEF_CSNP_TIMER);
	} else {
	    return(IPX_NLSP_CSNP_IVL_DEFAULT * ONESEC);
	}
    }
}

/*
 * Allocate and initialize an IS-IS IDB block, if necessary
 *
 * Returns a pointer to the block or NULL if no memory.
 */
isisidbtype *isis_alloc_isisidb (idbtype *idb, isis_proc_type proc_type,
				 ulong index)
{
    isisidbtype *isisidb;
    ulong	idb_number, ckt_number, ckt_limit, ckt_offset;
    dynamic_bitfield *ckt_bitfield;
    int		level_ix, isisindex;

    /* First create the table in the IDB if there isn't any. */

    if (!idb->isis_idb) {
	idb->isis_idb = malloc_named(sizeof(isisidbtype *) *
				     MAX_ISISNLSP_PROCESSES, "ISIS IDB table");
	if (!idb->isis_idb)
	    return(NULL);
    }

    /* Next, create the bitfield tables if there aren't any. */

    if (!isis_idb_bitfield) {
	isis_idb_bitfield = malloc_named(sizeof(dynamic_bitfield) *
					 MAX_ISISNLSP_PROCESSES,
					 "ISIS IDB bitfields");
	if (!isis_idb_bitfield)
	    return(NULL);
    }
    if (!isis_lan_bitfield) {
	isis_lan_bitfield = malloc_named(sizeof(dynamic_bitfield) *
					 MAX_ISISNLSP_PROCESSES,
					 "ISIS IDB bitfields");
	if (!isis_lan_bitfield)
	    return(NULL);
    }
    if (!isis_p2p_bitfield) {
	isis_p2p_bitfield = malloc_named(sizeof(dynamic_bitfield) *
					 MAX_ISISNLSP_PROCESSES,
					 "ISIS IDB bitfields");
	if (!isis_p2p_bitfield)
	    return(NULL);
    }

    if (!isis_pdbindex_valid(index))
	return(NULL);
    isisindex = isis_pdbindex_to_procindex(index);
    isisidb = idb->isis_idb[isisindex];
    if (isisidb)
	return(isisidb);

    /*
     * Assign a circuit number.  For LANs we limit the count;  for
     * P2P interfaces we don't.
     */
    if (!is_p2p(idb) ||
	proc_type == PROC_TYPE_NLSP) { /* May have pseudonode */
	ckt_limit = ISIS_LAN_CKT_COUNT;	/* Limit the circuits */
	ckt_bitfield = &isis_lan_bitfield[isisindex];
	ckt_offset = 0;			/* Number from zero */
	bitfield_set(0, ckt_bitfield);	/* Ensure we don't assign ckt 0 */
    } else {				/* No pseudonodes */
	ckt_limit = 0;			/* No limit */
	ckt_bitfield = &isis_p2p_bitfield[isisindex];
	ckt_offset = ISIS_P2P_CKT_OFFSET; /* Number from 0x100 */
    }

    if (!(bitfield_find_first_clear(ckt_bitfield, &ckt_number))) {

	/* No free bits.  If we are maxed, bail. */
	
	if (ckt_limit && ckt_bitfield->bitfield_size >= ckt_limit)
	    return(NULL);		/* Can't expand the field. */
	
	ckt_number = ckt_bitfield->bitfield_size; /* Expand it. */
    }

    /* Got a bit.  If it's past the max, bail.  Otherwise, use it. */
    
    if (ckt_limit && ckt_number >= ckt_limit)
	return(NULL);			/* Hit our limit. */

    bitfield_set(ckt_number, ckt_bitfield);

    /* Assign an interface number.  This field can expand as necessary. */

    if (!(bitfield_find_first_clear(&isis_idb_bitfield[isisindex],
				    &idb_number))) {
	idb_number = isis_idb_bitfield[isisindex].bitfield_size;
    }
    bitfield_set(idb_number, &isis_idb_bitfield[isisindex]);

    isisidb = idb->isis_idb[isisindex] = malloc_named(sizeof(isisidbtype),
						      "ISIS IDB");
    if (isisidb) {
	/*
	 * Initialize interface dependent IS-IS parameters. 
	 */
	if (proc_type == PROC_TYPE_ISIS) {
	    isisidb->isis_L1_prio = ISIS_DEF_DR_PRIO;
	    isisidb->isis_L1_cur_prio = ISIS_DEF_DR_PRIO;
	    isisidb->isis_L2_prio = ISIS_DEF_DR_PRIO;
	    isisidb->isis_L1_hello_interval = ISIS_DEF_HELLO_IVL;
	    isisidb->isis_L1_dr_hello_interval = 
		isisidb->isis_L1_hello_interval / ISIS_LAN_DR_HELLO_DIVISOR;
	    isisidb->isis_L2_hello_interval = ISIS_DEF_HELLO_IVL;
	    isisidb->isis_L2_dr_hello_interval = 
		isisidb->isis_L2_hello_interval / ISIS_LAN_DR_HELLO_DIVISOR;
	    for (level_ix = 0; level_ix < ISIS_NUM_LEVELS; level_ix++) {
		isisidb->csnp_ctl[level_ix].csnp_interval =
		    isis_default_csnp_timer(idb, PROC_TYPE_ISIS);
	    }
	    isisidb->pacing_interval = ISIS_DEF_PACING_INTERVAL;
	} else {			/* NLSP */
	    isisidb->isis_L1_prio = IPX_NLSP_PRIO_DEFAULT;
	    isisidb->isis_L1_cur_prio = IPX_NLSP_PRIO_DEFAULT;
	    isisidb->isis_L2_prio = IPX_NLSP_PRIO_DEFAULT;
	    isisidb->isis_L1_hello_interval = 
		IPX_NLSP_HELLO_IVL_DEFAULT * ONESEC;
	    isisidb->isis_L1_dr_hello_interval = 
		isisidb->isis_L1_hello_interval / NLSP_LAN_DR_HELLO_DIVISOR;
	    isisidb->isis_L2_hello_interval = 
		IPX_NLSP_HELLO_IVL_DEFAULT * ONESEC;
	    isisidb->isis_L2_dr_hello_interval = 
		isisidb->isis_L2_hello_interval / NLSP_LAN_DR_HELLO_DIVISOR;
	    for (level_ix = 0; level_ix < ISIS_NUM_LEVELS; level_ix++) {
		isisidb->csnp_ctl[level_ix].csnp_interval =
		    isis_default_csnp_timer(idb, PROC_TYPE_NLSP);
	    }
	    isisidb->pacing_interval = NLSP_MINIMUM_PACING_INTERVAL;
	    isisidb->cfg_pacing_interval = 0;
	}	    
	isisidb->isis_if_number = idb_number;
	isisidb->isis_local_ckt_number = ckt_number + ckt_offset;
	isisidb->isis_L1_hello_multiplier = ISIS_HT_MULTIPLIER;
	isisidb->isis_L2_hello_multiplier = ISIS_HT_MULTIPLIER;
	isisidb->isis_circuit_type = ISIS_CIRCUIT_L1L2;
	isisidb->isis_L1_metric = ISIS_DEF_METRIC;
	isisidb->isis_L2_metric = ISIS_DEF_METRIC;
	isisidb->isis_retrans_interval = ISIS_DEF_RETRANS_TIMER;
	isisidb->isis_L1_pw = NULL;
	isisidb->isis_L2_pw = NULL;
    }
    return(isisidb);
}

/*
 * isis_cleanup_pdb
 *
 * Clean up the PDB.  Can be called by either NLSP or ISIS, and
 * can be called if the PDB is only half-created (if we're cleaning up due
 * to malloc failures).  Note that this is only final cleanup;  some
 * protocol-specific stuff will need to happen elsewhere.
 */
void isis_cleanup_pdb (clns_pdbtype *pdb)
{
    ulong procindex;

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	if (pdb->adjacency_db) {
	    free(pdb->adjacency_db);
	    pdb->adjacency_db = NULL;
	}
    }
    if (pdb->L1_spf_log) {
	free(pdb->L1_spf_log);
	pdb->L1_spf_log = NULL;
    }
    if (pdb->L2_spf_log) {
	free(pdb->L2_spf_log);
	pdb->L2_spf_log = NULL;
    }
    if (pdb->backup_chunks) {
	chunk_destroy(pdb->backup_chunks);
	pdb->backup_chunks = NULL;
    }
    if (pdb->clns_watched_inputq)
	delete_watched_queue(&pdb->clns_watched_inputq);
    if (pdb->watched_running)
	delete_watched_boolean(&pdb->watched_running);
    if (pdb->router_wakeup)
	delete_watched_boolean(&pdb->router_wakeup);
    if (pdb->update_wakeup)
	delete_watched_boolean(&pdb->update_wakeup);
    if (pdb->update_inputq)
	delete_watched_queue(&pdb->update_inputq);
    if (pdb->pname) {
	free(pdb->pname);
	pdb->pname = NULL;
    }
    if (pdb->fastpsnp_cache) {
	free(pdb->fastpsnp_cache);
	pdb->fastpsnp_cache = NULL;
    }
    bitfield_destroy(&pdb->SRM_mask);
    procindex =  isis_pdbindex_to_procindex(pdb->index);
    bitfield_destroy(&isis_idb_bitfield[procindex]);
    bitfield_destroy(&isis_lan_bitfield[procindex]);
    bitfield_destroy(&isis_p2p_bitfield[procindex]);
}


/*
 * isis_pdb_init
 *
 *    Allocate and initialize a pdb for ISIS
 *
 * For NLSP, this will be called only when there is no existing PDB
 * (the existence is gated by the IPX PDB).
 */
clns_pdbtype *isis_pdb_init (char *router_name, isis_proc_type proc_type)
{

    clns_pdbtype *pdb;
    int          typeflag, index, i;

    typeflag = 0;
    index = 0;

    switch (proc_type) {
      case PROC_TYPE_NLSP:
	pdb = malloc_named(sizeof(clns_pdbtype), "NLSP PDB");
	if (!pdb) return(NULL);
	pdb->proc_type = proc_type;
	pdb->adjacency_db = 
	    clns_create_adjacency_db("NLSP adjacency database");
	if (pdb->adjacency_db == NULL)
	    goto malloc_failure;
	pdb->name = "nlsp";
	pdb->family = PDB_NOVELL;
	pdb->errmsg_name = "NLSP";
	pdb->packet_overhead = sizeof(ipxhdrtype);
	pdb->L1_adj_type = CLNS_NLSP_IS_ADJ_TYPE;
	pdb->L1_item_type = ISIS_NLSP_LINK_INFO;
	pdb->def_hello_ivl = IPX_NLSP_HELLO_IVL_DEFAULT * ONESEC;
	pdb->lan_dr_hello_divisor = NLSP_LAN_DR_HELLO_DIVISOR;
	pdb->level = ISIS_CIRCUIT_L1;
	pdb->lsp_lifetime = NOV_NLSP_MAXAGE_DEFAULT * ONESEC;
	pdb->lsp_refresh_interval = NOV_NLSP_MAX_LSP_GEN_DEFAULT * ONESEC;
	pdb->packet_mtu = NOV_NLSP_LSP_MTU_DEFAULT - pdb->packet_overhead;
	pdb->ignore_cksum_errs = TRUE;
	pdb->lsp_rt_frags = ISIS_NLSP_RT_FRAGS_DEFAULT;
	pdb->lsp_svc_frags = ISIS_NLSP_SVC_FRAGS_DEFAULT;
	break;

      case PROC_TYPE_ISIS:
	/*
	 * Delete this code when multiple IS-IS routing processes supported.
	 */
	pdb = find_isis_pdb();
	if (pdb) {
	    printf("\nISIS is already running; tag is %s", pdb->pname ?
		   pdb->pname : "\"\"");
	    return(NULL);
	}

	for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	    if (!strcasecmp(pdb->pname, 
			    strlen(router_name) ? router_name : NULL)) {
		if (!(pdb->typeflag & ISO_ISISMASK)) {
		    printf("\nDuplicate router tag");
		    return(NULL);
		}
		typeflag |= pdb->typeflag;
	    }
	}
	if ((typeflag | ~ISO_ISISMASK) == -1) {
	    return(NULL);
	}
	if (typeflag & ISO_ISIS0) {
	    typeflag = ISO_ISIS1;
	    index = CLNS_PDBINDEX_ISIS1;
	} else {
	    typeflag = ISO_ISIS0;
	    index = CLNS_PDBINDEX_ISIS0;
	}
	pdb = malloc_named(sizeof(clns_pdbtype), "ISIS PDB");
	if (!pdb) return(NULL);
	pdb->proc_type = proc_type;
	pdb->ip_proctype = PROC_ISIS;
	pdb->name = "isis";
	pdb->family = PDB_OSI;
	pdb->errmsg_name = "ISIS";
	pdb->packet_overhead = 0;
	pdb->L1_adj_type = CLNS_L1_IS_ADJ_TYPE;
	pdb->L1_item_type = ISIS_IS_NEIGHBORS;
	pdb->def_hello_ivl = ISIS_DEF_HELLO_IVL;
	pdb->lan_dr_hello_divisor = ISIS_LAN_DR_HELLO_DIVISOR;
	pdb->level = ISIS_CIRCUIT_L1L2;
	pdb->lsp_lifetime = ISIS_MAXAGE_DEFAULT * ONESEC;
	pdb->lsp_refresh_interval = ISIS_MAX_LSP_GEN_DEFAULT * ONESEC;
	pdb->adjacency_db = clns_adjacency_db;
	pdb->packet_mtu = ISIS_LSPBUFFERSIZE - pdb->packet_overhead;
	pdb->stationid_lookup = clns_stationid_lookup;
	pdb->L2_spf_log = malloc_named(sizeof(isis_spf_log), "SPF LOG");
	if (!pdb->L2_spf_log)
	    goto malloc_failure;
	break;

      default:				/* Can't ever happen, hah. */
	printf("\nISIS: Invalid process type!");
	return(NULL);
    }
    pdb->clns_pid = NO_PROCESS;
    pdb->update_pid = NO_PROCESS;
    pdb->backup_chunks = chunk_create(sizeof(isis_backuptype),
				      ISIS_BACKUP_CHUNK_INCR,
				      CHUNK_FLAGS_DYNAMIC,
				      NULL, 0,
				      "ISIS backup");
    if (!pdb->backup_chunks)
	goto malloc_failure;
    pdb->clns_watched_inputq = create_watched_queue("ISIS input queue",
						   0, 0);
    if (!pdb->clns_watched_inputq)
	goto malloc_failure;
    pdb->watched_running = create_watched_boolean("ISIS running", 0);
    if (!pdb->watched_running)
	goto malloc_failure;
    pdb->router_wakeup = create_watched_boolean("ISIS hello wakeup", 0);
    if (!pdb->router_wakeup)
	goto malloc_failure;
    pdb->update_wakeup = create_watched_boolean("ISIS update wakeup", 0);
    if (!pdb->update_wakeup)
	goto malloc_failure;
    pdb->L1_spf_log = malloc_named(sizeof(isis_spf_log), "SPF LOG");
    if (!pdb->L1_spf_log)
	goto malloc_failure;
    pdb->update_inputq = create_watched_queue("ISIS update queue",
					      ISIS_MAX_QUEUE_DEPTH, 0);
    if (!pdb->update_inputq)
	goto malloc_failure;
    pdb->default_packet_mtu = pdb->packet_mtu;
    for (i=0; i<ISIS_MAX_TRAFFIC; i++) {
	isis_traffic[pdb->proc_type][i] = 0;
    }
    pdb->typeflag = typeflag;
    pdb->index = index;
    pdb->redistallowed = typeflag | (ISO_STATIC | DECNET_DISCARD);
    pdb->distance = ISO_ISIS_DIST;
    setstring(&pdb->pname, router_name);

    pdb->changed = FALSE;
    pdb->domain = &pdb->net[1];
    pdb->domain_length = pdb->net[0] - 7;
    pdb->blindtime = ISO_IGRP_BLINDTIME; /* For redistribution */

    if (proc_type == PROC_TYPE_ISIS) {
	enqueue(&clns_protoQ, pdb);
    }

    /*
     * Now fill in the update process fields 
     */
    pdb->L1_changed = FALSE;
    pdb->L2_changed = FALSE;
    pdb->run_L1_spf = FALSE;
    pdb->run_L2_spf = FALSE;
    for (i=0; i<ISIS_NUM_LEVELS; i++) {
	pdb->spf_interval[i] = ISIS_NLSP_SPF_IVL_DEFAULT * ONESEC;
	pdb->prc_interval[i] = ISIS_NLSP_PRC_IVL_DEFAULT * ONESEC;
	pdb->lsp_gen_interval[i] = ISIS_NLSP_LSP_GEN_DEFAULT * ONESEC;
    }
    
    mgd_timer_init_parent(&pdb->timer, NULL);
    mgd_timer_init_parent(&pdb->update_timer, NULL);
    mgd_timer_init_parent(&pdb->lsp_timers, &pdb->update_timer);
    mgd_timer_init_leaf(&pdb->isis_age_timer, &pdb->update_timer,
			ISIS_AGE_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->L1_lsp_build_timer, &pdb->update_timer,
			ISIS_L1_LSP_BUILD_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->L2_lsp_build_timer, &pdb->update_timer,
			ISIS_L2_LSP_BUILD_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->L1_spf_timer, &pdb->update_timer,
			ISIS_L1_SPF_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->L2_spf_timer, &pdb->update_timer,
			ISIS_L2_SPF_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->L1_prc_timer, &pdb->update_timer,
			ISIS_L1_PRC_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&pdb->systemid_dup_timer, &pdb->update_timer,
			ISIS_DUPID_TIMER, NULL, FALSE);
    if (pdb->proc_type == PROC_TYPE_ISIS) {
	mgd_timer_start(&pdb->isis_age_timer, ISIS_AGE_INTERVAL);
	TIMER_START(pdb->checksum_timer, pdb->lsp_refresh_interval);
    } else {
	mgd_timer_start(&pdb->isis_age_timer, pdb->lsp_refresh_interval);
    }

    /*
     * Create the FastPSNP cache. If the malloc fails, we'll just run
     * without FastPSNP until the protocol is reenabled.
     */

    if (!pdb->fastpsnp_cache) {
	pdb->fastpsnp_cache = malloc_named(ISIS_FASTPSNP_CACHESIZE *
		        sizeof(isis_fastpsnp_type), "ISIS fast psnp cache");
    }

    /* Partition avoidance kludge fields. */

    pdb->partition_avoid_on = FALSE;
    pdb->partition_avoid_ok = FALSE;

    return(pdb);

  malloc_failure:
    isis_cleanup_pdb(pdb);
    free(pdb);
    printf(nomemory);
    return(NULL);
}

/*
 * isis_adj_addr
 *
 * Return SNPA for given adjacency to be inserted into an IS-IS IIH for
 * LAN media. For multi-access LANs, the system-id will be inserted. This
 * is so Frame Relay and X.25 designated router elections will work.
 * For media with non-canonical SNPAs, the canonical version is returned.
 *
 * Parameters:
 *  pdb = ISIS/NLSP PDB for which we are doing this operation
 *  idb = IDB on which this SNPA applies
 *  snpa = native SNPA
 *  sysid = System ID associated with this SNPA
 *  buffer = scratch buffer for building canonical SNPA
 *
 * Returns a pointer to the SNPA to use
 */
uchar *isis_adj_addr (clns_pdbtype *pdb, idbtype *idb, uchar *snpa,
		      uchar *sysid, uchar *buffer)
{
    if (idb_is_ieeelike(idb)) {
	/*
	 * Return canonical SNPA address for LANs, except for NLSP....
	 */
	if ((pdb->proc_type != PROC_TYPE_NLSP) && is_tokenring(idb->hwptr)) {
	    ieee_swap(snpa, buffer);
	    return(buffer);
	}
	return(snpa);
    } else {
	/*
	 * Return network layer system-id of adjacency for WANs.
	 */
	station_copy(sysid, buffer);
	return(buffer);
    }
}


/*
 * isis_dr_compare
 *
 * Compares two adjacencies to see which is preferable for being DR.
 * If the first adjacency (the "champ") is NULL, we simply return
 * the second one.
 *
 * Ugly kludge--if the second adjacency is NULL, we use ourselves.
 *
 * Returns the winning adjacency, or NULL if it's ourselves.
 */
static clns_adjtype *isis_dr_compare (clns_pdbtype *pdb, idbtype *idb, 
				      int level,
				      clns_adjtype *champ, 
				      clns_adjtype *challenge)
{
    int champ_pri, challenge_pri;
    idbtype *challenge_idb;
    uchar *challenge_snpa, *challenge_sysid;
    uchar        mac1[STATION_LENGTH], mac2[STATION_LENGTH];
    uchar        *mac1_addr, *mac2_addr;
    isisidbtype  *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)			/* Something's broken */
	return(NULL);

    /* If there's no champ, the challenger wins. */

    if (champ == NULL)
	return(challenge);

    /* Special-case ourselves as the challenger. */

    if (challenge == NULL) {
	challenge_pri = (level == ISIS_CIRCUIT_L1) ? 
	    isisidb->isis_L1_cur_prio : isisidb->isis_L2_prio;
	challenge_idb = idb;
	challenge_snpa = idb->hwptr->hardware;
	challenge_sysid = pdb->systemId;
    } else {
	challenge_pri = (level == ISIS_CIRCUIT_L1) ?
	    challenge->L1_lan_priority : challenge->L2_lan_priority;
	challenge_idb = challenge->idb;
	challenge_snpa = challenge->snpa.addr;
	challenge_sysid = &challenge->key[1];
    }
    champ_pri = (level == ISIS_CIRCUIT_L1) ?
	champ->L1_lan_priority : champ->L2_lan_priority;

    /*
     * Compare priorities.
     */
    if (challenge_pri > champ_pri) {
	return(challenge);		/* A clear winner. */
    } else if (challenge_pri < champ_pri) {
	return(champ);			/* No doubt. */
    } else {
	/*
	 * Priorities are equal - use the higher mac addr.
	 */
	mac1_addr = isis_adj_addr(pdb, champ->idb, champ->snpa.addr, 
				  &champ->key[1], mac1);
	mac2_addr = isis_adj_addr(pdb, challenge_idb, challenge_snpa, 
				  challenge_sysid, mac2);
	if (compare_stationid(mac1_addr, mac2_addr) < 0) {
	    return(challenge);
	}
    }
    return(champ);
}

/*
 * isis_elect_dr
 *
 * Elect a designated router.
 *
 * Returns a pointer to the winning adjacency, or NULL if this system
 * is the winner.  Returns TRUE if there were some adjacencies spotted.
 */
static boolean isis_elect_dr (clns_pdbtype *pdb, clns_adjtype **winner, 
				    idbtype *idb, int level)
{
    clns_adjtype *dr, *adj, *cur_adj;
    int          bucket, level1;
    boolean	 adj_match, adj_spotted;
    int		 adj_type;

    adj_spotted = FALSE;
    dr = NULL;
    adj = NULL;
    bucket = 0;
    level1 = (level == ISIS_CIRCUIT_L1);
    if (level1) {
	adj_type = pdb->L1_adj_type;
    } else {
	adj_type = CLNS_L2_IS_ADJ_TYPE;
    }

    /* First walk all of the adjacencies and find the best of the lot. */

    adj = NULL;
    cur_adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, idb, 0, cur_adj, 
				&bucket))) {
	cur_adj = adj;  /* Save a copy of where we are in the adj table */
	for (; adj; adj = adj->next_alias) {
	    if (adj->state != CLNS_ADJ_UP_STATE)
		continue;
	    if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
		continue;
	    if (adj->adjacencyType == CLNS_ES_ADJ_TYPE ||
		adj->adjacencyType == CLNS_IS_ADJ_TYPE) {
		if (level1)
		    adj_spotted = TRUE;
		continue;
	    }
	    adj_match = (adj->adjacencyType == adj_type);
	    if (pdb->proc_type == PROC_TYPE_ISIS)
		adj_match = adj_match || 
		    ((adj_type == CLNS_L1_IS_ADJ_TYPE) && !adj->L2only);
	    
	    if (adj_match) {

		/* Got the right type.  Let's compare 'em. */		

		dr = isis_dr_compare(pdb, idb, level, dr, adj);
		adj_spotted = TRUE;
	    }
	}
    }

    /*
     * Found the best of the adjacencies.  If there weren't any found,
     * we're the clear winner.  Otherwise, compare ourselves to the
     * previous winner.
     */
    if (dr != NULL) {			/* There was a winner */
	dr = isis_dr_compare(pdb, idb, level, dr, NULL);
    }
    *winner = dr;
    return(adj_spotted);
}

/*
 * isis_rw_purge_lsp
 *
 *    This routine is called at the bottom of a recursive walk.
 *    It will force the expiration of the specified lsps
 */
boolean isis_rw_purge_lsp (isis_tree_type *element, void *level_parm,
			   void *isis_pdb, void *lifetime_parm,
			   void *dummy4)
{
    isis_lspdbtype *lsp_db;
    clns_pdbtype *pdb;
    ulong *level;
    ulong *zero_age_lifetime;

    pdb = isis_pdb;
    level = level_parm;
    zero_age_lifetime = lifetime_parm;

    lsp_db = element->lsp;
    if (lsp_db && lsp_db->lsp_active){	/* Don't purge purged LSPs! */
 	isis_purge_lsp(pdb, lsp_db, *level, *zero_age_lifetime);
    }
    return(TRUE);
}


/*
 * isis_purge_dr
 *
 * Purge the DR LSPs from the LSP database.  We should do this only if
 * we are either becoming or resigning DR.
 *
 * If the pseudonode number is zero, this means that the DR isn't fully
 * elected yet (the other guy hasn't told us his pseudonode ID) so in
 * that case we just punt.
 */
void isis_purge_dr (int level, clns_pdbtype *pdb, isisidbtype *isisidb)
{
    isis_lspid link_id;
    isis_tree_type *lsp_tree;
    uchar *dr_id;

    /* First find the lsp id for the pseudo-node advertisement. */

    link_id.lsp_number = 0;
    if (level == ISIS_CIRCUIT_L1) {
	dr_id = isisidb->isis_L1_dr_id;
	lsp_tree = pdb->level1_LSPs;
    } else {
	dr_id = isisidb->isis_L2_dr_id;
	lsp_tree = pdb->level2_LSPs;
    }
    systemid_copy(dr_id, link_id.system_id);
    if (!link_id.pseudo_id) {
	adj_pbuginf(pdb, "Didn't purge DR LSP--not fully elected");
	return;
    }
    isis_recursive_walk(&link_id, ISIS_WALK_MATCH_SYSTEM, lsp_tree,
			isis_rw_purge_lsp, &level, pdb,
			&pdb->lsp_lifetime, NULL);
}

/*
 * isis_new_dr
 *
 * Set things up for a new DR.
 * dr = DR adjacency, or NULL if we are the new DR
 */
static void isis_new_dr (clns_pdbtype *pdb,
			 idbtype *idb,
			 int level,
			 clns_adjtype *dr)
{

    int     type;
    boolean its_us, it_was_us;
    uchar   old_id[SYSTEMID_LENGTH];
    uchar   *station_id;
    isisidbtype *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (dr) {
	station_id = &dr->key[1];
	its_us = FALSE;
    } else {
	station_id = pdb->systemId;
	its_us = TRUE;
    }

    adj_pbuginf(pdb, "New level %d DR %s on %s", level, 
		station_string(station_id), idb->namestring);

    /*
     * Save identity of old DR. 
     */
    if (level == ISIS_CIRCUIT_L1) {
	type = pdb->L1_adj_type;
	systemid_copy(isisidb->isis_L1_dr_id, old_id);
    } else {
	type = CLNS_L2_IS_ADJ_TYPE;
	systemid_copy(isisidb->isis_L2_dr_id, old_id);
    }

    it_was_us = (station_match(old_id, pdb->systemId));

    /*
     * Purge the old DR LSP if either the old or new DR is us and
     * the identity changed.
     */
    if (!station_match(old_id, station_id) && (its_us || it_was_us)) 
	isis_purge_dr(level, pdb, isisidb);

    /* %%% One day delete the old DR from our non-dr LSP. */

    /*
     * Fill in the new LAN id and dr fields and insert the new advertisement.
     * If we're elected, set the flag to generate our pseudonode LSP.
     * For NLSP, trigger injection of waiting backup routes.
     */
    if (level == ISIS_CIRCUIT_L1) {
  	if (its_us) {
	    systemid_copy(pdb->systemId, isisidb->isis_L1_circuit_id);
	    isisidb->isis_L1_circuit_id[ISIS_PSEUDO_ID] =
		isisidb->isis_local_ckt_number;
	    systemid_copy(isisidb->isis_L1_circuit_id, isisidb->isis_L1_dr_id);
	    isis_mark_L1_dr_changed(pdb, isisidb);
	    if (!it_was_us && pdb->proc_type == PROC_TYPE_NLSP) {
		pdb->inject_backups = TRUE;
	    }
  	} else {
	    /*
	     * If the other guy hasn't elected himself yet, get the DR ID
	     * from his system ID (but we won't have the pseudonode number).
	     * If he has elected himself, get it from the lan ID instead.
	     */
	    if (station_match(station_id, dr->L1_circuit_id)) {
		systemid_copy(dr->L1_circuit_id, isisidb->isis_L1_dr_id);
	    } else {
		systemid_copy(station_id, isisidb->isis_L1_dr_id);
		adj_pbuginf(pdb, "DR hasn't elected itself");
	    }
	    systemid_copy(dr->L1_circuit_id, isisidb->isis_L1_circuit_id);
	}

	/* %%% One day add the DR to our non-DR LSP here. */

	isis_mark_L1_changed(pdb);

    } else {
  	if (its_us) {
	    systemid_copy(pdb->systemId, isisidb->isis_L2_circuit_id);
	    isisidb->isis_L2_circuit_id[ISIS_PSEUDO_ID] =
		isisidb->isis_local_ckt_number;
	    systemid_copy(isisidb->isis_L2_circuit_id, isisidb->isis_L2_dr_id);
	    isis_mark_L2_dr_changed(pdb, isisidb);
  	} else {
	    if (station_match(station_id, dr->L2_circuit_id)) {
		systemid_copy(dr->L2_circuit_id, isisidb->isis_L2_dr_id);
	    } else {
		systemid_copy(station_id, isisidb->isis_L2_dr_id);
		adj_pbuginf(pdb, "DR hasn't elected itself");
	    }
	    systemid_copy(dr->L2_circuit_id, isisidb->isis_L2_circuit_id);
	}

	/* %%% One day add the DR to our non-DR LSP here. */

	isis_mark_L2_changed(pdb);
    }

    /*
     * Always send out a new hello after a new designated router is 
     * elected.
     */
    isis_schedule_hello(pdb, isisidb, level);
}

/*
 * isis_compute_L1_dr
 *
 *    Compute the level 1 designated router
 *
 *    First check if the timer has expired. If not, just return.
 *    Walk through the adjacency database looking for 'up' adjacencies.
 *    If there are none, don't claim to be 'designated' yourself.
 *
 *    From the 'up' adjacencies, choose the highest priority. If there is
 *    more than one, break ties with highest mac layer address.
 */
static void isis_compute_L1_dr (clns_pdbtype *pdb, idbtype *idb)
{
    clns_adjtype *dr;
    isis_lspdbtype *lsp;
    isis_lspid lspid;
    boolean	 adj_spotted;
    isisidbtype  *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (is_p2p(idb))
	return;

    adj_pbuginf(pdb, "Run level 1 DR election for %s", idb->namestring);
    isis_traffic[pdb->proc_type][ISIS_L1_DR_ELECTS]++;

    adj_spotted = isis_elect_dr(pdb, &dr, idb, ISIS_CIRCUIT_L1);

    /*
     * This takes care of the case where we have no adjacencies on this
     * interface yet.
     */
    if (pdb->proc_type == PROC_TYPE_ISIS && !adj_spotted) {
	adj_pbuginf(pdb, "No adjacencies found");
	return;
    }

    if (dr == NULL) {			/* We're DR */

	/* For NLSP, bump the DR priority so we will tend to stay DR. */

	if (pdb->proc_type == PROC_TYPE_NLSP) {
	    if (isisidb->isis_L1_prio <= ISIS_MAX_PRIO - NLSP_DR_PRIO_RAISE) {
		isisidb->isis_L1_cur_prio = 
		    isisidb->isis_L1_prio + NLSP_DR_PRIO_RAISE;
	    } else {
		isisidb->isis_L1_cur_prio = ISIS_MAX_PRIO;
	    }
	}

	/*
	 * If we were DR before and a non-dummy DR LSP is present, we're done.
	 */
	station_copy(pdb->systemId, lspid.system_id);
	lspid.pseudo_id = isisidb->isis_local_ckt_number;
	lspid.lsp_number = 0;
	if (isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L1)) {
	    lsp = isis_find_L1_lsp(pdb, &lspid);
	    if (lsp && lsp->lsp_active && !isis_dummy_lsp(lsp)) {
		adj_pbuginf(pdb, "No change (it's us)");
		return;
	    }
	}
    } else {

	/*
	 * If neither the DR nor the DR's idea of the circuit ID changed,
	 * we're done.
	 */
	if (isis_system_is_dr(pdb, &dr->key[1], isisidb, ISIS_CIRCUIT_L1) &&
	    systemid_match(isisidb->isis_L1_circuit_id, dr->L1_circuit_id)) {
	    adj_pbuginf(pdb, "No change");
	    return;
	}

	/* For NLSP, set the priority back down, in case we were DR. */
	
	if (pdb->proc_type == PROC_TYPE_NLSP) {
	    isisidb->isis_L1_cur_prio = isisidb->isis_L1_prio;
	}
    }
    isis_new_dr(pdb, idb, ISIS_CIRCUIT_L1, dr);
}


/*
 * isis_compute_L2_dr
 *
 */
static void isis_compute_L2_dr (clns_pdbtype *pdb, idbtype *idb)
{

    clns_adjtype *dr;
    isis_lspdbtype *lsp;
    isis_lspid   lspid;
    boolean      adj_spotted;
    isisidbtype  *isisidb;
  
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (is_p2p(idb)) 
	return;

    adj_pbuginf(pdb, "Run level 2 DR election for %s", idb->namestring);
    isis_traffic[pdb->proc_type][ISIS_L2_DR_ELECTS]++;

    adj_spotted = isis_elect_dr(pdb, &dr, idb, ISIS_CIRCUIT_L2);
    if (pdb->proc_type == PROC_TYPE_ISIS && !adj_spotted) {
	adj_pbuginf(pdb, "No adjacencies found");
	return;
    }

    if (dr == NULL) {			/* We're DR */

	/*
	 * If we were DR before and a non-dummy DR LSP is present, we're done.
	 */
	station_copy(pdb->systemId, lspid.system_id);
	lspid.pseudo_id = isisidb->isis_local_ckt_number;
	lspid.lsp_number = 0;
	if (isis_we_are_dr(pdb, idb, ISIS_CIRCUIT_L2)) {
	    lsp = isis_find_L2_lsp(pdb, &lspid);
    	    if (lsp && lsp->lsp_active && !isis_dummy_lsp(lsp)) {
		adj_pbuginf(pdb, "No change (it's us)");
		return;
	    }
	}

    } else {

	/*
	 * If neither the DR nor the DR's idea of the circuit ID changed,
	 * we're done.
	 */
	if (isis_system_is_dr(pdb, &dr->key[1], isisidb, ISIS_CIRCUIT_L2) &&
	    systemid_match(isisidb->isis_L2_circuit_id, dr->L2_circuit_id)) {
	    adj_pbuginf(pdb, "No change");
	    return;
	}
    }
    isis_new_dr(pdb, idb, ISIS_CIRCUIT_L2, dr);
}

/*
 * isis_send_packet
 *
 * Send out a packet.  Does proper encapsulation and disposes of the
 * packet properly.
 *
 * Returns an indication as to the success of the encapsulation.
 */
boolean isis_send_packet (clns_pdbtype *pdb, paktype *pak, ulong length,
			  idbtype *idb, hwaddrtype *snpa)
{
    boolean result;

    /*
     * If we are in adjacency holddown, toss all outgoing packets.
     * Cheap, but effective.
     */
    if (TIMER_RUNNING_AND_SLEEPING(pdb->adj_holddown_timer)) {
	datagram_done(pak);
	return(TRUE);
    }

    /*
     * Encapsulate the packet and send it out. 
     */
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	result = reg_invoke_ipx_nlsp_ipx_send_pkt(pdb, pak, length, idb);
    } else {
	result = isis_encaps(pak, length, idb, snpa);
	if (result) {
	    datagram_out(pak);
	} else {
	    datagram_done(pak);
	}
    }
    return(result);
}

/*
 * isis_send_serial_iih
 *
 */
static void isis_send_serial_iih (clns_pdbtype *pdb, idbtype *idb) 
{
    paktype             *pak;
    isis_pt_IIH_hdrtype *isis;
    isis_info_type      *item;
    uchar               *area_address;
    int                 ht, current_length;
    int                 nbytes, i;
    uchar      		*pak_start;
    boolean		result;
    nidbtype		*nidb;
    isisidbtype         *isisidb;
    uchar               minor_version;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (!isisidb->isis_interface_up)
	return;

    /*
     * Allocate and send a pt to pt IIH out this serial interface.
     * The -1 is to get max sized IIHes out DS3 interfaces. The fudge
     * byte puts the total frame size over idb->maxdgram.
     */
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	nbytes = idb->hwptr->maxmtu;
	minor_version = NLSP_VERSION;
    } else {
	nbytes = idb->clns_mtu - 1;
	minor_version = ISIS_VERSION;
    }
    pak = getbuffer(nbytes + pdb->packet_overhead);
    if (!pak)
	return;
    pak->flags |= PAK_PRIORITY;

    isis = isis_pak_start(pak, pdb);
    pak_start = (uchar *) isis;
    memset(pak_start, 0, nbytes);

    isis->pid = NLP_ISIS;
    isis->hlen = ISIS_PT_HELLO_HEADER_SIZE;
    isis->version_pid = minor_version;
    isis->id_length = 0;
    isis->type = ISIS_PT_HELLO;
    isis->version_pdu = ISIS_VERSION;
    isis->max_area_addresses = 0;
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	nidb = idb->primary_nidb;
	if (nidb == NULL) {
	    retbuffer(pak);
	    return;
	}
	isis->state = isisidb->isis_if_state;
    }
    isis->circuit_type = isisidb->isis_circuit_type & pdb->level;
    isis->local_circuit_id = isisidb->isis_local_ckt_number &
	ISIS_P2P_CKT_MASK;

    station_copy(pdb->systemId, isis->source_id);

    ht = isisidb->isis_L1_hello_interval * isisidb->isis_L1_hello_multiplier;
    PUTSHORT(isis->ht, ht/ONESEC);
    item = (isis_info_type *)isis->item;
    
    if (pdb->proc_type == PROC_TYPE_ISIS) {
	/*
	 * Insert the interface state in the hello.
	 */
	item->code = ISIS_SERIAL_CIRC_STATE;
	item->length = ISIS_SERIAL_CIRC_STATE_LENGTH;
	item->value[0] = isisidb->isis_if_state;
	ADVANCE_ITEM(item, FALSE);

	/*
	 * Insert authentication info, if any.
	 */
	current_length = (uchar *)item - pak_start;
	item = isis_insert_auth(item, NULL, isisidb, 
				(isis->circuit_type == ISIS_CIRCUIT_L1L2) ? 
				ISIS_CIRCUIT_L1 : isis->circuit_type,
				nbytes - current_length);
	
	/*
	 * Insert NLPID in packet.
	 */
	current_length = (uchar *)item - pak_start;
	isis_insert_nlpid(pdb, item, nbytes - current_length);
	ADVANCE_ITEM(item, FALSE);
	
	/* 
	 * Fill in the manual area addresses here. 
	 */ 
	item->code = ISIS_AREA_ADDRESSES;
	area_address = (uchar *)&item->value;
	for (i = 0; i < pdb->num_areas; i++) {
	    area_copy(pdb->area_address[i], *(NSAP_T *)area_address);
	    area_address += (area_address[0] + 1);
	}
	item->length = isis_item_length(item, area_address);
	ADVANCE_ITEM(item, FALSE);
	
	/*
	 * Fill in IP addresses, if any.
	 */
	current_length = (uchar *)item - pak_start;
	if (pdb->ip_pdb) {
	    reg_invoke_isis_insert_ip_if_address(pdb, item, idb,
						 isisidb->isis_circuit_type,
						 nbytes - current_length);
	    ADVANCE_ITEM(item, FALSE);
	}
    }

    /*
     * Fill in IPX information, if any.
     */
    if (pdb->ipx_pdb) {
	current_length = (uchar *)item - pak_start;
	item = reg_invoke_ipx_isis_insert_ipx_hello(pdb, item, idb,
						    nbytes - current_length);
    }

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	/*
	 * Pad out the rest of the packet.
	 */
	/*
	 * Note that 'nbytes' is the largest possible packet on this link.
	 * This is to guarantee that the adjacency will only come up if the
	 * largest packet size is actually acceptable to both sides.
	 *
	 * NOTE: in the case of x.25, this needs to be the maximum of
	 *   maximum packet size for this link
	 *   L1LSP buffer size
	 *   L2LSP buffer size
	 *  (see the spec)
	 * Since we won't sent out hellos on x.25, I am ignoring this now.
	 */
	current_length = (uchar *)item - pak_start;
	if ((nbytes - current_length) >= ISIS_INFO_TYPE_OVERHEAD) {
	    while ((nbytes - current_length) > 
		   (ISIS_INFO_TYPE_MAXDATA + ISIS_INFO_TYPE_OVERHEAD)) {
		item->code = ISIS_PAD;
		current_length += 
		    ISIS_INFO_TYPE_MAXDATA + ISIS_INFO_TYPE_OVERHEAD;
		item->length = ISIS_INFO_TYPE_MAXDATA;
		ADVANCE_ITEM(item, TRUE);
	    }
	    if ((nbytes - current_length) >= ISIS_INFO_TYPE_OVERHEAD && 
		nbytes != current_length) {
		item->code = ISIS_PAD;
		item->length = nbytes - current_length - 
		    ISIS_INFO_TYPE_OVERHEAD;
		current_length += (item->length + ISIS_INFO_TYPE_OVERHEAD);
		ADVANCE_ITEM(item, TRUE);
	    }
	}
    }
    current_length = (uchar *) item - pak_start;
    PUTSHORT(isis->pdu_length, current_length);

    result = isis_send_packet(pdb, pak, current_length, idb, &all_iss);
    if (result) {
        isis_traffic[pdb->proc_type][ISIS_PTP_HELLO_OUT]++;
	isisidb->isisidb_traffic[ISISIDB_PTP_HELLO_OUT]++;
        adj_pbuginf(pdb, "Sending serial IIH on %s", 
		    idb->namestring);
    } else {
	if (!is_x25(idb->hwptr)) {
	    adj_pbuginf(pdb, "Encapsulation failed on serial IIH (%s)",
			idb->namestring);
	}
    }
}

/*
 * isis_send_lan_iih
 *
 * Send a LAN Hello packet. Supports either level-1 or level-2.
 */
static void isis_send_lan_iih (clns_pdbtype *pdb, idbtype *idb, int level)
{
    paktype              *pak;
    isis_lan_IIH_hdrtype *isis;
    isis_info_type       *item;
    uchar                *area_address;
    uchar                *neighbor_id;
    clns_adjtype         *adj, *cur_adj;
    hwaddrtype           snpa;
    int                  nbytes, i, current_length, ht, bucket, tlv_length;
    boolean              dr;
    uchar                addr[IEEEBYTES];
    uchar		 *pak_start;
    boolean		 result;
    ulong		 interval;
    ulong		 multiplier;
    isisidbtype		 *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    if (!isisidb->isis_interface_up)
	return;

    /*
     * Allocate the packet.
     */
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	nbytes = pdb->packet_mtu;
    } else {
	nbytes = idb->clns_mtu;
    }
    pak = getbuffer(nbytes + pdb->packet_overhead);
    if (!pak)
	return;
    pak->flags |= PAK_PRIORITY;

    isis = isis_pak_start(pak, pdb);
    pak_start = (uchar *) isis;
    memset(pak_start, 0, nbytes);

    isis->pid = NLP_ISIS;
    isis->hlen = ISIS_LAN_HELLO_HEADER_SIZE;
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis->version_pid = NLSP_VERSION;
    } else {
	isis->version_pid = ISIS_VERSION;
    }
    isis->version_pdu = ISIS_VERSION;
    isis->id_length = 0;
    memset(&snpa, 0, sizeof(snpa)); /* So address is zero */
    snpa.type = STATION_ILLEGAL;
    snpa.length = 0;

    /*
     * Set up level dependent fields.
     */
    if (level == ISIS_CIRCUIT_L1) {
	isis->type = ISIS_LAN_L1_HELLO;
	isis->prio = isisidb->isis_L1_cur_prio;
	snpa.target_type = TARGET_OSI_ALL_L1;
	systemid_copy(isisidb->isis_L1_circuit_id, isis->lan_id);
 	dr = isis_we_are_dr(pdb, idb, CLNS_LEVEL1);
    } else {
	isis->type = ISIS_LAN_L2_HELLO;
	isis->prio = isisidb->isis_L2_prio;
	snpa.target_type = TARGET_OSI_ALL_L2;
	systemid_copy(isisidb->isis_L2_circuit_id, isis->lan_id);
 	dr = isis_we_are_dr(pdb, idb, CLNS_LEVEL2);
    }

    /* 
     * Finish off fixed fields.
     */
    isis->max_area_addresses = 0;
    isis->circuit_type = isisidb->isis_circuit_type & pdb->level;

    station_copy(pdb->systemId, isis->source_id);

    multiplier = isisidb->isis_L1_hello_multiplier;
    if (dr) {
	if (level == ISIS_CIRCUIT_L1) {
	    interval = isisidb->isis_L1_dr_hello_interval;
	} else {
	    interval = isisidb->isis_L2_dr_hello_interval;
	    multiplier = isisidb->isis_L2_hello_multiplier;
	}
    } else {
	if (level == ISIS_CIRCUIT_L1) {
	    interval = isisidb->isis_L1_hello_interval;
	} else {
	    interval = isisidb->isis_L2_hello_interval;
	    multiplier = isisidb->isis_L2_hello_multiplier;
	}
    }
    ht = interval * multiplier;
    PUTSHORT(isis->ht, ht/ONESEC);
    item = (isis_info_type *)isis->item;

    /* Do NLSP- and ISIS-specific stuff. */

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	isis->no_multicast = TRUE;

    } else {

	/*
	 * Insert authentication info, if any.
	 */
	current_length = (uchar *)item - pak_start;
	item = isis_insert_auth(item, NULL, isisidb, level,
				nbytes - current_length);
	
	/*
	 * Insert NLPID in packet.
	 */
	current_length = (uchar *)item - pak_start;
	isis_insert_nlpid(pdb, item, nbytes - current_length);
	ADVANCE_ITEM(item, FALSE);
	
	/*
	 * Fill in the manual area addresses here.
	 */
	item->code = ISIS_AREA_ADDRESSES;
	area_address = (uchar *)&item->value;
	for (i = 0; i < pdb->num_areas; i++) {
	    area_copy(pdb->area_address[i], *(NSAP_T *)area_address);
	    area_address += (area_address[0] + 1);
	}
	item->length = isis_item_length(item, area_address);
	ADVANCE_ITEM(item, FALSE);
	
	/*
	 * Fill in IP addresses, if any.
	 */
	if (pdb->ip_pdb) {
	    current_length = (uchar *)item - pak_start;
	    reg_invoke_isis_insert_ip_if_address (pdb, item, idb, level,
						  nbytes - current_length);
	}
	ADVANCE_ITEM(item, FALSE);
    }

    /*
     * Fill in IPX information, if any.
     */
    if (pdb->ipx_pdb) {
	current_length = (uchar *)item - pak_start;
	item = reg_invoke_ipx_isis_insert_ipx_hello(pdb, item, idb,
						    nbytes - current_length);
    }

    /*
     * Fill in the known adjacencies here.
     */
    item->code = ISIS_IS_NEIGHBORS_IIH;
    neighbor_id = (uchar *)&item->value;
    current_length = (uchar *)item - pak_start;
    tlv_length = 0;
    adj = NULL;
    cur_adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, idb, 0, cur_adj, 
				&bucket))) {
	cur_adj = adj;  /* Save a copy of where we are in the adj table */
	for (; adj; adj = adj->next_alias) {	
	    /*
	     * Terminate the TLV and start another one if we're about to
	     * fill this one.
	     */
	    if (ISIS_INFO_TYPE_MAXDATA - tlv_length < STATION_LENGTH) {
		item->length = isis_item_length(item, neighbor_id);
		ADVANCE_ITEM(item, TRUE);
		item->code = ISIS_IS_NEIGHBORS_IIH;
		neighbor_id = (uchar *)&item->value;
		current_length = (uchar *)item - pak_start;
		tlv_length = 0;
	    }
	    if (nbytes - current_length < STATION_LENGTH)
		goto filled_pdu;
	    if (pdb->proc_type == PROC_TYPE_NLSP) {
		if (adj->adjacencyType != CLNS_NLSP_IS_ADJ_TYPE)
		    continue;
	    } else {
		if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) {
		    if (level == ISIS_CIRCUIT_L1 && adj->L2only) continue;
		} else if (adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE) {
		    if (level == ISIS_CIRCUIT_L2) continue;
		} else {
		    continue;
		}
	    }
	    if (TIMER_RUNNING_AND_AWAKE(adj->expiration_time))
		continue;
	    if (adj->state == CLNS_ADJ_INIT_STATE || 
		adj->state == CLNS_ADJ_UP_STATE) {
		station_copy(isis_adj_addr(pdb, adj->idb, adj->snpa.addr, 
					   &adj->key[1], addr), neighbor_id);
		neighbor_id += STATION_LENGTH;
		current_length += STATION_LENGTH;
		tlv_length += STATION_LENGTH;
	    }
	}
    }
  filled_pdu:
    item->length = isis_item_length(item, neighbor_id);
    ADVANCE_ITEM(item, FALSE);

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	/*
	 * Pad out the rest of the packet. Note that each pad item may 
	 * be only 255 bytes long.
	 */
	current_length = (uchar *)item - pak_start;
	if ((nbytes - current_length) >= ISIS_INFO_TYPE_OVERHEAD) {
	    while ((nbytes - current_length) > 
		   ISIS_INFO_TYPE_OVERHEAD + ISIS_INFO_TYPE_MAXDATA) {
		item->code = ISIS_PAD;
		current_length += 
		    ISIS_INFO_TYPE_OVERHEAD + ISIS_INFO_TYPE_MAXDATA;
		item->length = ISIS_INFO_TYPE_MAXDATA;
		ADVANCE_ITEM(item, TRUE);	/* Length guaranteed nonzero */
	    }
	    if ((nbytes - current_length) > ISIS_INFO_TYPE_OVERHEAD) {
		item->code = ISIS_PAD;
		item->length = nbytes - current_length - 
		    ISIS_INFO_TYPE_OVERHEAD;
		current_length += (item->length + ISIS_INFO_TYPE_OVERHEAD);
		ADVANCE_ITEM(item, TRUE);	/* Length guaranteed nonzero */
	    }
	}
    }
    current_length = (uchar *)item - pak_start;
    PUTSHORT(isis->pdu_length, current_length);

    result = isis_send_packet(pdb, pak, current_length, idb, &snpa);
    if (result) {
	isis_traffic[pdb->proc_type]
	    [(level == ISIS_CIRCUIT_L1) ? 
	     ISIS_L1_HELLO_OUT : ISIS_L2_HELLO_OUT]++;
        isisidb->isisidb_traffic[(level == ISIS_CIRCUIT_L1) ?
				 ISISIDB_L1_HELLO_OUT :
				 ISISIDB_L2_HELLO_OUT]++;
	adj_pbuginf(pdb, "Sending L%d IIH on %s", level, 
		    idb->namestring);
    } else {
	adj_pbuginf(pdb, "Encapsulation failed for level %d IIH on %s",
		    level, idb->namestring);
    }
}

/*
 * isis_schedule_hello
 *
 * Schedule a hello to be sent as soon as possible.  This is either
 * immediately, or after the one second holddown period has expired.
 */
void isis_schedule_hello (clns_pdbtype *pdb, isisidbtype *isisidb, int level)
{
    sys_timestamp last_one;
    mgd_timer *timer;
    int delay;

    /* If we're not all quite there yet, bail. */

    if (!pdb || !isisidb)
	return;

    /* If we're not running on the interface, forget it. */

    if (!isisidb->isis_running)
	return;

    /* Pick the right info, depending on the level. */

    if (level == ISIS_CIRCUIT_L1) {
	last_one = isisidb->isis_last_hello;
	timer = &isisidb->isis_hello_timer;
    } else {
	last_one = isisidb->isis_last2_hello;
	timer = &isisidb->isis_hello2_timer;
    }

    /*
     * If we're in holddown, schedule the hello for later;  otherwise,
     * do it now.
     */
    delay = 0;
    if (CLOCK_IN_INTERVAL(last_one, ONESEC)) {
	delay = ONESEC - ELAPSED_TIME(last_one);
	if (delay < 0)
	    delay = 0;
    }
    mgd_timer_start(timer, delay);
}

/*
 * isis_send_iih
 *
 *    Send out one IIH out the specified interface. Don't send it
 *    if the last one was less than one second ago.  Don't send it
 *    if the partition avoidance kludge is enabled.
 */
void isis_send_iih (clns_pdbtype *pdb, idbtype *idb, int level)
{
    int serial;
    isisidbtype *isisidb;

    if (pdb->proc_type == PROC_TYPE_ISIS && (!pdb->net[0]))
	return;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;
    
    if (pdb->partition_avoid_ok)
	return;

    serial = is_p2p(idb);

    if (serial || (level == ISIS_CIRCUIT_L1)) {

	/*
	 * If we sent one in the last second, reschedule it.
	 */
	if (CLOCK_IN_INTERVAL(isisidb->isis_last_hello, ONESEC)) {
	    isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
	    return;
	}
	GET_TIMESTAMP(isisidb->isis_last_hello);
    } else {
	if (serial)
	    return;
	/*
	 * If we sent one in the last second, reschedule it.
	 */
	if (CLOCK_IN_INTERVAL(isisidb->isis_last2_hello, ONESEC)) {
	    isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L2);
	    return;
	}
	GET_TIMESTAMP(isisidb->isis_last2_hello);
    }

    /* 
     * Send the appropriate IIH - serial, LAN level1 or LAN level2. 
     */
    if (serial) {
	isis_send_serial_iih(pdb, idb);
    } else {
	if ((pdb->level & level) && 
	    (isisidb->isis_circuit_type & level)) {
	    isis_send_lan_iih(pdb, idb, level);
	}
    }
}

/*
 * isis_router
 *
 * Main ISIS routing process (actually the Hello process, but whatever.)
 *
 */
static process isis_router (void)
{
    idbqueuetype *idbqueue;
    idbtype      *idb;
    paktype      *pak;
    isishdrtype  *isis;
    isisidbtype  *isisidb;
    mgd_timer    *expired_timer;
    clns_adjtype *adj;
    clns_pdbtype *pdb;
    ulong major, minor;

    /* Initialization gorp. */

    if (!process_get_arg_ptr((void **) &pdb))
	process_kill(THIS_PROCESS);
    memory_new_owner(pdb);

    process_wait_on_system_init();

    /* Set up the event predicates. */

    process_watch_queue(pdb->clns_watched_inputq, ENABLE, RECURRING);
    process_watch_mgd_timer(&pdb->timer, ENABLE);
    process_watch_boolean(pdb->router_wakeup, ENABLE, RECURRING);

    while (TRUE) {

	process_wait_for_event();

	/* If the "running" flag is clear, wedge the process. */

	if (!clns_pdb_running(pdb)) {
	    process_push_event_list(NULL); /* Save the state. */
	    process_watch_boolean(pdb->watched_running, ENABLE, ONE_SHOT);
	    process_wait_for_event();	/* We'll wake up when it's time. */
	    process_watch_boolean(pdb->watched_running, DISABLE, ONE_SHOT);
	    process_pop_event_list(NULL);
	}

	/* Empty the event queue.  We process events in our own order. */

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
	      case QUEUE_EVENT:
	      case BOOLEAN_EVENT:
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
	process_set_boolean(pdb->router_wakeup, FALSE);

	/* Zap the adjacency holddown if it has expired. */

	if (TIMER_RUNNING_AND_AWAKE(pdb->adj_holddown_timer)) {
	    TIMER_STOP(pdb->adj_holddown_timer);
	}

	/* 
	 * Process the packets which have come in.  Drop all incoming
	 * packets if we're in adjacency holddown.
	 */
	pak = process_dequeue(pdb->clns_watched_inputq);
	while (pak) {
	    process_may_suspend();
	    idb = pak->if_input;
	    clear_if_input(pak);	/* Free the input queue slot. */
	    pak->if_input = idb;	/* Hack, but a reasonable one. */
	    isisidb = get_isis_idb(pdb, idb);
	    if (idb && isisidb && isisidb->isis_running &&
		!TIMER_RUNNING_AND_SLEEPING(pdb->adj_holddown_timer)) {
		isis = isis_pak_start(pak, pdb);
		
		switch (isis->type) {
		  case ISIS_PT_HELLO:
		    isis_traffic[pdb->proc_type][ISIS_PTP_HELLO_IN]++;
		    isis_rcv_serial_iih(pak, pdb, pak->if_input, isis);
		    break;
		  case ISIS_LAN_L1_HELLO:
		    isis_traffic[pdb->proc_type][ISIS_L1_HELLO_IN]++;
		    isis_rcv_lan_iih(pak, pdb, 
				     pak->if_input, isis, ISIS_CIRCUIT_L1);
		    break;
		  case ISIS_LAN_L2_HELLO:
		    isis_traffic[pdb->proc_type][ISIS_L2_HELLO_IN]++;
		    isis_rcv_lan_iih(pak, pdb, 
				     pak->if_input, isis, ISIS_CIRCUIT_L2);
		    break;
		    
		  case ISIS_L1_LSP:
		  case ISIS_L2_LSP:
		  case ISIS_L1_CSNP:
		  case ISIS_L2_CSNP:
		  case ISIS_L1_PSNP:
		  case ISIS_L2_PSNP:

		    /*
		     * Enqueue the packet into the secondary queue if there's
		     * enough room to hold it.
		     */
		    if (!process_is_queue_full(pdb->update_inputq)) {
			process_enqueue(pdb->update_inputq, pak);
		    } else {
			datagram_done(pak);
			pdb->input_drops++;
		    }
		    pak = process_dequeue(pdb->clns_watched_inputq);
		    continue;
		    
		  default:
		    update_pbuginf(pdb, "unrecognized packet type: %d", 
				   isis->type);
		    break;
		} 
	    }
	    datagram_done(pak);
	    pak = process_dequeue(pdb->clns_watched_inputq);
	}

	/*
	 * Do any requested DR elections.
	 */
	if (pdb->run_dr_election) {
	    pdb->run_dr_election = FALSE;
	    for (idbqueue = pdb->idbqueue; idbqueue;
		 idbqueue = idbqueue->next) {
		process_may_suspend();
		idb = idbqueue->idb;
		
		/* Skip all this if NLSP isn't running (RIP-only WAN i/f's) */
		
		isisidb = get_isis_idb(pdb, idb);
		if (!isisidb || !isisidb->isis_running)
		    continue;
		
		/*
		 * Various events will cause us to re-compute the designated 
		 * router. In particular, changes in priority and learning
		 * about new ISes may cause this flag to be set.
		 * We will re-compute the designated router BEFORE sending
		 * out hellos so that the new information is included 
		 * in the hellos.
		 */
		if (isisidb->isis_compute_L1_dr &&
		    !mgd_timer_running(&isisidb->isis_dr_timer)) {
		    isis_compute_L1_dr(pdb, idb);
		    isisidb->isis_compute_L1_dr = FALSE;
		}
		if (isisidb->isis_compute_L2_dr &&
		    !mgd_timer_running(&isisidb->isis_dr_timer)) {
		    isis_compute_L2_dr(pdb, idb);
		    isisidb->isis_compute_L2_dr = FALSE;
		}
	    }
	}

	/* Process any expired timers. */

	while ((expired_timer = mgd_timer_first_expired(&pdb->timer))) {
	    switch (mgd_timer_type(expired_timer)) {

	      case ISIS_L1_HELLO_TIMER:
		
		/* L1 Hello timer.  Send the hello as necessary. */

		idb = mgd_timer_context(expired_timer);
		isisidb = get_isis_idb(pdb, idb);
		if (!isisidb)
		    goto timer_error;
		mgd_timer_start_jittered(expired_timer,
					 isis_we_are_dr(pdb, idb,
							ISIS_CIRCUIT_L1) ?
					 isisidb->isis_L1_dr_hello_interval :
					 isisidb->isis_L1_hello_interval,
					 CLNS_JITTER_PCT);
		isis_send_iih(pdb, idb, ISIS_CIRCUIT_L1);
		
		break;

	      case ISIS_L2_HELLO_TIMER:
		
		/* L2 Hello timer.  Send the hello as necessary. */

		idb = mgd_timer_context(expired_timer);
		isisidb = get_isis_idb(pdb, idb);
		if (!isisidb)
		    goto timer_error;
		if (!is_p2p(idb) &&
		    (isisidb->isis_circuit_type & ISIS_CIRCUIT_L2)) {
		    mgd_timer_start_jittered(expired_timer,
					 isis_we_are_dr(pdb, idb,
							ISIS_CIRCUIT_L2) ?
					 isisidb->isis_L2_dr_hello_interval :
					 isisidb->isis_L2_hello_interval,
					 CLNS_JITTER_PCT);
		    isis_send_iih(pdb, idb, ISIS_CIRCUIT_L2);
		} else {
		    mgd_timer_stop(expired_timer);
		}
		break;

	      case ISIS_DR_TIMER:

		/*
		 * DR election timer.  If this is a LAN, force DR election.
		 * If this is a serial line, this is our indication that
		 * we should flood any necessary LSPs across the link.
		 * Note that the LSP pacing timer will be expiring
		 * at the same time as the DR timer for that WAN link, so
		 * we don't have to do anything at all.
		 */

		idb = mgd_timer_context(expired_timer);
		isisidb = get_isis_idb(pdb, idb);
		if (!isisidb)
		    goto timer_error;

		if (!is_p2p(idb)) {
 		    isis_trigger_dr_election(pdb, isisidb,
					     pdb->proc_type == PROC_TYPE_ISIS ?
 					     ISIS_CIRCUIT_L1L2 :
 					     ISIS_CIRCUIT_L1);
		}
		mgd_timer_stop(expired_timer);
		break;

	      case ISIS_ADJ_TIMER:

		/*
		 * Adjacency expired.  Go delete it.  Schedule a Hello to
		 * be sent to tell the world that it happened.
		 */
		adj = mgd_timer_context(expired_timer);
		mgd_timer_stop(expired_timer);
		isisidb = get_isis_idb(pdb, adj->idb);
		if (isisidb) {
		    if ((adj->adjacencyType & CLNS_L1_IS_ADJ_TYPE) ||
			(adj->adjacencyType & CLNS_NLSP_IS_ADJ_TYPE)) {
			isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
		    }
		    if (adj->adjacencyType & CLNS_L2_IS_ADJ_TYPE) {
			isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L2);
		    }
		}
		isis_log_adjacency_change(pdb, adj, FALSE,
					  "hold time expired");

		delete_adjacency(pdb->adjacency_db, adj);
		break;

	      default:
	      timer_error:
		mgd_timer_stop(expired_timer);
		break;
	    }
	}
    }
}

/*
 * isis_mark_lsp_purged
 *
 * Mark an LSP as purged.  This means turning off the "active" bit,
 * and starting the zero-age timer.  (The single lsp_timer is used
 * for both the hold timer and the zero-age timer;  the setting of
 * lsp_active determines which it is.)
 */
void isis_mark_lsp_purged (isis_lspdbtype *lsp, int zero_age_lifetime)
{
    lsp->lsp_active = FALSE;
    mgd_timer_start(&lsp->lsp_timer, zero_age_lifetime);
}

/*
 * isis_purge_lsp
 *
 * Prematurely age out an LSP.  Truncate it, mark it inactive, and flood
 * it.  Retain the purged LSP as indicated.
 */
void isis_purge_lsp (clns_pdbtype *pdb, isis_lspdbtype *lsp, int level,
		     ulong zero_age_lifetime)
{
    isis_mark_lsp_purged(lsp, zero_age_lifetime);
    lsp->db_length = lsp->data->item - (uchar *) lsp;
    compute_iso_checksum(&lsp->data->lspid, isis_lspdb_data_length(lsp),
			 lsp->data->checksum);
    isis_set_all_srm(pdb, lsp, NULL);
    update_pbuginf(pdb, "Purging LSP %s, seq %x",
		   lspid_string(&lsp->data->lspid), 
		   GETLONG(&lsp->data->sequence_number));
}

/*
 * isis_insert_auth
 *
 * Insert password in IS-IS packet.
 */
isis_info_type *isis_insert_auth (
    isis_info_type *pakptr,
    clns_pdbtype *pdb,
    isisidbtype *idb,
    int level,
    int bytes_left)
{

    uchar *pw_ptr = NULL;

    if (idb) {
	pw_ptr = (level == ISIS_CIRCUIT_L1) ? idb->isis_L1_pw : 
	    idb->isis_L2_pw;
    } else if (pdb) {
	pw_ptr = (level == ISIS_CIRCUIT_L1) ? pdb->area_password :
	    pdb->domain_password;
    }
    if (!pw_ptr) return(pakptr);

    if (bytes_left < ISIS_INFO_TYPE_OVERHEAD)
	return(pakptr);			/* Out of space. */

    pakptr->code = ISIS_AUTHENTICATION;
    pakptr->length = strlen((char *) pw_ptr);
    bytes_left -= ISIS_INFO_TYPE_OVERHEAD;

    if (bytes_left < pakptr->length) {
	pakptr->length = 0;		/* Never mind. */
	return(pakptr);			/* Out of space. */
    }

    /*
     * Cleartext password.
     */
    pakptr->value[0] = ISIS_AUTH_PW_TYPE;
    bcopy(pw_ptr, (char *) (pakptr->value+1), pakptr->length);
    pakptr->length++;  /* one byte for auth type */

    /*
     * Return new position of packet.
     */
    ADVANCE_ITEM(pakptr, TRUE);		/* Length guaranteed nonzero */
    return(pakptr);

}

/*
 * isis_check_auth
 *
 * Check incoming packet for proper password, if any. If the 
 * specified interface does not support authentication, return
 * TRUE. Otherwise, match the password configured (level dependent)
 * with one in packet.
 */

boolean isis_check_auth (
    isis_info_type *item,
    char *packet_end,
    clns_pdbtype *pdb,
    isisidbtype *idb,
    int level)
{

    uchar *password = NULL;
    int  i, length;

    if (idb) {
	if (level == ISIS_CIRCUIT_L1) {
	    if (!(password = idb->isis_L1_pw)) return(TRUE);
	} else {
	    if (!(password = idb->isis_L2_pw)) return(TRUE);
	}
    }

    if (pdb) {
	if (level == ISIS_CIRCUIT_L1) {
	    if (!(password = pdb->area_password)) return(TRUE);
	} else {
	    if (!(password = pdb->domain_password)) return(TRUE);
	}
    }

    /*
     * Search for Authentication option in packet. When found,
     * compare lengths then compare each byte.
     */
    while ((char *) item < packet_end) {
	if (item->code == ISIS_AUTHENTICATION) {
	    /* Must accept non-password authentication. */
	    if (item->value[0] != ISIS_AUTH_PW_TYPE) return(FALSE);

	    length = item->length - 1;  /* Account for Auth type field */
	    if (strlen((char *) password) != length) return(FALSE);
	    for (i = 1; i < length; i++) {
		if (*password++ != item->value[i]) return(FALSE);
	    }
	    return(TRUE);
	}
	ADVANCE_ITEM(item, TRUE);
    }
    return(FALSE);
}

/*
 * isis_lsp_gen_command
 *
 * Process the "lsp-gen-interval" command for NLSP and ISIS.
 *
 * interactive  did we come here from the parser or elsewhere
 * enabling     csb->sense  true if enabling
 * nvgen        csb->nvgen  true to write NVM commands
 * cmd          the command name csb->command
 * interval     the  interval (always set, regardless of csb->sense)
 * level         the level (1=L1, 2=L2, 0=both)
 * pdb          the clns_pdb
 */
void isis_lsp_gen_command (boolean interactive, boolean enabling,
			   boolean nvgen, char *cmd, int interval, int level,
			   clns_pdbtype *pdb)
{
    int ix;
    boolean non_default, multi_command;

    if (nvgen) {

	/* Decide whether we need one command or more than one. */

	non_default = FALSE;
	multi_command = FALSE;
	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    if (pdb->lsp_gen_interval[ix] !=
		ISIS_NLSP_LSP_GEN_DEFAULT * ONESEC)
		non_default = TRUE;
	    if (pdb->lsp_gen_interval[ix] != pdb->lsp_gen_interval[0])
		multi_command = TRUE;
	}
	if (multi_command) {
	    for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
		nv_write((pdb->lsp_gen_interval[ix] != 
			  ISIS_NLSP_LSP_GEN_DEFAULT * ONESEC),
			 "%s level-%d %d", cmd, ix + 1,
			 pdb->lsp_gen_interval[ix] / ONESEC);
	    }
	} else {
	    nv_write(non_default, "%s %d", cmd, 
		     pdb->lsp_gen_interval[0] / ONESEC);
	}
	return;
    }

    if (level) {
	pdb->lsp_gen_interval[level - 1] = interval * ONESEC;

    } else {

	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    pdb->lsp_gen_interval[ix] = interval * ONESEC;
	}
    }
}

/*
 * isis_upq_depth_command
 *
 * Process the "update-queue-depth" command for ISIS/NLSP.
 *
 * OBJ(int,1) = update queue depth if csb->sense is TRUE
 */
void isis_upq_depth_command (parseinfo *csb, clns_pdbtype *pdb)
{
    queuetype overflow_queue;
    int size;

    if (csb->nvgen) {
	size = process_queue_max_size(pdb->update_inputq);
	nv_write(size != ISIS_MAX_QUEUE_DEPTH, "%s %d", csb->nv_command, size);
	return;
    }

    queue_init(&overflow_queue, 0);
    process_resize_queue(pdb->update_inputq, ((csb->sense) ? GETOBJ(int,1) :
					      ISIS_MAX_QUEUE_DEPTH),
			 &overflow_queue);

    /* Free up any shed packets. */

    while (overflow_queue.qhead) {
	datagram_done(dequeue(&overflow_queue));
    }
}

/*
 * isis_lspmtu_command
 *
 * Process the "lsp-mtu" command for ISIS/NLSP.
 *
 * interactive  did we come here from the parser or elsewhere
 * enabling     csb->sense  true if enabling
 * nvgen        csb->nvgen  true to write NVM commands
 * cmd          the command name csb->command
 * mtu = OBJ(int,1) = new LSP MTU if csb->sense is TRUE
 * pdb          clns pdb
 */
void isis_lspmtu_command (boolean interactive, boolean sense, boolean nvgen,
			  char *cmd, int mtu, clns_pdbtype *pdb)
{
    if (nvgen) {
	nv_write(pdb->packet_mtu != pdb->default_packet_mtu, "%s %d",
		 cmd, pdb->packet_mtu + pdb->packet_overhead);
	return;
    }
    if (sense) {
	pdb->packet_mtu = mtu - pdb->packet_overhead;
    } else {
	pdb->packet_mtu = pdb->default_packet_mtu;
    }
}

/*
 * isis_prc_ivl_command
 *
 * Process the "prc-interval" command for NLSP.
 *
 * OBJ(int,1) = interval (set for all values of csb->sense)
 * OBJ(int,2) = level (1=L1, 2=L2, 0=both)
 */
void isis_prc_ivl_command (parseinfo *csb, clns_pdbtype *pdb)
{
    int ix;
    boolean non_default, multi_command;

    if (csb->nvgen) {

	/* Decide whether we need one command or more than one. */

	non_default = FALSE;
	multi_command = FALSE;
	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    if (pdb->prc_interval[ix] != ISIS_NLSP_PRC_IVL_DEFAULT * ONESEC)
		non_default = TRUE;
	    if (pdb->prc_interval[ix] != pdb->prc_interval[0])
		multi_command = TRUE;
	}
	if (multi_command) {
	    for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
		nv_write((pdb->prc_interval[ix] !=
			  ISIS_NLSP_PRC_IVL_DEFAULT * ONESEC),
			 "%s level-%d %d", csb->nv_command, ix + 1,
			 pdb->prc_interval[ix] / ONESEC);
	    }
	} else {
	    nv_write(non_default, "%s %d", csb->nv_command, 
		     pdb->prc_interval[0] / ONESEC);
	}
	return;
    }

    if (GETOBJ(int,2)) {
	pdb->prc_interval[GETOBJ(int,2) - 1] = GETOBJ(int,1) * ONESEC;

    } else {

	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    pdb->prc_interval[ix] = GETOBJ(int,1) * ONESEC;
	}
    }
}

/*
 * isis_spf_ivl_command
 *
 * Process the "spf-interval" command for NLSP and ISIS.
 *
 * OBJ(int,1) = interval (set for all values of csb->sense)
 * OBJ(int,2) = level (1=L1, 2=L2, 0=both)
 */
void isis_spf_ivl_command (parseinfo *csb, clns_pdbtype *pdb)
{
    int ix;
    boolean non_default, multi_command;

    if (csb->nvgen) {

	/* Decide whether we need one command or more than one. */

	non_default = FALSE;
	multi_command = FALSE;
	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    if (pdb->spf_interval[ix] != ISIS_NLSP_SPF_IVL_DEFAULT * ONESEC)
		non_default = TRUE;
	    if (pdb->spf_interval[ix] != pdb->spf_interval[0])
		multi_command = TRUE;
	}
	if (multi_command) {
	    for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
		nv_write((pdb->spf_interval[ix] !=
			  ISIS_NLSP_SPF_IVL_DEFAULT * ONESEC),
			 "%s level-%d %d", csb->nv_command, ix + 1,
			 pdb->spf_interval[ix] / ONESEC);
	    }
	} else {
	    nv_write(non_default, "%s %d", csb->nv_command, 
		     pdb->spf_interval[0] / ONESEC);
	}
	return;
    }

    if (GETOBJ(int,2)) {
	pdb->spf_interval[GETOBJ(int,2) - 1] = GETOBJ(int,1) * ONESEC;

    } else {

	for (ix = 0; ix < ISIS_NUM_LEVELS; ix++) {
	    pdb->spf_interval[ix] = GETOBJ(int,1) * ONESEC;
	}
    }
}

/*
 * isis_ignore_attached_bit_command
 *
 * Give the user a choice whether he wants a default route out of the area.
 *
 * OBJ(int,1) = 1 if ip, 2 if clns, 0 if unspecified
 */
void isis_ignore_attached_bit_command(parseinfo *csb, clns_pdbtype *pdb)
{
    int oldstatus;

    if (csb->nvgen) {
	if (pdb->ignore_att_bit_ip && pdb->ignore_att_bit_clns) {
	    nv_write(TRUE, "%s", csb->nv_command);
	} else {
	    nv_write(pdb->ignore_att_bit_ip, "%s ip", csb->nv_command);
	    nv_write(pdb->ignore_att_bit_clns, "%s clns", csb->nv_command);
	}
	return;
    }

    oldstatus = pdb->ignore_att_bit_ip + (pdb->ignore_att_bit_clns <<1);

    switch (GETOBJ(int, 1)) {
    case 0:
	pdb->ignore_att_bit_ip = csb->sense;
	pdb->ignore_att_bit_clns = csb->sense;
	break;
    case 1:
	pdb->ignore_att_bit_ip = csb->sense;
	break;
    case 2:
	pdb->ignore_att_bit_clns = csb->sense;
	break;
    default:
	break;
    }

    if (oldstatus != pdb->ignore_att_bit_ip + (pdb->ignore_att_bit_clns <<1)) {
	isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_ATTACH, NULL, TRUE);
    }
}

/*
 * isis_log_adj_changes_command
 *
 * Set the "log adjacency changes" flag.
 */
void isis_log_adj_changes_command (parseinfo *csb, clns_pdbtype *pdb)
{
    if (csb->nvgen) {
	nv_write(pdb->log_adj_changes, csb->nv_command);
	return;
    }
    pdb->log_adj_changes = (csb->sense != FALSE);
}

/*
 * isis_lsp_refresh_command
 *
 * Set the LSP refresh interval
 *
 * interactive  did we come here from the parser or elsewhere
 * enabling     csb->sense  true if enabling
 * nvgen        csb->nvgen  true to write NVM commands
 * cmd          the command name csb->command
 * interval =  OBJ(int,1), refresh interval in secs (regardless of csb->sense)
 * pdb          clns pdb
 */
void isis_lsp_refresh_command (boolean interactive, boolean enabling,
			       boolean nvgen, char *cmd, int interval,
			       clns_pdbtype *pdb)
{
    ulong default_val;

    /* First NVGENs. */

    if (nvgen) {
	if (pdb->proc_type == PROC_TYPE_ISIS)
	    default_val = ISIS_MAX_LSP_GEN_DEFAULT * ONESEC;
	else
	    default_val = NOV_NLSP_MAX_LSP_GEN_DEFAULT * ONESEC;
	nv_write(pdb->lsp_refresh_interval != default_val, "%s %d",
		 cmd, pdb->lsp_refresh_interval / ONESEC);
	return;
    }

    pdb->lsp_refresh_interval = interval * ONESEC;
    isis_refresh_lsps(pdb);		/* Pick up the new value. */
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	mgd_timer_start(&pdb->isis_age_timer, pdb->lsp_refresh_interval);
    } else {
	TIMER_START(pdb->checksum_timer, pdb->lsp_refresh_interval);
    }
}

/*
 * isis_lsp_lifetime_command
 *
 * Set the LSP lifetime
 *
 * interactive  did we come here from the parser or elsewhere
 * enabling     csb->sense  true if enabling
 * nvgen        csb->nvgen  true to write NVM commands
 * cmd          the command name csb->command
 * interval = OBJ(int,1) = lifetime in seconds (regardless of csb->sense)
 * pdb          clns pdb
 */
void isis_lsp_lifetime_command (boolean interactive, boolean enabling,
				boolean nvgen, char *cmd, int interval,
				clns_pdbtype *pdb)
{
    ulong default_val;

    /* First NVGENs. */

    if (nvgen) {
	if (pdb->proc_type == PROC_TYPE_ISIS)
	    default_val = ISIS_MAXAGE_DEFAULT * ONESEC;
	else
	    default_val = NOV_NLSP_MAXAGE_DEFAULT * ONESEC;
	nv_write(pdb->lsp_lifetime != default_val, "%s %d",
		 cmd, pdb->lsp_lifetime / ONESEC);
	return;
    }

    pdb->lsp_lifetime = interval * ONESEC;
    isis_refresh_lsps(pdb);		/* Send 'em out with the new life. */
}

/*
 * isis_sysid_display_command
 *
 * Process the "name-display-disable" commands for NLSP (and ISIS, perhaps)
 */
void isis_sysid_display_command (parseinfo *csb, clns_pdbtype *pdb)
{
    if (csb->nvgen) {
	nv_write(pdb->name_lookup_disabled, "%s", csb->nv_command);
	return;
    }
    pdb->name_lookup_disabled = csb->sense;
}

/*
 * isis_reset_circuit_id
 *
 * Reset ISIS circuit IDs back to default.
 */

void isis_reset_circuit_id (clns_pdbtype *pdb, idbtype *idb)
{
    isisidbtype *isisidb;

    if (!idb || !pdb)
	return;
    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)      		/* ISIS not yet initialized on this interface*/
	return;
    if (!itf_in_pdb(idb, pdb))
	return;
    station_copy(pdb->systemId, isisidb->isis_L1_circuit_id);
    isisidb->isis_L1_circuit_id[ISIS_PSEUDO_ID] =
	isisidb->isis_local_ckt_number;
    systemid_copy(isisidb->isis_L1_circuit_id,
		  isisidb->isis_L2_circuit_id);
    systemid_copy(isisidb->isis_L1_circuit_id, isisidb->isis_L1_dr_id);
    systemid_copy(isisidb->isis_L1_circuit_id, isisidb->isis_L2_dr_id);
}

/*
 * isis_release_idb_number
 *
 * Release allocated interface and circuit numbers for an ISIS idb.
 */
void isis_release_idb_number (clns_pdbtype *pdb, isisidbtype *isisidb)
{
    ulong procindex;
    ulong ckt_number;
    dynamic_bitfield *ckt_bitfield;

    procindex = isis_pdbindex_to_procindex(pdb->index);

    if ((isisidb->isis_if_number >= 0) && /* Still valid */
	isis_idb_bitfield && isis_lan_bitfield && isis_p2p_bitfield) {

	bitfield_clear(isisidb->isis_if_number, &isis_idb_bitfield[procindex]);

	if (isisidb->isis_local_ckt_number >= ISIS_P2P_CKT_OFFSET) { /* P2P */
	    ckt_number = isisidb->isis_local_ckt_number - ISIS_P2P_CKT_OFFSET;
	    ckt_bitfield = &isis_p2p_bitfield[procindex];
	} else {			/* LAN link */
	    ckt_number = isisidb->isis_local_ckt_number;
	    ckt_bitfield = &isis_lan_bitfield[procindex];
	}
	bitfield_clear(ckt_number, ckt_bitfield);

	isisidb->isis_if_number = -1;	/* an impossible if_number */
    }
}

/*
 * isis_reset_idb_params
 *
 * Turn off timers, etc.
 */
void isis_reset_idb_params (clns_pdbtype *pdb, idbtype *idb, 
			    boolean reset_if_number)
{
    isisidbtype *isisidb;
    isis_rexmit_entry *rexmit;
    isis_lspdbtype *lsp_db;
    int ix;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;
    mgd_timer_stop(&isisidb->isis_hello_timer);
    mgd_timer_stop(&isisidb->isis_hello2_timer);
    mgd_timer_stop(&isisidb->isis_dr_timer);

    if (isis_we_are_dr(pdb, idb, CLNS_LEVEL1)) {
	isis_purge_dr(CLNS_LEVEL1, pdb, isisidb);
    }
    if (isis_we_are_dr(pdb, idb, CLNS_LEVEL2)) {
	isis_purge_dr(CLNS_LEVEL2, pdb, isisidb);
    }
    bitfield_clear(isisidb->isis_if_number, &pdb->SRM_mask);
    for (ix=0; ix < ISIS_NUM_LEVELS; ix++) {
	mgd_timer_stop(&isisidb->csnp_ctl[ix].isis_csnp_timer);
    }
    mgd_timer_stop(&isisidb->isis_psnp_timer);
    mgd_timer_stop(&isisidb->isis_lsp_pacing_timer);
    isisidb->isis_lsp_send_requested = FALSE; /* Goes with the pacing timer */
    mgd_timer_stop(&isisidb->isis_rexmit_timer);
    isis_deconfigure_mesh_group(pdb, isisidb);

    /* Flush the retransmission queue. */

    while ((rexmit = dequeue(&isisidb->isis_rexmit_queue))) {
	if (rexmit->level == ISIS_CIRCUIT_L1)
	    lsp_db = isis_find_L1_lsp(pdb, &rexmit->timer_id);
	else
	    lsp_db = isis_find_L2_lsp(pdb, &rexmit->timer_id);
	if (lsp_db)
	    bitfield_clear(isisidb->isis_if_number, &lsp_db->on_rexmit_queue);
	free(rexmit);
    }

    isisidb->isis_running = FALSE;

    if (reset_if_number) {
	isis_release_idb_number(pdb, isisidb);
    }
}

/*
 * isis_start_interface
 *
 * Initialize IDB fields for IS-IS is now operational on this
 * interface.
 *
 */
void isis_start_interface (clns_pdbtype *pdb, idbtype *idb)
{
    isisidbtype *isisidb;

    isisidb = get_isis_idb(pdb, idb);
    if (!isisidb)
	return;

    /* If already running, bail. */

    if (isisidb->isis_running)
	return;

    isisidb->isis_running = TRUE;

    queue_init(&isisidb->isis_rexmit_queue, 0);

    /* 
     * Set up the various ISIS timers.
     */
    mgd_timer_init_leaf(&isisidb->isis_hello_timer, &pdb->timer,
			ISIS_L1_HELLO_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&isisidb->isis_hello2_timer, &pdb->timer,
			ISIS_L2_HELLO_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&isisidb->isis_dr_timer, &pdb->timer,
			ISIS_DR_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&isisidb->csnp_ctl[0].isis_csnp_timer,
			&pdb->update_timer, 
			ISIS_L1_CSNP_TIMER,
			idb, FALSE);
    mgd_timer_init_leaf(&isisidb->csnp_ctl[1].isis_csnp_timer,
			&pdb->update_timer,
			ISIS_L2_CSNP_TIMER,
			idb, FALSE);
    mgd_timer_init_leaf(&isisidb->isis_psnp_timer, &pdb->update_timer,
			ISIS_PSNP_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&isisidb->isis_rexmit_timer, &pdb->update_timer,
			ISIS_REXMIT_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&isisidb->isis_lsp_pacing_timer, &pdb->update_timer,
			ISIS_LSP_PACING_TIMER, idb, FALSE);

    if ((pdb->level & CLNS_LEVEL1) || is_p2p(idb)) {
	GET_TIMESTAMP(isisidb->isis_last_hello);
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L1);
    }
    if (pdb->level & CLNS_LEVEL2) {
	GET_TIMESTAMP(isisidb->isis_last2_hello);
	isis_schedule_hello(pdb, isisidb, ISIS_CIRCUIT_L2);
    }
    isisidb->isis_compute_L1_dr = FALSE;
    isisidb->isis_compute_L2_dr = FALSE;
    isisidb->isis_L1_dr_changed = FALSE;
    isisidb->isis_L2_dr_changed = FALSE;
    isisidb->isis_lsp_send_requested = FALSE;
    isisidb->isis_if_state = CLNS_ADJ_FAILED_STATE;

    isis_reset_circuit_id(pdb, idb);
	

    if (!is_p2p(idb)) {
	/*
	 * This timer is set to guarantee that we don't start running
	 * the designated router computation until after we should
	 * have heard from the other ISes on the LAN.  We do twice the
	 * hello interval for NLSP, since the new machine will normally
	 * never be the DR, but for ISIS we limit it to three seconds in
	 * order to not be without a DR for a long time.
	 */
	if (pdb->proc_type == PROC_TYPE_ISIS) {
	    mgd_timer_start(&isisidb->isis_dr_timer, 3 * ONESEC);
	} else {
	    mgd_timer_start(&isisidb->isis_dr_timer,
			    2 * isisidb->isis_L1_hello_interval);
	}

	/*
	 * Start up the CSNP timers.  We don't send periodic CSNPs on
	 * serial lines.
	 */
	mgd_timer_start_jittered(&isisidb->csnp_ctl[0].isis_csnp_timer,
				 isisidb->csnp_ctl[0].csnp_interval,
				 ISIS_JITTER_PERCENT);
	if (pdb->proc_type == PROC_TYPE_ISIS) {	/* Not for NLSP */
	    mgd_timer_start_jittered(&isisidb->csnp_ctl[1].isis_csnp_timer,
				     isisidb->csnp_ctl[1].csnp_interval,
				     ISIS_JITTER_PERCENT);
	}
    }

    /* Set the interface state according to the current reality. */

    isisidb->isis_interface_up = FALSE;	/* Be surprised if it's up. */
    if (pdb->proc_type == PROC_TYPE_ISIS) {
	reg_invoke_isis_update_interface_state(idb);
    } else {
	reg_invoke_ipx_nlsp_update_interface_state(idb, pdb);
    }

}


/* Type-to-name mapping for NLSP media types */

static struct media_map {
    ulong type;
    const char *name;
} const media_map[] = {
    {NLSP_TYPE_GENERIC_LAN, "Generic LAN"},
    {NLSP_TYPE_GENERIC_WAN, "Generic WAN"},
    {NLSP_TYPE_LOCALTALK, "LocalTalk"},
    {NLSP_TYPE_ETHERNET, "Ethernet"},
    {NLSP_TYPE_802_3_LLC, "802.3/LLC"},
    {NLSP_TYPE_802_3_RAW, "802.3 Raw"},
    {NLSP_TYPE_802_3_SNAP, "802.3/SNAP"},
    {NLSP_TYPE_802_5_LLC, "802.5/LLC"},
    {NLSP_TYPE_802_5_SNAP, "802.5/SNAP"},
    {NLSP_TYPE_ISDN, "ISDN"},
    {NLSP_TYPE_IP_TUNNEL, "IP Tunnel"},
    {NLSP_TYPE_IP_RELAY, "IP Relay"},
    {NLSP_TYPE_FDDI_LLC, "FDDI/LLC"},
    {NLSP_TYPE_FDDI_SNAP, "FDDI/SNAP"},
    {NLSP_TYPE_PPP, "PPP"},
    {NLSP_TYPE_X_25, "X.25"},
    {NLSP_TYPE_FRAME_RELAY, "Frame Relay"}
};

#define NUM_MEDIA_MAP (sizeof(media_map) / sizeof(struct media_map))

/*
 * nlsp_media_type
 *
 * Returns a string containing the media type corresponding to the
 * code provided.
 */
static const char *nlsp_media_type(ulong media_type)
{
    int i;
    for (i=0; i<NUM_MEDIA_MAP; i++) {
	if (media_map[i].type == media_type)
	    return(media_map[i].name);
    }
    return("Unknown");
}

/*
 * isis_print_lsp_bits
 *
 * Print the list of interfaces for which the funky bits are set.
 */

static void isis_print_lsp_bits (dynamic_bitfield *bits, char *bit_name,
				 idbtype **idb_lookup)
{
    ulong bit_index;
    ulong hit_count;

    /*
     * Do it the brute force way.  It's only a display routine, and printf()
     * is suspending for us anyhow.
     */
    printf("\n  %s bits set on", bit_name);
    hit_count = 0;
    for (bit_index = 0;  bit_index < bits->bitfield_size; bit_index++) {
	if (bitfield_check(bit_index, bits)) {
	    if (hit_count) {
		printf("\n                 ");
	    }
	    hit_count++;
	    if (idb_lookup[bit_index])
		printf(" %s", idb_lookup[bit_index]->namestring);
	    else
		printf(" interface %#x", bit_index);
	}
    }
    if (!hit_count)
	printf(" no interfaces");
}

/*
 * isis_display_lsp_internals
 *
 * Display the guts of an LSP
 */

static void isis_display_lsp_internals (clns_pdbtype *pdb, 
					isis_lspdbtype *lsp_db,
					char *buf)
{
    idbtype **idb_lookup;
    idbqueuetype *idbqueue;
    idbtype *idb;
    isisidbtype *isisidb;
    isis_lsp_parent *parent;
    ulong parent_index;
    char kind;
    int isisindex;

    /*
     * Build an array of IDB pointers from the set of IDBs on which ISIS
     * is configured.  It's kind of silly to do this for every LSP that
     * we display, but otherwise we'd have to find some global storage
     * for it, and we hardly ever do this, and it's for a hidden display
     * command anyhow.  Plus, it beats searching all the IDB queues for
     * every set bit.
     */
    isisindex = isis_pdbindex_to_procindex(pdb->index);
    idb_lookup = malloc(sizeof(idbtype *) *
			isis_idb_bitfield[isisindex].bitfield_size);
    if (!idb_lookup) {
	printf(nomemory);
	return;
    }
    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);
	if (isisidb) {
	    if (isisidb->isis_if_number <
		isis_idb_bitfield[isisindex].bitfield_size) {
		idb_lookup[isisidb->isis_if_number] = idb;
	    }
	}
    }

    /* First some basics. */

    printf("\n  Address %#x, length %d, max_length %d", lsp_db,
	   lsp_db->db_length, lsp_db->max_db_length);
    if (lsp_db->non_routing)
	printf(", non_routing");
    if (lsp_db->db_on_paths)
	printf(", on_paths");
    if (lsp_db->db_on_tent)
	printf(", on_tent");
    if (lsp_db->lsp_freed)
	printf(", freed!");

    /* Now some timers. */

    if (TIMER_RUNNING(lsp_db->next_gen)) {
	printf("\n  Next LSP generation allowed in %d msec",
	       TIME_LEFT_SLEEPING(lsp_db->next_gen));
    }
    if (mgd_timer_running(&lsp_db->gen_limit_timer)) {
	printf("\n  LSP will be generated in %d msec",
	       mgd_timer_left_sleeping(&lsp_db->gen_limit_timer));
    }
    if (lsp_db->lsp_refresh_timer) {
	printf("\n  LSP refresh in %d secs",
	       mgd_timer_left_sleeping(lsp_db->lsp_refresh_timer)/ONESEC);
    }

    /* Let's go home and meet the parents. */

    printf("\n  Root distance %d, index %d, parent index %d, parent count %d",
	   lsp_db->root_distance, lsp_db->lsp_index, lsp_db->parent_index,
	   lsp_db->parent_count);
    kind = (pdb->proc_type == PROC_TYPE_NLSP ? 'N' : 'A');
    for (parent_index = 0;
	 parent_index < lsp_db->parent_count;
	 parent_index++) {
	parent = &lsp_db->parents[parent_index];
	if (parent && parent->next_hop) {
	    clns_show_one_neighbor(pdb, parent->next_hop, kind, FALSE, TRUE);
	    if (pdb->proc_type == PROC_TYPE_NLSP) {
		printf("\n    Throughput %d Kbps, delay %d usec, mtu %d, "
		       "hops %d, ticks %d",
		       parent->throughput / 1000, parent->delay, parent->mtu,
		       parent->hops, parent->ticks);
	    }
	}
    }

    /* Now lets dump the cool bitfields. */

    isis_print_lsp_bits(&lsp_db->SRM, "SRM", idb_lookup);
    isis_print_lsp_bits(&lsp_db->SSN, "SSN", idb_lookup);
    isis_print_lsp_bits(&lsp_db->on_rexmit_queue, "RXM", idb_lookup);
    free(idb_lookup);
}

/*  
 * isis_display_lsp
 *
 * This routine is called at the bottom of a recursive walk of the AVL 
 * tree. It will display the contents of one LSP.
 */

#define DISPLAY_BUF_SIZE 200		/* More than big enough, yeah, right */

boolean isis_display_lsp (isis_tree_type *element,
				 int level,
				 boolean long_display,
				 boolean internal_display,
				 clns_pdbtype *pdb)
{

    isis_lspdbtype   *lsp_db;
    isis_info_type   *item, *old_item;
    isis_generic_ad  *adv;
    isis_metric_type *metric = NULL;
    char	     hold_time[20];
    int		     i;
    char 	     *display_buf;
    ulong            netmask;

    /* Bail out if we're quitting. */

    if (automore_quit())
	return(FALSE);

    lsp_db = element->lsp;
    if (!lsp_db)
	return(FALSE);			/* It was freed from under us */

    display_buf = malloc_named(DISPLAY_BUF_SIZE, "LSP display buffer");
    if (!display_buf)
	return(FALSE);

    /*
     * Display the link states from one IS.
     * Function like station_match, clns_stationid_lookup use global
     * circular buffers for generating their output. As printf could
     * suspend, it is possible for the buffers to be used by some other
     * process. To avoid garbage in display, use display_buf at 
     * appropriate places.
     */
    if (lsp_db->lsp_active) {
	sprintf(hold_time, "%d",
		mgd_timer_left_sleeping(&lsp_db->lsp_timer) / ONESEC);
    } else {
	sprintf(hold_time, "0 (%d)",
		mgd_timer_left_sleeping(&lsp_db->lsp_timer) / ONESEC);
    }

    sprintf(display_buf, "\n%20s%s 0x%08x   0x%04x        %14s%d/%d/%d",
	    lspid_name_string(pdb, &lsp_db->data->lspid),
	    (station_match(pdb->systemId, lsp_db->data->lspid.system_id)) ? 
	    "*" : " ", GETLONG(&lsp_db->data->sequence_number), 
	    GETSHORT(lsp_db->data->checksum), hold_time, 
	    (lsp_db->data->att & ISIS_LSP_L2_ATTACHED) ? 1 : 0, 
	    (lsp_db->data->p) ? 1 : 0, 
	    (lsp_db->data->hippity) ? 1 : 0);
    printf("%s", display_buf);
    if (pdb->long_route_display) {
	printf("  (%d)", lsp_db->lsp_index);
    }
    if (long_display) {
	adv = NULL;
	item = NULL;
	old_item = NULL;
	adv = isis_walk_lsp(pdb, &lsp_db, &item, adv, 0, level);

	while (adv && element->lsp == lsp_db) {

	    /*
	     * Display one item.
	     */
	    switch (item->code) {
	      case ISIS_ES_NEIGHBORS:
		metric = (isis_metric_type *)(char *) item->value;
		sprintf(display_buf, "\n  Metric: %2d ES %s",
			metric->default_metric,	
			clns_stationid_lookup(pdb, adv->es_neighbor));
		printf("%s", display_buf);
		break;

	      case ISIS_IS_NEIGHBORS:
		sprintf(display_buf, "\n  Metric: %2d IS %s",
			adv->is_neighbor.metric.default_metric,
			systemid_name_string(pdb,
					     adv->is_neighbor.neighbor_id));
		printf("%s", display_buf);
		break;

	      case ISIS_PREFIX_NEIGHBORS:
		metric = (isis_metric_type *)(char *)&item->value;
		sprintf(display_buf, "\n  Metric: %2d PR %s", 
			metric->default_metric,
			prefix_string(adv->prefix_neighbor));
		printf("%s", display_buf);
		break;

	      case ISIS_AREA_ADDRESSES:
		printf("\n  Area Address: %n", adv->area_address);
		break;

	      case ISIS_IP_INTERNAL_ADDR:
 		printf("\n  Metric: %2d IP %i %i", 
 		       adv->ip_info.metric.default_metric, 
		       GETLONG(&adv->ip_info.ip_address), 
 		       GETLONG(&adv->ip_info.mask));
 		break;
 
	      case ISIS_IP_EXTERNAL_ADDR:
 		printf("\n  Metric: %2d IP-External %i %i", 
 		       adv->ip_info.metric.default_metric, 
		       GETLONG(&adv->ip_info.ip_address), 
 		       GETLONG(&adv->ip_info.mask));
 		break;
 
	      case ISIS_IP_IF_ADDRESSES:
 		printf("\n  IP Address:  %i", GETLONG(&adv->ip_address));
 		break;
 
	      case ISIS_IDR_INFO:
 		isis_print_idr_info(item);
 		break;
 
	      case ISIS_NLPID:
 		if (old_item != item) {
 		    printf("\n  NLPID:       ");
 		}
 		printf("0x%02x ", adv->nlpid);
  		break;
  
	      case ISIS_AUTHENTICATION:
 		printf("\n  Auth:        Length: %d", item->length);
 		break;

	      case ISIS_NLSP_AREA_ADDRS:
		printf("\n  IPX Area Address: %08x %08x",
		       GETLONG(&adv->ipx_areas.net_number),
		       GETLONG(&adv->ipx_areas.net_mask));
		break;

	      case ISIS_NLSP_MGMT_INFO:
		printf("\n  IPX Mgmt Info %x.%e  Ver %d",
		       GETLONG(&adv->ipx_mgmt_info.net_number),
		       adv->ipx_mgmt_info.node_number,
		       adv->ipx_mgmt_info.ipx_version_number);
		if (adv->ipx_mgmt_info.router_name_length > 0) {
		    printf("  Name ");
		    for (i=0; i< adv->ipx_mgmt_info.router_name_length; i++) {
			printf("%c", adv->ipx_mgmt_info.router_name[i]);
		    }
		}
		break;

	      case ISIS_NLSP_LINK_INFO:
		sprintf(display_buf, "\n  Metric: %2d Lnk %17s  MTU %d  "
			"Dly %d  Thru %dK  %s",
			adv->ipx_link_info.cost,
			systemid_name_string(pdb,
					     adv->ipx_link_info.neighbor_id),
			GETLONG(&adv->ipx_link_info.mtu_size),
			GETLONG(&adv->ipx_link_info.delay),
			GETLONG(&adv->ipx_link_info.throughput)/1000,
			nlsp_media_type(
					GETSHORT(&adv->ipx_link_info.media_type)));
		printf("%s", display_buf);
		break;

	      case ISIS_NLSP_SVCS_INFO:
		printf("\n  Metric: %2d IPX SVC ", adv->ipx_svcs_info.hops);
		for (i=0; i<item->length - ISIS_NLSP_SVCS_INFO_LEN; i++) {
		    printf("%c", adv->ipx_svcs_info.name[i]);
		}
		printf("  %x.%e  Sock %d  Type %d",
		       GETLONG(&adv->ipx_svcs_info.net_number),
		       adv->ipx_svcs_info.node_number,
		       GETSHORT(&adv->ipx_svcs_info.socket),
		       GETSHORT(&adv->ipx_svcs_info.type));
		break;

	      case ISIS_NLSP_EXT_ROUTES:
		printf("\n  Metric: %2d IPX Ext %x  Ticks %d",
			   adv->ipx_ext_rts.hops,
			   GETLONG(&adv->ipx_ext_rts.net_number),
			   GETSHORT(&adv->ipx_ext_rts.ticks));
		break;

	      case ISIS_NLSP_SUMMARY_ROUTES:
		netmask = NOV_NETMASK_ALL;
		for (i = 32 - adv->ipx_sum_rts.masklen; i; i--)
		    netmask = netmask << 1;
		printf("\n  Area Count: %2d IPX Sum %x %-8x  Ticks %d",
			   adv->ipx_sum_rts.area_count,
			   GETLONG(&adv->ipx_sum_rts.address), netmask,
			   GETSHORT(&adv->ipx_sum_rts.ticks));
		break;

	      default:
		printf("\n  Code:   %2d Length: %d", item->code, 
		       item->length);
	    }
	    
	    old_item = item;
	    adv = isis_walk_lsp(pdb, &lsp_db, &item, adv, 0, level);
	}
    }
    if (internal_display) {
	isis_display_lsp_internals(pdb, lsp_db, display_buf);
    }
    if (long_display || internal_display)
	automore_conditional(0);
    free(display_buf);
    return(TRUE);
}

/*  
 * isis_rw_display_lsp
 *
 * This routine is called at the bottom of a recursive walk of the AVL 
 * tree. It will display the contents of one LSP.
 */
boolean isis_rw_display_lsp (isis_tree_type *element,
			     void *level_parm,
			     void *long_parm,
			     void *internal_parm,
			     void *pdb_parm)
{
    int *level;
    boolean *long_display;
    boolean *internal_display;
    clns_pdbtype *pdb;
    boolean result;

    level = level_parm;
    long_display = long_parm;
    internal_display = internal_parm;
    pdb = pdb_parm;
    result = isis_display_lsp(element, *level, *long_display,
			      *internal_display, pdb);
    return(result);
}

/*
 * parse_lspid
 *
 * Get an LSPID from a command line input. Can be in the form:
 *     xxxx.xxxx.xxxx.xx-xx  or
 *     name.xx-xx
 *
 * Returns TRUE if success, FALSE if LSPID can't be parsed.
 *
 */
static boolean parse_lspid (clns_pdbtype *pdb, uchar *lspid_str,
			    isis_lspid *lspid)
{

    uchar     *ptr;
    nametype  *name;
    uchar     token[TOKLEN];
    int       error, len;

    /*
     * Copy all but .xx-xx to temporary buffer.
     */
    len = strlen(lspid_str) - 6;
    if (len < 0)
	return(FALSE);
    bcopy(lspid_str, token, len);
    token[len] = '\0';
    ptr = token;

    /*
     * First try parsing 6-octet system-id "xxxx.xxxx.xxxx".
     */
    if (!parse_ether_address(ptr, lspid->system_id)) {

	/*
	 * Now try name lookup instead.
	 */
	if (pdb->proc_type == PROC_TYPE_ISIS) {
	    name = name_lookup(ptr, &error, NAM_CLNS);
	    if (!name)
		return(FALSE);
	    station_copy(get_station(name->v.clns.adr[0].addr.clns_address), 
			 lspid->system_id);
	} else {
	    name = name_lookup(ptr, &error, NAM_IPX);
	    if (!name)
		return(FALSE);
	    station_copy(name->v.ipx.adr.nov_addr.host, lspid->system_id);
	}    
    }
    ptr = lspid_str + len + 1;

    /*
     * Get pseudo-id and LSP number.
     */
    lspid->pseudo_id = atohex(ptr);
    ptr += 3;
    lspid->lsp_number = atohex(ptr);

    return(TRUE);		
}

const char isis_data_str[] = { "\nLSPID                 LSP Seq Num  "
				   "LSP Checksum  LSP Holdtime  ATT/P/OL" };

/*
 * isis_display_one_lsp
 *
 * Display a single LSP based on LSPID passed in.
 */
static void isis_display_one_lsp (clns_pdbtype *pdb,
				  uchar *lspid_str,
				  int level,
				  boolean long_display,
				  boolean internal_display)
{
    
    isis_tree_type *element;
    isis_lspid     lspid;
    int            h;

    /*
     * Convert LSPID stored in ASCII to an 8-octet value.
     */
    if (!parse_lspid(pdb, lspid_str, &lspid))
	return;

    if (level == ISIS_CIRCUIT_L1) {
	element = pdb->level1_LSPs;
    } else {
	element = pdb->level2_LSPs;
    }

    element = isis_get_avl_element(&lspid, &element, &h, NULL, NULL, NULL, 
				   0, NULL, NULL);
    if (!element) return;

    printf("\n\nIS-IS Level-%d LSP %s", level, lspid_name_string(pdb, &lspid));
    printf(isis_data_str);

    isis_display_lsp(element, level, long_display, internal_display, pdb);
}

/*
 * show_isis_database
 *
 *    This routine will walk the link state database and show
 *    all of the data we have stored.
 *
 *    Syntax:
 *      show isis database [level-1 | level-2 | Detail | Private | <lspid>]
 *
 *      By default, both level-1 and level-2 link state databases are
 *      displayed in summary format. If "Detail" is specified, the link
 *      state header and body is displayed. If <system id> is specified,
 *      the single LSP is displayed. Any of the parameters can appear in
 *      in any order.
 */

void show_isis_database (parseinfo *csb)
{
    clns_pdbtype *isis_pdb;
    int          level, call_level;
    boolean      long_display, display_one, internal_display;
    char         *name = NULL;

    isis_pdb = NULL;

    switch (csb->which) {
      case SHOW_ISIS_DATABASE:
	isis_pdb = find_isis_pdb();
	break;

      case SHOW_NLSP_DATABASE:
	name = GETOBJ(string,2);
	isis_pdb = reg_invoke_ipx_find_nlsp_pdb(!strcmp(name, "notag") ? '\0' :
						name);
	break;
    }

    if (!isis_pdb) return;

    level = GETOBJ(int,1);
    long_display = GETOBJ(int,2);
    display_one = (*GETOBJ(string,1));
    internal_display = GETOBJ(int,3);

    automore_enable(NULL);

    /*
     * Finally start walking the LSP tree - displaying as we go.
     */
    if ((level & ISIS_CIRCUIT_L1) && isis_pdb->level1_LSPs) {
	if (display_one) {
	    isis_display_one_lsp(isis_pdb, GETOBJ(string,1), ISIS_CIRCUIT_L1,
				 long_display, internal_display);
	} else {
	    if (isis_pdb->proc_type == PROC_TYPE_NLSP) {
		if (strlen(name)) { 
		    printf("\nNLSP Level-1 Link State Database: Tag Identifier = %s", name);
		    printf(isis_data_str);
		    call_level = CLNS_LEVEL1;
		    isis_recursive_walk(NULL, ISIS_WALK_ALL,
					isis_pdb->level1_LSPs, 
					isis_rw_display_lsp, 
					&call_level, &long_display,
					&internal_display, isis_pdb);
		} else {
		    reg_invoke_ipx_nlsp_display_all_database(long_display,
							     internal_display);
		}
	    } else {
		printf("\nIS-IS Level-1 Link State Database");
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
    if ((level & ISIS_CIRCUIT_L2) && isis_pdb->level2_LSPs) {
	if (display_one) {
	    isis_display_one_lsp(isis_pdb, GETOBJ(string,1), ISIS_CIRCUIT_L2,
				 long_display, internal_display);
	} else {
	    printf("\n\nIS-IS Level-2 Link State Database");
	    printf(isis_data_str);
	    call_level = CLNS_LEVEL2;
	    isis_recursive_walk(NULL, ISIS_WALK_ALL,
				isis_pdb->level2_LSPs, 
				isis_rw_display_lsp, 
				&call_level, &long_display,
				&internal_display, isis_pdb);
	}
    }
    automore_disable();
}

/*
 * isis_rw_hit_element
 *
 * This function is called by recursive_walk() as a ordered traversal
 * occurs through an AVL tree.
 */
static boolean isis_rw_hit_element (isis_tree_type *element,
				    void *parm1,
				    void *parm2,
				    void *parm3,
				    void *parm4)
{

    isis_lspdbtype *lsp;

    lsp = element->lsp;

    printf("\n---> Hit node %s", lspid_string(&lsp->data->lspid));
    return(TRUE);

}

/*
 * show_isis_tree
 *
 * This function will show the linkage of the link state database AVL
 * structure. This function is used mainly to see if the AVL tree is
 * balanced (debugging purposes).
 */
void show_isis_tree (parseinfo *csb)
{

    clns_pdbtype *isis_pdb;
    int          level;

    if (!(isis_pdb = find_isis_pdb())){
	return;
    }
    level = GETOBJ(int,1);

    automore_enable(NULL);
    if ((level & ISIS_CIRCUIT_L1) && isis_pdb->level1_LSPs) {
	printf("\nIS-IS Level-1 AVL Tree");
	isis_recursive_walk(NULL, ISIS_WALK_SHOW_ALL, isis_pdb->level1_LSPs, 
			    isis_rw_hit_element, NULL, NULL, NULL, NULL);
    }
    if ((level & ISIS_CIRCUIT_L2) && isis_pdb->level2_LSPs) {
	printf("\nIS-IS Level-2 AVL Tree");
	isis_recursive_walk(NULL, ISIS_WALK_SHOW_ALL, isis_pdb->level2_LSPs, 
			    isis_rw_hit_element, NULL, NULL, NULL, NULL);
    }
    automore_disable();
}

/*
 * isis_checksum_lsp
 *
 * Calculate checksum for link state packet.
 *
 */
void isis_checksum_lsp (clns_pdbtype *pdb, isis_lspdbtype *lsp)
{

    uchar  *start;
    int    length;
    ushort checksum;

    /*
     * Value of checksum field must be 0.
     */
    PUTSHORT(lsp->data->checksum, 0);

    /*
     * Compute checksum over entire PDU starting after the system id
     * portion.
     */
    start = (((uchar *) &lsp->data->lspid) + STATION_LENGTH);
    length = isis_lspdb_data_length(lsp) - STATION_LENGTH;

    checksum = isis_compute_checksum(start, length, lsp->data->checksum, 
				     pdb->systemId_checksum[0],
				     pdb->systemId_checksum[1]);

    PUTSHORT(lsp->data->checksum, checksum);
}

/*
 * isis_compute_checksum
 *
 * Compute ISO checksum where values C0 and C1 are initialized
 * by caller.
 */
#define MODX 4102
ushort isis_compute_checksum (
    uchar *start,
    int length,
    uchar *csp,
    int c0,
    int c1)
{

    uchar  *p, *p1, *p2, *p3;
    int    iq, ir;
    ushort checksum;

    p = start;
    p1 = p;
    p3 = start + length;

    while (p1 < p3) {
	/* outer sum accumulation loop */
	p2 = p1 + MODX; 
	if (p2 > p3)
	    p2 = p3;
	for (p = p1; p < p2; p++) {
	    /* inner sum accumulation loop */
	    c0 = c0 + (*p); 
	    c1 = c1 + c0;
	}
	/* adjust accumulated sums to mod 255 */
	c0 = c0%255;
	c1 = c1%255;
	p1 = p2;
    }

    if (csp) {
	/* compute and insert checksum octets */
	iq = ((length-((csp-start)+1))*c0 - c1)%255;
	if (iq <= 0)
	    iq = iq + 255;

	ir = (510 - c0 - iq);
	if (ir > 255)
	    ir = ir - 255;
    } else {
	iq = c0;
	ir = c1;
    }
    checksum = (iq << 8) & 0xff00;
    checksum |= (ir & 0xff);

    return(checksum);
}

/*
 * isis_restart
 *
 * Clear the LSP databases and all adjacencies and restart with a minty-fresh
 * attitude.
 */
void isis_restart (clns_pdbtype *pdb)
{
    /* Clear the databases.  This will age the LSP chains as well. */

    isis_clear_database(pdb, pdb->level1_LSPs);
    pdb->level1_LSPs = NULL;
    isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_DBCHANGED, NULL, TRUE);
    isis_clear_database(pdb, pdb->level2_LSPs);
    pdb->level2_LSPs = NULL;
    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_DBCHANGED, NULL, TRUE);

    /* Blast the adjacencies. */

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	reg_invoke_clns_clear_neighbors(CLEAR_NBRS_ALL);
    } else {
	reg_invoke_ipx_nlsp_clear_neighbors(pdb);
	isis_mark_L1_changed(pdb);
	reg_invoke_ipx_nlsp_mark_all_L1_dr_changed(pdb);
    }
}

/*
 * isis_verify_database_checksum
 *
 * Verify the checksum on an LSP in memory.  Blow the LSP database if
 * the checksum fails.
 *
 * Returns TRUE if the checksum is OK, FALSE if not.
 */
boolean isis_verify_database_checksum (clns_pdbtype *pdb,
					      isis_lspdbtype *lsp)
{
    if (!verify_iso_checksum(&lsp->data->lspid,
			     isis_lspdb_data_length(lsp),
			     &lsp->data->checksum)) {
	errmsg(&msgsym(CKSUMERR, CLNS), pdb->errmsg_name,
	       lsp->level, lspid_string(&lsp->data->lspid));
	isis_restart(pdb);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * isis_rw_ager
 *
 * Age individual entries when called from isis_recursive_walk.
 *
 */
static boolean isis_rw_ager (isis_tree_type *element, void *pdb_parm,
			     void *dummy1, void *dummy2, void *dummy3)
{

    isis_lspdbtype *lsp;
    clns_pdbtype *pdb;
    
    pdb = pdb_parm;
    lsp = element->lsp;

    /*
     * Compute the checksum for the LSP.  If it fails, blow the LSP databases
     * and all adjacencies.
     */
    if (lsp && lsp->lsp_active) {
	return(isis_verify_database_checksum(pdb, lsp));
    }
    return(TRUE);
}

/*
 * isis_ager
 *
 * This function does the following:
 *
 * 1) Update the state of the partition avoidance variable.
 * 2) If it's time, check the LSP database for corruption.
 * 3) Age the level 1 routing table.
 *
 * Partition avoidance kludge:  This is special case code to detect the
 * case where a router has only a single L1 link to an area and its
 * interface goes down, but another router is still connected to the
 * area and both routers announce L2 reachability.  This is a partitioned
 * area, and can happen in certain topologies.  If the "partition-avoidance"
 * router subcommand has been enabled, we will stop sending IIHs when
 * we have no L1, IS, or ES adjacencies.
 *
 * This routine is run every few seconds for ISIS, and once per refresh
 * interval for NLSP.
 */
void isis_ager (clns_pdbtype *pdb)
{
    /* Restart the timer. */

    if (pdb->proc_type == PROC_TYPE_ISIS) {
	mgd_timer_start(&pdb->isis_age_timer, ISIS_AGE_INTERVAL);
    } else {
	mgd_timer_start(&pdb->isis_age_timer, pdb->lsp_refresh_interval);
    }

    /* Do partition avoidance for ISIS. */

    if (pdb->proc_type == PROC_TYPE_ISIS)
	reg_invoke_isis_do_partition_avoidance(pdb);
    
    /* Checksum the database if it's time. */

    if (pdb->proc_type == PROC_TYPE_NLSP || AWAKE(pdb->checksum_timer)) {
  	isis_recursive_walk(NULL, ISIS_WALK_ALL, pdb->level1_LSPs,
 			    isis_rw_ager, pdb, NULL, NULL, NULL);
	if (pdb->level & CLNS_LEVEL2) {
  	    isis_recursive_walk(NULL, ISIS_WALK_ALL, pdb->level2_LSPs,
 				isis_rw_ager, pdb, NULL, NULL, NULL);
	}
	TIMER_START(pdb->checksum_timer, pdb->lsp_refresh_interval);
	isis_trigger_spf(pdb, CLNS_LEVEL1, ISIS_SPF_TRIG_PERIODIC, NULL, TRUE);
 	if (pdb->proc_type == PROC_TYPE_ISIS) {
	    isis_trigger_spf(pdb, CLNS_LEVEL2, ISIS_SPF_TRIG_PERIODIC, NULL,
			     TRUE);
	}
   }

    /*
     * Process level-1 routing table.
     */
    if (pdb->proc_type == PROC_TYPE_ISIS)
	reg_invoke_isis_age_L1_routing_table(pdb);
}


/*
 * isis_free_resources
 *
 * Deallocates memory for both the level-1 and level-2 link state databases
 * as well as the level-1 forwarding table.
 */
void isis_free_resources (clns_pdbtype *pdb)
{

    isis_ddbtype *ddb, *next;
    int          i, j;

    /*
     * Free both ddb's and associated rdb's for each route in the level-1
     * routing table.
     */
    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	for (ddb = pdb->route_table[i]; ddb; ddb = next) {
	    next = ddb->next;
	    for (j = 0; j < ddb->route_count; j++) {
		if (!ddb->next_hops[j]) continue;
		free(ddb->next_hops[j]);
	    }
	    free(ddb);
	}
    }

    /*
     * Do the link state databases. Don't worry about balancing.
     */
    isis_clear_database(pdb, pdb->level1_LSPs);
    isis_clear_database(pdb, pdb->level2_LSPs);

    if (pdb->lspix_table) {
	free(pdb->lspix_table);
	pdb->lspix_table = NULL;
    }
}

/*
 * isis_clear_database
 *
 * Frees memory used by the AVL tree and the LSP themselves.
 *
 */
void isis_clear_database (clns_pdbtype *pdb, isis_tree_type *element)
{
    isis_lspix_type *lspix_entry;
    isis_lspdbtype *lsp_db;
    ushort lsp_index;

    if (!element)
	return;

    if (element->llink) {
	isis_clear_database(pdb, element->llink);
	element->llink = NULL;
    }

    if (element->rlink) {
	isis_clear_database(pdb, element->rlink);
	element->rlink = NULL;
    }

    /* First age the LSP entry to get rid of attached cruft. */

    lsp_db = element->lsp;
    if (lsp_db) {
	lsp_index = lsp_db->lsp_index;
	lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp_index);
	if (lspix_entry) {
	    lspix_entry->lsp_version++;	/* Make it old. */
	    isis_flush_lsp_backup_chains(pdb, lspix_entry);
	    isis_age_one_lsp_chain(pdb, lsp_index); /* And age it. */
	}
    }

    /* Now delete the LSP. */

    isis_delink_one_lsp(pdb, element);
    isis_free_avl_element(element);
    return;
}

/*
 * isis_new_es_adjacency
 *
 * Determine if IS-IS should be called to add an ES adjacency to its
 * pseudo-node LSP.
 */
void isis_new_es_adjacency (clns_pdbtype *pdb, clns_adjtype *adj)
{
    isisidbtype *isisidb;

    /*
     * If IS-IS not enabled on this interface, do nothing.
     */
    if (!itf_in_pdb(adj->idb, pdb))
	return;

    /*
     * For each area address associated with this ES, determine if
     * there is a common area address with IS-IS. If the last area
     * address in the ES adjacency matchs one of the area addresses
     * in IS-IS, this is the first time that the ES becomes active in
     * the area. IS-IS should report it.
     */
    if (isis_area_in_pdb(pdb, adj)) {
	isisidb = get_isis_idb(pdb, adj->idb);
	isis_mark_L1_changed(pdb);
	if (isisidb) {
	    isis_mark_L1_dr_changed(pdb, isisidb);
	    isis_trigger_dr_election(pdb, isisidb, ISIS_CIRCUIT_L1);
	}
    }
}

/*
 * isis_fork_processes
 *
 * Fire off IS-IS/NLSP processes
 */

static char *const adj_proc_names[ISIS_NUM_PROC_TYPE] = {
    "IS-IS Adjacency", "NLSP Adjacency"
    };
static char *const update_proc_names[ISIS_NUM_PROC_TYPE] = {
    "IS-IS Update", "NLSP Update"
    };

void isis_fork_processes(clns_pdbtype *pdb, isis_proc_type proc_type)
{
    /*
     * Create the adjacency process, if necessary.
     */
    if (pdb->clns_pid == NO_PROCESS) {
	pdb->clns_pid = process_create(isis_router, adj_proc_names[proc_type],
				       IPROUTING_STACK, PRIO_NORMAL);
	if (pdb->clns_pid != NO_PROCESS) {
	    process_set_arg_ptr(pdb->clns_pid, pdb);
	}
    }
    /*
     * Now create the update process, if necessary.
     */
    if (pdb->update_pid == NO_PROCESS) {
	pdb->update_pid = process_create(isis_update,
					 update_proc_names[proc_type],
					 IPROUTING_STACK, PRIO_NORMAL);
	if (pdb->update_pid != NO_PROCESS) {
	    process_set_arg_ptr(pdb->update_pid, pdb);
	}
    }
}

/*
 * ip_isis_pdb_init
 *
 * PDB initialization stub for godawful registry callback hack.
 */
static clns_pdbtype *ip_isis_pdb_init (char *router_name)
{
    return(isis_pdb_init(router_name, PROC_TYPE_ISIS));
}

/*
 *
 */
static void isis_init (subsystype *subsys)
{
    reg_add_ip_isis_pdb_init(ip_isis_pdb_init, "isis_pdb_init");
    reg_add_isis_valid_lsp_version(isis_valid_lsp_version,
				   "isis_valid_lsp_version");
}
/*
 * ISIS subsystem header
 */

#define ISIS_MAJVERSION 1
#define ISIS_MINVERSION 0
#define ISIS_EDITVERSION  1

SUBSYS_HEADER(isis, ISIS_MAJVERSION, ISIS_MINVERSION, ISIS_EDITVERSION,
	      isis_init, SUBSYS_CLASS_PROTOCOL, NULL, NULL);


