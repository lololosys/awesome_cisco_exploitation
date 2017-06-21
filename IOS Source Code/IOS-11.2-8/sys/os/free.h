/* $Id: free.h,v 3.6.12.7 1996/09/06 01:11:39 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/free.h,v $
 *------------------------------------------------------------------
 * free.h -- free storage constants
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: free.h,v $
 * Revision 3.6.12.7  1996/09/06  01:11:39  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.6.12.6  1996/08/28  13:03:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.5  1996/06/11  19:19:44  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Restore ukernel build using 'kernel_validblock_diagnose'
 *
 * Revision 3.6.12.4  1996/06/04  05:16:03  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.6.12.3  1996/04/02  05:37:43  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.6.12.2  1996/03/21  23:21:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.12.1  1996/03/18  21:29:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:25:48  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/12  23:47:24  mbeesley
 * CSCdi48761:  Need to be able to delete memory alias
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  08:42:44  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.6  1996/02/20  06:15:29  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.5  1996/02/12  18:18:29  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.4  1996/02/08  20:13:08  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1996/01/25  18:37:59  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.2  1995/11/17  18:47:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:18:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/19  18:38:42  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.6  1995/08/12  07:09:38  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.5  1995/08/06  14:23:47  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.4  1995/06/28  05:56:40  dkatz
 * CSCdi36468:  MALLOCFAIL at boot up
 * Move malloc failure reporting so that the box doesn't whimper when
 * falling over to an alternate pool.
 *
 * Revision 2.3  1995/06/20  06:39:53  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Invoke a "show mem fail alloc" when the parser can't get enough memory
 * to parse a command.
 *
 * Revision 2.2  1995/06/18  06:03:32  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.1  1995/06/07  21:55:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Memory allocation system
 *
 * Available memory in each pool (main memory, sram, io memory, etc.) is
 * organized as a series of free lists.  When a request is made for a block
 * of memory, a block is taken from the appropriate free list based on
 * size.  The free list selection process is performed using a binary search.
 *
 * A request is always satisfied from the list containing blocks that are
 * greater than or equal to the size of the requested block, so that
 * the first entry on the list will always be big enough, except for the
 * last list, which must satisfy arbitrarily large requests.
 *
 * If a free list is empty, successive free lists are examined.
 *
 * When a block is found that satisfies the request, it is trimmed to the
 * requested size, and the remainder is re-queued in another free list.
 * If the remaining fragment is smaller than MINDATAWORDS, it is not
 * trimmed.
 *
 * The free memory information lives in the data portion of the block.
 */
#ifndef __FREE_H__
#define __FREE_H__

/* debug malloc and free code */
/* #define FREEDEBUG	1 */

#define MAX_FREE_POOLS 63	/* Max number of free pools */

/* Memory check error codes 
 * This enumeration is to define various types of memory corruptions.
 * There is an array of messages defined in buffers_debug.c that
 * contains a text string for each enumberation value.  If a new value
 * is added here, then an appropriate string needs to be added to the 
 * 'bad_msg' array in buffers_debug.c.
 */
typedef enum bad_code {
    BADFREE_NULL = 0,
    BADBLOCK_REDZONE,
    BADBLOCK_MAGIC,
    BADBLOCK_PREVIOUS,
    BADBLOCK_PREV_SIZE,
    BADBLOCK_NEXT,
    BADBLOCK_NEXT_SIZE,
    BADBLOCK_MEMTOP,
    BADBLOCK_INUSE,
    BADBLOCK_REFCOUNT,
    BADBLOCK_FREEMAGIC,
    BADBLOCK_NOTALIGNED,
    BADFREE_INUSE,
    BADFREE_REFCOUNT,
    BADFREE_PREV_PTR,
    BADFREE_NEXT_PTR,
    BADFREE_WRONG_LIST,
    BADFREE_PREV,
    BADFREE_TAIL
} bad_code_t;


typedef char * (*kernel_validblock_diagnose_t)(mempool *, blocktype *,\
					   bad_code_t);
extern kernel_validblock_diagnose_t kernel_validblock_diagnose;
extern void kernel_set_validblock_diagnose_hook(kernel_validblock_diagnose_t);

   
/*
 * Free block information.
 */
typedef struct freeblocktype_ {
    ulong freemagic;
    ulong *last_deallocator;    /* Who deallocated this block? */
    ulong dummy[2];		/* Some protection against pointer smashes */
    struct freeblocktype_ *free_next; /* Next free pointer */
    struct freeblocktype_ **free_prev; /* Previous free pointer */
} freeblocktype;

