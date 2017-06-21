/* $Id: init.c,v 3.3.54.1 1996/03/21 23:30:46 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/init.c,v $
 *------------------------------------------------------------------
 * init.c
 *
 * April 1993, Michael Beesley and Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Sierra initialization file
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.3.54.1  1996/03/21  23:30:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/11  19:14:55  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:11  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1995/12/03  18:00:01  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:45:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "console.h"
#include "confreg.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "extern.h"
#include "cookie.h"

#include "../../../boot/cpu.h"

struct nvram *nvram_ptr = ((struct nvram *)(KSEG1_ADRSPC_NVRAM));
char *errlog_start = ((char *)(MON_FREE_MEM));
int cpu_type;
char ciscopro_flag;       /* CiscoPro platform */
char *platform_string = "C4500";
char *default_monlib_string = "%c4500-fslib-m";

/* forward declarations */
void init_soft_reset(int reset_type, unsigned long *frame);
void reset_io(void);

struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};

#define NUM_MTEST_VALS   (sizeof(mtestvals)/sizeof(struct mtestvals))
#define NVRAM_TEST_START (long *)((KSEG1_ADRSPC_NVRAM + sizeof(struct nvram) + 8) & ~7)
#define NVRAM_TEST_END   (long *)(MON_INIT_SP)
#define RAM_TEST_START (long *)(KSEG1_ADRSPC_RAM)
#define RAM_TEST_END   (long *)(MON_FREE_MEM)

/*
 * Initial Control     : Run LED Off, 128M DRAM bank size, LED1..LED4 Off,
 *                       Disabled Bus Errors, No Flash Programming
 *
 * Initial S.M Control : Odd Parity, Reset NIMs, LEDs Off, 64MBytes size
 */

#define INITIAL_SHMEM_CONTROL    (SHMEM_PARITY_ODD | SHMEM_SLOT_LEDS | SHMEM_PERR_CLEAR)
#define INITIAL_CONTROL          (CONTROL_RUNLAMP | CONTROL_LED_ALL)
static volatile int reset_type_sav;

/*
 * Initialize the hardware
 *
 * DANGER, DANGER, pay special attention to reset_type and frame.
 * They are not valid after a subroutine call.  They get passed in
 * to this routine in registers a0 and a1 from assembler code and are
 * not saved due to the way that this routine is entered.
 * I further caution you to make an init.lis file after making ANY
 * changes to this module to make sure reset_type and frame are in
 * a0 and a1 respectively.  The simplest of changes can cause the
 * compiler to move these registers around in the small portion of
 * code before our entry label.    - clev
 */
void
init(int reset_type, unsigned long *frame)
{
    int i, conf;
    long *mptr, expected, readval, val1, val2;
    char *cp;
    struct cookie *cookie_p;

    asm("	.globl __init");
    asm("__init:");

    /*
     * Spin-wait for awhile to allow the harware to come up to speed
     */
    for(i=0; i<0x10; i++)
	;

    /*
     * If this is a cold boot, Check that the control registers
     * have been reset. They should be all 0
     */
    if ( reset_type == RESET_COLD ) {
	if ( KSEG1_IO_ASIC->sys_ctl ) {
	    asm("    li    $4,3");             /* error 3 */
	    asm("    j     lights");
	} else if ( KSEG1_IO_ASIC->shmem_ctl ) {
	    asm("    li    $4,4");             /* error 4 */
	    asm("    j     lights");
	}
    }

    /*
     * Initialize the control register and confirm
     */ 
    asm("        .globl __init_ctl_reg");
    asm("__init_ctl_reg:");
    KSEG1_IO_ASIC->sys_ctl = INITIAL_CONTROL;
    if ( KSEG1_IO_ASIC->sys_ctl != INITIAL_CONTROL) {
	asm("    li    $4,5");            /* error 5 */
	asm("    j     lights");
    }

    /*
     * Initialize the shared memory control register and confirm
     */ 
    asm("        .globl __init_shmem_ctl_reg");
    asm("__init_shmem_ctl_reg:");
    KSEG1_IO_ASIC->shmem_ctl = INITIAL_SHMEM_CONTROL;
    if ( KSEG1_IO_ASIC->shmem_ctl != INITIAL_SHMEM_CONTROL) {
	asm("    li    $4,6");            /* error 6 */
	asm("    j     lights");
    }

    /*
     * Initialize the timer control register and confirm
     */
    KSEG1_IO_ASIC->ccr = 0;
    if ( KSEG1_IO_ASIC->ccr ) {
	asm("    li    $4,7");            /* error 7 */
	asm("    j     lights");
    }

    /*
     * If NVRAM is valid get our virtual config register
     * else use the default
     */
    if(NV_VALID_CONFREG) conf = (NVRAM_SAV)->confreg;
    else conf = DEFAULT_CONFIG;

    /*
     * Initialize the UART
     */
    asm("	.globl __init_uart");
    asm("__init_uart:");
    i = ((conf & CONSOLE_MASK) >> CONSOLE_SHIFT); 
    uartaddr->a.aux2 = 0;	/* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
#ifdef TESTTHISFIRST
    if(((uartaddr->a.mode) & 0xff) != (MODENORMAL | STOP16X16)){
	asm("    li    $4,8");  /* error number 8 */
	asm("    j     lights");
    }
