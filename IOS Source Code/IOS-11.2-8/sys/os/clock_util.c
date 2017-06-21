/* $Id: clock_util.c,v 3.3.62.4 1996/09/06 01:11:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/clock_util.c,v $
 *------------------------------------------------------------------
 * Clock utility routines
 *
 * Dave Katz, December 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clock_util.c,v $
 * Revision 3.3.62.4  1996/09/06  01:11:24  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/07/10  22:21:49  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/05/09  14:39:45  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.62.1.18.1  1996/04/27  07:16:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.62.1.8.1  1996/04/08  01:59:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.62.1  1996/03/18  21:29:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.66.1  1996/04/05  03:19:15  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3  1995/11/17  18:46:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:18:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/07/12  01:11:35  dkatz
 * CSCdi36947:  day-in-year value incorrectly computed
 *
 * Revision 2.4  1995/06/22  00:17:05  dkatz
 * CSCdi36255:  malloc() called from _doprnt thread
 * Use a stack variable instead.
 *
 * Revision 2.3  1995/06/21  08:56:09  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/16  04:29:16  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:54:10  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "../os/clock_internal.h"
#include "sys_registry.h"

/*
 * Return a time string given the time in NTP format.  The time zone is
 * interpreted according to the summer time rules in effect.
 *
 * The string is returned in the form "13:23:45.123 MET-DST Thu Jun 11 1995".
 * The timezone string is truncated to seven characters.
 *
 * The second parameter says whether or not milliseconds should be included
 * in the time string.
 *
 * The third parameter says how to handle summer time calculations:
 *	CALCULATE_SUMMER_TIME--use the provided timestamp to decide whether
 *		summer time is active or not.  This should be used with
 *		random timestamps since they don't have anything to do
 *		with the current system time.
 *	USE_STANDARD_TIME--always interpret in standard time.
 *	USE_SUMMER_TIME--always interpret in summer time.
 */
void local_time_string (char *buf, clock_epoch *epoch, boolean milliseconds, 
			summer_time_selection_type summer_type)
{
    timeval tv;
    char *tz_name;
    long tz_offset;
    char *format_string;

    /* Get the time zone information. */

    switch (summer_type) {

    case CALCULATE_SUMMER_TIME:
	clock_get_timezone_info(epoch, &tz_name, &tz_offset);
	break;

    case USE_STANDARD_TIME:
	get_summertime_info(FALSE, &tz_name, &tz_offset);
	break;

    case USE_SUMMER_TIME:
	get_summertime_info(TRUE, &tz_name, &tz_offset);
	break;
    }

    /* Do the conversion and formatting. */

    clock_epoch_to_timeval(epoch, &tv, tz_offset);
    if (milliseconds) {
	format_string = "%T.%k %tZ %tw %E";
    } else {
	format_string = "%T %tZ %tw %E";
    }
    format_time(buf, LOCAL_TIME_STRING_MINBUF, format_string, &tv, tz_name);
}

/*
 * current_time_string
 *
 * Return the current date and time in the form:
 *    13:23:45 MET-DST Thu Jun 11 1995
 *
 * The string is guaranteed to be <= CURRENT_TIME_STRING_MINBUF bytes in
 * length, including the trailing NUL.  The timezone string will be
 * truncated to seven characters.
 */
void current_time_string (char *buf)
{
    clock_epoch epoch;

    /* Get the current time. */

    clock_get_time(&epoch);

    /* Do the format. */

    sprintf(buf, "%CC", "%G", &epoch);
}

/*
 * unix_time_string
 *
 * Convert a Unix-format time to a time string.  Time is in the form:
 *    13:45:21 MET-DST Jun 11 1995
 *
 * The returned string is guaranteed to be <= UNIX_TIME_STRING_MINBUF bytes
 * including the trailing NUL.
 *
 * Time is always converted according to the local time zone.
 */
void unix_time_string(char *buf, ulong unix_time)
{
    clock_epoch epoch;

    /* Convert unix time to a time epoch. */

    unix_time_to_epoch(unix_time, &epoch);

    /* Now format and return. */

    sprintf(buf, "%CC", "%T %tZ %E", &epoch);
}

