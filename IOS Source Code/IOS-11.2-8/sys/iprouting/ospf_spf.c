/* $Id: ospf_spf.c,v 3.11.8.24 1996/09/12 21:19:15 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_spf.c,v $
 *------------------------------------------------------------------
 * ospf_spf.c -- module provides functionality to derive 
 *               routes using Dijkstra Algorithm(spf).
 *
 * December 28 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_spf.c,v $
 * Revision 3.11.8.24  1996/09/12  21:19:15  myeung
 * CSCdi61022:  OSPF fails to delete old subnet mask when mask changed.
 * Branch: California_branch
 * - Fix the case for inter-area route.
 *
 * Revision 3.11.8.23  1996/09/09  17:42:39  myeung
 * CSCdi61022:  OSPF fails to delete old subnet mask when mask changed.
 * Branch: California_branch
 *
 * Revision 3.11.8.22  1996/08/28  12:53:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.8.21  1996/08/16  22:11:07  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.11.8.20  1996/07/29  22:56:44  myeung
 * CSCdi61864:  no ip route while configuring causes Bus Error and reload
 * Branch: California_branch
 *
 * Revision 3.11.8.19  1996/07/27  10:28:21  myeung
 * CSCdi64208:  OSPF external route not installed
 * Branch: California_branch
 *
 * Revision 3.11.8.18  1996/07/19  07:27:17  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.11.8.17  1996/07/15  07:03:56  myeung
 * CSCdi62870:  OSPF crash in delete_old_routes
 * - Also correct headline for CSCdi61956
 * Branch: California_branch
 *
 * Revision 3.11.8.16  1996/07/11  03:47:15  klmj
 * CSCdi61956:  OSPF mem corruption in delete_rtr_route and
 * remove_invalid_routes
 * Branch: California_branch
 *
 * Revision 3.11.8.15  1996/06/28  23:19:24  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.8.14  1996/06/26  06:37:59  myeung
 * CSCdi61328:  OSPF route is missing after corresponding interface is
 * shutdown
 * Branch: California_branch
 *
 * Revision 3.11.8.13  1996/06/25  21:16:02  myeung
 * CSCdi45110:  OSPF does not load balance over External routes
 * Branch: California_branch
 * - Modify OSPF router route data structure and external route calculation
 *
 * Revision 3.11.8.12  1996/06/10  08:28:51  myeung
 * CSCdi46217:  OSPF does not route over unnumbered BRI
 * Branch: California_branch
 * - Grab the next hop address from neighbor information.
 *
 * Revision 3.11.8.11  1996/05/23  12:07:00  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.11.8.10  1996/05/17  21:38:41  myeung
 * CSCdi49377:  BGP: Does not advertise route to external peer
 * Branch: California_branch
 * - Put advertising router as source when installing stub route
 *
 * Revision 3.11.8.9  1996/05/11  00:08:23  myeung
 * CSCdi55640:  sh ip os da asb output incorrect
 * - Set db_rtr_reachable correctly for self-originated type 4 LSA
 * Branch: California_branch
 *
 * Revision 3.11.8.8  1996/05/10  17:23:27  myeung
 * CSCdi56847:  NSSA translater router has bad routing table after mask is
 * changed
 * Branch: California_branch
 *
 * Revision 3.11.8.7  1996/05/08  04:36:31  myeung
 * CSCdi56476:  Spurious memory access in rtr_backlink()
 * Branch: California_branch
 *
 * Revision 3.11.8.6  1996/05/04  01:22:11  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.11.8.5  1996/04/30  23:14:23  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.11.8.4  1996/04/29  10:02:55  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.11.8.3  1996/04/23  21:38:09  myeung
 * CSCdi50496:  ospf route not deleted when the interface address changed
 * Branch: California_branch
 *
 * Revision 3.11.8.2  1996/04/22  05:33:04  myeung
 * CSCdi48106:  OSPF fail to install parallel ex rt with forw addr through
 * sum rt
 * Branch: California_branch
 *
 * Revision 3.11.8.1  1996/04/16  19:02:02  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.11  1996/02/29  10:33:38  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.10  1996/02/08  03:05:51  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.9  1996/02/04  06:07:41  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.8  1996/01/25  09:24:41  myeung
 * CSCdi47030:  OSPF does not load balance with multiple interfaces on
 * same subnet
 *
 * Revision 3.7  1996/01/22  06:14:42  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/05  10:17:50  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/14  23:22:42  myeung
 * CSCdi45610:  spurious memory references in lookup_net_route
 *
 * Revision 3.4  1995/11/18  18:48:18  myeung
 * CSCdi43168:  Clean up OSPF flooding
 * - Consolidate flooding functions
 *
 * Revision 3.3  1995/11/17  17:36:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:10:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/10/12  11:51:06  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.19  1995/10/08  23:44:36  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.18  1995/10/08  22:49:50  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.17  1995/09/23  21:40:52  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.16  1995/09/06  22:53:41  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Resolve problem for boot image
 *
 * Revision 2.15  1995/09/05  22:09:17  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.14  1995/08/18  01:08:06  ahh
 * CSCdi38914:  OSPF crashes with clear ip route *
 * Dereferencing NULL is a bad thing.
 *
 * Revision 2.13  1995/08/01  15:16:00  myeung
 * CSCdi36150:  OSPF database synchronization problem after route recovers
 * - Always take care of the elasped time when calculating LSA age
 * - The maximum age used for comparsion between LSA should be 3600
 *
 * Revision 2.12  1995/07/21  23:57:02  myeung
 * CSCdi35411:  ALIGN-3-SPURIOUS during OSPF configuration or router reload
 *
 * Revision 2.11  1995/07/21  18:43:42  myeung
 * CSCdi36946:  OSPF ext. route with secondary addr. as forward addr. not
 * installed
 *
 * Revision 2.10  1995/07/01  02:19:25  myeung
 * CSCdi36659:  OSPF route pointing to simplex ethernet interface not
 * removed
 * - Store idb->output_if instead of idb in the delete-list
 *
 * Revision 2.9  1995/06/29  19:52:25  myeung
 * CSCdi36186:  OSPF ABR does not generate sum. LSA for connected
 * loopback/MA int.
 *
 * Revision 2.8  1995/06/28  03:21:40  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.7  1995/06/20 06:55:15  myeung
 * CSCdi34206:  OSPF Memory Leak on border area router
 *
 * Revision 2.6  1995/06/19  01:08:23  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/15  00:18:14  myeung
 * CSCdi35275:  Router crashes in _print_delete_list_db
 *
 * Revision 2.4  1995/06/13  01:44:13  myeung
 * CSCdi35692:  OSPF point-to-multipoint interface is ignored in SPF
 * calculation
 *
 * Revision 2.3  1995/06/10  01:22:59  myeung
 * CSCdi35182:  OSPF select ospf-enabled interface which is down as
 *              output interface
 *
 * Revision 2.2  1995/06/09  13:06:35  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:10:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * spf:			schedules spf to run latter.
 * run_spf:		executes spf.
 *
 */

#include "master.h"
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../if/network.h"
#include "route_inlines.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "ospf_public.h"
#include "ospf_inlines.h"


/*
 * External declarations
 */
extern void compute_iso_checksum(uchar *, int, uchar *);

/*
 * Forward Declarations 
 */
static lsdbtype *find_db(areatype *, ulong, ushort);
static ipaddrtype if_addr_to_stub(areatype *, lsdbtype *);

boolean ospf_spf_intra_acl_pass;
boolean ospf_spf_inter_acl_pass;
boolean ospf_spf_ext_acl_pass;


inline boolean acl_ospf_spf_intra_debug (ulong lsid) {
    return (ospf_debug_spf_intra && 
	    (!ospf_spf_intra_acl || fast_check(ospf_spf_intra_acl, lsid)));
}

inline boolean acl_ospf_spf_inter_debug (ulong lsid) {
    return (ospf_debug_spf_inter && 
	    (!ospf_spf_inter_acl || fast_check(ospf_spf_inter_acl, lsid)));
}

inline boolean acl_ospf_spf_ext_debug (ulong lsid) {
    return (ospf_debug_spf_ext && 
	    (!ospf_spf_ext_acl || fast_check(ospf_spf_ext_acl, lsid)));
}

/*
 * check_ospf_suspend
 */
inline void check_ospf_suspend (pdbtype *pdb,
			 int 	flag)
{
    SAVE_CALLER();

    if (OSPF_SHOULD_SUSPEND(pdb)) {
	switch (flag) {
	case DBASE_ACTIVE:    
	    /* 
	     * DBASE_ACTIVE means the database is being accessed, such as
	     * when update is being sent.
	     */
	    pdb->ospf_dbase_lock = DBASE_ACTIVE;
	    break;
	case DBASE_FREEZE:    
	    /* 
	     * DBASE_FREEZE means the database is being deleted or flushed 
	     */
	    pdb->ospf_dbase_lock = DBASE_FREEZE;
	    break;
	}
	ospf_event(OC_SUSPEND, caller(), 0);
	process_suspend();
	ospf_event(OC_UNSUSPEND, caller(), 0);
	pdb->ospf_dbase_lock = DBASE_INACTIVE;
	INIT_OSPF_SUSPEND(pdb);
    }
}
/*
 * check_ospf_suspend_limit
 * A convenient function to further rate-limit the suspension
 */  
inline void check_ospf_suspend_limit(pdbtype *pdb, int flag, int *count) {
    if (!*count) {
	check_ospf_suspend(pdb, flag);
	*count = LSA_PROCESSCOUNT; 
    }
    (*count)--;
}

/*
 * update_rtr_route:
 * updates a route to a router in an area.
 *
 * This router route table is maintained one per each
 * OSPF router process running in the box. This table keeps
 * track of how to reach Area Border Routers and AS Boundary Routes.
 *
 * This routine returns TRUE if the route is added.
 */

static boolean update_rtr_route (
    areatype 	*area,		/* updated route belongs to the area */
    lsdbtype 	*db,		/* Link State from which route derived */
    ushort  	rtr_type,	/* router Type (Area Border or AS Boundary) */
    ushort  	type,		/* route type (INTRA or INTER ) */
    ipaddrtype	if_addr,
    idbtype	*idb,			  
    ipaddrtype   source)
{
    rtr_route *route;
    ospf_rtr_path *path;
    pdbtype *pdb;
    int index;
    boolean new_route, recal_min_cost;
    ulong min_cost;

    ospf_event(OC_GENERIC, (ulong) "update_rtr_route", area->ar_id);
    pdb = area->ar_pdb_ptr;
    index = XHASH(db->db_dest, 0);
    new_route = FALSE;
    path = NULL;
    for (route = pdb->ospf_routes[index]; route ; route = route->rt_next) {
	if (route->rt_dest == db->db_dest) {
	    for (path = route->rt_path_list; path; path = path->orp_next) {
		if ((path->orp_forw_addr == if_addr) &&
		    (path->orp_idb == idb) &&
		    ((!(rtr_type & DTYPE_ABR)) || (path->orp_area == area))) {
		    break;
		}
	    }
	    break;
	}
    }
    if (route == NULL) {
	if ((route = malloc(sizeof(rtr_route))) == NULL) {
	    return(FALSE);
	}
	new_route = TRUE;
	/* insert it in the table */
	route->rt_next = (pdb->ospf_routes)[index];
	(pdb->ospf_routes)[index] = (rtr_route *)route;
    }
    if (!path) {
	path = malloc(sizeof(ospf_rtr_path));
	if (!path) {
	    if (new_route)
		free(route);
	    return (FALSE);
	}
	/*
	 * Insert path into path list
	 */
	path->orp_next = route->rt_path_list;
	route->rt_path_list = path;
    }
    if ((rtr_type & DTYPE_ASBR) && (db->db_dist != path->orp_cost)){
	spf(area, SPF_SCHD_ASE | SPF_SCHD_7_ASE, db->db_dest);
    }
    /* adding ASBR with ABR */
    if ((rtr_type & DTYPE_ASBR) && ((path->orp_dest_type & DTYPE_ASBR) == 0)) {
	spf(area, SPF_SCHD_ASE | SPF_SCHD_7_ASE, db->db_dest);
    }
    /* removing ASBR  */
    if (((rtr_type & DTYPE_ASBR) == 0) && (path->orp_dest_type & DTYPE_ASBR)) {
	spf(area, SPF_SCHD_ASE | SPF_SCHD_7_ASE, db->db_dest);
    }
    if ((rtr_type & DTYPE_ABR) && (db->db_dist != path->orp_cost)) {
	spf(area, SPF_SCHD_SNET, db->db_dest);
    }
    if ((rtr_type & DTYPE_ABR) && ((path->orp_dest_type & DTYPE_ABR) == 0)) {
	spf(area, SPF_SCHD_SNET, db->db_dest);
    }
    if (((rtr_type & DTYPE_ABR) == 0) && (path->orp_dest_type & DTYPE_ABR)) {
	spf(area, SPF_SCHD_SNET, db->db_dest);
    }

    /* now plug in info */
    route->rt_dest = db->db_dest;
    recal_min_cost = FALSE;
    if (path->orp_cost <= route->rt_min_cost) {
	/*
	 * We probably need to recalculate best cost.
	 */
	recal_min_cost = TRUE;
    }
    path->orp_cost = db->db_dist;
    path->orp_dest_type = rtr_type;
    path->orp_route_type = type;
    path->orp_area = area;
    path->orp_db = db;
    path->orp_idb = idb;
    path->orp_forw_addr = if_addr;
    path->orp_spf_inst = area->ar_spf_instance;
    path->orp_pdb_spf_inst = pdb->local_ospf_spf_instance;
    path->orp_infosource = source;
    if (new_route || (path->orp_cost < route->rt_min_cost)) {
	route->rt_min_cost = path->orp_cost;
    } else if (recal_min_cost) {
	/*
	 * Path cost increase and its old value is minimum,
	 * need to recalculate.
	 */
	min_cost = SUM_LS_INFINITY;
	for (path = route->rt_path_list; path; path = path->orp_next) {
	    if (path->orp_cost < min_cost)
		min_cost = path->orp_cost;	    
	}
	route->rt_min_cost = min_cost;
    }
    return(TRUE);

} /* end of update_rtr_route */

/*
 * lookup_rtr_route:
 * looks up route for given arguments
 *
 * Only Necessary arguments are passed and
 * the rest are passed as NULL. Only Non NULL arguments
 * are compared. (only dest_rtr can not be NULL).
 *
 * This routine returns pointer to route or NULL.
 * It also return pointer to the first path that match the 
 * the argument in good_path, if provided.
 */

rtr_route *lookup_rtr_route (pdbtype *pdb, areatype *area, ipaddrtype dest_rtr,
			     lsdbtype *db, ushort dest_type, ushort type,
			     ospf_rtr_path **good_path)
{
    rtr_route *route;
    ospf_rtr_path *path;
    rtr_route *good_route = NULL;
    ulong cost = INTRA_PATH_COST_MAX;
    boolean intra_route_seen = FALSE; 
    int index;

    index = XHASH(dest_rtr, 0);

    if (good_path)
	*good_path = NULL;

    for (route = pdb->ospf_routes[index]; route ; route = route->rt_next) {
	if (route->rt_dest == dest_rtr) {
	    for (path = route->rt_path_list; path; path = path->orp_next) {
		if ((!dest_type || path->orp_dest_type & dest_type) &&
		    (!area || path->orp_area == area) &&
		    (!type || path->orp_route_type == type) &&
		    (!db || path->orp_db == db)) {

		    if (path->orp_route_type == RTYPE_INTER) {
			if (intra_route_seen)
			    continue;
		    } else {
			if (intra_route_seen == FALSE) {
			    good_route = route;
			    if (good_path)
				*good_path = path;
			    intra_route_seen = TRUE;
			    cost = path->orp_cost;
			    continue;
			}
		    }
		    if (cost > path->orp_cost) {
			cost = path->orp_cost;
			good_route = route;
			if (good_path)
			    *good_path = path;
		    }
		}
	    }
	    break;
	}
    }
    if (good_route)
	return(good_route);
    return(NULL);
} /* end of lookup_rtr_route */

/*
 * delete_rtr_route:
 * deletes route that matches given arguments
 *
 * Only Necessary arguments are passed and the rest are passed as NULL,
 * except dest_rtr, gateway, and source can not be NULL!
 *
 * This routine returns nothing.
 */

static void delete_rtr_route (
    areatype	*area,	 /* route in the area or NULL(any area) */
    ipaddrtype 	dest_rtr,/* destination router id */
    ushort  	rtr_type,/* destination router type (AB or ASB) */
    ipaddrtype	gateway,
    idbtype	*idb,
    ushort  	type,	 /* type of route( INTRA, INTER or NULL) */
    ipaddrtype  source)
{
    rtr_route *route, *prev_route, *next_route;
    ospf_rtr_path *path, *prev_path, *next_path;
    lsdbtype *db;
    list_queue flood_list;
    pdbtype *pdb;
    areatype *ar;
    int index;
    ulong min_cost;

    ospf_event(OC_GENERIC, (ulong) "delete_rtr_route", area->ar_id);
    pdb = area->ar_pdb_ptr;
    index = XHASH(dest_rtr, 0);

    prev_route = NULL;
    for (route = pdb->ospf_routes[index]; route; route = next_route) {
	next_route = route->rt_next;
	if (route->rt_dest == dest_rtr) {
	    prev_path = NULL;
	    min_cost = SUM_LS_INFINITY;
	    for (path = route->rt_path_list; path; path = next_path) {
		next_path = path->orp_next;
		if ((path->orp_infosource == source) && 
		    (path->orp_forw_addr == gateway) &&
		    (!idb || path->orp_idb == idb) &&
		    (!rtr_type || (path->orp_dest_type & rtr_type)) &&
		    (!area || path->orp_area == area) &&
		    (!type || path->orp_route_type == type)) {
		    ospf_spf_acl_buginf("\nOSPF: Delete path to router %i "
					"via %i spf %d\n", 
					route->rt_dest, path->orp_forw_addr, 
					path->orp_spf_inst);
		    if (path->orp_flag == REDIST_LOST)
			pdb->ospf_flag |= REDIST_LOST;
		    rtr_type = path->orp_dest_type;
		    /*
		     * Remove path structure
		     */
		    if (prev_path)
			prev_path->orp_next = next_path;
		    else
			route->rt_path_list = next_path;
		    path->orp_next = NULL;
		    free(path);
		    if (rtr_type) {
			if (rtr_type == (DTYPE_ASBR | DTYPE_ABR)) {
			    spf(area, SPF_SCHD_ASE | SPF_SCHD_SNET, 
				route->rt_dest);
			} else
			    if (rtr_type & DTYPE_ASBR) {
				spf(pdb->ospf_pdb->ospf_dummy_area, 
				    SPF_SCHD_ASE, route->rt_dest);
			    } else {
				spf(area, SPF_SCHD_SNET, route->rt_dest);
			    }
			if (pdb->ospf_area_brtr && (area->ar_id == 0) && 
			    (rtr_type & DTYPE_ASBR)) {
			    flood_list.q_first = NULL;
			    flood_list.q_last  = NULL;
			    for (ar = pdb->ospf_area_list; ar; 
				 ar = ar->ar_next) {
				if (area == ar || ar->ar_stub)
				    continue;
				db = db_find_lsa(ar, dest_rtr, ROUTER_ID(ar),
						 LSA_TYPE_SUM_ASB);
				build_sum_lsa(&db, ar, dest_rtr, 0, 
					      SUM_LS_INFINITY, 
					      LSA_TYPE_SUM_ASB, RTYPE_INTRA,
					      &flood_list);
				flood_area(ar, &flood_list, NULL, NULL);
			    }
			}
		    }
		} else {
		    /*
		     * Keep track of min cost in case the best path is 
		     * deleted.
		     */
		    if (path->orp_cost < min_cost)
			 min_cost = path->orp_cost;
		    prev_path = path;
		}
	    }
	    if (!route->rt_path_list) {
		/*
		 * Remove route structure
		 */
		if (prev_route)
		    prev_route->rt_next = next_route;
		else
		    pdb->ospf_routes[index] = next_route;
		route->rt_next = NULL;
		free(route);
	    } else {
		route->rt_min_cost = min_cost;
	    }
	    break;
	}
	prev_route = route;
    }
} /* end of delete_rtr_route */

/*
 * remove_invalid_routes
 * Checks the internal routes to
 * routers and removes invalid routes.
 * This is needed, because there is a
 * bug in the code which is leaving invalid
 * routes in the table.
 */
static void remove_invalid_routes (areatype *area)
{
    rtr_route *route, *prev_route, *next_route;
    ospf_rtr_path *path, *prev_path, *next_path;
    pdbtype *pdb;
    int i;
    ulong min_cost;

    pdb = area->ar_pdb_ptr;

    for (i = 0; i < HTBLSIZE; i++) {
	prev_route = NULL;
	for (route = pdb->ospf_routes[i]; route; route = next_route) {
	    next_route = route->rt_next;
	    prev_path = NULL;
	    min_cost = SUM_LS_INFINITY;
	    for (path = route->rt_path_list; path; path = next_path) {
		next_path = path->orp_next;
		if ((path->orp_area == area) && 
		    (path->orp_spf_inst != area->ar_spf_instance)) {
		    if (prev_path)
			prev_path->orp_next = next_path;
		    else
			route->rt_path_list = next_path;
		    path->orp_next = NULL;
		    free(path);
		} else {
		    /*
		     * Keep track of best cost
		     */
		    if (path->orp_cost < min_cost)
			min_cost = path->orp_cost;
		    prev_path = path;
		}
	    }
	    if (!route->rt_path_list) {
		if (prev_route)
		    prev_route->rt_next = next_route;
		else
		    pdb->ospf_routes[i] = next_route;
		route->rt_next = NULL;
		free(route);
	    } else {
		route->rt_min_cost = min_cost;
	        prev_route = route;
	    }
	}
    }
} /* end of remove_invalid_routes */

/*
 * free_dpath:
 * 
 * Find a matching path in db_dpath_list and delete it.
 * gateway can be 0 in case of unnumbered, so check for NULL idb as a
 * mean of deleting the whole db_dpath_list instead.
 */

