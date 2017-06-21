/* $Id: boot.c,v 1.1.4.1 1996/05/09 14:59:56 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/boot.c,v $
 *-----------------------------------------------------------------
 * boot.c --  C code entry point for either ROM-based 68k Boots.
 *		(Based on Catalyst Boot.)
 *
 * November 30, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boot.c,v $
 * Revision 1.1.4.1  1996/05/09  14:59:56  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:24  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:05  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.22.6.2  1996/01/23  06:08:23  rlowe
 * Remove pause for operator prompt when in board debug (jumper) mode.
 * Keep it simple and smooth.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.22.6.1  1996/01/18  05:02:12  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.22  1995/10/21  04:32:36  vinod
 * Cosmetic clean up
 *
 * Revision 1.21  1995/10/14  08:46:48  rlowe
 * Fixes to allow NMP/SCP manufacturing diags to be run on system powerup.
 *
 * Revision 1.20  1995/10/13  06:06:41  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_error.h"
#include "lo_memblks.h"
#include "mfp_acp_defs.h"
#include "acp_c_defs.h"
#include "bcs_api.h"
#include "lcp_if.h"
#include "bdg_if.h"
#include "bt_globvar.h"


#define	BT_MAX_DNLD_RETRIES	4


extern void (* (_bt_VectorTbl []))() ;
extern const u_int8 * BT_BuildInfo [] ;


/* --------------------------------------------------------------------	*/
/*	Decode reset reason register for the operator.			*/
/*	Do NOT clear it, so other S/W can read it still.		*/
/* --------------------------------------------------------------------	*/
void BT_GiveResetReasons (void)
{
	u_int8	fPred ;
	u_int8	RstReason ;
	u_int8	Count ;


	fPred = FALSE ;
	RstReason = ((* (volatile u_int8 *)
		        (C_ADDR_HW_REGS (ACP_RDRG_RST_REASONS))) & 0x78) ;

	printf ("%sReset reason $%02x (", bts_PUD_NewLine, RstReason) ;

	for (Count = ACP_RRB_BUTTON_RST ;
				Count <= ACP_RRB_WATCHDOG_RST ; ++ Count)
	{
		if (RstReason & (1 << Count))
		{
			if (fPred)
			{
				printf (", ") ;
			}
			switch (Count)
			{
			    case (ACP_RRB_BUTTON_RST):
				printf ("manual") ;
				break ;
			    case (ACP_RRB_SOFTWARE_RST):
				printf ("software") ;
				break ;
			    case (ACP_RRB_BACKPLANE_RST):
				printf ("backplane") ;
				break ;
			    case (ACP_RRB_WATCHDOG_RST):
				printf ("watchdog") ;
				break ;
			    default:
				printf ("unknown") ;
				break ;
			}
			fPred = TRUE ;
		}
	}

	if (! fPred)
	{
		printf ("none") ;
	}
	printf (")") ;

}	/* BT_GiveResetReasons */

/* --------------------------------------------------------------------	*/
/*	Try to download SYNALC image to flash.				*/
/*	Returns non-zero length of ACP image, or 0 on any error.	*/
/* --------------------------------------------------------------------	*/
u_int32 BT_DownldFlash (void)
{
	u_int32		size = 0 ;
	u_int8		res  = 0 ;


		/* Normally do NOT return if download succeeds.  */
		/* But put in some code anyway for completeness. */
	PUNCH_WD ;
	if (res = BDG_Download (TRUE, A_SvcWatchdog))
	{
		PUNCH_WD ;
		size = BDG_GetDownldSizeX () ;
	}

	PUNCH_WD ;
	printf ("%sDownload to flash", bts_PUD_NewLine) ;

	PUNCH_WD ;
	if (res && size)
	{
		printf (" succeeded, %u (0x%x) bytes", size, size) ;
	}
	else	/* Download or its checksum failed. */
	{
		puts (bts_PUD_Failed) ;
	}

	PUNCH_WD ;
	puts (bts_PUD_NewLine0) ;

	PUNCH_WD ;
	return (size) ;

}	/* BT_DownldFlash */

