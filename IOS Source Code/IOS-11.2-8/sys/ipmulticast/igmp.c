/* $Id: igmp.c,v 3.18.12.27 1996/09/10 05:31:38 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/igmp.c,v $
 *------------------------------------------------------------------
 * igmp.c - IP Internet Group Management Protocol support.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Supports router-to-host communication for joining/leaving IP multicast
 * groups.
 *------------------------------------------------------------------
 * $Log: igmp.c,v $
 * Revision 3.18.12.27  1996/09/10  05:31:38  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.18.12.26  1996/09/10  01:07:07  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.18.12.25  1996/09/04  17:01:27  dino
 * CSCdi67530:  Periodic duplicates occur on member LAN using sparse-mode.
 * Branch: California_branch
 *
 * Revision 3.18.12.24  1996/08/26  19:33:01  dino
 * CSCdi67023:  DVMRP route-limit set to no limit is not carried across
 *              reboots
 * Branch: California_branch
 *
 * Revision 3.18.12.23  1996/08/26  06:48:59  dino
 * CSCdi67098:  Do not accept DVMRP neighbor probes/reports from
 * non-pruners
 * Branch: California_branch
 *
 * Revision 3.18.12.22  1996/08/21  01:21:02  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.18.12.21  1996/08/13  06:21:55  lwei
 * CSCdi65831:  SPARSE flag should be set in entries on non-DR
 *              last-hop routers too
 * Branch: California_branch
 *
 * Revision 3.18.12.20  1996/08/07  23:50:25  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.18.12.19  1996/08/05  19:20:13  dino
 * CSCdi65068:  midb->mode is no longer needed. Configured mode need not
 *              ever change
 * Branch: California_branch
 *
 * Revision 3.18.12.18  1996/07/18  04:45:23  lwei
 * CSCdi63238:  Oif list is truncated if IGMP cache is gone on DVMRP
 *              active inteface
 * Branch: California_branch
 *
 * Revision 3.18.12.17  1996/06/28  07:48:21  dino
 * CSCdi61612:  Put in DVMRP route-limit, have it default to 7000, avoid
 *              MBONE meltdown.
 * Branch: California_branch
 *
 * Revision 3.18.12.16  1996/06/21  18:43:02  tylin
 * CSCdi58913:  DVMRP control packets are not counted in the tunnel idb
 * stats
 * Branch: California_branch
 *
 * Revision 3.18.12.15  1996/06/18  01:46:37  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.18.12.14  1996/05/14  00:12:33  mleelani
 * CSCdi56461:  European cisco MBONE needs ip dvmrp metric-offset out
 * command
 * Branch: California_branch
 * Support outgoing DVMRP metric offsets.
 *
 * Revision 3.18.12.13  1996/05/08  04:29:39  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.18.12.12  1996/05/07  18:40:52  mleelani
 * CSCdi56837:  Attempt to initialize non-existent timer
 * Branch: California_branch
 * Make sure PIM/IGMP is enabled on the interface before starting
 * the query timer.
 *
 * Revision 3.18.12.11  1996/05/04  01:08:12  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.18.12.10  1996/05/01  00:19:25  mleelani
 * CSCdi55930:  Locally joined groups has 00:00:00 in show ip igmp group
 * display
 * Branch: California_branch
 * Account for change in sprint_ticks_in_dhms. Fix cosmetic display bugs.
 *
 * Revision 3.18.12.9  1996/04/23  17:35:12  mleelani
 * CSCdi55369:  Crashes immediately after booting with corrupt stack
 * Branch: California_branch
 * Incorrectly processing reports lead to recursion.
 *
 * Revision 3.18.12.8  1996/04/20  01:51:03  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.18.12.7  1996/04/10  01:17:56  mleelani
 * CSCdi54195:  Local IGMP group membership not subscribed correctly
 * Branch: California_branch
 * Missing patch from CSCdi51919.
 *
 * Revision 3.18.12.6  1996/04/06  00:06:08  mleelani
 * CSCdi53650:  igmp_add_group() should send unsol Report for
 * IGMP_USER_FLAG calls
 * Branch: California_branch
 * Make user processes send unsolicited reports consistently
 *
 * Revision 3.18.12.5  1996/04/04  20:45:11  dino
 * CSCdi51177:  Jhawk wants uptime and syslog of DVMRP tunnel transitions
 * Branch: California_branch
 *
 * Revision 3.18.12.4  1996/04/03  23:19:07  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.18.12.3  1996/03/30  01:18:09  mleelani
 * CSCdi52935:  IGMP query router does not change when IP address is
 * changed
 * Branch: California_branch
 * o Reflect querier address change.
 * o Restart query timer when query-interval is changed
 *
 * Revision 3.18.12.2  1996/03/27  23:29:31  mleelani
 * CSCdi52589:  UI misinformation of Igmp version
 * Branch: California_branch
 * Fix UI version bug.
 *
 * Revision 3.18.12.1  1996/03/18  20:18:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.15.2.3  1996/03/13  01:38:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.15.2.2  1996/03/07  09:47:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.15.2.1  1996/02/20  01:01:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.18  1996/02/13  22:32:26  dino
 * CSCdi48373:  When a member leaves a group, (S,G) are not triggered in
 *              Prunes
 *
 * Revision 3.17  1996/02/08  20:11:55  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.16  1996/02/01  06:04:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.15  1996/01/19  01:22:07  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.14  1996/01/12  22:55:58  mleelani
 * CSCdi46802:  IGMP needs faster leave response
 * Miscellaneous v2 enhancements.
 *
 * Revision 3.13  1996/01/05  10:17:21  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.12  1995/12/21  20:44:22  dino
 * CSCdi45700:  Dense-mode forwarding router doesnt unprune quickly for
 *              new member
 *
 * Revision 3.11  1995/12/07  04:42:50  dino
 * CSCdi45175:  PIM, IGMP, and DVMRP should use TOS_PREC_INET_CONTROL
 *
 * Revision 3.10  1995/12/01  18:38:34  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.9  1995/11/30  19:45:39  mleelani
 * CSCdi44744:  Spurious access in config command of dvmrp
 * igmp_commands() can be invoked from global configuration commands also.
 *
 * Revision 3.8  1995/11/30  04:42:03  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.7  1995/11/26  10:17:17  mleelani
 * CSCdi43414:  fastethernet interface doesnt have room on sho ip igmp gr
 * Modify show output.
 *
 * Revision 3.6  1995/11/26  10:06:53  mleelani
 * CSCdi42232:  Group membership information shown on deleted tunnel
 * Show membership only on active interfaces.
 *
 * Revision 3.5  1995/11/22  06:57:19  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.4  1995/11/21  23:12:00  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  17:29:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:00:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/10/10  21:15:58  dino
 * CSCdi41878:  Monitor number of DVMRP routes received and errmsg()
 * during surges.
 *
 * Revision 2.12  1995/09/28  02:18:50  dino
 * CSCdi41187:  Mtrace is broken in a couple of scenarios and causes
 *              spurious errors
 *
 * Revision 2.11  1995/08/15  21:22:12  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.10  1995/08/03  19:56:22  dino
 * CSCdi38155:  Dont accept DVMRP packets on half-configured tunnels.
 *
 * Revision 2.9  1995/08/03  01:12:58  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * Revision 2.8  1995/07/26  05:13:13  hampton
 * Missed change cleaning up IP Multicast timers.  [CSCdi37539]
 *
 * Revision 2.7  1995/07/25 23:10:32  dino
 * CSCdi37661:  IGMP table is not cleared when more than one join is
 * received
 *
 * Revision 2.6  1995/07/24  07:33:34  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.5  1995/07/18 10:20:57  dino
 * CSCdi37233:  Local membership for sd not established when connected
 * member exists
 *
 * Revision 2.4  1995/07/10  18:39:59  dino
 * CSCdi36862:  Do faster pruning
 *
 * Revision 2.3  1995/07/01  07:46:11  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.2  1995/06/09  13:05:47  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:03:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <master.h>
#include <stdarg.h>
#include <interface_private.h>
#include <config.h>
#include <ieee.h>
#include <logger.h>
#include "../os/free.h"
#include "../os/signal.h"
#include "../if/network.h"
#include "parser_defs_igmp.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include <access.h>
#include "../ip/ipaccess.h"
#include "../ip/tunnel.h"
#include "../iprouting/route.h"
#include "ipmulticast_debug.h"
#include "mroute.h"
#include "igmp.h"
#include "dvmrp.h"
#include "pim.h"
#include "mbone_mtrace.h"
#include "sr_igmpmib.h"
#include "cgmp.h"
#include "../util/random.h"
#include <mgd_timers.h>
#include "igmp_private.h"
#include "../ui/common_strings.h"

/*
 * IGMP Storage.
 */
igmp_cachetype 	*igmp_cache[NETHASHLEN];
static pid_t 	igmp_running;
static watched_queue *igmpQ;
static sys_timestamp igmp_periodic_timer;
ipparamtype 	igmprouteparams = { 0, TOS_PREC_INET_CONTROL, 1, FRAGMENTS_ALLOWED, TRUE };
igmp_traffic_t igmp_traffic;

/*
 * igmp_alert_handler
 *
 * enqueue IGMP packets with RA to IGMP.
 *
*/
static void igmp_alert_handler (paktype *pak, boolean *absorbed)
{
    if (igmp_running) {
	process_enqueue(igmpQ, pak);
	*absorbed = TRUE;
    } else {
	*absorbed = FALSE;
    }
}

/*
 * igmp_init
 *
 * Initialize IGMP data structures.
 */
void igmp_init (void)
{

    /* Add a free list. */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(dvmrptype));

    /*
     * Initialize IGMP global variables.
     */
    igmp_running = 0;
    
    /*
     * Register some functions
     */
    ip_set_proto_outcounter(IGMP_PROT, &igmp_traffic.outputs);
    reg_add_ip_show_traffic(igmp_show_traffic, "igmp_show_traffic");
    reg_add_iprouting_alert(IGMP_PROT,igmp_alert_handler,"igmp_alert_handler");
    /*
     * Initialize IGMP parser chain.
     */
    igmp_parser_init();
}

/*
 * igmp_init_idb
 *
 * Initialize the IGMP fields in the idb.
 */
void igmp_init_idb (idbtype *idb)
{
    queue_init(&idb->ip_multiaddrQ, 0);
    idb->igmp_enabled = FALSE;
}

/*
 * igmp_input
 *
 * Called from ip_enqueue().
 */
void igmp_input (paktype *pak)
{
    if (igmp_running) {
	process_enqueue(igmpQ, pak);
    } else {
	datagram_done(pak);
    }
}

/*
 * igmp_process_do_packets
 *
 * Handle packets queued on the igmpQ.
 */
