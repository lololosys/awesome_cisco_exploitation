/* $Id: ip_media.c,v 3.10.10.13 1996/07/30 14:42:24 michellw Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_media.c,v $
 *------------------------------------------------------------------
 * ip_media.c -- media dependent IP routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_media.c,v $
 * Revision 3.10.10.13  1996/07/30  14:42:24  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.10.10.12  1996/07/29  22:45:43  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.10.10.11  1996/07/26  20:09:29  atsao
 * CSCdi55843:  Telnets across X.25 bridging fail
 * Branch: California_branch
 * Broadcast the IP host bridge datagram if its swidb matches with
 * the one in the bridge map entry of X25 mapping table.
 *
 * Revision 3.10.10.10  1996/07/23  18:46:26  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.10.10.9  1996/06/28  06:03:49  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.10.10.8  1996/06/21  00:48:49  rbadri
 * CSCdi60268:  LANE + bridging ==> EIGRP neighbors not found on elans
 * Branch: California_branch
 * In ip_macaddrs_update() use passed in idb to insert the IP multicast
 * address in the Software MAC Filter (SMF) rather than hwidb->firstsw.
 *
 * Revision 3.10.10.7  1996/05/09  14:30:52  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.10.6  1996/05/04  01:02:25  wilber
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
 * Revision 3.10.10.5  1996/04/27  05:51:18  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.10.10.4.2.1  1996/04/27  07:04:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.10.4  1996/04/25  23:14:03  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/04/26  01:21:29  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.12.2  1996/04/17  02:04:36  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.12.1  1996/02/23  20:23:32  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.10.10.3  1996/04/15  21:15:55  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.10.10.2  1996/04/08  20:52:05  ronnie
 * CSCdi51093:  no atm arp-server command causes crash
 * Branch: California_branch
 *
 * Revision 3.10.10.1.8.1  1996/04/08  01:55:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.10.1  1996/03/18  20:16:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.14.1  1996/03/28  17:26:00  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.10  1996/02/27  04:37:31  mikel
 * CSCdi49954:  EIGRP: modulatiry violation
 *
 * Revision 3.9  1996/02/23  20:41:32  mikel
 * CSCdi48057:  EIGRP: No discovery of neighb on fddi when bridge-gr
 * defined
 * High-end FDDI interfaces were not adding the eigrp multicast to
 * the software multicast filter during a non-reset change.  Insure
 * the smf adds and deletes the eigrp multicast properly.
 *
 * Revision 3.8  1996/02/13  19:03:50  rja
 * CSCdi43596:  ARP throttling not working
 * Replumbed IPv4 ARP Request software so that the ARP throttling
 * sanity checks always occur and aren't accidentally bypassed by
 * some callers.  Removed one now dead function in ip_arp.c
 *
 * Revision 3.7  1996/01/15  17:18:03  misko
 * CSCdi37400:  2511 passes packets after ipcp completes but before
 * reporting int up
 * Allow PPP to notify IP when IP comes up.  Since PPP can have NCP's that
 * come and go, and since IP routes may be installed when PPP negotiated,
 * we need to be able to tell IP to propogate those routes.  Otherwise
 * a lot of traffic gets dropped.  Take advantage of existing registrys
 * to notify IP when PPP thinks it is up.  In addition, fix network.c so
 * that when line protocol comes up, route propogation is not delayed by
 * the keepalive period.
 *
 * Revision 3.6  1995/12/06  18:59:22  atsao
 * CSCdi44889:  ip host mode not working over X.25
 * Check if tbridge_on_swidb is set to take care of the corner case where
 * no ip routing is enabled and no bridge-group is configured.
 *
 * Revision 3.5  1995/11/30  22:58:16  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.4  1995/11/27  08:44:49  atsao
 * CSCdi36544:  bridged IP to router doesnt work over X.25
 * ARP/IP cannot be bridged over X.25 when we are not routing IP and we
 * are the destination IP host bridge of these packets.
 *
 * Revision 3.3  1995/11/17  09:33:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:55:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:04:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/07  05:37:40  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.1  1995/06/07  20:57:23  hampton
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
#include "access.h"
#include "ip.h"
#include "../ip/ip_registry.h"
#include "ipaccess.h"
#include "ip_media.h"
#include "../if/static_map.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../srt/srt_registry.h"
#include "../wan/serial.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../x25/x25.h"
#include "../wan/nhrp_public.h"
#include "../smf/smf_public.h"
#include "../util/itemlist.h"
#include "ipaddress.h"

#include "../if/atm.h"
#include "../atm/sscop_public.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_ie.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_smap.h"
#include "../atm/atm_arp.h"

#include "../if/rif_inline.h"

static const uchar ip_llc[SAP_HDRBYTES]    = {SAP_IP, SAP_IP, LLC1_UI};
static const uchar probe_llc[SAP_HDRBYTES] = {SAP_EXTENDED, SAP_EXTENDED, LLC1_UI};
extern boolean ip_on_idb(idbtype *);
static uchar ipm_tr_functional[IEEEBYTES] =
    {0xc0, 0x00, 0x00, 0x04, 0x00, 0x00};

/*
 * ip_build_address
 *
 * Build an addrtype data structure containing the IP address
 * currently in use on this interface.
 */
