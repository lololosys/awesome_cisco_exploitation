/* $Id: iprouting_init.c,v 3.6.28.8 1996/08/28 12:52:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_init.c,v $
 *------------------------------------------------------------------
 * iprouting_init.c -- initialize IP Routing Protocol support.
 *
 * 20-August-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_init.c,v $
 * Revision 3.6.28.8  1996/08/28  12:52:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.28.7  1996/08/23  21:51:47  cire
 * CSCdi65276:  SCHED: Stack for process IP Background running low, 28/2000
 * Branch: California_branch
 * OSPF (ospf_route_adjust) invokes its state machine and chew mondo stack
 * (hungry little sucker isn't it).  It does this on IP background's stack
 * which was too small.  Bump the stack up to something more reasonable
 * considering OSPF's appetite.  (Note: OSPF someday will get rewritten
 * to not have this obnoxious behaviour).
 *
 * Revision 3.6.28.6  1996/05/21  09:53:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.28.5  1996/05/13  20:10:58  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.6.28.4  1996/05/07  00:14:27  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.6.28.3  1996/04/27  05:53:20  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.8.3  1996/04/26  01:37:49  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.8.2  1996/04/17  03:06:02  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.8.1  1996/02/04  06:29:00  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.6.28.2  1996/04/15  21:18:15  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.28.1  1996/03/18  20:28:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.4.1  1996/03/07  09:49:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/01/15  17:18:45  misko
 * CSCdi37400:  2511 passes packets after ipcp completes but before
 * reporting int up
 * Allow PPP to notify IP when IP comes up.  Since PPP can have NCP's that
 * come and go, and since IP routes may be installed when PPP negotiated,
 * we need to be able to tell IP to propogate those routes.  Otherwise
 * a lot of traffic gets dropped.  Take advantage of existing registrys
 * to notify IP when PPP thinks it is up.  In addition, fix network.c so
 * that when line protocol comes up, route propogation is not delayed by
 * the keepalive period.
 *
 * Revision 3.5  1995/12/17  18:31:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/07  22:29:48  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/19  06:07:48  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Return indication from route_add_neighbor() as whether routing tables
 * were modified in order to avoid unnecessary route adjustment.
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.2  1995/11/17  17:34:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:06:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/23  20:59:16  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.5  1995/10/04  22:15:27  rchandra
 * CSCdi41548:  BGP : wrong nexthop could be advertised to EBGP peers
 * - while doing the last resort brute force nexthop calculation, pass the
 *   mask to iprouting_first_hop to pick the correct entry.
 *
 * Revision 2.4  1995/09/19  18:37:55  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.3  1995/06/23  18:47:56  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  01:07:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:07:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "boxtypes.h"
#include "subsys.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "../os/chunk.h"
#include "../h/exception.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../iprouting/ospf_public.h"
#include "../ui/debug.h"
#include "../ip/ip_debug.h"
#include "../iprouting/route.h"
#include "../iprouting/sr_egpmib2.h"
#include "../iprouting/sr_bgpmib.h"
#include "../iprouting/sr_ospfmib.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../ip/ipfast.h"
#include "../parser/parser_actions.h"
#include "../iprouting/iprouting_actions.h"
#include "../iprouting/irdp.h"
#include "../routing/routing_registry.h"
#include "../iprouting/ip_routing.h"

/*
 * Extern declarations that should be inside include files
 */
extern void irdp_init(void);
extern void iprouting_parser_init(void);
extern void ip_mobile_init(void);

/*
 * IP routing global defines
 */
chunk_type *rdb_chunks;


/*
 * IP Routing subsystem header
 */

#define IPROUTING_MAJVERSION 1
#define IPROUTING_MINVERSION 0
#define IPROUTING_EDITVERSION  1

SUBSYS_HEADER(iprouting, IPROUTING_MAJVERSION, IPROUTING_MINVERSION,
	      IPROUTING_EDITVERSION, iprouting_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, routing, ipflow_pas_fs, ip_rsp_fs",
	      "req: iphost, routing");


/*
 * ip_background
 */
