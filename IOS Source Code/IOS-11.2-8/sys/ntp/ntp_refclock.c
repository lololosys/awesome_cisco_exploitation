/* $Id: ntp_refclock.c,v 3.1.2.3 1996/09/04 23:04:18 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_refclock.c,v $
 *------------------------------------------------------------------
 * NTP reference clock support
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_refclock.c,v $
 * Revision 3.1.2.3  1996/09/04  23:04:18  snyder
 * CSCdi68106:  save some space in ntp
 *              80 from image, 104 from data section
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/08/19  18:55:12  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:03:22  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:28  dkatz
 * Add placeholders.
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

#define CISCO
#define REFCLOCK

/*
 * ntp_refclock - processing support for reference clocks
 */
#ifdef REFCLOCK

#include "master.h"
#include "subsys.h"
#include <string.h>
#include "ttysrv.h"
#include "../util/qsort.h"
#include "config.h"

#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_registry.h"
#include "ntp_registry_private.h"

/*
 * Reference clock support is provided here by maintaining the fiction
 * that the clock is actually a peer. As no packets are exchanged with a
 * reference clock, however, we replace the transmit, receive and packet
 * procedures with separate code to simulate them. Routines
 * refclock_transmit() and refclock_receive() maintain the peer
 * variables in a state analogous to an actual peer and pass reference
 * clock data on through the filters. Routines refclock_peer() and
 * refclock_unpeer() are called to initialize and terminate reference
 * clock associations. A set of utility routines is included to open
 * serial devices, process sample data, edit input lines to extract
 * embedded timestamps and to peform various debugging functions.
 *
 * The main interface used by these routines is the refclockproc
 * structure, which contains for most drivers the decimal equivalants of
 * the year, day, month, hour, second and millisecond/microsecond
 * decoded from the ASCII timecode. Additional information includes the
 * receive timestamp, exception report, statistics tallies, etc. In
 * addition, there may be a driver-specific unit structure used for
 * local control of the device.
 *
 * The support routines are passed a pointer to the peer structure,
 * which is used for all peer-specific processing and contains a pointer
 * to the refclockproc structure, which in turn containes a pointer to
 * the unit structure, if used. In addition, some routines expect an
 * address in the dotted quad form 127.127.t.u, where t is the clock
 * type and u the unit. A table typeunit[type][unit] contains the peer
 * structure pointer for each configured clock type and unit.
 *
 * Most drivers support the 1-pps signal provided by some radios and
 * connected via a level converted described in the gadget directory.
 * The signal is captured using a separate, dedicated serial port and
 * the tty_clk line discipline/streams modules described in the kernel
 * directory. For the highest precision, the signal is captured using
 * the carrier-detect line of the same serial port using the ppsclock
 * streams module described in the ppsclock directory.
 */
#define	REFCLOCKMAXDISPERSE (FP_SECOND/4) /* max sample dispersion */
#define MAXUNIT		4	/* max units */
#ifndef CLKLDISC
#define CLKLDISC	10	/* XXX temp tty_clk line discipline */
#endif
#ifndef CHULDISC
#define CHULDISC	10	/* XXX temp tty_chu line discipline */
#endif

/*
 * Type/unit peer index. Used to find the peer structure for control and
 * debugging. When all clock drivers have been converted to new style,
 * this dissapears.
 */
static struct peer **typeunit;
struct refclock **refclock_conf;

/*
 * Inline returns address of a typeunit entry given clocktype and unit.
 */
static inline struct peer **typeunit_ptr (ulong type, ulong unit)
{
    return (typeunit + (unit * MAXUNIT) + type);
}

/*
 * ntp_refclock_status
 *
 * Returns a human-readable string for the status code
 */
static char *const status_strings[CEVNT_MAX+1] = {
    "Running normally",			/* CEVNT_NOMINAL */
    "Reply timeout",			/* CEVNT_TIMEOUT */
    "Invalid reply",			/* CEVNT_BADREPLY */
    "Clock fault",			/* CEVNT_FAULT */
    "Clock not synchronized",		/* CEVNT_SYNC */
    "Bad date",				/* CEVNT_BADDATE */
    "Bad time",				/* CEVNT_BADTIME */
    "No PPS signal"			/* CEVNT_NOPPS */
};
static char *ntp_refclock_status (u_char status)
{
    if (status > CEVNT_MAX)
	return("Unknown");
    return(status_strings[status]);
}

/*
 * refclock_report - note the occurance of an event
 *
 * This routine presently just remembers the report and logs it, but
 * does nothing heroic for the trap handler. It tries to be a good
 * citizen and bothers the system log only if things change.
 */
void
refclock_report(peer, code)
	struct peer *peer;
	u_int code;
{
	struct refclockproc *pp;

	if (!(pp = peer->procptr))
		return;
	if (code == CEVNT_BADREPLY)
		pp->badformat++;
	if (code == CEVNT_BADTIME)
		pp->baddata++;
	if (code == CEVNT_TIMEOUT)
		pp->noreply++;
	if (pp->currentstatus != code) {
		pp->currentstatus = code;
		pp->lastevent = code;
#ifdef CISCO
		if (ntp_event_debug) {
		    buginf("\nNTP: refclock line %d status: %s",
			   peer->refclkunit, ntp_refclock_status(code));
		}
#else
		if (code == CEVNT_FAULT)
		    NLOG(NLOG_CLOCKEVENT)
		      syslog(LOG_ERR,
			     "clock %s fault '%s' (0x%02x)",
			     refnumtoa(peer->srcadr.sin_addr.s_addr), ceventstr(code), code);
		else {
		    NLOG(NLOG_CLOCKEVENT)
		      syslog(LOG_INFO,
			     "clock %s event '%s' (0x%02x)",
			     refnumtoa(peer->srcadr.sin_addr.s_addr), ceventstr(code), code);
		}
#endif /* CISCO */
	}
}


/*
 * init_refclock - initialize the reference clock drivers
 *
 * This routine calls each of the drivers in turn to initialize internal
 * variables, if necessary. Most drivers have nothing to say at this
 * point.
 */
static void
init_refclock(void)
{
	int i;

	typeunit = malloc(sizeof(struct peer *) * NUM_REFCLK_TYPES * MAXUNIT);
	if (!typeunit)
	    return;

	for (i = 0; i < NUM_REFCLK_TYPES; i++) {
	    if (refclock_conf[i] != NULL &&
		refclock_conf[i]->clock_init != NULL) {
		(refclock_conf[i]->clock_init)();
	    }
	}
}


