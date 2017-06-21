/* $Id: at_ppp.c,v 3.3.46.6 1996/08/03 23:19:09 fox Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_ppp.c,v $
 *------------------------------------------------------------------
 * atalk_ppp.c -- handle Appletalk ppp interfaces
 *
 * January 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_ppp.c,v $
 * Revision 3.3.46.6  1996/08/03  23:19:09  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.46.5  1996/07/10  22:18:42  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.46.4  1996/05/01  14:26:14  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.3.46.3  1996/04/29  09:34:44  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.3.46.2  1996/03/23  01:27:15  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.46.1  1996/03/18  18:52:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/13  01:07:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.16.2  1996/03/07  08:29:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  13:33:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  22:27:20  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  08:43:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/22  06:36:44  tkolar
 * CSCdi38902:  ATCP client mode wont work with ISDN
 * Change method of handing out virtual-net node addresses to accomodate
 * arbitrary interfaces, and prevent non P2P interfaces from being
 * configured for Appletalk client mode.
 *
 * Revision 2.2  1995/08/10  00:04:24  satya
 * CSCdi37588:  ATCP printing causes line 1111 emulator crash
 *
 * Revision 2.1  1995/06/07  20:08:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <interface_private.h>
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "atalk_private.h"
#include "at_registry.h"
#include "at_arp.h"
#include "../wan/atcp.h"
#include "at_ppp.h"
#include "../arap/arap_public.h"


idbtype **atalk_ppp_nodes;



/*
 * at_ppp_intercept()
 *
 * Snag appletalk packets destined for the virtual appletalk net.
 *
 */
static void at_ppp_intercept (paktype *pak, boolean *intercepted)
{

    uchar dnode;
    ushort dnet;
    long fullnode;
    int count;
    paktype *pak2;
    idbtype *swidb;
    ushort source_net, source_node = 0;
    atalkidbtype *atalkidb;
    
    dnet = atalk_pak_dnet(pak);
    dnode = atalk_pak_dnode(pak);

    /* if atalk_virtual_zonelen is set then we have an internal network 
	set up */

    if (( dnet == atalk_virtual_net) && atalk_virtual_zonelen) {
	dnode = atalk_pak_dnode(pak);

	if (dnode == AT_PPP_ROUTER_NODE) {
	    /* traffic for the router, probabably an NBP lookup from a
		client */
	    atalk_DDPcksum(pak);
	    atalk_enqueue(pak);
	    *intercepted = TRUE;
	    return;
	}
	if (dnode != ATALK_BROAD_ADDR) {
            if (!atalk_ppp_nodes[dnode]) {
		/* we got a packet for a node that doesn't have an 
			interface asssociated with it */
                datagram_done(pak);
                *intercepted = TRUE;
                return;
            }
	    /* not a broadcast.  Send to the right idb */
	    fullnode = atalk_address(dnet, dnode);
	    pak->if_output = atalk_ppp_nodes[dnode];
	    atalkidb = atalk_getidb(pak->if_output);
	    
	    if (atalkidb && atalkidb->atalk_client_mode) {
	        atalk_send1(pak, fullnode);
	    } else {
		datagram_done(pak);
	    }
	} else {
	    /* broadcast.  Make a copy for every interface and 
		let 'em have it */
	    if ((pak->datagramstart[DDP_TYPE] == DDP_TYPE_NBP) &&
		  (pak->datagramstart[NBP_TYPE] >> 4 == NBP_LkUp)) {
		source_net = pak->datagramstart[NBP_NETHIGH];
		source_net <<= 8;
		source_net |= pak->datagramstart[NBP_NETLOW];

		if (source_net == atalk_virtual_net) {
		    /* record the source_node so that we don't send the
			broadcast to it */
		    source_node = pak->datagramstart[NBP_NODE];
		}
	    }

	    for (count = 0; count < ATALK_PPP_NODES; count++) {
	        swidb = atalk_ppp_nodes[count];
		atalkidb = atalk_getidb(swidb);
		if (atalkidb && atalkidb->atalk_client_mode
		    && atalkidb->atalk_operational && (count != source_node)) {
		    pak2 = pak_duplicate(pak);
		    if (pak2 != NULL) {
			fullnode = atalk_address(dnet,count);
			pak2->if_output = swidb;
			atalk_send1(pak2, fullnode);
		    }
		}
	    }
	    datagram_done(pak);
	}
	*intercepted = TRUE;
	return;
    }
    return;
}


/*
 * at_atcp_info()
 *
 * Set up NCP negotiation info for Appletalk/PPP
 *
 */
