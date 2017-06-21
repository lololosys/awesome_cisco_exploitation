/* $Id: vines_support.c,v 3.11.18.8 1996/08/07 09:04:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_support.c,v $
 *------------------------------------------------------------------
 * vines_support.c -- Banyan Vines miscellaneous support routines
 *
 * February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_support.c,v $
 * Revision 3.11.18.8  1996/08/07  09:04:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.18.7  1996/06/17  23:38:55  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.18.6  1996/06/04  01:47:00  ehk
 * CSCdi57934:  VINES: LAN Extender will not work with VINES.
 * Branch: California_branch
 * set default vines encapsulation to ARPA on LEX port and
 * made it user configurable.
 *
 * Revision 3.11.18.5  1996/05/21  10:08:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.18.4  1996/04/23  20:30:59  ehk
 * CSCdi55230:  Vines decimal disables vines process after reloading the
 * router
 * Branch: California_branch
 * need to consider "vines decimal" in addition to "vines routing"
 * when disallowing UI configuration (a byproduct of CSCdi52009 bug fix).
 *
 * Revision 3.11.18.3  1996/03/28  23:04:02  ehk
 * CSCdi52665:  %SYS-3-MGDTIMER: Running timer, init, timer = 9E53D4
 * Branch: California_branch
 * stop various timers in a vines IDB when the interface is disabled.
 *
 * Revision 3.11.18.2  1996/03/28  22:42:31  ehk
 * CSCdi51823:  Vines recompute does not recompute.
 * Branch: California_branch
 * take all interfaces into account when computing vines network
 * number (CSCdi51823). if no interface MAC address is available,
 * use the system default MAC addres (CSCdi52195). to be consistent
 * with other protocols, disallow vines UI configuration unless
 * vines routing is enabled (CSCdi52009).
 *
 * Revision 3.11.18.1  1996/03/18  22:32:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/13  02:05:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.2  1996/03/07  11:05:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  21:47:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/07  16:16:50  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.10  1996/02/01  06:11:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/29  07:30:57  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.8  1996/01/22  07:45:01  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1995/12/22  21:58:58  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.6  1995/12/17  18:40:47  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/14  23:47:54  slin
 * CSCdi45347:  Clear vine rout * clears the routes for ever
 * - Reset router sequence number so that we can relearn routes from SRTP
 *   neighbors
 *
 * Revision 3.4  1995/12/01  21:39:29  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/17  17:59:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/22  00:53:46  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.5  1995/08/17  21:35:31  slin
 * CSCdi38846:  show vines int can halt system
 *
 * Revision 2.4  1995/07/07  20:40:44  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.3  1995/06/23 18:48:58  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/13  22:10:38  slin
 * CSCdi35727:  no vines routing when Vines not running causes traceback
 * - Kill Vines processes only if they are running
 *
 * Revision 2.1  1995/06/07  23:15:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "ttysrv.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_vines.h"
#include "../if/network.h"
#include "../iprouting/route.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "vinesfast.h"
#include "logger.h"


/*
 * variables
 */
queuetype vines_nameQ;
char *const vines_ip_types[] = {"IP", "IPC", "SPP", "3", "ARP", "RTP", "ICP"};

/*
 * Global variables
 */
boolean vines_decimal_addresses;	/* Show addresses in decimal or hex */
char vines_hostname[VST_DOMAINLEN];


/*
 * External declarations.
 */
extern vinesidbtype *vines_deleted_idbs;

/**********************************************************************
 *
 *			GLOBAL COMMAND PARSING
 *
 **********************************************************************/

static char const vines_badaddr[] = "\n%%Illegal VINES address - %s";
static char const vines_questionable_addr[] = "
%% This VINES network number is in the Cisco address space, but
appears to have been generated by a different Cisco router.  Please
reissue the 'vines routing' command again and choose a different
address, or use the 'recompute' keyword.";

/*
 * vines_routing
 *
 * Common code for enabling/disabling vines.
 */
void vines_routing (boolean on)
{
    vinesidbtype *idb;
    idbtype *swidb;

    vines_running = on;
    FOR_ALL_SWIDBS(swidb)
	vines_rxtypes_update(swidb);
    vsvc_stop_services();
    if (on) {
	vsvc_start_services();
    }

    /*
     * The following sets up the per interface timers for VINES.  It also
     * handles enabling or disabling VINES fast switching when VINES
     * routing is enabled or disabled (needed for bridging).
     */
    FOR_ALL_VIDBS(idb) {
	if (on) {
	    mgd_timer_start(&idb->wakeup_timer, 0);
	    if (vines_is_wan(idb))
		mgd_timer_start(&idb->query_timer, 0);
	} else {
	    mgd_timer_stop(&idb->wakeup_timer);
	    mgd_timer_stop(&idb->query_timer);
	}
	vines_fastsetup(idb->phys_idb->hwptr);
    }

    /*
     * Kick all the processes so that they will exit.
     */
    if (!on) {
	if (vines_pid)
	    process_kill(vines_pid);
	if (vines_bg_pid)
	    process_kill(vines_bg_pid);
	if (vines_rtr_pid)
	    process_kill(vines_rtr_pid);
    }
}

/*
 * vroute_write_static 
 *
 * Helper routine for writing out static route entries.
 */
static boolean vroute_write_static (treeLink *link, void *dummy)
{
    vinesnettype *server;
    vinesroutetype *route;
    vroute_fn_data *data;

    server = vrouteCast(link);
    data = (vroute_fn_data *)dummy;
    FOR_ALL_ROUTES(server, route) {
	if (!(route->flags & V_NET_STATIC))
	    continue;
	nv_write(TRUE, "%s %Z %z %s", data->string, server->net,
		 route->gw->net, route->gw->host,
		 vmetric_to_config(route->metric));
    }
    return(FALSE);
}

/*
 * vines_command
 *
 * Process all VINES global commands.
 */
