/* $Id: novriproute.c,v 3.11.6.10 1996/09/05 23:28:21 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novriproute.c,v $
 *------------------------------------------------------------------
 * novriproute.c - support for Novell RIP routing protocol.
 * 
 * January 1988, Greg Satz 
 * May 1989, Bill Westfield, split generic code from xnsroute.c 
 * October 1992, Joanne Boyle, split Novell RIP from griproute.c
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novriproute.c,v $
 * Revision 3.11.6.10  1996/09/05  23:28:21  sluong
 * CSCdi54252:  Total count & number of routes could get out of sync in sh
 * ipx route
 * Branch: California_branch
 *
 * Revision 3.11.6.9  1996/08/28  13:25:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.6.8  1996/07/26  00:09:22  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.11.6.7  1996/06/28  23:33:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.6.6  1996/05/03  02:20:27  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.11.6.5  1996/04/29  17:02:32  sluong
 * CSCdi55250:  NULL dereference in novriproute.c
 * Branch: California_branch
 *
 * Revision 3.11.6.4  1996/04/09  22:33:20  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.11.6.3  1996/04/02  05:38:33  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.11.6.2  1996/03/26  18:10:50  john
 * CSCdi51858:  RIP ticks value off by one for subinterface network number
 * Branch: California_branch
 *
 * Revision 3.11.6.1  1996/03/18  22:54:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.4  1996/03/16  08:00:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.12.3  1996/03/13  02:15:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.2  1996/03/07  11:22:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.12.1  1996/02/20  22:01:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/05  23:12:46  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.10  1996/03/01  07:34:46  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.9  1996/02/14  23:01:46  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.8  1996/02/09  17:41:21  john
 * CSCdi47023:  IPX static routes on tied to a down interface are
 *              advertised up
 *
 * Revision 3.7  1996/01/04  07:13:03  sluong
 * CSCdi46224:  SAPs missing using RSUP when interface flaps
 *
 * Revision 3.6  1995/12/15  19:42:39  mschaef
 * CSCdi45600:  ipx cache wrong for some destinations when RIP
 *              route replaced NLSP
 *
 * Revision 3.5  1995/12/02  08:26:49  mschaef
 * CSCdi37520:  mac-address command not taken upon router reload or
 * initial config
 *
 * Revision 3.4  1995/11/20  17:55:36  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.3  1995/11/17  19:24:33  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:00:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:39:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/12  21:10:58  sluong
 * CSCdi41898:  clear ipx route <network> clears config as well as route
 *
 * Revision 2.10  1995/10/04  01:26:56  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *              Part 2: code review suggested better fix.
 *
 * Revision 2.9  1995/09/23  21:43:26  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.8  1995/08/25  14:12:00  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.7  1995/07/01 03:39:27  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.6  1995/06/27 05:53:28  sluong
 * CSCdi34380:  router crashed while displaying ipx routes
 *
 * Revision 2.5  1995/06/21  09:01:51  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/20  21:54:20  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/16  17:10:37  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.2  1995/06/09  13:26:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:29:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../xns/msg_ipx.c"		/* Not a typo, see logger.h */
#include "../os/free.h"
#include "config.h"
#include "xns.h"
#include "novell.h"
#include "novroute.h"
#include "novfast.h"
#include "../if/network.h"
#include "logger.h"
#include "novigrp.h"
#include "novisis.h"
#include "novnlsp.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "sys_registry.h"
#include "../xns/ipx_registry.h"

int next_hop_column; /* Used by route display routines */


/*
 * novell_really_count_routes
 * Count the total number of routes in the table
 */
static int novell_really_count_routes (void)
{
    int i, count = 0;
    novndbtype *p;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
	for (p = (*novell_table)[i]; p; p = p->next) {
	    count++;
	}
    }
#if 0					/* When the EIGRP support is there */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, count_route_patricia, &count);
#endif

    return(count);
}
static boolean compare_route_counts (void)
{
    if (novell_route_count == novell_really_count_routes()) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * Support for the Advertise To Lost Routes List.
 * This is a doubly linked list headed by the global structure
 * novell_atlr_head. An empty list points to itself.
 */
void novell_init_atlr_list (void)
{
    novell_atlr_head.next_atlr = novell_atlr_head.prev_atlr =
      &novell_atlr_head;
}
void novell_remove_from_atlr_list (novndbtype *ndb)
{
    if (ndb->next_atlr == NULL)
      return;
    ndb->next_atlr->prev_atlr = ndb->prev_atlr;
    ndb->prev_atlr->next_atlr = ndb->next_atlr;
    ndb->prev_atlr = ndb->next_atlr = NULL;
}
void novell_insert_into_atlr_list_tail (novndbtype *ndb)
{
    /*
     * If already in lost route queue, *remove* it. Then place it back in the
     * tail. This will cause a route learned on say, 10 interfaces, if truly
     * lost on all ten to move in and out of this queue 9 times. By doing
     * this, we allow there to be a half second delay between lost route
     * advertisements from any source before we advertise back the lost route
     * if we have any remaining. So a 'slow neighbor' can advertise all
     * lost routes to us (1/2 second per interface) without us advertising
     * them back to it. Even with the 10 path example given above, with
     * 9 paths lost, 1 remaining, it would take at worst 4.5 seconds, often
     * much sooner, to advertise back the route to the router that lost it,
     * well within the allowable time to preserve sessions.
     */
    novell_remove_from_atlr_list(ndb);

    ndb->next_atlr = &novell_atlr_head;
    ndb->prev_atlr = novell_atlr_head.prev_atlr;
    novell_atlr_head.prev_atlr->next_atlr = ndb;
    novell_atlr_head.prev_atlr = ndb;

    /*
     * Note the time this route was scheduled for ATLR:
     */
    GET_TIMESTAMP(ndb->atlr_time);

    /*
     * Fire up the ATLR timer if idle:
     */
    if (!mgd_timer_running(&novell_atlr_timer)) {
	mgd_timer_start(&novell_atlr_timer,
			NOVELL_ATLR_DELAY + (NOVELL_ATLR_GRANULARITY/4));
    }
}
novndbtype *novell_get_atlr_list_head (void)
{
    /* test empty condition */
    if (novell_atlr_head.next_atlr == &novell_atlr_head)
        return(NULL);
    return(novell_atlr_head.next_atlr);
}
int novell_count_atlr_list_length (void)
{
    int count = 0;
    novndbtype *ndb;

    ndb = novell_atlr_head.next_atlr;

    while ((ndb != &novell_atlr_head) && (ndb != NULL)) {
	count++;
	ndb = ndb->next_atlr;
	if (count > 1000000) {
	    /* More than 1 million nets? Hardly. Break out. */
	    break;
	}
    }
    return(count);
}

/*
 *
 * novell_mark_advertise_to_lost
 * Mark a network as needing to be advertised to a router that lost it.
 */
void novell_mark_advertise_to_lost (novndbtype *ndb)
{
    if (novellrip_debug) {
	buginf("\nIPXRIP: Marking network %x for Advertisement to lost",
	       ndb->dstnet);
    }
    /* Place entry at the end of the current list */
    novell_insert_into_atlr_list_tail(ndb);
}

/*
 * nov_insert_ndb_placeholder
 *
 */
void nov_insert_ndb_placeholder (novndbtype *ndb, novndbtype *ph)
{
    if ((ndb == NULL) || ( ph == NULL) ||
	(ndb->type != IPX_NDB_ACTIVE) ||
	( ph->type != IPX_NDB_PLACEHOLDER) ||
	(ph->next_newest != NULL) ||
	(ph->prev_newest != NULL) ||
	(ph == novell_newest_routes) ||
	(ndb->options & IPX_NDB_OPTION_DELETED))
	{
	    return;
	}

    ph->next_newest = ndb;
    ph->prev_newest = ndb->prev_newest;
    if (ndb->prev_newest != NULL) {
	ndb->prev_newest->next_newest = ph;
    } else {
	novell_newest_routes = ph;
    }
    ndb->prev_newest = ph;
}

/*
 * nov_delete_ndb_placeholder
 *
 */
novndbtype *nov_delete_ndb_placeholder (novndbtype *ph)
{
    novndbtype *ndb;

    if (ph->type != IPX_NDB_PLACEHOLDER) {
	return(NULL);
    }

    if ((ph->next_newest == NULL) && (ph->prev_newest == NULL)) {
	return(ph);
    }

    /*
     * Prepare to return next active node:
     */
    ndb = ph->next_newest;

    /*
     * Delete placeholder:
     */
    if (ph->next_newest != NULL) {
	ph->next_newest->prev_newest = ph->prev_newest;
    }

    if (ph->prev_newest != NULL) {
	ph->prev_newest->next_newest = ph->next_newest;
    }

    /*
     * Update listhead if appropriate:
     */
    if (novell_newest_routes == ph) {
	novell_newest_routes = ph->next_newest;
    }

    ph->next_newest = NULL;
    ph->prev_newest = NULL;

    /*
     * Slide along past possible placeholders to return next active ndb:
     */
    while ((ndb != NULL) && (ndb->type == IPX_NDB_PLACEHOLDER)) {

	if ((ndb->type != IPX_NDB_ACTIVE) &&
	    (ndb->type != IPX_NDB_PLACEHOLDER))
	    {
		return(NULL);
	    }

	ndb = ndb->next_newest;
    }

    /*
     * Return next active ndb (if any):
     */
    return(ndb);
}

/*
 * nov_get_first_ndb
 *
 */
novndbtype *nov_get_first_ndb (void)
{
    novndbtype *ndb;

    /*
     * Slide along past possible placeholders to return first active ndb:
     */

    ndb = novell_newest_routes;

    while ((ndb != NULL) && (ndb->type != IPX_NDB_ACTIVE)) {

	if (ndb->type != IPX_NDB_PLACEHOLDER) {
	    return(NULL);
	}

	ndb = ndb->next_newest;
    }

    return(ndb);
}

/*
 * nov_get_next_ndb
 *
 */
novndbtype *nov_get_next_ndb (novndbtype *current)
{
    novndbtype *ndb;

    if (current == NULL) {
	return(NULL);
    }

    /*
     * Slide along past possible placeholders to return next active ndb:
     */
    ndb = current->next_newest;

    while ((ndb != NULL) && (ndb->type == IPX_NDB_PLACEHOLDER)) {
	ndb = ndb->next_newest;
    }

    if ((ndb != NULL) && (ndb->type != IPX_NDB_ACTIVE)) {
	return(NULL);
    }

    return(ndb);
}

/*
 * novell_mark_newest
 *
 * Promote ndb to head of newest list, without side effects.
 *
 */
void novell_mark_newest (novndbtype *ndb)
{
    GET_TIMESTAMP(ndb->ndb_changed);

    /* Delete from the current location, if any, and if not already newest */
    if ((!(ndb->options & IPX_NDB_OPTION_DELETED)) &&
	(ndb != novell_newest_routes)) {
	/* Move this entry out of the current list */
	if (ndb->prev_newest != NULL) {
	    ndb->prev_newest->next_newest = ndb->next_newest;
	}
	if (ndb->next_newest != NULL) {
	    ndb->next_newest->prev_newest = ndb->prev_newest;
	}
	/* Mark this as the newest route -- no prev, next is old newest */
	ndb->prev_newest = NULL;
	ndb->next_newest = novell_newest_routes;
	/* The old newest, if any, points to us */
	if (novell_newest_routes != NULL) {
	    novell_newest_routes->prev_newest = ndb;
	}
	/* Finally, the newest_routes pointer should point to this */
	novell_newest_routes = ndb;
    }
}

/*
 * novell_mark_changed
 * Mark a network as 'changed' for flash updates. Also, modify any SAPs
 * going to this network to new delay and hopcount values (if 'dosap' TRUE.)
 *
 */
void novell_mark_changed (novndbtype *ndb)
{

    /*
     * Keep a copy of the default route entry up to date:
     * Only treat the default route special if default route handling
     * is enabled.
     */
    if (ndb->dstnet == NOV_RIP_DEFAULT_ROUTE) {
	if ((ndb->hopcount >= novell_infinity) ||
	    (ndb->delay == NOVELL_INFDELAY) ||
	    TIMER_RUNNING(ndb->holddownuntil)) {
	    if (novell_default_ndb != NULL) {
		/* Do not clear the ndb yet */
		novcache_increment_version(NULL, NOVCACHE_NLSP_UNREACHABLE,
					   NOV_RIP_DEFAULT_ROUTE, 
					   NOV_NETMASK_ALL);
		/* OK, clear it now ... cache invalidate has seen it */
		novell_default_ndb = NULL;
	    }
	} else if (novell_default_route) {
	    novell_default_ndb = ndb;
	}
    }
    /* 
     * We don't advertise summary routes to the RIP cloud. Nor do we wish
     * to readvertise a reverse poisoned route. No need to promote either
     * to the front of the  novell_newest_routes list.
     */
    if (ndb->reverse_poisoned || (ndb->rt_mask != NOV_NETMASK_ALL)) {
	if (novellrip_debug || novellrip_event_debug) {
	    buginf("\nIPXRIP: Not marking summary %x %x for Flash Update",
		   ndb->dstnet, ndb->rt_mask);
	}
	return;
    }

    if (novellrip_debug || novellrip_event_debug) {
	buginf("\nIPXRIP: Marking network %x %x for Flash Update",
	       ndb->dstnet, ndb->rt_mask);
    }
    novell_mark_newest(ndb);

    process_set_boolean(novell_rip_part_wb, TRUE);
}

/*
 * novell_mark_rdb_changed
 * Mark an RDB block as having changed. Call all PDBs that have a redist
 * back into this, and report this event
 */
void novell_mark_rdb_changed (novndbtype *ndb, novrdbtype *rdb,
			      ulong flags,
			      enum IPX_ROUTE_EVENT event)
{
    novpdbtype *pdb;
    backuptype backup;

    /* Build backup info if appropriate */
    if (event == IPX_ROUTE_EVENT_PUSHED_OUT) {
	backup.pdbindex = ndb->novpdbindex;
	backup.backup_context_info = ((rdb->lsp_linkage.lsp_index << 16) &
				      rdb->lsp_linkage.lsp_version);
	backup.route_type = RTYPE_L1;
	ipx_insert_backup_info(&(ndb->backup_info), &backup,
			       NOV_INDEX2PDB(ndb->novpdbindex), FALSE);
    }
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->rdb_changed_callback) {
	    (*pdb->rdb_changed_callback)(pdb, ndb, rdb, event, flags);
	}
    }
}


/*
 * novell_mark_rip_changed
 * Do whatever work, including redistribution to other protocols, is required
 * to support changes to the RIP table.
 */
void novell_mark_rip_changed (novndbtype *ndb)
{
    int event;
    novpdbtype *pdb;

    /* Let RIP Flasher and SAP code know about it. */ 
    if ((ndb->hopcount >= novell_infinity) ||
	TIMER_RUNNING(ndb->holddownuntil)) {
	event = ROUTE_DOWN;
	if (ndb->hopcount >= novell_infinity) {
	    ndb->ext_hop = NOVELL_MAX_HOPS_ALLOWED;
	    ndb->delay = NOVELL_INFDELAY;
	    ndb->path_metric = METRIC_INACCESSIBLE;
	    ndb->igrp_delay = NOT_IBM_TICKS(ndb->delay);
	}
    } else
	event = ROUTE_UP;

    novell_mark_changed(ndb);
    pdb = nov_proctable[ndb->novpdbindex];
    nov_network_redist(pdb, ndb, event);
}

/*
 * novrip_route_adjust
 *
 */

void
novrip_route_adjust (idbtype *idb)
{
    nidbtype *nidb;
    boolean new_interface_up;
    void (*func)(nidbtype *);

    if ((!novell_running) || (!idb->novellnets))
	return;

    /* Handle the unique subinterface case of the interface being DELETED! */
    if (is_subinterface(idb) && (idb->subif_state == SUBIF_STATE_DELETED)) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		novell_delete_network(nidb);
	    }
	}
	return;
    }

    if (interface_up(idb)) {
	new_interface_up = TRUE;
	func = novell_nidb_bringup;
    } else {
	new_interface_up = FALSE;
	func = novell_nidb_bringdown;
    }

    nov_adjust_fastflags(idb);

    FOR_ALL_NIDBS(nidb) {
	if ((nidb->idb == idb) &&
	    (nidb->novell_interface_up != new_interface_up)) {
	    nidb->novell_interface_up = new_interface_up;
	    if (idb->hwptr->status & IDB_IEEEMASK) {
		ieee_copy(idb->hwptr->hardware, idb->novell_node);
	    }
	    (*func)(nidb);
	}
    }
}



/*
 * novrip_static_routes
 *
 * Handle static routes when an interface changes state.
 *
 */
void
novrip_static_routes (nidbtype *nidb, int event)
{
    novndbtype *p;
    int i, j;
    novrdbtype *pp;
    boolean mark_changed = FALSE;
    boolean deleted = FALSE;
    uchar *rinp, host[IEEEBYTES];

    for (i = 0; i < NOV_NETHASHLEN; i++) {
	if (event == ROUTE_UP) {
	    /*
	     * Put the floating static routes back in the table...
	     */
	    p = (*static_table)[i];
	    for (; p; p = p->next) {
		pp = p->path_list;
		if ((pp == NULL) ||
		    (pp->nidb != nidb) ||
		    (pp->nidb == NULL)) {
		    continue;
		}
		if (novrip_find_rt(p->dstnet, p->rt_mask, 
				   IPX_ROUTE_MATCH_EXACT) != NULL) {
		    continue;
		}
		if (p->options & IPX_NDB_OPTION_INTERFACE_STATIC) {
		    rinp = (uchar *)
		      &(pp->nidb->nov_ipxwan_remote_internal_net);
		    for (j = 0; j < 4; j++) {
			host[i] = *rinp++;
		    }
		    host[4] = host[5] = 0;
		} else {
		    ieee_copy(pp->srchost, host);
		}
		novrip_add_rt(&novfstatic_pdb, pp->nidb, p->dstnet, p->rt_mask,
			      pp->nidb->novellnet, host,
			      p->last_delay, p->last_hopcount,
			      (NET_FLOATING_STATIC | NET_STATIC),
			      IPX_ROUTE_ADD_NORMAL,
			      IPX_STATIC_DEFINE_TYPE_NO_NVGEN,
			      ((p->options & IPX_NDB_OPTION_INTERFACE_STATIC) ?
			       IPX_STATIC_TYPE_INTERFACE :
			       IPX_STATIC_TYPE_HOST),
			      IPX_FLOATING_STATIC_EXISTS_YES);
	    }
	}
	/*
	 * Put the normal static routes back in the table...
	 */
	p = (*novell_table)[i];
	for (; p; p = p->next) {
	    pp = p->path_list;
	    if ((!(p->flags & NET_STATIC)) ||
		(pp == NULL) ||
		(pp->nidb != nidb) ||
		(pp->nidb == NULL))
		continue;
	    if (event == ROUTE_UP) {
		if (((!(p->options & IPX_NDB_OPTION_VALID)) ||
		     TIMER_RUNNING(p->holddownuntil)) ||
		    nov_ipxwan_on_nidb(nidb))
		    mark_changed = TRUE;
		p->options |= IPX_NDB_OPTION_VALID;
		TIMER_STOP(p->holddownuntil);
		p->hopcount = p->ext_hop = p->last_hopcount;
		p->delay = p->ext_delay = p->last_delay;
		p->igrp_delay = NOT_IBM_TICKS(p->last_delay);
		GET_TIMESTAMP(pp->timer);
		if (nov_ipxwan_on_nidb(nidb)) {
		    pp->srcnet = nidb->novellnet;
		    rinp = (uchar *) &(nidb->nov_ipxwan_remote_internal_net);
		    for (j = 0; j < 4; j++) {
			pp->srchost[j] = *rinp++;
		    }
		    pp->srchost[4] = pp->srchost[5] = 0;
		}
	    } else {
		if ((p->options & IPX_NDB_OPTION_VALID) &&
		    (!TIMER_RUNNING(p->holddownuntil))) {
		    mark_changed = TRUE;
		    deleted = TRUE;
		}
		TIMER_START(p->holddownuntil,
			    pp->nidb->idb->nov_update_interval);
	        novcache_increment_version(NULL, NOVCACHE_RIP_STATIC_RT,
					   nidb->novellnet, NOV_NETMASK_ALL);
	    }
	    if (mark_changed) {
		novell_mark_rdb_changed(p, pp, NET_STATIC,
					!deleted ?
					IPX_ROUTE_EVENT_ADD :
					IPX_ROUTE_EVENT_DELETED);
		novell_mark_rip_changed(p);
	    }
	}
	/* Need to do the same thing for the patricia tree */
	rn_walktree_blocking(nov_rnhead->rnh_treetop, patricia_static_routes, 
			     event, nidb);
    }
}


