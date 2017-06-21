/* $Id: modemsupport.c,v 3.8.18.5 1996/08/08 17:56:31 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/ts/modemsupport.c,v $
 *------------------------------------------------------------------
 * modemsupport.c - Modem control routines
 *
 * February 24, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains routines specific to modem control on async
 * interfaces.
 *------------------------------------------------------------------
 * $Log: modemsupport.c,v $
 * Revision 3.8.18.5  1996/08/08  17:56:31  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Modem locking speed is now configureable which prevents changing of the
 * desired line speed. Configuration is also attempted after every hangup.
 *
 * Revision 3.8.18.4  1996/07/04  00:29:05  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.8.18.3  1996/04/08  20:11:37  syiu
 * CSCdi53895:  Async Callback broken
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/03/27  12:25:28  irfan
 * CSCdi48100:  bus error on restart (bcopy6)
 * Branch: California_branch
 * Code changes to avoid double execs on the same tty: Don't answer
 * new incoming calls while previous process/framing is not cleared
 * up fully. Avoid dropping the line twice when an async interface
 * resets. Never return from kfork(). In effect background
 * Modem process should develop better relationship with foreground
 * process/framing.
 *
 * Revision 3.8.18.1  1996/03/18  22:20:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.2  1996/03/09  05:13:51  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.12.1  1996/02/20  21:24:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/11  09:21:09  billw
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 *
 * Revision 3.7  1996/02/03  04:13:14  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.6  1995/12/22  21:58:53  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.5  1995/12/09  01:06:37  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Include forgotten invocation of modem_override.
 *
 * Revision 3.4  1995/11/20  10:21:22  billw
 * CSCdi42491:  Transition modem ri-is-cd to modem dialin
 *
 * Revision 3.3  1995/11/17  18:55:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:39:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:33:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/12  20:12:13  vandys
 * CSCdi32952:  user wants DTR to drop when dedicated async interface is
 * shut down
 *
 * Revision 2.3  1995/06/09  23:40:33  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.2  1995/06/09  13:22:04  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:09:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../ui/debug.h"
#include "../os/async.h"
#include "../os/os_debug.h"
#include "logger.h"
#include "config.h"
#include "../snmp/sr_rs232mibdefs.h"
#include "interface_private.h"
#include "../parser/parser_defs_config.h"
#include "../ts/modemcap_public.h"

/* Used so often, convenient to write this way */
#define MDMDBG if (modem_debug) buginf

/* Starting autobaud speed */
#define AUTOSPEED (2400)	/* For standard autobaud */
#define AUTOSPEEDHIGH (19200)	/*  ...for reliable 19.2K autobaud */


/*
 * Local data
 */

int modem_firstline;		/* Minimum line number with modem control */
int modem_lastline;		/* Max line + 1 with modem control */
long fastscan_delta = 0;	/* how often to scan fast modems. */
long fastscan_fork = 0;		/* Fork that is scanning modems */


/* How long to wait for autobaud activity to complete */
const int autobaud_retry = (1*60*4);	/* 1 minute, at 1/4 sec per try */

/*
 * async_autosetspeed()
 *	Set speed for autobaud on given line
 */
void
async_autosetspeed(tt_soc *tty)
{
    if (tty->capabilities & AUTOHIGH) {
	(void)setspeed(tty, AUTOSPEEDHIGH, AUTOSPEEDHIGH);
    } else {
	(void)setspeed(tty, AUTOSPEED, AUTOSPEED);
    }
}

/*
 * tty_startexec()
 *	Completed successful autobaud, start an exec
 */
void tty_startexec(tt_soc *tty)
{
    tty->statbits |= READY;
    tty->statbits &= ~AUTOBAUDING;
    MDMDBG("\nTTY%t: Autobaud speed verified, exiting", tty->ttynum);
    tty_clearbuffers(tty);	/* clear type-ahead */
    start_process(tty);		/* start an EXEC */
    process_kill (-1);
}

/*
 * auto_verify()
 *	Verify selected baud rate by reading a '\r'
 *
 * '\r' on success, failing character on mismatch, -1 on timeout.
 */
int auto_verify(tt_soc *tty)
{
    int ch, vertries;
    const int maxverify = (5*4);	/* 5 seconds, at 1/4 sec per try */

    /*
     * Try iteratively for the incoming data.  We keep
     * trying until we receive data or we time out.
     */
    vertries = maxverify;
    tty_clearbuffers(tty);
    while ((ch = getc(tty)) < 0 && vertries-- >= 0) {
	process_sleep_for(ONESEC/4);
    }
    MDMDBG("\nTTY%t: verify received 0x%x", tty->ttynum, ch);

    /*
     * Return character or timeout
     */
    return(ch);
}

/*
 * try_19200()
 *	For AUTOHIGH, try an initial read at 19200
 *
 * Returns 1 if we successfully read three CR's, 0 if we saw
 * data from some other baud rate.  This is all needed because 19.2K
 * is not successfully detected when the UART is set to lower baud
 * rates.
 */