static void free_dpath (lsdbtype *db, ipaddrtype gateway, idbtype *idb)
{
    pathtype	*path;
    pathtype	*temp;

    if (db->db_dpath_list == NULL)
	return;
    if (idb) {
	for (path = (pathtype *)(&db->db_dpath_list); path->pb_next ;
	     path = path->pb_next) {
	    if ((path->pb_next->pb_gateway == gateway) &&
		(path->pb_next->pb_firsthop_idb == idb)) {
		temp = path->pb_next;
		path->pb_next = path->pb_next->pb_next;
		temp->pb_next = NULL;
		free(temp);
		return;
	    }
	}
    } else {
	for (path = db->db_dpath_list; path ; path = temp) {
	    temp = path->pb_next;
	    path->pb_next = NULL;
	    free(path);
	}
	db->db_dpath_list = NULL;
    }
} /* end of free_dpath */

static void delete_one_route_list (
    lsdbtype    **route_list,
    lsdbtype 	*db,	/* delete this db from the list */
    ipaddrtype 	gateway,
    idbtype 	*idb)
{
    free_dpath(db, gateway, idb);
    if (db->db_dpath_list == NULL) {
	ospf_spf_acl_buginf("\nOSPF: delete lsa id %i, type %d, adv rtr %i "
			    "from delete list", LSA_ID(db), LSA_TYPE(db),
			    ADV_RTR(db));
	if (!db->db_rt_table) {
	    ospf_spf_acl_buginf("\nOSPF: lsa id %i not on delete list", 
				LSA_ID(db));
	    return;
	}
	/* delete this db from delete list */
	db->db_rt_table = FALSE;
	if (db->db_rprev == NULL) {
	    /*
	     * it must be first element in the
	     * the list
	     */
	    *route_list = db->db_rnext;
	    if (db->db_rnext)
		db->db_rnext->db_rprev = NULL;
	} else {
	    /* check if it is at the end */
	    if (db->db_rnext == NULL)
		db->db_rprev->db_rnext = NULL;
	    else {
		/* it must be in the middle */
		db->db_rnext->db_rprev = db->db_rprev;
		db->db_rprev->db_rnext = db->db_rnext;
	    }
	}
	db->db_rnext = NULL;
	db->db_rprev = NULL;
    }
} /* end of delete_one_route_list */

/*
 * delete_route_list:
 * delets Link State from route list that matches given arguments
 *
 * This route list keeps track of Link States that contributed
 * current routes in IP Global Routing table or router process specific
 * router route table. This list is used to delete invalid routes
 * from route table after completing the spf. This list is maintained 
 * per area. There are two pointers ar_delete_list and ar_insert_list 
 * in area structure that controls where to insert new Link State and what
 * old invalid routes to delete from routing tables. 
 *
 * If argument db is NULL, it deletes all entries in the list
 * ar_delete_list. If also turns off db_rt_table indicating that it is not 
 * in this list anymore. This does not check if this Link State is in 
 * delete_list or not before trying to delete as it assumes that it is in 
 * the list. Developer must be cautious in using this routine. 
 * (fields db_rnext and db_rprev area used to maintain this).
 *
 * This routine returns none.
 */

static void delete_route_list (
    lsdbtype    **route_list,
    lsdbtype 	*db,	/* delete this db from the list */
    ipaddrtype 	gateway,
    idbtype 	*idb)
{
    lsdbtype *temp;

    if (db) {
 	ospf_event(OC_DEL_RT_LST, (ulong) LSA_ID(db),
 		   ((ulong) db | (LSA_TYPE(db) << 24)));
	delete_one_route_list(route_list, db, gateway, idb);
    } else {
	if (db == NULL) {
	    for (db = *route_list; db; db = temp ) {
		db->db_rt_table = FALSE;
		free_dpath(db, 0, 0);
		temp = db->db_rnext;
		db->db_rnext = NULL;
		db->db_rprev = NULL;
	    }
	    *route_list = NULL;
	}
    }
} /* end of delete_route_list */

/*
 * process_add_req
 * processes add requests from
 * Global routing table
 */

void process_add_req (pdbtype *pdb)
{
    ospfbackuptype *backup;
    ospfbackuptype *temp;
    areatype *area;
    lsdbtype *db;
    net_lsatype *net_lsa;
    pathtype *path;
    ndbtype *ndb;
    ipaddrtype address, mask;
    rdbtype *rdb;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    backup = pdb->add_requests;
    pdb->add_requests = NULL;
    for (; backup; backup = temp) {
	temp = backup->next;
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	switch(backup->route_type) {
	  case RTYPE_INTRA:
	    for (area = pdb->ospf_area_list;
			 area && area->ar_id != backup->context_info;
			 area = area->ar_next) {
		;  /* explicit empty statement */
	    }
	    if (area) {
		/*
		 * Note that we have not accounted for parallel paths.
		 * The parallel paths will be reinstalled when
		 * next SPF is done. However, it would be good to take 
		 * care of parallel path here too. It probably requires some
		 * change in OSPF routing table structure (The insert
		 * and delete list). We should pursue this when resources
		 * are available.
		 */ 
		db = find_db(area, backup->netnumber, backup->route_type);
		if (db && db->db_dist <= INTRA_PATH_COST_MAX) {
		    if ((db->db_lsa_ptr->lsh_type == LSA_TYPE_NET) 
			|| (db->db_lsa_ptr->lsh_type == LSA_TYPE_STUB)) { 
			net_lsa = (net_lsatype *)db->db_lsa_ptr;
			pdb->ospf_area_id = area->ar_id;
			pdb->spf_instance = area->ar_spf_instance;
			pdb->ospf_ex_forw_dist = 0;
			for (path = db->db_path_list; path;
			     path = path->pb_next) {
			    rdb = network_update(pdb,
						 db->db_dest, db->db_mask, 
						 path->pb_gateway,
						 path->pb_firsthop_idb,
						 db->db_dist,
						 0,
						 net_lsa->la_hdr.lsh_adv_rtr,
						 0, NULL,
						 backup->route_type);
			    if (rdb) {
				ospf_spf_buginf("\nOSPF: Added backup "
						"Network Route to %i"
						"\n      Metric: %d, "
						"Next Hop: %i",
						db->db_dest, db->db_dist,
						path->pb_gateway);
				rdb->ospf_rdb_lsdb = db;
				rdb->ospf_rdb_area = area;
			    }
			}
		    }
		}
	    }
	    break;
	  case RTYPE_INTER:
	    for (area = pdb->ospf_area_list;
			 area && area->ar_id != backup->context_info;
			 area = area->ar_next);
	    if (area) {
		db = find_db(area, backup->netnumber, backup->route_type);
		if (!db) {
		    /* 
		     * LSA might have host bit set, try look up for that.
		     */
		    db = find_db(area, backup->netnumber | ~backup->netmask,
				 backup->route_type);
		}
		if (db)
		    ospf_schedule_partial_spf(area, db);
	    }
	    break;
	  case RTYPE_EXTERN1:
	  case RTYPE_EXTERN2:
	    db = find_ex_db(ospf_pdb->ospf_dummy_area, backup->netnumber, 
			    0, LSA_TYPE_ASE);
  	    if (!db) {
		/* 
		 * LSA might have host bit set, try look up for that.
		 */
		db = find_ex_db(ospf_pdb->ospf_dummy_area, 
				backup->netnumber | ~backup->netmask, 0,
				LSA_TYPE_ASE);
	    }
	    if (db)
		ospf_schedule_partial_spf(ospf_pdb->ospf_dummy_area, db);
	    break;
	  case RTYPE_NSSA1: 
	  case RTYPE_NSSA2:
            for (area = pdb->ospf_area_list;
                         area && area->ar_id != backup->context_info;
                         area = area->ar_next);
            if (area) {
 	        db = find_ex_db(area, backup->netnumber, 0, LSA_TYPE_7_ASE);

		if (!db) {
                    /*
		     * LSA might have host bit set, try look up for that.
		     */
		    db = find_ex_db(area, backup->netnumber | ~backup->netmask,
				     0, LSA_TYPE_ASE);
		}
		if (db)
		    ospf_schedule_partial_spf(area, db);
	    }
	    break;
	  default:
	    if (backup->route_type == 0) {
		/*
		 * It is a request to start redistribution
		 * for a network into OSPF.
		 */
		address = backup->netnumber;
		mask =  backup->context_info;
		if (address == 0) {
		    if (default_ndb)
			network_redist(NULL, default_ndb, ROUTE_UP);
		    break;
		}
		ndb = ip_rtlookupdummy(address, mask);
		if (ndb)
		    network_redist(INDEX2PDB(ndb->pdbindex), ndb, ROUTE_UP);
	    } else 
		errmsg(&msgsym(INTERNALERR, OSPF), "Invalid route request");
	}
	free(backup);
    }
} /* end of process_add_req */

/*
 * ospf_rcv_add_req
 * receive request to add a
 * route from global routing table.
 */

void ospf_rcv_add_req (pdbtype *pdb, ipaddrtype address, ipaddrtype mask,
		       backuptype *ipbackup)
{
    ospfbackuptype *backup;

    if (pdb->ospf_dbase_lock == DBASE_FREEZE)
	return;
    backup = malloc(sizeof(ospfbackuptype));
    if (backup == NULL)
	return;
    backup->netnumber = address & mask;
    backup->netmask = mask;
    backup->context_info = ipbackup->backup_context_info;
    backup->route_type = ipbackup->route_type;
    if (pdb->add_requests)
	backup->next = pdb->add_requests;
    pdb->add_requests = backup;

} /* end of ospf_rcv_add_req */

/*
 * process_dbf_ase_use
 */
static inline void process_dbf_ase_use (areatype *area, lsdbtype *db) { 
    if (db->db_ase_use) {
	if (db->db_ase_count != db->db_path_count) {
	    spf(area, SPF_SCHD_ASE, LSA_ID(db) );
	}
    }
} /* process_dbf_ase_use */

/*
 * insert_route_list
 * insert the the db in the
 * list of db that keep track of
 * routes in routing table 
 */
	    
static void insert_route_list (lsdbtype **route_list, lsdbtype *db)
{
    ospf_event(OC_INS_RT_LST, (ulong) LSA_ID(db), 
 	       ((ulong) db | (LSA_TYPE(db) << 24)));
    if (db->db_dpath_list != NULL) {
	ospf_spf_acl_buginf("\nOSPF: Path left undeleted to %i", db->db_dest);
	return;
    }
    if (db->db_path_list == NULL) {
        ospf_spf_acl_buginf("\nOSPF: No new path to %i", db->db_dest);
	return;
    }
    
    ospf_spf_acl_buginf("\nOSPF: insert route list LS ID %i, type %d, "
			"adv rtr %i", LSA_ID(db), LSA_TYPE(db), ADV_RTR(db));

    if (db->db_rt_table) {
        ospf_spf_acl_buginf("\nOSPF: LSA ID %i already on the route list", 
			    LSA_ID(db));
	return;
    }
    db->db_rprev = NULL;
    db->db_rt_table = TRUE;
    db->db_rnext = *route_list;
    if (db->db_rnext)
        db->db_rnext->db_rprev = db;
    *route_list = db;
    ospf_event(OC_INS_RT_LST_INFO, (ulong) db->db_rnext, (ulong) db->db_rprev);
} /* end of insert_route_list */

/*
 * ospf_insert_faddr
 * insert the the external LSA
 * forward address if it is not
 * in the list.
 */

static void ospf_insert_faddr (pdbtype *pdb, ipaddrtype address)
{
    faddrtype *faddr;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    /*
     * Check if the forward address
     * is in the queue.
     */
    faddr = (faddrtype *)pdb->ospf_faddrq->qhead;
    for (; faddr ; faddr = faddr->next) {
	if (address == faddr->address)
	    return;
    }
    faddr = malloc(sizeof(faddrtype));
    if (faddr == NULL)
	return;
    faddr->address = address;
    enqueue(pdb->ospf_faddrq, faddr);
    if (!mgd_timer_running(&ospf_pdb->ospf_faddr_timer))
	mgd_timer_start(&ospf_pdb->ospf_faddr_timer, FADDR_INTERVAL * ONESEC);
}
/*
 * ospf_destroy_faddrq
 * destroy the queue and stop
 * the timer
 */

void ospf_destroy_faddrq (pdbtype *pdb)
{

    faddrtype *faddr;
    faddrtype *temp;

    faddr = (faddrtype *)pdb->ospf_faddrq->qhead;
    for (; faddr ; faddr = temp) {
	temp = faddr->next;
	free(faddr);
    }
    pdb->ospf_faddrq->qhead = NULL;
    pdb->ospf_faddrq->qtail = NULL;
    pdb->ospf_faddrq->count = 0;

    mgd_timer_stop(&pdb->ospf_pdb->ospf_faddr_timer);
}


/*
 * ospf_destroy_maxage
 * destroy the queue maxage list
 */

void ospf_destroy_maxage (pdbtype *pdb)
{
    lsdbtype *db, *temp;

    if (pdb->ospf_max_headptr == NULL)
	return;
    db = pdb->ospf_max_headptr;
    for (; db ; db = temp) {
	temp = db->db_mnext;
	db->db_mnext = NULL;
	db->db_mprev = NULL;
	db->db_in_maxageq = FALSE;
    }
    pdb->ospf_max_headptr = NULL;
    pdb->ospf_max_tailptr = NULL;
    pdb->ospf_flag &= ~OSPF_MAXAGEQ;
}


/*
 * ospf_service_maxage
 * If possible delete the
 * maxaged lsas.
 */

void ospf_service_maxage (pdbtype *pdb)
{
    lsdbtype *db, *temp, *new_headptr, *new_tailptr;

    if (pdb->ospf_max_headptr == NULL)
	return;
    new_headptr = NULL;
    new_tailptr = NULL;
    db = pdb->ospf_max_headptr;
    ospf_buginf("\nOSPF: service_maxage: Trying to delete MAXAGE LSA");
    ospf_event(OC_SERVICE_MAXAGE, (ulong) pdb->ospf_max_headptr, 
	       (ulong) pdb->ospf_max_tailptr);
    for (; db ; db = temp) {
	temp = db->db_mnext;
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	db->db_mnext = NULL;
	db->db_mprev = NULL;
	db->db_in_maxageq = FALSE;
	if (!db_free(pdb, db)) {
	    /* if db is not deleted, put it back on the list */
	    if (new_headptr == NULL) {
		new_headptr = db;
		db->db_mprev = NULL;
	    } else {
		db->db_mprev = new_tailptr;
		new_tailptr->db_mnext = db;
	    }
	    db->db_mnext = NULL;
	    new_tailptr = db;
	    db->db_in_maxageq = TRUE;
	}
    }
    pdb->ospf_max_headptr = new_headptr;
    pdb->ospf_max_tailptr = new_tailptr;
    if (new_headptr) {
	pdb->ospf_flag |= OSPF_MAXAGEQ;
	GET_TIMESTAMP(pdb->ospf_maxage_delete_time);
    }
}


/*
 * ospf_insert_maxage
 * insert maxaged lsa in a list
 */

void ospf_insert_maxage (pdbtype *pdb, lsdbtype *db)
{

    if ((db) 
	&& (db->db_lsa_ptr)
	&& (db->db_freeme)
	&& (ADV_AGE(db) >= MAXAGE)
	&& !db->db_in_maxageq) {
	ospf_event(OC_INSERT_MAXAGE, (ulong) db, (ulong) LSA_ID(db));
	if (pdb->ospf_max_headptr == NULL) {
	    pdb->ospf_max_headptr = db;
	    db->db_mprev = NULL;
	} else {
	    db->db_mprev = pdb->ospf_max_tailptr;
	    pdb->ospf_max_tailptr->db_mnext = db;
	}
	db->db_mnext = NULL;
	pdb->ospf_max_tailptr = db;
	db->db_in_maxageq = TRUE;
	if (!(pdb->ospf_flag & OSPF_MAXAGEQ)) {
	    GET_TIMESTAMP(pdb->ospf_maxage_delete_time);
	    pdb->ospf_flag |= OSPF_MAXAGEQ;
	}
    } else
	if ((db) && (db->db_lsa_ptr) 
	    && (ADV_AGE(db) < MAXAGE)
	    && db->db_in_maxageq) {
	    ospf_delete_maxage(pdb, db);
	}
}

/* ospf_delete_maxage
 * remove a db from link of MAXAGE LSA.  So it can be db_free().
 * Called from ospf_rcv_ack()
 */
void ospf_delete_maxage (
    pdbtype *pdb,
    lsdbtype *db)
{
    if ((pdb->ospf_max_headptr == NULL) || !db->db_in_maxageq)
	return;
    if (db->db_mprev == NULL)   /* if this db is the first one */
	pdb->ospf_max_headptr = db->db_mnext;
    else
	db->db_mprev->db_mnext = db->db_mnext;
    if (db->db_mnext == NULL)   /* if this db is the last one */
	pdb->ospf_max_tailptr = db->db_mprev;
    else
	db->db_mnext->db_mprev = db->db_mprev;
    db->db_mnext = NULL;
    db->db_mprev = NULL;
    db->db_in_maxageq = FALSE;
}


/*
 * ospf_service_exdelayq
 * flood the delayed
 * externals.
 */

void ospf_service_exdelayq (pdbtype *pdb)
{
    faddrtype *addr;
    lsdbtype *db;
    list_queue	flood_list;
    int lsa_count;
    int pak_count;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    if (pdb->ospf_exdelayq->qhead == NULL)
	return;
    lsa_count = 0;
    pak_count = 0;
    while ((addr = dequeue(pdb->ospf_exdelayq))) {
	db = db_find_lsa(ospf_pdb->ospf_dummy_area, addr->address,
			 pdb->ospf_rtr_id, LSA_TYPE_ASE);
	if (db && db->db_flood_dly) {
	    add_to_flood_list(db, &flood_list);
	    lsa_count++;
	    db->db_flood_dly = FALSE;
	}
	if (db)
	    ospf_insert_maxage(pdb, db);
	free(addr);
	if ( lsa_count >= MAX_EX_LSA_PAK) {
	    ex_flood_area(pdb, &flood_list, NULL, NULL);
	    pak_count++;
	    lsa_count = 0;
	    if (pak_count >= MAX_EX_PAK_RATE)
		return;
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	}
    }
    ex_flood_area(pdb, &flood_list, NULL, NULL);
} /* end of ospf_service_exdelayq */


/*
 * ospf_insert_exdelayq
 * insert the external LSA
 * for a partial spf
 */

void ospf_insert_exdelayq (pdbtype *pdb, lsdbtype *db)
{

    faddrtype *faddr;
    
    faddr = malloc(sizeof(faddrtype));
    if (faddr == NULL)
	return;
    db->db_flood_dly = TRUE;
    faddr->address = LSA_ID(db);
    enqueue(pdb->ospf_exdelayq, faddr);
}


/*
 * ospf_destroy_exdelayq
 * destroy the queue and stop
 * the timer
 */

void ospf_destroy_exdelayq (pdbtype *pdb)
{

    faddrtype *faddr;
    faddrtype *temp;

    if (pdb->ospf_exdelayq->qhead == NULL)
	return;
    faddr = (faddrtype *)pdb->ospf_exdelayq->qhead;
    for (; faddr ; faddr = temp) {
	temp = faddr->next;
	free(faddr);
    }
    pdb->ospf_exdelayq->qhead = NULL;
    pdb->ospf_exdelayq->qtail = NULL;
    pdb->ospf_exdelayq->count = 0;

} /* end of ospf_destroy_exdelayq */

/* 
 * ospf_lookup_net_route
 * Looks up ospf specific route in the ip global routing table.
 *
 * Note: compare_area indicates whether we should match area that the route
 * is learned to the area ptr argument passed. area must be NON-NULL, even
 * campare_area is FALSE, because area is used to deduce the pdb.
 */

ndbtype *ospf_lookup_net_route (areatype *area, ipaddrtype dest_addr, 
				ipaddrtype mask, ushort path_type, 
				boolean compare_area)
{
    ndbtype *db;
    ospf_idb_info *ospf_info;
    pdbtype *pdb = area->ar_pdb_ptr;
   
    /*
     * If we are looking up default destination, return ndb_zero only
     * when it was previously installed by OSPF.
     */
    if (dest_addr == 0) {
	if (ndb_zero && (ndb_zero->pdbindex == area->ar_pdb_ptr->index))
	    return(ndb_zero);
	else
	    return(NULL);
    }

    db = ip_rtlookup(dest_addr, mask);
    if (!db)
	return(NULL);

    if (mask) {
	if (db->netsmask != mask)
	    return(NULL);
    }
    ospf_info = NULL;
    if (db->lastinterface)
	ospf_info = db->lastinterface->ospf_info;
    /*
     * Make sure the route has the right pdb.
     */
    if (((NDB2PROCTYPE(db) & PROC_OSPF) && ip_own_ndb(db, pdb)) || 
	((db->attrflags & NET_ATTACHED) && ospf_info && 
	 ospf_info->enabled && (ospf_info->area_ptr->ar_pdb_ptr == pdb))) {
	if ((!path_type || db->route_type == path_type) &&
	    (!compare_area || db->ndb_area_id == area->ar_id))
	    return(db);
	if (db->attrflags & NET_ATTACHED)
 	    return(db);
    }
    return(NULL);
} /* end of ospf_lookup_net_route */


/*
 * ospf_get_idb
 * get an idb for a gateway
 */
static idbtype *ospf_get_idb (areatype *area, ipaddrtype address)
{
    rdbtype *rdb;
    idbtype *idb;
    ndbtype *db;

    db = ospf_lookup_net_route(area, address, 0, 0, FALSE);
    if (db == NULL)
	return(NULL);
    idb = NULL;
    if (db->routecount != 0) {
	rdb = db->rdb[0];
	idb = rdb->idb;
    } else if (db->lastinterface)
	idb = db->lastinterface;
    if (idb && !interface_up(idb))
	return(NULL);
    return(idb);

} /* end of ospf_get_idb */ 


/*
 * ospf_ex_faddr
 * Timer vector for activating
 * external SPF if necessary
 */

void ospf_ex_faddr (pdbtype *pdb)
{
    faddrtype *faddr;
    ndbtype *ndb;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "exfaddr", 0);
    if (pdb->ospf_dbase_lock == DBASE_FREEZE)
 	return;
    if (pdb->ospf_dbase_lock == DBASE_ACTIVE) {
 	mgd_timer_start(&ospf_pdb->ospf_faddr_timer, FADDR_INTERVAL * ONESEC);
 	return;
    }
    if (pdb->ospf_faddrq->qhead == NULL)
	return;
    if (!pdb->ospf_area_list)
	return;
    faddr = (faddrtype *)pdb->ospf_faddrq->qhead;
    for (; faddr ; faddr = faddr->next) {
	ndb = ospf_lookup_net_route(ospf_pdb->ospf_dummy_area,
				    faddr->address, 0, 0, FALSE);
	
	/*
	 * If we have valid route and if it is not of
	 * external route type, Schedule SPF calculations.
	 */
	if (ndb && (!(ndb->route_type & (RTYPE_EXTERN1 | RTYPE_EXTERN2)))) {
	    spf(ospf_pdb->ospf_dummy_area, (ushort)SPF_SCHD_ASE, 
		faddr->address);
	    return;
	}
    }
    mgd_timer_start(&ospf_pdb->ospf_faddr_timer, FADDR_INTERVAL * ONESEC);
}


