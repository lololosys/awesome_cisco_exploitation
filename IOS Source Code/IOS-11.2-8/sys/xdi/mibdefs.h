/* $Id: mibdefs.h,v 3.2 1995/11/17 18:10:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibdefs.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibdefs.h,v $
 * Revision 3.2  1995/11/17  18:10:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_mibdefs.h xm7_v3.1.1:cset.0166:12:12 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Management Information Base Header File

		MIB attribute and structure definition.

		File:			xm7_mibdefs.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file contains types and constants required for accessing
		the Management Information Base (MIB).  The MIB is accessed thru
		get, set, and assert funtions.  It contains or has access to all
		SMT attributes within a particular station.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
*
*	The following constants are used to access particular station
*	attributes.  The attributes maintained on a station are defined
*	in section 6.4 of the X3T9.5 FDDI Station Management
*	Standard.  When reading or updating an attribute, the
*	particular attribute being read/updated must be identified.
*	The following constants perform this role.
*
*	This set of attribute identifiers is based on the SMT 7.3
*	preliminary draft proposed standard. Earlier versions of this
*	file contained identifiers for SMT 6.2, as well as an XDI version
*	of a MIB for 5.1. The SMT 5.1 attributes are no longer supported.
*	The 6.2 attributes may be supported through the use of a defined
*	constant. By defining USE_62_DEFS, this file will map the
*	6.2 MIB attribute names to their 7.3 counterparts. This only applies
*	to name changes. Attributes with significant behavior changes,
*	those that have been moved to different objects, and those that
*	have been deleted are not supported.
*********************************************************************/

/*
*	This is the SMT version that is currently running on the machine.
*	SMT 7.0 incremented the operational version from 6.2. This version
*	of the XLNT Manager(tm) only supports SMT 7.0 and later (version 2).
*/
#define SMTOPVERSION 			0002
#define SMTHIVERSION 			0002
#define SMTLOVERSION 			0002

/*
*	This is the SMT 7.x and later MIB version identifier.
*/
#define	SMTMIBVERSIONID				0001

/*
*	This is the XDI OUI for the top three bytes of the attribute
*	ID. It is used for XDI-defined attributes in the vendor
*	specific MIB extensions.
*/
#define	VENDOR_MASK				((uInt32) 0xFFFFFF00)
#define	fddiXDI					((uInt32) ((XDI_OUI_VALUE << 8) & VENDOR_MASK))


/*********************************************************************
	The attribute ID values correspond to the parameter type
	values listed in the SMT documentation. The high byte contains
	the object and the lower byte is the attribute registration ID.

	The naming conviention used here is based on the attribute
	names used in the SMT standard. In most cases, the constant
	exactly matches the name used in the standard. In a few cases,
	minor alterations were made for readability and syntax. The
	dash (-) used by the standard is converted to an underscore
	(_) for C.
*********************************************************************/

/*
*	Object ID constants.
*/
#define	fddiSMT					0x1000
#define	fddiMAC					0x2000
#define	fddiPATH				0x3200
#define	fddiPORT				0x4000
#define	fddiVSO					0xFF00

/*
*	ID Masks.
*/
#define	MIB_OBJECT_MASK			0xFF00
#define	MIB_ID_MASK				0x00FF

/*
*	SMT Attribute Constants
*/
#define	fddiSMTStationIdGrp					(fddiSMT | 10)
#define	fddiSMTStationId					(fddiSMT | 11)
#define	fddiSMTOpVersionId					(fddiSMT | 13)
#define	fddiSMTHiVersionId					(fddiSMT | 14)
#define	fddiSMTLoVersionId					(fddiSMT | 15)
#define	fddiSMTManufacturerData				(fddiSMT | 16)
#define	fddiSMTUserData						(fddiSMT | 17)
#define	fddiSMTMIBVersionId					(fddiSMT | 18)

