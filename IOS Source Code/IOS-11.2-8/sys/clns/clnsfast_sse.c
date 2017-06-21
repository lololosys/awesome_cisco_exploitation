/* $Id: clnsfast_sse.c,v 3.3.60.1 1996/03/18 19:03:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/clns/clnsfast_sse.c,v $
 *------------------------------------------------------------------
 * clnsfast_sse.c - CLNS fast switching routines for the SSE
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clnsfast_sse.c,v $
 * Revision 3.3.60.1  1996/03/18  19:03:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:13:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  13:40:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:52:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/06/28  09:22:54  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/19  04:50:23  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/18  19:39:43  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:59:00  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:18:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "logger.h"
#include "access.h"
#include "subsys.h"
#include "clns_externs.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_registry.h"
#include "fastswitch_registry.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/sse_public.h"
#include "../hes/sse.h"
#include "../hes/sse_registry.h"
#include "clnsfast_sse.h"


/*
 * clnsfast_sse_dispatch
 * Return TRUE if the CLNS SSE cache needs to be updated.
 */

static boolean clnsfast_sse_dispatch (sse_protoinfo *protoinfo)
{
    sse_clnsinfo *clnsinfo = protoinfo->clnsinfo;

    if (!clnsinfo)
	return(FALSE);
    if (clnsinfo->cache_changes != clns_cache_changes) {
	if (sse_debug)
	    buginf("\nSSE: CLNS number of cache entries changed");
	return(TRUE);
    }
    if (clnsinfo->cache_version != clns_cache_version) {
	if (sse_debug)
	    buginf("\nSSE: CLNS cache change %d %d", clnsinfo->cache_version,
		   clns_cache_version);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * clnsfast_sse_update
 * Update the SSE cache for CLNS.
 */

static void clnsfast_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    sse_clnsinfo *clnsinfo = protoinfo->clnsinfo;
    clns_cachetype *cache;
    clns_cache_adjtype *cache_adj;
    int hash;
    idbtype *idb;
    hwidbtype *hwidb;
    mcitype *cbus;
    int len;
    uchar *ptr;

    if (!clnsinfo)
	return;
    if (!clnsfast_sse_dispatch(protoinfo))
	return;
    clnsinfo->cache_version = clns_cache_version;
    clnsinfo->cache_changes = clns_cache_changes;
    
    sse_init_tree(sse, SSE_PROTOCOL_CLNS);
    
    for (hash = 0; hash < CLNS_CACHE_SIZE; hash++) {

	/*
	 * Suspend now while we hold no pointers
	 */
	process_may_suspend();			

	for (cache = clns_cache[hash]; cache; cache = cache->next) {
	    
	    if (cache->cache_version != clns_cache_version)
		continue;
	    for (cache_adj = cache->cache_adj; cache_adj; cache_adj =
		 cache_adj->next) {
		
		/*
		 * Check to see if this is a legit entry to add.  If so,
		 * pass it in.
		 */
		if (!cache_adj->length || !cache_adj->adj)
		    goto next;
		idb = cache_adj->adj->idb;
		hwidb = idb->hwptr;
		if (sse_real(sse)) {
		    
		    /*
		     * SSE switching must be enabled.
		     */
		    if (!hwidb)
			goto next;
		    if (!(hwidb->clns_routecache & CLNS_SSE_ROUTECACHE))
			goto next;
		    if (!hwidb->clns_fastokay)
			goto next;
		    cbus = hwidb->devctl;
		    if (cbus->sseptr != sse)
			goto next;
		}
		
		if (idb->clns_filter_out)
		    goto next;
		if (idb->clns_filter_in)
		    goto next;
		
		len = cache->destination[0];
		ptr = &cache->destination[0];

		/*
		 * Note: We include the DA length indicator in the
		 * tree to disambiguate nested addresses and we
		 * don't route on the n-selector.
		 */

		if (sse_debug)
		    buginf("\nadded %n", cache->destination);
		if (!sse_add(sse, SSE_PROTOCOL_CLNS, len, ptr, ptr[len-1],
			     idb, cache_adj->rewrite, cache_adj->length, 1,
			     ET_SAP, 0, 0))
		    goto done;
	    next:
		/*
		 * If we've reached the end of the circular list, bail.
		 */
		if (cache_adj->next == cache->cache_adj)
		    break;
	    }
	}
    }    
 done:
    sse_compile(sse, SSE_PROTOCOL_CLNS);
}


/*
 * cbus_clnpcache_stats
 * Retrieve CLNP specific statistics for SSE switching
 */

static void cbus_clnpcache_stats (hwidbtype *idb)
{
    ulong counts;
    mciregtype *regptr;
    mcitype *mci;

    mci = idb->devctl;
    if (!(mci->flags & CBUSII))
	return;
    regptr = idb->mci_regptr;
    if (mci->sseptr) {
	regptr->argreg = ((FCI_CLNP_RX_PKTS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	idb->counters.inputs_auto[ACCT_PROTO_CLNS] += counts;
	clns_traffic[CLNS_INPUT] += counts;
	regptr->argreg = ((FCI_CLNP_RX_BYTES << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	idb->counters.rx_cumbytes_auto[ACCT_PROTO_CLNS] += regptr->largreg;
	regptr->argreg = ((FCI_CLNP_TX_PKTS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	idb->counters.outputs_auto[ACCT_PROTO_CLNS] += counts;
	clns_traffic[CLNS_FORWARD] += counts;
	regptr->argreg = ((FCI_CLNP_TX_BYTES << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	idb->counters.tx_cumbytes_auto[ACCT_PROTO_CLNS] += regptr->largreg;
	regptr->argreg = ((FCI_CLNP_TX_DROPS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	clns_traffic[CLNS_OUTPUTCNGSTN_DROPPED] += counts;
    }
}

/*
 * clnsfast_clear_sse
 * Force a recalculation of the cache.
 */

#ifdef NOBODY_CALLS_ME
void clnsfast_clear_sse (sse_protoinfo *protoinfo)
{
    sse_clnsinfo *clnsinfo = protoinfo->clnsinfo;

    if (!clnsinfo)
	return;
    clnsinfo->cache_version = 0;
    clnsinfo->cache_changes = 0;
}
#endif

/*
 * clnsfast_sse_init
 * Initialize sse information for CLNS.
 */

static void clnsfast_sse_init (sse_protoinfo *protoinfo)
{
    sse_clnsinfo *clnsinfo;

    clnsinfo = malloc(sizeof(sse_clnsinfo));
    protoinfo->clnsinfo = clnsinfo;
}

/*
 * clnsfast_sse_register_thyself
 * Subsystem initialization.
 */

static void clnsfast_sse_register_thyself (subsystype *subsys)
{
    reg_add_sse_update(clnsfast_sse_update, "clnsfast_sse_update");
    reg_add_sse_dispatch(clnsfast_sse_dispatch, "clnsfast_sse_dispatch");
    reg_add_sse_data_init(clnsfast_sse_init, "clnsfast_sse_init");
    reg_add_cbus_get_statistics(cbus_clnpcache_stats, "cbus_clnpcache_stats"); 
}

/* 
 * clns_sse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(clns_sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      clnsfast_sse_register_thyself, SUBSYS_CLASS_PROTOCOL,
	      "seq: clns",
	      "req: sse, clns");
