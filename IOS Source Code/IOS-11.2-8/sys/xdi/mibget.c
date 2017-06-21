/* $Id: mibget.c,v 3.2.62.1 1996/03/18 22:51:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/mibget.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibget.c,v $
 * Revision 3.2.62.1  1996/03/18  22:51:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  11:20:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:10:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibget.c xm7_v3.1.1:cset.0172:18:18 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB maintenance module.

		File:			xm7_mibget.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0172		01/13/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file provides the operational routines for retrieving
		attribute data from the MIB. The operational routines are
		divided into object groups: SMT, MAC, PATH, and PORT.

		The attributes in the MIB fall into two categories: always available
		and available when hardware is present. Each category has
		mandatory and optional attributes. The always available attributes
		allow for the management of objects not physically present.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_VSO		include code for vendor-specific object support.

		If the constant listed above is defined, then the
		associated portion of code is included in the compilation.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
#include        "cschdrsif.h"
#include        "xdihdrsif.h"
#include	"../if/if_fddi.h"

#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
	MIB Get Operations.
*********************************************************************/

uInt32
#ifdef __STDC__
GetMIBSMTAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	SMTBaseAttrType * smtPtr)
#else
GetMIBSMTAttr (stnID, bufSize, p, smtPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an fddiSMT attribute.
Parameters:	stnID	= index of station to use.
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
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
uInt16           index;					/* loop counter */
uInt16           count;					/* loop limit */
ResourceTableType *tablePtr;			/* MAC/PORT resource table element */
FDDIResourceId  *idPtr;					/* resource ID pointer */

	/*
	 * Check if buffer is minimum size. Header + data Length is the same for
	 * 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header amount.
	 */
	bufSize -= SIZE_TLVHdrType;

	/*
	 * Length is the same for 8, 16, and 32 bit data types which is the most
	 * common size.
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->paramType)
	{
		/*
		 * Invalid Types
		 */
	case fddiSMTStationAction:
	case fddiSMTVendorAction:
	case fddiSMTPeerWrapCondition:
	case fddiSMTVendorNotification:
		result = RC_IllegalOperation;
		break;

		/*
		 * SMT Attributes
		 */
	case fddiSMTStationId:
		p->paramLen = SIZE_SMTStationIdType;
		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;
		else
			MEMCOPY (p->SMTOTHER, &smtPtr->StationId, SIZE_SMTStationIdType);
		break;

	case fddiSMTOpVersionId:
		p->SMTPARAM16 = smtPtr->OpVersionId;
		break;

	case fddiSMTHiVersionId:
		p->SMTPARAM16 = smtPtr->HiVersionId;
		break;

	case fddiSMTLoVersionId:
		p->SMTPARAM16 = smtPtr->LoVersionId;
		break;

	case fddiSMTUserData:
		p->paramLen = SIZE_SMTUserDataType;
		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;
		else
			MEMCOPY (p->SMTOTHER, smtPtr->UserData, SIZE_SMTUserDataType);
		break;

	case fddiSMTMIBVersionId:
		p->SMTPARAM16 = smtPtr->MIBVersionId;
		break;

	case fddiSMTMac_Ct:
		p->SMTPARAM8 = smtPtr->Mac_Ct;
		break;

	case fddiSMTNonMaster_Ct:
		p->SMTPARAM8 = smtPtr->NonMaster_Ct;
		break;

	case fddiSMTMaster_Ct:
		p->SMTPARAM8 = smtPtr->Master_Ct;
		break;

	case fddiSMTAvailablePaths:
		p->SMTPARAM8 = smtPtr->AvailablePaths;
		break;

	case fddiSMTConfigCapabilities:
		p->SMTPARAM16 = smtPtr->ConfigCapabilities;
		break;

	case fddiSMTConfigPolicy:
		p->SMTPARAM16 = smtPtr->ConfigPolicy;
		break;

	case fddiSMTConnectionPolicy:
		p->SMTPARAM16 = smtPtr->ConnectionPolicy;
		break;

	case fddiSMTT_Notify:
		p->SMTPARAM16 = smtPtr->T_Notify;
		break;

	case fddiSMTStatRptPolicy:
		p->SMTPARAM8 = smtPtr->StatRptPolicy;
		break;

	case fddiSMTTrace_MaxExpiration:
		p->SMTPARAM32 = smtPtr->Trace_MaxExpiration;
		break;

	case fddiSMTPORTIndexes:
	case fddiSMTMACIndexes:
		/* get correct count and table */
		if (p->paramType == fddiSMTPORTIndexes)
		{
			count = smtPtr->xdi.Port_Ct;
			tablePtr = MIBDATA (stnID, mib->portTable);
		}

		else
		{
			count = smtPtr->Mac_Ct;
			tablePtr = MIBDATA (stnID, mib->macTable);
		}

		/* length = count * 2 */
		p->paramLen = count << 1;

		/* make multiple of 4 */
		if (p->paramLen & 0x3)
			p->paramLen += 2;

		/* check length */
		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;

		else
		{
			/* build attribute */
			for (index = 0, idPtr = (FDDIResourceId *) & p->SMTGENERAL;
				index < count;
				index++, tablePtr++)
			{
				*idPtr = tablePtr->resourceID;
				idPtr++;
			}

			/* pad if needed */
			if (count & 0x1)
				*idPtr = 0;
		}
		break;

	case fddiSMTBypassPresent:
		p->SMTPARAM8 = smtPtr->BypassPresent;
		break;

	case fddiSMTECMState:
		p->SMTPARAM16 = smtPtr->ECMState;
		break;

	case fddiSMTCF_State:
		p->SMTPARAM16 = smtPtr->CF_State;
		break;

	case fddiSMTRemoteDisconnectFlag:
		p->SMTPARAM8 = smtPtr->RemoteDisconnectFlag;
		break;

	case fddiSMTStationStatus:
		/*
		 * This attribute is not updated during SMT processing. It can be
		 * determined statically based upon the current CF-State.
		 */
		switch (smtPtr->CF_State)
		{
		case CF_thru:
			smtPtr->StationStatus = Status_thru;
			break;

		case CF_c_wrap_a:
		case CF_c_wrap_b:
		case CF_c_wrap_s:
			smtPtr->StationStatus = Status_concatenated;
			break;

		default:
			smtPtr->StationStatus = Status_separated;
			break;
		}

		p->SMTPARAM16 = smtPtr->StationStatus;
		break;

	case fddiSMTPeerWrapFlag:
		p->SMTPARAM8 = smtPtr->PeerWrapFlag;
		break;

	case fddiSMTTimeStamp:
		GetTimeStampMulti (stnID, &smtPtr->TimeStamp);
		p->paramLen = SIZE_FDDITimeStamp;
		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;
		else
			MEMCOPY (p->SMTOTHER, &smtPtr->TimeStamp, SIZE_FDDITimeStamp);
		break;

	case fddiSMTTransitionTimeStamp:
		p->paramLen = SIZE_FDDITimeStamp;
		if (bufSize < p->paramLen)
			result = RC_BUFFER_ERROR;
		else
			MEMCOPY (p->SMTOTHER, &smtPtr->TransitionTimeStamp,
				SIZE_FDDITimeStamp);
		break;

	case fddiSMTManufacturerData:
		if (smtPtr->xdi.packages & PKG_manufacturerdata)
		{
			p->paramLen = SIZE_SMTManufacturerDataType;
			if (bufSize < p->paramLen)
				result = RC_BUFFER_ERROR;
			else
				MEMCOPY (p->SMTOTHER, &smtPtr->ManufacturerData,
					SIZE_SMTManufacturerDataType);
		}
		else
			result = RC_NoParameter;

		break;

	case fddiSMTSetCount:
		if (smtPtr->xdi.packages & PKG_parametermanagement)
		{
			p->paramLen = SIZE_SMTSetCountType;
			if (bufSize < p->paramLen)
				result = RC_BUFFER_ERROR;
			else
				MEMCOPY (p->SMTOTHER, &smtPtr->SetCount, SIZE_SMTSetCountType);
		}
		else
			result = RC_NoParameter;

		break;

	case fddiSMTLastSetStationId:
		if (smtPtr->xdi.packages & PKG_parametermanagement)
		{
			p->paramLen = SIZE_SMTStationIdType;
			if (bufSize < p->paramLen)
				result = RC_BUFFER_ERROR;
			else
				MEMCOPY (p->SMTOTHER, &smtPtr->LastSetStationId,
					SIZE_SMTStationIdType);
		}
		else
			result = RC_NoParameter;

		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return (result);
}

