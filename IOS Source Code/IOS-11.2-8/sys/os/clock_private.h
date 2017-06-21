/* $Id: clock_private.h,v 3.1.62.2 1996/08/19 19:01:22 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/os/clock_private.h,v $
 *------------------------------------------------------------------
 * Private files for clock-related stuff, but not purely internal
 *
 * June 1995, Dave katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clock_private.h,v $
 * Revision 3.1.62.2  1996/08/19  19:01:22  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.62.1  1996/04/16  19:09:55  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  12:44:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/16  04:29:15  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CLOCK_PRIVATE_H_
#define _CLOCK_PRIVATE_H_

#define NTP_RESOLUTION 0x80000000 /* Frequency of NTP clock (in Hz) */
#define PIPS_PER_USEC (NTP_RESOLUTION/500000) /* 2^-32 secs per usec */
#define USEC_PER_SEC (1000000)		/* Microseconds per second */
#define NTP_SHIFT_FACTOR 1	/* Shift count to account for resolution */

extern char ntp_clock_status;

extern void do_calendar_update(void);
extern void invoke_clock_set(void (*)(ulong, ulong));
extern void do_read_calendar(void);
extern void set_clock_increment(ulong increment);
extern int clock_slew(int, clock_source);
extern void get_clock_params(ulong *clock_freq, ulong *max_slew, 
			     ulong *precision, ulong *incr, 
			     ulong *basic_incr, ulong *div);
extern void clock_set_leap_second(ulong, int);
extern ulong clock_get_leap_second(int *);

#endif /* _CLOCK_PRIVATE_H_ */
