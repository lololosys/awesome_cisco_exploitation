/* $Id: signal.c,v 3.2.58.1 1996/03/21 23:27:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/signal.c,v $
 *------------------------------------------------------------------
 * signal.c
 *
 * April 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Simulate the Un*x signal mechanism for 3 signals: SIGINT, SIGBUS,
 * & SIGALRM.
 *------------------------------------------------------------------
 * $Log: signal.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:14  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Simulate the Un*x signal mechanism for 3 signals: SIGINT, SIGBUS,
** & SIGALRM.
*/
#include "monitor.h"
#include "signal_4k.h"

SIG_PF sigintfcn, sigbusfcn, sigalrmfcn, sigdiagfcn;

SIG_PF
signal(sig,function)
    int sig;
    SIG_PF function;
{
    SIG_PF retval;

    switch(sig) {
    case SIGINT:
	retval = sigintfcn;
	sigintfcn = function;
	break;
    case SIGBUS:
	retval = sigbusfcn;
	sigbusfcn = function;
	break;
    case SIGALRM:
	retval = sigalrmfcn;
	sigalrmfcn = function;
	break;
    case SIGDIAG:
	retval = sigdiagfcn;
	sigdiagfcn = function;
	break;
    default:
	retval = (SIG_PF)-1;
	break;
    }
    return(retval);
}

/*
** Initialize all of the signals to something sane.
*/
initsigs()
{
    SIG_PF function = SIG_DFL;
    
    sigintfcn = function;
    sigbusfcn = function;
    sigalrmfcn = function;
    sigdiagfcn = function;
}
