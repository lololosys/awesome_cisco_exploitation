/* $Id: esis.c,v 3.7.26.5 1996/08/08 14:50:36 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/esis.c,v $
 *------------------------------------------------------------------
 * esis.c -- ISO 9542 ES-IS Routing Protocol 
 *  
 * Feb-8-1989 by Jim Forster
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: esis.c,v $
 * Revision 3.7.26.5  1996/08/08  14:50:36  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.7.26.4  1996/08/04  21:34:15  dkatz
 * CSCdi64354:  ESIS bonus ISHs quashed
 * Branch: California_branch
 * Remove arbitrarily complex logic around the sending of bonus ISHs;
 * rather, always send a rate-limited unicast ISH whenever a new
 * adjacency (ES or IS) is detected.  Send an ISH when the line
 * protocol comes up as well.
 *
 * Revision 3.7.26.3  1996/07/11  18:29:24  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.7.26.2  1996/05/04  20:04:11  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.7.26.1  1996/03/18  19:03:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.2  1996/03/13  01:13:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.1  1996/03/07  08:40:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/01/18  21:48:12  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/22  21:58:07  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.5  1995/12/01  17:01:36  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.4  1995/11/21  23:56:48  raj
 * CSCdi39109:  CLNS generates noise when interface is down
 * Added interface_up calls in various needed places and changed one
 * errmsg to a buginf instead.
 *
 * Revision 3.3  1995/11/17  08:53:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/23  18:02:57  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.4  1995/08/23  17:22:15  dkatz
 * CSCdi37612:  Received ESH flaps ISIS adjacency
 * Ignore received ESH if an ISIS adjacency is present.  Don't
 * syslog encaps failures.
 *
 * Revision 2.3  1995/06/18  06:18:09  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 12:59:02  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:18:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "address.h"
#include "logger.h"
#include "ieee.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "config.h"
#include "../decnet/dnconversion.h"
#include "../clns/clns.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"

/*
 * Local Definitions
 */
#define CHECK_DUP(opt)	if (pak->clns_flags & opt) {		\
			    opt_ok = FALSE;			\
			    break;				\
		        } else {				\
		            pak->clns_flags |= opt;		\
			}					\

/*
 * esis_ager
 * Age out entries from the adjacency database. Also, age out entries
 * from the CLNS cache.
 */
void esis_ager (void)
{

    clns_cachetype *cache_entry, *next_cache;
    int i;
    boolean can_purge_cache;

    /*
     * Age the adjacencies. This is done here because it needs to happen
     * even if we are not running a routing protocol
     */
    adjacency_ager(clns_adjacency_db);

    /*
     * Age the CLNS prefix routing table.
     */
    clns_ager();

    /* 
     * Now age the routing cache. This needs to be here because we need
     * to do it even when there are no igrp processes running.
     */
    for (i = 0; i < CLNS_CACHE_SIZE; i++){
	for (cache_entry = clns_cache[i]; cache_entry; 
	     cache_entry = next_cache) {
	    next_cache = cache_entry->next;
	    can_purge_cache = clns_perform_cache_flush() && 
		      (cache_entry->cache_version != clns_cache_version);
	    if (TIMER_RUNNING_AND_AWAKE(cache_entry->cache_expiration) ||
		      can_purge_cache) { 
		clns_rbuginf("\nCLNS-RT: Aging cache entry for: %n",
			     cache_entry->destination);
		nsap_cache_delete(cache_entry->destination);
	    }
	}
    }
}

/*
 * send_bonus_hello
 *
 * Send a bonus Hello packet in response to a newly established adjacency.
 * We delay the transmission for one second (giving us rate control) and
 * send the packet as a unicast.  If there was already a unicast packet
 * pending (meaning that somebody else is receiving the same treatment)
 * we send the packet as a multicast.
 */
static void send_bonus_hello (idbtype *idb, clns_adjtype *adj)
{

    sys_timestamp next_hello;
    if (!idb)
	return;

    TIMER_START(next_hello, ONESEC);
    mgd_timer_set_soonest(&idb->clns_hello_timer, next_hello);
    
    if (idb->clns_hello_addr || !adj) {	/* Already a unicast pending */
	idb->clns_hello_addr = NULL;
    } else {
	idb->clns_hello_addr = adj->snpa.addr;
	idb->clns_hello_type = adj->snpa.type;
	idb->clns_hello_addrlen = adj->snpa.length;
    }
}

/*
 * esis_route_adjust
 *
 * Called when the interface changes state.
 *
 * We want to send a bonus ISH when the interface comes up.
 */
void esis_route_adjust (idbtype *idb)
{
    if (interface_up(idb)) {
	send_bonus_hello(idb, NULL);
    }
}

/*
 * ish_input
 * Process an ISH packet
 */
