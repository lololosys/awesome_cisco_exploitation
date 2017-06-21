/* $Id: element.c,v 3.4.48.5 1996/08/28 13:03:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/element.c,v $
 *------------------------------------------------------------------
 * element.c - Element queue manipulation routines
 *
 * September 1995, Scott Mackie (moved from buffers.c)
 *
 * Copyright (c) 1995-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: element.c,v $
 * Revision 3.4.48.5  1996/08/28  13:03:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.48.4  1996/08/16  19:10:02  ahh
 * CSCdi63890:  %SYS-2-NOBLOCK: event dismiss with blocking disabled
 * Cannot block in IP Input context, so drop incoming SYN if we can't
 * respond reliably.
 * Branch: California_branch
 *
 * Revision 3.4.48.3  1996/08/13  18:46:52  widmer
 * CSCdi65503:  security hole when using httpd and no enable passwd is set
 * Branch: California_branch
 * Make http authentication work with "enable use-tacacs"
 *
 * Revision 3.4.48.2  1996/07/10  22:21:57  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.48.1  1996/03/18  21:29:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  10:18:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  16:38:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/07  22:31:06  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  18:46:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/11  12:12:34  tli
 * CSCdi41857:  show buffers variables should be unsigned
 *
 * Revision 2.1  1995/09/18  07:27:29  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "element.h"
#include "logger.h"
#include "buffers.h"
#include "buffers_inline.h"
#include "config.h"
#include "../if/network.h"
#include "msg_system.c"			/* Not a typo, see logger.h */



/*
 * Local Storage
 */
queuetype elementQ;			/* queue of queueing elements */
int elements;				/* count of free elements */
uint elementshit;			/* count of successes on get */
uint elementsmissed;			/* count of failures on get */
uint elementsmade;			/* count of new buffers made */


/*
 * element_init
 *
 * Initialize element support
 */

void element_init (void)
{
    /*
     * Set up queueing element first
     */
    queue_init(&elementQ, 0);
    elements = 0;
    elementshit = 0;
    elementsmissed = 0;
    grow_element(INITELEMENTS);
    elementsmade = 0;
    
    reg_add_critical_background(element_grow_request, "element_grow_request"); 
}


/*
 * grow_element
 * Grow the specified number of packet queueing elements
 */

void grow_element (int count)
{
    elementtype *element;
    int i, status;

    element = malloc_named(count*sizeof(elementtype), "Packet Elements");
    if (element) {
	status = raise_interrupt_level(ALL_DISABLE);
	for (i = 0; i < count; i++) {
	    enqueue(&elementQ, &element[i]);
	    elements++;
	    elementsmade++;
	}
	reset_interrupt_level(status);
    }
}

/*
 * elementBLOCK
 * Block until more queueing elements appear
 */

boolean elementBLOCK (void)
{
     return(elements ? FALSE : TRUE);
}

/*
 * pakqueue_resize
 * If new size is smaller than number of packets on queue, flush packets 
 * from end of queue.
 */

void pakqueue_resize (queuetype *queue, int newqsize)
{
    elementtype *elem;
    paktype *pak;
    int status;
    
    status = raise_interrupt_level(ALL_DISABLE);
    queue->maximum = newqsize;

    while (queue->count > newqsize) { /* Make sure queue not overfull */
	elem = queuepick(queue, queue->count-1);
	pak = pak_unqueue(queue, elem->pakptr);
	datagram_done(pak);	/* datagram_done() checks for NULL */
    }
    reset_interrupt_level(status);
}

/*
 * element_grow_request
 * Here from SERVICE_GROW_BUFFERS to create new queueing elements
 * if their number falls below the threshold.
 */
 
void element_grow_request (void)
{
    if (elements < MINELEMENTS)
	grow_element(MINELEMENTS);
}

/*
 * element_grow_check
 * Set sys_growbuffers if the number of elements falls below the
 * threshold.
 */
static inline void element_grow_check (void)
{
    if (elements < MINELEMENTS)
	critical_background_wake();
}

/*
 * element_dequeue
 * Remove the queue element and return it to the free element queue.
 */

void element_dequeue (queuetype *queue, elementtype *element)
{
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    unqueue(queue, element);
    elementshit++;
    element->pakptr = NULL;
    enqueue(&elementQ, element);
    elements++;

    reset_interrupt_level(status);
}

/*
 * pak_enqueue
 * Add packet to end of a queue
 */

paktype *pak_enqueue (queuetype *queue, paktype *pak)
{
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "pak_enqueue", pak, pak->refcount);
	reset_interrupt_level(status);
	return (NULL);
    }

    if (QUEUEFULL(queue)) {
	reset_interrupt_level(status);
	return (NULL);
    }
    element = dequeue(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	enqueue(queue, element);
	element_grow_check();
    } else {
	elementsmissed++;
	pak = NULL;
    }
    reset_interrupt_level(status);
    return(pak);
}
/*
 * pak_enqueue_reserve
 * Add packet to end of a queue if at least 'reserve' spaces are left
 */

paktype *pak_enqueue_reserve (queuetype *queue, paktype *pak, int reserve)
{
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "pak_enqueue", pak, pak->refcount);
	reset_interrupt_level(status);
	return (NULL);
    }

    if (QUEUEFULL_RESERVE(queue, reserve)) {
	reset_interrupt_level(status);
	return (NULL);
    }
    element = dequeue(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	enqueue(queue, element);
    } else {
	elementsmissed++;
	pak = NULL;
    }
    reset_interrupt_level(status);
    return(pak);
}

/*
 * pak_insqueue
 * Insert a packet in middle of a queue
 */

