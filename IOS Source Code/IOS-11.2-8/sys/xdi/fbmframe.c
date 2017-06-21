/* $Id: fbmframe.c,v 3.2.56.3 1996/07/17 19:16:27 vravi Exp $
 * $Source: /release/112/cvs/Xsys/xdi/fbmframe.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmframe.c,v $
 * Revision 3.2.56.3  1996/07/17  19:16:27  vravi
 * The NIFRequest routine always calls the SMTHeader routine with
 * NIFREQUEST parameter. If the transaction ID of the incoming NIF request
 * is zero, the change made ensures that the SMTHeader routine will not
 * randomize the transactionID.
 *
 * CSCdi58069:  Router responds with the incorrect FDDI transaction ID
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/03/22  00:15:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.56.1  1996/03/18  22:51:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.22.1  1996/03/07  11:20:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.2.1  1995/12/23  04:42:21  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2  1995/11/17  18:09:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmframe.c xm7_v3.1.1:cset.0166:11:11 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Build Frame Module

		File:			xm7_fbmframe.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		Frame Based Management for SMT.  This module contains routines
		for building NIF, SIF, ECHO, RDF, and ESF frames.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
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
#include    "../if/if_fddi.h"


/*********************************************************************
	Send Frame Routines
*********************************************************************/

uInt32
#ifdef __STDC__
BuildSMTHeaderMulti (uInt16 stnID, SMTFrameHdrType * frameHdr, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uChar frameClass, uChar frameType, uInt32 transID)
#else
BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr, outputMAC,
	frameClass, frameType, transID)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uChar            frameClass;
		uChar            frameType;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function creates the MAC and SMT header portion of
		a request or announcement frame.
Parameters:	stnID		= index of station to use.
			frameHdr	= buffer to use to build the frame. This
							must point to the start of the three pad
							bytes at the start of the frame buffer.
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			frameClass	= SMT frame class identifier.
			frameType	= SMT frame type identifier.
			transID		= transaction ID to use.
input:		frameHdr	= MAC and SMT frame header location in
							the frame.
			destAddr	= 48-bit station ID of destination.
			transID		= If 0, then a new transaction ID is
							generated. Otherwise the value given is
							used. This value must be in host order.
Output:		frameHdr	= contains frame's MAC and SMT headers set
							except for the InfoField_Length.
Return:		Transaction ID is returned if frame headers was built.
			Otherwise 0 is returned.
Modification History:
*********************************************************************/
{
	/*
	 * Set MAC header.
	 */
	frameHdr->macHdr.FC = fcField;
	MCopyAddress (frameHdr->macHdr.DA, destAddr);
	MCopyAddress (frameHdr->macHdr.SA,
		FBMDATA (stnID, fbmMACData[outputMAC].SMTAddress));

	/*
	 * Set SMT header.
	 */
	frameHdr->smtHdr.Frame_Class = frameClass;
	frameHdr->smtHdr.Frame_Type = frameType;
	frameHdr->smtHdr.Version_ID
		= Mhtons (FBMDATA (stnID, fbmStationData.opVersionID));

	/*
	 * Generate transaction ID when none is given and not a response frame.
	 */
	if ((transID == 0) && (frameType != SMTRESPONSE))
		transID = GenTransID (stnID);
	frameHdr->smtHdr.Transaction_ID = Mhtonl (transID);
	MEMCOPY (&frameHdr->smtHdr.Station_ID,
		&FBMDATA (stnID, fbmStationData.stationID),
		SIZE_SMTStationIdType);

	return (transID);
}


/*********************************************************************
	Neighborhood Information Frames
*********************************************************************/

uInt32
#ifdef __STDC__
BuildNIFRequest (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	uChar * destAddr, uInt16 outputMAC, uInt32 transID)
#else
BuildNIFRequest (stnID, frameBuffer, fcField, destAddr, outputMAC, transID)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		uChar           *destAddr;
		uInt16           outputMAC;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function builds an NIF Request.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
Output:		frameBuffer	= contains completed frame.
Return:		Transaction ID is returned if frame was built. Otherwise
			0 is returned.
