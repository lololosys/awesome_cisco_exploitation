/* $Id: decnet_media.c,v 3.4.26.7 1996/09/09 22:11:22 kleung Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet_media.c,v $
 *------------------------------------------------------------------
 * decnet_media.c -- media dependent routines for DECnet
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: decnet_media.c,v $
 * Revision 3.4.26.7  1996/09/09  22:11:22  kleung
 * CSCdi67780:  Token ring will not pass Decnet neighboring routes
 * Branch: California_branch
 *
 * Revision 3.4.26.6  1996/07/23  18:44:24  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.4.26.5  1996/07/18  18:15:12  tylin
 * CSCdi62693:  Command cdp neighbors detail generates alignment
 * corrections (IPX)
 *    Fix decnet_sprintf_address().
 * Branch: California_branch
 *
 * Revision 3.4.26.4  1996/07/09  05:48:38  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.4.26.3  1996/05/17  10:44:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.14.3  1996/05/05  22:54:34  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.14.2  1996/04/26  15:16:57  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.14.1  1996/04/03  13:58:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.26.2  1996/05/04  00:36:18  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Revision 3.4.26.1  1996/03/18  19:11:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/13  01:19:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.1  1996/03/07  08:42:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/18  20:52:18  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  08:58:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:16:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/07  05:30:26  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *
 * Revision 2.3  1995/07/07  23:04:35  asastry
 * CSCdi33292:  Access list for DECnet DDR dialer map does not function
 * properly when padding is present.
 *
 * Revision 2.2  1995/07/06  22:59:26  achopra
 * CSCdi32586:  dynamic frame relay map address for dec is incorrect.
 *
 * Revision 2.1  1995/06/07  20:22:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "decnet_externs.h"
#include "../if/network.h"
#include "../if/static_map.h"
#include "../if/rif_inline.h"
#include "../wan/smds.h"
#include "../x25/x25.h"
#include "../smf/smf_public.h"
#include "../srt/srt_registry.h"


/*
 * These constants are used to build node specific DECnet MAC addresses.
 * The "e" is for Ethernet, the "t" is for Token Ring.  Note that Token Ring
 * MAC addresses are bit reversed with respect to Ethernet addresses.
 * The Token Ring (4Mb/s) interface, Netronix, doesn't handle more than
 * one group address so we use two bits in the Token Ring functional.
 *
 * Strictly speaking we don't need to do the bit swapping on Token Ring
 * because we can at this point do whatever we want.  But it is closer to
 * what should really be there.
 */

uchar dn_t_gwaddr[IEEEBYTES];	/* idb->encaps variable for Token Ring */
uchar dn_e_baseaddr[IEEEBYTES]           = {0xAA, 0x00, 0x04, 0x00, 0x00, 0x00};
uchar dn_t_baseaddr[IEEEBYTES]           = {0x55, 0x00, 0x20, 0x00, 0x00, 0x00};
uchar dn_multi_e_router_l2[IEEEBYTES]    = {0x09, 0x00, 0x2b, 0x02, 0x00, 0x00};
uchar dn_multi_e_router_l1[IEEEBYTES]    = {0xAB, 0x00, 0x00, 0x03, 0x00, 0x00};
uchar dn_multi_e_node[IEEEBYTES]         = {0xAB, 0x00, 0x00, 0x04, 0x00, 0x00};
uchar dn_multi_e_prime_router[IEEEBYTES] = {0x09, 0x00, 0x2b, 0x02, 0x01, 0x0a};
uchar dn_multi_t_router_cisco[IEEEBYTES] = {0xC0, 0x00, 0x40, 0x00, 0x00, 0x00};
uchar dn_multi_t_node_cisco[IEEEBYTES]   = {0xC0, 0x00, 0x20, 0x00, 0x00, 0x00};
uchar dn_multi_t_router_dec[IEEEBYTES]   = {0xC0, 0x00, 0x10, 0x00, 0x00, 0x00};
uchar dn_multi_t_node_dec[IEEEBYTES]     = {0xC0, 0x00, 0x08, 0x00, 0x00, 0x00};


/*
 * decnet_build_address
 *
 * Build an addrtype data structure containing the DECNET address
 * currently in use on this interface.
 */
static void decnet_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{

    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_DECNET;
    addr->length = ADDRLEN_DECNET;
    addr->decnet_addr = dn_nets[swidb->dn_network]->my_dn_address;
}

