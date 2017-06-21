/* $Id: fbmraf.c,v 3.2 1995/11/17 18:10:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmraf.c,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fbmraf.c,v $
 * Revision 3.2  1995/11/17  18:10:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmraf.c xm7_v3.1.1:cset.0173:13:13 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Process RAF Module

		File:			xm7_fbmraf.c
		Created:		07/28/92

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is responsible for processing Resource Allocation
		Frames and taking whatever action is necessary for processing
		each frame.  For frames which require a require a response one
		is generated immediately.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_RAF		include code for RAF processing.

		Resource Allocation Frame processing is only included when the
		XM_USE_RAF constant is defined. Otherwise, default processing of
		RAFs (ie, deny requests and ignore other frames) is included.

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


/*
 * Resource Allocation Frame processing is only included when the XM_USE_RAF
 * constant is defined. Otherwise, default processing of RAFs (ie, deny
 * requests and ignore other frames) is included.
 */
#ifdef	XM_USE_RAF

/*********************************************************************
	RAF Frame Routines
*********************************************************************/

uInt32
#ifdef __STDC__
BuildRAFRequestMulti (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, uChar * info, uInt16 infoLen)
#else
BuildRAFRequestMulti (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		uChar           *info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds a Resource Allocation Request frame.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			info		= buffer containing data for RAF info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			info		= info field of the RAF. This field must
							be properly formatted in network byte order.
Output:		frameBuffer	= contains completed frame.
Return:		Transaction ID is returned if frame was built. Otherwise
			0 is returned.
Modification History:
*********************************************************************/
{
Void_p           memPtr;				/* Buffer used to build frame */
SMTFrameHdrType *frameHdr;				/* Used for building SMT header */
uInt16           frameLen;				/* Total bytes in outgoing frame */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	frameLen = 0;
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;


	/*
	 * Only build frame if info field fits into frame.
	 */
	if (infoLen <= MAX_SMT_INFO_LEN)
	{
		/*
		 * Initialize frame header.
		 */
		result = BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr,
			outputMAC, RAF_CLASS, SMTREQUEST, transID);

		/*
		 * Get the start of the info portion.
		 */
		memPtr = MADDPTR (Void_t, frameHdr, SIZE_SMTFrameHdrType);

		/* Fill in RAF data */
		if (info)
		{
			MEMCOPY (memPtr, info, infoLen);
			frameLen += infoLen;
			memPtr = MADDPTR (Void_t, memPtr, infoLen);
		}

		/*
		 * Set frame length.
		 */
		frameHdr->smtHdr.InfoField_Length = Mhtons (frameLen);
	}

	/*
	 * Return transaction ID.
	 */
	return (result);
}

uInt32
#ifdef __STDC__
BuildRAFResponseMulti (uInt16 stnID, Void_p frameBuffer,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, uChar * info, uInt16 infoLen)
#else
BuildRAFResponseMulti (stnID, frameBuffer, destAddr, outputMAC,
	transID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		uChar           *info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds an RAF Response.
Parameters:	stnID		= index of station to use.
			frameBuffer	= pointer to buffer for response frame.
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			info		= buffer containing data for RAF info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			info		= info field of the RAF. This field must
							be properly formatted in network byte order.
Output:		frameBuffer	= contains RAF Response Frame.
Return:		Transaction ID used in frame.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* Used for building SMT header frame */
uInt32           result;				/* value to return */

	/*
	 * Use BuildRAFRequestMulti() to create basic frame format.
	 */
	result = BuildRAFRequestMulti (stnID, frameBuffer, SMT_INFO_FC,
		destAddr, outputMAC, transID, info, infoLen);

	/*
	 * Set frame type.
	 */
	frameHdr = (SMTFrameHdrType *) frameBuffer;
	frameHdr->smtHdr.Frame_Type = SMTRESPONSE;

	/*
	 * Return transaction ID.
	 */
	return (result);
}

/*********************************************************************
	Synchronous Bandwidth Allocation Parameters
*********************************************************************/

uInt16
#ifdef __STDC__
ResourceType_Param (ResourceTypeParamType * param, uInt32 resType)
#else
ResourceType_Param (param, resType)
		ResourceTypeParamType *param;
		uInt32           resType;
