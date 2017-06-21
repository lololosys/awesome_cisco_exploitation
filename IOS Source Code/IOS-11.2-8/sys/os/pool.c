/* $Id: pool.c,v 3.6.20.4 1996/09/15 18:18:56 srane Exp $
 * $Source: /release/112/cvs/Xsys/os/pool.c,v $
 *------------------------------------------------------------------
 * pool.c - Support for generic pool manager
 *
 * September 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pool.c,v $
 * Revision 3.6.20.4  1996/09/15  18:18:56  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/08/28  13:03:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.2  1996/04/05  07:08:17  smackie
 * Clean up any chunky dangleberries in pool_destroy(). (CSCdi53075)
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/03/18  21:30:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/09  05:10:06  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.14.2  1996/03/07  10:19:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.1  1996/02/20  16:41:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/26  22:20:54  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.5  1995/12/13  00:55:16  mohsen
 * CSCdi43576:  Spurious memory access with atm_periodic
 * Fix a typo. Use an and (&&) instead of a comma operator.
 *
 * Revision 3.4  1995/12/07  22:31:08  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/29  05:53:54  smackie
 * Move the background processing of pool discards to the central pool
 * manager process. (CSCdi44506)
 *
 * Revision 3.2  1995/11/17  18:50:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  06:32:39  smackie
 * Add placeholders for Arkansas
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "pool.h"
#include "buffers.h"
#include "buffers_private.h"
#include "free.h"
#include "chunk.h"


/*
 * Local storage
 */

static int            pool_group_count;
static list_header    discardpoolQ;
static watched_queue *servicepoolQ;


/*
 * pool_process
 *
 * Manager process for managing growth of dynamic pools from interrupt
 * level.
 */

