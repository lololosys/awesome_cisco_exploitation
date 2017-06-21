/* $Id: queue.c,v 3.2.44.1 1996/03/18 21:31:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/queue.c,v $
 *------------------------------------------------------------------
 * queue.c - Simple single-linked queue support
 *
 * September 1995, Scott Mackie (moved from buffers.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: queue.c,v $
 * Revision 3.2.44.1  1996/03/18  21:31:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.12.2  1996/03/09  05:10:14  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.12.1  1996/02/20  16:41:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/12/26  23:08:56  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add rpratt's code for MIB/queue manipulation.
 *
 * Revision 3.1  1995/11/09  12:47:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/18  07:27:32  smackie
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
#include "logger.h"
#include "queue_inline.h"


/*
 * queue_init
 * Initialize a queuetype
 */

void queue_init (queuetype *q, int maximum)
{
    q->qhead = NULL;
    q->qtail = NULL;
    q->count = 0;
    q->maximum = maximum;
}

/*
 * queryqueuedepth -- Query the length of the queue.
 * Does not lock out interrupts.
 */

int queryqueuedepth (queuetype* q)
{
   return(q->count);
}

/*
 * peekqueuehead -- Return address of element at head of queue.
 * Does not lock out interrupts.
 */

void *peekqueuehead (queuetype* q)
{
   nexthelper* p;

   p = (nexthelper *)q->qhead;		/* first member */
   return(p);

}

/*
 * checkqueue
 * Return TRUE if specified element is in queue, FALSE otherwise
 * Does not lock out interrupts.
 */

boolean checkqueue (queuetype *q, void *e)
{
    nexthelper *p;

    if (q == NULL)
        return(FALSE);
    p = q->qhead;		/* first member */
    while (p) {
	if (p == e)	/* match? */
	    return(TRUE);		/* yes, return TRUE */
	else
	    p = p->next;		/* no, cdr the list */
    }
    return(FALSE);			/* not in queue, return FALSE */
}

/*
 * p_dequeue -- remove first element from a fifo queue
 * Locks out interrupts.
 */

void *p_dequeue (queuetype *qptr)
{
    void      *p;
    leveltype  status;

    status = raise_interrupt_level(ALL_DISABLE);
    p = dequeue_inline(qptr);
    reset_interrupt_level(status);	/* old priority */
    return (p);
}

/*
 * p_enqueue -- put an element into a fifo queue
 * Locks out interrupts.
 */

void p_enqueue (queuetype *qptr, void *eaddr)
{
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);
    enqueue_inline(qptr, eaddr);
    reset_interrupt_level(status);
}

/*
 * p_requeue - add an element to the *head* of a "fifo" queue.
 * Locks out interrupts.
 */

void p_requeue (queuetype *qptr, void *eaddr)
{
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);
    requeue_inline(qptr, eaddr);
    reset_interrupt_level(status);
}

/*
 * p_swapqueue - remove an old element from a fifo queue AND add a 
 * new element into the fifo queue at the old elements' position in
 * the fifo queue.
 * Locks out interrupts.
 */
 
void p_swapqueue (queuetype *qptr, void *enew, void *eold)
{
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);
    swapqueue_inline(qptr, enew, eold);
    reset_interrupt_level(status);
}

/*
 * p_unqueue -- delete a specified element from a fifo queue.
 * Lock out interrupts.
 */

void p_unqueue (queuetype *q, void *e)
{
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);
    unqueue_inline(q, e);
    reset_interrupt_level(status);
}

/*
 * p_unqueuenext -- delete the next element from a fifo queue.
 * Lock out interrupts.
 */

void p_unqueuenext (queuetype *q, void **prev)
{
    leveltype status;
    nexthelper *nextelement;

    status = raise_interrupt_level(ALL_DISABLE);
    
    nextelement = (nexthelper *)*prev;
    
    if (nextelement) {	   /* else we're at the end of the list - do nothing */
	*prev = nextelement->next;
	nextelement->next = NULL;
	
	if (!*prev) {	 /* prev now at the end of the list; fix the header */
	    if (prev == (void **)&q->qhead)
		q->qtail = NULL;   /* prev is the header, now an empty list */
	    else
		q->qtail = prev;   	/* real prev , it's at the end */
	}
	if ((--q->count < 0) && (q->maximum)) {
	      (*kernel_errmsg)(&msgsym(QCOUNT, SYS), "p_unqueuenext",
			       q, q->count);
	      q->count = 0;
	}
	
    }
    reset_interrupt_level(status);
    return;
}

