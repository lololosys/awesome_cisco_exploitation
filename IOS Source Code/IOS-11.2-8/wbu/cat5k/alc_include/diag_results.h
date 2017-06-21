/* $Id: diag_results.h,v 1.1.4.1 1996/05/09 14:55:34 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/diag_results.h,v $
 *------------------------------------------------------------------
 * diag_results.h -- diagnostic results to pass to NMP
 *
 * August 1995, rbau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diag_results.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:34  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:04  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:32  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/08/10  20:53:36  rbau
 * template for diagnostic results fields in SCP RESET_ACK message
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__DIAG_RESUTLS_H_
#define	__DIAG_RESUTLS_H_

#include  "scp_msgs.h"

/*------------------------------------------------------------------
 * index code for version info in resset ACK
 *------------------------------------------------------------------
 */
typedef enum _SW_Ver 
{
	ACP_boot1_ver  = 0,
	ACP_boot2_ver ,
	ACP_IOS_boot_ver,
	ACP_sw_ver,
	TXSAR_sw_ver,
	RXSAR_sw_ver,
} eVer_Index ;

typedef struct   _DiagResults
{
   /*
	 * mpu xtra diag result 
	 */
	char  alc_diag_results[MPU_XTRA_DIAG_BYTES];

	/*
	 * additional info in RESET_ACK message
	 */
	char  pmd_info[ADDITIONAL_INFO_SIZE];
} DiagResults_t;

#endif	/* __DIAG_RESULTS_H_ */
