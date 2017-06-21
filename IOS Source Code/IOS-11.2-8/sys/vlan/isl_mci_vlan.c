/*--------------------------------------------------------------------------
 *
 * $Id: isl_mci_vlan.c,v 3.9.10.8 1996/08/27 23:32:25 hou Exp $
 *
 * vlan/isl_mci_vlan.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Functions specific to the protocol independent routing between Inter
 * Switch Link (ISL) Virtual LANs on the MCI/CxBus Platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: isl_mci_vlan.c,v $
 * Revision 3.9.10.8  1996/08/27  23:32:25  hou
 * CSCdi66323:  Routers ISL encapsultaion on CGMP messages is incorrect
 * Branch: California_branch
 * - Clean up the ISL vLAN code so that we can do ISL encapsulation on
 *   different type packet not only ARPA type.
 *
 * Revision 3.9.10.7  1996/08/15  00:44:56  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.9.10.6  1996/08/02  01:13:02  hou
 * CSCdi64841:  ISL subinterface route packets even if it is shutdown.
 * Branch: California_branch
 *
 * Revision 3.9.10.5  1996/07/17  18:36:31  tylin
 * CSCdi62787:  CDP does not work correctly with ISL encapsulation
 *    CDP packet should be always SNAP encapsulated.
 * Branch: California_branch
 *
 * Revision 3.9.10.4  1996/06/28  21:57:41  dawnli
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
 * Revision 3.9.10.3  1996/05/22  02:08:12  hou
 * CSCdi56274:  7507 send out ill formated ISL encap BPDU
 * Branch: California_branch
 * - write correct length field and sap for ISL BPDU packet.
 * - set default ICMP redirects as FALSE on isl vlan subinterface.
 *
 * Revision 3.9.10.2  1996/03/29  02:30:50  hou
 * CSCdi51527:  vLAN ISL does not fast switch ipx ping echo return packets
 * Branch: California_branch
 * and also check swidb instead of hwidb to determine input interface for
 * 7000 platform.
 * - isl_fastswitch():
 *   set input_hwidb->fast_rcvidb to input vlan interface after vlan
 *   demultiplex.
 * - internet_enqueue():
 *   pass packet to IP queue only when the input swidb is IP enabled.
 * - nov_set_nidb_idb():
 *   assign novell idb by checking swidb instead of hwidb
 *
 * Revision 3.9.10.1  1996/03/18  22:33:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  07:53:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/07  11:05:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  21:48:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/25  03:52:42  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.8  1996/02/07  04:05:10  mmcneali
 * CSCdi45328:  VLAN Translator/Routers FAIL to decrement IP TTL.
 *
 * Revision 3.7  1996/02/05  04:04:07  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.6  1996/01/31  02:15:28  snyder
 * CSCdi47864:  spellink errors
 *              reponse -> response
 *
 * Revision 3.5  1996/01/23  08:55:23  mmcneali
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

#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../srt/srt_registry.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_cmd.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"

#include "../if/network.h"

#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"

#include "../xns/novell.h"

#include "../srt/span.h"
#include "../srt/srt_debug_flags.h"

#include "../tbridge/tbridge_sde.h"

#include "../cdp/cdp_debug_flags.h"

#include "vlan.h"
#include "vlan_debug_flags.h"
#include "vlan_private.h"
#include "vlan_mci_inline.h"


/*
 * Exported functions, static storage allocation and external declarations.
 */


/*
 * isl_fastswitch ()
 *
 * This is the entry point for Inter Switch Link (ISL) datagrams received by
 * Cisco's IOS from a Cisco 5000 (aka. Synergy) Switch.
 *
 * It assumes that the Interior Packet's D/A lies at MCI_ETHER_OFFSET - this
 * facilitates shoe-horning the interior packet into the routing or bridging
 * fastswitching routines after processing the ISL header. Note every time we
 * perform a read we automatically increment the read pointer to the next byte
 * location.
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
 *        Proceed to classify the interior packet. If it is a packet type
 *        we're not configured to route then we may be configured for bridging.
 *
 *        At this stage drop the 26 bytes of ISL encapsulation; before passing
 *        it to the correct routing engine and we're done ;-)
 *
 * If we attempt to route/bridge the packet, then we return the result of the
 * protocol specific Fast Switching routine.
 */

