/* $Id: smrp_port.c,v 3.4.48.1 1996/08/28 13:09:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_port.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP ports.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_port.c,v $
 * Revision 3.4.48.1  1996/08/28  13:09:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4  1995/12/08  04:55:58  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/17  18:54:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:25:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/09  17:26:30  kleung
 * CSCdi41821:  SMRP test router should not do port negotiation.
 *
 * Revision 2.9  1995/08/01  00:22:56  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.8  1995/07/18  22:38:04  kleung
 * CSCdi37265:  When router takes over Primarys groups, timers are not
 * started.
 *
 * Revision 2.7  1995/07/06  03:06:48  kleung
 * CSCdi36742:  Spurious memory access when SMRP port is deconfigured.
 *
 * Revision 2.6  1995/07/05  21:39:13  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.5  1995/06/23  18:49:16  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/21  18:06:49  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.3  1995/06/20  07:18:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:55:10  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface.h"
#include "encapstype.h"
#include "packet.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "mgd_timers.h"

/*
 *------------------------------------------------------------------
 * Module private definitions
 *------------------------------------------------------------------
 */

/*
 *------------------------------------------------------------------
 * Module private data and functions
 *------------------------------------------------------------------
 */
rbTree* smrp_PortTable;
ulong* SMRPPORT_stat;

static void smrp_DoPortPrimarySearch(SMRPPort *);

/*
 *------------------------------------------------------------------
 * smrp_port_Initialize			(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_port_Initialize ()
{
    int i;

    smrp_PortTable = RBTreeCreate("SMRP","SMRP", "Port Tree",
				  NULL, &smrp_port_debug->flag);
    SMRPASSERT(smrp_PortTable != NULL);

    SMRPPORT_stat = malloc(sizeof(ulong) * SMRPPORT_MAXTRAFFIC);
    if (SMRPPORT_stat == NULL) {
        smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }

    for (i=0; i < SMRPPORT_MAXTRAFFIC; i++)
	SMRPPORT_stat[i] = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_FindPort			(Module Private Function)
 *
 * Lookup the SMRP port descriptor given a port identifier.
 *------------------------------------------------------------------
 */
SMRPPort* 
smrp_FindPort (SMRPPortID pid)
{
    treeLink *link;

    link = RBTreeSearch(smrp_PortTable, pid);
    return (SMRPPort*) link;
}

SMRPPort *
smrp_FindPortFromPak (paktype *pak)
{
    SMRPNet net;

    if (!pak)
        return (NULL);
    
    if (pak->if_input == NULL || pak->linktype == ET_NULL)
	return (NULL);

    net.NetInterface = pak->if_input;
    net.NetType = pak->linktype;
    
    return (smrp_FindPort(smrp_MakePortID(&net)));
}

/*
 *------------------------------------------------------------------
 * smrp_CreatePort			(Network Layer Entrypoint)
 *
 * Create a new SMRP port descriptor and returns its identifier.
 *
 * The parse function that builds an SMRPNet structure is:
 *	netDescr = smrp_ParseNetRange(csb, linktype);
 *
 * If netDescr->ConfiguredRange != TRUE
 *	perform net layer specific automatic network range mapping
 * Else
 *	pass structure unchanged to smrp_CreatePortID.
 *------------------------------------------------------------------
 */
