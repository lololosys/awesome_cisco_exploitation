/* $Id: bt2_main.c,v 1.1.4.1 1996/05/09 14:55:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_flashboot/bt2_main.c,v $
 *------------------------------------------------------------------
 * bt2_main.c -- C code entry point for ACP Secondary (downloadable) Boot.
 *
 * December 21, 1994, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt2_main.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/03  05:20:33  rlowe
 * Fix startup diags so that local loop tests always use offline SAR
 * images, while syndiags loop test always uses run-time SAR images.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:23  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.3  1996/03/20  10:52:35  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.2  1996/03/20  04:06:00  vkrishna
 * Changes to support SAR firmware build as part of Cat5K ATM Module
 * IOS build procedure.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:40  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:08  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.31.2.1.4.2  1996/01/23  06:08:30  rlowe
 * Remove pause for operator prompt when in board debug (jumper) mode.
 * Keep it simple and smooth.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.31.2.1.4.1  1996/01/18  05:02:03  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.31.2.1  1995/11/15  23:10:53  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.32  1995/11/15  21:19:44  rbau
 * enable support for downloading off-line diag sar code
 *
 * Revision 1.31  1995/11/02  04:00:48  rlowe
 * Add (but disable) support for Tx/Rx SAR manufacturing loopback diag code.
 *
 * Revision 1.30  1995/10/27  01:14:00  rlowe
 * Change sequence so that manufacturing off-line diags startup
 * does not collide with normal Boot-2 startup, especially in
 * areas of LCP/BIGA/atmizer driver startups.
 *
 * Revision 1.29  1995/10/13  06:06:46  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.28  1995/09/09  00:15:36  rbau
 * removed function call LCP_SetPmdType. moved in fucntion BDG_DvrInit2
 *
 * Revision 1.27  1995/09/08  20:52:52  rbau
 * modified to use wrapper function to set PMD type in LCP driver.
 *
 * Revision 1.26  1995/09/08  00:34:05  rlowe
 * More fixes relating to doing LCP driver init and system loopbacks.
 *
 * Revision 1.25  1995/09/07  12:08:45  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.24  1995/08/23  20:28:05  vinod
 * ifdef'd out loopback test if fastboot jumper is set
 *
 * Revision 1.23  1995/08/23  19:53:29  vinod
 * take out LCP_Init3 for alpha test. revisit here later
 *
 * Revision 1.22  1995/08/11  01:55:40  rbau
 * added software version infos in the diagnostic results. Provided canned
 * diagnostic results if fastboot jumper is set.
 *
 * Revision 1.21  1995/08/04  02:21:56  rbau
 * call loopback diag if fastboot jumper is set
 *
 * Revision 1.20  1995/08/01  07:08:34  rlowe
 * Display image version info on way up.
 *
 * Revision 1.19  1995/07/27  08:20:16  rlowe
 * When downloading SARs for running loop diags at startup time, use
 * the run-time SAR images instead of following the DEBUG jumper.
 * This allows other code to the use that jumper without requiring
 * serial-line SAR downloads over PMON.
 *
 * Revision 1.18  1995/07/13  00:22:10  rlowe
 * Provide hooks for driving Power-Up Diags from Boot-2.
 *
 * Revision 1.17  1995/06/15  09:28:39  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.16  1995/06/09  11:44:06  rlowe
 * Make SAR resets atomic with respect to Frame Buffer accesses.  Fix
 * for HW bug where Frame-Buf arbitration could hang, thus causing 68k
 * (ACP) bus errors.
 *
 * Revision 1.15  1995/05/13  22:40:11  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.14  1995/05/05  22:34:47  rlowe
 * Have SARs downloaded by Boot Download Glue module.
 *
 * Revision 1.13  1995/05/04  11:28:56  rlowe
 * Add hooks to run downloaded image.
 *
 * Revision 1.12  1995/05/04  10:38:16  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.11  1995/04/28  14:53:21  rlowe
 * Re-structure to more easily support (in-band) downloads (temporarily).
 * Move SAR run-time downloads to a separate routine.  Move SAR PowerUp diags
 * to a separate routine.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_error.h"
#include "lo_memblks.h"
#include "mfp_acp_defs.h"
#include "mfp_hw_defs.h"
#include "acp_c_defs.h"
#include "bcs_api.h"
#include "boot_defs.h"
#include "bt_nvram.h"
#include "lcp_if.h"
#include "bdg_if.h"
#include "varargs.h"
#include "diag_results.h"
#include "../alc_drivers/lcp_defs.h"

extern const u_int8 *	Bt2_BuildInfo [] ;
extern int		diag_main ();
extern DiagResults_t  DiagResults;

u_int8 Bt2_SetupOffLineSarDiags (u_int8 fResetLCP) ;


/* --------------------------------------------------------------------	*/
/*	Do Tx/Rx SAR Power-Up Diags.					*/
/*	Return FALSE on download failure, TRUE on diags attempted.	*/
/* --------------------------------------------------------------------	*/
u_int8 Bt2_DoSar_PUD (register u_int8 * pReg, register p_void (*pFcn) ())
{
	volatile u_int32 *	pSarData ;
	u_int16		MFP_Mask ;
	int			i;

                /* Ensure Tx/Rx SARs are in reset. */
                /* Disable and clear pending Tx/Rx SAR interrupts. */
        PUNCH_WD ;
        BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
        BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

	PUNCH_WD ;
	MFP_Mask = MFP_IMSK_NONE ;
	if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_TXS_PUD, BCSL_BF_NONE)
			   == (p_void)(BCSSR_FAILURE))
		return (FALSE) ;

	PUNCH_WD ;
	if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_RXS_PUD, BCSL_BF_NONE)
			    == (p_void)(BCSSR_FAILURE))
		return (FALSE) ;


		/* Allow polling of Tx/Rx SAR interrupt lines. */
	PUNCH_WD ;
	MFP_MaskIntrW   ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE);
	MFP_EnableIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE);

		/* RLB BOOT2 diag on SAR -- drop reset on TXSAR */
	PUNCH_WD ;
	BDG_ClearReset (ACP_WRRG_TSAR_RESET_) ;
   	puts("released reset on TXSAR \n");

		/* SAR mem test takes 330ms, wait to release reset on RXSAR */
       	for (i=0;i< 200000; i++)
        {
		PUNCH_WD ;		/* dumb delay */
        }

		/* Drop reset on RXSAR, TxSAR will sync with it. */
        PUNCH_WD ;
	BDG_ClearReset (ACP_WRRG_RSAR_RESET_) ;
        puts("released reset on RXSAR \n");

		/* SAR memory test takes 330ms */
        PUNCH_WD ;
        for (i=0;i< 200000; i++)
        {
	       PUNCH_WD ;		/* dumb delay */
        }

		/* Watch for Tx/Rx SAR interrupt lines going active. */
        PUNCH_WD ;
	MFP_Mask = MFP_PollForIntr ((MFP_IMSK_RSAR | MFP_IMSK_TSAR),
				    FALSE, TRUE, TRUE) ;

		/* Put Tx/Rx SARs back into reset. */
		/* Disable and clear pending Tx/Rx SAR interrupts. */
        PUNCH_WD ;
	BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
	BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

        PUNCH_WD ;
	pSarData = (volatile u_int32 *) (ACP_HW_TSAR_RAM_BASE) ;
	printf ("\nTSAR PU result:  Adr $%08x  Exp $%08x  Got $%08x  Tst $%08x\n",
		pSarData [ 0], pSarData [ 1], pSarData [ 2], pSarData [ 3]) ;

        PUNCH_WD ;
	pSarData = (volatile u_int32 *) (ACP_HW_RSAR_RAM_BASE) ;
	printf ("\nRSAR PU result:  Adr $%08x  Exp $%08x  Got $%08x  Tst $%08x\n",
		pSarData [ 0], pSarData [ 1], pSarData [ 2], pSarData [ 3]) ;

        PUNCH_WD ;
	puts ("TxSAR halted ") ;
	if (MFP_Mask & MFP_IMSK_TSAR)
	{
		puts ("(passed PU diags)\n") ;
	}
	else
	{
		puts ("(failed PU diags)\n") ;
		A_DiagFail0 (PUD_TSAR_DIAG, BTNVR_TSARPDF_COUNT, AR_SvcWatchdog,
			     NULL, (MFP_IMSK_RSAR | MFP_IMSK_TSAR), MFP_Mask) ;
	}

        PUNCH_WD ;
	puts ("RxSAR halted ") ;
	if (MFP_Mask & MFP_IMSK_RSAR)
	{
		puts ("(passed PU diags)\n") ;
	}
	else
	{
		puts ("(failed PU diags)\n") ;
		A_DiagFail0 (PUD_RSAR_DIAG, BTNVR_RSARPDF_COUNT, AR_SvcWatchdog,
			     NULL, (MFP_IMSK_RSAR | MFP_IMSK_TSAR), MFP_Mask) ;
	}

	return (TRUE) ;

}	/* Bt2_DoSar_PUD */

