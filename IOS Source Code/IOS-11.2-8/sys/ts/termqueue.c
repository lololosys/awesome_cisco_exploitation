/* $Id: termqueue.c,v 3.2.62.1 1996/08/28 13:18:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/termqueue.c,v $
 *------------------------------------------------------------------
 * termqueue.c -- Inbound terminal port queueing
 *
 * 20-Jun-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termqueue.c,v $
 * Revision 3.2.62.1  1996/08/28  13:18:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:56:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/14  07:03:23  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.2  1995/06/20 20:10:51  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:10:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "termqueue.h"
#include "logger.h"
#include "../ts/msg_termqueue.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ts/ts_debug.h"

termqueue *termqueue_queuehead;		/* Pointer to first entry on queue */
termqueue *termqueue_queuetail;		/* Pointer to last entry on queue */
ushort termqueue_next_id;		/* Next ID to assign */
sys_timestamp termqueue_nexttime;	/* Next time to check termqueues */

/*
 * Externals
 */

static int polling_interval = TERMQUEUE_INTERVAL;

/*
 * termqueue_init - Initialize the terminal queue system
 *
 * This routine is used to initialize the terminal queueing system.
 */

void termqueue_init (void)
{
    termqueue_queuehead = NULL;		/* Mark queue as empty */
    termqueue_queuetail = NULL;		/* In both places */
    termqueue_next_id = 1;		/* Set initial ID number */
    GET_TIMESTAMP(termqueue_nexttime); 	/* Initialize next check */


    /*
     * Set up terminal server debugging flags. This may not be conceptually
     * the best place to do this, but it will work...
     */
    ts_debug_init();

    /*
     * Initialize termqueue parser support
     */
    termqueue_parser_init();

    /*
     * Register some functions
     */
    reg_add_onesec(termqueue_periodic, "termqueue_periodic");
    reg_add_line_handoff(termqueue_check, "termqueue_check");
}

/*
 * termqueue_enqueue - Enter a request into the terminal port queue
 *
 * This routine is used to enter a request into the terminal port queue.
 * We are passed the user's context handle and a pointer to the queue
 * dispatch vector.
 */

termqueue *termqueue_enqueue (
    void *handle,			/* User's context handle */
    queuevector *vector)		/* Pointer to dispatch vector */
{
    termqueue *p;

    p = malloc(sizeof(termqueue));	/* Try to get a queue entry */
    if (p) {				/* If we got one */
	GET_TIMESTAMP(p->time);		/* Set the time of entry */
	p->handle = handle;		/* Save the user's context handle */
	p->v = vector;			/* Save the dispatch vector pointer */
	p->id = termqueue_next_id++;	/* Set the entry ID */
	if (termqueue_next_id == 0) {	/* Handle counter wrap */
	    termqueue_next_id++;
	}
	if (!termqueue_queuehead) {	/* If the queue is now empty */
	    termqueue_queuehead = p;	/* Make this the queue head */
	} else {			
	    termqueue_queuetail->next = p; /* Set the previous link to next */
	}
	termqueue_queuetail = p;	/* And make this the new tail */
	if (termqueue_debug) {
	    buginf("\nTMQ: Enqueued entry %d, handle = %x, ptr = %x",
		   p->id, handle, p);
	}
    }
    return(p);				/* Return pointer or NULL */
}

/*
 * show_termqueue - Display a list of all of the queued entries
 */

void show_termqueue (parseinfo *csb)
{
    termqueue *entry;			/* Pointer to entry being checked */
    termqueue *next_entry;		/* Next entry to display */

    if (!termqueue_queuehead) {
	printf("Queue is empty\n");
	return;
    }

    next_entry = termqueue_queuehead;	/* Set the initial "next entry" */

    automore_enable(NULL);
    for (entry = termqueue_queuehead; entry != NULL; entry = entry->next) {
	if (entry == next_entry) {
	    next_entry = entry->next;	/* Get next entry to display */
	    printf("\n%-5d waiting ", entry->id); /* Display entry id */
	    print_dhms(entry->time);	/* Display elapsed time */
	    printf(" for ");		/* And some more text */
	    (*(entry->v->vshow))(entry->handle); /* Display entry */
	    entry = termqueue_queuehead; /* Start the search over again */
	}
    }

    if (next_entry) {			/* If we didn't find an entry */
	printf("(Queue changed during display)\n"); /* Say the queue changed */
    }
    automore_disable();
}

/*
 * delete_termqueue - Delete a queued entry
 *
 * This routine is used to delete an entry in the terminal queue. This is
 * a user interface command.
 */

