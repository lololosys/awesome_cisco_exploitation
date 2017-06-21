/* $Id: smrp_route.h,v 3.2 1995/11/17 18:54:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_route.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP routes.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_route.h,v $
 * Revision 3.2  1995/11/17  18:54:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/11  19:02:35  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.7  1995/09/15  21:49:47  mleelani
 * CSCdi40089:  smrp test failed, some routes missed from the routing table
 * Node version set incorrectly. Fix code review comments.
 *
 * Revision 2.6  1995/08/30  22:43:39  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.5  1995/08/21  18:58:30  mleelani
 * CSCdi38584:  Delete route still in SMRP routing table.
 * On receiving a poison, wait for a hold down before propagating better
 * routes to ensure that we get the poison from the parent link for that
 * route.
 *
 * Revision 2.4  1995/08/01  00:23:04  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.3  1995/07/05  21:39:19  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.2  1995/06/21  18:06:52  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.1  1995/06/07  22:45:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_ROUTE_H__
#define __SMRP_ROUTE_H__

struct SMRPResponse_;

/*
 *==================================================================
 * SMRP Routing Specific Definitions and Data Structures
 *==================================================================
 */

enum {
    NETRANGE_LESSTHAN = -1,
    NETRANGE_EQUAL = 0,
    NETRANGE_GREATERTHAN = 1,
    NETRANGE_OVERLAPS
};

/*
 * Bits  to identify special cases in routes.
 */
enum {
    VECTOR_PORT_PARENT_UNREACHABLE = 0x01,
};

/*
 * Different types of responses to Distance vectors received.
 */
enum {
    REPLY_ALL_BUT_INCOMING = 0,
    REPLY_ONLY_INCOMING = 1,
    REPLY_ALL_WITH_HOLDDOWN = 2,
    REPLY_ALL = 3
};


/*
 *------------------------------------------------------------------
 * SMRPNetRange - Complete route identifier (Key for routing table)
 *------------------------------------------------------------------
 */
typedef struct SMRPNetRange_ {
    SMRPNetwork	Start;
    SMRPNetwork	End;
} SMRPNetRange;

/*
 *------------------------------------------------------------------
 * SMRPRouteOp - Route selection and data used during tree walks.
 *------------------------------------------------------------------
 */
typedef struct SMRPRouteOp_ {
    SMRPPortID 	PortID;		/* Routing Selector iff != 0 */
    UnicastAddr NodeAddress;	/* Routing Selector iff != 0 */
    unsigned long long LastVersion;	/* Routing Selector iff != 0 */

    int		AreaSize;	/* Size of data area (Do not change) */
    uchar*	DataArea;	/* Pointer to data area (Do not change) */

    uchar*	DataPtr;	/* Pointer into data area */
    int		DataSize;	/* Size of used data area */
} SMRPRouteOp;

/*
 *------------------------------------------------------------------
 * SMRPPortParentLink - Many-To-Many link between routes and ports
 *------------------------------------------------------------------
 */
typedef struct SMRPPortParentLink_ {
    struct SMRPPortParentLink_* Next;
    SMRPPortID	PortID;
    UnicastAddr PortParentNodeAddress;
    ushort      Distance;
} SMRPPortParentLink;

/*
 *------------------------------------------------------------------
 * SMRPRoute - SMRP Routing Data Structure	(Extended RB Tree)
 *
 * All fields are private to the module.
 * Data is keyed on Range (non 32bit key).
 *------------------------------------------------------------------
 */
typedef struct SMRPRoute_ {
    struct treeLink_	link;

    SMRPNetRange Range;
    ushort	Distance;
    ushort      TunnelDistance;
    SMRPPortID	ParentNodePortID;
    UnicastAddr	ParentNodeAddress;

    SMRPPortParentLink* PortParents;

    sys_timestamp UpdateTime;
    unsigned long long RouteVersion;
    uchar flags;	/* Flags are used for special cases in
			   route propagation */
} SMRPRoute;

/*
 *------------------------------------------------------------------
 * SMRPVector - Extracted distance vector structure
 *------------------------------------------------------------------
 */
typedef struct SMRPVector_ {
    uchar		Distance;
    uchar		TunnelDistance;
    SMRPNetRange	Range;
} SMRPVector;

/*******************************************************************
 * Traffic statistics definitions
 *******************************************************************/
enum {
    SMRPROUTE_NEW_ROUTE,
    SMRPROUTE_RECYCLED_ROUTE,
    SMRPROUTE_DELETED_ROUTE,
    SMRPROUTE_MAXTRAFFIC		/* must be last */
    };

extern ulong* SMRPROUTE_stat;
extern int smrp_ignoreDistanceRequests;

/*
 *==================================================================
 * SMRP Internal Entry Points defined by smrp_route.
 *==================================================================
 */
extern void smrp_route_Initialize(void);
extern void smrp_route(void);

extern boolean smrp_CheckNetRange(SMRPNetRange const *);
extern void smrp_DeletePortFromAllRoutes(SMRPPort*);
extern SMRPRoute* smrp_FindRoute(const SMRPNetRange*);
extern SMRPRoute* smrp_FindRouteByNet(SMRPNetwork);
extern UnicastAddr smrp_RouteGetPortParentAddress(SMRPRoute*,SMRPPortID);
extern void smrp_RouteSetPortParentInfo(SMRPRoute *, SMRPPortID,
					UnicastAddr, ushort);
extern void smrp_PoisonRoutesForPort(SMRPPortID);
extern void smrp_PoisonRoutesFromNode(SMRPNode*);
extern void smrp_PortRouteChange(SMRPPort*,boolean);
extern void smrp_AddPortToAllRoutes(SMRPPort*);
extern boolean smrp_RouteAreWePortParentNode(SMRPRoute*,SMRPPortID);
extern void smrp_DoDistanceVectorRequest(SMRPPort *, SMRPResponse *);
extern void smrp_FixupPortParentLinks(UnicastAddr);
extern void smrp_FixupRouteDistances(void);
extern void smrp_SendRoutingTable(SMRPNode*,SMRPPort*);
extern inline ushort smrp_RouteCount(void);
extern void smrp_ForAllRoutesDo(treeProc,void*);
extern void smrp_DeleteRoute(SMRPRoute*);
extern boolean smrp_InitRouteOp(SMRPRouteOp *, int);
extern void smrp_ResetRouteOpData(SMRPRouteOp *);
extern void smrp_CleanupRouteOp(SMRPRouteOp *);
extern SMRPRoute *smrp_GetFirstRoute(void);
extern void smrp_StartHoldDownTimer(void);
extern SMRPPortParentLink *smrp_FindPortParentLink(SMRPRoute *route,
						   SMRPPortID pid);
static inline SMRPRoute *
smrp_GetNextRoute (SMRPRoute *route)
{
    if (route == NULL)
	return NULL;
    else
	return ((SMRPRoute *) RBTreeNextNode(&route->link));
}

#endif __SMRP_ROUTE_H__