#endif
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;	/* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;	/* Set Carrier detect on Console port */
#ifdef CONINTBRK
    uartaddr->a.aux2 = BREAKXA;	/* Enable interrupts for break */
#endif

    /*
    ** See comment about reset_type at beginning of this routine.
    */
    if ( reset_type != RESET_WDOG ) {
	asm("	.globl __init_version");
	asm("__init_version:");
	PUTSTRING(cp, banner_string);
    } else {

	/*
	 * We got rebooted by the watchdog, so memory is OK
	 * We pass reset_type to this subroutine even though
	 * it is not used there.  This is done to keep the 
	 * compiler from moving reset_type and frame around
	 * in registers. See comment about reset_type at
	 * beginning of this routine.
	 */
	init_soft_reset(reset_type, frame);
	goto exit_init;
    }

    /*
     * Now run a memory test of lower RAM.
     *
     * data = address : Check address aliasing
     * AA's/55's : Checkerboard
     * 55's/AA's : Inverse checkerboard
     * Note, memory range must be divisable by 8
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
	if (conf & DIAG_MASK) {
	    PUTSTRING(cp,"\r\nTesting lower main memory - ");
	    PUTSTRING(cp, mtestvals[i].name);
	}
	asm("	.globl __init_ram_test");
	asm("__init_ram_test:");
	val1 = mtestvals[i].val1;
	val2 = mtestvals[i].val2;
	for( mptr = RAM_TEST_START; mptr < RAM_TEST_END; ) {
	    if(i == 0) {  /* test 0 is "data = address" test */
		*mptr++ = (long)mptr;
	    } else {
		*mptr++ = val1;
		*mptr++ = val2;
	    }
	}
	for( mptr = RAM_TEST_START; mptr < RAM_TEST_END; mptr++) {
	    if( i == 0 )  {
		/* test 0 is "data = address" */
		expected = (long)mptr;
	    } else {
		if((long)mptr & 4)
		    expected = val2;
		else 
		    expected = val1;
	    }
	    readval = *mptr;
	    if( readval != expected ) {
		PUTSTRING(cp,"\r\nBad RAM at location 0x");
		PUTHEXNUM((long)mptr);
		PUTSTRING(cp,": wrote 0x");
		PUTHEXNUM(expected);
		PUTSTRING(cp,", read 0x");
		PUTHEXNUM(readval);
		asm("    li   $4,9");  /* error number 9 */
		asm("    j    lights");
	    }
	}
    }

    if (conf & DIAG_MASK) {
	PUTSTRING(cp,"\r\nClearing bss ");
    }
    asm("	.globl __init_clear_bss");
    asm("__init_clear_bss:");
    val1 = 0;
    for(mptr = (long *)&_fbss; mptr < (long *)&end; )
	*mptr++ = val1;
    reset_type_sav = reset_type;  /* save this info in .bss */

    /*
    ** If this is a cold boot, reset our autoboot
    ** state machine to zero in NVRAM.
    */
    if(reset_type == RESET_COLD) {
	clr_autoboot_state();
    }

    /*
    ** It is OK to use the stack & bss now!
    ** (don't need to use reset_type any more,
    ** use reset_type_sav in .bss)
    ** Get nvmonvars from NVRAM
    */
    if (conf & DIAG_MASK) {
	puts("\nReading monitor variables from NVRAM ");
    }
    nvraminit();
    clrerrlog();
    envflag = INPONCT;

#ifdef DEBUG
    printf("nvram_sav->confreg: %#x, nvram_sav->rconfreg: %#x, "
	   "our confreg: %#x, conf: %#x\n", (NVRAM_SAV)->confreg,
	   (NVRAM_SAV)->rconfreg, (NVRAM)->confreg, conf);
