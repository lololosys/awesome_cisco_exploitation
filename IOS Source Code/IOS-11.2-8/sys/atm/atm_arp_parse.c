/* $Id: atm_arp_parse.c,v 3.1.4.6 1996/08/28 12:37:07 thille Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_arp_parse.c,v $
 *------------------------------------------------------------------
 * A . T . M . _ . A . R . P . _ . P . A . R . S . E . . C
 *   - Generic atm arp parser support routines
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_arp_parse.c,v $
 * Revision 3.1.4.6  1996/08/28  12:37:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.5  1996/07/30  14:40:47  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.1.4.4  1996/05/21  09:42:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.3  1996/05/09  14:10:53  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.2  1996/05/01  14:44:59  achopra
 * CSCdi52007:  shut down an ATM ARP client on subinterface doesnt clear
 * map entry
 * Branch: California_branch
 *
 * Revision 3.1.4.1.2.1  1996/04/27  06:33:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:01:54  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:43  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../atm/parser_defs_atm.h"
#include "../lane/parser_defs_lane.h"
#include "../wan/serial.h"
#include "address.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../atalk/atalk.h"
#include "../parser/parser_actions.h"
#include "../srt/span.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../if/static_map.h"
#include "../atm/atmsig_api.h"
#include "../atm/sscop_public.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_ie.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_public.h"
#include "../atm/atmsig_smap.h"
#include "../wan/smds.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "fastswitch_registry.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../atm/atm_oam.h"
#include "../atm/atm_arp.h"
#include "../atm/ilmi_public.h"
#include "../atm/atm_registry.h"
#include "../atm/atm_registry.regc"
#include "../llc2/llc2.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"
#include "../lane/lanefast_les.h"

/*
 *   Handle configuration commands for atm arp
 *
 */
