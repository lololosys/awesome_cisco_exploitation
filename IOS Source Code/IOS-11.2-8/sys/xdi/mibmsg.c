/* $Id: mibmsg.c,v 3.2 1995/11/17 18:10:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibmsg.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibmsg.c,v $
 * Revision 3.2  1995/11/17  18:10:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibmsg.c xm7_v3.1.1:cset.0166:24:24 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		Message processing module.

		File:			xm7_mibmsg.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module provides for message services for the Management
		Information Base (MIB).

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_VSO_MASIC	include code for MASIC object support.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"smtmsg.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
	Global Variables
*********************************************************************/

/*
*	Undesirable connection table. Any position set to 1 indicates
*	an undesirable connection as defined by the FDDI Connection Rules.
*/
MCONST
static uChar     undesirableTable[4][4] =
{
 /* A	B	S	M */
	{1, 0, 1, 0},						/* A */
	{0, 1, 1, 0},						/* B */
	{1, 1, 0, 0},						/* S */
	{0, 0, 0, 1}						/* M */
};


/*********************************************************************
	MIB Event Message Generation
*********************************************************************/

void
#ifdef __STDC__
SendSMTEventMulti (uInt16 stnID, uInt16 eventType, uInt16 entity,
	uInt32 vendorID)
#else
SendSMTEventMulti (stnID, eventType, entity, vendorID)
		uInt16           stnID;
		uInt16           eventType;
		uInt16           entity;
		uInt32           vendorID;
#endif
/*********************************************************************
Function:	Send a condition or event notification to the appropriate
			Manager tasks and to the MAP. Sending the notification
			to FSP supports the Status Report Protocol (SRP). This
			routine also updates the transition time stamp as
			specified in the SRP.
Parameters:	stnID		= station index to use.
			eventType	= notification identifier as defined in the
						SMT MIB for SMT notification or the internal
						16-bit ID for vendor notifications.
			entity		= Manager index of entity reporting the
							event (0 if not needed).
			vendorID	= if eventType indicates a vendor-specific
							notification, then this parameter holds the
							vendor ID for the notification. Otherwise
							this value should be zero.
Input:		mib			= uses current information in MIB to
							report the event or condition.
Output:		mib			= may update the MIB for condition status
							and time stamping of events.
Return:		No value returned.
Notes:		This routine must only be called when a notificiation's
			condition state changes or an event occurs. Duplicate
			calls will change the value of the transition time stamp.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* message to send */
