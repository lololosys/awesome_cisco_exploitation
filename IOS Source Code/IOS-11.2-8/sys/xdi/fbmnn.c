/* $Id: fbmnn.c,v 3.2.58.1 1996/03/22 00:15:58 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/fbmnn.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmnn.c,v $
 * Revision 3.2.58.1  1996/03/22  00:15:58  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/23  04:42:22  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2  1995/11/17  18:09:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmnn.c xm7_v3.1.1:cset.0173:12:12 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Neighbor Notification Protocol Module

		File:			xm7_fbmnn.c
		Created:		09/05/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for processing the Neighbor Notification
		protocol specified by the SMT standard. Every FBM_CLOCK_PULSE
		seconds, ProcessNNTick() is executed. When the amount of time passed
		reaches T_Notify, then a NIF Request is transmitted. Request or
		Announcement frames are handled by ProcessNIFReceived(). Response
		frames get processed in ProcessNIFResponse().

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"fbmproto.h"
#include	"multistn.h"
#include    "../if/if_fddi.h"


/*********************************************************************
	FBM NN Protocol Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessNIFReceived (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessNIFReceived (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function is called from ProcessmFBMFrame() to
			handle NIF Requests and Announcements in conjunction
			with the Neighbor Notification protocol.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNum		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		May issue new frames, set MIB attributes, pass frame to
			MAP, or do nothing depending upon frame information and
			if it is part of the protocol or not.
Return:		None.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* header for received frame */
Int16            compare;				/* address compare result */
StationStateParamType *ssp;				/* station state param format */
Flag             frameError = CLEAR;	/* error found in frame */
ParamHdrType    *phdr;					/* parameter header format */
uChar           *p;						/* pointer into frame */
uInt16           len;					/* parameter length */
struct FBMMACInfo *macPtr;				/* MAC info structure */
uChar            unaDup;				/* temp flag setting */

	if (fddi_smt_packet_debug)
		buginf("\nIn ProcessNIFReceived, EAC bits = 0x%02x", EACbits);

	/*
	 * Get MAC info structure address.
	 */
	macPtr = &FBMDATA (stnID, fbmMACData[MACNum]);

	/*
	 * Get frame header.
	 */
	frameHdr = (SMTFrameHdrType *) memPtr;

	/*
	 * Check NIF_Received condition. Frame must meet criteria: FR_GOOD, Er=R,
	 * Ar=R, Cr=R, FCr=X'4F' (SMT NSA), DAr=X'FF FF FF FF FF FF' (Broadcast).
	 * FrameClassr, FrameTyper, and VersionIDr were checked by
	 * ProcessFBMFrame().
	 */
	MCompareAddress (frameHdr->macHdr.DA, SMT_Broadcast_DA, compare);
	if ((frameHdr->macHdr.FC == SMT_NSA_FC) && (compare == 0)
		&& (!(EACbits & (E_Indicator | A_Indicator | C_Indicator))))
	{

		/*
		 * Do Respond_Actions.
		 */

		/* reset TVU */
		macPtr->TVU = 0;

		/* check for new upstream neighbor */
		MCompareAddress (macPtr->UNA, frameHdr->macHdr.SA, compare);
		if (compare != 0)
		{
			/* set new UNA */
			MCopyAddress (macPtr->UNA, frameHdr->macHdr.SA);

			/* let MIB handle updates and notifications */
			SendFBMEvent (stnID, fddiMACUpstreamNbr, MACNum);
		}

		/* locate Station State parameter */
		ssp = (StationStateParamType *) FindParam (STATION_STATE_PARAM_TYPE,
			MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
			frameLen - SIZE_SMTFrameHdrType, NET_ORDER);
		if (ssp)
		{
			/* get current condition state reported */
			unaDup = (ssp->DuplAddress & DuplAddr_My_Duplicate)
				? SET : CLEAR;
			/* Change upstream neighbor DA flag if needed. */
			if (macPtr->myUNADuplicate != unaDup)
			{
				/* set new condition */
				macPtr->myUNADuplicate = unaDup;
				SendFBMEvent (stnID, fddiMACUNDA_Flag, MACNum);
			}
		}
		else
			/* assume format error if StationState not found */
			frameError = SET;
	}

	/*
	 * Respond to all request frames (as opposed to announcement frames).
	 */
	if (frameHdr->smtHdr.Frame_Type == SMTREQUEST)
	{
		/*
		 * Clear buffer for the response frame. Use RDF size in case this
		 * frame was not processed.
		 */
		MClearFrameBuffer (fbmFrameBuffer, (SIZE_RDF_HDR + frameLen));

		MCompareAddress (frameHdr->macHdr.DA, macPtr->SMTAddress, compare);

		/* If frame could not be processed, then send RDF */
		if (frameError)
		{
			/* build RDF frame */
			BuildRDFResponse (stnID, memPtr,
				fbmFrameBuffer, MACNum, RC_ERROR_UNKNOWN, frameLen);

			/* Send frame */
			SendSMTFrameMulti (stnID, fbmFrameBuffer,
				Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE, MACNum);
		}

		/*
		 * Else send response frame. Respond to all NIF Requests at this
		 * point.
		 */
		else
		{

			/* Build response frame */
			BuildNIFResponse (stnID, memPtr, fbmFrameBuffer, MACNum);

			if ((frameHdr->macHdr.FC == SMT_NSA_FC) && (compare == 0))
			{
				phdr = (ParamHdrType *) FindParam (ECHO_PARAM_TYPE,
					MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
					frameLen - SMT_FRAME_HDR_SIZE, NET_ORDER);
				if (phdr)
				{
					p = MADDPTR (uChar, fbmFrameBuffer,
						(Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
							+ SIZE_SMTFrameHdrType));
					len = Mntohs (phdr->length) + SIZE_ParamHdrType;
					MEMCOPY (p, phdr, len);
					fbmFrameHeader->smtHdr.InfoField_Length
						= Mhtons (Mntohs
						(fbmFrameHeader->smtHdr.InfoField_Length) + len);
				}
			}

			/* Send frame */
			SendSMTFrameMulti (stnID, fbmFrameBuffer,
				Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE, MACNum);
		}

	}

	/*
	 * If XM_USE_RAF is defined, then include the SBA monitoring code.
	 */
