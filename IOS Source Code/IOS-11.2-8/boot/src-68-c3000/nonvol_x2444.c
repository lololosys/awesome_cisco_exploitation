/* $Id: nonvol_x2444.c,v 3.1 1995/11/09 09:09:16 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/nonvol_x2444.c,v $
 *------------------------------------------------------------------
 * nonvol_x2444.c - Driver for Xicor X2444 non-volatile static RAM
 *  
 * December 23, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file was copied from the sys/os directory.
 *
 *------------------------------------------------------------------
 * $Log: nonvol_x2444.c,v $
 * Revision 3.1  1995/11/09  09:09:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1993/12/21 06:25:47  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:32  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/06/25  05:09:02  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "pcmap.h"
#include "nonvol_x2444.h"
void wastetime(unsigned long);
void usecdelay(unsigned);

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
    wastetime(1);

    /*
     * Give the command to the X2444
     */

    for (i = 0; i < readbits; i++) {
	if (cmd & 0x1)
	  X2444_PORT |= X2444_DI; /* Set the data in bit */
	else
	  X2444_PORT &= ~X2444_DI; /* Clear the data in bit */
	wastetime(1);
	X2444_PORT |= X2444_SK;	/* Set the clock bit */
	wastetime(1);
	X2444_PORT &= ~X2444_SK; /* Reset the clock bit */
	wastetime(1);
	cmd >>= 1;		/* Shift right one bit */
    }
    X2444_PORT |= X2444_DI;	/* Set data in bit */
    wastetime(1);

    /*
     * Read the response from the X2444
     */

    for (i = 0; i < writebits; i++) {
	if (X2444_PORT & X2444_DO) /* If the data bit is set */
	  value |= (1 << i);	/* Set the appropriate value bit */
	X2444_PORT |= X2444_SK;	/* Set the clock bit */
	wastetime(1);
	X2444_PORT &= ~X2444_SK; /* Clear the clock bit */
	wastetime(1);
    }

    /*
     * Disable the X2444
     */

    X2444_PORT &= ~X2444_CE;	/* Clear chip enable */
    wastetime(1);

    /*
     * We're done
     */
    return(value);		/* All done */
}

/*
 * routine to just do a fast spinloop to waste some time
 */
void
wastetime (n)
unsigned long n;
{
   for (; n>0; n--);
}
