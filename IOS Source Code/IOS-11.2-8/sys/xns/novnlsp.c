/* $Id: novnlsp.c,v 3.11.4.11 1996/09/13 21:27:11 akr Exp $
 * $Source: /release/112/cvs/Xsys/xns/novnlsp.c,v $
 *------------------------------------------------------------------
 * novnlsp.c -- Novell IPX NLSP support
 *
 * June 1994, Mark Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: novnlsp.c,v $
 * Revision 3.11.4.11  1996/09/13  21:27:11  akr
 * CSCdi68918:  when enable ipx nlsp debug, system produce erroneous
 * message
 * Branch: California_branch
 * Spurious memory access.
 *
 * Revision 3.11.4.10  1996/09/09  15:38:12  mschaef
 * CSCdi68274:  IPX SAP table may be incomplete when NLSP in use
 * Branch: California_branch
 *
 * Revision 3.11.4.9  1996/08/11  15:14:14  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Change the csnp-interval NVGEN rules to conform to the new
 * approach in ISIS.
 *
 * Revision 3.11.4.8  1996/08/08  14:51:22  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.
 *
 * Revision 3.11.4.7  1996/08/08  05:19:19  mschaef
 * CSCdi31506:  NLSP Ager process remains when NLSP is disabled
 * Branch: California_branch
 *
 * Also:
 *
 *   - Convert to use new scheduler primitives
 *   - Remove cruft including unused variables/params
 *
 * Revision 3.11.4.6  1996/07/30  22:00:05  jserpa
 * CSCdi63900:  IPX does not accept  SAP type 0 advertisements
 * Branch: California_branch
 * Corrected to accept SAP type 0 advertisements, as well as get nearest
 * server requests.
 *
 * Revision 3.11.4.5  1996/06/28  23:33:52  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/06/03  20:23:55  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/05/09  14:50:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.4.2  1996/05/03  02:20:08  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.11.4.1  1996/03/18  22:53:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  08:00:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/13  02:15:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/03/07  11:22:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11  1996/03/09  18:13:46  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.10  1996/03/07  23:13:20  mschaef
 * CSCdi49527:  %SYS-3-CPUHOG from NLSP Update or IPX EIGRP process
 *
 * Revision 3.9  1996/03/05  23:12:31  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.8  1996/03/01  07:34:37  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.7  1996/02/19  21:32:59  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.6  1996/02/14  23:01:40  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.5  1996/01/22  07:53:11  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/05  04:57:31  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/11  04:55:28  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.2  1995/11/17  19:24:22  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:39:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/26  00:54:05  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.13  1995/10/21  19:03:37  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.12  1995/10/02  16:57:08  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *
 * Revision 2.11  1995/09/27  23:49:35  sluong
 * CSCdi41138:  NLSP doesnt take precedence over floating static route
 *
 * Revision 2.10  1995/09/23  21:43:19  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.9  1995/08/25  15:00:09  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.8  1995/08/25  05:40:26  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.7  1995/08/23  18:27:33  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.6  1995/07/01  03:39:21  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.5  1995/06/28 09:34:00  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/21  09:01:47  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/18  06:20:13  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:26:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:29:43  hampton
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
#include "config.h"
#include "mgd_timers.h"
#include "../os/signal.h"
#include "../if/network.h"
#include "logger.h"
#include "xns.h"
#include "novfast.h"
#include "novell.h"
#include "novroute.h"
#include "novnlsp.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_ipx.h"
#include "../clns/isis_inlines.h"
#include "ipx_registry.h"
#include "subsys.h"
#include "sr_novell_nlsp.h"

#undef NOVELL_DEBUG

pid_t novnlsp_ager_pid;
mgd_timer novnlsp_ager_mt;


/*
 * novnlsp_lsp_link_to_rdb
 *
 * Returns an RDB pointer, given a pointer to the LSP link information
 * embedded therein
 */
static inline novrdbtype *novnlsp_lsp_link_to_rdb (isis_lsp_linkage *link)
{
    novrdbtype *rdb;
    if (link == NULL)
	return(NULL);
    rdb = (novrdbtype *) ((uchar *)link - offsetof(novrdbtype, lsp_linkage));
    return(rdb);
}

/*
 * novnlsp_lsp_link_to_srdb
 *
 * Returns a SAP RDB pointer, given a pointer to the LSP link information
 * embedded therein
 */
static inline novell_sapentry_rdb *novnlsp_lsp_link_to_srdb 
    (isis_lsp_linkage *link)
{
    novell_sapentry_rdb *srdb;
    if (link == NULL)
	return(NULL);
    srdb = (novell_sapentry_rdb *)
	((uchar *)link - offsetof(novell_sapentry_rdb, lsp_linkage));
    return(srdb);
}

/*
 * novnlsp_validate
 *
 * Quick and dirty check of routing/service data structure integrity.
 * For debugging only ... system slows to a crawl as data size
 * increases. One call does it all.
 */
