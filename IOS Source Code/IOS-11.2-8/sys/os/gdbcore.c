/* $Id: gdbcore.c,v 3.3.20.2 1996/07/10 22:22:10 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/gdbcore.c,v $
 *------------------------------------------------------------------
 * gdbcore.c  --  GDB core routines
 *  
 * October 1, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdbcore.c,v $
 * Revision 3.3.20.2  1996/07/10  22:22:10  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  21:29:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:09:39  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:18:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:39:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:07:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:48:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "config.h"
#include "gdb.h"
#include "ttysrv.h"
#include "logger.h"
#include "sched_private.h"
#include "sys_registry.h"
#include "exception.h"
#include "sum.h"
#include "../dev/monitor1.h"
#include "../../boot/cpu.h"

/* 
 * Local data
 */
boolean kernel_debugging = FALSE; /* TRUE when debugging kernel */
debug_context gdb_context = {};   /* Debugger context for kernel debugging */

ulong spurious_threshold = 0L;	/* Spurious interrupt thresohld */
watched_boolean *system_crashed;       /* True if a crash has occured */
sys_timestamp crash_msclock;           /* msclock at the time of the crash */
int exception_pid;		       /* PID of crashwriter process */
char *traceback_prefix = traceback_prefix_string;
char *dumpfile;			/* Default dumpfile name */
char *sl_dumpfile;              /* Default sl_dumpfile name */
addrtype dumphost = { 0, 0L, { 0 } };  /* Host for core dump */
addrtype sl_dumphost = { 0, 0L, { 0 } };  /* Host for slave core dump */

/*
 * gdb_flush - flush any pending output
 *
 * This routine is called to flush any pending output. Since we are using
 * polled I/O, we don't need to do anything.
 */

void gdb_flush (void)
{
}

/*
 * gdb_kill_debug - Try to kill a process being debugged
 *
 * This routine is called by the scheduler when a process is marked
 * to be deleted and it is also marked for debugging. We set the
 * process as crashed, and set up the context with a special vector
 * code which indicates terminated process.
 */

boolean gdb_kill_debug (sprocess *fork)
{
    debug_context *ctx = fork->debugger;

    if (!fork->crashed)	{		/* If we haven't been here */
	copy_fork_debug(ctx, fork);	/* Copy fork saved registers */
	fork->crashed = TRUE;		/* Mark the fork as crashed */
	ctx->signal = SIGTERM;		/* Set a signal value */
    }
    return(TRUE);			/* Don't kill the process */
}

/*
 * debug_suspend - Suspend a process for debugging
 *
 * Control is transferred here on the process stack when a process has
 * an exception which is eligable for debugging. We will mark the fork
 * as crashed, move the context into the debugging context for this
 * fork, and suspend the process.
 *
 * When suspend() resumes, this means that the debugging process has
 * cleared our crashed flag, enabling us to be scheduled. We will copy
 * the debugging context back to the process, and return from the
 * exception.
 */

void debug_suspend (void)
{
    debug_context *ctx = forkx->debugger;

    forkx->crashed = TRUE;

    exception_save_context(&ctx->cpu_ctx);	/* Save the context */
    ctx->signal = gdb_context.signal; /* Save the signal number */

    set_interrupt_level(ALL_ENABLE);	/* Allow interrupts again */
    blocking_disabled = FALSE;	/* Blocking is OK */
    if (blocking_disabled)
	last_proc_name = forkx->name;
    suspend();			/* Go to sleep until someone fixes us */

    raise_interrupt_level(ALL_DISABLE);	/* Stop the world again */
    exception_restore_context(&ctx->cpu_ctx);	/* Restore the context */
    gdb_mark_frame_unused(ctx); /* Mark context block as free */

    return_from_exception();
}

/*
 * gdb_exception - Exception handler for debugging kernel
 *
 * This routine is called by the exception handler when an exception
 * occurs. If the exception is to be trapped by a process debugging
 * another process, we switch back to the process stack so that we
 * can properly suspend the process. Otherwise, we check to see if
 * the kernel debugger is enabled or if this was a trace trap. If so,
 * we will invoke the debugger on the exception stack. Otherwise, we
 * will signal back to the system exception handler to process this.
 */

void gdb_exception (int signal)
{	
    /*
     * If this is SIGINT, do not enter the debugger. It may be useful
     * to add a flag which indicates that SIGINT (break) should be
     * debugguggable. This would allow debugging places where console
     * interrupts were locked out and the user needed to get debugger
     * attention.
     */

    if (signal == SIGINT)
      return;			/* Nothing to do here */

    /*
     * Remember the signal number
     */

     gdb_context.signal = signal;

    /* 
     * If we are running a process, we might be able to debug it
     */
    if ((!exception_interrupt()) &&
	system_running &&
	!schedflag &&
	forkx && forkx->debugger) {
	exception_jump(debug_suspend); /* Switch context */
    }
      
    if (kernel_debugging) {

	/* 
	 * Move the register and frame context into the work area
	 */

	exception_save_context(&gdb_context.cpu_ctx);

	/*
	 * Enter the debugger
	 */
	gdb_interface(&gdb_context, FALSE);

	/* 
	 * Move the registers and frame context back
	 */
	exception_restore_context(&gdb_context.cpu_ctx);

	return_from_exception();
    }
}

