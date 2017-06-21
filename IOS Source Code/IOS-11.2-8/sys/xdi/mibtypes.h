/* $Id: mibtypes.h,v 3.2 1995/11/17 18:10:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibtypes.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibtypes.h,v $
 * Revision 3.2  1995/11/17  18:10:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_mibtypes.h xm7_v3.1.1:cset.0166:11:11 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Management Information Base Header File

		MIB structure definition.

		File:			xm7_mibtypes.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file contains type definitions used by the Management
		Information Base (MIB).  The MIB is accessed through
		get, set, add and change routines.  It contains or has access
		to all SMT attributes within a particular station.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/**********************************************************************
*
*	The attributes in this version of the XLNT Manager(tm) are
*	grouped by object (SMT, MAC, PATH, and PORT). The order of
*	attributes in the object are based upon the packages described
*	in the SMT 7.x standard.
*
*	The member names are the attribute names with the object portion
*	deleted (e.g., fddiSMTStationId becomes StationId).
**********************************************************************/


/**********************************************************************
*	SMT Attributes
**********************************************************************/

typedef struct XDISMTAttributeStruct
{
	/* attributes */
	uChar            Station_Ct;
	uChar            RMTDupCapabilities;
	uChar            ConnectMax;
	uChar            MaxMasterCanConnect;
	uChar            GracefulMax;
	uChar            GracefulInsCapabilities;
	uInt16           GracefulInsMode;
	uInt16           GracefulInsMACID;

	/* working data */
	uChar            Port_Ct;
	uChar            Masic_Ct;
	uInt16           ABS_present;
	uInt32           packages;
	uInt32           syncBandwidth;
} XDISMTAttrType;

typedef struct SMTBaseAttrStruct
{
	/* fddiSMTBase */
	SMTStationIdType StationId;
	uInt16           OpVersionId;
	uInt16           HiVersionId;
	uInt16           LoVersionId;
	SMTUserDataType  UserData;
	uInt16           MIBVersionId;
	uChar            Mac_Ct;
	uChar            NonMaster_Ct;
	uChar            Master_Ct;
	FDDIAvailablePaths AvailablePaths;
	uInt16           ConfigCapabilities;
	uInt16           ConfigPolicy;
	uInt16           ConnectionPolicy;
	uInt16           T_Notify;
	FDDIFlag         StatRptPolicy;
	FDDITime         Trace_MaxExpiration;
	/* PORTIndexes built when requested */
	/* MACIndexes built when requested */
	FDDIFlag         BypassPresent;
	uInt16           ECMState;
	uInt16           CF_State;
	FDDIFlag         RemoteDisconnectFlag;
	uInt16           StationStatus;
	FDDIFlag         PeerWrapFlag;
	FDDITimeStamp    TimeStamp;
	FDDITimeStamp    TransitionTimeStamp;

	/* manufacturerdata */
	SMTManufacturerDataType ManufacturerData;

	/* parametermanagement */
	SMTSetCountType  SetCount;
	SMTStationIdType LastSetStationId;

	/* XDI attributes */
	XDISMTAttrType   xdi;

#ifdef	USE_VENDOR_SMT_ATTRIBUTES
	/* Vendor attributes */
	VendorSMTAttrType vendor;
#endif							/* USE_VENDOR_SMT_ATTRIBUTES */

} SMTBaseAttrType;


/**********************************************************************
*	MAC Attributes
**********************************************************************/

/* XDI Attributes */
typedef struct XDIMACAttributeStruct
{
	/* attributes */
	uInt16           RMTDupPolicy;
	uChar            xdiIndex;
	PathConfigType   configList[MAX_PATH_COUNT];
	uInt16           localPath;

	/* working data */
	uInt32           packages;
} XDIMACAttrType;

/* Working hardware values per MAC */
typedef struct XDIMACHWStruct
{
	/* attributes */
	uInt16           ConfigurationStatus;
	FDDIFlag         SM_MA_AvailFlag;

	/* working data */
	uChar            neighborFlags;
	uInt32           lastLost;
	uInt32           lastError;
	uInt32           lastFrame;
	uInt32           lastNotCopied;
	uInt32           lastCopied;
	FDDILongAddressType oldSMTAddress;
} XDIMACHWType;

typedef struct MACBaseAttrStruct
{
	/* fddiMACBase */
	FDDIResourceId   Index;
	MACRequestedPathsType RequestedPaths;
	uInt16           FrameErrorThreshold;
	FDDIPresent      HardwarePresent;
	FDDIFlag         MA_UnitdataEnable;

	/* framenotcopiedmanagement */
	uInt16           NotCopiedThreshold;

	/* XDI attributes */
	XDIMACAttrType   xdi;

#ifdef	USE_VENDOR_MAC_ATTRIBUTES
	/* Vendor attributes */
	VendorMACAttrType vendor;
#endif							/* USE_VENDOR_MAC_ATTRIBUTES */

} MACBaseAttrType;

