/* $Id: smrp_trans.h,v 3.2 1995/11/17 18:54:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_trans.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP transactions.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_trans.h,v $
 * Revision 3.2  1995/11/17  18:54:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/25  21:29:39  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.1  1995/06/07  22:45:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_TRANS_H__
#define __SMRP_TRANS_H__
#define SMRP_REQ_BUFFERSIZE (SMRP_HEADER_SIZE + 20)
#define SMRP_MAX_RESPONSE_SIZE (SMRP_HEADER_SIZE + 44)

typedef void (*smrpReqProc)(struct SMRPRequest_ *);

struct SMRPRequest_ {
    struct SMRPRequest_*	next;
    ushort		SequenceNumber;
    SMRPGroupID 	GroupAddress;
    UnicastAddr 	Destination;
    uchar		PacketType;
    uchar		State;
    ushort		Attempts;
    ushort              MaxAttempts;
    smrpReqProc		CompletionProc;
    SMRPPortID		PortID;
    MCHeader*		PacketData;
    uint		DataSize;
    boolean		DataAllocated;
    uchar		ResponseType;
    paktype*		ResponsePacket;
    MCHeader*		ResponseData;
    uint		ResponseSize;
    ulong		ErrorIndication;
    UnicastAddr 	Responder;
    sys_timestamp	LastAction;
    mgd_timer           Timer;
    ulong               RetransInterval;
    uchar		RequestDataBuffer[SMRP_REQ_BUFFERSIZE];
};

struct SMRPResponse_ {
    struct SMRPResponse_*	next;
    ushort		SequenceNumber;
    SMRPGroupID 	GroupAddress;
    UnicastAddr 	Source;
    uchar		PacketType;
    uchar		State;
    ulong		ErrorIndication;
    SMRPPortID		PortID;

    paktype*		RequestPacket;
    uchar *		RequestData;
    uint		RequestSize;
    sys_timestamp	LastAction;

    uint		ResponseSize;
    uchar 		ResponseType;
    uchar		ResponseBuffer[SMRP_MAX_RESPONSE_SIZE];
};

/*
 *------------------------------------------------------------------
 * SMRP Transaction Module Traffic Definitions
 *------------------------------------------------------------------
 */
enum {
    SMRPTRANS_REQUESTS_NEW,
    SMRPTRANS_REQUESTS_RECYCLED,
    SMRPTRANS_REQUESTS_FREED,
    SMRPTRANS_REQUESTS_DELETED,
    SMRPTRANS_REQUESTS_TIMEOUTS,
    SMRPTRANS_REQUESTS_RESENDS,
    SMRPTRANS_REQUESTS_DUP,
    SMRPTRANS_REQUESTS_DUP_INCOMPLETE,
    SMRPTRANS_REQUESTS_UNEXPECTED,
    SMRPTRANS_RESPONSES_NEW,
    SMRPTRANS_RESPONSES_RECYCLED,
    SMRPTRANS_RESPONSES_FREED,
    SMRPTRANS_RESPONSES_DELETED,
    SMRPTRANS_RESPONSES_UNEXPECTED,
    SMRPTRANS_RESPONSES_BAD,
    SMRPTRANS_MAXTRAFFIC		/* must be last */
    };

extern ulong* SMRPTRANS_stat;

void smrp_trans_Initialize(void);
void smrp_background(void);

boolean smrp_CreateRequest(SMRPPort *, uchar, SMRPGroupID, UnicastAddr,
			   void *, uint, ulong, ushort, smrpReqProc);
void smrp_SetResponseState(SMRPResponse *, RequestState);
void smrp_SendRejectResponse(SMRPResponse *, short);
void smrp_SendConfirmResponse(SMRPResponse *);
boolean smrp_ResponseIsConfirmation(SMRPRequest *);
void smrp_SendResponseByGroupAndType(SMRPGroupID, uchar, enum ErrorValues);
void smrp_SetGroupMemberTimer(sys_timestamp);
void smrp_SetGroupCreatorTimer(sys_timestamp);
inline SMRPResponse *smrp_FirstResponse(void);
inline SMRPRequest *smrp_FirstRequest(void);

void smrp_HandleRequest(SMRPPort *, paktype *, uchar, SMRPGroupID,
			UnicastAddr, ushort);
void smrp_HandleResponse(SMRPPort *, paktype *, uchar, SMRPGroupID,
			 UnicastAddr, ushort);
void smrp_DiscardResponse(SMRPResponse *);
void smrp_DiscardRequest(SMRPRequest *);
SMRPRequest *smrp_FindRequestByPort(SMRPPortID, uchar);
SMRPRequest *smrp_FindRequestByGroupAndType(SMRPGroupID, uchar);
void smrp_SetupCreatorTimer(SMRPGroup *);
void smrp_SetupMemberTimer(SMRPForward *);
#endif __SMRP_TRANS_H__

