/* $Id: csc2_console.c,v 3.4.12.3 1996/08/28 12:43:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/csc2_console.c,v $
 *------------------------------------------------------------------
 * csc2_console.c -- simple device driver for the Signetics 68562 UART
 * 
 * September 1987, Greg Satz
 * July 1989, Chris Shaker (Added AUX port support)
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csc2_console.c,v $
 * Revision 3.4.12.3  1996/08/28  12:43:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.12.2  1996/08/19  18:53:13  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.4.12.1  1996/03/18  19:40:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/14  00:56:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.20.2  1996/02/20  00:50:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  20:42:19  hampton
 * Remove platform specific dependencies from common tty header files.
 * [CSCdi48844]
 *
 * Revision 3.3.20.1  1996/01/24  21:45:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/01  18:47:36  sjackson
 * CSCdi36114:  async routing dies under heavy load
 *
 * Revision 3.2  1995/11/17  09:15:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:04:24  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:38:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_csc2.c"			/* Not a typo, see logger.h */
#include "stacks.h"
#include "../hes/s68562.h"
#include "ttysrv.h"
#include "../os/async.h"
#include "../os/clock.h"
#include "../ntp/ntp_public.h"
#include "../ntp/ntp_registry.h"

#ifdef DEBUG
static void sig_a_dump_regs(void);
static void sig_b_dump_regs(void);
#endif

extern void sig_a_receive(void);
extern void sig_a_transmit(void);
extern void sig_a_status(void);
extern void sig_a_spurious(void);

extern void sig_b_receive(void);
extern void sig_b_transmit(void);
extern void sig_b_status(void);
extern void sig_b_spurious(void);


#define ERROR	(-1)

static vectortype sig_a_vector;	/* local storage for Console Port */
static vectortype sig_b_vector;	/* local storage for Auxiliary Port */


/*
 * hes_ntpsetup
 *
 * Set up for NTP
 */
static boolean hes_ntpsetup (tt_soc *tty, ntp_setuptype type, ulong pps_flags)
{
    switch (type) {

      case NTP_SETUP_PROBE:

	/* Do we support NTP?  Sure. */

	return(TRUE);


      case NTP_SETUP_PPSTEST:

	/* Do we support these PPS bits?  We support RI and CTS. */

	return((pps_flags & (NTP_PPS_CTS | NTP_PPS_RI)) == pps_flags);


      case NTP_SETUP_DISABLE:
      case NTP_SETUP_ENABLETIME:

	/* Disabling PPS.  Untwiddle things. */

	uartaddr->b.ier &= ~IER_DCD_CTS;
	return(TRUE);


      case NTP_SETUP_ENABLETIMEPPS:
      case NTP_SETUP_ENABLEPPS:

	/* Enabling PPS.  Twiddle the various UART flags. */

	switch (pps_flags & (~NTP_PPS_INVERTED)) {
	  case NTP_PPS_CTS:
	    tty->ntp_status_mask = (ICTSR_DCTS|ICTSR_CTS);
	    if (pps_flags & NTP_PPS_INVERTED) {
		tty->ntp_status_value = ICTSR_DCTS|ICTSR_CTS;
	    } else {
		tty->ntp_status_value = ICTSR_DCTS;
	    }
	    break;

	  case NTP_PPS_RI:
	    tty->ntp_status_mask = (ICTSR_DDCD|ICTSR_DCD);
	    if (pps_flags & NTP_PPS_INVERTED) {
		tty->ntp_status_value = ICTSR_DDCD|ICTSR_DCD;
	    } else {
		tty->ntp_status_value = ICTSR_DDCD;
	    }
	    break;

	  default:
	    return(FALSE);		/* Bad parameter! */
	}

	uartaddr->b.ier |= IER_DCD_CTS;	/* We have to enable both. */
	uartaddr->b.ictsr = ICTSR_CTZERO|ICTSR_DDCD|ICTSR_DCTS;
	return(TRUE);

      default:
	return(FALSE);			/* Stupid compiler. */
    }
}

/*
 * console_startoutput - Start output on the CON port
 */

static boolean console_startoutput (tt_soc *tty)
{
    leveltype SR;

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);
 
    SR = raise_interrupt_level(TTY_DISABLE);

    /*
     * Enable transmit interrupts:
     */

    uartaddr->a.ier |= IER_TX_RDY;

    reset_interrupt_level(SR);
    return(TRUE);
}
/*
 * Interrupt handlers for the processor's Signetics 68562 uart:
 */

