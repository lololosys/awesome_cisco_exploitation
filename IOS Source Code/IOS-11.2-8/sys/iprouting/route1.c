/* $Id: route1.c,v 3.13.8.22 1996/09/06 13:27:53 fox Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route1.c,v $
 *------------------------------------------------------------------
 * route1.c -- Routing Support: Common Routines
 *
 * 22-March-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route1.c,v $
 * Revision 3.13.8.22  1996/09/06  13:27:53  fox
 * CSCdi50490:  neighbor route never installed
 * Branch: California_branch
 * Simplify neighbor route creation.  Try to add the route when IPCP or
 * SLIP are ready.  Also try to add the route if the interface comes up
 * while you know about the neighbor.
 *
 * Revision 3.13.8.21  1996/08/28  21:01:02  albright
 * CSCdi65258:  Timers basic is not working correctly under IGRP
 * Branch: California_branch
 *
 * Revision 3.13.8.20  1996/08/28  12:53:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.8.19  1996/08/06  22:21:47  gchristy
 * CSCdi65058:  IP Cache: excessive reaps with multi-path recursive routes
 * Branch: California_branch
 *  - When determining if a recursive cache entry is stale and the initial
 *    route lookup reveals multiple paths, check each one.
 *
 * Revision 3.13.8.18  1996/08/06  08:28:18  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.13.8.17  1996/08/03  23:30:40  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.13.8.16  1996/08/02  12:20:03  albright
 * CSCdi61734:  major net broadcast not forwarded by helper-address
 * Branch: California_branch
 *
 * Revision 3.13.8.15  1996/07/30  06:09:23  dawnli
 * CSCdi61869:  Wrong next-hop selection if set interface defined in
 * route-map.
 * Branch: California_branch
 * Define new route lookup routine based on a given interface.
 * Call it to set the right next-hop when route-map set interface
 * defined.
 *
 * Revision 3.13.8.14  1996/07/17  18:22:48  gchristy
 * CSCdi62877:  down interfaces or bogus static routes cause cache
 * thrashing
 * Branch: California_branch
 *  Only clean the cache if we actually deleted a route or it's in
 *  hold-down.
 *
 * Revision 3.13.8.13  1996/07/03  01:38:07  albright
 * CSCdi59425:  Incorrect entry of Major Net Summary RIP/IGRP w/ ip
 * unnumbered PPP
 * Branch: California_branch
 *
 * Revision 3.13.8.12  1996/06/28  23:19:36  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.13.8.11  1996/06/28  01:21:12  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.13.8.10  1996/06/24  16:05:34  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.13.8.9  1996/05/30  16:34:39  fox
 * CSCdi57995:  ipcp_down() removes routes from parallel non-ddr links
 * Branch: California_branch
 * Use IPADDR_ZERO appropriately.
 *
 * Revision 3.13.8.8  1996/05/22  17:19:13  rchandra
 * CSCdi57843:  BGP: Majornet route missing from IP routing table
 * Branch: California_branch
 * Before deleting a ndb make sure that it is the proper entry
 *
 * Revision 3.13.8.7  1996/05/21  09:53:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.8.6  1996/05/09  14:31:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.8.5  1996/05/04  01:22:27  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.13.8.4  1996/04/27  05:53:34  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.13.8.3.4.1  1996/04/27  07:05:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.8.3  1996/04/15  21:18:29  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.13.8.2  1996/04/02  05:37:24  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.13.8.1.8.1  1996/04/08  01:56:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.8.1  1996/03/18  20:29:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.12.1  1996/03/22  22:52:52  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/02/28  09:33:11  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.12  1996/02/13  20:38:45  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.11.8.1  1996/03/05  06:14:55  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.11.4.1  1996/02/27  21:09:31  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11  1996/02/01  06:06:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/22  06:14:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/08  22:55:20  bcole
 * CSCdi46504:  Static IP routes are not being installed
 *
 * 64 bit timestamps are not transparent...
 * Change the way some timers are initialized to expire immediately so
 * that they work with 64 bit timestamps.
 *
 * Revision 3.8.4.1  1996/02/15  19:17:46  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.8  1995/12/10  00:42:41  pst
 * CSCdi38750:  Allow static routes with both next hop and interface
 *              also add "permanent" keyword for non-flapping static routes
 *
 * Revision 3.7  1995/12/10  00:35:17  pst
 * CSCdi45335:  Make RIPv2 less conservative about mask guesing and fix
 * default
 *
 * Revision 3.6  1995/12/07  22:29:56  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/11/27  21:20:56  klmj
 * CSCdi37812:  IP: RIP does not immediately flush high metric routes
 *
 * Revision 3.4  1995/11/19  06:07:57  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Return indication from route_add_neighbor() as whether routing tables
 * were modified in order to avoid unnecessary route adjustment.
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.3  1995/11/18  03:29:06  pst
 * CSCdi43467:  unumbered interfaces dont pick up changes based upon
 * numbered change
 *
 * Revision 3.2  1995/11/17  17:36:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.24  1995/11/08  21:06:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.23  1995/10/23  20:59:36  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.22  1995/10/18  08:21:25  tli
 * CSCdi42366:  IP: cache confused
 * Flush the cache if we delete an ndb that has no rdb's.  It could be in
 * holddown.  If an ndb is in holddown, do not install /32's in the cache.
 *
 * Revision 2.21  1995/10/11  05:54:51  fox
 * CSCdi40958:  PPP address negotiation doesnt work with dialer interfaces
 * Point neighbor routes at the dialer interface if applicable
 * Delete a neighbor route even if the interface is not up
 *
 * Revision 2.20  1995/10/09  11:35:52  tli
 * CSCdi41807:  SSE: overlapping prefixes causes manager stop
 * If we remove an ndb from an interface prune, be sure to blow any longer
 * cache entries.
 *
 * Revision 2.19  1995/10/04  22:15:24  rchandra
 * CSCdi41548:  BGP : wrong nexthop could be advertised to EBGP peers
 * - while doing the last resort brute force nexthop calculation, pass the
 *   mask to iprouting_first_hop to pick the correct entry.
 *
 * Revision 2.18  1995/09/30  16:57:15  albright
 * CSCdi34629:  Variance and traffic share on eigrp not working
 *
 * Botched previous patch
 *
 * Revision 2.17  1995/09/29  14:45:18  albright
 * CSCdi34629:  Variance and traffic share on eigrp not working
 *
 * Also included: Unequal cost paths are, in some cases, being
 * incorrectly promoted in the eigrp (dual) topology table.
 *
 * Revision 2.16  1995/09/23  21:41:00  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.15  1995/09/21  22:12:20  raj
 * CSCdi35158:  major network summary not sent in RIP/IGRP out unnumbered
 * interface
 * Comments seemed to indicate this case had been taken care of already
 * but it hadn't.  It has now.
 *
 * Revision 2.14  1995/09/13  02:42:20  tli
 * CSCdi40246:  IP: cache ages static interface recursive entries too fast
 *
 * Revision 2.13  1995/09/11  23:50:54  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.12  1995/09/10 01:48:36  shankar
 * CSCdi38875:  Crash when updating static routes at end of Autoinstall
 * Part B of commit. credit pst with this one.
 *
 * Revision 2.11  1995/09/06  17:27:47  pst
 * CSCdi39840:  IP route cache maintenance needs more subtle invalidation
 * code
 *
 * Revision 2.10  1995/08/24  22:29:00  albright
 * CSCdi38766:  Static routes not redistributed into EIGRP after clear ip
 * route *
 *
 * Revision 2.9  1995/08/09  16:20:57  albright
 * CSCdi35180:  subnet mask of next GW for static routes not checked if
 * default defi
 *
 * Revision 2.8  1995/08/07  17:22:12  mikel
 * CSCdi36060:  <clear ip route *> corrupts memory and crashes 7000.
 * default_ndb was pointing to free'd memory due to a check_suspend in
 * table_init().
 *
 * Revision 2.7  1995/08/05  23:49:28  pst
 * CSCdi36713:  The IP route cache can get out of sync with the main
 * routing table if a route has just come out of holddown, new
 * information comes in,  and the route itself was never scrapped.
 *
 * While I'm at it, clean up some of the cache expiration code so
 * that there are less weird exception cases and more general cases.
 * This still needs work to get it to the ideal:
 *         "expire an entry if any rdb has changed (or gone away)"
 *         "expire an entry any time a [ns]db goes away"
 *
 * Revision 2.6  1995/07/21  18:23:14  myeung
 * CSCdi36948:  OSPF reinstall route with same metric causing cache
 * invalidation.
 *
 * Revision 2.5  1995/07/12  23:02:39  bcole
 * CSCdi28695: parallel paths on the same subnet cause erroneous IP
 * tfk. forwarding
 *
 * When equal cost load balancing over multiple directly connected
 * interfaces, be sure to locally receive IP packets destined for us,
 * instead of possibly forwarding the packet back out one of the alternate
 * interfaces.
 *
 * Revision 2.4  1995/06/28  03:21:54  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.3  1995/06/19 01:08:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:06:40  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:10:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "route.h"
#include "logger.h"
#include "../iprouting/msg_iproute.c"	/* Not a typo, see logger.h */
#include "../h/sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "route_inlines.h"
#include "../if/network.h"
#include "../ip/ip_media.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "ospf_public.h"
#include "route_map.h"
#include "../os/free.h"
#include "parser_defs_router.h"
#include "../wan/snapshot.h"


/*
 * Local Storage
 */
ndbtype *nettable[NETHASHLEN];		/* hash table of major IP nets */
ndbtype *default_ndb;			/* pointer to default net */
ndbtype *ndb_zero;			/* for network 0.0.0.0 handling */
static watched_boolean *iprouting_new_default_needed; /* Need to recompute
							 default gw? */
watched_boolean *ip_statics_unsynced;	/* Need to adjust IP static routes? */
static sys_timestamp iprouting_background_timer; /* simple one-minute timer */
ipaddrtype default_network[MAXDEFNETWORKS]; /* array of default networks */
int default_netcount;			/* count of default networks */
boolean default_netsource;		/* we can produce 0.0.0.0 defaults */
queuetype protoQ;			/* list of pdb's */
pdbtype *pdb_CONNECTED;			/* the pdb for connected nets */
pdbtype *pdb_STATIC;			/* the pdb for static nets */
queuetype ipstaticQ;			/* list of static routes. */
sys_timestamp ndb_failtime;		/* last time ndb_malloc() failed */
boolean iprouting_slow_converge;	/* TRUE to hold routes on link fail */
boolean iprouting_classless;		/* forward disconnected subnets */
ndbtype *supernettable[NETHASHLEN];	/* hash table of IP supernets */
int supernetmaskcount;			/* count of supernet masks */
masktabletype supernetmasks;		/* table of supernet masks */
ipstatictype *ipstatictable[NETHASHLEN];/* table for fast static route access*/

const char ip_pdb_name[] = "IP PDB";


/*
 * ndb_free
 *
 * Free an NDB structure
 *
 * Do protocol-specific cleanup if it's the last reference, or if asked.
 *
 * Normally we should be cleaning up while asked;  catching the last reference
 * and cleaning up is for paranoia purposes.  "cleanup_now" will normally
 * be TRUE, except for ndb_free's that undo mem_lock's.
 */
void ndb_free (ndbtype *ndb, boolean cleanup_now) {
    pdbtype *pdb;

    if (ndb == NULL)
	return;
    if (mem_refcount(ndb) == 1 || cleanup_now) {
	pdb = INDEX2PDB(ndb->pdbindex);
	if (pdb && pdb->ndb_cleanup)
	    (*pdb->ndb_cleanup)(pdb, ndb);
    }
    free(ndb);
}

/*
 * backup_free
 *
 * Free a backup structure
 *
 * Do protocol-specific cleanup if the refcount is 1, or if we've
 * been explicitly asked to.  Normally we will be asked to, except
 * when freeing something after a mem_lock.
 */
static void backup_free (backuptype *backup, boolean cleanup_now) {
    pdbtype *pdb;

    if (backup == NULL)
	return;
    if (mem_refcount(backup) == 1 || cleanup_now) {
	pdb = INDEX2PDB(backup->pdbindex);
	if (pdb && pdb->backup_cleanup)
	    (*pdb->backup_cleanup)(pdb, backup);
    }
    free(backup);
}

/*
 * net_init
 * Initialize network data structures
 */

void net_init (void)
{
    register int i;

    /* Register free lists for ndbs and rdbs. */

    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(ndbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL,
		  sizeof(ndbtype) + ((NETHASHLEN-1) * sizeof(ndbtype *)));

    /*
     * What we're really doing in this routine is firing up IP routing.
     * Set up the debug flags for it.
     */
    iprouting_debug_init();

    for (i = 0; i < NETHASHLEN; i++) {
	nettable[i] = supernettable[i] = NULL;
	ipstatictable[i] = NULL;
    }
    for (i = 0; i < MAXDEFNETWORKS; i++)
	default_network[i] = 0L;
    default_netcount = 0;
    supernetmaskcount = 0;
    default_netsource = FALSE;
    default_ndb = NULL;
    ndb_zero = NULL;
    iprouting_new_default_needed = create_watched_boolean("IP DefGW needed", 0);
    ip_statics_unsynced = create_watched_boolean("IP Statics unsynced", 0);
    iprouting_slow_converge = FALSE;
    iprouting_classless = FALSE;
    memset(ip_proctable, 0, MAX_IPPROCS * sizeof(pdbtype *));
    GET_TIMESTAMP(ndb_failtime);
    queue_init(&protoQ, 0);
    queue_init(&ipstaticQ, 0);

    pdb_CONNECTED = malloc_named(sizeof(pdbtype), ip_pdb_name);
    if (pdb_CONNECTED == NULL)
        return;
    pdb_CONNECTED->proctype = PROC_CONNECTED;
    (void) allocate_pdbindex(pdb_CONNECTED);
    pdb_CONNECTED->distance = CONNECTED_DISTANCE;
    pdb_CONNECTED->distance_cmp = iprouting_distcmp_generic;
    pdb_CONNECTED->name = "connected";
    pdb_CONNECTED->family = PDB_IP;
    pdb_CONNECTED->holddown = (holddowncbtype) return_true;
    pdb_CONNECTED->multipath = MAXROUTES;
    enqueue(&protoQ, pdb_CONNECTED);

    pdb_STATIC = malloc_named(sizeof(pdbtype), ip_pdb_name);
    if (pdb_STATIC == NULL)
        return;
    pdb_STATIC->proctype = PROC_STATIC;
    (void) allocate_pdbindex(pdb_STATIC);
    pdb_STATIC->distance = STATIC_DISTANCE;
    pdb_STATIC->distance_cmp = iprouting_distcmp_generic;
    pdb_STATIC->name = "static";
    pdb_STATIC->family = PDB_IP;
    pdb_STATIC->holddown = (holddowncbtype) return_true;
    pdb_STATIC->multipath = MAXROUTES;
    pdb_STATIC->query = adjust_static_routes;
    pdb_STATIC->add_route = ipstatic_callbackup_route;
    pdb_STATIC->malloc_backup = FALSE;
    enqueue(&protoQ, pdb_STATIC);
    if (router_enable && !system_loading)  {
	platform_buffer_init(PLATFORM_BUFFER_ROUTING);
    }
}

/*
 * table_init
 * Initialize the routing table using the interface data structures.
 */

void table_init (void)
{
    register ndbtype *ndb, *sdb;
    register int i, j;
    register idbtype *idb;
    ndbtype *temp_ndb;
    backuptype *backup, *backup_tmp;

    /*
     * First clear the entire routing table.
     */
    ndb_zero = default_ndb = NULL;
    for (i = 0; i < NETHASHLEN*2; i++) {
	process_may_suspend();
	if (i < NETHASHLEN) {
	    ndb = nettable[i];
	    nettable[i] = NULL;
	} else {
	    ndb = supernettable[i-NETHASHLEN];
	    supernettable[i-NETHASHLEN] = NULL;
	}
	while (ndb) {
	    if (ndb->attrflags & NET_SUBNETTED) {
		for (j = 0; j < NETHASHLEN; j++) {
		    sdb = ndb->subnettable[j];
		    /*
		     * Disconnect sdb
		     */
		    ndb->subnettable[j] = NULL;
		    while (sdb) {
			ip_free_all_rdb(sdb);
			temp_ndb = sdb;
			sdb = sdb->next;
			for (backup = temp_ndb->backup_info; backup;
			     backup = backup_tmp) {
			    backup_tmp = backup->next;
			    backup_free(backup, TRUE);
			}
			ndb_free(temp_ndb, TRUE);
		   }
		}
	    } else {
		ip_free_all_rdb(ndb);
	    }
	    temp_ndb = ndb;
	    ndb = ndb->next;
	    for (backup = temp_ndb->backup_info; backup;
		 backup = backup_tmp) {
		backup_tmp = backup->next;
		backup_free(backup, TRUE);
	    }
	    if (temp_ndb->attrflags & NET_VARIABLE) {
		free(temp_ndb->masktable);
		temp_ndb->masktable = NULL;
	    }
	    if (temp_ndb == default_ndb) {
		default_ndb = NULL;
		network_redist(NULL, temp_ndb, ROUTE_DOWN);
		ip_route_new_default();
	    }
	    ndb_free(temp_ndb, TRUE);
	}
    }

    /*
     * Scan all interfaces, building routing table as we go
     */
    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && interface_up(idb))
	    (void) route_interface(idb, TRUE, FALSE);
    }

    /*
     * Add static routes back into routing table, if present.
     */
    adjust_static_routes(NULL, NULL);
    /*
     * Select a new default gateway as soon as the static routes have been
     * installed.
     */
    process_set_boolean(iprouting_new_default_needed, TRUE);
    /*
     * Blow the route cache
     */
    reg_invoke_ipcache_invalidate(NULL, NULL, FALSE);

    /*
     * Poke the dynamic routing protocols if we're a running system.
     */
    if (systeminit_flag)
	table_reload(NULL);
}

/*
 * table_reload
 * Reload routing table with dynamic information by issuing a query for
 * all routing protocols that support such a function.
 * idb is valid only if the table_reload is called due to an event
 * on interface.
 */

void table_reload (idbtype *idb)
{
    pdbtype *pdb;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->running && pdb->query)
	    (*pdb->query)(pdb, idb);
    }
}

/*
 * route_interface
 * Add, update, or delete interface addresses in the routing table.
 * Invalidate routing cache as well.
 * Return an indication of whether or not the routing table might have
 * been changed.
 *
 * Note special code in route_set_neighbor also applies here.  Neighbor
 * routes will point to the 'netcfg_idb', which the top-level interface
 * in a rotary or rotary of rotaries.  The neighbor address is stored in
 * the individual interface so that we can easily remove it from the route
 * if the address is re-negotiated, the link goes down, etc.  But the
 * dialer design requires us to point the routes at the top-level interface.
 */

boolean route_interface (idbtype *idb, boolean addflag, boolean dostatics)
{
    ipaddtype *ptr;
    boolean changed_connected_route;

    /*
     * Do nothing if not running IP
     */
    if (!idb->ip_enabled)
	return(FALSE);

    changed_connected_route = FALSE;

    /*
     * If this interface does have an IP address, add it.  Add secondary
     * addresses even if there is no primary.
     */
    if (!idb->ip_unnumbered) {
	if (addflag) {
	    if (route_ifaddress(idb, idb->ip_address, idb->ip_nets_mask))
		changed_connected_route = TRUE;
	} else {
	    ip_route_delete(pdb_CONNECTED, idb->ip_address,
  			    idb->ip_nets_mask, IPADDR_ZERO,
			    IPADDR_ZERO, NULL, 0);
	    changed_connected_route = TRUE;
	}
    }

    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (addflag) {
	    if (route_ifaddress(idb, ptr->address, ptr->mask))
		changed_connected_route = TRUE;
	} else {
	    ip_route_delete(pdb_CONNECTED, ptr->address, ptr->mask,
			    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
	    changed_connected_route = TRUE;
	}
    }

    if (dostatics && changed_connected_route)
	adjust_static_routes(NULL, NULL);
    return(changed_connected_route);
}

/*
 * route_ifaddress
 * Add or update an interface address in the routing table.
 * Called by route_interface() for both primary and secondary addresses.
 * This routine assumes that the address and mask have been martian
 * checked and that the primary address characteristics of the interface
 * have already been set up by a caller of class_resolve().
 * Return an indication of whether or not the routing table has been
 * changed.
 */
  
