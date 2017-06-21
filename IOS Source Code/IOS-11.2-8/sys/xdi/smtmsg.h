/* $Id: smtmsg.h,v 3.2 1995/11/17 19:19:52 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/smtmsg.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: smtmsg.h,v $
 * Revision 3.2  1995/11/17  19:19:52  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_smtmsg.h xm7_v3.1.1:cset.0173:9:9 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Station Management Header File
		
		SMT Messages Header File

		File:			xm7_smtmsg.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This header file is used by all SMT modules that must communicate
		between processes.

		There are four types of messages: events, actions, requests,
		and indications. Events are generated asynchronously to notify
		one process of a change in another process. An example of this
		is when CMT changes the station configuration, it will notify
		the MIB of the new configuration. Actions are commands from
		one process to another; a Connect or Disconnect is an example
		of an action. A request is issued when one process requires
		information from another. An indication is the message sent in
		response to a request.

		A message consists of the following fields:

		destination
			This is the ID number of the process who will receive this message.
		source
			This is the ID number of the process who sent this message.
		type
			This is the message type: event, action, or response. This
			value is coded to include the message type, the associated
			entity, and the message ID.
		typeInfo
			This member is for additional type information.  It is used
			generally to hold the MIB attribute ID when requesting or
			responding with information in the MIB.
		localID
			Value reserved for MAP to specify. This field is set to 0 by
			frames generated within the Manager. For responses to the MAP,
			the value is set to the localID field of the request message.
		entity
			This is the PHY or MAC index if the data contained in the
			message pertains to a PHY or MAC.
		stnID
			This value is the station index of the destination station.
			In single station implementations, this value should be set
			to zero (0).
		data
			A union of data sizes to hold a single value.  This field
			is convenient for CSP to report new MIB values without
			having to allocate memory for the data.
		len1
			This is the length of the buffer at p1.
		p1
			A union of pointers to additional message buffers.
		len2
			This is the length of the buffer at p2.
		p2
			A pointer to the data buffer for the message.

		The process ID number is a manifest constant identifying either
		CSP, MSP, FSP, or MAP.

		The message type is a 16-bit value encoded as follows:
			0 x T P I I
				| | | |
				| | +-+- This is the message identifier.
				| |
				| +----- This is the process or object associated
				|         with the message.
				|
				+------- This is the message type.

		The implementation of the communication mechanism between processes
		is left to the implementer. Different processes may have different
		communication requirements. Therefore, it is important to refer
		to the interface documentation for each process before selecting
		a communication mechanism.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
	Data Structures
*********************************************************************/

