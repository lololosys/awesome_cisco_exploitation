/* $Id: bt_c_powdiag.c,v 1.1.4.1 1996/05/09 14:59:58 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/bt_c_powdiag.c,v $
 *-----------------------------------------------------------------
 * bt_c_powdiag.c -- ACP PROM Boot power-up diagnostics in C.
 *		These can be called once assembly code has setup a
 *		stack and DATA/BSS sections in memory.
 *
 * December 2, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_c_powdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  14:59:58  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:03  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:26  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:06  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.4.6.1  1996/01/18  05:02:12  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.4  1995/10/14  08:46:48  rlowe
 * Fixes to allow NMP/SCP manufacturing diags to be run on system powerup.
 *
 * Revision 1.3  1995/10/13  06:06:42  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "mfp_hw_defs.h"
#include "lo_wdog.h"
#include "lo_error.h"
#include "boot_defs.h"
#include "bt_nvram.h"
#include "bt_globvar.h"
#include "acp_c_defs.h"
#include "libc.h"


/* ------------------------------------------------------------	*/
/*	Handle error results obtained from these diag tests.	*/
/*	Note that the error location pointer may be byte OR	*/
/*	long-word aligned.					*/
/*	Trace output patterned after that in bt_powdiag.S.	*/
/* ------------------------------------------------------------	*/
void btPUD_HdlFail (u_int8 * pTest, p_void ErrLocn, u_int32 ErrExpct,
		    u_int32 ErrRead, u_int32 TestIndex,
		    u_int32 NV_FailRecOfst)
{
	PUNCH_WD ;
	printf ("%s%s%08x", bts_PUD_Failed, bts_PUD_Err_at, ErrLocn) ;

	printf ("%s%s%s%08x%s%08x", bts_PUD_NewLine, pTest,
		bts_PUD_Err_wr, ErrExpct, bts_PUD_Err_rd, ErrRead) ;

	printf ("%s%s%08x%s%s", bts_PUD_NewLine, bts_PUD_Hangup,
		TestIndex, bts_PUD_NewLine0, bts_PUD_NewLine0) ;

	while (1)
	{
		PUNCH_WD ;
		A_DiagFail0 (TestIndex, NV_FailRecOfst, AR_SvcWatchdog,
			     (u_int32 *) ErrLocn, ErrExpct, ErrRead) ;
	}

}	/* btPUD_HdlFail */

/* ------------------------------------------------------------	*/
/*	Generic diags control for 32-bit wide memory.		*/
/*	Trace output patterned after that in bt_powdiag.S.	*/
/* ------------------------------------------------------------	*/
void btPUD_Mem32Diag (	u_int32 * pBaseAddr,	u_int32  MemSize,
			u_int32   TestIndex,	u_int32  NV_FailRecOfst,
			u_int8    fFastBoot,	u_int8 * pName)
{
	u_int32 *	ErrLocn  = NULL ;
	u_int32		ErrExpct = 0 ;
	u_int32		ErrRead  = 0 ;
	u_int8 *	pTest    = bts_PUD_Err_Unk ;


	PUNCH_WD ;
	printf ("%s%s", bts_PUD_NewLine, pName) ;

		/* Are we bypassing diags ? */
	if (fFastBoot)
	{
		PUNCH_WD ;
		puts (bts_PUD_Bypass) ;

		PUNCH_WD ;
		return ;
	}


		/* Run the diag tests; stop on first error found. */
	do {
		PUNCH_WD ;
		if (A_MD_DataBusL (pBaseAddr, AR_SvcWatchdog,
				   & ErrLocn, & ErrExpct, & ErrRead))
			{ pTest = bts_PUD_Err_DB ; break ; }

		PUNCH_WD ;
		if (A_MD_AddrBusL (pBaseAddr, MemSize, AR_SvcWatchdog,
				   & ErrLocn, & ErrExpct, & ErrRead))
			{ pTest = bts_PUD_Err_AB ; break ; }

		PUNCH_WD ;
		if (A_MD_ByteWordEnb (pBaseAddr, AR_SvcWatchdog,
				      & ErrLocn, & ErrExpct, & ErrRead))
			{ pTest = bts_PUD_Err_DW ; break ; }

		PUNCH_WD ;
		if (A_MD_RW_Pattern (pBaseAddr, MemSize, BOOT_POWDIAG_PTRN_1,
				     AR_SvcWatchdog,
				     & ErrLocn, & ErrExpct, & ErrRead))
			{ pTest = bts_PUD_Err_P1 ; break ; }

		PUNCH_WD ;
		if (A_MD_RW_Pattern (pBaseAddr, MemSize, BOOT_POWDIAG_PTRN_2,
				     AR_SvcWatchdog,
				     & ErrLocn, & ErrExpct, & ErrRead))
			{ pTest = bts_PUD_Err_P2 ; break ; }

		PUNCH_WD ;
		puts (bts_PUD_Passed) ;	/* All diag tests were successful ! */

		PUNCH_WD ;
		return ;

	} while (0) ;


		/* Come here to handle any diag test errors.   */
		/* Do NOT expect to return from error handler! */
	PUNCH_WD ;
	btPUD_HdlFail (pTest, (p_void) ErrLocn, ErrExpct, ErrRead,
			TestIndex, NV_FailRecOfst) ;

}	/* btPUD_Mem32Diag */

