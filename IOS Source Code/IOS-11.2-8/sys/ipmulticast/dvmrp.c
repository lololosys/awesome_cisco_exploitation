/* $Id: dvmrp.c,v 3.23.8.25 1996/09/11 16:54:33 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/dvmrp.c,v $
 *------------------------------------------------------------------
 * dvmrp.c - Distance Vector Multicast Routing Protocol (DVMRP).
 *
 * August, 1994.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Supports interoperability with mrouted implementations. This is not
 * a complete DVMRP implementation. It supports basic interoperability
 * between PIM clouds and DVMRP clouds. There is also support for DVMRP
 * tunnels.
 *------------------------------------------------------------------
 * $Log: dvmrp.c,v $
 * Revision 3.23.8.25  1996/09/11  16:54:33  dino
 * CSCdi68465:  RP information is too difficult to change in last-hop
 * routers
 * Branch: California_branch
 *
 * Revision 3.23.8.24  1996/08/28  12:51:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.23.8.23  1996/08/26  06:48:54  dino
 * CSCdi67098:  Do not accept DVMRP neighbor probes/reports from
 * non-pruners
 * Branch: California_branch
 *
 * Revision 3.23.8.22  1996/08/21  01:20:56  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.23.8.21  1996/08/14  05:03:35  lwei
 * CSCdi64491:  Check (*,G) OIF list when new PIM neighbor comes up
 * Branch: California_branch
 *
 * Revision 3.23.8.20  1996/08/09  03:33:43  mleelani
 * CSCdi65587:  (S, G) state gratuitously created on receiving prunes.
 * Branch: California_branch
 * Create (S, G) state only on receiveing (S, G, RP-bit) prunes.
 *
 * Revision 3.23.8.19  1996/08/06  08:27:44  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.23.8.18  1996/08/03  01:14:27  dino
 * CSCdi65013:  DVMRP default route is included twice in a single Report
 *              packet.
 * Branch: California_branch
 *
 * Revision 3.23.8.17  1996/07/19  15:01:47  snyder
 * CSCdi63480:  char * to char[], save a tiny bit
 *              36 bytes.
 * Branch: California_branch
 *
 * Revision 3.23.8.16  1996/06/28  07:48:17  dino
 * CSCdi61612:  Put in DVMRP route-limit, have it default to 7000, avoid
 *              MBONE meltdown.
 * Branch: California_branch
 *
 * Revision 3.23.8.15  1996/06/24  16:05:00  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.23.8.14  1996/06/21  18:43:36  tylin
 * CSCdi58913:  DVMRP control packets are not counted in the tunnel idb
 * stats
 * Branch: California_branch
 *
 * Revision 3.23.8.13  1996/06/20  21:39:34  chwhite
 * CSCdi57508:  DVMRP ROUTEHOG notification counter wasnt initialized
 * Branch: California_branch
 *
 * Revision 3.23.8.12  1996/06/18  01:46:31  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.23.8.11  1996/06/08  04:11:55  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.23.8.10  1996/06/07  15:08:57  chwhite
 * CSCdi51969:  Jhawk wants mrinfo source-interface option
 * Branch: California_branch
 *
 * Revision 3.23.8.9  1996/05/21  21:01:25  lwei
 * CSCdi58303:  DVMRP prunes with long timers dont keep mdb long enough
 * Branch: California_branch
 *
 * Revision 3.23.8.8  1996/05/14  00:12:23  mleelani
 * CSCdi56461:  European cisco MBONE needs ip dvmrp metric-offset out
 * command
 * Branch: California_branch
 * Support outgoing DVMRP metric offsets.
 *
 * Revision 3.23.8.7  1996/05/04  01:08:06  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.23.8.6  1996/04/20  01:50:58  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.23.8.5  1996/04/15  21:17:24  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.23.8.4  1996/04/04  20:45:15  dino
 * CSCdi51177:  Jhawk wants uptime and syslog of DVMRP tunnel transitions
 * Branch: California_branch
 *
 * Revision 3.23.8.3  1996/04/03  23:19:02  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.23.8.2  1996/04/03  21:52:53  dino
 * CSCdi51024:  PIM nbma-mode adds too many interfaces to olist
 * Branch: California_branch
 *
 * Revision 3.23.8.1  1996/03/18  20:18:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.19.2.4  1996/03/16  06:55:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.19.2.3  1996/03/13  01:38:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.19.2.2  1996/03/07  09:47:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.19.2.1  1996/02/20  01:01:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.23  1996/02/29  20:31:59  dino
 * CSCdi50389:  When natural mask not in routing table DVMRP prune not
 *              processed
 *
 * Revision 3.22  1996/02/23  18:41:56  mleelani
 * CSCdi46003:  DVMRP tunnel may not be added to group OIF list
 * Make sure DVMRP tunnel interfaces are deleted and added properly
 * to the mroute OIF list.
 *
 * Revision 3.21  1996/02/13  08:13:55  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.20  1996/02/01  06:04:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.19  1996/01/18  01:47:53  dino
 * CSCdi47088:  DVMRP does not report majornet or unnum if pointing to
 *              majornet
 *
 * Revision 3.18  1996/01/10  22:04:31  dino
 * CSCdi46682:  DVMRP accept-filters cant filter out default route.
 *
 * Revision 3.17  1996/01/04  22:11:25  dino
 * CSCdi46407:  ip dvmrp metric 0 list x does not advertise routes not
 *              matching x
 *
 * Revision 3.16  1995/12/30  21:16:08  dino
 * CSCdi46259:  mrouted 3.8 sets the S-bit in mrinfo responses, cisco
 *              should too.
 *
 * Revision 3.15  1995/12/13  07:03:13  dino
 * CSCdi45504:  (S,G) entries are getting DVMRP prune that dont match
 *              aggregate
 *
 * Revision 3.14  1995/12/07  03:28:59  dino
 * CSCdi45160:  DVMRP radix tree is not matching on default route
 *
 * Revision 3.13  1995/12/03  18:06:26  dino
 * CSCdi44874:  no ip dvmrp debug detail turns on debugging.
 *
 * Revision 3.12  1995/12/02  07:23:45  dino
 * CSCdi44876:  DVMRP does not propagate default route with routing table
 *              metric
 *
 * Revision 3.11  1995/11/30  04:42:00  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.10  1995/11/30  00:11:32  dino
 * CSCdi44668:  Mark DVMRP tunnel DOWN is it is not receiving probes from
 * the neighb
 *
 * Revision 3.9  1995/11/28  08:20:50  dino
 * CSCdi44518:  Code review commenting fixing DVMRP pruning.
 *
 * Revision 3.8  1995/11/28  07:29:11  dino
 * CSCdi44463:  DVMRP does not support non-contiguous masks, protect
 * against them
 *
 * Revision 3.7  1995/11/22  22:20:10  dino
 * CSCdi44336:  mtrace interface counters not properly counted when
 * partially fs
 *
 * Revision 3.6  1995/11/22  06:57:17  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.5  1995/11/21  23:16:46  dino
 * CSCdi43458:  ciscos mrinfo should display capability flags too.
 *
 * Revision 3.4  1995/11/21  23:11:57  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  17:29:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:00:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.22  1995/11/08  21:05:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.21  1995/10/13  20:37:27  dino
 * CSCdi42036:  RPF lookup is not matching on the DVMRP default route
 *
 * Revision 2.20  1995/10/11  18:52:19  jzwiebel
 * CSCdi41955:  ip dvmrp metric <metric> dvmrp puts all routes in the
 * dvmrp table
 *
 * Revision 2.19  1995/10/10  21:15:51  dino
 * CSCdi41878:  Monitor number of DVMRP routes received and errmsg()
 * during surges.
 *
 * Revision 2.18  1995/10/06  09:00:37  dino
 * CSCdi41679:  mtrace should return non-responding router information
 *
 * Revision 2.17  1995/10/04  03:09:28  mleelani
 * CSCdi41404:  When DVMRP_ACTIVE(idb), we should poison-reverse
 * We posion reverse now if we have an active DVMRP neighbor and
 * dvmrp unicast is enabled. We dont have to send IGMP group reports
 * in this case.
 *
 * Revision 2.16  1995/09/20  20:47:56  dino
 * CSCdi40555:  DVMRP may print an uninitialized variable
 *
 * Revision 2.15  1995/09/08  16:07:51  hampton
 * Clean up nits related to the timer changes.  [CSCdi39967]
 *
 * Revision 2.14  1995/08/22 21:54:18  dino
 * CSCdi39110:  DVMRP version number should be added to all packets, fix
 * style
 *
 * Revision 2.13  1995/08/15  21:22:06  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.12  1995/08/15  00:35:10  dino
 * CSCdi38727:  DVMRP advertises duplicates when supernets exist in
 *              routing table.
 *
 * Revision 2.11  1995/08/09  23:14:14  asastry
 * CSCdi38504:  Limit DVMRP debug output using access lists for the
 * command 'debug ip dvmrp detail [in | out]'
 *
 * Revision 2.10  1995/08/03  01:12:54  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * Revision 2.9  1995/07/27  22:11:55  jzwiebel
 * CSCdi37791:  Clearing dvmrp routes crashes router
 *
 * Revision 2.8  1995/07/24  07:33:29  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.7  1995/07/18 07:32:39  asastry
 * CSCdi37082:  Enhance 'debug ip dvmrp detail' to allow '[in | out]'.
 *
 * Revision 2.6  1995/07/18  03:54:35  jzwiebel
 * Fixed comments to match cfg_int_ip_igmp.h
 *
 * CSCdi37232:  Update ip multicast comments
 *
 * Revision 2.5  1995/07/12  19:09:44  dino
 * CSCdi36982:  Lets dont send Reports for each DVMRP Probe received on
 *              LANs.
 *
 * Revision 2.4  1995/07/01  07:46:09  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.3  1995/06/10  20:39:46  jzwiebel
 * Enabled neighbor lists and all flags for DVMRP ask2_neighbor packets.
 *
 * CSCdi35660:  DVMRP: enable ask2_neighbor flags and neighbor list
 *
 * Revision 2.2  1995/06/10  04:08:52  dino
 * CSCdi35152:  Cant show DVMRP default route only.
 *
 * Revision 2.1  1995/06/07  21:03:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../ipmulticast/msg_dvmrp.c"	/* Not a typo, see logger.h */
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "name.h"
#include "ieee.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "parser_defs_igmp.h"
#include "../snmp/snmp_api.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/tunnel.h"
#include "../ip/parser_defs_ip.h"
#include "../iprouting/route.h"
#include "../iprouting/route_map.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/pim.h"
#include "../ipmulticast/mbone_mtrace.h"
#include "../ipmulticast/cgmp.h"

/*
 * DVMRP Storage.
 */
