/* $Id: smrp_clear.c,v 3.5 1996/02/07 16:15:15 widmer Exp $
 * $Source: /release/111/cvs/Xsys/smrp/smrp_clear.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP clear commands.
 *
 * April 1995, Kent Leung
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_clear.c,v $
 * Revision 3.5  1996/02/07  16:15:15  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:08:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:29:45  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:24:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/01  00:22:39  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/14  22:54:55  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../util/tree.h"
#include "../atalk/parser_defs_atalk.h"
#include "../smrp/parser_defs_smrp.h"
#include "../smrp/smrp_private.h"
#include "smrp_registry.h"

/*
 * smrp_DeleteDownedNbr
 *
 * Delete neighbors that are down.
 */
static boolean
smrp_DeleteDownedNbr (treeLink* link, void* p)
{
    SMRPNode* neighbor = (SMRPNode*) link;
 
    if (!neighbor)
        return(FALSE);

    if (smrp_IsNodeOperational(neighbor))
        return(FALSE);

    smrp_DeleteNode(neighbor);

    return(TRUE);
}

/*
 * smrp_DeleteDownedProtoNbr
 *
 * Display neighbor info for the specified protocol.
 */
static boolean
smrp_DeleteDownedProtoNbr (treeLink* link, void* proto)
{
    SMRPNode* neighbor = (SMRPNode*) link;
    SMRPPort* port;
 
    if (!neighbor)
        return(FALSE);

    port = smrp_FindPort(neighbor->PortID);

    if (!port)
	return(FALSE);

    if (port->NetworkType == (int) proto) 
        smrp_DeleteDownedNbr((treeLink*)neighbor, NULL);

    return(TRUE);
}

/*
 * smrp_ClearNeighbor
 *
 */
void
smrp_ClearNeighbor (parseinfo *csb)
{
    SMRPNode* node;
    UnicastAddr naddr;    

    /* Specific neighbor */
    if (GETOBJ(int,2)) {
        
        reg_invoke_smrp_parse_netaddr(GETOBJ(int,1), GETOBJ(paddr,1),
            &naddr);
        node = smrp_FindAdjacentNode(naddr);

        if (node) {
            if (!smrp_DeleteDownedNbr((treeLink*)node, NULL))
                printf("Neighbor is not down, cannot clear\n");
        } else
            printf("No such neighbor\n");

        return;
    }
   
    
    if (GETOBJ(int,1)) {
        /* All neighbors for specified network protocol */
        smrp_ForAllNodesDo(smrp_DeleteDownedProtoNbr, (void*)GETOBJ(int,1));
    } else {
        /* All neighbors */
        smrp_ForAllNodesDo(smrp_DeleteDownedNbr, NULL);
    }
}

/*
 * smrp_DeletePoisonedRoute
 *
 */
static boolean
smrp_DeletePoisonedRoute (treeLink* link, void* p)
{
    SMRPRoute* route = (SMRPRoute*) link;

    if (!route)
        return(FALSE);

    if (route->Distance != MCMaximumDistance) 
        return(FALSE);

    smrp_DeleteRoute(route);

    return(TRUE);
}

/*
 * smrp_ClearRoute
 *
 */
static void
smrp_ClearRoute (parseinfo *csb)
{
    SMRPRoute* route;
    SMRPNetRange range;

    /* Display a specific route */
    if (GETOBJ(int,3)) {
        range.Start = range.End = GETOBJ(int,2);
        route = smrp_FindRoute(&range);

        if (route) {
            if (!smrp_DeletePoisonedRoute((treeLink*)route, NULL))
                printf("Route is not poisoned, cannot clear\n");
        } else
            printf("No such route\n");
        return;
    }

    /* Delete all poisoned routes */
    smrp_ForAllRoutesDo(smrp_DeletePoisonedRoute, NULL);

}

/*
 * smrp_ClearNeighborTraffic
 *
 */
static boolean
smrp_ClearNeighborTraffic (treeLink* link, void* p)
{
    SMRPNode* neighbor = (SMRPNode*) link;
    int i;
    
    if (!neighbor)
        return(TRUE);

    for (i=0; i<NODEROUTE_MAXTRAFFIC; ++i) 
        neighbor->Routing_Stats[i] = 0;

    return(TRUE);
}

/*
 * smrp_ClearTraffic
 *
 */
void 
smrp_ClearTraffic (void)
{
    ushort i;

    for (i=0; i<SMRPPORT_MAXTRAFFIC; ++i) 
        SMRPPORT_stat[i] = 0;
    for (i=0; i<SMRPROUTE_MAXTRAFFIC; ++i) 
        SMRPROUTE_stat[i] = 0;
    for (i=0; i<SMRPGROUP_MAXTRAFFIC; ++i) 
        SMRPGROUP_stat[i] = 0;
    for (i=0; i<SMRPTRANS_MAXTRAFFIC; ++i) 
        SMRPTRANS_stat[i] = 0;
    for (i=0; i<SMRPNODE_MAXTRAFFIC; ++i) 
        SMRPNODE_stat[i] = 0;
    for (i=0; i<SMRP_MAXTRAFFIC; ++i) 
        smrp_stat[i] = 0;

    smrp_ForAllNodesDo(smrp_ClearNeighborTraffic, NULL);
}

/*
 * smrp_clear_command
 *
 * This command is designed to act as much as possible as the
 * "show appletalk" exec command, so as to minimise confusion.
 */
void 
smrp_clear_commands (parseinfo *csb)
{
    if (!smrp_running) {
	printf("\n%% SMRP is not running");
        return;
    }

    switch (csb->which) {
    case CLEAR_SMRP_MCACHE:
	smrpfast_InvalidateAllCache("smrp_clear_commands", NULL);
        smrpfast_fscache_flush();
        break;
    case CLEAR_SMRP_NEIGHBOR:
        smrp_ClearNeighbor(csb);
        break;
    case CLEAR_SMRP_ROUTE:
        smrp_ClearRoute(csb);
        break;
    case CLEAR_SMRP_TRAFFIC:
        smrp_ClearTraffic();
        break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

