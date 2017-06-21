/* $Id: les_console.c,v 3.5.12.5 1996/09/10 03:22:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/les_console.c,v $
 *------------------------------------------------------------------
 * les_console.c -- simple device driver for the SCN2681 DUART
 * 
 * October 19 1989, Chris Shaker, adapted from sts1_console.c
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_console.c,v $
 * Revision 3.5.12.5  1996/09/10  03:22:22  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/08/19  18:54:08  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.5.12.3  1996/04/16  19:02:51  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/03/21  22:52:50  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.12.1  1996/03/18  20:42:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/18  23:19:54  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.1  1996/02/08  08:13:35  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.5  1996/02/13  20:42:26  hampton
 * Remove platform specific dependencies from common tty header files.
 * [CSCdi48844]
 *
 * Revision 3.4.2.1  1996/01/24  22:24:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/22  06:37:52  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  17:41:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/23  17:39:40  jquan
 * CSCdi40522:  Modularity: sub_c3000 should not require sub_brut
 *
 * Revision 2.2  1995/06/09  13:11:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:37:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "config_register.h"
#include "sys_registry.h"
#include "les_console.h"
#include "ttysrv.h"
#include "../ts/s2681.h"
#include "stacks.h"
#include "../os/async.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../les/if_les.h"
#include "../les/platform_registry.h"
#include "../os/clock.h"
#include "../ntp/ntp_public.h"
#include "../ntp/ntp_registry.h"

#if !defined(XX) && !defined(SIERRA) && !defined(RSP) && !defined(PREDATOR)
#include "../les/brut.h"
#endif

/*
 * Local storage:
 */
vectortype console_vector;
vectortype auxline_vector;
char pan_uartinterrupt;
tt_soc *bruttty;
int platform_regcode;

/*
 * Map linear treatment of console+aux0..n onto AuxBase
 */
#define PANMODEM(x) ((x == 0) ? MODEMS[0] : MODEMS[(x-1) + AuxBase])

/*
 * Default ACR setting
 */
#define DEFAULT_ACR (BRGSET1+TIMXTAL)


/*
 * pan_console_interrupt - Handle interrupts for console and aux line
 */

