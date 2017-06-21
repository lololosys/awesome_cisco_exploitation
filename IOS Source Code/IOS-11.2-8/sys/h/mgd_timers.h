/* $Id: mgd_timers.h,v 3.5 1996/02/19 21:32:49 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/h/mgd_timers.h,v $
 *------------------------------------------------------------------
 * Managed Timer definitions
 *
 * December 1994, Dave Katz
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mgd_timers.h,v $
 * Revision 3.5  1996/02/19  21:32:49  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.4  1995/12/22  21:58:24  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  09:13:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/23  03:56:56  dkatz
 * CSCdi42575:  Managed timers could be more robust
 * Try to avoid interpreting garbage as indicating that the managed
 * timer has been initialized.
 *
 * Revision 2.2  1995/06/18  05:31:31  hampton
 * Close a small timing window in the scheduler's calls to the
 * mgd_timer_link and mgd_timer_delink routines.  [CSCdi35906]
 *
 * Revision 2.1  1995/06/07 20:36:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _MGD_TIMERS_H_
#define _MGD_TIMERS_H_

/*
 * mgd_timer_is_leaf
 *
 * Returns TRUE if managed timer is a leaf
 */
static inline boolean mgd_timer_is_leaf (mgd_timer *timer)
{
    return(timer->mt_leaf);
}

/*
 * mgd_timer_is_parent
 *
 * Returns TRUE if managed timer is a parent
 */
static inline boolean mgd_timer_is_parent (mgd_timer *timer)
{
    return(!(timer->mt_leaf));
}

/*
 * mgd_timer_expired
 *
 * Returns TRUE if managed timer is running and expired
 */
static inline boolean mgd_timer_expired (mgd_timer *timer)
{
    sys_timestamp exptime;
    exptime = timer->mt_exptime;
    return(TIMER_RUNNING_AND_AWAKE(exptime));
}

/*
 * mgd_timer_type
 *
 * Returns the timer type of this timer
 */
static inline ushort mgd_timer_type (mgd_timer *timer)
{
    return(timer->mt_type);
}

/*
 * mgd_timer_context
 *
 * Returns the context of this timer
 */
static inline void *mgd_timer_context (mgd_timer *timer)
{
    if (timer->mt_leaf)
	return(timer->mt_union.mt_context);
    else
	return(NULL);
}

/*
 * mgd_timer_additional_context
 * 
 * Returns additional context from this timer.
 */
static inline void *mgd_timer_additional_context (mgd_timer *timer,
						  ulong context_index)
{
    return(timer->mt_additional_context[context_index]);
}

/*
 * mgd_timer_set_additional_context
 *
 * Stores additional context information in this timer.
 */
static inline void mgd_timer_set_additional_context (mgd_timer *timer,
						     ulong context_index,
						     void *context_info)
{
    timer->mt_additional_context[context_index] = context_info;
}

/*
 * mgd_timer_exp_time
 *
 * Returns the expiration time of this timer
 */
static inline sys_timestamp mgd_timer_exp_time (mgd_timer *timer)
{
    return(timer->mt_exptime);
}

/*
 * mgd_timer_left_sleeping
 *
 * Returns the time in milliseconds left sleeping of this timer
 *
 * Returns zero if the timer isn't running.
 */
static inline long mgd_timer_left_sleeping (mgd_timer *timer)
{
    sys_timestamp exptime;
    exptime = timer->mt_exptime;
    if (!TIMER_RUNNING(exptime))
	return(0);
    return(TIME_LEFT_SLEEPING(exptime));
}

/*
 * mgd_timer_left_sleeping64
 *
 * Returns the time in milliseconds left sleeping of this timer
 *
 * Returns zero if the timer isn't running.
 */
static inline longlong mgd_timer_left_sleeping64 (mgd_timer *timer)
{
    sys_timestamp exptime;
    exptime = timer->mt_exptime;
    if (!TIMER_RUNNING(exptime))
	return(0);
    return(TIME_LEFT_SLEEPING64(exptime));
}

/*
 * mgd_timer_running
 *
 * Returns TRUE if the timer is running
 */
static inline boolean mgd_timer_running (mgd_timer *timer)
{
    return(TIMER_RUNNING(timer->mt_exptime));
}

/*
 * mgd_timer_running_and_sleeping
 *
 * Returns TRUE if the timer is running and unexpired
 */
static inline boolean mgd_timer_running_and_sleeping (mgd_timer *timer)
{
    sys_timestamp exptime;
    exptime = timer->mt_exptime;
    return(TIMER_RUNNING_AND_SLEEPING(exptime));
}

/*
 * mgd_timer_initialized
 *
 * Returns TRUE if the timer has been initialized
 */
static inline boolean mgd_timer_initialized (mgd_timer *timer)
{
    return(timer->mt_initialized == MT_INITIALIZED_MAGIC);
}

extern void mgd_timer_init_parent(mgd_timer *timer, mgd_timer *master);
extern void mgd_timer_init_leaf(mgd_timer *timer, mgd_timer *master,
				ushort type, void *context,
				boolean interrupt_environ);
extern void mgd_timer_set_soonest(mgd_timer *timer, sys_timestamp time);
extern void mgd_timer_start(mgd_timer *timer, ulong delay);
extern void mgd_timer_start_jittered(mgd_timer *timer, ulong delay,
				     ulong jitter_pct);
extern void mgd_timer_update(mgd_timer *timer, ulong delay);
extern void mgd_timer_update_jittered(mgd_timer *timer, ulong delay,
				      ulong jitter_pct);
extern void mgd_timer_stop(mgd_timer *timer);
extern mgd_timer *mgd_timer_first_expired(mgd_timer *timer);
extern mgd_timer *mgd_timer_first_running(mgd_timer *timer);
extern void mgd_timer_set_context(mgd_timer *timer, void *context);
extern void mgd_timer_set_type(mgd_timer *timer, ushort type);
extern void mgd_timer_set_exptime(mgd_timer *timer, sys_timestamp *time);
extern void mgd_timer_delink(mgd_timer **timer);
extern void mgd_timer_link(mgd_timer *timer, mgd_timer *parent,
			   mgd_timer **shadow, boolean interrupt_environ);
extern mgd_timer *mgd_timer_next_running(mgd_timer *timer);
extern mgd_timer *mgd_timer_first_child(mgd_timer *timer);
extern void mgd_timer_set_fenced(mgd_timer *timer, boolean state);
extern mgd_timer *mgd_timer_first_fenced(mgd_timer *timer);
extern void mgd_timer_change_to_leaf(mgd_timer *timer);
extern void mgd_timer_change_to_parent(mgd_timer *timer);
#define MGD_TIMER_DISPLAY_BUFMAX 80
extern void mgd_timer_display_tree(mgd_timer *timer,
				   char *(*)(mgd_timer *timer, ushort,
					     char *buf));

#endif					/* _MGD_TIMERS_H_ */
