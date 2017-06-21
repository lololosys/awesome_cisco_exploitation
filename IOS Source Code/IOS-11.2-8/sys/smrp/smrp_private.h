/* $Id: smrp_private.h,v 3.3.18.1 1996/05/02 08:44:51 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_private.h,v $
 *------------------------------------------------------------------
 * Private definitions for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_private.h,v $
 * Revision 3.3.18.1  1996/05/02  08:44:51  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3  1996/02/08  23:10:46  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.2  1995/11/17  18:54:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:25:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/21  18:58:25  mleelani
 * CSCdi38584:  Delete route still in SMRP routing table.
 * On receiving a poison, wait for a hold down before propagating better
 * routes to ensure that we get the poison from the parent link for that
 * route.
 *
 * Revision 2.3  1995/08/03  21:17:16  kleung
 * CSCdi38206:  SMRP member requests expires after poll timer.
 *
 * Revision 2.2  1995/06/14  22:55:13  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_PRIVATE_H__
#define __SMRP_PRIVATE_H__

/*******************************************************************
 * SMRP Data Types
 *******************************************************************/
extern uchar const SMRPVersion;

typedef ulong SMRPPortID;	/* SMRP Port Identifiers */

STRUCTURE_REF(SMRPPort);
STRUCTURE_REF(SMRPRequest);
STRUCTURE_REF(SMRPResponse);
STRUCTURE_REF(SMRPNode);
STRUCTURE_REF(SMRP_mc_node);

typedef struct SMRPIdbsb_ {
    idbtype *phys_idb;
    SMRPPort *atsmrp_port;
    SMRP_mc_node *smrp_mc_database;
} SMRPIdbsb;

/* Managed Timer types */
enum {
    SMRP_REQUEST_TIMER,
    SMRP_RESPONSE_TIMER,
    SMRP_GROUPMEMBER_TIMER,
    SMRP_GROUPCREATOR_TIMER
};

/*******************************************************************
 * 
 *******************************************************************/
enum {
    MCMaxNetwork = 0xffffff
};

enum {
    MCMaximumDistance = 0xff,		/* Unreachable distance */
    MCPortParentUnreachable = 0xfe      /* Distance to force D. Vector
					   packets */
};

enum GroupSocket {
    MCSocketStart = 1,
    MCSocketEnd = 254
};

enum {
    MaxRetries = 4,
    RequestInterval = 10 * ONESEC,
    ResponseInterval = 2 * RequestInterval * (MaxRetries + 1),
    CreatorInterval = 60 * ONESEC,
    MemberInterval = 30 * ONESEC,
    HelloInterval = 10 * ONESEC,
    NodeAgeInterval = 3 * HelloInterval,
    PurgeRouteInterval = ONEMIN,
    PrimaryRequestInterval = ONESEC,
    PrimaryRequestRetries = 4,
    SecondaryRequestInterval = ONESEC,
    SecondaryRequestRetries = 4,
    MemberRequestInterval = 5 * ONESEC,
    HolddownInterval = RequestInterval /*Holddown on poison from non-parent */ 
};


/*******************************************************************
 * SMRP Packet Types
 *******************************************************************/
enum TwoHighBits {
    MCDatagram = 0x00,
    MCRequestCommand = 0x40,
    MCPositiveResponse = 0x80,
    MCNegativeResponse = 0xC0
};

enum SixLowBits {
    MCData = 0,
    MCHello = 1,
    MCPrimaryNode = 2,
    MCSecondaryNode = 3,
    MCDistanceVector = 4,
    MCCreateGroup = 5,
    MCDeleteGroup = 6,
    MCJoinGroup = 7,
    MCLeaveGroup = 8,
    MCGroupCreator = 9,
    MCGroupMember = 10,
    MCAddEntry = 11,
    MCRemoveEntry = 12,
    MCTunnelNode = 13,
    MCLocateNode = 14,
    MCNotifyNode = 15
};

enum CiscoInternal {		/* not defined by spec */
    MCBadDatagram = 0xFF
    };

enum MulticastDataType {
    MCMulticastDatagram = MCData + MCDatagram
};

enum HelloType {
    MCHelloDatagram = MCHello + MCDatagram
};

enum PrimaryNodeType {
    MCPrimaryRequest = MCPrimaryNode + MCRequestCommand,
    MCPrimaryConfirm = MCPrimaryNode + MCPositiveResponse,
    MCPrimaryReject = MCPrimaryNode + MCNegativeResponse
};

enum SecondaryNodeType {
    MCSecondaryRequest = MCSecondaryNode + MCRequestCommand,
    MCSecondaryConfirm = MCSecondaryNode + MCPositiveResponse,
    MCSecondaryReject = MCSecondaryNode + MCNegativeResponse
};

enum AddEntryType {
    MCAddRequest = MCAddEntry + MCRequestCommand,
    MCAddConfirm = MCAddEntry + MCPositiveResponse
};

