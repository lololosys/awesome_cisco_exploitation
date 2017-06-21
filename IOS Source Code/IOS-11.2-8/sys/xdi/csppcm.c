/* $Id: csppcm.c,v 3.2 1995/11/17 18:09:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csppcm.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csppcm.c,v $
 * Revision 3.2  1995/11/17  18:09:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csppcm.c xm7_v3.1.1:cset.0166:13:13 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module

		PCM State Machine

		File:			xm7_csppcm.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module implements the PCM state machine listed in the
		ANSI X3T9.5 SMT document.

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
#include        "../if/if_fddi.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"


/*********************************************************************
		Global Variables
*********************************************************************/


/*********************************************************************
		PCM State Machine Port Management Functions

		The clean-up routine only applies to M ports. If XM_NO_M_PORT
		is not defined, then this routine included and a macro is
		defined to call the clean-up for M ports only. Otherwise, just
		the macro is defined with no content.
*********************************************************************/

#ifndef	XM_NO_M_PORT

static void
#ifdef __STDC__
CleanupPortMgmt (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
CleanupPortMgmt (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Reset port management state for the port specified. First
			reset any graceful/scheduled insertion processing. Then, if
			this port was in the process of connecting, signal ECM to
			retry any waiting ports. ECM is responsible for clearing
			the isWaiting and isConnecting flags.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Signals ECM to retry other ports.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* if port is connecting */
	if (portPtr->isConnecting)
	{
		/* if performing graceful insertion */
		if (CSPDATA (stnID, stationData.gracefulMode) == GIM_gracefulinsertion)
			/* cancel graceful insertion */
			MCANCEL_GRACE_INS (stnID, portPtr);

		/* if performing scheduled insertion */
		else if (CSPDATA (stnID, stationData.gracefulMode)
			== GIM_scheduledinsertion)
			/* cancel scheduled insertion */
			MCANCEL_SCHED_INS (stnID, portPtr);

		/* retry connection */
		SendSignalMulti (stnID, SIG_EC_PCM_RETRY, portPtr->ID, (uInt32) 0);
	}

	return;
}

/* Macro calls clean-up only for M ports */
#define	MCleanupPortMgmt(stnID, portPtr)\
	if (portPtr->PC_Type == PT_m)\
		CleanupPortMgmt (stnID, portPtr)

#else							/* XM_NO_M_PORT */

#define	MCleanupPortMgmt(stnID, portPtr)

#endif							/* XM_NO_M_PORT */


/*********************************************************************
		PCM State Machine State Entry Functions
*********************************************************************/

static void
#ifdef __STDC__
Off_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Off_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into OFF.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* reset TPC */
	SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Break_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Break_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into BREAK.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval = 0;				/* timeout value */

	/* determine timer value based on flag settings */
	if (!portPtr->LS_Flag)
		/*
		 * First time through so wait for TB_Min.
		 */
		tval = TB_Min;

	else if (!portPtr->BS_Flag)
		/*
		 * Stuck in break not set yet.
		 */
		tval = TB_Max;

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Connect_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Connect_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into CONNECT.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/*
	 * Set TPC
	 */
	if (portPtr->LS_Flag)
		SetCSPTimerMulti (stnID, C_Min, PCM_SIGNALS, portPtr->ID,
			&portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Next_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Next_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into NEXT.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval = T_Out;			/* timeout value */

	/*
	 * Set TPC
	 */
	if (portPtr->LS_Flag)
	{
		if (!portPtr->RC_Flag)
			tval = TL_Min;

		else if (portPtr->TD_Flag && !portPtr->TC_Flag)
		{
			if (portPtr->n == 7)
				tval = portPtr->LC_Test;

			else
				tval = portPtr->MACLoop_Time;
		}
	}

#ifndef	XM_NO_M_PORT

	/*
	 * Because an extended LCT takes such a long period of time, PCM allows
	 * other ports to try to connect while this port is doing its LCT.
	 */
	if (portPtr->isConnecting && (portPtr->n == 7) && (tval == LC_Extended))
	{
		/*
		 * Doing extended LCT, tell ECM to clear port management flags and
		 * start any waiting ports.
		 */
		SendSignalMulti (stnID, SIG_EC_PCM_RETRY, portPtr->ID, (uInt32) 0);
	}

#endif							/* XM_NO_M_PORT */

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Signal_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Signal_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into SIGNAL.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval;					/* timeout value */

	/*
	 * Set TPC
	 */
	if (portPtr->LS_Flag)
		tval = TL_Min;
	else
		tval = T_Out;

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Join_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Join_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into JOIN.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval;					/* timeout value */

	/*
	 * Set TPC
	 */
	if (portPtr->LS_Flag)
		tval = TL_Min;
	else
		tval = T_Out;

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);
	return;
}

static void
#ifdef __STDC__
Verify_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Verify_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into VERIFY.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval;					/* timeout value */

	/*
	 * Set TPC
	 */
	if (portPtr->LS_Flag)
		tval = TL_Min;

	else
		tval = T_Out;

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	return;
}

