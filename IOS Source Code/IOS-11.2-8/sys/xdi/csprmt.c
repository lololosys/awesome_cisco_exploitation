/* $Id: csprmt.c,v 3.2 1995/11/17 18:09:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csprmt.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csprmt.c,v $
 * Revision 3.2  1995/11/17  18:09:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csprmt.c xm7_v3.1.1:cset.0166:10:10 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		RMT State Machine
		
		File:			xm7_csprmt.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the RMT state machine listed in the
		ANSI X3T9.5 SMT document.
				
		This file consists of two parts. The first part consists of
		a set of functions that perform the various actions associated
		with a transition into a state. The second part implements the
		RMT state machine.

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
		Forward Declarations
*********************************************************************/

#ifdef __STDC__
static void      Ring_Op_Actions (uInt16 stnID, CSPMACInfoType * macPtr);
#else
static void      Ring_Op_Actions ();
#endif


/*********************************************************************
		State Entry Functions
*********************************************************************/

static void
#ifdef __STDC__
Non_Op_Entry (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Non_Op_Entry (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM1:NON_OP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* disable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, DISABLE);

	/* Reset TRM */
	SetCSPTimerMulti (stnID, T_Non_OP, RMT_SIGNALS, macPtr->ID, &macPtr->TRM);

	return;
}

