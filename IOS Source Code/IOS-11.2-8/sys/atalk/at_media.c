/* $Id: at_media.c,v 3.3.60.9 1996/09/07 07:22:45 kleung Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_media.c,v $
 *------------------------------------------------------------------
 * atalk_media.c -- media dependent AppleTalk routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_media.c,v $
 * Revision 3.3.60.9  1996/09/07  07:22:45  kleung
 * CSCdi67191:  Token ring interfaces not passing neighboring routes
 * Branch: California_branch
 *
 * Revision 3.3.60.8  1996/07/23  18:43:08  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.3.60.7  1996/07/10  22:18:30  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.60.6  1996/06/21  16:05:25  wmay
 * CSCdi60209:  2500 Router Crash PPP Bridge Config - use new atalk
 * structures - have those routines use the dialer netcfg_idb when
 * dealing with ppp half bridged and cpp packets
 * Branch: California_branch
 *
 * Revision 3.3.60.5  1996/05/04  00:33:59  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.3.60.4  1996/05/01  14:26:01  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.3.60.3  1996/04/10  19:52:24  slin
 * CSCdi53736:  AppleTalk over TokenRing broken
 * Branch: California_branch
 * - Functional wasn't being saved properly
 *
 * Revision 3.3.60.2  1996/03/23  01:27:02  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.60.1  1996/03/18  18:52:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:06:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:29:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:33:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:43:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:50:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/22  06:36:43  tkolar
 * CSCdi38902:  ATCP client mode wont work with ISDN
 * Change method of handing out virtual-net node addresses to accomodate
 * arbitrary interfaces, and prevent non P2P interfaces from being
 * configured for Appletalk client mode.
 *
 * Revision 2.2  1995/08/07  05:28:32  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *
 * Revision 2.1  1995/06/07  20:08:43  hampton
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
#include "access.h"
#include "../util/itemlist.h"
#include "atalk_private.h"
#include "at_arp.h"
#include "at_registry.h"
#include "../if/static_map.h"
#include "../if/rif_inline.h"
#include "../srt/srt_registry.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../wan/serial.h"
#include "../if/tring.h"

#include "../smf/smf_public.h"
#include "../ip/tunnel.h"
#include "../ip/ipaddress.h"
#include "../wan/dialer.h"

uchar apple_broadcast[IEEEBYTES] = {0x09, 0x00, 0x07, 0xFF, 0xFF, 0xFF};

/*
 * atalk_build_address
 *
 * Build an addrtype data structure containing the APPLETALK address
 * currently in use on this interface.
 */
static void atalk_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    atalkidbtype *atalkidb = atalk_getidb(swidb);
    
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_APPLE;
    addr->length = ADDRLEN_APPLE;
    addr->apple_addr.net = atalkidb->atalknet;
    addr->apple_addr.host = atalkidb->atalknode;
}

/*
 * atalk_sprintf_address
 *
 * Take an addrtype data structure and print it as an ATALK address.
 */
static void  atalk_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%d.%d", addr->apple_addr.net, addr->apple_addr.host);
}

