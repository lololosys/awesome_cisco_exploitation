/* $Id: fbmsrp.c,v 3.2 1995/11/17 18:10:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmsrp.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmsrp.c,v $
 * Revision 3.2  1995/11/17  18:10:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmsrp.c xm7_v3.1.1:cset.0166:13:13 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Status Reporting Protocol Module

		File:			xm7_fbmsrp.c
		Created:		09/05/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for processing the Status Reporting
		protocol specified by the SMT standard. Every FBM_CLOCK_PULSE
		seconds, ProcessSRPTick() is executed. Based on the current
		station status, the appropriate SRF is generated if necessary.

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
	Status Reporting Protocol Support Routines
*********************************************************************/

static           uInt16
#ifdef __STDC__
BuildSRFInfo (uInt16 stnID, FBMSRPInfoType * srpPtr, Void_p buffer)
#else
BuildSRFInfo (stnID, srpPtr, buffer)
		uInt16           stnID;
		FBMSRPInfoType  *srpPtr;
		Void_p           buffer;
#endif
/*********************************************************************
Function:	This routine builds the SRF Info field for the frame.
Parameters:	stnID	= index of station to use.
			srpPtr	= pointer to station's list of active notifications.
			buffer	= pointer to start of SMT Info field in frame buffer.
Input:		Uses station's notification list.
Output:		buffer	= Info field for SRF.
Return:		Length of SMT Info field.
Modification History:
*********************************************************************/
{
uInt16           infoFieldLen;			/* length of SMT info */
uInt16           frameLeft;				/* space left in frame */
uInt16           paramLen;				/* length of each parameter */
TLVParamType    *mibPtr;				/* MIB parameter */
ECEntryType     *ecPtr;					/* notification buffer */
uInt32           result;				/* return value from MIB Get */
XMMultipleType  *multiple;				/* format for multipleoccurrence flag */

	/*
	 * Initialize values.
	 */
	infoFieldLen = 0;
	frameLeft = MAX_SMT_XMIT_LEN;

	/* skip frame header and build only info field */
	buffer = MADDPTR (Void_t, buffer, SIZE_SMTFrameHdrType);
	frameLeft -= SIZE_SMTFrameHdrType;

	/*
	 * Get TimeStamp.
	 */
	mibPtr = (TLVParamType *) buffer;
	mibPtr->paramType = fddiSMTTimeStamp;
	mibPtr->paramLen = 4;
	FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType), (Void_p) mibPtr);
	paramLen = Attribute_Param ((TLVParamType *) buffer, mibPtr);
	buffer = MADDPTR (Void_t, buffer, paramLen);
	infoFieldLen += paramLen;
	frameLeft -= paramLen;

	/*
	 * Get TransitionTimeStamp.
	 */
	mibPtr = (TLVParamType *) buffer;
	mibPtr->paramType = fddiSMTTransitionTimeStamp;
	mibPtr->paramLen = 4;
	FBMGetMIBAttrMulti (stnID, sizeof (TLVParamType), (Void_p) mibPtr);
	paramLen = Attribute_Param ((TLVParamType *) buffer, mibPtr);
	buffer = MADDPTR (Void_t, buffer, paramLen);
	infoFieldLen += paramLen;
	frameLeft -= paramLen;

	/*
	 * Loop through notification list.
	 */
	ecPtr = srpPtr->srActive;
	while (ecPtr)
	{
		/* prepare to Get local notification report */
		mibPtr = (TLVParamType *) buffer;
		mibPtr->paramType = 0;
		mibPtr->paramLen = 8;			/* most requests have length of 8 */

		/* select ID based on notification ID */
		switch (ecPtr->ecID)
		{
		case fddiSMTPeerWrapCondition:
			mibPtr->paramType = fddiSMTVendorAttrib;
			mibPtr->paramLen = 4;		/* only request with a diff length */
			mibPtr->SMTVENDORID = xdiSMTPeerWrapReport;
			break;

		case fddiMACDuplicateAddressCondition:
			mibPtr->paramType = fddiMACVendorAttrib;
			mibPtr->MACINDEX = ecPtr->entity;
			mibPtr->MACVENDORID = xdiMACDuplicateAddressReport;
			break;

		case fddiMACFrameErrorCondition:
			mibPtr->paramType = fddiMACVendorAttrib;
			mibPtr->MACINDEX = ecPtr->entity;
			mibPtr->MACVENDORID = xdiMACFrameErrorReport;
			break;

		case fddiMACNotCopiedCondition:
			mibPtr->paramType = fddiMACVendorAttrib;
			mibPtr->MACINDEX = ecPtr->entity;
			mibPtr->MACVENDORID = xdiMACNotCopiedReport;
			break;

		case fddiMACNeighborChangeEvent:
			mibPtr->paramType = fddiMACVendorAttrib;
			mibPtr->MACINDEX = ecPtr->entity;
			mibPtr->MACVENDORID = xdiMACNeighborChangeReport;
			break;

		case fddiMACPathChangeEvent:
			mibPtr->paramType = fddiMACVendorAttrib;
			mibPtr->MACINDEX = ecPtr->entity;
			mibPtr->MACVENDORID = xdiMACPathChangeReport;
			break;

		case fddiMACVendorNotification:
			if (ecPtr->vendorID == xdiMACConfigurationFailedEvent)
			{
				mibPtr->paramType = fddiMACVendorAttrib;
				mibPtr->MACINDEX = ecPtr->entity;
				mibPtr->MACVENDORID = xdiMACConfigurationFailedReport;
			}

			else
			{
				/*
				 * Unknown notification type. Call local notification
				 * processing.
				 */
				LocalFBMNotificationMulti (stnID, mibPtr, ecPtr);
			}
			break;

		case fddiPORTLerCondition:
			mibPtr->paramType = fddiPORTVendorAttrib;
			mibPtr->PORTINDEX = ecPtr->entity;
			mibPtr->PORTVENDORID = xdiPORTLerReport;
			break;

		case fddiPORTUndesiredConnectionAttemptEvent:
			mibPtr->paramType = fddiPORTVendorAttrib;
			mibPtr->PORTINDEX = ecPtr->entity;
			mibPtr->PORTVENDORID = xdiPORTUndesiredConnectionReport;
			break;

		case fddiPORTEBErrorCondition:
			mibPtr->paramType = fddiPORTVendorAttrib;
			mibPtr->PORTINDEX = ecPtr->entity;
			mibPtr->PORTVENDORID = xdiPORTEBErrorReport;
			break;

		case fddiPORTPathChangeEvent:
			mibPtr->paramType = fddiPORTVendorAttrib;
			mibPtr->PORTINDEX = ecPtr->entity;
			mibPtr->PORTVENDORID = xdiPORTPathChangeReport;
			break;

		case fddiPORTVendorNotification:
			if (ecPtr->vendorID == xdiPORTLCTFailedEvent)
			{
				mibPtr->paramType = fddiPORTVendorAttrib;
				mibPtr->PORTINDEX = ecPtr->entity;
				mibPtr->PORTVENDORID = xdiPORTLCTFailedReport;
			}

			else
			{
				/*
				 * Unknown notification type. Call local notification
				 * processing.
				 */
				LocalFBMNotificationMulti (stnID, mibPtr, ecPtr);
			}
			break;

		default:
			/*
			 * Unknown notification type. Clear paramType to skip further
			 * processing.
			 */
			mibPtr->paramType = 0;
			break;
		}

		/*
		 * Get notification report from MIB.
		 */
		if (mibPtr->paramType)
		{
			result = FBMGetMIBAttrMulti (stnID, frameLeft, (Void_p) mibPtr);

			/* if event/condition retrieved OK, */
			if (result == RC_Success)
			{
				/* if enough space left */
				if (mibPtr->paramLen <= frameLeft)
				{
					/*
					 * Copy notification report into correct location in SRF.
					 * The report attributes place a complete notification
					 * report in the vendor specific data area. This is a
					 * full TLV encoding for SMT-defined and vendor-defined
					 * notifications. The copy overwrites the attribute type
					 * and length of the MIB Get request to leave only the
					 * notification TLV encoding in the frame.
					 */
					if (mibPtr->paramType == fddiSMTVendorAttrib)
						MEMCOPY (mibPtr, mibPtr->SMTVENDORDATA,
							mibPtr->paramLen);
					else
						MEMCOPY (mibPtr, mibPtr->MACVENDORDATA,
							mibPtr->paramLen);

					/* if event and multiple occurrences */
					if ((ecPtr->ecType == EC_TYPE_EVENT)
						&& (ecPtr->ecFlags & SR_MultipleOccurrence))
					{
						/*
						 * Set multiple occurrence in attribute. This code
						 * assumes that the format of an event attribute
						 * places the multiple occurrence flag at the
						 * BEGINNING of the event format.
						 */
						switch (mibPtr->paramType)
						{
						case fddiSMTVendorNotification:
							multiple = (XMMultipleType *) mibPtr->SMTVENDORDATA;
							break;

						case fddiMACVendorNotification:
						case fddiPATHVendorNotification:
						case fddiPORTVendorNotification:
							multiple = (XMMultipleType *) mibPtr->MACVENDORDATA;
							break;

						default:
							if ((mibPtr->paramType & MIB_OBJECT_MASK)
								== fddiSMT)
								multiple =
									(XMMultipleType *) & mibPtr->SMTGENERAL;
							else
								multiple =
									(XMMultipleType *) & mibPtr->MACGENERAL;
							break;
						}

						/* set multiple occurrence flag */
						multiple->multipleoccurrence = TRUE;
					}

					/* put into frame */
					paramLen = Attribute_Param ((TLVParamType *) buffer,
						mibPtr);
					buffer = MADDPTR (Void_t, buffer, paramLen);
					infoFieldLen += paramLen;
					frameLeft -= paramLen;
				}

				else
				{
					/* otherwise stop getting data */
					frameLeft = 0;
					break;
				}
			}
		}

		/* move to next notification */
		ecPtr = ecPtr->next;
	}

	return (infoFieldLen);
}

