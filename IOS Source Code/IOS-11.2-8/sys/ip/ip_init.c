/* $Id: ip_init.c,v 3.14.10.16 1996/09/05 23:59:26 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_init.c,v $
 *------------------------------------------------------------------
 * ip_init.c -- initialize IP support.
 *
 * 20-August-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_init.c,v $
 * Revision 3.14.10.16  1996/09/05  23:59:26  rbadri
 * CSCdi64212:  martian table bogusly externed in a .c file.
 * Branch: California_branch
 * Use registry to adjust the martian table. Created a new boolean
 * to mark the validity of the entry.
 *
 * Revision 3.14.10.15  1996/08/28  14:33:12  thille
 * CSCdi67083:  Message externs need a cleanup.
 * Branch: California_branch
 * Fix mangled file.
 *
 * Revision 3.14.10.14  1996/08/28  12:49:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.10.13  1996/06/18  01:44:55  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.14.10.12  1996/06/12  16:47:28  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - in init, don't set ip_enabled on interfaces that
 * won't get configured by the nvram.
 * Branch: California_branch
 *
 * Revision 3.14.10.11  1996/05/21  09:51:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.14.10.10  1996/05/17  11:24:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.14.2.4  1996/05/14  06:19:20  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.14.2.3  1996/05/05  23:11:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.14.2.2  1996/04/03  14:41:08  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.2.1  1996/03/22  18:53:55  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.14.10.9  1996/05/10  22:57:41  hampton
 * Fix process_wait_on_netsareup(), which relied on undocumented side
 * effect of process_wait_for_event_timed().  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.14.10.8  1996/05/08  20:55:51  lwei
 * CSCdi57099:  rsp-boot-m image can not be built due to the fix to
 * CSCdi56122
 * Branch: California_branch
 *
 * Revision 3.14.10.7  1996/05/08  01:47:00  lwei
 * CSCdi56122:  Tunnel interface default MTU should not show up in nvgen
 * output
 * Branch: California_branch
 *
 * Revision 3.14.10.6  1996/05/04  01:02:17  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.14.10.5  1996/04/24  16:20:19  ahh
 * CSCdi55543:  NAT: modularity problems
 * Fix build failures (Doh!)
 * Branch: California_branch
 *
 * Revision 3.14.10.4  1996/04/17  13:41:40  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.14.10.3  1996/04/15  21:15:52  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.14.10.2  1996/04/11  23:59:58  ebuell
 * CSCdi52592:  Performance degradation for 4500
 * Branch: California_branch
 * CSCdi52592:  Performance degradation for 4500
 *              Changed call to CPU consuming call to global variable
 *               reference in ip_bridging_idb.
 *
 * Revision 3.14.10.1  1996/03/18  20:16:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.6.5  1996/03/16  06:57:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.6.4  1996/03/13  01:37:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.6.3  1996/03/07  09:45:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.6.2  1996/02/20  00:59:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/02/25  03:52:25  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.13  1996/02/02  01:10:43  widmer
 * CSCdi48042:  Change functions in ip/iptrace.c to static
 *
 * Revision 3.12  1996/01/26  17:25:10  asb
 * CSCdi46322:  Should return async-bootp dns-server instead of routers
 * DNS server
 *
 * Revision 3.11  1996/01/25  11:19:16  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.10  1996/01/24  20:14:54  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.9.6.1  1996/01/24  22:20:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9  1996/01/11  02:26:31  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.8  1996/01/10  02:22:04  raj
 * CSCdi40929:  traceroute fails, ping works. port unreachables not sent
 * Allow unreachables within the first 3 minutes and fix timer code to
 * remove ip_icmpokay variable.
 *
 * Revision 3.7  1995/12/17  18:29:29  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/13  22:57:23  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.5  1995/12/07  22:29:04  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/19  06:05:57  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Don't send gratuitous ARP on the interface the peer is on.
 * Rename ip_send_gratuitous_arps() as ip_arp_send_gratuitous_arps().
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.3  1995/11/17  09:33:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:55:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  21:04:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/23  20:57:14  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.11  1995/10/16  19:39:59  pst
 * CSCdi41543:  IP tunnel address not wholly erased
 *
 * Revision 2.10  1995/09/18  19:35:42  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.9  1995/09/12  21:10:33  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.8  1995/09/11  23:49:11  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.7  1995/07/12 07:18:40  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.6  1995/07/09  21:09:48  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.5  1995/07/07  05:26:23  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.4  1995/06/23 18:47:52  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/19  04:08:39  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.2  1995/06/19  01:06:24  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:57:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "linktype.h"
#include "config.h"
#include "subsys.h"
#include "config_register.h"
#include "../snmp/snmp_api.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../snmp/snmp_registry.h"
#include "ip_registry.h"
#include "ip.h"
#include "logger.h"
#include "../tcp/tcp.h"
#include "../ui/debug.h"
#include "access.h"
#include "ipaccess.h"
#include "ipoptions.h"
#include "ip_debug.h"
#include "ip_media.h"
#include "ipfast.h"
#include "exception.h"
#include "../if/network.h"
#include "../iprouting/route.h"
#include "../ipmulticast/mroute.h"
#include "../ip/tunnel.h"
#include "../tcp/tcpcompress.h"
#include "parser.h"
#include "../util/random.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ip/msg_ip.c"		/* Not a typo, see logger.h */
#include "../if/arp.h"

