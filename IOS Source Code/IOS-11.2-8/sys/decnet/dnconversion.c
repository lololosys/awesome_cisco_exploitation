/* $Id: dnconversion.c,v 3.10.20.6 1996/08/03 23:21:51 fox Exp $
 * $Source: /release/112/cvs/Xsys/decnet/dnconversion.c,v $
 *------------------------------------------------------------------
 * dnconversion.c -- DECnet Phase IV to Phase V compatibilty
 *
 * 18-May-90, V. Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains all of the glue routines needed to support
 * compatibility between phase IV and phase V decnet. They are isolated
 * here to minimize dependancies between the DECnet IV and clns code.
 *------------------------------------------------------------------
 * $Log: dnconversion.c,v $
 * Revision 3.10.20.6  1996/08/03  23:21:51  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.20.5  1996/06/18  18:55:46  asastry
 * CSCdi58806:  DEC "advertised" route does not work correctly if ISO-IGRP
 * is used.
 * Branch: California_branch
 *
 * Revision 3.10.20.4  1996/05/30  01:19:27  asastry
 * CSCdi58810:  DEC 'advertise' route is wiped out by "dec routing"
 * command.
 * Branch: California_branch
 *
 * Revision 3.10.20.3  1996/05/29  19:07:58  asastry
 * CSCdi46634:  Misc DECnet code cleanup - get rid of unwanted registry
 * call.
 * Branch: California_branch
 *
 * Revision 3.10.20.2  1996/04/03  20:33:12  asastry
 * CSCdi39329:  DECnet IV/V conversion should preserve Intra-Ethernet Bit
 * if the sender is on same LAN and is a Phase IV host.
 *
 * Branch: California_branch
 *
 * Revision 3.10.20.1  1996/03/18  19:11:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.6.2  1996/03/07  08:42:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.6.1  1996/02/20  00:43:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/01  06:00:51  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/23  02:38:31  asastry
 * CSCdi47315:  DECnet IV/V conversion makes bogus entries in ISO-IGRP L2
 * routing table
 *
 * Revision 3.8  1996/01/11  02:06:02  asastry
 * CSCdi38569:  DECnet Phase-IV / Phase-V conversion is very slow
 *
 * Revision 3.7  1995/12/02  00:49:19  asastry
 * CSCdi44859:  DECnet conversion should make validity checks
 *
 * Revision 3.6  1995/12/01  17:01:53  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.5  1995/11/27  21:46:16  rchandra
 * CSCdi43854:  Decnet: converted packet with exceeded lifetime cause crash
 * - if lifetime is exceeded for the converted packet, do not try to
 *   generate clns error pdu
 * - allow the converted packet marked for return (DN_RET) to have twice
 *   the lifetime than the normal converted packet.
 *
 * Revision 3.4  1995/11/20  19:05:28  asastry
 * CSCdi43658:  no decnet advertise command crashes 4500
 *
 * Revision 3.3  1995/11/17  08:59:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:16:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/30  01:30:34  asastry
 * CSCdi39208:  Segv exception when running DECnet IV/V conversion. Remove
 * unnecessary argument in 'dn_phaseIV_address_check'.
 *
 * Revision 2.1  1995/06/07  20:23:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "decnet_externs.h"
#include "decnet_debug.h"
#include "access.h"
#include "address.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "dnconversion.h"
#include "config.h"
#include "parser_defs_dec.h"
#include "../ip/ip_registry.h"


/*
 * Local Variables
 */
static const uchar dn_hiord[4]                 = {0xAA, 0x00, 0x04, 0x00};
static uchar dn_phaseIVprefix[21]; /* we store a copy of the domain portion
				    * of the associated clns router in this
				    * variable
				    */


/*
 * External Declarations
 */
extern watched_queue *DNpacketQ;
extern queuetype decnet_advQ;
extern boolean dn_conversion;

/*
 * dn_get_address
 */

static uchar *dn_get_address (NSAP_T nsap)
{
    uchar *station;

    station = get_station(nsap);
    return(&station[4]);
}

/* 
 * dn_addr_to_clns
 * Convert an address from phase IV -> phase V. 
 */

static boolean dn_addr_to_clns (ushort ivarea, int ivnode, int firstbyte,
				uchar *address)
{
    phasevaddrtype *partial_address;

    partial_address = (phasevaddrtype *)(address + dn_phaseIVprefix[0] + 1);

    /*
     * Look at the first byte of data (the prot-type field).  If any
     * bit(s) within the bits masked by 0x7c is set or value is 0 then
     * we have a value reserved for NSP.  In this case we set the selector
     * field to NSP.  Otherwise we place the prot-type value in the selector.
     * For the destination address if we have a value reserved for NSP we
     * leave the data alone (first byte remains the prot-type field).
     * Otherwise, clns data starts at second byte of dn data.  Pointer to
     * data is manipulated by the caller (dn_pak_to_clns).
     */
    if (!(firstbyte & PHASEIV_NOT_NSP)) {
	partial_address->sel = NSP;
     } else {
	partial_address->sel = firstbyte;
    }
    bcopy(&dn_phaseIVprefix[1], &address[1], dn_phaseIVprefix[0]);
    address[0] = PHASEVADDRTYPE_SIZE + dn_phaseIVprefix[0];
    PUTSHORT(&partial_address->loc_area, ivarea);
    bcopy(dn_hiord, partial_address->id, 4);
    dn_addr(partial_address->addr, ivarea, ivnode);
    return(TRUE);
}

