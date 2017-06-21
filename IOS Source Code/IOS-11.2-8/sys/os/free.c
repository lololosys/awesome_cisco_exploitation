/* $Id: free.c,v 3.10.12.8 1996/09/06 01:11:33 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/free.c,v $
 *------------------------------------------------------------------
 * free.c -- free storage manager
 *  
 * June 1985, Kirk Lougheed
 * April 1990, Chris Shaker - Rewritten for Pancake
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: free.c,v $
 * Revision 3.10.12.8  1996/09/06  01:11:33  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.10.12.7  1996/07/10  22:22:04  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.10.12.6  1996/06/04  05:16:00  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.10.12.5  1996/04/13  06:05:26  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.10.12.4  1996/04/10  21:16:22  iwu
 * CSCdi53678:  mempool low_threshold & fragment_threshold initialized
 * incorrectly
 * Branch: California_branch
 *
 * Revision 3.10.12.3  1996/04/02  05:37:41  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.10.12.2  1996/03/21  23:21:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.12.1  1996/03/18  21:29:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.6  1996/03/18  23:25:45  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.5  1996/02/12  23:47:19  mbeesley
 * CSCdi48761:  Need to be able to delete memory alias
 * Branch: ELC_branch
 *
 * Revision 3.3.2.4  1996/02/10  00:18:01  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.3  1996/02/08  08:42:42  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/16  01:02:20  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:11:01  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.10  1996/02/12  18:18:25  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.9  1996/02/08  20:13:06  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/01/25  18:37:55  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.7  1996/01/22  06:56:09  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/05  04:56:33  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/15  22:34:20  hampton
 * Fix indirection through a NULL pointer.  [CSCdi45597]
 *
 * Revision 3.4  1995/12/07  22:12:40  hampton
 * Add a couple of new routines for naming memory blocks.  Also add a
 * routine for specifying the allocator of the memory block.  This routine
 * is used by realloc, strdup, et al. to indicate the what code really
 * requested the allocation of memory.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  18:47:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:18:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/25  08:37:05  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/08/12  07:09:36  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.6  1995/07/13  23:55:05  dkatz
 * CSCdi37079:  malloc variations need to save callers PC
 *
 * Revision 2.5  1995/06/28  05:56:38  dkatz
 * CSCdi36468:  MALLOCFAIL at boot up
 * Move malloc failure reporting so that the box doesn't whimper when
 * falling over to an alternate pool.
 *
 * Revision 2.4  1995/06/21  08:56:14  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/18  06:03:30  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.2  1995/06/11  21:06:08  dkatz
 * CSCdi35670:  Checkheaps timer initialized too late
 * Initialize timers in init routine, rather than in process.
 *
 * Revision 2.1  1995/06/07  21:54:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#include "sched_private.h"
#include "sum.h"
#include "mgd_timers.h"
#include "sys_registry.h"
#include "../os/chunk.h"
#include "../os/region.h"
#include "buffers_debug.h"
#include "free_private.h"

/*
 * Memory allocation system
 *
 * Memory is divided into a set of "pools" that correspond to blocks of
 * memory with discontiguous addresses (such as processor memory, I/O
 * memory, multibus memory, etc.)  Each pool is managed separately.
 *
 * All blocks of memory in a pool are threaded together in the order
 * in which they appear in memory.  This makes it easy to fragment
 * and coalesce blocks.
 *
 * Each pool has a set of free lists of different sizes.  Associated with
 * each free list is two ranges, the range of sizes of request to be
 * served from that list, and the range of sizes of memory blocks to be
 * saved on that list.  The two ranges overlap at a single value, which
 * is nominally the list "size".  This guarantees that a request directed
 * to a particular free list can be satisfied by the first block on the
 * list.  (For instance, if we have three list sizes, 64, 128, and 192,
 * the second list satisfies requests of sizes 65-128 and holds blocks
 * of sizes 128-191.)
 *
 * This has the effect that if a block of a particular size is created
 * and freed, it will not be used to satisfy a request of the same size
 * unless it is exactly the list "size" (128 in the above example).  Thus
 * it is a good idea to have lists corresponding to the sizes of commonly
 * used blocks.
 *
 * Each free list is threaded;  the pointers are kept in the data portion
 * of the memory block to reduce overhead (since the pointers are only
 * used when the block is not in active use.)
 *
 * The free lists in a pool are organized into a collapsed binary tree
 * according to size, in order to quickly find a list of the proper size.
 *
 * One free list is sort of special.  The last free list is
 * used to satisfy all requests greater than its size (and thus cannot
 * guarantee that the first block on the list will satisfy a request).
 *
 * Free lists are created dynamically, and can be created at any point
 * while the system is running.  This allows various system modules
 * to register a free list for data sizes with high turnover.  When a
 * new free list is created, the binary tree is rebalanced, and the
 * affected free lists are rebuilt properly.
 *
 * When a block of memory is requested, the tree of lists is walked until
 * a list of the proper size is found, and the first block on that list
 * that is big enough to satisfy the request is used (this is always the
 * first entry, except for the last list as mentioned above).
 * If no block on the list satisfies the request (usually meaning that the
 * list is empty), each successive list is examined.  If no list can
 * satisfy the request, we go back to the list prior to the "proper" one,
 * since it may have blocks of sufficient size (but we will have to do a
 * brute-force search to find it).
 *
 * The block is trimmed to the requested length, so long as the resulting
 * block is bigger than the minimum size.  The new fragment is requeued onto
 * the appropriate free list (it probably doesn't belong on the list on
 * which it was found).
 *
 * When a block is freed, its immediate neighbor blocks are examined.  If
 * either one is free, the newly freed block is coalesced with the neighbor
 * blocks, and the resulting block requeued onto the proper free list.
 *
 * In order to reduce fragmentation, fragments and coalesced blocks
 * are added to the end of the appropriate free list;  blocks which
 * are simply freed are added to the beginning.  Blocks are always
 * allocated from the beginning.  This tends to help blocks coalesce
 * when there is a lot of activity, since if the fragment from a block
 * is saved as long as possible, it is more likely to be free when
 * the first half of the block is freed (allowing coalescence).
 */


/*
 * The standard complaint when we run out of memory.  Also another
 * shared string.
 */
char *nomemory = "\n%%No memory available";
const char malloc_msg[] = "malloc";

/*
 * Local Storage
 */

mempool *mempools[MEMPOOL_CLASS_MAX]; /* Array of memory pools */
ulong heapcheck_ivl = HEAPCHECK_IVL_DEFAULT; /* Heap check interval */
ulong cksumcheck_ivl = CKSUMCHECK_IVL_DEFAULT; /* Heap check interval */
mgd_timer heapcheck_timer;
mgd_timer cksumcheck_timer;
mgd_timer check_timer;
pid_t checkheaps_pid;
typedef enum {HEAP, CKSUM} checkheaps_timer_type;
int retting_buffer = FALSE;		/* TRUE if returning a buffer */

/*
 * The following is used to record malloc failures for later display.
 */
malloc_failures malloc_failure_trace[MALLOC_FAILURE_RECORD_COUNT];
int malloc_failure_trace_number;	/* Next element to fill in */

/*
 * Memory Pools
 *
 * Here lie the memory pools and free list trees.
 */ 
static ulong freelist_default_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    FREESIZE(5), FREESIZE(6), FREESIZE(7),
    1500, 2000, 3000, 5000, 10000, 20000, 32768, 65536, 131072,
    LARGEST_DEFAULT_FREELIST_SIZE
};
static int nfreelist_default_block_sizes =
    sizeof(freelist_default_block_sizes)/sizeof(ulong);

struct memtrace freetrace[TRACE_SIZE], malloctrace[TRACE_SIZE];
uchar ixfree, ixmalloc;

#define MAX_FREE_TREE_DEPTH 10	/* This is pretty deep... */


/*
 * report_malloc_failure
 *
 * Report the fact that a malloc failed.  Record it for later display.
 */
void report_malloc_failure (mempool *pool, ulong size, ulong caller_pc,
			    ulong alignment)
{
    malloc_failures *failptr;

    /* Burp out a rate-limited error message. */

    (*kernel_errmsg)(&msgsym(MALLOCFAIL, SYS), size, caller_pc,
		     pool->name, alignment);

    /* Record it for posterity. */

    failptr = &malloc_failure_trace[malloc_failure_trace_number];
    GET_TIMESTAMP(failptr->failtime);
    failptr->pool = pool;
    failptr->size = size;
    failptr->alignment = alignment;
    failptr->calling_pc = caller_pc;

    /* Bump the index and reset if necessary. */
    
    malloc_failure_trace_number++;
    if (malloc_failure_trace_number >= MALLOC_FAILURE_RECORD_COUNT)
	malloc_failure_trace_number = 0;
}

void errmsg_mtrace (void)
{
#define MT(x) malloctrace[(ixmalloc + TRACE_SIZE - (x))%TRACE_SIZE]
#define FT(x)   freetrace[(ixfree   + TRACE_SIZE - (x))%TRACE_SIZE]
#define mt(x) MT(x).addr, MT(x).pc
#define ft(x) FT(x).addr, FT(x).pc
    (*kernel_errmsg)(&msgsym(MTRACE, SYS), "malloc",
	   mt(1), mt(2), mt(3), mt(4), mt(5), mt(6), mt(7), mt(8));
    (*kernel_errmsg)(&msgsym(MTRACE, SYS), "free",
	   ft(1), ft(2), ft(3), ft(4), ft(5), ft(6), ft(7), ft(8));
    return;
#undef ft
#undef mt
#undef FT
#undef MT
}

void errmsg_blkinfo (
     char* cp,
     blocktype* bp)
{
    ulong *mptr;

    (*kernel_errmsg)(&msgsym(BLKINFO, SYS), cp, (ulong) bp,
	   bp->size & FREEDMASK, bp->allocator_pc,
	   (bp->size & MEMINUSE) ? "InUse" : "Free",
	   freeptr(bp)->last_deallocator, bp->refcount);
    mptr = (ulong *) bp;
    (*kernel_errmsg)(&msgsym(MEMDUMP, SYS), mptr, *mptr, *(mptr + 1),
		     *(mptr + 2), *(mptr + 3));
    mptr += 4;
    (*kernel_errmsg)(&msgsym(MEMDUMP, SYS), mptr, *mptr, *(mptr + 1),
		     *(mptr + 2), *(mptr + 3));
    return;
}

/*
 * mempool_find_by_class
 *
 * Return a mempool pointer given a memory class
 */

mempool *mempool_find_by_class (mempool_class class)
{
    return(mempool_find_by_class_inline(class));
}


/*
 * mempool_find_by_addr
 * Return a pointer to the pool that the memory came from.  Returns NULL on
 * error. 
 */

mempool *mempool_find_by_addr (void *location)
{
    regiontype    *region;

    /*
     * Call region_find_by_addr.
     * It will find the region that constrains the address tightest.
     * The mempool pointer stored there is then returned to the caller.
     * This allows mempools within mempools to work properly.
     */
    region = region_find_by_addr(location);
    if (region)
        return(region->mempool);
    else
        return(NULL);
}

/*
 * Validate freeblock.  Crash if invalid.
 */
static boolean valid_freeblock (blocktype *curblock, 
				mempool *mem,
				ulong freelist_size)
{
    return(valid_freeblock_inline(curblock, mem, freelist_size, TRUE));
}