int try_19200(tt_soc *tty)
{
    int ch, tries;

    /*
     * Save previously received character if any as receive buffers
     * will be cleaned up by setspeed
     */
    ch = tty->isavedchar;

    /*
     * Get set for 19.2K test read
     */
    MDMDBG("\nTTY%t: try 19.2K", tty->ttynum);
    setspeed(tty, 19200, 19200);
    tty_portenable(tty);

    /*
     * Loop getting characters or waiting for them to arrive
     */
    for (tries = 0; tries < autobaud_retry; ++tries) {
	/*
	 * On true characters, see if we recognize them.  If not, return
	 * failure and allow the usual autobaud scan start up.
	 */
	if (ch != -1) {
	    tty->isavedchar = -1;
	} else {
	    ch = getc(tty);
	}
	if (ch >= 0) {
	    /*
	     * A CR means we're likely at the right speed. For autobaud return 
	     * success else get two verifying reads for autobaud high, and 
	     * and return success.
	     */
	    if (ch == '\r') {
                if (tty->capabilities & AUTOHIGH) {
		    if ((auto_verify(tty) & 0x7F) != '\r')
		        return(0);
		    if ((auto_verify(tty) & 0x7F) != '\r')
		        return(0);
		    MDMDBG("\nTTY%t: 19.2K detected", tty->ttynum);
		    return(1);
		} else {
                    MDMDBG("\nTTY%t: 19.2K detected", tty->ttynum);
                    return(1);
                }
	    }

	    /*
	     * Otherwise it would appear to be another baud rate.  Drop
	     * out to the regular algorithm.
	     */
	    return(0);
	}

	/*
	 * Not a "true" character.  Pause a 1/4 second.  Bomb if
	 * carrier dropped in the interim.
	 */
	process_sleep_for(ONESEC/4);
	if (tty->statbits & CARDROP) {
	    return(0);
	}
    }
    return(0);
}

/*
 * async_autobaud
 * Automatically determine line speed from 300 to 19,200 baud.
 * Detect character is RETURN.
 */
forktype
async_autobaud(tt_soc *tty)
{
    static const uchar
	abmask[] =	{ 0x7E, 0x70, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F },
	abchar[] =	{ 0x7E, 0x70, 0x0D, 0x3b, 0x66, 0x80, 0x0  };
    static const uint
	abspeed[] =	{ 9600, 4800, 2400, 1800, 1200, 600,  300, -1 };
    uint speed = -1;
    int ch, i, retries = autobaud_retry;

    MDMDBG("\nTTY%t: Autobaud started", tty->ttynum);

    /*
     * Try 19.2K first if it's configured.
     */
    if (tty->capabilities & AUTOHIGH) {
	if (try_19200(tty)) {
	    tty_startexec(tty);
	    return;
	}

	/*
	 * The wrong baud rate may well have left further cruft in
	 * our buffer.  Clear it out so the regular autobaud code
	 * can start cleanly.
	 */
	tty_clearbuffers(tty);
    }

    /*
     * Kick off regular autobaud
     */
    setspeed (tty, AUTOSPEED, AUTOSPEED);
    tty_portenable(tty);

    for (;;) {
	ch = getc(tty);			/* get character from the terminal */
	if (ch >= 0) {
	    MDMDBG("\n\nTTY%t: AUTOBAUD saw %#x at %u",tty->ttynum,
		    ch, AUTOSPEED);
	    for (i = 0; i < sizeof(abmask); i++)
		if ((ch & abmask[i]) == abchar[i])
		    break;
	    speed = abspeed[i];
	    if (speed != -1) {
try19:
		if (!setspeed (tty, speed, speed)) {
		    MDMDBG("\nTTY%t: autobaud failed setting speed %d",
			     tty->ttynum, speed);
		    setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}
		MDMDBG("\nTTY%t: autobaud line speed is %d baud?", 
			tty->ttynum, speed);
		ch = auto_verify(tty);
		if (ch < 0) {
		    MDMDBG("\nTTY%t: Autobaud speed verify timeout",
			     tty->ttynum);
		    (void)setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}

                /* Check for 19200, 14400 and 1800 and 600 speeds */

                if (((speed == 1800) && ((ch & 0x100) == 0x100)) ||
                    ((speed == 300) && ((ch & 0x7f) == 0x72)) ||
                    ((speed == 4800) && ((ch & 0xff) == 0x80))) {
                        speed = 600;
                        goto try19;
                }

                if (speed == 9600) {
                    switch (ch & 0x17f) {
                        case 0x072: speed = 19200;
                                    goto try19;
                        case 0x062: speed = 14400;
                                    goto try19;
                        case 0x100: speed = 1800;
                                    goto try19;
                    }
                }

		if ((ch == 015) || (ch == 0215)) {
		    tty_startexec(tty);
		} else {
		    MDMDBG("\nTTY%t: Autobaud verify failed (saw %#x), "
			"retrying", tty->ttynum, ch);
		    setspeed (tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);	
		    speed = -1;
		    continue;
		}
	    }
	}
	process_sleep_for(ONESEC >> 2);

	/*
	 * Either time out or give up if we were hung up (we are idle)
	 */
	if ((--retries <= 0) || (tty->statbits & CARDROP)) {
	    if (modem_debug) {
		if (retries <= 0)
		  ttybug(tty, "Autobaud timed out");
		if (tty->statbits & CARDROP)
		  ttybug(tty, "Autobaud line hung up");
	    }
	    death(tty);

/*
 * Should never get here.  For some reason the call to death() has returned.
 * Note it and kill it good.
 */

	    ttybug(tty, "Call to death() in autobaud() returned.");
	    process_kill(THIS_PROCESS);
	}
    }
}

