/* $Id: nhrp_cache.c,v 3.4.20.4 1996/07/23 13:31:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp_cache.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_cache.c,v $
 * Revision 3.4.20.4  1996/07/23  13:31:49  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4.20.3  1996/06/04  00:25:17  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * awareded -> awarded
 * Interworking,unspecified -> Interworking, unspecified
 * Requestor -> Requester
 *
 * Revision 3.4.20.2  1996/03/28  03:31:29  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.4.20.1  1996/03/18  22:48:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.12.2  1996/03/07  11:07:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.12.1  1996/02/20  21:52:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:12:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/24  03:05:20  bcole
 * CSCdi47377:  NHRP with ATM ARP server crashes
 *
 * Protect against passing null pointer to radix code, check for lack of
 * NHRP configuration during callback processing.
 *
 * Revision 3.2  1996/01/03  23:32:50  bcole
 * CSCdi46376:  NHRP: Finish post branch split file moves
 *
 * Revision 3.1  1995/11/09  13:50:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/06  21:55:03  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../ip/ip.h"
#include "../util/radix.h"
#include "nhrp_debug.h"
#include "nhrp.h"
#include "nhrp_public.h"
#include "nhrp_inlines.h"
#include "nhrp_vc.h"
#include "parser_defs_nhrp.h"

/*
 * Storage
 */

/*
 * nhrp_ifcache
 * Return a pointer to the NHRP cache for a particular interface.
 */

static inline rnhtype *nhrp_ifcache (idbtype *swidb, uchar protoidx)
{
    nhrpinfotype *nhrpinfo;
    
    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return(NULL);
    return(nhrpinfo->pro[protoidx].nhrp_head);
}

/*
 * nhrp_cache_walk
 * Wrapper routine to perform a cache table walk.
 */

int nhrp_cache_walk (rn_walk_function f, ...)
{
    va_list ap, apcpy;
    int result;
    idbtype *swidb;
    rnhtype *head;
    nhrpfuns *fun;

    va_start(ap, f);
    apcpy = ap;
    fun = va_arg(apcpy, nhrpfuns *);
    swidb = va_arg(apcpy, idbtype *);
    result = -1;
    if (!swidb) {
	FOR_ALL_SWIDBS(swidb) {
	    head = nhrp_ifcache(swidb, fun->protoidx);
	    if (!head)
		continue;
	    result = rn_walktree_blocking_list(head->rnh_treetop, f, ap);
	}
    } else {
	head = nhrp_ifcache(swidb, fun->protoidx);
	if (head)
	    result = rn_walktree_blocking_list(head->rnh_treetop, f, ap);
    }
    va_end(ap);
    return(result);
}

/*
 * nhrp_rn2cache
 * Map an abstract radix node to an NHRP cache table entry.
 */

static inline nhrpcachetype *nhrp_rn2cache (rntype *rn)
{
    return((nhrpcachetype *)rn);
}

/*
 * nhrp_cache_mode_string
 * Map an NHRP cache entry type to a string.
 */

static uchar *nhrp_cache_mode_string (nhrp_cachemodetype mode)
{
    switch (mode) {
    case NHRP_STATIC:
	return("static");
    case NHRP_DYNAMIC:
	return("dynamic");
    case NHRP_INCOMPLETE:
	return("incomplete");
    default:
	return("unknown");
    }
}

/*
 * nhrp_cache_hostmask
 * Dynamically build a host netmask
 */

static void nhrp_cache_hostmask (nhrpaddrnet *net, uchar len)
{
    uchar i, *maskp;

    maskp = ((uchar *)&net->gen.opaque) + NHRP_ALIGN(len + 1);
    for (i = 0; i < len; i++)
	*(maskp++) = 0xff;
}

/*
 * nhrp_cache_keyinit
 * Initialize the radix key length fields.
 */

static uchar *nhrp_cache_keyinit (nhrpaddrnet *net, nhrpfuns *fun)
{
    uchar *maskkeyp;
    uchar len;

    len = fun->nhrp_addlen;
    maskkeyp = nhrp_cache_keypos(net, len);
    net->gen.network_key = *maskkeyp = len + 1;
    if (fun->aggregate)
	return(maskkeyp);
    return(NULL);
}