void ish_input (paktype *pak)
{
    clnshdrtype *clns;
    NSAP_T net;	    /* current source address */
    ulong holdingtime;
    hwaddrtype snpa;
    ushort len, netl;
    uchar *ptr;
    int i;
    idbtype *idb;
    clns_adjtype *adjacency;
    clns_pdbtype *pdb;
    NSAP_T key;
    uchar *station_id;

    clns = (clnshdrtype *)clnsheadstart(pak);

    pdb = find_isis_pdb();

    idb = pak->if_input;
    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);
    holdingtime = GETSHORT(clns->esis_holding_time);

    esis_pbuginf("\nES-IS: ISH from %s (%s), HT %d", snpa_string(&snpa),
		 pak->if_input->namestring, holdingtime);

    len = CLNS_FIXEDHDR_SIZE;
    ptr = PAK2ADDR(pak);
    if (clns->hlen < ESIS_MINHELLOHEADERBYTES) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf("\nES-IS: Bad header length %d/%d", 
		     ESIS_MINHELLOHEADERBYTES, clns->hlen);
	return;
    }
    nsap_copy(ptr, net);
    netl = *ptr++;
    len += netl + 1;
    ptr += netl;
    if ((netl < CLNS_MIN_NSAP_LENGTH) || (netl >= CLNS_MAX_NSAP_LENGTH) ||
	len > clns->hlen) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf("\nES-IS: Bad NETL %d", netl);
	return;
    }

    /*
     * Validate AFI in advertised NET.
     */
    if (!AFI_OK(net)) {
	esis_pbuginf("\nES-IS: Bad AFI for %n", net);
	return;
    }

    if (pak->if_input->clns_dec_compatible) {
	net[0] += 1;
	net[net[0]] = 0;
    }

    /*
     * Filter the adjacency.
     */
    if (idb->clns_is_adj_filter) {
	if (!clns_filter_addr(idb->clns_is_adj_filter, (nsap_addr *) net,
			      NULL))
	    return;	/* Didn't pass. */
    }

    /*
     * Determine if the NSAP advertised is one of ours. If so, do not
     * build an adjacency. This is done by doing a route table lookup.
     * This will accomadate all types of routing (static, IS-IS, ISO-IGRP).
     */
    if (((int) clns_route(net, ROUTINGDEPTH)) == -1)
	return;

    /*
     *  ??? For now, this code will do the logic for serial line ISHes
     *  I will need to figure out the right place to put the
     *  logic for broadcast media and multi-cast media ISHes
     */
    key[0] = STATION_LENGTH;
    station_id = get_station(net);
    station_copy(station_id, &key[1]);

    adjacency = find_adjacency(clns_adjacency_db, key, FALSE, idb, 0);
    if (!adjacency) {
	adjacency = add_adjacency(clns_adjacency_db, key, idb, &snpa,
				  holdingtime, idb->clns_cluster_alias);
	if (!adjacency)
	    return;

	adjacency->protocol_source |= ISO_ESIS;
	adjacency->state = CLNS_ADJ_UP_STATE;
	adjacency->adjacencyType = CLNS_IS_ADJ_TYPE;
	adjacency->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;

	/*
	 * Fill in area addresses.
	 */
	i = net[0];
	if (i > (STATION_LENGTH+1)) {
	    net[0] -= 7;
	    nsap_copy(net, adjacency->area_address[0]);
	    adjacency->num_areas = 1;
	    net[0] = i;
	}

	if (pdb) {

	    /*
	     * If this interface is running isis, send out an IIH if 
	     * point-to-point link.
	     */
	    if (itf_in_pdb(idb, pdb)) {
		if (is_p2p(idb)) {
		    isis_send_iih(pdb, idb, ISIS_CIRCUIT_L1);
		} else {
		    isis_new_es_adjacency(pdb, adjacency);
		}
	    }

	    /*
	     * There might be a static route pointing to this new adjacency.
	     */
	    clns_adjust_static_routes(adjacency, adjacency->idb, TRUE, TRUE);
	}

	/* Send a bonus ISH to welcome our new neighbor. */

	send_bonus_hello(idb, adjacency);

    } else {

	/*
	 * If the SNPA doesn't match, drop the hello, since we might be
	 * seeing two hellos from a multihomed source with the LANs
	 * bridged together (or a rogue with the same system ID!).
	 * If the system's SNPA has changed, we'll time out this adjacency
	 * and then pick up a new one with the updated SNPA.
	 */

	if (!snpa_equ(&snpa, &adjacency->snpa)) {
	    esis_pbuginf(
    "\nES-IS: new SNPA %s doesn't match old SNPA %s--possible bridged LANs",
			 snpa_string(&snpa), snpa_string(&adjacency->snpa));
	    return;
	}

	/*
	 * Handle the case where an ES adjacency just changed into an
	 * IS and started sending ISHes.
	 */
	if (adjacency->adjacencyType == CLNS_ES_ADJ_TYPE) {
	    adjacency->adjacencyType = CLNS_IS_ADJ_TYPE;
	    adjacency->state = CLNS_ADJ_UP_STATE;
	}

	/*
	 * Since we didn't call 'add_adjacency', we need to
	 * refresh the adjacency, but only if ES-IS is the only
	 * protocol source (let IGRP/IS-IS do their own refreshes).
	 */
	if ((adjacency->protocol_source & ISO_ESIS) && 
	    !(adjacency->protocol_source & (ISO_IGRPMASK | ISO_ISISMASK))) {
	    TIMER_START(adjacency->expiration_time, holdingtime * ONESEC);
	}
    }

    /* 
     * If we are an end-system, check if the ESCT option is present
     * and use it for the ESH hello timer if less than existing one.
     * Mark this IS as the default IS.
     */
    if (!clns_router_running) {
	clns_mark_default_is_adj(adjacency);
	while (len < clns->hlen) {
	    if (*ptr++ == ESIS_ESCT_CODE) {
		i = *((ushort *) (ptr + 1));
		clns_esct_timer = (!clns_esct_timer) ? i : 
		    min(clns_esct_timer, i);
		break;
	    }
	    len += *ptr + 2;
	    ptr += *ptr + 1;
	}
    }

    clns_traffic[ESIS_ISHSRCVD]++;
}

/*
 * esh_input
 * Process an ESH packet
 */