#define freeptr(p) ((p)->data_area.free_info)

/*
 * Basic memory block structure
 */
struct blocktype_ {
    ulong blockmagic;
    pid_t allocator_pid;	/* Process that allocated this block */
    ulong allocator_check;	/* allocator's process addr */
    const char *allocator_name;	/* name of process that allocated */
    ulong allocator_pc;		/* PC of allocator */
    struct blocktype_ *next;	/* next block, or NULL */
    struct blocktype_ **previous; /* ptr to previous block's next ptr */
    int size;			/* size of data part in words, bit 31 set if 
				   in use */
    int refcount;		/* Count of references to this memory */
    union {
	uchar data[0];		/* Start of real data */
	freeblocktype free_info[0]; /* Start of free block info */
    } data_area;
};
#define BLOCKMAGIC 0xAB1234CD	/* Magic value for blockmagic */

/*
 * Entry in the free list tree.  The tree is organized as a binary
 * tree;  the left branch leads to smaller blocks and the right branch
 * leads to larger blocks.
 *
 * Note that the tail is an entire freeblock type.  This makes it
 * possible to delink blocks from a free list without knowing which
 * free list it is on.  This has a side effect that the head pointer
 * is always non-null (when the list is empty, the head pointer points
 * at the tail), and that the tail free_prev pointer is always
 * non-null (it points to the head when the list is empty).
 *
 * This leads to a curious construct--to determine whether you're
 * looking at the last item on a free list, you look at its
 * successor's free_next pointer--if that pointer is NULL, you're
 * looking at the last item (the successor is the tail structure).
 */
typedef struct free_tree_ {
    ulong blocksize;		/* Nominal block size */
    ulong minalloc;		/* Min req size serviced by this list */
    ulong maxalloc;		/* Max req size serviced by this list */
    ulong minsize;		/* Min block size on this list */
    ulong maxsize;		/* Max block size on this list */
    freeblocktype *head;	/* Head of this free pool */
    freeblocktype tail;		/* Tail of this free pool */
    struct free_tree_ *left;	/* Left child (smaller blocks) */
    struct free_tree_ *right;	/* Right child (larger blocks) */
    boolean configured;		/* TRUE if this list was hand-configured */
} free_tree;

/*
 * Define the possible mempool classes
 * Note that the CISCO-MEMORY-POOL-MIB relies on the ordering of these.
 * Please place new definitions after MEMPOOL_CLASS_MULTIBUS
 */

typedef enum mempool_class_ {
    MEMPOOL_CLASS_UNKNOWN  = 0,
    MEMPOOL_CLASS_LOCAL,
    MEMPOOL_CLASS_IOMEM,
    MEMPOOL_CLASS_PCIMEM,
    MEMPOOL_CLASS_FAST,
    MEMPOOL_CLASS_MULTIBUS,
    MEMPOOL_CLASS_ISTACK,
    MEMPOOL_CLASS_PSTACK,
    MEMPOOL_CLASS_LOCAL_EXCEPTION,
    MEMPOOL_CLASS_MAX
} mempool_class;

/*
 * Memory pool.
 */
struct mempool_ {
    char          *name;	   /* name of memory pool */
    mempool_class  class;          /* mempool class */
    list_header    regionlist;     /* memory regions for pool */
    ulong          size;           /* size of available memory, in words */
    ulong	   low_water;	   /* smallest size experienced thusfar */
    ulong          total;          /* total amount of memory in bytes */
    ulong          alignment;      /* basic alignment for memory */
    blocktype     *memhead;	   /* first block in the pool */
    long           memtest;        /* set to magic if testing underway */
    boolean        corrupt;	   /* if we have detected pool corruption */
    ulong          low_threshold;  /* low memory threshold in bytes */
    ulong          fragment_threshold; /* fragment threshold in bytes */
    mempool       *alternate;      /* Alternate Pool relative */
    free_tree     *freeroot;	   /* ptr to root of free tree */
    free_tree     *last_free_pool; /* pointer to the last free list */
    ulong          freelist_count; /* Count of free lists in use */
    free_tree      tree_array[MAX_FREE_POOLS]; /* Free tree array */
    mempool       *exception_pool; /* Pool used when this pool is corrupt */
    void         (*exception_pool_init)(mempool *);
};

