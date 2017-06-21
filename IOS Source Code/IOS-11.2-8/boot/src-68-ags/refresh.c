/* $Id: refresh.c,v 3.1 1995/11/09 09:08:40 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/refresh.c,v $
 *------------------------------------------------------------------
 * refresh.c -- Level 7 NMI Handling
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refresh.c,v $
 * Revision 3.1  1995/11/09  09:08:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:12  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/01/30  01:11:12  whp4
 * Add RP1 support.
 *
 * Revision 1.1  1992/03/07  23:56:29  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * For the Pippin processors, refresh is done by hardware
 * and the millisecond clock is handled by our caller.  We check for the
 * BREAK key and may do other things later.
 */

#include "../defs.h"
#include "../stdio.h"
#include "../i8254.h"
#include "../reason.h"
#include "../extern.h"

refresh()
{

    /*
     * Restart watchdog so it barks later (maybe)
     */
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
    ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
    ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;
    
    /*
     * Check the break key
     *
     * We have to wait for the break condition to end
     */
    if ((console->rsr & RSR_BRK_END) && config.breakenable) {
	brk = 10;
	console->rsr = (RSR_BRK_END|RSR_BRK_START);
    } else if (brk != 0 && --brk == 0) {
	int i;
	/*
	 * Reset interrupt
	 */
	abort = TRUE;
	while (console->gsr & GSR_A_RXRDY)
	    i = console->rxfifo[0];	/* Empty uart */
#ifdef	lint
	i = i;
#endif
    }
}

/*
 * t_disable
 * Disable timer interrupt
 */

t_disable()
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
}

/*
 * t_enable
 * Enable timer interrupt
 */

t_enable()
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
    ctaddr->counter2 = REFRFREQUENCY & 0xFF;
    ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;
}