Modification History:
*********************************************************************/
{
Void_p           memPtr;				/* Buffer used to build frame */
SMTFrameHdrType *frameHdr;				/* SMT header */
uInt16           frameLen;				/* Total bytes in info field */
uInt16           paramLen;				/* Total bytes of a frame parameter */
uInt32           result;				/* value to return */
TLVParamType     mibData;				/* for MAC frame status capabilities */
FBMMACInfoType  *macPtr;				/* MAC element */

	if (fddi_smt_packet_debug)
		buginf("\nIn BuildNIFRequest");
	/*
	 * Initialize values.
	 */
	frameLen = 0;
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;
	macPtr = &(FBMDATA (stnID, fbmMACData[outputMAC]));

	/*
	 * Initialize frame header.
	 */
	if (fcField == SMT_INFO_FC) {
		result = BuildSMTHeaderMulti (stnID, frameHdr, fcField,
			destAddr, outputMAC, NIF_CLASS, SMTRESPONSE, transID);
	} else {
		result = BuildSMTHeaderMulti (stnID, frameHdr, fcField,
			destAddr, outputMAC, NIF_CLASS, SMTREQUEST, transID);
	}

	/* set constant version ID */
	frameHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, frameHdr, SIZE_SMTFrameHdrType);

	/* set upstream neighbor address */
	paramLen = UNA_Param ((UNAParamType *) memPtr, macPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* set station descriptor */
	paramLen = StationDescriptor_Param (stnID, (StationDescParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* set station state */
	paramLen = StationState_Param (stnID,
		(StationStateParamType *) memPtr, outputMAC);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* get fddiMACFrameStatusCapabilities */
	MEMZERO (&mibData, sizeof (mibData));	/* Clear TLV buffer */
	mibData.paramType = fddiMACFrameStatusFunctions;
	mibData.paramLen = 4;
	mibData.MACINDEX = outputMAC;
	if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
		== RC_Success)
	{
		paramLen = Attribute_Param ((TLVParamType *) memPtr, &mibData);
		frameLen += paramLen;
	}

	else
	{
		/* something is wrong */
		result = 0;
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

uInt32
#ifdef __STDC__
BuildNIFAnnouncement (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC, uInt32 transID)
#else
BuildNIFAnnouncement (stnID, frameBuffer, fcField, destAddr,
	outputMAC, transID)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function builds an NIF Announcement. Since the NIF
			Announcement uses the same format as the NIF Request,
			this routine calls BuildNIFRequestMulti() to build
			the frame and just changes the frame type.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
Output:		frameBuffer	= contains complete NIF announcment frame.
Return:		Transaction ID is returned if frame was built. Otherwise
		0 is returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* MAC header */
uInt32           result;				/* value to return */

	if (fddi_smt_packet_debug)
		buginf("\nIn BuildNIFAnnounce");
	/*
	 * Initialize values.
	 */
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;

	/*
	 * Use BuildNIFRequestMulti() to create frame.
	 */
	result = BuildNIFRequest (stnID, frameBuffer, fcField,
		destAddr, outputMAC, transID);

	/* set frame type to Announcment */
	frameHdr->smtHdr.Frame_Type = SMTANNOUNCE;

	/*
	 * Return transaction ID.
	 */
	return (result);
}

void
#ifdef __STDC__
BuildNIFResponse (uInt16 stnID, Void_p requestFrame,
	Void_p responseBuffer, uInt16 outputMAC)
#else
BuildNIFResponse (stnID, requestFrame, responseBuffer, outputMAC)
		uInt16           stnID;
		Void_p           requestFrame;
		Void_p           responseBuffer;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function builds an NIF Response. Since the NIF
			Response uses the same format as the NIF Request,
			this routine calls BuildNIFRequestMulti() to build
			the frame and just changes the frame type.
Parameters:	stnID			= index of station to use.
			requestFrame	= pointer to NIF Request buffer.
			responseBuffer	= pointer to buffer for response frame.
			outputMAC		= index of MAC to send frame out on.
Input:		requestFrame	= contains NIF Request Frame to respond to.
Output:		responseBuffer	= contains NIF Response Frame.
Return:		No value returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *reqSMTHdr;				/* SMT header in request frame */
SMTFrameHdrType *rspSMTHdr;				/* SMT header in response frame */

	if (fddi_smt_packet_debug)
		buginf("\nIn BuildNIFResponse");
	/*
	 * Get frame headers.
	 */
	reqSMTHdr = (SMTFrameHdrType *) requestFrame;
	rspSMTHdr = (SMTFrameHdrType *) responseBuffer;

	/*
	 * Initialize frame header.
	 */
	BuildNIFRequest (stnID, responseBuffer, SMT_INFO_FC,
		reqSMTHdr->macHdr.SA, outputMAC,
		Mntohl (reqSMTHdr->smtHdr.Transaction_ID));

	/* set frame type */
	rspSMTHdr->smtHdr.Frame_Type = SMTRESPONSE;

	return;
}


/*********************************************************************
	Station Information Configuration Frames
*********************************************************************/

uInt32
#ifdef __STDC__
BuildSIFConfigRequest (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID)
#else
BuildSIFConfigRequest (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function builds an SIF Configuration Request.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
Output:		frameBuffer	= contains completed frame.
Return:		Transaction ID is returned if frame was built. Otherwise
		0 is returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* SMT header */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;

	/*
	 * Initialize frame header.
	 */
	result = BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr,
		outputMAC, SIFCONFIG_CLASS, SMTREQUEST, transID);

	/* set constant version ID */
	frameHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/*
	 * Set frame length.
	 */
	frameHdr->smtHdr.InfoField_Length = 0;

	/*
	 * Return transaction ID.
	 */
	return (result);
}

