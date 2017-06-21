/* $Id: btdl_sars.c,v 1.1.4.1 1996/05/09 14:55:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_genbtstrap/btdl_sars.c,v $
 *------------------------------------------------------------------
 * btdl_sars.c -- C code to handle SARs at downloading/init time.
 *
 * May 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: btdl_sars.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/03  05:20:38  rlowe
 * Fix startup diags so that local loop tests always use offline SAR
 * images, while syndiags loop test always uses run-time SAR images.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:54  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:46  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:14  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.16.2.1  1995/12/04  23:24:10  rlowe
 * Allow SARs to be downloaded with run-time or PMON code in any combination.
 *
 * Revision 1.16  1995/10/14  03:35:06  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.15  1995/10/13  06:06:49  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.14  1995/09/20  21:38:22  rbau
 * put back the code to read PMD type. was ifdef'd out
 *
 * Revision 1.13  1995/09/09  03:28:22  rlowe
 * Back out previous change (to set PMD type) as it causes IOS kernel
 * image to get link errors.
 *
 * Revision 1.12  1995/09/09  00:15:54  rbau
 * set PMD type in LCP driver.
 *
 * Revision 1.11  1995/07/27  22:14:05  rlowe
 * Add ALC combined binary image support to BDG (Boot download glue).
 *
 * Revision 1.10  1995/07/26  04:14:53  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.9  1995/07/24  22:18:46  rbau
 * Allow LCP driver debug prints for PUD (power-up diags) case.  By RKL.
 *
 * Revision 1.8  1995/07/13  00:07:01  rlowe
 * Add BDG_PUD_Init and other hooks so that Power-Up Diags can be driven
 * from Boot-2, while allowing compatibility with existing Kernel startup
 * code.
 *
 * Revision 1.7  1995/06/29  17:28:35  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.6  1995/06/15  09:28:27  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.5  1995/06/13  04:47:13  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.4  1995/06/09  23:27:05  rlowe
 * Re-allow fix to lock Frame Buffer when resetting s-bus devices (sars, biga).
 *
 * Revision 1.3  1995/06/09  12:00:07  rlowe
 * Temporarily suspend doing the Frame Buffer lock on SAR resets.
 * We are getting watchdog timeouts (hanging elsewhere in the system).
 *
 * Revision 1.2  1995/06/09  11:44:01  rlowe
 * Make SAR resets atomic with respect to Frame Buffer accesses.  Fix
 * for HW bug where Frame-Buf arbitration could hang, thus causing 68k
 * (ACP) bus errors.
 *
 * Revision 1.1  1995/05/13  22:40:07  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "bcs_api.h"
#include "bdg_if.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "lcp_if.h"
#include "lcp_api.h"
#include "vlan_defs.h"
#include "cam_if.h"
#include "sar_if.h"
#include "atmdrv_if.h"


/**/
/* --------------------------------------------------------------------	*/
/*	Download run-time (or PMON) code to Tx/Rx SARs.			*/
/*	Return FALSE on any download failure, TRUE on success.		*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DownldSars2 (u_int8 JmpVal, register p_void (*pFcn) ())
{

		/* Ensure Tx/Rx SARs are in reset. */
		/* Disable and clear pending Tx/Rx SAR interrupts. */
	PUNCH_WD ;
	BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
	BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

                /* For Debug case, load SARs with PMON.  Otherwise */
		/* use their run-time code.                        */
	PUNCH_WD ;
	switch (JmpVal)
	{
	    case 0x00:		/* Load both SARs with PMON. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_TXS_PMON, BCSL_BF_NONE)
			   == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_RXS_PMON, BCSL_BF_NONE)
			    == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

			/* SAR reset released by ATM driver at kernel init. */
		PUNCH_WD ;
                puts("Loaded PMON code to TX/RX SARs\n");
		break ;

	    case 0x0f:		/* Load RSAR with PMON, TSAR with run-time. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_TXS_RT, BCSL_BF_NONE)
			   == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_RXS_PMON, BCSL_BF_NONE)
			    == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
                puts("Loaded run-time code to TXSAR\n");
                puts("Loaded PMON code to RXSAR\n");
		break ;

	    case 0xf0:		/* Load TSAR with PMON, RSAR with run-time. */
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_TXS_PMON, BCSL_BF_NONE)
			   == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_RXS_RT, BCSL_BF_NONE)
			    == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
                puts("Loaded PMON code to TXSAR\n");
                puts("Loaded run-time code to RXSAR\n");
		break ;

	    case 0xff:		/* Load both SARs with run-time images. */
	    default:
		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_TXS_RT, BCSL_BF_NONE)
			   == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

		PUNCH_WD ;
		if ((*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_RXS_RT, BCSL_BF_NONE)
			    == (p_void)(BCSSR_FAILURE))
			return (FALSE) ;

			/* SAR reset released by ATM driver at kernel init. */
		PUNCH_WD ;
                puts("Loaded run-time code to TX/RX SARs\n");
		break ;
	}

	PUNCH_WD ;
	return (TRUE) ;

}	/* BDG_DownldSars2 */

