/* $Id: gdb_4k.c,v 3.1.6.3 1996/04/03 03:11:04 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/gdb_4k.c,v $
 *------------------------------------------------------------------
 * gdb_4k.c -- R4k specific gdb routines
 *  
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_4k.c,v $
 * Revision 3.1.6.3  1996/04/03  03:11:04  mbeesley
 * CSCdi53386:  11.2 C7100 modular-ukernel image wont compile
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/03/21  23:46:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/03/18  21:57:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.8.1  1996/03/18  23:32:53  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1  1996/03/09  00:55:50  hampton
 * Migrate platform specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.6  1996/02/01  06:07:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/06  03:22:47  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.4  1995/11/30  00:02:41  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.3  1995/11/17  18:47:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:18:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/25  11:44:41  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/07/18 06:48:14  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/07/07 05:26:37  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.1  1995/06/07 21:55:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "config.h"
#include "../os/gdb.h"
#include "ttysrv.h"
#include "../os/sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "../dev/monitor1.h"
#include "sys_registry.h"
#include "../os/timer_generic.h"
#include "exception.h"
#include "signal_4k.h"
#include "alignment.h"

/*
 * externs 
 */
extern ulong exception_code_start[], exception_code_end[];
extern void r4k_interrupt_handler(void);
extern void r4k_4ms_interrupt(void);

extern struct {
    ulong r4600_statusreg;	/* "1" bits enable interrupts */
    uchar ext_mask;		/* "1" bits mask interrupts - DCL or NEVADA */
    uint :24;
} r4k_level_table[];

/* Local storage */

r4k_context_t r4k_context_space;	   	/* context space */
cpu_context_t crash_context;		   	/* context for crashing */
int tlb_entries_used = 0;			/* TLB entries used */

static int gdb_crash_sig, gdb_crash_code;	/* crash signal and code */
static volatile ulong break_addr, break_opcode;	/* single step support */

crashinfo crash_ptr = { CRASH_MAGIC,
			CRASH_VERSION,
			CRASH_REASON_NOTCRASHED,
			-1,
			&crash_context.sc,
			(void *)RAMBASE,
                        (void *)TEXTBASE,
                        (void *)DATABASE,
                        (void *)BSSBASE,
			&crash_context
		      };

/*
 * R4k Register names
 */
char *r4k_reg_name[] = {
    "$0", "AT", "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3",
    "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3",
    "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1",
    "gp", "sp", "s8", "ra" 
};

/*
 * Fault history pointer and size
 */
static char *fh_ptr = NULL;
static int   fh_bytes = 0;

/*
 * Fault history line size
 */
#define FH_LINE_SIZE	80

/*
 * exception_save_context:
 * Used to save registers when a process takes an exception that
 * is elligible for debugging.
 */
void
exception_save_context(cpu_context_t *ctx)
{
    bcopy(&r4k_context_space, &ctx->sc, sizeof(r4k_context_t));
}

/*
 * exception_restore_context:
 * Used to restore registers/frame when a process is re-scheduled
 * for running after a gdb exception.
 */
void
exception_restore_context(cpu_context_t *ctx)
{
    bcopy(&ctx->sc, &r4k_context_space, sizeof(r4k_context_t));
}

/*
 * gdb_mark_frame_unused:
 * Mark a frame as unused
 */
void
gdb_mark_frame_unused(debug_context *ctx)
{
    ctx->cpu_ctx.used = 0;
}

/*
 * gdb_frame_unused:
 * Return TRUE if frame is unused, else return FALSE
 */
