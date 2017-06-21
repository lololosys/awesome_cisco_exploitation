/* $Id: particle.h,v 3.5.20.5 1996/08/28 13:03:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/particle.h,v $
 *------------------------------------------------------------------
 * particle.h - Definitions and prototypes for discrete buffer blocks
 *
 * September 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: particle.h,v $
 * Revision 3.5.20.5  1996/08/28  13:03:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.20.4  1996/07/30  01:08:58  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.5.20.3  1996/04/04  09:37:48  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.5.20.2  1996/03/21  23:21:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.20.1  1996/03/18  21:30:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.6  1996/03/03  08:39:49  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.2.2.5  1996/02/08  08:43:31  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.4  1996/01/16  01:03:29  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/11  19:11:35  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/05  06:35:21  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.2.2.1  1995/12/04  22:17:50  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.5  1996/01/26  22:20:51  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.4  1995/12/10  04:01:05  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.3  1995/11/28  02:25:48  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:50:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  06:32:46  smackie
 * Add placeholders for Arkansas
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "../os/queue_inline.h"
#include "../os/chunk.h"

#include "logger.h"
#ifndef __MSG_SYSTEM_C__
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#endif


/*
 * Defines
 */

#define PARTICLE_FLAGS_PERMANENT 0x0001
#define PARTICLE_NO_OFFSET ((uint)-1)  /* for negative offset comparisons */

/*
 * The amount of space to add to the front of each particle
 */
#define PARTICLE_ENCAPBYTES 32
#define PARTICLE_TRAILBYTES (PARTICLE_ENCAPBYTES - sizeof(pakdata))

#define particle_is_clone(particle) ((particle)->original)

#define PARTICLE_CHUNK_MAXIMUM 64


/*
 * Externs
 */
extern list_header particlepublicQ;  /* list of public particle pools */
extern list_header particleprivateQ; /* list of private particle pools */

extern queuetype cloneQ;
extern uint	 clone_hits, clone_misses;


/*
 * Prototypes
 *
 * These come first because some of the inlines use them.
 */

extern void          particle_pool_init(void);
extern boolean 	     particle_clones_init(int clones);
extern pooltype     *particle_pool_create(char *name, int group, int size,
					  uint flags, ulong alignment,
					  mempool *mempool);
extern boolean       particle_pool_create_cache(pooltype *pool, int max_size);
extern void          particle_pool_periodic(void);
extern particletype *getparticle(int size);
extern particletype *pool_getparticle(pooltype *pool);
extern void          retparticle(particletype *particle);
extern void          particle_enqueue(paktype *pak, particletype *particle);
extern particletype *particle_dequeue(paktype *pak);
extern void          particle_retbuffer(paktype *pak);
extern paktype	    *particle_reparent(paktype *pak, pooltype *pool);


/*
 * Inlines
 */

static inline int particle_get_refcount (particletype *particle)
{
    if (particle_is_clone(particle))
	return(particle->original->refcount);
    else
	return(particle->refcount);
}

static inline void particle_lock (particletype *particle)
{
    if (particle_is_clone(particle))
	atomic_increment(&particle->original->refcount);
    else
	atomic_increment(&particle->refcount);
}

static inline void particle_unlock (particletype *particle)
{
    if (particle_is_clone(particle))
	atomic_decrement(&particle->original->refcount);
    else
	atomic_decrement(&particle->refcount);
}


/*
 * particle_reset
 *
 * Initialize particle fields to a known state
 */

static inline void particle_reset (particletype *particle)
{
    particle->data_start = NULL;
    particle->data_bytes = 0;
}


/*
 * particle_center
 *
 * Find the network header detente of a particle
 */

static inline uchar *particle_center (particletype *particle)
{
    return(particle->data_block->data_area + particle->pool->encapsize);
}


/*
 * particle_find_by_addr_inline
 */

static inline
particletype *particle_find_by_addr_inline (particletype *particle,
					    void         *addr)
{
    uint size;

    while (particle) {
	/*
	 * Is our address within this particle?
	 */
	size = particle->pool->size;
	if (((uchar *)addr >= particle->data_block->data_area) ||
	    ((uchar *)addr < (particle->data_block->data_area + size))) {
	    break;
	}
	particle = particle->next;
    }
    return(particle);
}