#ifdef DEBUG_NIDB
/*
 * dump_rdb
 */
void
dump_rdb (novrdbtype *rdb)
{
    if (!(novellrip_debug || novellrip_event_debug))
	return;

    buginf("\n--rdb: via %x.%e (net %x, %s)",
	   rdb->srcnet,
	   rdb->srchost,
	   rdb->nidb->novellnet,
	   rdb->nidb->idb->namestring);
}

/*
 * dump_ndb
 */
void
dump_ndb (novndbtype *ndb)
{
    ulong pcnt;
    novrdbtype *rdb;

    if (!(novellrip_debug || novellrip_event_debug))
	return;

    buginf("\n-ndb: dstnet=%x, netmask=%x, hops=%d, flags=%x, pathcnt=%d",
	   ndb->dstnet,
	   ndb->rt_mask,
	   ndb->hopcount,
	   ndb->flags,
	   ndb->paths);

    pcnt = ndb->paths;
    rdb = ndb->path_list;

    while ((pcnt > 0) && (rdb != NULL)) {
	dump_rdb( rdb);
	rdb = rdb->next;
	pcnt--;
    }
}

/*
 * dump_free_rdb
 */
void
dump_free_rdb (novrdbtype *rdb)
{
    if (!(novellrip_debug || novellrip_event_debug))
	return;

    buginf("\nfree:");
    dump_rdb(rdb);
}

void
dump_free_ndb (novndbtype *ndb)
{
    if (!(novellrip_debug || novellrip_event_debug))
	return;

    buginf("\nfree:");
    dump_ndb(ndb);
}

void
dump_found_ndb (novndbtype *ndb)
{
    if (!(novellrip_debug || novellrip_event_debug))
	return;

    buginf("\nfound:");
    dump_ndb(ndb);
}
#endif

/*
 * nov_ndb_malloc
 *
 * Create a Novell NDB
 */
novndbtype *nov_ndb_malloc (void)
{
    novndbtype *ndb;

    if ((ndb = malloc_named(sizeof(novndbtype), "IPX NDB")) != NULL) {
	ndb->prev_newest = ndb->next_newest = NULL;
	ndb->next_atlr = ndb->prev_atlr = NULL;
	ndb->backup_info = NULL;
	ndb->options = 0;
	ndb->type = IPX_NDB_ACTIVE;
	novell_route_count++;
    }
    return(ndb);
}
/*
 * nov_ndb_malloc_placeholder
 *
 * Create a Novell NDB placeholder
 */
novndbtype *nov_ndb_malloc_placeholder (void)
{
    novndbtype *ndb;

    if ((ndb = malloc_named(sizeof(novndbtype), "IPX NDB PH")) != NULL) {
	ndb->type = IPX_NDB_PLACEHOLDER;
	/***** TEMPORARY *****/
	ndb->dstnet = 0x42424242;
	ndb->delay = ndb->hopcount = 1;
	ndb->rt_mask = NET_RIP;
	/***** TEMPORARY *****/
    }
    return(ndb);
}

/*
 * nov_ndb_free_newest
 *
 * Remove a Novell NDB from the newest list (only)
 */
void nov_ndb_free_newest (novndbtype *ndb)

{
    if (!ndb)
	return;

    if (novell_newest_routes == ndb) {
	novell_newest_routes = ndb->next_newest;
    }
    if (ndb->prev_newest != NULL) {
	ndb->prev_newest->next_newest = ndb->next_newest;
    }
    if (ndb->next_newest != NULL) {
	ndb->next_newest->prev_newest = ndb->prev_newest;
    }
    ndb->prev_newest = NULL;
    ndb->next_newest = NULL;

}

/*
 * nov_ndb_free
 *
 * Free a Novell NDB
 *
 * do_holddown tells us if we want a 10 second holddown or not for RIP routes
 */
void nov_ndb_free (novndbtype *ndb, enum IPX_ROUTE_FREE tablestatus,
		   boolean do_holddown)
{
    novndbtype *spf_ndb;
    backuptype *old_backup_info;
    novlsptype *desc;

    if (!ndb)
      return;

    ndb->options |= IPX_NDB_OPTION_DELETED;
    ndb->hopcount = NOVELL_MAX_HOPS_ALLOWED;
    ndb->delay = NOVELL_INFDELAY;
    ndb->path_list = NULL;
    ndb->nextpath = NULL;
    ndb->options &= ~IPX_NDB_OPTION_VALID;
    ndb->next = NULL;

    /* It's going away... take it out of the newest list */
    nov_ndb_free_newest(ndb);

    /* Remove it from this other doubly linked list */
    novell_remove_from_atlr_list(ndb);
    /*
     * Update total route count:
     */
    if (novell_route_count > 0) {
	novell_route_count--;
    }
    /*
     * Make sure the default route descriptor stays consistent:
     */
    if (ndb == novell_default_ndb) {
	novell_default_ndb = NULL;
    }

    /*
     * Common NLSP ndb cleanup. Make sure active/bkup ndb references
     * are consistent, even if call is non-NLSP (e.g., via nov_del_rt).
     * Without putting this in common code, things get really AFU.
     */
    if (ndb->flags & NET_NLSP) {
	spf_ndb = ndb->protocol_info.nlsp_info.spf_next;
	if (spf_ndb != NULL) {
	    ndb->protocol_info.nlsp_info.spf_next = NULL;
	    spf_ndb->protocol_info.nlsp_info.spf_next = NULL;
	}
    }
    /*
     * Common aggregated route cleanup. The lsp descriptor list should
     * be emptry before an aggregated route is deleted, but check anyway.
     */
    while (ndb->lsp_list != NULL) {
	desc = ndb->lsp_list;
	ndb->lsp_list = desc->next;
	free(desc);
    }
    
    if (tablestatus == IPX_ROUTE_FREE_NEVER_IN_TABLE) {
	/*
	 * Backup info should not normally be present in this case
	 * but cleanup anyway, just in case.
	 */
	if (ndb->backup_info != NULL) {
	    old_backup_info = ndb->backup_info;
	    ipx_delete_backup_list(old_backup_info);
	}
	free(ndb);
	return;
    }
    if (ndb->flags & (NET_RIP | NET_STATIC))
	novell_mark_rip_changed(ndb);
    /*
     * The NDB is not freed here, but put on this-here nov_freed_ndbQ, to
     * be handled by another process (novell_fork). This routine will free
     * the NDB, after calling the pdb->add_route routines that may apply.
     * This MUST be done in a separate process, because many routines call
     * nov_ndb_free in the middle of looping through the routing table. Adding
     * a route into the table in the middle of their traversal of it is a
     * "bozo no-no." However, non-IGRP routes are given some time to 'thaw'
     * before the request is made for other nets, to ensure we
     * don't hear back our own advertisements.
     */
    if (ndb->flags & NET_STATIC)
	nov_static_route_count--;
    if (do_holddown && (!(ndb->flags & (NET_IGRP|NET_NLSP)))) {
	TIMER_START(ndb->holddownuntil, 10 * ONESEC);
	enqueue(&nov_freed_ndbQ, ndb);
    } else {
	/* No need to wait. Put at FRONT of queue */
	TIMER_START(ndb->holddownuntil, 0);
	requeue(&nov_freed_ndbQ, ndb);
	novell_freed_ndbq_wakeup = 1;
    }
}

/*
 * novrip_shutdown
 *
 * Purge the route table "gracefully" as part of shutdown (allow
 * appropriate notifications as networks go down).
 */
void novrip_shutdown (void)
{
    novndbtype *p, *next_p;
    novpdbtype *pdb;
    int i;

    if (!novell_shutting_down) {
	return;
    }
    if (novellnlsp_debug || novelligrp_debug ||
	novellrip_debug || novellrip_event_debug) {
	buginf("\nIPX: purge route table");
    }
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	p = (*novell_table)[i];
	while (p) {
	    next_p = p->next;
	    if (!(p->flags & NET_CONNECTED)) {
		TIMER_START(p->holddownuntil, 0);
		pdb = nov_proctable[p->novpdbindex];
		nov_network_redist(pdb, p, ROUTE_DOWN);
		if (p->flags & NET_RIP) {
		    novell_mark_rip_changed(p);
		} else {
		    novell_mark_changed(p);
		}
	    }
	    p = next_p;
	}
    }
    /* Need to do the same thing for the patricia tree */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, patricia_shutdown);
}

/*
 * novrip_ager
 * Age the route tables. Will bring RIP routes INTO holddown, will take
 * all other routes but static OUT of holddown, and delete them.
 */

void
novrip_ager (void)
{
    novndbtype *p, *pred;
    int i;
    novrdbtype *pp, *last_pp, *next_pp;
    int pathcount;
    boolean null_pathlist = FALSE;
    nidbtype *nidb;
    boolean was_on = FALSE;
    boolean rip_cmode_debug;

    if (novell_shutting_down) {
	return;
    }
    /*
     * Screen all auto compatibility mode circuits for activity.
     * Deactivate if stale.
     */
    rip_cmode_debug = ((novellisis_debug || novellnlsp_debug) &&
		       (novellrip_debug || novellrip_event_debug));

    for (nidb = (nidbtype *) novidbQ.qhead; nidb; nidb = nidb->next) {
	if (nidb->lsp_rip_cfg == IPX_RIPSAP_CONFIG_AUTO) {
	    if (ELAPSED_TIME(nidb->lsp_rip_timestamp) >
		nidb->idb->nov_rip_multiplier*
		nidb->idb->nov_update_interval) {
		was_on = nidb->lsp_rip_on;
		novell_stop_nidb_rip_auto(nidb);
		if (rip_cmode_debug && was_on)
		    buginf("\nIPX-NLSP: deactivate RIP on net %x",
			   nidb->novellnet);
	    }
	}
    }
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	p = (*novell_table)[i];
	if (p == NULL)
	    continue;
	pred = p;
	while (p) {
	    /* Route not in holddown, if RIP maybe put it in, or trim paths */
	    if (!TIMER_RUNNING(p->holddownuntil) && (p->flags  & NET_RIP)) {
		pp = p->path_list;
		last_pp = (novrdbtype *) (&p->path_list);

		while (pp) {
		    if (!pp->snapshot_no_age &&
			CLOCK_OUTSIDE_INTERVAL(pp->timer,
				  (pp->nidb->idb->nov_update_interval
				   * pp->nidb->idb->nov_rip_multiplier))) {
			/*
			 * If this is the last path then mark as
			 * unreachable, else remove this path now and
			 * decrement the number of paths available.
			 */
			novcache_increment_version(NULL, 
						   NOVCACHE_RIP_AGER_STALE,
						   p->dstnet, p->rt_mask);
			if (p->paths == 1) {
			    novell_mark_rdb_changed(p, pp, NET_RIP,
						    IPX_ROUTE_EVENT_DELETED);
			    TIMER_START(p->holddownuntil,
					pp->nidb->idb->nov_update_interval);
			    novell_mark_rip_changed(p);
			    break;
			} else {
			    if ((last_pp->next = p->nextpath = pp->next) 
				== NULL)
				p->nextpath = p->path_list;
			    novell_mark_rdb_changed(p, pp, NET_RIP,
						    IPX_ROUTE_EVENT_DELETED);
			    novrdb_free(pp);
			    pp = last_pp;
			    p->paths--;
			}
		    }
		    last_pp = pp;
		    if (pp)
			pp = pp->next;
		}		/* while pp loop */
		/*
		 * Trim down the number of paths if it exceeds the allowed 
		 * value. If the paths need trimming then invalidate the cache.
		 * Note that we trim slower paths from end of list.
		 */
		if (p->paths > novell_maxpaths)
		    novcache_increment_version(NULL, NOVCACHE_RIP_AGER_TRIM,
					       p->dstnet, p->rt_mask);
		    
		while (p->paths > novell_maxpaths) {
		    p->paths--;
		    pp = p->path_list;
		    last_pp = (novrdbtype *) (&p->path_list);
		    while (pp->next) {
			last_pp = pp;
			pp = pp->next;
		    }
		    last_pp->next = NULL;
		    p->nextpath = p->path_list;
		    novell_mark_rdb_changed(p, pp, NET_RIP,
					    IPX_ROUTE_EVENT_DELETED);
		    novrdb_free(pp);
		}
	    } else if (!(p->flags & NET_STATIC) &&
		       TIMER_RUNNING(p->holddownuntil)) {
		/* Route in holddown and not static */
		if (!SLEEPING(p->holddownuntil) &&
		    (p->options & IPX_NDB_OPTION_VALID)) {
		    pathcount = 0;
		    null_pathlist = (p->path_list == NULL);
		    pp = p->path_list;
		    p->path_list = NULL;
		    while (pp != NULL) {
			pathcount++;
			next_pp = pp->next;
			novell_mark_rdb_changed(p, pp, p->flags,
						IPX_ROUTE_EVENT_DELETED);
			novrdb_free(pp);
			pp = next_pp;
		    }
		    /*
		     * In holddown, we should have one path (except NLSP),
		     * or NO paths with a NULL path list. If not, complain.
		     */
		    if ((pathcount != p->paths) ||
			((pathcount != 0) && null_pathlist) ||
			((pathcount > 1) && (!(p->flags & NET_NLSP)))) {
			errmsg(&msgsym(BADPATHS, IPX), p->paths,
			       NOVELL_TYPE, p->dstnet);
		    }
		    if (novellrip_debug || novellrip_event_debug)
			buginf("\nIPXRIP: garbage collecting entry for %x %x",
			       p->dstnet, p->rt_mask);
		    p->options &= ~IPX_NDB_OPTION_VALID;
		    if (p != pred)
			pred->next = p->next;
		    else {
			(*novell_table)[i] = p->next;
			pred = NULL;
		    }
		    novell_routes--;
		    nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
		    p = pred;
		}
	    } else if ((p->flags & NET_STATIC) &&
		       TIMER_RUNNING_AND_AWAKE(p->holddownuntil)) {
		/*
		 * Perform processing specific to STATIC routes. A static
		 * route in holddown will age out in one update interval.
		 * It will exit holddown, become invalid, and may be then
		 * be learned dynamically until and unless the associated
		 * interface becomes active again.
		 */
		p->options &= ~IPX_NDB_OPTION_VALID;
		TIMER_STOP(p->holddownuntil);
	    }
	    /* Did we delete the front of the list, if so, start over... */
	    if (pred == NULL) {
		pred = p = (*novell_table)[i];
	    } else {
		pred = p;
		if (p != NULL) {
		    p = p->next;
		}
	    }
	}
    }
    /* Do the same thing for the patricia tree */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, patricia_novrip_ager);

    /*
     * Periodically deallocate any svc entries that
     * have accumulated on private queue:
     */
    reg_invoke_ipx_nlsp_free_sdb_list();
}

/*
 * nov_same_idb
 *
 * If this IDB matches any idb in the path list then return TRUE.
 * Note that we pay special attention to maxpaths because NLSP route
 * descriptors keep all valid paths output by SPF.
 */

boolean
nov_same_idb (idbtype *idb, novrdbtype *pp)
{
    int pathcount = 0;

    if (idb) {
	for (; pp; pp = pp->next) {
	    if (!validmem(pp) ||
		!validmem(pp->nidb) ||
		!validmem(pp->nidb->idb)) {
		return(FALSE);
	    }
	    if (idb == pp->nidb->idb) {
		return(TRUE);
	    }
	    /*
	     * NLSP does not trim paths ... Do not exceed max usable paths:
	     */
	    if (++pathcount >= novell_maxpaths) {
		break;
	    }
	}
    }
    return(FALSE);
}

/*
 * nov_same_nidb
 *
 * If this (virtual) idb matches any (virtual) idb in the path list
 * then return TRUE. Note that we pay special attention to maxpaths
 * because NLSP route descriptors keep all valid paths output by SPF.
 */

boolean
nov_same_nidb (nidbtype *nidb, novrdbtype *pp)
{
    int pathcount  = 0;

    if (nidb) {
	for (; pp; pp = pp->next) {
	    if (!validmem(pp) || !validmem(pp->nidb)) {
		return(FALSE);
	    }
	    if (nidb == pp->nidb) {
		return(TRUE);
	    }
	    /*
	     * NLSP does not trim paths ... Do not exceed max usable paths:
	     */
	    if (++pathcount >= novell_maxpaths) {
		break;
	    }
	}
    }

    return(FALSE);
}

/*
 * novrip_interface_up
 * If any interface is up return TRUE
 */

boolean
novrip_interface_up (novrdbtype *pp)
{
    for (; pp; pp = pp->next) 
	if (NOVELL_NIDB_IDB_UP(pp->nidb))
	    return (TRUE);

    return (FALSE);
}

/*
 * novrip_poison
 * Poison a RIP derived IPX route.
 */
