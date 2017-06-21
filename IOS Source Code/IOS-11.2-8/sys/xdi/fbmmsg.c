/* $Id: fbmmsg.c,v 3.2.60.1 1996/03/18 22:51:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/fbmmsg.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmmsg.c,v $
 * Revision 3.2.60.1  1996/03/18  22:51:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.1  1996/03/07  11:20:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:09:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmmsg.c xm7_v3.1.1:cset.0173:18:18 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Message Processing Module

		File:			xm7_fbmmsg.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for processing incoming FBM messages
		and for formatting standard outgoing FBM messages.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_FLEX_CONF	include code for flexible configuration.
			XM_USE_RAF			include code for RAF processing.
			XM_USE_ESF			include code for ESF processing.

		The constant XM_USE_FLEX_CONF controls conditional compilation
		over routines to process messages handling flexible
		configuration changes. If this constant is NOT defined, then
		the associated routines are NOT included in the code and the
		system will not support flexible configurations.

		The constant XM_USE_RAF controls conditional compilation
		over routines to process Resource Allocation Frames. If this
		constant is NOT defined, then the associated routines are NOT
		included in the code and the system will not support RAFs nor
		synchronous bandwidth functions.

		The constant XM_USE_ESF controls conditional compilation
		over routines to process Extended Service Frames. If this
		constant is NOT defined, then the associated routines are NOT
		included in the code and the system will not support ESFs.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
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


/*********************************************************************
	FBM Outgoing Message Routines
*********************************************************************/

void
#ifdef __STDC__
PassMAPFrameMulti (uInt16 stnID, uInt16 messageType, Void_p framePtr,
	uInt16 frameLen, uInt16 MACNum, uChar EACbits)
#else
PassMAPFrameMulti (stnID, messageType, framePtr, frameLen, MACNum, EACbits)
		uInt16           stnID;
		uInt16           messageType;
		Void_p           framePtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
/*********************************************************************
Function:	This function passes a frame received by FBM to the MAP.
			The frame will generally be a response frame that answers
			a request issued by the MAP.
Parameters:	stnID		= index of station to use.
			messageType	= message type ID to use in message
			framePtr	= address of frame buffer.
			frameLen	= length of frame (in bytes)
			MACNum		= index of MAC that received the frame.
			EACbits		= E, A, and C indicators from MAC.
Input:		framePtr	= contains complete frame received by MAC.
Output:		Issues message containing frame to MAP.
Return:		No value returned.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* address of message buffer */
RecFrameType     recBuf;				/* frame info */

	/*
	 * Fill in message information.
	 */
	msg.destination = MAP_MSG_ID;
	msg.source = FBM_MSG_ID;
	msg.type = messageType;
	msg.entity = FBMDATA (stnID, fbmMACData[MACNum].Index);
	msg.localID = 0;
	msg.p1.recFrameBuf = &recBuf;
	msg.len1 = sizeof (RecFrameType);
	msg.p2 = framePtr;
	msg.len2 = frameLen + FC_PAD_SIZE;
	msg.stnID = stnID;

	recBuf.EACbits = EACbits;
	recBuf.frameLen = frameLen;

	/*
	 * Send the message.
	 */
	SendFBMMessage (&msg);

	return;
}

void
#ifdef __STDC__
SendFBMEvent (uInt16 stnID, uInt16 eventType, uInt16 entity)
#else
SendFBMEvent (stnID, eventType, entity)
		uInt16           stnID;
		uInt16           eventType;
		uInt16           entity;
