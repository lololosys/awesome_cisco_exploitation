/* $Id: pim.c,v 3.19.8.31 1996/09/14 00:24:45 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim.c,v $
 *------------------------------------------------------------------
 * pim.c - Protocol Independent Multicast support.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PIM is a dynamic IP multicasting protocol used to build shared and
 * source-based trees to route multicast datagrams.
 *------------------------------------------------------------------
 * $Log: pim.c,v $
 * Revision 3.19.8.31  1996/09/14  00:24:45  lwei
 * CSCdi68782:  <ip pim accept-rp auto-rp> may not reject some JOINS
 * Branch: California_branch
 *
 * Revision 3.19.8.30  1996/09/12  21:09:32  chwhite
 * CSCdi61325:  (*,G) JOINS received by the RP do not trigger (S,G) JOINS
 * to source
 * Branch: California_branch
 *
 * Revision 3.19.8.29  1996/09/10  05:31:57  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.19.8.28  1996/08/28  23:50:40  dino
 * CSCdi67491:  PIM Register messages go throughout MBONE because of a
 *              multicast RP
 * Branch: California_branch
 *
 * Revision 3.19.8.27  1996/08/28  12:51:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.19.8.26  1996/08/22  01:37:37  lwei
 * CSCdi66702:  PIM-1-FAST_RP_REACHABLE log message should be removed
 * Branch: California_branch
 *
 * Revision 3.19.8.25  1996/08/21  01:21:29  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.19.8.24  1996/08/14  05:03:42  lwei
 * CSCdi64491:  Check (*,G) OIF list when new PIM neighbor comes up
 * Branch: California_branch
 *
 * Revision 3.19.8.23  1996/08/09  03:33:37  mleelani
 * CSCdi65587:  (S, G) state gratuitously created on receiving prunes.
 * Branch: California_branch
 * Create (S, G) state only on receiveing (S, G, RP-bit) prunes.
 *
 * Revision 3.19.8.22  1996/08/07  23:50:50  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.19.8.21  1996/08/07  19:37:37  lwei
 * CSCdi65220:  System crash when dequeuing accept-rp data structure
 * Branch: California_branch
 *
 * Revision 3.19.8.20  1996/08/05  19:20:35  dino
 * CSCdi65068:  midb->mode is no longer needed. Configured mode need not
 *              ever change
 * Branch: California_branch
 *
 * Revision 3.19.8.19  1996/08/03  01:20:42  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.19.8.18  1996/07/23  23:59:06  mleelani
 * CSCdi63822:  HSRP/PIM cannot operate concurrently
 * Branch: California_branch
 * Disable only for BSI FDDI.
 *
 * Revision 3.19.8.17  1996/07/19  15:01:57  snyder
 * CSCdi63480:  char * to char[], save a tiny bit
 *              36 bytes.
 * Branch: California_branch
 *
 * Revision 3.19.8.16  1996/07/09  19:23:39  mleelani
 * CSCdi61977:  Dense mode OIFS get truncated over time
 * Branch: California_branch
 * (*, G) OIF list not copied correctly to (S, G) on receiving sparse
 * joins.
 *
 * Revision 3.19.8.15  1996/06/26  22:55:48  chwhite
 * CSCdi60084:  (*,G,RP-bit) Joins toward the RP may not trigger Joins
 * upstream
 * Branch: California_branch
 *
 * Revision 3.19.8.14  1996/06/25  06:05:00  lwei
 * CSCdi61125:  Fast RP-reachable message can be more concise
 * Branch: California_branch
 *
 * Revision 3.19.8.13  1996/06/25  03:04:53  chwhite
 * CSCdi61101:  (*,G) Joins do not trigger JOINS upstream when (*,G) state
 * is create
 * Branch: California_branch
 *
 * Revision 3.19.8.12  1996/06/24  23:33:52  chwhite
 * CSCdi58419:  <no ip pim rp> should clear the mr table
 * Branch: California_branch
 *
 * Revision 3.19.8.11  1996/06/20  01:13:08  lwei
 * CSCdi60852:  Back out fixes to CSCdi58281
 * Branch: California_branch
 *
 * Revision 3.19.8.10  1996/06/18  23:50:35  speakman
 * CSCdi30595:  mroute entries will not timeout if pim is not configured
 * on any intf
 * Branch: California_branch
 * Clear the mroute cache when the last PIM interface is de-configured.
 *
 * Revision 3.19.8.9  1996/06/18  01:47:00  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.19.8.8  1996/06/11  02:50:29  lwei
 * CSCdi59607:  RP-reachability messages need to be rate-limited
 * Branch: California_branch
 *
 * Revision 3.19.8.7  1996/05/28  20:33:28  jzwiebel
 * CSCdi58281:  Offset timers keep S,G state alive in RP and neighbor
 * routers
 * Branch: California_branch
 *
 * Revision 3.19.8.6  1996/05/24  06:15:23  lwei
 * CSCdi57845:  (S,G) olist infected by (*,G) null olist when dense-mode
 *              oif exists
 * Branch: California_branch
 *
 * Revision 3.19.8.5  1996/05/21  21:01:29  lwei
 * CSCdi58303:  DVMRP prunes with long timers dont keep mdb long enough
 * Branch: California_branch
 *
 * Revision 3.19.8.4  1996/05/08  04:29:50  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.19.8.3  1996/05/04  01:08:55  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.19.8.2  1996/04/03  18:39:53  dino
 * CSCdi51024:  PIM nbma-mode adds too many interfaces to olist
 * Branch: California_branch
 *
 * Revision 3.19.8.1  1996/03/18  20:19:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.2.4  1996/03/16  06:55:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.12.2.3  1996/03/13  01:38:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.12.2.2  1996/03/07  09:48:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.12.2.1  1996/02/20  01:01:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.19  1996/02/28  03:43:42  mleelani
 * CSCdi50113:  Mroutes not always added correctly on receiving
 * grafts/joins.
 * A valid mdb may not be always present on receiving grafts.
 *
 * Revision 3.18  1996/02/27  19:01:03  dino
 * CSCdi50013:  Wrong Assert winner when receiver of Assert has a DVMRP
 *              route to src
 *
 * Revision 3.17  1996/02/22  19:27:13  lwei
 * CSCdi49375:  Missing DVMRP graft on PIM/DVMRP border causes join latency
 *
 * Revision 3.16  1996/02/22  09:03:17  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.15  1996/02/08  03:39:28  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.14  1996/02/07  22:00:57  raj
 * CSCdi45762:  Warning issued when ip pim nbma-mode configured on PRI
 * links
 * Change message and make it issued for LAN interfaces instead.
 *
 * Revision 3.13  1996/02/01  06:04:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.12  1996/01/19  01:22:14  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.11  1996/01/05  10:17:24  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.10  1995/12/17  19:24:26  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.9  1995/12/14  09:55:02  dino
 * CSCdi45564:  Users are complaining that show ip pim interface is
 *              unreadable
 *
 * Revision 3.8  1995/12/06  20:23:46  mikel
 * CSCdi33290:  HSRP + PIM on low-end fddi if = packet replication
 * disable PIM and HSRP config on low end fddi due to the non ability
 * to program CAM.  Disallow for BSI chipset only.
 *
 * Revision 3.7  1995/12/01  18:38:48  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.6  1995/11/30  04:42:15  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.5  1995/11/22  06:57:25  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.4  1995/11/21  23:12:13  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  17:30:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/09/28  00:50:41  dino
 * CSCdi40951:  PIM breaks ISIS
 *
 * Revision 2.10  1995/08/11  22:33:24  dino
 * CSCdi38597:  unaligned access in PIM
 *
 * Revision 2.9  1995/08/11  07:10:12  dino
 * CSCdi38624:  When interface flaps, dense-mode interfaces are not put
 * back in oifs
 *
 * Revision 2.8  1995/08/09  00:20:42  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.7  1995/07/25  19:18:50  dino
 * CSCdi37594:  Delay pruning on LAN is expiring just before Join override
 *
 * Revision 2.6  1995/07/24  07:33:50  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.5  1995/07/01 07:46:18  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.4  1995/06/14  04:26:51  dino
 * CSCdi35672:  Groups with RPs known sometimes have D flag set.
 *
 * Revision 2.3  1995/06/10  03:53:51  dino
 * CSCdi35623:  EFT bugs found during 11.0 integration period.
 *
 * Revision 2.2  1995/06/09  13:05:49  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:04:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "ieee.h"
#include "../ipmulticast/msg_pim.c"	/* Not a typo, see logger.h */
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "media_registry.h"
#include "../ip/ip_registry.h"
#include <mgd_timers.h>
#include "../if/network.h"
#include "../os/signal.h"
#include "../ui/debug.h"
#include "parser_defs_pim.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/tunnel.h"
#include "../iprouting/route.h"
#include "ipmulticast_debug.h"
#include "mroute.h"
#include "igmp.h"
#include "pim.h"
#include "dvmrp.h"
#include "cgmp.h"
#include "../iprouting/standby.h"
#include "../ip/ipfast_flow.h"
#include "../util/random.h"

/*
 * Storage.
 */
static pid_t pim_pid;
watched_queue *pimQ;		/* Packet input queue */
static mgd_timer pim_periodic_timer;
pim_spttype *pim_spt_thresholds; /* Configured SPT thresholds */
queuetype pim_rp_filter;      /* RP accept filter queue */

static boolean pim_rp_announce;  /* Auto-RP flag to send RP announcement */
sys_timestamp pim_rp_announce_timer; /* Timer for sending RP announcement */
sys_timestamp pim_rp_mapping_timer; /* Timer for sending RP-mapping packets */
int pim_rp_mapping_ttl;       /* Auto-RP, TTL for RP discovery packets */
ipsocktype *rp_agent_socket;  /* Auto-RP, socket receiving RP announcement */
ipsocktype *pim_rp_discovery; /* Auto-RP, RP discovery socket at DRs */
int pim_autorp_min_mtu;       /* The minimal MTU of all mcast capable IDBs */
boolean pim_immediate_register_stop; /* RP always sends register-stop */
ipsocktype *pim_socket;       /* PIM socket for all PIM_PROT packets */
ulong pim_version = 1;	      /* Version of PIM, default to version 1 */
pim_traffic_t pim_traffic;
ipparamtype pimrouteparams = { 0, TOS_PREC_INET_CONTROL, 1,
			       FRAGMENTS_ALLOWED, TRUE };

static void pim_process_packets(void);
static void pim_traffic_stat(void);
static void pim_enqueue (paktype *);
static ipsocktype * find_pimsocket(void);
static void pimv2_process_packets(ipsocktype *);
 

/*
 * pim_ip_address_change
 *
 * IP address was added or deleted from idb.
 */
static void
pim_ip_address_change (idbtype *idb, ipaddrtype address, ipaddrtype mask,
		       boolean secondary, boolean adding)
{
    pim_rp_announce_type *rp_ann;
    
    /*
     * Ignore secondary address changes
     */
    if (secondary) {
	return;
    }
    
    for (rp_ann = pim_announced_rp.qhead; rp_ann; rp_ann = rp_ann->next) {

	if (rp_ann->idb != idb) {
	    continue;
	}

	if (rp_ann->holdtime) {

	    /*
	     * Set holdtime to zero so everyone stops using this address
	     */
	    pim_set_rp_ann_holdtime(rp_ann, 0);
	}
	
	if (adding) {

	    if (!address) {

		/*
		 * Adding unnumbered interface
		 */
		if (!idb->ip_unnumbered || !idb->ip_unnumbered->ip_address) {
		    return;
		}

		address = idb->ip_unnumbered->ip_address;
	    }

	    rp_ann->rp_address = address;

	    /*
	     * Restart holdtime in case it was zeroed
	     */
	    pim_set_rp_ann_holdtime(rp_ann, PIM_RP_ANNOUNCE_HOLDTIME);
	}
	return;
    }
}


/*
 * pim_init
 *
 * Initalize the PIM module variables.
 */
void pim_init (void)
{

    /*
     * Initialize PIM global variables.
     */
    pim_pid = 0;
    pim_message_interval = PIM_MESSAGE_INTERVAL;
    pimQ = create_watched_queue("PIM packets", 0, 0);
    
    /*
     * Initialize PIM parser chain.
     */
    pim_parser_init();

    /*
     * Register PIM functions
     */
    reg_add_ip_enqueue(PIM_PROT, pim_enqueue, "pim_enqueue");
    ip_set_proto_outcounter(PIM_PROT, &pim_traffic.outputs);
    reg_add_ip_show_traffic(pim_traffic_stat, "pim_traffic_stat");
    reg_add_ip_address_change(pim_ip_address_change, "pim_ip_address_change");
}

/*
 * pim_init_idb
 *
 * Initalize the PIM specific fields in the idb.
 */