static void novrip_poison (novndbtype *p, int delay, int hopcount)
{
    backuptype *bi, *binext;
    novpdbtype *loop_pdb;
    
    /*
     * Somebody may have better info for us. Alert the powers that be
     * as appropriate:
     */
    for (bi = p->backup_info; (bi != NULL); bi = binext) {
	binext = bi->next;
	loop_pdb = NOV_INDEX2PDB(bi->pdbindex);
	if ((loop_pdb != NULL) && (loop_pdb->add_route)) {
	    (*loop_pdb->add_route)(loop_pdb, p->dstnet, p->rt_mask,
				   bi->backup_context_info,
				   bi->route_type);
	}
	free(bi);
    }
    p->backup_info = NULL;
    /*
     * Nothing to do except bail if we are already in holddown:
     */
    if (TIMER_RUNNING(p->holddownuntil))
	return;
    /*
     * Slap route into holddown and notify folks as appropriate:
     */
    novcache_increment_version
	(NULL, NOVCACHE_RIP_WORSE_METRIC_LAST, p->dstnet, p->rt_mask);
    p->delay = p->ext_delay = delay;
    p->igrp_delay = NOT_IBM_TICKS(delay);
    p->hopcount = p->ext_hop = hopcount;
    TIMER_START(p->holddownuntil, ONEMIN);
    novell_mark_rip_changed(p);
    if (p->path_list != NULL) {
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: poison route to %x %x via %e"
		   ", delay %d, hops %d",
		   p->dstnet, p->rt_mask,
		   p->path_list->srchost, delay, hopcount);
    }
}

/*
 * novrip_init_backup_route_table
 *
 * Allocate and initialize the backup route table for use in maintaining
 * the isis/nlsp potential pseudonode LSP.
 */
static boolean inject_in_progress;         /* Reentrancy avoidance flag */
static boolean reentry_attempted;

boolean novrip_init_backup_tables (void)
{
    int i;

    inject_in_progress = FALSE;
    reentry_attempted = FALSE;

    if (novell_table2 == NULL) {
	novell_table2 = malloc_named(sizeof(nov_routetable),
				     "IPX backup table");
	if (novell_table2 == NULL) {
	    return(FALSE);
	}
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    (*novell_table2)[i] = NULL;
	}
    }
    if (novell_table3 == NULL) {
	novell_table3 = malloc_named(sizeof(nov_routetable),
				     "IPX service table");
	if (novell_table3 == NULL) {
	    return(FALSE);
	}
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    (*novell_table3)[i] = NULL;
	}
    }
    return(TRUE);
}

/*
 * novrip_age_backup_routes
 */
void novrip_age_backup_routes (void)
{
    novroutetype *p, *last_p, *next_p;
    nov_svc_type *s, *last_s, *next_s;
    int i;
    boolean debug = FALSE;

    /*
     * Dump any pessimal routes ... if they haven't
     * been reinstated yet then they are hopeless.
     */
    p = nov_pessimal_route_list;
    while (p != NULL) {
	next_p = p->next;
	free(p);
	p = next_p;
    }
    nov_pessimal_route_list = NULL;

    s = nov_pessimal_sap_list;
    while (s != NULL) {
	next_s = s->next;
	free(s);
	s = next_s;
    }
    nov_pessimal_sap_list = NULL;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_p = (novroutetype *) &(*novell_table2)[i];
	p = (*novell_table2)[i];
	while (p != NULL) {
	    next_p = p->next;
	    if (ELAPSED_TIME(p->update_timestamp) >
		p->nidb->idb->nov_rip_multiplier*
		p->nidb->idb->nov_update_interval) {
		/* delete current entry ... */
		if (debug)
		    buginf("\nIPXRIP: Delete backup route for net %x, mask %x"
			   " @[M%d/T%d/H%d] via %x.%e",
			   p->dstnet, p->rt_mask, p->metric, p->delay,
			   p->hopcount, p->snet, p->shost);
		last_p->next = p->next;
		free(p);
	    } else {
		last_p = p;
	    }
	    p = next_p;
	}
    }
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_s = (nov_svc_type *) &(*novell_table3)[i];
	s = (*novell_table3)[i];
	while (s != NULL) {
	    next_s = s->next;
	    if (ELAPSED_TIME(s->update_timestamp) >
		s->svc_nidb->idb->nov_sap_multiplier*
		s->svc_nidb->idb->nov_update_interval) {
		/* delete current entry ... */
		if (debug)
		    buginf("\nIPXSAP: Delete backup sap, type %d, '%s'",
			   s->sid.server_name);
		last_s->next = s->next;
		free(s);
	    } else {
		last_s = s;
	    }
	    s = next_s;
	}
    }
}

/*
 * novrip_process_backup_route
 */
void
novrip_process_backup_route (novpdbtype *pdb,
			     nidbtype *nidb,
			     ulong dstnet,
			     ulong netmask,
			     ushort metric,
			     ulong snet,
			     uchar *shost,
			     ushort hopcount,
			     ushort delay,
			     novpdbtype *nlsp_pdb)
{
    novroutetype *p, *last_p, *next_p;
    novroutetype *temp_p = NULL;
    int i;
    boolean same_host, unreachable;
    boolean superior_route_seen = FALSE;
    boolean inferior_route_deleted = FALSE;
    boolean debug = FALSE;

    /*
     * Potential pseudonode (backup route support) may be disabled:
     */
    if (!novell_potential_pseudonode) {
	return;
    }

    if (nlsp_pdb != NULL) {
	debug = (novell_private_debug && NOVNLSP_DEBUG(dstnet, nlsp_pdb));
    }

    /* Exit if source of route is backup table injection */
    if (inject_in_progress) {
	reentry_attempted = TRUE;
	return;
    }
    /* Immediate bail if network 0/-1  */
    /* -2 is treated like a normal route if default handling off */
    if (NOV_ILLEGAL_NET(dstnet)) {
	return;
    }
   /* For now accept only valid RIP routes */
    if ((!pdb->running) || (pdb->proctype != NET_RIP))
	return;

    if (debug)
	buginf("\nIPXRIP: Process backup route for net %x"
	       " @[M%d/T%d/H%d] via %x.%e",
	       dstnet, metric, delay, hopcount, snet, shost);

    unreachable = ((hopcount >= novell_infinity) ||
		   (delay == NOVELL_INFDELAY));

    /* Hash to row for destination network */

    i = NOV_NETHASH(dstnet);
    last_p = (novroutetype *) &(*novell_table2)[i];
    p = (*novell_table2)[i];

    /*
     * Traverse the complete list:
     *
     *   - Delete existing route and exit if poisoned
     *   - Delete if route is more distant but continue scan
     *   - Exit if superior backup route found
     *   - Refresh timer and exit if duplicate, continue scan if equal metric
     *   - Delete inferior routes if new route superior
     *
     * Add new route below if equal or superior
     */
    while (p != NULL) {
	next_p = p->next;
	/*
	 * Skip if not same network:
	 */
	if ((p->dstnet != dstnet) || (netmask != p->rt_mask)) {
	    last_p = p;
	    p = next_p;
	    continue;
	}
	same_host = ((snet == p->snet) && ieee_equal(p->shost, shost));
	/*
	 * Special handling if unreachable. If source is same host
	 * delete this route and exit, otherwise continue search.
	 */
	if (unreachable) {
	    if (same_host) {
		if (debug)
		    buginf("\nIPXRIP: Unreachable, same host:  net %x, mask %x"
			   " @[M%d/T%d/H%d] via %x.%e (delete)",
			   dstnet, netmask, p->metric, p->delay,
			   p->hopcount, p->snet, p->shost);
		last_p->next = next_p;
		free(p);
		return;
	    } else {
		if (debug)
		    buginf("\nIPXRIP: Unreachable, different host:  net %x"
			   " @[M%d/T%d/H%d] via %x.%e (continue)",
			   dstnet, p->metric, p->delay,
			   p->hopcount, p->snet, p->shost);
		last_p = p;
		p = next_p;
		continue;
	    }
	}

	/*
	 * Refresh timer and exit if equal metric, same host.
	 * Otherwise, continue search.
	 */
	if ((delay == p->delay) && (hopcount == p->hopcount)) {
	    if (same_host) {
		GET_TIMESTAMP(p->update_timestamp);
		if (debug) buginf(" (refresh)");
		return;
	    } else {
		last_p = p;
		p = next_p;
		continue;
	    }
	}

	/*
	 * Check if inferior route. If from same host, delete
	 * current entry. After the list is traversed, if
	 * this is the only backup route it will be added back
	 * at its new metric. The pessimal route list will be
	 * checked for equal cost routes recently marked as inferior
	 * (described below). These routes will be reinstated if found.
	 *
	 * If a better route exists from a different host mark this
	 * one as inferior. However, continue the search in order to
	 * remove the same host route if found. Inferior routes will
	 * be placed on the pessimal route list for possible reinstatement
	 * later. (This can happen if several equal cost routes become
	 * more distant in a short period of time.)
	 */
	if ((delay > p->delay) ||
	    ((delay == p->delay) && (hopcount > p->hopcount))) {
	    if (same_host) {
		if (debug)
		    buginf("\nIPXRIP: Inferior backup route for net %x"
			   " @[M%d/T%d/H%d] via %x.%e",
			   dstnet, p->metric, p->delay,
			   p->hopcount, p->snet, p->shost);
		temp_p = p;
		last_p->next = p = next_p;
		inferior_route_deleted = TRUE;
		temp_p->next = NULL;
		temp_p->pdb = pdb;
		temp_p->nidb = nidb;
		temp_p->metric = metric;
		temp_p->hopcount = hopcount;
		temp_p->delay = delay;
		GET_TIMESTAMP(temp_p->update_timestamp);
		if (superior_route_seen) {
		    /*
		     * Place known inferior route on pessimal list and exit:
		     */
		    temp_p->next = nov_pessimal_route_list;
		    nov_pessimal_route_list = temp_p;
		    if (debug) buginf(" (->pessimal1)");
		    return;
		} else {
		    /*
		     * Continue search for superior route. Note that
		     * temp_p now points to the inferior route. The route
		     * has been deleted from the backup table, and
		     * pointers updated accordingly.
		     */
		    continue;
		}
	    } else {
		superior_route_seen = TRUE;
		if (inferior_route_deleted) {
		    temp_p->next = nov_pessimal_route_list;
		    nov_pessimal_route_list = temp_p;
		    if (debug) buginf(" (->pessimal2)");
		    return;
		} else {
		    last_p = p;
		    p = next_p;
		    continue;
		}
	    }
	}

	/* Delete current route(s) if new route superior */
	if ((delay < p->delay) ||
	    ((delay == p->delay) && (hopcount < p->hopcount))) {
	    /* Better route pending ... delete current entry */
	    if (debug)
		buginf("\nIPXRIP: Delete backup route for net %x"
		       " @[M%d/T%d/H%d] via %x.%e",
		       dstnet, p->metric, p->delay,
		       p->hopcount, p->snet, p->shost);
	    free(p);
	    last_p->next = p = next_p;
	    continue;
	}
	last_p = p;
	p = next_p;
    }

    /*
     * Bail if poison or if this is simply an inferior route:
     */
    if (unreachable || superior_route_seen) {
	if (debug && superior_route_seen)
	    buginf(" (inferior)");
	return;
    }
    /*
     * New or adjusted entry:
     */
    if (temp_p != NULL) {
	/*
	 * Adjusted entry ... search pessimal list for similar routes
	 * recently declared pessimal:
	 */
	last_p = (novroutetype *)&nov_pessimal_route_list;
	p = nov_pessimal_route_list;

	while (p != NULL) {

	    next_p = p->next;

	    if ((dstnet == p->dstnet) &&
		(netmask == p->rt_mask) &&
		(delay == p->delay) &&
		(hopcount == p->hopcount)) {
		if (debug)
		    buginf("\nIPXRIP: Reinstate pessimal backup route"
			   " for net %x mask %x @[M%d/T%d/H%d] via %x.%e",
			   dstnet, netmask, metric, delay, hopcount, p->snet, 
			   p->shost);
		last_p->next = next_p;
		p->next = (*novell_table2)[i];
		(*novell_table2)[i] = p;
		p = next_p;
	    } else {
		last_p = p;
		p = next_p;
	    }
	}
	p = temp_p;
    } else {
	/*
	 * New entry ... allocate and build a descriptor from scratch:
	 */
	p = malloc_named(sizeof(novroutetype), "IPX RIP route");
    }
    if (p == NULL) {
	return;
    }
    if (debug) {
	buginf("\nIPXRIP: Create backup route for net %x mask %x"
	       " @[M%d/T%d/H%d] via %x.%e",
	       dstnet, netmask, metric, delay, hopcount, snet, shost);
	if (temp_p == p) {
	    buginf(" (reuse entry)");
	} else {
	    buginf(" (malloc entry)");
	}
    }
    p->pdb = pdb;
    p->nidb = nidb;
    p->dstnet = dstnet;
    p->rt_mask = netmask;
    p->metric = metric;
    p->delay = delay;
    p->hopcount = hopcount;
    p->snet = snet;
    ieee_copy(shost, p->shost);
    GET_TIMESTAMP(p->update_timestamp);
    p->next = (*novell_table2)[i];
    (*novell_table2)[i] = p;
    return;
}

/*
 * novrip_process_backup_service
 *
 * Store the best services advertised via SAP for use by the NLSP
 * potential pseudonode.
 *
 */
void
novrip_process_backup_service (novpdbtype *pdb,
			       nidbtype *nidb,
			       novell_serveridtype *sid,
			       ulong snet,
			       uchar *shost,
			       ulong flags)
{
    nov_svc_type *svc, *last_svc, *next_svc;
    nov_svc_type *temp_svc = NULL;
    int i;
    boolean same_host, unreachable;
    boolean superior_route_seen = FALSE;
    boolean inferior_route_deleted = FALSE;
    ushort type = GETSHORT(&sid->server_type);
    uchar *name = sid->server_name;
    ulong dnet = GETLONG(&sid->dnet);
    uchar *dhost = sid->dhost;
    ushort dsock = GETSHORT(&sid->dsock);
    ushort hops = GETSHORT(&sid->nethops);

    boolean debug = FALSE;

    /*
     * Potential pseudonode (backup route support) may be disabled:
     */
    if (!novell_potential_pseudonode) {
	return;
    }

    /* Exit if source of service is backup table injection */
    if (inject_in_progress) {
	reentry_attempted = TRUE;
	return;
    }
    /* Immediate bail if network 0/-1/-2 */
    if ((dnet < NOV_MIN_NET) || (dnet > NOV_MAX_NET)) {
	return;
    }
   /* Accept only valid external SAP services */
    if ((!pdb->running) || (pdb->proctype != NET_RIP)) {
	return;
    }
    if (debug) {
	buginf("\nIPXSAP: Process backup service %x:%s:%x.%e:%x @ [H%d]",
	       type, name, dnet, dhost, dsock, hops);
    }

    unreachable = (hops >= novell_infinity);

    /* Hash to row for destination network */

    i = NOV_NETHASH(dnet);
    last_svc = (nov_svc_type *) &(*novell_table3)[i];
    svc = (*novell_table3)[i];

    /*
     * Traverse the complete list:
     *
     *   - Delete existing sap and exit if poisoned
     *   - Delete if sap is more distant but continue scan
     *   - Exit if superior backup sap found
     *   - Refresh timer and exit if duplicate, continue scan if equal metric
     *   - Delete inferior saps if new sap superior
     *
     * Add new sap below if equal or superior
     */
    while (svc != NULL) {
	next_svc = svc->next;
	/*
	 * Skip if not same service:
	 */
	if ((svc->svc_type != type) || strcmp(svc->sid.server_name, name)) {
	    last_svc = svc;
	    svc = next_svc;
	    continue;
	}
	same_host = ((nidb == svc->svc_nidb) &&
		     (snet == svc->svc_snet) &&
		     ieee_equal(shost, (uchar*)&svc->svc_shost) &&
		     (flags = svc->svc_flags));
	/*
	 * Special handling if unreachable. Delete if same service
	 * heard on same network. %%% TEMPORARY %%%
	 */
	if (unreachable) {
	    if (same_host) {
		if (debug)
		    buginf("\nIPXSAP: Unreachable via %s, net %x"
			   " (same nidb ... delete)",
			   nidb->idb->short_namestring, nidb->novellnet);
		last_svc->next = next_svc;
		free(svc);
		return;
	    } else {
		if (debug)
		    buginf("\nIPXSAP: Unreachable via %s, net %x"
			   " (different nidb ... continue)",
			   nidb->idb->short_namestring, nidb->novellnet);
		last_svc = svc;
		svc = next_svc;
		continue;
	    }
	}

	/*
	 * Refresh timer and exit if equal metric, same host.
	 * Otherwise, continue search.
	 */

	if (hops == svc->svc_hops) {
	    if (same_host) {
		GET_TIMESTAMP(svc->update_timestamp);
		if (debug) buginf(" (refresh)");
		return;
	    } else {
		last_svc = svc;
		svc = next_svc;
		continue;
	    }
	}

	/*
	 * Check if inferior route. If from same host, delete
	 * current entry. After the list is traversed, if
	 * this is the only backup route it will be added back
	 * at its new metric. The pessimal route list will be
	 * checked for equal cost routes recently marked as inferior
	 * (described below). These routes will be reinstated if found.
	 *
	 * If a better route exists from a different host mark this
	 * one as inferior. However, continue the search in order to
	 * remove the same host route if found. Inferior routes will
	 * be placed on the pessimal route list for possible reinstatement
	 * later. (This can happen if several equal cost routes become
	 * more distant in a short period of time.)
	 */
	if (hops > svc->svc_hops) {
	    if (same_host) {
		if (debug)
		    buginf("\nIPXSAP: Inferior backup advertisement"
			   " via %s, net %x",
			   nidb->idb->short_namestring, nidb->novellnet);
		temp_svc = svc;
		last_svc->next = svc = next_svc;
		inferior_route_deleted = TRUE;
		temp_svc->next = NULL;
		temp_svc->pdb = pdb;
		temp_svc->svc_nidb = nidb;
		temp_svc->svc_hops = hops;
		GET_TIMESTAMP(temp_svc->update_timestamp);
		if (superior_route_seen) {
		    /*
		     * Place known inferior route on pessimal list and exit:
		     */
		    temp_svc->next = nov_pessimal_sap_list;
		    nov_pessimal_sap_list = temp_svc;
		    if (debug) buginf(" (->pessimal1)");
		    return;
		} else {
		    /*
		     * Continue search for superior route. Note that
		     * temp_p now points to the inferior route. The route
		     * has been deleted from the backup table, and
		     * pointers updated accordingly.
		     */
		    continue;
		}
	    } else {
		superior_route_seen = TRUE;
		if (inferior_route_deleted) {
		    temp_svc->next = nov_pessimal_sap_list;
		    nov_pessimal_sap_list = temp_svc;
		    if (debug) buginf(" (->pessimal2)");
		    return;
		} else {
		    last_svc = svc;
		    svc = next_svc;
		    continue;
		}
	    }
	}

	/* Delete current route(s) if new route superior */
	if (hops < svc->svc_hops) {
	    /* Better route pending ... delete current entry */
	    if (debug)
		buginf("\nIPXSAP: Better advertisement via %s, net %x"
		       ", delete service learned via %s, net %x",
		       nidb->idb->short_namestring, nidb->novellnet,
		       svc->svc_nidb->idb->short_namestring,
		       svc->svc_nidb->novellnet);
	    free(svc);
	    last_svc->next = svc = next_svc;
	    continue;
	}
	last_svc = svc;
	svc = next_svc;
    }

    /*
     * Bail if poison or if this is simply an inferior route:
     */
    if (unreachable || superior_route_seen) {
	if (debug && superior_route_seen)
	    buginf(" (inferior)");
	return;
    }
    /*
     * New or adjusted entry:
     */
    if (temp_svc != NULL) {
	/*
	 * Adjusted entry ... search pessimal list for similar routes
	 * recently declared pessimal:
	 */
	last_svc = (nov_svc_type *)&nov_pessimal_sap_list;
	svc = nov_pessimal_sap_list;

	while (svc != NULL) {

	    next_svc = svc->next;

	    if ((dnet == GETLONG(&svc->sid.dnet)) &&
		(hops == svc->svc_hops)) {
		if (debug)
		    buginf("\nIPXSAP: Reinstate pessimal backup service"
			   " via %s, net %x",
			   nidb->idb->short_namestring, nidb->novellnet);
		last_svc->next = next_svc;
		svc->next = (*novell_table3)[i];
		(*novell_table3)[i] = svc;
		svc = next_svc;
	    } else {
		last_svc = svc;
		svc = next_svc;
	    }
	}
	svc = temp_svc;
    } else {
	/*
	 * New entry ... allocate and build a descriptor from scratch:
	 */
	svc = malloc_named(sizeof(nov_svc_type), "IPX SAP service");
    }
    if (svc == NULL) {
	return;
    }
    svc->pdb = pdb;
    svc->svc_type = type;
    svc->svc_nidb = nidb;
    svc->svc_sock = dsock;
    svc->svc_snet = snet;
    ieee_copy(shost, svc->svc_shost);
    svc->svc_hops = hops;
    svc->svc_flags = flags;

    bcopy(sid, &svc->sid, sizeof(novell_serveridtype));

    if (debug) {
	buginf("\nIPXSAP: Create backup service for %s via %x.%e (%s)",
	       svc->sid.server_name,
	       svc->svc_snet, svc->svc_shost,
	       svc->svc_nidb->idb->short_namestring);
	if (temp_svc == svc) {
	    buginf(" (reuse entry)");
	} else {
	    buginf(" (malloc entry)");
	}
    }
    GET_TIMESTAMP(svc->update_timestamp);
    svc->next = (*novell_table3)[i];
    (*novell_table3)[i] = svc;
    return;
}

