/* $Id: smrp_util.c,v 3.2.62.2 1996/08/28 13:09:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_util.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_util.c,v $
 * Revision 3.2.62.2  1996/08/28  13:09:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/03/18  21:47:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:42:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:11:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:54:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:25:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/01  00:23:16  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.6  1995/07/25  21:29:40  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.5  1995/07/25  16:01:51  hampton
 * Temporary fix to smrp_Random.  [CSCdi37581]
 *
 * Revision 2.4  1995/07/24 22:30:56  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.3  1995/07/18 19:47:46  kleung
 * CSCdi37260:  The first MC group should be randomly selected.
 *
 * Revision 2.2  1995/06/14  22:55:21  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ctype.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "../util/random.h"

/*
 *------------------------------------------------------------------
 * smrp_util_Initialize		(Module Initialization)
 *
 * Perform initialization of module private data.
 *------------------------------------------------------------------
 */
void
smrp_util_Initialize (void)
{
}

boolean
smrp_LinkPortToIdb (SMRPPort *port, idbtype *idb)
{
    SMRPIdbsb *idbsb;
    swidb_sb_t sbtype;
    
    if (idb == NULL || port == NULL)
	return FALSE;

    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL) {
	idbsb = malloc(sizeof(SMRPIdbsb));
	if (idbsb == NULL)
	    return FALSE;
	sbtype = SWIDB_SB_SMRP;
	
	if (!idb_add_swsb(idb, &sbtype, idbsb)) {
	    free(idbsb);
	    return FALSE;
	}
	idbsb->phys_idb = idb;	/* Link sub-block to idb */
    }
	
    port->NetworkReference = idbsb;
    
    switch (port->NetworkType) {
    case LINK_APPLETALK:
	idbsb->atsmrp_port = port;
	break;
    default:
	return FALSE;
	break;
    }
    return TRUE;
}

SMRPPort*
smrp_FindPortFromIdb (SMRPIdbsb *idbsb, int nettype)
{
    
    if (idbsb == NULL)
	return NULL;

    switch (nettype) {
    case LINK_APPLETALK:
	return (idbsb->atsmrp_port);
    default:
	return NULL;
    }

}

ulong
smrp_MakePortID(SMRPNet const *net)
{
    return ((net->NetInterface->if_number << 8) | (0xff & net->NetType));
}

