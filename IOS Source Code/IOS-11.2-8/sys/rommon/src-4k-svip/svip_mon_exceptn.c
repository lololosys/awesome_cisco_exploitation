/* $Id: svip_mon_exceptn.c,v 1.1.6.1 1996/07/08 22:43:08 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_mon_exceptn.c,v $
 *------------------------------------------------------------------
 * svip_mon_exceptn.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_mon_exceptn.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:08  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:01  pgreenfi
 * Placeholder files for src-4k-svip.
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

#define COUNT2NANOSEC(count)       20 * count
#define NANOSEC2COUNT(nanosec)     nanosec/20
#define TIMER_EXPIRED(current, timer) ((current) > (timer))

#define MON_TIMER_VAL_MAX   2 * 1000 * 1000 * 1000
/*
 * External variables and functions
 */
extern unsigned char doauxtest;
extern long vec_start, vec_end;
extern int optind, sys_state;
extern void timerint();
extern SIG_PF sigbusfcn;
extern SIG_PF sigdiagfcn;
extern void clpd6720_mgmt_intr_handler(void);

/*
 * Variables defined in this module
 */
volatile unsigned char sh_statreg_snapshot, in_parity_test;
volatile int lev6count;
volatile unsigned long berr_addr;
volatile unsigned short cpu_flags;
unsigned short system_imask;
r4k_context_t *process_context_ptr;
r4k_context_t *kernel_context_ptr;
r4k_context_t kernel_context_body;
static unsigned short internal_test;
static unsigned char dcl_int_vect;
static unsigned char dcl_int_vmi;
static unsigned char memd_int_stat;

/*
 * Forward declarations
 */
static void chk_abort(r4k_context_t *frame, int offset);
static void abort_task(void);
static void handle_interrupt(r4k_context_t *frame, char *str);
static void handle_tlbmod(r4k_context_t *frame, char *str);
static void handle_tlbl(r4k_context_t *frame, char *str);
static void handle_tlbs(r4k_context_t *frame, char *str);
static void handle_adel(r4k_context_t *frame, char *str);
static void handle_ades(r4k_context_t *frame, char *str);
static void handle_ibe(r4k_context_t *frame, char *str);
static void handle_dbe(r4k_context_t *frame, char *str);
static void handle_syscall(r4k_context_t *frame, char *str);
static void handle_bp(r4k_context_t *frame, char *str);
static void handle_ri(r4k_context_t *frame, char *str);

static void QA_network_int(r4k_context_t *frame);
static void QA_command_int(r4k_context_t *frame);
static void cybus_stall(r4k_context_t *frame);
static void pcmcia_int(r4k_context_t *frame);
static void dcl_uart_int(r4k_context_t *frame);
static void dcl_write_berr(r4k_context_t *frame);
static void dcl_unused_interrupt(r4k_context_t *frame);    
static void ci_int_handler();


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
 * List of handlers for DCL
 */
void (*dcl_interrupt_table[])() = {
    QA_network_int,                     /* Level 0 */
    QA_command_int,                     /* Level 1 */
    cybus_stall,                        /* Level 2 */
    pcmcia_int,                         /* Level 3 */
    dcl_uart_int,                       /* Level 4 */
    timerint,                           /* Level 5 */
    dcl_write_berr,                     /* Level 6 */
    dcl_unused_interrupt,               /* Level 7 */    
};


/* 
 * Constant definition
 */
#define EXC_STATUS_SHIFT                4
#define EXC_NUM                         4
#define RST_WAIT_TIME                   0x2000

#define COMPUTE_DBUS_ADDRESS(slot)  (chasint_dbus_t *)((slot * 0x40) + DGBUSSTART)

/*
 * copy_context : Copy a stacked context into a context structure
 * and set valid. The err_flag parameter indicates which register holds
 * the real PC. If this is REG_ERR_EPC, copy this value into the
 * REG_EPC to allow the utilities to operate correctly
 */
long
copy_context(r4k_context_t *frame, int err_flag)
{
    r4k_context_t *context;

    if ( sys_state != SYSTEM_RUNNING )
	return(0);
    context = &kernel_context_body;
    kernel_context_ptr = context;
    movbyte((char *)frame, (char *)context, sizeof(r4k_context_t));
    if ( err_flag ) {
	context->epc = context->err_epc;
	context->epc_hi = context->err_epc_hi;
    }
    return((long)context);
}

/*
 * get_context_reg:
 * Return index into context structure
 */
int
get_context_reg(int reg)
{
    return((reg*2)+1);
}

/*
 * emul_bra : Emulate the branch instruction when an exception occurs in
 * the branch delay slot
 */