/* --------------------------------------------------------------------	*/
/*	ACP Primary Boot "C" entry point.				*/
/* --------------------------------------------------------------------	*/
void boot (void)
{
	p_void		(*pFcn) () ;
	u_int8 *	pVer ;
	u_int8 *	pVer2 ;
	u_int32		FailReason ;
	p_void		BCS_Res ;
	u_int8		fFastBoot ;


	/*
	 * Errors from this point are recovered by forcing red Status LED.
	 * Do NOT reboot CPU as we will get an infinite loop.  User can
	 * force reboot from NMP.
	 *
	 * If we are in FASTBOOT mode, bypass any diags that we can.
	 */
    PUNCH_WD ;
    pFcn  = NULL ;
    pVer  = NULL ;
    pVer2 = NULL ;
    FailReason = PUD_BT_EXCEPT ;
    BCS_Res    = NULL ;
    fFastBoot  = C_CHK_JMPR (ACP_RRB_JMP_FASTBOOT_) ;

    do {
		/* Note that LCP DPRAM diags have already been done, */
		/* and LCP has been dropped from reset already.      */
		/* NEVER reset the LCP unless the ACP is reset too ! */
		/* LCP is needed for mfg burn-in of C5k boxes.  This */
		/* is time-critical on powerup, so do it as soon as  */
		/* possible.  Groom MFP now as it may be needed.     */
	PUNCH_WD ;
	MFP_InitAll (FALSE, NULL, MFP_BASE_VECTOR,
			FALSE, TRUE, TRUE, TRUE, TRUE) ;

	PUNCH_WD ;
	spl7 () ;				/* Block interrupts used */
	IOBT_Init () ;				/*   by the scc code.    */


		/* Continue with normal start-up sequence. */
	PUNCH_WD ;
	if (fFastBoot)
	{
		pVer  = "0.000" ;		/* For higher-level code. */
		pVer2 = "FASTBOOT" ;		/* For console info. */
	}
	else	/* Get PROM image version info. */
	{
		pVer  = BCSp_VerifyProm () ;
		pVer2 = pVer ;
	}

	PUNCH_WD ;
	printf ("%s%s", bts_PUD_NewLine, "ROM checksum and validation") ;
	PUNCH_WD ;
	if (fFastBoot)
	{
		puts (bts_PUD_Bypass) ;
	}
	else  if ((pVer == NULL) || (pVer == (u_int8 *) (BCSSR_FAILURE)))
	{
		puts (bts_PUD_Failed) ;
		FailReason = PUD_BOOT_CSUM ;
		break ;				/* Fail to red LED. */
	}
	else	puts (bts_PUD_Passed) ;

	PUNCH_WD ;
	printf (", image version %s", pVer2) ;


		/* Continue on with Boot-time initialization. */

		/* Do not clear the reset reason register, let some */
		/* higher-level code do that.  Thus the next chunk  */
		/* of code to run will be able to read it too.      */
	PUNCH_WD ;
	BT_GiveResetReasons () ;

		/* Test other RAM areas. */
	PUNCH_WD ;
	btPUD_PowerUpDiags (fFastBoot) ;


	/*
	 * Errors after this point are recovered by downloading.
	 */
	PUNCH_WD ;
	spl7 () ;			/* Just in case was changed. */

	do {
			/* If SPARE/SW1 jumper is set force download. */
		PUNCH_WD ;
		printf ("%sFlash s/w override jumper", bts_PUD_NewLine) ;
		PUNCH_WD ;
		if (C_CHK_JMPR (ACP_RRB_JMP_SPARE_))
		{
			puts (bts_PUD_Enabled) ;
			break ;
		}
		else	puts (bts_PUD_Disabled) ;

		PUNCH_WD ;
		puts (bts_PUD_NewLine0) ;

			/* Get and validate BCS Service entry point. */
		PUNCH_WD ;
		pFcn = BCS_FindSvc (BCS_API_REV_0, TRUE, BCSL_BF_DO_CKSUM) ;
		PUNCH_WD ;
		printf ("%sFlash code server lookup", bts_PUD_NewLine) ;
		if (pFcn == (p_void)(BCSSR_FAILURE))
		{
			puts (bts_PUD_Failed) ;
			break ;
		}
		else	puts (bts_PUD_Passed) ;


			/* Check with BCS for Boot extension (replacement). */
		PUNCH_WD ;
		BCS_Res = (*pFcn) (BCS_API_REV_0, TRUE, BCSSR_BOOT_EXTENSION,
				   BCS_IH_TYPE_2ndBT, BCSL_BF_NONE) ;
		PUNCH_WD ;
		printf ("%sBoot extension check", bts_PUD_NewLine) ;
		if (BCS_Res == (p_void) (BCSSR_FAILURE))
		{
			puts (bts_PUD_Failed) ;
			break ;
		}
		else	puts (bts_PUD_Passed) ;
		puts ("\n") ;


			/* Get BCS to validate all other flash images. */
		PUNCH_WD ;
		if (! fFastBoot)
		{
			BCS_Res = (*pFcn) (BCS_API_REV_0, TRUE,
					   BCSSR_WALK_IMAGES,
					   BCS_IH_TYPE_2ndBT,
					   BCSL_BF_DO_CKSUM) ;
		}
		PUNCH_WD ;
		printf ("%sFlash sub-image walk", bts_PUD_NewLine) ;
		if (fFastBoot)
		{
			puts (bts_PUD_Bypass) ;
		}
		else  if (BCS_Res == (p_void) (BCSSR_FAILURE))
		{
			puts (bts_PUD_Failed) ;
			break ;
		}
		else	puts (bts_PUD_Passed) ;


			/* Ask BCS to kick us into the Secondary Boot.*/
			/* Assume that BCS is part of Secondary Boot, */
			/* so no further image checksum is needed.    */
			/* The last four arguments are for 2nd Boot.  */
		PUNCH_WD ;
		printf ("%sBooting into flash...\r\n\r", bts_PUD_NewLine) ;
		BCS_Res = (*pFcn) (BCS_API_REV_0, TRUE, BCSSR_RUN_BOOT,
				   BCS_IH_TYPE_2ndBT, BCSL_BF_NONE,
				   1, 1, _bt_VectorTbl, pVer) ;
		PUNCH_WD ;
		printf ("%sBoot attempt into flash", bts_PUD_NewLine) ;
		if (BCS_Res == (p_void) (BCSSR_FAILURE))
		{
			puts (bts_PUD_Failed) ;
			break ;
		}
		else	puts (" RETURNED") ;

			/* Hopefully we do NOT return. */
			/* If we do, try a download to recover. */

	} while (FALSE) ;


		/* --------------------------------------------- */
		/* Come here if error finding flash image, or if */
		/* that image (Secondary Boot) returns to us.    */
		/* Download new image to flash, then reset.      */
		/* --------------------------------------------- */
	PUNCH_WD ;
	spl7 () ;			/* Just in case was changed. */

	PUNCH_WD ;
	puts (bts_PUD_NewLine0) ;
	printf ("%sDownloading to flash from NMP...\r\n", bts_PUD_NewLine) ;

	PUNCH_WD ;
	BT_DownldFlash () ;		/* Try to download code via NMP. */

	PUNCH_WD ;
	printf ("%sForcing reset after download...\r\n\r", bts_PUD_NewLine) ;
	A_ResetCPU () ;			/* Reboot to restart after dnld. */

	PUNCH_WD ;
	FailReason = PUD_DNLD_CSUM ;	/* Hang board if get here...     */
	break ;				/* This is as far as we can go... */

    } while (FALSE) ;


    /*
     * Final resort:  force red Status LED here and hang CPU.
     */
    PUNCH_WD ;
    spl7 () ;				/* Just in case was changed. */

	/* Note special failure output format, same as assembly code uses. */
    PUNCH_WD ;
    printf ("%s%s%08x", bts_PUD_NewLine, bts_PUD_Hangup, FailReason) ;
    puts (bts_PUD_NewLine0) ;
    puts (bts_PUD_NewLine0) ;

    PUNCH_WD ;
    while (1)
    {
	    A_FailLeds (LOER_STD_LED_FLASH, FailReason,
			0xffffffff, AR_SvcWatchdog) ;
    }

}	/* boot */