/*
 * modem_startprocess
 * Start a process (EXEC or SETSPEED) on a modem line.
 */

static boolean modem_startprocess (
    tt_soc *tty)
{
    /*
     * Prevent exec startyup if line is already active
     */
    if (tty->statbits & HASPROC)
       return(FALSE);

    /*
     * If callback timer has expired, don't allow access 
     */
    if (reg_invoke_callback_expired_tty(tty, TRUE)){
       return(FALSE);
    }

    tty->statbits &= ~(HOLDING|WANTXON);	/* clear any flowcontrol */
    if (tty->capabilities & AUTOBAUD) {
	if (tty->statbits & AUTOBAUDING)
	    return(TRUE);
	tty->tiptop = cfork((forkproc *)auto_baud(tty),(long)tty,0,"SetSpeed",tty->ttynum);
	if (tty->tiptop > 0) {
	    tty->statbits |= (AUTOBAUDING | READY);
	    tty->isavedchar = -1;
	    process_set_crashblock(tty->tiptop, TRUE);
	    return(TRUE);

	} else
	    return(FALSE);
    }
    if (!(tty->capabilities & NO_FLUSH_ACTIVATE)) {
	clearinput(tty);		/* Clear any old input */
    }
    tty_portenable(tty);
    tty->statbits |= READY;
    start_process(tty);
    return(TRUE);
}

/*
 * modem_idle
 * See if someone is calling us.  We ignore RING transitions for a few
 * scheduler cycles to avoid raising DTR when RING drops on a modem line.
 */

static void modem_idle (
    register tt_soc *tty,
    int RING,
    int CTS)
{
    hwidbtype *idb;


    /*
     * we shouldn't be answering any new calls, if any process or
     * framing running on the line is still active. An example would
     * be an async idb with a long pulse-timer. Because of the delayed
     * reset of async idb's we can end up starting a new process on
     * the line. This new process has a miserable life trying to live
     * with the next process started (because async_reset will make the
     * line available again).
     * Delaying DTR enabling only, may still cause problems if modem
     * on the line, can indicate new call, despite our low DTR.
     */
    if (tty->statbits & (HASPROC | SLIPMODE | AUTOBAUDING)) {
	if (modem_debug) {
	    ttybug(tty, "cleanup pending. Delaying DTR");
	}
	if (tty->answercountdown > 1) {  /* Leave atleast a sec */
	    tty->answercountdown--;
	}
	return;
    }

    if (tty->answercountdown) {
	/*
	 * If this is a dedicated mode interface which is in shutdown
	 * mode, hold modem DTR low so we don't answer calls which we
	 * won't handle anyway.
	 */
	if ((tty->capabilities & (MAYSLIP | MUSTSLIP)) == MUSTSLIP) {
	    idb = tty->tty_idb;
	    if (idb && (idb->state == IDBS_ADMINDOWN)) {
		return;
	    }
	}
	if (--tty->answercountdown == 0) {
	    if (tty->capabilities & (MODEMCTS|MODEMRI)) {
		if (modem_debug)
		    ttybug(tty, "restoring DTR");
		tty_dtrstate(tty, TRUE);
	    }
	}
	return;
    }

    if ((tty->statbits2 & CTS_MODEM_DETECT_2) != CTS) {
	if (!CTS) {
	    if (modem_debug)
		ttybug(tty, "CTS went down on IDLE line");
            tty->statbits2 &= ~CTS_MODEM_DETECT_2;
	} else {
	    if (modem_debug)
		ttybug(tty, "CTS came up on IDLE line");
	    tty->statbits &= ~MODEM_DETECTED;
	    tty->statbits2 &= ~MODEM_CONFIGURED_2;
            tty->statbits2 |= CTS_MODEM_DETECT_2;
	    /* 
	     * If we're in discovery mode, clear whatever modem we thought
	     *	we had.
	     */
	    if (tty->capabilities2 & MA_DISCOVERY_2) {
		free(tty->modem_name);
		tty->modem_name = NULL;
	    }
	}
    }

    if (!(tty->statbits2 & MODEM_CONFIGURED_2) && 
		CLOCK_OUTSIDE_INTERVAL(tty->modem_checked, 10*ONESEC)) {
        if (reg_invoke_modem_autoconfigure(tty) == FALSE) {
                reg_invoke_modem_hardware_override(tty);
        }
        GET_TIMESTAMP(tty->modem_checked);

	/*  
	 *  after we get done autoconfing, let the modem hardware
	 *  know that the line is freed up again
	 */
	return;
    }

    switch (tty->capabilities & MODEMMASK) {
	case MODEMIN:
	    if (RING) {
		if (modem_debug)
		    ttybug(tty, "saw DSR, raising DTR");
		tty_dtrstate(tty, TRUE);
		tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
		tty->statbits |= RINGIN;	 /* say we are ringing */
		tty->answercountdown = tty->answertimeout; /* init timeout */
	    }
	    break;

	case MODEMCTS:
	    if (CTS) {
		if (modem_debug) {
		    if (tty->modem_type > MODEMTYPE_LOCAL)
			ttybug(tty, "DSR/DCD came up");
		    else
			ttybug(tty,"CTS came up");
		}			
		tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
		tty->statbits |= READY;
		tty_portenable(tty);
		reg_invoke_line_handoff();
	    }
	    break;

	case MODEMINOUT:
	case MODEMRI:
	    if (RING) {
		if (modem_debug)
		    ttybug(tty,"DSR came up");
	    	tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
		if (!modem_startprocess(tty))
		    tty->statbits |= HANGUP;
	    }
	    break;

	case MODEMOUT:
	    break;

	default:
	    if (modem_debug)
		ttybug(tty,"unknown state");
	    break;
    }
    if (modem_debug && !(tty->statbits & IDLE))
	buginf("\ntty%t: Modem: IDLE->%s", tty->ttynum, modem_statestring(tty));
}

