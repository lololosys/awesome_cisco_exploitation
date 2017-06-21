/* $Id: smrp_show.c,v 3.9.18.2 1996/09/11 20:09:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_show.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP show commands.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_show.c,v $
 * Revision 3.9.18.2  1996/09/11  20:09:40  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.9.18.1  1996/03/18  21:47:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:42:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:11:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/08  23:10:48  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.8  1996/02/07  16:15:19  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/06  18:41:14  kleung
 * CSCdi47296:  interface name get truncated in show smrp forward table
 *
 * Revision 3.6  1996/02/06  01:14:08  kleung
 * CSCdi47296:  interface name get truncated in show smrp forward table
 *
 * Revision 3.5  1996/02/01  06:09:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:46  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/17  18:54:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:25:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/11  19:02:37  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.13  1995/09/04  09:53:21  kleung
 * CSCdi39771:  Show smrp forward crash router.
 *
 * Revision 2.12  1995/08/30  22:43:42  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.11  1995/08/22  00:53:25  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.10  1995/08/09  03:41:57  kleung
 * CSCdi38461:  Need to support serial interface for SMRP.
 *
 * Revision 2.9  1995/08/03  21:17:14  kleung
 * CSCdi38206:  SMRP member requests expires after poll timer.
 *
 * Revision 2.8  1995/08/01  00:23:07  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.7  1995/07/25  21:29:32  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.6  1995/07/24  22:30:55  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.5  1995/07/06 03:06:52  kleung
 * CSCdi36742:  Spurious memory access when SMRP port is deconfigured.
 *
 * Revision 2.4  1995/07/05  21:39:21  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.3  1995/06/21  18:06:54  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.2  1995/06/14  22:55:16  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <stdarg.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "plural.h"
#include "mgd_timers.h"
#include "../util/tree.h"
#include "../atalk/parser_defs_atalk.h"
#include "../smrp/parser_defs_smrp.h"
#include "../smrp/smrp_private.h"
#include "../smrp/smrp_net.h"
#include "../smrp/smrp_globals.h"
#include "../smrp/smrp_group.h"
#include "../smrp/smrp_node.h"
#include "../smrp/smrp_port.h"
#include "../smrp/smrp_registry.h"
#include "../smrp/smrp_route.h"
#include "../smrp/smrp_trans.h"
#include "../smrp/smrp_util.h"

/*
 *------------------------------------------------------------------
 * smrp_DisplayTime		(SMRP Internal Function)
 *
 * Print the time value of 'ticks' as "never", "h:mm:ss", "dd:hh", or
 * "XwYd".
 *------------------------------------------------------------------
 */
static char*
smrp_DisplayTime (ulong ticks)
{
    static char str[40];
    
    (void) sprint_ticks_in_dhms(str, ticks);
    return(str);

}

/*
 *------------------------------------------------------------------
 * smrp_DisplayNetworkType		(SMRP Internal Function)
 *
 * Utility routine to display network type.
 *------------------------------------------------------------------
 */
static const char*
smrp_DisplayNetworkType(int type)
{
    /* Only AppleTalk support at this time */
    switch (type) {
    case LINK_APPLETALK:
        return "AT";
    default:
        return "?";
    }
}

/*
 * smrp_show_globals
 *
 * Show SMRP globals. 
 */
static void
smrp_show_globals (void)
{
    if (!smrp_running) {
        printf("\n  SMRP is not running");
        return;
    }

    printf("SMRP global information:");

    /* SMRP operation mode */
    printf("\n  SMRP is running.");
    if(smrp_test_running) 
        printf(" (test mode)");


    printf("\n\n  Group Info:");
    printf("\n    Creators are polled every %d seconds.",
        CreatorInterval/ONESEC);
    printf("\n    Members are polled every %d seconds.",
        MemberInterval/ONESEC);

    printf("\n\n  Neighbors Info:");
    printf("\n    Hellos are sent every %d seconds.",
        HelloInterval/ONESEC);
    printf("\n    Neighbors are down after not being heard for %d seconds.",
        NodeAgeInterval/ONESEC);
    printf("\n\n  Route Info:");
    printf("\n    Poisoned routes purged after %d seconds.",
        PurgeRouteInterval/ONESEC);
    printf("\n    Holddown period of %d seconds on receiving poisons.",
	   HolddownInterval/ONESEC);

    printf("\n\n  Transaction Info:");
    printf("\n    Primary requests retransmitted after %d second%s.",
        ArgAndPlural((PrimaryRequestInterval/ONESEC),"","s"));
    printf("\n    Secondary requests retransmitted after %d second%s.",
        ArgAndPlural((SecondaryRequestInterval/ONESEC),"","s"));
    printf("\n    Member requests retransmitted after %d second%s.",
        ArgAndPlural((MemberRequestInterval/ONESEC),"","s"));
    printf("\n    Default request retransmission is %d seconds.",
        RequestInterval/ONESEC);
    printf("\n    Default maximum number of request retries is %d times.",
        MaxRetries);
    printf("\n    Response transactions are discarded after %d seconds.",
        ResponseInterval/ONESEC);
    
}

/*
 * smrp_DisplayRequest
 *
 * Show SMRP pending requests.
 */
