/* $Id: reload.c,v 3.4.10.4 1996/09/11 15:52:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/reload.c,v $
 *------------------------------------------------------------------
 * Scheduled reload and message-blurting support
 *
 * September 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reload.c,v $
 * Revision 3.4.10.4  1996/09/11  15:52:08  snyder
 * CSCdi68773:  more things const
 *              536 out of data section
 *              100 out of image
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/08/28  13:04:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.2  1996/08/07  09:01:56  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/03/18  21:31:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:21:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  10:19:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:41:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/21  00:33:02  jquan
 * CSCdi45337:  wall() function used by flh is bogus. Added
 * datagram_done() after ttyline_sendmessage(). It is necessary.
 *
 * Revision 3.3  1996/02/13  19:13:14  jquan
 * CSCdi45337:  wall() function used by flh is bogus. Replace fprintf by
 * ttyline_sendmessage in wall() to avoid hanging system. Change comments
 * regarding wall() function. Remove datagram_done() after
 * ttyline_sendmessage() since the background tty output process will free
 * the buffer.
 *
 * Revision 3.2  1995/11/17  00:18:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  19:04:58  dkatz
 * CSCdi40898:  Synchronized reload needed
 * Code review nits.
 *
 * Revision 2.1  1995/09/24  00:54:42  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "async.h"
#include "clock.h"
#include "sys_registry.h"
#include "reload.h"
#include "hostname.h"
#include "logger.h"
#include "mgd_timers.h"
#include "msg_sched.c"			/* Not a typo, see logger.h */


/* Local storage for reload process */

static boolean reload_armed;		/* TRUE if reloading at timer expiry */
static boolean reload_reread_parameters;/* Process should reread params */
static mgd_timer reload_timer;		/* Timer for reload process */
static ulong reload_seconds;		/* secs_since_boot value for reload */
					/* NZ if "in", Z if "at" */
static clock_epoch reload_epoch;	/* Epoch that task should wake up */
static boolean reload_process_running;	/* TRUE if reload process exists */
static char *reload_message;		/* Non-NULL if there's a message */
static int reload_line;			/* TTY line, or -1 for all */
static boolean reload_requested;	/* TRUE if we reload at the end */
static const int *reload_interval_table;/* Non-NULL if interval table */
static int reload_message_interval;	/* Message interval if no table */
static char *reload_boot_reason;	/* Reload reason */
static const char reload_noise[] = {"\n\n\n\07***\n***"};
static const char reload_noise_2[] = {"\n***\n"};

/*
 * reload_get_delay
 *
 * Returns the number of seconds until the scheduled reload.  Assumes that
 * a scheduled reload is pending.  Returns a negative number if the
 * reload time has already passed.
 */
long reload_get_delay (void)
{
    long seconds_left;
    clock_epoch current_time;

    if (reload_seconds) {		/* "reload in" */
	seconds_left = reload_seconds - system_uptime_seconds();
    } else {			/* "reload at" */
	clock_get_time(&current_time);
	seconds_left = reload_epoch.epoch_secs - current_time.epoch_secs;
    }
    return(seconds_left);
}

/*
 * reload_send_message
 *
 * Send a message to all terminals.
 */
static void reload_send_message (const char *message)
{
    paktype *pak;

    pak = getbuffer(strlen(message) + 1); /* Get a buffer */
    if (!pak)
	return;
    strcpy(pak->datagramstart, message);
    pak->datagramsize = strlen(pak->datagramstart);
    ttyline_sendmessage(reload_line, pak);
    datagram_done(pak);
}

/*
 * reload_send_string
 *
 * Send a message to all terminals, embedding it in all the silliness.
 */
static void reload_send_string (char *message)
{
    char buffer[200];			/* Plenty big, yup, right. */

    sprintf(buffer, "%s --- %s ---", reload_noise, message);
    reload_send_message(buffer);
}

/*
 * reload_announce_impending_doom
 *
 * Send a message about the fact that the box will reload soon.
 */
static void reload_announce_impending_doom (int seconds_left)
{
    char buffer[80];			/* Plenty big... */
    ulong hours, minutes, seconds;

    seconds = seconds_left;
    hours = seconds / SECONDS_PER_HOUR;
    seconds -= hours * SECONDS_PER_HOUR;
    minutes = seconds / SECONDS_PER_MINUTE;
    seconds -= minutes * SECONDS_PER_MINUTE;

    sprintf(buffer, "SHUTDOWN in %d:%-0#2d:%-0#2d", hours, minutes, seconds);
    reload_send_string(buffer);
}

