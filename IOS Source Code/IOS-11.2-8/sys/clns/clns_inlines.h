/* $Id: clns_inlines.h,v 3.2.60.2 1996/09/11 20:09:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_inlines.h,v $
 *------------------------------------------------------------------
 * Inline routines for fast CLNS operations
 *
 * November 1992, Dave Katz
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: clns_inlines.h,v $
 * Revision 3.2.60.2  1996/09/11  20:09:19  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/03  21:49:24  asastry
 * CSCdi52752:  High CPU utilization during large CLNS file transfers
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:52:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/03  22:58:11  rchandra
 * CSCdi39069:  Route entries all hash to same bucket
 * - Use the last 4 bytes of the NSAP for hashing
 * - include the code-review changes for CSCdi38153
 * - rebuild both L1 and L2 LSPs when redistribute command is entered
 *
 * Revision 2.2  1995/08/15  17:29:40  asastry
 * CSCdi34841:  Check for idb being NULL before using it.
 *
 * Revision 2.1  1995/06/07  20:17:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * fast_net_equ
 *
 *    Compare two nsaps and return true if the 'net's are equal
 *    (Don't pay attention to the selector byte).  Do it in
 *    fullwords first, and then in whatever bytes are left.
 *    We include the length byte in the comparison.
 *
 *    There is no length check--it better have been checked before
 *    it got here!
 *
 *    Note that we assume that the address length bytes are word aligned!
 */
static const ulong residual_mask[] = {0, 0xff000000, 0xffff0000, 0xffffff00};

static inline boolean fast_net_equ (
    NSAP_T nsap1,
    NSAP_T nsap2,
    ulong len)
{

    ulong wlen, residual;

    nsap1 += len & (~3);		/* Initially point at the NSEL word*/
    nsap2 += len & (~3);

    /* Compare the last word, masked appropriately to remove the NSEL. */

    residual = len % 4;
    if (residual) {
	if ((*((ulong *)nsap1) ^ *((ulong *) nsap2))
	    & residual_mask[residual])
	    return(FALSE);
    }

    /* Compare the rest of the words. */

    wlen = len / 4;
    while (wlen--) {
	if (*(--((ulong *)nsap1)) != *(--((ulong *) nsap2)))
	    return(FALSE);
    }
    return(TRUE);
}


/* 
 * fast_nsap_cache_search
 *
 * This routine will search the cache for the destination NSAP found
 * in the CLNP header.
 *
 * Note that our hash routine uses the last four bytes of the NSAP,
 * and thus NSAPs shorter than 4 cannot be used in this routine
 * (better slow switch 'em!).
 */
static inline clns_cache_adjtype *fast_nsap_cache_search (
    clns_fast_header *header, ulong dest_addr_len)
{

#define NSEL_MASK 0xffffff00

    clns_cachetype     *cache;
    clns_cache_adjtype *cache_adj;
    idbtype *idb;
    union {
	ulong lword;
	ushort sword[2];
    } hash;

    /* 
     * Extract the last 4 bytes of the NSAP, and zero out the NSEL,
     * before cache look-up.
     */
    hash.lword = GETLONG(&(header->dest[1]) + dest_addr_len - 4);
    hash.lword &= NSEL_MASK;
    hash.lword ^= hash.sword[0];
    hash.lword ^= hash.lword >> 8;
    for (cache = clns_cache[hash.lword & 0xff]; cache; cache = cache->next) {
	if (fast_net_equ(header->dest, cache->destination, dest_addr_len)) {
	    if (clns_cache_version == cache->cache_version) break;
	}
    }

    if (cache) {
	cache_adj = cache->cache_adj;

	/*
	 * Do not advance cache->cache_adj if fast switching is disabled.
	 * This is done so that when we jump to process switching, the
	 * cache look up will still point to the cache adj entry we
	 * want.
	 */
	idb = cache_adj->adj->idb;
	if (idb && idb->hwptr->clns_fastokay)
	    cache->cache_adj = cache_adj->next;
    } else {
	cache_adj = NULL;
    }

    return (cache_adj);
}

#define MIN_CACHE_SEARCH_LEN 5	/* Can't do less bytes than this */

