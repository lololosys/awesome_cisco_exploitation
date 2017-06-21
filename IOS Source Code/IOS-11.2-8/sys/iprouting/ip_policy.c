/* $Id: ip_policy.c,v 3.4.44.8 1996/07/30 06:09:30 dawnli Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ip_policy.c,v $
 *------------------------------------------------------------------
 * ip_policy.c -- Policy routing
 *
 * Feb. 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_policy.c,v $
 * Revision 3.4.44.8  1996/07/30  06:09:30  dawnli
 * CSCdi61869:  Wrong next-hop selection if set interface defined in
 * route-map.
 * Branch: California_branch
 * Define new route lookup routine based on a given interface.
 * Call it to set the right next-hop when route-map set interface
 * defined.
 *
 * Revision 3.4.44.7  1996/06/28  23:18:55  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.44.6  1996/06/28  01:20:48  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.4.44.5  1996/05/21  09:53:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.44.4  1996/05/14  16:03:06  albright
 * CSCdi57188:  Debug IP Policy Output is misleading / Cosmetic Bug
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/05/04  01:21:36  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.4.44.2  1996/04/16  19:01:31  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.44.1  1996/03/18  20:28:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/13  01:39:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.2  1996/03/07  09:49:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  14:29:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/17  18:30:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/28  09:31:48  tli
 * CSCdi44106:  IP policy routing can botch IP checksum
 * Use Eq. 4.  Really
 *
 * Revision 3.2  1995/11/17  17:34:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/19  08:05:57  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.4  1995/08/19  22:22:02  tli
 * CSCdi36983:  Need policy routing for locally generated packets
 *
 * Revision 2.3  1995/07/12  07:19:10  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.2  1995/06/08  08:59:16  tli
 * CSCdi35274:  route-map test-1 crashes IOS
 * - If a route map does not set any routing information, it should be
 *   equivalent to not having policy routed the packet.
 * - "match length" clauses were having their flag stepped on because the
 *   flag update routine didn't know about the clause.
 *
 * Revision 2.1  1995/06/07  21:06:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ip_actions.h"
#include "../ip/ipfast.h"
#include "ip_policy.h"
#include "route.h"
#include "route_map.h"
#include "iprouting_debug.h"
#include "../routing/routing_registry.h"

static boolean local_policy_enabled;
static char *local_policy_routemap;


/*
 * ip_policy_match_ipaddress
 * Match a packet against a list of access lists.
 */