#define FOR_ALL_MEMPOOLS(pool, i)     \
    for ((pool) = mempools[0], (i)=0; \
	 (i) < MEMPOOL_CLASS_MAX;    \
	 (pool) = mempools[++(i)])

/*
 * Numerous confusing constants
 */

/*
 * MEMPOOL_ALTERNATE_IGNORE - Ignore any alternate pool if set
 * MEMPOOL_ALTERNATE_INCLUDE - Consider any alternate pool if set
 */
#define MEMPOOL_ALTERNATE_IGNORE	FALSE
#define MEMPOOL_ALTERNATE_INCLUDE	TRUE

/*
 * REALLOC_TRIM - Don't trim the buffer if we would gain back less
 * than this number of bytes.
 */
#define	REALLOC_TRIM 100

/*
 * BLOCKOVERHEADWORDS - Overhead of memory block header, not including
 * red zone pointer.  Should be for internal use only.
 */
#define BLOCKOVERHEADWORDS (btow(sizeof(blocktype)))

/*
 * Each memory block has a "red zone" at the end.  This is part of the
 * data area of the block;  the block is padded by REDZONEBYTES over
 * what was requested.
 */
#define REDZONE 0xfd0110df	/* Magic pattern */
#define REDZONEBYTES (sizeof(ulong))

/*
 * MEMORY_BLOCK_OVERHEAD - Total overhead of a memory block.  This
 * includes both the block header and the red zone.
 */
#define MEMORY_BLOCK_OVERHEAD (sizeof(blocktype) + REDZONEBYTES)

/*
 * MINDATAxx - Amount of real data space (not including the red zone)
 * in the smallest possible memory block.
 *
 * The value is determined by the size of the free block header.
 */
#define MINDATABYTES (sizeof(freeblocktype))
#define MINDATAWORDS (btow(MINDATABYTES))

/*
 * MINBLOCKWORDS - Size of minimum sized memory block, including all
 * overhead.  This is the size of the smallest fragment.
 */
#define MINBLOCKWORDS (BLOCKOVERHEADWORDS+MINDATAWORDS+btow(REDZONEBYTES))

/*
 * The "size" field of each memory block has the high-order bit set
 * when the block is in use.
 */
#define MEMINUSE 0x80000000
#define FREEDMASK 0x7FFFFFFF

/*
 * Constant truncates to the previous longword boundary.  Probably
 * shouldn't be here.
 */
#define LONGBOUND 0x0FFFFFFFC

/*
 * A known bogus address.  This probably sound't be here either.
 */
#define BAD_ADDRESS (blocktype *)0xFF012345

/*
 * The largest free list created by default.  Noted so that we can
 * decide whether to create a special free list for NV shadow RAM
 * during initialization.
 */
#define LARGEST_DEFAULT_FREELIST_SIZE 262144

/*
 * Watermark defaults
 */
#define MEMPOOL_LOW_THRESHOLD      96*1024
#define MEMPOOL_FRAGMENT_THRESHOLD 32*1024

/*
 * No new network buffers if coreleft is below this threshold.
 * If already defined in a 'machine' include file, don't change it.
 */
#ifndef BUFFERTHRESHOLD
# define BUFFERTHRESHOLD 5000
#endif

/*
 * Support a different buffer threshold for netbooting,
 * possibly platform specific
 */
#ifndef BOOTBUFFERTHRESHOLD
# define BOOTBUFFERTHRESHOLD 5000
#endif

#define MEMDEBUG	1      /* memory debug */


/*
 * Funny increment for defining default free list sizes.  It is designed to
 * exactly match increasing numbers of coalesced minimum sized blocks.
 */
#define FREESIZE(n) (MINDATABYTES+((n)*wtob(MINBLOCKWORDS)))

struct memtrace {
    uchar *addr;
    ulong pc;
};

#define	TRACE_SIZE	16	/* size of history trace */

#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#ifndef __MSG_SYSTEM_C__
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#endif

/*
 * Externs
 */

extern struct memtrace freetrace[TRACE_SIZE], malloctrace[TRACE_SIZE];
extern uchar ixfree, ixmalloc;

/*
 * Prototypes
 */