void esh_input(paktype *pak)
{
    clnshdrtype  *clns;
    uchar        *ptr;		/* pointer into packet header */
    idbtype      *idb;
    clns_adjtype *adjacency;
    clns_pdbtype *pdb, *isis_pdb;
    uchar        *sa;		/* current source address */
    ushort       num_sa;	/* number of source addresses */
    ushort       sal;		/* length of current source address */
    ulong        holdingtime;
    hwaddrtype   pak_src_snpa;
    ushort       len;
    NSAP_T       key;
    uchar        *station_id;
    int          i;

    clns = (clnshdrtype *)clnsheadstart(pak);

    /*
     * Get SNPA, interface, and holding time.
     */
    idb = pak->if_input;
    (*idb->hwptr->extract_snpa)(pak, &pak_src_snpa, SOURCE);
    holdingtime = GETSHORT(clns->esis_holding_time);

    clns_traffic[ESIS_ESHSRCVD]++;
    if (esis_event_debug || esis_packet_debug) {
	esis_pbuginf("\nES-IS: ESH from %s (%s), HT %d", 
		     snpa_string(&pak_src_snpa), pak->if_input->namestring,
		     holdingtime);
    }

    /*
     * Validate length of PDU.
     */
    len = CLNS_FIXEDHDR_SIZE;
    ptr = PAK2ADDR(pak);
    if (len >= clns->hlen) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf("\nES-IS: Bad header length %d", clns->hlen);
	return;
    }

    num_sa = *ptr;
    len = CLNS_FIXEDHDR_SIZE + 1;
    if (len >= clns->hlen || !num_sa) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf("\nES-IS: Bad header length %d", clns->hlen);
	return;
    }

    /*
     * This loop validates all NSAP addresses in the ESH.
     */
    sa = ptr;
    ptr++;
    while (num_sa--) {
	len += *ptr + 1;
	if (!NSAP_LENGTH_CHECK(ptr) || len > clns->hlen) {
	    clns_traffic[CLNS_HDRSYNTAX]++;
	    esis_pbuginf("\nES-IS: Bad SAL %d", *ptr);
	    return;
	}
	if (!AFI_OK(ptr)) {
	    clns_traffic[CLNS_HDRSYNTAX]++;
	    esis_pbuginf("\nES-IS: Bad AFI for %n", ptr);
	    return;
	}
	ptr += *ptr + 1;
    }

    isis_pdb = find_isis_pdb();

    /*
     * Process each NSAP address in ESH as an ES adjacency.
     */
    ptr = sa;
    num_sa = *ptr++;
    len = CLNS_FIXEDHDR_SIZE + 1;
    while (num_sa--) {
	sa = ptr;
	sal = *ptr++;
	len += sal + 1;
	ptr += sal;
	
	/*
	 * Filter the adjacency.
	 */
	if (idb->clns_es_adj_filter) {
	    if (!clns_filter_addr(idb->clns_es_adj_filter, (nsap_addr *) sa,
				  NULL))
		continue;	/* Didn't pass. */
	}

        /*
         * If doing DECNET Phase IV to V conversion, insert this
         * neighbor in the Phase IV database too.
         */
	reg_invoke_decnet_clns2dn_neighbor(TRUE, sa, idb, holdingtime*ONESEC);

	key[0] = STATION_LENGTH;
	station_id = get_station(sa);
	station_copy(station_id, &key [1]);

	adjacency = find_adjacency(clns_adjacency_db, key, FALSE, idb, 0);
	if (adjacency) {
	    for (; adjacency; adjacency = adjacency->next_alias) {

		/*
		 * If cluster aliasing enabled on this interface, compare
		 * SNPAs to get correct alias adjacency.
		 */
		if (idb->clns_cluster_alias &&
		    compare_stationid((uchar *) &adjacency->snpa.addr, 
				      (uchar *) &pak_src_snpa.addr)) {
		    continue;
		}
		break;
	    }
	}

	if (!adjacency) {

	    /*
	     * Create or update adjacency. Inform IS-IS about change.
	     */
	    adjacency = add_adjacency(clns_adjacency_db, key, idb,
				      &pak_src_snpa, holdingtime,
				      idb->clns_cluster_alias);
	    if (!adjacency)
		return;
		
	    adjacency->protocol_source |= ISO_ESIS;
	    adjacency->state = CLNS_ADJ_UP_STATE;
	    adjacency->adjacencyType = CLNS_ES_ADJ_TYPE;
	    adjacency->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;

	    /*
	     * Add area address to adjacency.
	     */
	    sa [0] -= (STATION_LENGTH + SELECTOR_LENGTH);
	    add_area_to_adj(sa, adjacency);

 	    /* 
	     * New neighbor, send unicast hello.
	     */
 	    send_bonus_hello(pak->if_input, adjacency);

 	    /*
	     * Inform IS-IS to add to pseudo-node LSP, if DR.
	     */
 	    if (isis_pdb)
		isis_new_es_adjacency(isis_pdb, adjacency);

	} else {

	    /*
	     * Don't override IS-IS or ISO-IGRP adjacency types.
	     */
	    if (adjacency->protocol_source & (ISO_IGRPMASK | ISO_ISISMASK)) {
		return;
	    }

	    /* 
	     * There was an existing adjacency. 
	     */
	    if ((adjacency->protocol_source & ISO_ESIS) ||
		(adjacency->protocol_source & DECNET_PHASEIV)) {
		TIMER_START(adjacency->expiration_time, holdingtime * ONESEC);
		snpa_copy(&pak_src_snpa, &adjacency->snpa);
		adjacency->adjacencyType = CLNS_ES_ADJ_TYPE;
		adjacency->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
	    }
	    adjacency->protocol_source |= ISO_ESIS;

	    /*
	     * If DECNET conversion is disabled and the adjacency was 
	     * learned by DECNET, clear that fact.
	     */
	    reg_invoke_decnet_clns2dn_neighbor(FALSE, sa, idb , 0L);

	    /*
	     * Check if an area address is already stored in adjacency, if not,
	     * add it and inform IS-IS.
	     */
	    sa [0] -= (STATION_LENGTH + SELECTOR_LENGTH);
	    for (i = 0; i < adjacency->num_areas; i++) {
		if (nsap_equ(sa, adjacency->area_address [i]))
		    break;
	    }
	    if (i == adjacency->num_areas) {
		if (adjacency->num_areas == MAX_AREA_ADDRESSES)
		    continue;
		add_area_to_adj(sa, adjacency);
		if (isis_pdb)
		    isis_new_es_adjacency(isis_pdb, adjacency);
	    }
	}
	sa [0] += (STATION_LENGTH + SELECTOR_LENGTH);

  	/*
 	 * If this adjacency belongs to an IGRP process - add
  	 * it to the IGRP database, too
  	 */
  	pdb = clns_find_pdb(sa);
  	if (pdb && (pdb->typeflag & ISO_IGRPMASK) &&
 	    (pdb->level & CLNS_LEVEL1)) {
  	    iso_igrp_setup_metric(iso_igrp_destination_update(pdb, 
		      CLNS_STATION_TYPE, key, idb, key, &pak_src_snpa, 
		      iso_igrp_compute_metric(pdb, idb), ISO_ESIS, 
		      CLNS_ROUTE_ENTRY, holdingtime*ONESEC), idb);
  	}
    }
}

/*
 * ra_input
 * Process an RA packet
 */
