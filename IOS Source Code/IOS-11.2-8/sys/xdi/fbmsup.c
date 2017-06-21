/* $Id: fbmsup.c,v 3.2 1995/11/17 18:10:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmsup.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmsup.c,v $
 * Revision 3.2  1995/11/17  18:10:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmsup.c xm7_v3.1.1:cset.0166:15:15 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Frame Support Module

		File:			xm7_fbmsup.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		Frame Based Management for SMT.  This module contains routines
		which support building frames for transmission.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_VSO			include code for vendor-specific object support.
			XM_USE_VSO_MASIC	include code for MASIC object support.
			XM_USE_VSO_OTHER	include code for other vendor objects support.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

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
	Frame Support Routines
*********************************************************************/

#define	MSWAP16(n, order)	(n) = (order) ? Mhtons (n) : Mntohs (n)
#define	MSWAP32(n, order)	(n) = (order) ? Mhtonl (n) : Mntohl (n)

/*
* The following routine is used only when the system includes the MASIC
* object in the MIB.
*/
#ifdef	XM_USE_VSO_MASIC

/*
* This routine is included only when LITTLE_ENDIAN is defined
* in smtdefs.h. The FDDI network is big endian and does not
* require byte swapping on big-endian processors.
*/
#ifdef	LITTLE_ENDIAN

void
#ifdef __STDC__
MASICAttrNetOrder (TLVParamType * param, uInt16 netOrder)
#else
MASICAttrNetOrder (param, netOrder)
		TLVParamType    *param;
		uInt16           netOrder;
#endif
/*********************************************************************
Function:	This function converts a MASIC MIB attribute between host byte
			order and network byte order.
Parameters:	param		= buffer containing MIB attribute.
			netOrder	= selects which order to output.
Input:		param		= contains a TLV encoded MIB attribute.
							Then parameter type, length and VSO ID have
							already been converted by AttrNetOrder().
			netOrder	= if set then convert from host to network byte
							order, else vice versa.
Output:		param		= TLV encoded MIB attribute with only the MASIC
							parameter having been altered by this routine.
							This should put the entire parameter
							(incl. header) into the requested byte order.
Return:		None.
Notes:		The conversion is done _in situ_. If the original value
			should preserved, then it must be copied before
			calling this routine.

			The Parameter_Type and Resource_Index (if
			any) are ALWAYS converted regardless of whether the
			Parameter_Type is valid or not.
Modification History:
*********************************************************************/
{
uInt16           vsoType;				/* MASIC parameter type */

	/*
	 * If the requested order is HOST_ORDER, then the data is assumed to be
	 * in network order. This means that the parameter type and index must
	 * use the converted version.
	 */
	if (netOrder == HOST_ORDER)
	{
		/* convert values */
		param->VSOTYPE = Mntohs (param->VSOTYPE);
		param->VSOINDEX = Mntohs (param->VSOINDEX);

		/* use converted values */
		vsoType = param->VSOTYPE;
	}

	else
	{
		/* use current order */
		vsoType = param->VSOTYPE;

		/* convert values */
		param->VSOTYPE = Mhtons (param->VSOTYPE);
		param->VSOINDEX = Mhtons (param->VSOINDEX);
	}

	switch (vsoType)
	{
		/*
		 * Convert 16-bit values.
		 */
	case calmMASICIndex:
	case calmMASICHardwarePresent:
	case calmMASICXDIIndex:
		MSWAP16 (param->VSOPARAM16, netOrder);
		break;

	default:
		break;
	}

	return;
}

#endif							/* LITTLE_ENDIAN */

#endif							/* XM_USE_VSO_MASIC */

#ifdef	LITTLE_ENDIAN

void
#ifdef __STDC__
AggregateNetOrder (uInt16 paramType, uInt16 paramLen, uInt32 vendorID,
	Void_p data, uInt16 netOrder)
#else
AggregateNetOrder (paramType, paramLen, vendorID, data, netOrder)
		uInt16           paramType;
		uInt16           paramLen;
		uInt32           vendorID;
		Void_p           data;
		uInt16           netOrder;