/* ------------------------------------------------------------	*/
/*	Generic diags control for 8-bit wide memory.		*/
/*	Trace output patterned after that in bt_powdiag.S.	*/
/* ------------------------------------------------------------	*/
void btPUD_Mem8Diag (	u_int8 * pBaseAddr,	u_int32  MemSize,
			u_int32  TestIndex,	u_int32  NV_FailRecOfst,
			u_int8   fFastBoot,	u_int8 * pName)
{
	u_int32 *	pBaseAdr2 ;
	u_int32		MemSize2  ;
	u_int32 *	ErrLocn32  = NULL ;
	u_int32		ErrExpct32 = 0 ;
	u_int32		ErrRead32  = 0 ;
	u_int8 *	pTest      = bts_PUD_Err_Unk ;
	u_int8 *	ErrLocn8   = NULL ;
	u_int8		ErrExpct8  = 0 ;
	u_int8		ErrRead8   = 0 ;


	PUNCH_WD ;
	printf ("%s%s", bts_PUD_NewLine, pName) ;

		/* Are we bypassing diags ? */
	if (fFastBoot)
	{
		PUNCH_WD ;
		puts (bts_PUD_Bypass) ;

		PUNCH_WD ;
		return ;
	}

	PUNCH_WD ;
	pBaseAdr2 = ((u_int32 *)(((u_int32)(pBaseAddr + 3)) & 0xfffffffc)) ;
	MemSize2  = MemSize - (((u_int32) pBaseAdr2) - ((u_int32) pBaseAddr)) ;

		/* Run the diag tests; stop on first error found. */
	do {
		PUNCH_WD ;
		if (A_MD_DataBusB (pBaseAddr, AR_SvcWatchdog,
				   & ErrLocn8, & ErrExpct8, & ErrRead8))
		{
			pTest      = bts_PUD_Err_DB ;
			ErrLocn32  = (u_int32 *) ErrLocn8 ;
			ErrExpct32 = ErrExpct8 ;
			ErrRead32  = ErrRead8  ;
			break ;
		}

		PUNCH_WD ;
		if (A_MD_AddrBusB (pBaseAddr, MemSize, AR_SvcWatchdog,
				   & ErrLocn8, & ErrExpct8, & ErrRead8))
		{
			pTest      = bts_PUD_Err_AB ;
			ErrLocn32  = (u_int32 *) ErrLocn8 ;
			ErrExpct32 = ErrExpct8 ;
			ErrRead32  = ErrRead8  ;
			break ;
		}

		PUNCH_WD ;
		if (A_MD_ByteWordEnb (pBaseAdr2, AR_SvcWatchdog,
				      & ErrLocn32, & ErrExpct32, & ErrRead32))
			{ pTest = bts_PUD_Err_DW ; break ; }

		PUNCH_WD ;
		if (A_MD_RW_Pattern (pBaseAdr2, MemSize2, BOOT_POWDIAG_PTRN_1,
				     AR_SvcWatchdog,
				     & ErrLocn32, & ErrExpct32, & ErrRead32))
			{ pTest = bts_PUD_Err_P1 ; break ; }

		PUNCH_WD ;
		if (A_MD_RW_Pattern (pBaseAdr2, MemSize2, BOOT_POWDIAG_PTRN_2,
				     AR_SvcWatchdog,
				     & ErrLocn32, & ErrExpct32, & ErrRead32))
			{ pTest = bts_PUD_Err_P2 ; break ; }

		PUNCH_WD ;
		puts (bts_PUD_Passed) ;	/* All diag tests were successful ! */

		PUNCH_WD ;
		return ;

	} while (0) ;


		/* Come here to handle any diag test errors.   */
		/* Do NOT expect to return from error handler! */
	PUNCH_WD ;
	btPUD_HdlFail (pTest, (p_void) ErrLocn32, ErrExpct32, ErrRead32,
			TestIndex, NV_FailRecOfst) ;

}	/* btPUD_Mem8Diag */

