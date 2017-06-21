/* $Id: memchr.c,v 1.1.4.1 1996/05/09 14:55:55 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/memchr.c,v $
 *------------------------------------------------------------------
 * memchr.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memchr.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:55  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:46  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)memchr.c	1.1 6/24/93 ";
#endif
#include "types.h"
#include "ctype.h"
/*
 * memchr - search for a byte
 *
 */

#define  __UNSCHAR(c)    ((unsigned char)(c))
void *
memchr(s, ucharwanted,size)
void *s;
uInt32 ucharwanted;
uInt32	size;
{
	register const uChar *scan;
	register uInt32 n;
	register uInt32 uc;

	scan = s;
	uc = __UNSCHAR(ucharwanted);
	for (n = size; n > 0; n--)
		if (__UNSCHAR(*scan) == uc)
			return((void *)scan);
		else
			scan++;

	return(NULL);
}
