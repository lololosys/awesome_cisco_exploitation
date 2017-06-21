/* $Id: nhrp_vc.c,v 3.6.22.3 1996/07/23 13:31:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp_vc.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_vc.c,v $
 * Revision 3.6.22.3  1996/07/23  13:31:50  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.22.2  1996/04/25  23:24:30  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.22.1.2.1  1996/03/24  04:05:52  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.22.1  1996/03/18  22:48:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  11:07:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:52:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/24  03:05:21  bcole
 * CSCdi47377:  NHRP with ATM ARP server crashes
 *
 * Protect against passing null pointer to radix code, check for lack of
 * NHRP configuration during callback processing.
 *
 * Revision 3.5  1996/01/16  19:37:05  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Move problem registry services to new registry.
 *
 * Revision 3.4  1996/01/11  20:47:47  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Temporarily backout use of private ATM registry...
 *
 * Revision 3.3  1996/01/11  07:06:34  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Glean NHRP information from ATM ARP server.
 *
 * Revision 3.2  1996/01/03  23:32:51  bcole
 * CSCdi46376:  NHRP: Finish post branch split file moves
 *
 * Revision 3.1  1995/11/09  13:50:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/06  21:55:03  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "../util/radix.h"
#include "nhrp_debug.h"
#include "nhrp.h"
#include "nhrp_public.h"
#include "nhrp_inlines.h"
#include "nhrp_vc.h"
#include "../if/static_map.h"
#include "../if/atm.h"
#include "../if/arp.h"
#include "../xns/novell.h"
#include "../ip/ip_registry.h"

/*
 * nhrp_maplist
 * Fetch the static map list which NHRP uses.
 */

static inline maplist_type *nhrp_maplist (idbtype *swidb)
{
    mapgroup_type *mapgrp;
    maplist_type *list_ptr;
    uchar mapname[NHRP_MAPNAME_LEN];

    sprintf(mapname, "%s-%s", NHRP_MAPNAME, swidb->namestring);
    list_ptr = maplist_lookup(mapname);
    /*
     * Does our invisible map already exist?  If not, create it.
     */
    if (list_ptr == NULL) {
	mapgrp = malloc(sizeof(mapgroup_type));
	if (mapgrp == NULL) {
	    return(NULL);
	}
	list_ptr = maplist_create(mapname, FALSE);
	if (list_ptr == NULL) {
	    free(mapgrp);
	    return(NULL);
	}
	enqueue(maplistQ, list_ptr);
	list_ptr->ref_count++;
	mapgrp->next = NULL;
	mapgrp->maplist = list_ptr;
	mapgroup_add(swidb, mapgrp);
    }
    return(list_ptr);
}

/*
 * nhrp_get_map
 * Lookup an entry in the VC static map table entry.  Verify that the
 * entry is of the proper address type.  Currently assumes ATM.
 */

static staticmap_type *nhrp_get_map (idbtype *swidb, nhrpaddr *dest,
			      ushort addr_type, nhrpfuns *fun)
{
    addrtype addr;
    hwaddrtype *hwconf;
    staticmap_type *map;
    maplist_type *list_ptr;

    list_ptr = nhrp_maplist(swidb);
    if (list_ptr == NULL)
	return(NULL);
    memset(&addr, 0, sizeof(addrtype));
    addr.type = fun->addrtype;
    addr.length = fun->nhrp_addlen;
    bcopy(dest, &addr.ip_addr, fun->nhrp_addlen);
    map = staticmap_lookup_list(list_ptr, &addr, fun->linktype, ET_ATM);
    if (!map)
	return(NULL);
    hwconf = &map->hwconf;
    /*
     * Sanity check the address type
     */
    switch (hwconf->type) {
    case STATION_ATMNSAP:
	if (addr_type != AF_NSAP)
	    return(NULL);
	break;
    case STATION_SMDS64:
	if (addr_type != AF_E164)
	    return(NULL);
	break;
    default:
	return(NULL);
	break;
    }
    return(map);
}

