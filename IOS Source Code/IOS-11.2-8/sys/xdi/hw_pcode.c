/* $Id: hw_pcode.c,v 3.1.60.1 1996/03/22 00:16:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/hw_pcode.c,v $
 *------------------------------------------------------------------
 * hw_pcode.c - 1F MIF68840 based PCI port adaptor support
 *              ( Port adapter platforms common PCM XDI code)
 *
 * December 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hw_pcode.c,v $
 * Revision 3.1.60.1  1996/03/22  00:16:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/01/25  12:10:56  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.2.1  1995/12/23  04:42:25  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/12/19  20:36:14  fsunaval
 * Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_hw_pcode.c xm7_v3.1.1:cset.0166:10:10 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		PCM State Machine Pseudo Code
		****	FOR MOTOROLA, DEC, and AMD PHYs WITH BUILT-IN
		****	PCM SUPPORT.

		File:			xm7_hw_pcode.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the PCM state machine pseudo code listed
		in the ANSI X3T9.5 SMT document.

		The difference between this implementation of the pseudo code and
		the completely software-based PCM pseudo code is the sequence in
		which the bits are processed and the access to the chip's
		transmit vector. Because the chip has the capability to transmit
		multiple bits without interaction, the transmit vectors containing
		several bits are built. The vector transmits as much information
		as is known at the time.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	"cschdrsif.h"
#include	"xdihdrsif.h"

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

#include    "../if/if_fddi.h"

/*********************************************************************
	Hardware I/F
*********************************************************************/

static void
#ifdef __STDC__
WriteVector (uInt16 stnID, CSPPORTInfoType * portPtr, uInt16 len)
#else
WriteVector (stnID, portPtr, len)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
		uInt16           len;
