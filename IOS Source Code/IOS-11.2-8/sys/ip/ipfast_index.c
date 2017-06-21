/* $Id: ipfast_index.c,v 3.8.6.10 1996/09/05 23:06:25 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_index.c,v $
 *------------------------------------------------------------------
 * ipfast_index.c - IP prefix lookup routines using indexed m-way
 * radix trie.
 *
 * *************************************************************
 * *** Please see ipfast_index.h for description and diagram ***
 * *************************************************************
 *
 * Aug 1995, Darren Kerr
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_index.c,v $
 * Revision 3.8.6.10  1996/09/05  23:06:25  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.8.6.9  1996/08/30  16:41:26  drowell
 * CSCdi67055:  When VIP1 or VIP2 have 8MB DRAM dont run DFS
 * Branch: California_branch
 * Do not pre-allocate DFS arrays on a 8Meg VIP.
 *
 * Revision 3.8.6.8  1996/08/12  06:12:15  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.8.6.7  1996/06/06  09:24:50  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.8.6.6  1996/06/06  08:58:41  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.8.6.5  1996/05/28  16:13:43  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/05/04  01:03:33  wilber
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
 * Revision 3.8.6.3  1996/04/29  04:46:38  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/04/10  03:38:22  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.8.6.1  1996/03/18  20:16:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:58:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  09:46:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  00:59:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/06  21:27:48  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Make DFS dynamically react to DRAM size and grab cache memory
 * accordingly.
 *
 * Revision 3.7  1996/03/06  00:08:06  kao
 * CSCdi50785:  Space of entire DFS cache can be reduced
 *
 * Revision 3.6  1996/02/24  02:14:15  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.5  1996/02/23  22:05:21  dlobete
 * CSCdi49690:  Automore enable needed on sho ip cache at RVIP console
 *
 * Revision 3.4  1996/02/01  06:04:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/19  03:02:47  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:34:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:02  dlobete
 * Placeholders for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "../ui/debug.h"
#include "../os/free.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ip_registry.h"
#include "../ip/ip_debug_flags.h"
#include "../iprouting/route.h"
#include "../iprouting/iprouting_debug.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel.h"
#include "../if/network.h"
#include "../hes/sse_registry.h"
#include "../iprouting/parser_defs_iprouting.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../src-vip/dfs_vip_idb.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"

#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

radixmnodetype ip_radixmnode_root;
ulong ipfast_prefix_switched;
ulong ipfast_prefix_local_switched;
ulong ipfast_prefix_misses;
ulong ipfast_prefix_badiplen;
static ulong mnodes_alloced;
static ulong prefixes_alloced;
static ulong rewrites_alloced;
static ulong memory_in_use;
static ulong total_adds;
static ulong total_invalidates;

/*
 * Queues of index cache data structures filled
 * statically at init time and cultivated at process level -
 * in this way we avoid mallocs at interrupt level when cache
 * updates and invalidates happen.
 */
static 	queuetype 	encapinfo_list;
static 	queuetype 	ipcache_list;
static	queuetype	mnode_list;

/*
 * This should get us through initial field trials...
 */
#define MAX_MNODES_8MB        0       /* ~0KB */
#define MAX_PREFIXES_8MB      0       /* ~0KB */
#define MAX_REWRITES_8MB      0       /* ~0KB */

#define MAX_MNODES_16MB       4000    /* ~4MB */
#define MAX_PREFIXES_16MB     80000   /* ~2.7MB */
#define MAX_REWRITES_16MB     80000   /* ~5MB */

#define MAX_MNODES_32MB       8000    /* ~8MB */
#define MAX_PREFIXES_32MB     120000   /* ~4MB */
#define MAX_REWRITES_32MB     120000   /* ~7.5MB */

#define MAX_MNODES_64MB       16000    /* ~16MB */
#define MAX_PREFIXES_64MB     240000   /* ~8MB */
#define MAX_REWRITES_64MB     240000   /* ~15MB */