#endif
/*********************************************************************
Function:	Send an FBM generated event to another process. This
			function determines what processes need to be notified
			based upon the event type.
Parameters:	stnID		= index of station to use.
			eventType	= type of FBM event.
			entity		= MAC, PATH or PORT index associated with the
							event (if needed).
Input:		FBM data for object associated with the event.
Output:		Issues event message to appropriate process(es).
Return:		None.
*********************************************************************/
{
SMTMessage       eventMsg;				/* message to send */

	/*
	 * Fill in message buffer.
	 */
	eventMsg.source = FBM_MSG_ID;
	eventMsg.destination = MIB_MSG_ID;	/* most messages go to MIB */
	eventMsg.type = FBM_EVENT_NOTIFY_MIB;
	eventMsg.typeInfo = (uInt16) eventType;
	eventMsg.entity = (uInt16) entity;
	eventMsg.len1 = 0;
	eventMsg.len2 = 0;
	eventMsg.localID = 0;
	eventMsg.stnID = stnID;

	/*
	 * Fill in data.
	 */
	switch (eventType)
	{
	case fddiMACUpstreamNbr:
		MCopyAddress (eventMsg.data.addr,
			FBMDATA (stnID, fbmMACData[entity].UNA));
		break;

	case fddiMACDownstreamNbr:
		MCopyAddress (eventMsg.data.addr,
			FBMDATA (stnID, fbmMACData[entity].DNA));
		break;

	case fddiMACDupAddressTest:
		eventMsg.data.b16 = FBMDATA (stnID,
			fbmMACData[entity].Dup_Addr_Test);
		break;

	case fddiMACUNDA_Flag:
		eventMsg.data.b8 = FBMDATA (stnID,
			fbmMACData[entity].myUNADuplicate);
		break;

	default:
		return;
	}

	/*
	 * Send message.
	 */
	SendFBMMessage (&eventMsg);

	return;
}

void
#ifdef __STDC__
ProcessSendFrame (SMTMessage * msg)
#else
ProcessSendFrame (msg)
		SMTMessage      *msg;
#endif
/*********************************************************************
Function:	Process a message that requests an SMT frame to be sent.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= complete contents of message.
Output:		Performs actions appropriate to message type and info.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID from meesage */
SendFrameType   *buf;					/* send frame info */
uInt32           transID;				/* request frame ID */
uInt16           outputMAC;				/* index of MAC to send frame on */
uInt16           result;				/* result of frame build */

	stnID = msg->stnID;
	result = 0;

	/*
	 * Set buffer pointer.
	 */
	buf = msg->p1.sendFrameBuf;

	/*
	 * Convert sourceMAC value from resource index to array index.
	 */
	transID = 0;
	switch (buf->frameClass)
	{
	case NIF_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.nif.sourceMAC,
			&outputMAC);
		break;

	case SIFCONFIG_CLASS:
	case SIFOP_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.sif.sourceMAC,
			&outputMAC);
		break;

	case ECF_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.ecf.sourceMAC,
			&outputMAC);
		break;

		/*
		 * If XM_USE_RAF is defined, then RAF processing may proceed.
		 * Otherwise, this case is handled as an undefined frame class
		 * through the default case.
		 */
#ifdef XM_USE_RAF
	case RAF_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.raf.sourceMAC,
			&outputMAC);
		break;
#else
	case RAF_CLASS:
		result = EFBM_RAF_NOT_SUPPORTED;
		break;
#endif							/* XM_USE_RAF */

	case RDF_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.rdf.sourceMAC,
			&outputMAC);
		break;

	case PMFGET_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.getpmf.sourceMAC,
			&outputMAC);
		break;

	case PMFSET_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.setpmf.sourceMAC,
			&outputMAC);
		break;

		/*
		 * If XM_USE_ESF is defined, then ESF processing may proceed.
		 * Otherwise, this case is handled as an undefined frame class
		 * through the default case.
		 */
#ifdef XM_USE_ESF
	case ESF_CLASS:
		result = GetMACIndexMulti (stnID, buf->frameData.esf.sourceMAC,
			&outputMAC);
		break;
#else
	case ESF_CLASS:
		result = EFBM_ESF_NOT_SUPPORTED;
		break;
