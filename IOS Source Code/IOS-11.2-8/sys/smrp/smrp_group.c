/* $Id: smrp_group.c,v 3.4.48.1 1996/03/18 21:47:44 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/smrp/smrp_group.c,v $
 *------------------------------------------------------------------
 * Support for SMRP Group and Forward entries.
 *
 * This module handles the Group Creation tables as well as the 
 * Group Forwarding table.  Group Creation tables are maintained
 * on a per port basis when we are the designated, primary or 
 * secondary, node for the port.  The Group Forwarding table is
 * shared by all ports for this node.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_group.c,v $
 * Revision 3.4.48.1  1996/03/18  21:47:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  10:42:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  17:10:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/08  04:55:55  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/20  22:46:47  snyder
 * CSCdi44202:  spellink errors
 *              Nonexistant -> Non-existent
 *
 * Revision 3.2  1995/11/17  18:53:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  21:25:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/11  19:02:18  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.11  1995/08/03  21:17:19  kleung
 * CSCdi38206:  SMRP member requests expires after poll timer.
 *
 * Revision 2.10  1995/08/01  00:22:45  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.9  1995/07/25  21:29:29  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.8  1995/07/18  19:47:39  kleung
 * CSCdi37260:  The first MC group should be randomly selected.
 *
 * Revision 2.7  1995/07/06  03:06:45  kleung
 * CSCdi36742:  Spurious memory access when SMRP port is deconfigured.
 *
 * Revision 2.6  1995/06/23  18:49:14  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  19:16:08  slin
 * CSCdi34668:  Long packets are not checked.
 *
 * Revision 2.4  1995/06/21  18:06:44  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.3  1995/06/20  07:18:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:55:01  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "config.h"
#include "ieee.h"
#include "../util/avl.h"
#include "mgd_timers.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include <ciscolib.h>
#include "../smf/smf_public.h"

/*
 *==================================================================
 * Module Private Definitions
 *==================================================================
 */

/* This structure is used to maintain information about what multicast
 * addresses we are listening to on an interface.
 */
struct SMRP_mc_node_ {
    avl_node_type avlnode;
    uchar mcaddr[IEEEBYTES];
    int uses;
};


/*
 *------------------------------------------------------------------
 * SMRP Group Module Traffic Definitions
 *------------------------------------------------------------------
 */

/*
 *==================================================================
 * Module private data and functions
 *==================================================================
 */
rbTree* smrp_GroupForwardingTable;
ulong* SMRPGROUP_stat;

static void smrp_SendRemoveGroupRequest(SMRPPort *, SMRPGroupID);
static void smrp_SendAddGroupRequest(SMRPPort *, UnicastAddr, SMRPGroupID,
				     UnicastAddr);
static void smrp_SendMemberRequestToPort(SMRPPort *, SMRPGroupID);
static void smrp_SendJoinRequest(SMRPPort *, SMRPGroupID, UnicastAddr);
static void smrp_SendLeaveRequest(SMRPPort *, SMRPGroupID, UnicastAddr);
static void smrp_ForwardResponseToPendingJoins(SMRPRequest *);
static void smrp_SendCreatorQueryRequest(SMRPPort *, UnicastAddr, SMRPGroupID);
static void smrp_GroupDeleteForwardNotify(SMRPForward *);
static void smrp_SetForwardState(SMRPForward *, ForwardState);
static void smrp_DeleteForward(SMRPGroupID);


/*
 *------------------------------------------------------------------
 * smrp_DisplayGroupAddr		(SMRP Internal Function)
 *
 * Print the group address.
 *------------------------------------------------------------------
 */
const char*
smrp_DisplayGroupAddr (SMRPGroupID gid)
{
    static char str[40];
    
    sprintf(str, "AT %d.%d", smrp_GroupNetFromID(gid), 
	    smrp_GroupSocketFromID(gid));
    return(str);

}

static boolean
smrp_ValidPortGroupNetwork (SMRPPort *port, SMRPNetwork net)
{
    return (net >= port->RangeStart && net <= port->RangeEnd);
}

static boolean
smrp_ValidPortGroupSocket (SMRPPort *port, SMRPSocket skt)
{
    return (skt >= MCSocketStart && skt <= MCSocketEnd);
}

/*
 *------------------------------------------------------------------
 * smrp_ValidPortGroupID		(Module Internal Function)
 *
 * Return TRUE if group ID is valid for this port.
 *
 *------------------------------------------------------------------
 */
static boolean
smrp_ValidPortGroupID (SMRPPort* port,SMRPGroupID gid)
{
    if (smrp_ValidPortGroupNetwork(port, smrp_GroupNetFromID(gid)) &&
	smrp_ValidPortGroupSocket(port, smrp_GroupSocketFromID(gid)))
	return TRUE;
    return FALSE;
}

/*
 *------------------------------------------------------------------
 * smrp_RandomPortGroupID		(Module Internal Function)
 *
 * Return the random but possibly used group ID for the port.
 *------------------------------------------------------------------
 */
static SMRPGroupID
smrp_RandomPortGroupID (SMRPPort* port)
{
    SMRPNetwork random_net;
    SMRPSocket random_skt;
    
    random_net = port->RangeStart + 
	smrp_Random((int) (port->RangeEnd - port->RangeStart + 1));
    random_skt = MCSocketStart +
	smrp_Random((int) (MCSocketEnd - MCSocketStart + 1));
    
    return smrp_MakeGroupID(random_net, random_skt);
}

/*
 *------------------------------------------------------------------
 * smrp_FirstPortGroupID		(Module Internal Function)
 *
 * Return the first valid but possibly used group ID for the port.
 *------------------------------------------------------------------
 */
static SMRPGroupID
smrp_FirstPortGroupID (SMRPPort* port)
{
    return smrp_MakeGroupID(port->RangeStart, MCSocketStart);
}

/*
 *------------------------------------------------------------------
 * smrp_NextPortGroupID			(Module Internal Function)
 *
 * Return the next valid but possibly used group ID for the port.
 * If the next ID is not valid, wrap back to the first ID.
 *
 * This routine assumes that ranges are consecutive.
 *------------------------------------------------------------------
 */
static SMRPGroupID
smrp_NextPortGroupID (SMRPPort* port,SMRPGroupID last)
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

    if (smrp_ValidPortGroupID(port, nextgid))
	return (nextgid);
    else
	return smrp_FirstPortGroupID(port);
}

/*
 *------------------------------------------------------------------
 * smrp_StopMemberTimer			(Module Internal Routine)
 *
 * Stop the member timer.
 *------------------------------------------------------------------
 */
static void
smrp_StopMemberTimer (SMRPForward* fwd)
{
    SMRPASSERT(fwd != NULL);
    mgd_timer_stop(&fwd->MemberTimer);
}

/*
 *------------------------------------------------------------------
 * smrp_StartMemberTimer		(Module Internal Routine)
 *
 * Start member timer in the forward entry.
 *------------------------------------------------------------------
 */
static void
smrp_StartMemberTimer (SMRPForward* fwd)
{
    SMRPASSERT(fwd != NULL);
    if (!mgd_timer_running_and_sleeping(&fwd->MemberTimer))
	mgd_timer_start(&fwd->MemberTimer, smrp_memberInterval);
}

