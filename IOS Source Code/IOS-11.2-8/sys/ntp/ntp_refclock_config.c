/* $Id: ntp_refclock_config.c,v 3.1.2.2 1996/09/04 23:04:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_refclock_config.c,v $
 *------------------------------------------------------------------
 * Config routines for NTP reference clock support
 *
 * July 1996, Dave Katz
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_refclock_config.c,v $
 * Revision 3.1.2.2  1996/09/04  23:04:19  snyder
 * CSCdi68106:  save some space in ntp
 *              80 from image, 104 from data section
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/19  18:55:15  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1  1996/07/28  02:29:22  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define CISCO

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ip/ip.h"
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "parser_defs_ntp.h"
#include "ttysrv.h"
#include "../os/async.h"
#include "../if/atm.h"
#include "../lane/lane.h"



/*
 * ntp_config_refclock_offset
 *
 * Convert integer microseconds to an l_fp and store it.
 */
static void ntp_config_refclock_offset (l_fp *offset, long value)
{
    boolean is_neg;

    is_neg = (value < 0);
    if (is_neg)
	value = -value;
    offset->l_ui = value / USEC_PER_SEC;
    value %= USEC_PER_SEC;
    value *= PIPS_PER_USEC;
    offset->l_uf = value;
    if (is_neg) {
	L_NEG(offset);
    }
}

/*
 * ntp_nv_refclock_offset
 *
 * Generate an NVGEN string for an offset if it is nonzero.
 */
static void ntp_nv_refclock_offset (l_fp *offset, char *keyword)
{
    boolean is_neg;
    l_fp abs_offset;
    int value;
    
    /* Bail if the offset is zero. */

    if (L_ISZERO(offset))
	return;

    /* Take the absolute value. */

    is_neg = L_ISNEG(offset);
    abs_offset = *offset;
    if (is_neg)
	L_NEG(&abs_offset);

    /* Build the value.  First, scale the fraction, then add the integer. */

    value = abs_offset.l_uf;
    value /= PIPS_PER_USEC;
    value += abs_offset.l_ui * USEC_PER_SEC;
    if (is_neg)
	value = -value; /* Restore the sign */
    nv_add(value, " %s %d", keyword, value);
}

static char *const pps_signal_names[NTP_PPS_BITNUM_COUNT] = {"dcd", "rts",
						             "cts", "dtr",
						             "dsr", "ri"};

/*
 * ntp_line_cmd
 *
 * Process NTP line commands
 *
 * OBJ(int,1) = manufacturer/model
 * OBJ(int,2) = PPS offset
 * OBJ(int,3) = PPS signal (NTP_PPS_xxx)
 * OBJ(int,4) = stratum
 * OBJ(int,5) = timestamp offset
 * OBJ(int,6) = NTP_PPS_INVERTED if inverting the timestamp
 *
 * For NV, the reference clock commands come in as "ntp refclock <type> pps",
 * with OBJ(int,1) filled in.
 */