#ifdef NOBODY_CALLS_ME
/*
 * Update the time zone in a timeval (adjusting the time and date).
 */
void update_time_zone(timeval *tv, long new_tz_offset)
{
    clock_epoch epoch;

    /* Cheap hack.  Convert it to an epoch and back. */

    clock_timeval_to_epoch(tv, &epoch);
    clock_epoch_to_timeval(&epoch, tv, new_tz_offset);
}
#endif /* NOBODY_CALLS_ME */


/*
 * unix_time_is_in_summer
 *
 * Returns TRUE if the supplied unix-format time lies within the summer
 * time period.
 */
boolean unix_time_is_in_summer (ulong unix_time)
{
    clock_epoch epoch;

    /* Convert unix time to an epoch. */

    unix_time_to_epoch(unix_time, &epoch);

    return(clock_time_is_in_summer(&epoch));
}

/*
 * unix_time_to_timeval
 *
 * Break a unix time value into a timeval, based on the local time zone.
 *
 * Returns the timezone string as well.
 */
void unix_time_to_timeval (ulong unix_time, timeval *tv, char **tz_name)
{
    clock_epoch epoch;
    long tz_offset;

    /* Convert unix time to an epoch. */

    unix_time_to_epoch(unix_time, &epoch);

    /* Get time zone information. */

    clock_get_timezone_info(&epoch, tz_name, &tz_offset);

    clock_epoch_to_timeval(&epoch, tv, tz_offset);
}


/*
 * unix_time_string_2
 *
 * Convert a Unix-format time to a time string.  The returned time is always
 * exactly UNIX_TIME_STRING_2_MINBUF bytes in length, including the trailing
 * NUL, in the form:
 *       "MMM DD YYYY HH:MM:SS"  (Example: "Mar 11 1960 08:47:00")
 *
 * The string is returned in the buffer supplied by the caller.
 */
void unix_time_string_2 (char *date_time_string, ulong unix_time)
{
    clock_epoch epoch;

    if (unix_time == 0) {
	strcpy(date_time_string,"--- -- ---- --:--:--");
    } else {
	unix_time_to_epoch(unix_time,&epoch);
	sprintf(date_time_string, "%CC", "%tn %d %Y %T", &epoch);
    }
}


/*
 * format_time
 *
 * Format a time string given a timeval structure and a formatting string.
 *
 * The null-terminated string is placed in the buffer provided and the
 * byte count (not including the NUL character) is returned.  If the 
 * result doesn't fit in the provided space, a value of zero is returned
 * and the buffer contains a null string (a single NUL character).
 *
 *
 * The formatting string consists of printf-like descriptors consisting
 * of a "%", an optional single character formatting descriptor, and a single
 * character field descriptor.  Other characters are copied verbatim.
 *
 * The formatting descriptors are:
 *	    b    Left blank fill numeric fields to "natural" field width
 *          t    Truncate fields to their minimum width
 *
 * By default, numbers are zero-filled to their "natural" width and
 * day/month names are fully spelled out.  When truncated, Day/month names are
 * three characters long, and time zone strings are at most seven characters.
 *
 * The field descriptors are:
 *
 *    (Basic descriptors)
 *
 *          %v   clock validity flag ("*", ".", or null)
 *
 *          %H   hour (00-23)
 *
 *          %i   hour (01-12)
 *
 *          %M   minute (00-59)
 *
 *          %S   second (00-59)
 *
 *          %k   millisecond (000-999)
 *
 *          %p   AM/PM
 *
 *          %Z   time zone string
 *
 *          %w   day of week (Sunday)
 *
 *          %m   month number (01-12)
 *
 *          %n   month name (January)
 *
 *          %d   day of month (01-31)
 *
 *          %y   year within century (00-99)
 *
 *          %Y   year, including century (1995)
 *
 *          %j   julian day of year (001-366)
 *
 *          %%   same as %
 *
 *     (Compound descriptors)
 *
 *          %D   date as 06/01/95
 *
 *          %E   date as Jun 1 1995
 *
 *          %F   date as June 1, 1995
 *
 *          %T   time as 13:45:21
 *
 *          %U   time as 05:45:59 PM
 *
 *          %B   date and time (13:23:45.123 Pacific-Daylight Thu Jun 1 1995)
 *
 *          %C   date and time (13:23:45 Pacific-Daylight Thu Jun 1 1995)
 *
 *          %G   date and time, truncated timezone (guaranteed max length)
 *		          (13:23:45 MET-DST Thu Jun 11 1995)
 *
 */
