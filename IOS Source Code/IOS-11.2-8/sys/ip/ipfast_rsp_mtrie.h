/* $Id: ipfast_rsp_mtrie.h,v 3.1.24.1 1996/04/30 21:18:10 dkerr Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp_mtrie.h,v $
 *------------------------------------------------------------------
 * <One line description here.>
 *
 * <Month> 1996 Darren Kerr
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp_mtrie.h,v $
 * Revision 3.1.24.1  1996/04/30  21:18:10  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/12  18:48:07  dkerr
 * Placeholder for mtrie cache lookup needed by RSP fastswitching.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MNODE_BITS      8
#define MNODE_LINKS     (1 << MNODE_BITS)
#define MNODE_MASK      (MNODE_LINKS - 1)
#define IPADDR_BITS     32

typedef struct radixmnodetype_ {
    struct radixmnodetype_ *children[MNODE_LINKS];
    int                    num_children;
} radixmnodetype;
    

extern radixmnodetype ip_radixmnode_root;
extern ulong leaf_refcount;

/*
 * The RADIXMNODE_BIT hack allows us to differentiate a pointer to a leaf
 * from a pointer to another mnode.  We use bit 0, since valid
 * pointers to cache entries or arrays should never be odd byte addresses.
 */
#define RADIXMNODE_BIT 1
#define RADIXMNODE_BIT_IS_SET(p) ((int)p & RADIXMNODE_BIT)
#define SET_RADIXMNODE_BIT(p) ((radixmnodetype *)((int)p | RADIXMNODE_BIT))
#define MASK_RADIXMNODE_BIT(p) ((radixmnodetype *)((int)p & ~RADIXMNODE_BIT))

/*
 * The IP prefix cache is stored in a multiway radix trie, where each non-leaf
 * node is a 256 entry wide array of links.  With only four octets in an
 * IPv4 destination address, the trie is a maximum of 4 layers deep.
 */
static inline ipcache_rntype *
mnode_prefix_lookup (ipaddrtype dst)
{
    radixmnodetype *mnode;
    charlong prefix;
    uchar *cp;

    cp = prefix.d.byte;
    prefix.d.lword = dst;
    mnode = &ip_radixmnode_root;

    /*
     * Use successive octets in the IP address to index a pointer
     * to the next layer down until a leaf or null entry is found.
     */
    do
	mnode = MASK_RADIXMNODE_BIT(mnode)->children[*cp++];
    while (RADIXMNODE_BIT_IS_SET(mnode));

    return((ipcache_rntype *)mnode);
}

static inline void
adjust_leaf_refcount (ipcache_rntype *cptr, int count)
{
    if (cptr) {
	leaf_refcount += count;
	ipcache_adjust_refcount(cptr, count);
    }
}

/*
 * Externs
 */
extern void mnode_prefix_delete(ipcache_rntype *cptr);
extern void mnode_prefix_add(ipcache_rntype *cptr);
extern void optimum_ipcache_show(ipaddrtype prefix, ipaddrtype mask);

/* end of file */
