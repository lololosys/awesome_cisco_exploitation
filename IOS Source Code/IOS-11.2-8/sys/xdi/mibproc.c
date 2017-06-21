/* $Id: mibproc.c,v 3.2.60.1 1996/03/18 22:51:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/mibproc.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibproc.c,v $
 * Revision 3.2.60.1  1996/03/18  22:51:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.1  1996/03/07  11:20:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:10:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibproc.c xm7_v3.1.1:cset.0166:15:15 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB maintenance module.

		File:			xm7_mibproc.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module provides the basic interface to the MIB maintenance
		routines. There are two MIB operations: Get and Set. The Set
		operation has two varieties: Change and Assert. A Change
		oepration requires a valid SetCount, whereas an Assert performs
		the operation when no SetCount is present.

		The Get operation has three varieties for this implementation.
		The MultiGetMIBAttrMulti() routine performs the Get operation
		as defined by the standard. It supports multiple attributes,
		group attributes, horizontal requests and any combination of
		these. The GetMIBAttrMulti() routine performs the older 6.2
		Get operation for single attributes or groups. It also supports
		horizontal requests on a single attributes. This routine is
		provided for backward compatibility with older MAPs. The
		GetXMMIBAttrMulti() routine is a special entry point used by
		FSP and CSP for rapid access to MIB information. It skips the
		buffer preprocessing and assumes one attribute only in the
		request. For MAC, PATH and PORT objects, the resource ID is
		taken to be the array index in the MIB table for the object. This
		avoids a search to convert the resource index into an array index.
		This routine does not support multiple attriubtes, groups or
		horizontal requests.

		In this implementation, the ChangeMIBAttrMulti() routine performs
		both a Change and Assert operation. If a SetCount is passed to
		the routine, a Change operation is performed. This includes
		validating the SetCount, changing the attribute, and, if the
		change was successful, incrementing the SetCount and setting
		the LastSetStationID. If no SetCount is passed to the routine,
		all of the above actions are done except for the validation
		of the SetCount.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_VSO			include code for vendor-specific objects.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
	MIB Attribute Groups
*********************************************************************/