static const char pct_B_string[] = "%H:%M:%S.%k %Z %tw %tn %td %Y";
static const char pct_C_string[] = "%H:%M:%S %Z %tw %tn %td %Y";
static const char pct_D_string[] = "%m/%d/%y";
static const char pct_E_string[] = "%tn %td %Y";
static const char pct_F_string[] = "%n %td, %Y";
static const char pct_G_string[] = "%H:%M:%S %tZ %tw %tn %td %Y";
static const char pct_T_string[] = "%H:%M:%S";
static const char pct_U_string[] = "%i:%M:%S %p";

#define DAYMONTH_TRUNCATED_WIDTH 3		/* "Tue", "Jan" */
#define TIMEZONE_TRUNCATED_WIDTH 7		/* "MET-DST" */

ulong format_time (char *buf, ulong buf_len, const char *fmt, timeval *time,
		   char *timezone_string)
{
    const char *save_fmt;
    const char *copy_string;
    char *buf_ptr;
    ulong string_width;			/* 0 if not copying a string */
    boolean truncated;
    boolean blanked;
    boolean expecting_descriptor;
    ulong number_to_convert;
    ulong conversion_width;		/* 0 if not converting a number */
    boolean recursing;
    ulong space_remaining;

    buf_ptr = buf;
    expecting_descriptor = FALSE;
    blanked = FALSE;
    truncated = FALSE;
    recursing = FALSE;
    save_fmt = NULL;
    conversion_width = 0;
    number_to_convert = 0;
    copy_string = NULL;
    string_width = 0;

    if (buf_len == 0)
	space_remaining = MAXUINT;  /* zero length means unlimited */
    else
	space_remaining = buf_len - 1;

    /* Top of main loop. */

    while (TRUE) {

	/*
	 * If the descriptor is not next, look for a "%", and otherwise
	 * copy the character.  Clear various flags.
	 */
	if (!expecting_descriptor) {
	    blanked = FALSE;
	    truncated = FALSE;

	    if (*fmt == '%') {		/* Looks like a descriptor */
		expecting_descriptor = TRUE;
	    } else {			/* Nope, just copy it if it fits. */
		copy_string = fmt;
		string_width = 1;
	    }
	} else {

	    /* The next character is a descriptor.  Process it. */

	    switch (*fmt) {

		/* % - put a single % in the buffer. */

	      case '%':
		copy_string = fmt;
		string_width = 1;
		expecting_descriptor = FALSE;
		break;

		/* b - set "blank fill" flag. */

	      case 'b':
		blanked = TRUE;
		break;

		/* B - format really fancy date and time (recurse). */

	      case 'B':
		save_fmt = fmt;
		fmt = pct_B_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* C - format fancy date and time (recurse). */

	      case 'C':
		save_fmt = fmt;
		fmt = pct_C_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* d - format day of month. */

	      case 'd':
		number_to_convert = time->day;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* D - format date (recurse). */

	      case 'D':
		save_fmt = fmt;
		fmt = pct_D_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* E - format date (recurse). */

	      case 'E':
		save_fmt = fmt;
		fmt = pct_E_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* F - format date (recurse). */

	      case 'F':
		save_fmt = fmt;
		fmt = pct_F_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* G - format fancy date and time (recurse). */

	      case 'G':
		save_fmt = fmt;
		fmt = pct_G_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* H - format hour (24 hr clock) */

	      case 'H':
		number_to_convert = time->hour;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* i - format hour (12 hr clock) */

	      case 'i':
		number_to_convert = time->hour % 12;
		if (number_to_convert == 0) /* 00 = 12... */
		    number_to_convert = 12;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* j - format day of year */

	      case 'j':
		number_to_convert = time->day_of_year;
		conversion_width = 3;
		expecting_descriptor = FALSE;
		break;

		/* k - format millisecond */

	      case 'k':
		number_to_convert = time->millisecond;
		conversion_width = 3;
		expecting_descriptor = FALSE;
		break;

		/* m - format month number */

	      case 'm':
		number_to_convert = time->month;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* M - format minute */

	      case 'M':
		number_to_convert = time->minute;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* n - format month name */

	      case 'n':
		copy_string = long_month_name[time->month - 1];
		string_width = strlen(copy_string);
		if (truncated)
		    string_width = min(string_width, DAYMONTH_TRUNCATED_WIDTH);
		expecting_descriptor = FALSE;
		break;

		/* p - format AM/PM */

	      case 'p':
		copy_string = (time->hour >= 12) ? "PM" : "AM";
		string_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* S - format seconds */

	      case 'S':
		number_to_convert = time->second;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* t - set "truncate" flag. */

	      case 't':
		truncated = TRUE;
		break;

		/* T - format time (recurse) */

	      case 'T':
		save_fmt = fmt;
		fmt = pct_T_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* U - format time (recurse) */

	      case 'U':
		save_fmt = fmt;
		fmt = pct_U_string;
		recursing = TRUE;
		expecting_descriptor = FALSE;
		break;

		/* v - format clock validity */

	      case 'v':
		if (!clock_is_probably_valid()) {
		    copy_string = "*";
		    string_width = 1;
		} else if (ntp_clock_status) {
		    copy_string = &ntp_clock_status;
		    string_width = 1;
		}
		expecting_descriptor = FALSE;
		break;

		/* w - format day of week */

	      case 'w':
		copy_string = long_day_name[time->day_of_week];
		string_width = strlen(copy_string);
		if (truncated)
		    string_width = min(string_width, DAYMONTH_TRUNCATED_WIDTH);
		expecting_descriptor = FALSE;
		break;

		/* y - format year within century */

	      case 'y':
		number_to_convert = time->year % YEARSPERCENTURY;
		conversion_width = 2;
		expecting_descriptor = FALSE;
		break;

		/* Y - format year A.D. */

	      case 'Y':
		number_to_convert = time->year;
		conversion_width = 4;
		expecting_descriptor = FALSE;
		break;

		/* Z - format time zone */

	      case 'Z':
		copy_string = timezone_string;
		string_width = strlen(copy_string);
		if (truncated)
		    string_width = min(string_width, TIMEZONE_TRUNCATED_WIDTH);
		expecting_descriptor = FALSE;
		break;
		
		/* Unknown--just eat it. */
		
	      default:
		expecting_descriptor = FALSE;
		break;
	    }
	}

	/* If we have a numeric conversion, do it now. */
	    
	if (conversion_width) {
		
	    /* Bail if there isn't enough space. */

	    if (space_remaining < conversion_width) {
		buf_ptr = buf;	/* Show that we bailed. */
		goto done;
	    }

	    /* Do appropriate conversion based on flags. */

	    if (truncated) {
		conversion_width = sprintf(buf_ptr, "%d",
					   number_to_convert);
	    } else if (blanked) {
		sprintf(buf_ptr, "%-*d", conversion_width,
			number_to_convert);
	    } else {
		sprintf(buf_ptr, "%0*d", conversion_width,
			number_to_convert);
	    }
	    buf_ptr += conversion_width;
	    space_remaining -= conversion_width;
	    conversion_width = 0;

	} else if (string_width) {

	    /* We have a string to copy.  Bail if it won't fit. */

	    if (space_remaining < string_width) {
		buf_ptr = buf;	/* Show that we bailed. */
		goto done;
	    }

	    /* Copy it in. */

	    bcopy(copy_string, buf_ptr, string_width);
	    buf_ptr += string_width;
	    space_remaining -= string_width;
	    string_width = 0;

	}

	/* If we're recursing, prepare for the recursion. */

	if (recursing) {
	    save_fmt++;			/* Advance past the earlier one. */
	    recursing = FALSE;
	} else {

	    /*
	     * Not recursing.  Advance the format string.  If we're at the
	     * end of the format string, pop out of recursion if we're
	     * doing it, or bail if we're done.
	     */
	    fmt++;
	    while (*fmt == '\0') {	/* End of string */
		if (!save_fmt)		/* Not recursing; really done! */
		    goto done;
		fmt = save_fmt;
		save_fmt = NULL;
	    }
	}
    }					/* end while(TRUE) */

    /* Calculate the space used and return it. */

  done:
    *buf_ptr = '\0';
    return ((ulong) buf_ptr - (ulong) buf);
}