#ifdef NOVELL_DEBUG
void novnlsp_bkup_ndb_validate (novndbtype *ndb)
{
    boolean dump = FALSE;
    novndbtype *next_ndb;

    if (!validmem_complete(ndb)) {
	buginf("\nNLSP: bkup ndb @%x invalid", ndb);
	if (dump) crashdump(0);
	return;
    }
    if (!(ndb->flags & NET_NLSP)) {
	buginf("\nNLSP: bkup ndb (%x) not NLSP", ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (!(ndb->options & IPX_NDB_OPTION_VALID)) {
	buginf("\nNLSP: bkup ndb (%x) not valid", ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb->options & IPX_NDB_OPTION_DELETED) {
	buginf("\nNLSP: bkup ndb (%x) deleted", ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb->paths == 0) {
	if (ndb->path_list != NULL) {
	    buginf("\nNLSP: bkup ndb (%x) w/%d path(s) has non-null pathlist",
		   ndb->dstnet, ndb->paths);
	    if (dump) crashdump(0);
	}
    }
    if (ndb->paths > 0) {
	if (ndb->path_list == NULL) {
	    buginf("\nNLSP: bkup ndb (%x) w/%d path(s) has null pathlist",
		   ndb->dstnet, ndb->paths);
	    if (dump) crashdump(0);
	}
    }
    if (ndb->backup_info != NULL) {
	buginf("\nNLSP: bkup backup list non-null");
	if (dump) crashdump(0);
    }
    if (!ndb->protocol_info.nlsp_info.spf_pessimal) {
	buginf("\nNLSP: bkup ndb (%x) not pessimal",
	       ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb->backup_info != NULL) {
	buginf("\nNLSP: bkup ndb (%x) backup_info non-null", ndb->dstnet);
    }
    next_ndb = ndb->protocol_info.nlsp_info.spf_next;
    if (next_ndb == NULL) {
	buginf("\nNLSP: bkup ndb (%x) has null next ptr", ndb->dstnet);
	if (dump) crashdump(0);
	return;
    }
    if (!validmem_complete(next_ndb)) {
	buginf("\nNLSP: bkup ndb (%x) back ptr (%x) invalid",
	       ndb->dstnet, next_ndb);
	if (dump) crashdump(0);
    }
    if (ndb->dstnet != next_ndb->dstnet) {
	buginf("\nNLSP: bkup ndb (%x) ndb (%x) inconsistent",
	       ndb->dstnet, next_ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb != next_ndb->protocol_info.nlsp_info.spf_next) {
	buginf("\nNLSP: bkup ndb (%x) ptrs inconsistent (%x/%x)",
	       next_ndb->dstnet, next_ndb, ndb);
	if (dump) crashdump(0);
    }
    if (next_ndb->protocol_info.nlsp_info.spf_pessimal) {
	buginf("\nNLSP: bkup ndb (%x) not pessimal",
	       next_ndb->dstnet);
	if (dump) crashdump(0);
    }
}

void novnlsp_ndb_validate (novndbtype *ndb)
{
    boolean dump = FALSE;
    novndbtype *next_ndb;

    if (!validmem_complete(ndb)) {
	buginf("\nNLSP: ndb @%x invalid", ndb);
	if (dump) crashdump(0);
	return;
    }
    if (!(ndb->options & IPX_NDB_OPTION_VALID)) {
	buginf("\nNLSP: ndb (%x) not valid", ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb->options & IPX_NDB_OPTION_DELETED) {
	buginf("\nNLSP: ndb (%x) deleted", ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (ndb->paths == 0) {
	if (ndb->path_list != NULL) {
	    buginf("\nNLSP: ndb (%x) w/%d path(s) has non-null pathlist",
		   ndb->dstnet, ndb->paths);
	    if (dump) crashdump(0);
	}
    }
    if (ndb->paths > 0) {
	if (ndb->path_list == NULL) {
	    buginf("\nNLSP: ndb (%x) w/%d path(s) has null pathlist",
		   ndb->dstnet, ndb->paths);
	    if (dump) crashdump(0);
	}
    }
    if (ndb->protocol_info.nlsp_info.spf_pessimal) {
	buginf("\nNLSP: ndb marked pessimal, net %x",
	       ndb->dstnet);
	if (dump) crashdump(0);
    }
    next_ndb = ndb->protocol_info.nlsp_info.spf_next;
    if (next_ndb == NULL) {
	return;
    }
    if (!validmem_complete(next_ndb)) {
	buginf("\nNLSP: ndb (%x) bkup ptr (%x) invalid",
	       ndb->dstnet, next_ndb);
	if (dump) crashdump(0);
	return;
    }
    if (ndb != next_ndb->protocol_info.nlsp_info.spf_next) {
	buginf("\nNLSP: ndb (%x) bkup ndb ptrs inconsistent (%x/%x)",
	       next_ndb->dstnet, ndb, next_ndb);
	if (dump) crashdump(0);
    }
    if (ndb->dstnet != next_ndb->dstnet) {
	buginf("\nNLSP: ndb (%x) bkup ndb (%x) inconsistent",
	       ndb->dstnet, next_ndb->dstnet);
	if (dump) crashdump(0);
	return;
    }
    if (!next_ndb->protocol_info.nlsp_info.spf_pessimal) {
	buginf("\nNLSP: ndb (%x) bkup ndb not pessimal",
	       ndb->dstnet);
	if (dump) crashdump(0);
    }
    if (next_ndb->backup_info != NULL) {
	buginf("\nNLSP: ndb (%x) bkup ndb backup_info non-null", ndb->dstnet);
    }
}

void novnlsp_validate (void)
{
    int i;
    novndbtype *ndb, *pndb = NULL;
    novrdbtype *rdb;
    novell_sapentrytype *sap;
    novell_sapentry_rdb *srdb;
    novell_sapqueuetype *sapq;
    int rdb_count = 0;

    if (!novell_debug1) {
	return;
    }
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	ndb = (*novell_table)[i];
	while (ndb != NULL) {
	    if (!validmem_complete(ndb)) {
		buginf("\nnovnlsp_validate: Bad ndb (%x)", ndb);
		crashdump(0);
	    }
	    if (ndb->flags & NET_NLSP) {
		pndb = ndb->protocol_info.nlsp_info.spf_next;
		if (pndb != NULL) {
		    if (!validmem_complete(pndb)) {
			buginf("\nnovnlsp_validate: Bad spf next (%x/%x)",
			       ndb->dstnet, pndb);
			crashdump(0);
		    }
		    if (pndb->protocol_info.nlsp_info.spf_next != ndb) {
			buginf("\nnovnlsp_validate: Bad spf next next"
			       "(%x/%x/%x/%x)",
			       ndb->dstnet, ndb, pndb,
			       pndb->protocol_info.nlsp_info.spf_next);
			crashdump(0);
		    }
		}
		rdb = ndb->path_list;
		while (rdb != NULL) {
		    rdb_count++;
		    if (!validmem_complete(rdb)) {
			buginf("\nnovnlsp_validate: Bad rdb (%x/%x)",
			       ndb->dstnet, rdb);
			crashdump(0);
		    }
		    if (ndb->flags & NET_NLSP) {
			if (!validmem_complete(rdb->ndb)) {
			    buginf("\nnovnlsp_validate: Bad rdb ndb (%x/%x)",
				   rdb, ndb);
			    crashdump(0);
			}
		    }
		    rdb = rdb->next;
		}
		if (rdb_count != ndb->paths) {
		    buginf("\nnovnlsp_validate: Bad rdb count (%x/%d/%d)",
			   ndb->dstnet, ndb->paths, rdb_count);
		    crashdump(0);
		}
		if (pndb != NULL) {
		    rdb = pndb->path_list;
		    while (rdb != NULL) {
			rdb_count++;
			if (!validmem_complete(rdb)) {
			    buginf("\nnovnlsp_validate: Bad spf rdb (%x/%x)",
				   pndb->dstnet, rdb);
			}
			if (!validmem_complete(rdb->ndb)) {
			    buginf("\nnovnlsp_validate: Bad spf rdb ndb"
				   " (%x/%x)",
				   rdb, pndb);
			    crashdump(0);
			}
			rdb = rdb->next;
		    }
		    if (rdb_count != pndb->paths) {
			buginf("\nnovnlsp_validate: Bad rdb count (%x/%d/%d)",
			       pndb->dstnet, pndb->paths, rdb_count);
			crashdump(0);
		    }
		}
	    }
	    ndb = ndb->next;
	}
    }
    if (!novell_debug2) {
	return;
    }
    for (sapq = (novell_sapqueuetype *) novell_sapQ.qhead;
	 sapq; sapq = sapq->next) {
	sap = (novell_sapentrytype*) sapq->saps.flink;
	while (sap != NULL) {
	    if (!validmem_complete(sap)) crashdump(0);
	    srdb = sap->paths;
	    while (srdb != NULL) {
		if (!validmem_complete(srdb)) crashdump(0);
		srdb = srdb->next;
	    }
	    sap = sap->next;
	}
    }
}
#endif

/*
 * novnlsp_process_packet
 *
 * Process an incoming NLSP packet
 */
void novnlsp_process_packet (paktype *pak)
{
    novpdbtype *pdb;
    clns_pdbtype *cpdb;
    ipxhdrtype *ipx = (ipxhdrtype *) ipxheadstart(pak); 

    /* 
     * Do some further sanity checks 
     */ 
    if (ipx->pt != NOVELL_TYPE_NLSP) { 
	datagram_done(pak); 
	return;
    } 
    /*
     * There must be an IPX PDB, a CLNS PDB, and the CLNS PDB must be
     * marked as "running".  If so, enqueue the packet onto the CLNS
     * PDB.
     */
    pdb = novnlsp_find_pdb(pak->nif_input->pname);
    if (pdb != NULL) {			/* Got it */
	cpdb = pdb->osi_pdb;
	if (cpdb != NULL) {
	    if (clns_pdb_running(cpdb)) {
		process_enqueue(cpdb->clns_watched_inputq, pak);
		return;
	    }
	}
    }

    /* Problems.  Throw it on the floor. */

    datagram_done(pak);
    return;
}

/*
 * ipx_nlsp_get_peer_network
 *
 * returns the NLSP peer's network number for this packet.
 *
 * used by novell_media routines to form proper next hop map address
 * for dialer, frame relay, X25, SMDS, ATM interfaces which may require
 * mapping for ipx packets.
 */
ulong ipx_nlsp_get_peer_network (paktype *pak)
{
    clns_adjtype *adj;
    int          bucket;
    ulong        net;
    clns_pdbtype *pdb;
    novpdbtype   *ipx_pdb;

    adj = NULL;
    net = 0L;

    for (ipx_pdb = novpdbQ.qhead; ipx_pdb && !net; ipx_pdb = ipx_pdb->next) {
	if ((ipx_pdb->proctype == NET_NLSP) && ipx_pdb->osi_pdb) {
	    pdb = ipx_pdb->osi_pdb;
	    for (bucket = 0; (bucket < CLNS_ADJ_DB_SIZE) && !net; bucket++) {
		for (adj = pdb->adjacency_db[bucket]; adj; ) {
		    if (!pak->if_output || pak->if_output == adj->idb) {
			if (adj->adjacencyType == CLNS_NLSP_IS_ADJ_TYPE) {
			    net = adj->ipx_net;
			    break;
			}
		    }
		    adj = adj->next;
		}
	    }
	}
    }
    return(net);
}


/*
 * novnlsp_debug_command
 *
 * Parse and record target network in appropriate pdb.
 *
 * GETOBJ(int,1) = IPX network
 */
void novnlsp_debug_command (parseinfo *csb)
{
    novpdbtype *pdb, *rip_pdb;
    ulong net;
    int ipxnlsp_instances = 0;

    net = GETOBJ(int,1);
    rip_pdb = (novpdbtype *)&novrip_pdb;
    
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_NLSP)
	    continue;
	ipxnlsp_instances++;
	if (csb->sense) {
	    if (net == 0) {
		printf("\n%%IPX network number not specified");
		return;
	    }
	    pdb->debug_net = net;
	    pdb->debug_set = TRUE;
	    novellnlsp_debug = TRUE;
	} else {
	    pdb->debug_set = FALSE;
	    net = pdb->debug_net;
	    pdb->debug_net = 0;
	    novellnlsp_debug = FALSE;
	}
    }

    if (!ipxnlsp_instances) {
	printf("\n%%IPX NLSP process not found");
	return;
    }

    rip_pdb->debug_net = (novellnlsp_debug ? net : 0);
    rip_pdb->debug_set = novellnlsp_debug;
    printf("\nIPX NLSP debugging %sabled for net %x",
	   (novellnlsp_debug ? "en" : "dis"), net);
}


/*
 * novnlsp_command
 *
 * Generic command handling for IPX NLSP
 *
 */
void novnlsp_command (parseinfo *csb)
{
    clns_pdbtype *cpdb;
    novpdbtype *pdb;

    if (!novell_default_route) {
      if (!csb->nvgen) {
	printf("\n%%IPX NLSP commands not allowed, default route is disabled");
      }
    }
    pdb = csb->protocol;
    if (pdb == NULL)
	return;				/* Shouldn't happen */
    cpdb = pdb->osi_pdb;
    if (cpdb == NULL)
	return;				/* Shouldn't happen */

    if (!pdb->entered)			/* %%% */
	return;

    switch(csb->which) {

      case NOV_NLSP_ENABLE_SUMMARY:
	isis_ipx_enable_summary_command(csb, cpdb);
	break;

      case NOV_NLSP_ENABLE_NP_LSP_SUMMARY:
	isis_ipx_enable_lsp_summary_command(csb, cpdb);
	break;

      case NOV_NLSP_LSP_HASH_ENABLE:
 	isis_ipx_lsp_hash_enable_command(csb, cpdb);
 	break;
	
      case NOV_NLSP_LOG_ADJ_CHG:
	isis_log_adj_changes_command(csb, cpdb);
	break;

      case NOV_NLSP_AREA_ADDR:
	isis_ipx_area_cmd(cpdb, TRUE, csb->sense, csb->nvgen, csb->nv_command,
			  GETOBJ(int,1), GETOBJ(int,2));
	break;
	
      case NOV_NLSP_PRC_IVL:
	isis_prc_ivl_command(csb, cpdb);
	break;

      case NOV_NLSP_SPF_IVL:
	isis_spf_ivl_command(csb, cpdb);
	break;

      case NOV_NLSP_LSP_GEN:
	isis_lsp_gen_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
			     GETOBJ(int,1), GETOBJ(int,2), cpdb);
	break;
	
      case NOV_NLSP_MAX_LSP_GEN:
	isis_lsp_refresh_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
				 GETOBJ(int,1), cpdb);
	break;

      case NOV_NLSP_MAXAGE:
	isis_lsp_lifetime_command(TRUE, csb->sense, csb->nvgen,
				  csb->nv_command, GETOBJ(int,1), cpdb);
	break;

      case NOV_NLSP_LSP_MTU:
	isis_lspmtu_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
			    GETOBJ(int,1), cpdb);
	break;

      case NOV_NLSP_SYSID_DISPLAY:
	isis_sysid_display_command(csb, cpdb);
	break;
	
      default:
	break;
    }
}

/*
 * novnlsp_change_interface
 *
 * Brings an NLSP interface up or down
 *
 * "active" is TRUE if NLSP should be actively running on this interface.
 * "sense" is TRUE if NLSP should advertise this interface if not running
 * on it.  active && !sense is an invalid state.
 */
void novnlsp_change_interface (novpdbtype *pdb, nidbtype *nidb, boolean sense,
			       boolean active)
{
    /*
     * Only remove nlsp enable if we are the instance running on this interface
     */
    if (!strcasecmp(nidb->pname, pdb->pname))
	nidb->nov_lsp_enabled = active;
    nidb->nov_lsp_isis = FALSE;

    /* Update the RIP/SAP mode variables. */

    if (active) {
	novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
	novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
    } else {
	if (!strcasecmp(nidb->pname, pdb->pname)) {
	    novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_ON);
	    novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_ON);
	}
    }
    
    /*
     * If there's a PDB, do it.  If not, it'll happen when
     * the task starts up.
     */
    if (pdb) {
	ipx_nlsp_interface_init(nidb, pdb, sense, active);
    }
}

/* 
 * Start up NLSP
 */
static novpdbtype *novnlsp_start (char *router_tag) 
{
    novpdbtype *pdb = nlsp_pdb_init(router_tag, NULL);

    if (pdb) {
	pdb->running = TRUE;
	novell_nlsp_running++;
	/*
	 * Possibly trim maxpaths since NLSP supports only 4:
	 */
	novell_paths_command(FALSE, TRUE, novell_cfg_maxpaths);
	novnlsp_ager_create(); /* Create ager if appropriate */
    }
    return(pdb);
}

/*
 * nlsp_if_command
 *
 * Interface command handling for IPX NLSP
 *
 */
void nlsp_if_command (parseinfo *csb)
{
    idbtype *idb;
    nidbtype *nidb;
    novpdbtype *pdb;
    char *name;
    boolean change;
    boolean default_hello;
    boolean default_hello_mult;
    isisidbtype *isisidb;

    if (!novell_running || novell_shutting_down) {
	if (!csb->nvgen) {
	    printf("\n%%IPX not running");
	}
	return;
    }

    change = FALSE;

    /*
     * Screen for valid interface, network, and protocol:
     */
    idb = csb->interface;
    if (!idb) {
	return;				/* Shouldn't happen */
    }
    /*
     * These commands can only be applied to the primary nidb
     * for an interface. NLSP cannot run on a secondary nidb.
     */
    nidb = IPX_PRIMARY_NIDB(idb);
    if (!nidb) {
	if (!csb->nvgen)
	    printf("\n%%Specify an IPX network first");
	return;
    }
    /*
     * Configuration update handling:
     */
    if (csb->nvgen) {
	pdb = novnlsp_find_pdb(nidb->pname);
	if (!pdb || !pdb->running || !pdb->osi_pdb) /*No process or disabled */
	    return;
	if (!nidb->nov_lsp_enabled)	/* Not enabled on interface */
	    return;

	isisidb = get_isis_idb(pdb->osi_pdb, idb);
	if (!isisidb)			/* No interface... */
	    return;

	switch(csb->which) {

	  case IPX_NLSP_ENABLE:	
	    if (!nidb->nov_lsp_enabled) 
		return;

	    nv_write(TRUE, csb->nv_command);
	    nv_add((boolean)nidb->pname, " %s", nidb->pname);
	    nv_add(TRUE, " enable");

	    switch(nidb->lsp_rip_cfg) {
	    case IPX_RIPSAP_CONFIG_ON:
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " rip on");
		break;

	    case IPX_RIPSAP_CONFIG_OFF:
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " rip off");
		break;
	      
	    default:
		break;
	    }

	    switch(nidb->lsp_sap_cfg) {
	    case IPX_RIPSAP_CONFIG_ON:
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " sap on");
		break;

	    case IPX_RIPSAP_CONFIG_OFF:
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " sap off");
		break;
	      
	    default:
		break;
	    }
	    
	    if (isisidb->nlsp_explicit_cost) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " metric %d", 
		       isisidb->isis_L1_metric);
	    }

	    if (isisidb->cfg_pacing_interval) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " lsp-interval %d", 
		       isisidb->cfg_pacing_interval);
	    }

	    if (isisidb->isis_retrans_interval != ISIS_DEF_RETRANS_TIMER) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " retransmit-interval %d", 
		       isisidb->isis_retrans_interval/ONESEC);
	    }

	    default_hello = (isisidb->isis_L1_hello_interval == 
			     (IPX_NLSP_HELLO_IVL_DEFAULT * ONESEC));
	    if (!default_hello) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " hello-interval %d", 
			 isisidb->isis_L1_hello_interval/ONESEC);
	    }

	    if (isisidb->isis_L1_prio != IPX_NLSP_PRIO_DEFAULT) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " priority %d", isisidb->isis_L1_prio);
	    }

	    if (isisidb->csnp_ctl[0].csnp_interval != 
		isis_default_csnp_timer(idb, PROC_TYPE_NLSP)) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " csnp-interval %d", 
		       isisidb->csnp_ctl[0].csnp_interval / ONESEC);
	    }
	    default_hello_mult =
		(isisidb->isis_L1_hello_multiplier == ISIS_HT_MULTIPLIER);
	    if (!default_hello_mult) {
		nv_write(TRUE, csb->nv_command);
		nv_add((boolean)nidb->pname, " %s", nidb->pname);
		nv_add(TRUE, " hello-multiplier %d", 
		       isisidb->isis_L1_hello_multiplier);
	    }
	default:
	    break;
	}
	return;
    }

    /*
     * IPX NLSP interface command handling:
     */

    /*
     * Get router tag string.
     */
    name = GETOBJ(string,1);	
    if (nidb->nov_lsp_enabled) {
	if (strcasecmp(nidb->pname, strlen(name) ? name : NULL)) {
	    printf("\n%%NLSP:  Only one NLSP process is allowed on the interface");
	    return;
	}
    }
    if (csb->which == IPX_NLSP_ENABLE) {
	if (!novell_nlsp_enabled) {
	    printf("\n%% Error: NLSP is not included in this IOS build");
	    return;
	}
	if (nidb->nov_ipxwan_client || nidb->nov_ipxwan_static) {
	    printf("\n%%IPX NLSP not allowed on an IPXWAN Client or Static "
		   "interface.");
	    return;
	}
	if (!is_p2p(idb) && !idb_is_ieeelike(idb)) {
	    printf("\n%%IPX NLSP not allowed on non-LAN multipoint interfaces");
	    return;
	}
	if (csb->sense == nidb->nov_lsp_enabled)
	    return;
	pdb = novnlsp_find_pdb(name);
	if (csb->sense) {
	    /*
	     * The pdb might not exist yet. If so,
	     * create the pdb but mark it as NOT running, if "ipx router nlsp"
	     * command has not yet been entered.
	     */
	    if (pdb == NULL) {
		pdb = reg_invoke_ipx_nlsp_start(name);
	    } else {
		if (!pdb->running) {
		    if (pdb->restart) {
			(*pdb->restart)(pdb);
		    }
		}
		pdb->entered = TRUE;
	    }

	    if (!pdb || !pdb->osi_pdb) {
		printf("\n%% Error creating NLSP routing process");
		return;
	    }
	    isisidb = isis_alloc_isisidb(idb, PROC_TYPE_NLSP,
					 pdb->osi_pdb->index);
	    if (!isisidb) {
		printf("\n%%NLSP:  Can't allocate memory");
		return;
	    }
	    setstring(&nidb->pname, name);
	    novnlsp_change_interface(pdb, nidb, TRUE, csb->sense);
	} else {
	    novnlsp_change_interface(pdb, nidb, TRUE, csb->sense);
	    if (nidb->pname) {
		free(nidb->pname);
		nidb->pname = NULL;
	    }
	}
	change = TRUE;
    } else {
	if (!nidb->nov_lsp_enabled) {
	    printf("\n%%Enable NLSP on the interface first");
	    return;
	}
	pdb = novnlsp_find_pdb(nidb->pname);
	if (!pdb || !pdb->osi_pdb)
	    return;
	isisidb = get_isis_idb(pdb->osi_pdb, idb);
	if (!isisidb)
	    return;

	switch (csb->which) {

	case IPX_NLSP_RIP_MODE:
	    if (!csb->sense) {
		/*
		 * Default compatibility mode is AUTO:
		 */
		novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
	    } else {
		switch (GETOBJ(int,1)) {
		case IPX_RIPSAP_ON:
		    if (nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_ON) {
			change = TRUE;
			novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_ON);
		    }
		    break;
		case IPX_RIPSAP_OFF:
		    if (nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_OFF) {
			change = TRUE;
			novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_OFF);
		    }
		    break;
		case IPX_RIPSAP_AUTO:
		    if (nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_AUTO) {
			change = TRUE;
			novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
		    }
		    break;
		default:
		    printf("\n%%IPX unknown value for NLSP RIP mode");
		    break;
		}
	    }
	    break;
	
	case IPX_NLSP_SAP_MODE:
	    if (!csb->sense) {
		/*
		 * Default compatibility mode is AUTO:
		 */
		novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
	    } else {
		switch (GETOBJ(int,1)) {
		case IPX_RIPSAP_ON:
		    if (nidb->lsp_sap_cfg != IPX_RIPSAP_CONFIG_ON) {
			change = TRUE;
			novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_ON);
		    }
		    break;
		case IPX_RIPSAP_OFF:
		    if (nidb->lsp_sap_cfg != IPX_RIPSAP_CONFIG_OFF) {
			change = TRUE;
			novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_OFF);
		    }
		    break;
		case IPX_RIPSAP_AUTO:
		    if (nidb->lsp_sap_cfg != IPX_RIPSAP_CONFIG_AUTO) {
			change = TRUE;
			novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_AUTO);
		    }
		    break;
		default:
		    printf("\n%%IPX unknown value for NLSP SAP mode");
		    break;
		}
	    }
	    break;

	case IPX_NLSP_METRIC:
	    isisidb->nlsp_explicit_cost = csb->sense;
	    if (csb->sense) {		/* Explicit set */
		isisidb->isis_L1_metric = GETOBJ(int,1);
	    } else {			/* Implicit set */
		ipx_nlsp_bw_delay_changed(nidb);
	    }
	    ipx_mark_non_dr_info_change(pdb);
	    break;

	case IPX_NLSP_LSPIVL_CMD:
	    if (csb->sense) {
		isisidb->cfg_pacing_interval = 
		    isisidb->pacing_interval = GETOBJ(int,1);
	    } else {
		isisidb->cfg_pacing_interval = 0;
		ipx_nlsp_bw_delay_changed(nidb);
	    }
	    break;
	
	
	case IPX_NLSP_RETRANS_IVL:
	    if (csb->sense) {
		isisidb->isis_retrans_interval = GETOBJ(int,1)*ONESEC;
	    } else {
		isisidb->isis_retrans_interval = ISIS_DEF_RETRANS_TIMER;
	    }
	    break;
	
	
	case IPX_NLSP_HELLO_IVL:
	    isisidb->isis_L1_hello_interval = GETOBJ(int,1) * ONESEC;
	    isisidb->isis_L1_dr_hello_interval =
		isisidb->isis_L1_hello_interval / NLSP_LAN_DR_HELLO_DIVISOR;
	    break;
	
	case IPX_NLSP_HELLO_MULT:
	    if (csb->sense)
		isisidb->isis_L1_hello_multiplier = GETOBJ(int,1);
	    else
		isisidb->isis_L1_hello_multiplier = ISIS_HT_MULTIPLIER;
	    break;
	
	case IPX_NLSP_PRIO:
	    isisidb->isis_L1_prio = GETOBJ(int,1);
	    isisidb->isis_L1_cur_prio = GETOBJ(int,1);
	    isis_schedule_hello(pdb->osi_pdb, isisidb, ISIS_CIRCUIT_L1);
	    isis_trigger_dr_election(pdb->osi_pdb, isisidb, ISIS_CIRCUIT_L1);
	    break;

	case IPX_NLSP_CSNP_IVL:
	    isisidb->csnp_ctl[0].csnp_interval = GETOBJ(int,1) * ONESEC;
	    if (isisidb->isis_running) {
		mgd_timer_start(&isisidb->csnp_ctl[0].isis_csnp_timer,
				isisidb->csnp_ctl[0].csnp_interval);
	    }
	    break;

	default:
	    printf("\n%%IPX unknown NLSP interface command");
	    break;
	}
    }
    if (change) {
	/* Force world to know it's gone and bring IPXWAN down... */
	novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
    }
}

