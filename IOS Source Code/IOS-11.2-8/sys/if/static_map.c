/* $Id: static_map.c,v 3.5.4.9 1996/06/07 17:11:16 rzagst Exp $
 * $Source: /release/112/cvs/Xsys/if/static_map.c,v $
 *------------------------------------------------------------------
 * static_map.c -- Support for generic static maps.
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: static_map.c,v $
 * Revision 3.5.4.9  1996/06/07  17:11:16  rzagst
 * CSCdi56880:  Cannot create SVC connection on pt-to-pt subinterface
 * Branch: California_branch
 *
 * Revision 3.5.4.8  1996/05/21  17:47:19  skoh
 * CSCdi57812:  Change local-addr to source-addr in map-list for FR SVCs
 * Branch: California_branch
 *
 * Revision 3.5.4.7  1996/05/15  05:40:27  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.5.4.6  1996/05/09  14:30:23  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.4.4.2.1  1996/04/27  07:04:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.4.5  1996/05/08  02:43:37  grobelch
 * - Create system list registry functions invoked by static-map software
 *   when:
 *     a static-map is deleted, a map-group is deleted, and when a
 *     static-map is updated. A provider of static-map service (such
 *     as ATM signalling) can register for this, check for one of it's
 *     map's being the subject of the call, and act appropriately.
 *   - Provide ATM signalling routines for the static-map and group
 *     registries. Remove ARP table entries for IP and free up
 *     ATM signalling-specific data structures.
 *   - Improve vc "glomming" for point-to-point VCs to take in
 *     AAL5MUX case.
 *   - Fix mistaken use of an enumeration as a bit-value.
 * CSCdi53027:  ATMSIG: SVC static maps not updated properly in all cases
 * Branch: California_branch
 *
 * Revision 3.5.4.4  1996/04/25  23:13:41  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.4.3  1996/04/08  20:51:48  ronnie
 * CSCdi51093:  no atm arp-server command causes crash
 * Branch: California_branch
 *
 * Revision 3.5.4.2.4.2  1996/04/11  00:04:14  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Track manual configuration of MTU on swidb basis
 * Save sub-interface MTU to NVRAM
 *
 * Revision 3.5.4.2.4.1  1996/04/08  01:54:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.4.2  1996/03/28  15:47:28  grobelch
 * - If malloc fails, get out.
 * CSCdi52827:  Static maps: invalid ptr use on malloc failure, adding map
 * to list
 * Branch: California_branch
 *
 * Revision 3.5.4.1  1996/03/18  20:14:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.2  1996/03/28  23:34:13  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * Bug fix for map-group on major interface.
 *
 * Revision 3.5.6.1  1996/03/28  17:25:43  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.5  1996/03/09  22:55:24  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.4  1995/12/17  18:28:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/20  16:03:06  ronnie
 * CSCdi42986:  staticmap_lookup_link always returns NULL
 *
 * Revision 3.2  1995/11/17  09:31:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/28  20:42:46  ronnie
 * CSCdi40913:  More alignment errors with RFC 1577
 *
 * Revision 2.6  1995/09/21  21:30:51  ronnie
 * CSCdi40742:  RFC 1577 clients generate alignment errors
 *
 * Revision 2.5  1995/09/20  21:28:21  ronnie
 * CSCdi39998:  ATM ARP Server calls staticmap_enter() with a NULL maplist
 *
 * Revision 2.4  1995/08/23  22:50:28  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.3  1995/06/22  01:28:12  bbenson
 * CSCdi35017:  Client should disallow rfc1577 and vice versa on subint.
 *
 * Revision 2.2  1995/06/19  01:05:52  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:54:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "network.h"
#include "config.h"
#include "parser.h"
#include "static_map.h"
#include "../lane/lane.h"
#include "../wan/parser_defs_frame.h"
#include "../tcp/http.h"
#include "../ip/ip_registry.h"
#include "bitlogic.h"

/*
 * Storage
 */
static queuetype map_listQ;		    /* queue of map lists */
static queuetype map_classQ;		    /* queue of map class */
queuetype *maplistQ = &map_listQ;   /* pointer to map list queue */
queuetype *mapclassQ = &map_classQ; /* pointer to map class queue */

/*
 * Forward declarations
 */
static void mapclass_delete(mapclass_type *);

/*
 * statichash()
 *
 * Force strict type checking for hash generation
 */
static inline uchar *
address_to_uchar (union addresses *address)
{
    return (uchar *)address;
}

static inline int
static_hash (addrtype *addr)
{
    if (addr == NULL)
	return 0;
    return xnethash(address_to_uchar(&addr->addr), addr->length) &
	(STATIC_MAP_HASH - 1);
}

static inline int
static_hw_hash (hwaddrtype *hwaddr)
{
    if (hwaddr == NULL)
	return 0;
    return xnethash(hwaddr->addr, hwaddr->length) & (STATIC_MAP_HASH - 1);
}

/*
 * staticmap_broadcast
 * Link all maps that are defined for pseudo-broadcast and return pointer
 * to the first one. 
 */
