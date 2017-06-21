/* $Id: errors.h,v 1.1.4.1 1996/05/09 15:01:33 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/errors.h,v $
 *-----------------------------------------------------------------
 * errors.h --  error codes defined for ATM SAR diag LEDs.
 *              LED lit if bit set, else LED off
 *
 * February 1995, rbau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: errors.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:33  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:34  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:34  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 * errors.h
 *   error codes defined for ATM SAR diag LEDs.
 *   LED lit if bit set, else LED off
 *
 * Date: 02/13/95  rbau
 *********************************************************************
 */
#ifndef _ERROR_H_
#define _ERROR_H_

#define err_code0	0x0	/* no errors, or code entry */
#define err_code1	0x1   	/* IRAM code overflow error */
#define err_code2	0x2   	/* Secondary Memory Data Bus error */
#define err_code3	0x3     /* Secondary Memory address bus error */
#define err_code4	0x4     /* Frame Buffer Data Bus error */
#define err_code5	0x5     /* Frame Buffer Address Bus error */
#define err_code6	0x6     /* SAR DMA error */ 	
#define err_code7	0x7     /* error */ 	
#define err_code8	0x8     /* error */ 	
#define err_code9	0x9     /* error */ 	
#define err_codeA	0xA     /* error */ 	
#define err_codeB	0xB     /* error */ 	
#define err_codeC	0xC     /* first round test passed(test in IRAM) */ 	
#define err_codeD	0xD     /* error */ 	
#define err_codeE	0xE     /* error */ 	
#define err_codeF	0xF     /* error */ 	

#define test_code0	err_code0   	/* code entry */ 
#define test_code1	err_code1   	/* Secondary Memory Data Bus test */
#define test_code2	err_code2  	/* Secondary Memory Address Bus test */
#define test_code3	err_code3   	/* Frame Buffer Data Bus test */
#define test_code4	err_code4   	/* Frame Buffer Address  Bus test */
#define test_code5	err_code5   	/* test */
#define test_code6	err_code6   	/* test */
#define test_code7	err_code7   	/* test */
#define test_code8	err_code8   	/* test */
#define test_code9	err_code9   	/* test */
#define test_codeA	err_codeA  	/* test */
#define test_codeB	err_codeB   	/* test */
#define test_codeC	err_codeC   	/* test */
#define test_codeD	err_codeD   	/* test */
#define test_codeE	err_codeE   	/* test */
#define test_codeF	err_codeF   	/* test */

#endif /* _ERROR_H_ */
