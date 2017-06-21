/* $Id: exceptn_mips.c,v 3.1.24.1 1996/03/21 23:30:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/exceptn_mips.c,v $
 *------------------------------------------------------------------
 * exceptn_mips.c
 *
 * Feb 1996, mansonw
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exceptn_mips.c,v $
 * Revision 3.1.24.1  1996/03/21  23:30:08  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/02/28  01:00:28  szhang
 * CSCdi50105:  C7100 ROMMON code cleanup
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1996/02/16  21:41:34  szhang
 * CSCdi49173:  Separate the startup and exception code
 * Branch: ELC_branch
 * Separate the startup and exception code into platform
 * dependent and platform independent files.
 *
 * Revision 3.1  1996/02/16  01:39:39  mansonw
 * Prepare for code modulization for exception and utility code
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "dis_mips.h"
#include "cpu_mips.h"
#include "extern.h"
#include "error.h"

extern r4k_context_t kernel_context_body, *kernel_context_ptr;
extern unsigned short cpu_flags;
extern unsigned short internal_test;
extern SIG_PF sigbusfcn;
extern r4k_exception_t cause_table[];
extern long vec_start, vec_end;

volatile unsigned long berr_addr;

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
 unsigned long
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
 * sig_callback :
 * Check if signal is enabled. If so, call the signal function
 * If not, check for ignore or default. Returns 0 if default behaviour
 * is required, else 1
 */
int
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
void
excpt_info(r4k_context_t *frame)
{
    if ( frame->sreg & SREG_ERL ) {
	printf("PC = 0x%x, ", frame->err_epc );
    } else {
	printf("PC = 0x%x, ", frame->epc );
    }
    printf("Cause = 0x%x, Status Reg = 0x%x\n", 
	   frame->cause, frame->sreg);
}

/*
 * sw_int_zero : 
 * Negate the interrupt, check if it is a test, if not print info. 
 */
void
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
void
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
 * handle_tlbmod : Handle TLB Modification exception
 */
void
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
void
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
void
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
void
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
void
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
void
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
void
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
void
handle_syscall(r4k_context_t *frame, char *str)
{
    unsigned long sreg;
    unsigned long a0, a1, a2, a3;

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

	sreg = frame->sreg;
	sreg = (sreg & ~SREG_IMASK) | SREG_IMASK7;  /* int level 7 only */
	setcp0_sreg(sreg);
	sreg |= SREG_IE;  /* enable interrupts */
	setcp0_sreg(sreg);
	sreg &= ~SREG_EXL;  /* clear EXL */
	setcp0_sreg(sreg);

	a0 = frame->regs[A0_REGNUM];
	a1 = frame->regs[A1_REGNUM];
	a2 = frame->regs[A2_REGNUM];
	a3 = frame->regs[A3_REGNUM];

	/*
	 * Process the EMT call and sign extend the return value
	 */
	frame->regs[V0_REGNUM] = emulate(frame, a0, a1, a2, a3);
	frame->regs[V0_REGNUM-1] = (frame->regs[V0_REGNUM] >> 31) ? -1 : 0;
    }
}

/*
 * handle_bp : Handle Break point exception
 */
void
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
void
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
 * cache_exception:
 * Handle a cache exception
 */
void
cache_exception(r4k_context_t *frame)
{
    union cerr_t cerr;

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

    cause = (frame->cause & CAUSE_MASK) >> CAUSE_SHIFT;
    ptr = &cause_table[cause];
    if ( ptr->func ) {
	(*ptr->func)(frame, ptr->str);
    } else {
	printf("\n%s\n", ptr->str);
	excpt_info(frame);
	chk_abort(frame, 0);
    }
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
