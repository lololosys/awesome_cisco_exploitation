/* $Id: smrp_node.c,v 3.4.48.1 1996/08/28 13:09:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_node.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP Nodes (Neighbors).
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_node.c,v $
 * Revision 3.4.48.1  1996/08/28  13:09:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4  1995/12/08  04:55:57  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/17  18:53:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/11  19:02:22  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.10  1995/09/15  21:49:41  mleelani
 * CSCdi40089:  smrp test failed, some routes missed from the routing table
 * Node version set incorrectly. Fix code review comments.
 *
 * Revision 2.9  1995/08/30  22:43:31  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.8  1995/08/01  00:22:51  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.7  1995/07/24  22:30:52  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.6  1995/06/23 18:49:15  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  19:16:10  slin
 * CSCdi34668:  Long packets are not checked.
 *
 * Revision 2.4  1995/06/21  18:06:47  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.3  1995/06/20  07:18:02  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:55:07  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface.h"
#include "packet.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"

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
static rbTree* smrp_AdjacentNodeTable;
ulong* SMRPNODE_stat;

/*
 *------------------------------------------------------------------
 * smrp_node_Initialize			(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_node_Initialize ()
{
    int i;

    smrp_AdjacentNodeTable = RBTreeCreate("SMRP","SMRP", "Adjacent Node Tree",
					  NULL, &smrp_neighbor_debug->flag);

    SMRPASSERT(smrp_AdjacentNodeTable != NULL);
    SMRPNODE_stat = malloc(sizeof(ulong) * SMRPNODE_MAXTRAFFIC);
    if (SMRPNODE_stat == NULL) {
        smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }

    for (i=0; i < SMRPNODE_MAXTRAFFIC; i++)
	SMRPNODE_stat[i] = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_FindAdjacentNode		(SMRP Internal Function)
 *
 * Find a node given its network layer address.
 *
 * To do: Add support in cases where network addresses are not
 *        unique across protocols.
 *------------------------------------------------------------------
 */
SMRPNode*
smrp_FindAdjacentNode (UnicastAddr naddr)
{
    treeLink *link;

    link = RBTreeSearch(smrp_AdjacentNodeTable, naddr);
    return (SMRPNode*) link;
}

/*
 *------------------------------------------------------------------
 * smrp_SetNodeState			(SMRP Internal Function)
 *
 * Set the state of the node and generate a message if it has
 * changed (unless it was MCNotInitialized).
 *------------------------------------------------------------------
 */
void
smrp_SetNodeState (SMRPNode* node,NodeState s)
{
    SMRPPort *port;
    char buf[30];

    SMRPASSERT(node != NULL);
    if (node->State != s) {
	if (node->State != MCNotInitialized) {
            /* neighbor debugging */
            port = smrp_FindPort(node->PortID);
            if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK))
                buginf("\nSMRP: Neighbor %s, state changed from \"%s\" to \"%s\"",
  	            PORTCB_PrintNetAddress(port, buf, node->NodeAddress),
	            smrp_StateMessage(node->State),
	            smrp_StateMessage(s));
	}
    }
    node->State = s;
}

/*
 *------------------------------------------------------------------
 * smrp_GetNodeRouteVersion		(SMRP Internal Function)
 *
 * Get the node's current route version. If NeedsRoutes is TRUE,
 * return 0 as route version.
 *------------------------------------------------------------------
 */
ushort
smrp_GetNodeRouteVersion (SMRPNode* node)
{
    SMRPASSERT(node != NULL);
    if (node->NeedsRoutes)
	return 0;
    return node->RouteVersion;
}

/*
 *------------------------------------------------------------------
 * smrp_SetNodeRouteVersion		(SMRP Internal Function)
 *
 * Set the node's current route version and clear the NeedsRoutes
 * flag.
 *------------------------------------------------------------------
 */