void pan_console_interrupt (void)
{
    ushort status;
    int c;
    uartaddress *uart;
    tt_soc *tty;
    tt_soc *aux_tty;		/* Aux pointer */
    int i;
    volatile uchar *data_ptr, *status_ptr;
    uchar ustat;
    clock_epoch curtime;
    boolean copy_timestamp;

    /*
     * First check for a dataset signal change for NTP.  Time is of the
     * essence!
     */
    aux_tty = PANMODEM(1);
    if (aux_tty->capabilities2 & PPS_CLOCK_2) {	/* You betcha */
	ustat = uartaddr->portchange; /* Get the port change flags */
	if ((ustat & aux_tty->ntp_status_mask) == aux_tty->ntp_status_value) {
	    clock_get_time_exact(&curtime);
	    reg_invoke_ntp_pps_tick(aux_tty, &curtime);
	}
    }

    for (i = 0; i <= nauxlines; i++) { /* Loop for all regular lines */
	tty = PANMODEM(i);
	if (!tty)
	  break;		/* Line not set up yet */

	data_ptr = tty->uartdata;
	status_ptr = tty->uartstatus;
	status = *status_ptr;

	if (status & OVRRNERROR) {
	    tty->overrun++;
	    uart = WHICHUART(tty->ttynum);
	    if (tty->ttynum == 0)
	      uart->commandA = RESETERROR;
	    else
	      uart->commandB = RESETERROR;
	}
	while (status & RXREADY) {

	    c = *data_ptr;

	    if (status & RCVBREAK) {	
		while (*status_ptr & RXREADY)
		  c = *data_ptr;	/* flush buffer */
		c = BREAK_CHAR;
	    }
	
	    tty->totalin++;

	    if (special_chars(tty,c) >= 0 &&
		!(tty->capabilities2 & RCV_SUPPR_CLOCK_2)) {
		

		/* Check for special NTP characters if appropriate. */

		copy_timestamp = FALSE;
		if (tty->capabilities2 & NTP_CLOCK_2) {
		    if (c == tty->ts_char_1 ||
			c == tty->ts_char_2) { /* Timestamp? */
			clock_get_time_exact(&curtime);
			copy_timestamp = TRUE;
		    }
		}
		
		if (tty->intail >= tty->inend)
		  tty->intail = tty->instart; /* Handle wraps */

		/* If we have used half of the input buffer,
		 * send an XOFF to try to stop the input.
		 */

		if (tty->incount > (tty->insize >> 1)) {
		    if (tty->intail == tty->inhead) {
			tty->overflow++;
			status = *status_ptr;
			continue;
		    }

		    /*
		     * If we are doing hardware flow control, drop RTS now.
		     * Note that this must be the aux port, since the console
		     * does not support hardware flow control.
		     */

		    if ((tty->ttynum == 1) && (tty->capabilities & HFLOWOUT)) {
			uartaddr->resetport = RTSB; /* Drop RTS */
			tty->modem_bits &= ~MODEMBIT_RTS; /* Save modem bits */
			tty->statbits |= STOPPEDINPUT; /* Flag input stopped */
		    }

		    tty->statbits |= SENDXOFF;
		    startoutput(tty);
		}
		/* so there is room  */
		*tty->intail++ = c;
		++tty->incount;

		/* Now add the timestamp to the buffer if appropriate. */

		if (copy_timestamp) {
		    reg_invoke_ntp_refclock_timestamp(tty, &curtime);
		}
	    }
	    status = *status_ptr;
	}			
	if (status & TXREADY) {

	    /*
	     * If the line is set to send flow control, do so now
	     */

	    if (tty->statbits & SENDXOFF) {
		if (tty->capabilities & SFLOWOUT) {
		    *data_ptr = tty->tty_stopchar;
		    tty->statbits |= SENTXOFF;
		    status = *status_ptr;
		}
		tty->statbits &= ~SENDXOFF;
	    }

	    /* Using the standard output service */

	    while (status & TXREADY) {

		if (tty->outcount) {
		    register uchar *cp = ++tty->outhead;
		    if (cp == tty->outend) {
			cp = tty->outstart;
			tty->outhead = cp;
		    }
		    tty->outcount--;
		    *data_ptr = *cp;
		    tty->totalout++;
		    status = *status_ptr;
		} else {
		    (*(tty->txfinish))(tty);
		    break;
		}
	    }		
	}
    }

#if !defined(XX) && !defined(SIERRA) && !defined(RSP) && !defined(PREDATOR)
    if (IS_BRUT(cookie) && bruttty)
	reg_invoke_brut2681_common(bruttty);	/* Pass interrupts to BRUT driver */
#endif
}

/*
 * pan_writemode
 * Write Mode Register 1 and 2.
 * Default values are written if any arguments are bogus.
 * (this routine ported from tty2681.c code for STP)
 */
