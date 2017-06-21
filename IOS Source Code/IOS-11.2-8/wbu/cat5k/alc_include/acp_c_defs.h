/* $Id: acp_c_defs.h,v 1.1.4.1 1996/05/09 14:55:26 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/acp_c_defs.h,v $
 *------------------------------------------------------------------
 * acp_c_defs.h -- ACP C source code definitions header file.
 *
 * These definitions are specific to C code and are not
 * to be used by assembly code.  These defs can be used
 * by C Boot code, or normal run-time code.
 *
 * December 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: acp_c_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:26  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:03  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:54  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:22  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ACP_C_DEFS_H
#define	__ACP_C_DEFS_H

	/* ============================================================	*/
	/*	This is the ACP C source code definitions header file.	*/
	/*	These definitions are specific to C code and are not	*/
	/*	to be used by assembly code.  These defs can be used	*/
	/*	by C Boot code, or normal run-time code.		*/
	/*								*/
	/*	By Rick Lowe, cisco, 94-12-02.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif
#ifndef	_BT_WDOG_H
#include "lo_wdog.h"
#endif
#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif


#define	C_ADDR_MFP(ofst)			/* Get base addr of MFP. */\
			((u_int32) ((ACP_HW_MFP_BASE) + (ofst)))

#define	C_ADDR_HW_REGS(ofst)			/* Get base addr of HW regs. */\
			((u_int32) ((ACP_HW_IOREG_BASE) + (ofst)))

#define	C_CHK_JMPR(which)			/* Check that jumper is set. */\
			(! ((* (u_int8 *) (C_ADDR_HW_REGS (ACP_RDRG_DEBUG))) & \
			    (1 << (which))))

#define	C_ACP_INTR2_SAR(tx_rx,normal,test)	/* Cause ACP->SAR interrupt. */\
			* (u_int32 *) (ACP_INTR2_SAR_REG (tx_rx)) = \
					(ACP_INTR2_SAR_DAT (normal,test))

#define	PUNCH_WD	A_SvcWatchdog ()


#endif	/* __ACP_C_DEFS_H */