/*
 * modem_ringing
 * Wait for CTS to come up or timeout
 */

static void modem_ringing (
    register tt_soc *tty,
    int CTS)
{
    if (CTS) {
	tty->statbits &= ~RINGIN;
	if (modem_debug) {
	    if (tty->modem_type > MODEMTYPE_LOCAL)
		ttybug(tty, "DSR/DCD came up");
	    else
		ttybug(tty,"CTS came up");
	}			
	if (tty->capabilities & MODEMOUT) {
	    tty_portenable(tty);	/* Enable RX, TX */
	    tty->statbits |= READY;
	    if (modem_debug)
		ttybug(tty, "Modem: RINGING->READY");
	    return;
	}
	tty_if_reset(tty);	/* make sure SLIP is off */
	if (!modem_startprocess(tty)) {
	    tty->statbits |= IDLE;
	    tty_portdisable(tty);
	    tty_dtrstate(tty, FALSE);
	    (*(tty->nvector->vclearoutput))(tty);
	    (*(tty->nvector->vclearinput))(tty);
	}
	if (modem_debug && !(tty->statbits & RINGIN))
	    buginf("\ntty%t: Modem: RINGING->%s", tty->ttynum, modem_statestring(tty));
	return;
    }
    if (tty->answercountdown)
	tty->answercountdown--;
    if (tty->answercountdown == 0) {
	if (modem_debug)
	    ttybug(tty, "timed out waiting for CTS");
	tty->statbits &= ~RINGIN;
	tty->statbits |= IDLE;
	tty_portdisable(tty);
	tty_dtrstate(tty, FALSE);
        tty->answercountdown = RINGWAIT;
	(*(tty->nvector->vclearoutput))(tty);
	(*(tty->nvector->vclearinput))(tty);

    }
    if (modem_debug && !(tty->statbits & RINGIN))
	buginf("\ntty%t: Modem: RINGING->%s", tty->ttynum, modem_statestring(tty));
}

/*
 * modem_hangup
 * Tiptop dead or modem was lost.  We begin a period of ignoring RING
 * transitions so that we don't raise DTR on a RING downward transition.
 */

static void modem_hangup (
    register tt_soc *tty)
{
    if (modem_debug)
	ttybug(tty, "dropping DTR, hanging up");
    tty_portdisable(tty);
    tty_dtrstate(tty, FALSE);
/*
 *  Call to tty_if_reset must be before setting up IDLE only, as
 *  tty_if_reset can itself set up HANGUP to request a call to
 *  modem_hangup when it beleives that it was called due to
 *  serial_pulsetimer going off...race-condition with modem_scanner
 */
    tty_if_reset(tty);
    tty->statbits &= ~(HANGUP + READY + RINGIN + CALLBACK_IN_PROGRESS);
    tty->statbits |= IDLE;
    tty->statbits2 &= ~MODEM_CONFIGURED_2;
    logger_ttydel(tty);			/* remove from logger queue */
    tty->answercountdown = RINGWAIT;
    (*(tty->nvector->vclearoutput))(tty);
    (*(tty->nvector->vclearinput))(tty);
    if (modem_debug && !(tty->statbits & HANGUP))
	buginf("\ntty%t: Modem: HANGUP->%s", tty->ttynum, modem_statestring(tty));
    /* 
     *  We know the modem is now IDLE, and DTR is low so we can't be 
     *   interrupted by a user calling.... now would be a good time for
     *   any modem cards to do their thing.
     */
    reg_invoke_modem_hardware_override(tty);

}

/*
 * modem_ready
 * The modem is working.  Check for hangups.
 */