sys_timestamp dvmrp_report_timer;
dvmrp_metrictype dvmrp_configured_metrics[DVMRP_MAX_METRIC];
ulong dvmrp_metric_count;
dvmrp_filter dvmrp_filters[DVMRP_MAX_FILTER];
ulong dvmrp_filter_count;
rnhtype *dvmrp_cache;
static ulong dvmrp_cache_count;
uchar dvmrp_debug_detail_flag;
ulong dvmrp_unicast_routing;
static const char mbone_warning[] = {
"WARNING: No access-list specified may cause excessive routes to be advertised
         into the MBONE" };
static ipparamtype mrinfodefaultparams =
    { 0, TOS_PREC_ROUTINE, TTL_DEFAULT, FRAGMENTS_ALLOWED, TRUE }; 
ulong dvmrp_skip_aging;
ulong dvmrp_route_limit = 7000;
ulong dvmrp_routehog_count = DVMRP_DEFAULT_COMPLAINT_COUNT;
queuetype dvmrp_summaryQ;
ulong dvmrp_distance = 0;


/*
 * dvmrp_cisco_version
 *
 * Store DVMRP version and capability bit information in IGMP header.
 */
static inline ulong dvmrp_cisco_version (void)
{
    ulong version;

    version = DVMRP_CISCO_VERSION;
    if (snmp_serving) version |= DVMRP_SNMP_CAPABLE;
    return(version);
}

/*
 * dvmrp_mbone_warning
 *
 * Give a warning message so people don't inadvertently inject thousands
 * of routes into the MBONE. This is to deal with the sensitive mrouted 
 * code. This is giving cisco an unnecessary bad name.
 */
static void dvmrp_mbone_warning (dvmrp_metrictype *dvmrp)
{

    /*
     * Don't send out message if metric is 0.
     */
    if (!dvmrp->metric) return;

    if (!dvmrp->acl) {
	printf(mbone_warning);
    }
}

/*
 * dvmrp_metric_command
 *
 * Process interface subcommand.
 * "ip dvmrp metric <metric> [<acl>] [route-map <map-name>] \
 *                                      [<protocol> <AS-number> | dvmrp]"
 */
void dvmrp_metric_command (parseinfo *csb)
{

    dvmrp_metrictype *dvmrp;
    pdbtype          *pdb;
    int              i, pdb_type, len;
    acl_headertype   *acl;
    
    pdb_type = PDB_IP;
    pdb = router_parse(csb, TRUE, &pdb_type);
    if (GETOBJ(int,2) || *GETOBJ(string,2)) {
	acl = access_parse_acl(GETOBJ(int,2), GETOBJ(string,2),
			       GETOBJ(int,2) == 0, ACL_IP_NAMED_SIMPLE);
	if (!acl)
	    return;
    } else
	acl = NULL;
    
    /*
     * Find entry.
     */
    dvmrp = NULL;
    for (i = 0; i < dvmrp_metric_count; i++) {
	dvmrp = &dvmrp_configured_metrics[i];
	if (dvmrp->idb != csb->interface) continue;
	if (dvmrp->acl != acl) continue;
	if (dvmrp->pdb != pdb) continue;
	if (dvmrp->dvmrp != GETOBJ(int,3)) continue;
	if (dvmrp->route_map &&
	    strcmp(dvmrp->route_map, GETOBJ(string,1))) continue;
	break;
    }
    if (i == dvmrp_metric_count) dvmrp = NULL;

    if (csb->sense) {
	if (!dvmrp) {
	    if (dvmrp_metric_count == DVMRP_MAX_METRIC) {
		printf("\nMax number of DVMRP metrics have been configured");
		return;
	    }
	    dvmrp = &dvmrp_configured_metrics[dvmrp_metric_count];
	    dvmrp_metric_count++;

	    /*	
	     * Just in case the routing process goes away. We get rid of it 
	     * when a "no ip dvmrp metric" command is entered.
	     */
	    if (pdb) mem_lock(pdb);
	}
	dvmrp->metric = GETOBJ(int,1);
	dvmrp->acl = acl;
	dvmrp->idb = csb->interface;
	dvmrp->pdb = pdb;
	dvmrp->dvmrp = GETOBJ(int,3);

	/*
	 * Allocate memory for route-map name if specified.
	 */
	if (!dvmrp->route_map && *GETOBJ(string,1)) {
	    len = strlen(GETOBJ(string,1)) + 1;
	    dvmrp->route_map = malloc(len);
	    if (!dvmrp->route_map) {
		printf("\nNo memory for route-map");
		return;
	    }
	    bcopy(GETOBJ(string,1), dvmrp->route_map, len);
	}

	if (csb->resolvemethod == RES_MANUAL) dvmrp_mbone_warning(dvmrp);
    } else {
	if (!dvmrp) {
	    printf("\nMetric or routing protocol not found");
	    return;
	}

	/*
	 * Delete refcount if we used a pdb.
	 */
	pdb = dvmrp->pdb;
	if (pdb) {
	    free(dvmrp->pdb);
	    dvmrp->pdb = NULL;
	}

	/*
	 * Get rid of route-map reference, if any.
	 */	
	if (dvmrp->route_map) {
	    free(dvmrp->route_map);
	    dvmrp->route_map = NULL;
	}

	/*
	 * Move each element up in the array to maintain order.
	 */
	dvmrp_metric_count--;
	for ( ; i < dvmrp_metric_count; i++) {
	    dvmrp_configured_metrics[i] = dvmrp_configured_metrics[i+1];
        }
    }
}

/*
 * dvmrp_receive_probe
 * 
 * Process received DVMRP probe. This function simply records the fact
 * that there is at least one DVMRP neighbor on the interface. This signals
 * for us to send DVMRP Report packets on interface.
 *
 * This function will treat received DVMRP Report messages as keep-alives.
 * This only does so when a previous DVMRP probe was received. We don't
 * want one cisco to think another cisco is a DVMRP router. cisco's don't
 * send probes.
 *
 * Return FALSE if packet should not be accepted.
 */
boolean dvmrp_receive_probe (idbtype *idb, ipaddrtype source, igmptype *igmp)
{

    boolean enabled;

    DVMRP_BUGINF("\nDVMRP: Received %s on %s from %i",
		(igmp->code == DVMRP_PROBE_CODE) ? "Probe" : "Report",
		idb->namestring, source);

    /*
     * Dont process the probe/report unless we have turned
     * on multicast in the interface and the interface is up and
     * properly configured.
     */
    if (!idb->pim_enabled) return(FALSE);
    if (!idb->ip_address && !idb->ip_unnumbered) return(FALSE);
    if (!interface_up(idb)) return (FALSE);

    /*
     * If neighbor is not a DVMRP Pruner/Grafter and we are configured on
     * this inteface to reject the peer, do so.
     */
    if (idb->dvmrp_reject_non_pruners && 
	!(igmp->address & DVMRP_PRUNE_CAPABLE)) {
	DVMRP_BUGINF("\nDVMRP: Reject non-pruning peer %i", source);
	errmsg(&msgsym(NONPRUNER, DVMRP), source, idb->namestring);
	return(FALSE);
    }

    if (!dvmrp_accept_filter(0, source, idb, NULL, TRUE)) {
	DVMRP_BUGINF(", denied");
	return(FALSE);
    }

    enabled = TIMER_RUNNING(idb->dvmrp_nbr_timer);

    /*
     * Only update timer if a probe was received. Note, that when two ciscos
     * are doing DVMRP unicast routing amoung each other, they only send
     * Reports and not probes. We use idb->dvmrp_nbr_timer to denote DVMRP
     * multicast routers present. 
     */
    if (!enabled && igmp->code == DVMRP_REPORT_CODE) return(TRUE);

    /*
     * Update the version number for the neighbor. Don't bother if
     * it's a multi-access LAN, we can't distinguish between various
     * neighbors.
     */
    if (is_p2p(idb)) {
	idb->dvmrp_nbr_version = igmp->address;
    } else {
	idb->dvmrp_nbr_version = 0;
    }

    /*
     * Reports from DVMRP multicast routers also update the timer. We don't 
     * want cisco Reports to update the timer so we have to check to see if 
     * the Report can from a PIM router.
     */
    if (pim_find_nbr(source, idb)) return(TRUE);

    /*
     * Reset neighbor expiration timer for interface so we know when the
     * last DVMRP neighbor goes away.
     */
    TIMER_START(idb->dvmrp_nbr_timer, DVMRP_NBR_EXPIRATION);

    /*
     * If this is the first neighbor, send report now. Add interface to
     * outgoing interface for all group entries.
     */
    if (!enabled) {
	GET_TIMESTAMP(idb->dvmrp_nbr_uptime);
	errmsg(&msgsym(NBRUP, DVMRP), source, idb->namestring);
	dvmrp_send_report(idb, DVMRPROUTERS);
	pim_add_mroute_idb(idb, source, FALSE);
    }

    /*
     * Start periodic report timer, if not running.
     */
    if (!TIMER_RUNNING(dvmrp_report_timer)) {
	TIMER_START(dvmrp_report_timer, DVMRP_REPORT_INTERVAL);
    }

    /*
     * Start timer for clearing idb->dvmrp_routes_rcvd so we can count
     * the number of routes received from the neighbor within minute (or so)
     * interval.
     */
    if (!TIMER_RUNNING(idb->dvmrp_routes_rcvd_timer)) {
	GET_TIMESTAMP(idb->dvmrp_routes_rcvd_timer);
    }
    return(TRUE);
}

/*
 * dvmrp_init_idb
 *
 * Initialize DVMRP related fields in the idb. This is done at IP multicast
 * subsystem startup. Everything not explicitly set is assumed to be
 * zero.
 */
void dvmrp_init_idb (idbtype *idb)
{
    idb->dvmrp_metric_offset_in  = 1;
    idb->dvmrp_metric_offset_out = 0;
    idb->dvmrp_routes_rcvd = 0;
    idb->dvmrp_auto_summary = TRUE;
}

/*
 * dvmrp_disable_idb
 *
 * No DVMRP probes have come in from any neighbors on this interface.
 * Disable DVMRP on this interface.
 */
void dvmrp_disable_idb (idbtype *interface)
{

    idbtype *idb;
    list_element *l_elt;
    
    /*
     * Already disabled on interface.
     */
    if (!TIMER_RUNNING(interface->dvmrp_nbr_timer)) return;

    TIMER_STOP(interface->dvmrp_nbr_timer);
    TIMER_STOP(interface->dvmrp_routes_rcvd_timer);
    interface->dvmrp_routes_rcvd = 0;
    interface->dvmrp_proutes_rcvd = 0;
    interface->dvmrp_nbr_version = 0;
    if (DVMRP_TUNNEL(interface)) {
	errmsg(&msgsym(NBRDOWN, DVMRP), interface->hwptr->tunnel->destination,
	       interface->namestring);
    }
    if (interface_up(interface)) {
	DVMRP_BUGINF("\nDVMRP: Neighbors timed out on interface %s", 
		     interface->namestring);
    } else {
	DVMRP_BUGINF("\nDVMRP: Neighbor went down on interface %s", 
		     interface->namestring);
    }

    /*
     * If doing DVMRP unicast routing in this router, don't disable the
     * Report timer.
     */
    if (dvmrp_unicast_routing) return;
	
    /*
     * Determine if report timer should be canceled. Do so if there are
     * no DVMRP active interfaces.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->igmp_enabled) continue;
	if (DVMRP_ACTIVE(idb)) break;
    }
    if (!idb) TIMER_STOP(dvmrp_report_timer);
}

/*
 * dvmrp_store_route_in_pkt
 * 	
 * Take route and encode in DVMRP Report packet format. Adjust packet pointer
 * and size values based on mask length.
 */
static void dvmrp_store_route_in_pkt (char **pkt_ptr, ulong *size_ptr, 
			       ipaddrtype address, ipaddrtype mask, 
			       ulong metric)
{
    char *pkt, *nptr;
    ulong size, width;
    
    pkt = *pkt_ptr;
    size = *size_ptr;

    /*
     * Store mask. Length is 3 bytes. Defines how many network
     * bytes there are.
     */
    PUTLONG(pkt, (mask << 8));
    width = 0;
    if ((mask << 8)) {
	while (*pkt != 0) {
	    width++;
	    pkt++;
	}
    }
    pkt += (3 - width);
    width++;
    size += (width + 4);

    /*
     * Store network number. Increment based on width calculated
     * by mask. Store metric of 1 unless we're doing split
     * horizon with poison reverse.
     */
    nptr = (char *) &address;
    while (width) {
	*pkt++ = *nptr++;
	width--;
    }
    *pkt++ = metric | DVMRP_END;

    /*
     * Return values to caller.
     */
    *pkt_ptr = pkt;
    *size_ptr = size;
}

/*
 * dvmrp_summary_address
 *
 * Check to see if there is a summary address configured for this route.
 */
static boolean dvmrp_summary_address (idbtype *idb, ipaddrtype address, 
			       ipaddrtype mask)
{
    dvmrp_sumtype *sum, *save_sum;

    /*
     * Find the longest masked summary.
     */
    save_sum = NULL;
    for (sum = dvmrp_summaryQ.qhead; sum; sum = sum->next) {
	if (sum->idb != idb) continue;
	if (mask >= sum->mask && ((address & sum->mask) == sum->address)) {
	    if (!save_sum || save_sum->mask < sum->mask) {
		save_sum = sum;
	    }
	}
    }

    /*
     * Keep track of the number of more specific routes that match this
     * summary. So later we know if we should advertise the summary.
     */
    if (save_sum) save_sum->count++;
    return((boolean) save_sum);
}

/*
 * dvmrp_clear_summary
 *
 * Clear the sum->count in all the summary entries.
 */
static void dvmrp_clear_summary (void)
{
    dvmrp_sumtype *sum;

    for (sum = dvmrp_summaryQ.qhead; sum; sum = sum->next) {
	sum->count = 0;
    }
}

/*
 * dvmrp_build_summary
 *
 * Look at dvmrp_summaryQ and advertise summaries with metric 1.
 */
static void dvmrp_build_summary (igmptype **igmp_ptr, paktype **pak_ptr, 
			  char **pkt_ptr, ulong *size_ptr, ipaddrtype nbr, 
			  idbtype *idb, ulong *route_count)
{
    dvmrp_sumtype *sum;

    for (sum = dvmrp_summaryQ.qhead; sum; sum = sum->next) {
	if (sum->idb != idb) continue;
	if (!sum->count) continue;

	if ((*size_ptr + DVMRP_ENTRY_SIZE) > DVMRP_MAX_SIZE) {
	    dvmrp_send_report_packet(*pak_ptr, *igmp_ptr, idb, nbr, *size_ptr);
	    *pak_ptr = dvmrp_getbuffer(igmp_ptr, pkt_ptr, size_ptr);
	    if (!*pak_ptr) return;
	}

	if (*route_count > dvmrp_route_limit) {
	    errmsg(&msgsym(ROUTELIMIT, DVMRP), dvmrp_route_limit);
	    return;
	}

	dvmrp_store_route_in_pkt(pkt_ptr, size_ptr, sum->address, sum->mask, 
				 sum->metric);
	(*route_count)++;

	DVMRP_OUT_BUGINF(sum->address, 
	     ("\nDVMRP: Report %i/%d, metric %d, from configured summary", 
	      sum->address, ip_bitsinmask(sum->mask), sum->metric));
	DVMRP_OUT_BUGINF(sum->address, 
	     ("\n       %d more specific routes matched", sum->count));
    }
}
static ndbtype temp_ndb;

/*
 * dvmrp_send_report
 *
 * Send DVMRP Report message on interfaces where we discovered that
 * DVMRP neighbors exist. If this function is called with a specific
 * idb, only send report on that interface. If a nbr is also with an
 * interface, the report is unicast to a single neighbor.
 */
void dvmrp_send_report (idbtype *interface, ipaddrtype nbr)
{
    idbtype    *idb;
    paktype    *pak;
    igmptype   *igmp;
    ndbtype    *ndb;
    ndbtype    *ndbptr, *sdbptr;
    idbtype    *nexthop_idb;
    char       *pkt;
    dvmrptype  *route;
    int        nbucket, sbucket;
    ulong      size, metric, unicast_count, dvmrp_count;
    ipaddrtype mask, idb_majornet, idb_mask;
    boolean    do_default, found_summary;
    list_element *l_elt;
    
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (interface && interface != idb) continue;
	if (!idb->igmp_enabled) continue;
	if (!DVMRP_ACTIVE(idb) && !idb->dvmrp_unicast) continue;
	if (!idb->ip_address && !idb->ip_unnumbered) continue;
	if (!interface_up(idb)) continue;

	pak = dvmrp_getbuffer(&igmp, &pkt, &size);
	if (!pak) return;

	DVMRP_BUGINF("\nDVMRP: Building Report for %s", idb->namestring);
	if (nbr) DVMRP_BUGINF(" %i", nbr);
	unicast_count = 0;
	dvmrp_count = 0;
	found_summary = FALSE;

	/*
	 * Use the DVMRP routing table for Reports in addition to the unicast
	 * routing table for DVMRP tunnels. Also, if we have an active
	 * dvmrp neighbor, we have to poison reverse.
	 */
	if (DVMRP_TUNNEL(idb) || idb->dvmrp_unicast) {
	    dvmrp_send_tunnel_report(&igmp, &pak, &pkt, &size, nbr, idb,
				     &dvmrp_count, &found_summary);
	    if (!pak) return;

	    /*
	     * If not enough room in packet, send packet and start new buffer
	     * for unicast routes. Or, if we exceeded the route-limit, just
	     * send what we already built.
	     */
	    if ((size + DVMRP_ENTRY_SIZE) > DVMRP_MAX_SIZE ||
		dvmrp_count > dvmrp_route_limit) {
		dvmrp_send_report_packet(pak, igmp, idb, nbr, size);
		if (dvmrp_count > dvmrp_route_limit) continue;
		pak = dvmrp_getbuffer(&igmp, &pkt, &size);
		if (!pak) return;
	    }
	}

	/*
	 * Get majornet for interface the advertisement will be sent out.
	 * If there is no IP address on interface and is not unnumbered,
	 * send network number routes only.
	 */
	idb_majornet = get_majornet(idb->ip_address);
	idb_mask = idb->ip_nets_mask;
	if (idb->ip_unnumbered) {
	    idb_majornet = get_majornet(idb->ip_unnumbered->ip_address);
	    idb_mask = idb->ip_unnumbered->ip_nets_mask;
	}

	do_default = idb->dvmrp_send_default;

	/*
	 * Scan through unicast routing table adverting all network routes.
	 */
	nbucket = 0;
	sbucket = 0;
	ndbptr = NULL;
	sdbptr = NULL;
	while ((ndb = geteachroute(&ndbptr, &nbucket, &sdbptr, &sbucket, 
				   TRUE))) {
	    
	    /*
	     * If configured to send default, put in beginning of Report 
	     * message.
	     */
	    if (do_default) {
		do_default = FALSE;
		ndbptr = sdbptr = NULL;
		nbucket = sbucket = 0;
		temp_ndb.netnumber = 0;
		temp_ndb.pdbindex = STATIC_DISTANCE;
		ndb = &temp_ndb;
		nexthop_idb = NULL;
		mask = 0;
	    } else {
		
		/*
		 * Don't advertise martians.
		 */
		if (martian(ndb->netnumber, FALSE)) continue;

		/*
		 * Don't advertise majornet out an interface which is a subnet
                 * of the same majornet. However, do adverise majornet out an 
                 * unnumbered interface pointing to the majornet.
		 */
		if (ndb->netnumber == idb_majornet &&
		    ndb->netsmask == get_majormask(ndb->netnumber)) {
		    if (idb_mask > ndb->netsmask) continue;
		}

		/*
		 * Don't advertise route if next-hop interface is not up. Don't
		 * advertise route if next-hop interface is not doing multicast
		 * routing.
		 */
		nexthop_idb = ipmulticast_resolve_nexthop(ndb, NULL,
							  ROUTINGDEPTH);
		if (!nexthop_idb || !interface_up(nexthop_idb) ||
		    !nexthop_idb->ip_mrouting) {
		    continue;
		}

		/*
		 * Only advertise subnets out the same majornet or supernet
		 * routes if they exist. Otherwise, advertise the majornet or
                 * supernet out another majornet if auto-summarization is in
		 * effect. ndbptr contains the dummy ndb. Don't process 
		 * subnets in geteachroute() by setting sdb pointers to NULL.
		 */
		if (get_majornet(ndb->netnumber) != idb_majornet &&
		    ndb->netsmask >= get_majormask(ndb->netnumber) &&
		    idb->dvmrp_auto_summary) {
		    ndb = ndbptr;
		    mask = get_majormask(ndb->netnumber);
		    sdbptr = NULL;
		    sbucket = 0;
		} else {

		    /*
		     * Since DVMRP encodes network masks with 3 bytes (and
		     * therefore assumes that all networks have at least a
		     * mask of 0xff000000) you cannot encode a class-A supernet
		     * in any DVMRP reports. Log such an event and loop.
		     */
		    if (!(ndb->netnumber & CLASS_A_BIT) && 
			ndb->netsmask < get_majormask(ndb->netnumber)) {
			DVMRP_OUT_BUGINF(ndb->netnumber,
                       ("\nDVMRP: Can't encode Class A supernet address %i/%d",
			ndb->netnumber, ip_bitsinmask(ndb->netsmask)));
			continue;
		    }
		    mask = ndb->netsmask;
		}

		/*
		 * Don't advertise route if the DVMRP route is preferred.
		 */
		route = dvmrp_get_route(ndb->netnumber, mask, FALSE);
		if (DVMRP_VALID(route) &&
		    !dvmrp_prefer_unicast_route(route, ndb)) continue;
	    }

	    /*
	     * Don't advertise route if configured with metric 0 or if no
	     * filters are configured.
	     */
	    metric = dvmrp_setup_metric(ndb, idb, FALSE);
	    if (!metric || metric == -1) continue;

	    /*
	     * Don't advertise route if it is a more specific route than
	     * a configured summary route.
	     */
	    if (dvmrp_summary_address(idb, ndb->netnumber, ndb->netsmask)) {
		found_summary = TRUE;
		continue;
	    }

	    /*
	     * If next-hop idb known, do split horizon check.
	     */
	    if (nexthop_idb && nexthop_idb == idb) {
		metric = DVMRP_INFINITY;
	    }

	    dvmrp_store_route_in_pkt(&pkt, &size, ndb->netnumber, mask, 
				     metric);

	    DVMRP_OUT_BUGINF(ndb->netnumber, 
		     ("\nDVMRP: Report %i/%d, metric %d", ndb->netnumber,
		      ip_bitsinmask(mask), metric));

	    /*
	     * Check if route-limit has exceeded. If so complain with log
	     * message. Someone is trying to trash the MBONE again.
	     */
	    unicast_count++; 
	    if (unicast_count + dvmrp_count > dvmrp_route_limit) {
		errmsg(&msgsym(ROUTELIMIT, DVMRP), dvmrp_route_limit);
		break;
	    }

	    /*
	     * If not enough room in packet, send packet and build new
	     * one.
	     */
	    if ((size + DVMRP_ENTRY_SIZE) > DVMRP_MAX_SIZE) {
		dvmrp_send_report_packet(pak, igmp, idb, nbr, size);
		pak = dvmrp_getbuffer(&igmp, &pkt, &size);
		if (!pak) return;
	    }

	    /*
	     * If originating default route only, terminate loop here.
	     */
	    if (idb->dvmrp_send_default == DVMRP_DEFAULT_ONLY) break;
	}

	/* 
	 * Do summaries, if any.
	 */
	if (idb->dvmrp_send_default != DVMRP_DEFAULT_ONLY && found_summary) {
	    dvmrp_build_summary(&igmp, &pak, &pkt, &size, nbr, idb, 
				&unicast_count);
	}

	/*
	 * Finished traversing routing table. Send packet if there is
	 * something in it.
	 */
	if (size != (IPHEADERBYTES(NOPT) + IGMPHEADERBYTES)) {
	    dvmrp_send_report_packet(pak, igmp, idb, nbr, size);
	} else {
	    datagram_done(pak);
	}

	DVMRP_BUGINF("\nDVMRP: %d unicast, %d DVMRP routes advertised", 
		     unicast_count, dvmrp_count);
	idb->dvmrp_unicast_out_count = unicast_count;
	idb->dvmrp_dvmrp_out_count = dvmrp_count;

	/*
	 * Advertise groups on this interface, if there is a discovered
	 * dvmrp neighbor and the interface is not a tunnel and not running
	 * dvmrp unicast routing. This is to make sure that the
	 * dvmrp neigbhor forwards multicast packets destined to
	 * these groups. If we have dvmrp unicast, we would have
	 * poisoned reversed already.
	 */
	if (!DVMRP_TUNNEL(idb) && !idb->dvmrp_unicast && DVMRP_ACTIVE(idb)) {
	    dvmrp_send_groups(idb);
	}
    }
    dvmrp_clear_summary();
}

/*
 * dvmrp_send_tunnel_report
 *
 * Build and send a Report packet on a DVMRP tunnel. This functions builds
 * Reports from the DVMRP routing table (because mrouted machines expect
 * poison-reverse packets or they don't send data to you). If there is
 * a corresponding route in the unicast routing table, it is only advertised
 * instead of the DVMRP route if the admin distance is better.
 */
void dvmrp_send_tunnel_report (igmptype **igmp, paktype **pak, char **pkt,
			       ulong *size, ipaddrtype nbr, idbtype *idb,
			       ulong *route_count, boolean *found_summary)
{
    if (!DVMRP_ROUTES) return;
    
    rn_walktree_timed(dvmrp_cache, dvmrp_build_report_entry,
		      ip_radix_successor, igmp, pak, pkt, size, nbr, idb,
		      route_count, found_summary);
}

/*
 * dvmrp_build_report_entry
 *
 * Get an entry from the DVMRP routing table and place in Report packet.
 */
int dvmrp_build_report_entry (rntype *node, va_list args)
{

    dvmrptype  *route;
    igmptype   **igmp_ptr;
    paktype    **pak_ptr;
    ulong      *size_ptr, *route_count;
    idbtype    *idb;
    ndbtype    *ndb;
    char       **pkt_ptr;
    boolean    *found_summary;
    ipaddrtype nbr, dest, mask;
    int        metric;
    idbtype    *nexthop_idb;
    
    route = (dvmrptype *) node;

    /*
     * An expired route in the routing table is like one that doesn't exist. It
     * will be deleted next aging interval.
     */
    if (AWAKE(route->expires)) return(0);

    igmp_ptr = va_arg(args, igmptype **);
    pak_ptr = va_arg(args, paktype **);
    pkt_ptr = va_arg(args, char **);
    size_ptr = va_arg(args, ulong *);
    nbr = va_arg(args, ipaddrtype);
    idb = va_arg(args, idbtype *);
    route_count = va_arg(args, ulong *);
    found_summary = va_arg(args, boolean *);

    /*
     * Do unicast routing table lookup and prefer it if better distance than 
     * the DVMRP route. Don't do anything here, it will be advertised in
     * calling functions.  If, however, the unicast route is reached
     * through a non multicast-enabled interface and the DVMRP route was
     * learned over this interface, then we may need to poison reverse the
     * DVMRP route.
     */
    dest = ip_radix2addr(route->dest);
    mask = ip_radix2addr(route->mask);
    ndb = ip_rtlookup(dest, mask);
    if (dvmrp_prefer_unicast_route(route, ndb)) {
	nexthop_idb = ipmulticast_resolve_nexthop(ndb, NULL, ROUTINGDEPTH);

	if (nexthop_idb && interface_up(nexthop_idb) &&
	    nexthop_idb->ip_mrouting) {
	    goto next;
	}
	if (route->idb != idb) {
	    goto next;
	}
    }
        
    /*
     * If sending default route only, only include poison-reverse routes and
     * no others.
     */ 
    if (idb->dvmrp_send_default == DVMRP_DEFAULT_ONLY && route->idb != idb) {
	goto next;
    }

    /*
     * If we are configured to send default on this interface and we find
     * the default in the DVMRP routing table, don't advertise with the
     * DVMRP routing table metric, use the configured metric. This will
     * be done when we traverse the unicast routes (after the calling
     * function completes. We don't want to advertise 0.0.0.0/0 twice in the 
     * same Report packet.
     */
    if (idb->dvmrp_send_default && !dest) {
	goto next;
    }

    /*
     * If this source is reachable over this tunnel, or if we an
     * active dvmrp neighbor, do poison-reverse. If
     * doing plain-ole DVMRP unicast routing, do split horizon.
     */
    if (route->idb == idb && route->metric < DVMRP_INFINITY) {

	/*
	 * We have to send poison-reverses in case we have an active
	 * dvmrp neighbor. This is done so that the upstream router
	 * does not consider this interface as a leaf.
	 */
	if (idb->dvmrp_unicast && !DVMRP_ACTIVE(idb)) goto next;
	metric = route->metric + DVMRP_INFINITY;
    } else {

	/*
	 * Run filters against DVMRP routes.
	 */
	temp_ndb.netnumber = dest;
	temp_ndb.pdbindex = UNKNOWN_DISTANCE;
	metric = dvmrp_setup_metric(&temp_ndb, idb, TRUE);
	if (!metric) goto next;
	if (metric == -1) {

	    /*
	     * The route is a DVMRP route and there is no
	     * configured metric. Add outgoing metric-offset,
	     * if present.
	     */
	    metric = route->metric + idb->dvmrp_metric_offset_out;
	}
    }

    /*
     * Check if we need to summarize routes. We don't summarize poisoned-
     * reversed routes.
     */
    if (metric < DVMRP_INFINITY) {
	if (dvmrp_summary_address(idb, dest, mask)) {
	    *found_summary = TRUE;
	    goto next;
	}
    }

    dvmrp_store_route_in_pkt(pkt_ptr, size_ptr, dest, mask, metric);
    (*route_count)++;
    DVMRP_OUT_BUGINF(dest, 
	     ("\nDVMRP: Report %i/%d, metric %d, from DVMRP table", dest, 
	      ip_bitsinmask(mask), metric));

    /*
     * Check route-limit, someone trying to flood the MBONE with too
     * many routes.
     */
    if (*route_count > dvmrp_route_limit) {
	errmsg(&msgsym(ROUTELIMIT, DVMRP), dvmrp_route_limit);
	return(1);
    }

    /*
     * If not enough room in packet, send packet and build new
     * one.
     */
    if ((*size_ptr + DVMRP_ENTRY_SIZE) > DVMRP_MAX_SIZE) {
	dvmrp_send_report_packet(*pak_ptr, *igmp_ptr, idb, nbr, *size_ptr);
	*pak_ptr = dvmrp_getbuffer(igmp_ptr, pkt_ptr, size_ptr);
	if (!*pak_ptr) return(-1);
    }

  next:
    return(0);
}

/*
 * dvmrp_setup_metric
 *
 * Advertise destination if configured with non-zero metric. If destination
 * does not have a configured metric and is directly connected, advertise
 * with metric 1.
 *
 * This code will also check if there are routes from a specific routing
 * protocol that should be advertised. DVMRP routes are special cased since
 * there is no pdb associated with it. Access-list entries that have the
 * "dvmrp" keyword are only applied to DVMRP routes.
 */
int dvmrp_setup_metric (ndbtype *ndb, idbtype *idb, boolean dvmrp_route)
{

    dvmrp_metrictype *dvmrp;
    int              i;
    boolean          deny;

    deny = FALSE;
    for (i = 0; i < dvmrp_metric_count; i++) {
	dvmrp = &dvmrp_configured_metrics[i];
	if (dvmrp->idb != idb) continue;
	if (dvmrp->pdb && dvmrp->pdb->index != ndb->pdbindex) continue;
	if (dvmrp_route && !dvmrp->dvmrp) continue;
	if (!dvmrp_route && dvmrp->dvmrp) continue;
	if (!dvmrp_route && dvmrp->route_map) {
	    if (!route_map_ip_check(dvmrp->route_map, ndb, 
				    INDEX2PDB(ndb->pdbindex))) continue;
	}

	if (!dvmrp->acl) {
	    return(dvmrp->metric);
	} else {

	    /*
	     * Handle case where there is an access-list reference but the
	     * access-list is not configured yet. Assume an implicit deny.
	     */
	    if (dvmrp->acl->list && nfast_check(dvmrp->acl, ndb->netnumber)) {
		return(dvmrp->metric);
	    } else {
		
		/*
		 * If this route didn't match the access-list but the metric
		 * configured is 0 (which means don't advertise the route),
		 * then this route *should* be advertised with native
		 * metric (1 for unicast, DVMRP routing table metric for DVMRP
		 * routes).
		 */
		if (!dvmrp->metric) continue;
	    }
	    deny = TRUE;
	}
    }

    /*
     * If directly connected network was not denied or there was no
     * configured metric, advertise (default behavior) with metric 1.
     */
    if (!deny && (ndb->attrflags & NET_ATTACHED)) {
	return(1);
    }

    /*
     * Use default metric for the default route if we are configured to
     * originate the DVMRP default route. Default metric is 1.
     *
     * Looking at ndb->pdbindex == UNKNOWN_DISTANCE means that the default
     * route resides in the DVMRP routing table. We want to use the DVMRP
     * routing table metric in this case.
     */
    if (!deny && !ndb->netnumber && !(ndb->pdbindex == UNKNOWN_DISTANCE) && 
	idb->dvmrp_send_default) {
	return(1);
    }

    /*
     * If no filter applied, let caller decide on metric. For unicast routes,
     * it will be 1. For DVMRP routes, it will be the DVMRP routing table
     * metric.
     */
    if (!deny) return(-1);
    
    /*
     * Everything else, don't advertise.
     */
    return(0);
}

/*
 * dvmrp_send_report_packet
 *
 * Checksum and send DVMRP report packet.
 */
void dvmrp_send_report_packet (paktype *pak, igmptype *igmp, idbtype *idb,
			       ipaddrtype nbr, ulong size)
{

    /*
     * Compute checksum.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    DVMRP_BUGINF("\nDVMRP: Send Report on %s to ", idb->namestring);

    /*
     * Send to IP.
     */
    if (DVMRP_TUNNEL(idb)) {
	DVMRP_BUGINF("%i", idb->hwptr->tunnel->destination);
	dvmrp_send_on_tunnel(pak, size, idb);
    } else {
	DVMRP_BUGINF("%i", nbr);
	ipwrite(pak, size, IGMP_PROT, idb->ip_address, nbr, idb, NULL, 
		&igmprouteparams);
    }
    igmp_traffic.output_dvmrp++;
}

/*
 * dvmrp_getbuffer
 *
 * Get buffer for IGMP packet and set up fixed header fields and pointers.
 */
paktype *dvmrp_getbuffer (igmptype **igmp_ptr, char **pkt_ptr, ulong *size_ptr)
{

    paktype  *pak;
    igmptype *igmp;
    char     *pkt;
    ulong    size;

    pak = getbuffer(DVMRP_MAX_SIZE);
    if (!pak) return(NULL);

    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_DVMRP_TYPE;
    igmp->code = DVMRP_REPORT_CODE;
    igmp->checksum = 0;
    igmp->address = dvmrp_cisco_version();
    pkt = (char *) igmp->data;

    *igmp_ptr = igmp;
    *pkt_ptr = pkt;
    *size_ptr = size;
    return(pak);
}

/*
 * dvmrp_send_groups
 *
 * Advertise all known groups not in prune state to DVMRP neighbors.
 */
void dvmrp_send_groups (idbtype *idb)
{

    gdbtype        *gdb;
    midbtype       *midb;
    int            i;
    igmp_cachetype igmp;

    DVMRP_BUGINF("\nDVMRP: Sending IGMP Reports for known groups on %s", 
		 idb->namestring);

    /*
     * Scan through hash table for all groups. Does not look at sources.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {

	    /*
	     * If there are directly connected members, no need
	     * to advertise it, the members will.
	     */
	    if (igmp_find_group(gdb->mdb.group, idb)) continue;

	    /*
	     * If entire group is in prune state or there are no
	     * outgoing interfaces, don't report group.
	     */
	    if (MDB_OUTPUTQ_NULL(&gdb->mdb)) continue;

	    /*
	     * If we are getting this information from a single point
	     * via DVMRP only, don't advertise group. Kinda doing
	     * split-horizon on group.
	     */
	    midb = (midbtype *) gdb->mdb.if_outputQ.qhead;
	    if (!midb->next && midb->idb == idb) continue;

	    /*
	     * Advertise group by sending an IGMP Report.	
	     */
	    DVMRP_OUT_BUGINF(gdb->mdb.group, 
		     ("\nDVMRP: Advertise group %i on %s", gdb->mdb.group,
		      idb->namestring));
	    igmp.group = gdb->mdb.group;
	    igmp.idb = idb;
	    igmp_send_report(&igmp);
	}
    }
}