#endif
/*********************************************************************
Function:	Create the Resource Type parameter.
Parameters:	param	= pointer to location where parameter should start.
			resType	= resource type to use in parameter.
Input:		None.
Output:		param	= contains the SMT Resource Type parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (RESOURCE_TYPE_PARAM_TYPE);
	param->Length = Mhtons (SIZE_ResourceTypeParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->ResourceType = Mhtonl (resType);

	return (SIZE_ResourceTypeParamType);
}

uInt16
#ifdef __STDC__
SBACommand_Param (SBACommandParamType * param, uInt32 command)
#else
SBACommand_Param (param, command)
		SBACommandParamType *param;
		uInt32           command;
#endif
/*********************************************************************
Function:	Create the SBA Command parameter.
Parameters:	param	= pointer to location where parameter should start.
			command	= SBA command to put into parameter.
Input:		None.
Output:		param	= contains the SMT SBA Command parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_COMMAND_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBACommandParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->SbaCommand = Mhtonl (command);

	return (SIZE_SBACommandParamType);
}

uInt16
#ifdef __STDC__
SBAPayload_Param (SBAPayloadParamType * param, uInt32 payload)
#else
SBAPayload_Param (param, payload)
		SBAPayloadParamType *param;
		uInt32           payload;
#endif
/*********************************************************************
Function:	Create the SBA Payload Request parameter.
Parameters:	param	= pointer to location where parameter should start.
			payload	= total synch bandwidth requested.
Input:		None.
Output:		param	= contains the SMT SBA Payload parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_PAYLOAD_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBAPayloadParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->SbaRequest = Mhtonl (payload);

	return (SIZE_SBAPayloadParamType);
}

uInt16
#ifdef __STDC__
SBAOverhead_Param (SBAOverheadParamType * param, uInt32 overhead)
#else
SBAOverhead_Param (param, overhead)
		SBAOverheadParamType *param;
		uInt32           overhead;
#endif
/*********************************************************************
Function:	Create the SBA Overhead Request parameter.
Parameters:	param		= pointer to location where parameter
							should start.
			overhead	= total synch bandwidth requested for overhead.
Input:		None.
Output:		param		= contains the SMT SBA Overhead parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_OVERHEAD_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBAOverheadParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->SbaOverheadReq = Mhtonl (overhead);

	return (SIZE_SBAOverheadParamType);
}

uInt16
#ifdef __STDC__
SBAAllocAddr_Param (SBAAllocAddrParamType * param, FDDILongAddressType address)
#else
SBAAllocAddr_Param (param, address)
		SBAAllocAddrParamType *param;
		FDDILongAddressType address;
#endif
/*********************************************************************
Function:	Create the SBA Allocation Address parameter.
Parameters:	param	= pointer to location where parameter should start.
			address	= buffer containing allocating station address.
Input:		None.
Output:		param	= contains the SMT SBA Allocation Address parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_ALLOC_ADDR_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBAAllocAddrParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	MCopyAddress (param->SbaAllocationAddr, address);

	return (SIZE_SBAAllocAddrParamType);
}

uInt16
#ifdef __STDC__
SBACategory_Param (SBACategoryParamType * param, uInt32 category)
#else
SBACategory_Param (param, category)
		SBACategoryParamType *param;
		uInt32           category;
#endif
/*********************************************************************
Function:	Create the SBA Category parameter.
Parameters:	param		= pointer to location where parameter
							should start.
			category	= allocation category.
Input:		None.
Output:		param		= contains the SMT SBA Category parameter.
Return:		Length in characters of the parameter including
		the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_CATEGORY_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBACategoryParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->Category = Mhtonl (category);

	return (SIZE_SBACategoryParamType);
}

uInt16
#ifdef __STDC__
MaxT_Neg_Param (MaxTnegParamType * param, uInt32 max_tneg)
#else
MaxT_Neg_Param (param, max_tneg)
		MaxTnegParamType *param;
		uInt32           max_tneg;
#endif
/*********************************************************************
Function:	Create the Maximum T_Neg parameter.
Parameters:	param		= pointer to location where parameter
							should start.
			max_tneg	= longest acceptable T_Neg for synch service.
Input:		None.
Output:		param		= contains the SMT Maximum T_Neg parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (MAX_T_NEG_PARAM_TYPE);
	param->Length = Mhtons (SIZE_MaxTnegParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->MaxTneg = Mhtonl (max_tneg);

	return (SIZE_MaxTnegParamType);
}

uInt16
#ifdef __STDC__
MinSegment_Param (MinSegmentParamType * param, uInt32 minSegment)
#else
MinSegment_Param (param, minSegment)
		MinSegmentParamType *param;
		uInt32           minSegment;
#endif
/*********************************************************************
Function:	Create the SBA Minimum Segment parameter.
Parameters:	param		= pointer to location where parameter
							should start.
			minSegment	= smallest # bytes per frame.
Input:		None.
Output:		param		= contains the SMT Minimum Segment parameter.
Return:		Length in characters of the parameter including
		the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (MIN_SEGMENT_PARAM_TYPE);
	param->Length = Mhtons (SIZE_MinSegmentParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->MinSegment = Mhtonl (minSegment);

	return (SIZE_MinSegmentParamType);
}

uInt16
#ifdef __STDC__
SBAAllocatable_Param (SBAAllocatableParamType * param, uInt32 allocatable)
#else
SBAAllocatable_Param (param, allocatable)
		SBAAllocatableParamType *param;
		uInt32           allocatable;
#endif
/*********************************************************************
Function:	Create the SBA Allocatable parameter.
Parameters:	param		= pointer to location where parameter
							should start.
			allocatable	= total synch bandwidth requested for overhead.
Input:		None.
Output:		param		= contains the SMT SBA Allocateable parameter.
Return:		Length in characters of the parameter including
		the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (SBA_ALLOCATABLE_PARAM_TYPE);
	param->Length = Mhtons (SIZE_SBAAllocatableParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->SbaAllocatable = Mhtonl (allocatable);

	return (SIZE_SBAAllocatableParamType);
}

static           uInt16
#ifdef __STDC__
SetSBAParams (uInt16 stnID, TLVParamType * param, uInt16 pathIndex,
	uInt32 command, uInt16 * totalLen)
#else
SetSBAParams (stnID, param, pathIndex, command, totalLen)
		uInt16           stnID;
		TLVParamType    *param;
		uInt16           pathIndex;
		uInt32           command;
		uInt16          *totalLen;
#endif
/*********************************************************************
Function:	This function builds the common parameters found in SBA frames.
Parameters:	stnID		= station index to use
			param		= address of buffer to put parameters
			pathIndex	= index of Path to use to retrieve data
			command		= SBA command for this frame (if 0, then resource
							type and command parameters are NOT put
							into buffer
			totalLen	= address of buffer to put total length of parameters
Input:		param		= a buffer that has been cleared
Output:		param		= contains the following attributes:
								SMT Resource Type
								SMT SBA Command
								fddiPATHIndex
								fddiPATHSbaPayload
								fddiPATHSbaOverhead
							the attributes are in TLV format.
							The value of param is set to the position
							in the buffer following the last parameter.
Return:		0						params were built successfully
			EFBM_SBA_NOT_AVAILABLE	SBA not supported on requested path
Modification History:
*********************************************************************/
{
uInt16           len;					/* parameter length added to frame */
uInt16           result;				/* return value */
uInt32           mibResult;				/* MIB access result */

	/*
	 * Initialize values
	 */
	result = 0;
	*totalLen = 0;

	/*
	 * If resource type and command parameters requested
	 */
	if (command)
	{
		/*
		 * Resource Type parameter
		 */
		len = ResourceType_Param ((ResourceTypeParamType *) param,
			RESOURCE_SB);
		*totalLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * SBA Command parameter
		 */
		len = SBACommand_Param ((SBACommandParamType *) param, command);
		*totalLen += len;
		param = MADDPTR (TLVParamType, param, len);
	}

	/*
	 * Get Path index
	 */
	param->paramType = fddiPATHIndex;
	param->paramLen = 4;
	param->PATHINDEX = pathIndex;
	mibResult = FBMMultiGetMIBAttrMulti (stnID, sizeof (TLVParamType),
		(Void_p) param, (TLVSetCountType *) NULL);
	if (mibResult == RC_Success)
	{
		/*
		 * Change order and move pointer
		 */
		len = param->paramLen + SIZE_TLVHdrType;
		*totalLen += len;
		AttrNetOrder (param, NET_ORDER);
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Get current payload
		 */
		param->paramType = fddiPATHSbaPayload;
		param->paramLen = 4;
		param->PATHINDEX = pathIndex;
		mibResult = FBMMultiGetMIBAttrMulti (stnID, sizeof (TLVParamType),
			(Void_p) param, (TLVSetCountType *) NULL);

		if (mibResult == RC_Success)
		{
			/*
			 * Change order and move pointer
			 */
			len = param->paramLen + SIZE_TLVHdrType;
			*totalLen += len;
			AttrNetOrder (param, NET_ORDER);
			param = MADDPTR (TLVParamType, param, len);

			/*
			 * Get overhead
			 */
			param->paramType = fddiPATHSbaOverhead;
			param->paramLen = 4;
			param->PATHINDEX = pathIndex;
			mibResult = FBMMultiGetMIBAttrMulti (stnID, sizeof (TLVParamType),
				(Void_p) param, (TLVSetCountType *) NULL);

			/*
			 * Change order and move pointer
			 */
			len = param->paramLen + SIZE_TLVHdrType;
			*totalLen += len;
			AttrNetOrder (param, NET_ORDER);
			param = MADDPTR (TLVParamType, param, len);
		}
	}

	if (mibResult != RC_Success)
	{
		/* no SBA available */
		result = EFBM_SBA_NOT_AVAILABLE;
	}

	return (result);
}


/*********************************************************************
	SBA Received Frame Processing Routines
*********************************************************************/

/*
 * The following defined values indicate which parameters have been found
 * in the received frame. This allows the SBA parsing routines to validate
 * that the contents of the frame contains the minimum set of parameters.
 * The parsing routines DO NOT check for duplicate parameters in a frame.
 * The last parameter found will be the value reported to the MAP.
 */
#define GOT_PATH_INDEX			0x0001	/* fddiPATHIndex */
#define GOT_REQ_PAYLOAD			0x0002	/* SBA Payload Request */
#define GOT_REQ_OVERHEAD		0x0004	/* SBA Overyead Request */
#define GOT_PAYLOAD				0x0008	/* fddiPATHSbaPayload */
#define GOT_OVERHEAD			0x0010	/* fddiPATHSbaOverhead */
#define GOT_ALLOC_ADDR			0x0020	/* SBA Allocation Address */
#define GOT_CATEGORY			0x0040	/* SBA Category */
#define GOT_MAX_TNEG			0x0080	/* Maximum T_Neg */
#define GOT_MIN_SEGMENT			0x0100	/* Minimum SBA Segment Size */
#define	GOT_REASON_CODE			0x0200	/* Reason Code */
#define	GOT_ALLOCATABLE			0x0400	/* SBA Allocatable */

