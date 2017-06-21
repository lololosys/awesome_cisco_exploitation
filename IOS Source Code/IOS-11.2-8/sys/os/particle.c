/* $Id: particle.c,v 3.8.12.2 1996/07/30 01:08:56 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/os/particle.c,v $
 *------------------------------------------------------------------
 * particle.c - Support for discrete buffer blocks
 *
 * September 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: particle.c,v $
 * Revision 3.8.12.2  1996/07/30  01:08:56  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/03/18  21:30:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.16.2  1996/03/09  05:10:00  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.16.1  1996/02/20  16:40:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  05:53:51  smackie
 * Align particle headers on cacheline boundaries. (CSCdi48798)
 *
 * Revision 3.7  1996/01/26  22:20:49  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.6  1995/12/10  04:01:04  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.5  1995/11/29  05:53:53  smackie
 * Move the background processing of pool discards to the central pool
 * manager process. (CSCdi44506)
 *
 * Revision 3.4  1995/11/28  02:25:47  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.3  1995/11/17  18:50:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  06:32:44  smackie
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
#include "chunk.h"
#include "pool.h"
#include "particle.h"
#include "buffers.h"
#include "buffers_inline.h"
#include "queue_inline.h"
#include "free.h"


/*
 * Local Storage
 */
list_header particlepublicQ;  /* list of public particle pools */
list_header particleprivateQ; /* list of private particle pools */

uint	    clone_hits;	      /* count of hits in the cloneq */
uint 	    clone_misses;     /* count of misses in the cloneq */

queuetype   cloneQ;	      /* queue of particle clones */


/*
 * Pool vectors
 */
static void *particle_pool_item_create(pooltype *pool, pool_item_type type);
static void particle_pool_item_destroy(pooltype *pool, void *item);
static void *particle_pool_item_get(pooltype *pool);
static void particle_pool_item_ret(pooltype *pool, void *item);
static void particle_pool_item_status(pooltype *pool, void *item,
				      pool_item_status *status);
static void particle_pool_cache_item_ret(void *item);
static void *particle_pool_cache_item_get(pooltype *pool);

static pool_item_vectors particle_pool_item_vectors = {
    particle_pool_item_create,
    particle_pool_item_destroy,
    particle_pool_item_get,
    particle_pool_item_ret,
    particle_pool_item_status,
    (pool_item_validate_t)return_true
};

static pool_cache_vectors particle_pool_cache_vectors = {
    particle_pool_cache_item_get,
    particle_pool_cache_item_ret,
    NULL,
};


/*
 * particle_pool_init
 *
 * Initialize particle pool support at runtime
 */

void particle_pool_init (void)
{
    /*
     * Initialize particle pool queues
     */
    list_create(&particlepublicQ, 0, "Public Particle Pools",
		LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&particleprivateQ, 0, "Private Particle Pools", 
		LIST_FLAGS_INTERRUPT_SAFE);

    /*
     * Register some functions
     */
    reg_add_onemin(particle_pool_periodic, "particle_pool_periodic");
}


/*
 * particle_clones_init:
 * Initilaize particle clone queue
 */
boolean particle_clones_init (int clones)
{
    int bytes, i;
    particletype *ptr;

    bytes = clones * sizeof(particletype);
    ptr = malloc_named_aligned(bytes, "Clones", sizeof(particletype));
    if (!ptr) {
	return(FALSE);
    }
    queue_init(&cloneQ, 0);
    for (i=0; i<clones; i++) {
	p_enqueue(&cloneQ, ptr);
	ptr += 1;
    }
    return(TRUE);
}


/*
 * particle_pool_create
 *
 * Create a new pool of particles
 */

pooltype *particle_pool_create (char    *name,
				int      group,
				int      size,
				uint     flags,
				ulong    alignment,
				mempool *mempool)
{
    pooltype    *pool;
    list_header *queue;

    if (!mempool)
	mempool = mempool_find_by_class(MEMPOOL_CLASS_IOMEM);
	
    if (group == POOL_GROUP_PUBLIC)
	queue = &particlepublicQ;
    else
	queue = &particleprivateQ;

    /*
     * Create our pool structure
     */
    pool = pool_create(name, group, size, flags, mempool, queue,
		       &particle_pool_item_vectors);

    /*
     * Make sure the candy store isn't empty...
     */
    if (!pool)
	return(NULL);

    /*
     * Store our alignment for the chunk creation
     */
    pool->alignment = alignment;
    
    /*
     * Add the pool encap and trailer sizes
     */
    pool->encapsize   = PARTICLE_ENCAPBYTES;
    pool->trailersize = PARTICLE_TRAILBYTES;

    return(pool);
}


/*
 * particle_pool_chunk_create
 *
 * Create a new chunk pool for particles or data
 */