boolean route_ifaddress (idbtype *idb, ipaddrtype address, ipaddrtype mask)
{
    boolean changed_connected_route;
    
    (void) network_update(pdb_CONNECTED, address & mask, mask, IPADDR_ZERO,
			  idb, 0, NET_ATTACHED, 0, 0,
			  &changed_connected_route, 0);
    network_revise(address);
    ip_arp_add_interface(idb, address);
    return(changed_connected_route);
}

/*
 * ip_route_adjust
 * Given an interface, add or remove routing table entry depending on whether
 * or not the line protocol is up or down.
 */

void ip_route_adjust (idbtype *idb)
{
    ndbtype *db;
    boolean present = FALSE;
    boolean unnumbered_change = FALSE;
    boolean is_up;
    int i;
    ipaddrtype addr;
    ipaddrtype mask;

    if (!idb->ip_enabled || !netsareup
	|| ((idb->ip_unnumbered == NULL) && !idb->ip_address))
	return;

    /*
     * If we have an entry, determine whether or not this interface
     * (or its transmit-interface) is mentioned in the route descriptors.
     * We may have multiple interfaces onto the same network!
     * If the interface is unnumbered, depend on the interface transition.
     * BOGON ALERT: this is all completely bogus.  route_adjust() should
     * really be called from an interface transition routine so that we
     * don't have to depend on being called once every keepalive period.
     */
    is_up = interface_up(idb);
    if (idb->ip_unnumbered) {
	if (!idb->hwptr->keep_transition)
	    return;
	unnumbered_change = TRUE;
    }
    if (!is_up && !unnumbered_change) {
	/*
	 * Lookup the network, and maybe the relevant subnet
	 */
	addr = idb->ip_address;
	mask = idb->ip_nets_mask;
	db = net_lookup(addr, FALSE);
	if (mask && db && (db->attrflags & NET_SUBNETTED))
	    db = subnet_lookup(db, addr, mask);
	
	/*
	 * We must ignore non-connected routing entries
	 */
	if (db && !(NDB2PROCTYPE(db) & PROC_CONNECTED))
	    db = NULL;
	
	if (db) {
	    for (i = 0; i < db->routecount; i++) {
		if (db->rdb[i]->idb == idb->output_if) {
		    present = TRUE;
		    break;
		}
	    }
	}
    }

    /*
     * Bring up or delete a route depending on line protocol state.
     * To allow flash updates to occur when we delete an interface route,
     * call pdb_topology() to activate the routing processes, then call
     * process_sleep_for() to allow those processes to run.  Then
     * delete the route. 
     */
    if (is_up) {
	if (route_interface(idb, TRUE, TRUE) || unnumbered_change) {
	    /*
	     * A change was actually made to the routing table
	     */
	    if (route_debug && !unnumbered_change)
		buginf("\nRT: interface %s added to routing table",
		       idb->namestring);
	    adjust_static_routes(NULL, NULL);
	    pdb_topology(idb);
	}
    } else {
	if (present || unnumbered_change) {
	    pdb_topology(idb);
	    if (!blocking_disabled)
		process_sleep_for(ONESEC/10);
	    if (route_debug)
	        buginf("\nRT: interface %s removed from routing table",
		    idb->namestring);
	    (void) route_interface(idb, FALSE, TRUE);

            /*
             * If the outgoing interface of a route entry is simplex,
             * the idb poits to its transmit-only interface,
             * not the receive-only interface.
             * Same situation on arp cache.
             *
             * Usually, the trasnmit-only and receive-only interfaces
             * of a simplex circuit have the same network number.
             * Need only call route_interface() once.
             */
	    if (iprouting_slow_converge)
		table_reload(idb->output_if);
	    else
		table_prune_idb(idb->output_if);

	    /*
	     * Only clear the entry from the arp table when the user
	     * shuts down the interface.  This prevents the arp cache
	     * from being flushed when we have an interface flapping.
	     * Do not flush entire arp table, only entries for this
	     * interface.
	     */
	    if ((idb->hwptr->status & IDB_IEEEMASK) && 
		(interface_admin_down(idb)))
		arp_flush_cache(idb->output_if);
	}
    }
}

/*
 * net_lookup
 * Given an IP address, return the best network pointer.
 * If we don't know about that major net, return pointer to net
 * of the default gateway if defaultflag is TRUE.
 *
 * You cannot lookup 0.0.0.0 with net_lookup, use netsuper.
 */

static inline ndbtype *net_lookup_inline (ipaddrtype address,
					  boolean defaultflag)
{
    register ipaddrtype majornet;
    register ndbtype *ndb;

    if (!(majornet = get_majornet(address)))
	return (NULL);

    for (ndb = nettable[nethash(majornet)]; ndb; ndb = ndb->next)
	if (ndb->netnumber == majornet)
	    return (ndb);

    return (defaultflag ? default_ndb : NULL);
}

ndbtype *net_lookup (ipaddrtype address, boolean defaultflag)
{
  return net_lookup_inline(address, defaultflag);
}

/*
 * subnet_lookup
 * Given a network pointer and an IP address, return a subnet pointer.  If
 * a mask is specified, then return an exact match.  If no mask is given
 * (mask == 0), then do longest match.
 */

static inline ndbtype *subnet_lookup_inline (ndbtype *ndb, ipaddrtype address,
					     ipaddrtype mask)
{
    register ipaddrtype subnet;
    register ndbtype *sdb;
    uint i;

    if (!(ndb->attrflags & NET_VARIABLE)) { /* single mask */
	if (mask && (mask != ndb->netsmask))
	    return(NULL);
        subnet = address & ndb->netsmask;
	sdb = ndb->subnettable[nethash(subnet)];
	for (; sdb; sdb = sdb->next) { 
	    if (sdb->netnumber == subnet) 
	    	return(sdb);
	}
    } else {				/* variable length mask */
	if (!mask) {
	    for (i = 0; i < ndb->maskcount; i++) {
		mask = ndb->masktable[i].mask;
		subnet = address & mask;
		sdb = ndb->subnettable[nethash(subnet)];
		for (; sdb; sdb = sdb->next) 
		    if ((sdb->netnumber == subnet) && (sdb->netsmask == mask))
			return(sdb);
	    }
	} else {
	    subnet = address & mask;
	    sdb = ndb->subnettable[nethash(subnet)];
	    for (; sdb; sdb = sdb->next) 
		if ((sdb->netnumber == subnet) && (sdb->netsmask == mask))
		    return(sdb);
	}
    }
    return(NULL);		/* unknown subnet */
}

ndbtype *subnet_lookup (ndbtype *ndb, ipaddrtype address, ipaddrtype mask)
{
    return subnet_lookup_inline(ndb, address, mask);
}

/*
 * supernet_lookup
 * Given an IP address and an optional mask, do a lookup in the supernet table.
 * If the mask is specified, then return an exact match, else do longest match.
 *
 * WARNING: 0.0.0.0/0.0.0.0 is treated as an exact match only due to the
 * nature of default ndb calculations.
 */

static inline ndbtype *supernet_lookup_inline (ipaddrtype address,
					       ipaddrtype mask,
					       boolean defaultflag)
{
    int i;
    ipaddrtype majormask, net;
    ndbtype *ndb;

    if (!mask && address) {	/* do longest match */
	majormask = get_majormask(address);

	/*
	 * This test is a kludge -- if we try to do a lookup on a network
	 * that is not currently A, B, or C (say a multicast address or
	 * broadcast) we should not return a route because we'll end up
	 * switching broadcasts (blegh!)
	 */
	if (majormask == MASK_HOST)
	    return (NULL);

	for (i = 0; i < supernetmaskcount; i++) {
	    mask = supernetmasks[i].mask;
	    if (SHORTEST_MASK(mask, majormask) == majormask)
		continue;

	    /*
	     * The following exception is a bow to IGRP.  We may learn
	     * default via say, RIP, and also have a candidate default
	     * path learned via IGRP.  If we simply return the supernet
	     * value, we are broken because we might ignore a default_ndb
	     * that has been chosen because it has a better administrative
	     * distance than the RIP default.  So much for simple code.
	     */
	    if (!mask)		/* if (net == 0 && mask == 0) */
	        continue;

	    net = address & mask;
	    for (ndb = supernettable[nethash(net)]; ndb; ndb = ndb->next)
		if ((ndb->netnumber == net) && (ndb->netsmask == mask))
		    return (ndb);
	}

    } else {	/* do exact match */
	net = address & mask;
	for (ndb = supernettable[nethash(net)]; ndb; ndb = ndb->next)
	    if ((ndb->netnumber == net) && (ndb->netsmask == mask))
		return (ndb);
    }

    return (defaultflag ? default_ndb : NULL);
}

ndbtype *supernet_lookup (ipaddrtype address, ipaddrtype mask,
			  boolean defaultflag)
{
    return supernet_lookup_inline(address, mask, defaultflag);
}

/*
 * netsuper_lookup
 *
 * This routine sort of behaves like ip_rtlookup(), except that it
 * is well behaved and will return a major network ndb even if the net
 * is subnetted.  It will -not- point at the summary sdb.
 */
static inline ndbtype *netsuper_lookup_inline (ipaddrtype address,
					       ipaddrtype mask,
					       boolean defaultflag)
{
    ndbtype *ndb;

    if (mask) {
	if (mask < get_majormask(address) || !address)
	    ndb = supernet_lookup_inline(address, mask, defaultflag);
	else
	    ndb = net_lookup_inline(address, defaultflag);
    } else {
	ndb = net_lookup_inline(address, FALSE);
	if (!ndb)
	    ndb = supernet_lookup_inline(address, mask, defaultflag);
    }

    return (ndb);
}

ndbtype *netsuper_lookup (ipaddrtype address, ipaddrtype mask,
			  boolean defaultflag)
{
    return (netsuper_lookup_inline(address, mask, defaultflag));
}

/*
 * iprouting_allnets_broadcast
 *
 * Return true if destination is an all nets broadcast.  This is used
 * by iprouting_first_hop.
 */
static inline boolean iprouting_allnets_broadcast (ipaddrtype address)
{
    ipaddrtype majornet, majormask;

    majormask = get_majormask(address);
    majornet = majormask & address;
    if ((address == majornet) || (address == (majornet | ~majormask))) 
	return(TRUE);
    return(FALSE);
}

/*
 * iprouting_first_hop
 * Figure out the first hop address of an IP datagram, assuming
 * we're a functional gateway. mask parameter is used by BGP when
 * it tries to find the best nexthop.
 */

idbtype *
iprouting_first_hop (ipaddrtype *address, ipaddrtype destination, 
		     ipaddrtype mask, int depth, ulong flags, 
		     ndbtype **retndb)
{
    ndbtype *ndb, *sdb;
    idbtype *idb;
    boolean defaultflag;
    boolean allnets;

    /*
     * Ensure that we don't overrun our stack because of recursive routing.
     */
    if (--depth <= 0) {
	if (route_debug)
	    buginf("\nRT: recursion error routing %i - probable routing loop",
								 *address);
	return(NULL);
    }

    defaultflag = ((flags & IP_PATH_EXACT_MATCH) == 0);
    /*
     * Lookup the net descriptor for the given IP address (or default).
     * If no network, give up.
     */
    ndb = netsuper_lookup_inline(*address, mask, defaultflag);
    if (!ndb)
      return(find_path(address, (flags | IP_PATH_USE_IF_GUESS)));

    /*
     * We come here with a default ndb if the first ndb turned
     * out to be unusable.
     */
retry:

    /*
     * If the network is not subnetted, pick a route to use.  If no route
     * and we have a default network that we're not now using, try again
     * with the default network.
     */
    if ((ndb->attrflags & NET_SUBNETTED) == 0) {
	idb = route_lookup(ndb, address, destination, depth, flags, retndb);
	if (idb)
	    return(idb);

	if ((ndb == default_ndb) || !default_ndb || (defaultflag == FALSE))
	  return(find_path(address, (flags | IP_PATH_USE_IF_GUESS)));

	ndb = default_ndb;
	goto retry;
    }

    /*
     * Net has subnets.  Lookup the subnet.
     * If the subnet exists, use the routing table entry.
     */
    sdb = subnet_lookup_inline(ndb, *address, mask);
    if (sdb) {
  	/*
	 * If spanning tree flooding is enabled, check if network broadcast
	 * address, if so, then it's not routable.
	 */
	allnets = iprouting_allnets_broadcast(*address);
	if (ip_spanning && allnets)
	    return(NULL);
	/*
	 * Subnet exists.  Lookup a route. 
	 */
	idb = route_lookup(sdb, address, destination, depth, flags, retndb);
	if (idb) {
	    /*
	     * If we are routing to null0, check if network broadcast
	     * address, if so, it's not routable.
	     */
	    if (allnets && (idb == nullidb))
		return(NULL);
	    return(idb);
	}
    }

    /*
     * If we're already at the end of our rope, give up.
     */
    if (ndb == default_ndb)
      return(find_path(address, (flags | IP_PATH_USE_IF_GUESS)));

    /*
     * Subnet doesn't exist.
     * OK, let's look to see if there is a supernet out there that covers
     * us (including the default_ndb).  If this supernet is known via
     * a mask-carrying protocol, then we're cool and we can use it to get to
     * other subnets.  Just in case someone is doing something weird like
     * redistributing from a CIDR border into a classfull internal network,
     * give them an emergency knob to bail out.
     * If the subdefault flag is set, don't allow the lack of subnet to
     * follow the default route.
     */
    ndb = supernet_lookup_inline(*address, mask, defaultflag);
    if (ndb && ((flags & IP_PATH_SUBDEFAULT) == 0) &&
	((NDB2PROCTYPE(ndb) & PROC_DISCSUBNETMASK) || iprouting_classless))
	goto retry;

    return(find_path(address, (flags | IP_PATH_USE_IF_GUESS)));
}

/*
 * iprouting_first_hop_on_idb
 * Figure out the first hop address of an IP datagram on the specified
 * interface, assuming we're a functional gateway.
 */
ipaddrtype iprouting_first_hop_on_idb (ipaddrtype address,
                                       idbtype *idb, ulong flags)
{
    ndbtype *ndb, *sdb;
    ipaddrtype majormask, majornet;
    boolean defaultflag;

    defaultflag = ((flags & IP_PATH_EXACT_MATCH) == 0);
    /*
     * Lookup the net descriptor for the given IP address (or default).
     * If no network, give up.
     */
    ndb = netsuper_lookup_inline(address, IPADDR_ZERO, defaultflag);
    if (!ndb)
        return(IPADDR_ZERO);

    /*
     * If the network is not subnetted, pick a route to use.
     */
    if ((ndb->attrflags & NET_SUBNETTED) == 0) {
	return(route_lookup_on_idb(ndb, address, idb, flags));
    }

    /*
     * Net has subnets.  Lookup the subnet.
     * If the subnet exists, use the routing table entry.
     */
    sdb = subnet_lookup_inline(ndb, address, IPADDR_ZERO);
    if (sdb) {
 	/*
 	 * Check if network broadcast address, if so, it's not routable.
 	 */
 	majormask = get_majormask(address);
 	majornet = majormask & address;
 	if ((address == majornet) || (address == (majornet | ~majormask))) 
 	    return(IPADDR_ZERO);

	/*
	 * Subnet exists.  Lookup a route. 
	 */
	return(route_lookup_on_idb(sdb, address, idb, flags));
    }
    return(IPADDR_ZERO);
}

/*
 * best_metric
 * Figure out the best metric route we have to a given address
 */

ulong ip_best_metric (ipaddrtype destination)
{
    ndbtype *db;
    rdbtype *rdb;
    idbtype *idb;
    int tries;

    tries = ROUTINGDEPTH;

    while (tries--) {
	db = ip_rtlookup(destination, 0);
	if (!db) {
	    if (default_ndb)
		db = default_ndb;
	    else
		goto fail;
	}

	if (db->routecount == 0)
	    goto fail;

	/*
	 * always use the metric from rdb[0] instead of worrying about
	 * round robins so we don't advertise different stuff
	 */

	rdb = db->rdb[0];
	idb = rdb->idb;

	if (idb && !interface_up(idb))
	    goto fail;

	/*
	 * We've selected a route.  If the interface pointer is NULL, we have
	 * a route that will require more routing to find the first hop
	 * IP address.  If no first hop (rdb->gateway) is specified, then we
	 * are dealing with a directly connected route.  If we are dealing with
	 * the default ndb, do not allow further recursion.
	 */
	if (idb)
	    return(rdb->metric);


	/*
	 * not directly connected, so loop again, trying next hop
	 */
	if (rdb->gateway)
	    destination = rdb->gateway;
	else
	    goto fail;
    }
 fail:
    return(METRIC_INACCESSIBLE);
}

/*
 * ip_rtlookup
 * Given an ip address and a mask, find an exact match in the routing table.
 * Given an ip address and mask == 0L, find the longest match.
 */

ndbtype *ip_rtlookup (ipaddrtype address, ipaddrtype mask)
{
    ndbtype *db;

    db = netsuper_lookup_inline(address, mask, FALSE);

    /* supernet lookup will explicitly NOT return ndb_zero except
       as an exact match, so we do it manually here */
    if (!db)
	return(mask ? NULL : ndb_zero);

    if (db->attrflags & NET_SUBNETTED) {
	db = subnet_lookup_inline(db, address, mask);
	if (!db) {
	    if (iprouting_classless) {
		db = supernet_lookup_inline(address, mask, FALSE);
		if (!db)
		    return(mask ? NULL : ndb_zero);
	    } else
		return(NULL);
	}
    }

    /* this protects us from longest matches if we specified a mask */
    if (mask && ((mask != db->netsmask) || (address != db->netnumber)))
	return(NULL);

    return(db);
}

/*
 * ip_rtlookupdummy
 * Given an ip address and a mask, find an exact match in the routing table.
 * Given an ip address and mask == 0L, find the longest match.
 * If we can't get an exact match, at least return the dummy ndb if available.
 */

ndbtype *ip_rtlookupdummy (ipaddrtype address, ipaddrtype mask)
{
    ndbtype *db, *sdb;

    db = netsuper_lookup_inline(address, mask, FALSE);

    /* supernet lookup will explicitly NOT return ndb_zero except
       as an exact match, so we do it manually here */
    if (!db)
	return(mask ? NULL : ndb_zero);

    if (db->attrflags & NET_SUBNETTED) {
	sdb = subnet_lookup_inline(db, address, mask);
	if (!sdb)
	    if (mask == get_majormask(address))
		return(db);		/* return dummy ndb */
	    else
		return(NULL);		/* didn't find network */
	db = sdb;
    }

    /* this protects us from longest matches if we specified a mask */
    if (mask && ((mask != db->netsmask) || (address != db->netnumber)))
	return(NULL);

    return(db);
}

/*
 * iprouting_rtlookup_exact
 * Given an ip address and a mask, find an exact match in the routing table.
 * If an exact match cannot be found, return NULL.
 */

static ndbtype *iprouting_rtlookup_exact (ipaddrtype addr, ipaddrtype mask)
{
    ndbtype *db;

    db = netsuper_lookup(addr, mask, FALSE);
    if (!db)
	return(NULL);
    if (db->attrflags & NET_SUBNETTED) {
	db = subnet_lookup(db, addr, mask);
	if (!db)
	    return(NULL);
    }
    /*
     * Only return an exact match
     */
    if (addr == db->netnumber) {
        if (mask == 0 || (mask == db->netsmask))
	    return(db);
    }
    return(NULL);
}

/*
 * route_lookup
 * Given a subnet pointer, return best interface and gateway to use.
 */