/*****
*	Structure of info member for FBM_ACTION_SEND_FRAME.
*****/
typedef struct SendFrameStruct
{
	uChar            frameClass;		/* frame class to send */
	uChar            frameType;			/* frame type to send */
	uInt32           sentTransID;		/* xaction ID sent in frame */
	/*
	 * The following union contains a different data structure for each frame
	 * class.
	 */
	union
	{
		/**
		* NIF - Request or Announcement
		*
		*	FC
		*		FC field to use must be SMT_INFO_FC (0x41) or
		*		SMT_NSA_FC (0x4F). Only used for NIF requests.
		*	destAddr
		*		Destination address for frame. Only used for NIF requests.
		*	sourceMAC
		*		Index of MAC to transmit frame.
		*	transID
		*		Transaction ID to use. 0 if one should be generated.
		**/
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			uInt32           transID;
		} nif;

		/**
		* SIF Configuration or Operation - Request
		*
		*	FC
		*		FC field to use must be SMT_INFO_FC (0x41) or
		*		SMT_NSA_FC (0x4F).
		*	destAddr
		*		Destination address for frame.
		*	sourceMAC
		*		Index of MAC to transmit frame.
		*	transID
		*		Transaction ID to use. 0 if one should be generated.
		**/
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			uInt32           transID;
		} sif;

		/**
		* ECF - Request
		*
		*	FC
		*		FC field to use must be SMT_INFO_FC (0x41) or
		*		SMT_NSA_FC (0x4F).
		*	destAddr
		*		Destination address for frame.
		*	sourceMAC
		*		Index of MAC to transmit frame.
		*	transID
		*		Transaction ID to use. 0 if one should be generated.
		*/
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			uInt32           transID;
		} ecf;

		/*
		 * RDF - Response
		 * 
		 * sourceMAC Index of MAC to transmit frame. reasonCode Reason for
		 * rejecting the frame. Contents of request frame including the
		 * padding before the FC field is provided by p2 in the message
		 * structure.
		 */
		struct
		{
			uInt16           sourceMAC;
			uInt32           reasonCode;
		} rdf;

		/*
		 * RAF - Announcement or Request
		 * 
		 * FC FC field to use must be SMT_INFO_FC (0x41) or SMT_NSA_FC (0x4F).
		 * destAddr Destination address for frame. sourceMAC Index of MAC to
		 * transmit frame. transID Transaction ID to use. 0 if one should be
		 * generated.
		 * 
		 * Contents of the SMT InfoField is provided by p2 in the message
		 * structure.
		 */
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			uInt32           transID;
		} raf;

		/*
		 * ESF - Announcement or Request
		 * 
		 * FC FC field to use must be SMT_INFO_FC (0x41) or SMT_NSA_FC (0x4F).
		 * destAddr Destination address for frame. sourceMAC Index of MAC to
		 * transmit frame. esfID ESF identification code. transID Transaction
		 * ID to use. 0 if one should be generated.
		 * 
		 * Contents of the SMT InfoField is provided by p2 in the message
		 * structure.
		 */
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			FDDILongAddressType esfID;
			uInt32           transID;
		} esf;

		/*
		 * PMF Get Request
		 * 
		 * FC FC field to use must be SMT_INFO_FC (0x41) or SMT_NSA_FC (0x4F).
		 * destAddr Destination address for frame. sourceMAC Index of MAC to
		 * transmit frame. transID Transaction ID to use. 0 if one should be
		 * generated.
		 * 
		 * Attributes for the Get request are provided by p2 in the message
		 * structure.
		 */
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			uInt32           transID;
		} getpmf;

		/*
		 * PMF Set Request
		 * 
		 * FC FC field to use must be SMT_INFO_FC (0x41) or SMT_NSA_FC (0x4F).
		 * destAddr Destination address for frame. sourceMAC Index of MAC to
		 * transmit frame. setCount Value of set count to use in frame.
		 * transID Transaction ID to use. 0 if one should be generated.
		 * 
		 * Contents of Set Request are put into p2 of the message.
		 */
		struct
		{
			uChar            FC;
			FDDILongAddressType destAddr;
			uInt16           sourceMAC;
			TLVSetCountType  setCount;
			uInt32           transID;
		} setpmf;
	} frameData;
} SendFrameType;

#define	SIZE_SendFrameType	(sizeof (SendFrameType))

/*****
*	Structure of info member of FBM_EVENT_FRAME_RECEIVED event.
*	Index of MAC that received the frame is given by member entity in
*	SMT message structure above.
*****/
typedef struct RecFrameStruct
{
	uChar            EACbits;			/* E, A, and C bits */
	uChar            pad;				/* padding */
	uInt16           frameLen;			/* total length of frame */
} RecFrameType;

#define	SIZE_RecFrameType	(sizeof (RecFrameType))

/*****
*	Structure used for all MIB action messages.
*****/
typedef struct MIBMessageStruct
{
	uInt32           result;			/* value returned by MIB routine */
	SMTStationIdType setStationID;		/* addr of station setting value */
	TLVSetCountType  setCount;			/* buffer for fddiSMTSetCount */
} MIBMsgType;

#define	SIZE_MIBMsgType		(sizeof (MIBMsgType))

/*****
*	Structure used for all MIB notification messages.
*****/
typedef struct MIBNotificationStruct
{
	uInt16           type;				/* EC_TYPE_EVENT or EC_TYPE_CONDITION */
	FDDIFlag         state;				/* condition state (if condition) */
	uInt32           info;				/* 4 octets for addl info */
} MIBNotificationType;

#define	SIZE_MIBNotificationType	(sizeof (MIBNotificationType))

/*****
*	Structure used to inform CSP/FSP when a MAC is added into the MIB.
*****/
typedef struct MIBMACAddStruct
{
	uChar            requestedPaths;	/* fddiMACRequestedPaths */
	uInt16           index;				/* fddiMACIndex */
	Flag             unitdataEnable;	/* fddiMACMA-UnitdataEnable */
	uInt16           rmtDupPolicy;		/* xdiMACRMTDupPolicy */
	uInt16           localPathIndex;	/* xdiMACLocalPath */
	                 PathConfigType		/* xdiMACPathConfiguration */
	                 configList[MAX_PATH_COUNT];
} MIBMACAddType;