static ulong max_mnodes;
static ulong max_prefixes;
static ulong max_rewrites;

 
static encapinfotype *create_rewrite_entry (uchar *mac, ulong len,
					    void *hwidb, ulong ipmtu)
{
    encapinfotype *eptr = NULL;

    if (rewrites_alloced < max_rewrites) {
	/*
	 * Pluck from encapinfo_list.
	 */
	eptr = encapinfo_list.qhead;
	if (eptr) {
	    if (len > MAX_ENCAP_DFS)
		return(NULL);

            /*
             * The evector will be non-NULL if we DFS
             * to this type of output interface.
             */
            if (!(eptr->evector = get_ipencaps_vector(hwidb)))
                return(NULL);

            /*
             * Assign crypto encap vector 
             * Should add default vector here.
             */
            reg_invoke_crypto_engine_vip_register_evector(eptr);

	    encapinfo_list.qhead = eptr->next;
	    eptr->next = NULL;
	    encapinfo_list.count--;

	    eptr->idb = hwidb;
	    eptr->net_mtu = ipmtu;
	    eptr->length = len;
	    bcopy(mac, eptr->m.macstring, len);

	    rewrites_alloced++;
	}
    }
    return(eptr);
}

static void free_rewrite_entry (encapinfotype *eptr)
{
    if (eptr && eptr->refcount <= 0) {
	rewrites_alloced--;
	/*
	 * Return to encapinfo_list.
	 */
        eptr->next = encapinfo_list.qhead;
        encapinfo_list.qhead = eptr;
        encapinfo_list.count++;
    }
}

/*
 * Alloc and initialize a prefix cache entry.
 */
static ipcachetype *create_ipcache_entry (ipaddrtype prefix, ipaddrtype mask,
					ipaddrtype nexthop, ulong output_acl,
					  ulong flags)
{
    ipcachetype *cptr = NULL;

    if (prefixes_alloced < max_prefixes) {
	cptr = ipcache_list.qhead;
	if (cptr) {
            ipcache_list.qhead = cptr->next;
            cptr->next = NULL;
            ipcache_list.count--;

	    cptr->prefix = prefix;
	    cptr->mask = mask;
	    cptr->mask_bits = ip_bitsinmask(mask);
	    cptr->nexthop = nexthop;
	    cptr->ip_output_accesslist = output_acl;
	    cptr->flags = flags;
	    cptr->refcount = 0;
	    GET_TIMESTAMP(cptr->last_update);
	    prefixes_alloced++;
	}
    }
    return(cptr);
}

void free_ipcache_entry (ipcachetype *cptr)
{
    if (cptr && cptr->refcount <= 0) {

	/*
	 * Not sure how this can happen to be a NULL
	 * pointer but protect against it anyway -
	 * see CSCdi43302.
	 */
	if (cptr->eptr) {
	    cptr->eptr->refcount--;
	    free_rewrite_entry(cptr->eptr);
	    cptr->eptr = NULL;
	}

        /*
         * Return to ipcache_list.
         */
        cptr->next = ipcache_list.qhead;
        ipcache_list.qhead = cptr;
        ipcache_list.count++;

	prefixes_alloced--;
    }
}

static radixmnodetype *create_mnode_entry (void)
{
    radixmnodetype *mnode = NULL;

    if (mnodes_alloced < max_mnodes) {
	mnode = mnode_list.qhead;
	if (mnode) {
            mnode_list.qhead = mnode->children[0];
            mnode->children[0] = NULL;
            mnode_list.count--;
	    mnode->refcount = 0;
	    mnodes_alloced++;
	}
    }
    return(mnode);
}

static void free_mnode_entry (radixmnodetype *mnode)
{
    if (mnode && mnode->refcount <= 0) {
	mnodes_alloced--;
        /*
         * Return to mnode_list.
         */
        mnode->children[0] = mnode_list.qhead;
        mnode_list.qhead = mnode;
        mnode_list.count++;

    }
}

/*
 * Free 'count' child links (and all their children as well) starting
 * at offset 'idx' in the radix m-node array.
 * If hwidb is set, don't free cache entries pointing to different hwidb.
 */
