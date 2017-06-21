/* $Id: csplem.c,v 3.2 1995/11/17 18:09:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csplem.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csplem.c,v $
 * Revision 3.2  1995/11/17  18:09:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csplem.c xm7_v3.1.1:cset.0170:11:11 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		LEM State Machine
		
		File:			xm7_csplem.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0170		01/12/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the LEM state machine as described in
		the XDI SMT documentation.
				
		This file consists of two parts. The first part consists of
		a set of functions that perform the various actions associated
		with a transition into a state. The second part implements the
		LEM state machine.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
#include	"multistn.h"


/*********************************************************************
		LEM Defined Values
*********************************************************************/

#define	LEM_SamplePeriod	4			/* seconds */
#define	LEM_SamplePeriod_USECS	((uTime) 4000000)	/* microseconds */

#define	LEM_MinWindowTime	512			/* seconds */
#define	LEM_MinSlotTime		64			/* seconds */
#define	LEM_MaxSlotTime		2000000		/* seconds */
#define	LEM_MinErrorCount	5


/*********************************************************************
		LEM Global Variables
*********************************************************************/

/*
 * The threshholdTable array determines the number of errors that may occur
 * within a 4 second sample period to achieve a 99% confidence factor that
 * the specifed error rate has been EXCEEDED. These numbers are based on the
 * formula
 *
 * # errors = {[2.33 + SQRT(2.33^2 + 4n)] / 2} ^ 2
 *
 * where n is the minimum number of errors required to exceed the specified
 * error rate (e.g., 50 errors in 4 seconds @ 10E-7 error rate). The
 * confidence factor reduces the algorithms sensitivity to burst error
 * conditions. This formula is from Clay Miller, Boeing.
 *
 * The first four entries are not used because error rates of 10E0 to 10E-3 are
 * not measured.
 */
static MCONST
uInt16
threshholdTable[16] =
{
	0, 0, 0, 0,							/* skip first four error rates 0 - 3 */
		50524,							/* 10E-4 */
		5167,							/* 10E-5 */
		555,							/* 10E-6 */
		69,								/* 10E-7 */
		14,								/* 10E-8 */
		6,								/* 10E-9 */
		6,								/* 10E-10 */
		5,								/* 10E-11 minimum error count of 5 */
		5,								/* 10E-12 */
		5,								/* 10E-13 */
		5,								/* 10E-14 */
		5								/* 10E-15 */
};

/*
 * The logTime array determines the logarithm (base 10) for log (125 *
 * sampleTime). Since 125 is a constant, only sampleTime is used to determine
 * the log and avoid the multiplication. To determine a log, compare
 * sampleTime to each entry (starting at 0) until the table entry exceeds
 * sampleTime. The index of this entry is the log. Note: the first two logs
 * (0 and 1) are not used.
 */
#define	MAX_LOG_TABLE	11
MCONST
static uInt32    logTime[MAX_LOG_TABLE] =
{
	0, 0, 8, 80, 800, 8000,
	80000, 800000, 8000000, 80000000, 800000000
};

/*
 * The logTable array determine straight base 10 logarithms using the same
 * method described above.
 */
MCONST
static uInt32    logTable[MAX_LOG_TABLE] =
{
	10, 100, 1000, 10000, 100000, 1000000, 10000000,
	100000000, 1000000000, -1, -1
};

/*********************************************************************
		Support Functions
*********************************************************************/

static           uInt16
#ifdef __STDC__
log (uInt32 n, MCONST uInt32 table[])
#else
log (n, table)
		uInt32           n;
		MCONST uInt32    table[];
#endif
/*********************************************************************
Function:	Estimate base 10 logarithm of a number.
Parameters:	n		= number to find log of.
			table	= base of log table to use.
Input:		None.
Output:		None.
Return:		Base 10 logarithm of number. This function only estimates
			the log value. The log returned is just the exponent.
			For example: log (5) = 0, log (12) = 1, log (3456) = 3.
Modification History:
*********************************************************************/
{
uInt16           l = 0;

	while ((l < MAX_LOG_TABLE) && (n >= table[l]))
		l++;

	return (l);
}


/*********************************************************************
		State Transition Functions
*********************************************************************/

