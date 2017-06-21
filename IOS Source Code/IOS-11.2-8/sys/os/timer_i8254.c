/* $Id: timer_i8254.c,v 3.3.4.1 1996/08/19 19:01:25 dkatz Exp $ 
 * $Source: /release/112/cvs/Xsys/os/timer_i8254.c,v $
 *------------------------------------------------------------------
 * timer_i8254.c : Timer support routines for M68K platforms
 * from if_68_timer.c
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_i8254.c,v $
 * Revision 3.3.4.1  1996/08/19  19:01:25  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.3  1996/03/11  11:46:17  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Deterge the code.
 *
 * Revision 3.2  1996/01/06  05:17:02  hampton
 * Split timer specific routines into multiple files.  Move the processor
 * specific routines into the src-xx-xxx directories.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/06  00:12:09  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 * Revision 3.4  1995/12/25  03:12:40  hampton
 * The usecdelay() routine never worked on the 2500.  Fix it.  [CSCdi45965]
 *
 * Revision 3.3  1995/12/21  16:56:49  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  18:48:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:29:19  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:55:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"

#if defined(RP1)	/* RP1 */
#include "../../boot/src-68-c7000/defs_rp1.h"
#endif /* RP1 */

#if defined(XX)					/* XX */
#include "../../boot/src-68-c4000/defs_xx.h"
#include "../../boot/src-68-c4000/pcmap.h"
#endif  /* XX */

#if defined(CSC2)				/* AGS */
#include "../../boot/src-68-ags/defs_csc2.h"
#endif /* CSC2 */

#include "../../boot/i8254.h"
#include "../os/timer_generic.h"
#include "../os/timer_i8254.h"
#include "../os/platform_clock.h"

/*************************************************************************
 * time_it:
 *     spin waits for count of cnt,
 *     returns loop time in usecs.
 *************************************************************************/

/* NB - the non-XX stuff includes the special code for the
 *      brut spin on the timer.  the new and improved DEC-Hack!
 */

/* NB - 8254 command settling time.  The 8254-2 chip specs the command
 *      settling time as 165 ns.  This time should occur between
 *      any write and a following read, or any two reads.
 *
 *      We used WASTETIME(5) which is sufficient on clean.
 *
 *      These times do not affect calibration, since the timer runs
 *      from the msb write to the timer register to the command
 *      to latch the current counter value.
 */

/* 
 * All M68k boxes except the 2500/3000 series.
 */

/*
 * time_it:
 * Perform time measurements
 */
int 
time_it (int cnt)
{
    ushort pad = 0x4000;  /* pad this amount so timer never times out */ 
    ushort tmp;

    i8254_start_counter(pad);
    WASTETIME(cnt); 
    tmp = i8254_read_counter();
    i8254_stop_counter();
    return(pad - tmp);
}

/*
 * restart_t2
 * Enable timer/refresh interrupt
 * for BRUT insures 16 ms without interrupt.
 */

void restart_t2 (void)
{
    i8254_set_refresh_frequency();
}

/*
 * restart_watchdog:
 * Restart watchdog so it barks later if we don't restart it again first.
 * You may want to refresh the watchdog timer during your procedure so it
 * does not time out (16ms) and reset the box.  This can be done with the
 * following code.
 */
void 
restart_watchdog (int milliseconds) 
{
    i8254_restart_watchdog(milliseconds);
}

/***************************************************************************
 *
 *         time delay routines          
 *
 ***************************************************************************/
/* (clev: for merlot)
 * wait at least n microseconds 
 * by watching 8254 timer counter 1 while looping.
 * the sequence of instructions is critical in this module due to the
 * slow internal workings of the 8254 chip.
 * especially important is the time between writing the control register
 * and reading something back.
 */
void usecdelay (unsigned n)
{
    unsigned short tmp;
    while (n) {
	tmp = n & 0x7fff;
	if (tmp == 0) tmp = 0x8000;
	n -= tmp;
	if (tmp < 5)
	    continue;
	tmp += 0x4000;  /* pad this amount so timer never times out */
	i8254_start_counter(tmp);
	while (tmp > 0x4000) {  /* break if below our pad value */
	    tmp = i8254_read_counter();
	}
    }

    i8254_stop_counter();
}

/*
 * get_timer_residual
 *
 * Get the current value of the 8254 timer residual.
 */
static ulong get_timer_residual (void)
{
    ulong residual;

    residual = i8254_read_timer_residual();
    if (residual <= REFRFREQUENCY)
	return(REFRFREQUENCY - residual); /* It counts backwards! */
    return(0);				/* Bad news... */
}

/*
 * get_timer_parameters
 *
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
    *freq_divisor = REFRFREQUENCY;
    *get_residual = get_timer_residual;
    *precision = 19;		/* microsecond clock */
}

/* End of file */