#endif
/*********************************************************************
Function:	This function support AttrNetOrder() by handling MIB attributes
			that have an aggregate data format. A function is used rather
			than converting inside of AttrNetOrder() in order to reduce
			code size and eliminate duplicate processing.
Parameters:	paramType	= parameter ID.
			paramLen	= length.
			vendorID	= the vendor ID.
			data		= a pointer to the parameter value.
			netOrder	= selects which order to output.
Input:		paramType	= contains the parameter ID in host order.
			paramLen	= length of parameter data in host order. This value
							is only used with fddiPATHConfiguration.
			vendorID	= if the parameter type indicates a vendor-specific
							parameter, then this value holds the vendor's
							parameter ID in host order.
			data		= a generic pointer to the value portion of the
							parameter.
			netOrder	= if set then convert from host to network byte
							order, else vice versa.
Output:		data		= encoded MIB attribute value converted to the
							requested byte order.
Return:		None.
Notes:		The conversion is done _in situ_. If the original value
			should preserved, then it must be copied before
			calling this routine.

			This routine is only included when LITTLE_ENDIAN is defined
			in smtdefs.h. The FDDI network is big endian and does not
			require byte swapping on big-endian processors.
Modification History:
*********************************************************************/
{
uInt16           lenCount;				/* loop counter */
union
{										/* use union to save stack space */
	FDDITimeStamp   *tsp;				/* timestamp format */
	SMTSetCountType *scp;				/* set count format */
	SMTPeerWrapConditionDataType *pwcp;	/* peer wrap condition format */
	MACDuplicateAddressDataType *dacp;	/* duplicate address format */
	MACFrameErrorDataType *fecp;		/* frame error format */
	MACNotCopiedDataType *nccp;			/* not copied format */
	MACNeighborChangeDataType *ncep;	/* neighbor change format */
	MACPathChangeDataType *mpcep;		/* MAC path change format */
	FDDIResourceTag *rtp;				/* PATH configuration */
	PORTLerDataType *lcp;				/* LER format */
	PORTLCTFailedDataType *lcf;			/* LCT failed format */
	PORTUndesiredConnectionDataType *ucep;	/* undesired connection fmt */
	PORTEBErrorDataType *ebecp;			/* EB error format */
	PORTPathChangeDataType *ppcep;		/* PORT path change format */
	TLVPathConfigType *pcp;				/* XDI Path configuration fmt */
	MACConfigurationFailedDataType *cfep;	/* config failed event fmt */
	MACSMTAddressChangedDataType *sacep;/* MAC address changed event */
	XDIConIndexType *cip;				/* connected index format */
	Void_p           v;					/* general assignment */
} ptr;									/* generic pointer */

	/*
	 * Cast generic pointer into union of pointers.
	 */
	ptr.v = data;

	/*
	 * Select parameter processing.
	 */
	switch (paramType)
	{
	case fddiSMTTimeStamp:
	case fddiSMTTransitionTimeStamp:
		MSWAP32 (ptr.tsp->hiword, netOrder);
		MSWAP32 (ptr.tsp->loword, netOrder);
		break;

	case fddiSMTSetCount:
		MSWAP32 (ptr.scp->count, netOrder);
		MSWAP32 (ptr.scp->settimestamp.hiword, netOrder);
		MSWAP32 (ptr.scp->settimestamp.loword, netOrder);
		break;

	case fddiSMTPeerWrapCondition:
		MSWAP16 (ptr.pwcp->cf_state, netOrder);
		break;

	case fddiMACDuplicateAddressCondition:
		MSWAP16 (ptr.dacp->duplicateaddress.dupcondition, netOrder);
		break;

	case fddiMACFrameErrorCondition:
		MSWAP32 (ptr.fecp->frame_ct, netOrder);
		MSWAP32 (ptr.fecp->error_ct, netOrder);
		MSWAP32 (ptr.fecp->lost_ct, netOrder);
		MSWAP16 (ptr.fecp->frameerrorratio, netOrder);
		break;

	case fddiMACNotCopiedCondition:
		MSWAP32 (ptr.nccp->notcopied_ct, netOrder);
		MSWAP32 (ptr.nccp->copied_ct, netOrder);
		MSWAP16 (ptr.nccp->notcopiedratio, netOrder);
		break;

	case fddiMACNeighborChangeEvent:
		MSWAP16 (ptr.ncep->neighborchange.currentpath, netOrder);
		break;

	case fddiMACPathChangeEvent:
		MSWAP16 (ptr.mpcep->pathchange.currentpath, netOrder);
		break;

	case fddiPATHConfiguration:
		/*
		 * Adjust plen by removing size reserved for parameter's index. Test
		 * length before subtracting. If length is too short to begin with,
		 * then skip this processing.
		 */
		if (paramLen > SIZE_DEFAULT)
		{
			paramLen -= SIZE_DEFAULT;
			for (lenCount = 0; lenCount < paramLen;
				lenCount += SIZE_FDDIResourceTag)
			{
				MSWAP16 (ptr.rtp->resourcetype, netOrder);
				MSWAP16 (ptr.rtp->resourceindex, netOrder);
				MSWAP16 (ptr.rtp->currentpath, netOrder);
				ptr.rtp = MADDPTR (FDDIResourceTag, ptr.rtp,
					SIZE_FDDIResourceTag);
			}
		}
		break;

	case fddiPORTLerCondition:
		MSWAP32 (ptr.lcp->ler.ler_reject_ct, netOrder);
		MSWAP32 (ptr.lcp->ler.ler_ct, netOrder);
		break;

	case fddiPORTUndesiredConnectionAttemptEvent:
		MSWAP16 (ptr.ucep->undesiredconnection.porttype, netOrder);
		MSWAP16 (ptr.ucep->undesiredconnection.connectstate, netOrder);
		MSWAP16 (ptr.ucep->undesiredconnection.pc_neighbor, netOrder);
		MSWAP16 (ptr.ucep->undesiredconnection.pc_withhold, netOrder);
		break;

	case fddiPORTEBErrorCondition:
		MSWAP32 (ptr.ebecp->eberrorcount, netOrder);
		break;

	case fddiPORTPathChangeEvent:
		MSWAP16 (ptr.ppcep->pathchange.currentpath, netOrder);
		MSWAP16 (ptr.ppcep->pathchange.mytype, netOrder);
		MSWAP16 (ptr.ppcep->pathchange.neighbortype, netOrder);
		break;

		/*
		 * Handle vendor-specific parameters.
		 */
	case fddiMACVendorAttrib:
	case fddiMACVendorAction:
	case fddiMACVendorNotification:
		if (vendorID == xdiMACPathConfiguration)
		{
			MSWAP16 (ptr.pcp->pathID, netOrder);
		}

		else if (vendorID == xdiMACConnectedIndex)
		{
			MSWAP16 (ptr.cip->resourceType, netOrder);
		}

		else if (vendorID == xdiMACConfigurationFailedEvent)
		{
			MSWAP16 (ptr.cfep->configurationfailed.failreason, netOrder);
			MSWAP32 (ptr.cfep->configurationfailed.tmaxcapability, netOrder);
			MSWAP32 (ptr.cfep->configurationfailed.tvxcapability, netOrder);
		}

		else if (vendorID == xdiMACSMTAddressChangedEvent)
		{
			MSWAP16 (ptr.sacep->addresschanged.currentpath, netOrder);
		}

		break;

	case fddiPATHVendorAttrib:
	case fddiPATHVendorAction:
	case fddiPATHVendorNotification:
		break;

	case fddiPORTVendorAttrib:
	case fddiPORTVendorAction:
	case fddiPORTVendorNotification:
		if (vendorID == xdiPORTPathConfiguration)
		{
			MSWAP16 (ptr.pcp->pathID, netOrder);
		}

		else if (vendorID == xdiPORTConnectedIndex)
		{
			MSWAP16 (ptr.cip->resourceType, netOrder);
		}

		else if (vendorID == xdiPORTLCTFailedEvent)
		{
			MSWAP16 (ptr.lcf->lctfail.mytype, netOrder);
			MSWAP16 (ptr.lcf->lctfail.neighbortype, netOrder);
			MSWAP16 (ptr.lcf->lctfail.pc_withhold, netOrder);
			MSWAP32 (ptr.lcf->lctfail.lctfail_ct, netOrder);
		}

		break;
	}

	return;
}

