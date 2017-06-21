/* $Id: strrchr.c,v 1.1.4.1 1996/05/09 14:56:07 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/strrchr.c,v $
 *------------------------------------------------------------------
 * strrchr.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strrchr.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:07  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:47  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:51  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:54:00  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)strrchr.c	1.1 6/24/93 ";
#endif
/*
						strrchr.c
  	Return the ptr in sp at which the character c last
  	appears; NULLPTR if not found
*/
#include "types.h"
char *
strrchr(sp, c)
register char *sp, c;
{
    register char *r;

    r = (char *)NULL;
    do {
        if(*sp == c)
            r = sp;
    } while(*sp++);
    return(r);
}
