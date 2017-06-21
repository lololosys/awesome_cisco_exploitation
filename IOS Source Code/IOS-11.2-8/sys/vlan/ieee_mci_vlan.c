/*-------------------------------------------------------------------------
 *
 * $Id: ieee_mci_vlan.c,v 3.5.20.1 1996/08/02 01:12:59 hou Exp $
 *
 * vlan/ieee_mci_vlan.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Functions specific to the routing/switching of protocols between IEEE
 * 802.10 vLANs on the MCI/CxBus Platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: ieee_mci_vlan.c,v $
 * Revision 3.5.20.1  1996/08/02  01:12:59  hou
 * CSCdi64841:  ISL subinterface route packets even if it is shutdown.
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/05  04:04:05  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.4  1996/01/25  11:22:56  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1996/01/23  08:55:20  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "ieee.h"
#include "packet.h"
#include "../ui/debug.h"
#include "config.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"

#include "../hes/sse_registry.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"

#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../srt/span.h"
#include "../srt/srt_debug_flags.h"

#include "../tbridge/tbridge_sde.h"

#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"

#include "../cdp/cdp_debug.h"

#include "vlan_debug.h"
#include "vlan.h"
#include "vlan_private.h"
#include "vlan_mci_inline.h"


/*
 * Forward references and external declarations.
 */


/*
 * dot10_ether_vlan_fastswitch ()
 *
 * This is the entry point for IEEE 802.10 VLAN datagrams received on an
 * Ethernet interface by the Cisco IOS on MCI/CBus platforms.
 *
 * It assumes that the high-end microcode has laid the packet down such that
 * the hardcoded read pointer `MCI_ETHER_OFFSET' is  14 bytes (i.e standard
 * Ethernet encapsulation size) back from the Network Layer header so as to
 * expedite the fast switch routing and minimally impact these generic
 * routines.
 * Nota Bene: Every time we perform a read we automatically increment the
 *            read pointer. Therefore have to factor that in subsequent
 *            read offsets.
 *
 * This function is called from interrupt level, and handles the packet as
 * follows:-
 *
 *        The dot10 SAID (i.e. vLAN colour) is extracted from bytes 18 and 19
 *        of the dot10 encapsulated packet.
 *
 *        Demultiplex on the colour to point to the correct Software IDB that
 *        is configured to support this Virtual LAN.
 *
 *        Proceed to classify the encapsulated packet. If it is a packet type
 *        we're not configured to route then see if configured for bridging.
 *
 * If we attempt to route/bridge the packet, then we return the result of the
 * protocol specific Fast Switching routine.
 */

boolean dot10_ether_vlan_fastswitch (hwidbtype *input_hwidb)
{

register ushort      dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;
register mciregtype  *regptr;

    /*
     * Firstly scrawl back the read pointer and take a look
     * at the 802.10 SAID colour field i.e. vLAN ID.
     */

    dot10_colour = vlan_mciread_dot10_colour(input_hwidb);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour, input_hwidb);

    /*
     * In the event of receiving a 802.10 packet with a vLAN Identifier
     * (i.e. Colour) that is not supported/configured on a software IDB
     * belonging to this interface, then just junk it.
     *
     * This should be rate limited ...
     */

    if ((dot10_vlan_interface == NULL) &&
	(dot10_vlan_interface->hwptr->tbridge_on_hwidb == FALSE)) {

        if (vlan_packet_debug) {

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		     input_hwidb->hw_namestring, dot10_colour);
        }

        return (TRUE);
    }

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this sub-interface.
     *
     * If not then consider transparently bridging it ;-(
     */

    dot10_interior_rxtype = classify_dot10_interior_packet(input_hwidb);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE) {
        /*
         * O.K. let's route the packet!!
         *
         * Note if it's IP then we have to adjust to ether_ip_fastswitch()'s
         * assumption about the read pointer pointing to the IP Destination
         * Address - remember the offset is in words.
         */

        if (dot10_interior_rxtype == RXTYPE_DODIP ||
            dot10_interior_rxtype == RXTYPE_SNAP_DODIP) {

            regptr = input_hwidb->mci_regptr;

            regptr->argreg = (input_hwidb->rxoffset + MCI_IP_DSTADR_OFFSET);
            regptr->cmdreg = MCI_CMD_RX_SELECT;
        }

        return (reg_invoke_ether_fs(dot10_interior_rxtype,
                                    dot10_vlan_interface->hwptr));
    }

    else if (dot10_vlan_interface->hwptr->tbridge_on_hwidb) {

        /*
         * Note once we invoke the bridging code, it builds a copy of the
         * packet's header (in "encap_pak") starting from "span_macoffset".
	 * (Effectively the tbridging code will demux again on the colour
	 * in making a forwarding decision, sigh).
	 *
	 * So determine the originating encapsulation, and set up read pointer.
         */

        regptr = input_hwidb->mci_regptr;

        regptr->argreg = (input_hwidb->rxoffset + SNAP_OUIBYTES);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

        if (regptr->readshort == SNAPSNAP)
            input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					   SDE_SAID_BYTES + SDE_PT_FLAG_BYTES);
	else
	    input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					    SDE_SAID_BYTES);

        reg_invoke_tbridge_mci_forward(dot10_vlan_interface);

	/* Return to normality for subsequent packets */

	input_hwidb->span_macoffset = MCI_ETHER_OFFSET;
	return(TRUE);
    }

    else if (vlan_packet_debug) {

        buginf ("\n\nvLAN: ISL packet received on %s with nonrouteable"
                " interior packet\n", dot10_vlan_interface->namestring);
    }

    /*
     * Well we couldn't route it and the subinterface isn't configured for
     * bridging so let the calling interrupt routine junk this packet.
     */

    return(TRUE);
}