#endif							/* LITTLE_ENDIAN */

void
#ifdef __STDC__
AttrNetOrder (TLVParamType * param, uInt16 netOrder)
#else
AttrNetOrder (param, netOrder)
		TLVParamType    *param;
		uInt16           netOrder;
#endif
/*********************************************************************
Function:	This function converts a MIB attribute between host byte
			order and network byte order.
Parameters:	param		= buffer containing MIB attribute.
			netOrder	= selects which order to output.
Input:		param		= contains a TLV encoded MIB attribute.
			netOrder	= if set then convert from host to network byte
							order, else vice versa.
Output:		param		= TLV encoded MIB attribute with all values
							(incl. header) converted to requested byte order.
Return:		None.
Notes:		The conversion is done _in situ_. If the original value
			should preserved, then it must be copied before
			calling this routine.

			This routine only operates when LITTLE_ENDIAN is defined
			in smtdefs.h. The FDDI network is big endian and does not
			require byte swapping on big-endian processors.
			The Parameter_Type, Parameter_Length and Resource_Index (if
			any) are ALWAYS converted regardless of whether the
			Parameter_Type is valid or not.
Modification History:
*********************************************************************/
{
#ifdef	LITTLE_ENDIAN
uInt16           ptype;					/* parameter type value */
uInt16           plen;					/* parameter length value */
uInt16           lenCount;				/* loop counter */
uInt16          *index;					/* MAC/PORT index */
uInt32           vendorID;				/* vendor ID in host format */
TLVParamType    *report;				/* report parameter format */
uInt16           reportType;			/* report parameter in host format */

#ifdef	XM_USE_VSO
uInt32           vsoObject;				/* vendor-specific object ID */
#endif							/* XM_USE_VSO */

	/*
	 * If the requested order is HOST_ORDER, then the data is assumed to be
	 * in network order. This means that the parameter type and length must
	 * use the converted version.
	 */
	if (netOrder == HOST_ORDER)
	{
		/* convert values */
		param->paramType = Mntohs (param->paramType);
		param->paramLen = Mntohs (param->paramLen);

		/* use converted values */
		ptype = param->paramType;
		plen = param->paramLen;
	}

	else
	{
		/* use current order */
		ptype = param->paramType;
		plen = param->paramLen;

		/* convert values */
		param->paramType = Mhtons (param->paramType);
		param->paramLen = Mhtons (param->paramLen);
	}

	/*
	 * Handle SMT values that do not have an index.
	 */
	if ((ptype & MIB_OBJECT_MASK) == fddiSMT)
	{

		/*
		 * Select data types by size.
		 */
		switch (ptype)
		{

			/*
			 * Convert 16-bit types.
			 */
		case fddiSMTOpVersionId:
		case fddiSMTHiVersionId:
		case fddiSMTLoVersionId:
		case fddiSMTMIBVersionId:
		case fddiSMTConfigCapabilities:
		case fddiSMTConfigPolicy:
		case fddiSMTConnectionPolicy:
		case fddiSMTT_Notify:
		case fddiSMTECMState:
		case fddiSMTCF_State:
		case fddiSMTStationStatus:
		case fddiSMTStationAction:
			MSWAP16 (param->SMTPARAM16, netOrder);
			break;

			/*
			 * Convert 32-bit types.
			 */
		case fddiSMTTrace_MaxExpiration:
			MSWAP32 (param->SMTPARAM32, netOrder);
			break;

			/*
			 * Convert special types.
			 */
		case fddiSMTMACIndexes:
		case fddiSMTPORTIndexes:
			index = (uInt16 *) param->SMTOTHER;
			for (lenCount = 0; lenCount < plen; lenCount += 2)
			{
				MSWAP16 (*index, netOrder);
				index++;
			}
			break;

		case fddiSMTTimeStamp:
		case fddiSMTTransitionTimeStamp:
		case fddiSMTSetCount:
		case fddiSMTPeerWrapCondition:
			AggregateNetOrder (ptype, (uInt16) 0, (uInt32) 0,
				(Void_p) param->SMTOTHER, netOrder);
			break;

			/*
			 * Handle vendor-specific parameters.
			 */
		case fddiSMTVendorAttrib:
		case fddiSMTVendorAction:
		case fddiSMTVendorNotification:
			/*
			 * Use host order vendor ID.
			 */
			vendorID = (netOrder == HOST_ORDER)
			/* convert to host order */
				? Mntohl (param->SMTVENDORID)
			/* already in host order */
				: param->SMTVENDORID;

			/*
			 * Process XDI attributes.
			 */
			if ((vendorID & VENDOR_MASK) == fddiXDI)
			{
				if (vendorID == xdiSMTPeerWrapReport)
				{
					/* locate report parameter */
					report = (TLVParamType *) param->SMTVENDORDATA;

					/* convert contents */
					MSWAP16 (report->paramType, netOrder);
					MSWAP16 (report->paramLen, netOrder);
					AggregateNetOrder (fddiSMTPeerWrapCondition, (uInt16) 0,
						(uInt32) 0, (Void_p) report->SMTOTHER, netOrder);
				}

#ifdef	XM_USE_VSO_MASIC
				else if (vendorID == calmSMTMASICIndexes)
				{
					/* not defined yet */
				}

				else if (vendorID == calmSMTMASICSecureRingCapability)
				{
					/* not defined yet */
				}

				else if (vendorID == calmSMTMASICSecureRingPolicy)
				{
					/* not defined yet */
				}

				else if (vendorID == calmSMTSecureRingPolicyReport)
				{
					/* not defined yet */
				}

				else if (vendorID == calmSMTSecureRingPolicyEvent)
				{
					/* not defined yet */
				}
#endif							/* XM_USE_VSO_MASIC */

#if (USE_SCHED_GRACE || USE_FULL_GRACE)
				else if ((vendorID == xdiSMTGracefulInsMode)
					|| (vendorID == xdiSMTGIMACID))
				{
					MSWAP16 (param->SMTVEND16, netOrder);
				}
#endif							/* (USE_SCHED_GRACE || USE_FULL_GRACE) */

				/**
				 * Remaining parameters not converted:
				 *
				 * xdiSMTStation_Ct
				 * xdiSMTSynchronousServiceFlag
				 * xdiSMTRMTDupCapabilities
				 * xdiSMTGracefulInsCapabilities
				 * xdiSMTGracefulMax
				 * xdiSMTConnectMax
				 * xdiSMTMaxMasterCanConnect
				 **/
			}

			else
				LocalAttrNetOrder (param, netOrder, ptype, plen);

			/*
			 * Swap vendor ID.
			 */
			param->SMTVENDORID = MSWAP32 (param->SMTVENDORID, netOrder);
			break;

			/**
			 * All other values are not converted.
			 *
			 * fddiSMTStationId
			 * fddiSMTUserData
			 * fddiSMTMac_Ct
			 * fddiSMTNonMaster_Ct
			 * fddiSMTMaster_Ct
			 * fddiSMTAvailablePaths
			 * fddiSMTStatRptPolicy
			 * fddiSMTBypassPresent
			 * fddiSMTRemoteDisconnectFlag
			 * fddiSMTPeerWrapFlag
			 * fddiSMTManufacturerData
			 * fddiSMTLastSetStationId
			 **/
		default:
			break;
		}
	}

#ifdef	XM_USE_VSO
	/*
	 * If supporting vendor-specific objects, then look for the VSO type.
	 */
	else if ((ptype & MIB_OBJECT_MASK) == fddiVSO)
	{
		/* get a copy of object type in host order */
		vsoObject = (netOrder == HOST_ORDER)
			? Mntohl (param->VSOOBJECT)
			: param->VSOOBJECT;

		/*
		 * Check if this is the MASIC object.
		 */
		if (vsoObject == VSO_calmMASIC)
		{
#ifdef	XM_USE_VSO_MASIC
			/*
			 * Convert object ID into requested order.
			 */
			param->VSOOBJECT = (netOrder == HOST_ORDER)
				? Mntohl (param->VSOOBJECT)
				: Mhtonl (param->VSOOBJECT);

			/*
			 * Call MASIC routine to convert data.
			 */
			MASICAttrNetOrder (param, netOrder);
#endif							/* XM_USE_VSO_MASIC */

			/*
			 * If XM_USE_VSO_MASIC is not defined, then this if() clause is
			 * empty and no action is taken.
			 */
		}

		/*
		 * Handle all other vendor-specific objects.
		 */
		else
		{
#ifdef	XM_USE_VSO_OTHER
			/*
			 * Call VSO routine to convert data.
			 */
			LocalAttrNetOrder (param, netOrder, ptype, plen);
#endif							/* XM_USE_VSO_OTHER */

			/*
			 * If XM_USE_VSO_OTHER is not defined, then this if() clause is
			 * empty and no action is taken.
			 */
		}
	}

#endif							/* XM_USE_VSO */

	/*
	 * Handle parameter with an index field.
	 */
	else
	{
		switch (ptype)
		{

			/*
			 * Convert 16-bit values.
			 */
		case fddiMACFrameStatusFunctions:
		case fddiMACBridgeFunctions:
		case fddiMACCurrentPath:
		case fddiMACDupAddressTest:
		case fddiMACDownstreamPORTType:
		case fddiMACIndex:
		case fddiMACFrameErrorThreshold:
		case fddiMACFrameErrorRatio:
		case fddiMACNotCopiedThreshold:
		case fddiMACNotCopiedRatio:
		case fddiMACRMTState:
		case fddiMACHardwarePresent:
		case fddiPATHIndex:
		case fddiPATHTraceStatus:
		case fddiPORTMy_Type:
		case fddiPORTNeighborType:
		case fddiPORTCurrentPath:
		case fddiPORTMACPlacement:
		case fddiPORTPMDClass:
		case fddiPORTIndex:
		case fddiPORTMaint_LS:
		case fddiPORTPC_LS:
		case fddiPORTConnectState:
		case fddiPORTPCMState:
		case fddiPORTPC_Withhold:
		case fddiPORTHardwarePresent:
		case fddiPORTAction:
			MSWAP16 (param->MACPARAM16, netOrder);
			break;

			/*
			 * Convert 32-bit values.
			 */
		case fddiMACT_MaxCapability:
		case fddiMACTVXCapability:
		case fddiMACT_Req:
		case fddiMACT_Neg:
		case fddiMACT_Max:
		case fddiMACTvxValue:
		case fddiMACT_Pri0:
		case fddiMACT_Pri1:
		case fddiMACT_Pri2:
		case fddiMACT_Pri3:
		case fddiMACT_Pri4:
		case fddiMACT_Pri5:
		case fddiMACT_Pri6:
		case fddiMACFrame_Ct:
		case fddiMACCopied_Ct:
		case fddiMACTransmit_Ct:
		case fddiMACToken_Ct:
		case fddiMACError_Ct:
		case fddiMACLost_Ct:
		case fddiMACTvxExpired_Ct:
		case fddiMACNotCopied_Ct:
		case fddiMACLate_Ct:
		case fddiMACRingOp_Ct:
		case fddiPATHRingLatency:
		case fddiPATHSbaPayload:
		case fddiPATHSbaOverhead:
		case fddiPATHT_Rmode:
		case fddiPATHSbaAvailable:
		case fddiPATHTVXLowerBound:
		case fddiPATHT_MaxLowerBound:
		case fddiPATHMaxT_Req:
		case fddiPORTMACLoop_Time:
		case fddiPORTEBError_Ct:
		case fddiPORTLCTFail_Ct:
		case fddiPORTLem_Reject_Ct:
		case fddiPORTLem_Ct:
			MSWAP32 (param->MACPARAM32, netOrder);
			break;

			/*
			 * Convert aggregate values.
			 */
		case fddiMACDuplicateAddressCondition:
		case fddiMACFrameErrorCondition:
		case fddiMACNotCopiedCondition:
		case fddiMACNeighborChangeEvent:
		case fddiMACPathChangeEvent:
		case fddiPATHConfiguration:
		case fddiPORTLerCondition:
		case fddiPORTUndesiredConnectionAttemptEvent:
		case fddiPORTEBErrorCondition:
		case fddiPORTPathChangeEvent:
			AggregateNetOrder (ptype, plen, (uInt32) 0,
				(Void_p) param->MACOTHER, netOrder);
			break;

			/*
			 * Vendor-specific attribute, notification and action processing
			 * for parameters with a resource index.
			 */
		case fddiMACVendorAttrib:
		case fddiMACVendorAction:
		case fddiMACVendorNotification:
			/*
			 * Use host order vendor ID.
			 */
			vendorID = (netOrder == HOST_ORDER)
			/* convert to host order */
				? Mntohl (param->MACVENDORID)
			/* already in host order */
				: param->MACVENDORID;

			/*
			 * Process XDI attributes.
			 */
			if ((vendorID & VENDOR_MASK) == fddiXDI)
			{
				/*
				 * Convert 16-bit values.
				 */
				if ((vendorID == xdiMACRMTDupPolicy) ||
					(vendorID == xdiMACLocalPath) ||
					(vendorID == xdiMACConfigurationStatus))
				{
					MSWAP16 (param->MACVEND16, netOrder);
				}

				/*
				 * Convert special values.
				 */
				else if ((vendorID == xdiMACPathConfiguration)
					|| (vendorID == xdiMACConnectedIndex))
				{
					AggregateNetOrder (ptype, (uInt16) 0, vendorID,
						(Void_p) param->MACVENDORDATA, netOrder);
				}

				/*
				 * Handle reports.
				 */
				else if ((vendorID == xdiMACDuplicateAddressReport)
						|| (vendorID == xdiMACFrameErrorReport)
						|| (vendorID == xdiMACNeighborChangeReport)
						|| (vendorID == xdiMACPathChangeReport)
						|| (vendorID == xdiMACNotCopiedReport)
						|| (vendorID == xdiMACConfigurationFailedReport)
					|| (vendorID == xdiMACSMTAddressChangedReport))
				{
					/* set up pointers */
					report = (TLVParamType *) param->MACVENDORDATA;

					/* get parameter type in host order */
					reportType = (netOrder == HOST_ORDER)
					/* convert to host order */
						? Mntohs (report->paramType)
					/* already in host order */
						: report->paramType;

					/* convert header info */
					MSWAP16 (report->paramType, netOrder);
					MSWAP16 (report->paramLen, netOrder);
					MSWAP16 (report->MACINDEX, netOrder);

					/* convert report-specific data */
					switch (reportType)
					{
					case fddiMACDuplicateAddressCondition:
					case fddiMACFrameErrorCondition:
					case fddiMACNotCopiedCondition:
					case fddiMACNeighborChangeEvent:
					case fddiMACPathChangeEvent:
						AggregateNetOrder (reportType, (uInt16) 0, (uInt32) 0,
							(Void_p) & report->MACGENERAL, netOrder);
						break;

					case fddiMACVendorNotification:
						/* locate report data and Convert vendor ID */
						MSWAP32 (report->MACVENDORID, netOrder);

						/* convert data */
						if (vendorID == xdiMACConfigurationFailedReport)
						{
							AggregateNetOrder (reportType, (uInt16) 0,
								xdiMACConfigurationFailedEvent,
								(Void_p) report->MACVENDORDATA, netOrder);
						}

						else if (vendorID == xdiMACSMTAddressChangedReport)
						{
							AggregateNetOrder (reportType, (uInt16) 0,
								xdiMACSMTAddressChangedEvent,
								(Void_p) report->MACVENDORDATA, netOrder);
						}

						break;

					default:
						break;
					}
				}

				/*
				 * Handle notifications
				 */
				else if ((vendorID == xdiMACConfigurationFailedEvent)
					|| (vendorID == xdiMACSMTAddressChangedEvent))
				{
					AggregateNetOrder (ptype, (uInt16) 0, vendorID,
						(Void_p) param->MACVENDORDATA, netOrder);
				}

				/**
				 * Remaining parameters not converted:
				 *
				 * xdiMACXDIIndex
				 * xdiMACSM_MA_AvailFlag
				 **/
			}

			else
				LocalAttrNetOrder (param, netOrder, ptype, plen);

			/*
			 * Swap vendor ID.
			 */
			param->MACVENDORID = MSWAP32 (param->MACVENDORID, netOrder);
			break;

		case fddiPATHVendorAttrib:
		case fddiPATHVendorAction:
		case fddiPATHVendorNotification:
			/*
			 * Use host order vendor ID.
			 */
			vendorID = (netOrder == HOST_ORDER)
			/* convert to host order */
				? Mntohl (param->PATHVENDORID)
			/* already in host order */
				: param->PATHVENDORID;

			/*
			 * Process XDI attributes.
			 */
			if ((vendorID & VENDOR_MASK) == fddiXDI)
			{
#ifdef	XM_USE_VSO_MASIC
				if (vendorID == calmPATHClaimWinner)
				{
					/* not defined yet */
				}

				else if (vendorID == calmPATHLastBeaconSA)
				{
					/* not defined yet */
				}
#endif							/* XM_USE_VSO_MASIC */

				/**
				 * Remaining parameters not converted:
				 *
				 * xdiPATHXDIIndex
				 **/
			}

			else
				LocalAttrNetOrder (param, netOrder, ptype, plen);

			/*
			 * Swap vendor ID.
			 */
			param->PATHVENDORID = MSWAP32 (param->PATHVENDORID, netOrder);
			break;

		case fddiPORTVendorAction:
		case fddiPORTVendorAttrib:
		case fddiPORTVendorNotification:
			/*
			 * Use host order vendor ID.
			 */
			vendorID = (netOrder == HOST_ORDER)
			/* convert to host order */
				? Mntohl (param->PORTVENDORID)
			/* already in host order */
				: param->PORTVENDORID;

			/*
			 * Process XDI attributes.
			 */
			if ((vendorID & VENDOR_MASK) == fddiXDI)
			{
				/*
				 * Convert 16-bit values.
				 */
				if (vendorID == xdiPORTLocalPath)
				{
					MSWAP16 (param->PORTVEND16, netOrder);
				}

				/*
				 * Convert special values.
				 */
				else if ((vendorID == xdiPORTPathConfiguration)
					|| (vendorID == xdiPORTConnectedIndex))
				{
					AggregateNetOrder (ptype, (uInt16) 0, vendorID,
						(Void_p) param->PORTVENDORDATA, netOrder);
				}

				/*
				 * Handle reports.
				 */
				else if ((vendorID == xdiPORTLerReport)
						|| (vendorID == xdiPORTUndesiredConnectionReport)
						|| (vendorID == xdiPORTPathChangeReport)
						|| (vendorID == xdiPORTEBErrorReport)
					|| (vendorID == xdiPORTLCTFailedReport))
				{
					/* set up pointers */
					report = (TLVParamType *) param->PORTVENDORDATA;

					/* get parameter type in host order */
					reportType = (netOrder == HOST_ORDER)
					/* convert to host order */
						? Mntohs (report->paramType)
					/* already in host order */
						: report->paramType;

					/* convert header info */
					MSWAP16 (report->paramType, netOrder);
					MSWAP16 (report->paramLen, netOrder);
					MSWAP16 (report->PORTINDEX, netOrder);

					/* convert report-specific data */
					switch (reportType)
					{
					case fddiPORTLerCondition:
					case fddiPORTUndesiredConnectionAttemptEvent:
					case fddiPORTPathChangeEvent:
					case fddiPORTEBErrorCondition:
						AggregateNetOrder (reportType, (uInt16) 0, (uInt32) 0,
							(Void_p) & report->PORTGENERAL, netOrder);
						break;

					case fddiPORTVendorNotification:
						/* locate report data and Convert vendor ID */
						MSWAP32 (report->PORTVENDORID, netOrder);

						/* convert data */
						if (vendorID == xdiPORTLCTFailedReport)
						{
							AggregateNetOrder (reportType, (uInt16) 0,
								xdiPORTLCTFailedEvent,
								(Void_p) report->PORTVENDORDATA, netOrder);
						}

						break;

					default:
						break;
					}
				}

				/*
				 * Handle notifications
				 */
				else if (vendorID == xdiPORTLCTFailedEvent)
				{
					AggregateNetOrder (ptype, (uInt16) 0, vendorID,
						(Void_p) param->PORTVENDORDATA, netOrder);
				}

				/**
				 * Remaining parameters not converted:
				 *
				 * xdiPORTXDIIndex
				 * xdiPORTUpstreamMACNeighbor
				 * xdiPORTOldUpstreamMACNeighbor
				 **/
			}

			else
				LocalAttrNetOrder (param, netOrder, ptype, plen);

			/*
			 * Swap vendor ID.
			 */
			param->PORTVENDORID = MSWAP32 (param->PORTVENDORID, netOrder);
			break;

			/**
			 * All other values not converted.
			 *
			 * fddiMACAvailablePaths
			 * fddiMACUpstreamNbr
			 * fddiMACDownstreamNbr
			 * fddiMACOldUpstreamNbr
			 * fddiMACOldDownstreamNbr
			 * fddiMACRequestedPaths
			 * fddiMACSMTAddress
			 * fddiMACLongGrpAddress
			 * fddiMACShortGrpAddress
			 * fddiMACDA_Flag
			 * fddiMACUNDA_Flag
			 * fddiMACFrameErrorFlag
			 * fddiMACNotCopiedFlag
			 * fddiMACMA_UnitdataAvailable
			 * fddiMACMA_UnitdataEnable
			 * fddiPORTConnectionPolicies
			 * fddiPORTMACIndicated
			 * fddiPORTRequestedPaths
			 * fddiPORTAvailablePaths
			 * fddiPORTConnectionCapabilities
			 * fddiPORTBS_Flag
			 * fddiPORTLer_Estimate
			 * fddiPORTLer_Cutoff
			 * fddiPORTLer_Alarm
			 * fddiPORTLerFlag
			 **/
		default:
			break;
		}

		/*
		 * Convert index.
		 */
		MSWAP16 (param->MACINDEX, netOrder);
	}
#endif							/* LITTLE_ENDIAN */

	return;
}


