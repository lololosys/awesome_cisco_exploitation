/* $Id: fbmframe.h,v 3.2 1995/11/17 18:09:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmframe.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fbmframe.h,v $
 * Revision 3.2  1995/11/17  18:09:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_fbmframe.h xm7_v3.1.1:cset.0166:7:7 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Frame Based Management Header File

		Frame structure definitions.

		File:			xm7_fbmframe.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file defines the formats of SMT frames.  SMT frames are
		used by SMT to communicate with other stations on the ring for
		the purpose of managing the ring.  The primary purpose of SMT
		frames is to provide information to the other stations to be
		used in managing the network.  These frames are not to be
		confused with any LLC frames which may be sent on the network.
		SMT frames do not use any LLC format and are defined by in
		section 4 of the X3T9.5 FDDI Station Management Specification.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*
*	Every frame sent on an FDDI network has a MAC header prepended
*	to it.  This MAC header is typically put on the frame by the
*	interface for LLC frames, however SMT builds its own MAC headers
*	for a number of reasons.  SMT needs to know more information than
*	most of the protocols because it is keeping track of the FDDI ring
*	status and needs to see information in the MAC header to track
*	this. Also SMT uses a different FC field in the MAC header than
*	LLC frames.  The layout of the MAC header is fixed and defined
*	in the FDDI MAC standard.
*
*	The padding at the start of the header aligns all following data
*	to appropriate memory boundaries.
*/
typedef struct MACHdrStruct
{
	uChar            pad[FC_PAD_SIZE];	/* shift FC to last byte in word */
	uChar            FC;				/* Frame control field */
	FDDILongAddressType DA;				/* Destination 48 bit address */
	FDDILongAddressType SA;				/* Source 48 bit address */
} MACHdrType;

#define	SIZE_MACHdrType				(13 + FC_PAD_SIZE)


/*
*	Every SMT frame has an SMT header on it.  The header immediately
*	follows the MAC header of the FDDI frame.  The format of an
*	SMT header is defined below.  This format is fixed and defined in
*	the FDDI Station Management specification.
*/
typedef struct SMTHdrStruct
{
	uChar            Frame_Class;		/* Frame class field */
	uChar            Frame_Type;		/* Frame type field */
	uInt16           Version_ID;		/* Version id field */
	uInt32           Transaction_ID;	/* Transaction id field */
	SMTStationIdType Station_ID;		/* Station id field */
	uChar            pad[2];			/* Padding */
	uInt16           InfoField_Length;	/* Length of the info field */
} SMTHdrType;

#define	SIZE_SMTHdrType				20

/*
*	General structure for SMT frame headers.
*/
typedef struct SMTFrameHdrStruct
{
	MACHdrType       macHdr;
	SMTHdrType       smtHdr;
} SMTFrameHdrType;

#define	SIZE_SMTFrameHdrType		(SIZE_MACHdrType + SIZE_SMTHdrType)

/*
*	Size of SMT frame header.
*/
#define	SMT_FRAME_HDR_SIZE			(SIZE_SMTFrameHdrType - FC_PAD_SIZE)


/*********************************************************************
	FC fields used by SMT.
*********************************************************************/

#define SMT_NSA_FC		0x4F			/* Next Station Addressing */
#define SMT_INFO_FC		0x41			/* SMT info */


/*********************************************************************
	The following constants are used for building SMT headers.
*********************************************************************/

/*
*	Definitions for the frame class field.
*/
#define NIF_CLASS		0x01			/* NIF class */
#define SIFCONFIG_CLASS	0x02			/* SIF configuration class */
#define SIFOP_CLASS		0x03			/* SIF operation class */
#define ECF_CLASS		0x04			/* Echo frame class */
#define RAF_CLASS		0x05			/* Resource alloc. frame class */
#define RDF_CLASS		0x06			/* Request denied frame class */
#define	SRF_CLASS		0x07			/* Status report frame class */
#define	PMFGET_CLASS	0x08			/* PMF get class */
#define	PMFSET_CLASS	0x09			/* PMF set class */
#define ESF_CLASS		0xFF			/* Extended service frame class */

