/* $Id: auxport.c,v 3.2.58.1 1996/03/21 23:26:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/auxport.c,v $
 *------------------------------------------------------------------
 * auxport.c
 *
 * February 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Auxilliary port I/O routines.
 *------------------------------------------------------------------
 * $Log: auxport.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:37  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:40:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "console.h"

/*
**  Initialize the AUX port (channel b) to 9600 baud.
*/
void
initaux(void)
{
    uartaddr->a.aux2    = 0;	               /* Turn off interrupts (IMR=0) */
    uartaddr->b.command = RESETMRPTR;
    uartaddr->b.mode    = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->b.mode    = MODENORMAL | STOP16X16;
    uartaddr->b.state   = (RXCLOCK * UARTspeeds[0]) | (TXCLOCK * UARTspeeds[0]);
    uartaddr->b.command = RESETRX;
    uartaddr->b.command = RESETTX;
    uartaddr->b.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    uartaddr->a.aux2    = 0;
    return;
}

/*
** Raw auxport output routine.
*/

void
auxout(int c)
{
    while((uartaddr->b.state & TXREADY) == 0)
	;			/* wait for ready */
    uartaddr->b.data = c;
}

/*
** Raw auxport input routine.
*/
int
auxin(void)
{
    if(uartaddr->b.state & RXREADY)
	return(uartaddr->b.data);
    return(-1);
}

/*
** Auxport status routine.
** Returns true (1) if there is a character to be had, false (0) otherwise.
*/
int
auxstat(void)
{
    if(uartaddr->b.state & RXREADY) return(1);
    return(0);  /* no character */
}
