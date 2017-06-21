/* $Id: itemlist.h,v 3.3 1996/01/25 23:12:31 gstovall Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/util/itemlist.h,v $
 *------------------------------------------------------------------
 * itemlist.h -- header file for itemlist.c
 *
 * October, 1995 David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: itemlist.h,v $
 * Revision 3.3  1996/01/25  23:12:31  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.2  1995/11/17  00:47:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:43:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/10  13:16:57  dstine
 * Add placeholders for Arkansas
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ITEMLIST_H__
#define __ITEMLIST_H__

extern	boolean		itemlist_dump(itemlist *, char *);
extern	itemlist	*itemlist_copy(itemlist *);
extern	int		itemlist_count(itemlist *);
extern	itemdesc	*itemlist_getnext(itemlist *);

/*
 * Convert a absolute memory address for an item into a itemlist-relative
 * address and vice-versa.
 *
 * NB: The item storage must be in the same memory space if you expect
 *     relative addressing to work.
 */
static inline void *
itemlist_abs_to_rel (itemlist *ilist, itemdesc *item)
{
    return ((void *)((uchar *)item->u.buffer - (uchar *)ilist));
}

static inline void *
itemlist_rel_to_abs (itemlist *ilist, itemdesc *item)
{
    return ((void *)((uchar *)ilist + item->u.l_value));
}

static inline void
itemlist_reset (itemlist *ilist)
{
    ilist->current_count = -1;
    ilist->current_item = NULL;
}

static inline sys_timestamp
item_get_timestamp (itemlist *ilist, itemdesc *item)
{
    sys_timestamp *item_time_p;

    item_time_p = item->u.buffer;
    return(*item_time_p);
}

static inline void
item_put_timestamp (itemlist *ilist, itemdesc *item, sys_timestamp value)
{
    sys_timestamp *item_time_p;

    item_time_p = item->u.buffer;
    *item_time_p = value;
}

/*
 * Some helper macros to allocate and initialize a one-off and a n-off
 * itemlist where 'n' is small, presumably on the stack of a function.
 */
#define ONE_ITEM_LIST(name, itm, len, val) \
        struct itemdesc_ name ## _item = { (itm), (len), 0,0,0,0,0, { (val) } }; \
        struct itemlist_ name ## _list = { 0,0,0,0,0, 1, -1, NULL, & name ## _item }

/* 
 * Define storage for a itemlist of 'n' items. This macro will not initialize
 * the list for the user. 'n' must be 1 or greater.
 */
#define N_ITEM_LIST(name, n) \
        struct itemdesc_ name ## _item[(n)]; \
        struct itemlist_ name ## _list = { 0,0,0,0,0, n, -1, NULL, (itemdesc *)& name ## _item[0] }

#endif	__ITEMLIST_H__
