/* $Id: ipstatic.c,v 3.7.12.2 1996/09/12 13:31:37 fox Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipstatic.c,v $
 *------------------------------------------------------------------
 * ipstatic.c	-- IP static routing
 *
 *	Feb 3, 1992, Tony Li
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *
 * $Log: ipstatic.c,v $
 * Revision 3.7.12.2  1996/09/12  13:31:37  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.7.12.1  1996/08/03  23:30:23  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7  1996/02/15  17:26:37  lol
 * CSCdi49026:  Crash in ip_delete_per_user_route()
 *
 * Revision 3.6  1996/01/22  06:14:25  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/04  22:15:05  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.4  1995/12/18  23:49:24  lol
 * CSCdi43005:  route attribute is not processed for sync ISDN
 *
 * Revision 3.3  1995/12/10  00:42:37  pst
 * CSCdi38750:  Allow static routes with both next hop and interface
 *              also add "permanent" keyword for non-flapping static routes
 *
 * Revision 3.2  1995/11/17  17:34:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:06:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/09  16:20:55  albright
 * CSCdi35180:  subnet mask of next GW for static routes not checked if
 * default defi
 *
 * Revision 2.2  1995/06/09  13:06:18  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:07:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "route.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"


/*
 * ip_static_update
 * For a single static route, send an update to the main IP routing table.
 * Return TRUE if the static route is added or else return FALSE.
 */

boolean ip_static_update (ipstatictype *sre)
{
    ipaddrtype dummy, nexthop;
    idbtype *idb;
    boolean delete, changed;
    ndbtype *ndb;

    if (!ip_router_running || !systeminit_flag || !netsareup)
	return(TRUE);

    /*
     * Don't add a static route through an interface that is down.
     */
    delete = FALSE;
    idb = sre->interface;
    if (idb) {
	/*
	 * This is the "route X serial 0" case.
	 */
	/*
	 * does the interface have IP enabled on it?
	 */
	if (!idb->ip_unnumbered && !idb->ip_address && (idb != nullidb))
	    delete = TRUE;
	else
	    delete = !interface_up(idb);
    } else {
	/*
	 * This is the "route X Y" case.
	 */
	nexthop = sre->gateway;
	if (!nexthop) {
	    if (!ndb_zero || !ndb_zero->rdb[0])
		delete = TRUE;
	    else {
		nexthop = ndb_zero->rdb[0]->gateway;
		idb = ndb_zero->rdb[0]->idb;
	    }
	}

	if (!delete) {
	    if (!idb) {
		dummy = nexthop;
		idb = reg_invoke_ip_first_hop(&dummy, dummy,
					      IP_PATH_SUBDEFAULT);
		/*
		 * If there is a route, insure that we aren't installing a
		 * recursive route.  There's no perfect way of detecting
		 * this, but this is a good start.  If the next hop is
		 * 0.0.0.0, ignore this check, as someone is probably
		 * pointing subnet default to global default.
		 */
		if (idb && nexthop) {
		    ndb = ip_rtlookup(nexthop, 0); /* longest match */
		    if (ndb) {
			
			/*
			 * If the ndb is a route for the next hop, check to
			 * see if it's _also_ a route covering the static.
			 * If so, don't install.
			 * If ndb is connected, then allow the route so
			 * that we arp for sre->gateway rather than 
			 * ndb->rdb->gateway.
			 */
			if (!(ndb->attrflags & NET_ATTACHED) &&
			    ((ndb->netsmask & sre->network) ==
			     ndb->netnumber) &&
			    (LONGEST_MASK(ndb->netsmask, sre->netsmask) ==
			     sre->netsmask))
			    idb = NULL;
		    }
		}
	    }
	    if (!idb)
		idb = samecable(nexthop);
	    if (!idb || !interface_up(idb))
		delete = TRUE;
	}
    }

    if (delete) {
	/*
	 * The route is unusable.
	 */
	if (!(sre->flags & NET_PERMANENT)) {
	    ip_route_delete(pdb_STATIC, sre->network, sre->netsmask,
			    sre->gateway, IPADDR_ZERO, idb, 0);
	    network_revise(sre->network);
	}
	return(FALSE);
    }

    pdb_STATIC->static_distance = sre->distance;
    
    network_update(pdb_STATIC, sre->network, sre->netsmask,
                   sre->gateway, sre->interface, 0L,
                   sre->flags, 0, sre->tag, &changed, 0);
    if (changed) {
	network_revise(sre->network);
        process_set_boolean(ip_statics_unsynced, TRUE);
	return(TRUE);
    } else
	return(FALSE);
}

