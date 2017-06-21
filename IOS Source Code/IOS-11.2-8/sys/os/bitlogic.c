/* $Id: bitlogic.c,v 3.1 1997/02/28 16:51:37 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/os/bitlogic.c,v $
 *------------------------------------------------------------------
 * Dynamic bitfield manipulation routines
 *
 * February 1997, Dave Katz
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bitlogic.c,v $
 * Revision 3.1  1997/02/28 16:51:37  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "bitlogic.h"
#include "chunk.h"
#include "string.h"


/*
 * Dynamic Bitfield Support
 *
 * This code supports the manipulation of variable-length bitfields.
 * The allocation and resizing of the bitfields is automatic and
 * hidden from the caller.
 *
 * The external data structure is the dynamic_bitfield structure.
 * All operations in this module are performed on this structure.
 *
 * Callers should embed a dynamic_bitfield structure wherever a
 * variable-length bitfield is needed, and then simply call the
 * manipulation routines.
 *
 * The following operations are supported:
 *   Set a bit
 *   Clear a bit
 *   Test a bit
 *   Set bits from a mask (OR the bits into the bitfield)
 *   Clear bits from a mask (AND the inverse of the bits into the bitfield)
 *   Find the first clear bit
 *
 * The two Set operations will resize the bit array if it is too small
 * given the desired bit number (or the size of the mask in the latter
 * case).  Nonexistent bits are considered to be clear (so a clear operation
 * is a no-op, and a test operation will return FALSE).
 *
 * The caller *must* call bitfield_destroy() when it is done using a bitfield,
 * in order to return the allocated memory.  Otherwise, memory leaks will
 * result.
 *
 * This is accomplished by maintaining a set of bitfield chunk pools of
 * various sizes (a single set of pools for the entire system).  These
 * pools are created upon demand, and are released when all component chunks
 * have been freed.
 */


/* List of all bitfield pools */

static dynamic_bitfield_pool *bitfield_pool;

/*
 * bitnum_to_pool_size
 *
 * Convert a bit number to the pool size required to hold it.
 *
 * Example--bit number 0-31 requires 32 bits.
 */
static inline ulong bitnum_to_pool_size (ulong bitnum)
{
    return(((bitnum / DYNAMIC_BITFIELD_INCREMENT) + 1) *
	   DYNAMIC_BITFIELD_INCREMENT);
}


/*
 * bitfield_pool_link
 *
 * Link a bitfield pool into the queue.  The position is defined by a
 * pointer to the previous guy's next pointer (which could be the
 * head pointer).
 */
static void bitfield_pool_link (dynamic_bitfield_pool *pool,
				dynamic_bitfield_pool **prev)
{
    dynamic_bitfield_pool *next;

    /* "Straightforward."  If you grok my twisted pointer style. */

    next = *prev;			/* Pointer to next guy */
    pool->next = next;
    pool->prev = prev;
    *prev = pool;
    if (next)
	next->prev = &pool->next;
}

/*
 * bitfield_pool_delink
 *
 * Delink a bitfield pool from the queue.
 */
static void bitfield_pool_delink (dynamic_bitfield_pool *pool)
{
    *(pool->prev) = pool->next;
    if (pool->next)
	pool->next->prev = pool->prev;
    pool->next = NULL;
    pool->prev = NULL;
}


/*
 * bitfield_pool_cleanup
 *
 * Walk the bitfield pool list, destroying any bitfield pool that is no
 * longer in use.
 */
static void bitfield_pool_cleanup (void)
{
    dynamic_bitfield_pool *pool, *next_pool;

    pool = bitfield_pool;

    /* Walk all pools, freeing up those that are idle. */

    while (pool) {
	next_pool = pool->next;
	if (chunk_is_destroyable(pool->bitfield_chunk)) {
	    bitfield_pool_delink(pool);
	    chunk_destroy(pool->bitfield_chunk);
	    free(pool);
	}
	pool = next_pool;
    }
}


/*
 * bitfield_get_pool
 *
 * Returns a pointer to an appropriately sized bitfield pool given the
 * bit number.  Creates a new pool if there is no appropriately sized
 * pool available.
 *
 * Returns NULL if no memory is available.
 */
static dynamic_bitfield_pool *bitfield_get_pool (ulong bitnum)
{
    dynamic_bitfield_pool *pool, **prev_pool;
    ulong chunk_size;
    ulong field_width;
    
    prev_pool = &bitfield_pool;
    field_width = bitnum_to_pool_size(bitnum);

    /*
     * Walk the pools.  If we find one that's the right size, we're done.
     * If we find one that's too small, create a new one and insert it
     * there (they're in descending order by size).
     */
    for (pool = bitfield_pool; pool; pool = pool->next) {
	if (pool->bitfield_size == field_width) /* Right size! */
	    return(pool);
	if (pool->bitfield_size < field_width) /* Too small! */
	    break;
	prev_pool = &pool->next;
    }

    /* Didn't find one the proper size.  Create one and insert it. */

    pool = malloc_named(sizeof(dynamic_bitfield_pool), "Bitfield pool");
    if (!pool)
	return(NULL);

    pool->bitfield_size = field_width;

    /* Round up the chunk size to the next longword size. */

    chunk_size = ((pool->bitfield_size +
		   (sizeof(ulong) * BITMAP_SIZE) - 1) /
		  (sizeof(ulong) * BITMAP_SIZE)) * sizeof(ulong);

    pool->bitfield_chunk = chunk_create(chunk_size,
					DYNAMIC_BITFIELD_CHUNK_COUNT,
					CHUNK_FLAGS_DYNAMIC,
					NULL, sizeof(ulong),
					"Bitfields");
    if (!pool->bitfield_chunk) {
	free(pool);
	return(NULL);
    }
    bitfield_pool_link(pool, prev_pool); /* Link it in the proper spot. */
    return(pool);
}