MCONST
static           uInt16
                 SMTStationIdGrpList[] =
{
	fddiSMTStationId, fddiSMTOpVersionId,
	fddiSMTHiVersionId, fddiSMTLoVersionId,
	fddiSMTManufacturerData, fddiSMTUserData,
	fddiSMTMIBVersionId,
	0},

                 SMTStationConfigGrpList[] =
{
	fddiSMTMac_Ct, fddiSMTNonMaster_Ct,
	fddiSMTMaster_Ct, fddiSMTAvailablePaths,
	fddiSMTConfigCapabilities, fddiSMTConfigPolicy,
	fddiSMTConnectionPolicy, fddiSMTT_Notify,
	fddiSMTStatRptPolicy, fddiSMTTrace_MaxExpiration,
	fddiSMTPORTIndexes, fddiSMTMACIndexes,
	fddiSMTBypassPresent,
	0},

                 SMTStatusGrpList[] =
{
	fddiSMTECMState, fddiSMTCF_State,
	fddiSMTRemoteDisconnectFlag,
	fddiSMTStationStatus, fddiSMTPeerWrapFlag,
	0},

                 SMTMIBOperationGrpList[] =
{
	fddiSMTTimeStamp, fddiSMTTransitionTimeStamp,
	fddiSMTSetCount, fddiSMTLastSetStationId,
	0},

                 MACCapabilitiesGrpList[] =
{
	fddiMACFrameStatusFunctions, fddiMACBridgeFunctions,
	fddiMACT_MaxCapability, fddiMACTVXCapability,
	0},

                 MACConfigGrpList[] =
{
	fddiMACAvailablePaths, fddiMACCurrentPath,
	fddiMACUpstreamNbr, fddiMACDownstreamNbr,
	fddiMACOldUpstreamNbr, fddiMACOldDownstreamNbr,
	fddiMACDupAddressTest, fddiMACRequestedPaths,
	fddiMACDownstreamPORTType, fddiMACIndex,
	0},

                 MACAddressGrpList[] =
{
	fddiMACSMTAddress, fddiMACLongGrpAddress,
	fddiMACShortGrpAddress,
	0},

                 MACOperationGrpList[] =
{
	fddiMACT_Req, fddiMACT_Neg,
	fddiMACT_Max, fddiMACTvxValue,
	fddiMACT_Pri0, fddiMACT_Pri1,
	fddiMACT_Pri2, fddiMACT_Pri3,
	fddiMACT_Pri4, fddiMACT_Pri5,
	fddiMACT_Pri6,
	0},

                 MACCountersGrpList[] =
{
	fddiMACFrame_Ct, fddiMACCopied_Ct,
	fddiMACTransmit_Ct, fddiMACToken_Ct,
	fddiMACError_Ct, fddiMACLost_Ct,
	fddiMACTvxExpired_Ct, fddiMACNotCopied_Ct,
	fddiMACLate_Ct, fddiMACRingOp_Ct,
	0},

                 MACFrameErrorConditionGrpList[] =
{
	fddiMACFrameErrorThreshold, fddiMACFrameErrorRatio,
	0},

                 MACNotCopiedConditionGrpList[] =
{
	fddiMACNotCopiedThreshold, fddiMACNotCopiedRatio,
	0},

                 MACStatusGrpList[] =
{
	fddiMACRMTState, fddiMACDA_Flag,
	fddiMACUNDA_Flag, fddiMACFrameErrorFlag,
	fddiMACNotCopiedFlag, fddiMACMA_UnitdataAvailable,
	fddiMACHardwarePresent, fddiMACMA_UnitdataEnable,
	0},

                 PATHConfigGrpList[] =
{
	fddiPATHIndex, fddiPATHRingLatency,
	fddiPATHTraceStatus, fddiPATHSbaPayload,
	fddiPATHSbaOverhead, fddiPATHConfiguration,
	fddiPATHT_Rmode, fddiPATHSbaAvailable,
	fddiPATHTVXLowerBound, fddiPATHT_MaxLowerBound,
	fddiPATHMaxT_Req,
	0},

                 PORTConfigGrpList[] =
{
	fddiPORTMy_Type, fddiPORTNeighborType,
	fddiPORTConnectionPolicies, fddiPORTMACIndicated,
	fddiPORTCurrentPath, fddiPORTRequestedPaths,
	fddiPORTMACPlacement, fddiPORTAvailablePaths,
	fddiPORTMACLoop_Time, fddiPORTPMDClass,
	fddiPORTConnectionCapabilities, fddiPORTIndex,
	0},

                 PORTOperationGrpList[] =
{
	fddiPORTMaint_LS, fddiPORTBS_Flag,
	fddiPORTPC_LS,
	0},

                 PORTErrorCtrsGrpList[] =
{
	fddiPORTEBError_Ct, fddiPORTLCTFail_Ct,
	0},

                 PORTLerGrpList[] =
{
	fddiPORTLer_Estimate, fddiPORTLem_Reject_Ct,
	fddiPORTLem_Ct, fddiPORTLer_Cutoff,
	fddiPORTLer_Alarm,
	0},

                 PORTStatusGrpList[] =
{
	fddiPORTConnectState, fddiPORTPCMState,
	fddiPORTPC_Withhold, fddiPORTLerFlag,
	fddiPORTHardwarePresent,
	0};


/*********************************************************************
	Local function definitions.
*********************************************************************/

static           uInt32
#ifdef __STDC__
MakeRequestBuffer (Void_p bufData, uInt16 bufSize)
#else
MakeRequestBuffer (bufData, bufSize)
		Void_p           bufData;
		uInt16           bufSize;