#define	fddiSMTStationConfigGrp				(fddiSMT | 20)
#define	fddiSMTMac_Ct						(fddiSMT | 21)
#define	fddiSMTNonMaster_Ct					(fddiSMT | 22)
#define	fddiSMTMaster_Ct					(fddiSMT | 23)
#define	fddiSMTAvailablePaths				(fddiSMT | 24)
#define	fddiSMTConfigCapabilities			(fddiSMT | 25)
#define	fddiSMTConfigPolicy					(fddiSMT | 26)
#define	fddiSMTConnectionPolicy				(fddiSMT | 27)
#define	fddiSMTT_Notify						(fddiSMT | 29)
#define	fddiSMTStatRptPolicy				(fddiSMT | 30)
#define	fddiSMTTrace_MaxExpiration			(fddiSMT | 31)
#define	fddiSMTPORTIndexes					(fddiSMT | 32)
#define	fddiSMTMACIndexes					(fddiSMT | 33)
#define	fddiSMTBypassPresent				(fddiSMT | 34)

#define	fddiSMTStatusGrp					(fddiSMT | 40)
#define	fddiSMTECMState						(fddiSMT | 41)
#define	fddiSMTCF_State						(fddiSMT | 42)
/* The Hold policy is not supported in release 7.3.1 and later */
#define	fddiSMTHoldState					(fddiSMT | 43)
#define	fddiSMTRemoteDisconnectFlag			(fddiSMT | 44)
#define	fddiSMTStationStatus				(fddiSMT | 45)
#define	fddiSMTPeerWrapFlag					(fddiSMT | 46)

#define	fddiSMTMIBOperationGrp				(fddiSMT | 50)
#define	fddiSMTTimeStamp					(fddiSMT | 51)
#define	fddiSMTTransitionTimeStamp			(fddiSMT | 52)
#define	fddiSMTSetCount						(fddiSMT | 53)
#define	fddiSMTLastSetStationId				(fddiSMT | 54)

#define	fddiSMTVendorAttrib					(fddiSMT | 255)


/*
*	MAC Attribute Constants
*/
#define	fddiMACCapabilitiesGrp				(fddiMAC | 10)
#define	fddiMACFrameStatusFunctions			(fddiMAC | 11)
#define	fddiMACBridgeFunctions				(fddiMAC | 12)
#define	fddiMACT_MaxCapability				(fddiMAC | 13)
#define	fddiMACTVXCapability				(fddiMAC | 14)

#define	fddiMACConfigGrp					(fddiMAC | 20)
#define	fddiMACAvailablePaths				(fddiMAC | 22)
#define	fddiMACCurrentPath					(fddiMAC | 23)
#define	fddiMACUpstreamNbr					(fddiMAC | 24)
#define	fddiMACDownstreamNbr				(fddiMAC | 25)
#define	fddiMACOldUpstreamNbr				(fddiMAC | 26)
#define	fddiMACOldDownstreamNbr				(fddiMAC | 27)
#define	fddiMACDupAddressTest				(fddiMAC | 29)
#define	fddiMACRequestedPaths				(fddiMAC | 32)
#define	fddiMACDownstreamPORTType			(fddiMAC | 33)
#define	fddiMACIndex						(fddiMAC | 34)

#define	fddiMACAddressGrp					(fddiMAC | 40)
#define	fddiMACSMTAddress					(fddiMAC | 41)
#define	fddiMACLongGrpAddress				(fddiMAC | 44)
#define	fddiMACShortGrpAddress				(fddiMAC | 45)

#define	fddiMACOperationGrp					(fddiMAC | 50)
#define	fddiMACT_Req						(fddiMAC | 51)
#define	fddiMACT_Neg						(fddiMAC | 52)
#define	fddiMACT_Max						(fddiMAC | 53)
#define	fddiMACTvxValue						(fddiMAC | 54)
#define	fddiMACT_Pri0						(fddiMAC | 56)
#define	fddiMACT_Pri1						(fddiMAC | 57)
#define	fddiMACT_Pri2						(fddiMAC | 58)
#define	fddiMACT_Pri3						(fddiMAC | 59)
#define	fddiMACT_Pri4						(fddiMAC | 60)
#define	fddiMACT_Pri5						(fddiMAC | 61)
#define	fddiMACT_Pri6						(fddiMAC | 62)