static void igmp_process_do_packets (void)
{
    iphdrtype  *ip;
    paktype    *pak;
    igmptype    *igmp;
    idbtype     *idb;
    igmpidbtype *igmpidb;
    ipaddrtype  source;
    int         length;
    
    /*
     * Process input queue.
     */
    while ((pak = process_dequeue(igmpQ))) {
	idb = pak->if_input;
	igmpidb = igmp_getidb(idb);
	ip = iphdrtype_start(pak);
	igmp = (igmptype *) ipdatastart_iph(ip);
	length = ip->tl - ipheadsize(ip);
	source = ip->srcadr;
	igmp_traffic.inputs++;

	/*
	 * Validate length of packet.
	 */
	if (length < IGMPHEADERBYTES) {
	    IGMP_BUGINF("\nIGMP: Packet too small from %i (%s)",
			source, idb_get_namestring(idb));
	    datagram_done(pak);
	    igmp_traffic.formaterr++;
	    continue;
	}
					  
	/*
	 * Checksum packet.
	 */
	if (ipcrc((ushort *) igmp, length)) {
	    IGMP_BUGINF("\nIGMP: Packet checksum error from %i (%s)",
			source, idb_get_namestring(idb));
	    datagram_done(pak);
	    igmp_traffic.checksumerr++;
	    continue;
	}

	/*
	 * Validate version number.
	 */
	if ((igmp->type & 0xF0) != IGMP_VERSION1_OR_2_TYPE) {
	    IGMP_BUGINF("\nIGMP: Bad version number %d from %i (%s)",
			((igmp->type & 0xF0) >> 4), source,
			idb_get_namestring(idb));
	    datagram_done(pak);
	    igmp_traffic.formaterr++;
	    continue;
	}

	/*
	 * Demux packet. Dispatch to IGMP, DVMRP, or PIM.
	 */
	switch (igmp->type) {
	case IGMP_QUERY_TYPE:
	    if (idb->igmp_enabled) {
		igmp_traffic.input_queries++;
		igmp_receive_query(igmp, source, idb);
	    }
	    break;


	case IGMP_NEW_REPORT_TYPE:
	    /*
	     * If we are in version1 router mode, act as one.
	     */
	    if (idb->igmp_enabled) {
		if (igmpidb->version == IGMP_VERSION1) {
		    IGMP_BUGINF("\nIGMP: Received v2 Report, from ");
		    IGMP_BUGINF("%i (%s) v1 interface",
				source, idb_get_namestring(idb));
                }
	    }
	    /* deliberate fall through*/

	case IGMP_REPORT_TYPE:
	    if (idb->igmp_enabled) {
		igmp_traffic.input_reports++;
		igmp_receive_report(igmp, source, ip->dstadr, idb);

		/*
		 * If CGMP is enabled and we are querier, send a CGMP 
		 * Join message in response to the received IGMP Report. 
		 * This tells the Cat5000 boxes out there to program the 
		 * bridge forwarding tables with the port the Report
		 * originator resides on.
		 */
		if (idb->cgmp_enabled && igmpidb->multicast_qr) {
		    cgmp_process_igmp_report(idb, igmpidb, pak, igmp, source, 
					     ip->dstadr);
		}
	    }
	    break;

	case IGMP_LEAVE_TYPE:
	    if (idb->igmp_enabled) {
		igmp_traffic.input_leaves++;
		
		/*
		 * Accept leave messages only if we are acting as a higher
		 * version router.
		 */
		if ((idb->pim_enabled) && (igmpidb->version > IGMP_VERSION1)) {
		    igmp_receive_leave(igmp, source, ip->dstadr, idb);
		}
		if (idb->cgmp_enabled && igmpidb->multicast_qr &&
		    idb->cgmp_process_leaves) {
		    cgmp_send_leave(igmp->address, idb, pak);
		}
	    }
	    break;

	case IGMP_PIM_TYPE:
	    igmp_traffic.input_pim++;

	    /*
	     * PIM gets control of pak if configured on interface.
	     */
	    if (idb->pim_enabled) {
		process_enqueue(pimQ, pak);
		continue;
	    }
	    break;
	    
	case IGMP_MTRACE_RESP:
	case IGMP_MTRACE_TYPE:

	    /*
	     * Check if packet came in over DVMRP tunnel. If so, reset
	     * pak->if_input to tunnel idb and do the accounting.
	     */
	    if (idb != samecable(ip->srcadr) || !idb->ip_mrouting) {
		idb = dvmrp_get_tunnel(ip);
		if (idb && interface_up(idb) && change_if_input(pak, idb)) {
		    dvmrp_tunnel_accounting(pak, idb);
		}
	    }
	    if (igmp->type == IGMP_MTRACE_RESP) {
		ip_mbone_mtrace_receive_response(igmp, pak, length);
		continue;
	    } else {
		if (!ip_mbone_mtrace_process_request(igmp, pak, length)) {
		    continue; 
		}
	    }
	    break;
	
	case IGMP_DVMRP_TYPE:
	    igmp_traffic.input_dvmrp++;
		
	    /*
	     * If unicast addressed DVMRP packet, get tunnel idb if any and
	     * do the accounting.
	     */
	    if (!IPMULTICAST(ip->dstadr)) {
		idb = dvmrp_get_tunnel(ip);
		if (idb) {
		    if (!interface_up(idb)) {
			DVMRP_BUGINF(
			    "\nDVMRP: Received packet on down tunnel %s",
				idb_get_namestring(idb));
			break;
		    } else if (change_if_input(pak, idb)) {
			dvmrp_tunnel_accounting(pak, idb);
		    }
		    igmpidb = igmp_getidb(idb);
		}
	    }
	    
	    /*
	     * Find what type of DVMRP packet.
	     */
	    switch (igmp->code) {
		
                /* 
		 * Intentionally fall through to DVMRP_REPORT_CODE from
		 * DVMRP_PROBE_CODE case.
		 */
	    case DVMRP_PROBE_CODE:
	    case DVMRP_REPORT_CODE:
		if (!idb) break;
		
		/*
		 * Set keepalive timers. Break out if Probe/Report source
		 * is denied.
		 */ 
		if (!dvmrp_receive_probe(idb, source, igmp)) break;
		
		/*
		 * If a Report came in on a DVMRP tunnel, actually process
		 * it and store the information. If we are doing DVMRP
		 * unicast routing, process the Report on any interface.
		 */
		if (igmp->code == DVMRP_REPORT_CODE) {
		    if (DVMRP_TUNNEL(idb) || idb->dvmrp_unicast) {
			dvmrp_receive_report(igmp, ip->srcadr, idb, 
					     length);
		    }
		    
		    /*
		     * Send CGMP proxy Join for DVMRP (non-cisco/non-CGMP)
		     * router.
		     */
		    if (idb->cgmp_proxy && igmpidb->multicast_qr) {
			cgmp_send_proxy(pak, igmp, source, idb);
		    }
		}
		break;

	    case DVMRP_PRUNE_CODE:
		if (!idb) break;
		dvmrp_receive_prune(pak, length, ip->srcadr, idb, igmp);
		break;

	    case DVMRP_GRAFT_CODE:
	    case DVMRP_GRAFT_ACK_CODE:
		if (!idb) break;
		dvmrp_receive_graft_or_ack(pak, length, ip->srcadr, idb, igmp);
		break;

	    case DVMRP_ASK_NBR_CODE:
	    case DVMRP_ASK_NBR2_CODE:
		dvmrp_receive_ask_nbr(ip->srcadr, ip->dstadr, 
				      pak->if_input, length,
				      (igmp->code == DVMRP_ASK_NBR2_CODE));
		break;

	    case DVMRP_NBR2_CODE:
		dvmrp_receive_nbr2(igmp, pak, length);
		continue;
		    
	    default:
		DVMRP_BUGINF("\nDVMRP: Unsupported code %d from %i (%s)",
			     igmp->code, source, 
			     idb_get_namestring(pak->if_input));
		break;
	    }
	    break;

	default:
	    IGMP_BUGINF(
		"\nIGMP: Unknown packet type %d, code %d, from %i (%s)", 
		igmp->type, igmp->code, source, idb_get_namestring(idb));
	}

	/*
	 * We're done with packet.
	 */
	datagram_done(pak);
    }
}

static void igmp_process_teardown (int signal, int dummy1, void *dummy2,
				   char *dummy3)
{
    paktype *pak;

    igmp_running = 0;
    reg_delete_ip_enqueue(IGMP_PROT);

    process_watch_timer(&igmp_periodic_timer, DISABLE);

    process_watch_queue(igmpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(igmpQ)))
	datagram_done(pak);
    delete_watched_queue(&igmpQ);
}

/*
 * igmp_process
 *
 * Process incoming IGMP packets or timer expiry events.
 */
static process igmp_process (void)
{
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, igmp_process_teardown);
    process_watch_queue(igmpQ, ENABLE, RECURRING);
    process_watch_timer(&igmp_periodic_timer, ENABLE);

    /*
     * Have IGMP register with IP only when process is started.
     */
    reg_add_ip_enqueue(IGMP_PROT, igmp_input, "igmp_input");
    TIMER_START(igmp_periodic_timer, IGMP_PERIODIC_INTVL);

    /*
     * Process spin-loop.
     */
    while (TRUE) {
	ulong major, minor;

	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		igmp_process_do_packets();
		break;
	    case TIMER_EVENT:
		TIMER_START(igmp_periodic_timer, IGMP_PERIODIC_INTVL);
		igmp_process_timers();
		break;
	    }
	}
    }
}

/*
 * igmp_send_query
 *
 * Send IGMP host query on interface.
 */
void igmp_send_query (idbtype *idb, ipaddrtype group)
{

    paktype  	 *pak, *dup;
    igmptype 	 *igmp;
    int      	 size;
    igmpidbtype *igmpidb = igmp_getidb(idb);

    if (!igmpidb)
	return;
    
    /*
     * Start timer now before doing anything, if we are sending
     * general membership query.
     */
    if (group == ALLSYSTEMS)
	mgd_timer_start(&igmpidb->query_timer, igmpidb->query_interval);
    
    /*
     * If no IP address on interface, check next time.
     */
    if (!idb->ip_enabled) return;

    /*
     * If the interface is not up then don't try sending anything
     */
    if (!interface_up(idb)) return;
 
    /*
     * Don't send queries on point-to-point links if there is a PIM router
     * on the other side or if the interface is a DVMRP tunnel.
     */
    if (is_p2p(idb)) {
	if (DVMRP_TUNNEL(idb) || idb->pim_nbrs) return;
    }

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_QUERY_TYPE;

    /*
     * If we are acting as a version1 router, do not send
     * response time. 
     */
    igmp->code = (igmpidb->version == IGMP_VERSION1) ? 0:
                  igmpidb->query_resp_value;

    if (group == ALLSYSTEMS) {
	igmp->address = 0;
    } else {
	igmp->address = group;
	igmp->code = IGMP_LAST_MEMBER_QUERY_RESP_VALUE;
    }
    igmp->checksum = 0;
    igmp->checksum = ipcrc((ushort *) igmp, IGMPHEADERBYTES);

    IGMP_BUGINF("\nIGMP: Send v%d Query on %s to %i",
		igmpidb->version, idb_get_namestring(idb), group);

    /*
     * Make copy for us in case we have Reports to send.
     */
    dup = pak_duplicate(pak);

    /*
     * Send to IP.
     */
    ipwrite(pak, size, IGMP_PROT, 0, group, idb, NULL, &igmprouteparams);
    igmp_traffic.output_queries++;
    GET_TIMESTAMP(igmpidb->query_resp_timer);

    /*
     * If we successfully copied the packet, process our own Query to
     * report local groups that we are members of.
     */
    if (dup) {
	igmp = (igmptype *) (ipheadstart(dup) + IPHEADERBYTES(NOPT));
	igmp_receive_query(igmp, idb->ip_address, idb);
	datagram_done(dup);
    }
}

/*
 * igmp_send_report
 *
 * Send IGMP host report out interface.
 */