#define	GOT_ALLOC_REQ\
	(GOT_PATH_INDEX | GOT_REQ_PAYLOAD | GOT_REQ_OVERHEAD | GOT_PAYLOAD |\
		GOT_OVERHEAD | GOT_ALLOC_ADDR | GOT_CATEGORY | GOT_MAX_TNEG |\
		GOT_MIN_SEGMENT)

#define	GOT_ALLOC_RSP\
	(GOT_REASON_CODE | GOT_PATH_INDEX | GOT_PAYLOAD | GOT_OVERHEAD |\
		GOT_ALLOC_ADDR | GOT_CATEGORY | GOT_ALLOCATABLE)

#define	GOT_CHANGE_REQ\
	(GOT_PATH_INDEX | GOT_PAYLOAD | GOT_OVERHEAD | GOT_CATEGORY)

#define	GOT_CHANGE_RSP\
	(GOT_PATH_INDEX | GOT_PAYLOAD | GOT_OVERHEAD | GOT_CATEGORY)

#define	GOT_REPORT_RSP\
	(GOT_PATH_INDEX | GOT_PAYLOAD | GOT_OVERHEAD)

static           uInt32
#ifdef __STDC__
ProcessSBAAllocRequest (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum, TLVParamType * info)
#else
ProcessSBAAllocRequest (stnID, frameHdr, MACNum, info)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
		TLVParamType    *info;
#endif
/*********************************************************************
Function:	This function processes an RAF Request for the synchronous
			bandwidth allocation. The frame is parsed (and validated
			in the process) and the information is sent to the synchronous
			bandwidth manager through the MAP.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine converts the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		The return value is the reason code. If the reason code is
			not RC_Success, the calling routine is responsible for sending
			the RDF. For missing parameters, the reason code
			IllegalOperation is used because this error is not described
			by the SMT document.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
SMTMessage       msg;					/* message to send */
SbaAllocReqType  allocReq;				/* allocation request parsing info */
uInt32           result;				/* return value */
uInt16           found;					/* which params found */
uInt16           pos;					/* position in info field */


	result = RC_Success;
	found = 0;

	/*
	 * Get parameters from frame header.
	 */
	MCopyAddress (allocReq.address, frameHdr->macHdr.SA);
	allocReq.transactionID = Mntohl (frameHdr->smtHdr.Transaction_ID);

	/*
	 * Loop through InfoField and parase parameters.
	 */
	pos = 0;
	param = info;
	while (pos < Mntohs (frameHdr->smtHdr.InfoField_Length))
	{
		switch (Mntohs (param->paramType))
		{
		case fddiPATHIndex:
			allocReq.pathIndex = Mntohs (param->PATHINDEX);
			found |= GOT_PATH_INDEX;
			break;

		case SBA_PAYLOAD_PARAM_TYPE:
			allocReq.requestPayload = Mntohl (param->SMTPARAM32);
			found |= GOT_REQ_PAYLOAD;
			break;

		case SBA_OVERHEAD_PARAM_TYPE:
			allocReq.requestOverhead = Mntohl (param->SMTPARAM32);
			found |= GOT_REQ_OVERHEAD;
			break;

		case fddiPATHSbaPayload:
			allocReq.payload = Mntohl (param->PATHPARAM32);
			found |= GOT_PAYLOAD;
			break;

		case fddiPATHSbaOverhead:
			allocReq.overhead = Mntohl (param->PATHPARAM32);
			found |= GOT_OVERHEAD;
			break;

		case SBA_ALLOC_ADDR_PARAM_TYPE:
			MCopyAddress (allocReq.allocAddress, param->SMTADDR);
			found |= GOT_ALLOC_ADDR;
			break;

		case SBA_CATEGORY_PARAM_TYPE:
			allocReq.category = Mntohl (param->SMTPARAM32);
			found |= GOT_CATEGORY;
			break;

		case MAX_T_NEG_PARAM_TYPE:
			allocReq.maxTNeg = Mntohl (param->SMTPARAM32);
			found |= GOT_MAX_TNEG;
			break;

		case MIN_SEGMENT_PARAM_TYPE:
			allocReq.minSegment = Mntohl (param->SMTPARAM32);
			found |= GOT_MIN_SEGMENT;
			break;

		default:
			/* ignore parameter */
			break;
		}

		/*
		 * Move position in frame
		 */
		pos += (Mntohs (param->paramLen) + SIZE_TLVHdrType);
		param = MADDPTR (TLVParamType, info, pos);
	}

	/*
	 * If all parameters were found,
	 */
	if (found == GOT_ALLOC_REQ)
	{
		/*
		 * Send information to MAP
		 */
		msg.destination = MAP_MSG_ID;
		msg.source = FBM_MSG_ID;
		msg.type = FBM_EVENT_SBA;
		msg.typeInfo = SBA_ALLOC_REQ_RX;
		msg.localID = 0;
		msg.entity = MACNum;
		msg.stnID = stnID;
		msg.len1 = SIZE_SbaAllocReqType;
		msg.len2 = 0;
		msg.p1.sba.allocReq = &allocReq;
		SendFBMMessage (&msg);
	}

	else
	{
		/*
		 * Return error code
		 */
		result = RC_IllegalOperation;
	}

	return (result);
}

static void
#ifdef __STDC__
ProcessSBAAllocResponse (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum, TLVParamType * info)
#else
ProcessSBAAllocResponse (stnID, frameHdr, MACNum, info)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
		TLVParamType    *info;
#endif
/*********************************************************************
Function:	This function processes an RAF Response for the synchronous
			bandwidth allocation. The frame is parsed (and validated
			in the process) and the information is sent to the synchronous
			bandwidth manager through the MAP.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine converts the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		None.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
SMTMessage       msg;					/* message to send */
SbaAllocRspType  allocRsp;				/* allocation response parsing info */
uInt16           found;					/* which params found */
uInt16           pos;					/* position in info field */


	found = 0;

	/*
	 * Get parameters from frame header.
	 */
	MCopyAddress (allocRsp.address, frameHdr->macHdr.SA);
	allocRsp.transactionID = Mntohl (frameHdr->smtHdr.Transaction_ID);

	/*
	 * Loop through InfoField and parase parameters.
	 */
	pos = 0;
	param = info;
	while (pos < Mntohs (frameHdr->smtHdr.InfoField_Length))
	{
		switch (Mntohs (param->paramType))
		{
		case REASON_CODE_PARAM_TYPE:
			allocRsp.reasonCode = Mntohl (param->SMTPARAM32);
			found |= GOT_REASON_CODE;
			break;

		case fddiPATHIndex:
			allocRsp.pathIndex = Mntohs (param->PATHINDEX);
			found |= GOT_PATH_INDEX;
			break;

		case fddiPATHSbaPayload:
			allocRsp.payload = Mntohl (param->PATHPARAM32);
			found |= GOT_PAYLOAD;
			break;

		case fddiPATHSbaOverhead:
			allocRsp.overhead = Mntohl (param->PATHPARAM32);
			found |= GOT_OVERHEAD;
			break;

		case SBA_ALLOC_ADDR_PARAM_TYPE:
			MCopyAddress (allocRsp.allocAddress, param->SMTADDR);
			found |= GOT_ALLOC_ADDR;
			break;

		case SBA_CATEGORY_PARAM_TYPE:
			allocRsp.category = Mntohl (param->SMTPARAM32);
			found |= GOT_CATEGORY;
			break;

		case SBA_ALLOCATABLE_PARAM_TYPE:
			allocRsp.allocatable = Mntohl (param->SMTPARAM32);
			found |= GOT_ALLOCATABLE;
			break;

		default:
			/* ignore parameter */
			break;
		}

		/*
		 * Move position in frame
		 */
		pos += (Mntohs (param->paramLen) + SIZE_TLVHdrType);
		param = MADDPTR (TLVParamType, info, pos);
	}

	/*
	 * If all parameters were found,
	 */
	if (found == GOT_ALLOC_RSP)
	{
		/*
		 * Send information to MAP
		 */
		msg.destination = MAP_MSG_ID;
		msg.source = FBM_MSG_ID;
		msg.type = FBM_EVENT_SBA;
		msg.typeInfo = SBA_ALLOC_RSP_RX;
		msg.localID = 0;
		msg.entity = MACNum;
		msg.stnID = stnID;
		msg.len1 = SIZE_SbaAllocRspType;
		msg.len2 = 0;
		msg.p1.sba.allocRsp = &allocRsp;
		SendFBMMessage (&msg);
	}

	return;
}