#define	fddiMACCountersGrp					(fddiMAC | 70)
#define	fddiMACFrame_Ct						(fddiMAC | 71)
#define	fddiMACCopied_Ct					(fddiMAC | 72)
#define	fddiMACTransmit_Ct					(fddiMAC | 73)
#define	fddiMACToken_Ct						(fddiMAC | 74)
#define	fddiMACError_Ct						(fddiMAC | 81)
#define	fddiMACLost_Ct						(fddiMAC | 82)
#define	fddiMACTvxExpired_Ct				(fddiMAC | 83)
#define	fddiMACNotCopied_Ct					(fddiMAC | 84)
#define	fddiMACLate_Ct						(fddiMAC | 85)
#define	fddiMACRingOp_Ct					(fddiMAC | 86)

#define	fddiMACFrameErrorConditionGrp		(fddiMAC | 90)
#define	fddiMACFrameErrorThreshold			(fddiMAC | 95)
#define	fddiMACFrameErrorRatio				(fddiMAC | 96)

#define	fddiMACNotCopiedConditionGrp		(fddiMAC | 100)
#define	fddiMACNotCopiedThreshold			(fddiMAC | 103)
#define	fddiMACNotCopiedRatio				(fddiMAC | 105)

#define	fddiMACStatusGrp					(fddiMAC | 110)
#define	fddiMACRMTState						(fddiMAC | 111)
#define	fddiMACDA_Flag						(fddiMAC | 112)
#define	fddiMACUNDA_Flag					(fddiMAC | 113)
#define	fddiMACFrameErrorFlag				(fddiMAC | 114)
#define	fddiMACNotCopiedFlag				(fddiMAC | 115)
#define	fddiMACMA_UnitdataAvailable			(fddiMAC | 116)
#define	fddiMACHardwarePresent				(fddiMAC | 117)
#define	fddiMACMA_UnitdataEnable			(fddiMAC | 118)

#define	fddiMACVendorAttrib					(fddiMAC | 255)

/*
*	Path Attributes
*/
#define	fddiPATHConfigGrp					(fddiPATH | 10)
#define	fddiPATHIndex						(fddiPATH | 11)
#define	fddiPATHRingLatency					(fddiPATH | 13)
#define	fddiPATHTraceStatus					(fddiPATH | 14)
#define	fddiPATHSbaPayload					(fddiPATH | 15)
#define	fddiPATHSbaOverhead					(fddiPATH | 16)
#define	fddiPATHConfiguration				(fddiPATH | 18)
#define	fddiPATHT_Rmode						(fddiPATH | 19)
#define	fddiPATHSbaAvailable				(fddiPATH | 20)
#define	fddiPATHTVXLowerBound				(fddiPATH | 21)
#define	fddiPATHT_MaxLowerBound				(fddiPATH | 22)
#define	fddiPATHMaxT_Req					(fddiPATH | 23)

#define	fddiPATHVendorAttrib				(fddiPATH | 255)


/*
*	Port Attributes
*/
#define	fddiPORTConfigGrp					(fddiPORT | 10)
#define	fddiPORTMy_Type						(fddiPORT | 12)
#define	fddiPORTNeighborType				(fddiPORT | 13)
#define	fddiPORTConnectionPolicies			(fddiPORT | 14)
#define	fddiPORTMACIndicated				(fddiPORT | 15)
#define	fddiPORTCurrentPath					(fddiPORT | 16)
#define	fddiPORTRequestedPaths				(fddiPORT | 17)
#define	fddiPORTMACPlacement				(fddiPORT | 18)
#define	fddiPORTAvailablePaths				(fddiPORT | 19)
#define	fddiPORTMACLoop_Time				(fddiPORT | 21)
#define	fddiPORTPMDClass					(fddiPORT | 22)
#define	fddiPORTConnectionCapabilities		(fddiPORT | 23)
#define	fddiPORTIndex						(fddiPORT | 29)