void igmp_send_report (igmp_cachetype *cache)
{

    paktype  *pak, *dup;
    igmptype *igmp;
    ulong    size;
    idbtype *idb = cache->idb;
    const igmpidbtype *igmpidb = igmp_getidb(idb);
    
    /*
     * If the interface is not up then don't try sending anything
     */
    if (!interface_up(idb) || !igmpidb) return;
 
    /*
     * Only send Reports if we are not doing multicast routing on this
     * point-to-point interface.
     */
    if (is_p2p(idb) && idb->pim_enabled) return;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));

    /*
     * Depending on our current version, send new or old report type.
     */
    igmp->type = (igmpidb->version == IGMP_VERSION1) ?
	             IGMP_REPORT_TYPE : IGMP_NEW_REPORT_TYPE;
    igmp->code = 0;
    igmp->address = cache->group;
    igmp->checksum = 0;
    igmp->checksum = ipcrc((ushort *) igmp, IGMPHEADERBYTES);

    IGMP_BUGINF("\nIGMP: Send v%d Report for %i on %s",
		igmpidb->version,
		cache->group,
		idb_get_namestring(idb));

    /*
     * If we are DR for interface, we may be only member on LAN for group.
     * Make us receive out own report so it goes in the multicast routing
     * table.
     */
    dup = NULL;
    if (idb->ip_mrouting && (cache->flags & IGMP_LOCAL_FLAG)) {
	dup = pak_duplicate(pak);
    }

    /*
     * Send to IP.
     */
    ipwrite(pak, size, IGMP_PROT, 0, cache->group, idb, NULL, 
	    &igmprouteparams);
    igmp_traffic.output_reports++;

    /*
     * Cancel report delay timer. Set last reporter to ourself.
     */
    TIMER_STOP(cache->report_delay_time);
    cache->last_reporter = idb->ip_address;

    /*
     * Pass our Report to receive processing.
     */
    if (dup) {
	igmp = (igmptype *) (ipheadstart(dup) + IPHEADERBYTES(NOPT));
	igmp_receive_report(igmp, idb->ip_address, igmp->address, idb);

	/*
	 * Send a CGMP join to tell switches this router is a member of
	 * a group.
	 */
	 if (idb->cgmp_enabled && igmpidb->multicast_qr) {
	     cgmp_send_self(CGMP_JOIN_TYPE, idb, igmp->address);
	 }
	datagram_done(dup);
    }
}

/*
 * igmp_receive_query
 *
 * Process incoming IGMP host query. 
 */
void igmp_receive_query (igmptype *igmp, ipaddrtype source, idbtype *idb)
{

    ipaddtype      *ptr;
    igmp_cachetype *cache;
    igmpidbtype    *igmpidb;

    /*
     * If the interface is not up then don't try receiving anything.
     * This is for tunnels.
     */
    if (!interface_up(idb)) return;
 
    if (!ip_ifaddress(idb, source)) {
	IGMP_BUGINF("\nIGMP: Received v%d Query from %i (%s)",
		    ((igmp->code) ? 2 : 1), source, idb_get_namestring(idb));
	igmpidb = igmp_getidb(idb);
	if ((igmp->code && igmpidb->version == IGMP_VERSION1) ||
	    (!igmp->code && igmpidb->version == IGMP_VERSION2)) {
	    igmpidb->wrong_versions++;
	}
	igmp_elect_qr(igmp, source, idb);
    }

    /*
     * Check if it is a group specific query.
     */
    if (igmp->address) {

	/*
	 * Find cache element if any, and report it in the usual
	 * fashion. Note that we process group specific query
	 * even if we are acting as a version1 router.
	 */
	cache = igmp_find_group(igmp->address, idb);
	if (!cache) return;

	for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead; ptr;
	     ptr = ptr->next) {
	    /*
	     * Set random backoff report timer if not already running.
	     */
	    if (ptr->address == igmp->address) {
		igmp_report_delay(cache, idb, igmp->code);
		break;
	    }
	}
	return;
    }

    
    /*
     * Set report delay timer for each group the router is a member of on
     * this interface. Don't consider local groups (224.0.0.0 - 224.0.0.255).
     */
    for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead; ptr; ptr = ptr->next) {
	if (IPMULTICAST_RSVD(ptr->address)) continue;

	/*
	 * If group not found in cache, something is inconsistent (bug).
	 */
	cache = igmp_find_group(ptr->address, idb);
	if (!cache) continue;

	/*
	 * Set random backoff report timer if not already running.
	 */
	igmp_report_delay(cache, idb, igmp->code);
    }
}

/*
 * igmp_receive_report
 *
 * Process incoming report. 
 */
void igmp_receive_report (const igmptype *igmp, ipaddrtype source,
			  ipaddrtype dest, idbtype *idb)
{

    igmp_cachetype *cache;
    ulong          expires;
    const igmpidbtype *igmpidb = igmp_getidb(idb);
    
    /*
     * If the interface is not up then don't try receiving anything.
     * This is for tunnels.
     */
    if (!interface_up(idb) || !igmpidb) return;

    IGMP_BUGINF("\nIGMP: Received v%d Report from %i (%s) for %i",
		((igmp->type == IGMP_NEW_REPORT_TYPE) ? 2 : 1),
		source, idb_get_namestring(idb), igmp->address);

    /*
     * If a router is advertising the DVMRP group address, use it to
     * kick start DVMRP interoperation. It turns out that probes are
     * seldom sent on a network where there exists multiple DVMRP
     * routers.
     */
    if (igmp->address == DVMRPROUTERS &&
	!TIMER_RUNNING(idb->dvmrp_nbr_timer)) {
	dvmrp_receive_probe(idb, source, 0);
	return;
    }

    /*
     * Validate that group advertised is not a reserved Class D address.
     */
    if (!IPMULTICAST(igmp->address) || IPMULTICAST_RSVD(igmp->address)) {
	IGMP_BUGINF("\nIGMP: Illegal group address %i", igmp->address);
	return;
    }

    /*
     * Validate that advertised group address is the same as the address used 
     * for the report packet.
     */
    if (igmp->address != dest) {
	IGMP_BUGINF
	    ("\nIGMP: Report for group %i, sent to address %i, discarded",
	     igmp->address, dest);
	return;
    }

    /*
     * Test access-list to see if group should be accepted.
     */
    if (igmpidb->access_group) {
	if (!nfast_check(igmpidb->access_group, igmp->address)) {
	    IGMP_BUGINF("\nIGMP: Group access denied");
	    return;
	}
    }

    /*
     * Test administratively scoped address boundary.
     * Reject report if on boundary.
     */
    if (ipmulticast_test_boundary(idb, igmp->address)) {
        IGMP_BUGINF("\nIGMP: Discard report at boundary (%s) for %i",
		     idb_get_namestring(idb), igmp->address);
	return;
    }

    /*
     * Don't accept IGMP reports from PIM neighbors colocated with DVMRP
     * routers. PIM speaking routers advertise groups via IGMP. It doesn't
     * mean the PIM routers are members of those groups.
     */
    if (TIMER_RUNNING(idb->dvmrp_nbr_timer)) {
	if (pim_find_nbr(source, idb)) {
	    IGMP_BUGINF("\nIGMP: Ignoring Report from PIM neighbor");
	    return;
	}
    }

    /*
     * If group already in cache, just update expiration time. Otherwise,
     * add new entry to cache. igmp_add_group() will inform necessary
     * multicast routing protocol.
     */
    expires = igmpidb->query_interval * IGMP_GROUP_MEM_MULT;
    cache = igmp_add_group(igmp->address, idb, expires, 0, source);

    /*
     * Cancel report delay time if we had set the report delay to report
     * the group. Another system's report delay timer was shorter.
     */
    if (cache) {
	/*
	 * If we are waiting for group membership reports after
	 * the reception of a leave message,
	 * prevent further last_member_queries for the same group.
	 */
	if (cache->last_member_query_count)
	    cache->last_member_query_count = 0;

	if (TIMER_RUNNING(cache->report_delay_time)) {
	    IGMP_BUGINF("\nIGMP: Cancel report for %i on %s",
			cache->group, idb_get_namestring(cache->idb));
	}
	TIMER_STOP(cache->report_delay_time);
	cache->last_reporter = source;
    

	/*
	 * If we received an old membership report, start ignoring leave 
	 * messages for that group and start the old host timer.
	 */
	if (igmp->type != IGMP_NEW_REPORT_TYPE) {
	    if (igmpidb->version == IGMP_VERSION2) {
		IGMP_BUGINF("\nIGMP: Starting old host present timer for ");
		IGMP_BUGINF("%i on %s",
			    cache->group, idb_get_namestring(cache->idb));
		TIMER_STOP(cache->older_host_timer);
		TIMER_START(cache->older_host_timer, 
			    igmpidb->query_interval * IGMP_GROUP_MEM_MULT);
	    }
	}
    }
}

/*
 * igmp_process_timers
 *
 * 1) Process IGMP host query timers.
 * 2) Check IGMP cache entries for time-out.
 * 3) Check report delay timer for announcing group membership.
 * 4) Check if DVMRP neighbors have timed out.
 * 5) Determine if DVMRP Reports should be sent.
 * 6) Check if query response interval has elasped to send CGMP batch joins.
 */
void igmp_process_timers (void)
{

    idbtype        *idb;
    igmp_cachetype *igmp, *next;
    int            i;
    list_element   *l_elt;
    igmpidbtype    *igmpidb;
    
    /*
     * Determine if DVMRP neighbors have timed out. Determine if IGMP host 
     * query should be sent.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	
	if (TIMER_RUNNING_AND_AWAKE(idb->dvmrp_nbr_timer)) {
	    dvmrp_disable_idb(idb);
	}

	/*
	 * Determine if idb->dvmrp_routes_rcvd should be cleared.
	 */	
	if (TIMER_RUNNING(idb->dvmrp_routes_rcvd_timer)) {
	    if (ELAPSED_TIME(idb->dvmrp_routes_rcvd_timer) >=
		DVMRP_CLEAR_ROUTES_RCVD) {
		dvmrp_route_complaint(idb);
		idb->dvmrp_routes_rcvd = 0;
		idb->dvmrp_proutes_rcvd = 0;
		GET_TIMESTAMP(idb->dvmrp_routes_rcvd_timer);
	    }
	}

	if (!idb->igmp_enabled) continue;
	igmpidb = igmp_getidb(idb);
	
	/*
	 * Check if any of the IGMP timers have expired.
	 */
	if (mgd_timer_expired(&igmpidb->master_timer)) {
	    igmp_do_mgd_timers(&igmpidb->master_timer, idb);
	}

  	/*
  	 * The case in which the IGMP querier changed and its timer
 	 * was stopped. Also, make sure we have the right IP address,
	 * in case it changed.
  	 */
 	if (igmpidb->multicast_qr) {
 
	    if (!mgd_timer_running(&igmpidb->query_timer)) {
	        igmp_send_query(idb, ALLSYSTEMS);
	    }
	    if (igmpidb->version > IGMP_VERSION1) {
	        igmpidb->querier_address = (is_p2p(idb)) ? 0 :idb->ip_address;
            }
	    if (TIMER_RUNNING(igmpidb->query_resp_timer) &&
		ELAPSED_TIME(igmpidb->query_resp_timer) > CGMP_BATCH_TIME) {
		cgmp_send_batch_join(idb, igmpidb);
	    }
  	}
    }

    /*
     * Check for IGMP cache entries have expired.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (igmp = igmp_cache[i]; igmp; igmp = next) {
	    next = igmp->next;
	    igmpidb = igmp_getidb(igmp->idb);
	    if (TIMER_RUNNING_AND_AWAKE(igmp->expire_time)) {
		IGMP_BUGINF("\nIGMP: Deleting %i on %s", igmp->group,
		idb_get_namestring(igmp->idb));

		/*
		 * If CGMP is enabled and we are querier, send a CGMP Leave
		 * message. This tells the Cat5000 boxes to delete their
		 * bridge forwarding entries for this group.
		 */
		if (igmp->idb->cgmp_enabled && igmpidb->multicast_qr) {
		    cgmp_send_leave(igmp->group, igmp->idb, NULL);
		    if (cgmp_sources_exist(igmp->group, igmp->idb)) {
			cgmp_send_self(CGMP_JOIN_TYPE, igmp->idb, igmp->group);
		    }
		}
		igmp_delete_group(igmp->group, igmp->idb);
		continue;
	    }

	    /*
	     * Check for the last member query retransmission.
	     */
	     if (igmpidb->multicast_qr) {
		 if (TIMER_RUNNING_AND_AWAKE(igmp->last_member_query_time))
		     igmp_process_last_member(igmp);
	     }
	    
            /*
             * If old host timer is awake, stop the timer so that 
             * we continue to process leave messages.
             */
	    if (TIMER_RUNNING_AND_AWAKE(igmp->older_host_timer)) {
		IGMP_BUGINF("\nIGMP: Cancelling old host present timer for ");
		IGMP_BUGINF("%i on %s",
			    igmp->group, idb_get_namestring(igmp->idb));
		TIMER_STOP(igmp->older_host_timer);
            }
	    
	    /*
	     * Check for report delay timers have expired. This occurs
	     * if the router has joined a multicast group on an interface.
	     * It is essentially acting as a host.
	     */
	    if (TIMER_RUNNING_AND_AWAKE(igmp->report_delay_time)) {
		igmp_send_report(igmp);
	    }
	}
    }

    /*
     * Determine if DVMRP Report should be sent.
     */
    if (TIMER_RUNNING_AND_AWAKE(dvmrp_report_timer)) {
	dvmrp_send_report(NULL, DVMRPROUTERS);
	TIMER_START(dvmrp_report_timer, DVMRP_REPORT_INTERVAL);
    }

    /*
     * Age DVMRP routes if any DVMRP tunnels are active.
     */
    if (DVMRP_ROUTES) {
	if (!dvmrp_skip_aging) {
	    dvmrp_age_routes();
	    dvmrp_skip_aging = DVMRP_AGING_RATE;
	} else {
	    dvmrp_skip_aging--;
	}
    }
}