/*
 * dn_check_clns_prefix
 *
 *    This routine will check if there is a CLNS prefix route corresponding
 *    to the given DECnet area before advertising the area. This applies only
 *    to DECnet areas that have been statically configured, using the
 *    'decnet advertise' command. If a corresponding CLNS route (either
 *    prefix or ISO-IGRP area route) is found, the routine will return
 *    TRUE, else it will return FALSE.
 */
boolean dn_check_clns_prefix (uchar ivarea)
{
    NSAP_T clns_prefix;

    /* 
     * Don't bother if conversion is not enabled 
     */    
    if (!dn_conversion)
	return (FALSE);

    dn_build_clns_prefix(ivarea, clns_prefix);
    return (reg_invoke_clns_route_search(clns_prefix));
}

/*
 * dn_build_clns_prefix
 *
 *    Given the DECnet IV area, build a CLNS prefix.
 */

void dn_build_clns_prefix (uchar ivarea, uchar *prefix)
{
    uchar length, *temp;

    temp = prefix;
    length = dn_phaseIVprefix[0];

    /*
     * First copy the DECnet conversion prefix into our buffer.
     */
    bcopy(&dn_phaseIVprefix[1], (temp + 1), length);

    /* 
     * Then fill in the area, and insert the prefix length
     */
    PUTSHORT((temp+length+1), ivarea);
    *prefix = length + AREA_LENGTH;
}

/*
 * dn_inform_clns
 *
 *    Inform CLNS about topology change. This applies only when there
 *    is a reachability change for the DECnet area indicated in the
 *    arg 'ivarea' (i.e. applies only to L-2 changes).
 *    
 *    We then call CLNS to see if there is a correponding CLNS static
 *    discard route. If there is, we change the state of the route to UP
 *    or down, based on whether the argument 'new_state' is TRUE or FALSE.
 */

void dn_inform_clns (uchar ivarea, uchar new_state)
{
    NSAP_T clns_prefix;

    /*
     * Don't bother if conversion is not enabled 
     */
    if (!dn_conversion)
	return;

    /*
     * Don't bother if CLNS is not running
     */
    if (!RUNNING(LINK_CLNS))
	return;

    /* 
     * Compute CLNS prefix
     */
    dn_build_clns_prefix(ivarea, clns_prefix);
    reg_invoke_clns_discard_route_adjust(clns_prefix, new_state);

    return;
}

/*
 * dn_phaseIV_address_check
 *
 *    V -> IV
 *    Return true if the specified address is phase IV conformant
 *    and can be converted to a phase IV address.
 *
 *    phaseIV_address must be non-null
 *    hiord must equal high order 32 bits of station id
 *    idp matches phaseIVprefix
 */

static boolean dn_phaseIV_address_check (NSAP_T address)
{
    uchar *area, *id;

    if (!nsap_match(dn_phaseIVprefix, address)) {
	return(FALSE);
    }

    id = get_station(address);

    /*
     * First 4 bytes of the station address need to be "AA-00-04-00"..
     */
    if (bcmp(dn_hiord, id, 4) != 0) {
	return(FALSE);
    }
    
    area = get_area(address);
    if ((area[0] != 0) || (area[1] > DN_MAX_AREA)) {
	return(FALSE);
    }

    return(TRUE);
}

/* 
 * dn_pak_to_clns
 * Convert a packet from phase IV format to phase V format.
 * If the packet cannot be converted, return NULL.
 * Note: that the selector byte must conform to all DEC
 * retrictions. 
 */

static paktype *dn_pak_to_clns (paktype *pak, dndatapacket *data,
				int firstbyte)
{ 
    paktype *clns_pak;
    clnshdrtype *clns;
    short area, size;
    uchar sel, *ptr;
    int padsize;
    dnhdrtype *dn;
 
    dn = (dnhdrtype *)dnheadstart(pak);

    size = get16(dn->paksize);

    /* 
     * Get a buffer to hold the CLNS fixed header, source and destination 
     * NSAPs, and paksize.
     */

    clns_pak = clns_getbuffer(CLNS_MINHEADERBYTES +
			      (2 * CLNS_MAX_NSAP_LENGTH) + size);
    if (clns_pak == NULL)
	return(NULL);
    clns_pak->clns_flags = CLNS_CONVERTED;

    clns = (clnshdrtype *)clnsheadstart(clns_pak);

    /*
     * Fixed portion of the packet header
     */
    clns->pid = NLP_ISO8473;
    /* skip header length for now */
    clns->version = CLNS_VERSION;

    /*
     * Packets with DN_RET set (i.e. those being returned) are allowed
     * to have twice the hop count than the  normal packets 
     */
    if (data->typecode & DN_RET)
	clns->lifetime = 128 - data->visits;
    else
	clns->lifetime = 64 - data->visits;

    clns->sp = 0;	/* not segmentation allowed */
    clns->ms = 0;	/* this is the only */
    clns->type = CLNS_TYPE_DATA; 
    clns_pak->if_input = pak->if_input;
    if (data->typecode & DN_RETREQ) 	/* if we're supposed to return */
	clns->er = 1;	/* send us error reports */
    else
	clns->er = 0;	/* don't send us error reports */

    clns_pak->clns_dstaddr = ptr = PAK2ADDR(clns_pak);
    /* Convert the address in place */
    area = dn_area(data->dest);
    if (!dn_addr_to_clns(area, dn_node(data->dest), firstbyte, ptr)) {
	retbuffer(clns_pak);
	return(NULL);
    }
    ptr += ptr[0] + 1;

    clns_pak->clns_srcaddr = ptr;
    /* Convert the address in place */
    area = dn_area(data->source);
    if (!dn_addr_to_clns(area, dn_node(data->source), firstbyte, ptr)) {
	retbuffer(clns_pak);
	return(NULL);
    }
    ptr += ptr[0];
    sel = *ptr++;
    /*
     * Header done -- fill in the header length, copy the entire request
     * packet.
     */
    clns->hlen = ptr - clnsheadstart(clns_pak); 
    padsize = (dn->pakstart[0] & DN_PAD) ? (dn->pakstart[0] & 0x7f) : 0;
    size = size - (SIZEOF_DNDATAPACKET);
    size = size - padsize;
    clns_pak->datagramstart = clnsheadstart(clns_pak);

    /*
     * If the SEL field is NSP, use the data portion as is, otherwise
     * the first byte of data (PROT-TYPE) has been copied into the
     * SEL field so start the CLNS data packet with the second byte
     * of the phase IV packet.
     */
    if (sel == NSP) {
	PUTSHORT (clns->clns_seglen, size + clns->hlen); 
	clns_pak->datagramsize = size + clns->hlen;
	bcopy(&data->databyte, ptr, size);
    } else  {
	PUTSHORT (clns->clns_seglen, (size + clns->hlen) - 1); 
	clns_pak->datagramsize = ((size + clns->hlen) - 1);
	bcopy((&data->databyte) + 1, ptr, (size - 1));
    }
    clns_pak->linktype = LINK_CLNS;
    clns_pak->clns_optpart = 0;
    PUTSHORT (clns->checksum, 0);
    return(clns_pak);
}