static process pool_process (void)
{
    ulong      major, minor;
    void      *item;
    pooltype  *pool;
    leveltype  level;

    process_watch_queue(servicepoolQ, ENABLE, RECURRING);

    while (TRUE) {
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		/*
		 * Dequeue the first pool awaiting tuning
		 */
		while ((pool = process_dequeue(servicepoolQ)) != NULL) {
		    /*
		     * If the pool pointer isn't valid, just keep on plowing.
		     * The pool might have been deleted in the interim.
		     */
		    if (!POOLVALID(pool))
			continue;

		    /*
		     * If there's no grow requests, ignore this pool
		     */
		    if (pool->servicemask == POOL_SERVICE_NONE)
			continue;
		    
		    level = raise_interrupt_level(ALL_DISABLE);

		    /*
		     * If the pool requested some discards, do that now
		     */
		    if (pool->servicemask & POOL_SERVICE_DISCARD) {
			/*
			 * Throw away all the discarded items
			 */
			while ((item = dequeue(&pool->discardqueue))) {
			    (*pool->item.destroy)(pool, item);
			}
		    }

		    /*
		     * If the pool requested some growth, do that now
		     */
		    if (pool->servicemask & POOL_SERVICE_GROWTH) {
			/*
			 * Add items until we reach minfree
			 */
			while (pool->free <= pool->minfree) {
			    item = (*pool->item.create)(pool,
						     POOL_ITEM_TYPE_TEMPORARY);
			    if (!item)
				break;
			    pool_enqueue_item(pool, item,
					      POOL_ITEM_TYPE_TEMPORARY);
			}
		    }

		    /*
		     * Reset the service mask
		     */
		    pool->servicemask = POOL_SERVICE_NONE;
 
		    reset_interrupt_level(level);
		}
		break;

	      default:
		(*kernel_errmsg)(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * pool_request_growth
 *
 * Called by pool clients to signal growth in a specified pool is 
 * required and unable to be performed at the current time (usually
 * because of interrupt restrictions
 */

void pool_request_growth (pooltype *pool)
{
    leveltype level;

    if (!pool)
	return;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }
    
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * If the mask is clear, we have to enqueue the pool for the
     * service process
     */
    if (pool->servicemask == POOL_SERVICE_NONE)
	process_enqueue(servicepoolQ, pool);

    /*
     * Remember the task we want
     */
    pool->servicemask |= POOL_SERVICE_GROWTH;

    reset_interrupt_level(level);
}


/*
 * pool_request_discard
 *
 * Called by pool clients in constrictive environments (like interrupt
 * wrappers) to allow items to be destroyed. 
 */

void pool_request_discard (pooltype *pool, void *item)
{
    leveltype level;

    if (!pool)
	return;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }
    
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Enqueue the item on the pool's discard queue
     */
    enqueue(&pool->discardqueue, item);

    /*
     * If the mask is clear, we have to enqueue the pool for the
     * service process
     */
    if (pool->servicemask == POOL_SERVICE_NONE)
	process_enqueue(servicepoolQ, pool);

    /*
     * Remember the task we want
     */
    pool->servicemask |= POOL_SERVICE_DISCARD;

    reset_interrupt_level(level);
}


/*
 * pool_init
 *
 * Initialize pool support
 */

void pool_init (void)
{
    list_create(&discardpoolQ, 0, "Discarded Pools",
		LIST_FLAGS_INTERRUPT_SAFE);
    pool_group_count = 1;
    reg_add_onemin(pool_periodic, "pool_periodic");
    servicepoolQ = create_watched_queue("Pool Grows", 0, 0);

    process_create(pool_process, "Pool Manager", NORMAL_STACK, PRIO_CRITICAL);
}


/*
 * pool_insert_compare
 *
 * Policy function for pool insertion. Returns TRUE if new should be 
 * inserted before runner
 */

static int pool_insert_compare (void *runner_v, void *new_v)
{
    pooltype *runner, *new;

    runner = runner_v;
    new    = new_v;

    if (runner == NULL)
        return (1);
    if (runner->group > new->group)
        return (1);
    if (runner->size > new->size)
        return (1);
 
    return (-1);
}


/*
 * pool_insert
 *
 * Add a pool descriptor to a pool list in ascending size order
 */

void pool_insert (list_header *list, pooltype *pool)
{
    if ((!list) || (!pool))
	return;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }

    list_insert(list, &pool->pool_element, pool, pool_insert_compare);
}


/*
 * pool_requeue
 *
 * Add a pool descriptor to the head of a pool list, ignoring the
 * group or size
 */

void pool_requeue (list_header *list, pooltype *pool)
{
    if ((!list) || (!pool))
	return;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }

    list_requeue(list, &pool->pool_element, pool);
}


/*
 * pool_remove
 * Remove pool descriptor from a pool queue
 */

boolean pool_remove (list_header *list, pooltype *pool)
{
    /*
     * Some basic sanity checking
     */
    if ((!list) || (!pool))
	return(FALSE);

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return(FALSE);
    }

    list_remove(list, &pool->pool_element, pool);

    return(TRUE);
}

/*
 * pool_destroy
 *
 * Destroy a pool. Destroy its cache, remove it from its
 * queue, free any items on its free list, and insert it into the
 * discardpoolQ to wait for the return of any outstanding items.
 */

void pool_destroy (pooltype *pool)
{
    nexthelper *item;
    leveltype   status;
    
    /*
     * Some basic sanity checking
     */
    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }
    
    pool_destroy_cache(pool);
    pool_remove(pool->pool_element.list, pool);
    
    /*
     * If there's anything in the buffer free list, then free it up.
     */
    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Pull the first item off the list
     */
    if (pool->free) {
	item = pool_dequeue_item(pool);
	
	/*
	 * Loop while we still have items to destroy
	 */
	while (item) {
	    (*pool->item.destroy)(pool, item);
	    item = pool_dequeue_item(pool);
	}
    }
    
    reset_interrupt_level(status);

    if (!pool->total) {
	/*
	 * Destroy any outstanding pool chunks and...
	 */
	if (pool->particle_chunk)
	    chunk_destroy(pool->particle_chunk);
	if (pool->data_chunk)
	    chunk_destroy(pool->data_chunk);

	/*
	 * ...finally kill the pool.
	 */
	free(pool);
    } else {
	pool_requeue(&discardpoolQ, pool);
    }
}