static void
#ifdef __STDC__
Active_Entry (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Active_Entry (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform actions for a transition into ACTIVE.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
uInt32           tval;					/* timeout value */

	/*
	 * Set TPC
	 */
	if ((portPtr->LS_Flag) && (!portPtr->CF_Join))
		tval = TL_Min;

	else if (!portPtr->LS_Flag)
		tval = T_Out;

	else
		tval = 0;

	SetCSPTimerMulti (stnID, tval, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	return;
}


/*********************************************************************
		PCM State Machine Actions
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
Function:	Perform action for a transition to OFF.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
	/* disable PORT interrupts */
	portPtr->intrState = PORT_INT_NONE;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* disable PMD */
	TransmitPMDMulti (stnID, portPtr->ID, DISABLE);

	/* transmit quiet */
	SendLineStateMulti (stnID, portPtr->ID, QLS);

	/* clear CF_Loop */
	SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

	/* clear CF_Join */
	SendSignalMulti (stnID, SIG_CF_Join, portPtr->ID, (uInt32) CLEAR);

	/* clear BS_Flag */
	portPtr->BS_Flag = CLEAR;
	SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);

	/* set PC_Withhold */
	portPtr->PC_Withhold = PCW_none;
	SendCSPEvent (stnID, fddiPORTPC_Withhold, portPtr->ID);

	/* set Connect_State */
	portPtr->Connect_State = CS_disabled;
	SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);

	/* set current PCM state */
	portPtr->pcState = PC_OFF;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* turn off scrubbing */
	SetCSPTimerMulti (stnID, (uTime) 0, PCM_SIGNALS, portPtr->ID,
		&portPtr->TScrub);

	/* clean-up port management */
	MCleanupPortMgmt (stnID, portPtr);

	/* enter OFF state */
	Off_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Break_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Break_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to BREAK.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* disable PMD */
	TransmitPMDMulti (stnID, portPtr->ID, DISABLE);

	/* transmit quiet */
	SendLineStateMulti (stnID, portPtr->ID, QLS);

	/* enable PORT interrupts */
	portPtr->intrState = PORT_INT_QLS | PORT_INT_HLS | PORT_INT_MLS;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* clear CF_Loop */
	SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

	/* clear CF_Join */
	SendSignalMulti (stnID, SIG_CF_Join, portPtr->ID, (uInt32) CLEAR);

	/* clear BS_Flag */
	portPtr->BS_Flag = CLEAR;
	SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* set PC_Mode */
	portPtr->PC_Mode = PC_Mode_None;

	/* clean up port management */
	MCleanupPortMgmt (stnID, portPtr);

	/* set connect state */
	if (portPtr->PC_Withhold == PCW_none)
	{
		/* set connect state */
		if (portPtr->Connect_State != CS_connecting)
		{
			portPtr->Connect_State = CS_connecting;
			SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);
		}
	}

	/* set current PCM state */
	portPtr->pcState = PC_BREAK;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* turn off scrubbing */
	SetCSPTimerMulti (stnID, (uTime) 0, PCM_SIGNALS, portPtr->ID,
		&portPtr->TScrub);

	/* enter BREAK state */
	Break_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Trace_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Trace_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to TRACE.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
	/* disable EB detection */
	portPtr->intrState &= ~(PORT_INT_EB | PORT_INT_NOISE);
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_TRACE;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* turn off scrubbing */
	SetCSPTimerMulti (stnID, (uTime) 0, PCM_SIGNALS, portPtr->ID,
		&portPtr->TScrub);

	/*
	 * Since the line state flag has not been set, test for line states that
	 * may already exist.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(22a) */
	if ((portPtr->PC_LS & (QLS | HLS))
		&& (CSPDATA (stnID, stationData.Path_Test) == PT_Passed))
	{
		SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Pending);
	}
	/* PC(22b) */
	else if ((portPtr->PC_LS & MLS) && !portPtr->TR_Flag)
	{

		portPtr->LS_Flag = SET;
		SendSignalMulti (stnID, SIG_Trace_Prop, portPtr->ID, (uInt32) RT_port);
	}
	/* transmit Master */
	SendLineStateMulti (stnID, portPtr->ID, MLS);

	return;
}