/*********************************************************************
	Frame Parsing Routines
*********************************************************************/

Void_p
#ifdef __STDC__
FindParam (uInt16 paramType, TLVParamType * buffer, uInt16 bufLen, Flag order)
#else
FindParam (paramType, buffer, bufLen, order)
		uInt16           paramType;
		TLVParamType    *buffer;
		uInt16           bufLen;
		Flag             order;
#endif
/*********************************************************************
Function:	This function searches through a buffer to find a
			particular parameter. Parameters must be in TLV format.
Parameters:	paramType	= parameter type to search for.
			buffer		= address of buffer to search.
			bufLen		= length of buffer.
			order		= HOST_ORDER if contents are in host order
						  NET_ORDER if contents are in network order
Input:		buffer		= contains parameters in TLV format.
							Parameters must be consecutive in buffer.
Output:		None.
Return:		Address of header for parameter requested.
			NULL if parameter could not be found.
Notes:		This routine stops searching the buffer if a parameter type
			of 0 is found.
Modifications:
*********************************************************************/
{
uInt16           ptype;					/* parameter type */
uInt16           plen;					/* length of parameter */
ParamHdrType    *phdr;					/* address of parameter header */
Int16            lenLeft;				/* amount of data left in buffer */

	/* get first parameter in buffer */
	phdr = (ParamHdrType *) buffer;
	lenLeft = (Int16) bufLen;

	/* loop through buffer */
	while (lenLeft > SIZE_ParamHdrType)
	{
		/* check type */
		ptype = (order == NET_ORDER)
			? Mntohs (phdr->type)
			: phdr->type;
		plen = (order == NET_ORDER)
			? Mntohs (phdr->length)
			: phdr->length;

		/* exit if type is found */
		if (ptype == paramType)
			break;

		/*
		 * Error conditions are type == 0 or length is not a multiple of 4 or
		 * length == 0. The only parameter that does not have a length that
		 * is a multiple of 4 is the SMT Echo parameter and this MUST be the
		 * last parameter in the buffer.
		 */
		if ((ptype == 0) || (plen & 3) || (plen == 0))
		{
			phdr = (ParamHdrType *) NULL;
			break;
		}

		/* move to next parameter */
		plen += SIZE_ParamHdrType;
		phdr = MADDPTR (ParamHdrType, phdr, plen);
		lenLeft -= plen;
	}

	if (lenLeft <= SIZE_ParamHdrType)
		phdr = (ParamHdrType *) NULL;

	return ((Void_p) phdr);
}

