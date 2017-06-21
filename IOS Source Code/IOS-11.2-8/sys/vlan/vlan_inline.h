/*--------------------------------------------------------------------------
 *
 * $Id: vlan_inline.h,v 3.10.6.8 1996/08/15 21:06:37 wilber Exp $
 *
 * vlan/vlan_inline.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Inline functions specific to the protocol independent routing and switching
 * between IEEE 802.10 based and Inter Switch Link (ISL) Virtual LANs on
 * CyBus platforms, i.e. Cisco 7500s and the Low End Platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_inline.h,v $
 * Revision 3.10.6.8  1996/08/15  21:06:37  wilber
 * CSCdi66158:  ISL routable packets get process-switched when IRB is
 * enabled
 * Give ISL routable pkt received on a bridged subinterface a chance to
 * lookup the fast cache
 * Branch: California_branch
 *
 * Revision 3.10.6.7  1996/08/15  00:45:03  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.10.6.6  1996/07/17  21:40:59  wilber
 * CSCdi62840:  line protocol goes down on FE when irb is configured
 * Don't pass BVI's hwidb to rsp_queue_ether_for_process()
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/07/17  18:36:34  tylin
 * CSCdi62787:  CDP does not work correctly with ISL encapsulation
 *    CDP packet should be always SNAP encapsulated.
 * Branch: California_branch
 *
 * Revision 3.10.6.4  1996/06/28  21:57:37  dawnli
 * CSCdi56696:  CDP-EV: Bad checksum in header
 * Branch: California_branch
 * If a CDP packet is ISL encapsulated, datagramsize is incorrect.
 * It includes ISL header length, which cause bad checksum.
 * If a regular CDP packet, not ISL encapsulated, is received,
 * if_input may set to an ISL sub-interface.
 *
 * Adjust datagramsize to the right value, and reset fast_rcvidb to
 * firstsw, so that if_input will be set to the right swidb.
 *
 * Revision 3.10.6.3  1996/05/24  20:33:08  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/03/29  19:20:56  hou
 * CSCdi45052:  4500 crashed on vLAN 802.10 routing while pinging 30000
 * packets
 * Branch: California_branch
 * - vlan_route_fddi_dot10_packet()
 *   changing of input_packet->data_area caused bsi eventually lose
 *   buffer.
 *
 * also, remove reg_invoke_rsp_fs_free_memd_pak(), using datagram_done()
 * instead to free buffer for both rsp and les(ISL).
 *
 * Revision 3.10.6.1  1996/03/29  03:18:36  hou
 * CSCdi50046:  vLAN 802.10 supports only IPX Arpa encaps.
 * Branch: California_branch
 * - should support default novell-ether IPX encap.
 *
 * Revision 3.10  1996/03/05  18:10:10  rbadri
 * CSCdi50469:  Transparent Bridging with ISL doesnt work
 * While writing the ISL header, restore datagramstart/mac_start to
 * the appropriate values to preserve the LLC header and data.
 *
 * Revision 3.9  1996/02/25  03:52:46  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.8  1996/02/05  04:04:11  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.7  1996/01/23  08:55:35  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#ifndef __VLAN_INLINE_H__
#define __VLAN_INLINE_H__

#include "../xns/novell.h"

/*
 * Platform specific exported functions and external declarations.
 */
extern  boolean    isl_fastswitch(paktype *);
extern  boolean    dot10_ether_vlan_fastswitch(paktype *);
extern  boolean    dot10_fddi_vlan_fastswitch(paktype *);
extern  boolean    dot10_tring_vlan_fastswitch(paktype *);

/*
 * isl_strip_header()
 *      This routine will strip off the ISL header in front of the packet
 * and render the frame back to what it was before the ISL header was
 * slapped on to the front of the frame.
 */
