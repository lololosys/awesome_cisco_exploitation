/* $Id: srt_media.c,v 3.3.62.3 1996/05/04 01:49:22 wilber Exp $
 * $Source: /release/112/cvs/Xsys/srt/srt_media.c,v $
 *------------------------------------------------------------------
 * srt_media.c -- media dependent bridging routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srt_media.c,v $
 * Revision 3.3.62.3  1996/05/04  01:49:22  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.3.62.2  1996/04/25  23:22:39  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.62.1.2.1  1996/03/24  04:05:11  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.62.1  1996/03/18  22:11:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  02:02:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  10:51:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  18:50:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:57:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:43:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/06/21  18:56:43  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Add sbridge_smf_update registry in srb_init.
 *
 * Revision 2.3  1995/06/21  02:06:54  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Insert IEEE spanning tree multicast address into SMF when AST over
 * SRB/FDDI is enabled.
 *
 * Revision 2.2  1995/06/20  16:47:03  lbustini
 * CSCdi27037:  bridged multicast doesnt initiate a call over DDR
 * Use pak->encsize rather than idb->encsize to determine offset of
 * ethertype.
 *
 * Revision 2.1  1995/06/07  23:02:44  hampton
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
#include "srt_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "address.h"
#include "span.h"
#include "../tbridge/tbridge_bvi.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../if/fddi.h"
#include "../wan/smds.h"
#include "../x25/x25.h"
#include "../smf/smf_public.h"

static uchar tr_bridge_multi[IEEEBYTES] = {0xC0, 0x00, 0x00, 0x00, 0x01, 0x00};
static const uchar ieee_llc[SAP_HDRBYTES] = {SAP_SPAN, SAP_SPAN, LLC1_UI};

/* SRB AST support */
uchar *bitswapped_ieee_bridge_func = (uchar *)&tr_bridge_multi;

/*
 * bridge_extract_typecode
 *
 * NOTE: This routine requires that the outgoing interface has been set.
 */
static ushort bridge_extract_typecode (paktype *pak)
{
    uchar *data;
    ushort typecode;

    data = pak->datagramstart + (2*IEEEBYTES);
    if (pak->if_output->hwptr->status & IDB_SERIAL)
	data += pak->encsize;
    else if (pak->if_output->hwptr->status & IDB_FDDI)
	data += FDDI_SNAP_ENCAPBYTES;
    typecode = GETSHORT(data);
    if (typecode <= MAXETHERSIZE) {
	typecode = GETSHORT(data+2);
	if (typecode == SNAPSNAP)
	    typecode = GETSHORT(data+8);
    }
    return(typecode);
}

/*
 * bridge_accesscheck
 *
 * NOTE: This routine requires that the outgoing interface has been set.
 */
static boolean bridge_accesscheck (paktype *pak, int accesslist)
{
    ushort typecode;

    typecode = bridge_extract_typecode(pak);
    return(access_check_type(accesslist, typecode));
}

/*
 * bridge_valid_access_list
 */
static boolean bridge_valid_access_list (int number)
{
    return((number >= MINTYPEACC) && (number <= MAXTYPEACC));
}

/*
 * span_macaddrs_update
 *
 * Called (via reg_invoke_media_interesting_addresses) when a
 * change in the listening state of an interface has taken place.
 * The interface is listening either selectively or promiscuously.
 *
 * Update the SMF so that spanning-tree-specific MAC addresses are
 * either inserted (the interface is listening promiscuously) or
 * deleted (the interface is listening selectively).  In addition,
 * populate any hardware address filters with the spanning-tree-
 * specific MAC addresses if transparent bridging or source-route
 * bridging is configured on the interface.
 */