#endif							/* XM_USE_ESF */

	default:
		result = EFBM_BAD_FRAME_CLASS;
		break;
	}

	/*
	 * Proceed if MAC was found.
	 */
	if (result == 0)
	{
		/*
		 * If frame is a response frame, then create a frame buffer for the
		 * request frame in the message.
		 */
		if (buf->frameType == SMTRESPONSE)
		{

			/*
			 * Select frame processing.
			 */
			switch (buf->frameClass)
			{
			case RDF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_RDF_HDR + msg->len2));
				BuildRDFResponse (stnID,
					msg->p2,
					fbmFrameBuffer,
					outputMAC,
					buf->frameData.rdf.reasonCode,
					msg->len2);
				transID = Mntohl (fbmFrameHeader->smtHdr.Transaction_ID);
				break;

				/*
				 * If XM_USE_RAF is defined, then RAF processing may proceed.
				 * Otherwise, this case is handled as an undefined frame
				 * class through the default case.
				 */
#ifdef XM_USE_RAF
			case RAF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_RAF_HDR + msg->len2));
				transID = BuildRAFResponseMulti (stnID,
					fbmFrameBuffer,
					buf->frameData.raf.destAddr,
					outputMAC,
					buf->frameData.raf.transID,
					(uChar *) msg->p2,
					msg->len2);
				break;
#endif							/* XM_USE_RAF */

				/*
				 * If XM_USE_ESF is defined, then ESF processing may proceed.
				 * Otherwise, this case is handled as an undefined frame
				 * class through the default case.
				 */
#ifdef XM_USE_ESF
			case ESF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_ESF_HDR + msg->len2));
				transID = BuildESFResponseMulti (stnID,
					fbmFrameBuffer,
					buf->frameData.esf.destAddr,
					outputMAC,
					buf->frameData.esf.transID,
					buf->frameData.esf.esfID,
					(uChar *) msg->p2,
					msg->len2);
				break;
#endif							/* XM_USE_ESF */

			default:
				/* frame class already checked during MAC index search above */
				break;
			}
		}

		else if (buf->frameType == SMTREQUEST)
		{

			/*
			 * Select frame processing.
			 */
			switch (buf->frameClass)
			{
			case NIF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, SIZE_NIF);
				transID = BuildNIFRequest (stnID,
					fbmFrameBuffer,
					buf->frameData.nif.FC,
					buf->frameData.nif.destAddr,
					outputMAC,
					buf->frameData.nif.transID);
				break;

			case SIFCONFIG_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, SIZE_SIFCON_REQ);
				transID = BuildSIFConfigRequest (stnID,
					fbmFrameBuffer,
					buf->frameData.sif.FC,
					buf->frameData.sif.destAddr,
					outputMAC,
					buf->frameData.sif.transID);
				break;

			case SIFOP_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, SIZE_SIFOP_REQ);
				transID = BuildSIFOpRequest (stnID,
					fbmFrameBuffer,
					buf->frameData.sif.FC,
					buf->frameData.sif.destAddr,
					outputMAC,
					buf->frameData.sif.transID);
				break;

			case ECF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_ECF_HDR + msg->len2));
				transID = BuildECFRequest (stnID,
					fbmFrameBuffer,
					buf->frameData.ecf.FC,
					buf->frameData.ecf.destAddr,
					outputMAC,
					buf->frameData.ecf.transID,
					msg->p2,
					msg->len2);
				break;

				/*
				 * If XM_USE_RAF is defined, then RAF processing may proceed.
				 * Otherwise, this case is handled as an undefined frame
				 * class through the default case.
				 */
#ifdef XM_USE_RAF
			case RAF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_RAF_HDR + msg->len2));
				transID = BuildRAFRequestMulti (stnID,
					fbmFrameBuffer,
					buf->frameData.raf.FC,
					buf->frameData.raf.destAddr,
					outputMAC,
					buf->frameData.raf.transID,
					(uChar *) msg->p2,
					msg->len2);
				break;
#endif							/* XM_USE_RAF */

				/*
				 * If XM_USE_ESF is defined, then ESF processing may proceed.
				 * Otherwise, this case is handled as an undefined frame
				 * class through the default case.
				 */
#ifdef XM_USE_ESF
			case ESF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_ESF_HDR + msg->len2));
				transID = BuildESFRequestMulti (stnID,
					fbmFrameBuffer,
					buf->frameData.esf.FC,
					buf->frameData.esf.destAddr,
					outputMAC,
					buf->frameData.esf.transID,
					buf->frameData.esf.esfID,
					(uChar *) msg->p2,
					msg->len2);
				break;