/*
 * create_sum
 * takes a db and creates a summary db
 * into a given area.
 * Called from 1) start_sum_brtr() while loop through area->ar_delete_list.
 */

static void create_sum (areatype *area, areatype *ar, lsdbtype *db,
		       ulong cost, list_queue *list)
{
    pdbtype *pdb;
    addr_range *range;
    lsdbtype *rdb;
    rtr_lsatype *rtr_lsa;

    pdb = area->ar_pdb_ptr;
    if (area == ar || ar->ar_stub_nosum)
	return;
    if ((db->db_lsa_ptr->lsh_type == LSA_TYPE_NET) 
	    ||  (db->db_lsa_ptr->lsh_type == LSA_TYPE_STUB)) {
	if (cost != SUM_LS_INFINITY) {
	    for (range = area->ar_range_list;range ; range = range->net_next) {
		if ((range->net_address & range->net_mask) 
		    == (db->db_dest & range->net_mask))
		    return;
	    }
	}
	/* if No db is preset create one */
	ospf_send_summary_lsa(&rdb,
			      ar,
			      db->db_dest,
			      db->db_mask,
			      cost,
			      LSA_TYPE_SUM_NET,
			      RTYPE_INTRA,
			      list);
    } else {
	/*
	 * Do not generate type 4 LSA into stub/NSSA area
	 * or from NSSA area, except for removing LSA.
	 */
	if ((cost != SUM_LS_INFINITY) && 
	    (ar->ar_stub || ar->ar_nssa || area->ar_nssa)) {
	    return;
	}
	rtr_lsa = (rtr_lsatype *)db->db_lsa_ptr;
	if ((rtr_lsa->la_E_B  & AS_BORDER_E_BIT)
	    || (cost == SUM_LS_INFINITY)) {
	    rdb = db_find_lsa(ar,
			      db->db_dest,
			      ROUTER_ID(ar),
			      LSA_TYPE_SUM_ASB);
	    /* if No db is preset create one */
	    build_sum_lsa(&rdb,
			  ar,
			  db->db_dest,
			  0,
			  cost,
			  LSA_TYPE_SUM_ASB,
			  RTYPE_INTRA,
			  list);
	} else 
	    return;
    }

} /* end of create_sum */


/*
 * generate_sum
 * takes a db from an area and
 * generates summaries into other
 * areas.
 */

void generate_sum (
    areatype *area,
    lsdbtype *db,
    ulong cost)
{
    list_queue flood_list;
    areatype *ar;
    pdbtype *pdb;

    pdb = area->ar_pdb_ptr;

    for (ar = pdb->ospf_area_list; ar ; ar = ar->ar_next) {
	flood_list.q_first = NULL;
	flood_list.q_last  = NULL;
 	create_sum(area, ar, db, cost, &flood_list);
	flood_area(ar, &flood_list, NULL, NULL);
    }
} /* end of generate_sum */


/*
 * delete_old_routes
 * deletes all invalid intra routes 
 * in the IP routing table from an
 * ospf area.
 */

static ushort delete_old_routes (areatype *area)
{
    lsdbtype *db, *rdb_db;
    net_lsatype *net_lsa;
    rtr_lsatype *rtr_lsa;
    ipaddrtype dest_addr;
    ushort change;
    pathtype *path;
    lsdbtype *temp;
    pdbtype *pdb;
    ushort dest_type;
    ipaddrtype source;
    addr_range *range;
    ndbtype *ndb;
    rdbtype *rdb;
    boolean found;
    int i;

    ospf_event(OC_GENERIC, (ulong) "delete_old_routes", area->ar_id);
    change = 0;
    dest_type = 0;
    db = area->ar_delete_list;
    pdb = area->ar_pdb_ptr;

    ospf_spf_intra_buginf("\nOSPF: Entered old delete routine");

     /* we are deleting paths in db_dpath_list which now are all unavailable 
      * (all the valid routes have been put into db_path_list and deleted 
      * from db_dpath_list).
      */
    for (; db ; db = db->db_rnext) {
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	ospf_spf_intra_acl_pass = acl_ospf_spf_intra_debug(LSA_ID(db));
	if (db->db_lsa_ptr->lsh_type == LSA_TYPE_RTR) {
	    rtr_lsa = (rtr_lsatype *)db->db_lsa_ptr;
	    source = rtr_lsa->la_hdr.lsh_adv_rtr;
	    if ((rtr_lsa->la_E_B & AS_AREA_BORDER_MASK)  
		==  AS_AREA_BORDER_MASK) {
		dest_type = (DTYPE_ABR | DTYPE_ASBR);
		change |= (SPF_SCHD_SNET | SPF_SCHD_ASE);
	    } else {
		if (rtr_lsa->la_E_B & AREA_BORDER_B_BIT) {
		    dest_type = DTYPE_ABR;
		    change |= SPF_SCHD_SNET;
		}
		if (rtr_lsa->la_E_B & AS_BORDER_E_BIT) {
		    dest_type = DTYPE_ASBR;
		    change |= SPF_SCHD_ASE;
		}
	    }
	    dest_addr = db->db_lsa_ptr->lsh_id;
	    if (db->db_ase_use || db->db_bdr_rtr)
		change |= SPF_SCHD_ASE;
	    for (path = db->db_dpath_list; path; path = path->pb_next)
		delete_rtr_route(area, dest_addr,0,path->pb_gateway, 
				 path->pb_firsthop_idb, RTYPE_INTRA, 
				 source);
	} else {
	    /*
	     * Remove old net or old stub net route
	     */
	    net_lsa = (net_lsatype *)db->db_lsa_ptr;
	    dest_addr = (net_lsa->la_hdr.lsh_id & net_lsa->la_mask);
	    source = net_lsa->la_hdr.lsh_adv_rtr;
	    for (path = db->db_dpath_list; path; path = path->pb_next) {
		/*
		 * Lookup ndb here as ndb could be removed in
		 * last iteration.
		 */
		ndb = ip_rtlookup(dest_addr, db->db_mask);
		if (!ndb || (ndb->pdbindex != pdb->index) || 
		    (ndb->route_type != RTYPE_INTRA)) {
		    ospf_spf_buginf("\nOSPF: No ndb for %sNET old route "
				    "%i, mask%#m, next hop %i",
				    (db->db_lsa_ptr->lsh_type ==
				     LSA_TYPE_STUB) ? "STUB " : "",
				    dest_addr, db->db_mask,
				    path->pb_gateway);
		    break;
		}
		found = FALSE;
		for (i = 0; i < ndb->routecount; i++) {
		    rdb = ndb->rdb[i];
		    if (rdb->gateway != path->pb_gateway)
			continue;
		    if (rdb->infosource != source)
			continue;
		    if (rdb->idb != path->pb_firsthop_idb)
			continue;
		    rdb_db = rdb->ospf_rdb_lsdb;
		    /*
		     * Make sure the rdb is installed by the
		     * same type of LSA as both network and
		     * stub network route can end up with
		     * the same parameters. And we don't
		     * want to remove a good network route
		     * when what we want is to remove the
		     * stub network route and vice versa.
		     */
		    if (rdb_db->db_lsa_ptr->lsh_type == 
			db->db_lsa_ptr->lsh_type)
			found = TRUE;
		    break;
		}
		if (found) {
		    ospf_spf_buginf("\nOSPF: Deleting %sNET old route %i, "
				    "mask%#m, next hop %i",
				    (db->db_lsa_ptr->lsh_type ==
				     LSA_TYPE_STUB) ? "STUB " : "",
				    dest_addr, db->db_mask, 
				    path->pb_gateway);
		    ip_route_delete(pdb, dest_addr, db->db_mask,
				    path->pb_gateway, source, 
				    path->pb_firsthop_idb,
				    RTYPE_INTRA);
		} else {
		    ospf_spf_buginf("\nOSPF: Old %sNET route %i, "
				    "mask%#m, next hop %i not exist",
				    (db->db_lsa_ptr->lsh_type ==
				     LSA_TYPE_STUB) ? "STUB " : "",
				    dest_addr, db->db_mask, 
				    path->pb_gateway);
		}
	    }
	}
    }
    
    /*
     * Process db on the delete list; first take it off from the delete
     * list, then check if there is any good path in db_path_list.
     * If yes, add it to the insert list, else invalidate the summary
     * network LSA that we might have generated into other areas.
     */
     
    db = area->ar_delete_list;
    for (; db ; db = temp) {
	temp = db->db_rnext;
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	ospf_spf_intra_acl_pass = acl_ospf_spf_intra_debug(LSA_ID(db));
 	/* delete and free path in db_dpath_list */
	if (db->db_ase_use || db->db_bdr_rtr) {
	    change |= SPF_SCHD_ASE;
	}
	delete_route_list(&(area->ar_delete_list), db, 0, 0);
	if (db->db_path_list) {
 	    /* put this db in db_insert_list, this can happen because
 	     * in addroute() when we call insert_route_list(), db_dpath_list
 	     * is not NULL.  Thus, we didn't put db in insert_list.
 	     */
	    insert_route_list(&(area->ar_insert_list), db);
	    process_dbf_ase_use(area, db); 
	} else {
	    db->db_ase_use = db->db_bdr_rtr = FALSE;
	    if (pdb->ospf_area_brtr) {
		if (db->db_lsa_ptr->lsh_type != LSA_TYPE_RTR) {
		    /* if db exactly matches the range and we are advertising
		     * this range, skip it.   Otherwise, flush no-longer
		     * reachable summary network.
		     */
		    range = find_area_range(area, db->db_dest);
		    if (range && (range->net_address == db->db_dest)
			&& (range->net_cost < SUM_LS_INFINITY))
			continue;
		}
		ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db));
		generate_sum(area, db, SUM_LS_INFINITY);
	    } 
	}
    }

    ospf_spf_intra_acl_pass = FALSE;
    /* 
     * now delete everything left in the delete_list.  Then move the insert
     * list (that is built in this round) to be the delete list (for next 
     * round of SPF) and generate summary network LSA into other areas as
     * needed.
     */
    delete_route_list(&(area->ar_delete_list), NULL, 0, 0);
    area->ar_delete_list = area->ar_insert_list;
    area->ar_insert_list = NULL;

    if (pdb->ospf_area_brtr) {
	db = area->ar_delete_list;
	for (; db ; db = db->db_rnext) {
 	    if (db->db_ar_range) {
		ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db));
 		generate_sum(area, db, SUM_LS_INFINITY); 
 		db->db_ar_range = FALSE;
	    } else {
		if (db->db_dest_match_ar_range) 
		    db->db_dest_match_ar_range = FALSE;
		else {
		    /*
		     * if this is a router LSA and it is not an ASBR,
		     * don't generate summary for it.
		     */
		    if ((db->db_lsa_ptr->lsh_type == LSA_TYPE_RTR)
			&& !db->db_bdr_rtr) 
			continue;
		    ospf_spf_inter_acl_pass = 
			acl_ospf_spf_inter_debug(LSA_ID(db));
		    generate_sum(area, db, db->db_dist);
		}
	    }
	}
    }
    ospf_spf_inter_acl_pass = FALSE;
    return(change);
}/* end of delete_old_routes */

/*
 * ex_delete_old_routes
 * deletes all invalid external routes 
 * in the IP routing table from an
 * ospf process.
 */

static void ex_delete_old_routes (pdbtype *pdb, areatype *area, uchar lsa_type)
{
    lsdbtype *db;
    ex_lsatype *ex_lsa;
    pathtype *path;
    lsdbtype *temp;
    ipaddrtype source;
    lsdbtype **insert_list, **delete_list;
    list_queue flood_list;
    ulong lsid, mask;
    ndbtype *ndb;

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    ospf_event(OC_GENERIC, (ulong) "ex_delete_old_routes", lsa_type);
    ospf_spf_ext_buginf("\nOSPF: ex_delete_old_routes");

    if (lsa_type == LSA_TYPE_ASE) {
	delete_list = &pdb->ospf_ex_delete;
	insert_list = &pdb->ospf_ex_insert;
    } else {
	delete_list = &area->ar_ex_delete_list;
	insert_list = &area->ar_ex_insert_list;
    }

    for (db = *delete_list; db ; db = db->db_rnext) {
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	ospf_spf_ext_acl_pass = acl_ospf_spf_ext_debug(LSA_ID(db));
	ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	lsid = ex_lsa->la_hdr.lsh_id;
	mask = ex_lsa->la_mask;
	ospf_spf_ext_acl_buginf("\nOSPF: ex-Deleting old route %i", lsid);
	source = db->db_lsa_ptr->lsh_adv_rtr;
	for (path = db->db_dpath_list; path; path = path->pb_next) {
	    ospf_spf_ext_acl_buginf("\nOSPF: Remove %i %i %i %i %s %x %x",
				    lsid & mask, mask, path->pb_gateway,
				    source, path->pb_firsthop_idb->namestring,
				    EXLSA_TO_ROUTE_TYPE(ex_lsa),
				    ex_lsa->la_metric.as_e_tos_metric);
	    ip_route_delete(pdb, lsid & mask, mask, path->pb_gateway, source, 
			    path->pb_firsthop_idb,
			    EXLSA_TO_ROUTE_TYPE(ex_lsa));
	}
	if (lsa_type == LSA_TYPE_7_ASE) {
	    ndb = ospf_lookup_net_route(area, lsid & mask, mask, 0, FALSE);
	    if (ndb)
		continue;
	    ospf_translate_one_type7(pdb, ndb, lsid, mask, &flood_list, TRUE);
	}
    }
    for (db = *delete_list; db ; db = temp) {
	ospf_spf_ext_acl_pass = acl_ospf_spf_ext_debug(LSA_ID(db));	
	temp = db->db_rnext;
	delete_route_list(delete_list, db, 0, 0);
	if (db->db_path_list)
	    insert_route_list(insert_list, db);
    }
    delete_route_list(delete_list, NULL, 0, 0);
    *delete_list = *insert_list;
    *insert_list = NULL;
    ospf_spf_ext_acl_pass = FALSE;
    ex_flood_area(pdb, &flood_list, NULL, NULL);
}/* end of ex_delete_old_routes */

/*
 * sum_delete_old_routes
 * deletes all invalid summary routes 
 * in the IP routing table from an
 * ospf area.
 */

static void sum_delete_old_routes (areatype *area)
{
    lsdbtype *db;
    sum_lsatype *sum_lsa;
    pathtype *path;
    lsdbtype *temp;
    pdbtype *pdb;
    ipaddrtype source;

    ospf_event(OC_GENERIC, (ulong) "sum_delete_old_routes", area->ar_id);
    ospf_spf_inter_buginf("\nOSPF: sum_delete_old_routes area %s", 
			  area->ar_id_str);
    pdb = area->ar_pdb_ptr;
    db = area->ar_sum_delete_list;
    for (; db ; db = db->db_rnext) {
 	check_ospf_suspend(pdb, DBASE_ACTIVE);
	ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db));
	sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
	ospf_spf_inter_acl_buginf("\nOSPF: sum-Deleting old route %i",
				  sum_lsa->la_hdr.lsh_id);
	source = db->db_lsa_ptr->lsh_adv_rtr;
	for (path = db->db_dpath_list; path; path = path->pb_next) {
	    if (db->db_lsa_ptr->lsh_type == LSA_TYPE_SUM_ASB)
		delete_rtr_route(area, sum_lsa->la_hdr.lsh_id,
				 DTYPE_ASBR, path->pb_gateway, 
				 path->pb_firsthop_idb, RTYPE_INTER,
				 source);
	    else
		ip_route_delete(pdb, 
				(sum_lsa->la_hdr.lsh_id & sum_lsa->la_mask), 
				sum_lsa->la_mask, path->pb_gateway, source, 
				path->pb_firsthop_idb, RTYPE_INTER);
	}
    }
    db = area->ar_sum_delete_list;
    for (; db ; db = temp) {
	ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db));
	temp = db->db_rnext;
	delete_route_list(&(area->ar_sum_delete_list), db, 0, 0);
	if (db->db_path_list)
	    insert_route_list(&(area->ar_sum_insert_list), db);
	else {
	    db->db_abr_flag = FALSE;
	    if ((pdb->ospf_area_brtr) && (area->ar_id == BACKBONE_AREA_ID))
		build_inter_route_all(db, area);
	} 
    }
    delete_route_list(&(area->ar_sum_delete_list), NULL, 0, 0);
    area->ar_sum_delete_list = area->ar_sum_insert_list;
    area->ar_sum_insert_list = NULL; 
    if ((pdb->ospf_area_brtr) && (area->ar_id == BACKBONE_AREA_ID)) {
	db = area->ar_sum_delete_list;
	for (; db ; db = db->db_rnext) {
	    ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db));
	    build_inter_route_all(db, area);
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	}
    }
    ospf_spf_inter_acl_pass = FALSE;
}/* end of sum_delete_old_routes */

/*
 * find_db
 * searches db list for the db that
 * contributed the route to a destination
 * and returns the pointer.
 */

static lsdbtype * find_db (
    areatype 	*area,
    ulong	ls_id,
    ushort	rtr_type)
{
    lsdbtype *db;

    if (rtr_type & RTYPE_INTRA)
	db = area->ar_delete_list;
    else
	db = area->ar_sum_delete_list;

    for (; db ; db = db->db_rnext) {
	if ((db->db_lsa_ptr->lsh_id == ls_id)
	    || (db->db_dest == ls_id))
	    return(db);
    }
    return(NULL);
} /* find_db */

/*
 * addroute
 * takes a db from an area and
 * adds its route to the routing table.
 */

static void addroute (areatype *area, lsdbtype *db)
{
    rtr_lsatype *rtr_lsa;
    net_lsatype *net_lsa;
    ipaddrtype  dest_addr;
    ushort	dest_type;
    rdbtype	*rdb;
    pdbtype	*pdb;
    pathtype	*path;
    addr_range	*range;
    ulong       source;
    boolean     valid_route;
    ndbtype	*ndb;

    pdb = area->ar_pdb_ptr;
    if (db->db_lsa_ptr->lsh_type == LSA_TYPE_RTR) {
	db->db_rtr_reachable = TRUE;
	rtr_lsa = (rtr_lsatype *)db->db_lsa_ptr;

	if ((rtr_lsa->la_E_B & AS_AREA_BORDER_MASK) == AS_AREA_BORDER_MASK) {
	    dest_type = (DTYPE_ABR | DTYPE_ASBR);
	    area->ar_bdr_count++;
	    area->ar_asr_count++;
	} else
	    if (rtr_lsa->la_E_B & (AREA_BORDER_B_BIT)) {
		area->ar_bdr_count++;
		dest_type = DTYPE_ABR;
	    } else
		if (rtr_lsa->la_E_B & (AS_BORDER_E_BIT)) {
		    area->ar_asr_count++;
		    dest_type = DTYPE_ASBR;
		} else
		    return;
	db->db_bdr_rtr = TRUE;
	db->db_dest = rtr_lsa->la_hdr.lsh_id;
	source = rtr_lsa->la_hdr.lsh_adv_rtr;
	for (path = db->db_path_list; path; path = path->pb_next) {
	    if (db->db_rt_table)
		delete_route_list(&(area->ar_delete_list), db, 
				  path->pb_gateway, path->pb_firsthop_idb);
	    ospf_spf_intra_acl_buginf("\nOSPF: Add Router Route to %i "
				      "via %i. Metric: %d", db->db_dest, 
				      path->pb_gateway, db->db_dist);
	    update_rtr_route(area, db, dest_type, RTYPE_INTRA,
			     path->pb_gateway, path->pb_firsthop_idb,
			     source);
	}
	insert_route_list(&(area->ar_insert_list), db);
	process_dbf_ase_use(area, db); 
    } else {
	if ((db->db_lsa_ptr->lsh_type == LSA_TYPE_NET) 
	    || (db->db_lsa_ptr->lsh_type == LSA_TYPE_STUB)) { 
	    net_lsa = (net_lsatype *)db->db_lsa_ptr;
	    dest_addr = (net_lsa->la_hdr.lsh_id & net_lsa->la_mask);
	    pdb->ospf_area_id = area->ar_id;
	    pdb->spf_instance = area->ar_spf_instance;
	    source =  net_lsa->la_hdr.lsh_adv_rtr;
 	    pdb->ospf_ex_forw_dist = 0;
 	    db->db_dest = dest_addr;
 	    db->db_mask = net_lsa->la_mask;
	    /*
	     * Now if we are an area border router, 
	     * Check if this route falls into any range and indicate that 
	     * with db_ar_range flag so we will maxage it out.   One
	     * exception case if the range and mask match this db
	     * EXACTLY, set db_dest_match_ar_range so we will not 
	     * maxage it. (otherwise we can maxage the summarized LSA that
	     * we just generate for the range)
	     */
	    if (pdb->ospf_area_brtr) {
		range = find_area_range(area, dest_addr);
 		if (range) {
		    if ((range->net_address == db->db_dest) &&
			(range->net_mask == db->db_mask))
			db->db_dest_match_ar_range = TRUE;
		    else
			db->db_ar_range = TRUE;

		    if (range->net_cost > db->db_dist) {
			range->net_cost = db->db_dist;
		    }
		}
	    }
	    /*
	     * We do not add path if
	     * (1) the destination is our interface address.
	     * (2) the destination is a connected network.
	     *     It prevents us to create a backup entry, which
	     *	   gets installed when the ip address is changed.
	     */	     
	    valid_route = !ip_ouraddress(dest_addr);
	    if (valid_route) {
		ndb = ip_rtlookup(dest_addr, net_lsa->la_mask);
		if (ndb) {
		    if (NDB2PROCTYPE(ndb) == PROC_CONNECTED) {
			valid_route = FALSE;
		    }
		}
	    }
	    for (path = db->db_path_list; path; path = path->pb_next) {
		/* delete the valid path from the db_dpath_list which
		 * we have from the previous SPF.   i.e. we will have
		 * all valid paths in db_path_list and all no-longer
		 * valid paths in db_dpath_list.
		 */
		if (db->db_rt_table)
		    delete_route_list(&(area->ar_delete_list), db, 
				      path->pb_gateway, 		     
				      path->pb_firsthop_idb);
		if (valid_route) {
		    rdb = network_update(pdb, dest_addr, net_lsa->la_mask,
					 path->pb_gateway,
					 path->pb_firsthop_idb, db->db_dist,
					 0, source,
					 0, NULL,
					 RTYPE_INTRA);
		    if (rdb) {
			ospf_spf_intra_acl_buginf("\nOSPF: Add Network "
						  "Route to %i Mask%#m. "
						  "Metric: %d, Next Hop: %i",
						  db->db_dest,
						  net_lsa->la_mask,
						  db->db_dist,
						  path->pb_gateway);
			rdb->ospf_rdb_lsdb = db;
			rdb->ospf_rdb_area = area;
		    }
		}
	    }
	    insert_route_list(&(area->ar_insert_list), db);
	    process_dbf_ase_use(area, db); 
	}
    }
} /* end of addroute */


