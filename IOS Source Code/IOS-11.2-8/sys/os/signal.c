/* $Id: signal.c,v 3.3.62.2 1996/08/28 13:04:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/signal.c,v $
 *------------------------------------------------------------------
 * signal.c - Per-thread signal support
 *
 * January 13, 1995, Kevin Paul Herbert
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: signal.c,v $
 * Revision 3.3.62.2  1996/08/28  13:04:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:31:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:10:29  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  16:41:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:51:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/08  16:17:00  hampton
 * Don't spin the random number generator as frequently from the
 * scheduler.  Fix test in signal_receive.  [CSCdi39965]
 *
 * Revision 2.2  1995/06/21 08:56:20  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  21:58:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sched_private.h"
#include "signal.h"
#include "gdb.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*********************************************************************
 *
 *		  PROCESS HARDWARE GENERATED SIGNALS
 *
 *********************************************************************/

/*
 * signal_resume - Resume a thread in a signal handler
 *
 * This routine is called on the caller's stack (or a thread-specific
 * exception stack) when an exception has occured that will be passed
 * to a signal handler.
 *
 * We are called with all interrupts disabled, and with the exception
 * context still available. We must save the exception context before
 * enabling interrupts, or the context will become unavailable.
 *
 * Note that the callback handler *is* allowed to use longjmp() to
 * alter the flow of control, so this routine must not depend on a return
 * after calling the user's handler. In particular, any context that is
 * saved must be allocated on the stack and not in dynamic memory.
 */

static void signal_resume (void)
{
    int signum;
    cpu_context_t ctx;
    signal_handler handler;
    signal_table *table;
    void *addr;

    /*
     * Save the exception context on the stack before enabling 
     * interrupts.
     */

    exception_save_context(&ctx); /* Save our context on the stack */
    signum = forkx->pending_signal; /* Save signal number on stack */
    addr = exception_fault_addr(); /* Save fault address on stack */

    /*
     * Enable interrupts. Note that once we enable interrupts, any
     * exceptions that occur are trappable.
     *
     * A consequence of this is that if the handler itself gets
     * an exception, we will exhaust the stack.
     */

    set_interrupt_level(ALL_ENABLE); /* Enable interrupts */

    /* Set up to call the handler */

    table = forkx->signal; /* Get signal table address */
    handler = table->dispatch[signum]; /* Get our handler address */

    /* Clear out the handler dispatch if this is a oneshot handler. */

    if (!table->permanent[signum])
	table->dispatch[signum] = SIG_DFL;

    /* Call the handler */

    (*handler)(signum, 0L, NULL, addr);

    /*
     * The handler has returned. Now restore the exception state.
     */

    raise_interrupt_level(ALL_DISABLE);	/* Disable interrupts */
    exception_restore_context(&ctx); /* Restore exception context */

    /*
     * Return from the exception
     */

    return_from_exception();
}

/*
 * signal_exception - Signal an exception
 *
 * This routine is used to signal an exception. We are called as part of
 * exception processing. All interrupts have been disabled at the time
 * of the exception.
 *
 * If this thread is trapping this exception, then we will resume execution
 * in the interrupted thread, setting up a new context. Otherwise, we will
 * return to the exception handling code which will treat this as an
 * unhandled exception.
 */

void signal_exception (int signum)
{
    signal_table *table;
    signal_handler handler;

    /*
     * If this exception occured with interrupts off, then it can
     * not be trapped.
     */

    if (exception_interrupt())
      return;			/* If locked out, no trapping */

    /*
     * If the scheduler is not yet set up, or if this exception occured
     * within scheduler context, it can not be trapped.
     */

    if ((!system_running) || schedflag || (!forkx))
      return;

    /*
     * By now, the exception has occured within the context of an
     * executing thread. Get the signal table for that thread. If
     * there is no signal table, then this exception is not being
     * trapped.
     */

    table = forkx->signal;	/* Get the signal table */
    if (!table)
      return;			/* If no table, no trapping */

    /*
     * Get the dispatch address for the handler
     *
     * If this signal is to be defaulted or ignored, do so here
     */

    handler = table->dispatch[signum]; /* Get the handler address */
    if (handler == SIG_DFL)
      return;			/* Default handler, nothing to do here */

    if (handler == SIG_IGN) {
	/* Should modify context to no reexecute fault, if possible */
	return_from_exception(); /* Return to the caller */
    }

    /*
     * Set the signal number in global storage and continue executing
     * on the caller stack. The exception jump mechanism does not support
     * parameter passing.
     *
     * Note that if we implement separate exception stacks for
     * threads, we would switch to that stack here.
     */

    forkx->pending_signal = signum; /* Set up the argument */
    exception_jump(signal_resume); /* Now continue on another stack */
}


/*********************************************************************
 *
 *		  PROCESS SOFTWARE GENERATED SIGNALS
 *
 *********************************************************************/

/*
 * signal_receive
 *
 * Receive a software generated signal from another process.  These
 * signals are similar to the signals sent by the unix 'kill' command.
 * The receiving process' execution thread will be modified wake up from
 * its current scheduler test and begin executing here.  When it returns
 * from this routine, it will begin executing the processes normal code
 * again as if the scheduler call had simple completed.
 */
