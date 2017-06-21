/* $Id: diagtools.c,v 3.2.58.1 1996/03/21 23:27:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/diagtools.c,v $
 *------------------------------------------------------------------
 * diagtools.c
 *
 * July 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Tools for diagnostics.
 *------------------------------------------------------------------
 * $Log: diagtools.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:12  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "signal_4k.h"

/*
** pstat -  timed polling status routine
**
** poll reading status and apply mask until status equals/does not equal
** the desired value or the alarm goes off
**
** time value is in milliseconds.
** a time of zero means to poll indefinitely.
** if a time is specified pstat will return the time left if status is
** satisfied, otherwise pstat will return zero.
** if status is satisfied, the status is copied to *retptr.
*/

static char abort_flag;

static
alrmfcn(void)
{
    abort_flag = 1;
}

int
pstat(unsigned long *location, int size, int eq, unsigned long mask, unsigned long cmpval, int msecs, unsigned long *retptr)
{
    long readval;
    SIG_PF savsig;

    abort_flag = 0;
    if(msecs) {
	savsig = signal(SIGALRM, (SIG_PF)alrmfcn);
	alarm(msecs);
    }
    while(1) {
	switch(size) {
	case 1:
	    readval = *(unsigned char *)location;
	    break;
	case 2:
	    readval = *(unsigned short *)location;
	    break;
	default:
	    readval = *location;
	    break;
	}
	readval &= mask;
	if(eq) {
	    if(readval == cmpval) break;
	} else {
	    if(readval != cmpval) break;
	}
	if(abort_flag) return(0);
    }
    if(retptr) *retptr = readval;
    if(msecs) {
	signal(SIGALRM, savsig);
	return(alarm(0));
    }
    return(0);
}










