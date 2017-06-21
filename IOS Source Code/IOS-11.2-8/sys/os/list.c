/* $Id: list.c,v 3.2.62.2 1996/07/10 22:22:18 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/list.c,v $
 *------------------------------------------------------------------
 * List Management toolkit
 *
 * February 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: list.c,v $
 * Revision 3.2.62.2  1996/07/10  22:22:18  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:30:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:09:48  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:18:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:39:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:18:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/19  18:38:45  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.2  1995/07/24  06:45:51  hampton
 * Optimizer Fun and Games 101.  [CSCdi36956]
 *
 * Revision 2.1  1995/06/07 21:56:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "logger.h"
#include "list_private.h"


/*
 * Local defines
 */

ulong        list_ID = 0;
list_header  listQ;
chunk_type  *list_element_chunk; /* chunks for list elements */
chunk_type  *list_header_chunk;  /* chunks for list headers */


/*
 * Bogus debugging until the current mess gets sorted out.
 */
#define list_state_buginf if (list_debug_state) (*kernel_buginf)
static boolean list_debug_state = FALSE;


/*
 * list_enqueue_default
 *
 * The default list enqueue function
 */

static boolean list_enqueue_default (list_header  *list,
				     list_element *element,
				     void         *data)
{
    leveltype level = ALL_ENABLE;

    list_interrupt_lock(list, &level);
    
    /*
     * Check to see if we have to provide the element
     */
    if (!element)
	element = list_malloc_element_inline(list);

    if (!LIST_VALID(list) || LIST_FULL(list) ||
	(!element) || (element->list)) {
	/*
	 * If the list isn't valid, it's full, we don't have an element
	 * or if it's already enqueued, bail out
	 */
	list_interrupt_unlock(list, &level);
	return(FALSE);
    } else {
	/*
	 * Remember the data we're enqueuing
	 */
	LIST_SET_DATA(element) = data;
	
	/*
	 * Insert the element at the tail end of the list
	 */
	list_insert_element_inline(list, list->tail, element);

	list_interrupt_unlock(list, &level);
	return(TRUE);
    }
}

/*
 * list_dequeue_default
 *
 * The default list dequeue function
 */

static void *list_dequeue_default (list_header *list)
{
    list_element *element = NULL;
    leveltype     level   = ALL_ENABLE;
    void         *data    = NULL;

    list_interrupt_lock(list, &level);

    if (LIST_VALID(list) && !LIST_EMPTY(list)) {
	/*
	 * Off with the head!
	 */
	element = list->head;
	
	/*
	 * Remember the data pointer whilst we trash the element
	 */
	data = LIST_GET_DATA(element);

	/*
	 * Remove the element from the list and possibly hand it back
	 * to our element stash
	 */
	list_remove_element_inline(list, element);
	list_free_element_inline(list, element);
    }

    list_interrupt_unlock(list, &level);
    return(data);
}

/*
 * list_requeue_default
 *
 * The default list requeue function
 */

static boolean list_requeue_default (list_header  *list,
				     list_element *element,
				     void         *data)
{
    leveltype level = ALL_ENABLE;
    
    list_interrupt_lock(list, &level);

    /*
     * Check to see if we have to provide the element
     */
    if (!element)
	element = list_malloc_element_inline(list);

    if (!LIST_VALID(list) || LIST_FULL(list) ||
	(!element) || (element->list)) {
	/*
	 * Can't requeue. If we got an element, we might have to hand it
	 * back to our stash.
	 */
	list_free_element_inline(list, element);
	list_interrupt_unlock(list, &level);
	return(FALSE);
    } else {
	/*
	 * Remember the data we're enqueuing
	 */
	LIST_SET_DATA(element) = data;
	
	/*
	 * Add the element to the head
	 */
	list_insert_element_inline(list, NULL, element); 
	list_interrupt_unlock(list, &level);
	return(TRUE);
    }
}

/*
 * list_insert_default
 *
 * The default list insert function
 */

