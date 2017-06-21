/* $Id: novell_media.c,v 3.2.62.5 1996/08/27 23:34:02 hou Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_media.c,v $
 *------------------------------------------------------------------
 * novell_media.c -- media dependent Novell routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_media.c,v $
 * Revision 3.2.62.5  1996/08/27  23:34:02  hou
 * llCVS:
 * ----------------------------------------------------------------------
 *
 * CSCdi66323:  Routers ISL encapsultaion on CGMP messages is incorrect
 * Branch: California_branch
 * - Clean up the ISL vLAN code so that we can do ISL encapsulation on
 *   different type packet not only ARPA type, it's NOVELL_ETHER for
 *   IPX routing.
 *
 * Revision 3.2.62.4  1996/07/23  18:49:57  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.2.62.3  1996/04/27  06:47:38  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.2  1996/04/26  00:11:25  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.20.1  1996/02/23  21:33:32  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.2.62.2  1996/04/10  04:23:10  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 *
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.2.62.1  1996/03/18  22:53:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  02:14:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  11:22:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  22:00:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:23:10  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:39:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/07  05:45:08  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/07/07  04:30:29  bbenson
 * CSCdi35019:  Fast/SSE switching of LANE IPX SAP encaps not working.
 * - Split out writing of lane encaps from nov_write_hes_aip_header.
 * - Allow same enctype on subinterfaces of non-broadcast media (AIP).
 * - Set uninitialized pak->encsize in novell_et_novell_ether.
 *
 * Revision 2.1  1995/06/07  23:28:43  hampton
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
#include "../wan/dialer_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "xns.h"
#include "novell.h"
#include "../if/static_map.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../x25/x25.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "ipx_registry.h"


/*
 *
 */
static const uchar novell_llc[SAP_HDRBYTES] = {SAP_NOVELL, SAP_NOVELL, LLC1_UI};


/*
 * ipx_nexthop_network 
 *
 * returns the next hop network number which should be a local network
 * number, in all but NLSPs case. In NLSP the next hop is
 * peer's Internal-Network number.
 * If IPXWAN is running this number is stored in the novell idb.
 */
ulong ipx_nexthop_network (paktype *pak)
{
    ulong net;
    net = 0;

    if (is_p2p(pak->if_output) &&
	(pak->nif_output->nov_lsp_enabled &&
	 !pak->nif_output->nov_lsp_isis)) {
	if (nov_ipxwan_on_nidb(pak->nif_output) &&
	    (pak->nif_output->nov_ipxwan_routing_type ==
	     NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)) {
	    net = pak->nif_output->nov_ipxwan_remote_internal_net;
	} else {
	    net = reg_invoke_ipx_nlsp_get_peer_network(pak);
	    if (net == 0L) {
		net = pak->nif_output->novellnet;
	    }
	}
    } else {
	net = pak->nif_output->novellnet;
    }
    return(net);
}

/*
 * novell_build_address
 *
 * Build an addrtype data structure containing the NOVELL address
 * currently in use on this interface.
 */
static void novell_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_NOVELL;
    addr->length = ADDRLEN_NOVELL;

    /*
     * If 'pak' is NULL, use net corresponding to primary nidb.
     */
    if (pak) 
        addr->nov_addr.net = ((nidbtype *)pak->nif_output)->novellnet;
    else {
	if (swidb->primary_nidb)
	    addr->nov_addr.net = ((nidbtype *)swidb->primary_nidb)->novellnet;
	else
	    addr->nov_addr.net = 0L;
    }

    ieee_copy(swidb->novell_node, addr->nov_addr.host);
}

/*
 * novell_build_packet_address
 *
 * Given a packet, build a addrtype data structure of the source/dest address.
 * The memset zero is important!
 */
static void novell_build_packet_address (
    paktype *pak,
    addrtype *addr,
    enum SRCDST which)
{
    ipxhdrtype *ipx = (ipxhdrtype *) ipxheadstart(pak);
    
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_NOVELL;
    addr->length = ADDRLEN_NOVELL;
    if (which == SOURCE) {
	addr->nov_addr.net = GET2WD(ipx->ssnet);
	ieee_copy((uchar *) ipx->shost, addr->nov_addr.host);
    } else {
	addr->nov_addr.net = GET2WD(ipx->ddnet);
	ieee_copy((uchar *) ipx->dhost, addr->nov_addr.host);
    }
}

/*
 * novell_sprintf_address
 *
 * Take an addrtype data structure and print it as an NOVELL address.
 */
static void novell_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%x.%e", GET2WD(&addr->nov_addr.net), addr->nov_addr.host);
}

/*
 * novell_sprintf_src_dst
 *
 * Given a packet, extract and print an NOVELL address.
 */
