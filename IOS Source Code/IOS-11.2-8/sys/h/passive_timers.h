/* $Id: passive_timers.h,v 3.6 1996/02/21 01:20:28 hampton Exp $
 * $Source: /release/111/cvs/Xsys/h/passive_timers.h,v $
 *------------------------------------------------------------------
 * Macros and declarations for passive timers
 *
 * June 1995, Dave Katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: passive_timers.h,v $
 * Revision 3.6  1996/02/21  01:20:28  hampton
 * Catch bad offsets to passive timer routines.  [CSCdi49419]
 *
 * Revision 3.5  1995/12/19  00:42:55  hampton
 * Trim some more memory usage out of the timers code.  [CSCdi45208]
 *
 * Revision 3.4  1995/11/23  06:50:36  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/22  09:17:46  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.2  1995/11/16  23:24:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/13  04:07:03  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.7  1995/09/12 16:24:17  hampton
 * Make the unix platform compile again.  [CSCdi39966]
 *
 * Revision 2.6  1995/09/11 23:52:32  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.5  1995/08/25 11:43:35  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.4  1995/07/18 06:45:55  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/07/12 23:43:56  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.2  1995/07/01 03:38:21  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.1  1995/06/16 17:07:31  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Move passive timer declarations to a new file.  Eliminate some obsolete
 * macros.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _PASSIVE_TIMERS_H_
#define _PASSIVE_TIMERS_H_


/*
 * SLEEPING and AWAKE--the most misused and buggy part of the system.
 *
 * NOTE:  PLEASE DO NOT ADD ANY NEW REFERENCES TO "msclock" SINCE THEY
 * SHOULD NO LONGER BE NECESSARY, AND WILL DELAY THE DAY WHEN WE CAN
 * GET OUT OF THIS MESS!!!!
 *
 *
 * There is an inherent ambiguity in the timer system, in that it has
 * only 32 bits of range at 1 msec resolution, which means that it wraps
 * after 49 days.  This means that there is an inherent ambiguity when
 * comparing timestamps, in that they will alias if they are more than
 * 24 days apart (in other words, if something happened 30 days in the
 * past, it will appear to be 19 days in the future unless I have some
 * other context to disambiguate with).
 *
 * This is made worse by the fact that some "timers" are really timestamps
 * in the past (check to see whether timestamp + delay comes after msclock)
 * and some are timestamps in the future (the epoch at which the timer will
 * expire).
 *
 * The SLEEPING and AWAKE macros are intended for timers-in-the-future.
 * They will work properly *only* when it is guaranteed that the time
 * is never more than 24 days before or after msclock.  This effectively
 * means that timers must be set at least once every 24 days, and cannot
 * be set to expire more than 24 days in the future.  A very common bug
 * is to have a very old timer laying around that suddenly appears after
 * 24 days to be armed and ready.
 *
 * For example, if a timer has the value of 100 and msclock has the value
 * 0x81000000, the timer looks to be almost 24 days in the future (instead
 * of slightly more than 24 days in the past).
 *
 * The XSLEEPING and XAWAKE macros reduce the ambiguity.  They require an
 * additional parameter, which is the maximum duration that the timer can
 * ever use.  This reduces the period of ambiguity to the maximum duration
 * of the timer.  This shifts the sequence space around so that it extends
 * only a little bit into the future, but much further into the past.  For
 * example, if the maximum duration of a particular timer is 5 seconds, 
 * XSLEEPING would work properly from just under 49 days in the past
 * until five seconds in the future.  So in this case if a timer has
 * the value 100 and msclock has the value 0x81000000, it will look to
 * be AWAKE rather than SLEEPING.
 *
 * XSLEEPING and XAWAKE add another degree of bug freedom.  If you guess
 * wrong about the max duration of the timer (say, you think it can only
 * be set to run for five seconds, but you set it 10 seconds into the future)
 * then it will appear to be long-expired rather than almost ready to expire.
 * Be careful out there, folks.
 *
 * By the way, all of this stuff works fine across msclock rollovers.
 * Really, it does.  I checked.  You don't need to revisit this every six
 * months.  Trust me.
 *
 * Note:  DO NOT EVER DO ARITHMETIC COMPARES WITH MSCLOCK!  An
 * all-too-common bug is to look for timer expiration by saying:
 *         if (foobar->timer < msclock) ...
 * This is guaranteed to be all screwed up if msclock rolls over in
 * the middle of the period.  USE THE MACROS BELOW TO DETECT TIMER
 * EXPIRATIONS!
 */


