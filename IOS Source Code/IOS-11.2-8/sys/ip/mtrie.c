/* $Id: mtrie.c,v 3.2 1996/03/04 15:31:15 dkerr Exp $
 * $Source: /release/111/cvs/Xsys/ip/mtrie.c,v $
 *------------------------------------------------------------------
 * mtrie.c
 *
 * Radix M-way Trie support routines for IP route cache.
 *
 * Aug 1995, Darren Kerr
 *
 * The concepts and routines in this file were derived from and/or
 * based on Paul Traina's radix trie for storing IP prefix cache entries,
 * as implemented in "ip/ipfast.c" and "os/radix.c".  The base
 * functionality found herein roughly approximates a subset of the
 * aforementioned code, i.e., it contains support for
 * adding, removing, and displaying prefix cache entries using a
 * radix trie.
 *
 * This particular implementation differs mainly in the following:
 *
 *   _ Uses M-way internal nodes (with M=256) in order to compare
 *     8 bits at a time, thereby limiting worst-case lookup to 4
 *     iterations of the search loop.
 *
 *   _ Splits the ipcache_rntype struct into three different pieces:
 *     1) internal node with 256 child links and one (unused) parent link,
 *     2) external (leaf) node with ip prefix, mask, and timestamp
 *     3) a mac encapsulation rewrite structure.
 *
 *   _ Allows overlapping prefixes, so as to be suitable for routes.
 *
 * An 'm-node' is an internal radix node that contains exactly M
 * child links.  Each link can point to another m-node, a leaf cache
 * entry, or NULL.  Links to m-nodes are distinguished from links to leaf
 * cache entries by its least significant bit, which must be masked
 * before using.
 *
 * We have defined M to be 256, so that 8 bits can be examined at once.
 * The highest layer of the trie corresponds to the first octet of
 * an IP destination address, with subsequent bytes searched at lower
 * layers.  The depth of a search for a particular destination address
 * is directly proportional to the number of mask *bytes* in the prefix
 * cache entry (e.g., a 16-bit prefix cache entry will be found
 * the second level in the trie, whereas a host prefix (32-bits) will
 * be found at the fourth level.
 *
 * Variable length prefix masks are handled by filling in multiple array
 * elements of the last m-node, such that all possible destinations
 * satisfying the match will point to the correct route-cache entry.
 *

 * The following diagram shows what the trie would look like
 * if the following prefixes were added to the cache,
 * 13.0.0.0/8, 171.69.0.0/16, and 192.31.7.128/29.
 *
 *
 *                         x.x.x.x
 *          0----13----------171-----192------255
 *          |     $           *       *         |              Layer 1 Index
 *          +-----|-----------|-------|---------+
 *       .________|     ._____|       |________.
 *       |              |                      |
 *  13.0.0.0/8      171.x.x.x              192.x.x.x
 *       |              |                      |
 * +----------+  0------69---------255  0-----31----------255
 * |cache info|  |      $            |  |     *             |  Layer 2 Indexes
 * +----------+  +------|------------+  +-----|-------------+
 *                      |                     |
 *                  171.69.0.0/16        192.31.x.x    
 *                      |                     |
 *                 +-------------+      0---7-------------255
 *                 | cache info  |      |   *               |  Layer 3 Index
 *                 +-------------+      +---|---------------+
 *                                          |
 *                                         192.31.7.x
 *                                          |
 *                                      +----128...135----255
 *                                      |     ********      |  Layer 4 Index
 *                                      +-----||||||||------+
 *                                            ||||||||
 *                                         192.31.7.128/29
 *                                         +--------------+
 *                                         |  cache info  |
 *                                         +--------------+
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mtrie.c,v $
 * Revision 3.2  1996/03/04  15:31:15  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.1.2.6  1996/02/09  11:35:07  tli
 * Branch: Brat_branch
 * Checkpoint
 *
 * Revision 3.1.2.5  1996/01/23  20:37:01  dkerr
 * Branch: Brat_branch
 * Added "show ip cache flow" command.
 * Convert flow cache to use FIB.
 * Added flow-export.
 *
 * Revision 3.1.2.4  1996/01/23  11:00:37  tli
 * Branch: Brat_branch
 * Fix some uses of the IP address change callback.
 * Track IP addresses and subnet broadcast addresses in the FIB.
 * Repair a bug with ICMP redirects.
 * Clean up debugging.
 *
 * Revision 3.1.2.3  1996/01/08  07:53:52  tli
 * Branch: Brat_branch
 * Adding a prefix should cause resolution of routes with a next hop
 * pointing to a less specific prefix.  This insures that next hops for
 * the new prefix are correctly assigned.
 *
 * Revision 3.1.2.2  1996/01/05  15:45:25  tli
 * Branch: Brat_branch
 * Sync to V111_0_14
 *
 * Revision 3.1.2.1  1995/12/14  23:21:37  dkerr
 * CSCdi45613:  mtrie fib support
 * Branch: Brat_branch
 * M-way radix trie support routines.
 * Early drop support for unknown routes.
 *
 * Revision 3.1  1995/12/14  19:19:40  dkerr
 * mtrie placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "ip.h"
#include "mtrie.h"

/*
 * Assumes that calling function is allocating a larger struct with
 * an mtrie_leaf at the front.
 */