/*
 * novrip_delete_backup_routes
 */
void
novrip_delete_all_backup_data (ulong dstnet, ulong netmask)
{
    novroutetype *p, *last_p, *next_p;
    nov_svc_type *s, *last_s, *next_s;
    int i;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
	if ((dstnet != 0) && (i != NOV_NETHASH(dstnet))) {
	    continue;
	}
	last_p = (novroutetype *) &(*novell_table2)[i];
	p = (*novell_table2)[i];
	for (; p; p = next_p) {
	    next_p = p->next;
	    if ((dstnet == 0) || ((p->dstnet == dstnet) && 
				  (p->rt_mask == netmask))) {
		/* delete current entry ... */
		last_p->next = p->next;
		free(p);
	    }
	}
	if (dstnet != 0) {
	    continue;
	}
	last_s = (nov_svc_type *) &(*novell_table3)[i];
	s = (*novell_table3)[i];
	for (; s; s = next_s) {
	    next_s = s->next;
	    /* delete current entry ... */
	    last_s->next = s->next;
	    free(s);
	}
	process_may_suspend();
    }
}

/*
 * novrip_inject_all_backup_routes
 */
void novrip_inject_all_backup_routes (boolean suspend_requested, 
				      novpdbtype *nlsp_pdb)
{
    novroutetype *p, *next_p, *last_p;
    int i;
    boolean debug = FALSE;
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_p = (novroutetype *) &(*novell_table2)[i];
	p = (*novell_table2)[i];
	while (p != NULL) {
	    next_p = p->next;

	    if (nlsp_pdb != NULL) {
		debug = (novell_private_debug &&
			 NOVNLSP_DEBUG(p->dstnet, nlsp_pdb));
	    }
	    if (debug)
		buginf("\n Injecting backup RIP route for net %x mask %x"
		       " @[M%d/T%d/H%d] via %x.%e",
		       p->dstnet, p->rt_mask, p->metric, p->delay,
		       p->hopcount, p->snet, p->shost);

	    inject_in_progress = TRUE;
	    reentry_attempted = FALSE;

	    novrip_update(p->nidb,
			  p->dstnet,
			  p->rt_mask,
			  p->metric,
			  p->snet,
			  &p->shost[0],
			  p->hopcount,
			  p->delay, NULL);

	    /*
	     * If novrip_update() attempted to install the backup route
	     * then the original XROUTE is still present and continues
	     * to be preferred over the backup route. In this case, keep
	     * it. If not, then the backup route must have been installed
	     * in the route table, or the XROUTE is no longer present
	     * and the backup route is uninteresting. In this case, remove
	     * it from the backup tables in order to avoid confusion.
	     */
	   
	    if (reentry_attempted) {
		if (debug) buginf(" (rejected)");
		last_p = p;
		p = next_p;
	    } else {
		if (debug) buginf(" (accepted)");
		last_p->next = next_p;
		free(p);
		p = next_p;
	    }
	    inject_in_progress = FALSE;
	}
	if (suspend_requested) {
	    process_may_suspend();
	}
    }
}

/*
 * novrip_inject_one_backup_route
 */
boolean novrip_inject_one_backup_route (ulong net, ulong netmask)
{
    novroutetype *p, *next_p, *last_p;
    int i;
    boolean hit = FALSE;
    boolean debug = FALSE;

    if (debug)
	buginf("\nIPXRIP: Searching for backup route to net %x", net);

    i = NOV_NETHASH(net);
    last_p = (novroutetype *) &(*novell_table2)[i];
    p = (*novell_table2)[i];
    while (p != NULL) {
	next_p = p->next;
	if ((p->dstnet == net) && (p->rt_mask == netmask)) {
	    hit = TRUE;
	    last_p->next = next_p;

	    if (debug)
		buginf("\nIPXRIP: Inject net %x mask %x backup route"
		       " via %x.%e @ [%d/%d]",
		       net, netmask, p->snet, p->shost, p->delay, p->hopcount);

	    inject_in_progress = TRUE;
	    reentry_attempted = FALSE;

	    novrip_update(p->nidb,
			  p->dstnet,
			  p->rt_mask,
			  p->metric,
			  p->snet,
			  &p->shost[0],
			  p->hopcount,
			  p->delay, NULL);

	    /*
	     * If novrip_update() attempted to install the backup route
	     * then the original XROUTE is still present and continues
	     * to be preferred over the backup route. In this case, keep
	     * it. If not, then the backup route must have been installed
	     * in the route table, or the XROUTE is no longer present
	     * and the backup route is uninteresting. In this case, remove
	     * it from the backup tables in order to avoid confusion.
	     */
	    if (reentry_attempted) {
		if (debug) buginf(" (rejected)");
		last_p = p;
	    } else {
		if (debug) buginf(" (accepted)");
		last_p->next = next_p;
		free(p);
	    }
	    inject_in_progress = FALSE;
	} else {
	    last_p = p;
	}
	p = next_p;
    }
    return(hit);
}

/*
 * novrip_inject_all_backup_services
 */
void novrip_inject_all_backup_services (boolean suspend_requested, 
					novpdbtype *nlsp_pdb)
{
    nov_svc_type *svc, *last_svc, *next_svc;
    int i;
    boolean debug = FALSE;
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_svc = (nov_svc_type *) &(*novell_table3)[i];
	svc = (*novell_table3)[i];
	while (svc != NULL) {
	    next_svc = svc->next;

	    if (nlsp_pdb != NULL) {
		debug = (novell_private_debug &&
			 NOVNLSP_DEBUG(GETLONG(&svc->sid.dnet), nlsp_pdb));
	    }

	    if (debug)
		buginf("\n Injecting backup service %x:%s:%x.%e:%x"
		       "via %x.%e @[H%d]",
		       svc->svc_type, svc->sid.server_name,
		       GETLONG(&svc->sid.dnet), svc->sid.dhost,
		       svc->svc_sock, svc->svc_snet, svc->svc_shost,
		       svc->svc_hops);

	    inject_in_progress = TRUE;
	    reentry_attempted = FALSE;

	    novell_addserver(svc->pdb,
			     &svc->sid,
			     svc->svc_nidb,
			     &svc->svc_shost[0],
			     svc->svc_snet,
			     svc->svc_flags,
			     svc->svc_hops,
			     NULL, NULL, 0, 0, FALSE);
	    /*
	     * If novrip_update() attempted to install the backup route
	     * then the original XROUTE is still present and continues
	     * to be preferred over the backup route. In this case, keep
	     * it. If not, then the backup route must have been installed
	     * in the route table, or the XROUTE is no longer present
	     * and the backup route is uninteresting. In this case, remove
	     * it from the backup tables in order to avoid confusion.
	     */
	   
	    if (reentry_attempted) {
		if (debug) buginf(" (rejected)");
		last_svc = svc;
		svc = next_svc;
	    } else {
		if (debug) buginf(" (accepted)");
		last_svc->next = next_svc;
		free(svc);
		svc = next_svc;
	    }
	    inject_in_progress = FALSE;
	}
	if (suspend_requested) {
	    process_may_suspend();
	}
    }
}

/*
 * novrip_display_backup_routes
 */
static void novrip_display_backup_services(ulong net);

void novrip_display_backup_routes (ulong net, ulong netmask)
{
    novroutetype *p, *last_p;
    int i, ii, hash, count = 0;
    boolean invalid_list = FALSE;
    ptable *rp;

    if (netmask == 0)
	netmask = NOV_NETMASK_ALL;
    printf("\n\nBackup route table:");
    /*
     * Run through the table once counting and locking everything:
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_p = (novroutetype *) &(*novell_table2)[i];
	for (p = (*novell_table2)[i]; p; p = p->next) {
	    if (validmem(p)) {
		last_p = p;
		if ((net != 0) && ((net != p->dstnet) ||
				   (netmask != p->rt_mask))) 
		    continue;
		inline_mem_lock(p);
		count++;
	    } else {
		invalid_list = TRUE;
		last_p->next = NULL; /* Fix it up as best we can */
		break;
	    }
	}
    }
    /*
     * Allocate an array of pointers to hold everything. If no memory,
     * bail ... but first run through the table again in order to unlock
     * (sigh)!
     */
    rp = malloc((count+1)*sizeof(uchar *));
    if (rp == NULL) {
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    for (p = (*novell_table2)[i]; p; p = p->next) {
		if (validmem(p)) {
		    if ((net != 0) && ((net != p->dstnet) ||
				       (netmask != p->rt_mask)))
			continue;
		    free(p);
		}
	    }
	}
	return;
    }
    /*
     * Fill in the array:
     */
    for (i = 0, ii = 0; i < NOV_NETHASHLEN; i++) {
	for (p = (*novell_table2)[i]; p; p = p->next) {
	    if (validmem(p)) {
		if ((net != 0) && ((net != p->dstnet) ||
				   (netmask != p->rt_mask)))
		    continue;
		*rp[ii++] = p;
	    }
	}
    }
    /*
     * It is now safe to suspend:
     */
    process_may_suspend();

    printf(" (%d entries)", count);
    if (invalid_list) printf(" *Bad*");

    /*
     * Do the display at our leisure:
     */
    automore_enable(NULL);

    for (i = 0; i < count; i++) {

	if (automore_quit())
	    break;

	p = *rp[i];
	hash = NOV_NETHASH(p->dstnet);

	printf("\n %-3d: Net %-08x mask %-08x, via %-08x.%e (%s) @[%d/%d], %ld sec",
	       hash, p->dstnet, p->rt_mask, p->snet, p->shost,
	       p->nidb->idb->short_namestring, p->delay, p->hopcount,
	       ELAPSED_TIME(p->update_timestamp)/ONESEC);

	if (automore_quit())
	    break;
    }

    automore_disable();

    /*
     * Step through the list of locked entries and free everything:
     */
    for (i = 0; i < count; i++) {
	free(*rp[i]);
    }
    free(rp);

    novrip_display_backup_services(net);
}

/*
 * novrip_display_backup_services
 */
static void novrip_display_backup_services (ulong net)
{
    nov_svc_type *p, *last_p;
    int i, ii, hash, count = 0;
    boolean invalid_list = FALSE;
    ptable *rp;

    printf("\n\nBackup service table:");
    /*
     * Run through the table once counting and locking everything:
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_p = (nov_svc_type *) &(*novell_table3)[i];
	for (p = (*novell_table3)[i]; p; p = p->next) {
	    if (validmem(p)) {
		last_p = p;
		if ((net != 0) && (net != GETLONG(&p->sid.dnet))) continue;
		inline_mem_lock(p);
		count++;
	    } else {
		invalid_list = TRUE;
		last_p->next = NULL; /* Fix it up as best we can */
		break;
	    }
	}
    }
    /*
     * Allocate an array of pointers to hold everything. If no memory,
     * bail ... but first run through the table again in order to unlock
     * (sigh)!
     */
    rp = malloc((count+1)*sizeof(uchar *));
    if (rp == NULL) {
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    for (p = (*novell_table3)[i]; p; p = p->next) {
		if (validmem(p)) {
		    if ((net != 0) && (net != GETLONG(&p->sid.dnet)))
			continue;
		    free(p);
		}
	    }
	}
	return;
    }
    /*
     * Fill in the array:
     */
    for (i = 0, ii = 0; i < NOV_NETHASHLEN; i++) {
	for (p = (*novell_table3)[i]; p; p = p->next) {
	    if (validmem(p)) {
		if ((net != 0) && (net != GETLONG(&p->sid.dnet))) continue;
		*rp[ii++] = p;
	    }
	}
    }
    /*
     * It is now safe to suspend:
     */
    process_may_suspend();

    printf(" (%d entries)", count);
    if (invalid_list) printf(" *Bad*");

    /*
     * Do the display at our leisure:
     */
    automore_enable(NULL);

    for (i = 0; i < count; i++) {

	if (automore_quit())
	    break;

	p = *rp[i];
	hash = NOV_NETHASH(GETLONG(&p->sid.dnet));

	printf("\n %-3d: %x:%s:%x.%e:%x via %x.%e (%s) @[H%d], %ld sec",
	       hash, p->svc_type, p->sid.server_name, GETLONG(&p->sid.dnet),
	       p->sid.dhost, p->svc_sock, p->svc_snet, p->svc_shost,
	       p->svc_nidb->idb->short_namestring, p->svc_hops,
	       ELAPSED_TIME(p->update_timestamp)/ONESEC);

	if (automore_quit())
	    break;
    }

    automore_disable();

    /*
     * Step through the list of locked entries and free everything:
     */
    for (i = 0; i < count; i++) {
	free(*rp[i]);
    }
    free(rp);
}

/*
 * novrip_update
 * Perform updating of the RIP routing tables
 */

