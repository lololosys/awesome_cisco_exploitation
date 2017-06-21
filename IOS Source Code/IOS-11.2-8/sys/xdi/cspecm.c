/* $Id: cspecm.c,v 3.2 1995/11/17 18:08:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspecm.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspecm.c,v $
 * Revision 3.2  1995/11/17  18:08:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspecm.c xm7_v3.1.1:cset.0166:15:15 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		ECM State Machine
		
		File:			xm7_cspecm.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the ECM state machine listed in the
		ANSI X3T9.5 SMT document.
				
		This file consists of two parts. The first part consists of
		a set of functions that perform the various actions associated
		with a transition into a state. The second part implements the
		ECM state machine.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_NO_M_PORT	do not include code for M port CFM.

		If the constant listed above is defined, then the
		associated portion of code is NOT included in the compilation.
		This "reverse option" allows the code to remain backward
		compatible with existing interface files and build procedures.

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
		Global Variables
*********************************************************************/

/*
 * This pointer is set upon entry into SignalECM(). By using the pointer, the
 * CSPDATA() macro does not have to recalculate structure addresses in the
 * multistation array. In single station mode, use of the pointers changes
 * little in code generated.
 */
static struct CSPStationInfo *stationPtr;


/*********************************************************************
		Support Functions
*********************************************************************/

static void
#ifdef __STDC__
CSPGetUpstreamID (uInt16 stnID, uInt16 inIndex, uInt16 inType,
	uInt16 * upIndex, uInt16 * upType)
#else
CSPGetUpstreamID (stnID, inIndex, inType, upIndex, upType)
		uInt16           stnID;
		uInt16           inIndex;
		uInt16           inType;
		uInt16          *upIndex;
		uInt16          *upType;
#endif
/*********************************************************************
Function:	Get the entity that is upstream from the input entity
			along the current token path.
Parameters:	stnID	= index of station to use.
			inIndex	= CSP index of starting entity.
			inType	= RT_port for PORT, or RT_mac for MAC.
			upIndex	= buffer for upstream entity index.
			upType	= buffer for upstream entity type.
Input:		None.
Output:		upIndex	= CSP index of upstream entity.
			upType	= type of upstream entity.
Return:		None.
Modification History:
*********************************************************************/
{
Flag             foundit = FALSE;		/* status of loop */
uInt16           currentIndex;			/* current entity index to inspect */
uInt16           currentType;			/* current entity type to inspect */
uInt16           outPath;				/* output path from current entity */
uInt16           pathIndex;				/* internal index of output path */
CSPPORTInfoType *portPtr;				/* port element pointer */
CSPMACInfoType  *macPtr;				/* MAC element pointer */

	/**
	 * WRF
	 * Initialize some variables to avoid compiler warning.
	 **/
	outPath = 0;
	pathIndex = 0;
	portPtr = NULL;
	macPtr = NULL;

	/* set default return values */
	currentIndex = inIndex;
	currentType = inType;

	/* get next entity's output path (ie, this entity's input) */
	if (inType == RT_mac)
	{
		macPtr = &(CSPDATA (stnID, macData[currentIndex]));
		switch (macPtr->cmState)
		{
		case CM_ISOLATED:
			/* entity is isolated, should not happen  */
			foundit = TRUE;
			break;

		case CM_LOCAL:
			/* set to local path index */
			outPath = macPtr->localPathIndex;
			break;

		case CM_SECONDARY:
			outPath = PI_secondary;
			break;

		case CM_PRIMARY:
			outPath = PI_primary;
			break;
		}
	}
	else
	{
		/* determine which is the input path */
		portPtr = &(CSPDATA (stnID, portData[currentIndex]));
		switch (portPtr->cmState)
		{
		case CM_ISOLATED:
			/* entity is isolated, should not happen  */
			foundit = TRUE;
			break;

		case CM_LOCAL:
			/* set to local path index */
			outPath = portPtr->localPathIndex;
			break;

		case CM_SECONDARY:
			outPath = PI_secondary;
			break;

		case CM_PRIMARY:
			outPath = PI_primary;
			break;

		case CM_CONCAT:
		case CM_THRU:
			if (portPtr->PC_Type == PT_a)
				outPath = PI_secondary;
			else
				outPath = PI_primary;
			break;
		}
	}

	/* locate upstream entity */
	while (!foundit)
	{
		pathIndex = outPath - 1;

		/* move to upstream entity */
		if (currentType == RT_mac)
		{
			currentIndex = macPtr->configList[pathIndex].prevIndex;
			currentType = macPtr->configList[pathIndex].prevType;
		}
		else
		{
			currentIndex = portPtr->configList[pathIndex].prevIndex;
			currentType = portPtr->configList[pathIndex].prevType;
		}

		/* check if this entity is on the path */
		if (currentType == RT_mac)
		{
			macPtr = &(CSPDATA (stnID, macData[currentIndex]));

			/* determine if entity is on path */
			switch (macPtr->cmState)
			{
			case CM_ISOLATED:
				break;

			case CM_LOCAL:
				/* use port's local path */
				foundit = (outPath == macPtr->localPathIndex);
				break;

			case CM_SECONDARY:
				foundit = (outPath == PI_secondary);
				break;

			case CM_PRIMARY:
				foundit = (outPath == PI_primary);
				break;
			}
		}
		else
		{
			portPtr = &(CSPDATA (stnID, portData[currentIndex]));

			/* determine if entity is on path */
			switch (portPtr->cmState)
			{
			case CM_ISOLATED:
				break;

			case CM_LOCAL:
				/* use port's local path */
				foundit = (outPath == portPtr->localPathIndex);
				break;

			case CM_SECONDARY:
				foundit = (outPath == PI_secondary);
				break;

			case CM_PRIMARY:
				foundit = (outPath == PI_primary);
				break;

			case CM_CONCAT:
				if (portPtr->PC_Type == PT_a)
					if (outPath == PI_primary)
						foundit = TRUE;
					else
						/* switch paths */
						outPath = PI_primary;
				else if (outPath == PI_secondary)
					foundit = TRUE;
				else
					/* switch path */
					outPath = PI_secondary;
				break;

			case CM_THRU:
				/*
				 * Thru case should always stop loop unless path is a local
				 * path.
				 */
				foundit = (outPath < PI_local);
				break;
			}
		}
	}

	*upIndex = currentIndex;
	*upType = currentType;

	return;
}

