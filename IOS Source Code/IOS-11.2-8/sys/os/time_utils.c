/* $Id: time_utils.c,v 3.9.8.2 1996/08/20 00:18:30 tylin Exp $
 * $Source: /release/112/cvs/Xsys/os/time_utils.c,v $
 *------------------------------------------------------------------
 * Utility routines for manipuating/printing millisecond based time
 * values.
 *
 * September 1995, David Hampton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: time_utils.c,v $
 * Revision 3.9.8.2  1996/08/20  00:18:30  tylin
 * CSCdi64738:  IGMP uptime gets incorrectly formatted
 *    Avoid sign extension.
 * Branch: California_branch
 *
 * Revision 3.9.8.1  1996/03/18  21:31:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.4  1996/03/16  07:21:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.12.3  1996/03/09  05:10:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.12.2  1996/03/07  10:20:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  16:42:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/29  01:00:50  hampton
 * CSCdi50129:  timer_add_delta needs to insure non-zero result
 *
 * Revision 3.8  1996/02/21  01:20:40  hampton
 * Catch bad offsets to passive timer routines.  [CSCdi49419]
 *
 * Revision 3.7  1996/02/08  23:10:12  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/01/04  07:36:41  hampton
 * Remove checks for a timestamp value rolling over to zero.  This only
 * happens once every 585 million years with a 64 bit timestamp.
 * [CSCdi46404]
 *
 * Revision 3.5  1995/12/19  00:43:03  hampton
 * Trim some more memory usage out of the timers code.  [CSCdi45208]
 *
 * Revision 3.4  1995/11/28  01:16:51  hampton
 * Implement code review changes for timer system diet.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/23  06:50:39  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.2  1995/11/17  00:19:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/25  19:37:45  hampton
 * New file for Arkansas.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "mgd_timers.h"
#include "plural.h"

/*
 * Definitions for printing times
 */

typedef struct timetype_ {
    ulonglong years;
    ulonglong weeks;
    ulonglong days;
    ulonglong hours;
    ulonglong minutes;	
    ulonglong seconds;
    ulonglong milliseconds;
} timetype;



/**********************************************************************
 *
 *		     READING / WRITING TIMESTAMPS
 *
 **********************************************************************/

void pak_get_ts (void *addr, void *timestamp)
{
    GETLONGLONG((ulonglong*)addr, timestamp);
}

void pak_put_ts (
    void *addr,
    void *timestamp)
{
    PUTLONGLONG((ulonglong*)addr, timestamp);
}


/*
 * time_normalize
 *
 * Given a timetype field, normalize all the values in the field.  This
 * routine is generally called with a single value in the milliseconds or
 * seconds field, and it then converts the value to seconds, minutes,
 * hours, days, weeks, and years.
 */
static void time_normalize (timetype *time)
{
    time->seconds += time->milliseconds/1000; time->milliseconds %= 1000;
    time->minutes += time->seconds/60; time->seconds %= 60;
    time->hours += time->minutes/60; time->minutes %= 60;
    time->days += time->hours/24; time->hours %= 24;
    time->years += time->days/365; time->days %= 365;
    time->weeks += time->days/7; time->days %= 7;
}

/*
 * timestamp_printf
 *
 * printf handler for 'T' format descriptor -- format a timestamp
 *
 * This routine absorbs one character from the format string and
 * (generally) a single parameter.
 *
 * The printing chatacters supported:
 *  [Aa] An absolute time
 *       - Arguments is a sys_timestamp (64 bits)
 *       - Prints the numerical value of the timestamp
 *  [Cc] A delta time in centiseconds
 *       - Argument is a ulong (32 bits)
 *       - Prints the specified number of centiseconds
 *  [Dd] A delta time
 *       - Argument is a sys_deltatime (64 bits)
 *       - Prints the specified number of milliseconds
 *  [Ee] An elapsed time
 *       - Arguments is a sys_timestamp (64 bits)
 *       - Prints the elapsed time since the timestamp
 *  [Ff] A future time
 *       - Arguments is a sys_timestamp (64 bits)
 *       - Prints the time remaining until the timestamp
 *  [Gg] A future time of a managed timer
 *       - Arguments is a pointer to a managed timer
 *       - Prints the time remaining until the timestamp
 *  [Mm] A delta time in milliseconds
 *       - Argument is a ulong (32 bits)
 *       - Prints the specified number of milliseconds
 *  [Nn] The time now
 *       - No argument
 *       - Prints the numerical value of the current time
 *  [Ss] A delta time in centiseconds
 *       - Argument is a ulong (32 bits)
 *       - Prints the specified number of seconds
 *
 * Other modifiers supported:
 *   nn  (field width)
 *    -  (right justification)
 *    #  (alternate format)
 *    l  (include milliseconds [some formats])
 *
 * The full set of combinations for a particular format includes both
 * the upper and lower case letter, and the formatting characters '#' and
 * 'l'.  In general, the uppercase letter requests a formatted time,
 * while the lowercase letter requests a raw number.  The 'l' modifier
 * requests that milliseconds be tacked onto the number of seconds.  As
 * an example, Here is the full set of combinations for the [Ee] pair.
 *
 *     %TE 	   hh:mm:ss      XdYYh  XwYd  XyYw
 *     %lTE	   hh:mm:ss.mmm  XdYYh  XwYd  XyYw
 *     %#TE	m  hh:mm         XdYYh  XwYd  XyYw
 *
 *     %Te 	s
 *     %lTe 	s.ms
 *     %#Te	ms
 */