#endif							/* XM_USE_ESF */

			case PMFGET_CLASS:
				MClearFrameBuffer (fbmFrameBuffer,
					(SIZE_PMF_GET_REQ_HDR + msg->len2));
				transID = BuildPMFGetRequest (stnID,
					fbmFrameHeader,
					buf->frameData.getpmf.destAddr,
					outputMAC,
					buf->frameData.getpmf.transID,
					(TLVParamType *) msg->p2,
					msg->len2);
				break;

			case PMFSET_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, MAX_FRAME_SIZE);
				transID = BuildPMFSetRequest (stnID,
					fbmFrameHeader,
					buf->frameData.setpmf.destAddr,
					outputMAC,
					buf->frameData.setpmf.transID,
					&buf->frameData.setpmf.setCount,
					(TLVParamType *) msg->p2);
				break;

			default:
				/* frame class already checked during MAC index search above */
				break;
			}
		}

		else if (buf->frameType == SMTANNOUNCE)
		{

			/*
			 * Select frame processing.
			 */
			switch (buf->frameClass)
			{
			case NIF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, SIZE_NIF);
				transID = BuildNIFAnnouncement (stnID,
					fbmFrameBuffer,
					buf->frameData.nif.FC,
					buf->frameData.nif.destAddr,
					outputMAC,
					buf->frameData.nif.transID);
				break;

				/*
				 * If XM_USE_ESF is defined, then ESF processing may proceed.
				 * Otherwise, this case is handled as an undefined frame
				 * class through the default case.
				 */
#ifdef XM_USE_ESF
			case ESF_CLASS:
				MClearFrameBuffer (fbmFrameBuffer, (SIZE_ESF_HDR + msg->len2));
				transID = BuildESFAnnouncementMulti (stnID,
					fbmFrameBuffer,
					buf->frameData.esf.FC,
					buf->frameData.esf.destAddr,
					outputMAC,
					buf->frameData.esf.transID,
					buf->frameData.esf.esfID,
					(uChar *) msg->p2,
					msg->len2);
				break;
#endif							/* XM_USE_ESF */

			default:
				/* frame class already checked during MAC index search above */
				break;
			}
		}

		/*
		 * If frame was built, then send it and notify requesting process of
		 * transaction ID.
		 */
		if (transID != 0)
		{

			/*
			 * Send frame.
			 */
			SendSMTFrameMulti (stnID, fbmFrameBuffer,
				Mntohs (fbmFrameHeader->smtHdr.InfoField_Length)
				+ SMT_FRAME_HDR_SIZE, outputMAC);

			/*
			 * Set message header info.
			 */
			msg->destination = msg->source;
			msg->source = FBM_MSG_ID;
			msg->type = FBM_RESPONSE_FRAME_SENT;

			/*
			 * Return SendFrameType structure as well.
			 */
			buf->sentTransID = transID;

			SendFBMMessage (msg);
		}

		/*
		 * Otherwise, mark error condition.
		 */
		else
		{
			result = EFBM_BAD_FRAME_REQUESTED;
		}
	}

	/*
	 * If an error occurred, send message.
	 */
	if (result != 0)
	{

		/*
		 * Set message header info.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_ERROR_MSG;
		msg->typeInfo = result;

		SendFBMMessage (msg);
	}

	return;
}

void
#ifdef __STDC__
ProcessFBMMIBChange (SMTMessage * smtmsg)
#else
ProcessFBMMIBChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a change from the MIB.
Parameters:	smtmsg	= address of buffer containing message.
Input:		smtmsg	= complete contents of message.
Output:		changes global data for FBM to use.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID from message */

	stnID = smtmsg->stnID;
	switch (smtmsg->typeInfo)
	{
	case fddiSMTT_Notify:
		FBMDATA (stnID, fbmStationData.T_Notify) = smtmsg->data.b16;
		break;

	case fddiSMTStatRptPolicy:
		/*
		 * Notification is only sent if state has changed, so set new value
		 * and treat change like SR_Enable signal.
		 */
		if (smtmsg->data.b8)
		{
			/* SR(20b) SR_Enable */
			FBMDATA (stnID, fbmSRPData.srState) = SR_WAIT;
			(void) InitSRP (stnID);		/* see fbmsrp.c */
		}

		else
			/* SR(02) NOT SR_Enable */
			FBMDATA (stnID, fbmSRPData.srState) = SR_DISABLED;
		break;

	case fddiMACDA_Flag:
		FBMDATA (stnID, fbmMACData[smtmsg->entity].myDuplicate)
			= smtmsg->data.b8;
		break;

	case fddiMACSMTAddress:
		MCopyAddress (FBMDATA (stnID, fbmMACData[smtmsg->entity].SMTAddress),
			smtmsg->data.addr);

		/* do NN protocol Reset_Actions */
		FBMDATA (stnID, fbmMACData[smtmsg->entity].Dup_Addr_Test) = DAT_none;
		SendFBMEvent (stnID, fddiMACDupAddressTest, smtmsg->entity);
		FBMDATA (stnID, fbmMACData[smtmsg->entity].NNTransactionID)
			= GenTransID (stnID);
		break;

	case fddiMACVendorAttrib:
		if (smtmsg->vendorID == xdiMACSM_MA_AvailFlag)
		{
			FBMDATA (stnID, fbmMACData[smtmsg->entity].SM_MA_Avail)
				= smtmsg->data.b8;
		}
		break;
	}

	return;
}