/*
 * Includes for ip_init() calls.
 */
#include "../os/boot.h"
#include "../ip/tftp.h"
#include "file.h"
#include "probe.h"

/*
 * Includes for ip parser initialization
 */
#include "../parser/parser_actions.h"
#include "../ip/ip_actions.h"
#include "../ip/ipaddress.h"

/*
 * Storage declarations
 */

queuetype echoQ;			/* ICMP Echo queue */
watched_queue *IPpacketQ;		/* IP input queue */
queuetype reassemblyQ;			/* reassembly queue */
queuetype IPsocketQ;			/* IP socket queue */
ipparamtype ipdefaultparams;		/* default paramaters for ipwrite */
ipparamtype iprouteparams;		/* routing paramaters for ipwrite */
uchar ip_secopt_bytes;			/* bytes in default security option */
uchar ip_secsys_low;			/* low  end system security level */
uchar ip_secsys_high;			/* high end system security level */
uchar ip_secsys_auth_low;		/* low  end system authorization */
uchar ip_secsys_auth_high;		/* high end system authorization */
sys_timestamp ip_icmptimer;		/* timer for start up */
boolean ip_forward_sdns;                /* forward SNDS protocol */
boolean ip_forward_nd;			/* forward ND protocol */
boolean ip_forward_udp;			/* forward UDP protocol */
boolean ip_spanning;		        /* use ST when forwarding UDP */
boolean ip_spanning_anybcast;           /* Accept any b'cast when flooding */
boolean ip_udp_fast_flood;              /*  and do it at interrupt level */
boolean ip_udp_flood_it;                /* logical and of previous 3 vars */
boolean ip_srcroute;			/* accept source route options? */
boolean ip_gratuitous_arps;		/* generate gratuitous arps */
boolean ip_router_running;		/* IP router is enabled */
boolean ip_mrouter_running;		/* IP multicast router is enabled */
boolean domain_enable;		        /* Domain-name hostname lookups */
boolean subnetzero_enable;	        /* Allow use of subnet zero */
boolean netsareup;			/* TRUE if addresses are known */
watched_boolean *watched_netsareup;
ipaddrtype rsrb_fst_name;		/* this peer's name */
					/* we can have ip without rsrb, but */
					/* not the other way around, so */
					/* allocate storage here to avoid */
					/* stub */ 
boolean ip_nat_anywhere;		/* TRUE if enabled on any interface */
ip_traffic_t ip_traffic;

/*
 * Variables used by SNMP support
 */
ulong ip_checkpointvalue;		/* for IP accounting checkpoint  */
ulong accessfailcnt;		/* pkts dropped due to access control*/


/*
 * IP subsystem header
 */

