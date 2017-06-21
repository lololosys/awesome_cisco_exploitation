/* $Id: smttypes.h,v 3.2.62.1 1996/09/11 15:52:31 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xdi/smttypes.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: smttypes.h,v $
 * Revision 3.2.62.1  1996/09/11  15:52:31  snyder
 * CSCdi68773:  more things const
 *              536 out of data section
 *              100 out of image
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:19:55  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_smttypes.h xm7_v3.1.1:cset.0166:13:13 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Station Management Header File
		
		SMT Type Definition Header File
		
		File:			xm7_smttypes.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This is the header file used by all SMT modules. This file contains
		type definitions for used by all SMT modules. The type definitions
		supplied here give the SMT system its portability.
			
		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
	Type Definitions
*********************************************************************/

/*
*	Default octet length for attributes of 4 octets or less.
*	Sizes for larger attributes are defined with the type definitions.
*/
#define	SIZE_DEFAULT	4


/*
*	Time Values
*
*	The Manager code uses time values measured either in microseconds
*	or seconds.
*/
typedef uInt32   uTime;

/*
*	Path Configuration
*	Each PORT and MAC has a path configuration entry for each PATH
*	in the node. This information provides a double linked list for
*	generating a path configuration list.
*/
typedef struct PathConfigStruct
{
	uChar            nextIndex;			/* next object's table index */
	uChar            nextType;			/* next object's rsrc type */
	uChar            prevIndex;			/* prev object's table index */
	uChar            prevType;			/* prev object's rsrc type */
} PathConfigType;


/*********************************************************************
*	FDDI Types
*
*	This section provides typedefs for the types defined in the
*	SMT 7.3 MIB (see section 6.4.5.7 of the standard). All of the
*	common definitions have typedefs here except for the SEQUENCE
*	OF types. The SEQUENCE OF types are implemented as arrays of
*	their underlying types. For the SMT, MAC, PATH, and PORT
*	definitions, only the aggregate data types have typedefs. The
*	simple types use an appropriately sized integer. Enumerated and
*	bit string types have their allowed values defined through
*	manifest constants (see fddihdr.h).
*
*	To avoid conflicts with other types, the common types have
*	"FDDI" prepended to the standard type name. SMT types use "SMT",
*	MAC types use "MAC", path types use "PATH" and port types use
*	"PORT".
*
*	For compatibility with earlier Manager releases, the previous
*	data types are also defined here in terms of the 7.3 type.
*********************************************************************/

/*
*	Common Types
*
*	Type not specified here:	Type representation:
*	PathConfig			ResourceTag[]
*/

/* ActionReply */
typedef Flag     FDDIActionReply;

/* AvailablePaths */
typedef uChar    FDDIAvailablePaths;

/* Counter */
typedef uInt32   FDDICounter;

/* CurrentPath */
typedef uInt16   FDDICurrentPath;

/* Flag */
typedef uChar    FDDIFlag;

/* LongAddressType */
typedef uChar    FDDILongAddressType[6];
#define	SIZE_FDDILongAddressType	6

/* PortType */
typedef uInt16   FDDIPortType;

/* PermittedPaths */
typedef uChar    FDDIPermittedPaths;

/* Present */
typedef uInt16   FDDIPresent;

/* ResourceId */
typedef uInt16   FDDIResourceId;

/* ResourceTag */
typedef struct ResourceTagStruct
{
	uChar            pad[2];
	uInt16           resourcetype;
	FDDIResourceId   resourceindex;
	FDDICurrentPath  currentpath;
} FDDIResourceTag;

#define	SIZE_FDDIResourceTag	8

/* Time */
typedef uInt32   FDDITime;

/* TimerTwosComplement */
typedef uInt32   FDDITimer2sComp;

/* TimeStamp */
typedef struct TimeStampStruct
{
	uInt32           hiword;			/* upper 32 bits */
	uInt32           loword;			/* lower 32 bits */
} FDDITimeStamp;