#define	SIZE_MIBMACAddType		(sizeof (MIBMACAddType))

/*****
*	Structure used to inform CSP/FSP when a MAC is inserted into the MIB.
*****/
typedef struct MIBMACInsertStruct
{
	uInt32           T_MaxCap;			/* fddiMACT-MaxCapability */
	uInt32           TVXCap;			/* fddiMACTVXCapability */
	uChar            availablePaths;	/* fddiMACAvailablePaths */
	                 FDDILongAddressType/* fddiMACSMTAddress */
	                 SMTAddress;
} MIBMACInsertType;

#define	SIZE_MIBMACInsertType		(sizeof (MIBMACInsertType))

/*****
*	Structure used to inform CSP/FSP when a PORT is added into the MIB.
*****/
typedef struct MIBPORTAddStruct
{
	uInt16           PC_Type;			/* fddiPORTMy-Type */
	uChar            connectionPolicies;/* fddiPORTConnectionPolicies */
	uChar            requestedPaths[3];	/* fddiPORTRequestedPaths */
	uInt16           index;				/* fddiPORTIndex */
	uChar            lerCutoff;			/* fddiPORTLer-Cutoff */
	uChar            lerAlarm;			/* fddiPORTLer-Alarm */
	uInt16           localPathIndex;	/* xdiPORTLocalPath */
	PathConfigType   configList[MAX_PATH_COUNT];	/* xdiPORTPathConfiguratio
													 * n */
	uInt32           MACLoop_Time;		/* fddiPORTMACLoop-Time */
	uInt16           Maint_LS;			/* fddiPORTMaint-LS */
} MIBPORTAddType;

#define	SIZE_MIBPORTAddType		(sizeof (MIBPORTAddType))

/*****
*	Structure used to inform CSP/FSP when a PORT is inserted into the MIB.
*****/
typedef struct MIBPORTInsertStruct
{
	uChar            availablePaths;	/* fddiPORTAvailablePaths */
	uChar            conCapabilities;	/* fddiPORTConnectionCapabilities */
} MIBPORTInsertType;

#define	SIZE_MIBPORTInsertType		(sizeof (MIBPORTInsertType))

/*****
*	SBA Event/Action structures.
*****/

/* structure for SBA Report Command/Response */
typedef struct SbaRptStruct
{
	FDDILongAddressType address;		/* SA or DA in frame */
	uInt32           transactionID;		/* transaction ID in frame */
	uInt16           pathIndex1;		/* primary path rsrc index */
	uInt32           payload1;			/* primary SBA payload */
	uInt32           overhead1;			/* primary SBA overhead */
	uInt16           pathIndex2;		/* secondary path rsrc index */
	uInt32           payload2;			/* secondary SBA payload */
	uInt32           overhead2;			/* secondary SBA overhead */
} SbaRptType;

#define	SIZE_SbaRptType				(sizeof (SbaRptType))

/* structure for SBA Request Allocation Command */
typedef struct SbaAllocReqStruct
{
	FDDILongAddressType address;		/* SA or DA in frame */
	uInt32           transactionID;		/* transaction ID in frame */
	uInt16           pathIndex;			/* allocation path rsrc index */
	uInt32           requestPayload;	/* requested payload */
	uInt32           requestOverhead;	/* requested overhead */
	uInt32           payload;			/* current payload */
	uInt32           overhead;			/* current overhead */
	FDDILongAddressType allocAddress;	/* addr responsible for alloc */
	uInt32           category;			/* allocation category */
	uInt32           maxTNeg;			/* max T_Neg acceptable for svc */
	uInt32           minSegment;		/* min # bytes/frame */
} SbaAllocReqType;

#define	SIZE_SbaAllocReqType		(sizeof (SbaAllocReqType))

/* structure for SBA Request Allocation Response */
typedef struct SbaAllocRspStruct
{
	FDDILongAddressType address;		/* SA or DA in frame */
	uInt32           transactionID;		/* transaction ID in frame */
	uInt32           reasonCode;		/* result of request */
	uInt16           pathIndex;			/* allocation path rsrc index */
	uInt32           payload;			/* current payload allocation */
	uInt32           overhead;			/* current overhead allocation */
	FDDILongAddressType allocAddress;	/* addr responsible for alloc */
	uInt32           category;			/* allocation category */
	uInt32           allocatable;		/* total SB available for alloc */
} SbaAllocRspType;

#define	SIZE_SbaAllocRspType		(sizeof (SbaAllocRspType))