/*
 * nlsp_add_all_ifs
 *
 * Add all interfaces to the NLSP process.  This is called when the
 * PDB is allocated in order to catch all of the interfaces that were
 * configured for NLSP before the "ipx router nlsp" command was
 * entered.  This will also put non-NLSP interfaces in "passive" mode.
 */
void nlsp_add_all_ifs (novpdbtype *pdb)
{
    nidbtype *nidb;

    FOR_ALL_NIDBS(nidb) {
	if (!nidb->nov_lsp_isis) {	/* Not running IS-IS */
	    ipx_nlsp_interface_init(nidb, pdb, TRUE, nidb->nov_lsp_enabled);
	}
    }
}

/*
 * novnlsp_insert_rdb_in_lsp_list
 *
 */
static void novnlsp_insert_rdb_in_lsp_list (novrdbtype *rdb,
					    isis_lsp_linkage **lh)
{
    if ((rdb == NULL) || (lh == NULL)) {
	return;
    }
    if ((rdb->ndb == NULL) || !(rdb->ndb->flags & NET_NLSP)) {
	return;
    }
    isis_add_entry_to_lsp_chain(&rdb->lsp_linkage, lh);
}

/*
 * novnlsp_delete_rdb_from_lsp_list
 *
 * Note: May be called safely even if rdb is not on any lsp list.
 */
void novnlsp_delete_rdb_from_lsp_list (novrdbtype *rdb)
{
    if (rdb == NULL) {
	return;
    }
    isis_delete_entry_from_lsp_chain(&rdb->lsp_linkage);
}

/*
 * novnlsp_free_rdb_list
 *
 */
void novnlsp_free_rdb_list (novndbtype *ndb,
				   enum IPX_ROUTE_FREE tablestatus)
{
    novrdbtype *rdb, *next_rdb;

    rdb = ndb->path_list;
    ndb->path_list = NULL;
    while (rdb != NULL) {
	next_rdb = rdb->next;
	if (tablestatus == IPX_ROUTE_FREE_WAS_IN_TABLE) {
	    novell_mark_rdb_changed(ndb, rdb, ndb->flags,
				    IPX_ROUTE_EVENT_DELETED);
	}
	novrdb_free(rdb);
	rdb = next_rdb;
    }
    ndb->path_list = ndb->nextpath = NULL;
    ndb->paths = 0;
}
static void novnlsp_free_ndb (novndbtype *ndb,
			      enum IPX_ROUTE_FREE tablestatus)
{
    novnlsp_free_rdb_list(ndb, tablestatus);
    nov_ndb_free(ndb, tablestatus, FALSE);
}

/*
 * novnlsp_insert_srdb_in_lsp_list
 *
 */
void novnlsp_insert_srdb_in_lsp_list (novell_sapentry_rdb *srdb,
				      isis_lsp_linkage **lh)
{
    if ((srdb == NULL) || (lh == NULL)) {
	return;
    }
    if ((srdb->sap == NULL) || !(srdb->sap->flags & NET_NLSP)) {
	return;
    }
    isis_add_entry_to_lsp_chain(&srdb->lsp_linkage, lh);
}

/*
 * novnlsp_delete_srdb_from_lsp_list
 *
 * Note: May be called safely even if sap is not on any lsp list.
 */
void novnlsp_delete_srdb_from_lsp_list (novell_sapentry_rdb *srdb)
{
    if (srdb == NULL) {
	return;
    }
    isis_delete_entry_from_lsp_chain(&srdb->lsp_linkage);
}


/*
 * novnlsp_ager_schedule
 *
 * Run the ager process in a little while
 *
 */
void novnlsp_ager_schedule (void)
{
    if (novnlsp_ager_pid != NO_PROCESS) {
	if (!mgd_timer_running(&novnlsp_ager_mt)) {
	    mgd_timer_start(&novnlsp_ager_mt, ONESEC*20);
	}
    }
}


