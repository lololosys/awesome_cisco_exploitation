/* $Id: strtol.c,v 1.1.4.1 1996/05/09 14:56:09 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/strtol.c,v $
 *------------------------------------------------------------------
 * strtol.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strtol.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:09  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:54  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:54:02  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)strtol.c	1.1 6/24/93 ";
#endif
/*
						strtol.c

	Convert a string to a long.
*/
#include "ctype.h"
#define DIGIT(x) (isdigit(x)? ((x)-'0'): (10+tolower(x)-'a'))
#define MBASE 36


long
strtol(str, ptr, base)
char *str, **ptr;
int base;
{
    long val;
    int xx, sign;
	char	*sptr;

    val = 0L;
    sign = 1;
    if(base < 0 || base > MBASE)
        goto OUT;
    /*
    while(isspace(*str))
        ++str;
    */
    if(*str == '-') {
        ++str;
        sign = -1;
    } else if(*str == '+')
        ++str;
    if(base == 0) {
        if(*str == '0') {
            ++str;
            if(*str == 'x' || *str == 'X') {
                ++str;
                base = 16;
            } else
                base = 8;
        } else
            base = 10;
    } else if(base == 16)
        if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
            str += 2;
    /*
     * for any base > 10, the digits incrementally following
     *  9 are assumed to be "abc...z" or "ABC...Z"
     */
	sptr = str;
	while(*sptr){
		if(isalpha(*sptr) && islower(*sptr))
			*sptr = toupper(*sptr);
		sptr++;
	}
    while(isalnum(*str) && (xx=DIGIT(*str)) < base) {
        /* accumulate neg avoids surprises near maxint */
        val = base*val - xx;
        ++str;
    }
OUT:
    if(ptr != (char**)0)
        *ptr = str;
    return(sign*(-val));
}
