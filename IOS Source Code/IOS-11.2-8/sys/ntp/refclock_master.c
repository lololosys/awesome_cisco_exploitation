/* $Id: refclock_master.c,v 3.1.2.2 1996/08/19 18:55:31 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/refclock_master.c,v $
 *------------------------------------------------------------------
 * Driver for NTP Master mode pseudoclock
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refclock_master.c,v $
 * Revision 3.1.2.2  1996/08/19  18:55:31  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:03:36  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:30  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This "reference clock" driver is used to support the "ntp master"
 * command.  It simply returns the current system clock value when
 * polled.
 */

#define CISCO

#include "master.h"
#include "subsys.h"
#include <string.h>
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ctype.h"


#include "ntp_stdlib.h"

/*
 * Interface definitions
 */
#define	REFID		"LOCL"	/*  reference ID */
#define	DESCRIPTION	"Local Clock"

/*
 * master_start - open the devices and initialize data for processing
 */
static int
master_start(int unit, struct peer *peer)
{
	struct refclockproc *pp;

	pp = peer->procptr;
	pp->io.srcclock = peer;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = sys_precision;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);

	return (1);
}

/*
 * master_poll - called by the transmit procedure
 */
static void
master_poll(int unit, struct peer *peer)
{
	struct refclockproc *pp;
	l_fp offset;

	/* Dummy things up and call the receive routine directly. */

	pp = peer->procptr;
	pp->polls++;
	pp->lasttime = current_time;
	get_systime(&pp->lastrec);
	pp->lastref = pp->lastrec;
	if (sys_leap == LEAP_NOTINSYNC) {
	    pp->leap = 0;
	} else {
	    pp->leap = sys_leap;
	}
	memset(&offset, 0, sizeof(offset));

	refclock_sample(&offset, pp, NSAMPLES, NSAMPLES);
        refclock_receive(peer, &pp->offset, 0, pp->dispersion, &pp->lastref,
	                 &pp->lastrec, pp->leap);
}

/*
 * Transfer vector
 */
static struct refclock refclock_master = {
    master_start,			/* start up driver */
    NULL,				/* shut down driver */
    master_poll,			/* transmit poll message */
    NULL,				/* not used */
    NULL,				/* initialize driver */
    NULL,				/* not used */
    NOFLAGS,				/* not used */
    NULL				/* not used */
};


/*
 * ntp_refclock_master_init
 *
 * Called at subsystem init time to link in.
 */
static void ntp_refclock_master_init (subsystype *subsys)
{
    ntp_refclock_register(&refclock_master, REFCLK_MASTER);
}


/*
 * NTP master refclock subsystem header
 */

#define NTP_REFCLOCK_MASTER_MAJVERSION 1
#define NTP_REFCLOCK_MASTER_MINVERSION 0
#define NTP_REFCLOCK_MASTER_EDITVERSION  1

SUBSYS_HEADER(ntp_refclock_master, NTP_REFCLOCK_MASTER_MAJVERSION,
	      NTP_REFCLOCK_MASTER_MINVERSION,
	      NTP_REFCLOCK_MASTER_EDITVERSION, ntp_refclock_master_init,
	      SUBSYS_CLASS_PROTOCOL, "req: ntp_refclock", "seq: ntp_refclock");