static void ip_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_IP;
    addr->length = ADDRLEN_IP;
    addr->ip_addr = swidb->ip_address;
}

/*
 * ip_sprintf_address
 *
 * Take an addrtype data structure and print it as an IP address.
 */
static void ip_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%i", addr->ip_addr);
}

/*
 * ip_build_packet_address
 *
 * Given a packet, build a addrtype data structure of the source or
 * destination address. The memset zero is important!
 */
static void ip_build_packet_address (
    paktype *pak,
    addrtype *addr,
    enum SRCDST which)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_IP;
    addr->length = ADDRLEN_IP;
    if (which == SOURCE)
        addr->ip_addr = ip->srcadr;
    else
        addr->ip_addr = ip->dstadr;
}

/*
 * ip_sprintf_src_dst
 *
 * Given a packet, extract and print a IP address.
 */
static void ip_sprintf_src_dst (
    char *string,
    paktype *pak_tmp,
    enum SRCDST which)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak_tmp);

    if (which == SOURCE)
	sprintf(string, "%i", ip->srcadr);
    else
        sprintf(string, "%i", ip->dstadr);
}

/*
 * ip_valid_access_list
 */
static boolean ip_valid_access_list (int number)
{
    return((number >= MINFASTACC) && (number <= MAXSLOWACC));
}

/*
 * ip_macaddrs_update
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
void ip_macaddrs_update (idbtype *idb)
{

    ethermultiaddrtype ipmultiaddr = ETHER_IP_MCAST;
    ethermultiaddrtype ipmultimask = ETHER_IP_MCAST_MASK;
    ipmultiaddtype *ptr;
    hwidbtype *hwidb;

    hwidb = idb->hwptr;


    if (!ip_on_idb(idb)) {
	/*
	 *  hmmm ... need to delete any SMF_MACTYP_IP_MCAST left in the SMF
	 * in case we are still bridging but have de-configured IP ... but
	 * I'm not sure if ip_multiaddrQ is still valid and can be followed
	 * to generate the addresses to delete ... the update call will delete
	 * them if we have de-configured tbridge ... do the deed by result:
	 */
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_delete_by_result(idb,
						SMF_MACTYP_IP_MCAST |
						SMF_ACTION_RECEIVE);
	} else {
	    reg_invoke_tbridge_smf_delete_by_result(idb,
						    SMF_MACTYP_IP_MCAST |
						    SMF_ACTION_RECEIVE);
	}
	return;
    }

    /*
     * If we are multicast routing on interface, open up address filter
     * to receive all data link multicast addresses. Otherwise, open up
     * address filter for groups that the router has joined.
     */
    if (idb->ip_mrouting) {
	if (hwidb->listen_range) {
	    (*hwidb->listen_range)(idb, ipmultiaddr.addr, ipmultimask.addr);
	}
    } else {
	for (ptr = (ipmultiaddtype *)idb->ip_multiaddrQ.qhead; ptr;
	     ptr = ptr->next) {

	    /*
	     * This evil kludge is here because of a bug in gcc-v2.3.3
	     * (921225-cygnus progressive) to deal with bad code generated
	     * with -mno-bitfield pull out the ipaddrtype addr=... when it
	     * works again.  (there is no fix yet (pst 12apr93)
	     */
	    ipaddrtype addr = ptr->address;
	    ipmultiaddr.bottom23 = addr;

	    if (is_bvi(hwidb)) {
		reg_invoke_bvi_smf_update(ipmultiaddr.addr,
					  idb,
					  SMF_MACTYP_IP_MCAST |
					  SMF_ACTION_RECEIVE,
					  IDB_BRIDGE);
	    } else {
		reg_invoke_tbridge_smf_update(ipmultiaddr.addr,
					      idb,
					      SMF_MACTYP_IP_MCAST |
					      SMF_ACTION_RECEIVE);
	    }

	    if (hwidb->listen)
		if_multi_add(hwidb, idb, ipmultiaddr.addr);
	}
    }
}

static void ip_set_tr_functional (hwidbtype *hwidb, idbtype *swidb)
{
    ONE_ITEM_LIST(tmp, TOKEN_HW_FUNCTIONAL, sizeof(ulong), 0);

    if (swidb->ip_multicast_use_functional) {
	/*
	 * Get current value
	 */
	idb_hw_state_config(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);
	tmp_item.u.l_value |= TRFUNC_IPMCAST;
	
	/*
	 * Write the new value
	 */
	itemlist_reset(&tmp_list);
	tmp_item.handled = FALSE;
	idb_hw_state_config(hwidb, IDB_CONTROL_SET_CONFIG, &tmp_list);
    }
}

