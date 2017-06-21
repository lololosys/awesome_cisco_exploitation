/* $Id: ipc_util.h,v 3.2 1995/11/17 17:29:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipc/ipc_util.h,v $
 *------------------------------------------------------------------
 * ipc_util.h - Header file for IPC utility code.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_util.h,v $
 * Revision 3.2  1995/11/17  17:29:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:03:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPC_UTIL_H__
#define __IPC_UTIL_H__

/*
 * Thread tables are low-overhead hash tables with linked-list threads
 * that allow fast, reentrant lookups and "in order" traversals without
 * the NP-complete problems of balanced tree reording under
 * the threat of re-entrant lookups, insertions and deletions.
 *
 * You create a table, and provide the manipulation routines for your
 * particular type of data.  Instead of the table routines having to know
 * how to compute the hash value for your items, or how to compare or
 * delete them, you provide the routines that do it.
 *
 * Here are the prototypes:
 */

/*
 * Compare: takes two of your table elements and returns 0 if they're the
 *          same, -1 if element A is less than element B and
 *          1 if element A is greater than element B.
 */

typedef int (*thread_key_compare)(void *, void *);

/*
 * Hash: takes a table element and the size of the hash table
 *       and computes an appropriate hash value for the element.
 */

typedef uint (*thread_hash)(void *, int);

/*
 * Delete: does what is necessary to delete the item handed to it.
 */

typedef void (*thread_delete)(void *);

/*
 * Each item you pass into the "thread tables" must begin with
 * the following 16 byte header.  The table manager uses these values
 * to maintain "next" and "previous" links in the linked list, and a
 * chain pointer to hash buckets in the hash table, as well as an item
 * reference count.
 */

typedef struct thread_linkage {
    struct thread_linkage *next;
    struct thread_linkage *prev;
    struct thread_linkage *chain;
    uint refcount;
} thread_linkage;

/*
 * Before you may operate on a table, you must use ipc_thread_create()
 * which hands back a pointer to the following structure.  You use this
 * pointer in ALL future calls to the table manager to indentify the
 * particular table you're working with.  You won't need to mess with
 * any of the fields defined below.  The table manager does this for you.
 */

typedef struct thread_table_ {
    char *name;				/* Name of this table */

    uint size;				/* Number of hash slots */
    thread_linkage **slots;		/* Hash table pointer to slots */

    thread_linkage *head;		/* Head of list */
    thread_linkage *tail;		/* End ot list */
    thread_linkage *current;		/* Current item, used when
					   traversing a list */

    boolean reset_search;		/* Flag that tells us to reset
					   a forward of backward traversal */

    uint lookups;
    uint comparisons;
    uint item_count;

    thread_key_compare compare;
    thread_hash hash;
    thread_delete delete;
} thread_table;


/*
 * TABLE API 
 */

/*
 * Create a table by prviding the size of the hash_table (which should be
 * a prime number for best performance), the table name, and the compare,
 * hash and delete functions discussed above.
 */
extern thread_table *ipc_thread_create(uint hash_table_size, char *name,
				       thread_key_compare compare_func,
				       thread_hash hash_func,
				       thread_delete delete_func);

/*
 * Destroy a thread table, and any elements currently held by it.
 * If any elements are locked by a refcount that's greater than 1, 
 * the elements are detached from the table, but not deleted.  They
 * will be deleted automatically when the item is unlocked.
 */
extern void ipc_thread_destroy(thread_table *table);

/*
 * To add items to the table, use the append or prepend functions.
 * These have no effect on where the data goes in the hash table,
 * but DO affect how the link list is threaded together.  Use
 * append() to add to the end of the list, and prepend() to put
 * this item at the beginning, pushing the current beginning under it.
 */

extern void ipc_thread_append(thread_table *table, void *item);
extern void ipc_thread_prepend(thread_table *table, void *item);

/*
 * To find items in a table, call the "lookup" routine, and provide a
 * "prototype" item that has the key fields set.  The lookup routine will
 * find a match in the hash table and return the original item to you.
 */

extern void *ipc_thread_lookup(thread_table *table, void *item);

/*
 * To traverse the list in the order (or reverse order) they were inserted
 * with use these forward/backward lookup functions to reset the "get_next"
 * and "get_prev" routines to search from the proper starting points.
 */

extern void ipc_thread_forward_lookup(thread_table *table);
extern void ipc_thread_backward_lookup(thread_table *table);

extern void *ipc_thread_get_next(thread_table *table);
extern void *ipc_thread_get_prev(thread_table *table);
extern void *ipc_thread_get_current(thread_table *table);
extern void *ipc_thread_get_head(thread_table *table);
extern void *ipc_thread_get_tail(thread_table *table);

/*
 * To remove items from the table, use the yank functions.
 * The generic yank function acts like the lookup function, except that
 * upon locating the item in question, *removes* the item from the 
 * table before giving it back to you.
 */

extern void *ipc_thread_yank(thread_table *table, void *item);
extern void *ipc_thread_yank_head(thread_table *table);
extern void *ipc_thread_yank_tail(thread_table *table);
extern void *ipc_thread_yank_current(thread_table *table);

/*
 * To lock an item, use the lock and unlock routines.
 * They manage the item's refcount field.  The act of inserting
 * an item into a table causes the refcount to increase.  The act of
 * removing an item from the table causes the refcount to decrease.
 *
 * If you call unlock() in an item and that happens to unlock the item
 * completely (i.e. refcount becomes zero) the item will be deleted for
 * you automatically.
 */

extern void ipc_thread_lock(void *);
extern void ipc_thread_unlock(thread_table *, void *);

/*
 * Returns a count of items in the table.
 */

extern uint ipc_thread_get_count(thread_table *table);

/*
 * hash_str() returns a cheap-and-dirty hash value for a string.
 * no guarantees are made towards it's actual value, or competency in
 * computing the values.
 */

extern uint ipc_thread_hash_str(char *string);

/*
 * chop_copy() attempts to copy a string into a destination string
 * truncating it to "length" characters.  If the original string is
 * longer than the destination, "..." replaces the tail end of the string.
 *
 * i.e. ipc_copy_copy("hello there", dest, 8);
 * makes dest = "hello...".
 */
extern void ipc_chop_copy(char *source, char *dest, uint length);

/*
 * clone_string() attempts to copy the source string and return a pointer
 * to the copy.  The memory for the copy is obtained by malloc.  Try not
 * to use this too much. :-)
 */
extern char *ipc_clone_string(char *source);

/*
 * extract_string_copy attempts to copy a variable length string
 * found in ipc message (format: n=2-byte length followed by n byte string.)
 * This routine returns a pointer to a copy of the equivalent null terminated
 * C-style string.
 */

extern char *ipc_extract_string_copy(ushort length, char *string);

STRUCTURE_REF(ipc_port_data);

#endif __IPC_UTIL_H__