/*
 * Timestamp-handling macros
 *
 * Please use the macros;  don't reference the inlines directly.  If
 * we ever manage to move to 64 bit timestamps, the inline calling
 * sequences will change.
 *
 *  CLOCK_IN_INTERVAL(Co, Delay) -
 *	True if current clock is in the interval Co..(Co+Delay-1)
 *	Works for full range of msclock.  Assumes that Co is sometime
 *	in the past (never in the future).  This works for any time
 *	interval up to (49 days - Delay).  If just under 49 days have
 *	elapsed since Co, the macro will return a false positive.
 *
 *  CLOCK_OUTSIDE_INTERVAL(Co, Delay) -
 *	The opposite of CLOCK_IN_INTERVAL.
 *
 *  CLOCK_IN_STARTUP_INTERVAL(Delay) - 
 *      The same as CLOCK_IN_INTERVAL, but is always in reference to
 *      the time that the router was rebooted.
 *
 *  CLOCK_DIFF_UNSIGNED(Co, Cn) - 
 *  CLOCK_DIFF_SIGNED(Co, Cn) -
 *	Calculate Cn - Co accounting for wrap.  If the signed version
 *	is used, it will return a range of -24 days to + 24 days.
 *	Use this if it is unclear about the time relationship between the
 *	two timestamps.  If the unsigned version is used, it
 *	will return a range of 0 to + 49 days.  This is useful if it
 *	is known a priori that the second timestamp is later than the first.
 *
 *  ELAPSED_TIME(timestamp) -
 *	Calculate the amount of time that has elapsed (in milliseconds)
 *	since the timestamp.  The result is an unsigned integer in the
 *	range 0 to 49 days.  If the timestamp is more than 49 days old,
 *	aliasing will result.
 *
 *  TIME_LEFT_SLEEPING(timestamp) -
 *	Calculate the time left sleeping (before we wake up).  Used only
 *	for timers-in-the-future.  Returns a positive integer in the
 *	range 0 to 24 days.  If the timer has expired, it returns 0.
 *	If the timer is more than 24 days in the future or past,
 *	aliasing will occur.
 *
 *  GET_TIMESTAMP(timestamp) -
 *      Note the current time in the timestamp.  For timers-in-the-past.
 *
 * GET_TIMESTAMP32(timestamp) - Retrieve the low order 32 bits of the
 *      current time.  This macro should only be used to supply time
 *      to an external system where the size of a timestamp has been
 *      explicitly defined as 32bits.
 *
 *  COPY_TIMESTAMP(timestamp1, timestamp2) -
 *      Copies timestamp1 into timestamp2.  Quicker, but non-atomic.
 *
 *  COPY_TIMESTAMP_ATOMIC(timestamp1, timestamp2) -
 *      Copies timestamp1 into timestamp2.  Ensures an atomic
 *      operation for the paranoid.
 *
 *  TIMER_LATER(ts1, ts2) -
 *      TRUE if ts1 is later than ts2.  They must be within 24.8 days
 *      of each other.
 *
 *  TIMER_EARLIER(ts1, ts2) -
 *      TRUE if ts1 is earlier than ts2.  They must be within 24.8 days
 *      of each other.
 *
 *
 * Simple timer macros
 *
 *	A series of macros is defined to provide a rudimentary timer
 *	service.  This service uses timers-in-the-future, and thus
 *	supports the use of AWAKE, SLEEPING, etc., along with all of
 *	the risks therein.  These timers are defined as being set to
 *	zero if they are not running, and to the expiration timestamp
 *	(guaranteed to be nonzero) if they are running.
 *
 *  TIMER_STOP(timer) -
 *	Stops a timer (sets it to zero).
 *
 *  TIMER_RUNNING(timer) -
 *	Tests a timer to see if it is active (non-zero) or not.
 *
 *  TIMER_RUNNING_AND_AWAKE(timer) -
 *	Tests a timer to see if it is active (non-zero) and it is
 *      awake.
 *
 *  TIMER_RUNNING_AND_SLEEPING(timer) -
 *	Tests a timer to see if it is active (non-zero) and it is
 *      still sleeping.
 *
 *  TIMER_START(timer, delay) -
 *  TIMER_START64(timer, delay) -
 *	Sets a timer to expire "delay" milliseconds in the future.
 *	TIMER_START64 accepts a 64 bit delay value.
 *
 *  TIMER_START_JITTERED(timer, delay, jitter_pct) -
 *	Sets a timer to expire "delay" milliseconds in the future,
 *	minus a random factor from zero to the specified percentage
 *	of the delay value.  The delay and jitter percentage must
 *      be such that the maximum jitter is greater than 128 msec,
 *      or jitter will not be applied.
 *
 *  TIMER_START_ABSOLUTE(timer, delay) -
 *  TIMER_START_ABSOLUTE64(timer, delay) -
 *	Sets a timer to expire "delay" milliseconds from the time the
 *	router was booted.  This routine should only be used to resolve
 *	timing windows that are caused by the router rebooting.
 *	TIMER_START_ABSOLUTE64 accepts a 64 bit delay value.
 *
 *  TIMER_START_GRANULAR(timer, delay, granularity) -
 *  TIMER_START_GRANULAR64(timer, delay, granularity) -
 *	Sets a timer to expire "delay" milliseconds in the future.
 *      The timer will always be rounded up to a multiple of the
 *      specified granularity.  TIMER_START_GRANULAR64 accepts a 64 bit
 *	delay and granularity values.
 *
 *  TIMER_UPDATE(timer, delay) -
 *  TIMER_UPDATE64(timer, delay) -
 *	Adds an additional "delay" milliseconds to the existing timer.
 *	Note that this macro will do nothing if the timer is stopped.
 *	TIMER_UPDATE64 accepts a 64 bit delay value.
 *
 *  TIMER_UPDATE_JITTERED(timer, delay, jitter_pct) -
 *	Adds an additonal "delay" milliseconds to the existing timer,
 *	minus a random factor from zero to the specified percentage
 *	of the delay value.  Note that this macro will do nothing
 *      if the timer is stopped.  The delay and jitter percentage must
 *      be such that the maximum jitter is greater than 128 msec,
 *      or jitter will not be applied.
 *
 *  TIMER_UPDATE_GRANULAR(timer, delay, granularity) -
 *  TIMER_UPDATE_GRANULAR64(timer, delay, granularity) -
 *	Adds an additonal "delay" milliseconds  to the existing timer.
 *      The timer will always be rounded up to a multiple of the
 *      specified granularity.  TIMER_UPDATE_GRANULAR64 accepts a 64 bit
 *	delay and granularity values.
 *
 *  TIMER_SOONEST(timer1, timer2)
 *	Returns the sooner of the two timers.  If one is not running,
 *      the running one is always returned.  If neither are running,
 *      a stopped timer (0) is returned.
 *
 *  TIMER_ADD_DELTA(timer, delay) -
 *  TIMER_ADD_DELTA64(timer, delay) -
 *      Returns the non-zero timer sum of a timer value and a delta.
 *      In contrast to TIMER_UPDATE, this will work even if timer was
 *      0. Not very efficient! TIMER_ADD_DELTA64 accepts a 64 bit
 *	delay value.
 *
 *  TIMER_SUB_DELTA(timer, delay) -
 *  TIMER_SUB_DELTA64(timer, delay) -
 *      Returns the non-zero timer difference of a timer value and a delta.
 *      In contrast to TIMER_UPDATE, this will work even if timer was
 *      0. Not very efficient, but sometimes useful when displaying timers.
 *	TIMER_SUB_DELTA64 accepts a 64 bit delay value.
 *
 *  TIMERS_EQUAL(timer1, timer2) -
 *      A convenient way of comparing two timers to see if they have
 *      the same expiration time.  This is nothing more than a wrapper
 *      around CLOCK_DIFF_SIGNED, but its easier to read than trying
 *      to decide if the equation should read equal or not-equal.
 *
 *  TIMERS_NOT_EQUAL(timer1, timer2) -
 *      A convenient way of comparing two timers to see if they have
 *      different expiration times.  This is nothing more than a wrapper
 *      around CLOCK_DIFF_SIGNED, but its easier to read than trying
 *      to decide if the equation should read equal or not-equal.
 */

