/* $Id: alc_flash.c,v 3.2.62.1 1996/05/09 14:40:56 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/alc_flash.c,v $
 *------------------------------------------------------------------
 * alc_flash.c -- Synergy ATM line card platform specific Flash routines
 *		  (based on 3000).
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alc_flash.c,v $
 * Revision 3.2.62.1  1996/05/09  14:40:56  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.2  1996/04/10  06:40:44  rlowe
 * Fix up ifdefs; clean-out old, obsolete code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/22  09:43:29  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:43  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:55  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:06  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:42:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  07:08:23  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:57:01  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/08/31  14:05:08  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  17:51:34  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  22:54:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	This file is now unused.  The ATM line card flash code
 *	is in ios/wbu/cat5k/alc_cbiflash.
 */