static void
#ifdef __STDC__
Ring_Op_Entry (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Ring_Op_Entry (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process transition in the RM2:RING_OP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	if (macPtr->Rmode)
		/* set Rmode timer */
		SetCSPTimerMulti (stnID,
			(uTime) CSPDATA (stnID, pathData[macPtr->cpIndex].T_Rmode),
			RMT_SIGNALS, macPtr->ID, &macPtr->TRM);

	else
		/* clear TRM */
		SetCSPTimerMulti (stnID, (uTime) 0, RMT_SIGNALS,
			macPtr->ID, &macPtr->TRM);

	return;
}

static void
#ifdef __STDC__
Detect_Entry (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Detect_Entry (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process transitions in the RM3:DETECT state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/*
	 * TRM only needs to be set while Beaconing. Else clear timer.
	 */
	if (macPtr->BN_Flag)
		/* Reset TRM */
		SetCSPTimerMulti (stnID, T_Stuck, RMT_SIGNALS, macPtr->ID,
			&macPtr->TRM);
	else
		/* Clear TRM */
		SetCSPTimerMulti (stnID, (uInt32) 0, RMT_SIGNALS, macPtr->ID,
			&macPtr->TRM);

	/* (re-)enable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, ENABLE);

	return;
}

static void
#ifdef __STDC__
Non_Op_Dup_Entry (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Non_Op_Dup_Entry (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM4:NON_OP_DUP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* Reset TRM */
	if (macPtr->BN_Flag)
		SetCSPTimerMulti (stnID, T_Stuck, RMT_SIGNALS, macPtr->ID,
			&macPtr->TRM);
	else
		SetCSPTimerMulti (stnID, T_Announce, RMT_SIGNALS, macPtr->ID,
			&macPtr->TRM);

	/* (re-)enable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, ENABLE);

	return;
}

/*
 * Ring_Op_Dup_Entry() is the same as Ring_Op_Entry().
 *
 * DIRECTED and TRACE have no transitions back into themselves so no
 * Directed_Entry() or Trace_Entry() is defined.
 */


/*********************************************************************
		State Transition Actions
*********************************************************************/

static void
#ifdef __STDC__
Jam_A_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Jam_A_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process Jam_A actions.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* Disconnect MAC PH_DATA.indication */
	SetMACInputMulti (stnID, macPtr->ID, FALSE);

	/* Prepare beacon frame */
	SetBeaconMulti (stnID, (uInt16) macPtr->ID, (uChar) BCN_CLAIM_FAILED,
		macPtr->SMTAddress, 0, (uChar *) NULL);

	/* Issue MAC Reset */
	MACResetRequestMulti (stnID, macPtr->ID);

	/* BeaconRequestMulti (stnID, macPtr->ID, DA = MLA, INFO = Jam_Info) */
	BeaconRequestMulti (stnID, macPtr->ID);

	/* Wait for T_Jam in special substate */
	SetCSPTimerMulti (stnID, T_Jam, RMT_SIGNALS, macPtr->ID, &macPtr->TRM);
	macPtr->rmState = RM_JAM_A_WAIT_TJAM;

	return;
}

static void
#ifdef __STDC__
Jam_B_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Jam_B_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process Jam_B actions.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* MAC Reset */
	MACResetRequestMulti (stnID, macPtr->ID);

	/* BeaconRequestMulti (stnID, macPtr->ID, DA = MLA, INFO = Jam_Info) */
	SetBeaconMulti (stnID, (uInt16) macPtr->ID, (uChar) BCN_CLAIM_FAILED,
		macPtr->SMTAddress, 0, (uChar *) NULL);
	BeaconRequestMulti (stnID, macPtr->ID);

	/* Wait for T_DBJ in special substate */
	SetCSPTimerMulti (stnID, T_DBJ, RMT_SIGNALS, macPtr->ID, &macPtr->TRM);
	macPtr->rmState = RM_JAM_B_WAIT_TDBJ;

	return;
}

static void
#ifdef __STDC__
Jam_Init_Actions (uInt16 stnID, CSPMACInfoType * macPtr, uChar jamPolicy)
#else
Jam_Init_Actions (stnID, macPtr, jamPolicy)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
		uChar            jamPolicy;
#endif
/*********************************************************************
Function:	Process actions for entering the RM0:ISOLATED state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
			jamPolicy	= policy flag for Jam A or Jam B actions.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/*
	 * Configure MAC to lose the Claim process to other MACs having T_Bid <=
	 * T_Req_Max and nonzero MAC addresses.
	 */
	ConfigureMACMulti (stnID, macPtr->ID, (Flag) ENABLE,
		macPtr->cpIndex, macPtr->TVX, macPtr->T_Max, macPtr->T_Max);

	/* Set JM_Flag */
	macPtr->JM_Flag = SET;

	/* Perform Jam_A_Actions or Jam_B_Actions */
	if (jamPolicy == RMT_DUP_JAM_A)
		Jam_A_Actions (stnID, macPtr);
	else
		Jam_B_Actions (stnID, macPtr);

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
Function:	Process leave actions.
Parameters:	stnID	= station index to use.
Input:		None.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{
extern	void fddi_dup_addr_shutdown (ushort stn_id, ushort mac_id);

	/* disconnect from ring */
	SendSignalMulti (stnID, SIG_Disconnect, 0, (uInt32) 0);

	/**
	 * WRF
	 * Cisco requires interface to shutdown.
	 **/
        /* shutdown station's MAC as well */
        fddi_dup_addr_shutdown (stnID, 0);

	return;
}

static void
#ifdef __STDC__
Change_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Change_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process change actions.
Parameters:	stnID	= station index to use.
				macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           result;				/* result of change attempt */

	/* attempt address change */
	result = ChangeMACAddressMulti (stnID, macPtr->ID, macPtr->SMTAddress);

	/* if successful */
	if (result == 0)
	{
		/* scrub path */
		StartScrubbingMulti (stnID, macPtr->cpIndex);

		/* clear DA_Flag if set */
		if (macPtr->DA_Flag)
		{
			macPtr->DA_Flag = CLEAR;
			SendCSPEvent (stnID, fddiMACDA_Flag, macPtr->ID);
		}
		/* notify MIB of new address */
		SendCSPEvent (stnID, fddiMACSMTAddress, macPtr->ID);
	}
	/* else */
	{
		/* do actions specified */
		if ((result == RMT_DUP_JAM_A) || (result == RMT_DUP_JAM_B))
			Jam_Init_Actions (stnID, macPtr, result);

		else
			Leave_Actions (stnID);
	}

	return;
}

