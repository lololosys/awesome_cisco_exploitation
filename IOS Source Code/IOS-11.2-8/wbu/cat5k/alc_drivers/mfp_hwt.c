/* $Id: mfp_hwt.c,v 1.1.4.1 1996/05/09 14:55:02 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/mfp_hwt.c,v $
 *-----------------------------------------------------------------
 * mfp_hwt.c -- HW timeout services provided through the MFP.
 *
 *		MFP Timer C is used to provide a guard timer on HW
 *		sense (or driver) conditions.  Drivers can set/clear
 *		this timer, providing a callback to be exercised if
 *		the guard timer expires.
 *
 * March 10, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp_hwt.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:02  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:22  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:29  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:58  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/20  05:04:50  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 * Revision 1.7  1995/06/13  23:34:30  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.6  1995/06/09  06:36:50  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Allow real recovery (reboot ACP/board) if HW (driver) timeout detected
 * (eg. on LCP Tx).
 *
 * Revision 1.5  1995/05/18  23:51:12  rlowe
 * Patch to previous check-in; forgot to include "acp_c_defs.h"
 * for PUNCH_WD.
 *
 * Revision 1.4  1995/05/18  23:47:45  rlowe
 * Revisited actions when driver timeouts detected.  Can reboot board or
 * just loop forever (hang).  Try to put out informative debug prints.
 *
 * Revision 1.3  1995/03/29  07:39:40  rlowe
 * Clean up code to be more robust; fix bug affecting table lookups.
 *
 * Revision 1.2  1995/03/15  10:44:32  rlowe
 * Added support for HW recovery timers, for aiding drivers waiting on HW.
 * Used by LCP driver (and maybe ATM driver).
 *
 * Revision 1.1  1995/03/10  10:31:56  rlowe
 * Initial checkin.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_memblks.h"
#include "lo_error.h"
#include "acp_c_defs.h"
#include "acp_hw_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"



/* --------------------------------------------------------------------	*/
/*	(Local) Data structures to support MFP HW Timeout checking.	*/
/* --------------------------------------------------------------------	*/

typedef struct MFPHWT_Timr
{
	MFPHWT_Cback (pFcn) ;
	u_int32			Arg_1 ;
	volatile u_int32	Count ;
} tMFPHWT_Timr ;

#define	MFPHWT_MAX_USE		(4)
#define	MFPHWT_MAX_TMRVAL	(0xffffffff)

typedef	struct MFPHWT_Glob
{
	u_int16			fPolling ;
	u_int16			TmrMask ;
	volatile u_int8 *	pTmrCtl ;
	volatile u_int8 *	pTmrDat ;
	u_int32			TmrData ;
	u_int32			TimeoutCount ;
	u_int32			MaxTimeVal ;
	u_int32			TimersUsed ;
	tMFPHWT_Timr		TM [MFPHWT_MAX_USE] ;
} tMFPHWT_Glob ;

tMFPHWT_Glob	MWG ;


/* --------------------------------------------------------------------	*/
/*	Default action for driver HW timer timeout -- reboot.		*/
/*	This is provided for clients to call from their callbacks.	*/
/* --------------------------------------------------------------------	*/
void MFP_HwtStdTimeout ()
{
	/* RKL:  do we need to log anything here ? */

#ifdef	DEBUG
	buginf ("\r\nDevice driver time out detected !!\n") ;
#endif

				/* RKL */
				/* What do we do when we're debugging ? */
	A_ResetCPU () ;		/* Reset ACP to its Primary Boot code. */

}	/* MFP_HwtStdTimeout */


/* --------------------------------------------------------------------	*/
/*	Process the various timers.  They countdown, and on zero call	*/
/*	the given HW timeout callback routine.				*/
/*	This routine runs during polling OR at interrupt-level.		*/
/* --------------------------------------------------------------------	*/
void MFP_HwtTicker (register tMFPHWT_Glob * pG, u_int8 fPoll)
{
	s_int32			Loop ;


	if (fPoll)
	{
		MFP_ClrPendIntr (pG->TmrMask, TRUE) ;
	}

	if (! pG->TimersUsed)
	{
		*(pG->pTmrCtl) = ((*(pG->pTmrCtl) & 0x0f) |
				  (MFP_TMR_CTL_C (MFP_TMR_STOPPED))) ;
	}
	else  for (Loop = 0 ; Loop < MFPHWT_MAX_USE ; ++ Loop)
	{
		if (pG->TM [Loop].Count)
		    if (-- (pG->TM [Loop].Count) == 0)
			if (pG->TM [Loop].pFcn != NULL)
			{
			    ++ (pG->TimeoutCount) ;
			    (* pG->TM [Loop].pFcn) (pG->TM [Loop].Arg_1) ;
			}
	}

}	/* MFP_HwtTicker */


