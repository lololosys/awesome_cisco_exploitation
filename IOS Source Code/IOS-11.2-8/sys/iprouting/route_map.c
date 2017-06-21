/* $Id: route_map.c,v 3.4.10.11 1996/07/09 07:35:58 dawnli Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route_map.c,v $
 *------------------------------------------------------------------
 * route_map.c -- Route map support routines.
 *
 * 23-January-1993
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route_map.c,v $
 * Revision 3.4.10.11  1996/07/09  07:35:58  dawnli
 * CSCdi57006:  route-map in configuration are stored/displayed in reverse
 * order
 * Branch: California_branch
 *
 * Revision 3.4.10.10  1996/06/28  23:19:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.10.9  1996/06/05  08:27:06  rchandra
 * CSCdi59547:  Routemap: %SYS-4-REGEXP: NULL string parameter error
 * Branch: California_branch
 * - do not pass null path to route_map_match_aspath
 *
 * Revision 3.4.10.8  1996/06/03  23:24:12  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.4.10.7  1996/05/04  01:22:39  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.4.10.6  1996/04/16  19:02:21  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.10.5  1996/04/05  03:46:40  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.4.10.4  1996/04/02  07:29:30  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.4.10.3  1996/03/28  23:59:18  rchandra
 * CSCdi52098:  BGP: exact keyword not accepted by route-map in community
 * match
 * Branch: California_branch
 * - fix my silly bug in handling exact community matching
 * - limit the range of valid community to 0xFFFF0000
 * - fix a cosmetic bug in show ip community-list
 *
 * Revision 3.4.10.2  1996/03/28  22:28:25  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.4.10.1  1996/03/18  20:29:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:58:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  09:50:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  01:04:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/24  02:01:55  pst
 * CSCdi49774:  modularize regexp's for bgp (part 3)
 *
 * Revision 3.3  1996/02/01  06:06:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  17:37:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/25  08:31:19  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/07/06  23:12:39  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.4  1995/06/23  14:26:05  widmer
 * CSCdi36318:  multiple route-map commands are not accepted
 * Fix route-map command NV generation
 *
 * Revision 2.3  1995/06/15  15:18:52  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/08  08:59:19  tli
 * CSCdi35274:  route-map test-1 crashes IOS
 * - If a route map does not set any routing information, it should be
 *   equivalent to not having policy routed the packet.
 * - "match length" clauses were having their flag stepped on because the
 *   flag update routine didn't know about the clause.
 *
 * Revision 2.1  1995/06/07  21:10:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "string.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_route_map.h"
#include "../util/sorted_array.h"
#include "../clns/clns.h"
#include "../clns/clns_externs.h"
#include "../ip/ip.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../iprouting/route_map.h"
#include "../iprouting/bgp.h"
#include "../clns/isis_externs.h"	
#include "../ip/ip_registry.h"
#include "../routing/routing_registry.h"
#include "route.h"
#include "igrp_common.h"


/*
 * Storage.
 */
route_map_headtype *route_maps[NETHASHLEN];

extern regexpacctype *as_path_accesslists[];

static const id_string match_array[] = {
    {ROUTE_MAP_MATCH_IPADDRESS, "network"},
    {ROUTE_MAP_MATCH_IPNEXTHOP, "nexthop"},
    {ROUTE_MAP_MATCH_IPROUTESOURCE, "route source"},
    {ROUTE_MAP_MATCH_CLNSADDRESS, "clns address"},
    {ROUTE_MAP_MATCH_CLNSNEXTHOP, "clns nexthop"},
    {ROUTE_MAP_MATCH_CLNSROUTESOURCE, "clns route source"},
    {ROUTE_MAP_MATCH_INTERFACE, "interface"},
    {ROUTE_MAP_MATCH_METRIC, "metric"},
    {ROUTE_MAP_MATCH_ROUTETYPE, "route type"},
    {ROUTE_MAP_MATCH_TAG, "tag"},
    {ROUTE_MAP_MATCH_ASPATH, "as path"},
    {ROUTE_MAP_MATCH_COMMUNITY, "community"},
    {ROUTE_MAP_MATCH_EXACT_COMMUNITY, "exact community"},
    {ROUTE_MAP_MATCH_LENGTH, "length"},
    {0, NULL}				/* should always be the last element */
};
	     
static const id_string set_array[] = {
    {ROUTE_MAP_METRIC_SET, "metric"},
    {ROUTE_MAP_METRICTYPE_SET, "metric type"},
    {ROUTE_MAP_TAG_SET, "tag"},
    {ROUTE_MAP_AUTOTAG_SET, "automatic tag"},
    {ROUTE_MAP_LEVEL_SET, "level"},
    {ROUTE_MAP_DEST_SET, "destination"},
    {ROUTE_MAP_LOCALPREF_SET, "local preference"},
    {ROUTE_MAP_WEIGHT_SET, "weight"},
    {ROUTE_MAP_NEXT_HOP_SET, "nexthop"},
    {ROUTE_MAP_ORIGIN_SET, "origin"},
    {ROUTE_MAP_DAMPENING_SET, "dampening"},
    {ROUTE_MAP_ASPATH_PND_SET, "as-path prepend"},
    {ROUTE_MAP_COMMUNITY_SET, "community"},
    {ROUTE_MAP_COMMUNITY_ADD, "community add"},
    {ROUTE_MAP_ASPATH_TAG_SET, "aspath tag"},
    {ROUTE_MAP_DEFAULT_NEXT_HOP_SET, "default nexthop"},
    {ROUTE_MAP_DEFAULT_INTERFACE_SET, "default interface"},
    {ROUTE_MAP_INTERFACE_SET, "interface"},
    {ROUTE_MAP_QOS_SET, "qos"},
    {ROUTE_MAP_PRECEDENCE_SET, "precedence"},
    {ROUTE_MAP_DESTPREF_SET, "destination preference"},
    {0, NULL}				/* should always be the last element */
};

#define	ROUTE_MAP_PATHSIZE	200


/*
 * route_map_command
 *
 * Process route-map command. Syntax is:
 *
 * [no] route-map <tag> [permit | deny] [<seq-num>]
 *
 * OBJ(string,1) == route_map tag.
 * OBJ(int,1)    == TRUE for "permit", FALSE for "deny"
 * OBJ(int,2)    == sequence number (< 0 means non specify)
 */
void route_map_command (parseinfo *csb)
{
    route_map_headtype *head;
    route_maptype      *map;
    int                 seq_num;

    map = NULL;

    /*
     * Find head of route-map queue.
     */
    head = find_route_map_head(GETOBJ(string,1));
    seq_num = GETOBJ(int,2);

    if (csb->sense) {
	if (!head) {
	    head = route_map_head_add(GETOBJ(string,1));
	    if (!head) {
		return;
	    }
	}
        if (seq_num < 0) {

            /*
             * Get the only entry.
             */
            map = head->map_queue.qhead;
            if (map && map->next) {

                /*
                 * <seq-num> is required when multiple entries defined.
                 */
                printf("\n%% Please specify the entry by its sequence");
                return;
            }
        } else {

            /*
             * Get specific route-map entry based on sequence number supplied.
             */
            map = find_route_map(head, seq_num);
        }

        /*
         * If route-map does not exist, create one. Set csb to point
         * to route-map for subsequent subcommands.
         */
	map = route_map_add(head, map, GETOBJ(int,1), seq_num);
	if (!map)
	    return;
	route_map_inform_protocols(head->map_tag, ROUTE_MAP_ADD);
	if (set_mode_byname(&csb->mode, "route-map", MODE_VERBOSE)) {
	    csb->route_map = map;
	}
    } else {
        if (!head) {
            printf("\n%% Could not find route-map %s", GETOBJ(string,1));
            return;
        }
	map = find_route_map(head, seq_num);

	if (!map && (seq_num >= 0)) {
	    printf("\n%% Could not find route-map entry %s %d", 
		   GETOBJ(string,1), seq_num);
	    return;
	}

	/*
	 * Deletes route-map. Returns TRUE if a route-map head is
	 * removed. 
	 * Depending on the returned value inform protocols with
	 * appropriate event.
	 */
	if (route_map_delete(head, map))
	    route_map_inform_protocols(GETOBJ(string,1), ROUTE_MAP_DELETE);
	else
	    route_map_inform_protocols(GETOBJ(string,1), ROUTE_MAP_MODIFY);
	csb->route_map = NULL;
    }
}