#define	fddiPORTOperationGrp				(fddiPORT | 30)
#define	fddiPORTMaint_LS					(fddiPORT | 31)
#define	fddiPORTBS_Flag						(fddiPORT | 33)
#define	fddiPORTPC_LS						(fddiPORT | 34)

#define	fddiPORTErrorCtrsGrp				(fddiPORT | 40)
#define	fddiPORTEBError_Ct					(fddiPORT | 41)
#define	fddiPORTLCTFail_Ct					(fddiPORT | 42)

#define	fddiPORTLerGrp						(fddiPORT | 50)
#define	fddiPORTLer_Estimate				(fddiPORT | 51)
#define	fddiPORTLem_Reject_Ct				(fddiPORT | 52)
#define	fddiPORTLem_Ct						(fddiPORT | 53)
#define	fddiPORTLer_Cutoff					(fddiPORT | 58)
#define	fddiPORTLer_Alarm					(fddiPORT | 59)

#define	fddiPORTStatusGrp					(fddiPORT | 60)
#define	fddiPORTConnectState				(fddiPORT | 61)
#define	fddiPORTPCMState					(fddiPORT | 62)
#define	fddiPORTPC_Withhold					(fddiPORT | 63)
#define	fddiPORTLerFlag						(fddiPORT | 64)
#define	fddiPORTHardwarePresent				(fddiPORT | 65)

#define	fddiPORTVendorAttrib				(fddiPORT | 255)


/*
*	Vendor-specific Object Attributes
*/
#define	fddiVSOVendorAttrib					(fddiVSO | 255)


/*
* Supported vendor-specific object IDs
*
* The object IDs represent the objecttype octet in the ObjectType
* definition. This octet is then used as the high-order octet of the
* object's parameter types. For example, the MASIC ID is 1. The MASIC
* object ID value is the XDI OUI plus the MASIC ID => 00 02 f4 01. All
* MASIC parameter types will have the format 01 xx.
*/
#define	VSO_CALM_MASIC_ID					(0x01)
#define	VSO_calmMASIC						(fddiXDI | VSO_CALM_MASIC_ID)


/*********************************************************************
	MIB Actions
*********************************************************************/

/* SMT Actions */
#define	fddiSMTStationAction				(fddiSMT | 60)
#define	fddiSMTVendorAction					(fddiSMT | 254)

/* MAC Actions */
#define	fddiMACVendorAction					(fddiMAC | 254)

/* PATH Actions */
#define	fddiPATHVendorAction				(fddiPATH | 254)

/* PORT Actions */
#define	fddiPORTAction						(fddiPORT | 70)
#define	fddiPORTVendorAction				(fddiPORT | 254)


/*********************************************************************
	MIB Notifications
*********************************************************************/

/* SMT Notifications */
/* The Hold policy is not supported in release 7.3.1 and later */
#define	fddiSMTHoldCondition				(fddiSMT | 71)
#define	fddiSMTPeerWrapCondition			(fddiSMT | 72)
#define	fddiSMTVendorNotification			(fddiSMT | 252)

/* MAC Notifications */
#define	fddiMACDuplicateAddressCondition	(fddiMAC | 140)
#define	fddiMACFrameErrorCondition			(fddiMAC | 141)
#define	fddiMACNotCopiedCondition			(fddiMAC | 142)
#define	fddiMACNeighborChangeEvent			(fddiMAC | 143)
#define	fddiMACPathChangeEvent				(fddiMAC | 144)
#define	fddiMACVendorNotification			(fddiMAC | 252)

/* PATH Notifications */
#define	fddiPATHVendorNotification			(fddiPATH | 252)

/* PORT Notifications */
#define	fddiPORTLerCondition				(fddiPORT | 80)
#define	fddiPORTUndesiredConnectionAttemptEvent	(fddiPORT | 81)
#define	fddiPORTEBErrorCondition			(fddiPORT | 82)
#define	fddiPORTPathChangeEvent				(fddiPORT | 83)
#define	fddiPORTVendorNotification			(fddiPORT | 252)


