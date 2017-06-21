/* $Id: c7100_ukernel.c,v 3.1.2.1 1996/03/21 23:48:18 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k-c7100/c7100_ukernel.c,v $
 *------------------------------------------------------------------
 * c7100_ukernel.c -- Predator specific routines necessary to build
 *		      sub_ukernel.o 
 *
 * Michael Beesley, March 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_ukernel.c,v $
 * Revision 3.1.2.1  1996/03/21  23:48:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1  1996/03/19  04:07:19  mbeesley
 * Placeholder files for Port Ready
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "packet.h"

#include "../os/gdb.h"

#include "../pas/if_pas.h"

#include "../src-4k/gt64010.h"
#include "../src-4k/gt64010_dma.h"

#include "c7100_fpga.h"

/*
 * flush_io_wbuffer:
 * Flush the IO write buffer
 */
int flush_io_wbuffer (void)
{
    volatile uchar *ptr = (volatile uchar *)(PHY_TO_KSEG1(PHY_ADRSPC_PROM));

    return(*ptr);
}

/*
 * write_berr_interrupt:
 * Handle hardware specific actions if we take a write bus error
 * interrupt. These are handled in the c7100 error interrupt handler.
 */
void
write_berr_interrupt (int code, r4k_context_t *sc)
{
}

/*
 * restart_watchdog:
 * Restart watchdog so it barks later if we don't restart it again first.
 */
void restart_watchdog (int t) 
{
    /*
     * Write to the Watchdog register to make sure
     * it is reset, and will give me 32msec before barking
     */
    C7100_IO_FPGA->watchdog_reset = 1;
    flush_io_wbuffer();

    /*
     * Enable the Watchdog register
     */
    C7100_IO_FPGA->led_reg |= WATCHDOG_ENABLE;
    flush_io_wbuffer();
}

/*
 * disable_watchdog:
 * Disable the watchdog timer. Return its setting if it was enabled,
 * else return zero.
 */
int disable_watchdog (void)
{
    uchar setting;

    setting = C7100_IO_FPGA->led_reg;
    C7100_IO_FPGA->led_reg &= ~WATCHDOG_ENABLE;
    flush_io_wbuffer();
    if (setting & WATCHDOG_ENABLE) {
	return(1);
    } else {
	return(0);
    }
}

/*
 * restart_t2:
 * Enable refresh timer (does not produce interrupts). Not used on C7100.
 */
void restart_t2 (void)
{
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