/*
 * ip_addstatic
 * Add a static route.  Assume that it has already been sanity checked.
 */

void ip_addstatic (ipaddrtype network, ipaddrtype mask, ipaddrtype gateway,
		   idbtype *idb, ulong tag, long flags, long distance, 
		   hwidbtype *hwidb)
{
    ipstatictype *sre, *p, *last_sre;
    int bucket;

    /*
     * Add or update static route entries.
     */
    for (sre = (ipstatictype *) ipstaticQ.qhead; sre; sre = sre->next) {
	if (sre->network == network) {
	    if (mask != sre->netsmask)
		continue;
	    if (distance != sre->distance)
		continue;
	    if (gateway && (gateway != sre->gateway))
		continue;
	    if (idb && (idb != sre->interface))
		continue;
	    break;
	}
    }
    if (sre) {
	sre->tag = tag;			/* tag could have been changed. */
	if (hwidb)
	    sre->user_hwidb = hwidb;	/* last interface to add is owner */
    } else {
	/*
	 * Add new entry into the linked list of static routes.
	 */
	sre = malloc(sizeof(ipstatictype));
	if (!sre)
	    return;

	sre->network = network;
	sre->gateway = gateway;
	sre->netsmask = mask;
	sre->interface = idb;
	sre->flags = flags;
	sre->distance = distance;
	sre->tag = tag;
	sre->user_hwidb = hwidb;

	last_sre = NULL;
	for (p = (ipstatictype *) ipstaticQ.qhead; p; p = p->next) {
	    if ((p->network > network) ||
		((p->network == network) && (p->netsmask > mask)) ||
		((p->network == network) && (p->netsmask == mask) &&
		 (p->distance > distance)))
		break;
	    last_sre = p;
	}
	insqueue(&ipstaticQ, sre, last_sre);

	/*
	 * Add the new static structure to the static table.
	 */
	last_sre = NULL;
	bucket = nethash(network);
	for (p = ipstatictable[bucket]; p; p = p->hashnext) {
	    if ((p->network > network) ||
		((p->network == network) && (p->netsmask > mask)) ||
		((p->network == network) && (p->netsmask == mask) &&
		 (p->distance > distance)))
		break;
	    last_sre = p;
	}
	if (last_sre) {
	    sre->hashnext = last_sre->hashnext;
	    sre->hashprev = last_sre;
	    if(last_sre->hashnext)
		last_sre->hashnext->hashprev = sre;
	    last_sre->hashnext = sre;
	} else {
	    sre->hashnext = ipstatictable[bucket];
	    if (ipstatictable[bucket])
		ipstatictable[bucket]->hashprev = sre;
	    ipstatictable[bucket] = sre;
	}
    }
    ip_static_update(sre);
}

/*
 * ip_delink_static
 * Delink a static route structure from the static table.
 * Called before deleting a static entry.
 */
static void ip_delink_static (ipstatictype *sre)
{
    if (sre->hashnext)
	sre->hashnext->hashprev = sre->hashprev;
    
    /*
     * If hashprev is NULL, we are delinking the first entry in a
     * hashbucket.
     */
 
   if (sre->hashprev)
	sre->hashprev->hashnext = sre->hashnext;
    else
	ipstatictable[nethash(sre->network)] = sre->hashnext;

    sre->hashnext = sre->hashprev = NULL;
}

/* 
 * Delete all per-user static routes which "belong" to the interface denoted
 * by idb. Typically because IPCP has declared it down. Used by Tacacs+.
 */
void
ip_delete_per_user_route (hwidbtype *idb)
{
    ipstatictype *sre, *sre_next;

    for (sre = (ipstatictype *) ipstaticQ.qhead; sre; sre = sre_next) {
	sre_next = sre->next;

	if (!(sre->flags & NET_PER_USER) || (sre->user_hwidb != idb)) {
	    continue;
	}

	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s delete route=%i %i %i",
		   idb->hw_namestring,
		   sre->network, sre->netsmask, sre->gateway);
	}

	ip_delink_static(sre);
	unqueue(&ipstaticQ, sre);
	ip_route_delete(pdb_STATIC, sre->network, sre->netsmask,
			sre->gateway, IPADDR_ZERO,
			sre->interface, 0);
	free(sre);
    }
}

/*
 * ip_deletestatic
 * Delete a static route.  Assume that it has already been sanity checked.
 * Helper routine to eliminate need for outside callers to have knowledge
 * of pdb_STATIC. 
 */
