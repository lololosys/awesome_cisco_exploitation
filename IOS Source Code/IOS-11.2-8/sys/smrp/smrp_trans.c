/* $Id: smrp_trans.c,v 3.3.62.3 1996/08/28 13:09:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_trans.c,v $
 *------------------------------------------------------------------
 * SMRP Background process and Transaction Management
 *
 * The background process handles all transaction packets, request
 * and response queues, and group timers.
 *
 * The basic design uses two queues for all transactions. A Request
 * queue used for queries that we initiate. And a Response queue used
 * for transactions that we are responding to. 
 *
 * The request queue entries are deleted after we process the response
 * that we receive.  The response is processed, once matched with a
 * request, using a callback specified in the entry.  After response
 * processing, the request is discarded.  If the request is
 * unanswered, a internally generated reject response, with the error
 * MCNoResponse, is sent to the callback.  Requests may be sent to
 * either a unicast address or a special all-nodes or all-endpoints
 * multicast depending on context.  Unless explicitly redirected, they
 * are sent to the all-nodes multicast.
 *
 * The response queue entries are created upon receipt of a request
 * packet.  The entry is referenced during all processing of the
 * request.  The processed entry remains in the queue until it expires
 * at which time it deleted from the queue.  If a duplicate request is
 * received, it is ignored if we are still processing the original
 * request or a duplicate response is generated if processing is
 * complete.  Responses are always unicast to the requestor.  Some
 * received requests may require our node to generate additional
 * requests, in such a situation, the original request(s) will be
 * processed by the callback handler of our request entry.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_trans.c,v $
 * Revision 3.3.62.3  1996/08/28  13:09:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/06/28  23:26:01  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/03/18  21:47:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:42:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:11:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:54:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/07/25  21:29:36  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.8  1995/07/18  19:47:43  kleung
 * CSCdi37260:  The first MC group should be randomly selected.
 *
 * Revision 2.7  1995/07/05  21:39:27  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.6  1995/06/23  18:49:18  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  19:16:11  slin
 * CSCdi34668:  Long packets are not checked.
 *
 * Revision 2.4  1995/06/21  18:06:56  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.3  1995/06/20  07:18:06  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:55:19  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../os/signal.h"
#include "interface.h"
#include "packet.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "mgd_timers.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

/*
 *------------------------------------------------------------------
 * Externals
 *------------------------------------------------------------------
 */
extern pid_t smrp_bg_pid;

/*
 *==================================================================
 * Module Private Definitions
 *==================================================================
 */
#define smrp_BackgroundMasterTimer (&smrptrans_globals->MasterTimer)
#define smrp_GroupCreatorTimer     (&smrptrans_globals->GroupCreatorTimer)
#define smrp_GroupMemberTimer      (&smrptrans_globals->GroupMemberTimer)
#define smrp_RequestTimer          (&smrptrans_globals->RequestTimer)
#define smrp_ResponseTimer         (&smrptrans_globals->ResponseTimer)

/*
 *==================================================================
 * Module private data and functions
 *==================================================================
 */

static ushort smrp_SequenceNumber;

static queuetype smrpRequestQ;
static queuetype smrpFreeReqQ;
static queuetype smrpResponseQ;
static queuetype smrpFreeResQ;

static struct smrptrans_globals_ {
    mgd_timer MasterTimer;	/* Master timer for background process */
    mgd_timer RequestTimer;	/* Timer for handling queued requests */
    mgd_timer ResponseTimer;	/* Timer for handling queued responses */
    mgd_timer GroupMemberTimer;	/* Timer for sending member queries */
    mgd_timer GroupCreatorTimer; /* Timer for sending creator queries */
} *smrptrans_globals;

ulong* SMRPTRANS_stat;

static SMRPResponse *smrp_FindResponse(ushort, UnicastAddr);
static void smrp_DispatchResponse(SMRPPort *, SMRPResponse *, uchar);
static void smrp_ResendResponse(SMRPResponse *);
static SMRPResponse *smrp_CreateResponse(SMRPPort *, paktype *, uchar,
					 SMRPGroupID, UnicastAddr, ushort);

void
smrp_SetupCreatorTimer (SMRPGroup *grp)
{
    mgd_timer_init_leaf(&grp->CreatorTimer, smrp_GroupCreatorTimer,
			SMRP_GROUPCREATOR_TIMER, grp, FALSE);
}

void
smrp_SetupMemberTimer (SMRPForward *fwd)
{
    mgd_timer_init_leaf(&fwd->MemberTimer, smrp_GroupMemberTimer,
			SMRP_GROUPMEMBER_TIMER, fwd, FALSE);
}

static inline void
smrp_StartRequestTimer (SMRPRequest *req)
{
    mgd_timer_start(&req->Timer, req->RetransInterval);
}

/*
 *------------------------------------------------------------------
 * smrp_NextSequenceNumber		(Module Inline Function)
 *
 * Return the next non-zero sequence number for our node.  The
 * sequence number of zero is used for non-transaction packets (Hello
 * packets) and all datagrams.
 *------------------------------------------------------------------
 */
