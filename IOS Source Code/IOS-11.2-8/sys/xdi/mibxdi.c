/* $Id: mibxdi.c,v 3.2 1995/11/17 18:11:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibxdi.c,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibxdi.c,v $
 * Revision 3.2  1995/11/17  18:11:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibxdi.c xm7_v3.1.1:cset.0166:20:20 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		XDI MIB Attribute Processing Module

		File:			xm7_mibxdi.c
		Created:		04/20/92

		Version:		%R		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module provides access to the FDDI MIB Vendor Attributes
		specified by XDI.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_VSO_MASIC	include code for MASIC object support.

		If the constant listed above is defined, then the
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
	XDI Attribute Routines
*********************************************************************/

uInt32
#ifdef __STDC__
GetXDISMTAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	SMTBaseAttrType * smtPtr)
#else
GetXDISMTAttr (stnID, bufSize, p, smtPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an xdiSMT attribute.
Parameters:	stnID	= index of station to use. This parameter may not be
						used if no interface routines are used to get
						the attribute value.
			bufSize	= maximum length of buffer bufData
						measured in chars.
			p		= pointer to buffer containing the
						requested attribute information
						and used to hold attribute value
						returned.
			smtPtr	= address of SMT attribute data.
Input:		p		= contains the requested attribute ID and
						object index (if required) using
						the MIB TLV encoding for a Get
						Request (see SMT document).
Output:		p		= holds the attribute value requested
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
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
TLVParamType    *report;				/* report format */
SMTPeerWrapConditionDataType *peerWrap;	/* PeerWrap format */

	/*
	 * Check if buffer is minimum size. Data Length is the same for 8, 16,
	 * and 32 bit data types. Header + vendor ID + data.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header amount and vendor ID.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT);

	/*
	 * Length is the same for 8, 16, and 32 bit data types which is the most
	 * common size.
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get report format in case this is a report request.
	 */
	report = (TLVParamType *) p->SMTVENDORDATA;

	/*
	 * Get value from MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if (p->SMTVENDORID == xdiSMTStation_Ct)
	{
		p->SMTVEND8 = smtPtr->xdi.Station_Ct;
	}

	else if (p->SMTVENDORID == xdiSMTSynchronousServiceFlag)
	{
		p->SMTVEND8 = (smtPtr->xdi.syncBandwidth != 0) ? TRUE : FALSE;
	}

	else if (p->SMTVENDORID == xdiSMTRMTDupCapabilities)
	{
		p->SMTVEND8 = smtPtr->xdi.RMTDupCapabilities;
	}

	else if (p->SMTVENDORID == xdiSMTGracefulInsCapabilities)
	{
		if (smtPtr->xdi.packages & PKG_xdigracefulentry)
			p->SMTVEND8 = smtPtr->xdi.GracefulInsCapabilities;
		else
			result = RC_NoParameter;
	}

	else if (p->SMTVENDORID == xdiSMTGracefulMax)
	{
		if (smtPtr->xdi.packages & PKG_xdigracefulentry)
			p->SMTVEND8 = smtPtr->xdi.GracefulMax;
		else
			result = RC_NoParameter;
	}

	else if (p->SMTVENDORID == xdiSMTConnectMax)
	{
		p->SMTVEND8 = smtPtr->xdi.ConnectMax;
	}

	else if (p->SMTVENDORID == xdiSMTGracefulInsMode)
	{
		if (smtPtr->xdi.packages & PKG_xdigracefulentry)
			p->SMTVEND16 = smtPtr->xdi.GracefulInsMode;
		else
			result = RC_NoParameter;
	}

	else if (p->SMTVENDORID == xdiSMTMaxMasterCanConnect)
	{
		p->SMTVEND8 = smtPtr->xdi.MaxMasterCanConnect;
	}

	/* XLNT Entry support */
	else if (p->SMTVENDORID == xdiSMTGIMACID)
	{
		p->SMTVEND16 = smtPtr->xdi.GracefulInsMACID;
	}

#ifdef	XM_USE_VSO_MASIC
	/* CALM MASIC support */
	else if ((p->SMTVENDORID == calmSMTMASICIndexes) ||
			(p->SMTVENDORID == calmSMTMASICSecureRingCapability) ||
			(p->SMTVENDORID == calmSMTMASICSecureRingPolicy) ||
		(p->SMTVENDORID == calmSMTSecureRingPolicyReport))
	{
		result = GetSMTMASICAttr (stnID, p, bufSize);
	}
#endif							/* XM_USE_VSO_MASIC */

	else if (p->SMTVENDORID == xdiSMTPeerWrapReport)
	{
		/* fill in parameter size: hdr + notification */
		p->paramLen = SIZE_TLVHdrType + SIZE_SMTPeerWrapConditionDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;
		else
		{
			/* fill in condition header */
			report->paramType = fddiSMTPeerWrapCondition;
			report->paramLen = SIZE_SMTPeerWrapConditionDataType;

			/* fill in condition data */
			peerWrap = (SMTPeerWrapConditionDataType *) & report->SMTGENERAL;

			peerWrap->conditionstate = smtPtr->PeerWrapFlag;
			peerWrap->cf_state = smtPtr->CF_State;
		}
	}

	else
	{
		result = RC_NoParameter;
	}

	/*
	 * Adjust parameter length to include vendor attr ID.
	 */
	if (result == RC_Success)
		p->paramLen += SIZE_DEFAULT;

	return (result);
}