/*
 * reload_send_special_message
 *
 * Send the special message to the selected terminals.
 * If it's not there, just send the termination string.
 */

#define LOC_BUFFER_SIZE 240
static void reload_send_special_message (void)
{
    char buf[LOC_BUFFER_SIZE];	/* More than enough, as usual. */
    int buf_offset;

    if (reload_message) {
	buf_offset = sprintf(buf, "\n*** Message from network to ");
	if (reload_line == -1) {
	    buf_offset += sprintf(buf + buf_offset, "all terminals");
	} else {
	    buf_offset += sprintf(buf + buf_offset, "tty%t", reload_line);
	}
	sprintf(buf + buf_offset, ":\n***\n");
	reload_send_message(buf);
	sstrncpy(buf, reload_message, LOC_BUFFER_SIZE-1); /* Space for NL */
	strcat(buf, "\n");
	reload_send_message(buf);
    } else {
	reload_send_message(reload_noise_2);
    }
}

/*
 * reload_kill_process
 *
 * Kill the scheduled reload process.
 *
 * Should not return.
 */
static void reload_kill_process (void)
{
    reload_process_running = FALSE;
    reload_armed = FALSE;
    mgd_timer_stop(&reload_timer);
    process_kill(THIS_PROCESS);
}

/*
 * reload_process
 *
 * Process to perform a delayed reload
 */
static void reload_process(void)
{
    long seconds_left;
    ulong delay;
    const int *target_pointer;
    long next_target;
    ulong major, minor;

    /* Set up to watch our managed timer. */

    process_watch_mgd_timer(&reload_timer, ENABLE);

  start_over:

    /*
     * Set up the initial target time based on how much delay we have left.
     * If the table is null, we use the interval instead.
     */
    reload_reread_parameters = FALSE;
    target_pointer = reload_interval_table;
    seconds_left = reload_get_delay();
    if (seconds_left < 0)
	seconds_left = 0;		/* Don't do anything silly. */
    if (target_pointer) {
	while (*target_pointer) {
	    if (*target_pointer <= seconds_left)
		break;
	    target_pointer++;
	}
	next_target = *target_pointer;
    } else {
	next_target = seconds_left - (seconds_left % reload_message_interval);
    }

    /* Now loop, waiting for the right moment. */

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:

		/*
		 * The timer should be the only thing.  We reset the
		 * timer later.
		 */
		break;
		
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}

	/* If we need to reread parameters, jump to the top and start over. */

	if (reload_reread_parameters)
	    goto start_over;		/* Probably clearer than a loop... */
	
	/* Bail if we're no longer armed. */
	
	if (!reload_armed) {
	
	    /* Mark that we're done, and die. */

	    if (reload_requested) {
		reload_send_string("SHUTDOWN ABORTED");
	    } else {
		reload_send_message(reload_noise);
	    }
	    reload_send_special_message();
	    reload_kill_process();
	}

	/*
	 * Still armed.  We advance toward a series of target times at which
	 * we fire off a message to the terminals.  Eventually we hit the
	 * final target and reload.
	 *
	 * We need to deal with the fact that the timer system slips
	 * relative to the time-of-day clock by as much as 1% on some
	 * platforms.  If we rely only on the timer system, the box may
	 * reload as much as 12 minutes early or late if set 24 hours in
	 * advance.
	 *
	 * To get around this, we rely on the time as determined by
	 * NTP (if "reload at"), or by the secs_since_boot cell, which
	 * ticks at an NTP-adjusted rate.  We set our timer to expire after
	 * 7/8 of the remaining time has gone by, then reevaluate.  When
	 * we get within 10 seconds, we just let it run to completion and
	 * handle the target then.
	 */

	seconds_left = reload_get_delay() - next_target;
	if (seconds_left <= 0 && next_target == 0) { /* Do it now! */

	    /* We're at the end of our rope.  Reload now if we should. */

	    if (reload_requested) {
		reload_send_string("SHUTDOWN NOW");
	    } else {
		reload_send_message(reload_noise);
	    }
	    reload_send_special_message();
	    if (reload_requested) {
		boot_reason = reload_boot_reason;
		reload_system(RELOAD_NORMAL);
	    }
	    reload_kill_process();

	} else {

	    /*
	     * Not reloading quite yet.  If we've hit a target time, send out
	     * a message.
	     */
	    if (seconds_left <= 0) {	/* New target time hit */
		if (reload_requested) {
		    reload_announce_impending_doom(next_target);
		} else {
		    reload_send_message(reload_noise);
		}
		reload_send_special_message();

		if (target_pointer) {
		    target_pointer++;
		    next_target = *target_pointer;
		} else {
		    next_target -= reload_message_interval;
		    if (next_target < 0) /* Can't happen, but check anyhow */
			next_target = 0;
		}
		seconds_left = reload_get_delay() - next_target;
	    }
	    delay = seconds_left * ONESEC;
	    if (seconds_left > 10) {	/* It's a ways off. */
		delay -= delay / 8;
	    }
	    mgd_timer_start(&reload_timer, delay);
	}
    }
}