void delete_termqueue (parseinfo *csb)
{
    int entrynum;
    termqueue *entry;

    entrynum = GETOBJ(int,1);

    if (entrynum != -1) {
	for (entry = termqueue_queuehead; entry != NULL; entry = entry->next) {
	    if (entry->id == entrynum) {
		(*(entry->v->vdelete))(entry->handle); /* Call the callback */
		termqueue_delete(entry); /* Delete the entry */
		printf("[OK]");		/* Say we did it */
		return;			/* And run away */
	    }
	}
	printf("Entry not found");
    } else {
	printf("Illegal entry number");
    }
}

/*
 * termqueue_check - Check terminal port queue for valid entries
 *
 * This routine is used to check to see if any pending requests can be
 * executed. It is called whenever a terminal becomes free, when rotary
 * group membership changes, or by LAT when the rotary group associated
 * with a service changes.
 */

void termqueue_check (void)
{
    GET_TIMESTAMP(termqueue_nexttime);
}

static void termqueue_checkall (void)
{
    termqueue *prev;			/* Previous entry in queue */
    termqueue *entry;			/* Entry to check */
    termqueue *q;			/* Temporary queue entry pointer */

    prev = NULL;			/* No previous pointer at first */
    entry = termqueue_queuehead;	/* Start with the first entry */

    while (entry) {			/* Walk entire queue */
	if ((*(entry->v->vstart))(entry->handle)) { /* If entry started */
	    if (termqueue_debug) {
		buginf("\nTMQ: Starting entry %d, handle = %x, ptr = %x",
		       entry->id, entry->handle, entry);
	    }
	    q = entry->next;		/* Save link to next entry */
	    free(entry);		/* Free this entry */
	    entry = q;			/* Get the next entry */
	    if (prev) {			/* If there was a previous entry */
		prev->next = q;		/* Update its link to next */
	    } else {			/* Otherwise */
		termqueue_queuehead = q; /* Set the new head pointer */
	    }
	    if (entry == NULL) {	/* If we deleted the tail */
		termqueue_queuetail = prev; /* Set the new previous pointer */
		if (prev == NULL) {	/* If we made the queue empty */
		    termqueue_queuehead = NULL; /* Fix head pointer as well */
		}
	    }
	} else {
	    prev = entry;		/* Not a match, set new previous */
	    entry = entry->next;	/* And get the link to next */
	}
    }
}

/*
 * termqueue_delete - Internal entry point to delete a queue entry
 *
 * This routine does the actual work of deleting a queue entry. It is called
 * by both the high-level user delete function, and internal processes which
 * delete entries due to protocol events.
 */

void termqueue_delete (termqueue *entry)
{
    termqueue *prev = NULL;
    termqueue *q;

    for (q = termqueue_queuehead; q != NULL; q = q->next) {
	if (q == entry)			/* If we found the entry */
	  break;			/* Exit this loop */
	prev = q;			/* Get new previous pointer */
    }

    if (q) {				/* If we found this entry */
	if (prev) {			/* If there is a previous entry */
	    prev->next = entry->next;	/* Update its link to next */
	} else {			/* Otherwise: */
	    termqueue_queuehead = entry->next; /* Set the new list header */
	}
	if (termqueue_queuetail == entry) { /* If we're deleting the tail */
	    termqueue_queuetail = prev;	/* Update the tail pointer */
	}
	if (termqueue_debug) {
	    buginf("\nTMQ: Deleting entry %d, handle = %x, ptr = %x",
		   entry->id, entry->handle, entry);
	}
	free(q);			/* Return the queue block */
    } else {
	errmsg(&msgsym(NOTFOUND, TMQ)); /* Indicate entry not found */
    }
}

/*
 * termqueue_remove
 * Remove an entry from the terminal queue.  This is used if the queue entry
 * is activated by something other than termqueue-check - it removes the
 * entry as though termqueue_check had been successfull.
 */

void termqueue_remove (void *handle)
{
    termqueue *entry;

    for (entry = termqueue_queuehead; entry != NULL; entry = entry->next)
	if (entry->handle == handle) {
	    termqueue_delete(entry); /* Delete the entry */
	    return;
	}
    buginf("\nTMQ: handle %x not found in queue", handle);
}

/*
 * termqueue_periodic
 * Periodically check the terminal queue, since events can happen that
 * we don't notice.  For example, a resource that a protocol translation
 * needs can free up at the far end of a network.
 */
void termqueue_periodic (void)
{
    if (AWAKE(termqueue_nexttime)) {
	termqueue_checkall();
	TIMER_START(termqueue_nexttime, polling_interval*ONESEC);
    }
}

/*
 * Terminal-queue entry-retry-interval subcommand
 * Set the queue entry retry interval parameter
 * Argument must be in units of seconds <0 255>
 */
void termqueue_interval_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(polling_interval != TERMQUEUE_INTERVAL,
		 "%s %d", csb->nv_command, polling_interval);
	return;
    }
    
    polling_interval = (csb->sense) ? GETOBJ(int,1) : TERMQUEUE_INTERVAL;
}
