/* $Id: gdb_68.c,v 3.1.6.2 1996/05/09 14:40:51 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68/gdb_68.c,v $
 *------------------------------------------------------------------
 * gdb_68.c -- M68K specific gdb routines
 *  
 * October 1, 1990, Kevin Paul Herbert
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_68.c,v $
 * Revision 3.1.6.2  1996/05/09  14:40:51  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.6.1.18.1  1996/04/27  07:25:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.6.1.8.1  1996/04/08  02:08:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.6.1  1996/03/18  21:58:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.20.1  1996/03/22  09:41:41  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1  1996/03/09  00:56:04  hampton
 * Migrate platform specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.5  1996/02/01  06:07:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/11/30  00:02:44  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.3  1995/11/17  18:48:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6.18.1  1996/02/03  07:08:13  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.6  1995/08/25  11:44:43  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.5.28.2  1995/09/20  03:29:30  rlowe
 * Allow, but do not require, system timer interrupt vector to be
 * different from NMI vector.  For 68k-related platforms.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.5.28.1  1995/09/11  01:56:54  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.5.12.1  1995/09/08  03:24:07  rlowe
 * Allow gdb to support platforms (such as SynALC) where system timer
 * interrupt is NOT on NMI.
 * Branch: Synalc2_branch
 *
 * Revision 2.5  1995/07/18 06:48:17  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.4  1995/07/17 06:17:39  abaindur
 * CSCdi37171:  Exception handling broken on 68k platforms
 * In cases where we save page 0 and stuff it with an illegal
 * instruction value, restore it back before we go pick a ROM
 * Monitor handler to drop down to after the exception.
 *
 * Revision 2.3  1995/07/07  05:26:40  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.2  1995/06/21 09:24:27  hampton
 * CSCdi36157:  arizona rxboot fails to net/flash boot
 *
 * Revision 2.1  1995/06/07 21:55:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "../os/gdb.h"
#include "ttysrv.h"
#include "../os/sched_private.h"
#include "logger.h"
#include "../dev/monitor1.h"
#include "sys_registry.h"
#include "exception.h"

/*
 * externs
 */
extern int exception_registers[72];
extern Frame exception_frame;
extern void exception_handler_clock(void);
extern void m68k_spurious(void);
extern ulong spurcnt;

/* Local storage */

void (*nmi_handler)(void) = NULL;         /* Pointer to current NMI handler */
static void (**rom_vector)(void) = NULL; /* Saved ROM monitor vector table */

void (*system_vector_table[VECTORCOUNT])(void) = {}; /* Vector table */

static cpu_context_t crash_context; /* Saved context at crash time */

static crashinfo crash_ptr = { CRASH_MAGIC,
			  CRASH_VERSION,
			  CRASH_REASON_NOTCRASHED,
			  -1,
			  &crash_context.registers,
			  (void *)RAMBASE,
                          (void *)TEXTBASE,
                          (void *)DATABASE,
                          (void *)BSSBASE,
			  &crash_context
			};
/*
 * Forward declaration
 */

static void exception_rom_monitor(void);

/*
 * exception_save_context:
 * Used to save registers/frame when a process takes an exception that
 * is elligible for debugging. Save registers and Frame.
 */
void
exception_save_context(cpu_context_t *ctx)
{
    bcopy(exception_registers, ctx->registers, REGBYTES);
    bcopy(&exception_frame, &ctx->saveFrame, sizeof (Frame));  
}

/*
 * exception_restore_context:
 * Used to restore registers/frame when a process is re-scheduled
 * for running after a gdb exception.
 */
void
exception_restore_context(cpu_context_t *ctx)
{
    bcopy(ctx->registers, exception_registers, REGBYTES); 
    bcopy(&ctx->saveFrame, &exception_frame, sizeof (Frame));
    exception_frame.sr = exception_registers[ PS ];	       /* SR	    */
    exception_frame.pc = (void *)exception_registers[ PC ];    /* PC	    */
}

/*
 * gdb_mark_frame_unused:
 * Mark a frame as unused
 */