void
smrp_SetNodeRouteVersion (SMRPNode* node,ushort ver)
{
    SMRPASSERT(node != NULL);
    node->RouteVersion = ver;
    node->NeedsRoutes = (ver == 0);
}

/*
 *------------------------------------------------------------------
 * smrp_CreateNode			(SMRP Internal Function)
 *
 * Create a new node, initialize it and add it to the table.
 *------------------------------------------------------------------
 */
static SMRPNode*
smrp_CreateNode (UnicastAddr naddr, SMRPPortID pid)
{
    SMRPNode* node;
    SMRPPort* port;
    char buf[30];
    int i;
    
    node = (SMRPNode*) RBTreeGetFreeNode(smrp_AdjacentNodeTable);
    if (node == NULL) {
	SMRPNODE_stat[SMRPNODE_NEW_NODES]++;
	node = malloc(sizeof(SMRPNode));
	if (node == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    return NULL;
	}
    } else {
	SMRPNODE_stat[SMRPNODE_RECYCLED_NODES]++;
    }

    node->State = MCNotInitialized;
    node->PortID = pid;
    node->NodeAddress = naddr;
    node->RouteVersion = 0;
    node->NeedsRoutes = FALSE;
    for (i=0;i < NODEROUTE_MAXTRAFFIC;i++)
	node->Routing_Stats[i] = 0;
    GET_TIMESTAMP(node->HelloTime);

    RBTreeInsert(node->NodeAddress, smrp_AdjacentNodeTable, &(node->link));

    /* neighbor debugging */
    port = smrp_FindPort(node->PortID);
    if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Neighbor %s, new neighbor added",
            PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

    return node;
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteNode			(SMRP Internal Function)
 *
 * Delete an adjacent node.
 *------------------------------------------------------------------
 */
void
smrp_DeleteNode (SMRPNode* node)
{
    SMRPPort* port;
    char buf[30];

    SMRPASSERT(node != NULL);

    RBTreeDelete(smrp_AdjacentNodeTable, &node->link);

    /* neighbor debugging */
    port = smrp_FindPort(node->PortID);
    if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Neighbor %s, neighbor deleted",
            PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

    SMRPNODE_stat[SMRPNODE_DELETED_NODES]++;
}

/*
 *------------------------------------------------------------------
 * smrp_UpdateAdjacentNode		(SMRP Internal Function)
 *
 * Perform the Update Node algorithm for a node.  Allocates a new
 * entry if needed.
 *------------------------------------------------------------------
 */
SMRPNode*
smrp_UpdateAdjacentNode (UnicastAddr naddr, SMRPPortID pid, uchar pt)
{
    SMRPNode* node = smrp_FindAdjacentNode(naddr);
    SMRPPort *port;
    char buf[30];
        
    if (node == NULL) {
	node = smrp_CreateNode(naddr, pid);
	if (node == NULL) 
	    return NULL;
        /* neighbor debugging */
        if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK)) {
	    port = smrp_FindPort(pid);
	    if (port) {
		errmsg(&msgsym(NEIGHBORUP, SMRP), 
                       idb_get_namestring(port->NetworkReference->phys_idb),
		       PORTCB_PrintNetAddress(port, buf, naddr));
	    }
	}

	smrp_SetNodeState(node,MCNotOperational);
    }

    SMRPASSERT(node->PortID == pid);
    GET_TIMESTAMP(node->HelloTime);
    if (node->State == MCNotOperational) {
	smrp_SetNodeState(node,MCNormalOperation);
	node->RouteVersion = 0;
	node->NeedsRoutes = TRUE;
	port = smrp_FindPort(node->PortID);
	if (port) 
	    smrp_SendRoutingTable(node, port);
    }

    return node;
}