static inline ushort
smrp_NextSequenceNumber (void)
{
    if (++smrp_SequenceNumber == 0) 
	smrp_SequenceNumber = 1;
    return smrp_SequenceNumber;
}

/*
 *------------------------------------------------------------------
 * smrp_PrintTransaction		(SMRP Internal Routine)
 *
 * Print transaction based on packet type.
 *------------------------------------------------------------------
 */
static void
smrp_PrintTransaction (SMRPPort* port, boolean sender, uchar pt, ushort seq,
    SMRPGroupID grp, UnicastAddr addr)
{
    uchar cmd;
    uchar pktype;
    char buf[30], buf2[30];

    cmd = smrp_GetPacketModifiers(pt);
    pktype = smrp_GetPacketType(pt);

    switch (pktype) {
    case MCHello:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Transaction for port %s, %s %s (grp %s) %s %s",
	        PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
                smrp_PrintPacketType(pktype),
                smrp_PrintCmdType(cmd),
                smrp_DisplayGroupAddr(grp),
                sender?"sent to":"received from",
	        PORTCB_PrintNetAddress(port, buf2, addr));
        break;

    case MCPrimaryNode:
    case MCSecondaryNode:
    case MCTunnelNode:
    case MCLocateNode:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)
            && smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Transaction for port %s, %s %s (seq %d) %s %s",
		   PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
		   smrp_PrintPacketType(pktype),
		   smrp_PrintCmdType(cmd),
		   seq,
		   sender?"sent to":"received from",
		   PORTCB_PrintNetAddress(port, buf2, addr));
        break;

    case MCDistanceVector:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)
            && smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Transaction for port %s, %s %s (seq %d) %s %s",
		   PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
		   smrp_PrintPacketType(pktype),
		   smrp_PrintCmdType(cmd),
		   seq,
		   sender?"sent to":"received from",
		   PORTCB_PrintNetAddress(port, buf2, addr));
        break;

    case MCGroupCreator:
    case MCGroupMember:
    case MCCreateGroup:
    case MCDeleteGroup:
    case MCJoinGroup:
    case MCLeaveGroup:
    case MCAddEntry:
    case MCRemoveEntry:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)
            && smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Transaction for group %s on port %s, %s %s (seq %d) %s %s",
		   smrp_DisplayGroupAddr(grp),
		   PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
		   smrp_PrintPacketType(pktype),
		   smrp_PrintCmdType(cmd),
		   seq,
		   sender?"sent to":"received from",
		   PORTCB_PrintNetAddress(port, buf2, addr));
        break;

    case MCNotifyNode:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)
            && smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Transaction for port %s, %s %s (seq %d) %s %s",
	        PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
                smrp_PrintPacketType(pktype),
                smrp_PrintCmdType(cmd),
                seq,
                sender?"sent to":"received from",
	        PORTCB_PrintNetAddress(port, buf2, addr));
        break;
    default:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK))
	    buginf("\nSMRP: Unrecognized transaction for port %s, "
		   "paktype %x, %s %s",
		   PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
		   pktype, sender ? "sent to" : "received from",
		   PORTCB_PrintNetAddress(port, buf2, addr));
	break;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_FirstRequest			(Module Inline Function)
 *
 * Return the first entry from the request queue or NULL if none.
 *------------------------------------------------------------------
 */
inline SMRPRequest*
smrp_FirstRequest (void)
{
    return (SMRPRequest*) smrpRequestQ.qhead;
}

/*
 *------------------------------------------------------------------
 * smrp_GetFreeRequest			(Module Inline Function)
 *
 * Return the first entry from the request free list or NULL if 
 * none.  The entry is removed from the free list.
 *------------------------------------------------------------------
 */
static inline SMRPRequest*
smrp_GetFreeRequest (void)
{
    return dequeue(&smrpFreeReqQ);
}

/*
 *------------------------------------------------------------------
 * smrp_SetRequestState			(Module Inline Function)
 *
 * Set the status field of a request.
 *------------------------------------------------------------------
 */
static inline void
smrp_SetRequestState (SMRPRequest* req, RequestState state)
{
    SMRPASSERT(req != NULL);
    req->State = state;
}
/*
 *------------------------------------------------------------------
 * smrp_FindRequest			(Module Internal Function)
 *
 * Find a request given it's sequence number.
 *------------------------------------------------------------------
 */
static SMRPRequest*
smrp_FindRequest (ushort seqNum, uchar pt)
{
    SMRPRequest* req = smrp_FirstRequest();
    while (req != NULL) {
	if (req->SequenceNumber == seqNum) {
            if (smrp_GetPacketType(pt) == smrp_GetPacketType(req->PacketType))
	        return req;
            else {
                smrp_Error("smrp_FindRequest, unmatched packet type");
	        return NULL;
	    }
	}
	req = req->next;
    }
    return NULL;
}