/*
 * Only include the routines to process flexible configuration message
 * if the XM_USE_FLEX_CONF constant is defined. The two routine affected
 * are: ProcessFBMMACChange() and ProcessFBMPORTChange().
 */
#ifdef XM_USE_FLEX_CONF

void
#ifdef __STDC__
ProcessFBMMACChange (SMTMessage * smtmsg)
#else
ProcessFBMMACChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a flexible configuration change for a MAC.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
FBMStationInfoType *stnPtr;				/* station info */
FBMMACInfoType  *macPtr;				/* MAC element pointer */
MIBMACAddType   *addInfo;				/* add msg info */
MIBMACInsertType *insertInfo;			/* insert msg info */
uInt16           stnID;					/* station in msg */
uInt16           resID;					/* resource ID counter */
uInt16           i;						/* loop counter */

	/* get MAC array element to use */
	stnID = smtmsg->stnID;
	stnPtr = &(FBMDATA (stnID, fbmStationData));
	macPtr = &(FBMDATA (stnID, fbmMACData[smtmsg->entity]));

	/* select message type */
	switch (smtmsg->type)
	{
	case MIB_EVENT_ADD_MAC:
		addInfo = smtmsg->p1.MACAddBuf;

		/*
		 * If index is set, then this MAC is either still in use or was not
		 * deleted. Ignore this request.
		 */
		if (macPtr->Index != 0)
		{
			break;
		}

		else
		{
			/* initialize info */
			macPtr->Index = addInfo->index;

			/* add MAC to MIB count */
			stnPtr->macCount++;
		}


		break;

	case MIB_EVENT_INSERT_MAC:
		insertInfo = smtmsg->p1.MACInsertBuf;

		/* if index is not set */
		if (macPtr->Index == 0)
		{
			/* then this MAC has not been added */
			break;
		}

		else
		{
			/* initialize info */
			macPtr->hwPresent = PRESENT;
			MCopyAddress (macPtr->SMTAddress, insertInfo->SMTAddress);

			/* add MAC to present count */
			stnPtr->macPresent++;
		}
		break;

	case MIB_EVENT_REMOVE_MAC:
		macPtr->hwPresent = NOTPRESENT;

		/* clear hardware-dependent info */
		InitFBMMACHWData (macPtr);

		/* remove MAC from present count */
		stnPtr->macPresent--;

		break;

	case MIB_EVENT_DELETE_MAC:
		/* clear base info (clears index) */
		InitFBMMACData (macPtr);

		/* remove MAC from MIB count */
		stnPtr->macCount--;

		break;
	}

	/* reset MAC resource IDs */
	resID = stnPtr->attachPresent + stnPtr->masterPresent + 1;

	for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		if (macPtr->hwPresent)
		{
			macPtr->resourceID = resID;
			resID++;
		}
	}

	return;
}

