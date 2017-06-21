/* $Id: lec_ipctst.c,v 3.2.62.1 1996/05/09 14:41:10 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/lec_ipctst.c,v $
 *------------------------------------------------------------------
 * lec_ipctst.c -- Tester for primitives to support messaging between the LANE
 *	and earl task running on the NMP.
 *
 * June 9, 1995.  Ratinder Ahuja.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_ipctst.c,v $
 * Revision 3.2.62.1  1996/05/09  14:41:10  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.2  1996/04/25  05:46:16  rlowe
 * Clean out obsolete files and code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/03/22  23:40:14  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:43:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.2  1996/03/20  10:48:50  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:55  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:10  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:17  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:43:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:53  shaker
 * Bump version numbers from 2.x to 3.x.
 * Revision 1.1.68.1  1996/02/03  07:08:34  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 1.1.50.2  1995/09/28  00:47:13  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Remove lec_ipc_inputQ_enabled.
 *
 * Revision 1.1.50.1  1995/09/11  01:57:09  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 1.1.34.2  1995/09/02  01:56:45  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 1.1.34.1  1995/08/22  17:52:04  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 1.1.2.2  1995/07/13  21:57:02  bbenson
 * Branch: Synergy_ATMlc_branch
 * lec_ipc.h moved to syn_common.
 *
 * Revision 1.1.2.1  1995/06/09  23:46:38  sakumar
 * Initial check-in of real, functional code.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/06/09  23:39:30  sakumar
 * Initial check-in of stub files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 /*
 * ---------------------------------------------------------------
 *
 *  FILE: lec_ipctst.c
 *
 *  CONTENTS:
 *  	This mimics the lec tasks interactions with the earl and 
 *	the spanning tree. 
 *
 *  OWNERS:
 *
 *      Ratinder P. S. Ahuja
 */

/*
 *	This file is now obsolete.  There is a real LEC now.
 */

