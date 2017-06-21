/* $Id: smrp_test.c,v 3.7.4.3 1996/06/28 23:25:59 hampton Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_test.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP test commands.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_test.c,v $
 * Revision 3.7.4.3  1996/06/28  23:25:59  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.4.2  1996/05/21  10:01:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.1  1996/03/18  21:47:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  07:29:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/07  10:42:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  17:11:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/09  22:57:27  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.6  1996/02/08  23:10:49  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.5  1996/02/07  16:15:21  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1995/12/17  18:37:00  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:57:14  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:54:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/06  02:06:22  kleung
 * CSCdi41656:  Enhance SMRP test mode for sending/receiving data.
 *
 * Revision 2.8  1995/08/01  00:23:10  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.7  1995/07/25  21:29:34  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.6  1995/07/05  21:39:24  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.5  1995/06/28  09:30:29  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/20  07:18:05  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/14  22:55:18  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.2  1995/06/09  13:18:49  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:45:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "../if/network.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "plural.h"

#include "../atalk/atalk_private.h"

#include "smrp_private.h"
#include "smrp_test.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_smrp.h"
#include "../parser/parser_defs_parser.h"

#include "smrp_registry.h"

/*
 * SMRP subsystem header
 */

#define SMRPTEST_MAJVERSION 1
#define SMRPTEST_MINVERSION 0
#define SMRPTEST_EDITVERSION  1