static void free_range (radixmnodetype *mnode, int idx, int count,
			void *hwidb)
{
    radixmnodetype *child;
    int max = (idx + count - 1) & 0xFF;
    
    for (; idx <= max; idx++) {
	child = mnode->children[idx];
	if (child) {
	    if (RADIXMNODE_BIT_IS_SET(child)) {
		child = MASK_RADIXMNODE_BIT(child);
		free_range(child, 0, 256, hwidb);
		if (child->refcount == 0) {
		    free_mnode_entry(child);
		    mnode->children[idx] = NULL;
		    mnode->refcount--;
		}
	    } else {
		ipcachetype *cptr = (ipcachetype *)child;
		
		if (!hwidb || (cptr->eptr->idb == hwidb)) {
		    cptr->refcount--;
		    cptr->flags &= ~IPCACHE_FLAG_VALID;
		    free_ipcache_entry(cptr);
		    mnode->children[idx] = NULL;
		    mnode->refcount--;
		}
	    }
	}
    }
}

/*
 * Remove an IP prefix from the mway radix trie
 */
void ipfast_prefix_delete (ipaddrtype prefix, int match_bits, 
			   void *hwidb)
{
    radixmnodetype *mnode, *next, *parent;
    uchar prefix_bytes[4], *cp, idx;
    int bits, leaves;

    *(ulong *)prefix_bytes = prefix;
    cp = &prefix_bytes[0];
    idx = *cp++;
    mnode = &ip_radixmnode_root;

    for (bits = match_bits; bits > 8; bits -= 8, idx = *cp++) {
	next = mnode->children[idx];
	if (!RADIXMNODE_BIT_IS_SET(next)) {
	    bits = 8;
	    break;
	} else
	  mnode = MASK_RADIXMNODE_BIT(next);
    }
    leaves = 1 << (8 - bits);
    free_range(mnode, idx, leaves, hwidb);

    /*
     * Now go back up through ancestors and delete any single-entry nodes.
     */
    while (mnode != &ip_radixmnode_root && mnode->refcount <= 0) {
	parent = mnode->parent;
	free_mnode_entry(mnode);
	parent->children[mnode->index] = NULL;
	parent->refcount--;
	mnode = parent;
    }
    total_invalidates++;
}

/*
 * Add an IP prefix to the radix m-way trie
 */
boolean ipfast_prefix_add (ipcachetype *cptr)
{
    radixmnodetype *mnode, *child;
    uchar *cp, idx;
    int bits, leaves;
    ipaddrtype prefix;
    int match_bits;

    match_bits = ip_bitsinmask(cptr->mask);
    prefix = cptr->prefix;
    cp = (uchar *)&cptr->prefix;
    idx = *cp++;
    mnode = &ip_radixmnode_root;

    for (bits = match_bits; bits > 8; bits -= 8, idx = *cp++) {
	child = mnode->children[idx];
	if (!RADIXMNODE_BIT_IS_SET(child)) {
	    if (child)
	      /*
	       * We used to have a leaf here, but now we need a branch.
	       * Free this entry.
	       */
	      free_range(mnode, idx, 1, NULL);
	    
	    /*
	     * We don't have a branch at this level, and there are more
	     * bits in the prefix, so allocate a new branch.
	     */
	    if (!(child = create_mnode_entry()))
	      return(FALSE);
	    mnode->children[idx] = SET_RADIXMNODE_BIT(child);
	    mnode->refcount++;
	    child->index = idx;
	    child->parent = mnode;
	}
	mnode = MASK_RADIXMNODE_BIT(child);
    }
    leaves = 1 << (8 - bits);

    free_range(mnode, idx, leaves, NULL);
    
    cptr->refcount += leaves;
    mnode->refcount += leaves;
    while (leaves-- > 0)
      mnode->children[idx++] = (radixmnodetype *)cptr;

    total_adds++;
    return(TRUE);
}


/*
 * Called through IPC routines from RSP
 *
 * LATER: use nexthop as a mac rewrite search key so that all
 * entries with the same nexthop address can share a common
 * encapinfotype struct.  This should save memory as well as
 * lowering the cpu's dcache thrashing.
 */