/*********************************************************************
	XDI Attributes

	The following constants define the 32-bit attribute IDs used in the MIB.
*********************************************************************/

/* XDI SMT attributes */
#define	xdiSMTStation_Ct					(fddiXDI | 0x01)
#define	xdiSMTSynchronousServiceFlag		(fddiXDI | 0x02)
#define	xdiSMTRMTDupCapabilities			(fddiXDI | 0x03)
#define	xdiSMTGracefulInsCapabilities		(fddiXDI | 0x04)
#define	xdiSMTGracefulMax					(fddiXDI | 0x05)
#define	xdiSMTConnectMax					(fddiXDI | 0x06)
#define	xdiSMTGracefulInsMode				(fddiXDI | 0x07)
#define	xdiSMTMaxMasterCanConnect			(fddiXDI | 0x08)
 /* XLNT Entry support */
#define	xdiSMTGIMACID						(fddiXDI | 0x09)
 /* CALM MASIC support */
#define	calmSMTMASICIndexes					(fddiXDI | 0x0A)
#define	calmSMTMASICSecureRingCapability	(fddiXDI | 0x0B)
#define	calmSMTMASICSecureRingPolicy		(fddiXDI | 0x0c)

#define	xdiSMTPeerWrapReport				(fddiXDI | 0x21)
 /* The Hold policy is not supported in release 7.3.1 and later */
#define xdiSMTHoldReport					(fddiXDI | 0x22)
 /* CALM MASIC support */
#define calmSMTSecureRingPolicyReport		(fddiXDI | 0x23)

/* XDI SMT Events and Conditions */
#define calmSMTSecureRingPolicyEvent		(fddiXDI | 0x41)


/* XDI MAC attributes */
#define	xdiMACRMTDupPolicy					(fddiXDI | 0x01)
#define	xdiMACXDIIndex						(fddiXDI | 0x02)
#define	xdiMACLocalPath						(fddiXDI | 0x03)
#define	xdiMACPathConfiguration				(fddiXDI | 0x04)
#define	xdiMACSM_MA_AvailFlag				(fddiXDI | 0x05)
#define	xdiMACConnectedIndex				(fddiXDI | 0x06)
#define	xdiMACConfigurationStatus			(fddiXDI | 0x07)

#define	xdiMACDuplicateAddressReport		(fddiXDI | 0x21)
#define	xdiMACFrameErrorReport				(fddiXDI | 0x22)
#define	xdiMACNeighborChangeReport			(fddiXDI | 0x23)
#define	xdiMACPathChangeReport				(fddiXDI | 0x24)
#define	xdiMACNotCopiedReport				(fddiXDI | 0x25)
#define	xdiMACConfigurationFailedReport		(fddiXDI | 0x26)
#define	xdiMACSMTAddressChangedReport		(fddiXDI | 0x27)

/* XDI MAC Events and Conditions */
#define	xdiMACConfigurationFailedEvent		(fddiXDI | 0x41)
#define	xdiMACSMTAddressChangedEvent		(fddiXDI | 0x42)

/* XDI PATH Attributes */
#define	xdiPATHXDIIndex						(fddiXDI | 0x01)
#define	calmPATHClaimWinner					(fddiXDI | 0x02)
#define	calmPATHLastBeaconSA				(fddiXDI | 0x03)

/* XDI PORT Attributes */
#define	xdiPORTXDIIndex						(fddiXDI | 0x01)
#define	xdiPORTLocalPath					(fddiXDI | 0x02)
#define	xdiPORTPathConfiguration			(fddiXDI | 0x03)
#define	xdiPORTConnectedIndex				(fddiXDI | 0x04)
#define	xdiPORTUpstreamMACNeighbor			(fddiXDI | 0x05)
#define	xdiPORTOldUpstreamMACNeighbor		(fddiXDI | 0x06)