boolean
smrp_can_add_hwaddr(hwidbtype *hwidb)
{
    if (hwidb->type == IDBTYPE_LANCE)
	return FALSE;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_IsSpecialMulticastID		(SMRP Internal Function)
 *
 * return TRUE if the address is a special muticast ID.
 *------------------------------------------------------------------
 */
boolean
smrp_IsSpecialMulticastID (UnicastAddr a)
{
    switch (a) {
    case AllNodesAddr:
    case AllEndpointsAddr:
    case AllEntitiesAddr:
	return TRUE;
    }
    return FALSE;
}

boolean
smrp_IsSpecialGroupID (SMRPGroupID gid)
{
    switch (gid) {
    case AllNodesGroup:
    case AllEndpointsGroup:
    case AllEntitiesGroup:
	return TRUE;
    default:
	return FALSE;
    }
}

SMRPGroupID
smrp_MapSpecialAddrToGroupID (UnicastAddr a)
{
    switch (a) {
    case AllNodesAddr:
	return AllNodesGroup;
    case AllEndpointsAddr:
	return AllEndpointsGroup;
    case AllEntitiesAddr:
	return AllEntitiesGroup;
    default:
	return 0;
    }
}

void
smrp_SetCommonMulticasts (SMRPPort *port)
{
    idbtype *idb;
    hwidbtype *hwidb;
    
    if (!port || !(idb = port->NetworkReference->phys_idb))
	return;

    hwidb = idb->hwptr;
    
    if (smrp_can_add_hwaddr(hwidb)) {
	PORTCB_Subscribe(port, AllNodesAddr);
	PORTCB_Subscribe(port, AllEntitiesAddr);
    } else {
	(hwidb->reset)(hwidb);
    }
    
}
	
/*
 *------------------------------------------------------------------
 * smrp_Random				(SMRP Internal Function)
 *
 * return an ushort in the range of (0 <= n < range).  
 *
 * A range of zero indicates that any non-zero ushort number
 * is desired.  This is used to seed our sequence number.
 *------------------------------------------------------------------
 */
ushort
smrp_Random (int range)
{
    ushort val;

    val = random_gen();
    if (range == 0) {
	if (val == 0)
	    return 1;
    	return val;
    } else if (range < 0)
	return 0;

    return val % range;
}

/*
 *------------------------------------------------------------------
 * smrp_ForwardStateMessage		(SMRP Internal Function)
 *
 * Utility routine to convert group forward status to strings.

 *------------------------------------------------------------------
 */
const char*
smrp_ForwardStateMessage (ForwardState s)
{
    switch (s) {
    case MCJoining:
	return "joining";
    case MCForwarding:
	return "forwarding";
    case MCLeaving:
	return "leaving";
    default:
	return "invalid??";
    }
}
const char*
smrp_ForwardStateShortMessage (ForwardState s)
{
    switch (s) {
    case MCJoining:
	return "Join";
    case MCForwarding:
	return "Fwd";
    case MCLeaving:
	return "Leave";
    default:
	return "?";
    }
}

/*
 *------------------------------------------------------------------
 * smrp_StateMessage			(SMRP Internal Function)
 *
 * Utility routine to convert NodeState values used by ports
 * and nodes to strings.
 *------------------------------------------------------------------
 */
const char*
smrp_StateMessage (NodeState s)
{
    switch (s) {
    case MCPrimaryOperation:
	return "primary op";
    case MCSecondaryOperation:
	return "secondary op";
    case MCNotOperational:
	return "down";
    case MCNormalOperation:
	return "normal op";
    case MCPrimaryNegotiation:
	return "primary neg";
    case MCSecondaryNegotiation:
	return "secondary neg";
    case MCNotInitialized:
	return "uninitialized";
    default:
	return "invalid";
    }
}

const char*
smrp_StateShortMessage (NodeState s)
{
    switch (s) {
    case MCPrimaryOperation:
	return "(P)";
    case MCSecondaryOperation:
	return "(S)";
    case MCNotOperational:
    case MCNotInitialized:
	return "-D-";
    case MCNormalOperation:
	return "(N)";
    case MCPrimaryNegotiation:
	return "PN..";
    case MCSecondaryNegotiation:
	return "SN..";
    default:
	return "?";
    }
}


/*
 *------------------------------------------------------------------
 * smrp_ErrorMessage		(SMRP Internal Function)
 *
 * Utility routine to convert ErrorValues values to strings.
 *------------------------------------------------------------------
 */
const char*
smrp_ErrorMessage (ulong error)
{
    short code = error;

    switch (code) {
    case MCNoError:
	return "no error";
    case MCPacketLength:
	return "packet length error";
    case MCBadState:
	return "bad state";
    case MCTableFull:
	return "table full";
    case MCRangeFull:
	return "range full";
    case MCListFull:
	return "list full";
    case MCRouteNotFound:
	return "route not found";
    case MCPortNotFound:
	return "port not found";
    case MCGroupNotFound:
	return "group not found";
    case MCNotCreator:
	return "not creator";
    case MCNoResponse:
	return "no response";
    case MCNoTunnelNeeded:
	return "no tunnel needed";
    default:
	return "unknown";
    }
}


/*
 *------------------------------------------------------------------
 * smrp_RouteStateMessage		(SMRP Internal Function)
 *
 * Utility routine to convert route status to strings.
 *------------------------------------------------------------------
 */
const char*
smrp_TransStateMessage (RequestState s)
{
    switch (s) {
    case MCRequestPending:
	return "Pending";
    case MCRequestInProgress:
	return "In Progress";
    case MCRequestComplete:
	return "Complete";
    default:
	return "Unknown";
    }
}

/*
 *------------------------------------------------------------------
 * smrp_PrintCmdType			(SMRP Internal Function)
 *
 * Utility routine to convert TwoHighBits values to strings.
 *------------------------------------------------------------------
 */
const char*
smrp_PrintCmdType (uchar pktype)
{
    uchar type;

    type = pktype & 0xc0;
    switch (type) {
    case MCDatagram:
	return "";                 /* packet type already specifies data */
    case MCRequestCommand:
	return "request";
    case MCPositiveResponse:
	return "positive response";
    case MCNegativeResponse:
	return "negative response";
    default:
	return "invalid";
    }
}

/*
 *------------------------------------------------------------------
 * smrp_StateMessage			(SMRP Internal Function)
 *
 * Utility routine to convert SixLowBits values to strings.
 *------------------------------------------------------------------
 */
const char*
smrp_PrintPacketType (uchar pktype)
{
    uchar type;

    type = pktype & 0x3f;
    switch (type) {
    case MCData:
	return "data";
    case MCHello:
	return "hello";
    case MCPrimaryNode:
	return "primary node";
    case MCSecondaryNode:
	return "secondary node";
    case MCDistanceVector:
	return "distance vector";
    case MCCreateGroup:
	return "create group";
    case MCDeleteGroup:
	return "delete group";
    case MCJoinGroup:
	return "join group";
    case MCLeaveGroup:
	return "leave group";
    case MCGroupCreator:
	return "group creator";
    case MCGroupMember:
	return "group member";
    case MCAddEntry:
	return "add entry";
    case MCRemoveEntry:
	return "remove entry";
    case MCTunnelNode:
	return "tunnel node";
    case MCLocateNode:
	return "locate node";
    case MCNotifyNode:
	return "notify node";
    default:
	return "invalid";
    }
}

/*
 *------------------------------------------------------------------
 * smrp_IsValidNetRange			(SMRP Internal Function)
 *
 * Returns TRUE if both the starting and ending networks of the
 * range is valid.
 *------------------------------------------------------------------
 */
boolean
smrp_IsValidNetRange (const SMRPNetRange* nr)
{
    if (!nr)
        return FALSE;
    if ((nr->Start == 0) || (nr->End == 0))
	return FALSE;
    if ((nr->Start > MCMaxNetwork) || (nr->End > MCMaxNetwork))
	return FALSE;
    if (nr->Start > nr->End)
	return FALSE;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_PrintNetRange		(SMRP Internal Routine)
 *
 * Utility routine for printing network range.
 *------------------------------------------------------------------
 */
const char*
smrp_PrintNetRange (SMRPNetRange const *nr)
{
    static char text[14];

    if (!nr)
        return "Unknown";

    sprintf(text, "AT %d-%d", nr->Start, nr->End);
    return(text);
}

/*
 *------------------------------------------------------------------
 * smrp_PrintNetRangeKey		(SMRP Internal Routine)
 *
 * Utility routine for printing group tree keys.
 *------------------------------------------------------------------
 */
void
smrp_PrintNetRangeKey (char *text, treeKey key)
{
    const SMRPNetRange* nr = (SMRPNetRange*) key.p;

    sprintf(text, "%d-%d", nr->Start, nr->End);
}

/*
 *------------------------------------------------------------------
 * smrp_SetupHeader			(SMRP Internal Routine)
 *
 * Initializes an MCHeader data structure.
 *------------------------------------------------------------------
 */
void
smrp_SetupHeader (MCHeader* hdr, uchar pt, ushort seq, SMRPGroupID grp)
{
    uchar *ptr;
    
    SMRPASSERT(hdr != NULL);

    ptr = (uchar *) hdr;
    
    *ptr = SMRPVersion;
    *(ptr + SMRP_PAKTYPE_OFFSET) = pt;
    PUTSHORT(ptr + SMRP_SEQNUM_OFFSET, seq);
    PUTLONG(ptr + SMRP_GROUPADDR_OFFSET, grp);
}

/*
 *------------------------------------------------------------------
 * smrp_IsFirstPreferred		(SMRP Internal Function)
 *
 * Returns TRUE if the first node address is preferred (eg. lower) 
 * to that of the second node address.
 *------------------------------------------------------------------
 */
boolean
smrp_IsFirstPreferred (UnicastAddr a1, UnicastAddr a2)
{
    return (a1 < a2);
}

/*
 *------------------------------------------------------------------
 * smrp_PutThreeBytes			(SMRP Internal Routine)
 *
 * Write the lower three bytes of the value into the buffer.
 *------------------------------------------------------------------
 */
void
smrp_PutThreeBytes (uchar* p, ulong d)
{
    SMRPNetworkBytes nb;

    nb.data = d;
    *p++ = nb.bytes[1];    
    *p++ = nb.bytes[2];    
    *p++ = nb.bytes[3];
}

/*
 *------------------------------------------------------------------
 * smrp_GetThreeBytes			(SMRP Internal Function)
 *
 * Returns an unsigned long of the next three bytes right-justified.
 *------------------------------------------------------------------
 */
ulong 
smrp_GetThreeBytes (uchar* p)
{
    SMRPNetworkBytes nb;

    nb.data = 0;
    nb.bytes[1] = *p++;
    nb.bytes[2] = *p++;
    nb.bytes[3] = *p++;
    return nb.data;
}

/*
 * smrp_assert - Check condition and report failures
 */
void
smrp_assert (char *file, int line)
{
    char *msg = string_getnext();
    sprintf(msg, "file \"%s\", line %d", file, line);
    errmsg(&msgsym(ASSERTFAILED,SMRP), msg);
}

/*
 * smrp_Error
 *
 * Print out error message
 */
void
smrp_Error (char const *msg)
{
    buginf("\nSMRP: %s", msg);
}

/*
 * smrp_DumpBuffer
 *
 * Print out buffer in hex.
 */
uchar *
smrp_DumpBuffer (uchar* data, uint len, uchar* buf)
{
    int i = 0;

    if (len !=0 && data != NULL)
	while (i < 2*len) {
	    buf[i++] = tohexchar((*data & 0xf0) >> 4);
	    buf[i++] = tohexchar(*data);
	    data++;
	}
    buf[i] = '\0';

    return (buf);
}
 