static void ra_input(paktype *pak)
{
    clnshdrtype *clns;
    nsap_addr *our_net;		/* Our NET */
    clns_local_nettype *local_net;
    hwaddrtype snpa;
    uchar *ptr;
    idbtype *idb;
    clns_pdbtype *pdb;
    int bytesleft;
    ulong type;
    int opt_len;
    uchar *sysid_ptr = NULL;
    int sysid_len = 0;
    paktype *new_pak;
    int pak_hlen;

    clns = (clnshdrtype *)clnsheadstart(pak);

    idb = pak->if_input;
    (*idb->hwptr->extract_snpa)(pak, &snpa, SOURCE);

    esis_pbuginf("\nES-IS: RA from %s (%s)", snpa_string(&snpa),
		 pak->if_input->namestring);
    clns_traffic[ESIS_RASRCVD]++;

    /* Walk the options looking for the SYSID option. */

    ptr = PAK2ADDR(pak);
    bytesleft = clns->hlen - CLNS_FIXEDHDR_SIZE;
    while (bytesleft) {
	if (bytesleft < CLNS_MIN_OPTION_SIZE) {
	    clns_traffic[CLNS_HDRSYNTAX]++;
	    esis_pbuginf("\nES-IS: Bad header length %d", clns->hlen);
	    return;
	}
	type = *ptr;
	opt_len = *(ptr + 1);
	bytesleft -= 2;		/* type and length */
	if (bytesleft < opt_len) {
	    clns_traffic[CLNS_HDRSYNTAX]++;
	    esis_pbuginf("\nES-IS: Bad header length %d", clns->hlen);
	    return;
	}
	switch (type) {
	case ESIS_SYSID_CODE:
	    /*
	     * Suggested system ID.  Right-justify it in the system ID
	     * field.  If it is too big, punt and use the SNPA instead.
	     */
	    if (opt_len <= STATION_LENGTH) {
		if (opt_len < 1) { /* We need *something* */
		    clns_traffic[CLNS_HDRSYNTAX]++;
		    esis_pbuginf("\nES-IS: SYSID length 0");
		    return;
		}
		sysid_ptr = ptr + 2; /* Skip the length field. */
		sysid_len = opt_len;
	    }
	    esis_pbuginf("\nES-IS: Suggested SYSID %s (%s)", 
			 nsap_addr_string((nsap_addr *) (ptr + 1)), 
			 sysid_ptr ? "accepted" : "rejected");
	    break;
	default:
	    esis_pbuginf("\nES-IS: Unrecognized option %x", type);
	    break;
	}
	bytesleft -= opt_len;
	ptr += opt_len + 2;
    }

    /* If we didn't find (or couldn't use) a system ID, use the SNPA. */

    if (!sysid_ptr) {
	sysid_ptr = snpa.addr;
	sysid_len = snpa.length;
    }

    /*
     * Now form the reply.  Start from the interface-specific NET;  if not
     * there, pull the first IS-IS area address;  if that's not there, 
     * use the first local NET.
     */
    our_net = NULL;
    pdb = find_isis_pdb();
    local_net = clns_local_nets.qhead;
    if (idb->clns_net) {
	our_net = NSAP_T_to_nsap_addr(idb->clns_net);
    } else if (pdb && pdb->num_areas) {
	our_net = NSAP_T_to_nsap_addr(&pdb->area_address[0]);
    } else if (local_net) {
	our_net = NSAP_T_to_nsap_addr(&local_net->net);
    }

    if (our_net && interface_up(idb)) {
	nsap_addr *new_net;
	uchar *new_sysid;
	clnshdrtype *new_clns;

	new_pak = clns_getbuffer(SMALLDATA);
	if (!new_pak) return;

	new_pak->flags |= PAK_PRIORITY;

	new_clns = (clnshdrtype *)clnsheadstart(pak);
	
	new_clns->pid = NLP_ESIS;
	new_clns->version = ESIS_VERSION;
	new_clns->lifetime = new_clns->sp = new_clns->ms = new_clns->er = 0;
	new_clns->type = ESIS_TYPE_AA;
	
	PUTSHORT(new_clns->esis_holding_time, ESIS_ADDR_ASSIGN_TIME);
	
	/* Build the new NET directly in the packet. */

	ptr = PAK2ADDR(new_pak);
	new_net = (nsap_addr *) ptr;
	bcopy(our_net, new_net, our_net->length + 1);
	new_sysid = &new_net->addr[new_net->length - SYSTEMID_LENGTH];
	memset(new_sysid, 0, STATION_LENGTH);
	bcopy(sysid_ptr, new_sysid + (STATION_LENGTH - sysid_len), sysid_len);

	ptr += our_net->length + 1;
	new_clns->hlen = pak_hlen = ptr - clnsheadstart(new_pak);

	/*
	 * Compute checksum if configured.
	 */
	if (idb->clns_checksum) {
	    compute_iso_checksum(clnsheadstart(new_pak),
				 pak_hlen,
				 new_clns->checksum);
	}

	/*
	 * Encapsulate and send AA.
	 */
	if (clns_encaps(new_pak, pak_hlen, idb, &snpa, NULL)){
	    datagram_out(new_pak);
	    clns_traffic[CLNS_OUTPUT]++;
	    clns_traffic[ESIS_AASSENT]++;
	    esis_pbuginf("\nES-IS: AA sent to %s (%s): NET %s, HT %d, HLEN %d",
			 snpa_string(&snpa), idb->namestring,
			 nsap_addr_string(new_net), ESIS_ADDR_ASSIGN_TIME, 
			 pak_hlen);
	} else {
	    datagram_done(new_pak);
	    if (!is_x25(idb->hwptr)){
		esis_pbuginf("\nES-IS: %s encapsulation of ISH failed, HLEN %d",
			     idb->namestring, pak_hlen);
	    }
	}
    }
}

/*
 * send_one_is_hello
 *
 * Send out one Intermediate System Hello on this interface.
 */
void send_one_is_hello (
    clns_pdbtype *pdb,
    idbtype      *idb,
    NSAP_T       net,
    hwaddrtype   *ish_target_snpa,
    char         *ish_target_string)

{

    paktype        *pak;
    clnshdrtype    *clns;
    uchar          *ptr;
    isis_info_type *item;
    ushort         pak_hlen;
    int            NET_length;
    clns_pdbtype   *isis_pdb;

    /*
     * If doing partition avoidance, don't send any ISHs.  If we do,
     * it refreshes our old IIHs and the adjacency doesn't go away.
     */
    isis_pdb = find_isis_pdb();
    if (isis_pdb) {
	if (isis_pdb->partition_avoid_ok) {
	    esis_pbuginf("\nES-IS: ISH to %s (%s) suppressed (partition avoidance)", 
			 ish_target_string, idb->namestring);
	    return;
	}
    }

    pak = clns_getbuffer(SMALLDATA);
    if (!pak)
	return;
    pak->flags |= PAK_PRIORITY;

    clns = (clnshdrtype *)clnsheadstart(pak);

    clns->pid = NLP_ESIS;
    clns->version = ESIS_VERSION;
    clns->lifetime = clns->sp = clns->ms = clns->er = 0;
    clns->type = ESIS_TYPE_ISH;

    PUTSHORT(clns->esis_holding_time, clns_holding_timer/ONESEC);

    ptr = PAK2ADDR(pak);
    if (idb->clns_dec_compatible) {
 	NET_length = net[0];
 	net[0] -= 1;
     } else {
 	NET_length = net[0]+1;
    }

    bcopy((uchar *)net, (uchar *)ptr, NET_length);

    if (idb->clns_dec_compatible)
	net[0] += 1;

    ptr += NET_length;
    pak_hlen = clns->hlen = ptr - clnsheadstart(pak);

    /*
     * Insert NLPID in packet for point-to-point links only.
     */
    if (is_p2p(idb)) {
	item = (isis_info_type *) ptr;
	isis_insert_nlpid(pdb, item, MAXINT); /* Assume it will fit */
	if (item->length) {
	    pak_hlen += (item->length + 2);
	    ptr += item->length + 2;
	}
    }

    /*
     * Insert ESCT option if configured.
     */
    if (idb->clns_esct_time) {
	item = (isis_info_type *) ptr;
	item->code = ESIS_ESCT_CODE;
	item->length = 2;
	*((ushort *) item->value) = idb->clns_esct_time;
	pak_hlen += (item->length + 2);
    }

    clns->hlen = pak_hlen;

    /*
     * Compute checksum if configured.
     */
    if (idb->clns_checksum) {
	compute_iso_checksum(clnsheadstart(pak),
			     clns->hlen,
			     clns->checksum);
    }

    /*
     * Encapsulate and send ISH.
     */
    if (clns_encaps(pak, pak_hlen, idb, ish_target_snpa, NULL)){
	datagram_out(pak);
	clns_traffic[CLNS_OUTPUT]++;
	clns_traffic[ESIS_ISHSSENT]++;
	esis_pbuginf("\nES-IS: ISH sent to %s (%s): NET %n, HT %d, HLEN %d",
		     ish_target_string, idb->namestring,
		     net, clns_holding_timer/ONESEC, pak_hlen);
    } else {
	datagram_done(pak);
	if (!is_x25(idb->hwptr)){
	    esis_pbuginf("\nES-IS: %s encapsulation of ISH failed, HLEN %d",
			 idb->namestring, pak_hlen);
	}
    }
}