/*
 * bitfield_destroy
 *
 * Free the bit array from a bitfield in preparation for its destruction.
 */
void bitfield_destroy (dynamic_bitfield *bitfield)
{
    dynamic_bitfield_pool *pool;

    if (!bitfield)
	return;

    if (!bitfield->bitfield_size)	/* Nothing there */
	return;

    /* Find the pool that matches this entry. */

    for (pool = bitfield_pool; pool; pool = pool->next) {
	if (pool->bitfield_size == bitfield->bitfield_size) {
	    break;
	}
    }

    /* Free the chunk. */

    if (pool) {
	chunk_free(pool->bitfield_chunk, bitfield->bits);
	bitfield->bits = NULL;
	bitfield->bitfield_size = 0;
    }

    /* Perhaps release the pool as well. */

    bitfield_pool_cleanup();
}


/*
 * bitfield_expand
 *
 * Expand a bitfield from its current size (which may be zero).  If there
 * is not an appropriately sized bitfield pool available, create a new one.
 *
 * If there is insufficient memory, we're out of luck--the protocol is
 * going to get slightly hosed.  In the interest of possible fragmentation
 * reduction we free the old bitfield anyhow.
 */
static void bitfield_expand (dynamic_bitfield *bitfield, ulong bitnum)
{
    dynamic_bitfield_pool *pool;
    ulong *newbits, newsize;

    /* Find the pool. */

    pool = bitfield_get_pool(bitnum);
    if (!pool)				/* uhoh */
	return;

    /* Grab some memory for the extended bits.  Copy in the old bits. */

    newbits = chunk_malloc(pool->bitfield_chunk);
    if (newbits) {
	memcpy(newbits, bitfield->bits, bitfield->bitfield_size / BITMAP_SIZE);
	newsize = pool->bitfield_size;
    } else {				/* No memory */
	newsize = 0;
    }
    
    /* Free up the old bits. */

    bitfield_destroy(bitfield);
    bitfield->bits = newbits;		/* NULL if we're out of memory */
    bitfield->bitfield_size = newsize;

    /* Try tossing the old pool, as it may be empty. */

    bitfield_pool_cleanup();
}


/*
 * bitfield_set
 *
 * Set a bit in a bitfield.  If the bitfield is too small, reallocate
 * it.  If the bitfield pool is too small, create a bigger one.
 * This version expects the caller to supply pointers to a bitfield
 * pool queue and a location where the current size is stored.
 */
void bitfield_set (ulong bitnum, dynamic_bitfield *bitfield)
{
    if (!bitfield)
	return;

    if (bitfield->bitfield_size <= bitnum) {	/* Need to expand */
	bitfield_expand(bitfield, bitnum);
	if (!bitfield->bits) {		/* Ran out of memory, bummer! */
	    return;
	}
    }
    SETBIT(bitnum, bitfield->bits);
}


/*
 * bitfield_clear
 *
 * Clear a bit in a bitfield.  If the bitfield is too small, skip it.
 */
void bitfield_clear (ulong bitnum, dynamic_bitfield *bitfield)
{
    if (!bitfield)
	return;

    if (bitfield->bitfield_size > bitnum) {	/* Bit exists */
	CLEARBIT(bitnum, bitfield->bits);
    }
}


/*
 * bitfield_check
 *
 * Test a bit in a bitfield.  Returns the bit value, or FALSE if the
 * bitfield is too small.
 */
boolean bitfield_check (ulong bitnum, dynamic_bitfield *bitfield)
{
    if (!bitfield)
	return(FALSE);

    if (bitfield->bitfield_size <= bitnum)	/* Bit doesn't exist */
	return(FALSE);

    return(CHECKBIT(bitnum, bitfield->bits));
}


/*
 * bitfield_setmask
 *
 * Set all bits in a bitfield from a bitfield mask.  Reallocate things
 * if the target is smaller than the mask.
 *
 * This version expects the caller to supply pointers to a bitfield
 * pool queue and a location where the current size is stored.
 */
void bitfield_setmask (dynamic_bitfield *mask, dynamic_bitfield *bitfield)
{
    if (!mask || !bitfield)
	return;

    if (!mask->bitfield_size)		/* No bits. */
	return;

    if (bitfield->bitfield_size < mask->bitfield_size) { /* Need to expand */
	bitfield_expand(bitfield, mask->bitfield_size - 1);
	if (!bitfield->bits) {		/* Ran out of memory, bummer! */
	    return;
	}
    }
    SETBITMASK(mask->bits, bitfield->bits, mask->bitfield_size);
}


/*
 * bitfield_clearmask
 *
 * Clear all bits in a bitfield based on a mask.  If the target and mask are
 * different sizes, clear only the overlap area.
 */
void bitfield_clearmask (dynamic_bitfield *mask, dynamic_bitfield *bitfield)
{
    ulong size;

    if (!mask || !bitfield)
	return;

    size = min(mask->bitfield_size, bitfield->bitfield_size);
    if (size) {
	CLEARBITMASK(mask->bits, bitfield->bits, size);
    }
}


/*
 * bitfield_find_first_clear
 *
 * Find the first clear bit in a bit array.  Returns TRUE if a bit was
 * found, or FALSE if not, and returns the bit number.
 */
boolean bitfield_find_first_clear (dynamic_bitfield *bitfield, ulong *bitnum)
{
    return(BITMASK_FIND_FIRST_CLEAR((uchar *)bitfield->bits,
				    bitfield->bitfield_size, bitnum));
}