static inline boolean ip_policy_match_ipaddress
    (paktype *pak, route_map_match_acl *match_list)
{
    route_map_match_acl *aptr;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (aptr = match_list; aptr; aptr = aptr->next) {
	if (ip_accesscheck(pak, aptr->acl)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * ip_policy_set_routing
 * Set up routing fields.  pak->desthost is only set here if the packet has
 * been source routed.  The order of evaluation here is crucial.  Return
 * TRUE if we set something.
 */

static inline boolean ip_policy_set_routing (paktype *pak, route_maptype *map)
{
    ipaddrtype dst, gateway;
    idbtype *outidb;
    iphdrtype *ip;
    route_map_interface *iptr;
    route_map_addrtype *aptr;
    boolean if_set;
    uchar obyte;

    ip = (iphdrtype *)ipheadstart(pak);
    if_set = FALSE;
    pak->flags |= PAK_ENCAPS_ONLY;
    dst = ip->dstadr;

    /*
     * Do accounting.
     */
    map->pkt_count++;
    map->byte_count += pak->datagramsize;

    /*
     * Mangle the TOS and Precedence fields as directed.
     */
    if (map->set_list.set_flags & (ROUTE_MAP_QOS_SET |
				   ROUTE_MAP_PRECEDENCE_SET)) {
	obyte = ip->tos;
	if (map->set_list.set_flags & ROUTE_MAP_QOS_SET) 
	    ip->tos = (ip->tos & ~IP_TOS_MASK) | map->set_list.qos;
	if (map->set_list.set_flags & ROUTE_MAP_PRECEDENCE_SET)
	    ip->tos = (ip->tos & ~IP_TOS_PRECEDENCE) |
		map->set_list.precedence;
	if (obyte != ip->tos) {

	    /*
	     * Things have changed, do the checksum fixup.  See RFC 1624.
	     */
	    ip->checksum = ip_checksum_adjust_inline(ip->checksum, obyte,
						     ip->tos);
	}
    }

    /*
     * First, see if we can set a next hop.
     */
    if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
	    if (aptr->address.type != ADDR_IP)
		continue;
	    outidb = samecable(aptr->address.ip_addr);
	    if (outidb) {
		pak->desthost = aptr->address.ip_addr;
		pak->if_output = outidb;
		return(TRUE);
	    }
	}
    }

    /*
     * Check all of the explicitly set interfaces.
     */
    if (map->set_list.set_flags & ROUTE_MAP_INTERFACE_SET) {
	for (iptr = map->set_list.interface; iptr; iptr = iptr->next) {
            outidb = iptr->idb;
	    if (interface_up_simple(outidb)) {
		gateway = iprouting_first_hop_on_idb(dst, outidb,
						     IP_PATH_DEFFLAGS);
		if (gateway == 0L) {
		    if (is_p2p(outidb))
			gateway = ip->dstadr;
		}
                if (gateway) {
                    pak->if_output = outidb;
                    pak->desthost = gateway;
                    return (TRUE);
                }
            }
	}
    }

    /*
     * If we don't care about the default settings, then don't do the
     * following very expensive computations.  If there is an explicit
     * route, then don't use the default configurations.
     */
    if ((map->set_list.set_flags & (ROUTE_MAP_DEFAULT_NEXT_HOP_SET |
				    ROUTE_MAP_DEFAULT_INTERFACE_SET))) {

	/*
	 * First check to see if default applies.
	 */
	outidb = reg_invoke_ip_first_hop(&dst, dst, IP_PATH_EXACT_MATCH);
	if (outidb)
	    return(if_set);

	if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_NEXT_HOP_SET) {
	    /*
	     * Walk the default next hop list.  If we have a useable next
	     * hop, and the interface has been set, then the next hop is
	     * ONLY usable if it points to the configured interface.
	     * Otherwise, use the interface associated with the next hop.
	     */
	    for (aptr = map->set_list.default_next_hop; aptr;
		 aptr = aptr->next) {
		outidb = samecable(aptr->address.ip_addr);
		if (!outidb)
		    continue;
		if (if_set && (outidb != pak->if_output))
		    continue;
		pak->if_output = outidb;
		pak->desthost = aptr->address.ip_addr;
		return(TRUE);
	    }
	}
	
	if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_INTERFACE_SET) {
	    for (iptr = map->set_list.default_interface; iptr;
		 iptr = iptr->next) {
		if (interface_up_simple(iptr->idb)) {
		    pak->if_output = iptr->idb;
		    if_set = TRUE;
		    break;
		}
	    }
	}
    }

    /*
     * If we've chosen an interface and not a next hop, try our destination
     * address.
     */
    if (if_set && !pak->desthost) {
	pak->desthost = ip->dstadr;
    }
    return(if_set);
}

/*
 * ip_policy_route_map_check
 * Evaluate a route map against a packet.
 */

static inline boolean ip_policy_route_map_check (paktype *pak,
						 route_map_headtype *head)
{
    route_maptype *map;

    for (map = (route_maptype *) head->map_queue.qhead; map; map =
	 map->next) {
	
	/*
	 * Match packets against access lists.
	 */
	if ((map->match_list.ip_address != NULL) && 
	    !ip_policy_match_ipaddress(pak, map->match_list.ip_address))
	    continue;

	/*
	 * Match packet length
	 */
	if ((map->match_list.match_flags & ROUTE_MAP_MATCH_LENGTH) &&
	    (!(pak->datagramsize >= map->match_list.min_length) ||
	     !(pak->datagramsize <= map->match_list.max_length)))
	    continue;

	/*
	 * If no matches failed, then this is a match.  If the item is a
	 * deny, then punt to normal forwarding.  Otherwise, process the
	 * set events.
	 */
	if (policy_debug) {
	    ipbug_no_outputif(pak, "policy match");
	    buginf("\nIP: route map %s, item %d, %s", head->map_tag,
		   map->seq_num, map->grant ? "permit" : "deny");
	}
	if (!map->grant) {
	    return(FALSE);
	}

	/*
	 * Set routing
	 */
	return(ip_policy_set_routing(pak, map));
    }
    return(FALSE);
}