/*
 * dvmrp_receive_report
 *
 * Process incoming DVMRP report packet. DVMRP Reports are only processed
 * when received over a DVMRP tunnel. In all other cases, they are used
 * as keepalives.
 */
void dvmrp_receive_report (igmptype *igmp, ipaddrtype source, idbtype *idb,
			   int length)
{

    uchar      *ptr, *addr_ptr;
    ipaddrtype address, mask;
    ulong      byte_count, metric, i, distance;
    boolean    more_dests;

    length -= IGMPHEADERBYTES;
    ptr = (uchar *) igmp->data;

    /*
     * Scan through packet.
     */
    while (length > 3) {
	mask = 0xff;
	byte_count = 1;
	metric = 0;

	/*
	 * Get mask.
	 */
	for (i = 0; i < 3; i++) {
	    if (*ptr) byte_count++;
	    mask = (mask << 8) | *ptr++;
	    length--;
	}

	/*
	 * Check if mask is contiguous. If not, ignore report from DVMRP
	 * neighbor, he is broken.
	 */
	if (!mask_contiguous(mask)) {
	    errmsg(&msgsym(BADMASK, DVMRP), mask, source);
	    return;
	}

	more_dests = TRUE;
	while (more_dests) {

	    /*
	     * Is there enough packet to hold destination and metric.
	     */
	    if (length < byte_count + 1) break;

	    /*
	     * Get address.
	     */
	    address = 0;
	    addr_ptr = (uchar *) &address;
	    for (i = 0; i < byte_count; i++) {
		*addr_ptr++ = *ptr++;
		length--;
	    }

	    /*
	     * Get metric.
	     */
	    metric = *ptr++;
	    length--;
	    more_dests = (metric & DVMRP_END) ? FALSE : TRUE;
	    metric &= ~DVMRP_END;

	    DVMRP_IN_BUGINF(address, 
		    ("\nDVMRP: Origin %i/%d, metric %d, metric-offset %d", 
		     address, ip_bitsinmask(mask), metric, 
		     idb->dvmrp_metric_offset_in));

	    /*
	     * Add metric-offset to announced metric.
	     */
	    metric += idb->dvmrp_metric_offset_in;

	    /*
	     * Store in routing table if metric less than infinity.
	     */
	    if (metric >= DVMRP_INFINITY) {
		idb->dvmrp_proutes_rcvd++;
		DVMRP_IN_BUGINF(address, (", infinity"));
		continue;
	    } else {
		idb->dvmrp_routes_rcvd++;
		DVMRP_IN_BUGINF(address, (", distance %d", distance));
	    }

	    /*
	     * Apply any input filters.
	     */
	    if (!dvmrp_accept_filter(address, source, idb, &distance, FALSE)) {
		DVMRP_IN_BUGINF(address, (", filtered"));
		continue;
	    }

	    /*
	     * If default route, don't accept it if we are generating one
	     * out this interface. Set mask for default route to 0.0.0.0 to
	     * store in routing table.
	     */
	    if (!address && mask == CLASS_A_MASK) {
		if (idb->dvmrp_send_default) {
		    DVMRP_IN_BUGINF(address, (", reject default route"));
		    continue;
		}
		mask = 0;
	    }
	    dvmrp_add_route(address, mask, source, idb, metric, distance);
	}
    }
}