/*
 * route_map_nvgen
 *
 * NV generate all "route-map" commands and associated subcommands.
 */

void route_map_nvgen (parseinfo *csb)
{
    route_map_headtype *head;
    route_maptype      *map;
    int                i;
    char *nv_command = NULL;

    setstring(&nv_command, csb->nv_command);
    /*
     * Set up parse chain to NVGEN route-maps and associated subcommands.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (head = route_maps[i]; head; head = head->next) {

            for (map = (route_maptype *) head->map_queue.qhead; map; 
                 map = map->next) {
                nv_write(TRUE, "%s %s %s %d", csb->nv_command,
                         map->head->map_tag,
                         (map->grant) ? "permit" : "deny",
                         map->seq_num);
                if (set_mode_byname(&csb->mode, "route-map", MODE_VERBOSE)) {
                    *csb->nv_command = '\0';
                    csb->route_map = map;
                    recurse_node(csb, get_mode_nv(csb));
                    strcpy(csb->nv_command, nv_command);
                }
            }
	}
    }
    free(nv_command);
}

/* 
 * route_map_head_add
 *
 * Add a route-map head entry. This is the first entry for the route-map
 * set.
 */
route_map_headtype *route_map_head_add (uchar *map_tag)
{
    route_map_headtype *head;
    route_map_headtype **ptr;
    int                len;

    /*
     * Adding a new map.
     */
    head = malloc(sizeof(route_map_headtype));
    if (!head) {
	return(NULL);
    }
    len = strlen(map_tag) + 1;
    head->map_tag = malloc(len);
    if (!head->map_tag) {
	free(head);
	return(NULL);
    }
    bcopy(map_tag, head->map_tag, len);

    /*
     * Insert in hash table.
     */
    ptr = &route_maps[route_map_hash(map_tag)]; 
    while (*ptr) {
	ptr = &(*ptr)->next;
    }
    *ptr = head;
    
    return(head);
}

/*
 * route_map_add
 *
 * Add specific route-map entry to set. This function supports replacing
 * an existing entry.
 */
route_maptype *route_map_add (route_map_headtype *head,
			      route_maptype      *map,
			      boolean            grant,
			      int                seq_num)
{
    route_maptype *map2, *tmp;
    int i;
    boolean       new_map;

    new_map = FALSE;
    if (!map) {
	map = malloc(sizeof(route_maptype));
	if (!map) {
	    return(NULL);
	}
	new_map = TRUE;
        map->seq_num = (seq_num < 0) ? ROUTE_MAP_INIT_SEQ : seq_num;
    }

    map->head = head;
    map->grant = grant;

    /*
     * Link new map entry in.
     */
    if (new_map) {
	for (map2 = (route_maptype *) head->map_queue.qhead; map2;
	     map2 = map2->next) {
	    if (seq_num > map2->seq_num) {
		if (!map2->next) {
		    enqueue(&head->map_queue, map);
		    goto exit;
		}
		if (seq_num < map2->next->seq_num) {
		    break;
		}
	    }
	}

	/*
	 * Insert in middle of list.
	 */
	if (map2) {
	    map->next = map2->next;
	    map2->next = map;
	    head->map_queue.count++;
	} else {

	    /*
	     * Reset head of list.
	     */
	    requeue(&head->map_queue, map);
	}
    }

    /*
     * As a new map is added recreate the index. 
     */
 exit:
    tmp = (route_maptype *)head->map_queue.qhead;
    for (i = 0; tmp; tmp = tmp->next, i++)
	tmp->index = i;

    return(map);
}

/*
 * route_map_delete
 *
 * Delete specific route-map entry or entire set.
 * Return TRUE if the whole route-map is removed 
 */
boolean route_map_delete (route_map_headtype *head, route_maptype *map)
{
    route_map_headtype **pptr;
    route_maptype *tmp, *next;
    int i;

    if (!map) {

        /*
         * delete all entries in the queue
         */
        for (map = head->map_queue.qhead; map; map = next){
            next = map->next;
            route_map_free_data(map);
            free(map);
        }
        head->map_queue.qhead = NULL;
        head->map_queue.count = 0;
    } else {
        unqueue(&head->map_queue, map);
        route_map_free_data(map);
        free(map);
    }

    if (!head->map_queue.count) {

	/*
	 * Delink from hash table.
	 */
	pptr = &route_maps[route_map_hash(head->map_tag)];
	for ( ; *pptr; pptr = &(*pptr)->next) {
	    if (head == *pptr) {
		break;
	    }
	}
	*pptr = head->next;
	free(head->map_tag);
	free(head);
	return(TRUE);
    } else {

	/*
	 * As a route-map entry has been removed, recreate the index.
	 */
	tmp = (route_maptype *)head->map_queue.qhead;
	for (i = 0; tmp; tmp = tmp->next, i++)
	    tmp->index = i;
    }

    return(FALSE);
}

/*
 * route_map_sanity_check 
 * Check if the match/set in the route-map is supported in this
 * usage. If it is not supported, display appropriate error message. 
 */
void route_map_sanity_check (route_map_nametype *user)
{
    route_map_headtype *head;
    const id_string *match, *set;

    if (!user || 
 	!user->head ||			/* route-map not defined */
 	!user->username)		/* user info not present */
 	return;

     head = user->head;

    /*
     * Walk thru the match/set array.  
     * If route-map has the match/set and userinfo does not support
     * it, issue relevant warning message. 
     */
    for (match = match_array; match->id; match++) {
 	if ((head->match_flags & match->id) &&
 	    !(user->match_flags & match->id))
 	    printf("\n%% \"%s\" used as %s route-map, %s match not supported",
 		   user->mapname, user->username, match->str);
    }

    for (set = set_array; set->id; set++) {
 	if ((head->set_flags & set->id) &&
 	    !(user->set_flags & set->id))
 	    printf("\n%% \"%s\" used as %s route-map, set %s not supported",
 		   user->mapname, user->username, set->str);
    }
}

/*
 * route_map_config
 */
boolean route_map_config (route_map_nametype **pptr, char *mapname,
			  const route_map_userinfo *userinfo)
{
    route_map_nametype *map;

    map = *pptr;

    if (!*mapname)
	return(TRUE);

    if (!map) {
	map = malloc(sizeof(route_map_nametype));
	if (!map) {
	    printf(nomemory);
	    return(FALSE);
	}
    }
    setstring(&(map->mapname), mapname);
    if (!map->mapname)				/* malloc failed */
	return(FALSE);
    map->head = find_route_map_head(mapname);
    if (userinfo) {
 	map->set_flags = userinfo->set_flags;
 	map->match_flags = userinfo->match_flags;
 	setstring(&(map->username), userinfo->username);
	if (!map->username)
	    return(FALSE);
    }

    route_map_sanity_check(map);
    *pptr = map;
    return(TRUE);
}

/*
 * route_map_deconfig
 */
void route_map_deconfig (route_map_nametype **map)
{
    if (*map) {
	free((*map)->mapname);
	free((*map)->username);
	free(*map);
	*map = NULL;
    }
}

/*
 * route_map_free_acl_item
 *
 * Free each item of a particular match list.
 */
static void route_map_free_acl_item (route_map_match_acl **acl_ptr)
{    
    route_map_match_acl *acl_item, *next;

    acl_item = *acl_ptr;
    
    for ( ; acl_item; acl_item = next) {
	next = acl_item->next;
	free(acl_item);
    }
    *acl_ptr = NULL;
}

/*
 * route_map_free_data
 *
 * Free the match/set data.
 */
void route_map_free_data (route_maptype *map)
{
    route_map_matchtype   *match;

    match = &map->match_list;

    route_map_free_acl_item(&match->ip_address);
    route_map_free_item(&match->clns_address);
    route_map_free_acl_item(&match->ip_next_hop);
    route_map_free_item(&match->clns_next_hop);
    route_map_free_acl_item(&match->ip_route_source);
    route_map_free_item(&match->clns_route_source);
    route_map_free_item(&match->metric);
    route_map_free_item(&match->route_type);
    route_map_free_item(&match->tag);
    route_map_free_item((route_map_match_value **) &match->interface);
    free(map->set_list.as_path);
    route_map_free_item(&match->community);
    free(map->set_list.community);
}

/*
 * route_map_free_item
 *
 * Free each item of a particular match list.
 */
void route_map_free_item (route_map_match_value **value_ptr)
{    
    route_map_match_value *value, *next;

    value = *value_ptr;
    
    for ( ; value; value = next) {
	next = value->next;
	free(value);
    }
    *value_ptr = NULL;
}

/*
 * route_map_match_free_address
 */
void route_map_match_free_address (route_maptype *map, int kind, int protocol)
{
    route_map_match_value **pptr;
    route_map_match_acl **aptr;

    if (protocol) {		/* CLNS */

	switch (kind) {
	case ROUTEMAP_ADDRESS:
	    pptr = &map->match_list.clns_address;
	    break;
	case ROUTEMAP_NEXTHOP:
	    pptr = &map->match_list.clns_next_hop;
	    break;
	case ROUTEMAP_ROUTESOURCE:
	    pptr = &map->match_list.clns_route_source;
	    break;
	default:
	    return;
	}

	route_map_free_item(pptr);
    } else {			/* IP */
	switch (kind) {
	case ROUTEMAP_ADDRESS:
	    aptr = &map->match_list.ip_address;
	    break;
	case ROUTEMAP_NEXTHOP:
	    aptr = &map->match_list.ip_next_hop;
	    break;
	case ROUTEMAP_ROUTESOURCE:
	    aptr = &map->match_list.ip_route_source;
	    break;
	default:
	    return;
	}

	route_map_free_acl_item(aptr);
    }
}

/*
 * find_route_map_head
 * Locate route-map entry based on tag name. This locates the first entry
 * in a set of route-maps that are sorted by sequence number.
 */
route_map_headtype *find_route_map_head (uchar *name)
{
    route_map_headtype *head;

    for (head = route_maps[route_map_hash(name)]; head; head = head->next) {
	if (!strcmp(name, head->map_tag)) {
	    break;
	}
    }
    return(head);
}

/*
 * find_route_map
 *
 * Find specific route-map entry using supplied sequence number.
 * Return NULL if sequence number is -1, or not supplied.
 */
route_maptype *find_route_map (route_map_headtype *head, int seq)
{
    route_maptype *map;

    if (!head || seq < 0) {
	return(NULL);
    }
    map = (route_maptype *) head->map_queue.qhead;
    for ( ; map; map = map->next) {
        if (map->seq_num == seq) {
            break;
        }
    }
    return(map);
}

/*
 * route_map_hash
 *
 * Hash algorithm for route-map array. If the array length is 
 * modified (currently NETHASHLEN), this function must be modified.
 */
ulong route_map_hash (uchar *tag)
{
    int i, len, hash;
    
    hash = 0;
    len = strlen(tag);

    for (i = 0; i < len; i++) {
	hash ^= *tag++;
    }
    hash &= 0xff;

    return(hash);
}

/*
 * route_map_match_interface_command
 *
 * Process "match interface <unit-number>" command.
 */
void route_map_match_interface_command (parseinfo *csb)
{
    route_maptype             *map;
    route_map_interface *interface;
    route_map_interface **pptr;
    
    if (!(map = csb->route_map)) {
	return;
    }

    pptr = &map->match_list.interface;
    if (csb->sense) {
	interface = (route_map_interface *) 
	    malloc(sizeof(route_map_interface));
	if (!interface) {
	    return;
	}

	interface->idb = GETOBJ(idb,1);
	while (*pptr) {
	    pptr = &(*pptr)->next;
	}
	*pptr = interface;
    } else {
	for ( ; *pptr; pptr = &(*pptr)->next) {
	    if ((*pptr)->idb == GETOBJ(idb,1)) {
		break;
	    }
	}
	if (!*pptr)			/* Not found */
	    return;
	interface = *pptr;
	*pptr = interface->next;
	free(interface);
    }
}

/*
 * route_map_match_metric_command
 */
void route_map_match_metric_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    route_map_process_match_item(&map->match_list.metric, GETOBJ(int,2),
				 csb->sense);
}