mtrie_leaf *
mtrie_leaf_alloc (mtrie_root *root, int bytes, boolean critical)
{
    mtrie_leaf *leaf;

    leaf = root->free_leaves;
    if (leaf && get_block_size(leaf, NULL) >= bytes) {
	root->free_leaves = leaf->back;
	root->leaves_pending--;
	bzero(leaf, bytes);
	return(leaf);
    }

    /*
     * Don't try to malloc at interrupt level
     */
    if (critical) {
	root->leaf_alloc_failed++;
	return(NULL);
    }
	  
    leaf = malloc_named_aligned(bytes, "IP mtrie leaf", RSP_DCACHE_LINE_SIZE);
    if (leaf) {
	root->leaves_alloced++;
	root->memory_in_use += get_block_size(leaf, NULL);
	mtrie_leaf_lock(root, leaf);
    } else
      root->leaf_alloc_failed++;
    return(leaf);
}

/*
 * Allocate an internal mtrie node during addition of mtrie leaves.
 */
static mtrie_node *
mtrie_node_alloc (mtrie_root *root)
{
    mtrie_node *mnode;

    mnode = root->free_nodes;
    if (mnode) {
	root->free_nodes = mnode->children[0];
	root->nodes_pending--;
	bzero(mnode, sizeof(mtrie_node));
	mnode->root = root;
	return(mnode);
    }

    /*
     * Don't try to malloc at interrupt level
     */
    if (critical) {
	root->node_alloc_failed++;
	return(NULL);
    }
	  
    mnode = malloc_named(sizeof(mtrie_node), "IP mtrie node");
    if (mnode) {
	root->nodes_alloced++;
	root->memory_in_use += sizeof(mtrie_node);
	mnode->root = root;
    } else
      root->node_alloc_failed++;
    return(mnode);
}

/*
 * Decrement mtrie leaf and free no one else is referencing.
 * If we free the leaf, also recursively decrement the refcount
 * of any less-specific overlapping route from back list.
 */
void
mtrie_leaf_free (mtrie_root *root, mtrie_leaf *leaf)
{
    if (!leaf)
	return;
    leaf->refcount--;
    root->leaf_refcount--;
    if (leaf->refcount <= 0) {
	if (leaf->back)
	    mtrie_leaf_free(root, leaf->back);
	if (root->onintstack) {
	    leaf->back = root->free_leaves;
	    root->free_leaves = leaf;
	    root->leaves_pending++;
	} else {
	    while (TRUE) {
		root->memory_in_use -= get_block_size(leaf, NULL);
		root->leaves_alloced--;
		free(leaf);
		leaf = root->free_leaves;
		if (!leaf)
		  break;
		root->leaves_pending--;
		root->free_leaves = leaf->back;
	    }
	}
    }
}

/*
 * Free mtrie internal node, and update global debug counters.
 */
static void
mtrie_node_free (mtrie_root *root, mtrie_node *node)
{
    if (root->onintstack) {
	node->children[0] = root->free_nodes;
	root->free_nodes = node;
	root->nodes_pending++;
    } else {
	while (TRUE) {
	    root->nodes_alloced--;
	    root->memory_in_use -= sizeof(mtrie_node);
	    free(node);
	    node = root->free_nodes;
	    if (!node)
	      break;
	    root->free_nodes = node->children[0];
	    root->nodes_pending--;
	}
    }
}

/*
 * Add a child leaf or node to parent node and increment the node's
 * child count (which is used when making pruning decisions).
 * If the child is a leaf, increment the child's refcount.
 */ 
