/* $Id: x25_address.c,v 3.6.46.9 1996/08/28 13:23:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_address.c,v $
 *------------------------------------------------------------------
 * x25_address.c -- X25 and higher level address mapping support
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_address.c,v $
 * Revision 3.6.46.9  1996/08/28  13:23:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.46.8  1996/06/28  23:32:39  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.46.7  1996/06/12  19:23:41  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.6.46.6  1996/06/01  07:19:46  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Add new CUD (0xDA) for CDP
 * Branch: California_branch
 *
 * Revision 3.6.46.5  1996/05/17  12:17:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.32.3  1996/05/06  00:10:55  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.32.2  1996/04/06  01:06:14  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.32.1  1996/04/03  22:17:53  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.46.4  1996/05/10  20:40:27  suresh
 * CSCdi51667:  Encapsulation Failed for Broadcast packets encap x25 over
 * BRI
 * Branch: California_branch
 * Corrected to get the map from D Channel rather B Channel
 *
 * Revision 3.6.46.3  1996/04/27  06:46:08  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.10.2  1996/04/26  00:04:53  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.10.1  1996/02/23  21:32:51  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.6.46.2  1996/04/15  21:25:25  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.46.1  1996/03/18  22:50:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.3  1996/03/13  02:13:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.14.2  1996/03/07  11:19:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.14.1  1996/02/20  21:55:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1995/12/17  18:41:56  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/06  18:59:05  atsao
 * CSCdi44889:  ip host mode not working over X.25
 * Check if tbridge_on_swidb is set to take care of the corner case where
 * no ip routing is enabled and no bridge-group is configured.
 *
 * Revision 3.4  1995/11/27  08:44:27  atsao
 * CSCdi36544:  bridged IP to router doesnt work over X.25
 * ARP/IP cannot be bridged over X.25 when we are not routing IP and we
 * are the destination IP host bridge of these packets.
 *
 * Revision 3.3  1995/11/22  18:55:17  slin
 * CSCdi38892:  vines over X.25 requesting a full routing update on all
 *              PVCs
 * - Send only to newly established PVC
 *
 * Revision 3.2  1995/11/17  18:07:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/06  18:47:21  atsao
 * CSCdi31948:  changing encap from x25 to frame-relay does bot clean sh
 * x25 map
 * Show X25 maps only if the interface is X25 encapsulated or has CMNS
 * enabled with CMNS maps configured.
 *
 * Revision 2.4  1995/08/09  00:35:50  atsao
 * CSCdi38397:  TCP header compression malloc failure will case X.25 NULL
 * deref.
 * Initialize api data structure only when the map pointer is not set to
 * NULL.
 *
 * Revision 2.3  1995/07/26  01:22:56  carvalho
 * CSCdi37453:  parser should not generate nvc option for X25 PVC.
 *
 * Revision 2.2  1995/07/24  20:37:35  carvalho
 * CSCdi37303:  Router crashed in parser when doing show x25 map, while
 * maps were being added and deleted in another session.  The x25map
 * structure was protected against being freed, but was not left in a
 * consistent state.
 *
 * Revision 2.1  1995/06/07  23:22:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "address.h"
#include "config.h"
#include "../ui/parse_util.h"
#include "packet.h"
#include "name.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "ieee.h"
#include "../ui/parse_util.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "x25.h"
#include "x25_proto.h"
#include "x25_address.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"

x25_addr_entry *x25_addr_hash_table[X25_ADDR_HASH_SIZE];

ushort addr_linktype[ADDR_MAX];
char *addr_x121_name = "X.121";

/*
 *  x25_map_service2link
 *  Return the datagram/packet LINK_ type from the X.25 map service bits
 */
uint x25_map_service2link (uint service)
{
    switch (service) {
      case X25_MAPS_APOLLO:
	return(LINK_APOLLO);
      case X25_MAPS_APPLE:
	return(LINK_APPLETALK);
      case X25_MAPS_BRIDGE:
	return(LINK_BRIDGE);
      case X25_MAPS_CDP:
	return(LINK_CDP);
      case X25_MAPS_CLNS:
	return(LINK_CLNS);
      case X25_MAPS_COMP_TCP:
	return(LINK_COMPRESSED_TCP);
      case X25_MAPS_DECNET:
	return(LINK_DECNET);
      case X25_MAPS_IP:
	return(LINK_IP);
      case X25_MAPS_NOVELL:
	return(LINK_NOVELL);
      case X25_MAPS_VINES:
	return(LINK_VINES);
      case X25_MAPS_XNS:
	return(LINK_XNS);
      case X25_MAPS_PAD:
	return(LINK_PAD);
      case X25_MAPS_CMNS:
	return(LINK_CMNS);
      case X25_MAPS_QLLC:
      case X25_MAPS_QLLC_NATIVE:
	return(LINK_QLLC);
    }

    /*
     *  sanity check; connection oriented and Cisco proprietary services can't
     *  be combined with any other service
     */
    if ((service == 0) ||
	(service & X25_MAPS_CON_ORIENTED) ||
	(service & X25_MAPE_CISCO_ONLY))
	return(LINK_ILLEGAL);

    return(LINK_X25_MULTI_ENC);
}

/*
 *  x25_map_link2service
 *  Return the X.25 service bit from a datagram/packet LINK_ type
 */
uint x25_map_link2service (uint linktype)
{
    switch (linktype) {
      case LINK_APOLLO:
	return(X25_MAPS_APOLLO);
      case LINK_APPLETALK:
	return(X25_MAPS_APPLE);
      case LINK_BRIDGE:
      case LINK_DEC_SPANNING:
      case LINK_IEEE_SPANNING:
	return(X25_MAPS_BRIDGE);
      case LINK_CDP:
	return(X25_MAPS_CDP);
      case LINK_CLNS:
      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
      case LINK_ISIS_ALL_L1_IS:
      case LINK_ISIS_ALL_L2_IS:
      case LINK_CLNS_BCAST:
	return(X25_MAPS_CLNS);
      case LINK_COMPRESSED_TCP:
      case LINK_UNCOMPRESSED_TCP:
	return(X25_MAPS_COMP_TCP);
      case LINK_DECNET:
      case LINK_DECNET_ROUTER_L1:
      case LINK_DECNET_ROUTER_L2:
      case LINK_DECNET_NODE:
      case LINK_DECNET_PRIME_ROUTER:
	return(X25_MAPS_DECNET);
      case LINK_IP:
	return(X25_MAPS_IP);
      case LINK_NOVELL:
	return(X25_MAPS_NOVELL);
      case LINK_VINES:
	return(X25_MAPS_VINES);
      case LINK_XNS:
	return(X25_MAPS_XNS);
      case LINK_PAD:
	return(X25_MAPS_PAD);
      case LINK_CMNS:
	return(X25_MAPS_CMNS);
      case LINK_QLLC:
	return(X25_MAPS_QLLC);
      case LINK_X25_MULTI_ENC:
	return(X25_MAPS_ENCAPS & ~X25_MAPE_CISCO_ONLY);
      default:
	return(0);
    }
}