MIBNotificationType notice;				/* notification info */
MIBType         *mibPtr;				/* MIB database */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Enter default message data. Always send to FSP. msg.len1 is set before
	 * sending message below.
	 */
	msg.source = MIB_MSG_ID;
	msg.destination = FBM_MSG_ID;
	msg.type = MIB_EVENT_NOTIFY_SMT;
	msg.typeInfo = eventType;
	msg.entity = entity;
	msg.localID = 0;
	msg.len1 = SIZE_MIBNotificationType;
	msg.p1.mibNoticeBuf = &notice;
	msg.len2 = 0;
	msg.p2 = (Void_p) NULL;
	msg.stnID = stnID;
	msg.data.b32 = (uInt32) 0;			/* Clear data field */
	msg.vendorID = vendorID;			/* in case this is vendor-specific */

	notice.state = 0;
	notice.type = 0;					/* set default to indicate no msg */
	notice.info = 0;					/* clear extra info */

	/*
	 * Select event processing.
	 */
	switch (eventType)
	{
	case fddiSMTPeerWrapCondition:
		notice.type = EC_TYPE_CONDITION;
		notice.state = mibPtr->smtData.PeerWrapFlag;
		break;

	case fddiSMTVendorNotification:
		/*
		 * The object identifier specifies a vendor-specific event or an
		 * unknown value. Call the local event processing routine to handle
		 * this case.
		 */
		ProcessVendorMIBEvent (&msg, &notice);
		break;

	case fddiMACNeighborChangeEvent:
		notice.type = EC_TYPE_EVENT;
		/* no data associated with this event */
		break;

	case fddiMACDuplicateAddressCondition:
		notice.type = EC_TYPE_CONDITION;

		/* set value to indicate conditions active */
		if (mibPtr->macBase[entity].hwData.DA_Flag)
			notice.info |= DC_mydup;
		if (mibPtr->macBase[entity].hwData.UNDA_Flag)
			notice.info |= DC_unadup;

		/* set condition state */
		notice.state = (notice.info) ? TRUE : FALSE;
		break;

	case fddiMACFrameErrorCondition:
		notice.type = EC_TYPE_CONDITION;
		notice.state = mibPtr->macBase[entity].hwData.FrameErrorFlag;
		break;

	case fddiMACNotCopiedCondition:
		notice.type = EC_TYPE_CONDITION;
		notice.state = mibPtr->macBase[entity].hwData.NotCopiedFlag;
		break;

	case fddiMACPathChangeEvent:
		notice.type = EC_TYPE_EVENT;
		notice.info = mibPtr->macBase[entity].hwData.CurrentPath;
		break;

	case fddiMACVendorNotification:
		if (vendorID == xdiMACConfigurationFailedEvent)
		{
			notice.type = EC_TYPE_EVENT;
			notice.info
				= mibPtr->macBase[entity].hwData.xdi.ConfigurationStatus;
		}

		else if (vendorID == xdiMACSMTAddressChangedEvent)
		{
			notice.type = EC_TYPE_EVENT;
		}

		else
		{
			/*
			 * The object identifier either specifies a vendor-specific
			 * event. Call the local event processing routine to handle this
			 * case.
			 */
			ProcessVendorMIBEvent (&msg, &notice);
		}
		break;

	case fddiPATHVendorNotification:
		/*
		 * The object identifier either specifies a vendor-specific event.
		 * Call the local event processing routine to handle this case.
		 */
		ProcessVendorMIBEvent (&msg, &notice);
		break;

	case fddiPORTLerCondition:
		notice.type = EC_TYPE_CONDITION;
		notice.state = mibPtr->portBase[entity].hwData.LerFlag;
		break;

	case fddiPORTUndesiredConnectionAttemptEvent:
		notice.type = EC_TYPE_EVENT;
		/* no data associated with this event */
		break;

	case fddiPORTPathChangeEvent:
		notice.type = EC_TYPE_EVENT;
		notice.info = mibPtr->portBase[entity].hwData.CurrentPath;
		break;

	case fddiPORTEBErrorCondition:
		notice.type = EC_TYPE_CONDITION;
		notice.state = mibPtr->portBase[entity].hwData.xdi.EBErrorFlag;
		break;

	case fddiPORTVendorNotification:
		if (vendorID == xdiPORTLCTFailedEvent)
		{
			notice.type = EC_TYPE_EVENT;
			/* no data associated with this event */
		}

		else
			/*
			 * The object identifier either specifies a vendor-specific
			 * event. Call the local event processing routine to handle this
			 * case.
			 */
			ProcessVendorMIBEvent (&msg, &notice);

		break;

	default:
		/*
		 * The object identifier either specifies an unknown value. Clear the
		 * notice type to skip any further processing.
		 */
		notice.type = 0;
		break;
	}

	/*
	 * If notice.type is set, then send this message.
	 */
	if (notice.type)
	{
		/* set transition time stamp as per SRP */
		GetTimeStampMulti (stnID, &mibPtr->smtData.TransitionTimeStamp);

		/* send message */
		SendMIBMessage (&msg);

		/* send a copy to the MAP */
		msg.destination = MAP_MSG_ID;

		/*
		 * Set entity member to the MIB resource index in message going to
		 * the MAP.
		 */
		switch (eventType & MIB_OBJECT_MASK)
		{
		case fddiMAC:					/* standard attribute ID */
			msg.entity = mibPtr->macBase[entity].baseData.Index;
			break;

		case fddiPATH:					/* standard attribute ID */
			msg.entity = mibPtr->pathBase[entity].baseData.Index;
			break;

		case fddiPORT:					/* standard attribute ID */
			msg.entity = mibPtr->portBase[entity].baseData.Index;
			break;

		default:
			/* no entity */
			break;
		}

		/*
		 * Send notification to the MAP.
		 */
		SendMIBMessage (&msg);
	}

	return;
}