staticmap_type *staticmap_broadcast (idbtype *idb, uint protocol, uint encap)
{
    staticmap_type *bc_map, *cur_map, *map_ptr;
    mapgroup_type *group_ptr;
    maplist_type *list_ptr;
    int i;

    cur_map = NULL;
    map_ptr = NULL;
    FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
        list_ptr = group_ptr->maplist;
	FOR_ALL_MAPS_IN_LIST(i, list_ptr, bc_map) {
	    if ((bc_map->encap == encap) && 
		  (bc_map->protocol == protocol) &&
		    reg_invoke_is_map_broadcast(encap, bc_map)) {
		if (!map_ptr) {
		    map_ptr = bc_map;	/* first map of the list */
		}
		if (!cur_map) {
		    cur_map = bc_map;	/* current map in the list */
		} else {
		    cur_map->next_bc = bc_map;
		    cur_map = cur_map->next_bc;
		}
	    }
	}
    }
    if (cur_map)
	cur_map->next_bc = NULL;	/* terminate linked list */

    return(map_ptr);
}

/*
 * staticmap_hwlookup_list
 * Return pointer to first map identified by data link layer address
 * within a given map list.  A protocol to match may be passed in as well.
 */
staticmap_type *staticmap_hwlookup_list (maplist_type *list_ptr,
                                         hwaddrtype *hwaddr,
					 uint encap,
					 boolean match_protocol,
					 link_t protocol)
{
    staticmap_type *map_ptr;
    leveltype level;
    int hash;

    level = raise_interrupt_level(NETS_DISABLE);
    hash = static_hw_hash(hwaddr);

    for (map_ptr = list_ptr->hwhash[hash]; map_ptr != NULL ;
	 map_ptr = map_ptr->next_hw) {
        if ((map_ptr->encap == encap) &&
	    (!match_protocol || map_ptr->protocol == protocol) &&
	    (bcmp(&map_ptr->hwconf, hwaddr, sizeof(hwaddrtype)) == 0))
            break;
    }
    reset_interrupt_level(level);

    return(map_ptr);
}

/*
 * staticmap_hwlookup
 * Return pointer to first map identified by data link address within a
 * given idb (possibly search across several map lists).  A protocol to
 * match may be passed in as well.
 */
staticmap_type *staticmap_hwlookup (idbtype *idb,
				    hwaddrtype *hwaddr,
				    uint encap,
				    boolean match_protocol,
				    link_t protocol)
{
    staticmap_type *map_ptr = NULL;
    mapgroup_type *group_ptr;

    /*
     * search all groups
     */
    FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
        map_ptr = staticmap_hwlookup_list(group_ptr->maplist, hwaddr, encap,
					  match_protocol, protocol);
        if (map_ptr)
            break;
    }
    return(map_ptr);
}

/*
 * staticmap_lookup_link
 * Return pointer to first map identified by protocol type within a
 * given map list
 */
static staticmap_type *staticmap_lookup_link (maplist_type	*list_ptr, 
					      uint		protocol,
					      uint		encap)
{
    staticmap_type *map_ptr;
    leveltype level;
    int hash;

    level = raise_interrupt_level(NETS_DISABLE);
    for (hash = 0; hash < STATIC_MAP_HASH; hash++) {
    	map_ptr = list_ptr->hash[hash];

    	while (map_ptr) {

            if ((map_ptr->protocol == protocol) && 
			(map_ptr->encap == encap)) {
		reset_interrupt_level(level);

		return(map_ptr);
	    } else {
            	map_ptr = map_ptr->next;
	    }
	}
    }
    reset_interrupt_level(level);

    return(map_ptr);
}

#define BITMAP_CHUNK (sizeof(ulong) * BITMAP_SIZE)
/*
 * staticmap_lookup_prefix
 * Perform a prefix based lookup.  'masklen' specifies how many bits of
 * network layer address to compare.
 */

staticmap_type *staticmap_lookup_prefix (maplist_type *list_ptr,
					 addrtype *addr, 
					 uint protocol, uint encap,
					 ushort masklen)
{
    staticmap_type *map_ptr;
    leveltype level;
    int hash;
    addrtype *mapaddr;
    ulong val, mapval, *mapcp, *cp, mask;

    if (!addr)
	return(NULL);
    /*
     * If mask is longer than the network layer address itself, give up.
     */
    if (masklen > (addr->length * BITMAP_SIZE))
	return(NULL);
    level = raise_interrupt_level(NETS_DISABLE);
    for (hash = 0; hash < STATIC_MAP_HASH; hash++) {
    	for (map_ptr = list_ptr->hash[hash]; map_ptr;
	     map_ptr = map_ptr->next) {
            if (map_ptr->protocol != protocol)
		continue;
	    if (map_ptr->encap != encap)
		continue;
	    mapaddr = &map_ptr->addr;
	    if (mapaddr->type != addr->type)
		continue;
	    if (mapaddr->length != addr->length)
		continue;
	    cp = (ulong *)&addr->addr;
	    mapcp = (ulong *)&mapaddr->addr;
	    /*
	     * Compare prefixes, BITMAP_CHUNK bits at a time.
	     * Once we've compared as many bits as indicated in the mask,
	     * comparison is complete.
	     */
	    for (;; cp++, mapcp++) {
		val = GETLONG(cp);
		mapval = GETLONG(mapcp);
		/*
		 * Only need to apply mask if there are less than BITMAP_CHUNK
		 * bits left to compare.
		 */
		if (masklen < BITMAP_CHUNK) {
		    mask = (0xffffffff << (BITMAP_CHUNK - masklen));
		    mapval &= mask;
		}
		if (val != mapval)
		    break;
		if (masklen <= BITMAP_CHUNK) {
		    /*
		     * The prefixes match.
		     */
		    reset_interrupt_level(level);
		    return(map_ptr);
		}
		masklen -= BITMAP_CHUNK;
	    }
	}
    }
    reset_interrupt_level(level);
    return(NULL);
}

