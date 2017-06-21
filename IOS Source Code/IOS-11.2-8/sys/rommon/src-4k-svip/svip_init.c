/* $Id: svip_init.c,v 1.1.6.1 1996/07/08 22:43:07 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_init.c,v $
 *------------------------------------------------------------------
 * svip_init.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_init.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:07  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:01  pgreenfi
 * Placeholder files for src-4k-svip.
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
#include "cpu.h"
#include "ralib.h"
#include "diagcode.h"


#define EXC_SW_NMI      13      /* S/W NMI (RSP) */
#define EXC_UNKNOWN_RESET 99    /* Unknown Reset */

struct nvram *nvram_ptr = ((struct nvram *)(KSEG1_ADRSPC_NVRAM));
char *errlog_start = ((char *)(MON_FREE_MEM));
int cpu_type;
char *platform_string = "SVIP";
char *sim_banner_string = "Svip";
char *default_monlib_string = "%svip-fslib-m";
unsigned char reason_reg;


/* forward declarations */
void init_soft_reset(int reset_type, unsigned long *frame);
void reset_io(void);
void init_body(int reset_type, unsigned long *frame);
void save_reset_reason();
unsigned char get_reset_reason();
void stop_irsp_ips();

struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};

extern unsigned long monitor_stk;

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
    int i, conf;
    long *mptr, expected, readval, val1, val2;
    unsigned long sreg = getcp0_sreg();

    /* 
     * ENTER initialization stage 1.
     */
    

    /*
     * Use default configuration.
     */
    conf = DEFAULT_CONFIG;

    /*
    ** See comment about reset_type at beginning of this routine.
    */
    if ( reset_type != RESET_COLD && reset_type != RESET_WARM ) {
	
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
     * Enter initialization stage 2.
     */
    /*
     * Now run a memory test of lower RAM.
     *
     * data = address : Check address aliasing
     * AA's/55's : Checkerboard
     * 55's/AA's : Inverse checkerboard
     * Note, memory range must be divisable by 8
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
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
		KSEG1_VIP_VFPGA->led_reg = (long)mptr;
		KSEG1_VIP_VFPGA->led_reg = expected;
		KSEG1_VIP_VFPGA->led_reg = readval;
		asm("    li   $4,7");  /* error number 7 */
		asm("    j    lights");
	    }
	}
    }


    /* 
     * Clear the lower 16K for cache initialization. (stage 3)
     */

    val1 = 0;
    for (mptr = (long *)KSEG1_ADRSPC_RAM; mptr < (long *)(KSEG1_ADRSPC_RAM + CACHE_SIZE);)
	*mptr++ = val1;

    diagcode(CODE_LOWERRAM, 0);

    /*
     * Clear the BSS segment
     */
    KSEG1_VIP_VFPGA->led_reg = 4;
    asm("	.globl __init_clear_bss");
    asm("__init_clear_bss:");
    val1 = 0;
    for(mptr = (long *)&_fbss; mptr < (long *)&end; )
	*mptr++ = val1;

    diagcode(CODE_CLEARBSS, 0);
    reset_type_sav = reset_type;  /* save this info in .bss */

    /*
    ** It is OK to use the stack & bss now!
    ** (don't need to use reset_type any more,
    ** use reset_type_sav in .bss)
    ** Get nvmonvars from NVRAM
    */

    diagcode(CODE_NVRAMINIT, 0);

    envflag = INPONCT;

    /*
     * Must initialize R4000, and the Nevada Equivalent Interrupt
     */
    cpu_type = CPU_VIP;
    sys_state = MONITOR_RUNNING;
    init_exceptions();
    diagcode(CODE_INITEXCEPTION, 0);

    init_tlb();
    diagcode(CODE_INITTLB, 0);
    init_cache();
    diagcode(CODE_INITCACHE, 0);
    svip_setup_PMA_registers();
    diagcode(CODE_SETUPPMA, 0);
    val1 = sizesram();
    diagcode(CODE_SIZESRAM, val1);

    val1 = sizemainmem();
    diagcode(CODE_CLEARDRAM, val1);

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
    int reason;


    if (reset_type == RESET_NMI) {
	reason = EXC_SW_NMI;
    } else {
        reason = EXC_UNKNOWN_RESET;
    }

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

#if 0
/*
 * Reset all I/O subsystem to a quiescent state.
 * Delay is mandatory in order to stablize the I/O subsystem.
 */
void
reset_io(void)
{

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
#endif

#if 0
/*
 * Name: save_reset_reason()
 *   Save the reset reason register into a global variable 
 */
void
save_reset_reason()
{
    reason_reg = KSEG1_VIP_VFPGA->reset_reason_reg;
}
#endif

/*
 * Name: get_reset_reason()
 *   Return the value of reset reason register
 */
unsigned char
get_reset_reason()
{
    return(reason_reg);
}

#if 0

/*
 * Name: stop_irsp_ips
 *  Stop all the IRSP IPs
 */
void
stop_irsp_ips()
{

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
#endif
/* End of Module */