#define	SIZE_FDDITimeStamp	8

/* VendorMIBType */
typedef struct VendorMIBStruct
{
	uChar            manufactureroui[3];
	uChar            specificdata;
} FDDIVendorMIBType;

/* XDI Connected Index */
typedef struct XDIConIndexStruct
{
	uChar            pad;
	uChar            xdiIndex;
	uInt16           resourceType;
} XDIConIndexType;


/*
*	SMT Types
*	Conditions and events are padded for use in TLV encoding.
*/

/* ManufacturerDataType */
typedef struct ManufacturerDataStruct
{
	uChar            manufacturerOUI[3];/* implementor's OUI */
	uChar            manufacturerData[29];	/* implementor's data */
} SMTManufacturerDataType;

#define	SIZE_SMTManufacturerDataType	32

/* PeerWrapConditionDataType */
typedef struct PeerWrapConditionDataStruct
{
	uChar            pad1[3];
	FDDIFlag         conditionstate;
	uChar            pad2[2];
	uInt16           cf_state;
} SMTPeerWrapConditionDataType;

#define	SIZE_SMTPeerWrapConditionDataType	8

/* SetCountType */
typedef struct SetCountStruct
{
	FDDICounter      count;				/* last counter value */
	FDDITimeStamp    settimestamp;		/* last time of set */
} SMTSetCountType;

#define	SIZE_SMTSetCountType	(SIZE_DEFAULT + SIZE_FDDITimeStamp)

/* StationIdType */
typedef struct StationIdStruct
{
	uChar            implementeroctet[2];	/* user octets */
	FDDILongAddressType longaddress;	/* IEEE address */
} SMTStationIdType;

#define	SIZE_SMTStationIdType	8

/* UserDataType */
typedef uChar    SMTUserDataType[32];
#define	SIZE_SMTUserDataType	32


/*
*	MAC Types
*	Conditions and events are padded for use in TLV encoding.
*/

/* DuplicateAddressType */
typedef struct DuplicateAddressStruct
{
	uChar            pad[2];
	uInt16           dupcondition;
	FDDILongAddressType fddilong;
	FDDILongAddressType fddiunalong;
} MACDuplicateAddressType;

/* DuplicateAddressDataType */
typedef struct DuplicateAddressDataStruct
{
	uChar            pad[3];
	FDDIFlag         conditionstate;
	MACDuplicateAddressType duplicateaddress;
} MACDuplicateAddressDataType;

#define	SIZE_MACDuplicateAddressDataType	20

/* FrameErrorDataType */
typedef struct FrameErrorDataStruct
{
	uChar            pad[3];
	FDDIFlag         conditionstate;
	FDDICounter      frame_ct;
	FDDICounter      error_ct;
	FDDICounter      lost_ct;
	uChar            pad2[2];
	uInt16           frameerrorratio;
} MACFrameErrorDataType;

#define	SIZE_MACFrameErrorDataType	20

/* NeighborChangeType */
typedef struct NeighborChangeStruct
{
	uChar            pad[3];
	uChar            nacondition;
	FDDILongAddressType old_una;
	FDDILongAddressType new_una;
	FDDILongAddressType old_dna;
	FDDILongAddressType new_dna;
	FDDICurrentPath  currentpath;
	FDDILongAddressType smtaddress;
} MACNeighborChangeType;

/* NeighborChangeDataType */
typedef struct NeighborChangeDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	MACNeighborChangeType neighborchange;
} MACNeighborChangeDataType;

#define	SIZE_MACNeighborChangeDataType	40

/* NotCopiedDataType */
typedef struct NotCopiedDataStruct
{
	uChar            pad1[3];
	FDDIFlag         conditionstate;
	FDDICounter      notcopied_ct;
	FDDICounter      copied_ct;
	uChar            pad2[2];
	uInt16           notcopiedratio;
} MACNotCopiedDataType;

