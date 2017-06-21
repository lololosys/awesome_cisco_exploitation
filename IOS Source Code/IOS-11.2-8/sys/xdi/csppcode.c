/* $Id: csppcode.c,v 3.2 1995/11/17 18:09:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csppcode.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csppcode.c,v $
 * Revision 3.2  1995/11/17  18:09:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csppcode.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		PCM State Machine Pseudo Code
		
		File:			xm7_csppcode.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the PCM state machine pseudo code listed
		in the ANSI X3T9.5 SMT document.
				
		This file consists of two functions: one for the Transmit Code
		and the other for the Receive Code. The routines for checking
		connection policies and neighbor entities are located in
		file smtmsg.c.

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
		PCM Transmit Code
*********************************************************************/

void
#ifdef __STDC__
PC_TCode_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PC_TCode_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	PCM Transmit Code.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Select transmit bit.
	 */
	switch (portPtr->n)
	{

	case 0:
		/*
		 * T_Val(0) Escape Bit. Reserved for future use. Set to 0.
		 */
		MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 1:
		/**
		 * T_Val(1) First bit for PC_Type. Bits 1 and 2 are set as follows:
		 *
		 * PC_Type		Bit 1		Bit 2
		 *	A				0			0
		 *	B				0			1
		 *	S				1			0
		 *	M				1			1
		 **/
		if (portPtr->PC_Type == PT_s || portPtr->PC_Type == PT_m)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 2:
		/*
		 * T_Val(2) Second bit for PC_Type. See T_Val(1).
		 */
		if (portPtr->PC_Type == PT_b || portPtr->PC_Type == PT_m)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 3:
		/*
		 * T_Val(3) PORT Compatibility. Set this bit if this station allows
		 * this topology connection.
		 */
		MSetBit (portPtr->T_Val, portPtr->n, Allow_Connection (stnID, portPtr));
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 4:
		/**
		 * T_Val(4) Bits 4 and 5 determine the duration of the Link
		 * Confidence Test using the following encoding:
		 *	Length		Bit 4	Bit 5
		 *	Short			0		0
		 *	Medium			0		1
		 *	Long			1		0
		 *	Extended		1		1
		 **/
		if (portPtr->PC_Withhold == PCW_none)
		{
			/* set connect state */
			if (portPtr->Connect_State != CS_connecting)
			{
				portPtr->Connect_State = CS_connecting;
				SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);
			}

			if (portPtr->LEM_Fail)
			{
				MSetBit (portPtr->T_Val, portPtr->n, SET);
				MSetBit (portPtr->T_Val, (portPtr->n + 1), CLEAR);
			}
			else
			{
				MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
				if (portPtr->PC_LCT_Fail > 0)
					MSetBit (portPtr->T_Val, (portPtr->n + 1), SET);
				else
					MSetBit (portPtr->T_Val, (portPtr->n + 1), CLEAR);
			}
		}
		else
		{
			/* set connect state */
			if (portPtr->Connect_State != CS_standby)
			{
				portPtr->Connect_State = CS_standby;
				SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);
			}

			MSetBit (portPtr->T_Val, portPtr->n, SET);
			MSetBit (portPtr->T_Val, (portPtr->n + 1), SET);
		}

		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 5:
		/*
		 * T_Val(5) Bit 5 is set in T_Val(4) so just send PC_Signal.
		 */
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 6:
		/*
		 * T_Val(6) SET if MAC is used at this end for LCT. Check station
		 * policies if MAC should be used.
		 */
		if ((portPtr->connectionPolicies & CP_pc_mac_lct)
			&& CSPDATA (stnID, stationData.CF_MAC))
		{
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		}
		else
		{
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
		}
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 7:
		/*
		 * T_Val(7) SET if this end failed the LCT.
		 */
		/*
		 * Turn off LCT. A direct call is made to SignalLEM() to keep PCM
		 * timer and LEM error counter and interrupts in sync. LCT stop
		 * should not have delay in getting a signal from PCM to LEM.
		 */
		SignalLEM (stnID, SIG_LCT_Stop, portPtr->ID, (uInt32) 0);

		/* disable EB detection */
		portPtr->intrState &= ~PORT_INT_EB;
		SetPORTInterruptsMulti (stnID, portPtr->ID,
			portPtr->intrState);

		SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) 0);
		if (portPtr->PC_LCT_Fail > 0)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 8:
		/*
		 * T_Val(8) SET if this end will provide a MAC for the local loop.
		 */
		if ((portPtr->connectionPolicies & CP_pc_mac_loop)
			&& CSPDATA (stnID, stationData.CF_MAC))
		{
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		}
		else
		{
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
		}
		SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		break;

	case 9:
		/*
		 * T_Val(9) SET if this end will place a MAC on the PORT output.
		 */
		SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

		/* disable EB detection */
		portPtr->intrState &= ~PORT_INT_EB;
		SetPORTInterruptsMulti (stnID, portPtr->ID,
			portPtr->intrState);

		if ((portPtr->PC_Withhold == PCW_none) && !(portPtr->WC_Flag))
		{
			/* MACOnOutput() handles modes */
			MSetBit (portPtr->T_Val, portPtr->n, MACOnOutput (stnID, portPtr));
			SendSignalMulti (stnID, SIG_PC_Signal, portPtr->ID, (uInt32) 0);
		}
		else
			SendSignalMulti (stnID, SIG_PC_Start, portPtr->ID, (uInt32) 0);

		break;
	}

	return;
}