/*
 * staticmap_lookup_list
 * Return pointer to first map identified by protocol address within a
 * given map list
 */
staticmap_type *staticmap_lookup_list (maplist_type *list_ptr, addrtype *addr, 
				       uint protocol, uint encap)
{
    staticmap_type *map_ptr;
    leveltype level;
    int hash;

    level = raise_interrupt_level(NETS_DISABLE);
    hash = static_hash(addr);
    map_ptr = list_ptr->hash[hash];

    while (map_ptr) {
        if ((map_ptr->protocol == protocol) &&
	    (map_ptr->encap == encap) &&
	    address_equ(&map_ptr->addr, addr))
	    break;
	else
            map_ptr = map_ptr->next;
    }
    reset_interrupt_level(level);

    return(map_ptr);
}

/*
 * staticmap_lookup
 * Return pointer to first map identified by protocol address within a 
 * given idb (possibly search across several map lists)
 */
staticmap_type *staticmap_lookup (idbtype *idb, addrtype *addr, 
				  uint protocol, uint encap, boolean broadcast)
{
    staticmap_type *map_ptr = NULL;
    mapgroup_type *group_ptr;

    /*
     * if broadcast flag set, link all broadcast maps instead
     */
    if (broadcast)
	return(staticmap_broadcast(idb, protocol, encap));

    /*
     * search all groups
     */
    FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
	if (addr) {
	    map_ptr = staticmap_lookup_list(group_ptr->maplist,
					addr, protocol, encap);
	} else {
	    map_ptr = staticmap_lookup_link(group_ptr->maplist,
					protocol, encap);
	}
	if (map_ptr)
	    break;
    }
    return(map_ptr);
}

/*
 * staticmap_enter
 * Add static map to map list
 */
staticmap_type *
staticmap_enter (maplist_type	*list_ptr,
		 addrtype 	*addr,
		 hwaddrtype 	*hwconf,
		 uint 		protocol,
		 uint 		encap,
		 enum STATIC_MAP_TYPE	map_type, 
		 char 		*name,
		 void 		*params)
{
    mapclass_type *class_ptr = NULL;
    staticmap_type *map_ptr, **map_link;
    atm_map_type *atm_map;
    leveltype level;
    int hash;

    /*
     * Careful engineers would not need this test.  I, on the other hand...
     */
    if (list_ptr == NULL)
	return NULL;
    
    /*
     * if map class is specified, check that class exists, if not create 
     * a non-visible one, if can't create one, return
     */
    if (name && (*name != '\0')) {
	class_ptr = mapclass_lookup(name, encap);
	if (!class_ptr) {
	    class_ptr = mapclass_create(name, encap, FALSE);

            /*
             * if map class successfully created, enqueue it
             * else return NULL
             */
            if (class_ptr)
                enqueue(mapclassQ, class_ptr);
            else 
                return(NULL);
	}
	class_ptr->ref_count++;
    }

    /*
     * create and initialize static map
     */
    map_ptr = malloc(sizeof(staticmap_type));
    if (!map_ptr)
	return NULL;
 
        map_ptr->next = NULL;
        map_ptr->next_hw = NULL;
	if (addr != NULL)
	    map_ptr->addr = *addr;
	if (hwconf != NULL)
	    map_ptr->hwconf = *hwconf;
        memset(&map_ptr->hwaddr, 0, sizeof(hwaddrtype));
        map_ptr->params = params;
        map_ptr->protocol = protocol;
        map_ptr->encap = encap;
        map_ptr->map_type = map_type;
        map_ptr->class = class_ptr;
	map_ptr->atm_map = NULL;
	map_ptr->list_ptr = list_ptr;
	
	/* 
	 * add encapsulation specific map structures here
	 */
	if (encap == ET_ATM) {
	  atm_map = malloc(sizeof(atm_map_type));
	  if (atm_map) {
	    memset(atm_map, 0, sizeof(atm_map_type));
	    map_ptr->atm_map = atm_map;
	  } 
	}

	/*
	 * insert at the end of protocol address link list
	 */
        hash = static_hash(addr);
	level = raise_interrupt_level(NETS_DISABLE);
	for (map_link = &list_ptr->hash[hash]; *map_link;
					map_link = &((*map_link)->next));
	*map_link = map_ptr;

        /*
         * insert at the end of link layer address link list
         */
	if (hwconf) {
            hash = static_hw_hash(hwconf);
	    for (map_link = &list_ptr->hwhash[hash]; *map_link;
					map_link = &((*map_link)->next_hw));
	    *map_link = map_ptr;
	}
	reset_interrupt_level(level);

    GET_TIMESTAMP(map_ptr->update_time);

    return(map_ptr);
}

/*
 * staticmap_delete
 * Delete individual static map from list
 */
