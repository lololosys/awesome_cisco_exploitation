/* $Id: chunk.c,v 3.6.18.5 1996/08/28 13:02:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/chunk.c,v $
 *------------------------------------------------------------------
 * chunk.c - A generic, fixed memory chunk manager
 *
 * April 1994, Scott Mackie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: chunk.c,v $
 * Revision 3.6.18.5  1996/08/28  13:02:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.4  1996/08/03  05:08:33  smackie
 * Add support for unzeroed chunks. (CSCdi65048)
 * Branch: California_branch
 *
 * Revision 3.6.18.3  1996/07/10  22:21:42  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.18.2  1996/06/19  23:17:07  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.6.18.1  1996/03/18  21:29:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.10.3  1996/03/09  05:08:57  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.10.2  1996/03/07  10:17:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.10.1  1996/02/20  16:37:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/07  22:58:45  smackie
 * Lock down chunk structures whilst perusing them for "show chunk"
 * to prevent bad references. Like, duh. (CSCdi48146)
 *
 * Revision 3.5  1996/01/29  07:29:09  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/05  04:56:30  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:45:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/19  18:38:37  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.3  1995/08/03  21:48:29  hampton
 * Chunk manager should record chunk usage high water mark. [CSCdi38207]
 *
 * Revision 2.2  1995/06/21 08:56:08  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  21:53:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "logger.h"
#include "config.h"
#include "free.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "chunk.h"
#include "chunk_private.h"

/*
 * A brief description of the Chunk Manager
 *
 * This is a hack. Plain and simple. It's a neat hack, but it's still
 * a hack. What this code does is allocate a big chunk of memory and
 * carve it up into smaller chunks. How's this different from the
 * memory manager we already have and malloc/free? Well, the main
 * difference is that the memory overhead is only paid for by the
 * big chunk, not for the little ones. This saves about 30 bytes per
 * element you normally allocate via malloc. Another difference is that
 * there are no interrupt restrictions on this code so you can use
 * chunk_malloc and chunk_free at interrupt level (of course, the
 * original allocation has to happen at process level and the chunk
 * can't grow at interrupt level).
 *
 * This code makes managing huge (>500 elements) lists of tiny structures
 * (i.e. under 24 bytes) more efficient.
 *
 * It's Chunkarific! ;-)
 *
 * Caveats
 * -------
 * 
 * o This code doesn't have the frills of malloc()/free(). It's not meant
 *   to. The protection against screwing up isn't anywhere near a full API.
 *   This code is meant for a narrow range of applications.
 *
 * o Dynamic growing of pools won't work from interrupt level. If you
 *   have to ask "why?" here, maybe you should go and read os/free.c
 *   (and os/buffers.c) before delving further.
 *
 * o It's possible to use this manager in a way that'll make matters
 *   worse. Much worse. Keep the maximum sizes in inverse proportion
 *   with the size of the structure. Use the "saved" count in "show
 *   chunk" to help you.
 *
 */

/*
 * Local variables
 */

queuetype chunkQ;
ulong      chunk_created;
ulong      chunk_destroyed;
ulong      chunk_sibling_created;
ulong      chunk_sibling_trimmed;


/*
 * chunk_init
 *
 * Initialize the gobal chunk manager variables and queue
 */

void chunk_init (void)
{
    /*
     * Create the master chunk queue
     */
    queue_init(&chunkQ, 0);

    /*
     * Init some counters
     */
    chunk_created = 0;
    chunk_destroyed = 0;
    chunk_sibling_created = 0;
    chunk_sibling_trimmed = 0;
}


/*
 * chunk_create
 *
 * Create a chunk descriptor. This mallocs enough space for the chunk
 * context and the data that it'll manage.
 */

