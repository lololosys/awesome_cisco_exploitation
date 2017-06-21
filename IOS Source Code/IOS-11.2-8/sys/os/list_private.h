/* $Id: list_private.h,v 3.2 1995/11/17 18:48:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/list_private.h,v $
 *------------------------------------------------------------------
 * list_private.h - Private definitions and prototypes for list manager
 *
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: list_private.h,v $
 * Revision 3.2  1995/11/17  18:48:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:56:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LIST_PRIVATE_H__
#define __LIST_PRIVATE_H__

#include "chunk.h"


/*
 * Defines
 */
#define LIST_ELEMENT_MAX_CHUNKS     50
#define LIST_HEADER_MAX_CHUNKS      50


/*
 * Inlines
 */


/*
 * Inlines for managing list element and header chunks
 */

extern chunk_type  *list_element_chunk;
extern chunk_type  *list_header_chunk;

static inline
list_element *list_malloc_element_inline (list_header *list)
{
    if (list->flags & LIST_FLAGS_AUTOMATIC)
	return(chunk_malloc(list_element_chunk));
    else
	return(NULL);
}

static inline
void list_free_element_inline (list_header  *list,
			       list_element *element)
{
    if (list->flags & LIST_FLAGS_AUTOMATIC)
	chunk_free(list_element_chunk, element);
}


/*
 * list_interrupt_lock
 *
 * Protect against interrupts if required
 *
 */

static inline void list_interrupt_lock (list_header *list,
					leveltype *level)
{
    if (list->flags & LIST_FLAGS_INTERRUPT_SAFE)
        *level = disable_interrupts();
}


/*
 * list_interrupt_unlock
 *
 * Free protection from interrupts
 *
 */

static inline void list_interrupt_unlock (list_header *list,
					  leveltype *level)
{
    if (list->flags & LIST_FLAGS_INTERRUPT_SAFE)
        enable_interrupts(*level);
}


/*
 * Useful list manipulation wrappers
 */

/*
 * list_insert_element_inline
 *
 * Inserts list element "new" after list element "current"
 */

static inline void list_insert_element_inline (list_header  *list,
					       list_element *current,
					       list_element *new)
{
    if (!current) {
	/*
	 * Insert new at the head of the list
	 */
	new->next  = list->head;
	list->head = new;
	new->prev  = NULL;

    } else {
	/*
	 * Insert new after current
	 */
	new->next     = current->next;
	current->next = new;
	new->prev     = current;
    }

    /*
     * If we're not the tail, attach the backpointer
     */
    if (new->next)
	new->next->prev = new;

    /*
     * If this is the tail, update the pointer.
     */
    if (current == list->tail)
	list->tail = new;

    new->list = list;
    new->list->count++;
}


/*
 * list_remove_element_inline
 *
 * Removes list element "current" from "list".
 */

static inline void list_remove_element_inline (list_header  *list,
					       list_element *current)
{
    /*
     * Remove "current" from list
     */
    if (current->next)
	current->next->prev = current->prev;

    if (current->prev)
	current->prev->next = current->next;

    if (current == list->head)
	list->head = current->next;

    if (current == list->tail)
	list->tail = current->prev;

    current->list->count--;
    current->next = current->prev = NULL;
    current->list = NULL;
}


/*
 * list_excise_element_inline
 *
 * Removes list element "current" from "list".  This routine assumes
 * the element is corrupted and must be removed the hard way.
 */

static inline void list_excise_element_inline (list_header  *list,
					       void *corrupted)
{
    list_element *predecessor, *successor;

    /*
     * Remove "current" from list
     */
    predecessor = list->head;
    if (predecessor == corrupted)
	predecessor = NULL;
    while (predecessor) {
	if (predecessor->next == corrupted)
	    break;
	predecessor = predecessor->next;
    }

    successor = list->tail;
    if (successor == corrupted)
	successor = NULL;
    while (successor) {
	if (successor->prev == corrupted)
	    break;
	successor = successor->prev;
    }

    if (successor)
	successor->prev = predecessor;

    if (predecessor)
	predecessor->next = successor;

    if (list->head == corrupted)
	list->head = successor;

    if (list->tail == corrupted)
	list->tail = predecessor;

    list->count--;
}


/*
 * Externs
 */

extern ulong       list_ID; 
extern list_header listQ;

#endif
