/* $Id: refresh.c,v 3.1 1995/11/09 09:09:49 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/refresh.c,v $
 *------------------------------------------------------------------
 * refresh.c -- Level 7 NMI Handling
 *
 * October 19, 1989, Chris Shaker
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: refresh.c,v $
 * Revision 3.1  1995/11/09  09:09:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:07  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1993/01/30  01:12:18  whp4
 * Add RP1 support.
 *
 * Revision 1.2  1992/04/18  00:00:40  clev
 * Added better support for shared memory parity errors and support for
 * EMT_ENABLE_DCACHE (and disable).  Also, added support for break enable
 * during boot and better decoding of bus errors.  Init was enhanced to
 * flash an error number in the run LED should the power-on confidence
 * test fail.  Init was also improved to initialize interrupt vectors up
 * to 0x200.
 *
 * Revision 1.1  1992/04/15  02:11:56  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * On Pancake, refresh is done by hardware and the millisecond clock
 * is handled by our caller.  We check for the <Break> key and may do
 * other things later:
 */

#include "../defs.h"
#include "../stdio.h"
#include "../i8254.h"
#include "../reason.h"
#include "../extern.h"

refresh()
{
    volatile int i;
    
    /*
     * Restart watchdog so it barks later if we don't restart it again first:
     */
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
    ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
    ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;
    
    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    if ((uartaddr->a.aux2 & BREAKXA) && (config.breakenable ||
	((clock < 60000) && (hclock == 0)))) {
	if (brk) {
	    abort = TRUE;
	    brk = 0;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	} else {
	    brk = 1;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	}
    }
#ifdef	lint
    i = i;
#endif
}

/*
 * t_disable
 * Disable timer/refresh interrupt
 */
t_disable()
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
}



/*
 * t_enable
 * Enable timer/refresh interrupt
 */

t_enable()
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
    ctaddr->counter2 = REFRFREQUENCY & 0xFF;
    ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;
}
