/* $Id: refclock_pps.c,v 3.1.2.1 1996/08/19 18:55:32 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/refclock_pps.c,v $
 *------------------------------------------------------------------
 * NTP driver for PPS pulse with no timecode
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refclock_pps.c,v $
 * Revision 3.1.2.1  1996/08/19  18:55:32  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1  1996/06/23  00:23:53  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define CISCO

#include "master.h"
#include "subsys.h"
#include <string.h>
#include "ttysrv.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ctype.h"
#include "ntp_stdlib.h"


/*
 * PPS driver
 *
 * This driver calculates the offset between the current system time and
 * the pulse time, but only if the clock has been set somehow.  The
 * implication is that the clock has been set +- 0.5 seconds.
 */

/*
 * Interface definitions
 */
#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"PPS\0"	/*  reference ID */
#define	DESCRIPTION	"Pulse-per-second discipline"



/*
 * pps_receive - handle the incoming PPS pulse
 *
 * We receive a buffer with nothing but the timestamp.
 */
static struct recvbuf *pps_receive (struct recvbuf *rbufp)
{
    struct refclockproc *pp;
    struct peer *peer;
    l_fp pps_time, target_time;
    tt_soc *tty;
    
    /*
     * Initialize pointers and read the receiver response
     */
    peer = rbufp->recv_srcclock;
    pp = peer->procptr;
    *pp->lastcode = '\0';
    pp->lencode = refclock_gtlin(rbufp, pp->lastcode, BMAX, &pps_time);

    /* Disable the serial port;  we've got the response. */
    
    tty = MODEMS[peer->refclkunit];
    tty_portdisable(tty);
    pp->pollcnt = NOPOLLS;		/* We just responded. */

    /* Set the leap code depending on whether we have good time. */

    if (clock_is_probably_valid()) {
	pp->leap = LEAP_NOWARNING;
    } else {
	pp->leap = LEAP_NOTINSYNC;
    }

    pp->lasttime = current_time;
    pp->lastrec = pps_time;

    /*
     * Figure out what time the interrupt *should* have happened based
     * on the timestamp (round down or up to the next whole second).
     */
    target_time = pps_time;
    if (target_time.l_uf >= 0x80000000) { /* Round up to next second */
	target_time.l_ui++;
    }
    target_time.l_uf = 0;

    /*
     * Adjust the stratum to equal the current system stratum if the
     * system stratum is better.
     */
    if ((pp->stratum_specified == NTP_MAXSTRATUM) &&
	peer->stratum > (sys_stratum - 1)) {
	peer->stratum = sys_stratum - 1;
    }
    
    /*
     * Process the new sample in the median filter and determine the
     * reference clock offset and dispersion.  This will fill in lastref.
     */
    if (!refclock_new_sample(pp, NSAMPLES, NSAMPLES, &target_time)) {
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
    
    return(rbufp);
}
    

/*
 * pps_poll - called by the transmit procedure
 */
static void pps_poll(int unit, struct peer *peer)
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
    
    /* Enable the serial port to receive the PPS pulse. */
    
    tty = MODEMS[peer->refclkunit];
    tty_portenable(tty);
}


/*
 * pps_start - open the devices and initialize data for processing
 */
static int pps_start (int unit, struct peer *peer)
{
    struct refclockproc *pp;
    
    pp = peer->procptr;
    pp->io.clock_recv = pps_receive;
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
 * pps_setup
 *
 * Set up TTY parameters and such for this new reference clock.
 */
static void pps_setup (struct peer *peer)
{
    tt_soc *tty;
    struct refclockproc *pp;

    pp = peer->procptr;
    tty = MODEMS[peer->refclkunit];

    peer->ttysetupparm = NTP_SETUP_ENABLEPPS;
    tty->capabilities2 |= PPS_CLOCK_2 | RCV_SUPPR_CLOCK_2;
    tty_portdisable(tty);		/* Shut it down for now. */
    clearinput(tty);			/* Clean up any loose data. */
}

/*
 * Transfer vector
 */
static struct refclock refclock_pps = {
    pps_start,				/* start up driver */
    NULL,				/* shut down driver */
    pps_poll,				/* transmit poll message */
    NULL,				/* not used */
    NULL,				/* initialize driver */
    NULL,				/* not used */
    NOFLAGS,				/* not used */
    pps_setup				/* Set up TTY parameters */
};


/*
 * ntp_refclock_pps_init
 *
 * Called at subsystem init time to link in.
 */
static void ntp_refclock_pps_init (subsystype *subsys)
{
    ntp_refclock_register(&refclock_pps, REFCLK_ATOM_PPS);
}


/*
 * NTP pps refclock subsystem header
 */

#define NTP_REFCLOCK_PPS_MAJVERSION 1
#define NTP_REFCLOCK_PPS_MINVERSION 0
#define NTP_REFCLOCK_PPS_EDITVERSION  1

SUBSYS_HEADER(ntp_refclock_pps, NTP_REFCLOCK_PPS_MAJVERSION,
	      NTP_REFCLOCK_PPS_MINVERSION,
	      NTP_REFCLOCK_PPS_EDITVERSION, ntp_refclock_pps_init,
	      SUBSYS_CLASS_PROTOCOL, "req: ntp_refclock", "seq: ntp_refclock");