/*
 * nhrp_get_atmarp
 * Look in the ATM ARP server table for an IP<->NSAP address mapping.
 */

static hwaddrtype *nhrp_get_atmarp (idbtype *swidb, ushort addr_type, nhrpaddr *dest,
			     nhrpfuns *fun)
{
    hwaddrtype *hwconf;
    
    /*
     * ATM arp only works for IP<->NSAP mappings
     */
    if (fun->linktype != LINK_IP)
	return(NULL);
    if (addr_type != AF_NSAP)
	return(NULL);
    hwconf = reg_invoke_ip_atm_mapping_find(swidb, dest->ip);
    if (!hwconf)
	return(NULL);
    if (hwconf->type != STATION_ATMNSAP)
	return(NULL);
    return(hwconf);
}

/*
 * nhrp_proto_resolve
 * Look in protocol specific static mapping table, and attempt to resolve
 * the network layer destination address.
 * Currently we look in:
 *   ATM arp table
 *   ATM static map table entries are examined.
 */

uchar *
nhrp_proto_resolve (idbtype *if_in, ushort addr_type, nhrpaddr *dest,
		    uchar *nbma_len, nhrpfuns *fun)
{
    hwaddrtype *hwconf;

    hwconf = nhrp_get_atmarp(if_in, addr_type, dest, fun);
    if (hwconf) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Resolved ", dest, fun);
	    buginf(" via ATM arp, if %s", if_in->namestring);
	}
    } else {
	return(NULL);
    }
    *nbma_len = hwconf->length * NHRP_BYTESIZE;
    return(hwconf->addr);
}

/*
 * nhrp_params_init
 * Initialize static map parameters, currently ATM specific.
 */

static void nhrp_params_init (atm_params_type *params)
{
    params->broadcast = FALSE;
    params->aal5mux = FALSE;
    params->fwpkrate0 = ATM_FWPKRATE0;
    params->bkpkrate0 = ATM_BKPKRATE0;
    params->fwpkrate1 = ATM_FWPKRATE1;
    params->bkpkrate1 = ATM_BKPKRATE1;
    params->fwstrate0 = ATM_FWSTRATE0;
    params->bkstrate0 = ATM_BKSTRATE0;
    params->fwstrate1 = ATM_FWSTRATE1;
    params->bkstrate1 = ATM_BKSTRATE1;
    params->fwbsize0 = ATM_FWBSIZE0;
    params->bkbsize0 = ATM_BKBSIZE0;
    params->fwbsize1 = ATM_FWBSIZE1;
    params->bkbsize1 = ATM_BKBSIZE1;
    params->trafmgmt = ATM_TRAFMGMT;
    params->layer2id = ATM_BLLIE;
}

/*
 * nhrp_add_map
 *
 * Add a dynamic static_map entry and open a SVC.
 */
