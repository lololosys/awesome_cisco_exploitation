/* $Id: old_timer_callbacks.c,v 3.4.22.2 1996/08/28 13:03:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/old_timer_callbacks.c,v $
 *------------------------------------------------------------------
 * old_timer_callbacks.c -- manages timer elements
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: old_timer_callbacks.c,v $
 * Revision 3.4.22.2  1996/08/28  13:03:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.22.1  1996/03/18  21:30:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.16.2  1996/03/07  10:19:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.16.1  1996/02/20  16:40:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  20:45:03  hampton
 * Commit code review comments.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/29  07:29:14  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1996/01/05  10:18:02  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/05  09:39:38  hampton
 * New files for old timer code.
 *
 * Revision 3.5  1995/12/22  21:58:45  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.4  1995/11/23  06:50:40  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/17  18:52:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  11:44:52  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/07/18 06:48:27  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/19 06:49:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:59:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * NOTE--THESE ROUTINES HAVE BEEN DEPRECATED!  PLEASE DO NOT USE THEM.
 * YOU'LL PROBABLY JUST HAVE TO WRITE SERIALIZATION CODE ANYHOW.  USE
 * PASSIVE TIMERS OR MANAGED TIMERS INSTEAD (see Doc/Timers).
 */

/*
 * Routines Exported from this module
 *
 * set_timer:		set timer to a value (milliseconds) and start running
 * set_timer_interval:	set timer interval, but don't touch running status
 * jitter_timer:	set timer jitter level
 * stop_timer:		stop the timer
 * create_timer:	create timer for use
 * destroy_timer:	destroy timer element
 * change_timer_arg:	change parameters to call back function.
 * active_timer:	checks if the timer is running
 * get_interval:	gets the value the timer is set to (in milliseconds)
 * timer_init:		init the timer process
 *
 */

#include "master.h"
#include "old_timer_callbacks.h"
#include "old_timer_callbacks_private.h"
#include "exception.h"
#include "mgd_timers.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * External definitions
 */
extern ulong jitter_seed;


/*
 * Local variables
 */
static struct timer_queue {		/* keeps timer related queues */
    timer_t *active_queue;
    timer_t *idle_queue;
    mgd_timer next_execution;
    sys_timestamp lasttime;
} timers = {NULL, NULL, {0}, timer_no_time};

/*
 * check_delay_value
 *
 * Blow up if the delay value is more than half of the timer space
 * minus some fudge.  We can't deal with that.
 */
static inline void check_delay_value (ulong value)
{
    if (value >= (MAXUINT/2) - ONEMIN)
	crashdump(0);
}

/*
 * jittered_interval
 *
 * Return current interval setting with jitter applied
 *
 * Assumes that tm_maxjitter has been set up appropriately
 */
static inline ulong jittered_interval (timer_t *timer_ptr)
{
    ulong jitter_value;
    ulong random_value;

    /* If there's no jitter, just do the easy thing. */

    if (timer_ptr->tm_jitter_pct == 0) {
	return(timer_ptr->tm_interval);
    }

    /*
     * Since the jitter factor is less than 100% and we do truncation
     * arithmetic, we are guaranteed that the jitter range is less
     * than the total delay.  As such, we can guarantee that
     * jitter_value is at least two less than the delay value,
     * so the offset will always be nonzero, and there will always
     * be at least some delay.  Whew.
     *
     * Spin the jitter seed to help randomize successive calls.
     */
    GET_TIMESTAMP32(random_value);
    jitter_seed = (jitter_seed << 4) ^ random_value;
    jitter_value = jitter_seed % timer_ptr->tm_maxjitter;
    return (timer_ptr->tm_interval - jitter_value);
}


/*
 * timer_queue_has_changed
 *
 * update timer queue variables when we alter the timer queue.
 */
static inline void timer_queue_has_changed (void)
{
    if (timers.active_queue != NULL)
	mgd_timer_set_exptime(&timers.next_execution,
			      &timers.active_queue->tm_expiration);
    else
	mgd_timer_stop(&timers.next_execution);
#ifdef DEBUG_TIMERS
    buginf("\nTask will next run at %#Ta",
	   mgd_timer_exp_time(&timers.next_execution));
#endif
}

/*
 * timer_enq:
 * enqueues the timer structure in 
 * IDLE or ACTIVE queue depending upon
 * the state of timer.
 */