static void
smrp_ProcessDownNode (SMRPNode *node, NodeState oldstate)
{
    SMRPPort* port;
    char buf[30];

    port = smrp_FindPort(node->PortID);

    if (oldstate != MCNotOperational) {
        /* neighbor debugging */
        if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK)) {

            errmsg(&msgsym(NEIGHBORDOWN, SMRP), 
                   idb_get_namestring(port->NetworkReference->phys_idb),
	           PORTCB_PrintNetAddress(port, buf, node->NodeAddress));
	}

	smrp_SetNodeState(node,MCNotOperational);

	/*
	 * Reset port parent links and make sure we ask for
	 * DV updates from adjacent nodes on the same port. 
	 */
	smrp_FixupPortParentLinks(node->NodeAddress);
	smrp_QueryParentNodes(port);
	smrp_FixupRouteDistances();

	/*
	 * Poison Routes
	 */
	smrp_PoisonRoutesFromNode(node);
	smrp_DeleteMatchingForwards(0, node->NodeAddress, FALSE, TRUE);
	/*
	 * Start the hold down timer if we have not started it.
	 */
	if (!smrp_ignoreDistanceRequests) {
	    smrp_StartHoldDownTimer();
	}
    }

    if (oldstate == MCPrimaryOperation) {

	/* Reparent all forwards through port->PortID to
	 * port->PrimaryNodeAddress to point to 0.
	 */
	smrp_ReparentForwards(port->PortID, port->PrimaryNodeAddress, 0,
			      FALSE);
	smrp_SetPortPrimary(port, 0);
	if (port->State == MCSecondaryOperation)
	    smrp_DoPortPrimaryNegotiation(port);
	
    } else if (oldstate == MCSecondaryOperation) {
	if (port->SecondaryNodeAddress == node->NodeAddress)
	    smrp_SetPortSecondary(port, 0);
	
	/* Try to become secondary */
	if (port->State == MCNormalOperation)
	    smrp_DoPortSecondaryNegotiation(port);
    }

}

void
smrp_DoNotifyRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    SMRPNode *node;
    NodeState notifypktstate;
    NodeState oldstate;
    
    smrp_stat[SMRP_NOTIFY_IN]++;
    
    if (res->RequestSize != SMRP_NOTIFY_SIZE) {
	if (res->RequestSize < SMRP_NOTIFY_SIZE)
	    smrp_stat[SMRP_RUNT]++;
	smrp_SetResponseState(res,MCRequestComplete);
	smrp_DiscardResponse(res);
	return;
    }

    notifypktstate = *res->RequestData;

    /* Discard all bogus packets */
    if (notifypktstate != MCPrimaryOperation &&
	notifypktstate != MCSecondaryOperation &&
	notifypktstate != MCNormalOperation &&
	notifypktstate != MCNotOperational) {

	smrp_SetResponseState(res,MCRequestComplete);
	smrp_DiscardResponse(res);
	return;
    }
    
    node = smrp_UpdateAdjacentNode(res->Source, port->PortID, res->PacketType);

    oldstate = node->State;
    smrp_SetNodeState(node, notifypktstate);
    
    if (notifypktstate == MCPrimaryOperation) {
	smrp_SetPortPrimary(port, node->NodeAddress);

	/* If this guy was secondary, clear it */
	if (port->SecondaryNodeAddress == node->NodeAddress)
	    smrp_SetPortSecondary(port, 0);

	/* Reparent all local forwards through port->PortID to point to
	 * node->NodeAddress.
	 */
	smrp_ReparentForwards(port->PortID, 0, node->NodeAddress, TRUE);
	if (port->State == MCNormalOperation)
	    smrp_DoPortSecondaryNegotiation(port);
    } else if (notifypktstate == MCSecondaryOperation) {
	smrp_SetPortSecondary(port, node->NodeAddress);

	/* If this guy was primary, clear it */
	if (port->PrimaryNodeAddress == node->NodeAddress)
	    smrp_SetPortPrimary(port, 0);
    
	if (port->State == MCPrimaryOperation)
	    smrp_SendPortGroupsToSecondary(port, node);
    } else if (notifypktstate == MCNormalOperation) {
	if (port->PrimaryNodeAddress == node->NodeAddress) {
	    smrp_SetPortPrimary(port, 0);
	    if (port->State == MCSecondaryOperation)
		smrp_DoPortPrimaryNegotiation(port);
	}
	if (port->SecondaryNodeAddress == node->NodeAddress) {
	    smrp_SetPortSecondary(port, 0);
	    if (port->State == MCNormalOperation)
		smrp_DoPortSecondaryNegotiation(port);
	}
    } else if (notifypktstate == MCNotOperational)
	smrp_ProcessDownNode(node, oldstate);
    
    smrp_SendConfirmResponse(res);
}