static void pan_writemode (tt_soc *tty, int stopbits, int databits, int parity)
{
    uartaddress *uart;
    int flow;

    uart = WHICHUART(tty->ttynum);
    switch (stopbits) {
	default:
	case STOPBITS2:
	    tty->tty_stopbits = STOPBITS2;
	    stopbits = STOP32X16;
	    break;
	case STOPBITS15:
	    tty->tty_stopbits = stopbits;
	    stopbits = STOP25X16;
	    break;
	case STOPBITS1:
	    tty->tty_stopbits = stopbits;
	    stopbits = STOP16X16;
	    break;
    }
    databits = databits - 5;
    if ((databits < DBITS5) || (databits > DBITS8))
	databits = DBITS8;
    tty->tty_databits = databits+5;
    switch (parity) {
      default:
      case PARITY_NO:
	tty->tty_parity = PARITY_NO;
	parity = PRTYOFF;
	break;
      case PARITY_ODD:
	tty->tty_parity = parity;
	parity = PRTYON + PRTYODD;
	break;
      case PARITY_EVEN:
	tty->tty_parity = parity;
	parity = PRTYON + PRTYEVEN;
	break;
      case PARITY_SPACE:
	tty->tty_parity = parity;
	parity = PRTYFORCE + PRTYEVEN;
	break;
      case PARITY_MARK: 
	tty->tty_parity = parity;
	parity = PRTYFORCE + PRTYODD;
	break;
    }
    if ( (tty->capabilities & (MODEMIN+MODEMOUT+MODEMCTS)) &&
	 ((tty->capabilities & MODEMRI) == 0)) {
	flow = 0;
    } else {
	flow = (tty->capabilities & (HFLOWIN+HFLOWOUT)) ? TXCTSCONTROL : 0;
    }

    /*
     * 0 is console, else aux (may be 1, may be higher in ASM or Copan
     */
    if (tty->ttynum == 0) {
    	uart->commandA = RESETMRPTR;
	uart->modeA = (short) (databits + parity + CHARERROR + RXRDYINTR);
	uart->modeA = (short) (flow + stopbits + MODENORMAL);
    } else {
    	uart->commandB = RESETMRPTR;
	uart->modeB = (short) (databits + parity + CHARERROR + RXRDYINTR);
	uart->modeB = (short) (flow + stopbits + MODENORMAL);
    }
}

/*
 * pan_stopbits
 * Set stop bits for a pan line
 */
static void pan_stopbits (tt_soc *tty, int count)
{
    if (count != tty->tty_stopbits) {
	pan_writemode(tty, count, tty->tty_databits, tty->tty_parity);
    }
}

/*
 * pan_parity
 * Set parity handling for a pan line
 */
static void pan_parity (tt_soc *tty, int count)
{
    if (count != tty->tty_parity) {
	pan_writemode(tty, tty->tty_stopbits, tty->tty_databits, count);
    }
}

/*
 * pan_databits
 * Set 5, 6, 7, or 8 bits for a pan line
 */
static void pan_databits (tt_soc *tty, int count)
{
    if (count != tty->tty_databits) {
	pan_writemode(tty, tty->tty_stopbits, count, tty->tty_parity);
    }
}

/*
 * analyzeAUXports
 * Say that we have an extra port on the CPU.
 */

int analyzeAUXports (void)
{
    if ((cpu_type == CPU_IGS_BRUT) ||
	(cpu_type == CPU_BASS))
      return(0);		/* No aux port on BRUT or BASS */

    return(1);			/* AUX port on other processors */
}

void pan_tx_disable (
     register tt_soc *tty)
{
    if (tty->ttynum == 0)
        uartaddr->commandA = TXDISABLE;
    else
        uartaddr->commandB = TXDISABLE;
}

/*
 * pan_baud2code
 * Convert a baud rate number to a code that the UART understands.
 * Returns -1 if baud rate illegal.
 *
 * We get 38.4Kbit when using Baud Rate Generator 1 by setting up the
 * Counter/Timer to generate 38.4Kbit when the "timer" baud rate is
 * specified.  This is accomplished by setting "48" in the counter/time
 * register and specifying crystal clock (1X) as the source.
 */

#define NBAUDS 14

const int baudtable[NBAUDS] = {
  75, 110, 134, 150, 300, 600, 1200, 2000, 2400, 4800, 1800, 9600, 19200, 38400
  };

int pan_baud2code (
    register int baud)
{
    register int code;
    
    for (code = 0; code < NBAUDS; code++) {
	if (baudtable[code] == baud)
	    return(code);
    }
    return(-1);
}

