/* $Id: atmtest.h,v 1.1.4.1 1996/05/09 14:54:45 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/atmtest.h,v $
 *-----------------------------------------------------------------
 * atmtest.h -- ATM test driver running on the host
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:45  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/27  21:55:43  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:10  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:40  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * atmtest.h -- ATM test driver running on the host
 *
 *------------------------------------------------------------------
 *
 *  FILE:  atmtest.h
 *
 *  CONTENTS:
 *
 *      The various test cases are:
 *          ATMDRV_INIT_TEST         
 *          ATMDRV_1ENQ_DEQ_TEST     send no-op to sars, wait for resp
 *          ATMDRV_NENQ_DEQ_TEST     send N no-ops to sars, wait for responses
 *          ATMDRV_SET/GET/CLEAR_1VPC_TEST
 *          ATMDRV_SET/GET/CLEAR_NVPC_TEST
 *          ATMDRV_SEND_NCELL_TEST   send 1 cell packet (bom/eom test)
 *          ATMDRV_SEND_NCELL_TEST   send 2 cell packet (bom, eom test)
 *          ATMDRV_SEND_NCELL_TEST   send 4 cell packet (bom, com, com, eom )
 *          ATMDRV_SEND_NCELL_TEST    1 1500 byte packet
 *          ATMDRV_SEND_NCELL_TEST    N 1500 byte packets
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/
 
#ifndef _ATMTST_H_
#define _ATMTST_H_

extern UINT32	testcase;
extern UINT32	testresult;


/* test states */
#define ATMTST_IACTIVE			0
#define	ATMTST_RUNNING			1
#define	ATMTST_PASSED			2
#define	ATMTST_FAILED			3

/* Defines for wait_for_interrupt() */
#define WAIT_FOREVER 			1
#define WAIT_TILL_TIMEOUT 		0
#define DEFAULT_MAX  			10


#define	ATMTST_INIT_TEST         1	/*send reset to ATMDRV, wait for resp 	*/
#define	ATMTST_1ENQ_DEQ_TEST     2	/*send no-op to sars, wait for resp 	*/
#define	ATMTST_NENQ_DEQ_TEST     3	/*send N noops to sars, wait for resp 	*/
#define	ATMTST_1VPC_TEST     	4	/*set, get clear 1 vpc					*/
#define	ATMTST_NVPC_TEST     	5	/*set, get clear N vpcs					*/
#define	ATMTST_1CAM_TEST     	6	/*set, get clear 1 cam entry			*/
#define	ATMTST_NCAM_TEST     	7	/*set, get clear N cam entries			*/

#define	ATMTST_NCELL_TEST     	8	/*send a various cell size frames		*/
#define	ATMTST_LAST_TEST			ATMTST_NCELL_TEST

#define	ATMTST_TEST_OVER			ATMTST_LAST_TEST+1

/*
**	PROTOTYPES
*/

extern void atmtst_interrupt_sars(void);
extern void atmtst_enqdeq(void);
extern void atmtst_Nenqdeq(void);
extern void atmtst_vpc_1test(void);
extern void atmtst_vpc_Ntest(void);
extern void atmtst_test_Ncell(void);

#endif /* _ATMTST_H_ */