static inline void isl_strip_header (paktype *pak)
{
    pak->datagramstart += ISL_HEADER_LENGTH;
    pak->datagramsize  -= ISL_HEADER_LENGTH;
    if (pak_has_particles(pak))
        particle_clip_inline(pak->particlequeue.qhead, -ISL_HEADER_LENGTH);
 
    pak->network_start  = (pak->datagramstart + ETHER_8023_ENCAPBYTES);
    pak->encsize        = ETHER_8023_ENCAPBYTES;
}

/*
 * route_interior_isl_packet() - Route an ISL encapsulated packet.
 * 
 * RSP-specific Note:
 *   We have arrived here from rsp_eip_fs() which has passed a received ISL
 *   datagram to isl_fastswitch(). If we fail to Fast Switch the packet
 *   because there is no route cache entry then Q it directly for Process
 *   Level handling; rather than falling back into the main rsp_eip_fs()
 *   interrupt handler because that would not allow us to reset the start
 *   of the packet i.e.  datagramstart to point to the interior packet
 *   encapsulted after the ISL encapsulation. So basically we complete
 *   processing of the received ISL packet here and always return TRUE in
 *   the isl_fastswitch() calling routine.
 * 
 *   At this point the packet structure reference "pak" is the static
 *   structure used for the actual packet as it was taken off the raw MEMD
 *   Q. This packet structure can only be used to manipulate the packet at
 *   interrupt level, so if we need to send it for Process Level switching
 *   then it is copied by rsp_queue_ether_for_process().
 *
 * For all other platforms, the queue_xxx_for_process registry points to
 * the return_false function.
 */
static inline boolean route_interior_isl_packet (paktype *pak,
					       enum RXTYPE isl_interior_rxtype)
{
    if (reg_invoke_ether_fs(isl_interior_rxtype, pak)) {
        return (TRUE);
    }
    else {
        /*
         * If Routing between vLANs on the RSP platform then need to
         * explicitly enqueue for Process Switching. O.K. to take the
         * fast registry hit because we should Fast Switch everything
         * but the first packet thru ... 
	 */
        return (reg_invoke_rsp_queue_ether_for_process(pak->if_input->hwptr,
	  				               pak, (ether_hdr *)
					               pak->datagramstart));
    }
}

/*
 * bridge_interior_isl_packet() - Bridge an ISL encapsulated packet.
 */
static inline boolean bridge_interior_isl_packet (paktype *pak)
{
    /*
     * The entry point into RSP transparent bridging 
     * expects to see the first software IDB.
     */
    if (reg_invoke_tbridge_forward(pak)) {
        return (TRUE);
    }
    else {
	/*
         * Nota Bene:- Cannot just return FALSE here for non-bridgeable
         *             datagrams matching entries in the Software MAC Filter
         *             such as Spanning Tree BPDUs, CDP announcements etc.
         *             since they are not classified via the generic
         *             _parse_packet() routines.
         */

	/*
	 * If the input interface is a BVI(Bridge-group Virtual Interface),
	 * it means IRB is configured and we overwrote the input interface
	 * to the BVI when we make bridge/route decision in tbridge
	 * fast-switching code since this is a routable packet.
	 * Unfortunately, rsp_queue_ether_for_process() requires that
	 * you pass in the real, received hwidb to get buffer from and
	 * throttle it if necessary.
	 */
	hwidbtype *inputhw;
	if (is_bvi(pak->if_input->hwptr) && pak->irb_rcvd_input) {
	    if (reg_invoke_ether_fs(pak->rxtype, pak)) {
		return (TRUE);
	    }
	    inputhw = pak->irb_rcvd_input->hwptr;
	} else {
	    inputhw = pak->if_input->hwptr;
	}
	
        return (reg_invoke_rsp_queue_ether_for_process(inputhw,
                                                       pak, (ether_hdr *)
                                                       pak->datagramstart));
    }
}

/*
 * isl_cdp_packet()
 * Test for the CDP packet, let the caller decide what to do.
 * CDP packet is always SNAP encap.
 */