typedef struct MACHWAttrStruct
{
	/* machardwarepresent */
	uInt16           FrameStatusFunctions;
	FDDITimer2sComp  T_MaxCapability;
	FDDITimer2sComp  TVXCapability;
	FDDIAvailablePaths AvailablePaths;
	FDDICurrentPath  CurrentPath;
	FDDILongAddressType UpstreamNbr;
	FDDILongAddressType DownstreamNbr;
	FDDILongAddressType OldUpstreamNbr;
	FDDILongAddressType OldDownstreamNbr;
	uInt16           DupAddressTest;
	FDDIPortType     DownstreamPORTType;
	FDDILongAddressType SMTAddress;
	FDDITimer2sComp  T_Req;
	FDDITimer2sComp  T_Neg;
	FDDITimer2sComp  T_Max;
	FDDITimer2sComp  TvxValue;
	FDDICounter      Frame_Ct;
	FDDICounter      Copied_Ct;
	FDDICounter      Transmit_Ct;
	FDDICounter      Error_Ct;
	FDDICounter      Lost_Ct;
	uInt16           FrameErrorRatio;
	uInt16           RMTState;
	FDDIFlag         DA_Flag;
	FDDIFlag         UNDA_Flag;
	FDDIFlag         FrameErrorFlag;
	FDDIFlag         MA_UnitdataAvailable;

	/* framenotcopied */
	FDDICounter      NotCopied_Ct;
	uInt16           NotCopiedRatio;
	FDDIFlag         NotCopiedFlag;

	/* priority threshold */
	FDDITimer2sComp  T_Pri0;
	FDDITimer2sComp  T_Pri1;
	FDDITimer2sComp  T_Pri2;
	FDDITimer2sComp  T_Pri3;
	FDDITimer2sComp  T_Pri4;
	FDDITimer2sComp  T_Pri5;
	FDDITimer2sComp  T_Pri6;

	/* tvxexpired */
	FDDICounter      TvxExpired_Ct;

	/* latecount */
	FDDICounter      Late_Ct;

	/* ringopcount */
	FDDICounter      RingOp_Ct;

	/* longgrpaddress */
	uChar           *LongGrpAddress;

	/* shortgrpaddress */
	uChar           *ShortGrpAddress;

	/* tokencounter */
	FDDICounter      Token_Ct;

	/* bridgefunctions */
	uInt16           BridgeFunctions;

	/* XDI values */
	XDIMACHWType     xdi;

#ifdef	USE_VENDOR_MACHW_ATTRIBUTES
	/* Vendor attributes */
	VendorMACHWType  vendor;
#endif							/* USE_VENDOR_MACHW_ATTRIBUTES */

} MACHWAttrType;


/**********************************************************************
*	PATH Attrbiutes
**********************************************************************/

/* XDI Attributes */
typedef struct XDIPATHAttributeStruct
{
	/* attributes */
	uChar            xdiIndex;
	uChar            xdiRunDiagnostics;

	/* working data */
	uInt32           packages;
} XDIPATHAttrType;

typedef struct PATHBaseAttrStruct
{
	/* fddiPATHBase */
	uInt16           Index;
	FDDITimer2sComp  TVXLowerBound;
	FDDITimer2sComp  T_MaxLowerBound;
	FDDITimer2sComp  MaxT_Req;

	/* synchbandwidthrequestor */
	uInt32           SbaPayload;
	uInt32           SbaOverhead;

	/* synchbandwidthmgt */
	uInt32           SbaAvailable;

	/* ringlatency */
	FDDITime         RingLatency;

	/* tracestatus */
	uInt16           TraceStatus;

	/* restrictedtoken */
	FDDITime         T_Rmode;

	/* XDI attributes */
	XDIPATHAttrType  xdi;

#ifdef	USE_VENDOR_PATH_ATTRIBUTES
	/* Vendor attributes */
	VendorPATHAttrType vendor;
#endif							/* USE_VENDOR_PATH_ATTRIBUTES */

} PATHBaseAttrType;


/**********************************************************************
*	PORT Attributes
**********************************************************************/

/* XDI Attributes */
typedef struct XDIPORTAttributeStruct
{
	/* attributes */
	uChar            xdiIndex;
	uInt16           localPath;
	PathConfigType   configList[MAX_PATH_COUNT];

	/* working data */
	uInt32           packages;
} XDIPORTAttrType;

/* Working hardware values per PORT */
typedef struct XDIPORTHWStruct
{
	/* attributes */
	FDDIFlag         EBErrorFlag;
	FDDILongAddressType UpstreamMACNeighbor;
	FDDILongAddressType OldUpstreamMACNeighbor;

	/* working data */
	uInt32           lastEB;
} XDIPORTHWType;