boolean
gdb_frame_unused(debug_context *ctx)
{
    if ( ctx->cpu_ctx.used == 0 ) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * exception_interrupt - Return TRUE if exception happened with interrupts off
 */

boolean
exception_interrupt (void)
{
    if ((r4k_context_space.sreg & CAUSE_IP23456) != 
	(r4k_level_table[0].r4600_statusreg & CAUSE_IP23456))
      return(TRUE);		/* Interrupts were locked out */

    return(FALSE);		/* Interrupts were enabled */
}

/*
 * copy_fork_debug - Copy fork context into debugging context
 *
 * This routine is used to set up debugging information about a process
 * which did not stop in the exception handler. We copy as much information
 * as we can from the saved registers and process stack area.
 */

void 
copy_fork_debug (debug_context *ctx, sprocess *fork)
{
    ctx->cpu_ctx.sc.regs[REG_INDEX(4)] = forkx->regs[0];	/* a0  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(5)] = forkx->regs[1];	/* a1  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(16)] = forkx->regs[2];	/* s0  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(17)] = forkx->regs[3];	/* s1  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(18)] = forkx->regs[4];	/* s2  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(19)] = forkx->regs[5];	/* s3  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(20)] = forkx->regs[6];	/* s4  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(21)] = forkx->regs[7];	/* s5  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(22)] = forkx->regs[8];	/* s6  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(23)] = forkx->regs[9];	/* s7  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(24)] = forkx->regs[10];	/* s8  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(29)] = forkx->regs[11];	/* sp  */
    ctx->cpu_ctx.sc.regs[REG_INDEX(31)] = forkx->regs[13];	/* ra  */
    ctx->cpu_ctx.sc.epc = forkx->regs[13];      		/* saved pc */
    ctx->cpu_ctx.sc.err_epc = forkx->regs[13];

    /*
     * We must install a "status" register into the context. Read the
     * present status register.
     */
    ctx->cpu_ctx.sc.sreg = getcp0_sreg();
}

/*
 * gdb_proc_cause_exception:
 * Arrange for a process to take an exception the next time it runs.
 * This is done by setting s/w interrupt 0
 */
void
gdb_proc_cause_exception(sprocess *fork)
{
    fork->ps |= CAUSE_IBIT0;	/* Set Bit 8 in Cause register */
}

/* 
 * computeSignal:
 * Return a Unix like signal number.
 * usually just returns the signal number from the context
 */
int 
computeSignal(int signal)
{
    switch(signal) {
    case SIGWBERR:
	return(SIGSEGV);		/* segment fault */
    case SIGCACHE:
	return(SIGBUS);			/* bus error */
    case SIGRELOAD:
	return(SIGKILL);		/* software forced reload */
    default:
	return(signal);
    }
}

/*
 * gdb_show_exception_info:
 * Print out some information on the exception taken
 */
void
gdb_show_exception_info(debug_context *ctx)
{
    gdb_printf("signal=%d, sr=0x%x, epc=0x%x, err epc=0x%x\n", 
			     ctx->signal,
			     ctx->cpu_ctx.sc.sreg, 
			     ctx->cpu_ctx.sc.epc,
	       		     ctx->cpu_ctx.sc.err_epc);
}

/*
 * gdb_get_cpuregs:
 * Write the cpu registers into the buffer, and return them 
 * to our gdb client
 */
void
gdb_get_cpuregs(debug_context *ctx)
{
    r4k_comm_context_t buf;
    int i;

    for (i=0; i<32; i++) {
	buf.regs[i] = ctx->cpu_ctx.sc.regs[REG_INDEX(i)];
    }

    buf.sr = ctx->cpu_ctx.sc.sreg;
    buf.lo = ctx->cpu_ctx.sc.mdlo;
    buf.hi = ctx->cpu_ctx.sc.mdhi;
    buf.bad = ctx->cpu_ctx.sc.badvaddr;
    buf.cause = ctx->cpu_ctx.sc.cause;
    buf.pc = ctx->cpu_ctx.sc.epc;
    buf.fp = ctx->cpu_ctx.sc.regs[REG_INDEX(29)];  /* stack pointer */

    mem2hex((char*)&buf, ctx->outbuf, COMM_REGBYTES);
}

/*
 * gdb_set_cpuregs:
 * Read the cpu registers from the buffer given to us by
 * our gdb client
 */
void
gdb_set_cpuregs(debug_context *ctx)
{
    r4k_comm_context_t buf;
    int i;

    hex2mem(&ctx->inbuf[1], (char *)&buf, COMM_REGBYTES);

    for (i=0; i<32; i++) {
	ctx->cpu_ctx.sc.regs[REG_INDEX(i)] = buf.regs[i];
	ctx->cpu_ctx.sc.regs[(i*2)] = (buf.regs[i] >> 31) ? -1 : 0;
    }

    ctx->cpu_ctx.sc.sreg = buf.sr;
    ctx->cpu_ctx.sc.mdlo = buf.lo;
    ctx->cpu_ctx.sc.mdlo_hi = (buf.lo >> 31) ? -1 : 0;
    ctx->cpu_ctx.sc.mdhi = buf.hi;
    ctx->cpu_ctx.sc.mdhi_hi = (buf.hi >> 31) ? -1 : 0;
    ctx->cpu_ctx.sc.badvaddr = buf.bad;
    ctx->cpu_ctx.sc.badvaddr_hi = (buf.bad >> 31) ? -1 : 0;
    ctx->cpu_ctx.sc.cause = buf.cause;
    ctx->cpu_ctx.sc.epc = buf.pc;
    ctx->cpu_ctx.sc.epc_hi = (buf.pc >> 31) ? -1 : 0;
}

static void
reenter_gdb(void)
{
    longjmp(&gdb_env, 1);		/* return to gdb */
}

/* 
 * a bus error has occurred, perform a longjmp
 * to return execution and allow handling of the error
 * We have two choices, we can either register a callback routine
 * in the context data structure, which will be returned to after
 * the exception returns and it will call longjmp(). Or we can
 * fix up the status register here and call longjmp() directly.
 * Choose the first method for the time being.
 */
static void
handle_buserror(int signal, int code, r4k_context_t *sc)
{
    sc->epc = (int)reenter_gdb;		/* register post exception callback */
    sc->epc_hi = 0;
}

/*
 * gdb_read_membytes:
 * Read bytes from our memory and return to gdb client
 */
void
gdb_read_membytes(debug_context *ctx)
{
    int addr, length, temp;
    SIG_PF sigsegv_save, sigbus_save;

    /*
     * Lock out interrupts and install bus error handlers
     */
    ctx->savelevel = raise_interrupt_level(ALL_DISABLE);
    sigbus_save = signal(SIGBUS, (SIG_PF)handle_buserror);
    sigsegv_save = signal(SIGSEGV, (SIG_PF)handle_buserror);

    temp = setjmp(&gdb_env);
    if (temp == 0) {
	if (parse2hexnum(&ctx->inbuf[1],&addr,&length)) {
	    mem2hex((char*) addr, ctx->outbuf, length);
	} else {
	    strcpy(ctx->outbuf,"E01");
	    if (gdb_debug)
		gdb_printf("malformed read memory command: %s", ctx->inbuf);
	}     
    } else {
	strcpy(ctx->outbuf,"E03");
	if (gdb_debug)
	    gdb_printf("bus error");
    }     
    
    /*
     * Restore bus error handler and re-allow interrupts
     */
    signal(SIGSEGV, sigsegv_save);
    signal(SIGBUS, sigbus_save);
    reset_interrupt_level(ctx->savelevel);    
}

/*
 * gdb_write_membytes:
 * Write bytes from the gdb client command buffer to our memory
 */
void
gdb_write_membytes(debug_context *ctx)
{
    int addr, length, temp;
    char *ptr;
    SIG_PF sigbus_save, sigsegv_save;

    /*
     * Lock out interrupts and install bus error handlers
     */
    ctx->savelevel = raise_interrupt_level(ALL_DISABLE);
    sigbus_save = signal(SIGBUS, (SIG_PF)handle_buserror);
    sigsegv_save = signal(SIGSEGV, (SIG_PF)handle_buserror);

    temp = setjmp(&gdb_env);
    if (temp == 0) {
	if (parse2hexnum(&ctx->inbuf[1],&addr,&length)) {
	    ptr = strchr(ctx->inbuf,':');
	    ptr += 1; /* point 1 past the colon */
	    
	    /*
	     * We must transform the address from a kuseg address
	     * to a KSeg0 address if it is in the text segment to allow
	     * gdb to write to write protected memory
	     */
	    if (addr >= RAMBASE && addr <= (long)TEXT_END) {
		addr = (addr - RAMBASE + R4K_K0BASE);
	    }
	    hex2mem(ptr, (char*) addr, length);
	    cache_flush();
	    level2_cache_flush();
	    strcpy(ctx->outbuf,"OK");
	} else {
	    strcpy(ctx->outbuf,"E02");
	    if (gdb_debug)
		gdb_printf("malformed write memory command: %s",ctx->inbuf);
	}     
    } else {
	strcpy(ctx->outbuf,"E03");
	if (gdb_debug)
	    gdb_printf("bus error");
    } 
    
    /*
     * Restore bus error handler and re-enable interrupts
     */
    signal(SIGSEGV, sigsegv_save);
    signal(SIGBUS, sigbus_save);
    reset_interrupt_level(ctx->savelevel);
}

/*
 * branch_predict :
 * If EPC register points to a jump instruction, return
 * the destination PC of the jump so the caller can fill
 * in the "stepping" breakpoint. Note, even if the original
 * gdb breakpoint was in the branch delay slot, the EPC
 * register will point to the branch instruction rather than
 * the branch delay instruction
 */
static int
branch_predict (r4k_context_t *sc)
{
    union instr_r4k *op = (union instr_r4k *)sc->epc;

    switch ( op->i_t.op ) {

    case OPCODE_SPECIAL:
	
	switch ( op->r_t.func ) {
	    
	case OPCODE_JALR:
	case OPCODE_JR:
	    return(r4k_get_cpu_reg(sc, op->r_t.rs));

	default:
	    return(sc->epc + 4);
	}
	break;

    case OPCODE_REGIMM:

	switch ( op->r_t.rt ) {

	case OPCODE_BGEZAL:
	case OPCODE_BGEZALL:
	case OPCODE_BGEZ:
	case OPCODE_BGEZL:
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) < 0)
		return(sc->epc + 8);
	    else
		return(sc->epc + (4 + (op->i_t.s_imd << 2)));

	case OPCODE_BLTZAL:
	case OPCODE_BLTZALL:
	case OPCODE_BLTZ:
	case OPCODE_BLTZL:
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) >= 0) 
		return(sc->epc + 8);
	    else
		return(sc->epc + (4 + (op->i_t.s_imd << 2)));

	default:
	    return(sc->epc + 4);
	}
	break;

    case OPCODE_JAL:
    case OPCODE_J:
	return((sc->epc & R4K_HI_ADDR_MASK) + (op->j_t.target << 2));
	
    case OPCODE_BEQ:
    case OPCODE_BEQL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) != r4k_get_cpu_reg(sc, op->i_t.rt))
	    return(sc->epc + 8);
	else
	    return(sc->epc + (4 + (op->i_t.s_imd << 2)));

    case OPCODE_BGTZ:
    case OPCODE_BGTZL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) <= 0)
	    return(sc->epc + 8);
	else
	    return(sc->epc + (4 + (op->i_t.s_imd << 2)));
	
    case OPCODE_BLEZ:
    case OPCODE_BLEZL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) > 0)
	    return(sc->epc + 8);
	else
	    return(sc->epc + (4 + (op->i_t.s_imd << 2)));

    case OPCODE_BNE:
    case OPCODE_BNEL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) == r4k_get_cpu_reg(sc, op->i_t.rt))
	    return(sc->epc + 8);
	else
	    return(sc->epc + (4 + (op->i_t.s_imd << 2)));

    default:
	return(sc->epc + 4);
    }
}