/*
 * route_map_match_tag_command
 */
void route_map_match_tag_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    route_map_process_match_item(&map->match_list.tag, GETOBJ(int,2),
				 csb->sense);
}

/*
 * route_map_match_routetype_command
 */
void route_map_match_routetype_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    
    route_map_process_match_item(&map->match_list.route_type, GETOBJ(int,2),
				 csb->sense);
}

/*
 * route_map_match_address_command
 */
void route_map_match_address_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    if (!GETOBJ(int,1)) {
	route_map_process_match_acl_item(&map->match_list.ip_address, 
					 GETOBJ(int,2),
					 GETOBJ(string,1),
					 csb->sense, GETOBJ(int,3));
    } else {
	route_map_process_match_sitem(&map->match_list.clns_address, 
				      GETOBJ(string,1), csb->sense);
    }
}

/*
 * route_map_match_nexthop_command
 */
void route_map_match_nexthop_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    if (!GETOBJ(int,1)) {
	route_map_process_match_acl_item(&map->match_list.ip_next_hop, 
					 GETOBJ(int,2),
					 GETOBJ(string,1),
					 csb->sense, GETOBJ(int,3));
    } else {
	route_map_process_match_sitem(&map->match_list.clns_next_hop,
				      GETOBJ(string,1),	csb->sense);
    }
}

/*
 * route_map_match_routesource_command
 */
void route_map_match_routesource_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    if (!GETOBJ(int,1)) {
	route_map_process_match_acl_item(&map->match_list.ip_route_source,
					 GETOBJ(int,2), GETOBJ(string,1),
					 csb->sense, GETOBJ(int,3));
    } else {
	route_map_process_match_sitem(&map->match_list.clns_route_source,
				      GETOBJ(string,1), csb->sense);
    }
}