void signal_receive (void)
{
    signal_table *table;
    signal_handler handler;
    int signum = forkx->pending_signal;
    boolean process_already_dead;

    /*
     * If this was a KILL signal, the process will die whether or not
     * it wants to.
     */
    process_already_dead = forkx->killed;
    if ((signum == SIGKILL) || (signum == SIGEXIT))
	forkx->killed = TRUE;

    /*
     * Get the signal table for that thread. If there is no signal table,
     * then this exception is not being trapped.
     */
    handler = NULL;
    table = forkx->signal;	/* Get the signal table */
    if (table) {		/* If no table, no trapping */
	/*
	 * Get the dispatch address for the handler.  Check to see if
	 * there is really something to do.
	 */
	handler = table->dispatch[signum];
	if ((handler != SIG_DFL) && (handler != SIG_IGN)) {

	    /*
	     * Clear out the handler dispatch if this is a one shot handler.
	     */
	    if (!table->permanent[signum])
		table->dispatch[signum] = SIG_DFL;
	    
	    /*
	     * Call the handler
	     */
	    (*handler)(signum, 0L, NULL, NULL);
	}
    }

    /*
     * Certain signals require special processing.  If this is a death
     * signal and the process is already marked as dead, then this
     * routine was called from process_last_gasp.  If not, then call that
     * routine.
     */
    if (signum == SIGWDOG)
	process_handle_watchdog(handler != NULL);
    if ((signum == SIGKILL) || (signum == SIGEXIT))
	if (!process_already_dead)
	    process_last_gasp();
}

/*
 * signal_send
 *
 * Send a signal to another process.  These signals are similar to the
 * signals sent by the unix 'kill' command.  The other processes
 * execution stack will be modified so that its current scheduler call
 * returns into the signal_receive routine, and that will return to the
 * previous execution point.  If a process sends a signal to itself, this
 * routine just calls signal_receive directly.
 */
void signal_send (pid_t pid, int signum)
{
    sprocess *proc;

    proc = process_pid2ptr(pid);
    if (!proc) {
	(*kernel_errmsg)(&msgsym(NOPROCESS, SYS), pid);
	return;
    }

    proc->pending_signal = signum;
    if (proc == forkx) {
	signal_receive();
    } else {
	alter_suspended_process(proc, signal_receive);
	process_wakeup_internal(proc, DIRECT_EVENT, 0);
    }
}


/*********************************************************************
 *
 *		     INITIALIZING SIGNAL HANDLERS
 *
 *********************************************************************/

/*
 * signal_set_common
 *
 * The common routine for setting both one-shot and permanent signal
 * handlers.
 */

static signal_handler signal_set_common (
    int signum,
    signal_handler handler,
    boolean permanent)
{
    signal_table *table;
    signal_handler old_handler;

    /*
     * Barf on untrappable signals.
     */
    if (signum == SIGKILL)
	return(SIG_ERR);

    /*
     * Get the pointer to this thread's signal table. If there isn't
     * one, create it.
     */
    table = forkx->signal;	/* Get the signal table */
    if (!table) {
	table = malloc_named(sizeof(signal_table), "Process Signals");
	if (!table)
	    return(SIG_ERR);	/* Need to pass back a failure */
	forkx->signal = table;	/* Remember table address */
    }

    old_handler = table->dispatch[signum]; /* Save the old handler address */
    table->dispatch[signum] = handler;	/* Set the handler address */
    table->permanent[signum] = permanent;	/* Mark as one-shot */

    return(old_handler);	/* Return the old value */
}

/*
 * signal_oneshot - Set a one-shot signal handler for a thread
 */

signal_handler signal_oneshot (int signum, signal_handler handler)
{
    return(signal_set_common(signum, handler, FALSE));
}

/*
 * signal_permanent - Set a permanent signal handler for a thread
 */

signal_handler signal_permanent (int signum, signal_handler handler)
{
    return(signal_set_common(signum, handler, TRUE));
}

#ifdef TESTCODE

/*
 * exception_test - Some test code
 */

jmp_buf buf;

static void exception_test_handler
(int signal, int code, void *x, char *a)
{
    printf("\nIn simple handler: signal = %d, addr = %x", signal, a);
    flush();
}

static void exception_test_handler_longjmp
(int signal, int code, void *x, char *a)
{
    printf("\nIn longjmp handler: signal = %d, addr = %x", signal, a);
    flush();
    longjmp(&buf, 1);
}

void exception_test (void)
{
    int y;

    printf("\nInstalling handler");
    flush();

    signal_oneshot(SIGBUS, exception_test_handler_longjmp);

    if (setjmp(&buf) == 0) {

	printf("\nExecuting bus error");
	flush();

	y = *(volatile int *)0xfffffffc;
    } else {
	printf("\nJust got back from bus error");
	flush();
    }

    printf("\nAfter bus error... doing illegal instruction test");
    flush();

    signal_oneshot(SIGILL, exception_test_handler);

    if (setjmp(&buf) == 0) {
	printf("\nExecuting illegal instruction");
	flush();

	asm("illegal");
    } else {
	printf("\nJust got back from illegal instruction");
    }

    printf("\nAfter illegal instruction");
    flush();

}

#endif /* TESTCODE */
