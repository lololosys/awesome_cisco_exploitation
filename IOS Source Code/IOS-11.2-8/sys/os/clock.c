/* $Id: clock.c,v 3.4.54.4 1996/07/10 22:21:44 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/clock.c,v $
 *------------------------------------------------------------------
 * Basic system clock support
 *
 * March 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clock.c,v $
 * Revision 3.4.54.4  1996/07/10  22:21:44  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.54.3  1996/07/05  19:01:54  snyder
 * CSCdi62128:  change some char*[] to const and save on data space
 * Branch: California_branch
 *         Gets 244 bytes out of data segment and into text.
 *
 * Revision 3.4.54.2  1996/06/24  19:06:29  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Missed some files.
 *
 * Revision 3.4.54.1  1996/03/18  21:29:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/09  05:08:59  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.20.2  1996/03/07  10:17:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/01/24  22:36:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/01  05:22:47  dkatz
 * CSCdi43033:  Summer time epochs may not be updated
 *
 * Revision 3.3  1995/11/17  18:45:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/20  04:49:32  dkatz
 * CSCdi42297:  Crash in vines time set-system
 * Tasmanian clocks don't need extra twiddling.
 *
 * Revision 2.5  1995/09/02  02:34:54  dkatz
 * CSCdi39760:  Need function to access timezone string
 * Add a function to do so.
 *
 * Revision 2.4  1995/07/14  02:04:59  dkatz
 * CSCdi37089:  Interface routine needed to obtain timezone offset
 *
 * Revision 2.3  1995/06/16  04:29:09  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/11  23:29:45  dkatz
 * CSCdi35384:  ICMP timestamp packets have bad checksums
 * Add a routine to return the current time in ICMP timestamp form.
 *
 * Revision 2.1  1995/06/07  21:53:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "../ip/ip.h"
#include "clock.h"
#include "clock_internal.h"
#include "sys_registry.h"

int sys_tz = DEFAULT_TZ_OFFSET;
char *sys_tz_name = DEFAULT_TZ_NAME;
static boolean clock_has_been_set;
boolean calendar_is_valid;
static clock_source clock_time_source;  /* Source of last time update */

recur recur_start, recur_end;

boolean summer_time_recurring;	/* True if recurring summer time */
boolean summer_time_enabled;	/* True if summer time is enabled */
boolean summer_time_active;	/* True if summer time is currently on */
long summer_time_offset = 0;	/* Offset for summer time */
clock_epoch summer_time_start;	/* Start of next or current summer time */
clock_epoch summer_time_end;	/* End of next or current summer time */
char *summer_tz_name;		/* Name of time zone in summer time */

int standard_tz = DEFAULT_TZ_OFFSET; /* Standard time zone offset */
char *standard_tz_name = DEFAULT_TZ_NAME; /* Standard time zone name */

const char *const month_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
			    "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *const day_name[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const char *const long_month_name[] = {"January", "February", "March", "April",
				 "May", "June", "July", "August", "September",
				 "October", "November", "December"};
const char *const long_day_name[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
			       "Thursday", "Friday", "Saturday"};

/*
 * Return an indication as to whether the clock is likely to be accurate.
 */

boolean clock_is_probably_valid (void)
{
    return(clock_has_been_set);
}


/*
 * Set the flag to say that the clock is probably valid.
 */
void clock_is_now_valid (void)
{
    clock_has_been_set = TRUE;
}


/*
 * Return the current time source.
 */
clock_source current_time_source (void)
{
    return(clock_time_source);
}


/*
 * Set the current time source.
 */
void clock_set_time_source (clock_source time_source)
{
    clock_time_source = time_source;
}

/*
 * Return the last day of a given month and year.  This is a cheap hack,
 * but what the hell.  The month and year should be filled into the timeval
 * struct;  the rest of the struct should be zero.  The timeval will be
 * left with the day in tv->day, and the day of the week in tv->day_of_week.
 */
