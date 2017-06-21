/* $Id: strcmp.c,v 1.1.4.1 1996/05/09 14:56:02 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/strcmp.c,v $
 *------------------------------------------------------------------
 * strcmp.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcmp.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:02  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:53  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)strcmp.c	1.1 6/24/93 ";
#endif
/*
					strcmp.c

	Compare two strings.  Return 0 if strings match, 1 if first string is
	greater than second, -1 if first string less than second.
*/
#include "types.h"
int
strcmp(a, b)
register uChar *a, *b;
{
    for(; *a == *b; a++, b++){
        if( !*a ) return(*b? 1: 0);
        if( !*b ) return(1);
    }
    if( *a > *b )
        return(1);
    if( *a < *b )
        return(-1);
}
