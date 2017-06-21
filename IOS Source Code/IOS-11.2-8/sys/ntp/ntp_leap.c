/* $Id: ntp_leap.c,v 3.2.62.3 1996/08/19 18:54:58 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_leap.c,v $
 *------------------------------------------------------------------
 * NTP leap second handling routines
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_leap.c,v $
 * Revision 3.2.62.3  1996/08/19  18:54:58  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.2  1996/05/07  18:05:32  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * occured -> occurred
 *
 * Revision 3.2.62.1  1996/04/16  19:03:15  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:45:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) David L. Mills 1992, 1993, 1994
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name University of Delaware not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The University of Delaware
 * makes no representations about the suitability this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC-1305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.kfu.com> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de>
 *      (PARSE (GENERIC) driver, STREAMS module for PARSE, support scripts,
 *       reference clock configuration scripts, Makefile cleanup)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (Spectractom WWVB, Austron GPS,
 *	Heath, ATOM, ACTS, KSI/Odetics IRIG-B clock drivers; pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 * Mike Iglesias (iglesias@uci.edu) (DEC Alpha changes)
 * Mark Andrews <marka@syd.dms.csiro.au> (Leitch atomic clock controller)
 * George Lindholm <lindholm@ucs.ubc.ca> (port to SunOS 5.1 operating system)
 * Jeff Johnson <jbj@chatham.usdesign.com> (massive prototyping overhaul)
 * Tom Moore <tmoore@fievel.daytonoh.ncr.com> (port to i386 svr4)
 * Piete Brooks <Piete.Brooks@cl.cam.ac.uk> (MSF clock driver, Trimble PARSE
 * 	support)
 * Karl Berry <karl@owl.HQ.ileaf.com> (syslog to file option)
 * Torsten Duwe <duwe@immd4.informatik.uni-erlangen.de> (Linux Port)
 * Paul A Vixie <vixie@vix.com> (TrueTime GPS driver)
 * Jim Jagielski <jim@jagubox.gsfc.nasa.gov> (A/UX port)
 * Ray Schnitzler <schnitz@unipress.com> (First pass at a Unixware1 port.)
 * Ajit Thyagarajan <ajit@ee.udel.edu> (IP multicast support)
 * Jeff Steinman <jss@pebbles.jpl.nasa.gov> (Datum PTS clock driver)
 * Tomoaki TSURUOKA <tsuruoka@nc.fukuoka-u.ac.jp> (TRAK clock driver)
 */

/*
 * ntp_leap - maintain leap bits and take action when a leap occurs
 */
#define CISCO

#ifdef CISCO
#include "master.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../os/clock.h"
#include "../os/clock_private.h"
#else
#include <stdio.h>
#endif /* CISCO */

#include "ntpd.h"
#include "ntp_stdlib.h"

/*
 * This module is devoted to maintaining the leap bits and taking
 * action when a leap second occurs.  It probably has bugs since
 * a leap second has never occurred to excercise the code.
 *
 * The code does two things when a leap second occurs.  It first
 * steps the clock one second in the appropriate direction.  It
 * then informs the reference clock code, if compiled in, that the
 * leap second has occurred so that any clocks which need to disable
 * themselves can do so.  This is done within the first few seconds
 * after midnight, UTC.
 *
 * The code maintains two variables which may be written externally,
 * leap_warning and leap_indicator.  Leap_warning can be written
 * any time in the month preceeding a leap second.  24 hours before
 * the leap is to occur, leap_warning's contents are copied to
 * leap_indicator.  The latter is used by reference clocks to set
 * their leap bits.
 *
 * The module normally maintains a timer which is arranged to expire
 * just after 0000Z one day before the leap.  On the day a leap might
 * occur the interrupt is aimed at 2200Z and every 5 minutes thereafter
 * until 1200Z to see if the leap bits appear.
 */

/*
 * The leap indicator and leap warning flags.  Set by control messages
 */
u_char leap_indicator;
u_char leap_warning;
u_char leap_mask;		/* set on day before a potential leap */
/*
 * Timer.  The timer code imports this so it can call us prior to
 * calling out any pending transmits.
 */
u_long leap_timer;

/*
 * We don't want to do anything drastic if the leap function is handled
 * by the kernel.
 */
extern int pll_control;		/* set nonzero if kernel pll in uss */