/*
 * route_map_process_match_item
 */
void route_map_process_match_item (route_map_match_value **pptr,
				   ulong                 value,
				   boolean               sense)
{
    route_map_match_value *number;

    if (sense) {
	while (*pptr) {
	    if ((*pptr)->value == value) {
		return;
	    }
	    pptr = &(*pptr)->next;
	}

	number = malloc(sizeof(route_map_match_value));
	if (!number) {
	    return;
	}

	number->value = value;
	*pptr = number;
    } else {
	for ( ; *pptr; pptr = &(*pptr)->next) {
	    if ((*pptr)->value == value) {
		break;
	    }
	}
	if (*pptr) {
	    number = *pptr;
	    *pptr = number->next;
	    free(number);
	}
    }
}

/*
 * route_map_process_match_sitem
 *
 * Store string item in match list.
 */
void route_map_process_match_sitem (route_map_match_value **pptr,
				    char *str,
				    boolean sense)
{
    route_map_match_value *value;
    ulong                 index;

    if (sense) {
	/*
	 * Get filter index for OSI filter.
	 */
	index = reg_invoke_ip_clns_filter_create(str, 0);
	if (!index) {
	    return;
	}

	/*
	 * Link in at end of list.
	 */
	while (*pptr) {
	    if ((*pptr)->value == index) {
		reg_invoke_ip_clns_filter_destroy(index);
		return;
	    }
	    pptr = &(*pptr)->next;
	}

	value = malloc(sizeof(route_map_match_value));
	if (!value) {
	    reg_invoke_ip_clns_filter_destroy(index);
	    return;
	}

	value->value = index;

	*pptr = value;
    } else {
	index = reg_invoke_ip_get_filter_index(str);

	for ( ; *pptr; pptr = &(*pptr)->next) {
	    if ((*pptr)->value == index) {
		break;
	    }
	}
	if (*pptr) {
	    reg_invoke_ip_clns_filter_destroy(index);
	    value = *pptr;
	    free((uchar *)value->value);
	    *pptr = value->next;
	    free(value);
	}
    }
}

/*
 * route_map_process_match_acl_item
 */
void route_map_process_match_acl_item (route_map_match_acl **aptr,
				       int list, char *name,
				       boolean sense, boolean is_alpha)
{
    acl_headertype      *acl;
    route_map_match_acl *new, *current;

    
    if (sense) {
	acl = access_parse_acl(list, name, is_alpha, ACL_UNDEFINED);

	if (!acl)
	    return;

    	while (*aptr) {
	    if ((*aptr)->acl == acl) {
		return;
	    }
	    aptr = &(*aptr)->next;
	}

	new = malloc(sizeof(route_map_match_acl));
	if (!new) {
	    return;
	}

	new->acl = acl;
	*aptr = new;
    } else {
	if (list) 
	    acl = acl_array[list];
	else {
	    if (!is_valid_nacl(name)) {
		printf(nacl_name_error);
		return;
	    }
	    acl = access_find_nacl(name);
	}
	for ( ; *aptr; aptr = &(*aptr)->next) {
	    if ((*aptr)->acl == acl) {
		break;
	    }
	}
	if (*aptr) {
	    current = *aptr;
	    *aptr = current->next;
	    free(current);
	}
    }
}

/*
 * route_map_nvadd_acl
 *
 * nv_add() access-list type match clauses.
 */
static void route_map_nvadd_acl (route_map_match_acl *address)
{
    for ( ; address; address = address->next) {
	if (address->acl)
	    nv_add(TRUE, " %s", address->acl->name);
    }
}

/*
 * route_map_match_nvgen
 *
 * NV generate all match subcommands for a given route-map.
 */
void route_map_match_nvgen (parseinfo *csb)
{
    route_maptype              *map;
    route_map_match_value      *number;
    route_map_interface        *interface;
    char                       *str;

    map = csb->route_map;
    if (!map) {
	return;
    }

    /*
     * Do IP address access-lists.
     */
    if (map->match_list.ip_address) {
	nv_write(TRUE, "%s ip address", csb->nv_command);
	route_map_nvadd_acl(map->match_list.ip_address);
    }

    /*
     * Do CLNS address access-lists.
     */
    if (map->match_list.clns_address) {
	nv_write(TRUE, "%s clns address", csb->nv_command);
	route_map_nvadd_address(map->match_list.clns_address, TRUE);
    }

    /*
     * Do IP next-hop access-lists.
     */
    if (map->match_list.ip_next_hop) {
	nv_write(TRUE, "%s ip next-hop", csb->nv_command);
	route_map_nvadd_acl(map->match_list.ip_next_hop);
    }

    /*
     * Do CLNS next-hop access-lists.
     */
    if (map->match_list.clns_next_hop) {
	nv_write(TRUE, "%s clns next-hop", csb->nv_command);
	route_map_nvadd_address(map->match_list.clns_next_hop, TRUE);
    }

    /*
     * Do IP route-source access-lists.
     */
    if (map->match_list.ip_route_source) {
	nv_write(TRUE, "%s ip route-source", csb->nv_command);
	route_map_nvadd_acl(map->match_list.ip_route_source);
    }

    /*
     * Do CLNS route-source access-lists.
     */
    if (map->match_list.clns_route_source) {
	nv_write(TRUE, "%s clns route-source", csb->nv_command);
	route_map_nvadd_address(map->match_list.clns_route_source, TRUE);
    }

    /*
     * Do interfaces.
     */
    nv_write((boolean) map->match_list.interface, "%s interface", 
	     csb->nv_command);
    for (interface = map->match_list.interface; interface; 
	 interface = interface->next) {
	nv_add(TRUE, " %s", interface->idb->namestring);
    }

    /*
     * Do metrics.
     */
    if (map->match_list.metric) {
	nv_write(TRUE, "%s metric", csb->nv_command);
	route_map_nvadd_number(map->match_list.metric);
    }

    /*
     * Do tags.
     */
    if (map->match_list.tag) {
	nv_write(TRUE, "%s tag", csb->nv_command);
	route_map_nvadd_number(map->match_list.tag);
    }

    /*
     * Do route-type.
     */
    if (map->match_list.route_type) {
	nv_write(TRUE, "%s route-type", csb->nv_command);
	for (number = map->match_list.route_type; number; 
	     number = number->next) {
	    switch (number->value) {
	    case ROUTE_MAP_INTERNAL:
		str = "internal";
		break;
	    case ROUTE_MAP_EXTERNAL:
		str = "external";
		break;
	    case ROUTE_MAP_LEVEL1:
		str = "level-1";
		break;
	    case ROUTE_MAP_LEVEL2:
		str = "level-2";
		break;
	    case ROUTE_MAP_LOCAL:
		str = "local";
		break;
	    case ROUTE_MAP_EX_TYPE1:
		str = "external type-1";
		break;
	    case ROUTE_MAP_EX_TYPE2:
		str = "external type-2";
		break;
	    default:
		return;
	    }	
	    nv_add(TRUE, " %s", str);
	}
    }

    /*
     * Invoke the registered functions.
     */
    reg_invoke_route_map_match_nvgen(csb, map);
}

/*
 * route_map_nvadd_address
 *
 * nv_add() access-list type match clauses.
 */
void route_map_nvadd_address (route_map_match_value *address, boolean string)
{
    for ( ; address; address = address->next) {
	if (string) {
	    nv_add(TRUE, " %s", reg_invoke_ip_filter_name(address->value));
	} else {
	    nv_add(TRUE, " %d", address->value);
	}
    }
}

