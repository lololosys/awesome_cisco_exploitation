/* $Id: path.c,v 3.3.54.3 1996/09/10 03:22:53 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/path.c,v $
 *------------------------------------------------------------------
 * path.c -- IP routing using ICMP redirects
 *
 * 25-July-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: path.c,v $
 * Revision 3.3.54.3  1996/09/10  03:22:53  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.3.54.2  1996/06/28  23:31:06  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/04/15  21:23:48  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/22  17:49:43  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.2  1995/11/17  18:55:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/20  18:06:20  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.3  1995/07/14  07:03:18  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.2  1995/06/20 04:14:48  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.1  1995/06/07  23:09:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../iprouting/route.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "config.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "../iprouting/gdp.h"


boolean ip_host_routing;	/* TRUE if path routing enabled */

/*
 * Path Descriptor
 */

typedef struct pathtype_ {
	struct pathtype_ *next;		/* pointer to next path in chain */
	ipaddrtype address;		/* host address */
	ipaddrtype gateway;		/* gateway, or zero if none */
	idbtype	*idb;			/* pointer to associated IDB */
	sys_timestamp timer;		/* time out for obsolesence */
	long uses;			/* number of datagrams routed here */
} pathtype;

#define PATHTIMEOUT (4*ONEHOUR)

pathtype *pathtable[NETHASHLEN];


/*
 * path_init
 * Initialize the path routing structure
 */

void path_init (void)
{
    long i;

    for (i = 0; i < NETHASHLEN; i++)
	pathtable[i] = NULL;
    reg_add_ar_cache_clear(ip_redirect_flush_cache, "ip_redirect_flush_cache");
}

/*
 * find_path
 *
 * Figure out the first hop address of an IP datagram using the path
 * structure If defaultflag is TRUE, return default idb if no extant
 * path.
 * Never return an idb that isn't up.
 */
idbtype *find_path (
    ipaddrtype *address,
    ulong flags)
{
    pathtype *path;
    idbtype *idb;
    boolean arp_ok;

    /*
     * We should only do anything if path support is explicitly enabled,
     * no IP routing process is running, or the system is not yet up.
     */

    if (!ip_host_routing && ip_router_running && systeminit_flag)
      return(NULL);

    /*
     * Don't inadvertently route IP multicast packets along default idb.
     * This is required in systems that don't support IP multicast forwarding.
     */
    if (ip_host_routing && IPMULTICAST(*address))
	return(NULL);

    /*
     * If this is a broadcast address on any interface, then don't
     * attempt to route the packet.
     */

    FOR_ALL_SWIDBS(idb)
      if (idb->ip_enabled)
	if (ip_broadcast(*address, idb) || ip_multicast(*address, idb))
	  return(NULL);

    /*
     * At this point, look up the destination in the path table
     */

    path = pathtable[nethash(*address)];
    while (path) {
	if ((path->address == *address) && path->gateway &&
	    interface_up(path->idb)) {
	    *address = path->gateway;
	    GET_TIMESTAMP(path->timer);
	    path->uses++;
	    return(path->idb);
	}
	path = path->next;
    }

    if (flags & IP_PATH_EXACT_MATCH)
	return(NULL);

    /*
     * Determine an interface.  Note that we may be any sort of
     * cisco creature who does not do IP routing.
     * - lookup in the ARP cache for a clue, any clue
     * - see if we're on the same cable
     * - maybe use a default gateway
     * - pick a random interface and pray.
     */
    arp_ok = (flags & IP_PATH_USE_PROXY);
    /*
     * If no IP routing turned on, don't proxy arp out, we will
     * use default-gw (dead or alive); unless the system is
     * booting up.
     */
    idb = ip_arp_route(*address, arp_ok &&
		       (ip_router_running || !systeminit_flag));

    /*
     * If the address is not in the ARP cache and we are a bridge
     * with a default gateway, determine if this address is off our
     * local subnet.  If it is, use the address of the default gateway
     * and check if *it* is in our ARP cache.
     *
     * We find first port for which we are connected.  Note
     * we assume that all interfaces are on the same cable
     * since we are bridging IP.  If we have heard from
     * someone we will have an entry in the ARP cache and
     * won't be here.
     */
    if (!idb)
	idb = samecable(*address);
    if (!(flags & IP_PATH_USE_IF_GUESS))
	return(idb);
    if (!idb && default_gateway) {
	*address = default_gateway;
	idb = samecable(*address);
	if (idb) {
	    if (((idb->hwptr->status & IDB_IEEEMASK) == 0) && !idb->arp_smds) {
		/*
		 * Default gateway reachability does not require an ARP
		 * table entry.
		 */
		return(idb);
	    }
	}
	idb = ip_arp_route(*address, arp_ok);
    }
    if (!idb)
	idb = pick_defaultidb(*address);
    if (idb && !interface_up(idb))
	idb = NULL;
    return(idb);
}