SMRPRequest *
smrp_FindRequestByPort (SMRPPortID pid, uchar pt)
{
    SMRPRequest *req = smrp_FirstRequest();
    while (req != NULL) {
	if (req->PortID == pid) {
	    if (smrp_GetPacketType(pt) == smrp_GetPacketType(req->PacketType))
		return req;
	}
	req = req->next;
    }
    return NULL;
}

SMRPRequest *
smrp_FindRequestByGroupAndType (SMRPGroupID gid, uchar pt)
{
    SMRPRequest *req = smrp_FirstRequest();
    while (req != NULL) {
	if (smrp_GetPacketType(pt) == smrp_GetPacketType(req->PacketType))
	    if (gid == req->GroupAddress)
		return req;
	req = req->next;
    }
    return NULL;
}
	
/*
 *------------------------------------------------------------------
 * smrp_SendRequest			(SMRP Internal Function)
 *
 * Send out a packet for the request.  Note: State does not change
 * to in progress until we successfully send a packet out.
 *------------------------------------------------------------------
 */
static boolean
smrp_SendRequest (SMRPRequest* req)
{
    paktype* pak;
    SMRPPort *port;
    
    SMRPASSERT(req != NULL);
    SMRPASSERT(req->State != MCRequestComplete);
    SMRPASSERT(req->PacketData != NULL);

    port = smrp_FindPort(req->PortID);
    if (!port || port->State == MCNotInitialized)
	return FALSE;
    
    smrp_SetupHeader(req->PacketData,req->PacketType,req->SequenceNumber,
		     req->GroupAddress);
    pak = smrp_CreatePacketWithData(port, req->PacketData,req->DataSize);
    if (pak == NULL) {
	return FALSE;
    }
    smrp_SetRequestState(req, MCRequestInProgress);
    req->Attempts++;
    GET_TIMESTAMP(req->LastAction);
    mgd_timer_start(&req->Timer, req->RetransInterval);
    
    if (smrp_IsSpecialMulticastID(req->Destination)) {
	PORTCB_SendMCPacket(port, pak,
			    smrp_MapSpecialAddrToGroupID(req->Destination));
    } else {
	PORTCB_SendPacket(port,pak, req->Destination);
    }
    smrp_stat[SMRP_REQUESTS_OUT]++;
    
    smrp_PrintTransaction(port,TRUE,req->PacketType,req->SequenceNumber,
		     req->GroupAddress,req->Destination);

    return TRUE;
}
/*
 *------------------------------------------------------------------
 * smrp_CreateRequest			(SMRP Internal Function)
 *
 * Create a request with the specified information.
 *------------------------------------------------------------------
 */
boolean
smrp_CreateRequest (SMRPPort* port, uchar pt, SMRPGroupID grp, 
		    UnicastAddr dest, void* pdata, uint psize,
		    ulong RetransTimeoutInterval, ushort MaxRetries,
		    smrpReqProc doneProc)
{
    SMRPRequest* req;

    SMRPASSERT(port != NULL);

    if (QUEUEEMPTY(&smrpFreeReqQ)) {
	req = malloc(sizeof(SMRPRequest));
	if (req == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    return FALSE;
	}
	mgd_timer_init_leaf(&req->Timer, smrp_RequestTimer,
			    SMRP_REQUEST_TIMER, req, FALSE);
	
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_NEW]++;
    } else {
	req = smrp_GetFreeRequest();
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_RECYCLED]++;
    }
    req->GroupAddress = grp;
    req->PacketType = pt;
    req->Destination = dest;
    req->CompletionProc = doneProc;
    req->PortID = port->PortID;
    req->State = MCRequestPending;
    req->Attempts = 0;
    req->MaxAttempts = MaxRetries ? MaxRetries : smrp_requestRetries;
    req->RetransInterval = RetransTimeoutInterval ? RetransTimeoutInterval
	: smrp_requestInterval;
    req->SequenceNumber = smrp_NextSequenceNumber();
    req->DataSize = SMRP_HEADER_SIZE + psize;
    if (req->DataSize <= SMRP_REQ_BUFFERSIZE) {
	req->PacketData = (MCHeader*) req->RequestDataBuffer;
	req->DataAllocated = FALSE;
    } else {
	req->PacketData = malloc(SMRP_HEADER_SIZE + psize);
	req->DataAllocated = TRUE;
    }
    if (req->PacketData == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	enqueue(&smrpFreeReqQ,req);
	return FALSE;
    }
    if (psize > 0)
	bcopy(pdata,(uchar *) req->PacketData + SMRP_HEADER_SIZE, psize);
    req->ResponsePacket = NULL;
    req->ResponseData = NULL;
    req->ResponseSize = 0;
    GET_TIMESTAMP(req->LastAction);

    smrp_StartRequestTimer(req);
        
    enqueue(&smrpRequestQ,req);
    smrp_SendRequest(req);
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DiscardRequest			(SMRP Internal Function)
 *
 * Discard a request by freeing resources and queueing it on the
 * free queue.
 *------------------------------------------------------------------
 */
