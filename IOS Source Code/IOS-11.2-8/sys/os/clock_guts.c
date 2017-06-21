/* $Id: clock_guts.c,v 3.4.54.5 1996/08/28 13:03:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/clock_guts.c,v $
 *------------------------------------------------------------------
 * Core clock support
 *
 * May 1992, Dave Katz.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Basic routines to support the "Unix clock model" for system timekeeping
 *------------------------------------------------------------------
 * $Log: clock_guts.c,v $
 * Revision 3.4.54.5  1996/08/28  13:03:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.54.4  1996/08/19  19:01:20  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.4.54.3  1996/07/10  22:21:47  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.54.2  1996/04/16  19:09:53  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.54.1  1996/03/18  21:29:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/09  05:09:07  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.20.1  1996/02/20  16:37:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/01  05:22:49  dkatz
 * CSCdi43033:  Summer time epochs may not be updated
 *
 * Revision 3.3  1995/11/17  18:45:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:29:13  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:54:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "clock.h"
#include "clock_private.h"
#include "clock_internal.h"
#include "platform_clock.h"
#include "string.h"
#include "sys_registry.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ntp/ntp_registry.h"

/*
 * Macro to add a fractional value to a clock_epoch, and increment an
 * auxiliary word if the fractional part rolls over.  Used to update
 * system_clock and secs_since_boot simultaneously.
 */
#ifndef mc68000			/* Sparc version */
#define ADD_UF_AUX(r_i, r_f, a_f, aux) /* ({r_i,aux},r_f) += a_f */ \
{\
    u_long local_frac = (r_f);\
    (r_f) += (a_f);\
    if ((r_f) < local_frac) {\
	(r_i)++;\
	(aux)++;\
    }\
}
#else
#define ADD_UF_AUX(r_i, r_f, a_f, aux) /* ({r_i,aux},r_f) += a_f */ \
    asm volatile ( "\
                    addl  %5,%1\n\
                    bcc   0f\n\
                    addql #1,%0\n\
                    addql #1,%2\n\
                 0:"\
: "=dm" ((r_i)), "=dm" ((r_f)), "=dm" ((aux)) \
: "0" ((r_i)), "1" ((r_f)), "d" ((a_f)))
#endif				/* UNIX */

/*
 * Low-level System Clock support
 *
 * The system clock fits the "Unix Clock Model."
 *
 * The current time is kept at a resolution greater than the resolution
 * of the hardware clock, and incremented by some value whenever the hardware
 * clock ticks.  To provide time adjustments, two methods are available--for
 * large adjustments, the time is simply set to the desired time.  For small
 * adjustments, the clock is slewed in such a way that the time is always
 * increasing monotonically--the amount added to the clock for each hardware
 * tick is reduced or increased until the desired adjustment has been
 * completed.
 *
 * The time is stored as a 64 bit binary fraction, with the "decimal" point
 * in the middle (or, if you like, think of it as a 64-bit integer in units
 * of 2^-32), specifying the number of seconds since 0000 UTC, January 1,
 * 1900, for the convenience of the NTP protocol.
 *
 * At each tick of the hardware clock, we add 2^32/Hz to the fractional
 * part of the time (and check for rollover).  Note that, due to truncation
 * error, the clock will run slow by a minute amount (68 parts per billion
 * at 1000 Hz, 11 parts per billion at 250 Hz).  This amount is too small
 * to really care about, and in fact is negligible compared to the error
 * of the crystal (on the order of 5000 parts per billion or more).  The
 * NTP protocol will compensate for this anyhow.
 *
 * When getting the current time, we read the current value of the timer
 * clock so that we can interpolate between ticks.  The true value for
 * each timer tick would be the basic tick increment divided by the
 * timer frequency, so to calculate the offset accurately we would calculate
 *   (increment / timer_freq) * timer_value
 * This is inconvenient, however, since we don't really want to do division
 * at all.  Another approach would be to precalculate (increment / timer_freq)
 * and then multiply by timer_value, but this leads to truncation errors.
 * Instead, we calculate:
 *    ((increment / timer_freq) * K) * timer_value / K
 * where K is a power of two;  then we can precalculate 
 * ((increment / timer_freq) * K) and then do a multiply and a shift in real
 * time and get very high accuracy for cheap.  The value of K is chosen to be
 * less than Hz (but as large as possible otherwise) so we get the best
 * accuracy but don't get any arithmetic overflows.
 *
 * This code relies on a platform-dependent routine that returns information
 * about the hardware timer, including a pointer to a routine to read the
 * timer residual.  We assume that the residual counts downward from
 * (precision - 1) to 0.
 */


/*
 * clock variables
 */

#define SLEW_RATE 2000		/* Slew rate (500 ppm) */

volatile clock_epoch system_clock; /* Current time */
ulong secs_since_boot;		/* Seconds since boot time */
char ntp_clock_status;		/* Character to display in "show clock" */

static ulong hz;		/* Estimated clock tick frequency */
static ulong increment;		/* Current SW increment per HW tick */
static ulong basic_increment;	/* Initial SW increment per HW tick */