/*
 * gdb_proc_continue:
 * Restart a process at an optional address
 */
void
gdb_proc_continue(debug_context *ctx, int step)
{
    ulong *addr;

    /* 
     * try to read optional parameter, addr unchanged if no parm
     */
    (void) parsehexnum(&ctx->inbuf[1],&ctx->cpu_ctx.sc.epc);

    /*
     * If we are stepping, we must write a "breakpoint" into 
     * the next instruction and note that we have done this. 
     * The breakpoint handler will check to see if we hit one
     * of these stepping breaks and restore the code
     * appropriately.
     *
     * Note : If the original gdb break point was in the place
     *        of a branch instruction or in the branch delay slot
     *        we must decode the branch and set the "stepping"
     *        breakpoint at the destination of the branch.
     */
    if (step) {
	break_addr = branch_predict(&ctx->cpu_ctx.sc);
	if (break_addr >= RAMBASE && break_addr <= (ulong)TEXT_END) {
	    addr = (ulong *)(break_addr - RAMBASE + R4K_K0BASE);
	    break_opcode = *addr;
	    *addr = OPCODE_BREAK;
	    cache_flush();
	    level2_cache_flush();
	}
    }

    /* 
     * clear the sw interrupt 0 bit
     */
    ctx->cpu_ctx.sc.cause &= ~CAUSE_IBIT0;
}

