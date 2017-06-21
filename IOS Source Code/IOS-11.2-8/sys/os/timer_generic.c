/* $Id: timer_generic.c,v 3.2 1996/01/06 05:17:01 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/timer_generic.c,v $
 *------------------------------------------------------------------
 * timer_generic.c - moved from if_timer.c
 *
 * November 1992, Dit Morse
 *
 * timer routines:
 *  - calibrate for spin waits
 *  - spin wait routine
 *  - usec dealy
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_generic.c,v $
 * Revision 3.2  1996/01/06  05:17:01  hampton
 * Split timer specific routines into multiple files.  Move the processor
 * specific routines into the src-xx-xxx directories.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/06  00:12:07  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 * Revision 3.4  1995/12/21  16:56:51  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/12/21  16:29:44  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  18:48:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/timer_generic.h"
#include "../os/platform_clock.h"

/***************************************************************************
 *
 *         timer routines          
 *
 ***************************************************************************/

#define TIMER_DATA(name) timer_data_t name = {0,0,0,0,0}

/*
 * timer_calibrate:
 * Calibrate the spin loop for a particular delay
 */
ulong 
timer_calibrate (ulong tt, boolean print_timer)
{
    TIMER_DATA(td);
    ushort t0, c0 = 0;
    ushort t10, c10 = 10;
    ushort t100, c100 = 100;
    ushort t1000, c1000 = 1000;
    ulong tm;              /* time measured */
    long n;
    long t = (long)tt;
    
    leveltype status;
    
    status = raise_interrupt_level(ALL_DISABLE);
    restart_watchdog(64);             /* watchdog for 64ms */
    restart_t2();                     /* restart 4ms timer */
    t0    = time_it(c0);
    t10   = time_it(c10);
    t100  = time_it(c100);
    t1000 = time_it(c1000);
    restart_watchdog(16);             /* watchdog for 16ms */
    reset_interrupt_level(status);
    
    /* calibrate and verify */

    /* t = a0 + a1*n */
    /* so:   a1 = (time1 - time0) / (count1 - count0)  is the slope */
    /* and:  a0 = (time1) / (a1*count1) is the offset */
    
    /* 
     * but the numbers loose all precision in integer arithmetic
     * so carry the calculations out 
     */

    td.t_del = t1000 - t100;
    td.c_del = c1000 - c100;
    td.a0 = (t1000 * 100) - ( (100 * c1000 * td.t_del) / td.c_del ) ;
    td.a1 = (100 * td.t_del) / td.c_del;
    if ( td.a0 < 0 )
	td.a0 = t0;       /* don't permit bogus x-axis crossing */
    td.t0 = t0;
    n = ((((( t * 100  - td.a0) * td.c_del ) / td.t_del) / 100 ) + 1);

    if ( n < 0 )
	n = 0;       /* machine too slow to time this, take minimum */

    /* retime using the calculated counts */

    if (print_timer) {
	
	status = raise_interrupt_level(ALL_DISABLE);
	restart_watchdog(64);		/* watchdog for 64ms */
	restart_t2();
	tm  = time_it(n);
	t0    = time_it(c0);
	t10   = time_it(c10);
	t100  = time_it(c100);
	t1000 = time_it(c1000);
	restart_watchdog(16);		/* watchdog for 16ms */
	reset_interrupt_level(status);
	
	printf("\ncalibration counts and times");
	printf("\n        t = a0 + a1 * n");
	printf("\n    (n = 0 is call/ret overhead)\n");
	
	printf("\nusec=%02d  usec measured=%02d, count=%02d\n\n", t, tm, n);
	
#define PT(N,t)  \
	printf("%4d   %4d    %4d      %4d\n", N, t, \
	       ( (td.a0 + ( (100 * N * td.t_del) / (td.c_del) ) ) / 100), \
	       (  td.t0     + ( (N * td.t_del) / (td.c_del) ) )  \
	       );
	
	
	printf("t0 = %d, a0*100 = %d, a1*100 = %d\n", 
	       td.t0,  td.a0,    td.a1);
	printf("N      t     CalcA0  CalcT0\n");
	PT(c0,t0);
	PT(c10,t10);
	PT(c100,t100);
	PT(c1000,t1000);
	
    }

    return((ulong)n);
}

void wastetime (ulong n)
{
    for ( ; n > 0; n--)
	;
}

void msecdelay (uint msecs)
{
    while (msecs > 0) {
	usecdelay(1000);
	--msecs;
    }
    return;
}

/* End of file */