static void
#ifdef __STDC__
Connect_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Connect_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to CONNECT.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* clear BS_Flag */
	portPtr->BS_Flag = CLEAR;
	SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);

	/*
	 * Connection is starting. Enable detection of SILS during the connection
	 * process.
	 */
	portPtr->intrState |= PORT_INT_SILS;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* set current PCM state */
	portPtr->pcState = PC_CONNECT;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/*
	 * Check for a other line state transitions.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(33) */
	if (portPtr->PC_LS & HLS)
		portPtr->LS_Flag = SET;

	/* PC(31b) */
	else if (portPtr->PC_LS & SILS)
	{

		Break_Actions (stnID, portPtr);
		return;
	}
	/* enable PMD transmit */
	TransmitPMDMulti (stnID, portPtr->ID, ENABLE);

	/* transmit Halt */
	SendLineStateMulti (stnID, portPtr->ID, HLS);

	/* enter CONNECT state */
	Connect_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Next_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Next_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to NEXT.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* clear LS_FLag, RC_Flag, TC_Flag, TD_Flag */
	portPtr->LS_Flag = CLEAR;
	portPtr->RC_Flag = CLEAR;
	portPtr->TC_Flag = CLEAR;
	portPtr->TD_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_NEXT;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/*
	 * Check for other line state transitions.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(44a) */
	if (portPtr->PC_LS & SILS)
	{
		/* turn off SILS detect */
		portPtr->intrState &= ~PORT_INT_SILS;
		SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);
		portPtr->LS_Flag = SET;
	}
	/*
	 * Recheck for QLS. QLS may occur in CONNECT just prior to the PC(34)
	 * transition. If this happens, the CONNECT state fields the line state
	 * change interrupt which results in no action. When the timer expires
	 * for PC(34) the state machine gets here expecting the line state to
	 * have remained stable and does not check for QLS.
	 * 
	 * This test for QLS must be done after transmitting ILS which would force
	 * the neighbor from CONNECT to BREAK. Also, since ReadLineStateMulti()
	 * has been called above, the code is assured that the QLS occurred while
	 * in NEXT. This means that if QLS appears in PC_LS the transition to
	 * BREAK can be taken.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);
	if (portPtr->PC_LS & QLS)
	{
		/* PC(41b) */
		Break_Actions (stnID, portPtr);
	}
	/*
	 * Check for Next_Error: (n > 4 & T_Val(4) & T_Val(5) & (not WC_Flag) &
	 * Connection_Policy(PC_Type, PC_Neighbor))
	 */
	else if ((portPtr->n > 4)
			&& !portPtr->WC_Flag
			&& (MReadBit (portPtr->T_Val, 4)
				&& MReadBit (portPtr->T_Val, 5)
		/* bit 3 is same as Conn_Policy */
			&& MReadBit (portPtr->T_Val, 3)))
	{
		/* PC(41b) */
		Break_Actions (stnID, portPtr);
	}
	else
	{
		/* transmit Idle */
		SendLineStateMulti (stnID, portPtr->ID, ILS);

		/* enter NEXT state */
		Next_Entry (stnID, portPtr);
	}

	return;
}

static void
#ifdef __STDC__
Signal_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Signal_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to SIGNAL.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_SIGNAL;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* check for other line state transitions */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(55a) */
	if (portPtr->PC_LS & HLS)
	{
		portPtr->LS_Flag = SET;
		MSetBit (portPtr->R_Val, portPtr->n, SET);
                /**
                 * WRF
                 * Log CMT debugging information.
                 **/
                fddi_cbug_pcm_rval (stnID, portPtr->ID, portPtr->n, 1);
	}
	/* PC(55b) */
	else if (portPtr->PC_LS & MLS)
	{
		portPtr->LS_Flag = SET;
		MSetBit (portPtr->R_Val, portPtr->n, CLEAR);
                /**
                 * WRF
                 * Log CMT debugging information.
                 **/
                fddi_cbug_pcm_rval (stnID, portPtr->ID, portPtr->n, 0);
	}
	/* transmit HLS for 1 or MLS for 0 */
	if (MReadBit (portPtr->T_Val, portPtr->n)) {
		SendLineStateMulti (stnID, portPtr->ID, HLS);
                /**
                 * WRF
                 * Log CMT debugging information.
                 **/
                fddi_cbug_pcm_tval (stnID, portPtr->ID, portPtr->n, 1);
	} else {
		SendLineStateMulti (stnID, portPtr->ID, MLS);
                /**
                 * WRF
                 * Log CMT debugging information.
                 **/
                fddi_cbug_pcm_tval (stnID, portPtr->ID, portPtr->n, 0);
	}

	/* enter SIGNAL */
	Signal_Entry (stnID, portPtr);

	/*
	 * Turn on SILS detection after transmitting line state. If SILS is
	 * currently being received, then SetPORTInterruptsMulti() must be sure
	 * to generate an interrupt. Some systems will not generate the SILS
	 * interrupt if the SILS detection is turned on while SILS is the current
	 * line state.
	 */
	portPtr->intrState |= PORT_INT_SILS;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	return;
}

static void
#ifdef __STDC__
Join_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Join_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to JOIN.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

#ifndef	XM_NO_M_PORT

	/*
	 * Normally, an extended LCT indicates that a link is being withheld.
	 * After the LCT, the PCM completes the signalling process and restarts
	 * the connection. However, some implementations may choose to use the
	 * extended LCT and then proceed with the connection.
	 * 
	 * For port management processing, other ports are allowed to connect while
	 * this port is doing an extended LCT. If this port wants to continue
	 * with the connection process, PCM must make sure that there is enough
	 * room to handle another connecting port.
	 */
	if ((portPtr->PC_Type == PT_m)
		&& CSPDATA (stnID, stationData.maxMasterCanConnect)
		&& !portPtr->isConnecting
		&& (portPtr->LC_Test == LC_Extended))
	{
		if (CSPDATA (stnID, stationData.numPortsConnecting)
			< CSPDATA (stnID, stationData.maxMasterCanConnect))
		{
			++CSPDATA (stnID, stationData.numPortsConnecting);
			portPtr->isConnecting = TRUE;
		}
		else
		{
			Break_Actions (stnID, portPtr);
			return;
		}
	}