/**/
/* --------------------------------------------------------------------	*/
/*	Download run-time (or PMON) code to Tx/Rx SARs.			*/
/*	Return FALSE on any download failure, TRUE on success.		*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DownldSars (register p_void (*pFcn) (), s_int8 fForceSarLoad)
{
	u_int8		JmpVal ;


	if (fForceSarLoad == BDG_DL_SAR_PMON)
	{
		JmpVal = 0x00 ;				/* Force PMON. */
	}
	else  if (fForceSarLoad <= BDG_DL_SAR_RT)
	{
		JmpVal = 0xff ;				/* Force run-time. */
	}
	else					/* Use appropriate jumper. */
	{
			/* Use MFG jumper. */
		JmpVal = (* (volatile u_int8 *) (ACP_HW_MFP_BASE + MFP_RO_GPDR)
			  & (MFP_GPI_MFGJMP_)) ;
	}


	return (BDG_DownldSars2 (JmpVal, pFcn)) ;

}	/* BDG_DownldSars */

/**/
/* --------------------------------------------------------------------	*/
/*	Do (TSAR) CAM init startup.  This is separate from driver init	*/
/*	so that kernel-time (not boot-time) startup can allow the MFP	*/
/*	to take interrupts.  This is because CAM transactions require	*/
/*	the ATM driver to handshake with the TSAR.  These handshakes	*/
/*	are handled by polling in the Boots, and interrupts in kernel.	*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_CamInit ()
{
	tLCP_MacAddrU	MyMacU ;
	tCOLOR		Color  ;
	u_int8		fDrvMod ;
	u_int8		RC ;


		/* In-band IPC is on the internal Synergy vlan. */
	PUNCH_WD ;
	Color = (tCOLOR) (INB_VLAN_NUM) ;		/* 0 */
	fDrvMod = FALSE ;				/* Just init this. */
	RC = FALSE ;					/* Just init this. */
#ifdef	DEBUG
	buginf ("\r\nBDG_CamInit: Color %d\r\n", Color) ;
#endif


	do {
			/* Get our own MAC address learned at LCP startup. */
		PUNCH_WD ;
		if (LCP_ReadMacAddr (& MyMacU) == NULL)
		{
#ifdef	DEBUG
			buginf ("\r\nBDG_CamInit: LCP_ReadMacAddr failed\r\n") ;
#endif
			break ;
		}


			/* Get TSAR to init its CAM. */
		PUNCH_WD ;
		if (cam_init ())
		{
#ifdef	DEBUG
			buginf ("\r\nBDG_CamInit: cam_init failed\r\n") ;
#endif
			break ;
		}


			/* Setup default TSAR CAM entry for in-band IPC. */
#ifdef	DEBUG
		PUNCH_WD ;
		buginf (
	"\r\nBDG_CamInit: color %d, mac %02x:%02x:%02x:%02x:%02x:%02x\r\n",
			Color, MyMacU.byte[0], MyMacU.byte[1], MyMacU.byte[2],
			MyMacU.byte[3], MyMacU.byte[4], MyMacU.byte[5]) ;
#endif
		PUNCH_WD ;
		if (cam_create_entry ((tpMACADDR) (MyMacU.byte), Color,
						CAME_DESTACP_C) == NULLP)
		{
#ifdef	DEBUG
			buginf ("\r\nBDG_CamInit: cam_create_entry failed\r\n");
#endif
			break ;
		}


			/* Indicate success. */
		RC = TRUE ;

	} while (FALSE) ;


	PUNCH_WD ;
	return (RC) ;			/* Indicate results. */

}	/* BDG_CamInit */

