/* $Id: calendar.c,v 3.4.4.2 1996/03/21 22:39:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/calendar.c,v $
 *------------------------------------------------------------------
 * calendar.c --  Routines and drivers for Real-Time Clock
 *
 * 1-Sept-1992, Gary Kramling
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: calendar.c,v $
 * Revision 3.4.4.2  1996/03/21  22:39:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.4.1  1996/03/18  19:39:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:17:57  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/01/15  10:18:36  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/11/22  20:30:33  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.4  1996/03/11  11:46:06  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Deterge the code.
 *
 * Revision 3.3  1995/12/12  09:05:53  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Whenever we write to the battery-backed calendar hardware on
 * the master, ship the value across to the slave's calendar too.
 *
 * Revision 3.2  1995/11/17  09:14:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:27:25  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:37:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "calendar_p.h"
#include "calendar.h"
#include "../os/clock.h"
#include "../os/clock_private.h"

#if defined(SIERRA)
#include "../src-4k-c4000/pcmap.h"
#endif

#if defined(C7000)
#include "../../boot/src-68-c7000/pcmap.h"
#endif

#if defined(RSP)
#include "../src-rsp/rsp_pcmap.h"
#endif

#if defined(PREDATOR)
#include "../src-4k-c7100/c7100_pcmap.h"
#endif

volatile char* calendar = (char *) ADRSPC_CALENDAR;

/*
 * wake_calendar
 * Sends testpattern to nvram address to get attention of calendar chip.
 * testpattern is a sequence of 64 unique bits that enables calendar for
 * read/write. 
 */
static void wake_calendar (void)
{
    /* This pattern of 64 sequential bits will wake the calendar */
    uchar wake_pattern[4] = {0xC5, 0x3A, 0xA3, 0x5C};
    volatile char test, test2;
    uint offset, n, k;

    test2 = *calendar; /* This initial read is required to move */
                       /* the calendar ptr back to beginning    */

    for ( k=0; k<2; k++) {
        for ( n=0; n<4; n++) {
            for ( offset=0; offset<8; offset++) {
                *calendar = wake_pattern[n] >> offset;
                test = wake_pattern[n] >> offset;
            }
        }
    }
}
/*
 * read_calendar
 * Read the date and time from the calendar chip.  Fill the calendar struct.
 */
static void read_calendar (calendar_t *cal)
{
    ulong value0, value1;
    uint offset;
    leveltype status;

    value0 = 0;
    value1 = 0;

    /*
     * Need to disable here so other processes don't scribble on nvram
     * and mess up the following sequence.
     */
    status = raise_interrupt_level(ALL_DISABLE);
    wake_calendar();

    for ( offset=0; offset<32; offset++) {
       if ( *calendar & 01 )
           value0 |= ( 1 << offset);
    }

    for ( offset=0; offset<32; offset++) {
        if ( *calendar & 01 )
            value1 |= ( 1 << offset);
    }
    reset_interrupt_level(status);

    cal->hours = (value0 >> 24) & HOURS_MASK;
    cal->minutes = (value0 >> 16) & MINUTES_MASK;
    cal->seconds = (value0 >> 8)  & SECONDS_MASK;
    cal->year  = (value1 >> 24) & YEAR_MASK;
    cal->month = (value1 >> 16) & MONTH_MASK;
    cal->date  = (value1 >> 8)  & DATE_MASK;
    cal->day   = value1 & DAY_MASK;
}

/*
 * write_calendar
 * Write the date and time to the calendar chip.
 */
void write_calendar (calendar_t *cal)
{
    uint offset, i, j;
    uchar *wr_ptr;
    leveltype status;

    wr_ptr = (uchar *) cal;

    cal->hund_seconds = 0;
    cal->mode_12_24 = 0;
    cal->reset = 1;
    cal->oscillator = 0;

    /*
     * Need to disable here so other processes don't scribble on nvram
     * and mess up the following sequence.
     */
    status = raise_interrupt_level(ALL_DISABLE);
    wake_calendar();

    for ( j = 0 ; j < 8; j++ ) {
        for (offset = 0 ; offset < 8; offset++, i++)
            *calendar = ( char ) ( (*wr_ptr >> offset) & 0xFF);
        wr_ptr++;
    }
    reset_interrupt_level(status);

    /*
     * Ship it across to the slave on HSA systems.  Otherwise, should we
     * fail over to the slave, when it boots, it won't have the correct
     * time stored in it's calendar chip.
     */
    reg_invoke_write_slave_calendar(cal);
}

/*
 * convert bcd to decimal
 */
static ulong bcd_to_decimal (ulong bcd)
{
    int i;
    ulong result = 0;
    for (i=0; i<8; i++) {
	result *= 10;
	result += (bcd >> 28) & 0xf;
	bcd = bcd << 4;
    }
    return(result);
}

/*
 * Convert decimal to BCD
 */
static ulong decimal_to_bcd (ulong decimal)
{
    int i;
    ulong result = 0;
    for (i=0; i<8; i++) {
	result |= (decimal % 10) << (4 * i);
	decimal /= 10;
    }
    return(result);
}

/*
 * read_calendar_for_ntp
 *
 * Read the calendar and return it in NTP's timeval structure.
 *
 * Leaves tz_offset, day_of_week, and millisecond zero.
 */
static void read_calendar_for_ntp (timeval *tv)
{
    calendar_t cal;
    read_calendar(&cal);
    tv->year = bcd_to_decimal(cal.year);
    if (tv->year < 89)
	tv->year += 2000;
    else
	tv->year += 1900;
    tv->month = bcd_to_decimal(cal.month);
    tv->day = bcd_to_decimal(cal.date);
    tv->hour = bcd_to_decimal(cal.hours);
    tv->minute = bcd_to_decimal(cal.minutes);
    tv->second = bcd_to_decimal(cal.seconds);
    tv->millisecond = 0;
    tv->day_of_week = 0;
    tv->tz_offset = 0;
}

/*
 * Write the calendar from NTP's timeval structure.
 */
static void write_calendar_from_ntp (timeval *tv)
{
    calendar_t cal;
    memset(&cal, 0, sizeof(cal));
    cal.year = decimal_to_bcd(tv->year % 100);
    cal.month = decimal_to_bcd(tv->month);
    cal.date = decimal_to_bcd(tv->day);
    cal.hours = decimal_to_bcd(tv->hour);
    cal.minutes = decimal_to_bcd(tv->minute);
    cal.seconds = decimal_to_bcd(tv->second);
    cal.day = tv->day_of_week + 1;
    write_calendar(&cal);
}

/*
 * cpu_supports_calendar
 *
 * Returns TRUE if this CPU supports a calendar.  We assume for now that
 * by virtue of being linked in, there must be a calendar.  The stub
 * returns FALSE.
 */
static boolean cpu_supports_calendar (void)
{
    return(TRUE);
}

static void calendar_init (subsystype* subsys)
{
    reg_add_cpu_supports_calendar(cpu_supports_calendar,
				  "cpu_supports_calendar");
    reg_add_ntp_read_calendar(read_calendar_for_ntp, "read_calendar_for_ntp");
    reg_add_ntp_write_calendar(write_calendar_from_ntp,
			       "write_calendar_from_ntp");
    do_read_calendar();
}

/*
 * CALENDAR subsystem header
 */

#define CALENDAR_MAJVERSION 1
#define CALENDAR_MINVERSION 0
#define CALENDAR_EDITVERSION  1

SUBSYS_HEADER(calendar, CALENDAR_MAJVERSION, CALENDAR_MINVERSION,
	      CALENDAR_EDITVERSION,
	      calendar_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