void
at_atcp_info (void *wantoptions, void *allowoptions, hwidbtype *idb)
{

    atalkidbtype *swidb;
    int count;
    atcp_options *wo, *ao;
    boolean nodeset = FALSE;

    wo = (atcp_options *) wantoptions;
    ao = (atcp_options *) allowoptions;

    swidb = atalk_getidb(firstsw_or_null(idb));

    if (!swidb || !swidb->atalk_allow_clients) {
        return;
    }

    if (!atalk_virtual_zonelen) {
	/* there's no internal network.  Bail. */
        errmsg(AT_ERR_NOVIRTUAL, idb_get_namestring(swidb->phys_idb));
	return;
    }


    wo->her_address_net = atalk_virtual_net;

    /* check to see if this idb is already a node */
    for (count = 1; count < ATALK_PPP_NODES; count++) {
       if (atalk_ppp_nodes[count] == swidb->phys_idb) {
	   nodeset = TRUE;
	   break;
       }
    }
    /* 
     * step through atalk_ppp_nodes, looking for a node that isn't in use.
     */
    if (!nodeset) {
	for (count = 1; count < ATALK_PPP_NODES; count++) {
	   if (!atalk_ppp_nodes[count]) {
	       atalk_ppp_nodes[count] = swidb->phys_idb;
	       wo->her_address_node = count;
	       break;
	   }
	}
    }

    bcopy(atalk_virtual_zone, wo->zone_name, atalk_virtual_zonelen);
    wo->zone_name_len = atalk_virtual_zonelen;

    wo->default_router_net = atalk_virtual_net;
    wo->default_router_node = AT_PPP_ROUTER_NODE;

    wo->server_class  = 1;
    wo->implementation_id = 42;

    bcopy(hostname, wo->server_name, strlen(hostname));
    wo->server_name_len = strlen(hostname);

    wo->neg_address = 0;	/* don't negotiate atalk address */
    wo->neg_routeprot = 0;	/* don't negotiate routing protocol */
    wo->neg_supbroad = 0;	/* don't negotiate broadcast suppression */
    wo->neg_compprot = 0;	/* don't negotiate compression*/
    wo->neg_server = 1;		/* negotiate server info */
    wo->neg_zone = 1;  		/* negotiate zone info */
    wo->neg_defrouter = 1;	/* negotiate a default router */

    ao->neg_address = 1;	/* negotiate atalk address */
    ao->neg_routeprot = 0;	/* don't negotiate routing protocol */
    ao->neg_supbroad = 0;	/* don't negotiate broadcast suppression */
    ao->neg_compprot = 0;	/* don't negotiate compression*/
    ao->neg_server = 0;		/* don't negotiate server info */
    ao->neg_zone = 0;  		/* don't negotiate zone info */
    ao->neg_defrouter = 0;	/* don't negotiate a default router */
    return;
}

static boolean at_ppp_client_down (idbtype *idb)
{
    int count;
    /* 
     * step through atalk_ppp_nodes, and remove this idb from the array
     */
    for (count = 1; count < ATALK_PPP_NODES; count++) {
       if (atalk_ppp_nodes[count] == idb) {
	   atalk_ppp_nodes[count] = NULL;
       }
    }
    return(TRUE);
}

static boolean at_ppp_client_up (atalkidbtype *idb)
{
    atalk_rdb_t *r = NULL;
    rbTree      *r_tree;

	/* while you weren't looking the appletalk interface magically leapt
		to the operational state */
    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, atalk_virtual_net, FALSE);
    atalkif_addZone(idb, atalk_PrimaryZone(r));
    idb->atalk_cablestart = idb->atalk_cableend = atalk_virtual_net;
    idb->atalknet = atalk_virtual_net;
    idb->atalknode = AT_PPP_ROUTER_NODE;
    idb->atalk_enabled = TRUE;
    idb->atalk_operational = TRUE;
    idb->aarp_tentative = AARP_ADDR_SET;
    idb->atalk_counter = -1;        /* Don't run AT FSM */
    idb->atalk_confaddr = 0;
    idb->atalk_status = opValid;
    idb->atalk_client_mode = TRUE;
    idb->atalk_enctype = LINK_APPLETALK;
    return(TRUE);
}

/*
 * init_atalk_ppp()
 *
 * Set up registries for appletalk/PPP
 *
 */
void init_atalk_ppp (void)
{
    atalk_ppp_nodes = malloc_named(ATALK_PPP_NODES * sizeof(idbtype *),
				   "AT PPP Nodes");
    if (atalk_ppp_nodes == NULL) {
	return;
    }
    reg_add_atalk_intercept(at_ppp_intercept, "at_ppp_intercept");
    reg_add_at_ppp_client_down(at_ppp_client_down, "at_ppp_client_down");
    reg_add_at_ppp_client_up(at_ppp_client_up, "at_ppp_client_up");
}