/*
 *------------------------------------------------------------------
 * smrp_group_Initialize		(Module Initialization)
 *
 * Perform initialization of module private data.
 *------------------------------------------------------------------
 */
void
smrp_group_Initialize (void)
{
    int i;

    smrp_GroupForwardingTable = RBTreeCreate("SMRP", "SMRP", 
					     "Group Forwarding Tree",
					     smrp_PrintGroupKey, 
					     &smrp_group_debug->flag);
    SMRPASSERT(smrp_GroupForwardingTable != NULL);

    SMRPGROUP_stat = malloc(sizeof(ulong) * SMRPGROUP_MAXTRAFFIC);
    if (SMRPGROUP_stat == NULL) {
        smrp_stat[SMRP_NO_BUFFERS]++;
        return;
    }

    for (i=0; i < SMRPGROUP_MAXTRAFFIC; i++)
	SMRPGROUP_stat[i] = 0;
}

/*
 *==================================================================
 * Port Group Creation Table Routines
 *
 * Note: The protocol spec refers to a global Group Creation Table
 * but our implementation creates a Group Table for each port when
 * we are the designated primary or secondary node. This
 * reduces table size and eases its management.  We also use a
 * simple link list since we usually check for non-existance of
 * entries and seldom perform entry lookups.
 *==================================================================
 */

/*
 *------------------------------------------------------------------
 * smrp_FindPortGroup			(SMRP Internal Function)
 *
 * Return the group entry from the port's Group Creation Table for
 * the specified group ID.
 *------------------------------------------------------------------
 */
SMRPGroup* 
smrp_FindPortGroup (SMRPPort* port, SMRPGroupID gadr)
{
    SMRPGroup* grp = port->GroupTable;

    while (grp != NULL) {
	if (grp->GroupAddress == gadr)
	    return grp;
	grp = grp->Next;
    }
    return NULL;
}

/*
 *------------------------------------------------------------------
 * smrp_InitPortGroupData		(SMRP Internal Function)
 *
 * Initialize all group fields stored in the port.  Called during
 * port descriptor initialization.
 *------------------------------------------------------------------
 */
void
smrp_InitPortGroupData (SMRPPort* port)
{
    port->GroupTable = NULL;
    port->LastGroupID = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_NewPortGroup			(Module Internal Function)
 *
 * Assign a new group address for the port.  Return the created
 * group entry.
 *
 * If we are the primary and we have a secondary, send a AddGroup
 * request to the secondary node.
 *------------------------------------------------------------------
 */
static SMRPGroup*
smrp_NewPortGroup (SMRPPort* port, UnicastAddr creator)
{
    SMRPGroup* grp;
    SMRPGroupID firstID;

    SMRPASSERT(smrp_IsOurNodePortPrimary(port));
    if (!smrp_ValidPortGroupID(port,port->LastGroupID)) {
	port->LastGroupID = smrp_RandomPortGroupID(port);
    }
    firstID = port->LastGroupID;
    while (smrp_FindPortGroup(port, port->LastGroupID) != NULL) {
	port->LastGroupID = smrp_NextPortGroupID(port,port->LastGroupID);
	if (port->LastGroupID == firstID) {
	    SMRPGROUP_stat[SMRPGROUP_RANGE_FULL]++;
	    return NULL;
	}
    }
    grp = malloc(sizeof(SMRPGroup));
    if (grp == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return NULL;
    }
    grp->GroupAddress = port->LastGroupID;
    grp->CreatorAddress = creator;
    grp->CreatorPort = port->PortID;

    /* Note creation time */
    GET_TIMESTAMP(grp->CreateTime);

    /* Link timer into master timer */
    smrp_SetupCreatorTimer(grp);
    
    grp->Next = port->GroupTable;
    port->GroupTable = grp;
    SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_ADDED]++;

    return grp;
}

/*
 *------------------------------------------------------------------
 * smrp_DeletePortGroup			(Module Internal Function)
 *
 * Deletes the requested group entry from the Group Creation table.  
 *
 * If we are the primary and we have a secondary, send a RemoveGroup
 * request to the secondary node.
 *------------------------------------------------------------------
 */