/*
 * sig_a_receive_int - Console Port Input interrupt:
 */

void sig_a_receive_int (void)
{
    register tt_soc *tty = MODEMS[0];
    register int c;

    while (uartaddr->a.gsr & GSR_A_RXRDY) {

	c = uartaddr->a.rxfifo[0];

	tty->totalin++;
	    
	if (special_chars(tty, c) >= 0) {
		    
	    if (tty->intail >= tty->inend)
	      tty->intail = tty->instart; /* Handle wraps */
		    
	    /* If we have used half of the input buffer,
	     * send an XOFF to try to stop the input.
	     */

	    if (tty->incount > (tty->insize >> 1)) {
		if (tty->intail == tty->inhead) {
		    tty->overflow++;
		    continue;
		}
		tty->statbits |= SENDXOFF;
		uartaddr->a.ier |= IER_TX_RDY;
	    }
	    /* so there is room  */
	    *tty->intail++ = c;
	    ++tty->incount;
	}
    }
}

/*
 * sig_a_transmit_int- Console Port Output interrupt:
 */

void sig_a_transmit_int (void)
{
    tt_soc *tty = MODEMS[0];

    if (tty->statbits & SENDXOFF && uartaddr->a.gsr & GSR_A_TXRDY) {
	if (tty->capabilities & SFLOWOUT) {
	    uartaddr->a.txfifo[0] = (uchar) tty->tty_stopchar;
	    tty->statbits |= SENTXOFF;
	}
	tty->statbits &= ~SENDXOFF;
    }

    while (uartaddr->a.gsr & GSR_A_TXRDY) {

	if (tty->outcount) {
	    register uchar *cp = ++tty->outhead;
	    if (cp == tty->outend) {
		cp = tty->outstart;
		tty->outhead = cp;
	    }
	    tty->outcount--;
	    uartaddr->a.txfifo[0] = *cp;
	    tty->totalout++;
	} else {
	    uartaddr->a.ier &= ~IER_TX_RDY;
	    break;
	}
    }
}

/*
 * sig_a_status_int - Console Port Status Change Interrupt:
 */
void sig_a_status_int (void)
{
    uchar status;

    /*
     * Note the error:
     */

    status = uartaddr->a.rsr;
    if (status & (RSR_FRAME | RSR_PARITY))
      MODEMS[0]->linenoise++;
    else if (status & RSR_OVERRUN)
      MODEMS[0]->overrun++;

    /*
     * Clear the error status bits:
     */
    uartaddr->a.trsr =
	TRSR_TX_EMPTY|TRSR_CTS_UNDER|TRSR_BREAK_ACK|TRSR_DPLL_ERR;
    uartaddr->a.rsr = RSR_CHAR_CMP|RSR_RTS_NEG|RSR_OVERRUN|RSR_BRK_END|
	RSR_BRK_START|RSR_FRAME|RSR_PARITY;
    uartaddr->a.ictsr = ICTSR_CTZERO|ICTSR_DDCD|ICTSR_DCTS;
}



/*
 * sig_a_spurious - handle spurious interrupts on A channel
 */

void sig_a_spurious_int (void)
{
#ifdef	DEBUG
    buginf("CONSOLE: channel A spurious interrupt:\n");
    sig_a_dump_regs();
    sig_b_dump_regs();
#endif

    /*
     * Clear any unwanted potential interrupt causes:
     */
    uartaddr->a.trsr =
	TRSR_TX_EMPTY|TRSR_CTS_UNDER|TRSR_BREAK_ACK|TRSR_DPLL_ERR;
    uartaddr->a.rsr = RSR_CHAR_CMP|RSR_RTS_NEG|RSR_OVERRUN|RSR_BRK_END|
	RSR_BRK_START|RSR_FRAME|RSR_PARITY;
    uartaddr->a.ictsr = ICTSR_CTZERO|ICTSR_DDCD|ICTSR_DCTS;
}

/*
 * console_putc - Output a character, polled
 */

