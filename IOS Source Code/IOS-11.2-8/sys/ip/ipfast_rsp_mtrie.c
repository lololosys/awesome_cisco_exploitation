/* $Id: ipfast_rsp_mtrie.c,v 3.1.24.3 1996/09/10 11:10:15 kao Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp_mtrie.c,v $
 *------------------------------------------------------------------
 * <One line description here.>
 *
 * <Month> 1996 Darren Kerr
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp_mtrie.c,v $
 * Revision 3.1.24.3  1996/09/10  11:10:15  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * move one global variable to fix a rsp-boot-mz build failure
 * Branch: California_branch
 *
 * Revision 3.1.24.2  1996/09/07  02:17:19  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase
 * the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.1.24.1  1996/04/30  21:18:09  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/12  18:47:33  dkerr
 * Placeholder for mtrie cache lookup.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../os/free.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_rsp_mtrie.h"

radixmnodetype ip_radixmnode_root;
ulong leaf_refcount;


static radixmnodetype *mnode_pending_head;
static ulong prefix_invalidate_target;
static ulong mnodes_alloced;
static ulong mnodes_pending;
static ulong memory_in_use;
static ulong mnode_alloc_failed;
static ulong total_adds;


static inline void
free_ipcache_entry (ipcache_rntype *cptr)
{
    adjust_leaf_refcount(cptr, -1);
}
	
/*
 * At process level, free any mnodes that couldn't be freed at
 * interrupt level.
 */
static void
free_pending_mnodes (void)
{
    radixmnodetype *mnode;
    leveltype level;

    if (mnode_pending_head && !onintstack()) {
	level = raise_interrupt_level(NETS_DISABLE);
	while ((mnode = mnode_pending_head) != NULL) {
	    mnode_pending_head = mnode->children[0];
	    mnodes_pending--;
	    memory_in_use -= sizeof(radixmnodetype);
	    mnodes_alloced--;
	    free(mnode);
	}
	reset_interrupt_level(level);
    }
}

static inline void
free_mnode_entry (radixmnodetype *mnode)
{
    if (onintstack()) {
	/*
	 * Can't free at interrupt level, so queue for later.
	 */
	mnode->children[0] = mnode_pending_head;
	mnode_pending_head = mnode;
	mnodes_pending++;
    } else {
	mnodes_alloced--;
	memory_in_use -= sizeof(radixmnodetype);
	free(mnode);
	free_pending_mnodes();
    }
}


static void
add_child_to_mnode (radixmnodetype *mnode, radixmnodetype *child, int idx)
{
    if (!mnode->children[idx]) {
	mnode->children[idx] = child;
	mnode->num_children++;
	if (!RADIXMNODE_BIT_IS_SET(child))
	    adjust_leaf_refcount((ipcache_rntype *)child, 1);
    }
}

static void
remove_child_from_mnode (radixmnodetype *mnode, int idx)
{
    radixmnodetype *child = mnode->children[idx];

    if (child) {
	if (RADIXMNODE_BIT_IS_SET(child)) {
	    child = MASK_RADIXMNODE_BIT(child);
	    if (child->num_children)
		return;
	    free_mnode_entry(child);
	} else
	    free_ipcache_entry((ipcache_rntype *)child);
	mnode->children[idx] = NULL;
	mnode->num_children--;
    }
}

static radixmnodetype *
create_mnode_entry (void)
{
    radixmnodetype *mnode = NULL;

    mnode = malloc_named(sizeof(radixmnodetype), "IP prefix mnode entry");
    if (mnode) {
	mnodes_alloced++;
	memory_in_use += sizeof(radixmnodetype);
    } else
	mnode_alloc_failed++;
    return(mnode);
}


/*
 * Recursively delete all leaves and mnodes matching the prefix/mask.
 */
