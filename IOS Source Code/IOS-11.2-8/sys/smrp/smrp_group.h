/* $Id: smrp_group.h,v 3.2 1995/11/17 18:53:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_group.h,v $
 *------------------------------------------------------------------
 * Support for SMRP Group and Forward entries.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_group.h,v $
 * Revision 3.2  1995/11/17  18:53:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:25:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/01  00:22:47  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/21  18:06:46  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.1  1995/06/07  22:45:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_GROUP_H__
#define __SMRP_GROUP_H__

#include "../util/tree.h"


/*
 *==================================================================
 * SMRP Group Specific Definitions
 *==================================================================
 */
typedef ulong SMRPNetwork;
typedef uchar SMRPSocket;

typedef enum {
    MCJoining = 1,		/* Group joining in progress */
    MCForwarding = 2,		/* Group data forwarding */
    MCLeaving = 3		/* Group leaving in progress */
} ForwardState;

enum {				/* Special group addresses */
    NullGroup = 0
};


/*
 *==================================================================
 * SMRP Group Specific Data Structures
 *==================================================================
 */

/*
 *------------------------------------------------------------------
 * SMRPChildPortLink - Many-To-Many link between groups and ports
 *------------------------------------------------------------------
 */
typedef struct SMRPChildPortLink_ {
    struct SMRPChildPortLink_* Next;

    SMRPPortID	ChildPortID;
    SMRPCache*  FastCache;

} SMRPChildPortLink;

/*
 *------------------------------------------------------------------
 * SMRPChildNodeLink - Many-To-Many link between nodes and forwards
 *------------------------------------------------------------------
 */
typedef struct SMRPChildNodeLink_ {
    struct SMRPChildNodeLink_* Next;

    SMRPPortID ChildNodePortID;
    UnicastAddr ChildNodeAddress;
} SMRPChildNodeLink;

/*
 *------------------------------------------------------------------
 * SMRPForward - SMRP Group Forwarding Table  	(Standard RB Tree)
 *
 * All fields are private to the module.
 * Data is keyed on GroupAddress.
 *------------------------------------------------------------------
 */
typedef struct SMRPForward_ {
    struct treeLink_	link;	/* Must be first */

    SMRPGroupID		GroupAddress;
    SMRPPortID		ParentPortID;
    UnicastAddr 	ParentNodeAddress;
    ForwardState	State;

    int			ChildPortCount;
    SMRPChildPortLink*	ChildPorts;
    boolean             ParentPortMembers; /* Members present on parent port */

    boolean             FastEnabled;       /* Enabled for f/s */
    boolean             CacheUpdated;      /* Cache was updated */
    boolean             CacheValid;        /* Cache is valid */
    sys_timestamp       CacheUpdateTime;   /* Last update time */

    sys_timestamp	UpdateTime;
    mgd_timer   	MemberTimer;
} SMRPForward;

/*
 *------------------------------------------------------------------
 * SMRPGroup - SMRP Group Creation Table       	(queue)
 *
 * All fields are private to the module.
 * Data is keyed on GroupAddress.
 *------------------------------------------------------------------
 */
typedef struct SMRPGroup_ {
    struct SMRPGroup_*	Next;

    SMRPGroupID		GroupAddress;
    UnicastAddr		CreatorAddress;
    SMRPPortID		CreatorPort;
/*
    ushort 		Version;	 Group table version (for secondary updates) */
    sys_timestamp	CreateTime;	/* When group was assigned. */
    mgd_timer   	CreatorTimer;	/* Timer for Group Creator checks */
} SMRPGroup;


/*
 *------------------------------------------------------------------
 * SMRP Group Module Traffic Definitions
 *------------------------------------------------------------------
 */
enum {
    SMRPGROUP_PORTGROUPS_ADDED,
    SMRPGROUP_PORTGROUPS_FREED,
    SMRPGROUP_NEW_FORWARDS,
    SMRPGROUP_RECYCLED_FORWARDS,
    SMRPGROUP_DELETED_FORWARDS,
    SMRPGROUP_CHILDPORTS_ADDED,
    SMRPGROUP_CHILDPORTS_FREED,
    SMRPGROUP_RANGE_FULL,
    SMRPGROUP_NOT_PRIMARY,
    SMRPGROUP_NO_ROUTE,
    SMRPGROUP_MAXTRAFFIC		/* must be last */
    };

extern rbTree* smrp_GroupForwardingTable;
extern ulong* SMRPGROUP_stat;


/*
 *==================================================================
 * SMRP Internal Entry Points defined by smrp_group.
 *==================================================================
 */
extern void smrp_group_Initialize(void);

extern SMRPGroup* smrp_FindPortGroup(struct SMRPPort_ *,SMRPGroupID);

extern void smrp_InitPortGroupData(struct SMRPPort_ *);
extern void smrp_DeleteAllPortGroups(struct SMRPPort_ *);

extern SMRPForward *smrp_GetFirstForward(void);
extern SMRPForward* smrp_FindForward(SMRPGroupID);
extern SMRPChildPortLink* smrp_FindForwardChildPort (SMRPForward*, SMRPPortID);

extern void smrp_HandleMemberTimer(SMRPForward *);
extern void smrp_HandleCreatorTimer(SMRPGroup *);

extern void smrp_DoJoinRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoDeleteRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoCreateRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoLeaveRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoMemberRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoRemoveRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoAddRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoLocateRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_ForAllForwardsDo(treeProc,void*);
extern void smrp_SendPortGroupsToSecondary(SMRPPort *, SMRPNode *);
extern void smrp_addresses(idbtype *idb);
extern void smrp_ListenToForwards(SMRPPort *, GroupToMCfn);
extern void smrp_ReparentForwards(SMRPPortID, UnicastAddr, UnicastAddr, boolean);
extern void smrp_AddMCToDatabase(SMRP_mc_node **, uchar *);
extern int smrp_DeleteMCFromDatabase(SMRP_mc_node **, uchar *);

extern const char *smrp_DisplayGroupAddr(SMRPGroupID);
extern void smrp_DeleteMatchingForwards(SMRPPortID, UnicastAddr, boolean, boolean);
extern void smrp_PrintGroupKey(char*, treeKey);

/*
 *------------------------------------------------------------------
 * smrp_GroupNetFromID			(Module Inline Function)
 *
 * Return a group network from a group ID.
 *------------------------------------------------------------------
 */
static inline SMRPNetwork 
smrp_GroupNetFromID (SMRPGroupID gid) 
{
    return (SMRPNetwork)(gid >> 8);
}

/*
 *------------------------------------------------------------------
 * smrp_GroupSocketFromID
 *
 * Return a group socket from a group ID.
 *------------------------------------------------------------------
 */
static inline SMRPSocket
smrp_GroupSocketFromID(SMRPGroupID grp)
{
    return (SMRPSocket) grp & 0xff;
}

/*
 *------------------------------------------------------------------
 * smrp_MakeGroupID			(Module Inline Function)
 *
 * Return a group ID given a group network and socket.
 *------------------------------------------------------------------
 */
static inline SMRPGroupID
smrp_MakeGroupID(SMRPNetwork net, SMRPSocket skt)
{
    return (SMRPGroupID) ((net << 8) | (skt & 0xff));
}

static inline SMRPForward *
smrp_GetNextForward (SMRPForward *fwd)
{
    if (fwd == NULL)
	return NULL;
    else
	return ((SMRPForward *) RBTreeNextNode(&fwd->link));
}

#endif __SMRP_GROUP_H__
