/* $Id: passive_timers.c,v 3.1.16.2 1996/08/28 13:03:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/passive_timers.c,v $
 *------------------------------------------------------------------
 * Utility routines for manipuating/printing millisecond based time
 * values.
 *
 * September 1995, David Hampton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: passive_timers.c,v $
 * Revision 3.1.16.2  1996/08/28  13:03:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.1  1996/03/18  21:30:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.2  1996/03/16  07:21:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.6.1  1996/03/09  05:10:03  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:07  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../dev/monitor1.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#define TSP_VALUE(x) ((x)->u.value)

static ulong clockdummy = 0;	/* Fake clock until we're set up */
static volatile ulong *clockptr = &clockdummy;  /* pointer to 32-bit millisecond clock */
static volatile ulong *hclockptr = &clockdummy; /* pointer to upper 32-bit ms clock */


/**********************************************************************
 *
 *			 Clock Initialization
 *
 **********************************************************************/

boolean system_clock_init (void)
{
    clockptr = GET_CLOCKADDR;	/* get address of low 32-bits of clock */
    if ((long)clockptr == -1)	/* check for old bootstrap */
	return(FALSE);
    hclockptr = GET_HCLOCKADDR;	/* get address of high 32-bits of clock */
    if ((long)hclockptr == -1)	/* check for old bootstrap */
	return(FALSE);
    /*
     * Init timers to zero. Some platforms such as Cancun with
     * old boot monitor and FLH don't properly rezero the clock.
     * See CSCdi29515.
     */
    *clockptr = 0;
    *hclockptr = 0;

#ifdef MSCLOCK_STARTVALUE
    /* used to simulate large uptimes */
    *clockptr = MSCLOCK_STARTVALUE;
    *hclockptr = MSCLOCK_HSTARTVALUE;
#endif
    return(TRUE);
}

/**********************************************************************
 *
 *			Clock Reading Routtines
 *
 **********************************************************************/

/*
 * get_timestamp_inline
 * get_timestamp32
 *
 * Recored the current system time.
 */
static inline void get_timestamp_inline (volatile sys_timestamp *timestamp)
{
    ulong temp;

    timestamp->u.p.low = *clockptr;
    timestamp->u.p.high = *hclockptr;
    temp = *clockptr;
    if (temp < timestamp->u.p.low) {
	timestamp->u.p.low = temp;
	timestamp->u.p.high = *hclockptr;
    }
}

static inline void get_timestamp32_inline (volatile ulong *timestamp32)
{
    *timestamp32 = *clockptr;
}

void get_timestamp (volatile sys_timestamp *timestamp)
{
    get_timestamp_inline(timestamp);
}

void get_timestamp32 (volatile ulong *timestamp32)
{
    get_timestamp32_inline(timestamp32);
}

/*
 * snmp_sysUpTime
 * Returns the time (in hundredths of a second) since the system was reset.
 * This is the standard time reference for network management applications.
 */

ulong snmp_sysUpTime (void)
{
    sys_timestamp now;
    
    get_timestamp_inline(&now);
    return((TS_VALUE(now)/SYSUPTIME_SCALE) & MAXULONG);
}

/*
 * timer_calculate_jitter
 *
 * Calculate jitter for the jittered timer routines.
 *
 * This used to be an inline, but it's pretty silly to do that, given
 * the size and time that it takes.
 *
 * Routine for calculating jitter.  The parameter says whether the jitter
 * factor should be added, subtracted, or spread around the nominal
 * value.  In general, doing anything but subtracting is a bad idea
 * (due to lengthening clock periods beyond their published values),
 * so if you are really going to use this, know what you're doing, and
 * write your own macro...
 */
