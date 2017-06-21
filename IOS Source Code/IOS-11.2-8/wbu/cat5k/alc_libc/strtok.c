/* $Id: strtok.c,v 1.1.4.1 1996/05/09 14:56:09 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/strtok.c,v $
 *------------------------------------------------------------------
 * strtok.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strtok.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:09  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:53  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:54:01  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)strtok.c	1.1 6/24/93 ";
#endif
/*
						strtok.c
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULLPTR when no
 * non-separator characters remain.
 * `subsequent' calls are calls with first argument NULLPTR.
 */
#include "types.h"

#define NULLPTR (char *)NULL
extern int strspn();
extern char *strpbrk();
char		*savept;

char *
strtok(string, sepset)
char    *string, *sepset;
{
    char   *p, *q, *r;


    /*first or subsequent call*/
    p = (string == NULLPTR)? savept: string;

    if(p == 0)      /* return if no tokens remaining */
        return(NULLPTR);

    q = p + strspn(p, sepset);  /* skip leading separators */

    if(*q == '\0')      /* return if no tokens remaining */
        return(NULLPTR);

    if((r = strpbrk(q, sepset)) == NULLPTR) /* move past token */
        savept = 0;   /* indicate this is last token */
    else {
        *r = '\0';
        savept = ++r;
    }
    return(q);
}