/*
 * dvmrp_prefer_unicast_route
 *
 * Compare DVMRP route against unicast route.  Return TRUE if the
 * unicast route is strictly better in distance or, if equidistant,
 * then if it is connected.  FALSE otherwise.
 */
boolean
dvmrp_prefer_unicast_route (dvmrptype *route, ndbtype *ndb)
{
    return(ndb && (ndb->distance < route->distance ||
		   (ndb->distance == route->distance &&
		    ndb->attrflags & NET_ATTACHED &&
		    !(NDB2PROCTYPE(ndb) & PROC_STATIC))));
}
/*
 * dvmrp_add_route
 *
 * Adds or replaces DVMRP route in DVMRP routing table.
 */
void dvmrp_add_route (ipaddrtype dest, ipaddrtype mask, ipaddrtype next_hop,
		      idbtype *idb, ulong metric, ulong distance)
{

    dvmrptype *route;

    /*
     * Initialize radix tree if this is the first route to be added.
     */
    if (!DVMRP_ROUTES) {
	rn_inithead((void **) &dvmrp_cache, IPRADIXDATAOFFSET);
	dvmrp_skip_aging = DVMRP_AGING_RATE;
    }

    route = dvmrp_get_route(dest, mask, FALSE);

    /*
     * If route found, compare distance/metrics. Use smaller metric.
     * If the same metric use route from smaller IP next-hop address.
     * Accept larger metric from the same next-hop so counting to infinity
     * can happen faster.
     */
    if (route) {
	if (distance > route->distance) return;

	/*
	 * Allow routes with better distance to override the 
	 * current route. If they have the same distance,
	 * choose better metric and if there is a tie, choose
	 * lower IP address.
	 */
	if (distance == route->distance) {
	    if (metric > route->metric && next_hop != route->next_hop) return;
	    if (metric == route->metric && next_hop > route->next_hop) return;
	}
	
	if (next_hop != route->next_hop && AWAKE(route->expires)) {
	    GET_TIMESTAMP(route->uptime);
	}
    } else {

	/*
	 * Build new entry.
	 */
	route = malloc(sizeof(dvmrptype));
	if (!route) return;

	GET_TIMESTAMP(route->uptime);
	ip_addr2radix(dest, route->dest);
	ip_addr2radix(mask, route->mask);
 	if (!rn_addroute(route->dest, route->mask, dvmrp_cache, 
 			 route->radix)) {
 	    free(route);
 	    return;
 	}
	dvmrp_cache_count++;
    }

    /*
     * Update fields in route entry.
     */
    route->next_hop = next_hop;
    route->idb = idb;
    route->metric = metric;
    route->distance = distance;
    TIMER_START(route->expires, DVMRP_EXPIRATION_TIME);
}