static void timer_enq (timer_t *timer_ptr)
{
    timer_t	*current;
    timer_t	**prev;

    /* If it should go into the idle queue, put it at the head. */

    if (timer_ptr->tm_state == TMS_IDLE) {
#ifdef DEBUG_TIMERS
	buginf("\nEnqueueing %x on idle queue", timer_ptr);
#endif
	timer_ptr->tm_next = timers.idle_queue;
	timer_ptr->tm_prev = &timers.idle_queue;
	if (timers.idle_queue) {
	    timers.idle_queue->tm_prev = &timer_ptr->tm_next;
	}
	timers.idle_queue = timer_ptr;

    } else {
	/*
	 * The timer goes into the run queue.  Insert it in order.
	 */
#ifdef DEBUG_TIMERS
	buginf("\nEnqueueing %x on active queue", timer_ptr);
#endif
	current = timers.active_queue;
	prev = &timers.active_queue;

	/* Walk the queue until we hit a later one or the end. */

	while (current) {
	    if (TIMER_LATER(current->tm_expiration,
			    timer_ptr->tm_expiration)) {
		break;
	    }
	    prev = &current->tm_next;
	    current = current->tm_next;
	}
	timer_ptr->tm_prev = prev;
	timer_ptr->tm_next = current;
	*prev = timer_ptr;
	if (current)
	    current->tm_prev = &timer_ptr->tm_next;
    }
    timer_queue_has_changed();
} /* end of timer_enq */


/*
 * timer_deq:
 * dequeues the timer element from
 * proper queue.
 */
static void timer_deq (timer_t *timer_ptr)
{

    timer_t *next, **prev;

    prev = timer_ptr->tm_prev;
    next = timer_ptr->tm_next;
    *prev = timer_ptr->tm_next;
    if (next)
	next->tm_prev = prev;

    timer_ptr->tm_prev = NULL;
    timer_ptr->tm_next = NULL;

    timer_queue_has_changed();
} /* end of timer_deq */

/*
 * timer_requeue
 *
 * Dequeue a timer, reset its expiration time, and requeue it.
 */
static inline void timer_requeue (timer_t *timer)
{
    timer_deq(timer);
    TIMER_START(timer->tm_expiration, jittered_interval(timer));
    timer->tm_state = TMS_RUNNING;
    timer_enq(timer);
}

/*
 * update_jitter_value
 *
 * Update tm_maxjitter when either the jitter percent or the interval
 * changes.
 *
 * We play games here to avoid truncation errors.
 */
static inline void update_jitter_value (timer_t *timer, ulong interval, 
					ulong jitter_pct)
{
    if (jitter_pct) {
	if (interval > (MAXULONG/2) / 100) {
	    timer->tm_maxjitter = (((interval/128) * jitter_pct) / 100) *
		128;
	} else {
	    timer->tm_maxjitter = (interval * jitter_pct) / 100;
	}
    } else {
	timer->tm_maxjitter = 0;
    }
}
    
/*
 * jitter_timer
 *
 * Set the jitter level on a timer, expressed as a percentage of the
 * timer delay to be randomized and subtracted from the interval.
 */
void jitter_timer (timer_t *timer, ulong jitter_pct)
{
    if (timer == NULL)
	return;
    if (jitter_pct > 99)
	crashdump(0);			/* Sorry, bogus. */
    timer->tm_jitter_pct = jitter_pct;
    update_jitter_value(timer, timer->tm_interval, jitter_pct);
}

/*
 * set_timer_interval_inline
 *
 * Set the time interval on the timer without affecting expiration
 */
static inline void set_timer_interval_inline (timer_t *timer, ulong value)
{
    check_delay_value(value);
    update_jitter_value(timer, value, timer->tm_jitter_pct);
    timer->tm_interval = value;
}

/*
 * set_timer_interval:
 *
 * Set the time interval on the timer without affecting expiration
 * (externally callable)
 */
void set_timer_interval (timer_t *timer, ulong value)
{
    if (timer != NULL)
	set_timer_interval_inline(timer, value);
}

/*
 * set_timer:
 *
 * Set the timer to the specified number of milliseconds (jittered, if
 * requested).  If already running and continuous, just update the
 * interval time for the next time around.  Otherwise, requeue it now
 * with the new interval.
 */
void set_timer (timer_t *timer, ulong value)
{
    if (timer == NULL)
	return;

    set_timer_interval_inline(timer, value);
    
    if ((timer->tm_type == TMT_ONCE) || (timer->tm_state == TMS_IDLE)) {
	timer_requeue(timer);
    }
}/* end of set_timer */