/*
 * Internal leap bits.  If we see leap bits set during the last
 * hour we set these.
 */
u_char leapbits;

/*
 * Constants.
 */
#define	OKAYTOSETWARNING	(31*24*60*60)
#define	DAYBEFORE		(24*60*60)
#define	TWOHOURSBEFORE		(2*60*60)
#define	FIVEMINUTES		(5*60)
#define	ONEMINUTE		(60)


/*
 * Some statistics counters
 */
u_long leap_processcalls;	/* calls to leap_process */
u_long leap_notclose;		/* leap found to be a long time from now */
u_long leap_monthofleap;	/* in the month of a leap */
u_long leap_dayofleap;		/* This is the day of the leap */
u_long leap_hoursfromleap;	/* only 2 hours from leap */
u_long leap_happened;		/* leap process saw the leap */

/*
 * Imported from the main module
 */
extern int debug;


static void	setnexttimeout	P((u_long));

/*
 * init_leap - initialize the leap module's data.
 */
void
init_leap()
{
	/*
	 * Zero the indicators.  Schedule an event for just after
	 * initialization so we can sort things out.
	 */
	leap_indicator = leap_warning = leap_mask = 0;
	leap_timer = 1<<EVENT_TIMEOUT;
	leapbits = 0;

	leap_processcalls = leap_notclose = 0;
	leap_monthofleap = leap_dayofleap = 0;
	leap_hoursfromleap = leap_happened = 0;
}


/*
 * leap_process - process a leap event expiry and/or a system time step
 */
void
leap_process()
{
	u_long leapnext;
	u_long leaplast;
	l_fp ts;
	u_char bits;
	extern u_char sys_leap;
#ifdef CISCO
	u_long leap_epoch;
	int leap_offset;
#endif /* CISCO */

	leap_processcalls++;
	get_systime(&ts);
	calleapwhen((u_long)ts.l_ui, &leaplast, &leapnext);
#ifdef CISCO
	leap_epoch = ts.l_ui + leapnext;
#endif /* CISCO */

	/*
	 * Figure out what to do based on how long to the next leap.
	 */
	if (leapnext > OKAYTOSETWARNING) {
		if (leaplast < ONEMINUTE) {
			/*
			 * The golden moment!  See if there's anything
			 * to do.
			 */
			leap_happened++;
			bits = 0;
			leap_mask = 0;
#ifndef CISCO				/* We use the kernel for leaps */
			if (leap_indicator != 0)
				bits = leap_indicator;
			else if (leapbits != 0)
				bits = leapbits;
			
			if (bits != 0 && !pll_control) {
				l_fp tmp;

				/*
				 * Step the clock 1 second in the proper
				 * direction.
				 */
				if (bits == LEAP_DELSECOND)
					tmp.l_i = 1;
				else
					tmp.l_i = -1;
				tmp.l_uf = 0;

				step_systime(&tmp);
				NLOG(NLOG_SYNCEVENT|NLOG_SYSEVENT) /* conditional if clause for conditional syslog */
				  syslog(LOG_NOTICE,
#ifdef SLEWALWAYS
			"leap second occurred, slewed time %s 1 second",
#else
			"leap second occurred, stepped time %s 1 second",
#endif
				    tmp.l_i > 0 ? "forward" : "back");
			}
#endif /* CISCO */
		} else {
			leap_notclose++;
		}
		leap_warning = 0;
	} else {
		if (leapnext > DAYBEFORE)
			leap_monthofleap++;
		else if (leapnext > TWOHOURSBEFORE)
			leap_dayofleap++;
		else
			leap_hoursfromleap++;
	}

	if (leapnext > DAYBEFORE) {
		leap_indicator = 0;
		leapbits = 0;
		/*
		 * Berkeley's setitimer call does result in alarm
		 * signal drift despite rumours to the contrary.
		 * Schedule an event no more than 24 hours into
		 * the future to allow the event time to be
		 * recomputed.
		 */
		if ((leapnext - DAYBEFORE) >= DAYBEFORE)
			setnexttimeout((u_long)DAYBEFORE);
		else
			setnexttimeout(leapnext - DAYBEFORE);
		return;
	}

	/*
	 * Here we're in the day of the leap.  Set the leap indicator
	 * bits from the warning, if necessary.
	 */
	if (leap_indicator == 0 && leap_warning != 0)
		leap_indicator = leap_warning;
	leap_mask = LEAP_NOTINSYNC;
	if (leapnext > TWOHOURSBEFORE) {
		leapbits = 0;
		setnexttimeout(leapnext - TWOHOURSBEFORE);
		return;
	}

	/*
	 * Here we're in the final 2 hours.  If sys_leap is set, set
	 * leapbits to it.
	 */
	if (sys_leap == LEAP_ADDSECOND || sys_leap == LEAP_DELSECOND)
		leapbits = sys_leap;
#ifdef CISCO

	/* Tell the kernel. */

	switch (leapbits) {
	  case LEAP_ADDSECOND:
	    leap_offset = -1;
	    break;
	  case LEAP_DELSECOND:
	    leap_offset = 1;
	    leap_epoch--;		/* We delete the last second! */
	    break;
	  default:
	    leap_offset = 0;
	    leap_epoch = 0;		/* Don't worry the kernel. */
	    break;
	}
	clock_set_leap_second(leap_epoch, leap_offset);

	/*
	 * Set the clock to expire a few seconds after the event
	 * so that we don't get confused by time marching backwards.
	 * We let the kernel do the work anyhow, so we don't need to be here
	 * right on time;  we just need to come in after the fact to clean
	 * up.
	 */
	setnexttimeout(leapnext + 10);
#else
	setnexttimeout((leapnext > FIVEMINUTES) ? FIVEMINUTES : leapnext);
#endif /* CISCO */
}