static void
smrp_DisplayRequest(SMRPRequest* req)
{
    SMRPPort* port;
    uchar cmd;
    uchar pktype;
    char buf[30];
    char data[100];
    int maxlen = 40;
    
    if (!req)
	return;
    
    port = smrp_FindPort(req->PortID);
    if (port == NULL)
	return;

    cmd = smrp_GetPacketModifiers(req->PacketType);
    pktype = smrp_GetPacketType(req->PacketType);

    printf("    Seq: %d\n", req->SequenceNumber);
    printf("\tGroup: %s,", smrp_DisplayGroupAddr(req->GroupAddress));
    printf("\tDest: %s\n", PORTCB_PrintNetAddress(port, buf, req->Destination));
    printf("\tPacket Type: %s %s\n", smrp_PrintPacketType(pktype),
        smrp_PrintCmdType(cmd));
    printf("\tState: %s,", smrp_TransStateMessage(req->State));
    printf("\tAttempts: %d\n", req->Attempts);
    printf("\tInterface: %s,",
	   idb_get_namestring(port->NetworkReference->phys_idb));
    printf("\tSrc: %s\n", PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    smrp_DumpBuffer((uchar *)req->PacketData, min(maxlen,req->DataSize), data);

    printf("\tData: %s%s\n", data, (strlen(data)==2*req->DataSize)?"":"...");
    printf("\tData Size: %d", req->DataSize);
    printf("\tData Allocated: %s\n", req->DataAllocated?"True":"False");
}

/*
 * smrp_show_pvt_req
 *
 * Show SMRP pending requests.
 */
static void
smrp_show_pvt_req (void)
{
    SMRPRequest* req = smrp_FirstRequest();

    printf("SMRP Requests:\n");
    while (req != NULL) {
	smrp_DisplayRequest(req);
	req = req->next;
    }
}

/*
 * smrp_DisplayResponse
 *
 * Show SMRP pending responses.
 */
static void
smrp_DisplayResponse(SMRPResponse* res)
{
    SMRPPort* port;
    uchar cmd;
    uchar pktype;
    char buf[30];
    char data[100];
    int maxlen = 40;
    
    if (!res) 
	return;
    
    port = smrp_FindPort(res->PortID);
    if (port == NULL)
	return;

    cmd = smrp_GetPacketModifiers(res->ResponseType);
    pktype = smrp_GetPacketType(res->ResponseType);

    printf("    Seq: %d\n", res->SequenceNumber);
    printf("\tGroup: %s,", smrp_DisplayGroupAddr(res->GroupAddress));
    printf("\tSource: %s\n", PORTCB_PrintNetAddress(port, buf, res->Source));
    printf("\tPacket Type: %s %s\n", smrp_PrintPacketType(pktype),
        smrp_PrintCmdType(cmd));
    printf("\tState: %s,", smrp_TransStateMessage(res->State));
    printf("\tError Code: %s\n", smrp_ErrorMessage(res->ErrorIndication));
    printf("\tInterface: %s,",
	   idb_get_namestring(port->NetworkReference->phys_idb));
    printf("\tSrc: %s\n", PORTCB_PrintNetAddress(port, buf, port->ThisNodeAddress));

    smrp_DumpBuffer(res->ResponseBuffer, min(maxlen,res->ResponseSize), data);

    printf("\tData: %s%s\n", data, (strlen(data)==2*res->ResponseSize)?"":"...");
    printf("\tData Size: %d\n", res->ResponseSize);

}

/*
 * smrp_show_pvt_resp
 *
 * Show SMRP pending responses.
 */
static void
smrp_show_pvt_resp (void)
{
    SMRPResponse* res = smrp_FirstResponse();

    printf("SMRP Responses:\n");
    while (res != NULL) {
	smrp_DisplayResponse(res);
	res = res->next;
    }
}

/*
 *------------------------------------------------------------------
 * smrp_ShowIFPort			(Module TreeProc Routine)
 *
 * Show protocol for any ports associated with this interface.  
 *------------------------------------------------------------------
 */
static boolean
smrp_ShowIFPort (treeLink* link, void* d)
{
    SMRPPort* port = (SMRPPort*) link;
    idbtype* idb = (idbtype*) d;

    if (port->NetworkReference->phys_idb == idb) {
	PORTCB_ShowPortProtocol(port);
    }
    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrp_ShowIFPorts			(SMRP Internal Routine)
 *
 * Show protocol for any ports which are associated with this 
 * interface.  This routine is called when an interface 
 * SHOW PROTOCOL command is processed.
 *------------------------------------------------------------------
 */
static void
smrp_ShowIFPorts (idbtype* idb)
{
    RBTreeForEachNode(smrp_ShowIFPort,(void*)idb,smrp_PortTable,FALSE);
}

/*
 * smrp_ShowProto
 *
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */
void 
smrp_ShowProto (idbtype *idb)
{
    if (idb == NULL) {
	if (smrp_running)
	    printf("\n  SMRP protocol is enabled");
	return;
    }
    smrp_ShowIFPorts(idb);
}

/*
 * smrp_DisplayPort
 *
 * Display port info.
 */
static boolean
smrp_DisplayPort (treeLink* link, void* idb)
{
    SMRPPort *port = (SMRPPort *) link;
    SMRPGroup *grp;
    char this[14], primary[14], secondary[14];
    char net[14];

    if (!port)
        return(TRUE);

    sprintf(net, "%d-%d", port->RangeStart, port->RangeEnd);

    printf("\n%18s %5s %14s %4s %9s %9s %9s",
	idb_get_namestring(port->NetworkReference->phys_idb), 
	smrp_StateShortMessage(port->State),
	net,
	smrp_DisplayNetworkType(port->NetworkType),
        PORTCB_PrintNetAddress(port, this, port->ThisNodeAddress),
        PORTCB_PrintNetAddress(port, primary, port->PrimaryNodeAddress),
        PORTCB_PrintNetAddress(port, secondary, port->SecondaryNodeAddress));
    
    /* More info if port is specified */
    if (idb) {
        grp = port->GroupTable;
        printf("\n\nCurrent groups:");
        while (grp) {
            printf(" %s",
                smrp_DisplayGroupAddr(grp->GroupAddress), grp->Next?",":"");
            grp = grp->Next;
	}
        printf("\nLast group: %s", 
            smrp_DisplayGroupAddr(port->LastGroupID));
        printf("\nSwitching path: %s", port->FastOkay?"Route cache":"Processor");
	if (!port->FastOkay)
	    printf(" (%s)", smrpfast_Reason(port));
	    
    }    
    return(TRUE);
}

/*
 * smrp_DisplayProtoPort
 *
 * Display port info for the specified protocol.
 */
static boolean
smrp_DisplayProtoPort (treeLink* link, void* proto)
{
    SMRPPort* port = (SMRPPort*) link;
 
    if (!port)
        return(TRUE);

    if (port->NetworkType == (int)proto) 
        smrp_DisplayPort((treeLink*)port, NULL);

    return(TRUE);
}

static const uchar smrp_port_banner[] = 
    {"Interface          State Network        Type Address   Primary   Secondary\n"
     "--------------------------------------------------------------------------\n"};
/*
 * smrp_show_port
 *
 * Show SMRP port table.
 */
static void
smrp_show_port (parseinfo *csb)
{
    idbtype* idb;
    SMRPIdbsb *idbsb;
    SMRPPort* port;
    int ntype;

    /* Display ports for specified interface */
    if (GETOBJ(int,2)) {
        idb = GETOBJ(idb,1);

	idbsb = smrp_GetIdbsb(idb);

	if (idbsb == NULL)
	    return;
	
        port = idbsb->atsmrp_port;

        automore_enable(smrp_port_banner);
        if (port)
            smrp_DisplayPort((treeLink*) port, idb);
        else
            printf("No such port\n");
        automore_disable();
        return;
    }

    printf("\nSMRP Port Table");
    automore_enable(smrp_port_banner);

    ntype = GETOBJ(int,1);
    if (ntype) {
        /* All ports for specified network protocol */
        smrp_ForAllPortsDo(smrp_DisplayProtoPort, (void*)ntype);
    } else {
        /* Display all ports */
        smrp_ForAllPortsDo(smrp_DisplayPort, NULL);
    }
    automore_disable();
}


/*
 * smrp_DisplayNeighbor
 *
 * Display neighbor info.
 */
static boolean
smrp_DisplayNeighbor (treeLink* link, void* nbr)
{
    char buf[30],buf1[10];
    SMRPNode* neighbor = (SMRPNode*) link;
    SMRPIdbsb * idbsb;
    SMRPPort* port;
    char *time = NULL;
    ulong highlong, lowlong;
     
    if (!neighbor)
        return(TRUE);

    idbsb = smrp_GetPortIDNetInterface(neighbor->PortID);

    if (!idbsb)
        return(TRUE);

    port = idbsb->atsmrp_port;

    if (!port)
	return TRUE;
    if (ELAPSED_TIME(neighbor->HelloTime)/ONESEC < 60) {
        sprintf(buf1, "%d", ELAPSED_TIME(neighbor->HelloTime)/ONESEC);
        time = buf1;
    } else
        time = smrp_DisplayTime(ELAPSED_TIME(neighbor->HelloTime));
        
    printf("\n%9s %5s %18s %8s",
        PORTCB_PrintNetAddress(port, buf, neighbor->NodeAddress),
        smrp_StateShortMessage(neighbor->State),
        idb_get_namestring(idbsb->phys_idb),
        time);

    if (nbr) {
        /* additional info for this neighbor */
        highlong = neighbor->RouteVersion >> 32;
        lowlong = neighbor->RouteVersion & 0x0000ffff;
        printf("\n\nRoute version: 0x%04x%04x", highlong, lowlong);
        printf("\nRoute needed: %s", neighbor->NeedsRoutes?"True":"False");
    }

    return(TRUE);
}

/*
 * smrp_DisplayProtoNeighbor
 *
 * Display neighbor info for the specified protocol.
 */
static boolean
smrp_DisplayProtoNeighbor (treeLink* link, void* proto)
{
    SMRPNode* neighbor = (SMRPNode*) link;
    SMRPPort* port;
 
    if (!neighbor)
        return(TRUE);

    port = smrp_FindPort(neighbor->PortID);

    if (!port)
	return TRUE;

    if (port->NetworkType == (int)proto) 
        smrp_DisplayNeighbor((treeLink*)neighbor, NULL);

    return(TRUE);
}

static const uchar smrp_nbr_banner[] = 
    {"Neighbor  State Interface          Last \n"
     "                                   Heard\n"
     "----------------------------------------\n"};

/*
 * smrp_show_neighbors
 *
 * Show either a detailed dump of neighbor information (net != 0) or all
 * known neighbors (net == 0).
 */
static void
smrp_show_neighbors (parseinfo *csb)
{
    SMRPNode* node;
    UnicastAddr naddr;    
    int ntype;

    /* Specific neighbor */
    if (GETOBJ(int,2)) {
        
        reg_invoke_smrp_parse_netaddr(GETOBJ(int,1), GETOBJ(paddr,1),
            &naddr);
        node = smrp_FindAdjacentNode(naddr);

        automore_enable(smrp_nbr_banner);
        if (node)
            smrp_DisplayNeighbor((treeLink*)node, node);
        else
            printf("No such neighbor\n");
        automore_disable();
        return;
    }
   
    printf("\nSMRP Neighbor Table");
    automore_enable(smrp_nbr_banner);
 
    ntype = GETOBJ(int,1);
    if (ntype) {
        /* All neighbors for specified network protocol */
        smrp_ForAllNodesDo(smrp_DisplayProtoNeighbor, (void*)ntype);
    } else {
        /* All neighbors */
        smrp_ForAllNodesDo(smrp_DisplayNeighbor, NULL);
    }
    automore_disable();

}

/*
 * smrp_DisplayRoute
 *
 * Display route info.  The first line contains all info in the header.  
 * If the address is not the same as this router's, then it will be displayed.
 * For each interface, there will be an address that points to where
 * the route goes through.  If this does not point to this router, it
 * will be displayed.
 * 
 */
static boolean
smrp_DisplayRoute (treeLink* link, void* p)
{
    char portaddr[10];
    char shortname[20];
    boolean first_time = TRUE;
    SMRPRoute* rte = (SMRPRoute*) link;
    SMRPRoute* route = (SMRPRoute*) p;
    SMRPIdbsb * idbsb;
    SMRPPort* routeparentport;
    SMRPPort* port;
    SMRPPortParentLink* portlink;
    char *displaybuf;
    uint DisplayBufSize;
    ulong highlong, lowlong;
   
    
    if (!rte)
        return(TRUE);

    idbsb = smrp_GetPortIDNetInterface(rte->ParentNodePortID);
    routeparentport = smrp_FindPort(rte->ParentNodePortID);
    portlink = rte->PortParents;

    displaybuf = malloc(WIDTH_DEFAULT);
    if (displaybuf == NULL)
	return (TRUE);
    DisplayBufSize = WIDTH_DEFAULT;
    
    safe_sprintf(&displaybuf, &DisplayBufSize,
		 "\n%14s %3d %6d %18s ", 
		 smrp_PrintNetRange(&rte->Range), 
		 rte->Distance,
		 rte->TunnelDistance,
		 idb_get_namestring(idbsb?idbsb->phys_idb:NULL));

    /* display ParentNodeAddress only if not this router's interface */
    if (routeparentport &&
        routeparentport->ThisNodeAddress != rte->ParentNodeAddress) {
        safe_sprintf(&displaybuf, &DisplayBufSize,
		     "%9s",
		     PORTCB_PrintNetAddress(routeparentport, portaddr, 
					    rte->ParentNodeAddress));
        first_time = FALSE;
    }

    /* additional addresses that's not this router's interface */
    while (portlink) {
        port = smrp_FindPort(portlink->PortID);
        if (port && port->ThisNodeAddress != portlink->PortParentNodeAddress
            && port->ThisNodeAddress != 0) {

            idbsb = smrp_GetPortIDNetInterface(portlink->PortID);
            if (idbsb)
                sprintf(shortname, "(%s)", idbsb->phys_idb->short_namestring);
            else
                sprintf(shortname, "(UNKNOWN)");

            if (port != routeparentport) {
                safe_sprintf(&displaybuf, &DisplayBufSize,
			     "\n                          %18s %9s",
			     shortname,
			     PORTCB_PrintNetAddress(port, portaddr,
						    portlink->PortParentNodeAddress));
            } else if (rte->ParentNodeAddress != 
                portlink->PortParentNodeAddress ) {

                /* this shouldn't happen */
                safe_sprintf(&displaybuf, &DisplayBufSize,
			     "\n                          %18s ERROR: %9s", 
			     shortname,
			     PORTCB_PrintNetAddress(port, portaddr, 
						    portlink->PortParentNodeAddress));
	    }
        }
        portlink = portlink->Next;	
    }

    if (route) {
        /* additional info for this route */
        highlong = route->RouteVersion >> 32;
        lowlong = route->RouteVersion & 0x0000ffff;
        safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\n\nRoute version: 0x%04x%04x", highlong, lowlong);
	safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\nElapsed update time: %s",
		     smrp_DisplayTime(ELAPSED_TIME(rte->UpdateTime)));
    }

    printf("%s", displaybuf);

    free(displaybuf);
    
    return(TRUE);        
}

static const uchar smrp_rte_banner[] = 
    {"Network        Hop Tunnel Interface          Address\n"
     "                                                    \n"
     "----------------------------------------------------\n"};
/*
 * smrp_show_route
 *
 * Show SMRP route table
 */
static void
smrp_show_route (parseinfo *csb)
{
    SMRPRoute* route, *nextroute;
    SMRPNetRange range;
    idbtype *idb = NULL;
    int ntype = 0;
    SMRPPort *port;
    
    range.Start = range.End = GETOBJ(int,2);

    switch (GETOBJ(int,3)) {
    case 0:
	/* Display routes for a particular protocol.  All routes if
	 * ntype = 0.
	 */
	ntype = GETOBJ(int, 1);
	break;
    case 1:
        /* Display a specific route */
        automore_enable(smrp_rte_banner);
        route = smrp_FindRoute(&range);
        if (route)
            smrp_DisplayRoute((treeLink*)route, route);
        else
            printf("No such route, %d\n", range.Start);
        automore_disable();
        return;
    case 2:
	/* Display routes learned through a particular interface */
	idb = GETOBJ(idb,1);
	break;
    }

    printf("\nSMRP Route Table");

    if (!ntype)
        printf("\n\n%d route%s in internet\n", 
            ArgAndPlural(smrp_RouteCount(),"","s"));

    automore_enable(smrp_rte_banner);
    route = smrp_GetFirstRoute();
    while (route != NULL) {
	nextroute = smrp_GetNextRoute(route);

	if (idb) {
	    port = smrp_FindPort(route->ParentNodePortID);
	    if (!port || port->NetworkReference->phys_idb != idb) {
		route = nextroute;
		continue;
	    }
	}
	    
	if (ntype) {
	    port = smrp_FindPort(route->ParentNodePortID);
	    if (!port || port->NetworkType != ntype) {
		route = nextroute;
		continue;
	    }
	}
	
	if (nextroute)
	    range = nextroute->Range; /* Save next route's range */
	smrp_DisplayRoute((treeLink *)route, NULL);
	if (nextroute) {
	    route = smrp_FindRoute(&range);
	    if (route == NULL) {
		printf("\nSMRP: Can't find next route.  "
		       "Reissue show command.");
		break;
	    }
	} else
	    break;
    }	    
    automore_disable();
}

/*
 * smrp_DisplayForward
 *
 * Display forwarding info.
 */
static boolean
smrp_DisplayForward (treeLink* link, void* group)
{
    char buf[30];
    boolean first_time = TRUE;
    SMRPForward* fwd = (SMRPForward*) link;
    SMRPIdbsb* idbsb;
    SMRPPort* port;
    SMRPChildPortLink* cport;
    char *displaybuf;
    uint DisplayBufSize;
    
    if (fwd == NULL)
        return(TRUE);

    idbsb = smrp_GetPortIDNetInterface(fwd->ParentPortID);
    port = smrp_FindPort(fwd->ParentPortID);

    if (idbsb == NULL || port == NULL)
	return(TRUE);

    displaybuf = malloc(WIDTH_DEFAULT);
    if (displaybuf == NULL)
	return (TRUE);
    DisplayBufSize = WIDTH_DEFAULT;

    /* Print parent info */
    safe_sprintf(&displaybuf, &DisplayBufSize,
		 "\n%12s %5s  %18s %9s ",
		 smrp_DisplayGroupAddr(fwd->GroupAddress),
		 smrp_ForwardStateShortMessage(fwd->State),
		 idb_get_namestring(idbsb->phys_idb),
		 PORTCB_PrintNetAddress(port, buf, fwd->ParentNodeAddress));

    for (cport = fwd->ChildPorts; cport; cport = cport->Next) {
        port = smrp_FindPort(cport->ChildPortID);
        idbsb = smrp_GetPortIDNetInterface(cport->ChildPortID);
	if (port == NULL || idbsb == NULL) 
	    continue;
        if (first_time) {
            first_time = FALSE;
            safe_sprintf(&displaybuf, &DisplayBufSize,
			 "%18s %9s",
			 idb_get_namestring(idbsb->phys_idb), 
			 PORTCB_PrintNetAddress(port, buf,
						port->ThisNodeAddress));
        } else {
            safe_sprintf(&displaybuf, &DisplayBufSize,
			 "\n                                                 %18s %9s",
			 idb_get_namestring(idbsb->phys_idb), 
			 PORTCB_PrintNetAddress(port, buf,
						port->ThisNodeAddress));
        }
    }

    /* More info if group is specified */
    if (group) {
        safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\n\nChild count: %d", fwd->ChildPortCount);
        safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\nElapsed update time: %s", 
		     smrp_DisplayTime(ELAPSED_TIME(fwd->UpdateTime)));
        safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\nNext poll time (sec): %d", 
		     mgd_timer_left_sleeping(&fwd->MemberTimer)/ONESEC);
    }

    printf("%s", displaybuf);
    free(displaybuf);
    return(TRUE);
}