static boolean
smrp_DeletePortGroup (SMRPPort* port, SMRPGroupID gid)
{
    SMRPGroup* g = port->GroupTable;
    SMRPGroup* ng;
    UnicastAddr secnode;
    SMRPForward *fwd;
    char buf[30];

    if (!g) {
	smrp_Error("Group address not found in port's Group Table");
	return FALSE;
    }

    if (g->GroupAddress == gid) {
	port->GroupTable = g->Next;
	mgd_timer_stop(&g->CreatorTimer);
	free(g);
    } else {
	ng = g->Next;
	while (ng != NULL) {
	    if (ng->GroupAddress == gid)
		break;
	    g = ng;
	    ng = g->Next;
	}
	if (ng == NULL) {
	    smrp_Error("Group address not found in the port's Group Table");
	    return FALSE;
	}
	g->Next = ng->Next;
	mgd_timer_stop(&ng->CreatorTimer);
	free(ng);
    }

    SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_FREED]++;

    if (smrp_IsOurNodePortPrimary(port)) {
	secnode = smrp_GetPortSecondary(port);
	if (secnode != 0) 
	    smrp_SendRemoveGroupRequest(port, gid);
    }

    /* Send delete request to all child ports */
    fwd = smrp_FindForward(gid);
    if (fwd)
	smrp_DeleteForward(fwd->GroupAddress);
    
    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
        buginf("\nSMRP: Group %s, deleted on port %s",
            smrp_DisplayGroupAddr(gid),
            PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteAllPortGroups		(SMRP Internal Function)
 *
 * Delete the entire Group Creation Table for the port.  This
 * is performed when we cease to be the authority or backup
 * designated node.  
 *
 * Note: No requests are sent as the result of this call.
 *------------------------------------------------------------------
 */
void
smrp_DeleteAllPortGroups (SMRPPort* port)
{
    SMRPGroup* grp = port->GroupTable;
    SMRPGroup* nxt = NULL;
    SMRPForward *fwd;
    
    port->GroupTable = NULL;

    while (grp != NULL) {
	nxt = grp->Next;

	fwd = smrp_FindForward(grp->GroupAddress);
	if (fwd)
	    smrp_DeleteForward(fwd->GroupAddress);
	
	mgd_timer_stop(&grp->CreatorTimer);
	free(grp);
	SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_FREED]++;
	grp = nxt;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_AddPortGroup			(Module Internal Function)
 *
 * Add a new group to the port's Group Creation Table.  
 *
 * Note: This is called during processing of a AddGroup request
 * from the port's primary node.
 *------------------------------------------------------------------
 */
static SMRPGroup* 
smrp_AddPortGroup (SMRPPort* port, SMRPGroupID gid, UnicastAddr creator)
{
    SMRPGroup* grp = smrp_FindPortGroup(port, gid);

    if (grp != NULL) {
	SMRPASSERT(grp->CreatorAddress == creator);
	return grp;
    }
    SMRPASSERT(smrp_IsOurNodePortSecondary(port));

    grp = malloc(sizeof(SMRPGroup));
    if (grp == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return NULL;
    }

    grp->GroupAddress = gid;
    grp->CreatorAddress = creator;
    grp->CreatorPort = port->PortID;

    GET_TIMESTAMP(grp->CreateTime);
    smrp_SetupCreatorTimer(grp);

    grp->Next = port->GroupTable;
    port->GroupTable = grp;

    port->LastGroupID = gid;

    SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_ADDED]++;
    return grp;
}

/*
 *==================================================================
 * Group Forwarding Table Routines
 *==================================================================
 */

SMRPForward *
smrp_GetFirstForward (void)
{
    return ((SMRPForward *) RBTreeFirstNode(smrp_GroupForwardingTable));
}

/*
 *------------------------------------------------------------------
 * smrp_FindForward			(SMRP Internal Function)
 *
 * Lookup the forward entry for a group id.  
 *------------------------------------------------------------------
 */
SMRPForward*
smrp_FindForward (SMRPGroupID gid)
{
    treeKey key;

    key.l = gid;
    return (SMRPForward*) RBTreeSearch(smrp_GroupForwardingTable,key);
}

/*
 *------------------------------------------------------------------
 * smrp_SetForwardState			(Module Internal Function)
 *
 * Set the Group Forward Table entry to the new state.
 *------------------------------------------------------------------
 */
static void
smrp_SetForwardState (SMRPForward* fwd, ForwardState new)
{
    SMRPASSERT(fwd != NULL);
    if (fwd->State != new) {
        /* group debugging */
        if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
	    buginf("\nSMRP: Group %s, forward state changed from %s to %s",
	        smrp_DisplayGroupAddr(fwd->GroupAddress),
	        smrp_ForwardStateMessage(fwd->State),
	        smrp_ForwardStateMessage(new));
    }
    fwd->State = new;
}

/*
 *------------------------------------------------------------------
 * smrp_CreateForward			(Module Internal Function)
 *
 * Create a Group Forwarding entry for a specific port id and 
 * initialize it.
 *------------------------------------------------------------------
 */
static SMRPForward*
smrp_CreateForward (SMRPGroupID gid, SMRPPortID pid)
{
    SMRPForward* fwd;
    SMRPRoute* route;
    treeKey key;

    route = smrp_FindRouteByNet(smrp_GroupNetFromID(gid));
    if (route == NULL) {
	SMRPGROUP_stat[SMRPGROUP_NO_ROUTE]++;
	smrp_Error("Cannot create forward entry - no route");
	return NULL;
    }
    fwd = (SMRPForward*) RBTreeGetFreeNode(smrp_GroupForwardingTable);
    if (fwd == NULL) {
	fwd = malloc(sizeof(SMRPForward));
	if (fwd == NULL) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    return NULL;
	} else {
	    SMRPGROUP_stat[SMRPGROUP_NEW_FORWARDS]++;
	    /* Link timer into master timer */
	    smrp_SetupMemberTimer(fwd);
	}
    } else {
	SMRPGROUP_stat[SMRPGROUP_RECYCLED_FORWARDS]++;
	if (mgd_timer_running(&fwd->MemberTimer))
	    smrp_Error("Recycled forward was found with running timer!");
    }
    
    fwd->GroupAddress = gid;
    fwd->ParentPortID = pid;
    fwd->ParentNodeAddress = smrp_RouteGetPortParentAddress(route,pid);
    fwd->State = MCJoining;
    fwd->ChildPortCount = 0;
    fwd->ChildPorts = NULL;

    /* fast switching info */
    fwd->FastEnabled = FALSE;
    fwd->CacheValid = FALSE;
    GET_TIMESTAMP(fwd->CacheUpdateTime);

    GET_TIMESTAMP(fwd->UpdateTime);

    key.l = fwd->GroupAddress;
    RBTreeInsert(key, smrp_GroupForwardingTable, &(fwd->link));
    return fwd;
}

/*
 *------------------------------------------------------------------
 * smrp_FindForwardChildPort		(Module Internal Function)
 *
 * Returns the requested PortID entry from the Group Forward entry.
 *------------------------------------------------------------------
 */
SMRPChildPortLink*
smrp_FindForwardChildPort (SMRPForward* fwd, SMRPPortID pid)
{
    SMRPChildPortLink* cp = fwd->ChildPorts;

    if (cp == NULL)
	return NULL;

    while (cp->ChildPortID != pid) {
	cp = cp->Next;
	if (cp == NULL)
	    break;
    }
    return cp;
}

/*
 *------------------------------------------------------------------
 * smrp_AddForwardChildPort		(Module Internal Function)
 *
 * Adds the requested PortID entry to the Group Forward entry.
 * Returns TRUE if sucessfully added or already exists.
 *------------------------------------------------------------------
 */