/*
 * I don't like putting these declarations here, but there's an ordering
 * issue between master.h and globs.h.  Such is life.
 */
typedef enum jitter_type_ {MINUS, PLUSMINUS, PLUS} jitter_type;

extern boolean awake(sys_timestamp *then);
extern boolean clock_in_interval(sys_timestamp *Co, ulong Delay);
extern boolean clock_in_startup_interval(ulong Delay);
extern boolean timer_running(sys_timestamp *Tm);
extern boolean timer_running_and_awake(sys_timestamp *Tm);
extern boolean timer_running_and_sleeping(sys_timestamp *Tm);
extern boolean xsleeping(sys_timestamp *then, ulong max);
extern long clock_diff_signed(sys_timestamp *Co, sys_timestamp *Cn);
extern longlong clock_diff_signed64(sys_timestamp *Co, sys_timestamp *Cn);
extern sys_timestamp timer_soonest(sys_timestamp *timer1, sys_timestamp *timer2);
extern ulong clock_diff_unsigned(sys_timestamp *Co, sys_timestamp *Cn);
extern ulong elapsed_time(volatile sys_timestamp *timestamp);
extern ulong time_left_sleeping(sys_timestamp *timestamp);
extern ulonglong clock_diff_unsigned64(sys_timestamp *Co, sys_timestamp *Cn);
extern ulonglong elapsed_time64(sys_timestamp *timestamp);
extern ulonglong time_left_sleeping64(sys_timestamp *timestamp);
extern void get_timestamp(volatile sys_timestamp *timestamp);
extern void get_timestamp32(volatile ulong *timestamp32);
extern void pak_get_ts(void *addr, void *timestamp);
extern void pak_put_ts(void *addr, void *timestamp);
extern sys_timestamp timer_add_delta(sys_timestamp *timer, long delta);
extern sys_timestamp timer_add_delta64(sys_timestamp *timer, longlong delta);
extern void timer_add_granular(sys_timestamp *Tm, long Dt, ulong Gr);
extern void timer_add_granular64(sys_timestamp *Tm, ulonglong Dt, ulonglong Gr);
extern void timer_start(sys_timestamp *Tm, long Dt);
extern void timer_start64(sys_timestamp *Tm, ulonglong Dt);
extern void timer_start_absolute(sys_timestamp *tm, ulong Dt);
extern void timer_start_absolute64(sys_timestamp *tm, ulonglong Dt);
extern void timer_start_granular(sys_timestamp *Tm, long Dt, ulong Gr);
extern void timer_start_granular64(sys_timestamp *Tm, ulonglong Dt, ulonglong Gr);
extern void timer_start_jitter(sys_timestamp *, ulong, ulong, jitter_type);
extern void timer_update(sys_timestamp *Tm, long Dt);
extern void timer_update64(sys_timestamp *Tm, ulonglong Dt);
extern void timer_update_jitter(sys_timestamp *Tm, long Dt, ulong jitter_pct);