static inline boolean isl_cdp_packet (paktype *packet, boolean isl_header)
{
    if (isl_header) {
	if (GETSHORT(packet->datagramstart + ISL_HEADER_LENGTH + ISO2TYPE)
	    != TYPE_CDP)
	    return FALSE;
	isl_strip_header(packet);
    } else {
	if (GETSHORT(packet->datagramstart + ISO2TYPE) != TYPE_CDP)
	    return FALSE;
    }

    return TRUE;
}

/*
 * vlan_read_isl_colour ()
 *
 * Extract the two bytes of Virtual LAN Identifier from a received ISL packet.
 *
 * Note since the bottom bit of the two byte vLAN ID (colour) field is used to
 * indicate whether or not the encapsulated packet is a Spanning Tree BPDU or
 * CDP datagram, shift the (10 bit) field.
 */
static inline ushort vlan_read_isl_colour (paktype *received_packet)
{
    return (GETSHORT(received_packet->datagramstart + ISL_VLAN_ID_OFFSET) >> 1);
}

/****************************************************/
/***                                              ***/
/*** ... Now comes the IEEE 802.10 vLAN stuff ... ***/
/***                                              ***/
/****************************************************/


/*
 * vlan_read_ethernet_dot10_colour ()
 *
 * Extract the 4 byte SAID (Virtual LAN Identifier) from a received IEEE
 * 802.10 packet on Ethernet.
 */

static inline ulong vlan_read_ethernet_dot10_colour (paktype *received_packet)
{
    return (GETLONG(received_packet->datagramstart + ETHER_8023_ENCAPBYTES +
		    SAP_HDRBYTES));
}


/*
 * vlan_read_fddi_dot10_colour ()
 *
 * Extract the 4 byte SAID (Virtual LAN Identifier) from a received IEEE
 * 802.10 packet on FDDI.
 */

static inline ulong vlan_read_fddi_dot10_colour (paktype *received_packet)
{
    return (GETLONG(received_packet->datagramstart + FDDI_SAP_ENCAPBYTES));
}


/*
 * vlan_read_tring_dot10_colour ()
 *
 * Extract the 4 byte SAID (Virtual LAN Identifier) from a received IEEE
 * 802.10 packet on Token Ring.
 */

static inline ulong vlan_read_tring_dot10_colour (paktype *received_packet)
{
    return (GETLONG(received_packet->datagramstart + TRING_SAP_ENCAPBYTES));
}


/*
 * Identify the type of the 802.10 interior packet on Ethernet.
 *
 * This is called in fastswitching a received dot10 frame, and allows us to
 * then determine if we're configured to route on this subinterface, and what
 * fast switching protocol registry to call.
 */

static inline enum RXTYPE classify_ether_dot10_packet (paktype *input_packet)
{

    ushort ether_type;

    ether_type = (GETSHORT(input_packet->datagramstart + ETHER_8023_ENCAPBYTES
			   + SDE_ENCAPBYTES));

    /*
     * Check to see if the original frame was an 802.3 or SNAP encapsulation.
     */

    if (ether_type == SNAPSNAP) {

        ether_type = (GETSHORT(input_packet->datagramstart + SDE_ENCAPBYTES +
			       ETHER_8023_ENCAPBYTES + (2 * SAP_HDRBYTES)));

	/* The following is UGLY, but it is more efficient (and predictable)
	 * than a GCC switch statement parsing for sparse values since we can
         * order based upon perceived frequency of traffic ;-)
         */

        if (ether_type == TYPE_IP10MB)
	    return (RXTYPE_DODIP);
        else if (ether_type == TYPE_NOVELL1)
	    return (RXTYPE_NOVELL1);
        else if ((ether_type == TYPE_ETHERTALK) || (ether_type == TYPE_AARP))
	    return (RXTYPE_APPLE);
        else if (ether_type == TYPE_RFC826_ARP)
	    return (RXTYPE_RFC826_ARP);
        else if (ether_type == TYPE_DECNET)
	    return (RXTYPE_DECNET);
        else if (ether_type == TYPE_XNS)
	    return (RXTYPE_XNS);
        else if (ether_type == TYPE_APOLLO)
	    return (RXTYPE_APOLLO);
	else 
	    return (RXTYPE_UNKNOWN);
    }

    else {

	if (ether_type == TYPE_CLNS)
	    return (RXTYPE_CLNS);
        else if (ether_type == (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8))
            return (RXTYPE_NOVELL2);
	else if (ether_type == (SAP_NOVELL | SAP_NOVELL << 8))
	    return (RXTYPE_SAP_NOVELL);
	else
	    return (RXTYPE_UNKNOWN);
    }
}