static           uInt32
#ifdef __STDC__
ProcessSBAChangeRequest (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum, TLVParamType * info)
#else
ProcessSBAChangeRequest (stnID, frameHdr, MACNum, info)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
		TLVParamType    *info;
#endif
/*********************************************************************
Function:	This function processes an RAF Request for changing the sync.
			bandwidth allocation. The frame is parsed (and validated
			in the process) and the information is sent to the synchronous
			bandwidth manager through the MAP.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine converts the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		The return value is the reason code. If the reason code is
			not RC_Success, the calling routine is responsible for sending
			the RDF. For missing parameters, the reason code
			IllegalOperation is used because this error is not described
			by the SMT document.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
SMTMessage       msg;					/* message to send */
SbaChangeType    change;				/* change allocation parsing info */
uInt32           result;				/* return value */
uInt16           found;					/* which params found */
uInt16           pos;					/* position in info field */


	result = RC_Success;
	found = 0;

	/*
	 * Get parameters from frame header.
	 */
	MCopyAddress (change.address, frameHdr->macHdr.SA);
	change.transactionID = Mntohl (frameHdr->smtHdr.Transaction_ID);

	/*
	 * Loop through InfoField and parase parameters.
	 */
	pos = 0;
	param = info;
	while (pos < Mntohs (frameHdr->smtHdr.InfoField_Length))
	{
		switch (Mntohs (param->paramType))
		{
		case fddiPATHIndex:
			change.pathIndex = Mntohs (param->PATHINDEX);
			found |= GOT_PATH_INDEX;
			break;

		case fddiPATHSbaPayload:
			change.payload = Mntohl (param->PATHPARAM32);
			found |= GOT_PAYLOAD;
			break;

		case fddiPATHSbaOverhead:
			change.overhead = Mntohl (param->PATHPARAM32);
			found |= GOT_OVERHEAD;
			break;

		case SBA_CATEGORY_PARAM_TYPE:
			change.category = Mntohl (param->SMTPARAM32);
			found |= GOT_CATEGORY;
			break;

		default:
			/* ignore parameter */
			break;
		}


		/*
		 * Move position in frame
		 */
		pos += (Mntohs (param->paramLen) + SIZE_TLVHdrType);
		param = MADDPTR (TLVParamType, info, pos);
	}

	/*
	 * If all parameters were found,
	 */
	if (found == GOT_CHANGE_REQ)
	{
		/*
		 * Send information to MAP
		 */
		msg.destination = MAP_MSG_ID;
		msg.source = FBM_MSG_ID;
		msg.type = FBM_EVENT_SBA;
		msg.typeInfo = SBA_CHNG_REQ_RX;
		msg.localID = 0;
		msg.entity = MACNum;
		msg.stnID = stnID;
		msg.len1 = SIZE_SbaChangeType;
		msg.len2 = 0;
		msg.p1.sba.change = &change;
		SendFBMMessage (&msg);
	}

	else
	{
		/*
		 * Return error code
		 */
		result = RC_IllegalOperation;
	}

	return (result);
}

static void
#ifdef __STDC__
ProcessSBAChangeResponse (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum, TLVParamType * info)
#else
ProcessSBAChangeResponse (stnID, frameHdr, MACNum, info)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
		TLVParamType    *info;
#endif
/*********************************************************************
Function:	This function processes an RAF Response for changing the sync.
			bandwidth allocation. The frame is parsed (and validated
			in the process) and the information is sent to the synchronous
			bandwidth manager through the MAP.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine converts the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		None.
Notes:		The SMT document (7.3) indicated that both paths should be
			reported in the Change Response frame. This cannot be done
			because the category parameter has no path information which
			makes it position dependent within the frame. Since the order
			of frame parameters is not guaranteed, correctly linking
			multiple categories to multiple paths is not possible.
				The intent of this frame is to report the new allocation
			in use for the ONE path changed. Therefore, this code assumes
			that only one path will be listed in this frame.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
SMTMessage       msg;					/* message to send */
SbaChangeType    change;				/* change allocation parsing info */
uInt16           found;					/* which params found */
uInt16           pos;					/* position in info field */


	found = 0;

	/*
	 * Get parameters from frame header.
	 */
	MCopyAddress (change.address, frameHdr->macHdr.SA);
	change.transactionID = Mntohl (frameHdr->smtHdr.Transaction_ID);

	/*
	 * Loop through InfoField and parase parameters.
	 */
	pos = 0;
	param = info;
	while (pos < Mntohs (frameHdr->smtHdr.InfoField_Length))
	{
		switch (Mntohs (param->paramType))
		{
		case fddiPATHIndex:
			change.pathIndex = Mntohs (param->PATHINDEX);
			found |= GOT_PATH_INDEX;
			break;

		case fddiPATHSbaPayload:
			change.payload = Mntohl (param->PATHPARAM32);
			found |= GOT_PAYLOAD;
			break;

		case fddiPATHSbaOverhead:
			change.overhead = Mntohl (param->PATHPARAM32);
			found |= GOT_OVERHEAD;
			break;

		case SBA_CATEGORY_PARAM_TYPE:
			change.category = Mntohl (param->SMTPARAM32);
			found |= GOT_CATEGORY;
			break;

		default:
			/* ignore parameter */
			break;
		}

		/*
		 * Move position in frame
		 */
		pos += (Mntohs (param->paramLen) + SIZE_TLVHdrType);
		param = MADDPTR (TLVParamType, info, pos);
	}

	/*
	 * If all parameters were found,
	 */
	if (found == GOT_CHANGE_RSP)
	{
		/*
		 * Send information to MAP
		 */
		msg.destination = MAP_MSG_ID;
		msg.source = FBM_MSG_ID;
		msg.type = FBM_EVENT_SBA;
		msg.typeInfo = SBA_CHNG_RSP_RX;
		msg.localID = 0;
		msg.entity = MACNum;
		msg.stnID = stnID;
		msg.len1 = SIZE_SbaChangeType;
		msg.len2 = 0;
		msg.p1.sba.change = &change;
		SendFBMMessage (&msg);
	}

	return;
}

static           uInt32
#ifdef __STDC__
ProcessSBAReportRequest (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum)
#else
ProcessSBAReportRequest (stnID, frameHdr, MACNum)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
#endif
/*********************************************************************
Function:	This function processes an RAF Request for the synchronous
			bandwidth allocation report. This routine builds and sends
			the response frame.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		The return value is the reason code. If the reason code is
			not RC_Success, the calling routine is responsible for sending
			the RDF.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt32           result;				/* return value */