/*
 *  x25_map_hash_init
 *  Initialize the mapped address hash table
 */

void x25_map_hash_init (void)
{
    int hash_bucket;
    int inx;

    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
	x25_addr_hash_table[hash_bucket] = NULL;
    }

    /*
     *  initialize the address linktype array which, unfortunately, can't be
     *  done statically
     */
    for (inx = 0; inx < ADDR_MAX; inx++)
	addr_linktype[inx] = LINK_ILLEGAL;

    addr_linktype[ADDR_IP] = LINK_IP;
    addr_linktype[ADDR_DECNET] = LINK_DECNET;
    addr_linktype[ADDR_XNS] = LINK_XNS;
    addr_linktype[ADDR_X121] = LINK_X25;
    addr_linktype[ADDR_APPLE] = LINK_APPLETALK;
    addr_linktype[ADDR_CLNS] = LINK_CLNS;
    addr_linktype[ADDR_LAT] = LINK_LAT;
    addr_linktype[ADDR_VINES] = LINK_VINES;
    addr_linktype[ADDR_CMNS] = LINK_CMNS;
    addr_linktype[ADDR_APOLLO] = LINK_APOLLO;
    addr_linktype[ADDR_STUN] = LINK_STUN;
    addr_linktype[ADDR_NOVELL] = LINK_NOVELL;
    addr_linktype[ADDR_QLLC] = LINK_QLLC;
}

/*
 *  x25_map_hash_restart
 *  Handle the mapped VCs after an interface RESTART event
 */

void x25_map_hash_restart (
    register hwidbtype *hwidb,
    register llctype *llc2)
{
    register int hash_bucket;
    register x25_addr_entry *addr_entry;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  delete all transient maps that that belong to the restarted interface
     */
    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; ) {
	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    if (((addr_entry->map->map_status & X25_MAP_CONFIGURED_M) !=
		 X25_MAP_CONFIGURED_V) &&
		(addr_entry->map->map_idb->hwptr == hwidb) &&
		(addr_entry->map->map_llc2 == llc2)) {
		x25_map_delete(addr_entry->map, FALSE);
		break;
	    }
	}

	/*
	 *  a map deletion modifies the chain in the hash bucket--
	 *  move on to the next bucket only if we didn't delete a map
	 */
	if (addr_entry == NULL)
	    hash_bucket++;
    }

    reset_interrupt_level(level);
}

/*
 *  x25_map_create
 *
 *  Create a mapping for one or more services.
 *
 *  The primary address (i.e. the address that would be present in an X.25
 *  packet, namely the X.121 address for encapsulation or PAD mappings or
 *  the CONS NSAP for CMNS mappings) must be the first address in the variable
 *  number of address pointer arguments.
 *
 *  Each service mapped must have a pointer to its secondary address (if any)
 *  presented in the arguments.  Services that do not take a secondary
 *  address (e.g. CLNP) should not attempt to define one.
 *
 *  The list of pointers to secondary addresses may have NULL pointers or
 *  reference addresses of type ADDR_ILLEGAL to facilitate coding.
 */