void novrip_update (nidbtype *nidb,
		    ulong dstnet,
		    ulong netmask,
		    ushort metric,
		    ulong snet,
		    uchar *shost,
		    ushort hopcount,
		    ushort delay,
		    backuptype *backup_info)
{
    novndbtype *p;
    int i;
    novrdbtype *pp, *last_pp, *temp_pp;
    novrdbtype *slower_last_pp, *slower_pp, *prev_last_pp = NULL;
    novpdbtype *pdb;
    nov_sumroutetype *rnnode;
    backuptype *old_backup_info = NULL;

    if (!novell_rip_running) {
	return;
    }

    /* Bump up parameters to true infinity... */
    if (hopcount >= NOVELL_RIP_ADV_INFINITY) {
	hopcount = NOVELL_MAX_HOPS_ALLOWED;
	delay = NOVELL_INFDELAY;
    }

    /* Immediate bail if network 0/-1 */
    /* -2 is treated like a normal route if default handling off */
    if (NOV_ILLEGAL_NET(dstnet)) {
	return;
    }
    p = novrip_find_rt(dstnet, netmask, IPX_ROUTE_MATCH_EXACT);
    /*
     * Not found, enter it.
     */
    if (p == NULL) {
        if (hopcount >= novell_infinity)
            return;
	/*
	 * Allow a few seconds of non-reliable back-wash. This is a minimum
	 * time to wait and 'forces' a very short holddown period.
	 */
	if (nov_nonreliable_ndb_on_free_queue(dstnet, netmask)) {
	    return;
	}

	p = nov_ndb_malloc();
	if (p == NULL) 
	    return;
	pp = malloc_named(sizeof(novrdbtype), "IPX RIP RDB");
	if (pp == NULL) {
	    nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	    return;
	}

	p->rt_mask = netmask;
	p->keylen = p->masklen = IPXRADIXKEYLEN;
	p->path_list = p->nextpath = pp;
	p->dstnet = dstnet;
	TIMER_STOP(p->holddownuntil);
 	p->reverse_poisoned = FALSE;
	p->hopcount = p->last_hopcount = p->ext_hop = hopcount;
	p->delay = p->ext_delay = p->last_delay = delay;
	p->igrp_delay = NOT_IBM_TICKS(delay);
	p->options = IPX_NDB_OPTION_VALID;
	p->novpdbindex = novrip_pdb.index;
	pp->nidb = nidb;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	novell_set_timer(pp, NULL);
	p->paths++;
	p->procmask = novrip_pdb.mask;
	pp->procmask = novrip_pdb.mask;
	p->flags = NET_RIP;		/* used as dynamic */
	/*
	 * Add the route to the hash table if netmask is -1.
	 */
	if (netmask == NOV_NETMASK_ALL) {
	    i = NOV_NETHASH(dstnet);
	    p->next = (*novell_table)[i];
	    (*novell_table)[i] = p;
	} else {
	    /*
	     * Add the route to the patricia tree if netmask is not -1.
	     */
	    rnnode = malloc(sizeof(nov_sumroutetype));
	    if (!rnnode) {
		free(pp);
		nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		return;
	    }
	    rnnode->ndb = p;
	    if (!rn_addroute(&p->keylen, &p->masklen, nov_rnhead, 
			    rnnode->rt_nodes)) {
		buginf("\nIPXRIP: novrip_update did not add %x %x properly",
		       p->dstnet, p->rt_mask);
	    }
	    p->flags |= NET_SUMMARY;
	}
	if (novellrip_debug)
	    buginf("\nIPXRIP: create route to %x %x via %e, delay %d, hops %d",
		   dstnet, netmask, shost, delay, hopcount);
	/*
	 * Invalidate current cache entries for new net if learned via
	 * default route.
	 */
	novcache_increment_version(NULL, NOVCACHE_RIP_NEW_ROUTE, p->dstnet, 
				   p->rt_mask);
	novell_routes++;
	novell_mark_rip_changed(p);
	if (backup_info != NULL) {
	    p->backup_info = backup_info;
	}
	novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_ADD);
	return;

	/* Don't override static/connected/internal routes, ever. */
	
    } else if (p->flags & (NET_STATIC | NET_CONNECTED | NET_INTERNAL) &&
	       !(p->flags & NET_FLOATING_STATIC)) {
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: %x %x not added, entry in table is"
		   " static/connected/internal", dstnet, netmask);
	goto check_for_lost_route;
    } else if (p->flags & (NET_ISIS | NET_NLSP)) {
	/*
	 * Special case processing for ISIS/NLSP XROUTES. If the incoming
	 * RIP route is the best (or tied for best) entry route in the local
	 * area we will install it in preference to the current XROUTE.
	 *
	 * Note that only XROUTEs are considered here. A RIP route will
	 * NEVER override a pure NLSP route. However, we may wish to
	 * store the RIP route in the backup table.
	 *
	 * This determination must be made by ISIS/NLSP via a callback.
	 * If the callback returns TRUE we will delete the current XROUTE,
	 * create a new RIP route, and perform redistribution.
	 *
	 * NOTE: Run the callback on pure NLSP routes since they may
	 *       be invalid and desire replacement.
	 */
	pdb = NULL;

	pdb = nov_proctable[p->novpdbindex];

	if ((pdb == NULL) || !validmem(pdb)) {
	    return;
	}
	
	if (pdb->xroute_callback != NULL)
	    if ((p->flags & NET_NLSP) &&
		(*pdb->xroute_callback)(pdb, p, delay, hopcount)) {

		/*
		 * Prefer the new RIP route. Delete and call recursively.
		 * This is a little wierd but it saves us a LOT of work and
		 * is hopefully the most reliable method in the end.
		 */

		/*
		 * Unlink ndb, deallocate all rdb's, then free ndb:
		 */
		if (novellrip_debug)
		    buginf("\nIPXRIP: delete XROUTE to %x %x", dstnet, 
			   netmask);

		novcache_increment_version
		    (NULL, NOVCACHE_RIP_BETTER_XROUTE, p->dstnet, p->rt_mask);

		p->options &= ~IPX_NDB_OPTION_VALID;
		novell_routes--;
		/*
		 * Absorb current route info into backup structure:
		 */
		ipx_ndb_self_backup(p);
		old_backup_info = p->backup_info;
		p->backup_info = NULL;
		novrip_del_rt(p->dstnet, p->rt_mask, 
			      IPX_ROUTE_DELETE_RETAIN_CONNECTED);
		
		novrip_update(nidb,
			      dstnet,
			      netmask,
			      metric,
			      snet,
			      shost,
			      hopcount,
			      delay,
			      old_backup_info);
		return;
	    } else {
		/*
		 * We continue to prefer the current XROUTE (or NLSP route).
		 * However, if this is the best RIP route known to us
		 * locally, save it in the backup route table. (This
		 * table represents part of our "potential pseudonode".)
		 *
		 * Note that this could be a poison or change to a route
		 * currently available in the backup table. The callee
		 * will handle this case as well. Finally, if it really
		 * is a poison, we will want to inform the source that
		 * we have a valid route via lost route.
		 */
		if (pdb->backup_route_callback != NULL)
		    (*pdb->backup_route_callback)(&novrip_pdb,
						  nidb,
						  dstnet,
						  netmask,
						  metric,
						  snet,
						  shost,
						  hopcount,
						  delay,
						  pdb);
		return;
		goto check_for_lost_route;
	    }
    } else if ((p->flags & NET_IGRP) && ((hopcount >= p->ext_hop) ||
					 TIMER_RUNNING(p->holddownuntil))) {
	/*
	 * Prefer EIGRP if the EIGRP external hop count is less than
	 * or equal to the the incomming RIP hop count.
	 */
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: %x %x not added, EIGRP ext hop count %d"
		   ", RIP hop count in update %d",
		   dstnet, netmask, p->ext_hop, hopcount);
	goto check_for_lost_route;
    } else if ((hopcount < novell_infinity) &&
	       (p->flags & NET_RIP) &&
	       NOVELL_EQUAL_ROUTE_INFO(hopcount, delay, p)) {
	/* Equal cost only when RIP */
	/*
	 * Equal cost, add multiple path. Also, update hopcount, delay and net
	 * type fields, since they could theoretically fluctuate independent
	 * of the cost. This means that in a UB environment the hopcount could
	 * theoretically oscillate with each update from a UB host that had
	 * two paths with the same delay but different hopcounts. Tough.
	 * A cost equal to or less than the cost we had when we put a
	 * route into holddown will resurrect it. Also, if we only have a
	 * single path to a destination, and we haven't heard from the next
	 * hop for a while, we'll let the new path overwrite the old, on
	 * the theory that the old may have gone away or gotten flaky.
	 */
	if (TIMER_RUNNING(p->holddownuntil)) {
	    TIMER_STOP(p->holddownuntil);
	    p->reverse_poisoned = FALSE;
	    novell_mark_rip_changed(p);
	}
	p->hopcount = p->ext_hop = p->last_hopcount = hopcount;
	p->delay = p->ext_delay = p->last_delay = delay;
	/* keep best delay of all paths to this dest in igrp_delay */
	if (NOT_IBM_TICKS(delay) < p->igrp_delay)
	    p->igrp_delay = NOT_IBM_TICKS(delay);
	prev_last_pp = slower_last_pp = slower_pp = NULL;
	last_pp = (novrdbtype *) (&p->path_list);
	pp = p->path_list;
	while (pp) {
	    /* One host, one vote. Same host, same nidb...  */
	    if ((pp->nidb == nidb) &&
		(pp->srcnet == snet) && ieee_equal(pp->srchost, shost) &&
		(pp->procmask == novrip_pdb.mask)) {
		novell_set_timer(pp, NULL);
		return;
	    } else if (novell_maxpaths == 1) {
		if (!pp->snapshot_no_age &&
		    CLOCK_OUTSIDE_INTERVAL(pp->timer,
			       (RIP_STALETIME *
				pp->nidb->idb->nov_update_interval))) {
		    /*
		     * We're only allowed one path, and we haven't heard from
		     * the one we've been using in a while. Change to a new
		     * one. Reasonably oscillation-safe if RIP_STALETIME is
		     * large enough...
		     */
		    if (novellrip_debug || novellrip_event_debug )
			buginf("\nIPXRIP: net %x %x path via %x.%e (%s) is stale; "
			       "now using %x.%e (%s)",  
			       p->dstnet, p->rt_mask, pp->srcnet, pp->srchost,
			       pp->nidb->idb->namestring,
			       snet, shost, nidb->idb->namestring);
		    pp->srcnet = snet;
		    ieee_copy(shost, pp->srchost);
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_DELETED);
		    pp->nidb = nidb;
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_ADD);
		    novell_set_timer(pp, NULL);
		    novcache_increment_version(NULL, 
					       NOVCACHE_RIP_EQ_PATH_STALE,
					       p->dstnet, p->rt_mask);
		    novell_mark_rip_changed(p);
		} else if (NOVELL_FASTER_NIDB(nidb, pp->nidb)) {
		    /* We're only allowed one path ... if this local
		     * interface is faster, use it in preference
		     * to the current path.
		     */
		    if (novellrip_debug || novellrip_event_debug )
			buginf("\nIPXRIP: net %x %x path via %x.%e (%s) is slower; "
			       "now using %x.%e (%s)",  
			       p->dstnet, p->rt_mask, pp->srcnet, pp->srchost,
			       pp->nidb->idb->namestring,
			       snet, shost, nidb->idb->namestring);
		    pp->srcnet = snet;
		    ieee_copy(shost, pp->srchost);
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_DELETED);
		    pp->nidb = nidb;
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_ADD);
		    novell_set_timer(pp, NULL);
		    novell_mark_rip_changed(p);
		    novcache_increment_version(NULL, 
					       NOVCACHE_RIP_EQ_PATH_FASTER1,
					       p->dstnet, p->rt_mask);
		}
		return;
	    }
	    if ((slower_pp == NULL) &&
		NOVELL_FASTER_NIDB(nidb, pp->nidb)) {
		slower_last_pp = last_pp;
		slower_pp = pp;
	    }
	    prev_last_pp = last_pp;
	    last_pp = pp;
	    pp = pp->next;
	}
	if (p->paths < novell_maxpaths) {
	    pp = malloc_named(sizeof(novrdbtype), "IPX RIP RDB");
	    if (pp == NULL) 
		return;
	    /*
	     * Keep the list sorted by increasing delay. This
	     * will help us to choose the fastest interface.
	     */
	    if (slower_pp != NULL) {
		pp->next = slower_pp;
		slower_last_pp->next = pp;
	    } else {
		last_pp->next = pp;
		pp->next = NULL;
	    }
	    p->paths++;
	    p->procmask = novrip_pdb.mask;
	    pp->procmask = novrip_pdb.mask;
	    p->flags = NET_RIP;		/* used as dynamic */
	    if (novellrip_debug || novellrip_event_debug )
		buginf("\nIPXRIP: Adding multiple path #%d for net %x %x",  
		       p->paths, dstnet, netmask);
	    pp->nidb = nidb;
	    pp->srcnet = snet;
	    ieee_copy(shost, pp->srchost);
	    novell_set_timer(pp, NULL);
	    novcache_increment_version(NULL, NOVCACHE_RIP_ADD_EQ_PATH,
				       p->dstnet, p->rt_mask);
	    novell_mark_rdb_changed(p, pp, p->flags,
				    IPX_ROUTE_EVENT_ADD);
	} else {
	    /* if this interface is faster then use it instead */
	    if (slower_pp != NULL) {
		/*
		 * If slower path is already at end of list then overwrite
		 * it. Otherwise, delete last path descriptor and sort back
		 * into list:
		 */
		if (slower_pp != last_pp) {
		    prev_last_pp->next = NULL;
		    last_pp->next = slower_pp;
		    slower_last_pp->next = last_pp;
		}
		novell_mark_rdb_changed(p, last_pp, p->flags,
					IPX_ROUTE_EVENT_DELETED);
		p->procmask = novrip_pdb.mask;
		last_pp->procmask = novrip_pdb.mask;
		p->flags = NET_RIP;	/* used as dynamic */
		if (novellrip_debug || novellrip_event_debug)
		    buginf("\nIPXRIP: Replacing path #%d for net %x %x",  
			   p->paths, dstnet, netmask);
		last_pp->nidb = nidb;
		last_pp->srcnet = snet;
		ieee_copy(shost, last_pp->srchost);
		novell_set_timer(last_pp, NULL);
		novell_mark_rdb_changed(p, last_pp, p->flags,
					IPX_ROUTE_EVENT_ADD);
		novcache_increment_version(NULL, 
					   NOVCACHE_RIP_ADD_EQ_PATH_FASTER,
					   p->dstnet, p->rt_mask);
		return;
	    }
	}
	return;
    } else if ((p->flags & NET_FLOATING_STATIC) || 
		((p->flags & NET_RIP) &&
		NOVELL_BETTER_ROUTE_INFO(hopcount, delay, p)) ||
	       ((p->flags & NET_IGRP) &&
		((hopcount < p->ext_hop) || 
		 (hopcount == 1 && p->ext_hop == 1)))) {
	/*
	 * WHEN A RIP ROUTE COMES IN:
	 * 
	 * When a rip/connected/static route is in the table as the current
	 * best:
	 *   Compare delay/hopcount as always
	 *
	 * When an IGRP route is in the table as the current best:
	 *   Always prefer IGRP unless RIP hopcount is better than IGRP
	 *   external hopcount metric.
	 */
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: better [%d/%d] route for %x %x from %x.%e"
		   ", flushing old [%d/%d] route/paths",
		   delay, hopcount, dstnet, netmask, snet, shost,
		   p->delay, p->hopcount);
        pdb = nov_proctable[p->novpdbindex];
	if (pdb->lost_route) {
	    (*pdb->lost_route)(pdb, p);
	}
	TIMER_STOP(p->holddownuntil);
 	p->reverse_poisoned = FALSE;
	p->hopcount = p->ext_hop = p->last_hopcount = hopcount;
	p->delay = p->ext_delay = p->last_delay = delay;
	p->path_metric = 0;
	p->igrp_delay = NOT_IBM_TICKS(delay);
	pp = malloc_named(sizeof(novrdbtype), "IPX RIP RDB");
	if (pp == NULL)
	    return;
	last_pp = temp_pp = p->path_list;
	/*
	 * If we had only a single path and the better metric is being
	 * advertised by the *same* next hop router, there is no need to
	 * flush the cache.
	 */
	if ((p->paths == 1) && (snet == temp_pp->srcnet) &&
	    (temp_pp->nidb == nidb) &&
	    ieee_equal(temp_pp->srchost, shost)) {
	    ;
	} else {
	    novcache_increment_version(NULL, NOVCACHE_RIP_BETTER_METRIC, 
				       p->dstnet, p->rt_mask);
	}
	p->path_list = NULL;
	while (last_pp) {
	    last_pp = temp_pp->next;
	    novell_mark_rdb_changed(p, temp_pp, p->flags,
				    IPX_ROUTE_EVENT_DELETED);
	    novrdb_free(temp_pp);
	    temp_pp = last_pp;
	}
	p->path_list = p->nextpath = pp;
	p->paths = 1;
	p->options = IPX_NDB_OPTION_VALID;
	p->procmask = novrip_pdb.mask;
	pp->procmask = novrip_pdb.mask;
	p->novpdbindex = novrip_pdb.index;
	p->flags = NET_RIP;		/* used as dynamic */
	pp->nidb = nidb;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	novell_set_timer(pp, NULL);
	novell_mark_rip_changed(p);
	novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_ADD);
	return;
    } else if ((p->flags & NET_RIP) &&
	(NOVELL_WORSE_ROUTE_INFO(hopcount, delay, p) ||
	 (hopcount >= novell_infinity))) {
	last_pp = (novrdbtype *) (&p->path_list);
	pp = p->path_list;
	while (pp) {
	    /*
	     * Worse cost from same host with same protocol
	     */
	    if ((snet == pp->srcnet) &&
		(pp->nidb == nidb) &&
		ieee_equal(pp->srchost, shost) &&
		(pp->procmask == novrip_pdb.mask)) {
		if (p->paths == 1) {
		    /* NLSP wants this kludge, no poisons! */
		    if ((hopcount < novell_infinity) &&
			(novell_nlsp_running > 0)) {
			/*
			 * This is a pessimal change (not a poison).
			 * If NLSP is running, treat pessimal routes
			 * as changed. (If not running, see below.)
			 */
			p->hopcount = p->ext_hop = p->last_hopcount = hopcount;
			p->delay = p->ext_delay = p->last_delay = delay;
			novell_mark_rdb_changed
			    (p, pp, p->flags, IPX_ROUTE_EVENT_CHANGE);
			novell_mark_rip_changed(p);
			return;
		    } else {
			/*
			 * This may be a poison or a pessimal route. In any
			 * case our convention has always been to poison
			 * when a RIP route goes pessimal. Preserve historic
			 * behavior unless NLSP running (see above).
			 */
			novell_mark_rdb_changed
			    (p, pp, p->flags, IPX_ROUTE_EVENT_DELETED);
			novrip_poison(p, delay, hopcount);
		    }
		    break;
		} else {
		    novcache_increment_version(NULL, 
					       NOVCACHE_RIP_WORSE_METRIC,
					       p->dstnet, p->rt_mask);
		    p->paths--;
		    if (novellrip_debug || novellrip_event_debug)
			buginf("\nIPXRIP: delete path to %x %x via %e"
			       ", %d paths remaining", 
			       dstnet, netmask, shost, p->paths);
		    if ((last_pp->next = p->nextpath = pp->next) == NULL)
			p->nextpath = p->path_list;
		    novell_mark_rdb_changed(p, pp, p->flags,
					    IPX_ROUTE_EVENT_DELETED);
		    novrdb_free(pp);
		    break;
		}
	    }
	    last_pp = pp;
	    pp = pp->next;
	}
    }
    /*
     * If we make it down here, we have a route that we chose not to
     * inject into the table, for whatever reason. Perhaps it is a
     * poison? If so, and if we have a valid route, then advertise
     * it back to the source of the poison via the lost route algorithm.
     */

    /*
     * Implement 'Novell Lost Route' algorithm which does the following:
     *    If another router advertising network 'X' as down, *and* we
     *    have another path to network 'X', we wait 1/2 second than send
     *    a 'flash' update out about network 'X' so that the router
     *    advertising it as 'down' can get this new route (if possible.).
     * The easiest way for our code to implement this hack is to simply
     * mark the 'changed' field. Note that our flash update throttling
     * may delay the sending of this stuff...
     */
  check_for_lost_route:
    if ((hopcount >= novell_infinity) &&
	(!TIMER_RUNNING(p->holddownuntil)) && (p->paths > 0)) {
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: mark lost route to %x.%e "
		   "for net %x %x, [%d/%d]",
		   snet, shost, dstnet, netmask, p->delay, p->hopcount);
	novell_mark_advertise_to_lost(p);
    }
}

/*
 * novrip_find_rt
 * Return ptr to routing table based on destination network
 */

novndbtype *novrip_find_rt (ulong net, ulong netmask, 
			    enum IPX_ROUTE_MATCH_TYPE how)
{
    novndbtype *p = NULL;
    nov_sumroutetype *rnnode;
    ipxradixkeytype net_key;

    if (netmask == NOV_NETMASK_ALL) {
	p = (*novell_table)[NOV_NETHASH(net)];
	for (; p; p = p->next) 
	    if (net == p->dstnet)
		break;
    } else {
	rnnode = (nov_sumroutetype *)
	    rn_match(ipxaddr2radixkey(net, &net_key), nov_rnhead);
	if (rnnode) {
	    p = rnnode->ndb;
	    if (p->dstnet != net || p->rt_mask != netmask)
		p = NULL;
	}
    }
    /* Doing a fuzz search? */
    if (how == IPX_ROUTE_MATCH_FUZZ) {
	if (p == NULL || !ipx_route_valid(p)) {
	    p = NULL; 
	    rnnode = (nov_sumroutetype *)
		rn_match(ipxaddr2radixkey(net, &net_key), nov_rnhead);
	    if (rnnode) {
		p = rnnode->ndb;
	    }
	    if (p == NULL || !ipx_route_valid(p)) {
		p = NULL;  
		if ((novell_default_ndb != NULL) && novell_default_route) {
		    p = novell_default_ndb;
		}
	    }
	}
    }
    return(p);
}

/*
 * nov_add_floating
 * Get a floating static route from the floating static table and put
 * it in the routing table.
 */

