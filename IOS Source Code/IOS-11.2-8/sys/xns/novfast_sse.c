/* $Id: novfast_sse.c,v 3.3.62.1 1996/03/18 22:53:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xns/novfast_sse.c,v $
 *------------------------------------------------------------------
 * novfast_sse.c - Novell fast switching routines for the SSE
 *
 * January 1993 William H. Palmer
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_sse.c,v $
 * Revision 3.3.62.1  1996/03/18  22:53:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  11:22:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  22:01:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  19:23:51  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:59:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/13  05:25:02  tli
 * CSCdi40250:  SSE: clean up IPX debugging
 *
 * Revision 2.4  1995/06/28  09:33:56  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/20  21:54:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:26:07  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:29:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "../parser/parser_defs_sse.h"
#include "xns.h"
#include "xnstypes.h"
#include "xnsfast.h"
#include "novfast.h"
#include "novell.h"
#include "subsys.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../hes/if_mci.h"
#include "../util/range.h"

#include "novfast_sse.h"
#include "../hes/sse_public.h"
#include "../hes/sse.h"
#include "../hes/sse_registry.h"

/*
 * Externs and global storage 
 */
extern long novcache_entries;
extern long novcache_valid;

#define TABLE(type, hash) (novcache[hash])
#define T_ADDR(type, hash) (&novcache[hash])


/*
 * novfast_sse_dispatch
 * Return TRUE if the Novell SSE cache needs to be updated.
 */

static boolean novfast_sse_dispatch (sse_protoinfo *protoinfo)
{
    sse_ipxinfo *ipx = protoinfo->ipxinfo;

    if (!ipx)
	return(FALSE);
    if (ipx->nov_cache_version != novcache_version)
	return(TRUE);
    if (ipx->nov_cache_entries != novcache_entries)
	return(TRUE);
    if (ipx->nov_cache_valid != novcache_valid)
	return(TRUE);
    return(FALSE);
}

/*
 * novfast_sse_update
 * Update the SSE cache for Novell/IPX.
 */

