/* $Id: nonvol_x2444.c,v 3.1.58.1 1996/05/09 13:54:28 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/nonvol_x2444.c,v $
 *------------------------------------------------------------------
 * nonvol_x2444.c -- Synergy ATM line card driver for Xicor X2444
 *		     non-volatile static RAM (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nonvol_x2444.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:22  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:22  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:21  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:22  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:59  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:54  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:50  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:53  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:08  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:57  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:37  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:56  rahuja
 * IPS ver 10.2 snapshot
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
#ifndef	SYNALC
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
#endif	/* SYNALC */
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