/*
 * igmp_static_join_command
 *
 * Join/Leave a multicast group statically for an interface.
 */
static void igmp_static_join_command (ipaddrtype group, idbtype *idb, 
                                      boolean    sense) 
{
    igmp_cachetype *cache;

    /*
     * Check for group sanity. Wildcard groups are not supported now.
     */
    if (!IPMULTICAST(group) || IPMULTICAST_RSVD(group)) {
	printf("\nIllegal multicast group address");
	return;
    }
    
    if (sense) {

	/*
	 * Add static group.
	 */
	cache = igmp_add_group(group, idb, 0, IGMP_STATIC_FLAG, 0);
    } else {
	
	/*
	 * Delete static group.
	 */
	cache = igmp_find_group(group, idb);
	if ((cache->flags & IGMP_STATIC_FLAG) &&
	    !(cache->flags & IGMP_LOCAL_FLAG))
	    igmp_delete_group(group, idb);
    }
}

/*
 * Nv Gen static group commands.
 */
static void igmp_static_nvgen (idbtype *idb, parseinfo *csb)
{
    int i; 
    igmp_cachetype *igmp, *next;
		
    /*
     * Display all static group commands.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (igmp = igmp_cache[i]; igmp; igmp = next) {
	    next = igmp->next;
                        
	    if (idb && idb != igmp->idb) continue;
                        
	    /*
	     * Join group overrides static group.
	     */
	    if ((igmp->group) && 
		(igmp->flags & IGMP_STATIC_FLAG) &&
		!(igmp->flags & IGMP_LOCAL_FLAG)) {
		    nv_write(TRUE, "%s %i", csb->nv_command, 
			     igmp->group);
	    }
			
	}
    }
}

/*
 * igmp_commands
 *
 * Process IGMP command.
 */
void igmp_commands (parseinfo *csb)
{
    idbtype          *idb;
    ipaddtype        *ptr;
    dvmrp_metrictype *dvmrp;
    dvmrp_filter     *filter;
    igmp_cachetype   *igmp;
    int              i;
    igmpidbtype      *igmp_idb;
    acl_headertype   *new;
    
    idb = csb->interface;
    igmp_idb = igmp_getidb(idb);
    
    /*
     * NV-generation.
     */
    if (csb->nvgen) {
	switch (csb->which) {
	  case IGMP_JOIN_GROUP:
	    for (ptr = idb->ip_multiaddrQ.qhead; ptr; ptr = ptr->next) {
		if (IPMULTICAST_RSVD(ptr->address)) continue;
		igmp = igmp_find_group(ptr->address, idb);
		if (!igmp || !(igmp->flags & IGMP_LOCAL_FLAG)) continue;
                if (igmp->flags & IGMP_USER_FLAG) continue;
		nv_write(TRUE, "%s %i", csb->nv_command, ptr->address);
	    }
	    break;
	
	case IGMP_STATIC_GROUP:
	    igmp_static_nvgen(idb, csb);
	    break;
	    
	case IGMP_QUERY_INTVL:
	    if (!igmp_idb) break;
	    nv_write((igmp_idb->query_interval != IGMP_QUERY_INTERVAL),
		     "%s %d", csb->nv_command, 
		     igmp_idb->query_interval/ONESEC);
	    break;
	    
	case IGMP_ACCESS_GROUP:
	    if (!igmp_idb) break;
	    if (igmp_idb->access_group)
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 igmp_idb->access_group->name);
	    break;
	    
	case IGMP_DVMRP_METRIC:
	    for (i = 0; i < dvmrp_metric_count; i++) {
		dvmrp = &dvmrp_configured_metrics[i];
		if (dvmrp->idb != idb) continue;
		nv_write(TRUE, "%s %d", csb->nv_command, dvmrp->metric);
		if (dvmrp->acl)
		    nv_add(TRUE, " list %s", dvmrp->acl->name);
 		if (dvmrp->route_map) nv_add(TRUE, " route-map %s",
 					     dvmrp->route_map);
 		if (dvmrp->dvmrp) nv_add(TRUE, " dvmrp");
		if (dvmrp->pdb) {
		    nv_add(TRUE, " %s", dvmrp->pdb->name);
		    nv_add(dvmrp->pdb->asystem, " %d", dvmrp->pdb->asystem);
		}
	    }
	    break;
	    
	case IGMP_DVMRP_FILTER:
	    for (i = 0; i < dvmrp_filter_count; i++) {
		filter = &dvmrp_filters[i];
		if (filter->idb != idb) continue;
		nv_write(TRUE, "%s %s", csb->nv_command, filter->acl->name);
		if (filter->nbr_acl)
		    nv_add(TRUE, " neighbor-list %s", filter->nbr_acl->name);
		nv_add(filter->distance, " %d", filter->distance);
	    }
	    break;
	    
	case IGMP_DVMRP_DEFAULT:
	    if (idb->dvmrp_send_default) {
		nv_write(TRUE, "%s", csb->nv_command);
		if (idb->dvmrp_send_default == DVMRP_SEND_DEFAULT) {
		    nv_add(TRUE, " originate");
		} else {
		    nv_add(TRUE, " only");
		}
	    }
	    break;
	    
	case IGMP_DVMRP_UNICAST:
	    nv_write(idb->dvmrp_unicast, "%s", csb->nv_command);
	    break;
	    
	case IGMP_DVMRP_MOFFSET:
	    
 	    /*
 	     * NVGEN incoming metric offset.
 	     */
 	    if (idb->dvmrp_metric_offset_in !=
		DVMRP_DEFAULT_METRIC_OFFSET_IN) {
 		nv_write(TRUE, "%s in %d", csb->nv_command ,
 			 idb->dvmrp_metric_offset_in);
 	    }
	    
 	    /*
 	     * NVGEN outgoing metric offset.
 	     */
 	    if (idb->dvmrp_metric_offset_out !=
 		DVMRP_DEFAULT_METRIC_OFFSET_OUT) {
 		nv_write(TRUE, "%s out %d", csb->nv_command, 
 			 idb->dvmrp_metric_offset_out);
  	    }
	    break;
	    
 	case IGMP_DVMRP_REJECT_NON_PRUNERS:
 	    nv_write(idb->dvmrp_reject_non_pruners, csb->nv_command);
 	    break;
	    
	case IGMP_DVMRP_ROUTEHOG:
	    if (dvmrp_routehog_count != DVMRP_DEFAULT_COMPLAINT_COUNT) {
		nv_write(TRUE, "%s %d", csb->nv_command, dvmrp_routehog_count);
	    }
	    break;
	    
 	case IGMP_DVMRP_ROUTE_LIMIT:
  	    if (igmp_running) {
 		if (dvmrp_route_limit == -1) {
 		    nv_write(TRUE, "no %s", csb->nv_command);
 		} else {
 		    nv_write(TRUE, "%s %d", csb->nv_command, 
 			     dvmrp_route_limit);
 		}
  	    }
	    break;
	    
	case IGMP_VERSION:
	    if ((igmp_idb) && (igmp_idb->cfg_version == IGMP_VERSION1))
		nv_write(TRUE, "%s %d", csb->nv_command, igmp_idb->version);
	    break;
	    
	case IGMP_DVMRP_DISTANCE:
 	    nv_write(dvmrp_distance, "%s %d", csb->nv_command, dvmrp_distance);
 	    break;
 
	case IGMP_QUERY_RESPONSE:
	    if (igmp_idb) {
		if (igmp_idb->query_resp_value !=
		    IGMP_MAX_QUERY_RESPONSE_VALUE) {
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     igmp_idb->query_resp_value / 10);
		}
	    }
	    break;
	    
	case IGMP_QUERY_TIMEOUT:
	    if (igmp_idb) {
		if ((igmp_idb->querier_timeout) &&
		    (igmp_idb->querier_timeout != (igmp_idb->query_interval *
						   IGMP_QR_PRESENT_MULT))) {
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     igmp_idb->querier_timeout);
		}
	    }
	    break;
	    
	default:
	    break;
	}
	return;
    }

    /*
     * If the user issued an igmp subcommand,
     * create the igmp_block.
     */
    if (idb && !igmp_idb) {
	if (!(igmp_idb = igmp_create_idb_block(idb))) {
	    return;
	}
    }

    /*
     * Process command.
     */
    switch (csb->which) {
    case IGMP_JOIN_GROUP:
	igmp_join_group_command(GETOBJ(paddr,1)->ip_addr, idb, csb->sense);
	break;
  
    case IGMP_STATIC_GROUP:
	igmp_static_join_command(GETOBJ(paddr,1)->ip_addr, idb, csb->sense);
	break;
	
    case IGMP_QUERY_INTVL:
	igmp_idb->query_interval = (csb->sense) ? GETOBJ(int,1) * ONESEC : 
	IGMP_QUERY_INTERVAL;
	if (igmp_idb->multicast_qr) {
	    mgd_timer_start(&igmp_idb->query_timer, igmp_idb->query_interval);
	}
	break;
	
    case IGMP_ACCESS_GROUP:
	if (csb->sense) {
	    new = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1),
				   GETOBJ(int,1) == 0, ACL_IP_NAMED_SIMPLE);
	    if (!new)
		break;
	} else
	    new = NULL;
	igmp_idb->access_group = new;
	break;
	
    case IGMP_DVMRP_METRIC:
	dvmrp_metric_command(csb);
	break;
	
    case IGMP_DVMRP_FILTER:
	dvmrp_accept_filter_command(csb);
	break;
	
    case IGMP_DVMRP_DEFAULT:
	idb->dvmrp_send_default = (csb->sense) ? GETOBJ(int,1) : 0;
	break;
	
    case IGMP_DVMRP_UNICAST:
	if (csb->sense) {
	    dvmrp_unicast_routing++;
	    idb->dvmrp_unicast = TRUE;
	    if (!TIMER_RUNNING(dvmrp_report_timer)) {
		TIMER_START(dvmrp_report_timer, DVMRP_REPORT_INTERVAL);
	    }
	} else {
	    dvmrp_unicast_routing--;
	    idb->dvmrp_unicast = FALSE;
	}
	break;
	
    case IGMP_DVMRP_MOFFSET:
	if (GETOBJ(int, 2)) {	/* outgoing metric offset */
	    idb->dvmrp_metric_offset_out = (csb->sense) ? GETOBJ(int,1) : 
	    DVMRP_DEFAULT_METRIC_OFFSET_OUT;
	} else {
	    idb->dvmrp_metric_offset_in = (csb->sense) ? GETOBJ(int,1) : 
	    DVMRP_DEFAULT_METRIC_OFFSET_IN;
	}
	break;
	
    case IGMP_DVMRP_REJECT_NON_PRUNERS:
	idb->dvmrp_reject_non_pruners = csb->sense;
	break;

    case IGMP_DVMRP_ROUTEHOG:
	dvmrp_routehog_count = 
	    (csb->sense) ? GETOBJ(int,1) : DVMRP_DEFAULT_COMPLAINT_COUNT;
	break;

    case IGMP_DVMRP_ROUTE_LIMIT:
	dvmrp_route_limit = (csb->sense) ? GETOBJ(int,1) : -1;
	break;
	
    case IGMP_DVMRP_DISTANCE:
	dvmrp_distance = (csb->sense) ? GETOBJ(int,1) : 0;
	break;

    case IGMP_VERSION:
	if (csb->sense) {
	    igmp_idb->cfg_version = GETOBJ(int, 1);
	    igmp_switch_version(idb, igmp_idb->cfg_version);
	} else {
	    igmp_switch_version(idb, IGMP_VERSION2);
	}
	break;
	
    case IGMP_QUERY_RESPONSE:
	igmp_idb->query_resp_value = 
	    (csb->sense) ? GETOBJ(int,1) * 10 :
	IGMP_MAX_QUERY_RESPONSE_VALUE;
	break;
	
    case IGMP_QUERY_TIMEOUT:
	igmp_idb->querier_timeout = 
	    (csb->sense) ? GETOBJ(int,1) * ONESEC : 0;
	break;
	
    default:
	break;
    }
}