/*
*	Definitions for the frame type field.
*/
#define SMTANNOUNCE		0x01			/* SMT announcement frame type */
#define SMTREQUEST		0x02			/* SMT request frame type */
#define SMTRESPONSE		0x03			/* SMT response frame type */

/*
*	Throughout the info fields of the SMT frames are found parameters
*	which have defined layout conventions.  These conventions
*	provide for a type and length field for each parameter and are
*	defined in the SMT specification.  The following typedefs provide
*	skeletons for these parameters.  These parameters are used by
*	SMT to build info fields for the frames it sends and for parsing
*	frames it received.
*/

/*
*	Each parameter has a fixed header on it which describes the
*	length and type of the parameter.
*/
typedef struct SMTParamHdrStruct
{
	uInt16           type;				/* The type of the parameter */
	uInt16           length;			/* Length of the parameter */
} ParamHdrType;

#define	SIZE_ParamHdrType			4

/*
*	Defines for the type field of the ParamHdrType
*/
#define UNA_PARAM_TYPE				0x0001	/* Upstream neighbor address */
#define STATION_DESC_PARAM_TYPE		0x0002	/* Station Descriptor */
#define STATION_STATE_PARAM_TYPE	0x0003	/* Station State */
#define MSG_TIME_STAMP_PARAM_TYPE	0x0004	/* Time Stamp */
#define STATION_POLICIES_PARAM_TYPE	0x0005	/* Policies used by station */
#define PATH_LATENCY_PARAM_TYPE		0x0006	/* Path latency type */
#define	MAC_NEIGHBORS_PARAM_TYPE	0x0007	/* MAC neighbors */
#define PATH_DESC_PARAM_TYPE		0x0008	/* Path descriptor */
#define MAC_STATUS_PARAM_TYPE		0x0009	/* MAC status */
#define LER_STATUS_PARAM_TYPE		0x000A	/* PORT LER monitor status */
#define FRAME_COUNTERS_PARAM_TYPE	0x000B	/* MAC frame counters */
#define FRAME_NOT_COPIED_PARAM_TYPE	0x000C	/* Frames not copied count */
#define PRIORITY_VALUES_PARAM_TYPE	0x000D	/* MAC priority values */
#define EB_STATUS_PARAM_TYPE		0x000E	/* Elasticity buffer status */
#define MANUFACTURER_PARAM_TYPE		0x000F	/* Manufacturer data field */
#define USER_PARAM_TYPE				0x0010	/* User defined data field */
#define ECHO_PARAM_TYPE				0x0011	/* Echo data */
#define REASON_CODE_PARAM_TYPE		0x0012	/* Reason code */
#define REJECTED_FRAME_PARAM_TYPE	0x0013	/* Rejected frame beginning */
#define SUPPORTED_VERSIONS_PARAM_TYPE	0x0014	/* SMT versions supported */
#define	RESOURCE_TYPE_PARAM_TYPE	0x0015	/* Resource type */
#define	SBA_COMMAND_PARAM_TYPE		0x0016	/* SBA command */
#define	SBA_PAYLOAD_PARAM_TYPE		0x0017	/* SBA payload request */
#define	SBA_OVERHEAD_PARAM_TYPE		0x0018	/* SBA overhead request */
#define	SBA_ALLOC_ADDR_PARAM_TYPE	0x0019	/* SBA allocation address */
#define	SBA_CATEGORY_PARAM_TYPE		0x001A	/* SBA category */
#define	MAX_T_NEG_PARAM_TYPE		0x001B	/* Maximum T_Neg */
#define	MIN_SEGMENT_PARAM_TYPE		0x001C	/* Minimum SBA segment size */
#define	SBA_ALLOCATABLE_PARAM_TYPE	0x001D	/* SBA Allocatable */
#define	AUTHORIZATION_PARAM_TYPE	0x0021	/* PMF authorization field */
#define ESFID_PARAM_TYPE			0xFFFF	/* Extended service field */