/*
 * refclock_newpeer - initialize and start a reference clock
 *
 * This routine allocates and initializes the interface structure which
 * supports a reference clock in the form of an ordinary NTP peer. A
 * driver-specific support routine completes the initialization, if
 * used. Default peer variables which identify the clock and establish
 * its reference ID and stratum are set here. It returns one if success
 * and zero if the clock address is invalid or already running,
 * insufficient resources are available or the driver declares a bum
 * rap.
 */
int
refclock_newpeer(peer)
	struct peer *peer;	/* peer structure pointer */
{
	struct refclockproc *pp;
	u_char clktype;
	int unit;

	/*
	 * Check for valid clock address. If already running, shut it
	 * down first.
	 */
	if (!ISREFCLOCKADR(&peer->srcadr)) {
#ifndef CISCO
		syslog(LOG_ERR,
		    "refclock_newpeer: clock address %s invalid",
		    ntoa(&peer->srcadr));
#endif /* CISCO */
		return (0);
	}
	clktype = REFCLOCKTYPE(&peer->srcadr);
	unit = REFCLOCKUNIT(&peer->srcadr);
	if (!refclock_conf || clktype >= NUM_REFCLK_TYPES || unit >= MAXUNIT ||
	    refclock_conf[clktype]->clock_start == NULL) {
#ifndef CISCO
		syslog(LOG_ERR,
		    "refclock_newpeer: clock type %d invalid\n",
		    clktype);
#endif /* CISCO */
		return (0);
	}
	refclock_unpeer(peer);

	/*
	 * Allocate and initialize interface structure
	 */
	
	if (!(pp = (struct refclockproc *)
	    emalloc(sizeof(struct refclockproc))))
		return (0);
	memset((char *)pp, 0, sizeof(struct refclockproc));
	*typeunit_ptr(clktype, unit) = peer;
	peer->procptr = pp;
#ifdef CISCO
	pp->refclock_peer = peer;
#endif /* CISCO */

	/*
	 * Initialize structures
	 */
	peer->refclktype = clktype;
	peer->refclkunit = unit;
	peer->flags |= FLAG_REFCLOCK;
	peer->event_timer.peer = peer;
	peer->event_timer.event_handler = refclock_transmit;
	pp->type = clktype;
	pp->timestarted = current_time;
	peer->refid = peer->srcadr.sin_addr.s_addr;
	peer->maxpoll = peer->minpoll;

	/*
	 * Do driver dependent initialization
	 */
	if (!refclock_conf || !refclock_conf[clktype] ||
	    !((refclock_conf[clktype]->clock_start)(unit, peer))) {
		free(pp);
		return (0);
	}
	peer->hpoll = peer->minpoll;
	peer->ppoll = peer->maxpoll;
	if (peer->stratum <= 1)
		peer->refid = pp->refid;
	else
		peer->refid = peer->srcadr.sin_addr.s_addr;

	/*
	 * Set up the timeout for polling and reachability determination
	 */
#ifdef CISCO
	peer->event_timer.event_time = current_time + ranp2(NTP_MINPOLL) + 1;
#else
	if (initializing) {
		init_peer_starttime += (1 << EVENT_TIMEOUT);
		if (init_peer_starttime >= (1 << peer->minpoll))
			init_peer_starttime = (1 << EVENT_TIMEOUT);
		peer->event_timer.event_time = init_peer_starttime;
	} else {
		peer->event_timer.event_time = current_time +
		    (1 << peer->hpoll);
	}
#endif /* CISCO */
	TIMER_ENQUEUE(timerqueue, &peer->event_timer);
	return (1);
}


/*
 * refclock_unpeer - shut down a clock
 */
void
refclock_unpeer(peer)
	struct peer *peer;	/* peer structure pointer */
{
	u_char clktype;
	int unit;

	/*
	 * Wiggle the driver to release its resources, then give back
	 * the interface structure.
	 */
	if (!peer->procptr)
		return;
	clktype = peer->refclktype;
	unit = peer->refclkunit;
	if (refclock_conf && refclock_conf[clktype]->clock_shutdown != NULL)
		(refclock_conf[clktype]->clock_shutdown)(unit, peer);
	free(peer->procptr);
	peer->procptr = 0;
}


/*
 * refclock_transmit - simulate the transmit procedure
 *
 * This routine implements the NTP transmit procedure for a reference
 * clock. This provides a mechanism to call the driver at the NTP poll
 * interval, as well as provides a reachability mechanism to detect a
 * broken radio or other madness.
 */
void
refclock_transmit(peer)
	struct peer *peer;	/* peer structure pointer */
{
	struct refclockproc *pp;
	u_char clktype;
	int unit;
	u_char opeer_reach;

	pp = peer->procptr;
	clktype = peer->refclktype;
	unit = peer->refclkunit;
	peer->sent++;

	/*
	 * The transmit procedure is supposed to freeze a timestamp.
	 * Get one just for fun, and to tell when we last were here.
	 */
	get_systime(&peer->xmt);

	/*
	 * Fiddle reachability.
	 */
	opeer_reach = peer->reach;
	peer->reach <<= 1;
	if (peer->reach == 0) {
		/*
		 * Clear this one out. No need to redo selection since
		 * this fellow will definitely be suffering from
		 * dispersion madness.
		 */
		if (opeer_reach != 0) {
			peer_clear(peer);
			peer->timereachable = current_time;
			report_event(EVNT_UNREACH, peer);
		}

	/*
	 * Update reachability and poll variables
	 */
	} else if ((opeer_reach & 3) == 0) {
		l_fp off;

		if (peer->valid > 0)
			peer->valid--;
		L_CLR(&off);
		clock_filter(peer, &off, 0, NTP_MAXDISPERSE);
		if (peer->flags & FLAG_SYSPEER)
			clock_select();
	} else if (peer->valid < NTP_SHIFT)
		peer->valid++;

	/*
	 * If he wants to be polled, do it. New style drivers do not use
	 * the unit argument, since the fudge stuff is in the
	 * refclockproc structure.
	 */
	if (refclock_conf && refclock_conf[clktype]->clock_poll != NULL)
		(refclock_conf[clktype]->clock_poll)(unit, peer);

	/*
	 * Finally, reset the timer
	 */
	if (peer->startup_polls) {
	    peer->startup_polls--;
	    peer->event_timer.event_time += 1;/* Rapidly do the first bunch */
	} else {
	    peer->event_timer.event_time += (1 << peer->hpoll);
	}
	TIMER_ENQUEUE(timerqueue, &peer->event_timer);
}


/*
 * Compare two l_fp's - used with qsort()
 */
static int
refclock_cmpl_fp(const void *p1, const void *p2) /* l_fp to compare */
{

	if (!L_ISGEQ((l_fp *)p1, (l_fp *)p2))
		return (-1);
	if (L_ISEQU((l_fp *)p1, (l_fp *)p2))
		return (0);
	return (1);
}