uInt32
#ifdef __STDC__
GetXDIMACAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	MACDataPtr macPtr)
#else
GetXDIMACAttr (stnID, bufSize, p, macPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an xdiMAC attribute.
Parameters:	stnID	= index of station to use.
			bufSize	= maximum length of buffer bufData
						measured in chars.
			p		= pointer to buffer containing the
						requested attribute information
						and used to hold attribute value
						returned.
			macPtr	= pointer to MAC's attribute structure in
						the MIB.
Input:		p		= contains the requested attribute ID and
						object index (if required) using
						the MIB TLV encoding for a Get
						Request (see SMT standard).
Output:		p		= holds the attribute value requested
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
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
uInt16           path;					/* index for configuration info */
uInt16           saveLen;				/* save orig len for validation */
TLVParamType    *report;				/* report format */
TLVPathConfigType *configPtr;			/* path config format */
XDIConIndexType *macConIndex;			/* connected index format */
/*
*	The pointers in this union are used to format responses to
*	requests. A union is used to reduce stack space requirements.
*/
union
{
	MACDuplicateAddressDataType *dupAddr;
	MACFrameErrorDataType *frameError;
	MACNeighborChangeDataType *nbrChange;
	MACPathChangeDataType *macChange;
	MACNotCopiedDataType *notCopied;
	MACConfigurationFailedDataType *macFailed;
	MACSMTAddressChangedDataType *macChanged;
} ptr;

        /**
         * WRF
         * Initializing macConIndex to avoid compiler warning.
         **/
        macConIndex = (XDIConIndexType *)0;

	/*
	 * Check if buffer is minimum size. Header + index + vendor ID + data
	 * Length is the same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + (3 * SIZE_DEFAULT)))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header, index and vendor ID length.
	 */
	bufSize -= (SIZE_TLVHdrType + (2 * SIZE_DEFAULT));

	/*
	 * Set length to most common size (index added later).
	 */
	saveLen = p->paramLen;
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get report format in case this is a report request.
	 */
	report = (TLVParamType *) p->MACVENDORDATA;

	/*
	 * Get value from MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if (p->MACVENDORID == xdiMACRMTDupPolicy)
	{
		p->MACVEND16 = macPtr->baseData.xdi.RMTDupPolicy;
	}

	else if (p->MACVENDORID == xdiMACXDIIndex)
	{
		p->MACVEND8 = macPtr->baseData.xdi.xdiIndex;
	}

	else if (p->MACVENDORID == xdiMACLocalPath)
	{
		p->MACVEND16 = macPtr->baseData.xdi.localPath;
	}

	else if (p->MACVENDORID == xdiMACPathConfiguration)
	{
		if (bufSize < SIZE_TLVPathConfigType)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Check parameter length to see if it includes the path index.
			 * Length should be resource index + vendor ID + resource index.
			 */
			if (saveLen < (3 * SIZE_DEFAULT))
				result = RC_LengthError;

			else
			{
				configPtr = (TLVPathConfigType *) p->MACVENDORDATA;
				p->paramLen = SIZE_TLVPathConfigType;
				if ((configPtr->pathID < 1)
					|| (configPtr->pathID > MAX_PATH_COUNT))
					result = RC_NoParameter;

				else
				{
					path = configPtr->pathID - 1;
					if (!MIBDATA (stnID, mib->pathBase[path].inUse))
						result = RC_NoParameter;

					else
					{
						MEMCOPY (&configPtr->nextIndex,
							&macPtr->baseData.xdi.configList[path],
							SIZE_DEFAULT);
					}
				}

			}
		}
	}

	else if (p->MACVENDORID == xdiMACSM_MA_AvailFlag)
	{
		if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
			p->MACVEND8 = macPtr->hwData.xdi.SM_MA_AvailFlag;
	}

	else if (p->MACVENDORID == xdiMACConnectedIndex)
	{
		if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;
		else
			macConIndex = (XDIConIndexType *) p->MACVENDORDATA;

		GetConnectedResource (stnID, macPtr->baseData.xdi.xdiIndex,
			RT_mac, &macConIndex->xdiIndex, &macConIndex->resourceType);

	}

	else if (p->MACVENDORID == xdiMACConfigurationStatus)
	{
		if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;
		else
			p->MACVEND16 = macPtr->hwData.xdi.ConfigurationStatus;
	}

	else if (p->MACVENDORID == xdiMACDuplicateAddressReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_MACDuplicateAddressDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;
		else
		{
			/* fill in condition header (size = macID + notif) */
			report->paramType = fddiMACDuplicateAddressCondition;
			report->paramLen = SIZE_DEFAULT + SIZE_MACDuplicateAddressDataType;
			report->MACINDEX = macPtr->baseData.Index;

			/* fill in condition data */
			ptr.dupAddr = (MACDuplicateAddressDataType *) & report->MACGENERAL;

			ptr.dupAddr->duplicateaddress.dupcondition = 0;
			if (macPtr->hwData.DA_Flag)
				ptr.dupAddr->duplicateaddress.dupcondition |= DC_mydup;
			if (macPtr->hwData.UNDA_Flag)
				ptr.dupAddr->duplicateaddress.dupcondition |= DC_unadup;
			MCopyAddress (ptr.dupAddr->duplicateaddress.fddilong,
				macPtr->hwData.SMTAddress);
			MCopyAddress (ptr.dupAddr->duplicateaddress.fddiunalong,
				macPtr->hwData.UpstreamNbr);
		}
	}

	else if (p->MACVENDORID == xdiMACFrameErrorReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_MACFrameErrorDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/* fill in condition header (size = macID + notif) */
			report->paramType = fddiMACFrameErrorCondition;
			report->paramLen = SIZE_DEFAULT + SIZE_MACFrameErrorDataType;
			report->MACINDEX = macPtr->baseData.Index;


			/* fill in condition data */
			ptr.frameError = (MACFrameErrorDataType *) & report->MACGENERAL;

			ptr.frameError->conditionstate = macPtr->hwData.FrameErrorFlag;
			result = GetMACDataMulti (stnID, fddiMACFrame_Ct,
				macPtr, (uChar *) & ptr.frameError->frame_ct);
			if (result == RC_Success)
				result = GetMACDataMulti (stnID, fddiMACError_Ct, macPtr,
					(uChar *) & ptr.frameError->error_ct);
			if (result == RC_Success)
				result = GetMACDataMulti (stnID, fddiMACLost_Ct, macPtr,
					(uChar *) & ptr.frameError->lost_ct);
			ptr.frameError->frameerrorratio = macPtr->hwData.FrameErrorRatio;
		}
	}

	else if (p->MACVENDORID == xdiMACNeighborChangeReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_MACNeighborChangeDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/* fill in condition header (size = macID + notif) */
			report->paramType = fddiMACNeighborChangeEvent;
			report->paramLen = SIZE_DEFAULT + SIZE_MACNeighborChangeDataType;
			report->MACINDEX = macPtr->baseData.Index;

			/* fill in event data */
			ptr.nbrChange = (MACNeighborChangeDataType *) & report->MACGENERAL;

			/* always set multiple FALSE, SRP tracks this */
			ptr.nbrChange->multipleoccurrence = FALSE;
			ptr.nbrChange->neighborchange.nacondition
				= macPtr->hwData.xdi.neighborFlags;
			macPtr->hwData.xdi.neighborFlags = NAC_None;
			MCopyAddress (ptr.nbrChange->neighborchange.old_una,
				macPtr->hwData.OldUpstreamNbr);
			MCopyAddress (ptr.nbrChange->neighborchange.new_una,
				macPtr->hwData.UpstreamNbr);
			MCopyAddress (ptr.nbrChange->neighborchange.old_dna,
				macPtr->hwData.OldDownstreamNbr);
			MCopyAddress (ptr.nbrChange->neighborchange.new_dna,
				macPtr->hwData.DownstreamNbr);
			ptr.nbrChange->neighborchange.currentpath
				= macPtr->hwData.CurrentPath;
			MCopyAddress (ptr.nbrChange->neighborchange.smtaddress,
				macPtr->hwData.SMTAddress);
		}
	}

	else if (p->MACVENDORID == xdiMACPathChangeReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_MACPathChangeDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/* fill in condition header (size = macID + notif) */
			report->paramType = fddiMACPathChangeEvent;
			report->paramLen = SIZE_DEFAULT + SIZE_MACPathChangeDataType;
			report->MACINDEX = macPtr->baseData.Index;

			/* set event */
			ptr.macChange = (MACPathChangeDataType *) & report->MACGENERAL;

			/* fill in event data */
			/* always set multiple FALSE, SRP tracks this */
			ptr.macChange->multipleoccurrence = FALSE;
			ptr.macChange->pathchange.currentpath
				= macPtr->hwData.CurrentPath;
			ptr.macChange->pathchange.availablepaths
				= macPtr->hwData.AvailablePaths;
			MEMCOPY (&ptr.macChange->pathchange.requestedpaths,
				&macPtr->baseData.RequestedPaths, SIZE_DEFAULT);
		}
	}

	else if (p->MACVENDORID == xdiMACNotCopiedReport)
	{
		if (macPtr->baseData.xdi.packages & PKG_framenotcopied)
		{
			/*
			 * Fill in parameter size: hdr + mac ID + notification.
			 */
			p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
				+ SIZE_MACNotCopiedDataType;

			if (bufSize < p->paramLen)
				result = RC_BUFFER_ERROR;

			else if (!macPtr->baseData.HardwarePresent)
				result = RC_NoParameter;

			else
			{
				/* fill in condition header (size = macID + notif) */
				report->paramType = fddiMACNotCopiedCondition;
				report->paramLen = SIZE_DEFAULT + SIZE_MACNotCopiedDataType;
				report->MACINDEX = macPtr->baseData.Index;

				/* set condition */
				ptr.notCopied = (MACNotCopiedDataType *) & report->MACGENERAL;

				/* fill in condition data */
				ptr.notCopied->conditionstate = macPtr->hwData.NotCopiedFlag;
				result = GetMACDataMulti (stnID, fddiMACNotCopied_Ct,
					macPtr, (uChar *) & ptr.notCopied->notcopied_ct);
				if (result == RC_Success)
					result = GetMACDataMulti (stnID, fddiMACCopied_Ct, macPtr,
						(uChar *) & ptr.notCopied->copied_ct);
				ptr.notCopied->notcopiedratio = macPtr->hwData.NotCopiedRatio;
			}
		}
		else
			result = RC_NoParameter;

	}

	else if (p->MACVENDORID == xdiMACConfigurationFailedReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + vendor ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_DEFAULT + SIZE_MACConfigurationFailedDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in condition header: size = mac ID + vendor ID +
			 * notification
			 */
			report->paramType = fddiMACVendorNotification;
			report->paramLen = SIZE_DEFAULT + SIZE_DEFAULT
				+ SIZE_MACConfigurationFailedDataType;
			report->MACINDEX = macPtr->baseData.Index;
			report->MACVENDORID = xdiMACConfigurationFailedEvent;

			/* set event */
			ptr.macFailed = (MACConfigurationFailedDataType *)
				report->MACVENDORDATA;

			/* fill in event data */
			/* always set multiple FALSE, SRP tracks this */
			ptr.macFailed->multipleoccurrence = FALSE;
			ptr.macFailed->configurationfailed.availablepaths
				= macPtr->hwData.AvailablePaths;
			ptr.macFailed->configurationfailed.requestedpaths
				= macPtr->baseData.RequestedPaths.reqPaths;
			ptr.macFailed->configurationfailed.failreason
				= macPtr->hwData.xdi.ConfigurationStatus;
			ptr.macFailed->configurationfailed.tmaxcapability
				= macPtr->hwData.T_MaxCapability;
			ptr.macFailed->configurationfailed.tvxcapability
				= macPtr->hwData.TVXCapability;
		}
	}

	else if (p->MACVENDORID == xdiMACSMTAddressChangedReport)
	{
		/*
		 * Fill in parameter size: hdr + mac ID + vendor ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_DEFAULT + SIZE_MACSMTAddressChangedDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!macPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in condition header: size = mac ID + vendor ID +
			 * notification
			 */
			report->paramType = fddiMACVendorNotification;
			report->paramLen = SIZE_DEFAULT + SIZE_DEFAULT
				+ SIZE_MACSMTAddressChangedDataType;
			report->MACINDEX = macPtr->baseData.Index;
			report->MACVENDORID = xdiMACSMTAddressChangedEvent;

			/* fill in event data */
			ptr.macChanged = (MACSMTAddressChangedDataType *)
				report->MACVENDORDATA;

			/* always set multiple FALSE, SRP tracks this */
			ptr.macChanged->multipleoccurrence = FALSE;
			ptr.macChanged->addresschanged.availablepaths
				= macPtr->hwData.AvailablePaths;
			ptr.macChanged->addresschanged.requestedpaths
				= macPtr->baseData.RequestedPaths.reqPaths;
			ptr.macChanged->addresschanged.currentpath
				= macPtr->hwData.CurrentPath;
			MCopyAddress (ptr.macChanged->addresschanged.previoussmtaddress,
				macPtr->hwData.xdi.oldSMTAddress);
			MCopyAddress (ptr.macChanged->addresschanged.currentsmtaddress,
				macPtr->hwData.SMTAddress);
		}
	}

	else
	{
		result = RC_NoParameter;
	}

	/*
	 * Parameter length includes data (already set) + vendor ID + resource
	 * index
	 */
	if (result == RC_Success)
		p->paramLen += (SIZE_DEFAULT + SIZE_DEFAULT);

	return (result);
}