/*
 * add_ex_route
 * takes a external db 
 * adds its route to the routing table.
 */


static void add_ex_route (
    pdbtype	*pdb,
    areatype	*area,
    lsdbtype 	*db,
    ushort 	route_type,
    boolean	flag)
{
    ex_lsatype *ex_lsa;
    ipaddrtype  dest_addr;
    ipaddrtype  source;
    rdbtype	*rdb;
    pathtype	*path;
    ulong       lasttime, currenttime, time[3];
    lsdbtype	**insert_list, **delete_list;

    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
    dest_addr = (ex_lsa->la_hdr.lsh_id & ex_lsa->la_mask);
    if (ip_ouraddress(dest_addr)) {
 	/*
 	 * If we won't install the route, delete the path in db_path_list
 	 * so db won't be put on the delete list
 	 */
 	free_plist(db);
  	return;
    }
    pdb->ospf_area_id = 0;
    source = ex_lsa->la_hdr.lsh_adv_rtr;
    pdb->spf_instance = pdb->local_ospf_spf_instance;
    pdb->ospf_ex_forw_dist = db->db_forw_dist;
    pdb->ospf_nssa_p_bit = ex_lsa->la_hdr.lsh_options & OPTION_N_P_BIT;
    pdb->ospf_ex_forw_addr = ex_lsa->la_metric.as_forw_addr;
    db->db_dest = dest_addr;
    db->db_mask = ex_lsa->la_mask;

    if (LSA_TYPE(db) == LSA_TYPE_ASE) {
	delete_list = &pdb->ospf_ex_delete;
	insert_list = &pdb->ospf_ex_insert;
    } else {
	delete_list = &area->ar_ex_delete_list;
	insert_list = &area->ar_ex_insert_list;
	pdb->ospf_area_id = area->ar_id;
    }

    lasttime = runtime();

    if (db->db_rt_table) {
	for (path = db->db_path_list; path; path = path->pb_next)
	    delete_route_list(delete_list, db, path->pb_gateway, 
			      path->pb_firsthop_idb);
    }
    /* add paths */

    currenttime = runtime();
    time[0] = currenttime - lasttime;
    lasttime = currenttime;	    

    for (path = db->db_path_list; path; path = path->pb_next) {
	if (ip_ouraddress(path->pb_gateway)) {
	    ospf_spec_buginf("\nOSPF: Ignore ext. route to %i with "
			     "next hop %i to our own interface", 
			     dest_addr, path->pb_gateway);
	    continue;
	}
	rdb = network_update(pdb, dest_addr, ex_lsa->la_mask,
			     path->pb_gateway, path->pb_firsthop_idb,
			     db->db_dist, 0,
			     source,
			     ex_lsa->la_metric.as_ex_tag, NULL,
			     route_type);
	if (rdb) {
	    ospf_spf_ext_acl_buginf("\n   Add External Route to %i. "
				    "Metric: %d, Next Hop: %i",
				    db->db_dest, db->db_dist,
				    path->pb_gateway);
	    rdb->ospf_rdb_lsdb = db;
	    rdb->ospf_rdb_area = area;
	}
    }

    currenttime = runtime();
    time[1] = currenttime - lasttime;
    lasttime = currenttime;	    
    
    if (flag)
        insert_route_list(delete_list, db);
    else 
        insert_route_list(insert_list, db);

    currenttime = runtime();
    time[2] = currenttime - lasttime;
    lasttime = currenttime;	    

    write_circular(ADD_EX_TIME, time, 3 /* there are three steps */);
} /* end of add_ex_route */

/*
 * add_sum_route
 * takes a summary db and
 * adds its route to the routing table.
 */

static void add_sum_route (
    areatype	*area,
    lsdbtype 	*db,
    boolean	flag)
{
    sum_lsatype *sum_lsa;
    ipaddrtype  dest_addr;
    ushort	dest_type;
    rdbtype	*rdb;
    pathtype	*path;
    pdbtype	*pdb;
    ipaddrtype  source;
    
    pdb = area->ar_pdb_ptr;
    sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
    source = sum_lsa->la_hdr.lsh_adv_rtr;
    if (db->db_lsa_ptr->lsh_type == LSA_TYPE_SUM_ASB) {
	db->db_rtr_reachable = TRUE;
	dest_addr = sum_lsa->la_hdr.lsh_id;
  	if (ip_ouraddress(dest_addr)) {
  	    /*
  	     * If we won't install the route, delete the path in db_path_list
  	     * so db won't be put on the delete list
  	     */
  	    free_plist(db);
   	    return;
  	}
	dest_type = DTYPE_ASBR;
	db->db_bdr_rtr = TRUE;
	db->db_dest = dest_addr;
	db->db_mask = sum_lsa->la_mask;
	for (path = db->db_path_list; path; path = path->pb_next) {
	    if (db->db_rt_table)
		delete_route_list(&(area->ar_sum_delete_list), db,
				  path->pb_gateway, 
				  path->pb_firsthop_idb);
	    ospf_spf_inter_acl_buginf("\nOSPF: Add Router Route to %i from "
				      "summary ASBR LSA. Metric: %d", 
				      db->db_dest, db->db_dist);
	    update_rtr_route(area, db, dest_type, RTYPE_INTER,
			     path->pb_gateway, path->pb_firsthop_idb, 
			     source);
	}
	if (flag)
	    insert_route_list(&(area->ar_sum_delete_list), db);
	else
	    insert_route_list(&(area->ar_sum_insert_list), db);
    } else {
	dest_addr = (sum_lsa->la_hdr.lsh_id & sum_lsa->la_mask);
  	if (ip_ouraddress(dest_addr)) {
  	    /*
  	     * If we won't install the route, delete the path in db_path_list
  	     * so db won't be put on the delete list
  	     */
  	    free_plist(db);
   	    return;
  	}
	pdb->ospf_area_id = area->ar_id;
	pdb->spf_instance = pdb->local_ospf_spf_instance;
	pdb->ospf_ex_forw_dist = 0;

	if (db->db_rt_table) {
	    for (path = db->db_path_list; path; path = path->pb_next) {
		delete_route_list(&(area->ar_sum_delete_list), db, 
				  path->pb_gateway,
				  path->pb_firsthop_idb);
	    }
	}
	/* add paths */
	db->db_dest = dest_addr;
	db->db_mask = sum_lsa->la_mask;
	for (path = db->db_path_list; path; path = path->pb_next) {
	    rdb =  network_update(pdb,
				  dest_addr,
				  sum_lsa->la_mask,
				  path->pb_gateway,
				  path->pb_firsthop_idb,
				  db->db_dist,
				  0,
				  source,
				  0, NULL,
				  RTYPE_INTER);
	    if (rdb) {
		ospf_spf_inter_acl_buginf("\n   Add Summary Route to "
					  "%i. Metric: %d, Next Hop: %i",
					  db->db_dest, db->db_dist,
					  path->pb_gateway);
		rdb->ospf_rdb_lsdb = db;
		rdb->ospf_rdb_area = area;
	    }
	}
        if (flag)
	    insert_route_list(&(area->ar_sum_delete_list), db);
	else
	    insert_route_list(&(area->ar_sum_insert_list), db);
    }
} /* end of add_sum_route */

/*
 * free_plist
 * takes a database blocks and frees
 * all path blocks in it. 
 */

void free_plist (lsdbtype *db)
{
    pathtype	*path_ptr;
    pathtype	*temp_ptr;

    if (db->db_path_list == NULL)
	return;
    for(path_ptr = db->db_path_list;path_ptr; path_ptr = temp_ptr) {
	temp_ptr = path_ptr->pb_next;
	path_ptr->pb_next = NULL;
	free(path_ptr);
    }
    db->db_path_list = NULL;
    db->db_path_count = 0;
} /* end of free_plist */

/*
 * ospf_ipaddr_to_idb
 * takes an address returns
 * its idb, if the address is local
 * Check against secondary addresses too if secondary is TRUE
 */

static idbtype * ospf_ipaddr_to_idb (areatype *area, ipaddrtype address, 
			      boolean secondary)
{
    idbtype *idb;
    ipaddtype *ptr;
    
    for (idb = area->ar_if_list; idb; idb = idb->ospf_info->next) {
	/*
	 * Ignore interface that is not up	
	 */
	if (idb->ospf_info->if_state == IFS_DOWN)
	    continue;
        if (idb->ospf_info->if_type == IF_VIRTUAL_LINK)
            continue;
	if (idb->ip_unnumbered)
	    continue;
        if (address == idb->ip_address)
            return(idb->output_if);
	if (secondary) {
	    for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	        if (ptr->address == address)
		    return(idb->output_if);
	    }	   
	}
    }
    return(NULL);
} /* end of ospf_ipaddr_to_idb */

/*
 * map_to_unnumbered_idb
 * takes router id and ifIndex, maps it to the right unnumbered interface.
 * ifIndex comes from our own router LSA so it is locally significant.
 * A next hop address is returned in if_ip_addr.
 */

static idbtype *map_to_unnumbered_idb (areatype *area, ipaddrtype router_id,
				       ulong ifIndex, ulong *if_ip_addr)
{
    idbtype *idb;
    nbrtype *nbr;
    
    for (idb = area->ar_if_list; idb; idb = idb->ospf_info->next) {
	if (!idb->ip_unnumbered || (idb->if_number != ifIndex))
	    continue;
	for (nbr = idb->ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	    if ((nbr->nbr_id == router_id) && (nbr->nbr_state == NBR_FULL)) {
		*if_ip_addr = nbr->nbr_ipaddr; 
		return(idb);
	    }
	}
    }
    return(NULL);
} /* end of map_to_unnumbered_idb */

/*
 * add_to_plist:
 * takes all the arguments required and 
 * adds a path block.
 */

static pathtype * add_to_plist (
    lsdbtype 	*db,	/* pointer to database element pointer */
    lsdbtype	*parent,	/* pointer to parent in spf tree */
    lsdbtype	*nexthop,	/* pointer to next hop router */
    idbtype	*idb,
    ipaddrtype	nh_if_addr)	/* interface address */
{
    pathtype	*path;

    /*
     * avoid adding path if there is 
     * already one with same nexthop and interface.
     */
    for (path = db->db_path_list; path ; path = path->pb_next) {
	if ((path->pb_gateway == nh_if_addr) &&
	    (path->pb_firsthop_idb == idb)) {
	    ospf_spf_buginf("\n         Exist path: next-hop %i, interface %s",
			    nh_if_addr, idb->namestring);
	    return(NULL);
	}
    }
    path = malloc(sizeof(pathtype));
    if (path) {
	path->pb_parent_ptr = parent;
	path->pb_firsthop_ptr = nexthop;
	path->pb_firsthop_idb = idb;
	path->pb_gateway = nh_if_addr;
	path->pb_next = db->db_path_list;
	db->db_path_list = path;
 	db->db_path_count++;
	ospf_spf_acl_buginf("\n   Add path: next-hop %i, interface %s",
			    nh_if_addr, idb->namestring);
	return(path);
    }
    return(NULL);
} /* end of add_to_plist */

/*
 * enq_clist:
 * takes area and db emement as arguments
 * and put it in asending order in clist.
 */

static void enq_clist(areatype *area, lsdbtype *db)
{
    lsdbtype *temp;

    if (db->db_where == LSDB_CLIST) {
	ospf_spf_intra_acl_buginf("\n   LSA already on the clist",
				  "\n   LS id. %i, Type %d, Adv Rtr. %i", 
				  LSA_ID(db), LSA_TYPE(db), ADV_RTR(db));
	return;
    }
		
    db->db_next = db->db_prev = NULL;
    if (!(temp = area->ar_clist))
	area->ar_clist = db;
    else {
	if ((temp->db_dist > db->db_dist) 
	    || ((temp->db_dist == db->db_dist)
		&& (LSA_TYPE(temp) == LSA_TYPE_RTR))) {
	    db->db_next = temp;
	    temp->db_prev = db;
	    area->ar_clist = db;
	} else {
	    /* search the list for right place */
	    for (; (temp->db_next && temp->db_next->db_dist < db->db_dist) ;
		 temp = temp->db_next);
	    
	    if (LSA_TYPE(db) == LSA_TYPE_RTR) {
		for (; (temp->db_next && temp->db_next->db_dist == db->db_dist)
		     && (LSA_TYPE(temp->db_next) == LSA_TYPE_NET);
		     temp = temp->db_next);
	    }
 	    db->db_next = temp->db_next;
	    if (db->db_next)
		db->db_next->db_prev = db;
	    temp->db_next = db;
	    db->db_prev = temp;
	}
    }
    db->db_where = LSDB_CLIST;
} /* end of enq_clist */

/*
 * deq_clist:
 * takes area and db element as arguments
 * and removes it from clist.
 */

static void deq_clist(areatype *area, lsdbtype *db)
{
    if (db->db_where == LSDB_CLIST) {
	if (area->ar_clist) {
	    if (db->db_prev == NULL) {
		/* it is the first element */
		area->ar_clist = db->db_next;
		if (db->db_next)
		    db->db_next->db_prev = NULL;
	    } else {	
		if (db->db_next == NULL)
		    /* it is at the end of the list */
		    db->db_prev->db_next = NULL;
		else {
		    /* it is in the middle of the list */
		    db->db_prev->db_next = db->db_next;
		    db->db_next->db_prev = db->db_prev;
		}
	    }
	}
	db->db_next = NULL;
	db->db_prev = NULL;
	db->db_where = LSDB_UNINIT;
    }
} /* end of deq_clist */

/*
 * add_path:
 * add a path to vertex.
 * ip_our_addr is our IP address while if_ip_addr is the neigbhor's 
 * IP address
 * if_our_addr is used to select the right output interface when
 * there are more than one interface in the same IP subnets.
 */

static void add_path (
    lsdbtype	*vertex,
    lsdbtype	*parent,
    ulong	newdist,
    areatype 	*area,
    ipaddrtype	if_our_addr,
    ipaddrtype	if_ip_addr)	/* if attached to this rtr, nbrs IP address */
{
    pathtype	*path;
    idbtype	*idb;
    boolean	local_address;
    ipaddtype	*ptr;

    /* 
     * return if the vertex is same as root
     * as we can not add paths to root from root
     */
    if (area && (vertex == area->ar_spf_ptr))
	return;
    if (vertex->db_path_list == NULL) {
	vertex->db_dist = newdist;
	/*
	 * put it in order on the candidate list,
	 * only router and net link advertisemets.
	 */
	if ((LSA_TYPE(vertex) < LSA_TYPE_SUM_NET) 
	    && !(STUB_NET(vertex)))
	    enq_clist(area, vertex);
    } else {
	/* if this path is already in there 
	 * avoid adding 
	 */
	for (path = vertex->db_path_list; path ; path = path->pb_next) {

	    if ((path->pb_parent_ptr == parent) 
		&& (if_ip_addr == path->pb_gateway))  {
		ospf_spf_acl_buginf("\n   Path with gateway %i, interface "
				    "address %i is already in the node's "
				    "database",
				    path->pb_gateway, if_ip_addr);
		return;
	    }
	}
    }
    /*
     * Calculate output interface given next hop (if_ip_addr), 
     * parent and vertex
     *
     * Possible parent/vertex combination
     * For intra route calculation
     * - Parent : router LSA; Vertex : network LSA
     * - Parent : router LSA; Vertex : router LSA
     * - Parent : network LSA; Vertex : router LSA
     * - Parent : router LSA; Vertex : stub LSA
     *
     * For inter route calculation
     * - Parent : router LSA; Vertex : summary/summary-asbr LSA
     *
     * For external route calculation
     * - Parent : router LSA; Vertex : external LSA (no forwarding address)
     * - Parent : network LSA; Vertex : external LSA (with forwarding address)
     * - Parent : stub LSA; Vertex : external LSA (with forwarding address)
     *
     */
    idb = NULL;
    if ((LSA_TYPE(parent) == LSA_TYPE_RTR) && 
	(ADV_RTR(parent) ==  ROUTER_ID(area))) {
        /*
	 * Only intra route calculation with 
	 * self-originated router LSA as parent can reach here
	 */
	if (STUB_NET(vertex))
	    if_our_addr = if_ip_addr = if_addr_to_stub(area, vertex);
	if ((!VALID_IPADDRESS(if_ip_addr)) 
	    && (LSA_TYPE(vertex) == LSA_TYPE_RTR)) {
	    /*
	     * This is the case of unnumbered link connecting two routers.
	     * if_ip_addr contains ifIndex.
	     */
	    idb = map_to_unnumbered_idb(area, ADV_RTR(vertex), 
					if_ip_addr, &if_ip_addr);
	} else {
	    idb = ospf_ipaddr_to_idb(area, if_our_addr, TRUE);
	    /*
	     * We may fail if we are adding path for virtual link,
	     * check OSPF route for output interface.
	     * Note that we do not have direct information to tell
	     * us if it is virtual link, but the following check
	     * is good enough to ensure it.
	     */
	    if (!idb && (area->ar_id == BACKBONE_AREA_ID) && 
		(LSA_TYPE(vertex) == LSA_TYPE_RTR))
	        idb = ospf_get_idb(area, if_ip_addr);
	}
	if (idb == NULL) {
	    ospf_spf_acl_buginf("\n   Add path fails: no output interface"
				" to %i, next hop %i", vertex->db_dest, 
				if_ip_addr);
	    return;
	}
	(void)add_to_plist(vertex, parent, parent, idb, if_ip_addr);
	vertex->db_direct = TRUE;
    } else {
	if (parent->db_direct && ((LSA_TYPE(parent) == LSA_TYPE_NET ) 
				  || (LSA_TYPE(parent) == LSA_TYPE_STUB))) {
	    /* 
	     * We come to here with three cases.
	     * 1. For intra route calculation, with network LSA as parent 
	     *    and router LSA as vertex.
	     * 2. For external route calculation, with network LSA as
	     *    parent and external LSA as vertex.
	     * 3. For external route calculation, with stub LSA as parent
	     *    and external LSA as vertex.
	     * Note that the parent network must be connected in this case.
	     */
	    for (path = parent->db_path_list; path ; path = path->pb_next) {
		idb = path->pb_firsthop_idb;
		/*
		 * We inherit the output interface from parent.
		 * If the parent's next hop is on the same net as 
		 * if_ip_addr, we use if_ip_addr as next hop because
		 * parent's next hop will be a local address; Otherwise,
		 * we inherit the next hop too.
		 */
		local_address = FALSE;
		if ((if_ip_addr & idb->ip_nets_mask) ==
		    (idb->ip_address & idb->ip_nets_mask)) {
		    local_address = TRUE;
		} else { 
		    for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
			 ptr = ptr->next)
			if ((if_ip_addr & ptr->mask) == 
			    (ptr->address & ptr->mask))
			    local_address = TRUE;
		}
		if (local_address)
		    (void)add_to_plist(vertex, parent, path->pb_firsthop_ptr,
				       path->pb_firsthop_idb, if_ip_addr);
		else
		    (void)add_to_plist(vertex, parent, path->pb_firsthop_ptr,
					       path->pb_firsthop_idb, 
					       path->pb_gateway);
	    }
	} else {
	    /* 
	     * Bind to parent's next hop and output interface for
	     * all remaining cases.
	     * 1. Intra route calculation where the parent network is not
	     *    connected.
	     * 2. Inter route calculation
	     * 3. External route calculation without forwarding address
	     * 4. External route calculation with forwarding address but
	     *    the parent network is not connected.
	     */
	    for (path = parent->db_path_list; path ; path = path->pb_next) {
		(void)add_to_plist(vertex,
				   parent,
				   path->pb_firsthop_ptr,
				   path->pb_firsthop_idb,
				   path->pb_gateway);
	    }
	}
    }
} /* end of add_path */

/*
 * add_better_path:
 * set the parent list to this parent only; 
 * spf has discovered this shorter path
 * so reinsert candidate in new order
 */

static void add_better_path (
    lsdbtype	*vertex,
    lsdbtype	*parent,
    ulong	newdist,
    areatype	*area,
    ipaddrtype	if_our_addr,
    ipaddrtype	if_ip_addr,
    pdbtype	*pdb)
{
    ospf_spf_acl_buginf("\n   Add better path to LSA ID %i, gateway %i, "
			"dist %d", vertex->db_lsa_ptr->lsh_id, if_ip_addr,
			newdist);
    if (area &&(vertex == area->ar_spf_ptr)) 
	return;
    /* free all the paths */
    free_plist(vertex);
    /* if not only one on list */
    if ((LSA_TYPE(vertex) < LSA_TYPE_SUM_NET) && !(STUB_NET(vertex))) {
	/* remove from candidate list */
	deq_clist(area, vertex);
	vertex->db_direct = FALSE;
    }
    if (area)
	add_path (vertex, parent, newdist, area, if_our_addr, if_ip_addr);
    else
	add_path (vertex, parent, newdist, pdb->ospf_pdb->ospf_dummy_area,
		  if_our_addr, if_ip_addr);
} /* end of add_better_path */

/*
 * getnext_clist:
 * Gets then next candidate that is closest 
 * to the root
 */

static lsdbtype * getnext_clist (areatype *area)
{
    lsdbtype *temp;

    if ((temp = area->ar_clist) == NULL) 
	return(NULL);
    area->ar_clist = temp->db_next;
    if (area->ar_clist != NULL)
	area->ar_clist->db_prev = NULL;
    temp->db_next =  NULL;
    temp->db_where = LSDB_UNINIT;
    return(temp);
} /* end of getnext_clist */

/*
 * if_addr_to_stub
 *  Given a net number return
 *  interface address if directly connected
 */

