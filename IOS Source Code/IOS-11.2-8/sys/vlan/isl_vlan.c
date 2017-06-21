/*--------------------------------------------------------------------------
 *
 * $Id: isl_vlan.c,v 3.8.10.10 1996/08/27 23:32:27 hou Exp $
 *
 * vlan/isl_vlan.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *
 * Functions specific to the protocol independent routing and translation
 * between Inter Switch Link (ISL) Virtual LANs.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: isl_vlan.c,v $
 * Revision 3.8.10.10  1996/08/27  23:32:27  hou
 * CSCdi66323:  Routers ISL encapsultaion on CGMP messages is incorrect
 * Branch: California_branch
 * - Clean up the ISL vLAN code so that we can do ISL encapsulation on
 *   different type packet not only ARPA type.
 *
 * Revision 3.8.10.9  1996/08/15  00:44:58  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.8.10.8  1996/08/02  01:13:03  hou
 * CSCdi64841:  ISL subinterface route packets even if it is shutdown.
 * Branch: California_branch
 *
 * Revision 3.8.10.7  1996/07/25  00:36:37  hou
 * CSCdi59925:  ISL bridging log counter does not have accounting info.
 * Branch: California_branch
 *
 * Revision 3.8.10.6  1996/07/17  18:36:33  tylin
 * CSCdi62787:  CDP does not work correctly with ISL encapsulation
 *    CDP packet should be always SNAP encapsulated.
 * Branch: California_branch
 *
 * Revision 3.8.10.5  1996/06/28  21:57:39  dawnli
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
 * Revision 3.8.10.4  1996/05/24  20:33:06  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/05/22  02:08:13  hou
 * CSCdi56274:  7507 send out ill formated ISL encap BPDU
 * Branch: California_branch
 * - write correct length field and sap for ISL BPDU packet.
 * - set default ICMP redirects as FALSE on isl vlan subinterface.
 *
 * Revision 3.8.10.2  1996/03/29  19:20:50  hou
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
 * Revision 3.8.10.1  1996/03/18  22:33:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.5  1996/03/16  07:53:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.4  1996/03/14  01:37:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.2.3  1996/03/07  11:06:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.2  1996/02/20  21:48:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/25  03:52:43  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.7  1996/02/05  04:04:08  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.6  1996/01/23  08:55:24  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ieee.h"
#include "packet.h"
#include "../ui/debug.h"
#include "config.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers_inline.h"

#include "sys_registry.h"
#include "fastswitch_registry.h"

#include "../src-rsp/rsp_if.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../if/ether_inline.h"

#include "../srt/span.h"
#include "../srt/srt_debug_flags.h"

#include "../tbridge/tbridge_sde.h"

#include "../ip/ip.h"
#include "../ip/ipfast.h"

#include "../xns/novell.h"

#include "../hes/if_cmd.h"
#include "../hes/if_trip.h"

#include "../cdp/cdp_debug_flags.h"

#include "vlan.h"
#include "vlan_debug_flags.h"
#include "vlan_private.h"
#include "vlan_inline.h"

/*
 * isl_fastswitch ()
 *
 * This is the entry point for Inter Switch Link (ISL) datagrams received by
 * Cisco's IOS running on an RSP platform from a Cisco vLAN Switch.
 *
 * This function is called from interrupt level, and handles the packet as
 * follows:-
 *
 *        The ISL Colour is extracted from bytes 20 and 21 of the exterior
 *        (i.e. ISL encapsulation) packet's header.
 *
 *        Note, if the bottom/least significant bit of this colour field is
 *        set then the interior packet is either a Spanning Tree PDU or a
 *        Cisco Discovery Protocol (CDP) packet.
 *
 *        Demultiplex on the colour to point to the correct Software IDB that
 *        is configured to support this Virtual LAN. If no subinterface
 *        configured for the particular vLAN ID then consider bridging the
 *        packet if so configured on the major (first) software IDB.
 *
 *	  Next, strip off the ISL header and classify the rxtype of the
 *	  interior packet. Then route the packet, if we are routing that
 *	  protocol, or bridge the packet if bridging is configured.
 *
 * If we attempt to route/bridge the packet, then we return the result of the
 * protocol specific Fast Switching routine.
 */