static chunk_type *particle_pool_chunk_create (pooltype *pool,
					       mempool  *mempool,
					       ulong     alignment,
					       ulong     size)
{
    chunk_type *chunk;
    ulong       flags = CHUNK_FLAGS_NONE;
    int         maximum;
    
    /*
     * If the pool's dynamic, the chunk's dynamic
     */
    if (pool->flags & POOL_DYNAMIC)
	flags |= CHUNK_FLAGS_DYNAMIC;
    
    /*
     * Work out the number of elements to populate the chunk with
     */
    if (pool->flags & POOL_DYNAMIC)
	maximum = min(PARTICLE_CHUNK_MAXIMUM, pool->permtarget);
    else
	maximum = pool->permtarget;

    /*
     * Create our chunk pool from the particle pool sizes
     */
    chunk = chunk_create(size,
			 maximum,
			 flags,
			 mempool, alignment,
			 pool->name);
    return(chunk);
}


/*
 * particle_pool_item_create
 *
 * Called to create a new particle
 */

static void *particle_pool_item_create (pooltype       *pool,
					pool_item_type  type)
{
    particletype *particle;
    pakdata      *data;

    /*
     * Have we had our chunks today?
     */
    if (!pool->particle_chunk) {
	pool->particle_chunk =
	    particle_pool_chunk_create(pool, NULL, 
				       sizeof(particletype),
				       sizeof(particletype));
	/*
	 * We failed. No particles today.
	 */
	if (!pool->particle_chunk)
	    return(NULL);
    }

    if (!pool->data_chunk) {
	int size = pool->size + pool->encapsize + pool->trailersize +
	           sizeof(pakdata);
	
	pool->data_chunk = particle_pool_chunk_create(pool,
						      pool->mempool,
						      pool->alignment,
						      size);
	/*
	 * We've got headers but no data. Time to bale.
	 */
	if (!pool->data_chunk)
	    return(NULL);
    }

    /*
     * Grab our particle header and associated data block
     */
    particle = chunk_malloc(pool->particle_chunk);
    if (!particle)
	return(NULL);

    data = chunk_malloc(pool->data_chunk);
    if (!data) {
	chunk_free(pool->particle_chunk, particle);
	return(NULL);
    }

    /*
     * Attach the particle to the pakdata
     */
    particle_attach_pakdata(pool, particle, data);

    /*
     * "We're one of a kind, oh yeah". Make sure that this is an original.
     */
    particle->original = NULL;

    /*
     * Beancounting
     */
    if (type == POOL_ITEM_TYPE_PERMANENT) {
	particle->flags |= PARTICLE_FLAGS_PERMANENT;
	pool->perm++;
    } else {
	pool->grows++;
    }
    pool->total++;

    return(particle);
}


/*
 * particle_pool_item_destroy
 *
 * Remove a particle
 */

