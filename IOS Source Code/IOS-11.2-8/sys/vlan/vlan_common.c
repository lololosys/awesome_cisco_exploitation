/*--------------------------------------------------------------------------
 *
 * $Id: vlan_common.c,v 3.9.6.15 1996/08/21 20:37:36 hampton Exp $
 *
 * vlan/vlan_common.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Definitions and functions supporting IEEE 802.10 and Inter Switch Link (ISL)
 * vLANs, and specifically the routing of protocols between Virtual LANs.
 *
 * This file contains functions common to the dot10 and ISL implementations
 * across the CxBus (i.e. C7000s), the CyBus (i.e. C7500s) and Low End
 * platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_common.c,v $
 * Revision 3.9.6.15  1996/08/21  20:37:36  hampton
 * Further isolate VLAN code from IPX internals.  [CSCdi66726]
 * Branch: California_branch
 *
 * Revision 3.9.6.14  1996/08/15  00:45:00  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.9.6.13  1996/08/13  02:26:07  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.9.6.12  1996/08/10  00:32:48  hou
 * CSCdi60803:  ISL encapsulated IP datagrams fragmented erroneously.
 * Branch: California_branch
 * - don't change ip mtu on ISL subinterface.
 *
 * Revision 3.9.6.11  1996/08/06  08:28:29  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.9.6.10  1996/07/27  00:15:12  ssangiah
 * CSCdi64084: c7200: Optimum/Flow switching from non-ISL to ISL interface
 *             is broken.
 * Branch: California_branch
 *
 * Revision 3.9.6.9  1996/07/26  00:08:20  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.9.6.8  1996/06/28  23:31:42  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.6.7  1996/06/24  16:05:51  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.9.6.6  1996/06/01  01:32:50  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.9.6.5  1996/05/23  07:38:34  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.9.6.4  1996/05/22  02:08:15  hou
 * CSCdi56274:  7507 send out ill formated ISL encap BPDU
 * Branch: California_branch
 * - write correct length field and sap for ISL BPDU packet.
 * - set default ICMP redirects as FALSE on isl vlan subinterface.
 *
 * Revision 3.9.6.3  1996/04/11  17:05:06  hou
 * CSCdi53052:  multiple vlan id causing vlan tbridging and routing problem
 * Branch: California_branch
 *
 * Revision 3.9.6.2  1996/03/23  01:34:29  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.9.6.1  1996/03/18  22:33:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/16  07:53:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.14.2  1996/03/13  02:11:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.14.1  1996/02/20  21:48:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/05  18:18:05  rbadri
 * CSCdi50472:  Transparent bridging with 802.10 does not work
 * Write the 802.10 header by moving datagramstart only by SDE_ENCAPBYTES.
 * When routing, don't check for tbridge_on_swidb on the interface.
 *
 * Revision 3.8  1996/02/25  03:52:45  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.7  1996/02/05  04:04:10  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.6  1996/01/23  08:55:28  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "ieee.h"
#include "packet.h"
#include "../ui/debug.h"
#include "config.h"

#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../ip/ip_registry.h"
#include "../srt/srt_registry.h"

#include "../h/encapstype.h"

#include "../ip/ip.h"
#include "../iprouting/route.h"

#include "../if/network.h"

#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/ether_inline.h"

#include "../if/arp.h"

#include "../srt/span.h"
#include "../tbridge/tbridge_sde.h"

#include "../smf/smf_public.h"

#include "vlan.h"
#include "vlan_debug.h"
#include "vlan_private.h"


/*
 *  Table of pointers to the Inter Synergy Link
 *  descriptor structures hashed by vLAN Colour.
 */

isl_vlan_descriptor *isl_vlan_table[MAX_ISL_VLANS];


/*
 * The following defines the ISL encapsulation header.
 *
 * This header array facilitates prepending an ISL encapsulation, since only a
 * few fields are actually used.
 *
 * Bytes 0 to 5 are the defined ISL Multicast Address.
 *
 * Bytes 6 to 11 will carry the Source MAC address of the egressing interface.
 *
 * This masquerades as a valid 802.3 packet on Fast Ethernet, therefore it
 * then bears a length field and a pseudo-SNAP encapsulation, carrying the
 * Cisco OUI.
 *
 * Bytes 20 and 21 have the ISL colour (vLAN ID) written into them, as
 * appropriate to the outgoing interface.
 */


uchar isl_header[ISL_HEADER_LENGTH] = { 0x01, 0x00, 0x0C, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0xAA, 0xAA, 0x03, 0x00,
                                        0x00, 0x0C, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00 };


/*
 * Register the Virtual LANs subsystem.
 */


#define VLAN_MAJVERSION      1
#define VLAN_MINVERSION      0
#define VLAN_EDITVERSION     1

SUBSYS_HEADER (vlan, VLAN_MAJVERSION, VLAN_MINVERSION, VLAN_EDITVERSION,
	       vlan_init, SUBSYS_CLASS_PROTOCOL, NULL, NULL);


/*
 * Retrieve the lowest configured ISL # for the given hwidb.
 * This is mainly for CDP to transmit only one packet on the wire.
 */

idbtype *isl_vlan_lowest_colour(hwidbtype *hwidb)
{
    idbtype *swidb, *found = NULL;
    ushort isl_colour = 0xFFFF;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	/*
	 * Check if ISL configured and CDP enabled.
	 */
	if (!swidb->isl_vlan || !swidb->cdp_enabled ||
	    !interface_up(swidb))
	    continue;

	if (swidb->isl_vlan->colour <= isl_colour) {
	    isl_colour = swidb->isl_vlan->colour;
	    found = swidb;
	}
    }

    return (found);
}