uInt32
#ifdef __STDC__
GetXDIPATHAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PATHDataPtr pathPtr)
#else
GetXDIPATHAttr (stnID, bufSize, p, pathPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PATHDataPtr      pathPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an xdiPATH attribute.
Parameters:	stnID	= index of station to use. This parameter may not be
						used if no interface routines are called to get
						attribute information.
			bufSize	= maximum length of buffer bufData
						measured in chars.
			p		= pointer to buffer containing the
						requested attribute information
						and used to hold attribute value
						returned.
			pathPtr	= pointer to PATH's attribute structure in
						the MIB.
Input:		p		= contains the requested attribute ID and
						object index (if required) using
						the MIB TLV encoding for a Get
						Request (see SMT document).
Output:		p		= holds the attribute value requested
						using the MIB TLV encoding specified
						in the SMT document.
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
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. Header + index + vendor ID + data
	 * Length is the same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header, index and vendor ID length.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT);

	/*
	 * Set length to most common size (index added later).
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if (p->PATHVENDORID == xdiPATHXDIIndex)
	{
		p->PATHVEND8 = pathPtr->baseData.xdi.xdiIndex;
	}

#ifdef	XM_USE_VSO_MASIC
	/* CALM MASIC support */
	else if ((p->PATHVENDORID == calmPATHClaimWinner) ||
		(p->PATHVENDORID == calmPATHClaimWinner))
	{
		result = GetPATHMASICAttr (stnID, p, bufSize);
	}
