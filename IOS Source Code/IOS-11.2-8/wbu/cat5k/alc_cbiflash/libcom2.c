/* $Id: libcom2.c,v 1.1.4.1 1996/05/09 14:54:23 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/libcom2.c,v $
 *------------------------------------------------------------------
 * libcom2.c
 * Library of functions and routines to be shared between the Common
 * Binary Linker and the Exploder.  These functions provide the
 * bridge between composition and decomposition between the two
 * halves of the GLUE Utility.
 *
 * ? 1995, ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: libcom2.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:23  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:48  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:19  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/10/04  01:07:52  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.2  1995/09/19  17:32:45  blouie
 * Added CVS header.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if	defined (SUN)
#include <stdio.h>
#include <sys/types.h>
#elif	defined (SYNALC)
#include "lo_wdog.h"
#include "acp_c_defs.h"
#include "cbi_flsh_plat.h"
#else
#include <types.h>
#include "register.h"
#include "ec040map.h"
#include "cbl.h"
extern uInt16 cntl2_shadow;
#endif

#include "libcom.h"


#define BOXWIDTH	62

/*
 * This function is called with a 4 byte aligned pointer and
 * a count of the number of words to check sum.  The value
 * csum is passed inorder to continue a check sum already in
 * progress.
 */
u_long checksum2(p, words, csum, pPrt)
   u_long *p;
   int words;
   u_long csum;
   int (*pPrt)() ;
{
	while (words--)
	{
#ifndef UTILITY
		if (!(words & 0xFFFL))
			PUNCH_WD;
#endif /* UTILITY */
		csum = (((csum & 0x80000000) ? 1 : 0) | (csum << 1)) ^ *p++;
	}
	return(csum);
}

/*
 * Draw edges around the caller specified string.
 *
 * CAUTION!!!  Be verrrrry careful when editing this function.  There
 * is only ONE line!  And it contains exactly the right number of blanks.
 * DON'T MESS WITH IT!!!
 */
void format2(s, pPrt)
   char *s;
   int (*pPrt)() ;
{
	if (pPrt)
	{
#ifdef COMMENT
		(*pPrt)("%s|\n", &"                                                             "[(*pPrt)("|    %s", s)]);
#else
		(*pPrt)("|    %s", s);
		(*pPrt)("%s|\n", &"                                                             "[strlen(s) + 5]);
#endif
	}
}

/*
 * Long word copy.  Copy words from src to dest for the number
 * of BYTES specified.  If the are unaligned bytes at the end
 * than do a byte copy on those bytes.
 */
void lcopy2(src, dest, size, pPrt)
   u_long *src, *dest;
   int size;
   int (*pPrt)() ;
{
	int words;
	int bytes;

	if (pPrt)
		(*pPrt)("COPY: src=%#x dest=%#x size=%d\n", src, dest, size);

	for (words = size / sizeof(long); words; --words)
		*dest++ = *src++;
	for (bytes = size % sizeof(long); bytes; --bytes)
		(((char *)dest) += 1)[-1] = (((char *)src) += 1)[-1];
}