void staticmap_delete (maplist_type *list_ptr, staticmap_type *map_ptr)
{
    staticmap_type **map_link, *bc_map;
    mapclass_type *class_ptr;
    maplist_type *tmp_list_ptr;
    leveltype level;
    int hash;

   /*
    * This is ugly. When broadcasts are to be performed all maps
    * related to the broadcast are linked via each node's next_bc
    * pointer in a singly-linked fashion. This makes it very hard to
    * locate all next_bc pointers that may be pointing at the node
    * to be deleted. So, hold on tight boys and girls....
    */
   FOR_ALL_MAPLISTS(tmp_list_ptr) {
       FOR_ALL_MAPS_IN_LIST(hash, tmp_list_ptr, bc_map) {
 
           if (bc_map->next_bc == map_ptr) {
               bc_map->next_bc = map_ptr->next_bc;
           }
       }
   }

    /* call map-type-specific function(s) to handle deletion of map. */
    /* NOTE: map, atm_map, and params are all freed HERE. Any other  */
    /* action not common to all maps is handled in the called fn(s). */
    reg_invoke_ext_staticmap_delete(map_ptr);

    /*
     * find and unlink the address map from the address hash and hwhash tables.
     */
    hash = static_hash(&map_ptr->addr);
    level = raise_interrupt_level(NETS_DISABLE);
    for (map_link = &list_ptr->hash[hash]; 
			*map_link && (*map_link != map_ptr);
				map_link = &((*map_link)->next));
    *map_link = map_ptr->next;

    hash = static_hw_hash(&map_ptr->hwconf);
    for (map_link = &list_ptr->hwhash[hash];
			*map_link && (*map_link != map_ptr);
				map_link = &((*map_link)->next_hw));
    *map_link = map_ptr->next_hw;
    reset_interrupt_level(level);

    /*
     * decrement map class reference count, and delete class if reference
     * count is equal to 0 and map class is not visible
     */
    class_ptr = map_ptr->class;
    if (class_ptr) {
	class_ptr->ref_count--;
	if (!(class_ptr->ref_count || class_ptr->visible))
	    mapclass_delete(class_ptr);
    }

    /* free any encapsulation specific structures here */
    if (map_ptr->atm_map != NULL)
	free(map_ptr->atm_map);

    /* free the map structure */

    if (map_ptr->params)              /* free the associated memory */
	free(map_ptr->params);
    /*
     * we'd better NULL its next field before freeing it. Otherwise we
     * may let a display thread which still possesses this block (e.g. for
     * a show cmd) to follow the potential junk link and results in a chaos.
     */
    map_ptr->next = NULL;		
    free(map_ptr);
}

/*
 * staticmap_replace_hw
 *	Called to replace the HWADDR of a particular protocol address.
 *	Means that we must unhash it from the old HASH table, change the
 *	HWADDR of the map entry & rehash it on the new HWADDR.
 */
void staticmap_replace_hw (
    maplist_type *list_ptr, 
    staticmap_type *map_ptr, 
    hwaddrtype *hwaddr)
{
    staticmap_type **map_link;
    leveltype level;
    int hash;

    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * Find existing entry & unlink it from this HASH queue.
     */
    hash = static_hw_hash(&map_ptr->hwconf);
    for (map_link = &list_ptr->hwhash[hash];
			*map_link && (*map_link != map_ptr);
				map_link = &((*map_link)->next_hw));
    *map_link = map_ptr->next_hw;
    map_ptr->next_hw = NULL;		/* unlink it here now. */

    /*
     * insert at the end of link layer address link list
     */
    bcopy(hwaddr, &map_ptr->hwconf, sizeof(hwaddrtype));
    hash = static_hw_hash(hwaddr);
    for (map_link = &list_ptr->hwhash[hash]; *map_link;
					map_link = &((*map_link)->next_hw));
    *map_link = map_ptr;
    reset_interrupt_level(level);
    GET_TIMESTAMP(map_ptr->update_time);
}

/*
 * staticmap_update
 * Update class for static map
 */