static ipaddrtype if_addr_to_stub (areatype *area, lsdbtype * vertex)
{
    idbtype *idb;
    net_lsatype *net_lsa;
    ipaddtype *ptr;
    ulong dest;

    net_lsa = (net_lsatype *)vertex->db_lsa_ptr;
    dest = net_lsa->la_hdr.lsh_id & net_lsa->la_mask;
    for( idb = area->ar_if_list; idb ; idb = idb->ospf_info->next) {
	if (idb->ospf_info->if_type == IF_VIRTUAL_LINK)
	    continue;
	if (idb->ospf_info->if_state == IFS_DOWN)
	    continue;
	if ((dest & idb->ip_nets_mask) == 
	    (idb->ip_address & idb->ip_nets_mask))
	    return(idb->ip_address);
	/* compare with all the secondary addresses also */
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
	     ptr = ptr->next) {
	    if ((dest & ptr->mask) == (ptr->address & ptr->mask))
		return(ptr->address);
	}
    }
    return((ipaddrtype )0);
} /* end of if_addr_to_stub */

/* 
 * delete_route_old_mask
 * called when receiving a LSA that we already have but the mask
 * has been changed. 
 */
void delete_route_old_mask (
    pdbtype  *pdb,
    areatype *area,
    lsdbtype *db)
{
    pathtype *path, *other_path;
    ex_lsatype *ex_lsa;
    lsdbtype *other_db = NULL, **route_list = NULL;
    ushort route_type;
    ipaddrtype source;
    ndbtype *ndb;
    list_queue flood_list;
    boolean delete_all = FALSE;

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    if (!db->db_mask_change) 
        return;
    db->db_mask_change = FALSE;

    if (db->db_rt_table) {
        ex_lsa = (ex_lsatype *) db->db_lsa_ptr;
        switch (LSA_TYPE(db)) {
            case LSA_TYPE_STUB : 
	    case LSA_TYPE_RTR  :
            case LSA_TYPE_NET  :
	        route_type = RTYPE_INTRA;
		route_list = &(area->ar_delete_list);
                break;
            case LSA_TYPE_SUM_NET :
            case LSA_TYPE_SUM_ASB :
	        route_type = RTYPE_INTER;
                route_list = &(area->ar_sum_delete_list);
                break;
            case LSA_TYPE_ASE :
                route_type = EXLSA_TO_ROUTE_TYPE(ex_lsa);
		route_list = (lsdbtype **) &(pdb->ospf_ex_delete);
		break;
	    case LSA_TYPE_7_ASE:
		route_type = EXLSA_TO_ROUTE_TYPE(ex_lsa);
		route_list = (lsdbtype **) &(area->ar_ex_delete_list); 
		break;
	    default :
	        return;
        }
	source = ADV_RTR(db);
        if (route_type == RTYPE_INTRA) {
            /*
             * Intra route
	     * Only mask could changed, remove the all old paths.
             */
	    delete_all = TRUE;
	} else {
            /*
             * Inter route
	     * Since we have host-bit-set LSA, it could be the case that the 
	     * LSA with the old mask now exist using a different (probably
	     * host-bit-set) link state ID.
             * If that LSA exist and already contribute route to the IP 
	     * routing table, we should remove only those old paths that is 
	     * not valid for that LSA now. Otherwise, remove all old paths. 
	     * 
             */
            other_db = ospf_db_find_lsa(area, db->db_dest, db->db_mask, 
                                            source, 
					    ex_lsa->la_hdr.lsh_type);
            if (!other_db || !other_db->db_rt_table)
                delete_all = TRUE;
	}
	ospf_spf_acl_buginf("\nOSPF: Delete route for type %d db "
			    "%i with old mask %i", LSA_TYPE(db), db->db_dest, 
			    db->db_mask);
	ospf_event(OC_MASK_CHANGED, (ulong) LSA_ID(db), 
		   ((ulong) db | (LSA_TYPE(db) << 24))); 
	if (LSA_TYPE(db) != LSA_TYPE_STUB) {
	    /* 
	     * For stub network LSA, delete_route_old_mask() is called within
	     * spf_intra(). Sigh ! At that time, db_path_list is moved to 
	     * db_dpath_list already by spf_init(). For other types, we have 
	     * to do it ourselves here.
	     */
	    db->db_dpath_list = db->db_path_list;
	    db->db_path_list = NULL;
	    db->db_path_count = 0;
	}
	if (delete_all) {
	    for (path = db->db_dpath_list; path; path = path->pb_next) {
		ip_route_delete(pdb, db->db_dest, db->db_mask, 
				path->pb_gateway, source,
				path->pb_firsthop_idb,
				route_type);
	    }
	} else {
            for (path = db->db_dpath_list; path; path = path->pb_next) {
                for (other_path = other_db->db_path_list; other_path ; 
		     other_path = other_path->pb_next) {
                    if ((other_path->pb_gateway == path->pb_gateway) &&
                        (other_path->pb_firsthop_idb == path->pb_firsthop_idb))
                        break;
		}
                if (!other_path) {
		    ip_route_delete(pdb, db->db_dest, db->db_mask, 
				    path->pb_gateway, source,
				    path->pb_firsthop_idb,
				    route_type);
		}
	    }
	}
	/*
	 * Remove the db_dpath_list anyway as it is only valid for the LSA
	 * with the old mask, not for the current one. Also remove db from
	 * delete_list and clear db_rt_table.
	 */
        delete_route_list(route_list, db, 0, 0);
	if (pdb->ospf_area_brtr) {
	    if (route_type == RTYPE_INTRA) { 
		generate_sum(area, db, SUM_LS_INFINITY);
	    } else if (route_type == RTYPE_INTER) {
		build_inter_route_all(db, area);
	    }
	}
    }
    /*
     * For type 7 route, we should remove the corresponding type 5 LSA
     */
    if (LSA_TYPE(db) == LSA_TYPE_7_ASE) {
	ndb = ospf_lookup_net_route(area, db->db_dest, db->db_mask, 0, FALSE);
	ospf_translate_one_type7(pdb, ndb, db->db_dest, db->db_mask, 
				 &flood_list, TRUE);
	ex_flood_area(pdb, &flood_list, NULL, NULL);
    }
}

/*
 * db_spf_init_common 
 */
static inline void db_spf_init_common (lsdbtype *db, ulong dist)
{
    db->db_dist = dist;
    if (db->db_rt_table)
        db->db_dpath_list = db->db_path_list;
    else {
        db->db_dpath_list = NULL;
	free_plist(db);
    }
    db->db_path_list = NULL;
    db->db_path_count = 0;
}

/*
 *  spf_init
 * free parent list and reset 
 * vertex variables to run spf
 */

static void spf_init (areatype *area)
{
    lsdbtype	*db;
    uchar	type;
    pdbtype	*pdb;
    int		count = 0;

    pdb = area->ar_pdb_ptr;
    ospf_spf_intra_buginf("\nOSPF: Initializing to run spf");
    for (type = 0; type <=  LSA_TYPE_NET; type++) {
        for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	     db = db_get_next_db(db)) { 
 	    check_ospf_suspend_limit(area->ar_pdb_ptr, DBASE_ACTIVE, &count);
	    if (db->db_lsa_ptr == NULL)
		continue;
	    db->db_where = LSDB_UNINIT;
	    db->db_next = NULL;
	    db->db_prev = NULL;
	    db->db_direct = FALSE;
	    db->db_ar_range = FALSE;
	    db->db_bdr_rtr = FALSE;
	    db->db_rtr_reachable = FALSE;
	    db_spf_init_common(db, INTRA_PATH_COST_MAX);
	}
    }	

    area->ar_clist = NULL;
    area->ar_spf_ptr = NULL;
    area->ar_spf_instance++;
    area->ar_ospfSpfRuns++;
    area->ar_spf_changes &= ~(SPF_SCHD_NET | SPF_SCHD_RTR);

} /* end of spf_init */

/* 
 * rtr_backlink:
 * Check rtr LSA next_vertex to see if there is a back link to parent 
 *   in vertex.
 *
 * next_hop comes from lnk_data in link leading to next_vertex.
 * It is ifIndex if the link is unnumbered p2p.
 * For other link type, it is router's associated IP interface address.
 * 
 */

static boolean rtr_backlink (
    areatype	*area,
    lsdbtype	*next_vertex,
    lsdbtype	*vertex,
    uchar	type,
    ipaddrtype	next_hop,
    ipaddrtype	*if_ip_addr)
{
    int i;
    link_info *link;
    lsatype *lsa;
    rtr_lsatype *rtr_lsa;
    ushort	link_sz;
    ushort	link_count;
    idbtype	*idb;
    nbrtype	*nbr;
    ospf_idb_info *ospf_info;

    lsa = vertex->db_lsa_ptr;
    rtr_lsa = (rtr_lsatype *)(next_vertex->db_lsa_ptr);
    link_count = rtr_lsa->la_lnk_cnt;
    link = &(rtr_lsa->la_link);

    link_sz = 0;	
    if (link_count == 0) {
	*if_ip_addr = 0;
	return(FALSE);
    }
    for (i=0; i < link_count ;i++,
	            link=(link_info *)((uchar *)link + link_sz)) {
	link_sz = LINK_INFO_SIZE + link->lnk_tos_count * LINK_METRIC_SIZE;
	if ((link->lnk_id == lsa->lsh_id) && (link->lnk_con_type == type)) {
	    if (area && (vertex == area->ar_spf_ptr)) {
		if ((type == LINK_IF_RTR) && (!(VALID_IPADDRESS(next_hop)))) {
		    /*
		     * This is the case of unnumbered serial links which 
		     * contain interface index (ifIndex) in the link data 
		     * field (next_hop comes from link->lnk_data in 
		     * spf_intra()).
		     */
		    *if_ip_addr = next_hop;
		    return(TRUE);
		} else {
		    /*
		     * Get our idb that is on the same network as next
		     * hop.  Then check that the link->lnk_data (ip address
		     * of the interface on the next-hop router) is also
		     * on the same network.
		     * We are checking a point-to-point link here. Since
		     * only primary address appear in point-to-point link,
		     * there is no need to check against secondary addresses.
		     */
		    idb = ospf_ipaddr_to_idb(area, next_hop, FALSE);
		    if (!idb || ((next_hop & idb->ip_nets_mask) 
				 != (link->lnk_data & idb->ip_nets_mask)))
			continue;
		    ospf_info = idb->ospf_info;
		    if ((type == LINK_IF_RTR) &&
			(ospf_info->if_type == IF_POINT_TO_POINT)) {
			/*
			 * Take care of parallel point-to-point links which
			 * belong to the same IP subnet.
			 */
			nbr = ospf_info->oi_nbr;
			if (!nbr || (nbr->nbr_ipaddr != link->lnk_data) ||
			    (nbr->nbr_state != NBR_FULL)) {
			    continue;
			}
		    }
		}
	    }
	    *if_ip_addr = link->lnk_data;
	    return(TRUE);
	}
    }
    *if_ip_addr = 0;
    return(FALSE);
} /* end of rtr_backlink */

/*
 * ospf_lookup_transit_area
 *
 * Lookup the list of interfaces in the backbone area for a virtual
 * interface to next_hop_rtr.  Return pointer to the transit area, if found.
 */
static areatype * ospf_lookup_transit_area (areatype *area,
				     ipaddrtype next_hop_rtr)
{
    idbtype *idb;
    ospf_idb_info *ospf_info;

    if (area != area->ar_pdb_ptr->ospf_backbone) {
	errmsg(&msgsym(VIRTUAL_IN_NON_BACKBONE, OSPF), area->ar_id_str);
    }

    for (idb = area->ar_if_list; idb ; idb = ospf_info->next) {
	ospf_info = idb->ospf_info;
	if ((ospf_info->if_type == IF_VIRTUAL_LINK) &&
	    (ospf_info->vnbr_id == next_hop_rtr)) {
	    return(ospf_info->vlink_trans);
	}
    }

    return(NULL);
}
    
/* 
 * virtual_backlink
 * 
 * Check next_vertex for a virtual link back to vertex.
 *
 * If found
 *     If we are the root node of this SPF,
 *         look up for a route to the next router in *the transit area*
 *         to get the correct next hop returned in if_ip_addr.  
 *     else
 *         simply return TRUE.
 *
 * Note that if_ip_addr is returned as zero for the else case, which
 * is ok because the following two condition means add_path() will
 * simply inherit paths from parent: 1) parent vertex is not the root
 * and 2) parent and child vertices for virtual link are always router
 * LSA.
 */
static boolean virtual_backlink (
    areatype 	*area,
    lsdbtype 	*next_vertex,
    lsdbtype 	*vertex,
    uchar 	type,
    ipaddrtype 	next_hop,
    ipaddrtype 	*if_ip_addr)
{
    int 	i;
    link_info 	*link;
    lsatype 	*lsa;
    rtr_lsatype *next_rtr_lsa;
    ushort 	link_sz;
    ushort 	link_count;
    rtr_route 	*route;
    ipaddrtype 	next_hop_rtr;
    areatype 	*transit_area;
    ospf_rtr_path *path;

    *if_ip_addr = 0;
    lsa = vertex->db_lsa_ptr;

    /* Get to the data part of the next vertex
     */
    next_rtr_lsa = (rtr_lsatype *)(next_vertex->db_lsa_ptr);
    link_count = next_rtr_lsa->la_lnk_cnt;
    link = &(next_rtr_lsa->la_link);

    link_sz = 0;	
    if (link_count == 0) {
	return(FALSE);
    }
    for (i=0; i < link_count ;i++,
	            link=(link_info *)((uchar *)link + link_sz)) {
	link_sz = LINK_INFO_SIZE + link->lnk_tos_count * LINK_METRIC_SIZE;

	/*
	 * Look for a virtual link in next_vertex's router LSA (link)
	 * pointing back to vertex (lsa).
	 */
	if ((link->lnk_id == lsa->lsh_id) && (link->lnk_con_type == type)) {
	    /*
	     * Found a match!  Check if we are the SPF root, i.e. we are
	     * one of the end point of this virtual link.
	     */
	    if (area && (vertex == area->ar_spf_ptr)) {
		next_hop_rtr = next_rtr_lsa->la_hdr.lsh_id;
		transit_area = ospf_lookup_transit_area(area, next_hop_rtr);
		if (transit_area == NULL)
		    return(FALSE);
		if ((route = lookup_rtr_route(area->ar_pdb_ptr, transit_area, 
					      next_hop_rtr, NULL, DTYPE_ABR,
					      0, &path)) == NULL)
		    return(FALSE);
		*if_ip_addr = path->orp_forw_addr;
	    }
	    return(TRUE);
	}
    }
    return(FALSE);
} /* end of virtual_backlink */

/*
 * net_backlink: 
 * checks net vertex to see if there is 
 * a back link to parent
 */

boolean net_backlink (ulong rtr_id, lsdbtype *db)
{
    int		i;
    net_lsatype	*net_lsa;
    ulong	*attach_router;
    int		net_count;

    net_lsa = (net_lsatype *)(db->db_lsa_ptr);
    net_count = ((net_lsa->la_hdr.lsh_length) - NET_LSA_HDR_SIZE) / 4;
    attach_router = &net_lsa->la_nbr;
    for( i = 0 ; i < net_count ; i++, attach_router++) {
	if (*attach_router == rtr_id)
	    return(TRUE);
    }

    return(FALSE);
} /* end of net_backlink */


static inline void ospf_add_stub_routes_inline (areatype *area,
						lsdbtype *stub_list)
{
    lsdbtype *stub;

    if (stub_list)
	ospf_spf_intra_acl_pass = acl_ospf_spf_intra_debug(LSA_ID(stub_list));
    while (stub_list) {
	stub = stub_list;
	stub_list = stub_list->db_next;
	stub->db_next = NULL;
	addroute(area, stub);
    }
}

static inline void ospf_free_stub_routes_inline (lsdbtype *stub_list)
{
    lsdbtype *stub;

    while (stub_list) {
	stub = stub_list;
	stub_list = stub_list->db_next;
	stub->db_next = NULL;
	free_plist(stub);
    }
}

static void ospf_add_all_stub_routes (areatype *area)
{
    lsdbtype *stub, *stub_list;
    ulong stub_last_id;
    pdbtype *pdb;

    pdb = area->ar_pdb_ptr;
    ospf_spf_intra_buginf("\nOSPF: Adding Stub nets");
    stub_last_id = 0;
    stub_list = NULL;
    check_ospf_suspend(pdb, DBASE_ACTIVE);
    for (stub = db_get_first_db(area->ar_lsdbase[LSA_TYPE_STUB]); stub; 
	 stub = db_get_next_db(stub)) {
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if (stub->db_where != LSDB_STUB)
	    continue;
	if (stub_last_id != LSA_ID(stub)) {
	    /* 
	     * New stub destination found.
	     * Add the best path to last stub destination.
	     */
	    ospf_add_stub_routes_inline(area, stub_list);
	    /*
	     * Initialize for next round
	     */
	    stub_last_id = LSA_ID(stub);
	    stub_list = stub;
	    continue;
	}
	/*
	 * Look for the best cost entry
	 */
	if (stub->db_dist < stub_list->db_dist) {
	    /*
	     * Find the new best, clean up the old ones.
	     */
	    ospf_free_stub_routes_inline(stub_list);
	    stub_list = stub;
	} else if (stub->db_dist == stub_list->db_dist) {
	    stub->db_next = stub_list;
	    stub_list = stub;
	} else {
	    /*
	     * It is not the best entry, remove the path information.
	     */
	    ospf_free_stub_routes_inline(stub);
	}
    }
    /*
     * Handle the last entry
     */
    ospf_add_stub_routes_inline(area, stub_list);
    ospf_spf_intra_acl_pass = FALSE;
    check_ospf_suspend(pdb, DBASE_ACTIVE);
}

/*
 * spf_intra:
 * build intra area spf tree and
 * add intra area routes.
 */