uInt32           command;				/* command param to builder function */

	/*
	 * Clear frame buffer. The report response consists of up to 8
	 * parameters. This code assumes each parameter has a header, an index
	 * and a value. This is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(8 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	BuildSMTHeaderMulti (stnID, fbmFrameHeader, SMT_INFO_FC,
		frameHdr->macHdr.SA, MACNum, RAF_CLASS,
		SMTRESPONSE, Mntohl (frameHdr->smtHdr.Transaction_ID));

	/*
	 * Locate start of info field
	 */
	param = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/*
	 * First put in primary path including the resource type and command
	 * parameters.
	 */
	command = SBA_Report_Allocation;
	result = SetSBAParams (stnID, param, PI_primary, command, &len);

	if (result == 0)
	{
		/*
		 * If successful, set frame length, move pointer and clear command
		 * parameter. Otherwise these value remain as is for the secondary
		 * path.
		 */
		frameLen = len;
		command = 0;
		param = MADDPTR (TLVParamType, param, len);
	}

	/*
	 * Now try the secondary path.
	 */
	result = SetSBAParams (stnID, param, PI_secondary, command, &len);

	if (result == 0)
	{
		/*
		 * If successful, add to the frame length.
		 */
		frameLen += len;
		result = RC_Success;
	}

	else
	{
		/*
		 * Otherwise, if primary was successful, clear result value.
		 */
		if (command == 0)
			result = RC_Success;

		else
			/*
			 * Set error code for return value.
			 */
			result = RC_IllegalOperation;
	}

	/*
	 * If frame building was successful
	 */
	if (result == RC_Success)
	{
		/*
		 * Send the frame
		 */
		fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
		SendSMTFrameMulti (stnID, fbmFrameBuffer,
			frameLen + SMT_FRAME_HDR_SIZE, MACNum);

	}

	return (result);
}

static void
#ifdef __STDC__
ProcessSBAReportResponse (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 MACNum, TLVParamType * info)
#else
ProcessSBAReportResponse (stnID, frameHdr, MACNum, info)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           MACNum;
		TLVParamType    *info;
#endif
/*********************************************************************
Function:	This function processes an RAF Response for the synchronous
			bandwidth allocation report. The frame is parsed (and validated
			in the process) and the information is sent to the synchronous
			bandwidth manager through the MAP.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of buffer containing frame.
			MACNum		= MAC index which received the frame.
			info		= pointer to start of info field.
Input:		frameHdr	= header to RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine converts the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		None.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
SMTMessage       msg;					/* message to send */
SbaRptType       report;				/* report response parsing info */
uInt16           found;					/* which params found */
uInt16           pos;					/* position in info field */


	found = 0;

	/*
	 * Get parameters from frame header.
	 */
	MCopyAddress (report.address, frameHdr->macHdr.SA);
	report.transactionID = Mntohl (frameHdr->smtHdr.Transaction_ID);

	/* mark paths as not reported */
	report.pathIndex1 = 0;
	report.pathIndex2 = 0;

	/*
	 * Loop through InfoField and parase parameters.
	 */
	pos = 0;
	param = info;
	while (pos < Mntohs (frameHdr->smtHdr.InfoField_Length))
	{
		switch (Mntohs (param->paramType))
		{
		case fddiPATHIndex:
			if (Mntohs (param->PATHINDEX) == PI_primary)
				report.pathIndex1 = Mntohs (param->PATHINDEX);
			else
				report.pathIndex2 = Mntohs (param->PATHINDEX);
			found |= GOT_PATH_INDEX;
			break;

		case fddiPATHSbaPayload:
			if (Mntohs (param->PATHINDEX) == PI_primary)
				report.payload1 = Mntohl (param->PATHPARAM32);
			else
				report.payload2 = Mntohl (param->PATHPARAM32);
			found |= GOT_PAYLOAD;
			break;

		case fddiPATHSbaOverhead:
			if (Mntohs (param->PATHINDEX) == PI_primary)
				report.overhead1 = Mntohl (param->PATHPARAM32);
			else
				report.overhead2 = Mntohl (param->PATHPARAM32);
			found |= GOT_OVERHEAD;
			break;

		default:
			/* ignore parameter */
			break;
		}

		/*
		 * Move position in frame
		 */
		pos += (Mntohs (param->paramLen) + SIZE_TLVHdrType);
		param = MADDPTR (TLVParamType, info, pos);
	}

	/*
	 * If all parameters were found,
	 */
	if (found == GOT_REPORT_RSP)
	{
		/*
		 * Send information to MAP
		 */
		msg.destination = MAP_MSG_ID;
		msg.source = FBM_MSG_ID;
		msg.type = FBM_EVENT_SBA;
		msg.typeInfo = SBA_RPT_RSP_RX;
		msg.localID = 0;
		msg.entity = MACNum;
		msg.stnID = stnID;
		msg.len1 = SIZE_SbaRptType;
		msg.len2 = 0;
		msg.p1.sba.report = &report;
		SendFBMMessage (&msg);
	}

	return;
}

static           uInt32
#ifdef __STDC__
ProcessSBARequest (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum)
#else
ProcessSBARequest (stnID, memPtr, frameLen, MACNum)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
#endif
/*********************************************************************
Function:	This function processes an RAF Request for the synchronous
			bandwidth resource type. The frame is parsed (and validated
			in the process) and the request is processed as follows:
				Request allocation	=> pass to MAP
				Change allocation	=> pass to MAP
				Report allocation	=> process and send response
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		This routine may convert the contents of the frame's
			InfoField from network byte order to host byte order.
Return:		The return value is the reason code. If the reason code is
			not RC_Success, the calling routine is responsible for sending
			the RDF.
Modification History:
*********************************************************************/
{
SBACommandParamType *cmd;				/* SBA command parameter */
uInt32           result;				/* processing result */
TLVParamType    *info;					/* pointer to start of info field */

	/*
	 * Locate command to process.
	 */
	info = MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType);
	cmd = (SBACommandParamType *) FindParam (SBA_COMMAND_PARAM_TYPE,
		info, (frameLen - SIZE_SMTFrameHdrType), NET_ORDER);
	if (cmd)
	{
		/*
		 * Select command procesing.
		 */
		if (Mntohl (cmd->SbaCommand) == SBA_Request_Allocation)
		{
			/* Parse frame */
			result = ProcessSBAAllocRequest (stnID, (SMTFrameHdrType *) memPtr,
				MACNum, info);
		}

		else if (Mntohl (cmd->SbaCommand) == SBA_Change_Allocation)
		{
			/* Parse frame */
			result = ProcessSBAChangeRequest (stnID, (SMTFrameHdrType *) memPtr,
				MACNum, info);
		}

		else if (Mntohl (cmd->SbaCommand) == SBA_Report_Allocation)
		{
			/* Build response */
			result = ProcessSBAReportRequest (stnID, (SMTFrameHdrType *) memPtr,
				MACNum);
		}

		else
		{
			/* unknown command */
			result = RC_IllegalOperation;
		}
	}

	return (result);
}