void
gdb_mark_frame_unused(debug_context *ctx)
{
    ctx->cpu_ctx.saveFrame.frameSize = 0;
}

/*
 * gdb_frame_unused:
 * Return TRUE if frame is unused, else return FALSE
 */
boolean
gdb_frame_unused(debug_context *ctx)
{
    if ( ctx->cpu_ctx.saveFrame.frameSize == 0 ) {
	return(TRUE);
    } else {
	return(FALSE);
    }
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
    ulong *sp;

    ctx->cpu_ctx.registers[2] = fork->regs[0];	/* D2 */
    ctx->cpu_ctx.registers[3] = fork->regs[1];	/* D3 */
    ctx->cpu_ctx.registers[4] = fork->regs[2];	/* D4 */
    ctx->cpu_ctx.registers[5] = fork->regs[3];	/* D5 */
    ctx->cpu_ctx.registers[6] = fork->regs[4];	/* D6 */
    ctx->cpu_ctx.registers[7] = fork->regs[5];	/* D7 */
    ctx->cpu_ctx.registers[10] = fork->regs[6]; /* A2 */
    ctx->cpu_ctx.registers[11] = fork->regs[7];	/* A3 */
    ctx->cpu_ctx.registers[12] = fork->regs[8];	/* A4 */
    ctx->cpu_ctx.registers[13] = fork->regs[9];	/* A5 */

    sp = fork->savedsp;				/* Get saved stack pointer */
    ctx->cpu_ctx.registers[14] = sp[0];		/* Post back the saved SP */
    ctx->cpu_ctx.registers[17] = sp[1];		/* And the saved PC */
}

/*
 * gdb_proc_cause_exception:
 * Arrange for a process to take an exception the next time it runs
 */
void
gdb_proc_cause_exception(sprocess *fork)
{
    fork->ps |= 0x8000;		/* Set the trace bit */
}

/* 
 * computeSignal:
 * Return a Unix like signal number from the 68000 exception number
 */
int 
computeSignal(int exceptionVector)
{
    int sigval;

    switch (exceptionVector) {
    case 0 : sigval = SIGSEGV; break; /* Jump to zero - segv */
    case 2 : sigval = SIGBUS; break; /* bus error           */
    case 3 : sigval = SIGBUS; break; /* address error       */
    case 4 : sigval = SIGILL;  break; /* illegal instruction */
    case 5 : sigval = SIGFPE;  break; /* zero divide         */
    case 6 : sigval = SIGUSR1; break; /* chk instruction     */
    case 7 : sigval = SIGUSR1; break; /* trapv instruction   */
    case 8 : sigval = SIGSEGV; break; /* privilege violation */
    case 9 : sigval = SIGTRAP;  break; /* trace trap          */
    case 10: sigval = SIGILL;  break; /* line 1010 emulator  */
    case 11: sigval = SIGILL;  break; /* line 1111 emulator  */
    case 31: sigval = SIGINT;  break; /* interrupt           */
    case 33: sigval = SIGTRAP;  break; /* breakpoint          */
    case 40: sigval = SIGFPE;  break; /* floating point err  */
    case 48: sigval = SIGFPE;  break; /* floating point err  */
    case 49: sigval = SIGFPE;  break; /* floating point err  */
    case 50: sigval = SIGFPE;  break; /* zero divide         */
    case 51: sigval = SIGFPE;  break; /* underflow           */
    case 52: sigval = SIGFPE;  break; /* operand error       */
    case 53: sigval = SIGFPE;  break; /* overflow            */
    case 54: sigval = SIGFPE;  break; /* NAN                 */
    default: sigval = SIGEMT;         /* "software generated"*/
    }
    return (sigval);
}

/*
 * gdb_show_exception_info:
 * Print out some information on the exception taken
 */
void
gdb_show_exception_info(debug_context *ctx)
{
    gdb_printf("vector=%d, sr=0x%x, pc=0x%x\n", 
			     ctx->cpu_ctx.saveFrame.exceptionVector,
			     ctx->cpu_ctx.registers[ PS ], 
			     ctx->cpu_ctx.registers[ PC ]);
}