void pim_init_idb (idbtype *idb)
{

    idb->ip_mrouting = FALSE;
    idb->pim_enabled = FALSE;
    idb->pim_mode = PIM_DENSE_MODE;
    idb->pim_query_interval = PIM_QUERY_INTERVAL;
}

static void pim_process_teardown (int signal, int dummy1, void *dummy2,
				  char *dummy3)
{
    paktype *pak;

    mgd_timer_stop(&pim_periodic_timer);
    process_watch_mgd_timer(&pim_periodic_timer, DISABLE);
    process_watch_queue(pimQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(pimQ)) != NULL)
	datagram_done(pak);
    if (pim_socket) {
	close_ipsocket(pim_socket);
	pim_socket = NULL;
    }
}

static void pim_check_rp_sockets (void)
{
    if (rp_agent_socket)
	process_watch_queue(rp_agent_socket->inq, ENABLE, RECURRING);
    if (pim_rp_discovery)
	process_watch_queue(pim_rp_discovery->inq, ENABLE, RECURRING);
}    

/*
 * pimv2_open_socket
 *
 * Open a routing socket for protocol PIM_PROT
 * If it fails to open the routing socket, it will also reset
 * the PIMV2 flag in pim_version
 */
static void pimv2_open_socket(void)
{
    pim_socket = find_pimsocket();
    if (!pim_socket) {
	pim_socket = open_ipsocket(PIM_PROT, NULL, NULL, 0, 0, 0);
	if (!pim_socket) {
	    pim_version = PIMV1;  /* No socket, don't do PIMV2 */
	    PIM_BUGINF(0, ("\nPIM: Can't open PIM socket, stay in version 1"));
	} else {
	    process_watch_queue(pim_socket->inq, ENABLE, RECURRING);
	}
    }
}

/*
 * pim_process
 *
 * Main line entry point for PIM.
 */
static process pim_process (void)
{
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, pim_process_teardown);
    if (!pimQ)
	process_kill(THIS_PROCESS);
    mgd_timer_init_leaf(&pim_periodic_timer, NULL, 0, NULL, FALSE);
    mgd_timer_start(&pim_periodic_timer, PIM_PERIODIC_INTVL);
    process_watch_queue(pimQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&pim_periodic_timer, ENABLE);
    pim_check_rp_sockets();
    if (pim_version & PIMV2) {
	pimv2_open_socket();
    }
    
    while (TRUE) {
	ulong major, minor;

	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		mgd_timer_start(&pim_periodic_timer, PIM_PERIODIC_INTVL);
		pim_process_timers();
		break;
	    case QUEUE_EVENT:
		pim_process_packets();
		if (pim_socket &&
		    (!process_is_queue_empty(pim_socket->inq))) {
		    pimv2_process_packets(pim_socket);
		}
		if (rp_agent_socket &&
		    (!process_is_queue_empty(rp_agent_socket->inq))) {
		    pim_receive_rp_announce();
		}
		if (pim_rp_discovery &&
		    (!process_is_queue_empty(pim_rp_discovery->inq))) {
		    pim_receive_rp_discovery();
		}
		break;
	    case DIRECT_EVENT:
		pim_check_rp_sockets();
		break;
	    }
	}		
    }
}

/* 
 * pim_code_string
 *
 * Return string associated with code field.
 */
uchar *pim_code_string (ulong code)
{

    uchar *str;

    switch (code) {
    case PIM_QUERY_CODE:
	str = "Router-Query";
	break;
    case PIM_REGISTER_CODE:
	str = "Register";
	break;
    case PIM_REGISTER_STOP_CODE:
	str = "Register-Stop";
	break;
    case PIM_JOIN_PRUNE_CODE:
	str = "Join/Prune";
	break;
    case PIM_RP_REACHABLE_CODE:
	str = "RP-Reachable";
	break;
    case PIM_ASSERT_CODE:
	str = "Assert";
	break;
    case PIM_GRAFT_CODE:
	str = "Graft";
	break;
    case PIM_GRAFT_ACK_CODE:
	str = "Graft-Ack";
	break;
    default:
	str = "Unknown";
    }
    return(str);
}

/*
 * pim_find_accept_rp_filter
 *
 * Return pointer of next pointer from previous entry if entry is found.
 */
boolean pim_accept_rp (ipaddrtype rp, ipaddrtype group)
{

    pim_rpfilter *filter;
    pim_rp_mapping *prm = NULL;

    /*
     * None configured. Accept any RP.
     */
    if (!pim_rp_filter.count) return(TRUE);

    /*
     * Traverse list of configured acceptable RPs. If there is a 0.0.0.0
     * configured accept-rp, we'll use any RP as long as it matches the
     * group access-list.
     */
    for (filter = (pim_rpfilter *) pim_rp_filter.qhead; filter; 
	 filter = filter->next) {

	/*
	 * This is for RPs learned from Auto-RP, ignore the adress field
	 * Don't check RP-mappings learned from autorp for the two Auto-RP
	 * well-known groups.
	 */
	if (filter->autorp) {
	    if (nfast_check(filter->group_acl, group) &&
		group != CISCO_RP_ANNOUNCE && group != CISCO_RP_DISCOVERY) {
		prm = pim_get_prm(group, 0);
		if (prm) return (TRUE);
	    }
	    continue;
	}
	if (filter->address == rp || !filter->address) {
	    return(nfast_check(filter->group_acl, group));
	}
    }
    return(FALSE);
}

/*
 * pim_accept_rp_command
 *
 * Process the "ip pim accept-rp <address> [<acl>]" global configuration
 * command.
 */
void pim_accept_rp_command (parseinfo *csb)
{

    pim_rpfilter *filter;
    ipaddrtype   rp;
    acl_headertype *acl;
    boolean	 autorp;
    idbtype	 *idb;
    list_element *l_elt;
    
    rp = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(int,1) || *GETOBJ(string,1)) {
	acl = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1),
			       GETOBJ(int,1) == 0, ACL_IP_NAMED_SIMPLE);
	if (!acl)
	    return;
    } else
	acl = NULL;
    
    autorp = GETOBJ(int,2);

    /*
     * Find entry. Key is RP address.
     */
    for (filter = pim_rp_filter.qhead; filter; filter = filter->next) {
	if (filter->address == rp && filter->autorp == autorp) break;
    }

    if (csb->sense) {
	if (!filter) {
	    filter = malloc(sizeof(pim_rpfilter));
	    if (!filter) return;
	    enqueue(&pim_rp_filter, filter);
	}
	filter->address = rp;
	filter->group_acl = acl;
	filter->autorp = autorp;

	/*
	 * If haven't set up to receive RP-discovery packets yet, need
	 * to do so now. Find the first pim enabled idb and set up socket
	 * as a receiver.
	 */
	FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	    if (!idb->pim_enabled) continue;
	    pim_enable_rp_discovery(idb);
	    process_wakeup(pim_pid);
	    break;
	}
    } else if (filter) {
	unqueue(&pim_rp_filter, filter);
	free(filter);
    }
}

/*
 * pim_rp_announce_command
 *
 * Process the "ip pim send-rp-announce <interface-unit> scope <ttl>
 *              group-list <acl>"
 * command.
 */
static void pim_rp_announce_command (parseinfo *csb)
{
    pim_rp_announce_type *rp_ann;
    ipaddrtype rp;
    acl_headertype *acl;
    ulong      ttl;
    idbtype    *idb;

    idb = GETOBJ(idb,1);
    ttl = GETOBJ(int,1);
    acl = (GETOBJ(int,2) || *GETOBJ(string,2))
	? access_parse_acl(GETOBJ(int,2), GETOBJ(string,2), GETOBJ(int,2) == 0,
			   ACL_IP_NAMED_SIMPLE)
	: NULL;

    /*
     * Store IP address to use as RP address.  If interface is unnumbered
     * use IP address of unnumbered interface.
     */
    if (idb->ip_address) {
	rp = idb->ip_address;
    } else {
	idbtype *tmpidb;

	tmpidb = idb->ip_unnumbered;
	
	if (!tmpidb || !tmpidb->ip_address) {
	    printf("%s failed: Must configure 'ip address' or 'ip unnumbered' "
		   "for %s\n", csb->nv_command, idb_get_namestring(idb));
	    return;
	}
	rp = tmpidb->ip_address;
    }

    /*
     * Find entry, key is idb (used to be RP address, but that can change).
     */
    for (rp_ann = pim_announced_rp.qhead; rp_ann; rp_ann = rp_ann->next) {
	if (rp_ann->idb == idb) break;
    }

    if (csb->sense) {
	if (!rp_ann) {
	   rp_ann = malloc(sizeof(pim_rp_announce_type));
	   if (!rp_ann) return;
	   enqueue(&pim_announced_rp, rp_ann);
	}
	rp_ann->rp_address = rp;
	rp_ann->ttl = ttl;
	rp_ann->group_acl = acl;
	rp_ann->idb = idb;
	pim_rp_announce = TRUE;
	pim_set_rp_ann_holdtime(rp_ann, PIM_RP_ANNOUNCE_HOLDTIME);
    } else {
	if (!rp_ann) return;

	/*
	 * Reset the holdtime for this entry to 0. And send
	 * a set of new RP-announce mesages, so that the RP-mapping
	 * agent can delete it immediately.
	 */
	pim_set_rp_ann_holdtime(rp_ann, 0);
	
	unqueue(&pim_announced_rp, rp_ann);
	free(rp_ann);
	if (QUEUEEMPTY(&pim_announced_rp)) {
	    pim_rp_announce = FALSE;
	    TIMER_STOP(pim_rp_announce_timer);
	}
    }
}

/*
 * pim_rp_discovery_command
 *
 * Process the "ip pim send-rp-discovery scope <ttl>" command.
 */
static void pim_rp_discovery_command (parseinfo *csb)
{

    idbtype  *idb;
    addrtype laddr;
    boolean  enabled;
    list_element *l_elt;
    
    enabled = FALSE;
    if (csb->sense) {
	if (rp_agent_socket) return;

	pim_rp_mapping_ttl = GETOBJ(int,1);
	TIMER_START(pim_rp_mapping_timer, PIM_RP_MAPPING_INTVL);

	/*
	 * Make all igmp enabled idbs members of the
	 * CISCO_RP_ANNOUNCE group.
	 */
	FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	    if (!idb->pim_enabled) continue;
	    enabled = TRUE;
	    igmp_add_group(CISCO_RP_ANNOUNCE, idb, 0, 
			   IGMP_LOCAL_FLAG | IGMP_USER_FLAG, 0);

	    /*
	     * Find out the minimal MTU size of all pim capable IDBs.
	     */
	    if (idb->ip_mtu < pim_autorp_min_mtu) {
		pim_autorp_min_mtu = idb->ip_mtu;
	    }
	}

	if (pim_autorp_min_mtu < SMALL_IP_MTU) {
	    pim_autorp_min_mtu = SMALL_IP_MTU;
	}

	if (!enabled) {
	    printf("No PIM enabled interface exists\n");
	    return;
	}

	/*
	 * Open UDP listening socket.
	 */
	laddr.type = ADDR_IP;
	laddr.ip_addr = CISCO_RP_ANNOUNCE;
	rp_agent_socket = open_ipsocket(UDP_PROT, &laddr, 0, PIM_RP_DISC,
					0, SOCK_ANYSRC);
	if (!rp_agent_socket) {
	    PIM_BUGINF(0, ("\nPIM: Can't open Auto-RP UDP socket"));
	    return;
	}
	process_wakeup(pim_pid);
	GET_TIMESTAMP(pim_rp_mapping_timer);
    } else {
	if (!rp_agent_socket) return;

	/*
	 * Leave the CISCO_RP_ANNOUNCE group
	 */
	FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	    if (!idb->pim_enabled) continue;
	    igmp_delete_group(CISCO_RP_ANNOUNCE, idb);
	}

	TIMER_STOP(pim_rp_mapping_timer);

	close_ipsocket(rp_agent_socket);
	rp_agent_socket = NULL;
    }
}

/*
 * pim_rp_announce_filter_command
 *
 * Process the PIM command:
 *
 *     [no] ip pim rp-announce-filter [rp-list <acl>] [group-list <acl>]
 */