/*
 * reload_schedule_with_message
 *
 * Schedules a reload at a point in the future, and issues periodic messages.
 * The code will also simply issue the messages without reloading;  this is
 * for compatibility with the SNMP code that will at some point be changed
 * to call it.
 *
 * reload_time = time to reload.  If NULL, reload_delay is used instead.
 *
 * reload_delay = delay before reload, in seconds.  Used if reload_time
 *		is NULL.
 *
 * interval_table = pointer to an array of integers containing the times
 *		before the reload event at which the message should be
 *		issued (in seconds).  A zero element terminates
 *		the array.  The array must be in descending order.  If NULL,
 *		a fixed interval of message_interval is used.  If
 *		message_interval is zero, a default table is used.
 *
 * message_interval = the interval, in seconds, between messages being
 *		blurted.  Used only if interval_table is NULL.
 *
 * tty_line = the TTY line to issue the messages to.  If tty_line is -1,
 *		the messages will go to all terminals.
 *
 * message = a message to send at each message interval, in addition to the
 *		reload message.  If NULL, no additional message is sent.
 *		Reload messages are only sent if do_reload is TRUE.
 *
 * reload_reason = A string containing the reason for reload
 *
 * temp_banner = TRUE if "message" should be installed as a temporary banner
 *		during the interval, or FALSE otherwise.
 *
 * do_reload = TRUE if we should actually do the reload, or FALSE if we should
 *		only issue the messages.  The reload message will only be
 *		sent if do_reload is TRUE;  if false, only the string in
 *		"message" will be sent.
 */
static const int reload_msg_intervals[] = {
    120*SECONDS_PER_MINUTE,
    60*SECONDS_PER_MINUTE,
    30*SECONDS_PER_MINUTE,
    5*SECONDS_PER_MINUTE,
    SECONDS_PER_MINUTE,
    0};
void reload_schedule_with_message (clock_epoch *reload_time,
				   ulong reload_delay,
				   int *interval_table, int message_interval,
				   int tty_line, char *message,
				   char *reload_reason, boolean temp_banner,
				   boolean do_reload)
{
    /* Do some validation of parameters.  Bail without comment if wrong. */

    if (!reload_time && !reload_delay)
	return;				/* Gotta have one */
    if (interval_table && message_interval)
	return;				/* Only one */
    if (!message) {
	if (temp_banner || !do_reload)
	    return;			/* Need a message for these. */
    }

    if (reload_time) {
	reload_epoch = *reload_time;	/* Get the value! */
	reload_seconds = 0;		/* Note that we're using epoch. */
    } else {
	reload_seconds = system_uptime_seconds() + reload_delay;
    }
    reload_message_interval = message_interval;
    if (!message_interval) {
	if (interval_table) {
	    reload_interval_table = interval_table;
	} else {
	    reload_interval_table = reload_msg_intervals;
	}
    }
    reload_line = tty_line;
    reload_message = message;
    reload_boot_reason = reload_reason;
    if (temp_banner)
	banner_temp = message;
    reload_requested = do_reload;

    reload_armed = TRUE;
    if (reload_process_running) {	/* Already running; tweak it. */
	reload_reread_parameters = TRUE; /* Made you look! */
    } else {
	reload_process_running = TRUE;
	mgd_timer_init_leaf(&reload_timer, NULL, 0, NULL, FALSE);
	process_create(reload_process, "Delayed Reload", NORMAL_STACK,
		       PRIO_NORMAL);
    }
    mgd_timer_start(&reload_timer, 0);	/* Make it run right away. */
}

/*
 * reload_cancel
 *
 * Aborts a scheduled reload that is in progress.
 */
void reload_cancel (void)
{
    reload_armed = FALSE;		/* Make it die. */
    mgd_timer_start(&reload_timer, 0);	/* Make it run right away. */
}

/*
 * reload_in_progress
 *
 * Returns TRUE if a reload is in progress, FALSE otherwise.
 */
boolean reload_in_progress (void)
{
    return(reload_requested && reload_armed);
}