enum CreateGroupType {
    MCCreateRequest = MCCreateGroup + MCRequestCommand,
    MCCreateConfirm = MCCreateGroup + MCPositiveResponse,
    MCCreateReject = MCCreateGroup + MCNegativeResponse
};

enum DeleteGroupType {
    MCDeleteRequest = MCDeleteGroup + MCRequestCommand,
    MCDeleteConfirm = MCDeleteGroup + MCPositiveResponse,
    MCDeleteReject = MCDeleteGroup + MCNegativeResponse
};

enum RemoveEntryType {
    MCRemoveRequest = MCRemoveEntry + MCRequestCommand,
    MCRemoveConfirm = MCRemoveEntry + MCPositiveResponse
};

enum JoinGroupType {
    MCJoinRequest = MCJoinGroup + MCRequestCommand,
    MCJoinConfirm = MCJoinGroup + MCPositiveResponse,
    MCJoinReject = MCJoinGroup + MCNegativeResponse
};

enum GroupCreatorType {
    MCCreatorRequest = MCGroupCreator + MCRequestCommand,
    MCCreatorConfirm = MCGroupCreator + MCPositiveResponse
};

enum GroupMemberType {
    MCMemberRequest = MCGroupMember + MCRequestCommand,
    MCMemberConfirm = MCGroupMember + MCPositiveResponse,
    MCMemberReject = MCGroupMember + MCNegativeResponse
};

enum LeaveGroupType {
    MCLeaveRequest = MCLeaveGroup + MCRequestCommand,
    MCLeaveConfirm = MCLeaveGroup + MCPositiveResponse,
    MCLeaveReject = MCLeaveGroup + MCNegativeResponse
};

enum DistanceVectorType {
    MCDistanceRequest = MCDistanceVector + MCRequestCommand,
    MCDistanceConfirm = MCDistanceVector + MCPositiveResponse
};

enum NotifyType {
    MCNotifyRequest = MCNotifyNode + MCRequestCommand,
    MCNotifyConfirm = MCNotifyNode + MCPositiveResponse,
    MCNotifyReject = MCNotifyNode + MCNegativeResponse
};
#define SMRP_NOTIFY_SIZE 1

enum LocateType {
    MCLocateRequest = MCLocateNode + MCRequestCommand,
    MCLocateConfirm = MCLocateNode + MCPositiveResponse,
    MCLocateReject = MCLocateNode + MCNegativeResponse
};

enum TunnelType {
    MCTunnelRequest = MCTunnelNode + MCRequestCommand,
    MCTunnelConfirm = MCTunnelNode + MCPositiveResponse,
    MCTunnelReject = MCTunnelNode + MCNegativeResponse
};

typedef struct {
    uchar ProtocolVersion;
    uchar PacketType;
    ushort SequenceNumber;
    ulong GroupAddress;
} MCHeader;
#define SMRP_HEADER_SIZE 8
#define SMRP_PAKTYPE_OFFSET 1
#define SMRP_SEQNUM_OFFSET 2
#define SMRP_GROUPADDR_OFFSET 4

/*
 * Distance Vector Packets
 */
#define SMRP_VECTOR_SIZE 8

typedef struct {
    MCHeader	Header;
    uchar	Vector[0];
} MCDistancePacket;

/*
 * Tunnel Packets
 */
#define SMRP_TUNNEL_PKT_SIZE 44 
typedef struct {
    ushort      RangeStart;
    ushort      RangeEnd;
    uchar       Requester;
    uchar       Distance;
    ulong       Address;
    char        Name[34];
} MCTunnelPacket;

#define SMRP_REJECT_SIZE 2

typedef enum RequestState_ {
    MCRequestPending = 0,	/* not defined by spec */
    MCRequestInProgress = 1,
    MCRequestComplete = 2
} RequestState;

enum ErrorValues {
    MCNoError = -7700,                   /* 0xE1EC */
    MCPacketLength = -7701,              /* 0xE1EB */
    MCBadState = -7702,                  /* 0xE1EA */
    MCTableFull = -7703,                 /* 0xE1E9 */
    MCRangeFull = -7704,                 /* 0xE1E8 */
    MCListFull = -7705,                  /* 0xE1E7 */
    MCRouteNotFound = -7706,             /* 0xE1E6 */
    MCPortNotFound = -7707,              /* 0xE1E5 */
    MCGroupNotFound = -7708,             /* 0xE1E4 */
    MCNotCreator = -7709,                /* 0xE1E3 */
    MCNoResponse = -7710,                /* 0xE1E2 */
    MCNoTunnelNeeded = -7711             /* 0xE1E1 */
};


/*******************************************************************
 * SMRP Port and Node States
 *******************************************************************/