chunk_type *chunk_create (ulong          size,
			  ulong          maximum,
			  ulong          flags, 
			  mempool       *mempool,
			  ulong          alignment,
			  char          *name)
{
    chunk_type *chunk;
    int         data_size, data_element_size, chunk_size, count;

    /*
     * If the mempool isn't a local one or the alignment is non-zero,
     * we allocate the header and freelist independantly from the data
     */
    if (alignment || ((mempool) && (mempool->class != MEMPOOL_CLASS_LOCAL)))
	flags |= CHUNK_FLAGS_INDEPENDANT;

    /*
     * Set our element size. This *might* not be the same as chunk->size
     * depending on the chunk flags
     */
    data_element_size = size;

    /*
     * If the chunk is lockable, we prefix each element with a 4 byte
     * refcount field
     */
    if (flags & CHUNK_FLAGS_LOCKABLE)
	data_element_size += sizeof(chunk_lockable_data);

    /*
     * Calculate the size of the chunk header and the data area
     */
    chunk_size = sizeof(chunk_type) + (sizeof(uchar *) * maximum);
    data_size = data_element_size * maximum;

    /*
     * If the data area is attached, add it to the chunk total
     */
    if (!(flags & CHUNK_FLAGS_INDEPENDANT))
	chunk_size += data_size;

    /*
     * If mempool is NULL, we use a default
     */
    if (!mempool)
	mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
    
    /*
     * Allocate our chunk
     */
    chunk = malloc_named(chunk_size, name);
    if (!chunk)
	return(NULL);

    /*
     * Fill in some context information.
     */
    chunk->size    = size;
    chunk->maximum = maximum;
    chunk->flags   = flags;
    chunk->name    = name;
    chunk->mempool = mempool;
    chunk->next    = NULL;
    chunk->alignment    = alignment;
    chunk->next_sibling = NULL;
    chunk->prev_sibling = NULL;

    /*
     * If the chunk is a independant one, the data is allocated separately
     * of the chunk header
     */
    if (chunk_is_independant(chunk)) {
	/*
	 * We have independant memory. Allocate the data now.
	 */
	chunk->data = mempool_malloc_aligned(mempool->class,
					     data_size,
					     alignment);

	/*
	 * Whoops. We have a header but no data. Bale.
	 */
	if (!chunk->data) {
	    free(chunk);
	    return(NULL);
	}

	/*
	 * Name our new block of data for posterity
	 */
	name_memory(chunk->data, name);

    } else {
	/*
	 * The data is attached to the chunk header
	 */
	chunk->data = (uchar *)&chunk->freelist[maximum];
    }

    /*
     * Build our boundary pointers for this chunk
     */
    chunk->end = chunk->data + data_size;

    /*
     * Populate the chunk freelist
     */
    for (count = 0; count < maximum; count++) {
	/*
	 * Step up the chunk data and build the freelist
	 */
	chunk->freelist[count] = chunk->data + (count * data_element_size);

	/*
	 * If the chunk is lockable, we use the first 4 bytes as a 
	 * refcount
	 */
	if (chunk_is_lockable(chunk))
	    chunk->freelist[count] += sizeof(chunk_lockable_data);
    }

    /*
     * Remember how big it got...
     */
    chunk->index     = maximum;
    chunk->inuse     = 0;
    chunk->inuse_hwm = 0;

    /*
     * Final housekeeping
     */
    chunk_created++;

    if (!chunk_is_sibling(chunk)) {
	/*
	 * Remember it for posterity
	 */
	enqueue(&chunkQ, chunk);
    }

    return(chunk);
}


/*
 * chunk_destroy
 *
 * Destroys a chunk structure depending on it's sibling status
 */

boolean chunk_destroy (chunk_type *chunk)
{
    /*
     * Make sure it's a live one
     */
    if (!chunk)
	return(FALSE);

    /*
     * All present and accounted for?
     */
    if (chunk->index != chunk->maximum) {
	(*kernel_errmsg)(&msgsym(CHUNKPARTIAL, SYS), chunk);
	return(FALSE);
    }

    /*
     * Decide if it's a sibling free or the main man.
     */
    if (chunk_is_sibling(chunk)) {
	/*
	 * Time to prune a spare sibling
	 */
	chunk->prev_sibling->next_sibling = chunk->next_sibling;
	if (chunk->next_sibling)
	    chunk->next_sibling->prev_sibling = chunk->prev_sibling;
	
	chunk_sibling_trimmed++;

    } else {
	/*
	 * It's a wipeout. Only allow the main structure to be deleted
	 * if there are no siblings.
	 */
	if (chunk->next_sibling != NULL) {
	    (*kernel_errmsg)(&msgsym(CHUNKSIBLINGS, SYS), chunk);
	    return(FALSE);
	}

	/*
	 * Pull the structure out of the global list
	 */
	unqueue(&chunkQ, chunk);
    }
	
    chunk_destroyed++;

    /*
     * Hand back that memory
     */
    if (chunk_is_independant(chunk))
	free(chunk->data);

    chunk->next = NULL;
    chunk->next_sibling = NULL;
    free(chunk);

    return(TRUE);
}

/*
 * chunk_is_destroyable
 *
 * Returns TRUE if all baby chunks have been freed and the initial chunk
 * can be destroyed.
 */
boolean chunk_is_destroyable (chunk_type *chunk)
{
    /* Can't destroy if there's anything still allocated from this guy. */

    if (chunk->index != chunk->maximum)
	return(FALSE);

    /* Can't destroy if there are any siblings. */

    if (chunk->next_sibling != NULL)
	return(FALSE);

    /* Guess it's OK. */

    return(TRUE);
}


/*
 * chunk_malloc
 *
 * Grabs a chunk of free memory from the specified chunk. If there isn't
 * enough memory, it'll try and create a sibling chunk if it can.
 */