#define GET_TIMESTAMP(timestamp) get_timestamp(&timestamp)
#define GET_TIMESTAMP32(timestamp32) get_timestamp32(&timestamp32)
#define COPY_TIMESTAMP(timestamp1, timestamp2) ((timestamp2) = (timestamp1))
#define COPY_TIMESTAMP_ATOMIC(timestamp1, timestamp2) \
		copy_timestamp_atomic(&timestamp1, &timestamp2)

#define CLOCK_DIFF_SIGNED(Co, Cn) (clock_diff_signed(&(Co), &(Cn)))
#define CLOCK_DIFF_SIGNED64(Co, Cn) (clock_diff_signed64(&(Co), &(Cn)))
#define CLOCK_DIFF_UNSIGNED(Co, Cn) (clock_diff_unsigned(&(Co), &(Cn)))
#define CLOCK_DIFF_UNSIGNED64(Co, Cn) (clock_diff_unsigned64(&(Co), &(Cn)))
#define CLOCK_IN_INTERVAL(Co,Delay) clock_in_interval(&(Co), (Delay))
#define CLOCK_OUTSIDE_INTERVAL(Co,Delay) (!CLOCK_IN_INTERVAL((Co),(Delay)))
#define CLOCK_IN_STARTUP_INTERVAL(Delay) clock_in_startup_interval((Delay))

