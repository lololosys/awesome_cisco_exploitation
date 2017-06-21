/* $Id: lo_error.h,v 1.1.4.1 1996/05/09 14:55:38 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/lo_error.h,v $
 *-----------------------------------------------------------------
 * lo_error.h -- Function prototypes and related definitions for
 *               generic low-level error handling.  Anything
 *               C-specific is indicated negatively with the flag
 *               ASM_MODE_FLG.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lo_error.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:38  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:36  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_LO_ERROR_H
#define	_LO_ERROR_H

	/* ====================================================	*/
	/*	Function prototypes and related definitions	*/
	/*	for generic low-level error handling.		*/
	/*	Anything C-specific is indicated negatively	*/
	/*	with the flag ASM_MODE_FLG.			*/
	/*	By Rick Lowe, cisco, 11-29-94.			*/
	/*	This is:  %W% %G%	*/
	/* ====================================================	*/

#ifndef	ASM_MODE_FLG			/* Following stuff C-specific	*/

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif	/* _ACP_BASIC_TYPES_H */


extern void A_DiagFail0 (	u_int32	ErrTestIndex,
				u_int32	ErrCounterOfst,
				void	(* WdogFcn)(),
				u_int32*pLastTest,
				u_int32	LastWrite,
				u_int32	LastRead ) ;

extern void A_FailLeds (	u_int32	MainLEDs,
				u_int32	DebugLEDs,
				u_int32	BuzzLoopCount,
				void	(* WdogFcn)() ) ;

extern void A_ResetCPU () ;

#endif	/* ASM_MODE_FLG */

	/* ============================================================	*/
	/*	Following stuff can be used by both C and assembly.	*/
	/* ============================================================	*/

#define	LOER_STD_LED_FLASH	(0x00000100)

	/* =============================================================== */
	/* Four debug LEDs provide up to 16 values to show power-up diags. */
	/* (Changes here must be reflected in Boot's NVRAM structure.)     */
	/* =============================================================== */

#define	PUD_MIN_VAL	(0x00)
#define	PUD_POWER_UP	(PUD_MIN_VAL)
#define	PUD_NVRAM	(0x01)
#define	PUD_BOOT_CSUM	(0x02)		/* Errors with Boot code checksum. */
#define	PUD_DRAM	(0x03)
#define	PUD_MFP		(0x04)		/* Errors with MFP diag. */
#define	PUD_UART	(PUD_MFP)	/* Errors with UART loopback diag. */
#define	PUD_8051_RAM	(0x05)
#define	PUD_FRAME_BUF	(0x06)
#define	PUD_TXSAR_RAM	(0x07)
#define	PUD_RXSAR_RAM	(0x08)
#define	PUD_FLSH_CSUM	(0x09)		/* Errors checking FLASH contents. */
#define	PUD_DNLD_CSUM	(0x0a)		/* Errors with network downloads.  */
#define	PUD_TSAR_DIAG	(0x0b)		/* Errors with TxSAR power-up diags. */
#define	PUD_RSAR_DIAG	(0x0c)		/* Errors with RxSAR power-up diags. */
#define	PUD_RESERVED	(0x0d)
#define	PUD_BT_EXCEPT	(0x0e)		/* Boot-time CPU exceptions. */
#define	PUD_MAX_VAL	(0x0f)		/* Also displayed on HW reset. */
#define	PUD_BT_IDLE_ERR	(PUD_MAX_VAL)

#endif	/* _LO_ERROR_H */
