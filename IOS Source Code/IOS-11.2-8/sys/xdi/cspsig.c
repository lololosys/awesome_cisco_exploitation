/* $Id: cspsig.c,v 3.2 1995/11/17 18:09:21 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xdi/cspsig.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspsig.c,v $
 * Revision 3.2  1995/11/17  18:09:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspsig.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		Signal Queue Module
		
		File:			xm7_cspsig.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the signal queue used by the CSP
		system. There are four functions associated with the signal
		queue: StartupSignalQueue (initialize queue for all stations),
		SendSignalMulti (add a signal to the queue), ReadSignal
		(read a signal from the queue), InitSignalQueue (initializes
		queue for a particular station), and EmptySignalQueue (checks
		if signal queue is empty).
				
		Throughout CSP, various events occur asynchronously. In order
		to process these events in the order in which they occur, they
		are placed in a FIFO queue. Each entry in the queue contains
		the event type (i.e., the signal), the entity to which this
		signal is being sent (the PORT or MAC index if applicable), and
		a 32-bit value containing data related to the event.
				
		The FIFO is implemented here as a circular queue in an array.
		The head is the index to the next entry to be added and the
		tail is the next entry to be read. The empty condition
		is denoted by the head and tail being equal.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"
#include        "../if/if_fddi.h"

#include	"smterror.h"
#include	"smtmsg.h"
#include	"fddihdr.h"
#include	"csphdr.h"
#include	"cspproto.h"
#include	"multistn.h"


/*********************************************************************
		Signal Queue Macros
*********************************************************************/

/*
 * The NextQueueEntry(n) macro returns the next index with a wrap to 0 if the
 * array size will be exceeded.
 */
#define	NextQueueEntry(n)	(((n) == MAX_SIG_QUEUE_SIZE - 1) ? 0 : (n) + 1)

/*
 * The NextQueuePtr(n) macro returns the next address with a wrap to the
 * start of the array if the array size will be exceeded.
 */
#define	NextQueuePtr(n)	(((n) == (sigQueue + (MAX_SIG_QUEUE_SIZE - 1)))\
			? sigQueue : (n) + 1)


/*********************************************************************
		Signal Queue Global Data
*********************************************************************/

uInt16           sqHead,				/* head index value */
                 sqTail;				/* tail index value */

struct SigQueueEntry
                 sigQueue[MAX_SIG_QUEUE_SIZE];	/* FIFO array */

/*
 * These pointers are used to speed up access to the signal queue. Since the
 * signal queue is used so frequently and requires interrupts to be disable
 * during access, these pointers are used so that the array reference does
 * not need to be recalculated on each entry into the queue routines.
 */
struct SigQueueEntry
                *sqhPtr,				/* head pointer */
                *sqtPtr;				/* tail pointer */


/*********************************************************************
		Signal Queue Functions
*********************************************************************/


uInt32
#ifdef __STDC__
StartupSignalQueue (void)
#else
StartupSignalQueue ()
#endif
/*********************************************************************
Function:	Initialize the signal queue structure.
Parameters:	None.
Input:		None.
Output:		Set sqHead and sqTail to 0; set sqhPtr and sqtPtr to
			the start of the queue.
Return:		0 if successful. Otherwise an error code is returned.
Notes:		This routine is called only once for all stations.
Modification History:
*********************************************************************/
{
ProcState        pState;				/* interrupt state */
Int16            i;						/* loop counter */

	/*
	 * Disable interrupts while looking at the pointers.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Clear pointers.
	 */
	sqHead = sqTail = 0;
	sqhPtr = sqtPtr = sigQueue;

	/*
	 * Clear queue.
	 */
	for (i = 0; i < MAX_SIG_QUEUE_SIZE; i++)
	{
		sigQueue[i].type = 0;
		sigQueue[i].entity = 0;
		sigQueue[i].data = 0;
		sigQueue[i].stnID = 0;
	}

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (0);
}