idbtype *route_lookup (ndbtype *sdb, ipaddrtype *address,
		       ipaddrtype destination, int depth, ulong flags,
		       ndbtype **retndb)
{
    int index, lastindex;
    rdbtype *rdb, *next_rdb;
    idbtype *idb, *next_idb;

    if ((flags & IP_PATH_RECURSIVE) && (flags & IP_PATH_ROUNDROBIN))
	sdb->attrflags |= NET_TARGET;

    if (retndb)
	*retndb = sdb;

    /*
     * If no routes, quit now.  If one route (the most common case),
     * optimize by selecting the first and only route.  If zero routes
     * we are probably in or just out of holddown.  Use stale information
     * rather than drop traffic.  If multiple paths, first use the current
     * route if its round robin count is still non-zero, else look for the
     * next route.
     */
    idb = NULL;
    if (sdb->routecount == 1) {
	rdb = sdb->rdb[0];
	idb = rdb->idb;
	if (idb && !interface_up(idb))
	    return(NULL);
    } else if (sdb->routecount == 0) {
	if (sdb->lastinterface && sdb->lastgateway) {
	    *address = sdb->lastgateway;
	    return(sdb->lastinterface);
	} else
	    return(NULL);
    } else {
	if (sdb->lastindex >= sdb->routecount) {
	    errmsg(&msgsym(PATHIDX, IPRT), sdb->lastindex, sdb->netnumber,
		   sdb->routecount);
	    sdb->lastindex = 0;
	}
	lastindex = sdb->lastindex;
	rdb = sdb->rdb[lastindex];
	idb = rdb->idb;
	if ((flags & IP_PATH_FAIL_FORUS) && ip_own_ndb(sdb, pdb_CONNECTED)) {
	    /*
	     * Multiple connected routes for this path.  Make sure one of these
	     * is not our address.  Otherwise, we are likely to accidently
	     * route a packet which is for us out one of our other
	     * connected interfaces.
	     */
	    if (ip_ouraddress(destination)) {
		return(NULL);
	    }
	}

	/*
	 * Run down the round robin counter, if it reaches zero, it's time
	 * to select a new route.  Start with the next route in the list,
	 * (modulo sdb->routecount) and search for the next valid target
	 * route.  If we fail to find anything, we're back where we started
	 * (i == sdb->lastindex).
	 */
	if ((flags & IP_PATH_ROUNDROBIN) && (--rdb->robin == 0)) {
	    rdb->robin = rdb->robin_reset;
	    index = lastindex;
	    do {
		if (++index >= sdb->routecount)
		    index = 0;
		next_rdb = sdb->rdb[index];
		if (next_rdb->robin_reset) {
		    next_idb = next_rdb->idb;
		    if (next_idb == NULL)
			break;
		    if (interface_up(next_idb))
			break;
		}
	    } while (index != lastindex);
	    sdb->lastindex = index;
	}
    }

    /*
     * We've selected a route.  If the interface pointer is NULL, we have
     * a static route that will require more routing to find the first hop
     * IP address.  If no first hop (rdb->gateway) is specified, then we
     * are dealing with a directly connected route.  If we are dealing with
     * the default ndb, do not allow further recursion.
     */
    if (rdb->gateway || !idb)
	*address = rdb->gateway;
    if (idb) {
	if ((*address == (sdb->netnumber & sdb->netsmask)) &&
	    (sdb->netsmask != MASK_HOST))
	    *address = destination;
	return(idb);
    }
    flags |= IP_PATH_RECURSIVE;
    /*
     * As an optimization, make sure to stop checking if packet is for us.  
     * We would have detected that case already (by finding the connected 
     * route before any recursing).
     */
    flags &= ~IP_PATH_FAIL_FORUS;

    if (sdb == default_ndb)
	flags |= IP_PATH_EXACT_MATCH;
    return(iprouting_first_hop(address, destination, 0, depth, flags, retndb));
}
/*
 * route_lookup_on_idb
 * Given a subnet pointer and an interface,
 * return the gateway on the specified interface.
 *
 * If one route (the most common case), check the first and only route
 * if the idb is same as requested.
 * If zero routes, we are probably in or just out of holddown.
 * Check the stale information rather than drop traffic.
 * If multiple paths, start from the current route, search for the route
 * with the requested idb. If we fail to find anything, we're back
 * where we started (i == sdb->lastindex).
 *
 * This function does not update load balance information.
 * Currently, this function is called when policy routing.
 */
ipaddrtype route_lookup_on_idb (ndbtype *sdb, ipaddrtype address,
                                idbtype *interface, ulong flags) 
{
    int index, lastindex;
    rdbtype *rdb;
    idbtype *idb;
    ipaddrtype gateway;

    rdb = NULL;                 /* compiler requires this */
    idb = NULL;

    if (sdb->routecount == 1) {
	rdb = sdb->rdb[0];
        idb = rdb->idb;

    } else if (sdb->routecount == 0) {
        if (sdb->lastinterface == interface)
	    return(sdb->lastgateway);

    } else {
        if (sdb->lastindex >= sdb->routecount) {
	    errmsg(&msgsym(PATHIDX, IPRT), sdb->lastindex, sdb->netnumber,
		   sdb->routecount);
	    sdb->lastindex = 0;
	}
	lastindex = sdb->lastindex;
        index = lastindex;

        do {
            rdb = sdb->rdb[index++];
            idb = rdb->idb;
            if (!idb) {
                /*
                 * this is a static route that will require more routing
                 * to find the first hop IP address and the idb
                 */
                gateway = rdb->gateway;
                flags &= ~IP_PATH_FAIL_FORUS;
                if (sdb == default_ndb)
                    flags |= IP_PATH_EXACT_MATCH;

                idb = iprouting_first_hop(&gateway, address, IPADDR_ZERO,
                                          ROUTINGDEPTH, flags, NULL);
            }
            if (idb == interface)
                break;

            if (index >= sdb->routecount)
                index = 0;

	} while (index != lastindex);
    }
    if (idb == interface)
        return(rdb->gateway);
    else
        return(address);
}

/*
 * get_nets_per_address
 *
 * Return the number of networks a given routing process has configured
 * relative to a particular address.
 */
int get_nets_per_address (pdbtype *pdb, ipaddrtype address)
{
    ipaddrtype majornet;
    int i, count;

    count = 0;
    majornet = get_majornet(address);
    for (i = 0; i < pdb->netcount; i++) {
	if (majornet == get_majornet(pdb->networks[i]))
	    count++;
    }
    return(count);
}

/*
 * get_nets_per_idb
 * Return the number of networks a routing process has configured for
 * a given interface.
 */

int get_nets_per_idb (pdbtype *pdb, idbtype *idb)
{

    ipaddtype *ptr;
    ipaddrtype address;
    int        count;

    count = 0;
    if (idb->ip_unnumbered)
	address = idb->ip_unnumbered->ip_address;
    else
	address = idb->ip_address;
    count += get_nets_per_address(pdb, address);
    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	count += get_nets_per_address(pdb, ptr->address);
    }
    return(count);
}

/*
 * ndb_malloc
 * Allocate memory for a network or subnet in the IP routing table
 * If we're low on memory, try reclaiming some before giving up.
 * Complain every two minutes if we can't do anything.
 */

ndbtype *ndb_malloc (int bytes)
{
    ndbtype *ndb;

    ndb = malloc_named(bytes, "IP NDB");
    if ((ndb == NULL) && CLOCK_OUTSIDE_INTERVAL(ndb_failtime, (2*ONEMIN))) {
	reg_invoke_ipcache_invalidate(NULL, NULL, FALSE);
        ndb = malloc_named(bytes, "IP NDB");
	if (ndb == NULL) {
	    GET_TIMESTAMP(ndb_failtime);
        }
    }
    return(ndb);
}

/*
 * network_create
 * Create a major network entry or a supernet entry
 */

ndbtype *network_create (pdbtype *pdb, ipaddrtype address, ipaddrtype mask)
{
    ipaddrtype majornet, majormask;
    boolean subnetted;
    int bucket, size;
    ndbtype *ndb;

    majormask = get_majormask(address);
    subnetted = (mask > majormask) && address;
    majornet = address & majormask;

    /*
     * Now create the network descriptor
     */
    bucket = nethash(majornet);
    size = sizeof(ndbtype);
    if (subnetted)
	size += (NETHASHLEN-1)*(sizeof(ndbtype *));
    ndb = ndb_malloc(size);
    if (!ndb)
	return(NULL);
    ndb->netnumber = majornet;
    if (subnetted)
	ndb->attrflags = NET_SUBNETTED;
    ndb->netsmask = mask;
    ndb->pdbindex = pdb->index;
    if (mask < majormask || !address) {		/* supernet */
	ndb->next = supernettable[bucket];
	supernettable[bucket] = ndb;
	ndb->attrflags |= NET_SUPERNET;
	ip_supernet_addmask(mask);
	if (!address && !mask) {
	    ndb_zero = ndb;
	    ndb->attrflags |= NET_SEXTERIOR;
	}	    
    } else {
	ndb->next = nettable[bucket];
	nettable[bucket] = ndb;
    }
    return(ndb);
}

/*
 * iprouting_exterior_change
 * Notify non-periodic update routing protocols that a route has changed
 * its interior/exterior disposition.
 */

static boolean iprouting_exterior_change (pdbtype *running_pdb, ndbtype *ndb,
					  ulong old_attrflags)
{
    pdbtype *pdb;
    boolean was_exterior, is_exterior;

    was_exterior = (old_attrflags & NET_EXTERIOR);
    is_exterior = (ndb->attrflags & NET_EXTERIOR);
    if (was_exterior == is_exterior)
	return(FALSE);
    if (route_debug)
	buginf("\nRT: %i is now %sterior", ndb->netnumber,
	       is_exterior ? "ex" : "in");
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(pdb->exterior_cb))
	    continue;
	/*
	 * Skip notification of running pdb.  It had best already know
	 * about this event.
	 */
	if (running_pdb && (running_pdb == pdb))
	    continue;
	if (!ip_redist_flag(ndb, pdb))
	    continue;
	(*pdb->exterior_cb)(pdb, ndb, is_exterior);
    }
    return(TRUE);
}


/*
 * network_redist 
 * Call the pdbs that might want to redistribute
 * this route.  BGP is also called back so that it can do IGP
 * synchronization. 
 */
void network_redist (pdbtype *pdb, ndbtype *ndb, int event)
{
    pdbtype *dist;

    if (ndb == NULL)
	return;
    if (ndb->attrflags & NET_NOREDIST)
	return;
    /*
     * when it is trying to redistribute
     * a default route, this routine will be
     * called with (pdb == NULL). When redistributing
     * default route, do not bother to check redistribution
     * flags, but make sure that we are not calling the pdb
     * that created this default.
     */
    if ((pdb == NULL) && (ndb == default_ndb)) {
	for (dist = protoQ.qhead; dist ; dist = dist->next) {
	    if ((dist->redist_callback) &&
		(dist->proctype & PROC_MAINTAINDEFAULT) &&
		(dist->index != ndb->pdbindex)) {
		(*dist->redist_callback)(dist, ndb, event, DEFAULT_ROUTE);
	    }
	}
	return;
    }
    if (pdb == NULL)
	pdb = INDEX2PDB(ndb->pdbindex);

    for (dist = protoQ.qhead; dist ; dist = dist->next) {
	if (!(dist->redist_callback))
	    continue;
	if (((ndb->attrflags & NET_SUBNETTED) == 0) &&
	    (dist->index == ndb->pdbindex)) {
	    /*
	     * Don't notify route owner of redist events regarding their
	     * route.  They should already know of this event because they
	     * caused it, or learned of it via other means (interface state
	     * change, table reload, etc.)
	     *
	     * If dummy ndb, owner is unknown so avoid this check.
	     */
	    continue;
	}
	/*
	 * BGP processes always get called so they can do IGP synchronization.
	 */
	if ((pdb->mask & dist->distflag) ||
	    ip_redist_flag(ndb, dist) ||
	    (dist->proctype & PROC_BGP)) {
	    (*dist->redist_callback)(dist, ndb, event, 0);
	}
	/*
	 * Handle the case where we are doing default route generation
	 * based on a route-map. If a route-map depends on this ndb,
	 * we must originate/withdraw the default.
	 */
	if (dist->default_routemap) {
	    if (route_map_ip_check(dist->default_routemap, ndb, dist)) {
		(*dist->redist_callback)(dist, ndb, event, DEFAULT_ROUTE);
	    }
	}
    }
}

/*
 * invalidate_redist
 * Invalidate redistributed routes from the
 * given routing process.
 */
void invalidate_redist (pdbtype *pdb)
{
    int i;
    ndbtype *ndb;
    ndbtype *sdb;
    int prev;

    for (i = 0; i < NETHASHLEN<<1; i++) {
	ndb = i < NETHASHLEN ? nettable[i] : supernettable[i-NETHASHLEN];
	for (; ndb; ndb = ndb->next) {
	    if (ndb->attrflags & NET_SUBNETTED) {
		prev = 0;
		while (prev < NETHASHLEN) {
		    sdb = ndb->subnettable[prev];
		    prev++;
		    while (sdb) {
			if (ip_pdb_on_ndb(sdb, pdb))
			    network_redist(pdb, sdb, ROUTE_DOWN);
			sdb = sdb->next;
		    }
		}
	    } else 
		if (ip_pdb_on_ndb(ndb, pdb))
		    network_redist(pdb, ndb, ROUTE_DOWN);
	}
    }

} /* invalid_redist */

/*
 * ip_ifmask
 * If the given address matches a major net on this interface, then return the 
 * appropriate mask.
 * If there are multiple masks on the interface, return the longest mask.
 */

ipaddrtype ip_ifmask (idbtype *idb, ipaddrtype address)
{
    ipaddrtype majornet, mask;
    ipaddtype *ptr;

    if (idb->ip_unnumbered)		/* check all idb's */
	idb = idb->ip_unnumbered;

    majornet  = get_majornet(address);
    mask      = MASK_UNKNOWN;		/* 0 */

    /*
     * if route matches major net on this interface
     * and there are no spare bits of route information after the subnet
     * mask is set, then this is a valid mask for that subnet...
     */

    if ((idb->ip_majornet == majornet) && 
        ((idb->ip_nets_mask & address) == address))
	mask = idb->ip_nets_mask;

    /*
     * look for more specific masks that meet above requirements...
     */

    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next)
	if ((get_majornet(ptr->address) == majornet) &&
	    ((ptr->mask & address) == address))
	    mask = LONGEST_MASK(mask, ptr->mask);

    return(mask);
}

static inline boolean
acl_route_debug (ipaddrtype address, ipaddrtype mask)
{
    acl_headertype *acl;

    if (!route_debug)
	return(FALSE);

    if (iprouting_acl) {
	acl = acl_array[iprouting_acl];
	return(route_check(acl, address, mask));
    }
    
    return(TRUE);
}

/*
 * iprouting_cache_clean
 * Delete all IP cache entries covered by this route.
 */

static inline void
iprouting_cache_clean (ndbtype *ndb)
{
    ipaddrtype mask = ndb_mask(ndb);
    reg_invoke_ipcache_invalidate_subrange(ndb->netnumber, mask, mask);
}

/*
 * free_all_rdbs
 * Delete all rdbs of a ndb and set ndb->routecount to 0.
 */

static inline void
free_all_rdbs (ndbtype *db)
{
    int ki;
    rdbtype *rdb;

    for (ki = 0; ki < db->routecount; ki++) {
	rdb = db->rdb[ki];
	db->rdb[ki] = NULL;
	rdb_free(rdb, TRUE);
    }
    db->routecount = 0;
}

/*
 * network_update
 * Create or update an entry in a network or subnet table.
 * Variable "db" refers to the descriptor block we are updating.
 */