#define AWAKE(then)	(awake(&(then)))
#define SLEEPING(then)	(!awake(&(then)))
#define XSLEEPING(then,max) (xsleeping(&(then), (max)))
#define XAWAKE(then,max) (!xsleeping(&(then),(max)))

#define ELAPSED_TIME(timestamp) (elapsed_time(&(timestamp)))
#define ELAPSED_TIME64(timestamp) (elapsed_time64(&(timestamp)))
#define TIME_LEFT_SLEEPING(T) time_left_sleeping(&(T))
#define TIME_LEFT_SLEEPING64(T) time_left_sleeping64(&(T))

#define TIMERS_EQUAL(T1,T2) (TS_VALUE(T1) == TS_VALUE(T2))
#define TIMERS_NOT_EQUAL(T1,T2) (TS_VALUE(T1) != TS_VALUE(T2))
#define TIMER_EARLIER(ts1, ts2) (clock_diff_signed64(&(ts2), &(ts1)) < 0)
#define TIMER_LATER(ts1, ts2) (clock_diff_signed64(&(ts1), &(ts2)) < 0)
#define TIMER_RUNNING(Tm) timer_running(&(Tm))
#define TIMER_RUNNING_AND_AWAKE(Tm) (timer_running_and_awake(&(Tm)))
#define TIMER_RUNNING_AND_SLEEPING(Tm) (timer_running_and_sleeping(&(Tm)))
#define TIMER_SOONEST(timer1, timer2) timer_soonest(&(timer1), &(timer2))

#define TIMER_START(Tm, Dt) {timer_start(&(Tm), Dt);}
#define TIMER_START64(Tm, Dt) {timer_start64(&(Tm), Dt);}
#define TIMER_START_ABSOLUTE(Tm, Dt) {timer_start_absolute(&(Tm), Dt);}
#define TIMER_START_ABSOLUTE64(Tm, Dt) {timer_start_absolute64(&(Tm), Dt);}
#define TIMER_START_GRANULAR(Tm, Dt, Gr) (timer_start_granular(&(Tm), Dt, Gr))
#define TIMER_START_GRANULAR64(Tm, Dt, Gr) (timer_start_granular64(&(Tm), Dt, Gr))
#define TIMER_START_JITTERED(Tm, Dt, Jitter) timer_start_jitter(&(Tm), Dt, Jitter, MINUS)
#define TIMER_STOP(Tm)       { TS_VALUE(Tm) = 0; }
#define TIMER_UPDATE(Tm, Dt) timer_update(&(Tm), Dt);
#define TIMER_UPDATE64(Tm, Dt) timer_update64(&(Tm), Dt);
#define TIMER_UPDATE_GRANULAR(Tm, Dt, Gr) (timer_add_granular(&(Tm), Dt, Gr))
#define TIMER_UPDATE_GRANULAR64(Tm, Dt, Gr) (timer_add_granular64(&(Tm), Dt, Gr))
#define TIMER_UPDATE_JITTERED(Tm, Dt, Jitter) timer_update_jitter(&(Tm), (Dt), (Jitter))
#define TIMER_ADD_DELTA(Tm, Dt) timer_add_delta(&(Tm), Dt)
#define TIMER_ADD_DELTA64(Tm, Dt) timer_add_delta64(&(Tm), Dt)
#define TIMER_SUB_DELTA(Tm, Dt) timer_add_delta(&(Tm), -(Dt))
#define TIMER_SUB_DELTA64(Tm, Dt) timer_add_delta64(&(Tm), -(Dt))


#define PAK_GET_TS(addr, value) pak_get_ts(addr, value)
#define PAK_PUT_TS(addr, val)   pak_put_ts(addr, &val)

/*
 * os/clock.c
 */
extern ulong system_uptime_seconds(void);

#endif /* _PASSIVE_TIMERS_H_ */