static ECEntryType *
#ifdef __STDC__
FindECElement (FBMSRPInfoType * srpPtr, uInt16 ecID, uInt16 entity,
	uInt32 vendorID)
#else
FindECElement (srpPtr, ecID, entity, vendorID)
		FBMSRPInfoType  *srpPtr;
		uInt16           ecID;
		uInt16           entity;
		uInt32           vendorID;
#endif
/*********************************************************************
Function:	This routine locates an existing the list element for the
			associated notification.
Parameters:	srpPtr		= status report protocol information.
			ecID		= local ID for notification.
			entity		= object's index (0 for SMT object).
			vendorID	= vendor ID for vendor-specific notifications.
Input:		Uses fbmSRPData.
Output:		None.
Return:		Pointer to list element for notification specified.
			NULL returned if not currently in list.
Modification History:
*********************************************************************/
{
ECEntryType     *resultPtr;				/* return value */

	resultPtr = srpPtr->srActive;
	while (resultPtr)
	{
		if ((resultPtr->ecID == ecID)
			&& (resultPtr->entity == entity)
			&& (resultPtr->vendorID == vendorID))
			break;
		resultPtr = resultPtr->next;
	}

	return (resultPtr);
}


/*********************************************************************
	Status Reporting Protocol Routines
*********************************************************************/

