/* $Id: buffers.c,v 3.9.12.12 1996/08/28 13:02:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers.c,v $
 *------------------------------------------------------------------
 * buffers.c -- manage network input/output buffers.
 * This file also contains the enqueue/dequeue fifo list management routines.
 *
 * 21-August-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buffers.c,v $
 * Revision 3.9.12.12  1996/08/28  13:02:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.11  1996/08/27  18:39:12  abaindur
 * CSCdi67226:  IP multicast fastswitching broken (regress)
 * Branch: California_branch
 *  Fix setting of pak->network_start and pak->flags after pak duplication.
 *  Also remove some duplicate pak field settings.
 *
 * Revision 3.9.12.10  1996/08/06  23:34:52  ahh
 * CSCdi64520:  %SYS-2-GETBUFFFAIL: Memory allocation of 67 bytes failed
 * Error message is confusing, make it more obvious that it is a buffer
 * allocation that is failing, not malloc.
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/08/04  06:27:15  thille
 * CSCdi64520:  %SYS-2-GETBUFFFAIL: Memory allocation of 67 bytes failed
 * Branch: California_branch
 * Add "debug buffer failures" command and default to office
 *
 * -Nick
 *
 * Revision 3.9.12.8  1996/07/30  01:08:50  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/07/23  13:26:56  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.12.6  1996/07/10  22:21:37  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/06/04  05:15:52  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.9.12.4  1996/05/28  17:21:42  ppearce
 * CSCdi58789:  buffers.c fails to compile when BUFDEBUG is enabled
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/04/04  09:37:32  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/03/21  23:21:10  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.12.1  1996/03/18  21:28:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.6  1996/03/18  23:25:22  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.5  1996/03/03  08:39:46  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.3.2.4  1996/02/08  08:42:07  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.3  1996/01/16  01:01:44  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.2  1995/12/11  19:10:47  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.1  1995/12/05  06:35:12  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.9  1996/02/13  20:39:03  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/12  02:59:00  jacobt
 * CSCdi48660:  call buffer_validate() if refcount == 0
 *
 * Revision 3.7  1996/01/26  22:20:41  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.6  1995/12/15  09:20:23  ogrady
 * CSCdi39140:  Multicast fastswitching doesnt work on an RSP box.
 * Change pak_duplicate() and pak_copy() and support routines to accept
 * pool->size of 0 used by rsp.  Also include datagram's offset in the
 * size when allocating new buffers for dup, clip, etc.
 *
 * Revision 3.5  1995/11/29  05:53:49  smackie
 * Move the background processing of pool discards to the central pool
 * manager process. (CSCdi44506)
 *
 * Revision 3.4  1995/11/22  06:53:32  kmoberg
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 *
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 * Let's try to get this in one more time...
 *
 * Revision 3.3  1995/11/17  18:44:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:18:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/26  03:44:10  kmoberg
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 * Clean up pak_grow_realign() and dlsw_work().
 *
 * Revision 2.14  1995/10/13  22:27:10  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 * CLSI message header was too big. Have to shift the message if necessary.
 *
 * Revision 2.13  1995/10/12  06:01:59  tli
 * CSCdi42042:  More buffer system housekeeping
 *
 * Revision 2.12  1995/09/21  23:58:46  dmccowan
 * CSCdi40832:  SNA XIDs may need additional encap bytes
 * add pak_dup_grow_realign()
 *
 * Revision 2.11  1995/09/18  07:27:25  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 * Revision 2.10  1995/09/13  23:49:33  smackie
 * Rework buffer pool resize logic to allow the "huge" pool to be resized
 * for a user even if buffers are outstanding. (CSCdi38912)
 *
 * Revision 2.9  1995/09/06  20:15:43  smackie
 * Add code review comments. (CSCdi37152)
 *
 * Revision 2.8  1995/08/12  07:09:28  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.7  1995/08/08  16:48:18  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.6  1995/07/31 20:27:02  lpereira
 * CSCdi37955:  APPN should not add permanent buffers to pool
 *
 * Revision 2.5  1995/07/18  04:16:10  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.4  1995/07/13  22:18:28  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Create a pool that uses the public pools for pak_dup, etc, and put
 * the RSP fast pak structure in it.
 *
 * Revision 2.3  1995/06/28  05:56:34  dkatz
 * CSCdi36468:  MALLOCFAIL at boot up
 * Move malloc failure reporting so that the box doesn't whimper when
 * falling over to an alternate pool.
 *
 * Revision 2.2  1995/06/21  08:56:06  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  21:53:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "pool.h"
#include "particle.h"
#include "buffers.h"
#include "buffers_private.h"
#include "buffers_inline.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#ifdef DEBUG
#define DEBUGCMD(x) x
#else
#define DEBUGCMD(x)
#endif

/*
 * Local Storage
 */
list_header publicpoolQ;		/* list of public pools */
list_header privatepoolQ;		/* list of private pools */
list_header headerpoolQ;		/* list of header pools */

pooltype *fs_particle_pool;		/* fastswitching particle pool */
pooltype *small;                        /* small buffers */
pooltype *middle;                       /* middle sized buffers */
pooltype *big;                          /* big buffers */
pooltype *verybig;                      /* verybig buffers */
pooltype *large;                        /* large buffers */
pooltype *huge;                         /* huge buffers */
pooltype *headerpool;                   /* headers */
pooltype *fspak_pool;			/* static fastswitching paktype hdrs */

static int pool_maximum_size;           /* Largest public pool size */
extern int retting_buffer;		/* nonzero if returning a buffer */

char *packet_header = "*Packet Header*"; /* Packet Header ident */
static char *packet_data   = "*Packet Data*"; /* Packet Data ident */

/*
 * The following is used to record getbuffer() failures for later display.
 */
getbuffer_failures getbuffer_failure_trace[GETBUFFER_FAILURE_RECORD_COUNT];
int getbuffer_failure_trace_number;	/* Next element to fill in */


/*
 * Pool vectors
 */

static void *pak_pool_item_create(pooltype *pool, pool_item_type type);
static void pak_pool_item_destroy(pooltype *pool, void *item);
static void *pak_pool_item_get(pooltype *pool);
static void pak_pool_item_ret(pooltype *pool, void *item);
static void pak_pool_item_status(pooltype *pool, void *item,
			      pool_item_status *status);
static void pak_pool_cache_item_ret(void *item);
static void *pak_pool_cache_item_get(pooltype *pool);

static pool_item_vectors pak_pool_item_vectors = {
    pak_pool_item_create,
    pak_pool_item_destroy,
    pak_pool_item_get,
    pak_pool_item_ret,
    pak_pool_item_status,
    pak_pool_item_validate
};

static pool_cache_vectors pak_pool_cache_vectors = {
    pak_pool_cache_item_get,
    pak_pool_cache_item_ret,
    NULL,
};


/*
 * report_getbuffer_failure
 *
 * Report the fact that a getbuffer() failed.  Record it for later display.
 */
void report_getbuffer_failure (pooltype *pool, ulong size, ulong caller_pc)
{
    getbuffer_failures *failptr;

    /* If buffer_fail_debug, Burp out a rate-limited error message. */
    if (buffer_fail_debug) {
	(*kernel_errmsg)(&msgsym(GETBUFFFAIL, SYS), pool->name, size,
			 caller_pc);
    }

    /* Record it for posterity. */

    failptr = &getbuffer_failure_trace[getbuffer_failure_trace_number];
    GET_TIMESTAMP(failptr->failtime);
    failptr->pool = pool;
    failptr->size = size;
    failptr->calling_pc = caller_pc;

    /* Bump the index and reset if necessary. */
    getbuffer_failure_trace_number = ((getbuffer_failure_trace_number + 1)
				      % GETBUFFER_FAILURE_RECORD_COUNT);
}