/*
 * route_map_nvadd_number
 *
 * nv_add() access-list type match clauses.
 */
void route_map_nvadd_number (route_map_match_value *number)
{
    for ( ; number; number = number->next) {
	if ((int) number->value < 0) {
	    nv_add(TRUE, " 0x%08x", number->value);
	} else {
	    nv_add(TRUE, " %d", number->value);
	}
    }
}

/*
 * route_map_set_nvgen
 *
 * NV generate the set subcommands for a given route-map.
 */
void route_map_set_nvgen (parseinfo *csb)
{
    route_maptype *map;
    uchar         *str;
    route_map_addrtype *aptr;
    boolean first;
    route_map_interface *iptr;

    map = csb->route_map;

    if (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) {
	if (map->set_list.flags)
	    str = ((map->set_list.flags & ROUTE_MAP_METRIC_ADD) ?
		   "+" : "-");
	else
	    str = NULL;

	if (map->set_list.metric.igrp_metric) {
	    nv_write(TRUE, "%s metric %u", csb->nv_command, 
		     map->set_list.metric.value.vector.user_bandwidth);
	    nv_add(TRUE, " %s%d", str,map->set_list.metric.value.vector.delay);
	    nv_add(TRUE, " %d", map->set_list.metric.value.vector.reliability);
	    nv_add(TRUE, " %d", map->set_list.metric.value.vector.loading);
	    nv_add(TRUE, " %d", map->set_list.metric.value.vector.mtu);
	} else {
	    nv_write(TRUE, "%s metric %s%u", csb->nv_command, str,
		     map->set_list.metric.value.scalar);
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) {
	nv_write(TRUE, "%s metric-type ", csb->nv_command);
	switch (map->set_list.metric_type) {
	case ROUTE_MAP_SET_TYPE1:
	    str = "type-1";
	    break;
	case ROUTE_MAP_SET_TYPE2:
	    str = "type-2";
	    break;
	case ROUTE_MAP_SET_INTERNAL:
	    str = "internal";
	    break;
	case ROUTE_MAP_SET_EXTERNAL:
	    str = "external";
	    break;
	default:
	    str = NULL;
	}
	if (str) {
	    nv_add(TRUE, "%s", str);
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_TAG_SET) {
	nv_write(TRUE, "%s tag ", csb->nv_command);
	if ((int) map->set_list.tag < 0) {
	    nv_add(TRUE, "0x%08x", map->set_list.tag);
	} else {
	    nv_add(TRUE, "%d", map->set_list.tag);
	}
    }
    if (map->set_list.set_flags & ROUTE_MAP_AUTOTAG_SET) {
	nv_write(TRUE, "%s automatic-tag", csb->nv_command);
    }

    if (map->set_list.set_flags & ROUTE_MAP_LEVEL_SET) {
	nv_write(TRUE, "%s level ", csb->nv_command);
	switch (map->set_list.level) {
	case ROUTE_MAP_SET_LEVEL1:
	    str = "level-1";
	    break;
	case ROUTE_MAP_SET_LEVEL2:
	    str = "level-2";
	    break;
	case ROUTE_MAP_SET_LEVEL12:
	    str = "level-1-2";
	    break;
	case ROUTE_MAP_SET_STUB:
	    str = "stub-area";
	    break;
	case ROUTE_MAP_SET_BACKBONE:
	    str = "backbone";
	    break;
	default:
	    str = NULL;
	}
	if (str) {
	    nv_add(TRUE, "%s", str);
	}
    }

    if ((map->set_list.set_flags & ROUTE_MAP_DEST_SET) &&
	(map->set_list.destination.type == ADDR_CLNS)) {
	nv_write(TRUE, "%s clns destination %n", csb->nv_command,
		 map->set_list.destination.clns_addr);
    }

    if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	first = TRUE;
	for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
	    if (aptr->address.type != ADDR_CLNS)
		continue;
	    if (first) {
		first = FALSE;
		nv_write(TRUE, "%s clns next-hop", csb->nv_command);
	    }
	    nv_add(TRUE, " %n", aptr->address.clns_addr);
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_INTERFACE_SET) {
	nv_write(TRUE, "%s interface", csb->nv_command);
	iptr = map->set_list.interface;
	for ( ; iptr; iptr = iptr->next) {
	    nv_add(TRUE, " %s", iptr->idb->namestring);
	}
    }
    /*
     * Invoke the registered functions.
     */
    reg_invoke_route_map_set_nvgen(csb, map);
}

/*
 * route_map_set_dest_command
 */
void route_map_set_dest_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_DEST_SET;
	bcopy(GETOBJ(paddr,1), &map->set_list.destination, 
	      sizeof(addrtype));

	/*
	 * If IP address specified, store mask.
	 */
	if (!GETOBJ(int,1)) {
	   map->set_list.mask = GETOBJ(paddr,2)->ip_addr;
	}
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_DEST_SET;
	map->set_list.destination.type = ADDR_ILLEGAL;
    }
}

/*
 * route_map_set_next_hop_command
 */
void route_map_set_next_hop_command (parseinfo *csb)
{
    route_maptype *map;
    ipaddrtype ipaddr;
    route_map_addrtype **pred, *aptr;

    map = csb->route_map;
    if (!map)
	return;
    
    ipaddr = GETOBJ(paddr,1)->ip_addr;
    pred = &map->set_list.next_hop;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_NEXT_HOP_SET;
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

	if (!map->set_list.next_hop)
	    map->set_list.set_flags &= ~ROUTE_MAP_NEXT_HOP_SET;
    }
}

/*
 * route_map_set_metric_command
 */
void route_map_set_metric_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_METRIC_SET;
	map->set_list.flags = GETOBJ(int,7);

	/*
	 * IGRP metric present.
	 */
	if (GETOBJ(int,3)) {
	    map->set_list.metric.igrp_metric = TRUE;
	    map->set_list.metric.value.vector.user_bandwidth = GETOBJ(int,2);
	    map->set_list.metric.value.vector.bandwidth =
		SCALED_BANDWIDTH(GETOBJ(int,2));
	    map->set_list.metric.value.vector.delay = GETOBJ(int,3);
	    map->set_list.metric.value.vector.reliability = GETOBJ(int,4);
	    map->set_list.metric.value.vector.loading = GETOBJ(int,5);
	    map->set_list.metric.value.vector.mtu = GETOBJ(int,6);
	} else {
	    map->set_list.metric.value.scalar = GETOBJ(int,2);
	    map->set_list.metric.igrp_metric = FALSE;
	}
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_METRIC_SET;
    }
}

/*
 * route_map_set_metrictype_command
 */
void route_map_set_metrictype_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_METRICTYPE_SET;
	map->set_list.metric_type = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_METRICTYPE_SET;
    }
}

/*
 * route_map_set_tag_command
 */
void route_map_set_tag_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_TAG_SET;
	map->set_list.tag = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_TAG_SET;
    }
}

/*
 * route_map_set_autotag_command
 */
void route_map_set_autotag_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_AUTOTAG_SET;
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_AUTOTAG_SET;
    }
}

/*
 * route_map_set_level_command
 */
void route_map_set_level_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_LEVEL_SET;
	map->set_list.level = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_LEVEL_SET;
    }
}

/*
 * route_map_set_interface_command
 * Parse a "set default interface" command.
 */