/*
 * decnet_sprintf_address
 *
 * Take an addrtype data structure and print it as an DECnet address.
 */
static void decnet_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%d.%d", (addr->decnet_addr >> 10),
			     (addr->decnet_addr & 0x3ff));
}

/*
 * decnet_sprintf_src_dst
 *
 * Given a packet, extract and print a DECnet address.
 */
static void decnet_sprintf_src_dst (
    char *string,
    paktype *inpak,
    enum SRCDST which)
{
    dndatapacket *pak;
    dnhdrtype *dnhdr;
    int firstbyte;
    uchar *data;

    dnhdr = (dnhdrtype *)dnheadstart(inpak);
    data = dnhdr->pakstart;

    firstbyte = *data;
    if (firstbyte & DN_PAD) {   
	int padcount;
	 
        padcount = firstbyte & 0x7f;
	data += padcount;
    }

    pak = (dndatapacket *)data;
    if (which == DESTINATION)
	sprintf(string, "%d.%d", dn_area(pak->dest), dn_node(pak->dest));
    else
        sprintf(string, "%d.%d", dn_area(pak->source), dn_node(pak->source));
}

/*
 * decnet_valid_access_list
 */
static boolean decnet_valid_access_list (int number)
{
    return((number >= MINDECACC) && (number <= MAXDECACC));
}

/*
 * decnet_macaddrs_update
 *
 * Called (via reg_invoke_media_interesting_addresses) when a
 * change in the listening state of an interface has taken place.
 * The interface is listening either selectively or promiscuously.
 *
 * If this protocol is configured on the interface, update the SMF
 * so that the protocol-specific MAC addresses are either inserted
 * (the interface is listening promiscuously) or deleted (the
 * interface is listening selectively).  In addition, populate
 * any hardware address filters with the protocol-specific MAC
 * addresses.
 *
 * If this protocol is NOT configured on the interface, simply
 * delete the protocol-specific MAC addresses from the SMF if
 * they are present.
 */
