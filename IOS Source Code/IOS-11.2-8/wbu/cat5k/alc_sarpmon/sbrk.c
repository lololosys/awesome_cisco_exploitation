/* $Id: sbrk.c,v 1.1.4.1 1996/05/09 15:00:31 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/sbrk.c,v $
 *------------------------------------------------------------------
 * sbrk.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sbrk.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:31  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:13  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:09  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <defines.h>
#include "pmon.h"

char *getenv();
#ifdef BSO_TASKING
extern char __lc_bh;
char *allocp1 = &__lc_bh;
#else
extern char end;
char *allocp1 = &end;
#endif

char *sbrk(n)
int n;
{
char *heaptop,*t;

t = getenv("heaptop");
if (t == 0) heaptop = (char *)CLIENTPC;
else {
	if (!atob(&heaptop,t,16)) {
		heaptop = (char *)CLIENTPC;
		}
	}
if (allocp1 + n <= heaptop) {
	allocp1 += n;
	return(allocp1 - n);
	} 
else return(0);
}

