/* $Id: cfg_clock.h,v 3.2.62.1 1996/08/12 16:05:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_clock.h,v $
 *------------------------------------------------------------------
 * clock command parser chains
 *
 * Dave Katz, March, 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_clock.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:42:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:02:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
* [no] clock calendar-valid
*/
EOLNS    (clock_calendar_valid_eol, clock_calendar_valid_command);
KEYWORD (clock_calendar_valid, clock_calendar_valid_eol, NONE,
	 "calendar-valid", "Calendar time is authoritative", PRIV_CONF);
/*
 * Only allow the above command if the cpu supports calendar hardware.
 */
ASSERT	(clock_cal_test, clock_calendar_valid, no_alt, 
	 reg_invoke_cpu_supports_calendar());

/******************************************************************************
 * clock summer-time name 
 * clock summer-time PDT recurring
 * clock summer-time PDT recurring 4 Sun Apr 02:00 1 Sun Oct 02:00 120
 * clock summer-time PDT recurring last Sun Apr 02:00 first Sun Oct 02:00
 * clock summer-time PDT date Apr 3 1993 02:00 Oct 5 1993 02:00
 * clock summer-time PDT date 3 Apr 1993 02:00 5 Oct 1993 02:00
 * no clock summer-time
 *
 * OBJ(string,1) = time zone
 * OBJ(int,1) = 1 if recurring, 0 if date
 * OBJ(int,2) = starting week number or date or 0 if default or -1 if "last"
 * OBJ(int,3) = starting day of week (0-6 = Sunday-Saturday) or year
 * OBJ(int,4) = starting month (1-12 = January-December)
 * OBJ(int,5) = starting hour
 * OBJ(int,6) = starting minute
 * OBJ(int,7) = ending week number or date or -1 if "last"
 * OBJ(int,8) = ending day of week or year
 * OBJ(int,9) = ending month
 * OBJ(int,10) = ending hour
 * OBJ(int,11) = ending minute
 * OBJ(int,12) = summer time offset in minutes or 0
 */

EOLNS    (summer_eol, summer_time_cmd);

NUMBER  (summer_offset, summer_eol, summer_eol, OBJ(int,12), 1, 60*24,
	 "Offset to add in minutes");

TIME    (summer_time_b, summer_offset, no_alt, OBJ(int,10), OBJ(int,11),
	 "Time to end (hh:mm)");
NUMBER  (summer_year_b, summer_time_b, no_alt, OBJ(int,8),
	 CLOCK_MIN_YEAR, CLOCK_MAX_YEAR,
	 "Year to end");
NUMBER  (summer_day_b2, summer_year_b, no_alt, OBJ(int,7), 1, 31,
	 "Date to end");
SPLIT   (summer_test_b, summer_day_b2, summer_year_b, OBJ(int,7), 0);
MONTH   (summer_month_b, summer_test_b, no_alt, OBJ(int,9),
	 "Month to end");
NUMBER  (summer_day_b, summer_month_b, summer_month_b, OBJ(int,7), 1, 31,
	 "Date to end");

TIME    (summer_time_a, summer_day_b, no_alt, OBJ(int,5), OBJ(int,6),
	 "Time to start (hh:mm)");
NUMBER  (summer_year_a, summer_time_a, no_alt, OBJ(int,3),
	 CLOCK_MIN_YEAR, CLOCK_MAX_YEAR,
	 "Year to start");
NUMBER  (summer_day_a2, summer_year_a, no_alt, OBJ(int,2), 1, 31,
	 "Date to start");
SPLIT   (summer_test_a, summer_day_a2, summer_year_a, OBJ(int,2), 0);
MONTH   (summer_month_a, summer_test_a, no_alt, OBJ(int,4),
	 "Month to start");
NUMBER  (summer_day_a, summer_month_a, summer_month_a, OBJ(int,2), 1, 31,
	 "Date to start");