x25map *x25_map_create (
    register idbtype *idb,
    ushort map_services,
    uint addr_cnt,
    addrtype *primary_addr,
    ... /* any secondary addresses */)
{
    register x25map *map;
    x25_addr_entry *addr_entry;
    addrtype *addr;
    va_list argptr;
    uint inx;
    uint cnt;
    uint hash;
    leveltype level;

    /*
     *  count the number of addresses we'll need to store in the map
     */
    va_start(argptr, primary_addr);
    for (cnt = 0, inx = 0; inx < addr_cnt; inx++) {
	if (inx == 0)
	    addr = primary_addr;
	else
	    addr = va_arg(argptr, addrtype *);

	if (addr && (addr->type != ADDR_ILLEGAL))
	    cnt++;
    }
    va_end(argptr);

    /*
     *  create the structure allowing for the defined address entry
     */
    if ((cnt < 1) ||
	((map = malloc(sizeof(x25map) +
		       ((cnt - 1) * sizeof(x25_addr_entry)))) == NULL)) {
	return(NULL);
    }

    /*
     *  initialize the structure
     *  assumes that the malloc()'ed block has been zero-filled
     */

    /*
     *  configuration information
     *  should be modified by the invoker as needed; configured facilities
     *  all default to zero
     */
    map->map_status = X25_MAP_TEMP;
    map->map_cud_pid_method = X25_CUD_PID_DEFAULT;
    map->map_idle = X25_MAP_DEFAULT_IDLE;
    map->map_flags = X25_MAPF_DEFAULT;
    map->map_maxnvc = idb->hwptr->x25_nvc;

    /*
     *  internal information
     *  most defaults to zero
     */
    map->map_idb = idb;

    /*
     *  process the variable number of addresses
     */

    /*
     *  set up a pointer to the first of the map's address entries and
     *  (re)initialize the variable parameter handle
     */
    addr_entry = &map->map_address[X25_MAP_PRIMARY_ADDR_INX];
    va_start(argptr, primary_addr);

    for (cnt = 0; cnt < addr_cnt; cnt++) {
	/*
	 *  get the pointer to the next address
	 */
	if (cnt == 0)
	    addr = primary_addr;
	else
	    addr = va_arg(argptr, addrtype *);

	if ((addr == NULL) || (addr->type == ADDR_ILLEGAL))
	    continue;

	/*
	 *  copy a recognized address type and do the recordkeeping
	 */

	switch (addr->type) {
	  case ADDR_APOLLO:
	    map->map_services |= map_services & X25_MAPS_APOLLO;
	    map->map_service_cnt++;
	    break;
	  case ADDR_APPLE:
	    map->map_services |= map_services & X25_MAPS_APPLE;
	    map->map_service_cnt++;
	    break;
	  case ADDR_CMNS:
	    map->map_services |= map_services & X25_MAPS_CMNS;
	    map->map_service_cnt++;
	    break;
	  case ADDR_DECNET:
	    map->map_services |= map_services & X25_MAPS_DECNET;
	    map->map_service_cnt++;
	    break;
	  case ADDR_X121:
	    /*
	     *  count the services with no secondary address
	     */
	    for (inx = 1; inx & X25_MAPS_ALL_BITS; inx <<= 1) {
		if ((inx & X25_MAPE_NO_SVC_ADDRESS) && (map_services & inx))
		    map->map_service_cnt++;
	    }
	    map->map_services |= map_services & X25_MAPE_NO_SVC_ADDRESS;
	    break;
	  case ADDR_IP:
	    if (map_services & X25_MAPS_COMP_TCP)
		map->map_service_cnt++;
	    if (map_services & X25_MAPS_IP)
		map->map_service_cnt++;
	    map->map_services |= map_services &
		(X25_MAPS_COMP_TCP | X25_MAPS_IP);
	    break;
	  case ADDR_NOVELL:
	    map->map_services |= map_services & X25_MAPS_NOVELL;
	    map->map_service_cnt++;
	    break;
	  case ADDR_VINES:
	    map->map_services |= map_services & X25_MAPS_VINES;
	    map->map_service_cnt++;
	    break;
	  case ADDR_XNS:
	    map->map_services |= map_services & X25_MAPS_XNS;
	    map->map_service_cnt++;
	    break;
	  case ADDR_QLLC:
	    map->map_services |= map_services & X25_MAPS_QLLC;
	    map->map_service_cnt++;
	    break;
	  case ADDR_MAC_KLUDGE:
	    break;
	  default:
	    continue;
	}

	map->map_addr_count++;
	addr_entry->map = map;
	address_copy(&addr_entry->addr, addr);
	addr_entry++;
    }

    va_end(argptr);

    /*
     *  validate the resulting map structure; we must have accounted for
     *  all requested services and these services must form a valid link type
     */

    if ((map->map_services != map_services) ||
	((map->map_linktype = x25_map_service2link(map_services)) ==
	 LINK_ILLEGAL)) {
	free(map);
	return(NULL);
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  insert the addresses into the hash table
     */
    for (inx = 0; inx < map->map_addr_count; inx++) {
	hash = X25_ADDR_HASH(&map->map_address[inx].addr);
	map->map_address[inx].next = x25_addr_hash_table[hash];
	x25_addr_hash_table[hash] = &map->map_address[inx];
    }

    /*
     *  check if the max. nvc count should be set to 1 (if a map with
     *  multiple encapsulation services includes a service that uses a
     *  single VC, it will only use the first mapped VC that is capable
     *  of carrying the protocol to send datagrams).
     */
    if ((map->map_services & X25_MAPS_CON_ORIENTED) ||
	((map->map_services & X25_MAPE_SINGLE_VC) &&
	 (map->map_service_cnt == 1)))
	map->map_maxnvc = 1;

    /*
     *  handle any required service initialization
     */
    if (map->map_services & X25_MAPS_COMP_TCP) {
	map->map_thc = malloc(sizeof(thcdbtype));
	if (map->map_thc) {
	    if (idb->hwptr->tcp_compress)
		(void) ip_header_compression_set(idb, FALSE, FALSE);
	    reg_invoke_hdr_compress_init(LINK_IP, map->map_thc,
					 DEFAULT_THC_STATES);
	    /*
	     *  initialize passive or active operation
	     */
	    if (map->map_flags & X25_MAPF_PASSIVE)
		map->map_thc->flags &= ~(THC_PERMACTIVE | THC_ACTIVE);
	    else
		map->map_thc->flags |= THC_PERMACTIVE | THC_ACTIVE;
	    idb->hwptr->x25_tcp_compress = TRUE;
	} else {
	    x25_map_delete(map, FALSE);
	    map = NULL;
	}
    }

    if (map) { 
	/*
	 *  initialize api data structure
	 */
	if (!reg_invoke_media_x25_address_map_enter(map->map_linktype, map)) {
	    x25_map_delete(map, FALSE);
	    map = NULL;
	}
    }

    reset_interrupt_level(level);
    return(map);
}

/*
 *  x25_map_match_service
 *
 * Return TRUE if the service matches regardless of protocol
 */
boolean x25_map_match_service (x25map *map, ushort service)
{
    if (service & X25_MAPS_ALL_QLLC) {
	service |= X25_MAPS_ALL_QLLC;	/*   */
    } 
    return(service & map->map_services);
}

/*
 *  x25_map_find
 *  Find an X.25 map structure for a given address/services, possibly skipping
 *  incoming-only or outgoing-only maps
 */
x25map *x25_map_find (
    hwidbtype *hwidb,
    addrtype *addr,
    uint match,
    uint services)
{
    register x25_addr_entry *hash_addr;
    x25map *map;
    ushort skip_mask;
    leveltype level;

    map = NULL;

    /*
     * If this interface is a member of a rotary group, lookup the map in the
     * master.
     */
    if (hwidb != NULL) {
	hwidb = get_netidb(hwidb);
    }

    /*
     *  determine the mask value to apply to map_flags
     */
    if (match & X25_MATCH_INCOMING)
	skip_mask = X25_MAPF_BAR_IN;
    else if (match & X25_MATCH_OUTGOING)
	skip_mask = X25_MAPF_BAR_OUT;
    else
	skip_mask = 0;

    level = raise_interrupt_level(NETS_DISABLE);

    if ((addr->type == ADDR_CMNS) &&
	(match & X25_MATCH_NSAP_BESTFIT)) {
	uint inx;
	int best_val = 0;
	int match_val;

	/*
	 *  find the closest match for the NSAP address, which may
	 *  reside anywhere in the hash table
	 */
	for (inx = 0; inx < X25_ADDR_HASH_SIZE; inx++) {
	    for (hash_addr = x25_addr_hash_table[inx];
		 hash_addr; hash_addr = hash_addr->next) {
		if ((hash_addr->addr.type == ADDR_CMNS) &&
		    (x25_map_match_service(hash_addr->map, services)) &&
		    ((hash_addr->map->map_flags & skip_mask) == 0) &&
		    (((hash_addr->map->map_status &
		       X25_MAP_CONFIGURED_M) == X25_MAP_CONFIGURED_V) ||
		     ((match & X25_MATCH_CONFIGURED) == 0))) {
		    match_val = reg_invoke_media_x25_nsap_compare(
			&hash_addr->addr.cmns_addr[0], &addr->cmns_addr[0]);

		    if (match_val > best_val) {
			best_val = match_val;
			map = hash_addr->map;
		    }
		}
	    }
	}
    } else {
	/*
	 *  we're looking for an exact match of this address
	 */
	for (hash_addr = x25_addr_hash_table[X25_ADDR_HASH(addr)];
	     hash_addr; hash_addr = hash_addr->next) {
	    if (((hash_addr->map->map_idb->hwptr == hwidb) ||
		 (hwidb == NULL)) &&
		(hash_addr->addr.type == addr->type) &&
		    (x25_map_match_service(hash_addr->map, services)) &&
		((hash_addr->map->map_flags & skip_mask) == 0) &&
		(bcmp(&hash_addr->addr, addr, sizeof(addrtype)) == 0) &&
		(((hash_addr->map->map_status &
		   X25_MAP_CONFIGURED_M) == X25_MAP_CONFIGURED_V) ||
		 ((match & X25_MATCH_CONFIGURED) == 0))) {
		map = hash_addr->map;
		break;
	    }
	}
    }

    reset_interrupt_level(level);

    return(map);
}

/*
 *  x25_map_delete
 *  Delete a X.25 map structure.
 */
void x25_map_delete (
    register x25map *map,
    boolean clear)
{
    register x25_addr_entry **hash_ptr;
    register x25_addr_entry *addr_entry;
    uint vc;
    leveltype level;

    /*
     *  ignore a NULL map address--allows x25_map_delete(x25_map_find())
     */
    if (!map)
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  unlink its hashed address table entries
     *  note that the map can persist after we free it (its memory may have
     *  been locked), so we must ensure that it's left in a consistent state
     *  so other processes using it don't get confused.  Specifically,
     *  map_addr_count must be zero when we're done (CSCdi37303).
     */
    while (map->map_addr_count) {
	addr_entry = &map->map_address[--map->map_addr_count];

	for (hash_ptr = &x25_addr_hash_table[X25_ADDR_HASH(&addr_entry->addr)];
	     ((*hash_ptr) != addr_entry); hash_ptr = &(*hash_ptr)->next)
	    ; /* EMPTY */

	reg_invoke_media_x25_address_map_delete(
			map->map_linktype, &addr_entry->addr);
	*hash_ptr = addr_entry->next;
	addr_entry->next = NULL;
	addr_entry->addr.type = ADDR_ILLEGAL;
    }

    /*
     *  remove all VCs from this mapping
     */

    if (clear && map->map_nvc)
	x25_map_update_nvc(map, 0);

    for (vc = map->map_nvc; vc--; ) {
	map->map_lci[vc]->lci_map = NULL;
	map->map_lci[vc] = NULL;
    }

    if (map->map_services & X25_MAPS_BRIDGE)
	reg_invoke_bridge_clear_interface(map->map_idb);

    /*
     *  check if any COMPRESSED_TCP service remains for this interface
     */
    if (map->map_services & X25_MAPS_COMP_TCP) {
	map->map_idb->hwptr->x25_tcp_compress =
	    (x25_map_type_check(map->map_idb->hwptr,
				NULL, LINK_COMPRESSED_TCP) != NULL);
    }

    reset_interrupt_level(level);

    /*
     *  free all malloc-ed memory
     */
    if (map->map_thc) {
	if (map->map_thc->tstate)
	    free(map->map_thc->tstate);
	if (map->map_thc->rstate)
	    free(map->map_thc->rstate);
	free(map->map_thc);
	map->map_thc = NULL;
    }

    if (map->map_bfe)
	free(map->map_bfe);
    if (map->map_fac.nuid)
	free(map->map_fac.nuid);
    map->map_bfe = NULL;
    map->map_fac.nuid = NULL;

    free(map);
}

/*
 *  x25_map_delete_subint
 *  Delete the map statements associated with a subinterface
 */

void x25_map_delete_subint (register idbtype *idb)
{
    register x25_addr_entry *addr_entry;
    register int hash_bucket;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; ) {
	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    if (addr_entry->map->map_idb == idb) {
		x25_map_delete(addr_entry->map, TRUE);
		break;
	    }
	}

	/*
	 *  map deletion disrupts the address chain, so only go on
	 *  to the next bucket if this pass deleted no maps
	 */
	if (addr_entry == NULL)
	    hash_bucket++;
    }

    reset_interrupt_level(level);
}

