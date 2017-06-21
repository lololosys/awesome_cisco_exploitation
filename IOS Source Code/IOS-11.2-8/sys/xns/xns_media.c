/* $Id: xns_media.c,v 3.4.12.2 1996/07/23 18:50:18 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/xns/xns_media.c,v $
 *------------------------------------------------------------------
 * xns_media.c -- media dependent XNS routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xns_media.c,v $
 * Revision 3.4.12.2  1996/07/23  18:50:18  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.4.12.1  1996/03/18  22:54:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.4  1996/03/16  08:01:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.3  1996/03/13  02:15:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  11:22:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  22:01:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/15  07:39:41  smackie
 * Fix another bizarro instance of the comma operator. (CSCdi49069)
 *
 * Revision 3.3  1995/11/17  19:25:31  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:00:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:45:09  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.1  1995/06/07  23:30:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "address.h"
#include "xns.h"
#include "../if/static_map.h"
#include "../if/tring.h"
#include "../if/rif_inline.h"
#include "../wan/smds.h"
#include "../x25/x25.h"
#include "../wan/dialer.h"

/* UB ethernet code */
static const uchar xns_ub_snap[SNAP_OUIBYTES] = {0x00, 0xDD, 0x0F};	
static const uchar xns_3com_llc[SAP_HDRBYTES] = {SAP_3COM, SAP_3COM, LLC1_UI};

#define HW3COM_UPPER	0x0260	/* For 3Com TR hack. */
#define HW3COM_LOWER	0x8c



/*
 * xns_build_address
 *
 * Build an addrtype data structure containing the XNS address
 * currently in use on this interface.
 */
static void xns_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    xnssb	*xsb;

    if (!(swidb && addr))
	return;
    xsb = idb_get_swsb_inline(swidb, SWIDB_SB_XNS);
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_XNS;
    addr->length = ADDRLEN_XNS;
    addr->xns_addr.net = xsb->xnsnet;
    ieee_copy(xsb->xns_node, addr->xns_addr.host);
}

/*
 * xns_sprintf_address
 *
 * Take an addrtype data structure and print it as an XNS address.
 */
static void xns_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%u.%e", addr->xns_addr.net, addr->xns_addr.host);
}

/*
 * xns_sprintf_src_dst
 *
 * Given a packet, extract and print an XNS address.
 */
static void xns_sprintf_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    xnshdrtype *xns;

    xns = (xnshdrtype *)xnsheadstart(pak);

    if (which == SOURCE)
	sprintf(string, "%u.%e", GET2WD(xns->ssnet), xns->shost);
    else
        sprintf(string, "%u.%e", GET2WD(xns->ddnet), xns->dhost);
}

/*
 * xns_valid_access_list
 */
static boolean xns_valid_access_list (int number)
{
    return((number >= MINXNSACC) && (number <= MAXEXNSACC));
}

/*
 * xns_ether_macaddr
 *
 * Turn an XNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_XNS
 */
static boolean xns_ether_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info)
{
    *enctype = ET_ARPA;
    *daddr = (uchar *)address;
    return(TRUE);
}

/*
 * xns_token_macaddr
 *
 * Turn an XNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_XNS
 */
static boolean xns_token_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    idbtype	*interface;
    hwidbtype	*hwidb;
    xnssb	*xsb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    if (!(interface && hwidb))
	return (FALSE);
    xsb = idb_get_swsb_inline(interface, SWIDB_SB_XNS);
    if (!xsb)
	return (FALSE);
    *daddr = (uchar *)address;
    switch (xsb->xns_enctype) {
      case ET_SNAP:
	*enctype = ET_SNAP;
	*other_info = zerosnap;
	break;

      case ET_UB_TR:
	*enctype = ET_SNAP;
	*other_info = xns_ub_snap;
	break;

      case ET_3COM_TR:
	*enctype = ET_SAP;
	*other_info = xns_3com_llc;
	break;

      default:
	return (FALSE);
    }

    /*
     * 3COM Hack.  Seems that in 3Com's 3+Shaft product they had
     * the unhealthy habit of having the MAC address and the
     * XNS node address be different.  The MAC address has the
     * local bit set while the node address didn't.
     */
    if (pak->linktype == LINK_XNS && hwidb->tr_3com_hack &&
	(GETSHORT(&daddr[0]) == HW3COM_UPPER) &&
	((*daddr)[2] == HW3COM_LOWER))
	(*daddr)[0] |= TR_LOCAL;

    *rif_flags = determine_rif(interface, MULTIRING_BIT_XNS);
    return (TRUE);
}

/*
 * xns_fddi_macaddr
 *
 * Turn an XNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_XNS
 */
static boolean xns_fddi_macaddr (paktype *pak, long address, int *enctype,
				 uchar **daddr, uchar const **other_info,
				 ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *enctype = pak->enctype;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_XNS);
    return (TRUE);
}

/*
 * xns_smds_macaddr
 *
 * Turn an XNS address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_XNS.
 */
static boolean xns_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr,
				 uchar const **other_info,
				 addrtype *proto_addr)
{
    xnssb	*xsb;

    xsb = idb_get_swsb_inline(pak->if_output, SWIDB_SB_XNS);
    if ((uchar *)address != baddr) {
	proto_addr->type = ADDR_XNS;
	proto_addr->length = ADDRLEN_XNS;
	proto_addr->xns_addr.net = xsb->xnsnet;
	ieee_copy((uchar *)address, proto_addr->xns_addr.host);
	*daddr = NULL;
    } else {
	*daddr = baddr;
    }

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return (TRUE);
}