/*
 * Identify the type of the 802.10 interior packet on FDDI.
 *
 * This is called in fastswitching a received dot10 frame, and allows us to
 * determine if we're configured to route on this subinterface, and what fast
 * switching protocol registry to call.
 */

static inline enum RXTYPE classify_fddi_dot10_packet (paktype *input_packet)
{

    ushort fddi_type;

    /*
     * Check to see if the frame has an 802.2 or SNAP encapsulation.
     * The latter is more prevalent on FDDI so check it first.
     */

    fddi_type = (GETSHORT(input_packet->datagramstart + FDDI_ENCAPBYTES +
			  SDE_ENCAPBYTES));

    if (fddi_type == SNAPSNAP) {

	/* The following is UGLY, but it is more efficient (and predictable)
	 * than a GCC switch statement parsing for sparse values since we can
         * order based upon perceived frequency of traffic ;-)
         */

	fddi_type = (GETSHORT(input_packet->datagramstart + FDDI_ENCAPBYTES
		     + SDE_ENCAPBYTES + (2 * SAP_HDRBYTES)));

        if (fddi_type == TYPE_IP10MB)
	    return (RXTYPE_DODIP);
        else if (fddi_type == TYPE_NOVELL1)
	    return (RXTYPE_NOVELL1);
        else if ((fddi_type == TYPE_ETHERTALK) || (fddi_type == TYPE_AARP))
	    return (RXTYPE_APPLE);
        else if (fddi_type == TYPE_RFC826_ARP)
	    return (RXTYPE_RFC826_ARP);
        else if (fddi_type == TYPE_DECNET)
	    return (RXTYPE_DECNET);
        else if (fddi_type == TYPE_XNS)
	    return (RXTYPE_XNS);
        else if (fddi_type == TYPE_APOLLO)
	    return (RXTYPE_APOLLO);
	else 
	    return (RXTYPE_UNKNOWN);
    }

    /* 
     * Now Classify either an 802.2 frame, based on LSAP value.
     */

    else {
        if (fddi_type == TYPE_CLNS)
	    return (RXTYPE_CLNS);
	else if (fddi_type == (SAP_NOVELL | SAP_NOVELL << 8))
	    return (RXTYPE_SAP_NOVELL);

	/* Legacy support for "Raw FDDI", sigh ...*/
 
        else if (fddi_type == (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8))
            return (RXTYPE_NOVELL2);
	else
	    return (RXTYPE_UNKNOWN);
    }
}


/*
 * Identify the type of the 802.10 interior packet on Token Ring.
 *
 * This is called in fastswitching a received dot10 frame, and allows us to
 * determine if we're configured to route on this subinterface, and what fast
 * switching protocol registry to call.
 *
 * Note dot10 is only currently defined for Transparently Bridged VLAN traffic
 * on Token Ring interfaces. This is a RIF-free zone!
 */

