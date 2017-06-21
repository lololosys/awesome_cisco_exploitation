/* $Id: fbmpmfp.c,v 3.2 1995/11/17 18:09:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmpmfp.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmpmfp.c,v $
 * Revision 3.2  1995/11/17  18:09:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmpmfp.c xm7_v3.1.1:cset.0166:11:11 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		PMF Protocol Processing Module

		File:			xm7_fbmpmfp.c
		Created:		09/06/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		Frame Based Management for SMT.  This module contains routines
		to process the Parameter Management Frames.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"smtmsg.h"
#include	"fbmproto.h"
#include	"multistn.h"



/*********************************************************************
	Parameter Management Frames
*********************************************************************/

uInt32
#ifdef __STDC__
ValidateAuthorization (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 macID)
#else
ValidateAuthorization (stnID, memPtr, frameLen, macID)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           macID;
#endif
/*********************************************************************
Function:	This function finds the authorization parameter in
			the frame and verifies its value.
Parameters:	stnID	= index of station to use.
			memPtr	= buffer of received frame (including PAD).
			uInt16	= frame length in bytes (excluding PAD).
			macID	= index of receiving MAC.
Input:		memPtr	= contains received frame.
Output:		None.
Return:		Reason code:
			RC_Success
				authorization valid or no authorization necessary
			RC_NotAuthorized
				authorization invalid or authorization required and
				not found in frame
Modification History:
*********************************************************************/
{
AuthorizationParamType *authorize;		/* authorization format */
uInt32           result;				/* return value */

	if (FBMDATA (stnID, fbmStationData.useAuthorization))
	{
		authorize = (AuthorizationParamType *)
			FindParam (AUTHORIZATION_PARAM_TYPE,
			MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
			frameLen - SMT_FRAME_HDR_SIZE,
			NET_ORDER);

		if (!authorize)
			result = RC_NotAuthorized;

		else
			result = ((VerifyAuthorizationMulti (stnID,
						authorize->Authorization,
						Mntohs (authorize->Length),
						memPtr, frameLen, macID))
				? RC_Success : RC_NotAuthorized);
	}

	else
		/* no authorization required */
		result = RC_Success;

	return (result);
}

void
#ifdef __STDC__
ProcessPMFGet (uInt16 stnID, Void_p memPtr, uInt16 MACNum)
#else
ProcessPMFGet (stnID, memPtr, MACNum)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           MACNum;
#endif
/*********************************************************************
Function:	This function handles the protocol processing for
			a received PMF Get frame.
Parameters:	stnID	= index of station to use.
			memPtr	= buffer of received frame.
			MACNum	= index of MAC that received the frame.
Input:		memPtr	= contains received frame.
Output:		Generates appropriate response frame.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           len;					/* total frame length */

	/*
	 * Clear frame buffer. Since it cannot be determined how long the
	 * response frame will be, clear the entire buffer.
	 */
	MClearFrameBuffer (fbmFrameBuffer, MAX_FRAME_SIZE);

	/* build response frame */
	BuildPMFGetResponse (stnID, (SMTFrameHdrType *) memPtr,
		fbmFrameHeader, MACNum);

	/* send frame */
	len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
		+ SMT_FRAME_HDR_SIZE;
	SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

	return;
}