static void last_day_of_month (timeval *tv)
{
    clock_epoch ntp;
    int month;

    /*
     * Convert to NTP format and back.  This will normalize the date.
     * If the month changes, subtract the extra days.
     */
    tv->day = 31;
    month = tv->month;
    clock_timeval_to_epoch(tv, &ntp);
    clock_epoch_to_timeval(&ntp, tv, 0);
    if (tv->month != month) {
	ntp.epoch_secs -= tv->day * SECSPERDAY;
	clock_epoch_to_timeval(&ntp, tv, 0);
    }
}

/*
 * Resolve the epoch given the time, the timezone, the day of the week, 
 * the month, the week number, and the year.
 *
 * Returns the results in NTP format.
 *
 * A week number of "-1" means the last week of the month.
 */

static void resolve_epoch (int hours, int minutes, int tz_offset, int weekday, 
			   int weeknum, int month, int year, clock_epoch *ntp)
{
    int day_offset;
    timeval tv;
    memset(&tv, 0, sizeof(tv));
    tv.month = month;
    tv.year = year;

    /* Play the game differently if the target week is the last. */

    if (weeknum == -1) {	/* Last week of month */

	/* Find out the last day of the month. */
	
	last_day_of_month(&tv);

	/* Adjust downward to the appropriate day of the week. */

	day_offset = tv.day_of_week - weekday;
	if (day_offset < 0)
	    day_offset += DAYSPERWEEK;
	tv.day -= day_offset;

    } else {			/* Numbered week of month */

	/* Find out the day of the week of the first day of the month. */

	tv.day = 1;
	clock_timeval_to_epoch(&tv, ntp);
	clock_epoch_to_timeval(ntp, &tv, 0);
	
	/* Adjust upward to the appropriate day of the week. */

	day_offset = weekday - tv.day_of_week;
	if (day_offset < 0)
	    day_offset += DAYSPERWEEK;
	tv.day += day_offset;

	/* Adjust upward to the appropriate week of the month. */

	tv.day += DAYSPERWEEK * (weeknum - 1);

    }

    /* Now factor in the time and time zone. */

    tv.hour = hours;
    tv.minute = minutes;
    tv.tz_offset = tz_offset;
    clock_timeval_to_epoch(&tv, ntp);
}


/*
 * Calculate target epochs for summer time to start and end
 *
 * Leaves start and end epochs uninitialized if summer time is disabled.
 */

static void do_summer_time_epochs (clock_epoch *current_time,
				   clock_epoch *summer_start,
				   clock_epoch *summer_end)
{
    int start_year, end_year;
    timeval tv;

    /* Don't bother if no summer time. */

    if (!summer_time_enabled)
	return;

    /* If we're not recurring, just use the system epochs. */

    if (!summer_time_recurring) {
	*summer_start = summer_time_start;
	*summer_end = summer_time_end;
	return;
    }

    /* Convert the time to a timeval. */

    clock_epoch_to_timeval(current_time, &tv, clock_timezone_offset());

    /* Calculate the start and end times based on the current year. */

    start_year = end_year = tv.year;

    resolve_epoch(recur_start.hours, recur_start.minutes,
		  standard_tz, recur_start.day, recur_start.week,
		  recur_start.month, start_year, summer_start);
    resolve_epoch(recur_end.hours, recur_end.minutes,
		  standard_tz + summer_time_offset, recur_end.day, 
		  recur_end.week, recur_end.month, end_year, summer_end);

    /*
     * If the end epoch is before the start epoch, we must be in the
     * southern hemisphere.  Subtract a year from the start epoch.
     */
    if (summer_end->epoch_secs < summer_start->epoch_secs) {
	start_year--;
	resolve_epoch(recur_start.hours, recur_start.minutes,
		      standard_tz, recur_start.day, 
		      recur_start.week, recur_start.month, start_year, 
		      summer_start);
    }
    /*
     * Now the start and end are in order.  If the current time is after
     * the end of summer time, bump the year and recalculate.  This will
     * result in the current time being either before or during summer time.
     */
    if (current_time->epoch_secs > summer_end->epoch_secs) {
	start_year++;
	end_year++;
	resolve_epoch(recur_start.hours, recur_start.minutes,
		      standard_tz, recur_start.day, recur_start.week,
		      recur_start.month, start_year, summer_start);
	resolve_epoch(recur_end.hours, recur_end.minutes,
		      standard_tz + summer_time_offset, recur_end.day, 
		      recur_end.week, recur_end.month, end_year, 
		      summer_end);
    }
}