#define	SIZE_MACNotCopiedDataType	16

typedef struct MACRequestedPathsStruct
{
	uChar            pad[3];
	FDDIPermittedPaths reqPaths;
} MACRequestedPathsType;

/* PathChangeType */
typedef struct MACPathChangeStruct
{
	uChar            pad;
	FDDIAvailablePaths availablepaths;
	FDDICurrentPath  currentpath;
	MACRequestedPathsType requestedpaths;
} MACPathChangeType;

/* PathChangeDataType */
typedef struct MACPathChangeDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	MACPathChangeType pathchange;
} MACPathChangeDataType;

#define	SIZE_MACPathChangeDataType	12

/* ShortAddressType */
typedef uChar    MACShortAddressType[2];

/* xdiMACConfigurationFailedEvent */
typedef struct ConfigurationFailedStruct
{
	FDDIAvailablePaths availablepaths;
	FDDIPermittedPaths requestedpaths;
	uInt16           failreason;
	FDDITimer2sComp  tmaxcapability;
	FDDITimer2sComp  tvxcapability;
} MACConfigurationFailedType;

typedef struct ConfigurationFailedDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	MACConfigurationFailedType configurationfailed;
} MACConfigurationFailedDataType;

#define	SIZE_MACConfigurationFailedDataType	16

/* xdiMACSMTAddressChangedEvent */
typedef struct SMTAddressChangedStruct
{
	FDDIAvailablePaths availablepaths;
	FDDIPermittedPaths requestedpaths;
	FDDICurrentPath  currentpath;
	FDDILongAddressType previoussmtaddress;
	FDDILongAddressType currentsmtaddress;
} MACSMTAddressChangedType;

typedef struct SMTAddressChangedDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	MACSMTAddressChangedType addresschanged;
} MACSMTAddressChangedDataType;

#define	SIZE_MACSMTAddressChangedDataType	20


/*
*	PATH Types
*
*	No aggregate data types defined for PATHs except for the
*	ConfigurationType. Since this is variable length, it is
*	implemented in the code as an array of FDDIResourceTags.
*/


/*
*	PORT Types
*/

/* EBErrorDataType */
typedef struct EBErrorDataStruct
{
	uChar            pad[3];
	FDDIFlag         conditionstate;
	FDDICounter      eberrorcount;
} PORTEBErrorDataType;

#define	SIZE_PORTEBErrorDataType	8

/* xdiPORTLCTFailedEvent */
typedef struct LCTFailedStruct
{
	uChar            pad[2];
	FDDIPortType     mytype;
	FDDIPortType     neighbortype;
	uInt16           pc_withhold;
	uInt32           lctfail_ct;
} PORTLCTFailedType;

typedef struct LCTFailedDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	PORTLCTFailedType lctfail;
} PORTLCTFailedDataType;

#define	SIZE_PORTLCTFailedDataType	16

/* LerType */
typedef struct LerStruct
{
	uChar            pad;
	uChar            ler_cutoff;
	uChar            ler_alarm;
	uChar            ler_estimate;
	FDDICounter      ler_reject_ct;
	FDDICounter      ler_ct;
} PORTLerType;

/* LerDataType */
typedef struct LerDataStruct
{
	uChar            pad[3];
	FDDIFlag         conditionstate;
	PORTLerType      ler;
} PORTLerDataType;

#define	SIZE_PORTLerDataType	16

/* MACIndicatedType */
typedef struct MACIndicatedStruct
{
	FDDIFlag         t_val9;
	FDDIFlag         r_val9;
} PORTMACIndicatedType;

#define	SIZE_PORTMACIndicatedType	2

/* RequestedPaths */
/* NOTE: The PORT requested paths are implemented as an array for easy access */
typedef struct PORTRequestedPathsStruct
{
	uChar            pad;
	FDDIPermittedPaths reqPaths[3];
} PORTRequestedPathsType;

