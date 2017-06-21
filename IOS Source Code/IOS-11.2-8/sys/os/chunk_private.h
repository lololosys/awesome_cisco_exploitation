/* $Id: chunk_private.h,v 3.1.2.2 1996/08/03 05:08:36 smackie Exp $
 * $Source: /release/112/cvs/Xsys/os/chunk_private.h,v $
 *------------------------------------------------------------------
 * chunk_private.h - Private includes for the chunk manager
 *
 * June 1996, Scott Mackie
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chunk_private.h,v $
 * Revision 3.1.2.2  1996/08/03  05:08:36  smackie
 * Add support for unzeroed chunks. (CSCdi65048)
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/19  23:17:13  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/15  01:07:25  smackie
 * Add placeholders for chunk manager spring clean
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CHUNK_PRIVATE_H__
#define __CHUNK_PRIVATE_H__

/*
 * Structures
 */

struct chunk_type_ {
    struct chunk_type_ *next;         /* Next mast chunk in chunkQ        */
    struct chunk_type_ *next_sibling; /* Next sibling in chunk family     */
    struct chunk_type_ *prev_sibling; /* Previous sibling in family       */
    uint                flags;        /* Chunk status flags               */
    uint                maximum;      /* Maximum number of chunk elements */
    uint                size;         /* Size in bytes of each element    */
    int                 index;        /* Index to first free element      */
    uint		inuse;        /* Total chunks currently in use    */
    uint		inuse_hwm;    /* HWM of Total chunks used         */
    mempool    	       *mempool;      /* Memory pool for the chunks       */
    ulong    	        alignment;    /* Fundamental alignment for chunks */
    char               *name;         /* Chunk name                       */
    uchar              *data;         /* Start of the actual element data */
    uchar              *end;          /* End of the chunk                 */
    uchar              *freelist[0];  /* Start of the free list           */
};

typedef struct chunk_lockable_data_ {
    ulong refcount;
    uchar data[0];
} chunk_lockable_data;

/*
 * Inlines
 */

static inline boolean chunk_is_dynamic (chunk_type *chunk)
{
    return(chunk->flags & CHUNK_FLAGS_DYNAMIC);
}

static inline boolean chunk_is_sibling (chunk_type *chunk)
{
    return(chunk->flags & CHUNK_FLAGS_SIBLING);
}

static inline boolean chunk_is_independant (chunk_type *chunk)
{
    return(chunk->flags & CHUNK_FLAGS_INDEPENDANT);
}

static inline boolean chunk_is_lockable (chunk_type *chunk)
{
    return(chunk->flags & CHUNK_FLAGS_LOCKABLE);
}

static inline boolean chunk_is_unzeroed (chunk_type *chunk)
{
    return(chunk->flags & CHUNK_FLAGS_UNZEROED);
}

static inline ulong *chunk_refcount (void *data)
{
    chunk_lockable_data *ldata;

    ldata = (chunk_lockable_data *)((ulong)data - sizeof(chunk_lockable_data));

    return(&ldata->refcount);
}

static inline boolean chunk_lock_inline (chunk_type *chunk,
					 void       *data,
					 boolean     reset)
{
    if (chunk_is_lockable(chunk)) {
	/*
	 * We're lockable. If reset is TRUE, we set the refcount to 1. If
	 * it's FALSE, we increment the refcount. This is usually passed
	 * in as a immediate value and gets optimised out
	 */
	if (reset)
	    *chunk_refcount(data) = 1;
	else
	    (*chunk_refcount(data))++;

	return(TRUE);
    } else {
	/*
	 * It's not lockable. Let the caller know
	 */
	return(FALSE);
    }
}

static inline boolean chunk_unlock_inline (chunk_type *chunk, void *data)
{
    ulong *refcount;

    if (chunk_is_lockable(chunk)) {
	/*
	 * We're unlockable. Decrement the refcount and return TRUE if the
	 * chunk is unlocked
	 */
	refcount = chunk_refcount(data);
	if (*refcount > 1) {
	    (*refcount)--;
	    return(FALSE);
	} else {
	    return(TRUE);
	}
    } else {
	/*
	 * It's not unlockable.
	 */
	return(TRUE);
    }
}

static inline void chunk_increment_inuse (chunk_type *head)
{
    /*
     * Increment the use count and check the high water mark.
     */
    head->inuse++;
    if (head->inuse > head->inuse_hwm)
	head->inuse_hwm = head->inuse;
}

static inline void chunk_decrement_inuse (chunk_type *head)
{
    head->inuse--;
}


/*
 * Externs
 */

extern queuetype chunkQ;
extern ulong     chunk_created;
extern ulong     chunk_destroyed;
extern ulong     chunk_sibling_created;
extern ulong     chunk_sibling_trimmed;

#endif