/*
 *  x25_map_addr_match
 *  Determine if an address is mapped
 */
boolean x25_map_addr_match(
    register x25map *map,
    register addrtype *addr)
{
    register uint inx;

    for (inx = 0; inx < map->map_addr_count; inx++) {
	if (addr->type == map->map_address[inx].addr.type)
	    return((addr->length == map->map_address[inx].addr.length) &&
		   (bcmp(&addr->addr,
			 &map->map_address[inx].addr.addr, addr->length) == 0));
    }

    return(FALSE);
}

/*
 *  x25_map_add_lci
 *  Assign a Virtual Circuit to a map.
 *
 *  Connection-oriented services do not need to supply a map structure;
 *  the necessary VC fields will be set.
 *
 *  For incoming encapsulation Calls, the decoded service type and CUD
 *  Protocol ID method used must be supplied.
 *
 *  For outgoing encapsulation Calls, the configured map/interface CUD
 *  method to use will be set.
 */
boolean x25_map_add_lci(
    register lcitype *lci,
    register x25map *map)
{
    uint service;
    uint lci_services;
    uint inx;
    uint linktype;
    addrtype *proto_addr;
    void (*media_add)(int linktype, idbtype *idb, addrtype *proto_addr);

    /*
     *  validate the assignment of this VC to the map
     */
    if ((lci->lci_map) ||
	(map &&
	 (map->map_nvc >= map->map_maxnvc) &&
	 (map->map_services & X25_MAPS_ENCAPS)))
	return(FALSE);

    lci_services = (map && (lci->lci_linktype == LINK_X25_MULTI_ENC)) ?
		map->map_services : x25_map_link2service(lci->lci_linktype);

    if (!map) {
	/*
	 *  only connection-oriented VCs may exist without a map structure
	 */
	lci->lci_map = NULL;
    } else if (map->map_services & X25_MAPS_CON_ORIENTED) {
	lci->lci_map = map;
	lci->lci_idb = map->map_idb;	/* map may be on a subinterface */
    } else {	/* encapsulation VC */
	/*
	 *  protocols that require a single VC can't have multiple
	 *  single-protocol VCs
	 */
	if ((lci_services & X25_MAPE_SINGLE_VC) &&
	    (lci->lci_linktype != LINK_X25_MULTI_ENC)) {
	    for (inx = 0; inx < map->map_nvc; inx++) {
		if (map->map_lci[inx]->lci_linktype == lci->lci_linktype)
		    return(FALSE);
	    }

	    /*
	     *  put a single-protocol VC before any multiprotocol VCs
	     */
	    for (inx = map->map_nvc; inx; inx--)
		map->map_lci[inx] = map->map_lci[inx - 1];
	    map->map_lci[0] = lci;
	} else {
	    map->map_lci[map->map_nvc] = lci;
	}

	map->map_nvc++;
	lci->lci_map = map;
	/*
	 * If map does not belong to hunt group leader, change idb ptr.
	 */
	if (!reg_invoke_dialer_huntgroup_leader(map->map_idb->hwptr))
	    lci->lci_idb = map->map_idb;      /* map may be on a subinterface */

	/*
	 *  for outgoing Calls, determine the CUD PID method to use
	 */
	if (lci->lci_outgoing) {
	    lci->lci_cud_pid_method =
		(map->map_cud_pid_method == X25_CUD_PID_DEFAULT) ?
		    map->map_idb->hwptr->x25_cud_pid_def :
		    map->map_cud_pid_method;

	    /*
	     *  force multiprotocol encapsulation when required
	     */
	    if ((map->map_service_cnt > 1) ||
		((x25_map_link2service(lci->lci_linktype) &
		  X25_MAPE_FORCE_MULTI_ENC) &&
		 (lci->lci_cud_pid_method != X25_CUD_PID_CISCO)))
		lci->lci_cud_pid_method = X25_CUD_PID_MULTI;
	}

	/*
	 *  determine the data PID method to use
	 */
	if (lci->lci_cud_pid_method == X25_CUD_PID_MULTI)
	    lci->lci_data_pid_method =
		(map->map_cud_pid_method == X25_CUD_PID_SNAP) ?
		    X25_DATA_PID_SNAP : X25_DATA_PID_MULTI;

	/*
	 *  set the VC idle value
	 */
	lci->lci_idle = ONEMIN *
		((map->map_idle == X25_MAP_DEFAULT_IDLE) ?
		 lci->lci_idb->hwptr->x25_idle : map->map_idle);
    }

    /*
     *  Tell the appropriate upper layer protocol(s), taking care
     *  to only notify those protocols the VC can actually transport.
     *
     *  WARNING!!!!  X.25 can do quirky things with VCs.  For instance,
     *  notification that a PVC has been created does not necessarily mean
     *  that the station at the other in is present and "connected", it
     *  only means that a pipe has been created on this end. The code is
     *  kinda twisted so caveat programmer!
     */
    media_add = (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) ?
	reg_invoke_media_x25_pvc_create : reg_invoke_media_x25_svc_connect;

    /*
     *  step through all the services mapped
     */
    for (service = 1;
	 service & (X25_MAPS_ENCAPS | X25_MAPS_CON_ORIENTED); service <<= 1) {
	if (lci_services & service) {
	    /*
	     *  search for the datagram address
	     */
	    linktype = x25_map_service2link(service);
	    proto_addr = NULL;

	    for (inx = X25_MAP_SECONDARY_ADDR_INX;
		 inx < map->map_addr_count; inx++) {
		if (addr_linktype[map->map_address[inx].addr.type] ==
		    linktype) {
		    proto_addr = &map->map_address[inx].addr;
		    break;
		}
	    }

	    if (proto_addr) {
		(*media_add)(linktype, lci->lci_idb, proto_addr);
	    }
	}	    
    }

    return(TRUE);
}

