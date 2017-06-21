/* $Id: ataurp_parse.c,v 3.1.8.4 1996/07/10 22:19:17 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/ataurp_parse.c,v $
 *------------------------------------------------------------------
 * Appletalk AURP-service parser routines
 *
 * March 1996, Dean Wong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ataurp_parse.c,v $
 * Revision 3.1.8.4  1996/07/10  22:19:17  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.8.3  1996/06/29  06:38:43  smackie
 * Positive Karma Adjustment. Fix 11.2 compilation errors. (CSCdi61558)
 * Branch: California_branch
 *
 * Revision 3.1.8.2  1996/06/29  00:07:19  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.1.8.1  1996/05/02  08:41:03  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/03/14  00:31:40  dwong
 * Placeholder for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <mgd_timers.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_globals.h"
#include "../parser/parser_defs_exec.h"
#include "../atalk/parser_defs_atalk.h"
#include "../ip/tunnel.h"
#include "at_aurp.h"

extern boolean atalkif_cmdzone(parseinfo *, idbtype *);
extern void aurp_show_topology(ushort);
extern void aurp_show_events(ushort);
extern boolean aurp_config_tunnel(atalkidbtype *, boolean, di_type);

extern ulong aurp_eventUpdateInterval;

void
show_appleaurp (parseinfo *csb)
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
    case SHOW_APPLE_AURP_TOPOLOGY:
	aurp_show_topology(GETOBJ(int,1));
	break;
    case SHOW_APPLE_AURP_EVENTS:
	aurp_show_events(GETOBJ(int,1));
	break;
    }
    automore_disable();
}

void
appleaurp_command (parseinfo *csb)
{
    csb->interface = NULL;

    if (csb->nvgen) {
	if (!atalk_running)
	    return;

	switch (csb->which) {
	case AURP_UPDATE:
	    nv_write(aurp_eventUpdateInterval != AURP_DEFAULT_UPDATE_RATE,
		     "%s %d", csb->nv_command,
		     aurp_eventUpdateInterval);
	    break;
	default:
	    printf("\n%% NVGEN: unknown command - %s", csb->nv_command);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case AURP_UPDATE:
	if (csb->sense && GETOBJ(int, 1) % 10 != 0) {
		printf("\n%% Update interval must be multiples of 10");
		break;
	}
	aurp_eventUpdateInterval = csb->sense ? GETOBJ(int, 1) :
	  AURP_DEFAULT_UPDATE_RATE;
	mgd_timer_start(&aurp_update_eventsTimer,
			aurp_eventUpdateInterval * ONESEC);
	break;

    default:
	buginf("\nAT: Unhandled configuration command");
	break;
    }
}	

void
ataurpif_command (parseinfo *csb)
{
    idbtype *idb;
    at_aurp_info *aurp_info;
    atalkidbtype *atalkidb;

    if (!atalk_running) {
	if (!csb->nvgen) {
	    printf(atalk_not_running);
	}
	return;
    }

    idb = csb->interface;

    if (csb->nvgen) {

        atalkidb = atalk_getidb(idb);

	if (!(atalkidb && atalkidb->atalk_enabled && atalk_running)) {
	    return;
	}

	switch (csb->which) {
	case AURP_TICKLE:
	    if (!hwidb_or_null(idb) || !hwidb_or_null(idb)->tunnel)
		return;

	    aurp_info = hwidb_or_null(idb)->tunnel->aurp;
	    if (!aurp_info)
		return;
	    nv_write(aurp_info &&
		     aurp_info->r_tickletime != AURP_DEFAULT_TICKLETIME,
		     "%s %d", csb->nv_command,
		     aurp_info->r_tickletime / ONESEC);
	    break;
	case AURP_RP:
	    nv_write(atalkidb->atalk_rp & ATALK_AURP_ENABLED, "%s",
		     csb->nv_command);
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
    case AURP_TICKLE:
	if (!hwidb_or_null(idb) || !hwidb_or_null(idb)->tunnel)
	    return;

	aurp_info = hwidb_or_null(idb)->tunnel->aurp;
	if (!aurp_info) {
	    printf("\nAURP protocol not configured on interface");
	    return;
	}
	aurp_info->r_tickletime = csb->sense ? GETOBJ(int, 1) * ONESEC :
	    AURP_DEFAULT_TICKLETIME;
	break;

    case AURP_RP:
	    if (csb->sense) {
		if (!aurp_config_tunnel(atalkidb, csb->sense, AURP_IP_DI)) {
		    printf("\n%%Error in configuring AURP on this interface");
		    break;
		}
		atalkidb->atalk_rp |= ATALK_AURP_ENABLED;
	    } else {
		if (!aurp_config_tunnel(atalkidb, csb->sense, AURP_IP_DI)) {
		    printf("\n%%Error in configuring AURP on this interface");
		    break;
		}
		atalkidb->atalk_rp &= ~ATALK_AURP_ENABLED;
	    }
	    break;
	
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