void console_putc (uchar ch)
{
    leveltype status;		/* Saved interrupt level */
    
    if (ch == '\n')
	console_putc('\r');	/* Preface NL with CR */
    
    status = raise_interrupt_level(TTY_DISABLE); /* Lock out interrupts */

    uartaddr->a.ccr = CCR_TCMD | CCR_ENABLE; /* Ensure transmitter enabled */

    while (!(uartaddr->a.gsr & GSR_A_TXRDY))
      ;				/* Wait for output ready */

    uartaddr->a.txfifo[0] = ch;	/* Output the character */

    reset_interrupt_level(status);		/* Restore interrupt level */
}

/* console_getc - Input a character, polled
 *
 * This routine is used to get characters for gdb with interrupts disabled.
 */

int console_getc (void)
{
    struct regpair *console = &uartaddr->a;
    char ch;

    while ((console->gsr & GSR_A_RXRDY) == 0) ;

    ch = console->rxfifo[0] & 0177;
    return(ch);
}

/*
 * sig_b_receive_int - Auxiliary Port Input interrupt:
 */

void sig_b_receive_int (void)
{
    register tt_soc *tty = MODEMS[AuxBase];
    register int c;
    boolean copy_timestamp;
    clock_epoch curtime;

    /*
     * Pull all of the characters out of the FIFO:
     */
    while (uartaddr->b.gsr & GSR_B_RXRDY) {

	c = uartaddr->b.rxfifo[0];

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

		/*
		 * Drop RTS if we are doing hardware flow control
		 */

		if (tty->capabilities & HFLOWOUT) {
		  uartaddr->b.omr &= ~OMR_ASSERT_RTS; /* Drop RTS */
		  tty->statbits |= STOPPEDINPUT;
		}

		if (tty->intail == tty->inhead) {
		    tty->overflow++;
		    continue;
		}
		tty->statbits |= SENDXOFF;
		uartaddr->b.ier |= IER_TX_RDY;
	    }
	    /* so there is room  */
	    *tty->intail++ = c;
	    ++tty->incount;

	    /* Now add the timestamp to the buffer if appropriate. */

	    if (copy_timestamp) {
		reg_invoke_ntp_refclock_timestamp(tty, &curtime);
	    }
	}
    }
}



/*
 * sig_b_transmit_int - Auxiliary Port Output interrupt:
 */

void sig_b_transmit_int (void)
{
    tt_soc *tty = MODEMS[AuxBase];

    if (tty->statbits & SENDXOFF && uartaddr->b.gsr & GSR_B_TXRDY) {
	if (tty->capabilities & SFLOWOUT) {
	    uartaddr->b.txfifo[0] = (uchar) tty->tty_stopchar;
	    tty->statbits |= SENTXOFF;
	}
	tty->statbits &= ~SENDXOFF;
    }

    while (uartaddr->b.gsr & GSR_B_TXRDY) {

	if (tty->outcount) {
	    register uchar *cp = ++tty->outhead;
	    if (cp == tty->outend) {
		cp = tty->outstart;
		tty->outhead = cp;
	    }
	    tty->outcount--;
	    uartaddr->b.txfifo[0] = *cp;
	    tty->totalout++;
	} else {
	    uartaddr->b.ier &= ~IER_TX_RDY;
	    break;
	}
    }
}



/*
 * sig_b_status_int - Auxiliary Port Status Change Interrupt:
 */
void sig_b_status_int (void)
{
    uchar status;

    /*
     * Note the error:
     */
    if (nauxlines) {
	status = uartaddr->b.rsr;
	if (status & (RSR_FRAME | RSR_PARITY))
	  MODEMS[AuxBase]->linenoise++;
	else if (status & RSR_OVERRUN)
	  MODEMS[AuxBase]->overrun++;
    }

    /*
     * Clear the error status bits:
     */
    uartaddr->b.trsr =
	TRSR_TX_EMPTY|TRSR_CTS_UNDER|TRSR_BREAK_ACK|TRSR_DPLL_ERR;
    uartaddr->b.rsr =
	RSR_CHAR_CMP|RSR_RTS_NEG|RSR_OVERRUN|RSR_BRK_END|
	    RSR_BRK_START|RSR_FRAME|RSR_PARITY;
    uartaddr->b.ictsr =
	ICTSR_CTZERO|ICTSR_DDCD|ICTSR_DCTS;
}



/*
 * sig_b_spurious
 * Handle spurious interrupts on B channel
 */