/*
 * dot10_fddi_vlan_fastswitch ()
 *
 * This is the entry point for IEEE 802.10 VLAN datagrams received on an
 * FDDI interface by the Cisco IOS on MCI/CBus platforms.
 *
 * It assumes that the high-end microcode has laid the packet down such that
 * the network layer header begins at MCI_OFFSETSIZE (i.e. "The Line"); so as
 * to expedite the fast switch routing and minimally impact these generic
 * routines.
 * Hence if the packet ingresses in 802.2 format then the hardcoded read
 * pointer `FDDI_RECEIVE_OFFSET' is 16 bytes (i.e MCI FDDI encapsulation size)
 * back and actually pointing to the start of the SAID field! If as is much
 * more common, the packet has come in in SNAP format then we need to back up
 * the read pointer to extract the dot10 vLAN colour/SAID field.
 */

boolean dot10_fddi_vlan_fastswitch (hwidbtype *input_hwidb)
{

register ushort      dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;
register mciregtype  *regptr;

    /*
     * Firstly scrawl back the read pointer and take a look
     * at the 802.10 SAID colour field i.e. vLAN ID.
     */

    dot10_colour = vlan_mciread_dot10_colour(input_hwidb);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour, input_hwidb);

    /*
     * In the event of receiving a 802.10 packet with a vLAN Identifier
     * (i.e. Colour) that is not supported/configured on a software IDB
     * belonging to this interface, then see if configured to natively
     * bridge it.
     */

    if ((dot10_vlan_interface == NULL) || 
	(dot10_vlan_interface->subif_state != SUBIF_STATE_UP)) {

	if (dot10_vlan_interface->hwptr->firstsw->tbridge_on_swidb == FALSE) {

            return (reg_invoke_tbridge_mci_forward(input_hwidb->firstsw));

        } else if (vlan_packet_debug) {

	    /* This should be rate limited ... */

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		     input_hwidb->hw_namestring, dot10_colour);
        }

        return (TRUE);
    }

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this sub-interface.
     *
     * If not then consider transparently bridging it ;-(
     */

    dot10_interior_rxtype = classify_dot10_interior_packet(input_hwidb);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE) {
        /*
         * O.K. let's route the packet!!
         *
         * Note if it's IP then we have to adjust to ether_ip_fastswitch()'s
         * assumption about the read pointer pointing to the IP Destination
         * Address - remember the offset is in words.
         */

        if (dot10_interior_rxtype == RXTYPE_DODIP ||
            dot10_interior_rxtype == RXTYPE_SNAP_DODIP) {

            regptr = input_hwidb->mci_regptr;

            regptr->argreg = (input_hwidb->rxoffset + MCI_IP_DSTADR_OFFSET);
            regptr->cmdreg = MCI_CMD_RX_SELECT;
        }

        return (reg_invoke_fddi_fs(dot10_interior_rxtype,
                                   dot10_vlan_interface->hwptr));
    }

    else if (dot10_vlan_interface->hwptr->tbridge_on_hwidb) {

        /*
         * Note once we invoke the bridging code, it builds a copy of the
         * packet's header (in "encap_pak") starting from "span_macoffset".
	 * (Effectively the tbridging code will demux again on the colour
	 * in making a forwarding decision, sigh).
	 *
	 * So determine the originating encapsulation, and set up read pointer.
         */

        regptr = input_hwidb->mci_regptr;

        regptr->argreg = (input_hwidb->rxoffset + SNAP_OUIBYTES);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

        if (regptr->readshort == SNAPSNAP)
            input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					   SDE_SAID_BYTES + SDE_PT_FLAG_BYTES);
	else
	    input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					    SDE_SAID_BYTES);

        reg_invoke_tbridge_mci_forward(dot10_vlan_interface);

	/* Return to normality for subsequent packets */

	input_hwidb->span_macoffset = MCI_ETHER_OFFSET;
	return(TRUE);
    }

    else if (vlan_packet_debug) {

        buginf ("\n\nvLAN: IEEE 802.10 packet received on %s with nonroutable"
                " interior packet\n\n", dot10_vlan_interface->namestring);
    }

    /*
     * Well we couldn't route it and the subinterface isn't configured for
     * bridging so let the calling interrupt routine junk this packet.
     */

    return(TRUE);
}


