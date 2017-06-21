/* $Id: wavl.c,v 3.2.62.4 1996/09/07 02:08:29 snyder Exp $
 * $Source: /release/112/cvs/Xsys/util/wavl.c,v $
 *------------------------------------------------------------------
 * Wrapper routines for multithreaded AVL trees
 *
 * April 1995, Ronnie Kon
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: wavl.c,v $
 * Revision 3.2.62.4  1996/09/07  02:08:29  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/28  13:19:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/06/05  21:11:55  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:31:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:00:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:45:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  21:45:22  ronnie
 * CSCdi40277:  Crash in atm arp server code
 *         Instrument the wavl code to check for inserting an already
 *         inserted node, or deleting an uninserted node.
 *
 * Revision 2.1  1995/06/07  23:13:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  WAVL: What the hell is going on here?
 *
 *  The wavl structure is a Wrapped AVL tree.  It presents two advantages
 *  over the raw AVL routines: first, it gives you a handle structure which
 *  holds the parameters the AVL routines need, which makes bookkeeping
 *  easier; and second, (and the reason this ever got written) is that it
 *  allows you to have an item threaded onto multiple AVL trees, for cases
 *  where you must search for items sorted on more than one key.
 *
 *  One drawback of AVL trees is that you cannot have multiple elements with
 *  the same key value on the tree.  This means that if you want to have your
 *  nodes threaded onto 3 trees based on 3 keys, each of these keys must
 *  always be unique.  (Practical example: you can thread based on IP
 *  addresses, but then every entry must have an IP address.  You cannot
 *  use some special value like 0.0.0.0 to represent no IP address, because
 *  you won't be able to have multiple 0.0.0.0 threaded onto the IP AVL tree).
 *
 *
 *  Cool.  So how do I use this?
 *
 *  The very first item in your data structure must be an array of
 *  wavl_node_type (one element per thread you want), which contains
 *  the stuff the wavl routines need to reference.  You will also need
 *  a wavl_handle for every WAVL tree you want to have.  You will take
 *  all actions by passing the handle for the WAVL tree and the
 *  wavl_node_type for the element you want added or deleted.
 *
 *  You first must call wavl_init(), passing in the wavl_handle you want
 *  to initialize, the number of AVL trees you want under this WAVL tree,
 *  and a comparison routine for each of the AVL trees.  If your
 *  wavl_node_type is not at the very top of the block you malloc()'d, you
 *  must also pass a pointer in to a routine which will convert between the
 *  two.  This is so the wavl_walk routine can mem_lock() the element before
 *  calling the walker routine.
 *
 *  The wavl_insert() routine will always insert onto every AVL tree.  If for
 *  any reason any of the inserts fails, it will go back and delete all of
 *  the inserts it had already accomplished.  You are guaranteed all-or-nothing
 *  insertion.
 *
 *  You can also insert or delete from a specific AVL tree, using the
 *  wavl_insert_thread() and wavl_delete_thread() routines.  If you do this
 *  you can leave the set of trees in a strange state.  On the other hand,
 *  there are applications where this is the correct thing to do.
 *
 *  If you are only changing one of the multiple key values, it is *always*
 *  a good idea to use the wavl_delete_thread() routine to delete the node
 *  from that specific AVL tree, and then wavl_insert_thread() to insert
 *  with the new key.  Deleting from all trees and then reinserting is a
 *  monumental waste of CPU cycles.
 *
 *  There is a wavl_walk() which will walk one of the AVL trees, a wavl_search
 *  which searches one of the AVL trees.  When you are done with a tree, you
 *  can call wavl_finish() to free up all its resources (this is important
 *  as wavl_init() calls malloc()--if you don't wavl_finish() you will have
 *  a memory leak.
 *
 *  In the comparison routines you register via wavl_init(), and the
 *  walker routines you have called via wavl_walk(), you must first
 *  call wavl_normalize() to readjust the pointer back to the beginning of
 *  your data structure.
 *
 *  It is a *VERY* good idea to front-end all the routines which return
 *  a (void *) or a (wavl_node_type *) with a conversion routine which you
 *  supply.  That way you preserve strict typechecking. */

#include "master.h"
#include <stdarg.h>
#include "logger.h"
#include "../util/msg_util.c"		/* Not a typo, see logger.h */
#include "wavl.h"

