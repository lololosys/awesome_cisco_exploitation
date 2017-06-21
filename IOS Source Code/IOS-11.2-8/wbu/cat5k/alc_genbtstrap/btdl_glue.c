/* $Id: btdl_glue.c,v 1.1.4.1 1996/05/09 14:55:17 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_genbtstrap/btdl_glue.c,v $
 *------------------------------------------------------------------
 * btdl_glue.c -- C code to glue together image downloading and flash
 *		  burning support.
 *
 * May 4, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: btdl_glue.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:17  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:54  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:45  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:13  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.22  1995/10/21  04:28:32  vinod
 * Handle download failure case.
 *
 * Revision 1.21  1995/10/14  03:35:05  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.20  1995/10/13  06:06:49  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.19  1995/09/07  12:08:54  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.18  1995/08/31  12:13:06  rlowe
 * Changes to work with IOS 11.0 and higher.
 * Fix bug where exploder was allowing interrupts after burning flash;
 * but IOS system tick (4ms) interrupt will call flash (monitor) code,
 * thereby accessing invalid code and causing various exceptions.
 *
 * Revision 1.17  1995/08/03  02:04:00  rlowe
 * Add OCS tag/signature string checking, to validate download image
 * as being for ATM line card.  Clean up some buginf/printf messages.
 * Cleanup code that was #if'd out.
 *
 * Revision 1.16  1995/07/27  22:13:57  rlowe
 * Add ALC combined binary image support to BDG (Boot download glue).
 *
 * Revision 1.15  1995/07/26  04:14:50  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.14  1995/07/13  00:06:57  rlowe
 * Add BDG_PUD_Init and other hooks so that Power-Up Diags can be driven
 * from Boot-2, while allowing compatibility with existing Kernel startup
 * code.
 *
 * Revision 1.13  1995/06/29  17:28:34  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.12  1995/06/15  09:28:25  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.11  1995/06/13  23:30:06  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.10  1995/06/13  04:47:11  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.9  1995/06/09  05:48:44  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to ACP.
 * Integrate to latest LCP driver (message model, startup & HW handshakes.
 *
 * Revision 1.8  1995/06/02  01:09:31  rlowe
 * Add support for burning in downloaded code to flash.
 *
 * Revision 1.7  1995/05/26  08:38:08  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.6  1995/05/20  03:07:15  rlowe
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
#include "lo_memblks.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "acp_image_map.h"
#include "bdg_if.h"
#include "mc68ec030_defs.h"
#include "cbi_defs.h"
#include "flash.h"
#include "libcom.h"
#include "acbl.h"


extern int	ipc_boot_dl (void) ;
extern void	ipc_dl_init (unsigned char *) ;


/**/
/* --------------------------------------------------------------------	*/
/*	Sync-up driver startups for ALC.				*/
/*	This code is used for initialization prior to Boot downloading.	*/
/*	Boot use implies polling mode for device drivers.		*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DvrInit ()
{
		/* Complete LCP driver init.  LCP (and SAGE) must be	*/
		/* running before BIGA will get to backplane properly.	*/
		/* Note that this currently drops BIGA from RESET.	*/
		/* Indicate that we are in Boot mode, NOT Kernel mode.	*/
	PUNCH_WD ;
	(void) BDG_LcpInit (-1, FALSE, FALSE, TRUE) ;

		/* We use BIGA directly for Boot downloading. */
	PUNCH_WD ;
	BT_biga_init () ;

	PUNCH_WD ;
	return (TRUE) ;		/* Indicate all is well. */

}	/* BDG_DvrInit */

/**/
/* --------------------------------------------------------------------	*/
/*	Clean up device drivers after completing image download.	*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_DnldDvrCleanUp ()
{
		/* We are done with the actual download, now reset */
		/* BIGA in preparation for normal use.             */
	PUNCH_WD ;
	BDG_ResetFrBufUsr (ACP_WRRG_BIGA_RESET_) ;
	BDG_ClearReset (ACP_WRRG_BIGA_RESET_) ;


	PUNCH_WD ;
	return (TRUE) ;		/* Indicate all is well. */

}	/* BDG_DnldDvrCleanUp */

/**/
/* --------------------------------------------------------------------	*/
/*	Do whatever it takes to get an (in-band) download working.	*/
/*	Assume that LCP/BIGA were previously dropped from reset,	*/
/*	ie. by Boot-1.  Assume that Tx/Rx SARs have been downloaded	*/
/*	and are ready to run.						*/
/*	Run with CPU Data cache disabled since flash contents will	*/
/*	change (erasure, writes) underneath us.				*/
/* --------------------------------------------------------------------	*/
u_int8 BDG_Download (u_int8 fReset, void (*pCallBack)(void))
{
	s_int32	RC = 0 ;		/* Use zero for success. */


		/* Sync up on relevant device drivers. */
	PUNCH_WD ;
	if (! BDG_DvrInit ())
		return (RC) ;		/* 0 */


		/* Try doing a real download.  Watch out for a.out header. */
	PUNCH_WD ;
	ipc_dl_init ((u_int8 *)(ARAM_DOWNLOAD_BUFR)) ;

#ifdef	BDG_FLASH_TEST_PATTERN
	PUNCH_WD ;
	RC = ! BDG_TestImage ((u_int32 *) (ARAM_DOWNLOAD_BUFR),
				BDG_GetDownldSizeX (), 0) ;
#else
	do {
		PUNCH_WD ;
		RC = ipc_boot_dl () ;
	} while (RC > 0) ;	/* Can be < 0, == 0, > 0. */
#endif

#ifdef	DEBUG
	PUNCH_WD ;
	buginf ("\r\nBDG_Download: image download %s\r\n",
		((! RC) ? "succeeded" : "failed")) ;
#endif

		/* Download is done, do any driver/HW cleanup. */
	PUNCH_WD ;
	(void) BDG_DnldDvrCleanUp () ;

	PUNCH_WD ;
	if (RC)
		return (! RC);			/* Return FALSE on failure. */
	else
		return (BDG_SetupExploder (fReset, pCallBack)) ;

}	/* BDG_Download */