/*
 * restart_timer:
 * restart timer with the same interval
 */
void restart_timer (timer_t *timer)
{
    if (timer)
	timer_requeue(timer);
}/* end of restart_timer */

	
/* 
 * stop_timer:
 * stops timer from running 
 */
void stop_timer (timer_t *timer)
{
    if (timer && timer->tm_state == TMS_RUNNING) {
	timer_deq(timer);
	timer->tm_state = TMS_IDLE;
	timer_enq(timer);
    }
} /* end of stop_timer */


/* change_timer_arg:
 * changes timer arguments .
 */
void change_timer_arg (timer_t *timer, void *func, void *arg1,
		       void *arg2, void *arg3)
{
    timer_func fun_ptr = func;
    if (timer) {
	timer->tm_fun_ptr = fun_ptr;
	timer->tm_arg1 = arg1;
	timer->tm_arg2 = arg2;
	timer->tm_arg3 = arg3;
    }
} /* end of change_timer_arg */


/*
 * create_timer:
 * creates a timers and sets call back function
 * and arguments provided.
 */
timer_t *create_timer (void *func, void *arg1, void *arg2, void *arg3,
		       char *name, timer_types type)
{
    timer_t	*timer_ptr;
    timer_func fun_ptr = func;

    /* Allocate some memory. */

    timer_ptr = malloc(sizeof(timer_t));
    if (timer_ptr == NULL) {
	return(NULL);
    }

    /* Set it all up. */

    timer_ptr->tm_fun_ptr = fun_ptr;
    timer_ptr->tm_arg1 = arg1;
    timer_ptr->tm_arg2 = arg2;
    timer_ptr->tm_arg3 = arg3;
    timer_ptr->tm_state = TMS_IDLE;
    timer_ptr->tm_name = name;
    timer_ptr->tm_type = type;

    timer_enq(timer_ptr);
    return(timer_ptr);
} /* end of create_timer */


/*
 * destroy_timer:
 * deletes the given timer
 */
void destroy_timer (timer_t *timer_ptr)
{
    if (timer_ptr == NULL)
	return;

    timer_deq(timer_ptr);
    timer_ptr->tm_next = NULL;	/* In case a "show" has locked it */
    timer_ptr->tm_prev = NULL;	/* Ditto */
    free(timer_ptr);
} /* end of destroy_timer */


/*
 * active_timer:
 * check if this timer active or not
 */
boolean active_timer (timer_t *timer_ptr)
{
    if (timer_ptr == NULL)
	return(FALSE);
    return(timer_ptr->tm_state == TMS_RUNNING);
} /* end of active_timer */


/*
 * get_interval:
 * Return the timer interval.
 */
ulong get_interval (timer_t *timer)
{
    if (timer == NULL)
	return(0);
    return (timer->tm_interval);
} /* end of get_interval */
/*
 * get_due_time:
 * Return the amount of time until the timer expires.
 */
ulong get_due_time (timer_t *timer)
{
    int due_time;

    if ((timer == NULL)|| (timer->tm_state != TMS_RUNNING)) {
	due_time = 0;
    } else {
	due_time = TIME_LEFT_SLEEPING(timer->tm_expiration);
    }
    return(due_time);
} /* end of get_due_time */

/*
 * timer_process:
 * timer process that spins the wheel
 * be cautious when changing this routine
 * as the timer element can be deleted
 * or state changed while we are servicing
 * it.
 */
