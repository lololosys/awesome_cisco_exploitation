/* $Id: mtrie.h,v 3.2 1996/03/04 15:31:17 dkerr Exp $
 * $Source: /release/111/cvs/Xsys/ip/mtrie.h,v $
 *------------------------------------------------------------------
 * mtrie.h
 *
 * Aug 1995, Darren Kerr
 *
 * IP prefix lookup structures for m-way radix trie.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mtrie.h,v $
 * Revision 3.2  1996/03/04  15:31:17  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 *
 * Revision 3.1.2.3  1996/01/08  07:53:53  tli
 * Branch: Brat_branch
 * Adding a prefix should cause resolution of routes with a next hop
 * pointing to a less specific prefix.  This insures that next hops for
 * the new prefix are correctly assigned.
 *
 * Revision 3.1.2.2  1996/01/05  15:45:26  tli
 * Branch: Brat_branch
 * Sync to V111_0_14
 *
 * Revision 3.1.2.1  1995/12/14  23:21:38  dkerr
 * CSCdi45613:  mtrie fib support
 * Branch: Brat_branch
 * M-way radix trie support routines.
 * Early drop support for unknown routes.
 *
 * Revision 3.1  1995/12/14  19:19:59  dkerr
 * mtrie placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MNODE_BITS      8
#define MNODE_LINKS     (1 << MNODE_BITS)
#define MNODE_MASK      (MNODE_LINKS - 1)
#define IPADDR_BITS     32

typedef struct mtrie_node_ mtrie_node;
typedef struct mtrie_leaf_ mtrie_leaf;
typedef struct mtrie_root_ mtrie_root;

struct mtrie_node_ {
    mtrie_node *children[MNODE_LINKS];
    mtrie_root *root;
    int        num_children;
};

struct mtrie_leaf_ {
    mtrie_leaf   *back;    /* next most-specific route */
    ipaddrtype   prefix;
    ipaddrtype   mask;
    int          refcount;
};

struct mtrie_root_ {
    mtrie_node node;
    ulong leaf_invalidates;
    ulong nodes_alloced;
    ulong leaves_alloced;
    ulong memory_in_use;
    ulong leaf_alloc_failed;
    ulong node_alloc_failed;
    ulong leaf_inserts;
    ulong leaf_refcount;
    ulong node_refcount;
    ulong nodes_pending;
    ulong leaves_pending;
    mtrie_node *free_nodes;
    mtrie_leaf *free_leaves;
    boolean onintstack;
}

/*
 * The RADIXMTRIE_BIT hack allows us to differentiate a pointer to a leaf
 * from a pointer to another MTRIE.  We use bit 0, since valid
 * pointers to cache entries or arrays should never be odd byte addresses.
 */
#define MTRIE_NODE_BIT 1
#define MTRIE_NODE_BIT_IS_SET(p) ((int)p & MTRIE_NODE_BIT)
#define SET_MTRIE_NODE_BIT(p) ((mtrie_node *)((int)p | MTRIE_NODE_BIT))
#define MASK_MTRIE_NODE_BIT(p) ((mtrie_node *)((int)p & ~MTRIE_NODE_BIT))

/*
 * The IP prefix cache is stored in a multiway radix trie, where each non-leaf
 * node is a 256 entry wide array of links.  With only four octets in an
 * IPv4 destination address, the trie is a maximum of 4 layers deep.
 */
static inline mtrie_leaf *
mtrie_longest_match (mtrie_root *root, ipaddrtype dst)
{
    mtrie_node *node = &root->node;
    charlong prefix;
    uchar *cp;

    cp = prefix.d.byte;
    prefix.d.lword = dst;

    /*
     * Use successive octets in the IP address to index a pointer
     * to the next layer down until a leaf or null entry is found.
     */
    do
	node = MASK_MTRIE_NODE_BIT(node)->children[*cp++];
    while (MTRIE_NODE_BIT_IS_SET(node));

    return((mtrie_leaf *)node);
}

/*
 * We're using these rather than mem_lock/free for performance reasons
 * due to very high refcount usage in large routing table with overlapping
 * routes.
 */
static inline void
mtrie_leaf_lock (mtrie_root *root, mtrie_leaf *leaf)
{
    root->leaf_refcount++;
    leaf->refcount++;
}

/*
 * mtrie_less_specific
 *
 * Passed a leaf, return the most specific covering leaf.
 * Fortunately, we already track this.
 */
static inline mtrie_leaf *
mtrie_less_specific (mtrie_leaf *leaf)
{    
    return(leaf->back);
}

extern void mtrie_leaf_free(mtrie_root *root, mtrie_leaf *leaf);
extern void mtrie_leaf_delete(mtrie_root *root, mtrie_leaf *leaf);
extern boolean mtrie_leaf_insert(mtrie_root *root, mtrie_leaf *cptr);
extern mtrie_leaf *mtrie_lookup_exact(mtrie_root *, ipaddrtype, ipaddrtype);
extern mtrie_leaf *mtrie_leaf_alloc(mtrie_root *root, int bytes);
extern mtrie_leaf *mtrie_next_leaf(mtrie_root *, mtrie_leaf *, uchar *);
extern mtrie_leaf *mtrie_first_leaf(mtrie_root *, ipaddrtype, ipaddrtype,
				    uchar *);