/*
 * ip_tunnel_macaddr
 * For multi-point GRE tunnels, determine IP address to use in the
 * underlying payload network.
 */

static boolean ip_tunnel_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, boolean *broadcast)
{
    uchar const *macaddr;
    ipaddrtype dest;
    
    if (IPMULTICAST(address) || (address == 0L) || (address == -1L) ||
	(pak->flags & PAK_XMITBCAST))
	*broadcast = TRUE;
    else
	*broadcast = FALSE;
    if (!reg_invoke_iprouting_macaddr(pak, address, &macaddr))
	return(FALSE);
    dest = *((ipaddrtype *)macaddr);
    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = dest;
    return(TRUE);
}

/*
 * ip_ether_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_IP
 */
static boolean ip_ether_macaddr (paktype *pak, long address, int *enctype,
				 uchar **daddr, uchar const **other_info)
{
    arptype *ptr;
    idbtype *interface;
    hwidbtype *hwidb;
    static ethermultiaddrtype ipmultiaddr = ETHER_IP_MCAST;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);
    *enctype = interface->ip_enctype;
    *other_info = (*enctype == ET_SAP) ? ip_llc : zerosnap;
    if (IPMULTICAST(address)) {
	ipmultiaddr.bottom23 = address;
	*daddr = ipmultiaddr.addr;
	return(TRUE);
    }
    if ((address == -1L) || (address == 0) || (pak->flags & PAK_XMITBCAST)) {
	*daddr = (uchar *) baddr;
	return(TRUE);
    }

    if (BRIDGING_IF(interface, LINK_IP) &&
	(interface->ip_address == address)) {
	*daddr = (uchar *)hwidb->hardware;
	return(TRUE);
    }

    ptr = ip_arp_lookup_and_resolve(interface, address);
    if (!ptr) {
	if (!reg_invoke_iprouting_macaddr(pak, address, (uchar const **)daddr))
	    return(FALSE);
	*enctype = ET_ARPA;
	*other_info = zerosnap;
	return(TRUE);
    }
    *daddr = (uchar *)ptr->hardware;
    *enctype = ptr->enctype;
    *other_info = (*enctype == ET_SAP) ? ip_llc : zerosnap;
    if (BRIDGING_IF(interface, LINK_IP) && ptr->idb)
	pak->if_output = ptr->idb;
    return(TRUE);
}

/*
 * arp_ether_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_ARP and LINK_RARP
 */
static boolean arp_ether_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info)
{
    /*
     * This only happens when bridging IP, and were trying to force
     * transit bridged broadcast arp datagram.
     */
    if (pak->enctype == ET_BRIDGE_ENCAPS)
	*enctype = ET_ARPA;
    else
	*enctype = pak->enctype;
    *daddr = (uchar *) address;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * probe_ether_macaddr
 *
 * Turn a probe address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_PROBE
 * DON'T revise encapsulation type here.  If the packet is locally generated,
 * the encapsulation was already explicitly chosen and set, otherwise the
 * encapsulation is known from the incoming packet.
 */
static boolean probe_ether_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info)
{
    *enctype = pak->enctype;
    *daddr = (uchar *) address;
    *other_info = probe_llc;
    return(TRUE);
}

/*
 * ip_token_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_IP
 */
static boolean ip_token_macaddr (paktype *pak, long address, int *enctype,
				 uchar **daddr, uchar const **other_info,
				 ulong *rif_flags)
{
    arptype *ptr;
    idbtype *interface;

    interface = pak->if_output;
    if ((address == -1L) || (address == 0) || (pak->flags & PAK_XMITBCAST)) {
	*daddr = (uchar *)trbaddr1;
    } else if (IPMULTICAST(address)) {
	*daddr = (!interface->ip_multicast_use_functional)
	    ? trbaddr1 : ipm_tr_functional;
    } else {
	ptr = ip_arp_lookup_and_resolve(interface, address);
	if (!ptr) 
	    return(FALSE);
	*daddr = (uchar *)ptr->hardware;
    }
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_IP);
    return(TRUE);
}

/*
 * arp_token_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_ARP, and LINK_RARP
 */
static boolean arp_token_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_IP);
    return(TRUE);
}

/*
 * ip_fddi_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_IP
 */
