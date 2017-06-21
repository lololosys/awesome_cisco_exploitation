/* $Id: tty2681.c,v 3.6.28.1 1996/09/10 03:22:56 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/tty2681.c,v $
 *------------------------------------------------------------------
 * tty2681.c -- common routines for STP and STS uart drivers.
 *  
 * December 1988   Bill Westfield
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tty2681.c,v $
 * Revision 3.6.28.1  1996/09/10  03:22:56  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.6  1996/01/22  07:30:00  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/16  12:00:58  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 *
 * Revision 3.4  1995/12/15  23:01:40  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 * and a in statbits, and maybe an instruction or two as well!
 *
 * Revision 3.3  1995/11/17  18:56:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/16  18:02:15  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.3  1995/08/25  11:45:32  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.2  1995/06/09 13:22:11  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:10:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "packet.h"
#include "stp.h"
#include "../ts/s2681.h"
#include "../os/async.h"

/* forward declarations */
static boolean STP_baudset(tt_soc *tty, int txbaud, int rxbaud);
static boolean STP_startoutput(tt_soc *tty);
static void STP_databits(tt_soc *tty, int count);
static void STP_parity(tt_soc *tty, int type);
static void STP_setflow(tt_soc *tty);
static void STP_dtrstate(tt_soc *tty, boolean sense);
static void STP_sendbreak(tt_soc *tty);
static void STP_clearoutput(tt_soc *tty);
static void STP_putpktdone(tt_soc *tty);
static modem_status STP_modemsignal(tt_soc *);
static modem_status STP_modemchange(tt_soc *);
static int STP_putpkt(tt_soc *, paktype *, uchar *, int);
static void STP_putc(tt_soc *, int);
static void STP_txenable(tt_soc *);
static void STP_rxdisable(tt_soc *);
static void STP_stopbits(tt_soc *tty, int count);
static void stp_synctx(void);
static  void stp_setsflow(tt_soc *);

/*
 * STP specific storage
 */
sys_timestamp lasttxsync;	/* next time to syncronize outputs */
uchar uartstatus[PLATFORM_MAXLINES/2]; /* Which interrupts are enabled (used)*/
vectortype STPvector;		/* terminal dispatch vectors */

/*
 * STP_vector_init - initialize the STP vectors
 */

void STP_vector_init (void)
{
    register vectortype *vector;

    vector = &STPvector;
    vector_init(vector);

    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vinputpending = serial_inputpending;
    vector->vputc = STP_putc;
    vector->vputpkt = STP_putpkt;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;

    vector->vclearoutput = STP_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->vstartoutput = STP_startoutput;
    vector->vsetspeed = STP_baudset;
    vector->vautosetspeed = async_autosetspeed;
    vector->vauto_baud = async_autobaud;
    vector->vstopbits = STP_stopbits;
    vector->vparitybits = STP_parity;
    vector->vdatabits = STP_databits;
    vector->vsendbreak = STP_sendbreak;
    vector->vdtrstate = STP_dtrstate;
    vector->vsetflow = STP_setflow;
    vector->vsetsflow = stp_setsflow;
    vector->vportenable = STP_txenable;
    vector->vportdisable = STP_rxdisable;
    vector->vmodemsignal = STP_modemsignal;
    vector->vmodemchange = STP_modemchange;
    vector->vservicehook = generic_servicehook;
    vector->vnoservicehook = generic_noservicehook;
    vector->vstopoutput = STP_txdisable;
}
/*
 * STP_putpkt
 * set interrupt pointers and output a packet
 */

static int STP_putpkt (tt_soc *tty, paktype *pak, uchar *datastart, int length)
{
    leveltype status;

    if ((length < 10) || tty->current.padding_ptr)
	return(generic_putpkt(tty, pak, datastart, length));

    if (tty->outcount != 0) {
	if (startoutput(tty))
	  tty_dallytimer(tty);
	return(0);			/* Do nothing if I/O pending */
    }

    if (tty->outpak)
      return(0);			/* Only one packet at a time */

    if (tty->statbits & (CARDROP|IDLE)) {
	return(length);			/* Say we did the whole string */
    }
	
    pak_lock(pak);			/* Place a lock on the packet */

    status = raise_interrupt_level(TTY_DISABLE); /* Stop interrupts */

    tty->outhead = datastart-1;		/* Set pointer */
    tty->outcount = length;		/* set length */
    tty->outtail = NULL;		/* say tail is invalid */
    tty->outend = datastart + length; 	/* set end pointer */
    tty->outpak = pak;			/* Set packet pointer */
    tty->txfinish = STP_putpktdone;	/* Set the transmit done routine */
    reset_interrupt_level(status);	/* Allow interrupts again */

    if (startoutput(tty))
      tty_dallytimer(tty);
    return(length);			/* Say we did the whole thing */
}