/*
 * first_hop_no_routing
 */

idbtype *first_hop_no_routing (
    ipaddrtype *address,
    ipaddrtype destination,
    ulong flags)
{
    flags |= IP_PATH_USE_IF_GUESS;
    return(find_path(address, flags));
}
/*
 * path_update
 * Update the routing table
 */

void path_update (ulong address, ulong gateway, idbtype *idb)
{
    pathtype *path;
    long index;

    if (martian(address, TRUE) || martian(gateway, TRUE))
	return;
    /*
     * If we receive a redirect to the same place
     * probably MAC level address changed so we invalidate
     * and just return
     */
    if (address == gateway) {
	ip_arp_invalidate(address);
	return;
    }
    index = nethash(address);
    path = pathtable[index];
    while (path) {
	if (path->address == address)
	    break;
	path = path->next;
    }
    if (!path) {
	path = malloc(sizeof(pathtype));
	if (!path)
	    return;
	path->next = pathtable[index];
	pathtable[index] = path;
    }
    path->address = address;
    path->gateway  = gateway;
    GET_TIMESTAMP(path->timer);
    path->idb = idb;
}

/*
 * path_ager
 * Paw through routing table, discarding old entries.
 * If flushflag is TRUE, flush all entries.
 */

static void path_ager (boolean flushflag)
{
    register pathtype *path, *pred;
    register long i;

    for (i = 0; i < NETHASHLEN; i++) {
	pred = (pathtype *) & pathtable[i];
	path = pred->next;
	while (path) {
	    if (flushflag || !CLOCK_IN_INTERVAL(path->timer, PATHTIMEOUT)) {
		pred->next = path->next;
		free(path);
		path = pred;
	    } else {
                pred = path;
            }
	    path = path->next;     
	}
    }
    if (flushflag && reg_invoke_ip_gdpclient_enable()) {
	default_gateway =
	    reg_invoke_ip_gdpclient_pick_defaultgw(default_gateway);
        reg_invoke_ip_defaultgw_change((ipaddrtype) default_gateway);
    }
}

/*
 * ip_redirect_age_cache
 * Periodic aging of ICMP redirect cache
 */

void ip_redirect_age_cache (void)
{
    path_ager(FALSE);
}

/*
 * ip_redirect_flush_cache
 * Unconditionally flush the ICMP redirect cache
 */

void ip_redirect_flush_cache (idbtype* idb)
{
    path_ager(TRUE);
}

/*
 * show_paths
 * Dump the redirect routing table to the screen
 */

static const char rtehead[] =
      "Host               Gateway           Last Use    Total Uses  Interface";

void show_paths (parseinfo *csb)
{
    int i, minute, hr;
    long header;
    ipaddrtype address;
    pathtype *path;
  
    automore_enable(NULL);
    address = GETOBJ(paddr,1)->ip_addr;
    reg_invoke_ip_gdpclient_show();
    printf("\nDefault gateway is ");
    if (default_gateway)
	printf("%i\n", default_gateway);
    else
	printf("not set\n");
    automore_header(rtehead);
    header = FALSE;
    for (i = 0; i < NETHASHLEN; i++) {
	path = pathtable[i];
	while (path) {
	    if (address && (path->address != address)) {
		path = path->next;
		continue;
	    }
	    header = TRUE;
	    minute = ELAPSED_TIME(path->timer)/ONEMIN;
	    if (minute < 0) minute = -minute;
	    hr = minute/60; minute %= 60;
	    printf("\n%19i%17i%-6d:%-02d    %-10d  %s",
		path->address, path->gateway, hr, minute,
		path->uses, path->idb->namestring);
	    path = path->next;
	}
    } 
    if (!header)
	printf("\nICMP redirect cache is empty");
    automore_disable();
}

/*
 * clear_redirect_cache - Clear out the contents of the redirect cache
 *
 */

void clear_redirect_cache (parseinfo *csb)
{
    ip_redirect_flush_cache(NULL);
}