/*
 * Calculate target epochs for summer time to start and end
 *
 * When we have recurring summer time, it is in terms of the "nth"
 * week of the month of any particular year.  We need to resolve this
 * into a start epoch and an end epoch (in terms of UTC).
 *
 * This is further complicated by the fact that, in the southern
 * hemisphere, the start date comes *after* the end date.
 *
 * This routine should be called whenever the clock is changed, and
 * whenever summer time ends.
 *
 * This routine updates summer_time_start and summer_time_end.
 *
 * If "curtime" is non-NULL, we use the time passed to us.  If NULL, we
 * look up the current time.  This is because clock_get_time will ultimately
 * call this routine recursively, so we avoid blowing the stack this way.
 */

void calculate_summer_time_epochs (clock_epoch *curtime)
{
    clock_epoch current_time;

    if (curtime == NULL) {
	clock_get_time(&current_time);
	curtime = &current_time;
    }
    do_summer_time_epochs(curtime, &summer_time_start, &summer_time_end);
}

/*
 * Get the name and timezone offset for standard or summer time.  The
 * parameter is TRUE if we want summer time, false if we want standard
 * time.
 */

void get_summertime_info (boolean summer_time, char **tz_name, long
			  *tz_offset)
{
    if (summer_time && summer_time_enabled) {
	*tz_name = summer_tz_name;
	*tz_offset = standard_tz + summer_time_offset;
	return;
    }

    *tz_name = standard_tz_name;
    *tz_offset = standard_tz;
}

/*
 * Check to see if summer time is in effect, and make appropriate
 * adjustments if so.  This routine needs to be called whenever the
 * time is referenced so that the summer time epochs are kept up to
 * date.
 *
 * This routine assumes that the summer time epochs are consistent with
 * the current time;  that is, that the time has not been stepped since
 * the epochs were calculated, and that the summer time and time zone
 * rules have not been altered.  If these assumptions do not hold true,
 * calculate_summer_time_epochs() should be called directly to get
 * consistency.  (We don't like to call it all the time because it is
 * rather expensive.)
 *
 * If curtime is non-NULL, we base the check on that value.  Otherwise
 * we call clock_get_time to get the current time.  This is effectively
 * a recursion-avoidance kludge, since clock_get_time calls this routine
 * back in order to handle the generic case of asking for the time
 * (this way we don't have to do any periodic checks to see if the time
 * has changed).
 */
void check_for_summer_time (clock_epoch *curtime)
{
    clock_epoch current_time;

    if (!summer_time_enabled)
	return;

    if (curtime == NULL) {
	clock_get_time(&current_time);
	curtime = &current_time;
    }

    /* If we're after the end of summer time, recalculate the epochs. */

    if (curtime->epoch_secs >= summer_time_end.epoch_secs) {
	calculate_summer_time_epochs(curtime);
    }

    /* If we're in the summer time period, make sure things are active. */

    if (curtime->epoch_secs >= summer_time_start.epoch_secs &&
	curtime->epoch_secs < summer_time_end.epoch_secs) {
	summer_time_active = TRUE;
    } else {

	/* Not in summer time.  Deactivate. */

	summer_time_active = FALSE;
    }
    get_summertime_info(summer_time_active, &sys_tz_name, (long *)&sys_tz);
}

/*
 * Get timezone information given a time stamp.  We interpret the summer
 * time rules as necessary.
 */