/*
 * refclock_new_sample
 *
 * Process a new sample, in NTP timestamp format.
 */
int refclock_new_sample (struct refclockproc *pp, int nstart, int nskeep,
			 l_fp *timestamp)
{

    L_ADD(timestamp, &pp->fudgetime1);
    L_ADD(timestamp, &pp->fudgetime2);
    
    pp->lastref = *timestamp;	/* save last reference time */
    
    /*
     * Include the configured fudgetime1 adjustment.
     */
    L_SUB(timestamp, &pp->lastrec); /* form true offset */
    
    return refclock_sample(timestamp, pp, nstart, nskeep);
}

/*
 * refclock_process - process a pile of samples from the clock
 *
 * This routine converts the timecode in the form days, hours, miinutes,
 * seconds, milliseconds/microseconds to internal timestamp format.
 * Further processing is then delegated to refclock sample
 *
 * Cisco version--if the year is nonzero, believe it.  If the month is
 * nonzero, the day is day-in-month instead of day-in-year.  If "decrement"
 * is TRUE, subtract a second from the converted timestamp (the PPS kludge).
 */
int
refclock_process(pp, nstart, nskeep, decrement)
	struct refclockproc *pp; /* peer structure pointer */
	int nstart;		/* stages of median filter */
	int nskeep;		/* stages after outlyer trim */
        boolean decrement;      /* Knock off a second */
{
	l_fp offset;
	timeval tv;

	if (!pp->year || !pp->month) { /* Uhoh */
	    return(0);			/* Forget it for now */
	}

	tv.year = pp->year;
	tv.month = pp->month;
	tv.day = pp->day;
	tv.hour = pp->hour;
	tv.minute = pp->minute;
	tv.second = pp->second;
	if (pp->usec) {			/* We only do millseconds */
	    tv.millisecond = pp->usec / 1000;
	} else {
	    tv.millisecond = pp->msec;
	}
	tv.tz_offset = pp->tz_offset;
	clock_timeval_to_epoch(&tv, &offset);
	if (decrement)
	    offset.l_ui--;

	return(refclock_new_sample(pp, nstart, nskeep, &offset));
}

/*
 * refclock_sample - process a pile of samples from the clock
 *
 * This routine converts the timecode in the form days, hours, miinutes,
 * seconds, milliseconds/microseconds to internal timestamp format. It
 * then calculates the difference from the receive timestamp and
 * assembles the samples in a shift register. It implements a recursive
 * median filter to suppress spikes in the data, as well as determine a
 * rough dispersion estimate. A configuration constant time adjustment
 * fudgetime1 can be added to the final offset to compensate for various
 * systematic errors. The routine returns one if success and zero if
 * failure due to invalid timecode data or very noisy offsets.
 *
 * This interface is needed to allow for clocks (e. g. parse) that can
 * provide the correct offset including year information (though NTP
 * usually gives up on offsets greater than 1000 seconds).
 */
int
refclock_sample(sample_offset, pp, nstart, nskeep)
        l_fp *sample_offset;	/* input offset (offset! - not a time stamp)
				   for filter machine */
	struct refclockproc *pp; /* peer structure pointer */
	int nstart;		/* stages of median filter */
	int nskeep;		/* stages after outlyer trim */
{
	int i, n;
	l_fp offset, median, lftmp;
	l_fp off[MAXSTAGE];
	u_fp disp;

        /*
	 * Subtract the receive timestamp from the timecode timestamp
	 * to form the raw offset. Insert in the median filter shift
	 * register.
	 */
	pp->nstages = nstart;
	offset = *sample_offset;

        i = ((int)(pp->coderecv)) % pp->nstages;

	pp->filter[i] = offset;
	if (pp->coderecv == 0)
		for (i = 1; i < pp->nstages; i++)
			pp->filter[i] = pp->filter[0];
	pp->coderecv++;

	/*
	 * Copy the raw offsets and sort into ascending order
	 */
	for (i = 0; i < pp->nstages; i++)
		off[i] = pp->filter[i];
	qsort((char *)off, pp->nstages, sizeof(l_fp), refclock_cmpl_fp);

	/*
	 * Reject the furthest from the median of nstages samples until
	 * nskeep samples remain.
	 */
	i = 0;
	n = pp->nstages;
	while ((n - i) > nskeep) {
		lftmp = off[n - 1];
		median = off[(n + i) / 2];
		L_SUB(&lftmp, &median);
		L_SUB(&median, &off[i]);
		if (L_ISHIS(&median, &lftmp)) {
			/* reject low end */
			i++;
		} else {
			/* reject high end */
			n--;
		}
	}

	/*
	 * Compute the dispersion based on the difference between the
	 * extremes of the remaining offsets. Add to this the time since
	 * the last clock update, which represents the dispersion
	 * increase with time. We know that NTP_MAXSKEW is 16. If the
	 * sum is greater than the allowed sample dispersion, bail out.
	 * If the loop is unlocked, return the most recent offset;
	 * otherwise, return the median offset.
	 */
	lftmp = off[n - 1];
	L_SUB(&lftmp, &off[i]);
	disp = LFPTOFP(&lftmp) + current_time - pp->lasttime;
	if (disp > REFCLOCKMAXDISPERSE)
		return (0);

	pp->offset = offset;
	pp->dispersion = disp;

	return (1);
}


/*
 * refclock_receive - simulate the receive and packet procedures
 *
 * This routine simulates the NTP receive and packet procedures for a
 * reference clock. This provides a mechanism in which the ordinary NTP
 * filter, selection and combining algorithms can be used to suppress
 * misbehaving radios and to mitigate between them when more than one is
 * available for backup.
 */