static boolean list_insert_default (list_header  *list,
				    list_element *element,
				    void         *data,
				    list_insert_func_t func)
{
    list_element *runner;
    int		  compare;
    leveltype     level = ALL_ENABLE;

    list_interrupt_lock(list, &level);

    /*
     * Check to see if we have to provide the element
     */
    if (!element)
	element = list_malloc_element_inline(list);

    if (LIST_VALID(list) && !LIST_FULL(list) &&
	(element) && !(element->list) && (func)) { 
	/*
	 * If we've got a valid list, an element that isn't already
	 * enqueued somewhere and a space on the list, attempt to
	 * find the parking space for this element
	 */

	/*
	 * Remember that data...
	 */
	LIST_SET_DATA(element) = data;
	
	/*
	 * Scan down the list looking for an insertion point
	 */
	FOR_ALL_ELEMENTS_IN_LIST(list, runner) {
	    /*
	     * Check to see if this element should be inserted before runner.
	     */
	    compare = (*func)(runner->data, element->data);
	    /*
	     * If compare is zero, the comparison function has marked these
	     * elements as identical. In this case, we don't insert the
	     * duplicate and return FALSE
	     */
	    if (compare == 0) {
		list_free_element_inline(list, element);
		list_interrupt_unlock(list, &level);
		return(FALSE);
	    }

	    /*
	     * If compare is greater than zero, we should insert the
	     * supplied element before the element we just compared
	     * against. Do this breaking out of the loop now.
	     */

	    if (compare > 0) {
		break;
	    }
	}

	/*
	 * If we're off the end, insert after the tail. Otherwise,
	 * insert before the one we've just found (if it's the head,
	 * prev will be NULL and the code will insert at the head).
	 */
	if (!runner)
	    runner = list->tail;
	else
	    runner = runner->prev;

	/*
	 * Insert our element into the spot we've found
	 */
	list_insert_element_inline(list, runner, element);
	list_interrupt_unlock(list, &level);
	return(TRUE);
    }

    list_interrupt_unlock(list, &level);
    return(FALSE);
}

/*
 * list_remove_default
 *
 * The default list remove function
 */

static void *list_remove_default (list_header  *list,
				  list_element *element,
				  void         *data)
{
    leveltype level = ALL_ENABLE;

    list_interrupt_lock(list, &level);

    if (LIST_VALID(list) && !LIST_EMPTY(list)) {
	/*
	 * If it's a valid list and the list isn't empty, then it's
	 * time to go for a stroll...
	 */
	if (!element && data) {
	    /*
	     * If data is non-NULL, we have to go for a walk down the
	     * list. Yeah, this doesn't scale but if you want it to
	     * be fast, pass in the element pointer.
	     */
	    FOR_ALL_ELEMENTS_IN_LIST(list, element) {
		if (element->data == data)
		    break;
	    }
	}
	
	/*
	 * If element is non-NULL, we've got a winner.
	 */
	if (element) {
	    /*
	     * Check and see if this element is in the list
	     */
	    if (element->list == list) {
		data = LIST_SET_DATA(element);
		list_remove_element_inline(list, element);
		list_free_element_inline(list, element);
	    } else {
		data = NULL;
	    }
	}
    }
	    
    list_interrupt_unlock(list, &level);

    /*
     * Fly. Be free.
     */
    return(data);
}

/*
 * list_excise
 *
 * The list excise function
 */

void list_excise (list_header  *list, void *corrupted)
{
    leveltype level = ALL_ENABLE;

    list_interrupt_lock(list, &level);

    if (LIST_VALID(list) && !LIST_EMPTY(list)) {
	list_excise_element_inline(list, corrupted);
    }
	    
    list_interrupt_unlock(list, &level);
}

/*
 * list_lookup_default
 *
 * The default list lookup function
 * It won't be fast ...
 */

static void *list_lookup_default (list_header  *list,
				    list_element *element,
				    void         *data,
				    list_lookup_func_t func)
{
    list_element *runner;
    leveltype     level = ALL_ENABLE;

    list_interrupt_lock(list, &level);

    /*
     * Check to see if we have to provide the element
     */
    if (!element)
	element = list_malloc_element_inline(list);

    if ((element) && (func)) { 
	/*
	 * If we've got a valid list, an element that isn't already
	 * enqueued somewhere and a space on the list, attempt to
	 * find the parking space for this element
	 */

	/*
	 * Remember that data...
	 */
	LIST_SET_DATA(element) = data;
	
	/*
	 * Scan down the list looking for a match
	 */
	FOR_ALL_ELEMENTS_IN_LIST(list, runner) {
	    /*
	     * Check to see if this element matches the runner.
	     */
	    if ( (*func)(runner->data, element->data) == 0) {
	    /*
	     * If the comparison function returns zero, it has marked
	     * these elements as identical. In this case, we see if we
	     * allocated the elemenent inline, dispose of it if
	     * necessary, and return the data.
	     */
		list_free_element_inline(list, element);
		list_interrupt_unlock(list, &level);
		return(LIST_GET_DATA(runner));
	    }
	}

	/*
	 * If we're off the end, we missed it.  Return NULL.
	 */
	list_free_element_inline(list, element);
	list_interrupt_unlock(list, &level);
	return(NULL);
    }

    list_interrupt_unlock(list, &level);
    return(NULL);
}