void vines_command (parseinfo *csb)
{
    ulong net, dstnet;
    ushort host;
    vinesnametype *namep;
    char *name;
    vinesnettype *server;
    vinesroutetype *route;
    vinesneighbortype *gw;
    vinespathtype *path;
    leveltype level;
    int i, metric;
    vroute_fn_data data;

    if (csb->nvgen) {
	vinesacctype *item;
	char buffer[100];

	if (!vines_running)
	    return;
	switch (csb->which) {
	  case VINES_ACCESSLIST:
	    for (i = 1; i <= VINESMAXACC; i++) {
		for (item = vines_access_lists[i]; item; item = item->next) {
		    /*
		     * Temporary patch to prevent router crash due to memory
		     * corruption (CSCdi49737). Get rid of validmem() once
		     * the cause is identified.
		     */
		    if (!validmem(item))
			break;
		    vines_access_str(buffer, i, item);
		    nv_write(TRUE, "%s %3d %s ", csb->nv_command, i, buffer);
		}
	    }
	    break;
	  case VINES_DECIMAL:
	    nv_write(vines_decimal_addresses, "%s", csb->nv_command);
	    break;
	  case VINES_ENHANCEMENTS:
	    if (waste_bandwidth_because_banyan_whines)
		nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	  case VINES_HOST:
	    namep = (vinesnametype *)vines_nameQ.qhead;
	    for ( ; namep; namep = namep->next) {
		nv_write(vines_running, "%s %s %z", csb->nv_command,
			 namep->name, namep->net, namep->host);
	    }
	    break;
	  case VINES_HOSTNAME:
	    nv_write(strlen(vines_hostname), "%s %s", csb->nv_command,
		    vines_hostname);
	    break;
	  case VINES_ROUTING:
	    nv_write(vines_running, "%s %z", csb->nv_command,
		     vines_network, vines_host);
	    break;
	  case VINES_ROUTE:
	    data.string = csb->nv_command;
	    vroute_WalkTree(vroute_write_static, &data);
	    break;
	  case VINES_SINGLE:
	    nv_write(vines_onlyone, "%s", csb->nv_command);
	    break;
	  case VINES_SRTP_ENABLED:
	    nv_write(vsrtp_enabled || mgd_timer_running(&vsrtp_start_timer),
		     "%s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!vines_running && 
	(csb->which != VINES_ROUTING && csb->which != VINES_DECIMAL)) {
	printf("\n%% VINES not running");
	return;
    }

    switch (csb->which) {
      case VINES_ACCESSLIST:
	vines_access_command(csb);
	break;
      case VINES_DECIMAL:
	vines_decimal_addresses = csb->sense;
	break;
      case VINES_ENHANCEMENTS:
	waste_bandwidth_because_banyan_whines =
	    !(csb->set_to_default || csb->sense);
	break;
      case VINES_HOST:
	if (!GETOBJ(int,9) && !GETOBJ(int,10)) {
	    if (!csb->sense) {
		while ((namep = dequeue(&vines_nameQ)) != NULL)
                    free(namep);
	    }
	    return;
	}
	if (csb->sense) {
	    name = NULL;
	    setstring(&name, GETOBJ(string,1));
	    net = GETOBJ(paddr,1)->vines_addr.net;
	    host = GETOBJ(paddr,1)->vines_addr.host;
	    vines_name_add(name, net, host);
	    free(name);
	} else {
	    vines_name_delete(GETOBJ(string,1));
	}
	break;
      case VINES_HOSTNAME:
	if (csb->sense) {
	    if (strlen(GETOBJ(string,1)) >= VST_DOMAINLEN) {
		printf("\n%% VINES hostname must be less than %d characters.",
		       VST_DOMAINLEN);
		break;
	    }
	    sstrncpy(vines_hostname, GETOBJ(string,1), VST_DOMAINLEN);
	} else {
	    vines_hostname[0] = '\0';
	}
	vsvc_hostname_changed(TRUE);
	break;
      case VINES_ROUTING:
	if (csb->sense) {
	    if (GETOBJ(int,1)) {
		net = GETOBJ(paddr,1)->vines_addr.net;
		host = GETOBJ(paddr,1)->vines_addr.host;
		if (!net || !host) {
		    printf("\n%% VINES net and host must be non-zero");
		    return;
		}
		if (net < VINES_NON_BANYAN) {
		    printf("\n%% VINES network numbers below %Z are reserved for servers.",
			   VINES_NON_BANYAN);
		    return;
		}
		if (host != 1) {
		    printf(vines_badaddr, "router must be host 1");
		    return;
		}
	    } else if (GETOBJ(int,2)) {
		/*
		 * Explicitly told to recompute the VINES address based
		 * upon the current interface addresses.
		 */
		net = vines_pick_default_net();
		host = VINES_SERVER;
	    } else {
		/*
		 * No address was specified.  If we have run VINES
		 * before, then we will use that previous address.  If
		 * VINES has never been run, then look for the first
		 * non-zero MAC address of an IEEE style interface, and
		 * modify it to map into the Banyan number space
		 * allocated to Cisco.
		 */
		if (vines_network && vines_host) {
		    net = vines_network;
		    host = vines_host;
		} else {
		    net = vines_pick_default_net();
		    host = VINES_SERVER;
		}
		if (net != vines_network) {
		    vines_client = VINES_1ST_CLIENT;
		}
	    }

	    /*
	     * Validate the network number.
	     */
	    if ((net & VINES_NET_MFG) == VINES_NET_CISCO) {
		if (!vines_validate_net(net)) {
		    printf(vines_questionable_addr);
		}
	    }

	    /*
	     * Change the address
	     */
	    vines_ager(TRUE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    vines_delete_server(vines_network);
	    vines_delete_neighbor(vines_network, vines_host);
	    vines_network = net;
	    vines_host = host;
	    gw = vines_add_neighbor(net, host, V_N_ME, 0, VSRTP_VERSION,
				    VSRTP_NODE_ROUTER);
	    vines_add_path(gw, V_N_ME, vinesnullidb, ET_NULL, NULL, 0,
			   VSRTP_VERSION, VSRTP_NODE_ROUTER);
	    server = vines_add_server(net, V_NET_ME);
	    vines_add_route(server, gw, V_NET_ME, 0);
	    reset_interrupt_level(level);

	    vines_start();
	    vines_routing(TRUE);
	    if (vsrtp_enabled)
		vsrtp_schedule_start();
	    mgd_timer_start(&vines_periodic_timer, VRTP_DEF_WAKEUP_INTERVAL);
	} else {
	    vines_routing(FALSE);
	    vines_poison_all_paths_by_idb(NULL);
	    vroute_send_full_updates(FALSE);
	    vines_ager(TRUE);
	    mgd_timer_stop(&vines_periodic_timer);
	}
	break;

      case VINES_ROUTE:
	dstnet = GETOBJ(paddr,1)->vines_addr.net;
	net = GETOBJ(paddr,2)->vines_addr.net;
	host = GETOBJ(paddr,2)->vines_addr.host;
	metric = vmetric_from_config(GETOBJ(int,2), GETOBJ(int,3));
	if ((dstnet == 0) || (dstnet == -1) || (dstnet == vines_network)) {
	    printf("\n%%Bad VINES network number - %s", csb->line);
	    return;
	}
	if ((net == vines_network) || (host != 1)) {
	    printf(vines_badaddr, "cannot route to that station");
	    return;
	}
	
	if (!csb->sense) {
	    server = vroute_FindEntry(dstnet);
	    gw = vneigh_FindEntry(net, host);
	    path = vines_get_path_by_xxx(gw, V_N_PLACEHOLDER, NULL, ET_NULL, NULL);

	    server = vines_delete_route_by_xxx(server, gw, V_NET_STATIC, 0,
					       timer_no_time);
	    if (path && --path->uses == 0)
		gw = vines_flush_paths_by_flag(gw, V_N_PLACEHOLDER, NULL);
	    break;
	}
	
	gw = vines_add_neighbor(net, host, V_N_STATIC, VINES_INFINITY,
				VRTP_VERSION, VRTP_NODE_ROUTER);
	path = vines_add_path(gw, V_N_PLACEHOLDER, vinesnullidb, ET_NULL, NULL,
			      VINES_INFINITY, VRTP_VERSION, VRTP_NODE_ROUTER);
	server = vines_add_server(dstnet, V_N_STATIC);
	route = vines_add_route(server, gw, V_NET_STATIC, metric);
	if (!gw || !path || !server || !route) {
	    printf("\n%%Cannot allocate memory for static route to %z", dstnet);
	    server = vines_delete_route_by_xxx(server, gw, V_NET_STATIC, 0,
					       timer_no_time);
    	    gw = vines_flush_paths_by_flag(gw, V_N_PLACEHOLDER, NULL);
	    break;
	}
	path->uses++;
	break;

      case VINES_SINGLE:
	vines_onlyone = csb->sense;
	break;

      case VINES_SRTP_ENABLED:
	if (csb->sense)
	    vsrtp_schedule_start();
	else
	    vsrtp_stop();
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/**********************************************************************
 *
 *		      INTERFACE COMMAND PARSING
 *
 **********************************************************************/

/*
 * vines_set_wakeup_time
 *
 * When an update interval is set for an interface, look for another
 * interface using this same time interval and synchronize them to the
 * same update time.  This will cause all interfaces using a 90 second
 * interval to send updates at the same time, all interfaces using a 60
 * second interval to send at the same time, etc.  This isn't required
 * for anything, just seemed like a nice enhancement.  Without this all
 * the interfaces send updates at slightly different times.
 */
void vines_set_wakeup_time (vinesidbtype *target)
{
    vinesidbtype *idb;
    sys_timestamp exp_time;

    if (target->vines_wakeup_interval == VRTP_DEF_WAKEUP_INTERVAL) {
	exp_time = mgd_timer_exp_time(&vines_periodic_timer);
	mgd_timer_set_exptime(&target->wakeup_timer, &exp_time);
	return;
    }

    FOR_ALL_VIDBS(idb) {
	if ((idb == target) || !idb->vines_metric || !vines_interface_up(idb) ||
	    !mgd_timer_running(&idb->wakeup_timer))
	    continue;
	if (idb->vines_wakeup_interval == target->vines_wakeup_interval) {
	    exp_time = mgd_timer_exp_time(&idb->wakeup_timer);
	    mgd_timer_set_exptime(&target->wakeup_timer,&exp_time);
	    return;
	}
    }
    mgd_timer_start(&target->wakeup_timer, target->vines_wakeup_interval);
}

/*
 * vines_compute_max_wakeup
 */
static void vines_compute_max_wakeup (void)
{
    vinesidbtype *idb;
    ulong max_wakeup = 0;

    FOR_ALL_VIDBS(idb) {
	if (!idb->vines_metric || !vines_interface_up(idb))
	    continue;
	max_wakeup = max(max_wakeup, idb->vines_wakeup_interval);
    }
    vines_slowest_update = max_wakeup;
}

/*
 * vines_bandwidth_changed
 *
 * Notification that the bandwidth of an interface has changed.  See
 * if the metric for this interface needs to be recomputed.
 */
void vines_bandwidth_changed (idbtype  *swidb)
{
    vinesidbtype *idb;

    idb = vines_getidb(swidb);
    if ((idb == NULL) || (idb->vines_metric == 0) || (!idb->metric_is_default))
	return;
    
    idb->vines_metric = vines_compute_default_metric(swidb);
}

/*
 * vneigh_write_static 
 *
 * Helper routine for writing out static neighbor entries.
 */
static boolean vneigh_write_static (treeLink *link, void *dummy)
{
    vinesneighbortype *neighbor;
    vinespathtype *path;
    vneigh_fn_data *data;

    neighbor = vneighCast(link);
    data = (vneigh_fn_data *)dummy;

    FOR_ALL_PATHS(neighbor, path) {
	if (!(path->flags & V_N_STATIC))
	    continue;
	if (data->idb && (path->idb != data->idb))
	    continue;
      nv_write(TRUE, "%s %z", data->string, neighbor->net, neighbor->host); 
      if ((path->enctype != ET_HDLC) && (path->enctype != ET_PPP)) { 
          nv_add(TRUE, " %e %s", path->dlc_addr.addr,
		 encstring(path->enctype));
      } 
      nv_add(TRUE, " %s", vmetric_to_config(path->metric)); 
    }
    return(FALSE);
}

/*
 * vinesif_command
 *
 * Process all VINES interface specific configuration commands.
 */
void vinesif_command (parseinfo *csb)
{
    idbtype *phys_idb;
    vinesidbtype *idb, *tmp;
    int which, metric, default_metric;
    ulong net, status, def_value;
    ushort host;
    vinesneighbortype *gw;
    vinespathtype *path;
    hwidbtype *hwidb;
    uchar node_type, node_flags;
    vneigh_fn_data data;

    phys_idb = csb->interface;
    if (csb->nvgen) {
	/*
	 * If vines not running in the router, never configured on
	 * this interface, or is simply not running on this interface,
	 * then bail out now.
	 */
	idb = vines_getidb(phys_idb);
	if (!vines_running || idb == NULL)
	    return;
	if (idb->vines_metric == 0L)
	    return;
	hwidb = phys_idb->hwptr;

	switch (csb->which) {
	  case VINES_ACCESSGROUP:
	    nv_write(idb->vines_accesslist != 0,
		     "%s %d", csb->nv_command,
		     idb->vines_accesslist);
	    break;

	  case VINES_COST:
	    nv_write((!idb->deletion_pending && idb->vines_metric),
		     "%s %s", csb->nv_command,
		     vmetric_to_config(idb->vines_metric));
	    break;

	  case VINES_ENCAPS:
	    status = hwidb->status;
	    if ((((idb_is_ether_like(phys_idb))) && (idb->vines_enctype != ET_ARPA)) ||
		((status & IDB_TR)    && (idb->vines_enctype != ET_VINES_TR))||
		((status & IDB_FDDI)  && (idb->vines_enctype != ET_SNAP))) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 encstring(idb->vines_enctype));
	    }
	    break;

	  case VINES_NEIGHBOR:
	    data.idb = idb;
	    data.string = csb->nv_command;
	    vneigh_WalkTree(vneigh_write_static, &data);
	    break;

	  case VINES_NET_FILTER_IN:
	    nv_write(idb->input_network_filter != 0,
		     "%s %d", csb->nv_command, idb->input_network_filter);
	    break;

	  case VINES_NET_FILTER_OUT:
	    nv_write(idb->output_network_filter != 0,
		     "%s %d", csb->nv_command, idb->output_network_filter);
	    break;

	  case VINES_SVC_CFG_LINE:
	    nv_write(idb->svc_line, "%s %d", csb->nv_command, idb->svc_line);
	    break;

	  case VINES_SVC_CFG_SLOT:
	    nv_write(idb->svc_slot, "%s %d", csb->nv_command, idb->svc_slot);
	    break;

	  case VINES_PROPAGATE:
	    switch (idb->propagate) {
	      case VPROP_NEVER:
		nv_write(TRUE, "no %s", csb->nv_command);
		break;
	      case VPROP_ALWAYS:
		nv_write(TRUE, "%s", csb->nv_command);
		break;
	      case VPROP_DYNAMIC:
		break;
	    }
	    break;

	  case VINES_REDIRECTCMD:
	    def_value = idb->vines_split_disabled ? 0 : ONESEC;
	    if (idb->vines_redirect_interval != def_value) {
		nv_write(TRUE, "%s %d", csb->nv_command,
			 idb->vines_redirect_interval);
	    }
	    break;

	  case VINES_ROUTEC:
	    if (reg_invoke_fast_supported(LINK_VINES, hwidb))
		nv_write(!hwidb->vines_routecache, "no %s", csb->nv_command );
	    break;

	  case VINES_ROUTER_FILTER_IN:
	    nv_write(idb->input_router_filter != 0,
		     "%s %d", csb->nv_command, idb->input_router_filter);
	    break;

	  case VINES_SERVERLESS:
	    switch (idb->serverless) {
	      case VSVLESS_NEVER:
		nv_write(TRUE, "no %s", csb->nv_command);
		break;
	      case VSVLESS_ALWAYS:
		nv_write(TRUE, "%s", csb->nv_command);
		break;
	      case VSVLESS_ALWAYS_BCAST:
		nv_write(TRUE, "%s broadcast", csb->nv_command);
		break;
	      case VSVLESS_DYNAMIC:
		break;
	    }
	    break;

	  case VINES_SPLIT:
	    nv_write(idb->vines_split_disabled, "no %s", csb->nv_command);
	    break;

	  case VINES_UPDATEDELTAS:
	    nv_write(idb->vines_update_delta_only, "%s", csb->nv_command, 0);
	    break;

	  case VINES_UPDATEINTERVAL:
	    nv_write(idb->vines_wakeup_interval != VRTP_DEF_WAKEUP_INTERVAL,
		     "%s %d", csb->nv_command,
		     idb->vines_wakeup_interval/ONESEC);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!vines_running) {
	printf("\n%% Enable VINES routing first");
	return;
    }

    /*
     * Need to insure that the vines idb information for this specific
     * interface exists.  Starting with the 10.2 release, these are
     * created upon demand, instead of being created at
     * initialization.  Why waste the memory if you never use it.
     */
    if (vines_getidb(phys_idb) == NULL) {
	vines_init_idb(phys_idb);
	if (vines_getidb(phys_idb) == NULL) {
	    printf("\n%% Cannot create data structures for vines interface information.");
	    return;
	}
	vines_rxtypes_update(phys_idb);
    }
    idb = vines_getidb(phys_idb);
    hwidb = phys_idb->hwptr;

    switch (csb->which) {
      case VINES_ACCESSGROUP:
	if (csb->sense) {
	    idb->vines_accesslist = GETOBJ(int,1);
	} else {
	    idb->vines_accesslist = 0;
	}
	break;

      case VINES_COST:
	if (csb->sense) {
	    if (idb->deletion_pending) {
		printf("%%VINES interface is being brought down. Please try again in a few moments.");
		return;
	    }
	    default_metric = vines_compute_default_metric(phys_idb);
	    if (GETOBJ(int,3)) {
		idb->vines_metric = vmetric_from_config(GETOBJ(int,1), GETOBJ(int,2));
	    } else {
		idb->vines_metric = default_metric;
	    }
	    if (!idb->vines_metric)
		idb->vines_metric = 1;
	    idb->metric_is_default = (idb->vines_metric == default_metric);

	    /*
	     * Set up all the interface variables according to the
	     * interface state as of this moment.  If/when the interface
	     * changes state later, vines_route_adjust will be called and
	     * will fix up everything for the new state.
	     */
	    if (interface_up(idb->phys_idb)) {
		idb->vines_lineup = TRUE;
		vroute_interface_came_up(idb);
	    } else {
		idb->vines_lineup = FALSE;
		vroute_interface_went_down(idb);
	    }
	} else {
	    vines_delete_if(idb);
	}
	vines_fastsetup(hwidb);
	varp_notify(idb);
	vines_rxtypes_update(phys_idb);
	break;

      case VINES_ENCAPS:
	status = idb_is_ieeelike(phys_idb);
	if (!status) {
            printf("\n%%VINES encapsulation may be specified only for 802.x interfaces");
            return;
        }
	if (!csb->sense || (GETOBJ(int,1) == ET_NULL)) {
	    /* Set default encapsulation for this interface type */
	    if (idb_is_ether_like(phys_idb)) {
		which = ET_ARPA;
	    } else if (idb_is_tokenring(phys_idb)) {
		which = ET_VINES_TR;
	    } else {
		which = ET_SNAP;
	    }	    
	} else {
	    which = GETOBJ(int,1);
	} 
        if (((idb_is_ether_like(phys_idb)) && which == ET_ARPA) ||
            ((idb_is_tokenring(phys_idb)) && which == ET_VINES_TR) ||
            which == ET_SNAP) {
            idb->vines_enctype = which;
        } else {
            printf("\n%%Bad VINES encapsulation type");
	}
        break;

      case VINES_NEIGHBOR:
	net = (*GETOBJ(paddr,1)).vines_addr.net;
	host = (*GETOBJ(paddr,1)).vines_addr.host;
	if (GETOBJ(int,4)) {
	    metric = vmetric_from_config(GETOBJ(int,2), GETOBJ(int,3));
	} else {
	    metric = idb->vines_metric;
	} 
	/* Pre-fill data for HDLC/PPP links */ 
	if (GETOBJ(int,5) != 0) { 
	    GETOBJ(hwaddr,1)->type = GETOBJ(int,5); 
	    GETOBJ(hwaddr,1)->target_type = TARGET_UNICAST; 
	    GETOBJ(hwaddr,1)->length = 0; 
	}
	if (csb->sense) {
	    boolean panic = GETOBJ(int,9);

	    node_type = (host == VINES_SERVER) ? VSRTP_NODE_ROUTER : VSRTP_NODE_CLIENT;
	    node_flags = (panic) ? V_N_DYNAMIC : V_N_STATIC;
	    gw = vines_add_neighbor(net,host,node_flags,metric,VRTP_VERSION,node_type);
	    if (!gw) {
		printf("\n%%Cannot add neighbor entry for %z", net, host);
		break;
	    }
	    path = vines_add_path(gw, node_flags, idb, GETOBJ(int,1),
				GETOBJ(hwaddr,1), metric,VRTP_VERSION,node_type);
	    if (path) {
		path->controller = VRTP_CONT_MULTI;
		path->machine = VRTP_MACH_FAST;
	    } else {
		printf("\n%%Cannot add path entry for %z", net, host);
	    }
	    if (panic)
		vroute_all_use_neighbor(gw);
	} else {
	    gw = vneigh_FindEntry(net, host);
	    gw = vines_delete_path(gw, V_N_STATIC, idb,
				   GETOBJ(int,1), GETOBJ(hwaddr,1));
	}
	break;

      case VINES_SVC_CFG_LINE:
	if (!vines_is_wan(idb)) {
	    printf("\n%%This command may only be used on WAN interfaces.");
	    break;
	}
	if (csb->sense == FALSE) {
	    idb->svc_slot = 0;
	    idb->svc_line = 0;
	    break;
	}
	FOR_ALL_VIDBS(tmp) {
	    if ((tmp == idb) || (tmp->svc_line == 0) || !vines_is_wan(tmp))
		continue;
	    if (tmp->svc_line == GETOBJ(int,1)) {
		printf("\n%%Line number %d already in use on interface %s.",
		       GETOBJ(int,1), tmp->phys_idb->namestring);
		break;
	    }
	}
	idb->svc_slot = VSVC_MAX_SLOT;
	idb->svc_line = GETOBJ(int,1);
	break;
	
      case VINES_SVC_CFG_SLOT:
	if (vines_is_wan(idb)) {
	    printf("\n%%This command may only be used on LAN interfaces.");
	    break;
	}
	if (csb->sense == FALSE) {
	    idb->svc_slot =  0;
	    break;
	}
	if (GETOBJ(int,1) == VSVC_MAX_SLOT) {
	    printf("\n%%Slot %d is reserved for WAN interfaces.", VSVC_MAX_SLOT);
	    break;
	}
	FOR_ALL_VIDBS(tmp) {
	    if ((tmp == idb) || (tmp->svc_slot == 0) || vines_is_wan(tmp))
		continue;
	    if (tmp->svc_slot == GETOBJ(int,1)) {
		printf("\n%%Slot number %d already in use on interface %s.",
		       GETOBJ(int,1), tmp->phys_idb->namestring);
		break;
	    }
	}
	idb->svc_slot = GETOBJ(int,1);
	break;

      case VINES_PROPAGATE:
	if (csb->set_to_default)
	    idb->propagate = VPROP_DYNAMIC;
	else if (!csb->sense) {
	    if (GETOBJ(int,1)) {
		idb->propagate = VPROP_ALWAYS;
	    } else {
		idb->propagate = VPROP_NEVER;
	    }
	} else {
	    if (GETOBJ(int,1)) {
		idb->propagate = VPROP_DYNAMIC;
	    } else {
		idb->propagate = VPROP_ALWAYS;
	    }
	}
	break;

      case VINES_NET_FILTER_IN:
	if (csb->sense) {
	    idb->input_network_filter = GETOBJ(int,1);
	} else {
	    idb->input_network_filter = 0;
	}
	break;

      case VINES_NET_FILTER_OUT:
	if (csb->sense) {
	    idb->output_network_filter = GETOBJ(int,1);
	} else {
	    idb->output_network_filter = 0;
	}
	break;

      case VINES_REDIRECTCMD:
	def_value = idb->vines_split_disabled ? 0 : ONESEC;
	if (csb->sense) {
	    idb->vines_redirect_interval = GETOBJ(int,1);
	} else {
	    idb->vines_redirect_interval = def_value;
	}
	break;

      case VINES_ROUTEC:
	hwidb->vines_routecache = (csb->set_to_default || csb->sense) &&
	    reg_invoke_fast_supported(LINK_VINES, hwidb);
	vines_fastsetup(hwidb);
	break;

      case VINES_ROUTER_FILTER_IN:
	if (csb->sense) {
	    idb->input_router_filter = GETOBJ(int,1);
	} else {
	    idb->input_router_filter = 0;
	}
	break;

      case VINES_SERVERLESS:
	if (csb->set_to_default)
	    idb->serverless = VSVLESS_DYNAMIC;
	else if (!csb->sense) {
	    if (GETOBJ(int,1) || GETOBJ(int,2)) {
		idb->serverless = VSVLESS_ALWAYS;
	    } else {
		idb->serverless = VSVLESS_NEVER;
	    }
	} else {
	    if (GETOBJ(int,1)) {
		idb->serverless = VSVLESS_DYNAMIC;
	    } else if (GETOBJ(int,2)) {
		idb->serverless = VSVLESS_ALWAYS_BCAST;
	    } else {
		idb->serverless = VSVLESS_ALWAYS;
	    }
	}
	break;

      case VINES_SPLIT:
	if (csb->set_to_default || csb->sense) {
	    idb->vines_split_disabled = FALSE;
	    if (idb->vines_redirect_interval == 0)
		idb->vines_redirect_interval = ONESEC;
	} else {
	    idb->vines_split_disabled = TRUE;
	    if (idb->vines_redirect_interval == ONESEC)
		idb->vines_redirect_interval = 0;
	}
	break;

      case VINES_UPDATEDELTAS:
	idb->vines_update_delta_only = csb->sense;
	idb->vines_update_count = VRTP_DEF_UPDATE_COUNT;
	vines_set_wakeup_time(idb);
	break;

      case VINES_UPDATEINTERVAL:
	if (csb->sense && GETOBJ(int,2)) {
	    GETOBJ(int,1) = ((GETOBJ(int,1) + 4) / 5) * 5; /* round */
	    if (GETOBJ(int,1)) {
		idb->vines_wakeup_interval = GETOBJ(int,1) * ONESEC;
	    } else {
		idb->vines_wakeup_interval = VRTP_DEF_WAKEUP_INTERVAL;
	    }
	} else {
	    idb->vines_wakeup_interval = VRTP_DEF_WAKEUP_INTERVAL;
	}
	vines_compute_max_wakeup();
	vines_set_wakeup_time(idb);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/**********************************************************************
 *
 *			COMMAND PARSING UTILITIES
 *
 **********************************************************************/

/*
 * parse_vinesaddress
 * Function to parse the construct <network address>:<host address>.
 * The colon (:) and the host address are optional.  A network address
 * of negative one (-1), indicates all networks.
 */
boolean parse_vinesaddress (char *token, ulong *network, ushort *host)
{
    boolean ok, parsed_net, parsed_host;

    parsed_net = parsed_host = FALSE;
    token = deblank(token);

    /*
     * Attempt a look up in the name cache
     */
    if (vines_lookup_address(token, network, host))
	return(TRUE);

    /*
     *  Special case network number '-1'.
     */
    *host = 0;
    if ((*token == '-') && (*(token+1) == '1')) {
	token = token + 2;
	*network = VINES_ALLNETS;
	parsed_net = TRUE;
    } else if (vines_decimal_addresses) {
	/*
	 * Look for a real decimal network number
	 */
	*network = parse_decimal_long(token, &ok);
	if (!ok)
	    return(FALSE);
	while ((*token >= '0') && (*token <= '9')) {
	    token++;
	}
	parsed_net = TRUE;
    } else {
	/*
	 * Look for a real hex network number
	 */
	*network = parse_hex_long(token);
	while (((*token >= '0') && (*token <= '9')) ||
	       ((toupper(*token) >= 'A') && (toupper(*token) <= 'F'))) {
	    token++;
	}
	parsed_net = TRUE;
    }

    /*
     * Now look for the host number
     */
    if (*token++ == ':') {
	if ((*token == '-') && (*(token+1) == '1')) {
	    token += 2;
	    *host = VINES_ALLHOSTS;
	    parsed_host = TRUE;
	} else if (vines_decimal_addresses) {
	    *host = parse_decimal_long(token, &ok);
	    if (!ok)
		return(FALSE);
	    while ((*token >= '0') && (*token <= '9')) {
		token++;
	    }
	    parsed_host = TRUE;
	} else {
	    *host = parse_hex_long(token);
	    while (((*token >= '0') && (*token <= '9')) ||
		   ((toupper(*token) >= 'A') && (toupper(*token) <= 'F'))) {
		token++;
	    }
	    parsed_host = TRUE;
	}
    }
    if (parsed_net && parsed_host)
	return(TRUE);
    return(FALSE);
}

/**********************************************************************
 *
 *			   DISPLAY ROUTINES
 *
 **********************************************************************/

/*
 * vines_interface_display
 *
 * Display VINES interface specific information.  This is the top level
 * function for interface dispolay, and will call out to other functions
 * to print information on specific areas.
 */
void vines_interface_display (vinesidbtype *target)
{
    vinesidbtype *idb;
    idbtype *phys_idb;
    boolean up;
    char *word;

    /*
     * Print some global information.  This should really be elsewhere
     * under a different command.
     */
    automore_enable(NULL);
    if (!target) {
	if ((vines_network & VINES_NET_MFG) == VINES_NET_CISCO) {
	    if (!vines_validate_net(vines_network))
		printf("\n%s\n", vines_questionable_addr);
	}
	printf("\nVINES address is %z", vines_network, vines_host);
	printf("\n  Next client will be %z", vines_network, vines_client);
	printf("\n  Addresses are displayed in %sdecimal format.",
	       vines_decimal_addresses ? "" : "hexa");
	vrtp_display_global();
	vsrtp_display_global();
    }

    /*
     * Now walk the interface queue, displaying the information for the
     * requested interfaces.
     */
    FOR_ALL_VIDBS(idb) {
	phys_idb = idb->phys_idb;
	if (target && (target != idb))
	    continue;
	if (idb == vinesnullidb)
	    continue;

	up = interface_up(phys_idb);
	if (vinestest_debug && (!up || !idb->vines_metric))
	    continue;
	printf("\n%s %s, line protocol %s, network layer %s",
	       idb->phys_idb->namestring, print_idbstate(phys_idb),
	       up ? "up" : "down", idb->vines_lineup ? "up" : "down");
	if (idb->vines_metric == 0L) {
	    printf("\n  VINES protocol processing disabled");
	    continue;
	}

	if (idb_is_ieeelike(phys_idb))
	    printf("\n  VINES broadcast encapsulation is %s",
		   encstring(idb->vines_enctype));
	printf("\n  Interface metric is %04x [%s] (%s seconds)",
	       idb->vines_metric,
	       vmetric_to_config(idb->vines_metric),
	       vmetric_to_seconds(idb->vines_metric));
	printf("\n  Split horizon is %sabled",
	       idb->vines_split_disabled ? "dis" : "en");
	varp_display_interface(idb);
	switch (idb->serverless) {
	  case VSVLESS_NEVER:		word = "disabled"; break;
	  case VSVLESS_DYNAMIC:		word = "dynamic";  break;
	  case VSVLESS_ALWAYS:		word = "enabled";  break;
	  case VSVLESS_ALWAYS_BCAST:	word = "enabled for broadcast";  break;
	  default:			word = "unknown";  break;
	}
	printf("\n  Serverless is %s", word);
	if (idb->serverless == VSVLESS_DYNAMIC) {
	    printf(", currently %sactive",
		   vines_servers_present(idb) ? "in" : "");
	}
	if (idb->vines_redirect_interval != 0) {
	    printf("\n  Redirect interval is %d ms.", idb->vines_redirect_interval);
	} else {
	    printf("\n  Redirects are disabled.");
	}
	switch (idb->propagate) {
	  case VPROP_NEVER:   word = "disabled"; break;
	  case VPROP_ALWAYS:  word = "enabled";  break;
	  case VPROP_DYNAMIC: word = "dynamic";  break;
	  default:            word = "unknown";  break;
	}
	printf("\n  Special propagation of broadcasts is %s", word);
	if (idb->propagate == VPROP_DYNAMIC) {
	    printf(", currently %sactive",
		   vines_any_servers_present() ? "in" : "");
	}
	printf("\n  Outgoing access list is "); notset(idb->vines_accesslist);
	printf("\n  Fast switching is ");
	if (reg_invoke_fast_supported(LINK_VINES,idb->phys_idb->hwptr))
	    printf(idb->phys_idb->hwptr->vines_fastokay ? "enabled" : "disabled");
	else
	    printf("not supported");
	if (idb->input_router_filter)
	    printf("\n  Received routing information source filtered by list %d.",
		   idb->input_router_filter);
	if (idb->input_network_filter)
	    printf("\n  Received routing information content filtered by list %d.",
		   idb->input_network_filter);
	if (idb->output_network_filter)
	    printf("\n  Transmitted routing information content filtered by list %d.",
		   idb->output_network_filter);
	vrtp_display_interface(idb);
	vsrtp_display_interface(idb);
	vneigh_display_interface(idb);
    }
    automore_disable();
}

/*
 * vines_timer_display
 *
 * Print out the all of the timers that are used by each vines process,
 * and when they will expire next.
 */
static void vines_timer_display (parseinfo *csb)
{
    vinesidbtype *idb;
    idbtype *phys_idb;
    char temp[40], temp2[40], temp3[40];

    automore_enable(NULL);

    /*
     * Vines Input
     */
    printf("\n\n  `Vines Input' Process Timers");
    printf("\n    No Timers.");
    
    /*
     * Vines Process
     */
    printf("\n\n  `Vines Protocols' Process Timers");
    varp_display_timer();
    vipc_display_timer();
    vsvc_display_timer();
    
    /*
     * Vines Router
     */
    printf("\n\n  `Vines Router' Process Timers");
    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&vines_timer));
    printf("\n    Master timer: %s", temp);

    if (mgd_timer_running(&vsrtp_start_timer)) {
	sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&vsrtp_start_timer));
	printf("\n      SRTP Startup timer: %s", temp);
    }
    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&vines_changed));
    printf("\n      Change Update timer: %s", temp);
    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&vines_periodic_timer));
    printf("\n      Route Ager timer: %s", temp);

    printf("\n    Per Interface Actions:");
    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&vines_idb_timer));
    printf("\n      Interface master timer: %s", temp);
    FOR_ALL_VIDBS(idb) {
	phys_idb = idb->phys_idb;
	if (idb == vinesnullidb)
	    continue;
	if (!interface_up(phys_idb) || !idb->vines_metric)
	    continue;
	if (mgd_timer_running(&idb->wakeup_timer))
	    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(&idb->wakeup_timer)); 
	else
	    sprintf(temp, "none");
	if (mgd_timer_running(&idb->query_timer))
	    sprint_ticks_in_dhms(temp2, mgd_timer_left_sleeping(&idb->query_timer));
	else
	    sprintf(temp2, "none");
	if (mgd_timer_running(&idb->sync_timer))
	    sprint_ticks_in_dhms(temp3, mgd_timer_left_sleeping(&idb->sync_timer));
	else
	    sprintf(temp3, "none");
	printf("\n      %20s  update %8s  query %8s  sync %8s",
	       phys_idb->namestring, temp, temp2, temp3);
    }

    printf("\n    Per neighbor peer actions:");
    vrtp_display_timer();
    vsrtp_display_timer();
    
    if (GETOBJ(int,1)) {
	printf("\n    Last Routing Update Pass:");
	vines_display_times();
    }

    automore_disable();
}