#ifndef	XM_NO_M_PORT
/*
 * The processing of SIG_EC_PCM_RETRY is only needed when M ports are
 * supported by the code.
 */

static void
#ifdef __STDC__
RetryPORTConnection (uInt16 stnID, uInt16 sigEntity)
#else
RetryPORTConnection (stnID, sigEntity)
		uInt16           stnID;
		uInt16           sigEntity;
#endif
/*********************************************************************
Function:	Process PORT management signal to retry a pending connection.
Parameters:	stnID		= index of station to use.
			sigEntity	= port that signalled retry.
Input:		Uses stationData and portData.
Output:		Changes stationData and portData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *portPtr;				/* port element ptr */
Int16            portsAvail;			/* # ports available to connect */
uInt16           portID;				/* loop counter of port to connect */
Int16            portCtr;				/* counter for # ports checked */

	/* get port's structure */
	portPtr = &CSPDATA (stnID, portData[sigEntity]);

	/*
	 * If this port was in the process of connecting, then clear the
	 * connecting flag and decrement the number of ports connecting. Also
	 * clear any insertion timers.
	 */
	if (portPtr->isConnecting)
	{
		--stationPtr->numPortsConnecting;
		portPtr->isConnecting = FALSE;

		/* cancel the graceful/scheduled insertion timeout. */
		SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &portPtr->TGI);
	}

	/* clear the waiting flag */
	portPtr->isWaiting = FALSE;

	/* try to restart as many PCMs as allowed  */
	portsAvail =
		stationPtr->maxMasterCanConnect - stationPtr->numPortsConnecting;
	if (portsAvail > 0)
	{
		/* start at next port */
		portID = sigEntity + 1;

		/* find as many ports to connect as possible */
		for (portCtr = 0; portsAvail && (portCtr < MAX_PORT_COUNT); portCtr++)
		{
			/* wrap at end of array */
			if (portID >= MAX_PORT_COUNT)
				portID = 0;

			portPtr = &CSPDATA (stnID, portData[portID]);

			/* if M port is waiting to connect */
			if (portPtr->hwPresent
				&& (portPtr->PC_Type == PT_m)
				&& portPtr->isWaiting)
			{
				/* signal connection retry */
				portPtr->isWaiting = FALSE;
				SendSignalMulti (stnID, SIG_PC_RETRY, portID, (uInt32) 0);
				--portsAvail;
			}

			portID++;
		}
	}

	return;
}