/*
 * ip_policy_forward
 * Perform policy based forwarding.  If we decide to forward a packet based
 * on policy, return TRUE.
 */

boolean ip_policy_forward (paktype *pak)
{
    idbtype *idb;
    ipinfotype *ipinfo;
    route_map_headtype *head;
    char *mapname;
    boolean success;

    idb = pak->if_input;
    if (idb) {
	ipinfo = idb->ip_info;
	if (!ipinfo || !ipinfo->policy_enabled)
	    return(FALSE);
	mapname = ipinfo->policy_routemap;
    } else {

	/*
	 * Locally generated packets.
	 */
	if (!local_policy_enabled)
	    return(FALSE);
	mapname = local_policy_routemap;
    }

    /*
     * Ok, there's policy on the interface.  Grab the route map and
     * evaluate it.  Code stolen from route_map_ip_check().  This could be
     * optimized by caching this information.
     */
    head = find_route_map_head(mapname);
    if (!head)
	return(FALSE);

    success = ip_policy_route_map_check(pak, head);
    if (policy_debug) {
	if (success) {
	    ipbug(pak, "policy routed");
	    buginf("\nIP: %s to %s %i", idb ? idb->namestring : "local",
		   pak->if_output ? pak->if_output->namestring : "unknown",
		   pak->desthost);
	} else {
	    ipbug(pak, "policy rejected -- normal forwarding");
	}
    }
    return(success);
}

/*
 * ip_policy_command
 * Parse IP policy commands.
 */

void ip_policy_command (parseinfo *csb)
{
    idbtype *idb;
    ipinfotype *ipinfo;
    int len;
    hwidbtype *hwidb;
    boolean policy;

    idb = csb->interface;
    if (!idb)
	return;
    ipinfo = idb->ip_info;
    if (!ipinfo)
	return;
    hwidb = hwidb_or_null(idb);
    if (!hwidb)
	return;

    if (csb->nvgen) {
	if (ipinfo->policy_enabled)
	    nv_write(TRUE, "ip policy route-map %s", ipinfo->policy_routemap);
	return;
    }

    if (!csb->sense) {
	if (ipinfo->policy_routemap) {
	    free(ipinfo->policy_routemap);
	    ipinfo->policy_routemap = NULL;
	}
	ipinfo->policy_enabled = FALSE;

	/*
	 * Walk all interfaces.  If we now no longer have policy routing
	 * enabled on any interface, then disable the policy flag on all
	 * interfaces.
	 */
	policy = FALSE;
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_info && idb->ip_info->policy_enabled) {
		policy = TRUE;
		break;
	    }
	}

	if (!policy) {
	    FOR_ALL_HWIDBS(hwidb) {
		hwidb->ip_fast_flags &= ~IP_FAST_POLICY;
	    }
	}
	return;
    }

    /*
     * Enable policy routing and save the route map name.
     */
    ipinfo->policy_enabled = FALSE;
    if (ipinfo->policy_routemap) {
	free(ipinfo->policy_routemap);
	ipinfo->policy_routemap = NULL;
    }
    len = strlen(GETOBJ(string, 1)) + 1;
    ipinfo->policy_routemap = malloc(len);
    if (!ipinfo->policy_routemap) {
	return;
    }
    ipinfo->policy_enabled = TRUE;
    bcopy(GETOBJ(string, 1), ipinfo->policy_routemap, len);

    /*
     * Tell all hardware interfaces to check for policy.
     */
    FOR_ALL_HWIDBS(hwidb) {
	hwidb->ip_fast_flags |= IP_FAST_POLICY;
    }
}

/*
 * ip_local_policy_command
 * Parse IP policy commands.
 */