/* structures for SBA Change Allocation Command/Response */
typedef struct SbaChangeStruct
{
	FDDILongAddressType address;		/* SA or DA in frame */
	uInt32           transactionID;		/* transaction ID in frame */
	uInt16           pathIndex;			/* path rsrc index */
	uInt32           payload;			/* new/current SBA payload */
	uInt32           overhead;			/* new/current SBA overhead */
	uInt32           category;			/* SBA category */
} SbaChangeType;

#define	SIZE_SbaChangeType		(sizeof (SbaChangeType))

/* general access union for SBA structures */
typedef union SbaInfoUnion
{
	SbaRptType      *report;			/* SBA_RPT_* messages */
	SbaAllocReqType *allocReq;			/* SBA_ALLOC_REQ_TX message */
	SbaAllocRspType *allocRsp;			/* SBA_ALLOC_RSP_RX message */
	SbaChangeType   *change;			/* SBA_CHNG_* message */
} SbaInfoType;


/*****
*	This is the message data structure.
*****/
typedef struct SMTMessageStruct
{
	uInt16           destination;		/* receiving process ID */
	uInt16           source;			/* sending process ID */
	uInt16           type;				/* message type */
	uInt16           typeInfo;			/* message content */
	uInt32           localID;			/* available for implementer */
	uInt16           entity;			/* MAC or PHY index (if needed) */
	uInt16           stnID;				/* SMT station index */
	union
	{
		uChar            b8;			/* 8-bit data item */
		uInt16           b16;			/* 16-bit data item */
		uInt32           b32;			/* 32-bit data item */
		FDDILongAddressType addr;		/* 48-bit data item */
		FDDITimeStamp    timeStamp;		/* 64-bit data item */
	} data;
	uInt16           len1;				/* length of data at p1 */
	uInt16           len2;				/* length of data at p2 */
	union
	{
		Void_p           ptr;			/* generic pointer */
		SendFrameType   *sendFrameBuf;
		RecFrameType    *recFrameBuf;
		MIBMsgType      *mibMsgBuf;
		MIBNotificationType *mibNoticeBuf;
		MIBMACAddType   *MACAddBuf;
		MIBMACInsertType *MACInsertBuf;
		MIBPORTAddType  *PORTAddBuf;
		MIBPORTInsertType *PORTInsertBuf;
		SbaInfoType      sba;
	} p1;
	Void_p           p2;				/* additional data buffer */
	uInt32           vendorID;			/* vendor-specific ID */
	uInt16           vendorType;		/* VSO param type */
} SMTMessage;

#define	SIZE_SMTMessage			(sizeof (SMTMessage))


/*********************************************************************
	Defined Message Values
*********************************************************************/

/*
*	Message process IDs.
*/
#define	MAP_MSG_ID			1
#define	FBM_MSG_ID			2
#define	MIB_MSG_ID			3
#define	CSP_MSG_ID			4

/*
*	Message types
*/
#define	EVENT_MSG			0x1000
#define	ACTION_MSG			0x2000
#define	RESPONSE_MSG		0x3000
#define	ERROR_MSG			0x4000

/*
*	Macro to extract message types.
*/
#define	MESSAGE_TYPE(i)		(i & 0xF000)

/*
*	Message processes and objects.
*/
#define	MAP_MSG				0x0100
#define	FBM_MSG				0x0200
#define	MIB_MSG				0x0300
#define	CSP_MSG				0x0400


/*********************************************************************
	Message Types
*********************************************************************/

/**********
*	FBM event messages.
**********/

/*
* The FBM_EVENT_NOTIFY_MIB message notifies the MIB of a change
* to an SMT managed attribute. The typeInfo member contains
* the MIB attribute ID for the attribute to change. The data field
* contains the new value.
*/
#define	FBM_EVENT_NOTIFY_MIB		(EVENT_MSG | FBM_MSG | 0x11)

/*
* The FBM_EVENT_FRAME_RECEIVED message notifies the MAP that FBM has
* received a message that does not belong to any of its protocols.
* Generally, this signifies receipt of a response frame in reply to
* a request sent by the MAP, or a status announcement frame.
*/
#define	FBM_EVENT_FRAME_RECEIVED	(EVENT_MSG | FBM_MSG | 0x12)

/*
* The FBM_EVENT_SBA message notifies the MAP that SMT has received a
* RAF for the SBA resource. The specific frame and command information
* is indicated by the typeInfo member in the message header using the
* defined values listed below. Events are always associated with
* received frames indicated by the _RX suffix on the constant name.
*/
#define	FBM_EVENT_SBA				(EVENT_MSG | FBM_MSG | 0x13)