static void pim_rp_announce_filter_command (parseinfo *csb)
{

    pim_rp_afilter_type *filter;
    acl_headertype *rp_acl, *group_acl;

    if (GETOBJ(int,1) || *GETOBJ(string,1)) {
	rp_acl = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1),
				  GETOBJ(int,1) == 0, ACL_IP_NAMED_SIMPLE);
	if (!rp_acl)
	    return;
    } else
	rp_acl = NULL;
    
    if (GETOBJ(int,2) || *GETOBJ(string,2)) {
	group_acl = access_parse_acl(GETOBJ(int,2), GETOBJ(string,2),
				     GETOBJ(int,2) == 0, ACL_IP_NAMED_SIMPLE);
	if (!group_acl)
	    return;
    } else
	group_acl = NULL;

    for (filter = pim_rp_announce_filter.qhead; filter; 
	 filter = filter->next) {
	if (filter->rp_acl == rp_acl) {
	    if (filter->group_acl == group_acl) break;
	}
    }

    if (csb->sense) {
	if (filter) return;
	filter = malloc(sizeof(pim_rp_afilter_type));
	filter->rp_acl = rp_acl;
	filter->group_acl = group_acl;
	enqueue(&pim_rp_announce_filter, filter);
    } else {
	if (!filter) {
	    printf("Filter not found\n");
	    return;
	}
	unqueue(&pim_rp_announce_filter, filter);
	free(filter);
    }
}

/*
 * pim_commands
 *
 * Process PIM related global and interface subcommands.
 */