/*
 * pool_create_group
 *
 * Create a new pool group to associate pools with. Returns
 * a group number if successful
 */

int pool_create_group (void)
{
    return(pool_group_count++);
}

/*
 * pool_create
 *
 * Create and initialize a pool
 *
 * Note that size of 0 is now allowed for pools
 */

pooltype *pool_create (char                  *name,
		       int                    group,
		       int                    size,
		       uint                   flags,
		       mempool               *mempool,
		       list_header           *list,
		       pool_item_vectors     *item)
{
    pooltype  *pool;

    /*
     * Perform sanity checks
     */
    if ((name == NULL) ||
	(size < 0) ||
	(group < POOL_GROUP_PUBLIC) || (group > POOL_GROUP_MAX)) {
	return(NULL);
    }

    pool = malloc_named(sizeof(pooltype), "Pool Info");

    if (!pool)
	return(NULL);
    
    /*
     * Initialize non-zero structure elements
     */
    queue_init(&pool->queue, 0);
    queue_init(&pool->discardqueue, 0);

    pool->servicemask = POOL_SERVICE_NONE;

    pool->size = size;
    pool->encapsize  = pool->trailersize = 0;
    pool->flags      = flags;
    pool->mempool    = mempool;
    pool->name       = name;
    pool->magic      = POOLMAGIC;

    /*
     * Initialize pool vectors
     */
    bcopy(item, &pool->item, sizeof(pool_item_vectors));

    /*
     * Set the pool group number
     */
    pool->group = group;
    pool_insert(list, pool);

    return(pool);
}


/*
 * pool_adjust
 * 
 * Adjust pool size parameters.
 */

void pool_adjust (pooltype *pool,
		  int       mincount,
		  int       maxcount,
		  int       permcount,
		  boolean   defaults)
{
    int               i, permdelta;
    nexthelper       *item, *next;
    pool_item_status  status;
    leveltype         level;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }

    level = raise_interrupt_level(ALL_DISABLE);

    pool->minfree = mincount;
    pool->maxfree = maxcount;

    permdelta = permcount - pool->perm;
    pool->permtarget = permcount;

    /*
     * Are we adding or subtracting items?
     */
    if (permdelta < 0) {
	/*
	 * We're subtracting
	 */
	item = pool->queue.qhead;
	for (i = 0; item && i < -permdelta; i++) {
	    next = item->next;
	    /*
	     * Grab the status for this element
	     */
	    (*pool->item.status)(pool, item, &status);
	    if (status.type == POOL_ITEM_TYPE_PERMANENT) {
		pool_unqueue_item(pool, item);
		(*pool->item.destroy)(pool, item);
	    }
	    item = next;
	}
    } else {
	/*
	 * We're adding new buffers
	 */
	for (i = 0; i < permdelta; i++) {
	    item = (*pool->item.create)(pool, POOL_ITEM_TYPE_PERMANENT);
	    if (!item)
		break;
	    pool_enqueue_item(pool, item, POOL_ITEM_TYPE_PERMANENT);
	}
    }

    /*
     * If we're setting these as defaults, do it now
     */
    if (defaults) {
	pool->defminfree = mincount;
	pool->defmaxfree = maxcount;
	pool->defperm    = pool->perm;
    }

    reset_interrupt_level(level);
}

/*
 * pool_resize
 *
 * Change the item size of a pool. *Must* be interrupt protected by
 * caller if any pool client runs at interrupt level.
 */
 