/*
 * STP_putpktdone - Interrupt completion handler for STP_putpkt
 */

static void STP_putpktdone (tt_soc *tty)
{
    paktype *pak = (paktype *)tty->outpak;

    if (pak) {
	tty->outpak = NULL;		/* Dump the pointer */
	datagram_done(pak);		/* Free the buffer */
    }
    serial_clearoutput(tty);		/* Fix the buffer pointers */
    tty->txfinish = STP_txdisable;	/* Set regular disable routine */
    STP_txdisable(tty);			/* And finish up there */
}

/*
 * STP_putc
 * Output a character and possible padding on a serial line.
 */

static void STP_putc (tt_soc *tty, int c)
{
    register uchar *t, padding;

    if (c == BREAK_CHAR) {
	sendbreak(tty);			/* generate hardware break */
	return;
    }

    /* 
     *  Put the char in the buffer in the next open slot
     */
    t = tty->outtail;
    if (t++ == NULL) {			/* running putstring now ? */
	tty_dallytimer(tty);
	edisms((blockproc *)sputBLOCK, (ulong)tty);	/* wait for it to finish */
	serial_clearoutput(tty);	/* reset pointers */
	t = tty->outtail+1;
    }
    if (t == tty->outend)
	t = tty->outstart;

    /*
     *	Check for buffer full, or for overwriting.  Also start an idle
     *  timeout in case the hardware is busted.
     */
    if (t == tty->outhead) {
	if (TIMER_RUNNING_AND_AWAKE(lasttxsync))
	    stp_synctx();
	while (t == tty->outhead) {
	    tty_dallytimer(tty);
	    (void)startoutput(tty);	/* make sure output is started */
            edisms((blockproc *)sputBLOCK, (ulong)tty);
	    if (tty->statbits & (CARDROP|IDLE|IDLETIMEOUT))
	        return;
            t = tty->outtail + 1;	/* reset t in case of buffer clear */
            if (t == tty->outend)
	        t = tty->outstart;
	}
    }
    *t = (uchar) c;			/* store character */

    /* 
     * See if output has gone idle, possibly restart output.
     * If startoutput() fails, we have an unready modem.
     */
    if (tty->outcount == 0) {
 	tty->outcount++;
        if (!startoutput(tty)) {
	    tty->outcount = 0;
	    return;
	}
    }
    else
	tty->outcount++;
    tty->outtail = t;

    /*
     * We've sent the character.  Now check if we should be
     * sending padding characters after it.  Note that quick_putc
     * doesn't block and doesn't call serial_putc, eliminating a
     * number of hangs and recursion problems.  We accept the very
     * occasional problem of the buffer filling while sending pad
     * characters.
     */
    if (tty->current.padding_ptr) {
	padding = tty->current.padding_ptr[c & 0x7F];
	while (padding--) 
	    quick_putc(tty, '\0');
    }
}

/*
 * STP_clearoutput
 * Clear output on an STP line.  This is different that serial_clearoutput
 * since we may have to empty the uart if hardware flow control is in use.
 */

static void STP_clearoutput (tt_soc *tty)
{
    serial_clearoutput(tty);	/* first flush our buffers */
    if (tty->capabilities & HFLOWOUT) { /* might be characters in uart ? */
	uartaddress *uart = WHICHUART(tty->ttynum);

	if ((tty->ttynum & 1) )
	    uart->commandA = (RESETTX);
	else
	    uart->commandB = (RESETTX);
	STP_txenable(tty);
    }	
}

/*
 * STP_txdisable
 * Disable transmitter
 */

void STP_txdisable (tt_soc *tty)
{
    uartaddress *uart = WHICHUART(tty->ttynum);

    if ((tty->ttynum & 1) )
	uart->commandA = TXDISABLE;	/* disable transmission */
    else
	uart->commandB = TXDISABLE;	/* disable transmission */
}


/*
 * STP_txenable
 * Enable transmitter
 */

static void STP_txenable (tt_soc *tty)
{
    uartaddress *uart = WHICHUART(tty->ttynum);

    if ((tty->ttynum & 1) )
	uart->commandA = (STP_TXENABLE + STP_RXENABLE);
    else
	uart->commandB = (STP_TXENABLE + STP_RXENABLE);
}

/*
 * STP_rxdisable
 * Disable receiver
 */