/*
 * Maintain the vLAN packet counters for Process Switched traffic.
 */

void vlan_packet_counters (paktype * vlan_packet, uchar vlan_encapsulation)
{

ulong *vlan_counter;

    if (vlan_encapsulation == ET_SDE)

	vlan_counter = vlan_packet->if_output->
	               sde_said_db->egressing_dot10_packets;

    else if (vlan_encapsulation == ET_ISL)

	vlan_counter = vlan_packet->if_output->isl_vlan->egressing_isl_packets;

    else
	return;

    switch (vlan_packet->linktype) {

	case LINK_IP:
            vlan_counter[VLAN_ROUTE_IP]++;
	    break;

	case LINK_NOVELL:
            vlan_counter[VLAN_ROUTE_NOVELL]++;
	    break;

	case LINK_APPLETALK:
            vlan_counter[VLAN_ROUTE_APPLE]++;
	    break;

	case LINK_DECNET:
            vlan_counter[VLAN_ROUTE_DECNET]++;
	    break;

	case LINK_CLNS:
            vlan_counter[VLAN_ROUTE_CLNS]++;
	    break;

	case LINK_XNS:
            vlan_counter[VLAN_ROUTE_XNS]++;
	    break;

	case LINK_VINES:
            vlan_counter[VLAN_ROUTE_VINES]++;
	    break;

	case LINK_APOLLO:
            vlan_counter[VLAN_ROUTE_APOLLO]++;
	    break;

	case LINK_BRIDGE:
            vlan_counter[VLAN_TBRIDGE]++;
	    break;
    }
}


/*
 * Remove any vLAN attributes when the subinterface is blown away.
 */

void vlan_adjust (idbtype *vlan_subinterface)
{

    if (vlan_subinterface->subif_state == SUBIF_STATE_DELETED) {

        if (IS_DOT10_VLAN_SUBINTERFACE(vlan_subinterface))
	    deencap_802_10_sde(vlan_subinterface, FALSE);

        if (IS_ISL_VLAN_SUBINTERFACE(vlan_subinterface))
	    deencap_isl_vlan(vlan_subinterface, FALSE);
    }
}


/*
 * Remove any bridging and/or routing subinterface attributes when the vLAN
 * encapsulation is removed from the interface - good housekeeping.
 */

void deconfigure_vlan_switching (idbtype *vlan_subinterface)
{

ipaddtype *ip_address_ptr, *temp_next_ip_ptr;

    if (vlan_subinterface->tbridge_on_swidb) {

	/*
	 * If deleting/changing vLAN encapsulation on a subinterface and
	 * the subinterface is bridging, remove the interface from the
	 * Bridge Group. Cannot have subinterface bridging without
	 * a vLAN encapsulation, be that ISL or 802.10 SDE.
	 */
        printf("\n\n%s vLAN encapsulation deleted, therefore"
	       " interface removed from Bridge Group %d.\n",
	       IS_ISL_VLAN_SUBINTERFACE(vlan_subinterface)? "ISL":"SDE",
	       vlan_subinterface->span_ptr->spanindex);

        if (vlan_subinterface->span_ptr->running)
            span_port_disable(vlan_subinterface->span_ptr, vlan_subinterface);

        span_delete_port(vlan_subinterface);
    }

    /* O.K. Let's now clear up any routing associated with this subinterface */

    if (vlan_subinterface->ip_enabled == TRUE) {

        vlan_subinterface->ip_enabled = FALSE;

        if (vlan_subinterface->ip_address)
            reg_invoke_ipcache_invalidate_range(vlan_subinterface->ip_address,
					     vlan_subinterface->ip_nets_mask);

        vlan_subinterface->ip_address = 0L;  /* look at ip_unnumbered */

        ip_rxtypes_update(vlan_subinterface);

	/* Clear all secondary addresses!! */

        for (ip_address_ptr = (ipaddtype *)
	                      vlan_subinterface->ip_addressQ.qhead;
             ip_address_ptr; ip_address_ptr = temp_next_ip_ptr) {

                reg_invoke_ipcache_invalidate_range(ip_address_ptr->address,
						    ip_address_ptr->mask);
                temp_next_ip_ptr = ip_address_ptr->next;
                free(ip_address_ptr);
        }

        vlan_subinterface->ip_addressQ.qhead = NULL;
        vlan_subinterface->ip_addressQ.qtail = NULL;
        vlan_subinterface->ip_addressQ.count = 0;
        vlan_subinterface->ip_unnumbered = NULL;
        reg_invoke_ip_address_change(vlan_subinterface, 0, 0, FALSE, FALSE);
        reg_invoke_clear_idb_routes(vlan_subinterface);
        arp_flush_cache(vlan_subinterface);
        recompute_syssec();
        fixup_ipsockets();
        ipaddr_table_rebuild(vlan_subinterface);
    }

    /*
     * IPX can have subinterfaces independent of vLAN routing, so if configured
     * with a Novell network number then return to the default encapsulation.
     *
     * For ISL on FastEthernet, it should actually be Ethernet V2 rather than
     * the deprecated "Novell Ether".
     */
    reg_invoke_set_protocol_encaps(LINK_NOVELL, vlan_subinterface,
				   ET_NULL /* will set default */);
}


/*
 * Generic encapsulation routine which configures a subinterface for
 * Inter Switch Link (ISL) vLAN encapsulation.
 *
 * Note encapsulation is currently only allowed on Fast Ethernet LAN
 * interfaces.
 *
 * This routine writes the ISL colour into the ISL vLAN Descriptor structure
 * and fill in the back pointer to the subinterface IDB it's configured on.
 *
 * Nota Bene: We maintain an array, indexed by hardware interface number, of 
 *            the software IDBs associated with this vLAN ID (colour) value;
 *            so that when we receive an ISL encapsulated packet we can
 *            demultiplex (based on a hash derived from the colour field) into
 *            an appropriate SWIDB. From the swidb we can the check if we're
 *            configured to route or bridge the interior packet type.
 *
 * If ISL vLAN ID (colour) not already present then malloc the necessary
 * space. Assume entries are zeroed upon creation, so no need to
 * initialize.
 */

