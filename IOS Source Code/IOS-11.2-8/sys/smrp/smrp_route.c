/* $Id: smrp_route.c,v 3.4.48.2 1996/08/28 13:09:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_route.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP routes.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_route.c,v $
 * Revision 3.4.48.2  1996/08/28  13:09:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.48.1  1996/08/28  00:51:28  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/08  04:56:00  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/17  18:54:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/10/12  18:43:10  kleung
 * CSCdi42084:  Some SMRP error messages do not follow convention.
 *
 * Revision 2.17  1995/10/11  20:28:07  mleelani
 * CSCdi41473:  Stale routes when the parent metric becomes worse
 * Treat it as a poison and renegotiate with neighbors.
 *
 * Revision 2.16  1995/10/11  19:02:30  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.15  1995/09/24  06:21:28  kleung
 * CSCdi40954:  Router crash in smrp_DeletePortFromRoute() when port
 * deconfiged.
 *
 * Revision 2.14  1995/09/15  22:06:30  mleelani
 * CSCdi39756:  2 forwarding paths exist when join to group across routers.
 * o Update port parents for directly connected networks also.
 * o Start PortParent negotiation whenever the port parent address
 *   has to be set to its own port address.
 * o Reduce the PortParent timer for faster convergence.
 *
 * Revision 2.13  1995/09/15  21:49:44  mleelani
 * CSCdi40089:  smrp test failed, some routes missed from the routing table
 * Node version set incorrectly. Fix code review comments.
 *
 * Revision 2.12  1995/08/30  22:43:37  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.11  1995/08/21  18:58:28  mleelani
 * CSCdi38584:  Delete route still in SMRP routing table.
 * On receiving a poison, wait for a hold down before propagating better
 * routes to ensure that we get the poison from the parent link for that
 * route.
 *
 * Revision 2.10  1995/08/01  00:23:02  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.9  1995/07/24  22:30:53  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.8  1995/07/06 03:06:50  kleung
 * CSCdi36742:  Spurious memory access when SMRP port is deconfigured.
 *
 * Revision 2.7  1995/07/05  21:39:16  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.6  1995/06/23  18:49:17  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  18:06:51  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.4  1995/06/20  07:18:04  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/14  22:55:14  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.2  1995/06/09  13:18:48  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:45:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../os/signal.h"
#include "interface.h"
#include "packet.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "mgd_timers.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

/*
 *------------------------------------------------------------------
 * Externals
 *------------------------------------------------------------------
 */
extern smrp_route_pid;

/*
 *------------------------------------------------------------------
 * Module private definitions
 *------------------------------------------------------------------
 */

static struct smrproute_globals_ {
    mgd_timer MasterTimer;	/* Master timer for route process */
    mgd_timer HoldDownTimer;	/* Timer for handling poisons */
    mgd_timer PurgeTimer;       /* For route purging */
    mgd_timer CacheTimer;       /* For FS Cache aging */
} *smrproute_globals;

#define smrp_routeMasterTimer     (&smrproute_globals->MasterTimer)
#define smrp_routeHoldDownTimer   (&smrproute_globals->HoldDownTimer)
#define smrp_routePurgeTimer      (&smrproute_globals->PurgeTimer)
#define smrp_ageCacheTimer        (&smrproute_globals->CacheTimer)


/*
 *------------------------------------------------------------------
 * Module private data and functions
 *------------------------------------------------------------------
 */

enum {
    SMRP_AGECACHE_TIMER,
    SMRP_HOLDDOWN_TIMER,
    SMRP_PORTPARENT_TIMER,
    SMRP_PURGEROUTE_TIMER
};
static rbTree* smrp_RoutingTable;
static unsigned long long smrp_RoutingVersion;
ulong* SMRPROUTE_stat;

/*
 * During holddown, this flag is used to prevent the node
 * from sending updates.
 */
int smrp_ignoreDistanceRequests = FALSE;

/*
 *------------------------------------------------------------------
 * smrp_NetRangeCompare			(Module Internal Function)
 *
 * Compare two SMRPNetRange's.
 *
 * Return NETRANGE_LESSTHAN if range, a, is completely less than range, b.
 * Return NETRANGE_EQUAL if range, a, overlaps range, b.
 * Return NETRANGE_GREATERTHAN if range, a, is completely greater than 
 *     range, b. 
 *
 * NOTE: Overlapping ranges return as if equal. 
 *------------------------------------------------------------------
 */
static int
smrp_NetRangeCompare (const SMRPNetRange* a, const SMRPNetRange* b)
{
    if ((a->Start == b->Start) && (a->End == b->End))
	return(NETRANGE_EQUAL);
    if (!((a->End < b->Start)||(b->End < a->Start)))
	return(NETRANGE_EQUAL);
    if (a->End < b->Start)
	return(NETRANGE_LESSTHAN);
    return(NETRANGE_GREATERTHAN);
}

/*
 *------------------------------------------------------------------
 * smrp_NetRangeIdentical		(Module Internal Function)
 *
 * Compare two SMRPNetRange's and return TRUE iff they are equal.
 *------------------------------------------------------------------
 */