boolean isl_fastswitch (paktype *pak)
{

    ushort  isl_colour;
    idbtype *isl_vlan_interface;

    /*
     * Firstly extract the ISL colour field i.e. vLAN ID
     */
    isl_colour = vlan_read_isl_colour(pak);
    isl_vlan_interface = isl_colour_2_swidb(pak->if_input->hwptr, isl_colour);

    /*
     * In the event of receiving an ISL packet with a vLAN Identifier
     * (i.e. colour) that is not supported/configured on a software IDB
     * belonging to this interface, then consider natively bridging it.
     */
    if ((isl_vlan_interface == NULL) ||
	(isl_vlan_interface->subif_state != SUBIF_STATE_UP)) {

	/*
	 * Always accept CDP packet sent on colour #1, rather
         * than bridging these packets.
	 */
	if (isl_colour == ISL_COLOUR_1 && isl_cdp_packet(pak, TRUE)) {
	    /* 
             * This is a cdp packet. Queue the packet to the process level.
             * Since the low end do not provide the queue_ether_for_process
             * routine, just return FALSE and let the driver handle it.
             */

	    return(reg_invoke_rsp_queue_ether_for_process(
                                                   pak->if_input->hwptr,
                                                   pak,
                                                   (ether_hdr *)
                                                   pak->datagramstart));
        }

        if (pak->if_input->tbridge_on_swidb == TRUE) {

        /*
	 * Here we have received a foreign vLAN colour (i.e. bearing a vLAN ID
	 * that we're not configured for) and if we're bridging on the
	 * main/first software IDB then we'll flood the fucker! This enables
	 * the router to act as a transit device and pass thru other ISL tagged
	 * traffic between switches that we're not explicitly configured to
	 * intercept and route or bridge ... This is possible because the 
	 * Software MAC Filter used by the transparent bridging code to absorb
	 * interesting multicasts is not populated with the Cisco/ISL multicast
	 * on the first swidb, rather only on those subinterfaces configured
	 * for ISL encapsulation.
	 * 
	 * Nota Bene:- If we're not bridging then we have to come this far just
	 *             to shed unwanted vLAN packets ...
	 *
         * Hopefully we won't be natively bridging ISL multicasts too often :-)
         */

	    return (reg_invoke_tbridge_forward(pak));

        } else if (vlan_packet_debug) {
            /*
	     * This should be rate limited ... 
             */
	    buginf ("\nvLAN: ISL packet received bearing colour ID %d on"
		    " %s\n      which has no subinterface configured to"
		    " route or bridge ID %d.\n", isl_colour,
		      pak->if_input->namestring, isl_colour);
	}

        /*
	 * We don't want to queue vLAN packets we don't know
	 * what to do with so now just flush the MEMD buffer.
	 */
	datagram_done(pak);
        return (TRUE);

    } else {

        pak->if_input = isl_vlan_interface;
    }

    /*
     * Now, strip off the ISL header and classify the inner packet.
     */
    isl_strip_header(pak);
    pak->rxtype = ether_decode_encapsulation_inline(pak,
                                          (ether_hdr *)pak->datagramstart);

    if (routing_isl_vlan_on_subif(isl_vlan_interface, pak->rxtype) == TRUE) {
      /*
       * Let's route the packet only if the packet's interior
       * destination MAC address is the physical MAC address in the
       * receiving interface or it is a non-unicast address.
       * Normally this check is done by the Address Filter however it
       * has already matched the incoming datagram against the Cisco
       * ISL Multicast and as such we now need to explicitly match the
       * destination address of the ISL encapsulated MAC frame.
       * A better solution is to turn on HSRP so that for a unicast
       * packet it is matched against the "Virtual MAC" associated
       * with the appropriate subinterface. Once HSRP over ISL is
       * supported this might be reworked.
       */
        if (ieee_equal(pak->datagramstart,
                       isl_vlan_interface->hwptr->hardware) ||
            ((pak->datagramstart[ETALKETHERDST]) & ETHER_MAC_MULTICAST))
            return (route_interior_isl_packet(pak, pak->rxtype));
        else {
          if (vlan_packet_debug) {
              buginf ("\nvLAN: Received ISL encapsulated %s packet for"
                      " routing bearing colour ID %d\n      on interface"
                      " %s but interior unicast packet not addressed\n "
                      "     to this interface so dropped.\n",
                      rxtype_string(pak->rxtype), isl_colour,
                      isl_vlan_interface->namestring);
          }
        }
    } else if (isl_vlan_interface->tbridge_on_swidb == TRUE) {
	/*
	 * If configured for bridging, then set up the packet attributes
	 * to reflect the interior packet and invoke bridging.
	 */

        isl_vlan_interface->isl_vlan->
          ingressing_isl_packets[VLAN_TBRIDGE]++;

        return (bridge_interior_isl_packet(pak));

    } else if (isl_cdp_packet(pak, FALSE) == TRUE) { 
        /* 
         * This is a cdp packet. Queue the packet to the process level.
         * Since the low end do not provide the queue_ether_for_process
         * routine, just return FALSE and let the driver handle it.
         */
        return(reg_invoke_rsp_queue_ether_for_process(pak->if_input->hwptr,
                                                   pak,
                                                   (ether_hdr *)
                                                   pak->datagramstart));

    } else if ((pak->rxtype == RXTYPE_RFC826_ARP) ||
	       (pak->rxtype == RXTYPE_REVERSE_ARP)) {
	/* 
	 * we don'r route (R)ARP packet, and the packet might be for me, might
	 * be for bridging; the bridgable (R)ARP has gone to bridging code, and
	 * the rest come here, let (R)ARP stack decides either to drop the
	 * packet or to queue it into (R)ARP Q.
	 */
	return (reg_invoke_rsp_queue_ether_for_process(pak->if_input->hwptr,
						       pak,
						       (ether_hdr *)
							pak->datagramstart)); 

    } else if (vlan_packet_debug) {

        buginf ("\nvLAN: Received ISL encapsulated %s packet bearing colour "
                "ID %d\n      on interface %s which is not configured to\n"
                "      route or bridge this packet type.\n",
		 rxtype_string(pak->rxtype), isl_colour,
		 isl_vlan_interface->namestring);
    }

    /*
     * We don't want to queue vLAN packets we don't know what to do with since
     * it's onerous to shed non-vLAN switched traffic via copying to DRAM for
     * Process Level discard, so now just flush the MEMD buffer.
     */
    datagram_done(pak);
    return (TRUE);
}