void ip_deletestatic (ipaddrtype network, ipaddrtype mask, idbtype *idb)
{
    ipstatictype *sre, *sre_next;

    for (sre = (ipstatictype *) ipstaticQ.qhead; sre; sre = sre_next) {
	sre_next = sre->next;
	if (sre->network == network) {
	    if (mask != sre->netsmask)
		continue;
	    if (idb && (idb != sre->interface))
		continue;
	    ip_delink_static(sre);
	    unqueue(&ipstaticQ, sre);
	    ip_route_delete(pdb_STATIC, network, sre->netsmask,
			    sre->gateway, IPADDR_ZERO,
			    sre->interface, 0);
	    free(sre);
	}
    }
}

/*
 * ip_static_deleteidb
 * Delete all interface static routes because said interface is going away.
 */
void ip_static_deleteidb (idbtype *idb)
{
    ipstatictype *sre, *sre_next;

    for (sre = (ipstatictype *)ipstaticQ.qhead; sre; sre = sre_next) {
	sre_next = sre->next;
	if (sre->interface == idb) {
	    ip_delink_static(sre);
	    unqueue(&ipstaticQ, sre);
	    ip_route_delete(pdb_STATIC, sre->network, sre->netsmask,
			    sre->gateway, IPADDR_ZERO,
			    sre->interface, 0);
	    free(sre);
	}
    }
}

/*
 * ip_route_nvgen
 * Write static routes. For the sake of cpu, process MAX_ENTRY_PER_NVGEN 
 * entries at a time. As ipstaticQ can change during suspend, we 
 * search the queue using the full key <network, mask, distance> to 
 * figure out the first entry for next run. 
 */

static void ip_route_nvgen (parseinfo *csb)
{
    ipaddrtype network, mask;
    long distance;
    ipstatictype *sre, *p;
    int entry;

    entry = 0;	
    sre = (ipstatictype *)ipstaticQ.qhead;
    while (sre) {
	/*
	 * Don't nvgen an internally created static route or a per-user
	 * static route installed from tacacs+
	 */
	if (sre->flags & (NET_INTERNAL|NET_PER_USER)) {
	    sre = sre->next;
	    continue; 
	}

	nv_write(TRUE, "%s %i %i",
		 csb->nv_command, sre->network, sre->netsmask);
	if (sre->interface)
	    nv_add(TRUE, " %s", sre->interface->namestring);
	if (sre->gateway || !sre->interface)
	    nv_add(TRUE, " %i", sre->gateway);
	nv_add(sre->distance != STATIC_DISTANCE, " %d", sre->distance);
	nv_add(sre->tag, " tag %u", sre->tag);
	nv_add(sre->flags & NET_PERMANENT, " permanent");
	if (entry++ < MAX_ENTRY_PER_NVGEN) {
	    sre = sre->next;
	    continue;
	}

	/* record the entry we last processed */
	network = sre->network;
	mask = sre->netsmask;
	distance = sre->distance;
	process_may_suspend();

	/* find the proper entry to start again */
	for (p = (ipstatictype *) ipstaticQ.qhead; p; p = p->next) {
	    if (p->network < network)
		continue;	/* redundant but for optimization */
	    if ((p->network > network) ||
		((p->network == network) && (p->netsmask > mask)) ||
		((p->network == network) && (p->netsmask == mask) &&
		 (p->distance > distance)))
		break;
	}
	sre = p;
	entry = 0;
    }
    return;
}

/*
 * ip_route_command
 * Add a static routing entry.
 *	 ip route <network> <gateway>
 *	 ip route <network> <interface name> <unit>
 *
 * The later form allows multiple networks/subnets on an interface.
 * This command now allows overlapping entries.  A full key for an entry is
 * <network> <mask> <distance>.
 */