static void
delete_range (radixmnodetype *mnode, uchar *pp, uchar *mp, hwidbtype *hwidb)
{
    radixmnodetype *child;
    ipcache_rntype *cptr;
    ulong prefix, mask, idx;

    mask = *mp;
    idx = prefix = *pp & mask;

    do {
	child = mnode->children[idx];
	if (RADIXMNODE_BIT_IS_SET(child)) {
	    child = MASK_RADIXMNODE_BIT(child);
	    delete_range(child, pp+1, mp+1, hwidb);
	    remove_child_from_mnode(mnode, idx);
	} else if ((cptr = (ipcache_rntype *)child) != NULL) {
	    remove_child_from_mnode(mnode, idx);
	}
    } while (((++idx & mask) == prefix) && (idx < MNODE_LINKS));
}

/*
 * Delete this route and any more specific routes from the cache.
 * This is registered through the sse delete callback.
 */
void
mnode_prefix_delete (ipcache_rntype *cptr)
{
    charlong pb, mb;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    pb.d.lword = cptr->prefix;
    mb.d.lword = cptr->mask;
    prefix_invalidate_target++;
    delete_range(&ip_radixmnode_root, pb.d.byte, mb.d.byte, NULL);
    
    reset_interrupt_level(level);
}

/*
 * Fill in the bottom-most mnode in a prefix's search path with
 * links to it's cache entry.
 *
 * This is complicated by the fact that if we allow overlapping prefixes,
 * there might be overlapping links to less-specific leaves, more specific
 * leaves, or even a lower-level mnode leading to a more-specific prefix.
 *
 * As a general rule, fill out all empty address space in the current
 * prefix's range, and replace any less-specific links.  This leaves
 * older, more-specific prefixes un-modified.
 */
static void
add_leaves (radixmnodetype *mnode, ipcache_rntype *cptr, int idx, int bits)
{
    int leaves;
    radixmnodetype *child;

    for (leaves = 1 << (8 - bits); leaves > 0; leaves--, idx++) {
	child = mnode->children[idx];
	if (RADIXMNODE_BIT_IS_SET(child)) {
	    /*
	     * Adding a less specific entry over a more specific entry.
	     * Fill in all empty leaves at all levels below this branch.
	     */
	    add_leaves(MASK_RADIXMNODE_BIT(child), cptr, 0, 0);
	} else if (child) {
	    ipcache_rntype *old_cptr = (ipcache_rntype *)child;
	    /*
	     * If the old entry was less specific than the new entry,
	     * then free the old one and replace with the new.
	     */
	    if (old_cptr->prefix <= cptr->prefix) {
		remove_child_from_mnode(mnode, idx);
		add_child_to_mnode(mnode, (radixmnodetype *)cptr, idx);
	    }
	} else
	    /*
	     * Easy case.  There was an empty leaf, so fill it in
	     * with this route.
	     */
	    add_child_to_mnode(mnode, (radixmnodetype *)cptr, idx);
    }
    total_adds++;
}

/*
 * Add any internal mnodes and external leaves necessary to represent
 * the complete IP address range of a prefix and mask.
 *
 * The first part of this routine traverses the unique path from the
 * root to the bottom-most mnode for this prefix.  This takes at
 * most three steps.  If any mnode in the path does not exist, it
 * creates one and inserts it into the trie.
 *
 * If a leaf is found where we need an mnode (i.e., a less-specific
 * overlapping prefix is already present in the trie), then create
 * an mnode and point all its links at the less-specific leaf.  Then
 * point the single link needed in the current more-specific prefix's
 * path.
 */