/* --------------------------------------------------------------------	*/
/*	C-code ISR for MFP Timer C.					*/
/*	Called by interrupt-level.					*/
/* --------------------------------------------------------------------	*/
void c_timer_C_hdlr ()
{
	MFP_HwtTicker (& MWG, FALSE) ;

}	/* c_timer_C_hdlr */


/* --------------------------------------------------------------------	*/
/*	Check for HW timeouts by polling MFP.				*/
/* --------------------------------------------------------------------	*/
u_int16 MFP_HwtPollChk (u_int16 DevMask, u_int8 fOnce)
{
	register tMFPHWT_Glob *	pG ;
	u_int32			Old_SR ;


	pG = & MWG ;
	DevMask |= pG->TmrMask ;

	Old_SR = spl7 () ;

	DevMask = MFP_PollForIntr (DevMask, FALSE, fOnce, TRUE) ;

		/* Handle any timeouts that may have occurred. */
	if (DevMask & pG->TmrMask)
	{
		MFP_HwtTicker (pG, TRUE) ;
		DevMask &= ~ (pG->TmrMask) ;
	}

	splx (Old_SR) ;

		/* Caller needs to check for any other HW status. */
	return (DevMask) ;

}	/* MFP_HwtPollChk */


/* --------------------------------------------------------------------	*/
/*	Clear a previous timer.						*/
/*	Returns 0 on success, -1 on failure.				*/
/* --------------------------------------------------------------------	*/
s_int32 MFP_HwtClrTimer (MFPHWT_Cback (pHdlr))
{
	register tMFPHWT_Glob *	pG ;
	u_int32			Old_SR ;
	s_int32			Loop ;
	s_int32			RC ;


	pG = & MWG ;
	RC = (-1) ;
	Old_SR = spl7 () ;

	if (pHdlr != NULL)
	    for (Loop = 0 ; Loop < MFPHWT_MAX_USE ; ++ Loop)
		if (pG->TM [Loop].pFcn == pHdlr)
		{
			pG->TM [Loop].pFcn  = NULL ;
			pG->TM [Loop].Count = 0 ;

			if (pG->TimersUsed > 0)
				-- (pG->TimersUsed) ;

			RC = 0 ;
		}

		/* If all timers cleared, de-activate Timer C. */
	if (! pG->TimersUsed)
	{
		*(pG->pTmrCtl) = ((*(pG->pTmrCtl) & 0x0f) |
				  (MFP_TMR_CTL_C (MFP_TMR_STOPPED))) ;
		MFP_ClrPendIntr (pG->TmrMask, TRUE) ;
	}

	splx (Old_SR) ;

	return (RC) ;

}	/* MFP_HwtClrTimer */


