/* $Id: string.h,v 1.1.4.1 1996/05/09 15:00:11 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/string.h,v $
 *-----------------------------------------------------------------
 * string.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: string.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:11  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:17  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:47  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _STRING_
#define _STRING_
char *strcat(),*strncat(),*strchr(),*strncpy(),*cc2str(),*rindex();
char *strcpy(),*strichr(),*strdchr(),*strposn(),*getword(),*index();
char *strset(),*strrset(),*strrchr(),*strbalp(),*strrpset(),*strpbrk();
char *strtok();
int strequ(),strlequ(),strlen(),strpat();

/* definitions for fmt parameter of str_fmt(p,width,fmt) */
#define FMT_RJUST 0
#define FMT_LJUST 1
#define FMT_RJUST0 2
#define FMT_CENTER 3

#endif /* _STRING_ */
