/* $Id: refresh.c,v 3.1 1995/11/09 09:10:29 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/refresh.c,v $
 *------------------------------------------------------------------
 * refresh.c -- Level 7 NMI Handling
 *
 * August 1986, Greg Satz
 * April 1992, Gary Kramling
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refresh.c,v $
 * Revision 3.1  1995/11/09  09:10:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1993/12/16 12:05:41  sthormod
 * ROM monitor support for nvram config compress & s/w config register.
 *
 * Revision 1.1  1993/09/24  16:34:33  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:12:02  whp4
 * Add RP1 support.
 *
 *
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
#include "pcmap.h"

int w_disable(); /* prototype */

refresh()
{
    volatile ulong env_result;
    volatile ulong *env_status;
    volatile ushort *env_control;

    env_status = (ulong *) ADRSPC_ENV_STS;
    env_control = (ushort *) ADRSPC_ENV_CTL;

    env_result = *env_status;
    if ((env_result & SUPPLY1_PF) && (env_result & SUPPLY2_PF))
        /* got powerfail on both supplies */
        if (pf_info.pf_addr) { /* ie. in sys. code and initialized */
            w_disable(); /* in case this is an intermittant pf */
            (*(void (*)())pf_info.pf_addr)();
        }

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
    if ((console->rsr & RSR_BRK_END) && (config.breakenable || 
       ((clock < 60000) && (hclock == 0)))) {
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
 * w_disable
 * Disable watchdog interrupt
 */

w_disable()
{
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
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
