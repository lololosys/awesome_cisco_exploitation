/* $Id: bdg_if.h,v 1.1.4.1 1996/05/09 14:55:30 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/bdg_if.h,v $
 *------------------------------------------------------------------
 * bdg_if.h -- Interface definitions for Boot's downloader glue.
 *
 * May 3, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bdg_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:30  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/03  05:20:40  rlowe
 * Fix startup diags so that local loop tests always use offline SAR
 * images, while syndiags loop test always uses run-time SAR images.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:59  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:26  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.19  1995/10/21  04:34:21  vinod
 * Added external declaration BDG_HasExploderChecksumOnly(void).
 *
 * Revision 1.18  1995/10/14  03:35:09  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.17  1995/08/31  12:17:04  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.16  1995/08/03  02:04:56  rlowe
 * Add some new APIs to support clean-up of BDG source.
 *
 * Revision 1.15  1995/07/27  22:03:47  rlowe
 * Merge ALC combined binary image support into BDG (Boot download
 * glue).
 *
 * Revision 1.14  1995/07/26  04:08:37  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.13  1995/07/13  00:05:17  rlowe
 * Add BDG_PUD_Init and other hooks so that Power-Up Diags can be driven
 * from Boot-2, while allowing compatibility with existing Kernel startup
 * code.
 *
 * Revision 1.12  1995/06/29  17:27:45  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.11  1995/06/15  09:22:10  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 *
 * Revision 1.10  1995/06/13  04:45:22  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.9  1995/06/09  11:43:40  rlowe
 * Make SAR resets atomic with respect to Frame Buffer accesses.  Fix
 * for HW bug where Frame-Buf arbitration could hang, thus causing 68k
 * (ACP) bus errors.
 *
 * Revision 1.8  1995/06/09  06:16:31  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to ACP.
 * Setup for ACP Rev-2 HW address map.
 *
 * Revision 1.7  1995/06/02  01:02:01  rlowe
 * Changes for ACP Rev 2 HW.  Also support flash code from CBI tools for
 * download.
 *
 * Revision 1.6  1995/05/26  08:36:44  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.5  1995/05/20  03:07:22  rlowe
 * Create separate routine for sync-ing up ALC device driver startup.
 *
 * Revision 1.4  1995/05/13  22:34:07  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.3  1995/05/05  22:26:48  rlowe
 * Have SARs downloaded by Boot Download Glue module.
 *
 * Revision 1.2  1995/05/04  11:28:57  rlowe
 * Add hooks to run downloaded image.
 *
 * Revision 1.1  1995/05/04  10:38:25  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BDG_IF_H
#define	__BDG_IF_H

	/* These are flag values to BDG_DownldSars. */
#define	BDG_DL_SAR_PMON		(0)
#define	BDG_DL_SAR_RT		(-1)
#define	BDG_DL_SAR_JMP		(1)


extern void	BDG_ResetFrBufUsr (u_int32 Which) ;
extern void	BDG_ClearReset (u_int32 Which) ;
extern u_int8	BDG_LcpInit (s_int8 fDbgPrts, u_int8 fPUD, u_int8 fKernel,
							u_int8 fPromDownld) ;
extern void	BDG_EraseFlash (void) ;

extern u_int8	BDG_NoExploder (void) ;
extern u_int8	BDG_HasExploderChecksumOnly (void) ;
extern u_int8	BDG_HasExploder (void (*pCallBack)(void)) ;
extern u_int8	BDG_SetupExploder (u_int8 fReset, void (*pCallBack)(void)) ;
extern u_int32	BDG_GetDownldSize (u_int32 Base) ;
extern u_int32	BDG_GetDownldSizeX (void) ;
extern u_int32	BDG_VerifyFlash (register u_int32 * pFlash,
				 register u_int32 * pRam, register u_int32 Len);
extern void	BDG_RunLoad  (void) ;

extern u_int8	BDG_DvrInit (void) ;
extern u_int8	BDG_Download (u_int8 fReset, void (*pCallBack)(void)) ;
extern u_int8	BDG_DnldDvrCleanUp (void) ;

extern u_int8	BDG_DownldSars2 (u_int8 JmpVal, p_void (*pFcn) (void)) ;
extern u_int8	BDG_DownldSars (p_void (*pFcn) (void),
					s_int8 fForceSarLoad) ;
extern u_int8	BDG_CamInit  (void) ;
extern u_int8	BDG_DvrInit2 (u_int8 fPUD, u_int8 fKernel) ;
extern u_int8	BDG_PUD_Init (void) ;
extern u_int8	BDG_SAR_InitRT (p_void (*pFcn) (void)) ;

#endif	/* __BDG_IF_H */
