/* $Id: rsp2_init.c,v 3.5.4.1 1996/03/21 23:32:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp2/rsp2_init.c,v $
 *------------------------------------------------------------------
 * rsp2_init.c
 *
 * 05/95, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp2_init.c,v $
 * Revision 3.5.4.1  1996/03/21  23:32:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:44  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:59:41  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:35  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.5  1996/03/11  04:54:02  tkam
 * CSCdi36205:  RSP1 Mitsubishi NVRAM failures
 * Added magic number matching for more robust nvram validity checking.
 *
 * Revision 3.4  1996/02/01  02:27:35  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 * Revision 3.3  1996/01/17  23:32:11  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:47:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:41:47  hampton
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
#include "dclasic.h"
#include "memdasic.h"
#include "dgbus.h"
#include "ralib.h"

#include "../../../boot/cpu.h"

#define EXC_SW_NMI      13      /* S/W NMI (RSP) */
#define EXC_UNKNOWN_RESET 99    /* Unknown Reset */

struct nvram *nvram_ptr = ((struct nvram *)(KSEG1_ADRSPC_NVRAM));
char *errlog_start = ((char *)(MON_FREE_MEM));
int cpu_type;
char *platform_string = "RSP2";
char *default_monlib_string = "%rsp-fslib-m";
unsigned char reason_reg;

/* forward declarations */
void init_soft_reset(int reset_type, unsigned long *frame);
void reset_io(void);
void init_body(int reset_type, unsigned long *frame);
void save_reset_reason();
unsigned char get_reset_reason();
void stop_irsp_ips();
void unmask_ms_int();

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
#define CACHE_SIZE               0x4000

static volatile int reset_type_sav;

#define MAX_SLOTS                 32
#define SLOT_EMPTY                -1
#define FCI_RSP1_CONTROLLER       14
#define COMPUTE_DBUS_ADDRESS(slot)  (struct dgtype_ *)((slot * 0x40) + DGBUSSTART)

int slots[MAX_SLOTS];            /* contain IP array */

ralib_vector_table_t *ralib_vector_table_ptr;
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
    int i, conf, j, k;
    long *mptr, expected, readval, val1, val2;
    char *cp;
    struct cookie *cookie_p;
    int counter;    /* delay counter */
    unsigned char *tval = (unsigned char *)(KSEG1_ADRSPC_NVRAM + 0x6000);
    unsigned long diagvect;
    volatile unsigned long *diagvect_ptr;
    int ms_status;

     /*
      * Wait here for 10us to  allow the hardware to come up to speed
      *
      */
    delay_10us(1);

    if (reset_type == RESET_COLD) {
	NVRAM_SAV->this_current_state = STATE_NON_PART;
	NVRAM_SAV->other_current_state = STATE_NON_PART;
    }

    /* set RSP READY bit ot NOT READY for slave */
    REG_IRSP2DBUS_FPGA->StatusReset = 0;

    /*
     * Show the LEDs 
     */
    REG_FPGA->led_reg &= 0;
    delay_10us(1000);
    REG_FPGA->led_reg |= 0xff;

    /*
     * If this is a cold boot, Check that the system control register
     * have been reset. They should be all 1.
     * The reset reason register is read. All bits will become 1.
     */
    if ( reset_type == RESET_COLD ) {
	if ( REG_FPGA->sys_ctrl_reg & CYBUS_RECV_PRESENT_MASK != CYBUS_RECV_PRESENT_MASK ) {
	    asm("    li    $4,3");             /* error 3 */
	    asm("    j     lights");
	}
	if ( REG_FPGA->reset_reason != 0) {
	    asm("    li    $4,4");             /* error 4 */
	    asm("    j     lights"); 
	}
    }

    /*
     * Initialize the timer control register and confirm
     * Disable watchdog timer and couter 1 and 2.
     */
    DCL_ASIC->ccr = 0;
    if ( DCL_ASIC->ccr ) {
	asm("    li    $4,5");            /* error 5 */
	asm("    j     lights");
    }


    /*
     * If NVRAM is valid get our virtual config register
     * else use the default
     */
    if(NV_VALID_CONFREG && (NVRAM_SAV->magic == NVMAGIC)) conf = (NVRAM_SAV)->confreg;
    else {
	conf = DEFAULT_CONFIG;
	NV_WR_CONFREG(conf);
    }

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
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;	/* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;	/* Set Carrier detect on Console port */
#ifdef CONINTBRK
    uartaddr->a.aux2 = BREAKXA;	/* Enable interrupts for break */
#endif

    save_reset_reason();

    /*
    ** See comment about reset_type at beginning of this routine.
    */
    if ( reset_type == RESET_COLD || reset_type == RESET_WARM ) {
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
		asm("    li   $4,7");  /* error number 7 */
		asm("    j    lights");
	    }
	}
    }

    /* 
     * Clear the lower 16K for cache initialization.
     */
    if (conf & DIAG_MASK) {
	PUTSTRING(cp, "\r\nClearing lower 16K memory for cache initialization");
    }
    val1 = 0;
    for (mptr = (long *)KSEG1_ADRSPC_RAM; mptr < (long *)(KSEG1_ADRSPC_RAM + CACHE_SIZE);)
	*mptr++ = val1;


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

    if (conf & DIAG_MASK) {
	printf("Master/Slave arbitration\n");
    }
    ms_status = ms_arbitration();

#ifdef DEBUG
    printf("return status = %d\n", ms_status);
#endif

    nvraminit();

    envflag = INPONCT;