/*
 * Remove LCI from hash table.
 */

void x25_map_remove_lci (
    register lcitype *lci)
{
    register x25map *map;
    register int inx;
    uint lci_services;
    uint service;
    void (*media_remove)(int linktype, idbtype *idb, lcitype *lci);

    if ((map = lci->lci_map) == NULL)
	return;
    lci->lci_map = NULL;

    /*
     *  Find the lci pointer in the map array
     */
    for (inx = 0; (inx < map->map_nvc) && (lci != map->map_lci[inx]); inx++)
	; /* EMPTY */

    if (inx >= map->map_nvc) {
	/*
	 *  we have a serious error if an encapsulation map isn't tracking
	 *  one of its VCs
	 */
	if (map->map_services & X25_MAPS_ENCAPS) {
	    errmsg(&msgsym(NOLCI, X25), lci->lci_idb->namestring,
		   lci->lci_lci, "addr map lci");
	    return;
	}
    } else {
	/*
	 *  fiddle the map's index of the last used VC
	 */
	if (map->map_index == inx)
	    map->map_index = 0;
	else if (map->map_index > inx)
	    map->map_index--;

	/*
	 *  shuffle the map VC pointers
	 */
	map->map_nvc--;
	while (inx < map->map_nvc) {
	    map->map_lci[inx] = map->map_lci[inx + 1];
	    inx++;
	}
	map->map_lci[map->map_nvc] = NULL;
    }

    /*
     *  Tell the appropriate upper layer protocol(s), taking care
     *  to only notify those protocols the VC can actually transport.
     *
     *  WARNING!!!!  X.25 can do quirky things with VCs.  For
     *  instance, a failed CALL can have its VC hang around for a
     *  period of time to fulfill the hold-down timer. The code is
     *  kinda twisted so caveat programmer!
     */
    lci_services = (lci->lci_linktype == LINK_X25_MULTI_ENC) ?
		map->map_services : x25_map_link2service(lci->lci_linktype);
    media_remove = (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) ?
	reg_invoke_media_x25_pvc_delete : reg_invoke_media_x25_svc_clear;

    for (service = 1;
	 service & (X25_MAPS_ENCAPS | X25_MAPS_CON_ORIENTED); service <<= 1) {
	if (lci_services & service)
	    (*media_remove)(x25_map_service2link(service), lci->lci_idb, lci);
    }

    /*
     *  delete an impermanent address map when all VCs that use it are gone
     */
    if ((map->map_nvc == 0) && (map->map_status != X25_MAP_PERM))
	x25_map_delete(map, FALSE);
}

/*
 *  x25_map_update_nvc
 *  Update a map's maximum number of VCs
 */

void x25_map_update_nvc (
    register x25map *map,
    int new)
{
    lcitype *lci;

    map->map_maxnvc = new;

    while (map->map_nvc > new) {
	/*
	 *  get rid of the last VC mapped
	 */
	map->map_nvc -= 1;
	lci = map->map_lci[map->map_nvc];
	map->map_lci[map->map_nvc] = NULL;
	lci->lci_map = NULL;

	switch (lci->lci_state) {
	  case X25_X1:
	    x25_call_remove(lci, 0, 0);
	    break;
	  case X25_P2:
	  case X25_P3:
	  case X25_P5:
	  case X25_P4:
	  case X25_D1:
	  case X25_D2:
	  case X25_D3:
	    if (IS_LCI_PVC(map->map_idb->hwptr, lci->lci_lci))
		x25_call_remove(lci, 0, 0);
	    else
		x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			X25_CLEAR_NUMBER_BUSY, X25_DIAG_MAINTENANCE_ACTION,
			NULL);
	    break;
	}
    }
}

/*
 *  x25_map_update_if_nvc
 *  When the interface max. nvc is changed, we need to update the max. nvc
 *  value of all encapsulation maps that use the existing (i.e. interface
 *  default) value; this operation also includes CLEARing any existing VCs
 *  in excess of the new limit.
 */
void x25_map_update_if_nvc (
    register hwidbtype *hwidb,
    int new)
{
    int hash_bucket;
    x25_addr_entry *addr_entry;
    x25map *map;
    leveltype level;

    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
        level = raise_interrupt_level(NETS_DISABLE);

	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    map = addr_entry->map;
	    if ((map->map_idb->hwptr != hwidb) ||
		(map->map_status != X25_MAP_PERM) ||
		(map->map_services & X25_MAPS_CON_ORIENTED))
		continue;

	    if (map->map_maxnvc == hwidb->x25_nvc)
		x25_map_update_nvc(map, new);
	}

	reset_interrupt_level(level);
    }

    hwidb->x25_nvc = new;
}

/*
 *  x25_map_flush_hold
 *  Flush any calls that are in hold down (state X1) when an incoming call
 *  is received.
 */

void x25_map_flush_hold (x25map *map)
{
    int vc;

    for (vc = 0; vc < map->map_nvc; ) {
	if (map->map_lci[vc]->lci_state == X25_X1)
	    x25_call_remove(map->map_lci[vc], 0, 0);
	else
	    vc++;
    }
}

/*
 *  x25_map_flush_rpoa
 *  Scan address maps to remove all references to a given RPOA structure
 */

void x25_map_flush_rpoa (rpoatype *rp)
{
    register int hash_bucket;
    register x25_addr_entry *addr_entry;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    if (addr_entry->map->map_fac.rpoa == rp)
		addr_entry->map->map_fac.rpoa = NULL;
	}
    }

    reset_interrupt_level(level);
}