SUBSYS_HEADER(smrptest, SMRPTEST_MAJVERSION, SMRPTEST_MINVERSION, SMRPTEST_EDITVERSION,
	      smrp_test_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: smrp", "req: smrp");

/*
 * Smrp TEST commands
 */
#define ALTERNATE NONE
#include        "exec_test_smrp.h"
LINK_POINT      (smrp_test_commands, ALTERNATE);
#undef  ALTERNATE

/*
 *------------------------------------------------------------------
 * Module private definitions
 *------------------------------------------------------------------
 */

/* global decl */
static boolean                         ReceiveDataFlag = 1;

/*
 *------------------------------------------------------------------
 * Module private data and functions
 *------------------------------------------------------------------
 */

/*
 * smrptest_ListenToTestAddr
 *
 * Listen to MC addresses.
 */
static void
smrptest_ListenToTestAddr (SMRPPort *port, GroupToMCfn mapper)
{
    uchar mcaddr[IEEEBYTES];
    hwidbtype *hwidb;
    SMRPTestGroup *group;
    idbtype* idb;
        
    if (!port || !(idb = port->NetworkReference->phys_idb))
	return;

    hwidb = idb->hwptr;
    group = TestGroupTable;
    
    while (group != NULL) {
        if (port->PortID != group->TestPortID) {
            group = group->Next;
            continue;
        }

	if (mapper(hwidb, group->TestGroupAddress, mcaddr)) {
	    if_multi_add(hwidb, idb, mcaddr);
	    buginf("\nSMRPTEST: Subscribed to group %s, %e", 
                smrp_DisplayGroupAddr(group->TestGroupAddress), mcaddr);
	}
        group = group->Next;
    }
}

/*
 * smrptest_FindTestGroup
 *
 * Find a group in the test group table.
 */
static boolean
smrptest_FindTestGroup (SMRPPortID pid, SMRPGroupID groupaddr)
{
    SMRPTestGroup* tg;

    tg = TestGroupTable;
    while (tg != NULL) {
        if (tg->TestGroupAddress == groupaddr && tg->TestPortID == pid) 
            return (TRUE);
        tg = tg->Next;
    }
    return (FALSE);
}
/*
 * smrptest_AddTestGroup
 *
 * Add a group to the test group table.
 */
static void
smrptest_AddTestGroup (SMRPPortID pid, SMRPGroupID groupaddr)
{
    SMRPTestGroup* grp, *tg;

    /* group exist already */
    if (smrptest_FindTestGroup(pid, groupaddr))
        return;

    /* create new group */
    grp = malloc(sizeof(SMRPTestGroup));
    if (grp == NULL) {
	return;
    }
    grp->TestPortID = pid;
    grp->TestGroupAddress = groupaddr;

    tg = TestGroupTable;
    if (tg == NULL) {
        TestGroupTable = grp;
        return;
    }

    while (tg->Next) 
        tg = tg->Next;
        
    tg->Next = grp;
}

/*
 * smrptest_DeleteTestGroup
 *
 * Delete a group from the test group table.
 */
static void
smrptest_DeleteTestGroup (SMRPPortID pid, SMRPGroupID groupaddr)
{
    SMRPTestGroup* prev, *cur;

    cur = TestGroupTable;
    if (cur == NULL) {
	smrp_Error("smrptest_DeleteTestGroup: Cannot delete group - empty table");
        return;
    }

    if (cur->TestGroupAddress == groupaddr && cur->TestPortID == pid) {
        TestGroupTable = cur->Next;
        free(cur);
        return;
    }

    while (cur) {
        prev = cur;
        cur = cur->Next;
        if (cur->TestGroupAddress == groupaddr && cur->TestPortID == pid) {
            prev->Next = cur->Next;
            free(cur);
            break;
        }
    }        

}

/*
 *------------------------------------------------------------------
 * smrptest_NextGroupID			(Module Internal Function)
 *
 * Return the next group ID.
 * If the next ID is not valid, wrap back to the first ID.
 *
 * This routine assumes that ranges are consecutive.
 *------------------------------------------------------------------
 */
static SMRPGroupID
smrptest_NextGroupID (SMRPGroupID last, SMRPNetwork start, SMRPNetwork end)
{
    SMRPNetwork net;
    SMRPSocket skt;
    SMRPGroupID nextgid;
    
    net = smrp_GroupNetFromID(last);
    skt = smrp_GroupSocketFromID(last);
    
    if (skt != MCSocketEnd)
	nextgid = smrp_MakeGroupID(net, skt + 1);
    else
	nextgid = smrp_MakeGroupID(net + 1, MCSocketStart);

    net = smrp_GroupNetFromID(nextgid);
    if (net < start || net > end)
	nextgid = smrp_MakeGroupID(start, MCSocketStart);

    return (nextgid);
}

/*
 * smrptest_CompleteRequest
 *
 * Completion procedure for closing out a request.
 */
static void
smrptest_CompleteRequest (SMRPRequest *req)
{
    SMRPPort *port;
    char addr[30];

    port = smrp_FindPort(req->PortID);
    if (!port)
	return;

    printf("\nSMRPTEST: %s - ", smrp_PrintPacketType(req->PacketType));

    if (smrp_ResponseIsConfirmation(req)) {
        printf("confirmed by %s", 
            PORTCB_PrintNetAddress(port, addr, req->Responder));

        /* Subscribe to MAC MC address if it's a Join Packet */
        if (req->ResponseType == MCJoinConfirm) {
            smrptest_AddTestGroup(port->PortID, req->GroupAddress);
            PORTCB_Subscribe(port, req->GroupAddress);
        }
        /* Unsubscribe if it's a Leave Packet */
        if (req->ResponseType == MCLeaveConfirm) {
            smrptest_DeleteTestGroup(port->PortID, req->GroupAddress);
            PORTCB_Unsubscribe(port, req->GroupAddress);
        }
        /* kleung - Display group address if it's a Create Packet */
        if (req->ResponseType == MCCreateConfirm) 
            printf(", new group is %s", 
                smrp_DisplayGroupAddr(req->GroupAddress));

    } else {
        printf("rejected by %s, error is \"%s\"", 
            PORTCB_PrintNetAddress(port, addr, req->Responder),
            smrp_ErrorMessage(req->ErrorIndication),
            req->ErrorIndication);
    }
    printf("\n");
    flush();
}


/*
 * smrptest_SendRequest
 *
 * Sends a request packet to the port.
 */
static void
smrptest_SendRequest (SMRPPort *port, uchar pt, SMRPGroupID group)
{
    smrp_CreateRequest(port, pt, group, AllNodesAddr,
		       NULL, 0, 0, 0, smrptest_CompleteRequest);
}

/*
 *------------------------------------------------------------------
 * smrptest_SetupHeader			(SMRP Internal Routine)
 *
 * Initializes an MCHeader data structure.
 *------------------------------------------------------------------
 */
static void
smrptest_SetupHeader (MCHeader* hdr, uchar ver, uchar pt, ushort seq, 
    SMRPGroupID grp)
{
    uchar *ptr;
    
    SMRPASSERT(hdr != NULL);

    ptr = (uchar *) hdr;
    
    *ptr = ver;
    *(ptr + SMRP_PAKTYPE_OFFSET) = pt;
    PUTSHORT(ptr + SMRP_SEQNUM_OFFSET, seq);
    PUTLONG(ptr + SMRP_GROUPADDR_OFFSET, grp);
}

/*
 * smrptest_SendTestPacket
 *
 * Create and send a specified packet on the port.
 */
static void
smrptest_SendTestPacket (parseinfo *csb)
{
    /* default header fields */
    uchar version = 1;
    uchar pt = 0;
    ushort seq = 0;
    ulong group = 0;
    char *data = '\0';

    int length = 0;
    paktype* pak;
    uchar* pp;
    idbtype* idb;
    SMRPPort* port;
    SMRPIdbsb *idbsb;

    /* get parsed info */
    if (GETOBJ(int,2))
        version = GETOBJ(int,3);
    if (GETOBJ(int,4))
        pt = GETOBJ(int,5);
    if (GETOBJ(int,6))
        seq = GETOBJ(int,7);
    if (GETOBJ(int,8)) 
        group = smrp_MakeGroupID(GETOBJ(int,9), GETOBJ(int,10));
    if (GETOBJ(int,11)) 
        group = GETOBJ(int,12);
    if (GETOBJ(int,13)) {
        data = GETOBJ(string,1);
        length = GETOBJ(int,14);
    }

    idb = GETOBJ(idb,1);
    idbsb = smrp_GetIdbsb(idb);
    if (idbsb == NULL)
	return;
    
    port = idbsb->atsmrp_port;
    
    pak = PORTCB_CreatePacket(port, SMRP_DATAHDR_SIZE + length);

    pp = pak->transport_start;
    smrptest_SetupHeader((MCHeader*)pp, version, pt, seq, group);

    pp += SMRP_HEADER_SIZE;

    bcopy(data, pp, length);

    PORTCB_SendMCPacket(port,pak,group);
}


/*
 *------------------------------------------------------------------
 * smrptest_AddVector			(Module TreeProc Routine)
 *
 * Add a distance vector for each route that qualifies.
 *------------------------------------------------------------------
 */
static void
smrptest_AddVector (SMRPNetwork start, SMRPNetwork end, ushort dist, 
    SMRPRouteOp *op) 
{
    op->DataSize += SMRP_VECTOR_SIZE;

    smrp_PutThreeBytes(op->DataPtr,start);
    op->DataPtr += 3;
    smrp_PutThreeBytes(op->DataPtr,end);
    op->DataPtr += 3;
    *op->DataPtr++ = dist;
    *op->DataPtr++ = 0;
}

/*
 * smrptest_SendDVPacket
 *
 * Create and send a Distance Vector packet on the port.
 */
static void
smrptest_SendDVPacket (parseinfo *csb)
{
    /* default values */
    int numroutes = 1;

    SMRPNetwork start;
    SMRPNetwork end;
    uchar dist;
    idbtype* idb;
    SMRPPort* port;
    int maxRt;
    UnicastAddr naddr;
    int rangesize;
    int i;
    int vectors;
    SMRPRouteOp op;
    SMRPIdbsb *idbsb;

    /* get parsed info */
    start = GETOBJ(int,2);
    end = GETOBJ(int,3);
    dist = GETOBJ(int,4);
    if (GETOBJ(int,5))
        numroutes = GETOBJ(int,5);
    reg_invoke_smrp_parse_netaddr(GETOBJ(int,6), GETOBJ(paddr,1),
            &naddr);
    idb = GETOBJ(idb,1);
    idbsb = smrp_GetIdbsb(idb);
    if (idbsb == NULL)
	return;
    
    port = idbsb->atsmrp_port;
    rangesize = end - start + 1;

    /* Max number of routes allowed in one packet */
    maxRt = (PORTCB_MaxPacketSize(port) - SMRP_HEADER_SIZE)
	     / SMRP_VECTOR_SIZE;

    if (!smrp_InitRouteOp(&op, maxRt * SMRP_VECTOR_SIZE)) 
	return;

    /* Send as many DV packets needed for all routes */
    while (numroutes) {
        if (numroutes > maxRt) 
            vectors = maxRt;
        else
            vectors = numroutes;

        for (i = 1; i <= vectors; i++) {
            printf ("\nSMRPTEST: range %d-%d hop %d", start, end, dist);
            smrptest_AddVector(start, end, dist, &op);
            start += rangesize;
            end += rangesize;
	}

        smrp_ResetRouteOpData(&op);
        smrp_CreateRequest(port, MCDistanceRequest, NullGroup,
		naddr, op.DataPtr, op.DataSize, 0, 0, NULL);

        printf("\nSMRPTEST: DV packet sent %d route%s", 
            ArgAndPlural(vectors,"","s"));

        numroutes -= vectors;

        /* Reset route buffer */
        op.DataPtr = op.DataArea;
        op.DataSize = 0;
    }
    printf("\n");
    flush();

    smrp_CleanupRouteOp(&op);
}

/*
 * smrptest_SendTunnelPacket
 *
 * Create and send a Distance Vector packet on the port.
 */
static void
smrptest_SendTunnelPacket (parseinfo *csb)
{
    ushort start;
    ushort end;
    uchar requester;
    uchar dist;
    idbtype* idb;
    SMRPPort* port;
    UnicastAddr naddr;
    char *namestr;
    char *data;
    int namelen;
    int len;
    SMRPIdbsb *idbsb;
    
    /* get parsed info */
    idb = GETOBJ(idb,1);
    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;
    
    start = (ushort)GETOBJ(int,2);
    end = (ushort)GETOBJ(int,3);
    requester = (uchar)GETOBJ(int,4);
    dist = (uchar)GETOBJ(int,5);
    reg_invoke_smrp_parse_netaddr(GETOBJ(int,6), GETOBJ(paddr,1), &naddr);
    namestr = GETOBJ(string,1);

    /* Allocate tunnel data size */
    data = malloc(sizeof(42));
    PUTSHORT(data, start);
    PUTSHORT(data, end);
    *data++ = requester;
    *data++ = dist;
    PUTLONG(data, 0);
    namelen = strlen(namestr);
    len = min(namelen,32);
    bcopy(data, namestr, len); 
    if (namelen < 32) 
        *data = '\0';

    port = idbsb->atsmrp_port;
    if (port)
        smrp_CreateRequest(port, MCTunnelRequest, NullGroup,
	    naddr, data, 10+len, 0, 0, NULL);
}

/*
 * smrptest_CreateData
 *
 * Create data buffer of specified size.
 */
static char*
smrptest_CreateData (int size)
{
    int i;
    char *content = "SMRP.";
    char *buf;
    char *p1, *p2;

    buf = malloc(sizeof(char) * size);
    p1 = buf;
    p2 = content;

    for (i=0; i<size; i++) {
        *p1++ = *p2++;
        if (*p2 == '.')
            p2 = content;        
    }

    return (buf);
}

/*
 * smrptest_SendData
 *
 * Create and send a data packet on the port.
 */
static void
smrptest_SendData (SMRPPort *port, SMRPGroupID group, int repeat, 
    int pause, int count, char* data, int length)
{
    paktype* pak;
    uchar* pp;
    int i;

    for (i=0; i<repeat; i++) {
        pak = PORTCB_CreatePacket(port, SMRP_DATAHDR_SIZE + length);

        pp = pak->transport_start;
        smrp_SetupHeader((MCHeader*)pp, MCMulticastDatagram, 0, group);

        pp += SMRP_DATAHDR_SIZE;
        bcopy(data, pp, length);

        if (pause && (i % count == 0))
            process_sleep_for(pause);
        PORTCB_SendMCPacket(port,pak,group);
    }
}


/*
 * smrptest_ReceiveData
 *
 * Receive data on the port.
 */
static boolean
smrptest_ReceiveData (SMRPPort *port, paktype* pak)
{
    MCHeader* hdr;
    SMRPGroupID gid;
    uchar* pp;
    char data[800];
    int len;
    char buf[30];

    hdr = (MCHeader *)pak->transport_start;
    gid = GETLONG(&hdr->GroupAddress);

    /* Go to data area */
    pp = (uchar*)hdr + SMRP_DATAHDR_SIZE;

    /* kleung - TODO put this in at_smrp.c to retrieve smrp data len */
    /* Print out data */
    len = min(pak->datagramsize - DDP_OVERHEADBYTES - SMRP_DATAHDR_SIZE, 799);
    bcopy(pp, data, len);
    data[len] = '\0';

    if (ReceiveDataFlag) {
	printf("\nSMRPTEST: Data received for group %s on %s - %s", 
            smrp_DisplayGroupAddr(gid), 
            PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress), 
            data);
	flush();
    }

    smrp_Dispose(-1, pak);

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrptest_DoGroupCreatorRequestPacket		(SMRP Internal Function)
 *
 * Just confirm all group creator requests.
 *------------------------------------------------------------------
 */
static void
smrptest_DoGroupCreatorRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    smrp_SendConfirmResponse(res);    
}

