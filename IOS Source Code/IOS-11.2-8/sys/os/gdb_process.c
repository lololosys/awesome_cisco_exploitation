/* $Id: gdb_process.c,v 3.1.16.3 1996/07/10 22:22:08 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/gdb_process.c,v $
 *------------------------------------------------------------------
 * gdb_process.c  --  GDB per process debugging
 *  
 * October 1, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_process.c,v $
 * Revision 3.1.16.3  1996/07/10  22:22:08  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.16.2  1996/06/28  23:24:45  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/03/18  21:29:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:09:37  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:09  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "string.h"
#include "gdb.h"
#include "ttysrv.h"
#include "sched_private.h"


/*
 * gdbioBLOCK - block routine for GDB process I/O
 *
 * This routine blocks for a character to be entered, or for modem
 * disconnect or the software escape sequence.
 */

boolean gdbioBLOCK (tt_soc *tty)
{
    if (inputpending(tty) || modemcheck(tty) || psiset(tty))
	return(FALSE);
    return(TRUE);
}

/*
 * debug_getchar - Get a character for process debugging
 */

int debug_getchar (void)
{
    int ch;

    while (TRUE) {
	if (modemcheck(stdio) || psiset(stdio))
	  return(-1);

	ch = getc(stdio);

	if (ch > 0 && ch <= 0xFF)
	  return(ch);
	edisms((blockproc *)gdbioBLOCK, (ulong)stdio);
    }
}

/*
 * debug_putchar - Get a character for process debugging
 */

void debug_putchar (uchar ch)
{
    putc(stdio, ch);
}

/*
 * debug_flush - Flush output for process debugging
 */

void debug_flush (void)
{
    startoutput(stdio);
}

/*
 * gdb_shutdown - Remove debugging context for a user
 *
 * This routine is called when a process debugging another process is being
 * deleted.
 */

void gdb_shutdown (tt_soc *tty)
{
    debug_context *ctx = (debug_context *)tty->debugging;
    sprocess *fork;

    if (ctx == NULL) {
	/* not doing any debugging */
	return;
    }

    fork = ctx->fork;
    if (fork) {
	fork->debugger = NULL;		/* Clear context pointer */
	fork->crashed = FALSE;		/* Not crashed anymore */

	while ( !gdb_frame_unused(ctx) ) {
	    process_sleep_for(ONESEC/10);/* Wait for context to be free */
	}
    }
    free(ctx);
    tty->debugging = NULL;
}

/*
 * crashBLOCK - block until a process crashes or user goes away
 */

boolean crashBLOCK (debug_context *ctx)
{
    sprocess *fork = ctx->fork;
    tt_soc *tty = ctx->tty;

    if (fork->crashed || psiset(tty) || modemcheck(tty) ||
	inputpending(tty)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * debug_process - Debug a running process
 *
 * This routine is used to debug a running process. We look up the
 * process, and flag it to execute a trace trap the next time it
 * runs.
 *
 * At that point, we sit in a loop, waiting for the process to crash.
 * When the process crashes, we run the gdb interface to debug it. When
 * the user continues the process, we clear the crash flag and wait for
 * it to crash again.
 */

void debug_process (int pid)
{
    debug_context *ctx;
    sprocess *fork;

    if (pid == -1) {			/* Check for invalid values */
	printf("\nInvalid number");
	return;
    }

    fork = process_pid2ptr(pid);	/* Look up the process */
    if (!fork) {
	printf("\nNo such process %d", pid);
	return;
    }

    if (fork == forkx) {
	printf("\nCan't debug your own process");
	return;
    }

    if (fork->debugger) {
	printf("\nProcess already debugged by tty%t",
	       fork->debugger->tty->ttynum);
	return;
    }

    ctx = malloc(sizeof(debug_context));
    if (!ctx) {
	printf(nomemory);
	return;
    }

    /* Set up the I/O routine pointers */

    ctx->getchar = debug_getchar;
    ctx->putchar = debug_putchar;
    ctx->flush = debug_flush;

    /* Save the fork pointer */

    ctx->fork = fork;

    /* Set up pointer back to the debugging tty */

    ctx->tty = stdio;

    /* And set the tty pointer to the context */

    stdio->debugging = ctx;

    fork->debugger = ctx;		/* Link it to our debug block */
    gdb_proc_cause_exception(fork);	/* Setup process to take exception */

    psion(stdio);			/* Listen for abort sequence */

    while (TRUE) {
	edisms((blockproc *)crashBLOCK, (ulong)ctx);
	while (inputpending(stdio))
	  getc(stdio);
	if (modemcheck(stdio) || psiset(stdio)) /* Quit is we should */
	  break;
	if (!fork->crashed)		/* Has process crashed yet? */
	  break;			/* No, loop until it does */

	if (!gdb_interface(ctx, FALSE))
	  break;			/* Examine the process */
	if (fork->killed)		/* Is the process dying? */
	  break;			/* Yes, get out */
	fork->crashed = FALSE;		/* Let the process run again */
    }
    fork->debugger = NULL;		/* Dump link to context */
    stdio->debugging = NULL;		/* Not debugging anymore */
    fork->crashed = FALSE;		/* Process not crashed now */

    while ( !gdb_frame_unused(ctx) ) {
	process_sleep_for(ONESEC/10);	/* Wait for frame to be free */
    }

    free(ctx);				/* Free the context block */
    psioff(stdio);			/* Don't listen to abort */
}    

/*
 * examine_process - Examine a process
 *
 * This is used to do a read-only examination of the system. It takes
 * an optional process number to set up stack context for; by default,
 * it will use the calling process.
 */

void examine_process (int pid)
{
    debug_context *ctx;
    sprocess *fork;

    fork = process_pid2ptr(pid); /* Look up the process */
    if (!fork) {
	printf("\nNo such process");
	return;
    }

    ctx = malloc(sizeof(debug_context));
    if (!ctx) {
	printf(nomemory);
	return;
    }

    /* Set up the I/O routine pointers */

    ctx->getchar = debug_getchar;
    ctx->putchar = debug_putchar;
    ctx->flush = debug_flush;

    stdio->debugging = ctx;

    /* Set up the process registers */

    copy_fork_debug(ctx, fork);
    ctx->signal = SIGTRAP;		/* Mark as a trap entry */

    psion(stdio);			/* Enable software interrupts */
    gdb_interface(ctx, TRUE);
    psioff(stdio);			/* Disable software interrupts */
    stdio->debugging = NULL;		/* Clear debugging pointer */
    free(ctx);
}