void
atm_arp_interface_commands (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    atm_db_t *atm;
    char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
    long delta = 0;

    
    idb = csb->interface;
    if (idb) {
	hwidb = idb->hwptr;
    } else {
	printf("\nerror: source idb not set");
	return;
    }
    atm = hwidb->atm_db;



    if (csb->nvgen) {
	switch(csb->which) {
	case ATM_CMD_1577EXT:
	    /*
	     *  What kind of extenesion to RFC 1577 are we doing?
	     */
	    nv_write(idb->rfc1577_flavor != RFC1577_EXT_NONE,
		     "%s BFI", csb->nv_command);
	    break;
	case ATM_CMD_ARPSERV:
	{
	    atm_arpserv_entry *entry;
	    
	    if (idb->atm_arp != NULL) {
		/*
		 *  First do ourselves, if we are an ARP Server
		 */
		if (idb->atm_arp->arp_flags & ATM_ARP_SERVER) {
		    /* we are an ARP server */
		    if (idb->atm_arp->as_data.server.timeout ==
			ATMARP_SERVERAGE) {
			/* default server timeout */
			nv_write(TRUE, "atm arp-server self");
		    } else {
			nv_write(TRUE, "atm arp-server self timeout %d",
				 idb->atm_arp->as_data.server.timeout /ONEMIN);
		    }
		}

		FOR_ALL_ARP_SERVERS(idb->atm_arp, entry) {
		    /* we are an ARP client */
		    if (idb->nsap != NULL && snpa_equ(idb->nsap, &entry->nsap))
			continue; /* We don't explicitly configure ourselves */
		    atm_printnsap(entry->nsap.addr, nsapstr);
		    nv_write(TRUE, "atm arp-server nsap %s", nsapstr);
		}
	    }
	    break;
	}
	default:
	    bad_parser_subcommand(csb,csb->which);
	    break;
	}
    }
    else {
	switch(csb->which) {
	case ATM_CMD_1577EXT:
	    if (is_atm_lane(idb)) {
		printf("\n%%ATM arp-server conflicts with LANE client.");
		return;
	    }

	    /*
	     *  They want to turn off the extensions.  This is exactly
	     *  identical to the explicit command to return to the vanilla
	     *  mode, so we'll handle them together.
	     */
	    if (!csb->sense || GETOBJ(int,1) == RFC1577_EXT_NONE) {
		/*
		 *  Figuring out what to do if we have multiple servers
		 *  defined and we want to drop back to just having one
		 *  is more work than I care to do.  So I'll just disallow
		 *  the transition.  Hey, God did it for quarks!
		 */
		if (idb->rfc1577_flavor == RFC1577_EXT_BFI &&
		    idb->atm_arp != NULL &&
		    idb->atm_arp->as_data.client.arp_serv_list != NULL &&
		    idb->atm_arp->as_data.client.arp_serv_list->next != NULL){
		    printf("\nPlease deconfigure all but one ARP Server before returning to standard operation");
		    break;
		}
		idb->rfc1577_flavor = RFC1577_EXT_NONE;
		break;
	    }
	    /*
	     *  If we get here, we are changing to a non-vanilla service.  At
	     *  present, there is only one.  This will probably have to change
	     *  fairly soon to a switch, but the above test for multiple
	     *  configured servers might need to change also.  Might as well
	     *  keep it simple for now.
	     */
	    idb->rfc1577_flavor = RFC1577_EXT_BFI;
	    break;
	case ATM_CMD_ARPSERV:
	    if (!csb->sense) {
		/*
		 *  Turn off the ARP Server stuff if it is on
		 */
		if (idb->atm_arp == NULL)
		    break;	/* no-op: it was already off */
		if (GETOBJ(int,1)) {
		    /* they want to turn off the local ARP Server */
		    if (idb->atm_arp->arp_flags & ATM_ARP_SERVER)
			atm_arp_serv_off(idb);
		    else
			printf("\n%%%s is not functioning as an ATM ARP Server"
			       , idb->namestring);
		} else {
		    /* they want to turn off one of the clients */
		    atm_arp_client_off(idb, GETOBJ(hwaddr,1));
		}

		/*
		 *  If that was our last server, free up the ATM ARP
		 *  structure
		 */
		if (!(idb->atm_arp->arp_flags & ATM_ARP_SERVER) &&
		    idb->atm_arp->as_data.client.arp_serv_list == NULL) {
		    free(idb->atm_arp);
		    idb->atm_arp = NULL;
		}
		break;
	    }
	    if (is_atm_lane(idb)) {
		printf("\n%%ATM arp-server conflicts with LANE client.");
		return;
	    }
	    /*
	     * They are turning this on.  OBJ(int,1) reflects whether this
	     * is a client or a server.  First, if you can only have one
	     * client active at a time, we must turn off the current one.
	     *
	     * But, because people occasionally make mistakes, we are going
	     * to explicitly test to see if we are just turning ourselves
	     * into an ARP Server when we already are one.  If we don't
	     * special-case this, we will forget about all our clients when
	     * we bounce.  Anyone specifically wanting this will have to
	     * do an explicit deconfig and reconfig.
	     */
	    if (is_atm_arp_server(idb) && GETOBJ(int,1))
		break;
	    if (idb->rfc1577_flavor == RFC1577_EXT_NONE &&
		idb->atm_arp != NULL &&
		idb->atm_arp->as_data.client.arp_serv_list != NULL) {
		if (idb->atm_arp->arp_flags & ATM_ARP_SERVER) {
		    atm_arp_serv_off(idb);
		} else {
		    atm_arp_client_off(idb, &idb->atm_arp->as_data.client.arp_serv_list->nsap);
		}
	    }

	    /*
	     * Use the default value of maxmtu_def, 
	     * if there is no user configured MTU.
	     */
	    if (idb->user_configured_mtu) {
		/* 
		 * no need to force a re-carve of buffer. Use the sub-if
		 * MTU, as set at the interface creation time.
		 */
		delta = 0;
	    } else {
		delta = hwidb->maxmtu_def - idb->sub_mtu;
		idb->sub_mtu = hwidb->maxmtu_def;
	    }

	    /*
	     * Set the maximum mtu on the hwidb, based on all the sub-mtu
	     * values in the sub-interfaces.
	     */
	    set_max_interface_mtu(idb, delta);

	    if (GETOBJ(int,1)) {
		/* It is a server.  OBJ(int,2) is the timeout time */
		atm_arp_serv_init(idb, GETOBJ(int,2) * ONEMIN);
	    } else {
		/* It is a client.  OBJ(hwaddr,1) is the NSAP */
		atm_arp_client_init(idb, GETOBJ(hwaddr,1));
	    }
	    atm_arp_build_maps(idb, NULL); /* Never too early to build maps */
	    break;
	default:
	    bad_parser_subcommand(csb,csb->which);
	    break;
	}
    }
}


/*
 * 
 *
 * Support atm arp specific parser commands
 * 
 *
 */
void
atm_show_arp_commands (parseinfo *csb)
{
    switch(csb->which) {
    case SHOW_ATM_ARPSERVER:
	atm_arp_serv_show(GETOBJ(idb,1));
	break;
    default:
	break;
    }
}
      