static void
#ifdef __STDC__
Isolated_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Isolated_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM0:ISOLATED state.
Parameters:	stnID	= station index to use.
				macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData and configures MAC.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* Configure MAC for isolation */
	ConfigureMACMulti (stnID, macPtr->ID, (Flag) DISABLE,
		0, macPtr->TVXCap, macPtr->T_MaxCap, macPtr->T_MaxCap);

	macPtr->MAC_Avail = CLEAR;
	SendCSPEvent (stnID, fddiMACMA_UnitdataAvailable, macPtr->ID);

	macPtr->Loop_Avail = CLEAR;

	macPtr->DA_Flag = CLEAR;

	macPtr->SM_MA_Avail = CLEAR;
	SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
		xdiMACSM_MA_AvailFlag, macPtr->ID);

	/* set current RMT state */
	macPtr->rmState = RM_ISOLATED;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* disable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, DISABLE);

	return;
}

static void
#ifdef __STDC__
Insert_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Insert_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process insert actions.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* configure MAC */
	ConfigureMACMulti (stnID, macPtr->ID, (Flag) ENABLE,
		macPtr->cpIndex, macPtr->TVX, macPtr->T_Max, macPtr->T_Req);

	/* MAC Reset */
	MACResetRequestMulti (stnID, macPtr->ID);

	/* start claim process */
	MACClaimRequestMulti (stnID, macPtr->ID);

	return;
}

static void
#ifdef __STDC__
Non_Op_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Non_Op_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM1:NON_OP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* Clear flag */
	macPtr->MAC_Avail = CLEAR;
	SendCSPEvent (stnID, fddiMACMA_UnitdataAvailable, macPtr->ID);
	macPtr->Loop_Avail = CLEAR;
	macPtr->SM_MA_Avail = CLEAR;
	SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
		xdiMACSM_MA_AvailFlag, macPtr->ID);

	/* set current RMT state */
	macPtr->rmState = RM_NON_OP;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* if ring is operational, */
	if (macPtr->ringOP)
		/* then go to RING_OP state */
		Ring_Op_Actions (stnID, macPtr);

	else
		/* else do standard entry actions */
		Non_Op_Entry (stnID, macPtr);

	return;
}

static void
#ifdef __STDC__
Ring_Op_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Ring_Op_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM2:RING_OP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* set availability flags */
	if (macPtr->RM_Loop)
	{
		macPtr->Loop_Avail = SET;
	}
	if (macPtr->RM_Join)
	{
		macPtr->SM_MA_Avail = SET;
		SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
			xdiMACSM_MA_AvailFlag, macPtr->ID);
		macPtr->MAC_Avail = (macPtr->unitdataEnable) ? SET : CLEAR;
		SendCSPEvent (stnID, fddiMACMA_UnitdataAvailable, macPtr->ID);
	}
	/* set current RMT state */
	macPtr->rmState = RM_RING_OP;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* disable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, DISABLE);

	/* check for restricted mode */
	macPtr->Rmode = (ReadMACR_FlagMulti (stnID, macPtr->ID)) ? SET : CLEAR;

	Ring_Op_Entry (stnID, macPtr);

	return;
}

static void
#ifdef __STDC__
Detect_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Detect_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM3:DETECT state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* set current RMT state */
	macPtr->rmState = RM_DETECT;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* if ring is operational */
	if (macPtr->ringOP)
		/* then go to RING_OP state */
		Ring_Op_Actions (stnID, macPtr);

	else
		/* enter DETECT */
		Detect_Entry (stnID, macPtr);

	return;
}

static void
#ifdef __STDC__
New_Dup_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
New_Dup_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process new duplicate actions.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* set DA_Flag if not set */
	if (!macPtr->DA_Flag)
	{
		macPtr->DA_Flag = SET;
		SendCSPEvent (stnID, fddiMACDA_Flag, macPtr->ID);
	}
	macPtr->BN_Flag = CLEAR;
	macPtr->JM_Flag = CLEAR;

	/*
	 * Perform Change_Actions(), Jam_Init_Actions() or Leave_Actions()
	 */
	if (macPtr->rmtDupPolicy == RMT_DUP_CHANGE)
		Change_Actions (stnID, macPtr);
	else if (macPtr->rmtDupPolicy == RMT_DUP_LEAVE)
		Leave_Actions (stnID);
	else
		Jam_Init_Actions (stnID, macPtr, macPtr->rmtDupPolicy);

	return;
}