SMRPPort *
smrp_CreatePort (const SMRPNet* netDescr, const SMRPCallDowns* handlers)
{
    SMRPPort* aPort;
    SMRPNetRange range;
    char buf[30];
    
    SMRPASSERT(netDescr != NULL);
    SMRPASSERT(handlers != NULL);
    SMRPASSERT(netDescr->NetInterface != NULL);

    range.Start = netDescr->RangeStart;
    range.End = netDescr->RangeEnd;
    if (!smrp_IsValidNetRange(&range)) {
	printf("%%SMRP: Invalid network range specified");
        return NULL;
    }

    if (!smrp_CheckNetRange(&range))
        return NULL;

    aPort = (SMRPPort*) RBTreeGetFreeNode(smrp_PortTable);
    if (aPort == NULL) {
	aPort = malloc(sizeof(SMRPPort));
	if (aPort == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    return NULL;
	} else
	    SMRPPORT_stat[SMRPPORT_NEW_PORTS]++;
    } else
	SMRPPORT_stat[SMRPPORT_RECYCLED_PORTS]++;

    aPort->PortID = smrp_MakePortID(netDescr);
    aPort->NetProtocolUp = FALSE;
    aPort->State = MCNotInitialized;
    aPort->RangeStart = netDescr->RangeStart;
    aPort->RangeEnd = netDescr->RangeEnd;
    aPort->ConfiguredRange = netDescr->ConfiguredRange;
    aPort->NetworkType = netDescr->NetType;
    aPort->ThisNodeAddress = 0;
    aPort->PrimaryNodeAddress = 0;
    aPort->SecondaryNodeAddress = 0;
    aPort->NetHandlers = handlers;

    if (!smrp_LinkPortToIdb(aPort, netDescr->NetInterface)) {
	free(aPort);
	return NULL;
    }
        
    smrp_InitPortGroupData(aPort);
    smrpfast_InitPortMCacheData(aPort);

    RBTreeInsert(aPort->PortID, smrp_PortTable, &(aPort->link));

    /* port debugging */
    if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Port %s, port created",
	    PORTCB_PrintNetAddress(aPort, buf, aPort->ThisNodeAddress));

    return aPort;
}

static boolean
smrp_DeletePort (treeLink *link, void *d)
{
    SMRPPort *p = (SMRPPort *) link;
    char buf[30];
    
    /* port debugging */
    if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
	buginf("\nSMRP: Port %s, port deleted",
	       PORTCB_PrintNetAddress(p, buf, p->ThisNodeAddress));

    smrp_ClosePort(p);
    smrp_DeleteNodesForPort(p);
    smrp_DeletePortFromAllRoutes(p);
    smrp_DeleteAllPortGroups(p);
    PORTCB_DeletePort(p);
    RBTreeDelete(smrp_PortTable, &p->link);
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DeletePortID			(Network Layer Entrypoint)
 *
 * Delete the SMRP port descriptor specified by its identifier.
 *------------------------------------------------------------------
 */
void
smrp_DeletePortID (SMRPPortID pid)
{
    SMRPPort* p = smrp_FindPort(pid);

    if (p) {
	smrp_DeletePort(&p->link, NULL);
    } else {
	smrp_Error("Attempt to delete an unknown port ID");
    }
}

/*
 *------------------------------------------------------------------
 * smrp_PortIDLinkState			(Network Layer Entrypoint)
 *
 * Notify SMRP of a change in the network link state for a port.
 *------------------------------------------------------------------
 */
void
smrp_PortIDLinkState (SMRPPort *p, boolean lstate)
{
    char buf[30];

    if (p) {
	if (p->NetProtocolUp != lstate) {
        /* port debugging */
        if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Port %s, line protocol changed to %s",
	        PORTCB_PrintNetAddress(p, buf, p->ThisNodeAddress),
 	        (lstate ? "UP" : "DOWN"));

	    if (!lstate) {
		smrp_ClosePort(p);
	    } else {
		smrp_StartPort(p);
	    }
	    p->NetProtocolUp = lstate;
	}
    }
}

/*
 *------------------------------------------------------------------
 * smrp_GetPortIDNetInterface		(Network Layer Entrypoint)
 *
 * Obtain the network reference from an SMRP port identifier.
 *------------------------------------------------------------------
 */
SMRPIdbsb *
smrp_GetPortIDNetInterface (SMRPPortID pid)
{
    SMRPPort* p = smrp_FindPort(pid);

    if (p) {
	return p->NetworkReference;
    }
    return NULL;
}

/*
 *------------------------------------------------------------------
 * smrp_IsPortOperational		(SMRP Internal Function)
 *
 * Return TRUE if the SMRP port is operational.
 *------------------------------------------------------------------
 */