static inline void
add_child_to_mnode (mtrie_root *root, mtrie_node *mnode,
		    mtrie_node *child, int idx)
{
    if (!mnode->children[idx]) {
	mnode->children[idx] = child;
	mnode->num_children++;
	root->node_refcount++;
	if (!MTRIE_NODE_BIT_IS_SET(child))
	    mtrie_leaf_lock(root, (mtrie_leaf *)child);
    }
}

/*
 * Remove a child leaf or node from a parent node.
 * If the child was a leaf with a non-NULL back link, this
 * implies that a less-specific overlapping route exists;  in
 * this case point the node link at the less specific route.
 */
static void
remove_child_from_mnode (mtrie_root *root, mtrie_node *mnode, int idx)
{
    mtrie_node *child = mnode->children[idx];
    mtrie_leaf *leaf, *old_leaf = NULL;

    if (MTRIE_NODE_BIT_IS_SET(child)) {
	child = MASK_MTRIE_NODE_BIT(child);
	if (child->num_children)
	    return;
	mtrie_node_free(root, child);
	mnode->children[idx] = NULL;
	mnode->num_children--;
	root->node_refcount--;
    } else if (child) { /* leaf */
	leaf = (mtrie_leaf *)child;
	old_leaf = leaf->back;
	mnode->children[idx] = (mtrie_node *)old_leaf;
	if (!old_leaf) {
	    /*
	     * No less specific overlapping route.  This index in
	     * the node is now empty, so decrement node child counter.
	     */
	    mnode->num_children--;
	    root->node_refcount--;
	} else {
	    /*
	     * Less specific overlapping route exists.  Increment refcount
	     * of less-specific route, since we just added an mnode link
	     * that points to it.
	     */
	    mtrie_leaf_lock(root, old_leaf);
	}
	/*
	 * Decrement refcount of leaf, since one less mnode link points
	 * at it.
	 */
	mtrie_leaf_free(root, leaf);
    }
}

/*
 * summarize_node
 *
 * If we've deleted any leaves in a node, then check to see
 * whether the node can be collapsed.  This might occur when
 * you add a more specific overlapping route, then later delete
 * that route.
 */
static void
summarize_node (mtrie_root *root, mtrie_node *parent,
		mtrie_node *child, int idx)
{
    int j;
    mtrie_node *leaf;
    
    if (child->num_children == 0) {
	/*
	 * Easy case.  There's no more children, so we surely can
	 * delete this puppy.
	 */
	remove_child_from_mnode(root, parent, idx);
    } else if (child->num_children == MNODE_LINKS) {
	/*
	 * There's still children in this mnode.
	 */
	leaf = child->children[0];
	if (MTRIE_NODE_BIT_IS_SET(leaf))
	  return;
	for (j=1; j<MNODE_LINKS; j++)
	    if (child->children[j] != leaf)
		return;
	/*
	 * All 256 of the child node's children are the same.
	 * We can summarize by replacing the child node with its
	 * 256 leaf references with a single leaf reference.
	 */
	parent->children[idx] = leaf;
	mtrie_leaf_lock(root, (mtrie_leaf *)leaf);

	/*
	 * This is an inefficient algorithm for fixing the refcounts,
	 * but left like this for simplicity.
	 */
	for (j=0; j<MNODE_LINKS; j++) {
	    mtrie_leaf_free(root, (mtrie_leaf *)leaf);
	    child->children[j] = NULL;
	}
	root->node_refcount -= MNODE_LINKS;
	mtrie_node_free(root, child);
    }
}

/*
 * Recursively delete all leaves and mnodes matching the prefix/mask.
 * This is complicated by the fact that we need to fill in holes
 * with less-specific routes when they exist.
 */
static void
delete_target (mtrie_node *mnode, uchar *pp, uchar *mp, mtrie_leaf *leaf)
{
    mtrie_root *root = mnode->root;
    mtrie_node *child;
    ulong prefix, mask, idx;
    mtrie_leaf *next, *last = NULL;

    mask = *mp;
    idx = prefix = *pp & mask;

    do {
	child = mnode->children[idx];
	if (MTRIE_NODE_BIT_IS_SET(child)) {
	    child = MASK_MTRIE_NODE_BIT(child);
	    delete_target(child, pp+1, mp+1, leaf);
	    summarize_node(root, mnode, child, idx);
	} else if ((mtrie_leaf *)child == leaf) {
	    remove_child_from_mnode(root, mnode, idx);
	} else if ((next = (mtrie_leaf *)child) != last) {
	    last = next;
	    while (next) {
		if (next->back == leaf) {
		    next->back = leaf->back;
		    leaf->back = NULL;
		    mtrie_leaf_free(root, leaf);
		    break;
		}
		next = next->back;
	    }
	}
    } while (((++idx & mask) == prefix) && (idx < MNODE_LINKS));
}

