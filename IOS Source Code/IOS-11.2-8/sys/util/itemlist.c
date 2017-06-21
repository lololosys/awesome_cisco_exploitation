/* $Id: itemlist.c,v 3.3.30.1 1996/03/18 22:30:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/util/itemlist.c,v $
 *------------------------------------------------------------------
 * itemlist.c -- Functions to manipulate itemlists and item
 *		 descriptors.
 *
 * October, 1995 David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: itemlist.c,v $
 * Revision 3.3.30.1  1996/03/18  22:30:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:59:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  07:30:30  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  00:46:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:43:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/10  13:16:51  dstine
 * Add placeholders for Arkansas
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface.h"
#include "itemlist.h"
#include "logger.h"
#include "../os/free.h"

boolean
itemlist_dump (itemlist *list, char *string)
{
    int			count;
    unsigned long	i;
    itemdesc		*id;

    if (list == NULL)
	return (FALSE);

    if (string)
	printf("\n%s", string);
    count = list->item_count;
    id = &list->items[0];
    i = (count == -1) ? MAXINT : count;
    while (i < count) {
	if (   list->items[i].item == 0
	    && list->items[i].length == 0
	    && list->items[i].u.l_value == 0L)
	    break;
	printf("\nItem code %u, Length: %u",
	       list->items[i].item, list->items[i].length);
	if (list->items[i].length <= sizeof(long))
	    printf(" Item: %08x(%d)");
	else
	    printf(" Pointer to Item: %08x");
    }
    return (TRUE);
}

/*
 * itemlist_copy
 *
 * Copy an itemlist, using only the required amount of memory.
 *
 * If the input list occupies a block of memory larger than necessary
 * to hold the itemlist as specified, then this function will copy the
 * list into a minimal-sized malloc area.
 *
 * If you pass in a list with only a null descriptor, then no output list
 * results and the function returns NULL.
 */
itemlist *
itemlist_copy (itemlist *src)
{
    ulong	real_count = 0;
    ulong	i = 0;
    ulong	real_size = 0;
    ulong	claimed_count;
    itemlist	*new_list = NULL;

    if (!src)
	return (FALSE);

    claimed_count = (src->item_count == -1) ? MAXINT : src->item_count;
    while (real_count < claimed_count) {
	if (   src->items[real_count].item == 0
	    && src->items[real_count].length == 0
	    && src->items[real_count].u.l_value == 0)
	    break;
	++real_count;
    }
    if (real_count == 0)
	return (NULL);
    real_size = (real_count)
	        * (sizeof(short) + sizeof(short) + sizeof(void*));
    new_list = malloc(real_size);

    if (new_list == NULL)
	return (NULL);

    /*
     * Copy the input list to the new list, appending a null descriptor
     * to the end of the list.
     */
    while (i < real_count) {
	/* Struct copy here */
	new_list->items[i] = src->items[i];
	++i;
    }
    return (new_list);
}

/*
 * itemlist_count
 *
 * Returns a count of how many items are in the list. If you pass in a
 * counted itemlist (ie, an itemlist with a 'item_count' != -1), then
 * that value is returned.
 */
int
itemlist_count (itemlist *ilist)
{
    int		computed_item_count = 0;
    itemdesc	*item;

    if (ilist == NULL)
	return (0);
    if (ilist->item_count != -1)
	return (ilist->item_count);

    item = &ilist->items[0];
    while (item->item && item->length && item->u.l_value) {
	computed_item_count++;
	item++;
    }

    return (computed_item_count);
}

/* 
 * itemlist_getnext
 *
 * Sigh. Another example of a "powerful get-next operator." Go ahead, gag
 * if you feel so inclined.
 *
 * Just as a list can be terminated either with a count or a null item
 * descriptor, the get-next operation can either count or walk
 * the itemlist.
 *
 * When you initially pass in an itemlist, it should be initialized as
 * follows:
 *
 * current_item = NULL
 * current_count = 0
 *
 * And the list should contain at least one item.
 *
 * When you use this function to walk a list, it will return with the list
 * in a state pointing to the next item to be examined; ie, it advances
 * the context *after* it computes the item pointer to return to the caller.
 *
 */
itemdesc *
itemlist_getnext (itemlist *ilist)
{
    itemdesc	*next = NULL;


    if (ilist == NULL)
	return (NULL);
    /*
     * If our context variables indicate we're in initial
     * state, then sync both variables (count and pointer) and
     * point at list element 1 (the zero'th element.
     */
    if (ilist->current_count == -1) {
	next = ilist->items;
	if (next->item == 0 && next->length == 0) {
	    next = NULL;		/* we're looking at a NIL list */
	} else {
	    ilist->current_item = next;
	    ilist->current_count = 0;
	}
    } else {				/* a non-virgin list */
	if (ilist->current_count < (ilist->item_count - 1)) {
	    ilist->current_count++;
	    next = &ilist->items[ilist->current_count];
	    ilist->current_item++;
	} else {
	    next = NULL;
	}
    }
    return (next);
}
