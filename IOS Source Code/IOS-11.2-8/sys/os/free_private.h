/* $Id: free_private.h,v 3.5.12.6 1996/08/28 13:03:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/free_private.h,v $
 *------------------------------------------------------------------
 * Private include for free.c
 *
 * December 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: free_private.h,v $
 * Revision 3.5.12.6  1996/08/28  13:03:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.5  1996/06/11  19:19:46  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Restore ukernel build using 'kernel_validblock_diagnose'
 *
 * Revision 3.5.12.4  1996/06/04  05:16:05  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.5.12.3  1996/04/13  06:05:31  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.5.12.2  1996/04/02  05:37:46  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.5.12.1  1996/03/18  21:29:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:20:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/09  05:09:28  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  16:39:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/20  06:15:31  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.4  1996/01/25  18:38:01  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.3  1995/11/17  18:47:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:37:07  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/06/18  06:03:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.1  1995/06/07  21:55:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../os/buffers_debug.h"       /* Required */
#include "logger.h"
#ifndef __MSG_SYSTEM_C__
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#endif

/*
 * Externals:
 */

/*
 * Structure for recording malloc failures
 */
typedef struct malloc_failures_ {
    sys_timestamp failtime;		/* Milliseconds since boot */
    mempool *pool;			/* Pool we tried to get from */
    ulong size;				/* Requested size in bytes */
    ulong alignment;			/* Requested alignment */
    ulong calling_pc;			/* Caller's address */
} malloc_failures;

#define MALLOC_FAILURE_RECORD_COUNT 10	/* Record the last 10 failures */
extern malloc_failures malloc_failure_trace[MALLOC_FAILURE_RECORD_COUNT];
extern int malloc_failure_trace_number;	/* Next element to fill in */

extern mempool *mempools[];
extern ulong heapcheck_ivl;
extern ulong cksumcheck_ivl;
extern mgd_timer heapcheck_timer;
extern mgd_timer cksumcheck_timer;

extern free_tree *freelist_find_by_size (free_tree *tree, ulong size);
extern void mempool_set_check_timer(mgd_timer *timer, ulong interval);
extern boolean spillmem(mempool *mem,
			boolean printing,
			boolean only_lost_memory,
			boolean alloc_process,
			ulong *usedbytes,
			ulong *freebytes,
			ulong *largestfree,
			ulong *lowestfree);

/*
 * PID defines
 */

#define ALLOC_PID_INIT -2	/* allocator_pid for initialization */
#define ALLOC_PID_SCHED -1	/* allocator_pid for scheduler */
#define MIN_ALLOC_PID -2	/* smallest legal value */

/*
 * Private constants
 */

#define HEAPCHECK_IVL_DEFAULT 60*ONESEC	/* Heap check interval default */
#define CKSUMCHECK_IVL_DEFAULT 60*ONESEC /* Code checksum interval default */
#define CHECKHEAPS_TIME 100	/* duration to run spillmem before s_suspend */

/*
 * Fetch the address of the previous block based on the address of this
 * block.  All the gorp below really reduces to an indexed indirect reference.
 */

static inline blocktype *prev_block(blocktype *ptr, mempool *mem) {
    if (ptr == mem->memhead)
	return(NULL);
    return ((blocktype *) ((uchar *) (ptr->previous) - 
			   ((uchar *) &(ptr->next) - (uchar *) ptr)));
}

#define IS_LAST_FREE_LIST(tree) (tree->maxsize == MAXBLOCKSIZE)

/*
 * Fetch the address of the previous free block based on the address of this
 * block.  All the gorp below really reduces to an indexed indirect reference.
 */
#define PREV_FREE_BLOCK(ptr) ((freeblocktype *) ((uchar *) ((ptr)->free_prev) - ((uchar *) &((ptr)->free_next) - (uchar *) ptr)))

#define FREEMAGIC 0xdeadbeef
#define MAXBLOCKSIZE 0xffffffff

#define WAY_TOO_MANY_TRIPS 10000000 /* Call this infinity + 1... */

#ifndef GROVEL
/*
 * block_size - return overall block size of a memory block given a
 * pointer to the data area.
 */
static inline ulong block_size (void *ptr)
{
    blocktype *block = data_to_block(ptr);
    if (block->blockmagic != BLOCKMAGIC) {
	crashdump(0);			/* Woe is us */
    }
    return(sizeof(blocktype) + wtob(block->size & FREEDMASK));
}