void
smrp_DiscardRequest (SMRPRequest* req)
{
    SMRPASSERT(req != NULL);
    unqueue(&smrpRequestQ,req);

    mgd_timer_stop(&req->Timer);

    if (req->ResponsePacket != NULL) {
	smrp_Dispose(-1,req->ResponsePacket);
	req->ResponsePacket = NULL;
    } else if (req->ResponseData != NULL) {
        /* Timed out requests cleaned up here */
	free(req->ResponseData); 
        req->ResponseData = NULL;
    }
    if (req->DataAllocated) {
	free(req->PacketData);
	req->PacketData = NULL;
	req->DataAllocated = FALSE;
    }
    if ((smrpFreeReqQ.count >= smrpFreeReqQ.maximum) && 
	(smrpFreeReqQ.maximum != 0)) {
	free(req);
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_FREED]++;
    } else {
	enqueue(&smrpFreeReqQ,req);
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_DELETED]++;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_HandleRequest			(SMRP Internal Routine)
 *
 * Process an incoming request packet by creating a response entry
 * and dispatching to the handler.
 *
 * NOTE: This routine assumes responsibility for the packet.  Do
 *       not dispose or access the packet after calling this 
 *       routine.
 *------------------------------------------------------------------
 */
void
smrp_HandleRequest (SMRPPort* port, paktype* pak, uchar pt, 
		    SMRPGroupID grp, UnicastAddr src, ushort seq)
{
    SMRPResponse* res;
    
    SMRPASSERT(port != NULL);
    SMRPASSERT(pak != NULL);

    SMRPASSERT(smrp_GetPacketModifiers(pt) == MCRequestCommand);

    smrp_PrintTransaction(port,FALSE,pt,seq,grp,src);

    res = smrp_FindResponse(seq,src);
    if (res != NULL) {
	if (res->State != MCRequestComplete) {
	    if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)) 
		buginf("\nSMRP: Discarded incompleted request");
	    smrp_Dispose(-1,pak);
  	    SMRPTRANS_stat[SMRPTRANS_REQUESTS_DUP_INCOMPLETE]++;
	    return;
	}
	if (res->ResponseType)
	    smrp_ResendResponse(res);
	smrp_Dispose(-1,pak);
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_DUP]++;
	return;
    }

    smrp_stat[SMRP_REQUESTS_IN]++;
    res = smrp_CreateResponse(port,pak,pt, grp,src,seq);
    if (res == NULL) {
	smrp_Error("Cannot create response - no memory");
	smrp_Dispose(-1,pak);
	return;
    }
/*
 * At this point, the packet is associated with the response entry
 * and it will be automagically freed when the request completes or
 * sometimes sooner.  Do not attempt to manually dispose of the 
 * packet beyond here [only use smrp_FreeResponseResources()].
 */
    if (res->State == MCRequestPending) {
        /* TESTMODE - Intercept requests */
        if (smrp_test_running) {
            reg_invoke_smrp_intercept_req_pkt(port,res,pt);
            return;
        }
	smrp_DispatchResponse(port,res,pt);
    } else
	if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)) 
	    buginf("\nSMRP: Response in wrong state in HandleRequest");
}
/*
 *------------------------------------------------------------------
 * smrp_TimeoutRequest			(Module Internal Function)
 *
 * Generate a reject response for an old unanswered request.
 *------------------------------------------------------------------
 */
static void
smrp_TimeoutRequest (SMRPRequest* req)
{
    MCHeader *hdr;
    SMRPPort* port = smrp_FindPort(req->PortID);

    if (port == NULL || port->State == MCNotInitialized) {
	smrp_DiscardRequest(req);
	return;
    }
    
    hdr = malloc(SMRP_HEADER_SIZE + SMRP_REJECT_SIZE);
    if (!hdr) {
	smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }
    
    if (req->CompletionProc != NULL) {
	smrp_SetupHeader(hdr,
			 smrp_GetPacketType(req->PacketType) + 
			 MCNegativeResponse,
			 req->SequenceNumber, req->GroupAddress);
	PUTSHORT(smrp_datastart_offset(hdr), MCNoResponse);
	req->ErrorIndication = MCNoResponse;
	req->Responder = port->ThisNodeAddress;
	smrp_SetRequestState(req,MCRequestComplete);

	req->ResponsePacket = NULL;
	req->ResponseData = hdr;
	req->ResponseSize = SMRP_HEADER_SIZE + SMRP_REJECT_SIZE;
	req->ResponseType = hdr->PacketType;
	GET_TIMESTAMP(req->LastAction);

	(*req->CompletionProc)(req);
    }
    smrp_DiscardRequest(req);
}