/*
 * dvmrp_get_route
 *
 * Lookup DVMRP route in DVMRP routing table. Do either longest match lookup
 * or exact match lookup.
 */
dvmrptype *dvmrp_get_route (ipaddrtype dest, ipaddrtype mask, boolean best)
{

    dvmrptype   *route;
    ipradixtype address;
    ipaddrtype  addr, msk;

    if (!DVMRP_ROUTES) return(NULL);

    ip_addr2radix(dest, address);
    route = (dvmrptype *) rn_match(address, dvmrp_cache);
    if (!route) return(NULL);

    if (best) {
	addr = ip_radix2addr(route->dest);
	msk = ip_radix2addr(route->mask);
	if ((dest & msk) != addr) return(NULL);
    } else {
	while (route && mask != ip_radix2addr(route->mask)) {
	    route = (dvmrptype *) route->radix[0].rn_dupedkey;
	}
    }
    return(route);
}

/*
 * dvmrp_age_routes
 *
 * Time out routes from the DVMRP routing table.
 */
void dvmrp_age_routes (void)
{

    ulong deleted;

    deleted = 0;
    DVMRP_BUGINF("\nDVMRP: Aging routes");

    rn_walktree(dvmrp_cache->rnh_treetop, dvmrp_age_one_route, &deleted);

    DVMRP_BUGINF(", %d entries expired", deleted);
}

/*
 * dvmrp_age_one_route
 * 
 * Look at a specific DVMRP route to see if it has timed-out.
 */
int dvmrp_age_one_route (rntype *node, va_list args)
{
    dvmrptype *route;
    ulong     *count;

    route = (dvmrptype *) node;
    count = va_arg(args, ulong *);

    if (SLEEPING(route->expires)) return(0);

    (*count)++;
    dvmrp_delete_route (route);
    return(0);
}

/*
 * dvmrp_delete_route
 *
 * Remove a dvmrp route from the radix tree
 */
int dvmrp_delete_route (dvmrptype *route)
{

    rn_delete(route->dest, route->mask, dvmrp_cache);
    free(route);
    dvmrp_cache_count--;
    return(0);
}

/*
 * dvmrp_clear_route
 * 
 * Process "clear ip dvmrp route [<route>]  | [*]" command.
 */
void dvmrp_clear_route (parseinfo *csb)
{

    dvmrptype *route;
    ipaddrtype address;

    address = GETOBJ(paddr,1)->ip_addr;

    /*
     * Delete the entire dvmrp routing table
     */
    if (!address) {
        if (!DVMRP_ROUTES) return;
        rn_walktree(dvmrp_cache->rnh_treetop, dvmrp_clear_one_route);
    } else {

        /*
         * Clear a single route.
         */
        route = dvmrp_get_route(address, 0, TRUE);
        if (route) {	
            dvmrp_clear_one_route((rntype *) route, NULL);
        } else {
            printf("\nRoute not found");
        }
    }
}

/*
 * dvmrp_clear_one_route
 *
 */
int dvmrp_clear_one_route (rntype *node, va_list args)
{

    dvmrptype *route;

    route = (dvmrptype *) node;

    dvmrp_delete_route(route);

    return(0);
}

static const char dvmrp_banner[] = { "DVMRP Routing Table" };
/*
 * dvmrp_show_route
 * 
 * Process "show ip dvmrp route [<address>]" command.
 */
void dvmrp_show_route (parseinfo *csb)
{

    dvmrptype  *route;
    idbtype    *idb;
    ipaddrtype address;
    boolean    address_supplied;

    if (!DVMRP_ROUTES) return;

    address = GETOBJ(paddr,1)->ip_addr;
    address_supplied = GETOBJ(int,1);
    idb = GETOBJ(idb,1);
    automore_enable(dvmrp_banner);
    printf(" - %d entries", dvmrp_cache_count);

    /*
     * Display entire table.
     */
    if (!address_supplied) {
    	rn_walktree_blocking(dvmrp_cache->rnh_treetop, dvmrp_show_one_route, 
			     idb);
    } else {

	/*
	 * Display single route. Do longest match lookup.
	 */
	route = dvmrp_get_route(address, 0, TRUE);
	if (route) {	
	    dvmrp_show_one_route((rntype *) route, NULL);
	} else {
	    printf("\nRoute not found");
	}
    }
    automore_disable();
}

/*
 * dvmrp_show_one_route
 *
 * Display one DVMRP route.
 */
int dvmrp_show_one_route (rntype *node, va_list args)
{
    
    dvmrptype *route;
    idbtype   *idb;
    uchar     uptime[20], expire[20];

    route = (dvmrptype *) node;
    idb = (args) ? va_arg(args, idbtype *) : NULL;

    /*
     * If we are looking at routes that have a specified next-hop
     * interface/tunnel, only display them.
     */
    if (idb && route->idb != idb) return(0);

    mem_lock(route);
    printf("\n%i/%d [%d/%d]", ip_radix2addr(route->dest), 
	   ip_bitsinmask(ip_radix2addr(route->mask)), route->distance, 
	   route->metric);

    /*
     * Display uptime and expire time.
     */
    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(route->uptime));
    if (!TIMER_RUNNING_AND_AWAKE(route->expires)) {
	sprint_ticks_in_dhms(expire, TIMER_RUNNING(route->expires) ?
			    TIME_LEFT_SLEEPING(route->expires) : 0);
    } else {
	sprintf(expire, "now");
    }
    printf(" uptime %s, expires %s", uptime, expire);

    /*
     * Display next-hop info.
     */
    printf("\n    via %i, %s", route->next_hop, route->idb->namestring);
    if (DVMRP_ACTIVE(route->idb) && route->idb->dvmrp_nbr_version) {
	printf(",");
	dvmrp_print_version(route->idb->dvmrp_nbr_version, TRUE);
    }

    free(route);
    return(automore_quit());
}


/*
 * dvmrp_accept_filter_command
 *
 * Process interface subcommand.
 * "ip dvmrp accept-filter <access-list> [neighbor-list <acl>] [<distance>]" 
 */
void dvmrp_accept_filter_command (parseinfo *csb)
{

    dvmrp_filter *filter;
    idbtype      *idb;
    ulong        distance, i;
    acl_headertype *acl, *nbr_acl;
    
    idb = csb->interface;
    distance = GETOBJ(int,3);
    filter = NULL;
    acl = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1), GETOBJ(int,1) == 0,
			   ACL_IP_NAMED_SIMPLE);
    if (!acl)
	return;
    if (GETOBJ(int,2) || *GETOBJ(string,2)) {
	nbr_acl = access_parse_acl(GETOBJ(int,2), GETOBJ(string,2),
				   GETOBJ(int,2) == 0, ACL_IP_NAMED_SIMPLE);
	if (!nbr_acl)
	    return;
    } else
	nbr_acl = NULL;
    
    /*
     * Find entry.
     */
    for (i = 0; i < dvmrp_filter_count; i++) {
	filter = &dvmrp_filters[i];
	if (filter->idb != idb) continue;
	if (filter->acl != acl) continue;
	break;
    }
    if (i == dvmrp_filter_count) filter = NULL;

    if (csb->sense) {
	if (!filter) {
	    if (dvmrp_filter_count == DVMRP_MAX_FILTER) {
		printf("\nMax number of DVMRP filters have been configured");
		return;
	    }
	    filter = &dvmrp_filters[dvmrp_filter_count];
	    dvmrp_filter_count++;
	}
	filter->acl = acl;
	filter->nbr_acl = nbr_acl;
	filter->idb = idb;
	filter->distance = distance;
    } else {
	if (!filter) {
	    printf("\nFilter not found");
	    return;
	}

	/*
	 * Move each element up in the array to maintain order.
	 */
	dvmrp_filter_count--;
	for ( ; i < dvmrp_filter_count; i++) {
	    dvmrp_filters[i] = dvmrp_filters[i+1];
        }
    }
}

/*
 * dvmrp_accept_filter
 *
 * Process destination advertised in DVMRP Report packet.
 */