/* --------------------------------------------------------------------	*/
/*	Set a timer for checking HW timeouts.				*/
/*	If it is already running, reset it to the new countdown value.	*/
/*	NewCount is in units of 10 msec each.				*/
/*	Returns 0 on success, -1 on failure.				*/
/* --------------------------------------------------------------------	*/
s_int32 MFP_HwtSetTimer (MFPHWT_Cback (pHdlr), u_int32 Arg, u_int32 NewCount)
{
	register tMFPHWT_Glob *	pG ;
	u_int32			Old_SR ;
	s_int32			Loop ;
	s_int32			Indx ;


	pG = & MWG ;
	Indx = (-1) ;
	Old_SR = spl7 () ;

	if (pHdlr != NULL)
	    for (Loop = 0 ; Loop < MFPHWT_MAX_USE ; ++ Loop)
		if (pG->TM [Loop].pFcn == pHdlr)
		{
			Indx = Loop ;		/* Already in table. */
			break ;
		}
		else  if (pG->TM [Loop].pFcn == NULL)
		{
			if (Indx == (-1))
				Indx = Loop ;	/* Free table slot. */
		}

	if (Indx >= 0)
	{
		if (pG->TM [Indx].pFcn == NULL)
		{
			pG->TM [Indx].pFcn  = pHdlr ;
			pG->TM [Indx].Arg_1 = Arg ;
			if (NewCount >= pG->MaxTimeVal)
				NewCount = pG->MaxTimeVal ;
			pG->TM [Indx].Count = NewCount ;

			if ((++ (pG->TimersUsed)) > MFPHWT_MAX_USE)
				pG->TimersUsed = MFPHWT_MAX_USE ;

				/* First timer set must activate Timer C. */
				/* Re-write timer data register to force  */
				/* its internal counter to required value.*/
			if (pG->TimersUsed == 1)
			{
				*(pG->pTmrCtl) = ((*(pG->pTmrCtl) & 0x0f) |
					(MFP_TMR_CTL_C (MFP_TMR_STOPPED))) ;
				MFP_ClrPendIntr (pG->TmrMask, TRUE) ;
				*(pG->pTmrDat) = pG->TmrData ;
				*(pG->pTmrCtl) = ((*(pG->pTmrCtl) & 0x0f) |
					(MFP_TMR_CTL_C (MFP_10MS_TMR_PSCALE))) ;
			}
		}
		else	/* Timer is already running.  Just reset it. */
		{	/* Bump requested period by 1 tick since an  */
			/* interrupt could fire right after here,    */
			/* yielding an actual timeout of 0.          */

			pG->TM [Indx].Arg_1 = Arg ;
			if (NewCount >= pG->MaxTimeVal)
				NewCount = pG->MaxTimeVal - 1 ;
			pG->TM [Indx].Count = NewCount + 1 ;
		}

		Indx = 0 ;
	}

	splx (Old_SR) ;

	return (Indx) ;

}	/* MFP_HwtSetTimer */


/* --------------------------------------------------------------------	*/
/*	Change state between Polling mode and Interrupt-driven mode.	*/
/* --------------------------------------------------------------------	*/
void MFP_HwtChgState (u_int8 fPolling)
{
	u_int32		Old_SR ;


	Old_SR = spl7 () ;

	if (fPolling)
		MFP_MaskIntrW (MFP_HWTOUT_TMR_BITMASK, FALSE) ;
	else
		MFP_UnmaskIntrW (MFP_HWTOUT_TMR_BITMASK, FALSE, FALSE) ;

	MWG.fPolling = fPolling ;

	splx (Old_SR) ;

}	/* MFP_HwtChgState */


/* --------------------------------------------------------------------	*/
/*	Initialize our (internal) data structures to prepare for	*/
/*	handling HW (driver) timeouts.					*/
/* --------------------------------------------------------------------	*/
void MFP_HwtInit (u_int8 fPolling)
{
	register tMFPHWT_Glob *	pG ;
	u_int32			Old_SR ;


	pG = & MWG ;
	Old_SR = spl7 () ;

	(void) A_BlockFill ((u_int8 *) pG, sizeof (*pG), 0, AR_SvcWatchdog) ;

	pG->TmrMask = MFP_HWTOUT_TMR_BITMASK ;
	pG->pTmrCtl = ((volatile u_int8 *) (ACP_HW_MFP_BASE + MFP_RO_TCDCR)) ;
	pG->pTmrDat = ((volatile u_int8 *) (ACP_HW_MFP_BASE + MFP_RO_TCDR )) ;
	pG->TmrData = MFP_10MS_TMR_COUNTER ;
	pG->MaxTimeVal = MFPHWT_MAX_TMRVAL ;

	*(pG->pTmrCtl) = ((*(pG->pTmrCtl) & 0x0f) |
			  (MFP_TMR_CTL_C (MFP_TMR_STOPPED))) ;

	*(pG->pTmrDat) = pG->TmrData ;

	MFP_HwtChgState (fPolling) ;
	MFP_ClrPendIntr (pG->TmrMask, TRUE) ;

	splx (Old_SR) ;

}	/* MFP_HwtInit */
