/* $Id: ipc_util.c,v 3.3.44.4 1996/08/28 12:50:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_util.c,v $
 *------------------------------------------------------------------
 * ipc_util.c - Utilities needed by the IPC system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_util.c,v $
 * Revision 3.3.44.4  1996/08/28  12:50:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.44.3  1996/06/28  23:18:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/06/10  04:52:08  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.3.44.1  1996/03/18  20:18:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  09:47:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  14:27:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  22:29:17  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:29:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:05:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  00:13:04  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.2  1995/07/12  22:27:10  dlobete
 * CSCdi34149:  Cleanup slave applications
 *
 * Revision 2.1  1995/06/07  21:03:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "sys_registry.h"
#include "file.h"
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_debug.h"

/*
 * Threaded Hash Table
 */


/* 
 * ipc_thread_create:
 *
 * Create a new threaded hash-table of a particular size.
 * Hash tables generally work best if their size is a prime number.
 *
 * Returns: A pointer to a new threaded hash table, or NULL if one
 *          couldn't be allocated.
 *
 * ### CALL ONLY FROM PROCESS LEVEL ###
 */

thread_table *ipc_thread_create (uint table_size, char *name,
				 thread_key_compare compare_func,
				 thread_hash hash_func,
				 thread_delete delete_func)
{
    thread_table *table;
        
    /* Create the table control structures */

    table = malloc_named(sizeof(thread_table), "IPC Thread");

    if (table == NULL) return NULL;

    /* Create the hash table slots array */

    table->slots = malloc_named(sizeof(void *) * table_size, "IPC Thread Slot");

    if (table->slots == NULL) {
	free (table);
	return NULL;
    }

    table->name = name;
    table->size = table_size;

    table->head = NULL;
    table->tail = NULL;
    table->current = NULL;
    table->reset_search = TRUE;

    table->lookups = 0;
    table->comparisons = 0;
    table->item_count = 0;

    table->compare = compare_func;
    table->hash = hash_func;
    table->delete = delete_func;

    return table;
}

/*
 * ipc_thread_destroy:
 *
 * Destroy a threaded hash table by freeing all the elements and
 * deleting the table control structure.
 * 
 * If items in the table are still being referenced elsewhere, those
 * items will be not be freed.
 *
 * ### CALL ONLY FROM PROCESS LEVEL ###
 */

void ipc_thread_destroy (thread_table *table)
{
    thread_linkage *next;

    /* Spin through each element and unhook it from the list */

    if (table->head != NULL) {
	do {
	    next = table->head->next;
	    ipc_thread_unlock(table, table->head);
	    table->head = next;
	} while (table->head != NULL);
    }

    free(table);
}

/*
 * hash_add
 *
 * Add an item into the hash table.
 */

static void hash_add (thread_table *table, thread_linkage *element)
{
    thread_linkage *next;
    int hash_value;

    next = NULL;
    hash_value = (*table->hash)(element, table->size);

    if (table->slots[hash_value] != NULL)
	next = table->slots[hash_value];

    table->slots[hash_value] = element;
    element->chain = next;
}

/*
 * ipc_thread_forward_lookup
 *
 * Reset an "in-order" traversal from the beginning of the table
 * to the end.
 */

void ipc_thread_forward_lookup (thread_table *table)
{
    table->current = table->head;
    table->reset_search = TRUE;
}

/*
 * ipc_thread_backward_lookup
 *
 * Reset an "in-order" traversal from the end of the table
 * to the end.
 */

void ipc_thread_backward_lookup (thread_table *table)
{
    table->current = table->tail;
    table->reset_search = TRUE;
}

/*
 * ipc_thread_append
 *
 * Add an element to the hash table, and link the element to the 
 * tail of the thread, making it the new tail.
 */

void ipc_thread_append (thread_table *table, void *item)
{
    thread_linkage *element;
    leveltype status;

    element = (thread_linkage *)item;

    status = raise_interrupt_level(NETS_DISABLE);

    /* Add to table thread */

    if (table->head == NULL) {
	element->next = NULL;
	element->prev = NULL;
	table->head = item;
	table->tail = item;
	table->current = item;
    } else {
	element->next = NULL;
	element->prev = table->tail;
	table->tail->next = element;
	table->tail = element;
    }

    hash_add(table, element);

    element->refcount++;

    reset_interrupt_level(status);

    table->item_count++;
}

/*
 * ipc_thread_prepend:
 *
 * Add an element to the hash table, and link the element to the 
 * head of the thread, making it the new head.
 */

