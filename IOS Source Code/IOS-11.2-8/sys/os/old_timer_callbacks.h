/* $Id: old_timer_callbacks.h,v 3.2 1996/01/05 10:18:02 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/old_timer_callbacks.h,v $
 *------------------------------------------------------------------
 * Public definitions for timer support
 *
 * December 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: old_timer_callbacks.h,v $
 * Revision 3.2  1996/01/05  10:18:02  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/05  09:39:39  hampton
 * New files for old timer code.
 *
 * Revision 3.2  1995/11/17  09:13:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:36:55  hampton
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
 * prevent multiple inclusion
 */
#ifndef __OLD_TIMER_CALLBACKS_H__
#define __OLD_TIMER_CALLBACKS_H__

/* Timer types */

typedef enum timer_types_ {
    TMT_ONCE,			/* One-shot timer */
    TMT_CONTINUOUS		/* Repeating timer */
} timer_types;

/*
 * Function prototypes for exported functions...
 */
void set_timer(timer_t *handle, ulong value);
void set_timer_interval(timer_t *handle, ulong value);
void jitter_timer(timer_t *handle, ulong jitter_percent);
void restart_timer(timer_t *handle);
void stop_timer(timer_t *handle);
void change_timer_arg(timer_t *, void *, void *, void *, void *);
timer_t *create_timer(void *, void *, void *, void *, char *, 
		      timer_types);

void destroy_timer(timer_t *handle);
boolean active_timer(timer_t *handle);
ulong get_interval(timer_t *handle);
ulong get_due_time(timer_t *handle);

void timer_init(void);

#endif			/* #ifndef __OLD_TIMER_CALLBACKS_H__ */