/*
 * pak_pool_init
 *
 * Initialize basic buffer pool support at runtime
 */

void pak_pool_init (void)
{
    /*
     * Initialize the element support
     */
    element_init();

    /*
     * Initialize the particle support
     */
    particle_pool_init();

    /*
     * Initialize buffer pool lists
     */
    list_create(&publicpoolQ,  0, "Public Pools",  LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&privatepoolQ, 0, "Private Pools", LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&headerpoolQ,  0, "Header Pools",  LIST_FLAGS_INTERRUPT_SAFE);

    /*
     * Create a free list for packet headers
     */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(paktype));
    
    small  = pak_pool_create("Small", POOL_GROUP_PUBLIC,
				 SMALLDATA, POOL_DEFAULT_FLAGS, NULL);
    middle = pak_pool_create("Middle", POOL_GROUP_PUBLIC,
				 MEDDATA, POOL_DEFAULT_FLAGS, NULL);
    big    = pak_pool_create("Big", POOL_GROUP_PUBLIC,
				 BIGDATA, POOL_DEFAULT_FLAGS, NULL);
    verybig = pak_pool_create("VeryBig", POOL_GROUP_PUBLIC,
				  VERYBIGDATA, POOL_DEFAULT_FLAGS, NULL);
    large  = pak_pool_create("Large", POOL_GROUP_PUBLIC,
				 LARGEDATA, POOL_DEFAULT_FLAGS, NULL);
    huge   = pak_pool_create("Huge", POOL_GROUP_PUBLIC,
				 DEF_HUGEDATA, POOL_DEFAULT_FLAGS, NULL);

    if (!system_loading) {
	/*
	 * Running normal system image:
	 */
	platform_buffer_init(PLATFORM_BUFFER_NORMAL);
    } else {
	/*
	 * Running secondary bootstrap image:
	 */
	platform_buffer_init(PLATFORM_BUFFER_BOOTSTRAP);
    }

    /*
     * Register some functions
     */
    reg_add_onemin(pak_pool_periodic, "pak_pool_periodic");

    buffer_sanity_init();
}


/*
 * pak_pool_create
 *
 * Create a new pool of paktype headers, possibly with data blocks
 */

pooltype *pak_pool_create (char    *name,
			   int      group,
			   int      size,
			   uint     flags,
			   mempool *mempool)
{
    pooltype  *pool;
    list_header *list;

    if (size) {
	if (group == POOL_GROUP_PUBLIC)
	    list = &publicpoolQ;
	else
	    list = &privatepoolQ;
    } else {
	list = &headerpoolQ;
    }

    /*
     * Create our pool structure
     */
    pool = pool_create(name, group, size, flags, mempool,
		       list, &pak_pool_item_vectors);

    if (!pool)
	return(NULL);
    
    /*
     * Was a size specified?
     */
    if (pool->size) {
	/*
	 * Find the mempool for buffers
	 */
	if (!pool->mempool)
	    pool->mempool = mempool_find_by_class(MEMPOOL_CLASS_IOMEM);
	
	/*
	 * If we have a mempool defined, add the new size
	 * to the mempool free list
	 */
	if (pool->mempool) {
	    mempool_add_free_list(pool->mempool->class, 
				  pool->size + BUFFEROVERHEADBYTES);
	}

	/*
	 * Add the pool encap and trailer sizes
	 */
	pool->encapsize   = ENCAPBYTES;
	pool->trailersize = TRAILBYTES;
	
    } else {
	pool->mempool = NULL;
    }

    /*
     * Remember our largest size for speed
     */
    pool_maximum_size = pak_pool_maximum_size();

    return(pool);
}


/*
 * pak_pool_item_create
 *
 * Called to create a new buffer for a paktype pool
 */

static void *pak_pool_item_create (pooltype       *pool,
				   pool_item_type  type)
{
    paktype      *pak;
    pakdata      *data;
    int           size;

    pak = malloc(sizeof(paktype));

    if (!pak)
	return(NULL);

    /*
     * If we've got a memory pool and pool size, we want a data block
     * allocated
     */
    if (pool->mempool && pool->size) {
	size = pool->size + BUFFEROVERHEADBYTES;
	data = mempool_malloc(pool->mempool->class, size);

	if (data) {
	    name_memory(data, packet_data);

	    /*
	     * Add our paktype backpointers
	     */
	    pak->data_area = data->data_area;

	    process_count_getbuffer(size);

	    /*
	     * Set our default pointers
	     */
	    pak->network_start = pak_center(pak);
	    pak->datagramstart = pak_center(pak);
	} else {
	    /*
	     * Hand back our header
	     */
	    free(pak);
	    pak = NULL;
	}
    }

    /*
     * If we've still got a valid paktype pointer, fill in the dots
     */
    if (pak) {
	name_memory(pak, packet_header);

	/*
	 * Add the rest of our information
	 */
	pak->pool = pool;
	if (type == POOL_ITEM_TYPE_PERMANENT) {
	    pak->buffer_flags |= PAK_BUF_PERMANENT;
	    pool->perm++;
	} else {
	    pool->grows++;
	}
	pool->total++;
    }
    return(pak);
}


/*
 * pak_pool_item_destroy
 *
 * Remove a paktype buffer
 */

static void pak_pool_item_destroy (pooltype *pool, void *item)
{
    paktype *pak = item;

    if (onintstack()) {
	/*
	 * If we're on an interrupt stack, enqueue for later destruction
	 */
	pool_request_discard(pool, item);
    } else {
	/*
	 * Adjust some counters
	 */
	if (pak->buffer_flags & PAK_BUF_PERMANENT) {
	    if (!(pool->userflags & POOL_USER_PERM))
		if (pool->permtarget > 0)
		    pool->permtarget--;
	    if (pool->defperm > 0)
		pool->defperm--;
	    pool->perm--;
	} else {
	    pool->trims++;
	}
	pool->total--;

	/*
	 * Free the actual data for the buffer. Remember, data_area
	 * doesn't actually point at the start of the malloc block
	 * as there a weenie backpointer header inserted beforehand.
	 */
	if (pak->data_area) {
	    retting_buffer++;
	    free(pak->data_area - sizeof(pakdata));
	    retting_buffer--;
	}
	
	/*
	 * Hand back the header
	 */
	free(pak);
    }
}


/*
 * pak_pool_item_get
 *
 * Grab a buffer from the freelist
 */

static void *pak_pool_item_get (pooltype *pool)
{
    paktype   *pak = NULL;

    /*
     * If the pool is dynamic, check our sizing
     */
    if (pool->flags & POOL_DYNAMIC) {
	/*
	 * If we're not at interrupt level attempt to top up the free
	 * list before pulling one from the top.
	 */
	if (!onintstack()) {
	    /*
	     * We'll try to top up this pool to minfree
	     */
	    while (pool->free <= pool->minfree) {
		pak = (*pool->item.create)(pool, POOL_ITEM_TYPE_TEMPORARY);
		if (!pak)
		    break;
		pool_enqueue_item(pool, pak, POOL_ITEM_TYPE_TEMPORARY);
	    }
	    
	    /*
	     * Create the buffer we need
	     */
	    pak = (*pool->item.create)(pool, POOL_ITEM_TYPE_TEMPORARY);
	} else {
	    /*
	     * We're at interrupt level. Mark the fact that we got in
	     * here so that process level might top things up
	     */
	    pool_request_growth(pool);
        }
    }
    
    /*
     * See if we've got any buffers left at all
     */
    if (!pak)
	pak = pool_dequeue_item(pool);

    /*
     * We really failed. Chalk up the failure.
     */
    if (!pak)
	pool->failures++;
    
    return(pak);
}