/*
 * send_is_hellos
 *
 * Send out the IS hellos. End Systems listen for ISHes to find the 
 * Intermediate Systems. An ISH is sent for each NET configured for the
 * system. There is one exception. If there is an interface specific
 * NET, only that one is advertised. This is done for compatibility with
 * AT&T end-system implementations.
 */
void send_is_hellos (
    idbtype    *idb,
    hwaddrtype *target_snpa,
    char       *target_string)
{
    clns_pdbtype       *pdb;
    clns_local_nettype *net;
    NSAP_T             *nsap;

    if (!interface_up(idb))
	return;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (!(pdb->level & CLNS_LEVEL1))
	    continue;
	if (!itf_in_pdb(idb, pdb))
	    continue;
	break;
    }

    /*
     * There is no routing protocol enabled on this interface and
     * CLNS is not enabled. Don't send any ISHes.
     */
    if (!pdb && !idb->clns_enabled)
	return;

    /*
     * Send an ISH for each NET configured for the system.
     */
    for (net = clns_local_nets.qhead; net; net = net->next) {

	/*
	 * Interface NSAP overrides any global NSAPs or router NETs.
	 */
	nsap = (idb->clns_net) ? (NSAP_T *) idb->clns_net : &net->net;
	send_one_is_hello(pdb, idb, *nsap, target_snpa, target_string);
	if (idb->clns_net)
	    return;
    }
}

/*
 * send_one_es_hello
 *
 *    Send an es hello out this interface if appropriate.
 *    This routine works for both Intermediate Systems and End Systems.
 *    Note that each esh will contain ALL of the nsaps configured on
 *    this node
 *    Note also that this implies that all NSAPs are stored in the
 *    non-conforming database. For some, this is redundant but it
 *    should do no harm.
 */
static void send_one_es_hello (
    idbtype    *idb,
    hwaddrtype *esh_target_snpa,
    char       *esh_target_string)
{
    paktype      *pak;
    clnshdrtype  *clns;
    clns_ddbtype *ddb;
    uchar        *ptr;
    ushort       nnsaps;
    ushort       pak_hlen;
    clns_pdbtype *isis_pdb;

    if (!interface_up(idb))
	return;

    /*
     * If doing partition avoidance, don't send any ESHs.  If we do,
     * it refreshes our old IIHs and the adjacency doesn't go away.
     */
    isis_pdb = find_isis_pdb();
    if (isis_pdb) {
 	if (isis_pdb->partition_avoid_ok) {
 	    esis_pbuginf("\nES-IS: ESH to %s (%s) suppressed (partition avoidance)", 
 			 esh_target_string, idb->namestring);
 	    return;
 	}
    }
 
    /*
     * Send out ESH's with our nsaps.  ESH's go to all IS's.
     */
    nnsaps = 0;
    pak = NULL;
    clns = NULL;
    ptr = NULL;
    for (ddb = nsap_next(NULL); ddb; ddb = nsap_next(ddb)) {
	if ((ddb->entry_type == CLNS_OURNSAP_ENTRY) ||
	    (!clns_router_running && (ddb->entry_type == CLNS_OURNET_ENTRY))) {
	    if (pak) {
		/*
		 * if there's no room for this nsap send what we've got
		 * so far.
		 */
		pak_hlen = (ptr - clnsheadstart(pak)) + ddb->destination[0] + 1;
		if (pak_hlen > CLNS_MAXHEADERBYTES ) {
		    pak_hlen = clns->hlen = ptr - clnsheadstart(pak);
		    ptr = PAK2ADDR(pak);
		    *ptr = (uchar) nnsaps;

		    /*
		     * Compute checksum if configured.
		     */
		    if (idb->clns_checksum) {
			compute_iso_checksum(clnsheadstart(pak),
					     clns->hlen, 
					     clns->checksum);
		    }

		    if (clns_encaps(pak, pak_hlen, idb,
				    esh_target_snpa, NULL)) {
			datagram_out(pak);
			clns_traffic[CLNS_OUTPUT]++;
			clns_traffic[ESIS_ESHSSENT]++;
		    } else {
			datagram_done(pak);
			if (!is_x25(idb->hwptr)) {
			    esis_pbuginf("\nESIS: ESH encaps failed on %s",
					 idb->namestring);
			}
		    }
		    pak = NULL;
		    ptr = NULL;
		    nnsaps = 0;
		}
	    }
	    if (!pak) {
		/*
		 * if we don't have a packet, get one, and init most of the
		 * header.  (We know at least one nsap will fit in a fresh
		 * packet).
		 */
		pak = clns_getbuffer(MEDDATA);
		if (!pak) {
		    return;
		}
		
		clns = (clnshdrtype *)clnsheadstart(pak);

		clns->pid = NLP_ESIS;
		clns->version = ESIS_VERSION;
		clns->lifetime = clns->sp = clns->ms = clns->er = 0;
		clns->type = ESIS_TYPE_ESH;

		/*
		 * Use suggested holding time advertised by IS.
		 */
		PUTSHORT(clns->esis_holding_time, (clns_esct_timer) ?
			 clns_esct_timer * 3:  clns_holding_timer/ONESEC);

		ptr = PAK2ADDR(pak);
		ptr++;			/* skip # of SA's for now */
		esis_pbuginf("\nES-IS: ESH sent to %s (%s)", esh_target_string,
			     idb->namestring);
	    }
	    /*
	     * Definitely room for this nsap.  Put it in the packet.
	     */
	    nnsaps++;
	    bcopy(ddb->destination, ptr, (ddb->destination[0] + 1));
	    ptr += ddb->destination[0] + 1;
	    esis_pbuginf("\n    NSAP %n", ddb->destination);
	}
    }

    /*
     * Send last packet.
     */
    if (ptr) {
	pak_hlen = clns->hlen = ptr - clnsheadstart(pak);
	ptr = PAK2ADDR(pak);
	*ptr = (uchar) nnsaps;

	/*
	 * Compute checksum if configured.
	 */
	if (idb->clns_checksum) {
	    compute_iso_checksum(clnsheadstart(pak),
				 clns->hlen,
				 clns->checksum);
	}

	if (clns_encaps(pak, pak_hlen, idb, esh_target_snpa, NULL)) {
	    datagram_out(pak);
	    clns_traffic[CLNS_OUTPUT]++;
	    clns_traffic[ESIS_ESHSSENT]++;
	} else {
	    datagram_done(pak);
	    if (!is_x25(idb->hwptr)) {
		esis_pbuginf("\nESIS: ESH encaps failed on %s",
			     idb->namestring);
	    }
	}
    }
    

}

