/* $Id: les_console_alc.c,v 3.2.62.2 1996/09/10 03:22:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/les_console_alc.c,v $
 *------------------------------------------------------------------
 * les_console.c -- simple device driver for the SCN2681 DUART
 * 
 * October 19 1989, Chris Shaker, adapted from sts1_console.c
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_console_alc.c,v $
 * Revision 3.2.62.2  1996/09/10  03:22:24  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/09  14:34:11  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:15:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  01:59:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/03/23  04:54:33  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:40:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:00  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:05  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:49:21  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:41:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.34.1  1996/02/03  07:07:38  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.2  1995/08/31  12:51:18  rlowe
 * Carryover applicable code from old Synergy_ATMlc_branch (10.3)
 * to Synalc2_branch (11.0).  Add SYNALC-specific entries to
 * other (les) platform-specific entries.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.2.1  1995/08/23  02:00:33  lcheung
 * Initial commit
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/23  01:58:14  lcheung
 * placholder file  for Synalc2_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config_register.h"
#include "sys_registry.h"
#include "../les/les_console_alc.h"
#include "ttysrv.h"
#include "stacks.h"
#include "../os/async.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../les/if_les.h"
#include "../../wbu/cat5k/alc_include/mfp_hw_defs.h"

/*
 * External declarations:
 */
extern void createlevel_ALC (int level, void (*routine) (void), char *string,
				long VecNum, long IntrIndex) ;

/*
 * Local storage:
 */
vectortype console_vector;
volatile boolean	Synalc_BrgTx = FALSE ;

/*
 * Map linear treatment of console+aux0..n onto AuxBase
 */
#define PANMODEM(x) ((x == 0) ? MODEMS[0] : MODEMS[(x-1) + AuxBase])

/*
 * pan_console_interrupt - Handle interrupts for console and aux line
 */

void pan_console_interrupt (void)
{
    ushort status;
    int c;
    uartaddress *uart;
    tt_soc *tty;
    int i;
    volatile uchar *data_ptr, *status_ptr, *statustx_ptr;

    if (Synalc_BrgTx)
    {
		/* Took a BRG interrupt to trigger Tx.  Now that */
		/* we are here, disable further BRG interrupts.  */
	MFP_MaskIntrW (MFP_BRG_TMR_BITMASK, TRUE) ;
	Synalc_BrgTx = FALSE ;
    }

    for (i = 0; i <= nauxlines; i++) { /* Loop for all regular lines */
	tty = PANMODEM(i);
	if (!tty)
	  break;		/* Line not set up yet */

	data_ptr = tty->uartdata;
	status_ptr = tty->uartstatus;
	statustx_ptr = status_ptr + 1 ;
	status = *status_ptr;

	if (status & OVRRNERROR) {
	    tty->overrun++;
	    uart = WHICHUART(tty->ttynum);
	    if (tty->ttynum == 0)
	    {
	      status |= RXREADY ;	/* Ensure do a read below, to clear. */
	    }
	}
	while (status & RXREADY) {

	    c = *data_ptr;

	    if (status & RCVBREAK) {	
		c = BREAK_CHAR;
	    }
	
	    tty->totalin++;

	    if (special_chars(tty,c) >= 0) {
		
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

		    tty->statbits |= SENDXOFF;
		    startoutput(tty);
		}
		/* so there is room  */
		*tty->intail++ = c;
		++tty->incount;
	    }
		/* Clear any pending UART Rx interrupts. */
	    MFP_ClrPendIntr ((MFP_IMSK_RXFL | MFP_IMSK_RXER), FALSE) ;
	    status = *status_ptr;
	}			
	status = *statustx_ptr;		/* Get tsr instead of rsr. */
	if (status & TXREADY) {

	    /*
	     * If the line is set to send flow control, do so now
	     */

	    if (tty->statbits & SENDXOFF) {
		if (tty->capabilities & SFLOWOUT) {
		    *data_ptr = tty->tty_stopchar;
		    tty->statbits |= SENTXOFF;
			/* Clear any pending UART Tx interrupts. */
		    MFP_ClrPendIntr ((MFP_IMSK_TXBE | MFP_IMSK_TXER), FALSE) ;

		    status = *statustx_ptr;		/* Read tsr, not rsr. */
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
			/* Clear any pending UART Tx interrupts. */
		    MFP_ClrPendIntr ((MFP_IMSK_TXBE | MFP_IMSK_TXER), FALSE) ;

		    status = *statustx_ptr;		/* Read tsr, not rsr. */
		} else {
		    (*(tty->txfinish))(tty);
		    break;
		}
	    }		
	}
    }

}

