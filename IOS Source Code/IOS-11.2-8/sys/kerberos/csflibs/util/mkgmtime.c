/* $Id: mkgmtime.c,v 3.2 1995/11/17 17:46:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/mkgmtime.c,v $
 *------------------------------------------------------------------
 * mkgmtime.c - Yet another gmtime() for Kerberos
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mkgmtime.c,v $
 * Revision 3.2  1995/11/17  17:46:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/* Original code placed in the public domain by Mark W. Eichin */

#include <csf/header.h>

/* Take a struct tm, return seconds from GMT epoch
 * As with mktime, this ignores tm_wday and tm_yday.
 * Unlike mktime, this does not set them... it only passes a return value.
 */

/* Total days through a given month. */
static const days_in_month [12] = {
	/* jan  feb  mar  apr  may  jun  jul  aug  sep  oct  nov  dec */
	/* 31   28   31   30   31   30   31   31   30   31   30   31  */
	    0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
};

#define hasleapday(year) (year%400?(year%100?(year%4?0:1):0):1)

csf_time_t csfCmkgmtime (
	const csf_tm *t
){
	csf_time_t accum;

	/* Ensure we have a valid time. */
	{
		int errflag = 0;
		errflag |= (t->tm_year < 70) || (t->tm_year > 138);
		errflag |= (t->tm_mon < 0) || (t->tm_mon > 11);
		/*TBD/JNK tm_mday one-based? should be >=1? */
		errflag |= (t->tm_mday < 0) || (t->tm_mday > 31);
		errflag |= (t->tm_hour < 0) || (t->tm_hour > 23);
		errflag |= (t->tm_min < 0) || (t->tm_min > 59);
		errflag |= (t->tm_sec < 0) || (t->tm_sec > 62);
		if (errflag) {
			return (csf_time_t)(-1);
		}
	}

	/* Years.
	 */
	accum = t->tm_year - 70;			/* Since epoch. */

	/* Days.
	 */
	accum *= 365;						/* 365 days year. */

	/* Days in leap years. */
	accum += (t->tm_year - 68) / 4;	/* All previous years. */
	if (t->tm_mon > 1) {				/* This year if > Feb (Jan == 0). */
		if (hasleapday( t->tm_year )) {
			accum += 1;
		}
	}

	/* Days this year. */
	accum += days_in_month[t->tm_mon];	/* Days since Jan 1. */
	/* accum += t->tm_mday - 1; */			/* Days this month. */
	accum += t->tm_mday;			/* Days this month. */

	/* And the rest..
	 */
	accum *= 24;				/* 24 hours/day */
	accum += t->tm_hour;
	accum *= 60;				/* 60 minutes/hour */
	accum += t->tm_min;
	accum *= 60;				/* 60 seconds/minute */
	accum += t->tm_sec;

	return accum;
}