/*
 * nhrp_cache_lookup_rnmask
 * Look up an IP address in the cache.
 */

nhrpcachetype *nhrp_cache_lookup_rnmask (idbtype *swidb, nhrpaddrnet *dest,
					 nhrpfuns *fun, boolean longest)
{
    rnhtype *head;
    rntype *rn;
    uchar *maskkeyp;
    ulong netid;
    ushort idx;
    boolean first;

    maskkeyp = nhrp_cache_keyinit(dest, fun);
    /*
     * Simply do longest match lookup?
     */
    if (longest)
	maskkeyp = NULL;
    netid = swidb ? nhrp_netid(swidb, fun) : 0;
    idx = fun->protoidx;
    first = TRUE;

    /*
     * Search all interfaces that are enabled for NHRP.
     */
    FOR_ALL_SWIDBS(swidb) {
	/*
	 * Make sure we are looking for on the same logical NBMA network.
	 */
	if (netid && (netid != nhrp_netid(swidb, fun)))
	    continue;

	head = nhrp_ifcache(swidb, idx);
	if (!head)
	    continue;

	if (nhrp_debug_cache) {
	    if (first) {
		first = FALSE;
		nhrp_bugnet("\nNHRP: Lookup cache entry ", dest, fun);
		if (longest) {
		    buginf(" (longest)");
		}
	    }
	}

	rn = rn_lookup(&dest->gen.network_key, maskkeyp, head);
	if (!rn)
	    continue;

	return(nhrp_rn2cache(rn));
    }
    return(NULL);
}

/*
 * nhrp_cache_lookup_mask
 * Same as nhrp_cache_lookup, but with netmask supplied.
 */

nhrpcachetype *nhrp_cache_lookup_mask (idbtype *swidb, nhrpaddr *dest,
				       nhrp_mask mask, nhrpfuns *fun)
{
    nhrpaddrnet net;
    uchar *maskkeyp;
    
    /*
     * Extract network layer address
     */
    bcopy(dest, net.gen.opaque, fun->nhrp_addlen);
    /*
     * Extract mask
     */
    maskkeyp = nhrp_cache_keypos(&net, fun->nhrp_addlen);
    if (mask)
	bcopy(mask, ++maskkeyp, fun->nhrp_addlen);
    else
	nhrp_cache_hostmask(&net, fun->nhrp_addlen);
    nhrp_cache_keyinit(&net, fun);
    return(nhrp_cache_lookup_rnmask(swidb, &net, fun, FALSE));
}

/*
 * nhrp_cache_lookup
 * Wrapper routine for nhrp_cache_lookup_rnmask
 */

nhrpcachetype *nhrp_cache_lookup (idbtype *swidb, nhrpaddr *dest,
				  nhrpfuns *fun)
{
    nhrpaddrnet net;
    
    /*
     * Extract network layer address
     */
    bcopy(dest, net.gen.opaque, fun->nhrp_addlen);
    /*
     * Build a host netmask
     */
    nhrp_cache_hostmask(&net, fun->nhrp_addlen);
    nhrp_cache_keyinit(&net, fun);
    return(nhrp_cache_lookup_rnmask(swidb, &net, fun, FALSE));
}

/*
 * nhrp_cache_lookup_comp
 * Look up an address in the cache, and only return information regarding
 * complete entries.  Do longest match lookup.
 */

nhrpcachetype *nhrp_cache_lookup_comp (idbtype *swidb, nhrpaddr *dest,
				       nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    nhrpaddrnet net;
    uchar *route_mask, *cache_mask;
    boolean static_route;

    /*
     * Extract network layer address.
     */
    memset(&net, 0, sizeof(nhrpaddrnet));
    bcopy(dest, net.gen.opaque, fun->nhrp_addlen);

    /*
     * Try longest match.
     */
    ptr = nhrp_cache_lookup_rnmask(swidb, &net, fun, TRUE);
    if (!ptr  || (ptr->mode == NHRP_INCOMPLETE) || !ptr->nbma_len)
	return(NULL);

    /*
     * If route entry is more specific than NHRP cache and it's not a
     * static route, then return NULL.
     */
    static_route = FALSE;
    route_mask = (*fun->nhrp_netsmask)(dest, &static_route);
    if (route_mask) {
	cache_mask = nhrp_cache_keypos(&ptr->prefix, fun->nhrp_addlen) + 1;
	if (!(*fun->nhrp_specific2)(cache_mask, route_mask) && !static_route)
	    return(NULL);
    }
    return(ptr);
}