static boolean ip_fddi_macaddr (paktype *pak, long address, int *enctype,
				uchar **daddr, uchar const **other_info,
				ulong *rif_flags)
{
    arptype *ptr;
    idbtype *interface;
    hwidbtype *hwidb;
    static ethermultiaddrtype ipmultiaddr = ETHER_IP_MCAST;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);

    *rif_flags = determine_rif(interface, MULTIRING_BIT_IP);

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     * We are talking IP or ARP onto the Fddi ring but to someone
     * beyond the Fddi ring (via an encapsulation bridge).  Folks
     * on the ring will NOT be in the bridge table because this
     * interface is encapsulating.  So fddi_bridge_datagram will
     * fail and we will do the right thing.
     */
    if (BRIDGING_IF(interface, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,interface)) &&
	  (hwidb->fddi_options & FDDI_ENCAPSULATE)) {
	*daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (*daddr) {
	    pak->linktype = LINK_BRIDGE;
	    *enctype = ET_SNAP;
	    *other_info = cisco_snap;
	    return(TRUE);
	}
	return(FALSE);
    }

    *enctype = interface->ip_enctype;
    *other_info = zerosnap;
    if (IPMULTICAST(address)) {
	ipmultiaddr.bottom23 = address;
	*daddr = ipmultiaddr.addr;
	return(TRUE);
    }

    if ((address == -1L) || (address == 0L) || (pak->flags & PAK_XMITBCAST)) {
	*daddr = (uchar *) baddr;
	return(TRUE);
    }

    ptr = ip_arp_lookup_and_resolve(interface, address);
    if (!ptr) 
	return(FALSE);
    *daddr = (uchar *)ptr->hardware;
    *enctype = ptr->enctype;
    return(TRUE);
}

/*
 * arp_fddi_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_ARP, LINK_RARP
 */
static boolean arp_fddi_macaddr (paktype *pak, long address, int *enctype,
				 uchar **daddr, uchar const **other_info,
				 ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;
    rfc_arp *arp;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     * We are talking IP or ARP onto the Fddi ring but to someone
     * beyond the Fddi ring (via an encapsulation bridge).  Folks
     * on the ring will NOT be in the bridge table because this
     * interface is encapsulating.  So fddi_bridge_datagram will
     * fail and we will do the right thing.
     */
    if (BRIDGING_IF(interface, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,interface)) &&
	  (hwidb->fddi_options & FDDI_ENCAPSULATE)) {
	*daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (*daddr) {
	    pak->linktype = LINK_BRIDGE;
	    *enctype = ET_SNAP;
	    *other_info = cisco_snap;
	    return(TRUE);
	}
	return(FALSE);
    }

    /*
     * RFC1188 check, a layer violation, but arp.c is messy enough.
     */
    arp = (rfc_arp *)rfc_arp_headstart(pak);
    if (arp->arpro == PR_IP)
	arp->arhrd = HW_ETH;
    
    *enctype = pak->if_output->ip_enctype;
    *daddr = (uchar *) address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_IP);
    return(TRUE);
}

/*
 * ip_arp_hdlc_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_IP
 */
static boolean ip_arp_hdlc_macaddr (paktype *pak, long address,
				    int *enctype, ushort *flags)
{
    if (BRIDGING_IF(pak->if_output, LINK_IP) &&
	RUNNING_IF(LINK_BRIDGE,pak->if_output)) {
	if (ether_vencap(pak, address)) {
	    pak->datagramsize += HDLC_ENCAPBYTES;
	    pak->datagramstart -= HDLC_ENCAPBYTES;
	    PUTLONG(pak->datagramstart, HDLC_BRIDGECODE);
	    pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	    *enctype = ET_NULL;
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    }
    if ((address == 0) || (address == -1) || (pak->flags & PAK_XMITBCAST))
	*flags |= HDLC_BCAST;
    return(TRUE);
}

/*
 * ip_smds_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_IP.
 */
static boolean ip_smds_macaddr (paktype *pak, long address, int *enctype,
				uchar const **daddr, uchar const **other_info,
				addrtype *proto_addr)
{
    arptype *arp;
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     */
    if (BRIDGING_IF(pak->if_output, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,pak->if_output))) {
	*daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (*daddr) {
	    pak->linktype = LINK_BRIDGE;
	    *enctype = ET_BRIDGE;
	    *other_info = zerosnap;
	    return(TRUE);
        }
    }
    if (IPMULTICAST(address) || (address == 0L) || (address == -1L)
	|| (pak->flags & PAK_XMITBCAST)) {
	*daddr = baddr;
    } else {
	if (!reg_invoke_iprouting_macaddr(pak, address, daddr)) {
	    arp = ip_arp_lookup_and_resolve(idb, address);
	    if (arp) {
		*daddr = arp->hardware;
	    } else {
		proto_addr->type = ADDR_IP;
		proto_addr->length = ADDRLEN_IP;
		proto_addr->ip_addr = address;
		*daddr = NULL;
	    }
	}
    }

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}
/*
 * arp_smds_macaddr
 *
 * Turn an ARP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_ARP.
 */
static boolean arp_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr, uchar const **other_info,
				 addrtype *proto_addr)
{
    /*
     * Kludge code in case we are not routing IP and are a bridge.
     */
    if (BRIDGING_IF(pak->if_output, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,pak->if_output))) {
	*daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (*daddr) {
	    pak->linktype = LINK_BRIDGE;
	    *enctype = ET_BRIDGE;
	    *other_info = zerosnap;
	    return(TRUE);
        }
    }
    *daddr = (uchar *)address;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * ip_x25_macaddr
 *
 * Turn an IP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_IP.
 */
