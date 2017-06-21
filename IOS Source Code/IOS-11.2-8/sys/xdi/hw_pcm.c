/* $Id: hw_pcm.c,v 3.1.60.1 1996/03/22 00:16:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/hw_pcm.c,v $
 *------------------------------------------------------------------
 * hw_pcm.c - 1F MIF68840 based PCI port adaptor support
 *             ( Port adapter platforms common PCM XDI code)
 *
 * December 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hw_pcm.c,v $
 * Revision 3.1.60.1  1996/03/22  00:16:02  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/02/05  07:19:27  fsunaval
 * Branch: ELC_branch
 *
 * Revision 3.1.2.2  1996/01/25  12:10:55  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.2.1  1995/12/23  04:42:24  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/12/19  20:36:13  fsunaval
 * Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_hw_pcm.c xm7_v3.1.1:cset.0166:20:20 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module

		PCM State Machine
		****	FOR MOTOROLA, DEC, and AMD PHYs WITH BUILT-IN
		****	PCM SUPPORT.

		File:			xm7_hw_pcm.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module supports the hardware-based PCM state machines
		supplied by the DEC, MOTOROLA and AMD PHYs.

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

#include	"cschdrsif.h"
#include	"xdihdrsif.h"
#include    "../if/if_fddi.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"mibdefs.h"

#include	"hw_phy.h"
#include	"hw_csp.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"

/*********************************************************************
	PCM State Machine Macros
*********************************************************************/

#define	MSetBits(val, mask, bits)\
			(((val) & ~(mask)) | (bits))
#define	MGetBIts(val, mask)\
			((val) & (mask))


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

	} else if (portPtr->isWaiting) {
                 portPtr->isWaiting = FALSE;
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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;

    fddi_cbuginf("\nOff_Entry:");

	/* set current PCM state */
	portPtr->pcState = PC_OFF;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* reset TPC */
	SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &portPtr->TPC);

	/* stop PCM */
	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* clear control bits */
	regVal &= ~PCM_CNTRL_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/*
	 * Clear PC_MAINT bit along with setting PC_Stop.
	 */
	regVal &= ~PC_MAINT_MASK;

	/* set PC_Stop */
	regVal |= PCM_CNTRL_OFF;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;

    fddi_cbuginf("\nBreak_Entry");

	/* disable EB & noise detection */
	portPtr->intrState &= ~(PORT_INT_EB | PORT_INT_NOISE);
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* set timer to check for BS_Flag */
	SetCSPTimerMulti (stnID, TB_Max, PCM_SIGNALS, portPtr->ID, &portPtr->TPC);

	/* start the PCM state machine */
	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* clear control bits */
	regVal &= ~PCM_CNTRL_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/* set PC_Start */
	regVal |= PCM_CNTRL_BREAK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

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
Function:	Cause the PHY to transition into JOIN.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;

    fddi_cbuginf("\nJoin_Entry");

	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* clear PC_JOIN bit */
	regVal &= ~PC_JOIN_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/* set PC_JOIN bit to enter JOIN */
	regVal |= PC_JOIN_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Notes:
Modification History:
*********************************************************************/
{
    fddi_cbuginf("\nActive_Entry:");

	/* enable EB and noise detection */
	portPtr->intrState |= (PORT_INT_EB | PORT_INT_NOISE);
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/*
	 * Clear any timer values in TPC
	 */
	if (portPtr->TPC)
		SetCSPTimerMulti (stnID, (uInt32) 0, PCM_SIGNALS, portPtr->ID,
			&portPtr->TPC);

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
Function:	Perform Off_Actions as described in the standard.
			Actual entry into the OFF state is done through
			Off_Entry().
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
    fddi_cbuginf("\nOff_Actions:");

	/* disable PORT interrupts */
	portPtr->intrState = PORT_INT_NONE;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* clear CF_Loop */
	SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

	/* clear CF_Join */
	SendSignalMulti (stnID, SIG_CF_Join, portPtr->ID, (uInt32) CLEAR);

	/* clear BS_Flag */
	if (portPtr->BS_Flag)
	{
		portPtr->BS_Flag = CLEAR;
		SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);
	}

	/* set PC_Withhold */
	portPtr->PC_Withhold = PCW_none;
	SendCSPEvent (stnID, fddiPORTPC_Withhold, portPtr->ID);

	/* set Connect_State */
	portPtr->Connect_State = CS_disabled;
	SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);


    /* Clear PC Neighbor info - seg - */
    portPtr->PC_Neighbor = PT_none;
    SendCSPEvent(stnID,fddiPORTNeighborType,portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* reset port management state */
	MCleanupPortMgmt (stnID, portPtr);

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
Function:	Perfrom actions when the PHY transitions to BREAK.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;

    fddi_cbuginf("\nBreak_Actions:");

	/* clear CF_Loop */
	SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

	/* clear CF_Join */
	SendSignalMulti (stnID, SIG_CF_Join, portPtr->ID, (uInt32) CLEAR);

	/* clear BS_Flag */
	if (portPtr->BS_Flag)
	{
		portPtr->BS_Flag = CLEAR;
		SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);
	}

	/* set PC_Mode */
	portPtr->PC_Mode = PC_Mode_None;

	/* set Connect_State if needed */
	if (portPtr->PC_Withhold == PCW_none)
	{
		portPtr->Connect_State = CS_connecting;
		SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);
	}

	/* set current PCM state */
	if (portPtr->pcState != PC_BREAK)
	{
		/* reset port management state */
		MCleanupPortMgmt (stnID, portPtr);

		portPtr->pcState = PC_BREAK;
		SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);
	}

    /* Clear PC Neighbor info - seg - */
    portPtr->PC_Neighbor = PT_none;
    SendCSPEvent(stnID,fddiPORTNeighborType,portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/*
	 * After running PCM, if the state machine returns to BREAK, the Motorola
	 * Rev C ELM chip requires going to the OFF state before the xmit vector
	 * can be rewritten.
	 */
	/* stop PCM */
	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* clear control bits */
	regVal &= ~PCM_CNTRL_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/* set PC_Stop */
	regVal |= PCM_CNTRL_OFF;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/* set the initial xmit vector and length register */
	portPtr->n = 0;
	PC_RCode_Actions (stnID, portPtr);

#ifndef	XM_NO_M_PORT
	/* enter BREAK state */
	if ((portPtr->PC_Type == PT_m)
		&& CSPDATA (stnID, stationData.maxMasterCanConnect))
	{
		if (CSPDATA (stnID, stationData.numPortsConnecting)
			< CSPDATA (stnID, stationData.maxMasterCanConnect))
			Break_Entry (stnID, portPtr);
		else
			portPtr->isWaiting = TRUE;
	}
	else
#endif							/* XM_NO_M_PORT */
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
Function:	Perfrom actions for a transition to TRACE when PHY
			enters TRACE.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;

    fddi_cbuginf("\nTrace_Actions:");

	/* disable EB detection */
	portPtr->intrState &= ~(PORT_INT_EB | PORT_INT_NOISE);
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* set current PCM state */
	portPtr->pcState = PC_TRACE;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* turn off LEM */
	SendSignalMulti (stnID, SIG_LEM_Stop, portPtr->ID, (uInt32) 0);

	/* put station into trace */
	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* clear control bits */
	regVal &= ~PCM_CNTRL_MASK;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

	/* set PC_Trace */
	regVal |= PCM_CNTRL_TRACE;
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

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
Function:	Perfrom actions when PHY transitions to NEXT.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
    fddi_cbuginf("\nNext_Actions:");

	/* clear TD_Flag */
	portPtr->TD_Flag = CLEAR;

	/* set current PCM state */
	portPtr->pcState = PC_NEXT;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

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
Function:	Perfrom actions when the PHY transitions to JOIN.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
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

	/* set current PCM state */
	portPtr->pcState = PC_JOIN;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* enter JOIN */
	Join_Entry (stnID, portPtr);

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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* set current PCM state */
	portPtr->pcState = PC_ACTIVE;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	/* clear LEM_Fail */
	portPtr->LEM_Fail = CLEAR;

	/* set Connect_State */
	portPtr->Connect_State = CS_active;
	SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);

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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           regVal;
uInt16           lsVal;

	/* disable all PORT interrupts */
	portPtr->intrState = PORT_INT_NONE;
	SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

	/* set current PCM state */
	portPtr->pcState = PC_MAINT;
	SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

	regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

	/* get maintenance line state */
	switch (portPtr->Maint_LS)
	{
	case QLS:
		lsVal = MAINT_LS_QLS;
		break;

	case ILS:
		lsVal = MAINT_LS_ILS;
		break;

	case HLS:
		lsVal = MAINT_LS_HLS;
		break;

	case MLS:
		lsVal = MAINT_LS_MLS;
		break;

	case PDRLS:
		lsVal = MAINT_LS_PDR;
		break;

	default:
		return;
	}

	/* set line state and MAINT state */
	regVal = MSetBits (regVal, MAINT_LS_MASK, lsVal);
	regVal = MSetBits (regVal, PC_MAINT_MASK, PC_MAINT_MASK);
	WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

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
Parameters:	sigType		= the signal ID.
			sigEntity	= entity (PHY) to process.
			sigData		= any data associated with the signal.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Notes:		The implementation of this state machine differs from
			the software implementation by processing based on
			signals instead of signals received in a certain state.
			Since the hardware PCM state can change without notifying
			the software, the software must infer the current PCM state.
			As signals come in, the software determines what state must
			process the signal. In most cases, the xx_Actions() routines
			handle the state processing.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *portPtr;				/* port element to use */