#define	xdiPORTLerReport					(fddiXDI | 0x21)
#define	xdiPORTUndesiredConnectionReport	(fddiXDI | 0x22)
#define	xdiPORTPathChangeReport				(fddiXDI | 0x23)
#define	xdiPORTEBErrorReport				(fddiXDI | 0x24)
#define	xdiPORTLCTFailedReport				(fddiXDI | 0x25)

/* XDI PORT Events and Conditions */
#define	xdiPORTLCTFailedEvent				(fddiXDI | 0x41)


/*********************************************************************
	CALM MASIC Attributes
*********************************************************************/

/* set upper byte of parameter types to object ID */
#define	calmMASIC						(VSO_CALM_MASIC_ID << 8)

#define	calmMASICConfigurationGrp			(calmMASIC | 10)
#define	calmMASICVersion					(calmMASIC | 11)
#define	calmMASICCapabilities				(calmMASIC | 12)
#define	calmMASICStartResource				(calmMASIC | 13)
#define	calmMASICEndResource				(calmMASIC | 14)
#define	calmMASICUpstreamResource			(calmMASIC | 15)
#define	calmMASICDownstreamResource			(calmMASIC | 16)
#define	calmMASICSliceLatency				(calmMASIC | 17)
#define	calmMASICIndex						(calmMASIC | 18)
#define	calmMASICHardwarePresent			(calmMASIC | 19)

#define	calmMASICOperationsGrp				(calmMASIC | 20)
#define	calmMASICCurrentPath				(calmMASIC | 21)
#define	calmMASICRingState					(calmMASIC | 22)
#define	calmMASICATHT						(calmMASIC | 23)
#define	calmMASICBeaconInit_Ct				(calmMASIC | 24)
#define	calmMASICClaimInit_Ct				(calmMASIC | 25)
#define	calmMASICTokenLost_Ct				(calmMASIC | 26)
#define	calmMASICPUNA						(calmMASIC | 27)

#define	calmMASICMonitoredAddress			(calmMASIC | 31)
#define	calmMASICStatsControl				(calmMASIC | 32)
#define	calmMASICStatsCounters				(calmMASIC | 33)
#define	calmMASICXDIIndex					(calmMASIC | 34)

#define	calmMASICTokenLossReport			(calmMASIC | 40)
#define	calmMASICBeaconInitReport			(calmMASIC | 41)
#define	calmMASICClaimInitReport			(calmMASIC | 42)
#define	calmMASICMACInitReport				(calmMASIC | 43)
#define	calmMASICMimicReport				(calmMASIC | 44)
#define	calmMASICStuckBeaconReport			(calmMASIC | 45)

/* Notifications */
#define	calmMASICTokenLossEvent				(calmMASIC | 50)
#define	calmMASICBeaconInitEvent			(calmMASIC | 51)
#define	calmMASICClaimInitEvent				(calmMASIC | 52)
#define	calmMASICMACInitEvent				(calmMASIC | 53)
#define	calmMASICMimicEvent					(calmMASIC | 54)
#define	calmMASICStuckBeaconEvent			(calmMASIC | 55)


/*********************************************************************
	Vendor-specific Object Macros
*********************************************************************/

#ifdef	XM_USE_VSO

#ifdef	XM_USE_VSO_MASIC

#define	MGetMIBMASICAttr(stnID, bufSize, bufData, bufUsed)\
			GetMASICParams (stnID, bufSize, bufData, bufUsed)
#define	MChangeMIBMASICAttr(stnID, bufData)\
			ChangeMIBMASICAttr (stnID, bufData)

#else							/* XM_USE_VSO_MASIC */

#define	MGetMIBMASICAttr(stnID, bufData)	RC_NoParameter
#define	MChangeMIBMASICAttr(stnID, bufData)	RC_NoParameter

#endif							/* XM_USE_VSO_MASIC */

#ifdef	XM_USE_VSO_OTHER

#ifdef	MULTIPLE_STATION

#define	MGetMIBVSOAttr(stnID, bufSize, bufData, bufUsed)\
			GetVSOValueMulti (stnID, bufSize, bufData, bufUsed)