#define IP_MAJVERSION 1
#define IP_MINVERSION 0
#define IP_EDITVERSION  1

SUBSYS_HEADER(iphost, IP_MAJVERSION, IP_MINVERSION, IP_EDITVERSION,
	      ip_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);


/*
 * process_wait_on_netsareup
 *
 * Wait for the netsareup flag to become true.  This is nothing more than
 * a blocking test on the system initialization flag.
 */
void process_wait_on_netsareup (ulong delta_time)
{
    sys_timestamp wakeup_time;

    /* Already started? */
    if (process_get_boolean(watched_netsareup))
	return;

    /*
     * Nope, wait for it.  See if the current event list can be saved.
     * If not, poll intermittently.
     */
    TIMER_START(wakeup_time, delta_time);
    while (!process_push_event_list(NULL)) {
	if (process_get_boolean(watched_netsareup) || AWAKE(wakeup_time))
	    return;
	process_sleep_for(ONESEC);
    }

    /*
     * Current event list saved.  Install a watch on the netsareup
     * variable and go to sleep.
     */
    process_watch_boolean(watched_netsareup, ENABLE, TRUE);
    process_watch_timer(&wakeup_time, ENABLE);
    while (!process_get_boolean(watched_netsareup) && !AWAKE(wakeup_time)) {
	process_set_wakeup_reasons((BOOLEAN_EVENT|TIMER_EVENT), ONE_SHOT);
	process_wait_for_event();
    }
    process_watch_timer(&wakeup_time, DISABLE);
    process_watch_boolean(watched_netsareup, DISABLE, TRUE);
    process_pop_event_list(NULL);
}

/*
 * ip_on_idb
 * Return TRUE if IP is enabled on the given interface
 * IP is a special case beacause IP can be running on an interface even if
 * IP routing is not running on the box.  Therefore, don't check whether
 * there is a router_running like all the other protocols.
 */

boolean ip_on_idb (idbtype *idb)
{
    return(idb->ip_enabled);
}

/*
 * ip_enable_test
 * Return TRUE if IP *routing* is enabled on the system.
 * Called by SERVICE_PROTO_RUNNING.
 * If you are doing host-like things, think of another way to do it.
 * This treats SERVICE_PROTO_RUNNING for LINK_IP in the same way as
 * all the other protocols.
 */

static boolean ip_enable_test (void)
{
    return(ip_router_running);
}

/*
 * ip_routing_idb
 * Return TRUE iff IP packets inbound on this interface should be
 * routed.
 */

static boolean ip_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the ip route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route ip" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_IP_RBI);
    } else {
	/*
	 * Otherwise, just verify that ip routing is enabled,
	 * and this interface is enabled for ip.  This latter
	 * is set by the "ip address" command.
	 */
	return(ip_router_running && swidb->ip_enabled);
    }
}

/*
 * ip_bridging_idb
 * Return TRUE iff IP packets inbound on this interface should be
 * bridged.
 */

static boolean ip_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the ip
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route ip" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_IP_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_IP_RBI));
    } else {
        if (router_enable)
	   /*
	    * To support SRB and the archaic transparent bridging
	    * behaviour just check for the absence of ip routing.
	    */
	   return(!ip_router_running);
        else
	   return (FALSE);
    }
}

/*
 * ip_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent/Integrated 
 * Routing and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void ip_rxtypes_update (idbtype *swidb)
{
    boolean rte_ip, br_ip;


    if (global_crb_enable || global_irb_enable) {
        rte_ip = ip_routing_idb(swidb);
	br_ip = ip_bridging_idb(swidb);
    } else {
        rte_ip = ip_router_running;
	br_ip = !rte_ip;
    }

    if (rte_ip) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_DODIP);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_DODIP);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SAP_IP);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_JUMBO_IP);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_IPUNC);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_DODIP);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_DODIP);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SAP_IP);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_JUMBO_IP);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_IPUNC);
    }

    if (br_ip) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_DODIP);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_DODIP);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_IP);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_JUMBO_IP);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_IPUNC);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_DODIP);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_DODIP);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_IP);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_JUMBO_IP);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_IPUNC);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * ip_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void ip_showproto (idbtype *idb)
{
    if (idb == NULL) {
	if (RUNNING(LINK_IP))
	    printf("\n  Internet Protocol routing is enabled");
	return;
    }
    if (idb->ip_enabled)
	printf("\n  Internet address is %i%m",
					idb->ip_address, idb->ip_nets_mask);
}

/*
 * show_iptraffic
 * Summary of IP protocol traffic.
 * We show ARP and Probe here since they are IP-centric.
 */