/* PathChangeType */
typedef struct PORTPathChangeStruct
{
	uChar            pad;
	FDDIAvailablePaths availablepaths;
	FDDICurrentPath  currentpath;
	PORTRequestedPathsType requestedpaths;
	FDDIPortType     mytype;
	FDDIPortType     neighbortype;
} PORTPathChangeType;

/* PathChangeDataType */
typedef struct PORTPathChangeDataStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	PORTPathChangeType pathchange;
} PORTPathChangeDataType;

#define	SIZE_PORTPathChangeDataType	16

/* UndesiredConnectionType */
typedef struct UndesiredConnectionStruct
{
	FDDIPortType     porttype;
	uInt16           connectstate;
	FDDIPortType     pc_neighbor;
	uInt16           pc_withhold;
} PORTUndesiredConnectionType;

/* UndesiredConnectionDataType */
typedef struct UndesiredConnectionDataType
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
	PORTUndesiredConnectionType undesiredconnection;
} PORTUndesiredConnectionDataType;

#define	SIZE_PORTUndesiredConnectionDataType	12


/*
*	To process events for the Status Report Protocol, the code
*	must have access to the multipleoccurrence field present
*	in all event formats. The XLNT Manager code assumes that
*	all events formats place the multipleoccurrence field at
*	the BEGINNING of the format like those specified for SMT-
*	defined events.
*/
typedef struct XMMultipleStruct
{
	uChar            pad[3];
	FDDIFlag         multipleoccurrence;
} XMMultipleType;


/**********************************************************************
*
*	The following typedefs provide data types and structures
*	for use throughout the MIB. These types align short data
*	values to 32-bit boundaries. This allows the MIB routines
*	to provide attributes properly aligned for frame transmission.
*	Thus, the frame processing routines do not need to know about
*	individual data sizes when building frame parameters.
*
**********************************************************************/

/*
*	8-bit attribute values.
*/
typedef struct SMT8BitStruct
{
	uChar            pad[3];
	uChar            data;
} TLV8BitType;

/*
*	16-bit attribute values.
*/
typedef struct SMT16BitStruct
{
	uChar            pad[2];
	uInt16           data;
} TLV16BitType;

/*
*	32-bit attribute values.
*	This type allows access to 32-bit attribute using member data
*	as with the 16- and 8-bit attributes.
*/
typedef struct SMT32BitStruct
{
	uInt32           data;
} TLV32BitType;

/*
*	Address/ID attribute values.
*	This type allows access to two-byte data field followed by
*	a 6-byte address.
*/
typedef struct SMTIdStruct
{
	uChar            data[2];
	FDDILongAddressType addr;
} TLVIdType;

#define	SIZE_TLVIdType		8

/*
*	This structure is used for the xdi???PathConfiguration attributes.
*/
typedef struct TLVPathConfigStruct
{
	uInt16           pad;
	uInt16           pathID;
	uChar            nextIndex;			/* next object's table index */
	uChar            nextType;			/* next object's rsrc type */
	uChar            prevIndex;			/* prev object's table index */
	uChar            prevType;			/* prev object's rsrc type */
} TLVPathConfigType;

#define	SIZE_TLVPathConfigType	8


/**********************************************************************
*
*	Typedefs for special attributes. These attributes are either
*	larger than 32 bits or consists of aggregate members.
*
**********************************************************************/

typedef struct TLVHdrStruct
{
	uInt16           paramType;
	uInt16           paramLen;
} TLVHdrType;

#define	SIZE_TLVHdrType		4