static boolean
smrp_AddForwardChildPort (SMRPForward* fwd, SMRPPortID pid)
{
    SMRPChildPortLink* cp = smrp_FindForwardChildPort(fwd, pid);
    SMRPPort* port;
    
    if (cp != NULL)
	return TRUE;

    if (pid == fwd->ParentPortID)
	return TRUE;

    cp = malloc(sizeof(SMRPChildPortLink));
    if (cp == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return FALSE;
    }

    cp->ChildPortID = pid;
    cp->Next = fwd->ChildPorts;
    fwd->ChildPortCount++;
    fwd->ChildPorts = cp;
    GET_TIMESTAMP(fwd->UpdateTime);

    /* Invalidate forward cache entry only if it can f/s */
    port = smrp_FindPort(pid);
    if (port && port->FastOkay)
	smrpfast_InvalidateFwdCache((treeLink*)fwd, NULL);

    SMRPGROUP_stat[SMRPGROUP_CHILDPORTS_ADDED]++;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_DeleteForwardChildPort		(Module Internal Function)
 *
 * Deletes the requested port entry from the Group Forward entry.  
 *------------------------------------------------------------------
 */
static void
smrp_DeleteForwardChildPort (SMRPForward* fwd, SMRPPortID pid)
{
    SMRPChildPortLink* cp = fwd->ChildPorts;
    SMRPChildPortLink* ncp;
    SMRPPort* port;
    
    if (cp->ChildPortID == pid) {
	fwd->ChildPorts = cp->Next;
	fwd->ChildPortCount--;
        GET_TIMESTAMP(fwd->UpdateTime);
	SMRPGROUP_stat[SMRPGROUP_CHILDPORTS_FREED]++;
	free(cp);
    } else {
	ncp = cp->Next;
	while (ncp != NULL) {
	    if (ncp->ChildPortID == pid)
		break;
	    cp = ncp;
	    ncp = cp->Next;
	}
	if (ncp == NULL) {
	    smrp_Error("Child port not found in Group Forward entry");
	    return;
	}
	cp->Next = ncp->Next;
	fwd->ChildPortCount--;
	GET_TIMESTAMP(fwd->UpdateTime);
	SMRPGROUP_stat[SMRPGROUP_CHILDPORTS_FREED]++;
	free(ncp);
    }
    /* Invalidate forward cache entry only if it can f/s */
    port = smrp_FindPort(pid);
    if (port && port->FastOkay)
	smrpfast_InvalidateFwdCache((treeLink*)fwd, NULL);

    GET_TIMESTAMP(fwd->UpdateTime);
}

static void
smrp_DeleteForward (SMRPGroupID gid)
{
    SMRPForward *fwd;
    SMRPChildPortLink *cp, *cpnext;
    SMRPPort *port;
    
    fwd = smrp_FindForward(gid);
    if (!fwd)
	return;

    if (fwd->State == MCForwarding) {
	smrp_GroupDeleteForwardNotify(fwd);
	port = smrp_FindPort(fwd->ParentPortID);
	if (port)
	    PORTCB_Unsubscribe(port, fwd->GroupAddress);
    }	

    cp = fwd->ChildPorts;
    
    while (cp != NULL) {
	cpnext = cp->Next;
	smrp_DeleteForwardChildPort(fwd, cp->ChildPortID);
	cp = cpnext;
    }
    
    SMRPASSERT(fwd->ChildPortCount == 0 && fwd->ChildPorts == NULL);
    
    smrp_StopMemberTimer(fwd);

    SMRPGROUP_stat[SMRPGROUP_DELETED_FORWARDS]++;

    if (fwd->FastEnabled)
	smrpfast_DeleteEntry(fwd);
    
    RBTreeDelete(smrp_GroupForwardingTable, &(fwd->link));

    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
        buginf("\nSMRP: Group %s, deleted forward entry",
            smrp_DisplayGroupAddr(gid));
}

static boolean
smrp_TreeDeleteMatchingForward (treeLink *link, void *data)
{
    SMRPForward *fwd = (SMRPForward *) link;
    SMRP_fn_data *criteria = (SMRP_fn_data *) data;
    SMRPRoute *route;
    
    if (fwd == NULL)
	return TRUE;
    route = smrp_FindRouteByNet(smrp_GroupNetFromID(fwd->GroupAddress));

    if (criteria->matchaddr && fwd->ParentNodeAddress != criteria->matchaddr)
	return TRUE;
    
    if (criteria->PortID && fwd->ParentPortID != criteria->PortID)
	return TRUE;
    
    if (criteria->localgroups && (!route || route->Distance != 0))
	return TRUE;

    if (criteria->nonlocalgroups && (!route || route->Distance == 0))
	return TRUE;

    smrp_GroupDeleteForwardNotify(fwd);
    smrp_DeleteForward(fwd->GroupAddress);
    return TRUE;
    
}

void
smrp_DeleteMatchingForwards (SMRPPortID pid, UnicastAddr matchaddr,
			     boolean localfwds, boolean nonlocalfwds)
{
    SMRP_fn_data data;
    
    memset(&data, 0, sizeof(SMRP_fn_data));
    data.PortID = pid;
    data.matchaddr = matchaddr;
    data.localgroups = localfwds;
    data.nonlocalgroups = nonlocalfwds;

    smrp_ForAllForwardsDo(smrp_TreeDeleteMatchingForward, &data);
}

/*******************************************************************
 * Request transaction routines
 *******************************************************************/

/*
 *------------------------------------------------------------------
 * smrp_DoCreateRequestPacket		(SMRP Internal Function)
 *
 * If we are the primary node for the port, process the request
 * and reply with the new group address.
 *------------------------------------------------------------------
 */
void
smrp_DoCreateRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    SMRPGroup* grp;
    char buf[30], buf2[30];
    UnicastAddr secnode;
    
    SMRPASSERT(res->GroupAddress == 0);

    smrp_stat[SMRP_CREATE_IN]++;
    
    if (!smrp_IsOurNodePortPrimary(port)) {
	smrp_SetResponseState(res,MCRequestComplete);
	smrp_DiscardResponse(res);
	SMRPGROUP_stat[SMRPGROUP_NOT_PRIMARY]++;
	return;
    }
    grp = smrp_NewPortGroup(port,res->Source);
    if (grp == NULL) {
	smrp_SendRejectResponse(res,MCRangeFull);

	return;
    }
    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Group %s, created on port %s by %s",
            smrp_DisplayGroupAddr(grp->GroupAddress),
            PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress),
            PORTCB_PrintNetAddress(port, buf2, res->Source));

    secnode = smrp_GetPortSecondary(port);
    if (secnode != 0) 
	smrp_SendAddGroupRequest(port, secnode, grp->GroupAddress,
				 grp->CreatorAddress);

    mgd_timer_start(&grp->CreatorTimer, smrp_creatorInterval);
    res->GroupAddress = grp->GroupAddress;
    smrp_SendConfirmResponse(res);    
}

/*
 *------------------------------------------------------------------
 * smrp_DoDeleteRequestPacket		(SMRP Internal Function)
 *
 * If we are the primary node for the port, process the request
 * if it came from the creator.
 *------------------------------------------------------------------
 */
void
smrp_DoDeleteRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    SMRPGroup* grp;
    SMRPForward *fwd;
    SMRPRequest *req;
    
    smrp_stat[SMRP_DELETE_IN]++;

    fwd = smrp_FindForward(res->GroupAddress);
    if (fwd) {    
	if (fwd->ParentNodeAddress != res->Source) {
	    SMRPGROUP_stat[SMRPGROUP_NOT_PRIMARY]++;
	    smrp_SetResponseState(res, MCRequestComplete);
	    smrp_DiscardResponse(res);
	    return;
	}
	smrp_DeleteForward(res->GroupAddress);
    }
    
    if (smrp_IsOurNodePortPrimary(port)) {
	grp = smrp_FindPortGroup(port,res->GroupAddress);
	if (grp == NULL) {
	    smrp_SendRejectResponse(res,MCGroupNotFound);
	    return;
	}

	if (grp->CreatorAddress != res->Source) {
	    smrp_SendRejectResponse(res,MCNotCreator);
	    return;
	}

	smrp_DeletePortGroup(port, res->GroupAddress);

	/* Clear out outstanding Creator Query */
	req = smrp_FindRequestByGroupAndType(res->GroupAddress,
					     MCCreatorRequest);
	if (req)
	    smrp_DiscardRequest(req);
	smrp_SendConfirmResponse(res);    
    } else {
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
        SMRPGROUP_stat[SMRPGROUP_NOT_PRIMARY]++;
    }
}

void
smrp_DoAddRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    SMRPGroup *group;
    char buf[30];
    UnicastAddr creator;
    
    smrp_stat[SMRP_ADDGRPENTRY_IN]++;

    /* We must be secondary */
    if (!smrp_IsOurNodePortSecondary(port)) {
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
	return;
    }

    /* Request must come from Primary node */
    if (res->Source != port->PrimaryNodeAddress) {
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
	return;
    }

    if (res->RequestSize != SMRP_UNICASTADDR_SIZE) {
	if (res->RequestSize < SMRP_UNICASTADDR_SIZE)
	    smrp_stat[SMRP_RUNT]++;
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
	return;
    }

    /* Get Creator Address from packet */
    creator = GETLONG(res->RequestData);
    
    group = smrp_AddPortGroup(port, res->GroupAddress, creator);

    if (group == NULL) {
	smrp_SendRejectResponse(res, MCTableFull);
	
	return;
    }
    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Group %s, added by %s to the group",
            smrp_DisplayGroupAddr(res->GroupAddress),
            PORTCB_PrintNetAddress(port, buf, res->Source));

    smrp_SendConfirmResponse(res);
}