#endif							/* XM_USE_VSO_MASIC */

	else
	{
		result = RC_NoParameter;
	}

	/*
	 * Parameter length includes data (already set) + vendor ID + resource
	 * index
	 */
	if (result == RC_Success)
		p->paramLen += (SIZE_DEFAULT + SIZE_DEFAULT);

	return (result);
}

uInt32
#ifdef __STDC__
GetXDIPORTAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PORTDataPtr portPtr)
#else
GetXDIPORTAttr (stnID, bufSize, p, portPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an xdiPORT attribute.
Parameters:	stnID	= index of station to use.
			bufSize	= maximum length of buffer bufData
						measured in chars.
			p		= pointer to buffer containing the
						requested attribute information
						and used to hold attribute value
						returned.
			portPtr	= pointer to PORT's attribute structure in
						the MIB.
Input:		p		= contains the requested attribute ID and
						object index (if required) using
						the MIB TLV encoding for a Get
						Request (see SMT document).
Output:		p		= holds the attribute value requested
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
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
uInt16           path;					/* index for configuration info */
uInt16           saveLen;				/* save orig length for validation */
TLVParamType    *report;				/* report format */
XDIConIndexType *portConIndex;			/* connected index format */
TLVPathConfigType *configPtr;			/* path config format */
/*
*	The pointers in this union are used to format responses to
*	requests. A union is used to reduce stack space requirements.
*/
union
{
	PORTLerDataType *ler;
	PORTUndesiredConnectionDataType *undesired;
	PORTPathChangeDataType *portChange;
	PORTEBErrorDataType *ebError;
	PORTLCTFailedDataType *lctFailed;
} ptr;

        /**
         * WRF
         * Initializing portConIndex to avoid compiler warning.
         **/
        portConIndex = (XDIConIndexType *)0;

	/*
	 * Check if buffer is minimum size. Header + index + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce size of buffer by header, vendor ID and index.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT);

	/*
	 * Set length to most common size.
	 */
	saveLen = p->paramLen;
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get report format in case this is a report request.
	 */
	report = (TLVParamType *) p->MACVENDORDATA;

	/*
	 * Get value from MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if (p->PORTVENDORID == xdiPORTXDIIndex)
	{
		p->PORTVEND8 = portPtr->baseData.xdi.xdiIndex;
	}

	else if (p->PORTVENDORID == xdiPORTLocalPath)
	{
		p->PORTVEND16 = portPtr->baseData.xdi.localPath;
	}

	else if (p->PORTVENDORID == xdiPORTPathConfiguration)
	{
		if (bufSize < SIZE_TLVPathConfigType)
			result = RC_BUFFER_ERROR;

		else if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Check parameter length to see if it includes the path index.
			 * Length should be resource index + vendor ID + resource index.
			 */
			if (saveLen < (3 * SIZE_DEFAULT))
				result = RC_LengthError;

			else
			{
				configPtr = (TLVPathConfigType *) p->PORTVENDORDATA;
				p->paramLen = SIZE_TLVPathConfigType;
				if ((configPtr->pathID < 1)
					|| (configPtr->pathID > MAX_PATH_COUNT))
					result = RC_NoParameter;

				else
				{
					path = configPtr->pathID - 1;
					if (!MIBDATA (stnID, mib->pathBase[path].inUse))
						result = RC_NoParameter;

					else
					{
						MEMCOPY (&configPtr->nextIndex,
							&portPtr->baseData.xdi.configList[path],
							SIZE_DEFAULT);
					}
				}
			}
		}
	}

	else if (p->PORTVENDORID == xdiPORTConnectedIndex)
	{
		if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;
		else
			portConIndex = (XDIConIndexType *) p->PORTVENDORDATA;

		GetConnectedResource (stnID, portPtr->baseData.xdi.xdiIndex,
			RT_port, &portConIndex->xdiIndex, &portConIndex->resourceType);

	}

	else if (p->PORTVENDORID == xdiPORTUpstreamMACNeighbor)
	{
		if (portPtr->baseData.xdi.packages & PKG_xdiportmapping)
		{
			MCopyAddress (p->PORTVENDADDR,
				portPtr->hwData.xdi.UpstreamMACNeighbor);
		}
		else
			result = RC_NoParameter;
	}

	else if (p->PORTVENDORID == xdiPORTOldUpstreamMACNeighbor)
	{
		if (portPtr->baseData.xdi.packages & PKG_xdiportmapping)
		{
			MCopyAddress (p->PORTVENDADDR,
				portPtr->hwData.xdi.OldUpstreamMACNeighbor);
		}
		else
			result = RC_NoParameter;
	}

	else if (p->PORTVENDORID == xdiPORTLerReport)
	{
		/*
		 * Fill in parameter size: hdr + port ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_PORTLerDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in condition header: size = port ID + notification
			 */
			report->paramType = fddiPORTLerCondition;
			report->paramLen = SIZE_DEFAULT + SIZE_PORTLerDataType;
			report->PORTINDEX = portPtr->baseData.Index;

			/* fill in condition data */
			ptr.ler = (PORTLerDataType *) & report->PORTGENERAL;
			ptr.ler->conditionstate = portPtr->hwData.LerFlag;
			ptr.ler->ler.ler_cutoff = portPtr->baseData.Ler_Cutoff;
			ptr.ler->ler.ler_alarm = portPtr->baseData.Ler_Alarm;
			ptr.ler->ler.ler_estimate = portPtr->hwData.Ler_Estimate;
			ptr.ler->ler.ler_reject_ct = portPtr->hwData.Lem_Reject_Ct;
			ptr.ler->ler.ler_ct = portPtr->hwData.Lem_Ct;
		}
	}

	else if (p->PORTVENDORID == xdiPORTUndesiredConnectionReport)
	{
		/*
		 * Fill in parameter size: hdr + port ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_PORTUndesiredConnectionDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in condition header: size = port ID + notification
			 */
			report->paramType = fddiPORTUndesiredConnectionAttemptEvent;
			report->paramLen = SIZE_DEFAULT
				+ SIZE_PORTUndesiredConnectionDataType;
			report->PORTINDEX = portPtr->baseData.Index;

			/* fill in condition data */
			ptr.undesired = (PORTUndesiredConnectionDataType *)
				& report->PORTGENERAL;

			/* always set multiple FALSE, SRP tracks this */
			ptr.undesired->multipleoccurrence = FALSE;
			ptr.undesired->undesiredconnection.porttype
				= portPtr->baseData.My_Type;
			ptr.undesired->undesiredconnection.connectstate
				= portPtr->hwData.ConnectState;
			ptr.undesired->undesiredconnection.pc_neighbor
				= portPtr->hwData.NeighborType;
			ptr.undesired->undesiredconnection.pc_withhold
				= portPtr->hwData.PC_Withhold;
		}
	}

	else if (p->PORTVENDORID == xdiPORTPathChangeReport)
	{
		/*
		 * Fill in parameter size: hdr + port ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_PORTPathChangeDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in event header: size = port ID + vendor ID +
			 * notification
			 */
			report->paramType = fddiPORTPathChangeEvent;
			report->paramLen = SIZE_DEFAULT + SIZE_PORTPathChangeDataType;
			report->PORTINDEX = portPtr->baseData.Index;

			/* fill in event data */
			ptr.portChange = (PORTPathChangeDataType *) & report->PORTGENERAL;

			/* always set multiple FALSE, SRP tracks this */
			ptr.portChange->multipleoccurrence = FALSE;
			ptr.portChange->pathchange.currentpath
				= portPtr->hwData.CurrentPath;
			ptr.portChange->pathchange.availablepaths
				= portPtr->hwData.AvailablePaths;
			MEMCOPY (&ptr.portChange->pathchange.requestedpaths,
				&portPtr->baseData.RequestedPaths, SIZE_DEFAULT);
			ptr.portChange->pathchange.mytype = portPtr->baseData.My_Type;
			ptr.portChange->pathchange.neighbortype
				= portPtr->hwData.NeighborType;
		}
	}

	else if (p->PORTVENDORID == xdiPORTEBErrorReport)
	{
		if (portPtr->baseData.xdi.packages & PKG_eberrorcounter)
		{
			/*
			 * Fill in parameter size: hdr + port ID + notification.
			 */
			p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
				+ SIZE_PORTEBErrorDataType;

			if (bufSize < p->paramLen)
				result = RC_BUFFER_ERROR;

			else if (!portPtr->baseData.HardwarePresent)
				result = RC_NoParameter;

			else
			{
				/*
				 * Fill in condition header: size = port ID + vendor ID +
				 * notification
				 */
				report->paramType = fddiPORTEBErrorCondition;
				report->paramLen = SIZE_DEFAULT + SIZE_PORTEBErrorDataType;
				report->PORTINDEX = portPtr->baseData.Index;

				/* fill in condition data */
				ptr.ebError = (PORTEBErrorDataType *) & report->PORTGENERAL;

				ptr.ebError->conditionstate = portPtr->hwData.xdi.EBErrorFlag;
				result = GetPORTDataMulti (stnID, fddiPORTEBError_Ct,
					portPtr, (uChar *) & ptr.ebError->eberrorcount);
			}
		}

		else
			result = RC_NoParameter;

	}

	else if (p->PORTVENDORID == xdiPORTLCTFailedReport)
	{
		/*
		 * Fill in parameter size: hdr + port ID vendor ID + notification.
		 */
		p->paramLen = SIZE_TLVHdrType + SIZE_DEFAULT
			+ SIZE_DEFAULT + SIZE_PORTLCTFailedDataType;

		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else if (!portPtr->baseData.HardwarePresent)
			result = RC_NoParameter;

		else
		{
			/*
			 * Fill in condition header: size = port ID + vendor ID +
			 * notification
			 */
			report->paramType = fddiPORTVendorNotification;
			report->paramLen = SIZE_DEFAULT + SIZE_DEFAULT
				+ SIZE_PORTLCTFailedDataType;
			report->PORTINDEX = portPtr->baseData.Index;
			report->PORTVENDORID = xdiPORTLCTFailedEvent;

			/* set event */
			ptr.lctFailed = (PORTLCTFailedDataType *) report->PORTVENDORDATA;

			/* fill in event data */
			/* always set multiple FALSE, SRP tracks this */
			ptr.lctFailed->multipleoccurrence = FALSE;
			ptr.lctFailed->lctfail.mytype = portPtr->baseData.My_Type;
			ptr.lctFailed->lctfail.neighbortype = portPtr->hwData.NeighborType;
			ptr.lctFailed->lctfail.pc_withhold = portPtr->hwData.PC_Withhold;
			ptr.lctFailed->lctfail.lctfail_ct = portPtr->hwData.LCTFail_Ct;
		}
	}

	else
	{
		result = RC_NoParameter;
	}

	/*
	 * Parameter length includes data (already set) + vendor ID + resource
	 * index
	 */
	if (result == RC_Success)
		p->paramLen += (SIZE_DEFAULT + SIZE_DEFAULT);

	return (result);
}


