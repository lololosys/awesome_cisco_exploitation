/* $Id: pred_mon_exceptn.c,v 3.2.58.2 1996/04/04 17:05:58 szhang Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_mon_exceptn.c,v $
 *------------------------------------------------------------------
 * pred_mon_exceptn.c -- Predator Monitor exception handler
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_mon_exceptn.c,v $
 * Revision 3.2.58.2  1996/04/04  17:05:58  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:31:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/02/28  01:00:39  szhang
 * CSCdi50105:  C7100 ROMMON code cleanup
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/24  02:32:49  mbeesley
 * CSCdi49533:  Error interrupt handler gets incorrect frame pointer.
 * Do more sanity checking on the frame pointer being passed back from
 * IOS before using it and crashing in the rom monitor.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/02/16  21:41:48  szhang
 * CSCdi49173:  Separate the startup and exception code
 * Branch: ELC_branch
 * Separate the startup and exception code into platform
 * dependent and platform independent files.
 *
 * Revision 3.2.2.5  1996/02/13  00:43:13  mbeesley
 * CSCdi48770:  Make reset_io() work, add support for fault history
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/01/20  01:22:55  szhang
 * Branch: ELC_branch
 * Make the parity detection code work on the Predator platform.
 *
 * Revision 3.2.2.3  1995/12/08  05:00:04  mbeesley
 * Branch: ELC_branch
 * Setup first 5 TLB entiries, init PCI, PCMCIA and ralib. Clean up
 * exception/interrupt handling, fix NVRAM handling.
 *
 * Revision 3.2.2.2  1995/12/01  04:23:53  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2.2.1  1995/11/23  19:14:43  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.2  1995/11/17  18:46:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Handle all of the exceptions.
 */

#include "monitor.h"
#include "console.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "bitdef.h"
#include "dis_mips.h"
#include "error.h"
#include "mon_boot.h"
#include "../../src-4k/gt64010.h"
#include "extern.h"

extern gt64010_regs_t *gt_ptr;
extern long timerint(r4k_context_t *frame);
extern SIG_PF sigbusfcn;
extern SIG_PF sigdiagfcn;


/*
 * Variables defined in this module
 */
volatile unsigned short cpu_flags;
r4k_context_t *process_context_ptr;
r4k_context_t *kernel_context_ptr;
r4k_context_t kernel_context_body;
unsigned short internal_test;

/*
 * Forward declarations
 */
void chk_abort(r4k_context_t *frame, int offset);
static void abort_task(void);
static void handle_interrupt(r4k_context_t *frame, char *str);
extern void handle_tlbmod(r4k_context_t *frame, char *str);
extern void handle_tlbl(r4k_context_t *frame, char *str);
extern void handle_tlbs(r4k_context_t *frame, char *str);
extern void handle_adel(r4k_context_t *frame, char *str);
extern void handle_ades(r4k_context_t *frame, char *str);
extern void handle_ibe(r4k_context_t *frame, char *str);
extern void handle_dbe(r4k_context_t *frame, char *str);
extern void handle_syscall(r4k_context_t *frame, char *str);
extern void handle_bp(r4k_context_t *frame, char *str);
extern void handle_ri(r4k_context_t *frame, char *str);

static void gt64010_int(r4k_context_t *frame);
static void system_error_int(r4k_context_t *frame);

extern long *evece;

/*
 * List of Cause strings
 */
r4k_exception_t cause_table[] = {
    { "*** CPU Interrupt ***", handle_interrupt },
    { "*** TLB Modification Exception ***", handle_tlbmod },
    { "*** TLB (Load/Fetch) Exception ***", handle_tlbl },
    { "*** TLB (Store) Exception ***", handle_tlbs },
    { "*** Address Error (Load/Fetch) Exception ***", handle_adel },
    { "*** Address Error (Store) Exception ***", handle_ades },
    { "*** Bus Error (Fetch) Exception ***", handle_ibe },
    { "*** Bus Error (Load) Exception ***", handle_dbe },
    { "*** System Call Exception ***", handle_syscall },
    { "*** Break Point Exception ***", handle_bp },
    { "*** Illegal Opcode Exception ***", handle_ri },
    { "*** Coprocessor Unusable Exception ***", 0 },
    { "*** Arithmetic Overflow Exception ***", 0 },
    { "*** Trap Exception ***", 0 },
    { "*** Virtual Coherency (Opcode) Exception ***", 0 },
    { "*** Floating Point Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Watch Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Reserved General Exception ***", 0 },
    { "*** Virtual Coherency (Data) Exception ***", 0 }
};