/*
 *  x25_map_type_check
 *  Check an interface to see if a map to the given address type is defined.
 */

x25map *x25_map_type_check (
    register hwidbtype *hwidb,
    register idbtype *idb,
    uint link_type)
{
    register int hash_bucket;
    register x25_addr_entry *addr_entry;
    uint service;
    leveltype level;

    /*
     * If this interface is a member of a rotary group, lookup the map in the
     * master.
     */
    hwidb = get_netidb(hwidb);
    if (idb && (idb->hwptr != hwidb))
	idb = hwidb->firstsw;

    service = x25_map_link2service(link_type);

    level = raise_interrupt_level(NETS_DISABLE);

    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    if ((idb ? (addr_entry->map->map_idb == idb) :
		       (addr_entry->map->map_idb->hwptr == hwidb)) &&
		    (x25_map_match_service(addr_entry->map, service)) ) {
		reset_interrupt_level(level);
		return(addr_entry->map);
	    }
	}
    }

    reset_interrupt_level(level);
    return(NULL);
}

/*
 *  x25_broadcast
 *  Support encapsulated protocol broadcast over X.25 virtual circuits;
 *  returns TRUE if one or more destinations were found and the first
 *  destination address is represented in the passed address structure,
 *  returns FALSE if no destinations were found.
 */

x25map *x25_broadcast (
    register idbtype *idb,
    register paktype *pak)
{
    register int hash_bucket;
    register x25_addr_entry *addr_entry;
    idbtype *swidb;
    paktype *newpak;
    x25map *first_map;
    x25map *map;
    uint service;

    first_map = NULL;
    service = x25_map_link2service(pak->linktype);

    swidb = get_pak_netidb(pak);

    /*
     *  find all maps for this protocol on this interface that
     *  accept broadcasts
     */
    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
	for (addr_entry = x25_addr_hash_table[hash_bucket];
	     addr_entry; addr_entry = addr_entry->next) {
	    map = addr_entry->map;

	    /*
	     *  skip this map if
	     *    it doesn't map this service, or
	     *    it's not on this subinterface, or
	     *    it doesn't transmit broadcasts
	     */
	    if (!(x25_map_match_service(map, service)) ||
		(map->map_idb != swidb) ||
		((map->map_flags & X25_MAPF_BCAST) == 0))
		continue;

	    /*
	     *  a map can be hashed multiple times, once for each address
	     *  mapped--skip map if this isn't the primary address
	     */
	    if ((&map->map_address[X25_MAP_PRIMARY_ADDR_INX]) != addr_entry)
		continue;

	    /*
	     *  the first destination found will get the original copy of
	     *  the broadcast datagram on returning from this procedure
	     */
	    if (first_map == NULL) {
		first_map = map;
		continue;
	    }

	    /*
	     *  all subsequent maps get a copy of the original packet
	     */
	    newpak = pak_duplicate(pak);
	    if (newpak) {
		if (x25_encaps(newpak, 0L, map))
		    x25_output(idb->hwptr, newpak, TAIL);
		else
		    datagram_done(newpak);
	    }
	}
    }

    return(first_map);
}

/*
 *  x25_nuid
 *  Generate an NUID facility encoding using either a Cisco or
 *  user-specified format.
 *
 *  Cisco format is:
 *    1		cisco magic cookie for nuid format (version number)
 *    N		user name length byte (up to 128)
 *    user	user name (no trailing zero)
 *    N		password length byte (up to 128, may be 0)
 *    password	password (omitted if length is 0)
 *
 *  User formatted NUID is whatever is specified.
 */

char nuid_string[X25_MAX_NUID_SIZE];

char *x25_nuid (
    x25map *map,
    boolean show)
{
    uchar *c, *s;

    if (map->map_fac.nuid == NULL || map->map_fac.nuid_len == 0)
	return(NULL);
    c = nuid_string;

    if (map->map_fac.cisco_nuid) {
	/* Cisco NUID format */
	s = map->map_fac.nuid + 1;
	bcopy(s + 1, c, *s);
	c += *s;
	if (show) {
	    s += *s + 1;
	    *c++ = ' ';
	    bcopy(s + 1, c, *s);
	    c += *s;
	}
	*c = '\0';
    } else if (show) {
	/* show user formatted NUID */
	bcopy((char *)map->map_fac.nuid, nuid_string, map->map_fac.nuid_len);
	nuid_string[map->map_fac.nuid_len] = '\0';
    } else {
	/* don't show user formatted NUID */
	return(NULL);
    }

    return(nuid_string);
}

/*
 *  x25_for_all_maps
 *  X.25 Address map visiting procedure, used for operations that need to
 *  visit all maps on an interface without disabling interrupts for the
 *  entire operation.  The passed procedure is invoked once for each map
 *  struture and is given the locked address of each map entry.  Note that
 *  there is a small chance that one or more map entries may be skipped or
 *  duplicated if the map table has entries added or deleted at interrupt
 *  level.
 */
static void x25_for_all_maps (
    idbtype *idb,
    void (*x25_map_invoke)(x25map *, void *),
    void *context)
{
    int hash_bucket;
    int addr_count;
    int inx;
    x25_addr_entry *addr_entry;
    x25map *map;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  index through the contents of all hash buckets
     */
    for (hash_bucket = 0; hash_bucket < X25_ADDR_HASH_SIZE; hash_bucket++) {
	for (addr_count = 0; ; addr_count++) {
	    /*
	     *  count off the addresses we've visited before
	     */
	    for (addr_entry = x25_addr_hash_table[hash_bucket], inx = 0;
		 addr_entry && (inx < addr_count);
		 addr_entry = addr_entry->next, inx++)
		; /* EMPTY */
	    /*
	     *  count past any secondary addresses or
	     *  addresses that don't belong to the requested interface
	     */
	    while (addr_entry &&
		   ((addr_entry !=
		     &addr_entry->map->map_address[X25_MAP_PRIMARY_ADDR_INX]) ||
		    (idb && (addr_entry->map->map_idb != idb)) ||
		    ((!ISCMNS_IEEE(addr_entry->map->map_idb->hwptr)) &&
		     (!is_x25(addr_entry->map->map_idb->hwptr))))) {
		addr_count++;
		addr_entry = addr_entry->next;
	    }

	    if (addr_entry == NULL)
		break;

	    /*
	     *  lock and visit the map
	     */
	    map = addr_entry->map;
	    mem_lock(map);
	    reset_interrupt_level(level);
	    (*x25_map_invoke)(map, context);
	    level = raise_interrupt_level(NETS_DISABLE);
	    free(map);
	}
    }

    reset_interrupt_level(level);
}

/*
 *  x25_cud_pid_method_str
 *  Generate the encapsulation method string
 */
