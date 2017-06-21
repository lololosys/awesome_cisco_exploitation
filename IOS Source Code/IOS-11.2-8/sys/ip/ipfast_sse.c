/* $Id: ipfast_sse.c,v 3.3.22.6 1996/08/06 08:27:38 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_sse.c,v $
 *------------------------------------------------------------------
 * ipfast_sse.c - IP fast switching routines for the SSE
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_sse.c,v $
 * Revision 3.3.22.6  1996/08/06  08:27:38  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.3.22.5  1996/06/24  16:04:51  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.3.22.4  1996/05/04  01:04:22  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.3.22.3  1996/04/24  00:06:15  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.3.22.2  1996/03/26  18:51:55  gchristy
 * CSCdi52104:  SSE: ACL checks are unnecessary when deleting cache entries
 * Branch: California_branch
 *
 * Revision 3.3.22.1  1996/03/18  20:17:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:46:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  01:00:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/23  08:49:59  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.2  1995/11/17  09:35:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/19  01:05:15  tli
 * CSCdi42444:  SSE: disabling the SSE doesnt purge its cache properly
 *
 * Revision 2.6  1995/09/25  08:32:16  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/04  06:43:32  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.4  1995/06/19 01:06:38  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/09  13:05:37  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/09 00:10:20  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:59:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <stdarg.h>
#include "packet.h"
#include "interface_private.h"
#include "logger.h"
#include "access.h"
#include "../hes/if_mci.h"
#include "../iprouting/route.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/ether.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../wan/smds.h"
#include "../util/range.h"
#include "../ip/ip.h"
#include "ipfast.h"
#include "ipfast_hes.h"
#include "ip_registry.h"
#include "../ui/debug.h"
#include "../iprouting/iprouting_debug.h"
#include "../iprouting/route_inlines.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/sse_public.h"
#include "../hes/sse.h"
#include "../hes/sse_registry.h"
#include "ipfast_sse.h"

#include "../vlan/vlan.h"


#define	WALK_CONTINUE	(0)
#define	WALK_COMPLETE	(-1)


/*
 * ipfast_sse_dispatch
 * Return TRUE if the IP SSE cache needs to be updated.
 */

boolean ipfast_sse_dispatch (sse_protoinfo *protoinfo)
{
    sse_ipinfo *ipinfo = protoinfo->ipinfo;

    if (!ipinfo)
	return(FALSE);

    if (ipinfo->ip_repopulate)
	return(TRUE);

    if (ipinfo->ip_rebuild)
	return(TRUE);

    return(FALSE);
}

/*
 * ipfast_sse_add_entry
 *
 * Given a fastswitching entry, add it to the SSE tree.  Return TRUE to
 * continue with the next entry, FALSE on some type of fatal error.
 */