void
smrp_DoRemoveRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    smrp_stat[SMRP_REMGRPENTRY_IN]++;
    
    SMRPASSERT(smrp_IsOurNodePortSecondary(port));

    if (smrp_DeletePortGroup(port, res->GroupAddress))
	smrp_SendConfirmResponse(res);
}

/*
 *------------------------------------------------------------------
 * smrp_DoLeaveRequestPacket		(SMRP Internal Function)
 *
 * Handle a Leave group request packet. 
 *------------------------------------------------------------------
 */
void
smrp_DoLeaveRequestPacket (SMRPPort *port, SMRPResponse* res)
{
    SMRPRoute* route;
    SMRPForward* fwd;

    smrp_stat[SMRP_LEAVE_IN]++;
    
    route = smrp_FindRouteByNet(smrp_GroupNetFromID(res->GroupAddress));
    if (route == NULL) {
	smrp_SendRejectResponse(res, MCGroupNotFound);
	return;
    }
    fwd = smrp_FindForward(res->GroupAddress);
    if (fwd != NULL) {
	if (fwd->State == MCForwarding || fwd->State == MCJoining) {
	    if (res->PortID != fwd->ParentPortID) {
		if (!smrp_FindForwardChildPort(fwd, res->PortID)) {
		    smrp_SendRejectResponse(res, MCPortNotFound);
		    return;
		}
	    } else if (!smrp_IsOurNodePortPrimary(port))
		return;

	    /* Spec says we should send a Member Query out this port to
	     * see if any other members are there.  We don't because
	     * the normal periodic query process will eventually find
	     * out.  In the meantime, just confirm the Leave Request.
	     */

	    smrp_SendConfirmResponse(res);
	} else
	    smrp_SendRejectResponse(res, MCBadState);
    } else {
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
    }
}

/*
 *------------------------------------------------------------------
 * smrp_DoJoinRequestPacket		(SMRP Internal Function)
 *
 * Handle a Join group request packet. 
 *------------------------------------------------------------------
 */
void
smrp_DoJoinRequestPacket (SMRPPort* port, SMRPResponse* res)
{
    SMRPRoute* route;
    SMRPForward* fwd;
    SMRPGroup *grp;
    UnicastAddr fwdaddr;
    SMRPPort *fwdport;
    char buf[30], buf2[30];
    SMRPRequest *req;
    
    SMRPASSERT(port != NULL);

    smrp_stat[SMRP_JOIN_IN]++;
    
    route = smrp_FindRouteByNet(smrp_GroupNetFromID(res->GroupAddress));
    if (route == NULL) {
	smrp_SendRejectResponse(res, MCGroupNotFound);
	return;
    }

    /* Discard request if we are not the Primary on the local connected net
       or we are not the port parent of the route */
    if ((route->Distance == 0 && res->PortID == route->ParentNodePortID &&
	 !smrp_IsOurNodePortPrimary(port))
	|| !smrp_RouteAreWePortParentNode(route, res->PortID)) {
        /* group debugging */
        if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Group %s, discard join request from %s, not"
                " responsible",
                smrp_DisplayGroupAddr(res->GroupAddress),
                PORTCB_PrintNetAddress(port, buf, res->Source));

        res->State = MCRequestComplete;
	smrp_DiscardResponse(res);
	return;
    }
    
    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Group %s, join requested from %s",
            smrp_DisplayGroupAddr(res->GroupAddress),
            PORTCB_PrintNetAddress(port, buf, res->Source));

    fwd = smrp_FindForward(res->GroupAddress);
    if (fwd != NULL) {

        /* group debugging */
        if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
            buginf("\nSMRP: Group %s, forward is found",
                smrp_DisplayGroupAddr(res->GroupAddress));

	if (fwd->State != MCForwarding && fwd->State != MCJoining) {
	    smrp_SendRejectResponse(res, MCBadState);
	    return;
	}
	if (res->PortID != fwd->ParentPortID) {
	    smrp_AddForwardChildPort(fwd, res->PortID);

	    /* If we're joining then don't send response until we move
	     * into forwarding.
	     */
	    if (fwd->State == MCJoining) {
                /* group debugging */
                if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
                    buginf("\nSMRP: Group %s, forward state is already"
			   " joining, ignored",
			   smrp_DisplayGroupAddr(res->GroupAddress));

		return;
	    }
	} else
	    fwd->ParentPortMembers = TRUE;

	/* Clear out old Member Queries on transaction queue */
	while ((req = smrp_FindRequestByGroupAndType(fwd->GroupAddress,
						     MCMemberRequest))
	       != NULL)
	    smrp_DiscardRequest(req);
	
	smrp_StartMemberTimer(fwd);
	smrp_SendConfirmResponse(res);
	return;
    }

    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Group %s, no forward found",
            smrp_DisplayGroupAddr(res->GroupAddress));

    if (route->Distance == 0) {
	if (res->PortID == route->ParentNodePortID) {
	    grp = smrp_FindPortGroup(port, res->GroupAddress);
	    if (grp != NULL) {
		fwd = smrp_CreateForward(grp->GroupAddress, grp->CreatorPort);
		if (fwd == NULL) {
		    smrp_SendRejectResponse(res, MCTableFull);
		    return;
		}
		fwd->ParentNodeAddress = grp->CreatorAddress;

		if (res->PortID != grp->CreatorPort)
		    smrp_AddForwardChildPort(fwd, res->PortID);
		else
		    fwd->ParentPortMembers = TRUE;
		
		/* Tell creator to start sending */
		smrp_SendJoinRequest(port, grp->GroupAddress,
				     grp->CreatorAddress);
	    } else
		smrp_SendRejectResponse(res,MCGroupNotFound);
	    return;
	} else {		/* Not on same network */
	    fwdport = smrp_FindPort(route->ParentNodePortID);

	    /* Make sure fwdport is operational */
	    if (!smrp_IsPortOperational(fwdport)) {
                /* group debugging */
                if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
                    buginf("\nSMRP: Group %s, join request discarded, fwd"
			   " parent port not operational",
			   smrp_DisplayGroupAddr(res->GroupAddress));
                res->State = MCRequestComplete;
		smrp_DiscardResponse(res);
		return;
	    }
		
	    if (smrp_IsOurNodePortPrimary(fwdport)) {
		/* Forward request to creator endpoint */
		grp = smrp_FindPortGroup(fwdport, res->GroupAddress);
		if (grp == NULL) {
		    smrp_SendRejectResponse(res, MCGroupNotFound);
		    return;
		}
		fwdaddr = grp->CreatorAddress;
	    } else
		/* Forward join request to primary */
		fwdaddr = fwdport->PrimaryNodeAddress;
	}
    } else {			/* Not local network */
	/* Forward join request to parent node */

	fwdport = smrp_FindPort(route->ParentNodePortID);
	
	fwdaddr = route->ParentNodeAddress;
    }    

    /* At this point we are either forwarding the request to the creator
     * endpoint or the next parent node.
     */

    fwd = smrp_CreateForward(res->GroupAddress, route->ParentNodePortID);
    if (fwd == NULL) {
	smrp_SendRejectResponse(res, MCListFull);
	return;
    }
    smrp_AddForwardChildPort(fwd, res->PortID);
    fwd->ParentNodeAddress = fwdaddr;
    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Group %s, created forward entry - parent %s,"
            " child %s",
            smrp_DisplayGroupAddr(res->GroupAddress),
            PORTCB_PrintNetAddress(port, buf, fwdaddr),
            PORTCB_PrintNetAddress(port, buf2, port->ThisNodeAddress));

    smrp_SendJoinRequest(fwdport, res->GroupAddress, fwdaddr);
}