void
#ifdef __STDC__
ProcessFBMPORTChange (SMTMessage * smtmsg)
#else
ProcessFBMPORTChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a flexible configuration change for a PORT.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
FBMStationInfoType *stnPtr;				/* station info */
FBMPORTInfoType *portPtr;				/* PORT element pointer */
FBMMACInfoType  *macPtr;				/* MAC element pointer */
MIBPORTAddType  *addInfo;				/* add msg info */
uInt16           stnID;					/* station in msg */
uInt16           resID;					/* resource ID counter */
uInt16           i;						/* loop counter */

	/* get PORT array element to use */
	stnID = smtmsg->stnID;
	stnPtr = &(FBMDATA (stnID, fbmStationData));
	portPtr = &(FBMDATA (stnID, fbmPORTData[smtmsg->entity]));

	/* select message type */
	switch (smtmsg->type)
	{
	case MIB_EVENT_ADD_PORT:
		addInfo = smtmsg->p1.PORTAddBuf;

		/*
		 * If index is set, then this PORT is either still in use or was not
		 * deleted. Ignore this request.
		 */
		if (portPtr->Index != 0)
		{
			break;
		}

		else
		{
			/* initialize info */
			portPtr->portType = addInfo->PC_Type;
			switch (portPtr->portType)
			{
			case PT_a:
				/* set station index value */
				stnPtr->A_Index = portPtr->ID;

				/* increment attach count */
				stnPtr->attachCount++;
				break;

			case PT_b:
				/* set station index value */
				stnPtr->B_Index = portPtr->ID;

				/* increment attach count */
				stnPtr->attachCount++;
				break;

			case PT_s:
				/* set station index value */
				stnPtr->S_Index = portPtr->ID;

				/* increment attach count */
				stnPtr->attachCount++;
				break;

			case PT_m:
				/* increment master count */
				stnPtr->masterCount++;
				break;
			}
			portPtr->Index = addInfo->index;
		}
		break;

	case MIB_EVENT_INSERT_PORT:
		/* if index is not set */
		if (portPtr->Index == 0)
		{
			/* then this PORT has not been added */
			break;
		}

		else
		{
			/* initialize info */
			portPtr->hwPresent = PRESENT;

			/* set port present count */
			if (portPtr->portType == PT_m)
				stnPtr->masterPresent++;
			else
				stnPtr->attachPresent++;
		}
		break;

	case MIB_EVENT_REMOVE_PORT:
		portPtr->hwPresent = NOTPRESENT;

		/* set port present count */
		if (portPtr->portType == PT_m)
			stnPtr->masterPresent--;
		else
			stnPtr->attachPresent--;

		/* clear hardware-dependent info */
		InitFBMPORTHWData (portPtr);

		break;

	case MIB_EVENT_DELETE_PORT:
		/* remove port from station index if A, B or S */
		switch (portPtr->portType)
		{
		case PT_a:
			stnPtr->A_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_b:
			stnPtr->B_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_s:
			stnPtr->S_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_m:
			stnPtr->masterCount--;
			break;
		}

		/* clear base info (clears index) */
		InitFBMPORTData (portPtr);
		break;
	}

	/* reset PORT and MAC resource IDs */
	resID = 1;

	for (i = 0, portPtr = FBMDATA (stnID, fbmPORTData);
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		if (portPtr->hwPresent)
		{
			portPtr->resourceID = resID;
			resID++;
		}
	}

	for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		if (macPtr->hwPresent)
		{
			macPtr->resourceID = resID;
			resID++;
		}
	}

	return;
}

#endif							/* XM_USE_FLEX_CONF */

void
#ifdef __STDC__
ProcessFBMMessageMulti (SMTMessage * msg)
#else
ProcessFBMMessageMulti (msg)
		SMTMessage      *msg;