void pim_commands (parseinfo *csb)
{

    idbtype              *idb;
    pim_rptype           *rp;
    pim_spttype          *thres;
    pim_rpfilter         *rpfilter;
    pim_rp_announce_type *rp_ann;
    pim_rp_afilter_type  *rp_afilter;

    idb = csb->interface;

    /*
     * Do NV generation.
     */
    if (csb->nvgen) {
	switch(csb->which) {
	case PIM_RP_ADDRESS:
	    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp;
		 rp = rp->next) {
		nv_write(TRUE, "%s %i", csb->nv_command, rp->rp_address);
		if (rp->group_acl)
		    nv_add(TRUE, " %s", rp->group_acl->name);
		nv_add(rp->override, " override");
	    }
	    break;
	case PIM_MSG_INTERVAL:
	    nv_write((pim_message_interval != PIM_MESSAGE_INTERVAL),
		     "%s %d", csb->nv_command, pim_message_interval/ONESEC);
	    break;
	case PIM_SM_MODE:
	    if (GETOBJ(int,1) != PIM_SM_MODE) break;
	    if (idb->pim_enabled && idb->pim_mode == PIM_SPARSE_MODE) {
		nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	case PIM_DM_MODE:
	    if (GETOBJ(int,1) != PIM_DM_MODE) break;
	    if (idb->pim_enabled && idb->pim_mode == PIM_DENSE_MODE) {
		nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	case PIM_S_D_MODE:
	    if (GETOBJ(int,1) != PIM_S_D_MODE) break;
	    if (idb->pim_enabled && idb->pim_mode == PIM_SPARSE_DENSE_MODE) {
		nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	case PIM_QUERY_INTVL:
	    nv_write((idb->pim_query_interval != PIM_QUERY_INTERVAL),
		     "%s %d", csb->nv_command, idb->pim_query_interval/ONESEC);
	    break;
	case PIM_NBMA_MODE:
	    nv_write(idb->pim_nbma_mode, "%s", csb->nv_command);
	    break;
	case PIM_SPT_THRESHOLD:
	    for (thres = pim_spt_thresholds; thres; thres = thres->next) {
		nv_write(TRUE, "%s ", csb->nv_command);
		if (thres->spt_threshold == PIM_INFINITE_THRESHOLD) {
		    nv_add(TRUE, "infinity");
		} else {
		    nv_add(TRUE, "%d", thres->spt_threshold/1000);
		}
		if (thres->group_acl)
		    nv_add(TRUE, " group-list %s", thres->group_acl->name);
	    }
	    break;
	case PIM_ACCEPT_RP:
	    for (rpfilter = pim_rp_filter.qhead; rpfilter; 
		 rpfilter = rpfilter->next) {
		nv_write(TRUE, "%s ", csb->nv_command);
		if (rpfilter->autorp) {
		    nv_add(rpfilter->autorp, "Auto-RP");
		} else {
		    nv_add(TRUE, "%i", rpfilter->address);
		}
		if (rpfilter->group_acl)
		    nv_add(TRUE, " %s", rpfilter->group_acl->name);
	    }
	    break;
	case PIM_SEND_RP_ANNOUNCE:
	    for (rp_ann = (pim_rp_announce_type *) pim_announced_rp.qhead;
		 rp_ann; rp_ann = rp_ann->next) {
	        nv_write(TRUE, "%s %s", csb->nv_command,
			 rp_ann->idb->namestring);
		nv_add(rp_ann->ttl, " scope %d", rp_ann->ttl);
		if (rp_ann->group_acl)
		    nv_add(TRUE, " group-list %s", rp_ann->group_acl->name);
	    }
	    break;
	case PIM_SEND_RP_DISCOVERY:
	    if (rp_agent_socket) {
		nv_write(TRUE, "%s scope %d", csb->nv_command,
			 pim_rp_mapping_ttl);
	    }
	    break;
	case PIM_ANNOUNCE_FILTER:
	    for (rp_afilter = pim_rp_announce_filter.qhead; rp_afilter; 
		 rp_afilter = rp_afilter->next) {
		nv_write(TRUE, "%s", csb->nv_command);
		if (rp_afilter->rp_acl) {
		    nv_add(TRUE, " rp-list %s", rp_afilter->rp_acl->name);
		}
		if (rp_afilter->group_acl) {
		    nv_add(TRUE, " group-list %s",
			   rp_afilter->group_acl->name);
		}
	    }
	    break;
	case PIM_FAST_REG_STOP:
	    if (pim_immediate_register_stop) {
	        nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	case PIM_VERSION:
	    if (pim_version & PIMV2) {
		nv_write(TRUE, "%s 2", csb->nv_command);
	    }
	    break;
	default:
	    break;
	}
	return;
    }

    /*
     * Process command.
     */
    switch(csb->which) {
    case PIM_RP_ADDRESS:
	pim_rp_command(csb);
	break;

    case PIM_MSG_INTERVAL:
	pim_message_interval = (csb->sense) ? GETOBJ(int,1) * ONESEC :
	    PIM_MESSAGE_INTERVAL;
	break;

    case PIM_DM_MODE:
    case PIM_SM_MODE:
    case PIM_S_D_MODE:
	if (csb->sense) {
	    if (!ip_mrouter_running) {
		printf("\n WARNING: \"ip multicast-routing\" is not "
	       "configured, IP Multicast packets will\n\t   not be forwarded");
	    }
	    if (GETOBJ(int,1) == PIM_SM_MODE) {
		idb->pim_mode = PIM_SPARSE_MODE;
	    } else if (GETOBJ(int,1) == PIM_DM_MODE) {
		idb->pim_mode = PIM_DENSE_MODE;
		if (idb->pim_nbma_mode) {
		   printf("\nPIM nbma-mode is not recommended for dense-mode");
		}
	    } else if (GETOBJ(int,1) == PIM_S_D_MODE) {
		idb->pim_mode = PIM_SPARSE_DENSE_MODE;
	    }
	    if (!pim_check_hsrp(idb))
		return;
	    pim_enable_idb(idb);
	} else {
	    pim_disable_idb(idb);
	}
	break;

    case PIM_QUERY_INTVL:
	idb->pim_query_interval = (csb->sense) ? GETOBJ(int,1) * ONESEC :
	    PIM_QUERY_INTERVAL;
	break;
    case PIM_NBMA_MODE:
	if (csb->sense) {
	    if (is_lan(idb->hwptr)) {
	       printf("\nPIM nbma-mode is not recommended for LAN interfaces");
	    }
	    if (idb->pim_enabled && (idb->pim_mode == PIM_DENSE_MODE ||
				     idb->pim_mode == PIM_SPARSE_DENSE_MODE)) {
	    printf("\nPIM nbma-mode is not recommended for %sdense-mode", 
		   idb->pim_mode == PIM_DENSE_MODE ? "" : "sparse-");
	    }
	}
	idb->pim_nbma_mode = csb->sense;
	break;
    case PIM_SPT_THRESHOLD:
	pim_spt_threshold_command(csb);
	break;
    case PIM_ACCEPT_RP:
	pim_accept_rp_command(csb);
	break;
    case PIM_SEND_RP_ANNOUNCE:
	pim_rp_announce_command(csb);
	break;
    case PIM_SEND_RP_DISCOVERY:
	pim_rp_discovery_command(csb);
	break;
    case PIM_ANNOUNCE_FILTER:
	pim_rp_announce_filter_command(csb);
	break;
    case PIM_FAST_REG_STOP:
        if (csb->sense) {
	    pim_immediate_register_stop = TRUE;
        } else {
	    pim_immediate_register_stop = FALSE;
        }
	break;
    case PIM_VERSION:
	if (csb->sense) {
	    pim_version = GETOBJ(int,1);
	    if (pim_version & PIMV2) pimv2_open_socket();
	} else {
	    pim_version = PIMV1;
	    if (pim_socket) {
		close_ipsocket(pim_socket);
		pim_socket = NULL;
	    }
	}
    default:
	break;
    }
}

/*
 * pim_enable_idb
 *
 * Enable PIM on interface. If already enabled, quietly return.
 */
void pim_enable_idb (idbtype *idb)
{
    igmpidbtype *igmpidb;
    
    if (idb->pim_enabled) return;

    /*	
     * Enable IGMP on interface.
     */
    igmp_enable_idb(idb);
    igmpidb = igmp_getidb(idb);
    if (!igmpidb) {
	printf("Could not enable PIM on %s", idb_get_namestring(idb));
	return;
    }
    
    idb->pim_enabled = TRUE;
    
    /*
     * Flag interface for doing IP multicast routing. Enable
     * the address filter for all data link multicast addresses.
     */
    idb->ip_mrouting = TRUE;
    if (idb->hwptr->afilter_reset) {
	(*idb->hwptr->afilter_reset)(idb->hwptr);
    } else {
	reg_invoke_media_interesting_addresses(idb);
    }

    /*
     * Assume sending IGMP host queries until PIM DR election says 
     * otherwise. Send host query now. Send router query now.
     */
    idb->ip_multicast_dr = TRUE;
    igmpidb->multicast_qr = TRUE;
    idb->pim_dr = 0;
    if (!is_p2p(idb)) {
	if (idb->ip_address) {
	    idb->pim_dr = idb->ip_address;
 	} else if (idb->ip_unnumbered) {
 	    idb->pim_dr = idb->ip_unnumbered->ip_address;
 	}
    }
    GET_TIMESTAMP(idb->pim_query_timer);
    igmp_send_query(idb, ALLSYSTEMS);

    /*
     * Now all routers (at least PIMv1) join the Auto-RP discovery group.
     */
    if (!pim_rp_discovery && interface_up(idb)) {
	if (!pim_enable_rp_discovery(idb)) return;
	process_wakeup(pim_pid);
    }

    if (pim_pid) return;

    /*
     * Start PIM process if not started.
     */
    pim_pid = process_create(pim_process, "PIM Process", NORMAL_STACK,
			     PRIO_NORMAL);
}

/*
 * pim_disable_idb
 *
 * Disable PIM on interface. Delete process if no more interfaces are running
 * PIM.
 */
void pim_disable_idb (idbtype *idb)
{
    pim_nbrtype *nbr, *next;
    igmp_cachetype *igmp;
    boolean any_enabled = FALSE;
    list_element *l_elt;
    
    if (!pim_pid) return;
    if (!idb->pim_enabled) return;

    /*
     * Disable PIM on interface. Make sure that we don't send IGMP host
     * queries if we are not a multicast router.
     */
    idb->pim_enabled = FALSE;
    ipmulticast_remove_idb(idb);
    idb->ip_multicast_dr = FALSE;

    /*
     * Disable multicast routing on interface. Reset address filter for
     * data link multicasts.
     */
    idb->ip_mrouting = FALSE;
    if (idb->hwptr->afilter_reset)
	(*idb->hwptr->afilter_reset)(idb->hwptr);
    else
	reg_invoke_media_interesting_addresses(idb);

    /*
     * Delete all PIM neighbors on inteface.
     */
    for (nbr = idb->pim_nbrs; nbr; nbr = next) {
	next = nbr->next;
	free(nbr);
    }
    idb->pim_nbrs = NULL;

    /*
     * If joined CISCO_RP_DISCOVERY group on this interface,
     * delete the cache.
     */
    igmp = igmp_find_group(CISCO_RP_DISCOVERY, idb);
    if (igmp) {
	igmp_delete_group(CISCO_RP_DISCOVERY, idb);
    }
    
    /*
     * Disable IGMP unless there are groups the router has joined.
     */
    igmp_disable_idb(idb);

    /*
     * If there is no PIM on any interfaces, kill the process.
     * If there is PIM on nother interface and we joined CISCO_RP_DISCOVERY
     * on the interface to be disabled, join CISCO_RP_DISCOVERY on another
     * pim interface.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (idb->pim_enabled) {
	    any_enabled = TRUE;
	    if (!igmp) break;
	    if (interface_up(idb)) {
		pim_enable_rp_discovery(idb);
		process_wakeup(pim_pid);
		return;
	    }
	}
    }

    /*
     * Close IP socket.
     */
    if (igmp && pim_rp_discovery) {
	close_ipsocket(pim_rp_discovery);
	pim_rp_discovery = NULL;
    }

    if (any_enabled)
	return;

    /*
     * Delete entire multicast routing table.
     */
    ip_clear_mroute_cache(0, NULL);

    /*
     * Delete entire Auto-RP cache.
     */
    ip_clear_autorp_cache();

    close_ipsocket(pim_socket);
    pim_socket = NULL;

    /*
     * Kill process.
     */
    process_kill(pim_pid);
    pim_pid = 0;
}

/*
 * pim_idb_comingup
 *
 * Interface is coming up, join the Auto-RP discovery group. Send CGMP self
 * join so switches can learn quickly where router ports are in a switched
 * network.
 */
void pim_idb_comingup (idbtype *idb)
{
    if (!pim_pid || !idb->pim_enabled)
	return;

    if (!pim_rp_discovery) {
	pim_enable_rp_discovery(idb);
	process_wakeup(pim_pid);
    }

    if (idb->cgmp_enabled) {
	cgmp_send_self(CGMP_JOIN_TYPE, idb, 0);
    }
    
    /*
     * Trigger a PIM query for robust neighbor discovery on
     * point-to-point links where query timers are long.
     */
    if (is_p2p(idb)) {
	pim_send_query(idb);
    }
}

/*
 * pim_idb_goingdown
 *
 * Interface is going down, join the Auto-RP discovery group
 * on other pim-enabled interface, if necessary.
 */
void pim_idb_goingdown (idbtype *idb)
{
    igmp_cachetype *igmp;
    idbtype *idb2;
    list_element *l_elt;
    
    if (!pim_pid || !idb->pim_enabled)
	return;

    igmp = igmp_find_group(CISCO_RP_DISCOVERY, idb);
    if (igmp) {
	igmp_delete_group(CISCO_RP_DISCOVERY, idb);
	FOR_ALL_IPMULTICAST_IDBS(idb2, l_elt) {
	    if (idb2 == idb)
		continue;
	    if (idb2->pim_enabled && interface_up(idb2)) {
		pim_enable_rp_discovery(idb2);
		process_wakeup(pim_pid);
		return;
	    }
	}

	if (pim_rp_discovery) {
	    close_ipsocket(pim_rp_discovery);
	    pim_rp_discovery = NULL;
	}
    }
}

/*
 * pim_show_commands
 */
void pim_show_commands (parseinfo *csb)
{
    switch (csb->which) {
    case PIM_SHOW_NBR:
	pim_show_neighbors(csb);
	break;
    case PIM_SHOW_RP:
	pim_show_rp(csb);
	break;
    case PIM_SHOW_IDB:
	pim_show_interface(csb);
	break;
    default:
	printf("\nIllegal PIM command");
    }
}

/*
 * pim_spt_threshold_command
 *
 * Process: [no] ip pim spt-threshold <kbps> | infinity [group-list <acl>]
 */
void pim_spt_threshold_command (parseinfo *csb)
{
    
    pim_spttype *thres, *prevtail;
    acl_headertype *group_acl;
    ulong       bps;

    bps = GETOBJ(int,1);
    if (bps != PIM_INFINITE_THRESHOLD) bps *= 1000;
    if (GETOBJ(int,2) || *GETOBJ(string,2)) {
	group_acl = access_parse_acl(GETOBJ(int,2), GETOBJ(string,2),
				     GETOBJ(int,2) == 0, ACL_IP_NAMED_SIMPLE);
	if (!group_acl)
	    return;
    } else
	group_acl = NULL;
    
    /*
     * Search for previously configurd threshold. Keep track of one before
     * the matched one for delete operations. Go to the tail for add
     * operations. prevtail is used for both the add and delete case since
     * both can't be done at one time.
     */
    prevtail = NULL;
    for (thres = pim_spt_thresholds; thres; thres = thres->next) {
	if (thres->group_acl == group_acl) {

	    /*
	     * Found entry, go get last in chain so we can link new one
	     * at end.
	     */
	    if (csb->sense) {
		prevtail = thres;
		while (prevtail->next) prevtail = prevtail->next;
	    }
	    break;
	}
	prevtail = thres;
    }

    /*
     * Add to tail of linked list if it doesn't already exist.
     */
    if (csb->sense) {
	if (!thres) {
	    thres = malloc(sizeof(pim_spttype));
	    if (!thres) return;
	    if (prevtail) {
		prevtail->next = thres;
	    } else {
		pim_spt_thresholds = thres;
	    }
	}
	thres->spt_threshold = bps;
	thres->group_acl = group_acl;
    } else {
	
	/*
	 * Delete from link list if it was found.
	 */
	if (!thres) {
	    printf("\nSPT threshold does not exist");
	    return;
	}
	if (prevtail) {
	    prevtail->next = thres->next;
	} else {
	    pim_spt_thresholds = thres->next;
	}
	free(thres);
    }
}

/*
 * pim_process_timers
 *
 * Go through processing of periodic timers.
 */
void pim_process_timers (void)
{
    list_element *l_elt;
    idbtype *idb;

    /*
     * Determine if PIM router query should be sent.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->pim_enabled) continue;
	if (AWAKE(idb->pim_query_timer)) {
	    pim_send_query(idb);
	    igmp_check_new_rp(0, idb);
	}

	/*
	 * Time-out PIM neighbors.
	 */
	pim_expire_nbrs(idb);
    }

    /*
     * Time-out multicast routing table entries.
     */
    ip_mroute_ager();

    /*
     * Check if need to send RP announcement for Auto RP discovery
     */
    if (pim_rp_announce && AWAKE(pim_rp_announce_timer)){
	pim_send_rp_announce();
    }

    /*
     * Check if we are an RP-mapping agent and need to send Auto_RP
     * discovery packet
     */
    if (rp_agent_socket && AWAKE(pim_rp_mapping_timer)) {
	pim_send_rp_mapping();
    }

    /*
     * Time-out the learned Auto-RP mapping entries.
     */
    if (pim_rp_discovery || rp_agent_socket) {
        ip_pim_autorp_ager();
    }
}

static void pim_process_packets (void)
{
    paktype   *pak;
    iphdrtype *ip;
    igmptype  *igmp;
    pimtype   *pim;
    ulong     length;

    /*
     * Process input queue.
     */
    while ((pak = process_dequeue(pimQ))) {
	ip = iphdrtype_start(pak);
	length = ip->tl - ipheadsize(ip);
	igmp = (igmptype *) ipdatastart_iph(ip);
	pim = (pimtype *) igmp->data;

	/*
	 * Validate PIM version number.
	 */
	if (!IS_PIM_VERSION1(igmp->address)) {
	    PIM_BUGINF(0, ("\nPIM: Incorrect PIM version, from %i (%s)",
			   ip->srcadr, pak->if_input->namestring));
	    datagram_done(pak);
	    continue;
	}
	    
	/*
	 * Are we debugging?
	 */
	if (pim_debug && 
	    (!pim_group_debug || igmp->code != PIM_QUERY_CODE)) {
	    buginf("\nPIM: Received %s on %s from %i",
		   pim_code_string(igmp->code), pak->if_input->namestring,
		   ip->srcadr);
	}

	/*
	 * Validate that at least an IGMP header is present.
	 */
	if (length < IGMPHEADERBYTES) {
	    PIM_BUGINF(0, ("\nPIM: Packet too short"));
	    datagram_done(pak);
	    continue;
	}

	length -= IGMPHEADERBYTES;

	switch (igmp->code) {
	case PIM_QUERY_CODE:
	    pim_receive_query(igmp, ip->srcadr, pak->if_input, length);
	    break;

	case PIM_REGISTER_CODE:
	    pimv1_receive_register(pak, igmp, pak->if_input);
	    break;

	case PIM_REGISTER_STOP_CODE:
	    pimv1_receive_register_stop(igmp, ip->srcadr, length);
	    break;

	case PIM_JOIN_PRUNE_CODE:
	    pim_receive_join_prune(igmp, pak->if_input, ip->srcadr, 
				   length);
	    break;

	case PIM_RP_REACHABLE_CODE:
	    pim_receive_rp_reach(pak, igmp, length);
	    break;

	case PIM_ASSERT_CODE:
	    pim_receive_assert(igmp, ip->srcadr, pak->if_input, length);
	    break;

	case PIM_GRAFT_CODE:
	    pim_receive_graft(igmp, ip->srcadr, pak->if_input, length);

	    /*
	     * Send acknowledgement to Graft. Use input buffer, just
	     * change type and rechecksum.
	     */
	    pim_send_graft_ack(pak, length);
	    continue;

	case PIM_GRAFT_ACK_CODE:
	    pim_receive_graft_ack(igmp, ip->srcadr, pak->if_input, length);
	    break;

	default:
	    break;
	}

	datagram_done(pak);
    }
}


/*
 * pim_send_query
 *
 * Send PIM router query on interface.
 */
void pim_send_query (idbtype *idb)
{

    paktype  *pak;
    igmptype *igmp;
    ulong    *pim;
    int      size;

    /*
     * Set timer now before doing anything.
     */
    TIMER_START_JITTERED(idb->pim_query_timer, idb->pim_query_interval,
			 PIM_JITTER_PCT);

    /*
     * If no IP address on interface, check next time.
     */
    if (!idb->ip_enabled) return;

    /*
     * Don't send PIM queries over DVMRP tunnels.
     */
    if (DVMRP_TUNNEL(idb)) return;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + PIM_QUERY_SIZE;
    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_QUERY_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;
    pim = (ulong *) igmp->data;
    *pim = (idb->pim_mode << 28) | 
	((idb->pim_query_interval / ONESEC) * PIM_MULTIPLIER);
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    if (pim_debug && !pim_group_debug) {
	buginf("\nPIM: Send Router-Query on %s", idb->namestring);
    }

    /*
     * Send to IP.
     */
    ipwrite(pak, size, IGMP_PROT, 0, ALLROUTERS, idb, NULL, &igmprouteparams);
    igmp_traffic.output_pim++;
}

/*
 * pim_receive_query
 *
 * Process incoming PIM router query.
 */
void pim_receive_query (igmptype *igmp, ipaddrtype source, idbtype *idb, 
			ulong length)
{

    pim_nbrtype *nbr;
    boolean     first_nbr, dense_group_only;
    ulong       mode_and_ht, mode, holdtime;
    igmpidbtype *igmpidb;
    
    if (length < PIM_QUERY_SIZE) {
	PIM_BUGINF(0, ("\nPIM: Packet too short"));
	return;
    }

    /*
     * Hearing queries from ourself, bad configuration or we're in a 
     * looped bridged network.
     */
    if (ip_ifaddress(idb, source)) return;

    /*
     * Get advertised mode and holdtime.
     */
    mode_and_ht = *((ulong *) (igmp->data));
    mode = PIM_QUERY_GET_MODE(mode_and_ht);
    holdtime = PIM_QUERY_GET_HT(mode_and_ht);

    first_nbr = FALSE;
    nbr = pim_find_nbr(source, idb);
    if (!nbr) {
	nbr = malloc(sizeof(pim_nbrtype));
	if (!nbr) return;
	nbr->nbr_address = source;
	GET_TIMESTAMP(nbr->uptime);
	nbr->next = idb->pim_nbrs;
	if (!idb->pim_nbrs) first_nbr = TRUE;
	idb->pim_nbrs = nbr;
    }

    /*
     * Update timer and mode for every query received.
     */
    TIMER_START(nbr->expires, (holdtime * ONESEC));
    nbr->mode = mode;

    /*
     * If this is the first neighbor on interface, there are no directly 
     * connected members on the interface, and the interface is configured
     * in dense-mode, we need to add interface to outgoing list for all
     * groups. If the interface is configured in sparse-dense-mode, only
     * add this interface if the group is in dense-mode.
     */
    if (first_nbr && idb->pim_mode != PIM_SPARSE_MODE) {
	dense_group_only =
		 (idb->pim_mode == PIM_SPARSE_DENSE_MODE) ? TRUE : FALSE;
	pim_add_mroute_idb(idb, source, dense_group_only);
    }

    /*
     * Check if this neighbor should be DR for interface.
     */
    if (!is_p2p(idb) && source > idb->pim_dr) {
	PIM_BUGINF(0, ("\nPIM: Changing DR for %s, from %i to %i",
		       idb->namestring, idb->pim_dr, source));
	idb->pim_dr = source;
	igmpidb = igmp_getidb(idb);
	if (igmpidb->version == IGMP_VERSION1) {
	    igmpidb->multicast_qr = FALSE;
	}
	idb->ip_multicast_dr = FALSE;

    }
}

/*
 * pim_rp_needs_to_join
 *
 * This function is used by pim_receive_prune() to decide if we are the
 * RP, and if we should consistently send joins due to the (S,G)iif in
 * the (*,G) olist.
 */
boolean pim_rp_needs_to_join (gdbtype *gdb, mdbtype *mdb)
{
    midbtype *midb;

    if (ip_ouraddress(gdb->rp.address)) {
	midb = ip_get_midb(&gdb->mdb.if_outputQ, mdb->if_input,
			   mdb->rpf_nbr, TRUE);
	if (midb) return(TRUE);
    }
    return(FALSE);
}

/*
 * pim_find_nbr
 *
 * Find PIM neighbor data structure on interface.
 */
pim_nbrtype *pim_find_nbr (ipaddrtype nbr_address, idbtype *idb)
{

    pim_nbrtype *nbr;

    if (!idb) return(NULL);

    for (nbr = idb->pim_nbrs; nbr; nbr = nbr->next) {
	if (nbr_address == nbr->nbr_address) break;
    }
    return(nbr);
}

/*
 * pim_expire_nbrs
 *
 * Time-out neighbor entries if there were no Router-Queries received.
 */
void pim_expire_nbrs (idbtype *idb)
{

    pim_nbrtype *nbr, *prev;
    ipaddrtype  best_addr, old_dr, our_address;
    boolean     interface_down;
    igmpidbtype *igmpidb;
    
    best_addr = 0;
    prev = NULL;
    interface_down = !interface_up(idb);

    /*
     * Traverse all neighbors for this interface.
     */
    for (nbr = idb->pim_nbrs; nbr; nbr = prev->next) {

	/*
	 * Neighbor has expired.
	 */
	if (AWAKE(nbr->expires) || interface_down) {
	    PIM_BUGINF(0, ("\nPIM: Neighbor %i (%s) %s", nbr->nbr_address, 
			   idb->namestring, (interface_down) ? 
			   "down, interface down" : "timed out"));

	    /*
	     * Delink from linked list and free memory.
	     */
	    if (prev) {
		prev->next = nbr->next;
	    } else {
		idb->pim_nbrs = nbr->next;
	    }
	    free(nbr);
	    nbr = prev;
	} else {

	    /*
	     * Only consider neighbors that haven't expired.
	     */
	    if (nbr->nbr_address > best_addr) best_addr = nbr->nbr_address;
	    prev = nbr;
	}
	if (!prev) break;
    }

    if (is_p2p(idb)) return;

    /*
     * Reset DR status if there was a change.
     */
    our_address = idb->ip_address;
    if (!our_address && idb->ip_unnumbered) {
	our_address = idb->ip_unnumbered->ip_address;
    }
    old_dr = idb->pim_dr;
    igmpidb = igmp_getidb(idb);
    if (best_addr && best_addr > our_address) {
	idb->pim_dr = best_addr;
	if (igmpidb->version == IGMP_VERSION1) {
	    igmpidb->multicast_qr = FALSE;
	}
	idb->ip_multicast_dr = FALSE;
    } else {
	idb->pim_dr = our_address;
	if (igmpidb->version == IGMP_VERSION1) {
	    igmpidb->multicast_qr = TRUE;
	}
	idb->ip_multicast_dr = TRUE;

    }

    if (old_dr != idb->pim_dr) {
	PIM_BUGINF(0, ("\nPIM: Changing DR for %s, from %i to %i %s",
		       idb->namestring, old_dr, idb->pim_dr,
		       (idb->pim_dr == idb->ip_address) ? "(this system)" : 
		       " "));
    }
}
static const uchar pim_nbr_banner[] = 
    { "Neighbor Address  Interface          Uptime    Expires   Mode" };

/*
 * pim_show_neighbors
 */
void pim_show_neighbors (parseinfo *csb)
{
    
    idbtype     *one_idb, *idb;
    pim_nbrtype *nbr;
    uchar       uptime[20];
    uchar       expires[20];
    list_element *l_elt;
    
    one_idb = GETOBJ(idb,1);

    printf("PIM Neighbor Table");
    automore_enable(pim_nbr_banner);
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (one_idb) idb = one_idb;
	for (nbr = idb->pim_nbrs; nbr; nbr = nbr->next) {
	    if (!TIMER_RUNNING_AND_AWAKE(nbr->expires)) {
		sprint_ticks_in_dhms(expires, TIMER_RUNNING(nbr->expires) ?
				    TIME_LEFT_SLEEPING(nbr->expires) : 0);
	    } else {
		sprintf(expires, "now");
	    }
	    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(nbr->uptime));
	    printf("\n%18i%19s%10s%10s%13s%s", nbr->nbr_address, 
		   idb->namestring, uptime, expires, 
		   (nbr->mode == PIM_DENSE_MODE) ? "Dense" :
		    (nbr->mode == PIM_SPARSE_MODE ) ? "Sparse" : "Sparse-Dense",
		   (nbr->nbr_address == idb->pim_dr) ? "(DR)" : " ");
	}
	if (one_idb) break;
    }
    automore_disable();
}
static const uchar pim_int_count_banner[] = { "
Address          Interface          FS  Mpackets In/Out" };


/*
 * pim_show_interface_count
 *
 * Show detail counts for PIM interface.
 */
static void pim_show_interface_count (parseinfo *csb)
{
    idbtype *idb, *one_idb;
    ipaddrtype ipaddr;
    list_element *l_elt;

    one_idb = GETOBJ(idb,1);

    automore_enable(pim_int_count_banner);
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->pim_enabled) continue;
	if (one_idb) {
	    idb = one_idb;
	    if (!idb->pim_enabled) break;
	}

	ipaddr = idb->ip_address;
	if (!ipaddr && idb->ip_unnumbered)
	    ipaddr = idb->ip_unnumbered->ip_address;
 	printf("\n%17i%19s%4s%d/%d", ipaddr, idb->namestring,
	       (idb->ip_multicast_fs) ? "*" : " ", idb->ip_multicast_in, 
	       idb->ip_multicast_out);

	if (one_idb) break;
	if (automore_quit()) break;
    }
    automore_disable();
}