/* 
 * Constant definition
 */
#define EXC_STATUS_SHIFT                4
#define EXC_NUM                         4
#define RST_WAIT_TIME                   0x2000


/*
 * init_exceptions :
 * Initialize exceptions sub-system. When the system runs with BEV de-asserted
 * the exception handling code must be moved to KSeg0 (IE cachable).
 */
void
init_exceptions(void)
{
    unsigned long sreg = getcp0_sreg();
    int ix;
    unsigned char temp;      /* temporary variable */

    /*
     * clear all the pending interrupts
     */
    gt_ptr->pci_int_cause = 0;

    /*
     * Use the CPU count compare mechanism to check the break key
     */
    setcp0_count(0);  /* initialize */
    setcp0_compare(MON_TIMER_VAL_20MS);  

    /*
     * Enable interrupts:
     * 		IP7 - Count/Compare
     * 		IP6 - External Errors
     *		IP4 - Timer/DMA interrupt from GT64010
     */
    sreg |= (SREG_IMASK4|SREG_IMASK6|SREG_IMASK7|SREG_IE);

    setcp0_sreg(sreg);

    gt_ptr->pci_cpu_mask = GT64010_CPU_INTR_MASK;
    gt_ptr->pci_pci_mask = GT64010_ERR_INTR_MASK;
    gt_ptr->pci_serr_mask |= 
	(SERR_ADDRESS_PAR_MASK | SERR_MASTER_WRITE_PAR_MASK |
	 SERR_MASTER_READ_PAR_MASK | SERR_DRAM_ERR_MASK |
	 SERR_MASTER_ABORT_MASK | SERR_TARGET_ABORT_MASK );
    KSEG1_C7100_MP_FPGA->err_int_stat_lsw |= 
	(C7100_GT64010_ERR_MASK |
	 C7100_PKT_SRAM_ERR_MASK |
	 C7100_OIR_EVENT_MASK);
}


/*
 * count_int : (If IP7 is NOT configured as external at boot time)
 * Count == Compare interrupt, check if a test, if not print some
 * information and deassert (write Compare register). 
 */
static int timer;

static void
count_int(r4k_context_t *frame)
{
    long compval;
    long rpc;

    if (internal_test) {
	cpu_flags = SREG_IMASK7;
	compval = 0;
    } else {
	compval = MON_TIMER_VAL_20MS;
	rpc = isconbrk();  /* poll the UART for a break condition */
	if (rpc) {  /* rig the stack with the desired return pc */
	    copy_context(frame, 0);
	    frame->epc = rpc;
	    frame->epc_hi = KSEG_MSB_ADDR;
	}
    }
    setcp0_count(0);  		/* clear count */
    setcp0_compare(compval);  	/* this clears the interrupt */
}

/*
 * gt64010_int:
 * Handle GT64010 interrupts coming in on IRQ4 (pci_cpu_mask)
 */
static void
gt64010_int(r4k_context_t *frame)
{
    ulong cause, real_cause, mask;
    
    cause = gt_ptr->pci_int_cause;
    mask = gt_ptr->pci_cpu_mask;
    real_cause = cause & mask;

    /*
     * Clear the interrupt
     */
    gt_ptr->pci_int_cause = ~(real_cause);

    printf("*** GT64010 Interrupt on IBIT4 ***\n");
    printf("mask=0x%x, cause=0x%x, real_cause=0x%x\n",
	   mask, cause, real_cause);
    excpt_info(frame);
    chk_abort(frame, 0);   /* use longjmp to return to monitor prompt */
}

/*
 * gt64010_fatal_errors:
 * Handle a GT64010 error interrupt coming in on IBIT6
 */
