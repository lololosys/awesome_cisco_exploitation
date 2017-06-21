/* $Id: console.c,v 3.1 1995/11/09 09:09:30 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/console.c,v $
 *------------------------------------------------------------------
 * Console dependant support for XX
 *
 * October 19, 1989, Chris Shaker
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: console.c,v $
 * Revision 3.1  1995/11/09  09:09:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:45  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1993/03/11  03:03:18  clev
 * Fix stalled console receiver in the ROM Monitor.
 *
 * Revision 1.2  1993/02/23  23:53:08  clev
 * Make sure the console UART transmitter is enabled in putchar() and
 * flush() in the ROM Monitor.
 *
 * Revision 1.1  1992/04/15  02:11:30  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../ctype.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"

/*
 * Note the S1BAUD - must set bit 7 in the ACR (BRGSET1):
 */

u_char UARTspeeds[] = {
    S1BAUD9600,				/* 9600 */
    S1BAUD4800,				/* 4800 */
    S1BAUD1200,				/* 1200 */
    S1BAUD2400,				/* 2400 */
    S1BAUD300,				/* 300 */
    S1BAUD600,				/* 600 */
    S1BAUD110,				/* 110 */
    S1BAUD9600				/* 9600 or something else */
};

/*
 * Make sure that busy-waits don't wait forever because the UART
 * transmitter or receiver is disabled. Also ensure that DTR is set.
 */

void init_console()
{
    console->command = STP_TXENABLE|STP_RXENABLE;
}

char getchar()
{
    register u_char c;

    c = 0;
    while (c == 0) {		/* Ignore <Break> characters */
	while ((console->state & RXREADY) == 0)
	    ;			/* busy wait for character */
	c = console->data & 0177;	/* strip parity */
    }
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
    
    c = 0;
    while (c == 0) {		/* Ignore <Break> chars */
	while ((console->state & RXREADY) == 0)
	    ;			/* busy wait for character */
	c = console->data & 0177;	/* strip parity */
    }
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
    /* make sure transmitter is enabled */
    console->command = STP_TXENABLE|STP_RXENABLE;
    while ((console->state & TXREADY) == 0)
	;			/* wait for ready */
    console->data = c;
}

void flush()
{
    /* make sure transmitter is enabled */
    console->command = STP_TXENABLE|STP_RXENABLE;
    /*
     * Wait until the transmitter is empty:
     */
    while ((console->state & TXEMPTY) == 0)
	;
    
    /*
     * Wait one character time for the slowest baud rate, 300 baud,
     * which is 30 ms:
     */
    delay(30);
}