static void novnlsp_age_routes (void)
{
    int i;
    novndbtype *ndb, *last_ndb, *next_ndb;
    nov_urd_type *urd, *last_urd, *next_urd;
    novpdbtype *bi_pdb;
    backuptype *bi, *next_bi, *last_bi;
    ushort index, version;
    boolean first_time = TRUE;
    boolean debug = FALSE;
    int count = 0;

    if (!novell_nlsp_running) {
	return;
    }
    rn_walktree_blocking(nov_rnhead->rnh_treetop, age_route_patricia);
    for (i = 0; i < NOV_NETHASHLEN; i++) {
 	last_ndb = (novndbtype *)&(*novell_table)[i];
 	ndb = (*novell_table)[i];
 	while (ndb != NULL) {
 	    next_ndb = ndb->next;
 	    if (ndb->flags & NET_NLSP) {
		if (TIMER_RUNNING(ndb->holddownuntil)) {
		    
		    if (AWAKE(ndb->holddownuntil)) {
			
			debug =
			    (NOVNLSP_DEBUG(ndb->dstnet,
					   NOV_INDEX2PDB(ndb->novpdbindex)) &&
			     novell_private_debug);
			
			if (debug && (count++ < 10)) {
			    if (first_time) buginf("\nIPX-NLSP: ager");
			    first_time = FALSE;
			    buginf(" (%x)", ndb->dstnet);
			    if (count == 10) buginf(" ...");
			}
			
			last_ndb->next = ndb->next;
			
			novnlsp_free_ndb(ndb, IPX_ROUTE_FREE_WAS_IN_TABLE);
			ndb = NULL;
		    } else {
			novnlsp_ager_schedule();
		    }
		}
	    }
 	    if (ndb != NULL) {
 		last_ndb = ndb;
 	    }
 	    ndb = next_ndb;
	}
	process_may_suspend();
    }
    /*
     * Scrub (NLSP context) in the urd table:
     */
    if (nov_urd_table == NULL) {
 	return;
    }
    
    first_time = TRUE;
    count = 0;
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	
	last_urd = (nov_urd_type *) &(*nov_urd_table)[i];
	
	for (urd = (*nov_urd_table)[i]; urd; urd = next_urd) {
	    
	    next_urd = urd->next;
	    last_bi = (backuptype *)&urd->backup_info;
	    
	    /*
	     * For each backup record hanging off this URD ...
	     * preserve if valid, otherwise delete.
	     */
	    for (bi = urd->backup_info; bi; bi = next_bi) {
		
		next_bi = bi->next;
		bi_pdb = NOV_INDEX2PDB(bi->pdbindex);
		
		if (bi_pdb && (bi_pdb->proctype == NET_NLSP)) {
		    index = (bi->backup_context_info >> 16);
		    version = (bi->backup_context_info & 0x0000ffff);
		    if ((bi->route_type == RTYPE_L1) &&
			isis_valid_lsp_version(bi_pdb->osi_pdb,
					       index, version)) {
			/*
			 * Valid, keep it:
			 */
			last_bi = bi;
			
		    } else {
			
			debug = (NOVNLSP_DEBUG(urd->dstnet, bi_pdb) &&
				 novell_private_debug);
			
			if (debug && (count++ < 5)) {
			    if (first_time) buginf("\nIPX-NLSP: ager");
			    first_time = FALSE;
			    buginf(" (%x:{%d/%d})",
				   urd->dstnet, index, version);
			    if (count == 5) buginf(" ...");
			}
			
			/*
			 * Stale, delete it:
			 */
			last_bi->next = next_bi;
			bi->next = NULL;
			free(bi);
		    }
		}
	    }
	    
	    if (urd->backup_info != NULL) {
		last_urd = urd;
	    } else {
		last_urd->next = next_urd;
		urd->next = NULL;
		free(urd);
		novell_urd_scrub_count++;
	    }
	}
	process_may_suspend();
    }
}


/*
 * novnlsp_ager_teardown
 *
 * Return process resources prior to final kill
 *
 */
static void novnlsp_ager_teardown (int signal, int dummy1,
				   void *dummy2, char *dummy3)
{
    process_watch_mgd_timer(&novnlsp_ager_mt, DISABLE);
    novnlsp_ager_pid = NO_PROCESS;
}

/*
 * novnlsp_ager_process
 *
 * Perform IPX NLSP route table aging following SPF
 *
 */

static process novnlsp_ager_process (void)
{
    ulong major, minor;

    process_wait_on_system_init();

    signal_permanent(SIGEXIT, novnlsp_ager_teardown);
    process_watch_mgd_timer(&novnlsp_ager_mt, ENABLE);

    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor))

	    switch (major) {

	      case TIMER_EVENT:
		mgd_timer_stop(&novnlsp_ager_mt);
		novnlsp_age_routes();
		break;

	      default:
		break;
	    }
    }
}

/*
 * novnlsp_ager_create
 *
 * Create ager process if it does not already exist.
 *
 */
void novnlsp_ager_create (void)
{
    if (novnlsp_ager_pid == NO_PROCESS) {
	novnlsp_ager_pid = process_create(novnlsp_ager_process, "NLSP Ager",
					  NORMAL_STACK, PRIO_LOW);
    }
}


/*
 * novnlsp_ager_kill
 *
 * Remove ager process if no NLSP instances remain active.
 *
 * NOTE: be sure to decrement the instance count/boolean
 * (novell_nlsp_running) *before* calling this routine.
 *
 */
void novnlsp_ager_kill (void)
{
    if (!novell_nlsp_running) {
	process_kill(novnlsp_ager_pid);
    }
}

/*
 * kludge_enabled
 * Return TRUE if experimental 'hacks' to the protocol are enabled
 */
boolean ipx_kludge_enabled (void)
{
    return(ipx_kludge_enabled_flag == TRUE);
}

/*
 * novnlsp_display_lsp_rt_chain
 * novnlsp_display_lsp_sv_chain
 *
 * LSP chain related debug output
 *
 */
static void novnlsp_display_lsp_rt_chain (isis_lsp_linkage **lh, int maxcnt)
{
    novrdbtype *rdb;
    int cnt = 0;
    
    if (lh != NULL) {
	buginf(", lh=%x", lh);
	rdb = novnlsp_lsp_link_to_rdb(*lh);
	while ((rdb != NULL) && (cnt++ < maxcnt)) {
	    buginf("\n ->%x = %x<-[%x:%x]->%x)", rdb,
		   rdb->lsp_linkage.lsp_prev, rdb->ndb->dstnet,
		   rdb->ndb->flags, rdb->lsp_linkage.lsp_next);
	    rdb = novnlsp_lsp_link_to_rdb(rdb->lsp_linkage.lsp_next);
	}
	if (rdb == NULL) {
	    buginf(" ->NULL");
	} else {
	    buginf(" -> ...");
	}
    }
}
static void novnlsp_display_lsp_sv_chain (isis_lsp_linkage **lh, int maxcnt)
{
    novell_sapentry_rdb *srdb;
    int cnt = 0;
    
    if (lh != NULL) {
	buginf(", lh=%x", lh);
	srdb = novnlsp_lsp_link_to_srdb(*lh);
	while ((srdb != NULL) && (cnt++ < maxcnt)) {
	    buginf("\n ->%x = %x<-[%s:%x]->%x",
		   srdb, srdb->lsp_linkage.lsp_prev,
		   srdb->sap->server.server_name,
		   srdb->sap->flags, srdb->lsp_linkage.lsp_next);
	    srdb = novnlsp_lsp_link_to_srdb(srdb->lsp_linkage.lsp_next);
	}
	if (srdb == NULL) {
	    buginf(" ->NULL");
	} else {
	    buginf(" -> ...");
	}
    }
}

/*
 * novnlsp_newage_routes
 *
 * Discard paths from a given LSP which have been declared obsolete. An
 * obsolete path is one whose lsp version does not match the current
 * version as recorded in the rdb.
 *
 * If the path is obsolete and is the last path for a given route, check
 * if a valid pessimal route is known. (SPF will give us non-optimal
 * (pessimal) routes ... we keep the best pessimal route (if any)
 * in a "backup" ndb, pointed to by the "active" ndb. In other words,
 * we always try to keep the "best worst" path or paths for each route,
 * if SPF has given it to us. This allows us to handle routes that
 * go pessimal with relative grace.
 *
 */
void novnlsp_newage_routes (novpdbtype *pdb,
			    isis_lsp_linkage **lh,
			    ushort lsp_version,
			    ushort lsp_index)
{
    novndbtype *ndb, *last_ndb, *spf_ndb, *p;
    novrdbtype *rdb, *next_rdb, *last_rdb;
    nov_sumroutetype *sumroute;
    int i, path_index;
    backuptype *old_backup_info;
    boolean active_ndb;
    boolean debug, event_debug, private_debug;

    if (pdb == NULL) {
	if (novellnlsp_debug) buginf("NLSP: NULL pdb (age_routes)");
	return;
    }

    if (pdb->proctype != NET_NLSP) {
	if (novellnlsp_debug) buginf("NLSP: bad pdb (age_routes)"
				     ", type = %x", pdb->proctype);
	return;
    }

    event_debug = (novellnlsp_event_debug);
    private_debug = (event_debug && novell_private_debug);

    if (event_debug) {
	buginf("\nNLSP: Age routes, lsp {%d/%d}", lsp_index, lsp_version);
	if (private_debug) {
	    novnlsp_display_lsp_rt_chain(lh, 3);
	}
    }
    if (lh == NULL) {
	return;
    }
    rdb = novnlsp_lsp_link_to_rdb(*lh);
    while (rdb != NULL) {
	next_rdb = novnlsp_lsp_link_to_rdb(rdb->lsp_linkage.lsp_next);
	ndb = rdb->ndb;

	debug = NOVNLSP_DEBUG(ndb->dstnet, pdb);
	active_ndb = !ndb->protocol_info.nlsp_info.spf_pessimal;
	if (rdb->lsp_linkage.lsp_version != lsp_version) {
	    if (debug) {
		buginf("\nNLSP: Remove%s path to %x, via %x.%e"
		       ", lsp {%d/%d}, rdb {%d/%d}",
		       (active_ndb ? "" : " bkup"),
		       ndb->dstnet, rdb->srcnet, rdb->srchost,
		       lsp_index, lsp_version,
		       rdb->lsp_linkage.lsp_index,
		       rdb->lsp_linkage.lsp_version);
	    }
	    /*
	     * Find then delete this rdb from path list:
	     */
	    path_index = 1;
	    last_rdb = (novrdbtype *)&ndb->path_list;
	    while (last_rdb != NULL) {
		if (last_rdb->next == rdb) {
		    break;
		}
		last_rdb = last_rdb->next;
		path_index++;
	    }
	    /*
	     * If rdb not on path list, deal with it (should not happen):
	     */
	    if (last_rdb == NULL) {
		if (debug) buginf(" (no match)");
		if (ndb->paths > 0) {
		  ndb->paths--;
		}
		novrdb_free(rdb);
		rdb = next_rdb;
		continue;
	    }
	    /*
	     * We're about to delete the last rdb and there is no pessimal
	     * route. Do the redistribution now before the last rdb is removed.
	     * This is because isis_ipx_redist_callback() needs ndb->path_list
	     * in order to mark the dr_info_change.
	     * We need to redist this route if the optimal and pessimal routes
	     * are not learned from the same nlsp pdb.
	     */
	    if ((ndb->paths == 1) && active_ndb && 
		 !TIMER_RUNNING(ndb->holddownuntil) &&
		 (!ndb->protocol_info.nlsp_info.spf_next ||
		  (ndb->protocol_info.nlsp_info.spf_next->novpdbindex != 
		   ndb->novpdbindex))) {
		nov_network_redist(pdb, ndb, ROUTE_DOWN);
	    }
	    /* Found it ... do the delete */
	    last_rdb->next = rdb->next;
	    /*
	     * Delete the current (active) ndb if no paths remain.
	     * Possibly activate backup ndb if present.
	     */
	    if (debug) buginf(" (delete)");
	    if (active_ndb) {
		novell_mark_rdb_changed
		    (ndb, rdb, ndb->flags, IPX_ROUTE_EVENT_DELETED);
		/*
		 * Only invalidate cache if this path could be used:
		 */
		if (path_index <= novell_maxpaths) {
		    novcache_increment_version
			(NULL, NOVCACHE_NLSP_PATH_CHANGE, ndb->dstnet,
			 ndb->rt_mask);
		}
	    }
	    novrdb_free(rdb);

	    if (ndb->paths > 0) {
		ndb->paths--;
	    }
	    if (ndb->paths > 0) {
		/*
		 * Alternate paths present:
		 */
		rdb = next_rdb;
		continue;
	    } else {
		/*
		 * That's all folks! This route is gone. Do we have
		 * a pessimal (backup) route to take it's place?
		 */

		spf_ndb = ndb->protocol_info.nlsp_info.spf_next;
		
		if (active_ndb) {
		    if (spf_ndb == NULL) {
			/*
			 * Active route, no known pessimal. Signal down:
			 */
			if (!TIMER_RUNNING(ndb->holddownuntil)) {
			    if (debug) buginf(" (unreachable)");
			    novcache_increment_version
				(NULL, NOVCACHE_NLSP_UNREACHABLE, ndb->dstnet,
				 ndb->rt_mask);
			    /*
			     * This route is history! Place in brief
			     * holddown and schedule ager:
			     */
			    TIMER_START(ndb->holddownuntil, ONESEC*2);/* %%% */
			    novnlsp_ager_schedule();
			    rdb = next_rdb;
			    continue;
			}
		    } else {
			/*
			 * Delete active ndb. A pessimal (backup) route
			 * is present. Try it, we might like it!
			 */
			if (ndb->rt_mask == NOV_NETMASK_ALL) {
			    i = NOV_NETHASH(ndb->dstnet);
			    last_ndb = (novndbtype *)&(*novell_table)[i];
			    p = (*novell_table)[i];
			    for (; p; last_ndb = p, p = p->next) {
				if (p->dstnet == ndb->dstnet) {
				    break;
				}
			    }
			    /*
			     * Continue if not found (should not happen)
			     */
			    if (p == NULL) {
				if (debug) {
				    buginf("\nNLSP: ndb not found, net %x",
					   ndb->dstnet);
				}
				rdb = next_rdb;
				continue;
			    }
			    /*
			     * Install in table, mark as active 
			     * and redistribute.
			     * Also, compensate total route counter for the
			     * substitution.
			     *
			     * %%% ??? Check first if still good ??? %%%
			     */
			    last_ndb->next = spf_ndb;
			    spf_ndb->next = p->next;
			} else {
			    if (!(sumroute = (nov_sumroutetype *)
				  rn_delete(&ndb->keylen, &ndb->masklen, 
					    nov_rnhead))) {
				buginf("\nIPX: novnlsp_newage_routes did not delete %x %x properly",
				       ndb->dstnet, ndb->rt_mask);
			    } 
			    sumroute->ndb = spf_ndb;
			    if (!rn_addroute(&spf_ndb->keylen, 
					     &spf_ndb->masklen, 
					     nov_rnhead, sumroute->rt_nodes)) {
				buginf("\nNLSP: novnlsp_newage_routes did not add %x %x properly",
				       spf_ndb->dstnet, spf_ndb->rt_mask);
			    }
			}
			old_backup_info = ndb->backup_info;
			ndb->backup_info = NULL;
			ndb->protocol_info.nlsp_info.spf_next = NULL;
			novell_route_count++;
			novnlsp_free_ndb(ndb, IPX_ROUTE_FREE_WAS_IN_TABLE);
			spf_ndb->backup_info = old_backup_info;
			spf_ndb->protocol_info.nlsp_info.spf_pessimal = FALSE;
			spf_ndb->protocol_info.nlsp_info.spf_next = NULL;
			if (debug) {
			    buginf("\nNLSP: Install bkup route to %x"
				   " @[M%u/T%u/H%u][Ext T%u/H%u]"
				   ", flags %x, %d path(s)",
				   spf_ndb->dstnet, spf_ndb->path_metric,
				   spf_ndb->delay, spf_ndb->hopcount,
				   spf_ndb->ext_delay, spf_ndb->ext_hop,
				   spf_ndb->flags, spf_ndb->paths);
			}
			nov_network_redist(pdb, spf_ndb, ROUTE_MODIF);
			novcache_increment_version(NULL,
						   NOVCACHE_NLSP_WORSE_METRIC,
						   spf_ndb->dstnet, 
						   spf_ndb->rt_mask);
			rdb = next_rdb;
			continue;
		    }
		} else {
		    /*
		     * Pessimal (backup) route is obsolete ... remove silently:
		     */
		    if (debug) {
			buginf(" (remove bkup)");
		    }
		    if (spf_ndb != NULL) {
			spf_ndb->protocol_info.nlsp_info.spf_next = NULL;
		    }
		    ndb->protocol_info.nlsp_info.spf_next = NULL;
		    novell_route_count++;
		    novnlsp_free_ndb(ndb, IPX_ROUTE_FREE_NEVER_IN_TABLE);
		    rdb = next_rdb;
		    continue;
		}
	    }
	}
	rdb = next_rdb;
    }
    if (event_debug) {
	buginf("\nNLSP: Age routes, lsp {%d/%d}", lsp_index, lsp_version);
	if (private_debug) {
	    novnlsp_display_lsp_rt_chain(lh, 3);
	}
	buginf(" (Exit)");
    }
}