/* 
 * getparticle_inline
 */

static inline particletype *getparticle_inline (pooltype *pool,
                                                int       size,
                                                boolean   fallback)
{
    particletype *particle;

    /*
     * Check to make sure pool or size isn't bogus
     */
    if ((!pool) || (pool->size < size))
	return(NULL);
    
    /*
     * If our particle watermarks are okay for this pool, just
     * yank a particle from the freelist.
     */
    if (pool_low_watermarks(pool) ||
	((particle = pool_dequeue_item(pool)) == NULL)) {
	/*
	 * We've got to create particles
	 */
	pool->miss++;
        particle = (*pool->item.get)(pool);
    }

    /*
     * If we failed to grab a particle from the same pool as the source
     * particle, try grabbing one from its designated fallback pool, if
     * one is defined.
     */
    if ((fallback) && (!particle) && (pool->fallback)) {
	pool = pool->fallback;
	if (pool_low_watermarks(pool) ||
	    ((particle = pool_dequeue_item(pool)) == NULL)) {
	    /*
	     * We've got to create particles
	     */
	    pool->miss++;
	    particle = (*pool->item.get)(pool);
	}
    }

    if (particle) {
	if (particle_get_refcount(particle) != 0) {
	    errmsg(&msgsym(BADSHARE, SYS),
		   "getparticle", particle, particle_get_refcount(particle));
	    particle = NULL;
	} else {
	    /*
	     * Do a sanity check on the pool that the particle
	     * thinks that it belongs to.
	     */
	    if (particle->pool != pool) {
		/*
		 * We're toast. Work out if it's a misplaced
		 * buffer or just complete garbage
		 */
		if (POOLVALID(particle->pool)) {
		    errmsg(&msgsym(WRONGPOOL, SYS),
			   particle, pool, particle->pool);
		} else {
		    errmsg(&msgsym(BADPOOL, SYS),
			   particle, particle->pool);
		}
		return(NULL);
	    }
	    particle_lock(particle);
	    particle->data_start = particle->data_block->data_area;
	    particle->data_bytes = pool->size;
	}
	return(particle);
    }
    return(NULL);
}

/*
 * retparticle_inline
 */