void
smrp_DoMemberRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    SMRPForward *fwd;

    smrp_stat[SMRP_MEMBER_IN]++;
    
    fwd = smrp_FindForward(res->GroupAddress);

    if (fwd && fwd->ParentNodeAddress == res->Source)
	smrp_SendConfirmResponse(res);
    else {
	smrp_SetResponseState(res, MCRequestComplete);
	smrp_DiscardResponse(res);
    }
    
}

void
smrp_DoLocateRequestPacket (SMRPPort *port, SMRPResponse *res)
{
    smrp_stat[SMRP_LOCATE_IN]++;

    if (smrp_IsOurNodePortPrimary(port))
	smrp_SendConfirmResponse(res);
    else {
	smrp_SetResponseState(res,MCRequestComplete);
	smrp_DiscardResponse(res);
    }
}

/*******************************************************************
 * Transaction completion routines
 *******************************************************************/

static void
smrp_CompleteCreatorQueryRequest (SMRPRequest *req)
{
    SMRPPort *port;
    SMRPGroup *group;
    char buf[30];
    
    port = smrp_FindPort(req->PortID);

    if (!port)
	return;
    
    if (!smrp_ResponseIsConfirmation(req)) {
	if (req->ErrorIndication == MCNoResponse) {
            /* group debugging */
            if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
                buginf("\nSMRP: Group %s, creator no longer up on %s",
                    smrp_DisplayGroupAddr(req->GroupAddress),
                    PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

	    smrp_DeleteForward(req->GroupAddress);
	    smrp_DeletePortGroup(port, req->GroupAddress);
	}
    } else {
	group = smrp_FindPortGroup(port, req->GroupAddress);

	if (group)
	    mgd_timer_start(&group->CreatorTimer, smrp_creatorInterval);
    }
    
}

/*
 *------------------------------------------------------------------
 * smrp_CompleteMemberRequest		(SMRP Internal Function)
 *
 * Handle a Member response packet.  Simply discards confirmations
 * but handles trims the group distribution if "no response" reject
 * occurs.
 *------------------------------------------------------------------
 */
static void
smrp_CompleteMemberRequest (SMRPRequest* req)
{
    SMRPForward* fwd = smrp_FindForward(req->GroupAddress);
    SMRPChildPortLink *cp;
    SMRPPort* port;
    SMRPRoute* route;
    SMRPPortParentLink *ppl;
    char buf[30];
    boolean IllegalForwarder = FALSE;

    if (fwd == NULL)
	return;

    port = smrp_FindPort(req->PortID);
    if (!port)
        return;
    route = smrp_FindRouteByNet(smrp_GroupNetFromID(fwd->GroupAddress));
    if (!route)
	return;
    ppl = smrp_FindPortParentLink(route, req->PortID);
    if (!ppl)
	return;

    /*
     * If we are forwarding and we are a child port,
     * and our ppl is not pointing to our
     * own port address, we are proably in a duplicate path.
     * We have to prune this path off.
     */
     if ((ppl->PortParentNodeAddress != port->ThisNodeAddress) &&
	(req->PortID != fwd->ParentPortID)) {
	IllegalForwarder = TRUE;
	if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
	    buginf("\nSMRP: Group %s pruning duplicate path on %s",
		   smrp_DisplayGroupAddr(req->GroupAddress),
		   PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));
    }
    
    if ((smrp_ResponseIsConfirmation(req)) && (!IllegalForwarder)) {
	smrp_StartMemberTimer(fwd);
	return;
    }

    if ((req->ErrorIndication != MCNoResponse) && (!IllegalForwarder)) {
	return;
    }

    /* group debugging */
    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
        buginf("\nSMRP: Group %s, member no longer up on %s",
            smrp_DisplayGroupAddr(req->GroupAddress),
            PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    if (req->PortID == fwd->ParentPortID)
	fwd->ParentPortMembers = FALSE;
    else {
	cp = smrp_FindForwardChildPort(fwd, req->PortID);
	if (cp) 
	    smrp_DeleteForwardChildPort(fwd, req->PortID);
    }

    if (fwd->ChildPorts == NULL) {
        /* group debugging */
        if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK)) 
	    buginf("\nSMRP: Group %s, no more child ports in forward entry",
		   smrp_DisplayGroupAddr(req->GroupAddress));

	smrp_SetForwardState(fwd,MCLeaving);
	smrp_StopMemberTimer(fwd);
	port = smrp_FindPort(fwd->ParentPortID);
	SMRPASSERT(port != NULL);
	PORTCB_Unsubscribe(port,req->GroupAddress);
	smrp_SendLeaveRequest(port, req->GroupAddress, fwd->ParentNodeAddress);
    } else
	smrp_StartMemberTimer(fwd);
}

/*
 *------------------------------------------------------------------
 * smrp_DoJoinConfirmPacket		(Module Internal Function)
 *
 * Handle a Join group confirm packet. 
 *------------------------------------------------------------------
 */
static void
smrp_DoJoinConfirmPacket (SMRPRequest *req)
{
    SMRPPort* port;
    SMRPForward* fwd;

    fwd = smrp_FindForward(req->GroupAddress);
    if (fwd != NULL) {
	SMRPASSERT(fwd->State == MCJoining);
	smrp_StartMemberTimer(fwd);
	smrp_SetForwardState(fwd, MCForwarding);
	port = smrp_FindPort(fwd->ParentPortID);
	if (port)
	    PORTCB_Subscribe(port, req->GroupAddress);
	smrp_ForwardResponseToPendingJoins(req);
    } else 
	smrp_Error("Join confirm without forwarding entry");
}

/*
 *------------------------------------------------------------------
 * smrp_DoJoinRejectPacket		(Module Internal Function)
 *
 * Handle a Join group Reject packet. 
 *------------------------------------------------------------------
 */
static void
smrp_DoJoinRejectPacket (SMRPRequest *req)
{
    SMRPPort* port = smrp_FindPort(req->PortID);
    SMRPGroup* grp;

    SMRPASSERT(port != NULL);
    smrp_DeleteForward(req->GroupAddress);
    if (smrp_IsOurNodePortPrimary(port)) {
	grp = smrp_FindPortGroup(port, req->GroupAddress);
	if (grp != NULL)
	    smrp_SendCreatorQueryRequest(port,grp->CreatorAddress,
					 grp->GroupAddress);
    }
    smrp_ForwardResponseToPendingJoins(req);
}

/*
 *------------------------------------------------------------------
 * smrp_CompleteJoinRequest		(SMRP Internal Function)
 *
 * Handle a Join group response packet.  Simply dispatches to the
 * appropriate handler.  Called as the completion routine for join 
 * requests.
 *------------------------------------------------------------------
 */
static void
smrp_CompleteJoinRequest (SMRPRequest * req)
{
    if (smrp_ResponseIsConfirmation(req))
	smrp_DoJoinConfirmPacket(req);
    else
	smrp_DoJoinRejectPacket(req);
}