static process ip_background (void)
{
    sys_timestamp wakeup_time;
    ulong major, minor;
    idbtype *idb;
    ulong message, value;
    void *pointer;

    /*
     * Setup all the hooks for this process.
     */
    reg_add_route_adjust_msg(ip_background_pid, "ip_background");
    TIMER_START(wakeup_time, ONEMIN);
    process_watch_timer(&wakeup_time, ENABLE);

    /*
     * Work, work, work...
     */
    while (ip_router_running) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		ip_redirect_age_cache();
		TIMER_UPDATE(wakeup_time, ONEMIN);
		break;

	      case MESSAGE_EVENT:
		while (process_get_message(&message, &pointer, &value)) {
		    switch (message) {
		      case MSG_ROUTE_ADJUST:
			idb = pointer;
			ip_route_adjust(idb);
			break;
		      case MSG_SOCKET_ADJUST:
 			fixup_ipsockets();
  			break;
		      default:
			errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), message,
			       pointer, value);
			break;
		    }
		}
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }

    /*
     * Clean up, and then terminate.
     */
    process_watch_timer(&wakeup_time, DISABLE);
    reg_delete_route_adjust_msg(ip_background_pid);
    ip_background_pid = 0;
    process_kill(THIS_PROCESS);
}

/*
 * iprouting_route_adjust
 * Send a message to the background process to do a route adjust
 */
static void iprouting_route_adjust (idbtype *idb)
{
    if (ip_background_pid)
        process_send_message(ip_background_pid, MSG_ROUTE_ADJUST, idb, 0);
}

/*
 * iprouting_first_hop_inline
 * Set the mask to 0 when calling iprouting_first_hop
 */

static inline idbtype *
iprouting_first_hop_inline (ipaddrtype *address, ipaddrtype destination,
			    ulong flags)
{
    return(iprouting_first_hop(address, destination, 0, ROUTINGDEPTH, flags,
			       NULL));
}

/*
 * iprouting_update_first_hop_reg
 * Conditionally register the first_hop stub registries.
 */

static void iprouting_update_first_hop_reg (boolean ip_routing)
{
    reg_delete_ip_first_hop();
    if (ip_routing && systeminit_flag) {
	reg_add_ip_first_hop(iprouting_first_hop_inline,
			     "iprouting_first_hop");
    } else {
	reg_add_ip_first_hop(first_hop_no_routing, "first_hop_no_routing");
    }
}

/*
 * iprouting_restarted
 * Initialize first_hop stub registries.
 * Establish IP static routes.
 */

static void iprouting_restarted (void)
{
    /*
     * Note: ordering is significant here
     */
    iprouting_update_first_hop_reg(ip_router_running);
    iprouting_background_onoff(ip_router_running);
}

/*
 * iprouting_onoff
 * If iprouting was really turned on or off, update first_hop stub
 * registries.  Turn on/off background process.
 */

static void iprouting_onoff (boolean ip_routing, boolean nochange)
{
    if (nochange)
	return;
    if (!ip_routing)
	iprouting_shutdown();
    else if (!ip_background_pid)
	ip_background_pid = process_create(ip_background, "IP Background",
					   IPROUTING_STACK, PRIO_NORMAL);
    iprouting_update_first_hop_reg(ip_routing);
    iprouting_background_onoff(ip_routing);
}

/*
 * iprouting_init_idb
 * Initialize IP specific parts of the idb
 */

static void iprouting_init_idb (idbtype *idb, boolean setup_hwidb)
{

    queue_init(&idb->ip_helpernetQ, 0);
    queue_init(&idb->ip_mhelpermapQ, 0);
    if (setup_hwidb) {
	idb->hwptr->ip_routecache = TRUE;
	reg_invoke_ip_platform_default(idb->hwptr, TRUE);
    }

    /* init OSPF idb fields */
    idb->ospf_info = NULL;

    /*
     * Allocate IP routing information.
     */
    if (!idb->ip_info) {
	idb->ip_info = malloc_named(sizeof(ipinfotype), "IDB: IP Routing");
    }
}

/*
 * iprouting_register_thyself
 */
