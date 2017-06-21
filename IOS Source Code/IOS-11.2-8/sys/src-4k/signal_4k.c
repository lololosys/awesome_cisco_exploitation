/* $Id: signal_4k.c,v 3.2.56.2 1996/03/29 20:04:37 mbeesley Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/signal_4k.c,v $
 *------------------------------------------------------------------
 * signal_4k.c - Support for signals on R4K
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: signal_4k.c,v $
 * Revision 3.2.56.2  1996/03/29  20:04:37  mbeesley
 * CSCdi52977: Some r4k implementations handle parity exceptions
 *             incorrectly. Fix them.
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/03/18  21:57:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:43:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  17:50:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:40:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:21:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "exception.h"
#include "../os/gdb.h"
#include "ttysrv.h"
#include "signal_4k.h"
#include "../dev/monitor1.h"

/*
 * r4k_cause_sig_map:
 * Maps an R4K Cause code to a signal number
 */
ulong r4k_cause_sig_map[] = {
    SIGINT,	/* CAUSE_INTERRUPT, should never happen	*/
    SIGSEGV,    /* CAUSE_TLB_MOD			*/
    SIGSEGV,	/* CAUSE_TLB_LOAD			*/
    SIGSEGV,	/* CAUSE_TLB_SAVE			*/
    SIGBUS,	/* CAUSE_ADDR_LOAD			*/
    SIGBUS,	/* CAUSE_ADDR_SAVE			*/
    SIGBUS,	/* CAUSE_BUS_INSTR			*/
    SIGBUS,	/* CAUSE_BUS_DATA			*/
    SIGKILL,	/* CAUSE_SYSCALL, should never happen	*/
    SIGTRAP,	/* CAUSE_BP				*/
    SIGILL,	/* CAUSE_ILLOP				*/
    SIGFPE,	/* CAUSE_CP_UNUSABLE			*/
    SIGTRAP,	/* CAUSE_OVFLW				*/
    SIGTRAP,	/* CAUSE_TRAP				*/
    SIGKILL,	/* CAUSE_VIRT_COHERENCY_INSTR		*/
    SIGFPE,	/* CAUSE_FPE				*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* CAUSE_WATCH, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL,	/* Reserved, should never happen	*/
    SIGKILL	/* CAUSE_VIRT_COHERENCY_DATA		*/
    };

/*
 * Buffer to hold cache parity exception error message, that gets
 * stored in NVRAM, before returning to rom monitor.
 */
char parity_error_buffer[256];

/*
 * sigfunc_table:
 * Make this .data as it is initialized before our .bss
 */
static SIG_PF sigfunc_table[MAX_SIGNAL_NUMBER] = {};

static boolean r4k_expect_bus_error = FALSE;
static volatile int r4k_bus_error_count = 0;

/*
 * r4k_signal_handler:
 * This is the entry point for the exception handling
 * on an R4K based machine. The context of the processor has
 * been saved in the context structure, the stack has been
 * moved if required. ERL/EXL have been cleared and Level 7
 * interrupt masks have been installed (with interrupts enabled).
 */