/*
 * delete_free_block
 *
 * Remove a block from its free list
 */
static inline void delete_free_block (blocktype *block)
{
    freeblocktype *freeblock = freeptr(block);
    *(freeblock->free_prev) = freeblock->free_next;
    freeblock->free_next->free_prev = freeblock->free_prev;
}

/*
 * add_free_block
 *
 * Add a block to its free list
 *
 * It optimizes if called with a constant.
 */
typedef enum {FREE_HEAD, FREE_TAIL} freetype;

static inline void add_free_block (blocktype *block, free_tree *tree,
				   freetype which_end)
{
    freeblocktype *freeblock = freeptr(block);
    if (which_end == FREE_HEAD) {	/* Put on the head of the queue */
	freeblock->free_next = tree->head;
	freeblock->free_prev = &tree->head;
	if (tree->head->free_next) {		/* List isn't empty */
	    tree->head->free_prev = &(freeblock->free_next);
	} else {			/* List is empty */
	    freeblock->free_next = &tree->tail;
	    tree->tail.free_prev = &(freeblock->free_next);
	}
	tree->head = freeblock;
    } else {				/* Put on the tail of the queue */
	freeblock->free_prev = tree->tail.free_prev;
	freeblock->free_next = &(tree->tail);
	*tree->tail.free_prev = freeblock;
	tree->tail.free_prev = &(freeblock->free_next);
    }
    freeblock->freemagic = FREEMAGIC;
}

/*
 * init_block
 * Initialize common variables in a new memory block.
 * Link the new block in with the pointers supplied.
 *
 * If "initial_block" is true, there is no previous block, and "head"
 * points to the head pointer.  If "initial_block" is false, there is
 * always a previous block, and "head" is not used.
 *
 * All of this will be optimized down to something reasonable.
 */
static inline
void init_block (blocktype *ptr,
		 blocktype *prev,
		 int size,
		 boolean initial_block,
		 blocktype **head,
		 ulong caller_pc)
{
    ptr->size = size;
    ptr->refcount = 0;
    ptr->allocator_pid = 0;
    ptr->allocator_check = 0;
    ptr->allocator_name = "(fragment)";
    ptr->allocator_pc = caller_pc;
    ptr->blockmagic = BLOCKMAGIC;
    if (initial_block) {
	ptr->next = NULL;
	ptr->previous = head;
	*head = ptr;
    } else {
	ptr->next = prev->next;
	if (ptr->next)
	    (ptr->next)->previous = &ptr->next;
	ptr->previous = &prev->next;
	prev->next = ptr;
    }
}

/*
 * init_new_block
 * Initialize common variables in a new memory block.
 * Link the new block in with the pointers supplied
 */
static inline
void init_new_block (blocktype *ptr,
		     blocktype *prev,
		     int        size,
		     ulong	caller_pc)
{
    init_block(ptr, prev, size, FALSE, NULL, caller_pc);
}

/*
 * right_freelist_for_block - returns TRUE if the freelist indicated is the
 * proper one for the block size indicated
 */
static inline boolean right_freelist_for_block (free_tree *tree, ulong size)
{
    return (size >= tree->minsize && size <= tree->maxsize);
}

/*
 * freelist_find_by_size
 *
 * Returns a pointer to the proper free list for a block of a given size
 */
static inline free_tree *freelist_find_by_size_inline (free_tree *tree,
						       ulong size)
{
    ulong depth = MAX_FREE_TREE_DEPTH; /* A limiter */
    for (;;) {
	if (--depth == 0)
	    crashdump(0);	/* I think something's broken */
	if (size < tree->minsize) { /* go left */
	    if (tree->left == NULL)
		break;
	    tree = tree->left;
	} else if (size > tree->maxsize) { /* go right */
	    if (tree->right == NULL)
		break;
	    tree = tree->right;
	} else {		/* Got it */
	    break;
	}
    }
    return (tree);
}

free_tree *freelist_find_by_size (free_tree *tree, ulong size)
{
    return(freelist_find_by_size_inline(tree, size));
}


/*
 * freelist_for_block - find the right free list for a block to
 * be freed
 */
static inline free_tree *freelist_find_by_block (mempool *pool,
						 blocktype *block)
{
    return (freelist_find_by_size_inline(pool->freeroot, block->size));
}

/*
 * freelist_for_malloc - find the right free list for a block to
 * be malloced.
 */
static inline free_tree *freelist_for_malloc (free_tree *tree, ulong size)
{
    ulong depth = MAX_FREE_TREE_DEPTH; /* A limiter */
    for (;;) {
	if (--depth == 0)
	    crashdump(0);	/* I think something's broken */
	if (size < tree->minalloc) { /* go left */
	    if (tree->left == NULL)
		break;
	    tree = tree->left;
	} else if (size > tree->maxalloc) { /* go right */
	    if (tree->right == NULL)
		break;
	    tree = tree->right;
	} else {		/* Got it */
	    break;
	}
    }
    return (tree);
}

/*
 * build_subtree
 *
 * Called recursively to build the binary tree for a free list.
 */
static free_tree *build_subtree (free_tree *treearray,
				       ulong base, ulong count, int depth)
{
    ulong mid = base + (count / 2);
    free_tree *ptr = treearray + mid;
    if (depth == 0)
	crashdump(0);		/* Too much recursion! */
    depth++;
    ptr->left = NULL;
    ptr->right = NULL;

    if (count >= 2)
	ptr->left = build_subtree(treearray, base, count / 2, depth);
    if (count >= 3)
	ptr->right = build_subtree(treearray, mid + 1, 
			     count - (count / 2) - 1, depth);
    return (ptr);
}

/*
 * mempool_add_free_list_internal - add a new free-list size to a memory pool
 *
 * This routine can be called at any time to register a new free list
 * size.
 *
 * We return a pointer to the new free list, or NULL if it failed.
 * A request to add a list that matches an existing size will silently
 * return a pointer to the existing list.
 *
 * "size" is the size of the desired data area, in bytes (the same value
 * used when calling malloc()).
 */
static free_tree *mempool_add_free_list_internal (mempool_class class,
						  ulong size)
{
    int        i, newslot;
    ulong      wordsize = malloc_word_size(size);
    free_tree *treeptr, *newtreeptr;
    int        tripcount;
    mempool   *mempool;

    /*
     * Get the mempool we need
     */
    mempool = mempool_find_by_class(class);
    if (!mempool)
	return(NULL);

    /*
     * Too many lists?
     */
    if (mempool->freelist_count >= MAX_FREE_POOLS)
	return(NULL);

    /*
     * Scan down the existing lists until we find where we need to go,
     * or we find a duplicate.
     */
    for (newslot=0; newslot < mempool->freelist_count; newslot++) {
	if (wordsize == mempool->tree_array[newslot].blocksize)
	    return(&(mempool->tree_array[newslot])); /* Duplicate. */
	if (wordsize < mempool->tree_array[newslot].blocksize)
	    break;
    }

    /* Next, shuffle all of the subsequent trees down one slot. */

    treeptr = mempool->last_free_pool;
    for (i = mempool->freelist_count - 1; i >= newslot; i--) {
	free_tree *nexttree = treeptr + 1;
	*nexttree = *treeptr;	/* Copy the tree entry */

	/*
	 * If the tree isn't empty, adjust the first and last entries
	 * to point at the new head and tail pointers.  If it's empty,
	 * make them point at each other.
	 */
	if (treeptr->head->free_next) {	/* List isn't empty */
	    nexttree->head->free_prev = &(nexttree->head);
	    *(nexttree->tail.free_prev) = &(nexttree->tail);
	} else {			/* Tail points at head */
	    nexttree->head = &nexttree->tail;
	    nexttree->tail.free_prev = &nexttree->head;
	}
	treeptr--;
    }

    /* Bump the count of free lists.  Note that the last list moved. */

    mempool->freelist_count++;
    mempool->last_free_pool++;

    /*
     * Fill in the fields of the new free list.  We have to adjust our
     * neighbors' sizes if they are there.
     */

    newtreeptr = &mempool->tree_array[newslot];
    newtreeptr->head = &newtreeptr->tail;
    newtreeptr->tail.free_prev = &newtreeptr->head;
    newtreeptr->blocksize = wordsize;
    newtreeptr->minsize = wordsize;
    newtreeptr->maxalloc = wordsize;
    if (newslot > 0) {
	free_tree *prevtreeptr = newtreeptr - 1;
	/*
	 * The block we're adding is not the first on the list.  Set
	 * our minalloc to the previous guy's blocksize + 1.  This is
	 * usually equal to the previous guy's maxalloc + 1 (which makes
	 * more intuitive sense), but if we're the last on the list, his
	 * maxalloc will be MAXBLOCKSIZE, which would be a mess.  We also
	 * rewrite his maxalloc to correct this eventuality.
	 */
	newtreeptr->minalloc = prevtreeptr->blocksize + 1;
	prevtreeptr->maxsize = wordsize - 1;
	prevtreeptr->maxalloc = prevtreeptr->blocksize;
    } else {
	/*
	 * We're the first on the list.  Set our minalloc and minsize 
	 * to zero to pick up all small stragglers.
	 */
	newtreeptr->minalloc = 0;
	newtreeptr->minsize = 0;
    }

    if (newslot >= mempool->freelist_count - 1) {
	/*
	 * We're last on the list.  Set our maxalloc and maxsize to
	 * MAXBLOCKSIZE, since we're going to take all big requests
	 * and store all big blocks.
	 */
	newtreeptr->maxalloc = MAXBLOCKSIZE;
	newtreeptr->maxsize = MAXBLOCKSIZE;
    } else {
	free_tree *nexttreeptr = newtreeptr + 1;
	/*
	 * Not the last on the list.  Set our maxsize to one less than
	 * the next guy's blocksize (which makes sense).  Set his
	 * minalloc to our maxalloc + 1 (effectively).  Set his
	 * minsize to his blocksize, in case he was first before (in
	 * which case his minsize was forced to zero).
	 */
	newtreeptr->maxsize = nexttreeptr->blocksize - 1;
	nexttreeptr->minalloc = wordsize + 1;
	nexttreeptr->minsize = nexttreeptr->blocksize;
    }

    /* Rebuild the tree. */

    mempool->freeroot = build_subtree(mempool->tree_array, 0, 
				   mempool->freelist_count, 
				   MAX_FREE_TREE_DEPTH);
    /*
     * Now relink the previous free list, since there may now be blocks
     * on it that should now be on our list.
     */

    if (newslot > 0) {		/* Not the first list */
	freeblocktype *curfree, *nextfree;
	free_tree *prevtreeptr = newtreeptr - 1;
	curfree = prevtreeptr->head; /* Always non-NULL */
	nextfree = curfree->free_next;
	tripcount = WAY_TOO_MANY_TRIPS;
	while (nextfree) {
	    blocktype *curblock = freeblock_to_block(curfree);
	    if (--tripcount < 0)
		crashdump(0);	/* Don't wedge forever */
	    if (!right_freelist_for_block(prevtreeptr, curblock->size)) {

		/* Doesn't belong to neighbor; must belong to us. */

		delete_free_block(curblock);
		add_free_block(curblock, newtreeptr, FREE_TAIL);
	    }
	    curfree = nextfree;
	    nextfree = curfree->free_next;
	}
    }
    return(newtreeptr);
}