static void
#ifdef __STDC__
ProcessSBAResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum)
#else
ProcessSBAResponse (stnID, memPtr, frameLen, MACNum)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
#endif
/*********************************************************************
Function:	This function processes an RAF Response. If the
			RAF does not fall into any resources supported by the
			XLNT Manager, the frame is passed to the interface
			routine LocalRAFResponseMulti(). This allows
			implementers to add their own resource allocation
			routines.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
SBACommandParamType *cmd;				/* SBA command parameter */
TLVParamType    *info;					/* pointer to start of InfoField */


	/*
	 * Locate command to process.
	 */
	info = MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType);
	cmd = (SBACommandParamType *) FindParam (SBA_COMMAND_PARAM_TYPE,
		info, (frameLen - SIZE_SMTFrameHdrType), NET_ORDER);
	if (cmd)
	{
		/*
		 * Select command procesing.
		 */
		if (Mntohl (cmd->SbaCommand) == SBA_Request_Allocation)
		{
			/* Parse frame */
			ProcessSBAAllocResponse (stnID,
				(SMTFrameHdrType *) memPtr, MACNum, info);
		}

		else if (Mntohl (cmd->SbaCommand) == SBA_Change_Allocation)
		{
			/* Parse frame */
			ProcessSBAChangeResponse (stnID,
				(SMTFrameHdrType *) memPtr, MACNum, info);
		}

		else if (Mntohl (cmd->SbaCommand) == SBA_Report_Allocation)
		{
			/* Parse frame */
			ProcessSBAReportResponse (stnID,
				(SMTFrameHdrType *) memPtr, MACNum, info);
		}
	}

	return;
}


/*********************************************************************
	SBA Transmit Frame Processing Routines
*********************************************************************/

static           uInt16
#ifdef __STDC__
SendSBAReportRequest (SMTMessage * msg, uInt16 outputMAC)
#else
SendSBAReportRequest (msg, outputMAC)
		SMTMessage      *msg;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP that requests
			a Report Allocation command request frame be sent.
Parameters:	msg			= address of buffer containing message.
			outputMAC	= array index of MAC to send frame.
Input:		msg			= message describing frame to build and the contents
							of the frame, if needed.
Output:		Sends frame.
Return:		0						frame was built and queued successfully
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt16           result;				/* return value */

	/*
	 * Clear frame buffer. The report request consists of 2 parameters. This
	 * code assumes each parameter has a header, an index and a value. This
	 * is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(2 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	msg->p1.sba.report->transactionID =
		BuildSMTHeaderMulti (msg->stnID, fbmFrameHeader, SMT_INFO_FC,
		msg->p1.sba.report->address, outputMAC, RAF_CLASS,
		SMTREQUEST, msg->p1.sba.report->transactionID);

	/*
	 * Locate start of info field
	 */
	param = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/*
	 * Resource Type parameter
	 */
	len = ResourceType_Param ((ResourceTypeParamType *) param, RESOURCE_SB);
	frameLen += len;
	param = MADDPTR (TLVParamType, param, len);

	/*
	 * SBA Command parameter
	 */
	len = SBACommand_Param ((SBACommandParamType *) param,
		SBA_Report_Allocation);
	frameLen += len;

	/*
	 * Send the frame
	 */
	fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
	SendSMTFrameMulti (msg->stnID, fbmFrameBuffer,
		frameLen + SMT_FRAME_HDR_SIZE, outputMAC);
	result = 0;

	return (result);
}

static           uInt16
#ifdef __STDC__
SendSBAAllocRequest (SMTMessage * msg, uInt16 outputMAC)
#else
SendSBAAllocRequest (msg, outputMAC)
		SMTMessage      *msg;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP for
			a Request Allocation command request frame be sent.
Parameters:	msg			= address of buffer containing message.
			outputMAC	= array index of MAC to send frame.
Input:		msg			= message describing frame to build and the contents
							of the frame, if needed.
Output:		Sends frame.
Return:		0						frame was built and queued successfully
			EFBM_SBA_NOT_AVAILABLE	SBA not supported on requested path
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt16           result;				/* return value */

	/*
	 * Clear frame buffer. The report request consists of 11 parameters. This
	 * code assumes each parameter has a header, an index and a value. This
	 * is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(11 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	msg->p1.sba.allocReq->transactionID =
		BuildSMTHeaderMulti (msg->stnID, fbmFrameHeader, SMT_INFO_FC,
		msg->p1.sba.allocReq->address, outputMAC, RAF_CLASS,
		SMTREQUEST, msg->p1.sba.allocReq->transactionID);

	/*
	 * Locate start of info field
	 */
	param = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/*
	 * Put in standard parameters
	 */
	result = SetSBAParams (msg->stnID, param, msg->p1.sba.allocReq->pathIndex,
		SBA_Request_Allocation, &len);

	if (result == 0)
	{
		/*
		 * Set frame length and move pointer
		 */
		frameLen = len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set requested payload
		 */
		len = SBAPayload_Param ((SBAPayloadParamType *) param,
			msg->p1.sba.allocReq->requestPayload);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set requested overhead
		 */
		len = SBAOverhead_Param ((SBAOverheadParamType *) param,
			msg->p1.sba.allocReq->requestOverhead);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set allocation address
		 */
		len = SBAAllocAddr_Param ((SBAAllocAddrParamType *) param,
			msg->p1.sba.allocReq->allocAddress);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set category
		 */
		len = SBACategory_Param ((SBACategoryParamType *) param,
			msg->p1.sba.allocReq->category);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set Max T_Neg
		 */
		len = MaxT_Neg_Param ((MaxTnegParamType *) param,
			msg->p1.sba.allocReq->maxTNeg);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set min segment
		 */
		len = MinSegment_Param ((MinSegmentParamType *) param,
			msg->p1.sba.allocReq->minSegment);
		frameLen += len;

		/*
		 * Send the frame
		 */
		fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
		SendSMTFrameMulti (msg->stnID, fbmFrameBuffer,
			frameLen + SMT_FRAME_HDR_SIZE, outputMAC);
	}

	return (result);
}

static           uInt16
#ifdef __STDC__
SendSBAAllocResponse (SMTMessage * msg, uInt16 outputMAC)
#else
SendSBAAllocResponse (msg, outputMAC)
		SMTMessage      *msg;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP for
			a Request Allocation command response frame be sent.
Parameters:	msg			= address of buffer containing message.
			outputMAC	= array index of MAC to send frame.
Input:		msg			= message describing frame to build and the contents
							of the frame, if needed.
Output:		Sends frame.
Return:		0						frame was built and queued successfully
			EFBM_SBA_NOT_AVAILABLE	SBA not supported on requested path
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt16           result;				/* return value */

	/*
	 * Clear frame buffer. The report request consists of 11 parameters. This
	 * code assumes each parameter has a header, an index and a value. This
	 * is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(11 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	msg->p1.sba.allocRsp->transactionID =
		BuildSMTHeaderMulti (msg->stnID, fbmFrameHeader, SMT_INFO_FC,
		msg->p1.sba.allocRsp->address, outputMAC, RAF_CLASS,
		SMTRESPONSE, msg->p1.sba.allocRsp->transactionID);

	/*
	 * Locate start of info field
	 */
	param = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/*
	 * Put in standard parameters
	 */
	result = SetSBAParams (msg->stnID, param, msg->p1.sba.allocRsp->pathIndex,
		SBA_Request_Allocation, &len);

	if (result == 0)
	{
		/*
		 * Set frame length and move pointer
		 */
		frameLen = len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set reason code
		 */
		len = ReasonCode_Param ((ReasonCodeParamType *) param,
			msg->p1.sba.allocRsp->reasonCode);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set allocation address
		 */
		len = SBAAllocAddr_Param ((SBAAllocAddrParamType *) param,
			msg->p1.sba.allocRsp->allocAddress);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set category
		 */
		len = SBACategory_Param ((SBACategoryParamType *) param,
			msg->p1.sba.allocRsp->category);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set allocatable
		 */
		len = SBAAllocatable_Param ((SBAAllocatableParamType *) param,
			msg->p1.sba.allocRsp->allocatable);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Send the frame
		 */
		fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
		SendSMTFrameMulti (msg->stnID, fbmFrameBuffer,
			frameLen + SMT_FRAME_HDR_SIZE, outputMAC);
	}

	return (result);
}