/*
 * dn_add_phaseIVneighbor
 * Called by iso igrp when a neighbor (adjacency) is added or refreshed
 * This puts the adjacency into the DECnet database.
 */

static void dn_add_phaseIVneighbor (NSAP_T nsap, idbtype *idb, int hello_time)
{
    uchar *address;
    short addr;
    int hernode, herarea;
    dnnet *net;

    net = dn_nets[idb->dn_network];
    if (!dn_phaseIV_address_check(nsap))
	return;

    addr = dn_idn(dn_get_address(nsap));
    address = dn_get_address(nsap);
    hernode = dn_node(address);
    herarea = dn_area(address);

    /*
     * We will only store information about end systems that
     * are in our area.
     */
    if ((!net) || (herarea != net->my_dn_area))
	return;
    dn_add_adjacency(address, addr, herarea, hernode, net, DN_NT_END, 
		     DN_RT_HELLO, hello_time, idb->dn_cost, PHASEV, NULL,
		     NULL, 0, idb);
}

/*
 * dn_cnv_check_route
 *
 * Check if there is a route for the given destination, and return TRUE if
 * there is; else return FALSE.
 */
static boolean dn_cnv_check_route (dnnet *net, uchar *dn_destination)
{
    uchar area;
    ushort node;
    dnroutetype *routeptr;

    node = dn_node(dn_destination);
    area = dn_area(dn_destination);

    /*
     * If destination is in a different area, we use the L2 routing table
     * (i.e. "dnareas[area]")  only if we are Level-2 and are "attached"
     */
    if (area != net->my_dn_area)  {  		
	if ((net->doareas) && (net->attached)) {
	    routeptr = net->dnareas[area];
	} else {
	    routeptr = net->dnroutes[0];
	}
    } else {
	routeptr = net->dnroutes[node];
    }   
    
    return(routeptr != NULL);
}

/*
 * dn_cnv_2_phaseIV_pak
 * Convert the specified clns packet to a decnet phase IV packet
 */

static paktype *dn_cnv_2_phaseIV_pak (paktype *clns_pak)
{
    unsigned char sel, *ptr, firstbyte;
    dndatapacket *data;
    ushort size, current_length, numbytes_needed;
    uchar *clns_data, netnum;
    paktype *dn_pak;
    clnshdrtype *clns;
    dnhdrtype *dn;
    dnnet *net;

    clns = (clnshdrtype *)clnsheadstart(clns_pak);

    if (clns->ms)     /* Multiple segments are not allowed */
	return(NULL);

    dn_pak = getbuffer(MAXDNDATASIZE);
    if (!dn_pak) {
	return(NULL);
    }

    dn = (dnhdrtype *)dnheadstart(dn_pak);

    size = GETSHORT(clns->clns_seglen) - clns->hlen;
    clns_data = clnsheadstart(clns_pak) + clns->hlen;
    netnum = (clns_pak->if_input) ? clns_pak->if_input->dn_network : 0;
    net = dn_nets[netnum];
    ptr = (uchar *)&dn->pakstart[0];
    dn_pak->datagramstart = (uchar *)&dn->pakstart[0];
    *ptr++ = 0x81;
    size = size + 1;
    data = (dndatapacket *)ptr;
    firstbyte = clns_data[0];
    ptr = PAK2ADDR(clns_pak); /* Position pointer to the destination address */
    if (!dn_phaseIV_address_check(ptr)) {
	dnet_pabuginf("\nDNET: Discard packet, bad Phase IV destination "
		      "address");
    dn_return_error:
  	retbuffer(dn_pak);
   	return(NULL); 
    }

    bcopy(dn_get_address(ptr), data->dest, 2);

    /*
     * Check if there is a IV route to the destination, and return FALSE if
     * there isn't one.
     */
    if (!dn_cnv_check_route(net, data->dest)) {
	dnet_pabuginf("\nDNET: Discarding packet - no IV route for "
		      "destination %d.%d",
		      dn_area(data->dest), dn_node(data->dest));
	goto dn_return_error;
    }

    ptr += ptr[0] + 1;     /* Postion pointer to the source address */

    if (!dn_phaseIV_address_check(ptr)) {
	dnet_pabuginf("\nDNET: Discard packet, bad Phase IV source address");
	goto dn_return_error;
    }

    bcopy(dn_get_address(ptr), data->source, 2);

    ptr += ptr[0];     /* Postion pointer to the selector byte */
    sel = *ptr;
    data->typecode = DN_DATA | DN_LONG;
    data->dest_area = 0;
    data->dest_subarea = 0;
    data->source_area = 0;
    data->source_subarea = 0;
    data->next_level_2 = 0;
    data->visits = 64 - min(clns->lifetime, 64);
    data->service = 0;
    data->protocol = 0;

    /* Pull out the ethernet addresses from the packet */
    bcopy(dn_hiord, data->dest_hi, 4);
    bcopy(dn_hiord, data->source_hi, 4);

    /* If return on error was requested lite the bit */
    if (clns->er)
	data->typecode |= DN_RETREQ;
 
    /* 
     * Ensure that we have enough space for the DECnet IV data portion
     */
    numbytes_needed = (sel == NSP) ? size : size+1 ;
    current_length = (char *)&data->databyte - (char *)dnheadstart(dn_pak);
    if ((MAXDNDATASIZE - current_length) < numbytes_needed) {
	dnet_pabuginf("\nDNET: Insufficient space for DECnet IV data");
 	goto dn_return_error;
    }

    if (sel == NSP) {
	bcopy(clns_data, &data->databyte, size);
    } else {
	data->databyte = sel;
	bcopy(clns_data, ((&data->databyte) + 1), size);
	size = size + 1;
    }

    put16t(dn->paksize, size + SIZEOF_DNDATAPACKET)
    dn_pak->linktype = LINK_DECNET;
    dn_pak->if_output = clns_pak->if_output;
    dn_pak->if_input = clns_pak->if_input;
    if (!dn_pak->if_input) {
	dn_pak->if_input = dn_echo_interface;
    }
    dn_pak->clns_flags = CLNS_CONVERTED;
    dn_pak->datagramsize = get16(dn->paksize);
    return(dn_pak);
}