boolean
ipfast_sse_add_entry (ssetype *sse, ipcache_rntype *entry)
{
    ipaddrtype dummy, address, mask;
    uchar scratch[4];
    int bytecount, len;
    mcitype *cbus;
    idbtype *idb;
    hwidbtype *hwidb;
    uchar rewrite[MAXMACSTRING+ISO2_OVERHEAD];
    uchar *rwptr;

    /*
     * It's a valid cache entry.  Calculate what to add to the
     * tree.  
     */
    idb = entry->idb;
    hwidb = idb->hwptr;
    if (sse_real(sse)) {
	
	/*
	 * SSE switching must be enabled.
	 * Fast access lists are supported.
	 *
	 * vLAN *subinterface* entries allowed, (even
	 * where major swidb is not so configured).
	 */

	if (!hwidb)
	    return(TRUE);

	if (IS_ISL_VLAN_SUBINTERFACE(idb) == TRUE) {

	    if (idb->isl_vlan->vlan_switching_path != vLAN_SSE_SWITCHING)
                return(TRUE);

        } else if (!(hwidb->ip_routecache & IP_SSE_ROUTECACHE))
	    return(TRUE);

	if (!hwidb->ip_fastokay)
	    return(TRUE);
	cbus = hwidb->devctl;
	if (cbus->sseptr != sse)
	    return(TRUE);
    }

    if (!reg_invoke_sse_acl_exists(sse, idb->ip_output_accesslist))
	return(TRUE);
    
    /*
     * Punt if policy routing is enabled anywhere.
     */
    if (hwidb->ip_fast_flags & IP_FAST_POLICY)
      return(TRUE);

    if (idb->ip_output_acctg)
	return(TRUE);

    /*
     * Prevent adding any destinations iff an input named ACL is
     * defined on any interface.
     */
    if (hwidb->ip_fast_flags & IP_FAST_INPUT_NACL)
	return(TRUE);
    /*
     * Don't let translated packets populate the SSE tree.
     */
    if (idb->ip_info->nat_enabled)
	return(TRUE);

    /*
     * Calculate the mask to be passed down.
     */
    address = entry->prefix;
    mask    = entry->mask;
    
    if (sse_debug && ipcache_acl_debug(entry->prefix, mask))
	buginf("\nSSE: add %i/%d", entry->prefix, ip_bitsinmask(mask));

    address &= mask;
    bytecount = ip_bytesinmask(mask);
    dummy = (address | ~mask);
    *((ipaddrtype *)scratch) = dummy;
    
    /*
     * Fix up the rewrite, if necessary.
     */
    len = entry->length;
    rwptr = rewrite;
    if (hwidb->status & IDB_TR) {
	len += 2;
	*rwptr++ = AC_PRI0;
	*rwptr++ = FC_TYPE_LLC;
    } else if (hwidb->status & IDB_FDDI) {
	len += 1;
	*rwptr++ = FDDI_LLC_FC_BYTE;
    }
    bcopy(entry->m.macstring, rwptr, entry->length);
    return(sse_add(sse, SSE_PROTOCOL_IP, bytecount, &address,
		   scratch[bytecount-1], idb, rewrite, len, 1,
		   hwidb->enctype, 0, SSE_ADD_INCREMENTAL));
} 

/*
 * ipfast_sse_delete_entry
 * Given a fastswitching entry, delete it from the SSE tree.
 */

void
ipfast_sse_delete_entry (ssetype *sse, ipcache_rntype *entry)
{
    ipaddrtype dummy, address, mask;
    uchar scratch[4];
    int bytecount;
    mcitype *cbus;
    idbtype *idb;
    hwidbtype *hwidb;
    sse_ipinfo *ipinfo;
    sse_protoinfo *protoinfo;
    boolean debug = FALSE;

    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
	buginf("\nSSE: delete attempt %i/%d", entry->prefix,
	       ip_bitsinmask(entry->mask));
    }

    /*
     * It's a valid cache entry.  Calculate what to add to the
     * tree.  
     */
    idb = entry->idb;
    hwidb = idb->hwptr;
    if (sse_real(sse)) {
	
	/*
	 * SSE switching must be enabled.
	 * Fast access lists are supported.
	 * Do NOT check to see whether SSE switching is enabled.  Someone
	 * might have just disabled it.
	 */
	if (!hwidb)
	    return;
	cbus = hwidb->devctl;
	if (cbus->sseptr != sse)
	    return;
    }

#ifdef DELETE_ME
    if ((idb->ip_output_accesslist >= MINFASTACC) &&
	(idb->ip_output_accesslist <= MAXSLOWACC)) {
	if (!reg_invoke_sse_acl_exists(sse, idb->ip_output_accesslist))
	    return;
    } else if (idb->ip_output_accesslist)
	return;