static unsigned long
emul_bra(r4k_context_t *frame)
{
    union instr_r4k *op = (union instr_r4k *)frame->epc;
    int rs_reg, rd_reg, rt_reg;
    long offset;

    switch ( op->i_t.op ) {

    case OPCODE_SPECIAL:
	
	switch ( op->r_t.func ) {
	    
	case OPCODE_JALR:
	    rd_reg = get_context_reg(op->r_t.rd);
	    rs_reg = get_context_reg(op->r_t.rs);
	    frame->regs[rd_reg] = (unsigned long)op + 8;
	    return( frame->regs[rs_reg] );
	    
	case OPCODE_JR:
	    rs_reg = get_context_reg(op->r_t.rs);
	    return( frame->regs[rs_reg] );
	    
	default: 
	    return(0);
	}
	break;
	
    case OPCODE_REGIMM:

	switch ( op->r_t.rt ) {
	    
	case OPCODE_BGEZ:
	case OPCODE_BGEZL:
	    rs_reg = get_context_reg(op->i_t.rs);
	    if ( (long)frame->regs[rs_reg] < 0 )
		return( (unsigned long)op + 8 );
	    offset = (long)(op->i_t.s_imd);
	    return( (unsigned long)op + 4 + (offset << 2) );
	    
	case OPCODE_BGEZAL:
	case OPCODE_BGEZALL:
	    rs_reg = get_context_reg(op->i_t.rs);
	    frame->regs[RA_REGNUM] = (unsigned long)op + 8;
	    if ( frame->regs[rs_reg] < 0 )
		return( (unsigned long)op + 8 );
	    offset = (long)(op->i_t.s_imd);
	    return( (unsigned long)op + 4 + (offset << 2) );
	    
	case OPCODE_BLTZ:
	case OPCODE_BLTZL:
	    rs_reg = get_context_reg(op->i_t.rs);
	    if ( frame->regs[rs_reg] >= 0 )
		return( (unsigned long)op + 8 );
	    offset = (long)(op->i_t.s_imd);
	    return( (unsigned long)op + 4 + (offset << 2) );
	    
	case OPCODE_BLTZAL:
	case OPCODE_BLTZALL:
	    rs_reg = get_context_reg(op->i_t.rs);
	    frame->regs[RA_REGNUM] = (unsigned long)op + 8;
	    if ( frame->regs[rs_reg] >= 0 )
		return( (unsigned long)op + 8 );
	    offset = (long)(op->i_t.s_imd);
	    return( (unsigned long)op + 4 + (offset << 2) );
	    
	default: 
	    return(0);
	}
	break;

    case OPCODE_J:
	return(((unsigned long)op & HI_ADDR_MASK) + (op->j_t.target << 2));
	
    case OPCODE_JAL:
	frame->regs[RA_REGNUM] = (unsigned long)op + 8;
	return(((unsigned long)op & HI_ADDR_MASK) + (op->j_t.target << 2));
	
    case OPCODE_BEQ:
    case OPCODE_BEQL:
	rs_reg = get_context_reg(op->i_t.rs);
	rt_reg = get_context_reg(op->i_t.rt);
	if ( frame->regs[rs_reg] != frame->regs[rt_reg] )
	    return( (unsigned long)op + 8 );
	offset = (long)(op->i_t.s_imd);
	return( (unsigned long)op + 4 + (offset << 2) );
	
    case OPCODE_BGTZ:
    case OPCODE_BGTZL:
	rs_reg = get_context_reg(op->i_t.rs);
	if ( frame->regs[rs_reg] <= 0 )
	    return( (unsigned long)op + 8 );
	offset = (long)(op->i_t.s_imd);
	return( (unsigned long)op + 4 + (offset << 2) );
	
    case OPCODE_BLEZ:
    case OPCODE_BLEZL:
	rs_reg = get_context_reg(op->i_t.rs);
	if ( frame->regs[rs_reg] > 0 )
	    return( (unsigned long)op + 8 );
	offset = (long)(op->i_t.s_imd);
	return( (unsigned long)op + 4 + (offset << 2) );
	
    case OPCODE_BNE:
    case OPCODE_BNEL:
	rs_reg = get_context_reg(op->i_t.rs);
	rt_reg = get_context_reg(op->i_t.rt);
	if ( frame->regs[rs_reg] == frame->regs[rt_reg] )
	    return( (unsigned long)op + 8 );
	offset = (long)(op->i_t.s_imd);
	return( (unsigned long)op + 4 + (offset << 2) );
	
    default: 
	return(0);
    } 
}

/*
 * init_exceptions :
 * Initialize exceptions sub-system. When the system runs with BEV de-asserted
 * the exception handling code must be moved to KSeg0 (IE cachable).
 */