/*********************************************************************
		PCM Receive Code
*********************************************************************/

void
#ifdef __STDC__
PC_RCode_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PC_RCode_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	PCM Receive Code.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Select receive bit.
	 */
	switch (portPtr->n)
	{

	case 0:
		/*
		 * R_Val(0) No receive actions, just start transmit code.
		 */
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 1:
		/*
		 * R_Val(1) Escape Bit. No receive action.
		 */
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 2:
		/*
		 * R_Val(2) First bit of PC_Type. No receive action.
		 */
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 3:
		/*
		 * R_Val(3) Second bit of PC_Type. Set PC_Neighbor.
		 */
		if (MReadBit (portPtr->R_Val, 1))
		{
			if (MReadBit (portPtr->R_Val, 2))
				portPtr->PC_Neighbor = PT_m;
			else
				portPtr->PC_Neighbor = PT_s;
		}
		else
		{
			if (MReadBit (portPtr->R_Val, 2))
				portPtr->PC_Neighbor = PT_b;
			else
				portPtr->PC_Neighbor = PT_a;
		}
		SendCSPEvent (stnID, fddiPORTNeighborType, portPtr->ID);
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 4:
		/*
		 * R_Val(4) Set PC_Withhold and PC_Mode
		 */
		if (portPtr->PC_Type == PT_m && portPtr->PC_Neighbor == PT_m)
		{
			portPtr->PC_Withhold = PCW_m_m;
		}
		else
		{
			if (MReadBit (portPtr->T_Val, 3) || MReadBit (portPtr->R_Val, 3))
			{
				if (portPtr->PC_Type == PT_m || portPtr->PC_Neighbor == PT_m)
					portPtr->PC_Mode = PC_Mode_Tree;
				else
					portPtr->PC_Mode = PC_Mode_Peer;

				/* reevaluate selection criteria */
				CFMPortSelectionCriteria (stnID, portPtr);

				if (portPtr->WC_Flag)
					portPtr->PC_Withhold = PCW_pathnotavailable;
				else
					portPtr->PC_Withhold = PCW_none;
			}
			else
			{
				portPtr->PC_Withhold = PCW_otherincompatible;
			}
		}

		SendCSPEvent (stnID, fddiPORTPC_Withhold, portPtr->ID);
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 5:
		/*
		 * R_Val(5) First bit of LCT duration. No receive action.
		 */
		PC_TCode_Actions (stnID, portPtr);
		break;

	case 6:
		/*
		 * R_Val(6) Determine LCT duration.
		 */
		if (MReadBit (portPtr->T_Val, 4) || MReadBit (portPtr->R_Val, 4))
		{
			if ((MReadBit (portPtr->T_Val, 4) && MReadBit (portPtr->T_Val, 5))
				|| (MReadBit (portPtr->R_Val, 4)
					&& MReadBit (portPtr->R_Val, 5)))
				portPtr->LC_Test = LC_Extended;
			else
				portPtr->LC_Test = LC_Long;
		}
		else
		{
			if (MReadBit (portPtr->T_Val, 5) || MReadBit (portPtr->R_Val, 5))
				portPtr->LC_Test = LC_Medium;
			else
				portPtr->LC_Test = LC_Short;
		}

		PC_TCode_Actions (stnID, portPtr);
		break;

	case 7:
		/*
		 * R_Val(7) Check if other end will use MAC for LCT.
		 */
		SendSignalMulti (stnID, SIG_PC_PDR, portPtr->ID, (uInt32) 0);

		if (MReadBit (portPtr->T_Val, 6))
			SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) SET);

		/*
		 * Turn on LCT. A direct call is made to SignalLEM() to keep PCM
		 * timer and LEM error counter and interrupts in synch. LCT start
		 * should not have delay in getting a signal from PCM to LEM.
		 */
		SignalLEM (stnID, SIG_LCT_Start, portPtr->ID, (uInt32) 0);

		/* enable EB detection */
		portPtr->intrState |= PORT_INT_EB;
		SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

		portPtr->TD_Flag = SET;
		break;

	case 8:
		/*
		 * R_Val(8) Check if LCT passed.
		 */
		if (MReadBit (portPtr->T_Val, 7) || MReadBit (portPtr->R_Val, 7))
			SendSignalMulti (stnID, SIG_PC_Start, portPtr->ID, (uInt32) 0);
		else
			PC_TCode_Actions (stnID, portPtr);
		break;

	case 9:
		/*
		 * R_Val(9) Check for local loop.
		 */
		if (MReadBit (portPtr->T_Val, 8) || MReadBit (portPtr->R_Val, 8))
		{
			SendSignalMulti (stnID, SIG_PC_PDR, portPtr->ID, (uInt32) 0);

			if (MReadBit (portPtr->T_Val, 8))
				SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) SET);

			/* enable EB detection */
			portPtr->intrState |= PORT_INT_EB;
			SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

			portPtr->TD_Flag = SET;
		}
		else
			PC_TCode_Actions (stnID, portPtr);
		break;

	case 10:
		/*
		 * R_Val(10) Set topology. SendCSPEvent() for fddiPORTMACIndicated
		 * reads R_Val(9) which was set in PC_TCode(9)_Actions.
		 */
		SendCSPEvent (stnID, fddiPORTMACIndicated, portPtr->ID);
		SendSignalMulti (stnID, SIG_PC_Join, portPtr->ID, (uInt32) 0);
		break;
	}

	return;
}