/*
 * list_move
 *
 * Move an element from one list to another.  This is used by the
 * scheduler to guarantee atomic moves.  DO NOT CHANGE THE VOLATILE
 * ASSIGNMENT.  The optimizer wants to read the list pointer before the
 * interrupt lockout, and then use the variable inside the interrupt
 * lockout even though it may have changed.  The volatile assignment
 * forces the list pointer to be read under interrupt lock so that it is
 * guaranteed to be correct.
 */
void list_move (list_header  *new_list,
		list_element *element)
{
    leveltype level = ALL_ENABLE;
    volatile list_element *volatile_element;

    if (!element)
	return;
    volatile_element = element;
    if (LIST_VALID(element->list) &&
	LIST_VALID(new_list) &&
	!LIST_EMPTY(element->list)) {
	    /*
	     * Remove the element from the old list and insert it into
	     * the new one.
	     */
	if ((element->list->flags & LIST_FLAGS_INTERRUPT_SAFE) ||
	    (new_list->flags & LIST_FLAGS_INTERRUPT_SAFE)) {
	    level = disable_interrupts();
	    list_remove_element_inline(volatile_element->list, element);
	    list_insert_element_inline(new_list, new_list->tail, element);
	    enable_interrupts(level);
	} else {
	    list_remove_element_inline(element->list, element);
	    list_insert_element_inline(new_list, new_list->tail, element);
	}
    }
}

/*
 * list_insert_element
 *
 * Inserts "element" after "previous in the given list.
 */

void list_insert_element (list_header  *list,
			  list_element *previous,
			  list_element *element)
{
    list_insert_element_inline(list, previous, element);
}

/*
 * list_remove_element
 *
 * Removes "element" from the given list
 */

void list_remove_element (list_header  *list,
			  list_element *element)
{
    list_remove_element_inline(list, element);
}


/*
 * list_action_vectors_default
 *
 * Default structure for list action routines
 */

static list_action_t list_action_vectors_default = {
    list_enqueue_default,
    list_dequeue_default,
    list_requeue_default,
    list_insert_default,
    list_remove_default,
    list_lookup_default,
};

/*
 * list_info_default
 */
static boolean list_info_default (
    list_header *hdr,
    list_element *element,
    char * dummy)
{
    return(FALSE);	/* Do nothing. */
}

/*
 * list_create
 *
 * Initialize a new list structure. If list is NULL, the function
 * will malloc space to hold the structure.
 *
 */

list_header *list_create (list_header   *list,
                          ushort         maximum,
			  char *const    name,
			  ushort         flags)
{
    if (!list) {
	/*
	 * The calling function was cheap and wants us to provide
	 * the space for the list structure. Be obliging.
	 */
	list = chunk_malloc(list_header_chunk);

	/*
	 * We asked, but none was given. Oh well...
	 */
	if (!list) {
	    return(NULL);
	}

	list->flags |= LIST_FLAGS_MALLOC;
    } else {
        /*
         * Ensure that the slate is clean before starting.
         */
        memset(list, 0, sizeof(list_header));
    }
    
    /*
     * Set key and default list parameters
     */
    list->ID      = list_ID++;

    list->maximum = maximum;
    list->count   = 0;
    list->head    = NULL;
    list->tail    = NULL;

    list->name    = (name != NULL) ? name : "(unknown)";
    list->flags  |= (flags & LIST_FLAGS_USER_MASK);

    /*
     * Set the default vectors
     */
    list_set_info(list, NULL);
    list_set_action(list, NULL);
    
    /*
     * Add our new list to the master list. If no list name
     * was specified, this list wants to remain quiet and low
     * key. Honor that request and don't insert it into the listQ.
     */
    if (name) {
	if (!list_enqueue(&listQ, &list->element, list)) {
	    /*
	     * Oops. Something went horribly wrong. Recover.
	     */
	    list_state_buginf("\nList(%d): Create - failure!"
			      "failed to insert list 0x%x", list->ID, list);
	
	    /*
	     * If we allocated it, we've got to hand it back
	     */
	    if (list->flags & LIST_FLAGS_MALLOC)
		chunk_free(list_header_chunk, list);

	    return(NULL);
	}
    }

    /*
     * Tell the world it's ready for use
     */
    list->flags |= LIST_FLAGS_VALID;

    list_state_buginf("\nList(%d): Create - list 0x%x, flags 0x%x, name %s",
		      list->ID, list, list->flags, list->name);

    return(list);
}