/*
 * dn_setup_prefix
 *    Set up the phaseIV prefix variable from the domain portion
 *    of the specified IGRP process.
 */

static boolean dn_setup_prefix (char *tag)
{
    clns_pdbtype *pdb;

    pdb = reg_invoke_ip_clns_find_router_name(tag);
    if (!pdb) {
	printf("\nIGRP router not found");
	return(FALSE);
    }
    /*
     * copy the prefix from the pdb into the prefix variable.
     */
    bcopy(pdb->domain, &dn_phaseIVprefix[1], pdb->domain_length);
    dn_phaseIVprefix[0] = pdb->domain_length;
    return(TRUE);
}

/*
 * dn_add_clns_neighbor
 *
 *     Called by decnet phaseIV when it wants to add a neighbor
 *     to the phaseV database.
 */

static void dn_add_clns_neighbor (paktype *pak, int herarea, int hernode,
				  int time)
{
    NSAP_T nsap;
    NSAP_T station;
    hwaddrtype snpa;
    clns_pdbtype *pdb;
    clns_rdbtype *rdb;
    clns_adjtype *adj;

    if (!(dn_addr_to_clns(herarea, hernode, (uchar) 0, (uchar *)(&nsap)))) {
	return;
    }
    
    /* 
     * Need to fill in the appropriate fields in the snpa 
     */
    (*pak->if_input->hwptr->extract_snpa)(pak, &snpa, SOURCE);

    time = time/ONESEC;
    bcopy(&nsap[nsap[0] - STATION_LENGTH], &station[1], STATION_LENGTH);
    station[0] = STATION_LENGTH;

    /*
     * If adjacency already exists, and it was created by a DECnet hello,
     * update the hello timer.
     */
    adj = find_adjacency(clns_adjacency_db, station, FALSE, pak->if_input, 0);
    if (adj) {
	if (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) {
	    TIMER_START(adj->expiration_time, time * ONESEC);

            pdb = clns_find_pdb(nsap);
	    /* 
	     * If ISO-IGRP, update expiration timer for the route as well.
	     * This applies only to L1 ISO-IGRP routes.
	     */
            if (pdb && (pdb->typeflag & ISO_IGRPMASK) && 
		        area_match(get_area(pdb->net), get_area(nsap))) {
  	        rdb = iso_igrp_destination_update(pdb, 
		          CLNS_STATION_TYPE, station, pak->if_input, station, 
		          &snpa, iso_igrp_compute_metric(pdb, pak->if_input), 
		          DECNET_PHASEIV, CLNS_ROUTE_ENTRY, time*ONESEC);
            } 
        }
	return;
    }

    /*
     * Add new adjacency to OSI adjacency database and inform OSI routing
     * protocols.
     */
    adj = add_adjacency(clns_adjacency_db, station, pak->if_input, &snpa, time,
			pak->if_input->clns_cluster_alias);
    /*
     * The function "add_adjacency" returns a NULL if malloc fails
     */
    if (!adj)
	return;

    /*
     * Assign fields.
     */
    adj->neighbor_pak_fmt = CLNS_PHASE_IV_FORMAT;
    adj->protocol_source |= DECNET_PHASEIV;
    adj->adjacencyType = CLNS_ES_ADJ_TYPE;
    adj->state = CLNS_ADJ_UP_STATE;
    nsap_copy(nsap, adj->area_address[0]);
    adj->area_address[0][0] -= SYSTEMID_LENGTH;
    adj->num_areas = 1;

    if (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) {
        pdb = clns_find_pdb(nsap);
        if (!pdb || !(pdb->level & CLNS_LEVEL1)) {
	    return;
	}

	/*
	 * If we are dealing with ISO IGRP here, make sure that we 
	 * add an entry only to the system ID (L1) ISO IGRP table.
	 */
        if (pdb->typeflag & ISO_IGRPMASK) {
	    if (area_match(get_area(pdb->net), get_area(nsap))) {
		iso_igrp_setup_metric(iso_igrp_destination_update(pdb, 
			      CLNS_STATION_TYPE, station, pak->if_input, station, 
			      &snpa, iso_igrp_compute_metric(pdb, pak->if_input), 
		              DECNET_PHASEIV, CLNS_ROUTE_ENTRY, time*ONESEC), 
			      pak->if_input);
	    }
        } else if (pdb->typeflag & ISO_ISISMASK) {
            isis_new_es_adjacency(pdb, adj);
	}
    }
}

