/* $Id: fbmproc.c,v 3.2.62.2 1996/05/18 08:34:20 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/xdi/fbmproc.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmproc.c,v $
 * Revision 3.2.62.2  1996/05/18  08:34:20  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.2.62.1  1996/03/18  22:51:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:20:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:57:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:09:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmproc.c xm7_v3.1.1:cset.0166:7:7 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Services Process Module

		FBM Queue Processing Module

		File:			xm7_fbmproc.c
		Created:		12/12/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is the processing entry point for FSP. The function
		ProcessFBM() begins the processing of any actions placed
		in the FBM work queue. This function is called from
		the event handling routine or any other interface routine
		that causes FSP to begin executing.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"smtmsg.h"
#include	"fddihdr.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"fbmproto.h"
#include	"multistn.h"


/*
*	Define macro to check frame limit value and stop processing if
*	not already defined.
*/
#ifndef	MFSPFrameLimit
#define	MFSPFrameLimit(count)	0
#endif							/* MFSPFrameLimit */


/*********************************************************************
	Processing Global Variables
*********************************************************************/

/*
*	Flag to prevent re-entrant operation.
*/
static Flag      operating;				/* flag is SET when running */


/*********************************************************************
	Queue Processing Functions
*********************************************************************/

uInt32
#ifdef __STDC__
InitProcessFBM (void)
#else
InitProcessFBM ()
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
ProcessFBM (ProcState * onState)
#else
ProcessFBM (onState)
		ProcState       *onState;
#endif
/*********************************************************************
Function:	Process all actions in the work queue. For each action
			read from the queue, this function calls the appropriate
			processing routine.
Parameters:	onState	= the processor interrupt state value to use
						when interrupts are enabled.
Input:		Reads from queue.
Output:		Removes signals from the queue.
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
*********************************************************************/
{
uInt16           actionType;			/* signal action read from queue */
uInt16           actionLen;				/* signal data length read from queue */
uInt16           actionEntity;			/* signal entity read from queue */
Void_p           actionPtr;				/* signal data ptr read from queue */
uChar            actionEAC;				/* signal EAC bits read from queue */
uInt16           stnID;					/* signal station read from queue */
uInt32           frameCount;			/* frame counter to limit processing */

	/*
	 * If an interrupt occurred during FSP processing and this function is
	 * called again from an interrupt handler, the operating flag must be
	 * checked to insure that FSP is not invoked more than once.
	 */
	if (operating)
		return;

	/* set operating flag. (Note: Interrupts are still off.) */
	operating = SET;

	/* initialize frame counter to limit frame processing */
	frameCount = 0;

	/* loop through all signals in the signal queue */
	while (ReadFBMQueueMulti (&stnID, &actionType, &actionPtr, &actionLen,
			&actionEntity, &actionEAC) != EFBM_Q_MT)
	{

		/* enable interrupts */
		MRestoreInterrupts (onState);

		/* process action as needed */
		switch (actionType)
		{
		case FBM_Q_FRAME:
	             /* Process the item only if its FSP has been completely
		      *	initialied.  Otherwise, ignore the signal, but
		      *	still return the buffer to smtbufQ.
		      */
		     if (FBMDATA (stnID, fbmStationData.FBMInitialized))
			ProcessFBMFrameMulti (stnID, actionPtr, actionLen,
				actionEntity, actionEAC);
			FreeFrameBuffer (actionPtr);
			frameCount++;
			break;

		case FBM_Q_MESSAGE:
	             /* Process the item only if its FSP has been completely
		      *	initialied.  Otherwise, ignore the signal, but
		      *	still return the buffer.
		      */
		     if (FBMDATA (stnID, fbmStationData.FBMInitialized))
			ProcessFBMMessageMulti ((SMTMessage *) actionPtr);
			FreeMessageBuffer ((SMTMessage *) actionPtr);
			break;

		case FBM_Q_TIMER:
			ProcessFBMTimerMulti (stnID);
			break;

		default:
			break;
		}

		/*
		 * Disable interrupts while checking queue to avoid race condition
		 * where a action is put on the queue after ReadFBMQueue() indicates
		 * the queue is empty.
		 */
		MDisableInterrupts (onState);

		/*
		 * Exit FSP if the max number of frames have been processed. (NOTE:
		 * Depending upon the definition of MFSPFrameLimit, this conditional
		 * statement may generate a compiler warning.
		 */
		if (MFSPFrameLimit (frameCount))
			break;
	}

	/* clear operating flag */
	operating = CLEAR;

	return;
}