boolean isl_fastswitch (hwidbtype *input_hwidb)
{

register ushort      isl_colour;
register ushort      fastswitch_result;
register enum RXTYPE isl_interior_rxtype;
register idbtype     *isl_vlan_interface;
register mciregtype  *regptr;

    /* Firstly extract the ISL colour field i.e. vLAN ID */

    isl_colour = vlan_read_isl_colour(input_hwidb);

    isl_vlan_interface = isl_colour_2_swidb(input_hwidb, isl_colour);

    /*
     * In the event of receiving an ISL packet with a vLAN Identifier
     * (i.e. colour) that is not supported/configured on a software IDB
     * belonging to this interface, then consider natively bridging it.
     */

    if ((isl_vlan_interface == NULL) ||
	(isl_vlan_interface->subif_state != SUBIF_STATE_UP)) {

	/*
	 * Always accept CDP packet sent on colour #1.
	 */
	if (isl_colour == ISL_COLOUR_1 &&
	    isl_cdp_mci_packet(input_hwidb))
	    return (FALSE);

        if (input_hwidb->firstsw->tbridge_on_swidb == TRUE) {

        /*
	 * Received ISL datagrams come up classified but never with interior
	 * packet type bits set (e.g. the Bridging Bit). Therefore if we
	 * receive an ISL packet bearing a colour that we're not configured
	 * for we need to check here in case we're bridging and should flood
	 * the fucker! This is possible because the Software MAC Filter used
	 * by the transparent bridging code to absorb interesting multicasts
	 * is not populated with the Cisco/ISL multicast on the first swidb;
	 * rather only on those subinterfaces configured for ISL encapsulation.
	 * 
	 * Nota Bene: If we're not bridging then we have to come this far just
	 *            to shed unwanted vLAN packets ...
	 *
         * Hopefully we won't be natively bridging ISL multicasts too often!
	 *
         * Nota Bene:- Once we invoke the bridging code, it builds a copy of
	 *             the packet's header (in "encap_pak") starting from
         *             "span_macoffset" which points to the start of the 
	 *             interior packet's D/A. This mechanism of adjusting
	 *             the read offset works because we assume that we can
	 *             always flood in the Fast Path.
	 */

            input_hwidb->span_macoffset -= ISL_INTERIOR_PACKET;
            fastswitch_result =
                reg_invoke_tbridge_mci_forward(input_hwidb->firstsw);
            input_hwidb->span_macoffset += ISL_INTERIOR_PACKET;
            return (fastswitch_result);

        } else if (vlan_packet_debug) {

	    /* This should be rate limited ... */

            buginf ("\nvLAN: ISL packet received bearing colour ID %d on"
                    " %s\n      which has no subinterface configured to"
		    " route or bridge ID %d.\n",
                      isl_colour, input_hwidb->hw_namestring, isl_colour);
        }

        regptr = input_hwidb->mci_regptr;
        regptr->cmdreg = MCI_CMD_RX_FLUSH;

	return (TRUE);
    } else {

        input_hwidb->fast_rcvidb = isl_vlan_interface; 

        /* Decrement the CBus/MCI length since used by Fast Switching */	
        input_hwidb->rxlength -= ISL_HEADER_LENGTH;
    }

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this subinterface.
     *
     * If not then consider transparently bridging the interior packet
     * or the exterior packet if so configured ;-(
     */

    isl_interior_rxtype = classify_isl_interior_packet(input_hwidb);

    if (routing_isl_vlan_on_subif(isl_vlan_interface,
			          isl_interior_rxtype) == TRUE) {

        /*
         * O.K. let's route the packet!
         *
	 * We can then just invoke the highend IP on Ethernet Fast Switching
	 * routine directly.
         */
        if (isl_interior_rxtype == RXTYPE_DODIP ||
            isl_interior_rxtype == RXTYPE_SNAP_DODIP) {

	    regptr = input_hwidb->mci_regptr;

            regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;
            regptr->cmdreg = MCI_CMD_RX_SELECT;

	    return (isl_ether_ip_fastswitch(input_hwidb));
        }

        return (reg_invoke_ether_fs(isl_interior_rxtype, input_hwidb));

    } else if (isl_vlan_interface->tbridge_on_swidb) {

        isl_vlan_interface->isl_vlan->
            ingressing_isl_packets[VLAN_TBRIDGE]++;

        return (reg_invoke_tbridge_mci_forward(isl_vlan_interface));

    } else if (vlan_packet_debug) {

	buginf ("\nvLAN: Received ISL encapsulated %s packet bearing colour "
		"ID %d\n      on interface %s which is not configured to"
		"\n      route or bridge this packet type.\n",
		 rxtype_string(isl_interior_rxtype), isl_colour,
		 isl_vlan_interface->namestring);
    }

    /*
     * Well we couldn't route it and the subinterface isn't configured for
     * bridging so let the calling interrupt routine handle this packet ...
     * it might be CDP/(R)ARP packet, will be Qed; or other non-routable 
     * and non-bridgable packet, will be junked.
     */
    return (FALSE);
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

/*
 * write_isl_vlan_colour_2_feip ()
 *
 * Provided that we are explicitly configured for vLAN SSE Switching
 * and via demand caching from the "protocol"cache_update() routines;
 * this function flags the FEIP card that it should now match on this
 * vLAN colour/subnet (it maintains a local on-card vLAN colour table)
 * and have the SSP attempt to silicon switch the packet!
 *
 * It also explicitly clears the vLAN ID from the FEIP maintained table
 * of colours/IDs that it will intercept and attempt to SSE switch.
 */

boolean write_isl_vlan_colour_2_feip (idbtype *vlan_subinterface,
				      boolean on_or_off)
{

ushort     mci_response;
leveltype  level;

    if (IS_SSE_SWITCHED_VLAN(vlan_subinterface)) {

        level = raise_interrupt_level(NETS_DISABLE);

	select_if(vlan_subinterface->hwptr);

	mci_response = send_feip_isl_vlan_colour(vlan_subinterface, on_or_off);

	if (mci_response != CMD_RSP_OKAY) {

            buginf("\nvLAN: Failed to write vLAN ID %d to the FEIP for"
		   " SSE switching.\n      CBus/MCI response was %x.\n",
		    vlan_subinterface->isl_vlan->colour, mci_response);

            reset_interrupt_level(level);
            return (FALSE);
        }
    
        reset_interrupt_level(level);
    }

    return (TRUE);
}


/*
 * set_vlan_sse_switched ()
 *
 * Flag SSE switching for a given vLAN.
 */

void set_vlan_sse_switched (idbtype *vlan_subinterface, boolean on_or_off)
{

    if (IS_ISL_VLAN_SUBINTERFACE(vlan_subinterface) == FALSE) {

	if (vlan_packet_debug) {

            printf("\nvLAN: Attempt made to configure SSE vLAN switching on\n"
		   "a subinterface not configured for vLAN encapsulation.");
	}
    }

    /*
     * If deconfiguring SSE vLAN switching, then return to
     * default setting and blow away the FEIP's table entry.
     */

    if (on_or_off == TRUE) {

	/* Demand caching will now populate the MEMA cache */

        vlan_subinterface->isl_vlan->vlan_switching_path = vLAN_SSE_SWITCHING;

    } else {

        /* Explicitly remove this vLAN colour from the FEIP maintained table */

        write_isl_vlan_colour_2_feip(vlan_subinterface, FALSE);

        vlan_subinterface->isl_vlan->vlan_switching_path = vLAN_FAST_SWITCHING;
    }
}