uInt32
#ifdef __STDC__
InitSignalQueue (uInt16 stnID)
#else
InitSignalQueue (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize the signal queue structure for one station.
Parameters:	stnID	= index of station to initialize.
Input:		None.
Output:		Removes any signals in the queue for the specified station.
Return:		0 if successful. Otherwise an error code is returned.
Modification History:
*********************************************************************/
{
ProcState        pState;				/* interrupt state */
Int16            i;						/* loop counter */
struct SigQueueEntry *sqp;				/* queue pointer */

	/*
	 * Disable interrupts while looking at the pointers.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Clear station's signals from queue.
	 */
	for (i = 0, sqp = sigQueue; i < MAX_SIG_QUEUE_SIZE; i++, sqp++)
	{
		if (sqp->stnID == stnID)
		{
			sqp->type = 0;
			sqp->entity = 0;
			sqp->data = 0;
			sqp->stnID = 0;
		}
	}

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (0);
}

uInt32
#ifdef __STDC__
EmptySignalQueue (void)
#else
EmptySignalQueue ()
#endif
/*********************************************************************
Function:	Checks if signal queue is empty.
Parameters:	None.
Input:		sqHead and sqTail.
Output:		None.
Return:		FALSE if queue has items in it. TRUE if the queue is empty.
*********************************************************************/
{
uInt32           result;				/* return value */
ProcState        pState;				/* interrupt state */

	/*
	 * Disable interrupts while looking at the pointers.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Get queue empty status.
	 */
	result = (sqHead == sqTail);

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (result);
}

uInt32
#ifdef __STDC__
ReadSignal (uInt16 * stnID, uInt16 * sigType, uInt16 * sigEntity,
	uInt32 * sigData)
#else
ReadSignal (stnID, sigType, sigEntity, sigData)
		uInt16          *stnID;
		uInt16          *sigType;
		uInt16          *sigEntity;
		uInt32          *sigData;
#endif
/*********************************************************************
Function:	Reads next signal from the queue.
Parameters:	stnID		= pointer to index of station to use.
			sigType		= pointer to buffer for signal type.
			sigEntity	= pointer to buffer for signal entity index.
			sigData		= pointer to buffer for signal data.
Input:		sqHead, sqTail, sqhPtr, sqtPtr, and sigQueue.
Output:		sigType, sigEntity, and sigData set. sigHead incremented.
Return:		0				if signal is retured successfully.
			ECSP_SIG_Q_MT	if queue is empty.
*********************************************************************/
{
ProcState        pState;				/* interrupt state */
uInt32           result = 0;			/* return value */

	/*
	 * Disable interrupts while reading queue.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Check for something to read.
	 */
	if (sqHead != sqTail)
	{
		/*
		 * Set return values.
		 */
		*stnID = sqhPtr->stnID;
		*sigType = sqhPtr->type;
		*sigEntity = sqhPtr->entity;
		*sigData = sqhPtr->data;

		/*
		 * Increment head pointer.
		 */
		sqHead = NextQueueEntry (sqHead);
		sqhPtr = NextQueuePtr (sqhPtr);
	}
	else
		result = ECSP_SIG_Q_MT;

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (result);
}

void
#ifdef __STDC__
SendSignalMulti (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SendSignalMulti (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Reads next signal from the queue.
Parameters:	stnID		= index of station associated with signal.
			sigType		= signal type to send.
			sigEntity	= signal entity index.
			sigData		= signal data.
Input:		sqHead, sqTail, sqhPtr, sqtPtr and sigQueue.
Output:		siqQueue entry added. sigTail incremented.
Return:		No value returned.
Note:		In the event that the signal queue becomes full, the signal
			will be lost and the connection process should be restarted.
*********************************************************************/
{
ProcState        pState;				/* interrupt state */

    /* JAH - FIXME */

    if (sqtPtr == NULL) return;   /* We haven't been offically
				     initialized yet!  GRRR. */


	/*
	 * Disable interrupts while updating queue.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Check for queue full condition.
	 */
	if (NextQueueEntry (sqTail) == sqHead)
	{
		LocalSignalQueueFull (sigQueue, &sqHead, &sqTail);

		/* if queue was modified, get new pointers */
		sqhPtr = &sigQueue[sqHead];
		sqtPtr = &sigQueue[sqTail];
	}
	/*
	 * Add event.
	 */
	sqtPtr->type = sigType;
	sqtPtr->entity = sigEntity;
	sqtPtr->data = sigData;
	sqtPtr->stnID = stnID;
	sqTail = NextQueueEntry (sqTail);
	sqtPtr = NextQueuePtr (sqtPtr);

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

        /**
        * WRF
        * Record CMT event in Cisco debugging buffer
        */
        switch (sigType)
        {
        /* ECM Signals */
        case SIG_Connect:
        case SIG_Disconnect:
        case SIG_Trace_Prop:
        case SIG_Path_Test:
                fddi_cbug_ecm_signal (stnID, sigType);
                break;
        case SIG_EC_PCM_RETRY:
        case SIG_EC_Timer:
                fddi_cbug_ecm_other (stnID, sigType);
                break;

        /* PCM Signals */
        case SIG_PC_Timer:
                fddi_cbug_pcm_tout (stnID, sigEntity);
                break;
        case SIG_PC_Enable:
        case SIG_PC_Disable:
        case SIG_PC_Start:
        case SIG_PC_Maint:
        case SIG_PC_Trace:
        case SIG_PC_Stop:
                fddi_cbug_pcm_signal (stnID, sigEntity, sigType);
                break;
        case SIG_Noise_Event:
                fddi_cbug_pcm_noise (stnID, sigEntity);
                break;
        case SIG_PC_LEM_Fail:
                fddi_cbug_lem_fail (stnID, sigEntity);
                break;
        case SIG_Line_State:
        case SIG_PC_PDR:
        case SIG_PC_Signal:
        case SIG_PC_Join:
        case SIG_Noise_Timer:
        case SIG_PC_Scrub_Complete:
        case SIG_PC_WC_Flag:
        case SIG_PC_RETRY:
                fddi_cbug_pcm_other (stnID, sigEntity, sigType);
                break;

        /* CFM Signals */
        case SIG_CF_Loop:
        case SIG_CF_Join:
        case SIG_CF_Change:
        case SIG_CF_Timer:
                fddi_cbug_cem_other (stnID, sigEntity, sigType);
                break;

        /* RMT Signals */
        case SIG_RM_Timer:
                fddi_cbug_rmt_tout (stnID, sigEntity);
                break;
        case SIG_MAC_Interrupt:
		fddi_cbug_mac_intr (stnID, sigEntity, sigType);
		break;
        case SIG_RM_Loop:
        case SIG_RM_Join:
        case SIG_Ring_OP:
        case SIG_Dup_Addr:
        case SIG_RM_DA_Flag:
        case SIG_RM_MA_Unitdata:
        case SIG_RM_Available:
		fddi_cbug_rmt_other (stnID, sigEntity, sigType);
                break;
        }

	return;
}


/*
 * If the constant MULTIPLE_STATION is not defined, then the system is being
 * compiled for single station use. In this case, the following routine(s)
 * need to be defined for the older single station calling sequence.
 */
#ifndef MULTIPLE_STATION

void
#ifdef __STDC__
SendSignal (uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SendSignal (sigType, sigEntity, sigData)
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
{

	SendSignalMulti (FIRST_STATION, sigType, sigEntity, sigData);
}

#endif							/* MULTIPLE_STATION */