/*
 * nhrp_cache_add
 * Add a new entry to the cache.  Callback supplied routine to fill in
 * the cache entry before linking in.
 */

nhrpcachetype *nhrp_cache_add (idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			       nhrp_cacheupdatefunc func, void *arg,
			       nhrpfuns *fun)
{
    uchar *maskkeyp;
    nhrpcachetype *ptr;
    nhrpaddrnet *prefix;

    ptr = malloc(sizeof(nhrpcachetype));
    if (ptr == NULL) {
	return(NULL);
    }
    prefix = &ptr->prefix;
    maskkeyp = nhrp_cache_keyinit(prefix, fun);
    nhrp_cache_hostmask(prefix, fun->nhrp_addlen);
    prefix->gen.protoidx = fun->protoidx;
    if ((*func)(ptr, idb, dest, mask, arg, fun) == FALSE) {
	free(ptr);
	return(NULL);
    }
    if (nhrp_debug_cache)
	nhrp_bugnet("\nNHRP: Adding cache entry for ", prefix, fun);
    if (!rn_addroute(&prefix->gen.network_key, maskkeyp,
		     nhrp_ifcache(idb, fun->protoidx), ptr->rt_nodes)) {
	free(ptr);
	return(NULL);
    }
    ptr->idb = idb;
    return(ptr);
}

/*
 * nhrp_cache_clear_nei
 * Send a purge notification to all NHRP stations which we had previously
 * provided an NHRP response to for this particular cache entry.
 */

static void nhrp_cache_clear_nei (nhrpcachetype *ptr)
{
    nhrpcacherequestor *nbr, *nextnbr;

    for (nbr = ptr->requests; nbr; nbr = nextnbr) {
	nextnbr = nbr->next;
	nbr->next = NULL;
	nhrp_queue_retrans(nbr, ptr->expire);
	nhrp_send_purge(nbr);
    }
    ptr->requests = NULL;
}

/*
 * nhrp_cache_delete
 * Delete a particular entry from the cache.  Unlink & free
 * resources.
 */

void nhrp_cache_delete (idbtype *swidb, nhrpcachetype *ptr)
{
    ushort protoidx;
    uchar *maskkeyp;
    nhrpfuns *fun;
    nhrpaddrnet *prefix;

    prefix = &ptr->prefix;
    protoidx = prefix->gen.protoidx;
    fun = nhrp_funmap(protoidx);
    maskkeyp = nhrp_cache_keypos(prefix, fun->nhrp_addlen);
    if (nhrp_debug_cache) {
	buginf("\nNHRP: Deleting %sentry",
	       ptr->mode == NHRP_INCOMPLETE ? "incomplete " : "");
	nhrp_bugnet(" for ", prefix, fun);
	buginf(" interface %s", swidb ? swidb->namestring : "");
    }
    if (!fun->aggregate)
	maskkeyp = NULL;
    nhrp_macaddr_unmap(swidb, (nhrpaddr *)prefix->gen.opaque, maskkeyp, fun);
    if (!rn_delete(&prefix->gen.network_key, maskkeyp,
		   nhrp_ifcache(swidb, protoidx))) {
	if (nhrp_debug_cache)
	    buginf("\nNHRP: Cache entry failed to delete");
	return;
    }
    free(ptr);
}

/*
 * nhrp_cache_delete_subr
 * Actually free a cache table entry.  Tear down VC too.
 */

void nhrp_cache_delete_subr (nhrpcachetype *ptr)
{
    idbtype *swidb;

    swidb = ptr->idb;
    nhrp_cache_clear_nei(ptr);
    nhrp_cache_delete(swidb, ptr);
}

/*
 * nhrp_cache_purge_id_subr
 * Delete a particular cache table entry.  Cache table entry is validated
 * based upon request ID.
 */

