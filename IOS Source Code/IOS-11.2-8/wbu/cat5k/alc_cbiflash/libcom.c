/* $Id: libcom.c,v 1.1.4.1 1996/05/09 14:54:22 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/libcom.c,v $
 *------------------------------------------------------------------
 * libcom.c
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
 * $Log: libcom.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:22  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:47  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:18  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7  1995/10/04  01:07:51  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.6  1995/09/19  17:31:48  blouie
 * Added CVS header.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifdef SUN
#include <stdio.h>
#include <sys/types.h>

#else
#ifdef	SYNALC
#include "cbi_flsh_plat.h"

#else
#include <types.h>
#endif

#endif

extern int printf () ;

#include "libcom.h"


/*
 * This function is called with a 4 byte aligned pointer and
 * a count of the number of words to check sum.  The value
 * csum is passed inorder to continue a check sum already in
 * progress.
 */
u_long checksum(p, words, csum)
   u_long *p;
   int words;
   u_long csum;
{
	return (checksum2 (p, words, csum, printf)) ;
}

/*
 * Draw edges around the caller specified string.
 *
 * CAUTION!!!  Be verrrrry careful when editing this function.  There
 * is only ONE line!  And it contains exactly the right number of blanks.
 * DON'T MESS WITH IT!!!
 */
void format(s)
   char *s;
{
	format2 (s, printf) ;
}

/*
 * Long word copy.  Copy words from src to dest for the number
 * of BYTES specified.  If the are unaligned bytes at the end
 * than do a byte copy on those bytes.
 */
void lcopy(src, dest, size)
   u_long *src, *dest;
   int size;
{
	lcopy2 (src, dest, size, printf) ;
}