void clock_get_timezone_info (clock_epoch *ntp_time, char **tz_name, long *tz_offset)
{

    clock_epoch summer_start, summer_end;

    /* Assume standard time. */

    get_summertime_info(FALSE, tz_name, tz_offset);

    /* If no summer time, we're outta here. */

    if (!summer_time_enabled)
	return;

    /* Get the summer time epochs. */

    do_summer_time_epochs(ntp_time, &summer_start, &summer_end);

    /* If it's summer time, use the summer time timezone. */

    if (ntp_time->epoch_secs >= summer_start.epoch_secs &&
	ntp_time->epoch_secs < summer_end.epoch_secs) {
	get_summertime_info(TRUE, tz_name, tz_offset);
    }
}

/*
 * Get timezone information given a time stamp.  We interpret the summer
 * time rules as necessary.
 */

boolean clock_time_is_in_summer (clock_epoch *ntp_time)
{

    clock_epoch summer_start, summer_end;

    /* If no summer time, we're outta here. */

    if (!summer_time_enabled)
	return(FALSE);

    /* Get the summer time epochs. */

    do_summer_time_epochs(ntp_time, &summer_start, &summer_end);

    /* If it's summer time, use the summer time timezone. */

    if (ntp_time->epoch_secs > summer_start.epoch_secs &&
	ntp_time->epoch_secs <= summer_end.epoch_secs)
	return(TRUE);
    return(FALSE);
}

/*
 * Guts of calendar reading
 */
void do_read_calendar (void)
{
    timeval tv;
    clock_epoch ntp;

    reg_invoke_ntp_read_calendar(&tv);
    tv.tz_offset = 0;		/* Calendar runs on UTC */
    clock_timeval_to_epoch(&tv, &ntp);
    clock_set(&ntp, CLOCK_SOURCE_CALENDAR);
}

/*
 * Read the calendar right now
 */
void clock_read_calendar_exec (parseinfo *csb)
{
    do_read_calendar();
}

/*
 * Guts of calendar update
 */
void do_calendar_update (void)
{
    clock_epoch ntp_time;
    timeval tv;

    clock_get_time(&ntp_time);
    clock_epoch_to_timeval(&ntp_time, &tv, 0); /* Calendar runs on UTC */
    reg_invoke_ntp_write_calendar(&tv);
}

/*
 * Update the hardware calendar right now
 *
 */
void clock_update_calendar_exec (parseinfo *csb)
{
    do_calendar_update();
}

/*
 * Initialize the clock system
 */
void clock_init (void)
{

    clock_has_been_set = FALSE;
    calendar_is_valid = FALSE;
    clock_set_time_source(CLOCK_SOURCE_NONE);

    /* Initialize the kernel clock */

    init_kernel_clock();
}

/*
 * clock_icmp_time
 *
 * Return ICMP-format timestamp (milliseconds since 0000 UT).  Sets the
 * high order bit if we're unsure of the time.
 *
 * We try to use precise time here.
 */
ulong clock_icmp_time (void)
{
    ulong result, seconds_since_midnight;
    clock_epoch curtime;
    timeval tv;

    /* Get the time;  convert to a timeval. */

    clock_get_time_exact(&curtime);
    clock_epoch_to_timeval(&curtime, &tv, 0);

    /* Now figure out how long ago midnight was. */

    seconds_since_midnight = curtime.epoch_secs % SECSPERDAY;

    /* Convert to milliseconds, throwing in the low order bits. */

    result = (seconds_since_midnight * MILLISECSPERSEC) +
	tv.millisecond;
    if (!clock_is_probably_valid())
	result |= ICMP_TIMESTAMPINVALID;

    return(result);
}


/*
 * clock_icmp_time_to_timeval
 *
 * Returns an ICMP timestamp in timeval format.  It is up to the caller
 * to interpret the INVALID bit and do the right thing.
 *
 * The value returned is adjusted for the supplied timezone.  Only the
 * hour/minute/second/millisecond fields are valid.
 */