void encap_isl_vlan (parseinfo *csb)
{

hwidbtype             *hwidb;
idbtype               *swidb, *idb;
isl_vlan_descriptor   *isl_vlan_index, *previous_isl_vlan_ptr;
ushort                encapsulation;
ushort                configured_isl_vlan_colour;
uchar                 index;
int                   subif_index;

    idb = csb->interface;   
    hwidb = idb->hwptr;

    encapsulation = GETOBJ(int, 1);

    configured_isl_vlan_colour = GETOBJ(int, 2);

    /*
     * Only supported on Fast Ethernet interfaces.
     *
     * Let's give decent error messages for this feature!
     */

    if (encapsulation != ET_ISL)
        return;

    if (!(is_isl_vlan_capable(idb))) {
        printf("\n\nISL vLAN encapsulation not supported"
	       " on this interface type.\n");
	return;
    }

    /*
     * Only one subinterface per hardware IDB can be
     * configured to be within the same Virtual LAN.
     */

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
    {
	if ((swidb->isl_vlan) 
            && ((swidb->isl_vlan->colour == configured_isl_vlan_colour)
            && (swidb != idb))) {

            printf("\n\nConfiguration of multiple subinterfaces of the same"
                   " main interface\nwith the same ISL vLAN colour (%d) is"
                   " not permitted."
                   "\n\nThis ISL colour is already configured on interface"
		   " %s.\n", configured_isl_vlan_colour, swidb->namestring);
            return;
        }
    }

    /*
     * Allow for fast switching of maximum MTU packets with the additional
     * vLAN encapsulation (mtu + encaps). Long word aligned.
     *
     * Set the IP MTU regardless to avoid fragmentation of 1500 byte
     * IP datagrams. 
     */

    if_maxdgram_adjust(hwidb, hwidb->max_buffer_size + ISL_HEADER_LENGTH);

    /*
     * VLAN's can have larger than media size frames, take that into account.
     */
    if (hwidb->max_buffer_size > hwidb->max_pak_size) {
	hwidb->max_pak_size = hwidb->max_pak_size_def + ISL_HEADER_LENGTH;
    }


    idb->ip_routecache_same = idb->ip_routecache_same_def = TRUE;

    /* Disable ICMP redirects on ISL vLAN subinterfaces by default */

    idb->ip_redirect = FALSE;

    /* Set up how ARP is done. */ 
    idb->arp_arpa = idb->arp_arpa_def = TRUE;
    idb->arp_probe = idb->arp_probe_def = FALSE;
    idb->arp_snap = idb->arp_snap_def = FALSE;
    idb->arp_timeout = ARP_MAXAGE;

    /*
     * IPX can have subinterfaces independent of vLAN bridging/routing, so
     * if configured with a Novell network number then override any default
     * encapsulation.
     */
    reg_invoke_set_protocol_encaps(LINK_NOVELL, idb, ET_ISL);

    /*
     * Walk the linked list of ISL vLAN Descriptors, typically there'll only
     * be one.
     *
     * Note we can do one of three things here:
     *
     * 1.  Add a new ISL structure to the hash table. Also add it to
     *     the subinterface software IDB.
     *
     * 2.  Link a new ISL structure to an existing chain.
     *
     * 3.  Add a subinterface to an existing ISL entry. i.e. another
     *     subinterface of a different physical interface supporting this
     *     ISL colour. Adding multiple subinterfaces to a virtual LAN is
     *     done via using the hardware IDBs interface index as a subscript
     *     into an array of MAX interfaces.
     */

    /* Obtain hash index from the ISL vLAN ID (i.e. Colour) */

    index = (configured_isl_vlan_colour >> 8);    
    index ^= configured_isl_vlan_colour;

    previous_isl_vlan_ptr = NULL;
    for (isl_vlan_index = isl_vlan_table[index]; isl_vlan_index != NULL; 
         isl_vlan_index = isl_vlan_index->next_isl_vlan_db) {

    /* If we've found an ISL colour matching that entered */

    if (isl_vlan_index->colour == configured_isl_vlan_colour) {

            isl_vlan_index->colour = configured_isl_vlan_colour;
	    subif_index = hwidb->hw_if_index;
            isl_vlan_index->isl_vlan_subinterface[subif_index]
		                              = idb;
            idb->isl_vlan = isl_vlan_index;
            isl_vlan_index->number_subif_in_isl_vlan++;

            /* 
             * Having configured an interface for ISL vLAN encapsulation now
             * program the FEIP address filter (currently supporting 32 entries
	     * to absorb ISL multicast packets received on that interface, if
	     * we haven't already. This check avoids dulpicates ...
             */

	    if (hwidb->isl_vlan_on_hwidb != TRUE) {
		hwidb->isl_vlan_on_hwidb = TRUE;
                if_multi_add(hwidb, idb, isl_vlan_multicast);
                if ((hwidb->type == IDBTYPE_FEIP) && 
                    !(hwidb->status & IDB_CBUS))
                    (*hwidb->reset)(hwidb);
	    }

	    return;
        }
        previous_isl_vlan_ptr = isl_vlan_index;
    }

    /* Else add a new ISL vLAN (colour) descriptor to the linked list */
    isl_vlan_index = malloc(sizeof(isl_vlan_descriptor));
    if (isl_vlan_index == NULL)
        return;

    if (previous_isl_vlan_ptr)
        previous_isl_vlan_ptr->next_isl_vlan_db = isl_vlan_index;
    else
        isl_vlan_table[index] = isl_vlan_index;     /* First on list */

    isl_vlan_index->colour = configured_isl_vlan_colour;
    subif_index = hwidb->hw_if_index;
    isl_vlan_index->isl_vlan_subinterface[subif_index] = idb;
    csb->interface->isl_vlan = isl_vlan_index;
    isl_vlan_index->number_subif_in_isl_vlan++;

    if (hwidb->isl_vlan_on_hwidb != TRUE) {
        hwidb->isl_vlan_on_hwidb = TRUE;
        if_multi_add(hwidb, idb, isl_vlan_multicast);
        if ((hwidb->type == IDBTYPE_FEIP) && !(hwidb->status & IDB_CBUS))
            (*hwidb->reset)(hwidb);
    }
}


