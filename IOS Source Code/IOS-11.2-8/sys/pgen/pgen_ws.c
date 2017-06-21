/* $Id: pgen_ws.c,v 3.2 1995/11/17 18:56:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pgen/pgen_ws.c,v $
 *------------------------------------------------------------------
 * P G E N _ W S . C
 *
 * Packet generator workspace manipulation routines. The workspace is
 * just a buffer queue, and these routines manage it. Note that packets
 * are often both in the workspace and in an output interface hold
 * queue; that's OK because the output queues are element queues and don't
 * use the "next" field in the buffer at all. Reference counts are adjusted
 * to make sure that drivers don't return the buffers to the free pools.
 *
 * Many of these routines take unsigned packet indices as parameters;
 * the first packet in the workspace is packet 1. None of these routines
 * issues any error messages; that's up to their callers.
 *
 * Copyright (c) 1992-1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_ws.c,v $
 * Revision 3.2  1995/11/17  18:56:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:18:11  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "pgen_private.h"

/* pgen_wscount returns the number of packets in the workspace */
unsigned pgen_wscount (queuetype *wspace)
{
    return QUEUESIZE(wspace);
}

/* pgen_wspick returns the n'th element of the workspace */
paktype *pgen_wspick (
    queuetype *wspace,
    unsigned pos)		/* Which element to return */
{
    return queuepick (wspace, pos-1);
}

/* pgen_wsnew creates a new element at the given position in the workspace.
 * It's OK to specify a position that's "off the end"; the packet gets
 * added at the end of the workspace.
 * 
 * This routine sets up the packet for generator use, initializing all
 * the fields that need to be set up, but aren't visible to the user.
 * The caller is responsible for setting up the user attributes, including
 * output interface, length, repeat count, name, and both versions of
 * the time-to-send field.
 *
 * The size given is the amount of usable space needed, not the nominal
 * size used for the packet in the buffer pool.
 */
paktype *pgen_wsnew (
    queuetype *wspace,		
    unsigned pos,		/* Index the new packet should get */
    unsigned size)		/* Size new packet buffer should have */
{
    paktype *pak;		/* Pointer to the packet buffer */
    unsigned curpos;		/* For chaining through the workspace */
    paktype * curptr;		/* For chaining through the workspace */
    int netsize;		/* "Nominal" OS buffer mgt packet size */

    /* Compute the packet size we need to request to hold our data */
    netsize = size - ENCAPBYTES;
    if (netsize <= 0) netsize = 1;

    /* Get the packet buffer */
    pak = getbuffer(netsize);
    if (pak == NULL) return NULL;

    /* Set up the fields in the buffer header */
    pak->linktype = LINK_LOOP;	        /* Has to be something... */
    pak->datagramstart = pak->data_area;/* Use encapsulation area fully */
    pak->datagramsize = 0;	        /* Caller should reset */

    /* Insert the packet into the workspace */
    if ((pos <= 1) || (wspace->qhead == NULL)) {
	pak->next = wspace->qhead;
	wspace->qhead = pak;
    } else {
	/* Find the packet that will be before ours */
	for (curpos = 1, curptr = wspace->qhead;
	     (curpos < pos - 1) && (curptr->next != NULL);
	     curpos++, curptr = curptr->next);
	/* Link ours in */
	pak->next = curptr->next;
	curptr->next = pak;
    }
    /* If we've added a packet at the end of the queue, fix the tail pointer */
    if (pak->next == NULL)
	wspace->qtail = pak;
    /* Update the queue element counter */
    wspace->count++;

    return pak;
}

/* pgen_wsgrow replaces the buffer at the given position in the workspace
 * with one of at least the specified size, and returns a pointer to the new
 * buffer. If the specified packet doesn't exist or it can't get a large
 * enough buffer, it returns NULL and doesn't modify the workspace.
 */