/*
 * Write the appropriate 26 bytes of ISL encapsulation, followed by the
 * interior packet's MAC header.
 *
 * Called by MEDIA_ETHER_MACWRITE, index is ET_ISL.
 *
 * As most of this encapsulation remains constant, quicker to enter the
 * source address, length and the colour into the header array, and then
 * bcopy the whole thing!
 *
 * Note write the source address of the egressing interface in case the
 * packet is processed by an intermediate switching device. Also bytes 20
 * and 21 assume the colour assigned to the "output_swidb".
 *
 * Nota Bene:  This is the packet memory based version as opposed to the
 *             CxBus equivalent which overloads the length field to flag
 *             the inner CRC ucode requirement.
 */
boolean write_isl_encapsulation (paktype *pak, hwidbtype *hwidb, int linktype,
				 char *interior_daddr, char const *other_info)
{
    idbtype *output_swidb;

    output_swidb = pak->if_output;

    /*
     * If datagramstart not setup, we have a bad caller, bail.
     */
    if (pak->datagramstart == NULL) {

        if (vlan_packet_debug) {

            buginf ("\n\nvLAN: Bad packet from %s send for ISL "
		    "encapsulation.\n", output_swidb->namestring);
        }

        return(FALSE);
    }

    /*
     * Use the MAC address associated with the egressing interface.
     *
     * Length field is computed by the FEIP ucode upon transmission.
     */
    ieee_copy (hwidb->hardware, &isl_header[6]);

    /*
     * Shift the colour field one bit position since ISL defines the LSB
     * to indicate whether or not the packet is a Spanning Tree BPDU or
     * Cisco Discovery packet!
     */
    PUTSHORT(&isl_header[20], (output_swidb->isl_vlan->colour << 1));

    if ((pak->linktype == LINK_IEEE_SPANNING) || (pak->linktype == LINK_CDP)) {
        isl_header[21] |= BPDU_OR_CDP_MASK;
    }

    /*
     * Mark the packet as requiring ISL Inner CRC generation; done by the FEIP
     * ucode until the MICE hardware assist is available at which time the
     * "PAK_DO_ISL_CRC" becomes redundant. This is for routed packets egressing
     * an ISL encapsulating subinterface, not for transit bridged datagrams
     * since the inner CRC is preserved in that case.
     */
    pak->flags |= PAK_DO_ISL_CRC;

    pak->datagramstart -= ISL_HEADER_LENGTH;
    pak->datagramsize  += ISL_HEADER_LENGTH;
  
    /*
     * Now prepend the whole 40 bytes, 26 for ISL then the interior MAC header.
     */
    bcopy (isl_header, pak->datagramstart, ISL_HEADER_LENGTH);
  
    vlan_packet_counters(pak, ET_ISL);
    pak->encsize += ISL_HEADER_LENGTH;
    return (TRUE);
}