static inline void retparticle_inline (particletype *particle)
{
    pooltype  *pool;
    int        refcount;

    if (!particle) {
	return;
    }

    /*
     * Is this a clone particle?
     */
    if (particle_is_clone(particle)) {
	particletype *original;

	/*
	 * Yup. We've got a clone. Handle this a little differently.
	 */
	original = particle->original;
	if (particle->next != NULL) {
	    errmsg(&msgsym(INLIST, SYS), particle);
	    return;
	}

	/*
	 * Hand back our clone header
	 */
	enqueue(&cloneQ, particle);
	
	/*
	 * We continue with the original particle.
	 */
	particle = original;
	if (particle->refcount > 1) {
	    particle_unlock(particle);
	    return;
	}
    }

    pool = particle->pool;
    if (!POOLVALID(pool)) {
	errmsg(&msgsym(BADPOOL, SYS), particle, pool);
	return;
    }
    
    refcount = particle_get_refcount(particle);
    if (refcount == 1) {
	if (particle->next != NULL) {
	    errmsg(&msgsym(INLIST, SYS), particle);
	    return;
	}

	if (pool->flags & POOL_CACHE) {
	    /*
	     * We have a buffer cache on this pool. Attempt to top it up
	     */
	    if (pool->cache_end < pool->cache_size) {
		/*
		 * Reset particle information and requeue it
		 */
		particle_reset(particle);
		pool->cache[pool->cache_end++] = particle;
		
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
	(*pool->item.ret)(pool, particle);

    } else if (refcount > 1) {
	particle_unlock(particle);
    } else {
        /*
         * bad use count, probably someone trying to free
         * memory that they've already freed. print an error
         * and a stack trace so we can catch them in the act.
         */
	errmsg(&msgsym(BADSHARE, SYS), "retparticle", particle, refcount);
    }
}


/*
 * particle_enqueue_inline
 *
 * Add a particle to the end of a paktype chain of particles
 */

static inline void particle_enqueue_inline (paktype      *pak,
					    particletype *particle)
{
    /*
     * If this is the first particle, set datagramstart
     */
    if (pak->particlequeue.count == 0) {
	pak->datagramstart = particle->data_start;
    }

    /*
     * Add this particle to the end of the paktype list
     */
    enqueue_inline(&pak->particlequeue, particle);

    /*
     * Increment our datagramsize accordingly
     */
    if (particle->data_bytes > 0)
	pak->datagramsize += particle->data_bytes;
}


/*
 * particle_dequeue_inline
 *
 * Remove a particle from the head of a paktype chain of particles
 */

static inline particletype *particle_dequeue_inline (paktype *pak)
{
    /*
     * Attempt to remove a particle from the head of the particle queue
     */
    return(dequeue_inline(&pak->particlequeue));
}

/*
 * particle_unqueue_inline
 *
 * Remove a particle from a paktype chain of particles
 */
 
static inline void particle_unqueue_inline (paktype *pak, particletype *mp)
{
    /*
     * Attempt to remove a particle from the particle queue
     */
    unqueue_inline(&pak->particlequeue, mp);
}

/*
 * particle_clip_inline:
 * Alter the size of a particle. A positive delta means we are making
 * the particle bigger by moving the start pointer backwards IE decrementing
 * it. A negative delta means we are making the particle smaller by moving
 * the start pointer forward IE incrementing it.
 */
static inline void particle_clip_inline (particletype *mp,
					 int delta)
{
    mp->data_start -= delta;
    mp->data_bytes += delta;
}

/*
 * allocate_fs_particle: 
 * Attempt to allocate a particle from the fastswitching particle pool
 */
static inline particletype *allocate_fs_particle (void)
{
    particletype *mp = NULL;

    mp = pool_dequeue_cache(fs_particle_pool);
    if (!mp && fs_particle_pool->fallback) {
	mp = pool_getparticle(fs_particle_pool->fallback);
    }
    if (mp) {
	mp->data_start = mp->data_block->data_area + PARTICLE_ENCAPBYTES;
    }
    return(mp);
}

/*
 * particle_clone
 *
 * Clone a particle *header* but use the original data. Note that
 * this is *not*, repeat *NOT*, a duplication. Any changes to the original
 * data will show up in all copies.
 * 
 * Note : It is assumed that this routine is called with network interrupts
 * disabled.
 */
static inline particletype *particle_clone (particletype *particle)
{
    particletype *clone;

    /*
     * Perform the usual paranoia
     */
    if (!particle) {
	return(NULL);
    }
    
    /*
     * Grab a header from the cloneQ
     */
    clone = dequeue(&cloneQ);
    if (!clone) {
	clone_misses++;
	return(NULL);
    }

    /*
     * Copy the header. Note that we don't have to adjust pointers
     * as we're using the same data
     */
    clone_hits++;
    clone->pool       = NULL;
    clone->pak        = particle->pak;
    clone->data_block = particle->data_block;
    clone->data_start = particle->data_start;
    clone->data_bytes = particle->data_bytes;
    clone->flags      = particle->flags;

    /*
     * Make sure that we're minty fresh for the caller.
     */
    clone->next     = NULL;
    clone->original = particle;

    /*
     * Increment our original data lock count.
     */
    particle_lock(particle);

    /*
     * Hand back our doppelganger
     */
    return(clone);
}

/*
 * particle_copy_to_buffer:
 * Copy a list of particles to a contigous buffer. Note, the original
 * partciles are not freed, this is up to the caller.
 */
static inline void particle_copy_to_buffer (paktype *scattered_pak, uchar *ptr)
{
    particletype *mp;

    for (mp=scattered_pak->particlequeue.qhead; mp; mp=mp->next) {
	bcopy(mp->data_start, ptr, mp->data_bytes);
	ptr += mp->data_bytes;
    }
}

#endif 
