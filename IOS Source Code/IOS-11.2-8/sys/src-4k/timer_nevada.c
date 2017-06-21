/* $Id: timer_nevada.c,v 3.2.56.1 1996/08/19 19:02:00 dkatz Exp $ 
 * $Source: /release/112/cvs/Xsys/src-4k/timer_nevada.c,v $
 *------------------------------------------------------------------
 * timer_nevada.c: Moved from if_4k_timer.c
 *      Timer suppor routines for R4k platforms
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_nevada.c,v $
 * Revision 3.2.56.1  1996/08/19  19:02:00  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2  1996/01/06  05:17:04  hampton
 * Split timer specific routines into multiple files.  Move the processor
 * specific routines into the src-xx-xxx directories.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/06  00:12:18  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 * Revision 3.4  1995/12/21  16:56:48  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/12/21  16:29:42  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  18:48:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:29:18  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:55:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/timer_generic.h"
#include "../os/platform_clock.h"
#include "../os/gdb.h"

#if defined(SIERRA)
#include "../src-4k/nevasic.h"
#include "../src-4k-c4000/pcmap.h"
#endif

/*************************************************************************
 * time_it:
 *     spin waits for count of cnt,
 *     returns loop time in usecs.
 *************************************************************************/

/*
 * time_it:
 * Perform time measurements
 */
int time_it (int cnt)
{
    unsigned long before, after, time;

    before = getcp0_count();

    WASTETIME(cnt);

    after = getcp0_count();
    
    if (before > after) {
	time = (0xffffffffU - before) + after;
    } else {
	time = after - before;
    } 
    
    /* Convert to microseconds and return. */

    return ((int) time / (R4K_CLOCK_1MS_INCR / 1000));
}

/*
 * restart_count_compare :
 * Reinitialize Count/Compare and restart watchdog if dog TRUE
 */
void restart_count_compare (int dog)
{
    setcp0_count(0);		   		/* set Count register	*/
    setcp0_compare(R4K_CLOCK_4MS_INCR);  	/* set Compare register	*/
    if (dog)
	restart_watchdog(16);	     		/* restart the dog	*/
}

/***************************************************************************
 *
 *         time delay routines          
 *
 ***************************************************************************/
/*
 * usecdelay:
 * Delay a number of micro seconds
 * Note: On r4600 based boxes that have 150Mhz internal CPU pipeline
 * speed, we assign the ns per count tick to be 13 as opposed to
 * 13.3333 ns. This causes usecdelay() to delay very slightly
 * longer than it should.
 */
void usecdelay (unsigned n)
{
    usec_timestamp timestamp;

    while (n > 0) {
	unsigned usecs;

	if (n > 1000*1000)
	    usecs = 1000*1000;
	else
	    usecs = n;
	USEC_TIMER_START(timestamp, usecs);
	while (!USEC_AWAKE(timestamp))
	    ;
	n -= usecs;
    }
    return;
}

/*
 * get_timer_residual:
 * Get the current value of the timer residual.
 *
 * Note: we are assuming that the changes to fddi support have
 * been incorporated and we are allways running on a 4ms tick
 */
static ulong get_timer_residual (void)
{
    leveltype oldsr;
    int diff;
    int count, comp;

    /*
     * Grab the counter and compare registers.  The counter counts up
     * to the compare and then causes an interrupt (at which point the
     * compare register is jacked up higher).  The counter is thus
     * continuous, and the compare jumps in 4msec steps.
     *
     * Since the compare register tells the time for the *next* interrupt,
     * we need to subtract the interval from it to get the time of the last
     * interrupt.
     */
    oldsr = raise_interrupt_level(ALL_DISABLE);
    comp = getcp0_compare();
    count = getcp0_count();
    reset_interrupt_level(oldsr);
    diff = count - (comp - R4K_CLOCK_4MS_INCR);
    if (diff < 0)			/* Something's really wrong */
	diff = 0;
    return(diff);
}

/*
 * get_timer_parameters:
 * Get information about the hardware timer (used by the system clock
 * support)
 */
void get_timer_parameters (ulong *freq_numerator, ulong *freq_denominator,
		      ulong *freq_divisor, 
		      get_resid_type *get_residual,
		      ulong *precision)
{
    *freq_numerator = NMI_FREQ_NUMERATOR;
    *freq_denominator = NMI_FREQ_DENOMINATOR;
    *freq_divisor = R4K_CLOCK_4MS_INCR;
    *get_residual = get_timer_residual;
    *precision = 24;		/* 20ns clock */
}

/* End of file */