#ifdef GLOBAL_DEBUG
static boolean wavl_debug = FALSE;	/* only changeable through debugger */
#endif

static const char uninit_msg[] = "attempt to reference an uninitialized wavl tree";
static const char null_handle_msg[] = "received a NULL handle";
static const char already_threaded_msg[] = "attempt to re-add a node to a tree";
static const char not_threaded_msg[] ="attempt to remove an unthreaded node from a tree";


/*
 *  mark_threaded
 *
 *  Flag a node as being threaded onto a given avl tree.  This is broken out
 *  so it is easier to debug when things go wrong.  Not that that ever happens,
 *  of course.
 */
static inline void
mark_threaded (wavl_node_type *w, boolean value)
{
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: node %x gets threaded flag value %d", w, value);
#endif
    w->threaded = value;
}

/*
 *  def_find_block
 *
 *  This converts a wavl pointer to the pointer to the top of its block.  It
 *  will be used by all wavl trees which do not pass their own routine in.
 */
static void *
def_find_block (wavl_node_type *w)
{
    return (void *)w;
}

/*
 *  wavl_init
 *
 *  Initialize a wrapped-AVL tree.  You must pass in a handle, the number of
 *  trees this wrapper will thread onto, a pointer to a routine which returns a
 *  pointer to the top of the memory block (or NULL if the wavl_node_type
 *  should be treated as the top of the memory block), and a list of comparison
 *  routines, one per thread.
 *
 *  Unfortunately, this routine must malloc.
 *
 *  This returns FALSE on success, TRUE on failure.
 */
boolean
wavl_init (wavl_handle *handle,
	   int num_threads,
	   void *(*findblock)(wavl_node_type *),
	   ...)
{
    int ix;
    va_list argptr;

#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: init handle %x, %d threads", handle, num_threads);
#endif

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(TRUE);
    }
    
    if (handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), "attempt to initialize an initialized"
	       " wavl tree");
	return(TRUE);
    }
    
    va_start(argptr, findblock);
    
    /* do the malloc's */
    if ((handle->top = malloc(sizeof(*handle->top) * num_threads)) == NULL)
	return(TRUE);
    if ((handle->compare =
	 malloc(sizeof(*handle->compare) * num_threads)) == NULL) {
	free(handle->top);
	handle->top = NULL;
	return(TRUE);
    }
    if ((handle->balancing_needed =
	 malloc(sizeof(*handle->balancing_needed) * num_threads)) == NULL) {
	free(handle->top);
	free(handle->compare);
	handle->top = NULL;
	handle->compare = NULL;
	return(TRUE);
    }

    handle->num_threads = num_threads;
    for (ix = 0 ; ix < num_threads ; ix++) {
	handle->compare[ix] = va_arg(argptr, avl_compare_type);
	handle->top[ix] = NULL;
	handle->balancing_needed[ix] = FALSE;
    }
    handle->find_blockptr = (findblock == NULL) ? def_find_block : findblock;
    handle->initialized = TRUE;
    va_end(argptr);

    return(FALSE);
}

/*
 *  wavl_finish
 *
 *  Called to free up any resources associated with this wavl tree.
 */
void
wavl_finish (wavl_handle * const handle)
{
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: finish handle %x", handle);
#endif    
    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return;
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return;
    }

    free(handle->top);
    free(handle->compare);
    free(handle->balancing_needed);
    handle->top = NULL;
    handle->compare = NULL;
    handle->balancing_needed = NULL;
    handle->initialized = FALSE;
}

/*
 *  wavl_walk
 *  wavl_do_walk
 *
 *  This will walk one of the wrapped-AVL trees, in order, executing the
 *  function at each node.  The walk will stop if the function returns FALSE.
 *
 *  This routine returns TRUE on successful completion of the walk, FALSE if
 *  it was aborted.
 *
 *  Unfortunately, we have to know about the insides of the AVL structure
 *  to do this efficiently.
 *
 *  Note: It is possible that the node being processed could be freed
 *  while a subtree is being traversed or while "proc" is processing
 *  the node. For example, if the contents of the node are printed the
 *  calling task could block allowing another task to alter the tree.
 *  For this reason the memory block associated with the specified tree
 *  node is mem_locked() while it it processed.
 */