void route_map_set_interface_command (parseinfo *csb)
{
    route_maptype *map;
    route_map_interface *iptr, **pred, *next;
    idbtype *idb;

    map = csb->route_map;
    if (!map)
	return;
    
    idb = GETOBJ(idb, 1);
    pred = &map->set_list.interface;

    if (csb->sense) {
	if (!idb)
	    return;

	map->set_list.set_flags |= ROUTE_MAP_INTERFACE_SET;
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
	    map->set_list.interface = NULL;
	    map->set_list.set_flags &= ~ROUTE_MAP_INTERFACE_SET;
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

	if (!map->set_list.interface)
	    map->set_list.set_flags &= ~ROUTE_MAP_INTERFACE_SET;
    }
}

/*
 * route_map_ip_check
 *
 * Apply route_map specified by caller to contents of an ndb.
 * The caller passes in match flags indicating which matches it supports.  
 * This function returns TRUE, if all match criteria are met for a 
 * given permit route-map. It returns FALSE, for a deny route-map. When all 
 * route-map entries are processed and a match has not been satisfied, the 
 * funtion returns FALSE.
 */
route_maptype *route_map_ip_check (uchar *map_name, ndbtype *ndb, pdbtype *pdb)
{
    route_maptype      *map;
    route_map_headtype *head;
    ulong              matches_supported, matches_used;
    ipaddrtype         mask;
    boolean            first_as_path;
    char	       *path;
    static char         buffer[ROUTE_MAP_PATHSIZE];
    
    if (!(head = find_route_map_head(map_name))) {
	return(NULL);
    }

    matches_supported = route_map_ip_matches_supported(pdb->proctype);
    matches_used = matches_supported & head->match_flags;

    first_as_path = TRUE;
    path = NULL;
    mask = ndb->attrflags & NET_SUBNETTED ?
	get_majormask(ndb->netnumber) : ndb->netsmask;

    for (map = (route_maptype *) head->map_queue.qhead; map; map = map->next) {

	/*
	 * Match net number against IP access-list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_IPADDRESS) {
	    if (!route_map_match_ipaddress(ndb->netnumber, mask,
					   map->match_list.ip_address)) {
		continue;
	    }
	}

	/*
	 * Match gateway against IP access-list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_IPNEXTHOP) {
	    if (ndb->rdb[0] &&
		!route_map_match_ipaddress(ndb->rdb[0]->gateway, 0,
					   map->match_list.ip_next_hop)) {
		continue;
	    }
	}

	/*
	 * Match info source against IP access-list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_IPROUTESOURCE) {
	    if (ndb->rdb[0] &&
		!route_map_match_ipaddress(ndb->rdb[0]->infosource, 0,
			   map->match_list.ip_route_source)) {
		continue;
	    }
	}

	/*
	 * Match ndb metric against match list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_METRIC) {
	    if (!route_map_match_number(ndb->metric,
			   map->match_list.metric)) {
		continue;
	    }
	}

	/*
	 * Match rdb interface against match list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_INTERFACE) {
	    if (ndb->rdb[0] &&
		!route_map_match_number((ulong) ndb->rdb[0]->idb,
 	       (route_map_match_value *) map->match_list.interface)) {
		continue;
	    }
	}

	/*
	 * Match ndb route type against match list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_ROUTETYPE) {
	    if (!route_map_match_routetype(ndb,
			   map->match_list.route_type)) {
		continue;
	    }
	}
	
	/*
	 * Match ndb tag against match list.
	 */
	if (matches_used & ROUTE_MAP_MATCH_TAG) {
	    if (!route_map_match_number(ndb->tag, 
			map->match_list.tag)) {
		continue;
	    }
	}

	/*
	 * Match AS path against match list. Allowed when handling
	 * a BGP learned ndb.
	 */
	if (NDB2PROCTYPE(ndb) & PROC_BGP) {
	    if (first_as_path) {
		path = reg_invoke_ip_path(PROC_BGP, ndb->netnumber, mask, 
					  buffer, sizeof(buffer));
		first_as_path = FALSE;
	    }

	    if (path &&
		!route_map_match_aspath(path, map->match_list.as_path)) {
		continue;
	    }
	}

	/*
	 * If no map was found with full matches, or one was found with a
	 * deny action, return FALSE.
	 */
	if (!map->grant) {
	    map = NULL;
	}
	break;
    }
    return(map);
}

/*
 * route_map_ip_matches_supported
 *
 * Return the bit mask of the matches supported for a given routing
 * protocol.
 */
ulong route_map_ip_matches_supported (ulong proctype)
{
    ulong matches = 0;

    switch (proctype) {
    case PROC_CONNECTED:
	matches = ROUTE_MAP_IP_CONNECTED_MATCHES;
	break;
    case PROC_STATIC:
	matches = ROUTE_MAP_IP_STATIC_MATCHES;
	break;
    case PROC_BGP:
	matches = ROUTE_MAP_BGP_MATCHES;
	break;
    case PROC_RIP:
	matches = ROUTE_MAP_RIP_MATCHES;
	break;
    case PROC_IGRP:
	matches = ROUTE_MAP_IGRP1_MATCHES;
	break;
    case PROC_EGP:
	matches = ROUTE_MAP_EGP_MATCHES;
	break;
    case PROC_OSPF:
	matches = ROUTE_MAP_OSPF_MATCHES;
	break;
    case PROC_ISIS:
	matches = ROUTE_MAP_ISIS_MATCHES;
	break;
    case PROC_IGRP2:
	matches = ROUTE_MAP_IGRP2_MATCHES;
	break;
    case PROC_ODR:
	matches = ROUTE_MAP_ODR_MATCHES;
	break;
    default:
	matches = 0;
    }
    return(matches);
}

/*
 * route_match_ipaddress
 * 
 * Match address with IP access-list entry.
 */