void
init_exceptions(void)
{
    unsigned long sreg = getcp0_sreg();
    unsigned long temp;

    /*
     * clear all pending error interrupt which is reported 
     * Nevada equivalent interrupt controller. These interrupts
     * are edge sensitive. When you read these two registers, 
     * interrupts are cleared.
     */
    temp = KSEG1_IO_ASIC->nev_isr;
    temp = KSEG1_IO_ASIC->be_isr;
    
#ifdef USEINTR
    /*
     * Enable the DBUS interrupt to NEVADA interrupt controller
     */
    KSEG1_IO_ASIC->nev_imr |= NEV_IMR_DBUS_INTR;
#endif
    /* 
     * Currently, we use polling to handle dbus interrupt for download.
     * So, all interrupts are masked.
     */
    KSEG1_IO_ASIC->nev_imr = 0;

    /* 
     * Disable all the interrupts.
     */
    sreg &= (~SREG_IMASK2 & ~SREG_IMASK3 & ~SREG_IMASK4 & ~SREG_IMASK5 & 
             ~SREG_IMASK6 & ~SREG_IMASK7 & ~SREG_IE);
    setcp0_sreg(sreg);

}

/*
 * intr_int7_on()
 */
intr_int7_on()
{
    unsigned long sreg = getcp0_sreg();

    sreg |= SREG_IMASK7;
    setcp0_sreg(sreg);
}


/*
 * sig_callback :
 * Check if signal is enabled. If so, call the signal function
 * If not, check for ignore or default. Returns 0 if default behaviour
 * is required, else 1
 */
static int
sig_callback(SIG_PF fnc, r4k_context_t *frame)
{

    switch ( (long)fnc ) {
    case SIG_IGN: 
	return(1);
    case SIG_DFL: 
	return(0);
    default:      
	if ((*fnc)((int)frame) == (int)SIG_DFL) {
	    return(0);
	} else {
	    return(1);
	}
    }
}

/*
 * excpt_info : Print out some relevant information for exceptions
 * If Sreg Bit 2 (ERL) is 1, print Error EPC, Else print EPC
 */
static void
excpt_info(r4k_context_t *frame)
{
    if ( frame->sreg & SREG_ERL ) {
	printf("PC = 0x%x, ", frame->err_epc );
    } else {
	printf("PC = 0x%x, ", frame->epc );
    }
    printf("Cause = 0x%x, Status Reg = 0x%x\n", 
	   frame->cause, frame->sreg);
    dump_int_reg();
}


/*
 * QA_network_int :
 * QA network interrupt.
 * This interrupt is caused by not-empty status in 
 * network queue.
 */
static void
QA_network_int(r4k_context_t *frame)
{

    printf("*** QA Network Interrupt (DCL int 0) ***\n");
    excpt_info(frame);
}


/*
 * QA_command_int :
 * QA command interrupt.
 * This interrupt is caused by not-empty status in 
 * command queue.
 */
static void
QA_command_int(r4k_context_t *frame)
{

    printf("*** QA Command Interrupt (DCL int 1) ***\n");
    excpt_info(frame);
}


/*
 * Cybus_stall :
 * Cybus stall interrupt.
 */
static void
cybus_stall(r4k_context_t *frame)
{
    printf("*** Cybus Stall Interrupt (DCL int 2)  ***\n");

    excpt_info(frame);
}


/*
 * pcmcia_int :
 * PCMCIA controller interrupt
 */
static void
pcmcia_int(r4k_context_t *frame)
{
    printf("*** PCMCIA Controller Interrupt (DCL int 3) ***\n");
    excpt_info(frame);
}


/*
 * dcl_uart_int :
 * DCL reported UART interrupt
 */
static void
dcl_uart_int(r4k_context_t *frame)
{
    printf(" DUART interrupt \n");
    excpt_info(frame);

}



/*
 * dcl_unused_interrupt:
 * DCL ASIC reported an unused interrupt
 */
static void
dcl_unused_interrupt(r4k_context_t *frame)
{
    printf("\n*** DCL ASIC Reported Unused Interrupt (DCL int 7) ***\n");
    excpt_info(frame);
}

	    
/*
 * dcl_write_berr:
 *  Write bus error interrupt 
 */
static void 
dcl_write_berr(r4k_context_t *frame)
{
} /* dcl_write_berr */
    

/*
 * sw_int_zero : 
 * Negate the interrupt, check if it is a test, if not print info. 
 */
static void
sw_int_zero(r4k_context_t *frame)
{
    unsigned long cause = getcp0_cause();

    cpu_flags = SREG_IMASK0;
    setcp0_cause(cause & ~CAUSE_IBIT0);
    if ( !internal_test ) {
	printf("\n*** Software Interrupt 0 ***\n");
	excpt_info(frame);
    }
}

/*
 * sw_int_one : 
 * Negate the interrupt, check if it is a test, if not print some info.
 */
static void
sw_int_one(r4k_context_t *frame)
{
    unsigned long cause = getcp0_cause();

    cpu_flags = SREG_IMASK1;
    setcp0_cause(cause & ~CAUSE_IBIT1);
    if ( !internal_test ) {
	printf("\n*** Software Interrupt 1 ***\n");
	excpt_info(frame);
    }
}


/* 
 * dcl_int
 * Handle external interrupts from DCL
 */
static void
dcl_int(r4k_context_t *frame)
{
} /* dcl_int */


save_int_reg()
{
#if 0
    dcl_int_vect = DCL_ASIC->int_vect;
    dcl_int_vmi = DCL_ASIC->int_vmi;
    memd_int_stat = MEMD_ASIC->int_stat_reg;
#endif
}