void ipc_thread_prepend (thread_table *table, void *item)
{
    thread_linkage *element;
    leveltype status;

    element = (thread_linkage *)item;

    status = raise_interrupt_level(NETS_DISABLE);
    
    if (table->head == NULL) {
	element->next = NULL;
	element->prev = NULL;
	table->head = element;
	table->tail = element;
	table->current = element;
    } else {
	element->next = table->head;
	element->prev = NULL;
	table->head->prev = element;
	table->head = element;
    }

    hash_add(table, element);

    element->refcount++;

    reset_interrupt_level(status);

    table->item_count++;
}

/*
 * ipc_thread_lookup:
 *
 * Take a template item (only containing the key fields) and return the
 * item in the table that matches the key field in the template.
 *
 * Returns: The matching item, or NULL if it couldn't be found.
 */

void *ipc_thread_lookup (thread_table *table, void *item)
{
    uint template_hash;
    thread_linkage *element;
    thread_linkage *next;

    element = (thread_linkage *)item;
    table->lookups++;

    template_hash = (*table->hash)(item, table->size);

    if ( (next = table->slots[template_hash]) == NULL) return NULL;

    do {
	table->comparisons++;
	if ( (*table->compare)(next, item) == 0) return(next);
	next = next->chain;
    } while (next != NULL);
    
    return NULL;
}

/*
 * hash_yank
 *
 * Remove an item from the hash table.
 *
 * Returns: the item.
 *
 */

static thread_linkage *hash_yank (thread_table *table,
				  thread_linkage *element)
{
    uint template_hash;
    thread_linkage *next;
    thread_linkage *prev;

    template_hash = (*table->hash)(element, table->size);
    table->lookups++;

    prev = NULL;

    if ( (next = table->slots[template_hash]) == NULL) return NULL;

    /*
     * Scurry through the hash chain, looking for our element.  Along
     * the way, we keep track of where we are in the chain, so if we
     * find the element, we can sew the chain pointer together.
     */
    do {
	table->comparisons++;
	if ( (*table->compare)(element,next) == 0) {
	    if (prev == NULL) {
		table->slots[template_hash] = next->chain;
	    } else {
		prev->chain = next->chain;
	    }
	    return next;
	}
	prev = next;
	next = next->chain;
    } while (next != NULL);
    return NULL;
}

/*
 * ipc_thread_yank:
 *
 * Yank a particular item (or template) from the hash table, and unhook its
 * threads.  You can either pass in the item directly, or a template 
 * for the system to search for.
 *
 * Returns: The item that was yanked, or NULL if it couldn't be found.
 */

void *ipc_thread_yank (thread_table *table, void *item)
{
    thread_linkage *element;
    leveltype status;

    if (item == NULL) return NULL;

    /* Yank it from the hash table */

    status = raise_interrupt_level(NETS_DISABLE);

    element = hash_yank(table, (thread_linkage *)item);
    if (element == NULL) {
	reset_interrupt_level(status);
	return NULL;
    }

    /* Unhook the pointers around this element. */

    if (element == table->head) {

	/*
	 * Case I: Yanking head of list.
	 */

	table->head = element->next;

	if (table->head == NULL) {
	    table->tail = NULL;
	    table->current = NULL;
	} else {
	    if (table->current == element) table->current = table->head;
	    table->head->prev = NULL;
	}
    } else if (element == table->tail) {

	/*
	 * Case II: Yanking Tail of List
	 */

	table->tail = element->prev;

	if (table->tail == NULL) {
	    table->head = NULL;
	    table->current = NULL;
	} else {
	    if (table->current == element) table->current = table->tail;
	    table->tail->next = NULL;
	}
    } else {

	/*
	 * Case III: Yanking somewhere in the middle.
	 */

	element->prev->next = element->next;
	element->next->prev = element->prev;

	if (table->current == element) table->current = element->next;
    }
    element->refcount--;

    reset_interrupt_level(status);

    table->item_count--;

    return element;
}

/*
 * ipc_thread_yank_head/tail/current
 * 
 * Yank the head/tail or current element of a particular table and return it.
 */

void *ipc_thread_yank_head (thread_table *table)
{
    return ipc_thread_yank(table, table->head);
}

void *ipc_thread_yank_tail (thread_table *table)
{
    return ipc_thread_yank(table, table->tail);
}

void *ipc_thread_yank_current (thread_table *table)
{
    return ipc_thread_yank(table, table->current);
}

/*
 * ipc_thread_get_next:
 *
 * Return the next element in a threaded hash table.
 * Use (ipc_thread_{forward,backward}_lookup()) to reset the search
 *
 */

void *ipc_thread_get_next (thread_table *table)
{
    if (table->current == NULL) return NULL;

    if (table->reset_search) {
	table->reset_search = FALSE;
	return table->current;
    }

    table->current = table->current->next;

    return table->current;
}