#define	MChangeMIBVSOAttr(stnID, bufData)\
			ChangeMIBVSOAttrMulti (stnID, bufData)

#else

#define	MGetMIBVSOAttr(stnID, bufSize, bufData, bufUsed)\
			GetVSOValue (bufSize, bufData, bufUsed)
#define	MChangeMIBVSOAttr(stnID, bufData)\
			ChangeMIBVSOAttr (bufData)

#endif							/* MULTIPLE_STATION */

#else							/* XM_USE_VSO_OTHER */

#define	MGetMIBVSOAttr(stnID, bufSize, bufData, bufUsed)\
			RC_NoParameter
#define	MChangeMIBVSOAttr(stnID, bufData)\
			RC_NoParameter

#endif							/* XM_USE_VSO_OTHER */

#endif							/* XM_USE_VSO */


/*********************************************************************
	SMT 6.2 Attribute Names

	To map the 6.2 attribute names to the 7.3 names, define the
	constant USE_62_DEFS. This allows 6.2 attribute name to be
	used along with the 7.3 names.

	NOTE: This process only remaps 6.2 names to 7.3 names. There is
	no mechanism for handling behavior changes bewteen the two
	revisions. Also, this mapping only applies to name changes.
	Deleted or moved attributes are not supported.
*********************************************************************/

#ifdef	USE_62_DEFS

#define	fddiSMTMAC_Ct					fddiSMTMac_Ct
#define	fddiSMTPathsAvailable			fddiSMTAvailablePaths
#define	fddiSMTStatusReporting			fddiSMTStatRptPolicy

#define	fddiMACFrameStatusCapabilities	fddiMACFrameStatusFunctions
#define	fddiMACBridgeFunction			fddiMACBridgeFunctions
#define	fddiMACT_MaxGreatestLowerBound	fddiMACT_MaxCapability
#define	fddiMACTVXGreatestLowerBound	fddiMACTVXCapability
#define	fddiMACPathsAvailable			fddiMACAvailablePaths
#define	fddiMACDup_Addr_Test			fddiMACDupAddressTest
#define	fddiMACPathsRequested			fddiMACRequestedPaths
#define	fddiMACLongGrpAddrs				fddiMACLongGrpAddress
#define	fddiMACShortGrpAddrs			fddiMACShortGrpAddress
#define	fddiMACDa_Flag					fddiMACDA_Flag
#define	fddiMACUnaDa_Flag				fddiMACUNDA_Flag
#define	fddiMACLLCServiceAvailable		fddiMACMA_UnitdataAvailable

#define	fddiPATHClassPATHConfigGrp		fddiPATHConfigGrp
#define	fddiPATHClassPATHRingLatency	fddiPATHRingLatency
#define	fddiPATHClassPATHTraceStatus	fddiPATHTraceStatus
#define	fddiPATHClassPATHSba			fddiPATHSbaPayload
#define	fddiPATHClassPATHSbaOverhead	fddiPATHSbaOverhead
#define	fddiPATHClassPATHConfiguration	fddiPATHConfiguration
#define	fddiPATHClassPATHT_Rmode		fddiPATHT_Rmode

#define	fddiPORTPC_Type					fddiPORTMy_Type
#define	fddiPORTPC_Neighbor				fddiPORTNeighborType
#define	fddiPORTRemoteMACIndicated		fddiPORTMACIndicated
#define	fddiPORTCE_State				fddiPORTCurrentPath
#define	fddiPORTPathsRequested			fddiPORTRequestedPaths
#define	fddiPORTFotxClass				fddiPORTPMDClass
#define	fddiPORTMaintLineState			fddiPORTMaint_LS

/*
*	The following attributes have been renamed and the old names
*	have new behavior definitions.
*
*	fddiMACFrameErrorCondition			fddiMACFrameErrorFlag
*	fddiMACNotCopiedCondition			fddiMACNotCopiedFlag
*	fddiPORTLerCondition				fddiPORTLerFlag
*/

#endif							/* USE_62_DEFS */