static boolean ip_x25_macaddr (paktype *pak, long address,
			       addrtype *proto_addr, lcitype **lci,
			       boolean *broadcast)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     * Set the linktype to LINK_BRIDGE so that this IP packet can
     * be processed properly by X25.
     */
    if (BRIDGING_IF(idb, LINK_IP) && (idb->tbridge_on_swidb)) {
	if (ether_vencap(pak, address)) {
	    /*
	     * Since X121 address is no longer significant to a brige
	     * map lookup, broadcast this packet. 
	     * hwidb->extract_snpa cannot be used to extract X.25 address
	     * on a pak did not originate from X.25.
	     */
	    pak->enctype = ET_BRIDGE;   /* Set bridge encapsulation */
	    pak->linktype = LINK_BRIDGE;
	    *broadcast = TRUE;
	    return(TRUE);
	} else {
	    return(FALSE); 
	}
    }
    if (IPMULTICAST(address) || (address == 0L) || (address == -1L)
	|| (pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    }
    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * arp_x25_macaddr
 *
 * Turn an ARP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_ARP.
 */
static boolean arp_x25_macaddr (paktype *pak, long address,
				addrtype *proto_addr, lcitype **lci,
				boolean *broadcast)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     * Set the linktype to LINK_BRIDGE so that this ARP packet can
     * be processed properly by X25.
     */
    if (BRIDGING_IF(idb, LINK_IP) && (idb->tbridge_on_swidb)) {
	if (ether_vencap(pak, address)) {
	    pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	    pak->linktype = LINK_BRIDGE;
	    *broadcast = TRUE;
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    }

    if ((address == 0L) || (address == -1L) || (pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    }

    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * tcp_x25_macaddr
 *
 * Turn an IP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_COMPRESSED_TCP and
 * LINK_UNCOMPRESSED_TCP.
 */
static boolean tcp_x25_macaddr (paktype *pak, long address,
				addrtype *proto_addr, lcitype **lci,
				boolean *broadcast)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    if (pak->linktype == LINK_COMPRESSED_TCP)
	pak->x25_flags |= PAK_X25_QBIT;

    if ((address == 0L) || (address == -1L) || (pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    }

    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * ip_frame_macaddr
 *
 * Turn an IP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_IP.
 */
static boolean ip_frame_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    if (BRIDGING_IF(idb, LINK_IP)) {
	if (ether_vencap(pak, address)) {
	    pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	    pak->linktype = LINK_BRIDGE;
	    *broadcast = TRUE;
	    return(TRUE);
	} else {
	    return(FALSE);
        }
    }

    if (IPMULTICAST(address) || (address == 0L) || (address == -1L) ||
	(pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    }

    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * arp_frame_macaddr
 *
 * Turn an ARP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_ARP.
 */
static boolean arp_frame_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, boolean *broadcast)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    if (BRIDGING_IF(idb, LINK_IP)) {
	if (ether_vencap(pak, address)) {
	    pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	    pak->linktype = LINK_BRIDGE;
	    *broadcast = TRUE;
	    return(TRUE);
	} else {
	    return(FALSE);
        }
    }

    if ((address == 0L) || (address == -1L) || (pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    }

    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * ip_arp_ddr_macaddr
 *
 * Turn an IP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_IP.
 */
static boolean ip_arp_ddr_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    *broadcast = FALSE;

    if (BRIDGING_IF(pak->if_output, LINK_IP)) {
        if (ether_vencap(pak, address)) {
            pak->enctype = ET_BRIDGE;   /* Set bridge encapsulation */
            pak->linktype = LINK_BRIDGE;
            *broadcast = TRUE;
            pak->dialer_save_type = LINK_BRIDGE;
            return(TRUE);
        } else {
            return(FALSE);
        }
    }

    pak->dialer_save_type = LINK_IP;

    if (IPMULTICAST(address) || (address == 0L) || (address == -1L) ||
	(pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	/*
	 * Need to return the address structure filled in to allow
	 * for backward compatibility with dialer maps of the form:
	 *   dialer map ip 255.255.255.255
	 */
	if (address != 0) {
            proto_addr->type = ADDR_IP;
            proto_addr->length = ADDRLEN_IP;
            proto_addr->ip_addr = address;
	}
	return(TRUE);
    }

    proto_addr->type = ADDR_IP;
    proto_addr->length = ADDRLEN_IP;
    proto_addr->ip_addr = address;
    return(TRUE);
}

/*
 * ip_channel_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_CHANNEL_MACADDR, indexed by LINK_IP.
 */

static boolean
ip_channel_macaddr (paktype *pak, long address, int *enctype,
		    boolean *broadcast, staticmap_type **map)
{
    idbtype *idb;
    hwidbtype *hwidb;
    addrtype proto_addr;
    maplist_type *channel_maplist = maplist_lookup(" channel");

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;

    if (IPMULTICAST(address) || (address == 0L) || (address == -1L) ||
	(pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	return(TRUE);
    } else {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_IP;
	proto_addr.length = ADDRLEN_IP;
	proto_addr.ip_addr = address;
    }

    *map = staticmap_lookup_list(channel_maplist, &proto_addr, LINK_IP, ET_CHANNEL);
    return(*map != NULL);
}


/*
 * ip_atm_macaddr
 *
 * Turn an IP address into an encapsulation type and a MAC address
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_IP.
 */
static boolean
ip_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		uchar const **snap_info, staticmap_type **map, 
		boolean *broadcast)
{
    arptype *arp;
    idbtype *idb;
    hwidbtype *hwidb;
    addrtype proto_addr;
    uchar *dad = NULL;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    *broadcast = FALSE;
    memset(daddr, 0, sizeof(hwaddrtype));
    *snap_info = zerosnap;

    /*
     * Kludge code in case we are not routing IP and are a bridge.
     */
    if (BRIDGING_IF(pak->if_output, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,pak->if_output))) {
	dad = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (dad) {
	    pak->linktype = LINK_BRIDGE;
	    daddr->length = STATIONLEN_ATMVC;
	    bcopy(dad, daddr->addr, STATIONLEN_SMDS64);
	    *snap_info = smds_bridge_oui;
	    pak->enctype = ET_BRIDGE;
	    return(TRUE);
        }
    }

    if (IPMULTICAST(address) || (address == 0L) || (address == -1L)
	|| (pak->flags & PAK_XMITBCAST)) {
	*broadcast = TRUE;
	*map = staticmap_lookup(idb, &proto_addr, LINK_IP,ET_ATM, *broadcast);
    } else {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_IP;
	proto_addr.length = ADDRLEN_IP;
	proto_addr.ip_addr = address;
	*map = staticmap_lookup(idb, &proto_addr, LINK_IP,ET_ATM,
				*broadcast);
	if (*map == NULL || ((*map)->map_type == MAP_DYNAMIC)) {
	    arp = ip_arp_lookup_and_resolve(idb, address);
	    if (arp) {
		daddr->type = arp->hwtype;
		daddr->length = arp->hwlen;
		bcopy(arp->hardware, daddr->addr, arp->hwlen);
		if ((pak->linktype != LINK_NHRP) &&
		    address != (iphdrtype_start(pak))->dstadr) {
		    reg_invoke_media_macaddr_map(pak, &proto_addr,
						 *broadcast, map);
		    if (*map) {
			/*
			 * Let NHRP map table entry take precedence over any
			 * ATM ARP entry.
			 */
			daddr->length = 0;
		    }
		}
		return(TRUE);
	    }
	}
    }

    /*
     * Attempt dynamic resolution?
     * Dynamic resolution can allow us to reach ip->dstadr directly,
     * even though the next hop according to the routing table may be
     * something else.
     */
    if ((*map == NULL) ||
	((pak->linktype != LINK_NHRP) &&
	 (address != (iphdrtype_start(pak))->dstadr)))
	reg_invoke_media_macaddr_map(pak, &proto_addr, *broadcast, map);
    return(*map != NULL);
}
/*
 * arp_atm_macaddr
 *
 * Turn an ARP address into an encapsulation type and a MAC address
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_ARP.
 */
static boolean
arp_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		 uchar const **snap_info, staticmap_type **map, 
		 boolean *broadcast)
{
    uchar *dad = NULL;

    *broadcast = FALSE;
    /*
     * Kludge code in case we are not routing IP and are a bridge.
     */
    if (BRIDGING_IF(pak->if_output, LINK_IP) &&
	(RUNNING_IF(LINK_BRIDGE,pak->if_output))) {
	dad = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (dad) {
	    pak->linktype = LINK_BRIDGE;
    	    daddr->length = STATIONLEN_ATMVC;
    	    bcopy(dad, daddr->addr, STATIONLEN_SMDS64);
	    pak->enctype = ET_BRIDGE;
	    *snap_info = smds_bridge_oui;
	    return(TRUE);
        }
    }

    *snap_info = zerosnap;

    /*
     * Ok..... We need to ARP.  Do we have an ATM ARP server?
     * If so, then send the ARP request to the ATM ARP server.  The
     * ARPREPLY will tell us the real NSAP connection to open for this
     * IP address
     * If the ATM ARP server entry isn't in a map-list yet, then dump
     * the ARP packet until the ATM ARP periodic routine makes the
     * STATIC-MAP entry for the server.
     */
    if(pak->if_output->atm_arp) {
	atm_arpserv_entry *entry;

	FOR_ALL_ARP_SERVERS(pak->if_output->atm_arp, entry) {
	    *map = staticmap_hwlookup(pak->if_output, &entry->nsap, ET_ATM,
				      TRUE, LINK_ARP);
	    if (*map != NULL)
		return(TRUE);
	}
	return(FALSE);
    }

    *broadcast = TRUE;
    memset(daddr, 0, sizeof(hwaddrtype));

    daddr->type = STATION_ATMVC;
    daddr->length = STATIONLEN_ATMVC;
    bcopy((uchar *)address, daddr->addr, STATIONLEN_SMDS64);

    return(TRUE);
}

/*
 * ip_ether_compute_mtu
 */
static int ip_ether_compute_mtu (idbtype *swidb, boolean get_default)
{
    switch (swidb->ip_enctype) {
      case ET_SAP:
	return(swidb->hwptr->maxmtu - ET_SAP_LEN);
      case ET_SNAP:
	return(swidb->hwptr->maxmtu - ET_SNAP_LEN);
      default:
	return(swidb->hwptr->maxmtu);
    }
}

/*
 * ip_fddi_compute_mtu
 *
 * The max is the interface MTU;  the default is the 1188 value.
 */
static int ip_fddi_compute_mtu (idbtype *swidb, boolean get_default)
{
    if (get_default && (swidb->hwptr->maxmtu >= FDDI_IP_MTU))
	return(FDDI_IP_MTU);
    return(swidb->hwptr->maxmtu);
}

/*
 * ip_atm_compute_mtu
 *
 * The max is the interface MTU;  the default is the 1188 value.
 */
static int ip_atm_compute_mtu (idbtype *swidb, boolean get_default)
{
/*
 * Leave at MAX MTU for now.
 *
    if (get_default && (swidb->hwptr->maxmtu >= FDDI_IP_MTU))
	return(FDDI_IP_MTU);
 */
    return(swidb->sub_mtu);
}

/*
 * ip_channel_compute_mtu
 *
 * The max is the interface MTU;  the default is the 1188 value.
 */
static int ip_channel_compute_mtu (idbtype *swidb, boolean get_default)
{
    if (get_default && (swidb->hwptr->maxmtu >= FDDI_IP_MTU))
	return(FDDI_IP_MTU);
    return(swidb->hwptr->maxmtu);
}

/*
 * ip_change_mtu
 *
 * A user has configured a different physical MTU size for this
 * interface.  Adjust the IP MTU size to make sure that it is still
 * valid.
 */
void ip_change_mtu (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    idbtype *idb;

    /*
     * If the mtu is being adjusted upward and the IP mtu was previously
     * equal to the maxmtu then adjust it upward as well.  This handles
     * the case where the nvram has only an mtu command that adjusts the
     * mtu skyward.  If the IP mtu is the same as the max then it wont
     * show up in the nvram at all since max is the defaults.
     */
    if (delta > 0) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (def_proto_mtu(idb, LINK_IP) == (idb->ip_mtu + delta))
		idb->ip_mtu += delta;
	}
    }

    /*
     * Don't let the max go lower than the IP mtu.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (max_proto_mtu(idb, LINK_IP) < idb->ip_mtu)
	    idb->ip_mtu = max_proto_mtu(idb, LINK_IP);
    }
}

/*
 * ip_set_default_mtu
 *
 * Set the default MTU value for an interface.  This routine is called
 * when an interface changes its physical MTU size.  For example, when a
 * token ring interface changes from 16M to 4M.
 */
static void ip_set_default_mtu (hwidbtype *hwidb)
{
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	idb->ip_mtu = def_proto_mtu(idb, LINK_IP);
    }
}

/*
 * ip_transit_bridging
 * A stub to lookup an IP address in the ARP cache and return a poitner
 * to the corresponding MAC address.  Necessary for transit bridging on
 * FDDI or SMDS.
 */

uchar *ip_transit_bridging (ipaddrtype address, idbtype *output)
{
    arptype *arp;

    if ((address == -1) || (address == 0))
	return(NULL);
    arp = ip_arp_lookup_and_resolve(output, address);
    if (arp == NULL)
	return(NULL);
    return((uchar *) &arp->hardware);
}

/*
 * ip_set_dialer_list
 * Set ip dialer-list used by fastswitching code.
 */
static void ip_set_dialer_list (dialerplisttype *diallst, int list)
{
    diallst->ip_dialerlist = list;
}

/*
 * ip_protocol_up
 * This routine is called by those encapsulations that have protocols
 * come up independant of line protocol (PPP is the primary user).  It
 * just calls route adjust so that the net_background task doesn't need to.
 */

static void ip_protocol_up (hwidbtype *hwidb, uint link)
{
    idbtype *idb;
    hwidbtype *tempidb;

    tempidb = get_netidb(hwidb); /* Use the hunt group master idb */
    idb = firstsw_or_null(tempidb);

    if ((link == LINK_IP) || (link == LINK_ALL)) {
        reg_invoke_iprouting_route_adjust(idb);
	ip_fixsocket_address(idb, tempidb);
    }
}

/*
 * ip_media_registry
 *
 * Register media dependent functions for IP
 */
void ip_media_registry (void)
{
    reg_add_arp_enq(PR_IP, ip_arp_input, "ip_arp_input");
    reg_add_restarted(ip_arp_announcement, "ip_arp_announcement");
    reg_add_arp_reload(ip_arp_reload, "ip_arp_reload");
    reg_add_no_rif(LINK_IP, ip_arp_resolve, "ip_arp_resolve");
    reg_add_proto_address(LINK_IP, ip_build_address, "ip_build_address");
    reg_add_sprintf_address_link(LINK_IP,
		      ip_sprintf_address, "ip_sprintf_address");
    reg_add_sprintf_address_link(LINK_UNCOMPRESSED_TCP,
		      ip_sprintf_address, "ip_sprintf_address");
    reg_add_sprintf_address_link(LINK_COMPRESSED_TCP,
		      ip_sprintf_address, "ip_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_IP,
		      ip_sprintf_address, "ip_sprintf_address");
    reg_add_build_packet_address(LINK_IP, ip_build_packet_address,
				 "ip_build_packet_address");
    reg_add_sprintf_src_dst(LINK_IP, ip_sprintf_src_dst, "ip_sprintf_src_dst");
    reg_add_access_check(LINK_IP, ip_old_accesscheck, "ip_accesscheck");
    reg_add_valid_access_list(LINK_IP,
		      ip_valid_access_list, "ip_valid_access_list");
    reg_add_media_interesting_addresses(ip_macaddrs_update,
					"ip_macaddrs_update");
    reg_add_media_ether_macaddr(LINK_IP, ip_ether_macaddr, "ip_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_ARP, arp_ether_macaddr,
				"arp_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_RARP, arp_ether_macaddr,
				"arp_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_PROBE, probe_ether_macaddr,
				"probe_ether_macaddr");
    reg_add_media_tunnel_macaddr(LINK_IP, ip_tunnel_macaddr,
				 "ip_tunnel_macaddr");
    reg_add_media_token_macaddr(LINK_IP, ip_token_macaddr, "ip_token_macaddr");
    reg_add_media_token_macaddr(LINK_ARP, arp_token_macaddr,
				"arp_token_macaddr");
    reg_add_media_token_macaddr(LINK_RARP, arp_token_macaddr,
				"arp_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_IP, ip_fddi_macaddr, "ip_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_ARP, arp_fddi_macaddr, "arp_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_RARP, arp_fddi_macaddr,"arp_fddi_macaddr");
    reg_add_media_hdlc_macaddr(LINK_IP, ip_arp_hdlc_macaddr,
			       "ip_arp_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_ARP, ip_arp_hdlc_macaddr,
			       "ip_arp_hdlc_macaddr");
    reg_add_media_smds_macaddr(LINK_IP, ip_smds_macaddr, "ip_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_ARP, arp_smds_macaddr, "arp_smds_macaddr");
    reg_add_media_x25_macaddr(LINK_IP, ip_x25_macaddr, "ip_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_ARP, arp_x25_macaddr, "arp_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_COMPRESSED_TCP, tcp_x25_macaddr,
			      "tcp_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_UNCOMPRESSED_TCP, tcp_x25_macaddr,
			      "tcp_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_IP, ip_frame_macaddr, "ip_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_ARP, arp_frame_macaddr,
				"arp_frame_macaddr");

    reg_add_media_ddr_macaddr(LINK_IP, ip_arp_ddr_macaddr, 
    				"ip_arp_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_ARP, ip_arp_ddr_macaddr,
				"ip_arp_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_COMPRESSED_TCP, ip_arp_ddr_macaddr,
			      "ip_arp_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_UNCOMPRESSED_TCP, ip_arp_ddr_macaddr,
			      "ip_arp_ddr_macaddr");

    reg_add_media_atm_macaddr(LINK_IP, ip_atm_macaddr, "ip_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_ARP, arp_atm_macaddr, "arp_atm_macaddr");

    reg_add_media_channel_macaddr(LINK_IP, ip_channel_macaddr, "ip_channel_macaddr");
    reg_add_media_ether_mtu(LINK_IP, ip_ether_compute_mtu,
			    "ip_ether_compute_mtu");
    reg_add_media_fddi_mtu(LINK_IP, ip_fddi_compute_mtu,"ip_fddi_compute_mtu");
    reg_add_media_atm_mtu(LINK_IP, ip_atm_compute_mtu,"ip_atm_compute_mtu");
    reg_add_media_channel_mtu(LINK_IP, ip_channel_compute_mtu, "ip_channel_compute_mtu");
    reg_add_media_change_mtu(ip_change_mtu, "ip_change_mtu");
    reg_add_media_default_mtu(ip_set_default_mtu, "ip_set_default_mtu");
    reg_add_media_protocol_up(ip_protocol_up, "ip_protocol_up");
    reg_add_set_dialer_list(LINK_IP, ip_set_dialer_list, "ip_set_dialer_list");
    reg_add_media_tring_add_functional(ip_set_tr_functional,
				       "ip_set_tr_functional");
}