void sig_b_spurious_int (void)
{
    tt_soc *aux_tty;
    uchar status;
    clock_epoch curtime;

#ifdef	DEBUG
    buginf("AUX: channel B spurious interrupt:\n");
    sig_a_dump_regs();
    sig_b_dump_regs();
#endif
    aux_tty = MODEMS[AuxBase];
    if (aux_tty->capabilities2 & PPS_CLOCK_2) {	/* You betcha */
	status = uartaddr->b.ictsr;
	if ((status & aux_tty->ntp_status_mask) == aux_tty->ntp_status_value) {
	    clock_get_time_exact(&curtime);
	    reg_invoke_ntp_pps_tick(aux_tty, &curtime);
	}
    }

    /*
     * Clear potential interrupt causes:
     */
    uartaddr->b.trsr =
	TRSR_TX_EMPTY|TRSR_CTS_UNDER|TRSR_BREAK_ACK|TRSR_DPLL_ERR;
    uartaddr->b.rsr = RSR_CHAR_CMP|RSR_RTS_NEG|RSR_OVERRUN|RSR_BRK_END|
	RSR_BRK_START|RSR_FRAME|RSR_PARITY;
    uartaddr->b.ictsr = ICTSR_CTZERO|ICTSR_DDCD|ICTSR_DCTS;
}


/*
 * AUX_startoutput - Start output on the AUX port
 */

static boolean AUX_startoutput (tt_soc *tty)
{
    leveltype SR;

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);

    /*
     * Do nothing if output stopped
     */

    if (tty->statbits & (HOLDING | WANTXON))
      return(TRUE);

    /*
     * Lock out interrupts while touching registers
     */

    SR = raise_interrupt_level(TTY_DISABLE);

    /*
     * Enable transmit interrupts:
     */

    uartaddr->b.ier |= IER_TX_RDY;

    reset_interrupt_level(SR);
    return(TRUE);
}

/*
 * AUX_startinput - start up pending input
 *
 * This routine is used to start up pending input that may have been
 * stopped. We assert RTS here. There is no reason to check if we
 * are actually using hardware flow control, since if we are not, RTS
 * is always enabled.
 */

static void AUX_startinput (tt_soc *tty)
{
    uartaddr->b.omr |= OMR_ASSERT_RTS;

    tty->statbits &= ~STOPPEDINPUT; /* Input not stopped anymore */
}

/*
 * AUX_baud2code
 * Convert a baud rate number to a code that the UART understands.
 * Returns -1 if baud rate illegal.
 */

#define NBAUDS 16

static const int AUX_baudtable[NBAUDS] = {
	50, 75, 110, 134, 150, 200, 300, 600, 1050,
	1200, 2000, 2400, 4800, 9600, 19200, 38400
};

static int AUX_baud2code (register int baud)
{
    int code;

    for (code = 0; code < NBAUDS; code++) {
	if (AUX_baudtable[code] == baud)
	    return(code);
    }
    return(-1);
}

/*
 * AUX_baudset - set the baud rate for the auxiliary port
 */

static boolean AUX_baudset (tt_soc *tty, int txbaud, int rxbaud)
{
    int txcode, rxcode;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);

    /*
     * Default baud rates if so desired
     */
    if (txbaud == -1)
	txbaud = tty->tty_txspeed;
    if (rxbaud == -1)
	rxbaud = tty->tty_rxspeed;

    /*
     * Translate speeds to codes.  Change baud rates if codes are good.
     */
    txcode = AUX_baud2code(txbaud);
    rxcode = AUX_baud2code(rxbaud);
    if ((txcode == -1) || (rxcode == -1)) {
	reset_interrupt_level(SR);
	return(FALSE);
    }
    tty->tty_txspeed = txbaud;
    tty->tty_rxspeed = rxbaud;

    uartaddr->b.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_DISABLE;

    uartaddr->b.ttr = TTR_CLOCKBRG | txcode;
    uartaddr->b.rtr = RTR_CLOCKBRG | rxcode;

    uartaddr->b.ccr = CCR_TCMD | CCR_RESET;
    uartaddr->b.ccr = CCR_RCMD | CCR_RESET;
    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_ENABLE;

    reset_interrupt_level(SR);
    return(TRUE);
}

/*
 * AUX_dtrstate - change the state of DTR on the AUX port
 */