char *x25_cud_pid_method_str (uint method)
{
    switch (method) {
      case X25_CUD_PID_NONE:
	return("none");
      case X25_CUD_PID_DEFAULT:
	return("default");
      case X25_CUD_PID_CISCO:
	return("cisco");
      case X25_CUD_PID_IETF:
	return("ietf");
      case X25_CUD_PID_SNAP:
	return("snap");
      case X25_CUD_PID_MULTI:
	return("multi");
    }
    return("?");
}

/*
 *  x25_map_entry_nvgen
 *  Generate the configuration information for an X.25 map entry or
 *  an encapsulation PVC
 */
void x25_map_entry_nvgen (x25map *map, void *context)
{
    char buffer[80];
    x25_addr_entry *addr_entry;
    uint count;

    /*
     *  if we have a context, we're examining all maps and generating
     *  the permanent NV configurations; if we don't have a context,
     *  we're generating the NV configuration of a special-case map (PVC),
     *  so no test applies
     */
    if (context != NULL) {
	if (map->map_status != X25_MAP_PERM)
	    return;

	/*
	 *  print the start of the new configuration line
	 */
	nv_write(TRUE, "%s", context);
    }

    /*
     *  generate the secondary protocol/address pairs for most structures;
     *  CONS must start with the primary address
     */
    if (map->map_services & X25_MAPS_CMNS) {
	addr_entry = &map->map_address[X25_MAP_PRIMARY_ADDR_INX];
	count = map->map_addr_count;
    } else {
	addr_entry = &map->map_address[X25_MAP_SECONDARY_ADDR_INX];
	count = map->map_addr_count - 1;
    }

    for ( ; count--; addr_entry++) {
	/*
	 *  print our address configuration information
	 */
	switch (addr_entry->addr.type) {
	  case ADDR_X121:
	    /*
	     *  special case--no address type
	     */
	    nv_add(TRUE, " %s", addr_entry->addr.x121_addr);
	    break;
	  case ADDR_MAC_KLUDGE:
	    /*
	     *  special case--non-standard representation
	     */
	    nv_add(TRUE, " %e", addr_entry->addr.mac_kludge_addr);
	    break;
	  case ADDR_DECNET:
	    /*
	     *  special case--address needs massaging
	     */
	    nv_add(TRUE, " %s %v", print_protocol(LINK_DECNET),
		   swap_16bit_word(addr_entry->addr.decnet_addr));
	    break;
	  case ADDR_IP:
	    /*
	     *  special case--two protocols use the address
	     */
	    nv_add((map->map_services & X25_MAPS_IP), " %s %i",
		   print_protocol(LINK_IP), addr_entry->addr.ip_addr);
	    nv_add((map->map_services & X25_MAPS_COMP_TCP), " %s %i",
		   print_protocol(LINK_COMPRESSED_TCP),
		   addr_entry->addr.ip_addr);
	    break;
	  default:
	    /*
	     *  standard case
	     */
	    reg_invoke_sprintf_address_link(
			addr_linktype[addr_entry->addr.type],
			buffer, &addr_entry->addr);
	    nv_add(TRUE, " %s %s",
		   print_protocol(addr_linktype[addr_entry->addr.type]),
		   buffer);
	    break;
	}
    }

    /*
     *  generate the remaining primary address information
     */
    if ((map->map_services & X25_MAPS_CMNS) == 0) {
	addr_entry = &map->map_address[X25_MAP_PRIMARY_ADDR_INX];
	reg_invoke_sprintf_address_link(
		addr_linktype[addr_entry->addr.type],
		buffer, &addr_entry->addr);

	nv_add((map->map_services & X25_MAPS_PAD), " pad");
	nv_add((map->map_services & X25_MAPS_BRIDGE), " bridge");
	nv_add((map->map_services & X25_MAPS_CDP), " cdp");
	nv_add((map->map_services & X25_MAPS_CLNS), " clns");
	nv_add((map->map_services & X25_MAPS_QLLC_NATIVE), " qllc");
	nv_add(TRUE, " %s", buffer);
    }

    /*
     *  generate the option configuration information for the map
     */
    nv_add(map->map_flags & X25_MAPF_BCAST, " broadcast");
    nv_add(map->map_fac.pin && map->map_fac.pout, " packetsize %d %d",
	   1 << map->map_fac.pin, 1 << map->map_fac.pout);
    nv_add(map->map_fac.win && map->map_fac.wout, " windowsize %d %d",
	   map->map_fac.win, map->map_fac.wout);
    nv_add(map->map_flags & X25_MAPF_PASSIVE, " passive");
    nv_add(map->map_cud_pid_method != X25_CUD_PID_DEFAULT, " method %s",
	   x25_cud_pid_method_str(map->map_cud_pid_method));
    nv_add(map->map_fac.rev, " reverse");
    nv_add(map->map_flags & X25_MAPF_REV_OK, " accept-reverse");
    nv_add(map->map_fac.cug, " cug %d", map->map_fac.cug);
    nv_add((map->map_maxnvc != map->map_idb->hwptr->x25_nvc) &&
	   (map->map_services & X25_MAPS_ENCAPS) &&
	   (map->map_services & ~X25_MAPE_SINGLE_VC) &&
	   (map->map_status != X25_MAP_PVC), " nvc %d",
	   map->map_maxnvc);
    nv_add(map->map_fac.tput_in && map->map_fac.tput_out, " throughput %d %d",
	   x25_facil_thru(map->map_fac.tput_in),
	   x25_facil_thru(map->map_fac.tput_out));
    if (map->map_fac.nuid_len)
	nv_add(TRUE, map->map_fac.cisco_nuid ? " nuid %s" : " nudata \"%s\"",
		x25_nuid(map, TRUE));
    if (map->map_fac.rpoa)
	nv_add(TRUE, " rpoa %s", map->map_fac.rpoa->name);
    nv_add(map->map_fac.tdelay, " transit-delay %d",
	   map->map_fac.tdelay);
    nv_add((map->map_flags & X25_MAPF_BAR_IN) &&
	   map->map_status != X25_MAP_PVC, " no-incoming");
    nv_add((map->map_flags & X25_MAPF_BAR_OUT) &&
	   map->map_status != X25_MAP_PVC, " no-outgoing");
    nv_add(map->map_idle != X25_MAP_DEFAULT_IDLE, " idle %d", map->map_idle);
    nv_add(map->map_flags & X25_MAPF_PBP, " compress");
}

/*
 *  x25_map_nvgen
 *  Generate the map configuration information for an interface
 */

void x25_map_nvgen (parseinfo *csb)
{
    x25_for_all_maps(csb->interface, x25_map_entry_nvgen, csb->nv_command);
}

/*
 *  show_x25map
 *  Display the data stored in an X.25 address map
 */