#define TIMESTAMP_PRINTF_LEN 80		/* More than big enough. */
static int timestamp_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
			     boolean rjust, int width, int size, int hash,
			     int precision)
{
    char out_string[TIMESTAMP_PRINTF_LEN];
    sys_timestamp timestamp;
    timetype time;
    ulong length;
    ulong max_length;
    int char_count;
    boolean numerical = FALSE;
    mgd_timer *mtimer;

    /* Format the string. */

    max_length = TIMESTAMP_PRINTF_LEN;
    if (width != -1)
	max_length = min(max_length, width);

    /* The next character is a descriptor.  Process it. */
    
    memset(&time, 0, sizeof(time));
    switch (*(*fmt)++) {

      /* A - print the absolute value of the given timestamp. */
      case 'a':
	numerical = TRUE;
      case 'A':
	timestamp = va_arg(*argp, sys_timestamp);
	time.milliseconds = TS_VALUE(timestamp);
	break;

      /* C - print a delta time, given a 32 bit number in centiseconds. */
      case 'c':
	numerical = TRUE;
      case 'C':
	time.milliseconds = va_arg(*argp, ulong) * SYSUPTIME_SCALE;
	break;

      /* D - print a delta time, given a 64 bit number in milliseconds. */
      case 'd':
	numerical = TRUE;
      case 'D':
	time.milliseconds = va_arg(*argp, sys_deltatime);
	break;

      /* E - print the time elapsed since the given timestamp. */
      case 'e':
	numerical = TRUE;
      case 'E':
	timestamp = va_arg(*argp, sys_timestamp);
	if (!TIMER_RUNNING(timestamp)) {
	    strcpy(out_string, "never");
	    length = 5;
	    goto override;
	}
	time.milliseconds = elapsed_time64(&timestamp);
	break;

      /* F - Print the time remaining until the given timestamp. */
      case 'f':
	numerical = TRUE;
      case 'F':
	timestamp = va_arg(*argp, sys_timestamp);
	if (!TIMER_RUNNING(timestamp)) {
	    strcpy(out_string, "never");
	    length = 5;
	    goto override;
	}
	time.milliseconds = time_left_sleeping64(&timestamp);
	break;

      /* G - Print the time remaining until the given managed timer expires. */
      case 'g':
	numerical = TRUE;
      case 'G':
	mtimer = va_arg(*argp, mgd_timer*);
	timestamp = mgd_timer_exp_time(mtimer);
	if (!TIMER_RUNNING(timestamp)) {
	    strcpy(out_string, "never");
	    length = 5;
	    goto override;
	}
	time.milliseconds = TIME_LEFT_SLEEPING64(timestamp);
	break;

      /* M - print a delta time, given a 32 bit number in milliseconds. */
      case 'm':
	numerical = TRUE;
      case 'M':
	time.milliseconds = va_arg(*argp, ulong);
	break;

      /* N - print the current value of the system clock. */
      case 'n':
	numerical = TRUE;
      case 'N':
	GET_TIMESTAMP(timestamp);
	time.milliseconds = TS_VALUE(timestamp);
	break;

      /* S - print a delta time, given a 32 bit number in seconds. */
      case 's':
	numerical = TRUE;
	/* Numerical always needs ms field filled in. */
	time.milliseconds = va_arg(*argp, ulong) * ONESEC;
	break;
      case 'S':
	time.seconds = va_arg(*argp, ulong);
	break;

      /* Unknown--just eat it. */
      default:
	return(0);
    }

    if (numerical) {
	if (hash) {		/* Print raw numbers? */
	    length = sprintf(out_string, "0x%llx", time.milliseconds);
	} else {
	    time.seconds = time.milliseconds / ONESEC;
	    time.milliseconds = time.milliseconds % ONESEC;
	    length = sprintf(out_string, "%llu", time.seconds);
	    if (size) {
		length += sprintf(out_string+length, ".%03llu", time.milliseconds);
	    }
	}
    } else {
	time_normalize(&time);
	if (time.years) {
	    length = sprintf(out_string, "%lluy%lluw", time.years, time.weeks);
	} else if (time.weeks) {
	    length = sprintf(out_string, "%lluw%llud", time.weeks, time.days);
	} else if (time.days > 0) {
	    length = sprintf(out_string, "%llud%-02lluh", time.days, time.hours);
	} else {
	    if (hash) {
		if (time.hours > 0) {
		    length = sprintf(out_string, "%-02llu:%-02llu", time.hours,
			     time.minutes);
		} else {
		    length = sprintf(out_string, "%llu", time.minutes);
		}
	    } else {
		length = sprintf(out_string, "%-02llu:%-02llu:%-02llu", time.hours,
				 time.minutes, time.seconds);
		if (size) {
		    length += sprintf(out_string+length, ".%03llu", time.milliseconds);
		}
	    }
	}
    }
    
  override:
    /*
     * If right justification and a width is specified, emit the appropriate
     * number of blanks.
     */
    if (rjust && (width != -1) && (length < width)) {
	for (char_count = 0; char_count < (width - length); char_count++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	}
    }

    /* Now do the string as appropriate. */
    if (*cp) {
	strcpy(*cp, out_string);
	*cp += length;
    } else {
	_ttyprintf(tty, "%s", out_string);
    }

    /* Now put in trailing blanks as appropriate. */

    if (!rjust && (width != -1) && (length < width)) {
	for (char_count = 0; char_count < (width-length); char_count++) {
	    doprintc(cp, tty, ' '); /* trailing fill - spaces*/
	}
    }

    return((width == -1) ? length : width);
}

