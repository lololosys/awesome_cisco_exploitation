/* $Id: clock.h,v 3.2.62.5 1996/09/06 01:11:21 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/clock.h,v $
 *------------------------------------------------------------------
 * Definitions for general clock support
 *
 * March 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file is for callers that don't need to understand the NTP
 * timestamp format.  Those who do need to include clock_internal.h.
 *------------------------------------------------------------------
 * $Log: clock.h,v $
 * Revision 3.2.62.5  1996/09/06  01:11:21  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/07/19  01:33:29  snyder
 * CSCdi63443:  Dont need 2 copies of Month and Day Names
 *              Saves 220 bytes, 76 in data segment
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/07/05  19:01:55  snyder
 * CSCdi62128:  change some char*[] to const and save on data space
 * Branch: California_branch
 *         Gets 244 bytes out of data segment and into text.
 *
 * Revision 3.2.62.2  1996/06/24  19:06:32  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Missed some files.
 *
 * Revision 3.2.62.1  1996/03/18  21:29:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:09:03  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  16:37:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:45:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:18:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/25  19:04:57  dkatz
 * CSCdi40898:  Synchronized reload needed
 * Code review nits.
 *
 * Revision 2.6  1995/09/02  02:34:56  dkatz
 * CSCdi39760:  Need function to access timezone string
 * Add a function to do so.
 *
 * Revision 2.5  1995/07/14  02:05:01  dkatz
 * CSCdi37089:  Interface routine needed to obtain timezone offset
 *
 * Revision 2.4  1995/06/21  07:45:22  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 * Ensure 7000 boot roms compile.
 *
 * Revision 2.3  1995/06/16  04:29:11  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/11  23:29:47  dkatz
 * CSCdi35384:  ICMP timestamp packets have bad checksums
 * Add a routine to return the current time in ICMP timestamp form.
 *
 * Revision 2.1  1995/06/07  21:54:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

/*
 * Days in each month.  30 days hath September...
 */
#define	JAN	31
#define	FEB	28
#define	FEBLEAP	29
#define	MAR	31
#define	APR	30
#define	MAY	31
#define	JUN	30
#define	JUL	31
#define	AUG	31
#define	SEP	30
#define	OCT	31
#define	NOV	30
#define	DEC	31

/*
 * We deal in a 4 year cycle starting at March 1, 1900.  We assume
 * we will only want to deal with dates since then, and not to exceed
 * the rollover day in 2036.
 */
#define MILLISECSPERSEC (1000U)			/* milliseconds per second */
#define	SECSPERMIN	(60U)			/* seconds per minute */
#define	MINSPERHR	(60U)			/* minutes per hour */
#define	HRSPERDAY	(24U)			/* hours per day */
#define DAYSPERWEEK	(7U)			/* days per week */
#define	DAYSPERYEAR	(365U)			/* days per year */
#define DAYSPERLEAPYEAR (366U)			/* days per leap year */
#define YEARSPERCENTURY (100U)			/* years per century */

#define SECSPERHR       (SECSPERMIN * MINSPERHR)
#define	SECSPERDAY	(SECSPERMIN * MINSPERHR * HRSPERDAY)
#define MILLISECSPERDAY (SECSPERDAY * MILLSECSPERSEC)
#define SECSPERYEAR	(DAYSPERYEAR * SECSPERDAY)	/* regular year */
#define	SECSPERLEAPYEAR	(DAYSPERLEAPYEAR * SECSPERDAY)	/* leap year */

#define SECONDS_PER_MINUTE SECSPERMIN	/* Another name */
#define MINUTES_PER_HOUR MINSPERHR	/* Ditto. */
#define SECONDS_PER_HOUR SECSPERHR	/* Me too. */

#define	MAR1900		((JAN+FEB) * SECSPERDAY) /* no leap year in 1900 */
#define	DAYSPERCYCLE	(DAYSPERYEAR+DAYSPERYEAR+DAYSPERYEAR+DAYSPERLEAPYEAR)	/* 3 normal years plus leap */
#define	SECSPERCYCLE	(DAYSPERCYCLE*SECSPERDAY)
#define	YEARSPERCYCLE	4

/*
 * Gross hacks.  I have illicit knowlege that there won't be overflows
 * here, the compiler often can't tell this.
 */
#define	TIMES60(val)	(((val)<<6) - ((val)<<2))	/* *64 - *4 */
#define	TIMES24(val)	(((val)<<4) + ((val)<<3))	/* *16 + *8 */
#define	TIMESDPERC(val)	(((val)<<10) + ((val)<<8) \
			+ ((val)<<7) + ((val)<<5) \
			+ ((val)<<4) + ((val)<<2) + (val))	/* *big* hack */

/*
 * Some important dates.
 */
#define	CYCLE23		(23U)
#define	STARTCYCLE23	(CYCLE23*SECSPERCYCLE)
#define JAN11900DOW	1U	/* Jan 1, 1900 was a Monday */

/*
 * The length of January + February in leap and non-leap years.
 */
#define	JANFEBNOLEAP	((JAN+FEB) * SECSPERDAY)
#define	JANFEBLEAP	((JAN+FEBLEAP) * SECSPERDAY)

#define MAR1968		(MAR1900+(17*SECSPERCYCLE)) /* March 1, 1968 */
#define MAR1970		(MAR1968+(2*SECSPERYEAR)) /* March 1, 1970 */
#define JAN1970		(MAR1970-JANFEBNOLEAP) /* Jan 1, 1970 */
#define MAR1992         (STARTCYCLE23 + MAR1900)
#define MAR1993		(MAR1992 + SECSPERYEAR)	/* March 1, 1993 */

