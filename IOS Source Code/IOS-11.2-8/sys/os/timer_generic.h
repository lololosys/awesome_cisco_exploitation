/* $Id: timer_generic.h,v 3.2 1996/01/06 03:22:50 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/timer_generic.h,v $
 *------------------------------------------------------------------
 * timer_generic.h - moved from if_timer.h
 *
 * November 1992, Dit Morse
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_generic.h,v $
 * Revision 3.2  1996/01/06  03:22:50  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/06  00:12:08  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 * Revision 3.3  1995/12/21  16:56:45  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:41:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:37:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* These routines are used to calibrate the enivronment (speed)
 * for short spin waits based on a tight count loop.
 *
 * The technique (see routines for more detail) is:
 *
 * call timer_calibrate with a target time in usecs.
 *  - it will calibrate the spin loop for
 *    the environment so that calls to the routine
 *    wastetime will wait for the desired time.
 *
 *  - call wastetime(n) whenever you wish to wait for t usecs.
 *
 * Notes:
 * 
 *  1. you must call timer_calibrate in the same environment
 *     that you wish to spin wait in.  Specifically of note are:
 * 
 *      - datacache On/Off,  
 *      - instruction cache On/Off 
 *        we always run with both these on, except for flash programming
 *      - memory bus width
 *      - run from RAM, ROM, Flash
 * 
 *  2. calibrate turns off interrupts, and stuffs values to the
 *     nmi timer (counter 2) and watchdog (counter 0) so they will
 *     not interrupt and skew the calibration.  This will result 
 *     in a possible slight glitch in the 4ms timer - which will
 *     run up to 4ms longer during calibration.
 * 
 *  3. wastetime does not turnoff interrupts, or mess with the
 *     timers.  Consequently the spin wait may be longer if
 *     such interrupts occur.  The 4ms interrupt seems to take
 *     30usecs on a 25mHz 68030.
 * 
 * 
 * Calibration technique:
 *
 * The process consists of 
 * 1.  timing the spin loop using the 8254 (counter 1) 
 *     for two values of n; currently values of 100 and 1000, 
 *     determined empirically.
 *
 * 2.  using thes value to solve for a0 and a1, in the
 *     equation:
 *               t = a0 + a1*n
 *
 * 3.  using those values to solve for n given t.
 *
 *
 * An example, using timer_print, from a merlot running from RAM:

    calibration counts and times
            t = a0 + a1 * n
        (n = 0 is call/ret overhead)

    usec=10  usec measured=10, count=12
    
    t0 = 3, a0*100 = 278, a1*100 = 65
    N      t     CalcA0  CalcT0
    0      3       2         3
    10     10      9         9
    100    68      68        68
    1000   655     655       655

 * Notes on the above:
 * 
 * usec=10
 *     the goal is 10 usec, and the timer was calibrate for this.
 * count=12
 *     the calculated count was was 12.
 *     the calculated count has one added to it to insure
 *     that at least the desired time is attained.
 * usec measured=10,
 *     the measured time in the spin loop, using the 8254, was 10 usecs.
 *
 * t0 = 3
 *     t0 is the time to run the spinloop (wastetime(0);) for a
 *     count of zero.  neglecting the time due to starting and
 *     latching the timer, it should measure  the call/ret
 *     overhead, plus the time to determine the count is
 *     zero and bail.
 * a0 = 278
 *     The is the calulated t0 - i.e. - where the t-axis crossing
 *     occurs.  Scaled * 100 to retain precision.
 * CalcA0
 *     CalcA0 = a0 + a1 * n     - no rounding.
 * CalcT0
 *     CalcT0 = t0 + a1 * n     - no rounding.
 *
 * 
 * Code:
 *  the code is currently located in the file if/flash_lesdvr.c
 *  since it was built for use by the flash erase/programming
 *  code.  If this code is of general value, it can be moved
 *  to some general place.
 *
 */

 /* Usage notes:
  *
  * this file must be included.
  * the manufacturer's recommended usage is:

  void routine() {
       ....
      ulong 22usecs;        / * will hold count for 22 usec loop * /
      ...
      22usecs = timer_calibrate(22, FALSE);  / * calibrate for 22 usecs * /
      ...

      wastetime(22usecs);                   / * 22 usec spin loop * /

      }
 *
 */

/*
 * Data structures
 */
typedef struct timer_data_t_ {
    long a0;
    long a1;
    long t0;
    long t_del;
    long c_del;
} timer_data_t;

/*
 * Prototypes
 */
extern ulong timer_calibrate(ulong, boolean);	/* usecs and print flag */

extern int time_it(int);
extern void restart_t2(void);
extern void restart_watchdog(int);

/* End of file */