/*
 *------------------------------------------------------------------
 * smrptest_DoMemberRequestPacket		(SMRP Internal Function)
 *
 * Just confirm all member requests.
 *------------------------------------------------------------------
 */
static void
smrptest_DoMemberRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    if (smrptest_FindTestGroup(port->PortID, res->GroupAddress))
	smrp_SendConfirmResponse(res);
}

/*
 *------------------------------------------------------------------
 * smrptest_DoJoinRequestPacket		(SMRP Internal Function)
 *
 * Just confirm all join requests.
 *------------------------------------------------------------------
 */
static void
smrptest_DoJoinRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    smrp_SendConfirmResponse(res);    
}

/*
 *------------------------------------------------------------------
 * smrptest_DoLeaveRequestPacket		(SMRP Internal Function)
 *
 * Just confirm all leave requests.
 *------------------------------------------------------------------
 */
static void
smrptest_DoLeaveRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    smrp_SendConfirmResponse(res);    
}

/*
 *------------------------------------------------------------------
 * smrptest_DispatchResponse		(Module Internal Function)
 *
 * Dispatch to the appropriate handler for the request.
 *------------------------------------------------------------------
 */
static boolean
smrptest_DispatchResponse (SMRPPort* port, SMRPResponse* res, uchar pt)
{
    smrp_SetResponseState(res,MCRequestInProgress);
    switch (pt) {
    case MCPrimaryRequest:
    case MCSecondaryRequest:
    case MCCreateRequest:
    case MCAddRequest:
    case MCDeleteRequest:
    case MCRemoveRequest:
    case MCNotifyRequest:
    case MCLocateRequest:
    case MCTunnelRequest:
    case MCDistanceRequest:
	/* ignore */
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
	break;
    case MCJoinRequest:
	smrptest_DoJoinRequestPacket(port,res);
	break;
    case MCLeaveRequest:
	smrptest_DoLeaveRequestPacket(port,res);
	break;
    case MCMemberRequest:
	smrptest_DoMemberRequestPacket(port,res);
	break;
    case MCCreatorRequest:
        smrptest_DoGroupCreatorRequestPacket(port, res);
        break;
    default:
	buginf("SMRPTEST: No handler for packet type %", pt);
    }

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrptest_CreateGroups		(Module Internal Function)
 *
 * Send Create Group requests.
 *------------------------------------------------------------------
 */
static void
smrptest_CreateGroups(parseinfo *csb, SMRPPort *port)
{
    int i = 1;
    int num_groups = 1;

    /* Create one or multiple groups */
    if (GETOBJ(int,3)) 
        num_groups = GETOBJ(int,2);

    while (i++ <= num_groups) {
        smrptest_SendRequest(port, MCCreateRequest, NullGroup);
        process_sleep_for(10);
    }

}

/*
 *------------------------------------------------------------------
 * smrptest_DeleteGroups		(Module Internal Function)
 *
 * Send Delete Group requests.
 *------------------------------------------------------------------
 */
static void
smrptest_DeleteGroups(parseinfo *csb, SMRPPort *port)
{
    int i = 1;
    int num_groups = 1;
    SMRPGroupID group;
    SMRPNetwork start;
    SMRPNetwork end;

    group = smrp_MakeGroupID(GETOBJ(int,6), GETOBJ(int,7));
    start = GETOBJ(int,3);
    end = GETOBJ(int,4);
    
    /* Delete one or multiple groups */
    if (GETOBJ(int,5)) 
        num_groups = GETOBJ(int,2);

    while (i++ <= num_groups) {
        smrptest_SendRequest(port, MCDeleteRequest, group);
	group = smrptest_NextGroupID(group, start, end);
    }

}

#if 0
/*
 *------------------------------------------------------------------
 * smrptest_NotifyPorts		(Module Internal Function)
 *
 * Send Notify packets out all ports.
 *------------------------------------------------------------------
 */
static void
smrptest_NotifyPorts(treeLink *p, void *data)
{
    SMRPPort *port = (SMRPPort *)p;

    smrp_SetPortState(port, MCNotOperational);
    smrp_SendNotifyToPort(port);
}
#endif

/*
 * smrp_test_commands
 *
 */
void
smrp_test_commands(parseinfo *csb)
{
    SMRPPort* port = NULL;
    SMRPGroupID group;
    idbtype* idb;
    char* data;
    int NumToSend = 1;
    int DataSize;
    int PauseTime = 0;
    int PauseCnt = 1;
    SMRPIdbsb *idbsb;
    int WaitTime = 2000;
    
    if (!smrp_running) {
        printf("\n%% SMRP not running");
	return;
    }

    switch (csb->which) {

    case SMRP_TEST_CMD:
        idb = GETOBJ(idb,1);
	idbsb = smrp_GetIdbsb(idb);
	if (idbsb == NULL || (port = idbsb->atsmrp_port) == NULL) {
	    /* Data receive on/off doesn't need interface */
	    if (GETOBJ(int,1) != SMRP_TEST_DATA || !(GETOBJ(int,4))) {
		printf("\n%% SMRP not enabled on interface");
		return;
	    }
	}

        switch (GETOBJ(int,1)) {
        case SMRP_TEST_CREATE:
            smrptest_CreateGroups(csb, port);
	    process_sleep_for(WaitTime);
            break;
        case SMRP_TEST_DELETE:
            smrptest_DeleteGroups(csb, port);
	    process_sleep_for(WaitTime);
            break;
        case SMRP_TEST_DATA:
            group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
            /* Set receive data mode */
            if (GETOBJ(int,4)) 
                ReceiveDataFlag = GETOBJ(int,5);
	    else {
                if (GETOBJ(int,7)) 
                    NumToSend = GETOBJ(int,6);
                if (GETOBJ(int,9)) {
                    DataSize = GETOBJ(int,8);
                    data = smrptest_CreateData(DataSize);
                } else {
                    data = GETOBJ(string,1);
                    DataSize = strlen(data);
		}
                if (GETOBJ(int,11)) 
                    PauseTime = GETOBJ(int,10);

                if (GETOBJ(int,13)) 
                    PauseCnt = GETOBJ(int,12);

                smrptest_SendData(port, group, NumToSend, PauseTime, 
				  PauseCnt, data, DataSize);
	    }
            break;
        case SMRP_TEST_JOIN:
            group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
            smrptest_SendRequest(port, MCJoinRequest, group);
	    process_sleep_for(3 * WaitTime);
            break;
        case SMRP_TEST_LEAVE:
            group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
            smrptest_SendRequest(port, MCLeaveRequest, group);
	    process_sleep_for(2 * WaitTime);
            break;
        case SMRP_TEST_LOCATE:
            smrptest_SendRequest(port, MCLocateRequest, NullGroup);
	    process_sleep_for(WaitTime);
            break;
        case SMRP_TEST_PACKET:
            smrptest_SendTestPacket(csb);
            break;
        case SMRP_TEST_ROUTE:
            smrptest_SendDVPacket(csb);
            break;
        case SMRP_TEST_TUNNEL:
            smrptest_SendTunnelPacket(csb);
	    process_sleep_for(2 * WaitTime);
            break;
        default:
	    bad_parser_subcommand(csb, csb->which);
            break;
        }
        break;

    case SMRP_TEST_END:
        csb->mode = exec_mode;
        break;

    case SMRP_TEST_STOP:
        smrp_test_running = FALSE;
        smrp_CloseAllPorts();
        break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

}


void
smrp_test (parseinfo *csb)
{
    if (!smrp_running) {
        printf("\n%% SMRP not running");
	return;
    }

    if (!smrp_test_running) {
        smrp_test_running = TRUE;
        smrp_CloseAllPorts();
    }
    set_mode_byname(&csb->mode, "smrp-test", MODE_VERBOSE);
}

/*
 * smrp_register_thyself
 */
static void 
smrp_test_register_thyself (void)
{
    reg_add_smrp_intercept_data_pkt(smrptest_ReceiveData,
        "smrptest_ReceiveData");
    reg_add_smrp_intercept_req_pkt(smrptest_DispatchResponse, 
        "smrptest_DispatchResponse");
    reg_add_smrp_set_test_addresses(smrptest_ListenToTestAddr, 
        "smrptest_ListenToTestAddr");
}

void
smrp_test_init (subsystype *subsys)
{
    smrp_test_register_thyself();

    parser_add_commands(PARSE_ADD_TEST_CMD,&pname(smrp_test_commands),
			"smrp");
    parser_add_mode("smrp-test", "smrp test", "Smrp test mode",
		    FALSE, FALSE, NULL, NULL, NULL,
		    &pname(top_test_smrp),
		    NULL);

    TestGroupTable = NULL;
}
