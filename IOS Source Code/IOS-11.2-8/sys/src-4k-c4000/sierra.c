/* $Id: sierra.c,v 3.3.62.2 1996/08/28 13:11:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra.c,v $
 *------------------------------------------------------------------
 * Sierra specific routines
 *
 * Oct 1993, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:58:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:11:53  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  17:50:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:40:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:19:00  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:53:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "logger.h"
#include "../os/free.h"
#include "../asm-mips/asm_4k_c4000.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Check for an overtemperature condition,
 * and print an alert message if so:
 */
void check_for_sierra_overtemp (void)
{
    uchar status;

    if (cpu_type != CPU_SIERRA)
	return;

    /*
     * Check for overtemperature:
     * 		register B set to status by les_registry_init()
     * 		read status
     * 		
     */
    status = IO_ASIC->sys_stat1;
    if (!(status & CONTROL_OVER_TEMP))
	errmsg(&msgsym(OVERTEMP, SYS));
}

/*
 * Control shared memory parity : 1 - Odd, 0 - Even
 */
static void set_shmem_parity (int odd)
{
    if (odd) {
	IO_ASIC->shmem_ctl |= SHMEM_PARITY_ODD;		/* set odd parity */
    } else {
	IO_ASIC->shmem_ctl &= ~SHMEM_PARITY_ODD;	/* set even parity */
    }
    flush_io_wbuffer();
}

/*
 * Set Nevada Registers A, B and C as follows:
 *	A status
 *	B status
 *	C control
 */
void set_nevada_regs (void)
{
    if (cpu_type != CPU_SIERRA)
	return;

    /*
     * 0 means status
     * 1 means control
     * Bit 0 - Reg A. This is status for Sierra
     * Bit 1 - Reg B. This is status for Sierra
     * Bit 2 - Reg C. This is control for Sierra
     *
     * Note: We will initialize the register to its passive
     * state before configuring Nevada to drive this register.
     */
    IO_ASIC->sys_stat2 = 0xff;
    IO_ASIC->asic_reg = ASIC_CONT_C;
    flush_io_wbuffer();
}


/*
 * flush_shmem_wbuffer:
 * Flush the Shared Memory write buffer
 */
int flush_shmem_wbuffer (void)
{
    return( *(int *)(ADRSPC_SHAREDMEM) );
}

#if 0					/* not used */
/*
 * flush_dram_wbuffer:
 * Flush the DRAM write buffer
 */
int
flush_dram_wbuffer (void)
{
    return( *(int *)(ADRSPC_RAM) );
}

/*
 * flush_all_wbuffers:
 * Flush all write buffers
 */
void
flush_all_wbuffers (void)
{
    flush_dram_wbuffer();
    flush_shmem_wbuffer();
    flush_io_wbuffer();
}
#endif

/*
 * Test the system run LED and the NIM LEDs. It's a Xmas tree !!!
 * Then restore the original values
 */
void test_leds (parseinfo *csb)
{
    ushort shmem_ctl_copy;
    int i;

    shmem_ctl_copy    = IO_ASIC->shmem_ctl;
    for (i = 0; i < 30; i++) {
      /*
       * Turn all LEDs off
       */
      IO_ASIC->sys_ctl |= CONTROL_RUNLAMP;
      IO_ASIC->shmem_ctl |= SHMEM_SLOT_LEDS;
      flush_io_wbuffer();
      process_sleep_for(100);          /* Let other processes run again */
      /*
       * Turn all LEDs on
       */
      IO_ASIC->sys_ctl &= ~CONTROL_RUNLAMP;
      IO_ASIC->shmem_ctl &= ~SHMEM_SLOT_LEDS;
      flush_io_wbuffer();
      process_sleep_for(100);          /* Let other processes run again */
    }
    /*
     * Restore old NIM LEDs status
     */
    IO_ASIC->shmem_ctl = shmem_ctl_copy;
    flush_io_wbuffer();
}

/*
 * test_parity_io:
 * Test IO memory parity errors
 */
void test_parity_io (void)
{
    volatile uint *address;
    mempool *mempool;
    uint data;
    leveltype level;

    mempool = mempool_find_by_class(MEMPOOL_CLASS_IOMEM);

    address = (volatile uint *)mempool->memhead;
    printf("Creating a RAM parity error in IO memory at address %#x...\n",
	   address);
    flush();
    process_sleep_for(100);		/* let output out */

    level = set_interrupt_level(LEVEL_CONSOLE); /* allow parity interrupt */
    set_shmem_parity(0);		/* set even parity */
    data = *address;			/* cause parity error */
    set_shmem_parity(1);	        /* set odd parity */
    reset_interrupt_level(level);
}


/*
 * platform_is_c4700
 * Return TRUE if we are running on a c4700
 */
boolean platform_is_c4700 (void)
{
    /*
     * If there is a level 2 cache and the pipeline of
     * the processor is going faster than 100Mhz, it is a c4700
     */
    if (level2_cache_present && (r4k_pipe_speed > 100)) {
      return(TRUE);
    } else {
      return(FALSE);
    }
}


/*
 * platform_is_c4500
 * Return TRUE if we are running on a c4500/c4500m
 */
boolean platform_is_c4500 (void)
{
    /*
     * If there is no level 2 cache and the pipeline speed
     * is 100Mhz, then it is a c4500
     */
    if (!level2_cache_present && (r4k_pipe_speed == 100)) {
      return(TRUE);
    } else {
      return(FALSE);
    }
}


