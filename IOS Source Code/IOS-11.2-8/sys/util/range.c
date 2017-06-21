/* $Id: range.c,v 3.4.52.2 1996/08/28 13:19:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/util/range.c,v $
 *------------------------------------------------------------------
 * range.c - Routines for doing range arithmetic.  
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: range.c,v $
 * Revision 3.4.52.2  1996/08/28  13:19:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.52.1  1996/04/12  16:53:02  mshannon
 * CSCdi36175:  range code doesnt check malloc return code
 * Branch: California_branch
 * Null pointer to checks added to range list routines
 *
 * Revision 3.4  1995/12/04  05:08:12  tli
 * CSCdi42930:  range list code cleanup
 * Clean up some more unnecessary casts.
 *
 * Revision 3.3  1995/12/02  09:05:58  tli
 * CSCdi42930:  range list code cleanup
 * Remove some unnecessary casts.
 *
 * Revision 3.2  1995/11/17  17:56:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "../util/msg_util.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "exception.h"
#include "../util/avl.h"
#include "../util/range_private.h"
#include "../util/range.h"


/*
 * range_compare
 *
 * Compare two range nodes and say GT, EQ or LT.
 */

static enum avl_compare 
range_compare (avl_node_type *a, avl_node_type *b)
{
    rangetype *arga = (rangetype *)a;
    rangetype *argb = (rangetype *)b;

    if (!a || !b) {
	errmsg(&msgsym(RANGEINCON, UTIL), 3, a);
	return(AVL_EQ);
    }
    if (arga->end < argb->start)
	return(AVL_LT);
    if (arga->start > argb->end)
	return(AVL_GT);
    return(AVL_EQ);
}

/*
 * Inlines to do type conversion to AVL routines.
 */

/*
 * range_search
 *
 * Wrapper for avl_search
 */

static inline rangetype *
range_search (rangelist *rl, rangetype *range)
{
    return((rangetype *) avl_search(&rl->top->avl_node,
				    &range->avl_node,
				    range_compare));
}

/*
 * range_get_first
 *
 * Wrapper for avl_get_first
 */

static inline rangetype *
range_get_first (rangelist *rl)
{
    return((rangetype *) avl_get_first(&rl->top->avl_node));
}

/*
 * range_get_next
 *
 * Wrapper for avl_get_next.
 */

static inline rangetype *
range_get_next (rangelist *rl, rangetype *r)
{
    return((rangetype *) avl_get_next(&rl->top->avl_node,
				  &r->avl_node, range_compare));
}

/*
 * range_get_quickest
 *
 * Wrapper for avl_get_quickest.
 */

static inline rangetype *
range_get_quickest (rangelist *rl)
{
    return((rangetype *)avl_get_quickest(&rl->top->avl_node));
}

/*
 * range_insert
 *
 * Wrapper for avl_insert.
 */
static inline rangetype *
range_insert (rangelist *rl, rangetype *new)
{
    boolean balancing;

    return((rangetype *) avl_insert((avl_node_type **)rl, &new->avl_node,
				&balancing, range_compare));
}

/*
 * range_delete
 *
 * Wrapper for avl_delete.
 */

static inline rangetype *
range_delete (rangelist *rl, rangetype *toast)
{
    boolean balancing;

    return((rangetype *) avl_delete((avl_node_type **)rl,
				&toast->avl_node, &balancing, 
				range_compare));
}

/*
 * rangelist_create
 * Create a new, empty range list and return a pointer to it.
 */

rangelist *rangelist_create (void)
{
    rangelist *rlptr;

    rlptr = malloc(sizeof(rangelist));
    if (rlptr == NULL)
	return(NULL);
    rlptr->top = NULL;			/* Ptr to ptr to tree */
    rlptr->thread = RANGE_NOTHREAD;
    return(rlptr);
}

/*
 * rangelist_destroy
 * Destroy a previously allocated rangelist.
 */

void rangelist_destroy (rangelist *rl)
{
    rangetype *node, *next;

    if (!rl)
	return;
    next = NULL;
    node = range_get_first(rl);
    while (node) {
	next = range_get_next(rl, node);
	node = range_delete(rl, node);
	if (!node) {
	    errmsg(&msgsym(RANGEINCON, UTIL), 2, node);
	    return;
	}
	free(node);
	node = next;
    }
    free(rl);
}

/*
 * range_malloc
 * Allocate a new range [start..end] and return it.  
 * It's not connected to anything.
 */

static rangetype *
range_malloc (uint start, uint end)
{
    rangetype *r;
    
    r = malloc(sizeof(rangetype));
    if (!r)
	return(NULL);
    r->start = start;
    r->end = end;
    r->thread = RANGE_NOTHREAD;
    return(r);
}

/*
 * rangelist_init
 * Initialize an existing range list with [start..end]
 */

void rangelist_init (rangelist *rl, uint start, uint end)
{
    rangetype *r;
    rangetype *node, *next;

    if (!rl) {  /* if no range list, exit */
	errmsg(&msgsym(RANGENULLINPUT, UTIL));
	return;
    }
    next = NULL;
    node = range_get_first(rl);
    while (node) {
	next = range_get_next(rl, node);
	node = range_delete(rl, node);
	if (!node) {
	    errmsg(&msgsym(RANGEINCON, UTIL), 1, node);
	    return;
	}
	free(node);
	node = next;
    }
    r = range_malloc(start, end);
    if (r) 
	range_insert(rl, r);
}

/*
 * rangelist_malloc_one
 * Get one entry from the range list.  Currenty this does nothing fancy.
 * Returns FALSE on failure.
 * Other routines depend on the order of elements that this returns.
 */