static void
#ifdef __STDC__
Dup_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Dup_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process duplicate actions.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	if (macPtr->JM_Flag)
	{
		/*
		 * Perform Jam_A_Actions() or Jam_B_Actions()
		 */
		if (macPtr->rmtDupPolicy == RMT_DUP_JAM_A)
			Jam_A_Actions (stnID, macPtr);
		else
			Jam_B_Actions (stnID, macPtr);
	}
	else
	{
		/*
		 * Perform Change_Actions(), Jam_Init_Actions() or Leave_Actions()
		 */
		if (macPtr->rmtDupPolicy == RMT_DUP_CHANGE)
			Change_Actions (stnID, macPtr);
		else if (macPtr->rmtDupPolicy == RMT_DUP_LEAVE)
			Leave_Actions (stnID);
		else
			Jam_Init_Actions (stnID, macPtr, macPtr->rmtDupPolicy);
	}

	return;
}

static void
#ifdef __STDC__
Non_Op_Dup_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Non_Op_Dup_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM4:NON_OP_DUP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* set current RMT state */
	macPtr->rmState = RM_NON_OP_DUP;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* if ring is operational, */
	if (macPtr->ringOP)
		/* then go to RING_OP state */
		Ring_Op_Actions (stnID, macPtr);

	else
		/* else do entry actions */
		Non_Op_Dup_Entry (stnID, macPtr);

	return;
}

static void
#ifdef __STDC__
Ring_Op_Dup_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Ring_Op_Dup_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM5:RING_OP_DUP state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* set current RMT state */
	macPtr->rmState = RM_RING_OP_DUP;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* disable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, DISABLE);

	/* check for restricted mode */
	macPtr->Rmode = (ReadMACR_FlagMulti (stnID, macPtr->ID)) ? SET : CLEAR;

	/* RING_OP and RING_OP_DUP share the same entry routine */
	Ring_Op_Entry (stnID, macPtr);

	return;
}

static void
#ifdef __STDC__
Directed_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Directed_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM6:DIRECTED state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* Reset TRM */
	SetCSPTimerMulti (stnID, T_Direct, RMT_SIGNALS, macPtr->ID,
		&macPtr->TRM);

	/* MAC Reset */
	MACResetRequestMulti (stnID, macPtr->ID);

	/* Beacon Request: DA = multicast, INFO = Directed_Info */
	SetBeaconMulti (stnID, (uInt16) macPtr->ID, (uChar) BCN_DIRECTED,
		SMT_Directed_Beacon_DA, (uInt16) 6, (uChar *) macPtr->upstreamNbr);
	BeaconRequestMulti (stnID, macPtr->ID);

	/* set new RMT state */
	macPtr->rmState = RM_DIRECTED;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* (re-)enable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, ENABLE);

	return;
}

static void
#ifdef __STDC__
Trace_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Trace_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering the RM7:TRACE state.
Parameters:	stnID	= station index to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* Reset TRM */
	SetCSPTimerMulti (stnID, (uTime) 0, RMT_SIGNALS, macPtr->ID,
		&macPtr->TRM);

	/* set current RMT state */
	macPtr->rmState = RM_TRACE;
	SendCSPEvent (stnID, fddiMACRMTState, macPtr->ID);

	/* disable MAC interrupts */
	MACSetInterruptsMulti (stnID, macPtr->ID, DISABLE);

	/* set Trace_Prop */
	SendSignalMulti (stnID, SIG_Trace_Prop, macPtr->ID, (uInt32) RT_mac);

	return;
}

static void
#ifdef __STDC__
Rmode_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
Rmode_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process actions for entering restricted transmissions.
Parameters:	stnID	= index of station to use.
			macPtr	= MAC element to use.
Input:		None.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/* SM_MA_CONTROL.request(Reset) */
	MACResetRequestMulti (stnID, macPtr->ID);

	/* SM_MA_CONTROL.request(Claim) */
	MACClaimRequestMulti (stnID, macPtr->ID);

	return;
}