void add_ipcache_entry (ipaddrtype prefix, ipaddrtype mask,
			ipaddrtype nexthop, ulong ip_mtu,
			ulong output_acl, ulong len, 
			uchar *mac, void *hwidb)
{
    ipcachetype *cptr;
    encapinfotype *eptr;
    
    cptr = create_ipcache_entry(prefix, mask, nexthop, output_acl,
				IPCACHE_FLAG_VALID);
    if (cptr) {
	eptr = create_rewrite_entry(mac, len, hwidb, ip_mtu);
	if (eptr) {
	    cptr->eptr = eptr;
	    eptr->refcount++;
	    if (ipfast_prefix_add(cptr))
	      return;
	}
	free_ipcache_entry(cptr);
    }
}
	
static boolean display_entry (ipcachetype *entry)
{
    int len, j;
    char time[20];
    uchar c[4];
    encapinfotype *eptr = entry->eptr;

    sprint_dhms(time, entry->last_update);

    *(ulong *)c = entry->prefix;
    len = printf("%d.%d.%d.%d/%d", c[0], c[1], c[2], c[3],
		 ip_bitsinmask(entry->mask));
    printf("%*c %9s %15s ", 19-len, ' ',
	   time, dfs_get_namestring(eptr->idb));

    for (j = 0; j < eptr->length; j++) {
	if ((j % 16 == 0) && j)
	  printf("\n%46c", ' ');
	printf("%-02x", eptr->m.macstring[j]);
    }
    printf("\n");

    if (automore_quit())
	return(FALSE);

    process_may_suspend();

    return(TRUE);
}

/*
 * This has a hole, since we may delete an mnode sub-trie during display.
 * Later: fix this.  Lots of options.
 */
static boolean show_range (radixmnodetype *mnode, ulong offset, ulong leaves)
{
    ipcachetype *cptr;
    radixmnodetype *next, *prev = NULL;
    
    for (; leaves--; offset++) {
	next = mnode->children[offset];
	if (!next)
	  continue;

	if (RADIXMNODE_BIT_IS_SET(next)) {
	    if (!show_range(MASK_RADIXMNODE_BIT(next), 0, LINKS_PER_MNODE))
	      return(FALSE);
	} else if (next != prev) {
	    cptr = (ipcachetype *)next;
	    if (!display_entry(cptr))
		return(FALSE);
	    prev = next;
	}
    }
    return(TRUE);
}

/*
 * Show all leaves matching prefix and mask.
 */
void ipfast_prefix_show (ipaddrtype prefix, int match_bits)
{
    radixmnodetype *mnode, *next;
    uchar prefix_bytes[4], *cp, idx;
    int bits, leaves;

    *(ulong *)prefix_bytes = prefix;
    cp = &prefix_bytes[0];
    idx = *cp++;
    mnode = &ip_radixmnode_root;

    automore_enable(NULL);

    for (bits = match_bits; bits > 8; bits -= 8, idx = *cp++) {
	next = mnode->children[idx];

	if (!RADIXMNODE_BIT_IS_SET(next)) {
	    /*
	     * A less specific match was found.  Show it anyway.
	     */
	    bits = 8;
	    break;
	}
	mnode = MASK_RADIXMNODE_BIT(next);
    }
    leaves = 1 << (8 - bits);

    show_range(mnode, idx, leaves);
}

void ipcache_show (parseinfo *csb)
{
    ipaddrtype prefix, mask;

    static const char hdr[] =
	"Prefix/Length       Age       Interface       MAC Header\n";
    
    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	prefix = GETOBJ(paddr,1)->ip_addr;
	mask   = GETOBJ(paddr,2)->ip_addr;
	prefix &= mask;
    } else
	prefix = mask = 0;
    
    printf("IP radix m-way trie cache  %lu entries, %lu tables\n",
	   prefixes_alloced, mnodes_alloced);
    printf("  %lu unique encapsulations,  %lu total bytes used by cache\n",
	   rewrites_alloced, memory_in_use);
    printf("  %lu adds, %lu invalidates, %lu switched, %lu local-switched, %lu misses\n",
 	   total_adds, total_invalidates, ipfast_prefix_switched,
	   ipfast_prefix_local_switched, ipfast_prefix_misses);
    printf("  %lu badiplen\n", ipfast_prefix_badiplen);	   

    automore_header(hdr);
    
    ipfast_prefix_show(prefix, ip_bitsinmask(mask));
    automore_disable();

    vip_ipflow_show(prefix, mask);
}


