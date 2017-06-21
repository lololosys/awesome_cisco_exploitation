/* $Id: bigatest.c,v 1.1.4.1 1996/05/09 14:54:46 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bigatest.c,v $
 *-----------------------------------------------------------------
 * bigatest.c -- test driver for the TX SAR
 *
 * December 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bigatest.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:46  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/25  05:46:21  rlowe
 * Clean out obsolete files and code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:15  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:12  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:42  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/04  01:30:51  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.7  1995/03/18  01:58:00  sakumar
 * Finished biag sar integration
 *
 * Revision 1.6  1995/02/17  21:26:03  rlowe
 * Unify to Ratinder's latest code.  Mask off MSB when checking "status1" in
 * procedure "bigatst_checkinit".
 *
 * Revision 1.5  1995/02/06  19:06:49  rahuja
 * Changed OK and ERR to ACP_OK, ACP_ERR
 *
 * Revision 1.4  1995/01/14  00:05:16  rahuja
 * Finished biga testing
 *
 * Revision 1.3  1994/12/20  22:51:32  rahuja
 * support for sending
 *
 * Revision 1.2  1994/12/09  22:10:00  rahuja
 * *** empty log message ***
 *
 * Revision 1.1  1994/12/02  19:02:28  rahuja
 * Checked in after unit testing
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
 

/*
 *
 *  FILE:  bigatest.c
 *
 *  CONTENTS:
 *
 *	This file contains the test driver for the TX SAR
 *		The various test cases are:
 *			BIGA_INIT_TEST			
 *			BIGA_1ENQ_DEQ_TEST	
 *			BIGA_NENQ_DEQ_TEST		
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/

/*
 *      This file is now obsolete.  Diags that utilize the device
 *      drivers are now performed at boot-up time.
 */