/*********************************************************************
	The various parameter types.
*********************************************************************/

/* Upstream neighbor address */
typedef struct UNAParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[2];			/* Padding */
	FDDILongAddressType UNA;			/* actual address of the una */
} UNAParamType;

#define	SIZE_UNAParamType			12

/* Station Descriptor */
typedef struct StationDescParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            NodeClass;			/* Type of station. */
	uChar            Mac_Ct;			/* # of MACs in station */
	uChar            NonMaster_Ct;		/* # of attachments */
	uChar            Master_Ct;			/* # of Master attachments */
} StationDescParamType;

#define	SIZE_StationDescParamType	8

/* Station State */
typedef struct StationStateParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[2];			/* Padding */
	uChar            Topology;			/* Topology of station */
	uChar            DuplAddress;		/* Duplicate address status */
} StationStateParamType;

#define	SIZE_StationStateParamType	8

/* Message Time Stamp */
typedef struct MsgTimeStampParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	FDDITimeStamp    MsgTimeStamp;		/* actual time stamp */
} MsgTimeStampParamType;

#define	SIZE_MsgTimeStampParamType	12

/* Station Policies */
typedef struct StationPoliciesParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           ConfigPolicy;		/* 1 if Hold available */
	uInt16           ConnectionPolicy;	/* connection types rejected */
} StationPoliciesParamType;

#define	SIZE_StationPoliciesParamType	8

/* Path Latency Contribution per Ring */
typedef struct PathLatencyParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           PORTOutIndex1;		/* output PORT on ring 1 */
	uInt16           PathLatencyRing1;	/* Latency in octets */
	uInt16           PORTOutIndex2;		/* output PORT on ring 2 */
	uInt16           PathLatencyRing2;	/* Latency in in octets */
} PathLatencyParamType;

#define	SIZE_PathLatencyParamType		12

/* MAC Neighbors */
typedef struct MACNeighborsParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiMACIndex value */
	uInt16           MAC_Index;			/* MAC index in path descriptor */
	FDDILongAddressType UNA;			/* Upstream neighbor */
	FDDILongAddressType DNA;			/* Downstream neighbor */
} MACNeighborsParamType;

#define	SIZE_MACNeighborsParamType		20

/*
*	Path Descriptor
*	The path descriptor consists of records for each PORT and each
*	MAC in the station.
*/
typedef struct PathPORTRecordStruct
{
	uInt16           MIB_Index;			/* fddiPORTIndex value */
	uChar            PORTType;			/* PHY type A B S M */
	uChar            ConnectState;		/* Connect state */
	uChar            RemotePORTType;	/* Remote PHY type */
	uChar            RemoteMAC;			/* Remote MAC */
	uInt16           ConResourceIndex;	/* output connected resource */
} PathPORTRecordType;

#define	SIZE_PathPORTRecordType			8

typedef struct PathMACRecordStruct
{
	FDDILongAddressType MAC_Addr;		/* MAC's address */
	uInt16           ConResourceIndex;	/* Output of MAC */
} PathMACRecordType;

#define	SIZE_PathMACRecordType			8

typedef struct PathDescParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	union
	{
		PathPORTRecordType port;
		PathMACRecordType mac;
	} record;
} PathDescParamType;

/* MAC Status */
typedef struct MACStatusStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiMACIndex value */
	uInt16           MAC_Index;			/* MAC index in path descriptor */
	uInt32           T_Req;				/* T-Req for this MAC */
	uInt32           T_Neg;				/* T-Neg for this MAC */
	uInt32           T_Max;				/* T-Max for this MAC */
	uInt32           TVXValue;			/* TVX value for this MAC */
	uInt32           T_Min;				/* T-Min for this MAC */
	uInt32           SBA;				/* SBA for this MAC */
	uInt32           Frame_Ct;			/* Frame count for this MAC */
	uInt32           Error_Ct;			/* Error count for this MAC */
	uInt32           Lost_Ct;			/* Lost frame count for this MAC */
} MACStatusParamType;