/*
 * restore_vectors:
 * Restor evectors after running the system. The monitor restores
 * all vectors via setting BEV bit back to bootstrap state.
 */
void
restore_vectors(void)
{
}

/*
 * Initialize the clock tick handler
 */
void
init_clocktick_handler(void)
{
    uchar ie;
    ulong i;

    /*
     * We must initialize our level 7 stack, and then install
     * our own interrupt handler into the exception table in low core.
     * Our heap has been initialized at this point, so we are set.
     *
     * Note: To be completely safe, we'll disable interrupts while
     *       we do this.
     */
    disable_watchdog();
    i = getcp0_sreg();
    ie = i & SREG_IE;
    setcp0_sreg(i & ~SREG_IE);

    createlevel(NMI_LEVEL, r4k_4ms_interrupt, "NMI Interrupt Handler");
    ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_INTERRUPT] = 
	(ulong)r4k_interrupt_handler;

    /*
     * Flush the cache and init count/compare
     */
    cache_flush();
    setcp0_compare(getcp0_count() + R4K_CLOCK_4MS_INCR);

    /*
     * Initialize the watchdog counter to 16ms
     */
    restart_watchdog(16);

    /*
     * re-enable interrupts
     */
    if (ie)
	setcp0_sreg(i | SREG_IE);
}