#endif    

    /*
     * Calculate the mask to be passed down.
     */
    address = entry->prefix;
    mask    = entry->mask;
    
    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
	buginf("\nSSE: delete %i/%d", entry->prefix,
	       ip_bitsinmask(entry->mask));
	debug = TRUE;
    }

    address &= mask;
    bytecount = ip_bytesinmask(mask);
    dummy = (address | ~mask);
    *((ipaddrtype *)scratch) = dummy;
    
    if (sse_delete(sse, SSE_PROTOCOL_IP, bytecount, &address,
		   scratch[bytecount-1], debug)) {
	/*
	 * There was some reason why we couldn't do a delete.  Do a rebuild.
	 */
	if (sse_debug)
	    buginf("\nIP: incremental SSE delete failure for %i/%d",
		   entry->prefix, ip_bitsinmask(entry->mask));
	protoinfo = reg_invoke_sse_protoinfo(sse);
	if (!protoinfo)
	    return;
	ipinfo = protoinfo->ipinfo;
	if (!ipinfo)
	    return;
	ipinfo->ip_rebuild = TRUE;
    }
}

/*
 * ipfast_sse_update_walker
 * Given a prefix cache entry, create an SSE cache entry.
 * Called by ipfast_sse_update  via radix tree walker
 */

static int ipfast_sse_update_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *ptr = rn2entry(rn);
    ssetype *sse;

    sse = va_arg(ap, ssetype *);

    if (!ipfast_sse_add_entry(sse, ptr))
	return(WALK_COMPLETE);

    return WALK_CONTINUE;
}

/*
 * ipfast_sse_update
 * Update the SP+ cache for IP.
 */

void ipfast_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    sse_ipinfo *ipinfo = protoinfo->ipinfo;
    ulong iter_runtime;
    idbtype *idb;

    if (!ipinfo)
	return;
    if (!ipfast_sse_dispatch(protoinfo))
	return;

    if (ipinfo->ip_rebuild) {
	/*
	 * Eliminate the current tree.
	 */
	iter_runtime = runtime();
	sse_init_tree(sse, SSE_PROTOCOL_IP);
	sse_purge(sse, SSE_PROTOCOL_IP);
	if (sse_debug)
	    buginf("\nSSE: IP cache purge took %dms",
		   runtime() - iter_runtime); 
	
	ipinfo->ip_rebuild = FALSE;

	/*
	 * Repopulation can take a very long time.  If we let the manager
	 * run now, and then come back later and repopulate, things will go
	 * much faster as we get the hardware re-enabled more quickly.
	 */
	ipinfo->ip_repopulate = TRUE;
	reg_invoke_sse_wake_manager();
	return;
    }
    
    /*
     * Check for policy routing on the cbus.  If any
     * exist, bail out now.  It would be nice if this wasn't so expensive.
     */
    FOR_ALL_SWIDBS(idb) {
	if ((!idb->ip_info || idb->ip_info->policy_enabled) &&
	    (idb->hwptr->status & IDB_CBUS)) {
	    return;
	}
    }

    /*
     * Scan the fast switching prefix cache and build a new tree.
     */
    iter_runtime = runtime();

    rn_walktree_timed(ipcache_head, ipfast_sse_update_walker,
		      ip_radix_successor, sse);

    if (sse_debug)
	buginf("\nSSE: IP cache repopulation took %dms",
	       runtime() - iter_runtime); 
    ipinfo->ip_repopulate = FALSE;
    process_may_suspend();
}

/*
 * ipfast_sse_ipcache_add
 *
 * Perform an incremental add to the SSE cache.  This is a callback from
 * the IP fastswitching cache maintenance code.
 */

static void
ipfast_sse_ipcache_add (ipcache_rntype *entry)
{
    sse_protoinfo *protoinfo;
    sse_ipinfo *ipinfo;
    mcitype *cbus;
    ssetype *sse;
    
    if (sse_manager_blocked)
	return;

    if (!entry)
	return;
    if (!entry->hwidb)
	return;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	protoinfo = reg_invoke_sse_protoinfo(sse);
	if (!protoinfo)
	    return;
	ipinfo = protoinfo->ipinfo;
	if (!ipinfo)
	    return;
	if (ipinfo->ip_rebuild) {
	    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
		buginf("\nSSE: add out of sync %i/%d", entry->prefix,
		       ip_bitsinmask(entry->mask));
	    }
	    continue;
	}
	if (onintstack()) {
	    /*
	     * We don't deal with life at interrupt level yet.  Blow the cache.
	     */
	    ipinfo->ip_rebuild = TRUE;
	    if (sse_debug) {
		buginf("\nSSE: add failure at interrupt level");
		bugtrace();
	    }
	    continue;
	}
	ipfast_sse_add_entry(sse, entry);
    }
}