rdbtype *
network_update (
    pdbtype *pdb,		/* protocol descriptor block */
    ipaddrtype address,		/* network, subnet, or interface address */
    ipaddrtype mask,		/* mask for this route */
    ipaddrtype gateway,		/* a local gateway, or zero if d/c */
    idbtype *interface,		/* pointer to an interface */
    long metric,		/* metric.  Update method specific. */
    long flags,			/* how this update happened */
    ipaddrtype source,		/* who told us this route */
    ulong tag,			/* arbitrary tag field */
    boolean *changed_rdb,	/* made change to routing table ? */
    ushort route_type)		/* type of route */
{
    ndbtype *ndb, *sdb, *db;
    rdbtype *rdb;
    int i, worstindex, rdbindex, distance, deleted;
    long worst;
    boolean createflag, delete_majornet_route, supernet, subnetzero_allowed;
    boolean distance_changed, mask_supplied;
    ushort event;
    pdbtype *old_pdb;
    ulong old_attrflags;
    enum DISTTYPE dist_status;
    acl_headertype *acl;
    ipaddrtype majormask;

    distance_changed = FALSE;
    delete_majornet_route = FALSE;
    supernet = FALSE;
    /*
     * If we create an ndb, we want to suspend() after updating it.
     * This gives the scheduler a chance to run during long initial updates.
     */
    createflag = FALSE;
    event = 0;
    if (changed_rdb)
	*changed_rdb = FALSE;

    /*
     * Compute administrative distance.  If maxed out, just drop the entry.
     */
    distance = pdb_distance(pdb, source, address, mask, flags, route_type);
    if (distance >= MAXDISTANCE)
	return(NULL);

    /*
     * For simplex circuits we may need to map a receive-only interface
     * to its transmit-only counterpart.  Normally if we can receive on
     * an interface we expect to be able to also transmit on that interface.
     *
     * While we know we have an interface, check if an interface specific
     * distribute-list has been specified.
     * Don't create entry if interface line protocol is down.
     */
    if (interface) {
	interface = interface->output_if;
        acl = interface->ip_iprotofltr[pdb->index];
        if (acl && (network_check(acl, gateway, address) == FALSE))
	    return(NULL);
	if (!interface_up(interface))
	    return(NULL);
    }
 
    /*
     * If we're filtering incoming updates, check this one out.
     */
    if (pdb->ifltr_all.acl) {
	if (network_check(pdb->ifltr_all.acl, gateway, address) == FALSE)
	    return(NULL);
    }

    /*
     * Make sure that we got a sane netmask.  If we didn't get one, look at 
     * the mask on the interface.  Otherwise, it's a host route.  Do a sanity
     * check just to make sure that the address has no host bits.
     * While trying to decide if the mask is sane, take into account that
     * few protocols are allowed to receive subnetzero mask in update even
     * when subnetzero is not enabled.
     */
    majormask = get_majormask(address);
    mask_supplied = mask || (address == IPADDR_ZERO);

    subnetzero_allowed = subnetzero_enable || mask_supplied;

    if (!good_route_mask(address, mask, subnetzero_allowed)) {
	if (interface)
	    mask = ip_ifmask(interface, address);
	if (!mask)
	    mask = majormask;
    }
    if ((mask & address) != address)
	mask = MASK_HOST;

    /*
     * If some twit sent us a route to ourselves, ignore him.
     */
    if ((mask == MASK_HOST) && (pdb != pdb_CONNECTED) &&
	ip_ouraddress(address))
	return(NULL);

    /*
     * Lookup the subnet or network.
     * Create entry if it doesn't already exist.
     * Don't create an entry for an inaccessible subnet or network.
     * Don't create an entry for a martian network, unless Null0 is interface
     * Don't create subnet zero unless we explicitly permit that.
     * Allow network 0.0.0.0 as special case for VLSM capable protocols,
     * and EGP.
     */

    if (mask < majormask || !address) {
	ndb = supernet_lookup(address, mask, FALSE);
	supernet = TRUE;
    } else {
	ndb = net_lookup(address, FALSE);
	supernet = FALSE;
    }
    if (!ndb) {
	if (metric == METRIC_INACCESSIBLE)
	    return(NULL);
	if (martian(address, FALSE)) {
	    /* only allow network zero from certain protocols */
	    if ((address == 0L) &&
		!(pdb->proctype &
		 (PROC_CONNECTED|PROC_RIP|PROC_BGP|PROC_IGRP2
		  |PROC_STATIC|PROC_OSPF|PROC_EGP|PROC_ISIS))) {
		return(NULL);
	    }
	    /*
	     * Never enter network other than class A, B and C 
	     * into routing table.
	     */
	    if (CLASSD_AND_GREATER(address))
	        return(NULL);
	    /* only allow non-supernet martians if we're coming from null */
	    if (!supernet && interface && (interface != nullidb))
		return(NULL);
	}
	if ((get_majornet(address) == address) && (majormask != mask) &&
	    !subnetzero_allowed && !supernet)
	    return(NULL);	
	ndb = network_create(pdb, address, mask);
	if (!ndb)
	    return(NULL);
	createflag = TRUE;
	ndb->metric = metric;
	ndb->distance = distance;
	ndb->lastgateway = gateway;
	ndb->lastinterface = interface;
	ndb->tag = tag;
	if (pdb->ndb_init)
	    (*pdb->ndb_init)(pdb, ndb, TRUE);
	ndb->route_type = route_type;
	if ((flags & NET_ATTACHED) && (mask != ndb->netsmask)) {
	    ndb->attachcount++;
	    ndb->attrflags |= NET_ATTACHED;
	}
    }

    /*
     * Don't install network routes that we receive if we have an
     * attached subnet already on this network (assuming that this
     * is a protocol that doesn't support VLSM)
     *
     */
    if (!mask_supplied && ((ndb->attrflags & (NET_ATTACHED|NET_SUBNETTED)) ==
	 (NET_ATTACHED|NET_SUBNETTED)) && (mask == majormask)) {
	/*
	 * We can imply that the mask is okay if igrp has flagged this route
	 * as a system route, which means a majornet route.
	 */
	if ((flags & NET_SYSTEM) == 0)
	    return (NULL);
	/*
	 * This is funky.  We are trying to accept a majornet route in only
	 * the case where ppp has installed a host route for the neighbor
	 * at the other end of the link and there is no other subnet in
	 * the routing table - YET!
	 */
	if (ndb->netsmask != MASK_HOST)
	    return (NULL);
    }

    /*
     * If the ndb is not subnetted, and the mask doesn't match the route
     * that we're adding, subnet the network.
     */
    if (((ndb->attrflags & NET_SUBNETTED) == 0) && (mask != ndb->netsmask)) {
	/*
	 * If the existing summary route ndb was learned via a protocol
	 * that doesn't support VLSM, and the new subnet route is attached,
	 * then flush the learned summary route (after we first make the new
	 * subnet route).
	 */
	if (!mask_supplied && (flags & NET_ATTACHED))
	    delete_majornet_route = TRUE;
     	ndb = ip_ndb_subnet(ndb);
 	if (!ndb)
 	    return(NULL);
    }

    if (ndb->attrflags & NET_SUBNETTED) {
 	sdb = subnet_lookup(ndb, address, mask);
	if (!sdb) {
	    if (metric == METRIC_INACCESSIBLE)
	        return(NULL);
	    if (martian(address, FALSE))
		if (interface && (interface != nullidb))
		    return(NULL);
	    if ((ndb->netnumber == address) && (majormask != mask) &&
		!subnetzero_allowed)
		return(NULL);	
	    address = (address & mask);
	    if (!(ndb->attrflags & NET_VARIABLE) && (ndb->netsmask != mask))
	        if (!ip_ndb_variable(ndb))
		    return(NULL);
	    sdb = ndb_malloc(sizeof(ndbtype));
	    if (!sdb)
	        return(NULL);
	    createflag = TRUE;
	    i = nethash(address);
	    sdb->next = ndb->subnettable[i];
	    ndb->subnettable[i] = sdb;
	    ndb->subnets++;
	    sdb->netnumber = address;
	    sdb->netsmask = mask;
	    sdb->metric = metric;
	    sdb->distance = distance;
	    sdb->tag = tag;
	    sdb->attrflags = NET_SUBNET;
	    sdb->lastgateway = gateway;
	    sdb->lastinterface = interface;
	    if (pdb->ndb_init)
		(*pdb->ndb_init)(pdb, sdb, TRUE);
	    sdb->route_type = route_type;
 	    if (ndb->attrflags & NET_VARIABLE) 
 	        ip_ndb_addmask(ndb, mask);
	    if (flags & NET_ATTACHED) {
		sdb->attachcount++;
		sdb->attrflags |= NET_ATTACHED;
		ndb->attachcount++;
		ndb->attrflags |= NET_ATTACHED;
	    }
	}
	/*
	 * Invalidate ndb for our majornet
	 */
	if (delete_majornet_route)
	    ip_route_delete(INDEX2PDB(ndb->pdbindex), ndb->netnumber,
			    ndb->netsmask, IPADDR_ZERO,
			    IPADDR_ZERO, NULL, 0); 
    } else
 	sdb = NULL;

    db = sdb ? sdb : ndb;

    /*
     * Check administrative distance. The update method with the lowest
     * distance is the one we will select.
     */
    dist_status = (*pdb->distance_cmp)(pdb, distance, db, metric, route_type);
    switch (dist_status) {
    case IP_DIST_WORSE:
	/*
	 * keep the info about the route with
	 * higher distance, in case the present
	 * route goes away, we need to bring back
	 * this route. Keep this information only
	 * if the calling pdb has call back mechanism.
	 */
	if (pdb->add_route) {
 
  	    /*
 	     * Check if this entry is already in the list.
 	     */
 	    if (!check_backup_info(pdb, db)) {
 		build_backup_info(pdb, db, route_type, FALSE);
  	    }
  	}
  	return(NULL);
	break;
    case IP_DIST_BETTER:
	distance_changed = TRUE;
	if (metric == METRIC_INACCESSIBLE)
	     return(NULL);
	old_pdb = INDEX2PDB(db->pdbindex);

	/*
	 * Back up the old info before deleting it.  Note that for ISIS
	 * this will clean out the ndb_isis_backup_info word for us.
	 */
  	if (old_pdb && old_pdb->add_route) {
 	    build_backup_info(old_pdb, db, db->route_type, TRUE);
  	}
	if (acl_route_debug(db->netnumber, db->netsmask))
	    buginf("\nRT: closer admin distance for %i, flushing %d routes",
			db->netnumber, db->routecount);
	free_all_rdbs(db);
	iprouting_cache_clean(db);
	if (db == default_ndb) {
	    /* this just flags the default as down, not the route itself */
	    network_redist(NULL, db, ROUTE_DOWN);
	    default_ndb = NULL;
	    ip_route_new_default();
	}

	/*
	 * Since the old route is being over written
	 * invalidate its redistribution before over
	 * writing.
	 */
	if (!ip_own_ndb(db, pdb)) {
	    network_redist(old_pdb, db, ROUTE_DOWN);

	    /*
	     * IP-IGRP2 needs to be informed when route disappears in routing
	     * table. It has to keep its topology table in sync.
	     */
	    if (old_pdb->lost_route)
		(*old_pdb->lost_route)(old_pdb, db);
	}

	db->lastindex = 0;
	db->metric = metric;
	db->distance = distance;
	db->tag = 0;
	TIMER_STOP(db->blindtimer);
	db->attrflags &= NET_PERMATTR;
	if (pdb->ndb_init)
	    (*pdb->ndb_init) (pdb, db, TRUE);
	db->route_type = route_type;
	if (db->attrflags & NET_ATTACHED) {
	    if (sdb && (--ndb->attachcount == 0))
		ndb->attrflags &= ~NET_ATTACHED;
	    db->attrflags &= ~NET_ATTACHED;
	    db->attachcount = 0;
	}
	if (flags & NET_ATTACHED) {
	    if (sdb) {
		ndb->attachcount++;
		ndb->attrflags |= NET_ATTACHED;
	    }
	    db->attrflags |= NET_ATTACHED;
	    db->attachcount = 1;
	}
	break;
    case IP_DIST_SAME:
	break;
    }
    /*
     * If we're in holddown, ignore this update.
     */
    if (TIMER_RUNNING(db->blindtimer)) {
	if (SLEEPING(db->blindtimer))
	    return(NULL);
	else {
	    TIMER_STOP(db->blindtimer);
	    if (acl_route_debug(db->netnumber, db->netsmask))
	        buginf("\nRT: %i came out of holddown", db->netnumber);
	}
    }

    /*
     * Make sure the gateway is really on this interface.
     * If not, set interface pointer to NULL as a signal to route_lookup
     * that further routing is required.  This allows us to run EGP from
     * behind another gateway and still get good information.
     * If the input interface is unnumbered, skip this check so we
     * *don't* recurse in route_lookup().  If split horizon is disabled on
     * this interface, also skip it.
     * If this update is via Mobility, skip this check, as all bets are
     * off.
     */
    if (gateway && interface && !interface->ip_split_disabled &&
	!(pdb->proctype & (PROC_MOBILE|PROC_STATIC))) {
	if (!ip_onifsubnet(interface, gateway))
	    interface = NULL;
    }

    /*
     * Look for a matching routing descriptor block.
     * This code sets up an rdb pointer and its index.
     */
    rdbindex = -1;
    rdb = NULL;
    for (i = 0; i < db->routecount; i++) {
	rdb = db->rdb[i];
	if ((rdb->gateway == gateway) 
	    &&((NDB2PROCTYPE(db) & PROC_OSPF) || (rdb->infosource == source)) 
	    && (rdb->idb == interface)) {
	    rdbindex = i;
	    break;
	}
    }
    if (rdbindex == -1)		/* never matched, clear any rdb pointer */
	rdb = NULL;

    /*
     * Update has a better (smaller) metric than the existing best metric.
     * Flush existing information that doesn't pass the holddown test
     * based on the new, better metric.  If there isn't at least one open
     * slot, throw away worst cost metric.  If they are all equal cost,
     * throw away the first one in the list to make room for new
     * information.
     *
     * OSPF note :
     * OSPF already keeps track of the routes it installed. Whenever OSPF
     * calls network_update() for a particular destination, the
     * possibilities are -
     *
     * (1) A new run of spf
     * (2) OSPF has found an equal cost route during the same spf
     * (3) OSPF has found a lower cost/better type route during 
     *     the same spf
     *
     * All OSPF handling happen in the following if clause, except
     * when it is an equal cost route, in which case we bypass the
     * if clause. 
     * We bypass the if clause to allow -
     * (1) parallel paths existed in the same spf
     * (2) avoidance of unnecessary cache invalidation when 
     *     there is no change in route from the previous spf 
     *
     */
    if ((metric < db->metric) ||
  	((pdb->proctype & PROC_OSPF) &&
	 ((metric != db->metric) || 
	  (route_type != db->route_type) ||
	  ((route_type & (RTYPE_NSSA2 | RTYPE_EXTERN2)) && 
	   (db->forw_dist != pdb->ospf_ex_forw_dist))))) {
	if (rdb) {
	    if (acl_route_debug(db->netnumber, db->netsmask)) {
		buginf("\nRT: metric change to %i via %i",
		       db->netnumber, rdb->gateway);
		buginf(", %s metric [%d/%d]\n	new metric [%d/%d]",
		       pdb->name, db->distance, rdb->metric,
		       distance, metric);
	    }
	    
	    /*
	     * This is done later anyway, but since we may be
	     * calling route_compress() we don't want it filling
	     * in db->metric with the old information.
	     */
	    rdb->metric = metric;
	    if (changed_rdb)
		*changed_rdb = TRUE;
	}

	/*
	 * Delete inferior routes - we'll relearn them if necessary.
	 */
	db->metric = metric;
	deleted = 0;
	for (i = 0; i < db->routecount; i++) {
	    if (rdb == db->rdb[i]) {
		continue;
	    }
	    if ((pdb->is_feasible) &&
                ((*pdb->is_feasible)(pdb, db->rdb[i], metric) == TRUE)) {
		/* keep the route, so do nothing */
	    } else {
                if (acl_route_debug(db->netnumber, db->netsmask)) {
                    buginf("\nRT: del %i%m via %i, %s metric [%d/%d]",
                           db->netnumber, db->netsmask, db->rdb[i]->gateway,
                           pdb->name, db->distance, db->rdb[i]->metric);
                }
		rdb_free(db->rdb[i], TRUE);
		db->rdb[i] = NULL;
		deleted++;
	    }
	}

	/*
	 * If we deleted anything, squash out the holes.
	 */
	if (deleted > 0) {
	    iprouting_cache_clean(db);
	    db->routecount -= deleted;
	    db->lastindex = 0;
	    if (db->routecount > 0)
		route_compress(db);
	} else if ((rdb == NULL) && (db->routecount == pdb->multipath)) {
	    /*
	     * Didn't do anything yet.  Get rid of one of these routes.
	     * If they are all equal cost, squash the first one.
	     */
	    worst = 0; worstindex = 0;
	    for (i = 0; i < db->routecount; i++)
		if (db->rdb[i]->metric > worst) {
		    worstindex = i;
		    worst = db->rdb[i]->metric;
		}
	    rdb_delete(pdb, db, worstindex);
	    /*
	     * Reset metric in case routecount just went to zero...
	     */
	    db->metric = metric;
	}
	if (db == default_ndb) {
	    /* this just flags default route as down, not the route itself */
	    network_redist(NULL, db, ROUTE_DOWN);
	    default_ndb = NULL;
	    ip_route_new_default();
	}
	TIMER_STOP(db->blindtimer);
	event = ROUTE_MODIF;
    } else if (metric > db->metric) {

	/*
	 * The update has a worse (larger) metric than our recorded best.
	 * Call protocol specific function to determine what we're going
	 * to do with this worse route.
	 */
	if (pdb->holddown) {
	    if ((*pdb->holddown)(pdb, db, rdb, metric)) {
		/*
		 * Path is definitely worse.  If we're updating an old
		 * path, flush it.  If all paths to that network then vanish,
		 * we may then go into holddown.
		 */
		if (rdb)
		    rdb_delete(pdb, db, rdbindex); /* very bad, flush */
		return(NULL);
	    } else {
		/*
		 * Path is slightly worse, but we're keeping it.
		 * Possibly promote another path as being best.
		 * If metric has turned inaccessible on a known path,
		 * flush that path.  If no paths left, we may go into
		 * holddown on that route.
		 */
		if (rdb) {
                    if (acl_route_debug(db->netnumber, db->netsmask)) {
                        buginf("\nRT: metric change to %i via %i",
                               db->netnumber, rdb->gateway);
                        buginf(", %s metric [%d/%d]\n	new metric [%d/%d]",
                               pdb->name, db->distance, rdb->metric,
                               distance, metric);
                    }

		    rdb->metric = metric;
		    db->metric = METRIC_INACCESSIBLE;
		    for (i = 0; i < db->routecount; i++)
			if (db->rdb[i]->metric < db->metric)
			    db->metric = db->rdb[i]->metric;
		    if (metric == METRIC_INACCESSIBLE) {
			rdb_delete(pdb, db, rdbindex);
			return(NULL);
		    }
		} else if (db->routecount == pdb->multipath) {
		    /*
		     * We may want to add this route if it is better than some
		     * other feasible successor.
		     */
		    worst = metric;
		    worstindex = -1;
		    for (i = 0; i < db->routecount; i++) {
			if (db->rdb[i]->metric > worst) {
			    worstindex = i;
			    worst = db->rdb[i]->metric;
			}
		    }
		    if (worstindex == -1)
			return(NULL);
		    rdb_delete(pdb, db, worstindex);
		}
	    }
	}
    }

    /*
     * If no matches (rdb == NULL), create a new route, if possible
     * Don't create if the route is inaccessible.
     */
    if (rdb == NULL) {
	if (db->routecount >= pdb->multipath) 
	    return(NULL);
	if (metric == METRIC_INACCESSIBLE)
	    return(NULL);
	rdb = rdb_malloc();
	if (!rdb) {
	    if (db->routecount == 0)
		db->metric = METRIC_INACCESSIBLE;
	    return(NULL);
	}
	db->rdb[db->routecount++] = rdb;
	if ((db->routecount > 1) && (pdb->traffic_share == TRAFFIC_MIN))
	    rdb->robin_reset = rdb->robin = 0;
	else
	    rdb->robin_reset = rdb->robin = ROBIN_DEFAULT;
	if (db->routecount == 1) {
	    pdb->changed++;		/* net became accessible */
	    GET_TIMESTAMP(pdb->whenchanged);
	    process_wakeup(pdb->pid);
	}
	if (acl_route_debug(db->netnumber, db->netsmask))
	    buginf("\nRT: add %i%m via %i, %s metric [%d/%d]",
		   db->netnumber, db->netsmask, gateway, pdb->name,
		   distance, metric);
	if (changed_rdb)
	    *changed_rdb = TRUE;
	event = ROUTE_UP;
	/*
	 * If we're here and createflag is not set, then we're adding a
	 * parallel path or we just came out of holddown.  In either case,
	 * clean out the cache.
	 */
	if (!createflag)
	    iprouting_cache_clean(db);
    }

    /*
     * We have a routing block.  Update it.
     * Record manner of update and any other flag bits
     * Also marked last time subnet block itself was updated.
     */
    old_attrflags = db->attrflags;
    /*
     * Make sure we don't inherit former DEXTERIOR state
     */
    db->attrflags &= ~NET_DEXTERIOR;
    db->attrflags |= flags;

    /*
     * If we're changing PDB indices, call any protocol-specific code to
     * handle the PDB change.
     *
     * This complication is necessary because we don't just delete and
     * recreate the NDB when a route is replaced with something with a
     * better administrative distance.
     *
     * We only do this when there hasn't been a change in administrative
     * distance, since if there has been a distance override, the cleanup
     * has already been done earlier in the code.  Essentially this code
     * will only come into play if there are two protocol instances at the
     * same admin distance.
     */
    if (pdb->index != db->pdbindex && !distance_changed) {

 	pdbtype *oldpdb;

 	oldpdb = INDEX2PDB(db->pdbindex);

 	if (oldpdb && oldpdb->ndb_pdbchange)
 	    (*oldpdb->ndb_pdbchange) (oldpdb, db, rdb, FALSE);

 	if (pdb && pdb->ndb_pdbchange)
 	    (*pdb->ndb_pdbchange) (pdb, db, rdb, TRUE);
    }

    db->tag = tag;
    if (pdb->ndb_init)
 	(*pdb->ndb_init)(pdb, db, FALSE);
    db->route_type = route_type;
    rdb->gateway = gateway;
    rdb->infosource = source;
    rdb->idb = interface;
    rdb->metric = metric;
    rdb->pdbindex = pdb->index;
    if (pdb->rdb_init)
 	(*pdb->rdb_init)(pdb, db, rdb);
    if (metric < METRIC_INACCESSIBLE) {
 	if (rdb->rdb_flags & IP_RDB_ISIS) { /* No RDB timer for ISIS */
 	    GET_TIMESTAMP(db->nettimer);
 	} else {
 	    GET_TIMESTAMP(rdb->rdb_routetimer);
	    COPY_TIMESTAMP(rdb->rdb_routetimer, db->nettimer);
 	}
	if (pdb->supports_snapshot) {
	    if (reg_invoke_snapshot_aging_denied(rdb->idb))
		rdb->rdb_flags |= IP_RDB_SNAPSHOT_ROUTE;
	} else {
	    rdb->rdb_flags &= ~IP_RDB_SNAPSHOT_ROUTE;
	}
	if (flags & NET_PERMANENT)
	    rdb->rdb_flags |= NET_PERMANENT;
	else
	    rdb->rdb_flags &= ~NET_PERMANENT;
	db->lastgateway = gateway;
	db->lastinterface = interface;
	if (!ip_own_ndb(db, pdb))
	    event = ROUTE_UP;
    }
    db->pdbindex = pdb->index;
    ip_robin_setup(pdb, db);

    if (createflag) {
	event = ROUTE_UP;
	reg_invoke_ipcache_invalidate_target(db->netnumber, mask, mask);
    } else
	iprouting_exterior_change(pdb, db, old_attrflags);
    
    if (event)
	network_redist(pdb, db, event);

    if (!address && !mask && (event == ROUTE_UP))
	ip_route_new_default();

    /*
     * If we created a network or subnet, let the scheduler run for a bit,
     * unless the protocol request atomic updates.
     * Note that we need to be a bit tricky here since we are holding a rdb
     * pointer.  Lock the rdb and check if it is still useful when we come
     * back.  Note that we cannot free() before calling mem_refcount()
     * since the block may coalesce during a free().
     */
    if ((createflag) && (pdb->atomic_update == FALSE) &&
	(!blocking_disabled)) {
	rdb_lock(rdb);
        process_suspend();
	if (rdb_refcount(rdb) == 1) {
	    rdb_free(rdb, FALSE);
	    return(NULL);
	}
	rdb_free(rdb, FALSE);
    }
    return(rdb);
}

/*
 * rdb_delete
 * Delete a routing descriptor block.  Maybe go into holddown.
 * Invalidate the routing cache.
 */

void rdb_delete (pdbtype *pdb, ndbtype *db, int index)
{
    rdbtype *rdb;

    if (db->routecount == 0)
	return;

    iprouting_cache_clean(db);

    rdb = db->rdb[index];
    if (acl_route_debug(db->netnumber, db->netsmask))
	buginf("\nRT: delete route to %i via %i, %s metric [%d/%d]",
	db->netnumber, rdb->gateway, pdb->name, db->distance, rdb->metric);
    db->rdb[index] = NULL;
    rdb_free(rdb, TRUE);		/* flush route descriptor */
    if (--db->routecount == 0) {	/* maybe go into route blindness */
	db->lastindex = 0;
	if (pdb->holdokay)
	    TIMER_START(db->blindtimer, pdb->holdtime);
	db->metric = METRIC_INACCESSIBLE;
	pdb->changed++;			/* subnet became inaccessible */
	GET_TIMESTAMP(pdb->whenchanged);
	process_wakeup(pdb->pid);
	if (acl_route_debug(db->netnumber, db->netsmask))
	    buginf("\nRT: no routes to %i%s", db->netnumber,
		   pdb->holdokay ? ", entering holddown" : NULL);
	if (db == default_ndb) {
	    /* this just flags the default as down, not the route itself */
	    network_redist(NULL, db, ROUTE_DOWN);
	    default_ndb = NULL;
	    ip_route_new_default();
	}
	/* this then flags the route as down */
	network_redist(pdb, db, ROUTE_DOWN);
    } else {
	/*
	 * Compress existing entries, possibly reset best metric.
	 */
	route_compress(db);
    }
}