/*
 * gdb_init - Initialization
 */

void gdb_init_core (void)
{
    gdb_context.getchar = console_getc;
    gdb_context.putchar = console_putc;
    gdb_context.flush = gdb_flush;
}

/* 
 * This function is called at system startup to set up trapping for the
 * exceptions that we trap.
 */

void init_exception_handler (void)
{
    gdb_init_exceptions();

    /* 
     * Initialize GDB
     */
    gdb_init_core();
}

/*
 * gdb_kernel_enable - Enable kernel debugging
 *
 * This routine is used to enable kernel debugging.
 */

void gdb_kernel_enable (void)
{
    kernel_debugging = TRUE;

    /* Don't checksum code--we might plant breakpoints. */

    defeat_sum();	
}


/* 
 * This function will generate a breakpoint exception.  It is used at the
 * beginning of a program to sync up with a debugger and can be used
 * otherwise as a quick means to stop program execution and "break" into
 * the debugger.
 */
   
boolean breakpoint (void)
{
    if (kernel_debugging ||
	(system_running && !schedflag && forkx->debugger && !get_interrupt_level())) {
	BREAKPOINT();
	return(TRUE);
    }

    return(FALSE);
}

int gdb_printf (const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    return(_doprnt(NULL, CONTTY, fmt, ap, FALSE));
    va_end(ap);
}


/*
 * If we are going to attempt to write a core file, we return here on
 * the stack of the process that crashed. This is necessary so that
 * we can suspend this process to run the scheduler and write the core
 * file.
 */

void exception_write_core (void)
{
    forkx->crashed = TRUE;		/* Mark the process as crashed */
    if (!forkx->corrupt)
	list_move(&procq_dead, &forkx->sched_list);
    sched_mark_router_crashing();	/* don't run some processes */
    set_interrupt_level(ALL_ENABLE);	/* Enable interrupts */
    if (blocking_disabled)
	last_proc_name = forkx->name;
    suspend();				/* Suspend the process */
    while (TRUE)			/* Should never come back! */
      ;
}

/*
 * rp_csc4_parity_error_test()
 *   This function reads all of DRAM  to test if a parity error
 *   occured. Please see crashdump() 
 */
 
void rp_csc4_parity_error_test(void)
{
    uint old_cacr = 0;
    uint nwords;
    volatile uint *current;
    uint data;
    
    /*
     * Disable Instruction and Data Caches
     */
    
    ttyprintf(CONTTY,"\n\nTesting Memory For parity Error...");
    old_cacr = mon_setcacr(0);
    asm(".word 0xf4f8"); /* 68040 CPUSH instruction to  */
    /* flush and invalidate caches */
     
    nwords = ((uint)mon_getmemsize() - (uint)RAMBASE) /
 	sizeof(uint);
    current = (volatile uint *)RAMBASE;
    
    while (nwords--) {
 	data = *current;
 	current ++; 
    }
    ttyprintf(CONTTY, "\nNo Parity Error Occurred.");
    (void)mon_setcacr(old_cacr);
}

/*
 * crashdump - Intentionally crash the sytem and take a crash dump.
 * If there is a crash in progress, just return.  Optionally pause so that
 * the logger can get the error message out.  
 */

void crashdump (int pause)
{
    static boolean called = FALSE;
    extern int cpu_type;

    if (called) {
	return;
    }
    called = TRUE;

    breakpoint();		/* Go into the debugger, if enabled */

    ttyprintf(CONTTY, "\n\n%%Software-forced reload\n");

    /* 
     * Interim Fix for CSCdi52697. A undetected parity error can occur
     * on an CSC/4 or a RP previous to revs: 
     *  73-0877-08 REV:-A0   ROUTE PROCESSOR
     *  73-1247-03 REV:-A0   ROUTE PROCESSOR,64MB
     * It exists in all revs of CSC/4's and there will not
     * be a hardware fix for the CSC/4.
     * 
     * With cache enabled, it is possible to "miss" a
     * parity error. As an interim fix we can test Dram for a
     * parity error through a uncached read of all of Dram. 
     * If a parity error occurs, this will be reported through
     * a bus error.
     */
    
    if ((cpu_type == CPU_CSC4) || (cpu_type == CPU_RP1)) {       
 	
 	/*
 	 * This is a 68040 processor. Test for Parity error.
 	 */
 	
 	rp_csc4_parity_error_test();
    }
    
    (void) set_interrupt_level(ALL_ENABLE);
    blocking_disabled = FALSE;

    /*
     * Dally a while if asked and if not called in scheduler context
     */
    if (pause && !schedflag) {
	ttyprintf(CONTTY, "\nPreparing to dump core...");
	process_sleep_for(pause*ONESEC);
    }

    /*
     * Invoke logger_flush only if "exception dump" is not configured.
     * Otherwise, leave it to the exception_crash_process to do it,
     * so that all buginf messages during the core dump operation
     * can be flushed to the console.
     */
    if (!exception_pid) 
        reg_invoke_flush_logging();

    crashpoint();
    called = FALSE;
}