void ntp_line_cmd (parseinfo *csb)
{
    tt_soc *tty;
    ntp_refclock *refclock;
    int ttycount;
    struct peer *peer;
    struct sockaddr_in peer_addr;
    ulong pps_ix;

    /* As usual, NV generation first. */

    if (csb->nvgen) {
	if (!ntp_running)
	    return;

	switch (csb->which) {

	    /* [no] ntp refclock <manufacturer> <model> pps <signal> [inverted]
	     *      [timestamp-offset <microseconds>]
	     *      [pps-offset <microseconds>] [stratum <n>]
	     */
	  case NTP_REFCLOCK_LINE_CMD:
	  case NTP_PPS_DIS_LINE_CMD:
	    for (ttycount = csb->line1; ttycount < csb->line2; ttycount++) {
		tty = MODEMS[ttycount];
		if (tty->capabilities2 & NTP_BITS_2) {

		    /* Got one.  Pull up its data structure. */
		    
		    refclock = tty->tty_refclock;
		    if (!refclock)	/* Weird */
			continue;
		    peer = refclock->refclock_peer;
		    if (!peer)		/* Weird */
			continue;

		    /* Skip if this isn't our clock type. */

		    if (peer->refclktype != GETOBJ(int,1))
			continue;

		    /* Spit out the start of the command. */

		    nv_write(TRUE, "%s ", csb->nv_command);
		    
		    /* Next, add the PPS signal. */

		    if (peer->pps_flags) {
			for (pps_ix = 0; pps_ix < NTP_PPS_BITNUM_COUNT;
			     pps_ix++) {
			    if (peer->pps_flags & (1 << pps_ix)) {
				nv_add(TRUE, pps_signal_names[pps_ix]);
				break;
			    }
			}
		    } else {
			nv_add(TRUE, "none");
		    }
		    
		    /* Add the "inverted" flag as appropriate. */
		    
		    nv_add((peer->pps_flags & NTP_PPS_INVERTED),
			   " inverted");
		    
		    /* Add the offsets as appropriate. */

		    ntp_nv_refclock_offset(&refclock->fudgetime1,
					   "timestamp-offset");
		    ntp_nv_refclock_offset(&refclock->fudgetime2,
					   "pps-offset");
		    
		    /* And finally the stratum. */
		    
		    nv_add(refclock->stratum_specified != NTP_MAXSTRATUM,
			   " stratum %d", refclock->stratum_specified);
		}
	    }
	}
	return;
    }
    init_ntp_process(NULL);

    switch (csb->which) {

      case NTP_REFCLOCK_LINE_CMD:
      case NTP_PPS_DIS_LINE_CMD:

	/* Some fool might configure it across multiple lines.  Why not? */
	
	for (ttycount = csb->line1; ttycount < csb->line2; ttycount++) {
	    if (csb->sense) {
		
		/* Try grabbing the port (but don't allow the console). */
		
		if (ttycount == 0) {
		    tty = NULL;
		} else {
		    tty = tty_allocate(ttycount, TRUE);
		}
		if (!tty) {
		    printf("\n%%NTP: line %d unavailable", ttycount);
		    continue;
		}
		
		/*
		 * See if the port is capable.  It is if the probe
		 * returns TRUE.
		 */
		if (!(*tty->nvector->vntpsetup)(tty, NTP_SETUP_PROBE, 0)) {
		    printf("\n%%NTP: line %d cannot support NTP", ttycount);
		    tty_deallocate(tty);
		    continue;
		}
		
		/* Got it.  Create the peer. */
		
		memset(&peer_addr, 0, sizeof(peer_addr));
		peer_addr.sin_port = NTP_PORT;
		peer_addr.sin_addr.s_addr = REFCLOCK_ADDR |
		    (GETOBJ(int,1) << 8) | ttycount;
		peer = peer_config(&peer_addr, NULL, MODE_ACTIVE,
				   NTP_VERSION, NTP_MINDPOLL,
				   NTP_MAXDPOLL, 0, 0, 0, GETOBJ(int,4));
		if (!peer || !peer->procptr) {
		    printf("\n%%NTP: Couldn't configure reference clock");
		    tty_deallocate(tty);
		    return;
		}
		
		/* Got it.  Set capabilities, and link it all together. */

		tty_savedefaults(tty);
		refclock = peer->procptr;
		tty->tty_refclock = refclock;
		peer->pps_flags = GETOBJ(int,3) | GETOBJ(int,6);
		if (GETOBJ(int,7))
		    refclock->stratum_specified = GETOBJ(int,4);
		else
		    refclock->stratum_specified = NTP_MAXSTRATUM;
		ntp_config_refclock_offset(&refclock->fudgetime1,
					   GETOBJ(int,5));
		ntp_config_refclock_offset(&refclock->fudgetime2,
					   GETOBJ(int,2));
		
		/* Launch the process to gather the bytes. */
		
		tty->tiptop = cfork ((forkproc *) ntp_async_process,
				     (long) tty, 2000, "NTP refclock",
				     CONSOLE_LINENUM);
		if (tty->tiptop == NO_PROCESS) { /* Uhoh */
		    tty_reset(tty);
		    tty_deallocate(tty);
		    unpeer(peer);
		    printf("\n%%NTP: Couldn't create process");
		    return;
		}

		/* Call the driver to finish setting up the TTY port. */

		if (refclock_conf)
		    (*(refclock_conf[peer->refclktype]->clock_setup))(peer);
		clearinput(tty);

		/* Kick the hardware. */

		(*tty->nvector->vntpsetup)(tty, peer->ttysetupparm,
					   GETOBJ(int,3));
		tty->capabilities &= ~(MODEMMASK|HFLOWOUT|SFLOWOUT);

	    } else {			/* "no" */

		tty = MODEMS[ttycount];
		if (tty->capabilities2 & NTP_BITS_2) {

		    /* It's there. */

		    (*tty->nvector->vntpsetup)(tty, NTP_SETUP_DISABLE, 0);
		    refclock = tty->tty_refclock;
		    tty->tty_refclock = NULL;
		    process_kill(tty->tiptop); /* Wipe out the process */
		    tty->tiptop = NO_PROCESS;
		    tty_reset(tty);	/* Clean up */
		    tty_deallocate(tty); /* Ditto */
		    tty_portenable(tty); /* Let it run */
		    if (refclock) {	/* Better be here! */
			peer = refclock->refclock_peer;
			unpeer(peer);
		    }
		} else {
		    printf("\n%%NTP: No reference clock on line %d", ttycount);
		}
	    }
	}
    }
}