KEYWORD (summer_date, summer_day_a, no_alt, "date",
	   "Configure absolute summer time", PRIV_CONF);

TIME    (summer_time_2, summer_offset, no_alt, OBJ(int,10), OBJ(int,11),
	 "Time to end (hh:mm)");
MONTH   (summer_month_2, summer_time_2, no_alt, OBJ(int,9),
	 "Month to end");
DAY     (summer_day_2, summer_month_2, no_alt, OBJ(int,8), "Weekday to end");
NUMBER  (summer_week_2, summer_day_2, summer_eol, OBJ(int,7), 1, 4,
	 "Week number to end");
KEYWORD_ID(summer_last_2, summer_day_2, summer_week_2, OBJ(int,7), -1,
	   "last", "Last week of the month", PRIV_CONF);
KEYWORD_ID(summer_first_2, summer_day_2, summer_last_2, OBJ(int,7), 1, 
	   "first", "First week of the month", PRIV_CONF);

TIME    (summer_time_1, summer_first_2, no_alt, OBJ(int,5), OBJ(int,6),
	 "Time to start (hh:mm)");
MONTH   (summer_month_1, summer_time_1, no_alt, OBJ(int,4),
	 "Month to start");
DAY     (summer_day_1, summer_month_1, no_alt, OBJ(int,3), "Weekday to start");
NUMBER  (summer_week_1, summer_day_1, summer_eol, OBJ(int,2), 1, 4,
	 "Week number to start");
KEYWORD_ID(summer_last_1, summer_day_1, summer_week_1, OBJ(int,2), -1,
	   "last", "Last week of the month", PRIV_CONF);
KEYWORD_ID(summer_first_1, summer_day_1, summer_last_1, OBJ(int,2), 1, 
	   "first", "First week of the month", PRIV_CONF);

KEYWORD_ID(summer_recurring, summer_first_1, summer_date, OBJ(int,1), 1, 
	   "recurring", "Configure recurring summer time", PRIV_CONF);

STRING  (summer_name, summer_recurring, no_alt, OBJ(string,1),
	 "name of time zone in summer");
NOPREFIX(summer_no, summer_name, summer_eol);
NVGENNS  (summer_nvgen, summer_no, summer_time_cmd);
KEYWORD (clock_summer_time, summer_nvgen, clock_cal_test, "summer-time",
	 "Configure summer (daylight savings) time", PRIV_CONF);

/******************************************************************************
* clock timezone name [-]hrs [mins]
* no clock timezone
*
* The form of the command "timezone name..." is hidden and supported for
* backward beta compatibility.
*
* OBJ(int,1) = hours offset (may be negative)
* OBJ(int,2) = minutes offset
* OBJ(string,1) = time zone name
*/
EOLNS    (timezone_eol, timezone_cmd);
NUMBER  (timezone_mins, timezone_eol, timezone_eol, OBJ(int,2),
	 0, 59, "Minutes offset from UTC");
SIGNED_DEC(timezone_hrs, timezone_mins, no_alt, OBJ(int,1), -23, 23,
	 "Hours offset from UTC");
STRING  (timezone_name, timezone_hrs, no_alt, OBJ(string,1),
	 "name of time zone");
NOPREFIX(timezone_no, timezone_name, timezone_eol);
NVGENNS  (timezone_nvgen, timezone_no, timezone_cmd);

KEYWORD (timezone, timezone_nvgen, clock_summer_time, "timezone",
	 "Configure time zone", PRIV_CONF);

SPLIT   (timezone_hidden_nvgen, timezone_no, no_alt, nvgen, FALSE);

KEYWORD (timezone_hidden, timezone_hidden_nvgen, ALTERNATE,
	 "timezone", NULL, PRIV_CONF|PRIV_HIDDEN);

KEYWORD (clock, timezone,  timezone_hidden, "clock", 
	 "Configure time-of-day clock", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE clock