#endif							/* XM_NO_M_PORT */


/*********************************************************************
		State Transition Functions
*********************************************************************/

static void
#ifdef __STDC__
Out_Actions (uInt16 stnID)
#else
Out_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC0:OUT.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
CSPPATHInfoType *pathPtr;				/* path element */

	/*
	 * If the value of Path_Test is not Passed, and we didn't just come from
	 * a Path_Test, then do a path test.
	 */
	if ((stationPtr->Path_Test != PT_Passed)
		&& (stationPtr->ecState != EC_PATH_TEST))
	{
		/* perform path test */
		if (StationPathTestMulti (stnID))
		{
			/* path test passed */
			stationPtr->Path_Test = PT_Passed;

			/* clear path status */
			for (i = 0, pathPtr = CSPDATA (stnID, pathData);
				i < MAX_PATH_COUNT;
				i++, pathPtr++)
			{
				/*
				 * If trace status is set to something, then clear it and
				 * report it to MSP.
				 */
				if (pathPtr->traceStatus)
				{
					pathPtr->traceStatus = TS_None;
					SendCSPEvent (stnID, fddiPATHTraceStatus, pathPtr->ID);
				}
			}
		}
		else
			/* path test failed */
			stationPtr->Path_Test = PT_Failed;
	}

	/* set current ECM state */
	stationPtr->ecState = EC_OUT;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	return;
}

static void
#ifdef __STDC__
In_Actions (uInt16 stnID)
#else
In_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC1:IN.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
CSPPORTInfoType *portPtr;				/* port element pointer */
CSPMACInfoType  *macPtr;				/* MAC element pointer */

	/* set current ECM state */
	stationPtr->ecState = EC_IN;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/*
	 * XDI has added a flag called RM_Available to the RMT state machine.
	 * This flag ties the operation of RMT into the state of the station and
	 * allows ECM to control the MACs as well as the PORTs. The RM_Available
	 * flag functions like the PC_Start signal. Note that the MACs run before
	 * the PORTs to allow the configuration flags to get set properly.
	 */
	for (i = 0, macPtr = CSPDATA (stnID, macData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		if (macPtr->hwPresent)
		{
			/* enable RMT */
			SendSignalMulti (stnID, SIG_RM_Available, i, (uInt32) SET);

			/* run configuration criteria */
			CFMMACSelectionCriteria (stnID, macPtr);
		}
	}

	/*
	 * CLEAR *:Trace_Prop Note: Since Trace_Prop is used as a signal to ECM
	 * it does not need to be explicitly cleared as described in SMT.
	 */

	/* send *:PC_Start */
	for (i = 0, portPtr = CSPDATA (stnID, portData);
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		if (portPtr->hwPresent)
			SendSignalMulti (stnID, SIG_PC_Start, i, (uInt32) 0);
	}

	return;
}

static void
#ifdef __STDC__
Trace_Actions (uInt16 stnID)
#else
Trace_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC2:TRACE
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	/* set current ECM state */
	stationPtr->ecState = EC_TRACE;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/* reset TEC */
	SetCSPTimerMulti (stnID, stationPtr->Trace_Max, ECM_SIGNALS, 0,
		&stationPtr->TEC);

	return;
}