/*
 * Deencapsulation routine for ISL vLAN encapsulating interfaces.
 */

void deencap_isl_vlan (idbtype *swidb, boolean sense)
{

isl_vlan_descriptor  *isl_vlan_ptr, *previous_isl_vlan_ptr;
uchar                index;
int                  subif_index;
ushort               isl_vlan_colour;

    /*
     * If no other vLAN encapsulation or ISL vLAN descriptor
     * on the interface, then we have nothing to do.
     */

    /* If other vLAN encapsulations is on the interface, we
     * have to do more clear up than just change the vLAN ID
     */
    if (swidb->sde_said_db != NULL)
        reg_invoke_deencap_802_10_sde(swidb, FALSE);

    if (swidb->isl_vlan == NULL)
        return;

    /*
     * If deleting ISL vLAN encapsulation on a subinterface and the
     * subinterface is configured for bridging and/or routing then
     * tidy up by deconfiguring ...
     */

    if (!sense)
        deconfigure_vlan_switching(swidb);

    /*
     * In deencapsulating an ISL vLAN subinterface we have three scenarios:
     *
     * 1.  If the ISL vLAN descriptor is the first entry in the hash chain and
     *     it is only applied to this interface then we can blow it away;
     *     afterall each ISL descriptor requires > 1K. This is the case where
     *     the Network Administrator is deconfiguring the feature.
     *
     * 2.  If it's not the first entry in the hash chain then we need to tidy
     *     up the linked list.
     *
     * 3.  If the ISL vLAN Identifier (colour) is applied on other interfaces
     *     then just remove this software IDB from the array of interfaces.
     */

    /* Obtain hash index from the ISL vLAN ID (Colour) */

    isl_vlan_colour = swidb->isl_vlan->colour;

    index = (isl_vlan_colour >> 8);
    index ^= isl_vlan_colour;

    previous_isl_vlan_ptr = NULL;
    for (isl_vlan_ptr = isl_vlan_table[index]; isl_vlan_ptr; 
	 isl_vlan_ptr = isl_vlan_ptr->next_isl_vlan_db) {

        if (isl_vlan_ptr == swidb->isl_vlan) {

            if (isl_vlan_ptr->number_subif_in_isl_vlan == 1) {

                /* 
                 * If not the initial entry in the hash chain then link
		 * previous and next, otherwise clear the said_table entry.
                 */

	        if (previous_isl_vlan_ptr != NULL)
		    previous_isl_vlan_ptr->next_isl_vlan_db 
		    = isl_vlan_ptr->next_isl_vlan_db;

		else 
		    isl_vlan_table[index] = isl_vlan_ptr->next_isl_vlan_db;

                free(swidb->isl_vlan);
                swidb->isl_vlan = NULL;

                return;

            } else {

	    /*
	     * There exist other subinterfaces belonging to this Virtual
	     * LAN therefore just delete the reference to this SWIDB in
	     * the ISL vLAN descriptor structure.
	     */

                subif_index = swidb->hwptr->hw_if_index;
	        isl_vlan_ptr->isl_vlan_subinterface[subif_index] = NULL;
		isl_vlan_ptr->number_subif_in_isl_vlan--;
		return;
            }
        }

        previous_isl_vlan_ptr = isl_vlan_ptr;
    }
}


/*
 * Called if configuring a bridging subinterface for ISL Virtual LANs.
 * (i.e. bridging out an ISL encapsulating software IDB).
 *
 * Check that media is Fast Ethernet, ISL vLANs currently only supported on
 * 100 Meg Ethernet.
 *  
 * Also subinterface vLAN bridging only works in conjunction with the
 * IEEE 802.1d version of our friend the Spanning Tree.
 *
 * Note to allow bridging between ISL vLANs there is no restriction on the
 * number of ISL encapsulating vLAN subinterfaces you can have within a 
 * Bridge Group.
 */
	    
boolean isl_vlan_bridging_capable_subinterface (idbtype *input_swidb,
						int index, spantype *span)
{
    idbtype *swidb;
    int span_index;

    if (input_swidb->hwptr->type != IDBTYPE_FEIP) {

        printf("\n\nBridging on an ISL encapsulating subinterface only"
	       " supported\non Fast Ethernet Interfaces.\n");
        return(FALSE);
    }

    if (span->protocol != SPAN_PROTO_IEEE) {

        printf("\n\nBridging on a subinterface only supported"
               "\nin conjunction with the IEEE Spanning Tree.\n");
        return(FALSE);
    }

    else {

        FOR_ALL_SWIDBS(swidb)
	{
	    span_index = idb_sw_get_spanindex(swidb);
            if ((swidb->isl_vlan != NULL)
	        && (swidb->isl_vlan == input_swidb->isl_vlan)
                && (span_index != 0)
		&& (span_index != index)) {

                   printf("\n\nWarning ISL Virtual LAN ID %d already"
                          " in use within Bridge Group %d.\n",
                          swidb->isl_vlan->colour, span_index);
	    }
        }
    }

    /*
     *  Now set the tbridge_media on the subinterface software IDB - used
     *  by the switching logic to build the ISL encapsulated packet.
     */

    input_swidb->tbridge_media = TBR_ETHER_ISL_VLAN;

    return (TRUE);
}