Void_p
#ifdef __STDC__
FindFrameParam (uInt16 paramType, SMTFrameHdrType * frameHdr)
#else
FindFrameParam (paramType, frameHdr)
		uInt16           paramType;
		SMTFrameHdrType *frameHdr;
#endif
/*********************************************************************
Function:	This function searches through a frame to find a
			particular parameter.
Parameters:	paramType	= parameter type to search for.
			frameHdr	= address of start of frame.
Input:		frameHdr	= contains frame to search through.
Output:		None.
Return:		Address of header for parameter requested.
			NULL if parameter could not be found.
*********************************************************************/
{
TLVParamType    *info;					/* info field in frame */
uInt16           infoLen;				/* length of SMT INFO */
Void_p           param;					/* location of parameter in frame */

	/* get size of INFO field */
	infoLen = Mntohs (frameHdr->smtHdr.InfoField_Length);

	/* move to first parameter in SMT frame */
	info = MADDPTR (TLVParamType, frameHdr, SIZE_SMTFrameHdrType);

	/* find parameter */
	param = FindParam (paramType, info, infoLen, NET_ORDER);

	/* return position */
	return (param);
}

uInt16
#ifdef __STDC__
GetMACIndexMulti (uInt16 stnID, uInt16 sourceMAC, uInt16 * outputMAC)
#else
GetMACIndexMulti (stnID, sourceMAC, outputMAC)
		uInt16           stnID;
		uInt16           sourceMAC;
		uInt16          *outputMAC;