/*
 * ip_find_rdb_index
 *
 * Determine the RDB index of an RDB given the RDB pointer and NDB.
 *
 * Returns the RDB index, or -1 if not found.
 */
int ip_find_rdb_index (ndbtype *ndb, rdbtype *rdb)
{
    int ix;

    for (ix = 0; ix < ndb->routecount; ix++) {
	if (rdb == ndb->rdb[ix])
	    return(ix);
    }
    return(-1);
}

/*
 * simple_variance
 * Let the caller know if this rdb is feasible compared to the best route.
 */
boolean simple_variance (pdbtype *pdb, rdbtype *rdb, ulong bestmetric)
{
    if ((rdb->rdb_nexthop_metric < bestmetric) &&
        (bestmetric < pdb->igrp_variance * bestmetric))
        return(TRUE);
    return(FALSE);
}

/*
 * simple_holddown
 * This routine is a pdb function.  If the new metric is greater than
 * the old metric, we want to go into holddown.  This is adequate for
 * hop count protocols such as RIP.
 */

boolean simple_holddown (
    pdbtype *pdb,
    ndbtype *ndb,
    rdbtype *rdb,
    ulong newmetric)
{
    if (rdb == NULL)		/* ignore unknown worse routes */
	return(TRUE);
    return((newmetric > ndb->metric) ? TRUE : FALSE);
}

/*
 * ip_free_sdb
 * Free a sdb and clean up all of the warts.  The caller is responsible for
 * managing pointers to the sdb.
 *
 * network_redist is called twice if the route being deleted is the default
 * ndb because the first call to network_redist gets interpreted as "default
 * is down" so we call it again after clearing default_ndb to signal that
 * the route itself is ALSO down.
 *
 * NOTE : In case of Variably subnetted ndb, the ndb could become unsubnetted.
 * This will happen, if deleting a subnet leaves a majornet subnet as the only
 * remaining subnet. If this happens, it is indicated by setting *pred to NULL.
 * The caller should handle this situation appropriatly.
 */