static void novell_sprintf_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (which == SOURCE)
	sprintf(string, "%x.%e", GET2WD(ipx->ssnet), ipx->shost);
    else
        sprintf(string, "%x.%e", GET2WD(ipx->ddnet), ipx->dhost);
}

/*
 * novell_valid_access_list
 */
static boolean novell_valid_access_list (int number)
{
    return((number >= MINNOVACC) && (number <= MAXNOVSAPACC));
}

/*
 * novell_ether_macaddr
 *
 * Turn a Novell address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_NOVELL
 */
static boolean novell_ether_macaddr (paktype *pak, long address, int *enctype,
				     uchar **daddr, uchar const **other_info)
{
    *daddr = (uchar *)address;
    if (pak->enctype == ET_ISL)
        *enctype = ET_NOVELL_ETHER;
    else
        *enctype = pak->enctype;
    if (pak->enctype == ET_SAP)
	*other_info = novell_llc;
    else
	*other_info = zerosnap;
    return(TRUE);
}

/*
 * novell_tunnel_macaddr
 * For multi-point GRE tunnels, determine IP address to use in the
 * underlying payload network.
 */

static boolean novell_tunnel_macaddr (paktype *pak, long address,
				      addrtype *proto_addr, boolean *broadcast)
{
    uchar const *macaddr;
    ipaddrtype dest;
    
    if (is_ieee_bcast((uchar *)address)) {
	*broadcast = TRUE;
	return(FALSE);
    }
    *broadcast = FALSE;
    if (!reg_invoke_ipxnhrp_macaddr(pak, address, &macaddr))
	return(FALSE);
    dest = *((ipaddrtype *)macaddr);
    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = dest;
    return(TRUE);
}

/*
 * novell_et_novell_ether
 *
 * Do the funky encapsulation that Novell uses on Ethernets.
 * Called by MEDIA_ETHER_MACWRITE, indexed by ET_NOVELL_ETHER
 */
static boolean novell_et_novell_ether (paktype *pak, hwidbtype *hwidb,
				       int linktype, char *daddr,
				       char const *other_info)
{
    ether_novell *novell;

    pak->datagramstart -= sizeof(ether_novell);
    novell = (ether_novell *)pak->datagramstart;
    ieee_copy(daddr, novell->daddr);
    ieee_copy(hwidb->hardware, novell->saddr);
    novell->length = pak->datagramsize;
    novell->data[0] = 0xFFFF;		/* trash XNS CRC */
    pak->datagramsize += sizeof(ether_novell);

    pak->mac_start = (uchar *)novell;
    pak->addr_start = novell->daddr;
    pak->rif_start = NULL;
    pak->riflen	= 0;
    pak->info_start = NULL;
    pak->encsize = ETHER_ARPA_ENCAPBYTES;
    return(TRUE);
}

/*
 * novell_et_novell_fddi
 *
 * Do the funky encapsulation that Novell uses on FDDI.
 */
static boolean novell_et_novell_fddi (paktype *pak, hwidbtype *hwidb,
				      int linktype,
				      char *daddr, char *saddr,
				      char const *other_info, riftype *rifptr)
{
    fddi_novell *fddi;
    int riflen = 0;

    if (rifptr != NULL) {
	riflen = rifptr->length;
    }

    pak->datagramstart -= (sizeof(fddi_novell) + riflen);
    pak->datagramsize += (sizeof(fddi_novell) + riflen);

    fddi = (fddi_novell *)pak->datagramstart;
    
    ieee_copy(daddr, fddi->daddr);
    ieee_copy(saddr, fddi->saddr);
    if (riflen > 0) {
	if (hwidb->type == IDBTYPE_FDDIT)
	    fddi->saddr[0] |= FDDIT_RII;
	else
	    fddi->saddr[0] |= FDDI_RII;	/* indicate we are here */
	bcopy(rifptr->rif, fddi->data, riflen);
    }

    (ushort)fddi->data[riflen] = 0xFFFF;	/* No checksum */

    pak->mac_start  = (uchar *)fddi;
    pak->addr_start = fddi->daddr;
    pak->rif_start  = (riflen ? (uchar *)fddi->data : NULL);
    pak->riflen     = riflen;
    pak->info_start = (uchar *)fddi->data;
    pak->encsize    = sizeof(fddi_novell) + riflen;
    return(TRUE);
}

/*
 * novell_token_macaddr
 *
 * Turn a Novell address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_NOVELL
 */
static boolean novell_token_macaddr (paktype *pak, long address, int *enctype,
				     uchar **daddr, uchar const **other_info,
				     ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    switch (pak->enctype) {
      case ET_SNAP:
	*enctype = ET_SNAP;
	*other_info = zerosnap;
	break;

      case ET_SAP:
	*enctype = ET_SAP;
	*other_info = novell_llc;
	break;

      default:
	return(FALSE);
    }

    *daddr = (uchar *)address;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_NOVELL);
    return(TRUE);
}