static void
smrp_HandleRequestTimeout (SMRPRequest *req)
{
    SMRPASSERT(req != NULL);
    
    if (req->Attempts > req->MaxAttempts) {
	smrp_TimeoutRequest(req);
	SMRPTRANS_stat[SMRPTRANS_REQUESTS_TIMEOUTS]++;
    } else {
	if (smrp_SendRequest(req))
	    SMRPTRANS_stat[SMRPTRANS_REQUESTS_RESENDS]++;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_FirstResponse			(Module Inline Function)
 *
 * Return the first entry from the response queue or NULL if none.
 *------------------------------------------------------------------
 */
inline SMRPResponse*
smrp_FirstResponse (void)
{
    return (SMRPResponse*) smrpResponseQ.qhead;
}


/*
 *------------------------------------------------------------------
 * smrp_GetFreeResponse			(Module Inline Function)
 *
 * Return the first entry from the response free list or NULL if 
 * none.  The entry is removed from the free list.
 *------------------------------------------------------------------
 */
static inline SMRPResponse*
smrp_GetFreeResponse (void)
{
    return dequeue(&smrpFreeResQ);
}

/*
 *------------------------------------------------------------------
 * smrp_HandleResponse			(SMRP Internal Routine)
 *
 * Process an incoming response packet by finding its associated
 * request entry.
 *
 * NOTE: This routine assumes responsibility for the packet.  Do
 *       not dispose or access the packet after calling this 
 *       routine.
 *------------------------------------------------------------------
 */
void
smrp_HandleResponse (SMRPPort* port, paktype* pak, uchar pt, 
		     SMRPGroupID grp, UnicastAddr src, ushort seq)
{
    SMRPRequest* req;
    
    SMRPASSERT(port != NULL);
    SMRPASSERT(pak != NULL);

    smrp_PrintTransaction(port,FALSE,pt,seq,grp,src);

    req = smrp_FindRequest(seq, pt);
    if (req == NULL) {
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_UNEXPECTED]++;
	smrp_Dispose(-1,pak);
	return;
    }

    /* TESTMODE - Create group request and response has diff group addr */
    if ((smrp_GetPacketType(req->PacketType) != smrp_GetPacketType(pt)) ||
	(req->GroupAddress != grp && !smrp_test_running)) {
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_BAD]++;
	smrp_Dispose(-1,pak);
	return;
    }
    /* TESTMODE - Set group addr to the response */
    if (smrp_test_running && pt == MCCreateConfirm) 
        req->GroupAddress = grp;

    req->State = MCRequestComplete;
    req->ResponseType = pt;
    GET_TIMESTAMP(req->LastAction);
    req->Responder = src;
    req->ResponsePacket = pak;
    req->ResponseData =(MCHeader*)pak->transport_start;
    req->ResponseSize = pak->datagramsize
	- (pak->transport_start - pak->datagramstart);
    if (smrp_GetPacketModifiers(req->ResponseType) == MCPositiveResponse) {
	req->ErrorIndication = MCNoError;
	smrp_stat[SMRP_CONFIRMS_IN]++;
    } else {
	if (req->ResponseSize != SMRP_HEADER_SIZE + SMRP_REJECT_SIZE) {
	    if (req->ResponseSize < SMRP_HEADER_SIZE + SMRP_REJECT_SIZE)
		smrp_stat[SMRP_RUNT]++;
	    smrp_DiscardRequest(req);
	    return;
	}
	
	req->ErrorIndication = GETSHORT(smrp_datastart(pak));
	smrp_stat[SMRP_REJECTS_IN]++;
    }
    if (req->CompletionProc != NULL) 
    	(*req->CompletionProc)(req);

    smrp_DiscardRequest(req);	/* also frees the packet */
}

/*
 *------------------------------------------------------------------
 * smrp_FindResponse			(Module Internal Function)
 *
 * Find the response queue entry with a matching sequence number
 * and source address.
 *------------------------------------------------------------------
 */
static SMRPResponse*
smrp_FindResponse (ushort seqNum, UnicastAddr src)
{
    SMRPResponse* res = smrp_FirstResponse();
    while (res != NULL) {
	if ((res->SequenceNumber == seqNum) && (src == res->Source))
	    return res;
	res = res->next;
    }
    return NULL;
}
/*
 *------------------------------------------------------------------
 * smrp_FreeResponseResources		(Module Internal Routine)
 *
 * Discard the resources associated with the response entry.  For
 * now, it disposes of the request packet and clears the associated
 * pointers.
 *------------------------------------------------------------------
 */
static void
smrp_FreeResponseResources (SMRPResponse* res)
{
    SMRPASSERT(res->State == MCRequestComplete);
    if (res->RequestPacket != NULL) {
	smrp_Dispose(-1,res->RequestPacket);
	res->RequestPacket = NULL;
	res->RequestData = NULL;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_DiscardResponse			(Module Internal Function)
 *
 * Discard a response by freeing resources and queueing it on the
 * free queue.
 *------------------------------------------------------------------
 */
void
smrp_DiscardResponse (SMRPResponse* res)
{
    SMRPASSERT(res != NULL);
    unqueue(&smrpResponseQ,res);
    smrp_FreeResponseResources(res);
    if ((smrpFreeResQ.count >= smrpFreeResQ.maximum) && 
	(smrpFreeResQ.maximum != 0)) {
	free(res);
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_FREED]++;
    } else {
	enqueue(&smrpFreeResQ,res);
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_DELETED]++;
    }
}
/*
 *------------------------------------------------------------------
 * smrp_DispatchResponse		(Module Internal Function)
 *
 * Dispatch to the appropriate handler for the request.
 *------------------------------------------------------------------
 */