boolean
smrp_IsPortOperational (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    switch (port->State) {
    case MCPrimaryOperation:
    case MCSecondaryOperation:
    case MCNormalOperation:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_SetPortState			(SMRP Internal Function)
 *
 * Change the current port state.
 *------------------------------------------------------------------
 */
void
smrp_SetPortState (SMRPPort* port, NodeState newState)
{
    char buf[30];

    SMRPASSERT(port != NULL);
    if (port->State != newState) {
        /* port debugging */
        if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Port %s, state changed from \"%s\" to \"%s\"",
	        PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
		smrp_StateMessage(port->State),
		smrp_StateMessage(newState));
    }
    port->State = newState;
}

static boolean
smrp_SendNotifyToNode (treeLink *p, void *data)
{
    SMRPNode *node = (SMRPNode *) p;
    SMRPPort *port = (SMRPPort *) data;

    if (port->PortID != node->PortID)
	return TRUE;

    if (!smrp_IsNodeOperational(node))
        return TRUE;

    smrp_stat[SMRP_NOTIFY_OUT]++;

    smrp_CreateRequest(port, MCNotifyRequest, NullGroup, node->NodeAddress,
		       &port->State, sizeof(port->State), 0, 0, NULL);
    return TRUE;
}

void
smrp_SendNotifyToPort (SMRPPort *port)
{
    SMRPRequest *req;
    
    SMRPASSERT(port->State != MCPrimaryNegotiation &&
	       port->State != MCSecondaryNegotiation &&
	       port->State != MCNotInitialized);

    /* Discard old notifies */
    while ((req = smrp_FindRequestByPort(port->PortID, MCNotifyRequest))) 
	smrp_DiscardRequest(req);

    smrp_ForAllNodesDo(smrp_SendNotifyToNode, port);
}

/*
 *------------------------------------------------------------------
 * smrp_SendHelloToPort			(SMRP Internal Function)
 *
 * Sends a hello datagram to the SMRP port.  Should be called
 * for any state change as well as periodically.
 *------------------------------------------------------------------
 */
static void
smrp_SendHelloToPort (SMRPPort* port)
{
    paktype* pak;
    MCHeader hp;

    SMRPASSERT(port != NULL);

    /* TESTMODE - Don't send hellos 
    if (smrp_test_running) {
        return;
    }
*/

    smrp_SetupHeader(&hp, MCHelloDatagram, 0, NullGroup);

#ifdef __SMRP_CISCO__
    hp.RangeStart = port->RangeStart;
    hp.RangeEnd = port->RangeEnd;
#endif __SMRP_CISCO__

    pak = smrp_CreatePacketWithHeader(port, &hp, 1);
    if (pak == NULL) 
	return;

    *smrp_datastart(pak) = port->State;
    
    smrp_stat[SMRP_HELLO_OUT]++;

    PORTCB_SendMCPacket(port, pak, AllNodesAddr);
}
/*
 *------------------------------------------------------------------
 * smrp_BecomePortPrimary		(SMRP Internal Function)
 *
 * Called when primary negotiation completes and our node is
 * now the primary.
 *
 * This function can be invoked from either of two states:
 *	PrimaryNegotiation - We are a new primary (DeleteAllGroups)
 *	SecondaryOperation - The old primary went down (KeepGroups)
 *------------------------------------------------------------------
 */
static void
smrp_BecomePortPrimary (SMRPPort* port)
{
    SMRPRoute *route;
    SMRPGroup *group;
    SMRPForward *fwd;
    
    SMRPASSERT(port != NULL);

    if (port->State != MCSecondaryOperation &&
	port->State != MCPrimaryNegotiation)
	smrp_DeleteAllPortGroups(port);
    smrp_SetPortState(port,MCPrimaryOperation);
    group = port->GroupTable;
    while (group != NULL) {
	fwd = smrp_FindForward(group->GroupAddress);
	if (fwd)
	    fwd->ParentNodeAddress = group->CreatorAddress;
	mgd_timer_start(&group->CreatorTimer, smrp_creatorInterval);
	group = group->Next;
    }
    smrp_SetPortPrimary(port,port->ThisNodeAddress);
    route = smrp_FindRouteByNet(port->RangeStart);
    if (route) {
	route->ParentNodeAddress = port->ThisNodeAddress;
	smrp_RouteSetPortParentInfo(route, port->PortID, port->ThisNodeAddress,
				    route->Distance + 1);
    }
    smrp_SendNotifyToPort(port);
}

/*
 *------------------------------------------------------------------
 * smrp_BecomePortSecondary		(SMRP Internal Function)
 *
 * Called when secondary negotiation completes and our node is
 * now the secondary.  Any groups are always deleted.
 *------------------------------------------------------------------
 */
static void
smrp_BecomePortSecondary (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    smrp_DeleteAllPortGroups(port);
    smrp_SetPortState(port,MCSecondaryOperation);
    smrp_SendNotifyToPort(port);
    smrp_SetPortSecondary(port,port->ThisNodeAddress);
}

/*
 *------------------------------------------------------------------
 * smrp_BecomePortOperational		(SMRP Internal Function)
 *
 * Called when secondary negotiation fails and our node is
 * now operational.  Any groups are always deleted.
 *------------------------------------------------------------------
 */
static void
smrp_BecomePortOperational (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    smrp_DeleteAllPortGroups(port);
    smrp_SetPortState(port,MCNormalOperation);
    smrp_SendNotifyToPort(port);
}

/*
 *------------------------------------------------------------------
 * smrp_IsOurNodePortPrimary		(SMRP Internal Function)
 *
 * Return TRUE iff we are the SMRP primary for the local net.
 *------------------------------------------------------------------
 */
boolean
smrp_IsOurNodePortPrimary (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    switch (port->State) {
    case MCPrimaryOperation:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_IsOurNodePortSecondary		(SMRP Internal Function)
 *
 * Return TRUE iff we are the SMRP secondary for the local net.
 *------------------------------------------------------------------
 */
boolean
smrp_IsOurNodePortSecondary (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    switch (port->State) {
    case MCSecondaryOperation:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_IsPortActive			(SMRP Internal Function)
 *
 * Return TRUE if the SMRP port is active although not necessarily
 * operational.
 *------------------------------------------------------------------
 */
boolean
smrp_IsPortActive (SMRPPort* port)
{
    SMRPASSERT(port != NULL);
    
    switch (port->State) {
    case MCPrimaryOperation:
    case MCSecondaryOperation:
    case MCNormalOperation:
    case MCSecondaryNegotiation:
    case MCPrimaryNegotiation:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_GetPortSecondary		(SMRP Internal Function)
 *
 * Get the current port secondary.  Returns 0 if none.
 *------------------------------------------------------------------
 */
UnicastAddr
smrp_GetPortSecondary (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    return port->SecondaryNodeAddress;
}

/*
 *------------------------------------------------------------------
 * smrp_SetPortPrimary			(SMRP Internal Function)
 *
 * Set the primary node for this SMRP port.  Automatically clears
 * secondary if primary changes.
 *------------------------------------------------------------------
 */
void
smrp_SetPortPrimary (SMRPPort* port, UnicastAddr primary)
{
    char buf[30], buf2[30], buf3[30];

    SMRPASSERT(port != NULL);
    if (port->PrimaryNodeAddress != primary) {

	if (primary == 0) {

	} else if (primary == port->ThisNodeAddress) {
	    SMRPASSERT(port->State == MCPrimaryOperation);
	    if (port->SecondaryNodeAddress == port->ThisNodeAddress)
		smrp_SetPortSecondary(port, 0);
	}

        /* port debugging */
        if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Port %s, primary router changed from %s to %s",
                PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
                PORTCB_PrintNetAddress(port, buf2, port->PrimaryNodeAddress),
                PORTCB_PrintNetAddress(port, buf3, primary));

    }
 	        
    port->PrimaryNodeAddress = primary;
}

/*
 *------------------------------------------------------------------
 * smrp_SetPortSecondary		(SMRP Internal Function)
 *
 * Set the secondary node for this SMRP port.
 *------------------------------------------------------------------
 */
void
smrp_SetPortSecondary (SMRPPort* port, UnicastAddr secondary)
{
    char buf[30], buf2[30], buf3[30];

    SMRPASSERT(port != NULL);
    if (port->SecondaryNodeAddress != secondary) {
	if (secondary == 0) {

	} else if (secondary == port->ThisNodeAddress) {

	} else {

	}
        /* port debugging */
        if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Port %s, secondary router changed from %s to %s",
                PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
                PORTCB_PrintNetAddress(port, buf2, port->SecondaryNodeAddress),
                PORTCB_PrintNetAddress(port, buf3, secondary));
    }
    port->SecondaryNodeAddress = secondary;
}

/*
 *------------------------------------------------------------------
 * smrp_ForAllPortsDo			(SMRP Internal Function)
 *
 * Invoke the specified function for each SMRP port.  The port
 * entry should not be modified for any reason.
 *------------------------------------------------------------------
 */
void
smrp_ForAllPortsDo (treeProc tp, void* d)
{
    RBTreeForEachNode(tp,d,smrp_PortTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_OpenPort			(SMRP Internal Function)
 *
 * Check link state and if up, initialize the port for startup.
 *
 *
 * Our implementation breaks the port bringup process into two
 * distinct operations.  Creating the port builds the necessary data
 * structures but does not start up the port protocol. 
 *
 * The maintenenace process periodically attempts to start the port by
 * calling OpenPort.  The OpenPort will fail until network protocol is
 * running.  During this time, the port status is MCNotInitialized
 * indicating that there are no multicast subscriptions nor are any
 * operations permitted other than OpenPort.
 *
 * Once line protocol is running, we obtain our net address as well as
 * subscribing to the necessary multicast addresses.  With these
 * actions complete, we set the status to MCNotOperational.  At this
 * point, we are at the equivalent state per the spec.
 *------------------------------------------------------------------
 */
static boolean
smrp_OpenPort (SMRPPort* port)
{
    char buf[30];

    SMRPASSERT(port != NULL);
    SMRPASSERT(port->State == MCNotInitialized);
    
    port->NetProtocolUp = PORTCB_LinkState(port);
    if (!port->NetProtocolUp) {
	return FALSE;
    }
    
    if (!port->ConfiguredRange) {
	SMRPNet net;
	SMRPCallDowns const *handlers;
	SMRPNetRange range;
	
	net.ConfiguredRange = FALSE;
	net.NetInterface = port->NetworkReference->phys_idb;

	reg_invoke_smrp_add_netrange(port->NetworkType, &net, &handlers,
				     FALSE);
	if (handlers == NULL)
	    return FALSE;

	range.Start = net.RangeStart;
	range.End = net.RangeEnd;

	if (!smrp_IsValidNetRange(&range) || !smrp_CheckNetRange(&range))
	    return FALSE;

	port->RangeStart = net.RangeStart;
	port->RangeEnd = net.RangeEnd;
    }
    
    port->ThisNodeAddress = PORTCB_NetAddress(port);

    if (!PORTCB_OpenPort(port))
	return FALSE;
    
    smrp_SetPortPrimary(port, 0);
    smrp_SetPortSecondary(port, 0);
    smrp_SetPortState(port, MCNotOperational);
    smrp_SetCommonMulticasts(port);

    /* port debugging */
    if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
	errmsg(&msgsym(PORTUP, SMRP), 
	       idb_get_namestring(port->NetworkReference->phys_idb),
	       PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_ClosePort			(SMRP Internal Function)
 *
 * Shutdown an SMRP port that may be operational.  Notify
 * SMRP routing and the network layer.  This operation is 
 * performed whenever link state goes down or when deleting
 * an active port.
 *------------------------------------------------------------------
 */
void
smrp_ClosePort (SMRPPort* port)
{
    char buf[30];

    SMRPASSERT(port != NULL);

    if (port->State == MCNotInitialized)
	return;
    
    if (smrp_IsPortActive(port)) {
	smrp_PoisonRoutesForPort(port->PortID);
    }
    smrp_DeleteAllPortGroups(port);
    smrp_DeleteMatchingForwards(port->PortID, 0, FALSE, FALSE);
    
    /* Set port state to not operational so that a Notify request can be
     * sent.
     */
    smrp_SetPortState(port, MCNotOperational);
    smrp_SendNotifyToPort(port);
    smrp_MarkPortNodesDown(port);
    smrp_SetPortState(port,MCNotInitialized);

    PORTCB_ClosePort(port);

    /* port debugging */
    if (smrp_debug(smrp_port_debug, SMRP_NULL_MASK))
	errmsg(&msgsym(PORTDOWN, SMRP), 
	       idb_get_namestring(port->NetworkReference->phys_idb),
	       PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    smrp_SetPortPrimary(port,0);
    smrp_SetPortSecondary(port, 0);
    port->ThisNodeAddress = 0;
}

/*
 * smrp_CompletePrimaryRequest
 *
 * Completion procedure for closing out a Primary Request.  issearch is
 * TRUE if transaction was initiated just to find the primary.  FALSE if
 * trying to become primary.
 */
static void
smrp_CompletePrimaryRequest (SMRPRequest *req, boolean issearch)
{
    SMRPNode *node;
    SMRPPort *port;
    SMRPRoute *route;
    SMRPNetRange range;
    
    port = smrp_FindPort(req->PortID);
    if (!port)
	return;
    
    if (!issearch)
	SMRPASSERT(port->State == MCPrimaryNegotiation);

    range.Start = port->RangeStart;
    range.End = port->RangeEnd;
    route = smrp_FindRoute(&range);
    
    if (smrp_ResponseIsConfirmation(req)) {
	/* Responder is primary */
	if (req->Responder != port->ThisNodeAddress) {
	    node = smrp_UpdateAdjacentNode(req->Responder, req->PortID,
					   req->ResponseType);
	    if (node)
		smrp_SetNodeState(node, MCPrimaryOperation);
	}
	smrp_SetPortPrimary(port, req->Responder);
#if APPLE_SPEC
	if (route)
	    route->ParentNodeAddress = req->Responder;
#endif
	if (!issearch)
	    smrp_BecomePortSecondary(port);
	
    } else if (!issearch) {
	if (req->ErrorIndication == MCNoResponse) {
	    smrp_BecomePortPrimary(port);
	    if (route)
		route->ParentNodeAddress = port->ThisNodeAddress;
	}
    }
}

static void
smrp_CompletePrimaryNegotiation (SMRPRequest *req)
{
    smrp_CompletePrimaryRequest(req, FALSE);
}

static void
smrp_CompletePrimarySearch (SMRPRequest *req)
{
    smrp_CompletePrimaryRequest(req, TRUE);
}

static void
smrp_CompleteSecondaryRequest (SMRPRequest *req)
{
    SMRPPort *port;
    SMRPNode *node;
    
    
    port = smrp_FindPort(req->PortID);
    if (!port)
	return;

    SMRPASSERT(port->State == MCSecondaryNegotiation);

    if (smrp_ResponseIsConfirmation(req)) {
	/* Responder is secondary */
	if (req->Responder != port->ThisNodeAddress) {
	    node = smrp_UpdateAdjacentNode(req->Responder, req->PortID,
					   req->ResponseType);
	    if (node)
		smrp_SetNodeState(node, MCSecondaryOperation);
	}
	smrp_SetPortSecondary(port, req->Responder);
	smrp_BecomePortOperational(port);

	/* Find out who primary is */
	smrp_DoPortPrimarySearch(port);
    } else {
	if (req->ErrorIndication == MCNoResponse) {
	    smrp_BecomePortSecondary(port);

	    /* See if primary node has died.  If so, we need to become
	     * primary.
	     */
	    node = smrp_FindAdjacentNode(port->PrimaryNodeAddress);
   	    if (!node || node->State == MCNotOperational) {
		smrp_DoPortPrimaryNegotiation(port);
	    } 
	}
    }
}

/*
 * smrp_SendPrimaryNodeRequest
 *
 * Sends a designated node request packet to the port.
 */
static void
smrp_SendPrimaryNodeRequest (SMRPPort *port)
{
    smrp_stat[SMRP_PNODE_OUT]++;
    smrp_CreateRequest(port, MCPrimaryRequest, NullGroup, AllNodesAddr,
		       NULL, 0, PrimaryRequestInterval, PrimaryRequestRetries,
		       smrp_CompletePrimaryNegotiation);
}

/*
 * smrp_SendSecondaryNodeRequest
 *
 * Sends a designated node request packet to the port.
 */
static void
smrp_SendSecondaryNodeRequest (SMRPPort *port)
{
    smrp_stat[SMRP_SNODE_OUT]++;
    smrp_CreateRequest(port, MCSecondaryRequest, NullGroup, AllNodesAddr,
		       NULL, 0, SecondaryRequestInterval,
		       SecondaryRequestRetries, smrp_CompleteSecondaryRequest);
}

/*
 *------------------------------------------------------------------
 * smrp_StartPort			(SMRP Internal Function)
 *
 * Starts, or restarts, the SMRP protocol for a port.  Returns
 * TRUE if the port is successfully started.
 *
 * Our implementation enhances the spec by closing
 * the port if it is currently operational.  This should not 
 * occur with the current architecture but is needed to insure
 * that routing and other data structures are updated.
 *------------------------------------------------------------------
 */
boolean
smrp_StartPort (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    if (smrp_IsPortOperational(port))
	smrp_ClosePort(port);

    if (!smrp_OpenPort(port))
	return FALSE;
    smrp_PortRouteChange(port, TRUE);
    smrp_AddPortToAllRoutes(port);
    smrp_SendHelloToPort(port);
    smrp_DoPortSecondaryNegotiation(port);
    smrpfast_SetupPort(port);

    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DoPortSecondaryNegotiation	(SMRP Internal Function)
 *
 * Try to become the secondary node for a local net.
 *------------------------------------------------------------------
 */
void
smrp_DoPortSecondaryNegotiation (SMRPPort* port)
{
    SMRPASSERT(port != NULL);

    /* TESTMODE - Don't negotiate */
    if (smrp_test_running) {
        smrp_BecomePortOperational(port);
        return;
    }

    smrp_SetPortSecondary(port,0);
    smrp_SetPortState(port,MCSecondaryNegotiation);
    smrp_SendSecondaryNodeRequest(port);
}

void
smrp_DoPortPrimaryNegotiation (SMRPPort *port)
{
    smrp_SetPortState(port, MCPrimaryNegotiation);
    smrp_SendPrimaryNodeRequest(port);
}

static void
smrp_DoPortPrimarySearch (SMRPPort *port)
{
    smrp_stat[SMRP_PNODE_OUT]++;
    smrp_CreateRequest(port, MCPrimaryRequest, NullGroup, AllNodesAddr,
		       NULL, 0, PrimaryRequestInterval, PrimaryRequestRetries,
		       smrp_CompletePrimarySearch);
}

/*
 *------------------------------------------------------------------
 * smrp_HandlePort			(Module TreeProc Routine)
 *
 * Handle processing for a port from the table.  Currently checks
 * if the port is initialized and either starts it or sends out
 * the periodic hello.
 *------------------------------------------------------------------
 */
static boolean
smrp_HandlePort (treeLink* link, void* d)
{
    SMRPPort* port = (SMRPPort*) link;

    if (!smrp_IsPortActive(port)) {
	smrp_StartPort(port);
	return (TRUE);
    }
    /* Sanity check designated routers */
    if (port->State == MCNormalOperation)
	if (port->PrimaryNodeAddress == 0 || port->SecondaryNodeAddress == 0)
	    smrp_DoPortSecondaryNegotiation(port);
    else if (port->State == MCSecondaryOperation)
	if (port->PrimaryNodeAddress == 0)
	    smrp_DoPortPrimaryNegotiation(port);
    
    smrp_SendHelloToPort(port);
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_HandleAllPorts			(SMRP Internal Function)
 *
 * Start any ports that are not operational and generate hello
 * packets for each port.  
 *------------------------------------------------------------------
 */
void
smrp_HandleAllPorts (void)
{
    RBTreeForEachNode(smrp_HandlePort, NULL,smrp_PortTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_ResetIFPort			(Module TreeProc Routine)
 *
 * Close any ports associated with this interface.  Called when an
 * interface is cleared by the user.
 *------------------------------------------------------------------
 */
static boolean
smrp_ResetIFPort (treeLink* link, void* d)
{
    SMRPPort* port = (SMRPPort*) link;
    idbtype* idb = (idbtype*) d;

    if (port->NetworkReference->phys_idb == idb) {
	smrp_ClosePort(port);
    }
    return (TRUE);
}

void
smrp_route_adjust (idbtype *idb)
{
    if (!smrp_running)
	return;

    reg_invoke_smrp_port_route_adjust(idb);
}    
    
/*
 *------------------------------------------------------------------
 * smrp_ResetIFNotify			(SMRP Registry Routine)
 *
 * Close any ports which are associated with this interface.  This
 * routine is called when an interface is cleared or flaps.
 *------------------------------------------------------------------
 */
void
smrp_ResetIFNotify (idbtype* idb)
{
    RBTreeForEachNode(smrp_ResetIFPort,idb,smrp_PortTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_IFFixupNotify			(SMRP Registry Routine)
 *
 * Close any ports which are associated with this interface.  This
 * routine is called when an interface encapsulation changes.
 *------------------------------------------------------------------
 */
void
smrp_IFFixupNotify (idbtype* idb, int old_encap)
{
    RBTreeForEachNode(smrp_ResetIFPort,idb,smrp_PortTable,FALSE);
}

static boolean
smrp_TreeClosePort (treeLink *link, void *d)
{
    SMRPPort *port = (SMRPPort *) link;

    smrp_ClosePort(port);
    return TRUE;
}

void
smrp_CloseAllPorts (void)
{
    RBTreeForEachNode(smrp_TreeClosePort, NULL, smrp_PortTable, FALSE);
}

void
smrp_DoPrimaryRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    SMRPNode *node;
    
    smrp_stat[SMRP_PNODE_IN]++;
    
    smrp_UpdateAdjacentNode(res->Source, res->PortID, res->PacketType);

    /* TESTMODE - Don't negotiate */
    if (smrp_test_running) {
        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
        return;
    }

    if (smrp_IsOurNodePortPrimary(port)
	|| (port->State == MCPrimaryNegotiation
	    && smrp_IsFirstPreferred(port->ThisNodeAddress, res->Source)))
	smrp_SendConfirmResponse(res);
    else if (smrp_IsOurNodePortSecondary(port)) {
	/* CISCO Extension */
	node = smrp_FindAdjacentNode(port->PrimaryNodeAddress);
	if (node) {
	    /* See if primary node has died.  If so, we need to become
	     * primary.
	     */
	    smrp_CheckAdjacentNode(node);
	}

	if (smrp_IsOurNodePortPrimary(port)) {
	    /* We became primary, so answer this request. */
	    smrp_SendConfirmResponse(res);

        } else { 
            /* Free this request so we can check when it comes again */
            res->State = MCRequestComplete;
   	    smrp_DiscardResponse(res);
      }
    } else { /* Ignore this request */
        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
    }
}

void
smrp_DoSecondaryRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    smrp_stat[SMRP_SNODE_IN]++;
    
    smrp_UpdateAdjacentNode(res->Source, port->PortID, res->PacketType);

    /* TESTMODE - Don't negotiate */
    if (smrp_test_running) {
        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
        return;
    }

    if (smrp_IsOurNodePortSecondary(port)
	|| (port->State == MCSecondaryNegotiation
	    && smrp_IsFirstPreferred(port->ThisNodeAddress, res->Source))
	|| (port->State == MCPrimaryNegotiation))
	smrp_SendConfirmResponse(res);
    else {
        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
    }
}

void
smrp_DoTunnelRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    smrp_stat[SMRP_TUNNEL_IN]++;

    if (!PORTCB_DoTunnelRequest(port, res)) {
        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
    }
}