/*
 * Write the ISL encapsulation and colour to Non-Volatile Memory.
 */

void isl_vlan_interface_nvram_write (parseinfo *csb)
{
    if (csb->interface->isl_vlan) 
       nv_write(TRUE,"encapsulation isl %d",csb->interface->isl_vlan->colour);
}


/*
 * write_dot10_encapsulation ()
 *
 * Roll back and lay down an IEEE 802.10 vLAN encapsulation header prepended
 * to a network buffer. Called from the FDDI and `Broken Ring' vencaps
 * functions if there is a dot10 SAID associated with this software IDB.
 *
 * Nota Bene: It assumes that the caller has wound back datagramstart and
 *            stuffed in the AC/FC bytes and setup pak->addr_start as
 *            appropriate.
 */

boolean write_dot10_encapsulation (paktype *pak, idbtype *output_swidb)
{

register ulong said_colour;
register uchar *packet_write_ptr;

    said_colour = output_swidb->sde_said_db->sdb_said;

    if (output_swidb->hwptr->status & IDB_FDDI)
        packet_write_ptr = pak->datagramstart + FDDI_FC;

    /* Currently only supported on FDDI, Ethernet flavours and Token Ring */

    else
        packet_write_ptr = pak->datagramstart + TR_ACFC_SIZE;

    /* Destination and Source Address copied */

    ieee_copy(pak->addr_start, packet_write_ptr);
    ieee_copy(pak->addr_start + IEEEBYTES, packet_write_ptr + IEEEBYTES);

    packet_write_ptr += (2 * IEEEBYTES);

    /* Dot10 LLC Type 1, SAID, Station ID and no Fragment Indicator */

    PUTSHORT(packet_write_ptr, (SAP_SDE | SAP_SDE << 8));
    packet_write_ptr += 2;
    
    PUTSHORT(packet_write_ptr, (LLC1_UI << 8));
    packet_write_ptr += 1;

    PUTLONG(packet_write_ptr, said_colour);
    packet_write_ptr += 4;

    ieee_copy(pak->addr_start + IEEEBYTES, packet_write_ptr);

    /* Flag is zero for now - no fragmentation support */

    return(TRUE);
}


/*
 * write_ether_dot10_encapsulation ()
 *
 * Roll back and lay down an IEEE 802.10 vLAN encapsulation header on Ethernet.
 * Called from the ether_vencap() function if there is a dot10 SAID associated
 * with this software IDB.
 */

boolean write_ether_dot10_encapsulation (paktype *pak, idbtype *output_swidb)
{

register ulong said_colour;
register uchar *packet_write_ptr;

    said_colour = output_swidb->sde_said_db->sdb_said;

    /* Wind back the write pointer, including an 802.3 length */

    packet_write_ptr = pak->datagramstart - SDE_ENCAPBYTES;

    /* Destination and Source Address copied */

    ieee_copy(pak->datagramstart, packet_write_ptr);
    ieee_copy(pak->datagramstart + IEEEBYTES, packet_write_ptr + IEEEBYTES);

    packet_write_ptr += (2 * IEEEBYTES);

    PUTSHORT(packet_write_ptr, pak->datagramsize += SDE_ENCAPBYTES);
    packet_write_ptr += 2;

    /* Dot10 LLC Type 1, SAID, Station ID and no Fragment Indicator */

    PUTSHORT(packet_write_ptr, (SAP_SDE | SAP_SDE << 8));
    packet_write_ptr += 2;
    
    PUTSHORT(packet_write_ptr, (LLC1_UI << 8));
    packet_write_ptr += 1;

    PUTLONG(packet_write_ptr, said_colour);
    packet_write_ptr += 4;

    ieee_copy(pak->datagramstart + IEEEBYTES, packet_write_ptr);

    pak->datagramstart -= SDE_ENCAPBYTES;

    /* Flag is zero for now - no fragmentation support */

    return(TRUE);
}


/*
 * isl_multicast_address_update
 *
 * Called via LIST registry "reg_invoke_media_interesting_addresses" when
 * a change in the listening state of an interface has taken place.
 * With an ISL vLAN trunk, the interface should only be listening selectively
 * for the ISL multicast but let's not assume that it isn't in promiscuous
 * mode.
 *
 * This is only applicable to Fast Ethernet interfaces configured for ISL
 * vLAN routing.
 *
 * If configured on the subinterface, update the Software MAC Filter so that
 * the ISL specific MAC address is inserted (the interface is listening
 * promiscuously) or deleted (the interface is listening selectively).
 * In addition, populate the hi-end hardware address filter with the ISL
 * multicast.
 *
 * If ISL is not configured on the subinterface, simply delete the ISL
 * specific MAC address from the SMF if present.
 */

static void isl_multicast_address_update (idbtype *swidb)
{

    if (IS_ISL_VLAN_SUBINTERFACE(swidb)) {

        reg_invoke_tbridge_smf_update(isl_vlan_multicast, swidb,
                                      SMF_MACTYP_ISL_VLAN_MCAST |
                                      SMF_ACTION_RECEIVE);

        if_multi_add(swidb->hwptr, swidb, isl_vlan_multicast);

    } else {

        reg_invoke_tbridge_smf_delete(isl_vlan_multicast, swidb,
                                      SMF_MACTYP_ISL_VLAN_MCAST |
                                      SMF_ACTION_RECEIVE);
    }
}