static const uchar pim_int_banner[] = { "
Address          Interface          Mode         Nbr   Query     DR
                                                 Count Intvl" };

/*
 * pim_show_interface
 *
 * Process "show ip pim interface [<interface-unit>]".
 */
void pim_show_interface (parseinfo *csb)
{

    idbtype     *idb, *one_idb;
    pim_nbrtype *nbr;
    int         count;
    ipaddrtype	ipaddr;
    list_element *l_elt;
    
    /*
     * Show counts only.
     */
    if (GETOBJ(int,1)) {
	pim_show_interface_count(csb);
	return;
    }

    one_idb = GETOBJ(idb,1);

    automore_enable(pim_int_banner);
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->pim_enabled) continue;
	if (one_idb) {
	    idb = one_idb;
	    if (!idb->pim_enabled) break;
	}
	count = 0;
	for (nbr = idb->pim_nbrs; nbr; nbr = nbr->next) {
	    count++;
	}
	if (DVMRP_TUNNEL(idb) && DVMRP_ACTIVE(idb)) count = 1;

	ipaddr = idb->ip_address;
	if (!ipaddr && idb->ip_unnumbered)
	    ipaddr = idb->ip_unnumbered->ip_address;
 	printf("\n%17i%19s%13s%6d%7d%i", ipaddr, idb->namestring,
  	       (DVMRP_TUNNEL(idb)) ? "DVMRP" : 
 		(idb->pim_mode == PIM_DENSE_MODE) ? "Dense" :
		(idb->pim_mode == PIM_SPARSE_MODE) ? "Sparse" : "Sparse-Dense",
	       count, idb->pim_query_interval/ONESEC, idb->pim_dr);

	if (one_idb) break;
	if (automore_quit()) break;
    }
    automore_disable();
}

/*
 * pim_init_parm
 *
 * Initialize a PIM parameter block for parsing PIM Join/Prune packets. This
 * function is require to be called before entering pim_parse_packet() for
 * the first time in a loop.
 */
void pim_init_parm(pim_parmtype *parm, pimtype *pim, ulong paklen)
{

    parm->paklen = paklen;
    parm->pim = pim;
    parm->group = NULL;
    parm->source = NULL;
    parm->groups_left = pim->num_groups + 1;
    parm->sources_left = 0;
    parm->join_list = FALSE;
}

/*
 * pim_validate_header
 *
 * Verify that address lengths are 4-byte IP addresses.
 */