/*
 * igmp_join_group_command
 *
 * Join/Leave a multicast group for an interface. Not allowed to join to
 * group address between 224.0.0.0 and 224.0.0.255.
 */
void igmp_join_group_command (ipaddrtype group, idbtype *idb, boolean join)
{

    igmp_cachetype *cache;

    /*
     * Only accept multicast addresses.
     */
    if (!IPMULTICAST(group)) {
	printf("\nNot a IP multicast group address");
	return;
    }

    /*
     * Don't allow joining to local group addresses.
     */
    if (IPMULTICAST_RSVD(group)) {
	printf("\nIllegal multicast group address");
	return;
    }

    /*
     * Process join or leave. If joining, send report now.
     */
    if (join) {
	igmp_enable_idb(idb);
	cache = igmp_add_group(group, idb, 0, IGMP_LOCAL_FLAG, 0);
	if (cache) igmp_send_report(cache);
    } else {

	/*
	 * If a process joined this group, can't leave now.
	 */
	cache = igmp_find_group(group, idb);
        
	if (!cache)
	    return; 
	
	if (cache->flags & IGMP_USER_FLAG) {
	    printf("\nCannot leave a group a process has joined");
	    return;
	}

	/*
	 * Send leave message and then delete the cache.
	 */
	igmp_send_leave(group, idb);
	igmp_delete_group(group, idb);
    }
}

/*
 * igmp_add_group
 *
 * Adds a group to the IGMP cache. timer value is in milliseconds. 0 means
 * no expiration.
 */
igmp_cachetype *igmp_add_group (ipaddrtype group, idbtype *idb, 
				ulong expire_time, ulong flags, 
				ipaddrtype reporter)
{

    igmp_cachetype *igmp;
    int            hash;
    boolean        local, new_group;
    igmpidbtype    *igmpidb;

    new_group = FALSE;

    /*
     * If group exists, update expiration time and return cache entry.
     */
    igmp = igmp_find_group(group, idb);
    if (!igmp) {
	igmp = malloc(sizeof(igmp_cachetype));
	if (!igmp) return(NULL);

	igmpidb =igmp_getidb(idb);
	if (igmpidb) igmpidb->group_joins++;

	/*
	 * Store data and link into hash table.
	 */
	new_group = TRUE;
	igmp->group = group;
	igmp->idb = idb;
	TIMER_STOP(igmp->report_delay_time);
	GET_TIMESTAMP(igmp->uptime);
	hash = nethash(group);
	igmp->next = igmp_cache[hash];
	igmp_cache[hash] = igmp;
    }

    /*
     * Check if group is currently local.
     */
    local = (igmp->flags & IGMP_LOCAL_FLAG);

    /*
     * Turn on data link multicast address. This is the case where this
     * system has joined a group. Don't turn on if we are already in the
     * group.
     */
    if (!local && (flags & IGMP_LOCAL_FLAG)) {
	ip_multicast_ctl(idb, group, TRUE);
    }

    if (!local) {
	TIMER_STOP(igmp->expire_time);
	if (expire_time) TIMER_START(igmp->expire_time, expire_time);
    }
    igmp->flags |= flags;

    /*
     * Add interface to all (Si,G) mroutes if we are a multicast router.
     */
    if (ip_mrouter_running) {
	igmp_add_mroute_idb(group, idb, local, new_group, reporter);
    }

    /*
     * Send unsolicited report if the group has USER_FLAG set.
     */
    if (flags & IGMP_USER_FLAG) {
	igmp_send_report(igmp);
    }
    
    return(igmp);
}

/*
 * igmp_delete_group
 *
 * Deletes a group from the IGMP cache.
 */
void igmp_delete_group (ipaddrtype group, idbtype *idb)
{

    igmp_cachetype *igmp, *prev;
    ipaddtype      *ptr;
    int            hash;
    igmpidbtype    *igmpidb;

    igmp = igmp_find_group(group, idb);
    if (!igmp) return;

    igmpidb = igmp_getidb(idb);
    if (igmpidb) igmpidb->group_leaves++;

    hash = nethash(group);
    prev = NULL;
    for (igmp = igmp_cache[hash]; igmp; igmp = igmp->next) {
	if (igmp->group == group && igmp->idb == idb) break;
	prev = igmp;
    }

    /*
     * Delink entry from igmp_cache and free memory.
     */
    if (prev) {
	prev->next = igmp->next;
    } else {
	igmp_cache[hash] = igmp->next;
    }

    /*
     * Turn off data link multicast address.
     */
    if (igmp->flags & IGMP_LOCAL_FLAG) {
	ip_multicast_ctl(idb, group, FALSE);
    }
    igmp->next = NULL;
    free(igmp);

    /*
     * Delete interface from multicast routing table.
     */
    igmp_delete_mroute_idb(group, idb);

    /*
     * Don't disable IGMP on interface if we are multicast routing on it.
     */
    if (idb->ip_mrouting) return;
    
    /*
     * Disable IGMP on interface if there are no group members. Check non-local
     * addresses only.
     */
    for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead; ptr; ptr = ptr->next) {
	if (!IPMULTICAST_RSVD(ptr->address)) break;
    }
    if (!ptr) igmp_disable_idb(idb);
}

/*
 * igmp_find_group
 *
 * Get a group/idb pair from the IGMP cache.
 */
igmp_cachetype *igmp_find_group (ipaddrtype group, idbtype *idb)
{

    igmp_cachetype *igmp;

    for (igmp = igmp_cache[nethash(group)]; igmp; igmp = igmp->next) {
	if (igmp->group == group && igmp->idb == idb) break;
    }
    return(igmp);
}

/*
 * igmp_enable_idb
 *
 * Enable IGMP on interface. If already enabled, quietly return.
 */
void igmp_enable_idb (idbtype *idb)
{
    igmpidbtype *igmpidb;
    
    if (idb->igmp_enabled) return;

    /*
     * Create igmp sub-block if not allocated already.
     * Bail if we cannot create igmp sub-block.
     */
    if (!(igmpidb = igmp_create_idb_block(idb))) return;
    idb->igmp_enabled = TRUE;
    ipmulticast_add_idb(idb);
    
    /*
     * Initialize managed timers
     */
    mgd_timer_init_parent(&igmpidb->master_timer, NULL);
    mgd_timer_init_leaf(&igmpidb->query_timer, &igmpidb->master_timer,
			IGMP_QUERY, NULL, FALSE);
    mgd_timer_init_leaf(&igmpidb->querier_present_timer,
			&igmpidb->master_timer,
			IGMP_OTHER_QUERIER, NULL, FALSE);
    mgd_timer_init_leaf(&igmpidb->older_router_timer,
			&igmpidb->master_timer,
			IGMP_OLDER_ROUTER, NULL, FALSE);
    mgd_timer_init_leaf(&igmpidb->cgmp_timer,
			&igmpidb->master_timer,
			CGMP_TIMER, NULL, FALSE);


    /*
     * Enable data link multicast reception.
     */
    ip_multicast_ctl(idb, ALLSYSTEMS, TRUE);
    ip_multicast_ctl(idb, ALLROUTERS, TRUE);

    /*
     * Assume that we are in host mode - don't send host queries.
     */
    igmpidb->multicast_qr = FALSE;
    igmpidb->querier_address = (is_p2p(idb)) ? 0 :idb->ip_address;

    if (igmp_running) return;

    /*
     * Start IGMP process if not started.
     */
    igmpQ = create_watched_queue("IGMP Input Queue", 0, 0);
    if (!igmpQ)
	return;
    
    igmp_running = process_create(igmp_process, "IGMP Input", NORMAL_STACK,
				  PRIO_NORMAL);
    if (igmp_running == NO_PROCESS)
	igmp_running = 0;
}

/*
 * igmp_disable_idb
 *
 * Disable IGMP on interface. Delete process if no more groups active.
 * Do not disable IGMP if any routing protocol is enabled on interface.
 */
void igmp_disable_idb (idbtype *idb)
{
    igmpidbtype *igmpidb;
    ipaddtype *ptr;
    list_element *l_elt;
    
    if (!igmp_running) return;
    if (!idb->igmp_enabled) return;

    /*
     * PIM is running, keep IGMP running. If CGMP is running, keep IGMP
     * running.
     */
    if (idb->pim_enabled) return;
    if (idb->cgmp_enabled) return;
    igmpidb = igmp_getidb(idb);
    igmpidb->multicast_qr = FALSE;

    /*
     * Don't disable IGMP if there are groups the router has joined.
     */
    for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead; ptr; ptr = ptr->next) {
	if (!IPMULTICAST_RSVD(ptr->address)) return;
    }
    
    /*
     * Disable IGMP on interface. Disable DVMRP neigbhor state.
     */
    idb->igmp_enabled = FALSE;
    ipmulticast_remove_idb(idb);
    mgd_timer_stop(&igmpidb->master_timer);
    free(igmpidb);
    idb_delete_swsb(idb, SWIDB_SB_IGMP);
    
    if (DVMRP_ACTIVE(idb)) dvmrp_disable_idb(idb);

    /*
     * Disable data link multicast reception.
     */
    ip_multicast_ctl(idb, ALLSYSTEMS, FALSE);
    ip_multicast_ctl(idb, ALLROUTERS, FALSE);

    /*
     * Clear all groups on interface from IGMP cache.
     */
    igmp_clear_cache(0, idb);

    /*
     * If there is no IGMP on any interfaces, kill the process.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (idb->igmp_enabled) return;
    }

    process_kill(igmp_running);
}

/*
 * igmp_show_commands
 *
 * Process "show ip igmp ..." commands.
 */