void show_iptraffic (parseinfo *csb)
{
    automore_enable(NULL);
    ip_show_traffic();
    icmp_show_traffic();
    reg_invoke_ip_show_traffic();
    arp_show_traffic();
    automore_disable();
}

/*
 * ip_init_idb
 * Initialize IP specific parts of the idb
 */

static void ip_init_idb (idbtype *idb, boolean setup_hwidb)
{
    idbtype *tmp_idb;

    idb->ip_enabled = FALSE;
    idb->ip_mtu = def_proto_mtu(idb, LINK_IP);
    idb->ip_maskreply = FALSE;
    idb->ip_unreach = TRUE;
    idb->ip_routecache_same = idb->ip_routecache_same_def;
    idb->ip_redirect = TRUE;
    idb->ip_directbcast = -1;
    idb->ip_sec_default    = TRUE;
    idb->ip_sec_implicit   = TRUE;
    idb->ip_sec_multilevel = FALSE;
    idb->ip_sec_extended   = IP_ESO_DENY;
    idb->ip_sec_cipso      = IP_CIPSO_DENY;
    idb->ip_sec_instyle    = SECSTYLE_NONE;
    idb->ip_sec_outstyle   = SECSTYLE_NONE;
    idb->ip_sec_low        = SECLVL_UNCLASSIFIED;
    idb->ip_sec_high       = SECLVL_UNCLASSIFIED;
    idb->ip_sec_def_class  = SECLVL_UNCLASSIFIED;
    idb->ip_sec_auth_low   = AUTH_GENSER;
    idb->ip_sec_auth_high  = AUTH_GENSER;
    idb->ip_sec_def_auth   = AUTH_GENSER;

    if (idb->hwptr->status & IDB_ETHER) {

        idb->ip_enctype    = ET_ARPA;
        idb->arp_arpa      = idb->arp_arpa_def = TRUE;

    } else if (idb->hwptr->status & IDB_802ONLY) {

        idb->ip_enctype    = ET_SNAP;
	idb->arp_snap      = idb->arp_snap_def = TRUE;
    }

    queue_init(&idb->ip_sec_nleso, 0);
    queue_init(&idb->ip_sec_aeso, 0);
    idb->ip_maskid = random_gen() ^ (ulong) idb;
    idb->ip_quench_interval = ONESEC;
    idb->ip_unreach_interval = ONESEC/2;
    idb->ip_broadcast = (configregister & CFG_ZEROS) ?
	IP_BSD_LIMITED_BROADCAST : IP_LIMITED_BROADCAST;
    queue_init(&idb->ip_addressQ, 0);
    queue_init(&idb->ip_gdp_proxyQ, 0);
    if (setup_hwidb) {
	idb->hwptr->ip_routecache = FALSE;

	/*
	 * Set up ip_fast_flags.
	 */
	FOR_ALL_SWIDBS(tmp_idb) {
	    if (tmp_idb->ip_input_accesslist) 
		idb->hwptr->ip_fast_flags |= IP_FAST_INPUTACCESSLISTS;
	    if (tmp_idb->ip_info && tmp_idb->ip_info->policy_enabled)
		idb->hwptr->ip_fast_flags |= IP_FAST_POLICY;
	}
    }

    /*
     * Allocate IP host information.
     */
    if (!idb->iphost_info) {
	idb->iphost_info = malloc_named(sizeof(iphostinfotype), "IDB: IP Host");
    }
}

/*
 * ip_tcp_compress_stub
 * Handle compression attempt when TCP compression subsystem is not around.
 */