#endif
/*********************************************************************
Function:	This function scans the current list of MACs and converts
			the given MIB resource index into the internal array index.
Parameters:	stnID		= index of station to use.
			sourceMAC	= MIB resource ID of the MAC.
			outputMAC	= buffer to hold internal array index.
Input:		fbmMACData	= array of MAC info.
Output:		outputMAC	= internal array index of MAC if found. Otherwise,
							value is MAX_MAC_COUNT.
Return:		0						= if MAC is found
			EFBM_BAD_MAC_INDEX		= if index is not found
			EFBM_MAC_NOT_AVAILABLE	= if MAC found but not available to xmit
Modification History:
*********************************************************************/
{
FBMMACInfoType  *macPtr;				/* pointer into MAC info array */
uInt16           macID;					/* array index */
uInt16           result;				/* search result */

	/*
	 * Initialize return values.
	 */
	*outputMAC = MAX_MAC_COUNT;
	result = EFBM_BAD_MAC_INDEX;

	/*
	 * The size of the MAC array can be up to 255 elements. However, most
	 * systems will have only a handful so a linear search of the array is
	 * done. Also, this routine only services request messages from an
	 * application which should not have any significant response time
	 * constraints.
	 */
	for (macID = 0, macPtr = &(FBMDATA (stnID, fbmMACData[0]));
		macID < MAX_MAC_COUNT;
		macID++, macPtr++)
	{
		/* if resource index matches */
		if (macPtr->Index == sourceMAC)
		{
			/* if MAC is available (implies HW present) */
			if (macPtr->SM_MA_Avail)
			{
				/* use this array value */
				result = 0;
				*outputMAC = macID;
				break;
			}

			else
			{
				/* MAC is not avaiable, exit loop */
				result = EFBM_MAC_NOT_AVAILABLE;
				break;
			}
		}
	}

	return (result);
}