/* Default time zone information */

#define DEFAULT_TZ_OFFSET 0	/* UTC, of course */
#define DEFAULT_TZ_NAME "UTC" /* Dave Mills wouldn't have it otherwise. */
#define DEFAULT_DST_OFFSET (SECSPERHR) /* Default DST offset in seconds */

/*
 * Minimum and maximum years
 * 1993 is sorta arbitrary, 2035 is when a 32-bit second count
 * from 1900 rolls over.
 */

#define CLOCK_MIN_YEAR	1993
#define CLOCK_MAX_YEAR	2035

/*
 * A broken up representation of time
 */

struct timeval_ {
    ulong year;				/* Year, AD (1993, not 93!) */
    ulong month;			/* Month in year (Jan = 1) */
    ulong day;				/* Day in month (1-31) */
    ulong hour;				/* Hour in day (0-23) */
    ulong minute;			/* Minute in hour (0-59) */
    ulong second;			/* Second in minute (0-59) */
    ulong millisecond;			/* Millisecond in second (0-999) */
    ulong day_of_week;			/* Sunday = 0, Saturday = 6 */
    ulong day_of_year;			/* Day in Year (1-366) */
    long tz_offset;			/* Timezone offset (secs from UTC) */
};

/*
 * Time returned by secs_and_nsecs_since_jan_1_1970
 */

typedef struct secs_and_nsecs_ {
    ulong seconds;		/* Integer seconds since 0000 UTC 1/1/1970 */
    ulong nanoseconds;		/* Nanosecond residual */
} secs_and_nsecs;

/*
 * The system time is kept as a 64 bit value, in seconds and fractional
 * seconds since 00:00 UTC 1 Jan 1900.  This is the same format as NTP
 * uses.
 */
typedef struct {
	union {
		ulong Xl_ui;
		long Xl_i;
	} Ul_i;				/* Integer part */
	union {
		ulong Xl_uf;
		long Xl_f;
	} Ul_f;				/* Fraction part */
} clock_epoch;

#define epoch_secs	Ul_i.Xl_ui		/* integral seconds */
#define	epoch_frac	Ul_f.Xl_uf		/* fractional part */

/*
 * Potential clock sources
 */
typedef enum clock_source_ {
    CLOCK_SOURCE_NONE,		/* Unsynchronized */
    CLOCK_SOURCE_NTP,		/* Network Time Protocol */
    CLOCK_SOURCE_SNTP,		/* Simple Network Time Protocol */
    CLOCK_SOURCE_MANUAL,	/* Manual configuration */
    CLOCK_SOURCE_CALENDAR,	/* Hardware calendar */
    CLOCK_SOURCE_VINES		/* Vines time service */
} clock_source;

/*
 * enum for local_time_string
 */
typedef enum {
    CALCULATE_SUMMER_TIME,	/* Calculate summer time for this timestamp */
    USE_STANDARD_TIME,		/* Use standard time for this timestamp */
    USE_SUMMER_TIME		/* Use summer time for this timestamp */
} summer_time_selection_type;

/*
 * Structure to hold DST starting and ending information
 */

typedef struct recur_ {
    int day;		/* Day of week */
    int week;		/* Week of month (-1 if last) */
    int month;		/* Month of year */
    int hours;		/* Time (hours) */
    int minutes;	/* Time (minutes) */
} recur;

#define MONTHS_IN_YEAR 12
#define DAYS_IN_WEEK    7

extern const char *const day_name[], *const month_name[];
extern const char *const long_day_name[], *const long_month_name[];

extern ulong unix_time(void);
extern void secs_and_nsecs_since_jan_1_1970(secs_and_nsecs *);
extern boolean clock_set_unix(ulong, clock_source);
extern void clock_is_now_valid(void);
extern clock_source current_time_source(void);
extern boolean clock_is_probably_valid(void);
extern void unix_time_string(char *, ulong);
extern void current_time_string(char *);
extern ulong clock_icmp_time(void);
extern void clock_icmp_time_to_timeval(ulong, int, timeval *);
extern void unix_time_to_timeval(ulong, timeval *, char **);
extern boolean unix_time_is_in_summer(ulong);
extern boolean clock_time_is_in_summer(clock_epoch *);
extern void clock_init(void);
extern void unix_time_string_2 (char *, ulong);
extern void unix_time_to_epoch (ulong, clock_epoch *);
extern ulong clock_epoch_to_unix_time(clock_epoch *);
extern void clock_get_time(clock_epoch *timeptr);
extern void clock_get_time_exact(clock_epoch *timeptr);
extern ulong clock_get_microsecs(void);
extern void clock_set(clock_epoch *, clock_source);
extern void clock_get_timezone_info(clock_epoch *, char **, long *);
extern ulong format_time(char *, ulong, const char *, timeval *, char *);
extern void clock_epoch_to_timeval(clock_epoch *, timeval *, long);
extern void clock_timeval_to_epoch(timeval *, clock_epoch *);
extern ulong clock_timeval_to_unix_time(timeval *);
extern int clock_timezone_offset(void);
extern char *clock_timezone_name(void);
extern int clock_epoch_compare(clock_epoch *, clock_epoch *);

#include "clock_format_string_lengths.h"

#endif /* _CLOCK_H_ */
