/* $Id: csptimer.c,v 3.2 1995/11/17 18:09:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csptimer.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csptimer.c,v $
 * Revision 3.2  1995/11/17  18:09:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csptimer.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		Timer List Module

		File:			xm7_csptimer.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the timer entry point used for the
		CSP timers. The CSP system requires at least one system
		timer to be available for use. However, CSP must multiplex
		this one timer among the various state machines runnings.
				
		To handle this multiplexing, this module provides an
		interface to a timer list. As intervals need to be timed,
		each state machine requests a timer to be set. This module
		returns an ID value to coordinate the request. When the
		interval expires, the timer module sends a timer signal
		to the state machine along with the ID value. This tells the
		state machine which timer expired.
				
		The timer module consists of four functions: StartupCSPTimer(),
		ServiceCSPTimer(), SetCSPTimerMulti(), and InitCSPTimer().
		The StartupCSPTimer() routine initializes the timer data structures
		for all stations. The InitCSPTimer() routine initializes the
		timer list and system timer for a specific station. The
		SetCSPTimerMulti() routine allows a state machine to add and/or
		remove a timer event. The ServiceCSPTimer() routine is called
		when the system timer expires.
				
		The timer list is implemented as an array. Each element
		in the array consists of the time amount (in microseconds),
		the state machine requesting the timer, and the state machine
		entity (PORT or MAC if needed). A busy flag indicates
		that the entry is in use by a timer event.

		Multistation Notes:
		The one timer list supports all stations. A station ID value
		identifies which station receives the event.

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


/*********************************************************************
		Signal Queue Macros
*********************************************************************/

/*
 * The NextTimerID(n) macro returns the next timer event ID with a wrap to 1
 * if the ID values wrap to negative values.
 */
#define	NextTimerID(n)	(((n + 1) < 1) ? 1 : n + 1)


/*********************************************************************
		Timer List Global Data
*********************************************************************/

uInt16           currentID;				/* current event ID */
uInt16           timerCount;			/* # of active events in list */
uInt16           timerIndex;			/* index of event in system timer */

struct TimerListEntry
                 timerList[MAX_TIMER_LIST_SIZE];	/* FIFO array */


/*********************************************************************
		Timer List Functions
*********************************************************************/

uInt32
#ifdef __STDC__
StartupCSPTimer ()
#else
StartupCSPTimer ()
#endif
/*********************************************************************
Function:	Initialize the timer list structure for all stations.
Parameters:	None.
Input:		None.
Output:		Clears the entire timer list and initializes all
			timer list variables.
Notes:		This routine must be called only once.
Return:		0 if successful. Otherwise an error code is returned.
*********************************************************************/
{
ProcState        pState;
Int16            i;
struct TimerListEntry *tptr;

	/*
	 * Disable interrupts while setting table.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Set initial values.
	 */
	timerCount = 0;
	timerIndex = -1;
	currentID = 0;

	/*
	 * Clear table.
	 */
	for (i = 0, tptr = timerList;
		i < MAX_TIMER_LIST_SIZE;
		i++, tptr++)
	{
		tptr->busy = CLEAR;
	}

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (0);
}

