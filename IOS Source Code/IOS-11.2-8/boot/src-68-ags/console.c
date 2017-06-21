/* $Id: console.c,v 3.1 1995/11/09 09:08:26 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/console.c,v $
 *------------------------------------------------------------------
 * Console dependant support
 *
 * April 1987, Greg Satz
 *
 * Copyright (c) 1987-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: console.c,v $
 * Revision 3.1  1995/11/09  09:08:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:32:57  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1992/03/07  23:56:17  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../ctype.h"
#include "../reason.h"
#include "../extern.h"

u_char UARTspeeds[] = {
    0xD,				/* 9600 */
    0xC,				/* 4800 */
    0x9,				/* 1200 */
    0xB,				/* 2400 */
    0x6,				/* 300 */
    0x7,				/* 600 */
    0x2,				/* 110 */
    0xD					/* 9600 or something else */
};

/*
 * Make sure that the monitor does not busy wait for a disabled UART:
 */
void init_console() {
    console->ccr = CCR_TCMD | CCR_ENABLE;
    console->ccr = CCR_RCMD | CCR_ENABLE;
}

char getchar()
{
    register u_char c;
    
    while ((console->gsr & GSR_A_RXRDY) == 0)
	;			/* busy wait for character */
    c = console->rxfifo[0] & 0177;	/* strip parity */
    if (echoing)
	putchar(c);
    if (islower(c))
	c = toupper(c);
    return(c);
}

/*
 * getch doesn't echo or case-coerce
 */

char getch()
{
    register u_char c;
    
    while ((console->gsr & GSR_A_RXRDY) == 0)
	;
    c = console->rxfifo[0] & 0177;	/* strip parity */
    return(c);
}

void putchar(c)
    register char c;
{
    if (c < ' ') {
	switch (c) {
	  case '\007':
	  case '\010':
	  case '\011':
	  case '\r':
	    break;		/* pass BELL, BS, TAB, and CR */
	  case '\n':
	    putchar('\r');
	    break;		/* for a LF, ensure CR */
	  default:
	    putchar('^');
	    c += '@';
	    break;		/* others to arrow format */
	}
    }
    
    while ((console->gsr & GSR_A_TXRDY) == 0)
	;			/* wait for ready */
    console->txfifo[0] = c;
}

void flush()
{
    /*
     * Wait until transmitter is completely empty:
     */
    console->ccr = CCR_TCMD | CCR_ENABLE;
    while (((console->gsr & GSR_A_TXRDY) == 0) ||
	   ((console->trsr & TRSR_TX_EMPTY) == 0))
	;
    
    /*
     * Wait one character time for the slowest baud rate, 300 baud,
     * which is 30 ms:
     */
    delay(30);
}