/*
 * pan_baudset - set the baud rate, enable TX and RX actions
 */

boolean pan_baudset (
    register tt_soc *tty,
    int txbaud,
    int rxbaud)
{
    register int txcode, rxcode;
    leveltype SR;

    /*
     * Default baud rates if so desired
     */
    SR = raise_interrupt_level(TTY_DISABLE);
    if (txbaud == -1)
	txbaud = tty->tty_txspeed;
    if (rxbaud == -1)
	rxbaud = tty->tty_rxspeed;

    /*
     * Don't touch the hardware if no change being made (prevents garbage):
     */
    if ((txbaud == tty->tty_txspeed) && (rxbaud == tty->tty_rxspeed)) {
	reset_interrupt_level(SR);
	return(TRUE);
    }
    
    /*
     * Translate speeds to codes.  Change baud rates if codes are good
     * and are different from what we were already running with:
     */
    txcode = pan_baud2code(txbaud);
    rxcode = pan_baud2code(rxbaud);
    if ((txcode == -1) || (rxcode == -1)) {
	reset_interrupt_level(SR);
	return(FALSE);
    }
    tty->tty_txspeed = txbaud;
    tty->tty_rxspeed = rxbaud;
    
    if (tty->ttynum == 0) {
	uartaddr->statusA = (RXCLOCK * rxcode + TXCLOCK * txcode);
	uartaddr->commandA = (STP_TXENABLE + STP_RXENABLE);
    } else {
	uartaddr->statusB = (RXCLOCK * rxcode + TXCLOCK * txcode);
	uartaddr->commandB = (STP_TXENABLE + STP_RXENABLE);
    }
    reset_interrupt_level(SR);
    return(TRUE);
}

/*
 * pan_aux_dtr_state - Assert or clear DTR on auxiliary port
 */

void pan_aux_dtr_state (
    register tt_soc *tty,
    boolean sense)
{
    uchar flag;

    if (modem_debug)
	buginf("\nTTY%t: Set DTR to %d", tty->ttynum, sense);
    if (tty->ttynum == 0)
      flag = DTRA;
    else
      flag = DTRB;
    
    if (sense == TRUE) {
	uartaddr->setport = flag;
	tty->modem_bits |= MODEMBIT_DTR; /* Update saved modem state */
    } else {
	uartaddr->resetport = flag;
	tty->modem_bits &= ~MODEMBIT_DTR; /* Update saved modem state */
    }
}

/*
 * pan_modemsignal - Return the current modem control signals
 *
 * This routine is used to read the current state of the modem control
 * signals. Because the modem state for both lines is returned
 * in a single read operation, we handle both the console and the
 * aux port here.
 */

modem_status pan_modemsignal (
     tt_soc *tty)
{
    tt_soc *console_tty;	/* Console pointer */
    tt_soc *aux_tty;		/* Aux pointer */
    uchar ustat;		/* Current modem state/change flags */
    modem_status summary;	/* Returned status */

    console_tty = PANMODEM(0);	/* Get console pointer */
    aux_tty = PANMODEM(1);	/* Get aux pointer */

    ustat = uartaddr->portchange; /* Get the port change flags */

    /*
     * Handle the change flags for each line. We do both lines here
     * since reading the port change flags clears the change flags.
     */

    if (ustat & DELTA_RINGA)
      console_tty->modem_changes.ring = TRUE;

    if (ustat & DELTA_CTSB)
      aux_tty->modem_changes.cts = TRUE;

    /*
     * Now return the status for the selected line.
     */

    if (tty->ttynum == 0) {
	summary.cts = FALSE;
	summary.ring = !(ustat & RINGA);
    } else {
	summary.cts = !(ustat & CTSB);
	summary.ring = !(uartaddr->portstatus & DCDB);
	summary.rts = ((tty->modem_bits & MODEMBIT_RTS) != 0);
	summary.dtr = ((tty->modem_bits & MODEMBIT_DTR) != 0);
    }

    return(summary);
}