static void span_macaddrs_update (idbtype *idb)
{
    hwidbtype *hwidb;

    hwidb = idb->hwptr;

    if (is_bvi(hwidb)) {
	uchar ieee_bridge_func[IEEEBYTES];
	ieee_swap(bitswapped_ieee_bridge_func, ieee_bridge_func);
	reg_invoke_bvi_smf_update(transit_bridge_multicast,
				  idb,
				  SMF_MACTYP_FDDI_TRANSIT |
				  SMF_ACTION_RECEIVE,
				  IDB_FDDI);
	/* Token only */
	reg_invoke_bvi_smf_update(ieee_bridge_func,
				  idb,
				  SMF_MACTYP_FUNC_SPAN |
				  SMF_ACTION_RECEIVE,
				  IDB_TR);
	/* All interfaces but token */
	reg_invoke_bvi_smf_update(dec_span_address,
				  idb,
				  SMF_MACTYP_DEC_SPAN |
				  SMF_ACTION_RECEIVE,
				  (IDB_LEX|IDB_ETHER|IDB_SERIAL|IDB_FDDI|
				   IDB_ISDN|IDB_ATM));
	reg_invoke_bvi_smf_update(ieee_span_address,
				  idb,
				  SMF_MACTYP_IEEE_SPAN |
				  SMF_ACTION_RECEIVE,
				  (IDB_LEX|IDB_ETHER|IDB_SERIAL|IDB_FDDI|
				   IDB_ISDN|IDB_ATM));
	return;
    }


    if (hwidb->status & IDB_FDDI) {
	/*
	 * Always enabling this address allows us to hear cisco transit
	 * bridges when we are in tranparent mode and report that there
	 * is a mis-configuration on the ring.
	 */
	reg_invoke_tbridge_smf_update(transit_bridge_multicast,
				      idb,
				      SMF_MACTYP_FDDI_TRANSIT |
				      SMF_ACTION_RECEIVE);
	if ((RUNNING_IF(LINK_BRIDGE,idb) || RUNNING_IF(LINK_SRB,idb)) &&
	    bridge_enable)
	    if_multi_add(hwidb, idb, transit_bridge_multicast); 
	if (hwidb->fddi_options & FDDI_ENCAPSULATE)
	    return;
    }

    if (is_tokenring(hwidb)) {
	uchar ieee_bridge_func[IEEEBYTES];

	ieee_swap(bitswapped_ieee_bridge_func, ieee_bridge_func);
	reg_invoke_tbridge_smf_update(ieee_bridge_func,
				      idb,
				      SMF_MACTYP_FUNC_SPAN |
				      SMF_ACTION_RECEIVE);
    } else {
	reg_invoke_tbridge_smf_update(dec_span_address,
				      idb,
				      SMF_MACTYP_DEC_SPAN |
				      SMF_ACTION_RECEIVE);
	reg_invoke_tbridge_smf_update(ieee_span_address,
				      idb,
				      SMF_MACTYP_IEEE_SPAN |
				      SMF_ACTION_RECEIVE);

	reg_invoke_sbridge_smf_update(ieee_span_address,
				      idb,
				      SMF_MACTYP_IEEE_IBM_SPAN |
				      SMF_ACTION_RECEIVE);
	
	if (RUNNING_IF(LINK_BRIDGE,idb) && bridge_enable) {
	    if_multi_add(hwidb, idb, dec_span_address);
	    if_multi_add(hwidb, idb, ieee_span_address);
	} else if (RUNNING_IF(LINK_SRB, idb) && bridge_enable) {
	    if_multi_add(hwidb, idb, ieee_span_address);
	}
    }
}

/*
 * span_dec_ether_macaddr
 *
 * Turn an DEC ST address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_DEC_SPANNING
 */
static boolean span_dec_ether_macaddr (paktype *pak, long address,
				       int *enctype, uchar **daddr,
				       uchar const **other_info)
{
    *enctype = ET_ARPA;
    *daddr = dec_span_address;
    return(TRUE);
}

/*
 * span_ieee_ether_macaddr
 *
 * Turn an IEEE ST address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_IEEE_SPANNING
 */
static boolean span_ieee_ether_macaddr (paktype *pak, long address,
					int *enctype, uchar **daddr,
					uchar const **other_info)
{
    *enctype = ET_SAP;
    *daddr = ieee_span_address;
    *other_info = ieee_llc;
    return(TRUE);
}

/*
 * span_dec_token_macaddr
 *
 * Turn an DEC ST address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_DEC_SPANNING
 */