void gt64010_fatal_errors (void)
{
    ulong reg_cause, cause, mask;
    
    reg_cause = gt_ptr->pci_int_cause;
    mask = gt_ptr->pci_pci_mask;
    cause = reg_cause & mask;

    /*
     * Clear the interrupt
     */
    gt_ptr->pci_int_cause = ~(cause);
    printf(" mask=0x%x, cause=0x%x, real_cause=0x%x\n",
	   mask, reg_cause, cause);

    /*
     * print out detailed interrupt cause 
     */
    if (cause & INT_DRAM_OUT_OF_RANGE) {
	printf(" Memory out of range error\n");
    }
    if (cause & INT_DMA_OUT_OF_RANGE) {
	printf(" DMA out of range error\n");
    }
    if (cause & INT_CPU_OUT_OF_RANGE) {
	printf(" CPU out of range error\n");
    }
    if (cause & INT_MASTER_READ_PAR) {
	printf(" Parity error on master read\n");
    }
    if (cause & INT_SLAVE_WRITE_PAR) {
	printf(" Parity error on slave write\n");
    }
    if (cause & INT_MASTER_WRITE_PAR) {
	printf(" Parity error on master write\n");
    }
    if (cause & INT_SLAVE_READ_PAR) {
	printf(" Parity error on slave read\n");
    }
    if (cause & INT_ADDRESS_PARITY) {
	printf(" Address parity error\n");
    }
    if (cause & INT_DRAM_ERROR) {
	printf(" Memory parity error (external)\n");
    }
    if (cause & INT_MASTER_ABORT) {
	printf(" PCI Master abort\n");
    }
    if (cause & INT_TARGET_ABORT) {
	printf(" PCI Target abort\n");
    }
    if (cause & INT_RETRY_CNT_EXP) {
	printf(" PCI retry counter expired\n");
    }

    /*
     * Printout some interesting registers
     */
    printf(" bus_err_high=0x%x, bus_err_low=0x%x, addr_decode_err=0x%x\n",
	   gt_ptr->bus_err_high, gt_ptr->bus_err_low,
	   gt_ptr->addr_decode_err);
}

static void
system_error_int(r4k_context_t *frame)
{
    unsigned long err_int_stat_lsw;

    err_int_stat_lsw = KSEG1_C7100_MP_FPGA->err_int_stat_lsw;
    printf("*** System Error Interrupt (IBIT6) ***\n");
    printf("int_stat register = 0x%x\n", err_int_stat_lsw);

    if (err_int_stat_lsw & C7100_GT64010_ERR_MASK) {
	printf("GT64010 Error Interrupt, Cause(s):\n");
	gt64010_fatal_errors();
    }
    if (err_int_stat_lsw & C7100_PKT_SRAM_ERR_MASK) {
	printf("Packet memory addressing error\n");
    }
    if (err_int_stat_lsw & C7100_OIR_EVENT_MASK) {
	printf("OIR event on swappable port adaptor(s)\n");
	KSEG1_C7100_MP_FPGA->err_int_stat_lsw |= C7100_OIR_EVENT_MASK;
    }
    excpt_info(frame);
    chk_abort(frame, 0);    /* use longjmp to return to monitor prompt */
}

/*
 * handle_interrupt :
 * Handle system interrupts
 */
static void
handle_interrupt(r4k_context_t *frame, char *str)
{
    long sreg, cause, intrpt;

    sreg = frame->sreg & CAUSE_IMASK;
    cause = frame->cause & CAUSE_IMASK;

    intrpt = sreg & cause;
    if ( !intrpt ) {

unknown_interrupt:

	printf("\n*** Unknown External Interrupt ***\n");
	printf("Stacked Cause = 0x%x, Stacked Status Reg = 0x%x\n",
	       frame->cause, frame->sreg);
	printf("Current Cause IP[7..0] = 0x%x, Current SREG IP[7..0] = 0x%x\n",
	       cause>>CAUSE_ISHIFT, sreg>>CAUSE_ISHIFT);
	return;
    }

    /*
     * If IP0 is asserted, call sw int 0
     */
    if (intrpt & CAUSE_IBIT0) {
	sw_int_zero(frame);
	return;
    } else if (intrpt & CAUSE_IBIT1) {
	sw_int_one(frame);
	return;
    } else if (intrpt & CAUSE_IBIT7) {
	count_int(frame);
	return;
    } else if (intrpt & CAUSE_IBIT4) {
	gt64010_int(frame);
	return;
    } else if (intrpt & CAUSE_IBIT6) {
	system_error_int(frame);
	return;
    }

    /*
     * One of our un-connnected interrupts caused
     * us to come here.
     */
    goto unknown_interrupt;
}