/*
*	The uInt32 x[1] arrays are used as placeholders to locate the address
*	of a data value within the TLV encoded information. A uInt32 is used
*	here because all SMT alignment occurs on 32-bit boundaries. This allows
*	correct casting of pointers to the appropriate data structure type.
*/
typedef struct TLVParamStruct
{
	uInt16           paramType;			/* Type and length are */
	uInt16           paramLen;			/* common to all params */
	union
	{
		struct
		{
			TLV16BitType     paramIndex;/* Some attrs require index */
			union
			{
				TLV8BitType      p8;
				TLV16BitType     p16;
				TLV32BitType     p32;
				TLVIdType        pId;
				uChar            pOther[12];
				struct
				{
					TLV32BitType     otherID;
					union
					{
						TLV8BitType      ovend8;
						TLV16BitType     ovend16;
						TLV32BitType     ovend32;
						TLVIdType        ovendId;
						uInt32           ovendValue[1];
					} otherval;
				} otherVendor;
			} pval;
		} otherAttr;
		union
		{
			TLV8BitType      smt8;		/* SMT attrs need no index */
			TLV16BitType     smt16;
			TLV32BitType     smt32;
			TLVIdType        smtId;
			uChar            smtOther[32];
			struct
			{
				TLV32BitType     smtID;
				union
				{
					TLV8BitType      smtvend8;
					TLV16BitType     smtvend16;
					TLV32BitType     smtvend32;
					TLVIdType        smtvendId;
					uInt32           smtvendValue[1];
				} smtval;
			} smtVendor;
		} smtAttr;
		struct
		{
			uInt32           vsoVendorID;	/* vendor's object ID */
			uInt16           vsoType;	/* VSO Attrib ID */
			uInt16           vsoIndex;	/* object index or pad */
			union
			{
				TLV8BitType      vso8;
				TLV16BitType     vso16;
				TLV32BitType     vso32;
				TLVIdType        vsoId;
				uChar            vsoOther[12];
			} vsoval;
		} vsoAttr;
	} paramValue;
} TLVParamType;

typedef struct TLVSetCountStruct
{
	uInt16           paramType;
	uInt16           paramLen;
	SMTSetCountType  paramValue;
} TLVSetCountType;

#define	SIZE_TLVSetCountType	(SIZE_TLVHdrType + SIZE_SMTSetCountType)


/*********************************************************************
*	Macro Definitions
*********************************************************************/

/*
*	Definitions to access parameter values.
*
*	Use the macros to access the appropriate parameter element
*	in the TLVParamType. To use them, put the element reference
*	in front of the macro as follows:
*		x.SMTPARAM8
*		y->MACINDEX
*	where x is of type TLVParamType and y is a pointer to a
*	TLVParamType.
*
*	The xxxGENERAL macro gives a data type on a 32-bit boundary for general
*	data alignment.
*/
#define	SMTPVAL		paramValue.smtAttr
#define	SMTPARAM8	paramValue.smtAttr.smt8.data
#define	SMTPARAM16	paramValue.smtAttr.smt16.data
#define	SMTPARAM32	paramValue.smtAttr.smt32.data
#define	SMTID		paramValue.smtAttr.smtId
#define	SMTADDR		paramValue.smtAttr.smtId.addr
#define	SMTOTHER	paramValue.smtAttr.smtOther
#define	SMTVENDOR	paramValue.smtAttr.smtVendor
#define	SMTVENDORID	paramValue.smtAttr.smtVendor.smtID.data
#define	SMTVENDORDATA	paramValue.smtAttr.smtVendor.smtval.smtvendValue
#define	SMTVEND8	paramValue.smtAttr.smtVendor.smtval.smtvend8.data
#define	SMTVEND16	paramValue.smtAttr.smtVendor.smtval.smtvend16.data
#define	SMTVEND32	paramValue.smtAttr.smtVendor.smtval.smtvend32.data
#define	SMTVENDID	paramValue.smtAttr.smtVendor.smtval.smtvendId
#define	SMTVENDADDR	paramValue.smtAttr.smtVendor.smtval.smtvendId.addr
#define	SMTGENERAL	paramValue.smtAttr.smt32.data