/*
 * dn_delete_clns_neighbor
 *
 *     Called by decnet phaseIV when it wants to add a neighbor
 *     to the phaseV database.
 */

static boolean dn_delete_clns_neighbor (int herarea, int hernode)
{
    NSAP_T nsap;

    if (!(dn_addr_to_clns(herarea, hernode, (uchar) 0, (uchar *) &nsap))) {
	return(FALSE);
    }
    nsap_delete(nsap);
    return(TRUE);
}

/*
 * dn_clns_switch
 */

static boolean dn_clns_switch (paktype *pak, dndatapacket *data)
{ 
    paktype *clns_pak;
    NSAP_T temp_nsap;
    int area;

    area = dn_area(data->dest);
    if (!dn_addr_to_clns(area, dn_node(data->dest),data->databyte,temp_nsap)) {
	return(FALSE);
    }
	
    temp_nsap[temp_nsap[0]] = 0;
    
    clns_pak = dn_pak_to_clns(pak, data, data->databyte);
    if (!clns_pak) {
	return(FALSE);
    }

    if (!clns_route(clns_pak->clns_dstaddr, ROUTINGDEPTH)) {
	retbuffer(clns_pak);
	return(FALSE);
    }
 
    /*
     * Queue packet for CLNS processing only if we have a route.
     */
    process_enqueue_pak(clnsQ, clns_pak);
    
    return(TRUE);
}

/*
 * clns_giveto_decnet
 * This CLNS packet couldn't be switched.   See if DECnet wants it.
 * Destination wasn't in any of the OSI routing tables.
 * If we are in 'conversion' mode and this is a convertable address
 * check if there is a route in the phaseIV database
 * Return TRUE if DECnet accepted and packet was absorbed.
 * Return FALSE if DECnet didn't take; packet is still available.
 */

static boolean clns_giveto_decnet (paktype *pak, uchar *indata,
				   boolean addresscheck)
{
    paktype *phaseIV_pak;
    dnhdrtype *dnet_hdr;
    dndatapacket *dnet_data;
    dnnet *net;
    dnroutetype *routeptr;
    uint area, node;

    if (dn_conversion && !(pak->clns_flags & CLNS_CONVERTED)) {
	/*
	 * If addresscheck is wanted, make sure this CLNS packet
	 * corresponds to a known DECNET IV address
	 */
	if (addresscheck) {
	    if (!dn_phaseIV_address_check(pak->clns_dstaddr))
		return(FALSE);
	}

	/*
	 * Convert CLNS packet to DECNET packet and pass to DECNET
	 */
	phaseIV_pak = dn_cnv_2_phaseIV_pak(pak);
	if (phaseIV_pak) {
	    dnet_hdr  = (dnhdrtype *)dnheadstart(phaseIV_pak);
	    dnet_data = (dndatapacket *)&dnet_hdr->pakstart[1];
 
 	    /*
 	     * Figure out whether we should set the intra-ethernet bit.
 	     * We do this by looking to see if there is a DECnet IV entry
 	     * for the *source* in the IV table. If there is, we know that
 	     * the destination (who is Phase IV, and on the same wire - we
 	     * are talking about the intra-ethernet bit here, remember)
 	     * will respond in Phase IV, and we would like the response
 	     * to bypass the router. So we want to preserve the
 	     * intra-ethernet bit.
 	     */
 	    net = (pak->if_input) ? dn_nets[pak->if_input->dn_network] : 
 			            dn_nets[0];
 	    area = dn_area(dnet_data->source);
 	    node = dn_node(dnet_data->source);
 
 	    if (area != net->my_dn_area)  {  		
 		if ((net->doareas) && (net->attached)) {
 		    routeptr = net->dnareas[area];
 		} else {
 		    routeptr = net->dnroutes[0];
		}
 	    } else {
 		routeptr = net->dnroutes[node];
 	    }
 	    
 	    /* 
 	     * We have an entry for the source in the IV table - set the
 	     * intra-ethernet bit in the packet before enqueueing to
 	     * Phase IV.
 	     */
 	    if (routeptr && (routeptr->type == PHASEIV))
 		dnet_data->typecode |= DN_LOCAL;
 
	    process_enqueue_pak(DNpacketQ, phaseIV_pak);
	    clns_pbuginf("\nCLNS: Enqueuing src %n to DECnet dst %n (%d.%d)",
			 pak->clns_srcaddr, pak->clns_dstaddr,
			 dn_area(dnet_data->dest), dn_node(dnet_data->dest));
	    datagram_done(pak);
	    return(TRUE);
	} 
    }
    return(FALSE);
}

/*
 * dn_giveto_clns
 * Pass a DECNET packet to CLNS
 */

