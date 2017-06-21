/* $Id: sim_console.c,v 3.2.62.1 1996/03/18 22:20:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ts/sim_console.c,v $
 *------------------------------------------------------------------
 * sim_console - Console driver for Motorola SIM (System Integration Module)
 *  
 * November 27, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sim_console.c,v $
 * Revision 3.2.62.1  1996/03/18  22:20:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  23:29:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:56:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "stacks.h"
#include "../os/async.h"
#include "../dev/monitor1.h"

extern void sim_int(void);
void sim_tx_disable(tt_soc *);
boolean sim_startoutput(tt_soc *);

vectortype console_vector;

/*
 * console_init - Set up the SIM as system console
 */

tt_soc *console_init ()
{
    register vectortype *vector;
    register tt_soc *tty;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);
    vector = &console_vector;
    vector_init(vector);
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput; 
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = sim_startoutput;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vputpkt = generic_putpkt;
    vector->vservicehook = generic_servicehook;
    vector->vnoservicehook = generic_noservicehook;
    vector->vstopoutput = sim_tx_disable;

    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty, MAXINCHARS, MAXOUTCHARS);
    tty->txfinish = sim_tx_disable;
    /*
     * This may be a lie (the rom monitor has control), but initialize it
     * to the default anyway so that NV generation doesn't get confused.
     */
    tty->tty_stopbits = STOPBITS2;

    exceptionHandler(QSMVector, sim_int);

    createlevel(LEVEL_CONSOLE, NULL, "Console Uart");

    QSM->sccr1 = SCCR1_TE | SCCR1_RE | SCCR1_TIE | SCCR1_RIE;
			/* Enable transmit, receive, transmit interrupt,
			   and receive interrupt */

    reset_interrupt_level(SR);
    return(tty);
}

/*
 * sim_console_interrupt
 */

void sim_console_interrupt (void)
{
    tt_soc *tty;
    ushort status;
    int c;

    tty = MODEMS[0];		/* Get the console pointer */
    if (!tty)
      return;			/* Strange - not quite set up yet */

    while ((status = QSM->scsr) & SCSR_RDRF) {	/* We have data to receive */

	c = QSM->scdr;		/* Pick up the character */

	if (status & SCSR_OR) {
	    tty->overrun++;
        }

	if (status & SCSR_FE) {	/* Framing error is a break */
	    if ((tty != console) || (mon_simbrk() == -1)) {
		c = BREAK_CHAR;	/* Pass on for things like LAT */
	    } else {
		continue;	/* Otherwise skip & continue */
	    }
	}

	tty->totalin++;

	if (tty->rxservice) {
	  uchar c2 = c;

	  (*(tty->rxservice))(tty, &c2, sizeof(c2));
	} else {
	    if (special_chars(tty, c) >= 0) {
		
		if (tty->intail >= tty->inend)
		  tty->intail = tty->instart; /* Handle wraps */
		
		/* If we have used half of the input buffer,
		 * send an XOFF to try to stop the input.
		 */
		    
		if (tty->incount > (tty->insize >> 1)) {
		    if (tty->intail == tty->inhead) {
			tty->overrun++;
			continue;
		    }
		    tty->statbits |= SENDXOFF;
		    QSM->sccr1 |= SCCR1_TIE;
		}
		/* so there is room  */
		*tty->intail++ = c;
		++tty->incount;
	    }
	}
    }

    while ((status = QSM->scsr) & SCSR_TDRE) {

	/*
	 * If the line is set to send flow control, do so now
	 */

	if (tty->statbits & SENDXOFF) {
	    if (tty->capabilities & SFLOWOUT) {
		QSM->scdr = tty->tty_stopchar;
		tty->statbits |= SENTXOFF;
	    }
	    tty->statbits &= ~SENDXOFF;
	} else if (tty->txservice) {
	    uchar c2;

	    /*
	     * If the line has a special transmit service, get the
	     * characters from there.
	     */
	    if ((*(tty->txservice))(tty, &c2, sizeof(c2)) > 0) {
		QSM->scdr = c2;
		tty->totalout++;
	    } else {
		(*(tty->txfinish))(tty);
		break;
	    }
	} else {

	    /* Using the standard output service */
		
	    if (tty->outcount) {
		register uchar *cp = ++tty->outhead;
		if (cp == tty->outend) {
		    cp = tty->outstart;
		    tty->outhead = cp;
		}
		tty->outcount--;
		QSM->scdr = *cp;
		tty->totalout++;
	    } else {
		(*(tty->txfinish))(tty);
		break;
	    }
	}		
    }
}

/*
 * sim_tx_disable - Disable transmit interrupts
 */

void sim_tx_disable (tty)
     tt_soc *tty;
{
    QSM->sccr1 &= ~SCCR1_TIE;
}

/*
 * sim_startoutput - Start output by forcing a transmit interrupt
 */

boolean sim_startoutput (tty)
     tt_soc *tty;
{
    if (!(tty->statbits & (HOLDING | WANTXON)))
      QSM->sccr1 |= SCCR1_TIE;
    return(TRUE);
}

/*
 * console_putc - Output a character, polled
 */

void console_putc (ch)
     uchar ch;
{
    if (ch == '\n')
      console_putc('\r');	/* Preface NL with CR */

    QSM->sccr1 |= SCCR1_TE;	/* Ensure transmitter is enabled */

    while (!(QSM->scsr & SCSR_TDRE))
      ;				/* Wait for output ready */

    QSM->scdr = ch;		/* Output the character */
}

/* 
 * console_getc - Read a character, polled
 */

int console_getc (void)
{
    leveltype SR;
    char ch;

    SR = raise_interrupt_level(TTY_DISABLE);

    while (!(QSM->scsr & SCSR_RDRF))
      ;				/* Wait for a character to come in */

    ch = QSM->scdr;		/* Get the character */

    reset_interrupt_level(SR);	/* Reset interrupts */

    return (ch & 0x7f);
}