/*
 * abort_task : Longjump to monjmpptr with a return of 2
 * This routine will re-enable global interrupts, and de-assert interrupt
 * level and error level. This is needed for when an exception within an
 * exception occurs resulting in a longjump to the monitor. In this
 * scenario, the interrupt mask and exception/error level would
 * have remained asserted.
 */
static void
abort_task(void)
{
    setcp0_sreg( (getcp0_sreg() & ~(SREG_ERL|SREG_EXL)) | SREG_IE );
    sys_state = MONITOR_RUNNING;
    cache_flush(0, -1);
    if(monjmpptr) longjmp(monjmpptr,2);
}

/*
 * chk_abort :
 * If we are in the monitor and the jmp_buf pointer is initialized
 * or STOPONERR is set, set post interrupt callback to abort_task.
 */
void
chk_abort(r4k_context_t *frame, int offset)
{
    int err_reg = ( frame->sreg & SREG_ERL ) ? 1 : 0;

    if (monjmpptr && (envflag == INMON || stoponerr())) {
	copy_context(frame, err_reg);
	if (err_reg) {
	    frame->err_epc = (unsigned long)abort_task;
	    frame->err_epc_hi = KSEG_MSB_ADDR;
	} else {
	    frame->epc = (unsigned long)abort_task;
	    frame->epc_hi = KSEG_MSB_ADDR;
	}
    } else {
	printf("ROM Monitor Can Not Recover From Exception\n");
	printf("A Board Reset Is Issued\n");
	KSEG1_PRED_FPGA->sw_nmi_reg = 0;            /* issue a NMI reset */
    }

}


/*
 * system_returned:
 * Handle a system image returning through the cpu exception emt 
 * call. Check if the system was really running, initialize
 * some context pointers, and return to the monitor.
 */
