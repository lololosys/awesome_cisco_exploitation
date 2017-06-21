/* $Id: pool.h,v 3.5.12.4 1996/09/15 18:18:57 srane Exp $
 * $Source: /release/112/cvs/Xsys/os/pool.h,v $
 *------------------------------------------------------------------
 * pool.h - Definitions and prototypes for generic pool manager
 *
 * September 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pool.h,v $
 * Revision 3.5.12.4  1996/09/15  18:18:57  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/07/30  01:09:00  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/03/21  23:21:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.12.1  1996/03/18  21:30:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.5  1996/03/18  23:26:32  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.4  1996/03/03  08:39:50  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.2.2.3  1996/02/08  08:43:35  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/29  19:04:12  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.2.2.1  1995/12/11  19:11:37  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.5  1996/02/15  07:47:07  smackie
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net. (CSCdi49069)
 *
 * Revision 3.4  1996/01/26  22:20:56  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.3  1995/11/29  05:53:55  smackie
 * Move the background processing of pool discards to the central pool
 * manager process. (CSCdi44506)
 *
 * Revision 3.2  1995/11/17  18:50:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  06:32:42  smackie
 * Add placeholders for Arkansas
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __POOL_H__
#define __POOL_H__

#include <ciscolib.h>

/*
 * Pool enums, typedefs and structures
 */

typedef enum pool_item_type_ {
    POOL_ITEM_TYPE_UNKNOWN,
    POOL_ITEM_TYPE_PERMANENT,
    POOL_ITEM_TYPE_TEMPORARY
} pool_item_type;

typedef struct pool_item_status_ {
    pool_item_type type;
    sys_timestamp  age;
} pool_item_status;

typedef  void * (*pool_item_create_t)(pooltype *, pool_item_type type);
typedef  void   (*pool_item_destroy_t)(pooltype *, void *);
typedef  void   (*pool_item_status_t)(pooltype *,
				      void *,
				      pool_item_status *status);
typedef  void * (*pool_item_get_t)(pooltype *);
typedef  void   (*pool_item_ret_t)(pooltype *, void *);
typedef boolean (*pool_item_validate_t)(pooltype *, void *);

typedef  void * (*pool_cache_get_t)(pooltype *);
typedef  void   (*pool_cache_ret_t)(void *);
typedef	 void   (*pool_cache_threshold_t)(pooltype *pool);

typedef struct pool_item_vectors_ {
    pool_item_create_t   create;
    pool_item_destroy_t  destroy;
    pool_item_get_t      get;
    pool_item_ret_t      ret;
    pool_item_status_t   status;
    pool_item_validate_t validate;
} pool_item_vectors;

typedef struct pool_cache_vectors_ {
    pool_cache_get_t       get;
    pool_cache_ret_t       ret;
    pool_cache_threshold_t threshold;
} pool_cache_vectors;

struct pooltype_ {
    pooltype          *_next;           /* next pointer for process queue */
    uint               magic;           /* magic number */
    pool_item_vectors  item;            /* item action vectors */
    queuetype          queue;		/* pool free queue */
    queuetype          discardqueue;    /* queue of discarded items */
    list_element       pool_element;    /* pool queuing element */
    int                group;           /* pool group number */
    int                size;		/* size of items in bytes */
    int                encapsize;       /* size of encapsulation area */
    int                trailersize;     /* size of trailer area */
    int                total;		/* total free items */
    int                free;		/* current free items */
    uint               hit;		/* total hits in queue */
    uint               miss;		/* total misses in queue */
    uint               trims;		/* number of items pruned */
    uint               grows;		/* number of items failures */
    uint               failures;	/* low memory item failures */
    uint               nostorage;	/* number of items created */
    int                perm;            /* total # of permanent items */
    int                defperm;         /* default permanent items */
    int                permtarget;      /* desired # of permanent items */
    int                maxfree;         /* maximum free items allowed */
    int                defmaxfree;      /* default max free items */
    int                minfree;         /* minimum free items */
    int                defminfree;      /* default minimum free items */
    int                init_tmp;	/* initial temporary items */
    uint               flags;           /* pool status flags */
    char               userflags;	/* flags for pool configuration cmds */
    ulong              alignment;       /* pool item alignment */
    mempool           *mempool;         /* IO Memory pool to allocate from */
    chunk_type        *particle_chunk;  /* Pointer to particle chunk */
    chunk_type        *data_chunk;      /* Pointer to packet data chunk */
    char              *name;		/* name of pool */
    hwidbtype         *hwidb;           /* Hardware IDB for private pools */
    struct pooltype_  *fallback;        /* Optional fallback pool */
    uint               servicemask;     /* pool manager service mask */