static void STP_rxdisable (tt_soc *tty)
{
    uartaddress *uart = WHICHUART(tty->ttynum);

    if ((tty->ttynum & 1) )
	uart->commandA = RXDISABLE;
    else
	uart->commandB = RXDISABLE;
}

/*
 * STP_startoutput
 * Start output by forcing a transmit interrupt.
 * Will not do anything if the line is a modem and is not ready.
 */

static boolean STP_startoutput (tt_soc *tty)
{
    leveltype level = raise_interrupt_level(TTY_DISABLE);
    register boolean retval = TRUE;

    if (tty->statbits & (HOLDING | WANTXON)) {
        ;		/* Do nothing if output stopped */
    } else {
	if ((tty->capabilities & (MODEMIN+MODEMOUT)) == 0) {
	    STP_txenable(tty);
	} else {
	    /*
	     *	A modem. Can not interrupt if IDLE || RINGIN so ...
	     */
	    if (tty->statbits & READY) {
		STP_txenable(tty);
	    } else {
		retval = FALSE;
	    }
	}

    }
    reset_interrupt_level(level);
    return (retval);
}

/*
 * STP_writemode
 * Write Mode Register 1 and 2.
 * Default values are written if any arguments are bogus.
 */

static void STP_writemode (
    tt_soc *tty,
    int stopbits,
    int databits,
    int parity)
{
    uartaddress *uart;
    int flow;

    uart = WHICHUART(tty->ttynum);
    switch (stopbits) {
	default:
	case STOPBITS2: tty->tty_stopbits = STOPBITS2; stopbits = STOP32X16; break;
	case STOPBITS15: tty->tty_stopbits = stopbits; stopbits = STOP25X16; break;
	case STOPBITS1: tty->tty_stopbits = stopbits; stopbits = STOP16X16; break;
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
	 ((tty->capabilities & MODEMRI) == 0))
	flow = 0;
    else
	flow = (tty->capabilities & HFLOWIN) ? TXCTSCONTROL : 0;
    if ((tty->ttynum & 1) ) {
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
 * STP_stopbits
 * Set stop bits for an STP line
 */

static void STP_stopbits (tt_soc *tty, int count)
{
    if (count != tty->tty_stopbits) {
	STP_writemode(tty, count, tty->tty_databits, tty->tty_parity);
    }
}

/*
 * STP_databits
 * Set 5, 6, 7, or 8 bits for an STP line
 */

static void STP_databits (tt_soc *tty, int count)
{
    if (count != tty->tty_databits) {
	STP_writemode(tty, tty->tty_stopbits, count, tty->tty_parity);
    }
}

/*
 * STP_parity
 * Set parity handling for an STP line
 */

static void STP_parity (tt_soc *tty, int type)
{
    if (type != tty->tty_parity) {
	STP_writemode(tty, tty->tty_stopbits, tty->tty_databits, type);
    }
}

/*
 * STP_setflow
 * Set hardware dependent flow control characteristics
 */

static void STP_setflow (tt_soc *tty)
{
    STP_writemode(tty, tty->tty_stopbits, tty->tty_databits, tty->tty_parity);
}

/*
 * STP_dtrstate
 * Assert or clear DTR.
 */

static void STP_dtrstate (tt_soc *tty, boolean sense)
{
    register uartaddress *uart;
    register int dtrbits;

    uart = WHICHUART(tty->ttynum);
    dtrbits = ((tty->ttynum & 1) ) ? DTRA : DTRB;
    if (sense == TRUE) {
	uart->setport = dtrbits;
	tty->modem_bits |= MODEMBIT_DTR;	/* Update saved modem state */
    } else {
	uart->resetport = dtrbits;
	tty->modem_bits &= ~MODEMBIT_DTR;	/* Update saved modem state */
    }
}

/*
 * STP_baud2code
 * Convert a baud rate number to a code that the UART understands.
 * Returns -1 if baud rate illegal.
 *
 * We get 38.4Kbit when using Baud Rate Generator 1 by setting up the
 * Counter/Timer to generate 38.4Kbit when the "timer" baud rate is
 * specified.  This is accomplished by setting "48" in the counter/time
 * register and specifying crystal clock (1X) as the source.
 */

#define NBAUDS 13

const int baudtable[NBAUDS] = {
75, 110, 134, 150, 300, 600, 1200, 2000, 2400, 4800, 1800, 9600, 19200
};

const int xbaudtable[] = {57600, 38400, 28800, 23040, 16457, 14400, 12800, 11520};
const int xbaudtimer[] = {    2,     3,     4,     5,     7,     8,     9,    10};

int STP_baud2code (int baud)
{
    int code;

    for (code = 0; code < NBAUDS; code++) {
	if (baudtable[code] == baud)
	    return(code);
    }
    return(-1);
}

/*
 * STP_baud2timer - Convert baud rate to counter/timer rate
 */

static int STP_baud2timer (int baud)
{
    int i;

    for (i = 0; i < (sizeof(xbaudtable) / sizeof(int)); i++)
      if (xbaudtable[i] == baud)
	return(xbaudtimer[i]);

    return(0);			/* Speed not found */
}

/*
 * STP_baudset
 * Set the baud rate, enable TX and RX actions
 */

static boolean STP_baudset (tt_soc *tty, int txbaud, int rxbaud)
{
    int txcode, rxcode;
    tt_soc *other_tty;
    uartaddress *uart = WHICHUART(tty->ttynum);
    int timer = 0;

    other_tty = MODEMS[OTHERLINE(tty->ttynum)];
				/* Get other TTY on this UART */

    /*
     * Default baud rates if so desired
     */
    if (txbaud == -1)
	txbaud = tty->tty_txspeed;
    if (rxbaud == -1)
	rxbaud = tty->tty_rxspeed;

    /*
     * Don't touch the hardware if no change being made (prevents garbage)
     */

    if ((txbaud == tty->tty_txspeed) && (rxbaud == tty->tty_rxspeed)) {
	return(TRUE);
    }

    /*
     * Translate speeds to codes.  Change baud rates if codes are good.
     */
    txcode = STP_baud2code(txbaud);
    rxcode = STP_baud2code(rxbaud);

    /*
     * Handle non-standard transmit speeds
     */

    if (txcode == -1) {		/* Possible nonstandard speed? */
	timer = STP_baud2timer(txbaud);
	if (!timer)
	  return(FALSE);	/* Speed is unknown */

	if ((other_tty->tty_txspeed != txbaud &&
	     STP_baud2timer(other_tty->tty_txspeed)) ||
	    (other_tty->tty_rxspeed != txbaud &&
	     STP_baud2timer(other_tty->tty_rxspeed)))
	  return(FALSE);	/* Other speed already in use */

	txcode = 13;		/* OK, non-standard speed is OK */
    }

    /*
     * Handle non-standard receive speeds
     */

    if (rxcode == -1) {		/* Possible nonstandard speed? */
	timer = STP_baud2timer(rxbaud);
	if (!timer)
	  return(FALSE);	/* Speed is unknown */

	if ((other_tty->tty_txspeed != rxbaud &&
	     STP_baud2timer(other_tty->tty_txspeed)) ||
	    (other_tty->tty_rxspeed != rxbaud &&
	     STP_baud2timer(other_tty->tty_rxspeed)))
	  return(FALSE);	/* Other speed already in use */

	rxcode = 13;		/* OK, non-standard speed is OK */
    }

    if ((txcode == 13 && rxcode == 13) && (txbaud != rxbaud))
      return(FALSE);		/* Only one programmable speed per line */

    tty->tty_txspeed = txbaud;
    tty->tty_rxspeed = rxbaud;

    if (timer) {
	uart->CTUpper = 0;
	uart->CTLower = timer;	/* Set the baud timer */
    }

    if ((tty->ttynum & 1) ) {
	uart->statusA = (RXCLOCK * rxcode + TXCLOCK * txcode);
	uart->commandA = (STP_TXENABLE + STP_RXENABLE);
    } else {
	uart->statusB = (RXCLOCK * rxcode + TXCLOCK * txcode);
	uart->commandB = (STP_TXENABLE + STP_RXENABLE);
    }
    return(TRUE);
}

/*
 * STP_sendbreak
 * Send a 500 ms break sequence on this line
 */

static uint STP_breakservice (tt_soc *tty, uchar *p, int len)
{
    /*
     * Transmit nulls during the break interval
     */
    p[0] = '\0';
    return(1);
}


static void STP_sendbreak (tt_soc *tty)
{
    int uartnumber = (tty->ttynum - STPBase) >> 1;
    uartaddress *uart = WHICHUART(tty->ttynum);

    if ((tty->ttynum & 1)) {
	uartstatus[uartnumber] &= ~TXRDYA;
	uart->interrupt = uartstatus[uartnumber];
	tty->statbits |= SENDINGBREAK;
	tty->txservice = STP_breakservice;
	uart->commandA = STP_TXENABLE;
	uart->commandA = STARTBREAK;
	process_sleep_for(ONESEC/2);
	uart->commandA = STOPBREAK;
	tty->txservice = NULL;
	uartstatus[uartnumber] |= TXRDYA;
	uart->interrupt = uartstatus[uartnumber];
	tty->statbits &= ~SENDINGBREAK;
    } else {
	uartstatus[uartnumber] &= ~TXRDYB;
	uart->interrupt = uartstatus[uartnumber];
	tty->txservice = STP_breakservice;
	tty->statbits |= SENDINGBREAK;
	uart->commandB = STP_TXENABLE;
	uart->commandB = STARTBREAK;
	process_sleep_for(ONESEC/2);
	tty->txservice = NULL;
	uart->commandB = STOPBREAK;
	uartstatus[uartnumber] |= TXRDYB;
	uart->interrupt = uartstatus[uartnumber];
	tty->statbits &= ~SENDINGBREAK;
    }
}

/*
 * stp_synctx
 * Attempt to cause STP TX interrupts to all happen at the same time.
 * When a large number of lines are all doing output at high speed at
 * the same time (eg to printers), this is significantly better than
 * having the interrupts occur at random times with respect to one
 * another.  To achieve this, we stop listening to STP TX interrupts
 * for a time larger than a single character time, and all the pending
 * interrupts collect until we turn them on again.
 */

static void stp_synctx (void)
{
    uartaddress *limit = (UARTSTART + ((nSTPlines) >> 1));
    uartaddress *uart;
    leveltype status;
	
    for (uart = UARTSTART; uart <= limit; uart++)
	uart->interrupt = (RXRDYB + RXRDYA);
    DELAY(1);			/* wait one millisecond */
    status = raise_interrupt_level(TTY_DISABLE);
    for (uart = UARTSTART; uart <= limit; uart++)
	uart->interrupt = (TXRDYA + TXRDYB + RXRDYB + RXRDYA);
    reset_interrupt_level(status);
    if (TIMER_RUNNING(lasttxsync))
	TIMER_START(lasttxsync, 3*ONESEC);
}

/*
 * stp_setsflow
 * Turn flow control on or off.  Mostly, we have to check whether we are
 * in the middle of negotiation when state changes (it won't do to stop
 * recognizing XON while we are XOFFed.)
 * If we ever use a smart uart, it might need to know.
 */
static void stp_setsflow (tt_soc *tty)
{
    if (tty->statbits & WANTXON)	/* waiting for XON ? */
	if ((tty->capabilities & SFLOWIN) == 0) { /* flow control OFF? */
	    tty->txservice = NULL;	/* make sure output is started ! */
	    tty->statbits &= ~WANTXON;
	    STP_txenable(tty);
	}
}

/*
 * Return the current modem control signals
 */

static modem_status STP_modemsignal (tt_soc *tty)
{
    uartaddress *uart;
    unsigned short ustat;
    tt_soc *other_tty;
    modem_status summary;

    uart = WHICHUART(tty->ttynum); /* Get the UART pointer */
    ustat = uart->portchange;	/* Get the port change flags */
    other_tty = MODEMS[OTHERLINE(tty->ttynum)];

    /*
     * Handle change flags and current status for this line
     */

    if (tty->ttynum & 1) {

	if (ustat & CTSAX)
	  tty->modem_changes.cts = TRUE;
	if (ustat & RINGAX)
	  tty->modem_changes.ring = TRUE;

	summary.cts = !(ustat & CTSA);
	summary.ring = !(ustat & RINGA);
    } else {

	if (ustat & CTSBX)
	  tty->modem_changes.cts = TRUE;
	if (ustat & RINGBX)
	  tty->modem_changes.ring = TRUE;

	summary.cts = !(ustat & CTSB);
	summary.ring = !(ustat & RINGB);
    }
    summary.dtr = ((tty->modem_bits & MODEMBIT_DTR) != 0);

    /*
     * Handle change flags for the other line
     */

    if (other_tty->ttynum & 1) {
	if (ustat & CTSAX)
	  other_tty->modem_changes.cts = TRUE;
	if (ustat & RINGAX)
	  other_tty->modem_changes.ring = TRUE;
    } else {
	if (ustat & CTSBX)
	  other_tty->modem_changes.cts = TRUE;
	if (ustat & RINGBX)
	  other_tty->modem_changes.ring = TRUE;
    }

    return(summary);
}

/*
 * Return (and clear) the current modem control state flags
 */

static modem_status STP_modemchange (tt_soc *tty)
{
    modem_status summary;

    STP_modemsignal(tty);	/* Read current status */
    summary = tty->modem_changes; /* Pick up the current change flag */
    tty->modem_changes = ((modem_status){0}); /* Reset the flags */

    return(summary);
}