dump_int_reg()
{
    printf("DCL Masked Interrupt Register = 0x%08x\n", dcl_int_vmi);
    printf("DCL Interrupt Value Register = 0x%08x\n", dcl_int_vect);
    printf("MEMD Int 6 Status Register = 0x%08x\n", memd_int_stat);
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
#ifdef CONBRKINT
    cpu_flags = SREG_IMASK7;
    if (!internal_test && sys_state != MONITOR_RUNNING) {
	printf("\n*** Count / Compare Exception ***\n");
	excpt_info(frame);
    }
    setcp0_compare(0);  /* this clears the interrupt */
#else
    /*
    ** The break mechanism is handled by count/compare
    */
    long compval;
    long rpc;

    if(internal_test) {
	cpu_flags = SREG_IMASK7;
	compval = 0;
    } else {
	compval = MON_TIMER_VAL_20MS;
	rpc = isconbrk();  /* poll the UART for a break condition */
	if(rpc) {  /* rig the stack with the desired return pc */
	    copy_context(frame, 0);
	    frame->epc = rpc;
	    frame->epc_hi = KSEG_MSB_ADDR;
	}
    }
    setcp0_count(0);  /* clear count */
    setcp0_compare(compval);  /* this clears the interrupt */
#endif
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

    } else if (intrpt & CAUSE_IBIT2) {
	dcl_int(frame);
	return;
    }

    /*
     * One of our un-connnected interrupts caused
     * us to come here.
     */
    goto unknown_interrupt;
}

/*
 * handle_tlbmod : Handle TLB Modification exception
 */
static void
handle_tlbmod(r4k_context_t *frame, char *str)
{
    cpu_flags = CAUSE_TLB_MOD;
    hkeepflags |= H_BUSERR;
    berr_addr = frame->badvaddr;
    if ( ! sig_callback(sigbusfcn,frame) ) {
	printf("\n%s\n", str);
	printf("Access address = 0x%x\n", berr_addr);
	excpt_info(frame);
	chk_abort(frame, (frame->cause & CAUSE_BD_SLOT)?8:4);
    } else {
	frame->epc += (frame->cause & CAUSE_BD_SLOT)?8:4;
    }
}

/*
 * handle_tlbl : Handle TLB exception for load or opcode fetch
 */
static void
handle_tlbl(r4k_context_t *frame, char *str)
{
    cpu_flags = CAUSE_TLB_LOAD;
    hkeepflags |= H_BUSERR;
    berr_addr = frame->badvaddr;
    if ( ! sig_callback(sigbusfcn,frame) ) {
	printf("\n%s\n", str);
	printf("Access address = 0x%x\n", berr_addr);
	excpt_info(frame);
	chk_abort(frame, (frame->cause & CAUSE_BD_SLOT)?8:4);
    } else {
	frame->epc += (frame->cause & CAUSE_BD_SLOT)?8:4;
    }
}

/*
 * handle_tlbs : Handle TLB exception for store
 */
static void
handle_tlbs(r4k_context_t *frame, char *str)
{
    cpu_flags = CAUSE_TLB_SAVE;
    hkeepflags |= H_BUSERR;
    berr_addr = frame->badvaddr;
    if ( ! sig_callback(sigbusfcn,frame) ) {
	printf("\n%s\n", str);
	printf("Access address = 0x%x\n", berr_addr);
	excpt_info(frame);
	chk_abort(frame, (frame->cause & CAUSE_BD_SLOT)?8:4);
    } else {
	frame->epc += (frame->cause & CAUSE_BD_SLOT)?8:4;
    }
}

/*
 * handle_adel : Handle Address error exception (load/fetch)
 */