void *chunk_malloc (chunk_type *chunk)
{
    leveltype   level;
    chunk_type *head;
    void       *data;

    /*
     * Make sure we have a chunk to malloc from
     */
    if (!chunk)
	return(NULL);

    head = chunk;

    while (chunk) {
	/*
	 * Check to see if this chunk's got some memory spare...
	 */
	level = raise_interrupt_level(ALL_DISABLE);

	if (chunk->index > 0) {
	    /*
	     * Fetch the next free chunk
	     */
	    chunk->index--;
	    data = chunk->freelist[chunk->index];
	    reset_interrupt_level(level);

	    /*
	     * Keep some chunk useage accounting
	     */
	    chunk_increment_inuse(head);

	    /*
	     * Zero out the memory if the chunk isn't marked as being
	     * unzeroed
	     */
	    if (!chunk_is_unzeroed(chunk))
		memset(data, 0, chunk->size);

	    /*
	     * Initialize the chunk refcount if we have to
	     */
	    chunk_lock_inline(chunk, data, TRUE);

	    return(data);
	}

	reset_interrupt_level(level);
	
	/*
	 * Any other siblings to cadge a bit of memory from?
	 */
	if (chunk->next_sibling) {
	    chunk = chunk->next_sibling;
	} else {
	    /*
	     * We're all out of chunks. If we're allowed to grab some
	     * more chunks, do so
	     */
	    if (chunk_is_dynamic(chunk)) {
		chunk_type *new;
	
		/*
		 * Grab another chunk of memory
		 */
		new = chunk_create(chunk->size, 
				   chunk->maximum,
				   chunk->flags | CHUNK_FLAGS_SIBLING,
				   chunk->mempool, 
				   chunk->alignment,
				   chunk->name);
	    
		/*
		 * If we managed to grab another big chunk, add it to
		 * the end of the chain
		 */
		if (new) {
		    chunk->next_sibling = new;
		    new->prev_sibling   = chunk;
		    
		    chunk_sibling_created++;
		}
		    
		/*
		 * Let the chunk point at the new block and go once
		 * more around the loop
		 */
		chunk = new;
	    } else {
		/*
		 * It's all over. 
		 */
		chunk = NULL;
	    }
	}
    }

    /*
     * Tough luck.
     */
    return(NULL);
}


/*
 * chunk_free
 *
 * Tries to return a chunk to the specified descriptor. If the chunk
 * isn't within the bounds of the descriptor passed in, it'll walk
 * down the sibling list looking for the right chunk to return to.
 */

boolean chunk_free (chunk_type *chunk, void *data)
{
    leveltype   level;
    chunk_type *head;

    /*
     * Make sure we have a chunk and data to work with
     */
    if ((!chunk) || (!data))
	return(FALSE);

    /*
     * If we have a refcount, is it time to free yet?
     */
    if (!chunk_unlock_inline(chunk, data))
	return(TRUE);

    head = chunk;

    while (TRUE) {
	/*
	 * Does this data belong to this chunk?
	 */
	if (((uchar *)data >= chunk->data) && ((uchar *)data < chunk->end)) {
	    /*
	     * Add a little sanity checking
	     */
	    if (chunk->index >= chunk->maximum) {
		/*
		 * Something's gone horribly wrong.
		 */
		(*kernel_errmsg)(&msgsym(CHUNKBOUNDS, SYS), chunk, data);
		return(FALSE);
	    }

	    /*
	     * Put the memory back in the freelist safely
	     */
	    level = raise_interrupt_level(ALL_DISABLE);
	    chunk->freelist[chunk->index++] = data;
	    reset_interrupt_level(level);

	    /*
	     * Keep some chunk useage accounting
	     */
	    chunk_decrement_inuse(head);

	    /*
	     * Okay. Is this chunk now full and a sibling? If it is, it's
	     * time to hand this puppy back.
	     */
	    if (chunk_is_dynamic(chunk) &&
		chunk_is_sibling(chunk) &&
		(chunk->index == chunk->maximum))
		chunk_destroy(chunk);
	    
	    return(TRUE);
	}
	
	/*
	 * If there's some siblings, it probably belongs to them...
	 */
	if (chunk->next_sibling)
	    chunk = chunk->next_sibling;
	else
	    break;
    }

    (*kernel_errmsg)(&msgsym(CHUNKFREE, SYS), chunk, data);
    return(FALSE);
}
			       

/*
 * chunk_lock
 *
 * Attempts to increment the refcount of an element of a chunk that
 * supports lockable elements
 */

boolean chunk_lock (chunk_type *chunk, void *data)
{
    /*
     * Increment the refcount
     */
    if (!chunk_lock_inline(chunk, data, FALSE)) {
	(*kernel_errmsg)(&msgsym(CHUNKLOCK, SYS), chunk, data);
	return(FALSE);
    } else {
	return(TRUE);
    }
}
	


	
	    
