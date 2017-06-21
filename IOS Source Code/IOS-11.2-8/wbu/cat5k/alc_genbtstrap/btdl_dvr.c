/* $Id: btdl_dvr.c,v 1.1.4.1 1996/05/09 14:55:16 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_genbtstrap/btdl_dvr.c,v $
 *------------------------------------------------------------------
 * btdl_dvr.c -- common driver init sequencing and interface code
 *		 for Boot and Kernel (start-up) use.  Also has
 *		 common (CBI) flash interfacing code.
 *
 * May 3, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: btdl_dvr.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:16  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:44  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:53  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:12  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.14  1995/10/14  03:35:04  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.13  1995/10/13  06:06:48  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.12  1995/09/07  12:08:53  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.11  1995/07/26  04:14:47  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.10  1995/07/13  00:06:48  rlowe
 * Add BDG_PUD_Init and other hooks so that Power-Up Diags can be driven
 * from Boot-2, while allowing compatibility with existing Kernel startup
 * code.
 *
 * Revision 1.9  1995/06/29  17:28:32  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.8  1995/06/15  09:28:24  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.7  1995/06/13  23:29:47  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.6  1995/06/13  04:47:10  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.5  1995/06/09  05:48:43  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to ACP.
 * Integrate to latest LCP driver (message model, startup & HW handshakes.
 *
 * Revision 1.4  1995/06/02  01:09:24  rlowe
 * Add support for burning in downloaded code to flash.
 *
 * Revision 1.3  1995/05/27  02:45:20  sakumar
 * Fix IOS init-time run-to-completion by NOT allowing ATM driver
 * edisms (use driver polling mode instead) until IOS init is done.
 *
 * Revision 1.2  1995/05/26  08:38:08  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.1  1995/05/20  03:07:14  rlowe
 * Create separate routine for sync-ing up ALC device driver startup.
 *
 * Revision 1.5  1995/05/13  22:40:06  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.4  1995/05/05  22:33:11  rlowe
 * Have SARs downloaded by Boot Download Glue module.
 *
 * Revision 1.3  1995/05/04  11:28:51  rlowe
 * Add hooks to run downloaded image.
 *
 * Revision 1.2  1995/05/04  10:38:02  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.1  1995/05/04  09:51:41  rlowe
 * Add "glue" code to get Boot-mode code downloads going over in-band IPC.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "lcp_if.h"
#include "mc68ec030_defs.h"
#include "cbi_defs.h"
#include "flash.h"
#include "bdg_if.h"
#include "bt_nvram.h"


/**/
/* --------------------------------------------------------------------	*/
/*	Reset a Frame Buffer user (S-bus device) such that the f-buf	*/
/*	will not be locked.  Otherwise, the 68k (ACP) could bus error	*/
/*	on the next fr-buf access.  This applies to the BIGA and SARs.	*/
/* --------------------------------------------------------------------	*/
void BDG_ResetFrBufUsr (u_int32 Which)
{
	register volatile u_int8 *	pReg ;
	u_int32				Val ;


		/* We lock the Frame Buffer memory first, to ensure */
		/* that the SARs (or BIGA) cannot be accessing the  */
		/* Frame Buffer when they reset.  Otherwise they    */
		/* could be reset during an access to the Frame     */
		/* Buffer and leave the HW (s-bus) arbitration in a */
		/* bad state.  ACP (68k) can then bus error on its  */
		/* next Frame Buffer access.                        */
	pReg = (volatile u_int8 *) (ACP_HW_IOREG_BASE) ;
	Val = * (volatile u_int32 *) (ACP_HW_FRBUF_BASE | ACP_HW_LOCK_ADRBIT) ;

		/* Now that the Frame Buffer is locked, reset the device. */
	pReg [Which] = ON_ ;

		/* Device is reset, now unlock the Frame Buffer. */
	Val = * (volatile u_int32 *) (ACP_HW_FRBUF_BASE) ;

		/* Punch the watchdog AFTER everything is taken */
		/* care of.  That way if we hang above, the     */
		/* watchdog can still reset us (ACP).           */
		/* Disable/clear any pending SAR interrupts.    */
	PUNCH_WD ;
	if (Which == ACP_WRRG_TSAR_RESET_)
		MFP_DisableIntrW (MFP_IMSK_TSAR) ;

	else  if (Which == ACP_WRRG_RSAR_RESET_)
		MFP_DisableIntrW (MFP_IMSK_RSAR) ;

}	/* BDG_ResetFrBufUsr */

