/* $Id: fbmesf.c,v 3.2 1995/11/17 18:09:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmesf.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmesf.c,v $
 * Revision 3.2  1995/11/17  18:09:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmesf.c xm7_v3.1.1:cset.0173:11:11 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Process ESF Frames Module

		File:			xm7_fbmesf.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is responsible for processing ESF frames and taking
		whatever action is necessary for processing each frame.  For
		frames which require a require a response one is generated
		immediately.

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
 * Extended Service Frame processing is only included when the XM_USE_ESF
 * constant is defined. Otherwise, default processing of ESFs (ie, deny
 * requests and ignore other frames) is included.
 */
#ifdef	XM_USE_ESF

/*********************************************************************
	Extended Service Parameters
*********************************************************************/

uInt16
#ifdef __STDC__
ESFID_Param (ESFIDParamType * param, FDDILongAddressType esfID)
#else
ESFID_Param (param, esfID)
		ESFIDParamType  *param;
		FDDILongAddressType esfID;
#endif
/*********************************************************************
Function:	Create the ESF ID parameter.
Parameters:	param	= pointer to location where parameter should start.
			esfID	= 48-bit, IEEE-assigned ESF identifier.
Input:		None.
Output:		param	= contains the SMT frame parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (ESFID_PARAM_TYPE);
	param->Length = Mhtons (SIZE_ESFIDParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	MEMCOPY (param->ESF_ID, (uChar *) esfID, 6);

	return (SIZE_ESFIDParamType);
}


/*********************************************************************
	Extended Service Frames
*********************************************************************/