paktype *pgen_wsgrow (
    queuetype *wspace,
    unsigned pos,		/* Index of the packet to act on */
    unsigned size)		/* New size */
{
    paktype *pak;		/* The packet we act on */
    paktype *newpak;		/* The new copy of the packet we act on */
    paktype *prevpak;		/* The packet before the one we act on */
    paktype *nextpak;		/* The packet after the one we act on */
    int netsize;		/* OS/buffer mgt view of required size */

    /* Figure out what the buffer management code will think is the
     * needed packet size.
     */
    netsize = size - ENCAPBYTES;
    if (netsize <= 0) netsize = 1;

    /* Find the packet before the one we want to act on */
    if (pos > 1) {
	prevpak = pgen_wspick (wspace, pos - 1);
	if (prevpak == NULL) return NULL;
	pak = prevpak->next;
    } else {
	prevpak = NULL;
	pak = wspace->qhead;
    }
    /* Is there such a packet as the one we've been asked to do? */
    if (pak == NULL) return NULL;

    /* Unlink this packet from the chain. Get a new, presumably bigger,
     * packet, and copy the data into it.
     */
    nextpak = pak->next;
    pak->next = NULL;

    newpak = pak_grow(pak, pak->datagramsize, netsize);
    /* If we couldn't get a new packet, put the old one back into the queue */
    if (newpak == NULL) {
	pak->next = nextpak;
	return NULL;
    }

    /* Link the new packet into the chain in place of the old. */
    newpak->next = nextpak;
    if (prevpak != NULL) {
	prevpak->next = newpak;
    } else {
	wspace->qhead = newpak;
    }

    /* If we've changed the last packet in the queue, fix the tail pointer */
    if (wspace->qtail == pak)
	wspace->qtail = newpak;

    return newpak;
}

/* pgen_wsdel removes a selected packet from the workspace */
void pgen_wsdel (
    queuetype *wspace,
    unsigned pos)		/* Number of packet to remove */
{
    paktype *pak;		/* Packet we're removing */
    paktype *prevpak;		/* Packet before the one we're removing */

    /* Find the packet we want to act on (and the one before it)*/
    if (pos > 1) {
	prevpak = pgen_wspick (wspace, pos - 1);
	if (prevpak == NULL) return;
	pak = prevpak->next;
    } else {
	prevpak = NULL;
	pak = wspace->qhead;
    }
    /* Is there such a packet as the one we've been asked to do? */
    if (pak == NULL) return;

    /* Unlink the packet from the workspace */
    if (prevpak != NULL)
	prevpak->next = pak->next;
    else
	wspace->qhead = pak->next;
    pak->next = NULL;

    /* If this packet has a name, we need to free the storage for that */
    if (pak->pgen_nameptr != NULL)
	free (pak->pgen_nameptr);

    /* Throw it back... */
    datagram_done (pak);

    /* Fix the tail pointer if the packet we just got rid of was the last */
    if (wspace->qtail == pak)
	wspace->qtail = prevpak;     /* May be NULL; that's OK. */

    /* Update the count. */
    wspace->count--;

}

/* pgen_wsfreeall cleans up all the packets in the workspace */
void pgen_wsfreeall (queuetype *wspace)
{
    unsigned i;			/* Loop counter */
    unsigned wslen;		/* How many to do */

    /* Find out how long the workspace is. */
    wslen = pgen_wscount(wspace);

    /* Loop through the workspace, freeing all the packets */
    for (i = 0; i < wslen; i++)
	pgen_wsdel (wspace, 1);
}

/* pgen_wsbyname searches the workspace for a packet with a given name,
 * and returns its index. Returns 0 if it can't find a packet with the
 * desired name.
 */
unsigned pgen_wsbyname (
    queuetype *wspace,
    char *targname)		/* Name to search for */
{
    paktype *pak;		/* Packet being scanned */
    unsigned i;			/* Used to count the index */

    for (i = 1, pak = (paktype *) wspace->qhead;
	 pak != NULL;
	 i++, pak = pak->next)
	if ((pak->pgen_nameptr != NULL) &&
	    !strcmp((char *) pak->pgen_nameptr, targname))
	    return i;

    return 0;
}

/* pgen_wssort sorts the packets in a range of the workspace by
 * time-to-send. Ordering within runs of packets with identical
 * time-to-send fields is preserved. The algorithm is a basic
 * bubble sort.
 */