static void AUX_dtrstate (tt_soc *tty, boolean state)
{
    if (state)
	uartaddr->b.omr |= OMR_ASSERT_DTR;
    else
	uartaddr->b.omr &= ~OMR_ASSERT_DTR;
}

/*
 * AUX_setflow - Set flow control state
 *
 * This routine is used to set hardware flow control state.
 *
 * The state bit HFLOWOUT indicates that we output hardware flow control.
 * In other words, we assert RTS to start up input, and clear RTS to stop
 * input.
 *
 * The state bit HFLOWIN indicates that we input hardware flow control. 
 * In other words, if CTS is asserted, we will output characters. If
 * CTS is clear, we will stop output.
 */

static void AUX_setflow (tt_soc *tty)
{
    /*
     * Unconditionally set RTS here. Reason: if hardware flow control
     * is disabled, we want to assert RTS. If hardware flow control is
     * enabled, then by setting RTS we assume that there is room for input -
     * if our assumption is wrong, the interrupt service routine will fix it.
     */

    uartaddr->b.omr |= OMR_ASSERT_RTS; /* Set RTS */

    /*
     * Set of handing of CTS based on hardware flow control state
     */

    if (tty->capabilities & HFLOWIN)
      uartaddr->b.tpr |= TPR_CTS_ENABLE_TX;
    else
      uartaddr->b.tpr &= ~TPR_CTS_ENABLE_TX;
}

/*
 * AUX_tx_disable - Stop pending output
 *
 * This routine is used to stop pending output.
 */

static void AUX_tx_disable (tt_soc *tty)
{
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE); /* Lock out terminal interrupts */

    uartaddr->b.ier &= ~IER_TX_RDY; /* Disable transmit interrupts */

    reset_interrupt_level(SR);		/* Restore interrupts */
}

/*
 * AUX_modemsignal - Return the current state of modem signals
 */

static modem_status AUX_modemsignal (tt_soc *tty)
{
    modem_status summary = { 0 };	/* Returned status */

    if (uartaddr->b.ictsr & ICTSR_DCD)
	summary.ring = TRUE;	/* If RING (CD) is TRUE, indicate so */

    if (uartaddr->b.ictsr & ICTSR_CTS)
	summary.cts = TRUE;	/* If CTS is TRUE, indicate so */

    if (uartaddr->b.omr & OMR_ASSERT_DTR)
	summary.dtr = TRUE;	/* IF DTR is TRUE, indicate so */

    if (uartaddr->b.omr & OMR_ASSERT_RTS)
	summary.rts = TRUE;	/* IF RTS is TRUE, indicate so */

    return(summary);		/* Return the summary */
}

/*
 * AUX_modemchange - Indicate whether modem control signals have changed
 *
 * This routine is used to determine whether the modem control signals
 * have changed. In the case of the RING signal, we just return the
 * current state, since the hardware does not track changes on RING.
 */

static modem_status AUX_modemchange (tt_soc *tty)
{
    modem_status summary = { 0 };
    uchar status;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);	/* Lock out interrupts */

    status = uartaddr->b.ictsr;	/* Read the register */
    uartaddr->b.ictsr = status;	/* And write back the bits we've read */

    reset_interrupt_level(SR);		/* Enable interrupts */

    if (status & ICTSR_DDCD)
      summary.ring = TRUE;	/* If RING (CD) has changed, indicate so */

    if (status & ICTSR_DCTS)	/* If CTS has changed, indicate so */
      summary.cts = TRUE;

    return(summary);		/* Return the value */
}

/*
 * AUX_portdisable - Disable a port
 *
 * This routine is used to disable the AUX port. We will disable
 * both transmit and receive interrupts.
 */

static void AUX_portdisable (tt_soc *tty)
{
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);

    uartaddr->b.ier = 0;	/* Disable all interrupt sources */

    reset_interrupt_level(SR);
}

/*
 * AUX_portenable
 *
 * This routine is used to enable the AUX port. We enable interrupts.
 */

static void AUX_portenable (tt_soc *tty)
{
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);

    uartaddr->b.ier = IER_RX_RDY | IER_RSR_54 | IER_RSR_10;
    if (tty->capabilities2 & PPS_CLOCK_2) { /* Picking up PPS pulses? */
	uartaddr->b.ier |= IER_DCD_CTS;	/* Yep. */
    }

    reset_interrupt_level(SR);		/* Enable interrupts */
}