#endif							/* XM_NO_M_PORT */

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_JOIN;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* check for other line state transitions */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(66) */
	if (portPtr->PC_LS & HLS)
		portPtr->LS_Flag = SET;

	/* Turn on SILS detection. */
	portPtr->intrState |= PORT_INT_SILS;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* transmit Halt */
	SendLineStateMulti (stnID, portPtr->ID, HLS);

	/* enter JOIN */
	Join_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Verify_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Verify_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to VERIFY.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* clear LS_Flag */
	portPtr->LS_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_VERIFY;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/*
	 * Check for other line state transitions.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(77) */
	if (portPtr->PC_LS & MLS)
		portPtr->LS_Flag = SET;

	/* PC(71b) */
	else if (portPtr->PC_LS & SILS)
	{
		Break_Actions (stnID, portPtr);
		return;
	}
	/* transmit Master */
	SendLineStateMulti (stnID, portPtr->ID, MLS);

	/* enter VERIFY */
	Verify_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Active_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Active_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to ACTIVE.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{

	/* clear LS_Flag, TR_Flag */
	portPtr->LS_Flag = CLEAR;
	portPtr->TR_Flag = CLEAR;

	/* set Connect_State */
	portPtr->Connect_State = CS_active;
	SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);

	/* clear scrubbing timer to indicate no scrubbing in progress */
	portPtr->TScrub = 0;

	/* set current PCM state */
	portPtr->pcState = PC_ACTIVE;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/*
	 * Check for other line state transitions.
	 */
	portPtr->PC_LS = ReadLineStateMulti (stnID, portPtr->ID);

	/* PC(88a) */
	if (portPtr->PC_LS & SILS)
	{
		portPtr->LS_Flag = SET;

		/* connection process has completed, disable SILS detection */
		portPtr->intrState &= ~PORT_INT_SILS;
		SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);
	}
	/* transmit Idle */
	SendLineStateMulti (stnID, portPtr->ID, ILS);

	/* enter ACTIVE */
	Active_Entry (stnID, portPtr);

	return;
}

static void
#ifdef __STDC__
Maint_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Maint_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perfrom actions for a transition to MAINT.
Parameters:	stnID	= index of station to use.
			portPtr	= PORT's array element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
	/* disable PORT interrupts */
	portPtr->intrState = PORT_INT_NONE;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* set current PCM state */
	portPtr->pcState = PC_MAINT;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* set PMD */
	if (portPtr->Maint_LS == QLS)
		TransmitPMDMulti (stnID, portPtr->ID, DISABLE);
	else
		TransmitPMDMulti (stnID, portPtr->ID, ENABLE);

	/* turn off scrubbing */
	SetCSPTimerMulti (stnID, (uTime) 0, PCM_SIGNALS, portPtr->ID,
		&portPtr->TScrub);

	/* transmit requested line state */
	SendLineStateMulti (stnID, portPtr->ID, portPtr->Maint_LS);

	return;
}