/*
 * pak_pool_item_ret
 *
 */

static void pak_pool_item_ret (pooltype *pool, void *item)
{
    pool_item_type  type;
    paktype        *pak = item;

    type = (pak->buffer_flags & PAK_BUF_PERMANENT) ?
	POOL_ITEM_TYPE_PERMANENT : POOL_ITEM_TYPE_TEMPORARY;

    /*
     * Safety checks
     */
    if (buffer_debug) {
	paktype *test;

	for (test = pool->queue.qhead; test; test = test->next) {
	    if (pak == test) {
		errmsg(&msgsym(ALREADYFREE, SYS), pak, pool->name);
		return;
	    }
	}
	for (test = pool->discardqueue.qhead; test; test = test->next) {
	    if (pak == test) {
		errmsg(&msgsym(ALREADYFREE, SYS), pak, "discard");
		return;
	    }
	}
	if (!buffer_validate(pak))
	    return;
    }

    if ((pool->free >= pool->maxfree) && (type != POOL_ITEM_TYPE_PERMANENT)) {
	(*pool->item.destroy)(pool, item);
    } else {
	pak_reset(pak);
	pak->refcount = 0;
	pool_enqueue_item(pool, item, type);
    }
}


/*
 * pak_pool_item_status
 *
 */

static void pak_pool_item_status (pooltype         *pool,
				      void             *item,
				      pool_item_status *status)
    {
    paktype        *pak = item;

    if (pak->buffer_flags & PAK_BUF_PERMANENT)
	status->type = POOL_ITEM_TYPE_PERMANENT;
    else
	status->type = POOL_ITEM_TYPE_TEMPORARY;
    status->age = pak->gp_timer;
}


/*
 * pak_pool_create_cache
 *
 * Create a new cache pool of paktype headers
 */

boolean pak_pool_create_cache (pooltype *pool, int max_size)
{
    /*
     * Create our pool cache structure
     */
    return(pool_create_cache(pool, max_size,
			     &pak_pool_cache_vectors, 0));
}


/*
 * pak_pool_cache_item_get
 *
 * Get an element from a pool to fill it's cache
 */

static void *pak_pool_cache_item_get (pooltype *pool)
{
    return(pool_getbuffer(pool));
}


/*
 * pak_pool_cache_item_get
 *
 * Get an element from a pool to fill it's cache
 */

static void pak_pool_cache_item_ret (void *item)
{
    return(discard_buffer(item));
}


/*
 * pak_pool_periodic
 *
 * Handle process level growth, trims and discards
 */

void pak_pool_periodic (void)
{
    list_element *element;
    pooltype     *pool;

    /*
     * Tune the public pools
     */
    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	pool_prune(pool);
    }

    /*
     * Tune the private pools
     */
    FOR_ALL_DATA_IN_LIST(&privatepoolQ, element, pool) {
	pool_prune(pool);
    }
}


/* 
 * getbuffer_inline
 *
 * Get a buffer from the specified pool.
 */

static inline paktype *getbuffer_inline (pooltype *pool,
					 ulong     saved_caller_pc,
					 int       size,
					 boolean   fallback)
{
    paktype *pak;
    leveltype status;
    caller_t caller_pc;

    caller_pc = (caller_t) saved_caller_pc;
    /*
     * Check to make sure pool isn't bogus
     */
    if ((!pool) || (pool->size < size))
	return(NULL);
    
    status = raise_interrupt_level(ALL_DISABLE);

    /*
     * If our buffer watermarks are okay for this pool, just
     * yank a buffer from the freelist.
     */
    if (pool_low_watermarks(pool) ||
	((pak = pool_dequeue_item(pool)) == NULL)) {
	/*
	 * We've got to create buffers
	 */
	pool->miss++;
        pak = (*pool->item.get)(pool);
    }

    /*
     * If we failed to grab a buffer from the same pool as the source
     * buffer, try grabbing one from its designated fallback pool, if
     * one is defined.
     */
    if ((fallback) && (!pak) && (pool->fallback)) {
	pool = pool->fallback;
	if (pool_low_watermarks(pool) ||
	    ((pak = pool_dequeue_item(pool)) == NULL)) {
	    /*
	     * We've got to create buffers
	     */
	    pool->miss++;
	    pak = (*pool->item.get)(pool);
	}
    }
    reset_interrupt_level(status);

    if (pak) {
 	buffer_sanity_setcaller_inline(pak, caller_pc);
	if (pak->refcount != 0) {
	    errmsg(&msgsym(BADSHARE, SYS),
		   "pool_getbuffer", pak, pak->refcount);
	    pak = NULL;
	} else {
	    /*
	     * Do a sanity check on the pool that the buffer
	     * thinks that it belongs to.
	     */
	    if (pak->pool != pool) {
		/*
		 * We're toast. Work out if it's a misplaced
		 * buffer or just complete garbage
		 */
		if (POOLVALID(pak->pool)) {
		    errmsg(&msgsym(WRONGPOOL, SYS),
			   pak, pool, pak->pool);
		} else {
		    errmsg(&msgsym(BADPOOL, SYS),
			   pak, pak->pool);
		}
		return(NULL);
	    }
 	    if (!buffer_sanity_check_inline(pak))
 		return(NULL);	    
	    pak_lock(pak);
	    pak->network_start = pak_center(pak);
	    pak->datagramstart = pak_center(pak);
	}
	return(pak);
    }
    report_getbuffer_failure(pool, size, saved_caller_pc);
    return(NULL);
}


/*
 * getbuffer
 * Allocate a network buffer with at least the given number of bytes
 * from the public buffer pools.
 */

paktype *getbuffer (int size)
{
    list_element *element;
    paktype      *pak = NULL;
    pooltype     *pool;

    SAVE_CALLER();

    /*
     * Check for size out of range
     */
    if ((size < 0) || (size > pool_maximum_size)) {
	errmsg(&msgsym(GETBUF, SYS), size);
	return(NULL);
    }
    
    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	pak = getbuffer_inline(pool, caller(), size, FALSE);
	if (pak)
	    break;
    }

    return(pak);
}

/* 
 * pool_getbuffer
 *
 * Get a buffer from the specified pool
 */

paktype *pool_getbuffer (pooltype *pool)
{
    SAVE_CALLER();
    return(getbuffer_inline(pool, caller(), pool->size, TRUE));
}


/*
 * input_getbuffer
 * Allocate a network buffer with at least the given number of bytes
 * from the public buffer pools.
 * Charge the input buffer to the given interface.  Refuse to allocate
 * buffers to input congested interfaces.  We assume this routine is
 * called from interrupt level so we don't need to interlock the interface
 * input queueing count.
 */