boolean pim_validate_header (pimtype *pim, int code)
{

    uchar *str;

    str = NULL;
    if (pim->maddr_length != sizeof(ipaddrtype)) {
	str = "multicast";
    }
    if (pim->addr_length != sizeof(ipaddrtype)) {
	str = "source";
    }

    if (str) {
	PIM_BUGINF(0, ("\nPIM: Invalid %s address length in %s", str,
		       pim_code_string(code)));
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pim_parse_packet
 *
 * Parse a Join/Prune packet. Continue walk through packet when return value
 * is TRUE.
 */
boolean pim_parse_packet (pim_parmtype *parm)
{

    while (TRUE) {
	if (!parm->groups_left) break;

	/*
	 * Not enough buffer for group entry, end parse traversal.
	 */
	if (parm->paklen < PIM_ADDRTYPE_SIZE) break;

	/*
	 * If there are more sources left for this group, return new 
	 * source, and adjust sources_left count and packet length.
	 */
	if (parm->sources_left) {
	    parm->sources_left--;
	    parm->source++;
	    parm->paklen -= PIM_ADDRTYPE_SIZE;
	    return(TRUE);
	} else if (parm->join_list) {

	    /*
	     * There are no more join sources in packet. Traverse the prune
	     * sources and loop. 
	     */
	    parm->sources_left = parm->group->num_prune_sources;
	    parm->join_list = FALSE;
	} else {

	    /*
	     * There are no more join or prune sources for this group. See
	     * if there are more groups. Loop.
	     */
	    if (parm->paklen < sizeof(pim_grouptype)) break;
	    parm->groups_left--;

	    if (parm->groups_left) {
		parm->paklen -= sizeof(pim_grouptype);

		/*
		 * This handles both the initial case and when there are 
		 * multiple groups in a packet.
		 */
		if (parm->source) {
		    parm->group = (pim_grouptype *) (parm->source + 1);
		} else {
		    parm->group = (pim_grouptype *) parm->pim->data;
		}
		parm->sources_left = parm->group->num_join_sources;
		parm->join_list = TRUE;

		/*
		 * Set pointer to first source for new group, then back
		 * up so next interation points to right place.
		 */
		parm->source = (pim_addrtype *) (parm->group + 1);
		parm->source--;
	    }
	}
    }
    parm->group = NULL;
    return(FALSE);
}

/*
 * pim_parse_getnext_group
 *
 * Skip over sources for a particular group in a PIM Join/Prune message.
 * Returns TRUE, if there is another group in packet.
 */
boolean pim_parse_getnext_group (pim_parmtype *parm)
{

    ipaddrtype group;

    group = parm->group->group;
    while (pim_parse_packet(parm)) {
	if (group != parm->group->group) break;
    }
    return((boolean) parm->group);
}

/*
 * pim_parse_goto_prunes
 *
 * Skip over join sources for a particular group in a PIM Join/Prune message.
 * This gets you to the prune list, if any. This function assumes that you
 * are in the middle of the join list. Returns FALSE, if no prune-list for
 * current group.
 */
boolean pim_parse_goto_prunes (pim_parmtype *parm)
{

    ipaddrtype group;

    group = parm->group->group;
    while (pim_parse_packet(parm)) {
	if (group == parm->group->group && !parm->join_list) return(TRUE);
	if (group != parm->group->group) break;
    }
    return(FALSE);
}

/*
 * pim_send_join_prune
 *
 * Send join or prune for single (S,G) out specified interface.
 */
void pim_send_join_prune (mdbtype *mdb, idbtype *idb, ipaddrtype nbr, 
			  boolean join_request, boolean wc_bit, boolean rp_bit,
			  boolean sparse_bit)
{

    paktype       *pak;
    gdbtype       *gdb;
    igmptype      *igmp;
    pimtype       *pim;
    pim_grouptype *group;
    pim_addrtype  *addr;
    int           size;
    ipaddrtype    source;

    /*
     * If there are no PIM neighbors on interface, don't send anything. Or
     * if the neighbor is not a PIM router.
     */
    if (!idb || !idb->pim_nbrs) return;
    if (!pim_find_nbr(nbr, idb)) return;

    /*
     * If we are sending a Join or Prune for a source we are directly
     * connected, return. Exclude this check for sending to the RP.
     */
    if (!rp_bit && samecable(ip_radix2addr(mdb->source)) == idb) return;

    /*
     * If the (S,G) incoming interface  is an administratively scoped
     * boundary, and this is a join message, don't send the packet.
     */
    if (ipmulticast_test_boundary(idb, mdb->group) && join_request) {
	PIM_BUGINF(mdb->group, ("\nPIM: (No join sent across boundary)"));
	return;
    }
    
    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype) + 
	+ sizeof(pim_grouptype) + PIM_ADDRTYPE_SIZE;

    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_JOIN_PRUNE_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Build PIM fixed part of header.
     */
    pim = (pimtype *) igmp->data;
    pim->address = nbr;
    pim->holdtime = (PIM_MESSAGE_INTERVAL * PIM_MULTIPLIER) / ONESEC;
    pim->reserved1 = 0;
    pim->reserved2 = 0;
    pim->maddr_length = sizeof(ipaddrtype);
    pim->addr_length = sizeof(ipaddrtype);
    pim->num_groups = 1;

    /*
     * Build PIM group part of message.
     */
    group = (pim_grouptype *) pim->data;
    group->group = mdb->group;
    group->group_mask = MASK_HOST;
    group->num_join_sources = (join_request) ? 1 : 0;
    group->num_prune_sources = (!join_request) ? 1 : 0;
    addr = (pim_addrtype *) (group + 1);
    addr->pad = 0;
    addr->sparse_bit = sparse_bit;
    addr->wc_bit = wc_bit;
    addr->rp_bit = rp_bit;

    addr->mask_length = ip_bitsinmask(ip_radix2addr(mdb->mask));
    source = ip_radix2addr(mdb->source);
    PUTLONG(addr->address, source);

    if (PIM_DEBUG(group->group)) {
	buginf("\nPIM: Send %s on %s to %i for (%i/%d, %i)", 
	       (join_request) ? "Join" : "Prune", idb->namestring, 
	       nbr, source, addr->mask_length, group->group);
	if (rp_bit) buginf(", RP-bit");
    }

    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    /*
     * Send to IP.
     */
    gdb = ip_get_gdb(mdb->group);
 
    if (!idb->pim_nbma_mode || (gdb && IS_DENSE_MODE(idb, gdb))) {
	nbr = ALLROUTERS;
    }
    ipwrite(pak, size, IGMP_PROT, 0, nbr, idb, NULL, &igmprouteparams);
    igmp_traffic.output_pim++;
}

/*
 * pim_receive_join_prune
 *
 * Process received PIM Join/Prune packet.
 */
void pim_receive_join_prune (igmptype *igmp, idbtype *if_input, 
			     ipaddrtype source, ulong paklen)
{

    pimtype      *pim;
    pim_parmtype parm;

    pim = (pimtype *) igmp->data;

    /*
     * Validate fixed PIM header.
     */
    if (!pim_validate_header(pim, igmp->code)) return;

    /*
     * Packet length passed in does not include IP and IGMP headers.
     * Leave length of PIM message body.
     *
     * Process join list for all groups in packet.
     */
    pim_init_parm(&parm, pim, paklen);
    pim_receive_join(&parm, if_input, source, pim->address,
		     pim->holdtime * ONESEC);

    /*
     * Setup parm block to point to begining of packet. Then process prune 
     * list for all groups in packet.
     */
    pim_init_parm(&parm, pim, paklen);
    pim_receive_prune(&parm, if_input, source, pim->address,
		      pim->holdtime * ONESEC);
}

/*
 * pim_receive_prune
 *
 * Process PIM prune message. This function only looks at the prune-list in the
 * message. The (S,G) entries for unknown groups (Gs) are ignored. 
 *
 * If a prune is received on a point-to-point link, put the interface in
 * Prune state. Do the following procedures for multi-access links:
 *
 * If prune is addressed to this system, schedule a deletion of an interface
 * from the outgoing interface list for (S,G). If any joins come in during
 * the schedule period, cancel the deletion. This is done because one router
 * on a LAN may not want packets but other routers do.
 *
 * If the prune is not addressed to this system (it has been data link
 * multicast), we should determine if we expect packets on the interface
 * for (S,G). If so, schedule sending a join message. If another router's
 * join is received, don't cancel sending the join.
 */
void pim_receive_prune (pim_parmtype *parm, idbtype *if_input, 
			ipaddrtype prune_sender, ipaddrtype target, 
			ulong holdtime)
{

    gdbtype      *gdb;
    mdbtype      *mdb;
    midbtype     *midb;
    ipaddrtype   addr, mask;
    boolean	 to_us;

    /*
     * Only process prune entries.
     */
    gdb = NULL;
    to_us = ip_ifaddress(if_input, target) ? TRUE : FALSE;

    while (pim_parse_packet(parm)) {

	/*
	 * Skip over the join entries, if any.
	 */
	if (parm->join_list) {
	    if (!pim_parse_goto_prunes(parm)) continue;
	}

	/*
	 * Find (*,G) entry.
	 */
	if (!gdb || gdb->mdb.group != parm->group->group) {
	    gdb = ip_get_gdb(parm->group->group);
	}

	/*
	 * Find (S,G) entry in routing table.
	 */
	mask = ip_bitcount2mask(parm->source->mask_length);
	addr = GETLONG(parm->source->address);
	mdb = (gdb) ? ip_get_best_mdb(gdb, addr) : NULL;

	if (parm->source->wc_bit) {
	    PIM_BUGINF(parm->group->group, 
		       ("\nPIM: Prune-list: (*, %i) ", parm->group->group));
	    if (!gdb) continue;
	    mdb = &gdb->mdb;
	} else {
	    PIM_BUGINF(parm->group->group, 
		       ("\nPIM: Prune-list: (%i/%d, %i) %s", addr, 
			parm->source->mask_length, parm->group->group,
		       (parm->source->rp_bit) ? "RP-bit set " : " "));
	}
	if (parm->source->rp_bit && parm->source->wc_bit) {
	    PIM_BUGINF(parm->group->group, ("RP %i", addr));
	}
	
	/*
	 * If no state exists and this prune is not addressed
	 * to us, skip it so no local state is created. Note the test
	 * is on mdb.
	 */
	if (!mdb && !to_us) continue;
	
	/*
	 * If prune not addressed to this system, another router on the
	 * LAN is informing sending router (the one that has this LAN in its
         * outgoing interface list) to prune. If we still want packets,
	 * send delayed join.
	 */
	if (!is_p2p(if_input) && !to_us) {
	    if (if_input->pim_nbma_mode && IS_SPARSE_GROUP(gdb)) continue;
	    if (mdb && mdb->if_input == if_input) {
		if (!MDB_OUTPUTQ_NULL(mdb) ||
		    pim_rp_needs_to_join(gdb, mdb)) {
		    pim_schedule_join(mdb);
		}
	    }
	    continue;
	}

	/*
	 * Prune is addressed to this system or is received on point-to-point
	 * link.
	 *
	 * Find interface that Prune was received on in outgoing interface
	 * list. If found, set delay timer for changing to Prune state if
	 * received on a multi-access media. If not, create state only for
 	 * (S, G, RP-bit) prunes.
	 */
	if (!mdb) {
	    if (parm->source->rp_bit) {
		pim_add_mroute(&gdb, &mdb, parm->group->group, addr, mask, 
			       if_input, 0, MIDB_PRUNE_STATE,
			       holdtime, FALSE);
	    }
	    if (!mdb) continue;
	    pim_send_join_prune(mdb, gdb->mdb.if_input, 
				gdb->mdb.rpf_nbr, FALSE, FALSE, TRUE, TRUE);
	    if (!ip_ouraddress(gdb->rp.address)) {
		mdb->flags |= RP_BIT_FLAG;
		ip_rpf_check_mroute(gdb, mdb);
	    }
	}

	/*
	 * If the RP-bit was sent in the (S,G) Prune, the RP-bit is not set 
	 * in the entry and there is a null outgoing interface list, we want 
	 * to point this (S,G) towards the RP.
	 *
	 * If we are getting a Prune on the incoming interface, ignore and
	 * don't make it an RP-bit entry. Also, only do RP-bit processing
	 * if you are not the RP.
	 */
        if (!ip_ouraddress(gdb->rp.address)) {
	    if (parm->source->rp_bit && !(mdb->flags & RP_BIT_FLAG) && 
		MDB_OUTPUTQ_NULL(mdb) && mdb->if_input != if_input) {
  	        mdb->flags |= RP_BIT_FLAG;
 	        ip_rpf_check_mroute(gdb, mdb);
 	      }
	 }
  
	/*
	 * Update (S,G) timer when an (S,G,RP-bit) Prune is received and
	 * the entry is a negative cache entry.
	 */
	if (MDB_OUTPUTQ_NULL(mdb) && parm->source->rp_bit && 
	    !parm->source->wc_bit) {
	    TIMER_START(mdb->expires, MDB_EXPIRATION_TIME); 
	}

	/*
         * Process outgoing interface for interface Prune was received on.
	 */
	midb = ip_get_midb(&mdb->if_outputQ, if_input, prune_sender,
			   IS_SPARSE_GROUP(gdb));
	if (!midb) continue;
	TIMER_START(midb->expires, MDB_EXPIRATION_TIME);
	
	if (midb->state == MIDB_FORWARD_STATE) {

	    /*
	     * If there are directly connected members or DVMRP neighbors
	     * present on interface, don't prune interface.
	     */
	    if (DVMRP_ACTIVE(if_input) || 
		igmp_find_group(mdb->group, if_input)) continue;

	    /*
	     * Prune this interface now, if it is p2p, or the prune
	     * was for (S,G) RP-bit.
	     */
	    if (is_p2p(if_input) ||
		(parm->source->rp_bit && !parm->source->wc_bit)) {
		pim_prune_interface(gdb, mdb, midb, MDB_EXPIRATION_TIME);
		continue;
	    }
	    TIMER_START(midb->prune_time, PIM_PRUNE_DELAY);
	    PIM_BUGINF(parm->group->group,
	       ("\nPIM: Schedule to prune %s for (%i/%d, %i)", 
		midb->idb->namestring, addr, parm->source->mask_length, 
		parm->group->group));
	}
    }
}

/*
 * pim_receive_join
 *
 * Process PIM join message.
 */
void pim_receive_join (pim_parmtype *parm, idbtype *if_input, 
		       ipaddrtype join_sender, ipaddrtype target, 
		       ulong holdtime)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    ipaddrtype addr, mask;
    boolean    to_us;
    pim_rptype *rp;
    
    /*
     * Process the join-list only.
     */
    gdb = NULL;
    to_us = ip_ifaddress(if_input, target) ? TRUE: FALSE;

    PIM_BUGINF(0, (to_us ? ", to us" : ", not to us"));

    while (pim_parse_packet(parm)) {

	/*
	 * Finished all join-list for this group, check next group.
	 */
	if (!parm->join_list) {
	    if (!pim_parse_getnext_group(parm)) break;
	}

	/*
	 * Find (*,G) entry.
	 */
	if (!gdb || gdb->mdb.group != parm->group->group) {
	    gdb = ip_get_gdb(parm->group->group);
	}

	/*
	 * If no state exists and join message not addressed to this
	 * system, skip it. This check skips unwanted (*,G) immediately,
	 * a (S,G) join addressed to someone else can still fall
	 * over this test if no (S,G) entry exists but gdb exists.
	 * Such a join will be skipped later, since no new (S,G) state is
	 * created when we get to the join suppression logic.
	 */
	if (!gdb && !to_us) continue;

	mask = ip_bitcount2mask(parm->source->mask_length);
	addr = GETLONG(parm->source->address);

	/*
	 * Find (S,G) entry in routing table or (*,G) if WC-bit is set. If
	 * the WC-bit is set, we assume the source address is the RP. Store
	 * this information in the mdb for (*,G).
	 */
	mdb = NULL;
	if (parm->source->wc_bit) {
	    PIM_BUGINF(parm->group->group, 
		       ("\nPIM: Join-list: (*, %i)", parm->group->group));
	    if (parm->source->rp_bit) {
		boolean    newgdb = FALSE;

		PIM_BUGINF(parm->group->group, (" RP %i", addr));

		/*
		 * Test that the RP-address is a valid unicast address.
		 */
		if (martian(addr, TRUE)) {
		    PIM_BUGINF(0, ("\nPIM: Invalid RP address %i, RP ignored",
				   addr));
		    continue;
		}

		/*
		 * No (*,G) state, create it.
		 */
		if (!gdb) {
		    ip_add_mroute(parm->group->group, 0, MASK_HOST, NULL, 0, 
				  0, holdtime);
		    gdb = ip_get_gdb(parm->group->group);
		    if (!gdb) continue;
		    newgdb = TRUE;
		}

		/*
		 * If we accept the RP, store it. Otherwise, just create the
		 * state so it can be flagged to a network admin. Also issue
		 * a log message.
		 */
		if (pim_accept_rp(addr, gdb->mdb.group)) {
		    rp = pim_find_rp(gdb->mdb.group, addr);
		    pim_store_rp_info(gdb, addr, RP_UP, TRUE,
				      rp ? !rp->configured : TRUE);
		} else {
		    errmsg(&msgsym(INVALID_RP_JOIN, PIM), gdb->mdb.group,
			   join_sender, addr);
		    continue;
		}
		if (newgdb) ip_build_outputQ(NULL, gdb, &gdb->mdb);
		addr = 0;
		mask = MASK_HOST;
		mdb = &gdb->mdb;
	    }
	} else {
	    mdb = (gdb) ? ip_get_best_mdb(gdb, addr) : NULL;
	    PIM_BUGINF(parm->group->group, 
		       ("\nPIM: Join-list: (%i/%d, %i)", addr, 
			parm->source->mask_length, parm->group->group));
	}

	/*	
	 * For debugging only.
	 */
	if (parm->source->rp_bit) {
	    PIM_BUGINF(parm->group->group, (", RP-bit set"));
	}
	if (parm->source->sparse_bit) {
	    PIM_BUGINF(parm->group->group, (", S-bit set"));
	}

	/*
	 * If if_input is an administratively scoped boundary,
	 * don't bother to create S,G, add outgoing interface,
	 * or participate in join suppresion.
	 */
        if (ipmulticast_test_boundary(if_input, parm->group->group)) {
             PIM_BUGINF(parm->group->group,
                        ("\nPIM: No addition to olist at scoped boundary"));
	     continue;
	}

	/*
	 * If join address to this system, add interface to outgoing list.
	 * Otherwise, cancel sending a join if one was scheduled.
	 */
	if (to_us) {
	    pim_add_mroute(&gdb, &mdb, parm->group->group, addr, mask, 
		   if_input, join_sender, MIDB_FORWARD_STATE, holdtime,
                   parm->source->sparse_bit);
	    if (!mdb) continue;

	    if (&gdb->mdb != mdb) {

		/*
		 * If RP-bit is set in packet and not in entry, set in
		 * entry and change RPF interface/nbr to point towards
		 * RP.
		 */
		if (parm->source->rp_bit && !(mdb->flags & RP_BIT_FLAG)) {
		    mdb->flags |= RP_BIT_FLAG;
		    ip_rpf_check_mroute(gdb, mdb);
		} 

		/*
		 * If RP-bit is cleared in packet and is set in entry, clear
		 * bit in entry and change RPF interface/nbr to point towards
		 * source.
		 */
		if (!parm->source->rp_bit && (mdb->flags & RP_BIT_FLAG)) {
		    mdb->flags &= ~RP_BIT_FLAG;
		    ip_rpf_check_mroute(gdb, mdb);
		}
	    } else {

		/*
		 * If this is a (*,G) entry, update outgoing list entry for 
		 * each (S,G) if in sparse mode. Update (*,G) timer so it
		 * stays alive when there are no (S,G) entries associated with
		 * group.
		 */
		if (IS_SPARSE_GROUP(gdb) && &gdb->mdb == mdb) {
		    TIMER_START(gdb->mdb.expires, holdtime);
		    					
		    rn_walktree(gdb->radix_head->rnh_treetop, 
				pim_add_one_mroute_idb, if_input, TRUE, gdb,
				join_sender);
		}
	    }
	} else {

	    /*
	     * Not addressed to this system. Doing Join suppression.
	     */
	    if (!mdb) continue;
	    if (!gdb || mdb == &gdb->mdb) continue;
	    if (mdb->if_input == if_input && TIMER_RUNNING(mdb->send_join)) {
		TIMER_STOP(mdb->send_join);
		PIM_BUGINF(mdb->group, 
			   ("\nPIM: Cancel sending Join for (%i/%d, %i) on %s",
			    ip_radix2addr(mdb->source), 
			    ip_bitsinmask(ip_radix2addr(mdb->mask)),
			    mdb->group, if_input->namestring));
	    }
	}
    }
}