/*
 * novnlsp_newage_services
 *
 */
void novnlsp_newage_services (novpdbtype *pdb,
			      isis_lsp_linkage **lh,
			      ushort lsp_version,
			      ushort lsp_index)
{
    novell_sapentry_rdb *srdb, *last_srdb, *next_srdb;
    novell_sapentrytype *sap;
    boolean debug, event_debug, private_debug;

    event_debug = (novellnlsp_event_debug);
    private_debug = (event_debug && novell_private_debug);

    if (event_debug) {
	buginf("\nNLSP: Age srvces, lsp {%d/%d}", lsp_index, lsp_version);
	if (private_debug) {
	    novnlsp_display_lsp_sv_chain(lh, 3);
	}
    }

    srdb = novnlsp_lsp_link_to_srdb(*lh);
    while (srdb != NULL) {
	debug = FALSE;
	next_srdb = novnlsp_lsp_link_to_srdb(srdb->lsp_linkage.lsp_next);
	sap = srdb->sap;

	if ((sap->flags & NET_NLSP) && (sap->novpdbindex == pdb->index)) {
	    debug = NOVNLSP_DEBUG(sap->server.dnet, pdb);
	    if (srdb->lsp_linkage.lsp_version != lsp_version) {
		if (debug) buginf("\nNLSP: Age sap %s, net %x"
				  ", lsp {%d/%d}, srdb {%d/%d} via %x.%e",
				  sap->server.server_name, sap->server.dnet,
				  lsp_index, lsp_version,
				  srdb->lsp_linkage.lsp_index,
				  srdb->lsp_linkage.lsp_version,
				  srdb->snet, srdb->srchost);
		/*
		 * Find then delete this srdb from path list:
		 */
		last_srdb = (novell_sapentry_rdb *)&sap->paths;
		while (last_srdb != NULL) {
		    if (last_srdb->next == srdb) {
			break;
		    }
		    last_srdb = last_srdb->next;
		}
		if (last_srdb == NULL) {
		    if (debug) buginf(" (no match)");
		    srdb = next_srdb;
		    continue;
		}
		if (debug) buginf(" (delete)");
		last_srdb->next = srdb->next;
		if (sap->pathcount > 0) {
		    sap->pathcount--;
		}
		novell_mark_srdb_changed(sap, srdb, sap->flags,
					 IPX_ROUTE_EVENT_DELETED);
		novsrdb_free(srdb);
		if (sap->paths == NULL) {
		    if (debug) buginf("(poison)");
		    novell_sap_poison(sap);
		    /*
		     * Run ager to scrub urd table:
		     */
		    novnlsp_ager_schedule();
		}
	    }
	}
	srdb = next_srdb;
    }
    if (event_debug) {
	buginf("\nNLSP: Age srvces, lsp {%d/%d}", lsp_index, lsp_version);
	if (private_debug) {
	    novnlsp_display_lsp_sv_chain(lh, 3);
	}
	buginf(" (Exit)");
    }
}