static void
#ifdef __STDC__
Include_Assertion (ECEntryType * ecPtr)
#else
Include_Assertion (ecPtr)
		ECEntryType     *ecPtr;
#endif
/*********************************************************************
Function:	This routine performs the Include_Assertion actions of
			the status report protocol.
Parameters:	ecPtr	= pointer to condition element.
Input:		Uses fbmSRPData.
Output:		Updates condition entry in fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
	ecPtr->ecFlags |= SR_ReportRequired;
	ecPtr->ecFlags |= SR_ConditionState;

	return;
}

static void
#ifdef __STDC__
Include_Deassertion (ECEntryType * ecPtr)
#else
Include_Deassertion (ecPtr)
		ECEntryType     *ecPtr;
#endif
/*********************************************************************
Function:	This routine performs the Include_Deassertion actions of
			the status report protocol.
Parameters:	ecPtr	= pointer to condition element.
Input:		Uses fbmSRPData.
Output:		Updates condition entry in fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
	ecPtr->ecFlags &= ~SR_ConditionState;

	return;
}

static void
#ifdef __STDC__
Include_Event (ECEntryType * ecPtr)
#else
Include_Event (ecPtr)
		ECEntryType     *ecPtr;
#endif
/*********************************************************************
Function:	This routine performs the Include_Event actions of
			the status report protocol.
Parameters:	ecPtr	= pointer to event element.
Input:		Uses fbmSRPData.
Output:		Updates event entry in fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
	if (ecPtr->ecFlags & SR_ReportRequired)
		ecPtr->ecFlags |= SR_MultipleOccurrence;

	else
		ecPtr->ecFlags |= SR_ReportRequired;

	return;
}

static void
#ifdef __STDC__
Clear_All_ReportRequired (FBMSRPInfoType * srpPtr)
#else
Clear_All_ReportRequired (srpPtr)
		FBMSRPInfoType  *srpPtr;
#endif
/*********************************************************************
Function:	This routine performs the Clear_All_ReportRequired actions of
			the status report protocol.
Parameters:	srpPtr	= Status report protocol info.
Input:		Uses fbmSRPData.
Output:		Clears ReportRequired flag in all entries in fbmSRPData.
Return:		None.
Notes:		The Clear_Event_ReportRequired actions are explicitly included
			in this routine for efficiency.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
ECEntryType     *currPtr;				/* current element in list */
ECEntryType     *nextPtr;				/* next element in list */

	/*
	 * Loop through array and put all elements on the free list.
	 */
	for (i = 0,
		currPtr = &srpPtr->srList[0],
		nextPtr = &srpPtr->srList[1];
		i < EC_LIST_SIZE;
		i++,
		currPtr++,
		nextPtr++)
	{
		currPtr->ecID = 0;
		currPtr->vendorID = 0;
		currPtr->entity = 0;
		currPtr->ecType = 0;
		currPtr->ecFlags = 0;
		currPtr->next = nextPtr;
	}

	srpPtr->srList[EC_LIST_SIZE - 1].next = (ECEntryType *) NULL;
	srpPtr->srFree = &srpPtr->srList[0];
	srpPtr->srActive = (ECEntryType *) NULL;

	return;
}