/*
 *------------------------------------------------------------------
 * smrp_ProcessHello			(SMRP Internal Function)
 *
 * Process a Hello packet from an adjacent node.  
 *------------------------------------------------------------------
 */
void
smrp_ProcessHello (UnicastAddr naddr, SMRPPort* port, char *nodestate)
{
    NodeState neighborstate;
    
    smrp_UpdateAdjacentNode(naddr, port->PortID, MCHelloDatagram);

    /* Now do some sanity checking of neighbor's state */
    neighborstate = (NodeState) *nodestate;
    
    if ((neighborstate == MCPrimaryOperation
	 && smrp_IsOurNodePortPrimary(port))) {
	smrp_SetPortPrimary(port, 0);
	smrp_DoPortSecondaryNegotiation(port);
    } else if (neighborstate == MCSecondaryOperation
		&& smrp_IsOurNodePortSecondary(port)) {
	smrp_DoPortSecondaryNegotiation(port);
    } else if (neighborstate == MCNormalOperation)
	if (naddr == port->PrimaryNodeAddress) {
	    smrp_SetPortPrimary(port, 0);
	    smrp_DoPortSecondaryNegotiation(port);
	} else if (naddr == port->SecondaryNodeAddress) {
	    if (port->State == MCNormalOperation)
		smrp_DoPortSecondaryNegotiation(port);
	}
}


/*
 *------------------------------------------------------------------
 * smrp_CheckAdjacentNode		(SMRP Internal Function)
 *
 * Check the node to verify that it is still operational.  Called
 * for each node on every Adjacent Node Timer.
 *------------------------------------------------------------------
 */