void
nov_add_floating (
    novpdbtype *pdb,
    ulong net,
    ulong netmask,
    ulong unsed1,		  
    ulong unused2)
{
    novndbtype *p;
    int i;
    uchar host[IEEEBYTES], *rinp;

    /*
     * If there is a dynamically learned route in the table, don't add
     * the floating static. A route that is about to be deleted will
     * have an infinity metric.
     */
    if ((p = novrip_find_rt(net, netmask, IPX_ROUTE_MATCH_EXACT))) { 
	/*
	 * We should *never* add a route here, even if the existing route
	 * has an infinite metric. We must wait until this NDB is
	 * DELETED. Otherwise, the route would be in the table *twice*.
	 */
	return;
    }

    /* Look for net in floating static table */

    p = (*static_table)[NOV_NETHASH(net)];
    for (; p; p = p->next) {
	if ((net == p->dstnet) && (p->rt_mask == netmask))
            break;
    }

    if ((p == NULL) || (p->path_list == NULL)) {
        return;
    }

    /* Found it, put in routing table. */

    /* Copy correct source host */
    if (p->options & IPX_NDB_OPTION_INTERFACE_STATIC) {
	rinp = (uchar *) &(p->path_list->nidb->nov_ipxwan_remote_internal_net);
	for (i = 0; i < 4; i++) {
	    host[i] = *rinp++;
	}
	host[4] = host[5] = 0;
    } else {
	ieee_copy(p->path_list->srchost, host);
    }
    novrip_add_rt(&novfstatic_pdb, p->path_list->nidb, net, netmask,
		  p->path_list->nidb->novellnet, host,
		  p->last_delay, p->last_hopcount,
		  (NET_FLOATING_STATIC | NET_STATIC),
		  IPX_ROUTE_ADD_NORMAL,  IPX_STATIC_DEFINE_TYPE_NO_NVGEN,
		  ((p->options & IPX_NDB_OPTION_INTERFACE_STATIC) ?
		   IPX_STATIC_TYPE_INTERFACE : IPX_STATIC_TYPE_HOST),
		  IPX_FLOATING_STATIC_EXISTS_YES);
}

/* novigrp_network_revise
 *
 * For connected routes, scan pdb looking for networks that match and 
 * then call nov_network_finish for each one. 
 */
static void nov_network_revise (ulong network)
{
    novpdbtype *pdb;
    int i;

    for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb = pdb->next)
        for (i = 0; i < pdb->netcount; i++)
            if (pdb->networks[i] == network) {
                nov_network_finish(pdb, network, TRUE);
		break;
	    }
}

/*
 * novrip_add_rt
 * Add an IPX RIP route to the table
 */

void
novrip_add_rt (
    novpdbtype *pdb,
    nidbtype *nidb,
    ulong dstnet,
    ulong netmask,	       
    ulong net,
    uchar *host,
    int ticks,
    int hops,
    ulong flags,
    enum IPX_ROUTE_ADD route_type,
    enum IPX_STATIC_DEFINE_TYPE nvgen_option,
    enum IPX_STATIC_TYPE static_type,
    enum IPX_FLOATING_STATIC_EXISTS floating_static_exists)
{
    int bucket;
    novndbtype *p;
    novrdbtype *pp = NULL;
    novrdbtype *newrdb;
    novndbtype *newndb = NULL;
    novndbtype *have_dynamic;
    nov_sumroutetype *rnnode;

    /* Immediate bail if network 0/-1 
     * -2 is treated like a normal route if default handling off */
    if (NOV_ILLEGAL_NET(dstnet) && !ZERO_PREFIX(dstnet, netmask)) {
	return;
    }
    have_dynamic = novrip_find_rt(dstnet, netmask, IPX_ROUTE_MATCH_EXACT);

    /* Don't add the route to the table again if it is not floating static */
    if (!(flags & NET_FLOATING_STATIC) && have_dynamic)
	return;
    p = nov_ndb_malloc();
    if (p == NULL) 
	return;
    pp = malloc_named(sizeof(novrdbtype), "IPX RIP RDB");
    if (pp == NULL) {
	nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	return;
    }
    p->options = IPX_NDB_OPTION_VALID;
    p->path_list = p->nextpath = pp;
    p->paths = 1;
    /*
     * Declare imaginary delay for internal delivery:
     */
    if (flags & NET_INTERNAL) {
	p->delay = p->last_delay = NOVELL_DEFAULT_CONNECTED_TICKS;;
    }
    p->flags = flags;
    if (nvgen_option == IPX_STATIC_DEFINE_TYPE_NO_NVGEN) {
	p->options |= IPX_NDB_OPTION_STATIC_NO_NVGEN;
    }
    if (static_type == IPX_STATIC_TYPE_INTERFACE) {
	p->options |= IPX_NDB_OPTION_INTERFACE_STATIC;
    }

    /*
     * Fill in the fields from the pdb.
     */
    p->procmask = pdb->mask;
    p->novpdbindex = pdb->index;
    TIMER_STOP(p->holddownuntil);
    if (netmask == NOV_NETMASK_ALL) {
	p->delay = p->ext_delay = p->last_delay = (flags & NET_CONNECTED) ?
	  NOVELL_DEFAULT_CONNECTED_TICKS :
	    (ticks ? ticks : NOVELL_DEFAULT_CONNECTED_TICKS);
	p->hopcount = p->last_hopcount = p->ext_hop = 
	  (flags & NET_CONNECTED) ?  NOVELL_DEFAULT_CONNECTED_HOPS :
	    (hops ? hops : NOVELL_DEFAULT_RIP_HOPS);
	p->area_count = 0;
    } else {
	p->delay = p->ext_delay = p->last_delay = (flags & NET_CONNECTED) ?
	  NOVELL_DEFAULT_CONNECTED_TICKS :
	  (ticks ? ticks : NOVELL_DEFAULT_CONNECTED_TICKS);
	p->area_count = hops ? hops : NOVELL_DEFAULT_AREA_COUNT;
	p->hopcount = p->ext_hop = p->last_hopcount = NOVELL_MAX_HOPS_ALLOWED;
    }
    p->dstnet = dstnet;
    p->rt_mask = netmask;
    p->keylen = p->masklen = IPXRADIXKEYLEN;
    pp->nidb = nidb;
    pp->srcnet = net;
    pp->procmask = pdb->mask;
    GET_TIMESTAMP(pp->timer);
    pp->uses = 0L;
    if (host)
	ieee_copy(host, pp->srchost);
    bucket = NOV_NETHASH(dstnet);
    /*
     * Put floating static routes in the static routing table. Routes remain
     * in this table even while not in the routing table.
     */
    if (flags & NET_FLOATING_STATIC) {
	if (floating_static_exists == IPX_FLOATING_STATIC_EXISTS_NO) {
	    if (!(newndb = nov_ndb_malloc())) {
		free(pp);
		nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		return;
	    }
	    if (!(newrdb = malloc_named(sizeof(novrdbtype), "IPX RIP RDB"))) {
		free(pp);
		nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		nov_ndb_free(newndb, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		return;
	    }
	    bcopy(p, newndb, sizeof(novndbtype));
	    bcopy(pp, newrdb, sizeof(novrdbtype));
	    newndb->path_list = newndb->nextpath = newrdb;
	    newndb->next = (*static_table)[bucket];
	    (*static_table)[bucket] = newndb;
	}
    }
    /*
     * If this is a floating static and no dynamically learned info is
     * in the routing table or if this is a "hard" static route, add
     * route to routing table.
     */
    if (!have_dynamic || !(flags & NET_FLOATING_STATIC)) {
	/*
	 * Add the route to the hash table if netmask is -1.
	 */
	if (netmask == NOV_NETMASK_ALL) {
	    p->next = (*novell_table)[bucket];
	    (*novell_table)[bucket] = p;
	} else {
	    /*
	     * Add the route to the patricia tree if netmask is not -1.
	     */
	    rnnode = malloc(sizeof(nov_sumroutetype));
	    if (!rnnode) {
		free(pp);
		nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
		return;
	    }
	    rnnode->ndb = p;
	    if (!rn_addroute(&p->keylen, &p->masklen, nov_rnhead, 
			    rnnode->rt_nodes)) {
		buginf("\nIPXRIP: novrip_add_rt did not add %x %x properly",
		       p->dstnet, p->rt_mask);
	    }
	    p->flags |= NET_SUMMARY;
	}
    } else {
	free(pp);
	nov_ndb_free(p, IPX_ROUTE_FREE_NEVER_IN_TABLE, TRUE);
	return;
    }
    /*
     * Flash the addition unless connected network or static network and
     * the interface is down!
     */
    if ((!(flags & (NET_CONNECTED|NET_STATIC))) || (interface_up(nidb->idb))) {
      novell_mark_rip_changed(p);
    } else {
      p->hopcount = p->ext_hop =  novell_infinity; /* Network is down */
    }
    /*
     * If network is directly connected, then set the redist flags as needed
     */
    if (flags & NET_CONNECTED)
	nov_network_revise(nidb->novellnet);

    novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_ADD);
}

/*
 * nov_del_floating
 * Remove a route from the floating static table
 */

void
nov_del_floating (
    ulong net, ulong netmask)
{
    novndbtype *p, *pred;
    novrdbtype *pp, *last_pp;
    int bucket;

    bucket = NOV_NETHASH(net);
    p = (*static_table)[bucket];
    pred = p;
    while (p && (p->dstnet != net || p->rt_mask != netmask)) {
        pred = p;
        p = p->next;
    }
    if (p) {
        pp = p->path_list;
        last_pp = pp = p->path_list;
        while (last_pp) {
            last_pp = pp->next;
	    free(pp);
            pp = last_pp;
        }
        if (p != pred)
            pred->next = p->next;
        else
            (*static_table)[bucket] = p->next;
        free (p);
    }
}

/*
 * novrip_del_rt
 * Remove a RIP route from the table
 */

void
novrip_del_rt (             
    ulong net,
    ulong netmask,
    enum IPX_ROUTE_DELETE action)
{
    novndbtype *p, *pred;
    novrdbtype *pp, *last_pp;
    novpdbtype *pdb;
    int bucket;
    nov_sumroutetype *rnnode;
    ipxradixkeytype net_key, mask_key;

    if (netmask != NOV_NETMASK_ALL) {
	rnnode = (nov_sumroutetype *)
	    rn_delete(ipxaddr2radixkey(net, &net_key), 
		      ipxaddr2radixkey(netmask, &mask_key), nov_rnhead);
	if (rnnode) {
	    p = rnnode->ndb;
	    if (p) {
		pp = p->path_list;
		if ((action == IPX_ROUTE_DELETE_RETAIN_CONNECTED) &&
		    (p->flags & NET_CONNECTED)) {
		    buginf("\nCan't delete interface route to network %q",
			   NOVELL_TYPE, p->dstnet);
		    return;
		}
		last_pp = pp = p->path_list;
		p->path_list = NULL;
		while (last_pp) {
		    last_pp = pp->next;
		    novell_mark_rdb_changed(p, pp, p->flags, 
					    IPX_ROUTE_EVENT_DELETED);
		    novrdb_free(pp);
		    pp = last_pp;
		}
		if (novellrip_debug || novellrip_event_debug)
		    buginf("\nIPXRIP: Deleting network %x %x", p->dstnet,
			   p->rt_mask);
		pdb = nov_proctable[p->novpdbindex];
		if (pdb->lost_route)
		    (*pdb->lost_route)(pdb, p);
		p->options |= IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP;
		nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
	    }
	    free(rnnode);
	}
	return;
    }
    bucket = NOV_NETHASH(net);
    p = (*novell_table)[bucket];
    pred = p;
    while (p && (p->dstnet != net)) {
	pred = p;
	p = p->next;
    }
    if (p) {
	pp = p->path_list;
	if ((action == IPX_ROUTE_DELETE_REMOVE_DYNAMIC) &&
	    !(p->flags & (NET_RIP | NET_IGRP | NET_NLSP | NET_ISIS))) {
            printf("\n%%Network %x is %s and cannot be cleared", p->dstnet,
		   p->flags & NET_INTERNAL ? "internal" :
		   p->flags & NET_CONNECTED ? "connected" : 
		   p->flags & NET_FLOATING_STATIC ? "floating static" : 
		   p->flags & NET_STATIC ? "static" : "unknown");
	    return;
	}
	if ((action == IPX_ROUTE_DELETE_RETAIN_CONNECTED) &&
	    (p->flags & NET_CONNECTED)) {
	    buginf("\nCan't delete interface route to network %q",
		   NOVELL_TYPE, p->dstnet);
	    return;
	}
	last_pp = pp = p->path_list;
	p->path_list = NULL;
	while (last_pp) {
	    last_pp = pp->next;
	    novell_mark_rdb_changed(p, pp, p->flags, IPX_ROUTE_EVENT_DELETED);
	    novrdb_free(pp);
	    pp = last_pp;
	}
	if (p != pred)
	    pred->next = p->next;
	else
	    (*novell_table)[bucket] = p->next;
	if (novellrip_debug || novellrip_event_debug)
	  buginf("\nIPXRIP: Deleting network %x %x",
		 p->dstnet, p->rt_mask);
        pdb = nov_proctable[p->novpdbindex];
	if (pdb->lost_route)
	    (*pdb->lost_route)(pdb, p);
	p->options |= IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP;
	nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
    }
}

void
novrip_prune_nidb (nidbtype *nidb)
{
    novndbtype *p, *pred;
    int i;
    novrdbtype *pp, *last_pp;
    novpdbtype *pdb;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
        p = (*novell_table)[i];
        if (p == NULL)
            continue;
        pred = p;
        while (p) {
            if ((p->flags & (NET_RIP | NET_IGRP | NET_CONNECTED))) {
                last_pp = (novrdbtype *) (&p->path_list);
		pp = p->path_list;
                while (pp) {
                    if (pp->nidb == nidb) {
			last_pp->next = pp->next;
			p->paths--;
			novell_mark_rdb_changed(p, pp, p->flags,
						IPX_ROUTE_EVENT_DELETED);
			novrdb_free(pp);
			pp = last_pp->next;
		    } else { 
			last_pp = pp;
			pp = pp->next;
		    } 
                }
                p->nextpath = p->path_list;
		if (p->paths <= 0) {
		    if (p->flags & (NET_IGRP)) {
			if (novellrip_debug || novellrip_event_debug)
			    buginf("\nIPXRIP: garbage collecting entry for %x %x",
				   p->dstnet, p->rt_mask);
			p->options &= ~IPX_NDB_OPTION_VALID;
			if (p != pred)
			    pred->next = p->next;
			else {
			    (*novell_table)[i] = p->next;
			    pred = NULL;
			}
			novell_routes--;
			pdb = nov_proctable[p->novpdbindex];
			nov_network_redist(pdb, p, ROUTE_DOWN);
			nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
			p = pred;
		    } else {
			/* Force route to be RIP */
			p->flags &= ~(NET_IGRP | NET_CONNECTED);
			p->flags |= NET_RIP;
			/* RIP routes should go into holddown */
			novrip_poison(p, NOVELL_INFDELAY, novell_infinity);
		    }
		}
	    }
	    /* Did we delete the front of the list, if so, start over... */
	    if (pred == NULL) {
		pred = p = (*novell_table)[i];
	    } else {
		pred = p;
		if (p != NULL) {
		    p = p->next;
		}
	    }
	}
    }
}

/*
 * novell_count_routes
 * Count the total number of routes in the table
 */
int novell_count_routes (void)
{
    return(novell_route_count);
}

static void free_rdb_list (nov_rdb_list *lh)
{
    nov_rdb_list *le, *next_le;

    for (le = lh; le; le = next_le) {
	next_le = le->next;
	free(le->rdb); /* Unlock rdb */
	free(le);
    }
}

/*
 * display_igrp_path
 */
static void display_igrp_path (novrdbtype *pp,
			       boolean first_path,
			       int len,
			       boolean detailed)
{
    if (first_path) {
	printf(" via %-8x.%e, age ", pp->srcnet, pp->srchost);
	print_dhms(pp->timer); 
	printf(",\n\t\t\t %ldu,", pp->uses);
	if (pp->nidb == NULL) {
	    printf(" no interface");
	} else {
	    printf(" %s%s", pp->nidb->idb->short_namestring,
		   (NOVELL_NIDB_IDB_UP(pp->nidb) ? "" : " (down)"));
	}
    } else {
	printf("\n%-*cvia %-8x.%e, age ",
	       len, ' ', pp->srcnet, pp->srchost);
	print_dhms(pp->timer); 
	printf(",\n\t\t\t %ldu,", pp->uses);
	if (pp->nidb == NULL) {
	    printf(" no interface");
	} else {
	    printf(" %s%s", pp->nidb->idb->short_namestring,
		   (NOVELL_NIDB_IDB_UP(pp->nidb) ? "" : " (down)"));
	}
    }
}

/*
 * display_igrp_path_list
 */
static void display_igrp_path_list (nov_rdb_list *first_le,
				    int len,
				    boolean detailed)
{
    nov_rdb_list *le, *next_le;
    boolean first_path = TRUE;

    for (le = first_le; le; le = next_le) {
	next_le = le->next;
	display_igrp_path(le->rdb, first_path, len, detailed);
	first_path = FALSE;
    }
}

/*
 * display_path
 */
static void display_path (novndbtype *p,
			  novrdbtype *pp,
			  boolean first_path,
			  int len,
			  boolean detailed)
{

    if (!detailed) {
	if (!first_path) {
	    printf("\n%-*c", len, ' ');
	}
	printf("%-*cvia %-8x.%e",
	       (((next_hop_column > 0) && ((next_hop_column - len) > 0)) ?
		(next_hop_column - len + 1) : 0), ' ',
	       pp->srcnet, pp->srchost);
    } else {
	printf("\n%-*c-- via %-8x.%e", 10, ' ', pp->srcnet, pp->srchost);
    }
    if (p->flags & NET_STATIC || pp->nidb == nullnidb) {
	printf(",       ");
    } else {
	printf(", %-4ds,", ELAPSED_TIME(pp->timer)/ONESEC);
    }
    if (detailed) {
	printf("%-6du,", pp->uses);
    }
    if (pp->nidb == NULL) {
	printf(" no interface");
    } else {
	printf(" %s%s", pp->nidb->idb->short_namestring,
	       (NOVELL_NIDB_IDB_UP(pp->nidb) ? "" : "(down)"));
    }
    if ((p->flags & NET_NLSP) && detailed) {
	printf("\n%-*c%d:%d:%d:%e:%-02d:%e", 17, ' ',
	       pp->protocol_info.nlsp_info.cost.total_throughput,
	       pp->protocol_info.nlsp_info.cost.total_delay,
	       pp->protocol_info.nlsp_info.cost.mtu,
	       pp->protocol_info.nlsp_info.cost.system_id,
	       pp->protocol_info.nlsp_info.cost.circuit_id,
	       pp->protocol_info.nlsp_info.cost.mac_addr);
	printf(" {%d/%d}", pp->lsp_linkage.lsp_index,
	       pp->lsp_linkage.lsp_version);
    }
}

/*
 * display_path_list
 */
static void display_path_list (novndbtype *p,
			       nov_rdb_list *first_le,
			       int len,
			       boolean detailed)
{
    nov_rdb_list *le, *next_le;
    boolean first_path = TRUE;
    int pathcount = 0;

    for (le = first_le; le; le = next_le) {
	next_le = le->next;
	display_path(p, le->rdb, first_path, len, detailed);
	first_path = FALSE;
	if ((++pathcount >= novell_maxpaths) && (p->flags & NET_NLSP)) {
	    break;
	}
    }
}