static boolean
wavl_do_walk (const wavl_handle *handle,
	      int thread,
	      wavl_walker_type proc,
	      avl_node_type *a_element,
	      va_list argptr)
{
    wavl_node_type   *w_element = avl_to_wavl(a_element, thread);
    void             *block_ptr = (handle->find_blockptr)(w_element);

    mem_lock(block_ptr);

    if (a_element->left) {
        if (!wavl_do_walk(handle, thread, proc, a_element->left, argptr) ||
            (mem_refcount(block_ptr) == 1)) {

            free(block_ptr);
            return(FALSE);
        }
    }

    if (!(*proc)(w_element, argptr)  ||  (mem_refcount(block_ptr) == 1)) {
        free(block_ptr);
        return(FALSE);
    }

    if (a_element->right) {
        if (!wavl_do_walk(handle, thread, proc, a_element->right, argptr) ||
            (mem_refcount(block_ptr) == 1)) {

            free(block_ptr);
            return(FALSE);
        }
    }

    free(block_ptr);
    return(TRUE);
}

/*
 * wavl_walk
 *
 * Wrapper for wavl_do_walk to prepare the arglist
 *   so that it doesn't get recursively recomputed.
 */
boolean
wavl_walk (const wavl_handle *handle, int thread, wavl_walker_type proc, ...)
{
    va_list argptr;
    boolean retval;

#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: walk   handle %x, thread %d", handle, thread);
#endif    

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(FALSE);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(FALSE);
    }

    if (handle->top[thread] == NULL) {
	/* we just traversed an empty tree */
	return(TRUE);
    }

    va_start(argptr, proc);		/* Get remaining args */
    retval = wavl_do_walk(handle, thread, proc, handle->top[thread], argptr);
    va_end(argptr);
    return(retval);
}

/*
 *  wavl_search
 *
 *  Search the indicated tree for the passed goal.  If no matching
 *  node exists, return NULL.  Goal is a dummy node which the caller
 *  must allocate and salt with the desired search value.
 */
wavl_node_type *
wavl_search (const wavl_handle	*handle,
	     wavl_node_type	*goal,
	     int		thread)
{
    avl_node_type	*result;
    
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: search  handle %x, goal %x, thread %d", handle, goal,
	       thread);
#endif    

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    result = avl_search(handle->top[thread],
			wavl_to_avl(goal, thread),
			handle->compare[thread]);
    return(avl_to_wavl(result, thread));
}

/*
 *  wavl_get_first
 *
 *  Return the first node in the specified tree, or NULL if it is empty
 */
wavl_node_type *
wavl_get_first (const wavl_handle *handle, int thread)
{
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: get first   handle %x, thread %d", handle, thread);
#endif    

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }
    return(avl_to_wavl(avl_get_first(handle->top[thread]), thread));
}

/*
 *  wavl_get_next
 *
 *  What can be more exciting than the avl_get_next function?  Why, the
 *  mind-numbingly complex wavl_get_next function.  The algorithm for this
 *  is so complex that I have masked it in apparantly simple code so you
 *  won't feel bad that you can't understand it.
 */
wavl_node_type *
wavl_get_next (const wavl_handle *handle, wavl_node_type *node, int thread)
{
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: get next    handle %x, node %x, thread %d", handle, node, thread);
#endif    

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    return(avl_to_wavl(avl_get_next(handle->top[thread],
				    wavl_to_avl(node, thread),
				    handle->compare[thread]), thread));
}

/*
 *  wavl_insert
 *
 *  Insert an item on a wavl tree.  Return a pointer to the node or NULL if
 *  there is a failure.
 */