static void modem_ready (
    register tt_soc *tty,
    int RING,
    int CTS)
{
    switch (tty->capabilities & MODEMMASK) {
	case MODEMIN:
	case MODEMOUT:
	case MODEMCTS:
	    if (!CTS) {
		if (modem_debug) {
		    if (tty->modem_type > MODEMTYPE_LOCAL &&
			((tty->capabilities & MODEMMASK) != MODEMIN))
			ttybug(tty, "DSR/DCD was dropped");
		    else
			ttybug(tty,"CTS was dropped");
		}			
		tty->statbits &= ~READY;	/* ready off */
		(*(tty->nvector->vclearoutput))(tty);
		(*(tty->nvector->vclearinput))(tty);
		if ((tty->statbits & (HASPROC|AUTOBAUDING)) &&
			!(tty->statbits & SLIPMODE)) {
		    tty->statbits |= CARDROP;
		    /*
		     * Drop DTR immediately to prevent modem from answering
		     * another call while we clean up connections, etc.
		     */
		    dtrstate(tty,FALSE);
		    if (modem_debug)
			ttybug(tty, "Quickly dropping DTR");
		}
		else				/* no process. Hang up here */
		    tty->statbits |= HANGUP;
	    }
	    break;

	case MODEMINOUT:
	    if (tty->conntop || (tty->statbits & DIALOUT)) 
					/* Outgoing connection ? */
		break;			/*  outgoing modem, dont drop */
	    /* Fall through */		/* no, must be a phone call. */

	    /*
	     * Note: It would be really nice if we noticed that DSR was
	     * dropped on an outgoing connection *if it was ever up*.
	     * Otherwise, we will not notice if the connection goes away.
	     */

	case MODEMRI:
	    if (!RING) {
		if (modem_debug)
		    ttybug(tty, "DSR was dropped");
		tty->statbits &= ~READY;	/* ready off */
		(*(tty->nvector->vclearoutput))(tty);
		(*(tty->nvector->vclearinput))(tty);
		if ((tty->statbits & (HASPROC|AUTOBAUDING)) &&
			!(tty->statbits & SLIPMODE)) {
		    tty->statbits |= CARDROP;
		    /*
		     * Drop DTR immediately to prevent modem from answering
		     * another call while we clean up connections, etc.
		     */
		    dtrstate(tty,FALSE);
		    if (modem_debug)
			ttybug(tty, "Quickly dropping DTR");
		} else				/* no process. Hang up here */
		    tty->statbits |= HANGUP;
	    }
	    break;

	default:
	    if (modem_debug)
		ttybug(tty,"unknown state");
    }
    if (modem_debug && !(tty->statbits & READY))
	buginf("\ntty%t: Modem: READY->%s", tty->ttynum, modem_statestring(tty));
}

/*
 * modem_scanner - scan modem lines to see what is going on
 *
 * We watch modems and lines for which CTS must be high.
 */

void modem_scanner (void)
{
    int line;
    tt_soc *tty;
    modem_status current;
    modem_status changed;

    for (line = modem_firstline; line < modem_lastline; line++) {
	tty = MODEMS[line];
	if (!tty)
	  continue;

	/*
	 *  If we're in the process of autoconfiguring, the modem
	 *    signals may be doing strange things.  Ignore them.
	 */
	if (tty->statbits2 & MODEM_AUTOCONFING_2)
	  continue;

	if (tty->capabilities & MODEMMASK) {

	    current = tty_modemsignal(tty); /* Get current signals */
	    changed = tty_modemchange(tty); /* Get recent changes */

	    if (tty->modem_type == MODEMTYPE_HOST ||
		tty->modem_type == MODEMTYPE_PRINTER) {
		/*
		 * for new types of modem control, look at DSR instead of
		 * CTS signal.  This allows concurrent HW flow control,
		 * and (usually) unmodified cables on cisco-500 and 25xx.
		 */
		current.cts = current.ring;
		changed.cts = changed.ring;
	    }
	    /*
	     * On modem callin line, we want to check for transtions on the
	     * RING input (the phone rang)  Otherwise, use the actual state
	     */
	    
	    switch (tty->statbits & (IDLE + RINGIN + HANGUP + READY)) {
	      case IDLE: 
		modem_idle(tty,
			   tty->capabilities & MODEMIN ?
			   changed.ring : current.ring,
			   current.cts);
		break;

	      case RINGIN:
		modem_ringing(tty, current.cts);
		break;

	      case HANGUP:
		modem_hangup(tty);
		break;

	      case READY: 
		modem_ready(tty,
			    tty->capabilities & MODEMIN ?
			    changed.ring : current.ring,
			    tty->capabilities & MODEMOUT ?
			    (!changed.cts) : current.cts);
		break;

	      case 0:
		/*
		 * when we have signalled CARDROP to the user process,
		 * It can take some time before the phone is finally
		 * hung up.  Just wait...
		 */
		if ((tty->statbits & CARDROP) &&
		    (++tty->answercountdown < 50))
		  break;
		tty->answercountdown = 0;
		/* else fall though... */
	      default:
		if (modem_debug)
		  buginf("\nTTY%t: Funny modem state %lx", tty->ttynum,
			 tty->statbits);
		break;
	    }
	}
    }
}


/*
 * modemline - Set the modem status of a line
 *
 * If sense is TRUE, enable modem control; if false, disable it
 */