/*
 * gdb_get_cpuregs:
 * Write the cpu registers into the buffer, and return them 
 * to our gdb client
 */
void
gdb_get_cpuregs(debug_context *ctx)
{
    mem2hex((char*) ctx->cpu_ctx.registers, ctx->outbuf, NUMREGBYTES);
}

/*
 * gdb_set_cpuregs:
 * Read the cpu registers from the buffer given to us by
 * our gdb client
 */
void
gdb_set_cpuregs(debug_context *ctx)
{
    hex2mem(&ctx->inbuf[1], (char*) ctx->cpu_ctx.registers, NUMREGBYTES);
}

/* 
 * a bus error has occurred, perform a longjmp
 * to return execution and allow handling of the error
 */
static void 
handle_buserror(void)
{
    longjmp(&gdb_env,1);
}

/*
 * gdb_read_membytes:
 * Read bytes from our memory and return to gdb client
 */
void
gdb_read_membytes(debug_context *ctx)
{
    int addr, length;

    /*
     * Lock out interrupts and install bus error handler
     */
    ctx->savelevel = raise_interrupt_level(ALL_DISABLE);
    ctx->handler = exceptionHandler(2, handle_buserror);

    if (setjmp(&gdb_env) == 0) {
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
    exceptionHandler(2,ctx->handler);
    reset_interrupt_level(ctx->savelevel);    
}

/*
 * gdb_write_membytes:
 * Write bytes from the gdb client command buffer to our memory
 */
void
gdb_write_membytes(debug_context *ctx)
{
    int addr, length;
    char *ptr;

    /*
     * Lock out interrupts and install bus error handler
     */
    ctx->savelevel = raise_interrupt_level(ALL_DISABLE);
    ctx->handler = exceptionHandler(2, handle_buserror);

    if (setjmp(&gdb_env) == 0) {
	if (parse2hexnum(&ctx->inbuf[1],&addr,&length)) {
	    ptr = strchr(ctx->inbuf,':');
	    ptr += 1; /* point 1 past the colon */
	    hex2mem(ptr, (char*) addr, length);
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
    exceptionHandler(2,ctx->handler);
    reset_interrupt_level(ctx->savelevel);
}

/*
 * gdb_proc_continue:
 * Restart a process at an optional address
 */
void
gdb_proc_continue(debug_context *ctx, int step)
{
    /* 
     * try to read optional parameter, addr unchanged if no parm
     */
    (void) parsehexnum(&ctx->inbuf[1],&ctx->cpu_ctx.registers[ PC ]);
    
    /* 
     * clear the trace bit
     */
    ctx->cpu_ctx.registers[ PS ] &= 0x7fff;
    
    /*
     * set the trace bit if we're stepping
     */
    if (step) 
	ctx->cpu_ctx.registers[ PS ] |= 0x8000;
}

/*
 * restore_vectors:
 * Restore vectors after running the system. This is done for compatibility
 * with old versions of the ROM monitor, which expect the system to leave
 * the vectors alone.
 *
 * In order to protect the ROM monitor from pending interrupts which are
 * not initialized in its vector table, we also lock out interrupts at this
 * point.
 */

void
restore_vectors(void)
{
#ifdef TRAP_PAGE_ZERO
    sched_restore_page_zero();
#endif
    raise_interrupt_level(ALL_DISABLE); /* Stop the world... */
    setvbr(rom_vector);		/* I want to get off */
}

/*
 * trap_bus_errors:
 * Restore debugger bus error handler
 */
void
trap_bus_errors(void)
{
    exceptionHandler(2, exception_handler);
}

/*
 * touch_device
 * Determines if an address exists by reading. Returns boolean.
 */

boolean touch_device (void *addr)
{
    leveltype SR;
    int temp;

    SR = raise_interrupt_level(ALL_DISABLE); /* Stop the world */

    exceptionHandler(2, handle_buserror); /* Trap bus errors here  */
    
    if (setjmp(&gdb_env) == 0) {
	
	/*
	 * Attempt to access the location as a byte
	 */

	temp = *((volatile char *)addr); /* Access the byte */

	/*
	 * If we got here, the address is valid. Disable exception
	 * catching and return TRUE.
	 */
	
	trap_bus_errors();	/* Debugger gets traps now */

	reset_interrupt_level(SR); /* Enable interrupts again */

	return(TRUE);	/* Indicate success */
    }	    

    /*
     * Failed, rearm trap and access as a word if an even address.
     * This kludgery is compatible with the legacy ROM monitor.
     */

    if (!((int)addr & 1)) {

	if (setjmp(&gdb_env) == FALSE) {

	    /*
	     * Access the location as a 16-bit word
	     */

	    temp = *((volatile short *)addr); /* Access the word */

	    /*
	     * If we got here, the address is valid. Disable
	     * exception catching and return TRUE.
	     */

	    trap_bus_errors();	/* Traps back to the debugger */
	    
	    reset_interrupt_level(SR); /* Enable interrupts again */

	    return(TRUE);	/* Indicate success */
	}
    }


    trap_bus_errors();		/* Traps go back to the debugger */
    reset_interrupt_level(SR);	/* Enable interrupts again */

    return(FALSE);		/* By now, return failure */
}

/*
 * Setup special handler for clock interrupts. This must be called
 * after freespace_init is called so that BSS memory is zeroed
 */
void
init_clocktick_handler(void)
{
    nmi_handler = system_vector_table[TIMER_VECTOR]; 
				/* small window handled here */
				/* exception_handler_clock calls */
				/* nmi_handler and it's NULL before */
				/* this */
    nmi_handler = exceptionHandler(TIMER_VECTOR, exception_handler_clock);
}

/*
 * exception_rom_monitor - Return to the ROM monitor
 */
static void exception_rom_monitor (void)
{
    int sigval;
#ifdef SYSRETURN
    int status;
#endif
    ulong *sp;

    sigval = computeSignal(exception_frame.exceptionVector);
    sp = (ulong *)exception_registers[A7]; /* Get saved stack pointer */

#ifdef SYSRETURN
    /*
     * Attempt to return the signal to the ROM monitor directly
     */

    status = mon_cpu_exception(sigval,
			       exception_frame.exceptionVector,
			       &crash_context);

    if (status == 0) {		/* Monitor handled it */
	return_from_exception();
    }
#endif /* SYSRETURN */

    /*
     * Handle break exceptions.
     *
     * If this was a break condition, then set the vector to
     * trap #0. This will cause the monitor to report undefined
     * trap.
     */

    if (exception_frame.exceptionVector == NMIVector) {
	exception_frame.exceptionVector = TRAP0Vector;
	exception_frame.format = TRAP0Vector << 2;
    }
    
#ifdef ROMRETURN

    /*
     * Push a new C-style frame on the stack to restore our context
     *
     * This allows us to execute code at rom_monitor_return after the
     * rom monitor continues.
     */

    *--sp = (ulong)exception_frame.pc;	/* Save the PC that we interrupted */
    *--sp = exception_registers[A6]; /* Save the A6 that we interrupted */
    
    (ulong *)exception_registers[A6] = sp; /* Set a new A6 value */
    (void (*)(void))exception_frame.pc = &_rom_monitor_return;
					 /* Set monitor return */
#endif /* ROMRETURN */

    /*
     * Push the exception stack frame back on the stack
     */

    ((ushort *)sp) -= exception_frame.frameSize; /* Make room for frame */

    (ulong *)exception_registers[A7] = sp; /* Set new stack pointer */

    bcopy(&exception_frame.sr, sp, exception_frame.frameSize * 2);
					/* Push the modified frame */

#ifdef TRAP_PAGE_ZERO
    /*
     * Restore exception vectors in page zero so we can
     * retrieve the ROM Monitor's exception handler for 
     * that exception when we insert the PC in the dummy 
     * stack frame we create to return to the ROM Monitor.
     */

    sched_restore_page_zero(); 
#endif /* TRAP_PAGE_ZERO */

    /*
     * Push our own stack frame to force control to resume in the ROM
     * montior.
     */

    exception_frame.frameSize = 4;	/* New frame is short */
    exception_frame.sr = 0x2700;	/* Set new PS value */
    exception_frame.pc = rom_vector[exception_frame.exceptionVector];
    exception_frame.format = 0;		/* Format 0 */

    /*
     * We will now return from this exception, via our synthetic stack
     * frame, to the original ROM monitor trap handler.
     *
     */

    return_from_exception();	/* How long to the point of know return? */
}

/*
 * handle_exception:
 * This function is called whenever an exception occurs. The front end
 * has saved all registers in exception_registers[], and the frame at
 * the time of exception into exception_frame.
 *
 * We will attempt to dispatch the exception to GDB. If GDB handles the
 * exception, it will not return.
 *
 * At this point, we will pass the exception back to the ROM monitor for
 * handling. This involves moving the exception frame back onto the saved
 * stack, and creating a "type 0" frame which will cause a return to the
 * ROM monitor.
 */

void handle_exception (int exceptionVector)
{
    int signal;

    /*
     * Get the signal number from the exception vector
     */

    signal = computeSignal(exceptionVector);

    /*
     * Give the signal handler an opportunity to handle the exception.
     * If the signal handler processes it, then it will not return.
     */

    signal_exception(signal);	/* Offer the exception */

    /*
     * Next give gdb an opportunity to handle this exception. If gdb
     * handles the exception, then it will not return, but instead
     * resume from the exception itself.
     */

    gdb_exception(signal);
    
    /*
     * If this is a spurious exception, then count it, and return
     * to the system if the spurious exception count is not exceeded.
     *
     * Note that spurious exceptions generally indicate hardware
     * problems or firmware bugs, so we only tolerate a few of them
     * before crashing the system.
     */

    if (IS_SPURIOUS_INTERRUPT(exceptionVector)) {
	spurcnt++;		/* Count it */
	if ((!spurious_threshold) || (spurcnt < spurious_threshold))
	  return;			/* Back from whence we came */
    }

    if (!process_get_boolean(system_crashed)) {
	process_set_boolean(system_crashed, TRUE); /* Prevent re-entrancy */
	GET_TIMESTAMP(crash_msclock);        /* Remember what time it was. */

	/*
	 * Preserve crash registers and frame
	 */

	exception_save_context(&crash_context);	/* Save crash context */
	
	if (dumphost.ip_addr &&		/* If we have someone to dump to */
	    dumpfile &&			/* And we have a dump file name */
	    system_running && 		/* And the scheduler is running */
	    !schedflag && 		/* And we were in process state */
	    forkx &&			/* And the process is set up */
	    exception_pid &&		/* And the crash process is OK */
	    !(crash_context.registers [ PS ] & 0x700)) { 
					/* And interrupts were on */

	    /*
	     * Check to see if the crash context area has been corrupted.
	     * If so, restore it and set a special reason code.
	     */

	    if ((crash_ptr.magic != CRASH_MAGIC) ||
		(CRASH_INFO != ((crashinfo *)(((char *)&crash_ptr)
					      - ((char *)RAMBASE))))) {
		crash_ptr.magic = CRASH_MAGIC;
		crash_ptr.version = CRASH_VERSION;
		crash_ptr.reason = CRASH_REASON_CORRUPT;
		crash_ptr.old_context = &crash_context.registers;
		crash_ptr.context = &crash_context;
		crash_ptr.rambase = (void *)RAMBASE;
                crash_ptr.textbase = (void *)TEXTBASE;
                crash_ptr.database = (void *)DATABASE;
                crash_ptr.bssbase  = (void *)BSSBASE;
		CRASH_INFO =  (crashinfo *)(((char *)&crash_ptr)
					    - ((char *)RAMBASE));
	    } else {

		/* Context not corrupt - just indicate exception */

		crash_ptr.reason = CRASH_REASON_EXCEPTION;
	    }
	    crash_ptr.cpu_vector = exceptionVector; /* Set vector */
	    ttyprintf(CONTTY, 
		      "\n\nUnexpected exception to CPU vector %d, "
		      "PC = %8x",
		      exceptionVector,
		      crash_context.saveFrame.pc);
	    ttyprintf(CONTTY, "%s\n", exception_save_traceback());
	    exception_jump(exception_write_core); /* Switch context */
	}
    }

    exception_rom_monitor();		/* Back to the ROM monitor */
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
    exception_rom_monitor();
}

/*
 * exceptionHandler:
 * Register an exception handler. This routine is used to load specific
 * values into the M68K vector table
 */
void (*exceptionHandler (int exception, void (*handler)(void)))(void)
{
    void (*oldHandler)(void);
    void (**vector_table)(void); /* Pointer to vectors */

    vector_table = getvbr();

    /* Load the vector table entry */

    oldHandler = vector_table[exception];
    vector_table[exception] = handler;
    return(oldHandler);
}

/*
 * gdb_init_exceptions:
 * Thus routine is called at system startup to set up trapping
 * of all the exceptions that the system image handles.
 */
void
gdb_init_exceptions(void)
{
    int exception;

    /*
     * Initialize all vectors to point to our exception handler
     */

    for (exception = 0; exception < VECTORCOUNT; exception++)
      system_vector_table[exception] = exception_handler;

    /*
     * Copy EMT call and NMI vectors from the ROM monitor
     * Some platforms such as SynALC do not use NMI for system timer vector.
     */

    rom_vector = getvbr();	/* Get the ROM monitor table */
    system_vector_table[NMIVector] = rom_vector[NMIVector];
    if (NMIVector != TIMER_VECTOR)
    {
	system_vector_table[TIMER_VECTOR] = rom_vector[TIMER_VECTOR];
    }
    system_vector_table[TRAP15Vector] = rom_vector[TRAP15Vector];

    /* 
     * If RAMBASE is zero on this platform, then stuff "illegal"
     * instructions in locations 0 - 6 to trap jumps to zero and
     * similar events.
     */
#if (RAMBASE == 0)
    *(ulong *)0x0 = ILLEGAL_INSTRUCTION;
    *(ulong *)0x4 = ILLEGAL_INSTRUCTION;
#endif

    /*
     * Switch to the new vector table
     */

    setvbr(system_vector_table); /* A hollow voice says "plugh" */

    /* 
     * Set up the crash information vector
     */
    CRASH_INFO = (crashinfo *)(((char *)&crash_ptr) - ((char *)RAMBASE));
}


/*
 * exception_interrupt - Determine if an exception occured during interrupts
 *
 * This routine is used to determine if the exception being processed
 * occured during an interrupt handler, or with interrupts disabled.
 */

boolean exception_interrupt (void)
{
    if (exception_registers[PS] & 0x700)
      return(TRUE);

    return(FALSE);

}

/*
 * exception_fault_addr - Return the fault address
 *
 * This routine returns the fault address from the current exception.
 * This is normally the PC, however if the exception is an address
 * exception, it will be the address that was being accessed.
 */

void *exception_fault_addr (void)
{
    switch (exception_frame.format & 0xf000) {
      case 0x0000:		/* Simple exceptions */
      case 0x1000:		/* Interrupt stack throwaway */
      default:

	return(exception_frame.pc);

      case 0x2000:		/* Instruction related traps */
      case 0x9000:		/* Coprocessor mid-instruction fault */
      case 0xc000:		/* CPU32 bus errors */

	return(*(void **)(&exception_frame.framedata[0]));

      case 0x7000:		/* 68040 access error */

	return(*(void **)(&exception_frame.framedata[6]));

      case 0x8000:		/* 68010 bus error */
	return(*(void **)(&exception_frame.framedata[1]));

      case 0xa000:		/* Short bus cycle fault */
      case 0xb000:		/* Long bus cycle fault */

	return(*(void **)(&exception_frame.framedata[4]));

    }
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
    frame = (ulong*)exception_registers[ A6 ];
    pc = (ulong*)exception_registers[ PC ];

    buf += sprintf(buf, "%s%x ", traceback_prefix, pc);
    for (ix = 0; ix < nframes; ++ix) {
	frame = trace_caller(frame, &pc);
	if (frame == NULL)
	    break;
	buf += sprintf(buf, "%x ", pc);
    }
    *--buf = '\0';		/* dump trailing space */
    return(buffer);
}
