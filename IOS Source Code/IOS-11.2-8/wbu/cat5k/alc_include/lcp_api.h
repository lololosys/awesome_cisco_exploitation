/* $Id: lcp_api.h,v 1.1.4.1 1996/05/09 14:55:35 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/lcp_api.h,v $
 *-----------------------------------------------------------------
 * lcp_api.h -- LCP driver simple API definitions.
 *
 * July 12, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_api.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:35  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/11  05:31:27  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:05  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:32  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.10  1995/10/20  05:04:52  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 * Revision 1.9  1995/10/14  03:35:10  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.8  1995/10/13  06:06:53  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.7  1995/09/08  21:02:46  rbau
 * added new wrapper to set PMD type in LCP driver
 *
 * Revision 1.6  1995/09/08  18:35:23  rbau
 * added API function to read PMD type
 *
 * Revision 1.5  1995/09/07  12:09:09  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.4  1995/08/31  12:17:04  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.3  1995/07/27  01:23:11  rlowe
 * Cleanup LANE Set Port State upcall from LCP driver so that
 * Boot code does not need to know about it.  Register the
 * upcall from ALC-specific startup.
 *
 * Revision 1.2  1995/07/26  04:08:35  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.1  1995/07/12  23:21:53  rlowe
 * Moved some simpler extern API definitions to new files "lcp_api.h".
 * This helps clients of these APIs avoid having to include some header
 * files from syn_common.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LCP_API_H
#define _LCP_API_H

#ifndef _ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif

/* --------------------------------------------	*/
/*	Some nice constants, part of API.	*/
/* --------------------------------------------	*/

typedef union LCP_IPAddrU
{
	u_int8	ch [4] ;
	u_int32	ul ;
} tLCP_IPAddrU ;

/* ----------------------------	*/
/*	Driver return codes.	*/
/* ----------------------------	*/

typedef enum LCP_RetCodes
{
	LCP_RC_Normal	= 0,
	LCP_RC_Bad_Ptr,			/* Invalid message buffer pointer.  */
	LCP_RC_Msg_Len,			/* Bad mesage length field in hdr.  */
	LCP_RC_Tx_Busy,			/* Tx blocked, another in progress. */
	LCP_RC_Tx_TOut,			/* Tx timed out on LCP ack.         */
	LCP_RC_InitBusy,		/* LCP still initializing.          */
} eLCP_RetCodes ;

/* ----------------------------	*/
/*	Driver entry points.	*/
/* ----------------------------	*/

extern s_int32 LCP_TxBusyPoll (u_int8 fOnce) ;
extern u_int32 LCP_GetDrvrInfoSiz (void) ;
extern p_void LCP_GetDriverInfo (p_void pBuf) ;
extern u_int8 LCP_PhysSlotNbr (void) ;
extern u_int8 LCP_NMP_PhysSlotNbr (void) ;
extern u_int16 LCP_PmdType (void) ;
extern void LCP_SetPmdType (u_int16) ;
extern u_int16 LCP_CardInstanceNbr (void) ;
extern u_int32 LCP_AtmIf_StartIndex (void) ;
extern u_int32 LCP_AtmIf_IndexRange (void) ;
extern tLCP_IPAddrU * LCP_NMP_IP_Addr (tLCP_IPAddrU * pRtn) ;
extern tLCP_IPAddrU * LCP_ACP_IP_Addr (tLCP_IPAddrU * pRtn) ;
extern u_int8  LCP_SetDbgPrt (s_int8 NewVal) ;
extern void LCP_GroomForPromBoot (void) ;
extern s_int32 LCP_DriverInit_1 (void (** VecTbl)(void), u_int8 fReset,
				 u_int8 fMFPIntr, u_int8 fKernelMode,
				 p_void pPreviousLDI) ;
extern s_int32 LCP_DriverInit_2 (u_int8 fOnce,
				 u_int8 fFwdRsp, u_int8 fPolling) ;
extern s_int32 LCP_DriverInit_3 (u_int8 fOnce, u_int8 fFwdRsp, u_int8 fPolling,
				 u_int8 DiagsLen, u_int8 * pDiagRes) ;
extern u_int8 LCP_DriverInit_4 (u_int8 fOnce) ;
extern s_int32 LCP_DriverInit_5 (u_int8 fOnce, u_int8 fPolling) ;
extern void LCP_Rx_Isr (void) ;
extern void LCP_Tx_Isr (void) ;

extern void A_LCP_Release (u_int32 fCleanIntrsToLCP) ;	/* Normal C use. */
extern void AR_LCP_Release (void) ;	/* Non-stack (register linkage) use. */

extern void LCP_MarkFlashCodeState (u_int8 fBad) ;

extern void LCPl_RegPortStateInd (u_int32 (* pFcn)(u_int32, u_int32, u_int32)) ;
extern void LCPl_RegVlanParamCmd (u_int32 (* pFcn)(u_int8 *)) ;

#endif	/* _LCP_API_H */