/*********************************************************************
	Change XDI Attribute Routines
*********************************************************************/

uInt32
#ifdef __STDC__
ChangeXDISMTAttr (uInt16 stnID, TLVParamType * p, SMTBaseAttrType * smtPtr)
#else
ChangeXDISMTAttr (stnID, p, smtPtr)
		uInt16           stnID;
		TLVParamType    *p;
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change an xdiSMT attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			smtPtr	= address of SMT attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT document).
Output:		p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
SMTMessage       notifyMsg;				/* message to other processes */
MACDataPtr       macPtr;				/* 921123.GI MIB element for MAC */
Int16            index;					/* 921123.GI array index */
Flag             GIModeChanged;			/* 921123.GI graceful mode changed */

	/*
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if ((p->SMTVENDORID == xdiSMTStation_Ct) ||
		(p->SMTVENDORID == xdiSMTSynchronousServiceFlag) ||
		(p->SMTVENDORID == xdiSMTRMTDupCapabilities) ||
		(p->SMTVENDORID == xdiSMTGracefulInsCapabilities) ||
		(p->SMTVENDORID == xdiSMTGracefulMax) ||
		(p->SMTVENDORID == xdiSMTConnectMax) ||
		(p->SMTVENDORID == xdiSMTPeerWrapReport))
	{
		result = RC_IllegalOperation;
	}

	/* XLNT Entry support */
	else if (p->SMTVENDORID == xdiSMTGracefulInsMode)
	{
		if (smtPtr->xdi.packages & PKG_xdigracefulentry)
		{
			GIModeChanged = FALSE;
			switch (p->SMTVEND16)
			{
			case GIM_off:
				if (smtPtr->xdi.GracefulInsMode != GIM_off)
					GIModeChanged = TRUE;
				smtPtr->xdi.GracefulInsMode = GIM_off;
				break;

			case GIM_scheduledinsertion:
				/* validate capability */
				if (smtPtr->xdi.GracefulInsCapabilities
					& GIC_scheduledinsertion)
				{
					if (smtPtr->xdi.GracefulInsMode != GIM_scheduledinsertion)
						GIModeChanged = TRUE;
					smtPtr->xdi.GracefulInsMode = GIM_scheduledinsertion;
				}

				else
					result = RC_OutOfRange;

				break;

			case GIM_gracefulinsertion:
				/* validate capability */
				if (smtPtr->xdi.GracefulInsCapabilities
					& GIC_gracefulinsertion)
				{
					if (smtPtr->xdi.GracefulInsMode != GIM_gracefulinsertion)
						GIModeChanged = TRUE;
					smtPtr->xdi.GracefulInsMode = GIM_gracefulinsertion;
				}

				else
					result = RC_OutOfRange;

				break;

			case GIM_diagnostics:
				/* validate capability */
				if (smtPtr->xdi.GracefulInsCapabilities
					& GIC_diagnostics)
				{
					if (smtPtr->xdi.GracefulInsMode != GIM_diagnostics)
						GIModeChanged = TRUE;
					smtPtr->xdi.GracefulInsMode = GIM_diagnostics;
				}

				else
					result = RC_OutOfRange;

				break;

			default:
				result = RC_OutOfRange;
				break;
			}

			if (result == RC_Success)
			{
				/* set changed value */
				if (GIModeChanged)
				{
					/* call routine to store value in NV memory */
					SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

					/* Notify CSP */
					notifyMsg.destination = CSP_MSG_ID;
					notifyMsg.source = MIB_MSG_ID;
					notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
					notifyMsg.typeInfo = fddiSMTVendorAttrib;
					notifyMsg.vendorID = xdiSMTGracefulInsMode;
					notifyMsg.entity = 0;
					notifyMsg.localID = 0;
					notifyMsg.data.b16 = p->SMTVEND16;
					notifyMsg.len1 = 0;
					notifyMsg.len2 = 0;
					notifyMsg.stnID = stnID;
					SendMIBMessage (&notifyMsg);
				}

				/* set length in case request had extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}
		}

		else
			result = RC_NoParameter;

	}

	/* XLNT Entry support */
	else if (p->SMTVENDORID == xdiSMTMaxMasterCanConnect)
	{
		/* check if there is a limit */
		if (smtPtr->xdi.ConnectMax)
		{
			/* if not within limit */
			if ((p->SMTVEND8 == 0)
				|| (p->SMTVEND8 > smtPtr->xdi.ConnectMax))
			{
				result = RC_OutOfRange;
			}
		}

		if (result == RC_Success)
		{
			/* set new value */
			smtPtr->xdi.MaxMasterCanConnect = p->SMTVEND8;

			/* call routine to store value in NV memory */
			SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

			/* Pass message to CSP */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.source = MIB_MSG_ID;
			notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
			notifyMsg.typeInfo = fddiSMTVendorAttrib;
			notifyMsg.vendorID = xdiSMTMaxMasterCanConnect;
			notifyMsg.entity = 0;
			notifyMsg.localID = 0;
			notifyMsg.data.b8 = p->SMTVEND8;
			notifyMsg.len1 = 0;
			notifyMsg.len2 = 0;
			notifyMsg.stnID = stnID;
			SendMIBMessage (&notifyMsg);

			/* set length in case request had extra data */
			p->paramLen = (2 * SIZE_DEFAULT);
		}
	}

	/* XLNT Entry support */
	else if (p->SMTVENDORID == xdiSMTGIMACID)
	{
		/* set new value */
		index = MFindResourceID (p->SMTVEND16,
			MIBDATA (stnID, mib->macTable),
			MIBDATA (stnID, mib->smtData.Mac_Ct));

		if (index < 0)
		{
			result = RC_OutOfRange;
		}

		else
		{
			index = MIBDATA (stnID, mib->macTable[index].index);
			macPtr = &MIBDATA (stnID, mib->macBase[index]);

			if (!(macPtr->hwData.AvailablePaths & AP_local))
			{
				result = RC_OutOfRange;
			}

			else
			{
				smtPtr->xdi.GracefulInsMACID = p->SMTVEND16;

				/* call routine to store value in NV memory */
				SetSMTDataMulti (stnID, p->paramType, smtPtr, p);

				/* Pass message to CSP */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.source = MIB_MSG_ID;
				notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
				notifyMsg.typeInfo = fddiSMTVendorAttrib;
				notifyMsg.vendorID = xdiSMTGIMACID;
				notifyMsg.entity = 0;
				notifyMsg.localID = 0;
				notifyMsg.data.b16 = index;
				notifyMsg.len1 = 0;
				notifyMsg.len2 = 0;
				notifyMsg.stnID = stnID;
				SendMIBMessage (&notifyMsg);

				/* set length in case request had extra data */
				p->paramLen = (2 * SIZE_DEFAULT);
			}
		}
	}

#ifdef	XM_USE_VSO_MASIC
	/* CALM MASIC support */
	else if ((p->SMTVENDORID == calmSMTMASICIndexes) ||
			(p->SMTVENDORID == calmSMTMASICSecureRingCapability) ||
		(p->SMTVENDORID == calmSMTSecureRingPolicyReport))
	{
		result = RC_IllegalOperation;
	}

	else if (p->SMTVENDORID == calmSMTMASICSecureRingPolicy)
	{
		result = ChangeSMTMASICAttr (stnID, p);
	}
#endif							/* XM_USE_VSO_MASIC */

	else
	{
		result = RC_NoParameter;
	}


	return (result);
}

uInt32
#ifdef __STDC__
ChangeXDIMACAttr (uInt16 stnID, TLVParamType * p, MACDataPtr macPtr)
#else
ChangeXDIMACAttr (stnID, p, macPtr)
		uInt16           stnID;
		TLVParamType    *p;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change an xdiMAC attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			macPtr	= address of MAC attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT document).
Output:		p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
SMTMessage       notifyMsg;				/* message to other processes */
uChar            temp;					/* temp value */

	/*
	 * Set up notification message.
	 */
	notifyMsg.source = MIB_MSG_ID;
	notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
	notifyMsg.entity = macPtr->baseData.xdi.xdiIndex;
	notifyMsg.localID = 0;
	notifyMsg.len1 = 0;
	notifyMsg.len2 = 0;
	notifyMsg.stnID = stnID;

	/*
	 * Change value in MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if ((p->MACVENDORID == xdiMACXDIIndex) ||
		(p->MACVENDORID == xdiMACPathConfiguration) ||
		(p->MACVENDORID == xdiMACSM_MA_AvailFlag) ||
		(p->MACVENDORID == xdiMACConnectedIndex) ||
		(p->MACVENDORID == xdiMACConfigurationStatus) ||
		(p->MACVENDORID == xdiMACDuplicateAddressReport) ||
		(p->MACVENDORID == xdiMACFrameErrorReport) ||
		(p->MACVENDORID == xdiMACNeighborChangeReport) ||
		(p->MACVENDORID == xdiMACPathChangeReport) ||
		(p->MACVENDORID == xdiMACNotCopiedReport) ||
		(p->MACVENDORID == xdiMACConfigurationFailedReport) ||
		(p->MACVENDORID == xdiMACSMTAddressChangedReport))
	{
		result = RC_IllegalOperation;
	}

	else if (p->MACVENDORID == xdiMACRMTDupPolicy)
	{
		/* validate range */
		if ((p->MACVEND16 < RMT_DUP_CHANGE) || (p->MACVEND16 > RMT_DUP_LEAVE))
		{
			result = RC_OutOfRange;
		}

		else
		{
			/* convert to bit position */
			temp = (1 << (p->MACVEND16 - 1));

			/* if capability is set */
			if (temp & MIBDATA (stnID, mib->smtData.xdi.RMTDupCapabilities))
			{
				/* set new value */
				macPtr->baseData.xdi.RMTDupPolicy = p->MACVEND16;

				/* call routine to store value in NV memory */
				SetMACDataMulti (stnID, p->paramType, macPtr, p);

				/* notify CSP */
				notifyMsg.destination = CSP_MSG_ID;
				notifyMsg.typeInfo = fddiMACVendorAttrib;
				notifyMsg.vendorID = xdiMACRMTDupPolicy;
				notifyMsg.data.b16 = macPtr->baseData.xdi.RMTDupPolicy;
				SendMIBMessage (&notifyMsg);

				/* set length in case request had extra data */
				p->paramLen = (3 * SIZE_DEFAULT);
			}

			else
				result = RC_OutOfRange;
		}
	}

	else if (p->MACVENDORID == xdiMACLocalPath)
	{
		/* if this is a local path and path is available */
		if ((p->MACVEND16 >= PI_local) && (p->MACVEND16 <= MAX_PATH_COUNT)
			&& (MIBDATA (stnID, mib->pathBase[p->MACVEND16 - 1].inUse)))
		{
			macPtr->baseData.xdi.localPath = p->MACVEND16;

			/* call routine to store value in NV memory */
			SetMACDataMulti (stnID, p->paramType, macPtr, p);

			/* notify CSP */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.typeInfo = fddiMACVendorAttrib;
			notifyMsg.vendorID = xdiMACLocalPath;
			notifyMsg.data.b16 = macPtr->baseData.xdi.localPath;
			SendMIBMessage (&notifyMsg);

			/* set length in case request had extra data */
			p->paramLen = (3 * SIZE_DEFAULT);
		}

		else
			result = RC_OutOfRange;
	}

	else
	{
		result = RC_NoParameter;
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeXDIPATHAttr (uInt16 stnID, TLVParamType * p, PATHDataPtr pathPtr)
#else
ChangeXDIPATHAttr (stnID, p, pathPtr)
		uInt16           stnID;
		TLVParamType    *p;
		PATHDataPtr      pathPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change an xdiPATH attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			pathPtr	= address of PATH attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT document).