void
#ifdef __STDC__
DetectMIBConditions (uInt16 stnID)
#else
DetectMIBConditions (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	When requested, either based on a timer or request from
			another process, MSP checks the MIB values for the
			threshold conditions to be reported by SRFs. This routine
			not only checks for new threshold conditions, it also
			checks for active conditions on inactive objects.
			Currently, this check only applies to the LerCondition.
			The SMT and threshold conditions are cleared when they
			are re-evaluated.
Parameters:	stnID		= index of station to use.
Input:		Uses the MIB.
Output:		Updates MIB condition values.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt32           lost,					/* values used for delta calcs */
                 error,
                 frame,
                 notCopied,
                 copied,
                 temp,
                 deltaTop,
                 deltaBottom;
uInt32           result;				/* request result value */
FDDIFlag         state;					/* state value */
uInt16           i;						/* loop counter */
MIBType         *mibPtr;				/* MIB database */
MACDataPtr       macPtr;				/* MAC element */
PORTDataPtr      portPtr;				/* PORT element */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	for (i = 0, macPtr = mibPtr->macBase;
		i < mibPtr->smtData.Mac_Ct;
		i++, macPtr++)
	{
		/* skip if not in MIB or not present */
		if ((!macPtr->inUse) || (!macPtr->baseData.HardwarePresent))
			continue;

/**
* MAC Frame Error Condition
*
* This condition exists when the FrameErrorRatio exceeds the
* FrameErrorThreshold:
*	x = deltaLost + deltaError
*	y = deltaFrame + deltaLost
*	The definitions of frame, error and lost count imply x <= y.
*	The condition exists when
*		threshold < (x * 2^16) / y
* Since lost and error counts are 32-bit values, multiplying by 2^16 may
* cause an overflow. To avoid this condition, if x > 2^16, then y must
* also be > 2^16. Therefore, instead of x * 2^16, the ratio is calculated
* as x / (y / 2^16). If either x or y is 0, then the condition doesn't
* exist. The code below prefers the y / 2^16, so it checks if y >= 2^16
* first.
**/
		/* get lost count values */
		state = FALSE;
		lost = error = frame = 0;
		result = GetMACDataMulti (stnID, fddiMACLost_Ct,
			macPtr, (uChar *) & lost);
		if (result != RC_Success)
			/*
			 * These are mandatory counters. If an error occurs, then skip
			 * this MAC.
			 */
			continue;

		/* get error count values */
		result = GetMACDataMulti (stnID, fddiMACError_Ct,
			macPtr, (uChar *) & error);
		if (result != RC_Success)
			continue;

		/* get frame count values */
		result = GetMACDataMulti (stnID, fddiMACFrame_Ct,
			macPtr, (uChar *) & frame);
		if (result != RC_Success)
			continue;

		/*
		 * Get delta values. Checking for counter wrap condition is done. If
		 * the counter wraps, then the subtraction is negated. The delta lost
		 * value is stored in temp because it is used in the top and bottom.
		 */
		temp = lost - macPtr->hwData.xdi.lastLost;
		if (lost < macPtr->hwData.xdi.lastLost)
			temp = -temp;

		deltaTop = error - macPtr->hwData.xdi.lastError;
		if (error < macPtr->hwData.xdi.lastError)
			deltaTop = -deltaTop;
		deltaTop += temp;				/* add delta lost */

		deltaBottom = frame - macPtr->hwData.xdi.lastFrame;
		if (frame < macPtr->hwData.xdi.lastFrame)
			deltaBottom = -deltaBottom;
		deltaBottom += temp;			/* add delta lost */

		/* calculate only when both values are available */
		if (deltaTop && deltaBottom)
		{
			if (deltaBottom > ((uInt32) 0x0FFFF))
			{
				/* y >= 2^16 so do division by shift */
				deltaBottom >>= 16;
				/* clear upper bits */
				deltaBottom &= ((uInt32) 0x0FFFF);
			}

			else
			{
				/* multiply x by 2^16 */
				deltaTop <<= 16;
			}

			/* get ratio */
			deltaTop /= deltaBottom;
			macPtr->hwData.FrameErrorRatio = (uInt16) deltaTop;

			/* get condition state */
			state = (deltaTop > macPtr->baseData.FrameErrorThreshold)
				? TRUE : FALSE;
		}

		/* check for change of state */
		if (state != macPtr->hwData.FrameErrorFlag)
		{
			/* update state values */
			macPtr->hwData.FrameErrorFlag = state;
			SendSMTEventMulti (stnID, fddiMACFrameErrorCondition, i,
				(uInt32) 0);
			LocalMIBEventMulti (stnID, mibPtr, fddiMACFrameErrorFlag, i,
				(uInt32) 0);
		}

		/*
		 * Update base values for next interval.
		 */
		macPtr->hwData.xdi.lastLost = lost;
		macPtr->hwData.xdi.lastError = error;
		macPtr->hwData.xdi.lastFrame = frame;

/**
* MAC Not Copied Condition
*
* This condition is calculated in a similar manner to the frame error
* condition above. It is only calculated when the framenotcopied and
* framenotcopiedmanagement packages are supported.
**/
		if ((macPtr->baseData.xdi.packages & PKG_framenotcopied)
			&& (macPtr->baseData.xdi.packages & PKG_framenotcopiedmanagement))
		{
			state = FALSE;
			notCopied = copied = 0;

			/* get not copied count values */
			result = GetMACDataMulti (stnID, fddiMACNotCopied_Ct,
				macPtr, (uChar *) & notCopied);
			if (result != RC_Success)
				/* optional parameter, skip this ratio */
				continue;

			/* get copied count values */
			result = GetMACDataMulti (stnID, fddiMACCopied_Ct,
				macPtr, (uChar *) & copied);
			if (result != RC_Success)
				/* error, go to next MAC */
				continue;

			/* get deltas */
			deltaTop = notCopied - macPtr->hwData.xdi.lastNotCopied;
			if (notCopied < macPtr->hwData.xdi.lastNotCopied)
				deltaTop = -deltaTop;

			deltaBottom = copied - macPtr->hwData.xdi.lastCopied;
			if (copied < macPtr->hwData.xdi.lastCopied)
				deltaBottom = -deltaBottom;
			deltaBottom += deltaTop;	/* add notCopied delta */

			/* calculate only when both values are available */
			if (deltaTop && deltaBottom)
			{
				if (deltaBottom > ((uInt32) 0x0FFFF))
				{
					/* y >= 2^16 so do division by shift */
					deltaBottom >>= 16;
					/* clear upper bits */
					deltaBottom &= ((uInt32) 0x0FFFF);
				}

				else
				{
					/* multiply x by 2^16 */
					deltaTop <<= 16;
				}

				/* get ratio */
				deltaTop /= deltaBottom;
				macPtr->hwData.NotCopiedRatio = (uInt16) deltaTop;

				/* get condition state */
				state = (deltaTop > macPtr->baseData.NotCopiedThreshold)
					? TRUE : FALSE;
			}

			/* check for change of state */
			if (state != macPtr->hwData.NotCopiedFlag)
			{
				/* update state values */
				macPtr->hwData.NotCopiedFlag = state;
				SendSMTEventMulti (stnID, fddiMACNotCopiedCondition, i,
					(uInt32) 0);
				LocalMIBEventMulti (stnID, mibPtr, fddiMACNotCopiedFlag, i,
					(uInt32) 0);
			}

			/*
			 * Update base values for next interval.
			 */
			macPtr->hwData.xdi.lastNotCopied = notCopied;
			macPtr->hwData.xdi.lastCopied = copied;
		}
	}

	for (i = 0, portPtr = mibPtr->portBase;
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		/* skip if not in MIB or not present */
		if ((!portPtr->inUse) || (!portPtr->baseData.HardwarePresent))
			continue;

		/* check for inactive port */
		if (portPtr->hwData.PCMState != PCM_pc8)
		{
			/* check for LerCondition active */
			if (portPtr->hwData.LerFlag)
			{
				/* clear status */
				portPtr->hwData.LerFlag = FALSE;
				SendSMTEventMulti (stnID, fddiPORTLerCondition, i,
					(uInt32) 0);
				LocalMIBEventMulti (stnID, mibPtr, fddiPORTLerFlag, i,
					(uInt32) 0);
			}
		}

/*
 * PHY EB Error Condition
 */
		/* get EB error count value */
		result = GetPORTDataMulti (stnID, fddiPORTEBError_Ct,
			portPtr, (uChar *) & temp);
		if (result != RC_Success)
			/* optional parameter not supported */
			continue;

		state = (temp != portPtr->hwData.xdi.lastEB)
			? TRUE : FALSE;

		if (state != portPtr->hwData.xdi.EBErrorFlag)
		{
			/* update state values */
			portPtr->hwData.xdi.EBErrorFlag = state;
			SendSMTEventMulti (stnID, fddiPORTEBErrorCondition, i,
				(uInt32) 0);
			LocalMIBEventMulti (stnID, mibPtr, fddiPORTEBErrorCondition, i,
				(uInt32) 0);
			portPtr->hwData.xdi.lastEB = temp;
		}
	}

/*
 * If including the MASIC object, then check for notifications from
 * the MASICs.
 */
#ifdef	XM_USE_VSO_MASIC

	DetectMASICNotifications (stnID, mibPtr);

#endif							/* XM_USE_VSO_MASIC */

	/*
	 * Check for vendor-specific notifications including those related to
	 * vendor-specific objects.
	 */
	DetectVendorNotificationsMulti (stnID, mibPtr);

	return;
}