pooltype *pool_resize (pooltype *pool,
                       int       size,
                       int       defminfree,
                       int       defmaxfree,
                       int       defperm)
{
    pooltype  *new;
    int        minfree, maxfree, permtarget;
        
    /*
     * Create our new pool structure
     */
    new = pool_create(pool->name, pool->group, size,
                      pool->flags & (~POOL_CACHE),
                      pool->mempool, pool->pool_element.list, &pool->item);
    if (!new)
        return(NULL);
    
    /*
       * Fill the new pool to look like the original template
       */
    if (pool->userflags & POOL_USER_PERM)
        permtarget = pool->permtarget;
    else
        permtarget = defperm;
    if (pool->userflags & POOL_USER_MAX)
        maxfree = pool->maxfree;
    else
        maxfree = defmaxfree;
    if (pool->userflags & POOL_USER_MIN)
        minfree = pool->minfree;
    else
        minfree = defminfree;
    /*
     * Fill the new pool to look like the original template
     */    
    pool_adjust(new, minfree, maxfree, permtarget, FALSE);
    
    /* Save the defaults */
    new->defminfree = defminfree;
    new->defmaxfree = defmaxfree;
    new->defperm    = defperm;
        
    /*
     * Remember if any user commands were specified so that the new
     * pool NVGENs correctly
     */
    new->userflags = pool->userflags;
    
    /*
     * Throw away the old pool
     */
    pool_destroy(pool);
    
    return(new);
}

/*
 * pool_periodic
 *
 * Utility function that checks to see if pools can be completely
 * destroyed at this point.
 */

void pool_periodic (void)
{
    list_element *element, *nextelement;
    pooltype     *pool;
    
    FOR_ALL_DATA_IN_LIST_W_NEXT(&discardpoolQ, element, nextelement, pool) {
	pool_destroy(pool);
    }
}


/*
 * pool_prune
 *
 */  

void pool_prune (pooltype *pool)
{
    nexthelper       *item, *next;
    pool_item_status  status;
    leveltype         level;
    int               i;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return;
    }

    /*
     * If we're not dynamic, go no further
     */
    if (!(pool->flags & POOL_DYNAMIC))
	return;
	
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Now, check the minfree and create if necessary, otherwise trim
     * oldies. 
     */
    if (pool->free < pool->minfree) {
	for (i = pool->minfree - pool->free; i; i--) {
	    item = (*pool->item.create)(pool, POOL_ITEM_TYPE_TEMPORARY);
	    if (!item)
		break;
	    pool_enqueue_item(pool, item, POOL_ITEM_TYPE_TEMPORARY);
	}
    } else {
	item = pool->queue.qhead;
	while (item != NULL) {
	    next = item->next;
	    if (item == next) {
		pool_unqueue_item(pool, item);
		(*kernel_errmsg)(&msgsym(SELFLINKED, SYS), item, pool->name);
		break;
	    }
	    
	    /*
	     * Grab the status for this element
	     */
	    (*pool->item.status)(pool, item, &status);
	    if ((status.type == POOL_ITEM_TYPE_TEMPORARY) &&
		((pool->free > pool->maxfree) ||
		 (ELAPSED_TIME(status.age) > POOL_PRUNE_TIME))) {
		/*
		 * Time to die.
		 */
		pool_unqueue_item(pool, item);
		(*pool->item.destroy)(pool, item);
		if (pool->free <= pool->minfree)
		    break;
	    }
	    item = next;
	}
    }
    reset_interrupt_level(level);
}


/*
 * Create an pool item cache
 */

boolean pool_create_cache (pooltype           *pool,
			   int                 max_size,
			   pool_cache_vectors *cache_item,
			   int                 threshold)
{
    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return(FALSE);
    }

    pool->cache = malloc_named_fast(max_size * sizeof(void *), "Pool Cache");
    if (!pool->cache)
        return(FALSE);

    pool->cache_max_size = max_size;
    pool->cache_size = 0;
    pool->cache_end  = 0;
    pool->cache_threshold_count = 0;
    pool->cache_threshold = threshold;

    bcopy(cache_item, &pool->cache_item, sizeof(pool_cache_vectors));

    pool->flags |= POOL_CACHE;
    
    return(TRUE);
}