Output:		p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Change value from MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if (p->PATHVENDORID == xdiPATHXDIIndex)
	{
		result = RC_IllegalOperation;
	}

	/*
	 * Currently, XDI has no PATH attributes that can be changed.
	 */

	else
	{
		result = RC_NoParameter;
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeXDIPORTAttr (uInt16 stnID, TLVParamType * p, PORTDataPtr portPtr)
#else
ChangeXDIPORTAttr (stnID, p, portPtr)
		uInt16           stnID;
		TLVParamType    *p;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change an xdiPORT attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			portPtr	= address of PORT attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT document).
Output:		p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
SMTMessage       notifyMsg;				/* message to other processes */

	/*
	 * Set up notification message.
	 */
	notifyMsg.source = MIB_MSG_ID;
	notifyMsg.type = MIB_EVENT_NOTIFY_CHANGE;
	notifyMsg.entity = portPtr->baseData.xdi.xdiIndex;
	notifyMsg.localID = 0;
	notifyMsg.len1 = 0;
	notifyMsg.len2 = 0;
	notifyMsg.stnID = stnID;

	/*
	 * Change value in MIB.
	 * 
	 * A switch() statement cannot be used here because the attribute IDs are
	 * 4-octets. C requires case values to be of type int which may not be
	 * large enough on some platforms.
	 */
	if ((p->PORTVENDORID == xdiPORTXDIIndex) ||
		(p->PORTVENDORID == xdiPORTPathConfiguration) ||
		(p->PORTVENDORID == xdiPORTConnectedIndex) ||
		(p->PORTVENDORID == xdiPORTUpstreamMACNeighbor) ||
		(p->PORTVENDORID == xdiPORTOldUpstreamMACNeighbor) ||
		(p->PORTVENDORID == xdiPORTLerReport) ||
		(p->PORTVENDORID == xdiPORTUndesiredConnectionReport) ||
		(p->PORTVENDORID == xdiPORTPathChangeReport) ||
		(p->PORTVENDORID == xdiPORTEBErrorReport) ||
		(p->PORTVENDORID == xdiPORTLCTFailedReport))
	{
		result = RC_IllegalOperation;
	}

	else if (p->PORTVENDORID == xdiPORTLocalPath)
	{
		/* if this is a local path and path is available */
		if ((p->PORTVEND16 >= PI_local) && (p->PORTVEND16 <= MAX_PATH_COUNT)
			&& (MIBDATA (stnID, mib->pathBase[p->PORTVEND16 - 1].inUse)))
		{
			portPtr->baseData.xdi.localPath = p->PORTVEND16;

			/* call routine to store value in NV memory */
			SetPORTDataMulti (stnID, p->paramType, portPtr, p);

			/* notify CSP */
			notifyMsg.destination = CSP_MSG_ID;
			notifyMsg.typeInfo = fddiPORTVendorAttrib;
			notifyMsg.vendorID = xdiPORTLocalPath;
			notifyMsg.data.b16 = portPtr->baseData.xdi.localPath;
			SendMIBMessage (&notifyMsg);

			/* set length in case request had extra data */
			p->paramLen = (3 * SIZE_DEFAULT);
		}

		else
			result = RC_OutOfRange;
	}

	else
	{
		result = RC_NoParameter;
	}

	return (result);
}