extern void errmsg_blkinfo (char* cp, blocktype* bp);
extern void errmsg_mtrace(void);
extern void memory_init(void);
extern mempool *mempool_find_by_class(mempool_class class);
extern mempool *mempool_create(mempool *mempool, char *name,
			       regiontype *region, ulong alignment,
			       const ulong *free_list_sizes, 
                               ulong free_list_count,
			       mempool_class class);
extern mempool *mempool_find_by_addr(void *location);
extern void mempool_add_alias_pool(mempool_class class, mempool *mempool);
extern void mempool_delete_alias_pool(mempool_class class);
extern void mempool_add_alternate_pool(mempool *, mempool *);
extern void mempool_add_exception_pool(mempool *, mempool *);
extern void mempool_add_exception_pool_init(mempool *, void (*)(mempool *));
extern boolean mempool_add_region(mempool *mempool, regiontype *region);
extern void mempool_set_low_threshold(mempool_class class, ulong size);
extern void mempool_set_fragment_threshold(mempool_class class, ulong size);
extern boolean mempool_add_free_list(mempool_class class, ulong size);
extern boolean mempool_add_free_list_manual(mempool_class class, ulong size);
extern void *mempool_malloc(mempool_class class, uint size);
extern void *mempool_malloc_aligned(mempool_class class,
				    uint size, uint align);
extern boolean validblock(blocktype *bp, mempool *mp);
extern boolean validblock_gentle(blocktype *bp, mempool *mp);
extern blocktype *block_suspend(mempool *mempool,
				blocktype *block,
				sys_timestamp *suspend_timer);
extern blocktype *get_aligned_block(register int alignment,
				    register int nwords,
				    mempool *mem, ulong caller_pc);
extern blocktype *get_block(register int nwords, mempool *mem,
			    ulong caller_pc);
extern boolean memory_available (ulong size, mempool *mem);
extern void memory_new_owner (void *mem);
extern void memory_specify_owner (void *mem, char *name, pid_t pid,
				  ulong check);
extern ulong mempool_get_free_bytes(mempool_class class, boolean alternate);
extern boolean mempool_is_empty(mempool_class class);
extern boolean mempool_is_low(mempool_class class);
extern ulong mempool_get_used_bytes(mempool_class class, boolean alternate);
extern ulong mempool_get_total_bytes(mempool_class class, boolean alternate);
extern void name_memory(void *mem, char *name);
extern void poison(uchar *address, int count);
extern boolean mem_lockable(void *ptr);
extern boolean validmem_complete(void *mem);
extern boolean validmem_complete_interrupt(void *mem);
extern boolean validpiece(void *ptr, boolean validate_block);
extern boolean linkinblock(void *ptr, void *blockptr);
extern ulong get_block_size(void *memptr, ulong *overheadbytes);
extern void tally_memory_usage(mempool_class class, boolean alternate,
			       ulong *initcount, ulong *schedcount,
			       ulong *deadcount);
extern void checkheaps(void);
extern void checkheaps_for_scheduler(char *process, void *sched_test);
extern void init_checkheaps(void);
extern void thrash_command(parseinfo *csb);
extern void report_malloc_failure(mempool *mem, ulong size, ulong cl, 
				  ulong alignment);
typedef boolean (*memory_pool_walk_rtn_t)(mempool*, blocktype *, void *);
extern  boolean memory_pool_walk(mempool *mem, memory_pool_walk_rtn_t rtn,
				 void *rtn_parameter);
extern void show_memory_allocfail(parseinfo *csb);
extern void show_memory_by_alloc_pc(void);
extern boolean validmem_complete_interrupt(void *);
extern ulong mempool_get_largest_block(mempool_class class);

/*
 * Inlines
 */

static inline void mtrace (
     struct memtrace ta[TRACE_SIZE],
     uchar* ixp,
     uchar *addr, 
     ulong pc)
{
    if (++*ixp == TRACE_SIZE)
	*ixp = 0;
    ta[*ixp].addr = addr;
    ta[*ixp].pc = pc;
    return;
}


/*
 * Convert a data pointer to a block pointer.  Note that all of the below
 * optimizes into a single "lea" instruction.
 */
static inline blocktype *data_to_block(uchar *ptr)
{
    blocktype *block;
    ulong blockdiff;

    blockdiff = (ulong) (block->data_area.data) - (ulong) block;
    block = (blocktype *) (ptr - blockdiff);
    return(block);
}

/*
 * Convert a freeblock pointer to a block pointer.  Note that all of the below
 * optimizes into a single "lea" instruction.
 */