static void
#ifdef __STDC__
Clear_Event_ReportRequired (FBMSRPInfoType * srpPtr)
#else
Clear_Event_ReportRequired (srpPtr)
		FBMSRPInfoType  *srpPtr;
#endif
/*********************************************************************
Function:	This routine performs the Clear_Event_ReportRequired
			actions of the status report protocol.
Parameters:	srpPtr	= Status report protocol info.
Input:		Uses fbmSRPData.
Output:		Clears ReportRequired flag in all events in fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
ECEntryType     *currPtr;				/* current element in list */
ECEntryType     *lastPtr;				/* previous element in list */
ECEntryType     *nextPtr;				/* next element in list */

	/*
	 * Loop through active list and put event elements on the free list.
	 */
	lastPtr = (ECEntryType *) NULL;
	currPtr = srpPtr->srActive;
	while (currPtr)
	{
		/* save next element to check */
		nextPtr = currPtr->next;

		/* if this is an event */
		if (currPtr->ecType == EC_TYPE_EVENT)
		{
			/* clear info */
			currPtr->ecID = 0;
			currPtr->vendorID = 0;
			currPtr->entity = 0;
			currPtr->ecType = 0;
			currPtr->ecFlags = 0;

			/* remove from active list */
			if (lastPtr)
				/* update previous entry's next pointer */
				lastPtr->next = nextPtr;

			/* if current entry is start of active list */
			if (currPtr == srpPtr->srActive)
				/* move active start to next entry */
				srpPtr->srActive = nextPtr;

			/* put on free list */
			currPtr->next = srpPtr->srFree;
			srpPtr->srFree = currPtr;
		}

		else
		{
			/* move previous pointer to this element */
			lastPtr = currPtr;
		}

		/* get next element */
		currPtr = nextPtr;
	}

	return;
}

static void
#ifdef __STDC__
Clear_Deasserted_Condition_ReportRequired (FBMSRPInfoType * srpPtr)
#else
Clear_Deasserted_Condition_ReportRequired (srpPtr)
		FBMSRPInfoType  *srpPtr;
#endif
/*********************************************************************
Function:	This routine performs the Clear_Deasserted_Condition_ReportRequired
			actions of the status report protocol.
Parameters:	srpPtr	= Status report protocol info.
Input:		Uses fbmSRPData.
Output:		Clears ReportRequired flag in all deasserted conditions
			in fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
ECEntryType     *currPtr;				/* current element in list */
ECEntryType     *lastPtr;				/* previous element in list */
ECEntryType     *nextPtr;				/* next element in list */

	/*
	 * Loop through active list and put event elements on the free list.
	 */
	lastPtr = (ECEntryType *) NULL;
	currPtr = srpPtr->srActive;
	while (currPtr)
	{
		/* save next element to check */
		nextPtr = currPtr->next;

		/* if this is an inactive condition */
		if ((currPtr->ecType == EC_TYPE_CONDITION)
			&& !(currPtr->ecFlags & SR_ConditionState))
		{
			/* clear info */
			currPtr->ecID = 0;
			currPtr->vendorID = 0;
			currPtr->entity = 0;
			currPtr->ecType = 0;
			currPtr->ecFlags = 0;

			/* remove from active list */
			if (lastPtr)
				/* update previous entry's next pointer */
				lastPtr->next = nextPtr;

			/* if current entry is start of active list */
			if (currPtr == srpPtr->srActive)
				/* move active start to next entry */
				srpPtr->srActive = nextPtr;

			/* put on free list */
			currPtr->next = srpPtr->srFree;
			srpPtr->srFree = currPtr;
		}

		else
		{
			/* move previous pointer to this element */
			lastPtr = currPtr;
		}

		/* get next element */
		currPtr = nextPtr;
	}

	return;
}

