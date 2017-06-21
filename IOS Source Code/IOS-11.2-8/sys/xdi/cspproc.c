/* $Id: cspproc.c,v 3.2 1995/11/17 18:09:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspproc.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspproc.c,v $
 * Revision 3.2  1995/11/17  18:09:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspproc.c xm7_v3.1.1:cset.0166:8:8 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		CSP Signal Processing Module
		
		File:			xm7_cspproc.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is the interrupt entry point. The function
		ProcessCSP() begins the processing of any signals placed
		in the signal queue. This function will be called from
		the interrupt handling routine or any other interface routine
		that causes CSP to begin executing.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"


/*********************************************************************
		Signal Queue Global Variables
*********************************************************************/

/*
 * Make operating flag global to allow for explicit initialization.
 */
static Flag      operating;				/* flag SET when running */


/*********************************************************************
		Signal Queue Processing Function
*********************************************************************/

uInt32
#ifdef __STDC__
InitProcessCSP (void)
#else
InitProcessCSP ()
#endif
/*********************************************************************
Function:	Initialize system for processing CSP events.
Parameters:	None.
Input:		None.
Output:		Sets operating flag to CLEAR.
Return:		0 if successful, otherwise error code.
Modification History:
*********************************************************************/
{
	operating = CLEAR;
	return (0);
}

void
#ifdef __STDC__
ProcessCSP (ProcState * onState)
#else
ProcessCSP (onState)
		ProcState       *onState;
#endif
/*********************************************************************
Function:	Process all signals in the signal queue. For each signal
			read from the signal queue, this function activates the
			appropriate state machine or service routine.
Parameters:	onState	= the processor interrupt state value to use
						when interrupts are enabled.
Input:		Reads from signal queue.
Output:		Empties signal queue.
			onState	= this parameter is updated by the disable/restore
						interrupt macros. If subsequent interrupt level
						changes work correctly, this value should be
						returned unchanged. If this value returns with
						a different interrupt level, then it is an
						indication of a mismatched disable/restore in
						the state machine processing (including
						interface routines).
Return:		None.
Notes:		IMPORTANT! Interrupts must be disabled when this function is
			called. This function will enable interrupts to the level
			specified by onState.

			This routine executes until the signal queue is empty.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID read from queue */
uInt16           sigType;				/* signal type read from queue */
uInt16           sigEntity;				/* signal's entity read from queue */
uInt32           sigData;				/* signal's data read from queue */

	/*
	 * If an interrupt occurred during CSP processing and this function is
	 * called again from the interrupt handler, the operating flag is checked
	 * to insure that CSP is not invoked more than once.
	 */
	if (operating)
		return;

	/*
	 * Set operating flag. (Note: Interrupts are still off.)
	 */
	operating = SET;

	/*
	 * Loop through all signals in the signal queue.
	 */
	while (ReadSignal (&stnID, &sigType, &sigEntity, &sigData) != ECSP_SIG_Q_MT)
	{
		/*
		 * If CSP has not been completely initialized for the station, then
		 * ignore the signal. Continue with loop to empty signal queue.
		 */
		if (!CSPDATA (stnID, stationData.CSPInitialized))
			continue;

		/*
		 * Enable interrupts.
		 */
		MRestoreCSPInterrupts (onState);

		/*
		 * Activate state machine. State machine is specified by high byte.
		 */
		switch (sigType & 0xFF00)
		{
		case ECM_SIGNALS:
			SignalECM (stnID, sigType, sigEntity, sigData);
			break;

		case PCM_SIGNALS:
			SignalPCM (stnID, sigType, sigEntity, sigData);
			break;

		case RMT_SIGNALS:
			SignalRMT (stnID, sigType, sigEntity, sigData);
			break;

		case CFM_SIGNALS:
			SignalCFM (stnID, sigType, sigEntity, sigData);
			break;

		case LEM_SIGNALS:
			SignalLEM (stnID, sigType, sigEntity, sigData);
			break;

			/* XLNT Entry support */
		case LMM_SIGNALS:
			MSIGNAL_LMM (stnID, sigType, sigEntity, sigData);
			break;

		default:
			break;
		}

		/*
		 * Disable interrupts while checking signal queue to avoid race
		 * condition where a signal is put on the queue after ReadSignal()
		 * indicates the queue is empty.
		 */
		MDisableCSPInterrupts (onState);
	}

	/*
	 * Clear operating flag.
	 */
	operating = CLEAR;

	return;
}