/*
 * atalk_macaddrs_update
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
static void atalk_macaddrs_update (idbtype *idb)
{
    hwidbtype	*hwidb;
    uchar apple_tr_broadcast_swap[IEEEBYTES];
    uchar apple_zone_multicast_tmp[IEEEBYTES];
    atalkidbtype *atalkidb;
    
    if (idb == NULL)
	return;
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return;

    ieee_swap(apple_tr_broadcast, apple_tr_broadcast_swap);
    atalkidb = atalk_getidb(idb);
    
    if (atalk_on_idb(idb)) {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_update(apple_broadcast,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(apple_old_fddi_broadcast,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_FDDI);
	    reg_invoke_bvi_smf_update(apple_tr_broadcast_swap,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);

	    if (atalkidb->zonelist != NULL) {
		reg_invoke_bvi_smf_update(atalkidb->atalk_zone_multicast,
					  idb,
					  SMF_MACTYP_AT_ZONE |
					  SMF_ACTION_RECEIVE,
					  IDB_BRIDGE);
		ieee_swap(atalkidb->atalk_zone_multicast, apple_zone_multicast_tmp);
		reg_invoke_bvi_smf_update(apple_zone_multicast_tmp,
					  idb,
					  SMF_MACTYP_AT_ZONE |
					  SMF_ACTION_RECEIVE,
					  IDB_TR);
	    }
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    reg_invoke_tbridge_smf_update(apple_tr_broadcast_swap,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, apple_tr_broadcast);
	} else {
	    reg_invoke_tbridge_smf_update(apple_broadcast,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, apple_broadcast);
	}

	if (idb_is_fddi(idb)) {
	    reg_invoke_tbridge_smf_update(apple_old_fddi_broadcast,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, apple_old_fddi_broadcast);
	} 
    
	if (atalkidb->zonelist != NULL) {
	    if (idb_is_tokenring(idb)) {
		ieee_swap(atalkidb->atalk_zone_multicast, apple_zone_multicast_tmp);
	    } else {
		ieee_copy(atalkidb->atalk_zone_multicast, apple_zone_multicast_tmp);
	    }		
	    reg_invoke_tbridge_smf_update(apple_zone_multicast_tmp,
					  idb,
					  SMF_MACTYP_AT_ZONE |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, atalkidb->atalk_zone_multicast);
	}
    } else {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_delete(apple_broadcast,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(apple_old_fddi_broadcast,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_FDDI);
	    reg_invoke_bvi_smf_delete(apple_tr_broadcast_swap,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    reg_invoke_bvi_smf_delete_by_result(idb,
						SMF_MACTYP_AT_ZONE |
						SMF_ACTION_RECEIVE);
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    reg_invoke_tbridge_smf_delete(apple_tr_broadcast_swap,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	} else {
	    reg_invoke_tbridge_smf_delete(apple_broadcast,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	}

	if (idb_is_fddi(idb)) {
	    reg_invoke_tbridge_smf_delete(apple_old_fddi_broadcast,
					  idb,
					  SMF_MACTYP_AT_BCAST |
					  SMF_ACTION_RECEIVE);
	}
	
	reg_invoke_tbridge_smf_delete_by_result(idb,
						SMF_MACTYP_AT_ZONE |
						SMF_ACTION_RECEIVE);
    }
}

/*
 * atalk_protocol_down
 *
 */
static void atalk_protocol_down (hwidbtype *idb, uint link)
{
    idbtype *swidb;
    atalkidbtype *atalkidb;
    
    if (link == LINK_APPLETALK) {
	
	swidb = firstsw_or_null(idb);
	atalkidb = atalk_getidb(swidb);
	
	if (atalkidb && atalkidb->atalk_client_mode) {
	    reg_invoke_at_ppp_client_down(swidb);
	}
    }
}

/*
 * atalk_protocol_up
 *
 */
static void atalk_protocol_up (hwidbtype *idb, uint link)
{
    atalkidbtype *swidb;

    swidb = atalk_getidb(firstsw_or_null(idb));

    if (link == LINK_APPLETALK) {
	if (swidb && swidb->atalk_allow_clients) {
	    if (atalk_virtual_zonelen) {
		reg_invoke_at_ppp_client_up(swidb);
	    } else {
		/* there's no internal network.  Let the admin know. */
		errmsg(AT_ERR_NOVIRTUAL, idb_get_namestring(swidb->phys_idb));
		return;
	    }
	}
    }
}

/*
 * atalk_build_packet_address
 *
 * Given a packet, build a addrtype data structure of the source or destination
 * address. The memset is important!
 */
static void atalk_build_packet_address (
    paktype *pak,
    addrtype *addr,
    enum SRCDST which)
{
    ataddrtype ataddr =
      (which == SOURCE) ? atalk_pak_srcaddr(pak) : atalk_pak_dstaddr(pak);

    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_APPLE;
    addr->length = ADDRLEN_APPLE;
    addr->apple_addr.net = atalk_netonly(ataddr);
    addr->apple_addr.host = atalk_nodeonly(ataddr);
}

