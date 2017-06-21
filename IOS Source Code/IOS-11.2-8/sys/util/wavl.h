/* $Id: wavl.h,v 3.2.62.1 1996/06/05 21:11:57 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/util/wavl.h,v $
 *------------------------------------------------------------------
 * Wrapper routines for multithreaded AVL trees.
 *
 * April 1995, Ronnie Kon
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: wavl.h,v $
 * Revision 3.2.62.1  1996/06/05  21:11:57  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:56:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  21:45:23  ronnie
 * CSCdi40277:  Crash in atm arp server code
 *         Instrument the wavl code to check for inserting an already
 *         inserted node, or deleting an uninserted node.
 *
 * Revision 2.1  1995/06/07  23:13:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_WAVL_H
#define HAVE_WAVL_H

#include "avl.h"		/* Bad form to include from an include file,
				 * but in this case I think it makes things
				 * much more clear
				 */



/*
 *  This represents a node on a wavl tree.  Structures using wavl's need
 *  to have one of these per thread at the top of their structures.
 */
typedef struct wavl_node_ {
    avl_node_type 	avl_node;	/* one per tree we're threaded on */
    boolean		threaded;	/* true if we are threaded now */
} wavl_node_type;

/*
 *  This represents an entire wavl tree
 */
typedef struct wavl_handle_ {
    int			num_threads;	/* number of trees we're threaded on */
    avl_node_type	**top;		/* one top per thread */
    avl_compare_type	*compare;	/* one compare per thread */
    boolean		*balancing_needed;	/* one balancing per thread */
    void		*(*find_blockptr)(wavl_node_type *);
				/* pointer to a routine to convert between
				 * the wavl_node_type and the top of the
				 * element
				 */
    boolean		initialized;
} wavl_handle;

typedef boolean (*wavl_walker_type)(wavl_node_type *, va_list);


/*
 * Routines to cast and pointer shift between avl and wavl node
 * pointers.  No, this is not hyper-pedantic.  Successful companies
 * concentrate on cast containment.
 */
static inline avl_node_type *
wavl_to_avl (wavl_node_type *node, int thread)
{
    if (node == NULL)
	return(NULL);
    return(avl_node_type *)((char *)node + thread * sizeof(wavl_node_type));
}
static inline wavl_node_type *
avl_to_wavl (avl_node_type *node, int thread)
{
    if (node == NULL)
	return(NULL);
    return(wavl_node_type *)((char *)node - thread * sizeof(wavl_node_type));
}

/*
 *  wavl_normalize
 *
 *  Because the underlying avl routines expect their pointers to be to
 *  the tops of the structures, the wavl comparison and walker
 *  routines must call this normalize routine to reset the pointers
 *  from the current avl_node_type to the start of the structure.
 */
static inline wavl_node_type *
wavl_normalize (avl_node_type *node, int thread)
{
    return (avl_to_wavl(node, thread));
}

boolean		wavl_init(wavl_handle *, int, void *(*f)(wavl_node_type *), ...);
void		wavl_finish(wavl_handle * const);
boolean		wavl_walk(const wavl_handle *, int, wavl_walker_type, ...);
wavl_node_type *wavl_search(const wavl_handle *, wavl_node_type *, int);
wavl_node_type *wavl_get_first(const wavl_handle *, int);
wavl_node_type *wavl_get_next(const wavl_handle *, wavl_node_type *, int);
wavl_node_type *wavl_insert(const wavl_handle *, wavl_node_type *);
wavl_node_type *wavl_delete(const wavl_handle *, wavl_node_type *);
wavl_node_type *wavl_insert_thread(const wavl_handle *, int, wavl_node_type *);
wavl_node_type *wavl_delete_thread(const wavl_handle *, int, wavl_node_type *);

#endif