/*
 * pan_print_modemctrl
 * print the states of the modem control input bits (AUX line only)
 *
 */

static void pan_print_modemctrl (tt_soc *tty)
{
    modem_status summary;    

    if (tty->type != AUX_LINE)
	return;

    /* 
     * Get the modem signal information of the tty 
     */
    summary = pan_modemsignal(tty);

    /* 
     * Display the modem signals - note that ring is DSR so long 
     * as Cisco MMOD connectors are used (pin 8 to pin 6).
     */
    printf("\nModem hardware state: %sCTS%s%sDSR%s %sDTR %sRTS",
	   summary.cts ? "":"no", summary.cts ? "* ":" ",
	   summary.ring ? "":"no", summary.ring ? "* ":" ",
	   summary.dtr ? "":"no",  summary.rts ? "":"no");

}	

/*
 * pan_modemchange - Return (and clear) the current modem control state flags
 *
 * This is the service routine to return the current modem control change
 * flags. We update the change flags from both lines, pick up the change
 * flags for this line, clear them out, and return them.
 *
 * This is necessary because we have to track the state in software to
 * workaround the fact that a read of the change flag effects both lines
 * in the duart.
 */

modem_status pan_modemchange (
     tt_soc *tty)
{
    modem_status summary;

    pan_modemsignal(tty);	/* Update current state */
    summary = tty->modem_changes; /* Pick up current change flag */
    tty->modem_changes = ((modem_status){0}); /* Reset the flags */

    return(summary);
}

/*
 * pan_portdisable - Disable a port temporarily
 */

void pan_portdisable (
     tt_soc *tty)
{
    if (tty->ttynum == 0)
      uartaddr->commandA = RXDISABLE;
    else
      uartaddr->commandB = RXDISABLE;
}

/*
 * pan_portenable - Enable a port
 */

void pan_portenable (
     tt_soc *tty)
{
    if (tty->ttynum == 0)
      uartaddr->commandA = (STP_TXENABLE + STP_RXENABLE);
    else
      uartaddr->commandB = (STP_TXENABLE + STP_RXENABLE);
}

/*
 * pan_startoutput - Start output by forcing a transmit interrupt.
 */

boolean pan_startoutput (
    register tt_soc *tty)
{
    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);

    if (tty->statbits & (HOLDING | WANTXON))
      return(TRUE);		/* Do nothing if output stopped */

    if ((tty->capabilities & (MODEMIN+MODEMOUT)) == 0) {
	pan_portenable(tty);
	return(TRUE);
    }

    /*
     *	A modem. Can not interrupt if IDLE || RINGIN so ...
     */

    if (tty->statbits & READY) {
	pan_portenable(tty);
	return(TRUE);
    }

    return(FALSE);
}

/*
 * pan_startinput - Reset any stopped input
 */

void pan_startinput (
     tt_soc *tty)
{
    if (tty->ttynum == 0)
      return;			/* Nothing to do for console line */

    uartaddr->setport = RTSB;	/* Set RTS */
    tty->modem_bits |= MODEMBIT_RTS;	/* Update saved modem state. */
    tty->statbits &= ~STOPPEDINPUT; /* Input not stopped anymore */
}

/*
 * pan_setflow - Set up hardware flow control
 */

void pan_setflow (
     tt_soc *tty)
{
    if (tty->ttynum == 0) 	/* Not allowed on console */
      return;

    /*
     * Unconditionally set RTS here. Reason: if hardware flow control
     * is disabled, we want to assert RTS. If hardware flow control is
     * enabled, then by setting RTS we assume that there is room for input -
     * if our assumption is wrong, the interrupt service routine will fix it.
     */

    uartaddr->setport = RTSB;	/* Set RTS */
    tty->modem_bits |= MODEMBIT_RTS;	/* Update saved modem state. */

    /*
     * Set up handling of CTS based on hardware flow control state
     */

    if (tty->capabilities & HFLOWIN)
      uartaddr->modeB = MODENORMAL | STOP16X16 | TXCTSCONTROL;
    else
      uartaddr->modeB = MODENORMAL | STOP16X16;
}