/* --------------------------------------------------------------------	*/
/*	If a debug monitor exists, go to it.  Else just return.		*/
/* --------------------------------------------------------------------	*/
void Bt2_DebugMon ()
{
	diag_main(); 	

}	/* Bt2_DebugMon */

/* --------------------------------------------------------------------	*/
/*	Try to scratch out flash images.  Could be tough, as we		*/
/*	are currently running from flash ...				*/
/* --------------------------------------------------------------------	*/
void Bt2_ScrFlash (p_void (*pFcn) ())
{
	PUNCH_WD ;

	if (pFcn != (p_void)(BCSSR_FAILURE))
		(*pFcn) (BCS_API_REV_0, TRUE, BCSSR_SCRATCH_IMAGE,
				BCS_IH_TYPE_2ndBT, BCSL_BF_NONE) ;

}	/* Bt2_ScrFlash */

/* --------------------------------------------------------------------	*/
/*	Do NOT change the parameters to this function without ALSO	*/
/*	changing the calls in the Secondary Boot startup code		*/
/*	(bt2_start.S:__Bt2_Start) and Primary Boot (boot.c:boot, etc)	*/
/*	and BCS (bcs_ServIf.c).						*/
/* --------------------------------------------------------------------	*/
u_int32 Bt2_Main (va_list p_varargs)
{
	u_int8 * pReg ;
	u_int8	 JmpVal ;
	p_void	 (*pFcn) () ;
	u_int8	 LoadVers [BCS_IHL_IMAGE_VERSION] ;
	u_int32	 CallerVersion ;		/* Varargs #1 */
	u_int32	 CallerState ;			/* Varargs #2 */
	void (** CallerVecs) () ;		/* Varargs #3 */
	u_int8 * pPromVersion ;			/* Varargs #4 */


	PUNCH_WD ;
	spl7 () ;		/* Block interrupts used by the scc code. */
	CallerVersion = va_arg (p_varargs, u_int32) ;
	CallerState   = va_arg (p_varargs, u_int32) ;
	CallerVecs    = (void (**)()) (va_arg (p_varargs, u_int32)) ;
	pPromVersion  = (u_int8 *) (va_arg (p_varargs, u_int32)) ;

	MFP_InitAll (FALSE, NULL, MFP_BASE_VECTOR, FALSE,
			TRUE, TRUE, TRUE, TRUE) ;

		/* BIGA driver init routines expect handshake from LCP,  */
		/* so drop LCP out of reset along with BIGA, BEFORE BIGA */
		/* driver is created.                                    */
	pFcn = (p_void) (BCSSR_FAILURE) ;
	pReg = (u_int8 *) (ACP_HW_IOREG_BASE) ;
	JmpVal = pReg [ACP_RDRG_DEBUG] ;

	PUNCH_WD ;
	IOBT_Init () ;
	PUNCH_WD ;
	puts ("\r\nACP Flash Boot of ") ;
	PUNCH_WD ;
	puts (Bt2_BuildInfo) ;
	puts ("\n\n") ;

		/* Ensure that NVRAM LCP/LDI (Boot) pointer is cleared */
		/* on Boot-2 startup.                                  */
	((tAcpBtNvram *)(BOOT_NVRAM_AREA))->pOld_LDI = (u_int32) (NULL) ;


	PUNCH_WD ;
	spl7 () ;				/* Just in case was changed. */
	do {
			/* Get BCS Service entry point.  Don't bother to     */
			/* checksum code as it is already part of this Boot. */
		pFcn = BCS_FindSvc (BCS_API_REV_0, TRUE, BCSL_BF_NONE) ;
		if (pFcn == (p_void)(BCSSR_FAILURE))
			break ;


			/* Get full-image version info. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_READ_IMAGE_VERS,
				BCS_IH_TYPE_VINFO, BCSL_BF_NONE,
				LoadVers, sizeof (LoadVers))
		    == (p_void)(BCSSR_FAILURE))
			break ;

		PUNCH_WD ;
		puts ("\nCatalyst 5000 ATM line card, flash SW ver. ") ;
		puts (LoadVers) ;
		puts ("\n\n") ;
		set_ver_in_diagresults(LoadVers, ACP_sw_ver);

		/* 
		 * Get Boot_1 version number.
		 */
		set_ver_in_diagresults(pPromVersion, ACP_boot1_ver);

			/* Get Boot-2 image version info. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_READ_IMAGE_VERS,
				BCS_IH_TYPE_2ndBT, BCSL_BF_NONE,
				LoadVers, sizeof (LoadVers))
			== (p_void)(BCSSR_FAILURE))
			break ;

		set_ver_in_diagresults(LoadVers, ACP_boot2_ver);

			/* Get IOS BOOT image version info. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_READ_IMAGE_VERS,
				BCS_IH_TYPE_IOSBT, BCSL_BF_NONE,
				LoadVers, sizeof (LoadVers))
			== (p_void)(BCSSR_FAILURE))
			break ;

		set_ver_in_diagresults(LoadVers, ACP_IOS_boot_ver);

		/*
		 * Get TxSAR Run-time image version info.
		 */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_READ_IMAGE_VERS,
				BCS_IH_TYPE_TXS_RT, BCSL_BF_NONE,
				LoadVers, sizeof (LoadVers))
			== (p_void)(BCSSR_FAILURE))
			break ;

		set_ver_in_diagresults(LoadVers, TXSAR_sw_ver);

		/*
		 * Get RxSAR Run-time image version info.
		 */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_READ_IMAGE_VERS,
				BCS_IH_TYPE_RXS_RT, BCSL_BF_NONE,
				LoadVers, sizeof (LoadVers))
			== (p_void)(BCSSR_FAILURE))
			break ;

		set_ver_in_diagresults(LoadVers, RXSAR_sw_ver);

		PUNCH_WD ;
		puts ("\n") ;

		/* Check for mfg off-line diags.  Do NOT return if enabled. */
		PUNCH_WD ;
		diag_main();		/* Do NOT return if mfg diags enab. */


		/* If NOT in fastboot mode, run SAR Power-Up Diags. */
		PUNCH_WD ;
		if (JmpVal & (1 << ACP_RRB_JMP_FASTBOOT_))
		{						/* J3 */
			if (! Bt2_DoSar_PUD (pReg, pFcn))
				break ;
		}


		/* Init LCP driver and do SCP handshakes with NMP.      */
		/* Then, do real or canned system diags.  SCP hand-     */
		/* shakes are completed when diag results are reported. */
	        PUNCH_WD ;
		if (! Bt2_SetupOffLineSarDiags (FALSE))
			break ;

		PUNCH_WD ;
		if (! BDG_PUD_Init ())
			break ;


		/* Ensure that NVRAM LCP/LDI (Boot) pointer is set */
		/* when available and LCP driver is initialized.   */
		((tAcpBtNvram *)(BOOT_NVRAM_AREA))->pOld_LDI = (u_int32) & LDI ;

		PUNCH_WD ;
		ALC_SysDiag (((JmpVal & (1 << ACP_RRB_JMP_FASTBOOT_)) != 0),
			     pFcn) ;


			/* Prep the SARs for the Kernel. */
                PUNCH_WD ;
		if (! BDG_DownldSars (pFcn, BDG_DL_SAR_JMP))
			break ;

		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_RUN_VEC_TABL,
				BCS_IH_TYPE_IOSBT, BCSL_BF_NONE)
		    == (p_void)(BCSSR_FAILURE))
			break ;

			/* Hopefully we do NOT return. */

	} while (FALSE) ;

		/* --------------------------------------------	*/
		/* Come here if error downloading flash images,	*/
		/* or if IOS Boot fails/returns to us.  Fall to	*/
		/* any debug monitor if available else scratch	*/
		/* flash and reset so that Primary Boot will	*/
		/* re-download images from NMP.			*/
		/* --------------------------------------------	*/

	PUNCH_WD ;
	spl7 () ;				/* Just in case was changed. */
	Bt2_DebugMon () ;			/* Does debug monitor exist ? */

		/* In case of failure, just return to PROM boot */
		/* which should then await download from NMP.   */