void *nhrp_add_map (idbtype *swidb, addrtype *proto_addr, uchar const *daddr,
		    nhrpfuns *fun)
{
    staticmap_type *map;
    hwaddrtype hwconf, *hwconfp;
    ushort addr_type;
    uchar hwtype;
    uchar addr_len;
    maplist_type *list_ptr;
    uint protocol;
    atm_params_type *params;

    /*
     * If directly connected and a VC exists already, then no need to add
     * a map.
     */
    if (nhrp_vc_exists(swidb, (nhrpaddr *)&proto_addr->addr, fun))
	return(NULL);

    map = staticmap_lookup(swidb, proto_addr, fun->linktype, ET_ATM, FALSE);

    if (!map) {
	hwconfp = &hwconf;
	memset(hwconfp, 0, sizeof(hwaddrtype));
	addr_type = nhrp_nbma_type(swidb);
	switch (addr_type) {
	  case AF_NSAP:
	    hwtype = STATION_ATMNSAP;
	    break;
	  case AF_E164:
	    hwtype = STATION_SMDS64;
	    break;
	  default:
	    return(NULL);
	}
	hwconfp->type = hwtype;
	addr_len = nhrp_af_len(addr_type);
	bcopy(daddr, hwconfp->addr, addr_len);
	hwconfp->length = addr_len;

	list_ptr = nhrp_maplist(swidb);
	if (!list_ptr)
	    return(NULL);
	protocol = fun->linktype;

	/*
	 * Fill in info for dynamic "static_map" cache entry
	 */
	params = malloc(sizeof(atm_params_type));
	if (!params)
	    return(NULL);

	nhrp_params_init(params);
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Adding entry for ",
			 (nhrpaddr *)&proto_addr->addr, fun);
	    buginf(" via %s", swidb->namestring);
	}
	map = staticmap_enter(list_ptr, proto_addr, hwconfp, protocol,
			      ET_ATM, MAP_DYNAMIC, NULL, params);
	if (!map) {
	    if (nhrp_debug)
		buginf("\nNHRP: Cannot add static map entry");
	    free(params);
	    return(NULL);
	}
     }

    /*
     * Setup SVC, if SVC setup already then invalidate route cache.
     */
    if (!reg_invoke_ip_atm_map_request_vc(swidb, map)) {
	(*fun->nhrp_checkcache)((nhrpaddr *)&map->addr.addr);
	return((void *)map);
    }
    return(NULL);
}

/*
 * nhrp_delete_map
 * Delete a particular map table entry.
 */

void nhrp_delete_map (maplist_type *list_ptr, staticmap_type *map,
		      idbtype *swidb, nhrpfuns *fun)
{
    nhrpaddr *dest;
    
    if (nhrp_debug) {
	dest = (nhrpaddr *)&map->addr.addr;
	nhrp_bugaddr("\nNHRP: Removing map table entry for ", dest, fun);
	buginf(" via %s", swidb->namestring);
    }
    staticmap_delete(list_ptr, map);
}

/*
 * nhrp_macaddr_unmap_prefix
 * Perform prefix invalidation.
 */

void nhrp_macaddr_unmap_prefix (maplist_type *list, idbtype *swidb,
				nhrpaddr *dest, ushort masklen,
				ushort addr_type, nhrpfuns *fun)
{
    addrtype addr;
    staticmap_type *map;

    memset(&addr, 0, sizeof(addrtype));
    addr.type = fun->addrtype;
    addr.length = fun->nhrp_addlen;
    bcopy(dest, &addr.ip_addr, fun->nhrp_addlen);
    for (;;) {
	map = staticmap_lookup_prefix(list, &addr, fun->linktype,
				      ET_ATM, masklen);
	if (!map)
	    return;
	nhrp_delete_map(list, map, swidb, fun);
    }
}

/*
 * nhrp_macaddr_unmap
 * Delete a map table entry.  Currently assumes ATM "static" map table entries.
 */

void nhrp_macaddr_unmap (idbtype *swidb, nhrpaddr *dest, uchar *maskkeyp,
			 nhrpfuns *fun)
{
    maplist_type *list_ptr;
    staticmap_type *map;
    ushort addr_type;
    ipaddrtype *mask;
    ushort maskbits;

    list_ptr = nhrp_maplist(swidb);
    if (list_ptr == NULL)
	return;
    addr_type = nhrp_nbma_type(swidb);
    if (maskkeyp) {
	/*
	 * Cache entry has a mask associated with it.  If this entry
	 * specifies a range, then do a range invalidation.
	 */
	mask = (ipaddrtype *)(++maskkeyp);
	if (*mask != MASK_HOST) {
	    maskbits = ip_bitsinmask(*mask);
	    if (nhrp_debug) {
		nhrp_bugaddr("\nNHRP: Invalidating map tables for prefix ",
			     dest, fun);
		buginf("/%d via %s", maskbits, swidb->namestring);
	    }
	    nhrp_macaddr_unmap_prefix(list_ptr, swidb, dest, maskbits,
				      addr_type, fun);
	    return;
	}
    }
    map = nhrp_get_map(swidb, dest, addr_type, fun);
    if (map == NULL)
	return;
    nhrp_delete_map(list_ptr, map, swidb, fun);
}