static void show_x25map (
    x25map *map,
    void *context)
{
    int count;
    x25_addr_entry *addr_entry;
    char buffer[80];

    printf("\n%s:", map->map_idb->namestring);

    for (count = 0, addr_entry = &map->map_address[X25_MAP_PRIMARY_ADDR_INX];
	 count < map->map_addr_count; count++, addr_entry++) {

	switch (addr_entry->addr.type) {
	  case ADDR_X121:
	    printf("%s X.121 %s%s", (count > 1) ? ",\n\t" : "",
		addr_entry->addr.x121_addr, (count == 0) ? " <-->" : "");
	    break;
	  case ADDR_MAC_KLUDGE:
	    printf("%s mac %e%s", (count > 1) ? ",\n\t" : "",
		addr_entry->addr.mac_kludge_addr, (count == 0) ? " <-->" : "");
	    break;
	  case ADDR_IP:
	    /*
	     *  an IP address is used for either or both IP and Compressed
	     *  TCP encapsulation
	     */
	    reg_invoke_sprintf_address_link(
			addr_linktype[addr_entry->addr.type],
			buffer, &addr_entry->addr);
	    if (map->map_services & X25_MAPS_IP) {
		printf("%s %s %s%s", (count > 1) ? ",\n\t" : "",
			print_protocol(LINK_IP), buffer,
			(count == 0) ? " <-->" : "");

		if (map->map_services & X25_MAPS_COMP_TCP)
		    printf("%s %s %s", ",\n\t",
			   print_protocol(LINK_COMPRESSED_TCP), buffer);
	    } else {
		printf("%s %s %s%s", (count > 1) ? ",\n\t" : "",
			print_protocol(LINK_COMPRESSED_TCP), buffer,
			(count == 0) ? " <-->" : "");
	    }
	    break;
	  case ADDR_DECNET:
	    printf("%s %s %v%s", (count > 1) ? ",\n\t" : "",
			print_protocol(LINK_DECNET),
			swap_16bit_word(addr_entry->addr.decnet_addr),
			(count == 0) ? " <-->" : "");
	    break;
	  default:
	    reg_invoke_sprintf_address_link(
			addr_linktype[addr_entry->addr.type],
			buffer, &addr_entry->addr);
	    printf("%s %s %s%s", (count > 1) ? ",\n\t" : "",
			print_protocol(addr_linktype[addr_entry->addr.type]),
			buffer, (count == 0) ? " <-->" : "");
	    break;
	}
    }

    if (map->map_services & X25_MAPS_CLNS)
	printf("%s %s", (map->map_service_cnt > 1) ? ",\n\t" : "",
		print_protocol(LINK_CLNS));
    if (map->map_services & X25_MAPS_PAD)
	printf(" %s", print_protocol(LINK_PAD));
    if (map->map_services & X25_MAPS_BRIDGE)
	printf(" %s", print_protocol(LINK_BRIDGE));
    if (map->map_services & X25_MAPS_CDP)
	printf(" %s", print_protocol(LINK_CDP));
    if (map->map_services & X25_MAPS_QLLC_NATIVE)
	printf(" %s", print_protocol(LINK_QLLC));

    switch (map->map_status) {
      case X25_MAP_CONSTR:
	printf("\n  CONSTRUCTED");
	break;
      case X25_MAP_PERM:
	printf("\n  PERMANENT");
	break;
      case X25_MAP_TEMP:
	printf("\n  TEMPORARY");
	break;
      case X25_MAP_PVC:
	printf("\n  PVC");
	break;
      default:
	printf("\n  ???");
	break;
    }

    if (map->map_flags & X25_MAPF_BCAST)
	printf(", BROADCAST");
    if (map->map_fac.pin && map->map_fac.pout)
	printf(", PACKETSIZE %d %d",
	    1 << map->map_fac.pin, 1 << map->map_fac.pout);
    if (map->map_fac.win && map->map_fac.wout)
	printf(", WINDOWSIZE %d %d",
		map->map_fac.win, map->map_fac.wout);
    if (map->map_flags & X25_MAPF_PASSIVE)
	printf(", PASSIVE");
    if ((map->map_cud_pid_method != X25_CUD_PID_DEFAULT) &&
	(map->map_cud_pid_method != map->map_idb->hwptr->x25_cud_pid_def))
	printf(", METHOD %s", x25_cud_pid_method_str(map->map_cud_pid_method));
    if (map->map_fac.rev)
	printf(", REVERSE");
    if (map->map_flags & X25_MAPF_REV_OK)
	printf(", ACCEPT-REVERSE");
    if (map->map_fac.cug)
	printf(", CUG %d", map->map_fac.cug);
    if ((map->map_maxnvc != map->map_idb->hwptr->x25_nvc) &&
	(map->map_services & X25_MAPS_ENCAPS) &&
	(map->map_services & ~X25_MAPE_SINGLE_VC))
	printf(", NVC %d", map->map_maxnvc);
    if (map->map_fac.tput_in && map->map_fac.tput_out)
	printf(", THROUGHPUT %d %d",
	       x25_facil_thru(map->map_fac.tput_in),
	       x25_facil_thru(map->map_fac.tput_out));
    if (map->map_fac.nuid_len)
	printf(", %s %s", map->map_fac.cisco_nuid ? "NUID" : "NUDATA",
		x25_nuid(map, FALSE));
    if (map->map_fac.rpoa != NULL)
	printf(", RPOA %s", map->map_fac.rpoa->name);
    if (map->map_fac.tdelay)
	printf(", TRANSIT-DELAY %d", map->map_fac.tdelay);
    if (map->map_flags & X25_MAPF_BAR_IN)
	printf(", NO-INCOMING");
    if (map->map_flags & X25_MAPF_BAR_OUT)
	printf(", NO-OUTGOING");
    if (map->map_idle != X25_MAP_DEFAULT_IDLE)
	printf(", IDLE %d", map->map_idle);
    if (map->map_flags & X25_MAPF_PBP)
	printf(", COMPRESS");

    if (map->map_nvc) {
	printf(", %d VC%s:", map->map_nvc,
	       map->map_nvc > 1 ? "s" : "");
	for (count = 0; count < map->map_nvc; count++)
	    printf("%s %d%s",
		   (count == 0) ? "" : ",", map->map_lci[count]->lci_lci,
		   (map->map_index == count) ? "*" : "");
    }
    if (map->map_thc) {
	printf("\n  TCP Header Compression statistics:");
	reg_invoke_show_compression_stats(LINK_IP, map->map_idb->hwptr,
					  map->map_thc);
    }
}

/*
 *  show_x25_maps
 *  Display the address mapping information
 */

void show_x25_maps (void)
{
    automore_enable(NULL);
    x25_for_all_maps(NULL, show_x25map, NULL);
    automore_disable();
}

/*
 * x25_prx25_addr
 * Output a pair of X.25 addresses
 */

uchar *x25_prx25_addr (register uchar *c)
{
    uchar addr[40], *faddr, *taddr;
    ushort tlen, flen;

    flen = (*c >> 4) & 0xf;
    tlen = *c & 0xf;
    bcdtochar(c + 1, addr, tlen+flen);
    c += (bcdlen_inline(*c) + 1);
    taddr = addr;
    faddr = addr + tlen;
    buginf("\nFrom(%d): %*s To(%d): %*s",
	flen, flen, faddr, tlen, tlen, taddr);
    return(c);
}