/*
 * AUX_writemode
 * Write Mode Register 1 and 2.
 * Default values are written if any arguments are bogus.
 * (this routine ported from tty2681.c code for STP)
 */
static void AUX_writemode (tt_soc *tty, int stopbits, int databits, int parity)
{

    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE); /* Lock out terminal interrupts */

    switch (stopbits) {
	default:
	case STOPBITS2:
	    tty->tty_stopbits = STOPBITS2;
	    stopbits = TPR_2_STOP_BITS;
	    break;
	case STOPBITS15:
	    tty->tty_stopbits = stopbits;
	    stopbits = TPR_15_STOP_BITS;
	    break;
	case STOPBITS1:
	    tty->tty_stopbits = stopbits;
	    stopbits = TPR_1_STOP_BIT;
	    break;
    }

    databits = databits - 5;
    if ((databits < TPR_5_DATA_BITS) || (databits > TPR_8_DATA_BITS))
	databits = TPR_8_DATA_BITS;
    tty->tty_databits = databits+5;

    switch (parity) {
      default:
      case PARITY_NO:
	tty->tty_parity = PARITY_NO;
	parity = CMR1_PARITY_OFF;
	break;
      case PARITY_ODD:
	tty->tty_parity = parity;
	parity = CMR1_PARITY_ON + CMR1_ODD_PARITY;
	break;
      case PARITY_EVEN:
	tty->tty_parity = parity;
	parity = CMR1_PARITY_ON + CMR1_EVEN_PARITY;
	break;
      case PARITY_SPACE:
	tty->tty_parity = parity;
	parity = CMR1_FORCE_PARITY + CMR1_EVEN_PARITY;
	break;
      case PARITY_MARK: 
	tty->tty_parity = parity;
	parity = CMR1_FORCE_PARITY + CMR1_ODD_PARITY; 
	break;
    }

    /*
     * Set UART channel B:
     */

    uartaddr->b.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_DISABLE;

    uartaddr->b.cmr1 = CMR1_ASYNC | parity;
    uartaddr->b.tpr = (stopbits | databits);
    uartaddr->b.rpr = databits;

    /*
     * Set handling of CTS based on hardware flow control state
     */

    if (tty->capabilities & HFLOWIN)
      uartaddr->b.tpr |= TPR_CTS_ENABLE_TX;
    else
      uartaddr->b.tpr &= ~TPR_CTS_ENABLE_TX;

    uartaddr->b.ccr = CCR_TCMD | CCR_RESET;
    uartaddr->b.ccr = CCR_RCMD | CCR_RESET;

    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_ENABLE;

    reset_interrupt_level(SR);		/* Restore interrupts */

}

/*
 * AUX_stopbits
 * Set stop bits for a line
 */
static void AUX_stopbits (tt_soc *tty, int count)
{
    if (count != tty->tty_stopbits) {
	AUX_writemode(tty, count, tty->tty_databits, tty->tty_parity);
    }
}

/*
 * AUX_parity
 * Set parity handling for a line
 */
static void AUX_parity (tt_soc *tty, int count)
{
    if (count != tty->tty_parity) {
	AUX_writemode(tty, tty->tty_stopbits, tty->tty_databits, count);
    }
}

/*
 * AUX_databits
 * Set 5, 6, 7, or 8 bits for a line
 */
static void AUX_databits (tt_soc *tty, int count)
{
    if (count != tty->tty_databits) {
	AUX_writemode(tty, tty->tty_stopbits, count, tty->tty_parity);
    }
}


#ifdef DEBUG
static void sig_dump_registers (struct regpair *regs)
{
    buginf(" cmr1=0x%02x, cmr2=0x%02x, s1r=0x%02x, s2r=0x%02x\n",
	   regs->cmr1, regs->cmr2, regs->s1r, regs->s2r);
    buginf(" tpr=0x%02x, ttr=0x%02x, rpr=0x%02x, rtr=0x%02x\n",
	   regs->tpr, regs->ttr, regs->rpr, regs->rtr);
    buginf(" ctprh=0x%02x, ctprl=0x%02x, ctcr=0x%02x, omr=0x%02x\n",
	   regs->ctprh, regs->ctprl, regs->ctcr, regs->omr);
    buginf(" cth=0x%02x, ctl=0x%02x, pcr=0x%02x, ccr=0x%02x\n",
	   regs->cth, regs->ctl, regs->pcr, regs->ccr);
    buginf(" rsr=0x%02x, trsr=0x%02x, ictsr=0x%02x, gsr=0x%02x\n",
	   regs->rsr, regs->trsr, regs->ictsr, regs->gsr);
    buginf(" ier=0x%02x, ivr=0x%02x, icr=0x%02x\n",
	   regs->ier, regs->ivr, regs->icr);
}

