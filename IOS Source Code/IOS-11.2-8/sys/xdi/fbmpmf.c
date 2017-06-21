/* $Id: fbmpmf.c,v 3.2 1995/11/17 18:09:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmpmf.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmpmf.c,v $
 * Revision 3.2  1995/11/17  18:09:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmpmf.c xm7_v3.1.1:cset.0166:12:12 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Build PMFs Module

		File:			xm7_fbmpmf.c
		Created:		09/04/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		Frame Based Management for SMT.  This module contains routines
		to build the Parameter Management Frames.

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
BuildPMFGetRequest (uInt16 stnID, SMTFrameHdrType * frameHdr,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, TLVParamType * srcPtr, uInt16 bufLen)
#else
BuildPMFGetRequest (stnID, frameHdr, destAddr, outputMAC,
	transID, srcPtr, bufLen)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		TLVParamType    *srcPtr;
		uInt16           bufLen;
#endif
/*********************************************************************
Function:	This function builds PMF Get Request.
Parameters:	stnID		= index of station to use.
			frameHdr	= buffer to use to build the frame
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			srcPtr		= buffer containing the MIB attribute(s) to get.
			bufLen		= len of data in srcPtr.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			srcPtr		= contains type-length-value encoding of
							the MIB attribute(s) to get.
Output:		frameHdr	= contains completed frame.
Return:		Transaction ID is returned if frame was built. Otherwise
			0 is returned.
Notes:		The AttrNetOrder() routine will convert ALL information
			in the parameter. Since this is just a Get request, the
			complete parameter may not exist and the next parameter
			may be located immediately following this partial parameter
			information. A call to AttrNetOrder() would try to convert
			parts of the next parameter. Therefore, the contents of
			buffer are copied to the frame buffer one at a time. This
			requires that buffer and frameHdr be distinct buffer areas.
Modification History:
*********************************************************************/
{
TLVParamType    *destPtr;				/* parameter format */
uInt16           frameLen;				/* Total bytes in outgoing frame */
uInt16           paramLen;				/* Length of frame parameter */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	frameLen = 0;
	result = 0;

	/*
	 * Initialize frame header.
	 */
	result = BuildSMTHeaderMulti (stnID, frameHdr,
		SMT_INFO_FC, destAddr, outputMAC, PMFGET_CLASS,
		SMTREQUEST, transID);

	/*
	 * Get the start of the info portion.
	 */
	destPtr = MADDPTR (TLVParamType, frameHdr, SIZE_SMTFrameHdrType);

	frameLen = 0;						/* total attribute length */
	while (frameLen < bufLen)
	{
		/* check for end of data in buffer */
		if (srcPtr->paramType == 0)
			break;

		/* check if parameter does not fit */
		paramLen = srcPtr->paramLen + SIZE_TLVHdrType;
		if ((frameLen + paramLen) > MAX_SMT_XMIT_LEN)
			break;

		/* copy parameter to frame */
		MEMCOPY (destPtr, srcPtr, paramLen);
		AttrNetOrder (destPtr, NET_ORDER);
		frameLen += paramLen;
		srcPtr = MADDPTR (TLVParamType, srcPtr, paramLen);
		destPtr = MADDPTR (TLVParamType, destPtr, paramLen);
	}

	/*
	 * Set frame length.
	 */
	frameHdr->smtHdr.InfoField_Length = Mhtons (frameLen);

	/*
	 * Return transaction ID.
	 */
	return (result);
}

void
#ifdef __STDC__
BuildPMFGetResponse (uInt16 stnID, SMTFrameHdrType * reqframeHdr,
	SMTFrameHdrType * rspframeHdr, uInt16 outputMAC)
#else
BuildPMFGetResponse (stnID, reqframeHdr, rspframeHdr, outputMAC)
		uInt16           stnID;
		SMTFrameHdrType *reqframeHdr;
		SMTFrameHdrType *rspframeHdr;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function builds a PMF Get Response.
Parameters:	stnID		= index of station to use.
			reqframeHdr	= pointer to PMF Get Request frame buffer.
			rspframeHdr	= pointer to buffer for response frame.
			outputMAC	= index of MAC to send frame out on.
Input:		reqframeHdr	= contains PMF Get Request Frame to respond to.
Output:		rspframeHdr	= contains the completed Response frame.
Return:		Transaction ID of frame is returned. Otherwise 0 is
			returned to indicate frame was not built.