/*********************************************************************
	Process MIB Messages Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessMIBAction (SMTMessage * smtmsg)
#else
ProcessMIBAction (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process an action message sent to the MIB. The only
			actions handled by the MIB are actions to set attributes
			in the MIB.
Parameters:	smtmsg	= buffer for message received by MIB for processing.
Input:		smtmsg	= contains action message to process.
Output:		Depends upon action.
Return:		No value returned.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID from message */
MIBMsgType      *mibInfo;				/* message format */

	stnID = smtmsg->stnID;
	/* Perform requested action. */
	switch (smtmsg->type)
	{
		/*
		 * MIB Operations
		 */
	case MIB_ACTION_GET_ATTR:
	case MIB_ACTION_MULTIGET_ATTR:
		/* Fill response message. */
		smtmsg->destination = smtmsg->source;
		smtmsg->source = MIB_MSG_ID;
		mibInfo = smtmsg->p1.mibMsgBuf;

		/*
		 * As a precaution, the MIB Get operation now has two entry points.
		 * The default, MIB_ACTION_GET_ATTR, only allows single attribute
		 * Gets. The alternate, MIB_ACTION_MULTIGET_ATTR, allows multiple
		 * attribute Gets. This prevents older (i.e., prior to SMT 7.x) MAP
		 * code from using buffers with excess data in them. The multiple Get
		 * operation requires a parameter of type zero (0) to be the last
		 * parameter in the request buffer.
		 */
		if (smtmsg->type == MIB_ACTION_GET_ATTR)
		{
			/* Get MIB information for one attribute */
			mibInfo->result = GetMIBAttrMulti (stnID, smtmsg->len2,
				smtmsg->p2, &mibInfo->setCount);
		}

		else
		{
			/* Get MIB information for possible multiple attrs */
			mibInfo->result = MultiGetMIBAttrMulti (stnID,
				smtmsg->len2, smtmsg->p2, &mibInfo->setCount);
		}

		/*
		 * Set return message type.
		 */
		smtmsg->type = MIB_RESPONSE_GET_ATTR;

		/* Return response. */
		SendMIBMessage (smtmsg);
		break;

	case MIB_ACTION_CHANGE_ATTR:
		/* Fill response message. */
		smtmsg->destination = smtmsg->source;
		smtmsg->source = MIB_MSG_ID;
		smtmsg->type = MIB_RESPONSE_CHANGE_ATTR;

		/* Change MIB information. */
		mibInfo = smtmsg->p1.mibMsgBuf;
		mibInfo->result = ChangeMIBAttrMulti (stnID, smtmsg->len2,
			smtmsg->p2, &mibInfo->setCount, &mibInfo->setStationID);

		/* Return response. */
		SendMIBMessage (smtmsg);
		break;

	case MIB_ACTION_DETECT_CONDITIONS:
		/* Look for conditions */
		DetectMIBConditions (stnID);
		break;

		/* get attribute using ID provided by CSP or FSP */
	case MIB_ACTION_SHORT_GET_ATTR:
		/* Fill response message. */
		smtmsg->destination = smtmsg->source;
		smtmsg->source = MIB_MSG_ID;
		smtmsg->type = MIB_RESPONSE_GET_ATTR;

		/* Get MIB information for CSP or FSP */
		mibInfo = smtmsg->p1.mibMsgBuf;
		mibInfo->result = GetXMMIBAttrMulti (smtmsg->stnID,
			smtmsg->len2, smtmsg->p2);

		/* Return response. */
		SendMIBMessage (smtmsg);
		break;

	default:
		LocalMIBMessages (smtmsg);
		break;
	}

	return;
}