static void
#ifdef __STDC__
Off_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Off_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process entry to LE0:OFF.
Parameters:	stnID	= index of station to use.
			portPtr	= array element of port.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	/* set current LEM state */
	portPtr->leState = LE_OFF;

	/* disable LEM hardware */
	SetLEMStateMulti (stnID, portPtr->ID, DISABLE);

	/* clear timer */
	SetCSPTimerMulti (stnID, (uInt32) 0, 0, 0, &portPtr->TLE);

	return;
}

static void
#ifdef __STDC__
Start_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Start_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process entry to LE1:MONITOR.
Parameters:	stnID	= index of station to use.
			portPtr	= array element of port.
Input:		Uses portData & threshholdTable array.
Output:		Changes portData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
struct TimeCount *windowPtr;			/* window entry */

	/* clear window */
	for (i = 0, windowPtr = portPtr->lemWindow;
		i < LEM_WindowSize;
		i++, windowPtr++)
	{
		windowPtr->count = 0;
		windowPtr->time = (uInt32) 0;
	}
	portPtr->slotNumber = 0;


	/* set current LEM state */
	portPtr->leState = LE_MONITOR;

	/* set up LEM hardware */
	portPtr->threshhold = threshholdTable[portPtr->lerCutoff];
	SetLEMCounterMulti (stnID, portPtr->ID, portPtr->threshhold);
	SetLEMStateMulti (stnID, portPtr->ID, ENABLE);

	/* start interval timer in usecs */
	SetCSPTimerMulti (stnID, LEM_SamplePeriod_USECS, LEM_SIGNALS,
		portPtr->ID, &portPtr->TLE);

	return;
}

static void
#ifdef __STDC__
Cutoff_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Cutoff_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process threshhold exceeded.
Parameters:	stnID	= index of station to use.
			portPtr	= array element of port.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	/* Set LEM_Fail */
	portPtr->LEM_Fail = SET;

	/* Increment reject count */
	portPtr->LEM_Reject_Ct++;

	/* Send LEM_Stop */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* Send PC_LEM_Fail */
	SendSignalMulti (stnID, SIG_PC_LEM_Fail, portPtr->ID, (uInt32) 0);

	/* Report cutoff event */
	SendCSPEvent (stnID, fddiPORTLem_Reject_Ct, portPtr->ID);

	/* send event if condition not already set */
	if (!portPtr->lerFlag)
	{
		portPtr->lerFlag = SET;
		SendCSPEvent (stnID, fddiPORTLerFlag, portPtr->ID);
	}

	return;
}

static void
#ifdef __STDC__
Monitor_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Monitor_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process sample period expiration in LE1:MONITOR.
Parameters:	stnID	= index of station to use.
			portPtr	= array element for port.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt32           leCount;				/* # errors counted during sample */