static inline enum RXTYPE classify_tring_dot10_packet (paktype *input_packet)
{

    ushort token_ring_type;

    /*
     * Check to see if the frame has an 802.2 or SNAP encapsulation.
     */

    token_ring_type = (GETSHORT(input_packet->datagramstart + TRING_ENCAPBYTES
				+ SDE_ENCAPBYTES));

    if (token_ring_type == SNAPSNAP) {

	/* The following is UGLY, but it is more efficient (and predictable)
	 * than a GCC switch statement parsing for sparse values since we can
         * order based upon perceived frequency of traffic ;-)
         */

	token_ring_type = (GETSHORT(input_packet->datagramstart + SDE_ENCAPBYTES
			           + TRING_ENCAPBYTES + (2 * SAP_HDRBYTES)));

        if (token_ring_type == TYPE_IP10MB)
	    return (RXTYPE_DODIP);
        else if (token_ring_type == TYPE_NOVELL1)
	    return (RXTYPE_NOVELL1);
        else if ((token_ring_type == TYPE_ETHERTALK) ||
		 (token_ring_type == TYPE_AARP))
	    return (RXTYPE_APPLE);
        else if (token_ring_type == TYPE_RFC826_ARP)
	    return (RXTYPE_RFC826_ARP);
        else if (token_ring_type == TYPE_DECNET)
	    return (RXTYPE_DECNET);
        else if (token_ring_type == TYPE_XNS)
	    return (RXTYPE_XNS);
        else if (token_ring_type == TYPE_APOLLO)
	    return (RXTYPE_APOLLO);
	else 
	    return (RXTYPE_UNKNOWN);
    }

    /* 
     * Now Classify either an 802.2 frame, based on LSAP value.
     */

    else {
        if (token_ring_type == TYPE_CLNS)
	    return (RXTYPE_CLNS);
	else if (token_ring_type == (SAP_NOVELL | SAP_NOVELL << 8))
	    return (RXTYPE_SAP_NOVELL);
	else
	    return (RXTYPE_UNKNOWN);
    }
}


/*
 * vlan_route_fddi_dot10_packet()
 *
 * Attempt to fast switch an IEEE 802.10 packet received on an FDDI interface.
 * Note we don't reconstruct the "regular" MAC header because theoretically
 * the routing fast switching routines should only be interested in "network
 * start" on ... I guess we'll find out when a particular protocol isn't
 * routed ;-)
 *
 * Here's what the FDDI 802.10 packet looks like in the packet buffer,
 * depending on whether it was originally 802.2 or SNAP encapsulated :-
 *
 * --------------------------------------------------------------
 * |  |    |      |      |    |          |Frag|                 |
 * |FC|Dest|Source|0A0A03|SAID|Station ID|    |DSAP SSAP Control|
 * |  |    |      |      |    |          |Flag|                 |
 * --------------------------------------------------------------
 *                <------- 16 Bytes ---------->
 * 
 * ---------------------------------------------------------------------------
 * |  |    |      |      |    |          |Frag|RFC 1042 SNAP Header |Original|
 * |FC|Dest|Source|0A0A03|SAID|Station ID|    |                     |        |
 * |  |    |      |      |    |          |Flag|AA AA 03 OUI 00 00 00|  Type  |
 * ---------------------------------------------------------------------------
 * <-- 13 Bytes --><-------- 16 Bytes --------><--------- 8 Bytes ----------->
 *
 * Where network_start needs to point to the end of each of these respective
 * packets. Don't mess with pak->datagramstart in case we fail to route this
 * packet and subsequently try to bridge it.
 */

static inline boolean vlan_route_fddi_dot10_packet (paktype     *input_packet,
					            enum RXTYPE dot10_rxtype)
{
    /*
     * Check for SNAP or 802.2 encapsulation, and reposition as appropriate.
     */

    if (GETSHORT(input_packet->datagramstart + FDDI_ENCAPBYTES +
		 SDE_ENCAPBYTES) == SNAPSNAP) {
	
        input_packet->network_start  = (input_packet->datagramstart +
                                        SDE_ENCAPBYTES + FDDI_SNAP_ENCAPBYTES);
        input_packet->encsize        = FDDI_SNAP_ENCAPBYTES + SDE_ENCAPBYTES;
    }