/*
 * pim_prune_interface
 *
 * Set an midb in Prune state. Trigger a prune if all outgoing interfaces
 * have just gone to prune state. If the interface is configured as dense-mode,
 * keep the interface in the outgoing list in Prune state. If the interface
 * is configured as sparse-mode, delete it from outgoing interface list.
 */
void pim_prune_interface (gdbtype *gdb, mdbtype *mdb, midbtype *midb,
			  ulong prune_time)
{

    midbtype *midb2;

    /*
     * Clear indication to prune (delete) interface.
     */
    TIMER_STOP(midb->prune_time);

    midb->state = MIDB_PRUNE_STATE;
    TIMER_START(midb->expires, prune_time);

    PIM_BUGINF(mdb->group, 
	       ("\nPIM: Prune %s/%i from (%i/%d, %i)", midb->idb->namestring,
		midb->next_hop, ip_radix2addr(mdb->source), 
		ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group));

    /*
     * Sparse-mode interfaces get deleted. Dense-mode interfaces stay
     * in outgoing interface list in Prune state.
     */
    if (!IS_DENSE_MODE(midb->idb, gdb) && !DVMRP_ACTIVE(midb->idb)) {
	ip_delete_mroute_idb(mdb, midb->idb, midb->next_hop);
	PIM_BUGINF(mdb->group, (" - deleted"));
    }

    for (midb2 = (midbtype *) mdb->if_outputQ.qhead; midb2; 
	 midb2 = midb2->next) {
	if (midb2->state == MIDB_FORWARD_STATE) break;
    }
    if (midb2) return;

    mdb->flags |= PRUNE_FLAG;
    mdb->send_graft = FALSE;
    ip_store_mdb_timer(mdb);
    TIMER_START(mdb->expires, mdb->expiration_time);
    
    /*
     * Send prune at next schedule time. If prune timer already set, make
     * sure that we clear the state that indicates a prune should go up
     * a non-RPF interface. Negative cache entries should have prunes sent
     * to the RPF neighbor before non-RPF neighbor prunes.
     */
    if (!TIMER_RUNNING(mdb->send_prune)) GET_TIMESTAMP(mdb->send_prune);

    for (midb2 = (midbtype *) mdb->if_outputQ.qhead; midb2; 
	 midb2 = midb2->next) midb2->send_prune = FALSE;
}

/*
 * pim_add_mroute_idb
 *
 * Add interface to all groups that we have known state for.
 * The interface is only added to dense groups if dense_group_only flag
 * is set to TRUE.
 */
void pim_add_mroute_idb (idbtype *idb, ipaddrtype join_sender,
			 boolean dense_group_only)
{

    gdbtype *gdb;
    int     i;
    boolean send_graft;
    va_list args[4];

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {

	    /*
	     * If we are asked to only add this interface to dense groups,
	     * and this group is in sparse mode, skip.
	     */
	    if (dense_group_only &&
		(gdb->rp.address || pim_find_rp(gdb->mdb.group, 0))) {
		continue;
	    }

	    /*	
	     * Decide if triggered Graft should be sent.
	     */
	    send_graft = FALSE;
	    if (idb->ip_multicast_dr) {
		GET_TIMESTAMP(gdb->send_graft_time);
		send_graft = TRUE;
	    }

	    /*
	     * Process each (Si,G).
	     */
	    rn_walktree(gdb->radix_head->rnh_treetop, pim_add_one_mroute_idb, 
			idb, send_graft, gdb, join_sender);
	    
	    /*
	     * Process the (*,G) entry itself.
	     */
	    send_graft = FALSE;
	    args[0] = (va_list) idb;
	    args[1] = (va_list) send_graft;
	    args[2] = (va_list) gdb;
	    args[3] = (va_list) join_sender;
	    pim_add_one_mroute_idb((rntype *) &gdb->mdb, (va_list) args);
	}
    }
}

/*
 * pim_add_one_mroute_idb
 *
 * Add interface to outgoing interface list for a given multicast route entry.
 * Set an expiration timer for the entry.
 */
int pim_add_one_mroute_idb (rntype *node, va_list args)
{
    mdbtype    *mdb;
    gdbtype    *gdb;
    idbtype    *idb;
    boolean    send_graft;
    boolean    negative;
    ipaddrtype join_sender;
    
    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);
    send_graft = va_arg(args, boolean);
    gdb = va_arg(args, gdbtype *);
    join_sender = va_arg(args, ipaddrtype);
    negative = FALSE;

    if (MDB_OUTPUTQ_NULL(mdb)) negative = TRUE;

    ip_add_mroute_idb(gdb, mdb, idb, join_sender, MDB_EXPIRATION_TIME, 
		      MIDB_FORWARD_STATE, TRUE);

    /*
     * We had a negative cache entry and caller wants to graft. Flag (S,G)
     * entry to include it in the Join-list of the Graft. Set the graft
     * time so the next scheduled interval we will send a graft.
     */
    if (negative && send_graft) {
	GET_TIMESTAMP(gdb->send_graft_time);
	mdb->send_graft = TRUE;
    }

    return(0);
}

/*
 * pim_schedule_join
 *
 * Set random delay timer to send join.
 */
void pim_schedule_join (mdbtype *mdb)
{

    ulong delay;

    /*
     * Join delay timer already set, don't reset it.
     */
    if (TIMER_RUNNING(mdb->send_join)) {
	PIM_BUGINF(mdb->group, 
	   ("\nPIM: Join delay timer already set for (%i/%d, %i) on %s",
	    ip_radix2addr(mdb->source), 
	    ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group, 
	    mdb->if_input->namestring));
	return;
    }

    delay = PIM_RANDOM_DELAY(mdb);
    TIMER_START(mdb->send_join, delay);

    PIM_BUGINF(mdb->group, 
       ("\nPIM: Set join delay timer to %d seconds for (%i/%d, %i) on %s",
	delay/ONESEC, ip_radix2addr(mdb->source), 
	ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group, 
	mdb->if_input->namestring));
}

/*
 * pim_add_mroute
 *
 * Add interface passed in received_idb to olist for the mdb passed by
 * reference in *mdb_ptr.  If *mdb_ptr is NULL, gdb and mdb state are
 * created will be returned in *gdb_ptr and *mdb_ptr if the interface was
 * successfully added.  Otherwise, NULL is returned.
 */