#define	SIZE_MACStatusParamType			44

/* PORT Link Error Rate Monitoring (LER) Status */
typedef struct LERStatusParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiPORTIndex */
	uInt16           PORT_Index;		/* PORT index in path descriptor */
	uChar            pad;				/* padding */
	uChar            Ler_Cutoff;		/* LER cutoff value */
	uChar            Ler_Alarm;			/* LER alarm value */
	uChar            Ler_Estimate;		/* LER estimate value */
	uInt32           Lem_Reject_Ct;		/* LEM reject count value */
	uInt32           Lem_Ct;			/* LEM count value */
} LERStatusParamType;

#define	SIZE_LERStatusParamType			20

/* MAC Frame Counters */
typedef struct FrameCountersParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiMACIndex value */
	uInt16           MAC_Index;			/* MAC index in path descriptor */
	uInt32           Copied_Ct;			/* Number of frames copied */
	uInt32           Transmit_Ct;		/* Number of frames transmitted */
} FrameCountersParamType;

#define	SIZE_FrameCountersParamType		16

/* MAC Frame Not Copied Count */
typedef struct FrameNotCopiedParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiMACIndex value */
	uInt16           MAC_Index;			/* MAC index in path descriptor */
	uInt32           NotCopied_Ct;		/* Number of frames not copied */
} FrameNotCopiedParamType;

#define	SIZE_FrameNotCopiedParamType	12

/* MAC Priority Values */
typedef struct PriorityValuesParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiMACIndex value */
	uInt16           MAC_Index;			/* MAC index in path descriptor */
	uInt32           T_Pri0;			/* T_priority0 */
	uInt32           T_Pri1;			/* T_priority1 */
	uInt32           T_Pri2;			/* T_priority2 */
	uInt32           T_Pri3;			/* T_priority3 */
	uInt32           T_Pri4;			/* T_priority4 */
	uInt32           T_Pri5;			/* T_priority5 */
	uInt32           T_Pri6;			/* T_priority6 */
} PriorityValuesParamType;

#define	SIZE_PriorityValuesParamType	36

/* PORT EB Status */
typedef struct EBStatusParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt16           MIB_Index;			/* fddiPORTIndex */
	uInt16           PORT_Index;		/* PORT index in path descriptor */
	uInt32           EbErrorCt;			/* Elasticity buffer error count */
} EBStatusParamType;

#define	SIZE_EBStatusParamType			12

/* Manufacturer Field */
typedef struct ManufacturerParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            manuf_OUI[3];		/* manufacturer OUI */
	uChar            manuf_data[29];	/* manufacturer data */
} ManufacturerParamType;

#define	SIZE_ManufacturerParamType		36

/* User Field */
typedef struct UserParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            user_data[32];		/* user data field */
} UserParamType;

#define	SIZE_UserParamType				36

/* Echo Data */
typedef struct EchoParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            echo_data[1];		/* variable length array */
} EchoParamType;

/* Reason Code */
typedef struct ReasonCodeParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           reason;			/* Reason SMT frame rejected */
} ReasonCodeParamType;

#define	SIZE_ReasonCodeParamType		8

/* Rejected Frame Beginning */
#define	REJECTED_PAD_SIZE		3
typedef struct RejectedFrameParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[REJECTED_PAD_SIZE];
	uChar            rejected_frame_beginning[1];	/* first n octets */
} RejectedFrameParamType;

/* SMT Supported Versions */
typedef struct SupportedVersionsParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[2];			/* padding */
	uChar            numVersions;		/* # of versions supported */
	uChar            indexOpVersion;	/* current operational version */
	uInt16           Version[1];		/* variable list of versions */
} SupportedVersionsParamType;

/* Resource Type */
typedef struct ResourceTypeParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           ResourceType;		/* resource type to allocate */
} ResourceTypeParamType;