#ifdef	XM_USE_RAF
	MonitorNIFSBA (stnID, memPtr, frameLen, MACNum, EACbits);
#endif

	return;
}

void
#ifdef __STDC__
ProcessNIFResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessNIFResponse (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an NIF response frame and performs
			duplicate address detection, available communication
			detection, and downstream neighbor detection.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= total length of frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= NIF response frame received.
Output:		Performs duplicate address detection and notifies MIB if
			a duplicate address is found. Sets the MAC's downstream
			neighbor, if available.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
uInt32           transID;				/* Frame's xaction ID */
SMTFrameHdrType *frameHdr;				/* header to frame */
Int16            compare;				/* result of address compare */
struct FBMMACInfo *macPtr;				/* MAC info structure */

	if (fddi_smt_packet_debug)
		buginf("\nIn ProcessNIFResponse, EAC bits = 0x%02x", EACbits);

	/*
	 * Get MAC info structure address.
	 */
	macPtr = &FBMDATA (stnID, fbmMACData[MACNum]);

	/*
	 * Get header format.
	 */
	frameHdr = (SMTFrameHdrType *) memPtr;

	/*
	 * Check for Norm_Response or Dup_Response. First check looks for
	 * FCr=X'41', DAr=This fddiMACSMTAddress, FrameClassr, FrameTyper,
	 * VersionIDr and E indicator checked in ProcessFBMFrame().
	 */
	MCompareAddress (frameHdr->macHdr.DA, macPtr->SMTAddress, compare);
	if ((frameHdr->macHdr.FC == SMT_INFO_FC) && (compare == 0))
	{
		/* get trans ID from frame */
		transID = Mntohl (frameHdr->smtHdr.Transaction_ID);

		/*
		 * Dup_Response: If Ar = S, then this MAC address is a duplicate. Do
		 * Fail_Actions.
		 */
		if (EACbits & A_Indicator)
		{

			/* set Dup_Addr_Test if changed */
			if (macPtr->Dup_Addr_Test != DAT_fail)
			{
				macPtr->Dup_Addr_Test = DAT_fail;
				SendFBMEvent (stnID, fddiMACDupAddressTest, MACNum);
			}

			/* if transIDr == NN_Transaction_ID */
			if (transID == macPtr->NNTransactionID)
			{
				/* and SA != DNA */
				MCompareAddress (macPtr->DNA, frameHdr->macHdr.SA, compare);
				if (compare != 0)
				{
					MCopyAddress (macPtr->DNA, frameHdr->macHdr.SA);
					SendFBMEvent (stnID, fddiMACDownstreamNbr, MACNum);
				}
				/* else no change */
			}

			else
			{
				/*
				 * This frame was not sent by this NN transmitter. Send the
				 * frame to the MAP.
				 */
				PassMAPFrameMulti (stnID, FBM_EVENT_FRAME_RECEIVED, memPtr,
					frameLen, MACNum, EACbits);
			}
		}

		else
			/*
			 * Norm_Response: transIDr == NN_Transaction_ID. Do Pass_Actions.
			 */
		if (transID == macPtr->NNTransactionID)
		{
			/* set Dup_Addr_Test if changed */
			if (macPtr->Dup_Addr_Test != DAT_pass)
			{
				macPtr->Dup_Addr_Test = DAT_pass;
				SendFBMEvent (stnID, fddiMACDupAddressTest, MACNum);
			}

			/* reset TVD */
			macPtr->TVD = 0;

			/* set new transaction ID */
			macPtr->NNTransactionID = GenTransID (stnID);

			/* set DNA */
			MCompareAddress (macPtr->DNA, frameHdr->macHdr.SA, compare);
			if (compare != 0)
			{
				MCopyAddress (macPtr->DNA, frameHdr->macHdr.SA);
				SendFBMEvent (stnID, fddiMACDownstreamNbr, MACNum);
			}
		}

		else
		{
			/*
			 * This frame was not sent by this NN transmitter. Send the frame
			 * to the MAP.
			 */
			PassMAPFrameMulti (stnID, FBM_EVENT_FRAME_RECEIVED, memPtr,
				frameLen, MACNum, EACbits);
		}

		/*
		 * If XM_USE_RAF is defined, then include the SBA monitoring code.
		 */
#ifdef	XM_USE_RAF
		MonitorNIFSBA (stnID, memPtr, frameLen, MACNum, EACbits);
#endif
	}

	/*
	 * Else, no action is taken if the DA != fddiSMTMACAddress.
	 */

	return;
}

