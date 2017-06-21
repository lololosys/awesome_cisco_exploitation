/* $Id: refresh.c,v 3.1 1995/11/09 09:09:21 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/refresh.c,v $
 *------------------------------------------------------------------
 * refresh.c -- Level 7 NMI Handling
 *
 * October 19, 1989, Chris Shaker
 *
 * Copyright (c) 1989-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refresh.c,v $
 * Revision 3.1  1995/11/09  09:09:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:37  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.5  1993/01/30  01:11:25  whp4
 * Add RP1 support.
 *
 * Revision 1.4  1992/10/22  05:35:42  mikehub
 * changed a != to a |= in t_disable_brut().
 *
 * Revision 1.3  1992/08/31  20:38:06  dcoli
 * Changes for Brut.
 *
 * Revision 1.2  1992/05/12  22:20:55  clev
 * - Eliminate support for the physical configuration register on Pan.
 *
 * - Add support for the monitor command "o/r" for Pan and XX.
 *
 * - Break enable during boot on Pan.
 *
 * - Flash support added for Pan.
 *
 * Revision 1.1  1992/03/07  23:57:41  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * On Pancake, refresh is done by hardware and the millisecond clock
 * is handled by our caller.  We check for the <Break> key and may do
 * other things later:
 * The Brut project has its own routine because the Brut timer registers
 * are accessed 16 bits at a time instead of 8 bits at a time.
 * They are also at a different address. The Brut box also doesn't
 * access the control register. This is done in the ASIC. A seperate
 * ISR is provided to prevent run time checks for a Brut box.
 */

#include "../defs.h"
#include "../stdio.h"
#include "../i8254.h"
#include "../reason.h"
#include "../extern.h"

extern cookie_t *cookie;

refresh()
{
    volatile int i;
    
    /*
     * Restart watchdog so it barks later if we don't restart it again first:
     * Do this here, finish after we deal with UART (saves one NOP).
     */
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
    
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

    /*
     * Finish reseting watchdog timer counter:
     */
    ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
    asm("nop");			/* 68030 on Merlot is too fast! */
    asm("nop");
    ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;

#ifdef	lint
    i = i;
#endif
} /* refresh() */

refresh_brut()
{
    volatile int i;
    
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

    /*
     * reset watchdog timer counter:
     */
    *(short *)ADRSPC_BRUT_TIMER0 = REFRFREQUENCY * 4;

#ifdef	lint
    i = i;
#endif
} /* refresh_dec() */

/*
 * t_disable
 * Disable timer/refresh interrupt 
 */
t_disable()
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
}

/*
 * t_disable_brut
 * Disable timer/refresh interrupt for the brut project.
 */
t_disable_brut()
{
    /* set the timer reset bit high */
    *(volatile u_short *)ADRSPC_CONTROL3 |= BRUT_RESET_COUNTER;
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

/*
 * t_enable_brut
 * Enable timer/refresh interrupt for the brut project.
 */
t_enable_brut()
{
    /* clear the timer reset bit */
    *(volatile u_short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_COUNTER);
    /* reload a value into the timer */
    *(short *)ADRSPC_BRUT_TIMER2 = REFRFREQUENCY;
}