novrdbtype *novnlsp_update (novpdbtype *pdb,
			       ulong dstnet,
			       ulong netmask,
			       ulong snet,
			       uchar *shost,
			       nidbtype *nidb,
			       nov_cost_param_type *cost,
			       ulong flags,
			       isis_lsp_linkage **lh,
			       ushort lsp_version,
			       ushort lsp_index)
{
    novndbtype *p, *new_p, *old_p, *last_p, *next_p;
    int i;
    novrdbtype *pp, *last_pp, *temp_pp;
    novpdbtype *old_pdb = NULL;
    boolean old_rip, old_xroute, old_nlsp, old_eigrp;
    boolean new_xroute, new_nlsp;
    boolean reachable;
    int path_index;
    boolean debug, event_debug, private_debug;
    nov_sumroutetype *rnnode;
    backuptype backup, *old_backup_info, *bi;

    /*
     * Generic screening:
     */
    if (!validmem(nidb)) {
	if (novellnlsp_debug) {
	    buginf("\nNLSP: Update nidb bad (net %x)", dstnet);
	}
	return(NULL);
    }
    if (pdb == NULL) {
	if (novellnlsp_debug) buginf("\nNLSP: Update pdb is NULL");
	return(NULL);
    }
    if (pdb->proctype != NET_NLSP) {
	if (novellnlsp_debug) buginf("\nNLSP: Update pdb not NLSP");
	return(NULL);
    }
    backup.pdbindex = pdb->index;
    backup.backup_context_info = ((lsp_index << 16) | (lsp_version));
    backup.route_type = RTYPE_L1;
    debug = NOVNLSP_DEBUG(dstnet, pdb);
    event_debug = novellnlsp_event_debug;
    private_debug = (event_debug && novell_private_debug);

    /*
     * Sanity check this request:
     */
    if (debug) {
	buginf("\nNLSP: Route to %x %x @[M%u/T%u/H%u][Ext T%u/H%u]"
	       ", via %x.%e, flags %x, lsp {%d/%d}",
	       dstnet, netmask, cost->metric, cost->delay, cost->hops,
	       cost->ext_delay, cost->ext_hops, snet, shost, flags,
	       lsp_index, lsp_version);
	if (private_debug) {
	    novnlsp_display_lsp_rt_chain(lh, 1);
	}
	if (novellnlsp_debug && novell_verbose_debug) {
	    buginf("\nNLSP: OOP=%u:%u:%u:%e:%u:%e",
		   cost->oop.total_throughput,
		   cost->oop.total_delay,
		   cost->oop.mtu,
		   &cost->oop.system_id[0],
		   cost->oop.circuit_id,
		   &cost->oop.mac_addr[0]);
	}
    }
    /* Immediate bail if network 0/-1 (and -2 and no rip default) */
    /* We will not allow default route handling to be disabled if */
    /* NLSP is running so special handling of -2 is being removed */
    if (NOV_ILLEGAL_NET(dstnet) && !ZERO_PREFIX(dstnet, netmask)) {
	if (novellnlsp_debug)
	    buginf(", ignoring 0/-1");
	return(NULL);
    }
    new_xroute = (flags & NET_XROUTE);
    new_nlsp = (!new_xroute);

    reachable = !((new_nlsp && (cost->metric == METRIC_INACCESSIBLE)) ||
		  (new_xroute && (cost->hops >= novell_infinity)));
    if (!reachable) {
	if (debug) buginf(" (unreachable)");
	return(NULL);
    }
    
    last_p = NULL;
    if (netmask == NOV_NETMASK_ALL) {
	i = NOV_NETHASH(dstnet);
	last_p = (novndbtype *)&(*novell_table)[i];
	p = (*novell_table)[i];
	for (; p; last_p = p, p = p->next)
	    if (p->dstnet == dstnet)
		break;
    } else {
	p = novrip_find_rt(dstnet, netmask, IPX_ROUTE_MATCH_EXACT);
    }
    /*
     * Not found, enter it.
     */
    if (p == NULL) {
        p = nov_ndb_malloc();
        if (p == NULL)
            return(NULL);
        pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
        if (pp == NULL) {
            nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	    return(NULL);
        }
	p->rt_mask = netmask;
	p->keylen = p->masklen = IPXRADIXKEYLEN;
        p->path_list = p->nextpath = pp;
        p->dstnet = dstnet;
        TIMER_STOP(p->holddownuntil);
        p->hopcount = cost->hops;
	p->area_count = cost->area_count;
        p->delay = p->last_delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
        p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
	p->options = IPX_NDB_OPTION_VALID;
	p->novpdbindex = pdb->index;

        pp->nidb = nidb;
	pp->ndb = p;
	pp->lsp_linkage.lsp_version = lsp_version;
	pp->lsp_linkage.lsp_index = lsp_index;
        pp->srcnet = snet;
        ieee_copy(shost, pp->srchost);
        GET_TIMESTAMP(pp->timer);
	bcopy(&cost->oop, &pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
        p->paths = 1;
	if (netmask == NOV_NETMASK_ALL) {
	    i = NOV_NETHASH(dstnet);
	    p->next = (*novell_table)[i];
	    (*novell_table)[i] = p;
	} else {
	    rnnode = malloc(sizeof(nov_sumroutetype));
	    if (!rnnode) {
		free(pp);
		nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		return(NULL);
	    }
	    rnnode->ndb = p;
	    if (!rn_addroute(&p->keylen, &p->masklen, nov_rnhead, 
			    rnnode->rt_nodes)) {
		buginf("\nNLSP: novnlsp_update did not add %x %x properly",
		       dstnet, netmask);
	    }
	}
        novell_routes++;
        p->flags = pdb->proctype | flags; /* Record protocol and attributes */
	p->procmask = pdb->mask;   /* Record which instance of protocol ... */
	pp->procmask = pdb->mask;  /* ... for both network and path desc    */
        if (debug) {
	    buginf("\nNLSP: Create route to %x %-8x "
		   "@[M%u/T%u/H%u][Ext T%u/H%u]"
		   ", via %x.%e, flags %x, lsp {%d/%d}, total=%d",
		   dstnet, netmask, cost->metric, cost->delay, cost->hops,
		   cost->ext_delay, cost->ext_hops, snet, shost,
		   p->flags, lsp_index, lsp_version, novell_routes);
	}
	novnlsp_insert_rdb_in_lsp_list(pp, lh);
	novcache_increment_version(NULL, NOVCACHE_NLSP_NEW_ROUTE, dstnet, 
				   netmask);
	nov_network_redist(pdb, p, ROUTE_UP);
	novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_ADD);

        return(pp);
    }

    /*
     * Don't override static/connected/internal routes, ever!
     */
    if (p->flags & (NET_STATIC | NET_CONNECTED | NET_INTERNAL) &&
	!(p->flags & NET_FLOATING_STATIC)) {
	if (debug)
	   buginf("\nNLSP: Net %x %x not added, entry in table"
		  " is %s", dstnet, netmask,
		  ((p->flags & NET_STATIC) ? "static" :
		   ((p->flags & NET_CONNECTED) ? "connected" : "internal")));
	ipx_insert_backup_info(&(p->backup_info), &backup, pdb, debug);
	return(NULL);
    }

    /*
     * Route entry present ... analyze:
     */
    old_pdb = nov_proctable[p->novpdbindex];
    old_rip = (p->flags & NET_RIP);
    old_xroute = (p->flags & NET_XROUTE);
    old_nlsp = ((p->flags & NET_NLSP) && !old_xroute);
    old_eigrp = (p->flags & NET_IGRP);
    if (old_eigrp && (cost->hops >= p->ext_hop)) {
	if (novellnlsp_debug) {
	    buginf("\nNLSP: %x not added, EIGRP ext hop count %d"
		   ", NLSP hop count in update %d",
		   dstnet, p->ext_hop, cost->hops);
	}
	ipx_insert_backup_info(&(p->backup_info), &backup, pdb, debug);
	return(NULL);
    }
    /*
     * Equal metric, add multiple path.
     *
     * Route is equal if ...
     *
     * - old and new routes are NLSP with same cost.
     * - old and new routes are XROUTE with same external delay/hopcount
     *   and same cost. The cost for the network is the cost of the best
     *   RDB.
     * - for address summary, old and new routes with the same ext ticks
     *
     * An old RIP route can never be equal to an NLSP or XROUTE. We either
     * prefer the NLSP route, prefer the XROUTE because it has the best
     * external metric in the area, or prefer this RIP route because it
     * has the best or is equal to the best external metric in the area.
     */
    if ((netmask == NOV_NETMASK_ALL && old_nlsp && new_nlsp && 
	 (p->path_metric == cost->metric)) ||
	((p->ext_delay == cost->ext_delay) && (netmask != NOV_NETMASK_ALL)) ||
	(old_xroute && new_xroute && (p->paths != 0) &&
	 NOVELL_EQUAL_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p) &&
	 (p->path_metric == cost->metric))) {
	TIMER_STOP(p->holddownuntil);
        p->hopcount = cost->hops;
	p->area_count = cost->area_count;
        p->delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
	p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
	/*
	 * Search for this path in path list. Update entry if found.
	 */
	last_pp = (novrdbtype *)&p->path_list;
 	pp = p->path_list;
	while (pp) {
	    /* One host, one vote */
	    if ((pp->nidb == nidb) &&
   		(pp->lsp_linkage.lsp_index == lsp_index) &&
   		(pp->srcnet == snet) &&
   		(bcmp(pp->srchost, shost, IEEEBYTES) == 0)) {
 		if (NLSP_SAME_COST(&cost->oop,
 				   &pp->protocol_info.nlsp_info.cost)) {
 		    GET_TIMESTAMP(pp->timer);
 		    pp->lsp_linkage.lsp_version = lsp_version;
 		    if (debug) buginf(" (refresh)");
 		    return(pp);
 		} else {
 		    /*
 		     * Order of preference has changed. Delete and
 		     * treat as new path.
 		     */
 		    if (debug) buginf(" (modif oop)");
		    novcache_increment_version
			(NULL, NOVCACHE_NLSP_PATH_CHANGE, dstnet, netmask);
		    novell_mark_rdb_changed
			(p, pp, p->flags, IPX_ROUTE_EVENT_DELETED);
 		    last_pp->next = pp->next;
 		    novrdb_free(pp);
 		    p->paths--;
 		    break;
 		}
 	    }
 	    last_pp = pp;
 	    pp = pp->next;
 	}
	/*
	 * Not found ... new entry ... add it. The new path will be sorted
	 * in according to NLSP "order of preference of routes". (See NLSP
	 * spec, section 6-2). Keep everything ... there is no longer an
	 * ager ... NLSP keeps all equal cost paths.
	 */
	path_index = 1;
	last_pp = (novrdbtype *)&p->path_list;
	pp = p->path_list;
        while ((pp != NULL) &&
	       NLSP_WORSE_COST(&cost->oop,
			       &pp->protocol_info.nlsp_info.cost)) {
	    last_pp = pp;
	    pp = pp->next;
	    path_index++;
	}
	temp_pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
	if (temp_pp == NULL) {
	    ipx_insert_backup_info(&(p->backup_info), &backup, pdb, debug);
	    return(NULL);
	}
        temp_pp->nidb = nidb;
        temp_pp->srcnet = snet;
        ieee_copy(shost, temp_pp->srchost);
        GET_TIMESTAMP(temp_pp->timer);
	temp_pp->procmask = pdb->mask;
	temp_pp->ndb = p;
	temp_pp->lsp_linkage.lsp_version = lsp_version;
	temp_pp->lsp_linkage.lsp_index = lsp_index;
	bcopy(&cost->oop, &temp_pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
	last_pp->next = temp_pp;
	if (pp != NULL) {
	    temp_pp->next = pp;
	}
	p->nextpath = p->path_list;
	p->paths++;
	p->flags = pdb->proctype | flags | (p->flags);
	p->procmask = pdb->mask;
	if (debug) buginf(" (path %d)", p->paths);
	novnlsp_insert_rdb_in_lsp_list(temp_pp, lh);
	/*
	 * Only invalidate cache if this path will actually be used:
	 */
	if (path_index <= novell_maxpaths) {
	    novcache_increment_version
		(NULL, NOVCACHE_NLSP_PATH_CHANGE, dstnet, netmask);
	}
	novell_mark_rdb_changed(p, temp_pp, p->flags, IPX_ROUTE_EVENT_ADD);
	return(temp_pp);
    } 
    /*
     * Handle better metric:
     *
     * Route is better if ...
     *
     * - old route in holddown, new route is anything coming in this routine
     * - old route is FLOATING, new route is anything coming in this routine
     * - old route is EIGRP, new route is anything coming in this routine
     * - old route is RIP, new route is NLSP
     * - old route is RIP, new route is XROUTE with better ext ticks/hops
     * - old route is XROUTE, new route is XROUTE with better ext ticks/hops
     * - old route is XROUTE, new route is NLSP
     * - old route is NLSP, new route is NLSP with better metric
     * - for address summary, new route with better ext ticks
     */
    if (TIMER_RUNNING(p->holddownuntil) ||
	(p->flags & NET_FLOATING_STATIC) || (old_eigrp) ||
	((p->ext_delay > cost->ext_delay) && (netmask != NOV_NETMASK_ALL)) ||
	(old_rip && new_nlsp) ||
	(old_rip && new_xroute &&
	 NOVELL_BETTER_ROUTE_INFO(cost->ext_hops, cost->ext_delay, p)) ||
	(old_xroute && new_nlsp) ||
	(old_xroute && new_xroute &&
	 NOVELL_BETTER_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p)) ||
	(old_xroute && new_xroute &&
	 NOVELL_EQUAL_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p) &&
	 (cost->metric < p->path_metric)) ||
	(netmask == NOV_NETMASK_ALL && old_nlsp && new_nlsp && 
	 (cost->metric < p->path_metric))) {
        if (debug) {
            buginf("\nNLSP: better route @[M%u/T%u/H%u][Ext T%u/H%u]"
		   " for %x via %x.%e", cost->metric, cost->delay, cost->hops,
		   cost->ext_delay, cost->ext_hops, dstnet, snet, shost);
	    buginf("\n      flushing %d path(s) @[M%u/T%u/H%u][Ext T%u/H%u]",
		   p->paths, p->path_metric, p->delay, p->hopcount,
		   p->ext_delay, p->ext_hop);
	}
	if (old_pdb != pdb) {
	    if (old_pdb->lost_route)
	      (*old_pdb->lost_route)(old_pdb, p);
	    nov_network_redist(old_pdb, p, ROUTE_DOWN);
	}
        pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
        if (pp == NULL) {
    	    ipx_insert_backup_info(&(p->backup_info), &backup, pdb, debug);
            return(NULL);
	}
	/*
	 * Current route info will become pessimal (if valid)
	 * and current pessimal (if present) will be dumped.
	 */
	GET_TIMESTAMP(p->ndb_changed);
	new_p = next_p = NULL;
	
	if (TIMER_RUNNING(p->holddownuntil)) {
	    if (debug) buginf(" (holddown)");
	    /*
	     * Route is in holddown. Basically, usurp this
	     * ndb and convert it into a valid NLSP route.
	     */
	    novnlsp_free_rdb_list(p, IPX_ROUTE_FREE_NEVER_IN_TABLE);
	    p->protocol_info.nlsp_info.spf_next = NULL;
	    p->protocol_info.nlsp_info.spf_pessimal = FALSE;
	} else {
	    /*
	     * Current route is active.
	     */
	    if (p->flags & NET_NLSP) {
		if (debug) buginf(" (active->bkup)");
		/*
		 * Active, NLSP: this ndb will become the new bkup ndb.
		 * Place a new active ndb in the route table. Delete
		 * current bkup if present, after capturing its
		 * context in the backup info list.
		 */
		new_p = nov_ndb_malloc();
		if (new_p == NULL) {
		    ipx_insert_backup_info(&(p->backup_info), &backup,
					   pdb, debug);
		    return(NULL);
		}
		novell_route_count--;
		new_p->path_list = NULL;
		new_p->dstnet = dstnet;
		new_p->rt_mask = netmask;
		new_p->keylen = new_p->masklen = IPXRADIXKEYLEN;
		new_p->protocol_info.nlsp_info.spf_next = p;
		new_p->protocol_info.nlsp_info.spf_pessimal = FALSE;
		new_p->backup_info = p->backup_info;
		if (netmask == NOV_NETMASK_ALL) {
		    new_p->next = p->next;
		    last_p->next = new_p;
		} else {
		    if (!(rnnode = (nov_sumroutetype *)
			  rn_delete(&p->keylen, &p->masklen, nov_rnhead))) {
			buginf("\nIPX: novnlsp_update did not delete %x %x properly",
			       p->dstnet, p->rt_mask);
		    } else {
			rnnode->ndb = new_p;
			if (!rn_addroute(&new_p->keylen, &new_p->masklen, 
					 nov_rnhead, rnnode->rt_nodes)) {
			    buginf("\nNLSP: novnlsp_update did not add %x %x properly",
				   new_p->dstnet, new_p->rt_mask);
			} 
		    }
		}
		nov_ndb_free_newest(p);
		p->next = NULL;
		p->backup_info = NULL;
		temp_pp = p->path_list;
		while (temp_pp != NULL) {
		    novell_mark_rdb_changed
			(p, temp_pp, p->flags, IPX_ROUTE_EVENT_DELETED);
		    temp_pp = temp_pp->next;
		}
		if (p->protocol_info.nlsp_info.spf_next != NULL) {
		    if (debug) buginf(" (del bkup)");
		    next_p = p->protocol_info.nlsp_info.spf_next;
		    new_p->path_list = next_p->path_list;
		    ipx_ndb_self_backup(new_p);
		    novnlsp_free_rdb_list
			(next_p, IPX_ROUTE_FREE_NEVER_IN_TABLE);
		    novell_route_count++;
		    nov_ndb_free
			(next_p, IPX_ROUTE_FREE_NEVER_IN_TABLE, FALSE);
		}
		p->protocol_info.nlsp_info.spf_next = new_p;
		p->protocol_info.nlsp_info.spf_pessimal = TRUE;
	    } else {
		/*
		 * Active, not NLSP ... backup current info:
		 */
		if (debug) buginf(" (active->NLSP)");
		ipx_ndb_self_backup(p);
		novnlsp_free_rdb_list(p, IPX_ROUTE_FREE_WAS_IN_TABLE);
		p->protocol_info.nlsp_info.spf_next = NULL;
		p->protocol_info.nlsp_info.spf_pessimal = FALSE;
	    }
	    /*
	     * If a new active ndb was created, make sure it is
	     * referenced correctly for the code which follows.
	     */
	    if (new_p != NULL) {
		next_p = p;
		p = new_p;
	    }
	}
	/*
	 * Update current entry with new info:
	 */
        TIMER_STOP(p->holddownuntil);
        p->hopcount = cost->hops;
	p->area_count = cost->area_count;
        p->delay = p->last_delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
	p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
        p->path_list = p->nextpath = pp;
        p->paths = 1;
	p->options = IPX_NDB_OPTION_VALID;
	p->novpdbindex = pdb->index;
	p->flags = pdb->proctype | flags | (p->flags & NET_SUMMARY);
        p->procmask = pdb->mask;
	pp->procmask = pdb->mask;
        pp->nidb = nidb;
        pp->srcnet = snet;
        ieee_copy(shost, pp->srchost);
        GET_TIMESTAMP(pp->timer);
	pp->ndb = p;
	pp->lsp_linkage.lsp_version = lsp_version;
	pp->lsp_linkage.lsp_index = lsp_index;
	bcopy(&cost->oop, &pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
	novnlsp_insert_rdb_in_lsp_list(pp, lh);
	if (old_pdb == pdb) {
	    nov_network_redist(pdb, p, ROUTE_MODIF);
	} else {
	    nov_network_redist(pdb, p, ROUTE_UP);
	}
	novcache_increment_version(NULL, NOVCACHE_NLSP_BETTER_METRIC, dstnet,
				   netmask);
	novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_ADD);
	return(pp);
    }
    /*
     * Handle inferior metric:
     *
     * Route is inferior if ...
     *
     * - old route is NLSP, new route is NLSP with inferior metric
     * - old route is NLSP, new route is XROUTE
     * - old route is XROUTE, new route is XROUTE with inferior ticks/hops
     */

    if (debug) buginf(" (inferior ...");

    /*
     * Backup inferior information if we have decided to prefer an
     * existing non-NLSP route. Otherwise, continue and see if the
     * inferior info should be kept as a candidate (pessimal) route.
     * This is because we have no way of knowing at this point if
     * apparently superior NLSP information will continue to be valid
     * following subsequent ager run(s).
     */

    if (!(p->flags & NET_NLSP)) {
	if (debug) buginf(" ignore)");
	ipx_insert_backup_info(&(p->backup_info), &backup, pdb, debug);
	return(NULL);
    }
    if (debug) buginf(" process)");
    /*
     * At this point we have a valid alternate (NLSP) route and a previously
     * better (NLSP) route which may become obsolete. However, it may be
     * revalidated later in this spf run. In case the current route is not
     * regenerated, remember this one. Allocate and build a new descriptor
     * and attach it to the previous route.
     *
     * Note that candidate rdb's are linked into an appropriate lsp chain.
     */
    old_p = p;

    if (p->protocol_info.nlsp_info.spf_next == NULL) {
	new_p = nov_ndb_malloc();
	if (new_p == NULL) {
	    return(NULL);
	}
	pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
	if (pp == NULL) {
	    nov_ndb_free(new_p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	    return(NULL);
	}
	/*
	 * OK, this is a keeper, but we need to compensate the route
	 * count because this isn't really and active route ndb.
	 */
	novell_route_count--;
	p->protocol_info.nlsp_info.spf_next = new_p;
	p = new_p;
	/*
	 * Keep a back pointer to the active ndb. This is necessary
	 * in order to invalidate the active ndb pointer to this bkup
	 * ndb, when the bkup ndb is aged but the active ndb remains valid.
	 */
	p->protocol_info.nlsp_info.spf_next = old_p;
	p->protocol_info.nlsp_info.spf_pessimal = TRUE;
	p->path_list = p->nextpath = pp;
	p->dstnet = dstnet;
	p->rt_mask = netmask;
	p->keylen = p->masklen = IPXRADIXKEYLEN;
	TIMER_STOP(p->holddownuntil);
	p->hopcount = cost->hops;
	p->area_count = cost->area_count;
	p->delay = p->last_delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
	p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
	p->options |= IPX_NDB_OPTION_VALID;
	p->novpdbindex = pdb->index;
	
	pp->nidb = nidb;
	pp->ndb = p;
	pp->lsp_linkage.lsp_version = lsp_version;
	pp->lsp_linkage.lsp_index = lsp_index;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	GET_TIMESTAMP(pp->timer);
	bcopy(&cost->oop, &pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
	p->paths = 1;
	p->flags = pdb->proctype | flags | p->flags;
	p->procmask = pdb->mask;
	pp->procmask = pdb->mask;
	if (debug) {
	    buginf("\nNLSP: Create bkup route to %x"
		   " @[M%u/T%u/H%u][Ext T%u/H%u], via %x.%e"
		   ", flags %x, lsp {%d/%d}",
		   dstnet, cost->metric, cost->delay, cost->hops,
		   cost->ext_delay, cost->ext_hops, snet, shost,
		   p->flags, lsp_index, lsp_version);
	}
	novnlsp_insert_rdb_in_lsp_list(pp, lh); /* %%% */
	return(pp);
    } else {
	p = p->protocol_info.nlsp_info.spf_next;

	/*
	 * Tidy up in case someone forgot:
	 */
	p->protocol_info.nlsp_info.spf_next = old_p;
	p->protocol_info.nlsp_info.spf_pessimal = TRUE;
    }

    /*
     * We are now referencing the new, spf candidate ndb.
     * Reanalyze route attributes:
     */
    old_pdb = nov_proctable[p->novpdbindex];
    old_rip = (p->flags & NET_RIP);
    old_xroute = (p->flags & NET_XROUTE);
    old_nlsp = ((p->flags & NET_NLSP) && !old_xroute);
    /*
     * At this point we will only accept equal or better
     * for our potentially valid "inferior" route:
     */
    if ((old_nlsp && new_nlsp && (p->path_metric == cost->metric)) ||
	(old_xroute && new_xroute &&
	 NOVELL_EQUAL_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p) &&
	 (p->path_metric == cost->metric))) {
	TIMER_STOP(p->holddownuntil);
	p->hopcount = cost->hops;
	p->area_count = cost->area_count;
	p->delay = p->last_delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
	p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
	/*
	 * Search for this path in path list. Update entry if found.
	 * Actually, this probably should not happen. However, do it
	 * anyway just in case.
	 */
	last_pp = pp = p->path_list;
	while (pp) {
	    last_pp = pp;
	    /* One host, one vote */
            if ((pp->nidb == nidb) &&
		(pp->lsp_linkage.lsp_index == lsp_index) &&
		(pp->srcnet == snet) &&
		(bcmp(pp->srchost, shost, IEEEBYTES) == 0)) {
		GET_TIMESTAMP(pp->timer);
		pp->lsp_linkage.lsp_version = lsp_version;
		if (debug) buginf(" (bkup refresh)");
		return(pp);
	    } 
	    pp = pp->next;
	}
	/*
	 * Not found ... new entry ... add it. Ignore novell_maxpaths.
	 * NLSP keeps everything.
	 * The new path will be sorted in NLSP "order of preference
	 * of routes" order (See NLSP spec, section 6-2).
	 */
	last_pp = (novrdbtype *)&p->path_list;
	pp = p->path_list;
        while ((pp != NULL) &&
	       NLSP_WORSE_COST
	       (&cost->oop, &pp->protocol_info.nlsp_info.cost)) {
	    last_pp = pp;
	    pp = pp->next;
	}
	temp_pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
	if (temp_pp == NULL) {
	    return(NULL);
	}	
	temp_pp->nidb = nidb;
	temp_pp->ndb = p;
	temp_pp->lsp_linkage.lsp_version = lsp_version;
	temp_pp->lsp_linkage.lsp_index = lsp_index;
	temp_pp->srcnet = snet;
	ieee_copy(shost, temp_pp->srchost);
	GET_TIMESTAMP(temp_pp->timer);
	temp_pp->procmask = pdb->mask;
	bcopy(&cost->oop, &temp_pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
	
	last_pp->next = temp_pp;
	if (pp != NULL) {
	    temp_pp->next = pp;
	}
	p->paths++;
	p->flags = pdb->proctype | flags | p->flags;
	p->procmask = pdb->mask;
	if (debug) {
	    buginf("\nNLSP: Adding new bkup path for net %x", dstnet);
	}
	novnlsp_insert_rdb_in_lsp_list(temp_pp, lh); /* %%% */
	return(temp_pp);
    } 
    /*
     * Handle better metric:
     *
     * Route is better if ...
     *
     * - old route is XROUTE, new route is XROUTE with better ticks/hopcount
     * - old route is XROUTE, new route is NLSP
     * - old route is NLSP, new route is NLSP with better metric
     * - for address summary, new route with better ext ticks
     */
    if ((old_xroute && new_nlsp) ||
	((p->ext_delay > cost->ext_delay) && (netmask != NOV_NETMASK_ALL)) ||
	(old_xroute && new_xroute &&
	 NOVELL_BETTER_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p)) ||
	(old_xroute && new_xroute &&
	 NOVELL_EQUAL_XROUTE_INFO(cost->ext_hops, cost->ext_delay, p) &&
	 (cost->metric < p->path_metric)) ||
	(netmask == NOV_NETMASK_ALL && old_nlsp && new_nlsp && 
	 (cost->metric < p->path_metric))) {
	if (debug) {
            buginf("\nNLSP: better bkup route @[M%u/T%u/H%u][Ext T%u/H%u]"
		   " for %x via %x.%e,", cost->metric, cost->delay, cost->hops,
		   cost->ext_delay, cost->ext_hops, dstnet, snet, shost);
	    buginf("\n      flushing %d path(s) @[M%u/T%u/H%u][Ext T%u/H%u]",
		   p->paths, p->path_metric, p->delay, p->hopcount,
		   p->ext_delay, p->ext_hop);
	} 
	pp = malloc_named(sizeof(novrdbtype), "NLSP RDB");
	if (pp == NULL) {
	    ipx_insert_backup_info(&(old_p->backup_info), &backup, pdb, debug);
	    return(NULL);
	}
	/*
	 * Save current bkup path detail in active route backup info list.
	 * Backup info should only be attached to the active ndb.
	 */
	ipx_ndb_self_backup(p);
	old_backup_info = p->backup_info;
	p->backup_info = NULL;

	while (old_backup_info != NULL) {
	    bi = old_backup_info;
	    old_backup_info = bi->next;
	    bi->next = old_p->backup_info;
	    old_p->backup_info = bi;
	}

	p->hopcount = cost->hops;
	p->area_count = cost->area_count;
	p->delay = p->last_delay = cost->delay;
	p->igrp_delay = NOT_IBM_TICKS(cost->delay);
	p->ext_hop = cost->ext_hops;
	p->ext_delay = cost->ext_delay;
	p->path_metric = cost->metric;
	novnlsp_free_rdb_list(p, IPX_ROUTE_FREE_NEVER_IN_TABLE);
	p->path_list = p->nextpath = pp;
	p->paths = 1;
	p->options = IPX_NDB_OPTION_VALID;
	p->novpdbindex = pdb->index;
	p->flags = pdb->proctype | flags | p->flags;
	p->procmask = pdb->mask;
	pp->procmask = pdb->mask;
	pp->nidb = nidb;
	pp->ndb = p;
	pp->lsp_linkage.lsp_version = lsp_version;
	pp->lsp_linkage.lsp_index = lsp_index;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	GET_TIMESTAMP(pp->timer);
	bcopy(&cost->oop, &pp->protocol_info.nlsp_info.cost,
	      sizeof(nov_cost_type));
	novnlsp_insert_rdb_in_lsp_list(pp, lh); /* %%% */
	return(pp);
    }
    /*
     * Route not handled:
     */
    if (debug)
	buginf(" (not handled)");
    
    /*
     * Maintain backup info. Always associate with active route:
     */
    ipx_insert_backup_info(&(old_p->backup_info), &backup, pdb, debug);
    return(NULL); /* .... */
}