    /*
     * Item cache support
     */
    pool_cache_vectors   cache_item;      /* Cache vectors */
    void               **cache;           /* pointer to item cache block */
    int                  cache_size;      /* current size of cache */
    int                  cache_max_size;  /* max items in cache array */
    int                  cache_end;       /* current pointer of cache block */
    int                  cache_threshold; /* threshold limit for callback */
    int                  cache_threshold_count; /* threshold trigger count */
};

#define POOLMAGIC 0xdeadb10b
#define POOLVALID(x) ((x) && (x)->magic == POOLMAGIC)

/*
 * pool->flags definitions
 *
 * POOL_DYNAMIC - Set   : The pool sizes are adjusted dynamically by background
 *                        processes and extra buffers can be malloc'd/freed
 *                Clear : The pool sizes are static.
 *
 * POOL_SANITY  - Set   : Items are considered for mempool sanity checks
 *                Clear : No attempt is made to do extra mempool checks.
 *
 * POOL_CACHE   - Set   : A cache is enabled for the pool descriptor.
 *                        This functionality replaces the low-end cache code.
 *                Clear : No cache is in use
 *
 * POOL_PUBLIC_DUPLICATE - Set   : Pool functions should always use the public
 *                                 pools for copying an item from this pool.
 *                         Clear : Try to get another item from the same
 *                                 pool when copying this packet.
 *
 * POOL_INVISIBLE - Set   : Pool is not displayed by show_pool()
 *                  Clear : Pool is displayed by show_pool()
 */

#define POOL_DYNAMIC		0x000000001
#define POOL_SANITY		0x000000002
#define POOL_CACHE		0x000000004
#define POOL_PUBLIC_DUPLICATE	0x000000008
#define POOL_INVISIBLE	        0x000000010

#define POOL_DEFAULT_FLAGS (POOL_DYNAMIC | POOL_SANITY)

/*
 * Pool Service defines
 */

#define POOL_SERVICE_NONE       0x000000000
#define POOL_SERVICE_GROWTH     0x000000001
#define POOL_SERVICE_DISCARD    0x000000002


/*
 * Pool Group defines
 */

#define POOL_GROUP_MAX       512
#define POOL_GROUP_PUBLIC    0
#define POOL_GROUP_INVALID  -1

/*
 * User configuration flags, for pool->userflags
 */

#define POOL_USER_MAX	0x1		/* set if user set max-free */
#define POOL_USER_PERM	0x2		/* set if user set permanent */
#define POOL_USER_MIN	0x4		/* set if user set min-free */

/*
 * Time interbal for pool pruning
 */
#define POOL_PRUNE_TIME   (2*ONEHOUR)

/*
 * Inlines
 */


/*
 * pool_dequeue_item
 *
 * Attempt to dequeue an item. Decrease the free count if we did.
 */

static inline void *pool_dequeue_item (pooltype *pool)
{
    void *item;

    item = dequeue(&pool->queue);
    if (item) {
	pool->hit++;
	pool->free--;
    }
    return(item);
}


/*
 * pool_unqueue_item
 *
 * Attempt to unqueue a specific item.
 */

