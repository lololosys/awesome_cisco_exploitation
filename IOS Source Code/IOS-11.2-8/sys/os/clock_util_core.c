/* $Id: clock_util_core.c,v 3.1.16.2 1996/07/10 22:21:51 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/clock_util_core.c,v $
 *------------------------------------------------------------------
 * Clock utility routines included in the ukernel
 *
 * Dave Katz, December 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clock_util_core.c,v $
 * Revision 3.1.16.2  1996/07/10  22:21:51  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/03/18  21:29:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:09:11  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:08  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright 1992 David L. Mills
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name University of Delaware
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  The
 * University of Delaware makes no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC01305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.sac.ca.us> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de> (DCF77 clock
 *      driver, STREAMS module for DCF77, support scripts)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (WWVB and Austron GPS clock drivers,
 *	pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 */

#include "master.h"
#include "../os/clock.h"


/*
 * Table of day offsets within year (March-based)
 */
static ulong month_offset[] = {0,
				   MAR, 
				   MAR+APR, 
				   MAR+APR+MAY, 
				   MAR+APR+MAY+JUN,
				   MAR+APR+MAY+JUN+JUL,
				   MAR+APR+MAY+JUN+JUL+AUG,
				   MAR+APR+MAY+JUN+JUL+AUG+SEP,
				   MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT,
				   MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV,
				   MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC,
				   MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC+JAN
				   };


/*
 * clock_epoch_to_timeval - convert NTP timestamp to a timeval
 *
 */
void clock_epoch_to_timeval(clock_epoch *ntp, timeval *tv, long tz_offset)
{
    ulong secnum, cyclenum, sec_in_cycle, year_in_cycle, sec_in_year;
    ulong day_in_year, sec_in_day, sec_in_hour, month_in_year, day_in_month;
    ulong year_fudge, day_in_epoch;

    tv->tz_offset = tz_offset;

    /* Fudge if it's before March 1, 1900. */

    if (ntp->epoch_secs < MAR1900)
	year_fudge = SECSPERYEAR;
    else
	year_fudge = 0;

    /* First figure out which cycle we're in. */

    secnum = ntp->epoch_secs + tz_offset + year_fudge - MAR1900;
    cyclenum = secnum / SECSPERCYCLE;
    sec_in_cycle = secnum % SECSPERCYCLE;

    /* Now what day since the beginning of NTP time, and the day of week. */

    day_in_epoch = (ntp->epoch_secs + tz_offset) / SECSPERDAY;
    tv->day_of_week = (day_in_epoch + JAN11900DOW) % 7;

    /* Now the year in cycle. */

    year_in_cycle = sec_in_cycle / SECSPERYEAR;
    sec_in_year = sec_in_cycle % SECSPERYEAR;
    if (year_in_cycle == YEARSPERCYCLE) {	/* Feb 29? */
	year_in_cycle--;
	sec_in_year += SECSPERYEAR;
    }

    /* Now the day in month and month in year. */

    day_in_year = sec_in_year / SECSPERDAY;
    sec_in_day = sec_in_year % SECSPERDAY;
    for (month_in_year = 1; month_in_year < 12; month_in_year++) {
	if (day_in_year < month_offset[month_in_year])
	    break;
    }
    month_in_year--;
    day_in_month = day_in_year - month_offset[month_in_year];

    /* Now we can figure out the calendar year. */

    tv->year = 1900 + (YEARSPERCYCLE * cyclenum) + year_in_cycle + 
	(month_in_year >= 10) - (year_fudge != 0);

    /* And the calendar month... */

    tv->month = ((month_in_year + 2) % 12) + 1;

    /* And the day... */

    tv->day = day_in_month + 1;

    /* And the hour... */

    tv->hour = sec_in_day / SECSPERHR;
    sec_in_hour = sec_in_day % SECSPERHR;

    /* And the minute... */

    tv->minute = sec_in_hour / SECSPERMIN;

    /* And the second... */

    tv->second = sec_in_hour % SECSPERMIN;

    /* And the millisecond... */

    tv->millisecond = ntp->epoch_frac / (((1<<29) / 125) + 1);

    /*
     * Calculate the Julian date (day in year).  Note that this is mildly
     * nontrivial because our calculations are March-based, and we need
     * to start the year in January instead (how inconvenient).
     */
    if (month_in_year > 9) {		/* Jan or Feb */
	tv->day_of_year = day_in_year - (DAYSPERYEAR - JAN - FEB) + 1;
    } else {
	tv->day_of_year = day_in_year + JAN + FEB + 1;
	if (year_in_cycle == 0) {		/* Leap year? */
	    tv->day_of_year++;
	}
    }
}

/*
 * clock_timeval_to_epoch - convert a timeval to an NTP timestamp
 */
void clock_timeval_to_epoch(timeval *tv, clock_epoch *ntp)
{
    ulong local_month, local_year, day_in_year, cyclenum, year_in_cycle;
    ulong day_in_cycle;

    ntp->epoch_frac = tv->millisecond * (((1<<29) / 125) + 1);

    local_month = (tv->month + 9) % 12;
    local_year = tv->year - (tv->month < 3);

    day_in_year = tv->day - 1 + month_offset[local_month];
    cyclenum = (local_year - 1900) / YEARSPERCYCLE;
    year_in_cycle = (local_year - 1900) % YEARSPERCYCLE;
    day_in_cycle = year_in_cycle * DAYSPERYEAR + day_in_year;
    ntp->epoch_secs = MAR1900 + (cyclenum * SECSPERCYCLE) + 
	(day_in_cycle * SECSPERDAY) + (tv->hour * SECSPERHR) + 
	    (tv->minute * SECSPERMIN) + tv->second - tv->tz_offset;
}

/*
 * clock_timeval_to_unix_time
 *
 * Converts a timeval structure to a Unix-style timestamp
 */
ulong clock_timeval_to_unix_time (timeval *tv)
{
    clock_epoch epoch;

    /* Convert to an epoch, and then to Unix time. */

    clock_timeval_to_epoch(tv, &epoch);
    return (clock_epoch_to_unix_time(&epoch));
}

/*
 * unix_time_to_epoch
 *
 * Convert a unix-style timestamp (seconds since 1 Jan 1970) into a
 * clock_epoch structure.
 */
void unix_time_to_epoch (ulong unix_time, clock_epoch *epoch_ptr)
{

    epoch_ptr->epoch_secs = unix_time + JAN1970;
    epoch_ptr->epoch_frac = 0;
}

/*
 * clock_epoch_to_unix_time
 *
 * Convert an epoch to a Unix-style timestamp (seconds since 0000 UTC
 * 1 Jan 1970).
 *
 * If the current time is previous to 1970, this routine will return
 * unpredictable results.
 */
ulong clock_epoch_to_unix_time (clock_epoch *epoch_ptr)
{
    return (epoch_ptr->epoch_secs - JAN1970);
}