uInt32           totalTime;				/* total time in window */
uInt32           totalError;			/* total errors in window */
uInt16           errLog;				/* log of error count */
uInt16           timeLog;				/* log of total time */
uInt16           i;						/* loop counter */
uInt16           oldEst;				/* previous estimate */
struct TimeCount *windowPtr;			/* window element pointers */

	/* get count left till threshhold */
	leCount = ReadLEMCounterMulti (stnID, portPtr->ID);

	/* get # of errs that occurred */
	leCount = portPtr->threshhold - leCount;

	/* restart sample period */
	portPtr->threshhold = threshholdTable[portPtr->lerCutoff];
	SetLEMCounterMulti (stnID, portPtr->ID, portPtr->threshhold);
	SetCSPTimerMulti (stnID, LEM_SamplePeriod_USECS, LEM_SIGNALS,
		portPtr->ID, &portPtr->TLE);

	/* sum current totals in table */
	totalError = 0;
	totalTime = 0;
	for (i = 0, windowPtr = &portPtr->lemWindow[0];
		i < LEM_WindowSize;
		i++, windowPtr++)
	{
		totalError += windowPtr->count;
		totalTime += windowPtr->time;
	}

	/*
	 * Determine accumulation action to take. The algorithm moves to the next
	 * window slot when (no errors in current slot AND the MaxSlotTime is
	 * reached) OR (no errors in current slot AND errors have been counted in
	 * the last sample period AND (the total window time >= MinWindowTime) OR
	 * (current slot time >= MinSlotTime)).
	 */
	/*
	 * 731-007 LJP The update algorithm applies some minor modifications.
	 * First, if MaxSlotTime is met, then move to the next slot regardless of
	 * the error count in the slot (we can't fit any more information in this
	 * slot). The rest remains the same. This will cause the slot to move
	 * immediately when errors are encounted and either of the minimum time
	 * values has been met. The slot will remain the same as long as no errors
	 * are counted or errors are counted on each interval. If errors are
	 * being counted and then they stop, the window will move. This will age
	 * the slot where errors occurred.
	 */
	windowPtr = &portPtr->lemWindow[portPtr->slotNumber];
	if ((windowPtr->time >= LEM_MaxSlotTime)
		|| ((windowPtr->count == 0)
			&& (leCount > 0)
			&& ((totalTime > LEM_MinWindowTime)
				|| (windowPtr->time >= LEM_MinSlotTime))))
	{
		/* move to next slot */
		portPtr->slotNumber++;
		if (portPtr->slotNumber >= LEM_WindowSize)
			portPtr->slotNumber = 0;
		windowPtr = &portPtr->lemWindow[portPtr->slotNumber];

		/* clear slot values */
		windowPtr->time = 0;
		windowPtr->count = 0;
	}
	/* accumulate data into table */
	windowPtr->count += leCount;
	windowPtr->time += LEM_SamplePeriod;
	totalError += leCount;
	totalTime += LEM_SamplePeriod;

	/* calculate estimate */
	oldEst = portPtr->estimate;
	errLog = log ((uInt32) totalError, logTable);
	timeLog = log (totalTime, logTime);
	portPtr->estimate = 6 + timeLog - errLog;

	/*
	 * At this point, a reasonable estimate has been made. However, this may
	 * not suffice. If the number of errors seen so far is not significant
	 * (i.e., totalError < LEM_MinErrorCount) and the estimate is less than
	 * or equal to the alarm rate, then not enough time has passed to collect
	 * enough bits required to reach the alarm rate. In this case, assume
	 * that the error rate is at least one better than the alarm rate.
	 */
	if ((totalError < LEM_MinErrorCount)
		&& (portPtr->estimate <= portPtr->lerAlarm))
	{
		portPtr->estimate = (portPtr->lerAlarm < LER_MAX)
			? portPtr->lerAlarm + 1 : LER_MAX;
	}
	/* report new error count if new errors were counted */
	if (leCount)
	{
		portPtr->lemCount += leCount;	/* total # errors */
		SendCSPEvent (stnID, fddiPORTLem_Ct, portPtr->ID);
	}
	/* report new error rate estimate */
	if (oldEst != portPtr->estimate)
		SendCSPEvent (stnID, fddiPORTLer_Estimate, portPtr->ID);

	/* if cutoff reached */
	if (portPtr->estimate <= portPtr->lerCutoff)
	{
		Cutoff_Actions (stnID, portPtr);
	}
	/* if alarm reached */
	else if (portPtr->estimate <= portPtr->lerAlarm)
	{
		/* send event if condition not already set */
		if (!portPtr->lerFlag)
		{
			portPtr->lerFlag = SET;
			SendCSPEvent (stnID, fddiPORTLerFlag, portPtr->ID);
		}
	}
	/* status is OK */
	else
	{
		/* check if condition needs to be cleared */
		if (portPtr->lerFlag)
		{
			/* clear flag */
			portPtr->lerFlag = CLEAR;
			SendCSPEvent (stnID, fddiPORTLerFlag, portPtr->ID);
		}
	}
	return;
}

/*********************************************************************
		Link Confidence Test Routines
*********************************************************************/

static void
#ifdef __STDC__
Start_LCT_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Start_LCT_Actions (stnID, portPtr)
		CSPPORTInfoType *portPtr;
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to LE2:LCT
Parameters:	stnID	= index of station to use.
			portPtr	= array element of port.
Input:		Uses portData, and stationData.
Output:		Changes portData.
Return:		No value returned.
Notes:		This routine is activated by a direct call to SignalLEM
			from the PCM pseudo-code to keep the PCM timer and LEM
			counter in sync. If PCM had to wait for a signal to reach
			LEM, the time delay may cause errors to be missed or the
			timing of LCT to be incorrect.