void pgen_wssort (queuetype *wspace, unsigned int firstpos, unsigned int lastpos)
{
    paktype *predpak;		/* Packet before first in sort range */
    paktype *firstpak;		/* First packet in sort range */
    paktype *prevpak;		/* Packet before the "current" one */
    paktype *curpak;		/* "current" packet */
    paktype *succpak;		/* Packet after last in sort range */
    paktype *swappak;		/* Temporary for swapping packets */
    boolean sorted;		/* All in order? */

    /* Both ends of the range must exist... */
    if (firstpos == 0)
	firstpos = 1;
    if (firstpos > pgen_wscount(wspace))
	firstpos = pgen_wscount(wspace);
    if (lastpos == 0)
	lastpos = 1;
    if (lastpos > pgen_wscount(wspace))
	lastpos = pgen_wscount(wspace);

    /* It's pointless to sort a single packet */
    if (firstpos == lastpos) return;

    /* Start by finding the bounds of the sublist we're going to sort. */
    if (firstpos > 1) {
	predpak = pgen_wspick (wspace, firstpos - 1);
	firstpak = predpak->next;
    } else {
	predpak = NULL;
	firstpak = wspace->qhead;
    }
    succpak = (pgen_wspick (wspace, lastpos))->next;	/* May be NULL */

    /* The actual sort. We run the outer loop until we've done a pass
     * without finding any out-of-order packets. The inner loop just
     * runs through the list. If it finds a pair of packets out of
     * order, it swaps them and continues.
     */
    do {
	sorted = TRUE;		/* Innocent until proven guilty */

	for (curpak = firstpak, prevpak = predpak;
	     curpak->next != succpak;
	     prevpak = curpak, curpak = curpak->next) {

	    /* If curpak and curpak->next are out of order, swap them */
	    if (curpak->pgen_ttosend > curpak->next->pgen_ttosend) {

		/* Swap the packets in the list. */
		if (prevpak != NULL)
		    prevpak->next = curpak->next;
		else
		    wspace->qhead = curpak->next;

		swappak = curpak->next->next;
		curpak->next->next = curpak;
		curpak->next = swappak;

		/* Adjust the tail pointer if needed */
		if (curpak->next == NULL)
		    wspace->qtail = curpak;

		/* If we just swapped the first two packets, we have
		 * a new firstpak.
		 */
		if (firstpak == curpak) {
		    if (prevpak != NULL)
			firstpak = prevpak->next;
		    else
			firstpak = wspace->qhead;
		}

		/* Start again from where we left off. */
		if (prevpak != NULL) 
		    curpak = prevpak->next;
		else
		    curpak = wspace->qhead;

		/* Note that it's not sorted yet */
		sorted = FALSE;
	    }
	}
	
	/* Let other processes run between passes */
	process_may_suspend();

    } while (!sorted);
} 

#ifdef NOTDEF    
/* pgen_wssort sorts the packets in a range of the workspace by
 * time-to-send. Ordering within runs of packets with identical
 * time-to-send fields is preserved. The algorithm is an insertion
 * sort. It's n-squared, and a bubble sort would probably run a lot
 * faster on most of the data we'll get. It does, however, have the
 * great advantage of keeping the list manipulation simple. We just
 * create a new, sorted list from the target segment of the old list,
 * and slip it into place when it's done.
 */
void pgen_wssort (wspace, firstpos, lastpos)
    queuetype *wspace;
    unsigned firstpos;
    unsigned lastpos;
{
    queuetype newws;		/* The sorted list we create */
    paktype *predpak;		/* Packet before first in sort range */
    paktype *pak;		/* Packet we're working on */
    paktype *succpak;		/* Packet after last in sort range */

    /* Both ends of the range must exist... */
    if (firstpos == 0)
	firstpos = 1;
    if (firstpos > pgen_wscount(wspace))
	firstpos = pgen_wscount(wspace);
    if (lastpos == 0)
	lastpos = 1;
    if (lastpos > pgen_wscount(wspace))
	lastpos = pgen_wscount(wspace);

    /* It's pointless to sort a single packet */
    if (firstpos == lastpos) return;

    /* Start by finding the bounds of the sublist we're going to sort. */
    if (firstpos > 1) {
	predpak = pgen_wspick (wspace, firstpos - 1);
	pak = predpak->next;
    } else {
	predpak = NULL;
	pak = (paktype *) wspace->qhead;
    }
    succpak = ((paktype *) newws.qtail)->next;

    /* Now build the new list. This is a double loop; for each element
     * of the old list, we run down the new list until we find the right
     * place, and insert it there.
     */

}	    
#endif NOTDEF