void igmp_show_commands (parseinfo *csb)
{

    switch (csb->which) {
    case IGMP_SHOW_GROUPS:
	igmp_show_groups(csb);
	break;

    case IGMP_SHOW_INTERFACE:
	igmp_show_interface(csb);
	break;

    default:
	break;
    }
}    
static const uchar igmp_show_groups_hdr[] = {
"IGMP Connected Group Membership
Group Address    Interface            Uptime    Expires   Last Reporter"};

/*
 * igmp_show_groups
 *
 * Process "show ip igmp groups" command. Command syntax is:
 *
 * show ip igmp groups [<group-address>]
 */
void igmp_show_groups (parseinfo *csb)
{

    igmp_cachetype *igmp, *next;
    idbtype        *idb;
    int            i;
    uchar          uptime[20];
    uchar          expires[20];
    ipaddrtype     group;

    group = GETOBJ(paddr,1)->ip_addr;
    idb = GETOBJ(idb,1);

    automore_enable(igmp_show_groups_hdr);
    for (i = 0; i < NETHASHLEN; i++) {
	for (igmp = igmp_cache[i]; igmp; igmp = next) {
	    next = igmp->next;

	    /*
	     * Selection criteria.
	     */
	    if (group && group != igmp->group) continue;
	    if (idb && idb != igmp->idb) continue;
            if (!interface_up(igmp->idb)) continue;

	    /*
	     * Calculate uptime.
	     */
	    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(igmp->uptime));
            if (!TIMER_RUNNING_AND_AWAKE(igmp->expire_time)) {
               if (TIMER_RUNNING(igmp->expire_time)) {
                   sprint_ticks_in_dhms(expires,
                                        TIME_LEFT_SLEEPING(igmp->expire_time));
               } else {
                   sprintf(expires, "never");
               }
            } else {
                sprintf(expires, "now");
            }

	    mem_lock(igmp);
	    printf("\n%17i%21s%10s%10s%i", igmp->group, 
		   idb_get_namestring(igmp->idb),
		   uptime, expires, igmp->last_reporter);
	    free(igmp);
	}
    }
    automore_disable();
}

/*
 * igmp_show_interface
 */
void igmp_show_interface (parseinfo *csb)
{
    automore_enable(NULL);
    show_swidb_chain(GETOBJ(idb,1), igmp_display_idb, 0);
    automore_disable();
}

/*
 * igmp_display_idb
 *
 * Process a IGMP related display for an idb.
 */
void igmp_display_idb (idbtype *idb)
{

    ipaddtype    *ptr;
    int          i;
    boolean      banner;
    char         buf[80];
    const igmpidbtype  *igmpidb = igmp_getidb(idb);
    
    printf(if_is_line_proto_is_msg, idb_get_namestring(idb),
          print_idbstate(idb), interface_up(idb) ? "up" : "down");
    if (!idb->ip_enabled) {
	printf("\n  Internet protocol processing disabled");
	return;
    }

    if (idb->ip_address)
	printf("\n  Internet address is %i, subnet mask is %i",
	  idb->ip_address, idb->ip_nets_mask);
    if (idb->ip_unnumbered)
	printf("\n  Interface is unnumbered.  Using address of %s (%i)",
		idb_get_namestring(idb->ip_unnumbered),
		idb->ip_unnumbered->ip_address);

    /*
     * Print IGMP specific data.
     */
    printf("\n  IGMP is %sabled on interface", 
	   (idb->igmp_enabled) ? "en" : "dis");
    if (igmpidb && (idb->igmp_enabled)) {
	printf("\n  Current IGMP version is %d", igmpidb->version);
	printf("\n  CGMP%sis %sabled on interface",  (idb->cgmp_proxy) ? 
	       " proxy " : " ", (idb->cgmp_enabled) ? "en" : "dis");
	printf("\n  IGMP query interval is %d seconds", 
	       igmpidb->query_interval/ONESEC);
	if (igmpidb->version > IGMP_VERSION1) {
	    printf("\n  IGMP querier timeout is %d seconds",
		   ((igmpidb->querier_timeout) ?
		    igmpidb->querier_timeout/ONESEC :
		    (igmpidb->query_interval * IGMP_QR_PRESENT_MULT)/ONESEC));
	    printf("\n  IGMP max query response time is %d seconds",
		   (igmpidb->query_resp_value / 10));
	}
	printf("\n  Inbound IGMP access group is ");
	access_print_name(igmpidb->access_group);
    }
    printf("\n  Multicast routing is %sabled on interface", 
	   (idb->ip_mrouting) ? "en" : "dis");
    printf("\n  Multicast TTL threshold is %d", idb->ip_multicast_threshold);
    if (idb->ip_mrouting && !is_p2p(idb)) {
	printf("\n  Multicast designated router (DR) is %i %s", idb->pim_dr,
	       (idb->pim_dr == idb->ip_address) ? "(this system)" : " ");
	if (igmpidb)
	    if (igmpidb->version > IGMP_VERSION1) {
		printf("\n  IGMP querying router is %i %s",
		       igmpidb->querier_address,
		       (igmpidb->multicast_qr) ? "(this system)" : " ");
	    } else {
		printf("\n  IGMP querying router is %i %s",
		       idb->pim_dr,
		       (igmpidb->multicast_qr) ? "(this system)" : " ");
	    }
	
    }

    /*
     * Print DVMRP specific data.
     */
    if (idb->dvmrp_unicast) printf("\n  DVMRP unicast routing is enabled");
    if (TIMER_RUNNING(idb->dvmrp_nbr_timer)) {
	sprint_ticks_in_dhms(buf, ELAPSED_TIME(idb->dvmrp_nbr_uptime));
	printf("\n  DVMRP/mrouted neighbors present for %s", buf);
	printf("\n    ");
	dvmrp_print_version(idb->dvmrp_nbr_version, TRUE);
	sprint_ticks_in_dhms(buf, ELAPSED_TIME(idb->dvmrp_routes_rcvd_timer));
    }
    
    if (idb->dvmrp_unicast || TIMER_RUNNING(idb->dvmrp_nbr_timer)) {
	
	printf(
        "\n  %d DVMRP routes + %d poison-reverse routes received in last %s", 
	       idb->dvmrp_routes_rcvd, idb->dvmrp_proutes_rcvd, buf);
	if (idb->dvmrp_routes_rcvd >= dvmrp_routehog_count) {
	    printf(", *** ALERT ***");
	}
	printf("\n  %d/%d Unicast/DVMRP routes last advertised by DVMRP",
	       idb->dvmrp_unicast_out_count, idb->dvmrp_dvmrp_out_count);
    }

    if (idb->dvmrp_send_default) {
	printf("\n  DVMRP originating default route%son interface",
	       (idb->dvmrp_send_default == DVMRP_DEFAULT_ONLY) ? 
	       " only " : " ");
    }

    /*
     * Display groups joined.
     */
    banner = FALSE;
    i = 2;
    for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead; ptr; ptr = ptr->next) {
	if (IPMULTICAST_RSVD(ptr->address)) continue;
	if (!banner) {
	    banner = TRUE;
	    printf("\n  Multicast groups joined:");
	}
	printf("%s%i", i++ % 6 ? " " : "\n      ", ptr->address);
    }
    if (!banner) printf("\n  No multicast groups joined");
}

/*
 * igmp_show_traffic
 *
 * Print IGMP statistics
 */
void igmp_show_traffic (void)
{

    printf("\n\nIGMP statistics: Sent/Received");
    printf("\n  Total: %u/%u, Format errors: 0/%u, Checksum errors: 0/%u",
	   igmp_traffic.outputs, igmp_traffic.inputs,
	   igmp_traffic.formaterr, igmp_traffic.checksumerr);
    printf
    ("\n  Host Queries: %u/%u, Host Reports: %u/%u, Host Leaves: %u%u \n  DVMRP: %u/%u, PIM: %u/%u",
     igmp_traffic.output_queries, igmp_traffic.input_queries,
     igmp_traffic.output_reports, igmp_traffic.input_reports,
     igmp_traffic.output_leaves, igmp_traffic.input_leaves,
     igmp_traffic.output_dvmrp, igmp_traffic.input_dvmrp,
     igmp_traffic.output_pim, igmp_traffic.input_pim);
}

/*
 * igmp_add_mroute_idb
 *
 * A group member has been learned via IGMP. Add the interface to the
 * multicast routing entry (*,G). Go through each (Si,G) to add interface
 * to outgoing interface list.
 */
void igmp_add_mroute_idb (ipaddrtype group, idbtype *idb, boolean local_flag, 
			  boolean new_group, ipaddrtype reporter)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    va_list    args[5];
    boolean    sparse, add_to_gdb, trigger_join;

    /*
     * Create/update (*,G) entry. Add interface to outgoing list.
     */
    mdb = ip_add_mroute(group, 0, MASK_HOST, NULL, 0, 0, MDB_EXPIRATION_TIME);
    if (!mdb) return;

    gdb = ip_get_gdb(group);
    if (!gdb) return;

    /*
     * Build outgoing interface list for (*,G). This list is made of interfaces
     * where there are PIM neighbors present. ip_add_mroute_idb() (called by
     * build_outputQ()) won't put this interface in the olist if it is in 
     * NBMA-mode. Reason being is that it doesn't have the IGMP reporter 
     * address. The add_to_gdb flag below instructs to add the NBMA-mode 
     * interface to the (*,G).
     */
    add_to_gdb = TRUE;
    trigger_join = FALSE;
    if (!mdb->if_outputQ.count) {
	ip_build_outputQ(NULL, gdb, mdb);
	mdb->flags |= CONNECTED_FLAG;
	if (local_flag) mdb->flags |= LOCAL_FLAG;
	add_to_gdb = idb->pim_nbma_mode;
	if (new_group) trigger_join = TRUE;
    }

    /*
     * Add interface to (*,G) entry.
     */
    if (add_to_gdb) {
	args[0] = (va_list) idb;
	args[1] = (va_list) gdb;
	args[2] = (va_list) local_flag;
	args[3] = (va_list) new_group;
	args[4] = (va_list) reporter;
	igmp_add_one_mroute_idb((rntype *) &gdb->mdb, (va_list) args);
    }
    
    /*
     * Look up to see if the group is supposed
     * to be treated in sparse-mode. If we find an RP for this group,
     * we will be in sparse-mode. Otherwise, the group is in dense-mode.
     */
    sparse = (pim_find_rp(gdb->mdb.group, 0) ? TRUE : FALSE);
    if (sparse) {

	/*
	 * If we are the DR on the LAN, get all RP mappings.
	 */
	if (idb->ip_multicast_dr || is_p2p(idb)) {
	    pim_get_all_rps(gdb, FALSE, FALSE);
	}
	if (!(gdb->mdb.flags & SPARSE_FLAG)) {
	    gdb->mdb.flags |= SPARSE_FLAG;
	    pim_delete_sparse_dense_oif(gdb);
	}
    }
    
    /* 
     * Send graft message for dense mode group if this group is new.
     * For Graft messages, The (*,G) entry has the retransmission timer. 
     * All (S,G) entries have boolean value to indicate if one should be sent 
     * for it.
     */
    if (!sparse && new_group && !gdb->rp.address) {
	GET_TIMESTAMP(gdb->send_graft_time);
	gdb->mdb.flags &= ~SPARSE_FLAG;
    }
   
    /*
     * Process each (Si,G).
     */
    rn_walktree(gdb->radix_head->rnh_treetop, igmp_add_one_mroute_idb, idb, 
		gdb, local_flag, new_group, reporter);

    /*
     * If this was the first olist entry to be added for the group, trigger
     * a Join for (*,G) and all (S,G) in a batch Join message.
     */
    if (trigger_join && IS_SPARSE_GROUP(gdb)) {
	pim_send_join_prune(&gdb->mdb, gdb->mdb.if_input, gdb->mdb.rpf_nbr,
			    TRUE, TRUE, TRUE, TRUE);
	pim_send_batch_join(gdb);
    }
}

/*
 * igmp_add_one_mroute_idb
 *
 * Add interface to outgoing interface list for a given multicast route entry.
 * Set an expiration timer for the entry.
 */