#endif
/*********************************************************************
Function:	This function validates the contents of the request
			buffer and copies its contents to the temporary buffer
			space.
Parameters:	bufData		= address of request buffer
			bufSize		= maximum length of request buffer
Input:		bufData		= contains MIB attribute(s) to get in TLV
Output:		mibBuf		= contains copy of requested attributes
Return:		Reason code indicating result of test:
			RC_Success
				Contents of buffer are OK so far.
			RC_LengthError
				An invalid parameter length has been found.
Notes:		To support multiple Get operations in a single request,
			the length of the TLV encoded requests must be less than
			the length given by bufSize. Also, the contents of the
			buffer following the requested attributes must be set
			to 0.
Modification History:
*********************************************************************/
{
uInt16           plen,					/* parameter length */
                 len;					/* length counter */
TLVHdrType      *param;					/* parameter ptr */
uInt32           result = RC_Success;	/* return value */

	/*
	 * Skip through parameters and verify that the InfoField length ends
	 * where a parameter ends.
	 */
	param = (TLVHdrType *) (bufData);
	len = 0;
	plen = 0;

	/*
	 * Validate and copy attributes. All parameter lengths must be a multiple
	 * of 4.
	 */
	while (param->paramType)
	{
		/*
		 * Validate length. If length EXACTLY reaches then end of the buffer,
		 * then assume this was the last parameter. If length exceeds the
		 * buffer size, then there was a format problem in the request
		 * buffer.
		 */
		if (len > bufSize)
		{
			result = RC_LengthError;
			break;
		}

		else if (len == bufSize)
		{
			/*
			 * This loop has now exceeded the end of the request buffer. Any
			 * valid information at this point is questionable. It is quite
			 * possible that the request buffer was made to exactly fit the
			 * expected response. At this point, just end the request (last
			 * parameter is marked outside of loop below).
			 */
			break;
		}

		/* get attribute length */
		plen = param->paramLen + SIZE_TLVHdrType;

		/* validate length */
		if (plen & 3)
		{
			result = RC_LengthError;
			break;
		}

		/* validate minimum length */
		switch (param->paramType & MIB_OBJECT_MASK)
		{
		case fddiSMT:
			if (param->paramType == fddiSMTVendorAttrib)
			{
				/* min size must include vendor ID */
				if (param->paramLen < SIZE_DEFAULT)
					result = RC_LengthError;
			}
			/* all other SMT attributes have a 0 min length */
			break;

		case fddiMAC:
		case fddiPATH:
		case fddiPORT:
			if ((param->paramType == fddiMACVendorAttrib)
				|| (param->paramType == fddiPATHVendorAttrib)
				|| (param->paramType == fddiPORTVendorAttrib))
			{
				/* min size must include resource index + vendor ID */
				if (param->paramLen < (2 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			else
			{
				/* min size must include resource index */
				if (param->paramLen < SIZE_DEFAULT)
					result = RC_LengthError;
			}
			break;

#ifdef	XM_USE_VSO

		case fddiVSO:
			/* min size must include vendor ID */
			if (param->paramLen < SIZE_DEFAULT)
				result = RC_LengthError;
			break;

#endif							/* XM_USE_VSO */

		default:
			/*
			 * Just skip this parameter and let the Get routines process this
			 * and anything else in the buffer.
			 */
			break;
		}

		/* check result of min length tests */
		if (result != RC_Success)
			break;

		/* check maximum temp buffer size */
		if ((len + plen) > mibBufferLen)
		{
			result = RC_BUFFER_ERROR;
			break;
		}

		/* copy attribute */
		MEMCOPY (((uChar *) mibBufferPtr) + len, (uChar *) param, plen);

		/* increment length */
		len += plen;

		/* get next attribute */
		param = MADDPTR (TLVHdrType, param, plen);
	}

	/*
	 * Mark end of parameter list.
	 */
	if (result == RC_Success)
	{
		param = MADDPTR (TLVHdrType, mibBufferPtr, len);
		param->paramType = 0;
	}

	return (result);
}


/*********************************************************************
	MIB Access Routines
*********************************************************************/

uInt32
#ifdef __STDC__
MultiGetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount)
#else
MultiGetMIBAttrMulti (stnID, bufSize, bufData, setCount)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
#endif
/*********************************************************************
Function:	MIB access routine to get MIB attribute. This routine
			performs that actual Get operation according to SMT Rev 7.x
			PMF protocol. Multiple Gets within one request are accepted
			(see Notes below).
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			setCount	= pointer to buffer to hold current value
							of the fddiSMTSetCountAttribute
							(may be NULL if current SetCount
							is not required).
Input:		bufData		= contains the requested attribute ID and
							object index (if required) using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			setCount	= if not NULL, this buffer gets set to the
							current value of fddiSMTSetCount,
							also using the MIB TLV encoding.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported
			RC_IllegalOperation
				illegal parameter requested
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute or attribute group. In this
				case, what has been processed will be returned
				in the buffer.
Notes:	The bufData buffer must have at least four zeroes (0) following the
		last attribute to Get or the value of bufSize must match the sum of
		the attribute lengths given in the buffer. This is the only way that
		this routine knows where the end of the request is located.

		For the attributes that consist of lists of items this routine
		returns as many items in the group as will fit in the buffer.
Modification History:
*********************************************************************/
{
TLVHdrType      *tempParam;				/* Parameter header in temp buffer */
TLVHdrType      *resParam;				/* Parameter header in response buf */
uInt16           bufLeft;				/* remaining space in buffer */
uInt16           bufUsed;				/* buffer space used */
uInt16           attrLen;				/* space used by attribute(s) */
uInt16           attrCount;				/* number of attributes retrieved */
uInt32           result;				/* function return value */
Flag             noParamFlag;			/* set if NoParam returned */

	/*
	 * Initialize no parameter flag.
	 */
	noParamFlag = CLEAR;
	attrCount = 0;

	/*
	 * Validate and copy contents of request buffer.
	 */
	result = MakeRequestBuffer (bufData, bufSize);
	if (result == RC_Success)
	{
		/*
		 * Process each attribute in the temporary buffer. The attributes get
		 * copied back to the original buffer because the requester may not
		 * have left space for the value field (the value field is optional
		 * in a request). The end of the request is the parameter type of
		 * zero. This always exists because MakeRequestBuffer() puts it
		 * there.
		 */
		tempParam = (TLVHdrType *) mibBufferPtr;
		bufLeft = bufSize;
		bufUsed = 0;

		/* clear response buffer */
		MEMZERO ((uChar *) bufData, bufSize);

		while (tempParam->paramType)
		{
			/* get response buffer position */
			resParam = MADDPTR (TLVHdrType, bufData, bufUsed);

			/*
			 * If there is not enough space to write the parameter, then
			 * exit.
			 */
			if (bufLeft < (tempParam->paramLen + SIZE_TLVHdrType))
			{
				result = RC_BUFFER_ERROR;
				break;
			}

			/* copy attribute to response buffer */
			MEMCOPY ((uChar *) resParam, (uChar *) tempParam,
				tempParam->paramLen + SIZE_TLVHdrType);

			/* get next attribute */
			tempParam = MADDPTR (TLVHdrType, tempParam,
				(tempParam->paramLen + SIZE_TLVHdrType));

			/*
			 * Select proper processing routine. Groups are checked first,
			 * then vendor specific attributes, then individual attributes.
			 */
			switch (resParam->paramType)
			{
			case fddiSMTStationIdGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					SMTStationIdGrpList, &attrLen);
				break;

			case fddiSMTStationConfigGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					SMTStationConfigGrpList, &attrLen);
				break;

			case fddiSMTStatusGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					SMTStatusGrpList, &attrLen);
				break;


			case fddiSMTMIBOperationGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					SMTMIBOperationGrpList, &attrLen);
				break;

			case fddiMACFrameErrorConditionGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACFrameErrorConditionGrpList, &attrLen);
				break;

			case fddiMACStatusGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACStatusGrpList, &attrLen);
				break;

			case fddiMACConfigGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACConfigGrpList, &attrLen);
				break;

			case fddiMACNotCopiedConditionGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACNotCopiedConditionGrpList, &attrLen);
				break;

			case fddiMACCapabilitiesGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACCapabilitiesGrpList, &attrLen);
				break;

			case fddiMACAddressGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACAddressGrpList, &attrLen);
				break;

			case fddiMACOperationGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACOperationGrpList, &attrLen);
				break;

			case fddiMACCountersGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					MACCountersGrpList, &attrLen);
				break;

			case fddiPATHConfigGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PATHConfigGrpList, &attrLen);
				break;

			case fddiPORTConfigGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PORTConfigGrpList, &attrLen);
				break;

			case fddiPORTStatusGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PORTStatusGrpList, &attrLen);
				break;

			case fddiPORTLerGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PORTLerGrpList, &attrLen);
				break;

			case fddiPORTOperationGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PORTOperationGrpList, &attrLen);
				break;

			case fddiPORTErrorCtrsGrp:
				result = GetMIBGroup (stnID, bufLeft, (Void_p) resParam,
					PORTErrorCtrsGrpList, &attrLen);
				break;

			default:
				result = GetMIBValue (stnID, bufLeft,
					(Void_p) resParam, &attrLen);
			}

			/* if buffer is full or invalid attribute */
			if ((result == RC_BUFFER_ERROR) || (result == RC_IllegalOperation))
			{
				/* clear type/length */
				resParam->paramType = 0;
				resParam->paramLen = 0;

				/* then exit */
				break;
			}

			/* else if get was successful */
			else if (result == RC_Success)
			{
				/* accumulate lengths */
				attrCount++;
				bufLeft -= attrLen;
				bufUsed += attrLen;
			}

			/* if result is NoParameter */
			else if (result == RC_NoParameter)
			{
				/* clear type/length */
				resParam->paramType = 0;
				resParam->paramLen = 0;

				/* set flag for determining correct return value */
				noParamFlag = SET;
			}
		}
	}

	if (setCount != (TLVSetCountType *) NULL)
	{
		setCount->paramType = fddiSMTSetCount;
		setCount->paramLen = SIZE_SMTSetCountType;
		MEMCOPY (&setCount->paramValue,
			&(MIBDATA (stnID, mib->smtData.SetCount)),
			SIZE_SMTSetCountType);
	}

	/*
	 * If MSP was able to process some of the request, but any one of the
	 * requested attributes returned NoParameter, then the return value must
	 * indicate this.
	 */
	if (result == RC_Success && noParamFlag)
		result = RC_NoParameter;

	/*
	 * If there were no parameters found, then clear the first parameter type
	 * and length to mark the buffer as empty.
	 */
	if (attrCount == 0)
		MEMZERO (bufData, SIZE_TLVHdrType);

	return (result);
}