#define	MACPVAL		paramValue.otherAttr.pval
#define	MACINDEX	paramValue.otherAttr.paramIndex.data
#define	MACPARAM8	paramValue.otherAttr.pval.p8.data
#define	MACPARAM16	paramValue.otherAttr.pval.p16.data
#define	MACPARAM32	paramValue.otherAttr.pval.p32.data
#define	MACID		paramValue.otherAttr.pval.pId
#define	MACADDR		paramValue.otherAttr.pval.pId.addr
#define	MACOTHER	paramValue.otherAttr.pval.pOther
#define	MACVENDOR	paramValue.otherAttr.pval.otherVendor
#define	MACVENDORID	paramValue.otherAttr.pval.otherVendor.otherID.data
#define	MACVENDORDATA\
					paramValue.otherAttr.pval.otherVendor.otherval.ovendValue
#define	MACVEND8	paramValue.otherAttr.pval.otherVendor.otherval.ovend8.data
#define	MACVEND16	paramValue.otherAttr.pval.otherVendor.otherval.ovend16.data
#define	MACVEND32	paramValue.otherAttr.pval.otherVendor.otherval.ovend32.data
#define	MACVENDID	paramValue.otherAttr.pval.otherVendor.otherval.ovendId
#define	MACVENDADDR	paramValue.otherAttr.pval.otherVendor.otherval.ovendId.addr
#define	MACGENERAL	paramValue.otherAttr.pval.p32.data

#define	PATHPVAL	paramValue.otherAttr.pval
#define	PATHINDEX	paramValue.otherAttr.paramIndex.data
#define	PATHPARAM8	paramValue.otherAttr.pval.p8.data
#define	PATHPARAM16	paramValue.otherAttr.pval.p16.data
#define	PATHPARAM32	paramValue.otherAttr.pval.p32.data
#define	PATHOTHER	paramValue.otherAttr.pval.pOther
#define	PATHVENDOR	paramValue.otherAttr.pval.otherVendor
#define	PATHVENDORID	paramValue.otherAttr.pval.otherVendor.otherID.data
#define	PATHVENDORDATA\
					paramValue.otherAttr.pval.otherVendor.otherval.ovendValue
#define	PATHVEND8	paramValue.otherAttr.pval.otherVendor.otherval.ovend8.data
#define	PATHVEND16	paramValue.otherAttr.pval.otherVendor.otherval.ovend16.data
#define	PATHVEND32	paramValue.otherAttr.pval.otherVendor.otherval.ovend32.data
#define	PATHVENDID	paramValue.otherAttr.pval.otherVendor.otherval.ovendId
#define	PATHVENDADDR\
					paramValue.otherAttr.pval.otherVendor.otherval.ovendId.addr
#define	PATHGENERAL	paramValue.otherAttr.pval.p32.data

#define	PORTPVAL	paramValue.otherAttr.pval
#define	PORTINDEX	paramValue.otherAttr.paramIndex.data
#define	PORTPARAM8	paramValue.otherAttr.pval.p8.data
#define	PORTPARAM16	paramValue.otherAttr.pval.p16.data
#define	PORTPARAM32	paramValue.otherAttr.pval.p32.data
#define	PORTOTHER	paramValue.otherAttr.pval.pOther
#define	PORTVENDOR	paramValue.otherAttr.pval.otherVendor
#define	PORTVENDORID	paramValue.otherAttr.pval.otherVendor.otherID.data
#define	PORTVENDORDATA\
					paramValue.otherAttr.pval.otherVendor.otherval.ovendValue
#define	PORTVEND8	paramValue.otherAttr.pval.otherVendor.otherval.ovend8.data
#define	PORTVEND16	paramValue.otherAttr.pval.otherVendor.otherval.ovend16.data
#define	PORTVEND32	paramValue.otherAttr.pval.otherVendor.otherval.ovend32.data
#define	PORTVENDID	paramValue.otherAttr.pval.otherVendor.otherval.ovendId
#define	PORTVENDADDR\
					paramValue.otherAttr.pval.otherVendor.otherval.ovendId.addr