/*
 *------------------------------------------------------------------
 * smrp_CompleteLeaveRequest		(SMRP Internal Function)
 *
 * Handle a Leave group response packet. 
 *------------------------------------------------------------------
 */
static void
smrp_CompleteLeaveRequest (SMRPRequest* req)
{
    SMRPForward *fwd;

    fwd = smrp_FindForward(req->GroupAddress);
    
    if (!fwd || fwd->State != MCLeaving) {
	smrp_Error("\nSMRP: Non-existent or bad state for forward in "
	       "CompleteLeaveRequest");
	return;
    }
    
    smrp_DeleteForward(req->GroupAddress);
}

/*
 *------------------------------------------------------------------
 * smrp_SendAddGroupRequest
 *
 * Send an Add Group Entry request packet to the secondary node on 
 * this port.
 *------------------------------------------------------------------
 */
static void
smrp_SendAddGroupRequest (SMRPPort *port, UnicastAddr secondarynode,
			  SMRPGroupID gid, UnicastAddr creator)
{
    smrp_stat[SMRP_ADDGRPENTRY_OUT]++;
    smrp_CreateRequest(port, MCAddRequest, gid, secondarynode, &creator,
		       sizeof(UnicastAddr), 0, 0, NULL);
}

/*
 *------------------------------------------------------------------
 * smrp_SendPortDeleteGroupRequest
 * 
 * Send a Delete Group request packet
 *------------------------------------------------------------------
 */
static void
smrp_SendPortDeleteGroupRequest (SMRPPortID portid, SMRPGroupID gid)
{
    SMRPPort *port;
    
    smrp_stat[SMRP_DELETE_OUT]++;

    port = smrp_FindPort(portid);

    if (port)
	smrp_CreateRequest(port, MCDeleteRequest, gid, AllEndpointsAddr,
			   NULL, 0, 0, 0, NULL);
}

/*
 *------------------------------------------------------------------
 * smrp_SendRemoveGroupRequest
 * 
 * Send a Remove Group request packet to a secondary node.
 *------------------------------------------------------------------
 */


static void
smrp_SendRemoveGroupRequest (SMRPPort *port, SMRPGroupID gid)
{
    UnicastAddr secnode;

    smrp_stat[SMRP_REMGRPENTRY_OUT]++;

    secnode = smrp_GetPortSecondary(port);
    if (secnode)
	smrp_CreateRequest(port, MCRemoveRequest, gid, secnode, NULL, 0, 0, 0,
			   NULL);
}

/*
 *------------------------------------------------------------------
 * smrp_SendPortGroupsToSecondary
 *
 * Send Add Groups for all groups on this port to the secondary node
 *------------------------------------------------------------------
 */