/*
 * atalk_sprintf_src_dst
 *
 * Given a packet, extract and print a APPLETALK address.
 */
static void atalk_sprintf_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    if (which == SOURCE)
	sprintf(string, "%a", atalk_pak_srcaddr(pak));
    else
        sprintf(string, "%a", atalk_pak_dstaddr(pak));
}

/*
 * atalk_valid_access_list
 */
static boolean atalk_valid_access_list (int number)
{
    return((number >= MINATALKACC) && (number <= MAXATALKACC));
}

/*
 * atalk_ether_macaddr
 *
 * Turn an AppleTalk address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_ATALK_SHORT and 
 * LINK_APPLETALK.
 */
static boolean atalk_ether_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info)
{
    aarpentry_t *aarp_e;
    atalkidbtype *idb;

    /*
     * Use the netcfg_idb because we could be in this routine with a
     * dialer member idb.  It doesn't have any atalk config, the
     * dialer master does.  This is for cpp encapsulation, or ppp
     * half-bridging
     */
    idb = atalk_getidb(get_netcfg_idb(pak->if_output));
    
    if ((pak->linktype == LINK_ATALK_SHORT) &&
	atalkif_net_extended(idb))
	    return(FALSE);
    if (pak->atalk_dstmcast == ATALK_ALL_MULTICAST) {
	*daddr = (uchar *) atalk_broadcast(idb);
    } else if (pak->atalk_dstmcast == ATALK_SMRP_MULTICAST) {
	if (!reg_invoke_atalk_special_macaddr(pak->atalk_dstmcast, pak, daddr))
	    return FALSE;
    } else if (pak->atalk_dstmcast != ATALK_UNICAST) {
	*daddr = (uchar*) atalk_zone_multicast(idb, pak->atalk_dstmcast);
    } else {
	aarp_e = (*idb->atalk_lookup)(address, idb, pak);
	if (!aarp_e)
	    return(FALSE);		/* let upper layer drop 'pak' */
	*daddr = (uchar *) aarp_e->hw_addr;
    }
    if (atalkif_net_extended(idb)) {
	*enctype = ET_SNAP;
	*other_info = apple_snap;
    } else {
	*enctype = ET_ARPA;
	*other_info = zerosnap;
    }
    return(TRUE);
}

/*
 * aarp_ether_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_AARP.
 */
static boolean aarp_ether_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info)
{
    atalkidbtype *interface;
    /*
     * Use the netcfg_idb because we could be in this routine with a
     * dialer member idb.  It doesn't have any atalk config, the
     * dialer master does.  This is for cpp encapsulation, or ppp
     * half-bridging
     */
    interface = atalk_getidb(get_netcfg_idb(pak->if_output));
    *enctype = interface->aarp_enctype;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * atalk_token_macaddr
 *
 * Turn an AppleTalk address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_token_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    aarpentry_t *aarp_e;
    atalkidbtype *interface;

    interface = atalk_getidb(pak->if_output);

    if (pak->atalk_dstmcast == ATALK_ALL_MULTICAST) {
	*daddr = (uchar *)atalk_broadcast(interface);
    } else if (pak->atalk_dstmcast != ATALK_UNICAST) {
	*daddr = (uchar*)atalk_zone_multicast(interface, pak->atalk_dstmcast);
    } else {
	aarp_e = (*interface->atalk_lookup)(address, interface, pak);
	if (!aarp_e)
	    return(FALSE);		/* let upper layer drop 'pak' */
	*daddr = (uchar *)aarp_e->hw_addr;
    }
    
    *enctype = interface->atalk_enctype;
    *other_info = apple_snap;
    *rif_flags = determine_rif(interface->phys_idb, MULTIRING_BIT_APPLE);
    return(TRUE);
}

/*
 * aarp_token_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_AARP.
 */
static boolean aarp_token_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_APPLE);
    return(TRUE);
}