/**/
/* --------------------------------------------------------------------	*/
/*	Sync-up driver startups for ALC.				*/
/*	This code is common to both Boot and Kernel use.  Note that	*/
/*	Boot use implies polling mode, Kernel uses interrupts.		*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DvrInit2 (u_int8 fPUD, u_int8 fKernel)
{
		/* Complete LCP driver init.  BIGA and LCP must be	*/
		/* running before ATM driver will get to HW properly.	*/
		/* Note that this currently drops BIGA from RESET.	*/
		/* Inform the LCP driver of our Kernel/Boot mode.	*/
	PUNCH_WD ;
	if (fPUD)
			/* Prints on dbg jpr. */
		(void) BDG_LcpInit (-1, fPUD, fKernel, FALSE) ;
	else
			/* Force dbg prts off.*/
		(void) BDG_LcpInit (0, fPUD, fKernel, FALSE) ;


		/* Complete ATM driver init. */
	PUNCH_WD ;
	MFP_MaskIntrW   ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE) ;
	MFP_EnableIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE) ;

	PUNCH_WD ;
	LCP_SetPmdType(rd_pmd_type());

	PUNCH_WD ;
	atm_drv_init () ;	/* SAR interrupts masked at MFP on return. */


	PUNCH_WD ;
	return (TRUE) ;		/* Indicate all is well. */

}	/* BDG_DvrInit2 */

/**/
/* --------------------------------------------------------------------	*/
/*	Sync-up driver startups for ALC.				*/
/*	This code is common to both Boot and Kernel use.  Note that	*/
/*	Boot use implies polling mode, Kernel uses interrupts.		*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DvrInit ()
{
	PUNCH_WD ;
	return (BDG_DvrInit2 (FALSE, TRUE)) ;

}	/* BDG_DvrInit */

/**/
/* --------------------------------------------------------------------	*/
/*	Sync-up driver startups for ALC Power-Up Diags.			*/
/*	This code is used in the Boot when starting Power-Up Diags.	*/
/*	Boot use implies device driver polling mode.			*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_PUD_Init ()
{
	PUNCH_WD ;
	return (BDG_DvrInit2 (TRUE, FALSE)) ;

}	/* BDG_PUD_Init */

/**/
/* --------------------------------------------------------------------	*/
/*	Load Tx and Rx SARs with run-time code, ie. for doing NMP	*/
/*	system loopbacks.  SARs are put into RESET state here.		*/
/*	BIGA is not touched here, nor LCP or NTC.			*/
/*	Returns TRUE on success, otherwise FALSE.			*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_SAR_InitRT (register p_void (*pFcn) (void))
{
	u_int8	RC = FALSE ;


        PUNCH_WD ;
	do {
			/* Get BCS Service entry point.  Don't bother to     */
			/* checksum code as it is already part of this Boot. */
		PUNCH_WD ;
		if ((pFcn == (p_void)(BCSSR_FAILURE)) || (pFcn == NULL))
			break ;

                	/* Load SARs with respective run-time code.  */
                	/* This will ensure Tx/Rx SARs are in reset, */
	                /* and disable/clear pending SAR interrupts. */
                PUNCH_WD ;
		if (! BDG_DownldSars (pFcn, BDG_DL_SAR_RT))
			break ;
 
                PUNCH_WD ;
                puts ("Loaded run-time code to TX/RX SARs\n") ;

			/* Complete ATM driver (re-)init. */
		PUNCH_WD ;
		MFP_MaskIntrW   ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE) ;
		MFP_EnableIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE) ;

		PUNCH_WD ;
		atm_drv_init () ;	/* SAR intrs masked at MFP on return. */

                PUNCH_WD ;
		RC = TRUE ;

	} while (FALSE) ;

	return (RC) ;

}	/* BDG_SAR_InitRT */
