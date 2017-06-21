/* $Id: fbmrcv.c,v 3.2 1995/11/17 18:10:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmrcv.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmrcv.c,v $
 * Revision 3.2  1995/11/17  18:10:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmrcv.c xm7_v3.1.1:cset.0173:13:13 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Receive SMT Frames Module

		File:			xm7_fbmrcv.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is responsible for receiving SMT frames and taking
		whatever action is necessary for processing each frame.  For
		frames which require a require a response one is generated
		immediately.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"fbmproto.h"
#include	"multistn.h"


/*********************************************************************
	FBM Receive Routines
*********************************************************************/

static           uInt32
#ifdef __STDC__
ValidateFrameContents (uInt16 stnID, SMTFrameHdrType * frameHdr,
	uInt16 frameLen)
#else
ValidateFrameContents (stnID, frameHdr, frameLen)
		uInt16           stnID;
		SMTFrameHdrType *frameHdr;
		uInt16           frameLen;
#endif
/*********************************************************************
Function:	This function checks the contents of the frame to
			verify that its basic format and size are correct.
Parameters:	stnID		= index of station to use.
			frameHdr	= address of frame buffer.
			frameLen	= length of frame (in bytes).
Input:		frameHdr	= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		None.
Return:		Reason code indicating result of test:
			RC_Success
				Contents of frame are OK so far.
			RC_LengthError
				Length of frame and info field do not match or
				frame parameter length incorrect.
			RC_FrameVersionNotSupported
				Version specifed in frame is not supported
				by this release.
Modification History:
*********************************************************************/
{
uInt16           infoLen;				/* info field length */
uInt16           plen;					/* parameter length */
uInt16           len;					/* length counter */
TLVHdrType      *param;					/* parameter ptr */
uInt32           result;				/* return value */

	result = RC_Success;

	/*
	 * Verify there is enough data to form a valid SMT header and the amount
	 * of data received is what the frame says should be there and the frame
	 * is not too long.
	 */
	infoLen = Mntohs (frameHdr->smtHdr.InfoField_Length);
	if ((frameLen < SMT_FRAME_HDR_SIZE)
		|| (frameLen != (infoLen + SMT_FRAME_HDR_SIZE))
		|| (infoLen > MAX_SMT_INFO_LEN))
	{

		/* lengths do not match or frame is too small or too large */
		result = RC_LengthError;
	}

	else
	{
		/*
		 * Skip through parameters and verify that the InfoField length ends
		 * where a parameter ends.
		 */
		param = MADDPTR (TLVHdrType, frameHdr, SIZE_SMTFrameHdrType);
		len = 0;

		/*
		 * Adding an odd length to the current parameter pointer causes the
		 * next parameter to start at an odd address. This causes memory
		 * accessing problems on some processors.
		 * 
		 * To prevent this, all parameter lengths must be a multiple of 4 except
		 * for Echo Data. This exception may be any length as long it is the
		 * last parameter in the frame.
		 */
		plen = 0;
		while (len < infoLen)
		{

			/*
			 * This plen is for the previous parameter. Since len is not less
			 * than infoLen, it was not the last parameter. If its length is
			 * not a multiple of 4, then the frame is invalid.
			 */
			if (plen & 3)
				/* fall out and force length error */
				break;

			plen = (Mntohs (param->paramLen)) + SIZE_TLVHdrType;
			if (param->paramType == 0)
				/* fall out and force length error */
				break;

			len += plen;
			param = MADDPTR (TLVHdrType, param, plen);
		}

		/*
		 * Check if sizes match. If forced out early, this test will also be
		 * true.
		 */
		if (len != infoLen)
			result = RC_LengthError;

		else
		{
			/*
			 * Verify version. Select version test based on frame type.
			 */
			switch (frameHdr->smtHdr.Frame_Class)
			{
				/*
				 * This first set of frame types only uses a constant for the
				 * frame version.
				 */
			case NIF_CLASS:
			case SIFCONFIG_CLASS:
			case SIFOP_CLASS:
			case ECF_CLASS:
				if (Mntohs (frameHdr->smtHdr.Version_ID)
					!= SMT_VERSION_CONSTANT)
					result = RC_FrameVersionNotSupported;
				break;

				/*
				 * Specifically check the other frame classes in case this
				 * frame class is not valid.
				 */
			case RAF_CLASS:
			case SRF_CLASS:
			case PMFGET_CLASS:
			case PMFSET_CLASS:
			case ESF_CLASS:
				if ((Mntohs (frameHdr->smtHdr.Version_ID)
						< FBMDATA (stnID, fbmStationData.loVersionID))
					|| (Mntohs (frameHdr->smtHdr.Version_ID)
						> FBMDATA (stnID, fbmStationData.hiVersionID)))
				{
					/* version is not in range */
					result = RC_FrameVersionNotSupported;
				}
				break;

				/*
				 * NON-STANDARD: For RDFs, this code does not attempt to
				 * validate the Version ID. This is due to the fact that SMT
				 * specifies the Version ID for the RDF to be the Version ID
				 * for that station's SMT. However, if the reason that the
				 * request frame is being denied is due to a version
				 * mismatch, then the RDF coming back from the other station
				 * will also have a version mismatch. Therefore, in order for
				 * this station to recognize the problem, the Version ID must
				 * not be validate on received RDFs.
				 */
			case RDF_CLASS:
				break;

				/*
				 * Anything not recognized has an unknown frame class.
				 */
			default:
				result = RC_FrameClassNotSupported;
				break;
			}
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
ProcessFBMFrameMulti (uInt16 stnID, Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessFBMFrameMulti (stnID, memPtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function is called from the frame service
			routine when a frame has been received by the station.
			This function checks the EAC bits to see if the frame
			needs to be processed by this station and takes
			appropriate action based upon the frame type.
Parameters:	stnID		= index of station to use.
			memPtr		= address of frame buffer.
			frameLen	= length of frame (in bytes).
			MACNum		= MAC index that received the frame.
			EACbits		= EAC indicators as received by the MAC.
Input:		memPtr		= contains frame to process. This must
							include the three pad bytes at the start
							of the frame.
Output:		May issue new frames, set MIB attributes, pass frame to
			MAP, or do nothing.
Return:		0.
Modification History:
*********************************************************************/
{
SMTFrameHdrType *frameHdr;				/* header for frames to process */
uInt16           len;					/* frame length */
uInt32           reasonCode;			/* validation result */

	/*
	 * Don't process until initialized.
	 */
	if (!FBMDATA (stnID, fbmStationData.FBMInitialized))
		return (0);

	/*
	 * Check E indicator.
	 */
	if (!(EACbits & E_Indicator))
	{
		/* E indicator not set, continue processing frame */

		/*
		 * Get frame header.
		 */
		frameHdr = (SMTFrameHdrType *) memPtr;

		/*
		 * Validate frame contents and format.
		 */
		reasonCode = ValidateFrameContents (stnID, frameHdr, frameLen);
		if (reasonCode != RC_Success)
		{

			/*
			 * Frame is not valid. If this is a request, then issue a RDF.
			 */
			if (frameHdr->smtHdr.Frame_Type == SMTREQUEST)
			{

				/*
				 * RDFs are sent to Request frames only. Of the Request
				 * frames, this code assumes that the MAC and frame interface
				 * only pass SMT frames to be processed by this station. Of
				 * these frames, the only ones that SMT does not have respond
				 * to are secondary NSAs (FC=0x4F and * Ar=S).
				 */
				if ((frameHdr->macHdr.FC == SMT_INFO_FC)
					|| ((frameHdr->macHdr.FC == SMT_NSA_FC)
						&& !(EACbits & A_Indicator)))
				{
					/* build RDF frame */
					MClearFrameBuffer (fbmFrameBuffer,
						(SIZE_RDF_HDR + frameLen));
					BuildRDFResponse (stnID, memPtr,
						fbmFrameBuffer, MACNum, reasonCode, frameLen);

					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);
				}
			}
		}

		else
		{
			/*
			 * Select process based on frame type.
			 */
			switch (frameHdr->smtHdr.Frame_Type)
			{

				/*
				 * Announcement frames.
				 */
			case SMTANNOUNCE:
				/* Process frame classes */
				switch (frameHdr->smtHdr.Frame_Class)
				{
				case NIF_CLASS:
					/* handle through NN protocol */
					ProcessNIFReceived (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case ESF_CLASS:
					/* do internal processing */
					ProcessESFAnnouncement (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case SRF_CLASS:
					/* send this frame to the MAP */
					PassMAPFrameMulti (stnID, FBM_EVENT_FRAME_RECEIVED,
						memPtr, frameLen, MACNum, EACbits);
					break;

				default:
					break;
				}
				break;

				/*
				 * Request frames.
				 */
			case SMTREQUEST:
				/* Do not respond to secondary NSAs */
				if ((frameHdr->macHdr.FC == SMT_NSA_FC)
					&& (EACbits & A_Indicator))
					/* jump to end of routine */
					break;

				/*
				 * Process frame classes.
				 */
				switch (frameHdr->smtHdr.Frame_Class)
				{
				case NIF_CLASS:
					/* handle through NN protocol */
					ProcessNIFReceived (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case SIFCONFIG_CLASS:
					/* build response frame */
					MClearFrameBuffer (fbmFrameBuffer, SIZE_SIFCON_RESP);
					BuildSIFConfigResponse (stnID, memPtr,
						fbmFrameBuffer, MACNum);

					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

					break;

				case SIFOP_CLASS:
					/* build response frame */
					MClearFrameBuffer (fbmFrameBuffer, SIZE_SIFOP_RESP);
					BuildSIFOpResponse (stnID, memPtr, fbmFrameBuffer, MACNum);

					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

					break;

				case ECF_CLASS:
					/* build response frame */
					MClearFrameBuffer (fbmFrameBuffer, frameLen);
					BuildECFResponse (stnID, memPtr, fbmFrameBuffer, MACNum);

					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

					break;

				case ESF_CLASS:
					/* do internal processing */
					ProcessESFRequest (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case PMFGET_CLASS:
					/* handle through PMF protocol */
					ProcessPMFGet (stnID, memPtr, MACNum);
					break;

				case PMFSET_CLASS:
					/* handle through PMF protocol */
					ProcessPMFSet (stnID, memPtr, frameLen, MACNum);
					break;

				case RAF_CLASS:
					/* do interal processin */
					ProcessRAFRequest (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				default:
					/* build RDF frame */
					MClearFrameBuffer (fbmFrameBuffer,
						(SIZE_RDF_HDR + frameLen));
					BuildRDFResponse (stnID, memPtr, fbmFrameBuffer,
						MACNum, RC_FrameClassNotSupported, frameLen);

					/* send frame */
					len = Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
						+ SMT_FRAME_HDR_SIZE;
					SendSMTFrameMulti (stnID, fbmFrameBuffer, len, MACNum);

					break;
				}
				break;

				/*
				 * Response frame.
				 */
			case SMTRESPONSE:
				/* Process frame classes */
				switch (frameHdr->smtHdr.Frame_Class)
				{
				case NIF_CLASS:
					/* handle through NN protocol */
					ProcessNIFResponse (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case ESF_CLASS:
					/* do internal processing */
					ProcessESFResponse (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case RAF_CLASS:
					/* do interal processin */
					ProcessRAFResponse (stnID, memPtr, frameLen,
						MACNum, EACbits);
					break;

				case SIFCONFIG_CLASS:
				case SIFOP_CLASS:
				case ECF_CLASS:
				case PMFGET_CLASS:
				case PMFSET_CLASS:
				case RDF_CLASS:
					/* send response frame to MAP */
					PassMAPFrameMulti (stnID, FBM_EVENT_FRAME_RECEIVED,
						memPtr, frameLen, MACNum, EACbits);
					break;

				default:
					break;
				}
				break;
			}

			/*
			 * Check monitoring modes.
			 */
			if ((frameHdr->smtHdr.Frame_Class == NIF_CLASS)
				&& (FBMDATA (stnID, fbmStationData.monitorMode)
					& FMM_MONITOR_NIF))
			{
				/* report NIF to MAP */
				PassMAPFrameMulti (stnID, FBM_EVENT_MONITOR_NIF, memPtr,
					frameLen, MACNum, EACbits);
			}

			if (FBMDATA (stnID, fbmStationData.monitorMode) & FMM_MONITOR_SMT)
			{
				/* report all SMT frames to MAP */
				PassMAPFrameMulti (stnID, FBM_EVENT_MONITOR_SMT, memPtr,
					frameLen, MACNum, EACbits);
			}
		}
	}

	LocalFBMFramesMulti (stnID, memPtr, frameLen, MACNum, EACbits);

	return (0);
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
ProcessFBMFrame (Void_p memPtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
ProcessFBMFrame (memPtr, frameLen, MACNum, EACbits)
		Void_p           memPtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
{

	return (ProcessFBMFrameMulti (FIRST_STATION, memPtr, frameLen,
			MACNum, EACbits));
}

#endif							/* MULTIPLE_STATION */