/*
 * send_es_hellos
 *
 *    Send out the ES hellos.
 *
 *    For each area that this interface is in, send out an
 *    esh contains all of the nsaps for that area.
 */
static void send_es_hellos (
    idbtype *idb,
    hwaddrtype *esh_target_snpa,
    char *esh_target_string)
{
    clns_pdbtype *pdb;
    idbqueuetype *idbqueue;
    boolean sent_one;

    pdb = clns_protoQ.qhead;
    sent_one = FALSE;
    while (pdb){
	if (pdb->level & CLNS_LEVEL1) {
	    /* 
	     * This pdb is for  a level 1 router
	     * Only send out the hello if this interface is
	     * in this area.
	     */
	    idbqueue = pdb->idbqueue;
	    while (idbqueue) {
 		if (idbqueue->idb == idb) {
		    send_one_es_hello(idb, esh_target_snpa,
				      esh_target_string);
		    sent_one = TRUE;
		    break;
		}
		idbqueue = idbqueue->next;
	    }
	}
	pdb = pdb->next;
    }
    if (!sent_one)
	send_one_es_hello(idb, esh_target_snpa, esh_target_string);
}
    


/* static char *unicast_string = "Unicast"; */
static char *all_ess_string = "All ESs";
static char *all_iss_string = "All ISs";


/*
 * esis_sendhello
 * Send ESH's and ISH's out this interface.
 * We always send out ESH's, naming all the nsap's we have.  If we're an IS,
 * we send out the ISH as well.
 */
static void esis_sendhello (idbtype *idb)
{
    hwaddrtype new_station;
    hwaddrtype *esh_target_snpa;
    hwaddrtype *ish_target_snpa;
    char *esh_target_string;	/* Either "All ESs", "All ISs", or actual */
    char *ish_target_string;	/* snpa address */

    /*
     * Schedule the next hello at the regular time, even if this one is
     * a bonus one.  If this is the regularly scheduled time, then force
     * a multicast update, even if we saw a new entry very recently.
     */
    if (TIMER_RUNNING_AND_AWAKE(idb->clns_sched_hello_time)) {
	TIMER_UPDATE_JITTERED(idb->clns_sched_hello_time,
			      clns_config_timer,
			      CLNS_JITTER_PCT);
	idb->clns_hello_addr = NULL;	/* force it be multicast */
    }
    mgd_timer_set_exptime(&idb->clns_hello_timer, &idb->clns_sched_hello_time);

    /* If the interface is down, bail now. */

    if (!interface_up(idb))
	return;

    /*
     * If we're sending a bonus hello to a new station, construct
     * an snpa out of the addr & type left in the idb.
     */
    if (idb->clns_hello_addr) {
	new_station.type = idb->clns_hello_type;
	new_station.length = idb->clns_hello_addrlen;
	new_station.target_type = TARGET_UNICAST;
	bcopy(idb->clns_hello_addr, new_station.addr,idb->clns_hello_addrlen);
	idb->clns_hello_addr = NULL;	/* we only send it once */
	esh_target_string = snpa_string(&new_station);
	ish_target_string = esh_target_string;
	esh_target_snpa = &new_station;
	ish_target_snpa = &new_station;
    } else {
	ish_target_snpa = &all_ess;
	ish_target_string = all_ess_string;
	esh_target_snpa = &all_iss;
	esh_target_string = all_iss_string;
    }

    if (clns_router_running) {
	isisidbtype *isisidb;
	isisidb = NULL;
	if (idb->isis_idb) {
 	    isisidb =
 		idb->isis_idb[isis_pdbindex_to_procindex(CLNS_PDBINDEX_ISIS0)];
	}
	if (is_p2p(idb) || ((idb->clns_level & CLNS_LEVEL1) && 
			    (!isisidb || (isisidb->isis_circuit_type &
					  ISIS_CIRCUIT_L1)))) {
	    send_is_hellos(idb, ish_target_snpa, ish_target_string);
	}
    } else {
	send_es_hellos(idb, esh_target_snpa, esh_target_string);

	/*
	 * Use an IS suggested timer if one was received.
	 */
	if (clns_esct_timer) {
	    mgd_timer_start(&idb->clns_hello_timer, clns_esct_timer * ONESEC);
	}
    }
}

/*
 * rd_input
 * Process a received RD PDU (redirect).  Ignore it if we're an IS,
 * otherwise put it in the routing table.
 */