static int nhrp_cache_purge_id_subr (rntype *rn, va_list ap)
{
    nhrpcachetype *ptr;
    ulong reqid;
    idbtype *swidb;
    nhrpfuns *fun;

    ptr = nhrp_rn2cache(rn);
    fun = va_arg(ap, nhrpfuns *);
    swidb = va_arg(ap, idbtype *);
    reqid = va_arg(ap, ulong);
    if (ptr->reqid != reqid)
	return(FALSE);
    if (ptr->mode != NHRP_DYNAMIC) {
	if (nhrp_debug) {
	    nhrp_bugnet("\nNHRP: Rejected purge request of ", &ptr->prefix,
			fun);
	    buginf(".  Entry is %s", nhrp_cache_mode_string(ptr->mode));
	}
	return(TRUE);
    }
    nhrp_cache_delete_subr(ptr);
    return(TRUE);
}

/*
 * nhrp_cache_purge_id
 * Trudge through cache, searching for a dynamic entry which matches the
 * passed in request ID.  If found, delete entry.
 */

void nhrp_cache_purge_id (idbtype *swidb, ulong reqid, nhrpfuns *fun)
{
    if (!nhrp_cache_walk(nhrp_cache_purge_id_subr, fun, swidb, reqid)) {
	if (nhrp_debug_cache)
	    buginf("\nNHRP: Failed to find cache entry matching reqid %u",
		   reqid);
    }
}

/*
 * nhrp_cache_clear_subr
 * Eliminate all dynamic cache table entries for an interface, and all
 * static entries not matching the passed in address type.
 */

static int nhrp_cache_clear_subr (rntype *rn, va_list ap)
{
    nhrpcachetype *ptr;
    idbtype *idb;
    ushort addr_type;
    nhrpfuns *fun;

    ptr = nhrp_rn2cache(rn);
    fun = va_arg(ap, nhrpfuns *);
    idb = va_arg(ap, idbtype *);
    addr_type = va_arg(ap, ushort);
    if (idb && ptr->idb != idb)
	return(0);
    switch (ptr->mode) {
    case NHRP_STATIC:
	if (addr_type == ptr->addr_type) {
	    /*
	     * Entry is still valid
	     */
	    return(0);
	}
	/*
	 * Fall through
	 */
    case NHRP_DYNAMIC:
    default:
	nhrp_cache_delete_subr(ptr);
	return(0);
    }
}

/*
 * nhrp_cache_clear
 * We've received a callback that it is time to clear the cache.
 * Consider that an interface's encapsulation may have changed, so even
 * static cache entries may need to be invalidated.
 */

void nhrp_cache_clear (idbtype *idb, nhrpfuns *fun)
{
    ushort addr_type;
    
    if (idb == NULL)
	return;
    if (!nhrp_if(idb, fun))
	return;
    addr_type = nhrp_nbma_type(idb);
    if (nhrp_debug_cache) {
	buginf("\nNHRP: Cache clear for %s, addr type %d",
	       idb->namestring, addr_type);
    }
    nhrp_cache_walk(nhrp_cache_clear_subr, fun, idb, addr_type);
}

/*
 * nhrp_cache_clear_wrap
 * Registry wrapper routine to clear the address resolution cache.
 */

void nhrp_cache_clear_wrap (idbtype *swidb)
{
    ushort i;
    nhrpfuns *fun;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	nhrp_cache_clear(swidb, fun);
    }
}

/*
 * nhrp_cache_if_delete
 * Delete all static and dynamic maps associated with the interface, as
 * the interface itself is being deleted.
 */

void nhrp_cache_if_delete (idbtype *idb)
{
    ushort i;
    nhrpfuns *fun;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	if (!nhrp_if(idb, fun))
	    continue;
	if (nhrp_debug_cache)
	    buginf("\nNHRP: Cache clear for %s proto %d", idb->namestring,
		   fun->protoidx);
	nhrp_cache_walk(nhrp_cache_clear_subr, fun, idb, AF_RESERVED);
    }
}

/*
 * nhrp_cache_age_subr
 * Age the dynamic entries in the cache.  Before entries expire, attempt to
 * refresh them, and shut off expiration timer.
 * If flush flag is set, expire entries regardless of age.
 */