ulong jitter_seed;		/* Seed value for timer jitter calculations */
static ulong timer_calculate_jitter (
    ulong delay,
    ulong jitter_pct,
    jitter_type jittertype)
{
    ulong jitter_value;
    ulong jitter_range;
    ulong random_value;

    /*
     * Avoid bogus jitter percentages.
     */
    if (jitter_pct > 99) {
	return(delay);
    }

    /*
     * Calculate the maximum jitter amount (being careful about
     * truncation error).  If the range is too small to jitter, forget it.
     */
    if (delay > 0xffffffff/100) {	/* Divide first? */
	jitter_range = (delay / 100) * jitter_pct;
    } else {
	jitter_range = (delay * jitter_pct) / 100;
    }
    if (jitter_range < 2)		/* Can't really jitter */
	return(delay);

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
    get_timestamp32_inline(&random_value);
    jitter_seed = (jitter_seed << 4) ^ random_value;
    jitter_value = jitter_seed % jitter_range;

    /* Modify the value accordingly. */

    switch (jittertype) {
      case MINUS:
	return(delay - jitter_value);

      case PLUS:
	return(delay + jitter_value);

      case PLUSMINUS:
	return(delay + jitter_value - (jitter_range / 2));

	/*
	 * Shouldn't happen but is required for compiler joy.
	 */
      default:
	return(delay);
    }
}

/**********************************************************************
 *
 *			Private Timer Inlines
 *
 **********************************************************************/

static inline ulonglong clock_diff_unsigned64_inline (
    volatile sys_timestamp *Co,
    volatile sys_timestamp *Cn)
{
    return((ulonglong)(TSP_VALUE(Cn) - TSP_VALUE(Co)));
}

static inline ulong clock_diff_unsigned_inline (
    volatile sys_timestamp *Co,
    volatile sys_timestamp *Cn)
{
    ulonglong difference;

    difference = clock_diff_unsigned64_inline(Co, Cn);
    if (difference > MAXULONG)
	return(MAXULONG);
    return(difference & 0xFFFFFFFF);
}

static inline longlong clock_diff_signed64_inline (sys_timestamp *Co,
						   sys_timestamp *Cn)
{
    return((longlong)(TSP_VALUE(Cn) - TSP_VALUE(Co)));
}

static inline long clock_diff_signed_inline (sys_timestamp *Co, sys_timestamp *Cn)
{
    longlong difference;

    difference = clock_diff_signed64_inline(Co, Cn);
    if (difference > MAXLONG)
	return(MAXLONG);
    if (difference < -MAXLONG)
	return(-MAXLONG);
    return(difference & 0xFFFFFFFF);
}

static inline boolean awake_inline (sys_timestamp *then)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return(clock_diff_signed64_inline(&now, then) <= 0);
}

static inline boolean timer_running_inline (sys_timestamp *Tm)
{
    return(TS_VALUE(*Tm) != 0);
}


/**********************************************************************
 *
 *		     Timer Manipulation Routines
 *
 * These have been migrated here from passive_timers.h due to image
 * bloat.  This got out of hand when timers were changed from 32 bit
 * quantities to 64 bit quantities.
 *
 **********************************************************************/

/*
 * copy_timestamp_atomic
 *
 * Perform an atomic copy of one timestamp to another.
 */
void copy_timestamp_atomic (
    sys_timestamp *timestamp1,
    sys_timestamp *timestamp2)
{
    leveltype level;

    level = disable_interrupts();
    *timestamp2 = *timestamp1;
    enable_interrupts(level);
}

/*
 * clock_diff_unsigned64
 * clock_diff_unsigned
 * clock_diff_signed64
 * clock_diff_signed
 *
 * Compute the difference between two timestamps.
 */
ulonglong clock_diff_unsigned64 (sys_timestamp *Co, sys_timestamp *Cn)
{
    return(clock_diff_unsigned64_inline (Co, Cn));
}

ulong clock_diff_unsigned (sys_timestamp *Co, sys_timestamp *Cn)
{
    return(clock_diff_unsigned_inline (Co, Cn));
}

longlong clock_diff_signed64 (sys_timestamp *Co, sys_timestamp *Cn)
{
    return(clock_diff_signed64_inline(Co, Cn));
}