static inline void pool_unqueue_item (pooltype *pool, void *item)
{
    unqueue(&pool->queue, item);
    pool->free--;
}


/*
 * pool_enqueue_item
 *
 * Attempt to enqueue an item. Increase the free count.
 */

static inline void pool_enqueue_item (pooltype       *pool,
				      void           *item,
				      pool_item_type  type)
{
    /*
     * Permanent to head of queue, temporary to tail of queue
     */
    if (type == POOL_ITEM_TYPE_PERMANENT) 
	requeue(&pool->queue, item);
    else
	enqueue(&pool->queue, item);
    pool->free++;
}


/*
 * pool_cache_available_items
 *
 * Return the number of items in the pools cache
 */

static inline int pool_cache_available_items (pooltype *pool)
{
    return(pool->cache_end);
}


/*
 * pool_dequeue_cache
 *
 * Grabs an item from the pool cache
 */

static inline void *pool_dequeue_cache (pooltype *pool)
{
    if (pool->cache_end) {
	/*
	 * We have items in the cache. Grab one.
	 */
	return(pool->cache[--pool->cache_end]);
    } else {
	/*
	 * Yes, we have no bananas
	 */
	return(NULL);
    }
}

/*
 * pool_enqueue_cache
 *
 * Puys an item back on the pool cache
 */

static inline boolean pool_enqueue_cache (pooltype *pool, void *item)
{
    if (pool->cache_end < pool->cache_size) {
	pool->cache[pool->cache_end++] = item;
	return(TRUE);
    } else {
	return(FALSE);
    }
}


/*
 * pool_low_watermarks
 *
 * Check and see whether we need to signal for growth in a particular
 * buffer pool or not.
 *
 * Returns TRUE if we're running low
 */

static inline boolean pool_low_watermarks (pooltype *pool)
{
    return((pool->flags & POOL_DYNAMIC) && (pool->free < pool->minfree));
}


/*
 * pool_find_by_name
 *
 * Returns a pool pointer given a name
 */

static inline pooltype *pool_find_by_name_internal (list_header *list,
						    char        *name)
{
    pooltype     *pool;
    list_element *element;

    FOR_ALL_DATA_IN_LIST(list, element, pool) {
	/*
	 * Check to see if the size required will fit this pool
	 */
	if (strcasecmp(pool->name, name) == 0)
	    break;
    }

    return(pool);
}

/*
 * Prototypes
 */

extern void pool_request_growth(pooltype *pool);
extern void pool_request_discard(pooltype *pool, void *item);
extern void pool_init(void);
extern void pool_insert(list_header *list, pooltype *pool);
extern void pool_requeue(list_header *list, pooltype *pool);
extern boolean pool_remove(list_header *list, pooltype *pool);
extern void pool_destroy(pooltype *pool);
extern int pool_create_group(void);
extern pooltype *pool_create(char *name, int group, int size,
			     uint flags, mempool *mempool, list_header *list,
			     pool_item_vectors *item);
extern void pool_adjust(pooltype *pool, int mincount, int maxcount,
			int permcount, boolean defaults);
extern pooltype *pool_resize(pooltype *pool, int size, int defminfree,
                             int defmaxfree, int defperm);
extern void pool_periodic(void);
extern void pool_prune(pooltype *pool);
extern boolean pool_create_cache(pooltype *pool,
				 int max_size,
				 pool_cache_vectors *cache_item,
				 int threshold);
extern boolean pool_set_cache_threshold(pooltype              *pool,
					int                    threshold,
					pool_cache_threshold_t item_threshold);
extern void pool_grow_cache(pooltype *pool, int additional_items);
extern void pool_shrink_cache(pooltype *pool, int amount_to_shrink);
extern void pool_adjust_cache(pooltype *pool, int new_size);
extern void pool_destroy_cache(pooltype *pool);
extern void show_particle_statistics(parseinfo *csb);

#endif

