/* $Id: clock_internal.h,v 3.4 1995/12/01 05:22:50 dkatz Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/clock_internal.h,v $
 *------------------------------------------------------------------
 * Internal definitions for clock routines
 *
 * May 1992, Dave Katz.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: clock_internal.h,v $
 * Revision 3.4  1995/12/01  05:22:50  dkatz
 * CSCdi43033:  Summer time epochs may not be updated
 *
 * Revision 3.3  1995/11/17  18:45:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:29:14  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:54:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file is for the private use of the clock code.
 */

extern ulong secs_since_boot;
extern int sys_tz;
extern char *sys_tz_name;

extern void clock_tick(void);
extern void init_kernel_clock(void);
#ifdef NOBODY_CALLS_ME
extern void update_time_zone(timeval *, long);
#endif /* NOBODY_CALLS_ME */
extern void calculate_summer_time_epochs(clock_epoch *);
extern void check_for_summer_time(clock_epoch *);
extern void clock_set_time_source(clock_source);
extern void clock_register_printf(void);
extern void get_summertime_info(boolean, char **, long *);
extern void local_time_string(char *, clock_epoch *, boolean,
			      summer_time_selection_type);

extern volatile clock_epoch system_clock;