void rd_input (paktype *pak)
{
    clnshdrtype *clns;  /* structure pointer to clns header */
    uchar *ptr;		/* pointer into packet header */
    ushort len;		/* length of header so far */
    clns_ddbtype *ddb;	/* nsap routing info base */
    ulong pak_holding_time;	/* holding time extracted from packet */
    NSAP_T *da;		/* Destination address */
    ushort dal;		/* Destination address length */
    uchar *pak_bsnpa;	/* Better SNPA in packet */
    ushort pak_bsnpal;	/* Length of bsnpa in packet */
    hwaddrtype bsnpa;	/* extracted bsnpa put in standard format */
    ushort netl;	/* NET Length */
    NSAP_T *net;	/* Network Entity Title */
    uchar *amask;	/* address mask option */
    ushort amaskl;	/* length of address mask */
    ushort ouramaskl;	/* number of leading bytes that are all 0xff */
    int i;
    NSAP_T rd_da;	/* redirect dest addr */
    hwaddrtype pak_src_snpa;	/* sourse snpa extracted from packet */
    boolean len_ok;
    boolean opt_ok;
    clns_adjtype *adj;
    idbtype *idb;
    
    clns = (clnshdrtype *)clnsheadstart(pak);

    idb = pak->if_input;
    pak_holding_time = GETSHORT(clns->esis_holding_time);
    if (esis_event_debug || esis_packet_debug) {
	(*idb->hwptr->extract_snpa)(pak, &pak_src_snpa, SOURCE);
	/*
	 * ESIS: RD from Ethernet0 <snpa>, HT 300
	 */
	esis_pbuginf("\nES-IS: RD from %s (%s), HT %d",
		     snpa_string(&pak_src_snpa), idb->namestring,
		     pak_holding_time);
    }
    if (clns_router_running) {
	esis_pbuginf(", ignored.  We are not an ES.");
	return;
    }
    len = CLNS_FIXEDHDR_SIZE;
    ptr = clnsheadstart(pak) + len;

    if (clns->hlen < ESIS_MINRDHEADERBYTES) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf("\nES-IS: Bad header length %d/%d", 
		     ESIS_MINRDHEADERBYTES, clns->hlen);
	return;
    }

    /*
     * Destination Address
     */
    da = (NSAP_T *) ptr;
    dal = *ptr++;
    len += dal + 1;
    ptr += dal;
    if ((dal < CLNS_MIN_NSAP_LENGTH) || (dal >= CLNS_MAX_NSAP_LENGTH) ||
	(len > clns->hlen)) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf(", bad DAL %d", dal);
	return;
    }
    if (!AFI_OK(((uchar *) da))) {
	esis_pbuginf("\nES-IS: Bad AFI for DA %n", (char *)da);
	return;
    }

    /*
     * Better SNPA.  Check that the length fits in the packet header,
     * and that it is correct for the particular subnet its on.
     * Save a pointer to the BSNPA and it length.
     */
    pak_bsnpal = *ptr++;
    pak_bsnpa = ptr;
    len += pak_bsnpal + 1;
    ptr += pak_bsnpal;
    if (len > clns->hlen) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf(", bad BSNPAL %d", pak_bsnpal);
	return;
    }
    switch (idb->hwptr->enctype) {
      case ET_X25:
	/* 
	 * X.121 address are sent as up to 14 BCD digits, 2 digits per
	 * byte, so we need to include the length, and then figure
	 * how many bytes are needed
	 */
	bsnpa.type = STATION_X121;
	bsnpa.target_type = TARGET_UNICAST;
	bsnpa.length = pak_bsnpa[0] & 0xf;	/* low 4 bits */
	len_ok = ((pak_bsnpal <= ((STATIONLEN_X121 + 2) / 2)) &&
		  (bsnpa.length <= STATIONLEN_X121) &&
		  (!badbcd(&pak_bsnpa[0], bsnpa.length)));
	bcdtochar(&pak_bsnpa[0], bsnpa.addr, bsnpa.length);
	break;
      case ET_HDLC:
	len_ok = TRUE;
	break;
      case ET_ARPA:
      case ET_SNAP:
      case ET_SAP:
	/* *** LATER -- fix for TR Source routing **** */
	len_ok = (pak_bsnpal == STATIONLEN_IEEE48);
	if (len_ok) {
	    bsnpa.type = STATION_IEEE48;
	    bsnpa.length = STATIONLEN_IEEE48;
	    bsnpa.target_type = TARGET_UNICAST;	/* *** LATER -- check this */
	    ieee_copy(pak_bsnpa, bsnpa.addr);
	}
	break;
      case ET_SMDS:
	len_ok = (pak_bsnpal == STATIONLEN_SMDS64);
	if (len_ok) {
	    bsnpa.type = STATION_SMDS64;
	    bsnpa.length = STATIONLEN_SMDS64;
	    bsnpa.target_type = TARGET_UNICAST;	/* *** LATER -- check this */
	    bcopy(pak_bsnpa, bsnpa.addr, STATIONLEN_SMDS64);
	}
	break;
      default:
	len_ok = FALSE;
	break;
    }
    if (!len_ok) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf(", bad BSNPAL %d", pak_bsnpal);
	return;
    }
    /*
     * NET.  May be zero length.
     */
    net = (NSAP_T *) ptr;
    netl = *ptr++;
    len += netl + 1;
    ptr += netl;
    if ((netl >= CLNS_MAX_NSAP_LENGTH) || (len > clns->hlen)) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	esis_pbuginf(", bad NETL %d", netl);
	return;
    }

    /*
     * If NET specified, check if valid AFI.
     */
    if (netl && !AFI_OK(((uchar *) net))) {
	esis_pbuginf("\nES-IS: Bad AFI for NET %n", (char *) da);
	return;
    }

    /*
     * Options.  If there's still some more to the header, then it
     * must be options.  The only one we're interested in is the
     * 'Address Mask' option.
     */
    amaskl = 0;
    amask = NULL;
    if (len < clns->hlen) {
	pak->clns_optpart = ptr;
	opt_ok = TRUE;
	while ((len < clns->hlen) && opt_ok) {
	    switch (*ptr) {
	      case CLNS_OPT_ADDRMASK:
		CHECK_DUP(CLNS_OPT_ADDRMASK);
		amaskl = ptr[1];
		if (amaskl < CLNS_MIN_NSAP_LENGTH || 
		    amaskl >= CLNS_MAX_NSAP_LENGTH) {
		    clns_traffic[ESIS_BADOPTION]++;
		    return;
		}
		amask = &ptr[1];
		break;
	      case CLNS_OPT_SNPAMASK:
		CHECK_DUP(CLNS_OPT_SNPAMASK);
		break;		/* no-op */
	      case CLNS_OPT_ESCT:
		CHECK_DUP(CLNS_OPT_ESCT);
		break;		/* no-op */
	      case CLNS_OPT_PADDING:
		CHECK_DUP(CLNS_OPT_HASPAD);
		break;		/* padding option is a no-op */
	      case CLNS_OPT_PRIORITY:
		CHECK_DUP(CLNS_OPT_HASPRIOR);
		break;		/* priority option is a no-op */
	      case CLNS_OPT_QOS:
		CHECK_DUP(CLNS_OPT_HASQOS);
		if ((ptr[2] & CLNS_QOS_GLOBAL) == CLNS_QOS_GLOBAL) {
		    if (ptr[1] != CLNS_OPT_GQOS_LEN) {
			clns_traffic[CLNS_BADOPTION]++;
			return;
		    }
		    pak->clns_flags |= CLNS_OPT_HASGQOS;
		}	/* non-global qos is no-op'ed */
		pak->clns_qos = ptr;	/* save important option */
		break;

	      case CLNS_OPT_SECURITY:
	      case CLNS_OPT_RROUTE:
	      case CLNS_OPT_SROUTE:
	      case CLNS_OPT_DISCARD_REASON:
	      default:
		clns_traffic[ESIS_BADOPTION]++;
		return;
	    }
	    len += ptr[1] + 2;
	    ptr += ptr[1] + 2;
	}
	if (!opt_ok) {
	    clns_traffic[ESIS_BADOPTION]++;
	    esis_pbuginf(", bad option %#x at %d", *ptr, len);
	    return;
	}
    }

    /*
     * All done cracking the packet (whew!).  Now we find out how many
     * bytes at the beginning of the Address Mask are all 1's, and the
     * use that many bytes of the destination as the key in the
     * routing table entry.
     * LATER -- if there are trailing non-contiguous 1's, ignore the mask
     */
    if (amaskl) {
	for (i = 0; ((i < amaskl) && (amask[i+1] == 0xff)); i++) {
	}
	ouramaskl = i;
    } else {
	ouramaskl = dal;	/* no addr mask, use all of dest addr */
    }

    /*
     * Create the destination nsap that we'll put in the routing table,
     * which is the destination nsap as adjusted by the address mask,
     * then put this nsap in the nrib, as a redirect entry, if its not
     * in there already.  We could change the length in the packet but
     * I don't like altering the packet.
     */
    nsap_copy(*da, rd_da);
    rd_da[0] = ouramaskl;

    ddb = nsap_lookup(*da, MATCH_EXACT);
    if (ddb) {
	if (ddb->entry_type != CLNS_REDIRECT_ENTRY) {
	    esis_pbuginf(", ddb entry already exists as %s",
			 clns_hellostring(ddb->entry_type));
	    clns_traffic[ESIS_ADDRCONFLICTSRCVD]++;
	    return;
	}
    } else {
	ulong exp_time;
	GET_TIMESTAMP32(exp_time);
	exp_time += pak_holding_time * ONESEC;
	if (!(clns_route_update(rd_da, *net, idb,
				CLNS_REDIRECT_ENTRY, ISO_ESIS,
				ISO_ESIS_DIST,
				exp_time, timer_no_time, 0, NULL))) {
	    esis_pbuginf("\nES-IS: could not insert nsap %n for RD PDU",rd_da);
			 
	    return;
	}
    }

    /*
     * Store net's SNPA info in adjacency database. 
     */
    station_copy(get_station(*net), &rd_da[1]); /* Use rd_da as temp spot */
    rd_da[0] = STATION_LENGTH;
    adj = add_adjacency(clns_adjacency_db, rd_da, idb, &bsnpa,
			pak_holding_time, idb->clns_cluster_alias);
    if (adj) {
	adj->state = CLNS_ADJ_UP_STATE;
	adj->adjacencyType = CLNS_IS_ADJ_TYPE;
	adj->protocol_source |= ISO_ESIS;
	add_area_to_adj(*net, adj);
    }

    /*
     ** ESIS: RD from Ethernet0 <snpa>, HT 300,
     **       redirecting <nsap> to [<net> @ ] <bsnpa> [for all <addrmask>]
     */
    esis_pbuginf("\n      redirecting %n to %s%s%s%s%s",
		*da, 
		(netl) ? nsap_string(*net) : "", 
		(netl) ? " @ " : "",
		snpa_string(&bsnpa),
		(amaskl) ? " for all " : "",
		(amaskl) ? nsap_string(*(NSAP_T *)amask) : "");
    clns_traffic[ESIS_RDUSRCVD]++;
}


