/* $Id: rsp_ukernel.c,v 3.1.20.2 1996/03/21 23:50:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rsp/rsp_ukernel.c,v $
 *------------------------------------------------------------------
 * rsp_ukernel.c -- C7500 specific routines necessary to build
 *		    sub_ukernel.o 
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ukernel.c,v $
 * Revision 3.1.20.2  1996/03/21  23:50:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.20.1  1996/03/18  22:02:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:13:09  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:30  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "rsp_pcmap.h"
#include "../asm-mips/asm_4k_rsp.h"

tinybool rsp2 = FALSE;

int disable_watchdog (void)
{
    int prev_setting;
    volatile uchar *countctl;
    volatile ushort *sreg;

    /*
     * On RSP2s disable the master/slave watchdog.  How much time
     * is left on this timer doesn't matter since we always  max
     * it out anyway.  See comment below in restart_watchdog().
     * So we let the 'primary' watchdog specify the return value.
     */
    if (rsp2) {
	sreg = (ushort *) ADRSPC_MASTER_TIMER_ENABLE;
	*sreg = ~MASTER_TIMER_ENABLE;
    }

    /*
     * Now go ahead and take care of the primary watchdog.
     */
    countctl = (uchar*) ADRSPC_COUNTCTL;
    if (*countctl & COUNTCTL_C0ENABLE)
	prev_setting = 1;
    else
	prev_setting = 0;
    *countctl &= ~COUNTCTL_C0ENABLE;
    *countctl;			/* don't return until write is complete */

    return prev_setting;
}

void restart_watchdog (int msec)
{
    volatile uchar *countctl;
    volatile ushort *counter0;
    volatile ushort *sreg;

    countctl = (uchar*) ADRSPC_COUNTCTL;
    counter0 = (ushort*) ADRSPC_COUNTER0;
    *counter0 = (MSEC_SETTING * msec);
    *countctl |= COUNTCTL_C0ENABLE;
	
    /*
     * On RSP2's start the master/slave watchdog timer.  This is
     * more than a little dodgy.  Currently, the longest period
     * this timer can be set for is 255 * 41usecs, or about 10msecs.
     * The os code that calls this routine typically passes it a
     * parameter of 16msecs, and somtimes of 64msecs.  Hoewver, both
     * of these settings are bogus.  The timer gets stroked every 4msecs,
     * so 16msecs is way conservative.  The OS code sets it to 64msecs
     * when it's about to disable 'all' interrupts, but disabling 'all'
     * interrupts doesn't disable the 4msec heartbeat anyway, so this
     * is unecessary.  In summary, we'll ignore the value passed to
     * this routine, and always set it to it's maximum value (10msecs).
     * A hardware modification is currently under consideration to
     * determine if we can make this timer longer, in which case we
     * should revisit this code, to make use of the parameter passed
     * in. Should such a revisit take place, see also disable_watchdog()
     * above. XXX
     */
    if (rsp2) {
	sreg = (ushort *)ADRSPC_MASTER_TIMER;
	*sreg = MASTER_TIMER_10MSEC;
	sreg = (ushort *) ADRSPC_MASTER_TIMER_ENABLE;
	*sreg = MASTER_TIMER_ENABLE;
    }
}

/*
 * restart_t2:
 * Enable refresh timer (does not produce interrupts)
 */
void restart_t2 (void)
{
    /* not used on RSP */
}

/*
 * write_berr_interrupt:
 * Handle hardware specific actions if we take a write
 * bus error interrupt
 */
void
write_berr_interrupt (int code, r4k_context_t *sc)
{
    /* 
     * interrupt already acknowledged in interrupt wrapper
     * and write bus errors not handled via SIGWBERR
     */
}

/*
 * platform_fault_history:
 * Save platform dependent fault history
 */
void platform_fault_history (int sig, int code, r4k_context_t *sc,
			     char *fh, int fhsize)
{
}

/* end of file */
