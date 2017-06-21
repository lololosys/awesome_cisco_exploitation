/* $Id: datatest.c,v 3.1.6.1 1996/05/09 14:41:06 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/datatest.c,v $
 *------------------------------------------------------------------
 * datatest.c -- TX SAR frame send tests
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: datatest.c,v $
 * Revision 3.1.6.1  1996/05/09  14:41:06  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.1  1996/04/27  07:25:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.2  1996/04/25  05:46:15  rlowe
 * Clean out obsolete files and code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.40.1  1996/04/08  02:08:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.24.3  1996/03/27  21:55:22  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.2  1996/03/22  23:40:10  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.1  1996/03/22  09:43:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.2.2  1996/03/20  10:48:47  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.1  1996/03/13  03:18:35  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1  1996/03/11  08:15:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 *  FILE:  datatest.c
 *
 *  CONTENTS:
 *		Test the TX sar data send functionality
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/

/*
 *	This file is now obsolete.  Diags that utilize the device
 *	drivers are now performed at boot-up time.
 */