void
#ifdef __STDC__
ProcessMIBEvent (SMTMessage * smtmsg)
#else
ProcessMIBEvent (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process an event message sent to the MIB.
Parameters:	smtmsg	= buffer for message received by MIB for processing.
Input:		smtmsg	= contains request message to process.
Output:		Issues indication message.
Return:		No value returned.
Modification History:
*********************************************************************/
{
Int32            temp;					/* comparison result */
uInt16           stnID;					/* station ID from message */
MIBType         *mibPtr;				/* MIB database */
MACDataPtr       macPtr;				/* MAC element */
PORTDataPtr      portPtr;				/* PORT element */
Int16            compare;				/* Address compare result */

	/**
	 * WRF
	 * Initialize some variables to avoid compiler warnings.
	 **/
	macPtr = NULL;
	portPtr = NULL;

	/*
	 * Set MIB database to use.
	 */
	stnID = smtmsg->stnID;
	mibPtr = MIBDATA (stnID, mib);

	switch (smtmsg->type)
	{
	case CSP_EVENT_NOTIFY_MIB:
	case FBM_EVENT_NOTIFY_MIB:
		switch (smtmsg->typeInfo & MIB_OBJECT_MASK)
		{
		case fddiMAC:					/* standard attribute ID */
			macPtr = &mibPtr->macBase[smtmsg->entity];
			break;

		case fddiPORT:					/* standard attribute ID */
			portPtr = &mibPtr->portBase[smtmsg->entity];
			break;
		}

		switch (smtmsg->typeInfo)
		{

			/*
			 * SMT Attributes
			 */
		case fddiSMTECMState:
			mibPtr->smtData.ECMState = smtmsg->data.b16;
			break;

		case fddiSMTCF_State:
			mibPtr->smtData.CF_State = smtmsg->data.b16;
			break;

		case fddiSMTPeerWrapFlag:
			mibPtr->smtData.PeerWrapFlag = smtmsg->data.b8;

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiSMTPeerWrapCondition, 0,
				(uInt32) 0);
			break;

			/*
			 * MAC Attributes
			 */
		case fddiMACCurrentPath:
			/* save old value */
			temp = macPtr->hwData.CurrentPath;

			/* set new value */
			macPtr->hwData.CurrentPath = smtmsg->data.b16;

			/*
			 * Suppress event if isolated->local or local->isolated. The MAC
			 * CFM only allows transitions between isolated and local so we
			 * only need to check if new value is different and neither is
			 * local.
			 */
			if ((temp != macPtr->hwData.CurrentPath)
				&& (temp != CP_local)
				&& (macPtr->hwData.CurrentPath != CP_local))
			{
				/* notify SMT */
				SendSMTEventMulti (stnID, fddiMACPathChangeEvent,
					smtmsg->entity, (uInt32) 0);
			}
			break;

		case fddiMACUpstreamNbr:
			/* save old address if one exists */
			MCompareAddress (macPtr->hwData.UpstreamNbr,
				SMT_Unknown_Address, compare);
			if (compare != 0)
			{
				/* Save old address */
				MCopyAddress (macPtr->hwData.OldUpstreamNbr,
					macPtr->hwData.UpstreamNbr);
			}

			/* set new address */
			MCopyAddress (macPtr->hwData.UpstreamNbr, smtmsg->data.addr);

			/* set event flags */
			macPtr->hwData.xdi.neighborFlags |= NAC_unachange;

			/* Notify CSP of new UNA */
			smtmsg->destination = CSP_MSG_ID;
			smtmsg->source = MIB_MSG_ID;
			smtmsg->type = MIB_EVENT_NOTIFY_CHANGE;
			SendMIBMessage (smtmsg);

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiMACNeighborChangeEvent,
				smtmsg->entity, (uInt32) 0);
			break;

		case fddiMACDownstreamNbr:
			/* save old address if one exists */
			MCompareAddress (macPtr->hwData.DownstreamNbr,
				SMT_Unknown_Address, compare);
			if (compare != 0)
			{
				/* Save old address */
				MCopyAddress (macPtr->hwData.OldDownstreamNbr,
					macPtr->hwData.DownstreamNbr);
			}

			/* set new address */
			MCopyAddress (macPtr->hwData.DownstreamNbr, smtmsg->data.addr);

			/* set event flags */
			macPtr->hwData.xdi.neighborFlags |= NAC_dnachange;

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiMACNeighborChangeEvent,
				smtmsg->entity, (uInt32) 0);
			break;

		case fddiMACDupAddressTest:
			macPtr->hwData.DupAddressTest = smtmsg->data.b16;

			/* Pass the message on to CSP for RMT processing */
			smtmsg->destination = CSP_MSG_ID;
			smtmsg->source = MIB_MSG_ID;
			smtmsg->type = MIB_EVENT_NOTIFY_CHANGE;
			SendMIBMessage (smtmsg);

			/*
			 * No SMT event is given here. RMT will receive the test result
			 * and set fddiMACDA_Flag accordingly. This gets sent back to the
			 * MIB and the event is driven of that attribute.
			 */
			break;

		case fddiMACRMTState:
			macPtr->hwData.RMTState = smtmsg->data.b16;
			break;

		case fddiMACDA_Flag:
			macPtr->hwData.DA_Flag = smtmsg->data.b8;

			/* notify FSP */
			smtmsg->destination = FBM_MSG_ID;
			smtmsg->source = MIB_MSG_ID;
			smtmsg->type = MIB_EVENT_NOTIFY_CHANGE;
			SendMIBMessage (smtmsg);

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiMACDuplicateAddressCondition,
				smtmsg->entity, (uInt32) 0);
			break;

		case fddiMACUNDA_Flag:
			macPtr->hwData.UNDA_Flag = smtmsg->data.b8;

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiMACDuplicateAddressCondition,
				smtmsg->entity, (uInt32) 0);
			break;

		case fddiMACMA_UnitdataAvailable:
			macPtr->hwData.MA_UnitdataAvailable = smtmsg->data.b8;
			break;

		case fddiMACSMTAddress:
			/* RMT changed MAC's address */
			MCopyAddress (macPtr->hwData.SMTAddress, smtmsg->data.addr);

			/* notify FSP */
			smtmsg->destination = FBM_MSG_ID;
			smtmsg->source = MIB_MSG_ID;
			smtmsg->type = MIB_EVENT_NOTIFY_CHANGE;
			SendMIBMessage (smtmsg);

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiMACVendorNotification,
				smtmsg->entity, xdiMACSMTAddressChangedEvent);

			break;

		case fddiMACVendorAttrib:
			if (smtmsg->vendorID == xdiMACSM_MA_AvailFlag)
			{
				macPtr->hwData.xdi.SM_MA_AvailFlag = smtmsg->data.b8;

				/* notify FSP */
				smtmsg->destination = FBM_MSG_ID;
				smtmsg->source = MIB_MSG_ID;
				smtmsg->type = MIB_EVENT_NOTIFY_CHANGE;
				SendMIBMessage (smtmsg);
			}

			else if (smtmsg->vendorID == xdiMACConfigurationStatus)
			{
				/* MAC/PATH parameter mismatch */
				macPtr->hwData.xdi.ConfigurationStatus = smtmsg->data.b16;

				/* if configuration failed */
				if (macPtr->hwData.xdi.ConfigurationStatus
					!= XDIMAC_configstatus_success)
				{
					/* notify SMT */
					SendSMTEventMulti (stnID, fddiMACVendorNotification,
						smtmsg->entity, xdiMACConfigurationFailedEvent);
				}
			}
			break;

			/*
			 * PATH Attributes
			 */
		case fddiPATHTraceStatus:
			mibPtr->pathBase[smtmsg->entity].baseData.TraceStatus
				= smtmsg->data.b16;
			break;

			/*
			 * PORT Attributes
			 */
		case fddiPORTNeighborType:
			portPtr->hwData.NeighborType = smtmsg->data.b16;

			/*
			 * Note: undesirable connection checked when PC_Withhold is
			 * reported
			 */
			break;

		case fddiPORTMACIndicated:
			/*
			 * The values reported from CSP give the bit positions. These
			 * need to be translated into flags for the parameter.
			 */
			portPtr->hwData.MACIndicated.t_val9 =
				(smtmsg->data.b8 & CSP_TVAL9) ? TRUE : FALSE;
			portPtr->hwData.MACIndicated.r_val9 =
				(smtmsg->data.b8 & CSP_RVAL9) ? TRUE : FALSE;
			break;

		case fddiPORTCurrentPath:
			/* save old value */
			temp = portPtr->hwData.CurrentPath;

			/* set new value */
			portPtr->hwData.CurrentPath = smtmsg->data.b16;

			/*
			 * Suppress event if isolated->local or local->isolated. The PORT
			 * CFM only allows transitions between isolated and local so we
			 * only need to check if new value is different and neither is
			 * local.
			 */
			if ((temp != portPtr->hwData.CurrentPath)
				&& (temp != CP_local)
				&& (portPtr->hwData.CurrentPath != CP_local))
			{
				/* notify SMT */
				SendSMTEventMulti (stnID, fddiPORTPathChangeEvent,
					smtmsg->entity, (uInt32) 0);
			}
			break;

		case fddiPORTBS_Flag:
			portPtr->hwData.BS_Flag = smtmsg->data.b8;
			break;

		case fddiPORTLCTFail_Ct:
			/*
			 * fddiPORTLCTFail-Ct represents the number of LCT failures for
			 * this port. It is only reported by CSP when a failure has
			 * occurred.
			 */
			portPtr->hwData.LCTFail_Ct++;

			/*
			 * If this port is in standby mode (i.e., its connection is being
			 * withheld), then report this failure as an XLNT Designs
			 * vendor-specific event. This lets network managers know that a
			 * possible back-up link is not available.
			 */
			if (portPtr->hwData.ConnectState == CS_standby)
				SendSMTEventMulti (stnID, fddiPORTVendorNotification,
					smtmsg->entity, xdiPORTLCTFailedEvent);
			break;

		case fddiPORTLer_Estimate:
			portPtr->hwData.Ler_Estimate = smtmsg->data.b8;
			break;

		case fddiPORTLem_Reject_Ct:
			portPtr->hwData.Lem_Reject_Ct = smtmsg->data.b32;
			break;

		case fddiPORTLem_Ct:
			portPtr->hwData.Lem_Ct = smtmsg->data.b32;
			break;

		case fddiPORTConnectState:
			portPtr->hwData.ConnectState = smtmsg->data.b16;
			break;

		case fddiPORTPCMState:
			portPtr->hwData.PCMState = smtmsg->data.b16;
			break;

		case fddiPORTPC_Withhold:
			portPtr->hwData.PC_Withhold = smtmsg->data.b16;

			/* check for undesirable connections */
			if (undesirableTable
				[portPtr->baseData.My_Type][portPtr->hwData.NeighborType])
			{
				/* notify SMT */
				SendSMTEventMulti (stnID,
					fddiPORTUndesiredConnectionAttemptEvent, smtmsg->entity,
					(uInt32) 0);
			}

			break;

		case fddiPORTLerFlag:
			portPtr->hwData.LerFlag = smtmsg->data.b8;

			/* notify SMT */
			SendSMTEventMulti (stnID, fddiPORTLerCondition, smtmsg->entity,
				(uInt32) 0);
			break;

		case fddiPORTVendorAttrib:
			if (smtmsg->vendorID == xdiPORTUpstreamMACNeighbor)
			{
				MCopyAddress (portPtr->hwData.xdi.OldUpstreamMACNeighbor,
					portPtr->hwData.xdi.UpstreamMACNeighbor);
				MCopyAddress (portPtr->hwData.xdi.UpstreamMACNeighbor,
					smtmsg->data.addr);
			}
			break;

		default:
			break;
		}

		LocalMIBEventMulti (stnID, mibPtr, smtmsg->typeInfo, smtmsg->entity,
			smtmsg->vendorID);
		break;

	default:
		LocalMIBMessages (smtmsg);
		break;
	}

	return;
}