#endif
/*********************************************************************
Function:	Process a message received by FBM.
Parameters:	msg	= address of buffer containing message.
Input:		msg	= complete contents of message.
Output:		Performs actions appropriate to message type and info.
Return:		None.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID from message */

	stnID = msg->stnID;

	/*
	 * Process only if FBM is initialized.
	 */
	if (!FBMDATA (stnID, fbmStationData.FBMInitialized))
	{
		return;
	}

	/*
	 * Select message processing.
	 */
	switch (msg->type)
	{

		/*
		 * Process events.
		 */
	case MIB_EVENT_NOTIFY_CHANGE:
		ProcessFBMMIBChange (msg);
		break;

	case MIB_EVENT_NOTIFY_SMT:
		ProcessSRPReport (msg);
		break;

/*
 * Only call these message processing routines if flexible configuration
 * is supported by the system.
 */
#ifdef XM_USE_FLEX_CONF

	case MIB_EVENT_ADD_MAC:
	case MIB_EVENT_INSERT_MAC:
	case MIB_EVENT_REMOVE_MAC:
	case MIB_EVENT_DELETE_MAC:
		ProcessFBMMACChange (msg);
		break;

	case MIB_EVENT_ADD_PORT:
	case MIB_EVENT_INSERT_PORT:
	case MIB_EVENT_REMOVE_PORT:
	case MIB_EVENT_DELETE_PORT:
		ProcessFBMPORTChange (msg);
		break;

#endif							/* XM_USE_FLEX_CONF */

		/*
		 * Process actions.
		 */
	case FBM_ACTION_SEND_FRAME:
		ProcessSendFrame (msg);
		break;

	case FBM_ACTION_MONITOR_NIF:
		/*
		 * Set NIF monitoring mode.
		 */
		FBMDATA (msg->stnID, fbmStationData.monitorMode)
			= (msg->data.b32 == ENABLE)
		/* turn on monitoring */
			? FBMDATA (msg->stnID, fbmStationData.monitorMode)
			| FMM_MONITOR_NIF
		/* turn off monitoring */
			: FBMDATA (msg->stnID, fbmStationData.monitorMode)
			& ~FMM_MONITOR_NIF;

		/*
		 * Send reponse message.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_RESPONSE_MONITOR_NIF;

		SendFBMMessage (msg);
		break;

	case FBM_ACTION_MONITOR_SMT:
		/*
		 * Set SMT frame monitoring mode.
		 */
		FBMDATA (msg->stnID, fbmStationData.monitorMode)
			= (msg->data.b32 == ENABLE)
		/* turn on monitoring */
			? FBMDATA (msg->stnID, fbmStationData.monitorMode)
			| FMM_MONITOR_SMT
		/* turn off monitoring */
			: FBMDATA (msg->stnID, fbmStationData.monitorMode)
			& ~FMM_MONITOR_SMT;

		/*
		 * Send reponse message.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_RESPONSE_MONITOR_SMT;

		SendFBMMessage (msg);
		break;

#ifdef XM_USE_RAF
	case FBM_ACTION_SBA:
	case FBM_ACTION_SBA_CONTROL:
		ProcessSBAMessage (msg);
		break;

#else							/* XM_USE_RAF */

	case FBM_ACTION_SBA:
	case FBM_ACTION_SBA_CONTROL:
		/*
		 * Send error message.
		 */
		msg->destination = msg->source;
		msg->source = FBM_MSG_ID;
		msg->type = FBM_ERROR_MSG;
		msg->typeInfo = EFBM_RAF_NOT_SUPPORTED;
		SendFBMMessage (msg);
		break;
#endif							/* XM_USE_RAF */

		/*
		 * Handle all others locally.
		 */
	default:
		LocalFBMMessages (msg);
		break;
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

void
#ifdef __STDC__
PassMAPFrame (uInt16 messageType, Void_p framePtr, uInt16 frameLen,
	uInt16 MACNum, uChar EACbits)
#else
PassMAPFrame (messageType, framePtr, frameLen, MACNum, EACbits)
		uInt16           messageType;
		Void_p           framePtr;
		uInt16           frameLen;
		uInt16           MACNum;
		uChar            EACbits;
#endif
{

	PassMAPFrameMulti (FIRST_STATION, messageType, framePtr,
		frameLen, MACNum, EACbits);
}

void
#ifdef __STDC__
ProcessFBMMessage (SMTMessage * msg)
#else
ProcessFBMMessage (msg)
		SMTMessage      *msg;
#endif
{

	msg->stnID = FIRST_STATION;
	ProcessFBMMessageMulti (msg);
}

#endif							/* MULTIPLE_STATION */