boolean dvmrp_accept_filter (ipaddrtype address, ipaddrtype nbr, idbtype *idb,
			     ulong *distance, boolean nbr_only)
{

    dvmrp_filter *filter;
    int          i;
    boolean      no_idb_filters;

    /*
     * If there are no accept-filters configured, accept everything with
     * distance from "ip dvmrp distance" command.
     */
    if (distance) *distance = dvmrp_distance;
    if (!dvmrp_filter_count) return(TRUE);

    no_idb_filters = TRUE;
    for (i = 0; i < dvmrp_filter_count; i++) {
	filter = &dvmrp_filters[i];
	if (filter->idb != idb) continue;

	/*
	 * Found access-list for interface. If a user wants to filter all
	 * DVMRP destinations in Reports, it supplies a access-list of 0
	 * for the destinations. The code below does the right thing without
	 * interrogating address.
	 */
	no_idb_filters = FALSE;
	if (nfast_check(filter->nbr_acl, nbr)) {
	    if (nbr_only || nfast_check(filter->acl, address)) {
		if (distance) *distance = filter->distance;
		return(TRUE);
	    }
	}
    }
    return(no_idb_filters);
}

/*
 * dvmrp_auto_summary_command
 *
 * Process the "[no] ip dvmrp auto-summary" interface subcommand.
 */
void dvmrp_auto_summary_command (parseinfo *csb)
{
    idbtype *idb;

    idb = csb->interface;

    /*
     * DVMRP auto-summary is on by default.
     */
    if (csb->nvgen) {
	if (!idb->dvmrp_auto_summary) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	}
	return;
    }
    idb->dvmrp_auto_summary = csb->sense;
}

/*
 * dvmrp_summary_address_command
 *
 * Process the command:
 *
 * [no] ip dvmrp summary-address <address> <mask> metric <metric>
 */
void dvmrp_summary_address_command (parseinfo *csb)
{
    idbtype       *idb;
    dvmrp_sumtype *sum;
    ipaddrtype    addr, mask;
    ulong         metric;

    idb = csb->interface;
    
    /*
     * NV generation.
     */	
    if (csb->nvgen) {
	for (sum = dvmrp_summaryQ.qhead; sum; sum = sum->next) {
	    if (sum->idb != idb) continue;
	    nv_write(TRUE, "%s %i %i", csb->nv_command, sum->address, 
		     sum->mask);
	    if (sum->metric != DVMRP_DEFAULT_SUMMARY_METRIC) {
		nv_add(TRUE, " metric %d", sum->metric);
	    }
	}
	return;
    }

    addr = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    metric = GETOBJ(int,1);

    /*
     * Find summary address info for interface.
     */
    for (sum = dvmrp_summaryQ.qhead; sum; sum = sum->next) {
	if (sum->idb != idb) continue;
	if (sum->address != addr) continue;

	if (sum->mask != mask) continue;
	break;
    }

    if (csb->sense) {
	if (sum) return;

	/*
	 * Validate address and mask.
	 */
	if (IPMULTICAST(addr)) {
	    printf("\nIllegal address");
	    return;
	}
	if (!mask_contiguous(mask)) {
	    printf("\nIllegal netmask");
	    return;
	}
	if (mask < CLASS_A_MASK) {
	    printf("\nDVMRP doesn't support masks less than /8");
	    return;
	}
	sum = malloc(sizeof(dvmrp_sumtype));
	if (!sum) return;
	sum->idb = idb;
	sum->address = addr;
	sum->mask = mask;
	sum->metric = metric;
	enqueue(&dvmrp_summaryQ, sum);
    } else {
	if (!sum) return;
	unqueue(&dvmrp_summaryQ, sum);
	free(sum);
    }
}

/*
 * dvmrp_get_tunnel
 *
 * We received a DVMRP packet unicast to us. Find tunnel idb that corresponds
 * to the source and destination IP addresses in header.
 */
idbtype *dvmrp_get_tunnel(iphdrtype *ip)
{
    tunnel_info *tinfo;
    idbtype     *idb;

    idb = NULL;

    FOR_ALL_TUNNELS(tinfo) {
	if (tinfo->mode == TUN_MODE_DVMRP_IP && 
	    ip->srcadr == tinfo->destination && 
	    tinfo->hwidb->state == IDBS_UP) {
	    idb = tinfo->hwidb->firstsw;
	    break;
	}
    }
    return(idb);
}

/*
 * dvmrp_get_tunnel_source
 *
 * Get source IP address assigned to tunnel. If source address is based on 
 * an idb, use IP address associated with idb for source. If that idb is
 * unnumbered, use the IP address of the interface it points to.
 */
ipaddrtype dvmrp_get_tunnel_source (idbtype *idb)
{

    ipaddrtype source;

    source = idb->hwptr->tunnel->source;
    if (!source && idb->hwptr->tunnel->source_idb) {
	source = idb->hwptr->tunnel->source_idb->ip_address;
	if (!source && idb->hwptr->tunnel->source_idb->ip_unnumbered) {
	    source = idb->hwptr->tunnel->source_idb->ip_unnumbered->ip_address;
	}
    }
    return(source);
}

/*
 * dvmrp_tunnel_accounting
 *
 * The caller is responsible for passing valid tunnel idb, i.e. call
 * dvmrp_get_tunnel() to get the tunnel idb before calling this function.
 * The caller is also responsible for clearing old input queueing accounting.
 */
void dvmrp_tunnel_accounting (paktype *pak, idbtype *idb)
{
    idb->hwptr->counters.inputs++;
    idb->hwptr->counters.rx_cumbytes += pak->datagramsize;
    GET_TIMESTAMP(idb->hwptr->lastinput);
}

/*
 * dvmrp_send_on_tunnel
 *
 * Send packet over DVMRP tunnel. This consists of sending an IP packet with
 * a single header. The source and destination addresses are the ones 
 * configured for the tunnel.
 */
void dvmrp_send_on_tunnel (paktype *pak, ulong size, idbtype *idb)
{

    ipaddrtype source, dest;

    /*
     * Get source and destination tunnel addresses.
     */
    source = dvmrp_get_tunnel_source(idb);
    dest = idb->hwptr->tunnel->destination;

    igmprouteparams.ttl = TTL_DEFAULT;
    ipwrite(pak, size, IGMP_PROT, source, dest, NULL, NULL, &igmprouteparams);
    igmprouteparams.ttl = 1;
}

/*
 * dvmrp_receive_ask_nbr
 *
 * Process ASK_NBR or ASK_NBR2 DVMRP packet. This is sent when a UNIX machine
 * does a mrinfo. 
 */
void dvmrp_receive_ask_nbr (ipaddrtype source, ipaddrtype myaddr, 
			    idbtype *if_input, ulong length, boolean use_flags)
{

    idbtype          *idb;
    dvmrp_asknbrtype *dvmrp;
    char             *pkt_ptr, found_flags = 0;
    paktype          *pak;
    igmptype         *igmp;
    ipaddrtype       *address;
    ulong            size, entry_size;
    boolean          dvmrp_tunnel;
    uchar            *count_ptr, *flags_ptr;
    pim_nbrtype      *nbr;
    list_element     *l_elt;
    
    DVMRP_BUGINF("\nDVMRP: Received Ask-Nbr%srequest from %i on %s",
		 (use_flags) ? "2 " : " ", source, if_input->namestring);

    /*
     * Packet too short.
     */
    if (length < sizeof(igmptype)) {
	DVMRP_BUGINF("\nDVMRP: Received packet too short");
	return;
    }

    /*
     * Get buffer for reply. Fill in fixed IGMP fields.
     */
    pak = dvmrp_getbuffer(&igmp, &pkt_ptr, &size);
    if (!pak) return;
    igmp->code = (use_flags) ? DVMRP_NBR2_CODE : DVMRP_NBR_CODE;
    igmp->address |= DVMRP_AUTORP_CAPABLE;
    dvmrp = (dvmrp_asknbrtype *) pkt_ptr;

    /*
     * Include all multicast interfaces. If the neigbhor is not known or
     * there are many PIM and/or DVMRP neighbors on a single interface, 
     * return 0.0.0.0.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->ip_mrouting) continue;
	dvmrp_tunnel = DVMRP_TUNNEL(idb);

 	entry_size = DVMRP_ASKNBR_SIZE;
 	if (use_flags) entry_size++;

	if (dvmrp_tunnel) {
 	    PUTLONG(&dvmrp->local_address, dvmrp_get_tunnel_source(idb)); 
	} else {
 	    PUTLONG(&dvmrp->local_address, idb->ip_address);
	    if (!idb->ip_address && idb->ip_unnumbered) {
 	        PUTLONG(&dvmrp->local_address, 
                          idb->ip_unnumbered->ip_address);
	    }
	}
	
	dvmrp->metric = idb->dvmrp_metric_offset_in;
	dvmrp->threshold = idb->ip_multicast_threshold;

	/*
	 * Insert flags if request was a ASK_NBR2. Get neighbor count pointer
	 * and first neighbor address pointer.
 	 * If the the interface is a GRE tunnel then both the DVMRP_TUNNEL
 	 * and the PIM_FLAG are set.
 	 * If the interface has no neighbors of any type it is marked as a 
 	 * DVMRP_LEAF.
         * If a DVMRP tunnel is not receiving probes from its neighbor, the
         * interface is marked DVMRP_DOWN_FLAG.
	 */
	if (use_flags) {
	    flags_ptr = &dvmrp->count_flags;
	    *flags_ptr = 0;
	    if (dvmrp_tunnel) {
 	        *flags_ptr |= DVMRP_TUNNEL_FLAG;
 	    } else {
 	        *flags_ptr |= DVMRP_PIM_FLAG;
 		if (is_tunnel(idb->hwptr)) {
 		    *flags_ptr |= DVMRP_TUNNEL_FLAG;
 		}
 	    }
	    if (!interface_up(idb) || (dvmrp_tunnel && !DVMRP_ACTIVE(idb))) {
                *flags_ptr |= DVMRP_DOWN_FLAG;
            }
	    if (interface_admin_down(idb)) *flags_ptr |= DVMRP_DISABLE_FLAG;
	    if (idb->ip_enabled) {
		if (idb->ip_address == idb->pim_dr) {
		    *flags_ptr |= DVMRP_QUERIER_FLAG;
		}
		if (!idb->ip_address && idb->ip_unnumbered &&
		    idb->ip_unnumbered->ip_address == idb->pim_dr) {
		    *flags_ptr |= DVMRP_QUERIER_FLAG;
		}
	    } else {
		*flags_ptr |= DVMRP_DISABLE_FLAG;
	    }
	    if (!idb->pim_nbrs && !DVMRP_ACTIVE(idb)) {
	        *flags_ptr |= DVMRP_LEAF;
	    }
	    count_ptr = dvmrp->data;
	    address = (ipaddrtype *) (dvmrp->data + 1);
	} else {
	    flags_ptr = NULL;
	    count_ptr = &dvmrp->count_flags;
	    address = (ipaddrtype *) dvmrp->data;
	}

	/*
 	 * Set the neighbor count and the list of neighbors.
 	 * If there are no neighbors set the neighbor count to 1 and
 	 * return 0.0.0.0 so mrinfo can still decode the interface flags.
	 */
 	if (dvmrp_tunnel) {
            PUTLONG(address, ((tunnel_info *) 
			      idb->hwptr->tunnel)->destination);
 	    *count_ptr = 1;
 	} else if (idb->pim_nbrs) {
 	    *count_ptr = 0;
 	    entry_size -= sizeof(ipaddrtype);
 	    for (nbr = (pim_nbrtype *) idb->pim_nbrs; nbr; nbr = nbr->next) {
 	        PUTLONG(address, nbr->nbr_address);
 		address++;
 		(*count_ptr)++;
 		entry_size += sizeof(ipaddrtype);
 
                /*
 		 * If the packet is too large, store the flag information for
 		 * the interface then send this packet off, start another.
 		 */
 		if ((size + entry_size + sizeof(ipaddrtype)) > 
		                                      DVMRP_MAX_SIZE) {
 		    if (use_flags) found_flags = *flags_ptr;
 		    size += entry_size;
 		    igmp->checksum = ipcrc((ushort *) igmp, 
 					   size - IPHEADERBYTES(NOPT));
 		    igmprouteparams.ttl = TTL_DEFAULT;
 		    ipwrite(pak, size, IGMP_PROT, myaddr, source, NULL, NULL, 
 			    &igmprouteparams);
 		    igmprouteparams.ttl = 1;
 
 		    /* 
 		     * Get a new packet.
 		     */
 		    pak = dvmrp_getbuffer(&igmp, &pkt_ptr, &size);
 		    if (!pak) return;
 		    igmp->code = (use_flags) ? DVMRP_NBR2_CODE : 
		                                           DVMRP_NBR_CODE;
 		    dvmrp = (dvmrp_asknbrtype *) pkt_ptr;
 		    entry_size = DVMRP_ASKNBR_SIZE;
 
 		    /* 
 		     * refill all the ask_neighbor fields.
 		     */
 		    if (dvmrp_tunnel) {
 		        PUTLONG(&dvmrp->local_address, 
 				dvmrp_get_tunnel_source(idb)); 
 		    } else {
 		        PUTLONG(&dvmrp->local_address, idb->ip_address);
 			if (!idb->ip_address && idb->ip_unnumbered) {
 			    PUTLONG(&dvmrp->local_address, 
 				    idb->ip_unnumbered->ip_address);
 			}
 		    }
 
 		    dvmrp->metric = idb->dvmrp_metric_offset_in;
 		    dvmrp->threshold = idb->ip_multicast_threshold;
 
 		    /*
 		     * Get the neighbor count and the flags and the neighbor
 		     * list pointers pointing to the right locations.
 		     */
 		    if (use_flags) {
 		        flags_ptr = &dvmrp->count_flags;
 		        *flags_ptr = found_flags;
 			count_ptr = dvmrp->data;
 		        address = (ipaddrtype *) (dvmrp->data + 1);
 			entry_size++;
 		    } else {
 		        count_ptr = &dvmrp->count_flags;
 		        address = (ipaddrtype *) dvmrp->data;
 		    }
 		    *count_ptr = 0;
 		}
 	    }
         } else {
 	    PUTLONG(address, 0);
 	    *count_ptr = 1;
 	}

	/*
	 * Adjust current size of packet and move pointer for the next
 	 * interface.  If the packet won't hold the next interface, send
 	 * it off and get a new buffer.
	 */
	size += entry_size;
	dvmrp = (dvmrp_asknbrtype *) (((uchar *) dvmrp) + entry_size);
	
 	if ((size + DVMRP_ASKNBR_SIZE) > DVMRP_MAX_SIZE) {
 	    igmp->checksum = ipcrc((ushort *) igmp, 
 					   size - IPHEADERBYTES(NOPT));
 	    igmprouteparams.ttl = TTL_DEFAULT;
 	    ipwrite(pak, size, IGMP_PROT, myaddr, source, NULL, NULL, 
 			    &igmprouteparams);
 	    igmprouteparams.ttl = 1;
 
 	    pak = dvmrp_getbuffer(&igmp, &pkt_ptr, &size);
 	    if (!pak) return;
 	    igmp->code = (use_flags) ? DVMRP_NBR2_CODE : DVMRP_NBR_CODE;
 	    dvmrp = (dvmrp_asknbrtype *) pkt_ptr;
         }
    }

    /*
     * Checksum and send out.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    DVMRP_BUGINF("\nDVMRP: Send Ask-Nbr%sresponse to %i",
		 (use_flags) ? "2 " : " ", source);

    igmprouteparams.ttl = TTL_DEFAULT;
    ipwrite(pak, size, IGMP_PROT, myaddr, source, NULL, NULL, 
	    &igmprouteparams);
    igmprouteparams.ttl = 1;
    igmp_traffic.output_dvmrp++;
}

/* 
 * dvmrp_send_asknbr2_request
 *
 * Send a request for an mrinfo response to router specified by dest.
 * Called by mrinfo and mtrace.
 *
 */