static void
handle_adel(r4k_context_t *frame, char *str)
{
    union instr_r4k *op;
    int s_reg, d_reg, i;
    unsigned char *addr, buf[4];
    unsigned short sh_unsig;
    short sh_sig;
    long long_sig;
    unsigned long sreg;

    if ( frame->epc & 0x3 ) {              /* Fetch caused the error */
	printf("\n%s\n", str);
	printf("Access address = 0x%x\n", frame->badvaddr);
	excpt_info(frame);
	chk_abort(frame, 0);
	return;
    }

    /*
     * Re-enable interrupts, allowing the timer to tick
     */
    sreg = getcp0_sreg();
    setcp0_sreg(sreg & ~SREG_EXL);

    if ( !((NVRAM)->diagflag & D_QUIETMODE) ) {
	printf("\n*** Emulating mis-aligned load at 0x%.8x\n", 
	       frame->badvaddr);
	printf("    PC = 0x%.8x ... ", frame->epc);
    }

    /*
     * Get the op code address, the base register and the offset
     * Note, big endian has most sig. byte at low address
     */
    op = (union instr_r4k *)((frame->cause & CAUSE_BD_SLOT) ?
			     (frame->epc + 4) :
			     (frame->epc) );
    s_reg = get_context_reg(op->i_t.rs);
    d_reg = get_context_reg(op->i_t.rt);
    addr = (unsigned char *)((long)frame->regs[s_reg] + op->i_t.s_imd);
    switch ( op->i_t.op ) {
    case OPCODE_LHU:
	for (i=0; i<2; i++)
	    buf[i] = *addr++;
	sh_unsig = *(unsigned short *)(buf);
	frame->regs[d_reg] = (unsigned long)sh_unsig;
	break;
    case OPCODE_LH:
	for (i=0; i<2; i++)
	    buf[i] = *addr++;
	sh_sig = *(short *)(buf);
	long_sig = (long)sh_sig;
	frame->regs[d_reg] = long_sig;
	break;
    case OPCODE_LW:
	for (i=0; i<4; i++)
	    buf[i] = *addr++;
	long_sig = *(long *)buf;
	frame->regs[d_reg] = long_sig;
	break;
    case OPCODE_LWU:
	for (i=0; i<4; i++)
	    buf[i] = *addr++;
	frame->regs[d_reg] = *(unsigned long *)buf;
	frame->regs[d_reg-1] = 0;
	break;
    default:
	if ( !((NVRAM)->diagflag & D_QUIETMODE) )
	    printf("failed, opcode = 0x%x\n", op->i_t.op);
	chk_abort(frame, 0);
	return;
    }
    if ( frame->cause & CAUSE_BD_SLOT ) {
	if ( frame->epc = emul_bra(frame), !frame->epc ) {
	    if ( !((NVRAM)->diagflag & D_QUIETMODE) )
		printf("failed, unknown branch\n");
	    chk_abort(frame, 0);
	} else {
	    if ( !((NVRAM)->diagflag & D_QUIETMODE) )
		printf("succeeded\n");
	}
    } else {
	if ( !((NVRAM)->diagflag & D_QUIETMODE) )
	    printf("succeeded\n");
	frame->epc += 4;
    }
}

/*
 * handle_ades : Handle Address error exception (store)
 */
static void
handle_ades(r4k_context_t *frame, char *str)
{
    union instr_r4k *op;
    int s_reg, d_reg, i;
    unsigned char *addr;
    unsigned long sreg;

    /*
     * Re-enable interrupts, allowing the timer to tick
     */
    sreg = getcp0_sreg();
    setcp0_sreg(sreg & ~SREG_EXL);

    if ( !((NVRAM)->diagflag & D_QUIETMODE) ) {
	printf("\n*** Emulating mis-aligned store at 0x%.8x\n", 
	       frame->badvaddr);
	printf("    PC = 0x%.8x ... ", frame->epc);
    }

    /*
     * Get the op code address, the base register and the offset
     * Note, big endian has most sig. byte at low address
     */
    op = (union instr_r4k *)((frame->cause & CAUSE_BD_SLOT) ?
			     (frame->epc + 4) :
			     (frame->epc) );
    d_reg = get_context_reg(op->i_t.rs);
    s_reg = get_context_reg(op->i_t.rt);
    addr = (unsigned char *)((long)frame->regs[d_reg] + op->i_t.s_imd);
    switch ( op->i_t.op ) {
    case OPCODE_SH:
	for (i=1; i>=0; i--)
	    *addr++ = (frame->regs[s_reg] >> i*8) & 0xff;
	break;
    case OPCODE_SW:
	for (i=3; i>=0; i--)
	    *addr++ = (frame->regs[s_reg] >> i*8) & 0xff;
	break;
    default:
	if ( !((NVRAM)->diagflag & D_QUIETMODE) )
	    printf("failed, opcode = 0x%x\n", op->i_t.op);
	chk_abort(frame, 0);
	return;
    }
    if ( frame->cause & CAUSE_BD_SLOT ) {
	if ( frame->epc = emul_bra(frame), !frame->epc ) {
	    if ( !((NVRAM)->diagflag & D_QUIETMODE) )
		printf("failed, unknown branch\n");
	    chk_abort(frame, 0);
	} else {
	    if ( !((NVRAM)->diagflag & D_QUIETMODE) )
		printf("succeeded\n");
	}
    } else {
	if ( !((NVRAM)->diagflag & D_QUIETMODE) )
	    printf("succeeded\n");
	frame->epc += 4;
    }
}

/*
 * handle_ibe : Handle Bus error exception for opcode fetch
 */
static void
handle_ibe(r4k_context_t *frame, char *str)
{
    hkeepflags |= H_BUSERR;
    berr_addr = (long)frame->epc + (frame->cause & CAUSE_BD_SLOT)?4:0;
    printf("\n%s\n", str);
    printf("Access address = 0x%x\n", berr_addr);
    excpt_info(frame);
    chk_abort(frame, 0);
}

/*
 * excpt_dbe : Handle Bus error exception for data loads
 */