/*
 * ipc_thread_get_prev:
 *
 * Return the previous element in a threaded hash table.
 * Use (ipc_thread_{forward,backward}_lookup()) to reset the search
 *
 */

void *ipc_thread_get_prev (thread_table *table)
{
    if (table->current == NULL) return NULL;
    
    if (table->reset_search) {
	table->reset_search = FALSE;
	return table->current;
    }

    table->current = table->current->prev;
    return table->current;
}

/*
 * ipc_thread_get_current/head/tail:
 *
 * Get the current/head/tail element in a hash table, undergoing a search.
 *
 */

void *ipc_thread_get_current (thread_table *table)
{
    return table->current;
}

void *ipc_thread_get_head (thread_table *table)
{
    return table->head;
}

void *ipc_thread_get_tail (thread_table *table)
{
    return table->tail;
}

/*
 * ipc_thread_lock:
 *
 * Lock the item by increasing its refcount.
 */

void ipc_thread_lock (void *item)
{
    thread_linkage *link;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);

    link = (thread_linkage *)item;
    
    if (link->refcount == 0) {
	errmsg(&msgsym(LOCK,IPC));
	return;
    }

    link->refcount++;

    reset_interrupt_level(status);

}

/*
 * ipc_thread_unlock:
 *
 * Attempt to unlock an item by decreasing its refcount by one.
 * It the refcount drops to zero, nobody needs it hanging around anymore
 * so delete it.
 */

void ipc_thread_unlock (thread_table *table, void *item)
{
    thread_linkage *link;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);

    link = (thread_linkage *)item;

    if (link->refcount == 0) {
	errmsg(&msgsym(UNLOCK,IPC));
	reset_interrupt_level(status);
	return;
    }

    link->refcount--;
    
    if (link->refcount == 0) {
	link->next = NULL;
	link->prev = NULL;
	link->chain = NULL;
	(*table->delete)(item);
    }

    reset_interrupt_level(status);
}

/*
 * ipc_thread_get_count:
 *
 * Just how many items are in this table?
 */

uint ipc_thread_get_count (thread_table *table)
{
    return table->item_count;
}

/*
 * ipc_thread_hash_str:
 *
 * Return a simple non-scientific hash string by adding in the byte value
 * for each character and shifting the result left by two.
 * 
 * Yeah, we start to lose information after 24 characters, and limit the
 * information to the last 32 characters, but it meets our needs.
 *
 * Feel free to enhance it if you like.
 */

uint ipc_thread_hash_str (char *string)
{
    uint hash_value;

    hash_value = 0;

    while (*string != '\0')
	hash_value = (hash_value << 1) + *string++;

    return hash_value;
}

/*
 * ipc_chop_copy:
 *
 * copy source string to destination string, except make sure to truncate
 * the destination string to chop_length bytes.  If we do truncate, we'll
 * add "..." to the dest string.  In any case, we will never copy past
 * the end of dest.
 */

void ipc_chop_copy (char *source, char *dest, uint chop_length)
{
    uint source_len;
	
    dest[0] = '\0';
    source_len = strlen(source);
    if (chop_length < 5) return;
    
    if (source_len > chop_length) {
	bcopy(source,dest,chop_length-3);
	dest[chop_length-3] = '\0';
	strcat(dest,"...");
    } else {
	strcpy(dest,source);
    }
}

char *ipc_clone_string (char *source)
{
    char *new_string;

    new_string = malloc_named(strlen(source) + 1, "IPC Name String");
    
    if (new_string) strcpy(new_string, source);

    return new_string;
}

char *ipc_decode_error (ipc_error_code code)
{
    switch(code) {
	case IPC_OK: 		return "no error";
	case IPC_INTERNAL:      return "internal error";
	case IPC_NO_SUCH_SEAT:  return "no such seat address";
	case IPC_SEAT_IN_USE:   return "seat already registered";
	case IPC_NO_MEMORY: 	return "no memory available";
	case IPC_PORT_IN_USE: 	return "port in use";
	case IPC_TIMEOUT: 	return "timeout";
	case IPC_NO_SUCH_PORT: 	return "no such port";
	default: 		return "UNKNOWN ERROR";
    }
}

void ipc_error_debug (char *routine, ipc_error_code code)
{
    if (ipc_debug_events) {
	buginf("IPC: %s %s", routine, ipc_decode_error(code));
    }
}

char *ipc_extract_string_copy(ushort length, char *string)
{
    char *result;

    result = malloc(length + 1);

    if (result == NULL) return NULL;

    bcopy(string, result, length);

    return result;
}