paktype *pak_insqueue (queuetype *queue, paktype *pak, elementtype *after)
{
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "pak_insqueue", pak, pak->refcount);
	reset_interrupt_level(status);
	return (NULL);
    }

    if (QUEUEFULL(queue)) {
	reset_interrupt_level(status);
	return (NULL);
    }
    element = dequeue(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	insqueue(queue, element, after);
	element_grow_check();
    } else {
	elementsmissed++;
	pak = NULL;
    }
    reset_interrupt_level(status);
    return(pak);
}

/*
 * pak_get_element
 * Dequeues an element from the element Queue and updates
 * appropriate counts.
 */

void *pak_get_element (paktype *pak)
{
    elementtype  *el;
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);
    if (pak->refcount < 1) {
        errmsg(&msgsym(BADSHARE, SYS), "pak_get_element", pak, pak->refcount);
        reset_interrupt_level(status);
        return (NULL);
    }
    el = dequeue(&elementQ);
    if (el) 
    {
        elementshit++;
        elements--;
        el->pakptr = pak;
	element_grow_check();
    }

    reset_interrupt_level(status);
    return(el);
}


/*
 * pak_dequeue
 * Remove packet from head of queue
 */

paktype *pak_dequeue (queuetype *queue)
{
    paktype *pak;
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    element = dequeue(queue);
    if (element) {
	elementshit++;
	pak = element->pakptr;
	element->pakptr = NULL;
	enqueue(&elementQ, element);
	elements++;
	if (pak->refcount < 1) {
	    errmsg(&msgsym(BADSHARE, SYS), "pak_dequeue", pak, pak->refcount);
	    pak = NULL;
	}
    } else
	pak = NULL;
    reset_interrupt_level(status);
    return(pak);
}

/*
 * pak_requeue
 * Add packet to front of a queue
 */

paktype *pak_requeue (queuetype *queue, paktype *pak)
{
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "pak_requeue", pak, pak->refcount);
	reset_interrupt_level(status);
	return (NULL);
    }

    if (QUEUEFULL(queue)) {
	reset_interrupt_level(status);
	return (NULL);
    }
    element = dequeue(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	requeue(queue, element);
	element_grow_check();
    } else {
	elementsmissed++;
	pak = NULL;
    }
    reset_interrupt_level(status);
    return(pak);
}

/*
 * pak_unqueue
 * Remove packet from any position in queue
 */

paktype *pak_unqueue (queuetype *queue, paktype *pak)
{
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);

    if (pak->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "pak_unqueue", pak, pak->refcount);
	reset_interrupt_level(status);
	return (NULL);
    }

    element = queue->qhead;
    while (element) {
	if (element->pakptr == pak) {
	    unqueue(queue, element);
	    element->pakptr = NULL;
	    enqueue(&elementQ, element);
	    elementshit++;
	    elements++;
	    reset_interrupt_level(status);
	    return(pak);
	}
	element = element->next;
    }
    reset_interrupt_level(status);
    return(NULL);
}


/*
 * data_enqueue
 * Add arbitary data to end of a queue (using element pointers)
 */

void *data_enqueue (queuetype *queue, void *data)
{
    elementtype *element;
    leveltype status;

    if (QUEUEFULL(queue))
	return(NULL);
    status = raise_interrupt_level(ALL_DISABLE);
    element = dequeue(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = data;
	enqueue(queue, element);
 	element_grow_check();
    } else {
	elementsmissed++;
	data = NULL;
    }
    reset_interrupt_level(status);
    return(data);
}

/*
 * data_dequeue
 * Remove packet from head of queue
 */

void *data_dequeue (queuetype *queue)
{
    void *data;
    elementtype *element;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    element = dequeue(queue);
    if (element) {
	elementshit++;
	data = element->pakptr;
	element->pakptr = NULL;
	enqueue(&elementQ, element);
	elements++;
    } else
	data = NULL;
    reset_interrupt_level(status);
    return(data);
}

/*
 * data_walklist
 * Walk the list of elementtypes, executing the function "rtn"
 * on each item of data in the list.
 */

typedef void (*data_walklist_t)(paktype *); /* for type coercion */

void data_walklist (queuetype *queue, void *rtn)
{
    elementtype *element, *next;

    for (element = queue->qhead; element; element = next) {
	next = element->next;
	(*((data_walklist_t)rtn))(element->pakptr);
     }
}

/*
 * data_insertlist
 * Walk the list of elementtypes and use the function "insert_in_front"
 * to add a new item.  The function returns TRUE if the newitem must
 * be inserted in front of the current item.
 */

typedef boolean (*data_insertlist_t)(paktype *, void *);   /* type coercion */

boolean data_insertlist (queuetype *queue, void *newitem,
			 void *insert_in_front)
{
    elementtype *curr, *prev, *new;

    prev = NULL;
    for (curr = queue->qhead; curr; curr = curr->next) {
	if ((*((data_insertlist_t)insert_in_front))(curr->pakptr, newitem)) {
	    new = dequeue(&elementQ);
	    if (new) {
		elementshit++;
		elements--;
		new->pakptr = newitem;
		element_grow_check();
	    } else {
		elementsmissed++;
		return(FALSE);
	    }
	    if (prev)
		prev->next = new;
	    else
		queue->qhead = new;
	    new->next = curr;
	    queue->count++;
	    return(TRUE);
	}
	prev = curr;
    }
    data_enqueue(queue, newitem);
    return(TRUE);
}

/*
 * find_namedthing
 * Find a namedthing in a queue of namedthings.
 */

void *find_namedthing (queuetype *queue, const char *name)
{
    namedthingtype *thing;

    for (thing = queue->qhead; thing; thing = thing->next) {
	if (!strcasecmp(thing->name, name)) {
	    return(thing);
	}
    }
    return(NULL);
}

/*
 * element_num_elements
 *
 * Return number of queueing elements available
 */

int element_num_elements (void)
{
    return(elements);
}