/*
 * display_route_conn
 */
static void display_route_conn (novndbtype *p,
				nov_rdb_list *le,
				novpdbtype *pdb,
				boolean detailed)
{
    const char *encap;
    hwidbtype *hwidb = NULL;
    int l = -1;

    if ((le == NULL) || (le->rdb == NULL) || (le->rdb->nidb == NULL)) {
	return;
    }
    if (le->rdb->nidb->idb != NULL) {
	hwidb = hwidb_or_null(le->rdb->nidb->idb);
    }
    encap = ((le->rdb->nidb->enctype != ET_HDLC) ? 
	     encstring(le->rdb->nidb->enctype) :
	     (hwidb != NULL) ? encstring(hwidb->enctype) : "NONE");
    
    if (p->flags & NET_INTERNAL) {
	printf("\nL   %-8x is the internal network",
	       p->dstnet);
    } else {

	if (le->rdb->nidb->idb != NULL) {
	    l += printf("\n%c   %-8x (%s),",
			(nov_ipxwan_on_nidb(le->rdb->nidb) ? 'W' :
			 (IPX_NIDB_IS_PRIMARY(le->rdb->nidb) ? 'C' : 'c')),
			p->dstnet, encap);
	    
	    l += printf("%-*c %s%s",
			13-strlen(encap), ' ',
			le->rdb->nidb->idb->short_namestring,
			(NOVELL_NIDB_IDB_UP(le->rdb->nidb) ? "" : " (down)"));
	} else {
	    l += printf("\n%c   %-8x (%s),",
			(nov_ipxwan_on_nidb(le->rdb->nidb) ? 'W' : 'C'),
			p->dstnet, encap);
	    
	    l += printf("%-*c %s", 13-strlen(encap), ' ', "no interface");
	}
    }
}

/*
 * display_route_static
 */
static void display_route_static (novndbtype *p,
				  nov_rdb_list *le,
				  novpdbtype *pdb,
				  boolean detailed)
{
    int l = -1;

    l += printf("\n%c%c  %-8x",
		(p->flags & NET_FLOATING_STATIC) ? 'F' : 'S', 
		(p->flags & NET_SUMMARY) ? 'A' : ' ', p->dstnet);

    if (p->rt_mask != NOV_NETMASK_ALL)
	l += printf(" %-8x ", p->rt_mask);

    if (p->hopcount >= novell_infinity) {
      l += printf("[**/");
    } else {
      if ((p->hopcount > NOVELL_DEFAULT_RIP_HOPS) ||
	  (p->delay > NOVELL_DEFAULT_CONNECTED_TICKS))
	l += printf("[%-02u/", p->delay);
    }
    if (p->delay == NOVELL_INFDELAY) {
      l += printf("**]");
    } else {
      if ((p->hopcount > NOVELL_DEFAULT_RIP_HOPS) ||
	  (p->delay > NOVELL_DEFAULT_CONNECTED_TICKS))
	l += printf("%-02u]", p->hopcount);
    }
    display_path_list(p, le, l, detailed);
}

/*
 * display_route_rip
 */
static void display_route_rip (novndbtype *p,
			       nov_rdb_list *le,
			       novpdbtype *pdb,
			       boolean detailed)
{
    int l = -1;

    l += printf("\nR%s%-8x ", (TIMER_RUNNING(p->holddownuntil) ?
			       "H  " : "   "), p->dstnet);
    if (p->hopcount >= novell_infinity) {
	l += printf("[**/");
    } else {
	l += printf("[%-02u/", p->delay);
    }
    if (p->delay == NOVELL_INFDELAY) {
	l += printf("**]");
    } else {
	l += printf("%-02u]", p->hopcount);
    }
    display_path_list(p, le, l, detailed);
}

/*
 * display_route_igrp
 */
static void display_route_igrp (novndbtype *p,
				nov_rdb_list *le,
				novpdbtype *pdb,
				boolean detailed)
{
    int l = -1;

    l += printf("\nE%s%-8x ", ((p->flags & NET_SUMMARY) &&
			       (TIMER_RUNNING(p->holddownuntil)) ?
			       "AH " : (TIMER_RUNNING(p->holddownuntil)) ? 
			       "H  " : (p->flags & NET_SUMMARY) ?
			       "A  " : "   "), p->dstnet);

    if (p->rt_mask != NOV_NETMASK_ALL)
	l += printf("%-8x ", p->rt_mask);

    if (p->path_metric == METRIC_INACCESSIBLE) {
	l += printf("[**/");
    } else {
	l += printf("[%u/", p->path_metric);
    }
    if (p->ext_hop >= novell_infinity) {
	l += printf("**]");
    } else {
	l += printf("%u]", p->ext_hop);
    }
    display_igrp_path_list(le, l, detailed);
}

/*
 * display_route_isis
 */
static void display_route_isis (novndbtype *p,
				nov_rdb_list *le,
				novpdbtype *pdb,
				boolean detailed)
{
    int l = -1;

    l += printf("\nI%s%-8x ",
	   ((p->flags & NET_XROUTE) ? "X  " : "   "), p->dstnet);
    if (p->path_metric == METRIC_INACCESSIBLE) {
	l += printf("[**/");
    } else {
	l += printf("[%-02u/", p->path_metric);
    }
    if (p->delay == NOVELL_INFDELAY) {
	l += printf("**/");
    } else {
	l += printf("%-02u/", p->delay);
    }
    if (p->hopcount >= novell_infinity) {
	l += printf("**]");
    } else {
	l += printf("%-02u]", p->hopcount);
    }
    display_path_list(p, le, l, detailed);
}

/*
 * display_route_nlsp
 */
static void display_route_nlsp (novndbtype *p,
				nov_rdb_list *le,
				novpdbtype *pdb,
				boolean detailed)
{
    int l = -1;

    if (!TIMER_RUNNING(p->holddownuntil)) {
	l += printf("\nN%s%-8x ", ((p->flags & (NET_XROUTE | NET_SUMMARY)) ==
				   (NET_XROUTE | NET_SUMMARY)) ? "XA "
		    : (p->flags & NET_XROUTE) ? "X  " : 
		    (p->flags & NET_SUMMARY) ?
		    "A  " : "   ", p->dstnet);
    } else {
	l += printf("\nN%s%-8x ", ((p->flags & (NET_XROUTE | NET_SUMMARY)) == 
				   (NET_XROUTE | NET_SUMMARY)) ? 
		    "XAH" : (p->flags & NET_XROUTE) ? "XH " : 
		    (p->flags & NET_SUMMARY) ? "AH " : "   ", p->dstnet);
    }

    if (p->rt_mask != NOV_NETMASK_ALL)
	l += printf("%-8x ", p->rt_mask);

    if (le && le->rdb && le->rdb->nidb == nullnidb) {
	if (l > next_hop_column) {
	    next_hop_column = l;
	}
	display_path_list(p, le, l, detailed);
	return;
    }

    if (p->path_metric == METRIC_INACCESSIBLE) {
	l += printf("[**]");
    } else {
	l += printf("[%u]", p->path_metric);
    }
    if (p->delay == NOVELL_INFDELAY) {
	l += printf("[**/");
    } else {
	l += printf("[%-02u/", p->delay);
    }
    if (p->rt_mask == NOV_NETMASK_ALL) {
	if (p->hopcount >= novell_infinity) {
	    l += printf("**]");
	} else {
	    l += printf("%-02u]", p->hopcount);
	}
    } else {
	l += printf("%-02u]", p->area_count);
    }
    if (p->flags & NET_XROUTE) {
	if (p->ext_delay == NOVELL_INFDELAY) {
	    l += printf("[**/");
	} else {
	    l += printf("[%-02u/", p->ext_delay);
	}
	if (p->ext_hop >= novell_infinity) {
	    l += printf("**]");
	} else {
	    l += printf("%-02u]", p->ext_hop);
	}
    }
    if (l > next_hop_column) {
	next_hop_column = l;
    }
    display_path_list(p, le, l, detailed);
}

/*
 * display_route_unknown
 */
static void display_route_unknown (novndbtype *p)
{
    printf("\nU   %-8x %-8x, type unknown", p->dstnet, p->rt_mask);
}

/*
 * display_route
 */

static void display_route (novndbtype *p, boolean detailed)
{
    novpdbtype *pdb;
    novrdbtype *rdb;
    ulong flags;
    nov_rdb_list *rdb_lh = NULL;  /* rdb display listhead */
    nov_rdb_list *le;             /* rdb display list element */
    nov_rdb_list *last_le = NULL;

    flags = p->flags;
    flags = flags &
	(NET_CONNECTED|NET_STATIC|NET_RIP|NET_IGRP|NET_ISIS|NET_NLSP);

    /*
     * Build a list of rdb descriptors, since all kinds of mean,
     * nasty, ugly things can happen to rdb's during the display.
     * Be sure to preserve rdb order since the descriptor list is
     * what is displayed (not the rdb list).
     */
    for (rdb = p->path_list; rdb != NULL; rdb = rdb->next) {
	le = malloc(sizeof(nov_rdb_list));
	if (le == NULL) {
	    free_rdb_list(rdb_lh);
	    return;
	}
	mem_lock(rdb); /* Lock rdb */
	le->rdb = rdb;
	if (rdb_lh == NULL) {
	    rdb_lh = le;
	}
	if (last_le != NULL) {
	    last_le->next = le;
	}
	last_le = le;
    }
    /*
     * Remember the first rdb descriptor, if any:
     */
    le = rdb_lh;

    pdb = nov_proctable[p->novpdbindex];

    switch (flags) {
      case NET_CONNECTED:
	display_route_conn(p, le, pdb, detailed);
	break;

      case NET_STATIC:
	display_route_static(p, le, pdb, detailed);
	break;

      case NET_RIP:
	display_route_rip(p, le, pdb, detailed);
	break;

      case NET_IGRP:
	display_route_igrp(p, le, pdb, detailed);
	break;

      case NET_ISIS:
	display_route_isis(p, le, pdb, detailed);
	break;

      case NET_NLSP:
	display_route_nlsp(p, le, pdb, detailed);
	break;

      default:
	display_route_unknown(p);
	break;

    }
    free_rdb_list(rdb_lh);
}

/*
 * Compare two route table network entries:
 */
static boolean cmp_ndb_nets (void *n1, void *n2)
{
    novndbtype *ndb1;
    novndbtype *ndb2;
    boolean result;

    ndb1 = (novndbtype *)n1;
    ndb2 = (novndbtype *)n2;

    result = (ndb1->dstnet < ndb2->dstnet);

    return(result);
}

/*
 * show_novriproutes
 * Display Novell routing information
 *
 * net = 0           => all routes
 * net = 1..FFFFFFFD => specific route
 * net = -1          => default routes only
 */

void show_novriproutes (ulong net, boolean detailed)
{
    novndbtype *p, *next_p;
    int i, ii, count = 0;
    boolean invalid_ndb = FALSE;
    ptable *rp;                         /* Route entry list */
    novndbtype *connected_ndb = NULL;   /* Connected networks*/
    novndbtype *static_ndb = NULL;      /* Static networks */
    novndbtype *internal_ndb = NULL;    /* Internal networks */
    novndbtype *dynamic_ndb = NULL;     /* Dynamic networks */
    novndbtype *mixed_ndb = NULL;       /* Arbitrary mix */
    int first_connected = 0, connected_count = 0;
    int first_static = 0, static_count = 0;
    int first_internal = 0, internal_count = 0;
    int first_dynamic = 0, dynamic_count = 0;
    int first_mixed = 0, mixed_count = 0;
    nov_sumroutetype *rnnode;
    ipxradixkeytype net_key;
    int total_ipx_routes;

    next_hop_column = -1;

    /*
     * Step through the table entry by entry, locking each. Use
     * temp pointer to create lists of internal, connected, and
     * static entries. Keep a total count. Finally, allocate an
     * array to hold pointers to all selected route entries.
     * Suspends are not allowed during this phase. This can be
     * changed ... however, a boolean will be necessary in order
     * to prevent reentrancy, which is not supported by the code
     * below (until the call on process_may_suspend).
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	if ((net != 0) && (i != NOV_NETHASH(net))) {
	    continue;
	}
	for (p = (*novell_table)[i]; p; p = p->next) {
	    if (validmem(p)) {
		if ((net != 0) && (net != p->dstnet)) {
		    continue;
		}
		inline_mem_lock(p);
		count++;
		p->temp_next = NULL;
		/*
		 * Make sure that everything that gets locked gets
		 * put on some list.
		 */
		if (p->flags & (NET_INTERNAL|NET_CONNECTED)) {
		    if (p->flags & NET_INTERNAL) {
			p->temp_next = internal_ndb;
			internal_ndb = p;
			internal_count++;
		    } else if (p->flags & NET_CONNECTED) {
			p->temp_next = connected_ndb;
			connected_ndb = p;
			connected_count++;
		    } else if (p->flags & NET_STATIC) {
			p->temp_next = static_ndb;
			static_ndb = p;
			static_count++;
		    }
		} else {
		    p->temp_next = mixed_ndb;
		    mixed_ndb = p;
		    mixed_count++;
		}
	    } else {
		invalid_ndb = TRUE;
		break;
	    }
	}
    }
    /*
     * Allocate an array of pointer to route entry large enough
     * to accomodate all routes. Attempt to back out gracefully
     * if this fails. Allocate at least one element even if no
     * routes have been selected for display.
     */
    rp = malloc((count+1)*sizeof(uchar *));

    /*
     * Build the array, or in rare circumstances, unlock all
     * route entries if the array allocation failed.
     */

    /*
     *  ... internal networks first:
     */
    ii = 0;
    p = internal_ndb;
    first_internal = ii;
    for (i = 0; i < internal_count; i++) {
	next_p = p->temp_next;
	if (rp != NULL) {
	    *rp[ii++] = p;
	} else {
	    free(p);
	}
	p = next_p;
    }
    /*
     * ... connected networks:
     */
    p = connected_ndb;
    first_connected = ii;
    for (i = 0; i < connected_count; i++) {
	next_p = p->temp_next;
	if (rp != NULL) {
	    *rp[ii++] = p;
	} else {
	    free(p);
	}
	p = next_p;
    }
    /*
     * ... static networks:
     */
    p = static_ndb;
    first_static = ii;
    for (i = 0; i < static_count; i++) {
	next_p = p->temp_next;
	if (rp != NULL) {
	    *rp[ii++] = p;
	} else {
	    free(p);
	}
	p = next_p;
    }
    /*
     * ... dynamic networks:
     */
    p = dynamic_ndb;
    first_dynamic = ii;
    for (i = 0; i < dynamic_count; i++) {
	next_p = p->temp_next;
	if (rp != NULL) {
	    *rp[ii++] = p;
	} else {
	    free(p);
	}
	p = next_p;
    }
    /*
     * Finally everything else:
     */
    p = mixed_ndb;
    first_mixed = ii;
    for (i = 0; i < mixed_count; i++) {
	next_p = p->temp_next;
	if (rp != NULL) {
	    *rp[ii++] = p;
	} else {
	    free(p);
	}
	p = next_p;
    }
    /*
     * Bail if the array malloc failed ... everything that was locked
     * has been released.
     */
    if (rp == NULL) {
	return;
    }
    /*
     * Pointer table complete ... all route tables entries are locked.
     * It is now safe to suspend.
     */
    total_ipx_routes = novell_count_routes();
    process_may_suspend();

    if (internal_count > 1) {
	nov_quicksort(&rp[first_internal], 0,
		      (internal_count-1), 0, cmp_ndb_nets);
    }
    if (connected_count > 1) {
	nov_quicksort(&rp[first_connected], 0,
		      (connected_count-1), 0, cmp_ndb_nets);
    }
    if (static_count > 1) {
	nov_quicksort(&rp[first_static], 0,
		      (static_count-1), 0, cmp_ndb_nets);
    }
    if (dynamic_count > 1) {
	nov_quicksort(&rp[first_dynamic], 0,
		      (dynamic_count-1), 0, cmp_ndb_nets);
    }
    if (mixed_count > 1) {
	nov_quicksort(&rp[first_mixed], 0,
		      (mixed_count-1), 0, cmp_ndb_nets);
    }

    automore_enable(NULL);
    
    printf("\nCodes: C - Connected primary network,    c - Connected secondary network");
    printf("\n       S - Static, F - Floating static, L - Local (internal), W - IPXWAN");
    printf("\n       R - RIP, E - EIGRP, N - NLSP, X - External, A - Aggregate");
    printf("\n       s - seconds, u - uses");
    printf("\n");
    printf("\n%d Total IPX routes.", total_ipx_routes);
    printf(" Up to %d parallel paths and %d hops allowed.",
	   novell_maxpaths, novell_infinity);
    if (novell_private_debug) {
	printf("\nInternal counts: %d=%d+%d+%d+%d+%d, invalid=%d"
	       ", cmp=%d, %d/%d", count, internal_count, connected_count,
	       static_count, dynamic_count, mixed_count, invalid_ndb,
	       compare_route_counts(), novell_route_count,
	       novell_really_count_routes());
    
    }
    novell_route_count = novell_really_count_routes();
     printf("\n");
    /*
     * Special handling for the default route:
     */
    if (!novell_default_route) {
      printf("\nDefault route handling is disabled.");
    } else {
      /*
       * Allow default route to be displayed if present regardless of
       * configuration ... in case there is a bug and the default route
       * is in the route table even though it shouldn't be.
       */
      p = novrip_find_rt(NOV_RIP_DEFAULT_ROUTE, NOV_NETMASK_ALL, 
			 IPX_ROUTE_MATCH_EXACT);
      if ((p != NULL) && validmem(p)) {
	mem_lock(p);
	if (novell_default_route) {
	  printf("\nCurrent default route is:\n");
	}
	display_route(p, detailed);
	free(p);
      } else {
	if (novell_default_route) {
	  printf("\nNo default route known.");
	}
      }
    }
    printf("\n");
    switch (net) {
    case 0:
	/* Display all routes */
	rn_walktree_blocking(nov_rnhead->rnh_treetop, display_patricia, &detailed);
	break;
    case -1:
	/* Display default route only */
	break;
    default:
	/*
	 * Display a specific route. But none is found. Find the longest match
	 * route and display it. This is a nice way to find out the route that
	 * is used to forward packets.
	 */
	if (count == 0) {
	    rnnode = (nov_sumroutetype *)
		rn_match(ipxaddr2radixkey(net, &net_key), nov_rnhead);
	    if (rnnode) 
		display_route(rnnode->ndb, detailed);
	}
	break;
    }
    /*
     * Step through the locked table entry by entry and display.
     * Select a single network if one was specified by the user.
     * Do not redisplay the default route if displayed above.
     */
    for (i = 0; i < count; i++) {

	if (automore_quit())
	    break;

	p = *rp[i];

	/*
	 * Skip over default route since it has already been
	 * displayed if present:
	 */
	if (novell_default_route && (p->dstnet == NOV_RIP_DEFAULT_ROUTE)) {
	    continue;
	}
	if ((novell_default_ndb != NULL) &&
	    (p->dstnet == novell_default_ndb->dstnet)) {
	    continue;
	}

	display_route(p, detailed);

	if (automore_quit())
	    break;
    }

    automore_disable();

    /*
     * Step through the list of locked entries and free everything:
     */
    for (i = 0; i < count; i++) {
	free(*rp[i]);
    }
    free(rp);
}