int igmp_add_one_mroute_idb (rntype *node, va_list args)
{

    mdbtype  *mdb;
    gdbtype  *gdb;
    idbtype  *idb;
    midbtype *midb;
    boolean  local_flag, new_group;
    ulong    state;
    ipaddrtype reporter;
    
    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);
    gdb = va_arg(args, gdbtype *);
    local_flag = va_arg(args, boolean);
    new_group = va_arg(args, boolean);
    reporter = va_arg(args, ipaddrtype);

    /*
     * Indicate that this group has directly connected members.
     */
    mdb->flags |= CONNECTED_FLAG;

    /*
     * Indicate if this system is a member of the group. Make sure fastswitch
     * flag is FALSE so process level switching can make a copy for IP
     * client.
     */
    if (local_flag) {
	mdb->fastswitch = FALSE;
	mdb->flags |= LOCAL_FLAG;
    }

    /*
     * If (S,G),RP-bit entry, we should now be on shortest path tree. Switch 
     * RPF parameters to point towards source.
     */
    if (mdb->flags & RP_BIT_FLAG) {
	mdb->flags &= ~RP_BIT_FLAG;
	ip_rpf_check_mroute(gdb, mdb);
    }
    
    /*
     * Set flag to send Graft message for this (S,G) entry. If source
     * is on this interface, we are at top of tree. Don't send graft
     * because no one will acknowledge.
     */
    if (new_group && samecable(ip_radix2addr(mdb->source)) != idb) {
	mdb->send_graft = TRUE;
    }

    /*
     * If this is a sparse-mode interface, we only have the DR put the
     * interface in its olist. This is so we can optimize to avoid 
     * periodic Assert behavior and therefore avoid short duplicate bursts 
     * every 3 minutes.
     */
    if (!IS_DENSE_MODE(idb, gdb) && !idb->ip_multicast_dr) return(0);

    /*
     * Don't update expiration timer, if in prune state. If interface does
     * not exist, add interface to outgoing list. Otherwise, just update 
     * expiration timer.
     */
    midb = ip_get_midb(&mdb->if_outputQ, idb, 0, FALSE);
    state = (midb) ? midb->state : MIDB_NULL_STATE;
    if (!midb || state == MIDB_FORWARD_STATE) {
	ip_add_mroute_idb(gdb, mdb, idb, reporter, MDB_EXPIRATION_TIME, 
			  MIDB_FORWARD_STATE, TRUE);
    }

    /*
     * If this is a newly learned group, we are sending a Graft upstream
     * for all (S,G). If we are forwarder (determined by Assert election or
     * by creating an midb starting out as forwarder) on this interface and 
     * the interface is in Prune state, put in Forward state.
     */
    if (new_group && state == MIDB_PRUNE_STATE && midb->forwarder) {
	ip_add_mroute_idb(gdb, mdb, idb, reporter, MDB_EXPIRATION_TIME, 
			  MIDB_FORWARD_STATE, TRUE);
    }
    return(0);
}

/*
 * igmp_delete_mroute_idb
 *
 * Delete interface from outgoing interface list of a (*,G) entry if and
 * only if we were a member of the group. The (Si,G) entries will time
 * out.
 */
void igmp_delete_mroute_idb (ipaddrtype group, idbtype *idb)
{

    gdbtype  *gdb;
    midbtype *midb;

    /*
     * Find group.
     */
    gdb = ip_get_gdb(group);
    if (!gdb) return;

    /*
     * Find outgoing interface in group. 
     */
    midb = ip_get_midb(&gdb->mdb.if_outputQ, idb, 0, FALSE);
    if (midb) {

	/*
	 * Don't delete interfaces if we have PIM neighbors or DVMRP
	 * neighbor(s) for a dense-mode group. For sparse-mode groups, only
	 * delete group from olist entries if there has been no PIM joiners for
	 * the group.
	 */
	if (DVMRP_ACTIVE(idb)) return;
	if (!IS_SPARSE_GROUP(gdb)) {
	    if (idb->pim_nbrs) return;
	} else if (idb->hwptr->type != IDBTYPE_LB) {
	    if (TIMER_RUNNING(midb->last_join) &&
		ELAPSED_TIME(midb->last_join) <= MDB_EXPIRATION_TIME) return;
	}
 
	/*	
	 * Delete idb from olist for (*,G) and all (Si,G). Don't trigger prune
	 * here. Wait for data packet to arrive, if ever. Then trigger when
	 * you have to.
	 */
	ip_delete_mroute_idb(&gdb->mdb, idb, gdb->mdb.group);
	rn_walktree(gdb->radix_head->rnh_treetop, igmp_delete_each_mroute_idb,
		    idb);
    } else {

	/*
	 * Fall through if members left the incoming interface for (*,G)
	 * and the group is flaged as connected. We want to trigger a
	 * Prune (perhaps from the DR that uses this LAN to send Joins).
	 */
	if (gdb->mdb.if_input != idb || !(gdb->mdb.flags & CONNECTED_FLAG)) {
	    return;
	}
    }

    /*
     * If group is in sparse-mode and there are no more outgoing interfaces,
     * send (*,G) prune up RP-tree (if we are on the RP-tree).
     */
    if (MDB_OUTPUTQ_NULL(&gdb->mdb) && IS_SPARSE_GROUP(gdb)) {
	if (gdb->mdb.if_input) {
	    pim_send_join_prune(&gdb->mdb, gdb->mdb.if_input, 
				gdb->mdb.rpf_nbr, FALSE, TRUE, TRUE, TRUE);
	}
    }
}

/*
 * igmp_delete_each_mroute_idb
 *	
 * Go through each (S,G) and delete outgoing interface list entry for
 * sparse-mode group.
 */
int igmp_delete_each_mroute_idb (rntype *node, va_list args)
{

    mdbtype *mdb;
    idbtype *idb;
    
    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);

    ip_delete_mroute_idb(mdb, idb, mdb->group);
    return(0);
}

/*
 * igmp_clear_cache
 *
 * Clear IGMP cache. If interface specified, only clear entries for
 * respective interface. If group specified, clear all entries associated
 * with group.
 */
void igmp_clear_cache (ipaddrtype group, const idbtype *idb)
{

    igmp_cachetype *igmp, *next;
    int            i;
    const igmpidbtype    *igmpidb;
    
    for (i = 0; i < NETHASHLEN; i++) {
	for (igmp = igmp_cache[i]; igmp; igmp = next) {
	    next = igmp->next;
	    if (idb && idb != igmp->idb) continue;
	    if (group && group != igmp->group) continue;
	    if (igmp->flags & IGMP_LOCAL_FLAG) continue;
	    if (igmp->flags & IGMP_STATIC_FLAG) continue;
	    igmpidb = igmp_getidb(igmp->idb);

	    /*
	     * If CGMP is enabled and we are querier, send a CGMP Leave
	     * message. This tells the Cat5000 boxes to delete their
	     * bridge forwarding entries for this group.
	     */
	    if (igmp->idb->cgmp_enabled && igmpidb->multicast_qr) {
		cgmp_send_leave(igmp->group, igmp->idb, NULL);
 		if (cgmp_sources_exist(igmp->group, igmp->idb)) {
 		    cgmp_send_self(CGMP_JOIN_TYPE, igmp->idb, igmp->group);
 		}
	    }
	    igmp_delete_group(igmp->group, igmp->idb);
	}
    }
}

/*
 * igmp_clear_group_command
 *
 * Process "clear ip igmp group [<group> | <interface>]".
 */
void igmp_clear_group_command (parseinfo *csb)
{
    
    idbtype    *idb;
    ipaddrtype group;

    group = GETOBJ(paddr,1)->ip_addr;
    idb = GETOBJ(idb,1);

    /*
     * Clear specific group on all interfaces or clear all groups on
     * a specific interface. Or a combination of all.
     */
    igmp_clear_cache(group, idb);
}


/*
 * The following functions are for incorporating version 2 support
 */
/*
 * igmp_send_leave
 *
 * Send a leave message for this group. 
 */
void igmp_send_leave (ipaddrtype group, idbtype *idb)
{
    igmp_cachetype *cache;
    paktype        *pak;
    igmptype       *igmp;
    ulong          size;

    cache = igmp_find_group(group, idb);
    if (!cache) return;

    /*
     * Send a leave message only if we are the last reporter.
     */
    if (!ip_ifaddress(idb, cache->last_reporter))
	return;
    
    /*
     * If the interface is not up then don't try sending anything
     */
    if (!interface_up(idb)) return;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_LEAVE_TYPE;
    igmp->code = 0;
    igmp->address = group;
    igmp->checksum = 0;
    igmp->checksum = ipcrc((ushort *) igmp, IGMPHEADERBYTES);

    IGMP_BUGINF("\nIGMP: Send Leave for %i on %s", group,
		idb_get_namestring(cache->idb));

    /*
     * Send to IP. We dont have to send the leave message to ourselves
     * since we already decided to leave. (unlike reports)
     */
    ipwrite(pak, size, IGMP_PROT, 0, ALLROUTERS, cache->idb, NULL, 
	    &igmprouteparams);
    igmp_traffic.output_leaves++;
}


/*
 * igmp_receive_leave
 *
 * Process incoming leave.
 * We allow Non-queriers also to receive the leave message so
 * that all routers can leave that group faster.
 */
static void igmp_receive_leave (const igmptype *igmp, ipaddrtype source,
				ipaddrtype dest, idbtype *idb)
{
    const igmpidbtype *igmpidb;
    igmp_cachetype *cache;
    ulong          expires;

    igmpidb = igmp_getidb(idb);

    /*
     * If the interface is not up then don't try receiving anything.
     * This is for tunnels.
     */
    if (!interface_up(idb) || !igmpidb) return;
    
    /*
     * If we get our own leave message, do not waste processing time.
     */
    if (!ip_ifaddress(idb, source)) {
	IGMP_BUGINF("\nIGMP: Received Leave from %i (%s) for %i",
		    source, idb_get_namestring(idb), igmp->address);
    } else {
	return;
    }

    /*
     * Test access-list to see if group should be accepted.
     */
    if (igmpidb->access_group) {
	if (!nfast_check(igmpidb->access_group, igmp->address)) {
	    IGMP_BUGINF("\nIGMP: Group access denied on %s", 
			idb_get_namestring(idb));
	    return;
	}
    }
    
    cache = igmp_find_group(igmp->address, idb);
    if (cache) {
    
	/*
	 * If we are a v2 router and we are in ignore leave
	 * messages mode, do so.
	 */ 
	if (!IGMP_LEAVE_ENABLED(cache)) {
	    IGMP_BUGINF("\nIGMP: Leave for group");
	    IGMP_BUGINF(" %i, sent to address %i on %s, discarded", 
			igmp->address, dest, idb_get_namestring(idb));
	    return;
	}      
	
	/*
	 * Change the expiry time in the cache, and
	 * send group source query if we are the querier router.
         * (2 times last_member_query_interval) + 1 sec.
	 */
	expires = (IGMP_LAST_MEMBER_QUERY_INTERVAL *
		   IGMP_LAST_MEMBER_QUERY_COUNT) + ONESEC;
	TIMER_START(cache->expire_time, expires);

	/*
	 * We should allow only the querier router to send
	 * group specific queries.
	 */
	if (igmpidb->multicast_qr) {
	    TIMER_START(cache->last_member_query_time,
			IGMP_LAST_MEMBER_QUERY_INTERVAL);
	    cache->last_member_query_count = IGMP_LAST_MEMBER_QUERY_COUNT - 1;
	    igmp_send_query(idb, igmp->address);
	}
    }
}


/*
 * igmp_switch_host_version
 *
 * We have expired the old router present timer.
 * This means we can switch back to version2.
 */
static void igmp_switch_host_version (const idbtype *idb)
{
    igmpidbtype *igmpidb = igmp_getidb(idb);

    IGMP_BUGINF("\nIGMP: Switching IGMP version back from %d to %d on %s",
		    igmpidb->version, IGMP_VERSION2, idb_get_namestring(idb));
    igmpidb->version = IGMP_VERSION2;
}


/*
 * igmp_switch_version
 *
 * For manual configuration changes of IGMP version only.
 * We dont try interoperability anymore.
 */