/*
 * ipfast_sse_ipcache_delete
 *
 * Perform an incremental delete on the SSE cache.  This is a callback from
 * the IP fastswitching cache maintenance code.
 */

static void
ipfast_sse_ipcache_delete (ipcache_rntype *entry)
{
    sse_protoinfo *protoinfo;
    sse_ipinfo *ipinfo;
    mcitype *cbus;
    ssetype *sse;

    if (!entry) {
	if (sse_debug)
	    buginf("\nSSE: IP cache delete called with no entry");
	return;
    }

#ifdef NOTDEF
    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
	buginf("\nSSE: delete request %i/%d", entry->prefix,
	       ip_bitsinmask(entry->mask));
    }
#endif

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse) {
	    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
		buginf("\nSSE: delete missing sse %i/%d", entry->prefix,
		       ip_bitsinmask(entry->mask));
	    }
	    continue;
	}
	protoinfo = reg_invoke_sse_protoinfo(sse);
	if (!protoinfo) {
	    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
		buginf("\nSSE: delete missing protoinfo %i/%d", entry->prefix,
		       ip_bitsinmask(entry->mask));
	    }
	    continue;
	}
	ipinfo = protoinfo->ipinfo;
	if (!ipinfo) {
	    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
		buginf("\nSSE: delete missing ipinfo %i/%d", entry->prefix,
		       ip_bitsinmask(entry->mask));
	    }
	    continue;
	}

	if (ipinfo->ip_rebuild) {
	    /*
	     * Something major happened.  Do a rebuild.
	     */
	    if (sse_debug && ipcache_acl_debug(entry->prefix, entry->mask)) {
		buginf("\nSSE: delete out of sync %i/%d", entry->prefix,
		       ip_bitsinmask(entry->mask));
	    }
	    continue;
	}
	if (onintstack()) {
	    /*
	     * We don't deal with life at interrupt level yet.  Blow the cache.
	     */
	    ipinfo->ip_rebuild = TRUE;
	    if (sse_debug) {
		buginf("\nSSE: delete failure at interrupt level");
		bugtrace();
	    }
	    continue;
	}
	ipfast_sse_delete_entry(sse, entry);
    }
}

/*
 * ipfast_clear_sse
 * Force a recalculation of the cache.
 */

void ipfast_clear_sse (sse_protoinfo *protoinfo)
{
    sse_ipinfo *ipinfo = protoinfo->ipinfo;

    if (!ipinfo)
	return;
    ipinfo->ip_rebuild = TRUE;
    if (sse_debug && ipcache_debug)
	buginf("\nSSE: IP cache clear");
}

/*
 * ipfast_sse_init
 * Initialize sse information for IP.
 */

void ipfast_sse_init (sse_protoinfo *protoinfo)
{
    sse_ipinfo *ipinfo;

    ipinfo = malloc(sizeof(sse_ipinfo));
    protoinfo->ipinfo = ipinfo;
    ipinfo->ip_repopulate = FALSE;
    ipinfo->ip_rebuild = FALSE;
}

/*
 * ipfast_sse_register_thyself
 * Subsystem initialization.
 */

void ipfast_sse_register_thyself (void)
{
    reg_add_sse_update(ipfast_sse_update, "ipfast_sse_update");
    reg_add_sse_dispatch(ipfast_sse_dispatch, "ipfast_sse_dispatch");
    reg_add_sse_data_init(ipfast_sse_init, "ipfast_sse_init");
    reg_add_ipcache_add_callback(ipfast_sse_ipcache_add,
				 "ipfast_sse_ipcache_add");
    reg_add_ipcache_delete_callback(ipfast_sse_ipcache_delete,
				    "ipfast_sse_ipcache_delete");
    reg_add_sse_consistency(SSE_PROTOCOL_IP, sse_generic_consistency,
			    "sse_generic_consistency");
}
