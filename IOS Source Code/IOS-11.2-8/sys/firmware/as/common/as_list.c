/* $Id: list.c,v 1.3 1996/09/09 05:32:04 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/list.c,v $
 *------------------------------------------------------------------
 * list.c -- NEAT Processor's Link List Management Routines.
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: list.c,v $
 * Revision 1.3  1996/09/09 05:32:04  jchlin
 * o       Unwrap the unnecessary interrupt protection in
 *         dlink_node_insert_prev() which may cause interrupt blockout
 *         when curr_node == NULL.
 *
 * Revision 1.2  1996/05/09  20:32:52  jchlin
 * The initial commit for double link list utility.
 * Ths double link list utility can be used to make
 * list management faster than the single link list.
 *
 * Revision 1.1  1996/04/05  00:34:37  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef T1_FIRMWARE
#include "t1_headers.h"
#else
#include "e1_headers.h"
#endif

void dlink_node_init (dlink_node_t *node)
{
    node->next = node->prev = node;
}

void dlink_node_insert_next (dlink_node_t *curr_node, dlink_node_t *new_node)
{
    dlink_node_t *next_node, *tail;
    ushort level_saved;
 
    level_saved = set_ipl(PRIO_FDL);

    if (curr_node && new_node) {
        /*
         * chain the new node or list
         */
	tail = (dlink_node_t *)new_node->prev;
	next_node = (dlink_node_t *) curr_node->next;
        next_node->prev = tail;
        tail->next = next_node;
        curr_node->next = new_node;
        new_node->prev  = curr_node;
    }
    else if (curr_node) {
        /*
         * Treat it as a new node
         */
        curr_node->next = curr_node;
        curr_node->prev = curr_node;
    }
    else  if (new_node) {
        /*
         * Treat it as a new node
         */
        new_node->next = new_node;
        new_node->prev = new_node;
    }
    reset_ipl(level_saved);
}

void dlink_node_insert_prev (dlink_node_t *curr_node, dlink_node_t *new_node)
{
    if (!curr_node)
        return;

    dlink_node_insert_next((dlink_node_t *)curr_node->prev, new_node);
}
 
void dlink_node_delete (dlink_node_t **header, dlink_node_t *node)
{
    dlink_node_t *next_node, *prev_node;
    ushort level_saved;
 
    if (!node || !header) 
        return;

    level_saved = set_ipl(PRIO_FDL);

    next_node = (dlink_node_t *)node->next;
    prev_node = (dlink_node_t *)node->prev;

    node->next = node->prev = node;
    if (node != next_node) {
        if (*header == node)
            *header = next_node;
        prev_node->next = next_node;
        next_node->prev = prev_node;
    }
    else 
        *header = NULL;
    reset_ipl(level_saved);
}

void dlink_list_delete (dlink_node_t **header, dlink_node_t *first_node, 
                        dlink_node_t *last_node)
{
    dlink_node_t *next_node, *prev_node;
    ushort level_saved;
 
    if (!first_node || !last_node || !header)
        return;     /* wrong pointers */

    level_saved = set_ipl(PRIO_FDL);

    next_node = (dlink_node_t *)last_node->next;
    prev_node = (dlink_node_t *)first_node->prev;

    first_node->prev = last_node;
    last_node->next = first_node;

    if (first_node != next_node) {
        /*
         * Assume that the list cannot contain the header.
         */
        if (*header == first_node)
            *header = next_node;
        prev_node->next = next_node;
        next_node->prev = prev_node;
    }
    else 
        *header = NULL;
    reset_ipl(level_saved);
}