static const uchar smrp_fwd_banner[] = 
    {"Group        State             Parent                       Child\n"
     "Address             Interface          Address   Interface          Address\n"
     "---------------------------------------------------------------------------\n"};
/*
 * smrp_show_forward
 *
 * Show SMRP forward table
 */
static void
smrp_show_forward (parseinfo *csb)
{
    SMRPForward* fwd, *nextfwd;
    SMRPGroupID group;
    int ntype;
    SMRPPort *port;
    
    /* Display specified forwarding entry */
    if (GETOBJ(int,4)) {
        group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
        fwd = smrp_FindForward(group);
        automore_enable(smrp_fwd_banner);
        if (fwd)
            smrp_DisplayForward((treeLink*) fwd, &group);
        else
            printf("No such group in forwarding table\n");
        automore_disable();
        return;
    }

    printf("\nSMRP Forwarding Table");
    automore_enable(smrp_fwd_banner);

    ntype = GETOBJ(int,1);

    fwd = smrp_GetFirstForward();
    while (fwd != NULL) {
	nextfwd = smrp_GetNextForward(fwd);

	if (ntype) {
	    port = smrp_FindPort(fwd->ParentPortID);
	    if (!port || port->NetworkType != ntype) {
		fwd = nextfwd;
		continue;
	    }
	}

	if (nextfwd)
	    group = nextfwd->GroupAddress;
	smrp_DisplayForward((treeLink *) fwd, NULL);
	if (nextfwd) {
	    fwd = smrp_FindForward(group);
	    if (fwd == NULL) {
		printf("\nSMRP: Can't find next forward.  "
		       "Reissue show command.");
		break;
	    }
	} else
	    break;
    }
    automore_disable();

}