/*
 * timestamp_printf_init
 *
 * Register our whizzy printf routine.
 */
void timestamp_printf_init (void)
{
    reg_add_printf('T', timestamp_printf, "timestamp_printf");
}

/*
 * print_uptime
 *
 * Print system uptime.
 */
void print_uptime (void)
{
    sys_timestamp now;
    timetype time;

    printf("\n%s uptime is ", hostname);

    memset(&time, 0, sizeof(timetype));
    GET_TIMESTAMP(now);
    time.milliseconds = TS_VALUE(now);
    time_normalize(&time);
    if (time.years > 0)
	printf("%llu year%s, ", ArgAndPlural(time.years, "", "s"));
    if (time.weeks > 0)
	printf("%llu week%s, ", ArgAndPlural(time.weeks, "", "s"));
    if (time.days > 0)
	printf("%llu day%s, ", ArgAndPlural(time.days, "", "s"));
    if (time.hours > 0)
	printf("%llu hour%s, ", ArgAndPlural(time.hours, "", "s"));
    printf("%llu minute%s", ArgAndPlural(time.minutes, "", "s"));
}

/*
 * print_dhms, sprint_dhms
 *
 * Print the time elapsed since the time value of 'ticks'.  This interval
 * will be printed as "h:mm:ss", "dd:hh", "XwYd", or "XyYw".  If the
 * timer has not been started, the value will be printed as "never".
 */
void print_dhms (sys_timestamp time_in_past)
{
    printf("%TE", time_in_past);
}

int sprint_dhms (char *instring, sys_timestamp time_in_past)
{
    return(sprintf(instring, "%TE", time_in_past));
}

/*
 * print_ticks_in_dhms, sprint_ticks_in_dhms
 *
 * Print the time value of 'ticks'.  This interval will be printed as
 * "dd:hh", "hh:mm", or "XwYd".
 */
void print_ticks_in_dhms (long delta_time32)
{
    sys_deltatime delta_time64;

    delta_time64 = delta_time32 & MAXULONG;
    printf("%TD", delta_time64);
}

int sprint_ticks_in_dhms (char *instring, long delta_time32)
{
    sys_deltatime delta_time64;

    delta_time64 = delta_time32 & MAXULONG;
    return(sprintf(instring, "%TD", delta_time64));
}