Modification History:
*********************************************************************/
{
uInt16           frameLen;				/* total length of frame */
uInt16           reqLen;				/* infoField length of request frame */
uInt16           frameLeft;				/* space left in INFO field for attr */
uInt16           paramLen;				/* length of a parameter */
uInt32           reasonCode;			/* MIB Get result */
Void_p           memPtr;				/* pointer into SMT INFO field */
TLVParamType    *getPtr;				/* MIB attribute to get pointer */
TLVParamType    *attrPtr;				/* MIB attribute value pointer */
TLVParamType    *tempPtr;				/* MIB attribute value pointer */
TLVSetCountType *setCount;				/* set count pointer */
uInt16           attrLen;				/* total attribute length */

	/*
	 * Initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspframeHdr, SMT_INFO_FC,
		reqframeHdr->macHdr.SA, outputMAC,
		PMFGET_CLASS, SMTRESPONSE, Mntohl (reqframeHdr->smtHdr.Transaction_ID));

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, rspframeHdr, SIZE_SMTFrameHdrType);
	frameLen = 0;
	frameLeft = MAX_SMT_XMIT_LEN;

	/*
	 * Get attribute ID.
	 */
	getPtr = MADDPTR (TLVParamType, reqframeHdr, SIZE_SMTFrameHdrType);

	/*
	 * Fill in reason code as placeholder to find where MIB attribute value
	 * should go in response frame buffer.
	 */
	paramLen = ReasonCode_Param ((ReasonCodeParamType *) memPtr, (uInt32) 0);
	frameLeft -= paramLen;

	/*
	 * Get fddiSMTTimeStamp. First adjust memPtr to point to start of Time
	 * Stamp. Next, get parameter, Last, set paramLen so following code stays
	 * the same.
	 */
	tempPtr = MADDPTR (TLVParamType, memPtr, paramLen);
	tempPtr->paramType = fddiSMTTimeStamp;
	tempPtr->paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType), (Void_p) tempPtr);
	paramLen = tempPtr->paramLen + SIZE_TLVHdrType;
	frameLeft -= paramLen;

	/*
	 * Get SetCount and LastSetStationId. If SetCount cannot be retrieved,
	 * then assume Set operations are not supported. Save start location for
	 * set count buffer (current memPtr) and use first location after set
	 * count for MIB attribute buffer.
	 */
	setCount = MADDPTR (TLVSetCountType, tempPtr, paramLen);
	setCount->paramType = fddiSMTSetCount;
	setCount->paramLen = 0;
	reasonCode = FBMGetMIBAttrMulti (stnID, SIZE_TLVSetCountType,
		(Void_p) setCount);

	if (reasonCode == RC_Success)
	{
		/* Sets are supported. Now get LastSetStationId. */
		attrPtr = MADDPTR (TLVParamType, setCount, SIZE_TLVSetCountType);
		attrPtr->paramType = fddiSMTLastSetStationId;
		attrPtr->paramLen = 0;
		reasonCode = FBMGetMIBAttrMulti (stnID,
			SIZE_TLVHdrType + SIZE_SMTStationIdType,
			(Void_p) attrPtr);

		/* if LastSetStationId retrieved */
		if (reasonCode == RC_Success)
		{
			/* move pointer forward */
			attrPtr = MADDPTR (TLVParamType, attrPtr,
				SIZE_TLVHdrType + SIZE_SMTStationIdType);

			/* set frame space left */
			frameLeft -= (SIZE_TLVSetCountType + SIZE_TLVHdrType +
				SIZE_SMTStationIdType);
		}

		else
			/* don't send SetCount either */
			attrPtr = (TLVParamType *) setCount;
	}

	/*
	 * If the parametermanagement package is not supported or cannot retrieve
	 * SetCount for some reason. Set attrPtr here.
	 */
	else
		attrPtr = (TLVParamType *) setCount;

	/*
	 * At this point, attrPtr points to the place after the ReasonCode
	 * parameter if Set operations are not supported, or after the
	 * ReasonCode, SetCount and LastSetStationId parameters if Set operations
	 * are supported. Also frameLeft contains the number of bytes left in the
	 * SMT InfoField.
	 */

	/*
	 * Now, copy the request parameter(s) to the response buffer and convert
	 * them to host order. It is important to copy these parameters
	 * one-by-one. First, this insures that the response frame buffer does
	 * not get overrun. And second, the AttrNetOrder() routine will convert
	 * ALL information in the parameter. Since this is just a Get request,
	 * the complete parameter may not exist and the next parameter may be
	 * located immediately following this current parameter. The call to
	 * AttrNetOrder() could try to convert parts of the next parameter. By
	 * copying to the response buffer first, anything after this parameter is
	 * blank.
	 */
	attrLen = 0;						/* total attribute length */
	tempPtr = attrPtr;					/* destination for request attr */
	reqLen = Mntohs (reqframeHdr->smtHdr.InfoField_Length);
	while (attrLen < reqLen)
	{
		paramLen = Mntohs (getPtr->paramLen) + SIZE_TLVHdrType;
		if ((attrLen + paramLen) > frameLeft)
			/* stop here if parameter does not fit */
			break;

		MEMCOPY (tempPtr, getPtr, paramLen);
		AttrNetOrder (tempPtr, HOST_ORDER);
		attrLen += paramLen;
		getPtr = MADDPTR (TLVParamType, getPtr, paramLen);
		tempPtr = MADDPTR (TLVParamType, tempPtr, paramLen);
	}

	/*
	 * If nothing was copied, then there must be a length error: either
	 * request frame length was 0 or first parameter length was too long.
	 */
	if (attrLen == 0)
		reasonCode = RC_LengthError;

	else
	{
		/*
		 * Get MIB attribute(s).
		 */
		reasonCode = FBMMultiGetMIBAttrMulti (stnID, frameLeft,
			(Void_p) attrPtr, (TLVSetCountType *) NULL);
	}

	/*
	 * Fill in actual reason code.
	 */
	paramLen = ReasonCode_Param ((ReasonCodeParamType *) memPtr, reasonCode);
	frameLen += paramLen;

	/*
	 * If NoParameter is returned, then some parameters may have been
	 * processed.
	 */
	if ((reasonCode != RC_Success) && (reasonCode != RC_NoParameter))
	{
		/*
		 * Clear first parameter in list to mark end of frame's params.
		 */
		attrPtr->paramType = 0;
		attrPtr->paramLen = 0;
	}

	/*
	 * Convert all attributes starting with SetCount parameter. This code
	 * loops through all attributes in the frame and relies on the fact that
	 * the frame contents are cleared to 0 at the start of this routine and
	 * the FBMMultiGetMIBAttrMulti() routine only uses the buffer space
	 * required. By reaching the end of the buffer or finding an attribute ID
	 * of 0, the loop knows when to stop.
	 */
	attrPtr = MADDPTR (TLVParamType, memPtr, paramLen);
	attrLen = 0;
	while (attrPtr->paramType && (attrLen < frameLeft))
	{
		paramLen = Attribute_Param (attrPtr, attrPtr);
		frameLen += paramLen;
		attrLen += paramLen;
		attrPtr = MADDPTR (TLVParamType, attrPtr, paramLen);
	}

	/*
	 * Set info length.
	 */
	rspframeHdr->smtHdr.InfoField_Length = Mhtons (frameLen);

	return;
}