/*
 * pan_breakservice - Dummy txservice handler for sending breaks
 */
uint pan_breakservice (tt_soc *tty, uchar *p, int len)
{
    /* Transmit nulls during break interval */
    p[0] = '\0';
    return(1);
}

/*
 * pan_sendbreak - Send a break to the AUX port
 */

void pan_sendbreak (tt_soc *tty)
{
    if (tty->ttynum == 0)
      return;			/* Not allowed on console */

    pan_uartinterrupt &= ~TXRDYB; /* Disable interrupts */
    uartaddr->interrupt = pan_uartinterrupt; /* Set that in hardware */
    tty->txservice = pan_breakservice; /* Set a dummy ISR to be safe */
    tty->statbits |= SENDINGBREAK; /* Mark we are sending break */
    uartaddr->commandB = STP_TXENABLE; /* Enable the transmitter */
    uartaddr->commandB = STARTBREAK; /* Start sending break */
    process_sleep_for(ONESEC/2);	/* Delay 500 milliseconds */
    tty->txservice = NULL;	/* Disable transmit hook */
    uartaddr->commandB = STOPBREAK; /* Stop sending break */
    pan_uartinterrupt |= TXRDYB; /* Enable interrupts */
    uartaddr->interrupt = pan_uartinterrupt; /* Set that in hardware */
    tty->statbits &= ~SENDINGBREAK; /* No longer sending break */
}

/*
 * pan_ntpsetup
 *
 * Set up for NTP
 */
static boolean pan_ntpsetup (tt_soc *tty, ntp_setuptype type, ulong pps_flags)
{
    switch (type) {

      case NTP_SETUP_PROBE:

	/* Do we supprt NTP?  Sure. */
	return(TRUE);


      case NTP_SETUP_PPSTEST:

	/* Do we support these PPS bits?  We support RI and CTS. */

	return((pps_flags & (NTP_PPS_CTS | NTP_PPS_RI)) == pps_flags);


      case NTP_SETUP_DISABLE:
      case NTP_SETUP_ENABLETIME:

	/* Disabling PPS.  Untwiddle things. */

	uartaddr->portchange = DEFAULT_ACR;
	pan_uartinterrupt &= ~MODEMX;
	uartaddr->interrupt = pan_uartinterrupt;
	return(TRUE);

      case NTP_SETUP_ENABLETIMEPPS:
      case NTP_SETUP_ENABLEPPS:

	/* Enabling PPS.  Twiddle the various UART flags. */

	switch (pps_flags & (~NTP_PPS_INVERTED)) {
	  case NTP_PPS_CTS:
	    tty->ntp_status_mask = DELTA_CTSB | CTSB;
	    if (pps_flags & NTP_PPS_INVERTED) {
		tty->ntp_status_value = DELTA_CTSB;
	    } else {
		tty->ntp_status_value = DELTA_CTSB | CTSB;
	    }
	    uartaddr->portchange = DEFAULT_ACR | CTSBXENB;
	    break;

	  case NTP_PPS_RI:
	    tty->ntp_status_mask = DELTA_RINGB | RINGB;
	    if (pps_flags & NTP_PPS_INVERTED) {
		tty->ntp_status_value = DELTA_RINGB;
	    } else {
		tty->ntp_status_value = DELTA_RINGB | RINGB;
	    }
	    uartaddr->portchange = DEFAULT_ACR | RINGBXENB;
	    break;

	  default:
	    return(FALSE);		/* Bad parameter! */
	}

	pan_uartinterrupt |= MODEMX;
	uartaddr->interrupt = pan_uartinterrupt;
	return(TRUE);
	
      default:
	return(FALSE);
    }
}