static void
smrp_DispatchResponse (SMRPPort* port, SMRPResponse* res, uchar pt)
{
    char buf[80];
    
    smrp_SetResponseState(res,MCRequestInProgress);
    switch (pt) {
    case MCPrimaryRequest:
	smrp_DoPrimaryRequestPacket(port,res);
	break;
    case MCSecondaryRequest:
	smrp_DoSecondaryRequestPacket(port,res);
	break;
    case MCJoinRequest:
	smrp_DoJoinRequestPacket(port,res);
	break;
    case MCDeleteRequest:
	smrp_DoDeleteRequestPacket(port,res);
	break;
    case MCCreateRequest:
	smrp_DoCreateRequestPacket(port,res);
	break;
    case MCLeaveRequest:
	smrp_DoLeaveRequestPacket(port,res);
	break;
    case MCDistanceRequest:
	smrp_DoDistanceVectorRequest(port,res);
	break;
    case MCMemberRequest:
	smrp_DoMemberRequestPacket(port,res);
	break;
    case MCAddRequest:
	smrp_DoAddRequestPacket(port,res);
	break;
    case MCRemoveRequest:
	smrp_DoRemoveRequestPacket(port,res);
	break;
    case MCNotifyRequest:
	smrp_DoNotifyRequestPacket(port, res);
	break;
    case MCLocateRequest:
	smrp_DoLocateRequestPacket(port, res);
	break;
    case MCTunnelRequest:
	smrp_DoTunnelRequestPacket(port, res);
	break;
    default:
        /* transaction debugging */
        if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)) {
	    sprintf(buf, "SMRP bad packet type %x", pt);
            errmsg(&msgsym(DEBUGMSG, SMRP), buf);
	}
	
       	SMRPTRANS_stat[SMRPTRANS_REQUESTS_UNEXPECTED]++;
	res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
	break;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_SetResponseState		(SMRP Internal Routine)
 *
 * Set the state of the response entry.  If the state is changing
 * to complete, free any resources it has.
 *------------------------------------------------------------------
 */
void
smrp_SetResponseState (SMRPResponse* res, RequestState state)
{
    if (res->State != state) {
	res->State = state;
	if (state == MCRequestComplete)
	    smrp_FreeResponseResources(res);
    }
}

/*
 *------------------------------------------------------------------
 * smrp_CreateResponse			(SMRP Internal Function)
 *
 * Create a response entry with the specified information.  The
 * request packet is held until processing is completed and then
 * its released.  All request fields are invalid once the state
 * is MCRequestComplete.
 *------------------------------------------------------------------
 */
static SMRPResponse*
smrp_CreateResponse (SMRPPort* port, paktype* pak, uchar pt, 
		     SMRPGroupID grp, UnicastAddr src, ushort seq)
{
    SMRPResponse* res;

    SMRPASSERT(port != NULL);
    SMRPASSERT(pak != NULL);

    if (QUEUEEMPTY(&smrpFreeResQ)) {
	res = malloc(sizeof(SMRPResponse));
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_NEW]++;
    } else {
	res = smrp_GetFreeResponse();
	SMRPTRANS_stat[SMRPTRANS_RESPONSES_RECYCLED]++;
    }
    if (res == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return NULL;
    }
    memset(res, 0,sizeof(SMRPResponse));
    res->GroupAddress = grp;
    res->PacketType = pt;
    res->Source = src;
    res->PortID = port->PortID;
    res->State = MCRequestPending;
    res->SequenceNumber = seq;
    res->RequestPacket = pak;
    res->RequestData = smrp_datastart(pak);
    res->RequestSize = pak->datagramsize - (res->RequestData -
					    pak->datagramstart);
    res->ResponseSize = 0;
    GET_TIMESTAMP(res->LastAction);
    if (!mgd_timer_running(smrp_ResponseTimer))
	mgd_timer_start(smrp_ResponseTimer, smrp_responseInterval);
    enqueue(&smrpResponseQ,res);
    return res;
}

/*
 *------------------------------------------------------------------
 * smrp_SendResponse			(Module Internal Function)
 *
 * Create and send out a response packet for a completed response 
 * entry.
 *------------------------------------------------------------------
 */