/*
 * list_destroy
 *
 */

void list_destroy (list_header *list)
{
    list_state_buginf("\nList(%d): Destroy - list 0x%x", list->ID, list);

    /*
     * Attempt to remove the list element from the master list
     */
    if (!list_remove(&listQ, &list->element, NULL)) {
	list_state_buginf("\nList(%d): Destroy - failure!"
			  "failed to remove list 0x%x", list->ID, list);
	return;
    }

    if (list->flags & LIST_FLAGS_MALLOC) {
	/*
	 * Goodbye to the whole kitchen sink
	 */
	chunk_free(list_header_chunk, list);
    } else {
	/*
	 * Remove the list init flag
	 */
	list->flags = 0;
    }
}


/*
 * list_set_automatic
 *
 * Enable or disable the use of the list chunk manager for element
 * enqueue blocks
 */

void list_set_automatic (list_header *list, boolean enabled)
{
    if (!LIST_EMPTY(list))
	list_state_buginf("\nList(%d): Change State - warning!"
			  " non-empty list 0x%x", list->ID, list);
    
    if (enabled) {
        list->flags |= LIST_FLAGS_AUTOMATIC;
    } else {
        list->flags &= ~LIST_FLAGS_AUTOMATIC;
    }
}


/*
 * list_set_interrupt_safe
 *
 * Enable or disable interrupt protection for list operations
 */

void list_set_interrupt_safe (list_header *list, boolean enabled)
{
    if (!LIST_EMPTY(list))
	list_state_buginf("\nList(%d): Change State - warning!"
			  " non-empty list 0x%x", list->ID, list);
    
    if (enabled) {
        list->flags |= LIST_FLAGS_INTERRUPT_SAFE;
    } else {
        list->flags &= ~LIST_FLAGS_INTERRUPT_SAFE;
    }
}


/*
 * list_set_info
 *
 * Set the info vector for a list
 */

boolean list_set_info (list_header *list, list_info_t info)
{
    /*
     * Make sure we're sane
     */
    if (!list)
	return(FALSE);

    /*
     * Organic fix. If someone specifies NULL as the function vector
     * use return_false().
     */
    if (!info)
	info = list_info_default;

    /*
     * Set our vector
     */
    list->info = info;
    
    return(TRUE);
}


/*
 * list_get_info
 *
 * Get the info vector for a list
 */

list_info_t list_get_info (list_header *list)
{
    /*
     * Make sure we're sane
     */
    if (!list)
	return(NULL);

    return(list->info);
}


/*
 * list_set_action
 *
 * Set the action vectors for a list
 */

boolean list_set_action (list_header *list, list_action_t *action)
{
    /*
     * Make sure we're sane
     */
    if (!list)
	return(FALSE);

    /*
     * Organic fix. If someone specifies NULL as the action vector
     * pointer, we use the default vector block.
     */
    if (!action)
	action = &list_action_vectors_default;

    /*
     * Set our vector
     */
    list->action = action;
    
    return(TRUE);
}


/*
 * list_get_action
 *
 * Get the action vectors for a list
 */

list_action_t *list_get_action (list_header *list)
{
    /*
     * Make sure we're sane
     */
    if (!list)
	return(NULL);

    return(list->action);
}



/*
 * list_init
 *
 * Turn the key in the list ignition. Or something.
 */

void list_init (void)
{
    /*
     * Create the master list
     */
    list_create(&listQ, 0, NULL, 0);
}


/*
 * list_memory_init
 *
 * Initialize the chunk pools for automatic elements
 */

void list_memory_init (void)
{
    /*
     * Create a chunk for the tiny (16 byte) queue elements.
     */
    list_element_chunk = chunk_create(sizeof(list_element),
				      LIST_ELEMENT_MAX_CHUNKS,
				      CHUNK_FLAGS_DYNAMIC,
				      NULL, 0,
				      "List Elements");
    if (list_element_chunk == NULL) {
	crashdump(0);
        return;
    }

    /*
     * Create a chunk for the slightly less tiny, but still somewhat teeny,
     * list header structures
     */
    list_header_chunk = chunk_create(sizeof(list_header),
				     LIST_HEADER_MAX_CHUNKS,
				     CHUNK_FLAGS_DYNAMIC,
				     NULL, 0,
				     "List Headers");
    if (list_header_chunk == NULL) {
        chunk_destroy(list_element_chunk);
        crashdump(0);
    }
}
