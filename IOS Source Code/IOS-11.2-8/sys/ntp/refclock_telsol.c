/* $Id: refclock_telsol.c,v 3.1.2.3 1996/08/19 18:55:33 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/refclock_telsol.c,v $
 *------------------------------------------------------------------
 * Driver for Telecom Solutions GPS clock
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refclock_telsol.c,v $
 * Revision 3.1.2.3  1996/08/19  18:55:33  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.2  1996/05/09  14:34:34  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.2.1.8.1  1996/04/27  07:16:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.2.1  1996/04/16  19:03:37  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:31  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "subsys.h"
#include <string.h>
#include "ttysrv.h"
#include "../os/location.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ctype.h"
#include "ntp_stdlib.h"

/*
 * Telecom Solutions driver
 *
 * The Telecom Solutions gizmo supports both a PPS signal and an on-time
 * marker.  The timestamp consists of a string of characters, a CR/LF,
 * and an on-time marker (nominally simultaneous with a PPS pulse).
 *
 * We play some interesting games depending on whether or not the PPS option
 * is configured or not.
 *
 * If PPS is on, we treat the OTM as the first byte of the string, and the
 * LF as the last.  This allows us to receive the timestring well in advance
 * of the PPS pulse.  However, it is rather a pain to wake up a process
 * when the PPS pulse comes in, so we turn the whole thing upside down and
 * pick up the PPS pulse in *advance* of the timestamp;  then when we receive
 * the timestamp we subtract one second from it and feed that time to the
 * filter.  This way the whole process is triggered by the NTP Refclock
 * process, which recognizes the trailing LF and hands the packet off to the
 * main NTP process, which goes back and picks up the previous PPS timestamp
 * and the new timecode and does the Right Thing.
 *
 * If there is no PPS available, we trigger the packet timestamping and
 * forwarding based on the OTM character instead of the linefeed, and
 * use that timestamp.
 */

/*
 * Interface definitions
 */
#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/*  reference ID */
#define	DESCRIPTION	"Telecom Solutions GTI GPS Receiver"

/*
 * Timestamp format
 */
typedef struct telsol_ts_ {
    char comma1;
    char revision;			/* Current protocol revision */
    char comma2;
    char mjd[5];			/* Modified Julian Date */
    char comma3;
    char year[2];			/* Year in century */
    char slash1;
    char month[2];			/* Month in year */
    char slash2;
    char date[2];			/* Date in month */
    char comma4;
    char hour[2];			/* Hour in day */
    char colon1;
    char minute[2];			/* Minute in hour */
    char colon2;
    char second[2];			/* Second in minute */
    char comma5;
    char tzo_sign;			/* Time zone offset sign (+/-) */
    char tzo_int[2];			/* Time zone offset integer hours */
    char period1;
    char tzo_frac;			/* Time zone offset fraction */
    char comma6;
    char leap;				/* Leap second indicator (+/-/0) */
    char comma7;
    char lat_deg[2];			/* Latitude degrees */
    char lat_dir;			/* Latitude direction (N/S) */
    char lat_min_int[2];		/* Latitude integer minutes */
    char period2;
    char lat_min_frac[3];		/* Latitude fractional minutes */
    char comma8;
    char long_deg[3];			/* Longitude degrees */
    char long_dir;			/* Longitude direction (E/W) */
    char long_min_int[2];		/* Longitude integer minutes */
    char period3;
    char long_min_frac[3];		/* Longitude fractional minutes */
    char comma9;
    char alt_sign;			/* Altitude sign (+/-) */
    char altitude[4];			/* Altitude in meters */
    char status[0];			/* Alarms, etc. */
    char otm;				/* On-time marker (don't reference!) */
} telsol_ts;

#define TELSOL_VALID '*'		/* OTM if status is valid */
#define TELSOL_INVALID '!'		/* OTM if status is invalid */
#define TELSOL_REVISION 'A'		/* The version we support */


/*
 * telsol_decode - decode a timestamp field
 *
 * Returns TRUE and the value if the field parsed successfully.
 */
static boolean telsol_decode (char *string, ulong length, int *value)
{
    char ch;

    *value = 0;
    while (length--) {
	ch = *string;
	if (ch < '0' || ch > '9')
	    return(FALSE);
	*value = (*value * 10) + (ch - '0');
	string++;
    }
    return(TRUE);
}


/*
 * telsol_receive - receive data from the serial interface
 *
 * We play a funky game here.  The timestamp delivered here is valid as of
 * the *next* PPS signal, so we subtract a second and deliver the last
 * PPS time as the epoch (after ensuring that the timestamp comes less
 * than a second after the last PPS tick).
 */