static void spf_intra (areatype *area)
{
    lsdbtype 	*vertex;
    lsdbtype	*next_vertex;
    ulong	*attached_rtr;
    link_info	*link;
    pdbtype	*pdb;
    lsdbtype	*stub;
    ipaddrtype	if_ip_addr;
    uchar 	lookup;
    ushort	i;
    ulong	newdist;	
    rtr_lsatype *rtr_lsa;
    net_lsatype *s_net_lsa;
    net_lsatype *net_lsa;
    ushort	net_count;
    ushort	link_count;
    ushort	link_sz;
    ipaddrtype  next_hop;
    ulong	mask;
    list_queue	flood_list;
    addr_range	*range;
    lsdbtype	*db;
    rtr_route	*route;
    areatype	*ar;
    ospf_rtr_path *path;

    ospf_event(OC_GENERIC, (ulong) "spf_intra", area->ar_id);
    next_hop = 0;
    area->ar_bdr_count = 0;
    area->ar_asr_count = 0;
    pdb = area->ar_pdb_ptr;
    if (pdb->ospf_area_brtr) {
	for (range = area->ar_range_list; range ; range = range->net_next)
	    range->net_cost = SUM_LS_INFINITY;
    }
    /* find our own Link state Advertisement */
    vertex = db_find_lsa(area, 
			 area->ar_pdb_ptr->ospf_rtr_id,
			 area->ar_pdb_ptr->ospf_rtr_id,
			 LSA_TYPE_RTR);

    if (vertex == NULL) {
	/* Can not find our own LSA (the root node) */
	return;
    }
    /* start building the tree */
    vertex->db_dist = 0;
    area->ar_spf_ptr = vertex;
    for ( ; vertex ; vertex = getnext_clist(area)) {
        if (vertex->db_lsa_ptr == NULL)
           continue;
	ospf_spf_intra_acl_pass = acl_ospf_spf_intra_debug(LSA_ID(vertex));
	addroute(area, vertex);
 	check_ospf_suspend(pdb, DBASE_ACTIVE);
	/*
	 * check its age  and ignore
         * dbs with MAXAGE.
         */
	if (ADV_AGE(vertex) >= MAXAGE) {
	    ospf_spf_intra_acl_buginf("\nOSPF: Ignoring link state %i "
				      "type %d with MAXAGE", 
				      LSA_ID(vertex), LSA_TYPE(vertex));
	    continue;
	}
	if ( LSA_TYPE(vertex) == LSA_TYPE_RTR) {
	    /* check all links */
            rtr_lsa = (rtr_lsatype *)(vertex->db_lsa_ptr);
	    link_count = rtr_lsa->la_lnk_cnt;
            link = &(rtr_lsa->la_link);

	    ospf_spf_intra_acl_buginf("\n It is a router LSA %i. "
				      "Link Count %d",
				      rtr_lsa->la_hdr.lsh_id, link_count);
            link_sz = 0;	
	    for (i=0; i < link_count ; i++,
		               link=(link_info *)((uchar *)link + link_sz)) {
		link_sz = LINK_INFO_SIZE + 
		    link->lnk_tos_count * LINK_METRIC_SIZE;
		/* at present handle only links connected 
		 * to another router or links connected to
		 * transit network.
		 */
		ospf_spf_intra_acl_buginf("\n  Processing link %d, id %i, "
					  "link data %i, type %d", i, 
					  link->lnk_id, link->lnk_data, 
					  link->lnk_con_type);
		if ((link->lnk_con_type == LINK_IF_RTR)
 		    || (link->lnk_con_type == LINK_IF_TRANS_NET)
		    || (link->lnk_con_type == LINK_IF_VIRTUAL_LINK)) {
		    newdist = link->lnk_tos0_metric + vertex->db_dist;
		    next_hop = link->lnk_data;
 		    if ((link->lnk_con_type == LINK_IF_RTR) 
			|| (link->lnk_con_type == LINK_IF_VIRTUAL_LINK)){

			lookup = LSA_TYPE_RTR;
			/*
			 * handle virtual link for backbone
			 * here we need to lookup routing table
			 * to find nexthop to neighbor router on
			 * the virtual link.
			 */

		    } else
			lookup = LSA_TYPE_NET;

		    /* now lookup for advertisement */
		    if ((next_vertex = db_find_lsa( area,
						   link->lnk_id,
						   link->lnk_id,
						   lookup))
			== NULL) {
			ospf_spf_intra_acl_buginf("\n  Can not find next "
						  "node %i listed in LSA "
						  "LSA type %d, LS ID %i",
						  link->lnk_id, 
						  LSA_TYPE(vertex),
						  LSA_ID(vertex) );
			continue;
		    }
		    /* we found the next vertex */
		    /* if it is too old, next vertex */
	            if (ADV_AGE(next_vertex) >= MAXAGE) { 
			ospf_spf_intra_acl_buginf("\n  Ignoring Link state %i"
						  "type %d MAXAGE", 
						  LSA_ID(next_vertex),
						  LSA_TYPE(next_vertex));
			/* ignore and goto next link */
			continue;
		    }
		    /* check if there is a backlink from new vertex
		     * to previous vertex.
		     */
		    if (link->lnk_con_type == LINK_IF_RTR) {
			if (! rtr_backlink(area,
					   next_vertex,
					   vertex,
					   LINK_IF_RTR,
					   next_hop,
					   &if_ip_addr)) {
			    ospf_spf_intra_acl_buginf("\n  No RTR Back link");
			    continue;
			}
		    } else {
			if (link->lnk_con_type == LINK_IF_VIRTUAL_LINK) {
			    if (!virtual_backlink(area,
						  next_vertex,
						  vertex,
						  LINK_IF_VIRTUAL_LINK,
						  next_hop,
						  &if_ip_addr)) {
				ospf_spf_intra_acl_buginf("\n  No "
							  "VIRTUAL-LINK RTR");
				continue;
			    }
			} else {
			    if (link->lnk_con_type == LINK_IF_TRANS_NET)
				if (!net_backlink(vertex->db_lsa_ptr->lsh_id,
					      next_vertex)) {
				    ospf_spf_intra_acl_buginf("\n  No Net "
							      "back link");
				    continue;
				}
			    if_ip_addr = next_hop;
			}
		    }
		     /* if newdist is > than current, next vertex */
		    if (newdist > next_vertex->db_dist) {
			ospf_spf_intra_acl_buginf("\n  Ignore newdist %d "
						  "olddist %d",
						  newdist, 
						  next_vertex->db_dist);
			continue;
		    }
		    /* add equal cost route */
		    if (newdist == next_vertex->db_dist) {
			ospf_spf_intra_acl_buginf("\n  Add equal-length path "
						  "to %i, dist %d",
						  next_vertex->db_lsa_ptr->
						  lsh_id,
						  newdist);
			
			add_path(next_vertex, vertex, newdist,
				 area, next_hop, if_ip_addr);
		    }
		    if (newdist < next_vertex->db_dist)
			add_better_path(next_vertex, vertex, newdist, area,
				        next_hop, if_ip_addr, (pdbtype *)NULL);
		} else { /* stub link */
		    /* 
		     * stub link. process this link at this point and 
		     * and add paths to it if one availabe, but adding these
		     * nodes to spf will be done after spf algoritherm
		     * Note that we set the host bit for the LSID,
		     * it allows us to group and order all stub routes to the
		     * same destination together correctly.
		     * Another advantage using host bit set LSID is that
		     * we do not need to worry about mask changes for stub.
		     * The mask changes detection code is left intact below
		     * but should be removed later when we do OSPF rewrite.
		     */
		    newdist = link->lnk_tos0_metric + vertex->db_dist;
		    db_add_lsa(&stub, area, (link->lnk_id | ~link->lnk_data),
			       ADV_RTR(vertex), NULL, LSA_TYPE_STUB);
		    if (!stub) {
 			errmsg(&msgsym(NOMEMORY, OSPF), "stub LSA");
			return;
		    }
		    if (stub && !(stub->db_lsa_ptr)) { 
			stub->db_lsa_ptr = malloc(sizeof(net_lsatype));
			if (stub->db_lsa_ptr == NULL) {
			    continue;
			}
                        s_net_lsa = (net_lsatype *)stub->db_lsa_ptr;
                        s_net_lsa->la_hdr.lsh_id = (link->lnk_id |
						    ~link->lnk_data);
                        s_net_lsa->la_hdr.lsh_adv_rtr = ADV_RTR(vertex);
			s_net_lsa->la_mask = link->lnk_data;
			s_net_lsa->la_hdr.lsh_length = NET_LSA_HDR_SIZE; 
			COPY_TIMESTAMP(vertex->db_time, stub->db_time);
			/* add ls checksum */
			compute_iso_checksum((uchar *)s_net_lsa,
					     s_net_lsa->la_hdr.lsh_length,
				     (uchar *)&(s_net_lsa->la_hdr.lsh_chksum));
			add_better_path(stub, vertex, newdist, area,
					link->lnk_id, link->lnk_id,
					(pdbtype *)NULL);
		    } else { /* if there is stub link already */
			COPY_TIMESTAMP(vertex->db_time, stub->db_time);
                        s_net_lsa = (net_lsatype *)stub->db_lsa_ptr;
			if (s_net_lsa->la_mask == link->lnk_data) {
			    if (newdist == stub->db_dist) 
				add_path(stub, vertex, newdist, area,
					 link->lnk_id, link->lnk_id);
			    if (newdist < stub->db_dist)
				add_better_path(stub, vertex, newdist, area,
						link->lnk_id, link->lnk_id,
						(pdbtype *)NULL);
			    else
				ospf_spf_intra_acl_buginf("\n  Ignore "
							  "newdist %d "
							  "olddist %d",
							  newdist, 
							  stub->db_dist);
			    
			} else {  
			    /* 
			     * mask has changed.  
			     * Set db_mask_change so we will delete 
			     * routes with the old mask.
			     */
			    stub->db_mask_change = TRUE;
			    s_net_lsa->la_mask = link->lnk_data;
			    ospf_spf_inter_acl_pass = 
				acl_ospf_spf_inter_debug(LSA_ID(stub));
			    delete_route_old_mask(pdb, area, stub);
			    add_better_path(stub, vertex, newdist, area,
					    link->lnk_id, link->lnk_id,
					    (pdbtype *)NULL);
			    /* recalculate ls checksum */
			    compute_iso_checksum((uchar *)s_net_lsa,
				     s_net_lsa->la_hdr.lsh_length,
				     (uchar *)&(s_net_lsa->la_hdr.lsh_chksum));
			}
		    } /* end of if there is stub link already */
		    stub->db_where = LSDB_STUB;
		    continue;
		} /* end of stub link */  
	    } /* end of router links loops */
	} else /* it must be NET advertisement */
	    if ( LSA_TYPE(vertex) == LSA_TYPE_NET) {
		/* now zip through the net links */
		net_lsa = (net_lsatype *)vertex->db_lsa_ptr;
		net_count = ((net_lsa->la_hdr.lsh_length) - NET_LSA_HDR_SIZE);
		ospf_spf_intra_acl_pass = 
		    acl_ospf_spf_intra_debug(net_lsa->la_hdr.lsh_id);
		ospf_spf_intra_acl_buginf("\n It is a network LSA %i. "
					  "Router Count %d",
					  net_lsa->la_hdr.lsh_id, 
					  net_count >> 2);
		for (attached_rtr = &net_lsa->la_nbr , i = 0; i < net_count;
		                         attached_rtr++, i += 4) {
		    /* check other side */
		    if ((next_vertex = db_find_lsa(area,
						   *attached_rtr,
						   *attached_rtr,
						   LSA_TYPE_RTR))
			== NULL) {
			ospf_spf_intra_buginf("\n  No Link State to "
					      "router %i",
					      *attached_rtr);
			continue;
		    }
		    ospf_spf_intra_acl_buginf("\n  Processing router id %i",
					  *attached_rtr);

		    /*
		     * if it is on spftree, too old or no back link,
		     * go to next vertex
		     */
		    if (ADV_AGE(next_vertex) >= MAXAGE || 
			(!rtr_backlink(area, next_vertex, vertex,
				       LINK_IF_TRANS_NET, 0, &if_ip_addr))) {
			ospf_spf_intra_acl_buginf("\n No rtr back link");
			continue;
		    }
			/* 0 cost on same net */
		    if (vertex->db_dist > next_vertex->db_dist) {
			ospf_spf_intra_acl_buginf("\n  New newdist %d "
						  "olddist %d",
						  vertex->db_dist, 
						  next_vertex->db_dist);
			    continue;
		    }
		    if (vertex->db_dist == next_vertex->db_dist)
			add_path(next_vertex, vertex, vertex->db_dist,
				 area, 0, if_ip_addr);
		    if (vertex->db_dist < next_vertex->db_dist)
			add_better_path(next_vertex, vertex, vertex->db_dist,
					area, 0, if_ip_addr, (pdbtype *)NULL);
		}
	    } 
    }
    /* 
     * now do stub nets; at 
     * this point shortest intra
     * routes are known
     */
    ospf_add_all_stub_routes(area);
    /*
     * Now completed intra routes
     * Now check if we need to build summary
     * Link states and if there is a change 
     * flood them.
     */
    if (pdb->ospf_area_brtr) {
	flood_list.q_first = NULL;
	flood_list.q_last  = NULL;
	for (ar = pdb->ospf_area_list; ar ; ar = ar->ar_next) {
	    if (area == ar || ar->ar_stub_nosum)
		continue;
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	    for (range = area->ar_range_list;range ; range = range->net_next) {
		ospf_spf_inter_acl_pass = 
		    acl_ospf_spf_inter_debug(range->net_address);
		/* No db is preset create one */
		ospf_send_summary_lsa(&db,
				      ar,
				      range->net_address,
				      range->net_mask,
				      range->net_cost,
				      LSA_TYPE_SUM_NET,
				      RTYPE_INTRA,
				      &flood_list);
	    }

	    /* 
	     * Now time for AS Boundary router
	     * Do not generate ASBR summary into
	     * stub/NSSA area or from NSSA area.
	     */
	    if (!ar->ar_stub && !ar->ar_nssa && !area->ar_nssa) {
		mask = 0;
		for (i = 0; i < HTBLSIZE; i++) {
		    route = (area->ar_pdb_ptr->ospf_routes)[i];
		    for ( ; route ; route = route->rt_next) {
			for (path = route->rt_path_list; path; 
			     path = path->orp_next) {
			    if ((path->orp_dest_type & DTYPE_ASBR) &&
				(path->orp_spf_inst ==
				 area->ar_spf_instance) &&
				(path->orp_area == area)) {
				db = db_find_lsa(ar, route->rt_dest,
						 ROUTER_ID(area),
						 LSA_TYPE_SUM_ASB);
				ospf_spf_inter_acl_pass = 
				    acl_ospf_spf_inter_debug(route->rt_dest);
				build_sum_lsa(&db, ar, route->rt_dest,
					      mask, path->orp_cost,
					      LSA_TYPE_SUM_ASB, RTYPE_INTRA,
					      &flood_list);
			    }
			}
		    }
		}
	    }
	    /* Now flood them */
	    flood_area(ar, &flood_list, NULL, NULL);
	}
    }
    ospf_spf_inter_acl_pass = FALSE;
} /* end of spf_intra */

/*
 * summary_path_delete
 * Take a db and delete paths from dpath_list and from the routing table.
 */
static void summary_path_delete (pdbtype *pdb, areatype *area, lsdbtype *db)
{
    pathtype 	*path;
    sum_lsatype	*sum_lsa;
    ipaddrtype 	source;

    sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
    source = db->db_lsa_ptr->lsh_adv_rtr;
    for (path = db->db_dpath_list; path; path = path->pb_next) {
	if (db->db_lsa_ptr->lsh_type == LSA_TYPE_SUM_ASB)
	    delete_rtr_route(area, sum_lsa->la_hdr.lsh_id,
			     DTYPE_ASBR, path->pb_gateway, 
			     path->pb_firsthop_idb, RTYPE_INTER,
			     source);
	else
	    ip_route_delete(pdb, 
			    (sum_lsa->la_hdr.lsh_id & sum_lsa->la_mask),
			    sum_lsa->la_mask, path->pb_gateway,
			    source, path->pb_firsthop_idb, 
			    RTYPE_INTER);
    }
    delete_route_list(&(area->ar_sum_delete_list), db, 0, 0);
    if (db->db_path_list)
	insert_route_list(&(area->ar_sum_delete_list), db);
}


/*
 * process_sum_sub
 * process a single summary db
 * and adds its route if necessary
 * It will process all summary LSAs with the same LSID within one area
 */
static void process_sum_sub (
    lsdbtype *db,
    areatype *area,
    boolean partial)
{
    rtr_route	*route;
    sum_lsatype	*sum_lsa;
    ulong	dist;
    ndbtype	*ndb;
    rtr_route	*ndbroute;
    pdbtype	*pdb;
    lsdbtype *temp_db;
    int 	sum_index;
    ulong rtr_id[10];
    ulong currenttime, lasttime, time[4];
    lsdbtype *inter_db;
    ipaddrtype source;
    areatype	*ar;
    addr_range	*range;
    boolean 	looked_for_another_lsid;
    ulong	another_lsid;
    ospf_rtr_path *abr_path, *asbr_path;

    sum_index = 0;
    looked_for_another_lsid = FALSE;
    inter_db = NULL;
    pdb = area->ar_pdb_ptr;
    /*
     * init the array that keeps track
     * of link states we processed.
     */
    if (partial) {
	/* if this area is stub summary, run SPF only for default 
	 * summary LSA.   Do it here only for partial SPF because
	 * spf_sum() has already checked this when we do full SPF
	 */
	if (area->ar_stub_nosum 
	    && ((sum_lsatype *)db->db_lsa_ptr)->la_hdr.lsh_id != 0)
	    return;
	for (sum_index = 0; sum_index < 10; sum_index++)
	    rtr_id[sum_index] = 0;
	rtr_id[0] = ROUTER_ID(area);
	sum_index = 1;
    }
  back:
    ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(LSA_ID(db)); 
    sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
    source = db->db_lsa_ptr->lsh_adv_rtr;

    db->db_abr_flag = TRUE;
    inter_db = db;

    if (db->db_partial) {
	db->db_partial = FALSE;
	db->db_work->ow_db = NULL;
	db->db_work = NULL;
    }
    ospf_spf_inter_acl_buginf("\nOSPF: Start %sprocessing Summary LSA %i, "
			      "mask %i, adv %i, age %d, seq %#x (Area %s)",
			      partial ? "partial " : "", 
			      sum_lsa->la_hdr.lsh_id, sum_lsa->la_mask, 
			      sum_lsa->la_hdr.lsh_adv_rtr, REAL_AGE(db),
			      sum_lsa->la_hdr.lsh_seq, area->ar_id_str);


    if (partial)
        db_spf_init_common(db, SUM_LS_INFINITY);

    /* check if the LSA is originated by this router itself.
     * or received from a non backbone area when it is
     * a area border router.
     */
    if ((pdb->ospf_area_brtr && 
	 ospf_backbone_active(pdb->ospf_backbone, TRUE) && 
	 (area->ar_id != BACKBONE_AREA_ID)) || 
	(sum_lsa->la_hdr.lsh_adv_rtr == ROUTER_ID(area))) {
	ospf_spf_inter_acl_buginf("\nOSPF: Non-backbone/self-originated LSA");
        if (partial)
	    summary_path_delete(pdb, area, db);
	goto end_process;
    }

    if ((SUM_METRIC(sum_lsa->la_tos_metric) == SUM_LS_INFINITY)
	|| (ADV_AGE(db) >= MAXAGE)) {
        if (partial)
	    summary_path_delete(pdb, area, db);
	goto end_process;
    }

    /*
     * Check if this destination exactly matches any attached
     * area ranges.
     */
    if (sum_lsa->la_hdr.lsh_type == LSA_TYPE_SUM_NET) {
	for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	    for (range = ar->ar_range_list; 
		 range; range = range->net_next) {
		if ((range->net_cost != SUM_LS_INFINITY)
		    && (range->net_address == sum_lsa->la_hdr.lsh_id)
		    && (range->net_mask == sum_lsa->la_mask)) {
		    if (partial)
		        summary_path_delete(pdb, area, db);
		    goto end_process;
		}
	    }
	}
    }

/* check point 1 */
    lasttime = runtime();

    /* Now look up for originating Area Border Router */
    if ((route = lookup_rtr_route(pdb, area, sum_lsa->la_hdr.lsh_adv_rtr,
				  NULL, DTYPE_ABR, RTYPE_INTRA, 
				  &abr_path)) == NULL) {
	/* can not reach the area border router */
        ospf_spf_inter_buginf("\nOSPF: ABR not reachable %i", 
			      sum_lsa->la_hdr.lsh_adv_rtr);
  	if (partial)
	    summary_path_delete(pdb, area, db);
	goto end_process;
    }
    dist = (abr_path->orp_cost + SUM_METRIC(sum_lsa->la_tos_metric));

/* check point 2 */
    currenttime = runtime();
    time[0] = currenttime - lasttime;
    lasttime = currenttime;	    

    /* Now lookup destination */
    if (sum_lsa->la_hdr.lsh_type == LSA_TYPE_SUM_NET) {
	if ((ndb  = ospf_lookup_net_route(area, 
					  sum_lsa->la_hdr.lsh_id & 
					  sum_lsa->la_mask,
					  sum_lsa->la_mask, 0, 
					  FALSE)) != NULL) {
 	    if ((ndb->route_type & RTYPE_INTRA) ||
		(NDB2PROCTYPE(ndb) == PROC_CONNECTED)) {
		ospf_spf_inter_buginf("\nOSPF: Intra area/Connected route exists");
 		if (partial)
		    summary_path_delete(pdb, area, db);
  		goto end_process;
 	    }
	    if (ndb->spf_instance != pdb->local_ospf_spf_instance)
		ndb = NULL;
  	    if ((ndb && (ndb->route_type & RTYPE_INTER))
  		&& (ndb->metric < dist)) {
		if (partial)
		    summary_path_delete(pdb, area, db);
  		goto end_process;
  	    }
	}
	/*
	 * if there is a route type EXTERN,
	 * delete it.
	 */
	if (ndb 
	    && (ndb->route_type & (RTYPE_EXTERN1|RTYPE_EXTERN2))) {
	    ospf_spf_inter_buginf(" Deleting external to make it inter %i\n", 
		   ndb->netnumber);
	    ip_route_delete(pdb, ndb->netnumber, ndb->netsmask,
			    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
	    ndb = NULL;
	}
	inter_db = db;
	add_better_path(db, abr_path->orp_db, dist, area, 0, 0, 
			(pdbtype *)NULL);
    } else {
	/*
	 * Look for route to this ASBR in *any* area.
	 */
	if ((ndbroute  = lookup_rtr_route(pdb, NULL, sum_lsa->la_hdr.lsh_id,
					  NULL, 0, 0, &asbr_path)) != NULL) {
 	    if (asbr_path->orp_route_type & RTYPE_INTRA) {
 		if (partial)
		    summary_path_delete(pdb, area, db);
  		goto end_process;
 	    }
  	    if (asbr_path->orp_pdb_spf_inst != pdb->local_ospf_spf_instance) {
 		ndbroute = NULL;
		asbr_path = NULL;
	    }
  	    if (asbr_path && (asbr_path->orp_route_type & RTYPE_INTER)
 		&& (asbr_path->orp_cost < dist)) {
 		if (partial)
		    summary_path_delete(pdb, area, db);
  		goto end_process;
 	    }
	}

	inter_db = db;
	add_better_path(db, abr_path->orp_db, dist, area, 0, 0, 
			(pdbtype *)NULL);
    }

/* check point 3 */
    currenttime = runtime();
    time[1] = currenttime - lasttime;
    lasttime = currenttime;	    

    if (db->db_path_list == NULL) {
	ospf_spf_inter_buginf(" NO routes to parent abr %i , spf %d area spf %d\n",
	       route->rt_dest,
	       abr_path->orp_spf_inst,
	       area->ar_spf_instance);
    }
    add_sum_route(area, db, partial);

/* check point 4 */
    currenttime = runtime();
    time[2] = currenttime - lasttime;
    lasttime = currenttime;	    

    if (partial) {
  	summary_path_delete(pdb, area, db);
    }

/* check point 5 */
    currenttime = runtime();
    time[3] = currenttime - lasttime;
    lasttime = currenttime;	    

    write_circular(PROCESS_SUM_TIME, time, 4 /* there are fours steps*/);

  end_process:
    if ((partial) && sum_index < 10) {
	rtr_id[sum_index] = sum_lsa->la_hdr.lsh_adv_rtr;
	sum_index++;
	temp_db = find_other_sum_db(area, sum_lsa->la_hdr.lsh_id, 
				    sum_lsa->la_mask, &rtr_id[0],
				    sum_index, sum_lsa->la_hdr.lsh_type);
	/*
	 * Because LSA can have host bit set in the link state ID, we
	 * have to check for it, as well as normal LSA ID, no matter
	 * which ID we start from.  Note that this should be done for
	 * type 3 LSA (summary network LSA) only.
	 */
	if (!temp_db && !looked_for_another_lsid
	    && (sum_lsa->la_hdr.lsh_type == LSA_TYPE_SUM_NET)) {
	    if (sum_lsa->la_hdr.lsh_id == 
		(sum_lsa->la_hdr.lsh_id & sum_lsa->la_mask)) {
		another_lsid = (sum_lsa->la_hdr.lsh_id |
				~sum_lsa->la_mask);
	    } else {
		another_lsid = (sum_lsa->la_hdr.lsh_id &
				sum_lsa->la_mask);
	    }
	    temp_db = find_other_sum_db(area, another_lsid, 
					sum_lsa->la_mask, &rtr_id[0], 
					sum_index, sum_lsa->la_hdr.lsh_type);
	    looked_for_another_lsid = TRUE;
	}
	if ((temp_db) && (temp_db != db)) {
	    db = temp_db;
	    goto back;
	}
    }
    if (partial) {
	if ((pdb->ospf_area_brtr) && 
	    (area->ar_id == BACKBONE_AREA_ID)&&
	    (sum_lsa->la_hdr.lsh_adv_rtr != ROUTER_ID(area)))
	    build_inter_route_all(inter_db, area);
    }
    ospf_spf_inter_acl_pass = FALSE;
}


/*
 * process_sum
 * A wrapper for process_sum_sub().
 * Call process_sum_sub() for the input area.
 * If we are doing partial processing and we are not an ABR, call 
 * process_sum_sub() for all other attached areas to figure out 
 * the best route.
 */