paktype *input_getbuffer (int size, hwidbtype *idb)
{
    list_element *element;
    paktype      *pak = NULL;
    pooltype     *pool;

    SAVE_CALLER();

    /*
     * Check for size out of range
     */
    if ((size < 0) || (size > pool_maximum_size)) {
	errmsg(&msgsym(GETBUF, SYS), size);
	return(NULL);
    }

    /*
     * Check for input congestion
     */
    if (idb->input_qcount > idb->input_defqcount) {
	idb->counters.input_drops++;
	return(NULL);
    }

    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	pak = getbuffer_inline(pool, caller(), size, FALSE);
	if (pak)
	    break;
    }

    /*
     * Charge the buffer, or note that we don't have one to give.
     */
    if (pak) {
	/*
	 * Minor kludge alert--fast_rcvidb is only really used in the high
	 * end platforms (the interrupt routines set it up on a per-packet
	 * basis);  however, for all platforms, this field is initialzed
	 * to be equal to idb->firstsw, so it'll do what we want.
	 */
	pak->if_input = idb->fast_rcvidb; /* Get the right software IDB */
	pak->flags |= PAK_INPUTQ;
	idb->input_qcount++;
    } else
	idb->counters.output_nobuffers++;

    return(pak);
}


/*
 * retbuffer_inline
 * Return a network buffer to its free list. Must be called with 
 * interrupts disabled.
 *
 * Note that you most likely want to check pak->retbuffer_alternate
 * before calling this routine. Please see retbuffer() for an example.
 */

static inline void retbuffer_inline (paktype *pak)
{
    pooltype *pool;

    pool = pak->pool;
    if (!POOLVALID(pool)) {
	errmsg(&msgsym(BADPOOL, SYS), pak, pool);
	return;
    }

    if (!buffer_sanity_check_inline(pak))
  	return;

    if ( (pak->next != NULL) &&
	 ((pak->buffer_flags & PAK_BUF_DO_NOT_Q) == 0) ) {
	/* more sanity checking */
	BUFRETCMD(errmsg(&msgsym(INLIST1, SYS), pak, pak->caller_pc));
	errmsg(&msgsym(INLIST, SYS),pak);
	return;
    }
    if (pak->refcount != 1) {
	BUFRETCMD(errmsg(&msgsym(SHARED1, SYS), pak->refcount, pak,
			 pak->caller_pc));
	errmsg(&msgsym(SHARED, SYS), pak->refcount, pak);
	return;
    }

    clear_if_input_inline(pak);

    /*
     * If we've got particles, hand 'em back
     */
    if (pak_has_particles(pak))
	particle_retbuffer(pak);

    if (pool->flags & POOL_CACHE) {
	/*
	 * We have a buffer cache on this pool. Attempt to top it up
	 */
	if (pool->cache_end < pool->cache_size) {
            pak_reset(pak);
	    
	    pool->cache[pool->cache_end++] = pak;
	    
	    /*
	     * If a cache threshold callback has been registered
	     * and we crossed the threshold, make the callback
	     */
	    if (pool->cache_item.threshold &&
		pool->cache_end == pool->cache_threshold) {
		pool->cache_threshold_count++;
		(*pool->cache_item.threshold)(pool);
	    }
	    return;
	}
    }

    (*pool->item.ret)(pool, pak);
}

static inline void kill_fragment_chain (paktype *pak)
{
    paktype *next_frag1, *next_frag2;

    /*
     *  kill all fragments that are chained to this packet.
     */
    next_frag1 = pak->mlp_next_frag;
    while(next_frag1) {
	next_frag2 = next_frag1->mlp_next_frag;
	retbuffer_inline(next_frag1);
	next_frag1 = next_frag2;
    }
    
}


void retbuffer (paktype *pak)
{
    leveltype level;

    if (!pak)
	return;

    level = raise_interrupt_level(ALL_DISABLE);

    kill_fragment_chain(pak);

    retbuffer_inline(pak);
    reset_interrupt_level(level);
}

/*
 * datagram_done
 * Post a datagram "done".  Return a packet if this is the only accessor;
 * otherwise, free a lock on it.
 */

void datagram_done (paktype *pak)
{
    leveltype status;

    if (!pak)
	return;

    /* disable interrupts */
    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount == 1) {
        /*
         * this is the last user of this packet,
         * return the buffer to the buffer pool
         */

	kill_fragment_chain(pak);

        retbuffer_inline(pak);
    } else if (pak->refcount > 1) {
        /*
         * more then one user of this packet. 
         * decrement the use count.
         */
	/*
	 * if (refcount == 1) we will do  check in retbuffer_inline()
	 *
	 */
 	if (!buffer_sanity_check_inline(pak)) {
  	    reset_interrupt_level(status);
  	    return;
	}
	pak->refcount--;
    } else {
        /*
         * bad use count, probably someone trying to free
         * memory that they've already freed. print an error
         * and a stack trace so we can catch them in the act.
         */
	errmsg(&msgsym(BADSHARE, SYS), "datagram_done", pak,
	       pak->refcount);
 	buffer_sanity_check_inline(pak);
    }

    /* re-enable interrupts */
    reset_interrupt_level(status);
}


/*
 * discard buffer
 * Attempt to discard a (possibly permanent) buffer from the buffer pool.
 * 
 * If it's not permanent, give it back to the system via retbuffer()
 * Otherwise, destroy the buffer directly.
 * 
 * Called from the low end drivers to get rid of permanent buffers from 
 * buffer rings when ring changes occur.
 *
 * Called from this module when destroying a pool's buffer cache.
 *
 */

void discard_buffer (paktype *pak)
{
    pooltype *pool;

    pool = pak->pool;

    if (!POOLVALID(pool)) {
	errmsg(&msgsym(BADPOOL, SYS), pak, pool);
	return;
    }

    /*
     * Non permanent buffers just get tossed back
     */
    if (!pak->buffer_flags & PAK_BUF_PERMANENT) {
	retbuffer(pak);
	return;
    }

    clear_if_input(pak);

    (*pool->item.destroy)(pool, pak);
}


/*
 * pak_copy_data_inline
 *
 * Crufty routine to work around the non-safe aspects of our bcopy()
 */

static inline void pak_copy_data_inline (paktype *pak,
					 paktype *newpak,
					 uchar   *start,
					 uchar   *newstart,
					 ushort   size)
{
    /*
     * Move the data
     */
    if ((pak == newpak) && (newstart > start)) {
	uchar *src, *dst;
	/*
	 * Move the data up in memory. bcopy() isn't safe and we
	 * don't currently have a fast rbcopy(). So what follows
	 * an ugly and slow kludge that's at least vaguely 
	 * portable across processors. Thankfully we tend to move
	 * frames down in memory and not up...
	 */
	for (src = start + size, dst = newstart + size;
	     src >= start;
	     src--, dst--)
	    *dst = *src;
    } else {
	/*
	 * bcopy() is safe. Copy the memory to it's new home.
	 */
	bcopy(start, newstart, size);
    }
}

/*
 * pak_net_offset
 *
 * Return the number of bytes the datagram moved upward from
 * pak_center(pak) else 0 if less than or equal.
 */
static inline int pak_net_offset (paktype *pak)
{
    int offset;

    if (pak_has_particles(pak))
        return(0);

    if (pak->network_start >= pak->datagramstart)
        offset = pak->network_start - pak_center(pak);
    else
        offset = pak->datagramstart - pak_center(pak);

    return( (offset > 0) ? offset : 0);
}


/*
 * pak_adjust_pointer
 *
 * Applies an offset to a packet data pointer
 */
 
static inline uchar *pak_adjust_pointer (uchar *ptr, int offset)
{
    return ((ptr) ? (ptr + offset) : NULL);
}
 

/*
 * pak_get_particle_offset
 *
 * Calculates a base offset in a particle list
 */