void
smrp_SendPortGroupsToSecondary (SMRPPort *port, SMRPNode *node)
{
    SMRPGroup *grp = port->GroupTable;

    while (grp != NULL) {
	if (grp->CreatorPort == port->PortID)
	    smrp_SendAddGroupRequest(port, node->NodeAddress,
				     grp->GroupAddress, grp->CreatorAddress);
	grp = grp->Next;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_SendMemberRequestToPort
 *
 * Send member query
 *------------------------------------------------------------------
 */
static void
smrp_SendMemberRequestToPort (SMRPPort *port, SMRPGroupID gid)
{
    smrp_stat[SMRP_MEMBER_OUT]++;
    
    smrp_CreateRequest(port, MCMemberRequest, gid, AllEntitiesAddr, NULL, 0,
		       MemberRequestInterval, 0, smrp_CompleteMemberRequest);
}

/*
 *------------------------------------------------------------------
 * smrp_SendMemberRequestsForForward
 *
 * Send member query packets on all child ports of a forwarding
 * entry.
 *------------------------------------------------------------------
 */
static void
smrp_SendMemberRequestsForForward (SMRPForward *fwd)
{
    SMRPChildPortLink *portlink, *nextportlink;
    SMRPPort *port;
    
    portlink = fwd->ChildPorts;
    
    while (portlink != NULL) {
	nextportlink = portlink->Next;

	port = smrp_FindPort(portlink->ChildPortID);

	if (!port || !smrp_IsPortActive(port))
	    smrp_DeleteForwardChildPort(fwd, portlink->ChildPortID);
	else
	    smrp_SendMemberRequestToPort(port, fwd->GroupAddress);

	portlink = nextportlink;
    }
    if (fwd->ParentPortMembers == TRUE) {
	port = smrp_FindPort(fwd->ParentPortID);
	if (port)
	    smrp_SendMemberRequestToPort(port, fwd->GroupAddress);
    }
}

/*
 *------------------------------------------------------------------
 * smrp_SendJoinRequest			(SMRP Internal Function)
 *
 * Create a join request.
 *------------------------------------------------------------------
 */
static void
smrp_SendJoinRequest (SMRPPort* port, SMRPGroupID grp, UnicastAddr dest)
{
    smrp_stat[SMRP_JOIN_OUT]++;

    smrp_CreateRequest(port, MCJoinRequest, grp, dest, NULL, 0, 0, 0,
		       smrp_CompleteJoinRequest);
}

/*
 *------------------------------------------------------------------
 * smrp_SendCreatorQueryRequest
 * 
 * Send a Creator Query packet
 *------------------------------------------------------------------
 */
static void
smrp_SendCreatorQueryRequest (SMRPPort *port, UnicastAddr addr,
			      SMRPGroupID gid)
{
    smrp_stat[SMRP_CREATOR_OUT]++;
    
    smrp_CreateRequest(port, MCCreatorRequest, gid, addr, NULL, 0, 0, 0,
		       smrp_CompleteCreatorQueryRequest);
}

static void
smrp_SendLeaveRequest (SMRPPort *port, SMRPGroupID gid, UnicastAddr addr)
{
    smrp_stat[SMRP_LEAVE_OUT]++;

    smrp_CreateRequest(port, MCLeaveRequest, gid, addr, NULL, 0, 0, 0,
		       smrp_CompleteLeaveRequest);
}

/*
 *------------------------------------------------------------------
 * smrp_GroupDeleteForwardNotify	(Module Internal Function)
 *
 * Notify forwarding child nodes of a group delete
 * command.  Also notify any nodes that may be on parent port of
 * forward.
 *------------------------------------------------------------------
 */
static void
smrp_GroupDeleteForwardNotify (SMRPForward* fwd)
{
    SMRPChildPortLink* cp = fwd->ChildPorts;
        
    while (cp != NULL) {
	smrp_SendPortDeleteGroupRequest(cp->ChildPortID,fwd->GroupAddress);
	cp = cp->Next;
    }

    if (fwd->ParentPortMembers == TRUE)
	smrp_SendPortDeleteGroupRequest(fwd->ParentPortID, fwd->GroupAddress);
    
    smrp_SetForwardState(fwd, MCLeaving);
}

static void
smrp_ForwardResponseToPendingJoins (SMRPRequest *req)
{
    smrp_SendResponseByGroupAndType(req->GroupAddress, req->ResponseType,
				    req->ErrorIndication);
}


/*
 *------------------------------------------------------------------
 * smrp_ForAllForwardsDo		(SMRP Internal Function)
 *
 * Performs the treeProc for each group forwarding entry.
 *------------------------------------------------------------------
 */
void
smrp_ForAllForwardsDo (treeProc tp, void* data)
{
    RBTreeForEachNode(tp, data,smrp_GroupForwardingTable,FALSE);
}

static boolean
smrp_ReparentForward (treeLink *link, void *data)
{
    SMRPForward *fwd = (SMRPForward *)link;
    SMRP_fn_data *criteria = (SMRP_fn_data *) data;
    SMRPRoute *route;
    
    if (criteria->PortID && fwd->ParentPortID != criteria->PortID)
	return (TRUE);
    if (criteria->matchaddr && fwd->ParentNodeAddress != criteria->matchaddr)
	return (TRUE);
    if (criteria->localgroups) {
	route = smrp_FindRouteByNet(smrp_GroupNetFromID(fwd->GroupAddress));
	if (!route || route->Distance != 0 ||
	    route->ParentNodePortID != criteria->PortID)
	    return (TRUE);
    }
    fwd->ParentNodeAddress = criteria->setaddr;
    return (TRUE);
}

void
smrp_ReparentForwards (SMRPPortID pid, UnicastAddr matchaddr,
		       UnicastAddr setaddr, boolean onlylocalgroups)
{
    SMRP_fn_data data;
    
    data.PortID = pid;
    data.matchaddr = matchaddr;
    data.setaddr = setaddr;
    data.localgroups = onlylocalgroups;
    smrp_ForAllForwardsDo(smrp_ReparentForward, &data);
}

/*
 *------------------------------------------------------------------
 * smrp_HandleMemberTimer		(SMRP Internal Function)
 *
 * Called by background when the member timer expires.
 *------------------------------------------------------------------
 */
void
smrp_HandleMemberTimer (SMRPForward *fwd)
{
    smrp_SendMemberRequestsForForward(fwd);
}

/*
 *------------------------------------------------------------------
 * smrp_HandleCreatorTimer		(SMRP Internal Function)
 *
 * Called by background when the creator timer expires.
 *------------------------------------------------------------------
 */
void
smrp_HandleCreatorTimer (SMRPGroup *group)
{
    SMRPPort *port;
    
    port = smrp_FindPort(group->CreatorPort);
    if (!port || !smrp_IsOurNodePortPrimary(port))
	return;
    
    smrp_SendCreatorQueryRequest(port, group->CreatorAddress,
				 group->GroupAddress);
}

/*
 *------------------------------------------------------------------
 * smrp_PrintGroupKey			(SMRP Internal Routine)
 *
 * Utility routine for printing group tree keys.
 *------------------------------------------------------------------
 */
void
smrp_PrintGroupKey (char *text, treeKey key)
{
    sprintf(text, "#%d.%x", smrp_GroupNetFromID(key.l), 
	    smrp_GroupSocketFromID(key.l));
}

void
smrp_addresses (idbtype *idb)
{
    if (!smrp_running)
	return;
    
    reg_invoke_smrp_set_addresses(idb);
}

static boolean
smrp_ListenToForward (treeLink *link, void *data)
{
    SMRPForward *fwd = (SMRPForward *) link;
    SMRP_fn_data *criteria = (SMRP_fn_data *) data;
    uchar mcaddr[IEEEBYTES];
    hwidbtype *hwidb;
    idbtype *idb;
    SMRPPort *port;
    
    if (!fwd || fwd->State != MCForwarding)
	return TRUE;
    
    port = smrp_FindPort(criteria->PortID);
    if (!port) 
	return TRUE;

    idb = port->NetworkReference->phys_idb;

    if (!idb)
	return TRUE;
    
    hwidb = idb->hwptr;
    
    if (fwd->ParentPortID == criteria->PortID) {
	if ((criteria->mapper)(hwidb, fwd->GroupAddress, mcaddr)) {
	    reg_invoke_tbridge_smf_update(mcaddr,
	  			          idb,
				          SMF_MACTYP_AT_BCAST |
				          SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, mcaddr);
	    if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
		buginf("\nSMRP: Group %s, subscribed to forward port, %e", 
                    smrp_DisplayGroupAddr(fwd->GroupAddress), mcaddr);
	} 
    }
    
    return TRUE;
}

void
smrp_ListenToForwards (SMRPPort *port, GroupToMCfn mapper)

{
    SMRP_fn_data data;
    
    /* TESTMODE - subscribe to group multicast addr */
    if (smrp_test_running)
        reg_invoke_smrp_set_test_addresses(port, mapper);

    data.mapper = mapper;
    data.PortID = port->PortID;
    smrp_ForAllForwardsDo(smrp_ListenToForward, (void *) &data);
    
}    

static enum avl_compare
smrp_MCDatabaseCompare (avl_node_type *a, avl_node_type *b)
{
    SMRP_mc_node *arga, *argb;
    int result;

    arga = (SMRP_mc_node *)a;
    argb = (SMRP_mc_node *)b;
    
    result = ieee_compare(arga->mcaddr, argb->mcaddr);
    if (result > 0)
	return (AVL_GT);
    else if (result < 0)
	return (AVL_LT);
    else
	return (AVL_EQ);
}

static SMRP_mc_node *
smrp_InsertMCDatabase (SMRP_mc_node **db, SMRP_mc_node *node)
{
    boolean balancing;
    
    return ((SMRP_mc_node *) avl_insert((avl_node_type **) db,
					(avl_node_type *) node,
					&balancing,
					smrp_MCDatabaseCompare));
    
}

static SMRP_mc_node *
smrp_SearchMCDatabase (SMRP_mc_node *db, SMRP_mc_node *goal)
{
    return ((SMRP_mc_node *) avl_search((avl_node_type *)db,
					(avl_node_type *)goal,
					smrp_MCDatabaseCompare));
}

static SMRP_mc_node *
smrp_DeleteNodeFromDatabase (SMRP_mc_node **db, SMRP_mc_node *node)
{
    boolean balancing;
    
    return ((SMRP_mc_node *) avl_delete((avl_node_type **)db,
					(avl_node_type *)node,
					&balancing,
					smrp_MCDatabaseCompare));
}

void
smrp_AddMCToDatabase (SMRP_mc_node **db, uchar *mcaddr)
{
    SMRP_mc_node *node, *result;
    
    node = malloc(sizeof(SMRP_mc_node));
    if (!node) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return;
    }
    
    ieee_copy(mcaddr, node->mcaddr);

    result = smrp_SearchMCDatabase(*db, node);
    if (result) {
	result->uses++;
	free(node);
	return;
    }
    
    result = smrp_InsertMCDatabase(db, node);
    if (result) {
	result->uses++;
    } else
	free(node);
}

int
smrp_DeleteMCFromDatabase (SMRP_mc_node **db, uchar *mcaddr)
{
    SMRP_mc_node *node, goal;
    ieee_copy(mcaddr, goal.mcaddr);
    node = smrp_SearchMCDatabase(*db, &goal);
    if (!node)
	return -1;
    SMRPASSERT(node->uses != 0);
    if (--node->uses == 0) {
	smrp_DeleteNodeFromDatabase(db, node);
	free(node);
	return 0;
    } else
	return node->uses;
}