void
#ifdef __STDC__
ProcessMIBMessageMulti (SMTMessage * smtmsg)
#else
ProcessMIBMessageMulti (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a message received by MIB.
Parameters:	smtmsg	= address of buffer containing message.
Input:		smtmsg	= complete contents of message.
Output:		Performs actions appropriate to message type and info.
Return:		None.
Modification History:
*********************************************************************/
{

	/*
	 * Select message processing.
	 */
	switch (MESSAGE_TYPE (smtmsg->type))
	{
	case EVENT_MSG:
		ProcessMIBEvent (smtmsg);
		break;

	case ACTION_MSG:
		ProcessMIBAction (smtmsg);
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
SendSMTEvent (uInt16 eventType, uInt16 entity, uInt32 vendorID)
#else
SendSMTEvent (eventType, entity, vendorID)
		uInt16           eventType;
		uInt16           entity;
		uInt32           vendorID;
#endif
{
	SendSMTEventMulti (FIRST_STATION, eventType, entity, vendorID);
}

void
#ifdef __STDC__
ProcessMIBMessage (SMTMessage * smtmsg)
#else
ProcessMIBMessage (smtmsg)
		SMTMessage      *smtmsg;
#endif
{

	smtmsg->stnID = FIRST_STATION;
	ProcessMIBMessageMulti (smtmsg);
	return;
}

#endif							/* MULTIPLE_STATION */