static           uInt16
#ifdef __STDC__
SendSBAChangeRequest (SMTMessage * msg, uInt16 outputMAC)
#else
SendSBAChangeRequest (msg, outputMAC)
		SMTMessage      *msg;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP for
			a Change Allocation command request frame be sent.
Parameters:	msg			= address of buffer containing message.
			outputMAC	= array index of MAC to send frame.
Input:		msg			= message describing frame to build and the contents
							of the frame, if needed.
Output:		Sends frame.
Return:		0						frame was built and queued successfully
			EFBM_SBA_NOT_AVAILABLE	SBA not supported on requested path
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
TLVParamType    *info;					/* pointer to start of InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt16           result;				/* return value */

	/*
	 * Clear frame buffer. The report request consists of 11 parameters. This
	 * code assumes each parameter has a header, an index and a value. This
	 * is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(11 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	msg->p1.sba.change->transactionID =
		BuildSMTHeaderMulti (msg->stnID, fbmFrameHeader, SMT_INFO_FC,
		msg->p1.sba.change->address, outputMAC, RAF_CLASS,
		SMTREQUEST, msg->p1.sba.change->transactionID);

	/*
	 * Locate start of info field
	 */
	info = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;
	param = info;

	/*
	 * Put in standard parameters
	 */
	result = SetSBAParams (msg->stnID, param, msg->p1.sba.change->pathIndex,
		SBA_Change_Allocation, &len);

	if (result == 0)
	{
		/*
		 * Set frame length and move pointer
		 */
		frameLen = len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set category
		 */
		len = SBACategory_Param ((SBACategoryParamType *) param,
			msg->p1.sba.change->category);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Now locate the payload and overhead parameters and set their
		 * values to the new values the destination station should use.
		 */
		param = (TLVParamType *) FindParam (fddiPATHSbaPayload,
			info, frameLen, NET_ORDER);
		if (param)						/* should be found, but just in case */
		{
			param->PATHPARAM32 = Mhtonl (msg->p1.sba.change->payload);
		}

		param = (TLVParamType *) FindParam (fddiPATHSbaOverhead,
			info, frameLen, NET_ORDER);
		if (param)						/* should be found, but just in case */
		{
			param->PATHPARAM32 = Mhtonl (msg->p1.sba.change->overhead);
		}

		/*
		 * Send the frame
		 */
		fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
		SendSMTFrameMulti (msg->stnID, fbmFrameBuffer,
			frameLen + SMT_FRAME_HDR_SIZE, outputMAC);
	}

	return (result);
}

static           uInt16
#ifdef __STDC__
SendSBAChangeResponse (SMTMessage * msg, uInt16 outputMAC)
#else
SendSBAChangeResponse (msg, outputMAC)
		SMTMessage      *msg;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP for
			a Change Allocation command response frame be sent.
Parameters:	msg			= address of buffer containing message.
			outputMAC	= array index of MAC to send frame.
Input:		msg			= message describing frame to build and the contents
							of the frame, if needed.
Output:		Sends frame.
Return:		0						frame was built and queued successfully
			EFBM_SBA_NOT_AVAILABLE	SBA not supported on requested path
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* parameter in InfoField */
uInt16           len;					/* parameter length added to frame */
uInt16           frameLen;				/* accumulated InfoField length */
uInt16           result;				/* return value */

	/*
	 * Clear frame buffer. The report request consists of 11 parameters. This
	 * code assumes each parameter has a header, an index and a value. This
	 * is more than enough to cover the frame size.
	 * 
	 */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_SMTFrameHdrType \
			+(11 * (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))));

	/*
	 * Build frame header.
	 */
	msg->p1.sba.change->transactionID =
		BuildSMTHeaderMulti (msg->stnID, fbmFrameHeader, SMT_INFO_FC,
		msg->p1.sba.change->address, outputMAC, RAF_CLASS,
		SMTRESPONSE, msg->p1.sba.change->transactionID);

	/*
	 * Locate start of info field
	 */
	param = MADDPTR (TLVParamType, fbmFrameBuffer, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/*
	 * Put in standard parameters
	 */
	result = SetSBAParams (msg->stnID, param, msg->p1.sba.change->pathIndex,
		SBA_Change_Allocation, &len);

	if (result == 0)
	{
		/*
		 * Set frame length and move pointer
		 */
		frameLen = len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Set category
		 */
		len = SBACategory_Param ((SBACategoryParamType *) param,
			msg->p1.sba.change->category);
		frameLen += len;
		param = MADDPTR (TLVParamType, param, len);

		/*
		 * Send the frame
		 */
		fbmFrameHeader->smtHdr.InfoField_Length = Mhtons (frameLen);
		SendSMTFrameMulti (msg->stnID, fbmFrameBuffer,
			frameLen + SMT_FRAME_HDR_SIZE, outputMAC);
	}

	return (result);
}