#define	SIZE_ResourceTypeParamType		8

/* SBA Command */
typedef struct SBACommandParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           SbaCommand;		/* SBA operation */
} SBACommandParamType;

#define	SIZE_SBACommandParamType		8

/* SBA Payload Request */
typedef struct SBAPayloadParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           SbaRequest;		/* Total synch bw requested */
} SBAPayloadParamType;

#define	SIZE_SBAPayloadParamType		8

/* SBA Overhead Request */
typedef struct SBAOverheadParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           SbaOverheadReq;	/* Total synch bw overhead */
} SBAOverheadParamType;

#define	SIZE_SBAOverheadParamType		8

/* SBA Allocation Address */
typedef struct SBAAllocAddrParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[2];
	FDDILongAddressType SbaAllocationAddr;	/* allocator address */
} SBAAllocAddrParamType;

#define	SIZE_SBAAllocAddrParamType		12

/* SBA Category */
typedef struct SBACategoryParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           Category;			/* type of allocation */
} SBACategoryParamType;

#define	SIZE_SBACategoryParamType		8

/* Maximum T_Neg */
typedef struct MaxTnegParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           MaxTneg;			/* longest T_Neg */
} MaxTnegParamType;

#define	SIZE_MaxTnegParamType			8

/* Minimum SBA Segment Size */
typedef struct MinSegmentParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           MinSegment;		/* smallest # bytes per frame */
} MinSegmentParamType;

#define	SIZE_MinSegmentParamType		8

/* SBA Allocatable */
typedef struct SBAAllocatableParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uInt32           SbaAllocatable;	/* total synch bw available */
} SBAAllocatableParamType;

#define	SIZE_SBAAllocatableParamType	8

/* Authorization Parameter */
typedef struct AuthorizationParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            Authorization[1];	/* authorization data */
} AuthorizationParamType;

/* ESF ID */
typedef struct ESFIDParamStruct
{
	uInt16           Type;				/* parameter type */
	uInt16           Length;			/* parameter length (excl. hdr) */
	uChar            pad[2];			/* padding */
	uChar            ESF_ID[6];			/* 48-bit, IEEE-assigned ID */
} ESFIDParamType;

#define	SIZE_ESFIDParamType			12


/*********************************************************************
	Specific Frame Parameter Values

	These values are used only by frame parameters and are not
	generally used in SMT.
*********************************************************************/

/* Connection policies for Station Policies */
#define	FBM_rejectA_A	0x0001
#define	FBM_rejectB_B	0x0002
#define	FBM_rejectS_S	0x0004
#define	FBM_rejectA_M	0x0008
#define	FBM_rejectB_M	0x0010
#define	FBM_rejectA_S	0x0020
#define	FBM_rejectB_S	0x0040
#define	FBM_rejectA_B	0x0080
#define	FBM_rejectB_A	0x0100
#define	FBM_rejectS_A	0x0200
#define	FBM_rejectS_B	0x0400
#define	FBM_rejectS_M	0x0800			/* this and remaining policies */
#define	FBM_rejectM_A	0x1000			/* match fddiSMTConnectionPolicy */
#define	FBM_rejectM_B	0x2000
#define	FBM_rejectM_S	0x4000
#define	FBM_rejectM_M	0x8000


/*
* The frame formats for NIF, SIF and ECF have been fixed with this version
* of SMT. These frames use a constant value as the version ID.
*/
#define	SMT_VERSION_CONSTANT	((uInt16) 1)


/*********************************************************************
	Frame Sizes

	These constants specify the amount of space that must be cleared
	in the frame buffer before building the frame. Constants with
	HDR in the name indicate that the size includes only the fixed
	parameters. These values must have the remaining info field length
	added to them.
*********************************************************************/

#define	SIZE_NIF\
	(SIZE_SMTFrameHdrType\
		+ SIZE_UNAParamType\
		+ SIZE_StationDescParamType\
		+ SIZE_StationStateParamType\
		+ SIZE_TLVHdrType + SIZE_DEFAULT)

