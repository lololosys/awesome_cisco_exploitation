/* $Id: queue_inline.h,v 3.2.44.3 1996/08/28 13:04:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/queue_inline.h,v $
 *------------------------------------------------------------------
 * queue_inline.h - Singly-linked list core inlines
 *
 * September 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: queue_inline.h,v $
 * Revision 3.2.44.3  1996/08/28  13:04:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.44.2  1996/05/04  22:12:50  rpratt
 * CSCdi56447:  swapqueue doesnt reset qtail when necessary
 * Branch: California_branch
 *
 * Revision 3.2.44.1  1996/03/18  21:31:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.12.2  1996/03/09  05:10:16  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.12.1  1996/02/20  16:41:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/12/26  23:08:57  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add rpratt's code for MIB/queue manipulation.
 *
 * Revision 3.1  1995/11/09  12:47:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/18  07:27:33  smackie
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

#ifndef __QUEUE_INLINE_H__
#define __QUEUE_INLINE_H__

#include "logger.h"
#ifndef __MSG_SYSTEM_C__
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#endif


/*
 * enqueue_inline - add an element to a fifo queue.
 */

static inline void enqueue_inline (queuetype *qptr, void *eaddr)
{
    nexthelper *p, *ptr;

    p = qptr->qtail;	/* last element pointer */
    ptr = eaddr;

    /*
     * Make sure the element isn't already queued or the last
     * element in this list
     */
    if ((ptr->next != NULL) || (p == ptr)) {
	(*kernel_errmsg)(&msgsym(LINKED, SYS), "enqueue", ptr, qptr);
	return;
    }
    if (!p)				/* q empty */
	qptr->qhead = ptr;
    else				/* not empty */
	p->next = ptr;			/* update link */
    qptr->qtail = ptr;			/* tail points to new element */
    qptr->count++;
}

/*
 * dequeue_inline - remove first element of a fifo queue.
 */

static inline void *dequeue_inline (queuetype *qptr)
{
    nexthelper *p;

    if (qptr == NULL)
	return(NULL);
    p = qptr->qhead;	                /* points to head of queue */
    if (p) {				/* is there a list? */
	qptr->qhead = p->next;		/* next link */
	if (!p->next)
	    qptr->qtail = NULL;		/* this was last guy, so zap tail */
	p->next = NULL;			/* clear link, just in case */
    }
    if (p && (--qptr->count < 0) && qptr->maximum) {
	(*kernel_errmsg)(&msgsym(QCOUNT, SYS), "dequeue", qptr, qptr->count);
	qptr->count = 0;
    }
    return(p);
}

/*
 * unqueue_inline -- delete a specified element from a fifo queue.
 */

static inline void unqueue_inline (queuetype *q, void *e)
{
    nexthelper *p;
    nexthelper *prev;
    nexthelper *eptr;

    eptr = e;
    p = q->qhead;		/* first member */
    prev = (nexthelper *)q;
    while (p) {
	if (p == eptr) {		/* delete this guy */
	    prev->next = p->next;		/* keep chain intact */
	    if (!p->next) {			/* zapped the tail */
		if (q->qhead == 0)
		    prev = 0;		/* tail was head */
		q->qtail = prev;
	    }
	    eptr->next = NULL;		/* clear element pointer */
	    if ((--q->count < 0) && (q->maximum)) {
		(*kernel_errmsg)(&msgsym(QCOUNT, SYS), "unqueue", q, q->count);
		q->count = 0;
	    }
	    return;			/* exit now */
	} else {
	    prev = p;
	    p = p->next;
	}
    }
    (*kernel_errmsg)(&msgsym(NOTQ, SYS), "unqueue", eptr, q);
}

/*
 * requeue_inline - add an element to the *head* of a "fifo" queue.
 */

static inline void requeue_inline (queuetype *qptr, void *eaddr)
{
    nexthelper *ptr;

    ptr = eaddr;
    if (ptr->next != NULL) {
	(*kernel_errmsg)(&msgsym(LINKED, SYS), "requeue", ptr, qptr);
	return;
    }
    if (qptr->qhead == NULL)		/* if list is empty */
	qptr->qtail = ptr;		/* update tail pointer */
    else
	ptr->next = qptr->qhead;	/* new head points to old head */
    qptr->qhead = ptr;			/* list head points to new head */
    qptr->count++;
}

/*
 * swapqueue_inline -- swap a new element for an old element on a fifo queue.
 */
 
static inline void swapqueue_inline (queuetype *q, void *enew, void *eold)
{
    nexthelper *p;
    nexthelper *prev;
    nexthelper *eoldptr;
    nexthelper *enewptr;
 
    enewptr = enew;
    eoldptr = eold;
    p = q->qhead;               /* first member */
    prev = (nexthelper *)q;
    while (p) {
        if (p == eoldptr) {                /* delete this guy */
            prev->next = enewptr;               /* tie in new element */
            enewptr->next = eoldptr->next;   /* keep chain intact */
            eoldptr->next = NULL;          /* clear element pointer */
 	    if (q->qtail == eoldptr)	     /* last element? */
 		q->qtail = enewptr;
            return;                     /* exit now */
        } else {
            prev = p;
            p = p->next;
        }
    }
    (*kernel_errmsg)(&msgsym(NOTQ, SYS), "swapqueue", eoldptr, q);
}


#endif