void
r4k_signal_handler(int signal_no, int code, r4k_context_t *c)
{
    SIG_PF func;

    /*
     * We come here to handle all exceptions and to handle the
     * interrupts that are really exceptions (write bus errors).
     */
    if (signal_no == SIGWBERR) {
	write_berr_interrupt(code, c);
    } else if (signal_no == SIGTRAP) {

	/*
	 * We can receive a SIGTRAP for a variety of reasons.
	 * 	1) Arithmetic overflow
	 * 	2) Breakpoint (from gdb, rom monitor or single stepping)
	 * 	3) A Trap instruction (never used)
	 * 	4) A software forced reload
	 *
	 * When gdb sets a breakpoint, the instruction is 'break 5'. 
	 * Both the monitor and single stepping uses 'break 0'.
	 * Software forced reload uses 'break 10'. This is guaranteed
	 * to *not* be in a branch delay slot and have a nop as the
	 * next instruction to allow the monitor to continue after
	 * a software forced reload (see abort() in os/gdb_4k.c).
	 */
	if (!(c->cause & CAUSE_BD_SLOT) &&
	    (*(ulong *)(c->epc) == OPCODE_RELOAD)) {
	    signal_no = SIGRELOAD;
	    c->epc += 4;
	}
    }

    /*
     * If a signal handler has been registered for this
     * signal, call it. Else, enter the gdb debugger
     */
    if (signal_no >= MAX_SIGNAL_NUMBER) {
	func = SIG_DFL;
    } else {
	func = sigfunc_table[signal_no];
    }

    switch ((int)func) {

    case (int)SIG_IGN:
	break;

    case (int)SIG_DFL:
	handle_exception(signal_no, code);
	break;

    default:
	/*
	 * A signal handler has been registered for this signal.
	 */
	(*func)(signal_no, code, c);
	break;
    }
}

/*
 * figure out whether the pointed-at instruction is a jump or branch
 * by decoding the six-bit opcode (see the table in the MIPS CPU manual)
 */
static boolean
is_jump (union instr_r4k* op)
{
    uchar op_row, op_col;
 
    op_row =  op->op_code >> 29;                /* op[31..29] */
    op_col = (op->op_code >> 26) & 0x7;         /* op[28..26] */
    switch (op_row) {
      case 0:
	if (op_col >= 2)
	    return TRUE;
	break;
 
      case 2:
	if (op_col >= 4)
	    return TRUE;
	break;
    }
    return FALSE;
}

static void
r4k_cacherr (int signal, int code, r4k_context_t *sc)
{
    ulong cp0_cacherr, addr;
    union instr_r4k *op;

    /*
     * In the guts of asm_4k.S, the ERL bit in the status register
     * got turned off very early to allow the kuseg memory space to
     * remain available thru the cached, mapped view (this is required
     * on boxes like Predator, Mars and VIP where the kuseg space
     * does not map 1:1 onto the physical address space so mapping is
     * always required). Consequently, we will set this bit in the 
     * saved context, to keep all the error reporting accurate.
     */
    sc->sreg |= SREG_ERL;

    cp0_cacherr = code;
    /* 
     * get the address that got parity error.
     */
    if (cp0_cacherr & CACHE_ERR_ER) { /* data cache? */
 
        op = (union instr_r4k*) sc->err_epc;
        if (is_jump(op))
            ++op;                       /* point to branch delay slot */
        addr = r4k_get_cpu_reg(sc, op->i_t.rs) + op->i_t.s_imd;
    } 
    else {
	addr = sc->err_epc;		/* instn cache addr */
    }

    sprintf(parity_error_buffer, 
	    "\nError: %s %s cache, fields: %s%s,%s%s%s"
	    "\nvirtual addr 0x%08x, physical addr(21:3) 0x%06x,"
            " vAddr(14:12) 0x%04x",
	    (cp0_cacherr & CACHE_ERR_EC) ? "(rsvd)"	: "primary",
	    (cp0_cacherr & CACHE_ERR_ER) ? "data"	: "instr",
	    (cp0_cacherr & CACHE_ERR_ED) ? "data"	: "",
	    (cp0_cacherr & CACHE_ERR_ET) ? " tag"	: "",
	    (cp0_cacherr & CACHE_ERR_ES) ? " miss"	: "",
	    (cp0_cacherr & CACHE_ERR_EE) ? " SysAD"	: "",
	    (cp0_cacherr & CACHE_ERR_EB) ? " EB"	: "",
            addr, (cp0_cacherr & CACHE_ERR_SLDX),
	    (cp0_cacherr & CACHE_ERR_PLDX) << 12);
    ttyprintf(CONTTY, "%s", parity_error_buffer);
    handle_exception(signal, code);
}

/*
 * r4k_init_signals:
 * Initialize the signal callback array
 */