static boolean span_dec_token_macaddr (paktype *pak, long address,
				       int *enctype, uchar **daddr,
				       uchar const **other_info,
				       ulong *rif_flags)
{
    *enctype = ET_SNAP;
    *daddr = tr_bridge_multi;
    *other_info = zerosnap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * span_ieee_token_macaddr
 *
 * Turn an IEEE ST address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_IEEE_SPANNING
 */
static boolean span_ieee_token_macaddr (paktype *pak, long address,
					int *enctype, uchar **daddr,
					uchar const **other_info,
					ulong *rif_flags)
{
    *enctype = ET_SAP;
    *daddr = tr_bridge_multi;
    *other_info = ieee_llc;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * span_dec_fddi_macaddr
 *
 * Turn an DEC ST address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_DEC_SPANNING
 */
static boolean span_dec_fddi_macaddr (paktype *pak, long address,
				      int *enctype, uchar **daddr,
				      uchar const **other_info, 
				      ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    if (hwidb->fddi_options & FDDI_ENCAPSULATE)
	*daddr = transit_bridge_multicast;
    else
	*daddr = dec_span_address;
    *enctype = ET_SNAP;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * span_ieee_fddi_macaddr
 *
 * Turn an IEEE ST address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_IEEE_SPANNING
 */
static boolean span_ieee_fddi_macaddr (paktype *pak, long address,
				       int *enctype, uchar **daddr,
				       uchar const **other_info,
				       ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    if (hwidb->fddi_options & FDDI_ENCAPSULATE)
	*daddr = transit_bridge_multicast;
    else
	*daddr = ieee_span_address;
    *enctype = ET_SAP;
    *other_info = ieee_llc;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * span_smds_macaddr
 *
 * Turn an SPAN address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_DEC_SPANNING and
 * LINK_IEEE_SPANNING.
 */
static boolean span_smds_macaddr (paktype *pak, long address, int *enctype,
				  uchar const **daddr,
				  uchar const **other_info,
				  addrtype *proto_addr)
{
    if (!RUNNING_IF(LINK_BRIDGE,pak->if_output))
	return(FALSE);

    *daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
    if(!*daddr)
	return(FALSE);

    *enctype = ET_BRIDGE;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * span_atm_macaddr
 *
 * Turn an SPAN address into an encapsulation type and a MAC address
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_DEC_SPANNING and
 * LINK_IEEE_SPANNING.
 */
static boolean
span_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		uchar const **snap_info, staticmap_type **map, 
		boolean *broadcast)
{
    uchar *dad = NULL;

    *broadcast = FALSE;
    if (!RUNNING_IF(LINK_BRIDGE,pak->if_output))
	return(FALSE);

    dad = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
    if (dad) {
    	daddr->length = STATIONLEN_ATMVC;
    	bcopy(dad, daddr->addr, STATIONLEN_SMDS64);
    } else {
	*broadcast = TRUE;
    }

    pak->enctype = ET_BRIDGE;
    *snap_info = smds_bridge_oui;
    return(TRUE);
}

/*
 * bridge_x25_macaddr
 *
 * Turn an BRIDGE address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_BRIDGE.
 */
static boolean bridge_x25_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, lcitype **lci,
				   boolean *broadcast)
{
    if (address) {
        bcopy((addrtype *)address, proto_addr, sizeof(addrtype));
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * span_x25_macaddr
 *
 * Turn an SPAN address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_DEC_SPANNING and
 * LINK_IEEE_SPANNING.
 */
static boolean span_x25_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, lcitype **lci,
				 boolean *broadcast)
{
    ether_hdr *ether;

    if (address) {
	bcopy(&address, proto_addr, sizeof(addrtype));
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }

    /*
     * Since we are sending both bridged packets and spanning
     * information over the same lci, we need to be able to 
     * differentiate between bridged packets and spanning information.
     * To do this we place a fake mac header in front of the spanning
     * information and set the type field to TYPE_DEC_SPANNING.
     */
    pak->datagramsize += ETHER_ARPA_ENCAPBYTES;
    pak->datagramstart -= ETHER_ARPA_ENCAPBYTES;
    ether = (ether_hdr *)pak->datagramstart;
    ether->type_or_len = (ushort) TYPE_DEC_SPANNING;
    return(TRUE);
}

/*
 * bridge_frame_macaddr
 *
 * Turn an BRIDGE address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_BRIDGE.
 */
static boolean bridge_frame_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    if (address) {
	memset(proto_addr, 0, sizeof(addrtype));
	proto_addr->addr.fr_address = *(ushort *)address;
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * span_frame_macaddr
 *
 * Turn an SPAN address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_DEC_SPANNING and
 * LINK_IEEE_SPANNING.
 */
static boolean span_frame_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    if (address) {
	bcopy(&address, proto_addr, sizeof(addrtype));
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * span_hdlc_macaddr
 *
 * We don't want to set the broadcast bit if we are bridging it
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_BRIDGE.
 */
static boolean span_hdlc_macaddr (paktype *pak, long address,
                                 int *enctype, ushort *flags)
{
    /* Make sure the broadcast bit, HDLC_BCAST, is not set */
    if ((address == 0) || (address == -1))
        *flags &= 0x7FFF;
    return(TRUE);
}

/*
 * span_ddr_macaddr
 *
 * Turn a BRIDGE address into a broadcast flag.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_BRIDGE, LINK_DEC_SPANNING and
 * LINK_IEEE_SPANNING.
 */
static boolean span_ddr_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    if (address) {
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_BRIDGE;
    return(TRUE);
}

/*
 * srt_media_registry
 *
 * Register media dependent functions for bridging
 */
void srt_media_registry (void)
{
    reg_add_extract_typecode(
		      bridge_extract_typecode, "bridge_extract_typecode");
    reg_add_access_check(LINK_BRIDGE,
		      bridge_accesscheck, "bridge_accesscheck");
    reg_add_valid_access_list(LINK_BRIDGE,
		      bridge_valid_access_list, "bridge_valid_access_list");
    reg_add_media_interesting_addresses(span_macaddrs_update,
					"span_macaddrs_update");
    reg_add_media_interesting_addresses(tbridge_bvi_macaddrs_update,
					"tbridge_bvi_macaddrs_update");
    reg_add_media_ether_macaddr(LINK_IEEE_SPANNING, span_ieee_ether_macaddr,
			    "span_ieee_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_DEC_SPANNING, span_dec_ether_macaddr,
			    "span_dec_ether_macaddr");
    reg_add_media_token_macaddr(LINK_IEEE_SPANNING, span_ieee_token_macaddr,
			    "span_ieee_token_macaddr");
    reg_add_media_token_macaddr(LINK_DEC_SPANNING, span_dec_token_macaddr,
			    "span_dec_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_IEEE_SPANNING, span_ieee_fddi_macaddr,
			   "span_ieee_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_DEC_SPANNING, span_dec_fddi_macaddr,
			   "span_dec_fddi_macaddr");
    reg_add_media_smds_macaddr(LINK_IEEE_SPANNING, span_smds_macaddr,
			       "span_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_DEC_SPANNING, span_smds_macaddr,
			       "span_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_BRIDGE, span_smds_macaddr, 
				"span_smds_macaddr");
    reg_add_media_x25_macaddr(LINK_BRIDGE, bridge_x25_macaddr,
			      "bridge_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_IEEE_SPANNING, span_x25_macaddr,
			      "span_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_DEC_SPANNING, span_x25_macaddr,
			      "span_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_BRIDGE, bridge_frame_macaddr,
				"bridge_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_IEEE_SPANNING, span_frame_macaddr,
				"span_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_DEC_SPANNING, span_frame_macaddr,
				"span_frame_macaddr");
    reg_add_media_ddr_macaddr(LINK_BRIDGE, span_ddr_macaddr,
				"span_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_IEEE_SPANNING, span_ddr_macaddr,
				"span_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_DEC_SPANNING, span_ddr_macaddr,
				"span_ddr_macaddr");
    reg_add_media_hdlc_macaddr(LINK_BRIDGE, span_hdlc_macaddr,
                               "span_hdlc_macaddr");
    reg_add_media_atm_macaddr(LINK_IEEE_SPANNING, span_atm_macaddr,
			       "span_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_DEC_SPANNING, span_atm_macaddr,
			       "span_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_BRIDGE, span_atm_macaddr, 
				"span_atm_macaddr");
}