void modemline (
     tt_soc *tty,
     boolean sense)
{
    /*
     * Ensure that this line supports modem control
     */

    if (tty->modem_type == MODEMTYPE_NONE)
      return;

    /*
     * Update the minimum and maximum line number for scanner
     */

    if (sense) {

	/*
	 * Adding a line, update the minimum and maximum
	 */

	if ((tty->ttynum < modem_firstline) | !modem_lastline)
	  modem_firstline = tty->ttynum;
	if (tty->ttynum >= modem_lastline)
	  modem_lastline = tty->ttynum + 1;

    } else {
    
	/*
	 * Removing a line; find a new base.
	 *
	 * First, check to see if this is the first line with modem control.
	 */

	if (tty->ttynum == modem_firstline) {

	    /*
	     * If this is both the first and last line with modem
	     * control, then we no longer have any modem lines to check.
	     */

	    if ((tty->ttynum + 1) == modem_lastline) {
		modem_firstline = 0;
		modem_lastline = 0;
	    } else {

		/*
		 * This is not the last line, so update the first modem
		 * control line.
		 */

		while (TRUE) {
		    modem_firstline++;
		    /*
		     * make sure we don't get past lastline CSCdi24499
		     * If we don't find any modem controlled line
		     *   then set both first and last to 0
		     */
		    if (modem_firstline == modem_lastline) {
			modem_firstline = 0;
			modem_lastline = 0;
			break;
		    }
		    if (MODEMS[modem_firstline] &&
			MODEMS[modem_firstline]->capabilities &
			(MODEMMASK|MODEMDTR))
			break;	/* Get out if we find a modem line */
		}
	    }
	} else if ((tty->ttynum+1) == modem_lastline) {

	    /*
	     * We are removing the last modem control line, so we'll
	     * have to find the new ending line.
	     */

	    while (TRUE) {

		if (--modem_lastline == 0)
		    break;
		if (MODEMS[modem_lastline - 1] &&
		    MODEMS[modem_lastline - 1]->capabilities &
		    (MODEMMASK|MODEMDTR))
		  break;	/* Get out if we find a modem line */
	    }
	}
    }

    /* Don't change flags if line is currently active */
    if (tty->statbits & (HASPROC | SLIPMODE)) {
	if (!sense)  /* Remember to reset NOBANNER when taking the shortcut */
	    tty->statbits &= ~NOBANNER;
	return;
    }

    if (sense) {
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) {
	  case MODEMINOUT:
	  case MODEMRI:
	    tty_portdisable(tty);
	    tty->statbits &= STATBITS_PERM; /* clear lots of bits */
	    tty->statbits |= (IDLE + NOBANNER);
	    /* Fall through */
	  case MODEMCTS:
	    if (modem_debug)
		ttybug(tty, "Config change: asserting DTR");
	    dtrstate(tty,TRUE);
	    break;
	  case MODEMDTR:
	    if (modem_debug)
		ttybug(tty, "Config change: dropping DTR");
	    dtrstate(tty,FALSE);
	    break;
	  default:
	    tty->statbits &= STATBITS_PERM; /* clear lots of (not all) bits */
	    tty->statbits |= (IDLE + NOBANNER);
	    tty_portdisable(tty);
	    if (modem_debug)
		ttybug(tty, "Config change: dropping DTR");
	    dtrstate(tty,FALSE);
	}
    } else {
	tty_portenable(tty);
	tty->statbits &= STATBITS_PERM; /* clear lots of bits */
        tty->statbits |= READY;
	if (modem_debug)
	    ttybug(tty, "Config change: asserting DTR");
	dtrstate(tty,TRUE);
    }
}

char *modem_statestring (tt_soc *tty)
{
    switch (tty->statbits & (RINGIN|IDLE|READY|HANGUP)) {
      case RINGIN:
	return("RINGING");
      case IDLE:
	return("IDLE");
      case READY:
	return("READY");
      case HANGUP:
	return("HANGUP");
      case 0:
	if (tty->statbits & CARDROP)
	    return("CARDROP");
	return("(null)");
      default:
	return("(unknown)");
    }
}

/*
 * fast_disconnect_scanner
 * Scan input modem signals to detect loss of carrier very quickly.
 */
static process fast_disconnect_scanner (void)
{
    int line;
    tt_soc *tty;
    modem_status current;
    modem_status changed;
    sys_timestamp fastscan_time;

    GET_TIMESTAMP(fastscan_time);
    while (TRUE) {
	process_sleep_until(&fastscan_time);
	TIMER_UPDATE(fastscan_time, fastscan_delta);

	for (line = modem_firstline; line < modem_lastline; line++) {
	    tty = MODEMS[line];
	    if (!tty)
		continue;
	
	    if (tty->capabilities & (MODEMOUT|MODEMCTS)) {
		if (tty->statbits & HANGUP) {
		    tty_portdisable(tty);
		    tty_dtrstate(tty, FALSE);
		}
		changed = tty_modemchange(tty); /* Get recent changes */
		if (!(changed.cts || changed.ring))
		    continue;
		current = tty_modemsignal(tty); /* Get current signals */
		if (tty->statbits & READY) {
		    modem_ready(tty, current.ring, !changed.cts);
		    if (tty->statbits & CARDROP) {
			if (modem_debug)
			    ttybug(tty, "Quickly dropping DTR from READY");
			tty_portdisable(tty);
			tty_dtrstate(tty, FALSE);
		    }
		}
	    }
	}
    }
}

void modemscan_command (parseinfo *csb)
{
    int n = GETOBJ(int,1);

    if (n == 0) {
	if (fastscan_fork) {
	    process_kill(fastscan_fork);
	    fastscan_fork = 0;
	}
    } else {
	fastscan_delta = n;
	if (fastscan_fork == 0) {
	    fastscan_fork = process_create(fast_disconnect_scanner,
					   "Modem Control", NORMAL_STACK,
					   PRIO_NORMAL);
	    process_set_crashblock(fastscan_fork, TRUE);
	}
    }
}

/*
 * modem_answer_timeout
 */