static int adjust;		/* Outstanding clock adjustment */
static int max_delta;		/* Maximum slew per clock tick */
static int delta;		/* Current slew per clock tick */
static ulong log2hz;		/* log2 of hz, truncated */
static ulong divisor;		/* timer clocks per tick */
static ulong precision;		/* Clock precision (2**x Hz) */
static ulong prescalar_factor;	/* increment per timer tick * 1<<log2hz */
static ulong freq_numerator;	/* Numerator of clock frequency */
static ulong freq_denominator;	/* Denominator of clock frequency */
static ulong (*get_residual)(void); /* Routine to get timer residual */
static ulong leap_second_epoch;		/* Time of leap second jump */
static int leap_second_offset;		/* Value to adjust at leap second */
long ntp_regcode;			/* It needs to be global... */
long ntp_private_regcode;		/* As does this... */

/*
 * Get the current clock parameters.
 */

void get_clock_params (
    ulong *clock_freq,
    ulong *max_slew,
    ulong *prec,
    ulong *incr,
    ulong *basic_incr,
    ulong *div)
{
    *clock_freq = hz;
    *max_slew = max_delta;
    *prec = precision;
    *incr = increment;
    *basic_incr = basic_increment;
    *div = divisor;
}


/*
 * Set the software clock increment per hardware clock tick.  This is
 * broken out so that we can adjust this increment separately if need
 * be (such as to compensate more cleanly for a way-off crystal).
 */
void set_clock_increment (ulong incr)
{
    ulong freq;
    delta = 0;
    adjust = 0;
    increment = incr;
    /*
     * Recalculate the basic clock frequency.  Make sure we round
     * rather than truncate.
     */
    freq = hz = (NTP_RESOLUTION + (incr >> (NTP_SHIFT_FACTOR + 1))) / 
	(incr >> NTP_SHIFT_FACTOR);
    max_delta = incr / SLEW_RATE;
    
    /* Calculate the log2 of the frequency. */

    freq &= 0x7fffffff;		/* Ensure that the loop completes */
    for (log2hz = 0; freq; log2hz++)
	freq = freq >> 1;
    log2hz--;
    prescalar_factor = incr * (1 << log2hz) / divisor;
}

/*
 * Call back to a clock freqency callback routine.
 *
 * Called when a module in another subsystem wants its clock frequency
 * set callback routine explicitly invoked.  This is typically done
 * when the process initializes since it probably missed the original setting
 * of the hardware clock frequency.
 */
void invoke_clock_set (void (*callback) (ulong, ulong))
{
    (*callback)(freq_numerator, freq_denominator);
}


/*
 * Set the hardware clock frequency.  We recalculate all of our interesting
 * fudge factors.
 *
 * This frequency is simply a first-order approximation of the true clock
 * frequency (which we will calculate as we go).
 *
 * The frequency is passed as a numerator and denominator so that we can
 * get higher accuracy (the 500-cs clock runs at 8192/33 Hz, which doesn't
 * end up as a very good integer).  We calculate the increment upside down
 * to avoid truncation errors.
 */

static void set_hw_clock_freq (ulong numerator, ulong denominator, ulong div)
{
    ulong incr;
    freq_numerator = numerator;
    freq_denominator = denominator;
    divisor = div;
    incr = ((NTP_RESOLUTION / numerator) * denominator) << NTP_SHIFT_FACTOR;
    basic_increment = incr;
    set_clock_increment(incr);
    reg_invoke_hw_clock_set(numerator, denominator);
    reg_invoke_time_set(FALSE);
}


/*
 * Slew the clock.  We return a nonzero value if the clock was already
 * being adjusted (and ignore the requested adjustment).
 *
 * The requested clock slew is in units of 2^-32 seconds, and the magnitude
 * must be less than 1/2 second (we're using 32 bit words, after all...).
 */

int clock_slew (int slew_value, clock_source source)
{
    int return_value = 0;
    clock_set_time_source(source);

    if (delta !=0) {
	return_value = 1;
    } else {
	adjust = slew_value;
	if (slew_value >= 0) {
	    delta = min(max_delta, slew_value);
	} else {
	    delta = max(-max_delta, slew_value);
	}
    }

    return(return_value);
}
/*
 * This routine gets the time as of the last NMI tick.  For exact time,
 * see above.
 */
void clock_get_time (clock_epoch *timeptr)
{
    /*
     * Get the clock value.  Get it again if it looks like it just rolled
     * over, to ensure that the seconds and fractional seconds are
     * consistent.
     */
    timeptr->epoch_frac = system_clock.epoch_frac;
    timeptr->epoch_secs = system_clock.epoch_secs;
    if (timeptr->epoch_frac > system_clock.epoch_frac) {
	timeptr->epoch_frac = system_clock.epoch_frac;
	timeptr->epoch_secs = system_clock.epoch_secs;
    }

    /* Now update the summer time info as necessary. */

    check_for_summer_time(timeptr);
}