uInt32
#ifdef __STDC__
GetMIBMACAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p, MACDataPtr macPtr)
#else
GetMIBMACAttr (stnID, bufSize, p, macPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an fddiMAC attribute.
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
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
MACHWAttrType   *macHW;					/* pointer to hardware info */
/*
* Variables used to determine DownstreamPORTType.
*/
uChar            startIndex;			/* starting entity index */
uInt16           startType;				/* starting entity type */
uChar            curIndex;				/* connected entity index */
uInt16           curType;				/* connected entity type */


	/*
	 * Check if buffer is minimum size. Header + index + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header and index length.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT);

	/*
	 * Set length to most common size (index added later).
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->paramType)
	{
		/*
		 * Invalid Types
		 */
	case fddiMACVendorAction:
	case fddiMACDuplicateAddressCondition:
	case fddiMACFrameErrorCondition:
	case fddiMACNeighborChangeEvent:
	case fddiMACPathChangeEvent:
	case fddiMACNotCopiedCondition:
	case fddiMACVendorNotification:
		result = RC_IllegalOperation;
		break;

		/*
		 * Base Attributes
		 */
	case fddiMACRequestedPaths:
		p->MACPARAM8 = macPtr->baseData.RequestedPaths.reqPaths;
		break;

	case fddiMACFrameErrorThreshold:
		p->MACPARAM16 = macPtr->baseData.FrameErrorThreshold;
		break;

	case fddiMACHardwarePresent:
		p->MACPARAM16 = macPtr->baseData.HardwarePresent;
		break;

	case fddiMACMA_UnitdataEnable:
		p->MACPARAM8 = macPtr->baseData.MA_UnitdataEnable;
		break;

	case fddiMACIndex:
		p->MACPARAM16 = macPtr->baseData.Index;
		break;

	case fddiMACNotCopiedThreshold:
		if ((macPtr->baseData.xdi.packages & PKG_framenotcopiedmanagement)
			&& (macPtr->baseData.xdi.packages & PKG_framenotcopied))
		{
			p->MACPARAM16 = macPtr->baseData.NotCopiedThreshold;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiMACVendorAttrib:
		break;

	default:
		/*
		 * The following attributes are only available when the underlying
		 * hardware is available.
		 */
		if (macPtr->baseData.HardwarePresent)
		{
			/* get info from local source */
			macHW = &macPtr->hwData;
			switch (p->paramType)
			{
				/*
				 * This first group of attributes belong to the
				 * machardwarepresent package and must be available if the
				 * hardware is present.
				 */
			case fddiMACFrameStatusFunctions:
				p->MACPARAM16 = macHW->FrameStatusFunctions;
				break;

			case fddiMACT_MaxCapability:
				p->MACPARAM32 = macHW->T_MaxCapability;
				break;

			case fddiMACTVXCapability:
				p->MACPARAM32 = macHW->TVXCapability;
				break;

			case fddiMACAvailablePaths:
				p->MACPARAM8 = macHW->AvailablePaths;
				break;

			case fddiMACCurrentPath:
				p->MACPARAM16 = macHW->CurrentPath;
				break;

			case fddiMACUpstreamNbr:
				p->paramLen = SIZE_TLVIdType;
				if (bufSize < p->paramLen)
					result = RC_BUFFER_ERROR;
				else
					MCopyAddress (p->MACADDR, macHW->UpstreamNbr);
				break;

			case fddiMACDownstreamNbr:
				p->paramLen = SIZE_TLVIdType;
				if (bufSize < p->paramLen)
					result = RC_BUFFER_ERROR;
				else
					MCopyAddress (p->MACADDR, macHW->DownstreamNbr);
				break;

			case fddiMACOldUpstreamNbr:
				p->paramLen = SIZE_TLVIdType;
				if (bufSize < p->paramLen)
					result = RC_BUFFER_ERROR;
				else
					MCopyAddress (p->MACADDR, macHW->OldUpstreamNbr);
				break;

			case fddiMACOldDownstreamNbr:
				p->paramLen = SIZE_TLVIdType;
				if (bufSize < p->paramLen)
					result = RC_BUFFER_ERROR;
				else
					MCopyAddress (p->MACADDR, macHW->OldDownstreamNbr);
				break;

			case fddiMACDupAddressTest:
				p->MACPARAM16 = macHW->DupAddressTest;
				break;

			case fddiMACDownstreamPORTType:
				/*
				 * Find the exit port for this MAC if it exists.
				 */
				startIndex = macPtr->baseData.xdi.xdiIndex;
				startType = RT_mac;
				do
				{
					/* get connected entity */
					GetConnectedResource (stnID, startIndex, startType,
						&curIndex, &curType);

					/* check if port found */
					if (curType == RT_port)
						break;

					/* set start values for next iteration */
					startIndex = curIndex;
					startType = curType;

					/* continue until back at start */
				}
				while (curIndex != macPtr->baseData.xdi.xdiIndex);

				if (curType == RT_port)
					macHW->DownstreamPORTType =
						MIBDATA (stnID,
						mib->portBase[curIndex].baseData.My_Type);
				else
					macHW->DownstreamPORTType = PT_none;

				p->MACPARAM16 = macHW->DownstreamPORTType;
				break;

			case fddiMACSMTAddress:
				p->paramLen = SIZE_TLVIdType;
				if (bufSize < p->paramLen)
					result = RC_BUFFER_ERROR;
				else
					MCopyAddress (p->MACADDR, macHW->SMTAddress);
				break;

				/*
				 * The following attributes come from sources other that the
				 * MIB itself. Usually from the MAC.
				 */
			case fddiMACT_Req:
			case fddiMACT_Neg:
			case fddiMACT_Max:
			case fddiMACTvxValue:
			case fddiMACFrame_Ct:
			case fddiMACCopied_Ct:
			case fddiMACTransmit_Ct:
			case fddiMACError_Ct:
			case fddiMACLost_Ct:
				result = GetMACDataMulti (stnID, p->paramType, macPtr,
					(uChar *) & p->MACGENERAL);
				break;

			case fddiMACFrameErrorRatio:
				p->MACPARAM16 = macHW->FrameErrorRatio;
				break;

			case fddiMACRMTState:
				p->MACPARAM16 = macHW->RMTState;
				break;

			case fddiMACDA_Flag:
				p->MACPARAM8 = macHW->DA_Flag;
				break;

			case fddiMACUNDA_Flag:
				p->MACPARAM8 = macHW->UNDA_Flag;
				break;

			case fddiMACFrameErrorFlag:
				p->MACPARAM8 = macHW->FrameErrorFlag;
				break;

			case fddiMACMA_UnitdataAvailable:
				p->MACPARAM8 = macHW->MA_UnitdataAvailable;
				break;

				/*
				 * framenotcopied package
				 */
			case fddiMACNotCopied_Ct:
				if (macPtr->baseData.xdi.packages & PKG_framenotcopied)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

			case fddiMACNotCopiedRatio:
				if (macPtr->baseData.xdi.packages & PKG_framenotcopied)
				{
					p->MACPARAM16 = macHW->NotCopiedRatio;
				}
				else
					result = RC_NoParameter;

				break;

			case fddiMACNotCopiedFlag:
				if (macPtr->baseData.xdi.packages & PKG_framenotcopied)
				{
					p->MACPARAM8 = macHW->NotCopiedFlag;
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * prioritythreshold package
				 */
			case fddiMACT_Pri0:
			case fddiMACT_Pri1:
			case fddiMACT_Pri2:
			case fddiMACT_Pri3:
			case fddiMACT_Pri4:
			case fddiMACT_Pri5:
			case fddiMACT_Pri6:
				if (macPtr->baseData.xdi.packages & PKG_prioritythreshold)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * tvxexpired package
				 */
			case fddiMACTvxExpired_Ct:
				if (macPtr->baseData.xdi.packages & PKG_tvxexpired)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * latecount package
				 */
			case fddiMACLate_Ct:
				if (macPtr->baseData.xdi.packages & PKG_latecount)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * ringopcount package
				 */
			case fddiMACRingOp_Ct:
				if (macPtr->baseData.xdi.packages & PKG_ringopcount)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;


				/*
				 * longgrpaddress package
				 */
			case fddiMACLongGrpAddress:
				if (macPtr->baseData.xdi.packages & PKG_longgrpaddress)
				{
					result = GetMACAddressesMulti (stnID, macPtr,
						(bufSize + SIZE_TLVHdrType + SIZE_DEFAULT), p);
				}
				else
					result = RC_NoParameter;

				break;


				/*
				 * shortgrpaddress package
				 */
			case fddiMACShortGrpAddress:
				if (macPtr->baseData.xdi.packages & PKG_shortgrpaddress)
				{
					result = GetMACAddressesMulti (stnID, macPtr,
						(bufSize + SIZE_TLVHdrType + SIZE_DEFAULT), p);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * tokencounter package
				 */
			case fddiMACToken_Ct:
				if (macPtr->baseData.xdi.packages & PKG_tokencounter)
				{
					result = GetMACDataMulti (stnID, p->paramType, macPtr,
						(uChar *) & p->MACGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * bridgefunctions package
				 */
			case fddiMACBridgeFunctions:
				if (macPtr->baseData.xdi.packages & PKG_bridgefunctions)
				{
					p->MACPARAM16 = macHW->BridgeFunctions;
				}
				else
					result = RC_NoParameter;

				break;

			default:
				result = RC_NoParameter;
				break;
			}
		}

		else
		{
			/* hardware not present */
			result = RC_NoParameter;
		}
	}

	/*
	 * Adjust parameter length to include entity index.
	 */
	if (result == RC_Success)
		p->paramLen += SIZE_DEFAULT;

	return (result);
}

uInt32
#ifdef __STDC__
GetMIBPATHAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PATHDataPtr pathPtr)
#else
GetMIBPATHAttr (stnID, bufSize, p, pathPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PATHDataPtr      pathPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an fddiPATH attribute.
Parameters:	stnID	= index of station to use.
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
uInt32           result = RC_Success;	/* return value */

	/*
	 * Check if buffer is minimum size. Header + index + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce size of buffer by header and index.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT);

	/*
	 * Set length to most common size (index added later).
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->paramType)
	{
		/*
		 * Invalid Types
		 */
	case fddiPATHVendorAction:
	case fddiPATHVendorNotification:
		result = RC_IllegalOperation;
		break;

		/*
		 * PATH Attributes
		 */
	case fddiPATHIndex:
		p->PATHPARAM16 = pathPtr->baseData.Index;
		break;

	case fddiPATHConfiguration:
		result = BuildPATHConfiguration (stnID,
			pathPtr->baseData.Index, bufSize, (Void_p) p);
		break;

	case fddiPATHTVXLowerBound:
		p->PATHPARAM32 = pathPtr->baseData.TVXLowerBound;
		break;

	case fddiPATHT_MaxLowerBound:
		p->PATHPARAM32 = pathPtr->baseData.T_MaxLowerBound;
		break;

	case fddiPATHMaxT_Req:
		p->PATHPARAM32 = pathPtr->baseData.MaxT_Req;
		break;

	case fddiPATHRingLatency:
		if (pathPtr->baseData.xdi.packages & PKG_ringlatency)
		{
			result = GetPATHDataMulti (stnID, p->paramType, pathPtr,
				(uChar *) & p->PATHPARAM32);
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHTraceStatus:
		if (pathPtr->baseData.xdi.packages & PKG_tracestatus)
		{
			p->PATHPARAM16 = pathPtr->baseData.TraceStatus;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHSbaPayload:
		if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthrequestor)
		{
			p->PATHPARAM32 = pathPtr->baseData.SbaPayload;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHSbaOverhead:
		if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthrequestor)
		{
			p->PATHPARAM32 = pathPtr->baseData.SbaOverhead;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHT_Rmode:
		if (pathPtr->baseData.xdi.packages & PKG_restrictedtoken)
		{
			p->PATHPARAM32 = pathPtr->baseData.T_Rmode;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHSbaAvailable:
		if (pathPtr->baseData.xdi.packages & PKG_synchbandwidthmgt)
		{
			p->PATHPARAM32 = pathPtr->baseData.SbaAvailable;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPATHVendorAttrib:
		break;

	default:
		result = RC_NoParameter;
		break;
	}

	/*
	 * Adjust parameter length to include entity index.
	 */
	if (result == RC_Success)
		p->paramLen += SIZE_DEFAULT;

	return (result);
}

uInt32
#ifdef __STDC__
GetMIBPORTAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PORTDataPtr portPtr)
#else
GetMIBPORTAttr (stnID, bufSize, p, portPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get an fddiPORT attribute.
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
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
PORTHWAttrType  *portHW;				/* pointer to hardware info */

	/*
	 * Check if buffer is minimum size. Header + index + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT + SIZE_DEFAULT))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce size of buffer by header and index.
	 */
	bufSize -= (SIZE_TLVHdrType + SIZE_DEFAULT);

	/*
	 * Set length to most common size.
	 */
	p->paramLen = SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->paramType)
	{
		/*
		 * Invalid Types
		 */
	case fddiPORTAction:
	case fddiPORTVendorAction:
	case fddiPORTLerCondition:
	case fddiPORTUndesiredConnectionAttemptEvent:
	case fddiPORTPathChangeEvent:
	case fddiPORTEBErrorCondition:
	case fddiPORTVendorNotification:
		result = RC_IllegalOperation;
		break;

		/*
		 * Base Attributes
		 */
	case fddiPORTMy_Type:
		/*
		 * This attribute is listed as Mandatory, if present, SMT 7.0. This
		 * implementation makes this attribute Mandatory to allows a
		 * management station to correctly set the fddiPORTRequestedPaths
		 * attribute. This status of fddiPORTMy_Type is expected to change in
		 * future SMT docs.
		 */
		p->PORTPARAM16 = portPtr->baseData.My_Type;
		break;

	case fddiPORTConnectionPolicies:
		p->PORTPARAM8 = portPtr->baseData.ConnectionPolicies;
		break;

	case fddiPORTRequestedPaths:
		p->paramLen = SIZE_DEFAULT;
		MEMCOPY (p->PORTOTHER, &portPtr->baseData.RequestedPaths, SIZE_DEFAULT);
		break;

	case fddiPORTIndex:
		p->PORTPARAM16 = portPtr->baseData.Index;
		break;

	case fddiPORTLer_Cutoff:
		p->PORTPARAM8 = portPtr->baseData.Ler_Cutoff;
		break;

	case fddiPORTLer_Alarm:
		p->PORTPARAM8 = portPtr->baseData.Ler_Alarm;
		break;

	case fddiPORTHardwarePresent:
		p->PORTPARAM16 = portPtr->baseData.HardwarePresent;
		break;

	case fddiPORTMACLoop_Time:
		if (portPtr->baseData.xdi.packages & PKG_maclocalloop)
		{
			p->PORTPARAM32 = portPtr->baseData.MACLoop_Time;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPORTMaint_LS:
		if (portPtr->baseData.xdi.packages & PKG_maint_LSmanagement)
		{
			p->PORTPARAM16 = portPtr->baseData.Maint_LS;
		}
		else
			result = RC_NoParameter;

		break;

	case fddiPORTVendorAttrib:
		break;

	default:
		/*
		 * The following attributes are only available when the underlying
		 * hardware is available.
		 */
		if (portPtr->baseData.HardwarePresent)
		{
			/* get info from local source */
			portHW = &portPtr->hwData;
			switch (p->paramType)
			{
			case fddiPORTNeighborType:
				/**
				 * WRF
				 * Report value based on microcode status
				 * on the High-End platforms.
				 **/
				p->PORTPARAM16 = CSPPCNBR;
				break;

			case fddiPORTMACIndicated:
				MEMCOPY (&p->PORTPARAM16, &portHW->MACIndicated,
					SIZE_PORTMACIndicatedType);
				break;

			case fddiPORTCurrentPath:
				p->PORTPARAM16 = portHW->CurrentPath;
				break;

			case fddiPORTMACPlacement:
				/* calculate from path configuration */
				p->PORTPARAM16 = GetMACPlacement (stnID, portPtr);
				break;

			case fddiPORTAvailablePaths:
				p->PORTPARAM8 = portHW->AvailablePaths;
				break;

			case fddiPORTPMDClass:
				p->PORTPARAM16 = portHW->PMDClass;
				break;

			case fddiPORTConnectionCapabilities:
				p->PORTPARAM8 = portHW->ConnectionCapabilities;
				break;

			case fddiPORTBS_Flag:
				p->PORTPARAM8 = portHW->BS_Flag;
				break;

			case fddiPORTLCTFail_Ct:
				p->PORTPARAM32 = portHW->LCTFail_Ct;
				break;

			case fddiPORTLer_Estimate:
				p->PORTPARAM8 = portHW->Ler_Estimate;
				break;

			case fddiPORTLem_Reject_Ct:
				p->PORTPARAM32 = portHW->Lem_Reject_Ct;
				break;

			case fddiPORTLem_Ct:
				p->PORTPARAM32 = portHW->Lem_Ct;
				break;

			case fddiPORTConnectState:
				/**
				 * WRF
				 * Report value based on microcode status
				 * on the High-End platforms.
				 **/
				p->PORTPARAM16 = CSPPCCONNST;
				break;

			case fddiPORTPCMState:
				/**
				 * WRF
				 * Report value based on microcode status
				 * on the High-End platforms.
				 **/
				p->PORTPARAM16 = CSPPCSTATE;
				break;

			case fddiPORTPC_Withhold:
				/**
				 * WRF
				 * Report value based on microcode status
				 * on the High-End platforms.
				 **/
				p->PORTPARAM16 = CSPPCWITHHOLD;
				break;

			case fddiPORTLerFlag:
				p->PORTPARAM8 = portHW->LerFlag;
				break;

				/*
				 * eberrorcounter package
				 */
			case fddiPORTEBError_Ct:
				if (portPtr->baseData.xdi.packages & PKG_eberrorcounter)
				{
					result = GetPORTDataMulti (stnID, p->paramType, portPtr,
						(uChar *) & p->PORTGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

				/*
				 * maint-LS package
				 */
			case fddiPORTPC_LS:
				if (portPtr->baseData.xdi.packages & PKG_maint_LS)
				{
					result = GetPORTDataMulti (stnID, p->paramType, portPtr,
						(uChar *) & p->PORTGENERAL);
				}
				else
					result = RC_NoParameter;

				break;

			default:
				result = RC_NoParameter;
				break;
			}
		}

		else
		{
			/* hardware is not available */
			result = RC_NoParameter;
		}
	}

	/*
	 * Adjust parameter length to include entity index.
	 */
	if (result == RC_Success)
		p->paramLen += SIZE_DEFAULT;

	return (result);
}


/*********************************************************************
	MIB Access Operations.

	These routines provide a front-end to the individual attribute
	accesses. Through this front-end, functionality for group Gets
	and horizontal Gets.
*********************************************************************/

uInt32
#ifdef __STDC__
GetMIBValue (uInt16 stnID, uInt16 bufSize, Void_p bufData, uInt16 * bufUsed)
#else
GetMIBValue (stnID, bufSize, bufData, bufUsed)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		uInt16          *bufUsed;
#endif
/*********************************************************************
Function:	Get the value for a MIB attribute.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information.
			bufUsed		= pointer to buffer to hold the length
							of all attributes returned.
Input:		None.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			bufUsed		= length, in bytes, of the TLV encoding of
							all attributes returned in bufData.
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
				requested attribute. In this case, as much as
				has been processed is returned.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* pointer to parameter to get */
TLVParamType    *startParam;			/* pointer to initial parameter */
uInt16           startParamLen;			/* initial parameter len */
MACDataType     *macPtr;				/* pointer into MAC base table */
PATHDataType    *pathPtr;				/* pointer into PATH base table */
PORTDataType    *portPtr;				/* pointer into PORT base table */
Int16            index;					/* loop index */
Int16            startindex,
                 endindex;				/* loop limits */
uInt16           len;					/* parameter length */
MIBType         *mibPtr;				/* MIB database */
uInt32           result;				/* return value */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Set startParam pointer and save initial request length.
	 */
	param = (TLVParamType *) bufData;
	startParam = param;
	startParamLen = param->paramLen;
	*bufUsed = 0;

	/*
	 * Default is NoParameter for the 0 index case. If no instances of the
	 * object are in the MIB, result will not get set to anything. This
	 * condition must return NoParameter (see 8.4.2.2.1).
	 */
	result = RC_NoParameter;
	switch (param->paramType & MIB_OBJECT_MASK)
	{
	case fddiSMT:
		/*
		 * If vendor specific attribute, route to correct vendor routine.
		 * Otherwise, process standard attribute.
		 */
		if (param->paramType == fddiSMTVendorAttrib)
		{
			if ((param->SMTVENDORID & VENDOR_MASK) == fddiXDI)
				result = GetXDISMTAttr (stnID, bufSize,
					param, &mibPtr->smtData);
			else
				result = GetVendorSMTAttrMulti (stnID, bufSize,
					param, &mibPtr->smtData);
		}
		else
			result = GetMIBSMTAttr (stnID, bufSize, param,
				&mibPtr->smtData);

		if (result == RC_Success)
			*bufUsed = param->paramLen + SIZE_TLVHdrType;
		break;

	case fddiMAC:
		/*
		 * If looking for all objects, then set loop limits to the entire
		 * table. Otherwise, find the one table entry and set the limits to
		 * include only that one element.
		 */
		if (param->MACINDEX == 0)
		{
			startindex = 0;
			endindex = MAX_MAC_COUNT;
		}

		else
		{
			/* find index of resource ID */
			index = MFindResourceID (param->MACINDEX,
				mibPtr->macTable, mibPtr->smtData.Mac_Ct);
			if (index >= 0)
			{
				/*
				 * Set startindex to table entry, not just index.
				 */
				startindex = mibPtr->macTable[index].index;
				endindex = startindex + 1;
			}

			else
			{
				/* ID not in MIB so force loop to skip it */
				startindex = MAX_MAC_COUNT;
				endindex = 0;
			}
		}

		/* Loop through list of objects to retrieve */
		for (index = startindex, macPtr = &mibPtr->macBase[startindex];
			index < endindex;
			index++, macPtr++)
		{
			if (macPtr->inUse)
			{
				/*
				 * If there is not enough space to write the parameter, then
				 * exit. Use initial parameter length.
				 */
				if (bufSize < (startParamLen + SIZE_TLVHdrType))
				{
					result = RC_BUFFER_ERROR;
					break;
				}

				/*
				 * Copy initial info for this request. It isn't enough to
				 * have just the parameter type and vendor ID because vendor
				 * attributes may have special formats that require
				 * additional information. Copy data from first position in
				 * buffer to current location in buffer.
				 */
				MEMCOPY (param, startParam,
					SIZE_TLVHdrType + startParamLen);

				/*
				 * If vendor specific attribute, route to correct vendor
				 * routine. Otherwise, process standard attribute.
				 */
				if (param->paramType == fddiMACVendorAttrib)
				{
					if ((param->MACVENDORID & VENDOR_MASK) == fddiXDI)
						result = GetXDIMACAttr (stnID, bufSize, param, macPtr);
					else
						result = GetVendorMACAttrMulti (stnID, bufSize,
							param, macPtr);
				}
				else
					result = GetMIBMACAttr (stnID, bufSize, param, macPtr);

				if (result == RC_Success)
				{
					/* set MAC resource ID */
					param->MACINDEX = macPtr->baseData.Index;
					len = param->paramLen + SIZE_TLVHdrType;
					*bufUsed += len;
					bufData = MADDPTR (Void_t, bufData, len);
					bufSize -= len;
					param = MADDPTR (TLVParamType, param, len);
				}

				else if (result != RC_NoParameter)
					break;
			}
		}

		/*
		 * If something was retrieved and last Get returned NoParameter, then
		 * change result to Success.
		 */
		if ((*bufUsed > 0) && (result == RC_NoParameter))
			/* set to Success */
			result = RC_Success;
		break;

	case fddiPATH:
		/*
		 * If looking for all objects, then set loop limits to the entire
		 * table. Otherwise, find the one table entry and set the limits to
		 * include only that one element.
		 */
		if (param->PATHINDEX == 0)
		{
			startindex = 0;
			endindex = MAX_PATH_COUNT;
		}

		else
		{
			/* find index of resource ID */
			/* Path indexes directly reference table index */
			index = param->PATHINDEX - 1;
			if (index >= MAX_PATH_COUNT)
			{
				startindex = MAX_PATH_COUNT;
				endindex = 0;
			}
			else
			{
				startindex = index;
				endindex = index + 1;
			}
		}

		/* Loop through list of objects to retrieve */
		for (index = startindex,
			pathPtr = &mibPtr->pathBase[startindex];
			index < endindex;
			index++, pathPtr++)
		{
			if (pathPtr->inUse)
			{
				/*
				 * If there is not enough space to write the parameter, then
				 * exit. Use initial parameter length
				 */
				if (bufSize < (startParamLen + SIZE_TLVHdrType))
				{
					result = RC_BUFFER_ERROR;
					break;
				}

				/*
				 * Copy initial info for this request. It isn't enough to
				 * have just the parameter type and vendor ID because vendor
				 * attributes may have special formats that require
				 * additional information. Copy data from first position to
				 * current location in buffer.
				 */
				MEMCOPY (param, startParam,
					SIZE_TLVHdrType + startParamLen);

				/*
				 * If vendor specific attribute, route to correct vendor
				 * routine. Otherwise, process standard attribute.
				 */
				if (param->paramType == fddiPATHVendorAttrib)
				{
					if ((param->PATHVENDORID & VENDOR_MASK) == fddiXDI)
						result = GetXDIPATHAttr (stnID, bufSize, param,
							pathPtr);
					else
						result = GetVendorPATHAttrMulti (stnID, bufSize,
							param, pathPtr);
				}
				else
					result = GetMIBPATHAttr (stnID, bufSize,
						param, pathPtr);

				if (result == RC_Success)
				{
					/* set PATH resource ID */
					param->PATHINDEX = pathPtr->baseData.Index;
					len = param->paramLen + SIZE_TLVHdrType;
					*bufUsed += len;
					bufData = MADDPTR (Void_t, bufData, len);
					bufSize -= len;
					param = MADDPTR (TLVParamType, param, len);
				}

				else if (result != RC_NoParameter)
					break;
			}
		}

		/*
		 * If something was retrieved and last Get returned NoParameter, then
		 * change result to Success.
		 */
		if ((*bufUsed > 0) && (result == RC_NoParameter))
			/* set to Success */
			result = RC_Success;
		break;

	case fddiPORT:
		/*
		 * If looking for all objects, then set loop limits to the entire
		 * table. Otherwise, find the one table entry and set the limits to
		 * include only that one element.
		 */
		if (param->PORTINDEX == 0)
		{
			startindex = 0;
			endindex = MAX_PORT_COUNT;
		}

		else
		{
			/* find index of resource ID */
			index = MFindResourceID (param->PORTINDEX, mibPtr->portTable,
				mibPtr->smtData.xdi.Port_Ct);
			if (index >= 0)
			{
				/*
				 * Set startindex to table entry, not just index.
				 */
				startindex = mibPtr->portTable[index].index;
				endindex = startindex + 1;
			}

			else
			{
				/* ID not in MIB so force loop to skip it */
				startindex = MAX_PORT_COUNT;
				endindex = 0;
			}
		}

		/* Loop through list of objects to retrieve */
		for (index = startindex,
			portPtr = &mibPtr->portBase[startindex];
			index < endindex;
			index++, portPtr++)
		{
			if (portPtr->inUse)
			{
				/*
				 * If there is not enough space to write the parameter, then
				 * exit. Use initial parameter length.
				 */
				if (bufSize < (startParamLen + SIZE_TLVHdrType))
				{
					result = RC_BUFFER_ERROR;
					break;
				}

				/*
				 * Copy initial info for this request. It isn't enough to
				 * have just the parameter type and vendor ID because vendor
				 * attributes may have special formats that require
				 * additional information. Copy data from first position to
				 * current location in buffer.
				 */
				MEMCOPY (param, startParam,
					SIZE_TLVHdrType + startParamLen);

				/*
				 * If vendor specific attribute, route to correct vendor
				 * routine. Otherwise, process standard attribute.
				 */
				if (param->paramType == fddiPORTVendorAttrib)
				{
					if ((param->PORTVENDORID & VENDOR_MASK) == fddiXDI)
						result = GetXDIPORTAttr (stnID,
							bufSize, param, portPtr);
					else
						result = GetVendorPORTAttrMulti (stnID, bufSize,
							param, portPtr);
				}
				else
					result = GetMIBPORTAttr (stnID, bufSize, param, portPtr);

				if (result == RC_Success)
				{
					/* set PORT resource ID */
					param->PORTINDEX = portPtr->baseData.Index;
					len = param->paramLen + SIZE_TLVHdrType;
					*bufUsed += len;
					bufData = MADDPTR (Void_t, bufData, len);
					bufSize -= len;
					param = MADDPTR (TLVParamType, param, len);
				}

				else if (result != RC_NoParameter)
					break;
			}
		}

		/*
		 * If something was retrieved and last Get returned NoParameter, then
		 * change result to Success.
		 */
		if ((*bufUsed > 0) && (result == RC_NoParameter))
			/* set to Success */
			result = RC_Success;
		break;

#ifdef	XM_USE_VSO
		/*
		 * If vendor-specific objects are supported, then call the get
		 * routine.
		 */
	case fddiVSO:
		/*
		 * Select appropriate routine based upon vendor OUI and ID.
		 */
		if (param->VSOOBJECT == VSO_calmMASIC)
			/* XLNT Design's MASIC object */
			result = MGetMIBMASICAttr (stnID, bufSize, bufData, bufUsed);

		else
			/* Other vendor-specific objects */
			result = MGetMIBVSOAttr (stnID, bufSize, bufData, bufUsed);

		break;

#endif							/* XM_USE_VSO */

	case 0:
		/*
		 * Object is from the X'00 xx' set
		 */
		result = RC_IllegalOperation;
		break;

	default:
		/*
		 * Requested attribute does not have a valid object type for this
		 * MIB.
		 */
		result = RC_NoParameter;
		break;
	}

	return (result);
}

uInt32
#ifdef __STDC__
GetMIBGroup (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	MCONST uInt16 groupList[], uInt16 * bufUsed)
#else
GetMIBGroup (stnID, bufSize, bufData, groupList, bufUsed)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		MCONST uInt16    groupList[];
		uInt16          *bufUsed;
#endif
/*********************************************************************
Function:	Build a MIB attribute group.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information
							and used to hold attribute group
							returned.
			groupList	= array of attribute IDs in the group.
			bufUsed		= pointer to buffer to hold the length
							of all attributes in the group.
Input:		groupList	= list of uInt16s of each MIB attribute
							in the group in the order they are
							to appear. The list is terminated
							by an attribute ID of 0.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT standard.
			bufUsed		= length, in bytes, of the TLV encoding of
							all attributes returned in bufData.
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
				requested attribute. In this case,
				as much as has been processed is returned.
Modification History:
*********************************************************************/
{
Int16            i;						/* loop counter */
TLVParamType    *p;						/* request attribute pointer */
uInt16           attrLen;				/* length of attribute */
uInt16           rsrcID;				/* entity resource ID */
uInt32           result = RC_NoParameter;	/* return value */
uInt16           attrCount;				/* # of attributes in group */


	/* set initial attribute pointer */
	p = (TLVParamType *) bufData;
	rsrcID = p->MACINDEX;

	/*
	 * Loop through attribute list
	 */
	*bufUsed = 0;
	for (attrCount = 0, i = 0; groupList[i] != 0; i++)
	{
		/*
		 * If there is not enough space to write the parameter type and
		 * length header, then exit.
		 */
		if (bufSize < SIZE_TLVHdrType)
		{
			result = RC_BUFFER_ERROR;
			break;
		}

		/* get attribute */
		p->paramType = groupList[i];
		p->paramLen = 0;
		switch (p->paramType & MIB_OBJECT_MASK)
		{
		case fddiMAC:
		case fddiPATH:
		case fddiPORT:
			/*
			 * If there is not enough space to write the resource index, then
			 * exit.
			 */
			if (bufSize < (SIZE_TLVHdrType + SIZE_DEFAULT))
			{
				result = RC_BUFFER_ERROR;
				break;
			}

			/* non-SMT objects require a resource ID */
			p->MACINDEX = rsrcID;

			break;

		case fddiSMT:
		default:
			/* no special space requirements for these objects */
			break;
		}

		/* get the attribute's value */
		result = GetMIBValue (stnID, bufSize, bufData, &attrLen);

		/* if buffer is full or invalid attribute */
		if ((result == RC_BUFFER_ERROR) || (result == RC_IllegalOperation))
			/* then exit */
			break;

		/* else if get was successful */
		else if (result == RC_Success)
		{
			/* move to the next position in the buffer */
			attrCount++;
			*bufUsed += attrLen;
			bufData = MADDPTR (Void_t, bufData, attrLen);
			bufSize -= attrLen;
			p = (TLVParamType *) bufData;
		}

		/* else return value is OK, just skip this parameter */
		else
		{
			p->paramType = 0;
			p->paramLen = 0;
			result = RC_NoParameter;
		}
	}

	/*
	 * If result is RC_NoParameter, check if group actually has no attributes
	 * or just the last attribute was not available.
	 */
	if ((result == RC_NoParameter) && (attrCount > 0))
		/* group has some attributes in it */
		result = RC_Success;

	return (result);
}