void ip_local_policy_command (parseinfo *csb)
{
    int len;

    if (csb->nvgen) {
	nv_write(local_policy_enabled, "%s route-map %s", csb->nv_command,
		 local_policy_routemap);
	return;
    }

    if (!csb->sense) {
	if (local_policy_routemap) {
	    free(local_policy_routemap);
	    local_policy_routemap = NULL;
	}
	local_policy_enabled = FALSE;
	return;
    }

    /*
     * Enable policy routing and save the route map name.
     */
    local_policy_enabled = FALSE;
    if (local_policy_routemap) {
	free(local_policy_routemap);
	local_policy_routemap = NULL;
    }
    len = strlen(GETOBJ(string, 1)) + 1;
    local_policy_routemap = malloc(len);
    if (!local_policy_routemap) {
	return;
    }
    local_policy_enabled = TRUE;
    bcopy(GETOBJ(string, 1), local_policy_routemap, len);
}

/*
 * route_map_set_default_int_command
 * Parse a "set default interface" command.
 */

void route_map_set_default_int_command (parseinfo *csb)
{
    route_maptype *map;
    route_map_interface *iptr, **pred, *next;
    idbtype *idb;

    map = csb->route_map;
    if (!map)
	return;
    
    idb = GETOBJ(idb, 1);
    pred = &map->set_list.default_interface;

    if (csb->sense) {
	if (!idb)
	    return;

	map->set_list.set_flags |= ROUTE_MAP_DEFAULT_INTERFACE_SET;
	for ( ; *pred; pred = &(*pred)->next) {
	    if ((*pred)->idb == idb)	/* already on list */
		return;
	}

	iptr = malloc(sizeof(route_map_interface));
	if (!iptr)
	    return;
	*pred = iptr;
	iptr->idb = idb;
    } else {

	if (!idb) {
	    for (iptr = *pred; iptr; iptr = next) {
		next = iptr->next;
		free(iptr);
	    }
	    map->set_list.default_interface = NULL;
	    map->set_list.set_flags &= ~ROUTE_MAP_DEFAULT_INTERFACE_SET;
	    return;
	}

	for ( ; *pred; pred = &(*pred)->next) {
	    if ((*pred)->idb == idb)
		break;
	}
		
	if (!*pred)			/* not found */
	    return;

	iptr = *pred;
	*pred = iptr->next;
	iptr->next = NULL;
	free(iptr);

	if (!map->set_list.default_interface)
	    map->set_list.set_flags &= ~ROUTE_MAP_DEFAULT_INTERFACE_SET;
    }
}

/*
 * route_map_set_default_nh_command
 * Parse a "set ip default next-hop" command.
 */

void route_map_set_default_nh_command (parseinfo *csb)
{
    route_maptype *map;
    ipaddrtype ipaddr;
    route_map_addrtype **pred, *aptr;

    map = csb->route_map;
    if (!map)
	return;
    
    ipaddr = GETOBJ(paddr,1)->ip_addr;
    pred = &map->set_list.default_next_hop;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_DEFAULT_NEXT_HOP_SET;
	for ( ; *pred; pred = &(*pred)->next) {
	    if ((*pred)->address.ip_addr == ipaddr) /* already on list */
		return;
	}

	aptr = malloc(sizeof(route_map_addrtype));
	if (!aptr)
	    return;
	*pred = aptr;
	bcopy(GETOBJ(paddr,1), &aptr->address, sizeof(addrtype));
    } else {
	for ( ; *pred; pred = &(*pred)->next) {
	    if ((*pred)->address.ip_addr == ipaddr)
		break;
	}
		
	if (!*pred)			/* not found */
	    return;

	aptr = *pred;
	*pred = aptr->next;
	aptr->next = NULL;
	free(aptr);

	if (!map->set_list.default_next_hop)
	    map->set_list.set_flags &= ~ROUTE_MAP_DEFAULT_NEXT_HOP_SET;
    }
}

/*
 * route_map_set_ip_tos_command
 * Parse a "set ip tos" command.
 */