void staticmap_update (
    maplist_type *list_ptr,
    staticmap_type *map_ptr, 
    hwaddrtype *hwconf_ptr,
    char *name)
{
    mapclass_type *newclass_ptr, *class_ptr = map_ptr->class;

    /* call map-type-specific function(s) to handle update of map.   */
    /* Any action not common to all maps is handled in the called fn(s). */
    reg_invoke_ext_staticmap_update(list_ptr,map_ptr,hwconf_ptr,name);

    /*
     * Update the HW address of this staticmap entry and rehash
     * as necessary.
     */
    if (hwconf_ptr)
    	staticmap_replace_hw(list_ptr, map_ptr, hwconf_ptr);

    /*
     * if static map doesn't have class defined
     *   if user just specified a class
     *     add class to static map
     * else
     *   if user just specified class
     *     update static map class
     *   else
     *     delete static map class
     */
    if (!class_ptr) {
	if (name && (*name != '\0')) {
	    /*
	     * if new class does not already exist, create an invisible one
	     * else link static map to existing class
	     */
	    class_ptr = mapclass_lookup(name, map_ptr->encap);
	    if (!class_ptr) {
		class_ptr = mapclass_create(name, map_ptr->encap, FALSE);
		if (class_ptr) {
		    enqueue(mapclassQ, class_ptr);
		} else {
                    printf("\n%% Can't add class name");
		    return;
		}
	    }
	    class_ptr->ref_count++;
	    map_ptr->class = class_ptr;
	}
    } else {
	if (name && (*name != '\0')) {
	    /*
             * class specified: if current class name != new class name
	     * replace class
             */
	    if (strcmp(class_ptr->name, name) != 0) {
                newclass_ptr = mapclass_lookup(name, map_ptr->encap);
                if (!newclass_ptr) {
                    newclass_ptr = mapclass_create(name, map_ptr->encap,
						   FALSE);
                    if (newclass_ptr) {
			enqueue(mapclassQ, newclass_ptr);
		    } else {
                        printf("\n%% Can't change class name");
			return;
		    }
                }
                class_ptr->ref_count--;
                if (!(class_ptr->ref_count || class_ptr->visible))
                    mapclass_delete(class_ptr);
		newclass_ptr->ref_count++;
		map_ptr->class = newclass_ptr;
	    } 
	} else {
	    /*
	     * no class specified: remove reference to current class
	     */
	    class_ptr->ref_count--;
	    if (!(class_ptr->ref_count || class_ptr->visible))
		mapclass_delete(class_ptr);
	    map_ptr->class = NULL;
	}
    }
    GET_TIMESTAMP(map_ptr->update_time);
}


/*
 * mapclass_lookup
 * Return pointer to map class identified by name and encap
 */
mapclass_type *mapclass_lookup (char *name, uint encap)
{
    mapclass_type *class_ptr;

    FOR_ALL_MAPCLASS(class_ptr) {
	if ((class_ptr->encap == encap) &&
	    (strcmp(name, class_ptr->name) == 0))
	    break;
    }

    return(class_ptr);
}

/*
 * mapclass_create
 * Return pointer to newly created map class, or NULL if failed
 */
mapclass_type *mapclass_create (char *name, uint encap, boolean visible)
{
    mapclass_type *class_ptr;

    class_ptr = malloc(sizeof(mapclass_type));

    if (class_ptr) {
       setstring(&class_ptr->name, name);
       /*
        * if failed to create class_ptr->name from the setstring
        * we have to free(class_ptr) to return(NULL)
        */
       if (!class_ptr->name) {
	   free(class_ptr);
           return(NULL);
       }
       class_ptr->next = NULL;
       class_ptr->ref_count = 0;
       class_ptr->visible = visible;
       class_ptr->encap = encap;
       class_ptr->params = NULL;
    }

    return(class_ptr);
}

/*
 * mapclass_delete
 * Delete map list
 */
static void mapclass_delete (mapclass_type *class_ptr)
{

    /*
     * if map class exists, make it invisible, physically delete it only
     * if ref_count is 0
     */
    if (class_ptr) {
        if (class_ptr->ref_count)
            class_ptr->visible = FALSE;
        else {
	    reg_invoke_mapclass_remove(class_ptr);
	    free(class_ptr->params);
            p_unqueue(mapclassQ, class_ptr);
            if (class_ptr->name)        /* free the associated memory */
                free(class_ptr->name);
            free(class_ptr);
        }
    }
}
/*
 * mapclass_command
 * Parse "map-class <name>" command
 */
void mapclass_command (parseinfo *csb)
{
    mapclass_type *class_ptr;
    const char *const_name;
    char *name;
    uint encap;

    if (csb->nvgen) {
	FOR_ALL_MAPCLASS(class_ptr) {
	    if (class_ptr->visible) {
		nv_write(TRUE, "!");
		const_name = encstring(class_ptr->encap);
		name = string_getnext();
		sstrncpy(name, const_name, 80);
		lowercase(name);
		nv_write(TRUE, "%s %s %s", csb->nv_command, name, 
			 class_ptr->name);
		if (set_mode_byname(&csb->mode, "map-class", MODE_SILENT)) {
		    reg_invoke_mapclass_nvgen(class_ptr->encap, class_ptr);
		    csb->mode = config_mode;
		}
	    }
	}
    } else {
	encap = GETOBJ(int, 1);
	name = GETOBJ(string, 1);
	class_ptr = mapclass_lookup(name, encap);

	if (csb->sense) {
	    /*
	     * if map class does not exist, create one
	     */
	    if (!class_ptr) {
		class_ptr = mapclass_create(name, encap, TRUE);

                /*
                 * if map class successfully created, enqueue it
                 * else notify user and return
                 */
                if (class_ptr) {
                    enqueue(mapclassQ, class_ptr);
                } else {
                    printf("\n%% Can't create map class");
                    return;
                }
            } else if (!class_ptr->visible) {
                /*
                 * class already exists, but not visible: unqueue it, requeue
		 * it as the last one and make it visible so it shows as the
		 * last one when doing 'write term'
                 */
		p_unqueue(mapclassQ, class_ptr);
		enqueue(mapclassQ, class_ptr);
                class_ptr->visible = TRUE;
	    }
	    csb->mode = map_class_mode;
	    csb->mapclass = class_ptr;
	    if (csb->flags & CONFIG_HTTP) {
		char *new_uri = string_getnext();
		char *old_uri = csb->http_state->uri;

		/* Setup new URI */
		sprintf(new_uri, "/%s/%s/%s", get_mode_name(csb->mode),
			encstring(class_ptr->encap), class_ptr->name);

		/* Call parser and generate help for new URI */
		csb->http_state->uri = new_uri;
		reg_invoke_http_parser(csb->http_state, csb->mode, FALSE);

		/* Put back old URI */
		csb->http_state->uri = old_uri;
	    }
	} else {
	    /*
	     * Used by FR SVCs. If someone frees a mapclass, 
	     * update all the static maps that use this map class
	     */
	    if (class_ptr) {
		mapclass_delete(class_ptr);
	    } else {
                printf("\n%% Can't delete map class '%s' - not configured",
		       name);
	    }
	}
    }
}

