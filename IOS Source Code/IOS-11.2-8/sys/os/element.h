/* $Id: element.h,v 3.1.4.1 1996/04/17 13:45:30 fred Exp $
 * $Source: /release/112/cvs/Xsys/os/element.h,v $
 *------------------------------------------------------------------
 * element.h - Element queue manipulation routines (inline)
 * 
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: element.h,v $
 * Revision 3.1.4.1  1996/04/17  13:45:30  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* #include "buffers.h" */
/* #include "buffers_private.h" */
/* #include "buffers_inline.h" */
#include "../os/queue_inline.h"

/*
 * Local Storage
 */
extern queuetype elementQ;		/* queue of queueing elements */
extern int elements;			/* count of free elements */
extern uint elementshit;		/* count of successes on get */
extern uint elementsmissed;		/* count of failures on get */
extern uint elementsmade;		/* count of new buffers made */

/*
 * Avoid implicit declaration different from actual.
 */
extern void critical_background_wake (void);

/*
 * element_grow_check
 * Set sys_growbuffers if the number of elements falls below the
 * threshold.
 */
static inline void element_grow_check_inline (void)
{
    if (elements < MINELEMENTS)
        critical_background_wake();
}

/*
 * pak_enqueue_inline
 * Add packet to end of a queue
 */

static inline
paktype *pak_enqueue_inline (queuetype *queue, paktype *pak)
{
    elementtype *element;


    if (pak->refcount < 1) {
 /* 	errmsg(&msgsym(BADSHARE, SYS), "pak_enqueue", pak, pak->refcount); */
	return (NULL);
    }

    if (QUEUEFULL(queue)) {
	return (NULL);
    }
    element = dequeue_inline(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	enqueue_inline(queue, element);
	element_grow_check_inline();
    } else {
	elementsmissed++;
	pak = NULL;
    }
    return(pak);
}
/*
 * pak_enqueue_reserve
 * Add packet to end of a queue if at least 'reserve' spaces are left
 */

static inline
paktype *pak_enqueue_reserve_inline (queuetype *queue, paktype *pak, int reserve)
{
    elementtype *element;


    if (pak->refcount < 1) {
 /* 	errmsg(&msgsym(BADSHARE, SYS), "pak_enqueue", pak, pak->refcount); */
	return (NULL);
    }

    if (QUEUEFULL_RESERVE(queue, reserve)) {
	return (NULL);
    }
    element = dequeue_inline(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	enqueue_inline(queue, element);
    } else {
	elementsmissed++;
	pak = NULL;
    }
    return(pak);
}

/*
 * pak_dequeue
 * Remove packet from head of queue
 */

static inline
paktype *pak_dequeue_inline (queuetype *queue)
{
    paktype *pak;
    elementtype *element;

    element = dequeue_inline(queue);
    if (element) {
	elementshit++;
	pak = element->pakptr;
	element->pakptr = NULL;
	enqueue_inline(&elementQ, element);
	elements++;
	if (pak->refcount < 1) {
 /* 	    errmsg(&msgsym(BADSHARE, SYS), "pak_dequeue", pak, pak->refcount); */
	    pak = NULL;
	}
    } else
	pak = NULL;
    return(pak);
}

/*
 * pak_requeue
 * Add packet to front of a queue
 */

static inline
paktype *pak_requeue_inline (queuetype *queue, paktype *pak)
{
    elementtype *element;


    if (pak->refcount < 1) {
 /* 	errmsg(&msgsym(BADSHARE, SYS), "pak_requeue", pak, pak->refcount); */
	return (NULL);
    }

    if (QUEUEFULL(queue)) {
	return (NULL);
    }
    element = dequeue_inline(&elementQ);
    if (element) {
	elementshit++;
	elements--;
	element->pakptr = pak;
	requeue_inline(queue, element);
	element_grow_check_inline();
    } else {
	elementsmissed++;
	pak = NULL;
    }
    return(pak);
}

/*
 * pak_unqueue_inline
 * Remove packet from any position in queue, optionally doing accessor check
 */

static inline paktype *pak_unqueue_inline (
    queuetype *queue,
    paktype *pak,
    boolean do_acc_check)
{
    elementtype *element;


    if (do_acc_check && (pak->refcount < 1)) {
 /* 	errmsg(&msgsym(BADSHARE, SYS), "pak_unqueue", pak, pak->refcount); */
	return (NULL);
    }

    element = queue->qhead;
    while (element) {
	if (element->pakptr == pak) {
	    unqueue_inline(queue, element);
	    element->pakptr = NULL;
	    enqueue_inline(&elementQ, element);
	    elementshit++;
	    elements++;
	    return(pak);
	}
	element = element->next;
    }
    return(NULL);
}