int nhrp_cache_age_subr (rntype *rn, va_list ap)
{
    boolean flush;
    nhrpcachetype *ptr;
    idbtype *swidb;
    nhrpfuns *fun;

    ptr = nhrp_rn2cache(rn);
    fun = va_arg(ap, nhrpfuns *);
    swidb = va_arg(ap, idbtype *);
    flush = va_arg(ap, boolean);
    switch (ptr->mode) {
    case NHRP_STATIC:
	break;
    case NHRP_INCOMPLETE:
	if (flush || AWAKE(ptr->expire)) {
	    nhrp_cache_delete_subr(ptr);
	}
	break;
    case NHRP_DYNAMIC:
	if (flush || !TIMER_RUNNING(ptr->expire)) {
	    nhrp_cache_delete_subr(ptr);
	} else {
	    if (CLOCK_IN_INTERVAL(ptr->expire, ONEMIN)) {
		TIMER_STOP(ptr->expire);
		/*
		 * Don't attempt to refresh implicit or registered cache
		 * entries, or other dynamic entries which have not
		 * been actively used.
		 */
		if (ptr->flags & NHRP_UFLAG &&
		    !(ptr->flags & (NHRP_IFLAG|NHRP_RFLAG))) {
		    /*
		     * Refresh cache entry
		     */
		    if (nhrp_debug) {
			nhrp_bugnet("\nNHRP: Refreshing cache entry for ",
				    &ptr->prefix, fun);
		    }
		    nhrp_req_enq(ptr->idb, nhrp_addr(&ptr->prefix.ip.target),
				 TRUE, fun);
		}
	    }
	    ptr->flags &= ~NHRP_UFLAG;
	}
	break;
    }
    return(0);
}

/*
 * nhrp_cache_age
 * Registry wrapper to age entire cache.
 */

void nhrp_cache_age (void)
{
    nhrpfuns *fun;
    ushort i;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	nhrp_cache_walk(nhrp_cache_age_subr, fun, NULL, FALSE);
    }
}

/*
 * nhrp_print_flags
 * Display flag information, from cache
 */

static void nhrp_print_flags (uchar flags)
{
    if (flags & NHRP_AFLAG)
	printf("authoritative ");
    if (flags & NHRP_IFLAG)
	printf("implicit ");
    if (!(flags & NHRP_PFLAG))
	printf("negative ");
    if (flags & NHRP_RFLAG)
	printf("registered ");
    if (flags & NHRP_GFLAG)
	printf("local ");
    if (flags & NHRP_UFLAG)
	printf("used ");
}

/*
 * nhrp_show_nei
 * Show who we have provided NHRP responses to for this particular cache
 * entry.
 */

static void nhrp_show_nei (nhrpcachetype *ptr, nhrpfuns *fun)
{
    nhrpcacherequestor *nbr, *nextnbr;

    for (nbr = ptr->requests; nbr; nbr = nextnbr) {
	mem_lock(nbr);
	nhrp_printaddr("\n  Requester: ", &nbr->src, fun);
	printf(" Request ID: %u", nbr->reqid);
	nextnbr = nbr->next;
	free(nbr);
    }
}

/*
 * nhrp_show_cache_subr
 * Display a particular cache table entry
 */