#endif
/*********************************************************************
Function:	Update the number of bits transmitted and write the
			transmit vector to the chip.
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
			len	= # of bits in current vector.
Input:		Uses pcmData.
Output:		Update pcmData[].n.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           regVal;

	/* get xmit bits from T_Val */
	regVal = portPtr->T_Val >> (portPtr->n - len);

	/* set xmit vector length */
	WritePHYRegMulti (stnID, portPtr->ID, VECTOR_LENGTH_REG, len - 1);

	/* set xmit vector */
	WritePHYRegMulti (stnID, portPtr->ID, XMIT_VECTOR_REG, regVal);

	return;
}


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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           vectorLength = 0;		/* current vector length */
ProcState        pState;				/* saved interrupt state */

	/*
	 * Select transmit bit.
	 */
	switch (portPtr->n)
	{
		/*
		 * T_Val(0) Escape Bit. Reserved for future use. Set to 0.
		 * 
		 * For h/w PCM, this first vector contains bits 0, 1 and 2. Processing
		 * for these bits is done at one time and starts by clearing the
		 * R_Val and T_Val values.
		 */
	case 0:
		/* clear R_Val and T_Val */
		portPtr->T_Val = portPtr->R_Val = 0;

		/* set bit 0 */
		MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* increment vector length */
		vectorLength++;
		portPtr->n++;

		/* fall through to process bit 1 */

		/**
		 * T_Val(1) First bit for PC_Type. Bits 1 and 2 are set as follows:
		 *	PC_Type		Bit 1	Bit 2
		 *		A			0		0
		 *		B			0		1
		 *		S			1		0
		 *		M			1		1
		 **/
	case 1:
		if (portPtr->PC_Type == PT_s || portPtr->PC_Type == PT_m)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* increment vector length */
		vectorLength++;
		portPtr->n++;

		/* fall through to process bit 2 */

		/*
		 * T_Val(2) Second bit for PC_Type. See T_Val(1).
		 */
	case 2:
		if (portPtr->PC_Type == PT_b || portPtr->PC_Type == PT_m)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* increment vector length */
		vectorLength++;
		portPtr->n++;

		/* write vector */
		WriteVector (stnID, portPtr, vectorLength);
		break;

		/*
		 * T_Val(3) PHY Compatibility. Set this bit if this station allows
		 * this topology connection.
		 */
	case 3:
		/*
		 * Set bit based on topology rules.
		 */
		MSetBit (portPtr->T_Val, portPtr->n,
			Allow_Connection (stnID, portPtr));

		/* set vector length and write vector */
		vectorLength++;
		portPtr->n++;
		WriteVector (stnID, portPtr, vectorLength);
		break;

		/**
		 * T_Val(4) Bits 4 and 5 determine the duration of the Link
		 * Confidence Test using the following encoding:
		 *	Length		Bit 4	Bit 5
		 *	Short			0		0
		 *	Medium			0		1
		 *	Long			1		0
		 *	Extended		1		1
		 **/
	case 4:
		if (portPtr->PC_Withhold == PCW_none)
		{
			/* set connect state */
			portPtr->Connect_State = CS_connecting;
			SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);

			if (portPtr->LEM_Fail)
			{
				MSetBit (portPtr->T_Val, portPtr->n, SET);
				MSetBit (portPtr->T_Val, portPtr->n + 1, CLEAR);
			}

			else
			{
				MSetBit (portPtr->T_Val, portPtr->n, CLEAR);
				if (portPtr->PC_LCT_Fail > 0)
					MSetBit (portPtr->T_Val, portPtr->n + 1, SET);
				else
					MSetBit (portPtr->T_Val, portPtr->n + 1, CLEAR);
			}
		}

		else
		{
			/* set connect state */
			portPtr->Connect_State = CS_standby;
			SendCSPEvent (stnID, fddiPORTConnectState, portPtr->ID);

			MSetBit (portPtr->T_Val, portPtr->n, SET);
			MSetBit (portPtr->T_Val, portPtr->n + 1, SET);
		}


		/* set vector length and process bit 5 */
		vectorLength++;
		portPtr->n++;

		/*
		 * T_Val(5) Bit 5 is set in T_Val(4) so just update length and
		 * continue to bit 6.
		 */
	case 5:
		vectorLength++;
		portPtr->n++;

		/*
		 * T_Val(6) SET if MAC is used at this end for LCT.
		 */
	case 6:
		/*
		 * Check station policies if MAC should be used.
		 */
		if ((portPtr->connectionPolicies & CP_pc_mac_lct)
			&& CSPDATA (stnID, stationData.CF_MAC))
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* update length and write vector */
		vectorLength++;
		portPtr->n++;
		WriteVector (stnID, portPtr, vectorLength);
		break;

		/*
		 * T_Val(7) SET if this end failed the LCT.
		 */
	case 7:
		/*
		 * Turn off LCT. A direct call is made to SignalLEM() to keep PCM
		 * timer and LEM error counter and interrupts in sync. LCT stop
		 * should not have a delay in getting a signal from PCM to LEM.
		 */
		SignalLEM (stnID, SIG_LCT_Stop, portPtr->ID, (uInt32) 0);

		/* disable EB detection */
		portPtr->intrState &= ~PORT_INT_EB;
		SetPORTInterruptsMulti (stnID, portPtr->ID, portPtr->intrState);

		SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) 0);
		if (portPtr->PC_LCT_Fail > 0)
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* update length and write vector */
		vectorLength++;
		portPtr->n++;

		/*
		 * Disable interrupts while writing this vector. This gives the
		 * system time to set the STATUS_B(PCM_SIGNALING) bit in the PHY.
		 */
		MDisableCSPInterrupts (&pState);

		WriteVector (stnID, portPtr, vectorLength);

		/*
		 * Clear the TC_Flag and restore interrupts to allow interrupt
		 * handler to begin processing PCM_CODE interrupts again.
		 */
		portPtr->TC_Flag = CLEAR;
		MRestoreCSPInterrupts (&pState);

		break;

		/*
		 * T_Val(8) SET if this end will provide a MAC for the local loop.
		 */
	case 8:
		if ((portPtr->connectionPolicies & CP_pc_mac_loop)
			&& CSPDATA (stnID, stationData.CF_MAC))
			MSetBit (portPtr->T_Val, portPtr->n, SET);
		else
			MSetBit (portPtr->T_Val, portPtr->n, CLEAR);

		/* update length and write vector */
		vectorLength++;
		portPtr->n++;
		WriteVector (stnID, portPtr, vectorLength);
		break;

		/*
		 * T_Val(9) SET if this end will place a MAC on the PHY output.
		 */
	case 9:
		SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) CLEAR);

		if ((portPtr->PC_Withhold == PCW_none) && !(portPtr->WC_Flag))
		{
			/* MACOnOutput() handles modes */
			MSetBit (portPtr->T_Val, portPtr->n, MACOnOutput (stnID, portPtr));

			/* update length and write vector */
			vectorLength++;
			portPtr->n++;
			WriteVector (stnID, portPtr, vectorLength);
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
Parameters:	stnID   = index of station to use.
			portPtr = PORT's array element.
Input:		Uses pcmData.
Output:		Changes pcmData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           rcvVal;				/* Received vector */

	/*
	 * Read received vector.
	 */
	rcvVal = ReadPHYRegMulti (stnID, portPtr->ID, RCV_VECTOR_REG);

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
		/* not used */
		break;

	case 2:
		/*
		 * R_Val(2) First bit of PC_Type. No receive action.
		 */
		/* not used */
		break;

	case 3:
		/*
		 * R_Val(3) Second bit of PC_Type. Set PC_Neighbor.
		 */
		/*
		 * First three bits are transmitted and received together. No
		 * shifting of received bits needed.
		 */
		rcvVal &= 7;
		portPtr->R_Val |= rcvVal;

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
		/*
		 * Shift next bit to proper position.
		 */
		rcvVal &= 1;
		portPtr->R_Val |= (rcvVal << 3);

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
		 * 
		 * Bits 4, 5 and 6 are transmitted and received together.
		 */

	case 6:
		/*
		 * R_Val(6) Determine LCT duration.
		 */

	case 7:
		/*
		 * R_Val(7) Check if other end will use MAC for LCT.
		 */
		/*
		 * Shift next bits to proper position.
		 */
		rcvVal &= 7;
		portPtr->R_Val |= (rcvVal << 4);

		/*
		 * Process bits 4 and 5
		 */
		if (MReadBit (portPtr->T_Val, 4) || MReadBit (portPtr->R_Val, 4))
		{
			if ((MReadBit (portPtr->T_Val, 4)
					&& MReadBit (portPtr->T_Val, 5))
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

		/*
		 * Process bit 6.
		 */
		SendSignalMulti (stnID, SIG_PC_PDR, portPtr->ID, (uInt32) 0);

		/*
		 * If my end is to place a MAC on the link, then do so if MAC is not
		 * already in use.
		 */
		if (MReadBit (portPtr->T_Val, 6))
		{
			SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) SET);
		}

		else
		{
			/*
			 * Otherwise set PC_LOOP to source IDLE. Must be cleared before
			 * set.
			 */
			rcvVal = ReadPHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG);
			rcvVal &= ~PC_LOOP_MASK;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, rcvVal);
			rcvVal |= PC_LOOP_IDLE;
			WritePHYRegMulti (stnID, portPtr->ID, CNTRL_B_REG, rcvVal);
		}

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
		/*
		 * Shift next bit to proper position.
		 */
		rcvVal &= 1;
		portPtr->R_Val |= (rcvVal << 7);

		if (MReadBit (portPtr->T_Val, 7) || MReadBit (portPtr->R_Val, 7))
			SendSignalMulti (stnID, SIG_PC_Start, portPtr->ID, (uInt32) 0);
		else
			PC_TCode_Actions (stnID, portPtr);
		break;

	case 9:
		/*
		 * R_Val(9) Check for local loop.
		 */
		/*
		 * Shift next bit to proper position.
		 */
		rcvVal &= 1;
		portPtr->R_Val |= (rcvVal << 8);

		if (MReadBit (portPtr->T_Val, 8) || MReadBit (portPtr->R_Val, 8))
		{
			SendSignalMulti (stnID, SIG_PC_PDR, portPtr->ID, (uInt32) 0);

			if (MReadBit (portPtr->T_Val, 8))
			{
				SendSignalMulti (stnID, SIG_CF_Loop, portPtr->ID, (uInt32) SET);
			}
			portPtr->TD_Flag = SET;
		}

		else
			PC_TCode_Actions (stnID, portPtr);
		break;

	case 10:
		/*
		 * R_Val(10) Set topology.
		 */
		/*
		 * Shift next bit to proper position.
		 */
		rcvVal &= 1;
		portPtr->R_Val |= (rcvVal << 9);

		SendCSPEvent (stnID, fddiPORTMACIndicated, portPtr->ID);
		SendSignalMulti (stnID, SIG_PC_Join, portPtr->ID, (uInt32) 0);
		break;
	}

	return;
}