static void sig_a_dump_regs (void)
{
    buginf("register dump for port A (Console):\n");
    sig_dump_registers(&uartaddr->a);
}

static void sig_b_dump_regs (void)
{
    buginf("register dump for port B (Auxiliary):\n");
    sig_dump_registers(&uartaddr->b);
}

#endif


/*
 * csc2_breakservice - Dummy txservice handler for sending breaks
 */
static uint csc2_breakservice (tt_soc *tty, uchar *p, int len)
{
    /* Transmit nulls during break interval */
    p[0] = '\0';
    return(1);
}

/*
 * AUX_sendbreak - Send a break to the AUX port
 */
static void AUX_sendbreak (tt_soc *tty)
{
    if (CTY_LINE == tty->type)
      return;	                   /* Not allowed on console */

    tty->statbits |= SENDINGBREAK; /* Mark we are sending break */
    AUX_tx_disable(tty);           /* Disable tx interrupts */
    tty->txservice = csc2_breakservice; /* Set a dummy ISR to be safe */

    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;  /* Enable Xmitter */
    uartaddr->b.ccr = CCR_TCMD | CCR_TBREAK;  /* Force Break */

    process_sleep_for(ONESEC/2);              /* Hold Break for 0.5 sec */

    uartaddr->b.ccr = CCR_TCMD | CCR_RESET;   /* Reset Xmitter, Status */
    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;  /* Re-enable Xmitter */

    tty->txservice = NULL;          /* Disable transmit hook */
    AUX_startoutput(tty);           /* Re-enable tx interrupts */
    tty->statbits &= ~SENDINGBREAK; /* No longer sending break */
}


/*
 * console_init - Set up Port A as the system console
 */
				
tt_soc *console_init (void)
{
    register vectortype *vector;
    register tt_soc *tty;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);
    vector = &sig_a_vector;
    vector_init(vector);

    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput; 
    vector->vinputpending = serial_inputpending;

    vector->vputc = serial_putc;
    vector->vstartoutput = console_startoutput;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->vputpkt = generic_putpkt;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;

    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;

    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS); /* set up buffering */ 
    /*
     * This may be a lie (the rom monitor has control), but initialize it
     * to the default anyway so that NV generation doesn't get confused.
     */
    tty->tty_stopbits = STOPBITS2;

    /*
     * We're using vectored interrupts.  If we ever get to the level 5
     * dispatch, something has gone wrong.
     */
    createlevel (LEVEL_CONSOLE, NULL, "Console UART");

    /*
     * Initialize interrupt vectors for channel A:
     */

    exceptionHandler(UARTIRQ1, sig_a_receive);	/* Receiver ready */
    exceptionHandler(UARTIRQ2, sig_a_transmit); /* Transmitter ready */
    exceptionHandler(UARTIRQ3, sig_a_status); /* Rx/Tx status change */
    exceptionHandler(UARTIRQ4, sig_a_spurious); /* Counter/Timer event */

    /*
     * Initialize UART channel A:
     */
    uartaddr->a.ctcr = 0;	/* Make sure the counter/timer is disabled */
    uartaddr->a.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->a.ccr = CCR_RCMD | CCR_DISABLE;
    uartaddr->a.ivr = UARTIRQ1;
    uartaddr->a.ier = IER_RX_RDY | IER_RSR_54 | IER_RSR_10;
    uartaddr->a.icr = ICR_INTERLEAVE_A | ICR_VSTATUS | ICR_A_ENABLE;
    uartaddr->a.ccr = CCR_TCMD | CCR_RESET;
    uartaddr->a.ccr = CCR_RCMD | CCR_RESET;
    uartaddr->a.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->a.ccr = CCR_RCMD | CCR_ENABLE;
    reset_interrupt_level(SR);
    return(tty);
}

/*
 * auxline_init - initialize port B of console UART:
 */