uInt32
#ifdef __STDC__
GetMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount)
#else
GetMIBAttrMulti (stnID, bufSize, bufData, setCount)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
#endif
/*********************************************************************
Function:	MIB access routine to get a single MIB attribute. This routine
			performs a Get operation where only ONE attribute is
			requested. This is the default routine to match the
			operation of previouse XLNT Manager(tm) release. This
			routine forces the request buffer to contain one ONE
			attribute request.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			setCount	= pointer to buffer to hold current value
							of the fddiSMTSetCountAttribute
							(may be NULL if current SetCount
							is not required).
Input:		bufData		= contains the requested attribute ID and
							object index (if required) using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			setCount	= if not NULL, this buffer gets set to the
							current value of fddiSMTSetCount,
							also using the MIB TLV encoding.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported
			RC_IllegalOperation
				illegal parameter requested
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute or attribute group. In this
				case, what has been processed will be returned
				in the buffer.
Notes:	This routine only allows SINGLE attribute requests. This provides
		backward compatibility with previous release interfaces. Otherwise,
		if multiple Gets were assumed, dirty buffers may look like they
		contain multiple requests and will cause errors.
*********************************************************************/
{
TLVHdrType      *param;					/* parameter header */
uInt16           len;					/* parameter length */

	/* get parameter */
	param = (TLVHdrType *) bufData;

	/* determine length */
	len = param->paramLen + SIZE_TLVHdrType;

	/* check if zeros need to be written to mark end */
	if (len < bufSize)
	{
		/* clear remainder of buffer */
		MEMZERO (MADDPTR (uChar, bufData, len), bufSize - len);
	}

	/*
	 * assume OK at this point. Any problems will be picked up later. Now
	 * call Get routine.
	 */
	return (MultiGetMIBAttrMulti (stnID, bufSize, bufData, setCount));
}