void route_map_set_ip_tos_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;
    if (!map)
	return;
    
    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_QOS_SET;
	map->set_list.qos = GETOBJ(int,1) << IP_TOS_SHIFT;
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_QOS_SET;
	map->set_list.qos = IP_TOS_NORMAL << IP_TOS_SHIFT;
    }
}    

/*
 * route_map_set_ip_precedence_command
 * Parse a "set ip precedence" command.
 */

void route_map_set_ip_precedence_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;
    if (!map)
	return;
    
    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_PRECEDENCE_SET;
	map->set_list.precedence = GETOBJ(int,1) << IP_PRECEDENCE_SHIFT;
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_PRECEDENCE_SET;
	map->set_list.precedence = TOS_PREC_ROUTINE << IP_PRECEDENCE_SHIFT;
    }
}    

/*
 * route_map_match_length_command
 * Parse a "match length" clause.
 */

void route_map_match_length_command (parseinfo *csb)
{
    route_maptype *map;
    int min, max, tmp;

    map = csb->route_map;
    if (!map)
	return;

    if (csb->sense) {
	min = GETOBJ(int, 1);
	max = GETOBJ(int, 2);
	if (min > max) {
	    tmp = min;
	    min = max;
	    max = tmp;
	}
	map->match_list.min_length = min;
	map->match_list.max_length = max;
	map->match_list.match_flags |= ROUTE_MAP_MATCH_LENGTH;
    } else {
	map->match_list.min_length = 0;
	map->match_list.max_length = 0;
	map->match_list.match_flags &= ~ROUTE_MAP_MATCH_LENGTH;
    }
}

/*
 * ip_policy_route_map_set_nvgen
 * NVgen IP policy route map clauses.
 */

static void ip_policy_route_map_set_nvgen (parseinfo *csb,
					   route_maptype *map)
{
    route_map_interface *iptr;
    route_map_addrtype *aptr;

    if (map->set_list.set_flags & ROUTE_MAP_QOS_SET) {
	nv_write(TRUE, "%s ip tos ", csb->nv_command);
	nv_add_id_string(tos_values, map->set_list.qos >>
			 IP_TOS_SHIFT); 
    }

    if (map->set_list.set_flags & ROUTE_MAP_PRECEDENCE_SET) {
	nv_write(TRUE, "%s ip precedence ", csb->nv_command);
	nv_add_id_string(precedence_values, map->set_list.precedence >>
			 IP_PRECEDENCE_SHIFT); 
    }

    if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_INTERFACE_SET) {
	nv_write(TRUE, "%s default interface", csb->nv_command);
	iptr = map->set_list.default_interface;
	for ( ; iptr; iptr = iptr->next) {
	    nv_add(TRUE, " %s", iptr->idb->namestring);
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_NEXT_HOP_SET) {
	nv_write(TRUE, "%s ip default next-hop", csb->nv_command);
	aptr = map->set_list.default_next_hop;
	for ( ; aptr; aptr = aptr->next) {
	    nv_add(TRUE, " %i", aptr->address.ip_addr);
	}
    }
}

/*
 * ip_policy_route_map_match_nvgen
 * NVgen IP policy route map clauses.
 */

static void ip_policy_route_map_match_nvgen (parseinfo *csb,
					     route_maptype *map)
{
    if (map->match_list.match_flags & ROUTE_MAP_MATCH_LENGTH) {
	nv_write(TRUE, "%s length %d %d", csb->nv_command,
		 map->match_list.min_length, map->match_list.max_length);
    }
}

/*
 * ip_policy_show_route_map_set
 * Show IP policy route map set clauses.
 */