static void pak_get_particle_offset (particletype *particle,
				     uchar        *ptr,
				     uint         *offset_value)
{
    int size;

    if (!ptr) {
	*offset_value = -1;
	return;
    }

    *offset_value = 0;

    while (particle) {
	size = particle->data_bytes;
	if ((ptr >= particle->data_start) &&
	    (ptr < (particle->data_start + size))) {
	    *offset_value += (uint)(ptr - particle->data_start);
	    return;
	}
	*offset_value  += size;
	particle        = particle->next;
    }
}

/*
 * pak_get_paktype_offset
 *
 * Calculates a base offset in a paktype
 */

static inline void pak_get_paktype_offset (paktype *pak,
					   uchar   *ptr,
					   uint    *offset_value)
{
    *offset_value = (ptr) ? (int)ptr - (int)pak->data_area : -1;
}


/*
 * pak_get_offset_table_inline
 *
 * Extracts an offset table based relatively on the paktype structure
 * and stores it in the supplied offset table. This is used for
 * generating generic offset translations
 */

static inline void pak_get_offset_table_inline (paktype        *pak,
						paktype_offset *offset)
{
    particletype *particle = pak_get_first_particle(pak);

    if (particle) {
	/*
	 * Calculate the offsets from our pointers
	 */
	pak_get_particle_offset(particle, pak->datagramstart,
				&offset->datagramoffset);
	pak_get_particle_offset(particle, pak->mac_start,
				&offset->mac_offset);
	pak_get_particle_offset(particle, pak->addr_start,
				&offset->addr_offset);
	pak_get_particle_offset(particle, pak->rif_start,
				&offset->rif_offset);
	pak_get_particle_offset(particle, pak->info_start,
				&offset->info_offset);
	pak_get_particle_offset(particle, pak->network_start,
				&offset->network_offset);
	pak_get_particle_offset(particle, pak->transport_start,
				&offset->transport_offset);
	pak_get_particle_offset(particle, pak->session_start,
				&offset->session_offset);
	pak_get_particle_offset(particle, pak->presentation_start,
				&offset->presentation_offset);
	pak_get_particle_offset(particle, pak->application_start,
				&offset->application_offset);
    } else {
	pak_get_paktype_offset(pak, pak->datagramstart,
			       &offset->datagramoffset);
	pak_get_paktype_offset(pak, pak->mac_start,
			       &offset->mac_offset);
	pak_get_paktype_offset(pak, pak->addr_start,
			       &offset->addr_offset);
	pak_get_paktype_offset(pak, pak->rif_start,
			       &offset->rif_offset);
	pak_get_paktype_offset(pak, pak->info_start,
			       &offset->info_offset);
	pak_get_paktype_offset(pak, pak->network_start,
			       &offset->network_offset);
	pak_get_paktype_offset(pak, pak->transport_start,
			       &offset->transport_offset);
	pak_get_paktype_offset(pak, pak->session_start,
			       &offset->session_offset);
	pak_get_paktype_offset(pak, pak->presentation_start,
			       &offset->presentation_offset);
	pak_get_paktype_offset(pak, pak->application_start,
			       &offset->application_offset);
    }
}

/*
 * pak_set_particle_offset
 *
 * Sets a pointer into a particle chain given a base offset
 */

static void pak_set_particle_offset (particletype  *particle,
				     uchar        **ptr,
				     uint           offset_value,
				     int            delta)
{
    ushort size;

    if (offset_value == PARTICLE_NO_OFFSET) {
	*ptr = NULL;
	return;
    }

    offset_value += delta;

    while (particle) {
	size = particle->data_bytes;
	*ptr = particle->data_start;
	if (offset_value <= size) {
	    *ptr += offset_value;
	    return;
	}
	offset_value -= size;
	particle      = particle->next;
    }
}

/*
 * pak_set_paktype_offset
 *
 * Sets a pointer into a paktype given a base offset
 */

static void pak_set_paktype_offset (paktype  *pak,
				    uchar   **ptr,
				    uint      offset_value,
				    int       delta)
{
    *ptr = (offset_value != PARTICLE_NO_OFFSET) ?
	pak->data_area + offset_value + delta : NULL;
}


/*
 * pak_set_offset_table_inline
 *
 * Given a packet and a start particle, apply a table offset to the pointers
 * in the paktype. An additional delta to the table can be specified.
 */

static inline void pak_set_offset_table_inline (paktype        *pak,
						paktype_offset *offset,
						int             delta)
{
    particletype *particle = pak_get_first_particle(pak);

    if (particle) {
	/*
	 * Calculate the offsets from our pointers
	 */
	pak_set_particle_offset(particle, &pak->datagramstart,
				offset->datagramoffset, delta);
	pak_set_particle_offset(particle, &pak->mac_start,
				offset->mac_offset, delta);
	pak_set_particle_offset(particle, &pak->addr_start,
				offset->addr_offset, delta);
	pak_set_particle_offset(particle, &pak->rif_start,
				offset->rif_offset, delta);
	pak_set_particle_offset(particle, &pak->info_start,
				offset->info_offset, delta);
	pak_set_particle_offset(particle, &pak->network_start,
				offset->network_offset, delta);
	pak_set_particle_offset(particle, &pak->transport_start,
				offset->transport_offset, delta);
	pak_set_particle_offset(particle, &pak->session_start,
				offset->session_offset, delta);
	pak_set_particle_offset(particle, &pak->presentation_start,
				offset->presentation_offset, delta);
	pak_set_particle_offset(particle, &pak->application_start,
				offset->application_offset, delta);
    } else {
	pak_set_paktype_offset(pak, &pak->datagramstart,
			       offset->datagramoffset, delta);
	pak_set_paktype_offset(pak, &pak->mac_start,
			       offset->mac_offset, delta);
	pak_set_paktype_offset(pak, &pak->addr_start,
			       offset->addr_offset, delta);
	pak_set_paktype_offset(pak, &pak->rif_start,
			       offset->rif_offset, delta);
	pak_set_paktype_offset(pak, &pak->info_start,
			       offset->info_offset, delta);
	pak_set_paktype_offset(pak, &pak->network_start,
			       offset->network_offset, delta);
	pak_set_paktype_offset(pak, &pak->transport_start,
			       offset->transport_offset, delta);
	pak_set_paktype_offset(pak, &pak->session_start,
			       offset->session_offset, delta);
	pak_set_paktype_offset(pak, &pak->presentation_start,
			       offset->presentation_offset, delta);
	pak_set_paktype_offset(pak, &pak->application_start,
			       offset->application_offset, delta);
    }
}


/*
 * pak_copy_header
 *
 * Copy the contents of a paktype header to a new header, preserving
 * the constant fields
 */
 
static void pak_copy_header (paktype *pak, paktype *newpak)
{
    uchar          *new_data_area;
    pooltype       *new_pool;
    buffer_flags_t  new_buffer_flags;
    queuetype       new_particlequeue;
    uchar          *new_buffer_header;

    /*
     * Save some context from newpak
     */
    new_buffer_flags  = newpak->buffer_flags;  /* Save buffer flags */
    new_pool          = newpak->pool;          /* Save pool pointer */
    new_data_area     = newpak->data_area;	   /* Save data pointer */
    new_buffer_header = newpak->buffer_header; /* Save buffer header */
    new_particlequeue = newpak->particlequeue; /* Save particle queue */
    
    /*
     * Copy the header
     */
    bcopy(pak, newpak, sizeof(paktype));
    
    /*
     * Restore the new context
     */
    newpak->buffer_flags  = new_buffer_flags;  /* Restore buffer flags */
    newpak->pool          = new_pool;          /* Restore pool pointer */
    newpak->data_area     = new_data_area;
    newpak->buffer_header = new_buffer_header; /* Restore buffer header */
    newpak->particlequeue = new_particlequeue; /* Restore particle queue */
    
    /*
     * Reset the basic variables
     */
    newpak->next          = NULL;	           /* Ensure no link to next */
    newpak->flags        &= ~PAK_INPUTQ;	   /* Clear PAK_INPUTQ */
    newpak->refcount      = 1;
}
 