/*
 * mempool_add_free_list - add a new free-list size to a memory pool
 *
 * This routine can be called at any time to register a new free list
 * size.
 *
 * We return TRUE if we succeeded, or FALSE if there is no more space.
 * A request to add a list that matches an existing size will silently
 * return TRUE.
 *
 * "size" is the size of the desired data area, in bytes (the same value
 * used when calling malloc()).
 */
boolean mempool_add_free_list (mempool_class class, ulong size)
{
    return (mempool_add_free_list_internal(class, size) != NULL);
}

/*
 * mempool_add_free_list_manual
 *
 * This routine is called from parser code to set a manual freelist entry
 */
boolean mempool_add_free_list_manual (mempool_class class, ulong size)
{
    free_tree *listptr;

    listptr = mempool_add_free_list_internal(class, size);
    if (listptr) {
	listptr->configured = TRUE;
	return(TRUE);
    } else {
	return(FALSE);
    }
}


/*
 * free_tree_init
 *
 * Initialize the free-list tree for a memory pool
 */
static void free_tree_init (mempool     *pool,
			    const ulong *blocksizes,
			    ulong        blockcount)
{
    int i;

    /* Die if bogus. */

    if (blockcount == 0)
	crashdump(0);			/* Must be at least one block */

    /* Initialize a null pool. */

    pool->last_free_pool = &(pool->tree_array[-1]); /* Scary... */
    pool->freelist_count = 0;

    /* Now add each free list to the tree. */

    for (i=0; i<blockcount; i++) {
	if (!mempool_add_free_list(pool->class, blocksizes[i]))
	    crashdump(0);
    }
}


/*
 * malloc/aligned malloc
 * Allocate memory initialized to zero.  Argument is in bytes.
 * Returns character pointer or NULL.
 * Must NOT be called from interrupt level.
 */

static inline void *malloc_aligned_inline (mempool_class class,
					   ulong caller_pc,
					   uint size, uint align)
{
    mempool        *mempool;
    void           *ptr;
    ulong           nbytes;

    mempool = mempool_find_by_class_inline(class);
    if (!mempool)
	return(NULL);

    /*
     * If we don't have an explicit alignment, use the default
     */
    if (!align)
	align = mempool->alignment;

    /*
     * If we have a default alignment in the pool, adjust the size
     * of the requested block so that we don't get weird fragments
     */
    if (mempool->alignment)
	size += mempool->alignment -
	        (size + MEMORY_BLOCK_OVERHEAD) % mempool->alignment;

    ptr = malloc_block(mempool, size, caller_pc, &nbytes, align);
    if (!ptr && mempool->alternate)
	ptr = malloc_block(mempool->alternate, size, caller_pc, &nbytes,
			   align);
    
    if (ptr != NULL) {
	process_count_malloc(nbytes);
    } else {

	/* Whimper. */

	report_malloc_failure(mempool, size, caller_pc, align);
    }
    return (ptr);			/* return pointer */
}


/*
 * malloc_aligned wrappers.
 *
 * The following wrappers provide access to the malloc code. Don't add
 * any more - consider the bloat effects.
 */

void *mempool_malloc (mempool_class class, uint size)
{
    SAVE_CALLER();
    return(malloc_aligned_inline(class, caller(), size, 0));
}    

void *mempool_malloc_aligned (mempool_class class, uint size, uint align)
{
    SAVE_CALLER();
    return(malloc_aligned_inline(class, caller(), size, align));
}    

void *malloc (size_t size)
{
    SAVE_CALLER();
    return(malloc_aligned_inline(MEMPOOL_CLASS_LOCAL, caller(), size, 0));
}

void *malloc_aligned (uint size, uint align)
{
    SAVE_CALLER();
    return(malloc_aligned_inline(MEMPOOL_CLASS_LOCAL, caller(), size, align));
}


/*
 * malloc_named
 * Like malloc, but allow the caller to specify the name shown in "show mem"
 * (So far, this is at the expense of remembering the process name.)
 */
void *malloc_named (uint size, const char *name)
{
    blocktype *block;
    void *ptr;

    SAVE_CALLER();

    ptr = malloc(size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}

/*
 * malloc_named_aligned
 * Like malloc_named, but allow the caller to specify alignment.
 */
void *malloc_named_aligned (uint size, const char *name, uint align)
{
    blocktype *block;
    void *ptr;

    SAVE_CALLER();

    ptr = malloc_aligned(size, align);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * malloc_iomem
 *
 * Allocate memory from the designated IO pool
 */

void *malloc_iomem (uint size)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_IOMEM, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_pc = caller();
    }
    return(ptr);
}

/*
 * malloc_named_iomem
 *
 * Allocate memory from the designated IO pool
 */

void *malloc_named_iomem (uint size, const char *name)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_IOMEM, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * malloc_iomem_aligned
 *
 * Allocate memory from the designated IO pool with an alignment
 */

void *malloc_iomem_aligned (uint size, uint align)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc_aligned(MEMPOOL_CLASS_IOMEM, size, align);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_pc = caller();
    }
    return(ptr);
}

void *malloc_named_iomem_aligned (uint size, uint align, const char *name)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc_aligned(MEMPOOL_CLASS_IOMEM, size, align);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * malloc_pcimem
 *
 * Allocate memory from the designated PCI pool
 */

void *malloc_pcimem (uint size)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_PCIMEM, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * malloc_named_pcimem
 *
 * Allocate memory from the designated PCI pool 
 */

void *malloc_named_pcimem (uint size, const char *name)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_PCIMEM, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * malloc_fast
 *
 * Allocate memory from the designated fast memory pool
 */

void *malloc_fast (uint size)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_FAST, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_pc = caller();
    }
    return(ptr);
}

/*
 * malloc_named_fast
 *
 * Allocate memory from the designated fast memory pool
 */

void *malloc_named_fast (uint size, const char *name)
{
    void *ptr;
    blocktype *block;

    SAVE_CALLER();
    ptr = mempool_malloc(MEMPOOL_CLASS_FAST, size);
    if (ptr) {
	block = data_to_block(ptr);
	block->allocator_name = name;
	block->allocator_pc = caller();
    }
    return(ptr);
}


/*
 * check_red
 *
 * Validate the red zone word for a block.  Returns TRUE if OK, FALSE
 * if corrupted.  Crashes on error if "crash" is TRUE.
 */
static inline boolean check_red (mempool *pool, blocktype* bp, boolean crash)
{
    if ((*rz_ptr(bp)) != REDZONE) {
	if (crash) {
	    CRASH_AND_BURN(pool, bp, 
			   (&msgsym(OVERRUN, SYS), bp, *rz_ptr(bp)),
			   BADBLOCK_REDZONE);
	}
	return(FALSE);
    }
    return(TRUE);
}