static void
#ifdef __STDC__
Leave_Actions (uInt16 stnID)
#else
Leave_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC3:LEAVE
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
CSPPORTInfoType *portPtr;				/* port element pointer */
CSPMACInfoType  *macPtr;				/* MAC element pointer */

	/* set current ECM state */
	stationPtr->ecState = EC_LEAVE;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/* send *:PC_Stop */
	for (i = 0, portPtr = CSPDATA (stnID, portData);
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		if (portPtr->hwPresent)
			SendSignalMulti (stnID, SIG_PC_Stop, i, (uInt32) 0);
	}

	/*
	 * XDI has added a flag called RM_Available to the RMT state machine.
	 * This flag ties the operation of RMT into the state of the station and
	 * allows ECM to control the MACs as well as the PORTs. The RM_Available
	 * flag functions like the PC_Start signal.
	 */
	for (i = 0, macPtr = CSPDATA (stnID, macData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		if (macPtr->hwPresent)
		{
			SendSignalMulti (stnID, SIG_RM_Available, i, (uInt32) CLEAR);
		}
	}

	/*
	 * Note: configuration selection criteria is not re-evaluated because the
	 * station is being disconnected and no entities will be in use.
	 */

	/* reset TEC */
	SetCSPTimerMulti (stnID, TD_Min, ECM_SIGNALS, 0, &stationPtr->TEC);

	return;
}

static void
#ifdef __STDC__
Path_Test_Actions (uInt16 stnID)
#else
Path_Test_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC4:PATH_TEST.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
CSPPATHInfoType *pathPtr;				/* path element */

	/* set current ECM state */
	stationPtr->ecState = EC_PATH_TEST;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/* set Path_Test = Testing */
	stationPtr->Path_Test = PT_Testing;

	/* perform path test */
	if (StationPathTestMulti (stnID))
	{
		/* path test passed */
		SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Passed);

		/* clear path status */
		for (i = 0, pathPtr = CSPDATA (stnID, pathData);
			i < MAX_PATH_COUNT;
			i++, pathPtr++)
		{
			/*
			 * If trace status is set to something, then clear it and report
			 * it to MSP.
			 */
			if (pathPtr->traceStatus)
			{
				pathPtr->traceStatus = TS_None;
				SendCSPEvent (stnID, fddiPATHTraceStatus, pathPtr->ID);
			}
		}
	}
	else
		/* path test failed */
		SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Failed);

	return;
}

static void
#ifdef __STDC__
Insert_Actions (uInt16 stnID)
#else
Insert_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC5:INSERT.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	/* set current ECM state */
	stationPtr->ecState = EC_INSERT;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/* SM_PM_BYPASS.request(Insert) */
	BypassRequestMulti (stnID, INSERTED);

	/* reset TEC */
	SetCSPTimerMulti (stnID, IN_Max, ECM_SIGNALS, 0, &stationPtr->TEC);

	return;
}

static void
#ifdef __STDC__
CheckStuckBypass (uInt16 stnID, CSPPORTInfoType * AportPtr,
	CSPPORTInfoType * BportPtr)
#else
CheckStuckBypass (stnID, AportPtr, BportPtr)
		uInt16           stnID;
		CSPPORTInfoType *AportPtr;
		CSPPORTInfoType *BportPtr;
#endif
/*********************************************************************
Function:	Check the incoming line states for a stuck bypass
			condition.
Parameters:	stnID		= index of station to use.
			AportPtr	= A port entry.
			BportPtr	= B port entry.
Input:		Uses stationData and portData.
Output:		Changes stationData and portData.
Return:		No value returned.
Notes:		This routine currently only applies to dual attachment
				nodes.
Modification History:
*********************************************************************/
{
	/*
	 * Before going to IN, check Path_Test. If not Passed, then someone must
	 * have requested a path test. Go to PATH_TEST before going to IN.
	 */
	if (stationPtr->Path_Test != PT_Passed)
		Path_Test_Actions (stnID);

	else
	{
		if (stationPtr->attachCount == 2)
		{
			if ((AportPtr->PC_LS & (QLS | HLS))
				&& (BportPtr->PC_LS & (QLS | HLS)))
			{
				/* EC(61) */
				stationPtr->SB_Flag = CLEAR;
				In_Actions (stnID);
			}
			else if (!stationPtr->SB_Flag
					&& (((AportPtr->PC_LS & (SILS | ILS))
							&& (BportPtr->PC_LS & QLS))
						|| ((AportPtr->PC_LS & QLS)
						&& (BportPtr->PC_LS & (SILS | ILS)))))
			{
				/* EC(66) */
				stationPtr->SB_Flag = SET;
			}
		}

		/*
		 * If the attachCount is not set to 2, then don't check for stuck
		 * bypass, just continue connection process.
		 */
		else
		{
			/* EC(61) */
			stationPtr->SB_Flag = CLEAR;
			In_Actions (stnID);
		}
	}

	return;
}