/*
 * esis_routeBLOCK
 * Block waiting for something for the ESIS router process to do
 */

boolean esis_routeBLOCK (
     /*queuetype * */ ulong q)
{
    if (((queuetype *)q)->qhead)
	return(FALSE);			/* input available */
    if (mgd_timer_expired(&esis_timer))
	return(FALSE);			/* timer expired */
    return(TRUE);
}

/*
 * esis_router
 * ISO 9542 End System to Intermediate System Routing Exchange Protocol.
 */

forktype esis_router (void)
{
    paktype *pak;
    clnshdrtype *clns;
    idbtype *idb;
    int nl_len;			/* len of network layer portion of packet */
    
    edisms(systeminitBLOCK, 0);
    process_set_priority(PRIO_LOW);
    while (TRUE) {
	edisms((blockproc *)esis_routeBLOCK, (ulong) &esisQ);
	/*
	 * First empty the ES-IS packet queue
	 */
	while ((pak = dequeue(&esisQ))) {
	    clns = (clnshdrtype *)clnsheadstart(pak);

	    pak->clns_nexthopaddr = NULL;
	    pak->clns_dstaddr = NULL;	/* ES-IS doesn't use these */
	    pak->clns_srcaddr = NULL;
	    pak->clns_segpart = NULL;
	    pak->clns_optpart = NULL;
	    pak->clns_datapart = NULL;
	    pak->clns_qos = NULL;
	    pak->clns_flags = 0;
	    nl_len = pak->datagramsize - (clnsheadstart(pak) - pak->datagramstart);
	    if ((clns->version != ESIS_VERSION) ||
		(clns->hlen < ESIS_MINHEADERBYTES) ||
		(clns->hlen > CLNS_MAXHEADERBYTES) ||
		(clns->lifetime != 0) ||
                (clns->hlen > nl_len)) {
		clns_traffic[CLNS_HDRSYNTAX]++;
		esis_pbuginf("\nES-IS: %s hdr syntax error", 
			   pak->if_input->namestring);
		datagram_done(pak);
		continue;
	    }
	    if (!verify_iso_checksum(clnsheadstart(pak),
				     clns->hlen,
				     clns->checksum)) {
		clns_traffic[CLNS_CHECKSUM]++;
		esis_pbuginf("\nES-IS: %s PDU received, bad cs",
			   pak->if_input->namestring);
		datagram_done(pak);
		continue;
	    }
	    switch (clns->type) {
	      case ESIS_TYPE_ESH:
		esh_input(pak);
		break;
	      case ESIS_TYPE_ISH:
		ish_input(pak);
		break;
	      case ESIS_TYPE_RD:
		rd_input(pak);
		break;
	      case ESIS_TYPE_RA:
		ra_input(pak);
		break;
	      default:
		esis_pbuginf("\nES-IS: Unknown PDU received on %s, type=%#x", 
			   pak->if_input->namestring, clns->type);
		break;
	    }
	    datagram_done(pak);
	}
	/*
	 * background stuff to do every so often
	 *     schedule next wakeup 
	 *     Age the ESH's, ISH's, and RDU's
	 *     Maybe send out some ISH's or ESH's
	 */
	while (mgd_timer_expired(&esis_timer)) {
	    mgd_timer *expired_timer;

	    process_may_suspend();

	    /* Something expired.  Get the expired timer entry. */

	    expired_timer = mgd_timer_first_expired(&esis_timer);
	    switch (mgd_timer_type(expired_timer)) {

		/* Ager timer.  Go run the ager. */

	      case ESIS_AGER_TIMER:
		esis_ager();
		mgd_timer_update(expired_timer, ESIS_PERIODIC_TIMER);
		break;

		/*
		 * One of the hello timers went off.  If the interface
		 * is still enabled for CLNS, attempt to send a hello
		 * (which resets the timer).  If CLNS has since been
		 * deconfigured, turn off the timer.  It will be restarted
		 * when the interface is configured for CLNS again.
		 */
	      case ESIS_HELLO_TIMER:
		idb = mgd_timer_context(expired_timer);
		if (idb->clns_enabled) {
		    esis_sendhello(idb);
		} else {
		    mgd_timer_stop(expired_timer);
		}
		break;

	      default:
		crashdump(0);		/* Can't happen! */
	    }
	}
    }
}