uInt32
#ifdef __STDC__
ChangeMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	TLVSetCountType * setCount, SMTStationIdType * stationID)
#else
ChangeMIBAttrMulti (stnID, bufSize, bufData, setCount, stationID)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
		SMTStationIdType *stationID;
#endif
/*********************************************************************
Function:	MIB access routine to do a Change or Assert operation
		on a MIB attribute.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars. This parameter
							is UNUSED, but is left here to
							preserve the interface.
			bufData		= pointer to buffer containing the
							new attribute information
							and used to hold attribute value
							returned.
			setCount	= pointer to buffer holding the current value
							of the fddiSMTSetCount attribute.
							If NULL, then the operation is an Assert.
			stationID	= pointer to buffer holding ID of station
							requesting the change.
Input:		bufData		= contains the new attribute value using
						the MIB TLV encoding for a Get
						Request (see SMT document).
			setCount	= contains the verification value for
							fddiSMTSetCount. This pointer may be supplied
							if from an SMT message. In this case, the
							parameter type will be zero if SetCount is
							to be ignored.
			stationID	= contains station ID of the station
							requesting the attribute change.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			setCount	= set to the value of fddiSMTSetCount after
							attempting the attribute change,
							also using the MIB TLV encoding. If
							NULL, then no SetCount is returned.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute not supported
			RC_IllegalOperation
				illegal parameter requested, or
				attempted to change a read-only parameter
			RC_OutOfRange
				value given is out of bounds
			RC_BadSetCount
				the value of setCount does not match the current
				MIB value
Notes:		Reason codes LengthError, FrameTooLong and NotAuthorized
			are generated by FSP in response to a PMF Set Request.
Modification History:
*********************************************************************/
{
TLVParamType    *phdr;					/* Parameter header */
uInt32           result;				/* function return value */
Int16            comp;					/* station ID comparision */
MIBType         *mibPtr;				/* MIB database */

	/* get MIB database */
	mibPtr = MIBDATA (stnID, mib);

	result = RC_Success;
	if (setCount != (TLVSetCountType *) NULL)
	{
		/* if valid set count in buffer */
		if (setCount->paramType == fddiSMTSetCount)
		{
			/* validate set count */
			if (setCount->paramValue.count != mibPtr->smtData.SetCount.count)
				result = RC_BadSetCount;
		}
	}

	if (result == RC_Success)
	{
		/*
		 * Get header info.
		 */
		phdr = (TLVParamType *) bufData;

		/*
		 * Select proper processing routine.
		 */
		switch (phdr->paramType & MIB_OBJECT_MASK)
		{
		case fddiSMT:
			/* validate minimum length */
			if (phdr->paramType == fddiSMTVendorAttrib)
			{
				/* min size must include vendor ID + a value */
				if (phdr->paramLen < (2 * SIZE_DEFAULT))
					result = RC_LengthError;
			}
			else
			{
				/* all other SMT attributes have a value */
				if (phdr->paramLen < SIZE_DEFAULT)
					result = RC_LengthError;
			}

			if (result == RC_Success)
				result = ChangeMIBSMTAttr (stnID, bufData);

			break;

		case fddiMAC:
			if (phdr->paramType == fddiMACVendorAttrib)
			{
				/* min size must include resource index + vendor ID + value */
				if (phdr->paramLen < (3 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			else
			{
				/* min size must include resource index + value */
				if (phdr->paramLen < (2 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			if (result == RC_Success)
				result = ChangeMIBMACAttr (stnID, bufData);

			break;

		case fddiPATH:
			if (phdr->paramType == fddiPATHVendorAttrib)
			{
				/* min size must include resource index + vendor ID + value */
				if (phdr->paramLen < (3 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			else
			{
				/* min size must include resource index + value */
				if (phdr->paramLen < (2 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			if (result == RC_Success)
				result = ChangeMIBPATHAttr (stnID, bufData);

			break;

		case fddiPORT:
			if (phdr->paramType == fddiPORTVendorAttrib)
			{
				/* min size must include resource index + vendor ID + value */
				if (phdr->paramLen < (3 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			else
			{
				/* min size must include resource index + value */
				if (phdr->paramLen < (2 * SIZE_DEFAULT))
					result = RC_LengthError;
			}

			if (result == RC_Success)
				result = ChangeMIBPORTAttr (stnID, bufData);
			break;

#ifdef	XM_USE_VSO

			/*
			 * If vendor-specific objects are supported, then call the
			 * appropriate change routine.
			 */
		case fddiVSO:
			/* min size must include vendor ID + value */
			if (phdr->paramLen < (2 * SIZE_DEFAULT))
				result = RC_LengthError;

			if (result == RC_Success)
			{
				if (phdr->VSOOBJECT == VSO_calmMASIC)
					/* XLNT Design's MASIC object */
					result = MChangeMIBMASICAttr (stnID, bufData);

				else
					/* Other vendor-specific objects */
					result = MChangeMIBVSOAttr (stnID, bufData);
			}

			break;

#endif							/* XM_USE_VSO */

		default:
			result = RC_IllegalOperation;
			break;
		}

		/*
		 * If change successful, update set count and set last set station
		 * ID.
		 */
		if (result == RC_Success)
		{
			mibPtr->smtData.SetCount.count++;
			GetTimeStampMulti (stnID, &mibPtr->smtData.SetCount.settimestamp);
			LocalMIBEventMulti (stnID, mibPtr, fddiSMTSetCount, 0, (uInt32) 0);

			MEMCOPY (&mibPtr->smtData.LastSetStationId,
				stationID, SIZE_SMTStationIdType);
			LocalMIBEventMulti (stnID, mibPtr, fddiSMTLastSetStationId, 0,
				(uInt32) 0);

			/* check for remote disconnect condition */
			if ((phdr->paramType == fddiSMTStationAction)
				&& (phdr->SMTPARAM16 == SMTAction_disconnect))
			{
				MCompareAddress (stationID->longaddress,
					mibPtr->smtData.StationId.longaddress,
					comp);
				/* set remote disconnect if not this station */
				mibPtr->smtData.RemoteDisconnectFlag
					= (comp == 0) ? FALSE : TRUE;
				LocalMIBEventMulti (stnID, mibPtr,
					fddiSMTRemoteDisconnectFlag, 0, (uInt32) 0);
			}
		}
	}

	/*
	 * Return current set count.
	 */
	if (setCount != (TLVSetCountType *) NULL)
	{
		setCount->paramType = fddiSMTSetCount;
		setCount->paramLen = SIZE_SMTSetCountType;
		MEMCOPY (&setCount->paramValue,
			&mibPtr->smtData.SetCount, SIZE_SMTSetCountType);
	}

	return (result);
}

uInt32
#ifdef __STDC__
GetXMMIBAttrMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData)
#else
GetXMMIBAttrMulti (stnID, bufSize, bufData)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	Get the value for a MIB attribute in response to a request
			from CSP or FSP.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information.
Input:		None.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute. In this case, as much as
				has been processed is returned.
Notes:		Although this routine more closely resembles GetMIBValue()
			in mibget.c, it is in this file because it is a publicly
			accessible interface. This routine may be called directly
			from CSPGetMIBAttr() or FBMGetMIBAttr().
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* pointer to parameter to get */
MACDataType     *macPtr;				/* pointer into MAC base table */
PATHDataType    *pathPtr;				/* pointer into PATH base table */
PORTDataType    *portPtr;				/* pointer into PORT base table */
MIBType         *mibPtr;				/* MIB database */
uInt32           result;				/* return value */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Select object processing.
	 */
	param = (TLVParamType *) bufData;
	switch (param->paramType & MIB_OBJECT_MASK)
	{
	case fddiSMT:
		/*
		 * If vendor specific attribute, route to correct vendor routine.
		 * Otherwise, process standard attribute.
		 */
		if (param->paramType == fddiSMTVendorAttrib)
		{
			if ((param->SMTVENDORID & VENDOR_MASK) == fddiXDI)
				result = GetXDISMTAttr (stnID, bufSize,
					param, &mibPtr->smtData);
			else
				result = GetVendorSMTAttrMulti (stnID, bufSize,
					param, &mibPtr->smtData);
		}
		else
			result = GetMIBSMTAttr (stnID, bufSize, param, &mibPtr->smtData);
		break;

	case fddiMAC:
		/*
		 * If vendor specific attribute, route to correct vendor routine.
		 * Otherwise, process standard attribute.
		 */
		macPtr = &mibPtr->macBase[param->MACINDEX];
		if (macPtr->inUse)
		{
			if (param->paramType == fddiMACVendorAttrib)
			{
				if ((param->MACVENDORID & VENDOR_MASK) == fddiXDI)
					result = GetXDIMACAttr (stnID, bufSize, param, macPtr);
				else
					result = GetVendorMACAttrMulti (stnID, bufSize,
						param, macPtr);
			}
			else
				result = GetMIBMACAttr (stnID, bufSize, param, macPtr);
		}
		else
			result = RC_NoParameter;

		/*
		 * If result is RC_Success, then set the resource ID. This overwrites
		 * the internal array index value passed into the routine.
		 */
		if (result == RC_Success)
			param->MACINDEX = macPtr->baseData.Index;

		break;

	case fddiPATH:
		/*
		 * If vendor specific attribute, route to correct vendor routine.
		 * Otherwise, process standard attribute.
		 */
		pathPtr = &mibPtr->pathBase[param->PATHINDEX];
		if (pathPtr->inUse)
		{
			if (param->paramType == fddiPATHVendorAttrib)
			{
				if ((param->PATHVENDORID & VENDOR_MASK) == fddiXDI)
					result = GetXDIPATHAttr (stnID, bufSize, param, pathPtr);
				else
					result = GetVendorPATHAttrMulti (stnID, bufSize,
						param, pathPtr);
			}
			else
				result = GetMIBPATHAttr (stnID, bufSize, param, pathPtr);
		}
		else
			result = RC_NoParameter;

		/*
		 * If result is RC_Success, then set the resource ID. This overwrites
		 * the internal array index value passed into the routine.
		 */
		if (result == RC_Success)
			param->PATHINDEX = pathPtr->baseData.Index;

		break;

	case fddiPORT:
		/*
		 * If vendor specific attribute, route to correct vendor routine.
		 * Otherwise, process standard attribute.
		 */
		portPtr = &mibPtr->portBase[param->PORTINDEX];
		if (portPtr->inUse)
		{
			if (param->paramType == fddiPORTVendorAttrib)
			{
				if ((param->PORTVENDORID & VENDOR_MASK) == fddiXDI)
					result = GetXDIPORTAttr (stnID, bufSize, param, portPtr);
				else
					result = GetVendorPORTAttrMulti (stnID, bufSize,
						param, portPtr);
			}
			else
				result = GetMIBPORTAttr (stnID, bufSize, param, portPtr);
		}
		else
			result = RC_NoParameter;

		/*
		 * If result is RC_Success, then set the resource ID. This overwrites
		 * the internal array index value passed into the routine.
		 */
		if (result == RC_Success)
			param->PORTINDEX = portPtr->baseData.Index;

		break;

	case fddiVSO:
		/*
		 * NOTE: This fast entry point does not support vendor-specific
		 * objects. Nothing within SMT has a need for fast access to a VSO,
		 * therefore all access will be through the standard entry points.
		 */
	default:
		result = RC_NoParameter;
		break;
	}

	return (result);
}


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
MultiGetMIBAttr (uInt16 bufSize, Void_p bufData, TLVSetCountType * setCount)
#else
MultiGetMIBAttr (bufSize, bufData, setCount)
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
#endif
{
	return (MultiGetMIBAttrMulti (FIRST_STATION, bufSize,
			bufData, setCount));
}

uInt32
#ifdef __STDC__
GetMIBAttr (uInt16 bufSize, Void_p bufData, TLVSetCountType * setCount)
#else
GetMIBAttr (bufSize, bufData, setCount)
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
#endif
{

	return (GetMIBAttrMulti (FIRST_STATION, bufSize, bufData, setCount));
}

uInt32
#ifdef __STDC__
ChangeMIBAttr (uInt16 bufSize, Void_p bufData, TLVSetCountType * setCount,
	SMTStationIdType * stationID)
#else
ChangeMIBAttr (bufSize, bufData, setCount, stationID)
		uInt16           bufSize;
		Void_p           bufData;
		TLVSetCountType *setCount;
		SMTStationIdType *stationID;
#endif
{

	return (ChangeMIBAttrMulti (FIRST_STATION, bufSize, bufData,
			setCount, stationID));
}

uInt32
#ifdef __STDC__
GetXMMIBAttr (uInt16 bufSize, Void_p bufData)
#else
GetXMMIBAttr (bufSize, bufData)
		uInt16           bufSize;
		Void_p           bufData;
#endif
{
	return (GetXMMIBAttrMulti (FIRST_STATION, bufSize, bufData));
}

#endif							/* MULTIPLE_STATION */