void
#ifdef __STDC__
MonitorNIFSBA (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
MonitorNIFSBA (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function checks the Station State parameter in an NIF.
			If the Synchronous service bit is set, then a message
			is sent to the MAP. This allows a management application
			to passively monitor which stations are reporting synchronous
			service in use.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= total length of frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= NIF frame received.
			fbmStationData	= current setting of monitoring mode.
Output:		Sends message to MAP if synchronous service bit is set.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* message to send */
StationStateParamType *ssp;				/* station state param format */

	if (FBMDATA (stnID, fbmStationData.monitorMode) & FMM_MONITOR_SBA)
	{
		/* locate Station State parameter */
		ssp = (StationStateParamType *) FindParam (STATION_STATE_PARAM_TYPE,
			MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
			frameLen - SIZE_SMTFrameHdrType, NET_ORDER);
		if (ssp)
		{
			/* if this station is reporting SBA in use */
			if (ssp->Topology & Topology_SBA)
			{
				/* report MAC address to MAP */
				msg.destination = MAP_MSG_ID;
				msg.source = FBM_MSG_ID;
				msg.type = FBM_EVENT_SBA_CONTROL;
				msg.typeInfo = SBA_MONITOR_NIF;
				msg.localID = 0;
				msg.entity = FBMDATA (stnID, fbmMACData[MACNum].Index);
				msg.stnID = stnID;
				MCopyAddress (msg.data.addr, ((MACHdrType *) memPtr)->SA);
				msg.len1 = 0;
				msg.len2 = 0;
				SendFBMMessage (&msg);
			}
		}
	}

	return;
}

void
#ifdef __STDC__
ProcessSBAMessage (SMTMessage * msg)
#else
ProcessSBAMessage (msg)
		SMTMessage      *msg;
#endif
/*********************************************************************
Function:	This function processes a message from the MAP to generate
			a synchronous bandwidth frame.
Parameters:	msg		= address of buffer containing message.
Input:		msg		= message describing frame to build and the contents
						of the frame, if needed.
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
uInt16           outputMAC;				/* internal MAC index to send frame */
uInt16           result;				/* return code from send routines */

	if (msg->type == FBM_ACTION_SBA)
	{
		/*
		 * Get internal MAC array index
		 */
		result = GetMACIndexMulti (msg->stnID, msg->entity, &outputMAC);
		if (result == 0)
		{
			/*
			 * Select message processing.
			 */
			switch (msg->typeInfo)
			{
			case SBA_RPT_REQ_TX:
				result = SendSBAReportRequest (msg, outputMAC);
				break;

			case SBA_ALLOC_REQ_TX:
				result = SendSBAAllocRequest (msg, outputMAC);
				break;

			case SBA_ALLOC_RSP_TX:
				result = SendSBAAllocResponse (msg, outputMAC);
				break;

			case SBA_CHNG_REQ_TX:
				result = SendSBAChangeRequest (msg, outputMAC);
				break;

			case SBA_CHNG_RSP_TX:
				result = SendSBAChangeResponse (msg, outputMAC);
				break;

			default:
				result = EFBM_SBA_BAD_COMMAND;
				break;
			}
		}

		if (!result)
		{
			/*
			 * Frame built and queued
			 */
			msg->destination = msg->source;
			msg->source = FBM_MSG_ID;
			msg->type = FBM_RESPONSE_SBA;
		}

		else
		{
			/*
			 * Send error message.
			 */
			msg->destination = msg->source;
			msg->source = FBM_MSG_ID;
			msg->type = FBM_ERROR_MSG;
			msg->typeInfo = result;
		}
	}

	else if (msg->type == FBM_ACTION_SBA_CONTROL)
	{
		/*
		 * Set SBA monitoring mode.
		 */
		FBMDATA (msg->stnID, fbmStationData.monitorMode)
			= (msg->data.b32 == ENABLE)
		/* turn on monitoring */
			? FBMDATA (msg->stnID, fbmStationData.monitorMode)
			| FMM_MONITOR_SBA
		/* turn off monitoring */
			: FBMDATA (msg->stnID, fbmStationData.monitorMode)
			& ~FMM_MONITOR_SBA;

		/*
		 * Send reponse message.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_RESPONSE_SBA_CONTROL;
	}

	else
	{
		/*
		 * Send error message.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_ERROR_MSG;
		msg->typeInfo = EFBM_MSG_FAILED;
	}

	SendFBMMessage (msg);

	return;
}


/*********************************************************************
	RAF Processing Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessRAFRequest (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessRAFRequest (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an RAF Request. If the
			RAF does not fall into any resources supported by the
			XLNT Manager, the frame is passed to the interface
			routine LocalRAFRequestMulti(). This allows
			implementers to add their own resource allocation
			routines.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
uInt32           result;				/* reason code from I/F routine */
uInt16           len;					/* RDF frame length */
ResourceTypeParamType *rsrc;			/* resource parameter */

	/*
	 * Locate resource to process.
	 */
	result = RC_FrameClassNotSupported;
	rsrc = (ResourceTypeParamType *) FindParam (RESOURCE_TYPE_PARAM_TYPE,
		MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
		frameLen - SIZE_SMTFrameHdrType, NET_ORDER);
	if (rsrc)
	{
		/*
		 * Process resource type.
		 */
		if (Mntohl (rsrc->ResourceType) == RESOURCE_SB)
		{
			/*
			 * Process synchronous bandwidth request.
			 */
			result = ProcessSBARequest (stnID, memPtr, frameLen, MACNum);
		}

		else
		{
			/*
			 * Unsupported resource, pass to interface.
			 */
			result = LocalRAFRequestMulti (stnID, memPtr, frameLen,
				MACNum, EACbits);
		}
	}

	else
	{
		/*
		 * Could not locate resource type. Invalid frame format. Because
		 * there is no specification for what reason code to use in this
		 * case, XLNT Designs is using IllegalOperation.
		 */
		result = RC_IllegalOperation;
	}

	if (result != RC_Success)
	{
		/* clear buffer for the response frame */
		MClearFrameBuffer (fbmFrameBuffer, (SIZE_RDF_HDR + frameLen));

		/* build RDF frame */
		BuildRDFResponse (stnID, memPtr, fbmFrameBuffer, MACNum, result,
			frameLen);

		/* send frame */
		len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
			+ SMT_FRAME_HDR_SIZE;
		SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);
	}

	return;
}

void
#ifdef __STDC__
ProcessRAFResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessRAFResponse (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an RAF Response. If the
			RAF does not fall into any resources supported by the
			XLNT Manager, the frame is passed to the interface
			routine LocalRAFResponseMulti(). This allows
			implementers to add their own resource allocation
			routines.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
ResourceTypeParamType *rsrc;			/* resource parameter */

	/*
	 * Locate resource to process.
	 */
	rsrc = (ResourceTypeParamType *) FindParam (RESOURCE_TYPE_PARAM_TYPE,
		MADDPTR (TLVParamType, memPtr, SIZE_SMTFrameHdrType),
		frameLen - SIZE_SMTFrameHdrType, NET_ORDER);
	if (rsrc)
	{
		/*
		 * Process resource type.
		 */
		if (Mntohl (rsrc->ResourceType) == RESOURCE_SB)
		{
			/*
			 * Process synchronous bandwidth request.
			 */
			ProcessSBAResponse (stnID, memPtr, frameLen, MACNum);
		}

		else
		{
			/*
			 * Unsupported resource, pass to interface.
			 */
			LocalRAFResponseMulti (stnID, memPtr, frameLen, MACNum, EACbits);
		}
	}

	return;
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
BuildRAFRequest (Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC, uInt32 transID,
	uChar * info, uInt16 infoLen)
#else
BuildRAFRequest (frameBuffer, fcField, destAddr, outputMAC, transID,
	info, infoLen)
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		uChar           *info;
		uInt16           infoLen;
#endif
{
	return (BuildRAFRequestMulti (FIRST_STATION, frameBuffer, fcField,
			destAddr, outputMAC, transID, info, infoLen));
}

uInt32
#ifdef __STDC__
BuildRAFResponse (Void_p frameBuffer, FDDILongAddressType destAddr,
	uInt16 outputMAC, uInt32 transID, uChar * info, uInt16 infoLen)
#else
BuildRAFResponse (frameBuffer, destAddr, outputMAC, transID, info, infoLen)
		Void_p           frameBuffer;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		uChar           *info;
		uInt16           infoLen;
#endif
{
	return (BuildRAFResponseMulti (FIRST_STATION, frameBuffer, destAddr,
			outputMAC, transID, info, infoLen));
}

#endif							/* MULTIPLE_STATION */


/*
 * If XM_USE_RAF is not defined, then the basic processing of RAFs is
 * included. This processing denies all request frames and drops
 * all other frames.
 *
 * This protocol DOES NOT pass frames to the local processing routines.
 * Therefore, the routines in fbmrafif.c do not need to be provided at all.
 */
#else							/* XM_USE_RAF */


/*********************************************************************
	Default RAF Processing Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessRAFRequest (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessRAFRequest (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an RAF Request by simply
			denying the request frame. Because no protocols using RAFs
			are supported, the request must be denied.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
uInt16           len;					/* RDF frame length */

	/* clear buffer for the response frame */
	MClearFrameBuffer (fbmFrameBuffer, (SIZE_RDF_HDR + frameLen));

	/* build RDF frame */
	BuildRDFResponse (stnID, memPtr, fbmFrameBuffer, MACNum,
		RC_FrameClassNotSupported, frameLen);

	/* send frame */
	len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
		+ SMT_FRAME_HDR_SIZE;
	SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

	return;
}

void
#ifdef __STDC__
ProcessRAFResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessRAFResponse (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an RAF Response by simply
			ignoring the frame. Because no protocols using RAFs are
			supported, nothing needs to be done. The parameters
			are passed to this routine to maintain interface
			compatibility with the routine of the same name if
			XM_USE_RAF is defined.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete RAF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{

	/*
	 * No supported resources, drop frame.
	 */
	return;
}

#endif							/* XM_USE_RAF */