void
#ifdef __STDC__
ProcessPMFSet (uInt16 stnID, Void_p memPtr, uInt16 frameLen, uInt16 MACNum)
#else
ProcessPMFSet (stnID, memPtr, frameLen, MACNum)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
#endif
/*********************************************************************
Function:	This function handles the protocol processing for
			a received PMF Set frame.
Parameters:	stnID		= index of station to use.
			memPtr		= buffer of received frame.
			frameLen	= length of frame.
			MACNum		= index of MAC that received the frame.
Input:		memPtr		= contains received frame.
Output:		Generates appropriate response.
Return:		None.
Modification History:
*********************************************************************/
{
TLVSetCountType *inSetCount,			/* input set count buffer */
                *outSetCount;			/* output set count */
TLVParamType    *lastID;				/* last set station ID buffer */
TLVParamType    *inData,				/* request MIB attribute */
                *outData;				/* response MIB attribute */
Void_p           inptr;					/* general pointer into req frame */
Void_p           outptr;				/* general pointer into resp frame */
uInt16           plen;					/* parameter length */
uInt16           len;					/* total length in progress */
uInt16           infoLen;				/* InfoField length */
SMTFrameHdrType *reqHdr;				/* request frame header */
ParamHdrType    *pHdr;					/* frame parameter header */
uInt32           reasonCode;			/* result of change op */

	/**
	 * WRF
	 * Initialize variables to avoid compiler warnings.
	 **/
	reqHdr = NULL;

	/*
	 * Clear frame buffer. Since it cannot be determined how long the
	 * response frame will be, clear the entire buffer.
	 */
	MClearFrameBuffer (fbmFrameBuffer, MAX_FRAME_SIZE);

	/*
	 * Init frame info and pointers.
	 */
	outptr = MADDPTR (Void_t, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	len = 0;
	infoLen = 0;

	/*
	 * Start with required parameters: ReasonCode, TimeStamp, SetCount and
	 * LastSetStationID. ReasonCode and TimeStamp must be first to remain
	 * compatible with BuildPMFSetResponse().
	 */
	/* reserve space for reason code */
	len = ReasonCode_Param ((ReasonCodeParamType *) outptr, (uInt32) 0);
	infoLen += len;
	outptr = MADDPTR (Void_t, outptr, len);

	/* get current time stamp */
	outData = (TLVParamType *) outptr;
	outData->paramType = fddiSMTTimeStamp;
	outData->paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType), (Void_p) outData);
	len = outData->paramLen + SIZE_TLVHdrType;
	infoLen += len;
	outptr = MADDPTR (Void_t, outptr, len);

	/* check for set count */
	outSetCount = (TLVSetCountType *) outptr;
	outSetCount->paramType = fddiSMTSetCount;
	outSetCount->paramLen = 0;
	reasonCode = FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType),
		(Void_p) outSetCount);
	if (reasonCode == RC_Success)
	{
		/*
		 * Parameter mgmt package is supported. Continue with Set operation
		 * on MIB.
		 */
		len = outSetCount->paramLen + SIZE_TLVHdrType;
		infoLen += len;
		outptr = MADDPTR (Void_t, outptr, len);

		/* reserve response last station ID */
		lastID = (TLVParamType *) outptr;
		lastID->paramType = fddiSMTLastSetStationId;
		lastID->paramLen = 0;
		len = SIZE_TLVIdType + SIZE_TLVHdrType;
		infoLen += len;
		outptr = MADDPTR (Void_t, outptr, len);

		/* MIB data buffer */
		outData = (TLVParamType *) outptr;

		/*
		 * Locate request attribute and set count.
		 */
		reqHdr = (SMTFrameHdrType *) memPtr;
		inptr = MADDPTR (Void_t, memPtr, SIZE_SMTFrameHdrType);
		inData = NULL;
		inSetCount = NULL;
		len = 0;
		while (len < Mntohs (reqHdr->smtHdr.InfoField_Length))
		{
			pHdr = (ParamHdrType *) inptr;
			switch (Mntohs (pHdr->type))
			{
			case fddiSMTSetCount:
				inSetCount = (TLVSetCountType *) inptr;
				break;

			default:
				/* verify this is a MIB attribute */
				if (Mntohs (pHdr->type) > fddiSMT)
					inData = (TLVParamType *) inptr;
				break;
			}

			/* parameter length is length plus header */
			plen = Mntohs (pHdr->length) + SIZE_ParamHdrType;
			inptr = MADDPTR (Void_t, inptr, plen);
			len += plen;
		}

		/*
		 * Check authorization. SetCount is optional. If it is required, then
		 * the authorization routine will invalidate request.
		 */
		if (inData == NULL)
			/* no parameter to set */
			reasonCode = RC_IllegalOperation;
		else
		{
			/* copy attribute and put into host byte order */
			MEMCOPY (outData, inData,
				Mntohs (inData->paramLen) + SIZE_TLVHdrType);
			AttrNetOrder (outData, HOST_ORDER);

			/* determine if requestor is authorized */
			reasonCode = ValidateAuthorization (stnID, memPtr,
				frameLen, MACNum);
		}

		if (reasonCode == RC_Success)
		{
			/* if set count exists */
			if (inSetCount != NULL)
			{
				/* copy set count and put into host byte order */
				MEMCOPY (outSetCount, inSetCount, SIZE_TLVSetCountType);
				AttrNetOrder ((TLVParamType *) outSetCount, HOST_ORDER);
			}

			/* copy last set station id (already in host byte order) */
			lastID->paramType = fddiSMTLastSetStationId;
			lastID->paramLen = SIZE_TLVIdType;
			MEMCOPY ((lastID->SMTOTHER), &reqHdr->smtHdr.Station_ID,
				SIZE_SMTStationIdType);

			/* request Set using response frame as buffer */
			reasonCode = FBMChangeMIBAttrMulti (stnID,
				MAX_SMT_XMIT_LEN - infoLen,
				(Void_p) outData, outSetCount,
				(SMTStationIdType *) & lastID->SMTID);
		}

		/*
		 * Complete frame building process.
		 */
		BuildPMFSetResponse (stnID, fbmFrameHeader, MACNum,
			reqHdr->macHdr.SA, reasonCode, (TLVParamType *) outSetCount,
			lastID, outData, Mntohl (reqHdr->smtHdr.Transaction_ID));
	}

	else
	{
		/*
		 * Parameter management package is not supported, therefore this
		 * frame class is not supported.
		 */
		MClearFrameBuffer (fbmFrameBuffer, (SIZE_RDF_HDR
				+ Mntohs (reqHdr->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE));
		BuildRDFResponse (stnID, memPtr, fbmFrameBuffer, MACNum,
			RC_FrameClassNotSupported,
			Mntohs (reqHdr->smtHdr.InfoField_Length) + SMT_FRAME_HDR_SIZE);
	}

	/*
	 * Send frame.
	 */
	len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
		+ SMT_FRAME_HDR_SIZE;
	SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

	return;
}