/*
 * smrp_DisplayMCache
 *
 * Display multicast cache info.
 */
static boolean
smrp_DisplayMCache (treeLink* link, void* group)
{
    boolean first_time = TRUE;
    SMRPForward* fwd = (SMRPForward*) link;
    SMRPIdbsb* idbsb;
    SMRPPort* port;
    SMRPChildPortLink* cport;
    char *displaybuf;
    uint DisplayBufSize;
    char buf1[100], buf2[100];
    
    if (fwd == NULL || !fwd->FastEnabled)
        return(TRUE);

    idbsb = smrp_GetPortIDNetInterface(fwd->ParentPortID);
    port = smrp_FindPort(fwd->ParentPortID);

    if (idbsb == NULL || port == NULL)
	return(TRUE);

    displaybuf = malloc(WIDTH_DEFAULT);
    if (displaybuf == NULL)
	return (TRUE);
    DisplayBufSize = WIDTH_DEFAULT;

    /* Print parent info */
    safe_sprintf(&displaybuf, &DisplayBufSize,
		 "\n%12s %3s %18s ",
		 smrp_DisplayGroupAddr(fwd->GroupAddress),
		 fwd->CacheValid?"Y":"N",
		 idb_get_namestring(idbsb->phys_idb));

    for (cport = fwd->ChildPorts; cport; cport = cport->Next) {
	/* Skip non-f/s entries */
        if (!cport->FastCache) 
	    continue;
	
        port = smrp_FindPort(cport->ChildPortID);
        idbsb = smrp_GetPortIDNetInterface(cport->ChildPortID);
	if (port == NULL || idbsb == NULL)
	    continue;
        if (first_time) {
            first_time = FALSE;
            safe_sprintf(&displaybuf, &DisplayBufSize,
			 "%18s %s\n                                                       %s",
			 idb_get_namestring(idbsb->phys_idb), 
			 smrpfast_DumpCacheMacHdr(cport->FastCache, buf1),
			 smrpfast_DumpCacheNetHdr(cport->FastCache, buf2));
	    

	} else {
            safe_sprintf(&displaybuf, &DisplayBufSize,
			 "\n                                    %18s %s\n                                                       %s",
			 idb_get_namestring(idbsb->phys_idb), 
			 smrpfast_DumpCacheMacHdr(cport->FastCache, buf1),
			 smrpfast_DumpCacheNetHdr(cport->FastCache, buf2));

        }
    }

    printf("%s", displaybuf);
    free(displaybuf);
    return(TRUE);
}