/*
 * pool_set_cache_threshold
 *
 * Add a theshold and a callback for a pool after creation
 */

boolean pool_set_cache_threshold (pooltype               *pool,
				  int                     threshold,
				  pool_cache_threshold_t  item_threshold)
{
    leveltype  level;

    if (!POOLVALID(pool)) {
	(*kernel_errmsg)(&msgsym(BADPOOLMAGIC, SYS), pool);
	return(FALSE);
    }

    if (!(pool->flags & POOL_CACHE))
        return(FALSE);

    level = raise_interrupt_level(ALL_DISABLE);

    pool->cache_threshold      = threshold;
    pool->cache_item.threshold = item_threshold;

    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * pool_grow_cache
 *
 * Expand the cache by a number of items.  
 */
  
void pool_grow_cache (pooltype *pool, int additional_items)
{
    boolean    done;
    leveltype  level;
    void      *item;
    int        cache_size = pool->cache_size;

    /*
     * Check to see that we didn't overflow the array that holds the
     * cache entries.
     */
    if ((additional_items + pool->cache_size) > pool->cache_max_size) {
	/*
	 * We overflowed the cache array, so just peg it.
	 */
	pool->cache_size = pool->cache_max_size;
    } else {
	/*
	 * We didn't overflow the cache array, so just adjust the boundary.
	 */
	pool->cache_size += additional_items;
    }

    /*
     * Fill the cache with the additional buffers.
     */
    done = FALSE;
    level = raise_interrupt_level(ALL_DISABLE);

    while ((cache_size < pool->cache_size) && !done) {
	item = (*pool->cache_item.get)(pool);
	if (item) {
	    pool_enqueue_cache(pool, item);
	    cache_size++;
	} else {
	    done = TRUE;
	}
    }
    reset_interrupt_level(level);
}

/*
 * pool_shrink_cache     
 *
 * Shrink the cache by a number of items.
 */

void pool_shrink_cache (pooltype *pool, int amount_to_shrink)
{
    leveltype  level;
    int        release_count;
    void      *item;

    release_count = amount_to_shrink;

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Clear the cache flag before the buffer handback so that the
     * buffer *really* gets freed an doesn't just end up in the cache
     * again causing an infinite loop.
     */
    pool->flags &= ~POOL_CACHE;
    pool->cache_size -= release_count;
  
    while (release_count && pool->cache_end) {
	release_count--;
	item = pool_dequeue_cache(pool);
	(*pool->cache_item.ret)(item);
    }

    /*
     * Free up newly created discarded buffers and reset the "cached" flag.
     */
    pool_prune(pool);
    pool->flags |= POOL_CACHE;
    
    reset_interrupt_level(level);
}

/*
 * pool_adjust_cache
 *
 * Adjust a pool's item cache size, and populate it (or depopulate it)
 * with permanent items
 */
  
void pool_adjust_cache (pooltype *pool, int new_size)
{
    int amt_to_adjust;

    amt_to_adjust = new_size - pool->cache_size;

    if (amt_to_adjust == 0) return;

    if (amt_to_adjust > 0)
	pool_grow_cache(pool, amt_to_adjust);
    else
	pool_shrink_cache(pool, -amt_to_adjust);
}

/*
 * pool_destroy_cache
 *
 * Destroy a pool's current item cache
 */

void pool_destroy_cache (pooltype *pool)
{
    leveltype level;

    if (!(pool->flags & POOL_CACHE))
        return;

    level = raise_interrupt_level(ALL_DISABLE);

    pool_shrink_cache(pool, pool->cache_size);  

    /*
     * Turn the cache off and delete it.
     */
    pool->flags &= ~POOL_CACHE;
    free(pool->cache);

    pool->cache_item.threshold  = NULL;
    pool->cache_threshold_count = 0;

    reset_interrupt_level(level);
}