/*
 * host_hash
 *
 */
static ushort host_hash (ulong dnet, uchar *dhost)
{
    ushort indx;
    
    indx = (dnet & 0xffff);
    indx += (dnet >> 16);
    indx += *(ushort*)&dhost[0];
    indx += *(ushort*)&dhost[2];
    indx += *(ushort*)&dhost[4];
    indx += (indx >> 8);
    return(indx);
}

/*
 * first_hop
 * Determine first hop for a datagram based on destination address
 *
 * Note: Normally pass roundrobin as enabled in order to cycle through
 * multiple paths if available (load share). However, occasional
 * callers (e.g., novecho) want info about the path which is about
 * to be selected by nov_send(). These pass FALSE so that next_path
 * is *not* updated as a side effect and nov_send() uses the same path.
 */

static uchar *first_hop (ulong dnet,
			 uchar *dhost,
			 nidbtype **nif_output,
			 ulong *flags,
			 novndbtype *p,
			 enum IPX_ROUND_ROBIN roundrobin)
{
    novrdbtype *pp, *ppp, *temp_pp;
    int path_index;
    ushort indx;

    *nif_output = NULL;
    *flags = 0;
    ipx_pak_ndb_for_cache = NULL;

    /*
     * First hop may be called without the right net, just pointing in
     * the hash structure. Find our net.
     */
    while (p) {
	if (p->dstnet == dnet) {
	    /* Don't use a route that has been poisoned */
	    if (!TIMER_RUNNING(p->holddownuntil))
		break;
	    return(NULL);
	}
	p = p->next;
    }
    if (p == NULL)
	return(NULL);
    if (!(pp = ppp = p->nextpath))
	return(NULL);

    /*
     * Figure out which (NLSP) path this is. This hack is required since NLSP
     * will keep ALL the paths it knows about, not just novell_maxpaths.
     */
    path_index = p->paths;
    if (p->flags & NET_NLSP) {
	path_index = 0;
	for (temp_pp = p->path_list; temp_pp; temp_pp = temp_pp->next) {
	    path_index++;
	    if (path_index > novell_maxpaths) {
		pp = ppp = p->nextpath = p->path_list;
		break;
	    }
	    if (pp == temp_pp) {
		break;
	    }
	}
    }

    /*
     * If load-sharing per host, do something different. Hash the
     * destination and use the result to pick the SAME path each
     * time. Mark the chosen path using p->nextpath.
     */
    if (nov_load_share_per_host) {
	pp = ppp = p->nextpath = p->path_list;
	indx = host_hash(dnet, dhost);
	indx = (indx % path_index);
	if (indx != 0) {
	    while (indx++ < path_index) {
		pp = ppp = p->nextpath = pp->next;
		if (pp == NULL) {
		    pp = ppp = p->nextpath = p->path_list;
		    break;
		}
	    }
	}
    }
    
    /*
     * If the interface is not up then find one that is
     */
    for (;;) {
	if (!validmem(pp))
	    return(NULL);
	/*
	 * It is possible for static routes to have a 'pp' that points
	 * to a NULL nidb. Watch out!
	 */
	if (pp->nidb != NULL) {
	    if (validmem(pp->nidb) && interface_up(pp->nidb->idb) &&
		NOVELL_NIDB_SENDS_ALLOWED(pp->nidb))
		break;
	}
        if (!(pp = pp->next))
            pp = p->path_list;
        if (pp == ppp)
            return(NULL);
    }
    if ((roundrobin == IPX_ROUND_ROBIN_YES) && !nov_load_share_per_host) {
	if (!(p->nextpath = pp->next))
	    p->nextpath = p->path_list;
    }
    pp->uses++;
    *nif_output = pp->nidb;
    *flags = p->flags;
    ipx_pak_ndb_for_cache = p;
    return((p->flags & NET_CONNECTED) ? dhost : pp->srchost);
}

uchar *novrip_first_hop (ulong dnet,
			 uchar *dhost,
			 nidbtype **nif_output,
			 ulong *flags,
			 novndbtype *p,
			 enum IPX_ROUND_ROBIN roundrobin,
			 enum IPX_DEFAULT_ROUTE default_route)
{
    uchar *temp = NULL;
    nov_sumroutetype *rnnode;
    ipxradixkeytype net_key;

    /*
     * We still won't allow an explicit send to network 0/-1:
     * -2 is treated like a normal route if default handling off
     */
    if (NOV_ILLEGAL_NET(dnet)) {
	return(NULL);
    }
    /*
     * Do an explicit lookup of the requested net. Make sure the NDB
     * pointer is well-formed. Skip to default routes if no NDB at all.
     */
    if (p != NULL) {
	if (!validmem(p)) {
	    return(NULL);
	} else {
	    temp = first_hop(dnet, dhost, nif_output, flags, p, roundrobin);
	}
    }
    /*
     * If that fails and the caller demands it, do a lookup on
     * the default route.
     */
    if ((temp == NULL) || (*nif_output == NULL)) {	
	/*
	 * Look up the patricia tree for a summary route.
	 */
	rnnode = (nov_sumroutetype *)
	    rn_match(ipxaddr2radixkey(dnet, &net_key), nov_rnhead);
	if (rnnode) {
	    p = rnnode->ndb;
	    /*
	     * Trick first_hop into giving us the dstnet that 
	     * we already have:
	     */
	    temp = first_hop(p->dstnet, dhost, nif_output,
			     flags, p, roundrobin);
	}
	if ((temp == NULL) &&
	    (default_route == IPX_DEFAULT_ROUTE_YES) &&
	    novell_default_route &&
	    (novell_default_ndb != NULL)) {
	    p = novell_default_ndb;
	    /*
	     * Trick first_hop into giving us the dstnet that 
	     * we already have:
	     */
	    temp = first_hop(p->dstnet, dhost, nif_output,
			     flags, p, roundrobin);
	}
    }
    /*
     * Return what we can:
     */
    return(temp);
}

/* novrip_purgetable
 * Delete all dynamically learned entries in a routing table
 */
void
novrip_purgetable (void)
{
    int i;			/* Loop index */
    novndbtype *p;		/* Route data for a network */
    novndbtype *lastp;	/* Save last valid p for unlinking */
    novrdbtype *pp;		/* Path entry in chain attached to a "p" */
    novrdbtype *lastpp;	/* Save pp for deletion */

    /* Loop over all the hash chains */
    for (i=0; i < NOV_NETHASHLEN; i++) {
	lastp = NULL;
	p = (*novell_table)[i];
	while (p != NULL)
	    /* Only do routes that were learned dynamically */
	    if (p->flags & (NET_RIP | NET_IGRP | NET_NLSP | NET_ISIS)) {
		/* Start by throwing away all the path entries */
		pp = p->path_list;
		p->path_list = NULL;
		while (pp != NULL) {
		    lastpp = pp;
		    pp = pp->next;
		    novell_mark_rdb_changed(p, lastpp, p->flags,
					    IPX_ROUTE_EVENT_DELETED);
		    novrdb_free(lastpp);
		}
		if (novellrip_debug || novellrip_event_debug)
		  buginf("\nIPXRIP: Deleting network %x %x in table-wide purge",
			 p->dstnet, p->rt_mask);
		/* Unlink this network from the hash chain */
		if (lastp == NULL) {
		    (*novell_table)[i] = p->next;
		} else {
		    lastp->next = p->next;
		}
		p->options |= IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP;
		nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, FALSE);
		if (lastp == NULL) {
		    p = (*novell_table)[i];
		} else {
		    p = lastp->next;
		}
	    } else {
		/* Not unlinking; this net is valid and we keep it */
		lastp = p;
		p = p->next;
	    }
    }
    /* 
     * Purge the patricia tree as well
     */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, reset_patricia);

    /*
     * Assume we got at least one and dump the fastswitch cache:
     */
    novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0, NOV_NETMASK_ALL);
}

/*
 * novrip_network_command
 *
 * Process "network" command for a RIP routing process. A stub for now,
 * since it has no real effect.
 */
void novrip_network_command (novpdbtype *pdb, ulong net, boolean sense)
{
    return;
}

/*
 * novrip_exist_associated_statics
 *
 * Return TRUE if there are any static routes associated with this NIDB.
 */
boolean novrip_exist_associated_statics (nidbtype *nidb)
{
    int i;
    novndbtype *p;
    novrdbtype *pp;
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	p = (*novell_table)[i];
	for (; p; p = p->next) {
	    pp = p->path_list;
	    if ((p->flags & NET_STATIC) &&
		(pp != NULL) &&
		(pp->nidb == nidb)) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * novrip_delete_associated_statics
 *
 * Delete static routes associated with this NIDB, from both main and
 * floating table.
 */
void novrip_delete_associated_statics (nidbtype *nidb)
{
    int i;
    novndbtype *p, *next_p, *prevp;
    novrdbtype *pp;
    boolean changed = FALSE;
    
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	p = (*novell_table)[i];
	for (; p; p = next_p) {
	    next_p = p->next;
	    pp = p->path_list;
	    if ((p->flags & NET_STATIC) &&
		(pp != NULL) &&
		(pp->nidb == nidb)) {
		changed = TRUE;
		novrip_del_rt(p->dstnet, p->rt_mask, 
			      IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	}
	/* Delete items from the floating static table, as needed */
	for (p = (*static_table)[i], prevp = NULL; p != NULL; p = next_p) {
	    next_p = p->next;
	    pp = p->path_list;
	    if ((pp != NULL) &&
		(pp->nidb == nidb)) {
		free(pp);
		free(p);
		if (prevp == NULL) {
		    (*static_table)[i] = next_p;
		} else {
		    prevp->next = next_p;
		}
	    } else {
		prevp = p;
	    }
	}
    }
    /* Do the same for the patricia tree */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, 
			 patricia_delete_associated_statics, nidb);

    if (changed) {
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0, 
				   NOV_NETMASK_ALL);
    }
}

/*
 * nov_nonreliable_ndb_on_free_queue
 *
 * Return TRUE if the network given is on the ndb free queue of non-reliable
 * protocols.
 */
boolean nov_nonreliable_ndb_on_free_queue (ulong dstnet, ulong netmask)
{
    novndbtype *ndb;

    /*
     * Look for the thing on the freed route queue. If found, and it
     * is a non-IGRP/ISIS/NLSP route, do not accept route at this time.
     * This implements a non-IGRP route holddown period. Later, when ndb
     * is deleted, we will ask for this information again if it is
     * still valid.
     *
     */
    for (ndb = ((novndbtype *) nov_freed_ndbQ.qhead); ndb != NULL;
	 ndb = ndb->next) {
	if ((ndb->dstnet == dstnet) && (ndb->rt_mask == netmask) &&
	    (!(ndb->flags & (NET_IGRP | NET_ISIS | NET_NLSP)))) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * display_patricia
 */
int display_patricia (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    boolean *detailed = va_arg(ap, boolean *);

    sumroute = (nov_sumroutetype *)rnnode;
    if (sumroute) {
	display_route(sumroute->ndb, *detailed);
	return NOV_WALK_CONTINUE;
    }
    return NOV_WALK_ABORT;
}

/*
 * count_route_patricia
 */
int count_route_patricia (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    int *count = va_arg(ap, int *);

    sumroute = (nov_sumroutetype *)rnnode;
    if (sumroute) {
	(*count)++;
	return NOV_WALK_CONTINUE;
    }
    return NOV_WALK_ABORT;
}

/*
 * reset_patricia
 */
int reset_patricia (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *net;
    novndbtype *p;		/* Route data for a network */
    novrdbtype *pp;		/* Path entry in chain attached to a "p" */
    novrdbtype *lastpp; 	/* Save pp for deletion */

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    p = net->ndb;
    if (p->path_list && p->path_list->nidb == nullnidb) {
	return NOV_WALK_CONTINUE;
    } else if (p->flags & (NET_RIP | NET_IGRP | NET_NLSP | NET_ISIS)) {
	if (!rn_delete(&p->keylen, &p->masklen, nov_rnhead))
	    buginf("\nIPX: reset_patricia did not delete %x %x correctly",
		   p->dstnet, p->rt_mask);    
	/* Start by throwing away all the path entries */
	pp = p->path_list;
	while (pp != NULL) {
	    lastpp = pp;
	    pp = pp->next;
	    novell_mark_rdb_changed(p, lastpp, p->flags,
				    IPX_ROUTE_EVENT_DELETED);
	    novrdb_free(lastpp);
	}
	p->options |= IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP;
	nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, FALSE);
	free(net);
    }

    return NOV_WALK_CONTINUE;
}

/*
 * patricia_static_routes
 */
int patricia_static_routes (rntype *rnnode, va_list ap)
{
    int event = va_arg(ap, int);
    nidbtype *nidb = va_arg(ap, nidbtype *);
    nov_sumroutetype *net;
    novndbtype *p;		/* Route data for a network */
    novrdbtype *pp;		/* Path entry in chain attached to a "p" */
    boolean mark_changed = FALSE;
    boolean deleted = FALSE;
    uchar *rinp;
    int j;

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    p = net->ndb;
    pp = p->path_list;
    if ((!(p->flags & NET_STATIC)) || (pp == NULL) ||
	(pp->nidb != nidb) || (pp->nidb == NULL))
	return NOV_WALK_CONTINUE;

    if (event == ROUTE_UP) {
	if (((!(p->options & IPX_NDB_OPTION_VALID)) ||
	     TIMER_RUNNING(p->holddownuntil)) ||
	    nov_ipxwan_on_nidb(nidb))
	    mark_changed = TRUE;
	p->options |= IPX_NDB_OPTION_VALID;
	TIMER_STOP(p->holddownuntil);
	p->hopcount = p->ext_hop = 1;
	p->delay = p->ext_delay = 1;
	p->igrp_delay = 0;
        GET_TIMESTAMP(pp->timer);
	if (nov_ipxwan_on_nidb(nidb)) {
	    pp->srcnet = nidb->novellnet;
	    rinp = (uchar *) &(nidb->nov_ipxwan_remote_internal_net);
	    for (j = 0; j < 4; j++) {
		pp->srchost[j] = *rinp++;
	    }
	    pp->srchost[4] = pp->srchost[5] = 0;
	}
    } else {
	if ((p->options & IPX_NDB_OPTION_VALID) &&
	    (!TIMER_RUNNING(p->holddownuntil))) {
	    mark_changed = TRUE;
	    deleted = TRUE;
	}
	TIMER_START(p->holddownuntil, pp->nidb->idb->nov_update_interval);
    }
    if (mark_changed) {
	novell_mark_rdb_changed(p, pp, NET_STATIC,
				!deleted ?
				IPX_ROUTE_EVENT_ADD :
				IPX_ROUTE_EVENT_DELETED);
	novell_mark_rip_changed(p);
    }

    return NOV_WALK_CONTINUE;
}

/*
 * patricia_novrip_ager
 */
int patricia_novrip_ager (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *net;
    novndbtype *p;
    novrdbtype *pp, *next_pp;
    int pathcount;
    boolean null_pathlist = FALSE;

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    p = net->ndb;
    if (!(p->flags & NET_STATIC) && TIMER_RUNNING(p->holddownuntil)) {
	/* Route in holddown and not static */
	if (!SLEEPING(p->holddownuntil) &&
	    (p->options & IPX_NDB_OPTION_VALID)) {
	    pathcount = 0;
	    null_pathlist = (p->path_list == NULL);
	    pp = p->path_list;
	    p->path_list = NULL;
	    while (pp != NULL) {
		pathcount++;
		next_pp = pp->next;
		novell_mark_rdb_changed(p, pp, p->flags,
					IPX_ROUTE_EVENT_DELETED);
		novrdb_free(pp);
		pp = next_pp;
	    }
	    /*
	     * In holddown, we should have one path (except NLSP),
	     * or NO paths with a NULL path list. If not, complain.
	     */
	    if ((pathcount != p->paths) ||
		((pathcount != 0) && null_pathlist) ||
		((pathcount > 1) && (!(p->flags & NET_NLSP)))) {
		errmsg(&msgsym(BADPATHS, IPX), p->paths,
		       NOVELL_TYPE, p->dstnet);
	    }
	    if (novellrip_debug || novellrip_event_debug)
		buginf("\nIPXRIP: garbage collecting entry for %x %x",
		       p->dstnet, p->rt_mask);
	    p->options &= ~IPX_NDB_OPTION_VALID;
	    novell_routes--;
	    if (!rn_delete(&p->keylen, &p->masklen, nov_rnhead))
		buginf("\n%%IPX: patricia_novrip_ager did not delete %x %x correctly", 
		       p->dstnet, p->rt_mask);    
	    nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
	    free(net);
	}
    } else if ((p->flags & NET_STATIC) &&
	       TIMER_RUNNING_AND_AWAKE(p->holddownuntil)) {
	/*
	 * Perform processing specific to STATIC routes. A static
	 * route in holddown will age out in one update interval.
	 * It will exit holddown, become invalid, and may be then
	 * be learned dynamically until and unless the associated
	 * interface becomes active again.
	 */
	p->options &= ~IPX_NDB_OPTION_VALID;
	TIMER_STOP(p->holddownuntil);
    }
    return NOV_WALK_CONTINUE;
}

/*
 * patricia_shutdown
 *
 * Purge the route table "gracefully" as part of shutdown (allow
 * appropriate notifications as networks go down).
 */
int patricia_shutdown (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *net;
    novndbtype *p;
    novpdbtype *pdb;

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    p = net->ndb;

    if (novellnlsp_debug || novelligrp_debug ||
	novellrip_debug || novellrip_event_debug) {
	buginf("\nIPX: purge patricia tree");
    }
    TIMER_START(p->holddownuntil, 0);
    pdb = nov_proctable[p->novpdbindex];
    nov_network_redist(pdb, p, ROUTE_DOWN);

    return NOV_WALK_CONTINUE;
}

/*
 * patricia_delete_associated_statics
 *
 * Delete static routes associated with this NIDB.
 */
int patricia_delete_associated_statics (rntype *rnnode, va_list ap)
{
    nidbtype *nidb = va_arg(ap, nidbtype *);
    nov_sumroutetype *net;
    novndbtype *p;
    novrdbtype *pp;

    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    p = net->ndb;
    pp = p->path_list;
    if ((p->flags & NET_STATIC) &&
	(pp != NULL) &&
	(pp->nidb == nidb)) {
	novrip_del_rt(p->dstnet, p->rt_mask, 
		      IPX_ROUTE_DELETE_RETAIN_CONNECTED);
    }

    return NOV_WALK_CONTINUE;
}