static void
#ifdef __STDC__
Check_Actions (uInt16 stnID)
#else
Check_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC6:CHECK.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *AportPtr;				/* A port element */
CSPPORTInfoType *BportPtr;				/* B port element */

	/*
	 * Don't report new state on re-entry.
	 */
	if (stationPtr->ecState != EC_CHECK)
	{
		/* set current ECM state */
		stationPtr->ecState = EC_CHECK;
		SendCSPEvent (stnID, fddiSMTECMState, 0);
	}

	/*
	 * Check attach ports for stuck bypass condition. Since PCM is not
	 * running (no PC_Start could have occurred by this point), the current
	 * line state values must be set. The line state is read twice to assure
	 * current information.
	 */
	AportPtr = &(CSPDATA (stnID, portData[stationPtr->A_Index]));
	BportPtr = &(CSPDATA (stnID, portData[stationPtr->B_Index]));

	if (AportPtr->hwPresent)
	{
		AportPtr->PC_LS = ReadLineStateMulti (stnID, AportPtr->ID);
		AportPtr->PC_LS = ReadLineStateMulti (stnID, AportPtr->ID);
	}
	else
		AportPtr->PC_LS = QLS;

	if (BportPtr->hwPresent)
	{
		BportPtr->PC_LS = ReadLineStateMulti (stnID, BportPtr->ID);
		BportPtr->PC_LS = ReadLineStateMulti (stnID, BportPtr->ID);
	}
	else
		BportPtr->PC_LS = QLS;

	CheckStuckBypass (stnID, AportPtr, BportPtr);

	/*
	 * Set TEC timer for 2 second interval if bypass is stuck.
	 */
	if (stationPtr->SB_Flag)
		SetCSPTimerMulti (stnID, (uInt32) 2000000, ECM_SIGNALS, 0,
			&stationPtr->TEC);

	return;
}

static void
#ifdef __STDC__
Deinsert_Actions (uInt16 stnID)
#else
Deinsert_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process entry to EC7:DEINSERT.
Parameters:	stnID	= index of station to use.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{

	/* set current ECM state */
	stationPtr->ecState = EC_DEINSERT;
	SendCSPEvent (stnID, fddiSMTECMState, 0);

	/* SM_PM_BYPASS.request(Insert) */
	BypassRequestMulti (stnID, DEINSERTED);

	/* reset TEC */
	SetCSPTimerMulti (stnID, I_Max, ECM_SIGNALS, 0, &stationPtr->TEC);

	return;
}

static void
#ifdef __STDC__
Prop_Actions (uInt16 stnID, uInt16 entity, uInt16 type)
#else
Prop_Actions (stnID, entity, type)
		uInt16           stnID;
		uInt16           entity;
		uInt16           type;
#endif
/*********************************************************************
Function:	Process actions associated with a Trace_Prop signal.
Parameters:	stnID	= index of station to use.
			entity	= CSP ID of entity issuing Trace_Prop signal.
			type	= RT_port or RT_mac indicating type of entity.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           nextEntity;			/* next trace entity index */
uInt16           nextType;				/* next trace entity type */
uInt16           pathID;				/* path status changed */

	/* if this is a MAC, set TraceInitiated */
	if (type == RT_mac)
	{
		pathID = CSPDATA (stnID, macData[entity].cpIndex);
		CSPDATA (stnID, pathData[pathID].traceStatus) |= TS_traceinitiated;
		SendCSPEvent (stnID, fddiPATHTraceStatus, pathID);
	}

	/* Get entity on input */
	CSPGetUpstreamID (stnID, entity, (uInt16) type, &nextEntity, &nextType);

	/* if entity with output to PORT is a MAC */
	if (nextType == RT_mac)
	{
		/* set TraceTerminated */
		pathID = CSPDATA (stnID, macData[nextEntity].cpIndex);
		CSPDATA (stnID, pathData[pathID].traceStatus) |= TS_traceterminated;
		SendCSPEvent (stnID, fddiPATHTraceStatus, pathID);

		/* set Path_Test = Pending */
		SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Pending);
	}
	else
	{
		/* set TracePropagated */
		pathID = CSPDATA (stnID, portData[nextEntity].cpIndex);
		CSPDATA (stnID, pathData[pathID].traceStatus) |= TS_tracepropagated;
		SendCSPEvent (stnID, fddiPATHTraceStatus, pathID);

		/* propagate trace */
		SendSignalMulti (stnID, SIG_PC_Trace, nextEntity, (uInt32) nextType);
	}

	return;
}

