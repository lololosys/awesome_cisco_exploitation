/* $Id: ntp_clock_interface.c,v 3.3.12.1 1996/04/16 19:03:06 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_clock_interface.c,v $
 *------------------------------------------------------------------
 * ntp_clock_interface.c - interface between system clock and NTP routines
 *
 * October 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This code interfaces the system clock routines to the NTP routines, to
 * avoid having to change the NTP code.
 *------------------------------------------------------------------
 * $Log: ntp_clock_interface.c,v $
 * Revision 3.3.12.1  1996/04/16  19:03:06  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  09:08:29  smackie
 * You can't take the address of a "register" variable. Not that the
 * "register" keyword does anything anyway with the current setting
 * of gcc optimizer we use. (CSCdi49069)
 *
 * Revision 3.2  1995/11/17  17:50:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:28:39  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:44:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "packet.h"
#include "ntpd.h"
#include "ntp_stdlib.h"
#include "../ip/ip.h"
#include "../os/clock_private.h"

u_int tsf_maxslew;		/* Max slew per 1<<CLOCK_ADJ interval */
static l_fp local_offset;	/* correction for current system time */
long sys_clock;			/* High order part of current time */

/*
 * init_systime - Calculate clock slew information.
 */
void init_systime (void)
{
    ulong tickadj;
    ulong hz;
    ulong precision;
    ulong dummy;
    
    /*
     * Obtain the values
     */
    get_clock_params(&hz, &tickadj, &precision, &dummy, &dummy, &dummy);
    sys_precision = -precision;
    
    /*
     * Set maximum slew based on the clock parameters.  Note
     * that tsf_maxslew is set slightly shorter than it needs to be as
     * insurance that all slews requested will complete in 1<<CLOCK_ADJ
     * seconds.
     *
     * We assume that the slew rate is small enough so that the below
     * calculation doesn't overflow (it is, by about 3 orders of
     * magnitude).
     */
    
    tsf_maxslew = tickadj * (hz-1) * (1<<CLOCK_ADJ);

    if (ntp_clockparms_debug) {
	buginf("\nNTP: Maxslew = %d", tsf_maxslew);
    }
    
    /*
     * Set the current offset to 0.
     */
    local_offset.l_ui = local_offset.l_uf = 0;
}


/*
 * get_systime - return the system time in timestamp format
 * As a side effect, update sys_clock.  This is done to hardware resolution.
 */
void get_systime (
l_fp *ts)
{
    /*
     * Get the current time and add in the current correction offset.
     */
    clock_get_time_exact(ts);
    L_ADD(ts, &local_offset);
    sys_clock = ts->l_ui;
}


/*
 * step_systime - do a step adjustment in the system time (at least from
 *		  NTP's point of view.
 */
int step_systime (
    l_fp *ts)
{
    l_fp curtime;
    
    clock_get_time(&curtime);
    if (ntp_clockadjust_debug) {
	buginf("\nNTP: step(%s): local_offset = %s, curtime = %s",
	       lfptoa(ts, 9), lfptoa(&local_offset, 9), lfptoa(&curtime, 9));
    }

    L_ADD(&local_offset, ts);
    L_ADD(&curtime, &local_offset);
    clock_set(&curtime, CLOCK_SOURCE_NTP);

    /* Wipe out the offset (we've just forced the clock to be up-to-date). */

    local_offset.l_ui = local_offset.l_uf = 0;
    return(0);
}


/*
 * adj_systime - called once every 1<<CLOCK_ADJ seconds to make system time
 *		 adjustments.
 */
int adj_systime (l_fp *adj)
{
    unsigned long offset_i, offset_f;
    register int isneg = 0;
    int this_slew;
    
    /*
     * Move the current offset into the registers.
     */
    offset_i = local_offset.l_ui;
    offset_f = local_offset.l_uf;
    
    /*
     * Add the new adjustment into the system offset.
     */
    M_ADDF(offset_i, offset_f, adj->l_f);
    if (M_ISNEG(offset_i, offset_f)) {
	isneg = 1;
	M_NEG(offset_i, offset_f);
    }

    if (offset_i > 0 || offset_f >= tsf_maxslew) {
	
	/*
	 * Slew is bigger than we can complete in
	 * the adjustment interval.  Make a maximum
	 * sized slew and reduce local_offset by this
	 * much.
	 */
	M_SUBUF(offset_i, offset_f, tsf_maxslew);
	if (isneg) {
	    this_slew = -tsf_maxslew;
	    M_NEG(offset_i, offset_f); /* Put the sign back */
	} else {
	    this_slew = tsf_maxslew;
	}
	
    } else {
	
	/* Slew is small enough.  Do it all at once and zap the offset. */
	
	if (isneg) {
	    this_slew = -offset_f;
	} else {
	    this_slew = offset_f;
	}
	offset_i = offset_f = 0;
    }
    
    if (clock_slew(this_slew, CLOCK_SOURCE_NTP)) {
	M_ADDF(offset_i, offset_f, this_slew); /* Put it back */
    }
    
    local_offset.l_ui = offset_i;
    local_offset.l_uf = offset_f;
    
    if (this_slew) {
	if (ntp_clockadjust_debug) {
	    buginf("\nNTP: adj(%s), rem. offset = %s, adj = %s",
		   mfptoa((adj->l_f<0?-1:0), adj->l_f, 9),
		   lfptoa(&local_offset, 9), 
		   mfptoa((this_slew<0?-1:0), this_slew, 9));
	}
    }
    return(0);
}