/*
 * atalk_fddi_macaddr
 *
 * Turn an AppleTalk address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_fddi_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    aarpentry_t *aarp_e;
    atalkidbtype *interface;

    interface = atalk_getidb(pak->if_output);

    if (pak->atalk_dstmcast == ATALK_ALL_MULTICAST) {
	*daddr = (uchar *)atalk_broadcast(interface);
    } else if (pak->atalk_dstmcast == ATALK_SMRP_MULTICAST) {
	if (!reg_invoke_atalk_special_macaddr(pak->atalk_dstmcast, pak, daddr))
	    return FALSE;
    } else if (pak->atalk_dstmcast != ATALK_UNICAST) {
	*daddr = (uchar*)atalk_zone_multicast(interface, pak->atalk_dstmcast);
    } else {
	aarp_e = (*interface->atalk_lookup)(address, interface, pak);
	if (!aarp_e)
	    return(FALSE);		/* let upper layer drop 'pak' */
	*daddr = (uchar *)aarp_e->hw_addr;
    }

    *enctype = interface->atalk_enctype; /* Currently, only ET_SNAP */
    *other_info = apple_snap;
    *rif_flags = determine_rif(interface->phys_idb, MULTIRING_BIT_APPLE);
    return(TRUE);
}

/*
 * aarp_fddi_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_AARP.
 */
static boolean aarp_fddi_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_APPLE);
    return(TRUE);
}

/*
 * atalk_hdlc_macaddr
 *
 * Turn an IP address into an encapsuatalkion type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_APPLETALK and
 * LINK_ATALK_SHORT
 */
static boolean atalk_hdlc_macaddr (paktype *pak, long address,
				   int *enctype, ushort *flags)
{
    if ((pak->linktype == LINK_ATALK_SHORT) &&
	atalkif_net_extended(atalk_getidb(pak->if_output)))
	return(FALSE);
    if (pak->atalk_dstmcast != ATALK_UNICAST)
	*flags |= HDLC_BCAST;
    return(TRUE);
}

/*
 * atalk_smds_macaddr
 *
 * Turn an AppleTalk address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_smds_macaddr (paktype *pak, long address, int *enctype,
				   uchar const **daddr,
				   uchar const **other_info,
				   addrtype *proto_addr)
{
    aarpentry_t *aarp_e;
    atalkidbtype *idb;

    idb = atalk_getidb(pak->if_output);

    aarp_e = NULL;
    if (pak->atalk_dstmcast == ATALK_UNICAST) {
	if(atalkif_net_extended(idb)) {
	    aarp_e = 
	    	(*idb->atalk_lookup)(address, idb, pak);
	    if (!aarp_e)
		return(FALSE);		/* let upper layer drop 'pak' */

	}
	if (aarp_e) {
	    *daddr = (uchar *)aarp_e->hw_addr;
	} else {
	    proto_addr->type = ADDR_APPLE;
	    proto_addr->length = ADDRLEN_APPLE;
	    proto_addr->apple_addr.net = (address & 0xffff00) >> 8;
	    proto_addr->apple_addr.host = address & 0xff;
	    *daddr = NULL;
	}
    } else {
	*daddr = baddr;
    }

    *enctype = ET_SNAP;
    *other_info = apple_snap;
    return(TRUE);
}

/*
 * aarp_smds_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_AARP.
 */