/* ------------------------------------------------------------	*/
/*	Generic diags control for 8-bit wide devices.		*/
/*	Trace output patterned after that in bt_powdiag.S.	*/
/* ------------------------------------------------------------	*/
void btPUD_Dev8Diag (	u_int8 * pBaseAddr,		u_int32 TestIndex,
			u_int32  NV_FailRecOfst,	u_int8  fFastBoot,
			u_int8 * pName)
{
	u_int8 *	pTest      = bts_PUD_Err_Unk ;
	u_int8 *	ErrLocn8   = NULL ;
	u_int8		ErrExpct8  = 0 ;
	u_int8		ErrRead8   = 0 ;


	PUNCH_WD ;
	printf ("%s%s", bts_PUD_NewLine, pName) ;

		/* Are we bypassing diags ? */
	if (fFastBoot)
	{
		PUNCH_WD ;
		puts (bts_PUD_Bypass) ;

		PUNCH_WD ;
		return ;
	}

		/* Run the diag tests; stop on first error found. */
	do {
		PUNCH_WD ;
		if (A_MD_DataBusB (pBaseAddr, AR_SvcWatchdog,
				   & ErrLocn8, & ErrExpct8, & ErrRead8))
			{ pTest = bts_PUD_Err_DB ; break ; }

		PUNCH_WD ;
		if (A_MD_RW_Byte (pBaseAddr, BOOT_POWDIAG_PTRN_3,
				  AR_SvcWatchdog,
				  & ErrLocn8, & ErrExpct8, & ErrRead8))
			{ pTest = bts_PUD_Err_P3 ; break ; }

		PUNCH_WD ;
		puts (bts_PUD_Passed) ;	/* All diag tests were successful ! */

		PUNCH_WD ;
		return ;

	} while (0) ;


		/* Come here to handle any diag test errors.   */
		/* Do NOT expect to return from error handler! */
		/* Note ErrLocn8 ptr may NOT be long-aligned.  */
	PUNCH_WD ;
	btPUD_HdlFail (pTest, (p_void) ErrLocn8, (u_int32) ErrExpct8,
			(u_int32) ErrRead8, TestIndex, NV_FailRecOfst) ;

}	/* btPUD_Dev8Diag */

/* ------------------------------------------------------------	*/
/*	This does a basic loopback test on the UART and then	*/
/*	readies the UART for Boot use.				*/
/* ------------------------------------------------------------	*/
void btPUD_Uart (u_int8 fFastBoot)
{
		/* Do not have anything yet... */
		/* Besides, UART is already in use for Boot console output. */
	PUNCH_WD ;

}	/* btPUD_Uart */

/* ------------------------------------------------------------	*/
/*	This drives C-based power-up diags for the board.	*/
/*	Trace output patterned after that in bt_powdiag.S.	*/
/* ------------------------------------------------------------	*/
void btPUD_PowerUpDiags (u_int8 fFastBoot)
{
		/* Note that UART may already be enabled for trace output. */

	PUNCH_WD ;
	btPUD_Uart (fFastBoot) ;

		/* May want to do diags on BIGA, NTC,	*/
		/* and PMD, as with MFP/UART ?  For MFP,*/
		/* do we need to add interrupt/timer	*/
		/* diags ?  What about DRAM non-caching	*/
		/* area ?				*/

	PUNCH_WD ;
	btPUD_Mem32Diag ((u_int32 *) (ACP_HW_FRBUF_BASE),
			 ACP_HW_FRBUF_SIZE, PUD_FRAME_BUF,
			 BTNVR_FBUFF_COUNT, fFastBoot, "Frame buffer RAM") ;

	PUNCH_WD ;
	btPUD_Mem32Diag ((u_int32 *) (ACP_HW_TSAR_RAM_BASE),
			 ACP_HW_TSAR_RAM_SIZE, PUD_TXSAR_RAM,
			 BTNVR_TSARF_COUNT, fFastBoot, "TSAR shared RAM") ;

	PUNCH_WD ;
	btPUD_Mem32Diag ((u_int32 *) (ACP_HW_RSAR_RAM_BASE),
			 ACP_HW_RSAR_RAM_SIZE, PUD_RXSAR_RAM,
			 BTNVR_RSARF_COUNT, fFastBoot, "RSAR shared RAM") ;

	PUNCH_WD ;

}	/* btPUD_PowerUpDiags */