static struct recvbuf *telsol_receive (struct recvbuf *rbufp)
{
    struct refclockproc *pp;
    struct peer *peer;
    l_fp trtmp, offset, pps_time;
    telsol_ts *ts;
    int dummy;
    int tz_int, tz_frac;
    int pos_deg, pos_min, pos_frac;
    tt_soc *tty;
    char otm;
    
    /*
     * Initialize pointers and read the receiver response
     */
    peer = rbufp->recv_srcclock;
    pp = peer->procptr;
    pps_time = pp->pps_time;
    *pp->lastcode = '\0';
    pp->lencode = refclock_gtlin(rbufp, pp->lastcode, BMAX, &trtmp);
    
    if (ntp_refclock_debug && ntp_packets_debug) {
	buginf("\nTimestamp %08x.%08x (%CC), PPS time %08x.%08x (%CC)",
	       trtmp.l_ui, trtmp.l_uf, "%B", &trtmp,
	       pps_time.l_ui, pps_time.l_uf, "%B", &pps_time);
	buginf("\nTimecode: '%s'", pp->lastcode);
    }
    
    /* If we're not polled, bail. */
    
    if (pp->pollcnt == NOPOLLS)
	return(rbufp);
    
    /* If we come up short, bail. */
    
    if (pp->lencode < sizeof(telsol_ts)) {
	
	/*
	 * We may be out of sync due to enabling receive interrupts during
	 * a timestamp.  Report it only if we've missed two poll
	 * opportunities.
	 */
	if (pp->pollcnt == 0)
	    refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /*
     * Check the timestamp compared to the PPS timestamp.  The received
     * timestamp must be between 0 and 1 second after the PPS timestamp,
     * or all bets are off.
     */
    if (peer->pps_flags) {
	offset = trtmp;
	L_SUB(&offset, &pps_time);
	if (L_ISNEG(&offset) || offset.l_ui != 0) {
	    refclock_report(peer, CEVNT_NOPPS);
	    return(rbufp);
	}
    }
    
    /*
     * Now parse the packet.  If we're running in PPS mode, the
     * OTM will be the first character;  if not, it will be the last.
     */
    if (peer->pps_flags) {		/* PPS configured */
	ts = (telsol_ts *) (&(pp->lastcode[1]));
	otm = pp->lastcode[0];
    } else {
	ts = (telsol_ts *) pp->lastcode;
	otm = pp->lastcode[pp->lencode-1];
    }
    switch (otm) {
      case TELSOL_VALID:
	pp->leap = LEAP_NOWARNING;
	break;
      case TELSOL_INVALID:
	pp->leap = LEAP_NOTINSYNC;
	break;
      default:
	
	/*
	 * We may be out of sync due to enabling receive interrupts during
	 * a timestamp.  Report it only if we've missed two poll
	 * opportunities.
	 */
	if (pp->pollcnt == 0)
	    refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Now the revision. */
    
    if (ts->revision != TELSOL_REVISION) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Now all the punctuation. */
    
    if (ts->comma1 != ',' ||
	ts->comma2 != ',' ||
	ts->comma3 != ',' ||
	ts->comma4 != ',' ||
	ts->comma5 != ',' ||
	ts->comma6 != ',' ||
	ts->comma7 != ',' ||
	ts->comma8 != ',' ||
	ts->comma9 != ',' ||
	ts->slash1 != '/' ||
	ts->slash2 != '/' ||
	ts->colon1 != ':' ||
	ts->colon2 != ':' ||
	ts->period1 != '.' ||
	ts->period2 != '.' ||
	ts->period3 != '.') {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Now the MJD. */
    
    if (!telsol_decode(ts->mjd, sizeof(ts->mjd), &dummy)) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Now the year. */
    
    if (!telsol_decode(ts->year, sizeof(ts->year), &pp->year)) {
	refclock_report(peer, CEVNT_BADDATE);
	return(rbufp);
    }
    
    /* Push it into the right century. */
    
    if (pp->year >= 36) {		/* The timestamp rolls over in 2036 */
	pp->year += 1900;
    } else {
	pp->year += 2000;
    }
    
    /* Now the month. */
    
    if (!telsol_decode(ts->month, sizeof(ts->month), &pp->month) ||
	pp->month < 1 || pp->month > 12) {
	refclock_report(peer, CEVNT_BADDATE);
	return(rbufp);
    }
    
    /* Now the date. */
    
    if (!telsol_decode(ts->date, sizeof(ts->date), &pp->day) ||
	pp->day < 1 || pp->day > 31) {
	refclock_report(peer, CEVNT_BADDATE);
	return(rbufp);
    }
    
    /* Now the hour. */
    
    if (!telsol_decode(ts->hour, sizeof(ts->hour), &pp->hour) ||
	pp->hour > 23) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    /* Now the minute. */
    
    if (!telsol_decode(ts->minute, sizeof(ts->minute), &pp->minute) ||
	pp->minute > 59) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    /* Now the second. */
    
    if (!telsol_decode(ts->second, sizeof(ts->second), &pp->second) ||
	pp->second > 59) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    /* Now the timezone. */
    
    if (!telsol_decode(ts->tzo_int, sizeof(ts->tzo_int), &tz_int) ||
	tz_int > 23) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    if (!telsol_decode(&ts->tzo_frac, sizeof(ts->tzo_frac), &tz_frac)) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    pp->tz_offset = (tz_int * (60 * 60)) + (tz_frac * (60 * 60 / 10));
    switch (ts->tzo_sign) {
      case '-':
	pp->tz_offset = -pp->tz_offset;
	break;
      case '+':
	break;
      default:
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    /* Now the leap indication. */
    
    switch (ts->leap) {
      case '+':
	pp->leap |= LEAP_ADDSECOND;
	break;
      case '-':
	pp->leap |= LEAP_DELSECOND;
	break;
      case '0':
	break;
      default:
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    /* Next the latitude. */
    
    if (!telsol_decode(ts->lat_deg, sizeof(ts->lat_deg), &pos_deg) ||
	pos_deg > 90) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    if (!telsol_decode(ts->lat_min_int, sizeof(ts->lat_min_int),
		       &pos_min) || pos_min > 59) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    if (!telsol_decode(ts->lat_min_frac, sizeof(ts->lat_min_frac),
		       &pos_frac)) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    pp->latitude =
	(pos_deg * MINUTES_PER_DEGREE * SECONDS_PER_MINUTE *
	 CENTISECS_PER_SECOND) +
	     (pos_min * SECONDS_PER_MINUTE * CENTISECS_PER_SECOND) +
		 (pos_frac * SECONDS_PER_MINUTE * CENTISECS_PER_SECOND / 1000);
    switch (ts->lat_dir) {
      case 'S':
	pp->latitude = -pp->latitude;
	break;
      case 'N':
	break;
      default:
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Next the longiitude. */
    
    if (!telsol_decode(ts->long_deg, sizeof(ts->long_deg), &pos_deg) ||
	pos_deg > 180) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    if (!telsol_decode(ts->long_min_int, sizeof(ts->long_min_int),
		       &pos_min) || pos_min > 59) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    if (!telsol_decode(ts->long_min_frac, sizeof(ts->long_min_frac),
		       &pos_frac)) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    pp->longitude =
	(pos_deg * MINUTES_PER_DEGREE * SECONDS_PER_MINUTE *
	 CENTISECS_PER_SECOND) +
	     (pos_min * SECONDS_PER_MINUTE * CENTISECS_PER_SECOND) +
		 (pos_frac * SECONDS_PER_MINUTE * CENTISECS_PER_SECOND / 1000);
    switch (ts->long_dir) {
      case 'W':
	pp->longitude = -pp->longitude;
	break;
      case 'E':
	break;
      default:
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    /* Lastly, altitude. */
    
    if (!telsol_decode(ts->altitude, sizeof(ts->altitude),
		       &pp->altitude)) {
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    pp->altitude = (pp->altitude * METERS_TO_FEET_NUMERATOR) /
	METERS_TO_FEET_DENOMINATOR;
    
    switch (ts->alt_sign) {
      case '-':
	pp->altitude = -pp->altitude;
	break;
      case '+':
	break;
      default:
	refclock_report(peer, CEVNT_BADREPLY);
	return(rbufp);
    }
    
    pp->lasttime = current_time;
    if (peer->pps_flags) {		/* PPS configured */
	pp->lastrec = pps_time;
    } else {
	pp->lastrec = trtmp;
    }
    
    /* Disable the serial port;  we've got the response. */
    
    tty = MODEMS[peer->refclkunit];
    tty_portdisable(tty);
    pp->pollcnt = NOPOLLS;		/* We just responded. */
    
    /*
     * Process the new sample in the median filter and determine the
     * reference clock offset and dispersion.  This will fill in lastref.
     */
    if (!refclock_process(pp, NSAMPLES, NSAMPLES, (peer->pps_flags != 0))) {
	refclock_report(peer, CEVNT_BADTIME);
	return(rbufp);
    }
    
    if (ntp_refclock_debug) {
	buginf("\nOffset %s, disp %s, reftime %08x.%08x (%CC), leap %d",
	       lfptoa(&pp->offset, 6), ufptoa(pp->dispersion, 6),
	       pp->lastref.l_ui, pp->lastref.l_uf, "%B", &pp->lastref,
	       pp->leap);
    }
    
    refclock_receive(peer, &pp->offset, 0, pp->dispersion, &pp->lastref,
		     &pp->lastrec, pp->leap);
    
    /* If the data is good, set the system location. */
    
    if (pp->leap != LEAP_NOTINSYNC) {
	location_set(pp->longitude, pp->latitude, pp->altitude);
    } else {
	refclock_report(peer, CEVNT_SYNC); /* It's out of sync */
    }
    return(rbufp);
}


/*
 * telsol_poll - called by the transmit procedure
 */
static void telsol_poll(int unit, struct peer *peer)
{
    struct refclockproc *pp;
    tt_soc *tty;
    
    /*
     * Decrement the poll counter to show that we have a poll
     * outstanding.
     */
    pp = peer->procptr;
    if (pp->pollcnt == 0)
	refclock_report(peer, CEVNT_TIMEOUT);
    else
	pp->pollcnt--;
    pp->polls++;
    
    /* Enable the serial port to receive the timestamp. */
    
    tty = MODEMS[peer->refclkunit];
    tty_portenable(tty);
}

/*
 * telsol_start - open the devices and initialize data for processing
 */
static int telsol_start (int unit, struct peer *peer)
{
    struct refclockproc *pp;
    
    pp = peer->procptr;
    pp->io.clock_recv = telsol_receive;
    pp->io.srcclock = peer;
    
    /*
     * Initialize miscellaneous variables
     */
    peer->precision = PRECISION;
    pp->clockdesc = DESCRIPTION;
    memcpy(&pp->refid, REFID, 4);
    
    pp->pollcnt = NOPOLLS;
    return (1);
}


/*
 * telsol_setup
 *
 * Set up TTY parameters and such for this new reference clock.
 */
static void telsol_setup (struct peer *peer)
{
    tt_soc *tty;
    struct refclockproc *pp;

    pp = peer->procptr;
    tty = MODEMS[peer->refclkunit];

    tty->capabilities2 |= NTP_CLOCK_2;

    /*
     * We set things up differently depending on whether we are looking for
     * a PPS signal or not.
     */
    if (peer->pps_flags) {		/* PPS requested */
	tty->ts_char_1 = LINEFEED;
	tty->ts_char_2 = BREAK_CHAR;	/* %%% Really an ignore */
	peer->ttysetupparm = NTP_SETUP_ENABLETIMEPPS;
	tty->capabilities2 |= PPS_CLOCK_2;
    } else {				/* No PPS */
	tty->ts_char_1 = TELSOL_VALID;
	tty->ts_char_2 = TELSOL_INVALID;
	peer->ttysetupparm = NTP_SETUP_ENABLETIME;
    }
    setspeed(tty, 9600, 9600);
    setparity(tty, PARITY_NO);
    setdatabits(tty, 8);
    tty_portdisable(tty);		/* Shut it down for now. */
}


/*
 * Transfer vector
 */
static struct refclock refclock_telsol = {
    telsol_start,			/* start up driver */
    NULL,				/* shut down driver */
    telsol_poll,			/* transmit poll message */
    NULL,				/* not used */
    NULL,				/* initialize driver */
    NULL,				/* not used */
    NOFLAGS,				/* not used */
    telsol_setup			/* Set up TTY parameters */
};


/*
 * ntp_refclock_telsol_init
 *
 * Called at subsystem init time to link in.
 */
static void ntp_refclock_telsol_init (subsystype *subsys)
{
    ntp_refclock_register(&refclock_telsol, REFCLK_TELSOL);
}


/*
 * NTP telsol refclock subsystem header
 */

#define NTP_REFCLOCK_TELSOL_MAJVERSION 1
#define NTP_REFCLOCK_TELSOL_MINVERSION 0
#define NTP_REFCLOCK_TELSOL_EDITVERSION  1

SUBSYS_HEADER(ntp_refclock_telsol, NTP_REFCLOCK_TELSOL_MAJVERSION,
	      NTP_REFCLOCK_TELSOL_MINVERSION,
	      NTP_REFCLOCK_TELSOL_EDITVERSION, ntp_refclock_telsol_init,
	      SUBSYS_CLASS_PROTOCOL, "req: ntp_refclock", "seq: ntp_refclock");