/*
 * block_end - return at pointer to the end of a block
 */
static inline blocktype *block_end (blocktype *block)
{
    ushort *end;

    end = (ushort *)block + BLOCKOVERHEADWORDS + (block->size & ~MEMINUSE);
    return((blocktype *)end);
}


/*
 * Macro to call when preparing to die.  The silliness with "crash_dummy"
 * is a kludge to force the compiler to generate separate calls to
 * crashdump() to make it easier to track things down.  Make sure the
 * message differs between calls to CRASH_AND_BURN in the same procedure
 * if you want this to work.
 */
#define CRASH_AND_BURN(pool, block, errmsg_params, code) \
{\
    char *blockmsg;\
    (pool)->corrupt = TRUE;\
    blockmsg = (*kernel_validblock_diagnose)(pool, block, code);\
    (*kernel_errmsg) errmsg_params;\
    errmsg_mtrace();\
    errmsg_blkinfo((blockmsg), (block));\
    if ((pool)->exception_pool_init)\
	(pool)->exception_pool_init(pool);\
    crashdump(0);\
    crash_dummy = (blockmsg);\
}
#define CRASH_AND_BURN_FOR_SCHEDULER(pool, block, errmsg_params, code, process, qualifier) \
{\
    char *blockmsg;\
    (pool)->corrupt = TRUE;\
    blockmsg = (*kernel_validblock_diagnose)(pool, block, code);\
    (*kernel_errmsg) errmsg_params;\
    errmsg_mtrace();\
    errmsg_blkinfo((blockmsg), (block));\
    if (sched_test) {\
        (*kernel_errmsg)(&msgsym(TESTINFO, SYS), sched_test, process);\
    } else {\
        (*kernel_errmsg)(&msgsym(PROCINFO, SYS), process);\
    }\
    if ((pool)->exception_pool_init)\
	(pool)->exception_pool_init(pool);\
    crashdump(0);\
    crash_dummy = (blockmsg);\
}
static uchar *crash_dummy;	/* Not actually used for anything... */


/*
 * Validate block on free list.  Returns TRUE if block is valid.
 * The compiler will optimize out the tests for "crash" if called with
 * a constant.
 */
static inline boolean valid_freeblock_inline (blocktype *curblock, 
					      mempool *mem,
					      ulong freelist_size, 
					      boolean crash)
{
    ulong magic;

    /*
     * Make sure free list is not clobbered:
     */
    if (curblock->size & MEMINUSE) {
	if (crash)
	    CRASH_AND_BURN(mem, curblock, 
			   (&msgsym(INUSEFREE, SYS), (ulong) curblock, 
			    freelist_size), 
			   BADBLOCK_INUSE);
	return FALSE;
    }
    
    if (curblock->refcount != 0) {
	if (crash)
	    CRASH_AND_BURN(mem, curblock,
			   (&msgsym(NZREFCNT, SYS), (ulong) curblock,
			    freelist_size, curblock->refcount),
			   BADBLOCK_REFCOUNT);
	return FALSE;
    }

    magic = freeptr(curblock)->freemagic;
    if (magic != FREEMAGIC) {
	if (crash)
	    CRASH_AND_BURN(mem, curblock,
			 (&msgsym(BADFREEMAGIC, SYS), (ulong) curblock, magic),
			 BADBLOCK_FREEMAGIC);
	return FALSE;
    }
			
    return TRUE;
}


/*
 * mempool_find_by_class_inline
 *
 * Return a mempool pointer given a memory class
 */

static inline mempool *mempool_find_by_class_inline (mempool_class class)
{
    return(mempools[class]);
}


/*
 * mempool_find_region_by_addr
 * 
 * Scan a mempool region to list and return the region if the memory
 * address given is within it's bounds.
 */

static inline regiontype *mempool_find_region_by_addr (mempool *pool,
						       void    *location)
{
    list_element *element;
    regiontype   *region;

    FOR_ALL_DATA_IN_LIST(&pool->regionlist, element, region) {
	if ((location >= (void *)region->start) &&
	    (location < (void *)region->end))
	    return (region);
    }

    return(NULL);
}


/*
 * mempool_is_alias
 *
 * Return a boolean value indicating whether a mempool is aliased or
 * not for this class
 */
static inline boolean mempool_is_alias (mempool_class class)
{
    mempool *mempool = mempool_find_by_class_inline(class);

    if (!mempool)
	return(FALSE);

    return(mempool->class != class);
}
#endif /* GROVEL */
