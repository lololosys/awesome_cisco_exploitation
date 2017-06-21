/* $Id: fbmq.c,v 3.2.62.1 1996/05/18 08:34:21 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/xdi/fbmq.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmq.c,v $
 * Revision 3.2.62.1  1996/05/18  08:34:21  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.2  1995/11/17  18:10:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmq.c xm7_v3.1.1:cset.0171:7:7 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Services Process Module

		Action Queue Module

		File:			xm7_fbmq.c
		Created:		12/12/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0171		01/12/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module implements a simple FIFO queue to store
		the actions to be processed by FSP. FSP handles three
		actions: frame received, message received, and timer tick.
		When a frame is received by the station, a frame received
		entry (FBM_Q_FRAME) is placed on the queue along with a pointer
		to the frame buffer. Upon receiving a message, a similar action
		takes place for the message action (FBM_Q_MESSAGE) and the
		message buffer. A timer action (FBM_Q_TIMER) does not have any
		data associated with it.

		There are four functions associated with the signal
		queue: StartupFBMQueue(), InitFBMQueue(), PostFBMQueueMulti(),
		ReadFBMQueueMulti() and EmptyFBMQueue().

		The FIFO is implemented here as a circular queue in an array.
		The head is the index to the next entry to be added and the
		tail is the next entry to be read. The empty condition
		is denoted by the head and tail being equal.

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
#include	"fbmproto.h"
#include	"multistn.h"


/*********************************************************************
	Queue Data Structures
*********************************************************************/

struct FBMQueueEntry
{
	uInt16           type;				/* queue entry type */
	Void_p           data;				/* pointer to additional data */
	uInt16           len;				/* len of frame */
	uInt16           entity;			/* MAC entity that rec'd frame */
	uChar            eac;				/* EAC indicators for frame */
	uInt16           stnID;				/* station index for queue entry */
};


/*********************************************************************
	Queue Macros
*********************************************************************/

/*
*	The NextQueueEntry(n) macro returns the next index with a
*	wrap to 0 if the array size will be exceeded.
*/
#define	NextQueueEntry(n)	((n == MAX_FBM_QUEUE_SIZE - 1) ? 0 : n + 1)

/*
*	The NextQueuePtr(n) macro returns the next address with a
*	wrap to the start of the array if the array size will be exceeded.
*/
#define NextQueuePtr(n)	(((n) == (fbmQueue + (MAX_FBM_QUEUE_SIZE - 1)))\
			? fbmQueue : (n) + 1)


/*********************************************************************
	Signal Queue Global Data
*********************************************************************/

static           fbmQHead,				/* head index value */
                 fbmQTail;				/* tail index value */

static struct FBMQueueEntry
                 fbmQueue[MAX_FBM_QUEUE_SIZE];	/* FIFO array */

/*
*       Timer event counter.
*/
Int16            fbmTimerCounter[MAX_STATION_COUNT];
Int16            fbmTimerStation;		/* next station to check */

/*
*       These pointers are used to speed up access to the signal queue.
*       Since the signal queue is used so frequently and requires
*       interrupts to be disable during access, these pointers are used
*       so that the array reference does not need to be recalculated on
*       each entry into the queue routines.
*/
static struct FBMQueueEntry
                *fbmhPtr,				/* head pointer */
                *fbmtPtr;				/* tail pointer */


/*********************************************************************
	Queue Functions
*********************************************************************/

