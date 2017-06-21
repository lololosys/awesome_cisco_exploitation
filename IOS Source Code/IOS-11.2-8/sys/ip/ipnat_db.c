/* $Id: ipnat_db.c,v 3.1.6.20 1996/09/07 01:07:14 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_db.c,v $
 *------------------------------------------------------------------
 * IP Network Address Translation database manipulation
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_db.c,v $
 * Revision 3.1.6.20  1996/09/07  01:07:14  ahh
 * CSCdi68015:  NAT: multi-homing not quite right
 * Route-map-based translation needs a slightly different address
 * allocation strategy -- each inside host gets one address per
 * route-map.  Do a little cleanup.
 * Branch: California_branch
 *
 * Revision 3.1.6.19  1996/08/28  22:57:49  ahh
 * CSCdi65898:  NAT: rsh not working when overloading
 * rsh wants rshd to connect back to it for stderr, so need to allow this
 * flow from a random privileged port in.
 * Branch: California_branch
 *
 * Revision 3.1.6.18  1996/08/28  12:50:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.6.17  1996/08/22  03:10:45  ahh
 * CSCdi66786:  NAT: address stolen from deleted pool
 * Check the return codes, that's what they're there for.
 * Branch: California_branch
 *
 * Revision 3.1.6.16  1996/08/22  00:27:50  ahh
 * CSCdi66753:  NAT: portlists not reaped when a mapping is deleted
 * As ye sow, so shall ye reap (or something like that).
 * Branch: California_branch
 *
 * Revision 3.1.6.15  1996/08/13  21:27:37  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.14  1996/08/08  18:02:28  ahh
 * CSCdi65508:  NAT: deleted interface appears in show command
 * Skip deleted interfaces.
 * Branch: California_branch
 *
 * Revision 3.1.6.13  1996/07/30  23:43:00  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.1.6.12  1996/07/29  22:45:55  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.1.6.11  1996/07/19  18:38:48  ahh
 * CSCdi63523:  NAT: more interrupt/process foolishness
 * Branch: California_branch
 *
 * Revision 3.1.6.10  1996/07/18  00:37:48  ahh
 * CSCdi63299:  NAT: managed timers are not automatically interrupt safe
 * Read the docs, dude.
 * Branch: California_branch
 *
 * Revision 3.1.6.9  1996/06/29  00:11:26  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.1.6.8  1996/06/20  02:26:02  ahh
 * CSCdi60871:  NAT: show when a translation would time out
 * Branch: California_branch
 *
 * Revision 3.1.6.7  1996/06/19  00:54:45  ahh
 * CSCdi60749:  NAT: rotary not working with simple acls
 * Branch: California_branch
 *
 * Revision 3.1.6.6  1996/06/06  18:05:25  ahh
 * CSCdi59725:  ip nat static addresses fail to write out properly
 * Put the oddball translations on the right list.
 * Branch: California_branch
 *
 * Revision 3.1.6.5  1996/06/03  23:23:13  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.4  1996/05/13  23:21:01  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/04  01:04:35  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.1.6.2  1996/04/24  00:06:28  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:21  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:25  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include "types.h"
#include "interface_private.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "logger.h"
#include "../ip/msg_ipnat.c"		/* Not a typo, see logger.h */
#include "ip.h"
#include "ip_debug.h"
#include "ip_registry.h"
#include "mgd_timers.h"
#include "access.h"
#include "ipaccess.h"
#include "ttysrv.h"		/* for printbits() */
#include "config.h"
#include "parser.h"
#include "parser_defs_ipnat.h"
#include "../parser/parser_defs_access.h"
#include "../os/chunk.h"
#include "../ui/service.h"
#include "../iprouting/route.h"
#include "../iprouting/route_map.h"
#include "ipaccess_private.h"
#include "ipaccess2.h"
#include "ipnat_private.h"
#include "ipnat_pool.h"

ipnat_entry *ipnat_statics[IPNAT_DOMAINS][IPNAT_LOOKUPS];
struct ipnat_timeout ipnat_timeouts[IPNAT_NUM_TIMEOUTS];
list_header ipnat_door_list, ipnat_fragment_list;

static ipnat_entry **ipnat_hash[IPNAT_LOOKUPS];
static ipnat_entry **ipnat_map_hash[IPNAT_LOOKUPS];
static list_header ipnat_portlist_hdrs[IPNAT_LOOKUPS];
static chunk_type *nat_chunks, *port_range_chunks;

/* Shouldn't need this forward reference, but I do. */
static ipaddrtype ipnat_steal_address(uchar, ipnat_mapping *, ushort,
				      enum ipnat_lookup);

/*
 * ipnat_hash_fn
 *
 * Just like nethash, but incorporates a port number.
 */

static int
ipnat_hash_fn (ulong addr1, ulong addr2, ushort port1, ushort port2)
{
    ushort bucket;

    bucket = addr1 >> 16;
    bucket ^= addr1 & 0xffff;
    bucket ^= port1;
    bucket ^= addr2 >> 16;
    bucket ^= addr2 & 0xffff;
    bucket ^= port2;
    bucket &= (IPNAT_HASHLEN - 1);	/* assumes this is a power of 2 */
    return(bucket);
}

/*
 * ipnat_add_hash
 *
 * Add a new entry to the two hash tables.
 */

static void
ipnat_add_hash (ipnat_entry *nat)
{
    enum ipnat_lookup lookup;
    int index;
    leveltype status;

    /*
     * The translation database can be accessed at interrupt level,
     * so these accesses must be protected.
     */
    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * Add to local and global hashes.
     */
    for (lookup = 0; lookup < IPNAT_LOOKUPS; lookup++) {
	index = ipnat_hash_fn(nat->address[IPNAT_INSIDE][lookup],
			      nat->address[IPNAT_OUTSIDE][lookup],
			      nat->port[IPNAT_INSIDE][lookup],
			      nat->port[IPNAT_OUTSIDE][lookup]);
#ifdef DEBUG
	if (ipnat_detailed_debug)
	    buginf("\nadd_hash: hash %i %i %d %d, %s %d",
		   nat->address[IPNAT_INSIDE][lookup],
		   nat->address[IPNAT_OUTSIDE][lookup],
		   nat->port[IPNAT_INSIDE][lookup],
		   nat->port[IPNAT_OUTSIDE][lookup],
		   (lookup == IPNAT_LOCAL) ? "local" : "global", index);
#endif
	nat->hash_next[lookup] = ipnat_hash[lookup][index];
	ipnat_hash[lookup][index] = nat;
    }

    reset_interrupt_level(status);
}

/*
 * ipnat_remove_hash
 *
 * Remove a translation from the hash tables.
 */

static void
ipnat_remove_hash (ipnat_entry *nat)
{
    enum ipnat_lookup lookup;
    int index;
    ipnat_entry *nat_iter, *last;

    /*
     * Remove from local and global hashes.
     */
    for (lookup = 0; lookup < IPNAT_LOOKUPS; lookup++) {
	index = ipnat_hash_fn(nat->address[IPNAT_INSIDE][lookup],
			      nat->address[IPNAT_OUTSIDE][lookup],
			      nat->port[IPNAT_INSIDE][lookup],
			      nat->port[IPNAT_OUTSIDE][lookup]);
	last = NULL;
	for (nat_iter = ipnat_hash[lookup][index]; nat_iter;
	     nat_iter = nat_iter->hash_next[lookup]) {
	    if (nat_iter == nat) {
		if (last)
		    last->hash_next[lookup] = nat_iter->hash_next[lookup];
		else
		    ipnat_hash[lookup][index] = nat_iter->hash_next[lookup];
		break;
	    }
	    last = nat_iter;
	}
	nat->hash_next[lookup] = NULL;
    }
}

/*
 * ipnat_free_all_hash
 *
 * Delete every dynamic entry in the hash tables.  A bit expensive,
 * but that shouldn't matter too much.
 */

void
ipnat_free_all_hash (void)
{
    ipnat_entry *nat, *next;
    int i;

    for (i = 0; i < IPNAT_HASHLEN; i++) {
	for (nat = ipnat_hash[IPNAT_GLOBAL][i]; nat; nat = next) {
	    next = nat->hash_next[IPNAT_GLOBAL];
	    if (!(nat->flags & IPNF_STATIC))
		ipnat_delete_entry(nat);
	}
    }
}

/*
 * ipnat_find_statics
 *
 * Find the right statics linked-list given a bunch of flags
 */

ipnat_entry **
ipnat_find_statics (uchar flags)
{
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;

    if (flags & IPNF_OUTSIDE) {
	domain = IPNAT_OUTSIDE;
	if (flags & IPNF_DEST)
	    lookup = IPNAT_LOCAL;
	else
	    lookup = IPNAT_GLOBAL;
    } else {
	domain = IPNAT_INSIDE;
	if (flags & IPNF_DEST)
	    lookup = IPNAT_GLOBAL;
	else
	    lookup = IPNAT_LOCAL;
    }

    return(&ipnat_statics[domain][lookup]);
}

/*
 * ipnat_add_static
 *
 * Add a new static entry to the static list.
 */

static void
ipnat_add_static (ipnat_entry *nat)
{
    ipnat_entry **statics;

    statics = ipnat_find_statics(nat->flags);
    nat->static_next = *statics;
    *statics = nat;
}

/*
 * ipnat_remove_static
 *
 * Remove a static translation from the static list.
 */

static void
ipnat_remove_static (ipnat_entry *nat)
{
    ipnat_entry **statics;
    ipnat_entry *nat_iter, *last;

    statics = ipnat_find_statics(nat->flags);
    last = NULL;
    for (nat_iter = *statics; nat_iter; nat_iter = nat_iter->static_next) {
	if (nat_iter == nat) {
	    if (last)
		last->static_next = nat_iter->static_next;
	    else
		*statics = nat_iter->static_next;
	    break;
	}
	last = nat_iter;
    }
    nat->static_next = NULL;
}

/*
 * ipnat_map_hash_init
 *
 * Allocate and initialize the auxiliary hash tables used
 * to be able to find the inside local->global translation
 * in use for a particular mapping.
 */