static const uchar smrp_mcache_banner[] = 
    {"Group        In  Parent             Child              MAC Header (Top)\n"
     "Address      Use Interface          Interface(s)       Network Header (Bottom)\n"
     "------------------------------------------------------------------------------\n"};
/*
 * smrp_show_mcache
 *
 * Show SMRP multicast cache table
 */
static void
smrp_show_mcache (parseinfo *csb)
{
    SMRPForward* fwd, *nextfwd;
    SMRPGroupID group;
    int ntype;
    SMRPPort *port;
    
    /* Display specified forwarding entry */
    if (GETOBJ(int,4)) {
        group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
        fwd = smrp_FindForward(group);
        automore_enable(smrp_mcache_banner);
        if (fwd)
            smrp_DisplayMCache((treeLink*) fwd, &group);
        else
            printf("No such group in multicast cache\n");
        automore_disable();
        return;
    }

    printf("\nSMRP Multicast Fast Switching Cache");

    automore_enable(smrp_mcache_banner);

    ntype = GETOBJ(int,1);

    fwd = smrp_GetFirstForward();
    while (fwd != NULL) {
	nextfwd = smrp_GetNextForward(fwd);

	if (ntype) {
	    port = smrp_FindPort(fwd->ParentPortID);
	    if (!port || port->NetworkType != ntype) {
		fwd = nextfwd;
		continue;
	    }
	}

	if (nextfwd)
	    group = nextfwd->GroupAddress;
	smrp_DisplayMCache((treeLink *) fwd, NULL);
	if (nextfwd) {
	    fwd = smrp_FindForward(group);
	    if (fwd == NULL) {
		printf("\nSMRP: Can't find next multicast cache entry.  "
		       "Reissue show command.");
		break;
	    }
	} else
	    break;
    }
    automore_disable();

}

	
/*
 * smrp_DisplayGroup
 *
 * Display group info.
 */