uInt32
#ifdef __STDC__
BuildESFRequestMulti (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFRequestMulti (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds an Extended Service Request frame.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			esfID		= buffer containing ESF identifier.
			info		= buffer containing data for ESF info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			esfID		= 48-bit, IEEE-assigned ESF ID.
			info		= info field of the ESF. This field must
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
uInt16           paramLen;				/* Length of frame parameter */
uInt32           result;				/* value to return */

	/*
	 * Initialize values.
	 */
	frameLen = 0;
	result = 0;
	frameHdr = (SMTFrameHdrType *) frameBuffer;

	/*
	 * Initialize frame header.
	 */
	result = BuildSMTHeaderMulti (stnID, frameHdr, fcField, destAddr,
		outputMAC, ESF_CLASS, SMTREQUEST, transID);

	/*
	 * Get the start of the info portion.
	 */
	memPtr = MADDPTR (Void_t, frameHdr, SIZE_SMTFrameHdrType);

	/* ESF ID parameter */
	paramLen = ESFID_Param ((ESFIDParamType *) memPtr, esfID);
	frameLen += paramLen;
	memPtr = MADDPTR (Void_t, memPtr, paramLen);

	/* Fill in ESF data */
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

	/*
	 * Return transaction ID.
	 */
	return (result);
}

uInt32
#ifdef __STDC__
BuildESFAnnouncementMulti (uInt16 stnID, Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFAnnouncementMulti (stnID, frameBuffer, fcField, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds an Extended Service Announcement frame.
Parameters:	stnID		= index of station to use.
			frameBuffer	= buffer to use to build the frame
			fcField		= value to use for FC field (X'41' or X'4F')
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			esfID		= buffer containing ESF identifier.
			info		= buffer containing data for ESF info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			esfID		= 48-bit, IEEE-assigned ESF ID.
			info		= info field of the ESF. This field must
							be properly formatted in network byte order.
Output:		frameBuffer	= contains complete echo frame.
Return:		Transaction ID is returned if frame was built. Otherwise
			0 is returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* Used for building SMT header frame */
uInt32           result;				/* value to return */

	/*
	 * Use BuildESFRequestMulti() to create basic frame format.
	 */
	result = BuildESFRequestMulti (stnID, frameBuffer, fcField, destAddr,
		outputMAC, transID, esfID, info, infoLen);

	/*
	 * Set frame type.
	 */
	frameHdr = (SMTFrameHdrType *) frameBuffer;
	frameHdr->smtHdr.Frame_Type = SMTANNOUNCE;

	/*
	 * Return transaction ID.
	 */
	return (result);
}

uInt32
#ifdef __STDC__
BuildESFResponseMulti (uInt16 stnID, Void_p frameBuffer,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFResponseMulti (stnID, frameBuffer, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		uInt16           stnID;
		Void_p           frameBuffer;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
/*********************************************************************
Function:	This function builds an ESF Response.
Parameters:	stnID		= index of station to use.
			frameBuffer	= pointer to buffer for response frame.
			destAddr	= buffer containing destination address.
			outputMAC	= index of MAC to send frame out on.
			transID		= transaction ID to use in frame.
			esfID		= buffer containing ESF identifier.
			info		= buffer containing data for ESF info field.
			infoLen		= The number of bytes in the information field.
Input:		destAddr	= 48-bit station ID of destination.
			transID		= if transaction ID is 0, then an ID will
							be assigned.
			esfID		= 48-bit, IEEE-assigned ESF ID.
			info		= info field of the ESF. This field must
							be properly formatted in network byte order.
Output:		frameBuffer	= contains ESF Response Frame.
Return:		No value returned.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* Used for building SMT header frame */
uInt32           result;				/* value to return */

	/*
	 * Use BuildESFRequestMulti() to create basic frame format.
	 */
	result = BuildESFRequestMulti (stnID, frameBuffer, SMT_INFO_FC,
		destAddr, outputMAC, transID, esfID, info, infoLen);

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
	ESF Receive Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessESFAnnouncement (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFAnnouncement (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Announcement. If the
			ESF does not fall into any approved extensions of the
			standard, a call to LocalESFAnnouncementMulti() is made.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None by this function.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
	/*
	 * No defined ESF processing. Call local options.
	 */
	LocalESFAnnouncementMulti (stnID, memPtr, frameLen, MACNum, EACbits);

	return;
}

void
#ifdef __STDC__
ProcessESFRequest (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFRequest (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Request. If the
			ESF does not fall into any approved extensions of the
			standard, a call to LocalESFRequestMulti() is made. If
			LocalESFRequestMulti() responds to the request, then
			the returned value must be RC_Suceess. Any other returned
			value is taken as the reason code to be used in an RDF.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None by this function.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
uInt32           result;				/* reason code from I/F routine */
uInt16           len;					/* RDF frame length */

	/*
	 * No defined ESF processing. Call local options.
	 */
	result = LocalESFRequestMulti (stnID, memPtr, frameLen,
		MACNum, EACbits);

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
ProcessESFResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFResponse (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Announcement. If the
			ESF does not fall into any approved extensions of the
			standard, a call to LocalESFAnnouncementMulti() is made.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
	/*
	 * No defined ESF processing. Call local options.
	 */
	LocalESFResponseMulti (stnID, memPtr, frameLen, MACNum, EACbits);

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
BuildESFRequest (Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFRequest (frameBuffer, fcField, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
{
	return (BuildESFRequestMulti (FIRST_STATION, frameBuffer, fcField,
			destAddr, outputMAC, transID, esfID, info, infoLen));
}

uInt32
#ifdef __STDC__
BuildESFAnnouncement (Void_p frameBuffer, uChar fcField,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFAnnouncement (frameBuffer, fcField, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		Void_p           frameBuffer;
		uChar            fcField;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
{
	return (BuildESFAnnouncementMulti (FIRST_STATION, frameBuffer, fcField,
			destAddr, outputMAC, transID, esfID, info, infoLen));
}

uInt32
#ifdef __STDC__
BuildESFResponse (Void_p frameBuffer,
	FDDILongAddressType destAddr, uInt16 outputMAC,
	uInt32 transID, FDDILongAddressType esfID,
	uChar * info, uInt16 infoLen)
#else
BuildESFResponse (frameBuffer, destAddr, outputMAC,
	transID, esfID, info, infoLen)
		Void_p           frameBuffer;
		FDDILongAddressType destAddr;
		uInt16           outputMAC;
		uInt32           transID;
		FDDILongAddressType esfID;
		uChar           *info;
		uInt16           infoLen;
#endif
{
	return (BuildESFResponseMulti (FIRST_STATION, frameBuffer, destAddr,
			outputMAC, transID, esfID, info, infoLen));
}

#endif							/* MULTIPLE_STATION */


/*
 * If XM_USE_ESF is not defined, then the basic processing of ESFs is
 * included. This processing denies all request frames and drops
 * all other frames.
 *
 * This protocol DOES NOT pass frames to the local processing routines.
 * Therefore, the routines in fbmesfif.c do not need to be provided at all.
 */
#else							/* XM_USE_ESF */

/*********************************************************************
	Default ESF Receive Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessESFAnnouncement (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFAnnouncement (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Announcement by simply
			ignoring the frame. Because no protocols using ESFs are
			supported, nothing needs to be done. The parameters
			are passed to this routine to maintain interface
			compatibility with the routine of the same name if
			XM_USE_ESF is defined.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
	/*
	 * No defined ESF processing. Drop frame.
	 */
	return;
}

void
#ifdef __STDC__
ProcessESFRequest (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFRequest (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Request by denying all
			requests.  Because no protocols using ESFs are
			supported, this frame cannot be processed.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None by this function.
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
ProcessESFResponse (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessESFResponse (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function processes an ESF Response by simply
			ignoring the frame. Because no protocols using ESFs are
			supported, nothing needs to be done. The parameters
			are passed to this routine to maintain interface
			compatibility with the routine of the same name if
			XM_USE_ESF is defined.
Parameters:	stnID		= index of station to use.
			memPtr		= address of buffer containing frame.
			frameLen	= length of total frame in bytes.
			MACNum		= MAC index which received the frame.
			EACbits		= EAC bits received by MAC.
Input:		memPtr		= complete ESF frame received. This frame
							has already had its E indicator and version
							verified by ProcessFBMFrame().
Output:		None.
Return:		No value is returned.
Modification History:
*********************************************************************/
{
	/*
	 * No defined ESF processing. Drop frame.
	 */
	return;
}

#endif							/* XM_USE_ESF */