static boolean
ipnat_map_hash_init (void)
{
    ipnat_map_hash[IPNAT_LOCAL] = malloc(sizeof(ipnat_entry *) *
					 IPNAT_HASHLEN);
    if (ipnat_map_hash[IPNAT_LOCAL] == NULL)
	return(FALSE);
    ipnat_map_hash[IPNAT_GLOBAL] = malloc(sizeof(ipnat_entry *) *
					  IPNAT_HASHLEN);
    if (ipnat_map_hash[IPNAT_GLOBAL] == NULL) {
	free(ipnat_map_hash[IPNAT_GLOBAL]);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ipnat_map_hash_fn
 *
 */

static int
ipnat_map_hash_fn (ulong addr, ipnat_mapping *mapping)
{
    ushort bucket;
    ulong temp = (ulong) mapping;

    bucket = addr >> 16;
    bucket ^= addr & 0xffff;
    bucket ^= temp >> 16;
    bucket ^= temp & 0xffff;
    bucket &= (IPNAT_HASHLEN - 1);	/* assumes this is a power of 2 */
    return(bucket);
}

/*
 * ipnat_add_map_hash
 *
 * Add an entry to the route-map hash table.
 */

static void
ipnat_add_map_hash (ipnat_entry *nat)
{
    enum ipnat_lookup lookup;
    int index;

    if (ipnat_map_hash[IPNAT_LOCAL] == NULL) {
	if (!ipnat_map_hash_init())
	    return;
    }

    /*
     * Add to local and global hashes.
     */
    for (lookup = 0; lookup < IPNAT_LOOKUPS; lookup++) {
	index = ipnat_map_hash_fn(nat->address[IPNAT_INSIDE][lookup],
				  nat->mapping[IPNAT_INSIDE]);
	nat->map_hash_next[lookup] = ipnat_map_hash[lookup][index];
	ipnat_map_hash[lookup][index] = nat;
    }
}

/*
 * ipnat_remove_map_hash
 *
 * Remove an entry from the route-map hash table.
 */

static void
ipnat_remove_map_hash (ipnat_entry *nat)
{
    enum ipnat_lookup lookup;
    int index;
    ipnat_entry *nat_iter, *last;

    if (ipnat_map_hash[IPNAT_LOCAL] == NULL)
	return;

    /*
     * Remove from local and global hashes.
     */
    for (lookup = 0; lookup < IPNAT_LOOKUPS; lookup++) {
	index = ipnat_map_hash_fn(nat->address[IPNAT_INSIDE][lookup],
				  nat->mapping[IPNAT_INSIDE]);
	last = NULL;
	for (nat_iter = ipnat_map_hash[lookup][index]; nat_iter;
	     nat_iter = nat_iter->map_hash_next[lookup]) {
	    if (nat_iter == nat) {
		if (last)
		    last->map_hash_next[lookup] =
			nat_iter->map_hash_next[lookup];
		else
		    ipnat_map_hash[lookup][index] =
			nat_iter->map_hash_next[lookup];
		break;
	    }
	    last = nat_iter;
	}
	nat->map_hash_next[lookup] = NULL;
    }
}

/*
 * ipnat_find_map_entry
 *
 * Find an entry in the route-map hash table.
 */

static ipnat_entry *
ipnat_find_map_entry (ipaddrtype address, ipnat_mapping *mapping,
		      enum ipnat_lookup lookup)
{
    int index;
    ipnat_entry *nat;

    if (ipnat_map_hash[IPNAT_LOCAL] == NULL)
	return(NULL);

    index = ipnat_map_hash_fn(address, mapping);
    for (nat = ipnat_map_hash[lookup][index]; nat;
	 nat = nat->map_hash_next[lookup]) {
	if (address == nat->address[IPNAT_INSIDE][lookup] &&
	    mapping == nat->mapping[IPNAT_INSIDE])
	    return(nat);
    }
    return(NULL);
}

/*
 * ipnat_lock_nat
 * ipnat_free_nat
 *
 * Lock and free routines for nat entries.
 * Don't call mem_lock() or free()/chunk_free(), use these instead.
 */

static void
ipnat_lock_nat (ipnat_entry *nat)
{
    nat->refcount++;
}

static void
ipnat_free_nat (ipnat_entry *nat)
{
    nat->refcount--;
    if (nat->refcount == 0)
	chunk_free(nat_chunks, nat);
}

/*
 * ipnat_start_inactivity_timer
 *
 * Start the inactivity timer for an entry.
 */

static void
ipnat_start_inactivity_timer (ipnat_entry *nat)
{
    int timeout;
    boolean timeout_enabled;

    timeout_enabled = FALSE;

    switch (nat->proto) {
    case TCP_PROT:
	/*
	 * If timing-out, the correct timer value to use is the finrst
	 * timeout.  Otherwise, use the normal TCP timeout.
	 */
	if (nat->flags & IPNF_TIMING_OUT) {
	    timeout = ipnat_timeouts[IPNAT_FINRST_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_FINRST_TIMEOUT].enabled;
	} else {
	    timeout = ipnat_timeouts[IPNAT_TCP_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_TCP_TIMEOUT].enabled;
	}
	break;

    case UDP_PROT:
	/*
	 * Use the normal UDP timeout unless this is DNS (allows
	 * a shorter timeout value for name lookups).
	 */
	if (nat->port[IPNAT_INSIDE][IPNAT_LOCAL] == DNS_PORT ||
	    nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] == DNS_PORT) {
	    timeout = ipnat_timeouts[IPNAT_DNS_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_DNS_TIMEOUT].enabled;
	} else {
	    timeout = ipnat_timeouts[IPNAT_UDP_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_UDP_TIMEOUT].enabled;
	}
	break;

    case ICMP_PROT:
	timeout = ipnat_timeouts[IPNAT_ICMP_TIMEOUT].timeout;
	timeout_enabled = ipnat_timeouts[IPNAT_ICMP_TIMEOUT].enabled;
	break;

    default:
	if ((nat->mapping[IPNAT_INSIDE] &&
	     (nat->mapping[IPNAT_INSIDE]->flags & IPNMF_OVERLOAD)) ||
	    (nat->flags & IPNF_DEST)) {
	    /*
	     * We know TCP traffic isn't using this translation
	     * entry, so go for a more aggressive timeout.
	     */
	    timeout = ipnat_timeouts[IPNAT_UDP_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_UDP_TIMEOUT].enabled;
	} else {
	    timeout = ipnat_timeouts[IPNAT_TIMEOUT].timeout;
	    timeout_enabled = ipnat_timeouts[IPNAT_TIMEOUT].enabled;
	}
	break;
    }

    if (timeout_enabled)
	mgd_timer_start(&nat->inactivity_timer, timeout);
}

/*
 * ipnat_create_entry
 *
 * Create a translation table entry and put it in the hash tables.
 */

ipnat_entry *
ipnat_create_entry (ipaddrtype inside_localaddr, ipaddrtype inside_globaladdr,
		    ushort inside_localport, ushort inside_globalport,
		    ipaddrtype outside_localaddr,
		    ipaddrtype outside_globaladdr,
		    ushort outside_localport, ushort outside_globalport,
		    uchar proto, uchar flags)
{
    ipnat_entry *nat;

    /*
     * Limit the number of dynamic entries that can be created.
     * You can have as many static entries as you can stomach.
     */
    if (!(flags & IPNF_STATIC) && ipnat_max_entries != IPNAT_NO_MAX &&
	(ipnat_stats.entries - ipnat_stats.statics) >= ipnat_max_entries) {
	if (ipnat_debug)
	    buginf("\nNAT: administratively-defined entry limit reached (%d)",
		   ipnat_max_entries);
	return(NULL);
    }

    nat = chunk_malloc(nat_chunks);
    if (nat == NULL) {
	/* Sorry, no memory. */
	return(NULL);
    }

    /*
     * Bump the accessor count since we need mem_lock/free semantics
     * and this is a chunk, not a malloced block.
     */
    ipnat_lock_nat(nat);

    GET_TIMESTAMP(nat->create_time);
    GET_TIMESTAMP(nat->used_time);
    nat->address[IPNAT_INSIDE][IPNAT_LOCAL] = inside_localaddr;
    nat->address[IPNAT_INSIDE][IPNAT_GLOBAL] = inside_globaladdr;
    nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL] = outside_localaddr;
    nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL] = outside_globaladdr;
    if (flags & IPNF_EXTENDED) {
	nat->proto = proto;
	nat->port[IPNAT_INSIDE][IPNAT_LOCAL] = inside_localport;
	nat->port[IPNAT_INSIDE][IPNAT_GLOBAL] = inside_globalport;
	nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL] = outside_localport;
	nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] = outside_globalport;
    }
    nat->flags = flags;

    if (!(flags & IPNF_STATIC)) {
	/*
	 * Dynamic entries time out.
	 */
	mgd_timer_init_leaf(&nat->inactivity_timer, &ipnat_entry_timer_master,
			    0, nat, TRUE);
	ipnat_start_inactivity_timer(nat);
    }

    /*
     * Add the new entry to the hash tables.
     */
    ipnat_add_hash(nat);

    /*
     * Add the new entry to the appropriate static list if it's static.
     */
    if (flags & IPNF_STATIC)
	ipnat_add_static(nat);

    /*
     * One more entry.
     */
    ipnat_stats.entries++;
    if (nat->flags & IPNF_EXTENDED)
	ipnat_stats.flows++;

    return(nat);
}

/*
 * ipnat_delete_entry
 *
 * Delete a translation entry.
 */