uInt32
#ifdef __STDC__
BuildPMFSetRequest (uInt16 stnID, SMTFrameHdrType * frameHdr,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, TLVSetCountType * setCount,
	TLVParamType * param)
#else
BuildPMFSetRequest (stnID, frameHdr, destAddr, outputMAC,
	transID, setCount, param)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		TLVSetCountType *setCount;
		TLVParamType    *param;
#endif
/*********************************************************************
Function:	This function builds a PMF Set Request.
Parameters:	stnID		= index of station to use.
			frameHdr	= buffer to use to build the frame
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			setCount	= buffer containing a Set Count parameter.
			param		= buffer containing the MIB attribute to set.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			setCount	= contains TLV encoding of the MIB
							Set Count attribute to use for the protocol.
			param		= contains TLV encoding of the MIB
							attribute to change.
Output:		frameHdr	= contains completed frame.
Return:		Transaction ID is returned if frame was built. Otherwise
		0 is returned.
Modification History:
*********************************************************************/
{
Void_p           memPtr;				/* Buffer used to build frame */
uInt16           frameLen;				/* Total bytes in outgoing frame */
uInt16           paramLen;				/* Length of frame parameter */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	frameLen = 0;
	result = 0;

	/*
	 * Initialize frame header.
	 */
	result = BuildSMTHeaderMulti (stnID, frameHdr, SMT_INFO_FC, destAddr,
		outputMAC, PMFSET_CLASS, SMTREQUEST, transID);

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, frameHdr, SIZE_SMTFrameHdrType);

	/*
	 * Set Set Count.
	 */
	paramLen = Attribute_Param ((TLVParamType *) memPtr,
		(TLVParamType *) setCount);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Set attribute.
	 */
	paramLen = Attribute_Param ((TLVParamType *) memPtr, param);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Set authorization.
	 */
	paramLen = Authorization_ParamMulti (stnID,
		(AuthorizationParamType *) memPtr,
		frameHdr, MAX_SMT_XMIT_LEN - frameLen, outputMAC);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Set frame length.
	 */
	frameHdr->smtHdr.InfoField_Length = Mhtons (frameLen);

	/*
	 * Return transaction ID.
	 */
	return (result);
}