/*
 * mapclass_exit_command
 */
void mapclass_exit_command (parseinfo *csb)
{
    csb->mode = config_mode;
    csb->mapclass = NULL;
}

/*
 * maplist_lookup
 * Returns pointer to map list identified by name
 */
maplist_type *maplist_lookup (char *name)
{
    maplist_type *list_ptr;

    FOR_ALL_MAPLISTS(list_ptr) {
	if (strcmp(name, list_ptr->name) == 0)
	    break;
    }

    return(list_ptr);
}
 
/*
 * maplist_create
 * Return pointer to newly created map list, or NULL if failed
 */
maplist_type *maplist_create (char *name, boolean visible)
{
    maplist_type *list_ptr;

    list_ptr = malloc(sizeof(maplist_type));

    if (list_ptr) {
       setstring(&list_ptr->name, name);
       /*
        *If failed to create the list_ptr->name from setstring
        *we have to free(list_ptr) to return(NULL).
        */
       if (!list_ptr->name) {
           free(list_ptr);
           return(NULL);
       }
	list_ptr->next = NULL;
	list_ptr->ref_count = 0;
	list_ptr->visible = visible;
	memset(list_ptr->hash, 0, STATIC_MAP_HASH);
	memset(list_ptr->hwhash, 0, STATIC_MAP_HASH);
    }

    return(list_ptr);
}

/*
 * maplist_clear
 * Delete all static maps from a map list
 */
static void maplist_clear (maplist_type *list_ptr)
{
    staticmap_type *map_ptr, tmp_ptr;
    int i;

    FOR_ALL_MAPS_IN_LIST(i, list_ptr, map_ptr) {
	tmp_ptr.next = map_ptr->next;
	staticmap_delete(list_ptr, map_ptr);
	map_ptr = &tmp_ptr;
    }

    memset(list_ptr->hash, 0, STATIC_MAP_HASH);
    memset(list_ptr->hwhash, 0, STATIC_MAP_HASH);
}

/*
 * maplist_delete
 * Delete map list
 */
static void maplist_delete (maplist_type *list_ptr)
{

    /*
     * if map list exists, delete all static maps and make list
     * invisible, physically delete list only if ref_count is 0
     */
    if (list_ptr) {
        maplist_clear(list_ptr);
        if (list_ptr->ref_count)
            list_ptr->visible = FALSE;
        else {
            p_unqueue(maplistQ, list_ptr);
            if (list_ptr->name)         /* free this associated memory */
                free(list_ptr->name);
	    /*
	     * we'd better NULL its next field before freeing it.
	     * The same reason as for when freeing a map entry in this
	     * file.
	     */
	    list_ptr->next = NULL;
            free(list_ptr);
        }
    }
}

/*
 * maplist_command
 * Parse "map list <name>" command
 */