#endif

    if(reset_type_sav == RESET_COLD)
	log_sys_ret(EXC_RESET, (r4k_context_t *)0);

    /*
     * Must initialize R4000, and the Nevada ASIC.
     */
    if (conf & DIAG_MASK) {
	puts("\nEnabling interrupts ");
    }

    sys_state = MONITOR_RUNNING;
    init_exceptions();

    if (conf & DIAG_MASK) {
	puts("\nInitializing TLB ");
    }
    init_tlb();

    if (conf & DIAG_MASK) {
	puts("\nInitializing cache ");
    }
    init_cache();

    if (conf & DIAG_MASK) {
	puts("\nEnabling cache parity errors ");
    }
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);  /* enable cache parity errors */

    if (conf & DIAG_MASK) {
	puts("\nReading cookie ");
    }
    cookie_p = get_cookie();
    ciscopro_flag = (cookie_p->processor & CISCOPRO_FLAG);
    cookie_p->processor &= ~CISCOPRO_FLAG; /* un-mask the real processor type */

    if(cookie_p->processor == PROCESSOR_SIERRA)
	cpu_type = CPU_SIERRA;

    /*
     * Take the NIM's out of reset
     */
    reset_io();
    
    if(conf & DIAG_MASK) printf("\nInitializing main memory ...");
    sizemainmem();
    if(conf & DIAG_MASK) printf("\nInitializing shared memory ...");
    sizeshmem();
    if(conf & DIAG_MASK) printf("\nSizing NVRAM ...");
    if(size_nvram() == (128 * 1024)) {
	KSEG1_IO_ASIC->sys_ctl |= NVRAM_SIZE_128K;
    }

    if (conf & DIAG_MASK) {
	puts("\nExiting init\n");
    }

exit_init:
    /*
     * Return to __init_done
     */
    asm("       .globl __init_done");
    asm(".set noreorder");
    asm("	j     __init_done");
    asm("       nop");
    asm("       nop");
    asm(".set reorder");
}

/*
 * init_soft_reset : Set up context after a watch dog soft reset
 * Note that reset type is passed to this routine even though it
 * is not used here.  See relevant comments in the routine init
 * (above).
 */
void
init_soft_reset(int reset_type, unsigned long *frame)
    /* do not remove reset_type parameter even though it's not used */
{
    r4k_context_t *ptr = (r4k_context_t *)frame;

    printf("\n*** Watch Dog Timeout ***\n");
    printf("PC = 0x%.8x, SP = 0x%.8x\n", ptr->err_epc, ptr->regs[SP_REGNUM]);
    init_exceptions();
    init_tlb();
    init_cache();
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);  /* enable cache parity errors */
    copy_context(frame, 1);
    /*
    ** Log the info for future use
    */
    log_sys_ret(EXC_WATCHDOG, frame);
    sys_state = MONITOR_RUNNING;
}

/*
 * Reset all IO devices (NIMs) to a quiescent state:
 * The final delay is to allow shared memory to stabilize and
 * is mandatory.
 */
void
reset_io(void)
{
    int counter;  /* delay counter */

    KSEG1_IO_ASIC->shmem_ctl &= ~SHMEM_SLOT_RESET;
    flush_io_wb();
    for ( counter=0 ; counter < 1000 ; counter ++) /* delay */
	;
    KSEG1_IO_ASIC->shmem_ctl |= SHMEM_SLOT_RESET;
    flush_io_wb();
    for ( counter=0 ; counter < 1000 ; counter ++) /* delay */
	;
}

/*
 * flush_io_wb:
 * Flush the IO write buffer
 */
int
flush_io_wb(void)
{
    return(KSEG1_IO_ASIC->asic_reg);
}

/*
 * flush_dram_wb:
 * Flush the DRAM write buffer
 */
int
flush_dram_wb(void)
{
    return( *(int *)(KSEG1_ADRSPC_RAM) );
}

/*
 * flush_shmem_wb:
 * Flush shared memory write buffer
 */
int
flush_shmem_wb(void)
{
    return( *(int *)(KSEG1_ADRSPC_SHAREDMEM) );
}

/*
 * flush_all_wb:
 * Flush all write buffers
 */
void
flush_all_wb(void)
{
    flush_io_wb();
    flush_shmem_wb();
    flush_dram_wb();
}

/*
** Stubs for NVRAM write protection on some platforms
*/
void
wr_ena_nvram()
{
    return;
}

void
wr_prot_nvram()
{
    return;
}
/* End of Module */