/*
 * ipcache_free_resources
 * Flush IP routing cache (called from command line)
 */

void
ipcache_free_resources (parseinfo *csb)
{
    ipaddrtype prefix, mask;
    
    if ((GETOBJ(paddr,1)->type == ADDR_IP) &&
	(GETOBJ(paddr,2)->type == ADDR_IP)) {

	prefix = GETOBJ(paddr,1)->ip_addr;
	mask   = GETOBJ(paddr,1)->ip_addr;
	prefix &= mask;

	ipfast_prefix_delete(prefix, ip_bitsinmask(mask), NULL);
    } else
	ipfast_prefix_delete(0, 0, NULL);
}


/*
 * ipfast_vip_init_lists
 * Initialize lists used for the index cache.
 */
void ipfast_vip_init_lists (ulong mem_size)

{
    ipcachetype *cptr;
    encapinfotype *eptr;
    radixmnodetype *mnode;
    int i, size;
   
    switch (mem_size) {
      case DFS_64MB_SIZE:
        max_prefixes    = MAX_PREFIXES_64MB;
        max_rewrites    = MAX_REWRITES_64MB;
        max_mnodes      = MAX_MNODES_64MB;
        break;

      case DFS_32MB_SIZE:
	max_prefixes    = MAX_PREFIXES_32MB;
	max_rewrites    = MAX_REWRITES_32MB;
	max_mnodes      = MAX_MNODES_32MB;
	break;

      case DFS_16MB_SIZE:
        max_prefixes    = MAX_PREFIXES_16MB;
        max_rewrites    = MAX_REWRITES_16MB;
        max_mnodes      = MAX_MNODES_16MB;
        break;

      case DFS_8MB_SIZE:
      default:
        max_prefixes    = MAX_PREFIXES_8MB;
        max_rewrites    = MAX_REWRITES_8MB;
        max_mnodes      = MAX_MNODES_8MB;
	break;
    }

    /*
     * Initialize queues.
     */
    ipcache_list.maximum = max_prefixes;
    encapinfo_list.maximum = max_rewrites;
    mnode_list.maximum = max_mnodes;

    /*
     * Do an initial allocation to fill queues.
     * Grow/shrink in periodic process-level task.
     */
    ipcache_list.count = 0;
    ipcache_list.qhead = NULL;
    ipcache_list.qtail = NULL;
    if (max_prefixes) {
        size = max_prefixes * sizeof(ipcachetype);
	cptr = malloc_named(size, "IP prefix cache");
	if (cptr) {
	    memory_in_use += size;
	    ipcache_list.count = max_prefixes;
	    ipcache_list.qhead = cptr;
	    for(i = 0; i < max_prefixes - 1; i++, cptr++)
	        cptr->next = cptr + 1;
	    cptr->next = NULL;
	    ipcache_list.qtail = cptr;
	}
    }
  
    encapinfo_list.count = 0;
    encapinfo_list.qhead = NULL;
    encapinfo_list.qtail = NULL;
    if (max_rewrites) {
        size = max_rewrites * sizeof(encapinfotype);
	eptr = malloc_named(size, "IP encap info");
	if (eptr) {
	    memory_in_use += size;
	    encapinfo_list.count = max_rewrites;
	    encapinfo_list.qhead = (encapinfotype *)eptr;
	    for(i = 0; i < max_rewrites - 1; i++, eptr++)
	        eptr->next = eptr + 1;
	    eptr->next = NULL;
	    encapinfo_list.qtail = eptr;
	}
    }

    mnode_list.count = 0;
    mnode_list.qhead = NULL;
    mnode_list.qtail = NULL;
    if (max_mnodes) {
        size = max_mnodes * sizeof(radixmnodetype);
	mnode = malloc_named(size, "IP prefix mode entry");
	if (mnode) {
	    memory_in_use += size;
	    mnode_list.count = max_mnodes;
	    mnode_list.qhead = mnode;
	    for(i = 0; i < max_mnodes - 1; i++, mnode++)
	        mnode->children[0] = mnode + 1;
	    mnode->children[0] = NULL;
	    mnode_list.qtail = mnode;
	}
    }
}
