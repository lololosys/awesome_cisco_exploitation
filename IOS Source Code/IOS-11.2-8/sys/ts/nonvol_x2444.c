/* $Id: nonvol_x2444.c,v 3.2 1995/11/17 18:55:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/nonvol_x2444.c,v $
 *------------------------------------------------------------------
 * nonvol_x2444.c - Driver for Xicor X2444 non-volatile static RAM
 *  
 * December 23, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nonvol_x2444.c,v $
 * Revision 3.2  1995/11/17  18:55:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:09:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "nonvol_x2444.h"

/*
 * x2444_io - Do I/O to the X2444
 *
 * This routine is used to pass a command to the X2444 and optionally
 * return a response.
 *
 * Data is read and written with the least significant bit first.
 */

ulong x2444_io (cmd, readbits, writebits)
     int cmd;
     int readbits;
     int writebits;
{
    int i;
    ulong value = 0;

    /*
     * Enable the X2444
     */

    X2444_PORT &= ~(X2444_SK | X2444_DI); /* Clear clock and data in */
    X2444_PORT |= X2444_CE;	/* Set chip enable */
    DELAY(1);			/* And delay for one millsecond */

    /*
     * Give the command to the X2444
     */

    for (i = 0; i < readbits; i++) {
	if (cmd & 0x1)
	  X2444_PORT |= X2444_DI; /* Set the data in bit */
	else
	  X2444_PORT &= ~X2444_DI; /* Clear the data in bit */
	DELAY(1);		/* Delay one millisecond */
	X2444_PORT |= X2444_SK;	/* Set the clock bit */
	DELAY(1);		/* Delay one millsecond */
	X2444_PORT &= ~X2444_SK; /* Reset the clock bit */
	DELAY(1);		/* Delay one millisecond */
	cmd >>= 1;		/* Shift right one bit */
    }
    X2444_PORT |= X2444_DI;     /* Set data in bit */
    DELAY(1);			/* Delay one millsecond */

    /*
     * Read the response from the X2444
     */

    for (i = 0; i < writebits; i++) {
	if (X2444_PORT & X2444_DO) /* If the data bit is set */
	  value |= (1 << i);	/* Set the appropriate value bit */
	X2444_PORT |= X2444_SK;	/* Set the clock bit */
	DELAY(1);		/* Delay one millsecond */
	X2444_PORT &= ~X2444_SK; /* Clear the clock bit */
	DELAY(1);		/* Delay one millsecond */
    }

    /*
     * Disable the X2444
     */

    X2444_PORT &= ~(X2444_CE | X2444_DI);	/* Clear chip enable and DI*/
    DELAY(1);			/* And delay */

    /*
     * We're done
     */

    return(value);		/* All done */
}
