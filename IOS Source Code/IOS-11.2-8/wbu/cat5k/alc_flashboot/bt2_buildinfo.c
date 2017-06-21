/* $Id: bt2_buildinfo.c,v 1.1.4.1 1996/05/09 14:55:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_flashboot/bt2_buildinfo.c,v $
 *-----------------------------------------------------------------
 * bt2_buildinfo.c -- build date/time.
 *
 * This file is built during any make that would otherwise produce
 * a new code image.  It gives the date and time of that build.
 *
 * January 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt2_buildinfo.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:34  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:08  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* --------------------------------------------------------------------	*/
/*	Build date/time.  This file is built during any make that	*/
/*	would otherwise produce a new code image.  It gives the		*/
/*	date and time of that build.					*/
/*									*/
/*	Modification History (Synergy):					*/
/*	95-01-25  RKL -	Originated.					*/
/* --------------------------------------------------------------------	*/



const unsigned char Bt2_BuildInfo [] = __DATE__ " " __TIME__ ;