static boolean aarp_smds_macaddr (paktype *pak, long address, int *enctype,
				  uchar const **daddr,
				  uchar const **other_info,
				  addrtype *proto_addr)
{
    /*
     * Only allowed on EXTENDED networks.  
     */
    if (!(atalkif_net_extended(atalk_getidb(pak->if_output)))) {
	return(FALSE);
    }
    *daddr = baddr;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * atalk_x25_macaddr
 *
 * Turn an APPLETALK address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_x25_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, lcitype **lci,
				  boolean *broadcast)
{
    if (pak->atalk_dstmcast == ATALK_UNICAST) {
	proto_addr->type = ADDR_APPLE;
	proto_addr->length = ADDRLEN_APPLE;
	proto_addr->apple_addr.net = atalk_netonly(address);
	proto_addr->apple_addr.host = atalk_nodeonly(address);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * atalk_frame_macaddr
 *
 * Turn an APPLETALK address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_frame_macaddr (paktype *pak, long address,
				    addrtype *proto_addr, boolean *broadcast)
{ 
    if (pak->atalk_dstmcast == ATALK_UNICAST) {
	proto_addr->type = ADDR_APPLE;
	proto_addr->length = ADDRLEN_APPLE;
	proto_addr->apple_addr.net = atalk_netonly(address);
	proto_addr->apple_addr.host = atalk_nodeonly(address);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * atalk_ddr_macaddr
 *
 * Turn an APPLETALK address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean atalk_ddr_macaddr (paktype *pak, long address,
				    addrtype *proto_addr, boolean *broadcast)
{ 
    if (pak->atalk_dstmcast == ATALK_UNICAST) {
	proto_addr->type = ADDR_APPLE;
	proto_addr->length = ADDRLEN_APPLE;
	proto_addr->apple_addr.net = atalk_netonly(address);
	proto_addr->apple_addr.host = atalk_nodeonly(address);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_APPLETALK;
    return(TRUE);
}
/*
 * aarp_ddr_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_AARP.
 *
 * Note - only return for ppp bridged or cpp interfaces
 */
static boolean aarp_ddr_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    hwidbtype *hwidb;

    hwidb = get_netidb(pak->if_output->hwptr);
    if ((is_ppp(hwidb) && hwidb->ppp_bridge_atalk) ||
	(is_cpp(hwidb))) {
	*broadcast = TRUE;
	pak->dialer_save_type = LINK_APPLETALK;
	return(TRUE);
    } 
    *broadcast = FALSE;
    return(FALSE);
}

/*
 * atalk_atm_macaddr
 *
 * Turn an APPLETALK address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_APPLETALK.
 */
static boolean
atalk_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		   uchar const **other_info, staticmap_type **map, 
		   boolean *broadcast)
{ 
    addrtype proto_addr;

    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = FALSE;
    if (pak->atalk_dstmcast == ATALK_UNICAST) {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_APPLE;
	proto_addr.length = ADDRLEN_APPLE;
	proto_addr.apple_addr.net = atalk_netonly(address);
	proto_addr.apple_addr.host = atalk_nodeonly(address);
    } else {
	*broadcast = TRUE;
    }
    *other_info = apple_snap;
    if ((*map = staticmap_lookup(pak->if_output, &proto_addr,
				 LINK_APPLETALK, ET_ATM, *broadcast)))
        return(TRUE);
    else
        return(FALSE);
}
/*
 * aarp_atm_macaddr
 *
 * Turn an AARP address into an encapsulation type and a MAC address
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_AARP.
 */
static boolean
aarp_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		  uchar const **other_info, staticmap_type **map, 
		  boolean *broadcast)
{
    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = TRUE;
    *other_info = zerosnap;
    *map = staticmap_broadcast(pak->if_output, LINK_APPLETALK, ET_ATM);
    return(TRUE);
}

/*
 * atalk_tunnel_macaddr
 *
 * For IPTalk Tunnels. Determine IP address to use in the underlying
 * payload network.
 */
static boolean
atalk_tunnel_macaddr (paktype *pak, long address, addrtype *proto_addr,
		      boolean *broadcast)
{
    atalkidbtype *atalkidb;
    tunnel_info *tinfo;
    idbtype *idb;
    
    tinfo = pak->if_output->hwptr->tunnel;
    if (!tinfo)
	return FALSE;
    
    if (tinfo->mode != TUN_MODE_IPTALK_UDP)
	return FALSE;
    
    /* turn AT broadcast into IP broadcast */    
    if (address == -1L) {
	*broadcast = TRUE;
	proto_addr->ip_addr = -1L;
	return TRUE;
    }
    
    atalkidb = atalk_getidb(pak->if_output);

    /* can only encapsulate things on this AT net */
    /* and it is associated with a valid node */
    if (!atalkif_ValidAddress(atalkidb,address))
	return FALSE;

    if (tinfo->source_idb)
	idb = tinfo->source_idb;
    else
	idb = ipaddr_findidb(tinfo->source);
	
    /* make sure IP subnet doesn't overlap address */
    if (atalk_nodeonly(address) & idb->ip_nets_mask)
        return FALSE;

    proto_addr->ip_addr = (idb_sw_get_ipaddress(idb) & idb->ip_nets_mask) |
	atalk_nodeonly(address);

    return TRUE;
}


/*
 * atalk_compute_mtu
 */
static int atalk_compute_mtu (idbtype *swidb, boolean get_default)
{
    return(DDP_MAX_DATABYTES + DDP_OVERHEADBYTES);
}

/*
 * atalk_swidb_encap
 *
 * Called when a SW idb is initialized to set the AppleTalk encapsulation
 * on said interface.
 */
static void
atalk_swidb_encap (idbtype *idb, int enctype)
{
    atalkidbtype *atalkidb;
    encapstype    computed_atalk_enctype;
    encapstype    computed_aarp_enctype;
    ulong         status;
    hwidbtype    *hwidb;
    
    atalkidb = atalk_getidb(idb);

    if (atalkidb == NULL)
	return;

    hwidb = idb->hwptr;
    status = hwidb->status;
    if (status & IDB_SERIAL) {
	switch (enctype) {
	case ET_HDLC:
	    computed_atalk_enctype =
		atalkif_net_extended(atalkidb) ? ET_HDLC : ET_ETHERTALK;
	    computed_aarp_enctype = ET_NULL;
	    break;
	case ET_SMDS:
	    computed_atalk_enctype = ET_SMDS;
	    computed_aarp_enctype =
		atalkif_net_extended(atalkidb) ? ET_SMDS : ET_NULL;
	    break;
	case ET_SLIP:
	case ET_ATM_DXI:
	case ET_FRAME_RELAY:
	    computed_atalk_enctype = enctype;
	    computed_aarp_enctype = ET_NULL;
	    break;
	case ET_NULL:
	    computed_atalk_enctype = ET_NULL;
	    computed_aarp_enctype = ET_NULL;
	    break;
	default:
	    computed_atalk_enctype =
		atalkif_net_extended(atalkidb) ? ET_NULL : ET_ETHERTALK;
	    computed_aarp_enctype = ET_NULL;
	    break;
	}
    } else if (is_atm_nolane(idb)) {
	computed_atalk_enctype = enctype;
	computed_aarp_enctype = enctype;
    } else if (is_tunnel(hwidb)) {
	computed_atalk_enctype = enctype;
	computed_aarp_enctype = ET_NULL;
    } else  {
	computed_atalk_enctype = enctype;
	computed_aarp_enctype = enctype;
    }
    
    atalkidb->atalk_enctype = computed_atalk_enctype;
    atalkidb->aarp_enctype = computed_aarp_enctype;
}

static void
atalk_set_tr_functional (hwidbtype *hwidb, idbtype *swidb)
{
    atalkidbtype *atalkidb;
    ONE_ITEM_LIST(tmp, TOKEN_HW_FUNCTIONAL, sizeof(ulong), 0);

    atalkidb = atalk_getidb(swidb);

    if (!atalkidb)
	return;
    
    if (RUNNING_IF(LINK_APPLETALK, swidb)) {
	/*
	 * Get current value
	 */
	idb_hw_state_config(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);
	tmp_item.u.l_value |= atalk_tr_broadcasts(atalkidb);
	
	/*
	 * Write the new value
	 */
	itemlist_reset(&tmp_list);
	tmp_item.handled = FALSE;
	idb_hw_state_config(hwidb, IDB_CONTROL_SET_CONFIG, &tmp_list);
    }
}


/*
 * atalk_media_registry
 * Register media dependent functions for appletalk
 */

void atalk_media_registry (void)
{
    reg_add_proto_address( LINK_APPLETALK,
		      atalk_build_address, "atalk_build_address");
    reg_add_sprintf_address_link( LINK_APPLETALK,
		      atalk_sprintf_address, "atalk_sprintf_address");
    reg_add_sprintf_address_addr( ADDR_APPLE,
		      atalk_sprintf_address, "atalk_sprintf_address");

    reg_add_build_packet_address(LINK_APPLETALK, atalk_build_packet_address,
				 "atalk_build_packet_address");
    reg_add_sprintf_src_dst( LINK_APPLETALK,
		      atalk_sprintf_src_dst, "atalk_sprintf_src_dst");
    reg_add_access_check( LINK_APPLETALK,
		      atalk_priorityaccesscheck, "atalk_priorityaccesscheck");
    reg_add_valid_access_list( LINK_APPLETALK,
		      atalk_valid_access_list, "atalk_valid_access_list");

    reg_add_media_interesting_addresses(atalk_macaddrs_update,
					"atalk_macaddrs_update");

    reg_add_media_ether_macaddr(LINK_APPLETALK, atalk_ether_macaddr,
				"atalk_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_ATALK_SHORT, atalk_ether_macaddr,
				"atalk_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_AARP, aarp_ether_macaddr,
				"aarp_ether_macaddr");

    reg_add_media_token_macaddr(LINK_APPLETALK, atalk_token_macaddr,
				"atalk_token_macaddr");
    reg_add_media_token_macaddr(LINK_AARP, aarp_token_macaddr,
				"aarp_token_macaddr");

    reg_add_media_fddi_macaddr(LINK_APPLETALK, atalk_fddi_macaddr,
			       "atalk_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_AARP, aarp_fddi_macaddr,
			       "aarp_fddi_macaddr");

    reg_add_media_hdlc_macaddr(LINK_APPLETALK, atalk_hdlc_macaddr,
			       "atalk_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_ATALK_SHORT, atalk_hdlc_macaddr,
			       "atalk_hdlc_macaddr");

    reg_add_media_smds_macaddr(LINK_APPLETALK, atalk_smds_macaddr,
			       "atalk_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_AARP, aarp_smds_macaddr,
			       "aarp_smds_macaddr");

    reg_add_media_x25_macaddr(LINK_APPLETALK, atalk_x25_macaddr,
			      "atalk_x25_macaddr");

    reg_add_media_frame_macaddr(LINK_APPLETALK, atalk_frame_macaddr,
				"atalk_frame_macaddr");

    reg_add_media_ddr_macaddr(LINK_APPLETALK, atalk_ddr_macaddr,
				"atalk_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_AARP, aarp_ddr_macaddr,
			      "aarp_ddr_macaddr");
    
    reg_add_media_atm_macaddr(LINK_APPLETALK, atalk_atm_macaddr,
			      "atalk_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_AARP, aarp_atm_macaddr, "aarp_atm_macaddr");

    reg_add_media_tunnel_macaddr(LINK_APPLETALK, atalk_tunnel_macaddr,
				 "atalk_tunnel_macaddr");
    
    reg_add_media_ether_mtu(LINK_APPLETALK, atalk_compute_mtu,
			    "atalk_compute_mtu");
    reg_add_media_ether_mtu(LINK_AARP, atalk_compute_mtu, "atalk_compute_mtu");
    reg_add_media_tring_mtu(LINK_APPLETALK, atalk_compute_mtu,
			    "atalk_compute_mtu");
    reg_add_media_tring_mtu(LINK_AARP, atalk_compute_mtu,
			    "atalk_compute_mtu");
    reg_add_media_fddi_mtu(LINK_APPLETALK, atalk_compute_mtu,
			   "atalk_compute_mtu");
    reg_add_media_fddi_mtu(LINK_AARP, atalk_compute_mtu, "atalk_compute_mtu");
    reg_add_media_protocol_up(atalk_protocol_up, "atalk_protocol_up");
    reg_add_media_protocol_down(atalk_protocol_down, "atalk_protocol_down");
    reg_add_media_enctype_setup(atalk_swidb_encap, "atalk_swidb_encap");
    reg_add_media_tring_add_functional(atalk_set_tr_functional,
				       "atalk_set_tr_functional");
}