tt_soc *auxline_init (void)
{
    register vectortype *vector;
    register tt_soc *tty;
    register int i;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);
    vector = &sig_b_vector;
    vector_init(vector);

    vector->vntpsetup = hes_ntpsetup;
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = AUX_startoutput;
    vector->vstartinput = AUX_startinput;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->vsetspeed = AUX_baudset;
    vector->vautosetspeed = async_autosetspeed;
    vector->vauto_baud = async_autobaud;
    vector->vstopbits = AUX_stopbits;
    vector->vparitybits = AUX_parity;
    vector->vdatabits = AUX_databits;
    vector->vdtrstate = AUX_dtrstate;
    vector->vsetflow = AUX_setflow;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vputpkt = generic_putpkt;
    vector->vstopoutput = AUX_tx_disable;
    vector->vmodemsignal = AUX_modemsignal;
    vector->vmodemchange = AUX_modemchange;
    vector->vportdisable = AUX_portdisable;
    vector->vportenable = AUX_portenable;
    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;
    vector->vsendbreak = AUX_sendbreak;

    tty = (tt_soc *) tty_init(AuxBase,READY,AUX_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS); /* set up buffering */ 

    tty->tty_txspeed = ASYNC_DEFSPEED;
    tty->tty_rxspeed = ASYNC_DEFSPEED;
    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->modem_type = MODEMTYPE_LOCAL; /* No modem control initially */

    /*
     * Initialize interrupt vectors for UART channel B:
     */
    exceptionHandler(UARTIRQ5, sig_b_receive);	/* Receiver ready */
    exceptionHandler(UARTIRQ6, sig_b_transmit);	/* Transmitter ready */
    exceptionHandler(UARTIRQ7, sig_b_status);	/* Rx/Tx status change */
    exceptionHandler(UARTIRQ8, sig_b_spurious);	/* Counter/Timer event */

    /*
     * Initialize UART channel B:
     */
    uartaddr->b.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_DISABLE;

    uartaddr->b.cmr1 = CMR1_ASYNC;
    uartaddr->b.tpr = TPR_2_STOP_BITS | TPR_8_DATA_BITS;
    uartaddr->b.ttr = TTR_CLOCKBRG | TTR_9600_BAUD;
    uartaddr->b.rpr = RPR_8_DATA_BITS;
    uartaddr->b.rtr = RTR_CLOCKBRG | TTR_9600_BAUD;

    /*
     * Due to a "feature" of some versions of the SCC68562, we must implement
     * a software workaround to clear any possible 'transmit break' condition
     * which may be set after a hardware reset. See the Signetics "Description
     * of Device Anomaly", SCN68562 - No Rev., March 4, 1987.
     */
    uartaddr->b.cmr2 = CMR2_LOOP | CMR2_POLLED;
    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->b.ccr = CCR_TCMD | CCR_TBREAK;
    for (i = 0; i < 1000000; i++)
	if (uartaddr->b.trsr & TRSR_BREAK_ACK)
	    break;
    if (i >= 1000000) {
	errmsg(&msgsym(BREAK, CSC2));
	uartaddr->b.ccr = CCR_TCMD | CCR_DISABLE;
	uartaddr->b.ccr = CCR_RCMD | CCR_DISABLE;
	reset_interrupt_level(SR);
	return((tt_soc *)ERROR);
    }
    uartaddr->b.ccr = CCR_TCMD | CCR_DISABLE;
    uartaddr->b.cmr2 = CMR2_POLLED;

    uartaddr->b.pcr = PCR_RTS_NOT_SYNOUT;

    uartaddr->b.omr = OMR_ASSERT_RTS | OMR_ASSERT_DTR | OMR_TXRDY_FIFO_EMPTY;
    uartaddr->b.ctcr = 0;	/* Make sure the counter/timer is disabled */

    uartaddr->b.ier = IER_RX_RDY | IER_RSR_54 | IER_RSR_10;
    uartaddr->a.icr |= ICR_B_ENABLE;
    uartaddr->b.ccr = CCR_TCMD | CCR_RESET;
    uartaddr->b.ccr = CCR_RCMD | CCR_RESET;
    uartaddr->b.ccr = CCR_TCMD | CCR_ENABLE;
    uartaddr->b.ccr = CCR_RCMD | CCR_ENABLE;

    AUX_dtrstate (tty, TRUE);

    reset_interrupt_level(SR);
    return(tty);
}