/* isl_enqueue
 * Take out the isl header, retrieve the encap type and queue
 * the pkt into the corresponding queue. ISL pkt which doesn't
 * hit fastswitch path comes here, eg echo packet for itself
 * on those interface which cann't set HEARSELF bit.
 */

void isl_enqueue (paktype *pak)
{
    ushort  isl_colour;
    idbtype *isl_vlan_interface;

    /*
     * Firstly extract the ISL colour field i.e. vLAN ID, since we got 
     * this pkt from process level, it should have been copied into the
     * pak buffer, we can read the vLAN color field directly from pak, 
     * even on the mci platform.
     */
    isl_colour = (GETSHORT(pak->datagramstart + ISL_VLAN_ID_OFFSET) >> 1);
    isl_vlan_interface = isl_colour_2_swidb(pak->if_input->hwptr, isl_colour);

    /*
     * In the event of receiving an ISL packet with a vLAN Identifier
     * (i.e. colour) that is not supported/configured on a software IDB
     * belonging to this interface, or the subinterface is shutdown, 
     * then drop the pkt.
     */
    if ((isl_vlan_interface == NULL) ||
	(isl_vlan_interface->subif_state != SUBIF_STATE_UP)) {
	datagram_done(pak);
	return;
    }

    /* 
     * There is no particle case in process level, don't have to worry
     * about particle packet.
     */
    pak->datagramstart += ISL_HEADER_LENGTH;
    pak->datagramsize  -= ISL_HEADER_LENGTH;

    /* 
     * ISL is only supported on FastEthernet interface, don't have to 
     * worry about other media.
     */
    pak->rxtype = ether_decode_encapsulation_inline(pak,
                                          (ether_hdr *)pak->datagramstart);
    raw_enqueue(pak->if_input->hwptr, pak);
}


void vlan_init (subsystype *subsys)
{

    vlan_debug_init(); 

    vlan_parser_init();

    /*
     * Register the necessary platform specific functions with the system
     * registry for the Fast Switching Tables.
     */

    vlan_register_fastswitching();

    reg_add_isl_vlan_bridging_capable_subinterface
                                   (isl_vlan_bridging_capable_subinterface,
                                    "isl_vlan_bridging_capable_subinterface");

    reg_add_write_dot10_encapsulation((service_write_dot10_encapsulation_type)
				      write_dot10_encapsulation,
                                      "write_dot10_encapsulation");

    reg_add_write_ether_dot10_encapsulation(
			        (service_write_ether_dot10_encapsulation_type)
				write_ether_dot10_encapsulation,
                                "write_ether_dot10_encapsulation");

    reg_add_media_ether_macwrite(ET_ISL, write_isl_encapsulation,
                                 "write_isl_encapsulation");

    reg_add_encaps_nvram_write(ET_ISL, isl_vlan_interface_nvram_write,
                               "isl_vlan_interface_nvram_write");

    reg_add_deencap_isl_vlan(deencap_isl_vlan, "deencap_isl_vlan");

    reg_add_encap_isl_vlan(encap_isl_vlan, "encap_isl_vlan");

    reg_add_vlan_packet_counters(vlan_packet_counters, "vlan_packet_counters");

    reg_add_vlan_adjust(vlan_adjust, "vlan_adjust");

    reg_add_media_interesting_addresses(isl_multicast_address_update,
                                        "isl_multicast_address_update");

    reg_add_deconfigure_vlan_switching(deconfigure_vlan_switching,
				       "deconfigure_vlan_switching");

    reg_add_isl_vlan_lowest_colour(isl_vlan_lowest_colour,
				   "isl_vlan_lowest_colour");

    reg_add_raw_enqueue(LINK_ISL, isl_enqueue, "isl_enqueue");
}


/*
 * There now follows Parser/Configuration supporting routines for vLANs.
 */


/*
 * show_vlans
 *
 * Display Virtual LAN configuration.
 *
 * Search for configured dot10 and ISL vLANs, unless a vLAN ID is given.
 */