/*
 * pan_writemode
 * Write Mode Register 1 and 2.
 * Default values are written if any arguments are bogus.
 * (this routine ported from tty2681.c code for STP)
 */
static void pan_writemode (tt_soc *tty, int stopbits, int databits, int parity)
{
    return ;

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
    return(0);			/* No aux port available for SYNALC. */

}

void pan_tx_disable (
     register tt_soc *tty)
{
    if (tty->ttynum == 0)
    {
	uartaddr->a.tsr = TXDISABLE;

	if (Synalc_BrgTx)
	{
		leveltype	SR ;

		/* Wanted BRG interrupt to trigger Tx.  Now that */
		/* we are here, disable further BRG interrupts.  */

		SR = raise_interrupt_level (TTY_DISABLE) ;
		MFP_MaskIntrW (MFP_BRG_TMR_BITMASK, TRUE) ;
		Synalc_BrgTx = FALSE ;
		reset_interrupt_level (SR) ;
	}
    }
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
     * Unsupported (for now).
     */
    reset_interrupt_level(SR);
    return(FALSE);
}

/*
 * pan_aux_dtr_state - Assert or clear DTR on auxiliary port
 */

void pan_aux_dtr_state (
    register tt_soc *tty,
    boolean sense)
{
    return ;

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
    modem_status summary;	/* Returned status */

    summary.cts  = FALSE ;
    summary.ring = FALSE ;

    return(summary);
}

/*
 * pan_print_modemctrl
 * print the states of the modem control input bits (AUX line only)
 *
 */

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
    {
	uartaddr->a.rsr = RXDISABLE ;
    }
}

/*
 * pan_portenable - Enable a port
 */

void pan_portenable (
     tt_soc *tty)
{
    if (tty->ttynum == 0)
    {
	leveltype	SR ;

		/* ------------------------------------------------------ */
		/* For 2681 (traditional code), enabling Tx will cause    */
		/* an interrupt.  This will cause Tx of next queued char. */
		/* For MFP, enabling Tx will NOT force a Tx interrupt.    */
		/* So, to keep similar upper-level logic, we must force   */
		/* a Tx (done) interrupt here ...                         */
		/* (2681 is level-triggered, MFP is edge triggered intrs.)*/
		/* ------------------------------------------------------ */
		/* Do this by use of the MFP Timer-D, which is the Baud   */
		/* Rate Generator.  Normally this interrupt is NOT used,  */
		/* but if we bind it to (a wrapper to) the UART interrupt */
		/* handler we can emulate the initial 2681 Tx interrupt.  */
		/* Enable the BRG interrupt AFTER priming the UART.       */
		/* BRG ISR/wrapper must disable BRG interrupts again.     */
		/* ------------------------------------------------------ */
		/* Keep in mind Tx may ALREADY be enabled and running !!  */
		/* ------------------------------------------------------ */

	uartaddr->a.rsr = STP_RXENABLE ;
	uartaddr->a.tsr = STP_TXENABLE ;

		/* BRG interrupts can happen right away, so block them  */
		/* until we can set our flag for UART ISR to use.  This */
		/* is also a critical section since ISR can clear flag  */
		/* if we set it before unmasking, giving same result.   */

	SR = raise_interrupt_level (TTY_DISABLE) ;
	MFP_UnmaskIntrW (MFP_BRG_TMR_BITMASK, TRUE, FALSE) ;
	Synalc_BrgTx = TRUE ;
	reset_interrupt_level (SR) ;
    }
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
    return ;

}

/*
 * pan_setflow - Set up hardware flow control
 */

void pan_setflow (
     tt_soc *tty)
{
    return ;
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
    return ;

}

/*
 * console_putc - Write a byte to the console, using polled I/O
 */