/*
 * vines_traffic_display_globals
 *
 * Display global traffic statistics by summing the statistics for
 * individual interfaces.
 */
static void vines_traffic_display_globals (void)
{
    ulong framesin, framesout, fmterrors, not_enabled, local_dst, bcast_in;
    ulong rx_forwarded, no_route, zero_hops, cksumerrors, bcast_fwd;
    ulong bcast_helpered, bcast_duplicate, rx_icp_error, ip_unknown, ipc_unknown;
    ulong rx_icp_metric, rx_icp_illegal, rx_ipc, rx_spp;
    ulong rx_echo, proxy_reply, tx_unicast, tx_bcast, tx_forwarded;
    ulong tx_encap, tx_access, tx_down, tx_b_forwarded, tx_nb_source;
    ulong tx_nb_notlan, tx_nb_notgt4800, tx_nb_ppcharge, tx_icp_error;
    ulong tx_icp_metric, tx_ipc, tx_spp, tx_echo, proxy;
    ulong rx_arpv0[VARP_LAST], tx_arpv0[VARP_LAST], rx_arpv0_illegal;
    ulong rx_arpv1[VARP_LAST], tx_arpv1[VARP_LAST], rx_arpv1_illegal;
    ulong rx_rtpv0[VRTP_LAST], tx_rtpv0[VRTP_LAST], rx_rtpv0_illegal;
    ulong rx_rtpv1[VRTP_LAST], tx_rtpv1[VRTP_LAST], rx_rtpv1_illegal;
    vinesidbtype *idb;
    hwidbtype *phys_idb;
    int i;

    framesin = framesout = fmterrors = not_enabled = local_dst = bcast_in = 0;
    rx_forwarded = no_route = zero_hops = cksumerrors = bcast_fwd = 0;
    bcast_helpered = bcast_duplicate = rx_arpv0_illegal = rx_arpv1_illegal = rx_icp_error = 0;
    rx_icp_metric = rx_icp_illegal = rx_ipc = rx_rtpv0_illegal = rx_rtpv1_illegal = rx_spp = 0;
    rx_echo = proxy_reply = tx_unicast = tx_bcast = tx_forwarded = 0;
    tx_encap = tx_access = tx_down = tx_b_forwarded = tx_nb_source = 0;
    tx_nb_notlan = tx_nb_notgt4800 = tx_nb_ppcharge = tx_icp_error = 0;
    tx_icp_metric = tx_ipc = tx_spp = tx_echo = proxy = 0;
    ip_unknown = ipc_unknown = 0;

    for (i = 0; i < VARP_LAST; i++) {
	rx_arpv0[i] = tx_arpv0[i] = 0;
    }
    for (i = 0; i < VARP_LAST; i++) {
	rx_arpv1[i] = tx_arpv1[i] = 0;
    }
    for (i = 0; i < VRTP_LAST; i++) {
	rx_rtpv0[i] = tx_rtpv0[i] = 0;
    }
    for (i = 0; i < VRTP55_LAST; i++) {
	rx_rtpv1[i] = tx_rtpv1[i] = 0;
    }

    FOR_ALL_VIDBS(idb) {
	if (idb == vinesnullidb)
	    continue;
	phys_idb = idb->phys_idb->hwptr;
	framesin  += vines_frames_deltain(phys_idb);
	framesout += vines_frames_deltaout(phys_idb);

	fmterrors       += (idb->rx_too_small - idb->si_rx_too_small)
	                 + (idb->rx_too_large - idb->si_rx_too_large)
	                 + (idb->rx_bad_size - idb->si_rx_bad_size);
	not_enabled     += idb->rx_not_enabled - idb->si_rx_not_enabled;
	local_dst       += idb->rx_local_dest - idb->si_rx_local_dest;
	bcast_in        += idb->rx_bcast_in - idb->si_rx_bcast_in;
	rx_forwarded    += idb->rx_forwarded - idb->si_rx_forwarded;
	no_route        += idb->rx_no_route - idb->si_rx_no_route;
	zero_hops       += idb->rx_zero_hop_count - idb->si_rx_zero_hop_count;
	cksumerrors     += idb->rx_checksum_error - idb->si_rx_checksum_error;
	ip_unknown      += idb->rx_ip_unknown - idb->si_rx_ip_unknown;
	ipc_unknown     += idb->rx_ipc_unknown - idb->si_rx_ipc_unknown;
	bcast_fwd       += idb->rx_bcast_forwarded - idb->si_rx_bcast_forwarded;
	bcast_helpered  += idb->rx_bcast_helpered - idb->si_rx_bcast_helpered;
	bcast_duplicate += idb->rx_bcast_duplicate - idb->si_rx_bcast_duplicate;
	rx_arpv0_illegal  += idb->rx_arpv0_illegal - idb->si_rx_arpv0_illegal;
	rx_arpv1_illegal  += idb->rx_arpv1_illegal - idb->si_rx_arpv1_illegal;
	rx_icp_error    += idb->rx_icp_error - idb->si_rx_icp_error;
	rx_icp_metric   += idb->rx_icp_metric - idb->si_rx_icp_metric;
	rx_icp_illegal  += idb->rx_icp_metric - idb->si_rx_icp_metric;
	rx_ipc	        += idb->rx_ipc - idb->si_rx_ipc;
	rx_rtpv0_illegal+= idb->rx_rtpv0_illegal - idb->si_rx_rtpv0_illegal;
	rx_rtpv1_illegal+= idb->rx_rtpv1_illegal - idb->si_rx_rtpv1_illegal;
	rx_spp	        += idb->rx_spp - idb->si_rx_spp;
	rx_echo         += idb->rx_echo - idb->si_rx_echo;
	proxy_reply     += idb->rx_proxy_reply - idb->si_rx_proxy_reply;

	tx_unicast      += idb->tx_unicast - idb->si_tx_unicast;
	tx_bcast        += idb->tx_bcast - idb->si_tx_bcast;
	tx_forwarded    += idb->tx_forwarded - idb->si_tx_forwarded;
	tx_encap        += idb->tx_failed_encaps - idb->si_tx_failed_encaps;
	tx_access       += idb->tx_failed_access - idb->si_tx_failed_access;
	tx_down         += idb->tx_failed_down - idb->si_tx_failed_down;
	tx_b_forwarded  += idb->tx_bcast_forwarded - idb->si_tx_bcast_forwarded;
	tx_nb_source    += idb->tx_notbcast_toward_source - idb->si_tx_notbcast_toward_source;
	tx_nb_notlan    += idb->tx_notbcast_notlan - idb->si_tx_notbcast_notlan;
	tx_nb_notgt4800 += idb->tx_notbcast_notgt4800 - idb->si_tx_notbcast_notgt4800;
	tx_nb_ppcharge  += idb->tx_notbcast_ppcharge - idb->si_tx_notbcast_ppcharge;
	tx_icp_error    += idb->tx_icp_error - idb->si_tx_icp_error;
	tx_icp_metric   += idb->tx_icp_metric - idb->si_tx_icp_metric;
	tx_ipc	        += idb->tx_ipc - idb->si_tx_ipc;
	tx_spp	        += idb->tx_spp - idb->si_tx_spp;
	tx_echo         += idb->tx_echo - idb->si_tx_echo;
	proxy           += idb->tx_proxy - idb->si_tx_proxy;
	
	for (i = 0; i < VARP_LAST; i++) {
	    rx_arpv0[i] += idb->rx_arpv0_type[i] - idb->si_rx_arpv0_type[i];
	    tx_arpv0[i] += idb->tx_arpv0_type[i] - idb->si_tx_arpv0_type[i];
	}
	for (i = 0; i < VARP_LAST; i++) {
	    rx_arpv1[i] += idb->rx_arpv1_type[i] - idb->si_rx_arpv1_type[i];
	    tx_arpv1[i] += idb->tx_arpv1_type[i] - idb->si_tx_arpv1_type[i];
	}
	for (i = 0; i < VRTP_LAST; i++) {
	    rx_rtpv0[i] += idb->rx_rtpv0_type[i] - idb->si_rx_rtpv0_type[i];
	    tx_rtpv0[i] += idb->tx_rtpv0_type[i] - idb->si_tx_rtpv0_type[i];
	}
	for (i = 0; i < VRTP55_LAST; i++) {
	    rx_rtpv1[i] += idb->rx_rtpv1_type[i] - idb->si_rx_rtpv1_type[i];
	    tx_rtpv1[i] += idb->tx_rtpv1_type[i] - idb->si_tx_rtpv1_type[i];
	}
    }
    
    printf("\n    Rcvd: %u total, %u format errors, %u not enabled,",
	   framesin, fmterrors, not_enabled);
    printf("\n          %u local dst, %u bcast, %u forwarded,",
	   local_dst, bcast_in, rx_forwarded);
    printf("\n          %u no route, %u zero hops",
	   no_route, zero_hops);
    printf("\n          %u checksum errors, %u IP unknown, %u IPC unknown",
	   cksumerrors,  ip_unknown, ipc_unknown);
    printf("\n          %u bcast forwarded, %u bcast helpered, %u dup bcast",
	   bcast_fwd, bcast_helpered, bcast_duplicate);

    printf("\n    Sent: %u total, %u unicast, %u bcast, %u forwarded,",
	   framesout, tx_unicast, tx_bcast, tx_forwarded);
    printf("\n          %u encap failed, %u access failed, %u down",
	   tx_encap, tx_access, tx_down);
    printf("\n          %u bcast fwd, %u not fwd (toward source)",
	   tx_b_forwarded, tx_nb_source);
    printf("\n          %u notlan, %u not gt4800, %u no pp charge",
	   tx_nb_notlan, tx_nb_notgt4800, tx_nb_ppcharge);

    printf("\n   ARPv0: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   rx_arpv0[0], rx_arpv0[1], rx_arpv0[2], rx_arpv0[3], rx_arpv0_illegal,
	   tx_arpv0[0], tx_arpv0[1], tx_arpv0[2], tx_arpv0[3]);
    printf("\n   ARPv1: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   rx_arpv1[0], rx_arpv1[1], rx_arpv1[2], rx_arpv1[3], rx_arpv1_illegal,
	   tx_arpv1[0], tx_arpv1[1], tx_arpv1[2], tx_arpv1[3]);
    printf("\n     ICP: Rcvd %u/%u/%u, Sent %u/%u",
	   rx_icp_error, rx_icp_metric, rx_icp_illegal, tx_icp_error,
	   tx_icp_metric);
    printf("\n     IPC: Rcvd %u, Sent %u", rx_ipc, tx_ipc);
    printf("\n   RTPv0: Rcvd %u/%u/%u/%u/%u/%u/%u/%u, Sent %u/%u/%u/%u/%u/%u/%u",
	   rx_rtpv0[0], rx_rtpv0[1], rx_rtpv0[2], rx_rtpv0[3], rx_rtpv0[4],
	   rx_rtpv0[5], rx_rtpv0[6], rx_rtpv0_illegal,
	   tx_rtpv0[0], tx_rtpv0[1], tx_rtpv0[2], tx_rtpv0[3], tx_rtpv0[4],
	   tx_rtpv0[5], tx_rtpv0[6]);
    printf("\n   RTPv1: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   rx_rtpv1[1], rx_rtpv1[2], rx_rtpv1[3], rx_rtpv1[4], rx_rtpv1_illegal,
	   tx_rtpv1[1], tx_rtpv1[2], tx_rtpv1[3], tx_rtpv1[4]);
    printf("\n     SPP: Rcvd %u, Sent %u", rx_spp, tx_spp);

    printf("\n    Echo: Rcvd %u, Sent %u", rx_echo, tx_echo);
    printf("\n   Proxy: Rcvd %u, Sent %u", proxy_reply, proxy);
}