/*********************************************************************
		PCM State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalPCM (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalPCM (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Process a PCM signal.
Parameters:	stnID		= index of station to use.
			sigType		= the signal ID.
			sigEntity	= entity (PHY) to process.
			sigData		= any data associated with the signal.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
struct CSPPORTInfo *portPtr;			/* port element to use */

	/* if Port is not present, then ignore the signal */
	if (!(CSPDATA (stnID, portData[sigEntity].hwPresent)))
		return;

	/* PORT data pointer */
	portPtr = &(CSPDATA (stnID, portData[sigEntity]));

	/*
	 * For more efficient processing, some signals are preprocessed here to
	 * avoid duplicate code throughout the state machine.
	 */
	switch (sigType)
	{

		/*
		 * Set line state changes.
		 */
	case SIG_Line_State:
		portPtr->PC_LS = (uInt16) sigData;
		break;

		/*
		 * If timer has expired, then verify this timer is the current event
		 * being timed.
		 */
	case SIG_PC_Timer:
		if (portPtr->TPC != sigData)
		{
			/* check noise timer */
			if (portPtr->TNE == sigData)
			{
				sigType = SIG_Noise_Timer;
				portPtr->TNE = 0;
			}
			/* check scrub timer */
			else if (portPtr->TScrub == sigData)
			{
				/*
				 * Convert signal type so that other scrubbing mechanisms can
				 * also signal scrubbing completion allowing PCM to change
				 * the PHY to transmit PDRs.
				 */
				sigType = SIG_PC_Scrub_Complete;
				portPtr->TScrub = 0;
			}
			/* not current timer */
			else
				return;
		}
		else
			/* mark timer as expired */
			portPtr->TPC = 0;
		break;

		/*
		 * Handle general transitions. PC_Start and PC_Stop signals are
		 * processed from all states except PC9:MAINT.
		 */
	case SIG_PC_Start:
		/* PC(01a) - PC(81a) */
		if (portPtr->pcState != PC_MAINT)
			/* go to BREAK state */
			Break_Actions (stnID, portPtr);

		return;

	case SIG_PC_Stop:
		/* PC(00) - PC(80) */
		if (portPtr->pcState != PC_MAINT)
		{
			/* go to OFF state */
			Off_Actions (stnID, portPtr);
		}

		return;

		/**
		 * PC(09a) - PC(99a)
		 * Handle general transitions. PC_Disable signal processed from all
		 * states.
		 **/
	case SIG_PC_Disable:
	    /**
	     * 731-004 LJP
	     * The PC_Disable signal must be processed in two steps. First,
	     * enter the Off state to force a configuration change. This puts
	     * CF_Join and CF_Loop signals on the signal queue. Then, re-issue
	     * the SIG_PC_Disable signal. On the second pass, the configuration
	     * change has been completed and the state machine goes to Maint.
	     **/
	    if (portPtr->pcState != PC_OFF)
	    {
		/* stop PCM first */
		Off_Actions (stnID, portPtr);

		/* resend PC_Disable so we come through again */
		SendSignalMulti (stnID, SIG_PC_Disable, sigEntity, (uInt32) 0);
	    }

	    else
	    {
		/* set current PCM state */
		portPtr->pcState = PC_MAINT;
		SendCSPEvent (stnID, fddiPORTPCMState, sigEntity);
	    }
	    return;
	    /* break */
	}

	/*
	 * Select PCM state for processing.
	 */
	switch (portPtr->pcState)
	{
		/*
		 * PC0:OFF
		 */
	case PC_OFF:
		switch (sigType)
		{
		case SIG_PC_Maint:
			/* PC(09b) set maintenance line state */
			portPtr->Maint_LS = (uInt16) sigData;
			Maint_Actions (stnID, portPtr);
			break;
		}
		break;

		/*
		 * PC1:BREAK
		 */
	case PC_BREAK:
		/*
		 * Note: In this state, the LS_Flag is used to determine if TB_Min
		 * time has expired.
		 */
		switch (sigType)
		{
		case SIG_PC_Timer:
			/* if TB_Max has expired */
			if (portPtr->LS_Flag)
			{
				/* PC(11b) Set BS_Flag */
				portPtr->BS_Flag = SET;
				SendCSPEvent (stnID, fddiPORTBS_Flag, sigEntity);
			}
			else
			{
				/* TB_Min has expired. Set LS_Flag */
				portPtr->LS_Flag = SET;

				/* if current line state is QLS or HLS */
				portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
				if (portPtr->PC_LS & (QLS | HLS))
					/* PC(13) */
					Connect_Actions (stnID, portPtr);
				else
					/* re-enter BREAK */
					Break_Entry (stnID, portPtr);
			}
			break;

		case SIG_Line_State:
			/* if enough time has passed */
			if (portPtr->LS_Flag)
			{
				/* if current line state is QLS or HLS */
				portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
				if (portPtr->PC_LS & (QLS | HLS))
					/* PC(13) */
					Connect_Actions (stnID, portPtr);
			}
			break;
		}
		break;

		/*
		 * PC2:TRACE
		 */
	case PC_TRACE:
		switch (sigType)
		{
		case SIG_Line_State:
			if ((portPtr->PC_LS & (QLS | HLS))
				&& (!portPtr->LS_Flag)
				&& (CSPDATA (stnID, stationData.Path_Test == PT_Passed)))
			{
				/* PC(22a) */
				SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Pending);
			}
			else if ((portPtr->PC_LS & (MLS)) && (!portPtr->LS_Flag)
				&& (!portPtr->TR_Flag))
			{
				/* PC(22b) */
				portPtr->LS_Flag = SET;
				SendSignalMulti (stnID, SIG_Trace_Prop,
					sigEntity, (uInt32) RT_port);
			}
			break;
		}
		break;

		/*
		 * PC3:CONNECT
		 */
	case PC_CONNECT:
		switch (sigType)
		{
		case SIG_Line_State:
			/* check line states only if LS_Flag is not set */
			if (!portPtr->LS_Flag)
			{
				if (portPtr->PC_LS & HLS)
				{
					/* PC(33) */
					portPtr->LS_Flag = SET;
					Connect_Entry (stnID, portPtr);
				}
				else if (portPtr->PC_LS & (SILS))
					/* PC(31b) If IDLE seen before HALT, restart connection */
					Break_Actions (stnID, portPtr);
			}
			break;

		case SIG_PC_RETRY:
			/* if waiting for timer to expire do nothing */
			if (portPtr->TPC)
				break;

			/* else fall through and do PC(34) */

		case SIG_PC_Timer:
			if (portPtr->LS_Flag)
			{
#ifndef	XM_NO_M_PORT
				/* if M port and port management is active */
				if ((portPtr->PC_Type == PT_m)
					&& CSPDATA (stnID, stationData.maxMasterCanConnect))
				{
					/* if # ports exceeds max */
					if ((CSPDATA (stnID, stationData.numPortsConnecting) >=
							CSPDATA (stnID, stationData.maxMasterCanConnect)))
					{
						/* mark port waiting to connect */
						portPtr->isWaiting = TRUE;
						break;
					}

					else
					{
						/* start a new connection */
						CSPDATA (stnID, stationData.numPortsConnecting)++;
						portPtr->isConnecting = TRUE;
					}
				}
#endif							/* XM_NO_M_PORT */

				/* PC(34) */
				portPtr->n = 0;
				Next_Actions (stnID, portPtr);
			}
			break;
		}
		break;

		/*
		 * PC4:NEXT
		 */
	case PC_NEXT:
		switch (sigType)
		{
		case SIG_Line_State:
			if (portPtr->PC_LS & QLS)
				/* PC(41b) */
				Break_Actions (stnID, portPtr);

			else if ((portPtr->PC_LS & SILS) && (!portPtr->LS_Flag))
			{
				/* PC(44a) */
				portPtr->intrState &= ~PORT_INT_SILS;
				SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);
				portPtr->LS_Flag = SET;
				Next_Entry (stnID, portPtr);
			}
			else if ((portPtr->PC_LS & (HLS | MLS)) && portPtr->TD_Flag
				&& !portPtr->TC_Flag)
			{
				/* PC(44d) */
				portPtr->TC_Flag = SET;
				PC_TCode_Actions (stnID, portPtr);
				Next_Entry (stnID, portPtr);
			}
			break;

		case SIG_PC_Timer:
			if (!portPtr->LS_Flag)
			{
				/*
				 * PC(41b) Check for break condition first (TPC > T_Out).
				 * Note that no other flags can get set until LS_Flag is set.
				 * Break only if n != 0.
				 */
				if (portPtr->n != 0)
					Break_Actions (stnID, portPtr);
			}
			else if (!portPtr->RC_Flag)
			{
				/* PC(44b) Otherwise next timer set is TPC > TL_Min */
				portPtr->RC_Flag = SET;
				PC_RCode_Actions (stnID, portPtr);
				Next_Entry (stnID, portPtr);

				portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
				if ((portPtr->PC_LS & (HLS | MLS))
					&& portPtr->TD_Flag && !(portPtr->TC_Flag))
				{
					/*
					 * PC(44d) If HLS or MLS occurs before TD_Flag then the
					 * event is ignored. Check LS here to see if HLS or MLS
					 * rec'd before code got a chance to start LCT.
					 */
					portPtr->TC_Flag = SET;
					PC_TCode_Actions (stnID, portPtr);
					Next_Entry (stnID, portPtr);
				}
			}
			else if (portPtr->TD_Flag && !portPtr->TC_Flag)
			{
				/* PC(44d) Otherwise check for transmit delay TPC > T_Next(n) */
				portPtr->TC_Flag = SET;
				PC_TCode_Actions (stnID, portPtr);
				Next_Entry (stnID, portPtr);
			}
			break;

		case SIG_PC_PDR:
			/* PC(44c) */
			SendLineStateMulti (stnID, sigEntity, PDRLS);
			Next_Entry (stnID, portPtr);
			break;

		case SIG_PC_Signal:
			/* PC(45) */
			Signal_Actions (stnID, portPtr);
			break;

		case SIG_PC_Join:
			if (portPtr->WC_Flag)
				/*
				 * Due to the delay added by the signal queue, a change to
				 * WC_Flag may not be read until PCM has left the NEXT state.
				 * Check WC_Flag before proceeding.
				 */
				Break_Actions (stnID, portPtr);

			else
				/* PC(46) */
				Join_Actions (stnID, portPtr);

			break;

		case SIG_PC_WC_Flag:
			/*
			 * Check for Next_Error: (n > 4 & T_Val(4) & T_Val(5) & (not
			 * WC_Flag) & Connection_Policy(PC_Type, PC_Neighbor))
			 */
			if ((portPtr->n > 4)
				&& !portPtr->WC_Flag
				&& (MReadBit (portPtr->T_Val, 4)
					&& MReadBit (portPtr->T_Val, 5)
			/* bit 3 is same as Conn_Policy */
					&& MReadBit (portPtr->T_Val, 3)))
			{
				/* PC(41b) */
				Break_Actions (stnID, portPtr);
			}
			break;
		}
		break;

		/*
		 * PC5:SIGNAL
		 */
	case PC_SIGNAL:
		switch (sigType)
		{
		case SIG_Line_State:
			if (portPtr->PC_LS & QLS)
				/* PC(51b) */
				Break_Actions (stnID, portPtr);

			/* Only process changes if LS_Flag is not set.  */
			else if (!portPtr->LS_Flag)
			{
				if (portPtr->PC_LS & HLS)
				{
					/* PC(55a) */
					portPtr->LS_Flag = SET;
					MSetBit (portPtr->R_Val, portPtr->n, SET);
			                /**
                			 * WRF
                			 * Log CMT debugging information.
			                 **/
                                        fddi_cbug_pcm_rval (stnID, portPtr->ID,portPtr->n, 1);

					/* Turn on SILS detection after LS_Flag is set */
					portPtr->intrState |= PORT_INT_SILS;
					SetPORTInterruptsMulti (stnID, portPtr->ID,
						portPtr->intrState);
					Signal_Entry (stnID, portPtr);
				}
				if (portPtr->PC_LS & MLS)
				{
					/* PC(55b) */
					portPtr->LS_Flag = SET;
					MSetBit (portPtr->R_Val, portPtr->n, CLEAR);
			                /**
                			 * WRF
                			 * Log CMT debugging information.
			                 **/
                                        fddi_cbug_pcm_rval (stnID, portPtr->ID,portPtr->n, 0);

					/* Turn on SILS detection after LS_Flag is set */
					portPtr->intrState |= PORT_INT_SILS;
					SetPORTInterruptsMulti (stnID, portPtr->ID,
						portPtr->intrState);
					Signal_Entry (stnID, portPtr);
				}
			}
			break;

		case SIG_PC_Timer:
			if (!portPtr->LS_Flag)
				/* PC(51b) */
				Break_Actions (stnID, portPtr);

			else
			{
				/* PC(54) */
				++portPtr->n;
				Next_Actions (stnID, portPtr);
			}
			break;
		}
		break;

		/*
		 * PC6:JOIN
		 */
	case PC_JOIN:
		switch (sigType)
		{
		case SIG_Line_State:
			if (portPtr->PC_LS & QLS)
			{
				/* PC(61b) */
				Break_Actions (stnID, portPtr);
			}
			else if (!portPtr->LS_Flag && (portPtr->PC_LS & HLS))
			{
				/* PC(66) */
				portPtr->LS_Flag = SET;
				Join_Entry (stnID, portPtr);
			}
			else
			{
				/*
				 * Second part to PC(61b) Note: PC_LS must be set to the
				 * current line state to check ILS because it could show both
				 * ILS and HLS from a legitimate PC(46) transition.
				 */
				portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
				if (portPtr->LS_Flag && (portPtr->PC_LS & SILS))
					Break_Actions (stnID, portPtr);
			}

			break;

		case SIG_PC_Timer:
			if (!portPtr->LS_Flag)
				/* PC(61b) */
				Break_Actions (stnID, portPtr);

			else
			{
				if (portPtr->WC_Flag)
					/*
					 * Due to the delay added by the signal queue, a change
					 * to WC_Flag may not be read until PCM has left the NEXT
					 * state. Check WC_Flag before proceeding.
					 */
					Break_Actions (stnID, portPtr);

				else
					/* PC(67) */
					Verify_Actions (stnID, portPtr);
			}

			break;

		case SIG_PC_WC_Flag:
			if (portPtr->WC_Flag)
				/*
				 * Due to the delay added by the signal queue, a change to
				 * WC_Flag may not be read until PCM has left the NEXT state.
				 * To handle this condition, the SIG_PC_WC_Flag signal must
				 * be checked in JOIN and VERIFY (it is already defined in
				 * ACTIVE).
				 */
				Break_Actions (stnID, portPtr);
			break;
		}
		break;

		/*
		 * PC7:VERIFY
		 */
	case PC_VERIFY:
		switch (sigType)
		{
		case SIG_Line_State:
			if (portPtr->PC_LS & QLS)
				/*
				 * PC(71b) Since ACTIVE sends ILS, this code must check for
				 * QLS, then MLS, then ILS to insure sequencing proceeds
				 * correctly.
				 */
				Break_Actions (stnID, portPtr);

			else if (!portPtr->LS_Flag)
			{
				if (portPtr->PC_LS & MLS)
				{
					/* PC(77) */
					portPtr->LS_Flag = SET;
					Verify_Entry (stnID, portPtr);
				}
				else if (portPtr->PC_LS & SILS)
					/* PC(71b) */
					Break_Actions (stnID, portPtr);
			}
			break;

		case SIG_PC_Timer:
			if (!portPtr->LS_Flag)
				/* PC(71b) */
				Break_Actions (stnID, portPtr);

			else
			{
				if (portPtr->WC_Flag)
					/*
					 * Due to the delay added by the signal queue, a change
					 * to WC_Flag may not be read until PCM has left the NEXT
					 * state. Check WC_Flag before proceeding.
					 */
					Break_Actions (stnID, portPtr);

				else
					/* PC(78) */
					Active_Actions (stnID, portPtr);
			}

			break;

		case SIG_PC_WC_Flag:
			if (portPtr->WC_Flag)
				/*
				 * Due to the delay added by the signal queue, a change to
				 * WC_Flag may not be read until PCM has left the NEXT state.
				 * To handle this condition, the SIG_PC_WC_Flag signal must
				 * be checked in JOIN and VERIFY (the check is already
				 * defined in ACTIVE).
				 */
				Break_Actions (stnID, portPtr);
			break;
		}
		break;

		/*
		 * PC8:ACTIVE
		 */
	case PC_ACTIVE:
		switch (sigType)
		{
		case SIG_PC_Timer:
			if (!portPtr->LS_Flag && !portPtr->TR_Flag)
			{
				/* PC(81b) */
				Break_Actions (stnID, portPtr);
			}
			/* TL_Min expired */
			else if (portPtr->LS_Flag && !portPtr->CF_Join)
			{
				/*
				 * PC(88c) Check for MLS received. All line states read here
				 * have occurred since entering ACTIVE due to read on entry.
				 */
				portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
				if ((portPtr->PC_LS & MLS) && !portPtr->TR_Flag)
				{

					/*
					 * PC(81b) Treat reception of MLS before configuring
					 * station as a fault condition. This terminates the
					 * Trace and does not include the station in the fault
					 * domain since it was not part of the token path.
					 */
					Break_Actions (stnID, portPtr);
				}
				else
				{
					/*
					 * PC(88b1) Refer to SIG_PC_PDR in PC(88b2) for
					 * explanation about dividing this transition into two
					 * parts.
					 */
					SendSignalMulti (stnID, SIG_CF_Join,
						sigEntity, (uInt32) SET);
					SendSignalMulti (stnID, SIG_PC_PDR, sigEntity, (uInt32) 0);
				}

				Active_Entry (stnID, portPtr);
			}
			break;

		case SIG_Line_State:
			if (!portPtr->TR_Flag && (portPtr->PC_LS & (HLS | QLS)))
			{
				/*
				 * PC(81b) Only transition to BREAK if TR_Flag not set.
				 */
				Break_Actions (stnID, portPtr);
			}
			else if (portPtr->CF_Join && !portPtr->TR_Flag
				&& (portPtr->PC_LS & MLS))
			{
				/*
				 * PC(88c) Use CF_Join flag instead of LS_Flag because the
				 * line state change interrupt may be due to the PC(78)
				 * transition.
				 */
				portPtr->TR_Flag = SET;
				SendSignalMulti (stnID, SIG_Trace_Prop,
					sigEntity, (uInt32) RT_port);
				Active_Entry (stnID, portPtr);
			}

			else if (portPtr->PC_LS & SILS)
			{
				/*
				 * If the noise timer is currently running, it can be stopped
				 * as soon as Idle is detected on the link.
				 */
				if (portPtr->TNE > 0)
					SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &portPtr->TNE);

				if (!portPtr->LS_Flag)
				{
					/* PC(88a) */
					portPtr->LS_Flag = SET;

					/*
					 * Connection process is complete. Disable SILS
					 * detection.
					 */
					portPtr->intrState &= ~PORT_INT_SILS;
					SetPORTInterruptsMulti (stnID, portPtr->ID,
						portPtr->intrState);
					Active_Entry (stnID, portPtr);
				}
			}

			/*
			 * Handle NLS reported as a separate condition.
			 */
			if ((portPtr->PC_LS & NLS) && (portPtr->TNE == 0))
			{
				/*
				 * PC(88na) This is an additional state to allow PCM to time
				 * the noise condition on a link. When noise is detected by
				 * the PHY, PCM may begin the noise timer.
				 */
				SetCSPTimerMulti (stnID, NS_Max, PCM_SIGNALS,
					sigEntity, &portPtr->TNE);
			}
			break;

		case SIG_PC_Trace:
			/* PC(82) */
			Trace_Actions (stnID, portPtr);
			break;

		case SIG_Noise_Event:
			if (!portPtr->TR_Flag)
				/*
				 * PC(81b) This signal replaces the TNE > NS_Max term on
				 * PC(81b) to allow noise detection either by hardware or
				 * PCM. This signal is used when a link is to be disconnected
				 * due to excessive noise.
				 */
				Break_Actions (stnID, portPtr);
			break;

		case SIG_Noise_Timer:
			/*
			 * PC(88nb) When the timer event is associated with the
			 * expiration of the noise timer, the event type is changed to
			 * SIG_Noise_Timer. This indicates that PCM has timed the noise
			 * condition on the link. If no Idle symbols have been seen since
			 * the timer began, then the link must be disconnected.
			 */
			portPtr->PC_LS = ReadLineStateMulti (stnID, sigEntity);
			if (!(portPtr->PC_LS & (ILS | SILS)))
				SendSignalMulti (stnID, SIG_Noise_Event,
					sigEntity, (uInt32) 0);
			break;

		case SIG_PC_PDR:
			/*
			 * PC(88b2) This is the second half of PC(88b). Since a software
			 * implementation of PCM cannot guarantee how long it will take
			 * for CFM to respond to the CF_Join flag, the SIG_PC_PDR signal
			 * is used. By placing the two events sequentially in the signal
			 * list, PCM is guaranteed that the station configuration has
			 * been completed before allowing the PHY to transmit PDUs.
			 * 
			 * Also, for Idle-based scrubbing, this transition must be held off
			 * when scrubbing is in progress (TScrub != 0). Having SIG_PC_PDR
			 * as a separate transition makes this operation easier to
			 * implement.
			 */
			if (portPtr->TScrub == 0)
			{
				SendLineStateMulti (stnID, sigEntity, PDRLS);

				/* enable EB detection */
				portPtr->intrState |= (PORT_INT_EB | PORT_INT_NOISE);
				SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

				SendSignalMulti (stnID, SIG_LEM_Start, sigEntity, (uInt32) 0);
				Active_Entry (stnID, portPtr);
			}
			break;

		case SIG_PC_WC_Flag:
			if (portPtr->WC_Flag)
				/*
				 * PC(81b) CFM has changed the state of the WC_Flag. Check
				 * new state and see if transition is required.
				 */
				Break_Actions (stnID, portPtr);
			break;

		case SIG_PC_Scrub_Complete:
			/*
			 * PC(88scrub) Additional state change to handle scrubbing using
			 * Idle symbols. When Idle-based scrubbing completes, the timer
			 * expiration signal is converted into this signal to restart
			 * sending PDRs through the PHY. Other scrubbing mechanisms may
			 * also send SIG_PC_Scrub_Complete to change the PHY transmission
			 * back to sending PDRs.
			 */
			SendSignalMulti (stnID, SIG_PC_PDR, sigEntity, (uInt32) 0);
			Active_Entry (stnID, portPtr);
			break;

		case SIG_PC_LEM_Fail:
			if (!portPtr->TR_Flag)
				/* PC(81b) */
				Break_Actions (stnID, portPtr);
			break;
		}
		break;

		/*
		 * PC9:MAINT
		 */
	case PC_MAINT:
		if (sigType == SIG_PC_Maint)
		{
			/*
			 * PC(99b) This is the only event recognized for this state at
			 * this point in the state machine.
			 */
			portPtr->Maint_LS = (uInt16) sigData;
			Maint_Actions (stnID, portPtr);
		}
		else if (sigType == SIG_PC_Enable)
			/* PC(90) */
			Off_Actions (stnID, portPtr);

		break;
	}

	return;
}