#define	SBA_RPT_RSP_RX		0x0101		/* Report allocation response */
#define	SBA_ALLOC_REQ_RX	0x0102		/* Allocation requested */
#define	SBA_ALLOC_RSP_RX	0x0103		/* Allocation request result */
#define	SBA_CHNG_REQ_RX		0x0104		/* Change allocation request */
#define	SBA_CHNG_RSP_RX		0x0105		/* Change allocation result */

/*
* The FBM_EVENT_SBA_CONTROL message notifies the MAP that SMT has
* detected some condition related to synchronous service.
*/
#define	FBM_EVENT_SBA_CONTROL		(EVENT_MSG | FBM_MSG | 0x14)

#define	SBA_MONITOR_NIF		0x0301

/*
* The FBM_EVENT_MONITOR_NIF message reports all NIFs that SMT receives.
* This reporting only occurs when NIF monitoring is enabled.
*/
#define	FBM_EVENT_MONITOR_NIF		(EVENT_MSG | FBM_MSG | 0x15)

/*
* The FBM_EVENT_MONITOR_SMT message reports all SMT frames that SMT receives.
* This reporting only occurs when SMT monitoring is enabled.
*/
#define	FBM_EVENT_MONITOR_SMT		(EVENT_MSG | FBM_MSG | 0x16)


/**********
*	MIB event messages.
**********/

/*
* The MIB_EVENT_NOTIFY_CHANGE message reports changes in MIB attribute
* value to other tasks in SMT. This allows the other tasks to maintain
* current information without having to poll the MIB.
*/
#define	MIB_EVENT_NOTIFY_CHANGE		(EVENT_MSG | MIB_MSG | 0x11)
#define	MIB_EVENT_NOTIFY_SMT		(EVENT_MSG | MIB_MSG | 0x12)

#define	MIB_EVENT_ADD_MAC			(EVENT_MSG | MIB_MSG | 0x13)
#define	MIB_EVENT_INSERT_MAC		(EVENT_MSG | MIB_MSG | 0x14)
#define	MIB_EVENT_REMOVE_MAC		(EVENT_MSG | MIB_MSG | 0x15)
#define	MIB_EVENT_DELETE_MAC		(EVENT_MSG | MIB_MSG | 0x16)

#define	MIB_EVENT_ADD_PORT			(EVENT_MSG | MIB_MSG | 0x17)
#define	MIB_EVENT_INSERT_PORT		(EVENT_MSG | MIB_MSG | 0x18)
#define	MIB_EVENT_REMOVE_PORT		(EVENT_MSG | MIB_MSG | 0x19)
#define	MIB_EVENT_DELETE_PORT		(EVENT_MSG | MIB_MSG | 0x1A)

#define	MIB_EVENT_ADD_MASIC			(EVENT_MSG | MIB_MSG | 0x1B)
#define	MIB_EVENT_INSERT_MASIC		(EVENT_MSG | MIB_MSG | 0x1C)
#define	MIB_EVENT_REMOVE_MASIC		(EVENT_MSG | MIB_MSG | 0x1D)
#define	MIB_EVENT_DELETE_MASIC		(EVENT_MSG | MIB_MSG | 0x1E)


/**********
*	CSP event messages.
**********/

/*
* The CSP_EVENT_NOTIFY_MIB message is used by CSP like the
* FBM_EVENT_NOTIFY_MIB message is used by the MIB.
*/
#define	CSP_EVENT_NOTIFY_MIB		(EVENT_MSG | CSP_MSG | 0x11)


/*
*	Action messages.
*/

/* FBM actions */
#define	FBM_ACTION_SEND_FRAME			(ACTION_MSG | FBM_MSG | 0x01)

/*
* The FBM_ACTION_SBA message allows the MAP to cause SMT to issue a
* RAF for the SBA resource. The specific frame and command information
* is indicated by the typeInfo member in the message header using the
* defined values listed below. Actions are always associated with
* transmitted frames indicated by the _TX suffix on the constant name.
*/
#define	FBM_ACTION_SBA					(ACTION_MSG | FBM_MSG | 0x02)