/*
 * handle_exception:
 * This function is called whenever an exception occurs. The front end
 * has saved the cpu context in r4k_context_space. ERL/EXL have been 
 * de-asserted and interrupts have been re-enabled at Level 7.
 *
 * We will attempt to dispatch the exception to GDB. If GDB handles the
 * exception, it will not return.
 *
 * At this point, we will pass the exception back to the ROM monitor for
 * handling. 
 */

void handle_exception (int signal_no, int code)
{
    ulong pc, *addr;
    int mapped_signal_no;

    /*
     * If we are taking a breakpoint and we have registered the fact that
     * we are doing single stepping and the address corresponds to the
     * single stepping pc, restore the original code
     */
    if (signal_no == SIGTRAP && break_addr == r4k_context_space.epc) {
	addr = (ulong *)(break_addr - RAMBASE + R4K_K0BASE);
	*addr = break_opcode;
	break_addr = 0;
	break_opcode = 0;
	cache_flush();
	level2_cache_flush();
    }

    mapped_signal_no = computeSignal(signal_no);

    /*
     * Give the signal handler an opportunity to handle the exception.
     * If the signal handler processes it, then it will not return.
     */

    signal_exception(mapped_signal_no);	/* Offer the exception */

    /*
     * Next give gdb an opportunity to handle this exception. If gdb
     * handles the exception, then it will not return, but instead
     * resume from the exception itself.
     */

    gdb_exception(mapped_signal_no); /* Offer the exception to gdb */
    
    if (!process_get_boolean(system_crashed)) {

	process_set_boolean(system_crashed, TRUE); /* Prevent re-entrancy */
	GET_TIMESTAMP(crash_msclock);        /* Remember what time it was. */
	/*
	 * Preserve crash context
	 */
	exception_save_context(&crash_context);
	
	/*
	 * See if we are setup to dump a core. Note, interrupts being
	 * enabled are defined as having IP[2..6] enabled IE all 1's
	 */
	if (dumphost.ip_addr &&		/* If we have someone to dump to */
	    dumpfile &&			/* And we have a dump file name */
	    system_running && 		/* And the scheduler is running */
	    !schedflag && 		/* And we were in process state */
	    forkx &&			/* And the process is set up */
	    exception_pid &&		/* And the crash process is OK */
	    				/* And interrupts are on */
 	    ((crash_context.sc.sreg & CAUSE_IP23456) ==
 	     (r4k_level_table[0].r4600_statusreg & CAUSE_IP23456))) {

	    /*
	     * Check to see if the crash context area has been corrupted.
	     * If so, restore it and set a special reason code.
	     */

	    if ((crash_ptr.magic != CRASH_MAGIC) ||
		(CRASH_INFO != &crash_ptr)) {
		crash_ptr.magic = CRASH_MAGIC;
		crash_ptr.version = CRASH_VERSION;
		crash_ptr.reason = CRASH_REASON_CORRUPT;
		crash_ptr.old_context = &crash_context.sc;
		crash_ptr.context = &crash_context;
		crash_ptr.rambase = (void *)RAMBASE;
                crash_ptr.textbase = (void *)TEXTBASE;
                crash_ptr.database = (void *)DATABASE;
                crash_ptr.bssbase  = (void *)BSSBASE;
		CRASH_INFO =  &crash_ptr;
	    } else {

		/* Context not corrupt - just indicate exception */

		crash_ptr.reason = CRASH_REASON_EXCEPTION;
	    }
	    crash_ptr.cpu_vector = signal_no; /* Set vector */
	    if ( crash_context.sc.sreg & SREG_ERL ) {
		pc = crash_context.sc.err_epc;
	    } else {
		pc = crash_context.sc.epc;
	    }
	    ttyprintf(CONTTY, 
		      "\n\nUnexpected exception, CPU signal %d, "
		      "PC = %#x\n", signal_no, pc);
	    ttyprintf(CONTTY, "%s\n", exception_save_traceback());

	    gdb_crash_code = code;		  /* Save crash code   */
	    gdb_crash_sig = signal_no;		  /* Save crash signal */
	    exception_jump(exception_write_core); /* Switch context    */
	}
    }

    /*
     * Return to the ROM Monitor
     */
    r4k_return_to_monitor(signal_no, code, &r4k_context_space);
}