    /* Handle special case for Novell_Ether packet from ethernet */
    else if (GETSHORT(input_packet->datagramstart + FDDI_ENCAPBYTES +
		 SDE_ENCAPBYTES) == NOVELL_NO_CHECKSUM) {

        input_packet->network_start = (input_packet->datagramstart +
				        FDDI_ENCAPBYTES + SDE_ENCAPBYTES);
	input_packet->encsize = FDDI_ENCAPBYTES + SDE_ENCAPBYTES;
      }

    else {

        input_packet->network_start  = (input_packet->datagramstart +
				        FDDI_SAP_ENCAPBYTES + SDE_ENCAPBYTES);
	input_packet->encsize        = FDDI_SAP_ENCAPBYTES + SDE_ENCAPBYTES;
    }

    input_packet->enctype = ET_SDE;

    if (reg_invoke_fddi_fs(dot10_rxtype, input_packet) == TRUE)
	return(TRUE);

    /*
     * Couldn't fast switch the routable protocol (coz' no cache entry),
     * therefore we now need to Q for process level switching. Since this
     * code is shared by the RSP and the Cisco 4x00 platforms use a registry
     * here to accommodate their differences. Not a big overhead because
     * should only be required for the first routed packet. 
     * Thus in the C4x00 case, returning FALSE will cause the calling interrupt
     * routine to enqueue the packet within bsi_proc_idud().
     *
     */

    if (reg_invoke_rsp_queue_fddi_for_process(input_packet->if_input->hwptr,
   			                      input_packet))
        return(TRUE);

    else
	return(FALSE);
}


/*
 * route_tring_dot10_packet()
 *
 * Attempt to fast switch an IEEE 802.10 packet received on a Token Ring
 * interface.
 *
 * Here's what the Token Ring 802.10 packet looks like in the packet buffer,
 * depending on whether it was originally 802.2 or SNAP encapsulated :-
 *
 * -----------------------------------------------------------------
 * |  |  |    |      |      |    |          |    |                 |
 * |AC|FC|Dest|Source|0A0A03|SAID|Station ID|Flag|DSAP SSAP Control|
 * |  |  |    |      |      |    |          |    |                 |
 * -----------------------------------------------------------------
 *                   <------- 16 Bytes ---------->
 * 
 * --------------------------------------------------------------------------
 * |  |  |    |      |      |    |          |    |     SNAP Header     |    |
 * |AC|FC|Dest|Source|0A0A03|SAID|Station ID|Frag|                     |Type|
 * |  |  |    |      |      |    |          |    |AA AA 03 OUI 00 00 00|    |
 * --------------------------------------------------------------------------
 * <--- 14 Bytes ---><-------- 16 Bytes --------><-------- 8 Bytes ---------->
 *
 * Where network_start needs to point to the end of each of these respective
 * packets. Don't mess with pak->datagramstart in case we fail to route this
 * packet and subsequently try to bridge it.
 */

static inline boolean route_tring_dot10_packet (paktype *input_packet,
			 	 	        enum RXTYPE dot10_rxtype)
{
    /*
     * Check for SNAP or 802.2 encapsulation, and reposition as appropriate.
     */

    if (GETSHORT(input_packet->datagramstart + TRING_ENCAPBYTES +
		 SDE_ENCAPBYTES) == SNAPSNAP) {
	
        input_packet->network_start  = (input_packet->datagramstart +
                                       SDE_ENCAPBYTES + TRING_SNAP_ENCAPBYTES);
        input_packet->encsize        = TRING_SNAP_ENCAPBYTES;
    }

    else {

        input_packet->network_start  = (input_packet->datagramstart +
				        TRING_SAP_ENCAPBYTES + SDE_ENCAPBYTES);
	input_packet->encsize        = TRING_ENCAPBYTES;  /* O.K. I'm lying */
    }

    if (reg_invoke_token_fs(dot10_rxtype, input_packet) == TRUE)
	return(TRUE);

