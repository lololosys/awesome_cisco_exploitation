/* $Id: bigatest.h,v 1.1.4.1 1996/05/09 14:54:47 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bigatest.h,v $
 *-----------------------------------------------------------------
 * bigatest.h -- header for test driver for the TX SAR
 *
 * December 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bigatest.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:47  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:13  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:43  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/04  01:30:51  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.1  1994/12/02  19:02:29  rahuja
 * Checked in after unit testing
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 *  FILE:  bigatest.h
 *
 *  CONTENTS:
 *
 *      The various test cases. 
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/
 
#ifndef _BIGATEST_H_
#define _BIGATEST_H_

extern UINT32	testcase;
extern UINT32	testresult;


/* test states */
#define BIGATST_IACTIVE			0
#define	BIGATST_RUNNING			1
#define	BIGATST_PASSED			2
#define	BIGATST_FAILED			3


/*
**	TEST cases
*/

#define	BIGA_INIT_TEST         1	/* initialize biga and verify			*/
#define	BIGA_1ENQ_DEQ_TEST     2	
#define	BIGA_NENQ_DEQ_TEST     3

#define	BIGA_LAST_TEST			BIGA_NENQ_DEQ_TEST
#define	BIGA_TEST_OVER			BIGA_LAST_TEST+1

/*
**	PROTOTYPES
*/

extern void bigatst_checkinit();


#endif /* _BIGATST_H_ */