void modem_answer_timeout (parseinfo *csb)
{
    long i;
    tt_soc *tty = MODEMS[csb->line1];

    if (csb->nvgen) {
	nv_write((tty->answertimeout != MODEMWAIT),
		 "%s %d", csb->nv_command, tty->answertimeout);
	return;
    }

    for (i = csb->line1; i < csb->line2; i++) {
	if (MODEMS[i]->modem_type == MODEMTYPE_NONE) {
	    printf("\n%% Line %t does not support modem control", i);
	    return;
	}
    }

    if (csb->sense) {
	setinteger(csb, GETOBJ(int,2), &console->answertimeout);
    } else {
	setinteger(csb, MODEMWAIT, &console->answertimeout);
    }
}

/*
 * modem_command
 * Set up modem lines.
 */

void modem_command (parseinfo *csb)
{
    register long i, sense;
    tt_soc *tty;

    if (csb->nvgen) {
	tty = MODEMS[csb->line1];
	if (GETOBJ(int,9) > MODEMTYPE_LOCAL) {
	    /*
	     * New style modem command.  Base NV config on contents.
	     */
	    if (tty->modem_type == GETOBJ(int,9)) {
		nv_write(TRUE, csb->nv_command);
	    }
	} else if (tty->modem_type <= MODEMTYPE_LOCAL &&
		   (tty->def_capabilities & (MODEMMASK|MODEMDTR)) ==
		   GETOBJ(int,10)) {
	    nv_write(TRUE, csb->nv_command);
	}
	return;
    }

    /*
     * Ensure that all lines in the range support modem control
     */
    for (i = csb->line1; i < csb->line2; i++) {
	if (MODEMS[i]->modem_type == MODEMTYPE_NONE) {
	    if (csb->resolvemethod == RES_MANUAL)
	      printf("\n%% Line %t does not support modem control", i);
	    return;
	}

	/* if hardware flow control IN is in effect, then we must disallow
	 * modem callin, callout or cts-required 
	 */

	if (MODEMS[i]->def_capabilities & HFLOWIN) {
	    if ((GETOBJ(int,10) == MODEMIN) ||
		(GETOBJ(int,10) == MODEMOUT) ||
		(GETOBJ(int,10) == MODEMCTS)) {
		if (csb->resolvemethod == RES_MANUAL)
		  printf("\n%% Line %t cannot be configured this way if hardware flowcontrol is in effect", i);
		return;
	    }
	}
    }
    /*
     * Handle some "new style" modem configurations.
     */
    if (csb->sense && GETOBJ(int, 10) == 0) {
	/*
	 * can't use setinteger, cause modem_type is a uchar!
	 */
	for (i = csb->line1; i < csb->line2; i++) {
	    MODEMS[i]->modem_type = GETOBJ(int,9);
	}
	switch (GETOBJ(int,9)) {
	  case MODEMTYPE_PRINTER:
	    GETOBJ(int,10) = MODEMCTS;
	    break;
	  case MODEMTYPE_HOST:
	    GETOBJ(int,10) = MODEMOUT;
	    break;
	}
    } else {
	for (i = csb->line1; i < csb->line2; i++) {
	    MODEMS[i]->modem_type = MODEMTYPE_LOCAL;
	}
    }
    /*
     * Clear all modem flags before setting or clearing a
     * particular flag.  modem states are orthogonal. 
     */
    sense = csb->sense;
    csb->sense = FALSE;
    docaps(csb,(MODEMMASK|MODEMDTR), FIRST_CAP);
    csb->sense = sense;
	
    /*
     * Set/clear new flag.
     */
    docaps(csb, GETOBJ(int,10), FIRST_CAP);
    for (i = csb->line1; i < csb->line2; i++) {
	modemline(MODEMS[i],csb->sense);
    }
}

/*
 * is_asm_ttyline
 * Returns TRUE if tty is an ASM tty line, FALSE otherwise.
 */
static boolean is_asm_ttyline(tt_soc *tty)

{
    if ((IS_FAMILY(FAMILY_CSC2)) && (tty->type == TTY_LINE))
      return(TRUE);
    else
      return(FALSE);
}

/*
 * snmp_num_modem_signals
 * Return number of input or output modem signals valid on this AUX line
 */

int snmp_num_modem_signals (tt_soc *tty, uchar input_or_output)

{
    if (input_or_output == SNMP_MODEM_INPUT_SIGNAL) {
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) { 
	case MODEMIN:
	    return(2);			/* CTS (modem signal), RING */
	case MODEMOUT:
	case MODEMCTS:
	    return(1);			/* CTS (modem signal) */
	case MODEMRI:
	case MODEMRI|MODEMOUT:
	    if (tty->capabilities & HFLOWIN)
		return(2);		/* CD (RING)  & CTS (flow control) */
	    else
		return(1);		/* CD (RING) */
	case MODEMDTR:
	default:
	    if (tty->capabilities & HFLOWIN)
		return(1);		/* CTS (flow control) */
	    else
		return(0);		/* No modem or flow control */
	}
    }

    if (input_or_output == SNMP_MODEM_OUTPUT_SIGNAL) {
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) {
	case MODEMIN:
	case MODEMOUT:
	case MODEMCTS:
	    return(1);			/* DTR */
	case MODEMRI:
	case MODEMRI|MODEMOUT:
	case MODEMDTR:
	    if ((tty->capabilities & HFLOWOUT) &&
		(!is_asm_ttyline(tty))) /* !(ASM tty line) */
		return(2);		/* DTR, RTS (flow control) */
	    else 
		return(1);		/* DTR */
	default:
	    if ((tty->capabilities & HFLOWOUT) &&
		(!is_asm_ttyline(tty))) /* !(ASM tty line) */
		return(1);		/* RTS (flow control) */
	    else
		return(0);		/* No modem or flow control */
	}
    }

    return(0);			/* Shouldn't ever get here, but... */
}