static void
smrp_DisplayGroup (SMRPGroup* grp)
{
    SMRPIdbsb * idbsb;
    SMRPPort* port;
    char buf[30];
    char *displaybuf;
    uint DisplayBufSize;

    if (!grp)
        return;

    idbsb = smrp_GetPortIDNetInterface(grp->CreatorPort);
    port = smrp_FindPort(grp->CreatorPort);

    if (!idbsb || !port)
	return;

    displaybuf = malloc(WIDTH_DEFAULT);
    if (displaybuf == NULL)
	return;
    DisplayBufSize = WIDTH_DEFAULT;

    safe_sprintf(&displaybuf, &DisplayBufSize,
		 "\n%12s %8s %4ld %18s %9s",
		 smrp_DisplayGroupAddr(grp->GroupAddress),
		 smrp_DisplayTime(ELAPSED_TIME(grp->CreateTime)),
		 mgd_timer_left_sleeping(&grp->CreatorTimer)/ONESEC,
		 idb_get_namestring(idbsb->phys_idb),
		 PORTCB_PrintNetAddress(port, buf, grp->CreatorAddress));
    printf("%s", displaybuf);
    free(displaybuf);
}

/*
 * smrp_DisplayPortGroups
 *
 * Display groups for specified port.
 */
static boolean
smrp_DisplayPortGroups (treeLink* link, void* data)
{
    SMRPPort* port = (SMRPPort*) link;
    SMRPGroupID* gid = (SMRPGroupID*) data;
    SMRPGroup* grp, *nextgrp;
    SMRPGroupID groupid = 0;
    
    /* Display specified group address */
    if (gid) {
	grp = smrp_FindPortGroup(port, *gid);
	if (grp)
	    smrp_DisplayGroup(grp);
	return(TRUE);
    } 

    grp = port->GroupTable;
    while (grp) {
	nextgrp = grp->Next;
	if (nextgrp)
	    groupid = nextgrp->GroupAddress;
	smrp_DisplayGroup(grp);
	if (nextgrp) {
	    grp = smrp_FindPortGroup(port, groupid);
	    if (grp == NULL) {
		printf("\nSMRP: Can't find next group.  "
		       "Reissue show command.");
		break;
	    }
	} else
	    break;
    }
    return(TRUE);
}

/*
 * smrp_DisplayProtoGroup
 *
 * Display group info for the specified protocol.
 */
static boolean
smrp_DisplayProtoGroup (treeLink* link, void* proto)
{
    SMRPPort* port = (SMRPPort*) link;
 
    if (!port)
	return TRUE;

    if (port->NetworkType == (int)proto) 
        smrp_DisplayPortGroups((treeLink*)port,NULL);

    return(TRUE);
}

static const uchar smrp_group_banner[] = 
    {"Group        Creation Next            Creator      \n"
     "Address      Time     Poll Interface          Address\n"
     "-----------------------------------------------------\n"};
/*
 * smrp_show_group
 *
 * Show SMRP group table
 */
static void
smrp_show_group (parseinfo *csb)
{
    SMRPPort *port;
    SMRPRoute *route;
    SMRPGroupID group;
    int ntype;

    /* Display specified group address */
    if (GETOBJ(int,4)) {
        group = smrp_MakeGroupID(GETOBJ(int,2), GETOBJ(int,3));
        automore_enable(smrp_group_banner);
	route = smrp_FindRouteByNet(GETOBJ(int,2));
	if (route) {
	    port = smrp_FindPort(route->ParentNodePortID);
	    if (port)
		smrp_DisplayPortGroups((treeLink *)port, &group);
	}
        automore_disable();
        return;
    }

    printf("\nSMRP Group Table");
    automore_enable(smrp_group_banner);

    ntype = GETOBJ(int,1);
    if (ntype) {
        /* All groups for specified network protocol */
        smrp_ForAllPortsDo(smrp_DisplayProtoGroup, (void*)ntype);
    } else {
        /* Display all groups */
        smrp_ForAllPortsDo(smrp_DisplayPortGroups, NULL);
    }
    automore_disable();

}

/*
 * smrp_show_port_traffic
 * print smrp port statistics 
 */
static void
smrp_show_port_traffic (void)
{
    automore_enable("SMRP port statistics:");

    printf("\n  Ports:  %d new, %d recycled, %d deleted",
        SMRPPORT_stat[SMRPPORT_NEW_PORTS],
        SMRPPORT_stat[SMRPPORT_RECYCLED_PORTS],
        SMRPPORT_stat[SMRPPORT_DELETED_PORTS]);
    automore_disable();
}

/*
 * smrp_show_neighbors_traffic
 *
 * Show SMRP neighbors traffic
 */
static void
smrp_show_neighbors_traffic (void)
{
    automore_enable("SMRP neighbor statistics:");

    printf("\n  Neighbors:  %d new, %d recycled, %d deleted",
        SMRPNODE_stat[SMRPNODE_NEW_NODES],
        SMRPNODE_stat[SMRPNODE_RECYCLED_NODES],
        SMRPNODE_stat[SMRPNODE_DELETED_NODES]);

    automore_disable();
}

/*
 * smrp_DisplayNbrRteTraffic
 *
 * Display neighbor statistics.
 */
