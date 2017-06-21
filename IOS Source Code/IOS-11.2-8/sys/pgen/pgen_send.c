/* $Id: pgen_send.c,v 3.4.44.1 1996/03/18 21:35:23 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/pgen/pgen_send.c,v $
 *------------------------------------------------------------------
 * P G E N _ S E N D . C
 *
 * Platform-independent packet timing, enqueueing, and sending stuff
 * for the packet generator. This is where the action is, or at least
 * where the action starts. The main entry point is pgen_send.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_send.c,v $
 * Revision 3.4.44.1  1996/03/18  21:35:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/13  01:52:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.1  1996/02/20  16:58:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/22  21:58:50  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  18:56:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:59:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/14  01:52:13  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.3  1995/08/25  11:44:58  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.2  1995/06/09 13:18:08  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "ttysrv.h"
#include "parser.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "pgen_private.h"

/* external function */

/* pgen_send is a very specialized datagram_out-like function. It
 * tries very hard to get the packet onto the output queue for the interface,
 * and won't return until it's done so, even if it has to dismiss to let
 * the queue drain. Checks for keyboard interrupts when it suspends to
 * avoid hanging up the system forever. Other differences from datagram_out
 * include locking packets so drivers don't return the buffers, honoring
 * packet repeat counts, and *not* doing any of the accounting or error
 * checking cruft. This function doesn't use the idb->oqueue function;
 * it sticks stuff directly on the interface hold queue. PAGENT packets
 * haven't been properly encapsulated for anything but datagram link layers,
 * anyway.
 */
void pgen_send (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak,           /* Last packet to send */
    int repeat,                 /* Times to repeat the packet sequence */
    int delay)                  /* Time to delay between each repeat loop */
{
    paktype *pak;               /* Current packet being sent */
    sys_timestamp starttime;    /* Time at which we start sending */
    sys_timestamp wakeuptime;   /* Constructed sleep timer */
    hwidbtype *hwidb;           /* Output hardware idbe for the packet */
    boolean success;            /* Send success or failure flag */
    int copies;			/* Number of times we've sent this packet */
    int retries;		/* Number of times we've waited for hold q */
    int incr, rptnum;           /* Variables used in repeat loop */

    /*
     * Replay repeat loop.
     * Repeat count of 0 indicates loop forever.
     */
    incr = repeat ? 1 : 0;
    for (rptnum = incr; rptnum <= repeat; rptnum += incr) {

        /* This timing loop is relative to the start time */
        GET_TIMESTAMP(starttime);

        /* Traverse packets to be transmitted loop */
        for (pak = firstpak; pak != lastpak; pak = pak->next) {

            /* If this packet's time has not yet come, wait for it. */
            if (CLOCK_IN_INTERVAL(starttime, pak->pgen_tick)) {
                if (psiset(stdio))
                    return;
		wakeuptime = TIMER_ADD_DELTA(starttime, pak->pgen_tick);
                process_sleep_until(&wakeuptime);
	    }

	    /* Make sure the interface is up */
	    hwidb = hwidb_or_null(pak->pgen_output);
	    if ((!hwidb) || (hwidb->state != IDBS_UP))
	        continue;

	    /* Per packet repeat loop */
	    for (copies = 0; copies < pak->pgen_rptcount; copies++) {

		success = FALSE;	/* Haven't sent it yet... */
		retries = 0;		/* Nor have we retried. */

		while (!success) {
		    /* Try to stick the packet on the output hold queue for
                     * the interface (note that this will massively break
                     * encapsulations like LAPB that expect to do this).
 		     */
		    pak_lock(pak);	/* Hold onto this sucker... */
		    success = holdq_enqueue (hwidb, pak, TRUE);

		    /* If we succeeded, we need to start output on the device
		     * to make sure our packet goes out. If we failed, we need
		     * to start output in case the device is wedged or became
		     * ready while we had interrupts disabled or something.
		     */
		    (*hwidb->soutput) (hwidb);

		    /* Sleep for interval time */
		    if(pak->pgen_interval) {
			if (psiset(stdio))
                                return;
			usecdelay(pak->pgen_interval/1000);
		    }
		    
		    if (!success) {
			/* Remember that this isn't the first time we've tried
			 * to get this packet out... well, the *next* time
                         * won't be the first, anyway.
			 */
			retries++;
			
			/* We're retrying in a pretty tight loop (hey, it's
                         * test code...). If we retry many times and don't
                         * get anywhere, let other processes run. If we still
                         * don't get anything after *lots* of retries, we'll
                         * dismiss for a period of time in the hope that the
                         * logjam (which may very well be of our own making)
                         * will clear. At that point, we'll also check to see
                         * if the user would like to give up.
			 */
			if ((retries % PGEN_RETRYDISMS) == 0)
			    process_may_suspend();
			if ((retries % PGEN_RETRYSUSP) == 0) {
			    process_sleep_for(PGEN_SUSPTIME);
			    if (psiset(stdio))
			        return;
			}
		    }
		}
	    }
	}
    }
}