    /*
     * Couldn't fast switch the routeable protocol (coz' no cache entry),
     * therefore we now need to Q for process level switching. Since this
     * code is shared by the RSP and the Cisco 4x00 platforms use a registry
     * here to accommodate their differences. Not a big overhead because
     * should only be required for the first routed packet.
     * For non-RSP platforms returning FALSE allows the calling interrupt
     * routine to enqueue the newly formatted packet.
     */

    if (reg_invoke_rsp_queue_trip_for_process(input_packet->if_input->hwptr,
			                      input_packet, CTR_RXSTAT_TB))
        return(TRUE);

    else
       return(FALSE);
}


/*
 * route_ether_dot10_packet()
 *
 * Attempt to fast switch an IEEE 802.10 packet received on an Ethernet
 * interface of a packet-memory based platform. Note we don't reconstruct the
 * "regular" MAC header because theoretically the routing fast switching
 * routines should only be interested in "network start" on ... I guess we'll
 * find out when a particular protocol isn't routed ;-)
 *
 * Here's what the Ethernet 802.10 packet looks like in the packet buffer,
 * depending on whether it was originally 802.3 or Version2/SNAP encapsulated:-
 *
 * ------------------------------------------------------------------
 * |    |      |      |      |    |          |Frag|                 |
 * |Dest|Source|Length|0A0A03|SAID|Station ID|    |DSAP SSAP Control|
 * |    |      |      |      |    |          |Flag|                 |
 * ------------------------------------------------------------------
 *                    <------- 16 Bytes ---------->
 * 
 * ----------------------------------------------------------------------------
 * |    |      |   |      |    |          |Frag|RFC 1042 SNAP Header |Original|
 * |Dest|Source|len|0A0A03|SAID|Station ID|    |                     |        |
 * |    |      |   |      |    |          |Flag|AA AA 03 OUI 00 00 00|  Type  |
 * ----------------------------------------------------------------------------
 * <-- 14 Bytes --><-------- 16 Bytes --------><---------- 8 Bytes ----------->
 *
 * Where network_start needs to point to the end of each of these respective
 * packets. Don't mess with pak->datagramstart in case we fail to route this
 * packet and subsequently try to bridge it.
 */

static inline boolean route_ether_dot10_packet (paktype *input_packet,
				 	        enum RXTYPE dot10_rxtype)
{
    /*
     * Check for SNAP or 802.2 encapsulation, and reposition as appropriate.
     */

    if (GETSHORT(input_packet->datagramstart + ETHER_8023_ENCAPBYTES +
		 SDE_ENCAPBYTES) == SNAPSNAP) {
	
        input_packet->network_start  = (input_packet->datagramstart +
                                       ETHER_SNAP_ENCAPBYTES + SDE_ENCAPBYTES);
        input_packet->encsize        = ETHER_SNAP_ENCAPBYTES;
    }

    else {

        input_packet->network_start  = (input_packet->datagramstart +
				       ETHER_SAP_ENCAPBYTES + SDE_ENCAPBYTES);
	input_packet->encsize        = ETHER_8023_ENCAPBYTES;
    }

    if (reg_invoke_ether_fs(dot10_rxtype, input_packet) == TRUE)
	return(TRUE);

    /*
     * Couldn't fast switch the routeable protocol (coz' no cache entry),
     * therefore we now need to Q for process level switching. Since this
     * code is shared by the RSP and the Cisco 4x00 platforms use a registry
     * here to accommodate their differences. Not a big overhead because
     * should only be required for the first routed packet.
     * For non-RSP platforms returning FALSE allows the calling interrupt
     * routine to enqueue the newly formatted packet.
     */

    if (reg_invoke_rsp_queue_ether_for_process(input_packet->if_input->hwptr,
			                       input_packet, (ether_hdr *)
					       input_packet->datagramstart))
        return(TRUE);

    else
	
	/*
	 * Returning FALSE here causes the interior routeable packet to be
	 * sent for Process Level switching on Cisco 4x00 platforms.
	 */

        return(FALSE);
}


#endif /* __VLAN_INLINE_H__ */