static boolean
smrp_DisplayNbrRteTraffic (treeLink* link, void* p)
{
    char buf[30];
    SMRPNode* neighbor = (SMRPNode*) link;
    SMRPPort *port;
    ulong* Neighbor_Stats;

    if (!neighbor)
        return(TRUE);

    Neighbor_Stats = neighbor->Routing_Stats;
    port = smrp_FindPort(neighbor->PortID);

    printf("\n  Neighbor AT %s:", 
        PORTCB_PrintNetAddress(port, buf, neighbor->NodeAddress));
    printf("\n	 %d received updates, %d sent updates",
	   Neighbor_Stats[NODEROUTE_UPDATES_IN],
	   Neighbor_Stats[NODEROUTE_UPDATES_OUT]);
    printf("\n	 %d received routes, %d sent routes",
	   Neighbor_Stats[NODEROUTE_VECTORS_IN],
	   Neighbor_Stats[NODEROUTE_VECTORS_OUT]);
    printf("\n	 %d poisoned, %d improved",
	   Neighbor_Stats[NODEROUTE_POISONED],
	   Neighbor_Stats[NODEROUTE_IMPROVED]);
    printf("\n	 %d better parent interface%s, %d worst parent interface%s",
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_BETTERPARENTINT],"","s"),
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_WORSTPARENTINT],"","s"));
    printf("\n	 %d better parent address%s, %d worst parent address%s",
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_BETTERPARENTADDR],"","es"),
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_WORSTPARENTADDR],"","es"));
    printf("\n	 %d bad range%s, %d overlap%s",
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_BADRANGE],"","s"),
	   ArgAndPlural(Neighbor_Stats[NODEROUTE_OVERLAPS_EXISTING],"","s"));

    return(TRUE);
}

/*
 * smrp_show_route_traffic
 * print smrp route statistics 
 */
static void
smrp_show_route_traffic (void)
{
    automore_enable("SMRP route statistics:");

    printf("\n  Routes:  %d new, %d recycled, %d deleted",
        SMRPROUTE_stat[SMRPROUTE_NEW_ROUTE],
        SMRPROUTE_stat[SMRPROUTE_RECYCLED_ROUTE],
        SMRPROUTE_stat[SMRPROUTE_DELETED_ROUTE]);

    /* Display statistics for all neighbors */
    smrp_ForAllNodesDo(smrp_DisplayNbrRteTraffic, NULL);

    automore_disable();
}

/*
 * smrp_show_group_traffic
 * print smrp group statistics 
 */
static void
smrp_show_group_traffic (void)
{
    automore_enable("SMRP group statistics:");

    printf("\n  Groups:  %d added, %d removed",
        SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_ADDED],
        SMRPGROUP_stat[SMRPGROUP_PORTGROUPS_FREED]);
    printf("\n  Forwards:  %d new, %d recycled, %d deleted",
        SMRPGROUP_stat[SMRPGROUP_NEW_FORWARDS],
        SMRPGROUP_stat[SMRPGROUP_RECYCLED_FORWARDS],
        SMRPGROUP_stat[SMRPGROUP_DELETED_FORWARDS]);
    printf("\n  Child Ports:  %d added, %d freed",
        SMRPGROUP_stat[SMRPGROUP_CHILDPORTS_ADDED],
        SMRPGROUP_stat[SMRPGROUP_CHILDPORTS_FREED]);
    printf("\n  Misc:  %d range full%s, %d not primary drop%s",
        ArgAndPlural(SMRPGROUP_stat[SMRPGROUP_RANGE_FULL],"","s"),
        ArgAndPlural(SMRPGROUP_stat[SMRPGROUP_NOT_PRIMARY],"","s"));
    printf("\n	 %d no route%s",
	   ArgAndPlural(SMRPGROUP_stat[SMRPGROUP_NO_ROUTE],"","s"));

    automore_disable();
}

/*
 * smrp_show_trans_traffic
 * print smrp transaction statistics 
 */
static void
smrp_show_trans_traffic (void)
{
    automore_enable("SMRP transaction statistics:");

    printf("\n  Requests:  %d new, %d recycled",
        SMRPTRANS_stat[SMRPTRANS_REQUESTS_NEW],
        SMRPTRANS_stat[SMRPTRANS_REQUESTS_RECYCLED]);
    printf("\n	 %d deleted, %d freed",
        SMRPTRANS_stat[SMRPTRANS_REQUESTS_DELETED],
        SMRPTRANS_stat[SMRPTRANS_REQUESTS_FREED]);
    printf("\n	 %d timeout%s, %d resend%s",
        ArgAndPlural(SMRPTRANS_stat[SMRPTRANS_REQUESTS_TIMEOUTS],"","s"),
        ArgAndPlural(SMRPTRANS_stat[SMRPTRANS_REQUESTS_RESENDS],"","s"));
    printf("\n	 %d duplicate%s, %d incomplete duplicate%s",
        ArgAndPlural(SMRPTRANS_stat[SMRPTRANS_REQUESTS_DUP],"","s"),
        ArgAndPlural(SMRPTRANS_stat[SMRPTRANS_REQUESTS_DUP_INCOMPLETE],"","s"));
    printf("\n	 %d unexpected",
        SMRPTRANS_stat[SMRPTRANS_REQUESTS_UNEXPECTED]);

    printf("\n  Responses:  %d new, %d recycled",
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_NEW],
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_RECYCLED]);
    printf("\n	 %d deleted, %d freed",
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_DELETED],
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_FREED]);
    printf("\n	 %d unexpected, %d bad",
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_UNEXPECTED],
        SMRPTRANS_stat[SMRPTRANS_RESPONSES_BAD]);

    automore_disable();
}

/*
 * smrp_show_traffic
 * print smrp traffic statistics 
 */