uInt32
#ifdef __STDC__
InitCSPTimer (uInt16 stnID)
#else
InitCSPTimer (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize the timer list structure.
			Calls to this routine remove only those entries associated
			with the specified station ID. If the current timer event
			belongs the station being initialized, then the entry is
			left active but the machine field is cleared to zero to
			prevent signalling of the time out.
Parameters:	stnID.
Input:		None.
Output:		Removes timer entries for the specifed station.
Return:		0 if successful. Otherwise an error code is returned.
*********************************************************************/
{
ProcState        pState;
uInt16           i;
struct TimerListEntry *tptr;

	/*
	 * Disable interrupts while setting table.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Remove events for this station.
	 */
	for (i = 0, tptr = timerList;
		i < MAX_TIMER_LIST_SIZE;
		i++, tptr++)
	{
		if (tptr->busy && (stnID == tptr->stnID))
		{
			/*
			 * If current event, then leave in list.
			 */
			if (i == timerIndex)
			{
				/*
				 * Leave in list but disable signalling.
				 */
				tptr->machine = 0;
			}
			else
			{

				/*
				 * Otherwise remove entry.
				 */
				tptr->busy = CLEAR;
				tptr->stnID = 0;
				--timerCount;			/* reduce event count */
			}
		}
	}

	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return (0);
}

void
#ifdef __STDC__
SetCSPTimerMulti (uInt16 stnID, uInt32 interval, uInt16 stateMachine,
	uInt16 stateEntity, uInt16 * stateID)
#else
SetCSPTimerMulti (stnID, interval, stateMachine, stateEntity, stateID)
		uInt16           stnID;
		uInt32           interval;
		uInt16           stateMachine;
		uInt16           stateEntity;
		uInt16          *stateID;
#endif
/*********************************************************************
Function:	Adds and/or deletes a timer interval event.
Parameters:	stnID			= index of station to use.
			interval		= time interval in MICROSECONDS
								if 0, no event is added.
			stateMachine	= state machine to signal on expiration.
			stateEntity		= state machine entity if required.
			stateID			= pointer to buffer with timer ID.
Input:		stateID buffer contains the ID of a timer event
			to remove from the list. If 0, then no deletion occurs.
			timerCount, timerIndex, and timerList.
Output:		timerCount, timerIndex, and timerList are changed accordingly.
			stateID buffer contains the ID of the added timer event.
			If no event is added, the buffer contains 0.
Return:		No value returned.
Modification History:
*********************************************************************/
{
ProcState        pState;				/* processor state */
uInt32           ticksLeft,				/* time left on system timer */
                 ticksPassed,			/* time passed */
                 minTime;				/* smallest time index value */
uInt16           deleteID;				/* ID to remove from list */
uInt16           i;
Flag             added;					/* SET if event is added */
struct TimerListEntry *tptr;			/* pointer to timer list elements */

	/*
	 * Disable interrupts while updating list.
	 */
	MDisableCSPInterrupts (&pState);

	/*
	 * Stop timer
	 */
	if (timerCount > 0)
	{
		ticksLeft = ReadSystemTimer ();
		SetSystemTimer ((uInt32) 0);
		if (ticksLeft >= timerList[timerIndex].time)
			ticksPassed = 0;
		else
			ticksPassed = timerList[timerIndex].time - ticksLeft;
	}
	else
		ticksPassed = 0;

	/*
	 * Set added flag.
	 */
	added = (interval == 0);			/* if SET, then not adding */

	/*
	 * Set delete ID, and clear returned ID.
	 */
	deleteID = *stateID;
	*stateID = 0;

	/*
	 * If time value is too small for timer, (NOTE: Depending upon the value
	 * of MIN_USECS, this conditional statement may generate a compiler
	 * warning.
	 */
	if (!added && (interval < MIN_USECS))
	{
		/*
		 * Delay.
		 */
		while (interval > 0)
			interval--;

		/*
		 * Post signal directly.
		 */
		currentID = NextTimerID (currentID);
		*stateID = currentID;
		SendSignalMulti (stnID, stateMachine | TIMER_SIGNAL,
			stateEntity, (uInt32) currentID);
		added = SET;
	}
	/*
	 * Loop through list.
	 */
	for (minTime = -1, i = 0, tptr = timerList;
		i < MAX_TIMER_LIST_SIZE;
		i++, tptr++)
	{
		/*
		 * If event found with delete ID, then delete entry.
		 */
		if (deleteID && tptr->busy && (deleteID == tptr->ID))
		{
			/*
			 * If current event and interval is too small, then don't delete
			 * from list. (NOTE: Depending upon the value of MIN_USECS, this
			 * conditional statement may generate a compiler warning.
			 */
			if (i == timerIndex && (tptr->time - ticksPassed) < MIN_USECS)
			{
				/* leave in list */
				tptr->machine = 0;
			}
			/*
			 * Otherwise remove entry.
			 */
			else
			{
				tptr->busy = CLEAR;		/* free entry */
				--timerCount;			/* reduce event count */
			}
			deleteID = 0;				/* clear delete ID */
		}
		/*
		 * if entry is empty and adding, then add here
		 */
		if (!tptr->busy)
		{
			if (!added)
			{
				tptr->stnID = stnID;
				tptr->time = interval;
				tptr->busy = SET;
				tptr->machine = stateMachine;
				tptr->entity = stateEntity;
				currentID = NextTimerID (currentID);
				tptr->ID = currentID;
				*stateID = currentID;
				++timerCount;
				added = SET;
			}
		}
		/*
		 * otherwise reduce time value by amount passed
		 */
		else
			tptr->time -= ticksPassed;

		/*
		 * if entry is smallest interval in list
		 */
		if (tptr->busy && (tptr->time < minTime))
		{
			/* set new current index and minTime */
			timerIndex = i;
			minTime = tptr->time;
		}
	}

	/*
	 * Restart timer if events are being timed
	 */
	if (timerCount > 0)
	{
		/*
		 * Make sure minTime has a value to insure that the system timer is
		 * marked as ON.
		 */
		if (minTime < 1)
			minTime = 1;
		SetSystemTimer (minTime);
	}
	/*
	 * Check for list full condition.
	 */
	if ((interval > 0) && !added)
	{
		/* report error condition */
		LocalTimerListFull (timerList, currentID, timerCount,
			timerIndex);
	}
	/*
	 * Enable interrupts.
	 */
	MRestoreCSPInterrupts (&pState);

	return;
}

void
#ifdef __STDC__
ServiceCSPTimer (void)
#else
ServiceCSPTimer ()
#endif
/*********************************************************************
Function:	Process a system timer interrupt. This routine updates
			the timer list by taking the current timer entry and
			subtracting its time interval from all active entries
			in the list. Any expired entries are posted to the
			signal queue and removed from the timer list. After
			this processing, the new shortest time interval is
			placed into the system timer.
Parameters:	None.
Input:		timerIndex, timerCount, and timerList.
Output:		timerIndex, timerCount, and timerList adjusted accordingly.
Return:		No value returned.
Note:		It is assumed that interrupts are already disabled when
					this function is called.
Modification History:
*********************************************************************/
{
uInt32           ticksPassed,			/* time passed */
                 minTime;				/* smallest time index value */
Int16            i;
struct TimerListEntry *tptr;			/* pointer to timer list elements */

	/*
	 * If no items in the list, then exit
	 */
	if (timerCount <= 0)
		return;

	/*
	 * Get time passed
	 */
	ticksPassed = timerList[timerIndex].time;

	/*
	 * Loop through list.
	 */
	for (minTime = -1, i = 0, tptr = timerList;
		i < MAX_TIMER_LIST_SIZE;
		i++, tptr++)
	{
		if (tptr->busy)
		{
			/*
			 * If time expired, then send a signal
			 */
			if (tptr->time <= ticksPassed)
			{
				/*
				 * Make sure state machine is valid, otherwise this is an
				 * unused, expired entry and should not be posted.
				 */
				if (tptr->machine)
					SendSignalMulti (tptr->stnID, tptr->machine | TIMER_SIGNAL,
						tptr->entity, (uInt32) tptr->ID);
				tptr->busy = CLEAR;
				--timerCount;
			}
			/*
			 * Otherwise check for new minTime
			 */
			else
			{
				/* reduce time value by amount passed */
				tptr->time -= ticksPassed;

				if (tptr->time < minTime)
				{
					/*
					 * set new current index and minTime
					 */
					timerIndex = i;
					minTime = tptr->time;
				}
			}
		}
	}

	/*
	 * Restart timer if events are being timed
	 */
	if (timerCount > 0)
		SetSystemTimer (minTime);

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
SetCSPTimer (uInt32 interval, uInt16 stateMachine,
	uInt16 stateEntity, uInt16 * stateID)
#else
SetCSPTimer (interval, stateMachine, stateEntity, stateID)
		uInt32           interval;
		uInt16           stateMachine;
		uInt16           stateEntity;
		uInt16          *stateID;
#endif
{
	SetCSPTimerMulti (FIRST_STATION, interval, stateMachine,
		stateEntity, stateID);
	return;
}

#endif							/* MULTIPLE_STATION */