void dvmrp_send_asknbr2_request (ipaddrtype dest, ipaddrtype source)
{
  
    paktype  *outpak;
    igmptype *igmp;
    ulong    size;

    outpak = getbuffer(DVMRP_MAX_SIZE);
    if (!outpak) return;

    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    igmp = (igmptype *) (ipheadstart(outpak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_DVMRP_TYPE;
    igmp->code = DVMRP_ASK_NBR2_CODE;
    igmp->checksum = 0;
    igmp->address = dvmrp_cisco_version();
  
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    if (source) {
	DVMRP_BUGINF("\nDVMRP: Send Ask-Nbr2 request to %i sourced from %i",
		     dest, source);
    } else {
	DVMRP_BUGINF("\nDVMRP: Send Ask-Nbr2 request to %i", dest);
    }
    
    ipwrite(outpak, size, IGMP_PROT, source, dest, NULL, NULL,
	    &mrinfodefaultparams);
    igmp_traffic.output_dvmrp++;
}

/*
 * dvmrp_receive_nbr2
 *
 * Process incoming DVMRP Nbr2 response packet. These should be in response
 * to mrinfo and mtrace requests. Packet is matched by source address, there
 * are no provisions for query id's in dvmrp_asknbr headers. (Sigh).
 */
void dvmrp_receive_nbr2 (igmptype *igmp, paktype *pak, int length)
{
    iphdrtype        *ip;
    mrinfoQtype      *asknbr; 
    dvmrp_asknbrtype *dvmrp;
    nametype         *name;
    addrtype         addr;

    ip = (iphdrtype *) ipheadstart(pak);
  
    DVMRP_BUGINF("\nDVMRP: Received Ask-Nbr2 response from %i (%s)", 
		 ip->srcadr, pak->if_input->namestring);
  
    if (length < sizeof(igmptype)) {
	DVMRP_BUGINF("\nDVMRP: Packet too short");
	return;
    }
  
    dvmrp = (dvmrp_asknbrtype *) igmp->data;
  
    name = reg_invoke_ip_address_lookup(ip->srcadr);

    for (asknbr = (mrinfoQtype *) ip_mrinfoQ.qhead; asknbr; 
	 asknbr = asknbr -> next) {

	/*
	 * Since the response might come from a different interface
	 * but the same machine, check to see if the ip address
	 * is in the name cache for that machine 
	 */
	addr.addr.ip_address = asknbr->id;
	if (name) {
	    if (!reg_invoke_name_match_number(ADDR_IP, name, &addr)) continue;
	}	
	else {
	    if (ip->srcadr != asknbr->id) continue;
	}
	
	enqueue(&asknbr->data, pak);
	GET_TIMESTAMP(pak->gp_timer);
	break;
    }
  
    if (!asknbr) datagram_done(pak);
}

/* 
 * mrinfo_process_asknbr2_response
 *
 * Print out information about neighbor from nbr2 packet. Displays all 
 * neighbor router with flags indicating tunnel status, protocol version, and 
 * leaf status.
 */
void mrinfo_process_asknbr2_response (paktype *pak, ipaddrtype router)
{

    iphdrtype *ip;
    igmptype  *igmp;
    uchar     *data, *eod;
    nametype  *name;
    ulong     version;
    int       length;
 
    /* 
     * Grab the relevant portions off the packet 
     */
    ip = (iphdrtype *) ipheadstart(pak);
    igmp = (igmptype *) ipdatastart(pak);
    data = igmp->data;
  
    length = ip->tl - (ip->ihl << 2) - sizeof(igmptype);

    eod = data + length;

    automore_enable(NULL);
 
    version = igmp->address;
    dvmrp_print_version(version, TRUE);
    printf(":\n");

    while (data < eod) {
	uchar metric;
	uchar threshold;
	uchar flags;
	int nbr_count;
	ipaddrtype l_addr;
    
	l_addr = *(ipaddrtype *) data;
    
	data += 4;
	metric = *data++;
	threshold = *data++;
	flags = *data++;
	nbr_count = *data++;
    
	while (--nbr_count >= 0) {
	    ulong neighbor = *(ulong *) data;
	    data += 4;
      
	    printf("  %i -> ", l_addr);
      
	    if ((name = reg_invoke_ip_address_lookup(neighbor))) {
		printf("%i (%s) [%d/%d", neighbor, name->name, metric, 
		       threshold);
	    } else {
		printf("%i [%d/%d",neighbor,metric, threshold);
	    }
      
	    if (flags & DVMRP_TUNNEL_FLAG) printf("/tunnel");
	    if (flags & DVMRP_TUNNEL_SR) printf("/srcrt");
	    if (flags & DVMRP_PIM_FLAG)	printf("/pim");
	    if (flags & DVMRP_QUERIER_FLAG) printf("/querier");
	    if (flags & DVMRP_DISABLE_FLAG) printf("/disabled");
	    if (flags & DVMRP_DOWN_FLAG) printf("/down");
	    if (flags & DVMRP_LEAF) printf("/leaf");
	    printf("]\n");
	}
    }
    automore_disable();
}

/*
 * dvmrp_prune_time -
 * 
 * Calculate the DVMRP prune time for the prune. Find the shortest 
 * among all outgoing prune timers.
 */
ulong dvmrp_prune_time (mdbtype *mdb)
{
    midbtype	*midb;
    ulong	timer;

    /*
     * Start with the default prune time and find the lowest expiration
     * time for all OIFs.
     */
    timer = DVMRP_PRUNE_TIME;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb;
	 midb = midb->next) {
	if (TIME_LEFT_SLEEPING(midb->expires) < timer) {
	    timer = TIME_LEFT_SLEEPING(midb->expires);
	}
    }
    
    /*
     * A default minimum in case timers are messed up.
     */
    if (timer == 0) {
	timer = DVMRP_MIN_PRUNE_TIME;
    }

    /*
     * The prune needs the prune time to be set in units of seconds.
     */
    timer = timer / ONESEC;
    
    return(timer);
}

/*
 * dvmrp_send_prune
 *
 * Send DVMRP Prune message for a given {source,group} pair.
 */
void dvmrp_send_prune (mdbtype *mdb, idbtype *idb)
{

    paktype         *pak;
    igmptype        *igmp;
    dvmrp_prunetype *prune;
    ulong           size;

    /*
     * Don't send Prune if source is directly connected.
     */
    if (!mdb->rpf_nbr) {
	if (samecable(ip_radix2addr(mdb->source))) return;
    }

    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(dvmrp_prunetype);
    pak = getbuffer(size);
    if (!pak) return;

    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_DVMRP_TYPE;
    igmp->code = DVMRP_PRUNE_CODE;
    igmp->checksum = 0;
    igmp->address = dvmrp_cisco_version();
    prune = (dvmrp_prunetype *) igmp->data;
    prune->source = ip_radix2addr(mdb->source);
    prune->group = mdb->group;

    /*
     * Calculate the prune time for the DVMRP prune.
     */
    prune->prune_timer = dvmrp_prune_time(mdb);

    /*
     * Compute checksum.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));
    if (!idb) idb = mdb->if_input;

    DVMRP_PRUNING_BUGINF("\nDVMRP: Send Prune for (%i, %i) on %s to ", 
			 prune->source, prune->group, idb->namestring);

    /*
     * Send to IP.
     */
    if (DVMRP_TUNNEL(idb)) {
	DVMRP_PRUNING_BUGINF("%i", idb->hwptr->tunnel->destination);
	dvmrp_send_on_tunnel(pak, size, idb);
    } else {
	DVMRP_PRUNING_BUGINF("%i", mdb->rpf_nbr);
	ipwrite(pak, size, IGMP_PROT, idb->ip_address, mdb->rpf_nbr, idb, 
		NULL, &igmprouteparams);
    }
    igmp_traffic.output_dvmrp++;
}

/*
 * dvmrp_send_graft
 *
 * Send DVMRP Graft.
 */
void dvmrp_send_graft (gdbtype *gdb, mdbtype *mdb)
{

    paktype         *pak;
    igmptype        *igmp;
    dvmrp_grafttype *graft;
    idbtype         *idb;
    ulong           size;
    ipaddrtype      nbr;

    /*
     * If DVMRP tunnel and neighbor doesn't do pruning, it won't accept
     * Grafts. So don't send them.
     */
    if (DVMRP_TUNNEL(mdb->if_input) && 	
	!DVMRP_PRUNER(mdb->if_input, mdb->if_input->dvmrp_nbr_version)) return;

    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(dvmrp_grafttype);
    pak = getbuffer(size);
    if (!pak) return;

    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_DVMRP_TYPE;
    igmp->code = DVMRP_GRAFT_CODE;
    igmp->checksum = 0;
    igmp->address = dvmrp_cisco_version();
    graft = (dvmrp_grafttype *) igmp->data;
    graft->source = ip_radix2addr(mdb->source);
    graft->group = mdb->group;

    /*
     * Compute checksum.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    idb = mdb->if_input;
    nbr = mdb->rpf_nbr;

    DVMRP_PRUNING_BUGINF("\nDVMRP: Send Graft for (%i, %i) on %s to %i", 
			 graft->source, graft->group, idb->namestring, nbr);

    /*
     * Send to IP.
     */
    if (DVMRP_TUNNEL(idb)) {
	dvmrp_send_on_tunnel(pak, size, idb);
    } else {
	ipwrite(pak, size, IGMP_PROT, idb->ip_address, nbr, idb, NULL, 
		&igmprouteparams);
    }
    igmp_traffic.output_dvmrp++;

    /*
     * Set the retransmission timer (for the group).
     */
    mdb->send_graft = TRUE;
    TIMER_START(gdb->send_graft_time, DVMRP_GRAFT_FREQUENCY);
}

/*
 * dvmrp_send_graft_ack
 *
 * Send DVMRP Graft-Ack to originator of Graft.
 */
void dvmrp_send_graft_ack (ipaddrtype nbr, idbtype *idb, ipaddrtype group,
			   ipaddrtype source)
{
    paktype         *pak;
    igmptype        *igmp;
    dvmrp_grafttype *gack;
    ulong           size;

    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(dvmrp_grafttype);
    pak = getbuffer(size);
    if (!pak) return;

    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_DVMRP_TYPE;
    igmp->code = DVMRP_GRAFT_ACK_CODE;
    igmp->checksum = 0;
    igmp->address = dvmrp_cisco_version();
    gack = (dvmrp_grafttype *) igmp->data;
    gack->source = source;
    gack->group = group;

    /*
     * Compute checksum.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    DVMRP_PRUNING_BUGINF("\nDVMRP: Send Graft-Ack for (%i, %i) on %s to %i", 
			 gack->source, gack->group, idb->namestring, nbr);

    /*
     * Send to IP.
     */
    if (DVMRP_TUNNEL(idb)) {
	dvmrp_send_on_tunnel(pak, size, idb);
    } else {
	ipwrite(pak, size, IGMP_PROT, idb->ip_address, nbr, idb, NULL, 
		&igmprouteparams);
    }
    igmp_traffic.output_dvmrp++;
}

/*
 * dvmrp_print_version
 *
 * Print version information from an DVMRP version field.
 */
void dvmrp_print_version (ulong version, boolean print_flags)
{
    printf(" [version %s %d.%d]", (version == 1) ? "proteon/mrouted" :
	   ((version & 0xff) == 2) ? "mrouted" :
	   ((version & 0xff) == 3) ? "mrouted" :
	   ((version & 0xff) == 4) ? "mrouted" :
	   ((version & 0xff) == 10) ? "cisco" : 
	   ((version & 0xff) == 11) ? "cisco" : "",
	   version & 0xff, (version >> 8) & 0xff);

    if (print_flags) {
	printf(" [flags: ");
	if (version & DVMRP_LEAF_CAPABLE) printf("L");
	if (version & DVMRP_GENID_CAPABLE) printf("G");
	if (version & DVMRP_PRUNE_CAPABLE) printf("P");
	if (version & DVMRP_MTRACE_CAPABLE) printf("M");
	if (version & DVMRP_SNMP_CAPABLE) printf("S");
	if (version & DVMRP_AUTORP_CAPABLE) printf("A");
	printf("]");
    }
}

/*
 * dvmrp_route_complaint
 *
 * Originate log message if the number of DVMRP routes have surged. This
 * is provided to tell net adminstrators that something has gone severely
 * wrong on the MBONE.
 */
void dvmrp_route_complaint (idbtype *idb)
{

    char buf[80];
    
    /*
     * Only report for DVMRP tunnels.
     */
    if (!DVMRP_TUNNEL(idb)) return;

    if (idb->dvmrp_routes_rcvd >= dvmrp_routehog_count) {
	sprint_ticks_in_dhms(buf, ELAPSED_TIME(idb->dvmrp_routes_rcvd_timer));
	errmsg(&msgsym(ROUTEHOG, DVMRP), idb->dvmrp_routes_rcvd,
	       idb->hwptr->tunnel->destination, idb->namestring, buf);
    }
}

/*
 * dvmrp_get_mask
 *
 * Determine what mask should be used to process a DVMRP Prune or Graft
 * message. If the RPF lookup returned a unicast route that is variably
 * subnetted, used the shortest mask (not the longest, which is used in
 * ip_rpf_lookup()). If the RPF lookup returned a DVMRP route or static
 * mroute, use the mask associated with it.
 */
static ipaddrtype dvmrp_get_mask (ipaddrtype source, uchar rpf_type, ipaddrtype mask)
{

    ndbtype *ndb;

    /*
     * Only consider RPF lookups from unicast routing table.
     */
    if (rpf_type != RPF_UNICAST) return(mask);

    /*
     * If network number is not found, something is really screwed up.
     */
    ndb = net_lookup(source, FALSE);
    if (!ndb) return(mask);

    /*
     * If there is only a single mask, there is no confusion.
     */
    if (ndb->maskcount == 1 || !ndb->masktable) return(mask);

    /*
     * Return shortest mask. It is last element of mask array.
     */
    mask =  ndb->masktable[ndb->maskcount-1].mask;
    if (mask > get_majormask(ndb->netnumber)) {
	mask = get_majormask(ndb->netnumber);
    }
    return(mask);
}

/*
 * dvmrp_lookup_source
 *
 * Find a source(s) that is being pruned or grafted from the a DVMRP
 * message.
 */
static void dvmrp_lookup_source (idbtype *idb, ipaddrtype msg_source, 
			  ipaddrtype *source, ipaddrtype *mask)
{
    uchar rpf_type;

    /*
     * If we are originating the DVMRP default route and the prune source is
     * 0.0.0.0, no need to do an RPF lookup, the default route might not
     * be in any routing table. We want to prune all active (S,G)s.
     */
    if (!msg_source && idb->dvmrp_send_default) {
	*source = 0;
	*mask = 0;
	DVMRP_BUGINF("\n       No RPF lookup, using default route");
    } else {

	/*
	 * Position radix tree at subnet aggregate. Then process each more 
	 * specific route subordinate to the subnet route. DVMRP prunes to 
         * subnets and does not supply a subnet mask, ugh. Get mask by doing 
	 * an RPF lookup. Since the downstream router sent us a prune, we are 
	 * its upstream RPF neighbor and therefore advertised the route. So 
	 * we have the subnet mask, just got to get it.
	 */
	if (!ip_rpf_lookup(msg_source, MASK_HOST, NULL, mask, NULL, 
			   &rpf_type, ROUTINGDEPTH)) return;

	DVMRP_BUGINF("\n       RPF lookup mask %i, RPF type %s", *mask,
		     (rpf_type == RPF_UNICAST) ? "unicast" : 
		     (rpf_type == RPF_DVMRP) ? "dvmrp" : "static");

	/*
	 * Might need shortest mask if a VLSM unicast route exists.
	 */
	*mask = dvmrp_get_mask(msg_source, rpf_type, *mask);
	*source = msg_source & *mask;
    }

    DVMRP_BUGINF("\n       Using %i/%i for multicast search", *source, *mask);
}

/*
 * dvmrp_receive_prune
 *
 * Process received DVMRP Prune message.
 */
void dvmrp_receive_prune (paktype *pak, ulong length, ipaddrtype nbr,
			  idbtype *idb, igmptype *igmp)
{

    dvmrp_prunetype *prune;
    gdbtype         *gdb;
    mdbtype         *mdb;
    midbtype        *midb;
    ipaddrtype      source, mask;

    DVMRP_PRUNING_BUGINF("\nDVMRP: Received Prune on %s from %i", 
			 idb->namestring, nbr);

    if (length < (IGMPHEADERBYTES + sizeof(dvmrp_prunetype))) {
	DVMRP_PRUNING_BUGINF("\nDVMRP: Packet too short");
	return;
    }

    /*
     * If this interface has PIM neighbors, don't Prune. If running 
     * sparse-mode there is an explicit downstream PIM neighbor. If running
     * dense-mode, there may be other PIM neighbors that want packet.
     */
    if (idb->pim_nbrs) {
	DVMRP_PRUNING_BUGINF("\nDVMRP: Prune ignored, PIM neighbors exist");
	return;
    }

    /*
     * Only process Prunes on point-to-point links. We don't do it on LANs
     * because we don't want to keep neighbor state per group. This may
     * change in the future if we are pushed hard enough to do it.
     */
    if (!is_p2p(idb)) return;

    /*
     * Point to prune data in message. If group lookup fails, don't create
     * state.
     */
    prune = (dvmrp_prunetype *) igmp->data;
    DVMRP_PRUNING_BUGINF("\n       for (%i, %i), prune-timer %d secs", 
			 prune->source, prune->group, prune->prune_timer);

    gdb = ip_get_gdb(prune->group);
    if (!gdb) return;

    dvmrp_lookup_source(idb, prune->source, &source, &mask);

    /*
     * Process each (S,G) that is a more specific route than prune->source/
     * mask.
     */
    mdb = ip_get_worst_mdb(gdb, source, mask);
    while (mdb) {

	/*
	 * Check to see if interface is in outgoing interface list. If 
	 * not or the interface is already pruned, just return.
	 */	
	midb = ip_get_midb(&mdb->if_outputQ, idb, 0, FALSE);
	if (midb && midb->state == MIDB_FORWARD_STATE) {
	    DVMRP_PRUNING_BUGINF("\nDVMRP: Process prune for (%i/%d, %i)", 
				 ip_radix2addr(mdb->source), 
				 ip_bitsinmask(ip_radix2addr(mdb->mask)), 
				 mdb->group);

	    /*
	     * Prune interface now.
	     */
	    pim_prune_interface(gdb, mdb, midb, prune->prune_timer * ONESEC);

	    /*
	     * If the prune timer in the DVMRP prune is larger than
	     * the default PIM-DM prune timer, change the mdb's expiration
	     * timer to match that.
	     */
	    ip_store_mdb_timer(mdb);
	    TIMER_START(mdb->expires, mdb->expiration_time);
	}
        mdb = ip_next_specific_mdb(gdb, ip_radix2addr(mdb->source), 
  			           prune->source, mask);	
    }
}

/*
 * dvmrp_receive_graft_or_ack
 *
 * Process received DVMRP Graft or Graft-Ack message.
 */
void dvmrp_receive_graft_or_ack (paktype *pak, ulong length, ipaddrtype nbr,
				 idbtype *idb, igmptype *igmp)
{

    dvmrp_grafttype *graft;
    gdbtype         *gdb;
    mdbtype         *mdb;
    ipaddrtype      source, mask;

    DVMRP_PRUNING_BUGINF("\nDVMRP: Received Graft%son %s from %i", 
			 (igmp->code == DVMRP_GRAFT_ACK_CODE) ? "-Ack " : " ", 
			 idb->namestring, nbr);

    if (length < (IGMPHEADERBYTES + sizeof(dvmrp_grafttype))) {
	DVMRP_PRUNING_BUGINF("\nDVMRP: Packet too short");
	return;
    }

    graft = (dvmrp_grafttype *) igmp->data;
    DVMRP_PRUNING_BUGINF("\n       for (%i, %i)", graft->source, graft->group);

    mdb = NULL;
    gdb = ip_get_gdb(graft->group);
    if (gdb) mdb = ip_get_best_mdb(gdb, graft->source);

    /*
     * Process Graft-Ack and return.
     */
    if (igmp->code == DVMRP_GRAFT_ACK_CODE) {
	if (mdb) mdb->send_graft = FALSE;
	return;
    }

    /*
     * Send DVMRP Graft-Ack now in response to received Graft.
     */
    dvmrp_send_graft_ack(nbr, idb, graft->group, graft->source);
    if (!gdb) return;

    dvmrp_lookup_source(idb, graft->source, &source, &mask);

    mdb = ip_get_worst_mdb(gdb, source, mask);
    while (mdb) {
	DVMRP_PRUNING_BUGINF("\nDVMRP: Process graft for (%i/%d, %i)", 
			     ip_radix2addr(mdb->source),
			     ip_bitsinmask(ip_radix2addr(mdb->mask)), 
			     mdb->group);

	pim_add_mroute(&gdb, &mdb, mdb->group, 0, MASK_HOST, idb, nbr, 
		       MIDB_FORWARD_STATE, MDB_EXPIRATION_TIME, FALSE);

	/*
	 * Abort if our mdb is wiped out, most likely from a multicast
	 * boundary, but possibly due to low memory.
	 */
	if (!mdb) {
	    return;
	}

	mdb = ip_next_specific_mdb(gdb, ip_radix2addr(mdb->source), 
				   graft->source, mask);	
    }
}
      