static void particle_pool_item_destroy (pooltype *pool, void *item)
{
    particletype *particle = item;

    if (onintstack()) {
	/*
	 * If we're on an interrupt stack, enqueue for later destruction
	 */
	pool_request_discard(pool, item);
    } else {
	/*
	 * Adjust some counters
	 */
	if (particle->flags & PARTICLE_FLAGS_PERMANENT) {
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
	 * Free the particle chunk and it's data
	 */
	chunk_free(pool->data_chunk, particle->data_block);
	chunk_free(pool->particle_chunk, particle);
    }
}


/*
 * particle_pool_item_get
 *
 * Grab a particle from the freelist
 */

static void *particle_pool_item_get (pooltype *pool)
{
    particletype *particle = NULL;

    /*
     * If the pool is dynamic, check our sizing
     */
    if (pool->flags & POOL_DYNAMIC) {
	/*
	 * Make sure we're not at interrupt level
	 */
	if (!onintstack()) {
	    /*
	     * We'll try to top up this pool to minfree
	     */
	    while (pool->free <= pool->minfree) {
		particle = (*pool->item.create)(pool,
						POOL_ITEM_TYPE_TEMPORARY);
		if (!particle)
		    break;
		pool_enqueue_item(pool, particle, POOL_ITEM_TYPE_TEMPORARY);
	    }
	
	    /*
	     * Create the particle we need
	     */
	    particle = (*pool->item.create)(pool, POOL_ITEM_TYPE_TEMPORARY);
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
    if (!particle)
	particle = pool_dequeue_item(pool);
    
    /*
     * We really failed. Chalk up the failure.
     */
    if (!particle)
	pool->failures++;
    
    return(particle);
}


/*
 * particle_pool_item_ret
 *
 */

static void particle_pool_item_ret (pooltype *pool, void *item)
{
    pool_item_type  type;
    particletype   *particle = item;

    type = (particle->flags & PARTICLE_FLAGS_PERMANENT) ?
	POOL_ITEM_TYPE_PERMANENT : POOL_ITEM_TYPE_TEMPORARY;

    if ((pool->free >= pool->maxfree) && (type != POOL_ITEM_TYPE_PERMANENT)) {
	(*pool->item.destroy)(pool, item);
    } else {
	particle_reset(particle);
	particle->refcount = 0;
	pool_enqueue_item(pool, item, type);
    }
}


/*
 * particle_pool_item_status
 *
 */

static void particle_pool_item_status (pooltype         *pool,
				       void             *item,
				       pool_item_status *status)
{
    particletype *particle = item;

    if (particle->flags & PARTICLE_FLAGS_PERMANENT)
	status->type = POOL_ITEM_TYPE_PERMANENT;
    else
	status->type = POOL_ITEM_TYPE_TEMPORARY;
    TIMER_STOP(status->age);
}


/*
 * particle_pool_create_cache
 *
 * Create a new cache pool of particles
 */

boolean particle_pool_create_cache (pooltype *pool, int max_size)
{
    /*
     * Create our pool cache structure
     */
    return(pool_create_cache(pool, max_size,
			     &particle_pool_cache_vectors, 0));
}


/*
 * particle_pool_cache_item_get
 *
 * Get an element from a pool to fill it's cache
 */

static void *particle_pool_cache_item_get (pooltype *pool)
{
    return(pool_getparticle(pool));
}


/*
 * particle_pool_cache_item_get
 *
 * Get an element from a pool to fill it's cache
 */

static void particle_pool_cache_item_ret (void *item)
{
    return(retparticle(item));
}


/*
 * particle_pool_periodic
 *
 * Handle process level growth, trims and discards
 */

void particle_pool_periodic (void)
{
    list_element *element;
    pooltype     *pool;

    /*
     * Tune the particle pools
     */
    FOR_ALL_DATA_IN_LIST(&particlepublicQ, element, pool) {
	pool_prune(pool);
    }

    FOR_ALL_DATA_IN_LIST(&particleprivateQ, element, pool) {
	pool_prune(pool);
    }
}


/*
 * particle_find_by_addr
 *
 * Given a head particle and an address, returns a pointer to the particle
 * that the address is in, or NULL
 */

particletype *particle_find_by_addr (particletype *particle, void *addr)
{
    return(particle_find_by_addr_inline(particle, addr));
}


/* 
 * getparticle
 *
 * Get a particle of a certain size from a public pool
 */

particletype *getparticle (int size)
{
    leveltype     status;
    list_element *element;
    particletype *particle = NULL;
    pooltype     *pool;

    /*
     * Check for size out of range
     */
    if (size < 0) {
	return(NULL);
    }
    
    status = raise_interrupt_level(ALL_DISABLE);

    FOR_ALL_DATA_IN_LIST(&particlepublicQ, element, pool) {
	particle = getparticle_inline(pool, size, FALSE);
	if (particle)
	    break;
    }

    reset_interrupt_level(status);
    return(particle);
}


/* 
 * pool_getparticle
 *
 * Get a particle from a specified pool
 */

particletype *pool_getparticle (pooltype *pool)
{
    leveltype     status;
    particletype *particle;

    status = raise_interrupt_level(ALL_DISABLE);
    particle = getparticle_inline(pool, pool->size, TRUE);
    reset_interrupt_level(status);
    return(particle);
}

/*
 * retparticle
 *
 * Return a particle to it's pool owner
 */

void retparticle (particletype *particle)
{
    leveltype  status;

    status = raise_interrupt_level(ALL_DISABLE);
    retparticle_inline(particle);
    reset_interrupt_level(status);
}


/*
 * particle_enqueue
 *
 * Add a particle to the end of a paktype chain of particles
 */

void particle_enqueue (paktype *pak, particletype *particle)
{
    leveltype  status;

    status = raise_interrupt_level(ALL_DISABLE);
    particle_enqueue_inline(pak, particle);
    reset_interrupt_level(status);
}


/*
 * particle_dequeue
 *
 * Remove a particle from the head of the paktype chain
 */

particletype *particle_dequeue (paktype *pak)
{
    leveltype     status;
    particletype *particle;

    status = raise_interrupt_level(ALL_DISABLE);
    particle = particle_dequeue_inline(pak);
    reset_interrupt_level(status);
    
    return(particle);
}


/*
 * particle_retbuffer
 *
 * Frees all of the particles chained to a buffer
 */

void particle_retbuffer (paktype *pak)
{
    particletype *particle;

    while ((particle = particle_dequeue_inline(pak))) {
	retparticle_inline(particle);
    }
}


/*
 * particle_reparent
 *
 * Reparent a list of particles onto a new header.
 */
paktype *particle_reparent (paktype *pak, pooltype *pool)
{
    paktype *newpak;

    newpak = pool_dequeue_cache(pool);
    if (!newpak) {
	newpak = pool_getbuffer(pool);
	if (!newpak) {
	    return(NULL);
	}
    }

    /*
     * Move over the particles
     */
    newpak->particlequeue.count = pak->particlequeue.count;
    newpak->particlequeue.qhead = pak->particlequeue.qhead;
    newpak->particlequeue.qtail = pak->particlequeue.qtail;
    newpak->datagramstart = pak->datagramstart;
    newpak->datagramsize = pak->datagramsize;
    newpak->acct_proto = pak->acct_proto;

    /*
     * Zero the pak fields
     */
    pak->particlequeue.count = 0;
    pak->particlequeue.qhead = pak->particlequeue.qtail = NULL;

    return(newpak);
}
