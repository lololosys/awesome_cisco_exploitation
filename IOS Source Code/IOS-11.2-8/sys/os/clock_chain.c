/* $Id: clock_chain.c,v 3.1.16.5 1996/08/07 09:01:44 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/clock_chain.c,v $
 *------------------------------------------------------------------
 * Parse routines for Basic system clock support
 *
 * March 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clock_chain.c,v $
 * Revision 3.1.16.5  1996/08/07  09:01:44  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.16.4  1996/06/24  19:06:35  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Missed some files.
 *
 * Revision 3.1.16.3  1996/05/21  09:58:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.16.2  1996/05/09  14:39:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.16.1.18.1  1996/04/27  07:16:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.16.1.8.1  1996/04/09  01:15:25  cakyol
 * clock format should be %CC not %C.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.16.1  1996/03/18  21:29:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:09:05  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:05  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "clock.h"
#include "clock_internal.h"
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"


/* Default information for recurring summer time (US rules) */

static recur default_start = {0 /* Sunday */, 1, 4 /* April */,
					   02, 00};
static recur default_end = {0 /* Sunday */, -1 /* last */, 
						10 /* October */, 02, 00};
static boolean standard_tz_name_malloced = FALSE;

extern boolean calendar_is_valid;

extern recur recur_start, recur_end;

extern boolean summer_time_recurring;	/* True if recurring summer time */
extern boolean summer_time_enabled;	/* True if summer time is enabled */
extern boolean summer_time_active;	/* True if summer time is currently on */
extern long summer_time_offset;		/* Offset for summer time */
extern clock_epoch summer_time_start;	/* Start of next or current summer time */
extern clock_epoch summer_time_end;	/* End of next or current summer time */
extern char *summer_tz_name;		/* Name of time zone in summer time */

extern int standard_tz;			/* Standard time zone offset */
extern char *standard_tz_name;		/* Standard time zone name */


/*
 * Parse chains for clock exec commands
 */
