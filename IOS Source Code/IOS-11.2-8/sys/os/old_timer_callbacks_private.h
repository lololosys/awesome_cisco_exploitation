/* $Id: old_timer_callbacks_private.h,v 3.2 1996/01/05 10:18:04 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/old_timer_callbacks_private.h,v $
 *------------------------------------------------------------------
 * Private definitions for timer support
 *
 * December 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * The timer structures are defined here.  This should only be included
 * in internal timer code--the callers should only be able to see a
 * non-dereferencable pointer (courtesy of globs.h).
 *------------------------------------------------------------------
 * $Log: old_timer_callbacks_private.h,v $
 * Revision 3.2  1996/01/05  10:18:04  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/05  09:39:40  hampton
 * New files for old timer code.
 *
 * Revision 3.2  1995/11/17  18:52:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:59:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Timer states */

typedef enum timer_states_ {
    TMS_IDLE,			/* Timer is idle */
    TMS_RUNNING,		/* Timer is running */
    TMS_UNUSED                  /* Timer is unused */
} timer_states;

/*
 * Timer structure
 */
struct timer_t_ {
    struct timer_t_ *tm_next;	/* Pointer to next entry */
    struct timer_t_ **tm_prev;	/* Pointer to previous entry's next pointer */
    timer_states tm_state;	/* Timer state (TMS_...) */
    sys_timestamp tm_expiration; /* Expiration timestamp */
    ulong tm_interval;		/* Timer interval */
    ulong tm_jitter_pct;	/* Jitter percentage or zero */
    ulong tm_maxjitter;		/* Maximum jitter to subtract */
    timer_types tm_type;	/* Timer type (TMT_...)  */
    ulong tm_invoked;		/* Number of times invoked */
    ulong tm_missed;		/* Number of times interval was missed */
    char *tm_name;		/* Name of timer */
    timer_func tm_fun_ptr;	/* Function to call when the timer expires */
    void *tm_arg1;		/* Arguments to call function with */
    void *tm_arg2;
    void *tm_arg3;
};

void timers_parser_init(void);