void
ipnat_delete_entry (ipnat_entry *nat)
{
    ipaddrtype address;
    ipnat_mapping *mapping;
    leveltype status;

    if (!nat)
	return;

    /*
     * The translation database can be accessed at interrupt level,
     * so these accesses must be protected.
     */
    status = raise_interrupt_level(NETS_DISABLE);
    ipnat_remove_hash(nat);
    reset_interrupt_level(status);

    if (!(nat->flags & IPNF_STATIC)) {
	if (nat->mapping[IPNAT_INSIDE]) {
	    if (!(nat->more_flags & IPNF2_PORT_STOLEN) &&
		(nat->flags & (IPNF_DEST|IPNF_EXTENDED)) == IPNF_EXTENDED)
		ipnat_free_port(nat->proto,
				nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
				IPNAT_GLOBAL);

	    /* Figure out which one was allocated */
	    if (nat->flags & IPNF_DEST)
		address = nat->address[IPNAT_INSIDE][IPNAT_LOCAL];
	    else
		address = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	    mapping = nat->mapping[IPNAT_INSIDE];
	    if (mapping->poolname) {
		ipnat_addrpool_free(mapping->poolname, address);
		if (samecable(address) &&
		    ipnat_addrpool_refcount(mapping->poolname, address) == 0) {
		    if (ipnat_detailed_debug)
			buginf("\nNAT: deleting alias for %i", address);
		    reg_invoke_ipalias_delete_dynamic(address);
		}
	    }
	    mapping->refcount--;
	    if (mapping->match_type == IPNAT_MATCH_ROUTE_MAP)
		ipnat_remove_map_hash(nat);
	}

	if ((nat->flags & IPNF_OUTSIDE) && nat->mapping[IPNAT_OUTSIDE]) {
	    /* Figure out which one was allocated */
	    if (nat->flags & IPNF_DEST)
		address = nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	    else
		address = nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	    mapping = nat->mapping[IPNAT_OUTSIDE];
	    if (mapping->poolname)
		ipnat_addrpool_free(mapping->poolname, address);
	    mapping->refcount--;
	}

	mgd_timer_stop(&nat->inactivity_timer);
    } else {
	ipnat_remove_static(nat);
    }

    /*
     * Delete any stream editing information.
     */
    if (nat->edit_context) {
	ipnat_destroy_all_seq_delta_pairs(nat->edit_context);
	ipnat_delete_edit_context(nat->edit_context);
	nat->edit_context = NULL;
    }

    ipnat_free_nat(nat);

    /*
     * One less entry.
     */
    ipnat_stats.entries--;
    if (nat->flags & IPNF_EXTENDED)
	ipnat_stats.flows--;
}

/*
 * ipnat_reshuffle_nat
 *
 * Reshuffle the given entry to the head of its hash chain.
 */

static inline void
ipnat_reshuffle_nat (ipnat_entry *nat, ipnat_entry *last, int index,
		     enum ipnat_lookup lookup)
{
    last->hash_next[lookup] = nat->hash_next[lookup];
    nat->hash_next[lookup] = ipnat_hash[lookup][index];
    ipnat_hash[lookup][index] = nat;
}

/*
 * ipnat_find_entry
 *
 * Find an entry.
 */

ipnat_entry *
ipnat_find_entry (ipaddrtype insideaddr, ipaddrtype outsideaddr,
		  ushort insideport, ushort outsideport, uchar proto,
		  enum ipnat_lookup lookup)
{
    ipnat_entry *nat, *last, *maybe_nat, *maybe_last;
    int index;
    leveltype status;

#ifdef DEBUG
    if (ipnat_detailed_debug)
	buginf("\nfind_entry: hash %i %i %d %d, %s",
	       insideaddr, outsideaddr, insideport, outsideport,
	       (lookup == IPNAT_LOCAL) ? "local" : "global");
#endif
    index = ipnat_hash_fn(insideaddr, outsideaddr, insideport, outsideport);

    last = maybe_nat = maybe_last = NULL;

    /*
     * The translation database can be accessed (and entries
     * reshuffled) at interrupt level, so the search must be protected.
     */
    status = raise_interrupt_level(NETS_DISABLE);

    for (nat = ipnat_hash[lookup][index]; nat; nat = nat->hash_next[lookup]) {
	if (insideaddr == nat->address[IPNAT_INSIDE][lookup]) {
	    if (nat->flags & IPNF_EXTENDED) {
		if (insideport == nat->port[IPNAT_INSIDE][lookup] &&
		    proto == nat->proto) {
		    if (outsideaddr) {
			if (outsideaddr == nat->address[IPNAT_OUTSIDE][lookup] &&
			    outsideport == nat->port[IPNAT_OUTSIDE][lookup]) {
			    /* Hit -- everything matched.  */
			    break;
			}
		    } else {
			/* Hit -- inside address and port matched extended. */
			break;
		    }
		}
	    } else if (nat->flags & IPNF_OUTSIDE) {
		if (outsideaddr == nat->address[IPNAT_OUTSIDE][lookup]) {
		    /* Hit -- everything matched.  */
		    break;
		}
	    } else if (!proto) {
		/* Hit -- inside address matched a simple entry. */
		maybe_nat = nat;
		maybe_last = last;
	    }
	}
	last = nat;
    }
    
    if (nat == NULL) {
	nat = maybe_nat;
	last = maybe_last;
    }

    if (nat && last)
	ipnat_reshuffle_nat(nat, last, index, lookup);

    reset_interrupt_level(status);

    if (nat) {
	/*
	 * Got a match, update the use time and start the inactivity
	 * timer if this is entry was dynamically created.
	 */
	GET_TIMESTAMP(nat->used_time);
	if (!(nat->flags & IPNF_STATIC))
	    ipnat_start_inactivity_timer(nat);
    }

    return(nat);
}

/*
 * ipnat_match_acl
 *
 */

static boolean
ipnat_match_acl (acl_headertype *acl, ipaddrtype address, paktype *pak)
{
    iphdrtype *ip;

    /*
     * If the ACL doesn't exist, DO NOT consider this to be a match.
     */
    if (ipaccess_emptylist(acl))
	return(FALSE);

    /*
     * Some types of access-lists will allow non-zero fragments
     * since they have no way of knowing if fragment zero could
     * have been allowed or not.  We DO NOT want a match in this
     * case because this will cause an unwanted simple translation
     * to be created.
     */
    ip = (iphdrtype *) ipheadstart(pak);
    if (getfo(ip) && ipaccess_willpassfrags(acl))
	return(FALSE);

    /*
     * Now hit the access-list.
     */
    switch (acl->type) {
    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	if (nfast_check(acl, address))
	    return(TRUE);
	break;

    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:
	if (ip_accesscheck(pak, acl))
	    return(TRUE);
	break;

    default:
	break;
    }

    /*
     * No clue.
     */
    return(FALSE);
}

/*
 * ipnat_match_ipaddress
 *
 */

static inline boolean
ipnat_match_ipaddress (route_map_match_acl *match_list, ipaddrtype address,
		       paktype *pak)
{
    route_map_match_acl *aptr;

    /*
     * Nothing in list, therefore it DOES NOT match.
     */
    if (!match_list)
	return(FALSE);

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (aptr = match_list; aptr; aptr = aptr->next) {
	if (ipnat_match_acl(aptr->acl, address, pak))
	    return(TRUE);
    }

    return(FALSE);
}

/*
 * ipnat_match_route_map
 *
 */

static boolean
ipnat_match_route_map (route_maptype *map, ipaddrtype address, paktype *pak)
{
    for (; map; map = map->next) {
	/*
	 * Scan the route-map, looking for failures.
	 */
	if ((map->match_list.ip_address != NULL) &&
	    !ipnat_match_ipaddress(map->match_list.ip_address, address, pak))
	    continue;

	if ((map->match_list.match_flags & ROUTE_MAP_MATCH_INTERFACE) &&
	    (map->match_list.interface->idb != pak->if_output))
	    continue;

	if ((map->match_list.match_flags & ROUTE_MAP_MATCH_IPNEXTHOP) &&
	    !route_map_match_ipaddress(pak->desthost, IPADDR_ZERO,
				       map->match_list.ip_next_hop))
	    continue;

	/*
	 * No failures, must be a match.
	 */
	return(map->grant);
    }

    return(FALSE);
}

/*
 * ipnat_find_mapping
 *
 * Find the mapping that would be used for a particular
 * untranslated address.
 */

ipnat_mapping *
ipnat_find_mapping (paktype *pak, ipaddrtype address, enum ipnat_lookup lookup,
		    enum ipnat_domain domain)
{
    ipnat_mapping *mapping;
    list_header *mappings;
    list_element *el;
    route_map_headtype *head;

    /* Figure out which set of pools to use */
    mappings = &ipnat_mappings[domain][lookup];

    /*
     * Using the access-list(s) find the pool to
     * allocate the new address from.
     */
    FOR_ALL_DATA_IN_LIST(mappings, el, mapping) {
	switch (mapping->match_type) {
	case IPNAT_MATCH_ACL:
	    if (ipnat_match_acl(mapping->acl, address, pak))
		return(mapping);
	    break;

	case IPNAT_MATCH_ROUTE_MAP:
	    head = find_route_map_head(mapping->mapname);
	    if (!head)
		break;

	    if (ipnat_match_route_map((route_maptype *) head->map_queue.qhead,
				      address, pak)) {
		if (ipnat_detailed_debug)
		    buginf("\nNAT: map match %s", mapping->mapname);
		return(mapping);
	    }
	    break;
	}
    }

    return(NULL);
}

/*
 * ipnat_establish_alias
 *
 * Set up an alias for the newly-allocated address if necessary.
 * If send_arp is TRUE, we will ask the alias insertion code to
 * check for another host on the same wire with the IP address
 * before inserting it in the alias database.  Most of the time
 * we can't do this, since we need to set up the aliases on the fly
 * and trust the user to not configure things badly, but we can do
 * the check when static translations are being configured.
 */

boolean
ipnat_establish_alias (ipaddrtype new_address, boolean send_arp)
{
    /*
     * If the system isn't fully initialized, interfaces won't have
     * their addresses yet, and we can't determine if we need to
     * install an alias yet.  Send a message to the ager process to
     * try installing the alias after initialization has completed.
     */
    if (!systeminit_flag) {
	process_send_message(ipnat_ager_pid, IPNAT_MSG_INSTALL_ALIAS, NULL,
			     new_address);
	/* Assume it worked. */
	return(TRUE);
    }

    if (samecable(new_address)) {
	/*
	 * Address is on directly attached net -- install an alias.
	 */
	if (ipnat_detailed_debug)
	    buginf("\nNAT: installing alias for address %i", new_address);
	if (!reg_invoke_ipalias_insert_dynamic(new_address, NULL, send_arp)) {
	    if (ipnat_detailed_debug)
		buginf("\nNAT: alias insert failed for %i", new_address);
	    return(FALSE);
	}
    }

    /*
     * Not needed, or it worked.
     */
    return(TRUE);
}

/*
 * ipnat_allocate_address
 */

static ipaddrtype
ipnat_allocate_address (paktype *pak, ipaddrtype address,
			ipnat_mapping **mapping_return, boolean steal_ok,
			uchar proto, ushort port, boolean *drop,
			enum ipnat_lookup lookup, enum ipnat_domain domain)
{
    ipnat_mapping *mapping;
    ipaddrtype new_address;
    enum ipnat_addrpool_type pool_type;

