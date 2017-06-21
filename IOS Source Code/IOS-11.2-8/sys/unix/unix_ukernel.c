/* $Id: unix_ukernel.c,v 3.1.6.1 1996/03/18 22:30:19 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/unix/unix_ukernel.c,v $
 *------------------------------------------------------------------
 * unix_ukernel.c - Unix kernel routines
 *
 * March 1996, David Hampton
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_ukernel.c,v $
 * Revision 3.1.6.1  1996/03/18  22:30:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/09  05:14:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/03/08  22:07:12  hampton
 * Add placeholders for Dead King branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We compile this module using the standard UNIX include paths
 */
#include "unix.h"
#include <stdio.h>
#ifdef __svr4__
#undef boolean		/* "boolean" is an enum in sys/types.h on Solaris */
#endif /* svr4 */
#include <sys/types.h>
#ifdef __svr4__
#define boolean int	/* ... but we still need our definition from unix.h */
#endif /* svr4 */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <string.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <net/if.h>
#ifndef __svr4__
#include <net/nit_if.h>
#else /* __svr4__ */
#include <sys/sockio.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/dlpi.h>
#endif /* __svr4__ */

#ifndef __svr4__
static struct sgttyb ttymode;
#else /* __svr4__ */
static struct termios ttymode;
#endif /* __svr4__ */

/*
 * The system clock
 */
volatile unsigned long clock_low = 0;
volatile unsigned long clock_high = 0;


/*
 *
 */
#ifdef __svr4__
void unix_clock_tick (int sig)
#else __svr4__
void unix_clock_tick (
    int sig,
    int code,
    struct sigcontext *scp,
    char *addr)
#endif __svr4__
{
    unsigned long old_clock = clock_low;

    clock_low += 4;
    if (clock_low < old_clock)
	clock_high++;
}

/*
 * setup_signal_handlers()
 * Set up in signal handling we may need
 */
void setup_signal_handlers (void)
{
    struct itimerval clock_setup;
    struct timeval four_ms;

    /*
     * Setup to catch alarm clock signal
     */
#ifdef __svr4__
    sigset(SIGVTALRM, unix_clock_tick); 
#else __svr4__
    signal(SIGVTALRM, unix_clock_tick); 
#endif __svr4__

    /* 4000 microseconds == 4 millisecond */
    four_ms.tv_sec = 0;
    four_ms.tv_usec = 4000;
    clock_setup.it_value = four_ms;
    clock_setup.it_interval = four_ms;
    setitimer(ITIMER_VIRTUAL, &clock_setup, NULL);
}

/*
 * unix_brk
 * Set maximum memory usage for this UNIX process
 */

int unix_brk (incr)
    int incr;
{
    int result;

    result = sbrk(incr);
    return(result == -1 ? TRUE : FALSE);
}

/*
 * unix_setcooked
 * Put stdin back into cooked mode.
 */

void unix_setcooked ()
{
#ifndef __svr4__
    ioctl(0, TIOCSETN, &ttymode);
#else /* __svr4__ */
    if (tcsetattr(0, TCSAFLUSH, &ttymode) == -1) {
	perror("tcsetattr");
	return;
    }
#endif /* __svr4__ */
}

/*
 * unix_setraw
 * Put stdin into raw mode.
 */

void unix_setraw ()
{
#ifndef __svr4__
    struct sgttyb ttydata;

    /* get current parameters */
    if (ioctl(0, TIOCGETP, &ttydata) == -1) {
	perror("TIOCGETP");
	return;
    }
    ttymode = ttydata;
    ttydata.sg_flags &= ~CRMOD; /* don't convert CR to LF on input */
    ttydata.sg_flags &= ~ECHO;	/* we handle the echo ourself */
    ttydata.sg_flags |= CBREAK;	/* activate on character input */
    ttydata.sg_flags |= RAW;	/* no output processing of characters */
    ioctl(0, TIOCSETN, &ttydata); /* set new parameters */
#else /* __svr4__ */
    struct termios ttydata;

    if (tcgetattr(0, &ttymode) == -1) {
	perror("tcgetattr");
	return;
    }
    ttydata = ttymode;
    ttydata.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    ttydata.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    ttydata.c_cflag &= ~(CSIZE | PARENB);
    ttydata.c_cflag |= CS8;
    ttydata.c_oflag &= ~(OPOST);
    ttydata.c_cc[VMIN] = 1;
    ttydata.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSAFLUSH, &ttydata) == -1) {
	perror("tcsetattr");
	return;
    }
#endif /* __svr4__ */
}

/*
 * unix_exit
 * Implement the cisco "reload" command
 */
void 
unix_exit (int core_flag)
{
    unix_setcooked();
    if (core_flag) {
	kill(0, SIGABRT);
    } else {
	exit(0);
    }
}