long clock_diff_signed (sys_timestamp *Co, sys_timestamp *Cn)
{
    return(clock_diff_signed_inline(Co, Cn));
}

/*
 * awake
 * xsleeping - no longer neececessary with 292,271,023 year wrap times
 * clock_in_interval
 * clock_in_startup_interval
 *
 * Has a timestamp expired?
 */
boolean awake (sys_timestamp *then)
{
    return(awake_inline(then));
}

boolean xsleeping (sys_timestamp *then, ulong max)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return((clock_diff_unsigned64_inline(&now, then) - 1) <= max);
}

boolean clock_in_interval (sys_timestamp *Co, ulong Delay)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return((clock_diff_unsigned64_inline(Co, &now)) < Delay);
}

boolean clock_in_startup_interval (ulong Delay)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return(TS_VALUE(now) < Delay);
}

/*
 * elapsed_time64
 * elapsed_time
 * time_left_sleeping64
 * time_left_sleeping
 *
 * How long since/until a timer expires.
 */
ulonglong elapsed_time64 (sys_timestamp *timestamp)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return(clock_diff_unsigned64_inline(timestamp, &now));
}

ulong elapsed_time (volatile sys_timestamp *timestamp)
{
    sys_timestamp now;

    get_timestamp_inline(&now);
    return(clock_diff_unsigned_inline(timestamp, &now));
}

ulonglong time_left_sleeping64 (sys_timestamp *timestamp)
{
    sys_timestamp now;
    longlong remaining;

    get_timestamp_inline(&now);
    remaining = clock_diff_signed64_inline(&now, timestamp);
    if (remaining < 0)
	remaining = 0;
    return(remaining);
}

ulong time_left_sleeping (sys_timestamp *timestamp)
{
    sys_timestamp now;
    longlong remaining;

    get_timestamp_inline(&now);
    remaining = clock_diff_signed64_inline(&now, timestamp);
    if (remaining > MAXLONG)
	return(MAXLONG);
    if (remaining < 0)
	remaining = 0;
    return(remaining & 0xFFFFFFFF);
}

/*
 * timer_running
 * timer_running_and_awake
 * timer_running_and_sleeping 
 *
 * Check to see if a timer is running, and also check some other
 * condition.
 */
boolean timer_running (sys_timestamp *Tm)
{
    return(timer_running_inline(Tm));
}

boolean timer_running_and_awake (sys_timestamp *Tm)
{
    return(timer_running_inline(Tm) && awake_inline(Tm));
}

boolean timer_running_and_sleeping (sys_timestamp *Tm)
{
    return(timer_running_inline(Tm) && !awake_inline(Tm));
}

sys_timestamp timer_soonest (
    sys_timestamp *timer1,
    sys_timestamp *timer2)
{
    if (!timer_running_inline(timer2))
	return(*timer1);
    if (timer_running_inline(timer1) &&
	(clock_diff_signed64_inline(timer2, timer1) < 0)) 
	return(*timer1);
    else
	return(*timer2);
}

/*
 * timer_start
 * timer_start64
 * timer_start_absolute
 * timer_start_absolute64
 * timer_add_delta
 * timer_add_delta64
 * timer_start_jitter
 * timer_add_jitter
 * timer_start_granular 
 * timer_start_granular64 
 * timer_add_granular
 * timer_add_granular64
 *
 * Manipulate the expiration time of a timestamp.
 */
void timer_start (sys_timestamp *Tm, long Dt)
{
    if (Dt < 0) {
	(*kernel_errmsg)(&msgsym(TIMERNEG, SYS), Tm, Dt);
	Dt = 1;
    }
    get_timestamp_inline(Tm);
    TSP_VALUE(Tm) += Dt;
}

void timer_start64 (sys_timestamp *Tm, ulonglong Dt)
{
    get_timestamp_inline(Tm);
    TSP_VALUE(Tm) += Dt;
}

void timer_start_absolute (sys_timestamp *Tm, ulong Dt)
{
    TSP_VALUE(Tm) = Dt;
}