#define	SBA_RPT_REQ_TX		0x0201		/* Report allocation request */
#define	SBA_ALLOC_REQ_TX	0x0202		/* Request allocation from allocator */
#define	SBA_ALLOC_RSP_TX	0x0203		/* Allocation request result */
#define	SBA_CHNG_REQ_TX		0x0204		/* Request change allocation */
#define	SBA_CHNG_RSP_TX		0x0205		/* Change allocation result */

/*
* The FBM_ACTION_SBA_CONTROL message changes the monitoring state for
* synchronous monitoring functions.
*/
#define	FBM_ACTION_SBA_CONTROL			(ACTION_MSG | FBM_MSG | 0x03)

/*
* The FBM_ACTION_MONITOR_NIF message changes the monitoring state for
* reporting NIFs.
*/
#define	FBM_ACTION_MONITOR_NIF			(ACTION_MSG | FBM_MSG | 0x04)

/*
* The FBM_ACTION_MONITOR_SMT message changes the monitoring state for
* reporting SMT frames.
*/
#define	FBM_ACTION_MONITOR_SMT			(ACTION_MSG | FBM_MSG | 0x05)


/* MIB actions */
#define	MIB_ACTION_GET_ATTR				(ACTION_MSG | MIB_MSG | 0x01)
#define	MIB_ACTION_CHANGE_ATTR			(ACTION_MSG | MIB_MSG | 0x02)
#define	MIB_ACTION_DETECT_CONDITIONS	(ACTION_MSG | MIB_MSG | 0x06)
#define	MIB_ACTION_MULTIGET_ATTR		(ACTION_MSG | MIB_MSG | 0x07)
#define	MIB_ACTION_SHORT_GET_ATTR		(ACTION_MSG | MIB_MSG | 0x08)

/* CSP actions */
#define	CSP_ACTION_FDDI_SMT				(ACTION_MSG | CSP_MSG | 0x11)
#define	CSP_ACTION_FDDI_MAC				(ACTION_MSG | CSP_MSG | 0x12)
#define	CSP_ACTION_FDDI_PORT			(ACTION_MSG | CSP_MSG | 0x13)
#define	CSP_ACTION_FDDI_PATH			(ACTION_MSG | CSP_MSG | 0x14)

/**********
*	Response messages.
**********/

/* Station responses */

/* MAC responses */

/* PHY responses */

/* MAP responses */

/* FBM responses */
#define	FBM_RESPONSE_FRAME_SENT		(RESPONSE_MSG | FBM_MSG | 0x01)
#define	FBM_RESPONSE_SBA			(RESPONSE_MSG | FBM_MSG | 0x02)

/*
* The FBM_RESPONSE_SBA_CONTROL message reports the monitoring state for
* synchronous monitoring functions.
*/
#define	FBM_RESPONSE_SBA_CONTROL	(RESPONSE_MSG | FBM_MSG | 0x03)

/*
* The FBM_RESPONSE_MONITOR_NIF message reports the monitoring state for
* reporting NIFs.
*/
#define	FBM_RESPONSE_MONITOR_NIF	(RESPONSE_MSG | FBM_MSG | 0x04)

/*
* The FBM_RESPONSE_MONITOR_SMT message reports the monitoring state for
* reporting SMT frames.
*/
#define	FBM_RESPONSE_MONITOR_SMT	(RESPONSE_MSG | FBM_MSG | 0x05)

/* MIB responses */
#define	MIB_RESPONSE_GET_ATTR		(RESPONSE_MSG | MIB_MSG | 0x01)
#define	MIB_RESPONSE_CHANGE_ATTR	(RESPONSE_MSG | MIB_MSG | 0x02)

/* CSP responses */


/*********
*	Error Message Types
*********/

#define	MIB_ERROR_MSG				(ERROR_MSG | MIB_MSG | MIB_MSG_ID)
#define CSP_ERROR_MSG				(ERROR_MSG | CSP_MSG | CSP_MSG_ID)
#define	FBM_ERROR_MSG				(ERROR_MSG | FBM_MSG | FBM_MSG_ID)


#ifdef	USE_62_DEFS

typedef SendFrameType SendFrameBuf;
typedef RecFrameType RecFrameBuf;

#define	SMT_SEND_BUF_SIZE			SIZE_SendFrameBuf
#define	SMT_REC_BUF_SIZE			SIZE_RecFrameBuf
#define	MIB_MSG_SIZE				SIZE_MIBMsgType

#endif							/* USE_62_DEFS */


/*********************************************************************
	Implementer Defined Message Types
*********************************************************************/

/*
#include	"smtmsgim.h"
*/
#include	"../if/if_xdi.h"
