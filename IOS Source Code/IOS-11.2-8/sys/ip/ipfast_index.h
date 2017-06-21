/* $Id: ipfast_index.h,v 3.5.6.7 1996/08/12 06:12:17 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_index.h,v $
 *------------------------------------------------------------------
 * ipfast_index.h
 *
 * Aug 1995, Darren Kerr
 *
 * IP prefix lookup structures for indexed m-way radix trie.
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
 *
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_index.h,v $
 * Revision 3.5.6.7  1996/08/12  06:12:17  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.5.6.6  1996/06/06  08:58:42  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.5.6.5  1996/05/28  16:13:45  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/04/29  04:46:40  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/04/10  03:38:24  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/04/08  13:44:43  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/03/21  22:50:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:26:24  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  17:28:21  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  07:47:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.5  1996/03/06  00:08:09  kao
 * CSCdi50785:  Space of entire DFS cache can be reduced
 *
 * Revision 3.4  1996/02/24  02:14:19  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.3  1996/01/19  03:02:48  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:34:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:01  dlobete
 * Placeholders for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define LINKS_PER_MNODE   256
#define MNODE_MASK        (LINKS_PER_MNODE - 1)

typedef struct radixmnodetype_ {
    struct radixmnodetype_ *children[LINKS_PER_MNODE];
    struct radixmnodetype_ *parent;
    ipaddrtype             prefix;
    ipaddrtype             mask;
    uchar                  bits;
    uchar                  index;
    short                  refcount;
} radixmnodetype;
    

extern radixmnodetype ip_radixmnode_root;


typedef boolean (*encapvectortype)(paktype *pak, encapinfotype *eptr,
				   hwidbtype *input, int input_encap_len,
				   ulong netlen, void *bhptr);

/*
 * Everything needed to know to re-encapsulate an IP packet.
 * These fields used to be part of the ip cache structure, but
 * separating them allows all routes with the same next hop to
 * share a common rewrite.  This saves memory and reduces cache
 * thrashing.
 *
 * The 'm' field needs to be last, since it's just a placeholder
 * for mac rewrite.
 *
 * Currently tokenring without RIF is the largest encapsulation that
 * we support.  No mongo smds or multiring crap yet.
 * If we change the below size, make sure that the particle buffer
 * has room for it.
 */
#define MAX_ENCAP_DFS   22

struct encapinfotype_ {
    struct encapinfotype_    *next;    /* for chaining multicast floods */
    void                     *idb;     /* output hw interface */
    uchar                    length;   /* encap size *including* AC/FC */
    ushort                   net_mtu;  /* network mtu */
    int                      refcount; /* # prefixes and flows using it */
    encapvectortype          evector;  /* encapsulation rewrite vector */
    union {
	ulong  maclong[0];
	ushort macshort[0];
	uchar  macstring[0];
	uchar  macchar[MAX_ENCAP_DFS];
    } m;                               /* mac rewrite data start */
};

/*
 * IP routing cache data structure
 * 
 */
struct ipcachetype_ {
    struct ipcachetype_ *next;         /* for load sharing? */
    ipaddrtype		prefix;        /* IP network address prefix */
    ipaddrtype		mask;          /* significant bits in prefix */
    ipaddrtype		nexthop;       /* to find encaps */
    sys_timestamp	last_update;   /* last time updated */
    radixmnodetype      *parentnode;   /* reverse link to parent in trie */
    
    /*
     * Most of the following fields are used during switching fast path.
     * Try to keep them contiguous to avoid cache thrashing.
     */
    encapinfotype       *eptr;         /* encapsulation rewrite stuff */
    short		refcount;      /* # ipflows referencing it */
    uchar		flags;
    uchar		mask_bits;     /* # of bits in prefix mask */
    ulong		ip_output_accesslist;
};

#define	IPCACHE_FLAG_RECURSIVE	0x01	/* built through recursive route */
#define	IPCACHE_FLAG_VALID	0x02	/* referenced from secondary cache */


/*
 * The RADIXMNODE_BIT hack allows us to differentiate a pointer to a leaf
 * from a pointer to another index (array).  We use bit 0, since valid
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
static inline ipcachetype *ipfast_prefix_lookup (ipaddrtype dst)
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

    return((ipcachetype *)mnode);
}


/*
 * Externs
 */
extern void ipfast_prefix_delete(ipaddrtype pref, int bits, void *hwidb);
extern ulong ipfast_prefix_badiplen;
extern void add_ipcache_entry(ipaddrtype prefix, ipaddrtype mask,
			      ipaddrtype nexthop, ulong ip_mtu,
			      ulong output_acl, ulong len, 
			      uchar *mac, void *hwidb);
extern void free_ipcache_entry(ipcachetype *cptr);
extern char *dfs_get_namestring(void *hwidb);
extern int dfs_get_slotunit(void *hwidb);
extern void ipfast_vip_init_lists(ulong);

extern ulong ipfast_prefix_switched;
extern ulong ipfast_prefix_local_switched;
extern ulong ipfast_prefix_misses;