/*
 * pak_copy_contiguous_to_contiguous
 *
 * It slices, it dices, etc.
 *
 * This function is the primary base of all of the contiguous packet
 * data copying routines. The parameters are
 *
 *  pak      - source paktype
 *  newpak   - destination paktype (may be the same as the source)
 *  start    - pointer to the start of the source data to copy
 *  newstart - pointer to the destination of the data
 *  size     - amount of data to copy. Must be sanity checked.
 *
 * This routine will move all the data and realign the pointers in
 * the destination paktype to account for any shifts in alignment.
 */

static void pak_copy_contiguous_to_contiguous (paktype *pak,
					       paktype *newpak,
					       uchar   *start,
					       uchar   *newstart,
					       ushort   size)
{
    uint            end;
    int             delta;
    caller_t caller_pc;
     
    buffer_sanity_savecaller_inline(&caller_pc);

    /*
     * Make sure the i's are dotted and the t's crossed
     */
    if (pak_has_particles(pak)) {
	errmsg(&msgsym(BADBUFFER, SYS), "scattered", "contiguous src",
	       pak, pak->pool);
	return;
    }

    if (pak_has_particles(newpak)) {
	errmsg(&msgsym(BADBUFFER, SYS), "scattered", "contiguous dst",
	       newpak, newpak->pool);
	return;
    }

    /*
     * zero pool size used on RSP for pool of variable sizes
     */
    if (pak->pool->size) {
	end = (uint)pak_center(pak) + pak->pool->size + pak->pool->trailersize; 
	if (((uint)start + size) > end)
	    size = min(size, end - (uint)start);
    }

    if (newpak->pool->size) {
	end = (uint)pak_center(newpak) + newpak->pool->size +
	      newpak->pool->trailersize; 
	if (((uint)newstart + size) > end)
	    size = min(size, end - (uint)newstart);
    }

    /*
     * If it's a different buffer, copy the header
     */
    if (pak != newpak)
	pak_copy_header(pak, newpak);

    /*
     * We will have read the source packet, and will also have written the
     * destination packet. Check each packet to see whether or not it is
     * using the dirty flag. If it is using the dirty flag, mark it as
     * dirty so that the device driver will flush the data out of the
     * processor cache before giving the packet to a DMA device.
     */
    pak_set_dirty(pak);
    pak_set_dirty(newpak);

    /*
     * Do the actual grunt work of copying the data
     */
    pak_copy_data_inline(pak, newpak, start, newstart, size);
    
    /*
     * Work out what the relative offset is now
     */
    delta = newstart - start;
    
    /*
     * Rebuild all the pointers into the packet
     */
    newpak->datagramstart = pak_adjust_pointer(pak->datagramstart, delta);
    newpak->mac_start     = pak_adjust_pointer(pak->mac_start, delta);
    newpak->addr_start    = pak_adjust_pointer(pak->addr_start, delta);
    newpak->rif_start     = pak_adjust_pointer(pak->rif_start, delta);
    newpak->info_start    = pak_adjust_pointer(pak->info_start, delta);
    newpak->network_start = pak_adjust_pointer(pak->network_start, delta);
    newpak->transport_start = pak_adjust_pointer(pak->transport_start,
						 delta);
    newpak->session_start = pak_adjust_pointer(pak->session_start, delta);
    newpak->presentation_start =pak_adjust_pointer(pak->presentation_start,
						   delta);
    newpak->application_start = pak_adjust_pointer(pak->application_start,
						   delta);

    buffer_sanity_setcaller_inline(newpak, caller_pc);
}


/*
 * pak_copy_scatter_to_contiguous
 *
 * This function is the primary base of all of the scattered packet
 * data copying routines. The parameters are
 *
 *  pak      - source paktype
 *  newpak   - destination paktype (may be the same as the source)
 *  start    - pointer to the start of the source data to copy
 *  newstart - pointer to the destination of the data
 *  size     - amount of data to copy. Must be sanity checked.
 *
 * This routine will move all the data and realign the pointers in
 * the destination paktype to account for any shifts in alignment.
 */

void pak_copy_scatter_to_contiguous (paktype *pak,
				     paktype *newpak,
				     uchar   *start,
				     uchar   *newstart,
				     ushort   size)
{
    particletype   *particle;
    uint            end;
    int             delta;
    paktype_offset  offset;
    ushort          copysize;
    caller_t caller_pc;
    
    buffer_sanity_savecaller_inline(&caller_pc);

    /*
     * Make sure the i's are dotted and the t's crossed
     */
    if (!pak_has_particles(pak)) {
	errmsg(&msgsym(BADBUFFER, SYS), "contiguous", "scattered src",
	       pak, pak->pool);
	return;
    }

    if (pak_has_particles(newpak)) {
	errmsg(&msgsym(BADBUFFER, SYS), "scattered", "contiguous dst",
	       newpak, newpak->pool);
	return;
    }

    /*
     * Make sure we're copying within bounds
     */
    if (newpak->pool->size) {
	end = (uint)pak_center(newpak) + newpak->pool->size; 
	if (((uint)newstart + size) > end)
	    size = min(size, end - (uint)newstart);
    }

    /*
     * If it's a different buffer, copy the header
     */
    if (pak != newpak)
	pak_copy_header(pak, newpak);

    /*
     * We will have read the source packet, and will also have written the
     * destination packet. Check each packet to see whether or not it is
     * using the dirty flag. If it is using the dirty flag, mark it as
     * dirty so that the device driver will flush the data out of the
     * processor cache before giving the packet to a DMA device.
     */
    pak_set_dirty(pak);
    pak_set_dirty(newpak);

    /*
     * Do the actual grunt work of copying the data
     */
    pak_get_offset_table_inline(pak, &offset);

    /*
     * Work out the delta
     */
    delta = (newstart - newpak->data_area) - pak_get_leading_size(pak, start);
    
    particle = pak_get_first_particle(pak);
    while (particle) {
	/*
	 * Size is the minimum between what's left to copy and what's
	 * left to the end of the current particle
	 */
	copysize = particle->data_bytes - (start - particle->data_start);
	copysize = min(copysize, size);
	
	/*
	 * Copy the data across
	 */
	pak_copy_data_inline(pak, newpak, start, newstart, copysize);
	
	/*
	 * Update our pointers and sizes
	 */
	newstart += copysize;
	size     -= copysize;
	particle  = particle->next;
	if (particle)
	    start = particle->data_start;
    }

    pak_set_offset_table_inline(newpak, &offset, delta);

    buffer_sanity_setcaller_inline(newpak, caller_pc);
}