void timer_start_absolute64 (sys_timestamp *Tm, ulonglong Dt)
{
    TSP_VALUE(Tm) = Dt;
}

sys_timestamp timer_add_delta (sys_timestamp *timer, long delta)
{
    sys_timestamp temp;

    TS_VALUE(temp) = TSP_VALUE(timer) + delta;
    if (TS_VALUE(temp) == 0) 
	TS_VALUE(temp) = 1;
    return(temp);
}

sys_timestamp timer_add_delta64 (sys_timestamp *timer, longlong delta)
{
    sys_timestamp temp;

    TS_VALUE(temp) = TSP_VALUE(timer) + delta;
    if (TS_VALUE(temp) == 0) 
	TS_VALUE(temp) = 1;
    return(temp);
}

void timer_update (sys_timestamp *Tm, long Dt)
{
    if (Dt < 0) {
	(*kernel_errmsg)(&msgsym(TIMERNEG, SYS), Tm, Dt);
	Dt = 1;
    }
    if (!timer_running_inline(Tm))
	return;
    TSP_VALUE(Tm) += Dt;
}

void timer_update64 (sys_timestamp *Tm, ulonglong Dt)
{
    if (!timer_running_inline(Tm))
	return;
    TSP_VALUE(Tm) += Dt;
}

void timer_start_jitter (
    sys_timestamp *Tm,
    ulong delay,
    ulong jitter_pct,
    jitter_type jittertype) 
{
    ulong jittered_delay;

    get_timestamp_inline(Tm);
    jittered_delay = timer_calculate_jitter(delay, jitter_pct, jittertype);
    TSP_VALUE(Tm) += jittered_delay;
}

/*
 * Inline for adding jitter to a timestamp.  Returns the new timestamp
 * value, minus jitter.
 */
void timer_update_jitter (
    sys_timestamp *Tm,
    long Dt,
    ulong jitter_pct)
{
    ulong jittered_delay;

    if (Dt < 0) {
	(*kernel_errmsg)(&msgsym(TIMERNEG, SYS), Tm, Dt);
	Dt = 1;
    }
    if (!timer_running_inline(Tm))
	return;

    jittered_delay = timer_calculate_jitter(Dt, jitter_pct, MINUS);
    TSP_VALUE(Tm) += jittered_delay;
}

void timer_start_granular (
    sys_timestamp *Tm,
    long Dt,
    ulong Gr)
{
    if (Dt < 0) {
	(*kernel_errmsg)(&msgsym(TIMERNEG, SYS), Tm, Dt);
	Dt = 1;
    }
    get_timestamp_inline(Tm);
    TSP_VALUE(Tm) += Dt + (Gr - 1);
    TSP_VALUE(Tm) -= (TSP_VALUE(Tm) % Gr);
}

void timer_start_granular64 (
    sys_timestamp *Tm,
    ulonglong Dt,
    ulonglong Gr)
{
    get_timestamp_inline(Tm);
    TSP_VALUE(Tm) += Dt + (Gr - 1);
    TSP_VALUE(Tm) -= (TSP_VALUE(Tm) % Gr);
}

void timer_add_granular (
    sys_timestamp *Tm,
    long Dt,
    ulong Gr)
{
    if (Dt < 0) {
	(*kernel_errmsg)(&msgsym(TIMERNEG, SYS), Tm, Dt);
	Dt = 1;
    }
    if (!timer_running_inline(Tm))
	return;

    TSP_VALUE(Tm) = TSP_VALUE(Tm) + Dt + (Gr - 1);
    TSP_VALUE(Tm) -= (TSP_VALUE(Tm) % Gr);
}

void timer_add_granular64 (
    sys_timestamp *Tm,
    ulonglong Dt,
    ulonglong Gr)
{
    if (!timer_running_inline(Tm))
	return;

    TSP_VALUE(Tm) = TSP_VALUE(Tm) + Dt + (Gr - 1);
    TSP_VALUE(Tm) -= (TSP_VALUE(Tm) % Gr);
}