/*
 * nlsp_valid_adjacency
 * A simple wrapper to the ISIS call, to allow for modularity w/o creating a
 * whole CLNS subsystem.
 */
static boolean nlsp_valid_adjacency (paktype *pak, int level)
{
    clns_pdbtype *cpdb;
    cpdb = find_nlsp_pdb(pak->nif_input->pname);
    if (!cpdb)
	return(FALSE);
    return(isis_valid_adjacency(cpdb, pak, level));
}

/*
 * show_nlsp_database
 * A simple wrapper to the ISIS call, to allow for modularity w/o creating a
 * whole CLNS subsystem.
 */
void show_nlsp_database (parseinfo *csb)
{
    return(show_isis_database(csb));
}

/*
 * show_nlsp_spf_log
 * A simple wrapper to the ISIS call, to allow for modularity w/o creating a
 * whole CLNS subsystem.
 */
void show_nlsp_spf_log (parseinfo *csb)
{
    show_isis_spf_log(csb);
}

/*
 * show_nlsp_timers
 * A simple wrapper to the ISIS call, to allow for modularity w/o creating a
 * whole CLNS subsystem.
 */
void show_nlsp_timers (parseinfo *csb)
{
    show_isis_timers(csb);
}

/*
 * novnlsp_clear_all
 *
 * Handle the "clear ipx nlsp [tag] *" command.
 */