uInt32
#ifdef __STDC__
StartupFBMQueue (void)
#else
StartupFBMQueue ()
#endif
/*********************************************************************
Function:	Initialize the action queue structure for all stations.
Parameters:	None.
Input:		None.
Output:		Set fbmQHead and fbmQTail to 0 and clear queue and
			timer tables.
Return:		0 if successful. Otherwise an error code is returned.
Modification History:
*********************************************************************/
{
ProcState        pState;
Int16            i;

	/* disable interrupts while looking at the pointers */
	MDisableInterrupts (&pState);

	/* clear pointers */
	fbmQHead = fbmQTail = 0;
	fbmhPtr = fbmtPtr = fbmQueue;

	/* clear queue */
	for (i = 0; i < MAX_FBM_QUEUE_SIZE; i++)
	{
		fbmQueue[i].type = 0;
		fbmQueue[i].data = NULL;
	}

	/* clear timer count */
	for (i = 0; i < MAX_STATION_COUNT; i++)
		fbmTimerCounter[i] = 0;

	/* start with first station */
	fbmTimerStation = 0;

	/* enable interrupts */
	MRestoreInterrupts (&pState);

	return (0);
}

uInt32
#ifdef __STDC__
InitFBMQueue (uInt16 stnID)
#else
InitFBMQueue (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize the action queue structure for one station.
Parameters:	stnID	= index of station to initialize.
Input:		None.
Output:		Set fbmQHead and fbmQTail to 0.
Return:		0 if successful. Otherwise an error code is returned.
Modification History:
*********************************************************************/
{
ProcState        pState;
Int16            i;
struct FBMQueueEntry *sqp;

	/* disable interrupts while looking at the pointers */
	MDisableInterrupts (&pState);

	/* remove station's entries from queue */
	for (i = 0, sqp = fbmQueue; i < MAX_FBM_QUEUE_SIZE; i++, sqp++)
	{
		if ((sqp->stnID == stnID) && (sqp->data))
		{
			/* process action as needed */
			switch (sqp->type)
			{
			case FBM_Q_FRAME:
				FreeFrameBuffer (sqp->data);
				break;

			case FBM_Q_MESSAGE:
				FreeMessageBuffer ((SMTMessage *) sqp->data);
				break;
			}

			sqp->type = 0;
			sqp->data = NULL;
		}
	}

	/* clear timer count */
	fbmTimerCounter[stnID] = 0;

	/* enable interrupts */
	MRestoreInterrupts (&pState);

	return (0);
}

uInt32
#ifdef __STDC__
EmptyFBMQueue (void)
#else
EmptyFBMQueue ()
#endif
/*********************************************************************
Function:	Checks if signal queue is empty.
Parameters:	None.
Input:		fbmQHead and fbmQTail.
Output:		None.
Return:		FALSE if queue has items in it. TRUE if the queue is empty.
*********************************************************************/
{
uInt32           result;				/* return value */
ProcState        pState;				/* processor interrupt state */
Int16            i;						/* loop counter */
Int16           *timerPtr;				/* timer element */

	/* disable interrupts while looking at the pointers */
	MDisableInterrupts (&pState);

	/* get queue empty status */
	result = (fbmQHead == fbmQTail);

	/* check timer counter */
	for (i = 0, timerPtr = fbmTimerCounter;
		i < MAX_STATION_COUNT;
		i++, timerPtr++)
	{
		result &= (*timerPtr <= 0);
	}

	/* enable interrupts */
	MRestoreInterrupts (&pState);

	return (result);
}

uInt32
#ifdef __STDC__
ReadFBMQueueMulti (uInt16 * stnID, uInt16 * actionType, Void_p * actionData,
	uInt16 * actionLen, uInt16 * actionEntity, uChar * actionEAC)
#else
ReadFBMQueueMulti (stnID, actionType, actionData, actionLen, actionEntity,
	actionEAC)
		uInt16          *stnID;
		uInt16          *actionType;
		Void_p          *actionData;
		uInt16          *actionLen;
		uInt16          *actionEntity;
		uChar           *actionEAC;
#endif
/*********************************************************************
Function:	Reads next action from the queue.
Parameters:	stnID			= pointer to buffer for station index.
			actionType		= pointer to buffer for action type.
			actionData		= pointer to buffer for action data pointer.
			actionLen		= pointer to buffer for frame length.
			actionEntity	= pointer to buffer for entity ID.
			actionEAC		= pointer to buffer for EAC bits.
Input:		fbmQHead, fbmQTail, and fbmQueue.
Output:		fbmQHead incremented.
			stnID			= contains station index to operate on.
			actionType		= contains type of FBM action.
			actionData		= pointer to a buffer containing the
								action's associated data (i.e.,
								frame buffer for FBM_Q_FRAME and
								message buffer for FBM_Q_MESSAGE).
								Not used for FBM_Q_TIMER.
			actionLen		= length of frame in buffer for FBM_Q_FRAME.
								Not used for other actions.
			actionEntity	= index of entity for FBM_Q_FRAME.
								Not used for other actions.
			actionEAC		= EAC bits received with frame for
								FBM_Q_FRAME. Not used with other actions.
Return:		0			if signal is retured successfully.
			EFBM_Q_MT	if queue is empty.
*********************************************************************/
{
ProcState        pState;				/* processor interrupt state */
uInt32           result;				/* return value */
Int16            stopStation;			/* stn ID where timer loop should
										 * stop */

	/* disable interrupts while reading queue */
	MDisableInterrupts (&pState);
	result = EFBM_Q_MT;

	/*
	 * Check for timer events. This loop does a round-robin check to see if a
	 * station has any timer events to process. This prevents one station
	 * from processing a timer event, then getting another event which it is
	 * processing. Without the round-robin, other stations may get starved
	 * out of processing time. The fbmTimerStation variable holds the ID of
	 * the station to start processing.
	 */
	stopStation = fbmTimerStation;
	do
	{
		/* if this station has a timer event pending */
		if (fbmTimerCounter[fbmTimerStation] > 0)
		{
			/* decrement event counter */
			fbmTimerCounter[fbmTimerStation]--;

			/* set return parameters */
			*actionType = FBM_Q_TIMER;
			*stnID = fbmTimerStation;

			/* mark result code to indicate an event has been found */
			result = 0;
		}

		/* move to the next station ID */
		fbmTimerStation++;
		if (fbmTimerStation >= MAX_STATION_COUNT)
			fbmTimerStation = 0;

		/*
		 * Repeat loop until all stations have been checked or an event has
		 * been found.
		 */
	}
	while ((fbmTimerStation != stopStation) && (result != 0));

	/* if no timer events found */
	if (result != 0)
	{
		/* if queue is not empty, */
		if (fbmQHead != fbmQTail)
		{
			/* set return values */
			*actionType = fbmhPtr->type;
			*stnID = fbmhPtr->stnID;
			switch (fbmhPtr->type)
			{
			case FBM_Q_FRAME:
				*actionData = fbmhPtr->data;
				*actionLen = fbmhPtr->len;
				*actionEntity = fbmhPtr->entity;
				*actionEAC = fbmhPtr->eac;
				break;

			case FBM_Q_MESSAGE:
				*actionData = fbmhPtr->data;
				break;
			}

			/* mark the item removed from the FBMQueue */
			fbmhPtr->type = 0;
			fbmhPtr->data = NULL;

			/* increment head pointer */
			fbmQHead = NextQueueEntry (fbmQHead);
			fbmhPtr = NextQueuePtr (fbmhPtr);

			/* set return value */
			result = 0;
		}
	}

	/* enable interrupts */
	MRestoreInterrupts (&pState);

	return (result);
}

uInt32
#ifdef __STDC__
PostFBMQueueMulti (uInt16 stnID, uInt16 actionType, Void_p actionData,
	uInt16 actionLen, uInt16 actionEntity, uChar actionEAC)
#else
PostFBMQueueMulti (stnID, actionType, actionData, actionLen,
	actionEntity, actionEAC)
		uInt16           stnID;
		uInt16           actionType;
		Void_p           actionData;
		uInt16           actionLen;
		uInt16           actionEntity;
		uChar            actionEAC;
#endif
/*********************************************************************
Function:	Adds an action to the queue.
Parameters:	stnID			= station index to store.
			actionType		= action to store.
			actionData		= pointer to action data.
			actionLen		= length of frame for FBM_Q_FRAME type
			actionEntity	= index of entity for FBM_Q_FRAME type
			actionEAC		= EAC bits for FBM_Q_FRAME
Input:		fbmQHead, fbmQTail, and fbmQueue.
Output:		fbmQueue entry added. fbmQTail incremented.
Return:		0			if successfully added to queue.
			EFBM_Q_FULL	if queue is full.
Note:		In the event that the queue becomes full, the requested
			event is NOT added to the queue. The calling routine is
			responsible for releasing the frame or message buffer
			if needed. A message is sent to the MAP indicating the
			error condition.
*********************************************************************/
{
ProcState        pState;				/* processor intr state */
SMTMessage       smtmsg;				/* error message */
uInt32           result;				/* return value */

	/* disable interrupts while updating queue */
	MDisableInterrupts (&pState);
	result = 0;

	/* if timer event, */
	if (actionType == FBM_Q_TIMER)
	{
		/* add to counter and return */
		fbmTimerCounter[stnID]++;
	}

	else
	{
		/* check for queue full condition */
		if (NextQueueEntry (fbmQTail) == fbmQHead)
		{

			/*
			 * Report error condition to MAP. Do NOT clear the queue.
			 */
			smtmsg.destination = MAP_MSG_ID;
			smtmsg.source = FBM_MSG_ID;
			smtmsg.type = FBM_ERROR_MSG;
			smtmsg.typeInfo = EFBM_Q_FULL;
			smtmsg.localID = 0;
			smtmsg.len1 = 0;
			smtmsg.len2 = 0;
			smtmsg.stnID = stnID;
			SendFBMMessage (&smtmsg);

			result = EFBM_Q_FULL;
		}

		else
		{
			/* add action */
			fbmtPtr->type = actionType;
			fbmtPtr->stnID = stnID;
			switch (actionType)
			{
			case FBM_Q_FRAME:
				fbmtPtr->data = actionData;
				fbmtPtr->len = actionLen;
				fbmtPtr->entity = actionEntity;
				fbmtPtr->eac = actionEAC;
				break;

			case FBM_Q_MESSAGE:
				fbmtPtr->data = actionData;
				break;
			}

			/* increment tail pointer */
			fbmQTail = NextQueueEntry (fbmQTail);
			fbmtPtr = NextQueuePtr (fbmtPtr);
		}
	}

	/* enable interrupts */
	MRestoreInterrupts (&pState);

	return (result);
}


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
PostFBMQueue (uInt16 actionType, Void_p actionData, uInt16 actionLen,
	uInt16 actionEntity, uChar actionEAC)
#else
PostFBMQueue (actionType, actionData, actionLen, actionEntity, actionEAC)
		uInt16           actionType;
		Void_p           actionData;
		uInt16           actionLen;
		uInt16           actionEntity;
		uChar            actionEAC;
#endif
{

	return (PostFBMQueueMulti (FIRST_STATION, actionType, actionData,
			actionLen, actionEntity, actionEAC));
}

uInt32
#ifdef __STDC__
ReadFBMQueue (uInt16 * actionType, Void_p * actionData, uInt16 * actionLen,
	uInt16 * actionEntity, uChar * actionEAC)
#else
ReadFBMQueue (actionType, actionData, actionLen, actionEntity, actionEAC)
		uInt16          *actionType;
		Void_p          *actionData;
		uInt16          *actionLen;
		uInt16          *actionEntity;
		uChar           *actionEAC;
#endif
{
uInt16           dummy;

	return (ReadFBMQueueMulti (&dummy, actionType, actionData, actionLen,
			actionEntity, actionEAC));
}

#endif							/* MULTIPLE_STATION */