    mapping = ipnat_find_mapping(pak, address, lookup, domain);
    if (mapping == NULL) {
	/* This is actually a soft failure, the packet should not be dropped */
	*drop = FALSE;
	return(IPADDR_ZERO);
    }

    *drop = TRUE;

    /*
     * Get the address from the interface if that's what they want...
     */
    if (mapping->source_type == IPNAT_SOURCE_INTERFACE) {
	if (mapping->idb && mapping->idb->ip_address &&
	    interface_up(mapping->idb)) {
	    if (steal_ok) {
		*mapping_return = mapping;
		return(mapping->idb->ip_address);
	    } else {
		return(IPADDR_ZERO);
	    }
	}
    }

    /*
     * Address must be coming from a pool, but if the pool name
     * is gone for some reason, we're stuck.
     */
    if (mapping->poolname == NULL)
	return(IPADDR_ZERO);

    /*
     * Try to get the same global address as before for this
     * host if matching via route-maps.
     */
    if (mapping->match_type == IPNAT_MATCH_ROUTE_MAP) {
	ipnat_entry *nat;

	nat = ipnat_find_map_entry(address, mapping, IPNAT_LOCAL);
	if (nat) {
	    ipnat_addrpool_bump(mapping->poolname,
				nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	    *mapping_return = mapping;
	    return(nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	}
	/* No luck, allocate as usual. */
    }

    new_address = IPADDR_ZERO;

    /*
     * Either steal the address (if allowed) or allocate a new one.
     */
    if (steal_ok && (mapping->flags & IPNMF_OVERLOAD)) {
	new_address = ipnat_steal_address(proto, mapping, port,
					  lookup == IPNAT_LOCAL ?
					  IPNAT_GLOBAL : IPNAT_LOCAL);
	if (new_address == IPADDR_ZERO && ipnat_detailed_debug)
	    buginf("\nNAT: address not stolen for %i, proto %d port %d",
		   address, proto, port);
    }

    if (new_address == IPADDR_ZERO) {
	new_address = ipnat_addrpool_alloc(mapping->poolname, address);
	if (new_address == IPADDR_ZERO) {
	    if (ipnat_detailed_debug)
		buginf("\nNAT: failed to allocate address for %i, list/map %s",
		       address, mapping->acl ? mapping->acl->name :
		       mapping->mapname);
	    return(IPADDR_ZERO);
	}

	pool_type = ipnat_addrpool_pooltype(mapping->poolname);
	if (pool_type != IPNAT_POOL_ROTARY) {
	    /*
	     * If this address is from a non-rotary pool (and so is
	     * not the address of a real host), set up an alias for
	     * this address if it's on a connected subnet so that the
	     * router answers ARPs for this address.
	     */
	    (void) ipnat_establish_alias(new_address, FALSE);
	}
    }

    *mapping_return = mapping;
    return(new_address);
}

/*
 * ipnat_free_address
 */

static void
ipnat_free_address (ipnat_mapping *mapping, ipaddrtype address)
{
    if (mapping->poolname)
	ipnat_addrpool_free(mapping->poolname, address);
}

/*
 * ipnat_clone_entry
 *
 * Create a new extended entry for the local/global translation given.
 */

ipnat_entry *
ipnat_clone_entry (ipnat_entry *nat, ipnat_entry *template,
		   ipnat_mapping *mapping, ushort inside_localport,
		   uchar proto, ipaddrtype outside_localaddr,
		   ushort outsideport)
{
    uint inside_globalport;
    ipaddrtype outside_globaladdr;
    ipnat_entry *nat2;
    uchar flags;

    if (ipnat_detailed_debug)
	buginf("\nNAT: clone: allocating new port for %d", inside_localport);
    inside_globalport = ipnat_allocate_port(mapping, proto,
				    nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				    nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				    inside_localport, IPNAT_GLOBAL);
    if (ipnat_detailed_debug)
	buginf("\nNAT: clone: got port %d", inside_globalport);
    if (inside_globalport == IPNAT_NO_PORT) {
	/* Something is seriously wrong */
	if (ipnat_debug)
	    buginf("\nNAT: no global port for %i",
		   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	return(NULL);
    }

    flags = IPNF_EXTENDED;

    if (template) {
	outside_globaladdr = template->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	flags |= IPNF_OUTSIDE;
	if (ipnat_debug)
	    buginf("\nNAT: setting up outside mapping %i->%i",
		   outside_localaddr, outside_globaladdr);
    } else {
	outside_globaladdr = outside_localaddr;
    }

    if (ipnat_detailed_debug)
	buginf("\nNAT: clone: creating new entry %i %i %d %d, %d, %i, %i, %d",
	       nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
	       nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
	       inside_localport, inside_globalport, proto, outside_localaddr,
	       outside_globaladdr, outsideport);
    nat2 = ipnat_create_entry(nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
			      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			      inside_localport, inside_globalport,
			      outside_localaddr, outside_globaladdr,
			      outsideport, outsideport, proto, flags);
    if (nat2 == NULL) {
	/* We lose */
	if (ipnat_debug)
	    buginf("\nNAT: no new entry for %i %i",
		   nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
		   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	ipnat_free_port(proto, nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			inside_globalport, IPNAT_GLOBAL);
	return(NULL);
    }
    ipnat_addrpool_bump(mapping->poolname,
			nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
    mapping->refcount++;
    nat2->mapping[IPNAT_INSIDE] = mapping;

    if (template) {
	mapping = template->mapping[IPNAT_OUTSIDE];
	nat2->mapping[IPNAT_OUTSIDE] = mapping;
	if (mapping) {
	    mapping->refcount++;
	    ipnat_addrpool_bump(mapping->poolname,
				nat2->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	}
    }
    return(nat2);
}

/*
 * address/port database
 */

STRUCTURE_REF(port_range);
STRUCTURE_REF(ipnat_portlist);

struct port_range_ {
    ushort     low_port, high_port;
    uint       localport;
    ipaddrtype localaddr;
    uint       refcount;
    port_range *next;
};

struct ipnat_portlist_ {
    uchar proto;
    ipaddrtype globaladdr;
    port_range *low_freelist;		/* free list */
    port_range *priv_freelist;		/* free list */
    port_range *user_freelist;		/* free list */
    port_range *high_freelist;		/* free list */
    ipnat_mapping *mapping;

    list_element element;		/* linkage */
};

static inline boolean
ipnat_port_range_is_free(port_range *range)
{
    return (range->localport == IPNAT_NO_PORT);
}

static port_range **
ipnat_freelistp (ipnat_portlist *portlist, ushort port)
{
    if (port < IPPORT_RESERVED/2)
	return(&portlist->low_freelist);
    if (port < IPPORT_RESERVED)
	return(&portlist->priv_freelist);
    if (port < IPPORT_USER_RESERVED)
	return(&portlist->user_freelist);
    return(&portlist->high_freelist);
}

void
ipnat_portlist_init (void)
{
    enum ipnat_lookup lookup;

    for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	list_create(&ipnat_portlist_hdrs[lookup], 0, "IP NAT Portlists", 0);
    }
}

static boolean
ipnat_portlist_empty (ipnat_portlist *portlist)
{
    if (portlist->low_freelist->next || portlist->priv_freelist->next ||
	portlist->user_freelist->next || portlist->high_freelist->next)
	return(FALSE);
    return(TRUE);
}

static boolean
ipnat_suballocate_port (port_range **freelistp, port_range *range,
			port_range *prev, ushort newport, 
			ushort localport, ipaddrtype localaddr)
{
    port_range *allocrange, *newrange;

    if (range->low_port == range->high_port) {
	/* Simple case, just mark this block as allocated */
	range->localport = localport;
	range->localaddr = localaddr;
	range->refcount++;
    } else {
	/* Create a block for the allocated port. */
	allocrange = chunk_malloc(port_range_chunks);
	if (allocrange == NULL)
	    return(FALSE);	/* Can't allocate. */
	allocrange->low_port = allocrange->high_port = newport;
	allocrange->localport = localport;
	allocrange->localaddr = localaddr;
	allocrange->refcount++;

	/* Add the new port range to the free list */
	if (newport == range->low_port) {
	    /* Link it before this one. */
	    range->low_port++;
	    if (prev)
		prev->next = allocrange;
	    else
		*freelistp = allocrange;
	    allocrange->next = range;
	} else if (newport == range->high_port) {
	    /* Link it after this one. */
	    range->high_port--;
	    allocrange->next = range->next;
	    range->next = allocrange;
	} else {
	    /* Urgh, must split the existing range around it. */
	    newrange = chunk_malloc(port_range_chunks);
	    if (newrange == NULL) {
		chunk_free(port_range_chunks, allocrange);
		return(FALSE);	/* Can't allocate. */
	    }
	    newrange->low_port = range->low_port;
	    newrange->high_port = allocrange->low_port - 1;
	    range->low_port = allocrange->high_port + 1;
	    allocrange->next = range;
	    newrange->next = allocrange;
	    if (prev)
		prev->next = newrange;
	    else
		*freelistp = newrange;
	}
    }
    return(TRUE);
}

static port_range *
ipnat_create_port_range (ushort low_port, ushort high_port)
{
    port_range *range;

    /* Create the low chunk */
    range = chunk_malloc(port_range_chunks);
    if (range == NULL) {
	return(NULL);
    }
    range->low_port = low_port;
    range->high_port = high_port;
    range->localport = IPNAT_NO_PORT;
    return(range);
}

static ipnat_portlist *
ipnat_find_portlist (uchar proto, ipaddrtype globaladdr,
		     enum ipnat_lookup lookup)
{
    list_element *el;
    ipnat_portlist *portlist;
    list_header *portlist_hdr;

    portlist_hdr = &ipnat_portlist_hdrs[lookup];

    FOR_ALL_DATA_IN_LIST(portlist_hdr, el, portlist) {
	if (portlist->proto == proto && portlist->globaladdr == globaladdr)
	    return(portlist);
    }
    return(NULL);
}

static void
ipnat_delete_portlist (ipnat_portlist *portlist)
{
    /*
     * Remove it from its list if it's there.
     */
    if (LIST_ELEMENT_QUEUED(&portlist->element))
	list_remove(ELEMENT_GET_LIST(&portlist->element), &portlist->element,
		    portlist);

    /*
     * Blow some chunks.
     */
    if (portlist->low_freelist)
	chunk_free(port_range_chunks, portlist->low_freelist);
    if (portlist->priv_freelist)
	chunk_free(port_range_chunks, portlist->priv_freelist);
    if (portlist->user_freelist)
	chunk_free(port_range_chunks, portlist->user_freelist);
    if (portlist->high_freelist)
	chunk_free(port_range_chunks, portlist->high_freelist);

    /*
     * Free the rest.
     */
    free(portlist);
}

static ipnat_portlist *
ipnat_create_portlist (ipnat_mapping *mapping, uchar proto,
		       ipaddrtype globaladdr, enum ipnat_lookup lookup)
{
    ipnat_portlist *portlist;
    port_range *range;

    portlist = ipnat_find_portlist(proto, globaladdr, lookup);
    if (portlist) {
	if (ipnat_debug)
	    buginf("\nNAT: portlist %d %i already exists!", proto,
		   globaladdr);
	return(NULL);
    }

    portlist = malloc(sizeof(ipnat_portlist));
    if (portlist == NULL) {
	return(NULL);
    }
    portlist->globaladdr = globaladdr;
    portlist->proto = proto;
    portlist->mapping = mapping;

    /* */
    range = ipnat_create_port_range((proto == ICMP_PROT) ? 0 : 1, 
				    (IPPORT_RESERVED / 2) - 1);
    if (range == NULL) {
	ipnat_delete_portlist(portlist);
	return(NULL);
    }
    portlist->low_freelist = range;

    /* */
    range = ipnat_create_port_range(IPPORT_RESERVED / 2, IPPORT_RESERVED - 1);
    if (range == NULL) {
	ipnat_delete_portlist(portlist);
	return(NULL);
    }
    portlist->priv_freelist = range;

    /* */
    range = ipnat_create_port_range(IPPORT_RESERVED, IPPORT_USER_RESERVED - 1);
    if (range == NULL) {
	ipnat_delete_portlist(portlist);
	return(NULL);
    }
    portlist->user_freelist = range;

    /* */
    range = ipnat_create_port_range(IPPORT_USER_RESERVED, 65535);
    if (range == NULL) {
	ipnat_delete_portlist(portlist);
	return(NULL);
    }
    portlist->high_freelist = range;

    list_enqueue(&ipnat_portlist_hdrs[lookup], &portlist->element, portlist);

    return(portlist);
}

uint
ipnat_allocate_port (ipnat_mapping *mapping, uchar proto,
		     ipaddrtype localaddr, ipaddrtype globaladdr,
		     ushort port, enum ipnat_lookup lookup)
{
    ipnat_portlist *portlist;
    port_range **freelistp;
    port_range *range, *last;
    port_range *free_range, *free_last;
    boolean assigned = FALSE;
    ushort newport;

    /*
     * Try to find the list for this protocol and global address.
     * If not found, create it now.
     */
    portlist = ipnat_find_portlist(proto, globaladdr, lookup);
    if (portlist == NULL) {
	portlist = ipnat_create_portlist(mapping, proto, globaladdr, lookup);
	if (portlist == NULL)
	    return(IPNAT_NO_PORT);
    }

    /*
     * If the portlist was created with a NULL mapping (a static-with-port
     * translation entry being extended, for example), fill in the mapping
     * now.
     */
    if (portlist->mapping == NULL && mapping)
	portlist->mapping = mapping;

    freelistp = ipnat_freelistp(portlist, port);

    /*
    ** See if the one they want is free. The algorithm works as follows:
    ** Try to assign the same port as passed in. If that value is in use,
    ** look for free ports less than the caller's port. Still no ports ?
    ** Find free ports greater than the caller's port.
    */

    free_range = free_last = last = NULL;
    for (range = *freelistp; range; last = range, range = range->next) {

        /* 
        ** Break from the loop if this range is free and our port is part 
        ** of the range or our port has been assigned to some other host.
        */

        if (ipnat_port_range_is_free(range)) {
            free_range = range;
            free_last  = last;
            if (assigned || 
               (port >= range->low_port && port <= range->high_port)) {
                break;
            }
        } else {

            /* 
            ** In a typical client-server model, the server socket remains 
            ** constant, while the client socket fluctuates. NAT may have 
            ** created a mapping for this address and port earlier when 
            ** processing a different connection. If a mapping exists, 
            ** increment the reference count.
            */

            if (range->localport == port && range->localaddr == localaddr) {
                range->refcount ++;
                return(range->low_port);
            }

            /* Has our port been assigned to some other host ? */
            if (range->low_port == port) {
                assigned = TRUE;
                if (free_range != NULL)
                    break;
            }
        } 
    }


    /* Allocate a new port */
    if (free_range != NULL) {
        newport = assigned ? free_range->low_port : port; 
        if (ipnat_suballocate_port(freelistp, free_range, free_last, 
                                   newport, port, localaddr)) {

            if (ipnat_detailed_debug) {
                buginf("\nNAT: ipnat_allocate_port: wanted %d got %d", 
                       port, newport );
            }
            return(newport);
        }
    }

    /* All ports have been allocated */
    return(IPNAT_NO_PORT);
}

/*
 * ipnat_reap_portlists
 *
 * Find all the portlists that reference a certain mapping
 * and delete them.
 */

void
ipnat_reap_portlists (ipnat_mapping *mapping)
{
    enum ipnat_lookup lookup;
    list_element *el, *next_el;
    ipnat_portlist *portlist;
    list_header *portlist_hdr;

    for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	portlist_hdr = &ipnat_portlist_hdrs[lookup];
	FOR_ALL_DATA_IN_LIST_W_NEXT(portlist_hdr, el, next_el, portlist) {
	    if (portlist->mapping == mapping) {
		if (ipnat_detailed_debug)
		    buginf("\nNAT: reaping portlist proto %d globaladdr %i",
			   portlist->proto, portlist->globaladdr);
		ipnat_delete_portlist(portlist);
	    }
	}
    }
}

/*
 * ipnat_steal_address
 *
 * Find the first portlist from the same mapping that has free ports,
 * and return the global address for that portlist.  The port will
 * have to be allocated later.
 */

static ipaddrtype
ipnat_steal_address (uchar proto, ipnat_mapping *mapping,
		     ushort inside_localport, enum ipnat_lookup lookup)
{
    list_header *portlist_hdr;
    list_element *el;
    ipnat_portlist *portlist;
    port_range **freelistp;

    portlist_hdr = &ipnat_portlist_hdrs[lookup];

    /*
     * Go through the local or global address/port database
     * to find the first one from the same mapping with free ports.
     */
    FOR_ALL_DATA_IN_LIST(portlist_hdr, el, portlist) {
	if (portlist->mapping != mapping)
	    continue;
	freelistp = ipnat_freelistp(portlist, inside_localport);
	if (*freelistp) {
	    if (!ipnat_addrpool_bump(mapping->poolname, portlist->globaladdr))
		continue;
	    return(portlist->globaladdr);
	}
    }

    return(IPADDR_ZERO);
}

/*
 * ipnat_free_port
 *
 * Free a port belonging to a particular global address and protocol.
 */

void
ipnat_free_port (uchar proto, ipaddrtype globaladdr, ushort port,
		 enum ipnat_lookup lookup) {

    ipnat_portlist *portlist;
    port_range **freelistp;
    port_range *range, *last, *next;

    /*
     * Find portlist for this address and protocol.
     */
    portlist = ipnat_find_portlist(proto, globaladdr, lookup);
    if (portlist == NULL) {
	if (ipnat_debug)
	    buginf("\nNAT: no portlist for proto %d globaladdr %i port %d",
		   proto, globaladdr, port);
	return;
    }

    freelistp = ipnat_freelistp(portlist, port);

    last = NULL;
    for (range = *freelistp; range; range = range->next) {
	if (range->low_port == port) {
	    if (ipnat_port_range_is_free(range)) 
		return;
	    range->refcount--;
	    if (range->refcount == 0) {
		range->localport = IPNAT_NO_PORT;
		/* Coalesce with previous range. */
		if (last && ipnat_port_range_is_free(last)) {
		    last->next = range->next;
		    last->high_port = port;
		    chunk_free(port_range_chunks, range);
		    range = last;
		}
		/* Coalesce with next range. */
		next = range->next;
		if (next && ipnat_port_range_is_free(next)) {
		    range->high_port = next->high_port;
		    range->next = next->next;
		    chunk_free(port_range_chunks, next);
		}
	    }
	    break;
	}
	last = range;
    }

    if (ipnat_portlist_empty(portlist))
	ipnat_delete_portlist(portlist);
}

/*
 * Various convenience functions
 */


/*
 * ipnat_new_inside_entry
 */

ipnat_entry *
ipnat_new_inside_entry (ipnat_entry *template, paktype *pak,
			ipaddrtype inside_localaddr,
			ipaddrtype outside_localaddr, ushort sport,
			ushort dport, uchar proto, boolean can_overload,
			boolean *drop)
{
    ipnat_mapping *mapping;
    ipaddrtype inside_globaladdr, outside_globaladdr;
    uint inside_globalport; 
    ushort outside_localport, outside_globalport;
    ipnat_entry *nat;
    uchar flags;

    /* Create an entry from scratch. */

    /*
     * Get the address from a simple entry if there's one there.
     */
    nat = ipnat_find_entry(inside_localaddr, IPADDR_ZERO, 0, 0, 0,
			   IPNAT_LOCAL);
    if (nat) {
	/* Get the global address from the simple entry. */
	inside_globaladdr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	mapping = NULL;
    } else {
	/* Allocate the global address. */
	inside_globaladdr = ipnat_allocate_address(pak, inside_localaddr,
						   &mapping, can_overload,
						   proto, sport, drop,
						   IPNAT_LOCAL, IPNAT_INSIDE);
	if (inside_globaladdr == IPADDR_ZERO) {
	    /* Address not allocated, probably because there is no mapping. */
	    return(NULL);
	}
    }

    /*
     * Allocate a port if necessary.
     */
    if (can_overload && mapping && (mapping->flags & IPNMF_OVERLOAD)) {
	inside_globalport = ipnat_allocate_port(mapping, proto,
						inside_localaddr,
						inside_globaladdr, sport,
						IPNAT_GLOBAL);
	if (inside_globalport == IPNAT_NO_PORT) {
	    if (ipnat_detailed_debug)
		buginf("\nNAT: ipnat_new_inside_entry: couldn't allocate port"
		       " for %i", inside_globaladdr);
	    ipnat_free_address(mapping, inside_globaladdr);
	    return(NULL);
	}
	flags = IPNF_EXTENDED;
    } else {
	inside_globalport = sport;
	flags = 0;
    }

    /*
     * Translations created via route-maps need to be extended entries
     * since the local address is no longer the sole selection criterion.
     * A local address may have more than one global address, usually
     * (always?) one per route-map.
     */
    if (mapping && mapping->match_type == IPNAT_MATCH_ROUTE_MAP)
	flags |= IPNF_EXTENDED;

    if (template) {
	outside_globaladdr = template->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	/* No outside port translation yet (if ever). */
	outside_localport = outside_globalport = dport;
	flags |= IPNF_OUTSIDE;
	if (ipnat_detailed_debug)
	    buginf("\nNAT: setting up outside mapping %i->%i",
		   outside_localaddr, outside_globaladdr);
    } else {
	if (!(flags & IPNF_EXTENDED))
	    outside_localaddr = IPADDR_ZERO;
	outside_globaladdr = (flags & IPNF_EXTENDED) ? outside_localaddr : 0;
	outside_localport = outside_globalport = (flags & IPNF_EXTENDED) ?
	    dport : 0;
    }

    /*
     * For ICMP "flows", the port numbers are really the single
     * identification field in the ICMP header.  If the inside global
     * value changes, the outside value must change, too.
     */
    if ((flags & IPNF_EXTENDED) && proto == ICMP_PROT &&
	inside_globalport != sport)
	outside_globalport = inside_globalport;

    /*
     * Create an entry with the information.
     */
    nat = ipnat_create_entry(inside_localaddr, inside_globaladdr, sport,
			     inside_globalport, outside_localaddr,
			     outside_globaladdr, outside_localport,
			     outside_globalport, proto, flags);
    if (nat == NULL) {
	/* We lose. */
	if (flags & IPNF_EXTENDED)
	    ipnat_free_port(proto, inside_globaladdr, inside_globalport,
			    IPNAT_GLOBAL);
	if (mapping)
	    ipnat_free_address(mapping, inside_globaladdr);
	return(NULL);
    }
    if (mapping) {
	mapping->refcount++;
	nat->mapping[IPNAT_INSIDE] = mapping;
    }
    if (template) {
	mapping = template->mapping[IPNAT_OUTSIDE];
	if (mapping) {
	    mapping->refcount++;
	    nat->mapping[IPNAT_OUTSIDE] = mapping;
	    ipnat_addrpool_bump(mapping->poolname,
				nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	}
    }

    /*
     * If this entry was created via a route-map match, then the entry
     * also has to be placed on the auxiliary hash table so that
     * another match from the same host will get the same global
     * address.
     */
    if (mapping && mapping->match_type == IPNAT_MATCH_ROUTE_MAP)
	ipnat_add_map_hash(nat);

    return(nat);
}

ipnat_entry *
ipnat_new_inside_dest_entry (ipnat_entry *template, paktype *pak,
			     ipaddrtype inside_globaladdr,
			     ipaddrtype outside_globaladdr,
			     ushort sport, ushort dport, uchar proto,
			     boolean *drop)
{
    ipnat_mapping *mapping;
    ipaddrtype inside_localaddr, outside_localaddr;
    ipnat_entry *nat;
    uchar flags;

    /* Create an entry from scratch. */
    inside_localaddr = ipnat_allocate_address(pak, inside_globaladdr, &mapping,
					      FALSE, proto, dport, drop,
					      IPNAT_GLOBAL, IPNAT_INSIDE);
    if (inside_localaddr == IPADDR_ZERO) {
	/* We lose */
	return(NULL);
    }

    flags = IPNF_EXTENDED | IPNF_DEST;
    if (template) {
	outside_localaddr = template->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	flags |= IPNF_OUTSIDE;
	if (ipnat_debug)
	    buginf("\nNAT: setting up outside mapping %i->%i",
		   outside_globaladdr, outside_localaddr);
    } else {
	outside_localaddr = outside_globaladdr;
    }

    nat = ipnat_create_entry(inside_localaddr, inside_globaladdr, dport, dport,
			     outside_localaddr, outside_globaladdr, sport,
			     sport, proto, flags);
    if (nat == NULL) {
	/* We lose. */
	ipnat_free_address(mapping, inside_localaddr);
	return(NULL);
    }
    mapping->refcount++;
    nat->mapping[IPNAT_INSIDE] = mapping;
    if (template) {
	mapping = template->mapping[IPNAT_OUTSIDE];
	nat->mapping[IPNAT_OUTSIDE] = mapping;
	if (mapping) {
	    mapping->refcount++;
	    ipnat_addrpool_bump(mapping->poolname,
				nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	}
    }
    return(nat);
}

ipnat_entry *
ipnat_new_outside_entry (paktype *pak, ipaddrtype outside_globaladdr,
			 boolean *drop)
{
    ipaddrtype outside_localaddr;
    ipnat_mapping *mapping;
    ipnat_entry *nat;

    outside_localaddr = ipnat_allocate_address(pak, outside_globaladdr,
					       &mapping, FALSE, 0, 0, drop,
					       IPNAT_GLOBAL, IPNAT_OUTSIDE);
    if (outside_localaddr == IPADDR_ZERO) {
	/* We lose */
	return(NULL);
    }

    nat = ipnat_create_entry(IPADDR_ZERO, IPADDR_ZERO, 0, 0, outside_localaddr,
			     outside_globaladdr, 0, 0, 0, IPNF_OUTSIDE);
    if (nat == NULL) {
	/* We lose. */
	ipnat_free_address(mapping, outside_localaddr);
	return(NULL);
    }
    mapping->refcount++;
    nat->mapping[IPNAT_OUTSIDE] = mapping;

    return(nat);
}

/*
 * Editing contexts
 */


/*
 * ipnat_create_edit_context
 *
 * Create an edit_context
 */

ipnat_edit_context *
ipnat_create_edit_context (iphdrtype *ip, ipnat_entry *nat,
			   enum local_type source_type,
			   enum ipnat_lookup lookup)
{
    ipnat_edit_context *conv;

    conv = malloc(sizeof(ipnat_edit_context));
    if (conv == NULL)
	return(NULL);

    conv->token_end = conv->token_string;
    conv->nat = nat;
    conv->source_type = source_type;

    if (ipnat_detailed_debug)
	buginf("\nNAT: created edit_context (%i,%d) -> (%i,%d)",
	       nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
	       nat->port[IPNAT_INSIDE][IPNAT_LOCAL],
	       nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL],
	       nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL]);
    return(conv);
}

/*
 * ipnat_delete_edit_context
 *
 * Delete an edit_context.
 */

void
ipnat_delete_edit_context (ipnat_edit_context *context)
{
    free(context);
}

/*
 * Doors -- how to deal with TFTP and rsh and other servers that
 * connect back to us from a random port number.  We set aside a
 * description of what we know about the reconnecting stream, and when
 * it comes in, create a translation for it to let it through.  This
 * is only needed when doing overloading with the mapping, since the
 * translations are on a flow basis.
 */

/*
 * ipnat_create_door
 *
 * Allocate a door.
 */

ipnat_door *
ipnat_create_door (void)
{
    return(malloc(sizeof(ipnat_door)));
}

/*
 * ipnat_add_door
 *
 * Add a door to the master list.
 */

void
ipnat_add_door (ipnat_door *new)
{
    list_enqueue(&ipnat_door_list, &new->element, new);
}

/*
 * ipnat_remove_door
 *
 * Remove the door from the master list.
 */

void
ipnat_remove_door (ipnat_door *door)
{
    list_remove(&ipnat_door_list, &door->element, NULL);
}

/*
 * ipnat_destroy_door
 *
 * Deallocate a door.
 */

void
ipnat_destroy_door (ipnat_door *door)
{
    mgd_timer_stop(&door->timeout_timer);
    free(door);
}

/*
 * ipnat_setup_door
 *
 * Create and initialize a door with its zillion parameters.
 */

boolean
ipnat_setup_door (ipaddrtype outsidehost, ipaddrtype insidehost,
		  ushort serverport, ushort outsideport,
		  ushort inside_localport, ushort inside_globalport,
		  ushort clientport, int proto, int trigger_proto, int flags)
{
    ipnat_door *door;
    char *protstring;

    /*
     * Allocate the door structure.
     */
    door = ipnat_create_door();
    if (door == NULL)
	return(FALSE);

    /*
     * Set up the fields.
     */
    door->outsidehost = outsidehost;
    door->insidehost = insidehost;
    door->serverport = serverport;
    door->outsideport = outsideport;
    door->insideport[IPNAT_LOCAL] = inside_localport;
    door->insideport[IPNAT_GLOBAL] = inside_globalport;
    door->clientport = clientport;
    door->proto = proto;
    door->trigger_proto = trigger_proto;
    door->flags = flags;

    /*
     * Let me know what's going on.
     */
    if (ipnat_detailed_debug) {
	protstring = lookup_id_string(proto_items, door->proto);
	buginf("\nNAT: create door %s (%i/*, %d/*) -> (%i, %d)", protstring,
	       door->outsidehost, door->serverport, door->insidehost,
	       door->insideport[IPNAT_GLOBAL]);
    }

    /*
     * Initialize the timer and start it.
     */
    mgd_timer_init_leaf(&door->timeout_timer, &ipnat_door_timer_master, 0,
			door, FALSE);
    mgd_timer_start(&door->timeout_timer, IPNAT_DOGGY_DOOR_TIMEOUT);

    /*
     * Add the door to the master list.
     */
    ipnat_add_door(door);

    /*
     * All done.
     */
    return(TRUE);
}

/*
 * ipnat_find_door
 *
 * Find a door on the master list.
 */

ipnat_door *
ipnat_find_door (uchar proto, ipaddrtype outsidehost, ipaddrtype insidehost,
		 ushort outsideport, ushort insideport,
		 enum ipnat_lookup lookup)
{
    list_element *el;
    ipnat_door *door;

    FOR_ALL_DATA_IN_LIST(&ipnat_door_list, el, door) {
	if (proto == door->proto && insidehost == door->insidehost &&
	    insideport == door->insideport[lookup]) {
	    /*
	     * Match the outside port if necessary.
	     */
	    if ((door->flags & IPNDF_KNOWN_PORT) &&
		outsideport != door->outsideport)
		continue;

	    /*
	     * Some protocols allow multi-homed hosts to answer
	     * back from another address.  Broken, but we have
	     * to deal with it.
	     */
	    if ((door->flags & IPNDF_OUTSIDEHOST_ANY) ||
		outsidehost == door->outsidehost)
		return(door);
	}
    }

    return(NULL);
}

/*
 * ipnat_open_door
 *
 * Find the door for the packet and create an entry.
 * This routine only deals with the packet coming from the outside.
 */

ipnat_entry *
ipnat_open_door (uchar proto, ipaddrtype srcadr, ipaddrtype dstadr,
		 ushort sport, ushort dport, paktype *pak)
{
    ipnat_door *door;
    ipnat_mapping *mappings[IPNAT_DOMAINS];
    ipnat_entry *nat;
    ipaddrtype outside_localaddr;
    boolean drop;
    char *protstring;

    door = ipnat_find_door(proto, srcadr, dstadr, sport, dport, IPNAT_GLOBAL);
    if (door == NULL)
	return(NULL);

    /*
     * Find the flow that created the doggy door.
     */
    nat = ipnat_find_entry(dstadr, door->outsidehost, door->clientport,
			   door->serverport, door->trigger_proto,
			   IPNAT_GLOBAL);
    if (nat == NULL) {
	if (ipnat_debug) {
	    protstring = lookup_id_string(proto_items, door->proto);
	    buginf("\nNAT: translation not found for door %s (%i/*, %d/*) ->"
		   " (%i, %d)",
		   protstring ? protstring : "?", door->outsidehost,
		   door->serverport, door->insidehost, door->clientport);
	}
	return(NULL);
    }

    mappings[IPNAT_INSIDE] = nat->mapping[IPNAT_INSIDE];
    mappings[IPNAT_OUTSIDE] = nat->mapping[IPNAT_OUTSIDE];
    /*
     * Create the translation for the new stream.
     * May need to get a new outside local srcadr and port.
     */
    outside_localaddr = ipnat_allocate_address(pak, srcadr,
					       &mappings[IPNAT_OUTSIDE], FALSE,
					       0, 0, &drop, IPNAT_GLOBAL,
					       IPNAT_OUTSIDE);
    if (outside_localaddr == IPADDR_ZERO)
	outside_localaddr = srcadr;
    nat = ipnat_create_entry(nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
			     nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			     door->insideport[IPNAT_LOCAL],
			     door->insideport[IPNAT_GLOBAL],
			     outside_localaddr, srcadr, sport, sport, proto,
			     nat->flags);
    if (nat == NULL) {
	return(NULL);
    }

    if (mappings[IPNAT_INSIDE])
	mappings[IPNAT_INSIDE]->refcount++;
    nat->mapping[IPNAT_INSIDE] = mappings[IPNAT_INSIDE];
    ipnat_addrpool_bump(mappings[IPNAT_INSIDE]->poolname,
			nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
    if (mappings[IPNAT_OUTSIDE])
	mappings[IPNAT_OUTSIDE]->refcount++;
    nat->mapping[IPNAT_OUTSIDE] = mappings[IPNAT_OUTSIDE];

    ipnat_remove_door(door);
    ipnat_destroy_door(door);

    return(nat);
}

/*
 * Fragments
 *
 * Overloading creates a problem when the router encounters fragments.
 * The code needs to do a flow lookup to support overloading, but
 * the full flow information is only available in the first fragment,
 * so fragments other than the first cannot be translated unless we
 * save the layer 4 flow information (source and destination ports)
 * for fragment zero, and use it for the lookup when non-initial fragments
 * come through.
 * 
 * -- The fragments should probably be a hash table, not a linked list.
 * -- The fragments should maybe be chunks (but don't want to
 *    preallocate a chunk array since most folks don't have an
 *    issue with fragments).
 */

/*
 * ipnat_create_fragment
 *
 * Allocate a fragment.
 */

ipnat_fragment *
ipnat_create_fragment (ipaddrtype srcadr, ipaddrtype dstadr, ushort id,
		       uchar prot, ushort sport, ushort dport)
{
    ipnat_fragment *fragment;

    /*
     * Allocate the fragment.
     */
    fragment = malloc(sizeof(ipnat_fragment));
    if (fragment == NULL)
	return(NULL);

    /*
     * Initialize it.
     */
    fragment->srcadr = srcadr;
    fragment->dstadr = dstadr;
    fragment->id = id;
    fragment->prot = prot;
    fragment->sport = sport;
    fragment->dport = dport;

    mgd_timer_init_leaf(&fragment->timeout_timer, &ipnat_fragment_timer_master,
			0, fragment, FALSE);
    mgd_timer_start(&fragment->timeout_timer, IPNAT_FRAGMENT_TIMEOUT);

    /*
     * Return it.
     */
    return(fragment);
}

/*
 * ipnat_add_fragment
 *
 * Add a fragment to the master list.
 */

void
ipnat_add_fragment (ipnat_fragment *new)
{
    /*
     * Add at the head, so old timing-out fragments accumulate
     * at the end.
     */
    list_requeue(&ipnat_fragment_list, &new->element, new);
}

/*
 * ipnat_remove_fragment
 *
 * Remove the fragment from the master list.
 */

void
ipnat_remove_fragment (ipnat_fragment *fragment)
{
    list_remove(&ipnat_fragment_list, &fragment->element, NULL);
}

/*
 * ipnat_destroy_fragment
 *
 * Deallocate a fragment.
 */

void
ipnat_destroy_fragment (ipnat_fragment *fragment)
{
    mgd_timer_stop(&fragment->timeout_timer);
    free(fragment);
}

/*
 * ipnat_find_fragment
 *
 * Find a fragment.
 */

ipnat_fragment *
ipnat_find_fragment (ipaddrtype srcadr, ipaddrtype dstadr, ushort id,
		     uchar prot)
{
    list_element *el;
    ipnat_fragment *fragment;

    FOR_ALL_DATA_IN_LIST(&ipnat_fragment_list, el, fragment) {
	if (srcadr == fragment->srcadr && dstadr == fragment->dstadr &&
	    id == fragment->id && prot == fragment->prot) {
	    return(fragment);
	}
    }

    return(NULL);
}

/*
 * ipnat_create_seq_delta_pair
 *
 * Create a sequence number/delta pair for remembering
 * when a delta to the sequence space starts.
 */

ipnat_seq_delta_pair *
ipnat_create_seq_delta_pair (ipnat_edit_context *context)
{
    ipnat_seq_delta_pair *pair;

    /*
     * Don't try to allocate if we've got too many.
     */
    if (context->sd_count == IPNAT_MAX_SEQ_DELTA_PAIRS)
	return(NULL);

    pair = malloc(sizeof(ipnat_seq_delta_pair));
    if (pair)
	context->sd_count++;
    return(pair);
}

/*
 * ipnat_destroy_seq_delta_pair
 *
 * Delete a sequence number/delta pair.
 */

void
ipnat_destroy_seq_delta_pair (ipnat_seq_delta_pair *pair,
			      ipnat_edit_context *context)
{
    free(pair);
    context->sd_count--;
}

/*
 * ipnat_remove_seq_delta_pair
 *
 * Remove the sequence number/delta pair from the context.
 */

void
ipnat_remove_seq_delta_pair (ipnat_seq_delta_pair *pair,
			     ipnat_edit_context *context)
{
    ipnat_seq_delta_pair *p, *last;

    last = NULL;
    for (p = context->sd_pairs; p; p = p->next) {
	if (p == pair) {
	    if (last)
		last->next = p->next;
	    else
		context->sd_pairs = p->next;
	    return;
	}
    }
}

/*
 * ipnat_destroy_all_seq_delta_pairs
 *
 * Unlink and delete all seq-delta pairs for a context.
 */

void
ipnat_destroy_all_seq_delta_pairs (ipnat_edit_context *context)
{
    ipnat_seq_delta_pair *p, *next;

    for (p = context->sd_pairs; p; p = next) {
	next = p->next;
	ipnat_destroy_seq_delta_pair(p, context);
    }
}

/*
 * ipnat_dump_all_seq_delta_pairs
 *
 * Print out seq-delta pairs for "show ip nat trans verbose"
 */

static void
ipnat_dump_all_seq_delta_pairs (ipnat_edit_context *context)
{
    ipnat_seq_delta_pair *p;

    for (p = context->sd_pairs; p; p = p->next)
	printf(" (%c %u, %d)", (p->domain == IPNAT_INSIDE) ? 'i' : 'o',
	       p->seq, p->delta);
}

/*
 * User interface
 */

static char *const ipnat_flagnames[] = {
    "static",			/* 0x0001 entry is static */
    "extended",			/* 0x0002 translation is extended */
    "dest",			/* 0x0004 created via dynamic dest trans */
    "outside",			/* 0x0008 outside */
    "timing-out",		/* 0x0010 timing out */
    "extendable",		/* 0x0020 entry is extendable (static) */
    "no-alias"			/* 0x0040 entry should not create alias (s) */
};
#define NIPNATFLAGNAMES (sizeof(ipnat_flagnames) / sizeof(ipnat_flagnames[0]))

/* Enough space for "255.1.255.255:6535": */
#define EIGHTEEN_SPACES		"                  "
/* Enough space for "255.255.255.255:65535": */
#define TWENTYONE_SPACES	"                     "

/*
< 90 chars width

Pro Inside global      Inside local       Outside local      Outside global
0         1         2         3         4         5         6         7
0123456789012345678901234567890123456789012345678901234567890123456789012345678
^   ^                  ^                  ^                  ^

>= 90 chars width

Pro Inside global         Inside local          Outside local         Outside global
    255.255.255.255:65535 255.255.255.255:65535 255.255.255.255:65535 255.255.255.255:65535
0         1         2         3         4         5         6         7
0123456789012345678901234567890123456789012345678901234567890123456789012345678
^   ^                     ^                     ^                     ^
*/

static void
ipnat_show_entry (ipnat_entry *nat, boolean verbose, boolean wide)
{
    int column;
    int entry_width;
    char *entry_pad;
    int tabs[4];

    if (wide) {
	entry_width = 21;
	entry_pad = TWENTYONE_SPACES;
    } else {
	entry_width = 18;
	entry_pad = EIGHTEEN_SPACES;
    }

    /* Set up the tab stops for the four addresses. */
    tabs[0] = 4;
    tabs[1] = tabs[0] + entry_width + 1;
    tabs[2] = tabs[1] + entry_width + 1;
    tabs[3] = tabs[2] + entry_width + 1;

    /* Protocol */
    printf("\n");
    column = 0;
    if (nat->flags & IPNF_EXTENDED)
	column += printf_id_string(proto_items, nat->proto);
    else
	column += printf("---");

    /* Inside global */
    if (column < tabs[0])
	column += printf(entry_pad + entry_width - (tabs[0] - column));
    else
	column += printf(" ");
    if (nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]) {
	column += printf("%i", nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	if (nat->flags & IPNF_EXTENDED)
	    column += printf(":%d", nat->port[IPNAT_INSIDE][IPNAT_GLOBAL]);
    } else {
	column += printf("---");
    }

    /* Inside local */
    if (column < tabs[1])
	column += printf(entry_pad + entry_width - (tabs[1] - column));
    else
	column += printf(" ");
    if (nat->address[IPNAT_INSIDE][IPNAT_LOCAL]) {
	column += printf("%i", nat->address[IPNAT_INSIDE][IPNAT_LOCAL]);
	if (nat->flags & IPNF_EXTENDED)
	    column += printf(":%d", nat->port[IPNAT_INSIDE][IPNAT_LOCAL]);
    } else {
	column += printf("---");
    }

    /* Outside local */
    if (column < tabs[2])
	column += printf(entry_pad + entry_width - (tabs[2] - column));
    else
	column += printf(" ");
    if (nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]) {
	column += printf("%i", nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	if (nat->flags & IPNF_EXTENDED)
	    column += printf(":%d", nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL]);
    } else {
	column += printf("---");
    }

    /* Outside global */
    if (column < tabs[3])
	column += printf(entry_pad + entry_width - (tabs[3] - column));
    else
	column += printf(" ");
    if (nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL]) {
	column += printf("%i", nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL]);
	if (nat->flags & IPNF_EXTENDED)
	    column += printf(":%d", nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL]);
    } else {
	column += printf("---");
    }

    if (verbose) {
	char buf1[20], buf2[20];

	sprint_dhms(buf1, nat->create_time);
	sprint_dhms(buf2, nat->used_time);
	printf("\n    create %s, use %s, ", buf1, buf2);
	if (!(nat->flags & IPNF_STATIC) &&
	    mgd_timer_running(&nat->inactivity_timer)) {
	    sprint_ticks_in_dhms(buf1,
			mgd_timer_left_sleeping(&nat->inactivity_timer));
	    printf("left %s, ", buf1);
	}
	printf("flags: ");
	printbits("", nat->flags, ipnat_flagnames, NIPNATFLAGNAMES, NULL);
	if ((nat->flags & IPNF_EXTENDED) && nat->edit_context &&
	    nat->edit_context->sd_pairs) {
	    printf("\n    seq-deltas:");
	    ipnat_dump_all_seq_delta_pairs(nat->edit_context);
	}
    }
}

static const char narrow_hdr[] = "Pro Inside global      Inside local       "
			      "Outside local      Outside global";
static const char wide_hdr[] = "Pro Inside global         Inside local          "
			    "Outside local         Outside global";

void
ipnat_show_entries (boolean verbose)
{
    ipnat_entry *nat;
    int i;
    boolean hdrshown, wide;

    if (stdio && stdio->tty_width >= 90)
	wide = TRUE;
    else
	wide = FALSE;
	
    automore_enable(NULL);
    hdrshown = FALSE;
    for (i = 0; i < IPNAT_HASHLEN; i++) {
	if (automore_quit())
	    break;
	for (nat = ipnat_hash[IPNAT_GLOBAL][i]; nat;
	     nat = nat->hash_next[IPNAT_GLOBAL]) {
	    if (automore_quit())
		break;
		
	    ipnat_lock_nat(nat);
	    if (!hdrshown) {
		automore_header(wide ? wide_hdr : narrow_hdr);
		hdrshown = TRUE;
	    }
	    ipnat_show_entry(nat, verbose, wide);

	    ipnat_free_nat(nat);	/* unlock and possibly free */
	}
    }

    automore_disable();
}

/*
 * ipnat_show_stats
 *
 * Show some statistics 
 */

void
ipnat_show_stats (void)
{
    list_element *el;
    ipnat_mapping *mapping;
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;
    hwidbtype *hwidb;
    idbtype *idb;
    boolean first;

    printf("\nTotal active translations: %d (%d static, %d dynamic;"
	   " %d extended)",
	   ipnat_stats.entries, ipnat_stats.statics,
	   ipnat_stats.entries - ipnat_stats.statics,
	   ipnat_stats.flows);

    printf("\nOutside interfaces: ");
    first = TRUE;
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_DELETED)
	    continue;
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (idb->ip_info->nat_enabled && idb->ip_info->nat_outside) {
		printf("%s%s", first ? "" : ", ", idb->namestring);
		if (first)
		    first = FALSE;
	    }
	}
    }

    printf("\nInside interfaces: ");
    first = TRUE;
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_DELETED)
	    continue;
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (idb->ip_info->nat_enabled && !idb->ip_info->nat_outside) {
		printf("%s%s", first ? "" : ", ", idb->namestring);
		if (first)
		    first = FALSE;
	    }
	}
    }

    printf("\nHits: %d  Misses: %d", ipnat_stats.hits, ipnat_stats.misses);
    printf("\nExpired translations: %d", ipnat_stats.entry_timeouts);
    printf("\nDynamic mappings:");
    for (domain = IPNAT_INSIDE; domain < IPNAT_DOMAINS; domain++) {
	for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	    if (!LIST_EMPTY(&ipnat_mappings[domain][lookup])) {
		char *s1, *s2;

		if (domain == IPNAT_INSIDE) {
		    s1 = "Inside";
		    s2 = (lookup == IPNAT_LOCAL) ? "Source" : "Destination";
		} else {
		    s1 = "Outside";
		    s2 = (lookup == IPNAT_LOCAL) ? "Destination" : "Source";
		}
		printf("\n-- %s %s", s1, s2);

		FOR_ALL_DATA_IN_LIST(&ipnat_mappings[domain][lookup], el,
				     mapping) {
		    switch (mapping->match_type) {
		    case IPNAT_MATCH_ACL:
			printf("\naccess-list %s ", mapping->acl->name);
			break;
		    case IPNAT_MATCH_ROUTE_MAP:
			printf("\nroute-map %s ", mapping->mapname);
			break;
		    }
		    switch (mapping->source_type) {
		    case IPNAT_SOURCE_POOL:
			printf("pool %s refcount %d", mapping->poolname,
			       mapping->refcount);
			ipnat_pool_dump_stats(mapping->poolname);
			break;
		    case IPNAT_SOURCE_INTERFACE:
			printf("interface %s refcount %d",
			       mapping->idb->namestring, mapping->refcount);
			break;
		    }
		}
	    }
	}
    }
    if (internal_cmd_enable) {
	int chain_len, longest_chain_len, total_chain_len, chains;
	ipnat_entry *nat;
	int i;

	for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	    longest_chain_len = total_chain_len = chains = 0;

	    for (i = 0; i < IPNAT_HASHLEN; i++) {
		chain_len = 0;
		nat = ipnat_hash[lookup][i];
		if (nat) {
		    chains++;
		    while (nat) {
			chain_len++;
			nat = nat->hash_next[lookup];
		    }
		    if (chain_len > longest_chain_len)
			longest_chain_len = chain_len;
		    total_chain_len += chain_len;
		}
	    }
	    printf("\nlongest chain in %s hash: %d, average length %d, "
		   "chains %d/%d",
		   lookup == IPNAT_LOCAL ? "local" : "global",
		   longest_chain_len, total_chain_len / IPNAT_HASHLEN,
		   chains, IPNAT_HASHLEN);
	}
    }
}