/*
 * dot10_tring_vlan_fastswitch ()
 *
 * This is the entry point for IEEE 802.10 VLAN datagrams received on a
 * transparent Token Ring interface by the Cisco IOS on MCI/CBus platforms.
 *
 * It assumes that the high-end microcode has laid the packet down such that
 * the network layer header begins at MCI_OFFSETSIZE (i.e. "The Line"); so as
 * to expedite the fast switch routing and minimally impact these generic
 * routines.
 */

boolean dot10_tring_vlan_fastswitch (hwidbtype *input_hwidb)
{

register ushort      dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;
register mciregtype  *regptr;

    /*
     * Firstly scrawl back the read pointer and take a look
     * at the 802.10 SAID colour field i.e. vLAN ID.
     */

    dot10_colour = vlan_mciread_dot10_colour(input_hwidb);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour, input_hwidb);

    /*
     * In the event of receiving a 802.10 packet with a VLAN Identifier
     * (i.e. Colour) that is not supported/configured on a software IDB
     * belonging to this interface, then just junk it.
     *
     * This should be rate limited ...
     */

    if ((dot10_vlan_interface == NULL) &&
	(!dot10_vlan_interface->hwptr->tbridge_on_hwidb)) {

        if (vlan_packet_debug) {

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		     input_hwidb->hw_namestring, dot10_colour);
        }

        return (TRUE);
    }

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this sub-interface.
     *
     * If not then consider transparently bridging it ;-(
     */

    dot10_interior_rxtype = classify_dot10_interior_packet(input_hwidb);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE) {
        /*
         * O.K. let's route the packet!!
         *
         * Note if it's IP then we have to adjust to ether_ip_fastswitch()'s
         * assumption about the read pointer pointing to the IP Destination
         * Address - remember the offset is in words.
         */

        if (dot10_interior_rxtype == RXTYPE_DODIP ||
            dot10_interior_rxtype == RXTYPE_SNAP_DODIP) {

            regptr = input_hwidb->mci_regptr;

            regptr->argreg = (input_hwidb->rxoffset + MCI_IP_DSTADR_OFFSET);
            regptr->cmdreg = MCI_CMD_RX_SELECT;
        }

        return (reg_invoke_ether_fs(dot10_interior_rxtype,
                                    dot10_vlan_interface->hwptr));
    }

    else if (dot10_vlan_interface->hwptr->tbridge_on_hwidb) {

        /*
         * Note once we invoke the bridging code, it builds a copy of the
         * packet's header (in "encap_pak") starting from "span_macoffset".
	 * (Effectively the tbridging code will demux again on the colour
	 * in making a forwarding decision, sigh).
	 *
	 * So determine the originating encapsulation, and set up read pointer.
         */

        regptr = input_hwidb->mci_regptr;

        regptr->argreg = (input_hwidb->rxoffset + SNAP_OUIBYTES);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

        if (regptr->readshort == SNAPSNAP)
            input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					   SDE_SAID_BYTES + SDE_PT_FLAG_BYTES);
	else
	    input_hwidb->span_macoffset -= (ETHER_8023_ENCAPWORDS +
					    SDE_SAID_BYTES);

        reg_invoke_tbridge_mci_forward(dot10_vlan_interface);

	/* Return to normality for subsequent packets */

	input_hwidb->span_macoffset = MCI_ETHER_OFFSET;
	return(TRUE);
    }

    else if (vlan_packet_debug) {

        buginf ("\n\nvLAN: ISL packet received on %s with nonrouteable"
                " interior packet\n\n", dot10_vlan_interface->namestring);
    }

    /*
     * Well we couldn't route it and the subinterface isn't configured for
     * bridging so let the calling interrupt routine junk this packet.
     */

    return(TRUE);
}


void vlan_register_fastswitching (void)
{
    /*
     * IEEE 802.10 frames for routing/switching can appear on all LAN media and
     * all platforms whereas ISL packets only supported on the Fast Ethernet
     * interface on Cisco 7xxx High End platforms.
     */

    reg_add_ether_fs(RXTYPE_IEEE_802_10,
		     (service_ether_fs_type) dot10_ether_vlan_fastswitch,
                     "dot10_ether_vlan_fastswitch");

    reg_add_fddi_fs(RXTYPE_IEEE_802_10,
                    (service_fddi_fs_type) dot10_fddi_vlan_fastswitch,
                    "dot10_fddi_vlan_fastswitch");

    reg_add_token_fs(RXTYPE_IEEE_802_10,
                    (service_token_fs_type) dot10_tring_vlan_fastswitch,
                    "dot10_tring_vlan_fastswitch");

    reg_add_ether_fs(RXTYPE_ISL, (service_ether_fs_type) isl_fastswitch,
                     "isl_fastswitch");

    reg_add_write_isl_vlan_colour_2_feip(write_isl_vlan_colour_2_feip,
                                         "write_isl_vlan_colour_2_feip");

    if (registry_populated(REG_SSE))
	reg_add_set_vlan_sse_switched(set_vlan_sse_switched,
				      "set_vlan_sse_switched");
}