static void
#ifdef __STDC__
Send_Frame (uInt16 stnID, FBMSRPInfoType * srpPtr)
#else
Send_Frame (stnID, srpPtr)
		uInt16           stnID;
		FBMSRPInfoType  *srpPtr;
#endif
/*********************************************************************
Function:	This routine performs the Send_Frame actions of
			the status report protocol.
Parameters:	stnID	= station index to use.
			srpPtr	= pointer to station's active notification list.
Input:		Uses fbmSRPData and fbmFrameBuffer.
Output:		Updates event entry in fbmSRPData.
Return:		Length of SMT InfoField of SRF.
Modification History:
*********************************************************************/
{
uInt16           i;						/* loop counter */
uInt16           infoLen;				/* length of SRF info field */
uInt32           transID;				/* transaction ID for SRF */

	/* clear RT_Flag */
	srpPtr->RT_Flag = CLEAR;

	/* Clear frame buffer */
	MClearFrameBuffer (fbmFrameBuffer, MAX_FRAME_SIZE);

	/* build SRF */
	infoLen = BuildSRFInfo (stnID, srpPtr, fbmFrameBuffer);

	/* set transID at 0 to get ID automatically */
	transID = 0;
	for (i = 0; i < MAX_MAC_COUNT; i++)
	{
		/* if MAC is available for xmit */
		if (FBMDATA (stnID, fbmMACData[i].SM_MA_Avail))
		{
			/* set RT_Flag */
			srpPtr->RT_Flag = SET;

			/* set frame header for this MAC */
			transID = BuildSMTHeaderMulti (stnID,
				fbmFrameHeader, SMT_INFO_FC, SMT_SRF_DA,
				i, SRF_CLASS, SMTANNOUNCE, transID);

			/* set frame length */
			fbmFrameHeader->smtHdr.InfoField_Length
				= Mhtons (infoLen);

			/* send frame */
			SendSMTFrameMulti (stnID, fbmFrameBuffer,
				infoLen + SMT_FRAME_HDR_SIZE, i);
		}
	}

	/* if RT_Flag has been set */
	if (srpPtr->RT_Flag)
	{
		Clear_Event_ReportRequired (srpPtr);
		Clear_Deasserted_Condition_ReportRequired (srpPtr);
	}

	return;
}

