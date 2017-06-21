/* $Id: lo_wdog.h,v 1.1.4.1 1996/05/09 14:55:40 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/lo_wdog.h,v $
 *-----------------------------------------------------------------
 * lo_wdog.h -- Function prototypes and related definitions for
 *              generic watchdog support routines.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lo_wdog.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:40  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:18  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:11  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:39  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_BT_WDOG_H
#define	_BT_WDOG_H

	/* ====================================================	*/
	/*	Function prototypes and related definitions	*/
	/*	for generic watchdog support routines.		*/
	/*	By Rick Lowe, cisco, 11-28-94.			*/
	/*	This is:  %W% %G%	*/
	/* ====================================================	*/


extern void AR_SvcWatchdog (void) ;	/* Non-stack (register linkage) use. */
extern void A_SvcWatchdog (void) ;	/* Normal use. */

#endif	/* _BT_WDOG_H */
