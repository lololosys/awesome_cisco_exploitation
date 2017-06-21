/* $Id: unix_timer.c,v 3.3.8.2 1996/08/19 19:02:15 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_timer.c,v $
 *------------------------------------------------------------------
 * unix_timer.c - Unix timer routines
 *
 * December 1991, Kirk Lougheed
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_timer.c,v $
 * Revision 3.3.8.2  1996/08/19  19:02:15  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.3.8.1  1996/03/18  22:30:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:14:37  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:59:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:28:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:30:08  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  23:12:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "logger.h"
#include "ttysrv.h"
#include "config.h"
#include "unix.h"
#include "../os/platform_clock.h"



/*
 * get_timer_parameters
 *
 * Get information about the hardware timer (used by the system clock
 * support)
 */
void get_timer_parameters (ulong *freq_numerator, ulong *freq_denominator,
			   ulong *freq_divisor, 
			   get_resid_type *get_residual,
			   ulong *precision)
{
    *freq_numerator = NMI_FREQ_NUMERATOR;
    *freq_denominator = NMI_FREQ_DENOMINATOR;
    *freq_divisor = 1;		/* No clock really */
    *get_residual = NULL;	/* No residual */
    *precision = 6;		/* lousy clock */
}

void wastetime (ulong n)
{
    for ( ; n > 0; n--)
	;
}

void msecdelay (uint msecs)
{
    while (msecs > 0) {
	usecdelay(1000);
	--msecs;
    }
    return;
}

/*
 * timer_calibrate:
 * Calibrate the spin loop for a particular delay
 */
ulong 
timer_calibrate (ulong tt, boolean print_timer)
{
    return(tt);
}