/*
 * clock_printf
 *
 * printf handler for 'C' format descriptor--format a clock string
 *
 * This routine absorbs two parameters;  the first is a pointer to
 * the time formatting string (per format_time) and the second is
 * a pointer to a clock_epoch structure containing the timestamp.
 *
 * The time is by default interpreted according to local timezone and
 * summer time rules.  If the "#" modifier is used, the time is formatted
 * as the default (UTC) instead.
 *
 * Other modifiers supported:
 *    nn (field width)
 *    -  (right justification)
 */
#define CLOCK_PRINTF_LEN 80		/* More than big enough. */
static int clock_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
			 boolean rjust, int width, int size, int hash,
			 int precision)
{
    char *fmt_string;
    char out_string[CLOCK_PRINTF_LEN];
    char *result_string;
    clock_epoch *timestamp;
    timeval tv;
    char *tz_name;
    long tz_offset;
    ulong length;
    ulong max_length;
    int char_count;

    /* Get the parameters. */

    fmt_string = va_arg(*argp, char *);
    timestamp = va_arg(*argp, clock_epoch *);

    /* Get time zone information. */

    clock_get_timezone_info(timestamp, &tz_name, &tz_offset);

    /* If hash is specified, use UTC (kludge). */

    if (hash) {
	tz_offset = DEFAULT_TZ_OFFSET;
	tz_name = DEFAULT_TZ_NAME;
    }

    /* Convert to a timeval. */

    clock_epoch_to_timeval(timestamp, &tv, tz_offset);

    /* Format the string. */

    max_length = CLOCK_PRINTF_LEN;
    if (width != -1)
	max_length = min(max_length, width);
    length = format_time(out_string, max_length, fmt_string, &tv, tz_name);
    result_string = out_string;

    /* If it didn't fit, insert a single "*" instead. */

    if (!length && width != 0) {
	result_string = "*";
	length = 1;
    }

    /*
     * If right justification and a width is specified, emit the appropriate
     * number of blanks.
     */
    if (rjust && width != -1 && length < width) {
	for (char_count = 0; char_count < (width - length); char_count++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	}
    }

    /* Now do the string as appropriate. */
    if (*cp) {
	strcpy(*cp, result_string);
	*cp += length;
    } else {
	_ttyprintf(tty, "%s", result_string);
    }

    /* Now put in trailing blanks as appropriate. */

    if (!rjust && width != -1 && (length < width)) {
	for (char_count = 0; char_count < (width-length); char_count++) {
	    doprintc(cp, tty, ' '); /* trailing fill - spaces*/
	}
    }

    return((width == -1) ? length : width);
}

/*
 * clock_register_printf
 *
 * Register our whizzy printf routine.
 */
void clock_register_printf (void)
{
    reg_add_printf('C', clock_printf, "clock_printf");
}

/*
 * clock_epoch_compare
 *
 * Compare to clock_epoch's and <0 if the first is less than the second,
 * 0 if they are equal, and >0 if the first is greater than the second.
 */

int clock_epoch_compare (clock_epoch *a, clock_epoch *b)
{
    if (a->epoch_secs > b->epoch_secs)
	return(1);
    else if (a->epoch_secs == b->epoch_secs)
	if (a->epoch_frac > b->epoch_frac)
	    return(1);
	else if (a->epoch_frac == b->epoch_frac)
	    return(0);
	else
	    return(-1);
    else
	return(-1);
}
