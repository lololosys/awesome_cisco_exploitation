/* $Id: fbmhdr.h,v 3.2 1995/11/17 18:09:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmhdr.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fbmhdr.h,v $
 * Revision 3.2  1995/11/17  18:09:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_fbmhdr.h xm7_v3.1.1:cset.0173:9:9 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Frame Services Process Header File
		
		FBM Header File
		
		File:			xm7_fbmhdr.h
		Created:		09/05/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This is the header file used by all FBM modules. This file contains
		definitions for data structures and defined values.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
	Data Structures
*********************************************************************/

/*
*	The following data structures define the global data used
*	throughout FBM. The data is divided to correspond to the station,
*	each MAC, and each PORT.
*/

/*
*	This data structure defines the set of data associated with the
*	station.
*/
typedef struct FBMStationInfo
{
	/* MIB SMT values */
	SMTStationIdType stationID;			/* fddiSMTStationId */
	uChar            macCount;			/* fddiSMTMac_Ct */
	uChar            attachCount;		/* fddiSMTNonMaster_Ct */
	uChar            masterCount;		/* fddiSMTMaster_Ct */
	uInt16           opVersionID;		/* fddiSMTOpVersionId */
	uInt16           hiVersionID;		/* fddiSMTHiVersionId */
	uInt16           loVersionID;		/* fddiSMTLoVersionId */
	uInt32           T_Notify;			/* fddiSMTT_Notify */

	/* SMT values from other attributes */
	uChar            useAuthorization;	/* SET to use authorization */

	/* Locally determined values */
	uInt16           macPresent;		/* # MACs hw present */
	uInt16           masterPresent;		/* # M ports hw present */
	uInt16           attachPresent;		/* # attach ports hw present */
	Flag             FBMInitialized;	/* SET after FBM initialized */
	uInt32           transactionID;		/* frame transaction ID */
	Int16            A_Index;			/* array index of A port */
	Int16            B_Index;			/* array index of B port */
	Int16            S_Index;			/* array index of S port */
	uInt32           monitorMode;		/* vector for frame monitoring */
} FBMStationInfoType;

/*
*	This data structure defines the set of data associated with the
*	MACs.
*/
typedef struct FBMMACInfo
{
	/* MIB MAC values */
	FDDILongAddressType SMTAddress;		/* MAC's SMT address */
	uInt16           hwPresent;			/* fddiMACHardwarePresent */
	uInt16           Index;				/* fddiMACIndex */
	FDDIFlag         SM_MA_Avail;		/* xdiMACSM_MA_Avail */

	/* MAC values from other attributes */
	uInt16           ID;				/* array index */
	uInt16           resourceID;		/* MAC's ConResourceIndex */

	/* State information */
	FDDILongAddressType UNA;			/* MAC's upstream neighbor */
	FDDILongAddressType DNA;			/* MAC's downstream neighbor */
	uChar            myDuplicate;		/* my MAC detected a dupl */
	uChar            myUNADuplicate;	/* my UNA detected a dupl */
	uInt16           Dup_Addr_Test;		/* dupl address test result */
	Int32            TNN;				/* NN protocol timer */
	Int32            TVU;				/* NN protocol timer */
	Int32            TVD;				/* NN protocol timer */
	uInt32           NNTransactionID;	/* NN protocol value */
} FBMMACInfoType;

/* PATH index values */
typedef struct FBMPATHInfo
{
	uInt16           Index;				/* fddiPATHIndex */
	uInt16           ID;				/* array index */
} FBMPATHInfoType;

/* PORT index values */
typedef struct FBMPORTInfo
{
	uInt16           Index;				/* fddiPORTIndex */
	uInt16           ID;				/* array index */
	uInt16           portType;			/* port's type */
	uInt16           hwPresent;			/* port physically present */
	uInt16           resourceID;		/* port's ConResourceIndex */
} FBMPORTInfoType;

/*
* This data structure contains information needed to support the
* Status Report Protocol.
*/
#define	MAX_SMT_EC_COUNT\
			(FDDI_SMT_EC_COUNT + XDI_SMT_EC_COUNT + VENDOR_SMT_EC_COUNT)

#define	MAX_MAC_EC_COUNT\
			(FDDI_MAC_EC_COUNT + XDI_MAC_EC_COUNT + VENDOR_MAC_EC_COUNT)

#define	MAX_PATH_EC_COUNT\
			(FDDI_PATH_EC_COUNT + XDI_PATH_EC_COUNT + VENDOR_PATH_EC_COUNT)

#define	MAX_PORT_EC_COUNT\
			(FDDI_PORT_EC_COUNT + XDI_PORT_EC_COUNT + VENDOR_PORT_EC_COUNT)

#define	EC_LIST_SIZE\
			(MAX_SMT_EC_COUNT +\
			(MAX_MAC_EC_COUNT * MAX_MAC_COUNT) +\
			(MAX_PATH_EC_COUNT * MAX_PATH_COUNT) +\
			(MAX_PORT_EC_COUNT * MAX_PORT_COUNT))