static paktype *ip_tcp_compress_stub (
    hwidbtype *idb,
    paktype *pak,
    boolean compress_cid,
    thcdbtype *thcdb)
{
    retbuffer(pak);
    return(NULL);
}

/*
 * ip_tcp_uncompress_stub
 * Handle uncompression attempt when TCP compression subsystem is not around.
 */

static paktype *ip_tcp_uncompress_stub (paktype *pak, hwidbtype *idb,
				 thcdbtype *thcdb)
{
    retbuffer(pak);
    return(NULL);
}

/*
 * ip_register_thyself
 */
static void ip_register_thyself (void)
{

    /*
     * Set up debugging flags
     */
    ip_debug_init();
    ip_media_registry();

    /*
     * make sure noone thinks ip background is running
     */
    ip_background_pid = 0;

    /*
     * Initialize global IP parser support
     */
    parser_add_protoaddr(LINK_COMPRESSED_TCP,	PMATCH_COMPRESSED_TCP,
			 "compressedtcp",	"Compressed TCP", 0, ADDR_IP);
    parser_add_protoaddr(LINK_IP,		PMATCH_IP PMATCH_DGRAM,
			 "ip",			"IP", 0, ADDR_IP);
    parser_add_address(ADDR_IP, ip_addr_short_help, match_ip_addr);

    /*
     * Register some functions with the REG_SYS super registry
     */
    reg_add_send_quench(LINK_IP, send_quench, "send_quench");
    reg_add_raw_enqueue(LINK_IP, internet_enqueue, "internet_enqueue");
    reg_add_sw_idb_init(ip_init_idb, "ip_init_idb");
    reg_add_proto_on_swidb(LINK_IP, ip_on_idb, "ip_on_idb");
    reg_add_swif_erase(ip_interface_erase, "ip_interface_erase");
    reg_add_routed_proto_on_swidb(LINK_IP, ip_on_idb, "ip_on_idb");
    reg_add_printf((long)'I', ip_printf, "ip_printf");
    reg_add_printf((long)'i', ip_printf, "ip_printf");
    reg_add_printf((long)'M', ip_mask_printf, "ip_mask_printf");
    reg_add_printf((long)'m', ip_mask_printf, "ip_mask_printf");
    reg_add_stay_shutdown(ip_stay_shutdown, "ip_stay_shutdown");
    reg_add_proto_running(LINK_IP, ip_enable_test, "ip_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_IP, ip_bridging_idb,
				    "ip_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_IP, ip_routing_idb,
				   "ip_routing_idb");
    reg_add_rxtypes_update(ip_rxtypes_update,"ip_rxtypes_update");
    reg_add_showproto(ip_showproto, "ip_showproto");
    reg_add_showproto_brief(ip_show_interface_brief,
			    "ip_show_interface_brief");
    reg_add_autoaddr_okay(ipisresolved, "ipisresolved");
    reg_add_autoaddr_load(ip_setaddresses, "ip_setaddresses");
    reg_add_ip_forus(ip_forus, "ip_forus");
    reg_add_setup_int(ip_setup_int, "ip_setup_int");
    reg_add_if_statechange_complete(ip_fixsocket_address, 
				    "ip_fixsocket_address");
    reg_add_arp_refresh(LINK_IP, ip_arp_refresh, "ip_arp_refresh");
    reg_add_arp_fscache_invalidate(LINK_IP, ip_arp_fscache_invalidate,
				   "ip_arp_fscache_invalidate");
    reg_add_print_network_header(LINK_IP, ip_print_network_header,
				 "ip_print_network_header");
    reg_add_determine_fair_queue_flow_id (LINK_IP, ip_fair_queue_flow_id,
				   "ip_fair_queue_flow_id");
    reg_add_determine_RED_precedence (LINK_IP, ip_red_precedence,
				   "ip_red_precedence");
    reg_add_ip_adjust_martian(ip_adjust_martian, "ip_adjust_martian");

    /*
     * Register some functions with the IP super registry.
     */
    reg_add_default_ip_enqueue(default_ip_enqueue, "default_ip_enqueue");
    reg_add_default_ip_forward_dbcast(ip_forward_dbcast, "ip_forward_dbcast");
    reg_add_ip_enqueue(ICMP_PROT, icmp_process, "icmp_process");
    reg_add_ip_enqueue(ND_PROT, nd_forward, "nd_forward");
    reg_add_ip_enqueue(SDNS0_PROT, sdns_forward, "sdns_forward");
    reg_add_ip_enqueue(SDNS1_PROT, sdns_forward, "sdns_forward");

    reg_add_ip_first_hop(first_hop_no_routing, "first_hop_no_routing");
    reg_add_ip_slarp(ip_slarp_resolve, "ip_slarp_resolve");
    reg_add_ip_send_echo(send_echo, "send_echo");
    reg_add_ip_trace_sendprobe(ip_trace_sendprobe, "ip_trace_sendprobe");
    reg_add_ip_ifbroadcast(ip_ifbroadcast, "ip_ifbroadcast");
    reg_add_ip_transit_bridging(ip_transit_bridging, "ip_transit_bridging");
    reg_add_nv_add_tcp_port(nv_add_tcp_port, "nv_add_tcp_port");
    reg_add_nv_add_udp_port(nv_add_udp_port, "nv_add_udp_port");
    reg_add_printf_tcp_port(printf_tcp_port, "printf_tcp_port");
    reg_add_printf_udp_port(printf_udp_port, "printf_udp_port");
    reg_add_ip_get_netsmask(ip_get_netsmask, "ip_get_netsmask");
    reg_add_ip_arp_send_gratuitous_arps(ip_arp_send_gratuitous_arps,
				    "ip_arp_send_gratuitous_arps");
    reg_add_ip_defaultgw(ip_defaultgw, "ip_defaultgw");
    reg_add_ip_address_duplicate(ip_address_duplicate, "ip_address_duplicate");
    reg_add_ip_set_per_user_acl(ip_set_per_user_acl, "ip_set_per_user_acl");
    reg_add_ip_gdpclient_enqueue(retbuffer, "retbuffer");
    reg_add_uncompress_tcpip_header(LINK_IP, ip_tcp_uncompress_stub,
				    "ip_tcp_uncompress_stub");
    reg_add_compress_tcpip_header(LINK_IP, ip_tcp_compress_stub,
				  "ip_tcp_compress_stub");
}