boolean rangelist_malloc_one (rangelist *rl, uint *retval)
{
    rangetype *r;
    rangetype temp;

    if (!rl)
	return(FALSE);

    r = NULL;

    /*
     * Check the thread of singles and see if we can find the first element
     * on this list.  If so, and it's a single, use it.
     */
    if (rl->thread != RANGE_NOTHREAD) {
	temp.start = temp.end = rl->thread;
	r = range_search(rl, &temp);
	if (!r) {
	    rl->thread = RANGE_NOTHREAD;
	} else {
	    rl->thread = r->thread;
	}
    }

    /*
     * If the thread didn't give us anything, look again, but be quick.
     */
    if (!r) {
	r = range_get_quickest(rl);
	if (!r)
	    return(FALSE);
    }
    *retval = r->start;
    if (r->start == r->end) {
	rl->thread = r->thread;
	r = range_delete(rl, r);
	if (!r) {
	    errmsg(&msgsym(RANGEINCON, UTIL), 4, *retval);
	    return(FALSE);
	}
	free(r);
    } else if (r->start > r->end) {
	errmsg(&msgsym(RANGEINCON, UTIL), 5, r->start);
	return(FALSE);
    } else {
	r->start++;
	if (r->start == r->end) {
	    r->thread = rl->thread;
	    rl->thread = r->start;
	}
    }
    return(TRUE);
}

/*
 * rangelist_malloc_block
 * Grab a contiguous block of numbers from a range.  Returns FALSE on
 * failure.
 */

boolean rangelist_malloc_block (rangelist *rl, uint count, uint *lower)
{
    rangetype *r;

    if (!rl)
	return(FALSE);
    r = range_get_first(rl);
    for ( ; r; r = range_get_next(rl, r)) {
	if ((r->end - r->start + 1) < count)
	    continue;
	*lower = r->start;
	if (r->start + count > r->end) {
	    r = range_delete(rl, r);
	    if (!r) {
		errmsg(&msgsym(RANGEINCON, UTIL), 10, *lower);
		return(FALSE);
	    }
	    free(r);
	} else {
	    r->start += count;
	}
	return(TRUE);
    }
    return(FALSE);
}

/*
 * rangelist_return_one
 * Return a number to the range.  Returns FALSE on error.
 */

boolean rangelist_return_one (rangelist *rl, uint value)
{
    rangetype temp;
    rangetype *curr, *next;
    
    if (!rl)
	return(FALSE);
    temp.start = temp.end = value - 1;
    curr = range_search(rl, &temp);
    if (curr) {
	if (curr->end == (value - 1)) {
	    curr->end++;
	} else {
	    errmsg(&msgsym(RANGEINCON, UTIL), 6, curr);
	    return(FALSE);
	}
    }
    next = range_get_next(rl, &temp);
    if (next) {
	if (next->start == (value + 1)) {
	    if (curr) {
		/*
		 * Coalesce.
		 */
		next = range_delete(rl, next);
		if (!next) {
		    errmsg(&msgsym(RANGEINCON, UTIL), 7, curr);
		    return(FALSE);
		}
		curr->end = next->end;
		free(next);
	    } else {
		next->start = value;
	    }
	    return(TRUE);
	}
    }
    if (!curr) {
	curr = range_malloc(value, value);
	if (!curr)
	    return(FALSE);
	curr = range_insert(rl, curr);
	if (!curr) {
	    errmsg(&msgsym(RANGEINCON, UTIL), 8, value);
	    return(FALSE);
	}
	curr->thread = rl->thread;
	rl->thread = value;
    }
    return(TRUE);
}

/*
 * rangelist_return_block
 * Return a block of numbers to the range.
 */

boolean rangelist_return_block (rangelist *rl, uint low, uint high)
{
    rangetype temp;
    rangetype *curr, *next;

    if (!rl)
	return(FALSE);
    temp.start = temp.end = low - 1;
    curr = range_search(rl, &temp);
    if (curr) {
	if (curr->end == (low - 1)) {
	    curr->end = high;
	} else {
	    errmsg(&msgsym(RANGEINCON, UTIL), 11, curr);
	    return(FALSE);
	}
    }
    next = range_get_next(rl, &temp);
    if (next) {
	if (next->start == (high + 1)) {
	    if (curr) {
		/*
		 * Coalesce.
		 */
		next = range_delete(rl, next);
		if (!next) {
		    errmsg(&msgsym(RANGEINCON, UTIL), 12, curr);
		    return(FALSE);
		}
		curr->end = next->end;
		free(next);
	    } else {
		next->start = low;
	    }
	    return(TRUE);
	}
    }
    if (!curr) {
	curr = range_malloc(low, high);
	if (!curr)
	    return(FALSE);
	curr = range_insert(rl, curr);
	if (!curr) {
	    errmsg(&msgsym(RANGEINCON, UTIL), 9, low);
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * show_rangelist
 * Print out contents of a rangelist.
 */

void show_rangelist (rangelist *rl)
{
    rangetype temp, *r;
    int bytecount;

    printf("\n  ");
    if (!rl)
	return;
    bytecount = 2;
    r = range_get_first(rl);
    temp.start = r->start;
    temp.end = r->end;
    while (r) {
	bytecount += printf("[%d", temp.start);
	if (temp.start != temp.end)
	    bytecount += printf("..%d]", temp.end);
	else
	    bytecount += printf("]");
	r = range_get_next(rl, &temp);
	if (r) {
	    temp.start = r->start;
	    temp.end = r->end;
	    bytecount += printf(", ");
	    if (bytecount > stdio->texti_info.width - 15) {
		printf("\n  ");
		bytecount = 2;
	    }
	}
    }
#if 0
    /* This is not memlock protected yet. */
    show_avl_tree(&rl->top->avl_node, range_compare);
#endif
}