uInt16           regVal;

	/*
	 * PORT data pointer.
	 */
	portPtr = &(CSPDATA (stnID, portData[sigEntity]));

	/*
	 * If Port is not operational, then ignore the signal.
	 */
	if (!portPtr->hwPresent)
		return;

	/*
	 * Handle all signals.
	 */
	switch (sigType)
	{
		/*
		 * This first set of signals comes from the Manager.
		 */
	case SIG_PC_Start:
		/*
		 * PC(01) - PC(81) From all states, except MAINT, this signal starts
		 * the connection. This causes a transition to BREAK.
		 * 
		 * If PCM is possibly in the middle of signalling, the XMIT_VECTOR
		 * cannot be written during Break_Actions(). To insure proper
		 * operation, if PCM is in BREAK, CONNECT, NEXT or SIGNAL, the chip
		 * is stopped and a restart signal is enqueued. This empties the
		 * signal queue of any extra signals and resyncs the h/w and s/w. For
		 * OFF, JOIN, VERIFY, ACTIVE, and TRACE, the PC_Start can be handled
		 * immediately.
		 */
		switch (portPtr->pcState)
		{
		case PC_OFF:
		case PC_JOIN:
		case PC_VERIFY:
		case PC_ACTIVE:
		case PC_TRACE:
			/* go to BREAK */
			Break_Actions (stnID, portPtr);
			break;

		case PC_BREAK:
		case PC_CONNECT:
		case PC_NEXT:
		case PC_SIGNAL:
			/* stop chip */
			regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG),

			/* clear control bits */
				regVal &= ~PCM_CNTRL_MASK;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

			/* set PC_Stop */
			regVal |= PCM_CNTRL_OFF;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

			/* enqueue restart signal */
			SendSignalMulti (stnID, SIG_PC_Restart, portPtr->ID, (uInt32) 0);
			break;

		default:
			break;
		}
		break;

	case SIG_PC_Restart:
		/*
		 * A PC_Start signal occurred while PCM was signalling. The queue has
		 * been cleared of h/w signals for this PHY. If PCM is not in OFF or
		 * MAINT (has not received PC_Stop, PC_Maint, or PC_Disable while
		 * waiting), then restart the connection.
		 */
	case SIG_PCM_BREAK:
		/*
		 * This hardware signal is here below so that it is only handled if
		 * PCM is not in OFF or MAINT (like SIG_PC_Restart). The PHY has
		 * indicated PCM has entered BREAK. Do Break_Actions() to restart
		 * connection.
		 */
		if (portPtr->pcState != PC_OFF && portPtr->pcState != PC_MAINT)
		{
			/* go to BREAK */
			Break_Actions (stnID, portPtr);
		}
		break;

	case SIG_PC_Stop:
		/*
		 * PC(00) - PC(80) From all states, except MAINT, this signal stops
		 * the PCM state machine. This causes a transition to OFF.
		 */
		if (portPtr->pcState != PC_MAINT)
		{
			/* do Off_Actions */
			Off_Actions (stnID, portPtr);

			/* go to OFF */
			Off_Entry (stnID, portPtr);
		}

		break;

	case SIG_PC_Maint:
		/*
		 * PC(09) and PC(99) This signal causes entry into MAINT from OFF or
		 * changes the maintenance line state if in MAINT.
		 */
		if ((portPtr->pcState == PC_OFF) || (portPtr->pcState == PC_MAINT))
		{
			/* set maintenance line state */
			portPtr->Maint_LS = (uInt16) sigData;
			/* go to MAINT */
			Maint_Actions (stnID, portPtr);
		}
		break;

	case SIG_PC_Trace:
		/* PC(82) ECM has requested this PHY to change from ACTIVE to TRACE */
		if (portPtr->pcState == PC_ACTIVE)
			Trace_Actions (stnID, portPtr);
		break;

	case SIG_PC_Join:
		/*
		 * PC(46) The pseudo code has completed its bit signalling and is
		 * ready to go from NEXT to JOIN.
		 */
		if (portPtr->pcState == PC_NEXT)
			Join_Actions (stnID, portPtr);
		break;

	case SIG_PC_Enable:
		/* PC(90) The management agent has requested this PHY to leave MAINT */
		if (portPtr->pcState == PC_MAINT)
		{
			Off_Actions (stnID, portPtr);
			Off_Entry (stnID, portPtr);
		}
		break;

	case SIG_PC_Disable:
		/*
		 * PC(09) - PC(99) The management agent wants this PHY to be in the
		 * MAINT state.
		 */
		/* do Off_Actions */
		Off_Actions (stnID, portPtr);

		/*
		 * Force PHY to OFF to enable MAINT state.
		 */
		Off_Entry (stnID, portPtr);

		/* set maintenance line state to QLS */
		portPtr->Maint_LS = (uInt16) QLS;

		/* enter MAINT */
		Maint_Actions (stnID, portPtr);

		break;

	case SIG_PC_WC_Flag:
		/* select processing based on state */
		switch (portPtr->pcState)
		{
		case PC_NEXT:
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

		case PC_JOIN:
		case PC_VERIFY:
		case PC_ACTIVE:
			/*
			 * Due to the delay added by the signal queue, a change to
			 * WC_Flag may not be read until PCM has left the NEXT state. To
			 * handle this condition, the SIG_PC_WC_Flag signal must be
			 * checked in JOIN and VERIFY (the check  is defined in ACTIVE).
			 */
			if (portPtr->WC_Flag)
			{
				/* PC(81b) */
				Break_Actions (stnID, portPtr);
			}
			break;

		default:
			/* ignore flag in other states */
			break;
		}
		break;

		/*
		 * Handle LEM termination signal.
		 */
	case SIG_PC_LEM_Fail:
		/* PC(81) LEM has reached its cutoff threshold */
		if (portPtr->pcState == PC_ACTIVE)
		{
			/* stop chip */
			regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);

			/* clear control bits */
			regVal &= ~PCM_CNTRL_MASK;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

			/* set PC_Stop */
			regVal |= PCM_CNTRL_OFF;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

			/* enqueue restart signal */
			SendSignalMulti (stnID, SIG_PC_Restart, portPtr->ID,
				(uInt32) 0);
		}
		break;

		/*
		 * Process the SIG_PC_RETRY signal. It is required for performing
		 * port scheduling or graceful insertion.
		 */
	case SIG_PC_RETRY:
		Break_Entry (stnID, portPtr);
		break;

	case SIG_PC_Timer:
		/**
		 * The software PCM timer is used for the following transitions:
		 *
		 * PC(31)
		 * check for stuck in BREAK condition
		 * PC(44d)
		 * timing the LCT and Loopback
		 *
		 * The software handles all LCT timing including the short timing.
		 **/
		if (portPtr->TPC != sigData)
			/* ignore timer if not current ID */
			break;

		/*
		 * Clear timer ID.
		 */
		portPtr->TPC = 0;

		/*
		 * Process event.
		 */
		if (portPtr->pcState == PC_BREAK)
		{
			/* PC(11) If chip is still in BREAK, then set BS_Flag */
			regVal = ReadPHYRegMulti (stnID, portPtr->ID, STATUS_B_REG);
			if ((regVal & PCM_STATE_MASK) == PCM_STATE_BREAK)
			{
				portPtr->BS_Flag = SET;
				SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);
			}

			/*
			 * Else this state machine can move to the CONNECT state. Further
			 * state machine activity will be reported through interrupts
			 * from the chip.
			 */
			else
			{
				/* PC(13) */
				portPtr->pcState = PC_CONNECT;
				SendCSPEvent (stnID, fddiPORTPCMState, portPtr->ID);

				/* clear BS_Flag */
				if (portPtr->BS_Flag)
				{
					portPtr->BS_Flag = CLEAR;
					SendCSPEvent (stnID, fddiPORTBS_Flag, portPtr->ID);
				}
			}
		}

		else if ((portPtr->pcState == PC_NEXT) && portPtr->TD_Flag)
		{
			/*
			 * Set TC_Flag to disable PCM_CODE processing in the interrupt
			 * handler.
			 * 
			 * Clear PC_LOOP bits to mark end of LCT.
			 * 
			 * Post the all clear signal to remove extraneous PCM_CODE signals.
			 * The all clear signal is a PCM_CODE signal with the data
			 * portion set to the value of SIG_PCM_CODE. Normally, this
			 * signal has 0 in the data portion. The LS_Flag is set when the
			 * all clear signal has been processed.
			 */
			if (portPtr->n == 7)
			{
				/* Set TC_Flag */
				portPtr->TC_Flag = SET;

				/* Clear PC_LOOP bits */
				regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);
				regVal &= ~PC_LOOP_MASK;
				WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

				/* Post all clear signal */
				SendSignalMulti (stnID, SIG_PCM_CODE, portPtr->ID,
					(uInt32) SIG_PCM_CODE);
				portPtr->LS_Flag = CLEAR;
			}

			/* PC(44d) Either LCT or Loopback has completed. Run T_Code */
			PC_TCode_Actions (stnID, portPtr);
			portPtr->TD_Flag = CLEAR;
		}
		break;

		/*
		 * The following signals are from the hardware PCM.
		 */
	case SIG_PCM_CODE:
		/*
		 * The PHY is in NEXT and has completed transmitting the XMIT_VECTOR
		 * specified. This signal requires determining the current bit being
		 * signalled and the actions to take based on the bits received.
		 */

		/*
		 * If software PCM is not in NEXT, then this is the first entry into
		 * NEXT.
		 */
		if (portPtr->pcState != PC_NEXT)
		{
#ifndef	XM_NO_M_PORT
			/*
			 * If it is a master port and port management is active, allow
			 * the connection to proceed only if the maximum number of ports
			 * has not been reached.
			 */
			if ((portPtr->PC_Type == PT_m)
				&& CSPDATA (stnID, stationData.maxMasterCanConnect))
			{
				if (CSPDATA (stnID, stationData.numPortsConnecting)
					< CSPDATA (stnID, stationData.maxMasterCanConnect))
				{
					++CSPDATA (stnID, stationData.numPortsConnecting);
					portPtr->isConnecting = TRUE;
				}
				else
				{
					portPtr->isWaiting = TRUE;
					Break_Actions (stnID, portPtr);
					break;
				}
			}
#endif							/* XM_NO_M_PORT */

			Next_Actions (stnID, portPtr);
		}

		/*
		 * Check for Next_Error: (n > 4 & T_Val(4) & T_Val(5) & (not WC_Flag)
		 * & Connection_Policy(PC_Type, PC_Neighbor))
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

		/*
		 * Check for early termination of LCT.
		 */
		if (portPtr->TD_Flag && (portPtr->n == 7))
		{
			/*
			 * Clear the PC_LOOP bits to mark the end of LCT.
			 * 
			 * Set LS_Flag to indicate the PCM_CODE signal can be processed.
			 * Extraneous signals will not occur if the other side terminates
			 * LCT first.
			 */
			regVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);
			regVal &= ~PC_LOOP_MASK;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, regVal);

			portPtr->LS_Flag = SET;

			/*
			 * PC(44d) LCT has been stopped before timer expired. Run T_Code
			 * as if timer expired (see above).
			 */
			PC_TCode_Actions (stnID, portPtr);
			portPtr->TD_Flag = CLEAR;
			SetCSPTimerMulti (stnID, (uTime) 0, 0, 0, &portPtr->TPC);
			break;
		}

		/*
		 * This test checks for the extraneous PCM_CODE signal.
		 */
		if ((portPtr->n == 8) && !portPtr->LS_Flag)
		{
			/*
			 * LS_Flag is not set so check the data portion.
			 */
			if (sigData == SIG_PCM_CODE)
			{
				/*
				 * This signal is the flag indicating the next PCM_CODE
				 * signal is valid.
				 */
				portPtr->LS_Flag = SET;
				break;
			}

			/*
			 * Otherwise, this is an extraneous signal so ignore it.
			 */
			break;
		}

		/*
		 * At this point, the PCM_CODE signal is valid.
		 */

		/* run R_Code */
		PC_RCode_Actions (stnID, portPtr);

		/* if transmit delay is active */
		if (portPtr->TD_Flag)
		{
#ifndef	XM_NO_M_PORT
			if (portPtr->isConnecting && (portPtr->LC_Test == LC_Extended))
			{
				/*
				 * doing extended LCT, don't connect now
				 */
				SendSignalMulti (stnID, SIG_EC_PCM_RETRY,
					sigEntity, (uInt32) 0);
			}
#endif							/* XM_NO_M_PORT */

			/* set CSP timer for LCT or loopback */
			SetCSPTimerMulti (stnID,
				((portPtr->n == 7) ? portPtr->LC_Test : portPtr->MACLoop_Time),
				PCM_SIGNALS, portPtr->ID,
				&portPtr->TPC);
		}

		break;

	case SIG_PCM_ENABLED:
		/*
		 * PC(88b) The PHY has entered ACTIVE. Signal CFM to affect the
		 * configuration changes required. Notify LEM to start monitoring the
		 * link.
		 */
		Active_Actions (stnID, portPtr);
		SendSignalMulti (stnID, SIG_CF_Join, portPtr->ID, (uInt32) SET);
		SendSignalMulti (stnID, SIG_LEM_Start, portPtr->ID, (uInt32) 0);
		break;

	case SIG_PCM_TRACE_PROP:
		/* PC(88c) or PC(22b) The PHY has received MLS in ACTIVE or TRACE */
		if (portPtr->pcState == PC_ACTIVE
			|| portPtr->pcState == PC_TRACE)
		{
			SendSignalMulti (stnID, SIG_Trace_Prop, portPtr->ID,
				(uInt32) RT_port);
		}
		break;

	case SIG_PCM_SELF_TEST:
		/* PC(22a) Signal ECM to begin the Self Test */
		if (portPtr->pcState == PC_TRACE)
			SendSignalMulti (stnID, SIG_Path_Test, 0,
				(uInt32) PT_Pending);
		break;

	default:
		break;
	}

	return;
}