static boolean
smrp_SendResponse (SMRPPort* port, SMRPResponse* res)
{
    paktype* pak;
    SMRPASSERT(port != NULL);
    SMRPASSERT(res->State == MCRequestComplete);

    if (res->ResponseSize <= SMRP_HEADER_SIZE)
	res->ResponseSize = SMRP_HEADER_SIZE;

    smrp_SetupHeader((MCHeader *) res->ResponseBuffer, res->ResponseType,
		     res->SequenceNumber, res->GroupAddress);

    pak = smrp_CreatePacketWithData(port, (MCHeader *) res->ResponseBuffer, 
				    res->ResponseSize);
    if (pak == NULL) {
	return FALSE;
    }
    GET_TIMESTAMP(res->LastAction);
    PORTCB_SendPacket(port,pak, res->Source);

    smrp_PrintTransaction(port,TRUE,res->ResponseType,res->SequenceNumber,
		     res->GroupAddress,res->Source);

    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_SendConfirmResponse		(SMRP Internal Function)
 *
 * Complete processing for this entry with no error and then 
 * generate a response.
 *------------------------------------------------------------------
 */
void
smrp_SendConfirmResponse (SMRPResponse* res)
{
    SMRPPort* port = smrp_FindPort(res->PortID);
    SMRPASSERT(res->State != MCRequestComplete);

    if (port == NULL || port->State == MCNotInitialized) {
	smrp_Error("smrp_SendResponse, bad port id in response entry");
	return;
    }
    res->ErrorIndication = MCNoError;
    res->ResponseType = smrp_GetPacketType(res->PacketType) + 
	MCPositiveResponse;
    smrp_SetResponseState(res,MCRequestComplete);
    smrp_SendResponse(port,res);
    smrp_stat[SMRP_CONFIRMS_OUT]++;
}

/*
 *------------------------------------------------------------------
 * smrp_SendRejectResponse		(SMRP Internal Function)
 *
 * Complete processing for this entry with the specified error and 
 * then generate a response.
 *------------------------------------------------------------------
 */
void
smrp_SendRejectResponse (SMRPResponse* res, short reason)
{
    SMRPPort* port = smrp_FindPort(res->PortID);
    SMRPASSERT(res->State != MCRequestComplete);

    if (port == NULL) {
	smrp_Error("smrp_SendResponse, bad port id in response entry");
	return;
    }
    res->ErrorIndication = reason;
    res->ResponseType = smrp_GetPacketType(res->PacketType) + 
	MCNegativeResponse;
    PUTSHORT(res->ResponseBuffer + SMRP_HEADER_SIZE, reason);
    res->ResponseSize = SMRP_HEADER_SIZE + SMRP_REJECT_SIZE;
    smrp_SetResponseState(res,MCRequestComplete);
    smrp_SendResponse(port,res);
    smrp_stat[SMRP_REJECTS_OUT]++;
}

/*
 *------------------------------------------------------------------
 * smrp_ResendResponse			(SMRP Internal Function)
 *
 * Regenerate a previously completed response.
 *------------------------------------------------------------------
 */
static void
smrp_ResendResponse (SMRPResponse* res)
{
    SMRPPort* port = smrp_FindPort(res->PortID);
    SMRPASSERT(res->State == MCRequestComplete);

    if (port == NULL || port->State == MCNotInitialized) {
	smrp_Error("smrp_SendResponse, bad port id in response entry");
	return;
    }
    smrp_SendResponse(port,res);
}

void
smrp_SendResponseByGroupAndType (SMRPGroupID gid, uchar pt,
				 enum ErrorValues errcode)
{
    SMRPResponse *res, *nextres;

    res = nextres = smrp_FirstResponse();

    if (res == NULL)
	return;
    while (nextres != NULL) {
	res = nextres;
	nextres = res->next;
	if (res->State == MCRequestComplete)
	    continue;
	if (res->GroupAddress != gid
	    || smrp_GetPacketType(res->PacketType) != smrp_GetPacketType(pt))
	    continue;
	switch (smrp_GetPacketModifiers(pt)) {
	case MCPositiveResponse:
	    smrp_SendConfirmResponse(res);
	    break;
	case MCNegativeResponse:
	    smrp_SendRejectResponse(res, errcode);
	    break;
	}
    }
}

/*
 *------------------------------------------------------------------
 * smrp_ScanResponses			(SMRP Internal Function)
 *
 * Scan the response queue and perform any necessary operations. 
 * Returns FALSE if no responses were pending.
 *------------------------------------------------------------------
 */
static boolean
smrp_ScanResponses (void)
{
    SMRPResponse* nextRes =  smrp_FirstResponse();
    SMRPResponse* res = nextRes;

    if (res == NULL)
	return FALSE;
    while (nextRes != NULL) {
	res = nextRes;
	nextRes = res->next;
	if (CLOCK_OUTSIDE_INTERVAL(res->LastAction,smrp_responseInterval)) {
            res->State = MCRequestComplete;
	    smrp_DiscardResponse(res);
	}
    }
    return TRUE;
}

boolean
smrp_ResponseIsConfirmation (SMRPRequest *req)
{
    return (smrp_GetPacketModifiers(req->ResponseType) == MCPositiveResponse);
}

static void
smrp_bg_do_timers (void)
{
    mgd_timer *BackgroundTimer = smrp_BackgroundMasterTimer;
    
    while (mgd_timer_expired(BackgroundTimer)) {
	mgd_timer *expired_timer;

	expired_timer = mgd_timer_first_expired(BackgroundTimer);
	switch (mgd_timer_type(expired_timer)) {
	case SMRP_REQUEST_TIMER:
	    mgd_timer_stop(expired_timer);
	    smrp_HandleRequestTimeout(mgd_timer_context(expired_timer));
	    break;
	case SMRP_RESPONSE_TIMER:
	    if (smrp_ScanResponses())
		mgd_timer_update(expired_timer,
				 smrp_responseInterval);
	    else
		mgd_timer_stop(expired_timer);
	    break;
	case SMRP_GROUPMEMBER_TIMER:
	    smrp_HandleMemberTimer(mgd_timer_context(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	case SMRP_GROUPCREATOR_TIMER:
	    smrp_HandleCreatorTimer(mgd_timer_context(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	default:
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

static void
smrp_bg_do_packets (void)
{
    paktype *pak;
    SMRPPort* port;
    SMRPGroupID grp;
    UnicastAddr src;
    ushort seq;
    uchar pt;

    while ((pak = process_dequeue(smrpbgQ)) != NULL) {

	port = smrp_FindPortFromPak(pak);
	if (port == NULL) {
	    if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)) 
		buginf("\nSMRP: Packet discarded, Bad Port ID");
	    smrp_Dispose(SMRP_BAD_PORTID, pak);
	    continue;
	}

	if (!PORTCB_CheckPacket(port,pak)) {
	    if (smrp_debug(smrp_trans_debug, SMRP_NULL_MASK)) 
		buginf("\nSMRP: Packet discarded, Check Packet failed");
	    smrp_stat[SMRP_INPUT_ERR]++;
	    continue;
	}

	pt = *(pak->transport_start + SMRP_PAKTYPE_OFFSET);
	seq = GETSHORT(pak->transport_start + SMRP_SEQNUM_OFFSET);
	grp = GETLONG(pak->transport_start + SMRP_GROUPADDR_OFFSET);
	src = PORTCB_GetPacketSource(port,pak);

	if ((smrp_GetPacketModifiers(pt) == MCPositiveResponse) ||
	    (smrp_GetPacketModifiers(pt) == MCNegativeResponse)) {
	    smrp_HandleResponse(port,pak,pt,grp,src,seq);
	    /* packet is freed by smrp_HandleResponse */
	    continue;
	}
	if (smrp_GetPacketModifiers(pt) == MCRequestCommand) {
	    smrp_HandleRequest(port,pak,pt,grp,src,seq);
	    /* packet is freed by smrp_HandleRequest */
	    continue;
	}
	smrp_Error("smrp_background, Unexpected packet encountered");
	smrp_Dispose(SMRP_UNEXPECTED_PKT,pak);
    }
}

static void
smrp_bg_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(smrp_BackgroundMasterTimer, DISABLE);
    process_watch_queue(smrpbgQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(smrpbgQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&smrpbgQ);
    smrp_bg_pid = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_background			(SMRP Background Process)
 *
 * This process handles request and response queues and all
 * non-routing transaction packets.
 *------------------------------------------------------------------
 */
process
smrp_background (void)
{
    ulong major, minor;

    /*
     * Startup
     */
    signal_permanent(SIGEXIT, smrp_bg_teardown);
    smrpbgQ = create_watched_queue("SMRP Transaction packets", 0, 0);
    process_watch_queue(smrpbgQ, ENABLE, RECURRING);
    process_watch_mgd_timer(smrp_BackgroundMasterTimer, ENABLE);

    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch(major) {
	    case TIMER_EVENT:
		smrp_bg_do_timers();
		break;
	    case QUEUE_EVENT:
		smrp_bg_do_packets();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }		
}
/*
 *------------------------------------------------------------------
 * smrp_trans_Initialize		(Module Initialization)
 *
 * Perform initialization of module private data.
 *------------------------------------------------------------------
 */
void
smrp_trans_Initialize (void)
{
    int i;

    smrp_SequenceNumber = smrp_Random(0);

    smrptrans_globals = malloc(sizeof(struct smrptrans_globals_));
    if (smrptrans_globals == NULL) {
        smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }

    mgd_timer_init_parent(smrp_BackgroundMasterTimer, NULL);
    mgd_timer_init_parent(smrp_GroupMemberTimer, smrp_BackgroundMasterTimer);
    mgd_timer_init_parent(smrp_GroupCreatorTimer, smrp_BackgroundMasterTimer);
    mgd_timer_init_parent(smrp_RequestTimer, smrp_BackgroundMasterTimer);
    mgd_timer_init_leaf(smrp_ResponseTimer, smrp_BackgroundMasterTimer,
			SMRP_RESPONSE_TIMER, NULL, FALSE);
    queue_init(&smrpRequestQ, 0);
    queue_init(&smrpFreeReqQ, 0);
    queue_init(&smrpFreeResQ, 0);
    queue_init(&smrpResponseQ, 0);

    SMRPTRANS_stat = malloc(sizeof(ulong) * SMRPTRANS_MAXTRAFFIC);
    if (SMRPTRANS_stat == NULL) {
        return;
    }

    for (i=0; i < SMRPTRANS_MAXTRAFFIC; i++)
	SMRPTRANS_stat[i] = 0;
}