/*
 * xns_x25_macaddr
 *
 * Turn an XNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_XNS.
 */
static boolean xns_x25_macaddr (paktype *pak, long address,
				addrtype *proto_addr, lcitype **lci,
				boolean *broadcast)
{
    idbtype *idb;
    xnssb	*xsb;

    idb = get_pak_netidb(pak);
    if (!idb) {
	return (FALSE);
    }
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (!xsb) {
	return (FALSE);
    }
    if (!is_ieee_bcast((uchar *)address)) {
	proto_addr->type = ADDR_XNS;
	proto_addr->length = ADDRLEN_XNS;
	proto_addr->xns_addr.net = xsb->xnsnet;
	ieee_copy((char *)address, proto_addr->xns_addr.host);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return (TRUE);
}

/*
 * xns_frame_macaddr
 *
 * Turn an XNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_XNS.
 */
static boolean xns_frame_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, boolean *broadcast)
{
    idbtype *idb;
    xnssb	*xsb;

    idb = pak->if_output;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (!is_ieee_bcast((uchar *)address)) {
	proto_addr->type = ADDR_XNS;
	proto_addr->length = ADDRLEN_XNS;
	proto_addr->xns_addr.net = xsb->xnsnet;
	ieee_copy((char *)address, proto_addr->xns_addr.host);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return (TRUE);
}

/*
 * xns_ddr_macaddr
 *
 * Turn an XNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_XNS.
 */
static boolean xns_ddr_macaddr (paktype *pak, long address,
				addrtype *proto_addr, boolean *broadcast)
{
    idbtype	*idb;
    xnssb	*xsb;

    idb = pak->if_output;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (!is_ieee_bcast((uchar *)address)) {
	proto_addr->type = ADDR_XNS;
	proto_addr->length = ADDRLEN_XNS;
	proto_addr->xns_addr.net = xsb->xnsnet;
	ieee_copy((char *)address, proto_addr->xns_addr.host);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_XNS;
    return (TRUE);
}

/*
 * xns_atm_macaddr
 *
 * Turn an XNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_XNS.
 */
static boolean xns_atm_macaddr (paktype *pak, long address, 
				hwaddrtype *daddr, uchar const **snap_info,
				staticmap_type **map, boolean *broadcast)
{
    idbtype	*idb;
    addrtype	proto_addr;
    xnssb	*xsb;

    memset(daddr, 0, sizeof(hwaddrtype));
    *snap_info = zerosnap;
    *broadcast = FALSE;
    idb = pak->if_output;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (!is_ieee_bcast((uchar *)address)) {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_XNS;
	proto_addr.length = ADDRLEN_XNS;
	proto_addr.xns_addr.net = xsb->xnsnet;
	ieee_copy((char *)address, proto_addr.xns_addr.host);
    } else {
	*broadcast = TRUE;
    }
    if ((*map = staticmap_lookup(idb, &proto_addr, LINK_XNS,ET_ATM,*broadcast)))
        return (TRUE);
    else
        return (FALSE);
}

/*
 * xns_hwaddr_notify
 *
 * Used to update our node address whenever the MAC address of
 * the HWidb changes.
 */
static void xns_hwaddr_notify (idbtype *idb, uchar *hwaddr, ushort len)
{
    xnssb	*xsb;

    if (!(idb && hwaddr))
	return;
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (!xsb)
	return;
    ieee_copy(hwaddr, xsb->xns_node);
}
    

/*
 * xns_media_registry
 *
 * Register media dependent functions for XNS
 */
void xns_media_registry (void)
{
    reg_add_proto_address(LINK_XNS, xns_build_address, "xns_build_address");
    reg_add_media_hwaddr_notify(xns_hwaddr_notify, "xns_hwaddr_notify");
    reg_add_sprintf_address_link(LINK_XNS,
		      xns_sprintf_address, "xns_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_XNS,
		      xns_sprintf_address, "xns_sprintf_address");
    reg_add_sprintf_src_dst(LINK_XNS,
		      xns_sprintf_src_dst, "xns_sprintf_src_dst");
    reg_add_access_check(LINK_XNS, xns_accesscheck, "xns_accesscheck");
    reg_add_valid_access_list(LINK_XNS,
		      xns_valid_access_list, "xns_valid_access_list");

    reg_add_media_ether_macaddr(LINK_XNS, xns_ether_macaddr,
				"xns_ether_macaddr");
    reg_add_media_token_macaddr(LINK_XNS, xns_token_macaddr,
				"xns_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_XNS, xns_fddi_macaddr, "xns_fddi_macaddr");
    reg_add_media_smds_macaddr(LINK_XNS, xns_smds_macaddr, "xns_smds_macaddr");
    reg_add_media_x25_macaddr(LINK_XNS, xns_x25_macaddr, "xns_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_XNS, xns_frame_macaddr,
				"xns_frame_macaddr");
    reg_add_media_ddr_macaddr(LINK_XNS, xns_ddr_macaddr,
				"xns_ddr_macaddr");
    reg_add_media_atm_macaddr(LINK_XNS, xns_atm_macaddr,
			      "xns_atm_macaddr");
}