typedef struct PORTBaseAttrStruct
{
	/* fddiPORTBase */
	FDDIPortType     My_Type;
	uChar            ConnectionPolicies;
	PORTRequestedPathsType RequestedPaths;
	FDDIResourceId   Index;
	uChar            Ler_Cutoff;
	uChar            Ler_Alarm;
	FDDIPresent      HardwarePresent;

	/* maclocalloop */
	FDDITime         MACLoop_Time;

	/* maint-LSmanagement */
	uInt16           Maint_LS;

	/* XDI attributes */
	XDIPORTAttrType  xdi;

#ifdef	USE_VENDOR_PORT_ATTRIBUTES
	/* Vendor attributes */
	VendorPORTAttrType vendor;
#endif							/* USE_VENDOR_PORT_ATTRIBUTES */

} PORTBaseAttrType;

typedef struct PORTHWAttrStruct
{
	/* porthardwarepresent */
	FDDIPortType     NeighborType;
	PORTMACIndicatedType MACIndicated;
	FDDICurrentPath  CurrentPath;
	FDDIResourceId   MACPlacement;
	FDDIAvailablePaths AvailablePaths;
	uInt16           PMDClass;
	uChar            ConnectionCapabilities;
	FDDIFlag         BS_Flag;
	FDDICounter      LCTFail_Ct;
	uChar            Ler_Estimate;
	FDDICounter      Lem_Reject_Ct;
	FDDICounter      Lem_Ct;
	uInt16           ConnectState;
	uInt16           PCMState;
	uInt16           PC_Withhold;
	FDDIFlag         LerFlag;

	/* eberrorcounter */
	FDDICounter      EBError_Ct;

	/* maint-LS */
	uInt16           PC_LS;

	/* XDI values */
	XDIPORTHWType    xdi;

#ifdef	USE_VENDOR_PORTHW_ATTRIBUTES
	/* Vendor attributes */
	VendorPORTHWType vendor;
#endif							/* USE_VENDOR_PORTHW_ATTRIBUTES */

} PORTHWAttrType;


/**********************************************************************
*	MASIC Attributes
*
*	This object is only included in the MIB only if
*	XM_USE_VSO_MASIC is defined.
**********************************************************************/

#ifdef	XM_USE_VSO_MASIC

typedef struct MASICBaseAttrStruct
{
	/* fddiMASICBase */
	FDDIResourceId   Index;
	FDDIPresent      HardwarePresent;
	uChar            xdiIndex;
} MASICBaseAttrType;

typedef struct MASICHWAttrStruct
{
	FDDICurrentPath  CurrentPath;
	FDDIAvailablePaths AvailablePaths;
} MASICHWAttrType;

typedef struct MASICDataStruct
{
	Flag             inUse;
	MASICBaseAttrType baseData;
	MASICHWAttrType  hwData;
} MASICDataType , *MASICDataPtr;

#else							/* XM_USE_VSO_MASIC */

 /* Define data types for function prototypes */
typedef Void_t   MASICBaseAttrType;
typedef Void_t   MASICHWAttrType;
typedef Void_t   MASICDataType;
typedef Void_p   MASICDataPtr;

#endif							/* XM_USE_VSO_MASIC */


/**********************************************************************
*
*	MIB Structure Definition
*
**********************************************************************/

/*
* MIB Base Data Structures
*/
typedef struct MACDataStruct
{
	Flag             inUse;
	MACBaseAttrType  baseData;
	MACHWAttrType    hwData;
} MACDataType   , *MACDataPtr;

typedef struct PATHDataStruct
{
	Flag             inUse;
	PATHBaseAttrType baseData;
} PATHDataType  , *PATHDataPtr;

typedef struct PORTDataStruct
{
	Flag             inUse;
	PORTBaseAttrType baseData;
	PORTHWAttrType   hwData;
} PORTDataType  , *PORTDataPtr;

/*
* Resource Tables
*/
typedef struct ResourceTableStruct
{
	uInt16           resourceID;		/* Object's ID */
	uInt16           index;				/* table array index */
} ResourceTableType;


/*
* MIB Structure
*/
typedef struct MIBStruct
{
	SMTBaseAttrType  smtData;

	ResourceTableType macTable[MAX_MAC_COUNT];
	MACDataType      macBase[MAX_MAC_COUNT];

	/* PATH resource IDs are used as indes so no table is needed */
	PATHDataType     pathBase[MAX_PATH_COUNT];

	ResourceTableType portTable[MAX_PORT_COUNT];
	PORTDataType     portBase[MAX_PORT_COUNT];

#ifdef	XM_USE_VSO

#ifdef	XM_USE_VSO_MASIC

	ResourceTableType masicTable[MAX_MASIC_COUNT];
	MASICDataType    masicBase[MAX_MASIC_COUNT];

#endif							/* XM_USE_VSO_MASIC */

#ifdef	XM_USE_VSO_OTHER
	Void_p           vsoData;
#endif							/* XM_USE_VSO_OTHER */

#endif							/* XM_USE_VSO */

} MIBType;


/*********************************************************************
	Global structure used for multiple station implementations.
*********************************************************************/

#ifdef	MULTIPLE_STATION
/*
*	Structure used for multistation implementations.
*/
struct MIBAllInfo
{
	MIBType         *mib;
};

#endif							/* MULTIPLE_STATION */