/**/
/* --------------------------------------------------------------------	*/
/*	Clear the reset state on a particular device.			*/
/* --------------------------------------------------------------------	*/
void BDG_ClearReset (u_int32 Which)
{
	PUNCH_WD ;
	* (volatile u_int8 *) (ACP_HW_IOREG_BASE + Which) = OFF_ ;

}	/* BDG_ClearReset */

/**/
/* --------------------------------------------------------------------	*/
/*	This goes through the LCP startup handshakes and ensures that	*/
/*	LCP (and NMP) data from startup is available from LCP driver.	*/
/*	We also groom the BIGA here.					*/
/*	This routine is expected to be called from within BDG only,	*/
/*	thus it can assume that caller has downloaded SARs, knows	*/
/*	about diag results, etc.					*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_LcpInit (s_int8 fDbgPrts, u_int8 fPUD, u_int8 fKernel,
						u_int8 fPromDownld)
{
	p_void	pOldLDI = NULL ;


		/* Complete LCP driver init.  BIGA and LCP must be	*/
		/* running before ATM driver will get to HW properly.	*/
		/* Drop BIGA from reset first, as LCP reset controls	*/
		/* both LCP and SAGE.  SAGE uses BIGA for DMA.		*/
	PUNCH_WD ;
	BDG_ResetFrBufUsr (ACP_WRRG_BIGA_RESET_) ;
	BDG_ClearReset (ACP_WRRG_BIGA_RESET_) ;

		/* Lookup the previous (Boot) LCP driver data if needed. */
	if (fKernel)
		pOldLDI = (p_void)
			  (((tAcpBtNvram *)(BOOT_NVRAM_AREA))->pOld_LDI) ;

		/* NEVER force LCP reset, even in Boot mode.    */
		/* This is done separately by Boot-1 to support */
		/* NMP/manufacturing diags.                     */
	PUNCH_WD ;
	LCP_DriverInit_1 (NULL, FALSE, TRUE, fKernel, pOldLDI) ;

	LCP_SetDbgPrt (fDbgPrts) ;

	PUNCH_WD ;
	MFP_MaskIntrW   ((MFP_IMSK_LCPRD | MFP_IMSK_LCPTP), FALSE) ;
	MFP_EnableIntrW ((MFP_IMSK_LCPRD | MFP_IMSK_LCPTP), FALSE) ;

	PUNCH_WD ;
	LCP_DriverInit_2 (FALSE, FALSE, TRUE) ;

#ifndef	ACP_REV0_LCP
	PUNCH_WD ;
	if (! fPUD)
	{
		if (fPromDownld)
			LCP_MarkFlashCodeState (TRUE) ;

		LCP_DriverInit_3 (FALSE, TRUE, TRUE, 0, NULL) ;
		LCP_DriverInit_5 (FALSE, (! fKernel)) ;
	}
#endif

	PUNCH_WD ;
	return (TRUE) ;		/* Indicate all is well. */

}	/* BDG_LcpInit */

/**/
/* --------------------------------------------------------------------	*/
/*	Cause the entire flash memory to be erased.			*/
/*	Run with CPU Data cache disabled since flash contents will	*/
/*	change (erasure) underneath us.					*/
/* --------------------------------------------------------------------	*/
void BDG_EraseFlash ()
{
	u_int32		OldCache ;
	u_int32		Old_ACU  ;


		/* Flush/disable D-cache.  Also flush I-cache in */
		/* case not already enabled (debug jumper case). */
		/* Buzz loop timing depends on I-cache enabled.  */
		/* Flush caches after changing ACU state.        */
	PUNCH_WD ;
	Old_ACU  = Sys_SetCpuACUx (ACU_FLASH_CTL, CBI_FLASH_ACU) ;
	OldCache = Sys_SetCpuCacheX (CPU030_ENAB_CACHES | CPU030_FLUSH_CACHES) ;


	PUNCH_WD ;
	(void) erase (CBI_FIRST_FLASH_BLK, CBI_NBR_FLASH_BLKS) ;


                /* Flush caches after changing ACU state. */
	PUNCH_WD ;
	(void) Sys_SetCpuACUx (ACU_FLASH_CTL, Old_ACU) ;
	(void) Sys_SetCpuCacheX (OldCache | CPU030_FLUSH_CACHES) ;

}	/* BDG_EraseFlash */