static boolean dn_giveto_clns (paktype *pak, uchar *indata,
			       boolean addresscheck)
{
    dndatapacket *data;
    paktype *clns_pak;

    data = (dndatapacket *) indata;

    /* 
     * Try clns database if we are doing conversion and this
     * packet came here from the clns code.
     */
    if (dn_conversion && !(pak->clns_flags & CLNS_CONVERTED)) {
 	dnet_pktbuginf("\nDNET-PKT: Enqueuing Phase IV packet src %d.%d "
		      "dst %d.%d to Phase V",
 		       dn_area(data->source),
 		       dn_node(data->source),
 		       dn_area(data->dest),
		       dn_node(data->dest));
	/*
	 * If we are switching the packet...
	 */
	if (pak->if_input != NULL) {
	    if (dn_clns_switch(pak, data)) {
		retbuffer(pak);
		dn_stat[DNST_CONVERT_IV]++; 
		return(TRUE);
	    }
	} else {
 	    /*
	     * If we are sourcing the packet (e.g. a 'ping' packet, the
	     * packet's input interface will be NULL. In this case, we
	     * we need to call a different routine that handles packets that
	     * we source.
	     */
	    clns_pak = dn_pak_to_clns(pak, data, data->databyte);
	    if (clns_pak) {
		clns_write(clns_pak);
		dn_stat[DNST_CONVERT_IV]++; 
	    }	    
	    retbuffer(pak);
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * dn_dn2clns_neighbor
 * Add or delete a neighbor in the CLNS database
 */

static void dn_dn2clns_neighbor (paktype *pak, int herarea, int hernode,
				 ulong time)
{
    if (!dn_conversion) {
	return;
    }
    if (pak == NULL) {
	dn_delete_clns_neighbor(herarea, hernode);
    } else {
	dn_add_clns_neighbor(pak, herarea, hernode, time);
    }
}

/*
 * clns_clns2dn_neighbor
 * Add or delete a neighbor in the DECnet database
 */

static void clns_clns2dn_neighbor (boolean addflag, NSAP_T nsap,
				   idbtype *swidb, ulong time)
{
    NSAP_T key;
    uchar *station_id;
    clns_adjtype *adjacency;

    if (addflag) {
	if (dn_conversion) {
	    dn_add_phaseIVneighbor(nsap, swidb, time);
	}
    } else {
	if (!dn_conversion) {
	    key[0] = STATION_LENGTH;
	    station_id = get_station(nsap);
	    station_copy(station_id, &key[1]);
	    adjacency = find_adjacency(clns_adjacency_db, key, FALSE, swidb, 0);
	    if (adjacency) {
		adjacency->protocol_source &= ~DECNET_PHASEIV;
	    }
	}
    }
}

/*
 * dn_clear_discard_routes
 * 
 * Routine to to clear all CLNS discard routes that were created by
 * DECnet
 */
void dn_clear_discard_routes()
{
    dnnet *net;
    uchar i, j;

    for (i = 0; i < DN_MAXNETS; i++) {
        if ((net = dn_nets[i]) == NULL)
	    continue;
	
	/*
	 * Don't bother if we aren't L2 ..
	 */
	if (!net->doareas)
	    continue;

        for (j = 1; j < DN_MAXAREAS; j++) {
            dn_inform_clns(j, FALSE);
        }
    }
}


/*
 * dn_set_conversion
 * Parser routine for enabling/disabling DECNET IV to V conversion. If we
 * are turning off conversion, we need to ask CLNS to turn off all CLNS
 * prefix routes (since there might be some that would have been off because
 * a DECnet IV route went away.
 */

void dn_set_conversion (parseinfo *csb)
{
    if (!csb->sense) {
	/*
	 * Quit if conversion was already disabled
	 */
	if (!dn_conversion)
	    return;

	/*
	 * Clean up CLNS static routes that were created as a result of 
	 * having DECnet conversion on. The CLNS routine that handles this
	 * will ensure that only DECnet-created discard routes are deleted.
	 */
	dn_clear_discard_routes();

	dn_conversion = FALSE;
    }
    else {
	/*
	 * If conversion is already on, clean up CLNS static routes that
	 * were created as a result of having DECnet conversion on.
	 */
	if (dn_conversion)
    	    dn_clear_discard_routes();

	switch (GETOBJ(int,2)) {
	  case CONVERGENCE_IGRP:
	    if (dn_setup_prefix(GETOBJ(string,1))) {
		dn_conversion = TRUE;
	    } else {
		printf("\nError in DECnet conversion command");
	    }
	    break;

	  default:
	    /*
	     * Parsing the "decnet conversion <prefix>" command.
	     */	
	    if (GETOBJ(paddr,1)->clns_addr[0] > MAX_DOMAIN_LENGTH) {
	        printf("\nDECnet conversion prefix too long");
		break;
	    }
	    nsap_copy(GETOBJ(paddr,1)->clns_addr, &dn_phaseIVprefix[0]);
	    dn_conversion = TRUE;		    
	    break;
	}
    }
}

static void dn_conversion_nvgen (char *nv_command, dnnet *net)
{
    int hops, cost;
    int i;

    nv_write(dn_conversion, "%s conversion %n", nv_command, dn_phaseIVprefix);
    for (i = 1; i < DN_MAXAREAS; i++) {
	if ((net->dn_adv_area[i]).cost <= DN_MAX_AREA_COST) {
	    hops = cost = 0;
	    nv_write(TRUE, "%s advertise %d", nv_command, i);
	    
	    hops = net->dn_adv_area[i].hops;
	    cost = net->dn_adv_area[i].cost;

	    if (hops || cost) {
		nv_add(TRUE, " %d %d", hops, cost);
	    }
	}
    }
}

/*
 * dn_remove_phaseIVroute
 * Remove a Phase IV route - used by IV to V conversion support
 */

static void dn_remove_phaseIVroute (dnnet *net, uchar area)
{
    dnroutetype **table, *temproute, *thisroute;

    /* 
     * Find the right entry (in the L2 routing table).
     */
    table = net->dnareas;
    thisroute = table[area];
    temproute = NULL;

    /*
     * If no route present, or this area route is a 'normal' Phase IV area
     * route, just return.
     */
    if (!thisroute || (thisroute->type != OSI_INJECTED)) {
        net->dn_adv_area[area].cost = DN_MAX_AREA_COST + 1;
	return;
    }

    while (thisroute) {
	temproute = thisroute->nextadj;
	thisroute->nextadj = NULL;
	free(thisroute);
	thisroute = temproute;
    }
    table[area] = NULL;
    net->dn_adv_area[area].cost = DN_MAX_AREA_COST + 1;
    dnet_robuginf("\nDNET-RT:Area %d removed from routing table", area);
    
    /* trigger a L2 routing update to reflect this change */
    TRIGGER_L2_ROUTES("Area removed from routing table");
}

/*
 * decnet_trigger_L2_routes
 * 
 * Cause a DECnet triggered L2 update, as a result of a change in the
 * CLNS prefix routes table. If there are no DECnet 'advertised' routes, just
 * return. 
 * 
 */
static void decnet_trigger_L2_routes (void)
{
    boolean adv_routes_present = FALSE;
    uchar i, j;
    dnnet *net;


    /*
     * Determine if there are any DEC 'advertised' routes
     */
    for (i = 0; (i < DN_MAXNETS && !adv_routes_present); i++) {
	if ((net = dn_nets[i]) == NULL)
	    continue;
	
	/*
	 * Don't bother if we aren't L2 ..
	 */
	if (!net->doareas)
	    continue;

        for (j = 1; j < DN_MAXAREAS; j++) {
            if ((net->dn_adv_area[j]).cost <= DN_MAX_AREA_COST) {
		/*
		 * We found an area that's being advertised ..
		 */
	        adv_routes_present = TRUE;
	        break;
	    }
        }
    }

    if (adv_routes_present)
        TRIGGER_L2_ROUTES("changes in CLNS prefix routing table");

    return;
}

/*
 * dn_add_delete_advQ
 *
 * Add an entry to or delete from the DEC 'advertise' queue. The flag
 * 'add' is TRUE if we need to add, FALSE otherwise. The search key is
 * the DECnet area.
 */
static void dn_add_delete_advQ (boolean add, uchar area, ushort hops, ushort cost)
{
    decnet_advQtype *entry;
    boolean new_entry;

    new_entry = FALSE;
    for (entry = (decnet_advQtype *) decnet_advQ.qhead; entry; 
	 entry = entry->next) {
	if (area == entry->area) 
	    break;
    }
    
    if (add) {
	/*
	 * If we are adding a new entry, create an entry and enqueue to the Q.
	 * In either case, update the Q entry with hop and cost values.
	 */
	if (!entry) {
	    entry = malloc(sizeof(decnet_advQtype));
            if (!entry) 
		return;

            enqueue(&decnet_advQ, entry);
	    new_entry = TRUE;
	    entry->area = area;
	}
        entry->hops = hops;
	entry->cost = cost;
	dnet_robuginf("\nDNET-RT: %s entry for area %d in adv Q",
		      new_entry ? "Created" : "Updated", area);
    }
    else {
	/*
	 * We are deleting an entry. If we found it, get rid of it. If we
	 * didn't, just return quietly.
	 */
        if (entry) {
            unqueue(&decnet_advQ, entry);
	    dnet_robuginf("\nDNET-RT: Deleted entry for area %d from adv Q",
	                  area);
	    free(entry);
        }
    }
}

/*
 * dn_reinstate_adv_routes
 *
 * Reinstate all DECnet 'advertised' routes. Called when we start 
 * DECnet routing.
 *
 */
void dn_reinstate_adv_routes (dnnet *net)
{
    decnet_advQtype *entry;

    for (entry = (decnet_advQtype *) decnet_advQ.qhead; entry;
	 entry = entry->next) {
	dn_add_phaseIVroute(net, entry->area, entry->hops, entry->cost);
    }
}

/*
 * dn_add_phaseIVroute
 *
 * Add a Phase IV route - used by IV to V conversion support
 */
void dn_add_phaseIVroute (dnnet *net, uchar area, int hops, int cost)
{
    dnroutetype **table, *thisroute;

    /* 
     * Find the right entry (in the L2 routing table).
     */
    table = net->dnareas;
    thisroute = table[area];

    /*
     * Do not create an 'advertised' entry for our own area !
     */
    if (area == net->my_dn_area)
	return;

    /*
     * Do not overwrite native route, if it exists.
     */
    if (thisroute && (thisroute->type == PHASEIV)) {
	/*
	 * However, update the information on 'advertised' areas so
	 * that we will always have the correct value of hops and cost.
	 */
        net->dn_adv_area[area].hops = hops;
        net->dn_adv_area[area].cost = cost;

	return;
    }

    if (!thisroute) {
        thisroute = malloc(sizeof(dnroutetype));
        if (!thisroute) {
            dn_stat[DN_NOMEM]++;
	    return;
        }
        table[area] = thisroute;

        /* ensure that conversion will occur */
	thisroute->interface = NULL; 
	thisroute->hops = hops;
	thisroute->cost = cost;
	thisroute->prio = DN_DEF_PRIO;
	dn_addr(thisroute->gateway, net->my_dn_area, net->my_dn_node);
	thisroute->flags = 0;
	thisroute->flags |= DN_RT_STATE_UP;
	thisroute->type = OSI_INJECTED;
	thisroute->numroutes = 1;
	thisroute->nextsplit = NULL;
	thisroute->nextadj = NULL;    
        thisroute->areavector = NULL;
        thisroute->vector = NULL;

	/* 
	 * Indicate that this area is being advertised.
	 */
        net->dn_adv_area[area].cost = cost;
        net->dn_adv_area[area].hops = hops;

	dnet_robuginf("\nDNET-RT:Area %d entered in routing table", area);
    }
    else {
        /*
	 * There is a 'dec advertise' route..overwrite with new hops
	 * and cost information.
	 */
        if (thisroute->hops != hops) {
	    thisroute->hops = hops;
            net->dn_adv_area[area].hops = hops;
	    dnet_robuginf("\nDNET-RT: Hop count for area %d changed to %d", 
			  area, hops);
	}
	if (thisroute->cost != cost) {
	    thisroute->cost = cost;	
            net->dn_adv_area[area].cost = cost;
	    dnet_robuginf("\nDNET-RT: Cost for area %d changed to %d", 
			  area, cost);
	}

    }
    return;
}

void decnet_advertise_command (parseinfo *csb)
{
    dnnet *net;

    if ((net = dn_initnet(GETOBJ(int,1))) == NULL) {
	return;
    }

    if (!csb->sense) { 
	dn_add_delete_advQ(FALSE, GETOBJ(int,2), 0, 0);
	dn_remove_phaseIVroute(net, GETOBJ(int,2));
    } else {
	if (!dn_running || !dn_conversion)  {
	    printf("\nNeed to have DECnet running with conversion enabled");
	} else if (!net->doareas) {
	    printf("\nNeed to be a Level 2 router");
	} else if (GETOBJ(int,2) > net->dn_max_area) {
	    printf("\nSpecified area exceeds DECnet MAX AREA parameter");
	} else {
	    dn_add_delete_advQ(TRUE, GETOBJ(int,2), GETOBJ(int,3),
				   GETOBJ(int,4));
	    dn_add_phaseIVroute(net, GETOBJ(int,2), GETOBJ(int,3),
				GETOBJ(int,4));
	}
    }
}

static uchar dnconversion_decnet_dececo (idbtype *idb)
{
    if (dn_conversion && idb->clns_enabled) {
	return(DN_FUNNY_PHASEIV_HELLO_VERSION);
    } else {
	return(DN_PHASEIV_VERSION);
    }
}

static void dnconversion_show_decnet (dnnet *net)
{
    int i;
    int num_areas;

    if (dn_conversion) {
	printf("\n  Phase IV <-> Phase V Conversion Enabled");
	printf("\n\tConversion prefix is: %n", dn_phaseIVprefix);
	num_areas = 0;
	printf("\n  Areas being advertised :");
	for (i = 1; i < DN_MAXAREAS; i++) {
	    if (net->dn_adv_area[i].cost <= DN_MAX_AREA_COST) {
		printf("%3d  ", i);
		num_areas++;
		if (num_areas && (num_areas % 10) == 0) {
		    printf("\n  ");
		}
	    }
	}
    }
}

boolean dnconversion_report(void)
{
    return(dn_conversion);
}

/*
 * dncnv_init
 * DECnet conversion init routine
 */

static void dncnv_init (subsystype *subsys)
{
    reg_add_decnet_convert(LINK_CLNS, clns_giveto_decnet,"clns_giveto_decnet");
    reg_add_decnet_convert(LINK_DECNET, dn_giveto_clns, "dn_giveto_clns");
    reg_add_decnet_dn2clns_neighbor(dn_dn2clns_neighbor,"dn_dn2clns_neighbor");
    reg_add_decnet_clns2dn_neighbor(clns_clns2dn_neighbor,
				    "clns_clns2dn_neighbor");
    reg_add_nv_decnet(dn_conversion_nvgen, "dn_conversion_nvgen");
    reg_add_decnet_hello_version(dnconversion_decnet_dececo,
				 "dnconversion_decnet_dececo");
    reg_add_decnet_show(dnconversion_show_decnet, "dnconversion_show_decnet");
    reg_add_dnconversion_report(dnconversion_report, "dnconversion_report");
    reg_add_decnet_trigger_L2_routes(decnet_trigger_L2_routes,
				     "decnet_trigger_L2_routes");
    reg_add_dn_inform_clns(dn_inform_clns, "dn_inform_clns");
    reg_add_dn_add_phaseIVroute(dn_add_phaseIVroute, "dn_add_phaseIVroute");
    reg_add_dn_check_clns_prefix(dn_check_clns_prefix, "dn_check_clns_prefix");
    reg_add_dn_clear_discard_routes(dn_clear_discard_routes,
                                    "dn_clear_discard_routes");
    reg_add_dn_reinstate_adv_routes(dn_reinstate_adv_routes, 
				    "dn_reinstate_adv_routes");
    dncnv_parser_init();
}

/*
 * The DECnet IV-V conversion subsystem header
 */

#define DECIV2V_MAJVERSION 1
#define DECIV2V_MINVERSION 0
#define DECIV2V_EDITVERSION  1

SUBSYS_HEADER(decnet_iv2v,
	      DECIV2V_MAJVERSION, DECIV2V_MINVERSION, DECIV2V_EDITVERSION,
	      dncnv_init, SUBSYS_CLASS_PROTOCOL,
	      "req: decnet, clns",
	      NULL);
