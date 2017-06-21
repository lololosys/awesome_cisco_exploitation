/* $Id: atip_parse.c,v 3.6.18.6 1996/07/10 22:19:23 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atip_parse.c,v $
 *------------------------------------------------------------------
 * Appletalk IP-services parser routines
 *
 * March 1994, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 $ $Log: atip_parse.c,v $
 * Revision 3.6.18.6  1996/07/10  22:19:23  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.18.5  1996/06/29  06:38:45  smackie
 * Positive Karma Adjustment. Fix 11.2 compilation errors. (CSCdi61558)
 * Branch: California_branch
 *
 * Revision 3.6.18.4  1996/06/29  00:07:21  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.6.18.3  1996/05/02  08:41:13  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.6.18.2  1996/03/23  01:28:15  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.18.1  1996/03/18  18:53:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:07:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  00:18:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/07  16:10:01  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  05:59:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:15  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/17  08:44:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_globals.h"
#include "macip_util.h"
#include "macip_db.h"
#include "macip_fsm.h"
#include "macip_server.h"
#include "parser_defs_atalk.h"
#include "../ip/tunnel.h"
#include "../ip/ipaddress.h"

extern void macip_command(parseinfo *csb);
extern boolean atalkif_cmdzone(parseinfo *, atalkidbtype *);

void
show_appleip (parseinfo *csb)
{
    if (!atalk_running) {
	printf(atalk_not_running);
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    case SHOW_APPLE_MACIP_CLIENTS:
	macip_show_clients();
	break;
    case SHOW_APPLE_MACIP_SERVERS:
	macip_show_servers();
	break;
    case SHOW_APPLE_MACIP_TRAFFIC:
	macip_show_traffic();
	break;
    }
    automore_disable();
}

void
appleip_command (parseinfo *csb)
{
    csb->interface = NULL;

    if (csb->nvgen) {
	if (!atalk_running)
	    return;

	switch (csb->which) {
	case ATALK_MACIP:
	    macip_command(csb);
	    break;
	case ATALK_IPTALK_BASEPORT:
	    nv_write((atalk_iptalk_baseport != ATALK_IPTALK_BASEPORT_DEFAULT),
		     "%s %d", csb->nv_command, atalk_iptalk_baseport);
	    break;
	default:
	    printf("\n%% NVGEN: unknown command - %s", csb->nv_command);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case ATALK_MACIP:
 	macip_command(csb);
	macip_start();	/* start macip process if not yet started */
	break;
    case ATALK_IPTALK_BASEPORT:
	atalk_iptalk_baseport = csb->sense ? GETOBJ(int,1)
                                           : ATALK_IPTALK_BASEPORT_DEFAULT;
	break;

    default:
	buginf("\nAT: Unhandled configuration command");
	break;
    }
}	

void
atipif_command (parseinfo *csb)
{
    idbtype *idb;
    atalkidbtype *atalkidb;
    
    if (!atalk_running) {
	if (!csb->nvgen) {
	    printf(atalk_not_running);
	}
	return;
    }

    idb = csb->interface;

    if (csb->nvgen) {
	zipifentry *z;

	atalkidb = atalk_getidb(idb);
	
	if (!(atalkidb && atalkidb->atalk_enabled && atalk_running)) {
	    return;
	}

	switch (csb->which) {
	case ATALK_IPTALK:
	    z = atalkif_firstZone(atalkidb);
	    if (z) {
		nv_write(TRUE, "%s %d ", csb->nv_command,
			 atalkidb->atalknet);
		atalk_nv_printzone(z->zone->pname);
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
	    
    if (atalk_getidb(idb) == NULL) {
	atalk_init_idb(idb);
	if (atalk_getidb(idb) == NULL) {
	    printf("\n%% Cannot create data structures for atalk interface "
		   "information.");
	    return;
	}
	atalk_rxtypes_update(idb);
    }

    atalkidb = atalk_getidb(idb);
    
    switch (csb->which) {
    case ATALK_IPTALK:
	if (csb->sense) {
	    tunnel_info *tinfo;
	    idbtype *ip_idb;
	    ushort net;
	    
	    net = GETOBJ(int,1);
	    tinfo = idb->hwptr->tunnel;

	    if (tinfo->source) {
		ip_idb = ipaddr_findidb(tinfo->source);
		if (!ip_idb)
		    printf("\nInterface not found for tunnel source ip "
			   "address");
		return;
	    } else if (tinfo->source_idb) {
		if (!idb_sw_get_ipaddress(tinfo->source_idb)) {
		    printf("\nTunnel source interface has no ip address");
		    return;
		}
		ip_idb = tinfo->source_idb;
	    } else {
		printf("\nMust specify tunnel source first");
		return;
	    }
	    atalkif_setup(atalkidb, net, 0, net,
			  (~(ip_idb->ip_nets_mask)) &
			  idb_sw_get_ipaddress(ip_idb) & 0xff,
			  TRUE);
	    atalkif_cmdzone(csb, atalkidb);
	} else
	    atalkif_shutdown(atalkidb);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