void
system_returned(r4k_context_t *frame, int sig, int code, r4k_context_t *proc)
{
    unsigned long sreg;
    int errcode;
    char *errptr;
    jmp_buf *monjmpptrsav;

    /*
     * Do not allow user to <break> out of this routine.
     * Save monjmpptr and set to NULL.
     */
    monjmpptrsav = monjmpptr;
    monjmpptr = (jmp_buf *)0;

    /*
     * Check if the system had been dispatched
     */
    if (sys_state != SYSTEM_RUNNING) {
	/* should never happen */
	printf("\n*** system_returned called but no system launched ***\n");
    } else {

	/*
	 * Reinitialize extrnal interrupt mask and enable
	 * the UART, print out some information about the crash.
	 * We will only take parity and Write Berr interrupts
	 */
	KSEG1_C7100_MP_FPGA->err_int_enbl_lsw = 0;
	uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;

	/*
	 * Errptr must be set in each case of the following switch statement.
	 * This must be backwards compatible with
	 * the error reporting mechanism in the old 68k monitor.
	 */
	errcode = 0;  /* init */
	switch (sig) {

	/*
	 * Break key
	 */
	case SIGQUIT:
	    errcode = EXC_ABORT;
	    errptr = "an abort due to Break Key";
	    hkeepflags |= H_USRINT;
	    break;

	/*
	 * Illegal instruction
	 */
	case SIGILL:
	    errptr = "an Illegal Opcode exception";
	    break;

	/*
	 * Unexpected interrupt
	 */
	case SIGINT:
	    errcode = EXC_UNEXP_INT;
	    errptr = "an unexpected h/w interrupt";
	    break;

	/*
	 * Segmentation violation
	 */
	case SIGSEGV:
	    errptr = "a SegV exception";
	    break;

	/*
	 * Bus error
	 */
	case SIGBUS:
	    errcode = EXC_BUSERR;
	    errptr = "a Bus Error exception";
	    break;

	/*
	 * Spurious reserved exceptions
	 */
	case SIGKILL:
	    errptr = "a reserved exception";
	    break;

	/*
	 * Arithmetic errors, Break point
	 */
	case SIGTRAP:
	    if (((code & CAUSE_MASK) >> CAUSE_SHIFT) == CAUSE_BP) {
		errcode = EXC_BREAK;
		errptr = "an abort due to Break Point";
	    } else {
		errptr = "an arithmetic exception";
	    }
	    break;

	/*
	 * Floating Point exception
	 */
	case SIGFPE:
	    errptr = "an FPU exception";
	    break;

	/*
	 * Cache parity exception
	 */
	case SIGCACHE:
	    errcode = EXC_PARITY;
	    errptr = "a Cache Parity Exception";
	    break;

	/*
	 * Write Bus Error exception
	 */
	case SIGWBERR:
	    errptr = "a Write Bus Error Interrupt";
	    break;

	/*
         * Error interrupt
         */
	case SIGERROR:
	    errptr = "an Error Interrupt";
	    break;

	/*
	 * Software forced crash
	 */
	case SIGRELOAD:
	    errptr = "a Software forced crash";
	    break;

	default:
	    errptr = "an unknown failure";
	    break;
	}

	printf("\n*** System received %s ***\n", errptr);

	/* kludgy, but backwards compatible with old 68k monitor */
	if(!errcode) errcode = (int)errptr;

	printf("signal= %#x, code= %#x, context= %#x\n", sig, code, proc);
	excpt_info(frame);

	/*
	 * If a system process context exists, save a pointer to it
	 */
	process_context_ptr = proc;

	/*
	 * save the context that was built returning to the monitor
	 */
	copy_context(frame, 0);

	/*
	 * Log the info for future use
	 */
	log_sys_ret(errcode, frame);
    }
    
    /*
     * Set the state to back in the monitor
     */
    sys_state = MONITOR_RUNNING;

    /*
     * Re-enable interrupts, IP7 (count/compare), and return to the monitor.
     * Do it in stages, to stay away from the weirdness in the chip
     * when changing these states.
     */
    sreg = getcp0_sreg();
    sreg &= ~(SREG_EXL|SREG_ERL|CAUSE_IMASK);
    setcp0_count(0);			           /* re-init count         */
    setcp0_compare(MON_TIMER_VAL_20MS);		   /* re-init compare */
    setcp0_sreg(sreg);				   /* de-assert EXl,ERL,IP  */
    sreg |= (CAUSE_IBIT7|SREG_IE|SREG_BEV);	   /* enable c/c, boot vect.*/
    setcp0_sreg(sreg);
    longjmp(monjmpptrsav, 3);			   /* quiet return */
}

/*
 * log_sys_ret : log the system return info for future use
 */
int
log_sys_ret(int reason, r4k_context_t *frame)
{
    union instr_r4k *op;
    int dest_reg;
    int numframes = 0;

    /*
     * Only log errors from healthy target images or reset
     */
    if (reason != EXC_RESET && !boot_satisfied) return;

    /*
     * If a system process context exists, use it
     */
    if (process_context_ptr) frame = process_context_ptr;

    /* init entire structure with zeroes */
    bzero(&(NVRAM)->sys_ret_info[BOOT_STACK_TRACE],
	  sizeof((NVRAM)->sys_ret_info));

    /*
     * Make sure we are dealing with a good frame pointer.
     */
    if (frame &&			/* must be non-zero */
	(!illegal_addr(frame))) {	/* must be a legal address */

	/*
	 * get our epc right (cache parity errors use err_epc)
	 */
	if ( frame->sreg & SREG_ERL ) frame->epc = frame->err_epc;

	numframes = stack_trace((unsigned long *)frame->epc,
				(unsigned long *)frame->regs[SP_REGNUM],
				frame->regs[RA_REGNUM], BOOT_STACK_FRAMES,
				&(NVRAM)->sys_ret_info[BOOT_STACK_TRACE]);

	/* Clear gt64010 address out-of-range int as stack_trace might have
	   touched invalid addresses */
	GT64010_REGS->pci_int_cause &= ~INT_CPU_OUT_OF_RANGE;

	(NVRAM)->sys_ret_info[BOOT_PC] = frame->epc;
	(NVRAM)->sys_ret_info[BOOT_ADDRESS] = 0;  /* init */
	if(reason == EXC_BUSERR) {
	    if(((frame->cause & CAUSE_MASK) >> CAUSE_SHIFT) == CAUSE_BUS_INSTR) {
		/* instruction fetch caused the bus error */
		(NVRAM)->sys_ret_info[BOOT_ADDRESS] = frame->epc;
	    } else if(!illegal_addr(frame->epc)) {
		/* 
		** Data fetch caused the bus error.
		**
		** extract the error address from the instruction at the epc
		**
		** Get the op code address, the base register and the offset
		** Note, big endian has most sig. byte at low address
		*/
		op = (union instr_r4k *)((frame->cause & CAUSE_BD_SLOT) ?
					 (frame->epc + 4) :
					 (frame->epc) );
		dest_reg = get_context_reg(op->i_t.rs);
		(NVRAM)->sys_ret_info[BOOT_ADDRESS] =
		    ((long)frame->regs[dest_reg] + op->i_t.s_imd);
	    }
	}
    }
    (NVRAM)->sys_ret_info[BOOT_REASON] = reason;
    (NVRAM)->sys_ret_info[BOOT_COUNT] = BOOTDATA;
    savenv();  /* write it out to NVRAM */
    return(numframes);
}


