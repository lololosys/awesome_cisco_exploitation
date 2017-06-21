/* $Id: smrp_node.h,v 3.2 1995/11/17 18:53:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_node.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP Nodes (Neighbors).
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_node.h,v $
 * Revision 3.2  1995/11/17  18:53:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/30  22:43:33  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.3  1995/08/01  00:22:52  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/07/05  21:39:11  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.1  1995/06/07  22:45:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_NODE_H__
#define __SMRP_NODE_H__

/*
 *------------------------------------------------------------------
 * SMRP Node Routing Traffic Definitions
 *------------------------------------------------------------------
 */
enum {
    NODEROUTE_UPDATES_IN,
    NODEROUTE_UPDATES_OUT,
    NODEROUTE_VECTORS_IN,
    NODEROUTE_VECTORS_OUT,
    NODEROUTE_BADRANGE,
    NODEROUTE_OVERLAPS_EXISTING,
    NODEROUTE_IMPROVED,
    NODEROUTE_BETTERPARENTINT,
    NODEROUTE_WORSTPARENTINT,
    NODEROUTE_BETTERPARENTADDR,
    NODEROUTE_WORSTPARENTADDR,
    NODEROUTE_POISONED,
    NODEROUTE_MAXTRAFFIC	/* must be last */
    };

enum {
    SMRPNODE_NEW_NODES,
    SMRPNODE_RECYCLED_NODES,
    SMRPNODE_DELETED_NODES,
    SMRPNODE_MAXTRAFFIC		/* must be last */
    };

extern ulong* SMRPNODE_stat;

/*
 *------------------------------------------------------------------
 * SMRP Node data structure
 *
 * Only traffic counters may be modified outside of smrp_node.
 *------------------------------------------------------------------
 */

struct SMRPNode_ {
    struct	treeLink_	link;

    UnicastAddr NodeAddress;
    SMRPPortID	PortID;
    NodeState	State;	
    sys_timestamp HelloTime;

    unsigned long long RouteVersion;
    boolean	NeedsRoutes;
    ulong	Routing_Stats[NODEROUTE_MAXTRAFFIC];
};

/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_node.
 *------------------------------------------------------------------
 */

/*
 *------------------------------------------------------------------
 * SMRP Internal Entry Points defined by smrp_node.
 *------------------------------------------------------------------
 */
extern void smrp_node_Initialize(void);

extern void smrp_DeleteNodesForPort(SMRPPort*);
extern void smrp_MarkPortNodesDown(SMRPPort*);
extern SMRPNode *smrp_UpdateAdjacentNode(UnicastAddr,SMRPPortID, uchar);
extern void smrp_SetNodeState(SMRPNode*, NodeState);
extern SMRPNode* smrp_FindAdjacentNode(UnicastAddr);
extern void smrp_CheckAdjacentNode(SMRPNode*);
extern ushort smrp_GetNodeRouteVersion(SMRPNode*);
extern void smrp_SetNodeRouteVersion(SMRPNode*,ushort);
extern boolean smrp_IsNodeOperational(SMRPNode *node);
extern void smrp_ForAllNodesDo(treeProc,void*);
extern void smrp_AgeAllNodes(void);
extern void smrp_ProcessHello(UnicastAddr,SMRPPort*, char *);
extern void smrp_DoNotifyRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DeleteAllNodes(void);
extern void smrp_DeleteNode(SMRPNode*);
extern void smrp_QueryParentNodes(SMRPPort *);
#endif __SMRP_NODE_H__