/*
* modem_signal_valid
* Returns TRUE if specified signal is valid, FALSE if not.
* If valid, sets *signal_state for caller.
*/
static boolean modem_signal_valid (tt_soc *tty,
				   int     signal_number,
				   long   *signal_state,
				   uchar   input_or_output)

{
    modem_status current_state;

    current_state = tty_modemsignal(tty); /* Get modem signal state */

    if (input_or_output == SNMP_MODEM_INPUT_SIGNAL) {
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) {
	  case MODEMIN:
	    if (signal_number == D_rs232InSigName_cts) {
		if (current_state.cts != 0)	/* CTS (modem signal) */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);
	    }
	    if (signal_number == D_rs232InSigName_ri) {
		if (current_state.ring != 0)		/* RING */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);
	    }
	    return(FALSE);
	  case MODEMOUT:
	  case MODEMCTS:
	    if (signal_number == D_rs232InSigName_cts) {
		if (current_state.cts != 0)	/* CTS (modem signal) */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);
	    }
	    return(FALSE);
	  case MODEMRI:
	  case MODEMRI|MODEMOUT:
	    if (signal_number == D_rs232InSigName_dcd) {
		if (current_state.ring != 0)		/* CD (RING) */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);
	    }
	    /* Fall through for modem control check */
	  case MODEMDTR:
	  default:
	    if ((signal_number == D_rs232InSigName_cts) &&
		(tty->capabilities & HFLOWIN)) {
		if (current_state.cts != 0)		/* CTS (flowcontrol) */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);
	    } else
		return(FALSE);
	}
    }

    if (input_or_output == SNMP_MODEM_OUTPUT_SIGNAL) {
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) {
	  case MODEMIN:
	  case MODEMOUT:
	  case MODEMCTS:
	    if (signal_number == D_rs232InSigName_dtr) {
		if (current_state.dtr != 0)		/* DTR */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);		/* DTR*/
	    } else
		return(FALSE);
	  case MODEMDTR:
	  case MODEMRI:
	  case MODEMRI|MODEMOUT:
	    if (signal_number == D_rs232InSigName_dtr) {
		if (current_state.dtr != 0)		/* DTR */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);		/* DTR*/
	    }
	    /* Fall through for modem control check */
	  default:
	    if ((signal_number == D_rs232InSigName_rts) &&
		(tty->capabilities & HFLOWOUT) &&
		(!is_asm_ttyline(tty))) { /* !(ASM tty line) */
		if (current_state.rts != 0)		/* RTS */
		    *signal_state = D_rs232InSigState_on;
		else
		    *signal_state = D_rs232InSigState_off;
		return(TRUE);		/* RTS (flow control) */
	    } else
		return(FALSE);
	}
    }

    return(FALSE);			/* Shouldn't ever get here, but... */
}

/*
 * snmp_get_modem_signal
 * Returns TRUE and fills in structure pointed to by signal pointer if
 * valid signal found.  Returns FALSE otherwise. 
 * If (next == TRUE), looks for any signal >= signal_num for this tty.
 * if (next != TRUE), only looks at signal_num.
 */

boolean snmp_get_modem_signal (tt_soc           *tty, 
			       snmp_signal_info *signal_ptr,
			       long              signal_num,
			       boolean           next,
			       uchar             input_or_output)
{
    while (signal_num <= D_rs232InSigName_sdcd) { /* SDCD is last signal */
	if (modem_signal_valid(tty, signal_num, 
			       &(signal_ptr->rs232_signal_state),
			       input_or_output)) {  
	    signal_ptr->rs232_signal_name = signal_num;
	    return(TRUE);
	}

	if (!next)			/* If doing GET, bail now */
	    return(FALSE);

	signal_num++;			/* Else look at next signal */
    }
    return(FALSE);
}

/*
 * print_modemtype
 * return a string describing the type of modem control configured
 */
char *print_modemtype (tt_soc *tty)
{
    switch (tty->modem_type) {
      case MODEMTYPE_DIALIN:
	return("dialin ");
      case MODEMTYPE_DIALOUT:
	return("dialout");
      case MODEMTYPE_DIALBOTH:
	return("inout  ");
      case MODEMTYPE_HOST:
	return("host   ");
      case MODEMTYPE_OLD:
	return("old    ");
      case MODEMTYPE_LIGHTS:
	return("lights ");
      case MODEMTYPE_PRINTER:
	return("printer");
      case MODEMTYPE_NONE:
	return("   -   ");
      case MODEMTYPE_LOCAL:
	switch (tty->capabilities & (MODEMMASK|MODEMDTR)) {
	  case MODEMIN:
	    return("callin ");
	  case MODEMOUT:
	    return("callout");
	  case MODEMCTS:
	    return("cts-req");
	  case MODEMRI:
	    return("DialIn ");
	  case MODEMINOUT:
	    return("inout  ");
	  case MODEMDTR:
	    return("DTR-Act");
	  default:
	    return("   -   ");
	}
      default:
	return("unknown");
    }
}