void console_putc (uchar ch)
{
    leveltype status;		/* Saved interrupt level */

    if (ch == '\n')
      console_putc('\r');	/* Preface NL with CR */

    status = raise_interrupt_level(TTY_DISABLE); /* Lock out interrupts */

    uartaddr->a.tsr = STP_TXENABLE; /* Ensure transmitter is enabled */

    while (!(uartaddr->a.tsr & TXREADY))
    	PUNCH_WD ;			/* Wait for transmitter ready */

    uartaddr->dataA = ch;	/* Store the character */

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

    uartaddr->a.rsr = STP_RXENABLE; /* Ensure receiver is enabled */

    while (!(uartaddr->a.rsr & RXREADY))
      PUNCH_WD ;				/* Loop for a character */

    ch = uartaddr->dataA;	/* Get the character */

    reset_interrupt_level(status); /* Restore interrupts */

    return(ch & 0x7f);		/* Return the character trimmed */
}

/*
 * console_debug_state - Set console debugging state
 *
 * This routine is called by the system debugger on the way in and out of
 * debugger state.
 *
 * This driver does not do any special processing at this point, so we
 * can simply return. 
 */

void console_debug_state (boolean newstate)
{
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

    SR = raise_interrupt_level(TTY_DISABLE);

    /*
     * Initialize the vector pointers
     */

    vector = &console_vector;
    vector_init(vector);

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

    /*
     * Set up the TTY data structure
     */

    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty, MAXINCHARS, MAXOUTCHARS);
    tty->uartdata = &uartaddr->dataA;
    tty->uartstatus = &uartaddr->a.rsr;
    tty->txfinish = pan_tx_disable;
    tty->tty_stopbits = STOPBITS2;

    createlevel_ALC (LEVEL_CONSOLE, pan_console_interrupt,
    			"Console Uart", CONSOLE_IVECTOR, ALC_INTR_CONS);

	/* To trigger Tx interrupts, use BRG to force an initial interrupt. */
	/* We use that interrupt to kick-off Tx service at interrupt level. */
    createlevel_ALC (BRG_INTLEVEL, pan_console_interrupt,
    			"Uart BRG", UARTBRG_IVECTOR, ALC_INTR_BRG);


    /*
     * Initialize the PORT A portion of the UART
     */

    uartaddr->a.rsr = STP_RXENABLE ;
    uartaddr->a.tsr = STP_TXENABLE ;
    tty->modem_bits &= ~(255);		/* clear saved modem state. */
    tty->modem_bits |= MODEMBIT_DTR;	/* Update saved modem state. */

    /*
     * Enable interrupts
     */

	/* Enable BRG/pseudo-Tx interrupt, to be similar to 2681 operation. */
	/* Disable separate MFP Rx/Tx error interrupt sources; check for    */
	/* errors with normal MFP UART Rx/Tx status checks.                 */
    MFP_MaskIntrW ((MFP_IMSK_RXER | MFP_IMSK_TXER), TRUE) ;
    MFP_UnmaskIntrW ((MFP_IMSK_RXFL | MFP_IMSK_TXBE | MFP_BRG_TMR_BITMASK),
								TRUE, TRUE) ;
    Synalc_BrgTx = TRUE ;

    reset_interrupt_level(SR);
    return(tty);
}

/*
 * auxline_init
 * Initialize port B of console UART.
 */

tt_soc *auxline_init (void)
{
    return(NULL);

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

/*
 * This code is a near-copy of that in ../../boot/src-68-alc/refresh.c.
 */
int
console_check_break(void)
{
    volatile u_char		i;
    register uartaddress *	ua ;
    register s_int16		Count ;
    int				RC ;

    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    ua    = uartaddr ;
    Count = MFP_UART_RXDEPTH + 1 ;
    RC    = 0 ;

    do {
	if (ua->a.rsr & RCVBREAK)
	{
		if ( !(configregister & CFG_BREAK) || 
		     (CLOCK_IN_STARTUP_INTERVAL (5*ONESEC)) )
		{
			brk = 1;
			MFP_ClrPendIntr ((MFP_IMSK_RXFL | MFP_IMSK_RXER),
									FALSE) ;
						/* Clear any BREAK interrupt. */
			i = uartaddr->dataA ;	/* Drain uart. */
		}
	}
	else  if (brk)		/* No longer BREAK detected at uart. */
	{
		RC  = 1 ;
		brk = 0 ;
		/* Any Rx char now valid, do NOT empty uart. */
	}
	else	break ;		/* No BREAK condition to de-glitch. */

    } while (-- Count > 0) ;

    return (RC) ;
}