void pim_add_mroute (gdbtype **gdb_ptr, mdbtype **mdb_ptr, ipaddrtype group, 
	     ipaddrtype source, ipaddrtype mask, idbtype *received_idb, 
	     ipaddrtype join_sender, ulong state, ulong holdtime,
	     boolean sparse)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    midbtype   *midb;
    idbtype    *rpf_idb;
    ipaddrtype rpf_source, rpf_mask, rpf_nbr;
    uchar      rpf_type;
    boolean    no_gdb, to_the_rp, we_are_rp;

    mdb = *mdb_ptr;
    gdb = *gdb_ptr;
    *mdb_ptr = NULL;
    *gdb_ptr = NULL;
    no_gdb = gdb ? FALSE : TRUE;
    
    /*
     * No state for (S,G), create it.
     */
    if (!mdb) {
	rpf_nbr = 0;
	rpf_idb = ip_rpf_lookup(source, mask, &rpf_source, &rpf_mask, &rpf_nbr,
				&rpf_type, ROUTINGDEPTH);
	if (!rpf_idb) return;

	/*
	 * Create (S,G) entry.
	 */
	mdb = ip_add_mroute(group, source, mask, rpf_idb, rpf_nbr, rpf_type, 
			    holdtime);
	if (!mdb) return;
	if (!gdb) gdb = ip_get_gdb(mdb->group);
	if (!gdb) return;

	/*
	 * If no gdb existed when we were processing this (S,G) prune/join,
	 * ip_add_mroute() above would have created a (S,G) and a (*,G).
	 * But the (*,G) entry would have a NULL olist. We need to
	 * put the dense mode interfaces in the (*,G) olist here.
	 *
	 * Also, store the RP in the gdb and tag the group as Sparse.
	 */
	if (no_gdb) {
	    ip_build_outputQ(NULL, gdb, &gdb->mdb);

	    if (pim_find_rp(gdb->mdb.group, 0)) {
		pim_get_all_rps(gdb, TRUE, FALSE);
	    }
	}
	
	/*
	 * Copy (*,G) outgoing list into (S,G).
	 */
	if (mdb != &gdb->mdb) {
	    ip_build_outputQ(&gdb->mdb.if_outputQ, gdb, mdb);
	}

	/*
	 * Send trigger join/graft or prune to RPF neigbhor.
	 * When sending join/graft, we make sure we do not send out of
	 * an administratively scoped boundary.
	 */
	if (state == MIDB_FORWARD_STATE &&
	    !ipmulticast_test_boundary(rpf_idb, group)) {
	    if (!IS_DENSE_MODE(mdb->if_input, gdb) && !DVMRP_RPF_NBR(mdb)) {
		pim_send_join_prune(mdb, rpf_idb, rpf_nbr, TRUE, FALSE, FALSE,
				    sparse);
	    } else {
		mdb->send_graft = TRUE;
		GET_TIMESTAMP(gdb->send_graft_time);
	    }
	}
    }

    /*
     * Add interface that message was received on to outgoing list if
     * it is not the RPF interface, and if the RPF interface is not an
     * administrative boundary.
     */
    if (mdb->if_input != received_idb) {
	boolean was_pruned = MDB_OUTPUTQ_NULL(mdb);

	we_are_rp = ip_ouraddress(gdb->rp.address);

	if (ipmulticast_test_boundary(mdb->if_input, mdb->group)) {
             PIM_BUGINF(mdb->group,
                  ("\nPIM: RPF from boundary. interface not added to olist"));
             return;
	}

	ip_add_mroute_idb(gdb, mdb, received_idb, join_sender, holdtime,
			  state, TRUE);

	if (PIM_DEBUG(mdb->group)) {
	    buginf("\nPIM: Add %s/%i to (", received_idb->namestring,
		   join_sender);
	    if (&gdb->mdb == mdb) {
		buginf("*");
	    } else {
		buginf("%i/%d", ip_radix2addr(mdb->source),
		       ip_bitsinmask(ip_radix2addr(mdb->mask)));
	    }
	    buginf(", %i), %s state", mdb->group, midb_state_str[state]);
	}

	midb = ip_get_midb(&mdb->if_outputQ, received_idb,
			   join_sender, IS_SPARSE_GROUP(gdb));
	if (!midb) return;
	
	/*	       
	 * Cancel prune interface indication if we just went into 
	 * Forward state. (i.e. a join came in to nullify a previous prune).
	 * If it was a negative cache entry, and the (S,G) incoming
	 * interface is not an administratively scoped boundary,
	 * trigger a Join.
	 */

	to_the_rp = (mdb == &gdb->mdb);
	
	if (state == MIDB_FORWARD_STATE) {
	    mdb->flags &= ~(PRUNE_FLAG | RP_BIT_FLAG);
	    TIMER_STOP(mdb->send_prune);
	    TIMER_STOP(midb->prune_time);

   	    /*
   	     * Flag that a sparse-mode Join came in so when members go away
  	     * on a LAN, IGMP can delete interface from the olist quickly.
  	     */
   	    if (IS_SPARSE_GROUP(gdb) && !IS_DENSE_MODE(received_idb, gdb)) {
   		GET_TIMESTAMP(midb->last_join);
   	    }
  	    
	    if (was_pruned) {
		if (mdb->if_input) {
		    if (!ipmulticast_test_boundary(mdb->if_input, group)) {
			if (!IS_DENSE_MODE(mdb->if_input, gdb) &&
			    !DVMRP_RPF_NBR(mdb)) {
			    pim_send_join_prune(mdb, mdb->if_input,
						mdb->rpf_nbr, TRUE, to_the_rp,
						to_the_rp, sparse);
			} else {
			    mdb->send_graft = TRUE;
			    GET_TIMESTAMP(gdb->send_graft_time);
			}
		    }
		} else { 
		
		    /*
		     * We are the RP and have received a (*,G) join for 
		     * a group with no previous members, send joins immediately
		     * to all sources.
		     */
		    if (we_are_rp && to_the_rp) {
			pim_send_batch_join(gdb);
		    }
		}	
	    }
	}

	/*
	 * If interface went into prune state, we might have to set
	 * the PRUNE_FLAG in the mdb.
	 */
	if (state == MIDB_PRUNE_STATE) {
	    pim_prune_interface(gdb, mdb, midb, MDB_EXPIRATION_TIME);
	}
    }

    /*
     * Restore or set new values.
     */
    *mdb_ptr = mdb;
    *gdb_ptr = gdb;
    return;
}

/*
 * pim_delink_packet
 *
 * Delink a Graft packet from a linked list chain of packets for different
 * neighbors on an interface. Delink a periodic Join packet from a linked
 * list.
 */
void pim_delink_packet (paktype **pak_pptr, paktype *ip)
{

    paktype *pak, *prev;

    prev = NULL;
    for (pak = *pak_pptr; pak; pak = (paktype *) pak->dataptr) {
	if (pak == ip) break;
	prev = pak;
    }
    if (prev) {
	prev->dataptr = ip->dataptr;
    } else {
	*pak_pptr = (paktype *)ip->dataptr;
    }
    ip->dataptr = NULL;
}

/*
 * pim_get_metric
 *
 * Get unicast routing metric for source host. This is used to insert metrics
 * in Assert messages.
 */
void pim_get_metric (ipaddrtype source, uchar rpf_type, ulong *metric_pref, 
		     ulong *metric)
{

    ndbtype   *db;
    dvmrptype *dvmrp;
    pdbtype   *pdb;

    /*
     * Assume source is not reachable.
     */
    *metric_pref = PIM_ASSERT_INFINITY;
    *metric = (int) 0;

    /*
     * RPFing from DVMRP routing table.
     */
    if (rpf_type == RPF_DVMRP) {
	dvmrp = dvmrp_get_route(source, 0, TRUE);
	if (!DVMRP_VALID(dvmrp)) return;
	*metric_pref = dvmrp->distance;
	*metric = dvmrp->metric;
	return;
    }

    /*
     * RPFing from unicast routing table, lookup route.
     */
    db = ip_rtlookup(source, 0);

    if (!db) {
	if (!default_ndb) return;
	db = default_ndb;
	if (!db) return;
    }

    /*
     * Use metric from unicast routing table.
     */
    *metric = db->metric;

    /*
     * Get adminstrative distance, use it for metric preference.
     */
    pdb = INDEX2PDB(db->pdbindex);
    if (!pdb) return;

    *metric_pref = pdb->distance;
}

/*
 * pim_compare_metrics
 *
 * Compares the first set of metrics with the second set. If the first set
 * are better, TRUE is returned. That is the neighbor metrics are better.
 */
boolean pim_compare_metrics(ipaddrtype nbr_addr, ulong nbr_metric_pref, 
			    ulong nbr_metric, ipaddrtype local_addr, 
			    ulong local_metric_pref, ulong local_metric)
{

    /*
     * The neighbor has worse metric preference than our current 
     * RPF neighbor (or us). Don't change anything, return metric worse.
     */
    if (nbr_metric_pref > local_metric_pref) return(FALSE);

    /*
     * Compare metrics when metric preferences are the same. If our current
     * RPF neighbor (or us) has better metric, keep it. Return worse metric.
     */
    if (nbr_metric_pref == local_metric_pref && 
	nbr_metric > local_metric) return(FALSE);

    /*
     * If metric preference and metric are the same, use higher IP address.
     */
    if (nbr_metric_pref == local_metric_pref && nbr_metric == local_metric &&
	local_addr >= nbr_addr) return(FALSE);

    return(TRUE);
}

/*
 * pim_check_hsrp
 *
 * Check interface to determine if compatable to run HSRP and PIM 
 * concurrently
 */
boolean pim_check_hsrp (idbtype *idb)
{
    /* 
     * Due to the CAM on lowend FDDI (bsi chipset), it is not possible
     * to support both HSRP and PIM at the same time.
     */
    if ((idb->hwptr->status & IDB_FDDI) && 
	(idb->hwptr->type == IDBTYPE_BSI) && 
	standby_if_mincfg(idb)) {
	printf("\nPIM: Interface %s cannot simultaneously support both\n"
	       "Hot Standby and IP Multicast routing.", idb->namestring);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pim_access_list_callback
 *
 * This routine is called back when an access-list is changed.
 */
void pim_access_list_callback(acl_headertype *acl)
{
   /*
    * Only care about the change if it is a SIMPLE access list.
    */
   if (!acl) return;

   if (acl->type != ACL_IP_SIMPLE && acl->type != ACL_IP_NAMED_SIMPLE) return;

   PIM_BUGINF(0, ("\nPIM: Configured access-list %s has changed.\n",
		  acl->name));
   /*
    * A number of checks may be done when an access-list is changed.
    *
    * For now, we only check the configured RP list, so that
    * changes in the way CISCO-RP-ANNOUNCE and CISCO-RP-DISCOVERY
    * are supported are reflected immediately in the RP mapping
    * cache.
    */
   pim_check_rp_lists(acl);
}

/*
 * pim_traffic_stat
 * Statistics of PIM_PROT type traffic
 */
static void pim_traffic_stat (void)
{
    printf("\n\nPIMv2 statistics: Sent/Received");
    printf("\n  Total: %lu/%lu, %lu checksum errors, %lu format errors",
	   pim_traffic.outputs, pim_traffic.inputs,
	   pim_traffic.checksumerr,  pim_traffic.formaterr);
    printf("\n  Registers: %lu/%lu, Register Stops: %lu/%lu",
	   pim_traffic.output_registers, pim_traffic.input_registers,
	   pim_traffic.output_register_stops,
	   pim_traffic.input_register_stops);
}

/*
 * pim_enqueue
 * Find the PIM socket, and give it the datagram.
 */
static void pim_enqueue (paktype *pak)
{
    iphdrtype *ip;
    ipsocktype *soc;

    ip = (iphdrtype *) ipheadstart(pak);

    pim_traffic.inputs++;
    soc = find_pimsocket();
    if (soc && (soc->inallocated != soc->inallowed)) {
	soc->inallocated++;
	enqueue_ip_socket(soc, pak);
    } else {
	retbuffer(pak);
    }
} 

/*
 * find_pimsocket
 * Given an IP packet, return the PIM socket or NULL
 */
static ipsocktype *find_pimsocket (void)
{
    register ipsocktype *soc;

    for (soc = (ipsocktype *) IPsocketQ.qhead; soc; soc = soc->next) {
	if (soc->protocol == PIM_PROT) return (soc);
    }
    return(NULL);
}

/*
 * pimv2_process_packets
 * Process packets from the PIM routing socket
 */
static void pimv2_process_packets (ipsocktype * soc)
{
    paktype *pak;
    iphdrtype *ip;
    pimv2_header *pim;
    uchar *data;
    idbtype *idb;
    int length;

    /*
     * Extract packet from the PIM routing socket queue
     */
    while ((pak = read_ipsocket(soc, FALSE, 0, &pim, &data))) {
	ip = (iphdrtype *) ipheadstart(pak);
	length = ip->tl - (ip->ihl << 2);
	idb = pak->if_input;
	
	/*
	 * Validate length of packet.
	 */
	if (length < PIMHEADERBYTES) {
	   PIM_BUGINF(0, ("\nPIM: v2, Packet too short from %i (%s)",
			 ip->srcadr, idb ? idb->namestring : ""));
	   pim_traffic.formaterr++;
	   datagram_done(pak);
	   continue;
	}

	/*
	 * Checksum datagram
	 */
	if (ipcrc((ushort *) pim, (ip->tl - (ip->ihl << 2)))) {
	    PIM_BUGINF(0, ("\nPIM: v2, bad checksum from %i (%s)",
			 ip->srcadr, idb ? idb->namestring : ""));
	    pim_traffic.checksumerr++;
	    datagram_done(pak);
	    continue;
	}

	/*
	 * Validate version number
	 */
	if (pim->ver != PIMV2) {
	    PIM_BUGINF(0, ("\nPIM: v2, Bad version number %d from %i (%s)",
			pim->ver, ip->srcadr, idb ? idb->namestring: ""));
	    pim_traffic.formaterr++;
	    datagram_done(pak);
	    continue;
	}

        /*
         * Are we debugging?
         */
        if (pim_debug &&
            (!pim_group_debug || pim->type != PIMV2_HELLO_TYPE)) {
            buginf("\nPIM: Received v2 %s on %s from %i",
                    pim_code_string(pim->type), pak->if_input->namestring,
                    ip->srcadr);
        }

	/*
	 * Demux packet
	 */
	switch (pim->type) {

	case PIMV2_REGISTER_TYPE:
	    pim_traffic.input_registers++;
	    pimv2_receive_register(pak, pim, pak->if_input);
	    break;

	case PIMV2_REG_STOP_TYPE:
	    pim_traffic.input_register_stops++;
	    pimv2_receive_register_stop(pim, ip->srcadr, length);
	    break;

	default:
	   PIM_BUGINF(0, ("\nPIM: Unknown v2 packet type %d from %i (%s)",
			pim->type, ip->srcadr, idb ? idb->namestring : ""));
	}

	/*
	 * Done with the packet.
	 */
	datagram_done(pak);
    }
}