/*
 * novell_fddi_macaddr
 *
 * Turn a Novell address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_NOVELL
 */
static boolean novell_fddi_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *enctype = pak->enctype;
    *daddr = (uchar *)address;
    if (pak->enctype == ET_SAP)
	*other_info = novell_llc;
    else
	*other_info = zerosnap;

    *rif_flags = determine_rif(interface, MULTIRING_BIT_NOVELL);
    return(TRUE);
}

/*
 * novell_smds_macaddr
 *
 * Turn an NOVELL address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_NOVELL.
 */
static boolean novell_smds_macaddr (paktype *pak, long address, int *enctype,
				    uchar const **daddr,
				    uchar const **other_info,
				    addrtype *proto_addr)
{
    if (!is_ieee_bcast((uchar *)address)) {
	if (!reg_invoke_ipxnhrp_macaddr(pak, address, daddr)) {
	    proto_addr->type = ADDR_NOVELL;
	    proto_addr->length = ADDRLEN_NOVELL;
	    proto_addr->xns_addr.net = ipx_nexthop_network(pak);
	    ieee_copy((uchar *)address, proto_addr->xns_addr.host);
	    *daddr = NULL;
	}
    } else {
	*daddr = baddr;
    }

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * novell_x25_macaddr
 *
 * Turn an NOVELL address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_NOVELL.
 */
static boolean novell_x25_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, lcitype **lci,
				   boolean *broadcast)
{
    if (!is_ieee_bcast((uchar *)address)) {
	proto_addr->type = ADDR_NOVELL;
	proto_addr->length = ADDRLEN_NOVELL;
	proto_addr->nov_addr.net = ipx_nexthop_network(pak);
	ieee_copy((char *)address, proto_addr->nov_addr.host);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * novell_frame_macaddr
 *
 * Turn an NOVELL address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_NOVELL.
 */
static boolean novell_frame_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    if (!is_ieee_bcast((uchar *)address)) {
	proto_addr->type = ADDR_NOVELL;
	proto_addr->length = ADDRLEN_NOVELL;
	proto_addr->nov_addr.net = ipx_nexthop_network(pak);
	ieee_copy((char *)address, proto_addr->nov_addr.host);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * novell_ddr_macaddr
 *
 * Turn an NOVELL address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_NOVELL.
 */
static boolean novell_ddr_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    /*
     * Need to allow the address structure to be filled in for both
     * broadcast and non-broadcast packets.  This is so older dialer
     * maps that mapped the broadcast address still work.
     */
    proto_addr->type = ADDR_NOVELL;
    proto_addr->length = ADDRLEN_NOVELL;
    proto_addr->nov_addr.net = ipx_nexthop_network(pak);
    ieee_copy((char *)address, proto_addr->nov_addr.host);

    if (!is_ieee_bcast((uchar *)address)) {
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_NOVELL;
    return(TRUE);
}

/*
 * novell_atm_macaddr
 *
 * Turn an NOVELL address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_NOVELL.
 */
static boolean novell_atm_macaddr (paktype *pak, long address, 
				    hwaddrtype *daddr, uchar const **snap_info,
				     staticmap_type **map, boolean *broadcast)
{
    addrtype proto_addr;
    ipxhdrtype *ipx;
    boolean check_map;

    *snap_info = zerosnap;
    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = FALSE;
    if (!is_ieee_bcast((uchar *)address)) {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_NOVELL;
	proto_addr.length = ADDRLEN_NOVELL;
	proto_addr.nov_addr.net = ipx_nexthop_network(pak);
	ieee_copy((char *)address, proto_addr.nov_addr.host);
    } else {
	*broadcast = TRUE;
    }
    *map = staticmap_lookup(pak->if_output, &proto_addr, LINK_NOVELL,
			    ET_ATM, *broadcast);
    /*
     * Attempt dynamic resolution?
     */
    if (*map == NULL) {
	reg_invoke_media_macaddr_map(pak, &proto_addr, *broadcast, map);
	return(*map != NULL);
    }
    if (*broadcast)
	return(*map != NULL);
    if (pak->linktype == LINK_NHRP) {
	check_map = TRUE;
    } else {
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	check_map = ((proto_addr.nov_addr.net != GET2WD(ipx->ddnet)) ||
		     !ieee_equal(proto_addr.nov_addr.host, ipx->dhost));
    }
    if (check_map)
	reg_invoke_media_macaddr_map(pak, &proto_addr, *broadcast, map);
    return(*map != NULL);
}

/*
 * novell_ether_mtu
 *
 * Return the IPX MTU on an ethernet interface given its encapsulation.
 * Note that this is ineffective with secondaries!  This will only work
 * properly if multiple encapsulations are supported using subinterfaces.
 * Currently the only user of this service is NLSP, so we're OK.  But beware!
 */
static int novell_ether_mtu (idbtype *swidb, boolean get_default)
{
    nidbtype *nidb;
    nidb = swidb->primary_nidb;
    if (nidb) {
	switch (nidb->enctype) {
	  case ET_SNAP:
	    return(swidb->hwptr->maxmtu - ET_SNAP_LEN);
	  case ET_SAP:
	    return(swidb->hwptr->maxmtu - ET_SAP_LEN);
	  case ET_NOVELL_ETHER:
	  case ET_ARPA:
	  default:
	    return(swidb->hwptr->maxmtu);
	}
    } else {
	return(swidb->hwptr->maxmtu);
    }
}

/*
 * novell_fddi_tring_mtu
 *
 * Return the IPX MTU on an FDDI or tring interface given its encapsulation.
 * Note that this is ineffective with secondaries!  This will only work
 * properly if multiple encapsulations are supported using subinterfaces.
 * Currently the only user of this service is NLSP, so we're OK.  But beware!
 *
 * Note that the MTU is biased for SNAP.
 */
static int novell_fddi_tring_mtu (idbtype *swidb, boolean get_default)
{
    nidbtype *nidb;
    nidb = swidb->primary_nidb;
    if (nidb) {
	switch (nidb->enctype) {
	  case ET_NOVELL_FDDI:
	    return(swidb->hwptr->maxmtu + ET_SNAP_LEN);
	  case ET_SAP:
	    return(swidb->hwptr->maxmtu + ET_SNAP_LEN - ET_SAP_LEN);
	  case ET_SNAP:
	  default:
	    return(swidb->hwptr->maxmtu);
	}
    } else {
	return(swidb->hwptr->maxmtu);
    }
}

/*
 * novell_set_dialer_list
 * Set novell dialer-list used by fastswitching code.
 */
static void novell_set_dialer_list (dialerplisttype *diallst, int list)
{
    diallst->ipx_dialerlist = (short)list;
}

/*
 * novell_media_registry
 *
 * Register media dependent functions for Novell
 */
void novell_media_registry (void)
{
    reg_add_proto_address(LINK_NOVELL,
		      novell_build_address, "novell_build_address");
    reg_add_sprintf_address_link(LINK_NOVELL,
		      novell_sprintf_address, "novell_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_NOVELL,
		      novell_sprintf_address, "novell_sprintf_address");
    reg_add_build_packet_address(LINK_NOVELL, novell_build_packet_address,
				 "novell_build_packet_address");
    reg_add_sprintf_src_dst(LINK_NOVELL,
		      novell_sprintf_src_dst, "novell_sprintf_src_dst");
    reg_add_access_check(LINK_NOVELL,
			 nov_accesscheck, "nov_accesscheck");
    reg_add_valid_access_list(LINK_NOVELL,
		      novell_valid_access_list, "novell_valid_access_list");

    reg_add_media_ether_macaddr(LINK_NOVELL, novell_ether_macaddr,
				"novell_ether_macaddr");
    reg_add_media_ether_macwrite(ET_NOVELL_ETHER, novell_et_novell_ether,
				 "novell_et_novell_ether");
    reg_add_media_fddi_macwrite(ET_NOVELL_FDDI, novell_et_novell_fddi,
				 "novell_et_novell_fddi");
    reg_add_media_token_macaddr(LINK_NOVELL, novell_token_macaddr,
				"novell_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_NOVELL, novell_fddi_macaddr,
			       "novell_fddi_macaddr");
    reg_add_media_smds_macaddr(LINK_NOVELL, novell_smds_macaddr,
			       "novell_smds_macaddr");
    reg_add_media_x25_macaddr(LINK_NOVELL, novell_x25_macaddr,
			      "novell_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_NOVELL, novell_frame_macaddr,
				"novell_frame_macaddr");
    reg_add_media_ddr_macaddr(LINK_NOVELL, novell_ddr_macaddr,
				"novell_ddr_macaddr");
    reg_add_media_atm_macaddr(LINK_NOVELL, novell_atm_macaddr,
			      "novell_atm_macaddr");
    reg_add_media_ether_mtu(LINK_NOVELL, novell_ether_mtu, "novell_ether_mtu");
    reg_add_media_fddi_mtu(LINK_NOVELL, novell_fddi_tring_mtu,
			   "novell_fddi_tring_mtu");
    reg_add_media_tring_mtu(LINK_NOVELL, novell_fddi_tring_mtu,
			    "novell_fddi_tring_mtu");
    reg_add_set_dialer_list(LINK_NOVELL, novell_set_dialer_list, 
			    "novell_set_dialer_list");
    reg_add_media_tunnel_macaddr(LINK_NOVELL, novell_tunnel_macaddr,
			       "novell_tunnel_macaddr");
}