static int nhrp_show_cache_subr (rntype *rn, va_list ap)
{
    nhrpcachetype *ptr;
    nhrp_cachemodetype mode;
    idbtype *idb;
    ulong expire;
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    boolean detail;
    nhrpfuns *fun;

    if (automore_quit())
	return(1);
    ptr = nhrp_rn2cache(rn);
    fun = va_arg(ap, nhrpfuns *);
    idb = va_arg(ap, idbtype *);
    mode = va_arg(ap, nhrp_cachemodetype);
    detail = va_arg(ap, boolean);
    /*
     * If we are only to display entries of a particular mode,
     * check it now.
     */
    if (mode && ptr->mode != mode)
	return(0);
    if (idb && ptr->idb != idb)
	return(0);
    nhrp_printpre("\n", &ptr->prefix, fun);
    if (ptr->flags & NHRP_PFLAG)
	if (ptr->nhop.ip != ptr->prefix.ip.target)
	    nhrp_printaddr(" via ", &ptr->nhop, fun);
    printf(", %s created ", ptr->idb ? ptr->idb->namestring : "");
    print_dhms(ptr->created);
    expire = TIME_LEFT_SLEEPING(ptr->expire);
    if (expire) {
	printf(" expire ");
	print_ticks_in_dhms(expire);
    } else {
	printf(" expired ");
    }
    printf("\n  Type: %s Flags: ",
	   nhrp_cache_mode_string(ptr->mode));
    nhrp_print_flags(ptr->flags);
    if (ptr->mode == NHRP_INCOMPLETE) {
	printf("\n  Cache hits: %u", ptr->hits);
    } else {
	printf("\n  NBMA address: %s ",
	       nhrp_sprint_nbma(nbma_buf, ptr->nbma.opaque,
				ptr->nbma_len, ptr->addr_type));
    }
    if (detail)
	nhrp_show_nei(ptr, fun);
    return(0);
}

/*
 * nhrp_show_cache
 * Display the cache.  If an interface was specified, limit display to just
 * that one interface.
 */

void nhrp_show_cache (ushort protoidx, idbtype *idb, int type, boolean detail)
{
    nhrp_cachemodetype mode;
    nhrpfuns *fun;

    mode = 0;
    switch (type) {
    case NHRP_SHOW_DYN:
	mode = NHRP_DYNAMIC;
	break;
    case NHRP_SHOW_STAT:
	mode = NHRP_STATIC;
	break;
    case NHRP_SHOW_INC:
	mode = NHRP_INCOMPLETE;
	break;
    }
    fun = nhrp_funmap(protoidx);
    if (!fun)
	return;
    automore_enable(NULL);
    nhrp_cache_walk(nhrp_show_cache_subr, fun, idb, mode, detail);
    automore_disable();
}

/*
 * nhrp_cache_clear_addr
 * Clear an NHRP cache table entry, as specified by the passed in network
 * layer address.
 */


void nhrp_cache_clear_addr (nhrpaddr *addr, nhrpfuns *fun)
{
    nhrpcachetype *ptr;

    ptr = nhrp_cache_lookup(NULL, addr, fun);
    if (ptr)
	nhrp_cache_delete(ptr->idb, ptr);
}

/*
 * nhrp_cache_clear_prefix
 * Clear an NHRP cache table entry, as specified by the passed in network
 * layer prefix.
 */


void nhrp_cache_clear_prefix (nhrpaddrnet *net, nhrpfuns *fun)
{
    nhrpcachetype *ptr;

    ptr = nhrp_cache_lookup_rnmask(NULL, net, fun, FALSE);
    if (ptr) {
	if (ptr->prefix.ip.mask != net->ip.mask)
	    return;
	/*
	 * Make sure cache entry was locally generated
	 */
	if ((ptr->flags & NHRP_GFLAG) == 0)
	    return;
	nhrp_cache_delete_subr(ptr);
    }
}

/*
 * nhrp_cache_nei
 * update cache entry with requestor's (neighbor's) information
 * Return an indication as to whether or not the update was successful.
 */

boolean nhrp_cache_nei (nhrpaddr *src, ulong reqid, nhrpcachetype *ptr,
			nhrpfuns *fun)
{
    nhrpcacherequestor *nbr;

    for (nbr = ptr->requests; nbr; nbr = nbr->next) {
	if ((*fun->nhrp_cmpaddr)(&nbr->src, src) == 0)
	    break;
    }
    if (!nbr) {
	nbr = malloc(sizeof(nhrpcacherequestor));
	if (!nbr)
	    return(FALSE);
	bcopy(src, &nbr->src, fun->nhrp_addlen);
	nbr->next = ptr->requests;
	nbr->protoidx = fun->protoidx;
	ptr->requests = nbr;
    }
    nbr->reqid = reqid;
    return(TRUE);
}

/*
 * nhrp_cache_init
 * Initialize the cache table for a particular interface.
 */

void nhrp_cache_init (rnhtype **nhrp_head)
{
    if (*nhrp_head == NULL)
	rn_inithead((void **) nhrp_head, IPRADIXDATAOFFSET);
}