wavl_node_type *
wavl_insert (const wavl_handle *handle, wavl_node_type *node)
{
    int ins_thread;
    avl_node_type *result;

#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: Insert  handle %x, node %x", handle, node);
#endif
    
    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    /*
     *  Now go add to all the threads
     */
    for (ins_thread = 0 ; ins_thread < handle->num_threads ; ins_thread++) {
	/*
	 *  Really bad things can happen if we try to thread an already
	 *  threaded node
	 */
	if (node[ins_thread].threaded) {
	    errmsg(&msgsym(TREE, UTIL), already_threaded_msg);
	    result = NULL;
	} else {
	    result = avl_insert(&handle->top[ins_thread],
				&node[ins_thread].avl_node,
				&handle->balancing_needed[ins_thread],
				handle->compare[ins_thread]);
	}
	if (result == NULL) {
	    /*
	     *  Failure.  Remove us from the trees we are already threaded
	     *  onto.
	     */
	    int del_thread;

	    for (del_thread = 0 ; del_thread < ins_thread ; del_thread++) {
		avl_node_type *del_node;
		/*
		 *  We'll errmsg out a warning in the failure to delete
		 *  case, but there is really nothing else we can do.  This
		 *  probably means that the comparison routine does not
		 *  compare two values consistantly.
		 */
		del_node = avl_delete(&handle->top[del_thread],
				      &node[del_thread].avl_node,
				      &handle->balancing_needed[del_thread],
				      handle->compare[del_thread]);
		if (del_node == NULL)
		    errmsg(&msgsym(TREE, UTIL), "failure to delete a node");
		else
		    mark_threaded(&node[del_thread], FALSE);
	    }
	    return(NULL);
	}
	mark_threaded(&node[ins_thread], TRUE);
    }
    return(node);
}

/*
 *  wavl_delete
 *
 *  Remove us from all the trees.  Return a pointer to the node if at
 *  least one of the avl_delete's was successful, NULL if *all* the
 *  deletes fail.
 */
wavl_node_type *
wavl_delete (const wavl_handle *handle, wavl_node_type *node)
{
    int del_thread;
    boolean had_success;
    avl_node_type *result;

#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: Delete  handle %x, node %x", handle, node);
#endif

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    had_success = FALSE;
    for (del_thread = 0 ; del_thread < handle->num_threads ; del_thread++) {
	if (!node[del_thread].threaded)
	    continue;		/* skip this one */
	result = avl_delete(&handle->top[del_thread],
			    &node[del_thread].avl_node,
			    &handle->balancing_needed[del_thread],
			    handle->compare[del_thread]);
	if (result != NULL) {
	    had_success = TRUE;
	    mark_threaded(&node[del_thread], FALSE);
	}
    }

    return(had_success ? node : NULL);
}

/*
 *  wavl_insert_thread
 *
 *  Insert an element onto only one of the multiple AVL trees.
 */
wavl_node_type *
wavl_insert_thread (const wavl_handle	*handle,
		    int 		 thread,
		    wavl_node_type 	*node)
{
    wavl_node_type *result;
    
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: Insert  handle %x, node %x  (Only thread %d)",
	       handle, node, thread);
#endif

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    if (node[thread].threaded) {
	errmsg(&msgsym(TREE, UTIL), already_threaded_msg);
	return(NULL);
    }
    
    result = avl_to_wavl(avl_insert(&handle->top[thread],
				    &node[thread].avl_node,
				    &handle->balancing_needed[thread],
				    handle->compare[thread]), thread);
    if (result != NULL)
	mark_threaded(&node[thread], TRUE);

    return result;
}

/*
 *  wavl_delete_thread
 *
 *  Delete an element from only one of the multiple AVL trees.
 */
wavl_node_type *
wavl_delete_thread (const wavl_handle	*handle,
		    int 		 thread,
		    wavl_node_type	*node)
{
    wavl_node_type *result;
    
#ifdef GLOBAL_DEBUG
    if (wavl_debug)
	buginf("\nWAVL: Delete  handle %x, node %x  (Only thread %d)",
	       handle, node, thread);
#endif

    /* this should never happen */
    if (handle == NULL) {
	errmsg(&msgsym(TREE, UTIL), null_handle_msg);
	return(NULL);
    }
    
    if (!handle->initialized) {
	errmsg(&msgsym(TREE, UTIL), uninit_msg);
	return(NULL);
    }

    if (!node[thread].threaded) {
	errmsg(&msgsym(TREE, UTIL), not_threaded_msg);
	return(NULL);
    }
    
    result = avl_to_wavl(avl_delete(&handle->top[thread],
				    &node[thread].avl_node,
				    &handle->balancing_needed[thread],
				    handle->compare[thread]), thread);
    if (result != NULL)
	mark_threaded(&node[thread], FALSE);
    return result;
}