void show_vlans (parseinfo *csb)
{

register int         vlan_tag, index, index2;
register uchar       bucket = 0;	/* Compiler needs this */
said_descriptor     *dot10_db;
isl_vlan_descriptor *isl_db;
idbtype             *swidb_index = NULL;
boolean              onceflag = FALSE;

    automore_enable(NULL);

    /*
     * If `vlan_tag' is zero we'll display all vLAN interfaces.
     *
     * Parser `NUMBER' MACRO will only accept a valid index.
     */

    vlan_tag = GETOBJ(int, 1);

    if (vlan_tag) {

	bucket = ((vlan_tag >> 8) ^ vlan_tag);

        if ((said_table[nethash(vlan_tag)] == NULL) &&
	    isl_vlan_table[bucket] == NULL) {

	    printf("\n\nNo subinterface configured for "
		   "vLAN Identifier %d.\n\n", vlan_tag);
            return;
        }
    }

    printf("\n");

    /*
     * Iterate through all vlan descriptors, firstly dot10 vLANs.
     */

    for (index = 0; index < MAX_SAIDS; index++) {

        if ((vlan_tag) && (nethash(vlan_tag) != index))
            continue;
	
	dot10_db = said_table[index];

	while(dot10_db != NULL) {

            if (vlan_tag && vlan_tag != dot10_db->sdb_said) {
                dot10_db = dot10_db->next_said_db;
                continue;
            }

	    onceflag = TRUE;

	    printf("\nVirtual LAN ID:  %d", dot10_db->sdb_said);
	    printf(" (IEEE 802.10 Encapsulation)\n\n");

	    printf("   vLAN Trunk Interface%s",
		   ((dot10_db->sdb_number_subif_in_bgroup > 1) ? "s:  "
	             :":   "));

	    for (index2 = 0; index2 < MAX_INTERFACES; index2++) {

		if (dot10_db->sdb_sub_interface[index2] != NULL)
		    printf("%s\n",
			   dot10_db->sdb_sub_interface[index2]->namestring);
	    }

	    printf("\n   Protocols Configured:   Address:"
	           "              Received:        Transmitted:\n");

	    for (index2 = 0; index2 < MAX_INTERFACES; index2++) {

                /*
                 * Go through and display only those protocols configured
                 * on a particular vLAN subinterface.
                 */

                if (dot10_db->sdb_sub_interface[index2] != NULL) {

		    swidb_index = dot10_db->sdb_sub_interface[index2];

		    if (reg_invoke_routed_proto_on_swidb(LINK_IP,
							 swidb_index)) {

                        printf("           IP              ");
		        printf("%15i%-15d%-20d\n",
			   idb_sw_get_ipaddress(swidb_index),
			   dot10_db->ingressing_dot10_packets[VLAN_ROUTE_IP],
			   dot10_db->egressing_dot10_packets[VLAN_ROUTE_IP]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_NOVELL,
							 swidb_index)) {

                        printf("           IPX     ");
                        printf("%-8x.%e%-15d%-20d\n",
			idb_sw_get_ipxnet(swidb_index),
			idb_sw_get_ipxnode(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_ROUTE_NOVELL],
			dot10_db->egressing_dot10_packets[VLAN_ROUTE_NOVELL]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_APPLETALK,
							 swidb_index)) {

                        printf("         AppleTalk         ");
		        printf("%15i%-15d%-20d\n",
			idb_sw_get_atalknet(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_ROUTE_APPLE],
			dot10_db->egressing_dot10_packets[VLAN_ROUTE_APPLE]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_DECNET,
							 swidb_index)) {

                        printf("          DECnet           ");
		        printf("%15d%-15d%-20d\n",
			idb_sw_get_dnnet(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_ROUTE_DECNET],
			dot10_db->egressing_dot10_packets[VLAN_ROUTE_DECNET]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_XNS,
							 swidb_index)) {

                        printf("          XNS              ");
                        printf("%-8u.%e%-15d%-20d\n",
			idb_sw_get_xnsnet(swidb_index),
			idb_sw_get_xnsnode(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_ROUTE_XNS],
			dot10_db->egressing_dot10_packets[VLAN_ROUTE_XNS]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_CLNS,
							 swidb_index)) {

                        printf("          CLNS             ");
		        printf("%15s%-15d%-20d\n",
			idb_sw_get_clnsnet(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_ROUTE_CLNS],
			dot10_db->egressing_dot10_packets[VLAN_ROUTE_CLNS]);
		    }

                    if (reg_invoke_proto_on_swidb(LINK_BRIDGE, swidb_index)) {

                        printf("        Bridging        Bridge Group ");
		        printf("%3d%-17d%-20d\n",
			idb_sw_get_spanindex(swidb_index),
			dot10_db->ingressing_dot10_packets[VLAN_TBRIDGE],
			dot10_db->egressing_dot10_packets[VLAN_TBRIDGE]);
		    }
		}
            }

	    dot10_db = dot10_db->next_said_db;
	}
    }

    /*
     * Now scan the Inter Switch Link vLAN Descriptors.
     */

    for (index = 0; index < MAX_ISL_VLANS; index++) {

        if (vlan_tag) {
            if (bucket != index)
                continue;
        }
	
	isl_db = isl_vlan_table[index];

	while(isl_db != NULL) {

            /* Skip unspecified colour */
            if (vlan_tag && vlan_tag != isl_db->colour) {
                isl_db = isl_db->next_isl_vlan_db;
                continue;
            }

            onceflag = TRUE;

	    printf("\nVirtual LAN ID:  %d", isl_db->colour);
	    printf(" (Inter Switch Link Encapsulation)\n\n");

	    /*
	     * Typically there should only be a
	     * single subinterface per vLAN/subnet.
	     */

	    printf("   vLAN Trunk Interface%s",
		   ((isl_db->number_subif_in_isl_vlan > 1) ? "s:  ":":   "));

	    for (index2 = 0; index2 < MAX_INTERFACES; index2++) {

		if (isl_db->isl_vlan_subinterface[index2] != NULL)
		    printf("%22s",
			   isl_db->isl_vlan_subinterface[index2]->namestring);
	    }

	    /* Indicate (for the C7000 platform) if SSE Switching */

	    if (isl_db->vlan_switching_path == vLAN_SSE_SWITCHING)
		printf("(Silicon Switched)");

	    printf("\n\n   Protocols Configured:   Address:"
	           "              Received:        Transmitted:\n");

	    for (index2 = 0; index2 < MAX_INTERFACES; index2++) {

                /*
                 * Go through and display only those protocols configured
                 * on a particular vLAN subinterface.
                 */

                if (isl_db->isl_vlan_subinterface[index2] != NULL) {

		    swidb_index = isl_db->isl_vlan_subinterface[index2];

		    if (reg_invoke_routed_proto_on_swidb(LINK_IP,
							 swidb_index)) {

                        printf("           IP              ");
		        printf("%15i%-15d%-20d\n",
			idb_sw_get_ipaddress(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_IP],
			isl_db->egressing_isl_packets[VLAN_ROUTE_IP]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_NOVELL,
							 swidb_index)) {

                        printf("           IPX     ");
                        printf("%-8x.%e%-15d%-20d\n",
			idb_sw_get_ipxnet(swidb_index),
			idb_sw_get_ipxnode(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_NOVELL],
			isl_db->egressing_isl_packets[VLAN_ROUTE_NOVELL]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_APPLETALK,
							 swidb_index)) {

                        printf("         AppleTalk         ");
		        printf("%15i%-15d%-20d\n",
			idb_sw_get_atalknet(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_APPLE],
			isl_db->egressing_isl_packets[VLAN_ROUTE_APPLE]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_DECNET,
							 swidb_index)) {

                        printf("           DECnet          ");
		        printf("%15d%-15d%-20d\n",
			idb_sw_get_dnnet(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_DECNET],
			isl_db->egressing_isl_packets[VLAN_ROUTE_DECNET]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_XNS,
							 swidb_index)) {

                        printf("           XNS             ");
                        printf("%-8u.%e%-15d%-20d\n",
			idb_sw_get_xnsnet(swidb_index),
			idb_sw_get_xnsnode(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_XNS],
			isl_db->egressing_isl_packets[VLAN_ROUTE_XNS]);
		    }

                    if (reg_invoke_routed_proto_on_swidb(LINK_CLNS,
							 swidb_index)) {

                        printf("          CLNS            ");
		        printf("%15s%-15d%-20d\n",
			idb_sw_get_clnsnet(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_ROUTE_CLNS],
			isl_db->egressing_isl_packets[VLAN_ROUTE_CLNS]);
		    }

                    if (reg_invoke_proto_on_swidb(LINK_BRIDGE, swidb_index)) {

                        printf("        Bridging        Bridge Group ");
		        printf("%3d%-17d%-20d\n",
			idb_sw_get_spanindex(swidb_index),
			isl_db->ingressing_isl_packets[VLAN_TBRIDGE],
			isl_db->egressing_isl_packets[VLAN_TBRIDGE]);

		    }
		}
            }

	    isl_db = isl_db->next_isl_vlan_db;
	}
    }

    if (onceflag == FALSE)
        printf("\nNo Virtual LANs configured.\n");

    automore_disable();
}