/*
 * nhrp_vc_exists
 *
 * Check if there is a VC to the destination already.
 * If yes, we then don't have to create a SVC.
 */
boolean nhrp_vc_exists (idbtype *swidb, nhrpaddr *addr, nhrpfuns *fun)
{
    arptype *arp;

    arp = arp_table_lookup(fun->linktype, addr->opaque, swidb);
    if (arp && arp->mode != ARP_INCOMPLETE) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: A VC exists for destination ", addr, fun);
	}
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nhrp_nhop_differ
 * Determine the ultimate network layer destination for the packet.
 * Compare the ultimate destination against the network layer address
 * we were requested to resolve.
 */

static boolean nhrp_nhop_differ (paktype *pak, nhrpfuns *fun,
				 addrtype *proto_addr, addrtype *proto_addr2)
{
    iphdrtype *ip;
    ipxhdrtype *ipx;
    nhrpaddr *dest;

    if (pak->linktype == LINK_NHRP)
	return(FALSE);
    switch (fun->linktype) {
    case LINK_IP:
    default:
	ip = iphdrtype_start(pak);
	dest = nhrp_addr(&ip->dstadr);
	if (dest->opaque == proto_addr->ip_addr)
	    return(FALSE);
	memset(proto_addr2, 0, sizeof(addrtype));
	bcopy(dest, &proto_addr2->ip_addr, fun->nhrp_addlen);
	break;
    case LINK_NOVELL:
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	if ((proto_addr->nov_addr.net == GET2WD(ipx->ddnet)) &&
	    ieee_equal(proto_addr->nov_addr.host, ipx->dhost))
	    return(FALSE);
	memset(proto_addr2, 0, sizeof(addrtype));
	ieee_copy(ipx->dhost, proto_addr2->nov_addr.host);
	proto_addr2->nov_addr.net = GET2WD(ipx->ddnet);
	break;
    }
    proto_addr2->type = fun->addrtype;
    proto_addr2->length = fun->nhrp_addlen;
    return(TRUE);
}

/*
 * nhrp_macaddr_map
 * Attempt to resolve nhop's NBMA address.  If resolved, generate a map
 * table entry for the link layer address.  Currently assumes ATM "static"
 * map table entries.
 */

boolean nhrp_macaddr_map (paktype *pak, addrtype *proto_addr, boolean bcast,
			  staticmap_type **map)
{
    uchar const *daddr;
    ushort addr_type;
    idbtype *if_out;
    uchar hwtype;
    nhrpaddr *dest;
    addrtype proto_addr2;
    staticmap_type *map2;
    boolean nhop_differ;
    ushort protoidx;
    nhrpfuns *fun;
    
    if (bcast)
	return(FALSE);
    dest = NULL;
    protoidx = nhrp_link2idx(pak->linktype);
    fun = nhrp_funmap(protoidx);
    if_out = pak->if_output;
    addr_type = nhrp_nbma_type(if_out);
    switch (addr_type) {
    case AF_NSAP:
	hwtype = STATION_ATMNSAP;
	break;
    case AF_E164:
	hwtype = STATION_SMDS64;
	break;
    default:
	return(FALSE);
	break;
    }

    nhop_differ = nhrp_nhop_differ(pak, fun, proto_addr, &proto_addr2);
    if (nhop_differ) {
	/*
	 * Attempt address resolution on ultimate destination.
	 */
	map2 = staticmap_lookup(if_out, &proto_addr2, fun->linktype,
				ET_ATM, FALSE);
	if (map2) {
	    /*
	     * Do we not yet have a VC for this map table entry?
	     * If we do not, then don't return this map table entry as
	     * the result.  However, still attempt ATM signalling so that
	     * this map table entry may subsequently get a VC.
	     */
	    if (map2->map_type == MAP_DYNAMIC &&
		reg_invoke_ip_atm_map_request_vc(if_out, map2)) {
		if (nhrp_debug) {
		    buginf("\nNHRP: No vc for %i", proto_addr2.ip_addr);
		}
	    } else {
		*map = map2;
		/*
		 * Prefer map table entries which are truly static (explicitly
		 * configured).
		 */
	    }
	} else if ((*fun->nhrp_lookup)(pak, (void *)&proto_addr2.ip_addr,
				       &daddr)) {
	    /*
	     * Prefer this dynamic info regarding the final destination over
	     * static info regarding the next hop.
	     */
	    map2 = nhrp_add_map(if_out, &proto_addr2, daddr, fun);
	    if (map2) {
		*map = map2;
	    }
	    return(*map != NULL);
	}
    }

    /*
     * Use existing map table entry, even if it is dynamic (and perhaps
     * no longer matching the NHRP cache table entry.)
     * Revising of existing map table entries should happen when the
     * cache entry is revised.  If the entry already has a VC associated
     * with it, this could be disruptive.
     */
    if (*map)
	return(TRUE);

    if ((*fun->nhrp_lookup)(pak, (void *)&proto_addr->ip_addr, &daddr)) {
	*map = nhrp_add_map(if_out, proto_addr, daddr, fun);
    }
    return(*map != NULL);
}

