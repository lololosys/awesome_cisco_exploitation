/* $Id: console.c,v 3.1.58.1 1996/05/09 13:54:16 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/console.c,v $
 *------------------------------------------------------------------
 * console.c -- console dependent support for Synergy ATM line card
 *		(based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: console.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:16  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:12  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:11  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:08  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:11  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:50  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:45  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:36  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:41  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:01  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:53  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:33  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:47  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../ctype.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "../flh_log.h" /* SYNALC_ADD */

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
    console->rsr = (STP_RXENABLE) ;
    console->tsr = (STP_TXENABLE) ;
}

char getchar()
{
    register u_char c;

    c = 0;
    while (c == 0) {		/* Ignore <Break> characters */
	while ((console->rsr & RXREADY) == 0)
		PUNCH_WD ;	/* Busy wait for char, wdog always enabled. */
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
	while ((console->rsr & RXREADY) == 0)
		PUNCH_WD ;	/* Busy wait for char, wdog always enabled. */
	c = console->data & 0177;	/* strip parity */
    }
    return(c);
}

void putchar(c)
    register char c;
{
    flh_putc(c); /* SYNALC_ADD */
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
    console->rsr = (STP_RXENABLE) ;
    console->tsr = (STP_TXENABLE) ;
    while ((console->tsr & TXREADY) == 0)
	PUNCH_WD ;	/* Busy wait for TxRdy, wdog always enabled. */
    console->data = c;
}

void flush()
{
    /* make sure transmitter is enabled */
    console->rsr = (STP_RXENABLE) ;
    console->tsr = (STP_TXENABLE) ;
    /*
     * Wait until the transmitter is empty:
     */
    while ((console->tsr & TXREADY) == 0)
	PUNCH_WD ;	/* Busy wait for TxRdy, wdog always enabled. */

#ifndef	SYNALC
    /*
     * Wait one character time for the slowest baud rate, 300 baud,
     * which is 30 ms:
     */
    delay(30);
#else
    /*
     * Wait one character time to drain the UART Tx shift register.
     * SYNALC baud rate is fixed at 9600, as console is debug-only lab
     * installation.  One-character time is thus ~1 msec.  Give a bit extra.
     */
    delay(2);
#endif
}