#define	PORTGENERAL	paramValue.otherAttr.pval.p32.data

/*
*	Vendor-specific Object Access Macros
*
*	XLNT Designs' vendor-specific object (VSO) attributes follow a similar
*	format to standard FDDI attributes. After the header information comes
*	the VSO index. For XDI objects, the VSO attribute consists of an
*	attribute ID and an optional instance index. If the object has only one
*	instance, then the index field functions as pad space.
*
*	The following macros provide easy access into a VSO using this format.
*
*	These macros are only defined if XM_USE_VSO is defined.
*/
#ifdef	XM_USE_VSO

#define	VSOOBJECT	paramValue.vsoAttr.vsoVendorID
#define	VSOTYPE		paramValue.vsoAttr.vsoType
#define	VSOINDEX	paramValue.vsoAttr.vsoIndex
#define	VSOVAL		paramValue.vsoAttr.vsoval
#define	VSOPARAM8	paramValue.vsoAttr.vsoval.vso8.data
#define	VSOPARAM16	paramValue.vsoAttr.vsoval.vso16.data
#define	VSOPARAM32	paramValue.vsoAttr.vsoval.vso32.data
#define	VSOID		paramValue.vsoAttr.vsoval.vsoId
#define	VSOADDR		paramValue.vsoAttr.vsoval.vsoId.addr
#define	VSOOTHER	paramValue.vsoAttr.vsoval.vsoOther
#define	VSOGENERAL	paramValue.vsoAttr.vsoval.vso32.data

#endif							/* XM_USE_VSO */


/*
*	The MADDPTR(p, type, bytes) macro adds the number of bytes specified
*	to any type of pointer. The resulting pointer type is given by type.
*	It is assumed that the resulting value is properly aligned for
*	a pointer of type "type".
*/
#define	MADDPTR(type, p, bytes)	((type *) ((Void_p) (((uChar *) p) + bytes)))


/*
*	Multi/Single Station Conversion
*
*	These macros provide the correct global data accessing reference
*	depending upon if the code is compiled for multistation or
*	single station operation.
*
*	The multistation version uses the stnID parameter to select the
*	proper data set. The single station version ignore the station
*	information.
*/
#ifdef MULTIPLE_STATION
# define	CSPDATA(stnID, var)	CSPInfo[stnID].var
# define	FBMDATA(stnID, var)	FBMInfo[stnID].var
# define	MIBDATA(stnID, var)	MIBInfo[stnID].var
#else							/* MULTIPLE_STATION */
# define	CSPDATA(stnID, var)	var
# define	FBMDATA(stnID, var)	var
# define	MIBDATA(stnID, var)	var
#endif							/* MULTIPLE_STATION */

/* If MAX_STATION_COUNT not defined, set default here. */
#ifndef	MAX_STATION_COUNT
# define	MAX_STATION_COUNT	1
#endif							/* MAX_STATION_COUNT */

/*
* If this macro has not been defined elsewhere (usually in smtdefs.h)
* by the implementer, then its empty default value is defined here.
*/
#ifndef	MCONST
# define	MCONST const
#endif							/* MCONST */


/*********************************************************************
	SMT 6.2 Type Names

	To map the 6.2 attribute names to the 7.2 names, define the
	constant USE_62_DEFS. This allows 6.2 attribute name to be
	used along with the 7.2 names.
*********************************************************************/

#ifdef	USE_62_DEFS

typedef MACShortAddressType MACAddr16;
typedef FDDILongAddressType MACAddr48;
typedef FDDITimeStamp SMTTimeStamp;
typedef uInt16   PC_LSType;
typedef PC_LSType LineState;

typedef struct SMTSetCountStruct
{
	uInt32           count;				/* last counter value */
	SMTTimeStamp     setTimeStamp;		/* last time of set */
} SetCountType;

/*
* The following types have been redefined:
*	SMTStationIdType
*	SMTManufacturerDataType
*/

#endif							/* USE_62_DEFS */