typedef enum {
    MCNotOperational = 1,
    MCPrimaryNegotiation,
    MCPrimaryOperation,
    MCSecondaryNegotiation,
    MCSecondaryOperation,
    MCNormalOperation,
    MCNotInitialized
} NodeState;


/*******************************************************************
 * SMRP Misc Definitions
 *******************************************************************/

/*
 *==================================================================
 * SMRP Include Files
 *==================================================================
 */

/*
 *------------------------------------------------------------------
 * SMRP Module Headers
 *------------------------------------------------------------------
 */
#include "smrp_net.h"
#include "smrp_fast.h"
#include "smrp_group.h"
#include "smrp_port.h"
#include "smrp_input.h"
#include "smrp_node.h"
#include "smrp_route.h"
#include "smrp_maint.h"
#include "smrp_util.h"
#include "smrp_parse.h"
#include "smrp_trans.h"
#include "smrp_globals.h"
#include "smrp_show.h"
#include "smrp_debug.h"
#include "smrp_clear.h"

/*******************************************************************
 * Useful Inline Routines
 *******************************************************************/
static inline SMRPIdbsb *
smrp_GetIdbsb (idbtype *idb)
{
    return ((SMRPIdbsb *) idb_get_swsb(idb, SWIDB_SB_SMRP));
}

/* Return lower six bits of protocol type */
extern inline uchar
smrp_GetPacketType(uchar pt)
{
    return pt & 0x3f;
}

/* Return upper two bits of protocol type */
extern inline uchar
smrp_GetPacketModifiers(uchar pt)
{
    return pt & 0xc0;
}

extern inline boolean
smrp_IsTransactionPacket(uchar pt)
{
    return ((pt & 0xC0) != 0);
}

extern inline boolean
smrp_IsRequestPacket(uchar pt)
{
    return ((pt & 0xc0) == MCRequestCommand);
}

extern inline boolean
smrp_IsResponsePacket(uchar pt)
{
    return ((pt & 0x80) != 0);
}

extern inline uchar *
smrp_datastart (paktype *pak)
{
    return (pak->transport_start + SMRP_HEADER_SIZE);
}

extern inline uchar *
smrp_datastart_offset (MCHeader *head)
{
    return ((uchar *) head + SMRP_HEADER_SIZE);
}

/*******************************************************************
 * Traffic statistics definitions
 *******************************************************************/
enum {
    SMRP_INPUT,
    SMRP_OUTPUT,
    SMRP_NO_BUFFERS,
    SMRP_MCDATA_IN,
    SMRP_MCDATA_OUT,
    SMRP_HELLO_IN,
    SMRP_HELLO_OUT,
    SMRP_REQUESTS_IN,
    SMRP_REQUESTS_OUT,
    SMRP_NOTIFY_IN,
    SMRP_NOTIFY_OUT,
    SMRP_CONFIRMS_IN,
    SMRP_CONFIRMS_OUT,
    SMRP_REJECTS_IN,
    SMRP_REJECTS_OUT,
    SMRP_PNODE_IN,
    SMRP_PNODE_OUT,
    SMRP_SNODE_IN,
    SMRP_SNODE_OUT,
    SMRP_DISTANCE_IN,
    SMRP_DISTANCE_OUT,
    SMRP_CREATE_IN,
    SMRP_DELETE_IN,
    SMRP_DELETE_OUT,
    SMRP_JOIN_IN,
    SMRP_JOIN_OUT,
    SMRP_LEAVE_IN,
    SMRP_LEAVE_OUT,
    SMRP_LOCATE_IN,
    SMRP_TUNNEL_IN,
    SMRP_CREATOR_OUT,
    SMRP_MEMBER_IN,
    SMRP_MEMBER_OUT,
    SMRP_ADDGRPENTRY_IN,
    SMRP_ADDGRPENTRY_OUT,
    SMRP_REMGRPENTRY_IN,
    SMRP_REMGRPENTRY_OUT,
    SMRP_NO_FORWARD,
    SMRP_FASTFWD_IN,
    SMRP_FASTFWD_OUT,
    SMRP_FORWARD_DUPS,
    SMRP_FORWARDS,
    SMRP_BAD_PORTID,
    SMRP_PORT_DOWN,
    SMRP_INPUT_ERR,
    SMRP_BADVERS,
    SMRP_RUNT,
    SMRP_BAD_PKTTYPE,
    SMRP_UNEXPECTED_PKT,
    SMRP_MAXTRAFFIC		/* always last */
};

#define SMRPASSERT(exp) {if (!(exp)) {smrp_assert(__FILE__, __LINE__);}}

typedef struct {
    SMRPPortID PortID;
    UnicastAddr matchaddr;
    UnicastAddr setaddr;
    GroupToMCfn mapper;
    boolean localgroups;
    boolean nonlocalgroups;
} SMRP_fn_data;

#endif __SMRP_PRIVATE_H__
