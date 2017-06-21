/* $Id: timer_i8254.h,v 3.2 1996/01/06 05:17:03 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/timer_i8254.h,v $
 *------------------------------------------------------------------
 * timer_i8254.h : Timer support routines for M68K platforms
 * from if_68_timer.c
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_i8254.h,v $
 * Revision 3.2  1996/01/06  05:17:03  hampton
 * Split timer specific routines into multiple files.  Move the processor
 * specific routines into the src-xx-xxx directories.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/06  00:12:11  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Local defines
 */
#define MSEC_SETTING    1000		/* timer setting for 1ms */



static inline void i8254_start_counter (ushort value)
{
    ctaddr->control = SEL_COUNTER1 | RW_LSBMSB | CT_MODE4; 
    ctaddr->counter1 = value;		/* program lsb */ 
    ctaddr->counter1 = value >> 8;	/* program msb */ 
    WASTETIME(5);			/* give timer a moment to load */
}

static inline ushort i8254_read_counter (void)
{
    ushort tmp;

    ctaddr->control = SEL_COUNTER1 | CT_MODE4;  /* latch counter 1 */
    WASTETIME(5);			/* give timer a moment to latch count */
    tmp = (ctaddr->counter1 & 0xff);	/* get lsb */
    WASTETIME(5);
    tmp |= (ctaddr->counter1 << 8);	/* get msb */
    return(tmp);
}

static inline void i8254_stop_counter (void)
{
    ctaddr->control = SEL_COUNTER1;	/* stop counting */ 
}

static inline void i8254_set_refresh_frequency (void)
{
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
    WASTETIME(5);
    ctaddr->counter2 = REFRFREQUENCY & 0xFF;
    WASTETIME(5);
    ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;
}

static inline void i8254_restart_watchdog (int milliseconds)
{
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0; 
    WASTETIME(5);
    ctaddr->counter0 = (milliseconds * MSEC_SETTING) & 0xFF; 
    WASTETIME(5);
    ctaddr->counter0 = ((milliseconds * MSEC_SETTING) >> 8) & 0xFF;
}

static inline ushort i8254_read_timer_residual (void)
{
    leveltype oldsr;
    ushort tmp;

    oldsr = disable_interrupts();
    ctaddr->control = SEL_COUNTER2 | CT_MODE2;
    WASTETIME(2);
    tmp = ctaddr->counter2;
    tmp |= ctaddr->counter2 << 8;
    enable_interrupts(oldsr);
    return(tmp);
}