void
refclock_receive(peer, offset, delay, dispersion, reftime, rectime, leap)
	struct peer *peer;	/* peer structure pointer */
	l_fp *offset;		/* computed offset (s) */
	s_fp delay;		/* computed delay to peer */
	u_fp dispersion;	/* computed dispersion to peer */
	l_fp *reftime;		/* time at last clock update */
	l_fp *rectime;		/* time at last peer update */
	int leap;		/* synchronization/leap code */
{
	int restrict;
	int trustable;
	int refclock_own_states;
	u_fp precision;

	peer->received++;
#ifdef DEBUG
	if (debug)
		printf("refclock_receive: %s %s %s %s)\n",
		    ntoa(&peer->srcadr), lfptoa(offset, 6),
		    fptoa(delay, 5), ufptoa(dispersion, 5));
#endif

	/*
         * some refclock implementations do a complete state and
	 * event handling. reporting events must be disabled for
	 * these critters (namely parse)
	 */
	
	refclock_own_states = leap & REFCLOCK_OWN_STATES;
	leap &= ~REFCLOCK_OWN_STATES;

	/*
	 * The authentication and access-control machinery works, but
	 * its utility may be questionable.
	 */
#ifdef CISCO
	restrict = ntp_restrictions(&peer->srcadr);
#else
	restrict = restrictions(&peer->srcadr);
#endif /* CISCO */
	if (restrict & (RES_IGNORE|RES_DONTSERVE))
		return;
	peer->processed++;
	peer->timereceived = current_time;
	if (restrict & RES_DONTTRUST)
		trustable = 0;
	else
		trustable = 1;

	if (peer->flags & FLAG_AUTHENABLE) {
		if (trustable)
			peer->flags |= FLAG_AUTHENTIC;
		else
			peer->flags &= ~FLAG_AUTHENTIC;
	}
	peer->leap = leap;

#ifdef CISCO				/* This can't be right */
	peer->rec = *rectime;
	peer->org = peer->reftime = *reftime;
#else
	/*
	 * Set the timestamps. rec and org are in local time, while ref
	 * is in timecode time.
	 */
	peer->rec = peer->org = *rectime;
	peer->reftime = *reftime;
#endif /* CISCO */

	/*
	 * If the interface has been set to any_interface, set it to the
	 * loopback address if we have one. This is so that peers which
	 * are unreachable are easy to see in the peer display.
	 */
	if (peer->dstadr == any_interface && loopback_interface != 0)
		peer->dstadr = loopback_interface;

	/*
	 * Set peer.pmode based on the hmode. For appearances only.
	 */
	switch (peer->hmode) {

	case MODE_ACTIVE:
		peer->pmode = MODE_PASSIVE;
		break;

	default:
		peer->pmode = MODE_SERVER;
		break;
	}

	/*
	 * Abandon ship if the radio came bum. We only got this far
	 * in order to make pretty billboards, even if bum.
	 */
	if (leap == LEAP_NOTINSYNC)
		return;
	/*
	 * If this guy was previously unreachable, report him
	 * reachable.
	 */
	if (peer->reach == 0) report_event(EVNT_REACH, peer);
		peer->reach |= 1;

	/*
	 * Give the data to the clock filter and update the clock. Note
	 * the clock reading precision initialized by the driver is
	 * added at this point.
	 */
	precision = FP_SECOND >> -(int)peer->precision;
	if (precision == 0)
		precision = 1;

	if (!refclock_own_states)
	  refclock_report(peer, CEVNT_NOMINAL);

	clock_filter(peer, offset, delay, dispersion + precision);
	clock_update(peer);
}


/*
 * refclock_gtlin - groom next input line and extract timestamp
 *
 * This routine processes the timecode received from the clock and
 * removes the parity bit and control characters. If a timestamp is
 * present in the timecode, as produced by the tty_clk line
 * discipline/streams module, it returns that as the timestamp;
 * otherwise, it returns the buffer timestamp. The routine return code
 * is the number of characters in the line.
 */
int
refclock_gtlin(rbufp, lineptr, bmax, tsptr)
	struct recvbuf *rbufp;	/* receive buffer pointer */
	char *lineptr;		/* current line pointer */
	int bmax;		/* remaining characters in line */
	l_fp *tsptr;		/* pointer to timestamp returned */
{
	char *dpt, *dpend, *dp;
	int i;
#ifdef CISCO
	l_fp trtmp;
#else
	l_fp trtmp, tstmp;
#endif /* CISCO */
	char c;

	/*
	 * Check for the presence of a timestamp left by the tty_clock
	 * line discipline/streams module and, if present, use that
	 * instead of the buffer timestamp captured by the I/O routines.
	 * We recognize a timestamp by noting its value is earlier than
	 * the buffer timestamp, but not more than one second earlier.
	 */
	dpt = (char *)&rbufp->recv_space;
	dpend = dpt + rbufp->recv_length;
	trtmp = rbufp->recv_time;
#ifndef CISCO
	if (dpend >= dpt + 8) {
		if (buftvtots(dpend - 8, &tstmp)) {
			L_SUB(&trtmp, &tstmp);
			if (trtmp.l_ui == 0) {
#ifdef DEBUG
				if (debug) {
					printf(
				    "refclock_gtlin: fd %d ldisc %s",
					    rbufp->fd,
					    lfptoa(&trtmp, 6));
					gettstamp(&trtmp);
					L_SUB(&trtmp, &tstmp);
					printf(" sigio %s\n",
					    lfptoa(&trtmp, 6));
				}
#endif
				dpend -= 8;
				trtmp = tstmp;
			} else
				trtmp = rbufp->recv_time;
		}
	}
#endif /* CISCO */

	/*
	 * Edit timecode to remove control chars. Don't monkey with the
	 * line buffer if the input buffer contains no ASCII printing
	 * characters.
	 */
	if (dpend - dpt > bmax - 1)
		dpend = dpt + bmax - 1;
	for (dp = lineptr; dpt < dpend; dpt++) {
		c = *dpt & 0x7f;
		if (c >= ' ')
			*dp++ = c;
	}
	i = dp - lineptr;
	if (i > 0)
		*dp = '\0';

#ifdef DEBUG
	if (debug)
        	printf("refclock_gtlin: fd %d time %s timecode %d %s\n",
		    rbufp->fd, ulfptoa(&trtmp, 6), i, lineptr);
#endif
	*tsptr = trtmp;
	return (i);
}

/*
 * The following code does not apply to WINNT & VMS ...
 */
#if defined(HAVE_TERMIOS) || defined(HAVE_SYSV_TTYS) || defined(HAVE_BSD_TTYS)

/*
 * refclock_open - open serial port for reference clock
 *
 * This routine opens a serial port for I/O and sets default options. It
 * returns the file descriptor if success and zero if failure.
 */