#ifdef DEBUG
    printf("nvram_sav->confreg: %#x, nvram_sav->rconfreg: %#x, "
	   "our confreg: %#x, conf: %#x\n", (NVRAM_SAV)->confreg,
	   (NVRAM_SAV)->rconfreg, (NVRAM)->confreg, conf);
#endif
/*  Commented out to perserve the crash info during hard reset.
 *  This is for the HSA as the slave will be hard reset by the master
 */
#if 0
    if(reset_type_sav == RESET_COLD)
	log_sys_ret(EXC_RESET, (r4k_context_t *)0);
#endif
    stop_irsp_ips();


    /*
     * Must initialize R4000, and the DCL ASIC.
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


    cpu_type = CPU_RSP;

    if(conf & DIAG_MASK) printf("\nInitializing main memory ...\n");
    sizemainmem();

    if(conf & DIAG_MASK) printf("\nInitializing shared memory ...\n");    
    sizeshmem();

    REG_FPGA->sys_ctrl_reg |= SYSCTRL_ENA_CBUS_DRIVERS; /* disabled CBUS driver */
    if(conf & DIAG_MASK) printf("\nDisabled CBUS driver ...\n");

#ifdef DEBUG
    printf("system control reg = %b\n", REG_FPGA->sys_ctrl_reg); 
#endif

    if(conf & DIAG_MASK) printf("\nSizing NVRAM ...\n");
    size_nvram();

    if (conf & DIAG_MASK) printf("\nEnable dBus driver\n");
    REG_FPGA->sys_ctrl_reg &= ((~SYSCTRL_ENA_DBUS_DRIVERS) & 0xFF);

    if (conf & DIAG_MASK) printf("\nInitializing PCMCIA controller\n");
    ralib_vector_table_ptr = ralib_init();

    if (conf & DIAG_MASK) {
	puts("\nExiting init\n");
    }

    unmask_ms_int();

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
    unsigned char reset_reason;
    int reason;

    stop_irsp_ips(); 

    reset_reason = ~get_reset_reason();

    if (reset_reason & SW_NMI) {
	printf("\n*** Software NMI ***\n");
	reason = EXC_SW_NMI;
    } else if (reset_reason & PUSH_BUTTON_NMI) {
	printf("\n*** PUSH BUTTON NMI ***\n");
	reason = EXC_RESET;
    } else if (reset_reason & SW_WATCHDOG_RST) {
	printf("\n*** Watch Dog Timeout ***\n");
	reason = EXC_WATCHDOG;
    } else if (reset_type == RESET_WARM) {
	printf("\n*** Reset or Reload Command ***\n");
	reason = EXC_RESET;
    } else {
	printf("\n*** Unknown Soft Reset ***\n");
	reason = EXC_UNKNOWN_RESET;
    }

    if (frame)
	printf("PC = 0x%.8x, SP = 0x%.8x\n", ptr->err_epc, ptr->regs[SP_REGNUM]);
    init_exceptions();
    init_tlb();
    init_cache();
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);  /* enable cache parity errors */
    if (frame) {
	copy_context(frame, 1);
	/*
	 ** Log the info for future use
	 */
	log_sys_ret(reason, frame);
    }
    sys_state = MONITOR_RUNNING;

    
}

/*
 * Reset all I/O subsystem to a quiescent state.
 * Delay is mandatory in order to stablize the I/O subsystem.
 */
void
reset_io(void)
{
    int counter;  /* delay counter */

    REG_FPGA->sw_perif_rst = 0;  /* reset dBus, PCMCIA, I/O FPGA, QA and MEMD */
    delay_10us(1);               /* at least wait for 580 ns */
    REG_FPGA->sw_perif_rst |= ~DBus_Rst;   /* Dbus reset bit = 1 */

}

/*
 * flush_io_wb:
 * Flush the IO write buffer
 */
int
flush_io_wb(void)
{
    return (*(int *)(KSEG1_ADRSPC_PROM + RESET_VEC_OFFSET));    /* read from I/O bus */
}

/*
 * flush_dram_wb:
 * Flush the DRAM write buffer
 */
int
flush_dram_wb(void)
{
}

/*
 * flush_shmem_wb:
 * Flush shared memory write buffer
 */
int
flush_shmem_wb(void)
{
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
 * Name: save_reset_reason()
 *   Save the reset reason register into a global variable 
 */
void
save_reset_reason()
{
    reason_reg = REG_FPGA->reset_reason;
}

/*
 * Name: get_reset_reason()
 *   Return the value of reset reason register
 */
unsigned char
get_reset_reason()
{
    return(reason_reg);
}



/*
 * Name: stop_irsp_ips
 *  Stop all the IRSP IPs
 */
void
stop_irsp_ips()
{
    int slot_number;
    struct dgtype_ *dgbus;
    int ix;

    /* 
     * disable the CBUS 
     */
    REG_FPGA->cbus0 &= ~CBUS_0_GRANT_ENABLE;
    while (REG_FPGA->cbus0 & CBUS_0_GRANT_ENABLE);   /* wait for the bit change */

    delay_10us(1);                                   /* wait for dbus to be disabled */

    REG_FPGA->cbus1 &= ~CBUS_1_GRANT_ENABLE;
    while (REG_FPGA->cbus1 & CBUS_1_GRANT_ENABLE);

    delay_10us(1);

    /*
     * disable the network interrupt and 
     * command interrupt in MEMD ASIC
     */
    MEMD_ASIC->QA_network_reg |= QA_NETWORK_MASK;
    MEMD_ASIC->QA_cmd_reg |= QA_CMD_MASK;
    REG_IRSP2DBUS_FPGA->DBusFPGAIntMask |= IRSP2DBUS_FPGA_INT_MCASIC_MASK;

} /* stop_irsp_ips */


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