static void ip_policy_show_route_map_set (route_maptype *map)
{
    route_map_interface *iptr;
    route_map_addrtype *aptr;

    if (map->set_list.set_flags & ROUTE_MAP_QOS_SET) {
	printf("\n    ip tos ");
	printf_id_string(tos_values, map->set_list.qos >>
			 IP_TOS_SHIFT); 
    }

    if (map->set_list.set_flags & ROUTE_MAP_PRECEDENCE_SET) {
	printf("\n    ip precedence ");
	printf_id_string(precedence_values, map->set_list.precedence >>
			 IP_PRECEDENCE_SHIFT); 
    }

    if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_INTERFACE_SET) {
	printf("\n    default interface");
	iptr = map->set_list.default_interface;
	for ( ; iptr; iptr = iptr->next) {
	    mem_lock(iptr);
	    printf(" %s", iptr->idb->namestring);
	    if (mem_refcount(iptr) == 1) {
		free(iptr);
		break;
	    } else
		free(iptr);
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_DEFAULT_NEXT_HOP_SET) {
	printf("\n    ip default next-hop");
	aptr = map->set_list.default_next_hop;
	for ( ; aptr; aptr = aptr->next) {
	    mem_lock(aptr);
	    printf(" %i", aptr->address.ip_addr);
	    if (mem_refcount(aptr) == 1) {
		free(aptr);
		break;
	    } else
		free(aptr);
	}
    }

    /*
     * Print accounting
     */
    printf("\n  Policy routing matches: %lu packets, %lu bytes",
	   map->pkt_count, map->byte_count);
}

/*
 * ip_policy_show_route_map_match
 * Show IP policy route map match clauses.
 */

static void ip_policy_show_route_map_match (route_maptype *map)
{
    if (map->match_list.match_flags & ROUTE_MAP_MATCH_LENGTH) {
	printf("\n    length %d %d", map->match_list.min_length,
	       map->match_list.max_length);
    }
}

/*
 * show_ip_local_policy_command
 * Show the current configuration of local policy routing.
 */

void show_ip_local_policy_command (parseinfo *csb)
{
    route_map_headtype *head;

    printf("Local policy routing is %sabled",
	   local_policy_enabled ? "en" : "dis");
    if (local_policy_enabled) {
	printf(", using route map %s", local_policy_routemap);
	head = find_route_map_head(local_policy_routemap);
	if (!head) {
	    printf("\nRoute map %s not found", local_policy_routemap);
	} else {
	    display_one_route_map(head);
	}
    }
}

/*
 * show_ip_policy_command
 * Show the current configuration of policy routing.
 */

static const char show_ip_policy_fmt[] = "\n%15s%s";

void show_ip_policy_command (parseinfo *csb)
{
    idbtype *idb;
    ipinfotype *ipinfo;

    automore_enable("Interface      Route map");
    if (local_policy_enabled) {
	printf(show_ip_policy_fmt, "local", local_policy_routemap);
    }
    FOR_ALL_SWIDBS(idb) {
	ipinfo = idb->ip_info;
	if (!ipinfo || !ipinfo->policy_enabled || !ipinfo->policy_routemap) { 
	    continue;
	}
	printf(show_ip_policy_fmt, idb->namestring, ipinfo->policy_routemap);
    }
    automore_disable();
}

/*
 * ip_policy_subsys_init
 * Initialize the IP policy routing subsystem.
 */

static void ip_policy_subsys_init (subsystype *subsys)
{
    local_policy_enabled = FALSE;
    local_policy_routemap = NULL;

    reg_add_ip_policy_forward(ip_policy_forward, "ip_policy_forward");

    reg_add_route_map_set_nvgen(ip_policy_route_map_set_nvgen,
				  "ip_policy_route_map_set_nvgen");
    reg_add_route_map_match_nvgen(ip_policy_route_map_match_nvgen,
				  "ip_policy_route_map_match_nvgen");
    reg_add_route_map_show_match(ip_policy_show_route_map_match,
				 "ip_policy_show_route_map_match");
    reg_add_route_map_show_set(ip_policy_show_route_map_set,
			       "ip_policy_show_route_map_set");

    ip_policy_parser_init();
}

/* 
 * IP policy subsystem header
 */

#define IP_POLICY_MAJVERSION 1
#define IP_POLICY_MINVERSION 0
#define IP_POLICY_EDITVERSION 1

SUBSYS_HEADER(ip_policy, IP_POLICY_MAJVERSION, IP_POLICY_MINVERSION,
	      IP_POLICY_EDITVERSION, ip_policy_subsys_init,
	      SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, iprouting, routing",
	      "req: iphost, iprouting, routing");