int
refclock_open(dev, speed, flags)
	char *dev;		/* device name pointer */
	int speed;		/* serial port speed (code) */
	int flags;		/* line discipline flags */
{
#ifndef CISCO
	int fd;
#ifdef HAVE_TERMIOS
	struct termios ttyb, *ttyp;
#endif /* HAVE_TERMIOS */
#ifdef HAVE_SYSV_TTYS
	struct termio ttyb, *ttyp;
#endif /* HAVE_SYSV_TTYS */
#ifdef HAVE_BSD_TTYS
	struct sgttyb ttyb, *ttyp;
#endif /* HAVE_BSD_TTYS */
#ifdef HAVE_MODEM_CONTROL
	u_long ltemp;
#endif /* HAVE_MODEM_CONTROL */

	/*
	 * Open serial port and set default options
	 */
	fd = open(dev, O_RDWR, 0777);
	if (fd == -1) {
		syslog(LOG_ERR, "refclock_open: %s: %m", dev);
		return (0);
	}

	/*
	 * The following sections initialize the serial line port in
	 * canonical (line-oriented) mode and set the specified line
	 * speed, 8 bits and no parity. The modem control, break, erase
	 * and kill functions are normally disabled. There is a
	 * different section for each terminal interface, as selected at
	 * compile time.
	 */
	ttyp = &ttyb;
#ifdef HAVE_TERMIOS

	/*
	 * POSIX serial line parameters (termios interface)
	 */
	if (tcgetattr(fd, ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d tcgetattr %m", fd);
		return (0);
	}

	/*
	 * Set canonical mode and local connection; set specified speed,
	 * 8 bits and no parity; map CR to NL; ignore break.
	 */
	ttyp->c_iflag = IGNBRK | IGNPAR | ICRNL;
	ttyp->c_oflag = 0;
	ttyp->c_cflag = CS8 | CLOCAL | CREAD;
	(void)cfsetispeed(&ttyb, speed);
	(void)cfsetospeed(&ttyb, speed);
	ttyp->c_lflag = ICANON;
	ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\0';
#ifdef HAVE_MODEM_CONTROL
	/*
	 * If we have modem control, check to see if modem leads are
	 * active; if so, set remote connection. This is necessary for
	 * the kernel pps mods to work.
	 */
	ltemp = 0;
	if (ioctl(fd, TIOCMGET, (char *)&ltemp) < 0)
		syslog(LOG_ERR,
		    "refclock_open: fd %d TIOCMGET %m", fd);
#if DEBUG
	if (debug)
		printf("refclock_open: fd %d modem status %lx\n",
		    fd, ltemp);
#endif
	if (ltemp & TIOCM_DSR)
		ttyp->c_cflag &= ~CLOCAL;
#endif /* HAVE_MODEM_CONTROL */
	if (tcsetattr(fd, TCSANOW, ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d tcsetattr %m", fd);
		return (0);
	}
	if (tcflush(fd, TCIOFLUSH) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d tcflush %m", fd);
		return (0);
	}
#endif /* HAVE_TERMIOS */

#ifdef HAVE_SYSV_TTYS

	/*
	 * System V serial line parameters (termio interface)
	 *
	 */
	if (ioctl(fd, TCGETA, ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d TCGETA %m", fd);
		return (0);
	}

	/*
	 * Set canonical mode and local connection; set specified speed,
	 * 8 bits and no parity; map CR to NL; ignore break.
	 */
	ttyp->c_iflag = IGNBRK | IGNPAR | ICRNL;
	ttyp->c_oflag = 0;
	ttyp->c_cflag = speed | CS8 | CLOCAL | CREAD;
	ttyp->c_lflag = ICANON;
	ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\0';
#ifdef HAVE_MODEM_CONTROL
	/*
	 * If we have modem control, check to see if modem leads are
	 * active; if so, set remote connection. This is necessary for
	 * the kernel pps mods to work.
	 */
	ltemp = 0;
	if (ioctl(fd, TIOCMGET, (char *)&ltemp) < 0)
		syslog(LOG_ERR,
		    "refclock_open: fd %d TIOCMGET %m", fd);
#if DEBUG
	if (debug)
		printf("refclock_open: fd %d modem status %lx\n",
		    fd, ltemp);
#endif
	if (ltemp & TIOCM_DSR)
		ttyp->c_cflag &= ~CLOCAL;
#endif /* HAVE_MODEM_CONTROL */
	if (ioctl(fd, TCSETA, ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d TCSETA %m", fd);
		return (0);
	}
#endif /* HAVE_SYSV_TTYS */

#ifdef HAVE_BSD_TTYS

	/*
	 * 4.3bsd serial line parameters (sgttyb interface)
	 */
	if (ioctl(fd, TIOCGETP, (char *)ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: fd %d TIOCGETP %m", fd);
		return (0);
	}
	ttyp->sg_ispeed = ttyp->sg_ospeed = speed;
	ttyp->sg_flags = EVENP | ODDP | CRMOD;
	if (ioctl(fd, TIOCSETP, (char *)ttyp) < 0) {
		syslog(LOG_ERR,
		    "refclock_open: TIOCSETP %m");
		return (0);
	}
#endif /* HAVE_BSD_TTYS */

	if (!refclock_ioctl(fd, flags)) {
		(void)close(fd);
		syslog(LOG_ERR, "refclock_open: fd %d ioctl fails",
		    fd);
		return (0);
	}
	return (fd);
#else
	return(0);
#endif /* CISCO */
}
#endif /* HAVE_TERMIOS || HAVE_SYSV_TTYS || HAVE_BSD_TTYS */


/*
 * refclock_ioctl - set serial port control functions
 *
 * This routine attempts to hide the internal, system-specific details
 * of serial ports. It can handle POSIX (termios), SYSV (termio) and BSD
 * (sgtty) interfaces with varying degrees of success. The routine sets
 * up the tty_clk, chu_clk and ppsclock streams module/line discipline,
 * if compiled in the daemon and requested in the call. The routine
 * returns one if success and zero if failure.
 */
int
refclock_ioctl(fd, flags)
	int fd;			/* file descriptor */
	int flags;		/* line discipline flags */
{
#ifndef CISCO
	/* simply return 1 if no UNIX line discipline is supported */
#if defined(HAVE_TERMIOS) || defined(HAVE_SYSV_TTYS) || defined(HAVE_BSD_TTYS)

#ifdef HAVE_TERMIOS
	struct termios ttyb, *ttyp;
#endif /* HAVE_TERMIOS */
#ifdef HAVE_SYSV_TTYS
        struct termio ttyb, *ttyp;
#endif /* HAVE_SYSV_TTYS */
#ifdef HAVE_BSD_TTYS
	struct sgttyb ttyb, *ttyp;
#endif /* HAVE_BSD_TTYS */

#ifdef DEBUG
	if (debug)
		printf("refclock_ioctl: fd %d flags %x\n",
		    fd, flags);
#endif

	/*
	 * The following sections select optional features, such as
	 * modem control, line discipline and so forth. Some require
	 * specific operating system support in the form of streams
	 * modules, which can be loaded and unloaded at run time without
	 * rebooting the kernel, or line discipline modules, which must
	 * be compiled in the kernel. The streams modules require System
	 * V STREAMS support, while the line discipline modules require
	 * 4.3bsd or later. The checking frenzy is attenuated here,
	 * since the device is already open.
	 *
	 * Note that both the clk and ppsclock modules are optional; the
	 * dang thing still works, but the accuracy improvement using
	 * them will not be available. The ppsclock module is associated
	 * with a specific, declared line and should be used only once.
	 * If requested, the chu module is mandatory, since the driver
	 * will not work without it.
	 *
	 * Use the LDISC_PPS option ONLY with Sun baseboard ttya or
	 * ttyb. Using it with the SPIF multipexor crashes the kernel.
	 */
	if (flags == 0)
		return (1);

#if !(defined(HAVE_TERMIOS) || defined(HAVE_BSD_TTYS))
	if (flags & (LDISC_CLK | LDISC_CHU | LDISC_PPS | LDISC_ACTS))
		syslog(LOG_ERR,
	    "refclock_ioctl: unsupported terminal interface");
		return (0);
#endif /* HAVE_TERMIOS HAVE_BSD_TTYS */

	ttyp = &ttyb;

#ifdef STREAM
#ifdef CLK

	/*
	 * The CLK option provides timestamping at the driver level. 
	 * It requires the tty_clk streams module and System V STREAMS
	 * support. 
	 */
	if (flags & (LDISC_CLK | LDISC_CLKPPS | LDISC_ACTS)) {
		if (ioctl(fd, I_PUSH, "clk") < 0)
			syslog(LOG_NOTICE,
	    "refclock_ioctl: optional clk streams module unavailable");
		else {
			char *str;

			if (flags & LDISC_PPS)
				str = "\377";
			else if (flags & LDISC_ACTS)
				str = "*";
			else
				str = "\n";
			if (ioctl(fd, CLK_SETSTR, str) < 0)
				syslog(LOG_ERR,
				    "refclock_ioctl: CLK_SETSTR %m");
		}
	}

	/*
	 * The ACTS line discipline requires additional line-ending
	 * character '*'.
	 */
	if (flags & LDISC_ACTS) {
		(void)tcgetattr(fd, ttyp);
		ttyp->c_cc[VEOL] = '*';
		(void)tcsetattr(fd, TCSANOW, ttyp);
	}
#else
	if (flags & LDISC_CLK)
		syslog(LOG_NOTICE,
	    "refclock_ioctl: optional clk streams module unsupported");
#endif /* CLK */
#ifdef CHU

	/*
	 * The CHU option provides timestamping and decoding for the CHU
	 * timecode. It requires the tty_chu streams module and System V
	 * STREAMS support.
	 */
	if (flags & LDISC_CHU) {
		(void)tcgetattr(fd, ttyp);
		ttyp->c_lflag = 0;
		ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\0';
		ttyp->c_cc[VMIN] = 1;
		ttyp->c_cc[VTIME] = 0;
		(void)tcsetattr(fd, TCSANOW, ttyp);
		(void)tcflush(fd, TCIOFLUSH);
		while (ioctl(fd, I_POP, 0) >= 0);
		if (ioctl(fd, I_PUSH, "chu") < 0) {
			syslog(LOG_ERR,
	    "refclock_ioctl: required chu streams module unavailable");
			return (0);
		}
	}
#else
	if (flags & LDISC_CHU) {
		syslog(LOG_ERR,
	    "refclock_ioctl: required chu streams module unsupported");
		return (0);
	}
#endif /* CHU */
#ifdef PPS

	/*
	 * The PPS option provides timestamping at the driver level.
	 * It uses a 1-pps signal and level converter (gadget box) and
	 * requires the ppsclock streams module and System V STREAMS
	 * support.
	 */
	if (flags & LDISC_PPS) {
		if (fdpps != -1) {
			syslog(LOG_ERR,
		    "refclock_ioctl: ppsclock already configured");
			return (0);
		}
		if (ioctl(fd, I_PUSH, "ppsclock") < 0)
			syslog(LOG_NOTICE,
	    "refclock_ioctl: optional ppsclock streams module unavailable");
		else
			fdpps = fd;
	}
#else
	if (flags & LDISC_PPS)
		syslog(LOG_NOTICE,
	    "refclock_ioctl: optional ppsclock streams module unsupported");
#endif /* PPS */

#else /* STREAM */

#ifdef HAVE_TERMIOS
#ifdef CLK

	/*
	 * The CLK option provides timestamping at the driver level. It
	 * requires the tty_clk line discipline and 4.3bsd or later.
	 */
	if (flags & (LDISC_CLK | LDISC_CLKPPS | LDISC_ACTS)) {
		(void)tcgetattr(fd, ttyp);
		ttyp->c_lflag = 0;
		if (flags & LDISC_CLKPPS)
			ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\377';
		else if (flags & LDISC_ACTS) {
			ttyp->c_cc[VERASE] = '*';
			ttyp->c_cc[VKILL] = '#';
		} else
			ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\n';
		ttyp->c_cc[VMIN] = 1;
		ttyp->c_cc[VTIME] = 0;
		ttyp->c_line = CLKLDISC;
		(void)tcsetattr(fd, TCSANOW, ttyp);
		(void)tcflush(fd, TCIOFLUSH);
	}
#else
	if (flags & LDISC_CLK)
		syslog(LOG_NOTICE,
		"refclock_ioctl: optional clk line discipline unsupported");
#endif /* CLK */
#ifdef CHU
	/*
	 * The CHU option provides timestamping and decoding for the CHU
	 * timecode. It requires the tty_chu line disciplne and 4.3bsd
	 * or later.
	 */
	if (flags & LDISC_CHU) {
		(void)tcgetattr(fd, ttyp);
		ttyp->c_lflag = 0;
		ttyp->c_cc[VERASE] = ttyp->c_cc[VKILL] = '\r';
		ttyp->c_cc[VMIN] = 1;
		ttyp->c_cc[VTIME] = 0;
		ttyp->c_line = CHULDISC;
		(void)tcsetattr(fd, TCSANOW, ttyp) < 0);
		(void)tcflush(fd, TCIOFLUSH);
	}
#else
	if (flags & LDISC_CHU) {
		syslog(LOG_ERR,
		"refclock_ioctl: required chu line discipline unsupported");
		return (0);
	}
#endif /* CHU */
#endif /* HAVE_TERMIOS */

#ifdef HAVE_BSD_TTYS
#ifdef CLK

	/*
	 * The CLK option provides timestamping at the driver level. It
	 * requires the tty_clk line discipline and 4.3bsd or later.
	 */
	if (flags & (LDISC_CLK | LDISC_CLKPPS | LDISC_ACTS)) {
		int ldisc = CLKLDISC;

		(void)ioctl(fd, TIOCGETP, (char *)ttyp);
		if (flags & LDISC_CLKPPS)
			ttyp->sg_erase = ttyp->sg_kill = '\377';
		else if (flags & LDISC_ACTS) {
			ttyp->sg_erase = '*';
			ttyp->sg_kill = '#';
		} else
			ttyp->sg_erase = ttyp->sg_kill = '\r';
		ttyp->sg_flags = RAW;
		(void)ioctl(fd, TIOCSETP, ttyp);
		if (ioctl(fd, TIOCSETD, (char *)&ldisc) < 0)
			syslog(LOG_NOTICE,
	    "refclock_ioctl: optional clk line discipline unavailable");
	}
#else
	if (flags & LDISC_CLK)
		syslog(LOG_NOTICE,
	    "refclock_ioctl: optional clk line discipline unsupported");

#endif /* CLK */
#ifdef CHU

	/*
	 * The CHU option provides timestamping and decoding for the CHU
	 * timecode. It requires the tty_chu line disciplne and 4.3bsd
	 * or later.
	 */
	if (flags & LDISC_CHU) {
		int ldisc = CHULDISC;

		(void)ioctl(fd, TIOCGETP, (char *)ttyp);
		ttyp->sg_erase = ttyp->sg_kill = '\r';
		ttyp->sg_flags = RAW;
		(void)ioctl(fd, TIOCSETP, (char *)ttyp);
		if (ioctl(fd, TIOCSETD, (char *)&ldisc) < 0) {
			syslog(LOG_ERR,
	    "refclock_ioctl: required chu line discipline unavailable");
			return (0);
		}
	}
#else
	if (flags & LDISC_CHU) {
		syslog(LOG_ERR,
	    "refclock_ioctl: required chu line discipline unsupported");
		return (0);
	}
#endif /* CHU */
#endif /* HAVE_BSD_TTYS */

#endif /* STREAM */

#endif /* HAVE_TERMIOS || HAVE_SYSV_TTYS || HAVE_BSD_TTYS */

#endif /* CISCO */
	return (1);
}

/*
 * refclock_control - set and/or return clock values
 *
 * This routine is used mainly for debugging. It returns designated
 * values from the interface structure that can be displayed using
 * xntpdc and the clockstat command. It can also be used to initialize
 * configuration variables, such as fudgetimes, fudgevalues, reference
 * ID and stratum.
 */
void
refclock_control(srcadr, in, out)
	struct sockaddr_in *srcadr;
	struct refclockstat *in;
	struct refclockstat *out;
{
	struct peer *peer;
	struct refclockproc *pp;
	u_char clktype;
	int unit;

	/*
	 * Check for valid address and running peer
	 */
	if (!ISREFCLOCKADR(srcadr))
		return;
	clktype = REFCLOCKTYPE(srcadr);
	unit = REFCLOCKUNIT(srcadr);
	if (clktype >= NUM_REFCLK_TYPES || unit >= MAXUNIT)
		return;
	if (!(peer = *typeunit_ptr(clktype, unit)))
		return;
	pp = peer->procptr;

	/*
	 * Initialize requested data
	 */
	if (in != 0) {
		if (in->haveflags & CLK_HAVETIME1)
			pp->fudgetime1 = in->fudgetime1;
		if (in->haveflags & CLK_HAVETIME2)
			pp->fudgetime2 = in->fudgetime2;
		if (in->haveflags & CLK_HAVEVAL1)
			peer->stratum = in->fudgeval1;
		if (in->haveflags & CLK_HAVEVAL2)
			pp->refid = in->fudgeval2;
		if (peer->stratum <= 1)
			peer->refid = pp->refid;
		else
			peer->refid = peer->srcadr.sin_addr.s_addr;
		if (in->haveflags & CLK_HAVEFLAG1) {
			pp->sloppyclockflag &= ~CLK_FLAG1;
			pp->sloppyclockflag |= in->flags & CLK_FLAG1;
		}
		if (in->haveflags & CLK_HAVEFLAG2) {
			pp->sloppyclockflag &= ~CLK_FLAG2;
			pp->sloppyclockflag |= in->flags & CLK_FLAG2;          
		}
		if (in->haveflags & CLK_HAVEFLAG3) {
			pp->sloppyclockflag &= ~CLK_FLAG3;
			pp->sloppyclockflag |= in->flags & CLK_FLAG3;          
		}
		if (in->haveflags & CLK_HAVEFLAG4) {
			pp->sloppyclockflag &= ~CLK_FLAG4;
			pp->sloppyclockflag |= in->flags & CLK_FLAG4;          
		}
#ifndef CISCO		
		if (in->flags & CLK_FLAG3)
			(void)refclock_ioctl(pp->io.fd, LDISC_PPS);
#endif /* CISCO */
	}

	/*
	 * Readback requested data
	 */
	if (out != 0) {
		out->haveflags = CLK_HAVETIME1 | CLK_HAVEVAL1 |
		    CLK_HAVEVAL2 | CLK_HAVEFLAG4;
		out->fudgetime1 = pp->fudgetime1;
		out->fudgetime2 = pp->fudgetime2;
		out->fudgeval1 = peer->stratum;
		out->fudgeval2 = pp->refid;
		out->flags = pp->sloppyclockflag;

		out->timereset = current_time - pp->timestarted;
		out->polls = pp->polls;
		out->noresponse = pp->noreply;
		out->badformat = pp->badformat;
		out->baddata = pp->baddata;

		out->lastevent = pp->lastevent;
		out->currentstatus = pp->currentstatus;
		out->type = pp->type;
		out->clockdesc = pp->clockdesc;
		out->lencode = pp->lencode;
		out->lastcode = pp->lastcode;
	}

	/*
	 * Give the stuff to the clock
	 */
	if (refclock_conf && refclock_conf[clktype]->clock_control != NULL)
		(refclock_conf[clktype]->clock_control)(unit, in, out);
}


/*
 * refclock_buginfo - return debugging info
 *
 * This routine is used mainly for debugging. It returns designated
 * values from the interface structure that can be displayed using
 * xntpdc and the clkbug command.
 */
void
refclock_buginfo(srcadr, bug)
	struct sockaddr_in *srcadr; /* clock address */
	struct refclockbug *bug; /* output structure */
{
	struct peer *peer;
	struct refclockproc *pp;
	u_char clktype;
	int unit;
	int i;

	/*
	 * Check for valid address and peer structure
	 */
	if (!ISREFCLOCKADR(srcadr))
		return;
	clktype = REFCLOCKTYPE(srcadr);
	unit = REFCLOCKUNIT(srcadr);
	if (clktype >= NUM_REFCLK_TYPES || unit >= MAXUNIT)
		return;
	if (!(peer = *typeunit_ptr(clktype, unit)))
		return;
	pp = peer->procptr;

	/*
	 * Copy structure values
	 */
	bug->nvalues = 8;
	bug->values[0] = pp->year;
	bug->values[1] = pp->day;
	bug->values[2] = pp->hour;
	bug->values[3] = pp->minute;
	bug->values[4] = pp->second;
	bug->values[5] = pp->msec;
	bug->values[6] = pp->yearstart;
	bug->values[7] = pp->coderecv;

	bug->ntimes = pp->nstages + 3;
	if (bug->ntimes > NCLKBUGTIMES)
		bug->ntimes = NCLKBUGTIMES;
	bug->stimes = 0xfffffffc;
	bug->times[0] = pp->lastref;
	bug->times[1] = pp->lastrec;
	UFPTOLFP(pp->dispersion, &bug->times[2]);
	for (i = 0; i < (int)bug->ntimes; i++)
		bug->times[i + 3] = pp->filter[i];

	/*
	 * Give the stuff to the clock
	 */
	if (refclock_conf && refclock_conf[clktype]->clock_buginfo != NULL)
		(refclock_conf[clktype]->clock_buginfo)(unit, bug);
}

#ifdef CISCO

/*
 * ntp_refclock_timestamp
 *
 * Copy a timestamp into the TTY input buffer.
 *
 * Plays the standard games with the buffer pointers.
 */
void ntp_refclock_timestamp (tt_soc *tty, clock_epoch *timestamp)
{
    int count;
    uchar *ts_ptr;
    
    /* Loop, stuffing in the timestamp. */

    count = sizeof(*timestamp);
    ts_ptr = (uchar *) timestamp;
    while (count--) {
	if (tty->intail >= tty->inend)
	    tty->intail = tty->instart; /* Handle wraps */
	if ((tty->intail == tty->inhead) && tty->incount) {
	    tty->overflow++;
	    break;
	}
	*tty->intail++ = *ts_ptr++;
	++tty->incount;
    }
}

/*
 * ntp_pps_tick
 *
 * Handle the PPS tick from a TTY.
 *
 * Called on the interrupt thread!  Be quick!
 *
 * If we are receiving timestamp data, put the epoch into the refclock
 * structure (so it doesn't stomp on any incoming data).  If we're
 * PPS-only, put it into the receive buffer.
 */
void ntp_pps_tick (tt_soc *tty, clock_epoch *ticktime)
{
    ntp_refclock *refclock;

    if (tty->capabilities2 & NTP_CLOCK_2) { /* Receiving timestamps */
	refclock = tty->tty_refclock;
	if (refclock) {
	    refclock->pps_time = *ticktime;
	}
    } else {
	ntp_refclock_timestamp(tty, ticktime);
    }
}

/*
 * ntp_pps_support_test
 *
 * Return TRUE if the specified port(s) support the specified data set
 * signal for use with PPS.
 */
boolean ntp_pps_support_test (parseinfo *csb, ulong pps_flag)
{
    tt_soc *tty;
    ulong linenum;

    /*
     * Return FALSE if we are being called on an alternate path (the
     * moral equivalent of a zillion TEST_MULTIPLE_FUNCS calls in the
     * parser chain).
     */
    if (csb->visible_ambig.multiple_funcs + csb->hidden_ambig.multiple_funcs)
	return(FALSE);

    /*
     * Run the full range of lines.  Return TRUE only if the bit is
     * supported on all lines.
     */
    for (linenum = csb->line1; linenum < csb->line2; linenum++) {
	tty = MODEMS[linenum];
	if (!(*tty->nvector->vntpsetup)(tty, NTP_SETUP_PPSTEST, pps_flag))
	    return(FALSE);
    }
    return(TRUE);
}


/*
 * ntp_refclock_register
 *
 * Register a reference clock driver.  This is expected to be done at
 * subsystem initialization time.
 */
void ntp_refclock_register (struct refclock *vector, ulong clock_type)
{
    if (clock_type < NUM_REFCLK_TYPES)
	refclock_conf[clock_type] = vector;
}

/*
 * ntp_refclock_present
 *
 * Returns TRUE if a driver for the reference clock is present.
 */
boolean ntp_refclock_present (ulong clock_type)
{
    return (clock_type < NUM_REFCLK_TYPES && refclock_conf &&
	    refclock_conf[clock_type]);
}


/*
 * ntp_any_refclocks_present
 *
 * Returns TRUE if any reference clocks (other than the master mode kludge
 * clock) are linked into the system.
 */
boolean ntp_any_refclocks_present (void)
{
    ulong index;

    if (refclock_conf) {
	for (index = 0; index < NUM_REFCLK_TYPES; index++) {
	    if (refclock_conf[index] && index != REFCLK_MASTER)
		return(TRUE);
	}
    }
    return(FALSE);
}
/*
 * ntp_refclock_init
 *
 * Called at startup time to link in this subsystem.
 */
static void ntp_refclock_init (subsystype *subsys)
{
    /*
     * Register some functions.
     */
    reg_add_ntp_refclock_control(refclock_control, "refclock_control");
    reg_add_ntp_refclock_buginfo(refclock_buginfo, "refclock_buginfo");
    reg_add_ntp_refclock_unpeer(refclock_unpeer, "refclock_unpeer");
    reg_add_ntp_refclock_newpeer(refclock_newpeer, "refclock_newpeer");
    reg_add_ntp_pps_tick(ntp_pps_tick, "ntp_pps_tick");
    reg_add_ntp_refclock_timestamp(ntp_refclock_timestamp,
				   "ntp_refclock_timestamp");
    reg_add_ntp_init_refclock(init_refclock, "init_refclock");
    reg_add_ntp_refclock_present(ntp_refclock_present, "ntp_refclock_present");
    reg_add_ntp_refclock_status(ntp_refclock_status, "ntp_refclock_status");

    /*
     * Register parse chains
     */
    ntp_refclock_chain_init();

    /*
     * Create the refclock pointer array.
     */
    refclock_conf = malloc_named(sizeof(struct refclock *) * NUM_REFCLK_TYPES,
				 "NTP refclock vectors");
}

/*
 * NTP refclock subsystem header
 */

#define NTP_REFCLOCK_MAJVERSION 1
#define NTP_REFCLOCK_MINVERSION 0
#define NTP_REFCLOCK_EDITVERSION  1

SUBSYS_HEADER(ntp_refclock, NTP_REFCLOCK_MAJVERSION, NTP_REFCLOCK_MINVERSION,
	      NTP_REFCLOCK_EDITVERSION, ntp_refclock_init,
	      SUBSYS_CLASS_PROTOCOL, "req: ntp", "seq: ntp");


#endif /* CISCO */
#endif /* REFCLOCK */