#if	0
	PUNCH_WD ;				/* Come here to scratch flash.*/
	Bt2_ScrFlash (pFcn) ;

	A_ResetCPU () ;				/* Reset to Primary Boot code.*/
#endif

	PUNCH_WD ;				/* Here if all else failed. */
	return (PUD_FLSH_CSUM) ;		/* Return to Primary Boot   */
						/*   which should download. */

}	/* Bt2_Main */

/* --------------------------------------------------------------------	*/
/*	Load Tx and Rx SARs with off-line diags code, ie. for doing	*/
/*	manufacturing loopbacks.  SARs are put into RESET state here.	*/
/*	While BIGA is not touched here, the LCP will be RESET if so	*/
/*	indicated.  The NTC is also RESET here.				*/
/*	Returns TRUE on success, otherwise FALSE.			*/
/* --------------------------------------------------------------------	*/
u_int8 Bt2_SetupOffLineSarDiags (u_int8 fResetLCP)
{
	p_void	(*pFcn) () ;
	u_int8	RC = FALSE ;
	volatile u_int8 *	pReg ;


                /* Ensure Tx/Rx SARs are in reset. */
                /* Disable and clear pending Tx/Rx SAR interrupts. */
        PUNCH_WD ;
        BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
        BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

		/* Reset LCP if needed; it will be released later  */
		/* during the normal driver startup sequence.  The */
		/* NTC is reset and released here too.             */
        PUNCH_WD ;
	pReg = (u_int8 *) (ACP_HW_IOREG_BASE) ;
	pReg [ACP_WRRG_NTC_RESET_] = ON_ ;
	if (fResetLCP)
	{
		pReg [ACP_WRRG_LCP_RESET_] = ON_ ;
	}
	BDG_ClearReset (ACP_WRRG_NTC_RESET_) ;

 
	do {
			/* Get BCS Service entry point.  Don't bother to     */
			/* checksum code as it is already part of this Boot. */
		PUNCH_WD ;
		pFcn = BCS_FindSvc (BCS_API_REV_0, TRUE, BCSL_BF_NONE) ;
		if (pFcn == (p_void)(BCSSR_FAILURE))
			break ;

                	/* Load SARs with respective off-line diags code. */
                PUNCH_WD ;
                if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
                                BCS_IH_TYPE_TXS_ODIAG, BCSL_BF_NONE)
                           == (p_void)(BCSSR_FAILURE))
			break ;
 
                PUNCH_WD ;
                if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
                                BCS_IH_TYPE_RXS_ODIAG, BCSL_BF_NONE)
                            == (p_void)(BCSSR_FAILURE))
			break ;
 
                        /* Assume caller will release SAR resets. */
                PUNCH_WD ;
                puts ("Loaded off-line diags code to TX/RX SARs\n") ;

                PUNCH_WD ;
		RC = TRUE ;

	} while (FALSE) ;

	return (RC) ;

}	/* Bt2_SetupOffLineSarDiags */