/*
 * console_putc - Write a byte to the console, using polled I/O
 */

void console_putc (uchar ch)
{
    leveltype status;		/* Saved interrupt level */
    volatile uchar dummy;

    if (ch == '\n')
      console_putc('\r');	/* Preface NL with CR */

    status = raise_interrupt_level(TTY_DISABLE); /* Lock out interrupts */

    uartaddr->commandA = STP_TXENABLE; /* Ensure transmitter is enabled */

    while (!(uartaddr->statusA & TXREADY)) ; /* Wait for transmitter ready */

    uartaddr->dataA = ch;	/* Store the character */

    dummy = uartaddr->statusA;  /* syncronize write cycle; see CSCdi67886 */

    reset_interrupt_level(status);		/* Restore interrupt level */
}

/*
 * console_getc - Read a byte from the console, using polled I/O
 */

int console_getc (void)
{
    char ch;
    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE); /* Lock out interrupts */

    uartaddr->commandA = STP_RXENABLE; /* Ensure receiver is enabled */

    while (!(uartaddr->statusA & RXREADY))
      ;				/* Loop for a character */

    ch = uartaddr->dataA;	/* Get the character */

    reset_interrupt_level(status); /* Restore interrupts */

    return(ch & 0x7f);		/* Return the character trimmed */
}

/*
 * les_vector_init
 *
 * Initialize TTY vector fields
 */
static void les_vector_init (vectortype *vector)
{
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = pan_startoutput;
    vector->vstartinput = pan_startinput;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->vsetspeed = pan_baudset;
    vector->vautosetspeed = async_autosetspeed;
    vector->vauto_baud = async_autobaud;
    vector->vstopbits = pan_stopbits;
    vector->vparitybits = pan_parity;
    vector->vdatabits = pan_databits;
    vector->vdtrstate = pan_aux_dtr_state;
    vector->vsetflow = pan_setflow;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vputpkt = generic_putpkt;
    vector->vstopoutput = pan_tx_disable;
    vector->vmodemsignal = pan_modemsignal;
    vector->vmodemchange = pan_modemchange;
    vector->vportdisable = pan_portdisable;
    vector->vportenable = pan_portenable;
    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;
    vector->vsendbreak = pan_sendbreak;
}

/*
 * console_init
 * Set up Port A as System Console. The ROM monitor will already
 * have initialized the speed and such.
 */
				
tt_soc *console_init (void)
{
    register tt_soc *tty;
    vectortype *vector;
    leveltype SR;
    uchar dummy;

    SR = raise_interrupt_level(TTY_DISABLE);

    /*
     * Initialize the vector pointers
     */

    vector = &console_vector;
    vector_init(vector);
    les_vector_init(vector);

    /*
     * Set up the TTY data structure
     */

    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty, MAXINCHARS, MAXOUTCHARS);
    tty->uartdata = &uartaddr->dataA;
    tty->uartstatus = &uartaddr->statusA;
    tty->txfinish = pan_tx_disable;
    tty->tty_stopbits = STOPBITS2;

    createlevel(LEVEL_CONSOLE, pan_console_interrupt, "Console Uart");

    /*
     * Initialize the common fields of the UART
     */

    uartaddr->portchange = DEFAULT_ACR;
    uartaddr->portstatus = 0;

    uartaddr->CTUpper = 0;		/* Set programmable rate */
    uartaddr->CTLower = 3;		/* To generate 38.4 kbit */
    dummy = uartaddr->setport;	/* start counter/timer(read setport)*/

    /*
     * Initialize the PORT A portion of the UART
     */

    uartaddr->commandA = RESETMRPTR;
    uartaddr->modeA = (RXRDYINTR + CHARERROR + PRTYOFF + DBITS8);
    uartaddr->modeA = (MODENORMAL + STOP16X16);
    uartaddr->commandA = (STP_TXENABLE + STP_RXENABLE);
    uartaddr->resetport = 255;		/* clear all RS-232 signals */
    tty->modem_bits &= ~(255);		/* clear saved modem state. */
    uartaddr->setport = DTRA;		/* raise DTR on A port */
    tty->modem_bits |= MODEMBIT_DTR;	/* Update saved modem state. */

    /*
     * Enable interrupts
     */

    pan_uartinterrupt = (RXRDYA|TXRDYA);
    uartaddr->interrupt = pan_uartinterrupt; /* Enable interrupts */

    reset_interrupt_level(SR);
    return(tty);
}