/*
 * nhrp_atmarp_invalidate
 * Let ATM arp trigger an NHRP cache invalidation.
 */

static void nhrp_atmarp_invalidate (idbtype *swidb, ipaddrtype dest,
			     hwaddrtype *hwaddr)
{
    nhrpfuns *fun;
    nhrpcachetype *ptr;

    /*
     * Check if NHRP is enabled on the interface.
     */
    if (!swidb || !swidb->nhrp_info ||
	(swidb->nhrp_info->pro[NHRP_NETIP].nhrp_net_id == NHRP_NETID_UNDEF))
	return;

    fun = nhrp_funmap(NHRP_NETIP);
    if (!fun)
	return;
    ptr = nhrp_cache_lookup(swidb, nhrp_addr(&dest), fun);
    if (!ptr)
	return;
    if (nhrp_debug)
	buginf("\nNHRP: Invalidating %i via ATM arp", dest);
    nhrp_cache_delete_subr(ptr);
}

/*
 * nhrp_atm_staticmap_connect
 *
 * Invalidate routing cache when SVC comes up in order to
 * populate our NHRP cache info.
 */
static void nhrp_atm_staticmap_connect (staticmap_type *map_ptr)
{
    nhrpfuns *fun;
    idbtype *swidb;
    ushort idx;

    /*
     * Is the protocol supported over NHRP ?
     */
    if (map_ptr->protocol != LINK_IP && map_ptr->protocol != LINK_NOVELL)
	return;

    /*
     * Is NHRP configured on the interface ?
     */
    swidb = map_ptr->sm_swidb;
    if (!swidb || !swidb->nhrp_info)
	return;
    idx = nhrp_link2idx(map_ptr->protocol);
    if (swidb->nhrp_info->pro[idx].nhrp_net_id == NHRP_NETID_UNDEF)
	return;

    fun = nhrp_funmap(idx);
    if (!fun) return;

    if (nhrp_debug) {
	nhrp_bugaddr("\nNHRP: SVC up for map table entry ",
		     (nhrpaddr *)&map_ptr->addr.addr, fun);
	buginf(" via %s", map_ptr->sm_swidb->namestring);
    }

    (*fun->nhrp_checkcache)((nhrpaddr *)&map_ptr->addr.addr);
}

/*
 * Initialize our NHRP VC support
 */

void nhrp_vc_init (void)
{
    reg_add_ip_atm_mapping_invalid(nhrp_atmarp_invalidate,
				   "nhrp_atmarp_invalidate");
    reg_add_ip_atm_staticmap_connect(nhrp_atm_staticmap_connect,
				     "nhrp_atm_staticmap_connect");
}