static void
handle_dbe(r4k_context_t *frame, char *str)
{
    union instr_r4k *op;
    int reg;

    hkeepflags |= H_BUSERR;
    /*
     * Decode instruction at REG_EPC and calculate illegal
     * virtual address
     */
    op = (union instr_r4k *)(frame->cause & CAUSE_BD_SLOT ?
			     frame->epc + 4 :
			     frame->epc);
    reg = get_context_reg(op->i_t.rs);
    berr_addr = ((long)frame->regs[reg] + (long)op->i_t.s_imd);
    if ( ! sig_callback(sigbusfcn,frame) ) {
	printf("\n%s\n", str);
	printf("Access address = 0x%x\n", berr_addr);
	excpt_info(frame);
	chk_abort(frame, (frame->cause & CAUSE_BD_SLOT)?8:4);
    } else {
	frame->epc += (frame->cause & CAUSE_BD_SLOT)?8:4;
    }
}

/*
 * handle_syscall : Handle System call exception
 */
static void
handle_syscall(r4k_context_t *frame, char *str)
{
    unsigned long sreg;
    unsigned long a0, a1, a2, a3;

*(unsigned short volatile *)0xa0000440 = 0x34;
*(unsigned long volatile *)0xa0000444 = getcp0_sreg();
    cpu_flags = frame->regs[A0_REGNUM];
    if ( frame->cause & CAUSE_BD_SLOT ) {
	printf("\n*** Warning, System Call in Branch Delay Slot\n");
	frame->epc += 8;
    } else {
	frame->epc += 4;
    }
    if ( !internal_test ) {

	/*
	 * If we need to take a lot of time we must re-enable
	 * interrupts so the watchdog gets serviced. Do the
	 * following :
	 *
	 * set IP 7
	 * IP 6-0 off
	 * set IE
	 * clear EXL
	 *
	 * This enables the count compare int only.  This allows
	 * console output during EMT call.
	 */
#if 0
	sreg = frame->sreg;
	sreg = (sreg & ~SREG_IMASK) | SREG_IMASK7;  /* int level 7 only */
	setcp0_sreg(sreg);
	sreg |= SREG_IE;  /* enable interrupts */
	setcp0_sreg(sreg);
	sreg &= ~SREG_EXL;  /* clear EXL */
	setcp0_sreg(sreg);
#endif

	a0 = frame->regs[A0_REGNUM];
	a1 = frame->regs[A1_REGNUM];
	a2 = frame->regs[A2_REGNUM];
	a3 = frame->regs[A3_REGNUM];

	/*
	 * Process the EMT call and sign extend the return value
	 */
*(unsigned short volatile *)0xa0000448 = a0;
	frame->regs[V0_REGNUM] = emulate(frame, a0, a1, a2, a3);
	frame->regs[V0_REGNUM-1] = (frame->regs[V0_REGNUM] >> 31) ? -1 : 0;
    }
}

/*
 * handle_bp : Handle Break point exception
 */
static void
handle_bp(r4k_context_t *frame, char *str)
{
    int offset = 4;

    cpu_flags = CAUSE_BP;
    if ( !internal_test ) {
	printf("\n%s\n", str);
	excpt_info(frame);
    }
    if ( frame->cause & CAUSE_BD_SLOT ) {
	printf("*** Warning, Breakpoint in Branch Delay Slot\n");
	offset = 8;
    }
    if ( !internal_test ) {
	chk_abort(frame, offset);
    } else {
	frame->epc += offset;
    }
}

/*
 * handle_ri : Handle Reserved instruction exception
 */
static void
handle_ri(r4k_context_t *frame, char *str)
{
    cpu_flags = CAUSE_ILLOP;
    if ( !internal_test ) {
	printf("\n%s\n", str);
	excpt_info(frame);
    }
    if ( !internal_test )
	chk_abort(frame, 0);
    else
	frame->epc += 4;
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
static void
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
    }

}


/*
 * cache_exception:
 * Handle a cache exception
 */
void
cache_exception(r4k_context_t *frame)
{
    union cerr_t cerr;

    cerr.cerr = getcp0_cerr();
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
}

/*
 * general_exception:
 * Handle all other exceptions (besides the cache)
 */