/*
 * ipnat_db_init
 *
 * Initialize all the database stuff: hash tables, lists, chunks, etc.
 */

void
ipnat_db_init (void)
{
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;

    if (ipnat_debug)
	buginf("\nNAT: allocating hash tables");
    ipnat_hash[IPNAT_LOCAL] = malloc(sizeof(ipnat_entry *) * IPNAT_HASHLEN);
    if (ipnat_hash[IPNAT_LOCAL] == NULL)
	return;
    ipnat_hash[IPNAT_GLOBAL] = malloc(sizeof(ipnat_entry *) * IPNAT_HASHLEN);
    if (ipnat_hash[IPNAT_GLOBAL] == NULL) {
	free(ipnat_hash[IPNAT_GLOBAL]);
	return;
    }

    for (domain = IPNAT_INSIDE; domain < IPNAT_DOMAINS; domain++) {
	for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	    ipnat_statics[domain][lookup] = NULL;
	    list_create(&ipnat_mappings[domain][lookup], 0,
			"IP NAT Mappings", 0);
	}
    }
    ipnat_portlist_init();

    nat_chunks = chunk_create(sizeof(ipnat_entry), IPNAT_ENTRY_CHUNK_MAX,
			      CHUNK_FLAGS_DYNAMIC, NULL, 0,
			      "NAT Entry Chunks");
    if (nat_chunks == NULL) {
	free(ipnat_hash[IPNAT_LOCAL]);
	free(ipnat_hash[IPNAT_GLOBAL]);
    }
    port_range_chunks = chunk_create(sizeof(port_range),
				     IPNAT_PORT_RANGE_CHUNK_MAX,
				     CHUNK_FLAGS_DYNAMIC, NULL, 0,
				     "NAT Port Range Chunks");
    if (port_range_chunks == NULL) {
	chunk_destroy(nat_chunks);
	free(ipnat_hash[IPNAT_LOCAL]);
	free(ipnat_hash[IPNAT_GLOBAL]);
    }

    list_create(&ipnat_door_list, 0, "IP NAT Doors", 0);
    list_create(&ipnat_fragment_list, 0, "IP NAT Fragments", 0);

    ipnat_stats.initialized = TRUE;
}