void novnlsp_clear_all (parseinfo *csb)
{
    char *name;
    clns_pdbtype *pdb;

    if (!novell_running || novell_shutting_down) {
	printf("\n%%IPX not running");
    } else {
	name = GETOBJ(string,1);
	pdb = find_nlsp_pdb(name);
	if (pdb == NULL) {
	    printf("\n%%IPX NLSP process not found");
	    return;
	}
	ipx_nlsp_clear_all(pdb);
    }
}

/*
 * clear_nlsp_neighbors
 *
 * Handle the "clear ipx nlsp [tag] neighbors" command.
 */
void clear_nlsp_neighbors (parseinfo *csb)
{
    char *name;
    clns_pdbtype *pdb;

    if (!novell_running || novell_shutting_down) {
	printf("\n%%IPX not running");
    } else {
	name = GETOBJ(string,1);
	pdb = find_nlsp_pdb(name);
	if (pdb == NULL) {
	    printf("\n%%IPX NLSP process not found");
	    return;
	}
	isis_clear_nlsp_neighbors(pdb);
    }
}

/*
 * novnlsp_svc_update
 *
 * Attempt to install a service specified by nlsp.
 */
void novnlsp_svc_update (novpdbtype *pdb,
			 idbtype *idb,
			 ushort svc_type,
			 uchar *svc_name,
			 int svc_name_len,
			 ulong svc_net,
			 uchar *svc_host,
			 ushort svc_sock,
			 nov_cost_param_type *cost,
			 isis_lsp_linkage **lh,
			 ushort lsp_version,
			 ushort lsp_index)
{
    int len;
    uchar shost[IEEEBYTES];
    novell_serveridtype sid;
    nidbtype *nidb = NULL;
    ulong flags = 0;
    uchar temp_svc_name[NOVELL_SERVER_NAME_LEN];
    boolean debug, event_debug, private_debug;

    /*
     * Generic screening:
     */
    if (pdb == NULL) {
	if (novellnlsp_debug) buginf("\nNLSP: Update pdb is NULL");
	return;
    }
    if (pdb->proctype != NET_NLSP) {
	if (novellnlsp_debug) buginf("\nNLSP: Update pdb not NLSP");
	return;
    }

    debug = NOVNLSP_DEBUG(svc_net, pdb);
    event_debug = novellnlsp_event_debug;
    private_debug = (event_debug && novell_private_debug);

    /*
     * Screen out anything dubious:
     */
    if ((svc_name_len == 0) ||
	(svc_name == NULL) ||
	(svc_net < NOV_MIN_NET) ||
	(svc_net > NOV_MAX_NET) ||
	(cost->hops >= novell_infinity) ||
	(cost->ext_hops > cost->hops)) {
	return;
    }
    if (debug) {
	len = min(svc_name_len, NOVELL_SERVER_NAME_LEN-1);
	bcopy(svc_name, temp_svc_name, len);
	temp_svc_name[len] = '\0';
	buginf("\nNLSP: SAP %d:%s:%x @[H%u] [Ext H%u] lsp {%d/%d}",
	       svc_type, &temp_svc_name, svc_net, cost->hops,
	       cost->ext_hops, lsp_index, lsp_version);
    }
    /*
     * OK, we have a valid service. Note that during SPF the route table
     * is unstable and will not be valid again until the ager has run.
     * However, note also that we now call addserver directly anyway.
     * Services with no route will be stored in the URD table until
     * we have a route.
     */
 
    debug = NOVNLSP_DEBUG(svc_net, pdb);
    flags = (NET_NLSP | NET_XROUTE);
    bzero(shost, IEEEBYTES);
    if (idb != NULL) {
	nidb = IPX_PRIMARY_NIDB(idb);
    }
    /*
     * Rebuild SAP record locally:
     */
    len = min(svc_name_len, NOVELL_SERVER_NAME_LEN-1);
    bcopy(svc_name, sid.server_name, len);
    sid.server_name[len] = '\0';
    PUTSHORT(&sid.server_type, svc_type);
    PUTLONG(&sid.dnet, svc_net);
    ieee_copy(svc_host, sid.dhost);
    PUTSHORT(&sid.dsock, svc_sock);
    PUTSHORT(&sid.nethops, cost->hops);
    
    if (debug) {
	
	buginf("\nNLSP: Add svc (direct) %x:%s:%x.%e:%x"
	       " via %x.%e (%s) @[H%d][Ext H%d] {%d/%d}",
	       svc_type, sid.server_name, svc_net, svc_host, svc_sock,
	       0, shost, (nidb ? idb->short_namestring : "?"),
	       cost->hops, cost->ext_hops, lsp_index, lsp_version);
	
	if (private_debug) {
	    buginf("\nlsp {%d/%d}, lh=%x", lsp_index, lsp_version, lh);
	    if (*lh != NULL) {
		buginf("->%x (->%x/%x<-", *lh,
		       (*lh)->lsp_next, (*lh)->lsp_prev);
	    } else {
		buginf("->NULL");
	    }
	}
    }
    
    if ((nidb != NULL) && (lh != NULL)) {
	
	novell_addserver(pdb, &sid, nidb, NULL, 0, flags, cost->ext_hops,
			 &cost->oop, lh, lsp_version, lsp_index, FALSE);
    } else {
	if (debug) {
	    buginf(" (no nidb for service)");
	}
    }
}

/*
 * novnlsp_cost_compare
 *
 * Implements the order of preference of routes from the NLSP spec.
 *
 * NOTE: This procedure assumes that end-to-end cost is known to be equal.
 *
 *     -1 = cost1 < cost2 (cost1 is better)
 *      0 = cost1 = cost2 (costs are equal)
 *     +1 = cost1 > cost2 (cost2 is better)
 */
int novnlsp_cost_compare (nov_cost_type *cost1,
			  nov_cost_type *cost2)
{
    int diff;

    if ((cost1 == NULL) || (cost2 == NULL)) {
	return(IPX_NLSP_COST_EQ);
    }

    if (cost1->total_throughput > cost2->total_throughput) {
	return(IPX_NLSP_COST_LT);
    } else if (cost1->total_throughput < cost2->total_throughput) {
	return(IPX_NLSP_COST_GT);
    }

    if (cost1->total_delay < cost2->total_delay) {
	return(IPX_NLSP_COST_LT);
    } else if (cost1->total_delay > cost2->total_delay) {
	return(IPX_NLSP_COST_GT);
    }

    if (cost1->mtu > cost2->mtu) {
	return(IPX_NLSP_COST_LT);
    } else if (cost1->mtu < cost2->mtu) {
	return(IPX_NLSP_COST_GT);
    }

    diff = ieee_compare(&cost1->system_id[0], &cost2->system_id[0]);
    if (diff < 0) {
	return(IPX_NLSP_COST_LT);
    } else if (diff > 0) {
	return(IPX_NLSP_COST_GT);
    }

    if (cost1->circuit_id < cost2->circuit_id) {
	return(IPX_NLSP_COST_LT);
    } else if (cost1->circuit_id > cost2->circuit_id) {
	return(IPX_NLSP_COST_GT);
    }

    diff = ieee_compare(&cost1->mac_addr[0], &cost2->mac_addr[0]);
    if (diff < 0) {
	return(IPX_NLSP_COST_LT);
    } else if (diff > 0) {
	return(IPX_NLSP_COST_GT);
    }

    return(IPX_NLSP_COST_EQ);
}

/*
 * novnlsp_dr_callback
 */
void novnlsp_dr_callback (boolean suspend_ok, novpdbtype *pdb)
{
    novrip_inject_all_backup_routes(suspend_ok, pdb);
    novrip_inject_all_backup_services(suspend_ok, pdb);
}

/*
 * novnlsp_process_redistribute
 * Process a redistribute everyting queue entry.
 */
void novnlsp_process_redistribute (
    novpdbtype *pdb, 
    novpdbtype *dist,
    boolean sense)
{
    if (pdb->running) {
	ipx_mark_dr_info_change(pdb, NULL);
	isis_mark_L1_changed(pdb->osi_pdb);
    }
    /*
     * Set the distflag for this pdb. 
     * It was already set in novell_redist_command for "redist" case.
     * Distflag needs to get set even if the process isn't running (yet).
     */
    if (!sense)
        pdb->distflag &= ~dist->mask;
}

/*
 * ipxnlsp_init
 * Initialize novell NLSP support
 */

static void ipxnlsp_init (subsystype *subsys)
{
    novell_nlsp_enabled = TRUE;	/* Give IPX a quick way of finding this */
    novnlsp_ager_pid = NO_PROCESS;
    mgd_timer_init_leaf(&novnlsp_ager_mt, NULL, 0, NULL, FALSE);

    reg_add_ipx_nlsp_process_packet(novnlsp_process_packet,
				    "novnlsp_process_packet");
    reg_add_ipx_nlsp_start(novnlsp_start, "novnlsp_start");
    reg_add_ipx_nlsp_valid_adjacency(nlsp_valid_adjacency,
				     "nlsp_valid_adjacency");
    reg_add_ipx_kludge_enabled(ipx_kludge_enabled, "ipx_kludge_enabled");

    reg_add_ipx_nlsp_delete_rdb_from_lsp_list
	(novnlsp_delete_rdb_from_lsp_list,
	 "novnlsp_delete_rdb_from_lsp_list");

    reg_add_ipx_nlsp_delete_srdb_from_lsp_list
	(novnlsp_delete_srdb_from_lsp_list,
	 "novnlsp_delete_srdb_from_lsp_list");

    reg_add_ipx_nlsp_insert_rdb_in_lsp_list
	(novnlsp_insert_rdb_in_lsp_list,
	 "novnlsp_insert_rdb_in_lsp_list");

    reg_add_ipx_nlsp_insert_srdb_in_lsp_list
	(novnlsp_insert_srdb_in_lsp_list,
	 "novnlsp_insert_srdb_in_lsp_list");

    reg_add_ipx_nlsp_cost_compare(novnlsp_cost_compare,
				  "novnlsp_cost_compare");
    
    reg_add_ipx_nlsp_get_peer_network(ipx_nlsp_get_peer_network,
				      "ipx_nlsp_get_peer_network");
    isis_ipx_system_init();
    ipx_nlsp_parser_init();
}

int age_route_patricia (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    novndbtype *ndb;    
    ulong net;

    sumroute = (nov_sumroutetype *)rnnode;
    ndb = sumroute->ndb;
    net = ndb->dstnet;
    if (sumroute) {
	if ((ndb->flags & NET_NLSP) &&
	    TIMER_RUNNING(ndb->holddownuntil)) {
	    if (AWAKE(ndb->holddownuntil)) {
		    
		if (novellnlsp_debug && novell_private_debug) {
		    buginf(" (%x)", ndb->dstnet);
		}
		if (!rn_delete(&ndb->keylen, &ndb->masklen, nov_rnhead))
		    buginf("\nIPX: age_route_patricia did not delete %x %x properly",
			   ndb->dstnet, ndb->rt_mask);
		novnlsp_free_ndb(ndb, IPX_ROUTE_FREE_WAS_IN_TABLE);
		free(sumroute);
	    } else {
		novnlsp_ager_schedule();
	    }
	}
	
	return NOV_WALK_CONTINUE;
    }
    return NOV_WALK_ABORT;
}

/*
 * The IPX_NLSP subsystem header. If you change this, be prepared to spend
 * about 10-20 thousand dollars in Novell re-certification fees. It'll
 * come out of YOUR paycheck if you change this without talking to a whole
 * lot of people first. :-)
 */
#define IPX_NLSP_MAJVERSION 1
#define IPX_NLSP_MINVERSION 0
#define IPX_NLSP_EDITVERSION  1

SUBSYS_HEADER(ipxnlsp, IPX_NLSP_MAJVERSION, IPX_NLSP_MINVERSION,
	      IPX_NLSP_EDITVERSION,
	      ipxnlsp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx, ipxwan",
	      "req: ipx, isis");