/*
 * pak_copy
 *
 * This copies one buffer to another without realigning the data
 *
 *
 * NOTE 
 * This routine will silently truncate the copy if the dest buffer is
 * not big enough.  Also note this routine increases the specified
 * size of the data to be copied by the offset of the datagram within
 * the source buffer in order to preserve the offsets.
 *
 * If the data offset in the source buffer is large (as with RSP
 * fastswitch rx offsets) and the destination buffer is only big enough
 * to hold the specified "size" argument the copy could get silently
 * truncated.  While there is a lot of extra slop in the true size of
 * the buffers it may not always guarantee success.
 * the dest buffer should be large enough to hold "size" +
 * (pak->network_start - pak_center(pak))
 *
 * Callers may be better off using pak_copy_and_recenter(), or
 * pak_clip() or pak_duplicate() which will alloc the proper size
 * buffer for the copy.
 */

void pak_copy (paktype *pak, paktype *newpak, int size)
{
    /*
     * include bytes up to The Line and data offset from it in to size
     */
    size += (ENCAPBYTES + pak_net_offset(pak));

    /*
     * Move the data
     */
    pak_copy_contiguous_to_contiguous(pak, newpak,
				      pak->data_area, newpak->data_area,
				      size);
}


/*
 * pak_copy_and_recenter
 *
 */

void pak_copy_and_recenter (paktype *pak, paktype *newpak, int size)
{
    int    offset;
    uchar *start, *newstart;

    /*
     * How much has network_start deviated from The Line
     */
    offset = pak->network_start - pak_center(pak);

    /*
     * Calculate our start and finish
     */
    if (offset > 0) {
	/*
	 * Adjust the start and newstart pointers so that the data
	 * gets moved down in memory to realign the network_start.
	 */
	start    = pak->data_area + offset;
	newstart = newpak->data_area;
    } else {
	/*
	 * Adjust the start and newstart pointers so that the data
	 * gets moved up in memory to realign the network_start.
	 *
	 * Note the *subtraction* of offset. As offset is negative,
	 * this is actually an addition. Arithmetic 101.
	 */
	start    = pak->data_area;
	newstart = newpak->data_area - offset;
    }

    size += (ENCAPBYTES + offset);

    /*
     * Move the data
     */
    pak_copy_contiguous_to_contiguous(pak, newpak, start, newstart, size);
}    


/*
 * pak_duplicate
 * Produce a duplicate of a packet
 */

paktype *pak_duplicate (paktype *pak)
{
    paktype  *newp;
    pooltype *pool = pak->pool;
    caller_t caller_pc;
    
    buffer_sanity_savecaller_inline(&caller_pc);

    if (POOLVALID(pool)) {
        if (pool->flags & POOL_PUBLIC_DUPLICATE) {
          newp = getbuffer(pak->datagramsize + pak_net_offset(pak));
        } else {
	  newp = pool_getbuffer(pool);
        }
        if (newp) {
            pak_copy(pak, newp, pak_get_pool_size(pak));
 	    buffer_sanity_setcaller_inline(newp, caller_pc);
	}
    } else {
	errmsg(&msgsym(BADPOOL, SYS), pak, pool);
	newp = NULL;
    }

    return(newp);
}

/*
 * pak_realign
 *
 * Snaps a frame back to the alignment given. If there's no alignment
 * given, the code assumes the default. The alignment is the
 * start of the network header, or, if you want to be nostalgic,
 * it's The Line. If there isn't enough room to do this, the function
 * will return FALSE, otherwise TRUE.
 *
 * This routine should be used in preference to homebrew confections
 * which attempt to do the same thing - i.e. this routine will get
 * fixed if things change - your's might not be so lucky. 
 */

boolean pak_realign (paktype *pak, uchar *alignment)
{
    int offset;
    uchar *new_datagramstart;

    /*
     * We're using a default
     */
    if (!alignment)
	alignment = pak_center(pak);

    /*
     * We need datagramstart and network_start to be set. If they aren't,
     * bale and bitch.
     */
    if ((!pak->datagramstart) || (!pak->network_start))
	return(FALSE);

    /*
     * Work out how far off we are
     */
    offset = alignment - pak->network_start;

    /*
     * It's bang on. Leave the frame alone
     */
    if (!offset)
	return (TRUE);
    
    /*
     * Some more sanity checking. datagramstart should be before
     * network_start for the following logic to make sense.
     */
    if (pak->network_start < pak->datagramstart)
	return(FALSE);

    new_datagramstart = (uchar *)(pak->datagramstart + offset);

    /*
     * Move the data
     */
    pak_copy_contiguous_to_contiguous(pak, pak,
				      pak->datagramstart, new_datagramstart,
				      pak->datagramsize);

    return(TRUE);
}

/*
 * pak_clip
 * pak_duplicate that copies a specified number of bytes from the original
 * packet into a new packet. A best fit pak size is determined depending on
 * the copy_size parameter.
 *
 * This function was intended to allow a subset of the data in larger paks to
 * be copied into smaller paks to save time and memory. However, this function
 * allow for smaller paks to be copied into larger paks, which will copy
 * garbage data from the contigous memory after the small pak into the larger
 * pak.
 */

paktype *pak_clip (paktype *pak, int copy_size)
{
    paktype *newp;

    if ((copy_size < 0) || (copy_size > pool_maximum_size)) {
	errmsg(&msgsym(GETBUF, SYS), copy_size);
	return(NULL);
    }

    newp = getbuffer(copy_size + pak_net_offset(pak));

    if (newp) {
        pak_copy(pak, newp, copy_size);
    }
    else {
        newp = NULL;
    }
    
    return(newp);
}

/*
 * pak_grow
 * allow a packet to be grown.  If there is room in the current packet,
 * return it, else get a bigger one, copy the data, and call datagramdone
 * on the old.  If we can't allocate a bigger one, return NULL, but don't
 * dispose of the old one.
 */
paktype *pak_grow (paktype *pak, int oldsize, int size)
{
    paktype *newp;

    size += pak_net_offset(pak);

    if (pak_get_pool_size(pak) >= size) {
	return(pak);
    } else {
	newp = getbuffer(size);
	if (newp) {
	    pak_copy(pak, newp, oldsize);
	    datagram_done(pak);
	    return(newp);
	}
    }
    return(NULL);
}


/*
 * pak_dup_grow_realign
 *
 * Combination of pak_duplicate, pak_grow, and pak_realign.
 *
 * allocates a new buffer, copies copy_size bytes from the old pak
 * to the new pak, leaving at least hdrbytes bytes before datagramstart.
 * new pak will be best fit size... clipped or grown if necessary.
 */

paktype *pak_dup_grow_realign (paktype *pak, int copy_size, int hdrbytes)
{
    paktype *newp;
    int old_encapbytes;
    int new_encapsize;

    if (!pak)
       return(NULL);

    old_encapbytes = pak->network_start - pak->datagramstart;

    if (old_encapbytes + hdrbytes < ENCAPBYTES) {
       new_encapsize = ENCAPBYTES;
    } else {
       new_encapsize = hdrbytes + old_encapbytes;
       new_encapsize += new_encapsize & 0x03;   /* ensure longword alignment*/
    }

    newp = getbuffer(copy_size + new_encapsize - ENCAPBYTES);

    if (!newp)
       return(NULL);

    pak_copy_contiguous_to_contiguous(pak, newp,
				      pak->datagramstart,
				      (newp->data_area + 
				       new_encapsize - old_encapbytes),
				      copy_size);

    return(newp);
}


/*
 * pak_grow_realign
 *
 * Combination of pak_grow, and pak_realign.
 *
 * Allocates a new buffer, copies copy_size bytes from the old pak
 * to the new pak, leaving at least hdrbytes bytes before datagramstart.
 * new pak will be best fit size... clipped or grown if necessary.
 */