#define SIZE_SIFCON_REQ\
	SIZE_SMTFrameHdrType

#define SIZE_SIFCON_RESP\
	(SIZE_SMTFrameHdrType\
		+ SIZE_MsgTimeStampParamType\
		+ SIZE_StationDescParamType\
		+ 12 /* only one supported version */\
		+ SIZE_StationStateParamType\
		+ SIZE_StationPoliciesParamType\
		+ (MAX_MAC_COUNT * SIZE_MACNeighborsParamType)\
		+ (SIZE_TLVHdrType\
			+ (MAX_PORT_COUNT * SIZE_PathPORTRecordType)\
			+ (MAX_MAC_COUNT * SIZE_PathMACRecordType))\
		+ SIZE_TLVSetCountType)

#define SIZE_SIFOP_REQ\
	SIZE_SMTFrameHdrType

#define SIZE_SIFOP_RESP\
	(SIZE_SMTFrameHdrType\
		+ SIZE_MsgTimeStampParamType\
		+ (MAX_MAC_COUNT * SIZE_MACStatusParamType)\
		+ (MAX_PORT_COUNT * SIZE_LERStatusParamType)\
		+ (MAX_MAC_COUNT * SIZE_FrameCountersParamType)\
		+ (MAX_MAC_COUNT * SIZE_FrameNotCopiedParamType)\
		+ (MAX_MAC_COUNT * SIZE_PriorityValuesParamType)\
		+ (MAX_PORT_COUNT * SIZE_EBStatusParamType)\
		+ SIZE_ManufacturerParamType\
		+ SIZE_UserParamType\
		+ SIZE_TLVSetCountType)

#define SIZE_ECF_HDR\
	(SIZE_SMTFrameHdrType\
		+ SIZE_TLVHdrType)

#define	SIZE_RAF_HDR\
	SIZE_SMTFrameHdrType

#define	SIZE_RDF_HDR\
	(SIZE_SMTFrameHdrType\
		+ SIZE_ReasonCodeParamType\
		+ 12 /* only one supported version */\
		+ (SIZE_TLVHdrType + REJECTED_PAD_SIZE))

#define SIZE_ESF_HDR\
	(SIZE_SMTFrameHdrType\
		+ SIZE_ESFIDParamType)

#define SIZE_PMF_GET_REQ_HDR\
	SIZE_SMTFrameHdrType


/*********************************************************************
	Clear Frame Macros

	These macros select the appropriate processing for clearing the
	frame buffer. The MClearFrameBuffer() macro is a front end to
	the integrator's MUserClearFrameBuffer() macro. This latter macro
	allows the integrator to select the appropriate interface routine
	or in-line processing for clearing the buffer. The MClearFrameBuffer()
	pre-processes the values passed to the integrator's macro to insure
	that the length does not exceed the maximum buffer size.

	For backward compatibility, if MUserClearFrameBuffer() is not defined,
	then default processing is done through the ClearFrameBuffer()
	interface routine which always clears the entire buffer.
*********************************************************************/

#ifndef MUserClearFrameBuffer
/*
 * Use ClearFrameBuffer as default processing.
 */
#define MClearFrameBuffer(buffer, len)\
	ClearFrameBuffer (buffer)

#else							/* MUserClearFrameBuffer */

#define MClearFrameBuffer(buffer, len)\
	MUserClearFrameBuffer (buffer,\
		(len < MAX_FRAME_SIZE ? len : MAX_FRAME_SIZE))

#endif							/* MUserClearFrameBuffer */


/*********************************************************************
        SMT 6.2 Constant Names

        To map the 6.2 constant names to the 7.x names, define the
        constant USE_62_DEFS. This allows 6.2 attribute name to be
        used along with the 7.x names.
*********************************************************************/

#ifdef	USE_62_DEFS

#define ECHO_CLASS	ECF_CLASS
#define	PMFCHANGE_CLASS	PMFSET_CLASS

#endif							/* USE_62_DEFS */