static void
#ifdef __STDC__
Trace_Timeout_Actions (uInt16 stnID)
#else
Trace_Timeout_Actions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Process actions associated with EC(23c).
Parameters:	stnID	= station index to use.
Input:		None.
Output:		Sets path status.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
CSPPATHInfoType *pathPtr;				/* path element */

	/* move to LEAVE state */
	Leave_Actions (stnID);

	/* Set timeout status where needed */
	for (i = 0, pathPtr = CSPDATA (stnID, pathData);
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		if (pathPtr->traceStatus & (TS_traceinitiated | TS_tracepropagated))
		{
			pathPtr->traceStatus |= TS_tracetimeout;
			SendCSPEvent (stnID, fddiPATHTraceStatus, pathPtr->ID);
		}
	}

	return;
}

/*********************************************************************
		ECM State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalECM (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalECM (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Process an ECM signal.
Parameters:	stnID	= index of station to use.
			sigType		= the signal ID.
			sigEntity	= entity (PORT or MAC) to process.
			sigData		= any data associated with the signal.
Input:		Uses stationData.
Output:		Changes stationData.
Return:		No value returned.
Notes:		The value of Path_Test may be set at any number of places
			in ECM or PCM. To facilitate the detection of changes in
			Path_Test outside of the state machine, the signal
			SIG_Path_Test is used. However, the ECM state machine may
			also set Path_Test during transitions. When Path_Test is
			set during a transition, the variable is set immediately
			and no signal is made to ECM.
Modification History:
*********************************************************************/
{
	/* set global structure pointers */
	stationPtr = &CSPDATA (stnID, stationData);

	/*
	 * First process any changes to Path_Test.
	 */
	if (sigType == SIG_Path_Test)
		stationPtr->Path_Test = (uChar) sigData;

	/*
	 * If timer has expired, then verify this timer is the current event
	 * being timed.
	 */
	if (sigType == SIG_EC_Timer)
	{
		if (stationPtr->TEC != sigData)
			return;						/* not current timer */
		else
			stationPtr->TEC = 0;		/* mark timer as expired */
	}

	/*
	 * Select ECM state for processing.
	 */
	switch (stationPtr->ecState)
	{

		/*
		 * EC0:OUT
		 */
	case EC_OUT:
		/*
		 * In this state, only one signal will case processing to occur -
		 * Connect. This will process only when Path_Test is Passed.
		 */
		if ((sigType == SIG_Connect) && (stationPtr->Path_Test == PT_Passed))
		{
			/* check station has a bypass switch */
			if (stationPtr->Bypass)
				/* EC(05) */
				Insert_Actions (stnID);
			else
				/* EC(01) */
				In_Actions (stnID);
		}

		/*
		 * If the value of Path_Test becomes Pending, then do Out_Actions to
		 * force a path test.
		 */
		else if ((sigType == SIG_Path_Test)
			&& (stationPtr->Path_Test == PT_Pending))
		{
			/* EC(00) */
			Out_Actions (stnID);
		}

		break;

		/*
		 * EC1:IN
		 */
	case EC_IN:
		/*
		 * Select action on signal.
		 */
		switch (sigType)
		{
		case SIG_Trace_Prop:
			/* EC(12) */
			Prop_Actions (stnID, sigEntity, (uInt16) sigData);
			Trace_Actions (stnID);
			break;

		case SIG_Disconnect:
			/* EC(13) */
			Leave_Actions (stnID);
			break;

#ifndef	XM_NO_M_PORT
		case SIG_EC_PCM_RETRY:
			/* port management */
			RetryPORTConnection (stnID, sigEntity);
			break;
#endif							/* XM_NO_M_PORT */

			/*
			 * If a path test is requested, then leave and do path test.
			 */
		case SIG_Path_Test:
			/* EC(13b) */
			Leave_Actions (stnID);
			break;
		}
		break;

		/*
		 * EC2:TRACE
		 */
	case EC_TRACE:
		switch (sigType)
		{
		case SIG_Trace_Prop:
			/* EC(22) */
			Prop_Actions (stnID, sigEntity, (uInt16) sigData);
			break;

		case SIG_Disconnect:
			/* EC(23a) */
			stationPtr->Path_Test = PT_Exiting;
			Leave_Actions (stnID);
			break;

		case SIG_Path_Test:
			if (stationPtr->Path_Test == PT_Pending)
				/* EC(23b) */
				Leave_Actions (stnID);
			break;

		case SIG_EC_Timer:
			/* EC(23c) */
			stationPtr->Path_Test = PT_Pending;
			Trace_Timeout_Actions (stnID);
			break;

#ifndef	XM_NO_M_PORT
		case SIG_EC_PCM_RETRY:
			/* port management */
			RetryPORTConnection (stnID, sigEntity);
			break;
#endif							/* XM_NO_M_PORT */
		}
		break;

		/*
		 * EC3:LEAVE
		 */
	case EC_LEAVE:
		switch (sigType)
		{
		case SIG_EC_Timer:
			if (stationPtr->Path_Test == PT_Pending)
				/* EC(34) */
				Path_Test_Actions (stnID);
			else if (stationPtr->Bypass)
				/* EC(37) */
				Deinsert_Actions (stnID);
			else
				/* EC(30) */
				Out_Actions (stnID);
			break;

		case SIG_Connect:
			if (stationPtr->Path_Test == PT_Passed)
				/* EC(31) */
				In_Actions (stnID);
			break;

		case SIG_Disconnect:
			if (stationPtr->Path_Test == PT_Pending)
				/* EC(33) */
				stationPtr->Path_Test = PT_Exiting;
			break;
		}
		break;

		/*
		 * EC4:PATH_TEST
		 */
	case EC_PATH_TEST:
		switch (sigType)
		{
		case SIG_Path_Test:
			if (stationPtr->Path_Test == PT_Failed)
			{
				if (stationPtr->Bypass)
					/* EC(47a) */
					Deinsert_Actions (stnID);
				else
					/* EC(40a) */
					Out_Actions (stnID);
			}
			else if (stationPtr->Path_Test == PT_Passed)
				/* EC(41) */
				In_Actions (stnID);
			break;

		case SIG_Disconnect:
			if (stationPtr->Bypass)
				/* EC(47b) */
				Deinsert_Actions (stnID);
			else
				/* EC(40b) */
				Out_Actions (stnID);
			break;
		}
		break;

		/*
		 * EC5:INSERT
		 */
	case EC_INSERT:
		switch (sigType)
		{
		case SIG_EC_Timer:
			/* EC(56) */
			Check_Actions (stnID);
			break;

		case SIG_Disconnect:
			/* EC(57) */
			Deinsert_Actions (stnID);
			break;
		}
		break;

		/*
		 * EC6:CHECK
		 */
	case EC_CHECK:
		switch (sigType)
		{
			/*
			 * check stuck condition based on timer interval.
			 */
		case SIG_EC_Timer:
			/* EC(66) */
			Check_Actions (stnID);
			break;

		case SIG_Disconnect:
			/* EC(67) */
			Deinsert_Actions (stnID);
			break;
		}
		break;

		/*
		 * EC7:DEINSERT
		 */
	case EC_DEINSERT:
		switch (sigType)
		{
		case SIG_EC_Timer:
			/* EC(70) */
			Out_Actions (stnID);
			break;

		case SIG_Connect:
			if (stationPtr->Path_Test == PT_Passed)
				/* EC(75) */
				Insert_Actions (stnID);
			break;
		}
		break;
	}

	return;
}