/*
 * gdb_crash_restore:
 * Restore context after a crash
 */
void
gdb_crash_restore(void)
{
    /*
     * Restore the registers, the frame and return to monitor
     */
    exception_restore_context(&crash_context);
    r4k_return_to_monitor(gdb_crash_sig, gdb_crash_code, &r4k_context_space);
}

/*
 * gdb_init_exceptions:
 * Thus routine is called at system startup to set up trapping
 * of all the exceptions that the system image handles.
 */
void
gdb_init_exceptions(void)
{
    ulong *addr, size;
    ulong cal_vector, int_vector;

    /*
     * Firstly, ask the monitor for the address of its exception
     * table. We then copy over the asm. language code for handling
     * normal exceptions, initialize our exception table, install the 
     * monitors interrupt and syscall (maybe trap ??) handler addresses
     * into our table. Flush the caches and set the BEV bit to normal
     */
    addr = mon_exception_addr();
    size = (ulong)exception_code_end - (ulong)exception_code_start;
    bcopy(exception_code_start, (char *)R4K_K0BASE, size);

    /*
     * Copy the monitors "syscall" exception and its interrupt
     * exception vector to our table
     */
    int_vector = addr[CAUSE_INTERRUPT];
    cal_vector = addr[CAUSE_SYSCALL];
    ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_INTERRUPT] = int_vector;
    ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_SYSCALL] = cal_vector;

    /*
     * Initialize the signals.
     */
    r4k_init_signals();

    /*
     * Flush the caches and set the BEV bit to Normal
     */
    cache_flush();
    size = getcp0_sreg();
    setcp0_sreg(size & ~SREG_BEV);

    if ( ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_INTERRUPT] != int_vector) {
        ttyprintf(CONTTY, "error, int_vector got corrupted, %#x\n",
		  ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_INTERRUPT]);
    }

    if ( ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_SYSCALL] != cal_vector) {
        ttyprintf(CONTTY, "error, cal_vector got corrupted, %#x\n",
		  ((ulong *)R4K_EXCEPTION_TABLE)[CAUSE_SYSCALL]);
    }

    /* 
     * Set up the crash information vector
     */
    CRASH_INFO = &crash_ptr;

    /*
     * Setup the fault history pointer and size
     */
    fh_ptr = mon_fault_history_addr();
    fh_bytes = mon_fault_history_bytes();
}

/*
 * abort:
 * Crash the system with a software forced crash. 
 */
void abort (void)
{
    asm volatile (".set noreorder");
    asm volatile ("nop;");
    asm volatile (".word %0" : /* no output */ : "i" (OPCODE_RELOAD));
    asm volatile ("nop");
    asm volatile (".set reorder");
}

/*
 * r4k_level_error:
 * Our raise_interrupt_level/get_interrupt_level mechanism's got corrupted
 */
