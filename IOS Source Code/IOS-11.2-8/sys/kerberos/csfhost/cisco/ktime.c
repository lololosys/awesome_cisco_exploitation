/* $Id: ktime.c,v 3.2.60.1 1996/04/19 15:12:18 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/ktime.c,v $
 *------------------------------------------------------------------
 * ktime.c - Mapping of Kerberos time to our internal NTP implementation
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktime.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:18  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/16  03:54:39  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.2  1996/04/02  08:56:53  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:55  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <csfc5/header.h>
#include "../os/clock.h"
#include "../ntp/ntp_fp.h"
#include "../ntp/ntp.h"

static ulong old_secs, old_usecs;	/* Value we last returned */

/*
 * csfC5_ustimeofday()
 *	Return sec/usec timer pair.
 */
csf_sts
csfCusTOD (csf_ui32 *seconds, csf_ui32 *microseconds)
{
    ulong secs, usecs;
    secs_and_nsecs now;

    secs_and_nsecs_since_jan_1_1970(&now);
    secs = now.seconds;
    usecs = now.nanoseconds / 1000;

    /*
     * Return success with the time values
     */
    *seconds = secs;
    *microseconds = usecs;

    return(CSF_STS_OK);
}

/*
 * csfC5_uustimeofday()
 *	Return sec/usec timer pair.  Guarantee returned value is unique.
 */
csf_sts
csfCuusTOD (csf_ui32 *seconds, csf_ui32 *microseconds)
{
    ulong secs, usecs;
    secs_and_nsecs now;

    secs_and_nsecs_since_jan_1_1970(&now);
    secs = now.seconds;
    usecs = now.nanoseconds / 1000;
    if ((secs == old_secs) && (usecs == old_usecs)) {
	usecs += 1L;
	if (usecs >= 1000000) {
	    secs += 1L;
	    usecs = 0L;
	}
    }
    old_secs = secs;
    old_usecs = usecs;

    /*
     * Return success with the time values
     */
    *seconds = secs;
    *microseconds = usecs;

    return(CSF_STS_OK);
}

/*
 * csf_gmtime1()
 *	Return Kerberos version of gmtime
 *
 * Unlike UNIX gmtime(), this one returns a malloc()'ed value to allow
 * it to be reentrant.
 */
struct tm *
csf_gmtime1(ulong *valp)
{
    struct timeval_ tv;
    struct tm *tm = NULL;
    clock_epoch epoch;

    /*
     * Just dump the seconds into an NTP struct and convert to timeval
     */

    unix_time_to_epoch(*valp, &epoch);
    clock_epoch_to_timeval(&epoch, &tv, 0L);

    /*
     * Convert to struct tm, and away we go
     */
    tm = malloc_named(sizeof(struct tm), "Kerberos:csf_gmtime1()");
    if (tm == NULL) {
	return(NULL);
    }
    tm->tm_year = tv.year;
    tm->tm_mon = tv.month;
    tm->tm_mday = tv.day;
    tm->tm_hour = tv.hour;
    tm->tm_min = tv.minute;
    tm->tm_sec = tv.second;
    tm->tm_wday = tv.day_of_week;
    tm->tm_yday = tv.day_of_year;
    tm->tm_isdst = 0;
    return(tm);
}

/*
 * cisco_kerb_time()
 *	Put down a "since the 70's" timestamp
 */
csf_time_t
cisco_kerb_time(csf_time_t *tp)
{
    secs_and_nsecs now;

    secs_and_nsecs_since_jan_1_1970(&now);
    if (tp) {
	*tp = now.seconds;
    }
    return(now.seconds);
}