void
r4k_init_signals(void)
{
    int i;

    for (i=0; i<MAX_SIGNAL_NUMBER; i++) {
	sigfunc_table[i] = SIG_DFL;
    }
    sigfunc_table[SIGCACHE] = (SIG_PF) r4k_cacherr;
}

/*
 * trap_bus_errors:
 * We negated the flag, thus informing our exception handlers that
 * bus errors are for real and are unexpected
 */
void
trap_bus_errors(void)
{
    r4k_expect_bus_error = FALSE;
}

/*
 * pass_bus_errors:
 * We set the flag, thus informing our exception handlers
 * that exceptions are expected
 */
void
pass_bus_errors(void)
{
    r4k_expect_bus_error = TRUE;
}

/*
 * handle_buserrors:
 * Used to trap exceptions for touch_device
 */
static int
handle_buserrors(int sig, int code, r4k_context_t *sc)
{
    /*
     * If we came here but we are not expecting bus errors or
     * if the instruction was in the branch delay slot,
     * try and enter the gdb debugger
     */
    if (!r4k_expect_bus_error || (sc->cause & CAUSE_BD_SLOT)) {
	handle_exception(sig, code);
    } else {
	r4k_bus_error_count += 1;
	sc->epc += 4;
    }
    return(0);
}

/*
 * touch_byte:
 * Read an address as a uchar for touch_device
 */
static int
touch_byte(void *addr)
{
    int data;

    asm volatile (".set noreorder");		/* .set noreorder	*/
    asm volatile ("lbu %0,0(%1); nop; nop; nop; nop"	/* lbu t0,0(a0) */
	: "=&r" (data)				/* output (data)        */
	: "r" (addr));	  			/* input (addr)		*/
    asm volatile (".set reorder");		/* .set reorder      	*/
    return(data);
}

/*
 * touch_short:
 * Read an address as a ushort for touch_device
 */
static int
touch_short(void *addr)
{
    int data;

    asm volatile (".set noreorder");		/* .set noreorder	*/
    asm volatile ("lhu %0,0(%1); nop; nop; nop; nop"	/* lhu t0,0(a0)	*/
	: "=&r" (data)				/* output (data)	*/
	: "r" (addr));  			/* input (addr)		*/
    asm volatile (".set reorder");		/* .set reorder      	*/
    return(data);
}

/*
 * signal:
 * Handle setting up signals
 */
SIG_PF
signal(int sig_no, SIG_PF handler)
{
    SIG_PF ptr;

    if (sig_no < 0 || sig_no >= MAX_SIGNAL_NUMBER) {
	return(SIG_DFL);
    }
    ptr = sigfunc_table[sig_no];
    sigfunc_table[sig_no] = handler;
    return(ptr);
}

/*
 * touch_device:
 * R4K implementation of touch_device. Determines if an address exists
 * by reading it as a byte, and if that fails read it as a short if
 * the address is on an even boundary.
 * Return FALSE if address does not exist, else TRUE
 */
boolean 
touch_device (void *addr)
{
    int bus_error_count;
    leveltype level;
    SIG_PF sigbus_save, sigsegv_save;

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * setup the variables
     */
    sigbus_save = signal(SIGBUS, (SIG_PF)handle_buserrors);
    sigsegv_save = signal(SIGSEGV, (SIG_PF)handle_buserrors);
    pass_bus_errors();
    r4k_bus_error_count = 0;

    /*
     * Do byte read
     */
    touch_byte(addr);

    /*
     * If we had bus errors, check short access
     */
    if (r4k_bus_error_count && !((ulong)addr & 0x01)) {
	r4k_bus_error_count = 0;
	touch_short(addr);
    }

    trap_bus_errors();
    signal(SIGBUS, sigbus_save);
    signal(SIGSEGV, sigsegv_save);
    bus_error_count = r4k_bus_error_count;

    reset_interrupt_level(level);

    if (bus_error_count) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/* End of file */
