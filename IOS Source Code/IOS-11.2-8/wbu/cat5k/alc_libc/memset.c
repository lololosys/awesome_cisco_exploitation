/* $Id: memset.c,v 1.1.4.1 1996/05/09 14:55:56 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/memset.c,v $
 *------------------------------------------------------------------
 * memset.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memset.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:56  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:48  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)memset.c	1.1 6/24/93 ";
#endif
/*
				memset.c

	Set memory to the specified value.
*/
#include "types.h"
uChar *
memset(buf, val, len)
register uChar   *buf;
register uChar    val;
register long    len;
{
    register uChar   *start = buf;

    while( len > 0 ){
        len--;
        *buf++ = val;
    }
    return(start);
} /* memset */