void maplist_command (parseinfo *csb)
{
    maplist_type *list_ptr;
    char *name;

    if (csb->nvgen) {
        staticmap_type *map_ptr;
        int i;

	FOR_ALL_MAPLISTS(list_ptr) {
	    if (list_ptr->visible) {
		nv_write(TRUE, "!");
		nv_write(TRUE, "%s %s", csb->nv_command, list_ptr->name);
		/*
		 * Some FR SVC stuff. At this point we don't know which interface
		 * this map-list will be associated with and hence, no filtering
		 * is possible. For FR SVCs a source address will always exist so 
		 * use that to decide.
		 */
		nv_add (list_ptr->addr.saddr.num_plan, " source-addr %s %s dest-addr %s %s",
		    (list_ptr->addr.saddr.num_plan == FR_E164_ADDR) ? "E164" : "X121",
		    (char *)&list_ptr->addr.saddr.addr,
		    (list_ptr->addr.daddr.num_plan == FR_E164_ADDR) ? "E164" : "X121",
		    (char *)&list_ptr->addr.daddr.addr);
		
		nv_add( list_ptr->addr.saddr.subaddr, " clps %d", 
					list_ptr->addr.saddr.subaddr);
		nv_add( list_ptr->addr.daddr.subaddr, " cdps %d", 
					list_ptr->addr.daddr.subaddr);
		if (list_ptr->priority_grp)
		    nv_add(TRUE, " %d", list_ptr->priority_grp);

		if (set_mode_byname(&csb->mode, "map-list", MODE_SILENT)) {
		    FOR_ALL_MAPS_IN_LIST(i, list_ptr, map_ptr) {
			if (map_ptr->map_type == MAP_STATIC) {
			   reg_invoke_staticmap_nvgen(map_ptr->encap, map_ptr);
		        }
		    }
		    csb->mode = config_mode;
		}
	    }
	}
    } else {
	name = GETOBJ(string, 1);
	list_ptr = maplist_lookup(name);

	if (csb->sense) {
            /*
             * if map list does not exist, create one
             */
	    if (!list_ptr) {
		list_ptr = maplist_create(name, TRUE);

		/*
		 * if map list successfully created, enqueue it
		 * else notify user and return
		 */
		if (list_ptr) {
		    enqueue(maplistQ, list_ptr);
		} else {
		    printf("\n%% Can't create map list");
		    return;
		}
	    } else if (!list_ptr->visible) {
		/*
		 * List already exists, but not visible: unqueue it, requeue it
		 * as the last one and make it visible
		 */
		p_unqueue(maplistQ, list_ptr);
		enqueue(maplistQ, list_ptr);
	        list_ptr->visible = TRUE;
	    }
		/*
		 * More FR SVC related stuff
		 * We need to make sure that the number type is coded as
		 * shown in page 80 of Q.931. "type of number" section.
		 */
		if (GETOBJ(int,1) && list_ptr) {
		    list_ptr->addr.saddr.num_plan = GETOBJ(int,1);
		    list_ptr->addr.saddr.subaddr = GETOBJ(int,4);
		    /* 
		     * Source address type
		     * National or international, brute force for now.
		     * 0x01 indicates an international number, only other 
		     * option is 0x0, "unknown" type.
		     */
		    list_ptr->addr.saddr.num_type = 0x01;
		    if (GETOBJ(int,1) == FR_E164_ADDR) {
			bcopy((uchar *)&GETOBJ(paddr,1)->addr,
				(uchar *)&list_ptr->addr.saddr.addr, STATIONLEN_E164);
		    } else {
			bcopy((uchar *)&GETOBJ(paddr,1)->addr,
				(uchar *)&list_ptr->addr.saddr.addr, STATIONLEN_X121-1);
		    }
		    list_ptr->addr.daddr.num_plan = GETOBJ(int,2);
		    list_ptr->addr.daddr.subaddr = GETOBJ(int,5);

		    /* 
		     * Destination address type
		     * National or international, brute force for now.
		     * 0x01 indicates an international number, only other 
		     * option is 0x0, "unknown" type.
		     */
		    list_ptr->addr.daddr.num_type = 0x01;
		    if (GETOBJ(int,2) == FR_E164_ADDR) {
		    	bcopy((uchar *)&GETOBJ(paddr,2)->addr,
		    		(uchar *)&list_ptr->addr.daddr.addr, STATIONLEN_E164);
		    } else {
		    	bcopy((uchar *)&GETOBJ(paddr,2)->addr,
		    		(uchar *)&list_ptr->addr.daddr.addr, STATIONLEN_X121-1);
		    }
		    if (GETOBJ(int,1) && (GETOBJ(int,1) == GETOBJ(int,2)) &&
		    	(strcmp((uchar *)&list_ptr->addr.saddr.addr,
		    		(uchar *)&list_ptr->addr.daddr.addr) == 0)) {
		    	printf("\nError: Destination address same as Source address");
		    	if ((list_ptr) && list_ptr->visible)
		    	    free(list_ptr);
		    	return;
		    }
		    list_ptr->addr.saddr.num_len =
		    			strlen((char *)&list_ptr->addr.saddr.addr);
		    list_ptr->addr.daddr.num_len =
		    			strlen((char *)&list_ptr->addr.daddr.addr);
	    }
	    csb->mode = map_list_mode;
	    csb->maplist = list_ptr;
	    if (csb->flags & CONFIG_HTTP) {
		char *new_uri = string_getnext();
		char *old_uri = csb->http_state->uri;

		/* Setup new URI */
		sprintf(new_uri, "/%s/%s",
			get_mode_name(csb->mode), list_ptr->name);

		/* Call parser and generate help for new URI */
		csb->http_state->uri = new_uri;
		reg_invoke_http_parser(csb->http_state, csb->mode, FALSE);

		/* Put back old URI */
		csb->http_state->uri = old_uri;
	    }
	} else {
	    if (list_ptr && list_ptr->visible)
		maplist_delete(list_ptr);
	    else
                printf("\n%% Can't delete map list '%s' - not configured", name);
	}
    }
}
/*
 * maplist_exit_command
 */
void maplist_exit_command (parseinfo *csb)
{
    csb->mode = config_mode;
    csb->maplist = NULL;
}

/*
 * mapgroup_lookup
 * Return pointer to map group identified by name belonging to idb
 */
mapgroup_type *mapgroup_lookup (idbtype *idb, char *name)
{
    mapgroup_type *group_ptr = NULL;
    maplist_type *list_ptr;

    FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
	list_ptr = group_ptr->maplist;
        if (strcmp(name, list_ptr->name) == 0)
            break;
    }

    return(group_ptr);
}

/*
 * mapgroup_add
 * Add map group to idb map group linked list
 */
void mapgroup_add (idbtype *idb, mapgroup_type *group_ptr)
{
    mapgroup_type **mapgrp;

    /*
     * add at the end of the list
     */
    for (mapgrp = &idb->mapgroup; *mapgrp; mapgrp = &((*mapgrp)->next));

    *mapgrp = group_ptr;
}

