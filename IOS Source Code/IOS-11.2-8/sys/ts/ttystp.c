/* $Id: ttystp.c,v 3.5.12.2 1996/05/21 10:07:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/ttystp.c,v $
 *------------------------------------------------------------------
 * ttystp.c -- device driver for the Sixteen Terminal Processor
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttystp.c,v $
 * Revision 3.5.12.2  1996/05/21  10:07:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.1  1996/05/21  06:41:58  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5  1996/02/13  20:42:36  hampton
 * Remove platform specific dependencies from common tty header files.
 * [CSCdi48844]
 *
 * Revision 3.4  1996/02/01  06:11:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:39:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:56:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  09:01:22  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  23:10:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "subsys.h"
#include "stacks.h"
#include "ttysrv.h"
#include "stp.h"
#include "s2681.h"
#include "../os/async.h"
#include "../snmp/chassismib_registry.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Parse chain for STP test command
 */
#define	ALTERNATE	NONE
#include "exec_test_lines.h"
LINK_POINT(test_stp_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * STPline_interrupt
 * Inline handler for a single line.
 */

static inline void STPline_interrupt (tt_soc *tty)
{
    ushort status;
    int c;
    uartaddress *uart;
    volatile ushort *data_ptr, *status_ptr;

    data_ptr = tty->uartdata;
    status_ptr = tty->uartstatus;
    status = *status_ptr;

    if (status & OVRRNERROR) {
	tty->overrun++;
	uart = WHICHUART(tty->ttynum);
	if (tty->ttynum & 1) {
	    uart->commandA = RESETERROR;
	} else {
	    uart->commandB = RESETERROR;
	}
    }

    /* 
     * For rxservice, accumulate data in chunks before invoking
     * the rxservice vector.
     */
    if (tty->rxservice) {
	uchar buf[16];
	uint len = 0;

	/*
	 * Assemble bytes out of the FIFO and hand to rxservice
	 */
	while (status & RXREADY) {
	    /*
	     * For break, just dump all data and finish
	     */
	    if (status & RCVBREAK) {
		while (*status_ptr & RXREADY)
		    c = *data_ptr;	/* flush buffer */
		len = 0;
		break;
	    }

	    /*
	     * Tally byte, accumulate to buffer
	     */
	    tty->totalin += 1;
	    buf[len++] = *data_ptr;

	    /*
	     * Flush on full buffer (unlikely)
	     */
	    if (len == sizeof(buf)) {
		(*tty->rxservice)(tty, buf, sizeof(buf));
		len = 0;
	    }

	    /*
	     * Get new status
	     */
	    status = *status_ptr;
	}

	/*
	 * Hand off any residual data
	 */
	if (len > 0) {
	    (*tty->rxservice)(tty, buf, len);
	}
    } else {
	while (status & RXREADY) {

	    c = *data_ptr;

	    if (status & RCVBREAK) {	
		while (*status_ptr & RXREADY)
		  c = *data_ptr;	/* flush buffer */
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
		    tty->statbits |= SENDXOFF;
		    startoutput(tty);
		}
		/* so there is room  */
		*tty->intail++ = c;
		++tty->incount;
	    }
	    status = *status_ptr;
	}			
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

	/*
	 * If the line has a special transmit service, get the
	 * characters from there.  TBD: can we see how many open
	 * bytes in the FIFO there are, and get that many from
	 * the txservice vector at once?
	 */
	if (tty->txservice) {		
	    while (status & TXREADY) {
		uchar c2;

		if ((*(tty->txservice))(tty, &c2, sizeof(c2)) > 0) {
		    *data_ptr = c2;
		    tty->totalout++;
		    status = *status_ptr;
		} else {
		    (*(tty->txfinish))(tty);
		    break;
		}
	    }
	    
	} else {

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
}

/*
 * STPinterrupt
 * Handle an interrupt from an STP card.
 */

void STPinterrupt (void)
{
    int i, boardstatus;
    tt_soc *tty;

    for (i = STPBase; i < STPBase+nSTPlines; i+=16) {
	tty = MODEMS[i];
	boardstatus = *tty->board_rxstatus | *tty->board_txstatus;
	while (boardstatus) {		/* start servicing TxRdy interrupts */
	    if (boardstatus & 1) {
		STPline_interrupt(tty);
	    }
	    tty++;
	    boardstatus >>= 1;
	}
    }
}

/*
 * stp_hardware_init
 * Initialize the hardware and data structures for the STP cards
 */

static void stp_hardware_init (void)
{
    uartaddress *uart;
    int i, j, code;
    uchar dummy;
    uartaddress *limit = (UARTSTART + ((nSTPlines-1) >> 1));
    tt_soc *tty;

    STP_vector_init();
    createlevel(2, STPinterrupt, "RS232 serial line cards");
    for (j = 0; j < nSTPlines; j += 16) {
        tty = malloc_named(16*sizeof(tt_soc), "CSC-16 data");
	if (!tty) {
	    printf(nomemory);
	    return;
	}
	for (i = j; i < j+16; i++) {
	    /*
	     * Fill in pointers to hardware that interrupt level needs
	     */
	    MODEMS[STPBase+i] = tty; /* Set the pointer */
	    tty_init_data(tty,STPBase+i,READY,TTY_LINE, &STPvector);
	    uart = UARTSTART+ ((i & 0xFFF0) >> 1);  /* first uart on board */
	    tty->board_rxstatus = &(uart+1)->boardstatus;
	    tty->board_txstatus = &(uart+2)->boardstatus;
	    tty->rxservice = NULL;
	    tty->txservice = NULL;
	    tty->txfinish = STP_txdisable;
	    uart = UARTSTART + (i >> 1);
	    if ((tty->ttynum & 1)) {
		tty->uartdata = & uart->dataA;
		tty->uartstatus = & uart->statusA;
	    } else {
		tty->uartdata = & uart->dataB;
		tty->uartstatus = & uart->statusB;
	    }
	    /*
	     * If we're bootloading, allocate tiny buffers so that 96-line
	     * units don't soak up so much memory.
	     */
	    if (system_loading) {
		serial_setbuffers(tty,20,20);
	    } else {
		serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS);
	    }
	    tty->tty_txspeed = ASYNC_DEFSPEED;
	    tty->tty_rxspeed = ASYNC_DEFSPEED;
	    tty->tty_databits = 8;
	    tty->tty_parity = PARITY_NO;
	    tty->tty_stopbits = STOPBITS2;
	    tty->tty_stopchar = CTRLS;	/* CTRL/S or XOFF */
	    tty->tty_startchar = CTRLQ; /* CTRL/Q or XON */
	    tty->modem_type = MODEMTYPE_LOCAL; /* Default to local line */
	    tty++;
	}
    }
    code = STP_baud2code(ASYNC_DEFSPEED);
    for (uart = UARTSTART, i=0; uart <= limit; uart++,i++) {

	/*
	 * First the A uart
	 */
	uart->commandA = RESETMRPTR;
	uart->modeA = (RXRDYINTR + CHARERROR + PRTYOFF + DBITS8);
	uart->modeA = (MODENORMAL + STOP16X16);
	uart->statusA = (RXCLOCK * code + TXCLOCK * code);
	uart->commandA = (TXDISABLE + STP_RXENABLE);
	uart->portchange = (BRGSET1+TIMXTAL+RINGBXENB+RINGAXENB+CTSBXENB+CTSAXENB);
	uart->interrupt = (TXRDYA + TXRDYB + RXRDYB + RXRDYA);
	uartstatus[i] = (TXRDYA + TXRDYB + RXRDYB + RXRDYA);
	uart->resetport = 255;		/* clear all RS-232 signals */
	tty = MODEMS[((i*2)+STPBase)];	/* set tty for TTY A */
	tty->modem_bits &= ~(255);	/* clear saved modem state */
	uart->setport = DTRA;		/* raise DTR on A port */
	tty->modem_bits |= MODEMBIT_DTR; /* Update saved modem state */
	uart->CTUpper = 0;
	uart->CTLower = 3;		/* generates 38.4 Kbit */

	/*
	 * Now the B uart
	 */
	uart->commandB = RESETMRPTR;
	uart->modeB = (RXRDYINTR + CHARERROR + PRTYOFF + DBITS8);
	uart->modeB = (MODENORMAL + STOP16X16);
	uart->statusB = (RXCLOCK * code + TXCLOCK * code);
	uart->portstatus = (OPRXRDYA+OPTXRDYA+OPRXRDYB+OPTXRDYB+CTOUT);
	uart->commandB = (TXDISABLE + STP_RXENABLE);
	tty = MODEMS[((i*2)+STPBase+1)]; /* set tty for TTY B */
	tty->modem_bits &= ~(255);	/* clear saved modem state (above) */
	uart->setport = DTRB;		/* raise DTR on B port */
	tty->modem_bits |= MODEMBIT_DTR; /* Update saved modem state */
	dummy = uart->setport;		/* start counter/timer(read setport)*/
#ifdef lint
	uart->setport = dummy;		/* keep lint quiet */
#endif
    }
}

/*
 * print_modemcontrol
 * print the states of the modem control input bits.
 *
 * Warning - this currently clears the state change register!
 */

static void print_modemcontrol (tt_soc *tty)
{
    uartaddress *uart;
    int ustat;
    int cts, ctsx, ring, ringx;

    if (tty->type != TTY_LINE)
	return;

    uart = WHICHUART(tty->ttynum);
    ustat = uart->portchange;

    if (tty->ttynum & 1) {
	cts = CTSA; ctsx = CTSAX; ring = RINGA; ringx = RINGAX;
    } else {
	cts = CTSB; ctsx = CTSBX; ring = RINGB; ringx = RINGBX;
    }
    printf("\nModem hardware state:  %sCTS%s%sRING%s",
	   ustat & cts ? "no":"", ustat & ctsx ? "* ":" ",
	   ustat & ring ? "no":"", ustat & ringx ? "* ":" ");
}	   

/*
 * stp_init
 * Detect any STP serial line interfaces.
 * Returns board count.
 */

#define STPBASE ((char *) (MULTIMEMBASE+0x40000))
#define STPOFFSET 0x100
#define STPMAX  7

static void stp_init (subsystype *subsys)
{
    int i, nSTPBoards;

    nSTPBoards = 0;		/* no boards discovered yet */
    nSTPlines = 0;		/* total lines = 16*nSTPBoards */
    STPBase = freeLineBase;	/* our base is the first free line */
    if (system_loading)		/* if netbooting, quit now. */
	return;
    for (i = 0; i < STPMAX; i++) {
	if (touch_device(&STPBASE[i*STPOFFSET])) {
	    if (i != nSTPBoards) {
		printf("\nTTY: serial cards out of sequence");
		nSTPBoards = 0;
	    } else {
		nSTPBoards++;
	    }
	}
    }
    if (nSTPBoards) {
	nSTPlines = 16*nSTPBoards;
	nttylines += nSTPlines;		/* add our tty lines */
	freeLineBase += nSTPlines;	/* adjust first free line */
	stp_hardware_init();
	for (i = 0; (i < nSTPBoards); i++)
	    reg_invoke_chassismib_add_card_entry(D_cardType_csc_16, NO_HOTSWAP,
				"16-line async serial interface",
				NO_SERIALNUM, NO_HWVERSION, NO_SWVERSION,
				NO_SLOTNUMBER);
	reg_add_tty_show(print_modemcontrol, "print_modemcontrol");
	reg_add_setup_global2(async_setup_global, "async_setup_global");
	parser_add_commands(PARSE_ADD_TEST_CMD, &pname(test_stp_commands),
			    "stp");
    }
}

/*
 * STP subsystem header
 */

#define STP_MAJVERSION 1
#define STP_MINVERSION 0
#define STP_EDITVERSION  1

SUBSYS_HEADER(stp, STP_MAJVERSION, STP_MINVERSION, STP_EDITVERSION,
	      stp_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
