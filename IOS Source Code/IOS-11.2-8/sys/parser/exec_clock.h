/* $Id: exec_clock.h,v 3.2.62.1 1996/08/12 16:06:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_clock.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L O C K . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clock.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:16:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clock update-calendar
 */

EOLNS    (clock_update_cal_eol, clock_update_calendar_exec);
KEYWORD (clock_update_cal, clock_update_cal_eol, no_alt, "update-calendar",
	 "Update the hardware calendar from the clock", PRIV_OPR);

/******************************************************************
 * clock read-calendar
 */

EOLNS    (clock_read_cal_eol, clock_read_calendar_exec);
KEYWORD (clock_read_cal, clock_read_cal_eol, clock_update_cal, "read-calendar",
	 "Read the hardware calendar into the clock", PRIV_OPR);

/* Do the above commands only if there is a hardware calendar. */

ASSERT  (clock_calendar_test2, clock_read_cal, no_alt, 
	 reg_invoke_cpu_supports_calendar());

/* Do the above commands if we started with "clock" and not "calendar". */

TESTVAR	(clock_calendar_test, clock_calendar_test2, NONE,
	 NONE, NONE, no_alt, OBJ(int,10), 0);

/******************************************************************
 * clock set hh:mm:ss {Mon dd yyyy | dd Mon yyyy}
 * calendar set hh:mm:ss {Mon dd yyyy | dd Mon yyyy}
 */

EOLNS    (clock_set_eol, clock_set_command);
NUMBER  (clock_set_year, clock_set_eol, no_alt, OBJ(int,6),
	 CLOCK_MIN_YEAR, CLOCK_MAX_YEAR,
	 "Year");
NUMBER  (clock_set_day_2, clock_set_year, no_alt, OBJ(int,5), 1, 31,
	 "Day of the month");
SPLIT   (clock_set_test, clock_set_day_2, clock_set_year, OBJ(int,5), 0);
MONTH   (clock_set_month, clock_set_test, no_alt, OBJ(int,4),
	 "Month of the year");
NUMBER  (clock_set_day, clock_set_month, clock_set_month, OBJ(int,5), 1, 31,
	 "Day of the month");
TIMESEC (clock_set_time, clock_set_day, no_alt, OBJ(int,1), OBJ(int,2),
	 OBJ(int,3), "Current Time");
KEYWORD (clock_set, clock_set_time, clock_calendar_test, "set", 
	 "Set the time and date", PRIV_OPR);
TEST_MULTIPLE_FUNCS(clock_test, clock_set, NONE, no_alt);
KEYWORD_ID (exec_calendar, clock_test, NONE,
	    OBJ(int,10), 1, 
	    "calendar",  "Manage the hardware calendar", PRIV_OPR);
ASSERT	(exec_calendar_test, exec_calendar, ALTERNATE, 
	 reg_invoke_cpu_supports_calendar());
KEYWORD (exec_clock, clock_test, exec_calendar_test, "clock", 
	 "Manage the system clock", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_clock
