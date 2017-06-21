/* $Id: bt_nvram.h,v 1.1.4.1 1996/05/09 14:55:33 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/bt_nvram.h,v $
 *------------------------------------------------------------------
 * bt_nvram.h -- ACP Boot NVRAM usage definitions.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_nvram.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:33  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:10  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:02  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:30  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BT_NVRAM_H
#define	__BT_NVRAM_H

	/* ============================================================	*/
	/*	These are ACP Boot NVRAM usage definitions.		*/
	/*	Definitions here are common to both C and assembly	*/
	/*	code.  Anything C-specific is indicated negatively	*/
	/*	with the flag ASM_MODE_FLG.				*/
	/*	By Rick Lowe, cisco, 94-11-28.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif


#ifndef	ASM_MODE_FLG			/* Following stuff C-specific	*/

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif

#ifndef	__BOOT_DEFS_H
#include "boot_defs.h"
#endif

	/* --------------------------------------------	*/
	/* Changes here affect assembly code and may	*/
	/* also affect low-level error code handling.	*/
	/* --------------------------------------------	*/
typedef struct AcpBtNvram
{
		/* These fields are for Boot CPU exception handling. */
	u_int32			NvExceptInfo [BOOT_EXCP_DATA_SZ / 4];
	u_int16			ResrvE_1 ;	/* + $0b4 */
	u_int16			NvExceptCount ;	/* + $0b6 */
	u_int32			pOld_LDI ;	/* + $0b8 */
		/* These fields are for flash writing (download) errors. */
	u_int32			NbrDownlds ;	/* + $0bc */
	u_int32			DownldFails ;	/* + $0c0 */
	u_int32			FlashPgmErrs ;	/* + $0c4 */
		/* These fields are for Boot power-up diags error handling. */
	u_int32			MemErrAddr ;	/* + $0c8 */
	u_int32			MemErrXpct ;	/* + $0cc */
	u_int32			MemErrRead ;	/* + $0d0 */
	u_int16			MemErrIndx ;	/* + $0d4 */
	u_int16			ResetCount ;	/* + $0d6 */
	u_int16			NVRamFails ;	/* + $0d8 */
	u_int16			BtChkSumErr ;	/* + $0da */
	u_int16			DramFails ;	/* + $0dc */
	u_int16			MfpFails ;	/* + $0de */
	u_int16			LcpFails ;	/* + $0e0 */
	u_int16			FBufFails ;	/* + $0e2 */
	u_int16			TSarFails ;	/* + $0e4 */
	u_int16			RSarFails ;	/* + $0e6 */
	u_int16			FlChkSumErr ;	/* + $0e8 */
	u_int16			DnChkSumErr ;	/* + $0ea */
	u_int16			TSarPDFail ;	/* + $0ec */
	u_int16			RSarPDFail ;	/* + $0ee */
	u_int16			ResrvF ;	/* + $0f0 */
		/* These fields are for Boot power-up diags and initialization. */
	u_int16			BtFmtVers ;	/* + $0f2 */
	u_int32			DiagArea ;	/* + $0f4 */
	u_int32			fValid ;	/* + $0f8 */
	struct AcpNvram *	pSelf ;		/* + $0fc */
} tAcpBtNvram ;				/* Max size is 256 bytes. */

#endif	/* ASM_MODE_FLG */

	/* ============================================================	*/
	/*	Following stuff can be used by assembly as well as C.	*/
	/* ============================================================	*/

	/* Boot usage values.  Boot area is at end of NVRAM. */
#define	BOOT_NVRAM_USED		(0x100)
#define	BOOT_NVRAM_AREA		\
		(BOOT_NVRAM_START + BOOT_NVRAM_SIZE - BOOT_NVRAM_USED)
#define	BOOT_NVRAM_FMT_VER	(1)

#define	ASM_ADDR_BOOT_NVRAM(where)		/* Asm:  get Boot NVRAM area.*/\
		ASM_ADDR_STORE (BOOT_NVRAM_AREA, where)

	/* (Assembly) Boot usage field offsets. */
#define	BTNVR_MEMERR_ADDR	(0x0c8)		/* NVRAM locn of 4-byte ptr   */
#define	BTNVR_MEMERR_XPCT	(0x0cc)		/* NVRAM locn of 4-byte long  */
#define	BTNVR_MEMERR_READ	(0x0d0)		/* NVRAM locn of 4-byte long  */
#define	BTNVR_MEMERR_INDX	(0x0d4)		/* NVRAM locn of 2-byte short */
#define	BTNVR_RESET_COUNT	(0x0d6)		/* NVRAM locn of 2-byte short */
#define	BTNVR_NVRMF_COUNT	(0x0d8)		/* NVRAM locn of 2-byte short */
#define	BTNVR_BTCSUMF_COUNT	(0x0da)		/* NVRAM locn of 2-byte short */
#define	BTNVR_DRAMF_COUNT	(0x0dc)		/* NVRAM locn of 2-byte short */
#define	BTNVR_MFPF_COUNT	(0x0de)		/* NVRAM locn of 2-byte short */
#define	BTNVR_LCPF_COUNT	(0x0e0)		/* NVRAM locn of 2-byte short */
#define	BTNVR_FBUFF_COUNT	(0x0e2)		/* NVRAM locn of 2-byte short */
#define	BTNVR_TSARF_COUNT	(0x0e4)		/* NVRAM locn of 2-byte short */
#define	BTNVR_RSARF_COUNT	(0x0e6)		/* NVRAM locn of 2-byte short */
#define	BTNVR_FLCSUMF_COUNT	(0x0e8)		/* NVRAM locn of 2-byte short */
#define	BTNVR_DNCSUMF_COUNT	(0x0ea)		/* NVRAM locn of 2-byte short */
#define	BTNVR_TSARPDF_COUNT	(0x0ec)		/* NVRAM locn of 2-byte short */
#define	BTNVR_RSARPDF_COUNT	(0x0ee)		/* NVRAM locn of 2-byte short */
#define	BTNVR_RESRVF_COUNT	(0x0f0)		/* NVRAM locn of 2-byte short */
#define	BTNVR_BT_FMT_VER	(0x0f2)		/* NVRAM locn of 2-byte short */
#define	BTNVR_DIAG_AREA		(0x0f4)		/* NVRAM locn of 4-byte long  */
#define	BTNVR_VALID_FLAG	(0x0f8)		/* NVRAM locn of 4-byte long  */
#define	BTNVR_PTR_SELF		(0x0fc)		/* NVRAM locn of 4-byte ptr   */
#define	BTNVR_VFLG_VALUE	(0xfeedbac0)	/* Value NVRAM valid flag. */

#endif	/* __BT_NVRAM_H */