/*
 * Get the exact time.  Read up the platform-dependent residual and
 * interpolate.
 *
 * Note that this routine is called from both interrupt and task threads,
 * so it must remain reentrant.
 *
 * Additionally, when reading the residual, it may roll over, causing the
 * main time to tick.  We avoid this by checking the time before and
 * after reading the timer;  if it has advanced, we do it a second time,
 * which should be OK.
 */
void clock_get_time_exact (clock_epoch *timeptr)
{
    int offset = 0;
    ulong last_frac;

    /* Get the system clock. */

    clock_get_time(timeptr);
    last_frac = timeptr->epoch_frac;

    /* Read the residual. */

    if (get_residual)
	offset = (*get_residual)();

    /* If the timer rolled over, do it again. */

    if (system_clock.epoch_frac != last_frac) {
	clock_get_time(timeptr);
	last_frac = timeptr->epoch_frac;
	if (get_residual)
	    offset = (*get_residual)();
    }

     /* Now scale the offset. */

    timeptr->epoch_frac += (offset * prescalar_factor) >> log2hz;

    /* Finally, adjust for rollover. */

    if (timeptr->epoch_frac < last_frac) {
	timeptr->epoch_secs++;
    }
}

/*
 * clock_get_microsecs
 *
 * Get the current time in terms of microseconds.  This is useless as a time
 * of day, but makes a dandy, fast-moving 32 bit value.
 */

ulong clock_get_microsecs (void)
{
    clock_epoch time;

#define MICROS_PER_SEC	1000000

    clock_get_time_exact(&time);
    return((time.epoch_secs % (MAXUINT / MICROS_PER_SEC)) * MICROS_PER_SEC +
	   time.epoch_frac / (MAXUINT / MICROS_PER_SEC));
}


/*
 * clock_set
 *
 * Set the current time.  This is used for gross time steps only.  Reasonable
 * time adjustments should be made by calling clock_slew instead.
 */
void clock_set (clock_epoch *timeptr, clock_source source)
{
    delta = 0;
    adjust = 0;
    system_clock = *timeptr;
    clock_set_time_source(source);
    calculate_summer_time_epochs(NULL);
    check_for_summer_time(NULL);
    reg_invoke_time_set(source == CLOCK_SOURCE_NTP);
}

/*
 * Process a hardware clock tick.  This routine is called at interrupt
 * time!
 */

void clock_tick (void)
{
    ulong local_incr;
    int local_delta;
    int local_adjust;
 
    local_incr = increment;		/* Get 'em in registers */
    local_delta = delta;

    /*
     * If we are slewing the clock, include the current slew delta when
     * bumping the software clock, and keep track of how much we've slewed
     * thusfar.
     */

    if (local_delta != 0) {
	local_incr += local_delta;
	ADD_UF_AUX(system_clock.epoch_secs, system_clock.epoch_frac, local_incr, 
		   secs_since_boot);
	adjust -= local_delta;
	local_adjust = adjust;
	if (local_delta >= 0) {
	    if (local_adjust < local_delta)
		delta = local_adjust;
	} else {
	    if (local_adjust > local_delta)
		delta = local_adjust;
	}

    } else {

	/* Not slewing.  Do it cheaply. */

	ADD_UF_AUX(system_clock.epoch_secs, system_clock.epoch_frac, local_incr,
		   secs_since_boot);
    }

    /* Look for a leap second. */

    if (leap_second_epoch) {		/* Something's coming */
	if (leap_second_epoch == system_clock.epoch_secs) { /* It's now! */
	    system_clock.epoch_secs += leap_second_offset; /* Adjust it */
	    leap_second_epoch = 0;	/* All done. */
	}
    }
}

/*
 * clock_set_leap_second
 *
 * Set the leap second parameters.
 */
void clock_set_leap_second (ulong epoch, int offset)
{
    leap_second_epoch = epoch;
    leap_second_offset = offset;
}

/*
 * clock_get_leap_second
 *
 * Returns the leap second parameters
 */
ulong clock_get_leap_second (int *offset)
{
    *offset = leap_second_offset;
    return(leap_second_epoch);
}

/*
 * Initialize the kernel clock.  We start at March 1, 1993, and throw
 * in the current msclock to help randomize things that use the
 * system clock as a random seed.
 */
void init_kernel_clock (void)
{
    ulong freq_numerator, freq_denominator, freq_divisor;
    sys_timestamp cur_msclock;
    system_clock.epoch_secs = MAR1993;	/* A reasonable place to start. */
    GET_TIMESTAMP(cur_msclock);		/* Throw in msclock */
    system_clock.epoch_secs += TS_VALUE(cur_msclock) / ONESEC;
    system_clock.epoch_frac = (TS_VALUE(cur_msclock) % ONESEC) *
	(MAXUINT / ONESEC);
    get_timer_parameters(&freq_numerator, &freq_denominator, &freq_divisor,
			 &get_residual, &precision);
    set_hw_clock_freq(freq_numerator, freq_denominator, freq_divisor);
}