/*
 * auxline_init
 * Initialize port B of console UART.
 */

tt_soc *auxline_init (void)
{
    register tt_soc *tty;
    leveltype SR;
    vectortype *vector;

    SR = raise_interrupt_level(TTY_DISABLE);

    vector = &auxline_vector;
    vector_init(vector);
    les_vector_init(vector);
    vector->vntpsetup = pan_ntpsetup;
    tty = (tt_soc *) tty_init(AuxBase,READY,AUX_LINE,vector);
    if (!tty) {
	reset_interrupt_level(SR);
	return(NULL);
    }

    serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS); /* set up buffering */ 

    tty->uartdata = &uartaddr->dataB;
    tty->uartstatus = &uartaddr->statusB;
    tty->txfinish = pan_tx_disable;

    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->modem_type = MODEMTYPE_LOCAL; /* No modem control initially */

    uartaddr->commandB = RESETMRPTR;
    uartaddr->modeB = (RXRDYINTR | CHARERROR | PRTYOFF | DBITS8);
    uartaddr->modeB = MODENORMAL | STOP16X16;

    tty->tty_rxspeed = tty->tty_txspeed = 0;
    setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);

    /*
     * Enable interrupts:
     */
    uartaddr->commandB = TXDISABLE + STP_RXENABLE;
    pan_uartinterrupt |= (RXRDYB | TXRDYB);
    uartaddr->interrupt = pan_uartinterrupt;
    uartaddr->setport = DTRB|RTSB;	/* Raise DTR and RTS */
    tty->modem_bits |= MODEMBIT_DTR|MODEMBIT_RTS; /* Update saved modem bits */

    reset_interrupt_level(SR);
    reg_add_tty_show(pan_print_modemctrl, "print_modemcontrol");
    return(tty);
}

/*
 * console_check_break:
 * Return 1 if a break has been recieved and they are enabled.
 * Else return 0. If 1 is returned the real time tick interrupt
 * handler will abort the image via mon_cpu_exception and we'll
 * end up back in the rom monitor. We check that breaks are enabled
 * before checking the UART, so we don't waste an expensive IO cycle.
 *
 * To help alleviate the race condition that occurs when the box
 * is configured with breaks disabled and autobooting, and there
 * is a problem with booting the required system image, we allways
 * treat breaks as enabled for the first few seconds of image
 * execution.
 *
 * Note: This routine is only used by boxes that do their own
 * break key processing, rather than letting the monitor do it.
 */
static int brk;

int
console_check_break(void)
{
    volatile unsigned char i;

    if (( !(configregister & CFG_BREAK) ||  CLOCK_IN_STARTUP_INTERVAL(5*ONESEC)) &&
	(uartaddr->interrupt & BREAKXA)) {

	if (brk) {
	    brk = 0;
	    while (uartaddr->statusA & RXREADY)
		i = uartaddr->dataA;
	    uartaddr->commandA = RESETERROR;
	    uartaddr->commandA = RESETDBREAK;
	    return(1);
	} else {
	    brk = 1;
	    while (uartaddr->statusA & RXREADY)
		i = uartaddr->dataA;
	    uartaddr->commandA = RESETERROR;
	    uartaddr->commandA = RESETDBREAK;
	}
    }
    return(0);
}