void ip_route_command (parseinfo *csb)
{
    ipaddrtype network, gateway, mask;
    long flags, distance;
    ulong tag;
    idbtype *idb;
    ipstatictype *sre, *sre_next;
    boolean gavedistance, gatewayset;

    if (!router_enable)			/* quit if not a router */
	return;

    if (csb->nvgen) {
	/*
	 * If static routing entries exist, write them to memory also.
	 */
	ip_route_nvgen(csb);
	return;
    }

    network = GETOBJ(paddr,1)->ip_addr;
    mask    = GETOBJ(paddr,2)->ip_addr;
    gateway = 0;
    gatewayset = FALSE;
    distance = STATIC_DISTANCE;
    gavedistance = FALSE;
    idb = NULL;
    flags = 0;
    tag = GETOBJ(int, 3);

    /* We have an interface */
    if (ISOBJ(idb,1)) {
	idb = GETOBJ(idb,1);
    }
    
    if ((GETOBJ(paddr,3))->type == ADDR_IP) {
	gateway = GETOBJ(paddr,3)->ip_addr;
	gatewayset = TRUE;
    } else {
        if (!idb && csb->sense) {
	   printf("%% Must specify a next hop IP address or interface");
	   return;
        }
    }

    /*
     * Static routes with both an interface and a gateway should behave like
     * rip learned routes, while static routes without an explicit gateway
     * should behave acording to historical cisco behavior (I think this is
     * broken, we never should have set NET_ATTACHED for static routes).
     *
     * -- pst 9dec1995
     */
    if (idb && !gatewayset)
	flags |= NET_ATTACHED;

    gavedistance = GETOBJ(int,2);
    if (gavedistance)
	distance = GETOBJ(int,1);

    if (GETOBJ(int,4))
	flags |= NET_PERMANENT;

    /*
     * Sanity check mask against address
     */
    if (!good_route_mask(network, mask, subnetzero_enable)) {
	printf("%%Inconsistent address and mask");
	return;
    }

    /*
     * If this is a delete, remote the static route entry from our linked
     * list. 
     */
    if (csb->sense == FALSE) {		/* handle "no route a.b.c.d" */
	boolean found_one = FALSE;	/* Set if we matched an entry */
	for (sre = (ipstatictype *) ipstaticQ.qhead; sre; sre = sre_next) {
	    sre_next = sre->next;
	    if (sre->network == network) {
		if (mask != sre->netsmask)
		    continue;
		if (gatewayset && (gateway != sre->gateway))
		    continue;
		if (idb && (idb != sre->interface))
		    continue;
		if (gavedistance && (distance != sre->distance))
		    continue;
		/* 
		 * Don't allow user to delete a internally generated
		 * static route.
		 */
		if (sre->flags & NET_INTERNAL) 
		    continue;  
		found_one = TRUE;
		ip_delink_static(sre);
		unqueue(&ipstaticQ, sre);
		ip_route_delete(pdb_STATIC, network, sre->netsmask,
				sre->gateway, IPADDR_ZERO,
				sre->interface, 0);
		free(sre);
	    }
	}
	if (!found_one)
	    printf("%%No matching route to delete");
	return;
    }

    if (CLASSD_AND_GREATER(network)) {
        printf("%%Invalid destination prefix");
        return;
    }
    /*
     * This must be an address.  Special case 0.0.0.0
     */
    if (gatewayset && (gateway != 0L) && martian(gateway, FALSE)) {
	printf("%%Invalid next hop address");
	return;
    }

    /*
     * Make sure we can't add a static route whose first hop is one of
     * our addresses.
     */
    if (gatewayset && ip_ouraddress(gateway)) {
	printf("%%Invalid next hop address (it's this router)");
	return;
    }

    ip_addstatic(network, mask, gateway, idb, tag, flags, distance, NULL); 
}

/*
 * ip_static_adjust
 * Walk linked list of IP static routes, revising them in the main routing
 * table as we go.
 */

void ip_static_adjust (void)
{
    ipstatictype *sre, *pred;

    process_set_boolean(ip_statics_unsynced, FALSE);
    for (sre = (ipstatictype *)ipstaticQ.qhead; sre;) {
	ip_static_update(sre);
	mem_lock(sre);
	process_may_suspend();
	pred = sre;
	sre = sre->next;
	free(pred);
    }
}

/*
 * adjust_static_routes
 * Schedule revision of static routes.
 *   - invoked when an interface goes up.
 *   - invoked by table_init to add back the static routes.
 *   - invoked periodically at interface keepalive time
 *     (even if keepalives are disabled...)
 */

void adjust_static_routes (pdbtype *pdb, idbtype *idb)
{
    process_set_boolean(ip_statics_unsynced, TRUE);
}

/*
 * ipstatic_callbackup_route
 * Call back to install the static route when a better distance route
 * went down.
 */
void ipstatic_callbackup_route (pdbtype *pdb, ipaddrtype address,
				ipaddrtype mask, backuptype *backup)
{
    ipstatictype *p;
    
    /*
     * The static entries in the table are ordered by distance.
     * Try the entries that has the matching network/mask.
     */
    for (p = ipstatictable[nethash(address)]; p; p = p->hashnext)
	if ((p->network == address) && (p->netsmask == mask)) {
	    if (ip_static_update(p))
		return;
	}
}