static inline boolean
smrp_NetRangeIdentical (const SMRPNetRange* a, const SMRPNetRange* b)
{
    if ((a->Start == b->Start) && (a->End == b->End))
	return(TRUE);

    return(FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_RBTreeNetRangeMax		(Module Internal Function)
 *
 * Compare two routing table keys and return max key
 *------------------------------------------------------------------
 */
static void 
smrp_RBTreeNetRangeMax (treeKey *result, treeKey a, treeKey b)
{
    if (smrp_NetRangeCompare((SMRPNetRange*)a.p, (SMRPNetRange*)b.p)
	== NETRANGE_GREATERTHAN)
	result->p = a.p;
    else
	result->p = b.p;
}

/*
 *------------------------------------------------------------------
 * smrp_RBTreeNetRangeCompare		(Module Internal Function)
 *
 * Compare two routing table keys and return result.
 *------------------------------------------------------------------
 */
static int 
smrp_RBTreeNetRangeCompare (treeKey a, treeKey b)
{
    return (smrp_NetRangeCompare((SMRPNetRange*)a.p, 
				 (SMRPNetRange*)b.p));
}

inline ushort
smrp_RouteCount (void)
{
    return smrp_RoutingTable->activeNodes;
}

SMRPRoute *
smrp_GetFirstRoute (void)
{
    return ((SMRPRoute *) RBTreeFirstNode(smrp_RoutingTable));
}

/*
 *------------------------------------------------------------------
 * smrp_FindRoute			(SMRP Internal Function)
 *
 * Find a routing entry given its range.  We may find an overlapping
 * so you must compare the ranges after the find.
 *------------------------------------------------------------------
 */
SMRPRoute*
smrp_FindRoute (const SMRPNetRange* rrange)
{
    treeLink *link;
    treeKey key;
    SMRPNetRange range;
    
    range = *rrange;
    key.p = &range;
    link = RBTreeSearch(smrp_RoutingTable, key);
    return (SMRPRoute*) link;
}

/*
 *------------------------------------------------------------------
 * smrp_FindRouteByNet			(SMRP Internal Function)
 *
 * Find a routing entry that contains this network number.
 *------------------------------------------------------------------
 */
SMRPRoute*
smrp_FindRouteByNet (SMRPNetwork net)
{
    SMRPNetRange range;

    range.Start = net;
    range.End = net;

    return (smrp_FindRoute(&range));
}

/*
 *------------------------------------------------------------------
 * smrp_RouteChanged			(Module Internal Function)
 *
 * Change the version number of a routing entry and update the
 * UpdateTime.
 *------------------------------------------------------------------
 */
static void
smrp_RouteChanged (SMRPRoute* route)
{
    SMRPASSERT(route != NULL);
    route->RouteVersion = ++smrp_RoutingVersion;
    GET_TIMESTAMP(route->UpdateTime);
}

SMRPPortParentLink *
smrp_FindPortParentLink (SMRPRoute *route, SMRPPortID pid)
{
    SMRPPortParentLink *ppl;

    if (route == NULL)
	return NULL;

    ppl = route->PortParents;

    if (ppl == NULL)
	return NULL;

    while (ppl->PortID != pid) {
	ppl = ppl->Next;
	if (ppl == NULL)
	    return NULL;
    }
    return ppl;
}

static boolean
smrp_FixupPortParentLink (treeLink *link, void *data)
{
    SMRPRoute *route = (SMRPRoute *) link;
    SMRP_fn_data *criteria = (SMRP_fn_data *) data;
    UnicastAddr nodeaddress = criteria->matchaddr;
    SMRPPortParentLink *ppl;
    SMRPPort *port;
    
    ppl = route->PortParents;

    if (ppl == NULL)
	return TRUE;
    
    while (ppl->PortParentNodeAddress != nodeaddress) {
	ppl = ppl->Next;
	if (ppl == NULL)
	    return TRUE;
    }
    
    port = smrp_FindPort(ppl->PortID);
    if (!port)
	return TRUE;

    /*
     * Change the address to zero for the Port Parent Timeout period.
     */
    ppl->PortParentNodeAddress = port->ThisNodeAddress;
    ppl->Distance = MCMaximumDistance;
    
    smrp_RouteChanged(route);

    /*
     * Without changing the routing table, we will force
     * an update with distance = 254.
     */
    route->flags |= VECTOR_PORT_PARENT_UNREACHABLE;

    return TRUE;
}
    
void
smrp_FixupPortParentLinks (UnicastAddr nodeaddress)
{
    SMRP_fn_data data;
    
    data.matchaddr = nodeaddress;
    
    smrp_ForAllRoutesDo(smrp_FixupPortParentLink, &data);
}

/*
 *------------------------------------------------------------------
 * smrp_RouteGetPortParentAddress	(SMRP Internal Function)
 *
 * This function returns the route's PortParentNodeAddress for
 * a given SMRPPortID.
 *------------------------------------------------------------------
 */
UnicastAddr
smrp_RouteGetPortParentAddress (SMRPRoute* route, SMRPPortID pid)
{
    SMRPPortParentLink* ppl;

    SMRPASSERT(route != NULL);

    /* First see if we can get it from the ParentNodeAddress */
    if (route->ParentNodePortID == pid)
	return (route->ParentNodeAddress);

    ppl = smrp_FindPortParentLink(route, pid);

    if (ppl == NULL)
	return 0;

    return ppl->PortParentNodeAddress;
}

static ushort
smrp_RouteGetPortParentDistance (SMRPRoute *route, SMRPPortID pid)
{
    SMRPPortParentLink *ppl;

    /*First see if we can get it from the ParentNodeAddress */
    if (route->ParentNodePortID == pid)
	return (route->Distance);

    ppl = smrp_FindPortParentLink(route, pid);

    if (ppl == NULL)
	return MCMaximumDistance;

    return ppl->Distance;
}

/*
 *------------------------------------------------------------------
 * smrp_RouteAreWePortParentNode	(SMRP Internal Function)
 *
 * Returns TRUE if we are the parent node for the specified route.
 *------------------------------------------------------------------
 */
boolean
smrp_RouteAreWePortParentNode (SMRPRoute* route, SMRPPortID pid)
{
    SMRPPortParentLink* ppl;
    SMRPPort* port = smrp_FindPort(pid);

    if (port == NULL)
	return FALSE;

    SMRPASSERT(route != NULL);
    ppl = route->PortParents;
    SMRPASSERT(ppl != NULL);

    return (smrp_RouteGetPortParentAddress(route, pid)
	    == port->ThisNodeAddress);
}

/*
 *------------------------------------------------------------------
 * smrp_RouteSetPortParentInfo  	(Module Internal Function)
 *
 * This function changes the route's PortParentNodeAddress for
 * a given SMRPPortID.
 *------------------------------------------------------------------
 */
void
smrp_RouteSetPortParentInfo (SMRPRoute* route, SMRPPortID pid, 
			     UnicastAddr ppNodeAddress, ushort ppDistance)
{
    SMRPPortParentLink* ppl;

    SMRPASSERT(route != NULL);

    ppl = smrp_FindPortParentLink(route, pid);

    if (ppl == NULL)
	return;
    
    ppl->PortParentNodeAddress = ppNodeAddress;
    ppl->Distance = ppDistance;
}

static boolean
smrproute_IsFirstPreferred (UnicastAddr a, UnicastAddr b)
{
    return (a > b);
}


/*
 * Make sure we check, if the new route we received can
 * change the port parent link of other ports too, along
 * with the current port.
 */
static void smrp_checkStalePortParents (SMRPRoute* route, /* Current route */
					SMRPPortID pid,   /* Incoming port */
					uchar metric      /* incoming metric */
					)
{
    SMRPPortParentLink* ppl;
    SMRPPort*  p;
    
    ppl = route->PortParents;
    while (ppl != NULL) {
	p = smrp_FindPort(ppl->PortID);

	/*
	 * See if the port is different from the port we received the
	 * route. If so, remove the stale ppl if any.
	 */	
	if ((p) && (ppl->PortID != pid)) {
	    if (metric + 1 < route->Distance - 1) {
		/*
		 * We are anyway better, than all neighbors.
		 * Blindly change all ppls to point to us.
		 */
		smrp_RouteSetPortParentInfo(route, ppl->PortID,
					    p->ThisNodeAddress,
					    metric + 2);
	    } else if (metric + 1 == route->Distance - 1) {
		if (ppl->PortParentNodeAddress != p->ThisNodeAddress) {
		    if (smrp_RouteGetPortParentDistance(route,
							ppl->PortID) >
			metric + 2) {
			/*
			 * These were the neighbors who had the same
			 * distance as us to this route.
			 */
			smrp_RouteSetPortParentInfo(route, ppl->PortID,
					    p->ThisNodeAddress,
					    metric + 2);
		    } else {
			/*
			 * These were the neighbors who had better
			 * distance than us to this route.
			 */
			if (smrproute_IsFirstPreferred(p->ThisNodeAddress,
					    ppl->PortParentNodeAddress))
			    smrp_RouteSetPortParentInfo(route, ppl->PortID,
					    p->ThisNodeAddress,
					    metric + 2);
		    }
		}
	    }

	    /*
	     * All other cases we never change ppl addresses on
	     * ports except for the port in which the route arrived.
	     */
	}
	ppl = ppl->Next;
    }
}



/*
 *------------------------------------------------------------------
 * smrp_LinkPortToRoute			(Module Internal Function)
 *
 * This routine creates a PortParentLink for the route.
 *------------------------------------------------------------------
 */
static void
smrp_LinkPortToRoute (SMRPRoute* route, SMRPPort* port)
{
    SMRPPortParentLink* ppl;

    if (route->ParentNodePortID == port->PortID) {
	route->ParentNodeAddress = port->ThisNodeAddress;
    }

    ppl = smrp_FindPortParentLink(route, port->PortID);

    if (ppl) {
	ppl->PortParentNodeAddress = port->ThisNodeAddress;
	ppl->Distance = MCMaximumDistance;
	return;
    }
    
    ppl = malloc(sizeof(SMRPPortParentLink));
    if (ppl == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return;
    }
    ppl->Next = route->PortParents;
    ppl->PortID = port->PortID;
    ppl->PortParentNodeAddress = port->ThisNodeAddress;
    ppl->Distance = MCMaximumDistance;
    route->PortParents = ppl;
}

/*
 *------------------------------------------------------------------
 * smrp_AddPortToRoute			(Module TreeProc Routine)
 *
 * This routine is called for each port by smrp_AddPortsToRoute
 * to create a parent port link.
 *------------------------------------------------------------------
 */
static boolean
smrp_AddPortToRoute (treeLink* link, void* r)
{
    SMRPRoute* route = (SMRPRoute*) r;
    SMRPPort* port = (SMRPPort*) link;

    smrp_LinkPortToRoute(route,port);
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_AddAllPortsToRoute		(Module Internal Function)
 *
 * Invoked after a route has been created, we add a PortParentLink
 * for each port.  If ports are added or deleted later, an
 * associated PortParentLink is created or removed.
 *------------------------------------------------------------------
 */
static void
smrp_AddAllPortsToRoute (SMRPRoute* route)
{
    smrp_ForAllPortsDo(smrp_AddPortToRoute, route);
}

/*
 *------------------------------------------------------------------
 * smrp_AddPortByRoute			(Module TreeProc Routine)
 *
 * This routine is called for each route by smrp_AddPortToRoutes
 * to create a parent port link.
 *------------------------------------------------------------------
 */
static boolean
smrp_AddPortByRoute (treeLink* link, void* p)
{
    SMRPRoute* route = (SMRPRoute*) link;
    SMRPPort* port = (SMRPPort*) p;

    smrp_LinkPortToRoute(route,port);
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_AddPortToAllRoutes		(SMRP Internal Function)
 *
 * When a new port is created, a new SMRPPortParentLink structure
 * must be created for each route.
 *------------------------------------------------------------------
 */
void
smrp_AddPortToAllRoutes (SMRPPort* port)
{
    RBTreeForEachNode(smrp_AddPortByRoute,port,smrp_RoutingTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_DeletePortFromRoute		(Module TreeProc Routine)
 *
 * This routine is called to remove the PortParentLink for each 
 * route.
 *------------------------------------------------------------------
 */
static boolean
smrp_DeletePortFromRoute (treeLink* link, void* p)
{
    SMRPRoute* route = (SMRPRoute*) link;
    SMRPPort* port = (SMRPPort*) p;
    SMRPPortParentLink* ppl = route->PortParents;
    SMRPPortParentLink* nppl;

    if (ppl == NULL || port == NULL)
	return (TRUE);

    if (ppl->PortID == port->PortID) {
	route->PortParents = ppl->Next;
	free(ppl);
	return (TRUE);
    } 
    nppl = ppl->Next;
    while (nppl != NULL) {
	if (nppl->PortID == port->PortID) 
	    break;
	ppl = nppl;
	nppl = ppl->Next;
    }
    if (nppl == NULL) {
	smrp_Error("Port not found in Route/Port link");
	return (TRUE);
    }
    ppl->Next = nppl->Next;
    free(nppl);
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_DeletePortFromAllRoutes		(SMRP Internal Function)
 *
 * When a port is deleted, the associated SMRPPortParentLink 
 * must be deleted from each route.
 *------------------------------------------------------------------
 */
void
smrp_DeletePortFromAllRoutes (SMRPPort* port)
{
    RBTreeForEachNode(smrp_DeletePortFromRoute,port,
		      smrp_RoutingTable,FALSE);
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteRoutePortParents		(Module Internal Function)
 *
 * When a route is deleted, all of the port links are released.
 *------------------------------------------------------------------
 */
static void
smrp_DeleteRoutePortParents (SMRPRoute* route)
{
    SMRPPortParentLink* ppl = route->PortParents;
    SMRPPortParentLink* nppl;

    while (ppl != NULL) {
	nppl = ppl->Next;
	free(ppl);
	ppl = nppl;
    }
    route->PortParents = NULL;
}

/*
 *------------------------------------------------------------------
 * smrp_ClearRouteOpData		(Module Internal Function)
 *
 * Clear the data area pointer so that it will not be freed.  This
 * is done if the malloc'ed data was freed or given away.
 *------------------------------------------------------------------
 */
static void
smrp_ClearRouteOpData (SMRPRouteOp* op)
{
    op->DataPtr = op->DataArea = NULL;
}
/*
 *------------------------------------------------------------------
 * smrp_InitRouteOp			(Module Internal Function)
 *
 * Initializes a routing op block to specify all routes qualify.
 * Returns TRUE, if no data area requested or the data area of 
 * dsize bytes was malloc'ed sucessfully.
 *------------------------------------------------------------------
 */
boolean
smrp_InitRouteOp (SMRPRouteOp* op, int dsize)
{
    op->PortID = 0;
    op->NodeAddress = 0;
    op->LastVersion = 0;
    op->AreaSize = dsize;
    op->DataSize = 0;

    if (dsize > 0) {
	op->DataArea = malloc(dsize);
	op->DataPtr = op->DataArea;
        if (op->DataArea == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	}
	return (op->DataArea != NULL);
    } 
    op->DataPtr = op->DataArea = NULL;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_CleanupRouteOp			(Module Internal Function)
 *
 * Releases any resources still referenced by the RouteOp.
 *------------------------------------------------------------------
 */
void
smrp_CleanupRouteOp (SMRPRouteOp* op)
{
    if (op->DataArea != NULL)
	free(op->DataArea);
    smrp_ClearRouteOpData(op);
}

/*
 *------------------------------------------------------------------
 * smrp_ResetRouteOpData		(Module Internal Function)
 *
 * Reset the data pointer to the beginning of the data area.  The
 * used data size, stored in DataSize is not changed.
 *------------------------------------------------------------------
 */
void
smrp_ResetRouteOpData (SMRPRouteOp* op)
{
    op->DataPtr = op->DataArea;
				/* Do not reset used buffer size */
}

/*
 *------------------------------------------------------------------
 * smrp_RouteOpQualify			(Module Internal Function)
 *
 * Return TRUE if this route meets all specified criteria.
 *------------------------------------------------------------------
 */
static boolean
smrp_RouteOpQualify (SMRPRoute* route,SMRPRouteOp* op, int dataSizeNeeded)
{
    if ((op->PortID != 0) && (op->PortID != route->ParentNodePortID))
	return FALSE;
    if ((op->NodeAddress != 0)
	&& (op->NodeAddress != route->ParentNodeAddress))
	return FALSE;
    if ((op->LastVersion != 0) && (op->LastVersion >= route->RouteVersion))
	return FALSE;
    if ((dataSizeNeeded != 0)
	&& ((op->DataSize + dataSizeNeeded) > op->AreaSize))
	return FALSE;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_AddVector			(Module TreeProc Routine)
 *
 * Add a distance vector for each route that qualifies.
 *------------------------------------------------------------------
 */
static boolean
smrp_AddVector (treeLink* link, void* d)
{
    SMRPRoute* route = (SMRPRoute*) link;
    SMRPRouteOp* op = (SMRPRouteOp*) d;

    if (!smrp_RouteOpQualify(route,op,SMRP_VECTOR_SIZE))
	return (TRUE);

    op->DataSize += SMRP_VECTOR_SIZE;

    smrp_PutThreeBytes(op->DataPtr,route->Range.Start);
    op->DataPtr += 3;
    smrp_PutThreeBytes(op->DataPtr,route->Range.End);
    op->DataPtr += 3;

    /*
     * Check if any route->flags are set.
     * Currently, we have only one case: When we want to send a
     * vector with distance = PortParentUnreachable(254).
     *
     * Notice that we do not change the actual distance in the routing
     * table. We just generate the special distance packet. 
     */
    if ((route->flags) && (route->flags & VECTOR_PORT_PARENT_UNREACHABLE)) {
	*op->DataPtr++ = MCPortParentUnreachable;
    } else
        *op->DataPtr++ = route->Distance;

    *op->DataPtr++ = route->TunnelDistance;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_SendRoutingTable		(SMRP Internal Function)
 *
 * Send a routing table update to a node.  It will send a
 * partial update if the node's routing version is set.
 *------------------------------------------------------------------
 */
void
smrp_SendRoutingTable (SMRPNode* node,SMRPPort* port)
{
    SMRPRouteOp op;
    ushort nversion;
    int rtSize;
    int maxRt;

    /* TESTMODE - Don't send any routes */
    if (smrp_test_running) {
        return;
    }

    maxRt = (PORTCB_MaxPacketSize(port) - SMRP_HEADER_SIZE) / SMRP_VECTOR_SIZE;

    nversion = smrp_GetNodeRouteVersion(node);

    if (!smrp_InitRouteOp(&op, smrp_RouteCount() * SMRP_VECTOR_SIZE)) 
	return;

    op.LastVersion = nversion;

    smrp_ForAllRoutesDo(smrp_AddVector, &op);

    rtSize = maxRt * SMRP_VECTOR_SIZE;
    smrp_ResetRouteOpData(&op);
    if (op.DataSize <= rtSize) {
	/* Update fits in one packet */
	if (!smrp_CreateRequest(port, MCDistanceRequest, NullGroup,
			        node->NodeAddress, op.DataPtr, op.DataSize, 
                                0, 0, NULL)) {
	    smrp_CleanupRouteOp(&op);
	    return;
	}
	node->Routing_Stats[NODEROUTE_UPDATES_OUT]++;
	node->Routing_Stats[NODEROUTE_VECTORS_OUT] += op.DataSize/
            SMRP_VECTOR_SIZE;
        smrp_stat[SMRP_DISTANCE_OUT]++;
    } else {
				/* Update requires multiple packets */
	while (op.DataSize > rtSize) {
	    if (!smrp_CreateRequest(port, MCDistanceRequest, NullGroup,
				    node->NodeAddress, op.DataPtr, rtSize,
				    0, 0, NULL)) {
		smrp_CleanupRouteOp(&op);
		return;
	    }
	    node->Routing_Stats[NODEROUTE_VECTORS_OUT] += op.DataSize/
                SMRP_VECTOR_SIZE;

	    op.DataPtr += rtSize;
	    op.DataSize -= rtSize;

	    node->Routing_Stats[NODEROUTE_UPDATES_OUT]++;
            smrp_stat[SMRP_DISTANCE_OUT]++;
	}
				/* Send remaining data in final packet */
	smrp_CreateRequest(port, MCDistanceRequest, NullGroup,
			   node->NodeAddress, op.DataPtr, op.DataSize, 0, 0,
			   NULL);
	node->Routing_Stats[NODEROUTE_UPDATES_OUT]++;
	node->Routing_Stats[NODEROUTE_VECTORS_OUT] += op.DataSize/
            SMRP_VECTOR_SIZE;
        smrp_stat[SMRP_DISTANCE_OUT]++;
    }
    smrp_CleanupRouteOp(&op);
    smrp_SetNodeRouteVersion(node,smrp_RoutingVersion);
}

/*
 *------------------------------------------------------------------
 * smrp_DoNodeRoutingUpdate		(Module TreeProc Routine)
 *
 * Performs several node checks and if all tests succeed, calls
 * SendRoutingTable to generate a, possibly partial, update.
 *------------------------------------------------------------------
 */
static boolean
smrp_DoNodeRoutingUpdate (treeLink* link, void* n)
{
    SMRPNode* node = (SMRPNode*) link;
    SMRPNode* skip = (SMRPNode*) n;
    SMRPPort* port;

    if (node == skip) 
	return (TRUE);

    if (smrp_GetNodeRouteVersion(node) == smrp_RoutingVersion)
	return (TRUE);

    if (smrp_IsNodeOperational(node)) {
	port = smrp_FindPort(node->PortID);

    if (port == NULL) 
	    return (TRUE);

	smrp_SendRoutingTable(node,port);
    }
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_SendRoutingUpdateToOtherNodes	(Module Internal Function)
 *
 * Sends routing updates to all other operational nodes by walking
 * the adjacent node table.  Set skipme to the node that should
 * not be updated, if any.
 *------------------------------------------------------------------
 */
static void
smrp_SendRoutingUpdateToOtherNodes (SMRPNode* skipme)
{
    smrp_ForAllNodesDo(smrp_DoNodeRoutingUpdate, skipme);
}


#if NOT_USED
/*
 *------------------------------------------------------------------
 * smrp_IsPortRoute			(Module Internal Function)
 *
 * Return TRUE if this is the permanent route for a port.  When
 * a port is deleted or the network range is changed, this function
 * will return FALSE.
 *------------------------------------------------------------------
 */
static boolean
smrp_IsPortRoute (SMRPRoute* route)
{
    SMRPPort* p = smrp_FindPort(route->ParentNodePortID);

    if (p == NULL)
	return FALSE;
    if ((route->Range.Start == p->RangeStart) && 
	(route->Range.End == p->RangeEnd)) {
	SMRPASSERT(route->Distance == 0);
	return TRUE;
    }
    return (FALSE);
}
#endif

/*
 *------------------------------------------------------------------
 * smrp_CreateRoute			(SMRP Internal Function)
 *
 * Lookup route and verify net range is same.  If no route exists,
 * create a route and initialize as a local route.  
 *
 * Reurns NULL if net range overlaps an existing entry or no memory
 * is available.
 *------------------------------------------------------------------
 */
static SMRPRoute*
smrp_CreateRoute (SMRPPort* port,const SMRPNetRange* range)
{
    SMRPRoute* route;
    treeKey key;

    if (!smrp_IsValidNetRange(range)) {
	smrp_Error("Range is not valid");
	return NULL;
    }

    route = smrp_FindRoute(range);
    if (route != NULL) {
	if (smrp_NetRangeIdentical(range,&route->Range)) {
	    route->Distance = 0;
	    route->ParentNodePortID = port->PortID;
	    route->ParentNodeAddress = port->ThisNodeAddress;
	    route->TunnelDistance = 0;
	    smrp_DeleteRoutePortParents(route);
	    smrp_AddAllPortsToRoute(route);
	    return route;
	}
	smrp_Error("Range overlaps an existing route");
	return NULL;
    }
    route = (SMRPRoute*) RBTreeGetFreeNode(smrp_RoutingTable);
    if (route == NULL) {
	route = malloc(sizeof(SMRPRoute));
	if (route == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    return NULL;
	} else
	    SMRPROUTE_stat[SMRPROUTE_NEW_ROUTE]++;	
    } else
	SMRPROUTE_stat[SMRPROUTE_RECYCLED_ROUTE]++;

    route->Range.Start = range->Start;
    route->Range.End = range->End;
    route->Distance = 0;
    route->TunnelDistance = 0;
    route->ParentNodePortID = port->PortID;
    route->ParentNodeAddress = port->ThisNodeAddress;

    route->RouteVersion = 0;
    route->PortParents = NULL;

    key.p = &route->Range;

    RBTreeInsert(key, smrp_RoutingTable, &(route->link));
    smrp_AddAllPortsToRoute(route);
    smrp_RouteChanged(route);

    return route;
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteRoute			(Module Internal Function)
 *
 * Delete a route from the routing table.  The PortParents must
 * be freed prior to deleting.
 *------------------------------------------------------------------
 */
void
smrp_DeleteRoute (SMRPRoute* route)
{
    char buf[30];
    SMRPPort* port;

    SMRPASSERT(route != NULL);
    
    /* route debugging */
    port = smrp_FindPort(route->ParentNodePortID);
    if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK)) {
	buginf("\nSMRP: Route %s, deleted ",
	       smrp_PrintNetRange(&route->Range));
	if (port)
	    buginf("with parent port %s",
		   PORTCB_PrintNetAddress(port, buf,
					  route->ParentNodeAddress));
    }

    if (route->PortParents != NULL)
	smrp_DeleteRoutePortParents(route);
    RBTreeDelete(smrp_RoutingTable, &route->link);
}

/*
 *------------------------------------------------------------------
 * smrp_PortRouteChange			(SMRP Internal Function)
 *
 * When the state of an SMRP port changes, this routine notifies
 * the routing process to create and/or change the associated
 * route.
 *
 * If the port is not active, we mark the route as unreachable.
 *
 * NOTE: Prior to changing a port's configuration, call this routine
 * with upState=FALSE to insure the old route is poisoned so that it 
 * will age normally after the change.
 *------------------------------------------------------------------
 */
void
smrp_PortRouteChange (SMRPPort* port,boolean upState)
{
    SMRPRoute* route;
    SMRPNetRange range;

    SMRPASSERT(port != NULL);
    range.Start = port->RangeStart;
    range.End = port->RangeEnd;
    SMRPASSERT(smrp_IsValidNetRange(&range));

    route = smrp_FindRoute(&range);
    if (route) {
	if (!smrp_NetRangeIdentical(&range, &route->Range)){
	    smrp_Error("Net range conflicts");
	    return;
	}
	if (route->ParentNodePortID != port->PortID)
	    if (route->Distance == 0) {
		smrp_Error("Route defined on another port");
		return;
	    } else
		route->ParentNodePortID = port->PortID;
	
	route->Distance = 0;
	route->TunnelDistance = 0;
        smrp_DeleteRoutePortParents(route);
        smrp_AddAllPortsToRoute(route);

	/*
	 * This will propagate the new connected route to
	 * all ports other than the one coming up.
	 */
	smrp_RouteChanged(route);
	smrp_SendRoutingUpdateToOtherNodes(NULL);

	/*
	 * Now Make sure we send 254 on all other ports,
	 * to sync up port parents.
	 */
	route->flags |= VECTOR_PORT_PARENT_UNREACHABLE;
	smrp_RouteChanged(route);
	smrp_SendRoutingUpdateToOtherNodes(NULL);
	route->flags &= !VECTOR_PORT_PARENT_UNREACHABLE;
	
        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, created as local network",
                   smrp_PrintNetRange(&range));
	return;
    }
    
    route = smrp_CreateRoute(port,&range);
    if (route == NULL) {
	smrp_Error("smrp_PortRouteChange: Unable to create route for port");
	return;
    }
	
    /* route debugging */
    if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Route %s, created as local network",
               smrp_PrintNetRange(&range));

    SMRPASSERT(route->Distance == 0);
    SMRPASSERT(route->ParentNodePortID == port->PortID);
    SMRPASSERT(route->ParentNodeAddress == port->ThisNodeAddress);
    
    /*
     * This will propagate the new connected route to
     * all ports other than the one coming up.
     */
    smrp_RouteChanged(route);
    smrp_SendRoutingUpdateToOtherNodes(NULL);
    
    /*
     * Now Make sure we send 254 on all other ports,
     * to sync up port parents.
     */
    route->flags |= VECTOR_PORT_PARENT_UNREACHABLE;
    smrp_RouteChanged(route);
    smrp_SendRoutingUpdateToOtherNodes(NULL);
    route->flags &= !VECTOR_PORT_PARENT_UNREACHABLE;
}

/*
 * Handler on expiry of the holddown timer.
 * Send distance vectors to all neighbors.
 */
static void
smrp_HandleHolddownTimeout (void)
{
    smrp_SendRoutingUpdateToOtherNodes(NULL);
    smrp_ignoreDistanceRequests = FALSE;
}

/*
 * Start Hold Down timer on receiveing a poison from
 * a non parent link.
 */
void
smrp_StartHoldDownTimer (void)
{
    smrp_ignoreDistanceRequests = TRUE;
    mgd_timer_start(smrp_routeHoldDownTimer, HolddownInterval);
}


/*
 *------------------------------------------------------------------
 * smrp_AgeRoute			(Module TreeProc Routine)
 *
 * Examines each route and if it has been down for more than
 * smrp_purgeRouteInterval, the route is discarded.  We do not
 * report this to our adjacent nodes.
 *
 * Routes associated with a port are never discarded unless the
 * port is deleted, then it will age out normally.
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */
static boolean
smrp_AgeRoute (treeLink* link, void* data)
{
    SMRPRoute* route = (SMRPRoute*)link;

#if NOT_USED
    if (smrp_IsPortRoute(route)) 
	return (TRUE);
#endif
    if (route->Distance == MCMaximumDistance) {
	if (CLOCK_OUTSIDE_INTERVAL(route->UpdateTime,
				   smrp_purgeRouteInterval)) {
	    smrp_DeleteRoute(route);
	}
    }
    process_may_suspend();
    return (TRUE);
}

/*
 * Handle timers related to routes.
 */
static void
smrp_route_do_timers (void)
{
    mgd_timer *RouteTimer = smrp_routeMasterTimer;

    while (mgd_timer_expired(RouteTimer)) {
	mgd_timer *expired_timer;
	
	expired_timer = mgd_timer_first_expired(RouteTimer);
	switch (mgd_timer_type(expired_timer)) {
	  case SMRP_HOLDDOWN_TIMER:
	    mgd_timer_stop(expired_timer);
	    smrp_HandleHolddownTimeout();
	    break;

	  case SMRP_PURGEROUTE_TIMER:
	    mgd_timer_stop(expired_timer);
            smrp_ForAllRoutesDo(smrp_AgeRoute, NULL);
            mgd_timer_start(smrp_routePurgeTimer, smrp_purgeRouteInterval);
            break;
	    
	  case SMRP_AGECACHE_TIMER:
	    mgd_timer_start(expired_timer, ONEMIN);
	    smrpfast_fscache_age();
            break;
	    
	  default:
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}


/*
 *------------------------------------------------------------------
 * smrp_ProcessDistanceVector		(SMRP Internal Function)
 *
 * Process a distance vector from node.  
 *
 * Returns FALSE on formatting errors to stop further processing.
 *------------------------------------------------------------------
 */
static boolean
smrp_ProcessDistanceVector (SMRPNode* node, SMRPPort* port, SMRPVector* v,
			    uchar *refresh_needed)
{
    SMRPRoute* route = NULL;
    char buf[30], buf2[30];
    UnicastAddr ppaddr;

    node->Routing_Stats[NODEROUTE_VECTORS_IN]++;

    if (!smrp_IsValidNetRange(&v->Range)) {

        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, from %s has invalid range",
                smrp_PrintNetRange(&v->Range),
  	        PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

	node->Routing_Stats[NODEROUTE_BADRANGE]++;
	return FALSE;
    }
    if ((v->TunnelDistance > MCMaximumDistance)
	|| (v->Distance > MCMaximumDistance)) {

        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, from %s has invalid distance value",
                smrp_PrintNetRange(&v->Range),
  	        PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

	return FALSE;
    }
#if NOT_USED
    if (!smrp_IncomingRouteAccessCheck(port,node,&v->Range)) {
	node->Routing_Stats[NODEROUTE_ACCESS_DENIED]++;
	return TRUE;
    }
#endif
    route = smrp_FindRoute(&v->Range);
    if (route == NULL) {
        /* Don't create poisoned route */
	if (v->Distance == MCMaximumDistance) {
            /* route debugging */
            if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                buginf("\nSMRP: Route %s, unknown route poisoned by %s, ignored",
                    smrp_PrintNetRange(&v->Range),
                    PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

            node->Routing_Stats[NODEROUTE_POISONED]++;
            return TRUE;
	} else if (v->Distance == MCPortParentUnreachable)
	    return TRUE;
    
	route = smrp_CreateRoute(port, &v->Range);
	if (route == NULL) 
	    return TRUE;

	route->Distance = v->Distance + 1;
	route->TunnelDistance = v->TunnelDistance;
	route->ParentNodePortID = port->PortID;
	route->ParentNodeAddress = node->NodeAddress;
	smrp_RouteSetPortParentInfo(route, port->PortID, node->NodeAddress,
				    route->Distance);
	smrp_RouteChanged(route);

        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, created via %s - hop %d tunnel %d",
                smrp_PrintNetRange(&route->Range),
  	        PORTCB_PrintNetAddress(port, buf, node->NodeAddress),
                route->Distance,
                route->TunnelDistance);

	return TRUE;
    }

    if (!smrp_NetRangeIdentical(&route->Range, &v->Range)) {

        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, from %s - overlaps existing route %s",
                smrp_PrintNetRange(&route->Range),
  	        PORTCB_PrintNetAddress(port, buf, node->NodeAddress),
                smrp_PrintNetRange(&v->Range));

	node->Routing_Stats[NODEROUTE_OVERLAPS_EXISTING]++;
	return TRUE;
    }
    /*
     * If we get the special packet asking us for an update,
     * send the routing table to that node and return;
     */
    if (v->Distance == MCPortParentUnreachable) {
	smrp_RouteChanged(route);
	*refresh_needed = REPLY_ALL;
	return TRUE;
    }

    if (v->Distance == MCMaximumDistance) {
	if (route->Distance != MCMaximumDistance)
	    if (route->ParentNodeAddress == node->NodeAddress) {
		node->Routing_Stats[NODEROUTE_POISONED]++;
		route->Distance = MCMaximumDistance;
		smrp_RouteSetPortParentInfo(route, route->ParentNodePortID,
					    port->ThisNodeAddress,
					    MCMaximumDistance);
		smrp_RouteChanged(route);
                /* route debugging */
                if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                    buginf("\nSMRP: Route %s, poisoned by %s",
                           smrp_PrintNetRange(&route->Range),
  	                   PORTCB_PrintNetAddress(port, buf, node->NodeAddress));
	    } else {
		/*
		 *  We set the PortParentAddress to our node
		 *  address, but we *do* depend on updates from
		 *  other routers on the sme port, to get the
		 *  correct ppl eventually. 
		 */
 	        if (port->PortID != route->ParentNodePortID) {
		    smrp_RouteSetPortParentInfo(route, node->PortID,
						port->ThisNodeAddress,
						MCMaximumDistance);
		    smrp_RouteChanged(route);

		    /* We have refresh all nodes, but use holddown */
		    *refresh_needed = REPLY_ALL_WITH_HOLDDOWN;
		}
		
                /* route debugging */
                if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                    buginf("\nSMRP: Route %s, poison notification from %s",
                           smrp_PrintNetRange(&route->Range),
  	                   PORTCB_PrintNetAddress(port, buf, node->NodeAddress));

            }
    } else if ((v->Distance >= route->Distance) &&
	       (route->ParentNodeAddress == node->NodeAddress)) {
	/*
	 * Our parent's route has actually become worse.
	 * Treat it as a poison, but send it to the parent also so that
	 * it will respond with the correct vector at the end of its holddown.
	 */
	node->Routing_Stats[NODEROUTE_POISONED]++;
	route->Distance = MCMaximumDistance;
	smrp_RouteSetPortParentInfo(route, route->ParentNodePortID,
				    port->ThisNodeAddress,
				    MCMaximumDistance);
	smrp_RouteChanged(route);
	/* route debugging */
	if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
	    buginf("\nSMRP: Route %s, worsened by parent %s",
		   smrp_PrintNetRange(&route->Range),
		   PORTCB_PrintNetAddress(port, buf, node->NodeAddress));
	*refresh_needed = REPLY_ALL;
    } else if (v->Distance + 1 < route->Distance) {
	node->Routing_Stats[NODEROUTE_IMPROVED]++;
        smrp_checkStalePortParents(route, node->PortID /*the new parent port*/,
				   v->Distance);
        /* route debugging */
        if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Route %s, improved via %s - %d->%d hop, %d->%d tunnel",
                smrp_PrintNetRange(&route->Range),
  	        PORTCB_PrintNetAddress(port, buf, node->NodeAddress),
		route->Distance, v->Distance+1,
                route->TunnelDistance, v->TunnelDistance);

	route->Distance = v->Distance + 1;
	route->TunnelDistance = v->TunnelDistance;
	route->ParentNodeAddress = node->NodeAddress;
	route->ParentNodePortID = node->PortID;
	smrp_RouteSetPortParentInfo(route, route->ParentNodePortID,
				    route->ParentNodeAddress, route->Distance);
	smrp_RouteChanged(route);

    } else if (v->Distance + 1 == route->Distance) {
	if (smrproute_IsFirstPreferred(node->NodeAddress,
				       route->ParentNodeAddress)) {

	    node->Routing_Stats[NODEROUTE_BETTERPARENTINT]++;

            /* route debugging */
            if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                buginf("\nSMRP: Route %s, switched to %s - higher address than %s",
                    smrp_PrintNetRange(&route->Range),
  	            PORTCB_PrintNetAddress(port, buf, node->NodeAddress),
  	            PORTCB_PrintNetAddress(port, buf2, route->ParentNodeAddress));

	    route->Distance = v->Distance + 1;
	    route->TunnelDistance = v->TunnelDistance;
	    route->ParentNodeAddress = node->NodeAddress;
	    route->ParentNodePortID = node->PortID;
	    smrp_RouteSetPortParentInfo(route, route->ParentNodePortID,
					route->ParentNodeAddress,
					route->Distance);
	    smrp_RouteChanged(route);
	} else {
	    node->Routing_Stats[NODEROUTE_WORSTPARENTINT]++;

	    /* This node is not the overall preferred parent, but we
	     * should defer to it on this port.
	     */
	    ppaddr = smrp_RouteGetPortParentAddress(route, node->PortID);
	    if ((ppaddr == port->ThisNodeAddress) ||
		(smrproute_IsFirstPreferred(node->NodeAddress,
					    ppaddr)))
		smrp_RouteSetPortParentInfo(route, node->PortID,
					    node->NodeAddress,
					    route->Distance);
	}
    } else if ((v->Distance == route->Distance) &&
	       (node->PortID != route->ParentNodePortID) &&
	       (v->Distance < smrp_RouteGetPortParentDistance(route,
							     port->PortID))) {

        ppaddr = smrp_RouteGetPortParentAddress(route, port->PortID);
	if (smrproute_IsFirstPreferred(node->NodeAddress, ppaddr)) {
	    node->Routing_Stats[NODEROUTE_BETTERPARENTADDR]++;

            /* route debugging */
            if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                buginf("\nSMRP: Route %s, parent port changed %s -> %s",
                    smrp_PrintNetRange(&route->Range),
  	            PORTCB_PrintNetAddress(port, buf, ppaddr),
     	            PORTCB_PrintNetAddress(port, buf2, node->NodeAddress));

	    smrp_RouteSetPortParentInfo(route, port->PortID,
					node->NodeAddress, v->Distance + 1);
	    smrp_RouteChanged(route);
	} else {
	    node->Routing_Stats[NODEROUTE_WORSTPARENTADDR]++;
	}
    }
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DoDistanceVectorRequest		(SMRP Internal Function)
 *
 * Process a distance vector request from a node.  
 *------------------------------------------------------------------
 */
void
smrp_DoDistanceVectorRequest (SMRPPort *port, SMRPResponse* req)
{
    SMRPNode* node;
    uchar* pv;
    SMRPVector v;
    int size;
    uchar refresh_needed = REPLY_ALL_BUT_INCOMING;
    
    /* TESTMODE - Don't receive any routes */
    if (smrp_test_running) {
        return;
    }

    if (port == NULL) {
	smrp_Error("smrp_DoDistanceVectorRequest: bad port id in request");
	return;
    }
    
    node = smrp_UpdateAdjacentNode(req->Source, req->PortID, req->PacketType);
    if (node == NULL) {
	return;
    }

    if (smrp_GetNodeRouteVersion(node) == 0) {
	smrp_SendRoutingTable(node, port);
    }
	
    SMRPASSERT(node->PortID == port->PortID);

    /*
     * Since we wait to send updates during a holddown,
     * route version can get stale during the delay.
     * It will be restored, once the holddown expires.
     */
    if (!smrp_ignoreDistanceRequests)
	SMRPASSERT(node->RouteVersion == smrp_RoutingVersion);

    node->Routing_Stats[NODEROUTE_UPDATES_IN]++;
    size = req->RequestSize;
    pv = req->RequestData;
    while (size >= SMRP_VECTOR_SIZE) {
	v.Range.Start = smrp_GetThreeBytes(pv);
	pv += 3;
	v.Range.End = smrp_GetThreeBytes(pv);
	pv += 3;
	v.Distance = *pv++;
	v.TunnelDistance = *pv++;
	size -= SMRP_VECTOR_SIZE;
	if (!smrp_ProcessDistanceVector(node, port, &v, &refresh_needed)) {
            /* route debugging */
            if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
                errmsg(&msgsym(DEBUGMSG, SMRP), "SMRP bad distance vector");
	    break;
	}
    }
    smrp_SendConfirmResponse(req);

    /*
     * case 1:
     * If we received at least one poison form a non-parent link,
     * we have to send updates to all nodes including the incoming
     * link, but make sure we wait for a hold down period, so that
     * we do not propagate false routes and end up in a loop.
     *
     * case 2: When we get a special vector request for an update
     * in case a node has gone down (d = 254), refresh all.
     * The decision was made to refresh all, since it will lead
     * to easier convergence than refreshing only the incoming node.
     *
     * case 3: Normal scenario, when you want to update all links
     * except the incoming link.(employing split horizon).
     *
     */
    switch (refresh_needed) {
      case REPLY_ALL_WITH_HOLDDOWN:
	if (!smrp_ignoreDistanceRequests) {
	    smrp_StartHoldDownTimer();
	}
	return;
	
      case REPLY_ALL:
	if (!smrp_ignoreDistanceRequests) {
	    smrp_SendRoutingTable(node, port);
	}
	/* deliberate fall through */

      case REPLY_ALL_BUT_INCOMING:
	if (!smrp_ignoreDistanceRequests) {
	    smrp_SetNodeRouteVersion(node,smrp_RoutingVersion);
	    smrp_SendRoutingUpdateToOtherNodes(node);
	}
	break;

      default:
	break;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_PoisonRoute			(Module TreeProc Routine)
 *
 * Set qualified routes to MCUnreachable state.
 *------------------------------------------------------------------
 */
static boolean
smrp_PoisonRoute (treeLink* link, void* d)
{
    SMRPRoute* route = (SMRPRoute*) link;
    SMRPRouteOp* op = (SMRPRouteOp*) d;

    if (!smrp_RouteOpQualify(route,op,0))
	return (TRUE);

    route->Distance = MCMaximumDistance;
    
    smrp_RouteChanged(route);

     /* route debugging */
    if (smrp_debug(smrp_route_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Route %s, has been poisoned",
               smrp_PrintNetRange(&route->Range));

    return (TRUE);
    
}

/*
 *------------------------------------------------------------------
 * smrp_PoisonRoutesFromNode		(SMRP Internal Function)
 *
 * Marks all routes with the ParentNodeAddress of this node to
 * MCUnreachable. Then sends routing updates to all operational 
 * nodes.
 *------------------------------------------------------------------
 */
void
smrp_PoisonRoutesFromNode (SMRPNode* node)
{
    SMRPRouteOp op;

    smrp_InitRouteOp(&op,0);
    op.NodeAddress = node->NodeAddress;

    smrp_ForAllRoutesDo(smrp_PoisonRoute, &op);

    smrp_SendRoutingUpdateToOtherNodes(node);
    smrp_CleanupRouteOp(&op);
}

/*
 *------------------------------------------------------------------
 * smrp_PoisonRoutesForPort		(SMRP Internal Function)
 *
 * Marks all routes with the ParentNodePortID of pid to
 * MCUnreachable. Then sends routing updates to all operational 
 * nodes.
 *------------------------------------------------------------------
 */
void
smrp_PoisonRoutesForPort (SMRPPortID pid)
{
    SMRPRouteOp op;

    smrp_InitRouteOp(&op,0);
    op.PortID = pid;

    smrp_ForAllRoutesDo(smrp_PoisonRoute, &op);

    smrp_SendRoutingUpdateToOtherNodes(NULL);
    smrp_CleanupRouteOp(&op);
}

/*
 *------------------------------------------------------------------
 * smrp_ForAllRoutesDo		(SMRP Internal Function)
 *
 * Performs the treeProc for each routing entry.
 *------------------------------------------------------------------
 */
void
smrp_ForAllRoutesDo (treeProc tp, void* data)
{
    RBTreeForEachNode(tp, data,smrp_RoutingTable,FALSE);
}

/*
 * smrp_CheckNetRange
 *
 * Determine if a route exists that partially overlaps range or is 
 * directly connected on another port
 */

boolean
smrp_CheckNetRange (SMRPNetRange const *range)
{
    SMRPRoute *route;

    route = smrp_FindRoute(range);

    if (route) {
	if (route->Distance == MCMaximumDistance) {
	    smrp_DeleteRoute(route);
	    return TRUE;
	}	    
	if (!smrp_NetRangeIdentical(range, &route->Range)) {
	    printf("\n%%SMRP: Network range conflicts with %s", 
                smrp_PrintNetRange(&route->Range));
	    return FALSE;
	}
	if (route->Distance == 0) {
	    printf("%%SMRP: Network range defined on another port");
	    return FALSE;
	}
    }
    return (TRUE);
}

static void
smrp_route_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(smrp_routeMasterTimer, DISABLE);
    process_watch_queue(smrpRouteQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(smrpRouteQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&smrpRouteQ);
    smrp_route_pid = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_route				(SMRP Routing Process)
 *
 * This process handles distance vector packets and maintains
 * the routing table.
 *------------------------------------------------------------------
 */
process
smrp_route (void)
{
    ulong major, minor;
    paktype* pak;
    UnicastAddr src;
    SMRPGroupID grp;
    uchar pt;
    ushort seq;
    SMRPPort* port;
    MCHeader *header;

    /*
     * Startup
     */
    signal_permanent(SIGEXIT, smrp_route_teardown);
    smrpRouteQ = create_watched_queue("SMRP Routing packets", 0, 0);
    process_watch_queue(smrpRouteQ, ENABLE, RECURRING);
    process_watch_mgd_timer(smrp_routeMasterTimer, ENABLE);

    mgd_timer_start(smrp_routePurgeTimer, smrp_purgeRouteInterval);
    mgd_timer_start(smrp_ageCacheTimer, ONEMIN);

    while (TRUE) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch(major) {
	    case TIMER_EVENT:
		smrp_route_do_timers();
		break;
	    case QUEUE_EVENT:
		while ((pak = process_dequeue(smrpRouteQ)) != NULL) {
		    smrp_stat[SMRP_DISTANCE_IN]++;

		    port = smrp_FindPortFromPak(pak);
		    if (port == NULL) {
			smrp_Dispose(SMRP_BAD_PORTID, pak);
			continue;
		    }
		    
		    if (!PORTCB_CheckPacket(port,pak)) {
			smrp_Dispose(SMRP_INPUT_ERR, pak);
			continue;
		    }
		    
		    header = (MCHeader *)pak->transport_start;
		    pt = header->PacketType;
		    seq = GETSHORT(&header->SequenceNumber);
		    grp = GETLONG(&header->GroupAddress);
		    src = PORTCB_GetPacketSource(port,pak);

		    if (smrp_GetPacketModifiers(pt) == MCRequestCommand) {
			smrp_HandleRequest(port, pak, pt, grp, src, seq);
			/* packet is freed by smrp_HandleRequest */
			continue;
		    } else if (smrp_GetPacketModifiers(pt) != MCData) {
			smrp_HandleResponse(port, pak, pt, grp, src, seq);
			continue;
		    }
		    smrp_Dispose(SMRP_UNEXPECTED_PKT,pak);
		}
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
 * smrp_route_Initialize		(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_route_Initialize (void)
{
    int i;

    smrp_RoutingTable = RBTreeCreate("SMRP","SMRP", "Routing Tree",
				     NULL, &smrp_route_debug->flag);
    SMRPASSERT(smrp_RoutingTable != NULL);
    RBTreeNon32Bit(smrp_RoutingTable, smrp_RBTreeNetRangeMax, 
		   smrp_RBTreeNetRangeCompare);

    smrp_RoutingVersion = 1;
    SMRPROUTE_stat = malloc(sizeof(ulong) * SMRPROUTE_MAXTRAFFIC);
    if (SMRPROUTE_stat == NULL) {
        smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }

    smrproute_globals = malloc(sizeof(struct smrproute_globals_));

    if (!smrproute_globals)
	return;

    mgd_timer_init_parent(smrp_routeMasterTimer, NULL);
    mgd_timer_init_leaf(smrp_routeHoldDownTimer, smrp_routeMasterTimer,
			SMRP_HOLDDOWN_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(smrp_routePurgeTimer, smrp_routeMasterTimer,
			SMRP_PURGEROUTE_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(smrp_ageCacheTimer, smrp_routeMasterTimer,
			SMRP_AGECACHE_TIMER, NULL, FALSE);

    for (i=0; i < SMRPROUTE_MAXTRAFFIC; i++)
	SMRPROUTE_stat[i] = 0;
}

/*
 * The following two functions are for cleaning up
 * McPortParentUnreachable cruft that may be present.
 */
static boolean
smrp_FixupRoute (treeLink* link, void* data)
{
    SMRPRoute* route = (SMRPRoute*)link;

    if (route->flags)
	route->flags &= !VECTOR_PORT_PARENT_UNREACHABLE;
    return TRUE;
}

void smrp_FixupRouteDistances (void)
{
    smrp_ForAllRoutesDo(smrp_FixupRoute, NULL);   
}