static void decnet_macaddrs_update (idbtype *idb)
{
    hwidbtype *hwidb;
    uchar dn_multi_t_router_dec_swap[IEEEBYTES];

    hwidb = idb->hwptr;
    ieee_swap(dn_multi_t_router_dec, dn_multi_t_router_dec_swap);

    if (decnet_on_idb(idb)) {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_update(dn_multi_e_router_l1,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(dn_multi_e_router_l2,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(dn_multi_e_node,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(dn_multi_e_prime_router,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);

	    reg_invoke_bvi_smf_update(dn_multi_t_router_dec_swap,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    reg_invoke_tbridge_smf_update(dn_multi_t_router_dec_swap,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, dn_multi_t_router_dec_swap);
	} else {
	    reg_invoke_tbridge_smf_update(dn_multi_e_router_l1,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(dn_multi_e_router_l2,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(dn_multi_e_node,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(dn_multi_e_prime_router,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, dn_multi_e_router_l1);
	    if_multi_add(hwidb, idb, dn_multi_e_router_l2);
	    if_multi_add(hwidb, idb, dn_multi_e_node);
	    if_multi_add(hwidb, idb, dn_multi_e_prime_router);
	}
    } else {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_delete(dn_multi_e_router_l1,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(dn_multi_e_router_l2,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(dn_multi_e_node,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(dn_multi_e_prime_router,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(dn_multi_t_router_dec_swap,
				      idb,
				      SMF_MACTYP_DECNET_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    reg_invoke_tbridge_smf_delete(dn_multi_t_router_dec_swap,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	} else {
	    reg_invoke_tbridge_smf_delete(dn_multi_e_router_l1,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(dn_multi_e_router_l2,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(dn_multi_e_node,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(dn_multi_e_prime_router,
					  idb,
					  SMF_MACTYP_DECNET_MCAST |
					  SMF_ACTION_RECEIVE);
	}
    }
}

/*
 * decnet_ether_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DECNET
 */
static boolean decnet_ether_macaddr (paktype *pak, long address, int *enctype,
				     uchar **daddr, uchar const **other_info)
{
    dnroutetype *route;

    route = (dnroutetype *)address;
    *daddr = (uchar *)route->snpa.addr;

    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * decnet_iv_prime_router_ether_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DECNET_PRIME_ROUTER
 */
static boolean decnet_iv_prime_router_ether_macaddr (paktype *pak, 
					    long address, int *enctype, 
					    uchar **daddr,
					    uchar const **other_info)
{
    *daddr = dn_multi_e_prime_router;
    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * decnet_router_L1_ether_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DECNET_ROUTER_L1
 */
static boolean decnet_router_L1_ether_macaddr (paktype *pak, long address,
					    int *enctype, uchar **daddr,
					    uchar const **other_info)
{
    *daddr = dn_multi_e_router_l1;
    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * decnet_router_L2_ether_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DECNET_ROUTER_L2
 */
static boolean decnet_router_L2_ether_macaddr (paktype *pak, long address,
					    int *enctype, uchar **daddr,
					    uchar const **other_info)
{
    *daddr = dn_multi_e_router_l2;
    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * decnet_node_ether_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DECNET_NODE
 */
static boolean decnet_node_ether_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info)
{
    *daddr = dn_multi_e_node;
    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * decnet_token_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_DECNET
 */
static boolean decnet_token_macaddr (paktype *pak, long address, int *enctype,
				     uchar **daddr, uchar const **other_info,
				     ulong *rif_flags)
{
    idbtype *interface;
    dnroutetype *route;

    interface = pak->if_output;
    route = (dnroutetype *)address;

    *daddr = (uchar *)route->snpa.addr;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);

    return(TRUE);
}

/*
 * decnet_iv_prime_router_token_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_DECNET_PRIME_ROUTER
 */
static boolean decnet_iv_prime_router_token_macaddr (paktype *pak, 
						     long address,
						     int *enctype, 
						     uchar **daddr,
						     uchar const **other_info,
						     ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    *daddr = (uchar *)&hwidb->dn_func->dn_tr_iv_prime_routers;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_router_token_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_DECNET_ROUTER_L1
 */
static boolean decnet_router_token_macaddr (paktype *pak, long address,
					    int *enctype, uchar **daddr,
					    uchar const **other_info,
					    ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    *daddr = (uchar *)&hwidb->dn_func->dn_tr_iv_routers;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_node_token_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_DECNET_NODE
 */
static boolean decnet_node_token_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info,
					  ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    *daddr = (uchar *)&hwidb->dn_func->dn_tr_iv_endnodes;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_fddi_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DECNET
 */
static boolean decnet_fddi_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    idbtype *interface;
    dnroutetype *route;

    interface = pak->if_output;
    route = (dnroutetype *)address;
    *daddr = (uchar *)route->snpa.addr;

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);

    return(TRUE);
}


/*
 * decnet_router_fddi_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DECNET_PRIME_ROUTER
 */
static boolean decnet_iv_prime_router_fddi_macaddr (paktype *pak, long address,
				  	           int *enctype, uchar **daddr,
					           uchar const **other_info,
						    ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = dn_multi_e_prime_router;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}


/*
 * decnet_router_fddi_L1_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DECNET_ROUTER_L1
 */
static boolean decnet_router_L1_fddi_macaddr (paktype *pak, long address,
					    int *enctype, uchar **daddr,
					    uchar const **other_info,
					      ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = dn_multi_e_router_l1;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_router_L2_fddi_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DECNET_ROUTER_L2
 */
static boolean decnet_router_L2_fddi_macaddr (paktype *pak, long address,
					    int *enctype, uchar **daddr,
					    uchar const **other_info,
					      ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = dn_multi_e_router_l2;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_node_fddi_macaddr
 * 
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DECNET_NODE
 */
static boolean decnet_node_fddi_macaddr (paktype *pak, long address,
					 int *enctype, uchar **daddr,
					 uchar const **other_info,
					 ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = dn_multi_e_node;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_DECNET);
    return(TRUE);
}

/*
 * decnet_smds_macaddr
 *
 * Turn a DECnet address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_DECNET, 
 * LINK_DECNET_NODE, LINK_DECNET_ROUTER_L1 and LINK_DECNET_ROUTER_L2.
 */
static boolean decnet_smds_macaddr (paktype *pak, long address, int *enctype,
				    uchar const **daddr,
				    uchar const **other_info,
				    addrtype *proto_addr)
{
    ushort dec_addr;
    boolean broadcast;

    if (address) {
	dec_addr =  GET_DECNET_ADDR(address);
    }
    else
	dec_addr = 0;
    
    broadcast = (dec_addr == *(ushort *)baddr);
    if ((pak->linktype == LINK_DECNET) && !broadcast) {
	proto_addr->type = ADDR_DECNET;
	proto_addr->length = ADDRLEN_DECNET;
	proto_addr->decnet_addr = dec_addr;
	*daddr = NULL;
    } else {
	*daddr = baddr;
    }
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * decnet_x25_macaddr
 *
 * Turn a DECnet address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_DECNET.
 */
static boolean decnet_x25_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, lcitype **lci,
				   boolean *broadcast)
{

    if (address != 0) {
	proto_addr->type = ADDR_DECNET;
	proto_addr->length = ADDRLEN_DECNET;
	proto_addr->decnet_addr = GET_DECNET_ADDR(address);
        *broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * decnet_frame_macaddr
 *
 * Turn a DECnet address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_DECNET.
 */
static boolean decnet_frame_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    if (address != 0) {
	proto_addr->type = ADDR_DECNET;
	proto_addr->length = ADDRLEN_DECNET;
	proto_addr->decnet_addr = GET_DECNET_ADDR(address);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * decnet_ddr_macaddr
 *
 * Turn a DECnet address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_DECNET.
 */
static boolean decnet_ddr_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    if (address != 0) {
	proto_addr->type = ADDR_DECNET;
	proto_addr->length = ADDRLEN_DECNET;
	proto_addr->decnet_addr = GET_DECNET_ADDR(address);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_DECNET;
    return(TRUE);
}

/*
 * decnet_atm_macaddr
 *
 * Turn a DECnet address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_DECNET.
 */
static boolean
decnet_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		    uchar const **snap_info, staticmap_type **map, 
		    boolean *broadcast)
{
    addrtype proto_addr;

    *snap_info = zerosnap;
    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = FALSE;
    if (address != 0) {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_DECNET;
	proto_addr.length = ADDRLEN_DECNET;
	proto_addr.decnet_addr = GET_DECNET_ADDR(address);
    } else {
	*broadcast = TRUE;
    }
    pak->linktype = LINK_DECNET;

    if ((*map = staticmap_lookup(pak->if_output, &proto_addr, LINK_DECNET,
                                                         ET_ATM, *broadcast)))
        return(TRUE);
    else
        return(FALSE);
}

/*
 * decnet_media_registry
 * Register media dependent code for DECnet
 */

void decnet_media_registry (void)
{
    reg_add_proto_address(LINK_DECNET,
		      decnet_build_address, "decnet_build_address");
    reg_add_proto_address(LINK_DECNET_ROUTER_L1,
		      decnet_build_address, "decnet_build_address");
    reg_add_proto_address(LINK_DECNET_ROUTER_L2,
		      decnet_build_address, "decnet_build_address");
    reg_add_proto_address(LINK_DECNET_PRIME_ROUTER,
		      decnet_build_address, "decnet_build_address");
    reg_add_proto_address(LINK_DECNET_NODE,
		      decnet_build_address, "decnet_build_address");
    reg_add_sprintf_address_link(LINK_DECNET,
		      decnet_sprintf_address, "decnet_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_DECNET,
		      decnet_sprintf_address, "decnet_sprintf_address");
    reg_add_sprintf_src_dst(LINK_DECNET,
		      decnet_sprintf_src_dst, "decnet_sprintf_src_dst");
    reg_add_access_check(LINK_DECNET, decnet_data_accesscheck,
			 "decnet_data_accesscheck");
    reg_add_valid_access_list(LINK_DECNET,
		      decnet_valid_access_list, "decnet_valid_access_list");
    reg_add_media_interesting_addresses(decnet_macaddrs_update,
					"decnet_macaddrs_update");
    reg_add_media_ether_macaddr(LINK_DECNET, decnet_ether_macaddr,
				"decnet_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_DECNET_NODE, decnet_node_ether_macaddr,
				"decnet_node_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_DECNET_ROUTER_L1,
 				decnet_router_L1_ether_macaddr,
 				"decnet_router_L1_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_DECNET_ROUTER_L2,
 				decnet_router_L2_ether_macaddr,
 				"decnet_router_L2_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_DECNET_PRIME_ROUTER,
				decnet_iv_prime_router_ether_macaddr,
				"decnet_iv_prime_router_ether_macaddr");

    reg_add_media_token_macaddr(LINK_DECNET, decnet_token_macaddr,
				"decnet_token_macaddr");
    reg_add_media_token_macaddr(LINK_DECNET_NODE, decnet_node_token_macaddr,
				"decnet_node_token_macaddr");
    reg_add_media_token_macaddr(LINK_DECNET_ROUTER_L1,
 				decnet_router_token_macaddr,
 				"decnet_router_token_macaddr");
    reg_add_media_token_macaddr(LINK_DECNET_ROUTER_L2,
 				decnet_router_token_macaddr,
  				"decnet_router_token_macaddr");
    reg_add_media_token_macaddr(LINK_DECNET_PRIME_ROUTER,
				decnet_iv_prime_router_token_macaddr,
				"decnet_iv_prime_router_token_macaddr");

    reg_add_media_fddi_macaddr(LINK_DECNET, decnet_fddi_macaddr,
			       "decnet_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_DECNET_NODE, decnet_node_fddi_macaddr,
			       "decnet_node_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_DECNET_ROUTER_L1, 
 			       decnet_router_L1_fddi_macaddr,
 			       "decnet_router_L1_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_DECNET_ROUTER_L2, 
 			       decnet_router_L2_fddi_macaddr,
 			       "decnet_router_L2_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_DECNET_PRIME_ROUTER,
				decnet_iv_prime_router_fddi_macaddr,
				"decnet_iv_prime_router_fddi_macaddr");

    reg_add_media_smds_macaddr(LINK_DECNET, decnet_smds_macaddr,
			       "decnet_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_DECNET_NODE, decnet_smds_macaddr,
			       "decnet_bcast_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_DECNET_ROUTER_L1, decnet_smds_macaddr,
 			       "decnet_bcast_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_DECNET_ROUTER_L2, decnet_smds_macaddr,
			       "decnet_bcast_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_DECNET_PRIME_ROUTER, decnet_smds_macaddr,
				"decnet_bcast_smds_macaddr");

    reg_add_media_x25_macaddr(LINK_DECNET, decnet_x25_macaddr,
			      "decnet_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_DECNET_NODE, decnet_x25_macaddr,
			      "decnet_bcast_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_DECNET_ROUTER_L1, decnet_x25_macaddr,
 			      "decnet_bcast_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_DECNET_ROUTER_L2, decnet_x25_macaddr,
			      "decnet_bcast_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_DECNET_PRIME_ROUTER, decnet_x25_macaddr,
			      "decnet_bcast_x25_macaddr");

    reg_add_media_frame_macaddr(LINK_DECNET, decnet_frame_macaddr,
				"decnet_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_DECNET_NODE, decnet_frame_macaddr,
				"decnet_bcast_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_DECNET_ROUTER_L1, decnet_frame_macaddr,
				"decnet_bcast_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_DECNET_ROUTER_L2, decnet_frame_macaddr,
				"decnet_bcast_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_DECNET_PRIME_ROUTER, decnet_frame_macaddr,
				"decnet_bcast_frame_macaddr");

    reg_add_media_ddr_macaddr(LINK_DECNET, decnet_ddr_macaddr,
				"decnet_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_DECNET_NODE, decnet_ddr_macaddr,
				"decnet_bcast_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_DECNET_ROUTER_L1, decnet_ddr_macaddr,
 				"decnet_bcast_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_DECNET_ROUTER_L2, decnet_ddr_macaddr,
				"decnet_bcast_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_DECNET_PRIME_ROUTER, decnet_ddr_macaddr,
				"decnet_bcast_ddr_macaddr");

    reg_add_media_atm_macaddr(LINK_DECNET, decnet_atm_macaddr,
			      "decnet_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_DECNET_NODE, decnet_atm_macaddr,
			      "decnet_bcast_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_DECNET_ROUTER_L1, decnet_atm_macaddr,
			      "decnet_bcast_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_DECNET_ROUTER_L2, decnet_atm_macaddr,
			      "decnet_bcast_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_DECNET_PRIME_ROUTER, decnet_atm_macaddr,
			      "decnet_bcast_atm_macaddr");

    reg_add_media_change_mtu(dn_change_mtu_size, "dn_change_mtu_size");
}