/*
 * setnexttimeout - set the next leap alarm
 */
static void
setnexttimeout(secs)
	u_long secs;
{
	/*
	 * We try to aim the time out at between 1 and 1+(1<<EVENT_TIMEOUT)
	 * seconds after the desired time.
	 */
	leap_timer = (secs + 1 + (1<<EVENT_TIMEOUT) + current_time)
	    & ~((1<<EVENT_TIMEOUT)-1);
}


/*
 * leap_setleap - set leap_indicator and/or leap_warning.  Return failure
 *		  if we don't want to do it.
 */
int
leap_setleap(indicator, warning)
	int indicator;
	int warning;
{
	u_long leapnext;
	u_long leaplast;
	l_fp ts;
	int i;

	get_systime(&ts);
	calleapwhen((u_long)ts.l_ui, &leaplast, &leapnext);

	i = 0;
	if (warning != ~0)
		if (leapnext > OKAYTOSETWARNING)
			i = 1;

	if (indicator != ~0)
		if (leapnext > DAYBEFORE)
			i = 1;
	
	if (i) {
#ifndef CISCO
		syslog(LOG_ERR,
		    "attempt to set leap bits at unlikely time of month");
#endif /* CISCO */
		return 0;
	}

	if (warning != ~0)
		leap_warning = warning;

	if (indicator != ~0) {
		if (indicator == LEAP_NOWARNING) {
			leap_warning = LEAP_NOWARNING;
		}
		leap_indicator = indicator;
	}
#ifdef CISCO
	ntp_inform_leap();
#endif /* CISCO */
	return 1;
}

/*
 * leap_actual
 *
 * calculate leap value - pass arg through if no local
 * configuration. Otherwise ise local configuration
 * (only used to cope with broken time servers and
 * broken refclocks)
 *
 * Mapping of leap_indicator:
 *	LEAP_NOWARNING
 *		pass peer value to sys_leap - usual operation
 *	LEAP_ADD/DEL_SECOND
 *		pass  LEAP_ADD/DEL_SECOND to sys_leap
 *	LEAP_NOTINSYNC
 *		pass LEAP_NOWARNING to sys_leap - effectively ignores leap
 */
/* there seems to be a bug in the IRIX 4 compiler which prevents
   u_char from beeing used in prototyped functions
   AIX also suffers from this.
   So give up and define it terms of int.
*/
int
leap_actual(l)
	int l ;
{
	if (leap_indicator != LEAP_NOWARNING) {
		if (leap_indicator == LEAP_NOTINSYNC)
			return LEAP_NOWARNING;
		else
			return leap_indicator;
	} else {
		return l;
	}
}

#ifdef CISCO
/*
 * ntp_inform_leap
 *
 * Inform the leap second support that something interesting has taken place.
 */
void ntp_inform_leap (void)
{
    leap_timer = 1;			/* Wake up the leap process */
}
#endif /* CISCO */