static inline blocktype *freeblock_to_block(freeblocktype *ptr)
{
    blocktype *block;
    ulong blockdiff;

    blockdiff = (ulong) (block->data_area.free_info) - (ulong) block;
    block = (blocktype *) ((ulong) ptr - blockdiff);
    return(block);
}

/*
 * Convert a block pointer to a data pointer.
 */
static inline uchar *block_to_data(blocktype *block)
{
    return(block->data_area.data);
}

/*
 * These are 'quick' mem locks and unlocks for code that knows its memory.
 * This saves an immense amount of CPU time, and is no riskier than a normal
 * pointer dereference.
 */
static inline void inline_mem_lock (void *ptr)
{
    blocktype *bp;
    bp = data_to_block(ptr);
    if (bp->blockmagic != BLOCKMAGIC)
	crashdump(0);		/* Mondo badness */
    bp->refcount++;
}

static inline void inline_mem_unlock_or_free (void *ptr)
{
    blocktype *bp;
    bp = data_to_block(ptr);
    if (bp->blockmagic != BLOCKMAGIC)
	crashdump(0);		/* Mondo badness */
    if (bp->refcount > 1) {
	bp->refcount--;
    } else {
	free(ptr);
    }
}

/* Return a pointer to the red zone.  Don't worry, it optimizes. */

static inline ulong *rz_ptr(blocktype *bp)
{
    ulong *redptr = ((ulong *) (block_to_data(bp) + wtob(bp->size))) - 1;
    return (redptr);
}

static inline void write_red (blocktype *bp)
{
    *rz_ptr(bp) = REDZONE;
    return;
}

/*
 * malloc_word_size - convert byte count to word count for mallocs
 */
static inline ulong malloc_word_size (ulong bytesize)
{
    ulong wordsize;
    wordsize = btow(bytesize);
    wordsize = (wordsize + 1) & (~1); /* Round up to longwords */
    if (wordsize < MINDATAWORDS)
	wordsize = MINDATAWORDS;
    wordsize += btow(REDZONEBYTES);
    return(wordsize);
}


/*
 * prototypes
 */

typedef enum memory_dump_data_type {
    MEMORY_DUMP_DATA_TYPE_NONE,
    MEMORY_DUMP_DATA_TYPE_ASCII,
    MEMORY_DUMP_DATA_TYPE_EBCDIC
} memory_dump_data_type_t;

typedef void (*memory_dump_output_t)(const char *, ...);

extern boolean memory_dump(void    *start,
			   void    *end,
			   boolean  suspend,
			   memory_dump_data_type_t type,
			   memory_dump_output_t    output);

#ifndef GROVEL
/*
 * malloc_block - do the basic work for a malloc
 *
 * Note that some of the code will be optimized out (notably the tests
 * of 'nbytes' and 'alignment' and the resulting code) depending on
 * the caller's parameters.
 *
 * Callers to this inline should call report_malloc_failure if it happens
 * to return NULL.  We don't do it here because it would otherwise
 * whimper before moving to a fallback pool.
 */

extern const char malloc_msg[];
static inline void *malloc_block (mempool *mem, ulong size, ulong cl, 
				 ulong *nbytes, ulong alignment)
{
    blocktype *bp;
    uchar *data;
    ulong wsize;

    if (onintstack()) {
	(*kernel_errmsg)(&msgsym(INVMEMINT, SYS), malloc_msg);
	return(NULL);
    }

    wsize = malloc_word_size(size);
    if (nbytes)
	*nbytes = mem->size;
    if (alignment)
	bp = get_aligned_block(alignment, wsize, mem, cl);
    else
	bp = get_block(wsize, mem, cl);
    if (bp != NULL) {
	if (nbytes)
	    *nbytes = wtob(bp->size + BLOCKOVERHEADWORDS);
	data = block_to_data(bp);
	memset(data, 0, size);		/* zero it */
	write_red(bp);
#ifdef FREEDEBUG
	(*kernel_buginf)("MALLOC_BLOCK: allocating %d words at 0x%x to "
			 "routine at 0x%x, coreleft = %d\n",
	       wsize, data, cl, mem->size);
#endif
    } else {
	data = NULL;
    }
    mtrace(malloctrace, &ixmalloc, data, cl);
    return(data);
}
#endif /* !GROVEL */

/*
Local Variables:
comment-column: 32
End:
*/

#endif		/* __FREE_H__ */