void
general_exception(r4k_context_t *frame)
{
    int cause;
    r4k_exception_t *ptr;

#ifdef DEBUG
/*
if(frame->cause != 0x8000 || frame->cause != 0x80008000)
printf("Excptn: Cause %#x\n",frame->cause);
*/
#endif

    cause = (frame->cause & CAUSE_MASK) >> CAUSE_SHIFT;
    ptr = &cause_table[cause];

    if ( ptr->func ) {
	(*ptr->func)(frame, ptr->str);
    } else {
	printf("\n%s\n", ptr->str);
	excpt_info(frame);
	chk_abort(frame, 0);
    }
#ifdef DEBUG
/*
if(frame->cause != 0x8000 || frame->cause != 0x80008000)
printf("eox\n");
*/
#endif
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
     ** Do not allow user to <break> out of this routine.
     ** Save monjmpptr and set to NULL.
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
	 * Reinitialize DCL interrupt mask and enable
	 * the UART, print out some information about the crash.
	 * We will only take parity and Write Berr interrupts
	 */
#if 0
	system_imask = DCL_ASIC->int_mask;
	DCL_ASIC->int_mask = ~INT_LEV6;		
	uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
#endif
	/*
	** Errptr must be set in each case of the following switch statement.
	** This must be backwards compatible with
	** the error reporting mechanism in the old 68k monitor.
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
         * Shared memory parity error
         */
	case SIGERROR:
	    errcode = EXC_SHMEM_PE;
	    errptr = "a Shared Memory Parity Error";
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
	** Log the info for future use
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
** log_sys_ret : log the system return info for future use
*/
int
log_sys_ret(int reason, r4k_context_t *frame)
{
    union instr_r4k *op;
    int dest_reg;
    int numframes = 0;

    /*
    ** Only log errors from healthy target images or reset
    */
    if(reason != EXC_RESET && !boot_satisfied) return;

    /*
    ** If a system process context exists, use it
    */
    if(process_context_ptr) frame = process_context_ptr;

    /* init entire structure with zeroes */
    bzero(&(NVRAM)->sys_ret_info[BOOT_STACK_TRACE],
	  sizeof((NVRAM)->sys_ret_info));

    if(frame) {
	/*
	** get our epc right (cache parity errors use err_epc)
	*/
	if ( frame->sreg & SREG_ERL ) frame->epc = frame->err_epc;

	numframes = stack_trace((unsigned long *)frame->epc,
				(unsigned long *)frame->regs[SP_REGNUM],
				frame->regs[RA_REGNUM], BOOT_STACK_FRAMES,
				&(NVRAM)->sys_ret_info[BOOT_STACK_TRACE]);

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
 * test_cntcmp : Test Count/Compare interrupt
 */
char *
test_cntcmp(void)
{
    unsigned long sreg = getcp0_sreg(), cnt;
    char *err = (char *)0;

    setcp0_sreg(sreg&~(SREG_IE|SREG_IMASK));       /* disable all interrupts */
    cnt = getcp0_count();
    wastetime(10);
    if ( getcp0_count() == cnt ) {
	err = "count register is not incrementing";
	goto test_exit;
    }
    cnt = getcp0_count();
    setcp0_compare(cnt+0x1000);
    if ( getcp0_cause() & SREG_IMASK7 ) {
	err = "count interrupt not negated after write to compare";
	goto test_exit;
    }
    wastetime(0x800);
    if ( !(getcp0_cause() & SREG_IMASK7 ) ) {
	err = "count interrupt not asserted after count equals compare";
	goto test_exit;
    }
    cnt = getcp0_count();
    setcp0_compare(cnt+0x1000);
    internal_test = 1;
    setcp0_sreg(sreg | SREG_IE | SREG_IMASK7 );    /* enable count interrupt */
    wastetime(0x800);                              /* interrupt here */
    setcp0_sreg(sreg&~SREG_IE);                    /* disable interrupts */
    internal_test = 0;
    if ( cpu_flags != SREG_IMASK7 )
	err = "count interrupt never occurred";

test_exit:
    internal_test = 0;
    setcp0_sreg(sreg);
    return(err);
}

/*
 * test_swint : Test S/W interrupt 0 and 1
 */
char *
test_swint(unsigned long mask)
{
    unsigned long sreg = getcp0_sreg();
    char *err = (char *)0;

    setcp0_sreg(sreg&~(SREG_IE|SREG_IMASK));       /* disable all interrupts */
    internal_test = 1;
    cpu_flags = 0;
    setcp0_cause( getcp0_cause() | mask );
    if ( cpu_flags ) {
	err = "sw interrupt taken while masked";
	goto test_exit;
    }
    if ( !(getcp0_cause() & mask) ) {
	err = "sw interrupt not indicated in cause register";
	goto test_exit;
    }
    setcp0_cause( getcp0_cause() & ~mask );
    if ( getcp0_cause() & mask ) {
	err = "sw interrupt remains asserted after write to cause reg";
	goto test_exit;
    }
    setcp0_sreg(sreg | SREG_IE | mask );    /* enable sw interrupt */
    setcp0_cause( getcp0_cause() | mask );  /* interrupt should happen here */
    setcp0_sreg(sreg&~SREG_IE);             /* disable interrupts */
    internal_test = 0;
    if ( cpu_flags != mask )
	err = "sw interrupt never occurred";

test_exit:
    internal_test = 0;
    setcp0_sreg(sreg);
    return(err);
}

/*
 * test_syscall : Test SYSCALL
 */
char *
test_syscall(void)
{
    unsigned long sreg = getcp0_sreg();
    char *err = (char *)0;

    setcp0_sreg(sreg&~(SREG_IE|SREG_IMASK));       /* disable all interrupts */
    internal_test = 1;
    cpu_flags = 0;
    sys_call(0x1234);                 /* make a system call */
    if ( cpu_flags != 0x1234 )
	err = "system call parameter passing failed";
    internal_test = 0;
    setcp0_sreg(sreg);
    return(err);
}

/*
 * test_bp : Test breakpoints
 */
char *
test_bp(void)
{
    unsigned long sreg = getcp0_sreg();
    char *err = (char *)0;

    setcp0_sreg(sreg&~(SREG_IE|SREG_IMASK));       /* disable all interrupts */
    internal_test = 1;
    cpu_flags = 0;
    break_pt();                       /* Take a break point */
    if ( cpu_flags != CAUSE_BP )
	err = "break point not taken";
    internal_test = 0;
    setcp0_sreg(sreg);
    return(err);
}

/*
 * test_ri : Test reserved instructions
 */
char *
test_ri(void)
{
    unsigned long sreg = getcp0_sreg();
    char *err = (char *)0;

    setcp0_sreg(sreg&~(SREG_IE|SREG_IMASK));   /* disable all interrupt */
    internal_test = 1;
    cpu_flags = 0;
    ill_opcode();
    if ( cpu_flags != CAUSE_ILLOP )
	err = "illegal instruction exception did not occur";
    internal_test = 0;
    setcp0_sreg(sreg);
    return(err);
}

/*
 * intr_test : Test the internal interrupts
 * Count/Compare, SW0, SW1, System Call, Breakpoint, Illegal Instr
 */
int
intr_test(void)
{
    SIG_PF savfcn;
    unsigned long sreg;
    char *err;

    /*
     * Initialize test name and copy of Status Reg, disable signals
     */
    testname("internal interrupt");
    savfcn = signal(SIGINT, SIG_IGN);
    sreg = getcp0_sreg();

    /* The individual tests will set internal_test = 1 when appropriate */
    if ( (err = test_cntcmp(), err ) ||
	 (err = test_swint(SREG_IMASK0), err) ||
	 (err = test_swint(SREG_IMASK1), err) ||
	 (err = test_syscall(), err) ||
	 (err = test_bp(), err) ||
	 (err = test_ri(), err) )  {
	internal_test = 0;
	signal(SIGINT, savfcn);
	setcp0_sreg(sreg);
	cterr('f',0,err);
	return(1);
    }

    /*
     * Finished successfully, restore Status Register, return success
     */
    internal_test = 0;
    signal(SIGINT, savfcn);
    setcp0_sreg(sreg);
    prcomplete(testpass, errcount, 0);
    return(0);
}

/*
 * move_vectors : Bootstrap (bev == 1), or Normal (bev == 0)
 */
int
move_vectors(int bev)
{
    unsigned long sreg;
    union instr_r4k *op;
    int i, tmp;

#ifdef MIPS64ELF
    bev = 0;
#endif

    if ( bev ) {
	/*
	 * Set vectors to Bootstrap mode
	 */
	setcp0_sreg( getcp0_sreg() | SREG_BEV );
    } else {

	/*
	 * Set vectors to normal mode, disable interrupts, init
	 * some of DRAM, move the code
	 * and restore the IE bit state in status register
	 */
	sreg = getcp0_sreg();
	setcp0_sreg(sreg & ~SREG_IE);

#ifdef DIAGMON
	/*
	** This code is is needed if the stack and bss sections are
	** in NVRAM and memory may not be initialized.
	*/
	filbyte((char *)KSEG1_ADRSPC_RAM, 0x10000, 0);
#endif
	movbyte( (char *)&vec_start, (char *)KSEG1_ADRSPC_RAM,
		(int)(&vec_end) - (int)(&vec_start) );
	cache_flush(0, -1);
	sreg &= ~SREG_BEV;
	setcp0_sreg(sreg);             /* Restore status reg */
    }
    return(0);
}

/*
 * move_stacks : KSeg1 (seg == 1), or KSeg0 (seg == 0)
 */
int
move_stacks(int seg)
{
    cache_flush(0, seg);
}

/*
 * bev_diag : Test the BEV bit in the status register
 */
int
bev_diag(void)
{
    unsigned long sreg;
    char *err = (char *)0;
    SIG_PF savfcn;

    /*
     * Initialize test name and copy of Status Reg, disable signals
     */
    testname("bev state");
    savfcn = signal(SIGINT, SIG_IGN);
    sreg = getcp0_sreg();

    move_vectors(1);              /* Set vectors to Bootstrap */
    if ( test_swint(SREG_IMASK0) ) {
	err = "failed with vectors in Bootstrap Mode";
    } else {
	move_vectors(0);          /* Set vectors to Normal */
	if ( test_swint(SREG_IMASK0) ) {
	    err = "failed with vectors in Normal Mode";
	}
    }
    internal_test = 0;
    signal(SIGINT, savfcn);
    setcp0_sreg(sreg);

    if ( err ) {
	cterr('f',0,err);
	return(1);
    } else {
	prcomplete(testpass, errcount, 0);
	return(0);
    }
}

/* End of Module */