void
r4k_level_error(ulong sreg, ulong nev_mask, 
		ulong intr_level, ulong call_pc)
{
    ulong sreg_copy = getcp0_sreg();

    disable_watchdog();
    sreg_copy &= ~SREG_IE;
    setcp0_sreg(sreg_copy);
    sreg_copy |= CAUSE_IBIT7;
    setcp0_sreg(sreg_copy);
    sreg_copy &= ~(SREG_EXL|SREG_ERL);
    setcp0_sreg(sreg_copy);

    ttyprintf(CONTTY, "\n%%LEV-1-R4K: Fatal cpu level corruption\n");
    ttyprintf(CONTTY, " sreg: %#x, mask: %#x, lvl: %d, caller: %#x\n",
	      sreg, nev_mask, intr_level, call_pc);
    crashdump(0);
}

/*
 * r4k_nmi_error:
 * We have reentered the NMI handler
 */
void
r4k_nmi_error (ulong nmi_count)
{
    ulong sreg = getcp0_sreg();

    disable_watchdog();
    sreg &= ~SREG_IE;
    setcp0_sreg(sreg);
    sreg |= CAUSE_IBIT7;
    setcp0_sreg(sreg);
    sreg &= ~(SREG_EXL|SREG_ERL);
    setcp0_sreg(sreg);

    ttyprintf(CONTTY, "\n%%NMI-1-R4K: NMI interrupt handler re-entered\n");
    ttyprintf(CONTTY, " nmi_count: %d, clock: %#Tn\n", nmi_count);
    crashdump(0);
}

/*
 * fault_history_purge:
 * Purge the fault history buffer
 */
void fault_history_purge (void)
{
    char *fh;
    int fhsize;

    if (system_loading) {	/* do not purge the history on netboot */
	return;
    }
    fh = fh_ptr;
    fhsize = fh_bytes;
    if ((!fh) || 
	(fh == (char *)-1) ||
	(fhsize <= 0)) {
	return;	     		/* roms are not supporting the fault history */
    }

    /*
     * Null terminate the fault history
     */
    *fh = 0;
}

/*
 * fault_history_display:
 * Display the fault history buffer
 */
void fault_history_display (void)
{
    char *fh;
    int fhsize;

    fh = fh_ptr;
    fhsize = fh_bytes;
    if ((!fh) || 
	(fh == (char *)-1) ||
	(fhsize <= 0)) {
	return;	     		/* roms are not supporting the fault history */
    }

    if (strlen(fh) <= 0) {
	return;			/* empty fault history buffer */
    }
    printf("\nFault History Buffer:%s\n", fh);
}

/*
 * fault_history_format:
 * Format the fault history buffer
 */
static void fault_history_format (int sig, int code, r4k_context_t *sc)
{
    char *fh;
    int fhsize, bytes, i;
    sys_timestamp time;

    fh = fh_ptr;
    fhsize = fh_bytes;
    if ((!fh) || 
	(fh == (char *)-1) ||
	(fhsize <= 0)) {
	return;		     	/* roms are not supporting the fault history */
    }

    /*
     * Write version
     */
    if (oemversion) {
	bytes = strlen(oemversion);
	if (fhsize <= bytes) {
	    *fh = 0;
	    return;
	}
	bytes = sprintf(fh, "\n%s", oemversion);
	fhsize -= bytes;
	fh += bytes;
    }
	
    /*
     * Write signal, code and uptime
     */
    if (fhsize < FH_LINE_SIZE) {
	*fh = 0;
	return;
    }
    bytes = sprintf(fh, "\nSignal = %d, Code = %#x, Uptime ", sig, code);
    fhsize -= bytes;
    fh += bytes;
    time.u.value = 1;
    bytes = sprintf(fh, "%TE", time);
    fhsize -= bytes;
    fh += bytes;
    
    /*
     * Save CPU context, normal registers, EPC, SREG and Cause
     */
    if (sc) {
	for (i=0; i<8; i++) {
	    if (fhsize < FH_LINE_SIZE) {
		*fh = 0;
		return;
	    }
	    bytes = sprintf(fh,
			    "\n%s : %08x, %s : %08x, %s : %08x, %s : %08x",
			    r4k_reg_name[(i*4)],   sc->regs[(i*8)+1],
			    r4k_reg_name[(i*4)+1], sc->regs[(i*8)+3],
			    r4k_reg_name[(i*4)+2], sc->regs[(i*8)+5],
			    r4k_reg_name[(i*4)+3], sc->regs[(i*8)+7]);
	    fhsize -= bytes;
	    fh += bytes;
	}
	if (fhsize < FH_LINE_SIZE) {
	    *fh = 0;
	    return;
	}
	bytes = sprintf(fh, "\nEPC : %08x, SREG : %08x, Cause : %08x",
			sc->epc, sc->sreg, sc->cause);
	fhsize -= bytes;
	fh += bytes;
    }
    /*
     * save parity_error_buffer contents as well, if any
     */
    if (parity_error_buffer[0] != '\0') {
        if (fhsize < strlen(parity_error_buffer)) {
            *fh = 0;
            return;
        }
        bytes = sprintf(fh, "%s", parity_error_buffer);
        fhsize -= bytes;
        fh += bytes;
    }
    /*
     * Call platform dependent routine to pick up platform
     * specific information.
     */
    *fh = 0;
    platform_fault_history(sig, code, sc, fh, fhsize);
}

