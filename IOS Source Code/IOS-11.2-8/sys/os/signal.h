/* $Id: signal.h,v 3.2 1995/11/17 18:51:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/signal.h,v $
 *------------------------------------------------------------------
 * signal.h - Definitions for signal system
 *
 * January 13, 1995, Kevin Paul Herbert
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: signal.h,v $
 * Revision 3.2  1995/11/17  18:51:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SIGNAL_H__
#define __SIGNAL_H__

/*
 * Define the signals
 */
#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt (rubout) */
#define	SIGQUIT	3	/* quit (ASCII FS) */
#define	SIGILL	4	/* illegal instruction */
#define	SIGTRAP	5	/* trace trap */
#define	SIGIOT	6	/* IOT instruction */
#define SIGABRT 6	/* used by abort */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* death of a child */
#define SIGCHLD SIGCLD	/* 4.3BSD's name */
#define	SIGPWR	19	/* power-fail restart */

#define SIGWDOG	20	/* watchdog timer expiration */
#define SIGEXIT	21	/* sent just prior to process destruction */

#define SIGMAX (SIGEXIT)	/* Maximum defined signal */

/*
 * The following oddball define is used by the Rom Monitor
 */
#define SIGDIAG 99      /* RSP Diagnostic Signal */

/*
 * Start of stuff that is C-only
 */

#ifndef ASMINCLUDE

/*
 * Type of a signal handler
 */

typedef void (*signal_handler)(int, int, void *, char *);

/*
 * Special values for signal operations
 *
 * Note: We define these as void * because there are two possible
 * structure types for signal handlers, depending on whether it is
 * the thread-oriented signalling or the R4K system-wide signalling.
 */

#define SIG_ERR			((void *)-1)
#define SIG_DFL			((void *)0)
#define SIG_IGN			((void *)1)


/*
 * Define the layout of a signal table
 */

struct signal_table_ {
    signal_handler dispatch[SIGMAX];	/* Dispatch table */
    tinybool permanent[SIGMAX];		/* One-shot or permanent */
};


/*
 * External routines
 */

extern void signal_exception(int);
extern void signal_send(pid_t, int);
extern signal_handler signal_oneshot(int, signal_handler);
extern signal_handler signal_permanent(int, signal_handler);

#endif /* ASMINCLUDE */

#endif /* __SIGNAL_H__ */
