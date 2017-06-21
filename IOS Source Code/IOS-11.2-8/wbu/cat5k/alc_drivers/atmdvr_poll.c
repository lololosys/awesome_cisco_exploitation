/* $Id: atmdvr_poll.c,v 1.1.4.1 1996/05/09 14:54:44 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/atmdvr_poll.c,v $
 *------------------------------------------------------------------
 * atmdvr__poll.c -- Polling support for the ATM driver.  Allows the ATM
 *		 driver to be used in Boot-level code, where interrupts
 *		 are disabled but device polling is available.
 *
 * April 28, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdvr_poll.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:44  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:19  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:10  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:39  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/13  06:33:14  rlowe
 * Cleaned up some ifdefs.
 *
 * Revision 1.7  1995/06/13  23:34:50  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.6  1995/05/26  10:06:35  rlowe
 * Rename ADVR_RxSarPollDismiss -> ADVR_RxSarPollEvWait.
 *
 * Revision 1.5  1995/05/26  08:42:59  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.4  1995/05/19  00:01:18  sakumar
 * Detect and handle SAR timeouts.
 *
 * Revision 1.3  1995/05/02  05:01:17  rlowe
 * Move some global counter definitions from datatest.c to atmdvr_gdata.c/h.
 * Eliminate some atm driver startup code from atmdvr_poll.c; all startup now
 * in atmdriver.c.
 *
 * Revision 1.2  1995/05/02  01:28:46  rlowe
 * Cleanup files a bit; ensure polling mode api's available in atm driver
 * interface file.
 *
 * Revision 1.1  1995/05/02  01:06:58  rlowe
 * Re-structure code to handling polling (boot case) as well as interrupt
 * mode (kernel case).  Isolate IOS-isms (such as edisms) a bit more.
 * Provide polling mode APIs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif


#include "basic_types.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "atmdvr_gdata.h"
#include "param.h"
#include "sar_if.h"
#include "atmdrv_if.h"

/*
**	PROTOTYPES
*/



/*
**	GLOBALS
*/


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_DismissOnRxSar2
*		ADVR_DismissOnTxSar2
*
*   PARAMETERS:
*           void * pFunc	-- unused, for compatibility with IOS "edisms".
*           tpSARCMD pCmd
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*		Do SAR service requests through polling interfaces to low-
*		level handlers (ie. ISRs).  Repeat until some outstanding
*		SAR transaction is acknowledged to the ACP by an interrupt.
*		MFP interrupt line polling routine punches HW watchdog for us.
*
*		Call appropriate polling dismiss routines contained inside
*		the ATM driver core code.
*
*******************************************************************************
*/

void
ADVR_DismissOnRxSar2 (void * pFunc, tpSARCMD pCmd)
{
	ADVR_RxSarPollEvWait (pFunc, pCmd) ;

}	/* ADVR_DismissOnRxSar2 */

void
ADVR_DismissOnTxSar2 (void * pFunc, tpSARCMD pCmd)
{
	ADVR_TxSarPollEvWait (pFunc, pCmd) ;

}	/* ADVR_DismissOnTxSar2 */


/*
*******************************************************************************
*
*   FUNCTION:
*           atmdrv_BLOCK
*
*   PARAMETERS:
*           tpSARCMD pCmd
*
*   RETURN:
*           TRUE
*			FALSE
*
*   ALGORITHMS
* 			Empty routine for now.  May want to check for driver timeouts.
*
*******************************************************************************
*/

tBOOLEAN
atmdrv_BLOCK (tpSARCMD pCmd)
{
}	/*end atmdrv_BLOCK */