static process timer_process (void)
{
    timer_t *timer;
    sys_timestamp current_time;

    process_wait_on_system_init();
    while (TRUE) {
	process_sleep_on_timer(&timers.next_execution);
	GET_TIMESTAMP(current_time);
	COPY_TIMESTAMP(current_time, timers.lasttime);
	timer = timers.active_queue;

	/* Run down the queue until we run out of expired timers. */

	while (timer && !TIMER_EARLIER(current_time, timer->tm_expiration)) {
#ifdef DEBUG_TIMERS
	    buginf("\nTimer %x expired, now = %#Ta, exp = %#Ta",
		   timer, current_time, timer->tm_expiration);
#endif
	    timer_deq(timer);

	    /*
	     * For continuous timers, bump up the expiration time by the
	     * interval (so we don't slip).  If the expiration time has
	     * already passed, effectively skip a number of intervals
	     * but continue to avoid slippage.
	     *
	     * For one-shots, idle the timer.
	     */
	    if (timer->tm_type == TMT_CONTINUOUS) {
		long late_time;
		TIMER_UPDATE(timer->tm_expiration, jittered_interval(timer));
		late_time = CLOCK_DIFF_SIGNED(timer->tm_expiration, 
					      current_time);
		if (late_time > 0) { /* Already expired? */
		    ulong missed_ivls = 
			(late_time / timer->tm_interval) + 1;
		    TIMER_UPDATE(timer->tm_expiration,
				 missed_ivls * timer->tm_interval);
		    timer->tm_missed += missed_ivls;
		}
#ifdef DEBUG_TIMERS
		buginf("\nRequeued, new exp = %#Ta", timer->tm_expiration);
#endif
	    } else { 
		timer->tm_state = TMS_IDLE;
#ifdef DEBUG_TIMERS
		buginf("\nNot requeued");
#endif
	    }
	    timer_enq(timer);
	    timer->tm_invoked++;

	    /* Call the callback routine. */

#ifdef DEBUG_TIMERS
	    buginf("\nInvoking %x", timer);
#endif
	    (*(timer->tm_fun_ptr))(timer->tm_arg1,
				   timer->tm_arg2,
				   timer->tm_arg3);

	    /* If the called function was a hog, rat on him. */

	    if (check_cpuhog()) {
		errmsg(&msgsym(TIMERHOG, SYS), timer->tm_fun_ptr);
	    }

	    /*
	     * Suspend in case the list is long.  If we actually suspended,
	     * we should update the time.
	     */
	    if (process_may_suspend()) {
		GET_TIMESTAMP(current_time);
		COPY_TIMESTAMP(current_time, timers.lasttime);
	    }
	    timer = timers.active_queue;
	}
    }
} /* end of timer_process */


/*
 * timer_init:
 * initialize the timer process
 */
void timer_init (void)
{
    mgd_timer_init_leaf(&timers.next_execution, NULL, 0, NULL, FALSE);
    process_create(timer_process, "Timers", NORMAL_STACK, PRIO_NORMAL);

    /*
     * Initialize timer parser support.
     */
    timers_parser_init();
}/* end of timer_init */

#ifdef DEBUG_TIMERS
static void dump_timer_queue (timer_t *timer)
{
    printf("\n\nTimer   Next   Prev  S   Expire   Interval  T Invoke Name");
    while (timer) {
	printf("\n%-6x %-6x %-6x %c %-#10Ta %-10d %c %-6d %s",
	       timer, timer->tm_next, timer->tm_prev,
	       (timer->tm_state == TMS_RUNNING) ? 'A':'I', 
	       timer->tm_expiration, timer->tm_interval,
	       (timer->tm_type == TMT_ONCE) ? 'O':'C', 
	       timer->tm_invoked, timer->tm_name);
	timer = timer->tm_next;
    }
}
#endif

/*
 * show_timers
 * displays active and idle timers
 */
void show_timers (parseinfo *csb)
{
    timer_t *timer;
    sys_timestamp now;
    int left;
    
    automore_enable("\nState  Handle  interval     due  invoked   missed"
	   "   Process");
    timer = timers.active_queue;
    GET_TIMESTAMP(now);
    while (timer) {
	timer_t *next;
	if (automore_quit())
	    return;
	mem_lock(timer);
	left = CLOCK_DIFF_SIGNED(now, timer->tm_expiration);
	if (left < 0)
	    left = 0;
	printf("\n  A%-10x%-9d%-9d%-8d%-8d     %s",
	       (ulong)timer,
	       timer->tm_interval,
	       left,
	       timer->tm_invoked,
	       timer->tm_missed,
	       timer->tm_name);
	next = timer->tm_next;
	free(timer);
	timer = next;
    }
    timer = timers.idle_queue;
    while (timer) {
	timer_t *next;
	if (automore_quit())
	    return;
	mem_lock(timer);
	printf("\n  I%-10x%-9d        -%-8d%-8d     %s",
	       (ulong)timer,
	       timer->tm_interval,
	       timer->tm_invoked,
	       timer->tm_missed,
	       timer->tm_name);
	next = timer->tm_next;
	free(timer);
	timer = next;
    }
#ifdef DEBUG_TIMERS
    dump_timer_queue(timers.active_queue);
    dump_timer_queue(timers.idle_queue);
#endif
    automore_disable();
} /* end of show_timers */

/* end of timer.c */