void process_sum (
    lsdbtype *db,
    areatype *area,
    boolean partial)
{
   sum_lsatype *sum_lsa = (sum_lsatype *) db->db_lsa_ptr;
   ulong lsid = sum_lsa->la_hdr.lsh_id;
   ulong mask = sum_lsa->la_mask;
   ulong another_lsid = (lsid == (lsid & mask)) ? (lsid | ~mask) : (lsid & mask);
   uchar lsa_type = sum_lsa->la_hdr.lsh_type;
   pdbtype *pdb = area->ar_pdb_ptr;
   areatype *ar;

   if (partial)
       pdb->local_ospf_spf_instance++;

   process_sum_sub(db, area, partial);

   if (partial && 
       (!pdb->ospf_area_brtr || 
	!ospf_backbone_active(pdb->ospf_backbone, TRUE))) {
       /*
	* If doing partial and we are not connected to the backbone,
	* do partial for all other areas.
	*/
       for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	   if (ar == area)
	       continue;
	   db = find_other_sum_db(ar, lsid, mask, NULL, 0, lsa_type);
	   if (!db && (lsa_type == LSA_TYPE_SUM_NET))
	       db = find_other_sum_db(ar, another_lsid, mask, NULL, 0, 
				      lsa_type);
	   if (db) 
	       process_sum_sub(db, ar, partial);
       }
   }
}

/*
 * take a db and delete its paths from
 * its dpath and from the routing table.
 *
 */

static void external_path_delete (
    pdbtype *pdb,
    areatype *area,
    lsdbtype *db,
    ushort route_type)

{
    pathtype *path;
    ex_lsatype *ex_lsa;
    ipaddrtype source;
    lsdbtype **delete_list;
    
    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
    if (LSA_TYPE(db) == LSA_TYPE_ASE)
	delete_list = &pdb->ospf_ex_delete;
    else
	delete_list = &area->ar_ex_delete_list;

    if (db->db_dpath_list) {
	source = db->db_lsa_ptr->lsh_adv_rtr;
	for (path = db->db_dpath_list; path; path = path->pb_next) {
	    ip_route_delete(pdb, ex_lsa->la_hdr.lsh_id & ex_lsa->la_mask,
			    ex_lsa->la_mask, path->pb_gateway, source, 
			    path->pb_firsthop_idb, route_type);
	}
	delete_route_list(delete_list, db, 0, 0);
	if (db->db_path_list)
	    insert_route_list(delete_list, db);
    }
}

/*
 * add_better_ex_path_fwd
 * Add path to external route towards forwarding address.
 * f_ndb is the ndb for the forwarding address network.
 * If forwarding address network is attached, f_db point to
 * the correct LSA entry directly.
 */
static void add_better_ex_path_fwd (lsdbtype *db, ndbtype *f_ndb, 
				    lsdbtype *f_db, ulong metric,
				    areatype *area, ulong gateway, 
				    pdbtype *pdb)
{
    int i;
    rdbtype *rdb;

    if (f_ndb->attrflags & NET_ATTACHED) {
	f_db->db_ase_use = TRUE;
	f_db->db_ase_count = f_db->db_path_count;
	add_better_path(db, f_db, metric, area, 0, gateway, pdb);
    } else {
	/*
	 * Go through all paths
	 */
	for (i = 0; i < f_ndb->routecount; i++) {
	    rdb = f_ndb->rdb[i];
	    f_db = rdb->ospf_rdb_lsdb;
	    if (!f_db) {
		errmsg(&msgsym(RDB_NO_LSA, OSPF), LSA_ID(db), ADV_RTR(db), 
		       LSA_TYPE(db), gateway, metric, f_ndb->netnumber, 
		       f_ndb->netsmask, NDB2PROCTYPE(f_ndb), 
		       f_ndb->attrflags, f_ndb->metric);
		continue;
	    }
	    f_db->db_ase_use = TRUE;
	    f_db->db_ase_count = f_db->db_path_count;
	    if (i) 
		add_path(db, f_db, metric, area, 0, gateway);
	    else
		add_better_path(db, f_db, metric, area, 0, gateway, pdb);
	}
    }
}

/*
 * add_better_ex_path
 * Add path to external route with no forwarding address.
 */
static void add_better_ex_path (lsdbtype *db, rtr_route *route, 
				ulong metric, areatype *area, pdbtype *pdb) {
    ospf_rtr_path *path;
    boolean first_path = TRUE;

    for (path = route->rt_path_list; path; path = path->orp_next) {
	if (path->orp_cost <= route->rt_min_cost) {
	    path->orp_db->db_ase_use = TRUE;
	    path->orp_db->db_ase_count = path->orp_db->db_path_count;
	    if (first_path) {
		add_better_path(db, path->orp_db, metric, area, 0,
				path->orp_forw_addr, pdb);
		first_path = FALSE;
	    } else {
		add_path(db, path->orp_db, metric, area, 0, 
			 path->orp_forw_addr);
	    }
	}
    }
}

/*
 * process_ase_sub
 * Handle single external route
 *
 */
static void process_ase_sub (
    lsdbtype *db,
    pdbtype *pdb,
    areatype *area,
    boolean partial)		/* partial SPF flag */
{
    rtr_route	*route;
    ex_lsatype	*ex_lsa;
    boolean	type2, ndb_type2, external, ndb_external, p_bit, ndb_p_bit;
    ipaddrtype	forw_addr, ndb_forw_addr;
    ipaddrtype  gateway;
    ulong	routing_dist, y, metric, old_metric;
    ndbtype	*f_ndb, *ndb;
    lsdbtype	*f_db, *temp_db;
    areatype	*temp_area;
    int 	ex_index;
    ulong 	rtr_id[10], currenttime, lasttime, time[5], another_lsid;
    boolean 	looked_for_another_lsid, db_lost;
    ushort	route_type_allowed, route_type;
    uchar	type;
    ospf_pdb_info *ospf_pdb;
    ospf_rtr_path *asbr_path;

    ospf_pdb = pdb->ospf_pdb;
    ex_index = 0;
    looked_for_another_lsid = FALSE;

    /*
     * init the array that keeps track
     * of link states we processed.
     */
    if (partial) {
	for (ex_index = 0; ex_index < 10; ex_index++)
	    rtr_id[ex_index] = 0;
	rtr_id[0] = pdb->ospf_rtr_id;
	ex_index = 1;
    }
    type = LSA_TYPE(db);
    if (type == LSA_TYPE_ASE)
	route_type_allowed = RTYPE_INTRA|RTYPE_INTER;
    else
	route_type_allowed = RTYPE_INTRA;
  back:
    ospf_spf_ext_acl_pass = acl_ospf_spf_ext_debug(LSA_ID(db));
    type2 = FALSE;
    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
    route_type = EXLSA_TO_ROUTE_TYPE(ex_lsa);
    temp_area = NULL;
    f_ndb = NULL;
    f_db = NULL;
    gateway = 0;
    /*
     * check if the cost specified is LSINFINITY
     * or if the LSA is originated by the router itself.
     */
    ospf_spf_ext_acl_buginf("\nOSPF: Start %sprocessing Type %d External "
			    "LSA %i, mask %i, adv %i, age %d, seq %#x, "
			    "metric %d, metric-type %d", 
			    partial ? "partial " : "", type,
			    ex_lsa->la_hdr.lsh_id, 
			    ex_lsa->la_mask, ex_lsa->la_hdr.lsh_adv_rtr, 
			    REAL_AGE(db), ex_lsa->la_hdr.lsh_seq,
			    ASE_METRIC(ex_lsa->la_metric.as_e_tos_metric),
			    (ASE_METRIC_TYPE(ex_lsa->la_metric.
					     as_e_tos_metric) ? 2 : 1));

    if (db->db_partial) {
	db->db_partial = FALSE;
	db->db_work->ow_db = NULL;
	db->db_work = NULL;
    }
    if (partial)
	db_spf_init_common(db, SUM_LS_INFINITY);

    if (ex_lsa->la_hdr.lsh_adv_rtr == pdb->ospf_rtr_id)
	goto end_process;

    if (pdb->ospf_as_brtr) {
	ospf_spf_check_ex_origin(area, db);
	if (!ospf_spf_check_default(db, area)) {
	    ospf_spf_ext_acl_buginf("\nOSPF: We originate default. "
				    "Don't install default from others");
	    if (partial)
		external_path_delete(pdb, area, db, route_type); 
	    goto end_process;
	}
    }
    
    if (!ospf_pdb->ospf_normal_area_count && (type == LSA_TYPE_ASE)) {
	ospf_spf_ext_acl_buginf("\nOSPF: No normal area");
	if (partial)
	    external_path_delete(pdb, area, db, route_type); 
	goto end_process;
    }

    if ((ASE_METRIC(ex_lsa->la_metric.as_e_tos_metric) == SUM_LS_INFINITY)
	|| (ADV_AGE(db) >= MAXAGE)){
	if (partial)
	    external_path_delete(pdb, area, db, route_type); 
	goto end_process;
    }
    if ((ndb = ospf_lookup_net_route(area, 
				     ex_lsa->la_hdr.lsh_id & ex_lsa->la_mask, 
				     ex_lsa->la_mask, 0, FALSE)) != NULL) {
	if (ndb->route_type & (RTYPE_INTRA|RTYPE_INTER)) {
	    ospf_spf_ext_acl_buginf("\nOSPF: we have intra or inter to %i",
				    ex_lsa->la_hdr.lsh_id);
	    if (db->db_rt_table) {
		if (type == LSA_TYPE_ASE)
		    delete_route_list(&pdb->ospf_ex_delete, db, 0, 0);
		else
		    delete_route_list(&area->ar_ex_delete_list, db, 0, 0);
	    }
	    goto end_process;
	}
    }
    old_metric = 0;
    if (ndb && (ndb->attrflags & NET_ATTACHED))
	old_metric = ndb->lastinterface->ospf_info->if_cost[0];
    else if (ndb) {
	if (ndb->spf_instance != pdb->local_ospf_spf_instance)
	    ndb = NULL;
	else
	    old_metric = ndb->metric;
    }


/* check point 1 */
    lasttime = runtime();
    /* Now look up for originating AS Border Router */
    if ((route = lookup_rtr_route(pdb, NULL, ex_lsa->la_hdr.lsh_adv_rtr,
				  NULL, DTYPE_ASBR, 0, &asbr_path)) == NULL) { 
	/* can not reach area border router */
	ospf_spf_ext_acl_buginf("\nOSPF: Did not find route to ASBR %i",
				ex_lsa->la_hdr.lsh_adv_rtr);
	if (partial)
	    external_path_delete(pdb, area, db, route_type); 
	goto end_process;
    }
    if (type == LSA_TYPE_7_ASE) {
	if (!ex_lsa->la_hdr.lsh_id && pdb->ospf_area_brtr &&
	    (asbr_path->orp_dest_type & DTYPE_ABR)) {
	    ospf_spf_ext_acl_buginf("\nOSPF: Don't install default from "
				    "other NSSA ABR");
	    if (partial)
		external_path_delete(pdb, area, db, route_type); 
	    goto end_process;
	}
    }
/* check point 2 */
    currenttime = runtime();
    time[0] = currenttime - lasttime;
    lasttime = currenttime;	    
    /* get forward address */
    forw_addr = ex_lsa->la_metric.as_forw_addr;
    if (forw_addr == 0) {
	/* forward it to originating router */
	routing_dist = route->rt_min_cost;
    } else {
	if ((f_ndb  = ospf_lookup_net_route(area, forw_addr, 
					    0, 0, FALSE)) == NULL) {
	    ospf_spf_ext_acl_buginf("\nOSPF: Fail to find route to "
				    "forwarding addr %i", forw_addr);
	    ospf_insert_faddr(pdb, forw_addr);
	    if (partial)
		external_path_delete(pdb, area, db, route_type); 
	    goto end_process;
	}
	if (!(f_ndb->route_type & route_type_allowed)
	    && ((f_ndb->attrflags & NET_ATTACHED) == 0)) {
	    ospf_spf_ext_acl_buginf("\nOSPF: Fail to find route to "
				    "forwarding addr %i", forw_addr);
	    ospf_insert_faddr(pdb, forw_addr);
	    if (partial)
		external_path_delete(pdb, area, db, route_type); 
	    goto end_process;
	}
	routing_dist = f_ndb->metric;
	if (f_ndb->attrflags & NET_ATTACHED) {
	    temp_area = f_ndb->lastinterface->ospf_info->area_ptr;
	    routing_dist = f_ndb->lastinterface->ospf_info->if_cost[0];
	    gateway = forw_addr;
	} else {
	    if ((temp_area = get_area_ptr(pdb, f_ndb->ndb_area_id)) == NULL) {
 		ospf_spf_ext_acl_buginf("\nOSPF: Fail to find area %i from "
					"network %i", f_ndb->ndb_area_id, 
					f_ndb->netnumber);
 		ospf_insert_faddr(pdb, forw_addr);
		if (partial)
		    external_path_delete(pdb, area, db, route_type); 
		goto end_process;
	    }
	    gateway = f_ndb->lastgateway;
	}
	if (f_ndb->attrflags & NET_ATTACHED) {
	    if ((f_db = find_db(temp_area, (forw_addr & f_ndb->netsmask),
			    RTYPE_INTRA)) == NULL) {
 		ospf_spf_ext_acl_buginf("\nOSPF: Fail to find connected "
					"db for forwarding address %i \n",
					forw_addr);
 		ospf_insert_faddr(pdb, forw_addr);
		if (partial)
		    external_path_delete(pdb, area, db, route_type); 
		goto end_process;
	    }
	} else {
	    if ((f_db = find_db(temp_area, (f_ndb->netnumber),
				f_ndb->route_type)) == NULL) {
		ospf_spf_ext_acl_buginf("\nOSPF: Fail to find db for "
					"forwarding addr %i", forw_addr);
 		ospf_insert_faddr(pdb, forw_addr);
		if (partial)
		    external_path_delete(pdb, area, db, route_type); 
		goto end_process;
	    }
	}
    }

/* check point 3 */
    currenttime = runtime();
    time[1] = currenttime - lasttime;
    lasttime = currenttime;	    

    y = ASE_METRIC(ex_lsa->la_metric.as_e_tos_metric);
    type2 = (ASE_METRIC_TYPE(ex_lsa->la_metric.as_e_tos_metric));

    if (type2)
	metric = y;
    else
	metric = routing_dist + y;

    db->db_forw_dist = routing_dist;

    if (ndb == NULL) {
	if (forw_addr == 0) {
	    add_better_ex_path(db, route, metric, area, pdb);
	} else {
	    /* forward address case */
	    add_better_ex_path_fwd(db, f_ndb, f_db, metric, area, 
				   gateway, pdb);
	}
    } else {
	/* there is an external route to this destination */
	ndb_type2 = ndb->route_type & (RTYPE_EXTERN2|RTYPE_NSSA2);
	p_bit = ex_lsa->la_hdr.lsh_options & OPTION_N_P_BIT;
	ndb_p_bit = ndb->ospf_nssa_p_bit;
	ndb_forw_addr = ndb->forw_addr;
	external = (type == LSA_TYPE_ASE);
	ndb_external = ndb->route_type & (RTYPE_EXTERN1|RTYPE_EXTERN2);
	db_lost = FALSE;
	
	if (!ndb_type2 && type2) {
	    /*
	     * Type 1 route exists
	     */
	    ospf_spf_ext_acl_buginf("\nOSPF: Type 1 route exist");
	    db_lost = TRUE;
	} else if ((ndb_type2 && type2) || (!ndb_type2 && !type2)) {
	    /*
	     * Same type
	     */
	    if ((old_metric == metric) && (!ndb_type2 || 
					   (ndb->forw_dist == routing_dist))) {
		/*
		 * Same distance
		 */
		if (ndb_external) {
		    /*
		     * Type 5 external route but we have type 7 external
		     */
		    if (!external) {
			ospf_spf_ext_acl_buginf("\nOSPF: Type 5 route exist");
			db_lost = TRUE;
		    }
		} else {
		    /*
		     * Type 7 with P bit and non-zero forward addr but
		     * we have any other type 7
		     */
		    if ((ndb_p_bit && ndb_forw_addr) && 
			(!external && (!p_bit || !forw_addr))) {
			ospf_spf_ext_acl_buginf("\nOSPF: Type 7 with P bit"
						" and non-zero forwarding"
						" address exist");
			db_lost = TRUE;
		    }
		}
	    } else {
		if (old_metric < metric) {
		    ospf_spf_ext_acl_buginf("\nOSPF: higher cost");
		    db_lost = TRUE;
		} else if ((old_metric == y) && ndb_type2 && 
			 (ndb->forw_dist < routing_dist)) {
		    ospf_spf_ext_acl_buginf("\nOSPF: higher forward cost");
		    db_lost = TRUE;
		}
	    }
	}
	if (db_lost) {
	    /*
	     * The existing route prevails, so keep
	     * the previous routes.
	     */
	    if (partial)
		external_path_delete(pdb, area, db, route_type); 
	    goto end_process;
	}
	
	/* install new route */
	if (forw_addr == 0) {
	    add_better_ex_path(db, route, metric, area, pdb);
	} else {
	    /* forward address case */
	    add_better_ex_path_fwd(db, f_ndb, f_db, metric, area, 
				   gateway, pdb);
	}
    }

/* check point 4 */
    currenttime = runtime();
    time[2] = currenttime - lasttime;
    lasttime = currenttime;	    

    add_ex_route(pdb, area, db, route_type, partial);

/* check point 5 */
    currenttime = runtime();
    time[3] = currenttime - lasttime;
    lasttime = currenttime;	    

    if (partial)
 	external_path_delete(pdb, area, db, route_type);

/* check point 6 */
    currenttime = runtime();
    time[4] = currenttime - lasttime;
    lasttime = currenttime;	    

    write_circular(PROCESS_ASE_TIME, time, 5 /* there are five steps */);

  end_process:
    if ((partial) && ex_index < 10) {
	rtr_id[ex_index] = ex_lsa->la_hdr.lsh_adv_rtr;
	ex_index++;
	temp_db = find_other_ex_db(area, ex_lsa->la_hdr.lsh_id, 
				   ex_lsa->la_mask, &rtr_id[0], 
				   ex_index, type);
	/*
	 * Because LSA can have host bit set in the link state ID, we
	 * have to check for it, as well as normal LSA ID, no matter
	 * which ID we start from.
	 */
	if (!temp_db && !looked_for_another_lsid) {
	    if (ex_lsa->la_hdr.lsh_id == 
		(ex_lsa->la_hdr.lsh_id & ex_lsa->la_mask)) {
		another_lsid = (ex_lsa->la_hdr.lsh_id |
				~ex_lsa->la_mask);
	    } else {
		another_lsid = (ex_lsa->la_hdr.lsh_id &
				ex_lsa->la_mask);
	    }
	    temp_db = find_other_ex_db(area, another_lsid, 
				       ex_lsa->la_mask, &rtr_id[0], 
				       ex_index, type);
	    looked_for_another_lsid = TRUE;
	}

	if ((temp_db) && (temp_db != db)) {
	    db = temp_db;
	    goto back;
	}
    }
    ospf_spf_ext_acl_pass = FALSE;
} /* end of process_ase_sub */

/*
 * process_ase
 * Process type 5 and type 7 LSA
 */
void process_ase (lsdbtype *db, pdbtype *pdb, areatype *area, boolean partial)
{
   lsdbtype *save_db;
   ex_lsatype *ex_lsa = (ex_lsatype *) db->db_lsa_ptr;
   ulong lsid = ex_lsa->la_hdr.lsh_id;
   ulong mask = ex_lsa->la_mask;
   ulong another_lsid = (lsid == (lsid & mask)) ? (lsid | ~mask) : (lsid & mask);
   areatype *ar;
   ndbtype *ndb;
   list_queue flood_list;
   ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

   flood_list.q_first = NULL;
   flood_list.q_last = NULL;
   
   save_db = db;
   if (partial) {
       pdb->local_ospf_spf_instance++;

       /*
	* Process type 5 LSA
	*/
       if (LSA_TYPE(db) == LSA_TYPE_7_ASE) {
	   db = find_other_ex_db(ospf_pdb->ospf_dummy_area, lsid,
				 mask, NULL, 0, LSA_TYPE_ASE);
	   if (!db)
	       db = find_other_ex_db(ospf_pdb->ospf_dummy_area, another_lsid,
				     mask, NULL, 0, LSA_TYPE_ASE);
       }
       if (db)
	   process_ase_sub(db, pdb, ospf_pdb->ospf_dummy_area, partial);

       /*
	* Process type 7 LSA
	*/
       for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	   if (!ar->ar_nssa)
	       continue;
	   if (LSA_TYPE(save_db) == LSA_TYPE_7_ASE)
	       db = save_db;
	   else 
	       db = find_other_ex_db(ar, lsid, mask, NULL, 0, LSA_TYPE_7_ASE);
	   if (!db) {
	       db = find_other_ex_db(ar, another_lsid, mask, NULL, 0, 
				     LSA_TYPE_7_ASE);
	   }
	   if (db)
	       process_ase_sub(db, pdb, ar, partial);
       }       
   } else {
       process_ase_sub(db, pdb, area, partial);
       /*
	* If we are doing full spf for type 5, no need to check for
	* translation.
	*/
       if (LSA_TYPE(db) == LSA_TYPE_ASE) {
	   return;
       }
   }

   /*
    * See if we should perform translation.
    * If we are not ABR or do not have NSSA, exit. Otherwise, If partial, 
    * we need to do the translation explictly here to add or remove the 
    * type 5 LSA.
    * For full, type 5 is removed in ex_delete_old_routes() and added by
    * ospf_set_summary_timer() in run_spf().
    */
   if (!pdb->ospf_area_brtr || !pdb->ospf_pdb->ospf_nssa_area_count ||
       !partial) {
       return;
   }
   ndb = ospf_lookup_net_route(area, lsid & mask, mask, 0, FALSE);
   ospf_translate_one_type7(pdb, ndb, lsid, mask, &flood_list, TRUE);
   ex_flood_area(pdb, &flood_list, NULL, NULL);
}

/*
 * virtual_link_check
 * checks if the virtual links are
 * up or down in an area.
 */

static void virtual_link_check (areatype *area)
{
    vlinktype *vlink;
    boolean flag;
    idbtype *idb;
    ospf_idb_info *ospf_info;
    list_queue	flood_list;
    areatype *backbone_area = area->ar_pdb_ptr->ospf_backbone;

    for (vlink = area->ar_vlink; vlink; vlink = vlink->vl_next) {
	idb = vlink->vl_idb;
	ospf_info = idb->ospf_info;
	if (ospf_vidb_up(idb, &flag)) {
	    /* 
	     * This interface is up.
	     * check if there is a change in
	     * the state of the interface.
	     */
	    ospf_info->enabled = TRUE;
	    if ( flag == TRUE) {
		if (ospf_info->if_state == IFS_DOWN) {
		    /*
		     * virtual link became reachable. Send i_up event
		     */
		    (*(if_state_machine[IFE_UP][ospf_info->if_state]))
			(idb, NULL);
		} else if (backbone_area) {
		    /* 
		     * virtual link cost changed. Originate a new
		     * router lsa for the backbone
		     */
		    flood_list.q_first = NULL;  
		    flood_list.q_last  = NULL;
		    build_rtr_lsa(backbone_area, &flood_list, FALSE);
		    flood_area(backbone_area, &flood_list, NULL, NULL);
		}
	    }
	} else {
	    if (flag == TRUE) {
		(*(if_state_machine[IFE_DOWN][ospf_info->if_state]))
		    (idb, NULL);
	    }
	}
    }
} /* end of virtual_link_check */