/*
 * vlan_clear_statistics
 *
 * Clear Virtual LAN statistical counters.
 */

void vlan_clear_statistics (parseinfo *csb)
{

register int         vlan_tag, index;
register uchar       bucket = 0;	/* Compiler needs this */
said_descriptor     *dot10_db;
isl_vlan_descriptor *isl_db;
boolean              onceflag = FALSE;

    /*
     * If `vlan_tag' is zero clear the statistics associated with all
     * configured vLANs.
     *
     * Parser `NUMBER' MACRO will only accept a valid index.
     */

    vlan_tag = GETOBJ(int, 1);

    if (vlan_tag) {

	bucket = ((vlan_tag >> 8) ^ vlan_tag);

        if ((said_table[nethash(vlan_tag)] == NULL) &&
	    isl_vlan_table[bucket] == NULL) {

	    printf("\n\nNo subinterface configured for "
		   "vLAN Identifier %d.\n", vlan_tag);
            return;
        }
    }

    /*
     * Iterate through all vlan descriptors, dot10 and ISL vLANs.
     */

    for (index = 0; index < MAX_SAIDS; index++) {

        if ((vlan_tag) && (nethash(vlan_tag) != index))
            continue;
	
	dot10_db = said_table[index];

	while(dot10_db != NULL) {

            if (vlan_tag && vlan_tag != dot10_db->sdb_said) {
                dot10_db = dot10_db->next_said_db;
                continue;
            }

	    onceflag = TRUE;

	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_IP]     = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_IP]      = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_NOVELL] = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_NOVELL]  = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_APPLE]  = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_APPLE]   = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_DECNET] = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_DECNET]  = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_CLNS]   = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_CLNS]    = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_XNS]    = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_XNS]     = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_VINES]  = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_VINES]   = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_ROUTE_APOLLO] = 0;
	    dot10_db->egressing_dot10_packets[VLAN_ROUTE_APOLLO]  = 0;
	    dot10_db->ingressing_dot10_packets[VLAN_TBRIDGE]      = 0;
	    dot10_db->egressing_dot10_packets[VLAN_TBRIDGE]       = 0;

	    dot10_db = dot10_db->next_said_db;
	}
    }

    for (index = 0; index < MAX_ISL_VLANS; index++) {

        if (vlan_tag) {
            if (bucket != index)
                continue;
        }

        isl_db = isl_vlan_table[index];

        while(isl_db != NULL) {

            /* Skip unspecified colour */
            if (vlan_tag && vlan_tag != isl_db->colour) {
                isl_db = isl_db->next_isl_vlan_db;
                continue;
            }

            onceflag = TRUE;

	    isl_db->ingressing_isl_packets[VLAN_ROUTE_IP]      = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_IP]       = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_NOVELL]  = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_NOVELL]   = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_APPLE]   = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_APPLE]    = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_DECNET]  = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_DECNET]   = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_CLNS]    = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_CLNS]     = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_XNS]     = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_XNS]      = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_VINES]   = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_VINES]    = 0;
	    isl_db->ingressing_isl_packets[VLAN_ROUTE_APOLLO]  = 0;
	    isl_db->egressing_isl_packets[VLAN_ROUTE_APOLLO]   = 0;
	    isl_db->ingressing_isl_packets[VLAN_TBRIDGE]       = 0;
	    isl_db->egressing_isl_packets[VLAN_TBRIDGE]        = 0;

	    isl_db = isl_db->next_isl_vlan_db;
	}
    }

    if (onceflag == FALSE)
        printf("\n\nNo Virtual LANs configured.\n");
}