static void igmp_switch_version (idbtype *idb, int version)
{
    igmpidbtype *igmpidb = igmp_getidb(idb);
        
    if (igmpidb->version == version)
	return;

    if (version == IGMP_VERSION2) {
	IGMP_BUGINF("\nIGMP: Switching IGMP version from %d to %d on %s",
		    igmpidb->version, IGMP_VERSION2, idb_get_namestring(idb));
	igmpidb->version = IGMP_VERSION2;
	
	/*
	 * Let us be the querier, unless we hear later from a version2
	 * router with a lesser IP address.
	 */
	if (idb->pim_enabled) {
	    if (!igmpidb->multicast_qr) {
		IGMP_BUGINF("\nIGMP: The v2 querier for %s is this system",
			    idb_get_namestring(idb));
	    }
	    igmpidb->multicast_qr = TRUE;
	    igmpidb->querier_address = idb->ip_address; 
	    igmp_send_query(idb, ALLSYSTEMS);
	}
    } else { /* switch to version 1 */
	IGMP_BUGINF("\nIGMP: Switching IGMP version from %d to %d on %s",
		    igmpidb->version, IGMP_VERSION1, idb_get_namestring(idb));
	igmpidb->version = IGMP_VERSION1;
	
	/*
	 * If we have multicast routing enabled,
	 * we will start the old designated router/querier
	 * election algorithm. PIM code will check
	 * the IGMP version and make sure the querier
	 * is selected correctly.
	 */
	if (idb->pim_enabled) {
	    if (!igmpidb->multicast_qr)
		IGMP_BUGINF("\nIGMP: The v1 querier for %s is this system",
			    idb_get_namestring(idb));
	    
	    igmpidb->multicast_qr = TRUE;
	    igmpidb->querier_address = idb->ip_address;
	    igmp_send_query(idb, ALLSYSTEMS);
	}
    }
}



/*
 * igmp_elect_qr
 *
 * From Version 2 onwards, the querier router for the LAN
 * is decided independant of the multicast protocol. We also
 * provide support for backward compatibilty in case we have version 1
 * routers.
 */
static void igmp_elect_qr (const igmptype *igmp, ipaddrtype source,
			   idbtype *idb)
{
    ipaddrtype my_address;
    igmpidbtype *igmpidb = igmp_getidb(idb);
    
    /*
     * If it is a v1 query and we are a v2 host, we have to act as a v1 host.
     * However, v2 router remains v2; No more auto-switching of versions.
     */
    if (!igmp->code) {
	
	/*
	 * If we are acting as just a host, we can try to switch back
	 * to version2 if the version1 router goes away.
	 */
	if (!idb->pim_enabled) {
	    
	    IGMP_BUGINF("\nIGMP: Starting old router present timer");
	    mgd_timer_stop(&igmpidb->older_router_timer);
	    mgd_timer_start(&igmpidb->older_router_timer,
			    igmpidb->query_interval *
			    IGMP_OLD_ROUTER_MULT);

	    if (igmpidb->version > IGMP_VERSION1) {
		igmp_switch_version(idb, IGMP_VERSION1);
	    }
	}
	
	return;
    }

    /*
     * v2 query has been received; elect the querier.
     */
    if ((idb->pim_enabled) && (igmpidb->version > IGMP_VERSION1)) {
	
        /* 
         * If we are acting as a v2 router,
	 * if the query address is less than ours,
	 * we are not the querier for this LAN. 
	 */
	my_address = idb->ip_address;
	if (!my_address && idb->ip_unnumbered) {
	    my_address = idb->ip_unnumbered->ip_address;
	}

	/*
	 * Ignore query from unnumbered P2P interfaces.
	 */
	if ((is_p2p(idb)) && (source == 0)) return;
	
	/*
	 * Choose the router with the lowest
	 * IP address as the querier on the
	 * network.
	 */
	if (source < my_address) {
	    igmpidb->multicast_qr = FALSE;
	    igmpidb->querier_address = source;
	    mgd_timer_stop(&igmpidb->querier_present_timer);
	    mgd_timer_start(&igmpidb->querier_present_timer,
			    (igmpidb->querier_timeout) ?
			    igmpidb->querier_timeout :
			    (igmpidb->query_interval *
			     IGMP_QR_PRESENT_MULT));
	}
    }
    
    /*
     * We are not implementing startup query interval for now.
     * does not buy us anything.
     */
}


/*
 * igmp_switch_qr
 *
 * Become the IGMP querier, if the previous querier times out.
 */
static void igmp_switch_qr (const idbtype *idb)
{
    igmpidbtype *igmpidb = igmp_getidb(idb);
    
    /*
     * Let us be the querier, unless we hear later from a version2
     * router with a lesser IP address.
     */
    if ((igmpidb->version > IGMP_VERSION1) && (idb->pim_enabled)) {
	if (!igmpidb->multicast_qr) {
	    IGMP_BUGINF("\nIGMP: Previous querier timed out. ");
	    IGMP_BUGINF("The IGMP v2 querier for %s is this system",
			idb_get_namestring(idb));
	}
	igmpidb->multicast_qr = TRUE;
	igmpidb->querier_address = idb->ip_address;
    }
}


/*
 * igmp_do_mgd_timers
 * 
 * Find out which timer(s) expired and perform the appropriate
 * actions.
 */
static void igmp_do_mgd_timers (mgd_timer *master_timer, idbtype *idb)
{
    igmpidbtype *igmpidb;
    mgd_timer   *expired_timer;
    
    igmpidb = igmp_getidb(idb);

    while (mgd_timer_expired(master_timer)) {
	expired_timer = mgd_timer_first_expired(master_timer);
	mgd_timer_stop(expired_timer);

	switch (mgd_timer_type(expired_timer)) {
	case CGMP_TIMER:
	    cgmp_send_self(CGMP_JOIN_TYPE, idb, 0);

	    /*
	     * CGMP Join interval is same as IGMP query interval.
	     */
	    mgd_timer_start(&igmpidb->cgmp_timer, igmpidb->query_interval);
	    break;

	case IGMP_QUERY:
	    if (igmpidb->multicast_qr) {

		/*
		 * Restart timer inside the callee.
		 */
		igmp_send_query(idb, ALLSYSTEMS);
	    }
	    break;
	    
	case IGMP_OTHER_QUERIER:

	    /*
	     * Restarting timer is the reponsibility
	     * of the callee.
	     */
	    igmp_switch_qr(idb);
	    break;
	    
	case IGMP_OLDER_ROUTER:

	    /*
	     * If we are just a host, try switching back to version2.
	     */
	    IGMP_BUGINF("\nIGMP: Old router present timer expired on %s",
			idb_get_namestring(idb));
	    if (!idb->pim_enabled) {
		igmp_switch_host_version(idb);
	    }
	    break;

	default:
	}
    }
}


/*
 * igmp_process_last_member
 *
 * Make sure the group specific query is retransmitted
 * at least twice.
 */
static void igmp_process_last_member (igmp_cachetype *igmp)
{
    if (igmp->last_member_query_count) {
	TIMER_START(igmp->last_member_query_time,
			    IGMP_LAST_MEMBER_QUERY_INTERVAL);
	igmp->last_member_query_count--;
        if (IGMP_LEAVE_ENABLED(igmp)) 
	    igmp_send_query(igmp->idb, igmp->group);
    } else
	TIMER_STOP(igmp->last_member_query_time);
}


/*
 * igmp_report_delay
 *
 * Start the random back off report timer.
 */
static void igmp_report_delay (igmp_cachetype *cache,
			       const idbtype  *idb,
			       uchar          report_delay)
{
    ulong          delay, loop;

    /*
     * Generate a random # between 0 and the report delay, with the seed
     * based on the IP address. Use this value for the report
     * delay.
     */
    if (!TIMER_RUNNING(cache->report_delay_time)) {
	if (!report_delay) {
	    report_delay = IGMP_REPORT_DELAY_MAX * 10;
	}
	delay = idb->ip_address ^ random_gen();
	delay |= nethash(cache->group);
	delay = (delay % report_delay) + 1;
	TIMER_START(cache->report_delay_time, ((delay * ONESEC) / 10));
	IGMP_BUGINF("\nIGMP: Set report delay time to %d.%d seconds for %i on %s",
		    (delay / 10), (delay % 10), cache->group,
		    idb_get_namestring(cache->idb));
	loop = random_gen() & 0x0f;
  	while (loop) loop--;
    }
}

/*
 * igmp_create_idb_block
 *
 * Allocate igmp_idb, if not already allocated..
 */
static igmpidbtype *igmp_create_idb_block (idbtype *idb)
{
    igmpidbtype *igmpidb;
    swidb_sb_t sbtype;
    
    if (!(igmpidb = igmp_getidb(idb))) {

	/*
	 * Allocate igmp sub-block in idb.
	 */
	igmpidb = malloc(sizeof(igmpidbtype));
	if (!igmpidb)
	    return(NULL);

	/*
	 * Add subblock to idb.
	 */
	sbtype = SWIDB_SB_IGMP;
	idb_add_swsb(idb, &sbtype, igmpidb);
	
	/*
	 * The default is version 2 for igmp.
	 */
	igmpidb->version = IGMP_VERSION2;
	igmpidb->cfg_version = IGMP_VERSION2; 
	igmpidb->query_interval = IGMP_QUERY_INTERVAL;
	igmpidb->query_resp_value = IGMP_MAX_QUERY_RESPONSE_VALUE;
	igmpidb->querier_timeout = 0;
	idb->igmp_enabled = FALSE;
	igmpidb->multicast_qr = FALSE;
    }

    return(igmpidb);
}

/*
 * igmp_check_new_rp
 *
 * When a new RP is defined, this function is called to store the RP
 * into existing gdb entries. Groups joined on multi-access LANs already
 * undergo periodic check of current RP when the IGMP membership reports
 * are received periodicly.
 *
 * This makes sure that groups joined on point-2-point
 * interfaces also have the correct mroute state.
 *
 * If parameter rp_address is 0, it checks if there is a new RP for
 * the group.
 */
void igmp_check_new_rp (ipaddrtype rp_address, const idbtype *idb)
{

    igmp_cachetype *igmp;
    gdbtype *gdb;
    pim_rptype *rp;
    int i;
    ulong expire;

    for (i = 0; i < NETHASHLEN; i++) {
	for (igmp = igmp_cache[i]; igmp; igmp = igmp->next) {

	    /*
	     * Ignore if this is not the round to deal with this idb yet.
	     */
	    if (idb && idb != igmp->idb) continue;

	    /*
	     * Only care about this if IGMP is on a p2p link or a
	     * a DR on a LAN or locally joined.
	     */
	    if (!is_p2p(igmp->idb) && !igmp->idb->ip_multicast_dr &&
		!(igmp->flags & IGMP_LOCAL_FLAG)) {
		continue;
	    }

	    /*
	     * Check if this RP serves this group.
	     */
	    rp = pim_find_rp(igmp->group, rp_address);
	    if (!rp) continue;

	    /*
	     * Find the group entry
	     */
	    gdb = ip_get_gdb(igmp->group);
	    if (!gdb) {

		/*
		 * The following call adds the gdb if it disappeared due
		 * to various reasons, such as "clear ip mr <group>",
		 * or someone de-configured an RP, etc. It also
		 * populates the outgoing interface list,
		 * stores the RP for a DR interface.
		 */
		if (TIMER_RUNNING_AND_AWAKE(igmp->expire_time)) continue;
		expire = TIME_LEFT_SLEEPING(igmp->expire_time);
		igmp_add_mroute_idb(igmp->group, igmp->idb,
				    (igmp->flags & IGMP_LOCAL_FLAG), 0, 0);
		continue;
	    }

	    /*
	     * If this RP is not already stored in the gdb,
	     * store the RP into the group entry and trigger a join.
	     */
	    if (rp->rp_address != gdb->rp.address) {
		gdb->rp.address = 0;
		pim_store_rp_info(gdb, rp->rp_address, RP_UP, TRUE,
				  !rp->configured);
	    }
	}
    }
}