/*
 * ip_init
 * Initialize the IP protocol handling
 */

void ip_init (subsystype *subsys)
{
    idbtype *idb;

    ip_register_thyself();

    /*
     * Initialize some IP global values
     */
    domain_enable = TRUE;
    subnetzero_enable = FALSE;

    /*
     * Initialize traffic tables.
     */
    memset(&ip_traffic, 0, sizeof(ip_traffic));
    memset(&icmp_traffic, 0, sizeof(icmp_traffic));
    memset(&udp_traffic, 0, sizeof(udp_traffic));

    /*
     * Initialize IP specific interface variables to default values
     * Only when the system is starting up do we enable IP by default.
     * This allows us to use BootP, RARP, etc. to resolve addresses.
     *
     * Only enable ip on the nvgenable interfaces - a better check
     * would be to use reg_invoke_interface_command_ok, but that gets
     * initialized after this subsystem, in the LIBRARY subsystems.
     * This is to appease the checks that all interfaces in the box
     * must have an ip address (or no ip address) defined in the config.
     */
    FOR_ALL_SWIDBS(idb) {
	ip_init_idb(idb, TRUE);
	/*
	 * If we don't have a hwidb, or we do not NVGEN this hwidb,
	 * we don't want to indicate that ip should be enabled
	 */
	if ((idb->hwptr == NULL) ||
	    (!(idb->hwptr->status & IDB_NO_NVGEN)))
	    idb->ip_enabled = TRUE;
    }

    /*
     * Initialize the queues
     */
    IPpacketQ = create_watched_queue("IP Packets", 0, 0);
    queue_init(&echoQ, 0);
    queue_init(&reassemblyQ, RAMAXBUFFERS);
    queue_init(&IPsocketQ, 0);
    ipdefaultparams.ident = 0;		/* default id */
    ipdefaultparams.tos = TOS_PREC_ROUTINE;  /* default type of service */
    ipdefaultparams.ttl = TTL_DEFAULT;	/* default time to live */
    ipdefaultparams.dontfragment = FRAGMENTS_ALLOWED;  /* can fragment */
    ipdefaultparams.log = TRUE;		/* okay to log packet */
    iprouteparams.ident = 0;		/* default id */
    iprouteparams.tos = TOS_PREC_ROUTINE;  /* default type of service */
    iprouteparams.ttl = TTL_2;		/* default time to live */
    iprouteparams.dontfragment = FRAGMENTS_ALLOWED;  /* can fragment */
    iprouteparams.log = TRUE;		/* okay to log packet */
    ip_secopt_bytes = 0;		/* no default security */
    ip_secsys_low = SECLVL_UNCLASSIFIED;	/* no system security */
    ip_secsys_high = SECLVL_UNCLASSIFIED;	/* no system security */
    ip_secsys_auth_low = AUTH_GENSER;		/* no system security */
    ip_secsys_auth_high = AUTH_GENSER;		/* no system security */
    ip_forward_sdns = FALSE;            /* don't forward SNDS protocol */
    ip_forward_nd = FALSE;		/* do not forward ND protocol, yet */
    ip_spanning = FALSE;	        /* don't forward ND/UDP by ST */
    ip_spanning_anybcast = FALSE;       /* don't accept just any b'cast */
    ip_srcroute = TRUE;			/* deal with source routes */
    ip_gratuitous_arps = TRUE;		/* generate gratuitous ARPs */
    process_create(ip_input, "IP Input", LARGE_STACK, PRIO_HIGH);
    TIMER_START(ip_icmptimer, ICMP_DELAYTIME); /* no redirects at first */
    ip_router_running =	FALSE;		/* set default router state */
    ip_mrouter_running = FALSE;		/* set default mrouter state */

    ip_host_routing = FALSE;		/* Enable host-style routing */
    netsareup = FALSE;
    ip_nat_anywhere = FALSE;		/* NAT configs will change this */
    watched_netsareup = create_watched_boolean("IP Configured", WB_IP_CONFIG);
    ip_acct_init();			/* initialize IP accounting */

    /*
     * Initialize SNMP variables
     */
    accessfailcnt = 0;
    ip_checkpointvalue = 0;

    /*
     * Verify that the start of the IP packet is on a long word
     * boundary. Print an error message if not:
     */
    {
	paktype *pak;

	pak = getbuffer(0);
	if (!pak) {
	    log_init = FALSE;
	    errmsg(&msgsym(GETBUF, SYS), 0);
	    crashdump(1);
	}
	if ((uint)ipheadstart(pak) % 4) {
	    log_init = FALSE;
	    errmsg(&msgsym(BADIPALIGN, IP), "ipheadstart",
		   (iphdrtype *)ipheadstart(pak));
	    crashdump(1);
	}
	if ((uint)&pak->datagramstart % 4) {
	    log_init = FALSE;
	    errmsg(&msgsym(BADIPALIGN, IP), "&datagramstart",
		   &pak->datagramstart);
	    crashdump(1);
	}
	if ((uint)&pak->datagramsize % 2) {
	    log_init = FALSE;
	    errmsg(&msgsym(BADIPALIGN, IP), "&datagramsize",
		   &pak->datagramsize);
	    crashdump(1);
	}
	retbuffer(pak);
    }

    /*
     * Access list and name service support.
     */
    ipaccess_init();

    /*
     * Initialize some applications that run only on IP.
     */
    ipaddr_table_init();        /* init our address database */
    boot_init();		/* init netbooting data */

    /*
     * Parser chain initialization
     */
    ip_parser_init();
    ipacclist_parser_init();
    ip_test_init();

    reg_add_ip_good_address(good_address, "good_address");

}
      