void
#ifdef __STDC__
BuildSIFConfigResponse (uInt16 stnID, Void_p requestFrame,
	Void_p responseBuffer, uInt16 outputMAC)
#else
BuildSIFConfigResponse (stnID, requestFrame, responseBuffer, outputMAC)
		uInt16           stnID;
		Void_p           requestFrame;
		Void_p           responseBuffer;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function builds an SIF Configuration Response.
Parameters:	stnID			= index of station to use.
			requestFrame	= pointer to SIF Request buffer.
			responseBuffer	= pointer to buffer for response frame.
			outputMAC		= index of MAC to send frame out on.
Input:		requestFrame	= contains SIF Request Frame to respond to.
Output:		responseBuffer	= contains SIF Response Frame.
Return:		No value returned.
Notes:		This routine makes sure that the info length size does not
			exceed the allowable transmit length.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *reqSMTHdr;				/* SMT header in request frame */
SMTFrameHdrType *rspSMTHdr;				/* SMT header in response frame */
Void_p           memPtr;				/* parameter pointer in frame */
uInt16           frameLen;				/* Total bytes in info field */
uInt16           paramLen;				/* Total bytes of a frame parameter */
TLVParamType     mibData;				/* for Set Count */
uInt16           i;						/* loop counter */
FBMMACInfoType  *macPtr;				/* MAC element */
uInt16           spaceNeeded;			/* variable len parameter size */

	/*
	 * Get frame headers.
	 */
	reqSMTHdr = (SMTFrameHdrType *) requestFrame;
	rspSMTHdr = (SMTFrameHdrType *) responseBuffer;

	/*
	 * Initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspSMTHdr, SMT_INFO_FC,
		reqSMTHdr->macHdr.SA, outputMAC, SIFCONFIG_CLASS,
		SMTRESPONSE, Mntohl (reqSMTHdr->smtHdr.Transaction_ID));

	/* set constant version ID */
	rspSMTHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/* get start of info field */
	memPtr = MADDPTR (Void_t, rspSMTHdr, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/* time stamp */
	paramLen = MsgTimeStamp_Param (stnID, (MsgTimeStampParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* station descriptor */
	paramLen = StationDescriptor_Param (stnID, (StationDescParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* SMT supported versions */
	paramLen = SupportedVersions_Param (stnID,
		(SupportedVersionsParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* station state */
	paramLen = StationState_Param (stnID,
		(StationStateParamType *) memPtr, outputMAC);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* station policies */
	paramLen = StationPolicies_Param (stnID,
		(StationPoliciesParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* set count */
	MEMZERO (&mibData, sizeof (mibData));	/* Clear TLV buffer */
	mibData.paramType = fddiSMTSetCount;
	mibData.paramLen = 0;
	if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
		== RC_Success)
	{
		paramLen = Attribute_Param ((TLVParamType *) memPtr, &mibData);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);
	}

	/*
	 * The path latency parameter is not supported by the XLNT Manager.
	 */

	/*
	 * Check if remaining parameters will fit in frame.
	 */
	spaceNeeded = (FBMDATA (stnID, fbmStationData.macPresent)
		* (SIZE_MACNeighborsParamType + SIZE_PathMACRecordType))
		+ ((FBMDATA (stnID, fbmStationData.attachPresent)
			+ FBMDATA (stnID, fbmStationData.masterPresent))
		* SIZE_PathPORTRecordType)
		+ SIZE_TLVHdrType;
	if ((MAX_SMT_XMIT_LEN - frameLen) > spaceNeeded)
	{
		/* MAC neighbors */
		for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
			i < MAX_MAC_COUNT;
			i++, macPtr++)
		{
			/* only use physically present MACs */
			if (macPtr->hwPresent)
			{
				paramLen = MACNeighbors_Param
					((MACNeighborsParamType *) memPtr, macPtr);
				frameLen += paramLen;
				memPtr = MADDPTR (Void_t, memPtr, paramLen);
			}
		}

		/* path descriptor */
		paramLen = PathDescriptor_Param (stnID, (PathDescParamType *) memPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);

		/*
		 * Set frame length.
		 */
		rspSMTHdr->smtHdr.InfoField_Length = Mhtons (frameLen);
	}

	else
	{
		/* issue RDF for frame too long */
		MClearFrameBuffer (responseBuffer,
			(SIZE_RDF_HDR + Mntohs (reqSMTHdr->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE));
		BuildRDFResponse (stnID, requestFrame, responseBuffer, outputMAC,
			RC_FrameTooLong,
			Mntohs (reqSMTHdr->smtHdr.InfoField_Length) + SMT_FRAME_HDR_SIZE);
	}

	return;
}


/*********************************************************************
	Station Information Operation Frames
*********************************************************************/

uInt32
#ifdef __STDC__
BuildSIFOpRequest (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID)
#else
BuildSIFOpRequest (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
#endif
/*********************************************************************
Function:	This function builds an SIF Operation Request.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
Output:		frameBuffer	= contains complete SIF Operation frame.
Return:		Transaction ID is returned if frame was built. Otherwise
			0 is returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* SMT header */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;

	/*
	 * Initialize frame header.
	 */
	result = BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr,
		outputMAC, SIFOP_CLASS, SMTREQUEST, transID);

	/* set constant version ID */
	frameHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/*
	 * Set frame length.
	 */
	frameHdr->smtHdr.InfoField_Length = 0;

	/*
	 * Return transaction ID.
	 */
	return (result);
}

void
#ifdef __STDC__
BuildSIFOpResponse (uInt16 stnID, Void_p requestFrame,
	Void_p responseBuffer, uInt16 outputMAC)
#else
BuildSIFOpResponse (stnID, requestFrame, responseBuffer, outputMAC)
		uInt16           stnID;
		Void_p           requestFrame;
		Void_p           responseBuffer;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function builds an SIF Operation Response.
Parameters:	stnID			= index of station to use.
			requestFrame	= pointer to SIF Request buffer.
			responseBuffer	= pointer to buffer for response frame.
			outputMAC		= index of MAC to send frame out on.
Input:		requestFrame	= contains SIF Request Frame to respond to.
Output:		responseBuffer	= contains SIF Response Frame.
Return:		No value returned.
Notes:		This routine checks the frame's info length after adding
			each MAC's or PORT's parameters. This length is checked after
			all parameters for the MAC or PORT are added to the frame
			because some parameters are optional and this routine can't
			determine the length statically.
				Although the length may exceed the allowable transmit info
			length size (4332), this is less than the frame buffer size
			of 4500. Therefore, there is not need to check for buffer
			overflow.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *reqSMTHdr;				/* SMT header in request frame */
SMTFrameHdrType *rspSMTHdr;				/* SMT header in response frame */
Void_p           memPtr;				/* parameter pointer in frame */
uInt16           frameLen;				/* Total bytes in info field */
uInt16           paramLen;				/* Total bytes of a frame parameter */
TLVParamType     mibData;				/* for Set Count */
uInt16           i;						/* loop counter */
FBMMACInfoType  *macPtr;				/* MAC element */
FBMPORTInfoType *portPtr;				/* PORT element */

	/*
	 * Get frame headers.
	 */
	reqSMTHdr = (SMTFrameHdrType *) requestFrame;
	rspSMTHdr = (SMTFrameHdrType *) responseBuffer;

	/*
	 * initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspSMTHdr, SMT_INFO_FC,
		reqSMTHdr->macHdr.SA, outputMAC, SIFOP_CLASS, SMTRESPONSE,
		Mntohl (reqSMTHdr->smtHdr.Transaction_ID));

	/* set constant version ID */
	rspSMTHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/* get start of info field */
	memPtr = MADDPTR (Void_t, rspSMTHdr, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/* time stamp */
	paramLen = MsgTimeStamp_Param (stnID, (MsgTimeStampParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* manufacturer field */
	paramLen = Manufacturer_Param (stnID, (ManufacturerParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* user field */
	paramLen = User_Param (stnID, (UserParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* set count */
	MEMZERO (&mibData, sizeof (mibData));	/* Clear TLV buffer */
	mibData.paramType = fddiSMTSetCount;
	mibData.paramLen = 0;
	if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
		== RC_Success)
	{
		paramLen = Attribute_Param ((TLVParamType *) memPtr, &mibData);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);
	}

	/* per MAC parameters */
	for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
		(frameLen < MAX_SMT_XMIT_LEN) && (i < MAX_MAC_COUNT);
		i++, macPtr++)
	{
		/* only use physically present MACs */
		if (!macPtr->hwPresent)
			continue;

		/* MAC status */
		paramLen = MACStatus_Param (stnID,
			(MACStatusParamType *) memPtr, macPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);

		/* MAC frame counters */
		paramLen = FrameCounters_Param (stnID,
			(FrameCountersParamType *) memPtr, macPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);

		/* MAC frame not copied */
		paramLen = FrameNotCopied_Param (stnID,
			(FrameNotCopiedParamType *) memPtr, macPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);

		/* MAC priority values */
		paramLen = PriorityValues_Param (stnID,
			(PriorityValuesParamType *) memPtr, macPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);
	}

	/*
	 * If loop exited early, then make sure the frame length exceeds the max
	 * value.
	 */
	if (i < MAX_MAC_COUNT)
		frameLen = MAX_SMT_XMIT_LEN + 1;

	/* per PORT parameters */
	for (i = 0, portPtr = FBMDATA (stnID, fbmPORTData);
		(frameLen < MAX_SMT_XMIT_LEN) && (i < MAX_PORT_COUNT);
		i++, portPtr++)
	{
		/* only use physically present ports */
		if (!portPtr->hwPresent)
			continue;

		/* PORT LEM status */
		paramLen = LERStatus_Param (stnID,
			(LERStatusParamType *) memPtr, portPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);

		/* PORT EB status */
		paramLen = EBStatus_Param (stnID,
			(EBStatusParamType *) memPtr, portPtr);
		frameLen += paramLen;
		memPtr = MADDPTR (Void_t, memPtr, paramLen);
	}

	/*
	 * If loop exited early, then make sure the frame length exceeds the max
	 * value.
	 */
	if (i < MAX_PORT_COUNT)
		frameLen = MAX_SMT_XMIT_LEN + 1;

	/* if frame is NOT too long */
	if (frameLen <= MAX_SMT_XMIT_LEN)
	{
		/* set frame length */
		rspSMTHdr->smtHdr.InfoField_Length = Mhtons (frameLen);
	}

	else
	{
		/* issue RDF for frame too long */
		MClearFrameBuffer (responseBuffer,
			(SIZE_RDF_HDR + Mntohs (reqSMTHdr->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE));
		BuildRDFResponse (stnID, requestFrame, responseBuffer, outputMAC,
			RC_FrameTooLong,
			Mntohs (reqSMTHdr->smtHdr.InfoField_Length) + SMT_FRAME_HDR_SIZE);
	}

	return;
}


/*********************************************************************
	ECFs
*********************************************************************/

uInt32
#ifdef __STDC__
BuildECFRequest (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC, uInt32 transID,
	Void_p info, uInt16 infoLen)
#else
BuildECFRequest (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		Void_p           info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds an Echo Request Frame.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			info		= buffer containing data for echo frame
							info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			info		= info field of echo frame.
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
	 * Only build frame if requested length fits into the frame.
	 */
	if ((infoLen + SIZE_TLVHdrType) <= MAX_SMT_INFO_LEN)
	{
		/*
		 * Initialize frame header.
		 */
		result = BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr,
			outputMAC, ECF_CLASS, SMTREQUEST, transID);

		/* set constant version ID */
		frameHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

		/*
		 * Get the start of the info portion.
		 */
		memPtr = MADDPTR (Void_t, frameBuffer, SIZE_SMTFrameHdrType);

		/*
		 * Fill in echo data
		 */
		frameLen = Echo_Param ((EchoParamType *) memPtr, info, infoLen);

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

void
#ifdef __STDC__
BuildECFResponse (uInt16 stnID, Void_p requestFrame,
	Void_p responseBuffer, uInt16 outputMAC)
#else
BuildECFResponse (stnID, requestFrame, responseBuffer, outputMAC)
		uInt16           stnID;
		Void_p           requestFrame;
		Void_p           responseBuffer;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	This function builds an Echo Response Frame.
Parameters:	stnID			= index of station to use.
			requestFrame	= pointer to Echo Request Frame buffer.
			responseBuffer	= pointer to buffer for response frame.
			outputMAC		= index of MAC to send frame out on.
Input:		requestFrame	= contains Echo Request Frame to respond to.
Output:		responseBuffer	= contains Echo Response Frame.
Return:		No value returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *reqframeHdr;			/* SMT header in request frame */
SMTFrameHdrType *rspframeHdr;			/* SMT header in response frame */
uInt16           frameLen;				/* SMT InfoField length */
uChar           *responsePtr;			/* Response SMT InfoField */
uChar           *requestPtr;			/* Request SMT InfoField */

	/*
	 * Get frame headers.
	 */
	reqframeHdr = (SMTFrameHdrType *) requestFrame;
	rspframeHdr = (SMTFrameHdrType *) responseBuffer;

	/*
	 * Initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspframeHdr, SMT_INFO_FC,
		reqframeHdr->macHdr.SA, outputMAC, ECF_CLASS, SMTRESPONSE,
		Mntohl (reqframeHdr->smtHdr.Transaction_ID));

	/* set constant version ID */
	rspframeHdr->smtHdr.Version_ID = Mhtons (SMT_VERSION_CONSTANT);

	/*
	 * Get and set info length.
	 */
	frameLen = Mntohs (reqframeHdr->smtHdr.InfoField_Length);
	rspframeHdr->smtHdr.InfoField_Length = reqframeHdr->smtHdr.InfoField_Length;

	/*
	 * Send response frame if length is OK. Otherwise, send RDF.
	 */
	if (frameLen <= MAX_SMT_INFO_LEN)
	{
		/*
		 * Get the start of the info portion.
		 */
		responsePtr = MADDPTR (uChar, rspframeHdr, SIZE_SMTFrameHdrType);
		requestPtr = MADDPTR (uChar, reqframeHdr, SIZE_SMTFrameHdrType);

		/*
		 * Copy the echo parameter.
		 */
		MEMCOPY (responsePtr, requestPtr, frameLen);
	}

	else
	{
		/* issue RDF for frame too long */
		MClearFrameBuffer (responseBuffer,
			(SIZE_RDF_HDR + Mntohs (reqframeHdr->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE));
		BuildRDFResponse (stnID, requestFrame, responseBuffer, outputMAC,
			RC_FrameTooLong,
			Mntohs (reqframeHdr->smtHdr.InfoField_Length) + SMT_FRAME_HDR_SIZE);
	}

	return;
}


/*********************************************************************
	Request Denied Frame
*********************************************************************/

void
#ifdef __STDC__
BuildRDFResponse (uInt16 stnID, Void_p requestFrame,
	Void_p responseBuffer, uInt16 outputMAC, uInt32 reasonCode,
	uInt16 reqFrameLen)
#else
BuildRDFResponse (stnID, requestFrame, responseBuffer, outputMAC,
	reasonCode, reqFrameLen)
		uInt16           stnID;
		Void_p           requestFrame;
		Void_p           responseBuffer;
		uInt16           outputMAC;
		uInt32           reasonCode;
		uInt16           reqFrameLen;
#endif
/*********************************************************************
Function:	This function builds a Request Denied Frame Response.
Parameters:	stnID			= index of station to use.
			requestFrame	= pointer to frame being denied.
			responseBuffer	= pointer to buffer for response frame.
			outputMAC		= index of MAC to send frame out on.
			reasonCode		= reason code for RDF
			reqFrameLen		= number of bytes actually received for this
							frame.
Input:		requestFrame	= contains frame being denied service.
Output:		responseBuffer	= contains RDF.
Return:		None.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *reqframeHdr;			/* SMT header in request frame */
SMTFrameHdrType *rspframeHdr;			/* SMT header in response frame */
Void_p           memPtr;				/* pointer to frame parameters */
uInt16           frameLen;				/* length of response frame INFO */
uInt16           paramLen;				/* length of frame parameters */

	/*
	 * Get frame headers.
	 */
	reqframeHdr = (SMTFrameHdrType *) requestFrame;
	rspframeHdr = (SMTFrameHdrType *) responseBuffer;

	/*
	 * Initialize frame header.
	 */
	BuildSMTHeaderMulti (stnID, rspframeHdr, SMT_INFO_FC,
		reqframeHdr->macHdr.SA, outputMAC, RDF_CLASS, SMTRESPONSE,
		Mntohl (reqframeHdr->smtHdr.Transaction_ID));

	/*
	 * NON-STANDARD: This code changes the version ID in the RDF. The SMT 7.3
	 * document incremented the version ID in the SMT header for RDFs to 2.
	 * However, if the requesting station is using a different version ID and
	 * does not support version ID 2, then it may invalidate the RDF that
	 * this station sends back. The other station will not know why it is not
	 * receiving responses from this station. To avoid this problem, this
	 * code sets the RDF's version ID to be the same as the version ID in the
	 * request frame.
	 */
	rspframeHdr->smtHdr.Version_ID = reqframeHdr->smtHdr.Version_ID;

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, rspframeHdr, SIZE_SMTFrameHdrType);
	frameLen = 0;

	/* reason code */
	paramLen = ReasonCode_Param ((ReasonCodeParamType *) memPtr, reasonCode);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* SMT supported versions */
	paramLen = SupportedVersions_Param (stnID,
		(SupportedVersionsParamType *) memPtr);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* refused frame fragment */
	paramLen = RejectedFrame_Param ((RejectedFrameParamType *) memPtr,
		reqframeHdr, (MAX_SMT_INFO_LEN - frameLen), reqFrameLen);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/*
	 * Set info length.
	 */
	rspframeHdr->smtHdr.InfoField_Length = Mhtons (frameLen);

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
BuildSMTHeader (SMTFrameHdrType * frameHdr, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uChar frameClass, uChar frameType, uInt32 transID)
#else
BuildSMTHeader (frameHdr, fcField, destAddr, outputMAC,
	frameClass, frameType, transID)
		SMTFrameHdrType *frameHdr;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uChar            frameClass;
		uChar            frameType;
		uInt32           transID;
#endif
{
	return (BuildSMTHeaderMulti (FIRST_STATION, frameHdr, fcField,
			destAddr, outputMAC, frameClass, frameType, transID));
}

#endif							/* MULTIPLE_STATION */