void
#ifdef __STDC__
BuildPMFSetResponse (uInt16 stnID, SMTFrameHdrType * rspframeHdr,
	uInt16 outputMAC, FDDILongAddressType destAddr,
	uInt32 reasonCode, TLVParamType * setCount,
	TLVParamType * lastSetID, TLVParamType * param,
	uInt32 transID)
#else
BuildPMFSetResponse (stnID, rspframeHdr, outputMAC, destAddr,
	reasonCode, setCount, lastSetID, param, transID)
		uInt16           stnID;
		SMTFrameHdrType *rspframeHdr;
		uInt16           outputMAC;
		FDDILongAddressType destAddr;
		uInt32           reasonCode;
		TLVParamType    *setCount;
		TLVParamType    *lastSetID;
		TLVParamType    *param;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function builds a PMF Set Response. The parameter
			interface does not include the request frame because
			most of the response processing is done by
			ProcessPMFSet() which lays out the response frame. This
			routine basically fills in the header.
Parameters:	stnID		= index of station to use.
			rspframeHdr	= pointer to buffer for response frame.
			outputMAC	= index of MAC to send frame out on.
			destAddr	= buffer containing destination address.
			setCount	= buffer containing Set Count.
			lastSetID	= buffer containing Last Set Station ID
			param		= buffer containing MIB attribute.
			transID		= transaction ID to use in frame
Input:		destAddr	= 48-bit station ID of destination.
			setCount	= contains the TLV encoded value of the Set
							Count attribute when the MIB attribute was
							changed.  This parameter may be a NULL if
							reasonCode is not Success and not Bad Set
							Count. Otherwise this buffer is located in
							the response frame.
			lastSetID	= contains the TLV encoded address of the
							fddiSMTLastSetStationId. This parameter may
							be a NULL if reasonCode is not Success and
							not Bad Set Count. This buffer is located
							in the response frame.
			param		= contains the TLV encoded MIB attribute.
							This parameter may be a NULL if reasonCode
							is not Success and not Bad Set Count.
							This buffer is located in the response frame.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
Output:		rspframeHdr	= contains the completed frame.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           frameLen;
uInt16           paramLen;
Void_p           memPtr;

	/*
	 * Initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspframeHdr, SMT_INFO_FC,
		destAddr, outputMAC, PMFSET_CLASS, SMTRESPONSE, transID);

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, rspframeHdr, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/* set reason code (always first parameter in this code) */
	paramLen = ReasonCode_Param ((ReasonCodeParamType *) memPtr, reasonCode);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * The time stamp is always next. It is set by ProcessPMFSet() so just
	 * convert it to network order here.
	 */
	paramLen = Attribute_Param ((TLVParamType *) memPtr,
		(TLVParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Set count and last station id always included in response.
	 */
	paramLen = Attribute_Param ((TLVParamType *) memPtr, setCount);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * If not successful, then determine actual last ID.
	 */
	if (reasonCode != RC_Success)
	{
		FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType), (Void_p) lastSetID);
	}

	/* put in last station id */
	paramLen = Attribute_Param ((TLVParamType *) memPtr, lastSetID);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Include attribute information if not an action.
	 */
	switch (param->paramType)
	{
	case fddiSMTStationAction:
	case fddiSMTVendorAction:
	case fddiMACVendorAction:
	case fddiPATHVendorAction:
	case fddiPORTAction:
	case fddiPORTVendorAction:
	case 0:							/* no parameter found */
		/* skip it */
		break;

	default:
		/* put in attribute */
		paramLen = Attribute_Param ((TLVParamType *) memPtr, param);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);
	}

	/*
	 * Set info length.
	 */
	rspframeHdr->smtHdr.InfoField_Length = Mhtons (frameLen);

	return;
}