static boolean
add_range (radixmnodetype *mnode, ipcache_rntype *cptr, int bits)
{
    radixmnodetype *child;
    int idx, j, shift;

    shift = IPADDR_BITS - MNODE_BITS;
    idx =  (cptr->prefix >> shift) & MNODE_MASK;

    while (bits > 8) {
	child = mnode->children[idx];
	if (!RADIXMNODE_BIT_IS_SET(child)) {
	    ipcache_rntype *old = (ipcache_rntype *)child;
	    /*
	     * We don't have an mnode at this level, and there are more
	     * bits in the prefix, so allocate a new branch.
	     */
	    if (!(child = create_mnode_entry()))
	      return(FALSE);
	
	    if (old) {
		/*
		 * We have a less-specific prefix leaf here.  In order to
		 * overlap prefixes, point all links of new mnode at
		 * the less-specific prefix leaf.
		 */
		for (j = 0; j < MNODE_LINKS; j++)
		    add_child_to_mnode(child, (radixmnodetype *) old, j);
	    }
	    remove_child_from_mnode(mnode, idx);
	    add_child_to_mnode(mnode, SET_RADIXMNODE_BIT(child), idx);
	}
	bits -= MNODE_BITS;
	shift -= MNODE_BITS;
	idx =  (cptr->prefix >> shift) & MNODE_MASK;
	mnode = MASK_RADIXMNODE_BIT(child);
    }
    add_leaves(mnode, cptr, idx, bits);
    return(TRUE);
}

/*
 * Add an IP prefix to the radix m-way trie
 * This is registered through the sse update callback.
 */
void mnode_prefix_add (ipcache_rntype *cptr)
{
    leveltype level;
    charlong p;
    int mask_bits = ip_bitsinmask(cptr->mask);
    
    free_pending_mnodes();
    p.d.lword = cptr->prefix;

    /*
     * Don't want to worry about any critical regions in the process-level
     * add or delete code that would screw up interrupt level switching.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Add all internal nodes and external leaves necessary to
     * cover complete range of prefix/mask.
     */
    add_range(&ip_radixmnode_root, cptr, mask_bits);
    reset_interrupt_level(level);
}

/*
 * Recursively examine successive bit regions in the prefix key
 * in order to find the first leaf in the trie whose key is greater
 * than the search key.
 * If one is found, update the search key with the current entry's
 * prefix to increment a subsequent get_next.
 */
static ipcache_rntype *
get_next (radixmnodetype *mnode, ipaddrtype *key, uchar *path)
{
    radixmnodetype *child;
    ipcache_rntype *next;

    do {
	child = mnode->children[*path];
	if (RADIXMNODE_BIT_IS_SET(child)) {
	    next = get_next(MASK_RADIXMNODE_BIT(child), key, path+1);
	    if (next)
	      return(next);
	} else {
	    next = (ipcache_rntype *) child;
	    if (next && next->prefix > *key) {
		*key = next->prefix;
		return(next);
	    }
	}
    } while ((*path += 1) != 0);
    return(NULL);
}

/*
 * Walk the mtrie, using radix code to display leaves.
 */
void
optimum_ipcache_show (ipaddrtype prefix, ipaddrtype mask)
{
    ipaddrtype key;
    charlong path;
    ipcache_rntype *cptr;
    radixmnodetype *root = &ip_radixmnode_root;
    
    automore_enable(NULL);
    printf("\nOptimum Route Cache\n");
    printf("  %lu prefixes, %lu nodes, %d leaf refcount, %luK bytes\n",
	   total_adds - prefix_invalidate_target,
	   mnodes_alloced, leaf_refcount, memory_in_use/1024);
    printf("  %lu nodes pending, %lu node alloc failures\n",
	   mnodes_pending, mnode_alloc_failed);
    printf("  %lu prefix updates, %lu prefix invalidations\n",
	   total_adds, prefix_invalidate_target);
	   
    path.d.lword = key = prefix;
    if (key)
	key--;
    
    automore_header(ipcache_hdr);
    while ((cptr = get_next(root, &key, path.d.byte)) != NULL) {
	if ((cptr->prefix & mask) != prefix)
	    break;
	mem_lock(cptr);
	ipcache_show_entry(cptr, FALSE);
	free(cptr);
	if (automore_quit())
	    break;
	process_may_suspend();
    }
    automore_disable();
}

/* end of file */