void
#ifdef __STDC__
ProcessSRPReport (SMTMessage * smtmsg)
#else
ProcessSRPReport (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	This routine is called to process an event or condition
			report from MSP.
Parameters:	smtmsg	= message containing report.
Input:		Uses fbmStationData and fbmSRPData.
Output:		Updates fbmStationData and fbmSRPData.
Return:		None.
Modification History:
*********************************************************************/
{
MIBNotificationType *notice;			/* message notification format */
FBMSRPInfoType  *srpPtr;				/* stations SRP data */
ECEntryType     *ecPtr;					/* notification entry */

	/*
	 * Get SRP info.
	 */
	srpPtr = &(FBMDATA (smtmsg->stnID, fbmSRPData));
	if (srpPtr->srState != SR_DISABLED)
	{
		/*
		 * Get message data.
		 */
		notice = smtmsg->p1.mibNoticeBuf;

		/*
		 * Locate notification info.
		 */
		ecPtr = FindECElement (srpPtr, smtmsg->typeInfo, smtmsg->entity,
			smtmsg->vendorID);

		/* if not currently in list */
		if (!ecPtr)
		{
			/* add it to the list */
			if (srpPtr->srFree)
			{
				/* take entry from free list */
				ecPtr = srpPtr->srFree;
				srpPtr->srFree = ecPtr->next;

				/* put it on active list */
				ecPtr->next = srpPtr->srActive;
				srpPtr->srActive = ecPtr;

				/* init info */
				ecPtr->ecID = smtmsg->typeInfo;
				ecPtr->vendorID = smtmsg->vendorID;
				ecPtr->entity = smtmsg->entity;
				ecPtr->ecType = notice->type;
				ecPtr->ecFlags = 0;
			}
		}

		/* if an element was found or added */
		if (ecPtr)
		{
			/* if this is a condition */
			if (ecPtr->ecType == EC_TYPE_CONDITION)
			{
				/* process transitions based on conditions state */
				if (notice->state)
				{
					/* common actions for SR(01a), SR(00b) and SR(11a) */
					srpPtr->SRThreshold = 2;
					Include_Assertion (ecPtr);
				}

				else
				{
					/* common actions for SR(01b), SR(00c) and SR(11b) */
					Include_Deassertion (ecPtr);
				}
			}

			else
			{
				/* common actions for SR(01c), SR(00d) and SR(11c) */
				Include_Event (ecPtr);
			}

			/* if in WAIT, */
			if (srpPtr->srState == SR_WAIT)
			{
				if (srpPtr->TSR < T_Limit)
					/* move to HOLDOFF */
					srpPtr->srState = SR_HOLDOFF;
				else
				{
					/**
					 * WRF - avoid sending frames
					 * from interrupt level. Don't call
					 * Send_Frame() but just set the timer
					 * so that the frame is sure to be
					 * sent.
					 **/
					/* Reset TSR */
					srpPtr->TSR = srpPtr->SRThreshold;
				}
			}
		}
	}

	return;
}


void
#ifdef __STDC__
ProcessSRPTick (uInt16 stnID)
#else
ProcessSRPTick (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This routine is called to process the periodic
			functions of the Status Reporting protocol.
Parameters:	stnID		= index of station to use.
Input:		Uses fbmStationData, ecList, and conditionList.
Output:		Updates fbmStationData, ecList, and conditionList.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       smtmsg;				/* message for MIB */
FBMSRPInfoType  *srpPtr;				/* this station's SRP state */

	/*
	 * Get station's SRP data.
	 */
	srpPtr = &FBMDATA (stnID, fbmSRPData);

	/*
	 * Send message to MIB to check for conditions.
	 */
	smtmsg.destination = MIB_MSG_ID;
	smtmsg.source = FBM_MSG_ID;
	smtmsg.type = MIB_ACTION_DETECT_CONDITIONS;
	smtmsg.typeInfo = 0;
	smtmsg.localID = 0;
	smtmsg.len1 = 0;
	smtmsg.len2 = 0;
	smtmsg.entity = 0;
	smtmsg.stnID = stnID;
	SendFBMMessage (&smtmsg);

	/*
	 * SR(00e) If timer has expired
	 */
	srpPtr->TSR += FBM_CLOCK_TICK;
	if ((srpPtr->srState == SR_WAIT) && (srpPtr->TSR >= srpPtr->SRThreshold))
	{
		/* if there is something to report */
		if (srpPtr->srActive)
		{
			/* do Send_Frame */
			Send_Frame (stnID, srpPtr);

			/* update time to next SRF */
			srpPtr->SRThreshold <<= 1;
			if (srpPtr->SRThreshold > Max_SRThreshold)
				srpPtr->SRThreshold = Max_SRThreshold;

			/* Reset TSR */
			srpPtr->TSR = 0;
		}
	}

	/*
	 * SR(10b)
	 */
	else if ((srpPtr->srState == SR_HOLDOFF) && (srpPtr->TSR >= T_Limit))
	{
		/* do Send_Frame */
		Send_Frame (stnID, srpPtr);
		srpPtr->srState = SR_WAIT;

		/* reset TSR */
		srpPtr->TSR = 0;
	}

	return;
}

uInt32
#ifdef __STDC__
InitSRP (uInt16 stnID)
#else
InitSRP (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize the Status Reporting Protocol. This routine
			performs the actions done on a Station Reset OR on an
			SR_Enable signal.
Parameters:	stnID		= index of station to use.
Input:		None.
Output:		Initializes the notification list to be empty.
Return:		0 if successful, otherwise an error code is returned.
Modification History:
*********************************************************************/
{
FBMSRPInfoType  *srpPtr;				/* station's SRP info */
TLVParamType     buffer;				/* used to get SRP state */

	srpPtr = &(FBMDATA (stnID, fbmSRPData));

	srpPtr->SRThreshold = 2;
	Clear_All_ReportRequired (srpPtr);

	buffer.paramType = fddiSMTStatRptPolicy;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);

	/* check state of SRP */
	srpPtr->srState = (buffer.SMTPARAM8) ? SR_WAIT : SR_DISABLED;

	/* reset TSR */
	srpPtr->TSR = 0;

	return (0);
}