static inline boolean ip_free_sdb (ndbtype *ndb, ndbtype *sdb, ndbtype **pred)
{
    ipaddrtype mask;

    network_redist(NULL, sdb, ROUTE_DOWN);

    if (sdb == default_ndb) {
	default_ndb = NULL;
	network_redist(NULL, sdb, ROUTE_DOWN);
	ip_route_new_default();
    }

    callbackup_routes(sdb);
    if (ndb->subnets)
	ndb->subnets--;
    mask = sdb->netsmask;
    if (sdb->attrflags & NET_ATTACHED) {
	if (--ndb->attachcount == 0) 
	    ndb->attrflags &= ~NET_ATTACHED;
    }
    sdb->next = NULL;
    ndb_free(sdb, TRUE);
    if (ndb->subnets == 0) {
	ndb->attrflags &= ~NET_SUBNETTED;
	free(ndb->masktable);
	ndb->masktable = NULL;
	ndb->netsmask = get_majormask(ndb->netnumber);
	ip_route_delete(INDEX2PDB(ndb->pdbindex),
			ndb->netnumber, ndb->netsmask,
			IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
	return(TRUE);
    } else {
	if (ndb->attrflags & NET_VARIABLE) {
	    ip_ndb_removemask(ndb, mask);
	    if (!(ndb->attrflags & NET_SUBNETTED))
		*pred = NULL;			/* subnetted to unsubnetted */
	}
    }
    return(FALSE);
}

/*
 * ip_free_ndb
 * Free an ndb and clean up all of the warts.  The caller is responsible for
 * managing pointers to the ndb.
 *
 * network_redist is called twice if the route being deleted is the default
 * ndb because the first call to network_redist gets interpreted as "default
 * is down" so we call it again after clearing default_ndb to signal that
 * the route itself is ALSO down.
 */

static inline void ip_free_ndb (ndbtype *ndb)
{
    /* flag the route (or default) as down */
    network_redist(NULL, ndb, ROUTE_DOWN);

    if (ndb == ndb_zero)
	ndb_zero = NULL;

    if (ndb == default_ndb) {
	default_ndb = NULL;
	network_redist(NULL, ndb, ROUTE_DOWN);
	ip_route_new_default();
    }

    callbackup_routes(ndb);
    ndb->next = NULL;
    free(ndb->masktable);
    ndb->masktable = NULL;
    if (ndb->attrflags & NET_SUPERNET)
	ip_supernet_removemask(ndb->netsmask);
    ndb_free(ndb, TRUE);
}

/*
 * table_ager
 * Protocol independent ager that runs down a table (super/net/sub)
 * Set pdb->changed if a group of routes becomes inaccessible.
 */
boolean table_ager (pdbtype *pdb, ndbtype *pred, ndbtype *ndb)
{
    register int i, count;
    ndbtype *sdb;
    rdbtype *rdb;
    ulong temp_metric;
    boolean walk_chain, sdb_now_ndb;
    ulong hash;

    /*
     * When we process the subnets of the variably subnetted ndb, the ndb
     * could become unsubnetted (via the call to ip_free_sdb). In that 
     * case, the subnet merged with the ndb needs to be processed and we
     * should not continue to walk thru the ndb chain. walk_chain and 
     * sdb_now_ndb are used to do this.
     */

    walk_chain = TRUE;
    sdb_now_ndb = FALSE;
    for (sdb = pred->next; sdb && walk_chain; pred = sdb, sdb = pred->next) {

	if (sdb_now_ndb)
	    walk_chain = FALSE;

	/*
	 * This allows a protocol to scan the sdb's of an ndb if it
	 * so desires. The protocol must be aware that the ndb may
	 * have not been created by it.
	 */
	if (pdb->route_ager) {
	    if (!(*pdb->route_ager)(pdb, sdb)) {
		continue;
	    }
	}

	/*
	 * Age only "pdb->mask" maintained subnets or networks.
	 */
	if (NDB2PROCTYPE(sdb) & PROC_NOAGERT)
	    continue;
	if (!ip_own_ndb(sdb, pdb))
	    continue;

	if (sdb->attrflags & NET_SUBNETTED)
	    continue;

	/*
	 * If a subnet is inaccessible, flush it if it hasn't been
	 * been updated in a very long time.
	 */
	if (sdb->metric == METRIC_INACCESSIBLE) {
	    if (sdb->routecount) {
		if (acl_route_debug(sdb->netnumber, sdb->netsmask))
		    buginf("\nRT: Flushing inaccessible subnet %i",
			   sdb->netnumber);
		free_all_rdbs(sdb);
		sdb->lastindex = 0;
	    }
	    iprouting_cache_clean(sdb);

	    if (CLOCK_OUTSIDE_INTERVAL(sdb->nettimer, pdb->flushtime)) {
		pred->next = sdb->next;
		if (acl_route_debug(sdb->netnumber, sdb->netsmask))
		    buginf("\nRT: garbage collecting entry for %i",
				    sdb->netnumber);
		if (ndb) {
		    if (ip_free_sdb(ndb, sdb, &pred))
			return(TRUE);	/* freed last subnet in NDB, abort */
		    if (!pred) {	
			/* 
			 * ndb changed from subnetted to unsubnetted.
			 * We need to process the unsubnetted ndb
			 * to avoid skiping an entry from aging.
			 */
			hash = nethash(ndb->netnumber);
			sdb = (ndbtype *)&nettable[hash];
			ndb = NULL;
			sdb_now_ndb = TRUE;
			continue;
		    }
		} else {
		    ip_free_ndb(sdb);
		}
		sdb = pred;
	    }
	    continue;
	}

	/*
	 * Subnet is accessible.  Flush any routes that are through
	 * a down interface or which have timed out from lack of updates.
	 *
	 * Note that ISIS routes should never actually get here, but we
	 * make sure that we don't look at the routetimer anyhow.
	 */
	count = sdb->routecount;
	for (i = 0; i < sdb->routecount; i++) {
	    rdb = sdb->rdb[i];
	    if ((rdb->idb && !interface_up(rdb->idb)) ||
		(rdb->rdb_flags & IP_RDB_ISIS) ||
		(CLOCK_OUTSIDE_INTERVAL(rdb->rdb_routetimer,
					pdb->invalidtime) &&
		 (!(rdb->rdb_flags & (IP_RDB_SNAPSHOT_ROUTE|
				      IP_RDB_PERMANENT))))) {
		if (acl_route_debug(sdb->netnumber, sdb->netsmask))
		    buginf("\nRT: flushed route to %i via %i (%s)",
			    sdb->netnumber, rdb->gateway,
			    rdb->idb ? rdb->idb->namestring : NULL);
		sdb->rdb[i] = NULL;
		rdb_free(rdb, TRUE);
		count--;
	    }
	}

	/*
	 * If we changed something:
	 *  - if no routes, net or subnet enters hold down.
	 *  - if still accessible, compress the route list.
	 *  - if it was the default exterior net, clear that pointer.
	 */
	if ((count == 0) || (sdb->routecount != count)) {
	    iprouting_cache_clean(sdb);
	    sdb->routecount = count;
	    if (count == 0) {
		if (acl_route_debug(sdb->netnumber, sdb->netsmask))
		    buginf("\nRT: no routes to %i%s", sdb->netnumber,
			   pdb->holdokay ? ", entering holddown" : "");
		sdb->lastindex = 0;
		if (pdb->holdokay)
		    TIMER_START(sdb->blindtimer, pdb->holdtime);
		sdb->metric = METRIC_INACCESSIBLE;
		pdb->changed++;
		GET_TIMESTAMP(pdb->whenchanged);
		process_wakeup(pdb->pid);
		if (sdb == default_ndb) {
		    network_redist(NULL, sdb, ROUTE_DOWN);
		    ip_route_new_default();
		}
		network_redist(pdb, sdb, ROUTE_DOWN);
	    } else {
	        /*
		 * Some of the cache entries that used default may
		 * have chosen the path we just deleted.  Remove
		 * any entries created by default.
		 */
	        if (sdb == default_ndb)
		    ipcache_invalidate_subrange(0, 0, 0);
		route_compress(sdb);
	    }
	}

	/*
	 * See if we need to reset the present best metric.
	 */	
	if (count != 0) {
	    temp_metric = METRIC_INACCESSIBLE;
	    for (i = 0 ; i < count; i++)
		if (sdb->rdb[i]->metric < temp_metric)
		    temp_metric = sdb->rdb[i]->metric;
	    sdb->metric = temp_metric;
	}

	/*
	 * If this is a network or subnet with more than one route
	 * descriptor, possibly adjust round robin count if IGRP derived
	 */
	ip_robin_setup(pdb, sdb);
    }
    if (sdb_now_ndb)
	return (TRUE);
    else
	return(FALSE);
}

/*
 * netarray_ager
 * Protocol independent network/subnet ager.
 * If ndb is NULL, age all networks maintained by that protocol.
 * If ndb is non-NULL, age all subnets maintained by that protocol.
 * Set pdb->changed if a group of routes becomes inaccessible.
 */

void netarray_ager (register pdbtype *pdb, ndbtype *ndb)
{
    int j;

    for (j = 0; j < NETHASHLEN; j++) {
	if (ndb) {
	    /* 
	     * age subnets of ndb. Returns true if the last sdb in ndb is
	     * expired or if the ndb has been unsubnetted.
	     */
	    if (table_ager(pdb, (ndbtype *) &ndb->subnettable[j], ndb))
		break;			/* expired last subnet in NDB */

	} else {
	    /* age all networks */
	    table_ager(pdb, (ndbtype *) &nettable[j], ndb);
	    table_ager(pdb, (ndbtype *) &supernettable[j], ndb);
	}
    }
}

/*
 * age_all_routes
 * ages all major nets and subnets by calling netarray_ager
 */
void age_all_routes (pdbtype *pdb)
{
    int i;
    ndbtype *ndb, *temp_ndb;
    
    /* age all major nets */
    netarray_ager(pdb, NULL);

    for (i = 0; i < NETHASHLEN; i++) {
	ndb = nettable[i];
	while (ndb) {
	    /*
	     * Ugly hack.  netarray_ager can cause the ndb to be
	     * freed.  Skip to the next ndb and then go back and
	     * age the previous one.
	     */
	    temp_ndb = ndb;
	    ndb = ndb->next;
	    if (temp_ndb->attrflags & NET_SUBNETTED)
		netarray_ager(pdb, temp_ndb);
	}
    }
}

/*
 * route_compress
 * Compress the array of routing pointers, reset best metric value.
 */

void route_compress (register ndbtype *sdb)
{
    register int i, j;
    register rdbtype *rdb;
    rdbtype *routes[MAXROUTES];

    if (!sdb->routecount)
      errmsg(&msgsym(COMPRESS, IPRT), sdb);
    for (i = 0; i < MAXROUTES; i++) {
	routes[i] = sdb->rdb[i];
	sdb->rdb[i] = NULL;
    }
    sdb->lastindex = 0;
    for (i = 0, j = 0; i < sdb->routecount; i++) {
	while (j < MAXROUTES && !routes[j]) j++;
	if (j >= MAXROUTES)
	    break;
	rdb = routes[j];
	rdb->robin = ROBIN_DEFAULT;
	rdb->robin_reset = ROBIN_DEFAULT;
	sdb->rdb[i] = rdb;
	routes[j++] = NULL;
    }

    /*
     * Remember a currently active gateway
     */
    if (sdb->routecount) {
	sdb->lastgateway = sdb->rdb[0]->gateway;
	sdb->lastinterface = sdb->rdb[0]->idb;
    }

    /*
     * Possibly select a new best metric for this net/subnet.
     */
    sdb->metric = METRIC_INACCESSIBLE;
    for (i = 0; i < sdb->routecount; i++)
	if (sdb->rdb[i]->metric < sdb->metric)
	    sdb->metric = sdb->rdb[i]->metric;

    /*
     * If this is a network or subnet with more than one route
     * descriptor, possibly adjust round robin count if IGRP
     * derived.  
     */
    ip_robin_setup(INDEX2PDB(sdb->pdbindex), sdb);

    /*
     * Inform redistributing proceeses about the
     * change in this route.
     */
    if (sdb->metric < METRIC_INACCESSIBLE)
	network_redist(INDEX2PDB(sdb->pdbindex), sdb, ROUTE_MODIF);
}

/*
 * ip_valid_splithorizon
 *
 * Perform split horizion checks on a given network.
 * (This is also used by some routing protocols to deal with default
 *  generation.)
 *
 */

inline boolean ip_valid_splithorizon (ipaddrtype dest, ndbtype *db,
				      idbtype *output_idb)
{
    int i;
    rdbtype *rdb;
    idbtype *idb;
    ipaddrtype dummy;

    if (output_idb->ip_split_disabled)
	return(TRUE);

    for (i = 0; i < db->routecount; i++) {
	rdb = db->rdb[i];
	
	/*
	 * If we don't have an interface associated with the rdb,
	 * recursively figure out which interface we would use.
	 */
	idb = rdb->idb;
	if (!idb) {
	    if (rdb->gateway)
		dummy = rdb->gateway;
	    else
		dummy = db->netnumber;
	    idb = iprouting_first_hop(&dummy, dummy, 0, ROUTINGDEPTH,
				       IP_PATH_DEFFLAGS, NULL);
	}
	
	/*
	 * Check for unroutable static or split horizon
	 * Split horizon fails if:
	 *    No interface available for given route
	 *    Interface matches update output interface
	 *    Sending a broadcast update
	 *    Sending update to source of route (if src exist)
	 *    Split horizon is enabled on the interface
	 *    Split horizon flag is set
	 */
	if (!idb) 
	    return(FALSE);
	if ((idb == output_idb) &&
	    !(NDB2PROCTYPE(db) & PROC_NOSPLITMASK)) {
	    if (dest == -1L)
		return(FALSE);
	    if (!(NDB2PROCTYPE(db) & PROC_OSPF)) {
		if (rdb->infosource && (dest == rdb->infosource))
		    return(FALSE);
	    }
	}
    }
    
    /*
     * If 
     *    this is a numbered interface and
     *    the candidate is a major net and
     *	  that major net is on this interface...
     *
     * Note: unnumbered interfaces are a special case, because we
     *	  don't know if the remote side is on our major net or not,
     *    so we send -both- the major net and subnet information down.
     *    It's up the remote side to decide what to do.  If the remote
     *    side screws up, we've got a problem, because we'll get a routing
     *    loop for "non existent" networks.
     */
    if (!output_idb->ip_unnumbered &&
        (db->subnets || get_majormask(db->netnumber) == db->netsmask) &&
	ip_ifnetwork(output_idb, db->netnumber))
	return(FALSE);

    return(TRUE);
}

/*
 * validnextnextwork
 *
 * Determine if a particular network is available for distribution.
 * Called by getnextnetwork().
 */

inline boolean validnextnetwork (
    pdbtype *pdb,
    ndbtype *ndb, 
    ndbtype *sdb,
    idbtype *interface,
    ipaddrtype mask,
    ipaddrtype dest,
    validtesttype test,
    boolean distribute_list_checkflag,
    pdbtype **return_src_pdb)
{
    pdbtype *src_pdb;

    /*
     * ignore the dummy ndb
     */
    if (sdb->attrflags & (NET_SUBNETTED | NET_NOREDIST)) 
	return(FALSE);

    /*
     * find the pdb that is the source protocol
     */
    src_pdb = ip_get_src_pdb(pdb, sdb, FALSE);
    if (src_pdb == NULL)
	return(FALSE);

    if (!(src_pdb->mask & pdb->distflag))
	return(FALSE);

    if (src_pdb->proctype & PROC_OSPF)
	if (!(ospf_check_match_routetype(pdb, src_pdb, sdb)))
	    return(FALSE);
    
    /*
     * Don't advertise major nets out the same numbered interface.  Unless
     * split horizon is disabled, 'natch.
     */
    if (!interface->ip_unnumbered &&
	ndb == sdb &&
	!interface->ip_split_disabled)
	return(FALSE);
    
    if (test && (!(*test)(mask, pdb, sdb)))
	return(FALSE);

    /*
     * Do canonical split horizon processing.
     */
    if (!ip_valid_splithorizon(dest, sdb, interface))
	return(FALSE);
    
    /*
     * Check route filtering (expensive)
     */
    if (distribute_list_checkflag) {
	if (!pdb_outfilter(pdb, interface, src_pdb, sdb->netnumber))
	    return(FALSE);
    }

    /*
     * Check route-maps (very expensive)
     */
    if (!pdb_routemap(pdb, src_pdb, sdb))
	return(FALSE);
    
    *return_src_pdb = src_pdb;
    return(TRUE);
}


/*
 * getnextnetwork
 * Return a pointer to the next network that meets the desired criteria.
 * Skips a network for which any route constitutes an in/out route
 * for the specified interface (split horizon routing) unless that network
 * was discoverd by a protocol such as EGP for which split horizon doesn't
 * apply (NET_NOSPLITMASK).
 *
 * Note well that we don't dereference the common_str_interface variable when
 * checking for split horizons.  Some callers thus set common_str_interface to
 * NULL to defeat the split horizon test.  Beware!
 *
 * Unnumbered interfaces add some interesting effects.  Unnumbered interfaces
 * point at another network attached to this box.  We would really like
 * to advertize this network out the unnumbered interface.  To allow this
 * to happen we must defeat the check for same ndb/sdb check.  Note split
 * horizon checks still apply for routes learned.
 *
 * When the subnetflag is FALSE, i.e we are sending autonomous system route in
 * IGRP or network route in RIP, we only look at major network ndb's.   If
 * the ndb is a dummy, we will look in its subnettable for an eligible
 * subnet that would make us advertised the summary, major network number.
 * This is the only case when distribute_list_checkflag is FALSE.   This flag
 * is passed to validnextnetwork() so distribute-list filter is not applied to
 * each individual subnet.  This is required to preserve the behavior in 9.1
 * where distribute-list filter is applied after the auto-summarization 
 * happens.   route-map is applied to each individual subnet.  This allows 
 * finer granularity of control, in case user wants to send the summary 
 * network ONLY when there is one specific subnet.    
 */

ndbtype *getnextnetwork (
    pdbtype *pdb,
    ndbtype *ndb,
    ndbtype *sdb,
    int *prev,
    register idbtype *interface,
    ipaddrtype mask,
    ipaddrtype dest,
    validtesttype test,
    boolean subnetflag,
    boolean distribute_list_checkflag,
    pdbtype **return_src_pdb)
{
    ndbtype *tdb;
    int subprev;
    
    tdb = NULL;
    /*
     * check next thread on sdb chain
     */
    if (sdb) {
	for (sdb = sdb->next; sdb; sdb = sdb->next) {
	    /*
	     * If sdb is a dummy, find the good subnet under it.   
	     * Check (sdb != ndb) so we don't do it when we are
	     * looking at a dummy ndb of the outgoing interface network
	     * (otherwise we will advertise the major network again while 
	     * we have already advertised the subnets of this network).
	     * (Of course, we *want* to do this for unnumbered links.)
	     */
	    if ((sdb->attrflags & NET_SUBNETTED) &&
		((sdb != ndb) || interface->ip_unnumbered)) {
		subprev = 0;
		tdb = find_valid_subnet(pdb, sdb, NULL, &subprev, 
					interface, mask, dest, test, 
					TRUE, FALSE, return_src_pdb);
	    }
	    if (tdb || 
		validnextnetwork(pdb, ndb, sdb, interface, mask, dest,
				 test, distribute_list_checkflag, 
				 return_src_pdb)) {
		sdb->advertisingmask |= pdb->mask;
		return(sdb);
	    }
	}
    }

    /*
     * find first network, or find next sdb chain
     */

    if (sdb == NULL) {
	while (*prev < NETHASHLEN) {
	    if (subnetflag)
		sdb = ndb->subnettable[*prev];
	    else {
		process_may_suspend();
		sdb = nettable[*prev];
	    }
	    *prev += 1;
	    while (sdb) {
		/*
		 * if sdb is a dummy, find if there is a good subnet under
		 * it.
		 */
		if ((sdb->attrflags & NET_SUBNETTED) &&
		    ((sdb != ndb) || interface->ip_unnumbered)) {
		    subprev = 0;
		    tdb = find_valid_subnet(pdb, sdb, NULL, &subprev, 
					    interface, mask, dest, test, 
					    TRUE, FALSE, return_src_pdb);
		}
		if (tdb ||
		    validnextnetwork(pdb, ndb, sdb, interface, mask, dest,
				     test, distribute_list_checkflag, 
				     return_src_pdb)) {
		    sdb->advertisingmask |= pdb->mask;
		    return(sdb);
		}
		sdb = sdb->next;
	    }
	}
	/*
	 * Don't do supernets if we're only looking at subnets or we're
	 * a dumb routing protocol.
	 */
	if (!(subnetflag || (pdb->proctype & PROC_NOSUPERNETS))) 
	    while (*prev >= NETHASHLEN && *prev < (NETHASHLEN<<1)) {
		process_may_suspend();
		sdb = supernettable[*prev-NETHASHLEN];
		*prev += 1;
		while (sdb) {
		    if (validnextnetwork(pdb,ndb,sdb,interface,mask,
					 dest,test, TRUE, return_src_pdb)) {
			sdb->advertisingmask |= pdb->mask;
			return(sdb);
		    }
		    sdb = sdb->next;
		}
	    }
    }
    return(NULL);
} /* end of getnextnetwork */

/*
 * find_valid_subnet
 * Given a dummy ndb (in sdb argument).  Calls getnextnetwork to look for 
 * an eligible subnet that would make the summary major network advertisable.
 *
 * Note that we apply the distribute-list filter on the dummy ndb
 * network number, instead of the subnet network number because of the
 * requirement that distribute-list filter is applied AFTER
 * auto-summarization had occurred.
 *
 * src_pdb is passed to getnextnetwork and find_valid_subnet to collect the
 * right source protocol that makes us advertised this route.
 */
ndbtype *find_valid_subnet (
    pdbtype *pdb,
    ndbtype *sdb,
    ndbtype *tdb,
    int *subprev,
    register idbtype *interface,
    ipaddrtype mask,
    ipaddrtype dest,
    validtesttype test,
    boolean subnetflag,
    boolean distribute_list_checkflag,
    pdbtype **return_pdb)
{
    pdbtype *src_pdb;

    do {
	tdb = getnextnetwork(pdb, sdb, tdb, subprev, interface,
			     mask, dest, NULL, subnetflag,
			     distribute_list_checkflag, &src_pdb);
	if (tdb && (!test || (*test)(mask, pdb, sdb)) &&
	    pdb_outfilter(pdb, interface, src_pdb, sdb->netnumber)) {
	    *return_pdb = src_pdb;
	    return(tdb);
	}
    } while (tdb);
    
    return(NULL);
} /* end of find_valid_subnet */

/*
 * ndb_is_up
 * Returns TRUE/FALSE as to whether the ndb has any usable rdbs
 * Warning: Calls to ip_route_new_default seem to rely upon this routine
 * to help select a new default route when an interface is going down
 * and the gateway of last resort is via that interface.
 */

boolean ndb_is_up (ndbtype *ndb)
{
    register rdbtype *rdb;
    register idbtype *idb;
    register int i;

    if ((ndb->attrflags & NET_SUBNETTED) == 0) {
	for (i = 0; i < ndb->routecount; i++) {
	    rdb = ndb->rdb[i];
	    if (rdb == NULL)		/* end of list, nothing up */
	        return(FALSE);
	    if (rdb->idb == NULL)	/* static route, always up (?) */
	        return(TRUE);
	    if (interface_up(rdb->idb)) /* definitely something up */
	        return(TRUE);
	}
    } else FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && (idb->ip_majornet == ndb->netnumber)
	    && interface_up(idb))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * ip_route_new_default
 * Find the best or closest exterior gateway.  We first minimize the
 * administrative distance, then pick the network with the lowest
 * metric within that distance.  If we pick a distance of CONNECTED_DISTANCE
 * then we are a directly connected exterior gateway.
 */

void ip_route_new_default (void)
{
    int i, j, k, metric, distance;
    ndbtype *ndb, *best, *extndb;
    ipaddrtype defnet;
    ulong old_attrflags, maj_attrflags;
    
    best = NULL;
    distance = UNKNOWN_DISTANCE;
    metric = METRIC_INACCESSIBLE;
    default_netsource = FALSE;
    for (i = 0; i < NETHASHLEN<<1; i++) {
	ndb = i < NETHASHLEN ? nettable[i] : supernettable[i-NETHASHLEN];
	for (; ndb; ndb = ndb->next) {

	    /*
	     * Scan the list of static defaults, making sure current
	     * routing table is in agreement.  Flag all major nets with
 	     * NET_SEXTERIOR.  If we discover a subnet default, reset the
	     * major network's default subnet.
	     */
	    old_attrflags = ndb->attrflags;
	    maj_attrflags = 0;
	    extndb = NULL;
	    /*
	     * 0.0.0.0 should always has NET_SEXTERIOR set
	     */
	    if (ndb != ndb_zero) {
		if (ndb->attrflags & NET_SUBNETTED) {
		    /*
		     * Find a possible majornet ndb hung off this dummy ndb.
		     * If found, it should also have its sexterior flag
		     * maintained.
		     */
		    extndb = subnet_lookup(ndb, ndb->netnumber,
					   ndb->netsmask);
		    if (extndb) {
			maj_attrflags = extndb->attrflags;
			extndb->attrflags &= ~NET_SEXTERIOR;
		    }
		}
	        ndb->attrflags &= ~NET_SEXTERIOR;
	    }
	    for (j = 0; j < default_netcount; j++) {
	        defnet = default_network[j];
		if (get_majornet(defnet) == ndb->netnumber) {
		    if (ndb->netnumber == defnet) {
			ndb->attrflags |= NET_SEXTERIOR;
			if (extndb)
			    extndb->attrflags |= NET_SEXTERIOR;
		    } else {
			/*
			 * Default network command included subnet number.
			 * Translate this into a subnet default static route.
			 */
			ip_addstatic(ndb->netnumber, get_majormask(defnet),
				     defnet, NULL, 0, 0, STATIC_DISTANCE, 
				     NULL);
			default_netcount--;
			for (k = j; k < default_netcount; k++) 
			    default_network[k] = default_network[k+1];
		    }
		    break;
		}
	    }
	    /*
	     * Notify pdb if this ndb changed its exterior disposition.
	     */
	    if (!iprouting_exterior_change(NULL, ndb, old_attrflags)) {
		/*
		 * Maybe the majornet under the dummy ndb has changed
		 * exterior disposition.
		 */
		if (extndb)
		    iprouting_exterior_change(NULL, extndb, maj_attrflags);
	    }

	    /*
	     * Only nets marked exterior are interesting.  Nets may
	     * be marked as such either in the dummy ndb or in the
	     * majornet route hung off the dummy ndb.  If such a
	     * network is directly connected and statically exterior,
	     * we can be a source of a default network such as used by RIP.
	     * ndb_zero is also a supernet entry and donot consider
	     * it to decide if there is a connected candidate default.
	     */
	    if ((ndb->attrflags & NET_EXTERIOR) == 0) {
		/*
		 * Majornet route hung off of dummy ndb may be DEXTERIOR.
		 */
		if (!extndb)
		    continue;
		if ((extndb->attrflags & NET_EXTERIOR) == 0)
		    continue;
	    } else {
		if (!extndb)
		    extndb = ndb;
	    }
	    if (((extndb->attrflags & (NET_ATTACHED|NET_SEXTERIOR)) ==
		(NET_ATTACHED|NET_SEXTERIOR)) && (extndb != ndb_zero)) {
		    default_netsource = TRUE;
	    } else if ((extndb->distance <= distance) &&
		       (extndb->metric < METRIC_INACCESSIBLE)) {
		if (ndb_is_up(extndb)) {
		    if (extndb->distance < distance) {
			distance = extndb->distance;
			metric = extndb->metric;
			best = extndb;
		    } else {
			if (extndb->metric < metric) {
			    metric = extndb->metric;
			    best = extndb;
			}
		    }
		}
	    }
	}
    }

    /*
     * If we are sourcing the default network, and we have a candidate,
     * and the candidate is NOT a static route, then clear the default_ndb.
     * Don't believe anyone else's dynamic default path information.
     */
    if (default_netsource && best &&
	((NDB2PROCTYPE(best) & PROC_STATIC) == 0)) {
	if (default_ndb) {
	    if (route_debug)
	        buginf("\nRT: default network %i has been cleared",
		       default_ndb->netnumber);
	    network_redist(NULL, default_ndb, ROUTE_DOWN);
	    default_ndb = NULL;
	    /* remove any entries created by the previous default */
	    reg_invoke_ipcache_invalidate_subrange(0, 0, 0);
	}
	return;
    }

    /*
     * Best route has been found or determined not to exist.
     */
    if (route_debug && (best != default_ndb) && best)
	buginf("\nRT: default path is now %i via %i",
	       best->netnumber, best->lastgateway);

    /*
     * This dance is to set the default path if we have found a best path.
     * Reset default_ndb to NULL if no candidates emerged.
     */
    if (best) {
	if (default_ndb != best) {
	    if (route_debug)
		buginf("\nRT: new default network %i", best->netnumber);
	    reg_invoke_ipcache_invalidate_subrange(0, 0, 0);
	    if (default_ndb) {
		network_redist(NULL, default_ndb, ROUTE_DOWN);
	    }
	    default_ndb = best;
	}
	/*
	 * this is called every time route exterior is
	 * called and there is a default to make sure that
	 * OSPF default will have correct metric info.
	 */
	network_redist(NULL, best, ROUTE_UP);
    } else if (default_ndb) {
	if (route_debug)
	    buginf("\nRT: default path has been cleared");
	network_redist(NULL, default_ndb, ROUTE_DOWN);
	default_ndb = NULL;
	reg_invoke_ipcache_invalidate_subrange(0, 0, 0);
    }
}

/*
 * ip_secondary
 * Handle the "[no] ip address <address> <mask> secondary" keyword.
 * This is for routing purposes only, hence the code is in this module.
 * Return an indication of whether or not a change was made.
 */

boolean
ip_secondary (idbtype *idb, boolean addflag,
	      ipaddrtype address, ipaddrtype mask)
{
    ipaddtype *ptr;

    if (idb->ip_address == address) {
	printf("Secondary can't be same as primary");
	return(FALSE);
    }

    /* 
     * Check to see if new address is a duplicate on other interfaces.
     */
    if (addflag && ip_address_duplicate(address, mask, idb, FALSE)) {
        return(FALSE);
    }

    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (ptr->address == address) {
	    reg_invoke_ipcache_invalidate_range(ptr->address, ptr->mask);
	    if (!addflag) {
		unqueue(&idb->ip_addressQ, ptr);
		ip_route_delete(pdb_CONNECTED, ptr->address, ptr->mask,
				IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
		free(ptr);
		arp_flush_cache(idb); 
	    } else if (mask != ptr->mask) {
		ptr->mask = mask;
		(void) route_ifaddress(idb, address, mask);
	    }
	    return(TRUE);
	}
    }
    if (addflag) {
	ptr = malloc(sizeof(ipaddtype));
	if (ptr) {
	    ptr->address = address;
	    ptr->mask = mask;
	    reg_invoke_ipcache_invalidate_range(ptr->address, ptr->mask);
	    enqueue(&idb->ip_addressQ, ptr);
	    (void) route_ifaddress(idb, address, mask);
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * table_walk
 * Walk the table, calling the passed function for each ndb.  The passed
 * function should return the predecessor to the next ndb to look at.
 * This function needs to be really careful about dangling pointers and
 * such.  It assumes that the passed func will not block, but may free the
 * ndb. 
 * Generally the functions should return the predecessor. But in case of
 * variably subnetted ndb, the pruning could leave the summary majornet
 * alone. Hence the ndb is not subnetted anymore and NULL is returned 
 * by the function.
 */

void table_walk (table_walk_proc_type func, void *param)
{
    ndbtype *ndb, *pred, *sdb, *spred;
    int i, j;

    for (i = 0; i < NETHASHLEN<<1; i++) {
	process_may_suspend();
	pred = i < NETHASHLEN ? (ndbtype *) &nettable[i]
			      : (ndbtype *) &supernettable[i-NETHASHLEN];
	for (ndb = pred->next; ndb; ndb = pred->next) {
	    if (!(ndb->attrflags & NET_SUBNETTED))
		pred = (* func)(pred, ndb, NULL, param);
	    else {
		mem_lock(ndb);
		for (j = 0; j < NETHASHLEN; j++) {
		    spred = (ndbtype *) &ndb->subnettable[j];
		    for (sdb = spred->next; sdb; sdb = spred->next) {
			spred = (* func)(spred, sdb, ndb, param);
			if (mem_refcount(ndb) == 1) {
			    ndb_free(ndb, FALSE);
			    goto nextndb;
			}
			/*
			 * If spred is null, it indicates that ndb is
			 * not subnetted anymore. In that case, we need
			 * to process the ndb that become unsubnetted.
			 */
			if (!spred) {
			    pred = (* func)(pred, ndb, NULL, param);
			    ndb_free(ndb, FALSE);
			    goto nextndb;
			}
		    }
		}
		pred = ndb;
		ndb_free(ndb, FALSE);
	    }
	nextndb:
	}
    }
}

/*
 * prune_ndb_idb
 * Prune an ndb of all routes to the interface idb.
 */

ndbtype *prune_ndb_idb (ndbtype *pred, ndbtype *db, ndbtype *ndb,
			void *param)
{
    boolean changed;
    int j, count;
    rdbtype *rdb;
    idbtype *idb;
    
    /*
     * Next, check routes to this network if any.  Delete any that
     * point out the specified interface.
     */
    changed = FALSE;
    count = db->routecount;
    idb = (idbtype *) param;
    for (j = 0; j < count; j++) {
	if (idb == db->rdb[j]->idb) {
	    rdb = db->rdb[j];
	    if (rdb->rdb_flags & IP_RDB_PERMANENT)
		continue;
	    db->rdb[j] = NULL;
	    if (acl_route_debug(db->netnumber, db->netsmask))
		buginf("\nRT: delete route to %i via %i, %s", 
		       db->netnumber, rdb->gateway, idb->namestring);
	    rdb_free(rdb, TRUE);
	    changed = TRUE;
	    db->routecount--;
	}		    
    }
    
    /*
     * If the routes have changed, maybe this db goes away or we
     * compress his routes.
     */
    if (changed || db->lastinterface == idb) {
	iprouting_cache_clean(db);
	if (db->routecount == 0) {
	    if (acl_route_debug(db->netnumber, db->netsmask))
		buginf("\nRT: no routes to %i, flushing",
		       db->netnumber);
	    pred->next = db->next;
	    if (ndb) 
		ip_free_sdb(ndb, db, &pred);
	    else
		ip_free_ndb(db);
	    return(pred);
	} else {
	    route_compress(db);
	}
    }
    return(db);
}

/*
 * table_prune_idb
 * Prune an interface from the routing table.
 */

void table_prune_idb (idbtype *idb)
{
    table_walk(prune_ndb_idb, (void *) idb);
    reg_invoke_ipcache_invalidate(hwidb_or_null(idb), NULL, FALSE);

    /*
     * Poke the dynamic routing protocols if we're a running system.
     */
    if (systeminit_flag)
	table_reload(idb);
}

/*
 * prune_ndb_pdb
 * Prune all ndbs learned from the given protocol.
 *
 * Prune all backup information pertaining to the given protocol.
 */

ndbtype *prune_ndb_pdb (ndbtype *pred, ndbtype *db, ndbtype *ndb,
			void *param)
{
    pdbtype *pdb;
    backuptype *backup, **backup_prev;

    pdb = (pdbtype *)param;

    /*
     * Next, check this ndb.
     */
    if (db->pdbindex == pdb->index) {
	ip_free_all_rdb(db);
	if (acl_route_debug(db->netnumber, db->netsmask))
	    buginf("\nRT: delete route to %i%m", 
		   db->netnumber, db->netsmask);
	pred->next = db->next;
	if (ndb)
	    ip_free_sdb(ndb, db, &pred);
	else
	    ip_free_ndb(db);
	return(pred);
    }

    /* Flush any backup entries corresponding to this PDB. */

    backup_prev = &db->backup_info;
    while ((backup = *backup_prev)) {
	if (backup->pdbindex == pdb->index) { /* It matches */
	    *backup_prev = backup->next; /* Unhook it. */
	    backup->next = NULL;
	    backup_free(backup, TRUE);	/* Free it */
	} else {
	    backup_prev = &backup->next;
	}
    }

    /*
     * cleanup the dummy_ndb's pdbindex so that it no longer references
     * the pdb that is about to be freed up.  Have it point to a child
     * sdb's pdbindex instead.
     */
    if (ndb != NULL) {
	if ((ndb->attrflags & NET_SUBNETTED) && (ndb->pdbindex == pdb->index))
	    ndb->pdbindex = db->pdbindex;
    }
    return(db);
}

/*
 * table_prune_pdb
 * Prune a process from the routing table.
 */

void table_prune_pdb (pdbtype *pdb)
{
    table_walk(prune_ndb_pdb, pdb);
    reg_invoke_ipcache_invalidate(NULL, NULL, FALSE);
}

/*
 * ip_ndb_subnet
 * Given an ndb which is not subnetted, allocate a new one which is subnetted
 * and copy over enough information to subnet it.  Save the old one as a
 * major mask subnet and return the new one.
 */

ndbtype *ip_ndb_subnet (ndbtype *ndb)
{
    ndbtype *nndb, *pred;
    int hash;

    iprouting_cache_clean(ndb);

    nndb = ndb_malloc(sizeof(ndbtype) + (NETHASHLEN-1)*(sizeof(ndbtype *)));
    if (!nndb)
	return(NULL);
    nndb->attrflags = NET_SUBNETTED;
    nndb->attrflags |= (ndb->attrflags & NET_ATTACHED);
    nndb->netsmask = ndb->netsmask;
    nndb->pdb_overmask = ndb->pdb_overmask;
    nndb->pdbindex = ndb->pdbindex;
    nndb->netnumber = ndb->netnumber;
    nndb->attachcount = ndb->attachcount;
    hash = nethash(ndb->netnumber);
    pred = (ndbtype *) &nettable[hash];
    for (; pred && (pred->next != ndb); pred = pred->next);
    if (!pred) {
	buginf("\nRT: error subnetting %i", ndb->netnumber);
	ndb_free(nndb, TRUE);
	return(NULL);
    }
    pred->next = nndb;
    nndb->next = ndb->next;
    ndb->next = NULL;
    ndb->attrflags &= ~NET_SUBNETTED;
    ndb->attrflags |= NET_SUBNET;
    nndb->subnettable[hash] = ndb;
    nndb->subnets++;
    return(nndb);
}


/*
 * ip_ndb_variable
 * Take an ndb which is currently subnetted and set it up for variable
 * length subnetting.  Return FALSE on error, TRUE on success.
 */

boolean ip_ndb_variable (ndbtype *ndb)
{
    masktype *table;

    table = malloc(MAXMASKS * sizeof(masktype));
    if (!table)
        return(FALSE);
    ndb->attrflags |= NET_VARIABLE;
    table[0].mask = ndb->netsmask;
    table[0].count = ndb->subnets;
    ndb->netsmask = get_majormask(ndb->netnumber);
    ndb->masktable = table;
    ndb->maskcount = 1;
    if (acl_route_debug(ndb->netnumber, ndb->netsmask))
	buginf("\nRT: network %i is now variably masked", ndb->netnumber);
    return(TRUE);
}

/*
 * ip_ndb_addmask
 * Add a netmask (and increment the reference count) for a variably subnettted
 * ndb.  The mask may already be in the table.
 */

void ip_ndb_addmask (ndbtype *ndb, ipaddrtype mask)
{
    masktype *table;
    int i, j;

    if (!(ndb->attrflags & NET_VARIABLE)) {
        buginf("\nRT: Adding a mask to %i, not variably subnetted.",
	       ndb->netnumber);
	return;
    }

    table = ndb->masktable;
    for (i = 0; i < ndb->maskcount; i++) {
        if (table[i].mask == mask) {
	    table[i].count++;
	    return;
	}
	if (table[i].mask < mask) {

	    /*
	     * Need to insert a new mask.
	     */
	    for (j = ++ndb->maskcount - 1; j > i; j--)
		table[j] = table[j-1];
	    table[i].mask = mask;
	    table[i].count = 1;

	    /*
	     * Blow away _all_ of the cache entries under this NDB if we're
	     * creating a new mask that is longer than the existing masks,
	     * as they need to be re-created with the longest mask (we can't
	     * backtrack up the route cache tree, as it's too slow).
	     */
	    if (i == 0)
		reg_invoke_ipcache_invalidate_range(ndb->netnumber,
						    ndb_mask(ndb));

	    return;
	}
    }
    i = ndb->maskcount++;
    table[i].mask = mask;
    table[i].count = 1;

    /*
     * Blow away _all_ of the cache entries under this NDB if we're
     * creating a new mask that is longer than the existing masks,
     * as they need to be re-created with the longest mask (we can't
     * backtrack up the route cache tree, as it's too slow).
     */
    if (i == 0)
	reg_invoke_ipcache_invalidate_range(ndb->netnumber, ndb_mask(ndb));
}

/*
 * ip_supernet_addmask
 * Add a netmask (and increment the reference count)
 * The mask may already be in the table.
 */

void ip_supernet_addmask (ipaddrtype mask)
{
    int i, j;

    for (i = 0; i < supernetmaskcount; i++) {
        if (supernetmasks[i].mask == mask) {
	    supernetmasks[i].count++;
	    return;
	}
	if ((supernetmasks[i].mask & mask) != mask) {
	    if (supernetmaskcount == MAXMASKS) {
		buginf("\nRT: trying to insert too many masks into "
		       "supernet table");
		return;
	    }
	    /*
	     * Need to insert a new mask.
	     */
	    for (j = ++supernetmaskcount - 1; j > i; j--)
		supernetmasks[j] = supernetmasks[j-1];
	    supernetmasks[i].mask = mask;
	    supernetmasks[i].count = 1;
	    return;
	}
    }

    if (supernetmaskcount == MAXMASKS) {
	buginf("\nRT: trying to append too many masks to supernet table");
	return;
    }

    i = supernetmaskcount++;
    supernetmasks[i].mask = mask;
    supernetmasks[i].count = 1;
}

/*
 * ip_ndb_unsubnet
 * Given an ndb with only a single subnet, which happens to have a major
 * mask associated with it, convert it back to a normal major net route.
 * Do this by _carefully_ copying the sdb on top of the ndb and freeing the
 * sdb.  This wastes the subnet table at the end of what now looks to be an
 * unsubnetted ndb.
 */

static void ip_ndb_unsubnet (ndbtype *ndb)
{
    ndbtype *sdb;
    int hash;
    pdbtype *pdb;

    if (ndb->subnets != 1) {
	buginf("\nRT: Trying to unsubnet %i, which has %d subnets",
	       ndb->netnumber, ndb->subnets);
	return;
    }
    sdb = subnet_lookup(ndb, ndb->netnumber, ndb->netsmask);
    if (!sdb) {
	buginf("\nRT: Trying to unsubnet %i, which has no major subnet",
	       ndb->netnumber);
	return;
    }
    hash = nethash(sdb->netnumber);
    ndb->subnettable[hash] = NULL;	/* disconnect sdb */
    sdb->next = ndb->next;		
    bcopy(sdb, ndb, sizeof(ndbtype));
    if (default_ndb == sdb)
	default_ndb = ndb;		/* Move default route over to new ndb*/

    pdb = INDEX2PDB(ndb->pdbindex);
    if (pdb->ndb_copy)
	(*pdb->ndb_copy)(pdb, sdb, ndb);

    sdb->next = NULL;
    ndb_free(sdb, TRUE);
    ndb->attrflags &= ~NET_SUBNET;
    ndb->subnets = 0;
}

/*
 * ip_ndb_removemask
 * Remove a netmask.  If it's no longer variably subnetted, then clean up
 * the unnecessary data structure.
 */

void ip_ndb_removemask (ndbtype *ndb, ipaddrtype mask)
{
    int i, j;
    masktype *table;

    table = ndb->masktable;
    if (!(ndb->attrflags & NET_VARIABLE) || !table) {
	buginf("\nRT: Trying to remove a mask from net %i, not variable",
	    ndb->netnumber);
	return;
    }

    for (i = 0; i < ndb->maskcount; i++) {
        if (table[i].mask == mask) {
	    if (--table[i].count == 0) {
		/*
		 * Blow away the cache for this entire ndb if we're removing
		 * the last entry of the longest mask present.
		 */
		if (i == 0)
		    reg_invoke_ipcache_invalidate_range(ndb->netnumber,
							ndb_mask(ndb));
	        if (--ndb->maskcount == 1) {
		    ndb->netsmask = table[1-i].mask;
		    ndb->attrflags &= ~NET_VARIABLE;
		    free(table);	
		    ndb->masktable = NULL;
		    ndb->maskcount = 0;
		    if (ndb->netsmask == get_majormask(ndb->netnumber))
			ip_ndb_unsubnet(ndb);
		    return;
		}
		for (j = i; j <= ndb->maskcount; j++) 
		    table[j] = table[j+1];
	    }
	    return;
	}
    }
    buginf("\nRT: trying to remove non-existent mask%#m from %i", mask,
	   ndb->netnumber);
}

/*
 * ip_supernet_removemask
 * Remove a netmask.  If it's no longer variably subnetted, then clean up
 * the unnecessary data structure.
 */

void ip_supernet_removemask (ipaddrtype mask)
{
    int i, j;

    for (i = 0; i < supernetmaskcount; i++) {
        if (supernetmasks[i].mask == mask) {
	    if (--supernetmasks[i].count == 0) {
		for (j = i; j <= supernetmaskcount; j++) 
		    supernetmasks[j] = supernetmasks[j+1];
		supernetmaskcount--;
	    }
	    return;
	}
    }
    buginf("\nRT: trying to remove non-existent mask%#m from supernet table",
	   mask);
}

/*
 * check_backup_info
 *
 * Check to see if pdb has any routes in the backup queue for this 
 * destination.
 */
backuptype *check_backup_info (pdbtype *pdb, ndbtype *ndb)
{

    backuptype *backup;

    for (backup = ndb->backup_info; backup; backup = backup->next) {
	if (backup->pdbindex == pdb->index) {
	    break;
	}
    }
    return(backup);
}

/*
 * build_backup_info
 *
 * Allocate a backup route and insert it in chain for this ndb.
 *
 * If a backup entry already exists for this protocol, it is cleaned
 * out and re-used.
 *
 * Calls the protocol-specific callback routine, if any, to build context
 * information for the backup entry.
 *
 * If "replaced" is TRUE, the route is being replaced by a route from a
 * protocol with better administrative distance.  If FALSE, the protocol
 * tried to install a route but there was a route with better administrative
 * distance already in place.
 */
backuptype *build_backup_info (pdbtype *pdb,
			       ndbtype *ndb,
			       ushort route_type,
			       boolean replaced)
{

    backuptype *backup;

    /*
     * Flag ndb, that this pdb needs callback.
     */
    ndb->backupmask |= pdb->mask;

    /*
     * Some protocols don't want backup entries allocated. This assumes
     * pdb->add_route has already been checked for non-NULL.
     */
    if (!pdb->malloc_backup)
	return(NULL);

    /*
     * Grab an old one if there, and clean it out.  Malloc a new one if there
     * isn't one there.
     */
    backup = check_backup_info(pdb, ndb);
    if (backup) {
	if (pdb->backup_cleanup)
	    (*pdb->backup_cleanup)(pdb, backup);
    } else {
	backup = malloc(sizeof(backuptype));
	if (!backup)
	    return(NULL);
    }

    /* Fill it in. */

    backup->pdbindex = pdb->index;
    backup->route_type = route_type;
    if (ndb->backup_info)
	backup->next = ndb->backup_info;
    ndb->backup_info = backup;
    if (pdb->build_backupinfo) {
	(*pdb->build_backupinfo)(pdb, ndb, backup, route_type, replaced);
    }
    return(backup);
}

/*
 * geteachroute
 *
 * Traverse the IP routing table. Get every entry from the IP routing table.
 * It calls process_may_suspend() at the end of a ndb chain if should_suspend
 * is TRUE.
 *
 * If the structure of the IP routing table changes, hopefully this
 * will be the only function that needs to change for IS-IS and IP-IGRP2,
 * OSPF, and BGP.
 */
ndbtype *geteachroute (ndbtype **ndbptr, int *ndb_bucket,
		       ndbtype **sdbptr, int *sdb_bucket,
		       boolean should_suspend)
{
    int i, j;

    for (i = *ndb_bucket; i < NETHASHLEN; i++) {
	*ndb_bucket = i;

	/*
	 * If calling function supplies ndb, use next pointer in linked
	 * list. Otherwise, get next hash location. All this occurs if
	 * we are done looking at all the subnets of the last ndb.
	 */
	if (!*sdbptr) {
	    if (*ndbptr) {
		*ndbptr = (*ndbptr)->next;
	    } else {
		*ndbptr = nettable[i];
	    }
	}

	/*
	 * Have ndb, traverse subnets for ndb if a subnetted network.
	 * Otherwise, return this ndb.
	 */
	while (*ndbptr && (*ndbptr)->attrflags & NET_SUBNETTED) {
	    for (j = *sdb_bucket; j < NETHASHLEN; j++) {
		*sdb_bucket = j;

		/*
		 * If calling function supplies sdb, use next pointer in 
		 * linked list. Otherwise, get next hash location.
		 */
		if (*sdbptr) {
		    *sdbptr = (*sdbptr)->next;
		} else {
		    *sdbptr = (*ndbptr)->subnettable[j];
		}

		/*
		 * Go to next hash location in subnet table.
		 */
		if (!*sdbptr) continue;

		/*
		 * This is the next subnet in the traversal, return. Buckets 
		 * and ndbptr are already updated.
		 */
		return(*sdbptr);
	    }

	    /*
	     * Went through all subnets, record the fact to caller, and go
	     * get next ndb.
	     */
	    *sdbptr = NULL;
	    *sdb_bucket = 0;
	    *ndbptr = (*ndbptr)->next;
	    if (!*ndbptr)
		break;
	}

	/*
         * This is the next network in the traversal, return. All caller
	 * parameters have been updated.  If it is NULL, we had finished
	 * the linked list so we will follow the for loop to the next
	 * hash location but check whether we should suspend before.
         */
	if (*ndbptr) {
	    return(*ndbptr);
	} else {

	    /*
	     * Go to next hash location after suspension, if requested by
	     * caller.
	     */
	    if (should_suspend)
		process_may_suspend();
	}
    }

    /*
     * Now traverse the supernet table
     */
    for (; i < (NETHASHLEN<<1); i++) {
	*ndb_bucket = i;

	if (*ndbptr)
	    *ndbptr = (*ndbptr)->next;
	else
	    *ndbptr = supernettable[i-NETHASHLEN];

	/*
	 * Go to next hash location.
	 */
	if (!*ndbptr) {
	    if (should_suspend)
		process_may_suspend();
	    continue;
	}

	/*
	 * This is the next network in the traversal, return. All caller
	 * parameters have been updated.
	 */
	return(*ndbptr);
    }

    return(NULL);
}

/*
 * route_add_neighbor
 * Add a neighbor route
 */

boolean route_add_neighbor (idbtype *idb, ipaddrtype addr)
{
    if (interface_up(idb) && addr &&
        route_ifaddress(idb, addr, MASK_HOST)) {
        adjust_static_routes(NULL, NULL);
        return TRUE;
    }
    return FALSE;
}

/*
 * callbackup_routes
 * Calls all protocols that tried to add routes to this entry and failed.
 * The route, for the protocol they failed to add, has gone unreachable.
 */
void callbackup_routes (ndbtype *ndb)
{
    backuptype *backup, local_backup;
    backuptype *temp_backup;
    pdbtype *pdb;

    /*
     * Process the protocols that use backup callback but don't malloc
     * entries.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (!pdb->add_route)
	    continue;
	if (!(ndb->backupmask & pdb->mask)) 
	    continue;
	if (!pdb->malloc_backup) {
	    memset(&local_backup, 0, sizeof(local_backup));
	    local_backup.route_type = ndb->route_type;
	    (*pdb->add_route)(pdb, ndb->netnumber, ndb->netsmask,
			      &local_backup);
	}
    }

    /*
     * Process the protocols that do use backup callback but do malloc
     * entries.
     */
    if (ndb->backup_info == NULL)
	return;

    for (backup = ndb->backup_info; backup; backup = temp_backup) {
	temp_backup = backup->next;
	pdb = INDEX2PDB(backup->pdbindex);
	if (pdb && pdb->add_route)
	    (*pdb->add_route)(pdb, ndb->netnumber, ndb->netsmask, backup);
	backup_free(backup, TRUE);
    }
    ndb->backup_info = NULL;
}

/*
 * ip_route_delete
 * Remove a network or subnet route.
 * If the last subnet route is removed, the network is flushed.
 * The routing cache is also invalidated.
 * The pdb is required.
 * The mask may be 0, in which case we will perform longest match.
 * The gateway, source and interface and route_type arguments are optional.
 * If specified, only rdb's with matching next hops, infosource or output 
 * interfaces will be removed.
 */

void ip_route_delete (pdbtype *pdb, ipaddrtype address, ipaddrtype mask,
		      ipaddrtype gateway, ipaddrtype source,idbtype *interface,
		      ushort route_type)
{
    ndbtype *ndb, *sdb, *pred, *target, *tdb;
    int i, deleted;
    rdbtype *rdb;
    ipaddrtype majornet;

    majornet = get_majornet(address);

    tdb = netsuper_lookup(address, mask, FALSE);
    if (!tdb || tdb->netnumber != majornet)
	return;

    pred = tdb->attrflags & NET_SUPERNET ?
	(ndbtype *) &supernettable[nethash(address)] :
	    (ndbtype *) &nettable[nethash(majornet)];
       
    ndb = pred->next;
    while (ndb) {
	if (ndb == tdb) {
	    if (ndb->attrflags & NET_SUBNETTED) {
		target = subnet_lookup(ndb, address, mask);
		if (!target)
		    return;
		pred = (ndbtype *)
		    & ndb->subnettable[nethash(address & target->netsmask)];
		sdb = pred->next;
		while (sdb) {
		    if (sdb == target) {
			if (pdb->index != sdb->pdbindex)
			    return;
 			if (route_type && (sdb->route_type != route_type))
 			    return;
			deleted = 0;
			for (i = 0; i < MAXROUTES; i++) {
			    rdb = sdb->rdb[i];
			    if (!rdb)
				continue;
			    if (gateway &&
				(rdb->gateway != gateway))
				continue;
			    if (source &&
				(rdb->infosource != source))
				continue;
			    if (interface &&
				(rdb->idb != interface))
				continue;
			    if (acl_route_debug(sdb->netnumber, sdb->netsmask))
				buginf("\nRT: del %i%m via %i,"
				       " %s metric [%d/%d]", 
				       sdb->netnumber, sdb->netsmask,
				       rdb->gateway, pdb->name,
				       sdb->distance, rdb->metric);
			    deleted++;
			    rdb_free(rdb, TRUE);
			    sdb->rdb[i] = NULL;
			    sdb->routecount--;
			}

			/*
			 * Only clean the cache if we actually deleted
			 * a route or if there are zero routes for
			 * this sdb as we are probably in or just out
			 * of holddown.
			 */
			if (deleted || (sdb->routecount == 0))
			    iprouting_cache_clean(sdb);

			if (sdb->routecount == 0) {
			    pred->next = sdb->next;
			    if (acl_route_debug(sdb->netnumber, sdb->netsmask))
				buginf("\nRT: delete subnet route to %i%m",
				       sdb->netnumber, sdb->netsmask);
			    ip_free_sdb(ndb, sdb, &pred);
			} else
			    route_compress(sdb);
			return;
		    }
		    pred = sdb;
		    sdb = pred->next;
		}
		return;
	    }
	    /*
	     * Before deleting make sure it is the correct entry
	     */
	    if (mask && 
		((ndb->netsmask != mask) ||
		 (ndb->netnumber != (address & mask))))
		return;
	    if (pdb->index != ndb->pdbindex)
		return;
	    if (route_type && (ndb->route_type != route_type))
		return;
	    deleted = 0;
	    for (i = 0; i < MAXROUTES; i++) {
		rdb = ndb->rdb[i];
		if (!rdb)
		    continue;
		if (gateway &&
		    (rdb->gateway != gateway))
		    continue;
		if (source &&
		    (rdb->infosource != source))
		    continue;
		if (interface &&
		    (rdb->idb != interface))
		    continue;
		if (acl_route_debug(ndb->netnumber, ndb->netsmask))
		    buginf("\nRT: del %i via %i, %s"
			   " metric [%d/%d]", 
			   ndb->netnumber, rdb->gateway,
			   pdb->name, ndb->distance,
			   rdb->metric);
		rdb_free(rdb, TRUE);
		ndb->rdb[i] = NULL;
		ndb->routecount--;
		deleted++;
	    }

	    /*
	     * Only clean the cache if we actually deleted a route or
	     * if there are zero routes for this ndb as we are
	     * probably in or just out of holddown.
	     */
	    if (deleted || (ndb->routecount == 0))
		iprouting_cache_clean(ndb);

	    if (ndb->routecount == 0) {
		pred->next = ndb->next;
		if (acl_route_debug(ndb->netnumber, ndb->netsmask))
		    buginf("\nRT: delete network route to %i", ndb->netnumber);
		ip_free_ndb(ndb);
	    } else 
		route_compress(ndb);
	    return;
	}
	pred = ndb;
	ndb = pred->next;
    }
}

/*
 * ip_robin_setup
 * Calculate round robin values for an ndb.  Assumes that sdb->metric is
 * already set to the optimal metric. We implement multiple algorithms for
 * determining the traffic share over the set of paths.
 */

void ip_robin_setup (pdbtype *pdb, ndbtype *sdb)
{
    int i, j;
    rdbtype *rdb;
    ulong largest, temp;
    int count;

    count = sdb->routecount;
    if ((count < 2) || !(NDB2PROCTYPE(sdb) & (PROC_IGRP | PROC_IGRP2)))
	return;

    switch (pdb->traffic_share) {
    case TRAFFIC_BALANCED:
    default:
	largest = 0;
	for (i = 0; i < count; i++) {
	    rdb = sdb->rdb[i];
	    if (rdb->metric > largest)
		largest = rdb->metric;
	}
	for (i = 0; i < count; i++) {
	    rdb = sdb->rdb[i];
	    if (rdb->metric) {
		temp = (largest << 1)/rdb->metric;
		temp = (temp & 1) ? temp + 1 : temp;
		rdb->robin_reset = max(1, (temp >> 1));
	    } else 
		rdb->robin_reset = ROBIN_DEFAULT;
	    rdb->robin = rdb->robin_reset;
	}
	break;
    case TRAFFIC_MIN:
	for (i = 0; i < count; i++) {
	    rdb = sdb->rdb[i];
	    if (rdb->metric == sdb->metric) {
		if (rdb->robin_reset != ROBIN_DEFAULT) {
		    rdb->robin_reset = ROBIN_DEFAULT;
		    rdb->robin = rdb->robin_reset;
		}
	    } else {
		if (rdb->robin_reset != 0) {
		    iprouting_cache_clean(sdb);
		    rdb->robin_reset = 0;
		    rdb->robin = rdb->robin_reset;
		}
		if (i == sdb->lastindex) {
		    /*
		     * Current pointer to next route is now bogus.
		     */
		    for (j = 0; j < count; j++) {
			if (sdb->rdb[j]->metric == sdb->metric) {
			    sdb->lastindex = j;
			    break;
			}
		    }
		}
	    }
	}
	break;
    }
}

/*
 * iprouting_distcmp_generic
 * Compare administrative distances in the traditional manner.
 * Return an indication of whether or not 'our' distance is better than
 * 'their' distance.
 */

enum DISTTYPE iprouting_distcmp_generic (pdbtype *pdb, uchar our_dist,
					 ndbtype *db, ulong metric,
					 ushort route_type)
{
    return(iprouting_distcmp_inline(our_dist, db->distance));
}

/*
 * iprouting_background
 * Periodically, and on demand, revise the gateway of last resort, and 
 * IP static routes.
 */

static process iprouting_background (void)
{
    ulong major, minor;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    process_watch_boolean(ip_statics_unsynced, ENABLE, RECURRING);
    process_watch_boolean(iprouting_new_default_needed, ENABLE, RECURRING);
    TIMER_START(iprouting_background_timer, ONEMIN);
    process_watch_timer(&iprouting_background_timer, ENABLE);

    while (TRUE) {
	process_wait_for_event();

	process_get_wakeup(&major, &minor);

	ip_static_adjust();

	/*
	 * Recompute gateway of last resort:
	 *  - periodically
	 *  - on demand, but intentionally right after the static routes
	 *    which the gateway of last resort may depend on have been revised
	 *
	 * Note that we restart the timer for one minute from now rather than
	 * one minute from that last time set.  This covers the case where we
	 * are told to recompute the gateway on demand.
	 */
	if ((major == TIMER_EVENT) ||
	    process_get_boolean(iprouting_new_default_needed)) {
	    process_set_boolean(iprouting_new_default_needed, FALSE);
	    ip_route_new_default();
	    TIMER_START(iprouting_background_timer, ONEMIN);
	}
    }
}

/*
 * iprouting_background_onoff
 * Start/stop the IP routing background process.
 */

void iprouting_background_onoff (boolean on)
{
    static int pid = NO_PROCESS;

    if (on) {
	if (pid == NO_PROCESS) {
	    pid = process_create(iprouting_background, "IP-RT Background",
				 NORMAL_STACK, PRIO_NORMAL);
	}
    } else {
	if (pid != NO_PROCESS) {
	    process_kill(pid);
	    pid = NO_PROCESS;
	}
    }
}

/*
 * iprouting_cache_mask
 * Given a destination address, compute the mask (if any) that comes from a
 * route lookup.  Return TRUE if a mask exists.
 *
 * There are a lot of sneaky things in here with respect to maintaining the
 * IP route cache.  One of the more subtle is that supernet cache entries are
 * never created with a mask size greater than the majormask.  This is done
 * so that it's inexpensive to always look at the smallest subnet mask in a
 * given target range, otherwise the cache could get populated with a
 * supernet entry and then never populated with a more specific host route
 * (as an example).
 * When we add a new route to the system, we are careful to blow away the
 * more specific and less specific entries covering that route,  but we don't
 * always -populate- the cache in order so you need to be careful to not
 * allow overlapping entries in the cache or it won't get populated.
 */

boolean iprouting_cache_mask (ipaddrtype address,
			      ipaddrtype *mask,
			      ipaddrtype *creator_mask)
{
    ndbtype *ndb, *sdb;

    ndb = netsuper_lookup_inline(address, 0, FALSE);
    if (!ndb) {
	/*
	 * If no net, we would follow default.
	 */
	if (default_ndb) {
	    *mask = get_majormask(address);
	    *creator_mask = 0;
	    return(TRUE);
	}
	return(FALSE);
    }

    /*
     * If it is not subnetted, then return the majormask if it is remote,
     * or a host mask if it is connected.
     */
    if (!(ndb->attrflags & NET_SUBNETTED)) {
	if ((ndb->attrflags & NET_ATTACHED) || (ndb->routecount != 1))
	    *mask = MASK_HOST;
	else
	    *mask = get_majormask(address);
	*creator_mask = ndb->netsmask;
	return(TRUE);
    }

    /*
     * If the network is variably subnetted, then use the longest mask
     * that this network has (otherwise there might be overlapping routes
     * with shorter masks).
     */
    *mask = ndb->attrflags & NET_VARIABLE ?
	ndb->masktable[0].mask : ndb->netsmask;

    /*
     * If the network is subnetted, then look at the subnet.  If the subnet
     * is attached, then we can't do better than a host route.  If there
     * is more than one route, then return a host mask for load balancing.
     */
    sdb = subnet_lookup(ndb, address, 0);
    if (sdb) {
	if ((sdb->attrflags & NET_ATTACHED) || (sdb->routecount > 1))
	    *mask = MASK_HOST;
	*creator_mask = sdb->netsmask;
    } else {
	/*
	 * Whoops, we're a a disconnected subnet.  Look up the supernet
	 * that covers us and set our creator_mask as appropriate.
	 */
	ndb = supernet_lookup_inline(address, 0, FALSE);
	if (!ndb) {
	    if (default_ndb) {
		*creator_mask = 0;
		return(TRUE);
	    }
	    return(FALSE);	/* this can't happen in reality */
	}
	*creator_mask = ndb->netsmask;
    }
    return(TRUE);
}

/*
 * iprouting_recursive
 * Determine if the path to a given destination requires recursive routing.
 */
boolean iprouting_recursive(ipaddrtype destination)
{
    ndbtype *db;
    int i;

    db = ip_rtlookup(destination, 0);
    if (!db) {
	if (!default_ndb)
	    return(FALSE);
	db = default_ndb;
    }

    for (i = 0; i < db->routecount; i++)
	if (db->rdb[i]->idb == NULL)
	    return (TRUE);

    return(FALSE);
}

/*
 * iprouting_nexthop_valid
 *
 * This is the guts of iprouting_destination_valid().
 */ 
static boolean iprouting_nexthop_valid (ipaddrtype destination,
					ipaddrtype old_nexthop,
					idbtype *old_idb)
{
    ipaddrtype new_nexthop;
    idbtype *new_idb;
    ndbtype *db;
    rdbtype *rdb;
    int i;

    new_nexthop = destination;
    db = NULL;

    new_idb = iprouting_first_hop(&new_nexthop, destination, 0, ROUTINGDEPTH,
				  IP_PATH_USE_PROXY, &db);

    if (!new_idb)
	return(FALSE);

    /*
     * Interface static routes can generate a new_nexthop of 0.0.0.0.
     */
    if (((new_nexthop == old_nexthop) ||
	 (0 == new_nexthop)) &&
	(new_idb == old_idb))
	return (TRUE);

    if (!db)
	return(FALSE);

    for (i = 0; i < db->routecount; i++) {
	rdb = db->rdb[i];

	/*
	 * Interface static routes have rdb->gateway == 0.
	 */
	if (((rdb->gateway == old_nexthop) ||
	     (0 == rdb->gateway)) &&
	    (rdb->idb == old_idb))
	    return(TRUE);
    }

    return (FALSE);

}

/*
 * iprouting_destination_valid
 *
 * Check and see if a recursive route still points to the same next-hop
 * and destination as we've supplied.  If they're the same, return true;
 * if the destination is no-longer valid, return false
 */

boolean iprouting_destination_valid (ipaddrtype destination,
				     ipaddrtype old_nexthop,
				     idbtype *old_idb)
{
    ndbtype *db;
    rdbtype *rdb;
    boolean result;
    int i;

    /*
     * If the initial lookup yields a route with more than one path,
     * check each one of them separately as iprouting_first_hop() can
     * return different ndbs in round-robin fashion causing us to
     * incorrectly believe this entry is no longer valid.
     *
     * Note: incorrect invalidations can still occur if more than one 
     * level of recursion is required to find a connected route and
     * one or more of those routes has multiple paths.
     */

    db = netsuper_lookup(destination, 0, TRUE);

    if (db && (db->routecount > 1)) {

	for (i = 0; i < db->routecount; i++) {
	    rdb = db->rdb[i];
	    if (rdb->idb) {
		/*
		 * If the route is directly connected, see if the
		 * nexthop matches. Interface static routes have
		 * rdb->gateway == 0. 
		 */
		if (((rdb->gateway == old_nexthop) ||
		     (0 == rdb->gateway)) &&
		    (rdb->idb == old_idb))
		    return(TRUE);
		else
		    continue;
	    } else {			/* Recursive route */
		result = iprouting_nexthop_valid(rdb->gateway,
						 old_nexthop,
						 old_idb); 
		if (result)
		    return(TRUE);
	    }
	}
    } else
	return(iprouting_nexthop_valid(destination, old_nexthop,
				       old_idb));
    return(FALSE);
    
}

/*
 * ip_route_start_aging_core
 * Protocol independent code that runs down a table (super/net/sub)
 * Starts aging snapshot routes listed
 */
static void ip_route_start_aging_core (pdbtype *pdb, ndbtype *pred,
				idbtype *idb, paktype *pak,
				ipaddrtype source)
{
    register int i;
    ndbtype *sdb;
    rdbtype *rdb;

    for (sdb = pred->next; sdb; pred = sdb, sdb = pred->next) {

	/*
	 * Age only "pdb->mask" maintained subnets or networks.
	 */
	if (NDB2PROCTYPE(sdb) & PROC_NOAGERT)
	    continue;
	if (!ip_own_ndb(sdb, pdb))
	    continue;

	if (sdb->attrflags & NET_SUBNETTED)
	    continue;

	/*
	 * If a subnet is inaccessible, don't play with it.
	 */
	if (sdb->metric == METRIC_INACCESSIBLE) {
	    continue;
	}

	/*
	 * Subnet is accessible.  Start aging any routes that are through
	 * the idb given.
	 */
	for (i = 0; i < sdb->routecount; i++) {
	    rdb = sdb->rdb[i];
	    if ((rdb->idb != idb) ||
		(!(rdb->rdb_flags & IP_RDB_PERMANENT)))
		continue;
	    if ((pak != NULL) && (rdb->infosource != source))
	        continue;
 	    if (rdb->rdb_flags & IP_RDB_ISIS) {	/* No routetimer for ISIS */
 		GET_TIMESTAMP(sdb->nettimer);
 	    } else {
 		GET_TIMESTAMP(rdb->rdb_routetimer);
		COPY_TIMESTAMP(rdb->rdb_routetimer, sdb->nettimer);
 	    }
 	    rdb->rdb_flags &= ~IP_RDB_SNAPSHOT_ROUTE;
	}
    }
}


/*
 * ip_route_start_aging_work
 * Start aging routes for network/subnet. If ndb is NULL, age all networks.
 * If ndb is non-NULL, age all subnets maintained for this ndb.
 */
static void ip_route_start_aging_work (pdbtype *pdb, idbtype *idb, paktype *pak,
				ipaddrtype source, ndbtype *ndb)
{
    int j;

    for (j = 0; j < NETHASHLEN; j++) {
	if (ndb) {
	    /* age subnets of ndb */
	    ip_route_start_aging_core(pdb, (ndbtype *) &ndb->subnettable[j],
				      idb, pak, source);
	} else {
	    /* age all networks */
	    ip_route_start_aging_core(pdb, (ndbtype *) &nettable[j],
				      idb, pak, source);
	    ip_route_start_aging_core(pdb, (ndbtype *) &supernettable[j],
				      idb, pak, source);
				      
	}
    }
}

/*
 * ip_route_start_aging_nets
 * Start aging routes for this PDB that supports snapshot for the IDB
 * shown.
 */
static void ip_route_start_aging_nets (pdbtype *pdb, idbtype *idb, paktype *pak,
				ipaddrtype source)
{
    int i;
    ndbtype *ndb, *temp_ndb;
    
    /* age all major nets */
    ip_route_start_aging_work(pdb, idb, pak, source, NULL);

    for (i = 0; i < NETHASHLEN; i++) {
	process_may_suspend();
	ndb = nettable[i];
	while (ndb) {
	    /*
	     * Ugly hack.  netarray_ager can cause the ndb to be
	     * freed.  Skip to the next ndb and then go back and
	     * age the previous one.
	     */
	    temp_ndb = ndb;
	    ndb = ndb->next;
	    if (temp_ndb->attrflags & NET_SUBNETTED)
	        ip_route_start_aging_work(pdb, idb, pak, source, temp_ndb);
	}
    }
}

/* 
 * ip_route_start_aging
 * Start aging all routes on all PDBs that support Snapshot.
 */
void ip_route_start_aging (idbtype *idb, paktype *pak)
{
    pdbtype *pdb;
    ipaddrtype source;
    iphdrtype *ip;
    
    source = 0;
    if (pak != NULL) {
	ip = (iphdrtype *) ipheadstart(pak);
	if (ip != NULL) {
	    source = ip->srcadr;
	}
    }

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->supports_snapshot) {
	    ip_route_start_aging_nets(pdb, idb, pak, source);
	}
    }
}

/*
 * iprouting_shutdown
 * Turn off IP routing after it has already been running
 */

void iprouting_shutdown (void)
{
    ipstatictype *sre;
    pdbtype *pdb, *next_pdb;
    int i;

    /*
     * clean the static table and delete static routes
     */
    for (i = 0; i < NETHASHLEN; ++i)
	ipstatictable[i] = NULL;

    while ((sre = dequeue(&ipstaticQ))) {
	ip_route_delete(pdb_STATIC, sre->network, sre->netsmask,
			IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
	free(sre);
    }
    /*
     * delete routing proceses
     */
    for (pdb = protoQ.qhead; pdb; pdb = next_pdb) {
	next_pdb = pdb->next;
	ip_router_erase(pdb);
    }
}

/*
 * iprouting_get_netsmask
 * Given an address, return the appropriate subnet mask.
 * Note that we purposely return the subnet mask of the last
 * entered network that matches -- this is consistent with
 * table_init's coercion scheme.  They should all be same
 * because of code in class_resolve but if not table_init
 * will coerce.
 */

void iprouting_get_netsmask (ipaddrtype address, ipaddrtype *mask)
{
    ndbtype *ndb, *sdb;

    *mask = get_majormask(address);
    ndb = net_lookup(address, FALSE);
    if (ndb && (ndb->attrflags & NET_SUBNETTED)) {
	sdb = subnet_lookup(ndb, address, 0);
	if (sdb) {
	    *mask = sdb->netsmask;
	    if (NDB2PROCTYPE(sdb) & PROC_CONNECTED)
		return;
	}
	*mask = ndb->netsmask;
	return;
    }
    *mask = get_majormask(address);
}

/*
 * iprouting_delete_conn_route
 * Delete a connected IP route from outside of the iprouting subsystem.
 */

void iprouting_delete_conn_route (ipaddrtype address, ipaddrtype mask)
{
    ip_route_delete(pdb_CONNECTED, address, mask, IPADDR_ZERO,
		    IPADDR_ZERO, NULL, 0);
}


/*
 * iprouting_delete_conn_route_int
 * Delete a connected IP route on a specific interface from outside 
 * of the iprouting subsystem.
 */
void iprouting_delete_conn_route_int (ipaddrtype address, ipaddrtype mask,
				      idbtype *idb)
{
    ip_route_delete(pdb_CONNECTED, address, mask, IPADDR_ZERO,
		    IPADDR_ZERO, idb, 0);
}


/*
 * iprouting_delete_route
 * Delete an exact match of an IP route.
 */

boolean iprouting_delete_route (ipaddrtype address, ipaddrtype mask)
{
    ndbtype *ndb;

    ndb = iprouting_rtlookup_exact(address, mask);
    if (!ndb)
	return(FALSE);
    ip_route_delete(INDEX2PDB(ndb->pdbindex), ndb->netnumber,
		    ndb->netsmask, IPADDR_ZERO, IPADDR_ZERO,
		    NULL, 0);
    return(TRUE);
}

/*
 * iprouting_gdp_default_ndb_prio
 * Determine the GDP priority of the default route.
 */

void iprouting_gdp_default_ndb_prio (ushort *prio)
{
    ulong proctype;

    if (default_netsource) {
	*prio = -1;
	return;
    }
   if (default_ndb == NULL || default_ndb->metric >= METRIC_INACCESSIBLE) {
	*prio = 0;
	return;
    }
    proctype = NDB2PROCTYPE(default_ndb);
    if (proctype & (PROC_CONNECTED|PROC_STATIC))
	*prio = -1;
    else if (proctype & (PROC_RIP))
	*prio = 0xffff - (default_ndb->metric + 1);
    else if (proctype & (PROC_EGP))
	*prio = 0xffff - default_ndb->metric;
    else if (proctype & PROC_IGRP)
	*prio = 0xffff - ((default_ndb->metric >> 8) & 0xFFFF);
    else
	*prio = 0;
}

/*
 * iprouting_lexcmp_rt
 * Determine if the new ndb is a better match than the current best
 * and return whichever is a better match
 */

static ndbtype *iprouting_lexcmp_rt (ipaddrtype addr, ndbtype *new, ndbtype *best)
{

    if (new->netnumber < addr) {
	/*
	 * the new is not a valid match, the previous best is still the best
	 */
	return (best);
    }
    /*
     * the new is a good match, see if it is a better match for a get-next
     */
    if ((best != NULL) && (best->netnumber <= new->netnumber)) {
	/*
	 * the previous best is still the best
	 */
	return (best);
    }
    /*
     * the new guy is the better guy
     */
    return (new);
}

/*
 * iprouting_snmp_rtlookup_exact
 * Lookup an exact match for an IP route in the routing table
 */

boolean iprouting_snmp_rtlookup_exact (ipaddrtype addr, ndbtype **ndb,
				       ulong *proctype, boolean *is_def)
{
    /* handle the default case */
    if (addr == 0) {
	if (!default_ndb)
	    return(FALSE);
	*is_def = TRUE;
	*ndb = default_ndb;
	*proctype = NDB2PROCTYPE(default_ndb);
	return(TRUE);
    }
    *ndb = iprouting_rtlookup_exact(addr, 0);
    if (*ndb) {
	*proctype = NDB2PROCTYPE((*ndb));
	return(TRUE);
    }
    return(FALSE);
}

/*
 * iprouting_snmp_rtlookup_lexnext
 * Lookup the lexicographically next route in the IP routing table
 *
 * NB:  This routine copies the found ndb into the buffer pointed at
 *      by best.  This buffer must be at least sizeof(ndbtype) in size.
 *      The buffer doesn't need to be large enough for the subnettable
 *	of subnetted ndb's because SNMP won't reference anything down there.
 */

boolean iprouting_snmp_rtlookup_lexnext (ipaddrtype addr, ndbtype *best,
					 ulong *proctype, boolean *is_def)
{
    ndbtype *ndb, *sdb, *db, *local_best, *tmp_best;
    int ndb_bucket, sdb_bucket;

    local_best = NULL;
    tmp_best = NULL;

    /* handle the default case */
    if (addr == 0 && default_ndb) {
	bcopy(default_ndb, best, sizeof(ndbtype));
	*is_def = TRUE;
	*proctype = NDB2PROCTYPE(default_ndb);
	return (TRUE);
    }

    ndb_bucket = sdb_bucket = 0;
    ndb = sdb = NULL;
    /*
     * Maintain locks on the ndbs while we traverse the IP routing table,
     * as the traversal may dismiss.  Free locks when done.
     */
    while ((db = geteachroute(&ndb, &ndb_bucket, &sdb, &sdb_bucket, TRUE))) {
	tmp_best = iprouting_lexcmp_rt(addr, db, local_best);
	if (tmp_best != local_best) {
	    ndb_free(local_best, FALSE);
	    local_best = tmp_best;
	    mem_lock(local_best);
	}
    }
    if (local_best) {
	*proctype = NDB2PROCTYPE((local_best));
	bcopy(local_best, best, sizeof(ndbtype));
	ndb_free(local_best, FALSE);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * iprouting_via_nonsubnet
 * Determine if route to a given IP destination is reachable via a route
 * which is not subnetted.  If so, return TRUE.  Otherwise, return FALSE.
 */

boolean iprouting_via_nonsubnet (ipaddrtype dest)
{
    ndbtype *ndb;

    /*
     * Lookup longest match
     */
    ndb = ip_rtlookup(dest, 0);
    if (!ndb)
	return(FALSE);
    if (ndb->attrflags & NET_SUBNET)
	return(FALSE);
    return(TRUE);
}

/*
 * iprouting_conn_plimit_exceed
 * Determine if passed in path count exceeds pdb_CONNECTED.multipath
 */

boolean iprouting_conn_plimit_exceed (ulong count)
{
    if (count >= pdb_CONNECTED->multipath)
	return(TRUE);
    return(FALSE);
}