/*
 * enqueue - add an element to a fifo queue.
 * Note no interrupt interlocking.
 */

void enqueue (queuetype *qptr, void *eaddr)
{
    enqueue_inline(qptr, eaddr);
}

/*
 * dequeue - remove first element of a fifo queue.
 * Note no interrupt interlocking.
 */

void *dequeue (queuetype *qptr)
{
    return(dequeue_inline(qptr));
}

/*
 * unqueue -- delete a specified element from a fifo queue.
 * Does not lock out interrupts.
 */

void unqueue (queuetype *q, void *e)
{
    unqueue_inline(q, e);
}

/*
 * requeue - add an element to the *head* of a "fifo" queue.
 * Performs the inverse operation of dequeue()
 * Note no interrupt interlocking.
 */

void requeue (queuetype *qptr, void *eaddr)
{
    requeue_inline(qptr, eaddr);
}

/*
 * remqueue - remove a specified element from a fifo queue. 
 * The previous queue element is supplied (paddr) to allow a direct lift 
 * (eaddr) and repair. If paddr is NULL, the first member in queue is removed.
 * Note no interrupt interlocking.
 */

void *remqueue (queuetype *qptr, void *eaddr, void *paddr)
{

    /*
     * If previous member is null and member is at the head of queue,
     * remove from head 
     */
    if ((eaddr == qptr->qhead) && 
        (paddr == NULL)) {
	return(dequeue(qptr));
    } else {
       /*
        * Check to make sure the previous member is really what it 
        * claims to be or that the current member or previous member are 
        * non-null
        */
       if ((paddr == NULL) || (eaddr == NULL) || 
           (((nexthelper *)paddr)->next != eaddr)){
          (*kernel_errmsg)(&msgsym(LINKED, SYS), "remqueue", eaddr, qptr);
          return(NULL);
       }

       /* 
        * Lift the candidate eaddr and repair pointers
        */
       ((nexthelper *)paddr)->next = ((nexthelper *)eaddr)->next;
       ((nexthelper *)eaddr)->next = NULL;

       /* 
        * Repair tail of queue, if necessary
        */
       if (((nexthelper *)paddr)->next == NULL)
           qptr->qtail = paddr;

        /*
         * Decrement queue count and do sanity check
         */
       if ((--qptr->count < 0) && qptr->maximum) {
	  (*kernel_errmsg)(&msgsym(QCOUNT, SYS), "remqueue", qptr,
			   qptr->count);
	  qptr->count = 0;
       }   
       return(eaddr);  
     }
  }

/*
 * insqueue - add an element to the *middle* of a "fifo" queue.
 *            'eaddr' goes immediately after 'paddr'.
 *
 * If 'paddr' is NULL, requeue is called.
 * Note no interrupt interlocking.
 */

void insqueue (queuetype *qptr, void *eaddr, void *paddr)
{
    nexthelper *eptr, *pptr;

    eptr = (nexthelper *)eaddr;
    pptr = (nexthelper *)paddr;
    if (pptr == NULL) {
	requeue(qptr, eptr);
	return;
    }
    if (eptr->next != NULL) {
	(*kernel_errmsg)(&msgsym(LINKED, SYS), "insqueue", eptr, qptr);
	return;
    }
    eptr->next = pptr->next;
    pptr->next = eptr;
    if (eptr->next == NULL) 
	qptr->qtail = eptr;		/* tail points to new element */
    qptr->count++;
}

/*
 * queueBLOCK
 * Schedule test for blocking on queue empty
 */

boolean queueBLOCK (queuetype *qptr)
{
    return(qptr->qhead ? FALSE : TRUE);
}