void
smrp_CheckAdjacentNode (SMRPNode* node)
{
    SMRPPort* port;
    char buf[30];
    
    if (!node)
        return;

    if (ELAPSED_TIME(node->HelloTime) > smrp_nodeAgeInterval) {
	port = smrp_FindPort(node->PortID);
	if (port == NULL) {
	    smrp_DeleteNode(node);
	    return;
	}

        /* Neighbor is already down */
        if (node->State == MCNotOperational)
            return;

        /* TESTMODE - Don't try to be Primary or Secondary */
        if (smrp_test_running)
            return;

        /* neighbor debugging */
        if (smrp_debug(smrp_neighbor_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Neighbor %s, no longer up",
                PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

	smrp_ProcessDownNode(node, node->State);
	
    }
}

/*
 *------------------------------------------------------------------
 * smrp_AgeNode				(Module TreeProc Routine)
 *
 * Perform all periodic node operations for a node.
 *------------------------------------------------------------------
 */
static boolean
smrp_AgeNode (treeLink* link, void* d)
{
    SMRPNode* node = (SMRPNode*) link;
    
    smrp_CheckAdjacentNode(node);
    return (TRUE);
    
}

/*
 *------------------------------------------------------------------
 * smrp_AgeAllNodes			(SMRP Internal Function)
 *
 * Perform periodic operations on all nodes.  Invoked by the SMRP
 * maintenance process every smrp_nodeAgerInterval.
 *------------------------------------------------------------------
 */
void
smrp_AgeAllNodes ()
{
    RBTreeForEachNode(smrp_AgeNode,(void*)NULL,smrp_AdjacentNodeTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_DeletePortNode			(Module TreeProc Routine)
 *
 * Deletes the node if it is associated with the specified port.
 *
 * Assumes that all shared resources, such as routes, have been
 * invalidated previously.  Nodes usually exist indefinitely but
 * they must be discarded if the port is deleted.
 *------------------------------------------------------------------
 */
static boolean
smrp_DeletePortNode (treeLink* link, void* p)
{
    SMRPPort* port = (SMRPPort*) p;
    SMRPNode* node = (SMRPNode*) link;
    if (node->PortID == port->PortID)
	smrp_DeleteNode(node);
    return (TRUE);
    
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteNodesForPort		(SMRP Internal Function)
 *
 * Remove all nodes associated with the specified port.  Other
 * resources such as routes must be cleaned up seperately since
 * we do not do so automatically here.
 *------------------------------------------------------------------
 */
void
smrp_DeleteNodesForPort (SMRPPort* port)
{
    RBTreeForEachNode(smrp_DeletePortNode,(void*)port,
		      smrp_AdjacentNodeTable,FALSE);
}

static boolean
smrp_TreeDeleteNode (treeLink *link, void *data)
{
    SMRPNode *node = (SMRPNode *) link;

    smrp_DeleteNode(node);
    return TRUE;
}

void
smrp_DeleteAllNodes (void)
{
    RBTreeForEachNode(smrp_TreeDeleteNode, NULL, smrp_AdjacentNodeTable,
		      FALSE);
}


/*
 *------------------------------------------------------------------
 * smrp_MarkPortNode			(Module TreeProc Routine)
 *
 * Forces the state of each node to down if it is for this port.
 *------------------------------------------------------------------
 */
static boolean
smrp_MarkPortNode (treeLink* link, void* p)
{
    SMRPPort* port = (SMRPPort*) p;
    SMRPNode* node = (SMRPNode*) link;
    if (node->PortID == port->PortID)  {
	smrp_SetNodeState(node, MCNotOperational);
	node->RouteVersion = 0;
	node->NeedsRoutes = TRUE;
    }
    
    return (TRUE);
    
}	

/*
 *------------------------------------------------------------------
 * smrp_MarkPortNodesDown		(SMRP Internal Function)
 *
 * When a port is closed, the state of all associated nodes is
 * changed to MCNotOperational.  We do not worry about the
 * node designations since the port is also down.
 *------------------------------------------------------------------
 */
void
smrp_MarkPortNodesDown (SMRPPort* port)
{
    RBTreeForEachNode(smrp_MarkPortNode, port,
		      smrp_AdjacentNodeTable, FALSE);
}
	
/*
 *------------------------------------------------------------------
 * smrp_ForAllNodesDo			(SMRP Internal Function)
 *
 * Performs the treeProc for each adjacent node.
 *------------------------------------------------------------------
 */
void
smrp_ForAllNodesDo (treeProc tp, void* data)
{
    RBTreeForEachNode(tp, data, smrp_AdjacentNodeTable,FALSE);
}

boolean
smrp_IsNodeOperational (SMRPNode *node)
{
    return (node->State != MCNotOperational);
}
/*
 * The vector with distance = 254 is sent only to other nodes
 * on the same port to force distance vector updates from them.
 */
static boolean smrp_SendSpecialDistanceVector (treeLink *link, void *p)
{
    SMRPPort* port = (SMRPPort*) p;
    SMRPNode* node = (SMRPNode*) link;

    if (node->PortID == port->PortID) 
	smrp_SendRoutingTable(node,port);
    
    return TRUE;
}

/*
 * Query all nodes on the same port for better port parents.
 */
void smrp_QueryParentNodes(SMRPPort *port)
{
    RBTreeForEachNode(smrp_SendSpecialDistanceVector, (void*)port,
		      smrp_AdjacentNodeTable, FALSE);
}