boolean route_map_match_ipaddress (ipaddrtype address, ipaddrtype mask,
				   route_map_match_acl *match_list)
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
	if (route_check(aptr->acl, address, mask)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * route_map_match_number
 *
 * Match number with match list entries.
 */
boolean route_map_match_number (ulong number,
				route_map_match_value *match_list)
{
    route_map_match_value *value;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (value = match_list; value; value = value->next) {
	if (value->value == number) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/* 
 * route_map_match_routetype
 *
 * Match route type from routing table entry with match list.
 */
boolean route_map_match_routetype (ndbtype *ndb,
				   route_map_match_value *match_list)
{
    route_map_match_value *value;
    ulong route_type = ndb->route_type;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (value = match_list; value; value = value->next) {
	switch (value->value) {
	case ROUTE_MAP_INTERNAL:
	    if (route_type & (RTYPE_INTRA | RTYPE_INTER | RTYPE_IGRP2_INT |
			      RTYPE_BGP_INT)) {
 		return(TRUE);
 	    }
 	    if (ndb->attrflags & NET_ATTACHED) {
		return(TRUE);
	    }
	    break;
	case ROUTE_MAP_EXTERNAL:
	    if (route_type & (RTYPE_EXTERN1 | RTYPE_EXTERN2 | 
			      RTYPE_IGRP2_EXT | RTYPE_BGP_EXT)) {
		return(TRUE);
	    }
	    break;
	case ROUTE_MAP_LEVEL1:
	    if (route_type & RTYPE_L1) {
		return(TRUE);
	    }
	    break;
	case ROUTE_MAP_LEVEL2:
	    if (route_type & RTYPE_L2) {
		return(TRUE);
	    }
	    break;
	case ROUTE_MAP_LOCAL:
	    if (route_type & RTYPE_BGP_LOC)
		return(TRUE);
	    break;
	case ROUTE_MAP_EX_TYPE1:
	    if (route_type & RTYPE_EXTERN1)
		return(TRUE);
	    break;
	case ROUTE_MAP_EX_TYPE2:
	    if (route_type & RTYPE_EXTERN2)
		return(TRUE);
	    break;
	default:
	    continue;
	}
    }
    return(FALSE);
}

/*
 * route_match_aspath
 * 
 * Match address with IP AS path access-list entry.
 */
boolean route_map_match_aspath (char *as_path,
				route_map_match_value *match_list)
{
    route_map_match_value *value;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (value = match_list; value; value = value->next) {
	if (regexp_accesscheck(as_path, value->value, as_path_accesslists)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * route_map_update_flags
 *
 * Update the match flags in all the members of a route-map.
 * Also set the match_flags and set_flags in the head of the route-map 
 * to reflect all the set/match flags present in its members.
 */
static void route_map_update_flags (char *map_name)
{
    route_map_headtype *head;
    route_maptype *map;
    route_map_matchtype *match;

    head = find_route_map_head(map_name);
    if (head) {
	head->match_flags = 0;
	head->set_flags = 0;

	map = (route_maptype *)head->map_queue.qhead;
	for (; map; map = map->next) {
	    match = &(map->match_list);
	    match->match_flags = (match->match_flags & 
				  ROUTE_MAP_MATCH_EXACT_COMMUNITY);

	    if (match->metric)
		match->match_flags |= ROUTE_MAP_MATCH_METRIC;
	    if (match->tag)
		match->match_flags |= ROUTE_MAP_MATCH_TAG;
	    if (match->as_path)
		match->match_flags |= ROUTE_MAP_MATCH_ASPATH;
	    if (match->community)
		match->match_flags |= ROUTE_MAP_MATCH_COMMUNITY;
	    if (match->route_type)
		match->match_flags |= ROUTE_MAP_MATCH_ROUTETYPE;
	    if (match->ip_address)
		match->match_flags |= ROUTE_MAP_MATCH_IPADDRESS;
	    if (match->ip_next_hop)
		match->match_flags |= ROUTE_MAP_MATCH_IPNEXTHOP;
	    if (match->clns_next_hop)
		match->match_flags |= ROUTE_MAP_MATCH_CLNSNEXTHOP;
	    if (match->ip_route_source)
		match->match_flags |= ROUTE_MAP_MATCH_IPROUTESOURCE;
	    if (match->clns_route_source)
		match->match_flags |= ROUTE_MAP_MATCH_CLNSROUTESOURCE;
	    if (match->max_length)
		match->match_flags |= ROUTE_MAP_MATCH_LENGTH;
	    if (match->interface)
 		match->match_flags |= ROUTE_MAP_MATCH_INTERFACE;

	    head->match_flags |= match->match_flags;
	    head->set_flags |= map->set_list.set_flags;
	}
    }
}

/*
 * route_map_inform_protocols
 *
 * Inform routing protocols when a route-map is added, deleted, or
 * changed. Only the incremental update type protocols are informed,
 * OSPF, BGP, IS-IS, and IP-IGRP2.
 */
void route_map_inform_protocols (char *map_name, int event)
{
    pdbtype      *pdb, *dist;
    int          i;

    /*
     * Update the set/match flags.
     */
    route_map_update_flags(map_name);

    /*
     * Inform IP routing protocols.
     */
    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(pdb->proctype & (PROC_OSPF|PROC_BGP|PROC_ISIS|PROC_IGRP2))) {
	    continue;
	}
	if (!pdb->redistribute) {
	    continue;
	}
	for (i = 0; i < MAX_IPPROCS; i++) {
	    if (strcmp(pdb->routemap[i], map_name)) {
		continue;
	    }
	    dist = INDEX2PDB(i);
	    (*pdb->redistribute)(pdb, dist, TRUE);
	}
	
	/*	
	 * Inform protocol if default-information route-map has changed.
	 */
	if (!strcmp(pdb->default_routemap, map_name)) {
	    (*pdb->redistribute)(pdb, NULL, TRUE);
	}
    }

    /*
     * Inform OSI and IP protocols that care about change in route-maps.
     */
    reg_invoke_ip_route_map_inform(map_name, event);
}

/*
 * route_map_eols
 *
 * Inform protocols that match/set command was entered.
 */
void route_map_eols (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;
    if (map) {
	route_map_inform_protocols(map->head->map_tag, ROUTE_MAP_MODIFY);
    }
}

/*
 * show_route_map
 *
 * Process exec command "show route-map [<map-name>]".
 */
void show_route_map (parseinfo *csb)
{
    route_map_headtype *head;
    int                i;

    automore_enable(NULL);

    /*
     * Display one route map.
     */
    if (*GETOBJ(string,1)) {
	head = find_route_map_head(GETOBJ(string,1));
	if (!head) {
	    printf("\nroute-map %s not found", GETOBJ(string,1));
	} else {
	    display_one_route_map(head);
	}
    } else {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (head = route_maps[i]; head; head = head->next) {
		display_one_route_map(head);
	    }
	}
    }

    automore_disable();
}

/*
 * display_one_route_map
 *
 * Display a single route-map entry.
 */
void display_one_route_map (route_map_headtype *head)
{
    route_maptype         *map;
    route_map_match_value *value;
    uchar                 *str;
    boolean 		  first;
    route_map_addrtype	  *aptr;
    route_map_interface   *iptr;
    route_map_match_acl   *aclptr;

    for (map = (route_maptype *) head->map_queue.qhead; map; map = map->next) {
	printf("\nroute-map %s, %s, sequence %d", head->map_tag, 
	       (map->grant) ? "permit" : "deny", map->seq_num);

	/*
	 * Print sub-commands.
	 */
	printf("\n  Match clauses:");

	aclptr = map->match_list.ip_address;
	if (aclptr) {
	    printf("\n    ip address (access-lists): ");
	}
	for (; aclptr; aclptr = aclptr->next) {
	    if (aclptr->acl)
		printf("%s ", aclptr->acl->name);
	}

	value = map->match_list.clns_address;
	if (value) {
	    printf("\n    clns address (filter-sets): ");
	}
	for (; value; value = value->next) {
	    printf("%s ", reg_invoke_ip_filter_name(value->value));
	}

	aclptr = map->match_list.ip_next_hop;
	if (aclptr) {
	    printf("\n    ip next-hop (access-lists): ");
	}
	for (; aclptr; aclptr = aclptr->next) {
	    if (aclptr->acl)
		printf("%s ", aclptr->acl->name);
	}

	value = map->match_list.clns_next_hop;
	if (value) {
	    printf("\n    clns next-hop (filter-sets): ");
	}
	for (; value; value = value->next) {
	    printf("%s ", reg_invoke_ip_filter_name(value->value));
	}

	aclptr = map->match_list.ip_route_source;
	if (aclptr) {
	    printf("\n    ip route-source (access-lists): ");
	}
	for (; aclptr; aclptr = aclptr->next) {
	    if (aclptr->acl)
		printf("%s ", aclptr->acl->name);
	}

	value = map->match_list.clns_route_source;
	if (value) {
	    printf("\n    clns route-source (filter-sets): ");
	}
	for (; value; value = value->next) {
	    printf("%s ", reg_invoke_ip_filter_name(value->value));
	}

	value = map->match_list.metric;
	if (value) {
	    printf("\n    metric ");
	}
	for (; value; value = value->next) {
	    printf("%d ", value->value);
	}

	value = map->match_list.route_type;
	if (value) {
	    printf("\n    route-type ");
	}
	for (; value; value = value->next) {
	    switch (value->value) {
	    case ROUTE_MAP_INTERNAL:
		str = "internal";
		break;
	    case ROUTE_MAP_EXTERNAL:
		str = "external";
		break;
	    case ROUTE_MAP_LEVEL1:
		str = "level-1";
		break;
	    case ROUTE_MAP_LEVEL2:
		str = "level-2";
		break;
	    case ROUTE_MAP_LOCAL:
		str = "local";
		break;
	    case ROUTE_MAP_EX_TYPE1:
		str = "external type-1";
		break;
	    case ROUTE_MAP_EX_TYPE2:
		str = "external type-2";
		break;
	    default:
		str = "unknown";
	    }	
	    printf("%s ", str);
	}

	value = map->match_list.tag;
	if (value) {
	    printf("\n    tag ");
	}
	for (; value; value = value->next) {
	    if ((int) value->value < 0) {
		printf("0x%08x ", value->value);
	    } else {
		printf("%d ", value->value);
	    }
	}

	value = (route_map_match_value *) map->match_list.interface;
	if (value) {
	    printf("\n    interface ");
	}
	for (; value; value = value->next) {
	    printf("%s ", 
		   ((route_map_interface *) value)->idb->namestring);
	}

	reg_invoke_route_map_show_match(map);

	printf("\n  Set clauses:");

	if (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) {
	    if (map->set_list.flags)
		str = ((map->set_list.flags & ROUTE_MAP_METRIC_ADD) ?
		       "+" : "-");
	    else
		str = NULL;

	    if (map->set_list.metric.igrp_metric) {
		printf("\n    metric %u", map->set_list.metric.value.scalar);
		printf(" %s%d %d %d %d",
		       str, 
		       map->set_list.metric.value.vector.delay,
		       map->set_list.metric.value.vector.reliability,
		       map->set_list.metric.value.vector.loading,
		       map->set_list.metric.value.vector.mtu);
	    } else {
		printf("\n    metric %s%u", str,
		       map->set_list.metric.value.scalar);
	    }	
	}

	if (map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) {
	    printf("\n    metric-type ");
	    switch (map->set_list.metric_type) {
	    case ROUTE_MAP_SET_TYPE1:
		str = "type-1";
		break;
	    case ROUTE_MAP_SET_TYPE2:
		str = "type-2";
		break;
	    case ROUTE_MAP_SET_INTERNAL:
		str = "internal";
		break;
	    case ROUTE_MAP_SET_EXTERNAL:
		str = "external";
		break;
	    default:
		str = NULL;
	    }
	    if (str) {
		printf(str);
	    }
	}

	if (map->set_list.set_flags & ROUTE_MAP_TAG_SET) {
	    printf("\n    tag ");
	    if ((int) map->set_list.tag < 0) {
		printf("0x%08x ", map->set_list.tag);
	    } else {
		printf("%d ", map->set_list.tag);
	    }
	}

	if (map->set_list.set_flags & ROUTE_MAP_AUTOTAG_SET) {
	    printf("\n    automatic-tag");
	}

	if (map->set_list.set_flags & ROUTE_MAP_LEVEL_SET) {
	    printf("\n    level ");
	    switch (map->set_list.level) {
	    case ROUTE_MAP_SET_LEVEL1:
		str = "level-1";
		break;
	    case ROUTE_MAP_SET_LEVEL2:
		str = "level-2";
		break;
	    case ROUTE_MAP_SET_LEVEL12:
		str = "level-1-2";
		break;
	    case ROUTE_MAP_SET_STUB:
		str = "stub-area";
		break;
	    case ROUTE_MAP_SET_BACKBONE:
		str = "backbone";
		break;
	    default:
		str = NULL;
	    }
	    if (str) printf(str);
	}

	if ((map->set_list.set_flags & ROUTE_MAP_DEST_SET) &&
	    (map->set_list.destination.type == ADDR_CLNS)) {
	    printf("\n    clns destination %n", 
		   map->set_list.destination.clns_addr);
	}

	if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	    first = TRUE;
	    for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
		if (aptr->address.type != ADDR_CLNS)
		    continue;
		if (first) {
		    first = FALSE;
		    printf("\n    clns next-hop");
		}
		printf(" %n", aptr->address.clns_addr);
	    }
	}

	if (map->set_list.set_flags & ROUTE_MAP_INTERFACE_SET) {
	    printf("\n    interface");
	    iptr = map->set_list.interface;
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
	
	reg_invoke_route_map_show_set(map);
    }
}

/*
 * route_map_default_check
 * Check if the route-map allows default generation.
 * If successfull return the map to allow set condition
 */
route_maptype *route_map_default_check (uchar *routemap)
{
    ndbtype *ndb;

    /*
     * Check access-lists configured in route-map to locate an ndb. If
     * there is no match, default route generation should not occur.
     */
    ndb = route_map_find_ndb(routemap);
    if (!ndb) 
	return(NULL);

    /*
     * ndb is found. Apply rest of configured route-map to ndb and
     * return the resulting route-map
     */
    return(route_map_ip_check(routemap, ndb,
			      INDEX2PDB(ndb->pdbindex)));
}

/*
 * route_map_find_ndb
 *
 * Given a configured route-map, find ndb that corresponds to an access-list
 * specificed destination.
 */
ndbtype *route_map_find_ndb (uchar *map_name)
{

    route_maptype      *map;
    route_map_headtype *head;
    ndbtype            *ndb;

    ndb = NULL;
    if (!(head = find_route_map_head(map_name))) return(NULL);

    /*
     * Go through all route-maps with same name.	
     */
    for (map = (route_maptype *) head->map_queue.qhead; map; map = map->next) {

	/*
	 * Get ndb based on IP access-list reference in route-map.
	 */
	ndb = route_map_use_accesslist(map->match_list.ip_address);
	if (ndb) break;
    }
    return(ndb);
}

/*
 * route_map_use_accesslist
 *
 * For each access-list reference in a route-map, do a route table lookup
 * to find corresponding ndb. If the access-list says deny, the route lookup
 * continues.
 */
ndbtype *route_map_use_accesslist (route_map_match_acl *match_list)
{
    route_map_match_acl   *list;
    fasttype              *grp;
    fastitem              *acc;
    ndbtype               *ndb;

    ndb = NULL;

    /*
     * For each "match ip address" access-list number, do a route lookup
     * for each wildcard entry for a given access-list.
     */
    for (list = match_list; list; list = list->next) {

	if (!list->acl || !list->acl->list)
	    continue;
	/*
	 * Validate that the user didn't enter an extended access list
	 */
	if ((list->acl->type != ACL_IP_SIMPLE) &&
	    (list->acl->type != ACL_IP_NAMED_SIMPLE))
	    continue; 

	grp = (fasttype *) list->acl->list;

	/*
	 * Access list found. Return, if route lookup successful and permit
	 * was configured. Otherwise, continue looking for access-list
	 * references in route-map match list.
	 */
	for (acc = grp->items; acc; acc = acc->next) {
	    ndb = ip_rtlookup(acc->permitmask, ~acc->wildmask);
	    if (ndb)
		if (ndb->metric != METRIC_INACCESSIBLE)
		    break;
	    ndb = NULL;
	}
	if (acc && acc->grant) break;
    }
    return(ndb);
}