static void
smrp_show_traffic (void)
{
    automore_enable("SMRP statistics:");

    printf("\n  Rcvd:  %d total, %d hello%s",
	   smrp_stat[SMRP_INPUT],
	   ArgAndPlural(smrp_stat[SMRP_HELLO_IN],"","s"));
    printf("\n	 %d mc data, %d fast handled",
	   smrp_stat[SMRP_MCDATA_IN],
	   smrp_stat[SMRP_FASTFWD_IN]);
    printf("\n	 %d request%s, %d confirm%s, %d reject%s",
	   ArgAndPlural(smrp_stat[SMRP_REQUESTS_IN],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_CONFIRMS_IN],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_REJECTS_IN],"","s"));
    printf("\n	 %d primar%s, %d secondar%s",
	   ArgAndPlural(smrp_stat[SMRP_PNODE_IN],"y","ies"),
	   ArgAndPlural(smrp_stat[SMRP_SNODE_IN],"y","ies"));
    printf("\n	 %d notif%s, %d distance vector%s",
	   ArgAndPlural(smrp_stat[SMRP_NOTIFY_IN],"y","ies"),
	   ArgAndPlural(smrp_stat[SMRP_DISTANCE_IN],"","s"));
    printf("\n	 %d create group%s, %d delete group%s",
           ArgAndPlural(smrp_stat[SMRP_CREATE_IN],"","s"),
           ArgAndPlural(smrp_stat[SMRP_DELETE_IN],"","s"));
    printf("\n	 %d join group%s, %d leave group%s",
           ArgAndPlural(smrp_stat[SMRP_JOIN_IN],"","s"),
           ArgAndPlural(smrp_stat[SMRP_LEAVE_IN],"","s"));
    printf("\n	 %d member%s",
           ArgAndPlural(smrp_stat[SMRP_MEMBER_IN],"","s"));
    printf("\n	 %d add group entr%s, %d remove group entr%s",
           ArgAndPlural(smrp_stat[SMRP_ADDGRPENTRY_IN],"y","ies"),
           ArgAndPlural(smrp_stat[SMRP_REMGRPENTRY_IN],"y","ies"));
    printf("\n	 %d locate%s, %d tunnel%s",
           ArgAndPlural(smrp_stat[SMRP_LOCATE_IN],"","s"),
           ArgAndPlural(smrp_stat[SMRP_TUNNEL_IN],"","s"));

    printf("\n  Sent:  %d total, %d hello%s",
	   smrp_stat[SMRP_OUTPUT],
	   ArgAndPlural(smrp_stat[SMRP_HELLO_OUT],"","s"));
    printf("\n	 %d duplicate mc data, %d mc data, %d fast forwarded",
	   smrp_stat[SMRP_FORWARD_DUPS],
	   smrp_stat[SMRP_FORWARDS],
	   smrp_stat[SMRP_FASTFWD_OUT]);
    printf("\n	 %d request%s, %d confirm%s, %d reject%s",
	   ArgAndPlural(smrp_stat[SMRP_REQUESTS_OUT],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_CONFIRMS_OUT],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_REJECTS_OUT],"","s"));
    printf("\n	 %d primar%s, %d secondar%s",
	   ArgAndPlural(smrp_stat[SMRP_PNODE_OUT],"y","ies"),
	   ArgAndPlural(smrp_stat[SMRP_SNODE_OUT],"y","ies"));
    printf("\n	 %d notif%s, %d distance vector%s",
	   ArgAndPlural(smrp_stat[SMRP_NOTIFY_OUT],"y","ies"),
	   ArgAndPlural(smrp_stat[SMRP_DISTANCE_OUT],"","s"));
    printf("\n	 %d join%s, %d leave%s",
           ArgAndPlural(smrp_stat[SMRP_JOIN_OUT],"","s"),
           ArgAndPlural(smrp_stat[SMRP_LEAVE_OUT],"","s"));
    printf("\n	 %d creator%s, %d member%s",
           ArgAndPlural(smrp_stat[SMRP_CREATOR_OUT],"","s"),
           ArgAndPlural(smrp_stat[SMRP_MEMBER_OUT],"","s"));
    printf("\n	 %d add group entr%s, %d remove group entr%s",
           ArgAndPlural(smrp_stat[SMRP_ADDGRPENTRY_OUT],"y","ies"),
           ArgAndPlural(smrp_stat[SMRP_REMGRPENTRY_OUT],"y","ies"));

    printf("\n  Misc:  %d no buffer%s, %d no forward%s",
	   ArgAndPlural(smrp_stat[SMRP_NO_BUFFERS],"","s"),
           ArgAndPlural(smrp_stat[SMRP_NO_FORWARD],"","s"));
    printf("\n	 %d bad portid%s, %d port down%s",
	   ArgAndPlural(smrp_stat[SMRP_BAD_PORTID],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_PORT_DOWN],"","s"));
    printf("\n	 %d bad version%s, %d runt%s",
	   ArgAndPlural(smrp_stat[SMRP_BADVERS],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_RUNT],"","s"));
    printf("\n	 %d bad packet type%s, %d input error%s",
	   ArgAndPlural(smrp_stat[SMRP_BAD_PKTTYPE],"","s"),
	   ArgAndPlural(smrp_stat[SMRP_INPUT_ERR],"","s"));
    
    automore_disable();

}

/*
 * smrp_show_commands
 */
void
smrp_show_commands (parseinfo *csb)
{
    if (!smrp_running) {
        printf("\n%% SMRP not running");
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
    case SHOW_SMRP_FWD:
        smrp_show_forward(csb);
        break;
    case SHOW_SMRP_GLOBALS:
        smrp_show_globals();
        break;
    case SHOW_SMRP_GROUP:
        smrp_show_group(csb);
        break;
    case SHOW_SMRP_MCACHE:
        smrp_show_mcache(csb);
        break;
    case SHOW_SMRP_NBR:
        smrp_show_neighbors(csb);
        break;
    case SHOW_SMRP_PORT:
        smrp_show_port(csb);
        break;
    case SHOW_SMRP_PRIVATE:
        switch (GETOBJ(int,1)) {
        case SMRP_SHOW_PVT_ALL:
            smrp_show_pvt_req();
            smrp_show_pvt_resp();
            break;
        case SMRP_SHOW_PVT_REQ:
            smrp_show_pvt_req();
            break;
        case SMRP_SHOW_PVT_RESP:
            smrp_show_pvt_resp();
            break;
	}
        break;
    case SHOW_SMRP_ROUTE:
        smrp_show_route(csb);
        break;
    case SHOW_SMRP_TRAFFIC:
        switch (GETOBJ(int,1)) {
        case SMRP_SHOW_TRAFFIC_ALL:
            smrp_show_traffic();
            smrp_show_group_traffic();
            smrp_show_neighbors_traffic ();
            smrp_show_port_traffic();
            smrp_show_route_traffic();
            smrp_show_trans_traffic();
            break;
        case SMRP_SHOW_TRAFFIC_GROUP:
            smrp_show_group_traffic();
            break;
        case SMRP_SHOW_TRAFFIC_NBR:
            smrp_show_neighbors_traffic();
            break;
        case SMRP_SHOW_TRAFFIC_PORT:
            smrp_show_port_traffic();
            break;
        case SMRP_SHOW_TRAFFIC_ROUTE:
            smrp_show_route_traffic();
            break;
        case SMRP_SHOW_TRAFFIC_TRANS:
            smrp_show_trans_traffic();
            break;
        default:
            smrp_show_traffic();
            break;
        }
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