/*
 * vines_traffic_display_by_idb
 *
 * Display the various activities that have been performed on received
 * packets, on a per interface basis.
 */
static void vines_traffic_display_by_idb (vinesidbtype *idb)
{
    hwidbtype *phys_idb;

    phys_idb = idb->phys_idb->hwptr;
/*
    Rcvd: 12345678 total, 12345678 format errors, 12345678 not enabled
          12345678 local dst, 12345678 bcast, 12345678 forwarded
          12345678 no route, 12345678 zero hops
          12345678 checksum errors, 12345678 IP unknown, 12345678 IPC unknown
          12345678 bcast forwarded, 12345678 bcast helpered, 12345678 dup bcast

    Sent: 12345678 total, 12345678 unicast, 12345678 bcast, 12345678 forwarded
          12345678 encap failed, 12345678 access failed, 12345678 down
          12345678 bcast fwd, 12345678 not fwd (toward source)
          12345678 notlan, 12345678 not gt4800, 12345678 no pp charge

     ARP: Rcvd 0/1/2/3/i, Sent 0/1/2/3
     ICP: Rcvd 0/1/i, Sent 0/1
     IPC: Rcvd x, Sent x
     RTP: Rcvd 0/1/2/3/4/5/6/i, Sent 0/1/2/3/4/5/6
     SPP: Rcvd x, Sent x

*/
    printf("\n    Rcvd: %u packets, %u bytes, %u format errors, %u not enabled",
	   vines_frames_deltain(phys_idb),
	   vines_bytes_deltain(phys_idb),
	   ((idb->rx_too_small - idb->si_rx_too_small) +
	    (idb->rx_too_large - idb->si_rx_too_large) +
	    (idb->rx_bad_size - idb->si_rx_bad_size)),
	   idb->rx_not_enabled - idb->si_rx_not_enabled);
    printf("\n          %u local dst, %u bcast, %u forwarded",
	   idb->rx_local_dest - idb->si_rx_local_dest,
	   idb->rx_bcast_in - idb->si_rx_bcast_in,
	   idb->rx_forwarded - idb->si_rx_forwarded +
	      vines_frames_fastin(phys_idb));
    printf("\n          %u no route, %u zero hops",
	   idb->rx_no_route - idb->si_rx_no_route,
	   idb->rx_zero_hop_count - idb->si_rx_zero_hop_count);
    printf("\n          %u checksum errors, %u IP unknown, %u IPC unknown",
	   idb->rx_checksum_error - idb->si_rx_checksum_error,
	   idb->rx_ip_unknown - idb->si_rx_ip_unknown,
	   idb->rx_ipc_unknown - idb->si_rx_ipc_unknown);
    printf("\n          %u bcast forwarded, %u bcast helpered, %u dup bcast",
	   idb->rx_bcast_forwarded - idb->si_rx_bcast_forwarded,
	   idb->rx_bcast_helpered - idb->si_rx_bcast_helpered,
	   idb->rx_bcast_duplicate - idb->si_rx_bcast_duplicate);

    printf("\n    Sent: %u packets, %u bytes",
	   vines_frames_deltaout(phys_idb),
	   vines_bytes_deltaout(phys_idb));
    printf("\n          %u unicast, %u bcast, %u forwarded",
	   idb->tx_unicast - idb->si_tx_unicast,
	   idb->tx_bcast - idb->si_tx_bcast,
	   idb->tx_forwarded - idb->si_tx_forwarded +
	       vines_frames_fastout(phys_idb));
    printf("\n          %u encap failed, %u access failed, %u down",
	   idb->tx_failed_encaps - idb->si_tx_failed_encaps,
	   idb->tx_failed_access - idb->si_tx_failed_access,
	   idb->tx_failed_down - idb->si_tx_failed_down);
    printf("\n          %u bcast fwd, %u not fwd (toward source)",
	   idb->tx_bcast_forwarded - idb->si_tx_bcast_forwarded,
	   idb->tx_notbcast_toward_source - idb->si_tx_notbcast_toward_source);
    printf("\n          %u notlan, %u not gt4800, %u no pp charge",
	   idb->tx_notbcast_notlan - idb->si_tx_notbcast_notlan,
	   idb->tx_notbcast_notgt4800 - idb->si_tx_notbcast_notgt4800,
	   idb->tx_notbcast_ppcharge - idb->si_tx_notbcast_ppcharge);

    printf("\n   ARPv0: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   idb->rx_arpv0_type[0] - idb->si_rx_arpv0_type[0],
	   idb->rx_arpv0_type[1] - idb->si_rx_arpv0_type[1],
	   idb->rx_arpv0_type[2] - idb->si_rx_arpv0_type[2],
	   idb->rx_arpv0_type[3] - idb->si_rx_arpv0_type[3],
	   idb->rx_arpv0_illegal - idb->si_rx_arpv0_illegal,
	   idb->tx_arpv0_type[0] - idb->si_tx_arpv0_type[0],
	   idb->tx_arpv0_type[1] - idb->si_tx_arpv0_type[1],
	   idb->tx_arpv0_type[2] - idb->si_tx_arpv0_type[2],
	   idb->tx_arpv0_type[3] - idb->si_tx_arpv0_type[3]);
    printf("\n   ARPv1: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   idb->rx_arpv1_type[0] - idb->si_rx_arpv1_type[0],
	   idb->rx_arpv1_type[1] - idb->si_rx_arpv1_type[1],
	   idb->rx_arpv1_type[2] - idb->si_rx_arpv1_type[2],
	   idb->rx_arpv1_type[3] - idb->si_rx_arpv1_type[3],
	   idb->rx_arpv1_illegal - idb->si_rx_arpv1_illegal,
	   idb->tx_arpv1_type[0] - idb->si_tx_arpv1_type[0],
	   idb->tx_arpv1_type[1] - idb->si_tx_arpv1_type[1],
	   idb->tx_arpv1_type[2] - idb->si_tx_arpv1_type[2],
	   idb->tx_arpv1_type[3] - idb->si_tx_arpv1_type[3]);
    printf("\n     ICP: Rcvd %u/%u/%u, Sent %u/%u",
	   idb->rx_icp_error - idb->si_rx_icp_error,
	   idb->rx_icp_metric - idb->si_rx_icp_metric,
	   idb->rx_icp_illegal - idb->si_rx_icp_illegal,
	   idb->tx_icp_error - idb->si_tx_icp_error,
	   idb->tx_icp_metric - idb->si_tx_icp_metric);
    printf("\n     IPC: Rcvd %u, Sent %u",
	   idb->rx_ipc - idb->si_rx_ipc,
	   idb->tx_ipc - idb->si_tx_ipc);
    printf("\n   RTPv0: Rcvd %u/%u/%u/%u/%u/%u/%u/%u, Sent %u/%u/%u/%u/%u/%u/%u",
	   idb->rx_rtpv0_type[0] - idb->si_rx_rtpv0_type[0],
	   idb->rx_rtpv0_type[1] - idb->si_rx_rtpv0_type[1],
	   idb->rx_rtpv0_type[2] - idb->si_rx_rtpv0_type[2],
	   idb->rx_rtpv0_type[3] - idb->si_rx_rtpv0_type[3],
	   idb->rx_rtpv0_type[4] - idb->si_rx_rtpv0_type[4],
	   idb->rx_rtpv0_type[5] - idb->si_rx_rtpv0_type[5],
	   idb->rx_rtpv0_type[6] - idb->si_rx_rtpv0_type[6],
	   idb->rx_rtpv0_illegal - idb->si_rx_rtpv0_illegal,
	   idb->tx_rtpv0_type[0] - idb->si_tx_rtpv0_type[0],
	   idb->tx_rtpv0_type[1] - idb->si_tx_rtpv0_type[1],
	   idb->tx_rtpv0_type[2] - idb->si_tx_rtpv0_type[2],
	   idb->tx_rtpv0_type[3] - idb->si_tx_rtpv0_type[3],
	   idb->tx_rtpv0_type[4] - idb->si_tx_rtpv0_type[4],
	   idb->tx_rtpv0_type[5] - idb->si_tx_rtpv0_type[5],
	   idb->tx_rtpv0_type[6] - idb->si_tx_rtpv0_type[6]);
    printf("\n   RTPv1: Rcvd %u/%u/%u/%u/%u, Sent %u/%u/%u/%u",
	   idb->rx_rtpv1_type[1] - idb->si_rx_rtpv1_type[1],
	   idb->rx_rtpv1_type[2] - idb->si_rx_rtpv1_type[2],
	   idb->rx_rtpv1_type[3] - idb->si_rx_rtpv1_type[3],
	   idb->rx_rtpv1_type[4] - idb->si_rx_rtpv1_type[4],
	   idb->rx_rtpv1_illegal - idb->si_rx_rtpv1_illegal,
	   idb->tx_rtpv1_type[1] - idb->si_tx_rtpv1_type[1],
	   idb->tx_rtpv1_type[2] - idb->si_tx_rtpv1_type[2],
	   idb->tx_rtpv1_type[3] - idb->si_tx_rtpv1_type[3],
	   idb->tx_rtpv1_type[4] - idb->si_tx_rtpv1_type[4]);
    printf("\n     SPP: Rcvd %u, Sent %u",
	   idb->rx_spp - idb->si_rx_spp,
	   idb->tx_spp - idb->si_tx_spp);

    printf("\n    Echo: Rcvd %u, Sent %u",
	   idb->rx_echo - idb->si_rx_echo,
	   idb->tx_echo - idb->si_tx_echo);
    printf("\n   Proxy: Rcvd %u, Sent %u",
	   idb->rx_proxy_reply - idb->si_rx_proxy_reply,
	   idb->tx_proxy - idb->si_tx_proxy);
}

