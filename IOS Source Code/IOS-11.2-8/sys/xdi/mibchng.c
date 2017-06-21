/* $Id: mibchng.c,v 3.2 1995/11/17 18:10:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibchng.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibchng.c,v $
 * Revision 3.2  1995/11/17  18:10:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibchng.c xm7_v3.1.1:cset.0172:12:12 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB maintenance module.

		File:			xm7_mibchng.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0172		01/13/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module supplies the Set operation for each MIB object
		group: SMT, MAC, PATH, and PORT. For any writable
		attribute, the new value must take effect immediately. This
		requires a relatively close coupling between the change routines
		provided here and underlying interface routines to perform the
		change in the station. These routines DO NOT handle vendor-
		specific attributes.

		For attributes related to SMT operation (e.g. policies, etc.),
		the XLNT Manager handles the change appropriately. For attributes
		affecting station operation (like, T_Req), the change request
		is passed to the interface routine. If the interface routine
		is successful at implementing the change, then the calling
		routine changes the value in the MIB.
		
		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
	MIB Functions.
*********************************************************************/

uInt32
#ifdef __STDC__
ChangeMIBSMTAttr (uInt16 stnID, Void_p bufData)
#else
ChangeMIBSMTAttr (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change an fddiSMT attribute.
Parameter:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information and used to
							hold attribute value returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get Request
							(see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
Notes:		All modifiable attributes call the interface routine
			SetSMTDataMulti(). This allows the implementer to store
			the new values into non-volatile memory.
Modification History:
*********************************************************************/
{
TLVParamType    *p;						/* attribute pointer */
SMTMessage       notifyMsg;				/* message to notify other tasks */
uInt32           result = RC_Success;	/* return code */
SMTBaseAttrType *smtPtr;				/* SMT object info */

	/*
	 * Set MIB SMT to use.
	 */
	smtPtr = &MIBDATA (stnID, mib->smtData);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/*
	 * Set notification message in case other tasks need to be notified of
	 * the change.
	 */
	notifyMsg.source = MIB_MSG_ID;
	notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
	notifyMsg.typeInfo = p->paramType;
	notifyMsg.entity = 0;
	notifyMsg.localID = 0;
	notifyMsg.len1 = 0;
	notifyMsg.len2 = 0;
	notifyMsg.stnID = stnID;

	/*
	 * Change value in MIB.
	 */
	switch (p->paramType)
	{
		/*****
		*	Filter GET-ONLY attributes
		*****/
	case fddiSMTStationId:
	case fddiSMTOpVersionId:
	case fddiSMTHiVersionId:
	case fddiSMTLoVersionId:
	case fddiSMTMIBVersionId:
	case fddiSMTMac_Ct:
	case fddiSMTNonMaster_Ct:
	case fddiSMTMaster_Ct:
	case fddiSMTAvailablePaths:
	case fddiSMTConfigCapabilities:
	case fddiSMTPORTIndexes:
	case fddiSMTMACIndexes:
	case fddiSMTBypassPresent:
	case fddiSMTECMState:
	case fddiSMTCF_State:
	case fddiSMTRemoteDisconnectFlag:
	case fddiSMTStationStatus:
	case fddiSMTPeerWrapFlag:
	case fddiSMTTimeStamp:
	case fddiSMTTransitionTimeStamp:
	case fddiSMTManufacturerData:
	case fddiSMTSetCount:
	case fddiSMTLastSetStationId:
	case fddiSMTStationIdGrp:
	case fddiSMTStationConfigGrp:
	case fddiSMTStatusGrp:
	case fddiSMTMIBOperationGrp:
	case fddiSMTPeerWrapCondition:
	case fddiSMTVendorNotification:
		result = RC_IllegalOperation;
		break;

		/*****
		 *	GET-REPLACE attributes
		 *****/
	case fddiSMTUserData:
		if (p->paramLen < SIZE_SMTUserDataType)
			result = RC_LengthError;
		else
		{
			/* force length in case request has extra data */
			p->paramLen = SIZE_SMTUserDataType;

			/* copy data to MIB */
			MEMCOPY (smtPtr->UserData, p->SMTOTHER, SIZE_SMTUserDataType);

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);
		}
		break;

	case fddiSMTConfigPolicy:
		/* force length in case request has extra data */
		p->paramLen = SIZE_DEFAULT;

		/* set attribute */
		smtPtr->ConfigPolicy = p->SMTPARAM16;

		/* call routine to store value in NV memory */
		SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

		/*
		 * Send notification. Policies are NOT bounded by the object's
		 * capabilities. However, the value reported to CSP is bounded by the
		 * capability.
		 */
		notifyMsg.destination = CSP_MSG_ID;
		notifyMsg.data.b16 = smtPtr->ConfigPolicy & smtPtr->ConfigCapabilities;
		SendMIBMessage (&notifyMsg);
		break;

	case fddiSMTConnectionPolicy:
		/* Connection policy must always reject M-M. */
		if (p->SMTPARAM16 & Con_rejectM_M)
		{
			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			/* set attribute */
			smtPtr->ConnectionPolicy = p->SMTPARAM16;

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* send notification message */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.data.b16 = smtPtr->ConnectionPolicy;
			SendMIBMessage (&notifyMsg);
		}
		else
			result = RC_OutOfRange;
		break;

	case fddiSMTT_Notify:
		if ((p->SMTPARAM16 < T_Notify_Min) || (p->SMTPARAM16 > T_Notify_Max))
			result = RC_OutOfRange;
		else
		{
			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			/* set attribute */
			smtPtr->T_Notify = p->SMTPARAM16;

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* send notification message */
			notifyMsg.destination = FBM_MSG_ID;
			notifyMsg.data.b16 = smtPtr->T_Notify;
			SendMIBMessage (&notifyMsg);
		}
		break;

	case fddiSMTStatRptPolicy:
		/* if status is changing */
		if (p->SMTPARAM8 != smtPtr->StatRptPolicy)
		{
			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			/* set attribute */
			smtPtr->StatRptPolicy = p->SMTPARAM8;

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* send notification message */
			notifyMsg.destination = FBM_MSG_ID;
			notifyMsg.data.b8 = smtPtr->StatRptPolicy;
			SendMIBMessage (&notifyMsg);
		}
		break;

	case fddiSMTTrace_MaxExpiration:
		if (p->SMTPARAM32 < MUsecs2Fddi (Min_Trace_Max))
			result = RC_OutOfRange;
		else
		{
			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			/* set attribute */
			smtPtr->Trace_MaxExpiration = p->SMTPARAM32;

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* send notification message */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.data.b32 = smtPtr->Trace_MaxExpiration;
			SendMIBMessage (&notifyMsg);
		}
		break;

	case fddiSMTVendorAttrib:
		/* if XDI attribute */
		if ((p->SMTVENDORID & VENDOR_MASK) == fddiXDI)
			/* use core routine for change */
			result = ChangeXDISMTAttr (stnID, p, smtPtr);

		else
			/* use I/F routine */
			result = ChangeVendorSMTAttrMulti (stnID, p, smtPtr);

		break;

		/*
		 * fddiSMTStationAction
		 */
	case fddiSMTStationAction:
		switch (p->SMTPARAM16)
		{
		case SMTAction_connect:
			/*
			 * Clear the fddiSMTRemoteDisconnectFlag value when a connect
			 * action is performed. Then continue with action processing.
			 */
			smtPtr->RemoteDisconnectFlag = CLEAR;

		case SMTAction_disconnect:
		case SMTAction_path_Test:
		case SMTAction_disable_a:
		case SMTAction_disable_b:
		case SMTAction_disable_m:
			/* Pass message to CSP */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.source = MIB_MSG_ID;
			notifyMsg.type = CSP_ACTION_FDDI_SMT;
			notifyMsg.typeInfo = p->SMTPARAM16;
			notifyMsg.localID = 0;
			notifyMsg.len1 = 0;
			notifyMsg.len2 = 0;
			SendMIBMessage (&notifyMsg);

			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			break;

		case SMTAction_self_Test:
			/* call routine to handle self test */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* force length in case request has extra data */
			p->paramLen = SIZE_DEFAULT;

			break;

		default:
			result = RC_OutOfRange;
			break;
		}
		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeMIBMACAttr (uInt16 stnID, Void_p bufData)
#else
ChangeMIBMACAttr (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change an fddiMAC attribute.
Parameters:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information and used to hold
							attribute value returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute or object ID not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
*********************************************************************/
{
TLVParamType    *p;						/* parameter structure */
SMTMessage       notifyMsg;				/* message to other processes */
uInt32           result = RC_Success;	/* return value */
MIBType         *mibPtr;				/* MIB database */
Int16            index;					/* array index */
MACBaseAttrType *macBase;				/* MAC attribute data */
MACDataPtr       macPtr;				/* MIB MAC element */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/* check if resource ID is in the table */
	index = MFindResourceID (p->MACINDEX, mibPtr->macTable,
		mibPtr->smtData.Mac_Ct);
	if (index < 0)
		/* index not found */
		result = RC_NoParameter;

	else
	{
		/* map index to MAC attributes */
		index = mibPtr->macTable[index].index;
		macPtr = &mibPtr->macBase[index];
		macBase = &macPtr->baseData;

		/*
		 * Set up notification message.
		 */
		notifyMsg.source = MIB_MSG_ID;
		notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
		notifyMsg.typeInfo = p->paramType;
		notifyMsg.entity = index;
		notifyMsg.localID = 0;
		notifyMsg.len1 = 0;
		notifyMsg.len2 = 0;
		notifyMsg.stnID = stnID;

		/*
		 * Change value in MIB.
		 */
		switch (p->paramType)
		{
			/*****
			*	Filter GET-ONLY attributes
			*****/
		case fddiMACHardwarePresent:
		case fddiMACIndex:
		case fddiMACFrameStatusFunctions:
		case fddiMACT_MaxCapability:
		case fddiMACTVXCapability:
		case fddiMACAvailablePaths:
		case fddiMACCurrentPath:
		case fddiMACUpstreamNbr:
		case fddiMACDownstreamNbr:
		case fddiMACOldUpstreamNbr:
		case fddiMACOldDownstreamNbr:
		case fddiMACDupAddressTest:
		case fddiMACDownstreamPORTType:
		case fddiMACSMTAddress:
		case fddiMACT_Req:
		case fddiMACT_Neg:
		case fddiMACT_Max:
		case fddiMACTvxValue:
		case fddiMACFrame_Ct:
		case fddiMACCopied_Ct:
		case fddiMACTransmit_Ct:
		case fddiMACError_Ct:
		case fddiMACLost_Ct:
		case fddiMACFrameErrorRatio:
		case fddiMACRMTState:
		case fddiMACDA_Flag:
		case fddiMACUNDA_Flag:
		case fddiMACFrameErrorFlag:
		case fddiMACMA_UnitdataAvailable:
		case fddiMACBridgeFunctions:
		case fddiMACLongGrpAddress:
		case fddiMACShortGrpAddress:
		case fddiMACT_Pri0:
		case fddiMACT_Pri1:
		case fddiMACT_Pri2:
		case fddiMACT_Pri3:
		case fddiMACT_Pri4:
		case fddiMACT_Pri5:
		case fddiMACT_Pri6:
		case fddiMACToken_Ct:
		case fddiMACTvxExpired_Ct:
		case fddiMACNotCopied_Ct:
		case fddiMACLate_Ct:
		case fddiMACRingOp_Ct:
		case fddiMACNotCopiedRatio:
		case fddiMACNotCopiedFlag:
		case fddiMACConfigGrp:
		case fddiMACFrameErrorConditionGrp:
		case fddiMACStatusGrp:
		case fddiMACNotCopiedConditionGrp:
		case fddiMACCapabilitiesGrp:
		case fddiMACAddressGrp:
		case fddiMACOperationGrp:
		case fddiMACCountersGrp:
		case fddiMACDuplicateAddressCondition:
		case fddiMACFrameErrorCondition:
		case fddiMACNeighborChangeEvent:
		case fddiMACPathChangeEvent:
		case fddiMACNotCopiedCondition:
		case fddiMACVendorNotification:
			result = RC_IllegalOperation;
			break;

			/*****
			*	GET-REPLACE attributes
			*****/
		case fddiMACRequestedPaths:
			/* force length in case request has extra data */
			p->paramLen = (2 * SIZE_DEFAULT);

			/* set attribute */
			macBase->RequestedPaths.reqPaths = p->MACPARAM8;

			/* call routine to store value in NV memory */
			SetMACDataMulti (stnID, p->paramType, macPtr, p);

			/*
			 * Send notification message. Use member b32 to get start of
			 * first word in data area.
			 */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.data.b8 = macBase->RequestedPaths.reqPaths;
			SendMIBMessage (&notifyMsg);
			break;

		case fddiMACFrameErrorThreshold:
			/* force length in case request has extra data */
			p->paramLen = (2 * SIZE_DEFAULT);

			macBase->FrameErrorThreshold = p->MACPARAM16;

			/* call routine to store value in NV memory */
			SetMACDataMulti (stnID, p->paramType, macPtr, p);

			break;

		case fddiMACMA_UnitdataEnable:
			/* force length in case request has extra data */
			p->paramLen = (2 * SIZE_DEFAULT);

			macBase->MA_UnitdataEnable = (p->MACPARAM8) ? TRUE : FALSE;

			/* call routine to store value in NV memory */
			SetMACDataMulti (stnID, p->paramType, macPtr, p);

			/* send notification message */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.data.b8 = macBase->MA_UnitdataEnable;
			SendMIBMessage (&notifyMsg);

			break;

		case fddiMACNotCopiedThreshold:
			/*
			 * This value is only supported if NotCopied_Ct is supported and
			 * manageabled.
			 */
			if ((macBase->xdi.packages & PKG_framenotcopiedmanagement)
				&& (macBase->xdi.packages & PKG_framenotcopied))
			{
				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);

				macBase->NotCopiedThreshold = p->MACPARAM16;

				/* call routine to store value in NV memory */
				SetMACDataMulti (stnID, p->paramType, macPtr, p);
			}

			else
				result = RC_NoParameter;
			break;

		case fddiMACVendorAttrib:
			/* if XDI attribute */
			if ((p->MACVENDORID & VENDOR_MASK) == fddiXDI)
				/* use core routine for change */
				result = ChangeXDIMACAttr (stnID, p, macPtr);

			else
				/* use I/F routine */
				result = ChangeVendorMACAttrMulti (stnID, p, macPtr);

			break;

		default:
			result = RC_NoParameter;
			break;
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeMIBPATHAttr (uInt16 stnID, Void_p bufData)
#else
ChangeMIBPATHAttr (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change an fddiPATH attribute.
Parameters:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information
							and used to hold attribute value
							returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT standard).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
Modification History:
*********************************************************************/
{
TLVParamType    *p;						/* attribute pointer */
SMTMessage       notifyMsg;				/* notification message */
uInt32           result = RC_Success;	/* return code */
MIBType         *mibPtr;				/* MIB database */
Int16            index;					/* PATH element index */
PATHDataPtr      pathPtr;				/* PATH data */
MACDataPtr       macPtr;				/* MAC element used for range check */
uInt16           i;						/* loop counter */
uChar            pathBits;				/* requested path bits */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/*
	 * Get index. PATH indexes directly access the MIB table.
	 */
	index = p->PATHINDEX - 1;

	/*
	 * If valid PATH index
	 */
	if (mibPtr->pathBase[index].inUse)
	{
		pathPtr = &mibPtr->pathBase[index];

		/*
		 * Set up notification message.
		 */
		notifyMsg.source = MIB_MSG_ID;
		notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
		notifyMsg.typeInfo = p->paramType;
		notifyMsg.entity = index;
		notifyMsg.localID = 0;
		notifyMsg.len1 = 0;
		notifyMsg.len2 = 0;
		notifyMsg.stnID = stnID;

		/*
		 * Change value in MIB.
		 */
		switch (p->paramType)
		{
			/*****
			*	Filter GET-ONLY attributes
			*****/
		case fddiPATHIndex:
		case fddiPATHConfiguration:
		case fddiPATHTraceStatus:
		case fddiPATHConfigGrp:
		case fddiPATHVendorNotification:
			result = RC_IllegalOperation;
			break;

			/*****
			*	GET-REPLACE attributes
			******/
		case fddiPATHRingLatency:
			if (pathPtr->baseData.xdi.packages & PKG_ringlatency)
			{
				result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);
				if (result == RC_Success)
				{
					pathPtr->baseData.RingLatency = p->PATHPARAM32;

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPATHTVXLowerBound:
			/**
			 * Validate range so that (in TIME VALUES):
			 *	0 < fddiPATHTVXLowerBound < fddiPATHMaxT-Req
			 * and
			 *	fddiPATHTVXLowerBound <= fddiMACTVXCapability
			 * of all MACs with this path requested. Although the standard says
			 * to check only MACs currently on the path, this superset avoids
			 * problems later on when a MAC with this path requested attempts
			 * to insert onto this path.
			 **/
			if ((p->PATHPARAM32 == 0)
				|| (p->PATHPARAM32 <= pathPtr->baseData.MaxT_Req))
			{
				result = RC_OutOfRange;
			}

			else
			{
				if (pathPtr->baseData.Index == PI_primary)
					pathBits = PP_primary_alt | PP_primary_pref;

				else if (pathPtr->baseData.Index == PI_secondary)
					pathBits = PP_secondary_alt | PP_secondary_pref;

				else
					pathBits = PP_local;

				for (i = 0, macPtr = mibPtr->macBase;
					(result == RC_Success) && (i < MAX_MAC_COUNT);
					i++, macPtr++)
				{
					/* if MAC has requested this path */
					if (macPtr->inUse
						&& macPtr->baseData.HardwarePresent
						&& (macPtr->baseData.RequestedPaths.reqPaths
							& pathBits))
					{
						if ((pathBits & PP_local)
							&& (macPtr->baseData.xdi.localPath
								== pathPtr->baseData.Index))
							/* not this local path */
							continue;

						if (p->PATHPARAM32 < macPtr->hwData.TVXCapability)
						{
							result = RC_OutOfRange;
						}
					}
				}

				/* attempt to instantiate value in hardware */
				if (result == RC_Success)
					result = SetPATHDataMulti (stnID, p->paramType,
						pathPtr, p);

				/* all set so put into MIB an notify CSP */
				if (result == RC_Success)
				{
					pathPtr->baseData.TVXLowerBound = p->PATHPARAM32;

					/* send notification message */
					notifyMsg.destination = CSP_MSG_ID;
					notifyMsg.data.b32 = pathPtr->baseData.TVXLowerBound;
					SendMIBMessage (&notifyMsg);

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			break;

		case fddiPATHT_MaxLowerBound:
			/**
			 * Validate range so that (in TIME VALUES):
			 *	fddiPATHMaxT-Req <= fddiPATHT-MaxLowerBound
			 * and
			 *	10ms <= fddiPATHT-MaxLowerBound
			 * and
			 *	fddiPATHT-MaxLowerBound < fddiMACT-MaxCapability
			 * of all MACs with this path requested. Although the standard says
			 * to check only MACs currently on the path, this superset avoids
			 * problems later on when a MAC with this path requested attempts
			 * to insert onto this path.
			 **/
			if ((pathPtr->baseData.MaxT_Req < p->PATHPARAM32)
				|| (FDDI_Min_T_MaxLowerBound < p->PATHPARAM32))
			{
				result = RC_OutOfRange;
			}

			else
			{
				if (pathPtr->baseData.Index == PI_primary)
					pathBits = PP_primary_alt | PP_primary_pref;

				else if (pathPtr->baseData.Index == PI_secondary)
					pathBits = PP_secondary_alt | PP_secondary_pref;

				else
					pathBits = PP_local;

				for (i = 0, macPtr = mibPtr->macBase;
					(result == RC_Success) && (i < MAX_MAC_COUNT);
					i++, macPtr++)
				{
					/* if MAC has requested this path */
					if (macPtr->inUse
						&& macPtr->baseData.HardwarePresent
						&& (macPtr->baseData.RequestedPaths.reqPaths
							& pathBits))
					{
						if ((pathBits & PP_local)
							&& (macPtr->baseData.xdi.localPath
								== pathPtr->baseData.Index))
							/* not this local path */
							continue;

						if (p->PATHPARAM32 <= macPtr->hwData.T_MaxCapability)
						{
							result = RC_OutOfRange;
						}
					}
				}

				/* attempt to instantiate value in hardware */
				if (result == RC_Success)
					result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);

				if (result == RC_Success)
				{
					pathPtr->baseData.T_MaxLowerBound = p->PATHPARAM32;

					/* send notification message */
					notifyMsg.destination = CSP_MSG_ID;
					notifyMsg.data.b32 = pathPtr->baseData.T_MaxLowerBound;
					SendMIBMessage (&notifyMsg);

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			break;

		case fddiPATHMaxT_Req:
			/**
			 * Validate range so that (in TIME VALUES):
			 *	fddiPATHTVXLowerBound < fddiPATHMaxT-Req
			 *		<= fddiPATHT_MaxLowerBound
			 **/
			if ((pathPtr->baseData.TVXLowerBound <= p->PATHPARAM32)
				|| (p->PATHPARAM32 < pathPtr->baseData.T_MaxLowerBound))
			{
				result = RC_OutOfRange;
			}

			else
			{
				/* attempt to instantiate value in hardware */
				result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);

				if (result == RC_Success)
				{
					pathPtr->baseData.MaxT_Req = p->PATHPARAM32;

					/* send notification message */
					notifyMsg.destination = CSP_MSG_ID;
					notifyMsg.data.b32 = pathPtr->baseData.MaxT_Req;
					SendMIBMessage (&notifyMsg);

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			break;

		case fddiPATHSbaPayload:
			if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthrequestor)
			{
				result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);
				if (result == RC_Success)
				{
					pathPtr->baseData.SbaPayload = p->PATHPARAM32;

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPATHSbaOverhead:
			if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthrequestor)
			{
				result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);
				if (result == RC_Success)
				{
					/* remove previous overhead allocated */
					mibPtr->smtData.xdi.syncBandwidth -=
						pathPtr->baseData.SbaOverhead;
					/* add new amount to total allocation */
					mibPtr->smtData.xdi.syncBandwidth += p->PATHPARAM32;

					/* set new overhead for path */
					pathPtr->baseData.SbaOverhead = p->PATHPARAM32;

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPATHT_Rmode:
			if (pathPtr->baseData.xdi.packages & PKG_restrictedtoken)
			{
				pathPtr->baseData.T_Rmode = p->PATHPARAM32;

				/* call routine to store value in NV memory */
				SetPATHDataMulti (stnID, p->paramType, pathPtr, p);

				/* send notification message */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.data.b32 = pathPtr->baseData.T_Rmode;
				SendMIBMessage (&notifyMsg);

				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPATHSbaAvailable:
			if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthmgt)
			{
				result = SetPATHDataMulti (stnID, p->paramType, pathPtr, p);
				if (result == RC_Success)
				{
					pathPtr->baseData.SbaAvailable = p->PATHPARAM32;

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPATHVendorAttrib:
			/* if XDI attribute */
			if ((p->PATHVENDORID & VENDOR_MASK) == fddiXDI)
				/* use core routine for change */
				result = ChangeXDIPATHAttr (stnID, p, pathPtr);

			else
				/* use I/F routine */
				result = ChangeVendorPATHAttrMulti (stnID, p, pathPtr);

			break;

		default:
			result = RC_NoParameter;
			break;
		}
	}

	else
		/* invalid PATH ID */
		result = RC_NoParameter;

	return (result);
}

uInt32
#ifdef __STDC__
ChangeMIBPORTAttr (uInt16 stnID, Void_p bufData)
#else
ChangeMIBPORTAttr (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change an fddiPORT attribute.
Parameters:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information
							and used to hold attribute value
							returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
*********************************************************************/
{
TLVParamType    *p;						/* parameter structure */
SMTMessage       notifyMsg;				/* message to other processes */
uInt32           result = RC_Success;	/* return value */
MIBType         *mibPtr;				/* MIB database */
Int16            index;					/* array index */
PORTBaseAttrType *portBase;				/* PORT attribute data */
PORTDataPtr      portPtr;				/* MIB PORT element */
PORTRequestedPathsType *reqPaths;		/* requested paths values */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/* check if resource ID is in the table */
	index = MFindResourceID (p->PORTINDEX, mibPtr->portTable,
		mibPtr->smtData.xdi.Port_Ct);
	if (index < 0)
		/* index not found */
		result = RC_NoParameter;

	else
	{
		/* map index to PORT attributes */
		index = mibPtr->portTable[index].index;
		portPtr = &mibPtr->portBase[index];
		portBase = &portPtr->baseData;

		/*
		 * Set up notification message.
		 */
		notifyMsg.source = MIB_MSG_ID;
		notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
		notifyMsg.typeInfo = p->paramType;
		notifyMsg.entity = index;
		notifyMsg.localID = 0;
		notifyMsg.len1 = 0;
		notifyMsg.len2 = 0;
		notifyMsg.stnID = stnID;

		/*
		 * Change value in MIB.
		 */
		switch (p->paramType)
		{
			/*****
			*	Filter GET-ONLY attributes
			*****/
		case fddiPORTMy_Type:
		case fddiPORTIndex:
		case fddiPORTHardwarePresent:
		case fddiPORTNeighborType:
		case fddiPORTMACIndicated:
		case fddiPORTCurrentPath:
		case fddiPORTMACPlacement:
		case fddiPORTAvailablePaths:
		case fddiPORTPMDClass:
		case fddiPORTConnectionCapabilities:
		case fddiPORTBS_Flag:
		case fddiPORTLCTFail_Ct:
		case fddiPORTLer_Estimate:
		case fddiPORTLem_Reject_Ct:
		case fddiPORTLem_Ct:
		case fddiPORTConnectState:
		case fddiPORTPCMState:
		case fddiPORTPC_Withhold:
		case fddiPORTLerFlag:
		case fddiPORTPC_LS:
		case fddiPORTEBError_Ct:
		case fddiPORTConfigGrp:
		case fddiPORTStatusGrp:
		case fddiPORTOperationGrp:
		case fddiPORTErrorCtrsGrp:
		case fddiPORTLerCondition:
		case fddiPORTUndesiredConnectionAttemptEvent:
		case fddiPORTPathChangeEvent:
		case fddiPORTEBErrorCondition:
		case fddiPORTVendorNotification:
			result = RC_IllegalOperation;
			break;

			/*****
			*	GET-REPLACE attributes
			*****/
		case fddiPORTConnectionPolicies:
			portBase->ConnectionPolicies = p->PORTPARAM8;

			/* call routine to store value in NV memory */
			SetPORTDataMulti (stnID, p->paramType, portPtr, p);

			/*
			 * Send notification. Policies are NOT bounded by the object's
			 * capabilities.
			 */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.data.b8 = portBase->ConnectionPolicies;
			SendMIBMessage (&notifyMsg);

			/* force length in case request has extra data */
			p->paramLen = (2 * SIZE_DEFAULT);

			break;

		case fddiPORTRequestedPaths:
			/* set attribute */
			reqPaths = (PORTRequestedPathsType *) & p->PORTGENERAL;
			portBase->RequestedPaths.reqPaths[RP_none]
				= reqPaths->reqPaths[RP_none];
			portBase->RequestedPaths.reqPaths[RP_tree]
				= reqPaths->reqPaths[RP_tree];
			portBase->RequestedPaths.reqPaths[RP_peer]
				= reqPaths->reqPaths[RP_peer];

			/* call routine to store value in NV memory */
			SetPORTDataMulti (stnID, p->paramType, portPtr, p);

			/*
			 * Send notification message. Use member b32 to get start of
			 * first word in data area.
			 */
			notifyMsg.destination = CSP_MSG_ID;
			reqPaths = (PORTRequestedPathsType *) & notifyMsg.data.b32;
			reqPaths->reqPaths[RP_none]
				= portBase->RequestedPaths.reqPaths[RP_none];
			reqPaths->reqPaths[RP_tree]
				= portBase->RequestedPaths.reqPaths[RP_tree];
			reqPaths->reqPaths[RP_peer]
				= portBase->RequestedPaths.reqPaths[RP_peer];
			SendMIBMessage (&notifyMsg);

			/* force length in case request has extra data */
			p->paramLen = (2 * SIZE_DEFAULT);

			break;

		case fddiPORTLer_Cutoff:
			if ((p->PORTPARAM8 >= LER_MIN) && (p->PORTPARAM8 <= LER_MAX))
			{
				portBase->Ler_Cutoff = p->PORTPARAM8;

				/* call routine to store value in NV memory */
				SetPORTDataMulti (stnID, p->paramType, portPtr, p);

				/* send notification message */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.data.b8 = portBase->Ler_Cutoff;
				SendMIBMessage (&notifyMsg);

				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}
			else
				result = RC_OutOfRange;
			break;

		case fddiPORTLer_Alarm:
			if ((p->PORTPARAM8 >= LER_MIN) && (p->PORTPARAM8 <= LER_MAX))
			{
				portBase->Ler_Alarm = p->PORTPARAM8;

				/* call routine to store value in NV memory */
				SetPORTDataMulti (stnID, p->paramType, portPtr, p);

				/* send notification message */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.data.b8 = portBase->Ler_Alarm;
				SendMIBMessage (&notifyMsg);

				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}
			else
				result = RC_OutOfRange;
			break;

		case fddiPORTMACLoop_Time:
			if (portBase->xdi.packages & PKG_maclocalloop)
			{
				portBase->MACLoop_Time = p->PORTPARAM32;

				/* call routine to store value in NV memory */
				SetPORTDataMulti (stnID, p->paramType, portPtr, p);

				/* send notification message */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.data.b32 = portBase->MACLoop_Time;
				SendMIBMessage (&notifyMsg);

				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}

			else
				result = RC_NoParameter;

			break;

		case fddiPORTMaint_LS:
			/* if supported */
			if (portBase->xdi.packages & PKG_maint_LSmanagement)
			{
				/* if within constraints */
				if (p->PORTPARAM16 <= LS_pdr)
				{
					portBase->Maint_LS = p->PORTPARAM16;

					/* call routine to store value in NV */
					SetPORTDataMulti (stnID, p->paramType, portPtr, p);

					/* send notification message */
					notifyMsg.destination = CSP_MSG_ID;
					notifyMsg.data.b8 = portBase->Maint_LS;
					SendMIBMessage (&notifyMsg);

					/* force length in case request has extra data */
					p->paramLen = (2 * SIZE_DEFAULT);
				}
			}
			else
				result = RC_NoParameter;

			break;

		case fddiPORTVendorAttrib:
			/* if XDI attribute */
			if ((p->PORTVENDORID & VENDOR_MASK) == fddiXDI)
				/* use core routine for change */
				result = ChangeXDIPORTAttr (stnID, p, portPtr);

			else
				/* use I/F routine */
				result = ChangeVendorPORTAttrMulti (stnID, p, portPtr);

			break;

			/*
			 * fddiPORTAction
			 */
		case fddiPORTAction:
			switch (p->PORTPARAM16)
			{
			case PORTAction_maintport:
				/* Put maintenance line state in data */
				notifyMsg.data.b8 = portBase->Maint_LS;
				/* rest of message is same as other actions */

			case PORTAction_enableport:
			case PORTAction_disableport:
			case PORTAction_startport:
			case PORTAction_stopport:
				/* Pass message to CSP */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.source = MIB_MSG_ID;
				notifyMsg.type = CSP_ACTION_FDDI_PORT;
				notifyMsg.typeInfo = p->PORTPARAM16;
				SendMIBMessage (&notifyMsg);

				/* force length in case request has extra data */
				p->paramLen = (2 * SIZE_DEFAULT);

				break;

				/* Default for out of range. */
			default:
				result = RC_OutOfRange;
				break;
			}
			break;

		default:
			result = RC_NoParameter;
			break;
		}
	}

	return (result);
}