paktype *pak_grow_realign (paktype *pak, int copy_size, int hdrbytes)
{
    paktype *newp;
    int old_encapbytes;
    int new_encapsize;

    if (!pak)
       return(NULL);

    old_encapbytes = pak->network_start - pak->datagramstart;

    if (old_encapbytes + hdrbytes < ENCAPBYTES) {
       return(pak);   /* We are ok use the same pak */
    } else {
       new_encapsize = hdrbytes + old_encapbytes;
       new_encapsize += new_encapsize & 0x03;   /* ensure longword alignment*/
    }

    newp = getbuffer(copy_size + new_encapsize - ENCAPBYTES);

    if (!newp)
	return(NULL);
    
    pak_copy_contiguous_to_contiguous(pak, newp,
				      pak->datagramstart, 
				      newp->data_area +
				      new_encapsize - old_encapbytes,
				      copy_size);
    
    datagram_done(pak);
    return(newp);
}

/*
 * pak_coalesce
 *
 * Coalesce a scatter packet into a contiguous one. Align network_start
 * to pak_center() on the copy.
 */

paktype *pak_coalesce (paktype *pak, paktype *newpak)
{
    pooltype *pool = pak->pool;
    int       datagramstart_offset, network_start_offset, offset;
    caller_t caller_pc;
     
    buffer_sanity_savecaller_inline(&caller_pc);

    if ((!pak) || (!newpak))
        return(NULL);

    if (!POOLVALID(pool)) {
	errmsg(&msgsym(BADPOOL, SYS), pak, pool);
	return(NULL);
    }

    if (!POOLVALID(newpak->pool)) {
        errmsg(&msgsym(BADPOOL, SYS), newpak, newpak->pool);
        return(NULL);
    }

    /*
     * Work out the offset of datagramstart and network_start. This
     * is effectively pak->encsize but we can't trust that.
     */
    datagramstart_offset = pak_get_leading_size(pak, pak->datagramstart);
    network_start_offset = pak_get_leading_size(pak, pak->network_start);
    offset = network_start_offset - datagramstart_offset;

    /*
     * Set the new start
     */
    newpak->datagramstart = pak_center(newpak) - offset;

    pak_copy_scatter_to_contiguous(pak, newpak,
				   pak->datagramstart,
				   newpak->datagramstart,
				   pak->datagramsize);

    buffer_sanity_setcaller_inline(newpak, caller_pc);
    return(newpak);
}

/*
 * pak_clone :
 * Clone a particle based packet. Note, the input packet must be in
 * particles, and it is assumed that this routine is called at interrupt level
 */

paktype *pak_clone (paktype *oldpak, paktype *newpak)
{
    particletype   *mp, *temp;

    /*
     * Make sure we are passed in a scattered packet
     */
    if (!pak_has_particles(oldpak)) {
	errmsg(&msgsym(BADBUFFER, SYS), "contiguous", "scattered src",
	       oldpak, oldpak->pool);
	return(NULL);
    }

    /*
     * Make sure we were handed in a new paktype header and there is
     * enought clones to clone it
     */
    if (!newpak || (cloneQ.count < pak_particle_count(oldpak))) {
	return(NULL);
    }

    /*
     * Setup the fields in the new paktype header
     */
    newpak->datagramsize  = 0;
    newpak->if_input      = oldpak->if_input;
    newpak->linktype      = oldpak->linktype;
    newpak->acct_proto    = oldpak->acct_proto;
    newpak->network_start = oldpak->network_start;

    /*
     * Clone the particles
     */
    for (mp=oldpak->particlequeue.qhead; mp; mp = mp->next) {
	temp = particle_clone(mp);
	particle_enqueue_inline(newpak, temp);
    }
    return(newpak);
}


/*
 * pak_get_pool_size
 * Return the maximum size of a packet.
 */

int pak_get_pool_size (paktype *pak)
{
    if (POOLVALID(pak->pool)) {
	if (pak->pool->size != 0)
	    return(pak->pool->size);
	else
	    return(pak->datagramsize);
    } else {
	errmsg(&msgsym(BADPOOL, SYS), pak, pak->pool);
	return(-1);
    }
}

/*
 * pak_pool_maximum_size
 *
 * Returns the size of the largest public pool
 */

int pak_pool_maximum_size (void)
{
    pooltype *pool;

    pool = LIST_TAIL_DATA(&publicpoolQ);
    if (pool)
	return(pool->size);
    else
	return(0);
}

/*
 * pak_pool_find_by_size
 * Return the public pool pointer for the specified packet size:
 */

pooltype *pak_pool_find_by_size (int size)
{
    list_element *element;
    pooltype     *pool;

    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	/*
	 * Check to see if the size required will fit this pool
	 */
	if (size <= pool->size)
	    break;
    }

    return(pool);
}

/*
 * pak_pool_find_by_name
 *
 * Returns a pool pointer given a buffer pool name
 */

pooltype *pak_pool_find_by_name (char *name)
{
    pooltype *pool;

    if (!name)
	return(NULL);

    pool = pool_find_by_name_internal(&publicpoolQ, name);
    if (!pool)
	pool = pool_find_by_name_internal(&privatepoolQ, name);

    return(pool);
}

/*
 * clear_if_input
 * This routine must be called whenever pak->if_input is being cleared.
 * It takes care of input queueing accounting.
 */

void clear_if_input (paktype *ptr)
{

    clear_if_input_inline(ptr);
}

/*
 * set_if_input
 * This routine must be called whenever pak->if_input is being set.
 * It takes care of input queueing accounting.
 */

void set_if_input (paktype *ptr, idbtype *idb)
{
    set_if_input_inline(ptr, idb);
}

/*
 * change_if_input
 *
 * Reallocate a packet from one input queue to another.  Returns FALSE if
 * the new input queue is congested and packet could not be re-entered.
 * returns TRUE if packet put on new input queue.
 */

boolean change_if_input (paktype *ptr, idbtype *new_input)
{
    hwidbtype *new_hwidb = new_input->hwptr;
    leveltype status;

    /* Same hwidb? Then ok to change. */
    if (ptr->if_input->hwptr == new_hwidb) {
	ptr->if_input = new_input;
	return(TRUE);
    }

    /*
     * Disable interrupts to interlock the interface input queuing count
     */
    status = raise_interrupt_level(ALL_DISABLE);

    /* Overloading the new input interface? */
    if (new_hwidb->input_qcount > new_hwidb->input_defqcount) {
	new_hwidb->counters.input_drops++;
	reset_interrupt_level(status);
	return(FALSE);
    }

    clear_if_input_inline(ptr);
    set_if_input_inline(ptr, new_input);
    reset_interrupt_level(status);
    return(TRUE);
}

/*
 * appn_init_pool
 * This function initialises permanent buffers in the public pool for APPN
 * and also returns the sizes of the various pools and the number of pools.
 */
#define MAX_APPN_POOL_SIZE 8192
void appn_init_pool (int *pool_info_p, int *router_bfr_pools)
{
    list_element *element;
    pooltype     *pool;
    int i = 0;

    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	if (pool->size <= MAX_APPN_POOL_SIZE) {
	    *(pool_info_p + i) = pool->size;
	    i ++;
	}
    }
    *router_bfr_pools = i;
}

/*
 * pak_encap()
 * Set the needed fields and call vencap
 */
boolean pak_encap (hwidbtype *hwidb, paktype *pak, int length, long address)

{
    pak->datagramstart = pak->network_start;
    pak->datagramsize = length;
    return(hwidb->vencap(pak, address));
}




