/* $Id: smrp_parse.c,v 3.4.18.1 1996/03/18 21:47:48 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/smrp/smrp_parse.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP parser commands.
 *
 * All command parsing is performed by this module. 
 *
 * Special network layer functions are provided to handle the SMRP
 * specific network range parameter.  This parse function returns the
 * SMRPNet structure that is used to create a port.  
 * After parsing, the network layer must check the ConfiguredRange
 * flag to determine if automatic mapping should be performed.  If not
 * set, the RangeStart and RangeEnd fields in the structure are
 * invalid.  They should be set to a valid range based upon the 
 * network configuration.  Note: If the range was not configured, SMRP
 * attempts to use the mapped range but if it disagrees with the 
 * local network the port will use the active values instead.  The
 * converse is not true, a configured range will keep the port down
 * until either the active range matches or there is no primary node
 * to prevent port startup.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_parse.c,v $
 * Revision 3.4.18.1  1996/03/18  21:47:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:59:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  17:10:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:15:17  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1995/11/17  18:53:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:25:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/09  03:41:59  kleung
 * CSCdi38461:  Need to support serial interface for SMRP.
 *
 * Revision 2.3  1995/08/01  00:22:54  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/14  22:55:08  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "../smrp/parser_defs_smrp.h"
#include "../smrp/smrp_fast.h"

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


/*
 *------------------------------------------------------------------
 * smrp_parse_Initialize		(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_parse_Initialize (void)
{
}

/*
 *------------------------------------------------------------------
 * smrp_RouteCache
 *
 * Enable/disable SMRP fast switching.  The SMRP multicast route
 * cache is updated if enabled.
 *------------------------------------------------------------------
 */
static void
smrp_MCRouteCache(parseinfo* csb)
{	
    idbtype* idb;
    hwidbtype* hwidb;
    SMRPIdbsb *idbsb;
    int nettype;
    SMRPPort* port;

    idb = csb->interface;

    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL) {
	printf("\n%% Hardware interface is missing");
	return;
    }

    idbsb = smrp_GetIdbsb(idb);
    nettype = GETOBJ(int, 1);

    /* 
     * NV generation 
     */
    if (csb->nvgen) {
	port = smrp_FindPortFromIdb(idbsb, nettype);
	if (port == NULL) 
	    return;
	
	if (!is_x25(hwidb)) {
	    nv_write((port->RouteCache == fsUserDisabled),
		     "no %s", csb->nv_command);
	    return;
	}
    }
	    
    if (!smrp_running) {
	printf("\n%%SMRP: SMRP routing must first be enabled");
	return;
    }

    port = smrp_FindPortFromIdb(idbsb, nettype);
    if (port == NULL) {
	printf("\n%%SMRP: SMRP not enabled on this interface");
	return;
    }

    /*
     * Don't allow multicast fastswitching on X.25 interfaces.
     */
    if (is_x25(hwidb)) {
	printf("\nSMRP fastswitching not allowed on X.25 interfaces");
	return;
    }

    port->RouteCache = fsUnknown;
    if (csb->sense) 
	smrpfast_ReinitPort(port);	/* Give any hardware warnings */
    else {
	port->RouteCache = fsUserDisabled;
	smrpfast_SetupPort(port);
    }
}



/*
 *------------------------------------------------------------------
 * smrp_ParseProtocol			(Network Layer Entrypoint)
 *
 * Parse a SMRP network range and return an SMRPNet structure.  If
 * a parse error occurs (or no more memory), return NULL.
 *
 * Performs all necessary checking of the SMRP network range.
 *
 * Asumption: Called as an interface configuration command. 
 *	proto smrp-net-range n-n	;; Yields an init SMRPNet*
 *	proto smrp-net-range 		;; Yields an empty SMRPNet*
 *   no proto smrp-net-range		;; Yields NULL.
 *------------------------------------------------------------------
 */
static void
smrp_ParseProtocol (parseinfo *csb)
{
    SMRPNetRange range;
    SMRPNet net;
    SMRPCallDowns const *handlers;
    SMRPPort *port;
    SMRPPortID pid;
    idbtype *idb;
    hwidbtype *hwidb;
    

    idb = csb->interface;
    SMRPASSERT(idb != NULL);

    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL) {
	printf("\n%% Hardware interface is missing");
	return;
    }

    /* 
     * NV generation 
     */
    if (csb->nvgen) {
	reg_invoke_smrp_nvgen_interface_protocol(csb);
	return;
    }
    
    if (!smrp_running) {
	printf("\n%%SMRP: SMRP routing must first be enabled");
	return;
    }

    if (!idb_is_ethernet(idb) &&
	!is_tunnel(hwidb) &&
	!(hwidb->status & IDB_SERIAL) &&
	!idb_is_fddi(idb)) {
	printf("\n%%SMRP: SMRP not supported on this interface");
	return;
    }
	
    if (GETOBJ(int,1) == 0) {
	range.Start = 0;
	range.End = 0;
    } else {
	range.Start = GETOBJ(int, 1);
	range.End = GETOBJ(int, 2);
	if (!smrp_IsValidNetRange(&range)) {
	    printf("\n%%SMRP: SMRP invalid network range");
	    return;
	}
    }
    net.NetType = GETOBJ(int, 3);
    net.NetInterface = csb->interface;
    net.RangeStart = range.Start;
    net.RangeEnd = range.End;
    net.ConfiguredRange = (range.Start != 0);

    if (!csb->sense) {
	pid = smrp_MakePortID(&net);
	port = smrp_FindPort(pid);
	if (!port) {
	    printf("\n%%SMRP: Port not found");
	    return;
	}
	smrp_DeletePortID(pid);
	return;
    }
    
    handlers = NULL;
    
    reg_invoke_smrp_add_netrange(net.NetType, &net, &handlers, TRUE);

    if (handlers == NULL)
	return;
    
    port = smrp_CreatePort(&net, handlers);

    if (!port)
	return;

    smrp_StartPort(port);
    
}

void
smrp_command (parseinfo *csb)
{

    if (csb->nvgen) {
	if (!smrp_running)
	    return;

	switch (csb->which) {
	case SMRP_ROUTING:
	    nv_write(smrp_running, "%s", csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case SMRP_ROUTING:
	if (csb->sense)
	    smrp_start();
	else
	    smrp_stop();
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void
smrpif_command (parseinfo *csb)
{
    idbtype* idb;

    idb = csb->interface;

    switch (csb->which) {
    case SMRP_MCROUTEC:
	smrp_MCRouteCache(csb);
	break;

    case SMRP_PROTO:
	smrp_ParseProtocol(csb);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