void clock_icmp_time_to_timeval (ulong time, int tz_offset, timeval *tvptr)
{
    ulong seconds, minutes, hours;
 
    time &= ~ICMP_TIMESTAMPINVALID;	/* mask off the pesky bit */
    time += SECSPERDAY * MILLISECSPERSEC; /* Ensure that we stay positive */
    time += tz_offset * MILLISECSPERSEC; /* Adjust for the timezone */
    tvptr->millisecond = time % MILLISECSPERSEC;
    seconds = time / MILLISECSPERSEC;
    tvptr->second = seconds % SECSPERMIN;
    minutes = seconds / SECSPERMIN;
    tvptr->minute = minutes % MINSPERHR;
    hours = minutes / MINSPERHR;
    tvptr->hour = hours % HRSPERDAY;
}

/*
 * See if the caller is allowed to set the time.  This code establishes
 * the pecking order for time services.
 */
static boolean time_setting_allowed (clock_source new_source)
{
    switch (current_time_source()) {

	/* Current is NTP. Allow manual and NTP time setting. */

    case CLOCK_SOURCE_NTP:
	switch (new_source) {
	case CLOCK_SOURCE_NTP:
	case CLOCK_SOURCE_MANUAL:
	    return(TRUE);
	default:
	    return(FALSE);
	}
	break;

	/* Current is SNTP. Allow manual, NTP, and SNTP time setting. */

    case CLOCK_SOURCE_SNTP:
	switch (new_source) {
	case CLOCK_SOURCE_NTP:
	case CLOCK_SOURCE_SNTP:
	case CLOCK_SOURCE_MANUAL:
	    return(TRUE);
	default:
	    return(FALSE);
	}
	break;

	/* Anybody else:  allow anybody to set the time. */

    case CLOCK_SOURCE_NONE:
    case CLOCK_SOURCE_MANUAL:
    case CLOCK_SOURCE_CALENDAR:
    case CLOCK_SOURCE_VINES:
    default:
	return(TRUE);
    }
}


/*
 * Set the current time based on seconds since 1/1/1970.  For outside
 * callers.  Returns whether or not the clock was set (i.e., whether
 * the clock precedence rules allow us to set the clock).
 */
boolean clock_set_unix (ulong unixtime, clock_source source)
{
    clock_epoch epoch;
    if (!time_setting_allowed(source))
	return(FALSE);

    unix_time_to_epoch(unixtime, &epoch);
    clock_set(&epoch, source);
    return(TRUE);
}
    
/*
 * This routine returns the time in seconds since January 1, 1970,
 * for those folks that like Un*x.  This time is always UTC relative.
 */
ulong unix_time (void)
{
    clock_epoch curtime;
    clock_get_time(&curtime);
    return(clock_epoch_to_unix_time(&curtime));
}

/*
 * This routine returns the time in seconds and nanoseconds since January
 * 1, 1970, for DNSIX.  Always UTC relative.  Truncation error yields
 * millisecond accuracy, but that's tough.
 */
void secs_and_nsecs_since_jan_1_1970 (secs_and_nsecs *result)
{
    clock_epoch curtime;
    clock_get_time(&curtime);
    result->seconds = clock_epoch_to_unix_time(&curtime);
    result->nanoseconds = (curtime.epoch_frac / (1<<16)) *
	(1000000000/(1<<16));
}


/*
 * Return the system uptime.  This value is guaranteed not to roll
 * over for 136 years.
 */
ulong system_uptime_seconds (void)
{
    return(secs_since_boot);
}


/*
 * Return the current timezone offset in integer seconds.
 */
int clock_timezone_offset (void)
{
    return(sys_tz);
}

/*
 * clock_timezone_name
 *
 * Returns a pointer to the current local timezone name.
 */
char *clock_timezone_name (void)
{
    return(sys_tz_name);
}