/*
 * vines_traffic_display
 *
 * Display VINES traffic statistics.  This will display all known
 * counters, both as global numbers, and on a per interface basis.
 */
void vines_traffic_display (vinesidbtype *target)
{
    vinesidbtype *idb;

    automore_enable(NULL);
    if (!target) {
	printf("\nSYSTEM TRAFFIC:");
	vines_traffic_display_globals();
	automore_conditional(0);
	
	printf("\n\nIPC TRAFFIC BY PORT NUMBER:");
	vipc_display_traffic();
	automore_conditional(0);
    }

    FOR_ALL_VIDBS(idb) {
	if (target && (target != idb))
	    continue;
	if ((idb == vinesnullidb) || (!idb->vines_metric))
	    continue;
	printf("\n\nInterface %s", idb->phys_idb->namestring);
	vines_traffic_display_by_idb(idb);
    }
    automore_disable();
}

/*
 * show_vines
 * Display VINES Protocol information
 */
void show_vines (parseinfo *csb)
{
    ulong net;
    ushort host;
    idbtype *swidb;
    vinesidbtype *idb;
    vinesneighbortype *neighbor;

    if (!vines_running) {
	printf("%% VINES not enabled");
	return;
    }

    /*
     * If any idb was specified, validate it *now*.  If it doesn't
     * exist, print a nice message and bail out.
     */
    if (GETOBJ(idb,1) != NULL) {
	swidb = GETOBJ(idb,1);
	idb = vines_getidb(swidb);
	if (idb == NULL) {
	    printf("\n%% VINES has never been configured on interface %s",
		   swidb->namestring);
	    return;
	}
    } else {
	swidb = NULL;
	idb = NULL;
    }

    switch (csb->which) {
      case SHOW_VINES_ACCESS:
	vines_access_display(GETOBJ(int,1));
	break;

      case SHOW_VINES_BOTH:
	net = (*GETOBJ(paddr,1)).vines_addr.net;
	host = (*GETOBJ(paddr,1)).vines_addr.host;
	vines_display_neighbors(net, host, NULL);
	printf("\n\n");
	vroute_display_all_nets(net, NULL);
	break;

      case SHOW_VINES_CACHE:
	switch (GETOBJ(int,1)) {
	  case DO_ALL:
	    show_vines_cache(0, 0, NULL, NULL);
	    break;
	  case DO_INTERFACE:
	    show_vines_cache(0, 0, NULL, idb);
	    break;
	  case DO_NEIGHBOR:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    neighbor = vneigh_FindEntry(net, host);
	    show_vines_cache(0, 0, neighbor, NULL);
	    break;
	  case DO_ONE:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    show_vines_cache(net, host, NULL, NULL);
	    break;
	  case DO_SERVER:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    show_vines_cache(net, 0, NULL, NULL);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	break;

      case SHOW_VINES_HOST:
	if (GETOBJ(int,1))
	    vines_name_display(GETOBJ(string,1));
	else
	    vines_name_display(NULL);
	break;

      case SHOW_VINES_INTERFACE:
	vines_interface_display(idb);
	break;

      case SHOW_VINES_IPC:
	vipc_display();
	break;

      case SHOW_VINES_NEIGHBORS:
	switch (GETOBJ(int,1)) {
	  case DO_ALL:
	    vines_display_neighbors(0, 0, NULL);
	    break;
	  case DO_INTERFACE:
	    vines_display_neighbors(0, 0, idb);
	    break;
	  case DO_ONE:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    vines_display_neighbors(net, host, NULL);
	    break;
	  case DO_SERVER:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    vines_display_neighbors(net, 0, NULL);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	break;

      case SHOW_VINES_ROUTE:
	switch (GETOBJ(int,1)) {
	  case DO_ALL:
	    vroute_display_all_nets(0, NULL);
	    break;
	  case BY_METRIC:
	    vroute_display_nets_by_metric(0, NULL);
	    break;
	  case DO_NEIGHBOR:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    neighbor = vneigh_FindEntry(net, host);
	    vroute_display_all_nets(0, neighbor);
	    break;
	  case DO_SERVER:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    vroute_display_all_nets(net, NULL);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	break;

      case SHOW_VINES_TIMERS:
	vines_timer_display(csb);
	break;

      case SHOW_VINES_TRAFFIC:
	vines_traffic_display(idb);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;

    }
}

/**********************************************************************
 *
 *			    CLEAR COMMANDS
 *
 **********************************************************************/

/*
 * vines_clear_cmd
 *
 * Clear one of the internal VINES tables.  The vines_changed variable is
 * trapped to prevent an abortive attempt to send a set of change
 * updates.  The attempt will abort because there are no entries marked
 * as changed (they were all deleted) even though the global changed flag
 * is set.
 */
void vines_clear_cmd (parseinfo *csb)
{
    sys_timestamp old_vines_changed;
    ulong net;
    ushort host;
    idbtype *swidb;
    vinesidbtype *idb;
    vinesneighbortype *neighbor;
    vinesnettype *server;

    if (!vines_running) {
	printf("%% VINES not enabled");
	return;
    }

    /*
     * If any idb was specified, validate it *now*.  If it doesn't
     * exist, print a nice message and bail out.
     */
    if (GETOBJ(idb,1) != NULL) {
	swidb = GETOBJ(idb,1);
	idb = vines_getidb(swidb);
	if (idb == NULL) {
	    printf("\n%% VINES has never been configured on interface %s",
		   swidb->namestring);
	    return;
	}
    } else {
	swidb = NULL;
	idb = NULL;
    }

    old_vines_changed = mgd_timer_exp_time(&vines_changed);

    switch (csb->which) {
      case CLEAR_VINES_CACHE:
	switch (GETOBJ(int,1)) {
	  case DO_INTERFACE:
	    vines_flush_cache_by_idb(hwidb_or_null(swidb));
	    break;
	  case DO_NEIGHBOR:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    neighbor = vneigh_FindEntry(net, host);
	    vines_flush_cache_by_neighbor(neighbor);
	    break;
	  case DO_SERVER:
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    server = vroute_FindEntry(net);
	    vines_flush_cache_by_server(server);
	    break;
	  case DO_ALL:
	    vinescache_ager(TRUE);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	break;

      case CLEAR_VINES_CACHE_COUNTERS:
	    vinescache_ager(TRUE);
	    vcache_clear_counters();
	    break;

      case CLEAR_VINES_NEIGHBOR:
	if (GETOBJ(int,1)) {
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    host = (*GETOBJ(paddr,1)).vines_addr.host;
	    vines_flush_dynamic_paths(net, host);
	} else {
	    vines_neighbor_ager(TRUE);
	    vroute_send_requests(NULL);
	}
	break;

      case CLEAR_VINES_ROUTE:
	if (GETOBJ(int,1)) {
	    net = (*GETOBJ(paddr,1)).vines_addr.net;
	    vines_flush_dynamic_routes(net);
	} else {
	    vinesneighbortype *neighbor;
	    vinespathtype *path;
	    
	    vroute_age_all_nets(TRUE);

	    /* Reset router sequence number for all paths.  This will
	     * enable us to relearn routes learned via SRTP.
	     */
	    for (neighbor = vneigh_GetFirstNeighbor(); neighbor;
		 neighbor = vneigh_GetNextNeighbor(neighbor)) {
		FOR_ALL_PATHS(neighbor, path)
		    path->origin_timestamp = 0;
	    }
	    
	    vroute_send_requests(NULL);
	}
	break;

      case CLEAR_VINES_TRAFFIC:
	vines_traffic_checkpoint();
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    if (TIMER_RUNNING(old_vines_changed)) {
	mgd_timer_set_exptime(&vines_changed, &old_vines_changed);
    } else {
	mgd_timer_stop(&vines_changed);
    }
}

/**********************************************************************
 *
 *	     VINES STATIC NAME <=> ADDRESS TABLE SUPPORT
 *
 **********************************************************************/

#define FOR_ALL_NAMES(name) \
    for ((name) = vines_nameQ.qhead; (name); (name) = (name)->next)

static const char vines_name_header[] = "Name             Address";

/*
 * vines_name_add
 *
 * Add a name/address pair to the static name/address mapping table.
 */
void vines_name_add (char *name, ulong net, ushort host)
{
    vinesnametype *namep;

    namep = malloc(sizeof(vinesnametype) + strlen(name) + 2);
    if (!namep)
	return;
    namep->net = net;
    namep->host = host;
    strcpy((char *)namep->name, name);
    vines_name_delete(name);			/* delete any old one */
    enqueue(&vines_nameQ, namep);		/* add the new one */
}

/*
 * vines_name_delete
 *
 * Remove a name/address pair from the static name/address mapping table.
 */
void vines_name_delete (char *name)
{
    vinesnametype *namep, *next;

    if (!vines_nameQ.qhead)
	return;

    for (namep = vines_nameQ.qhead; namep; namep = next) {
	next = namep->next;
	if (strcasecmp(namep->name, name) == 0) {
	    unqueue(&vines_nameQ, namep);
	    free(namep);
	}
    }
}

/*
 * vines_name_display
 *
 * Display all name/address pairs in the static name/address mapping
 * table.
 */
void vines_name_display (char *name)
{
    vinesnametype *namep;

    automore_enable(vines_name_header);
    FOR_ALL_NAMES(namep) {
	if (name && strcasecmp(namep->name, name) != 0)
	    continue;
	printf("\n%15s  %16z", namep->name, namep->net, namep->host);
    }
    automore_disable();
}

/*
 * vines_lookup_address
 *
 * Look up a name/address pairs by address.  This is most commonly called
 * by the printf routine.
 */
boolean vines_lookup_address (char *name, ulong *net, ushort *host)
{
    vinesnametype *namep;
    ulong st_net;

    if (strcasecmp(name, "Broadcast") == 0) {
	*net = VINES_ALLNETS;
	*host = VINES_ALLHOSTS;
	return(TRUE);
    }
    if (strlen(vines_hostname) && (strcasecmp(name, vines_hostname) == 0)) {
	*net = vines_network;
	*host = vines_host;
	return(TRUE);
    }
    st_net = vst_find_server_by_name(name);
    if (st_net != 0) {
	*net = st_net;
	*host = VINES_SERVER;
	return(TRUE);
    }
    FOR_ALL_NAMES(namep) {
	if (name && strcasecmp(namep->name, name) == 0) {
	    *net = namep->net;
	    *host = namep->host; 
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * vines_lookup_name
 *
 * Look up a name/address pairs by name.  This is most commonly called
 * by the parser..
 */
char *vines_lookup_name (ulong net, ushort host)
{
    vinesnametype *namep;
    char *st_name;

    if ((net == VINES_ALLNETS) || (host == VINES_ALLHOSTS))
	return("Broadcast");
    if ((net == VINES_UNKNOWNNET) && (host == VINES_UNKNOWNHOST))
	return("New Client");
    if ((net == vines_network) && (host == vines_host) && strlen(vines_hostname))
	return(vines_hostname);
    if (host == VINES_SERVER) {
	st_name = vst_find_server_by_address(net);
	if (st_name)
	    return(st_name);
    }
    FOR_ALL_NAMES(namep)
	if (net == namep->net && host == namep->host)
	    return(namep->name);
    return(NULL);
}

/**********************************************************************
 *
 *			      DEBUGGING
 *
 **********************************************************************/

/*
 * vinesbug
 *
 * Print a pretty buginf about a VINES packet.
 */
void vinesbug (paktype *pak, char *string)
{
    vinesiptype *vip;
    char in[20], out[20];

    vip = (vinesiptype *)pak->network_start;
    if (pak->if_input && (pak->if_input != nullidb))
	sprintf(in, " (%s)", pak->if_input->short_namestring);
    else
	sprintf(in, " (local)");
    if (pak->if_output && (pak->if_output != nullidb))
	sprintf(out, " (%s)", pak->if_output->short_namestring);
    else
	out[0] = '\0';

    buginf("\nVINES: s=%#z%s, d=%#z%s, %s [%d]",
	   GET2WD(vip->ssrcnet), vip->srchost, in,
	   GET2WD(vip->ddstnet), vip->dsthost, out,
	   string, pak->datagramsize);
}

/*
 * vinesbuggw
 *
 * Pretty print the status of a VINES packet being forwarded.
 */
void vinesbuggw (paktype *pak, vinesneighbortype *gw, char *string)
{
    vinesiptype *vip;
    char in[20], out[20], gate[25];

    vip = (vinesiptype *)pak->network_start;
    if (pak->if_input && (pak->if_input != nullidb))
	sprintf(in, " (%s)", pak->if_input->short_namestring);
    else
	sprintf(in, " (local)");
    if (pak->if_output && (pak->if_output != nullidb))
	sprintf(out, " (%s)", pak->if_output->short_namestring);
    else
	out[0] = '\0';
    if (gw)
	sprintf(gate, ", g=%#z", gw->net, gw->host);
    else
	gate[0] = '\0';
    
    buginf("\nVINES: s=%#z%s, d=%#z%s%s, %s",
	   GET2WD(vip->ssrcnet), vip->srchost, in,
	   GET2WD(vip->ddstnet), vip->dsthost, out,
	   gate, string);
}

/*
 * display_vinespak
 *
 * Print out all the information in a packet.  This set of routines does
 * not use any of the pointers in the packet header other than the
 * network start pointer, so it may be called at any point in the
 * processing of a packet.
 */
static void display_vinespak (
    paktype *pak,
    void (*fn)(const char *fmt, ...))
{
    vinesiptype *vip;
    vinesicptype *icp;
    int c, i, j, limit;
    uchar *data;
    hwaddrtype macaddr;
    char *type, srcmac[20], dstmac[20];
    idbtype *idb;

    data = NULL;
    limit = 0;

    /*
     * Print the VINES IP header.
     */
    vip = (vinesiptype *)pak->network_start;
    (*fn)("\nVINES: checksum %04x, length %04x, tc %02x, protocol ",
	   vip->cksum, vip->len, vip->tc);
    if (vip->protocol <= 5)
	(*fn)("%s", vines_ip_types[vip->protocol]);
    else
	(*fn)("%0x", vip->protocol);

    idb = pak->if_input;
    if (pak->if_input && pak->if_input->hwptr->status & IDB_IEEEMASK) {
	(*pak->if_input->hwptr->extract_snpa)(pak, &macaddr, DESTINATION);
	sprintf(dstmac, " (%e)", macaddr.addr);
	(*pak->if_input->hwptr->extract_snpa)(pak, &macaddr, SOURCE);
	sprintf(srcmac, " (%e)", macaddr.addr);
    } else if (pak->if_output && pak->if_output->hwptr->status & IDB_IEEEMASK) {
	(*pak->if_output->hwptr->extract_snpa)(pak, &macaddr, DESTINATION);
	sprintf(dstmac, " (%e)", macaddr.addr);
	(*pak->if_output->hwptr->extract_snpa)(pak, &macaddr, SOURCE);
	sprintf(srcmac, " (%e)", macaddr.addr);
    } else {
	dstmac[0] = '\0';
	srcmac[0] = '\0';
    }
    (*fn)("\n       dst %z%s src %z%s", GET2WD(vip->ddstnet),
	   vip->dsthost, dstmac, GET2WD(vip->ssrcnet), vip->srchost, srcmac);

    switch (vip->protocol) {
    case VINES_IPC:
	limit = vip->len - sizeof(vinesiptype);
	data = display_vinespak_ipc(vip->data, &limit, fn);
	break;

    case VINES_SPP:
	limit = vip->len - sizeof(vinesiptype);
	data = display_vinespak_spp(vip->data, &limit, fn);
	break;

    case VINES_ARP:
	data = varp_decode_packet(vip->data, &limit, fn);
	break;

    case VINES_RTP:
	limit = vip->len - sizeof(vinesiptype);
	data = vroute_decode_packet(vip->data, &limit, fn);
	break;
	
    case VINES_ICP:
	icp = (vinesicptype *)vip->data;
	data = (uchar *)icp->oldpak;
	limit = vip->len - sizeof(vinesiptype) - sizeof(vinesicptype);
	type = (icp->type > VICP_LAST)?"unknown":vicp_types[icp->type];
	(*fn) ("\n   ICP %s (%d), info %d", type, icp->type, icp->info);
	break;
    }

    limit = min(limit, 160);
    for (i = 0; i < limit; i += 16, data += 16) {
	(*fn)((i == 0) ? "\n  Data " : "\n       ");
	for (j = 0; ((j < 16)  && (i + j < limit)); j++)
	    (*fn)("%02x ", data[j] & 0xFF);
	if (j < 16)
	    (*fn)("%*c", (16-j)*3, ' ');
	(*fn)("     ");
	for (j = 0; ((j < 16) && (i + j < limit)); j++) {
	    c = data[j] & 0x7F;
	    if (c  < 0x20) c = '.';
	    if (c == 0x7F) c = '.';
	    (*fn)("%c", c);
	}
    }
}

/*
 * print_vinespak
 *
 * Call the packet decoder with 'printf' as the print routine.
 */
void print_vinespak (paktype *pak)
{
    display_vinespak(pak, (void (*)(const char *fmt, ...))printf);
}

/*
 * buginf_vinespak
 *
 * Call the packet decoder with 'buginf' as the print routine.
 */
void buginf_vinespak (paktype *pak)
{
    display_vinespak(pak, buginf);
}

/**********************************************************************
 *
 *		       PRINTF SUPPORT ROUTINES
 *
 **********************************************************************/

/*
 * vines_printf_littlez
 * Print an Vines address in the preferred format.
 * z   prints a vines address.  Takes two arguments, a ulong and a ushort.
 *     # - indicates that the textual name should be used if known.
 */

int vines_printf_littlez (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
			  boolean rjust, int width, int size, int hash,
			  int precision)
{
    int length, d, charcount;
    ulong n, a1;
    char *f;

    charcount = 0;
    n = va_arg(*argp,ulong);
    a1 = (ushort)va_arg(*argp,ulong);  /* all stack args are longs */
    if (hash) {		/* # flag = Use node name if known */
	f = vines_lookup_name(n, a1);
    } else {
	f = NULL;
    }
    if (*cp) {
	if (f != NULL)
	    length = sprintf(*cp,"%s",f);
	else if (vines_decimal_addresses)
	    length = sprintf(*cp,"%010u:%05u",n,a1);
	else
	    length = sprintf(*cp,"%08x:%04x",n,a1);
	*cp += length;
    } else {
	if (f != NULL)
	    length = _ttyprintf(tty,"%s",f);
	else if (vines_decimal_addresses)
	    length = _ttyprintf(tty,"%010u:%05u",n,a1);
	else
	    length = _ttyprintf(tty,"%08x:%04x",n,a1);
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/*
 * vines_swif_erase
 *
 * A software interface has been removed.  Find any vines interface structures
 * that correspond to the interface structure and reset them.
 */
void vines_swif_erase (idbtype *swidb)
{
    vinesidbtype *idb;
    leveltype level;

    /*
     * Does this exist?
     */
    idb = vines_getidb(swidb);
    if (idb == NULL)
	return;

    /*
     * Zap all information learned via this path.  This includes static
     * entries.  You can't have a static entry if you don't have an
     * interface.
     */
    vines_flush_all_paths_by_xxx(0, idb);
    idb->vines_metric = 0;

    /*
     * Move the idb to a "deleted" idbs queue.  It can't be freed,
     * because someone else might have a pointer to it and the code isn't
     * protected against that.  Yes, the unqueue runs the idb queue
     * again.  It updates the tail pointer properly though.  Kill the
     * swidb to vinesidb association because the code is protected
     * against it.  The vinesidb to swidb association must be kept
     * because there is also no protection for that.
     */
    unqueue(vines_idbQ, idb);
    level = raise_interrupt_level(ALL_DISABLE);

    mgd_timer_stop(&idb->sync_timer);
    mgd_timer_stop(&idb->query_timer);
    mgd_timer_stop(&idb->wakeup_timer);
    mgd_timer_stop(&idb->arp_timer);
    TIMER_STOP(idb->update_start_time);
    TIMER_STOP(idb->update_end_time);

    idb->next_deleted = vines_deleted_idbs;
    vines_deleted_idbs = idb;
    reset_interrupt_level(level);
    idb_delete_swsb(idb->phys_idb, SWIDB_SB_VINES);
}

/*
 * vines_printf_bigz
 * Print an Vines address in the preferred format.
 * Z   prints a vines server number.  Takes a ulong argument.
 *     # - indicates that the textual name should be used if known.
 */

int vines_printf_bigz (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
		       boolean rjust, int width, int size, int hash,
		       int precision)
{
    int length, d, charcount;
    ulong n;
    char *f;

    charcount = 0;
    n = va_arg(*argp,ulong);
    if (hash) {		/* # flag = Use node name if known */
	f = vines_lookup_name(n, 1);
    } else {
	f = NULL;
    }
    if (*cp) {
	if (f != NULL)
	    length = sprintf(*cp,"%s",f);
	else if (vines_decimal_addresses)
	    length = sprintf(*cp,"%010u",n);
	else
	    length = sprintf(*cp,"%08x",n);
	*cp += length;
    } else {
	if (f != NULL)
	    length = _ttyprintf(tty,"%s",f);
	else if (vines_decimal_addresses)
	    length = _ttyprintf(tty,"%010u",n);
	else
	    length = _ttyprintf(tty,"%08x",n);
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/**********************************************************************
 *
 *			    MISCELLANEOUS
 *
 **********************************************************************/

/*
 * vpath_id
 *
 * Take a path structure and print a unique identifier by concatenating
 * the neighbor's name, the interface name, and the MAC layer address.
 * All three components are necessary because the neighbor might be
 * something like a cisco router running decnet.
 */
char *vpath_id (vinespathtype *path)
{
    vinesneighbortype *neighbor;
    static char buffer[80];

    neighbor = path->neighbor;

    sprintf(buffer, "%#z-%s-", neighbor->net, neighbor->host,
	    path->idb->phys_idb->short_namestring);
    vines_dlc(buffer+strlen(buffer), &path->dlc_addr);
    return(buffer);
}

ulong vines_extract_serverid (vinesaddress *address)
{
    switch (address->family) {
      case VINES_ADDR_NATIVE:
	return(GET2WD(address->f.vip.nnet));
	
      default:
	return(0);
    }
}

vinesaddress *vines_create_port (
    int family,
    ushort port)
{
    static vinesaddress address;

    memset(&address, 0, sizeof(vinesaddress));
    switch (family) {
      case VINES_ADDR_NATIVE:
	address.family = family;
	PUT2WD(address.f.vip.nnet, vines_network);
	address.f.vip.host = vines_host;
	address.f.vip.port = port;
	break;
	
      default:
	break;
    }
    return(&address);
}

char *vines_printf_address (char *string, vinesaddress *address)
{
    switch (address->family) {
      case VINES_ADDR_NATIVE:
	sprintf(string, "V:%08x:%04x:%04x", GET2WD(address->f.vip.nnet),
		address->f.vip.host, address->f.vip.port);
	break;

      default:
	sprintf(string, "%20s", "unknown family");
	break;
    }
    return(string);
}

/*
 * vtimer_dump_path_queue
 *
 * Run a timer queue.  For each entry, print out when the action will
 * occur, and to whom it will occur.  "What" will occur is a function of
 * the queue itself, not of the entry.
 */
void vtimer_dump_path_queue (
    mgd_timer *head,
    char *lead_in,
    char *empty_string)
{
    vinespathtype *path;
    int header_length;
    mgd_timer *timer;
    char temp[40];

    /*
     * Empty ?
     */
    if (!mgd_timer_running(&vrtp_rc_timer)) {
	printf("\n%s%s", lead_in, empty_string);
	return;
    }


    /*
     * First entry
     */
    timer = mgd_timer_first_running(head);
    path = mgd_timer_context(timer);
    printf("\n%s%-10s %s", lead_in,
	   vtimer_pretty_print(timer, "zero", temp), vpath_id(path));

    /*
     * Remaining entries
     */
    header_length = strlen(lead_in);
    while ((timer = mgd_timer_next_running(timer)) != NULL) {
	path = mgd_timer_context(timer);
	printf("\n%*s%-10s %s", header_length, " ",
	       vtimer_pretty_print(timer, "zero", temp), vpath_id(path));
    }
}