/*
 * pred_cache_exception:
 * Handle a predator cache exception
 */
void
pred_cache_exception(r4k_context_t *frame)
{
    union cerr_t cerr;

    /* save the original gt64010 parity settings */
    unsigned long save_dram_bank0_param = gt_ptr->dram_bank0_param;
    unsigned long save_dram_bank1_param = gt_ptr->dram_bank1_param;
    unsigned long save_dram_bank2_param = gt_ptr->dram_bank2_param;
    unsigned long save_dram_bank3_param = gt_ptr->dram_bank3_param;

     /*
      * physically disable all the parity check
      */

    setcp0_sreg(getcp0_sreg() | SREG_DE);
    gt_ptr->dram_bank0_param &= ~DRAM_BANK_PARITY;
    gt_ptr->dram_bank1_param &= ~DRAM_BANK_PARITY;
    gt_ptr->dram_bank2_param &= ~DRAM_BANK_PARITY;
    gt_ptr->dram_bank3_param &= ~DRAM_BANK_PARITY;

#ifdef DEBUG
    printf("Cache Exception....\n");
    printf("ERROR EPC = %x\n", frame->err_epc);
#endif

    cerr.cerr = getcp0_cerr();
    hkeepflags |= H_BUSERR;		/* set Bus Error in flags */
    if ( ! sig_callback(sigbusfcn, frame) ) {
	printf("\n*** Cache Error Exception ***\n");
	printf("Cache Err Reg = 0x%x\n", cerr.cerr);
	printf("%s reference, %s cache, %s %s",
	       cerr.f.er ? "data" : "instruction",
	       cerr.f.ec ? "reserved" : "primary",
	       cerr.f.ed ? "data field error" : "",
	       cerr.f.et ? ", tag field error" : "" );
	if (cerr.f.es) {
	    printf(", cache miss parity error %s SysAD Bus %s\n",
		   cerr.f.ee ? "on" : "not on",
		   cerr.f.eb ? ", data error also occurred" : "");
	} else {
	    printf(", error %s SysAD Bus %s\n", 
		   cerr.f.ee ? "on" : "not on",
		   cerr.f.eb ? ", data error also occurred" : "");
	}
	excpt_info(frame);
	chk_abort(frame, 0);
    } else {
	frame->err_epc += (frame->cause & CAUSE_BD_SLOT)?8:4;
    }

    /* restore the original gt64010 parity settings */
    gt_ptr->dram_bank0_param = save_dram_bank0_param;
    gt_ptr->dram_bank1_param = save_dram_bank1_param;
    gt_ptr->dram_bank2_param = save_dram_bank2_param;
    gt_ptr->dram_bank3_param = save_dram_bank3_param;
 
    cache_flush(0, -1); /* flush caches to continue */
 
    /* manually clear EXL bit since eret only clears ERL */
    setcp0_sreg(getcp0_sreg() & ~SREG_EXL);
    frame->sreg &= ~SREG_EXL;

}

/* End of Module */