static inline boolean check_red_for_scheduler (
    mempool *pool,
    blocktype* bp,
    char *process,
    void *sched_test)
{
    if ((*rz_ptr(bp)) != REDZONE) {
	CRASH_AND_BURN_FOR_SCHEDULER(pool, bp, 
		       (&msgsym(OVERRUN, SYS), bp, *rz_ptr(bp)),
		       BADBLOCK_REDZONE, process, sched_test);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * determine if a block passes all trash tests before returning to
 * free pool.
 *
 * NOTE: the code sequences leading up to crashdump()'s in this routine
 * are explicitly coded differently, and the parameter to crashdump() is
 * different in each occurance so that the compiler will not optimize this
 * code to reuse text space.  We want the code paths to be different so
 * that we can tell how things failed by looking at the PC and/or stack frame
 * which would be impossible otherwise.
 *
 * The tests for "crash" will be optimized out according to the constant
 * parameter passed.
 *
 * The tests for "interrupt" will similarly be optimized out.  This
 * parameter should be TRUE for an interrupt-safe version.
 */
static inline boolean validblock_inline (blocktype *bp, mempool *mp,
					 boolean crash, boolean interrupt)
{
    blocktype  *prev;
    regiontype *region;

    /*
     * Null pointers are valid, what the heck.
     */
    if (bp == NULL)
	return TRUE;

    /*
     * Things are sad if memory is already corrupted.
     */
    if (mp->corrupt)
	return FALSE;

    /*
     * Make sure the block is in the memory pool and is longword aligned
     */
    region = mempool_find_region_by_addr(mp, bp);
    if (((ulong) bp & 0x0003) || !region) {
	if (crash) {
	    CRASH_AND_BURN(mp, bp, 
			   (&msgsym(BADMAGIC, SYS), 
			    (ulong) bp, bp->blockmagic),
			   BADBLOCK_NOTALIGNED);
	}
	return FALSE;
    }

    /*
     * For active blocks, the red zone area must be valid.
     */
    if (bp->size & MEMINUSE) {
 	if (!check_red(mp, bp, crash)) {
 	    return FALSE;
 	}
    }
 

    /*
     * If it's the last block, make sure that it ends well.
     */
    if (bp->next == NULL) {
	if (block_end(bp) != (blocktype *)region->end) {
	    if (crash)
		CRASH_AND_BURN(mp, bp, (&msgsym(BADBLOCK, SYS), (ulong) bp),
			        BADBLOCK_MEMTOP);
	    return FALSE;
	}
    }

    /* The block must have a valid blockmagic. */

    if (bp->blockmagic != BLOCKMAGIC) {
	if (crash) {
	    CRASH_AND_BURN(mp, bp, 
			   (&msgsym(BADMAGIC, SYS), 
			    (ulong) bp, bp->blockmagic),
			   BADBLOCK_MAGIC);
	}
	return FALSE;
    }

    if (!interrupt) {
	blocktype *next;

	/*
	 * If there is a link to previous, ensure that its link to next
	 * points back to us, and ensure that the address corresponds to
	 * the size.
	 */
	prev = prev_block(bp, mp);

	/*
	 * Check the previous block is okay
	 *
	 * Prev pointer must be nonzero
	 * Prev pointer must be in pool, or must point to head pointer
	 * Contents of prev pointer must point at the block
	 * Prev block must end at our block or the top of the region
	 */
	region = mempool_find_region_by_addr(mp, bp->previous);
	next = (prev) ? block_end(prev) : NULL;
	if (!bp->previous ||
	    (!region && (bp->previous != &mp->memhead)) ||
	    (*bp->previous != bp) ||
	    (prev && ((next != bp) &&
		      (next != (blocktype *)region->end)))) {
	    if (crash) {
		CRASH_AND_BURN(mp, bp, (&msgsym(BADBLOCK, SYS), (ulong) bp),
			       BADBLOCK_PREVIOUS);
	    }
	    return FALSE;
	}

	/*
	 * If there is a link to next, ensure that its link to previous
	 * points back to us, and ensure that the address corresponds to
	 * the size.  It's probably safe to test this at interrupt time,
	 * but I haven't the guts.
	 */
	
	/*
	 * Check the next block pointer is okay
	 *
	 * Next pointer must point within mempool
	 * Next block's prev pointer must point at our next pointer
	 * Our block must end just before the next block
	 */
	region = mempool_find_region_by_addr(mp, bp->next);
	next = block_end(bp);
	if (bp->next &&
	    ((!region) ||
	     (bp->next->previous != &bp->next) ||
	     ((bp->next != next) &&
	      (bp->next != (blocktype *)region->start)))) {
	    if (crash) {
		CRASH_AND_BURN(mp, bp, (&msgsym(BADBLOCK, SYS), (ulong) bp),
			       BADBLOCK_NEXT);
	    }
	    return FALSE;
	}
    }

    return TRUE;
}

/*
 * Externally callable routine to crash and burn as necessary.
 */
boolean validblock (blocktype *bp, mempool *mp)
{
    return(validblock_inline(bp, mp, TRUE, FALSE));
}

/*
 * Validate memory without crashing...
 */
boolean validblock_gentle (blocktype *bp, mempool *mp)
{
    return(validblock_inline(bp, mp, FALSE, FALSE));
}

/*
 * block_suspend
 *
 * Used by display functions to rest on a memory pool block and
 * suspend without opening up a window for memory corruption due
 * to memory pool coagulation
 */

blocktype *block_suspend (mempool       *mempool,
			  blocktype     *block,
			  sys_timestamp *suspend_timer)
{
    blocktype *prev;
		     
    if (!TIMER_RUNNING(*suspend_timer)) {
	TIMER_START(*suspend_timer, CHECKHEAPS_TIME);
	return(block);
    }
    
    /*
     * If the suspend timer is awake and we can rest on an
     * allocated block, do so
     */
    if (AWAKE(*suspend_timer) &&
	(block->size & MEMINUSE) && (block->refcount >= 1)) {
	inline_mem_lock(block_to_data(block));

	/*
	 * Pass the ball to someone else
	 */
	process_suspend();

	/*
	 * We're back. Is this block still kosher?
	 */
	if (!validblock_gentle(block, mempool))
	    return(NULL);
	prev = prev_block(block, mempool);
	inline_mem_unlock_or_free(block_to_data(block));	
	
	/*
	 * Check if current block has been coalesced. If so then continue 
	 * from the previous block.
	 */
	if (prev && prev->next != block)
	    block = prev;

	/*
	 * Start our timer for the next time
	 */
	TIMER_START(*suspend_timer, CHECKHEAPS_TIME);
    }
    return(block);
}

/*
 * get_aligned_block -- get a block of memory from a free pool.
 * make sure it is aligned to 1st arg.
 * Return zero if none is available
 */
blocktype *get_aligned_block (
    register int alignment,
    register int nwords,
    mempool *mem,
    ulong caller_pc)
{
    blocktype *bestchoice = NULL, *curblock = NULL, *newp;
    ulong preleft = 0, postleft = 0, disp;
    free_tree *curfreelist, *proper_freelist;
    int tripcount;
    boolean last_ditch_search;

    if (mem->corrupt) {
	mem = mem->exception_pool;
	if (!mem || mem->corrupt)
	    return(NULL);
    }

    if ((nwords <= 0) || (nwords > mem->size) ||
	(alignment <= 0) || (alignment & 1)) {
#ifdef FREEDEBUG
	(*kernel_buginf)("GET_ALIGNED_BLOCK: nwords=%d, mem->size=%d alignment=%d\n",
	       nwords, mem->size, alignment);
#endif FREEDEBUG
	return (NULL);
    }

    /*
     * Start from the appropriately sized freelist.  If there's anything on it,
     * and the block is big enough (this will be true for all lists except the
     * one with the biggest blocks), take it.  If not big enough, walk the list
     * searching.  If the list runs out (or was empty), move on to the next
     * list.  If we run out of lists, go back to the list previous to the
     * appropriately sized freelist, since it may contain blocks that are
     * big enough to satisfy the request.  If that list doesn't pan out,
     * we lose.
     */
    proper_freelist = freelist_for_malloc(mem->freeroot, nwords);
    curfreelist = proper_freelist;
    tripcount = WAY_TOO_MANY_TRIPS;
    last_ditch_search = FALSE;
    for (;;) {
	freeblocktype *curfree;
	if (--tripcount < 0)
	    crashdump(0);	/* Too many trips */
	for (curfree = curfreelist->head;  curfree->free_next;
	     curfree = curfree->free_next) {
	    curblock = freeblock_to_block(curfree);

	    if (--tripcount < 0)
		crashdump(0);	/* Too many trips */
	    /*
	     * Make sure free list is not clobbered:
	     */
	    if (!validblock(curblock, mem))
		return(NULL);
	    if (!valid_freeblock(curblock, mem, curfreelist->blocksize))
		return (NULL);
	    /* 
	     * If we can fit into this block, go for it (FIRST FIT):
	     * second conditional sees if can be aligned (truncated)
	     * and still support the size needed.
	     */
	    if (curblock->size >= nwords) {
		ulong block_data_ptr;
		ulong align;
		
		preleft = 0;
		bestchoice = curblock;
		block_data_ptr = (ulong) (curblock + 1);
		align = block_data_ptr % alignment;
		
		if (align) {			/* need to align? */
		    block_data_ptr += alignment - align;  /* Align it. */
		    
		    /*
		     * Work out how much space we've got at the
		     * start of the free block
		     */
		    bestchoice = ((blocktype *) block_data_ptr) - 1;
		    preleft = (short *)bestchoice - (short *)curblock;
		    
		    while (preleft < MINBLOCKWORDS) {
			/*
			 * Progressively shuffle the block up in memory
			 * until preleft is greater than MINBLOCKWORDS.
			 */
			preleft += btow(alignment);
			bestchoice = (blocktype *)((ulong)bestchoice +
						   (ulong)alignment);
		    }
		    
		    /*
		     * If the size of the block of memory isn't big
		     * enough to hold the 2 smaller blocks, then
		     * punt and try the next block
		     */
		    if (curblock->size < (nwords + preleft))
			continue;
		}
		
		/*
		 * Find the size of region after the aligned block
		 */
		postleft = curblock->size - preleft - nwords;
		goto got_block;
	    }
	}

	/*
	 * Nothing big enough on this list.  If we were doing our last-ditch
	 * search, bail out.
	 */
	if (last_ditch_search) {
	    curblock = NULL;		/* No stragglers, please. */
	    break;
	}

	/*
	 * If we just tried the last free list and didn't find anything,
	 * go back and do the last ditch search of the prior free list.
	 */
	if (IS_LAST_FREE_LIST(curfreelist)) {
	    if (proper_freelist == mem->tree_array) { /* No prior list! */
		curblock = NULL;	/* No stragglers, please. */
		break;
	    }
	    curfreelist = proper_freelist - 1; /* Check previous list. */
	    last_ditch_search = TRUE;
	} else {
	    curfreelist++;
	}
    }

got_block:    
    /*
     * If we fell out of the loop without finding something, then there was
     * nothing available.
     */
    if (!curblock) {
#ifdef FREEDEBUG
	(*kernel_buginf)("GET_ALIGNED_BLOCK: curblock is 0!\n");
#endif
	return (NULL);
    }

    /*
     * This code can be tricky to fully grok all of the offsets
     * and pointers used (tell me about it ;-). The following pearls
     * of wisdom might prove to be useful
     *
     * preleft  - The size of the block of memory left before the
     *            aligned block of memory. The size includes the
     *            BLOCKOVERHEADWORDS for the free block to be created.
     *
     * postleft - The size of the block of memory left after the
     *            aligned block of memory. The size includes the
     *            BLOCKOVERHEADWORDS for the free block to be created.
     *
     * curblock - Points to the block we found in the free list.
     *            If we are adjusting for alignment, this is effectively
     *            the header of the new free block we'll create before the
     *            one we're using.
     *
     * bestchoice - Points to the aligned memory block we're going to use.
     *
     * newp     - Points to the free block that we've created after
     *            truncating the block we're going to use.
     */

    /* First remove the block we found from the free list. */

    delete_free_block(curblock);

    /*
     * If we needed to align the start of the datablock,
     * initialize the header for bestchoice (as it'll point to
     * garbage), insert it and reduce the size of curblock
     */
    
    if (curblock != bestchoice) {
	/*
	 * Subtract the bestchoice header space from the total
	 */
	mem->size -= BLOCKOVERHEADWORDS;

	/*
	 * Initialize bestchoice into the memory chain
	 */
	init_new_block(bestchoice, curblock, curblock->size - preleft,
		       caller_pc);

        /* 
         * Adjust the free block at beginning
         */
	curblock->size = preleft - BLOCKOVERHEADWORDS;
    }

    /*
     * Is there space to build a new free block behind our
     * new data block?
     */
    newp = NULL;
    if (postleft >= MINBLOCKWORDS) {
	/* 
	 * Construct new free block at end of bestchoice
	 */
	disp = BLOCKOVERHEADWORDS + nwords;	/* In 16 bit words */
	newp = (blocktype *) ((short *)bestchoice + disp);
	
	/*
	 * Subtract the newp header space from the total
	 */
	mem->size -= BLOCKOVERHEADWORDS;
	
	/*
	 * Initialize newp into the memory chain
	 */
	init_new_block(newp, bestchoice, postleft - BLOCKOVERHEADWORDS,
		       caller_pc);
	bestchoice->size -= postleft;
    }

    /*
     * Now put any blocks we've created back on the free list.  The
     * final block we've created may be nearly as big as the original
     * block, so we check first to see if we should put it back on the
     * list we got it from.
     */

    if (curblock != bestchoice) { /* Created a small first block */
	add_free_block(curblock, 
		       freelist_find_by_size(mem->freeroot,
					     curblock->size),
		       FREE_TAIL);
    }

    if (newp) {			/* Created a trailing block */
	if (!right_freelist_for_block(curfreelist, newp->size)) {
	    curfreelist = freelist_find_by_size(mem->freeroot, newp->size);
	}
	add_free_block(newp, curfreelist, FREE_TAIL);
    }

    /*
     * Subtract the bestchoice data size from the free total
     */
    mem->size -= nwords;
    if (mem->size < mem->low_water)
	mem->low_water = mem->size;
    bestchoice->size |= MEMINUSE; /* mark in use */
    process_get_name(THIS_PROCESS, &bestchoice->allocator_name); /* Set process name */

    /* if we are the init or the scheduler, allocator_pid is special. */

    if (!system_running) {
        bestchoice->allocator_pid = ALLOC_PID_INIT;
    } else if (schedflag) {
        bestchoice->allocator_pid = ALLOC_PID_SCHED;
    } else if (forkx && forkx->init_process) {
 	bestchoice->allocator_pid = ALLOC_PID_INIT;
    } else {
        process_get_pid(&bestchoice->allocator_pid);
	bestchoice->allocator_check = (ulong)forkx;
    }
    
    bestchoice->allocator_pc = caller_pc; /* And caller PC */
    bestchoice->refcount++;	/* Set an accessor */
    return (bestchoice);	/* return ptr */
}

/*
 * get_block -- get a block of memory from a free pool.
 * Return zero if none is available
 */

blocktype *get_block (
    register int nwords,
    mempool *mem,
    ulong caller_pc)
{
    register int leftover, disp;
    register blocktype *newp, *bestchoice = NULL;
    free_tree *curfreelist, *proper_freelist;
    int tripcount;
    boolean last_ditch_search;

    if (mem->corrupt) {
	mem = mem->exception_pool;
	if (!mem || mem->corrupt)
	    return(NULL);
    }

    if ((nwords <= 0) || (nwords > mem->size)) {
#ifdef FREEDEBUG
	(*kernel_buginf)("GET_BLOCK: nwords=%d, coreleft=%d\n",
			 nwords, mem->size);
#endif
	return (0);
    }

    /*
     * Start from the appropriately sized freelist.  If there's anything on it,
     * and the block is big enough (this will be true for all lists except the
     * one with the biggest blocks), take it.  If not big enough, walk the list
     * searching.  If the list runs out (or was empty), move on to the next
     * list.  If we run out of lists, go back to the list previous to the
     * appropriately sized freelist, since it may contain blocks that are
     * big enough to satisfy the request.  If that list doesn't pan out,
     * we lose.
     */
    proper_freelist = freelist_for_malloc(mem->freeroot, nwords);
    curfreelist = proper_freelist;
    tripcount = WAY_TOO_MANY_TRIPS;
    last_ditch_search = FALSE;
    for (;;) {
	freeblocktype *curfree;
	if (--tripcount < 0)
	    crashdump(0);	/* Too many trips */
	for (curfree = curfreelist->head; curfree->free_next;
	     curfree = curfree->free_next) {
	    bestchoice = freeblock_to_block(curfree); 
	    if (--tripcount < 0)
		crashdump(0);	/* Too many trips */
	    /*
	     * Make sure free list is not clobbered:
	     */
	    if (!validblock(bestchoice, mem))
		return NULL;
	    if (!valid_freeblock(bestchoice, mem, curfreelist->blocksize))
		return NULL;
	    /*
	     * If we can fit into this block, go for it (FIRST FIT) (should
	     * always work except on the last list).
	     */
	    if (bestchoice->size >= nwords) {
		goto got_block;
	    }
	}

	/*
	 * Nothing big enough on this list.  If we were doing our last-ditch
	 * search, bail out.
	 */
	if (last_ditch_search) {
	    bestchoice = NULL;		/* No stragglers, please. */
	    break;
	}

	/*
	 * If we just tried the last free list and didn't find anything,
	 * go back and do the last ditch search of the prior free list.
	 */
	if (IS_LAST_FREE_LIST(curfreelist)) {
	    if (proper_freelist == mem->tree_array) { /* No prior list! */
		bestchoice = NULL;	/* No stragglers, please. */
		break;
	    }
	    curfreelist = proper_freelist - 1; /* Check previous list. */
	    last_ditch_search = TRUE;
	} else {
	    curfreelist++;
	}
    }
got_block:

    /*
     * If we fell out of the loop without finding something, then there was
     * nothing available.
     */
    if (!bestchoice) {
#ifdef FREEDEBUG
	(*kernel_buginf)("GET_BLOCK: bestchoice is 0!\n");
#endif
	return (NULL);
    }

    /* 
     *	Construct new free block at end of bestchoice unless the resulting
     *  block would be too small in which case allocate the entire block.
     *  The remaining block will probably end up in a different free list 
     *  than the one we're currently using, but we check.
     */
    leftover = bestchoice->size - nwords;
    if (leftover < MINBLOCKWORDS) {

	/* We're taking the whole block.  Just delink it. */

	nwords = bestchoice->size; /* absorbing all of the block */
	delete_free_block(bestchoice);
    } else {
	disp = BLOCKOVERHEADWORDS + nwords;	/* In 16 bit words */
	newp = (blocktype *) ((short *)bestchoice + disp);
	bestchoice->size = nwords;	/* lessen */
	init_new_block(newp, bestchoice, leftover - BLOCKOVERHEADWORDS,
		       caller_pc);
	mem->size -= BLOCKOVERHEADWORDS;
	/*
	 * Now, take the old block off of its free list, and add the new
	 * block onto the head of its free list.
	 */
	delete_free_block(bestchoice);
	if (!right_freelist_for_block(curfreelist, newp->size)) {
	    curfreelist = freelist_find_by_size(mem->freeroot, newp->size);
	}
	add_free_block(newp, curfreelist, FREE_TAIL);
    }
    mem->size -= nwords;
    if (mem->size < mem->low_water)
	mem->low_water = mem->size;
    bestchoice->size |= MEMINUSE; /* mark in use */
    process_get_name(THIS_PROCESS, &bestchoice->allocator_name); /* record who we are */

    /* Set the allocator PID properly. */

    if (!system_running) {
        bestchoice->allocator_pid = ALLOC_PID_INIT;
    } else if (schedflag) {
        bestchoice->allocator_pid = ALLOC_PID_SCHED;
    } else if (forkx && forkx->init_process) {
	bestchoice->allocator_pid = ALLOC_PID_INIT;
    } else {
        process_get_pid(&bestchoice->allocator_pid);
	bestchoice->allocator_check = (ulong)forkx;
    }
    bestchoice->allocator_pc = caller_pc;
    bestchoice->refcount++;	/* Set an accessor */
    return (bestchoice);	/* return ptr */
}

/*
 * Check to see if the requested amount of memory is available.
 */
boolean memory_available (ulong size, mempool *mem)
{
    free_tree *curfreelist, *lastfreelist;
    ulong nwords = btow(size);
    blocktype *bestchoice;
    int tripcount;

    if (mem->corrupt)
	return FALSE;

    /*
     * If the size is zero, return TRUE
     */
    if (!size)
	return(TRUE);

    /*
     * Check the last free list first.
     */
    lastfreelist = mem->last_free_pool;
    if (lastfreelist->head) {
	if (freeblock_to_block(lastfreelist->head)->size >= nwords) {
	    return (TRUE);
	}
    }

    /*
     * Start from the appropriately sized freelist.  If there's anything on it,
     * and the block is big enough (this will be true for all lists except the
     * one with the biggest blocks), take it.  If not big enough, walk the list
     * searching.  If the list runs out (or was empty), move on to the next
     * list.  Eventually we'll check the last list and either it'll 
     * work or we're out of business.
     */
    curfreelist = freelist_for_malloc(mem->freeroot, nwords);
    tripcount = WAY_TOO_MANY_TRIPS;
    for (;;) {
	freeblocktype *curfree;
	if (--tripcount < 0)
	    crashdump(0);	/* Too many trips */
	for (curfree = curfreelist->head; curfree->free_next;
	     curfree = curfree->free_next) {
	    bestchoice = freeblock_to_block(curfree);
	    if (--tripcount < 0)
		crashdump(0);	/* Too many trips */
	    /*
	     * Make sure free list is not clobbered:
	     */
	    if (!validblock(bestchoice, mem))
		return FALSE;
	    if (!valid_freeblock(bestchoice, mem, curfreelist->blocksize))
		return FALSE;
	    /*
	     * If we can fit into this block, go for it (FIRST FIT) (should
	     * always work except on the last list).
	     */
	    if (bestchoice->size >= nwords) {
		return(TRUE);
	    }
	}

	/* Ditch if we just tried the last list and failed. */

	if (IS_LAST_FREE_LIST(curfreelist))
	    break;
	curfreelist++;
    }

    /* If we fell out of the loop then there was nothing available. */

    return (FALSE);
}



/*
 * name_memory - Set an new name for a block of memory
 */

void name_memory (void *mem, char *name)
{
    blocktype *ptr = ((blocktype *)mem) - 1;

    ptr->allocator_name = name;
}

/*
 * memory_new_owner 
 *
 * Set the allocator pid and check value on this memory block.  This is
 * for when memory blocks are allocated by one process and passed to
 * another process, and then the first process exits.  Instead of
 * considering this memory as being owned by a dead process (and
 * therefore suspected as lost memory), it should be considered owned by
 * the new process.
 */
void memory_new_owner (void *mem)
{
    blocktype *ptr = data_to_block(mem);

    ptr->allocator_check = (ulong)forkx;
    process_get_pid(&ptr->allocator_pid);
}

/*
 * memory_specify_owner
 *
 * Used during process creation to name memory that will belong to 
 * the newly created process.
 */
void memory_specify_owner (void *mem, char *name, pid_t pid, ulong check)
{
    blocktype *ptr = data_to_block(mem);

    ptr->allocator_pid = pid;
    ptr->allocator_name = name;
    ptr->allocator_check = check;
}

/*
 * memory_specify_allocator
 *
 * Used by common subroutine (aka setstring) to reset the allocator pc.
 */
void memory_specify_allocator (void *mem, ulong caller_pc)
{
    blocktype *ptr = data_to_block(mem);

    ptr->allocator_pc = caller_pc;
}

/*
 * poison
 * Write an illegal address pattern into freed memory
 */

void poison (uchar *address, int count)
{
    ulong *lp;
    int lcount;
    ulong poison_value;
    uchar poison_value_char;

    /* First, do a few bytes so that we're word aligned. */
    
    poison_value_char = 0x0d;
    for (; count && ((int)address & 0x3); count--)
	*address++ = poison_value_char;

    /* Now do the longs. */
    lp = (ulong *) address;
    lcount = count >> 2;
    poison_value = 0x0d0d0d0d;
    for (; lcount > 16; lcount -= 16) {
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
	*lp++ = poison_value;
    }
    while (lcount) {
	*lp++ = poison_value;
	lcount--;
    }
    address = (uchar *) lp;
    count &= 0x3;

    /* Now do what's left. */
    for (; count; address++, count--)
	*address = poison_value_char;
}

/*
 * free
 * Release allocated memory. Nil pointers are ignored.
 * Obviously illegal pointers are ignored.
 * Must NOT be called from interrupt level.
 */

void free (void *ptr)
{
    register blocktype *piece, *pred, *succ;
    mempool *mem;
    ulong nbytes;
    freetype which_end;
    
    SAVE_CALLER();

    if (!ptr) {			/* ignore NIL pointers */ 
#ifdef FREEDEBUG
	(*kernel_buginf)("FREE: nil pointer from 0x%x\n", caller());
#endif
	return;
    }
    
    if (onintstack()) {
	(*kernel_errmsg)(&msgsym(INVMEMINT, SYS), "free");
	return;
    }

    mtrace(freetrace, &ixfree, ptr, caller());

    /*
     * Get an appropriate pool
     */
    mem = mempool_find_by_addr(ptr);
    if (!mem) {			/* bad pointer */
	(*kernel_errmsg)(&msgsym(FREEBAD, SYS), ptr);
	return;
    }

    if (mem->corrupt)
	return;

    /*
     * Make sure that pointer is valid and in use:
     */
    piece = data_to_block(ptr);
    if (!validblock(piece, mem))
	return;

    if (!(piece->size & MEMINUSE)) {
	CRASH_AND_BURN(mem, piece, 
		       (&msgsym(FREEFREE, SYS), ptr, piece->allocator_pc, 
			freeptr(piece)->last_deallocator),
		       BADFREE_INUSE);
	return;
    }

    if (!check_red(mem, piece, TRUE))
	return;

    if (piece->refcount < 1) {
	CRASH_AND_BURN(mem, piece, 
		       (&msgsym(BADSHARE, SYS), "free", piece, 
			piece->refcount),
		       BADFREE_REFCOUNT);
	return;
    }

    if (--piece->refcount)
	return;				/* Others are using the memory */

    /*
     * Free this memory:
     */
    piece->size &= FREEDMASK;
    mem->size += piece->size;	/* more free RAM now */

    nbytes = wtob(BLOCKOVERHEADWORDS + piece->size);

#ifdef MEMDEBUG
    poison((uchar *)ptr, wtob(piece->size));    /* write poison pattern */
    freeptr(piece)->last_deallocator = (ulong *) caller();
#endif

    /*
     * Can we merge this block with the previous block?
     */
    which_end = FREE_HEAD;
    pred = prev_block(piece, mem);	/* predecessor growth? */
    if (pred)				/* predecessor exist? */
	if (!(pred->size & MEMINUSE) && (block_end(pred) == piece)) {
	    delete_free_block(pred);
	    pred->size += (piece->size + BLOCKOVERHEADWORDS);
	    mem->size += BLOCKOVERHEADWORDS;	/* add Head to our count */
	    pred->next = piece->next;	/* skip over freed piece */
	    if (pred->next)		/* then, there is a forward link */
		(pred->next)->previous = &pred->next; /* back link to pred */
	    pred->allocator_name = "(coalesced)";
	    piece = pred;
	    which_end = FREE_TAIL;	/* Put coalesced blocks at the end */
	}

    /*
     * Can we merge this block with the following block?
     */
    succ = piece->next;	
    if (succ) {
	if (!(succ->size & MEMINUSE) && (block_end(piece) == succ)) {
	    delete_free_block(succ);
	    piece->size += (succ->size + BLOCKOVERHEADWORDS);	/* growing */
	    piece->next = succ->next;	/* skip successor */
	    mem->size += BLOCKOVERHEADWORDS;	/* add Head to our count */
	    if (piece->next)		/* then, we have a new successor */
		(piece->next)->previous = &piece->next;
	    piece->allocator_name = "(coalesced)";
	    which_end = FREE_TAIL;	/* Put coalesced blocks at the end */
	}
    }

    /*
     * Now put this block on the free list.
     */

    add_free_block(piece, freelist_find_by_block(mem, piece), which_end);

    if (!retting_buffer) {
	process_count_free(nbytes);
    } else {
	process_count_retbuffer(nbytes);
    }
    return;
}

/*
 * mem_lock - Add a reference to a piece of memory
 *
 * This routine is used to place an additional reference on a piece of memory
 * allocated by malloc(). free() will not return the buffer to the pool until
 * the reference count reaches zero.
 */

void mem_lock (void *ptr)
{
    blocktype *piece = data_to_block(ptr);

    if (piece->blockmagic != BLOCKMAGIC) {
	crashdump(0);			/* Woe is us */
    }

    if (piece->refcount < 1) {
	piece->refcount = 1;
	(*kernel_errmsg)(&msgsym(BADSHARE, SYS), "mem_lock", piece,
			 piece->refcount);
	return;
    }

    piece->refcount++;
}


/*
 * mem_lockable
 *
 * This routine tests to see if the memory block is from a valid
 * memory pool and if a mem_lock() can be performed on it.
 */
boolean mem_lockable (void *ptr)
{
    blocktype *piece = data_to_block(ptr);

    return((piece->blockmagic == BLOCKMAGIC));
}


/*
 * mem_refcount - Return the number of references to a block of memory.
 *
 * If a block of memory is being accessed via mem_lock() 
 * it can be useful to know how many references are currently open to it.
 * Return the reference count.
 */

int mem_refcount (void *ptr)
{
    blocktype *piece = data_to_block(ptr);

    return (piece->refcount);
}


/*
 * mempool_create
 *
 * Initialize a memory pool
 *
 * Sets up the data structure and links the entire block of memory
 * into the free list.
 */

mempool *mempool_create (mempool    *mempool,
			 char       *name,
			 regiontype *region,
			 ulong       alignment,
			 const ulong *free_list_sizes,
			 ulong       free_list_count, 
			 mempool_class class)
{
    blocktype *block;

    /*
     * Make sure that the supplied info is okay...
     */
    if ((!mempool) || (!name) || (!region) || (mempool_find_by_class(class)))
	return(NULL);

    /*
     * Zero out the pool
     */
    memset(mempool, 0, sizeof(mempool));

    /*
     * Set the pool internal variables
     */
    block = (blocktype *)region->start;
    mempool->memhead   = block;
    mempool->class     = class;
    mempool->name      = name;
    mempool->alignment = alignment;
    mempool->total     = region->size;

    /*
     * Initialize the region list and enqueue our region
     */
    if (!list_create(&mempool->regionlist, 0, mempool->name, 0))
	return(NULL);
    
    list_enqueue(&mempool->regionlist, &region->mempool_element, region);

    /*
     * If we don't have a freelist structure, use the default
     */
    if (!free_list_sizes) {
	free_list_sizes = freelist_default_block_sizes;
	free_list_count = nfreelist_default_block_sizes;
    }

    /*
     * Store the mempool pointer in the mempool type array.
     */
    mempools[class] = mempool;
    
    /*
     * Initialize our free tree
     */
    free_tree_init(mempool, free_list_sizes, free_list_count);

    /*
     * Create the intial mondo block and add it to the freelist
     */
    init_block(block, NULL, btow(region->size) - BLOCKOVERHEADWORDS,
	       TRUE, &mempool->memhead, (ulong) NULL);
    add_free_block(block, freelist_find_by_block(mempool, block), FREE_HEAD);

    /*
     * Remember the final working size (minus the header)
     */
    mempool->size = mempool->memhead->size;
    mempool->low_water = mempool->size;

    /*
     * Set the default watermarks
     */
    mempool_set_low_threshold(class, MEMPOOL_LOW_THRESHOLD);
    mempool_set_fragment_threshold(class, MEMPOOL_FRAGMENT_THRESHOLD);

    /*
     * Set the back pointer in the region structure to this mempool 
     */
    region->mempool = mempool;

    return(mempool);
}


/*
 * mempool_add_region
 *
 * Add a region to an existing memory pool
 *
 */

boolean mempool_add_region (mempool *mempool, regiontype *region)
{
    uint       size;
    blocktype *block, *tail;

    /*
     * Make sure that the supplied info is okay...
     */
    if ((!mempool) || (!region))
	return(FALSE);

    /*
     * Set the pool internal variables
     */
    block = (blocktype *)region->start;
    mempool->total += region->size;

    /*
     * Region meet mempool. Mempool meet region.
     */
    list_enqueue(&mempool->regionlist, &region->mempool_element, region);

    /*
     * Work out the basic size in words of the region as a mempool
     */
    size = btow(region->size) - BLOCKOVERHEADWORDS;
    
    /*
     * This is a relatively expensive way to find the tail but this
     * isn't a function that's called much
     */
    for (tail = mempool->memhead; tail->next; tail = tail->next);

    /*
     * Create the new mondo block and add it to the freelist
     */
    init_new_block(block, tail, size, (ulong)NULL);
    add_free_block(block, freelist_find_by_block(mempool, block), FREE_TAIL);

    /*
     * Remember the final working size (minus the header)
     */
    mempool->size += size;
    mempool->low_water = mempool->size;		/* Don't mislead! */

    /*
     * Set the back pointer in the region structure to this mempool 
     */
    region->mempool = mempool;

    return(TRUE);
}


/*
 * memory_init
 * 
 * Called at startup to initialize the region manager, memory pools
 * and chunk management.
 */

void memory_init (void)
{
    /*
     * Initialize region manager
     */
    region_init();

    /*
     * Let platforms start memory pools
     */
    platform_memory_init();

    /*
     * Make sure that the mandatory pools got declared
     */
    if (!(mempool_find_by_class(MEMPOOL_CLASS_LOCAL))  ||
	!(mempool_find_by_class(MEMPOOL_CLASS_IOMEM))  ||
	!(mempool_find_by_class(MEMPOOL_CLASS_FAST))   ||
	!(mempool_find_by_class(MEMPOOL_CLASS_ISTACK)) ||
	!(mempool_find_by_class(MEMPOOL_CLASS_PSTACK)))
	crashdump(0);

    /*
     * Initialize chunk manager
     */
    chunk_init();

    ixfree = ixmalloc = 0;
}

/*
 * mempool_add_alternate_pool
 *
 * Adds a fallback pool for allocation when memory in this pool runs out
 */
void mempool_add_alternate_pool (mempool *pool, mempool *alternate)
{
    /*
     * Basic sanity checks to remove some potential panty wetting
     */
    if ((pool == alternate) || (alternate->alternate == pool))
	return;

    pool->alternate = alternate;
}

/*
 * mempool_add_exception_pool
 *
 * Adds an exception pool for allocation when memory in this pool has
 * been marked corrupt.
 */
void mempool_add_exception_pool (mempool *pool, mempool *exception)
{
    /*
     * Basic sanity checks to remove some potential panty wetting
     */
    if ((pool == exception) || (exception->exception_pool == pool))
	return;

    pool->exception_pool = exception;
}

/*
 * mempool_add_exception_pool_init
 *
 * Adds an exception pool for allocation when memory in this pool has
 * been marked corrupt.
 */
void mempool_add_exception_pool_init (mempool *pool,
				      void (*exception_pool_init)(mempool *))
{
    pool->exception_pool_init = exception_pool_init;
}

/*
 * mempool_add_alias_pool
 *
 * Adds a pool alias for a MEMPOOL_CLASS_*. This allows pools to be
 * substitued for special cases (for example, if a box doesn't have
 * fast memory).
 */
void mempool_add_alias_pool (mempool_class class, mempool *alias)
{
    /*
     * Basic sanity checks
     */
    if (mempool_find_by_class(class))
	return;

    mempools[class] = alias;	
}

/*
 * mempool_delete_alias_pool
 * Delete a previously installed alias
 */
void mempool_delete_alias_pool (mempool_class class)
{
    mempools[class] = NULL;
}

/*
 * mempool_set_low_threshold
 *
 * Sets a low threshold for a memory pool class. This is used in
 * mempool_is_low() and mempool_is_empty().
 */
void mempool_set_low_threshold (mempool_class class, ulong low_threshold)
{
    mempool *mempool;

    /*
     * Basic sanity checks (make sure the pool exists and isn't an alias)
     */
    mempool = mempool_find_by_class(class);
    if (!mempool || mempool_is_alias(class))
	return;

    mempool->low_threshold = low_threshold;
}

/*
 * mempool_set_fragment_threshold
 *
 * Sets a fragment threshold for a memory pool class. This is used in
 * mempool_is_low() and mempool_is_empty().
 */
void mempool_set_fragment_threshold (mempool_class class,
				     ulong         fragment_threshold)
{
    mempool *mempool;

    /*
     * Basic sanity checks
     */
    mempool = mempool_find_by_class(class);
    if (!mempool || mempool_is_alias(class))
	return;

    mempool->fragment_threshold = fragment_threshold;
}

/*
 * validmem
 * Tests if location is part of a valid memory pool
 */

boolean validmem (void *location)
{
    return (mempool_find_by_addr(location) != NULL);
}

/*
 * validmem_complete
 *
 * Test to see if the location is a valid block.  Tests all safety valves.
 * Returns FALSE if the block is invalid.
 */
boolean validmem_complete (void *mem)
{
    blocktype *bp;
    mempool *mp;

    if (!mem)
	return(TRUE);

    mp = mempool_find_by_addr(mem);
    if (!mp)
	return(FALSE);

    bp = data_to_block(mem);
    if (!(bp->size & MEMINUSE))
	return(FALSE);

    return(validblock_gentle(bp, mp));
}

/*
 * Validate memory from an interrupt routine without crashing.
 * Note that the block must be in use, since free blocks cannot
 * remain self-consistent when viewed from interrupt routines.
 *
 * Returns TRUE if things are valid.
 */
boolean validmem_complete_interrupt (void *mem)
{
    blocktype *bp;
    mempool *mp;

    if (!mem)
	return(TRUE);

    mp = mempool_find_by_addr(mem);
    if (!mp)
	return(FALSE);

    bp = data_to_block(mem);
    if (!(bp->size & MEMINUSE))
	return(FALSE);

    return(validblock_inline(bp, mp, TRUE, TRUE)); /* Want to crash if
						      an error */
}

/*
 * validlink
 * Verify that a pointer is sensible
 */

boolean validlink (void *location)
{
    if (!location)
	return (TRUE);
    return (mempool_find_by_addr(location) != NULL);
}

/*
 * validate_memory
 * Analyze our memory allocation scheme
 */
static void validate_memory (mempool *mem)
{
    blocktype *block;
    int tripcount;
    sys_timestamp suspend_time;

    if (mem == NULL)
	return;

    block = mem->memhead;	/* start of list */
    if (mem->corrupt)
	return;			/* Don't bother--it's a mess. */
    TIMER_START(suspend_time, CHECKHEAPS_TIME);
    tripcount = WAY_TOO_MANY_TRIPS;
    while (block) {
	if (--tripcount < 0) {
	    crashdump(0);	/* We're busted */
	    break;
	}
	if (!validblock(block, mem))
	    break;
	if (!(block->size & MEMINUSE)) {
	    if (!valid_freeblock(block, mem, 0))
		break;
	}

	/*
	 * Suspend periodically.  We can safely do this by waiting until we
	 * are sitting on an in-use block;  we lock the block and then
	 * suspend.  We then back up to the previous block, unlock the
	 * one we were sitting on (which may free it, and which may be
	 * coalesced with the block we backed up to), and then we're off.
	 */
	if (AWAKE(suspend_time) && (block->size & MEMINUSE)
	    && (block->refcount >= 1)) {
	    blocktype *prev;
	    inline_mem_lock(block_to_data(block));
	    process_suspend();
	    
	    if (!validblock(block, mem))	/* check after suspend */
		break;
	    prev = prev_block(block, mem);
	    inline_mem_unlock_or_free(block_to_data(block));	
	    
	    /*
	     * Check if current block has been coalesced. If so then continue 
	     * from the previous block.
	     */
	    if (prev && prev->next != block)
		block = prev;
	    TIMER_START(suspend_time, CHECKHEAPS_TIME);
	}
	block = block->next;
    }
}

/*
 * validate_free_memory
 * Analyze our memory allocation scheme
 */

static void validate_free_memory(mempool *mem)
{
    blocktype *funrun = NULL;
    free_tree *tree = mem->tree_array;
    int tripcount;

    if (mem == NULL)
	return;

    if (mem->corrupt)
	return;

    if (!mem->memhead)
	return;

    tripcount = WAY_TOO_MANY_TRIPS;
    for (;;) {			/* Walk all the free lists */
	freeblocktype *curfree, *next;
	if (--tripcount < 0)
	    crashdump(0);	/* Data structures are messed up */
	curfree = tree->head;
	if (curfree->free_next) {
	    if (curfree->free_prev != &(tree->head)) {
		CRASH_AND_BURN(mem, funrun,
			       (&msgsym(BADFREEPTRS, SYS),
				"free_prev", 
				(ulong) curfree->free_prev,
				(ulong) freeblock_to_block(curfree),
				"head",
				&(tree->head)),
			       BADFREE_PREV_PTR);
	    }
	} else {
	    if (tree->tail.free_prev != &(tree->head)) {
		CRASH_AND_BURN(mem, funrun,
			       (&msgsym(BADFREEPTRS, SYS),
				"tail", 
				(ulong) tree->tail.free_prev,
				(ulong) tree,
				"head",
				&(tree->head)),
			       BADFREE_NEXT_PTR);
	    }
	}
	while (curfree->free_next) {
	    funrun = freeblock_to_block(curfree);
	    if (--tripcount < 0)
		crashdump(0);	/* What a mess. */
	    if (!validblock(funrun, mem))
		return;
	    if (!valid_freeblock(funrun, mem, tree->blocksize))
		return;
	    if (funrun->size < tree->minsize || funrun->size > tree->maxsize) {
		CRASH_AND_BURN(mem, funrun, 
			       (&msgsym(WRONGFREELIST, SYS), 
				(ulong) funrun, funrun->size, 
				tree->minsize, tree->maxsize),
			       BADFREE_WRONG_LIST);
	    }
	    next = curfree->free_next;
	    if (next) {
		if (next->free_prev != &curfree->free_next) {
		    CRASH_AND_BURN(mem, funrun,
				   (&msgsym(BADFREEPTRS, SYS),
				    "free_prev", 
				    (ulong) next->free_prev,
				    (ulong) freeblock_to_block(next),
				    "prev->free_next",
				    curfree->free_next),
				   BADFREE_PREV);
		}
	    } else {
		if (tree->tail.free_prev != &(freeptr(funrun)->free_next)) {
		    CRASH_AND_BURN(mem, funrun,
				   (&msgsym(BADFREEPTRS, SYS),
				    "tail",
				    (ulong) tree->tail.free_prev,
				    (ulong) tree,
				    "prev->free_next",
				    curfree->free_next),
				   BADFREE_TAIL);
		}
	    }
	    curfree = next;
	}
	if (tree == mem->last_free_pool)
	    break;
	tree++;
    }
}

/*
 * mempool_is_low
 *
 * Return TRUE if we're below the low threshold. Doesn't check alternates
 * nor fragmentation
 */

boolean mempool_is_low (mempool_class class)
{
    mempool *mempool = mempool_find_by_class_inline(class);

    if (mempool &&
	((!mempool->low_threshold) ||
	 (mempool->size > btow(mempool->low_threshold))))
	return(FALSE);
    else
	return(TRUE);
}

/*
 * mempool_is_empty
 *
 * A predicate for ascertaining memory scarcity. Checks everything and
 * the kitchen sink.
 */

boolean mempool_is_empty (mempool_class class)
{
    mempool *mempool, *alternate;

    mempool = mempool_find_by_class_inline(class);
    if (!mempool)
	return(TRUE);

    if (memory_available(mempool->fragment_threshold, mempool)) {
	/*
	 * If we've got a low watermark, check it
	 */
	if (!mempool->low_threshold ||
	    (mempool->size > btow(mempool->low_threshold)))
	    return(FALSE);
    }

    /*
     * Check to see what we've got in the alternate pool if it's
     * available
     */
    alternate = mempool->alternate;

    if (alternate &&
	memory_available(alternate->fragment_threshold, alternate)) {
	/*
	 * If we've got a low watermark for the alternate...
	 */
	if (!alternate->low_threshold ||
	    (alternate->size > btow(alternate->low_threshold)))
	    return(FALSE);
    }

    /*
     * "Yes! We have no bananas, da dah, da da dah..."
     */
    return(TRUE);
}

/*
 * mempool_get_free_bytes
 * Return number of bytes of free memory. Look at just the
 * specified memory pool or its alternate pool as well (if
 * one exists) based on the 'alternate' flag passed in.
 * Used by other routines outside of this module.
 */

ulong mempool_get_free_bytes (mempool_class class, boolean alternate)
{
    mempool *mempool = mempool_find_by_class_inline(class);
    ulong free_bytes;

    if (!mempool)
	return(0);

    free_bytes = mempool->size;
    if (alternate && mempool->alternate)
	free_bytes += mempool->alternate->size;

    return (wtob(free_bytes));
}

/*
 * mempool_get_used_bytes
 * Return number of bytes of used memory.
 * Look at just the specified memory pool or its alternate pool
 * as well (if one exists) based on the 'alternate' flag passed in.
 * Used by other routines outside of this module.
 */
ulong mempool_get_used_bytes (mempool_class class, boolean alternate)
{
    mempool *mempool = mempool_find_by_class_inline(class);
    ulong used_bytes;

    if (!mempool)
	return(0);

    used_bytes = mempool->total - wtob(mempool->size);
    if (alternate && mempool->alternate) {
	mempool = mempool->alternate;
	used_bytes += (mempool->total - wtob(mempool->size));
    }

    return (used_bytes);
}

/*
 * mempool_get_total_bytes
 * Return number of bytes of memory available.
 * Look at just the specified memory pool or its alternate pool
 * as well (if one exists) based on the 'alternate' flag passed in.
 * Used by other routines outside of this module.
 */
ulong mempool_get_total_bytes (mempool_class class, boolean alternate)
{
    mempool *mempool = mempool_find_by_class_inline(class);
    ulong tot_bytes;

    if (!mempool)
	return(0);

    tot_bytes = mempool->total;
    if (alternate && mempool->alternate)
	tot_bytes += mempool->alternate->total;

    return (tot_bytes);
}

/*
 * validpiece - Check if a piece of memory is still valid
 *
 * This routine is used to ensure that a piece of memory is still valid -
 * all links and guard words are checked for validity.
 */

boolean validpiece (void *ptr, boolean validate_block)
{
    blocktype *piece;
    mempool *mempool;
    
    mempool = mempool_find_by_addr(ptr);
    
    if (!mempool)
	return (FALSE);		/* Bad pointer */
    
    if (!validate_block)
	return(TRUE);		/* No further checking */
    
    piece = data_to_block(ptr); /* Get header pointer */
    
    return (validblock(piece, mempool)); /* Return status */
}

/*
 * linkinblock
 *
 * Check to see if a pointer points into a particular block of memory.
 */

boolean linkinblock (void *ptr, void *blockptr)
{
    uchar *chptr = ptr;
    uchar *chblockptr = blockptr;
    blocktype *piece = data_to_block(blockptr);
    mempool *mem = mempool_find_by_addr(blockptr);
    ulong size = block_size(blockptr);

    if (!mem)
      return (FALSE);		/* Bad pointer */
    if (!validblock(piece, mem))
	return (FALSE);

    return (chptr >= chblockptr && chptr < chblockptr + size);
}

/*
 * realloc - Reallocate a block of memory at a different size
 *
 * If we were unable to satisfy the request, return a null, and leave
 * the passed in buffer unaltered (i.e. it's not freed)
 */

void *realloc (void *ptr, size_t size)
{
    SAVE_CALLER();
    blocktype *old_hdr, *new_hdr;
    void *newbuf;
    int current_size;

    if (ptr == NULL) {
	newbuf = malloc(size);
	if (newbuf) {
	    new_hdr = data_to_block(newbuf);
	    new_hdr->allocator_pc = caller();
	}
	return(newbuf);
    }

    old_hdr = data_to_block(ptr);
    current_size = wtob(old_hdr->size & FREEDMASK) - REDZONEBYTES;
    if ((size > current_size) || ((current_size - size) > REALLOC_TRIM)) {
	newbuf = malloc(size);
	if (newbuf) {
	    bcopy(ptr, newbuf, min(current_size, size));
	    new_hdr = data_to_block(newbuf);
	    new_hdr->allocator_name = old_hdr->allocator_name;
	    new_hdr->allocator_pc = caller();
	    free(ptr);
	}
	return(newbuf);
    }

    return(ptr);
}

/*
 * get_block_size - get the full size in bytes of a memory block
 */
ulong get_block_size (void *memptr, ulong *overheadbytes)
{
    ulong size;

    if (!memptr)
	return (0);

    size = block_size(memptr);
    if (overheadbytes != NULL)
	*overheadbytes = MEMORY_BLOCK_OVERHEAD;
    return(size);
}

/*
 * tally_memory_usage - Add up memory utilization by task.
 *
 */
#define TALLY_SUSPEND_INTERVAL 100 /* msec */

void tally_memory_usage(mempool_class class, boolean alternate,
			ulong *initcount, ulong *schedcount, ulong *deadcount)
{
    sprocess_list *array;
    sprocess *sp;
    int i;
    blocktype *curblock, *prevblock;
    ulong blocksize;
    sys_timestamp suspend_time;
    mempool *altpool;
    mempool *mempool = mempool_find_by_class(class);
    
    if (!mempool)
	return;

    /* Initialize. */

    TIMER_START(suspend_time, TALLY_SUSPEND_INTERVAL);
    *initcount = *schedcount = *deadcount = 0;
    for (array = sarrayQ.qhead; array; array = array->sl_next) {
	for (i = 0; i < MAX_PROC_PER_ARRAY; i++) {
	    sp = array->sl_process[i];
	    if (sp)
		sp->mem_holding = 0;
	}
    }

    /*
     * Walk all blocks in the memory pool.
     * If there's an alternate pool, walk all blocks
     * in that pool too.
     */
    altpool = alternate ? mempool->alternate : NULL;
    while (mempool) {
	for (curblock = mempool->memhead;
	     curblock;
	     curblock = curblock->next) {

	    int pid;
	    if (curblock->refcount <= 0) /* It's not in use */
		continue;
	
	    /* Suspend periodically. */
	
	    if (AWAKE(suspend_time)) {
		inline_mem_lock(block_to_data(curblock)); /* Lock it. */
		process_suspend();
	    
		TIMER_START(suspend_time, TALLY_SUSPEND_INTERVAL);
		if (!validblock(curblock, mempool)) /* Just in case. */
		    break;
		prevblock = prev_block(curblock, mempool); /* We might free it. */
		inline_mem_unlock_or_free(block_to_data(curblock));
		if (prevblock)
		    curblock = prevblock->next;
		if (curblock == NULL) /* We coalesced into the last block. */
		    break;
		if (curblock->refcount <= 0) /* It's free now */
		    continue;
	    }
	
	    blocksize = wtob(curblock->size & FREEDMASK) + 
		sizeof(blocktype);
	    pid = curblock->allocator_pid;
	    if (curblock->allocator_pid == ALLOC_PID_INIT) {
		*initcount += blocksize;
	    } else if (curblock->allocator_pid == ALLOC_PID_SCHED) {
		*schedcount += blocksize;
	    } else if (curblock->allocator_pid < MIN_ALLOC_PID) {
		*deadcount += blocksize;
	    } else {
		/*
		 * A live PID.  If the scheduler block address doesn't
		 * match what's in the memory block, call it dead.
		 */
		sp = process_pid2ptr(pid);
		if (!sp || (curblock->allocator_check != (ulong)sp)) {
		    *deadcount += blocksize;
		} else {
		    sp->mem_holding += blocksize;
		}
	    }
	}
	mempool = altpool;
	altpool = NULL;  /* Lets not loop forever */
    }
}

/*
 * mempool_checkheap_for_scheduler - Do a quick test of all memory
 *
 * This routine takes a quick walk through the free list to determine
 * if there are any corrupt pointers. It does this *only* by verifying
 * that the block magic word and the red zone if the block is in use
 * are OK. This is what makes this routine *fast*.
 * This routine should not be confused with the far more thorough
 * checkheaps().
 */

static void mempool_checkheap_for_scheduler (
    mempool_class class,
    char *process,
    void *sched_test)
{
    blocktype *bp;
    mempool *mempool = mempool_find_by_class(class);

    if ((!mempool) || (mempool->corrupt))
	return;

    for (bp = mempool->memhead; bp; bp = bp->next) {
	if (bp->blockmagic != BLOCKMAGIC) {
	    CRASH_AND_BURN_FOR_SCHEDULER(mempool, bp, 
		      (&msgsym(BADMAGIC, SYS), (ulong) bp, bp->blockmagic),
		      BADBLOCK_MAGIC, process, sched_test);
	}
	/*
	 * For active blocks, the red zone area must be valid.
	 */
	if (bp->size & MEMINUSE) {
	    check_red_for_scheduler(mempool, bp, process, sched_test);
	}
    }
}

/*
 * checkheaps--check all pools and free lists
 */
void checkheaps (void)
{
    mempool *pool;
    mempool_class class;

    FOR_ALL_MEMPOOLS(pool, class) {
	if (pool && !mempool_is_alias(class)) {
	    validate_memory(pool);
	    validate_free_memory(pool);
	}
    }

    /*
     * Call platform specific memory checker for special regions
     * of memory (that are not normal heaps).
     */
    reg_invoke_check_memory_region();
}

/*
 * checkheaps_for_scheduler--quickly check all pools and free lists
 */
void checkheaps_for_scheduler (char *process, void *sched_test)
{
    mempool_class class;

    for (class = MEMPOOL_CLASS_UNKNOWN; class < MEMPOOL_CLASS_MAX; class++) {
	if (mempool_is_alias(class))
	    continue;
	mempool_checkheap_for_scheduler(class, process, sched_test);
    }
}

/*
 * mempool_set_check_timer
 *
 * Set a timer to the next expiration interval, or disable it if the
 * interval is zero.
 */
void mempool_set_check_timer(mgd_timer *timer, ulong interval)
{
    if (interval) {
	mgd_timer_start(timer, interval);
    } else {
	mgd_timer_stop(timer);
    }
}

/*
 * Heap check task.  Runs periodically to check all of the heaps.
 *
 * This is a separate task, so that if it detects an error and crashes,
 * it won't crash any important tasks necessary for taking a core dump.
 */
static process checkheaps_process (void)
{
    mgd_timer *expired_timer;

    /* Check for a corrupted system image. */
    validate_sum();

    /*
     * Initialize the timers as appropriate.  Note the cheap hack in starting
     * the heap check timer--if the interval is zero (no checking) it will
     * stay zero; otherwise it will start half a period early.  This is because
     * the default settings for both timers are the same, so this will make
     * them alternate every half period.  Cheap, but I already told you that.
     */
    mempool_set_check_timer(&heapcheck_timer, heapcheck_ivl / 2);
    mempool_set_check_timer(&cksumcheck_timer, cksumcheck_ivl);

    for (;;) {			/* Run forever. */
	process_sleep_on_timer(&check_timer);
	while (mgd_timer_expired(&check_timer)) {
	    expired_timer = mgd_timer_first_expired(&check_timer);
	    switch (mgd_timer_type(expired_timer)) {
	      case HEAP:
		checkheaps();		/* Check those nasty heaps. */
		mempool_set_check_timer(&heapcheck_timer, heapcheck_ivl);
		break;
	      case CKSUM:
		validate_sum();		/* Check for code corruption */
		mempool_set_check_timer(&cksumcheck_timer, cksumcheck_ivl);
		break;
	    }
	}
    }
}

/*
 * init_checkheaps - start up the heap check task
 */
void init_checkheaps (void)
{
    heapcheck_ivl = HEAPCHECK_IVL_DEFAULT;
    cksumcheck_ivl = CKSUMCHECK_IVL_DEFAULT;
    mgd_timer_init_parent(&check_timer, NULL);
    mgd_timer_init_leaf(&heapcheck_timer, &check_timer, HEAP, NULL, FALSE);
    mgd_timer_init_leaf(&cksumcheck_timer, &check_timer, CKSUM, NULL, FALSE);

    /* Fire up the heap check task. */
    checkheaps_pid = process_create(checkheaps_process, "Check heaps",
				    NORMAL_STACK, PRIO_LOW);
}

/*
 * return the memory available for buffers
 */
int get_available_buf_mem (void)
{
    return(mempool_get_total_bytes(MEMPOOL_CLASS_IOMEM,
				   MEMPOOL_ALTERNATE_IGNORE));
}

/*
 * mempool_get_largest_block -- return the size of the largest block
 * from the selected mempool
 */
ulong mempool_get_largest_block (mempool_class class)
{
    free_tree 	  *freelist;
    mempool 	  *mempool;
    ulong 	   max_size = 0;
    freeblocktype *freeblock;
    blocktype     *block = NULL;

    mempool = mempool_find_by_class_inline(class);

    if (mempool == NULL) {
	return 0;
    }

    /*
     * Find the last free list that has any blocks on it.
     */
    freelist = mempool->last_free_pool;
    while (freelist->head->free_next == NULL) {
	if (freelist == mempool->tree_array) {
	    return(0);			/* No blocks at all! */
	}
	freelist--;
    }

    /* 
     * now lets sift through this list looking for the largest block 
     */
    for (freeblock = freelist->head; 
	 freeblock->free_next; 
	 freeblock = freeblock->free_next) {

	block = freeblock_to_block(freeblock);

	/*
	 * do some validity checks for sanity
	 */
	if (!validblock(block, mempool))
	    return 0;
	if (!valid_freeblock(block, mempool, freelist->blocksize))
	    return 0;

	if (block->size > max_size) {
	    max_size = block->size;
	}
    }
    /* 
     * remove REDZONE bytes so that the returned area matches 
     * the maximum requested size
     */
    return(wtob(max_size) - REDZONEBYTES);
}

/*
 * setstring
 * Copy a string into free storage (if available), and make 'dest' 
 * point to that storage.
 *
 * Alert: On return from setstring(), it's the caller's responsibility 
 *        to check that *dest is not NULL.  If you find invocations of
 *        setstring() that don't check *dest on return, please fix them.
 */

void setstring (char **dest, const char *src)
{
    SAVE_CALLER();

    if (*dest != NULL)
	free (*dest);			/* release destination string */
    if (!src || !(*src)) {		/* if no or NULL source string .... */
	*dest = NULL;			/* just NULL out the destination */
	return;				/* and return now */
    }
    *dest = malloc(strlen(src) + 2);
    if (*dest) {
	strcpy(*dest, src);
	memory_specify_allocator(*dest, caller());
    }
}

/*
 * setstring_named
 *
 * Same as setstring, except does a malloc_named, instead of plain malloc.
 * Usefull for faster memory debugging, but higher memory-usage, because of
 * the name
 *
 * Alert: On return from setstring(), it's the caller's responsibility 
 *        to check that *dest is not NULL.  If you find invocations of
 *        setstring() that don't check *dest on return, please fix them.
 */

void setstring_named (char **dest, const char *src, char *name)
{
    SAVE_CALLER();

    if (*dest != NULL)
	free (*dest);			/* release destination string */
    if (!src || !(*src)) {		/* if no or NULL source string .... */
	*dest = NULL;			/* just NULL out the destination */
	return;				/* and return now */
    }
    *dest = malloc_named(strlen(src) + 2, name);
    if (*dest) {
	strcpy(*dest, src);
	memory_specify_allocator(*dest, caller());
    }
}