#define ALTERNATE       NONE
#include	"../parser/exec_show_clock.h"
LINK_POINT(clock_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for clock config commands
 */
#define ALTERNATE       NONE
#include	"../parser/cfg_clock.h"
LINK_POINT(clock_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for clock exec commands
 */
#define ALTERNATE       NONE
#include	"../parser/exec_clock.h"
LINK_POINT(clock_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for clock commands
 */
static parser_extension_request clock_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(clock_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(clock_config_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(clock_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * clock_ui_init
 *
 * Initialize clock parser support.
 *
 */
static void clock_ui_init (subsystype *subsys)
{
    parser_add_command_list(clock_chain_init_table, "clock");
    clock_register_printf();
}

/*
 * show {clock [detail] | calendar}
 *
 * OBJ(int,1) = 1 if "calendar"
 * OBJ(int,2) = 1 if "clock detail"
 */
static char *const time_sources[] = {
    "none",
    "NTP",
    "SNTP",
    "user configuration",
    "hardware calendar",
    "Vines"
};

void show_clock (parseinfo *csb)
{
    timeval tv;
    clock_epoch curtime;
    char *format_string;
    char timestring[LOCAL_TIME_STRING_MINBUF];

    /* Get the time from the appropriate source. */

    if (GETOBJ(int,1)) {		/* "show calendar" */
	reg_invoke_ntp_read_calendar(&tv);
	clock_timeval_to_epoch(&tv, &curtime);
	check_for_summer_time(&curtime);
	format_string = "%C";
    } else {				/* "show clock" */
	clock_get_time(&curtime);
	format_string = "%v%B";
    }
    printf("\n%CC", format_string, &curtime);

    if (GETOBJ(int,2)) {
	clock_source source = current_time_source();
	if (source == CLOCK_SOURCE_NONE) {
	    printf("\nNo time source");
	} else {
	    printf("\nTime source is %s", time_sources[source]);
	}
	if (summer_time_enabled) {
	    local_time_string(timestring, &summer_time_start, FALSE, 
			      USE_STANDARD_TIME);
	    printf("\nSummer time starts %s", timestring);
	    local_time_string(timestring, &summer_time_end, FALSE, 
			      USE_SUMMER_TIME);
	    printf("\nSummer time ends %s", timestring);
	}
    }
}


/*
 * clock summer-time name 
 * clock summer-time PDT recurring [4 Sun Apr 02:00 1 Sun Oct 02:00]
 * clock summer-time PDT recurring last Sun Apr 02:00 first Sun Oct 02:00
 * clock summer-time PDT date Apr 3 1993 02:00 Oct 5 1993 02:00
 * clock summer-time PDT date 3 Apr 1993 02:00 5 Oct 1993 02:00
 *
 * OBJ(string,1) = time zone
 * OBJ(int,1) = 0 if recurring, 1 if date
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

void summer_time_cmd (parseinfo *csb)
{
    /* First NVGEN. */

    if (csb->nvgen) {
	if (summer_time_enabled) {
	    nv_write(TRUE, "%s %s ", csb->nv_command, summer_tz_name);
	    if (summer_time_recurring) {
		nv_add(TRUE, "recurring");
		if (memcmp(&recur_start, &default_start, sizeof(recur)) || 
		    memcmp(&recur_end, &default_end, sizeof(recur))) {
		    if (recur_start.week == -1) {
			nv_add(TRUE, " last");
		    } else {
			nv_add(TRUE, " %d", recur_start.week);
		    }
		    nv_add(TRUE, " %s %s %d:%02d", day_name[recur_start.day],
			   month_name[recur_start.month - 1], 
			   recur_start.hours, recur_start.minutes);

		    if (recur_end.week == -1) {
			nv_add(TRUE, " last");
		    } else {
			nv_add(TRUE, " %d", recur_end.week);
		    }
		    nv_add(TRUE, " %s %s %d:%02d", day_name[recur_end.day],
			   month_name[recur_end.month - 1], recur_end.hours,
			   recur_end.minutes);
		}
	    } else {		/* Not recurring */
		timeval tv;
		clock_epoch_to_timeval(&summer_time_start, &tv, standard_tz);
		nv_add(TRUE, "date %s %d %d %d:%02d",
		       month_name[tv.month - 1], tv.day, tv.year, tv.hour,
		       tv.minute);
		clock_epoch_to_timeval(&summer_time_end, &tv, 
			       standard_tz + summer_time_offset);
		nv_add(TRUE, " %s %d %d %d:%02d",
		       month_name[tv.month - 1], tv.day, tv.year, tv.hour,
		       tv.minute);
	    }
	    nv_add(summer_time_offset != DEFAULT_DST_OFFSET, " %d",
		   summer_time_offset / SECSPERMIN);
	}
	return;
    }

    summer_time_enabled = csb->sense;

    /* Free any old time zone name. */

    free(summer_tz_name);
    summer_tz_name = NULL;

    if (csb->sense) {

	summer_time_recurring = GETOBJ(int,1);

	/* Copy the string. */

	setstring(&summer_tz_name, GETOBJ(string,1));

	/* Set up the offset. */

	if (GETOBJ(int,12)) {
	    summer_time_offset = GETOBJ(int,12) * SECSPERMIN;
	} else {
	    summer_time_offset = DEFAULT_DST_OFFSET;
	}

	/* Check for recurring time. */

	if (summer_time_recurring) {

	    /* If no parameters, use the default. */

	    if (GETOBJ(int,2) == 0) { /* No info */
		recur_start = default_start;
		recur_end = default_end;
	    } else {
		recur_start.day = GETOBJ(int,3);
		recur_start.week = GETOBJ(int,2);
		recur_start.month = GETOBJ(int,4);
		recur_start.hours = GETOBJ(int,5);
		recur_start.minutes = GETOBJ(int,6);
		recur_end.day = GETOBJ(int,8);
		recur_end.week = GETOBJ(int,7);
		recur_end.month = GETOBJ(int,9);
		recur_end.hours = GETOBJ(int,10);
		recur_end.minutes = GETOBJ(int,11);
	    }
	    calculate_summer_time_epochs(NULL);

	} else {		/* Not recurring */

	    timeval tv;

	    /* Create the absolute timestamps. */

	    memset(&tv, 0, sizeof(tv));
	    tv.year = GETOBJ(int,3);
	    tv.month = GETOBJ(int,4);
	    tv.day = GETOBJ(int,2);
	    tv.hour = GETOBJ(int,5);
	    tv.minute = GETOBJ(int,6);
	    tv.tz_offset = standard_tz;
	    clock_timeval_to_epoch(&tv, &summer_time_start);

	    tv.year = GETOBJ(int,8);
	    tv.month = GETOBJ(int,9);
	    tv.day = GETOBJ(int,7);
	    tv.hour = GETOBJ(int,10);
	    tv.minute = GETOBJ(int,11);
	    tv.tz_offset = standard_tz + summer_time_offset;
	    clock_timeval_to_epoch(&tv, &summer_time_end);
	}
	check_for_summer_time(NULL);
    } else {
	sys_tz = standard_tz;
	sys_tz_name = standard_tz_name;
	summer_time_offset = 0;
    }
}


/*
 * clock timezone name hrs [mins]
 * no clock timezone
 *
 * OBJ(int,1) = hours offset from UTC
 * OBJ(int,2) = minutes offset from UTC
 * OBJ(string,1) = timezone name
 */

void timezone_cmd (parseinfo *csb)
{
    int hours = 0, minutes = 0; /* Dumb compiler */

    /* NVGENs first, of course. */

    if (csb->nvgen) {
	if (standard_tz != DEFAULT_TZ_OFFSET ||
	    strcmp(standard_tz_name, DEFAULT_TZ_NAME)) {
	    if (standard_tz < 0) {
		hours = -((-standard_tz) / SECSPERHR);
		minutes = ((-standard_tz) % SECSPERHR) / SECSPERMIN;
	    } else {
		hours = standard_tz / SECSPERHR;
		minutes = (standard_tz % SECSPERHR) / SECSPERMIN;
	    }
	    nv_write(TRUE, "%s %s %d", csb->nv_command, standard_tz_name, 
		     hours);
	    nv_add(minutes, " %d", minutes);
	}
	return;
    }

    /* Free any malloced name string. */

    if (standard_tz_name_malloced) {
	free(standard_tz_name);
	standard_tz_name_malloced = FALSE;
    }

    /* "No" version first. */

    if (!csb->sense) {
	standard_tz = DEFAULT_TZ_OFFSET;
	standard_tz_name = DEFAULT_TZ_NAME;
    } else {

	/* Copy the string. */

	standard_tz_name = NULL;	/* setstring will free() it.. */
	setstring(&standard_tz_name, GETOBJ(string,1));
	standard_tz_name_malloced = TRUE;

	/* Set up the offset. */

	hours = GETOBJ(int,1);
	minutes = GETOBJ(int,2);
	if (hours < 0)
	    minutes = -minutes;
	standard_tz = (hours * SECSPERHR) + (minutes * SECSPERMIN);
    }

    /* Copy over to the system timezone variables. */

    sys_tz = standard_tz;
    sys_tz_name = standard_tz_name;

    /* Recalculate the summer time parameters, then see if it's summer time. */

    calculate_summer_time_epochs(NULL);
    check_for_summer_time(NULL);
}

/*
 * [no] clock calendar-valid
 *
 * Only called if calendar hardware is present.
 */
void clock_calendar_valid_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(calendar_is_valid, csb->nv_command);
	return;
    }
    calendar_is_valid = csb->sense;

    /*
     * If we've just said that the calendar is valid, say that the
     * time has been set (since we know we already read the calendar)
     * and update the NTP master mode as necessary.
     */
    if (calendar_is_valid) {
	clock_is_now_valid();
	reg_invoke_time_set(FALSE);
    }
}
	

/*
 * Set the time by user command.
 *
 * OBJ(int,1) = hours
 * OBJ(int,2) = minutes
 * OBJ(int,3) = seconds
 * OBJ(int,4) = month
 * OBJ(int,5) = day
 * OBJ(int,6) = year
 * OBJ(int,10) = nz if setting calendar instead of clock
 */
void clock_set_command (parseinfo *csb)
{
    timeval tv;
    clock_epoch ntp_time;
    char *dummy_tz_name;
    long first_offset;
    long summertime_offset;

    tv.hour = GETOBJ(int,1);
    tv.minute = GETOBJ(int,2);
    tv.second = GETOBJ(int,3);
    tv.millisecond = 0;
    tv.month = GETOBJ(int,4);
    tv.day = GETOBJ(int,5);
    tv.year = GETOBJ(int,6);

    /*
     * Resolve whether or not the time was entered in standard or summer time.
     * The expected behavior is that times typed in during summer time are
     * summer time-relative, and the same is true for times typed in during
     * winter time.  However, there are two ambiguous spots.  At the beginning
     * of summer time there is a period of times that don't exist (e.g., 2:30
     * a.m. doesn't exist on the day that summer time starts at 2:00 a.m.),
     * and at the end there is a period that happens twice (e.g., 1:30 a.m. PDT
     * and 1:30 a.m. PST).
     *
     * We resolve this in the following way:  an invalid time will produce an
     * error message.  If 1:30 at the end of summer is typed, first it will
     * be taken as summer time.  If the time is set again during the ambiguous
     * period, it will be then set to standard time.  If set yet again, it
     * will turn back to daylight time.
     */

    /* Initially interpret time as summer time. */

    get_summertime_info(TRUE, &dummy_tz_name, &summertime_offset);
    tv.tz_offset = summertime_offset;
    clock_timeval_to_epoch(&tv, &ntp_time);

    /* Check for the time singularity. */

    if ((summer_time_start.epoch_secs - summer_time_offset <=
	 ntp_time.epoch_secs) &&
	(ntp_time.epoch_secs < summer_time_start.epoch_secs)) {
	printf("\nInvalid time (doesn't exist due to summer time rules)");
	return;
    }

    /* Get the timezone for this time.  If it changed, it must be standard. */

    clock_get_timezone_info(&ntp_time, &dummy_tz_name, &first_offset);
    if (first_offset != summertime_offset) {
	get_summertime_info(FALSE, &dummy_tz_name, &tv.tz_offset);
	clock_timeval_to_epoch(&tv, &ntp_time);
    } else {

	clock_epoch current_time;

	/* It didn't change.  Switch to standard if we're at end of summer. */

	clock_get_time(&current_time);
	if ((summer_time_end.epoch_secs - summer_time_offset <
	     current_time.epoch_secs) &&
	    (current_time.epoch_secs < summer_time_end.epoch_secs)) {
	    get_summertime_info(FALSE, &dummy_tz_name, &tv.tz_offset);
	    clock_timeval_to_epoch(&tv, &ntp_time);
	}
    }

    /*
     * If setting the calendar, convert back to timeval format.  This serves
     * to normalize silly dates (like February 30), calculate the proper
     * day of the week, and get the time zone right.
     */
    if (GETOBJ(int,10)) {
	clock_epoch_to_timeval(&ntp_time, &tv, 0);
	reg_invoke_ntp_write_calendar(&tv);
    } else {
	clock_set(&ntp_time, CLOCK_SOURCE_MANUAL);
	clock_is_now_valid();
    }
}

/*
 * clock_ui subsystem header
 */
 
#define CLOCK_UI_MAJVERSION    1
#define CLOCK_UI_MINVERSION    0
#define CLOCK_UI_EDITVERSION   1
 
SUBSYS_HEADER(clock_ui, CLOCK_UI_MAJVERSION, CLOCK_UI_MINVERSION,
              CLOCK_UI_EDITVERSION,
              clock_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