static void novfast_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    novcachetype *ptr;
    novpathtype *pptr;
    ulong address;
    int maskbytes;
    uchar scratch[30];
    int i, count, len;
    uchar rewrite[MAXMAC_BYTES+2];
    uchar *rwptr;
    hwidbtype *hwidb;
    mcitype *cbus;
    sse_ipxinfo *ipx = protoinfo->ipxinfo;

    if (!novfast_sse_dispatch(protoinfo))
	return;
    maskbytes = IPX_HOST_BYTES;		/* stupid compiler */
    if (!ipx)
	return;
    ipx->nov_cache_version = novcache_version;
    ipx->nov_cache_entries = novcache_entries;
    ipx->nov_cache_valid = novcache_valid;
    if (sse_debug) {
	buginf("\nSSE: Built IPX cache from version %lu, %lu entries, "
	       "%lu valid",
	       ipx->nov_cache_version, ipx->nov_cache_entries,
	       ipx->nov_cache_valid);
    }

    sse_init_tree(sse, SSE_PROTOCOL_IPX);

    /*
     * Walk the fast switching cache and build a new tree.
     */
    for (i = 0; i < XNS_HASHLEN; i++) {
	/*
	 * Suspend now, while we have no pointers
	 */
	process_may_suspend();			
	for (ptr = novcache[i]; ptr; ptr = ptr->novc_next_ptr) {
	    if (ptr->novc_version != novcache_version)
		continue;

	    /*
	     * It's a valid cache entry.  Calculate what to add to the
	     * tree.  Add entry for each path.
	     */
	    pptr = ptr->novc_path_list_ptr;
	    for (count = ptr->novc_path_count; count; count--, pptr++) {
		
		hwidb = pptr->novp_idb_ptr->hwptr;
		if (sse_real(sse)) {
		    if (!hwidb)
			continue;
		    if (!(hwidb->novell_routecache & NOVELL_RCACHE_SSE))
			continue;
		    cbus = hwidb->devctl;
		    if (cbus->sseptr != sse)
			continue;
		}
		
		address = ptr->novc_dest_net;
		*((ulong *)scratch) = address;
#ifdef DEBUG
		if (sse_debug)
		    buginf("\nSSE: Examining cache entry %x.%e",
			   address, ptr->novc_dest_address);
#endif
		if (ptr->novc_connected) {
		    maskbytes = IPX_ADDR_BYTES;
		    ieee_copy(ptr->novc_dest_address, &scratch[IPX_NET_BYTES]);
		} else
		    maskbytes = IPX_NET_BYTES;
		
		/*
		 * Fix up the rewrite, if necessary.  Watch the length
		 * of what's added here...
		 */
		len = pptr->length;
		rwptr = rewrite;
		if (hwidb->status & IDB_TR) {
		    len += 2;
		    *rwptr++ = AC_PRI0;
		    *rwptr++ = FC_TYPE_LLC;
		} else if (hwidb->status & IDB_FDDI) {
		    len += 1;
		    *rwptr++ = FDDI_LLC_FC_BYTE;
		}
		bcopy(pptr->novp_mh.mac_bytes, rwptr, pptr->length);
#ifdef DEBUG
		if (sse_debug) {
		    buginf("\nSSE: Adding %d bytes of: %-02x%-02x"
			   "%-02x%-02x%-02x%-02x%-02x%-02x%-02x%-02x",
			   maskbytes, scratch[0], scratch[1],
			   scratch[2], scratch[3], scratch[4],
			   scratch[5], scratch[6], scratch[7],
			   scratch[8], scratch[9]);
		    buginf("\nSSE: Rewrite with %d bytes: %-02x%-02x%-02x"
			   "%-02x%-02x%-02x%-02x%-02x%-02x%-02x"
			   "%-02x%-02x%-02x%-02x%-02x",  
			   pptr->length, rewrite[0], rewrite[1],
			   rewrite[2], rewrite[3], rewrite[4],
			   rewrite[5], rewrite[6], rewrite[7],
			   rewrite[8], rewrite[9], rewrite[10],
			   rewrite[11], rewrite[12], rewrite[13],
			   rewrite[14]);
		}
#endif
		if (!sse_add(sse, SSE_PROTOCOL_IPX, maskbytes,
			     scratch, scratch[maskbytes-1],
			     pptr->novp_idb_ptr, rewrite, len, 1,
			     pptr->enctype, pptr->igrp_route,
			     SSE_ADD_COOKIE)) {
		    if (sse_debug)
			buginf("\nSSE: sse_add(%#x, %d, %d, %i) "
			       "returned FALSE", sse, SSE_PROTOCOL_IPX,
			       maskbytes, scratch);  
		    goto done;
		}
	    }
	}
    }
    
 done:
    process_may_suspend();
    sse_compile(sse, SSE_PROTOCOL_IPX);
}

/*
 * novfast_clear_sse
 * Force a recalculation of the cache.
 */

static void novfast_clear_sse (sse_protoinfo *protoinfo)
{
    sse_ipxinfo *ipx = protoinfo->ipxinfo;

    if (!ipx)
	return;
    if (sse_debug)
	buginf("\nSSE: IPX cleared");
    ipx->nov_cache_version = 0;
    ipx->nov_cache_entries = 0;
    ipx->nov_cache_valid = 0;
}

/*
 * novfast_sse_init
 * Initialize the IPX information for the sse.
 */

static void novfast_sse_init (sse_protoinfo *protoinfo)
{
    sse_ipxinfo *ipx;

    ipx = malloc(sizeof(sse_ipxinfo));
    if (ipx)
	protoinfo->ipxinfo = ipx;
}

/*
 * novell_sse_register_thyself
 * Register this subsystem.
 */

static void novell_sse_register_thyself (subsystype *subsys)
{
    reg_add_sse_update(novfast_sse_update, "novfast_sse_update");
    reg_add_sse_dispatch(novfast_sse_dispatch, "novfast_sse_dispatch");
    reg_add_sse_data_init(novfast_sse_init, "novfast_sse_init");
    reg_add_sse_protocols_invalid(novfast_clear_sse, "novfast_clear_sse");
    reg_add_sse_idb_invalid(novfast_clear_sse, "novfast_clear_sse");
    reg_add_sse_protocol_reset(SSE_PROTOCOL_IPX, novfast_clear_sse,
			       "novfast_clear_sse");

    ipx_sse_chain_init();
}

/* 
 * novell_sse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(ipx_sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      novell_sse_register_thyself, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx",
	      "req: sse, ipx");