/*
 * mapgroup_delete
 * Delete map group from idb map group linked list
 */
void mapgroup_delete (idbtype *idb, mapgroup_type *group_ptr)
{
    maplist_type *list_ptr;
    mapgroup_type **mapgrp;

    list_ptr = group_ptr->maplist;

    /* call map-type-specific function(s) to handle deletion of group. */
    /* NOTE: mapgroup is freed HERE. Any other action not common to all*/
    /*       maps is handled in the called fn(s). */
    reg_invoke_ext_mapgroup_delete(idb, list_ptr);
 
    /*
     * find element to unlink
     */
    for (mapgrp = &idb->mapgroup; *mapgrp && (*mapgrp != group_ptr);
                                                 mapgrp = &((*mapgrp)->next));

    *mapgrp = group_ptr->next;

    /*
     * decrement map list reference count, and delete map list if
     * reference count is equal to 0 and map list is not visible
     */
    list_ptr = group_ptr->maplist;
    list_ptr->ref_count--;
    if (!(list_ptr->ref_count || list_ptr->visible))
        maplist_delete(group_ptr->maplist);

    free(group_ptr);
}

/*
 * mapgroup_create
 * Create a mapgroup struct. init & assign it to the IDB
 */
void mapgroup_create (uchar *group_name, uchar *list_name, idbtype *idb)
{
    mapgroup_type *group_ptr;
    maplist_type *list_ptr;

    group_ptr = malloc(sizeof(mapgroup_type));
    if (!group_ptr) {
        printf(nomemory);
	return;
    }

    /*
     * if map group successfully created, add it to the idb
     * map group linked list , else notify user and return
     */
    list_ptr = maplist_lookup(list_name);

    /*
     * if map list does not exist, create an invisible one
     * as a name holder
     */
    if (!list_ptr) {
        list_ptr = maplist_create(list_name, FALSE);

        /*
         * if map list successfully created, enqueue it
         * else notify user and return
         */
        if (list_ptr) {
	    enqueue(maplistQ, list_ptr);
	} else {
            printf("\n%% Can't create map group");
            return;
        }
    }

    list_ptr->ref_count++;
    group_ptr->next = NULL;
    group_ptr->maplist = list_ptr;
    mapgroup_add(idb, group_ptr);
    
}

/*
 * mapgroup_command
 * Parse "map-group <name>" command
 */
void mapgroup_command (parseinfo *csb)
{
    mapgroup_type *group_ptr;
    maplist_type *list_ptr;
    idbtype *idb = csb->interface;
    char *name;
    hwidbtype *hwidb;
    long delta = 0;

    /*
     * Sanity check
     */
    if (!idb)
        return;

    if (csb->nvgen) {
	FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
	    list_ptr = group_ptr->maplist;
	    if(list_ptr->visible) 
            	nv_write(TRUE, "%s %s", csb->nv_command, list_ptr->name);
	}
    } else {
	name = GETOBJ(string, 1);
        group_ptr = mapgroup_lookup(idb, name);

        if (csb->sense) {
	    if (is_atm_lane(idb)) {
		printf("\n%%ATM map-group command conflicts with LANE client.");
		return;
	    }
	    
	    /* don't permit map-group on point-to-point ATM interface */
	    if ((is_atm(idb->hwptr)) && (is_p2p(idb))) {
		printf ("\nATM: cannot configure map-group on point-to-point"
			" sub-interface.");
		return;
	    }
		
            /*
             * if map group does not exist, create one
             */
            if (!group_ptr) {
		mapgroup_create(name, name, idb);
	    }

	    /*
	     * We are configuring a map-list. Use the default value of
	     * maxmtu_def, if there is no user configured MTU.
	     * 
	     */
	    hwidb = idb->hwptr;
	    if (idb->user_configured_mtu) {
		/* 
		 * no need to force a re-carve of buffer. Use the sub-if
		 * MTU, as set at the interface creation time.
		 */
		delta = 0;
	    } else {
		delta = hwidb->maxmtu_def - idb->sub_mtu;
		idb->sub_mtu = hwidb->maxmtu_def;
	    }
	    /*
	     * Set the maximum mtu on the hwidb, based on all the sub-mtu
	     * values in the sub-interfaces.
	     */

	    set_max_interface_mtu(idb, delta);

        } else {
            /*
             * if map group exists, delete it
             */
            if (group_ptr)
                mapgroup_delete(idb, group_ptr);
            else
		printf("\n%% Can't delete map group '%s' - not configured",
		       name);
        }
    }
}

/*
 * static_map_init
 * Initialize static map related information
 */
static void static_map_init (subsystype* subsys)
{
    queue_init(maplistQ, 0);
    queue_init(mapclassQ, 0);

    static_map_parser_init();
}

/*
 * Static map subsystem header
 */

#define STATICMAP_MAJVERSION 1
#define STATICMAP_MINVERSION 0
#define STATICMAP_EDITVERSION 1

SUBSYS_HEADER(static_map, STATICMAP_MAJVERSION, STATICMAP_MINVERSION,
	      STATICMAP_EDITVERSION, 
	      static_map_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);