void
#ifdef __STDC__
ProcessNNTick (uInt16 stnID)
#else
ProcessNNTick (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	For every clock tick in FBM, this routine is called
			as one of the timer service routines. When T_Notify
			time has passed, the NN protocol is executed.
Parameters:	stnID		= index of station to use.
Input:		TNN, TVU and TVD.
Output:		Timers updated. UNA and DNA cleared when TVU and TVD expire,
			respectively. When TNN expires, a NN NIF is issued.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
Int16            compare;				/* address comparison result */
uInt16           len;					/* length of send frame */
struct FBMMACInfo *macPtr;				/* MAC info structure */

	/*
	 * Process all MACs.
	 */
	for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
		i < FBMDATA (stnID, fbmStationData.macCount);
		i++, macPtr++)
	{

		/*
		 * Skip unavailable MACs.
		 */
		if (!macPtr->hwPresent)
			continue;

		/*
		 * Increment timers.
		 */
		macPtr->TNN += FBM_CLOCK_TICK;
		macPtr->TVU += FBM_CLOCK_TICK;
		macPtr->TVD += FBM_CLOCK_TICK;

		/*
		 * If TNN has timed out, do Send_Actions.
		 */
		if (macPtr->TNN >= FBMDATA (stnID, fbmStationData.T_Notify))
		{

			/* if MAC is available for SMT transmission, */
			if (macPtr->SM_MA_Avail)
			{

				/* Clear frame buffer */
				MClearFrameBuffer (fbmFrameBuffer, SIZE_NIF);

				/*
				 * Send an NIF Request.
				 */
				macPtr->NNTransactionID = BuildNIFRequest (stnID,
					fbmFrameBuffer,
					SMT_NSA_FC,
					SMT_Broadcast_DA,
					i,
					macPtr->NNTransactionID);

				/* check if frame was built */
				if (macPtr->NNTransactionID != 0)
				{
					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, i);

					/* Reset TNN */
					macPtr->TNN = 0;
				}
			}
		}

		/*
		 * If TVU has timed out, clear current UNA, reset TVU.
		 */
		if (macPtr->TVU > T_NN_Out)
		{
			/* clear UNA if needed */
			MCompareAddress (macPtr->UNA, SMT_Unknown_Address, compare);
			if (compare != 0)
			{
				MCopyAddress (macPtr->UNA, SMT_Unknown_Address);
				SendFBMEvent (stnID, (uInt16) fddiMACUpstreamNbr, i);
				macPtr->TVU = 0;

				/*
				 * Clear UNDA_Flag when upstream neighbor goes away, if
				 * needed.
				 */
				if (macPtr->myUNADuplicate)
				{
					/* set new condition */
					macPtr->myUNADuplicate = CLEAR;
					SendFBMEvent (stnID, (uInt16) fddiMACUNDA_Flag, i);
				}
			}
		}

		/*
		 * If TVD has timed out, clear current DNA, reset TVD.
		 */
		if (macPtr->TVD > T_NN_Out)
		{
			/* clear DNA if needed */
			MCompareAddress (macPtr->DNA, SMT_Unknown_Address, compare);
			if (compare != 0)
			{
				MCopyAddress (macPtr->DNA, SMT_Unknown_Address);
				SendFBMEvent (stnID, (uInt16) fddiMACDownstreamNbr, i);
				macPtr->TVD = 0;
			}
		}
	}

	return;
}
