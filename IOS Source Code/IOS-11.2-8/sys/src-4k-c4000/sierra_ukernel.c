/* $Id: sierra_ukernel.c,v 3.1.20.2 1996/03/21 23:47:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k-c4000/sierra_ukernel.c,v $
 *------------------------------------------------------------------
 * sierra_ukernel.c -- C4500 specific routines necessary to build
 *		       sub_ukernel.o 
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sierra_ukernel.c,v $
 * Revision 3.1.20.2  1996/03/21  23:47:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.20.1  1996/03/18  21:58:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:11:59  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:24  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/free.h"
#include "../asm-mips/asm_4k_c4000.h"
#include "../les/if_les.h"
#include "ttysrv.h"

lev2_jumptable_t lev2_jumptable[C4000_NUM_SLOTS];

/*
 * flush_io_wbuffer:
 * Flush the IO write buffer
 */
int flush_io_wbuffer (void)
{
    return(IO_ASIC->asic_reg);
}

/*
 * write_berr_interrupt:
 * Handle hardware specific actions if we take a write
 * bus error interrupt
 */
void write_berr_interrupt (int code, r4k_context_t *sc)
{
    volatile ulong err_addr;

    /*
     * EXL and IE have been de-asserted. Hence interrupts
     * are locked out, but exceptions can occur. To re-enable
     * the external interrupt controller, we have to clear the
     * bus error interrupt and send end of interrupt.
     */
    IO_ASIC->sys_stat2 &= ~(SS2_CLR_ALLBE);	/* assert clear */
    flush_io_wbuffer();				/* flush IO write buffer */
    IO_ASIC->sys_stat2 |= SS2_CLR_ALLBE;	/* de-assert clear */

    /*
     * Clear the EOI bit in the IO asic
     */
    IO_ASIC->int_eoi |= INT_LEV7;
    flush_io_wbuffer();

    /* Read 3 write bus error address registers.
     * Check if it's valid and print it out.
     */

    err_addr = *(volatile ulong *) (ADRSPC_DRAM_WBERR);
    if (err_addr & 0x01) {
        ttyprintf(CONTTY, "%%ERR-1-FATAL: Main memory write bus error interrupt\n");
        ttyprintf(CONTTY, " Error address = 0x%08x\n", (err_addr & 0xFFFFFFFC));
    } 
    err_addr = *(volatile ulong *) (ADRSPC_SHMEM_WBERR);
    if (err_addr & 0x01) {
        ttyprintf(CONTTY, "%%ERR-1-FATAL: Shared memory write bus error interrupt\n");
        ttyprintf(CONTTY, " Error address = 0x%08x\n", (err_addr & 0xFFFFFFFC));
    } 
    err_addr = *(volatile ulong *) (ADRSPC_IO_WBERR);
    if (err_addr & 0x01) {
        ttyprintf(CONTTY, "%%ERR-1-FATAL: I/O memory write bus error interrupt\n");
        ttyprintf(CONTTY, " Error address = 0x%08x\n", (err_addr & 0xFFFFFFFC));
    }   
}

/*
 * restart_watchdog:
 * Restart watchdog so it barks later if we don't restart it again first.
 * You may want to refresh the watchdog timer during your procedure so it
 * does not time out (16ms) and reset the box.  This can be done with the
 * following code.
 */
void restart_watchdog (int t) 
{
    IO_ASIC->ct0 = (MSEC_SETTING*t);
    IO_ASIC->ccr |= CCR_CT0_EN;
    flush_io_wbuffer();
}

/*
 * disable_watchdog:
 * Disable the watchdog timer. Return its setting if it was enabled,
 * else return zero.
 */
int disable_watchdog (void)
{
    int setting = 0;

    if (IO_ASIC->ccr & CCR_CT0_EN) {
	setting = 1;
    }
    IO_ASIC->ccr &= ~CCR_CT0_EN;
    flush_io_wbuffer();
    return(setting);
}

/*
 * restart_t2:
 * Enable refresh timer (does not produce interrupts)
 */
void restart_t2 (void)
{
    IO_ASIC->ct2 = REFRFREQUENCY;
    IO_ASIC->ccr |= CCR_CT2_EN;
    flush_io_wbuffer();
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