static void iprouting_register_thyself (void)
{

    /*
     * Set up debugging flags
     */
    ip_debug_init();
    parser_add_router(RMATCH_IP, parser_ip_router);

    /*
     * Register some functions with the REG_SYS super registry
     */
    reg_add_sw_idb_init(iprouting_init_idb, "iprouting_init_idb");
    reg_add_clear_routes(table_init, "table_init");
    reg_add_setup_init(iprouting_setup_init, "iprouting_setup_init");
    reg_add_setup_global(iprouting_setup_global, "iprouting_setup_global");
    reg_add_setup_end(iprouting_setup_end, "iprouting_setup_end");
    reg_add_clear_idb_routes(table_prune_idb, "table_prune_idb");
    reg_add_swif_erase(ip_static_deleteidb, "ip_static_deleteidb");
    reg_add_restarted(iprouting_restarted, "iprouting_restarted");
    reg_add_iprouting_route_adjust(iprouting_route_adjust, 
    				   "iprouting_route_adjust");

    /*
     * Register some functions with the REG_IP registry
     */
    reg_add_ip_routing(iprouting_onoff, "iprouting_onoff");
    reg_add_ip_add_static_route(ip_addstatic, "ip_addstatic");
    reg_add_ip_delete_per_user_route(ip_delete_per_user_route, 
				     "ip_delete_user_route");
    reg_add_ip_delete_static_route(ip_deletestatic, "ip_deletestatic");
    reg_add_ip_defaultnetwork_command(ip_defaultnetwork_command, 
					"ip_defaultnetwork_command");
    reg_add_ip_address_command(ip_address_command,
					"ip_address_command");
    reg_add_snapshot_start_aging_routes(LINK_IP, ip_route_start_aging,
					"ip_route_start_aging");
    reg_delete_ip_get_netsmask();
    reg_add_ip_get_netsmask(iprouting_get_netsmask, "iprouting_get_netsmask");
    reg_add_iprouting_delete_conn_route(iprouting_delete_conn_route,
				   "iprouting_delete_conn_route");
    reg_add_iprouting_delete_conn_route_int(iprouting_delete_conn_route_int,
				   "iprouting_delete_conn_route_int");
    reg_add_iprouting_delete_route(iprouting_delete_route,
				   "iprouting_delete_route");
    reg_add_iprouting_gdp_default_ndb_prio(iprouting_gdp_default_ndb_prio,
					   "iprouting_gdp_default_ndb_prio");
    reg_add_ip_add_p2p_neighbor_route(route_add_neighbor, "route_add_neighbor");
    reg_add_iprouting_irdp_send_report(iprouting_irdp_send_report,
				       "iprouting_irdp_send_report");
    reg_add_iprouting_via_nonsubnet(iprouting_via_nonsubnet,
				    "iprouting_via_nonsubnet");
    reg_add_ip_secondary(ip_secondary, "ip_secondary");
    reg_add_iprouting_snmp_rtlookup_exact(iprouting_snmp_rtlookup_exact,
					  "iprouting_snmp_rtlookup_exact");
    reg_add_iprouting_snmp_rtlookup_lexnext(iprouting_snmp_rtlookup_lexnext,
					    "iprouting_snmp_rtlookup_lexnext");
    reg_add_iprouting_conn_plimit_exceed(iprouting_conn_plimit_exceed,
					 "iprouting_conn_plimit_exceed");
    reg_add_default_iprouting_pdb_init(iprouting_pdb_init_default,
				       "iprouting_pdb_init_default");

    /*
     * Register some functions in the REG_ROUTING registry
     */
    reg_add_route_map_set_nvgen(ip_routing_route_map_set_nvgen,
				  "ip_routing_route_map_set_nvgen");
    reg_add_route_map_match_nvgen(ip_routing_route_map_match_nvgen,
				  "ip_routing_route_map_match_nvgen");
    reg_add_route_map_show_match(ip_routing_show_route_map_match,
				  "ip_routing_show_route_map_match");
    reg_add_route_map_show_set(ip_routing_show_route_map_set,
				  "ip_routing_show_route_map_set");
}

/*
 * iprouting_init
 * Initialize the IP protocol handling
 */

void iprouting_init (subsystype *subsys)
{
    idbtype *idb;

    iprouting_register_thyself();

    /*
     *
     */
    FOR_ALL_SWIDBS(idb)
	ip_rxtypes_update(idb);

    /*
     * Initialize the queues
     */
    ip_forward_nd = TRUE;		/* forward ND protocol */
    udp_forward_init();		        /* init UDP forwarding variables */

    if (router_enable && (boxtype != BOXTYPE_TS))
	ip_router_running = TRUE;	/* Set default state if router */

    if (ip_router_running)
	ip_background_pid = process_create(ip_background, "IP Background",
					   IPROUTING_STACK, PRIO_NORMAL);

    /*
     * Initialize IP route structures
     */
    rdb_chunks = chunk_create(sizeof(rdbtype), RDB_CHUNK_MAX,
			      CHUNK_FLAGS_DYNAMIC,
			      NULL, 0,
			      "IP RDB Chunk");
    if (!rdb_chunks)
	crashdump(0);

    /*
     * Initialize some applications that run only on IP.
     */
    net_init();			/* init routing data structures */

    /*
     * Initialize IP specific interface variables to default values
     * Only when the system is starting up do we enable IP by default.
     * This allows us to use BootP, RARP, etc. to resolve addresses.
     */
    FOR_ALL_SWIDBS(idb)
	iprouting_init_idb(idb, TRUE);

    /*
     * Initialize the IP routing protocols.
     */
    irdp_init();			/* init irdp */
    ip_mobile_init();			/* init ip mobility */

    /*
     * Parser chain initialization
     */
    iprouting_parser_init();
}