/*
 * spf_ex_init
 * Invalidate all the routes
 */
	
static void spf_ex_init (pdbtype *pdb, rbTree *lsdbase)
{
    lsdbtype	*db;
    int		count = 0;
    
    for (db = db_get_first_db(lsdbase); db; db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	if ((db->db_lsa_ptr == NULL)
	    || (db->db_lsa_ptr->lsh_adv_rtr == pdb->ospf_rtr_id)
 	    || (((ADV_AGE(db) >= MAXAGE) || db->db_freeme)
		&& !db->db_rt_table))
	    continue;
	db_spf_init_common(db, SUM_LS_INFINITY);
    }
} /* end of spf_ex_init */

/*
 * spf_ex
 * processes all external link states
 * and adds the routes if necessary.
 */
static void spf_ex (pdbtype *pdb, areatype *area, uchar type)
{
    lsdbtype 	*db;
    int 	count = 0;
    rbTree	*lsdbase;

    ospf_event(OC_GENERIC, (ulong) "spf_ex", type);
    ospf_spf_ext_buginf("\nOSPF: Started Building Type %d External Routes",
			type);
    ospf_destroy_faddrq(pdb);
    area->ar_spf_changes &= ~(SPF_SCHD_ASE | SPF_SCHD_7_ASE);
    lsdbase = area->ar_lsdbase[type];
    spf_ex_init(pdb, lsdbase);
    for (db = db_get_first_db(lsdbase); db;
	 db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	if ((db->db_lsa_ptr == NULL)
	    || (db->db_lsa_ptr->lsh_adv_rtr == pdb->ospf_rtr_id)
 	    || (((ADV_AGE(db) >= MAXAGE) || db->db_freeme)
		&& !db->db_rt_table))
	    continue;
	process_ase(db, pdb, area, FALSE);
    }
} /* end of spf_ex */

/*
 * spf_sum_init
 * Invalidate all the routes
 * and init them.
 */

static void spf_sum_init (areatype *area)
{
    pdbtype	*pdb;
    lsdbtype 	*db;
    ushort	type;
    int		count = 0;
    
    pdb = area->ar_pdb_ptr;
    
    /* grab all of the links */
    for (type = LSA_TYPE_SUM_NET; type <= LSA_TYPE_SUM_ASB; type++) {
        for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	     db = db_get_next_db(db)) {
	    check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	    if ((db->db_lsa_ptr == NULL) 
		|| (db->db_lsa_ptr->lsh_adv_rtr == ROUTER_ID(area))
		|| (((ADV_AGE(db) >= MAXAGE) || db->db_freeme)
		    && !db->db_rt_table))
		continue;
	    if (ADV_RTR(db) != pdb->ospf_rtr_id)
		db->db_rtr_reachable = FALSE;
	    db_spf_init_common(db, SUM_LS_INFINITY);
	}
    }
} /* end of spf_sum_init */

/*
 * spf_sum
 * processes all summary link states
 * and adds the routes if necessary.
 */

static void spf_sum (areatype *area)
{
    pdbtype	*pdb;
    lsdbtype 	*db;
    ushort	type;
    int 	count = 0;

    ospf_event(OC_GENERIC, (ulong) "spf_sum", area->ar_id);
    pdb = area->ar_pdb_ptr;
    ospf_spf_inter_buginf("\nOSPF: running spf for summaries area %s", 
			  area->ar_id_str);

#ifdef NOTDEF    
    ospf_spec_buginf("\nOSPF: Start full SPF for summaries, area %s, "
		   " process time %ums", area->ar_id_str, runtime());
#endif

    check_ospf_suspend(pdb, DBASE_ACTIVE);
    spf_sum_init(area);
      /* grab all of the links */
    for (type = LSA_TYPE_SUM_NET; type <= LSA_TYPE_SUM_ASB; type++) {
        for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	     db = db_get_next_db(db)) {
	    check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	    /* don't run SPF if
	     * db is self-originated
	     * db is MAXAGE
	     * this area is stub summary and db is not default summary LSA
	     */
	    if ((db->db_lsa_ptr == NULL) 
		|| (db->db_lsa_ptr->lsh_adv_rtr == ROUTER_ID(area))
		|| (((ADV_AGE(db) >= MAXAGE) || db->db_freeme)
		    && !db->db_rt_table)
		|| (area->ar_stub_nosum &&
		    ((sum_lsatype *)db->db_lsa_ptr)->la_hdr.lsh_id != 0))
		continue;
	    process_sum(db, area, FALSE);
	}
    }
} /* end of spf_sum */

/*
 * ospf_force_spf
 * schedules SPF to run all
 * link states in all areas in
 * the router.
 */

void ospf_force_spf (pdbtype *pdb, idbtype *idb)
{
    SAVE_CALLER();

    ospf_event(OC_GENERIC, (ulong) "ospf_force_spf", caller());
    
    if (idb)
	return;

    if (ospf_debug_spf_monitor || ospf_debug_spf)
	buginf("\nOSPF: Force running SPF");

    pdb->ospf_flag |= OSPF_IMMEDIATE_SPF;
}
/*
 * ospf_set_all_for_immediate_spf
 */
void ospf_set_all_for_immediate_spf (pdbtype *pdb)
{
    areatype *area;
    
    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	area->ar_schd_spf = TRUE;
	area->ar_spf_changes |= (SPF_SCHD_NET | SPF_SCHD_RTR |
				 SPF_SCHD_SNET | SPF_SCHD_ASB |
				 SPF_SCHD_7_ASE);
    }
    /*
     * Take care of dummy area
     */
    area = pdb->ospf_pdb->ospf_dummy_area;
    area->ar_spf_changes |= SPF_SCHD_ASE;
    area->ar_schd_spf = TRUE;
    pdb->run_spf = TRUE;
    GET_TIMESTAMP(pdb->spf_time);
    pdb->spf_wait_interval = 0;
}

/* spf
 * 
 * Set flags to indicate that OSPF needs to schedule SPF calculation
 */

void spf (areatype *area, ushort change, ulong lsid)
{
    char str[20];
    ushort schd_type5;
    pdbtype *pdb = area->ar_pdb_ptr;

    ospf_event(OC_SPF, area->ar_id, change);
    if (change == 0) {
	return; 
    }

    if (ospf_debug_spf_monitor || ospf_debug_spf) {
	change2lsatype(change, str);
	buginf("\nOSPF: Schedule SPF in area %s"
	       "\n      Change in LS ID %i, LSA type %s", 
	       area->ar_id_str, lsid, str);
    }
    pdb->ospf_pdb->ospf_spf_changes |= change;
    pdb->ospf_flag |= OSPF_SCHD_SPF;    

    /*
     * Ensure that SPF_SCHD_ASE only set for
     * dummy area.
     */
    schd_type5 = change & SPF_SCHD_ASE;
    change &= ~SPF_SCHD_ASE;
    if (change) {
	area->ar_spf_changes |= change;
	area->ar_schd_spf = TRUE;
    }
    if (schd_type5) {
	pdb->ospf_pdb->ospf_dummy_area->ar_spf_changes |= schd_type5;
	pdb->ospf_pdb->ospf_dummy_area->ar_schd_spf = TRUE;
    }
}


/*
 * ospf_area_set_spf_timer
 * Examine the area flag and decide whether SPF should be
 * run.
 */
static boolean ospf_area_set_spf_timer (areatype *area)
{
    pdbtype *pdb = area->ar_pdb_ptr;

    if (area->ar_schd_spf && area->ar_spf_changes) {
	if ((pdb->run_spf == FALSE) && (SPF_HOLD_EXPIRED(pdb))) {
	    GET_TIMESTAMP(pdb->spf_time);
	    pdb->spf_wait_interval = pdb->ospf_spf_schd_delay;
	    ospf_spec_buginf("\nOSPF: schedule SPF: "
			     "spf_time %ums wait_interval %us",
			     pdb->spf_time, pdb->spf_wait_interval);
	}
	pdb->run_spf = TRUE;
	return TRUE;
    }
    return FALSE;
}

/* ospf_set_spf_timer
 *
 * Set SPF timer if there has been a change in an area and no SPF
 * scheduled yet.
 */
void ospf_set_spf_timer (pdbtype *pdb)
{
    areatype *area;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (ospf_area_set_spf_timer(area))
	    return;
    }
    ospf_area_set_spf_timer(pdb->ospf_pdb->ospf_dummy_area);
}

/*
 * Find out the highest router ID amoung ABR/ASBR
 */
static ulong ospf_highest_rtr_id (pdbtype *pdb, areatype *area, ushort dest_type)
{
    rtr_route *route;
    ulong highest_rtr_id;
    int i;
    ospf_rtr_path *path;

    highest_rtr_id = pdb->ospf_rtr_id;

    for (i = 0; i < HTBLSIZE; i++) {
	for (route = (pdb->ospf_routes)[i]; route; route = route->rt_next) {
	    for (path = route->rt_path_list; path; path = path->orp_next) {
		if ((path->orp_area == area) && 
		    (path->orp_dest_type & dest_type)) {
		    if (highest_rtr_id < route->rt_dest)
			highest_rtr_id = route->rt_dest;
		    break;
		}
	    }
	}
    }
    return (highest_rtr_id);
}


/*
 * run_spf
 * looks at each area and run SPF if
 * necessary
 */

void run_spf (pdbtype *pdb)
{
    areatype	*area;
    ushort	change_record;
    ulong       start_time, lasttime;
    int i;
    spfruntimetype spf_start_time;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "run_spf", 0);
    GET_TIMESTAMP(spf_start_time.timestamp);
    for (i=0; i<7; i++)
	spf_start_time.mtime[i] = 0;
    change_record = 0;
    pdb->ospf_pdb->ospf_spf_changes = 0;
    start_time = runtime();
    ospf_spec_buginf("\nOSPF: Begin SPF at %#Tams, process time %ums",
		     spf_start_time.timestamp, start_time);
    ospf_spec_buginf("\n      spf_time %ums, wait_interval %us",
		     pdb->spf_time, pdb->spf_wait_interval);

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (area->ar_schd_spf) {
	    change_record |= area->ar_spf_changes;
	    ospf_spf_intra_buginf("\nOSPF: running SPF for area %s", 
				  area->ar_id_str);
	    area->ar_schd_spf = FALSE;
	    if (area->ar_spf_changes & (SPF_SCHD_NET | SPF_SCHD_RTR)) {
		spf_init(area);

                lasttime = runtime();
		spf_intra(area);
                spf_start_time.mtime[0] += runtime() - lasttime;

                lasttime = runtime();
		change_record |= delete_old_routes(area);
                spf_start_time.mtime[1] += runtime() - lasttime;

		change_record |= area->ar_spf_changes;
		virtual_link_check(area);
		/*
		 * Now we know all ABRs in the area, see if we
		 * are responsible for translating type-7 LSA
		 */
		if (pdb->ospf_area_brtr && area->ar_nssa) {
		    if (pdb->ospf_rtr_id == 
			ospf_highest_rtr_id(pdb, area, DTYPE_ABR)) {
			area->ar_nssa_translate_type7 = TRUE; 
		    } else {
			area->ar_nssa_translate_type7 = FALSE;
		    }
		}
		check_ospf_suspend(pdb, DBASE_ACTIVE);
	    }
	}
    }

    /*
     * We always run summary. Clear SPF_SCHD_SNET and SPF_SCHD_ASB before
     * running spf_sum().
     *
     * If we are ABR and has at least one neighbor, we only go through
     * summaries from the backbone as mentioned in the RFC. Otherwise, 
     * we go through summaries from all areas in order to work correctly
     * in case of (1) discontiguous backbone and (2) non-ABR attached to 
     * multiple areas.
     */
    if (pdb->ospf_area_brtr && 
	ospf_backbone_active(pdb->ospf_backbone, TRUE)) {
	areatype *backbone_area = NULL;
	for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	    if (area->ar_id == BACKBONE_AREA_ID) {
		backbone_area = area;
		continue;
	    }
	    spf_sum_init(area);
	    sum_delete_old_routes(area);
	    /*
	     * Clears all flag for non-backbone area.
	     */
	    area->ar_spf_changes &= ~(SPF_SCHD_SNET | SPF_SCHD_ASB | 
				      SPF_SCHD_7_ASE);
	}

	if (backbone_area == NULL) {
	    errmsg(&msgsym(NOBACKBONE, OSPF));
	    return;
	}

	/* Keep record and clear change flags
	 */
	change_record |= backbone_area->ar_spf_changes;
	backbone_area->ar_spf_changes &= ~(SPF_SCHD_SNET | SPF_SCHD_ASB | 
					    SPF_SCHD_7_ASE);

        lasttime = runtime();
	pdb->local_ospf_spf_instance++;
	spf_sum(backbone_area);
        spf_start_time.mtime[2] = runtime() - lasttime;

        lasttime = runtime();
	sum_delete_old_routes(backbone_area);
        spf_start_time.mtime[3] = runtime() - lasttime;
    } else { 
        spf_start_time.mtime[2] = 0; spf_start_time.mtime[3] = 0;
	pdb->local_ospf_spf_instance++;
	for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	    change_record |= area->ar_spf_changes;
	    area->ar_spf_changes &= ~(SPF_SCHD_SNET | SPF_SCHD_ASB);

            lasttime = runtime();
	    spf_sum(area);
            spf_start_time.mtime[2] += runtime() - lasttime;

            lasttime = runtime();
	    sum_delete_old_routes(area);
            spf_start_time.mtime[3] += runtime() - lasttime;
	}
    }
    /*
     * The following two lines of code is due 
     * an unknown bug in the code which is 
     * leaving invalid router entries in
     * the router route table.
     */
    for (area = pdb->ospf_area_list; area; area = area->ar_next)
	remove_invalid_routes(area);

    if (pdb->ospf_flag & REDIST_LOST) {
	pdb->ospf_flag &= ~REDIST_LOST;
	ospf_perform_asbr_duty(pdb);
    }
    
    /*
     * run stupid externals always
     * due to forwarding addresses
     * comming in unnoticed.
     */

    /*
     * Reset SPF flag for dummy area
     * as we are going to do SPF for type 5 now.
     */
    ospf_pdb->ospf_dummy_area->ar_schd_spf = FALSE;
    pdb->local_ospf_spf_instance++;
    lasttime = runtime();
    spf_ex(pdb, ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE);
    ex_delete_old_routes(pdb, ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE);
    spf_start_time.mtime[4] = runtime() - lasttime;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	/*
	 * We need to run SPF for type 7 LSA even though
	 * this area is not NSSA anymore. As it is the
	 * only way to remove type 7 LSA and its route and the translated
	 * type 5 LSA when an area is configured to switch
	 * from NSSA to normal area.
	 * No siginificant CPU problem here as the type 7 database
	 * will be empty once the old type 7 is removed.
	 */
	spf_ex(pdb, area, LSA_TYPE_7_ASE);
	ex_delete_old_routes(pdb, area, LSA_TYPE_7_ASE);
    }

    lasttime = runtime();
    spf_start_time.mtime[5] = runtime() - lasttime;

    if (pdb->ospf_area_brtr && pdb->ospf_pdb->ospf_nssa_area_count)
	ospf_set_summary_timer(pdb, OSPF_TRANSLATE_TYPE7);

    pdb->run_spf = FALSE;
    pdb->spf_wait_interval = pdb->ospf_spf_hold_interval;
    GET_TIMESTAMP(pdb->spf_time);
    
    ospf_spec_buginf("\nOSPF: End SPF at %#Tams, Total elapsed time %ums",
		     pdb->spf_time, runtime() - start_time);
    ospf_spec_buginf("\n        Intra: %ums, Inter: %ums, External: %ums\n",
		     spf_start_time.mtime[0],
		     spf_start_time.mtime[2],
		     spf_start_time.mtime[4]);

    spf_start_time.mtime[6] = (runtime() - start_time);
    spf_start_time.change = change_record;
    add_to_array(spf_start_time);
} /* end of run_spf */


/*
 * ospf_schedule_partial_spf
 * Schedule summary/external partial SPF request
 */
void ospf_schedule_partial_spf (areatype *area, lsdbtype *db)
{
    ospf_worktype *work;
    pdbtype *pdb = area->ar_pdb_ptr;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    /*
     * Skip partial SPF if corresponding full SPF is scheduled
     */
    if (pdb->run_spf) {
	if (ospf_pdb->ospf_spf_changes & (SPF_SCHD_INTRA|SPF_SCHD_INTER)) {
	    return;
	}
	if ((ospf_pdb->ospf_spf_changes & SPF_SCHD_EX) && 
	    ((LSA_TYPE(db) == LSA_TYPE_ASE) ||
	     (LSA_TYPE(db) == LSA_TYPE_7_ASE))) {
	    return;
	}
    }
    /*
     * Don't queue the same request twice
     */
    if (db->db_partial)
	return;
    ospf_spf_acl_buginf("\nOSPF: Schedule partial SPF - type %d id %i "
			"adv rtr %i", LSA_TYPE(db), LSA_ID(db), ADV_RTR(db));
    work = ospf_work_malloc();
    if (!work)
	return;
    work->ow_db = db;
    db->db_partial = TRUE;
    db->db_work = work;
    if (LSA_TYPE(db) != LSA_TYPE_ASE) {
	work->ow_area = area;
	if (LSA_TYPE(db) != LSA_TYPE_7_ASE) {
	    enqueue(&pdb->ospf_pdb->ospf_partial_sum_spfQ, work);
	} else {
	    enqueue(&pdb->ospf_pdb->ospf_partial_nssa_spfQ, work);
	}
    } else {
	work->ow_area = pdb->ospf_pdb->ospf_dummy_area;
	enqueue(&pdb->ospf_pdb->ospf_partial_ex_spfQ, work);
    }
}


/*
 * ospf_clean_partial_spfQ
 * Remove partial SPF request for the area
 */
void ospf_clean_partial_spfQ (areatype *area, queuetype *partial_spfQ)
{
    ospf_worktype *work;
    lsdbtype *db;
    int count = QUEUESIZE(partial_spfQ);

    while (count--) {
	work = dequeue(partial_spfQ);
	if (!area || (work->ow_area == area)) {
	    db = work->ow_db;
	    if (db) {
		db->db_partial = FALSE;
		db->db_work = NULL;
	    }
	    ospf_work_free(work);
	} else {
	    enqueue(partial_spfQ, work);	    
	}
    }
}

static int ospf_process_partial_spfQ (pdbtype *pdb, queuetype *partial_spfQ)
{					    
    ospf_worktype *work;
    lsdbtype *db;
    int	work_count = 0;
    ushort spf_changes = 0;

    while ((work = dequeue(partial_spfQ))) {
	db = work->ow_db;
	if (db) {
	    db->db_partial = FALSE;
	    db->db_work = NULL;
	    switch (LSA_TYPE(db)) {
	        case LSA_TYPE_SUM_ASB:
		    /* 
		     * Add as border rtr and schedule spf for external LSA
		     * as new external dest may have become reachable 
		     */
		    spf_changes = SPF_SCHD_EX;
	        case LSA_TYPE_SUM_NET:
		    process_sum(db, work->ow_area, TRUE);
		    break;
	        case LSA_TYPE_ASE:
	        case LSA_TYPE_7_ASE:
		    process_ase(db, pdb, work->ow_area, TRUE);
		    break;
	    }
	}
	ospf_work_free(work);
	if (work_count++ > LSA_PROCESSCOUNT)
	    break;
    }
    return (spf_changes);
}

/*
 * ospf_service_partial_spf
 * Process partial SPF request
 */
void ospf_service_partial_spf (pdbtype *pdb) 
{
    ushort spf_changes = 0;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_spf_buginf("\nOSPF: Service partial SPF %d/%d/%d",
		    QUEUESIZE(&ospf_pdb->ospf_partial_sum_spfQ),
		    QUEUESIZE(&ospf_pdb->ospf_partial_ex_spfQ),
		    QUEUESIZE(&ospf_pdb->ospf_partial_nssa_spfQ));
    /*
     * Intra SPF force full summary SPF and full external SPF.
     * We can skip the partial processing accordingly.
     */
    if (pdb->run_spf) {
	if (ospf_pdb->ospf_spf_changes & (SPF_SCHD_INTRA|SPF_SCHD_INTER)) {
	    if (!QUEUEEMPTY(&ospf_pdb->ospf_partial_sum_spfQ)) {
		ospf_spf_inter_buginf("\nOSPF: Clean up partial summary "
				      "SPF queue");
		ospf_clean_partial_spfQ(NULL, 
					&ospf_pdb->ospf_partial_sum_spfQ);
	    }
	}
    }
    spf_changes = 
	ospf_process_partial_spfQ(pdb, &ospf_pdb->ospf_partial_sum_spfQ);

    if (spf_changes) {
	spf(ospf_pdb->ospf_dummy_area, spf_changes, 0);
    }

    check_ospf_suspend(pdb, DBASE_ACTIVE);

    if (pdb->run_spf || spf_changes) {
	if (ospf_pdb->ospf_spf_changes) {
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	    if (!QUEUEEMPTY(&ospf_pdb->ospf_partial_ex_spfQ)) {
		ospf_spf_ext_buginf("\nOSPF: Clean up partial external "
				    "SPF queue");
		ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_ex_spfQ);
	    }
	    if (!QUEUEEMPTY(&ospf_pdb->ospf_partial_nssa_spfQ)) {
		ospf_spf_ext_buginf("\nOSPF: Clean up partial type 7 external "
				    "SPF queue");
		ospf_clean_partial_spfQ(NULL, 
					&ospf_pdb->ospf_partial_nssa_spfQ);
	    }
	}
    }
    spf_changes |= 
	ospf_process_partial_spfQ(pdb, &ospf_pdb->ospf_partial_ex_spfQ);
    check_ospf_suspend(pdb, DBASE_ACTIVE);

    spf_changes |= 
	ospf_process_partial_spfQ(pdb, &ospf_pdb->ospf_partial_nssa_spfQ);
    check_ospf_suspend(pdb, DBASE_ACTIVE);
}