Modification History:
*********************************************************************/
{
        /**
        * WRF
        * Changed LEM threshold from 3 to 1 to
        * pass ANTC tests
        **/
        /* enable LEM hardware with a threshhold of 1 */
        SetLEMCounterMulti (stnID, portPtr->ID, 1);
	SetLEMStateMulti (stnID, portPtr->ID, ENABLE);

	/* set up MAC if used */
	if ((portPtr->connectionPolicies & CP_pc_mac_lct)
		&& CSPDATA (stnID, stationData.CF_MAC))
		SetupMACLCTMulti (stnID, portPtr->ID);

	/* set current LEM state */
	portPtr->leState = LE_LCT;

	/* set LCT_Passed flag when starting LCT */
	portPtr->LCT_Passed = SET;

	return;
}


/*********************************************************************
		LEM State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalLEM (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalLEM (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Process an LEM signal.
Parameters:	stnID		= index of station to use.
			sigType		= the signal ID.
			sigEntity	= entity (PHY) to process.
			sigData		= any data associated with the signal.
Input:		Uses lemData.
Output:		Changes lemData.
Return:		No value returned.
Notes:		This state machine is NOT defined by the SMT standard.
			It is used by XDI to simplify SMT implementation.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *portPtr;				/* port element */

	/*
	 * Set port pointer.
	 */
	portPtr = &(CSPDATA (stnID, portData[sigEntity]));

	/*
	 * If timer has expired, then verify this timer is the current event
	 * being timed.
	 */
	if (sigType == SIG_LE_Timer)
	{
		if (portPtr->TLE != sigData)
			return;						/* not current timer */
		else
			portPtr->TLE = 0;			/* mark timer as expired */
	}
	/*
	 * Select LEM state for processing.
	 */
	switch (portPtr->leState)
	{

	case LE_OFF:
		/*
		 * LE0:OFF
		 */
		switch (sigType)
		{
		case SIG_LEM_Start:
			/* LE(01) */
			Start_Actions (stnID, portPtr);
			break;

		case SIG_LCT_Start:
			/* LE(02) */
			Start_LCT_Actions (stnID, portPtr);
			break;
		}
		break;

	case LE_MONITOR:
		/*
		 * LE1:MONITOR
		 */
		switch (sigType)
		{
		case SIG_LEM_Stop:
			/* LE(10) */
			Off_Actions (stnID, portPtr);
			break;

		case SIG_LEM_Threshhold:
			/* LE(11a) */
			/* report new error rate estimate */
			portPtr->lemCount += portPtr->threshhold;
			portPtr->estimate = portPtr->lerCutoff;
			SendCSPEvent (stnID, fddiPORTLem_Ct, portPtr->ID);
			SendCSPEvent (stnID, fddiPORTLer_Estimate, portPtr->ID);
			Cutoff_Actions (stnID, portPtr);
			break;

		case SIG_LE_Timer:
			/* LE(11b) */
			Monitor_Actions (stnID, portPtr);
			break;
		}
		break;

	case LE_LCT:
		/*
		 * LE2:LCT
		 */
		switch (sigType)
		{
		case SIG_LEM_Threshhold:
			/* LE(22) Clear passed condition. */
			portPtr->LCT_Passed = CLEAR;

			/* disable error detection and wait for LCT stop */
			SetLEMStateMulti (stnID, portPtr->ID, DISABLE);

			break;

		case SIG_LCT_Stop:
		case SIG_LEM_Stop:
			/* LE(20a,20b) SIG_LCT_Stop normal LCT termination */
			Off_Actions (stnID, portPtr);

			/* remove MAC if used */
			if ((portPtr->connectionPolicies & CP_pc_mac_lct)
				&& CSPDATA (stnID, stationData.CF_MAC))
				FinishMACLCTMulti (stnID, portPtr->ID);

			/*
			 * Do counters and LCT status only for SIG_LCT_Stop.
			 */
			if (sigType == SIG_LCT_Stop)
			{
				/*
				 * At this point, if LCT has passed, then the count of
				 * consecutive failures can be cleared and the LEM_Fail flag
				 * can be cleared. Otherwise increment count.
				 */
				if (portPtr->LCT_Passed)
				{
					portPtr->PC_LCT_Fail = 0;
					portPtr->LEM_Fail = CLEAR;
				}
				else
				{
					portPtr->PC_LCT_Fail++;

					/* notify MIB of new count value */
					SendCSPEvent (stnID, fddiPORTLCTFail_Ct, portPtr->ID);
				}
			}
			break;
		}
		break;
	}

	return;
}
