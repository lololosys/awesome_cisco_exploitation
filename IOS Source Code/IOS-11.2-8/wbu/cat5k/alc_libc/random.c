/* $Id: random.c,v 1.1.4.1 1996/05/09 14:55:58 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/random.c,v $
 *------------------------------------------------------------------
 * random.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: random.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:58  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:40  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:49  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)random.c	2.2 11/9/93 11/9/93 copyright (c) 1993 Xlnt Designs Inc";
#endif
#include "types.h"

#define PRIME_INT   0x87654321
#define PRIME_WORD  0x4321
#define PRIME_SEED  0x1234567

/*
 |  Generate a random number from the seed in '*mac' within the range
 |   of 0 to 'range'.
 |  If range is 0 then any 32-bit number is allowed
 */

static uInt32 mac32;
static uInt32 getrandom();

uInt32 
random32( uInt32 floor, uInt32 ceiling )
{
	uInt32 r;
	if ( floor >= ceiling )
		return( getrandom() );
	else {
		do 
			r = getrandom() % (ceiling+1);
		while ( r < floor );
		return( r );	
	}
}

uInt16 
random16( uInt16 floor, uInt16 ceiling )
{
	uInt16 r;
	if ( floor >= ceiling )
		return( getrandom());
	else {
		do 
			r = (getrandom() ) % (ceiling+1);
		while ( r < floor );
		return( r );	
	}
}

static uInt32
getrandom()
{
    mac32 = ( (mac32 + PRIME_SEED) % PRIME_INT );
	return( mac32 );
}