typedef struct ECEntryStruct
{
	uInt16           ecID;				/* 16-bit local identifier */
	uInt16           entity;			/* object index (0 for SMT) */
	uInt32           vendorID;			/* 32-bit vendor-specific event ID */
	uChar            ecType;			/* EC_TYPE_EVENT or EC_TYPE_CONDITION */
	uChar            ecFlags;			/* status flags */
	struct ECEntryStruct
	                *next;				/* next element in list */
} ECEntryType;

typedef struct FBMSRPInfo
{
	uInt16           SRThreshold;		/* time till next frame */
	FDDIFlag         RT_Flag;			/* flag if frame sent */
	uInt16           srState;			/* current state of SRP */
	Int32            TSR;				/* SRP timer */
	ECEntryType     *srActive;			/* active notifications list */
	ECEntryType     *srFree;			/* free elements list */
	ECEntryType      srList[EC_LIST_SIZE];	/* pool of elements */
} FBMSRPInfoType;

#ifdef MULTIPLE_STATION
struct FBMAllInfo
{
	FBMStationInfoType fbmStationData;
	FBMMACInfoType   fbmMACData[MAX_MAC_COUNT];
	FBMPATHInfoType  fbmPATHData[MAX_PATH_COUNT];
	FBMPORTInfoType  fbmPORTData[MAX_PORT_COUNT];
	FBMSRPInfoType   fbmSRPData;
};

#endif							/* MULTIPLE_STATION */


/*********************************************************************
	Defined Values
*********************************************************************/

/*
*	Operational Values
*
*	These defined values control the various operational features
*	in the FBM system.
*/

/*
*	Number of seconds between clock ticks.
*/
#define	FBM_CLOCK_TICK		2


/*
*	Byte order defines.
*/
#define	NET_ORDER			SET
#define	HOST_ORDER			CLEAR


/*
*	Action queue type IDs.
*/
#define	FBM_Q_FRAME			1
#define	FBM_Q_MESSAGE		2
#define	FBM_Q_TIMER			3

/*
*	Status Report Protocol States
*/
#define	SR_WAIT				0
#define	SR_HOLDOFF			1
#define	SR_DISABLED			2

/*
*	Status Report Protocol Flags
*/
#define	SR_ReportRequired		0x01
#define	SR_ConditionState		0x02
#define	SR_MultipleOccurrence	0x04

/*
*	Frame Monitoring Mode Bit Settings
*/
#define	FMM_MONITOR_NONE		((uInt32) 0)
#define	FMM_MONITOR_SBA			((uInt32) 0x01)
#define	FMM_MONITOR_NIF			((uInt32) 0x02)
#define	FMM_MONITOR_SMT			((uInt32) 0x04)

/*
*	Known Addresses
*/
#ifdef	SMT_Broadcast_DA
#undef	SMT_Broadcast_DA
#endif							/* SMT_Broadcast_DA */
#ifdef	NULL_ADDRESS
#undef	NULL_ADDRESS
#endif							/* NULL_ADDRESS */
#ifdef	SMT_Unknown_Address
#undef	SMT_Unknown_Address
#endif							/* SMT_Unknown_Address */
#ifdef	SMT_Directed_Beacon_DA
#undef	SMT_Directed_Beacon_DA
#endif							/* SMT_Directed_Beacon_DA */
#ifdef	SMT_SRF_DA
#undef	SMT_SRF_DA
#endif							/* SMT_SRF_DA */

#define	SMT_Broadcast_DA		fbmBroadcastAddr
#define	NULL_ADDRESS			fbmNullAddr
#define	SMT_Unknown_Address		fbmUnknownAddr
#define	SMT_Directed_Beacon_DA	fbmDirBcnDA
#define	SMT_SRF_DA				fbmSrfDA

#ifdef	USE_62_DEFS

#ifdef	BROADCAST_ADDRESS
#undef	BROADCAST_ADDRESS
#endif							/* BROADCAST_ADDRESS */
#ifdef	UNKNOWN_ADDRESS
#undef	UNKNOWN_ADDRESS
#endif							/* UNKNOWN_ADDRESS */
#ifdef	DIR_BCN_ADDRESS
#undef	DIR_BCN_ADDRESS
#endif							/* DIR_BCN_ADDRESS */
#ifdef	SRF_MULTICAST_ADDRESS
#undef	SRF_MULTICAST_ADDRESS
#endif							/* SRF_MULTICAST_ADDRESS */

#define	BROADCAST_ADDRESS		SMT_Broadcast_DA
#define	UNKNOWN_ADDRESS			SMT_Unknown_Address
#define	DIR_BCN_ADDRESS			SMT_Directed_Beacon_DA
#define	SRF_MULTICAST_ADDRESS	SMT_SRF_DA

#endif							/* USE_62_DEFS */