/*
 * Delete a route from the trie.
 */
void
mtrie_leaf_delete (mtrie_root *root, mtrie_leaf *leaf)
{
    charlong pb, mb;
    leveltype level;

    /*
     * Can't free leaves or nodes while at interrupt level.
     */
    root->onintstack = onintstack();
    
    /*
     * Set path and mask for quickly walking the mtrie to the
     * first matching leaves.
     */
    pb.d.lword = leaf->prefix;
    mb.d.lword = leaf->mask;
    root->leaf_invalidates++;
    level = raise_interrupt_level(NETS_DISABLE);
    delete_target(&root->node, pb.d.byte, mb.d.byte, leaf);
    reset_interrupt_level(level);
    root->onintstack = FALSE;
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
add_leaves (mtrie_node *mnode, mtrie_leaf *leaf, int idx, int bits)
{
    int leaves;
    mtrie_node *child;
    mtrie_root *root = mnode->root;

    for (leaves = 1 << (8 - bits); leaves > 0; leaves--, idx++) {
	child = mnode->children[idx];
	if (MTRIE_NODE_BIT_IS_SET(child)) {
	    /*
	     * Adding a less specific entry over a more specific entry.
	     * Fill in all empty leaves at all levels below this branch.
	     */
	    add_leaves(MASK_MTRIE_NODE_BIT(child), leaf, 0, 0);
	} else if (child) {
	    mtrie_leaf *old_leaf = (mtrie_leaf *)child;
	    if (leaf->mask > old_leaf->mask) {
		/*
		 * The new entry is more specific than the old entry,
		 * so replace the old entry with the new.  Make sure to
		 * set the new entry's back pointer at the old entry if
		 * we haven't already.
		 */
		mnode->children[idx] = (mtrie_node *)leaf;
		mtrie_leaf_lock(root, leaf);
		if (!leaf->back) {
		    leaf->back = old_leaf;
		    mtrie_leaf_lock(root, old_leaf);
		}
		mtrie_leaf_free(root, old_leaf);
	    } else if (leaf->mask < old_leaf->mask) {
		/*
		 * The new entry is less specific than the old.
		 * Search the old entry's back list for the least
		 * specific entry that is more specific than the
		 * new entry.  If that entry's back pointer doesn't
		 * already point to the new entry, then insert the
		 * new entry into that entry's back list.
		 */
		mtrie_leaf *back = old_leaf;
		while (back->back && back->back->mask > leaf->mask)
		    back = back->back;
		if (back->back != leaf) {
		    if (leaf->back)
			/*
			 * New entry already has common ancestor
			 * as back link, so decrement refcount.
			 */
			mtrie_leaf_free(root, back->back);
		    else
			/*
			 * Add ancestor to backlist.
			 */
			leaf->back = back->back;
		    back->back = leaf;
		    mtrie_leaf_lock(root, leaf);
		}
	    }
	} else
	    /*
	     * Easy case.  There was an empty leaf, so fill it in
	     * with this route.
	     */
	    add_child_to_mnode(root, mnode, (mtrie_node *)leaf, idx);
    }
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
 * point any link(s) needed in the current more-specific prefix's
 * path.
 */
static void
add_range (mtrie_root *root, mtrie_leaf *leaf, int bits)
{
    mtrie_node *child, *mnode = &root->node;
    int idx, j, shift;

    shift = IPADDR_BITS - MNODE_BITS;
    idx =  (leaf->prefix >> shift) & MNODE_MASK;

    while (bits > 8) {
	child = mnode->children[idx];
	if (!MTRIE_NODE_BIT_IS_SET(child)) {
	    mtrie_leaf *old = (mtrie_leaf *)child;
	    /*
	     * We don't have an mnode at this level, and there are more
	     * bits in the prefix, so allocate a new branch.
	     */
	    if (!(child = mtrie_node_alloc(root)))
	      return;
	
	    if (old) {
		/*
		 * We have a less-specific prefix leaf here.  In order to
		 * overlap prefixes, point all links of new mnode at
		 * the less-specific prefix leaf.
		 */
		for (j = 0; j < MNODE_LINKS; j++) {
		    child->children[j] = (mtrie_node *) old;
		    mtrie_leaf_lock(root, old);
		}
		child->num_children = MNODE_LINKS;
		root->node_refcount += MNODE_LINKS;
		/*
		 * Lost one link, added 256
		 */
		mtrie_leaf_free(root, old);
		mnode->num_children--;
		root->node_refcount--;
	    }
	    mnode->num_children++;
	    mnode->children[idx] = SET_MTRIE_NODE_BIT(child);
	    root->node_refcount++;
	}
	bits -= MNODE_BITS;
	shift -= MNODE_BITS;
	idx =  (leaf->prefix >> shift) & MNODE_MASK;
	mnode = MASK_MTRIE_NODE_BIT(child);
    }
    add_leaves(mnode, leaf, idx, bits);
}

mtrie_leaf *
mtrie_lookup_exact (mtrie_root *root, ipaddrtype dst, ipaddrtype mask)
{
    mtrie_leaf *leaf;
    
    leaf = mtrie_longest_match(root, dst);
    while (leaf) {
	if (leaf->prefix == dst && leaf->mask == mask)
	    break;
	leaf = leaf->back;
    }
    return(leaf);
}

/*
 * Add an IP prefix to the radix m-way trie
 * Leaf must be created using mtrie_leaf_alloc to init refcount.
 */
boolean
mtrie_leaf_insert (mtrie_root *root, mtrie_leaf *leaf)
{
    int mask_bits = ip_bitsinmask(leaf->mask);
    mtrie_leaf *next;
    leveltype level;

    /*
     * If this exact route already exists, just copy the new info
     * to the old descriptor.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    next = mtrie_lookup_exact(root, leaf->prefix, leaf->mask);
    if (next) {
	reset_interrupt_level(level);
	return(FALSE);
    }
	
    /*
     * Add all internal nodes and external leaves necessary to
     * cover complete range of prefix/mask.
     */
    add_range(root, leaf, mask_bits);
    root->leaf_inserts++;
    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * Recursively examine successive bytes in the prefix path
 * in order to find the first leaf in the trie whose prefix/mask is greater
 * than the previous.
 *
 * This is complicated by the fact that some leaves are hidden by
 * more-specific overlapping routes.  This requires walking the back
 * list.
 */
static mtrie_leaf *
next_leaf (mtrie_node *mnode, mtrie_leaf *last, uchar *path)
{
    mtrie_node *child;
    mtrie_leaf *next;

    do {
	child = mnode->children[*path];
	if (MTRIE_NODE_BIT_IS_SET(child)) {
	    next = next_leaf(MASK_MTRIE_NODE_BIT(child), last, path+1);
	    if (next)
	      return(next);
	} else {
	    next = (mtrie_leaf *) child;
	    if (next && next != last) {
		while (next->back &&
		       ((next->back->prefix > last->prefix) ||
			((next->back->prefix == last->prefix) &&
			 (next->back->mask > last->mask)))) {
		    next = next->back;
		}
		
		if (next->prefix > last->prefix ||
		    (next->prefix == last->prefix && next->mask > last->mask))
		    return(next);
	    }
	}
    } while ((*path += 1) != 0);
    return(NULL);
}

mtrie_leaf *
mtrie_next_leaf (mtrie_root *root, mtrie_leaf *last, uchar *path)
{
    return(next_leaf(&root->node, last, path));
}

/*
 * Find the first leaf in an mtrie matching the passed prefix/mask range.
 * This routine may be used in concert with mtrie_next_leaf to walk
 * a portion (or all) of an mtrie.
 */
mtrie_leaf *
mtrie_first_leaf (mtrie_root *root, ipaddrtype prefix, ipaddrtype mask,
		  uchar *path)
{
    mtrie_leaf *leaf, default_leaf;

    /*
     * See if there's an exact match.
     */
    leaf = mtrie_longest_match(root, prefix & mask);
    if (leaf && (leaf->prefix & mask) == prefix) {
	while (leaf->back && (leaf->back->prefix & mask) == prefix)
	    leaf = leaf->back;
	*(ipaddrtype *)path = leaf->prefix;
	return(leaf);
    }

    /*
     * No exact match.  Search for first more specific leaf within the
     * range of this route and mask.
     */
    *(ipaddrtype *)path = prefix & mask;
    default_leaf.prefix = prefix;
    default_leaf.mask = mask;
    
    leaf = mtrie_next_leaf(root, &default_leaf, path);
    return(leaf);
}

/* end of file */
