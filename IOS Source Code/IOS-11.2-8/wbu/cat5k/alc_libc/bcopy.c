/* $Id: bcopy.c,v 1.1.4.1 1996/05/09 14:55:51 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/bcopy.c,v $
 *------------------------------------------------------------------
 * bcopy.c
 *
 * ?
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcopy.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:51  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:30  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:42  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)osdep.c	1.1 10/17/93 10/22/93 copyright (c) 1993 Xlnt Designs Inc";
#endif

#include "types.h"


void
bcopy (from, to, len)
/*********************************************************************

Function:   byte copy function.
Parameters:
Input:      src and dst addresses and number of bytes to copy.
Output:     None
Return:     None
Notes:
Modification History:
*********************************************************************/
caddr_t from, to;
short len;
{
	for (; len > 0; *to++ = *from++, len-- );
}