/*********************************************************************
	Transaction ID Generator
*********************************************************************/

uInt32
#ifdef __STDC__
GenTransID (uInt16 stnID)
#else
GenTransID (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Generate a transaction identifier to be used in an outgoing
			SMT frame.  The transaction identifier is a 32 bit number that
			is used to track SMT frames.  The implementation here is to
			keep a global counter which is incremented each time it read.
			The pre-incremented value is returned to the caller to be used
			in the SMT frame.
Parameters:	stnID		= index of station to use.
Input:		Uses fbmStationData.
Output:		Updates transaction ID in fbmStationData.
Return:		A 32-bit number is returned which may be used
			as a transaction identifier in an SMT frame.
Modification History:
*********************************************************************/
{
ProcState        pState;				/* For saving interrupt level */
uInt32           retVal;				/* Value of global counter before
										 * incrementing it. This is the value
										 * returned to our caller */

	MDisableInterrupts (&pState);
	retVal = FBMDATA (stnID, fbmStationData.transactionID);
	FBMDATA (stnID, fbmStationData.transactionID) += 1;
	if (FBMDATA (stnID, fbmStationData.transactionID) == 0)
		FBMDATA (stnID, fbmStationData.transactionID) = 1;
	MRestoreInterrupts (&pState);
	return (retVal);
}

uInt32
#ifdef __STDC__
InitFBMTransId (uInt16 stnID)
#else
InitFBMTransId (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize the transaction identifier to be used in
		outgoing SMT frames.
Parameters:	None.
Input:		None.
Output:		Sets fbmTransactionID to 1.
Return:		0 if successful, otherwise error code.
Modification History:
*********************************************************************/
{
	FBMDATA (stnID, fbmStationData.transactionID) = 1;
	return (0);
}


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt16
#ifdef __STDC__
GetMACIndex (uInt16 sourceMAC, uInt16 * outputMAC)
#else
GetMACIndex (sourceMAC, outputMAC)
		uInt16           sourceMAC;
		uInt16          *outputMAC;
#endif
{
	return (GetMACIndexMulti (FIRST_STATION, sourceMAC, outputMAC));
}

#endif							/* MULTIPLE_STATION */