/*********************************************************************
		RMT State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalRMT (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalRMT (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Process an RMT signal.
Parameters:	stnID		= index of station to use.
			sigType		= the signal ID.
			sigEntity	= entity (MAC) to process.
			sigData		= any data associated with the signal.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
CSPMACInfoType  *macPtr;				/* MAC element to use */

	/*
	 * Set global structure pointers.
	 */
	macPtr = &CSPDATA (stnID, macData[sigEntity]);

	/* If MAC is not present, then ignore the signal. */
	if (!macPtr->hwPresent)
		return;

	/*
	 * First process any changes to RM_Join or RM_Loop. Also RM_Available,
	 * XDI flag to control when RMT runs.
	 */
	if (sigType == SIG_RM_Loop)
		macPtr->RM_Loop = (Flag) sigData;

	else if (sigType == SIG_RM_Join)
		macPtr->RM_Join = (Flag) sigData;

	else if (sigType == SIG_RM_Available)
	{
		/*
		 * Process RM_Available through CFM selection criteria and exit.
		 */
		macPtr->RM_Available = (Flag) sigData;
		CFMMACSelectionCriteria (stnID, macPtr);
		return;
	}

	/*
	 * RM(00) - RM(70): General transitions.
	 */
	if ((macPtr->rmState != RM_ISOLATED)
		&& (!(macPtr->RM_Loop || macPtr->RM_Join)))
	{
		Isolated_Actions (stnID, macPtr);
		return;
	}

	/*
	 * If ring operational changes, set RMT flag.
	 */
	if (sigType == SIG_Ring_OP)
		/* set to logical value presented by sigData */
		macPtr->ringOP = sigData ? SET : CLEAR;

	/*
	 * If timer has expired, then verify this timer is the current event
	 * being timed.
	 */
	else if (sigType == SIG_RM_Timer)
	{

		/*
		 * Determine which timer expired.
		 */
		if (macPtr->rmState == RM_DETECT)
		{
			if (macPtr->TRC == sigData)
			{
				/*
				 * This timer has measured 2*D_Max since claiming.
				 */
				macPtr->claimTimed = SET;
				macPtr->TRC = 0;
				Detect_Entry (stnID, macPtr);
				return;
			}
			else if ((macPtr->TRB == sigData)
				&& (!macPtr->BN_Flag))
			{
				/*
				 * This timer has measured 2*D_Max since beaconing.
				 */
				macPtr->beaconTimed = SET;
				macPtr->TRB = 0;
				Detect_Entry (stnID, macPtr);
				return;
			}
		}
		if (macPtr->TRM != sigData)
			/* not current timer */
			return;

		else
			/* mark timer as expired */
			macPtr->TRM = 0;
	}
	/*
	 * Select RMT state for processing.
	 */
	switch (macPtr->rmState)
	{
		/*
		 * RM0:ISOLATED
		 */
	case RM_ISOLATED:
		switch (sigType)
		{
		case SIG_RM_Loop:
		case SIG_RM_Join:
			/* RM(01) Modified to include XDI flag to control RMT */
			if (macPtr->RM_Loop || macPtr->RM_Join)
			{
				Insert_Actions (stnID, macPtr);
				Non_Op_Actions (stnID, macPtr);
			}
			break;
		}
		break;

		/*
		 * RM1:NON_OP
		 */
	case RM_NON_OP:
		switch (sigType)
		{
		case SIG_Ring_OP:
			if (macPtr->ringOP)
			{
				/* RM(12) */
				Ring_Op_Actions (stnID, macPtr);

				/* XLNT Entry support */
				if (macPtr->ID == CSPDATA (stnID, stationData.giMACID))
					SendSignalMulti (stnID, SIG_Claim_Complete, macPtr->ID,
						(uInt32) 0);
			}
			break;

		case SIG_RM_Timer:
			/* RM(13) */
			macPtr->MAC_Avail = CLEAR;
			SendCSPEvent (stnID, fddiMACMA_UnitdataAvailable, macPtr->ID);
			macPtr->Loop_Avail = CLEAR;
			macPtr->SM_MA_Avail = CLEAR;
			SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
				xdiMACSM_MA_AvailFlag, macPtr->ID);
			macPtr->BN_Flag = CLEAR;
			macPtr->claimTimed = CLEAR;
			macPtr->beaconTimed = CLEAR;
			SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &macPtr->TRC);
			SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &macPtr->TRB);
			Detect_Actions (stnID, macPtr);
			break;
		}
		break;

		/*
		 * RM2:RING_OP
		 */
	case RM_RING_OP:
		switch (sigType)
		{
		case SIG_Ring_OP:
			if (!macPtr->ringOP)
			{
				/* RM(21a) */
				Non_Op_Actions (stnID, macPtr);
			}

			else
			{
				/* XLNT Entry support */
				if (macPtr->ID == CSPDATA (stnID, stationData.giMACID))
					SendSignalMulti (stnID, SIG_Claim_Complete, macPtr->ID,
						(uInt32) 0);
			}

			break;

		case SIG_Dup_Addr:
			if (sigData == DAT_fail)
			{
				/* RM(25) */
				macPtr->MAC_Avail = CLEAR;
				SendCSPEvent (stnID, fddiMACMA_UnitdataAvailable, macPtr->ID);
				macPtr->Loop_Avail = CLEAR;

				/* set DA_Flag if not set */
				if (!macPtr->DA_Flag)
				{
					macPtr->DA_Flag = SET;
					SendCSPEvent (stnID, fddiMACDA_Flag, sigEntity);
				}
				Ring_Op_Dup_Actions (stnID, macPtr);
			}
			break;

		case SIG_MAC_Interrupt:
			if (sigData & MAC_Rmode)
			{
				/* if T_Rmode has a value */
				if (CSPDATA (stnID, pathData[macPtr->cpIndex].T_Rmode))
				{
					/* RM(22a) Reset TRM */
					macPtr->Rmode = SET;
					Ring_Op_Entry (stnID, macPtr);
				}
				else
				{
					/* RM(21b) */
					macPtr->Rmode = CLEAR;
					Rmode_Actions (stnID, macPtr);
					Non_Op_Actions (stnID, macPtr);
				}
			}
			break;

		case SIG_RM_Timer:
			/* Rmode will always be clear after this transition */
			macPtr->Rmode = CLEAR;
			if (ReadMACR_FlagMulti (stnID, sigEntity))
			{
				/* RM(21b) */
				Rmode_Actions (stnID, macPtr);
				Non_Op_Actions (stnID, macPtr);
			}
			break;

		case SIG_RM_MA_Unitdata:
			/* RM(22b) */
			macPtr->unitdataEnable = (Flag) sigData;
			Ring_Op_Actions (stnID, macPtr);
			break;
		}

		break;

		/*
		 * RM3:DETECT
		 */
	case RM_DETECT:
		switch (sigType)
		{

		case SIG_Ring_OP:
			if (macPtr->ringOP)
			{
				/* RM(32) */
				Ring_Op_Actions (stnID, macPtr);

				/* XLNT Entry support */
				if (macPtr->ID == CSPDATA (stnID, stationData.giMACID))
					SendSignalMulti (stnID, SIG_Claim_Complete, macPtr->ID,
						(uInt32) 0);
			}
			break;

		case SIG_MAC_Interrupt:
			if ((sigData & (MAC_My_Beacon | MAC_Other_Beacon))
				&& (macPtr->BN_Flag))
			{
				/*
				 * RM(33a) Leave Beacon state and start TRB to time since
				 * beaconing.
				 */
				macPtr->BN_Flag = CLEAR;
				SetCSPTimerMulti (stnID, Dup_Max, RMT_SIGNALS,
					sigEntity, &macPtr->TRB);
				Detect_Entry (stnID, macPtr);
			}
			else if ((sigData & MAC_TRT_In_T4T5) && (!macPtr->BN_Flag))
			{
				/*
				 * RM(33b) Assume MAC was in Claim, set timer to time since
				 * claiming.
				 */
				SetCSPTimerMulti (stnID, Dup_Max, RMT_SIGNALS,
					sigEntity, &macPtr->TRC);

				/*
				 * Enter the Beacon state.
				 */
				macPtr->BN_Flag = SET;
				macPtr->beaconTimed = CLEAR;
				Detect_Entry (stnID, macPtr);
			}
			else if (
				/* RM(34a) */
					((sigData & MAC_My_Claim) && macPtr->claimTimed)
					||
				/* RM(34b) */
					((sigData & MAC_My_Beacon)
						&& macPtr->beaconTimed)
					||
				/* RM(34c) */
					(sigData & MAC_TBid_TReq_Mismatch)
				)
			{
				New_Dup_Actions (stnID, macPtr);
				Non_Op_Dup_Actions (stnID, macPtr);
			}
			break;

		case SIG_RM_Timer:
			if (macPtr->RM_Join
				&& macPtr->BN_Flag)
			{
				/* RM(36) */
				Directed_Actions (stnID, macPtr);
			}
			/*
			 * NOTE: Special transitions for timing since entering Claim or
			 * 2D_Max since leaving Claim or Beacon are handled at the start
			 * of this routine.
			 */
			break;

		}
		break;

		/*
		 * RM4:NON_OP_DUP
		 */
	case RM_NON_OP_DUP:
		switch (sigType)
		{
		case SIG_RM_DA_Flag:
			/* Set new value of DA_Flag */
			if (macPtr->DA_Flag != (Flag) sigData)
			{
				macPtr->DA_Flag = (Flag) sigData;
				SendCSPEvent (stnID, fddiMACDA_Flag, sigEntity);
			}
			if (!(macPtr->DA_Flag))
				/* RM(41) */
				Non_Op_Actions (stnID, macPtr);
			break;

		case SIG_MAC_Interrupt:

			if ((sigData & (MAC_My_Beacon | MAC_Other_Beacon))
				&& macPtr->BN_Flag)
			{
				/* RM(44a) */
				macPtr->BN_Flag = CLEAR;
				Non_Op_Dup_Entry (stnID, macPtr);
			}
			else if (!macPtr->BN_Flag && (sigData & MAC_TRT_In_T4T5))
			{
				/* RM(44b) */
				macPtr->BN_Flag = SET;
				Non_Op_Dup_Entry (stnID, macPtr);
			}
			break;

		case SIG_Ring_OP:
			if (macPtr->ringOP)
			{
				/* RM(45) */
				macPtr->SM_MA_Avail = SET;
				SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
					xdiMACSM_MA_AvailFlag, macPtr->ID);
				Ring_Op_Dup_Actions (stnID, macPtr);
			}
			break;

		case SIG_RM_Timer:
			if (!macPtr->BN_Flag)
			{
				/* RM(44c) */
				Dup_Actions (stnID, macPtr);
				Non_Op_Dup_Entry (stnID, macPtr);
			}
			else
			{
				if (macPtr->RM_Join)
					/* RM(46) */
					Directed_Actions (stnID, macPtr);
			}
			break;
		}
		break;

		/*
		 * RM5:RING_OP_DUP
		 */
	case RM_RING_OP_DUP:
		switch (sigType)
		{
		case SIG_Dup_Addr:
			if (sigData == DAT_pass)
			{
				/* RM(52) */
				/* set DA_Flag if not set */
				if (macPtr->DA_Flag)
				{
					macPtr->DA_Flag = CLEAR;
					SendCSPEvent (stnID, fddiMACDA_Flag, sigEntity);
				}
				Ring_Op_Actions (stnID, macPtr);

				/*
				 * XLNT Entry support
				 */
				if (macPtr->ID == CSPDATA (stnID, stationData.giMACID))
					SendSignalMulti (stnID, SIG_Claim_Complete, macPtr->ID,
						(uInt32) 0);
			}
			break;

		case SIG_Ring_OP:
			if (!macPtr->ringOP)
			{
				/* RM(54a) */
				macPtr->JM_Flag = CLEAR;
				macPtr->BN_Flag = CLEAR;
				macPtr->SM_MA_Avail = CLEAR;
				SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
					xdiMACSM_MA_AvailFlag, macPtr->ID);
				Non_Op_Dup_Actions (stnID, macPtr);
			}
			break;

		case SIG_MAC_Interrupt:
			if (sigData & MAC_Rmode)
			{
				/* if T_Rmode has a value */
				if (CSPDATA (stnID, pathData[macPtr->cpIndex].T_Rmode))
				{
					/* RM(55) Reset TRM */
					macPtr->Rmode = SET;
					Ring_Op_Entry (stnID, macPtr);
				}
				else
				{
					/* RM(54b) */
					macPtr->Rmode = CLEAR;
					Rmode_Actions (stnID, macPtr);
					Non_Op_Dup_Actions (stnID, macPtr);
				}
			}
			break;

		case SIG_RM_Timer:
			macPtr->Rmode = CLEAR;
			if (ReadMACR_FlagMulti (stnID, sigEntity))
			{
				/* RM(54b) */
				Rmode_Actions (stnID, macPtr);
				Non_Op_Dup_Actions (stnID, macPtr);
			}
			break;
		}

		break;

		/*
		 * RM6:DIRECTED
		 */
	case RM_DIRECTED:
		switch (sigType)
		{
		case SIG_MAC_Interrupt:
			if (sigData & (MAC_My_Beacon | MAC_Other_Beacon))
			{
				if (!macPtr->DA_Flag)
				{
					/* RM(63) Restore normal beacon */
					SetBeaconMulti (stnID, sigEntity, (uChar) BCN_CLAIM_FAILED,
						NULL_ADDRESS, 0, (uChar *) NULL);
					macPtr->BN_Flag = CLEAR;
					Detect_Actions (stnID, macPtr);
				}
				else
				{
					/* RM(64) Restore normal beacon */
					SetBeaconMulti (stnID, sigEntity, (uChar) BCN_CLAIM_FAILED,
						NULL_ADDRESS, 0, (uChar *) NULL);
					macPtr->BN_Flag = CLEAR;
					Non_Op_Dup_Actions (stnID, macPtr);
				}
			}
			break;

		case SIG_RM_Timer:
			if (macPtr->TRM == 0)
			{
				/* RM(67) Restore normal beacon */
				SetBeaconMulti (stnID, sigEntity, (uChar) BCN_CLAIM_FAILED,
					NULL_ADDRESS, 0, (uChar *) NULL);
				Trace_Actions (stnID, macPtr);
			}
			break;
		}
		break;

		/*
		 * RM7:TRACE
		 */
	case RM_TRACE:
		/*
		 * No specific transitions here. Only the general transistions
		 * handled above.
		 */
		break;

		/*
		 * The following states are special substates to handle the interim
		 * wait periods during beacon jamming.
		 */
	case RM_JAM_A_WAIT_TJAM:
		/*
		 * Wait for T_Jam in Jam_A_Actions.
		 */
		if (sigType == SIG_RM_Timer)
		{
			/*
			 * Jam_A_Actions complete.
			 */

			/* Restore MAC input */
			SetMACInputMulti (stnID, sigEntity, TRUE);

			/* Restore normal beacon */
			SetBeaconMulti (stnID, sigEntity, (uChar) BCN_CLAIM_FAILED,
				NULL_ADDRESS, 0, (uChar *) NULL);

			/* Request MAC reset */
			MACResetRequestMulti (stnID, sigEntity);

			/* Return to NON_OP_DUP state */
			Non_Op_Dup_Actions (stnID, macPtr);
		}
		break;

	case RM_JAM_B_WAIT_TDBJ:
		/*
		 * Wait for T_DBJ in Jam_B_Actions.
		 */
		if (sigType == SIG_RM_Timer)
		{
			/* MAC Reset */
			MACResetRequestMulti (stnID, sigEntity);

			/* Beacon Request: DA = MLA, INFO = Jam_Info */
			BeaconRequestMulti (stnID, sigEntity);

			/* Wait for T_Jam in second substate */
			SetCSPTimerMulti (stnID, T_Jam, RMT_SIGNALS, sigEntity,
				&macPtr->TRM);
			macPtr->rmState = RM_JAM_B_WAIT_TJAM;
		}
		break;

	case RM_JAM_B_WAIT_TJAM:
		/*
		 * Wait for T_Jam in Jam_B_Actions.
		 */
		if (sigType == SIG_RM_Timer)
		{
			/* MAC Reset */
			MACResetRequestMulti (stnID, sigEntity);

			/* Restore normal beacon */
			SetBeaconMulti (stnID, sigEntity, (uChar) BCN_CLAIM_FAILED,
				NULL_ADDRESS, 0, (uChar *) NULL);

			/* Return to NON_OP_DUP state */
			Non_Op_Dup_Actions (stnID, macPtr);
		}
		break;

	}

	return;
}