/*
 * r4k_return_to_monitor:
 * Return to the monitor cleanly
 */
void
r4k_return_to_monitor (int sig, int code, r4k_context_t *sc)
{
    ulong sreg;
    int dog;

    /*
     * We return through an EMT call.  To allow the monitor
     * to come back here, we will ensure that EXL and ERL are
     * both negated (allowing EPC to get update when the syscall
     * exception is taken in emt_call()). We will also disable
     * the watchdog and interrupts so that the box does not get
     * hosed 16ms after getting to the monitor's command line.
     */
    sreg = getcp0_sreg();			       /* get sreg 	   */
    setcp0_sreg(sreg & ~SREG_IE);		       /* deassert IE	   */
    dog = disable_watchdog();			       /* disable the dog  */
    setcp0_sreg(sreg & ~(SREG_ERL|SREG_EXL|SREG_IE));  /* deassert EXL,ERL */

    /*
     * Fill in the fault history if it is supported
     */
    fault_history_format(sig, code, sc);

    /*
     * Go back to the monitor
     */
    mon_cpu_exception(sig, code, sc);

    /*
     * If the monitor "continues" with the image, we come back here.
     * We must re-initialize count/compare so that we start getting
     * 4ms interrupts, as opposed to waiting 81 seconds for the first
     * one (which results in a watchdog timeout)
     */
    restart_count_compare(dog);
    setcp0_sreg(sreg);				/* restore status reg	*/
}

/*
 * r4k_unused_handler:
 * We took an unsed interrupt. Print a message to that effect
 * and return to the monitor with a SIGINT.
 */
void
r4k_unused_handler (int vector, ulong sreg)
{
    ttyprintf(CONTTY, "\n%%INT-1-FATAL: Unused interrupt handler called\n");
    ttyprintf(CONTTY, " vector: 0x%02x, sreg: %#x\n", vector, sreg);
    reg_invoke_flush_logging();
    r4k_return_to_monitor(SIGINT, 0, 0);
}

/*
 * exception_fault_addr - Return the fault address
 *
 * This routine returns the fault address from the current exception.
 * We return the bad virtual address stored in the context structure.
 */

void *exception_fault_addr (void)
{
    return((void *)(r4k_context_space.badvaddr));
}
    
/*
 * generate stack trace to buffer
 */
char *exception_save_traceback (void)
{
    int ix, nframes;
    ulong *frame, *pc;
    char *buf;
    static char buffer[TRACEBACK_SIZE];

    buf = buffer;
    nframes = LOG_TRACE_MAX;
    frame = (ulong*)r4k_context_space.regs[REG_INDEX(R4K_SP_REG)];
    pc = (ulong*)r4k_context_space.epc;

    buf += sprintf(buf, "%s%x ", traceback_prefix, pc);
    for (ix = 0; ix < nframes; ++ix) {
	frame = trace_caller(frame, &pc);
	if (frame == NULL) {
	    if (ix != 0)
		break;

	    /* Oops, started in a stub function. */
	    frame = (ulong*)r4k_context_space.regs[REG_INDEX(R4K_SP_REG)];
	    pc = (ulong*)r4k_context_space.regs[REG_INDEX(R4K_RA_REG)];
	}
	buf += sprintf(buf, "%x ", pc);
    }
    *--buf = '\0';		/* dump trailing space */
    return(buffer);
}
/* End of file */
