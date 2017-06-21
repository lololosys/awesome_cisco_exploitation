/*--------------------------------------------------------------------------
 *
 * $Id: ieee_vlan.c,v 3.6.6.1 1996/08/02 01:13:00 hou Exp $
 *
 * vlan/ieee_vlan.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Functions specific to the routing of protocols between IEEE 802.10 vLANS
 * on the Routing and Switching Processor (RSP) and Low End platforms.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: ieee_vlan.c,v $
 * Revision 3.6.6.1  1996/08/02  01:13:00  hou
 * CSCdi64841:  ISL subinterface route packets even if it is shutdown.
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/05  00:10:10  hou
 * CSCdi49781:  VLAN 802.10 translation on IP is broken
 * - dot10_fddi_vlan_fastswitch()
 *   drop the packet if it's a vlan packet with wrong sde
 *   and we cann't bridge it
 *
 * Revision 3.5  1996/02/05  04:04:06  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.4  1996/01/23  08:55:21  mmcneali
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
#include "logger.h"
 
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers_inline.h"

#include "sys_registry.h"
#include "fastswitch_registry.h"

#include "../hes/if_mci.h"

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

#include "../src-rsp/rsp_if.h"

#include "../hes/if_cmd.h"
#include "../hes/if_trip.h"

#include "vlan_debug.h"
#include "vlan.h"
#include "vlan_private.h"
#include "vlan_inline.h"


/*
 * dot10_ether_vlan_fastswitch ()
 *
 * This is the entry point for IEEE 802.10 VLAN datagrams received by the Cisco
 * IOS on the Low End (i.e. Cisco 4x00) or RSP (i.e. Cisco 75xx) platform.
 *
 * This function is called from interrupt level, and handles the packet as
 * follows:-
 *
 * If we attempt to route/bridge the packet, then we return the result of the
 * protocol specific Fast Switching routine.
 */

boolean dot10_ether_vlan_fastswitch (paktype *packet_2_fastswitch)
{

register ushort      dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;

    /* Firstly take a look at the 802.10 SAID colour field i.e. VLAN ID */

    dot10_colour = vlan_read_ethernet_dot10_colour(packet_2_fastswitch);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour,
			   packet_2_fastswitch->if_input->hwptr);
    /*
     * In the event of receiving a 802.10 packet with a vLAN Identifier
     * (i.e. Colour) that is not supported/configured on a software IDB
     * belonging to this interface, then just junk it.
     *
     * This should be rate limited ...
     */

    if (dot10_vlan_interface == NULL) {

        if (vlan_packet_debug) {

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		     packet_2_fastswitch->if_input->namestring, dot10_colour);
        }

        return (FALSE);

    } else

        packet_2_fastswitch->if_input = dot10_vlan_interface;

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this subinterface.
     */

    dot10_interior_rxtype = classify_ether_dot10_packet(packet_2_fastswitch);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE)

        /* Let's route the packet!! */

        return(route_ether_dot10_packet(packet_2_fastswitch,
					dot10_interior_rxtype));
    /*
     * Otherwise return to the calling fast switch interrupt and let it
     * determine if we're bridging the packet, either subinterface dot10
     * bridging or regular bridging on the major software IDB.
     */

    return(FALSE);
}


/*
 * dot10_fddi_vlan_fastswitch()
 *
 * This is the entry point for IEEE 802.10 vLAN datagrams received by the
 * Cisco IOS on an FDDI interface on the Low End (i.e. Cisco 4x00) or RSP
 * (i.e. Cisco 75xx) platforms.
 *
 * This function is called from interrupt level, and handles the packet as
 * follows:-
 *
 * Extract the dot10 SAID (i.e. vLAN Colour) and demultiplex on it to obtain
 * the corresponding software IDB.
 *
 * Classify the interior packet, and if configured to route on that interface
 * invoke the appropriate fast switching routine, otherwise return to the
 * fast switching interrupt and maybe try to bridge it.
 */

boolean dot10_fddi_vlan_fastswitch (paktype *packet_2_fastswitch)
{

register ulong       dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;

    /* Firstly take a look at the 802.10 SAID colour field i.e. vLAN ID */

    dot10_colour = vlan_read_fddi_dot10_colour(packet_2_fastswitch);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour,
			   packet_2_fastswitch->if_input->hwptr);
    /*
     * In the event of receiving a 802.10 packet with a vLAN Identifier
     * (i.e. colour) that is not supported/configured on a software IDB
     * belonging to this interface, then see if configured to natively
     * bridge it.
     */

    if ((dot10_vlan_interface == NULL) ||
	(dot10_vlan_interface->subif_state != SUBIF_STATE_UP)) {

        if (packet_2_fastswitch->if_input->tbridge_on_swidb == TRUE) {

            return (reg_invoke_tbridge_forward(packet_2_fastswitch));

        } else if (vlan_packet_debug) {

	    /* This should be rate limited ... */

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		      packet_2_fastswitch->if_input->namestring, dot10_colour);
        }

        datagram_done(packet_2_fastswitch);
        return (TRUE);

    } 

    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this subinterface.
     */

    dot10_interior_rxtype = classify_fddi_dot10_packet(packet_2_fastswitch);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE) {

        packet_2_fastswitch->if_input = dot10_vlan_interface;

        /* Let's route the packet!! */

        return(vlan_route_fddi_dot10_packet(packet_2_fastswitch,
				            dot10_interior_rxtype));
    }
    /*
     * Otherwise return to the calling fast switch interrupt and let it
     * determine if we're bridging the packet.
     */

    return(FALSE);
}


/*
 * dot10_tring_vlan_fastswitch()
 *
 * This is the entry point for IEEE 802.10 VLAN datagrams received by the
 * Cisco IOS on a Token Ring interface on the Low End (i.e. Cisco 4x00) or
 * RSP (i.e. Cisco 750x) platforms.
 *
 * This function is called from interrupt level, and handles the packet as
 * follows:-
 *
 * Extract the dot10 SAID (i.e. VLAN Colour) and demultiplex on it to obtain
 * the corresponding software IDB.
 *
 * Classify the interior packet, and if configured to route on that interface
 * invoke the appropriate fast switching routine, otherwise return to the
 * fast switching interrupt and maybe try to bridge it.
 */

boolean dot10_tring_vlan_fastswitch (paktype *packet_2_fastswitch)
{

register ulong       dot10_colour;
register enum RXTYPE dot10_interior_rxtype;
register idbtype     *dot10_vlan_interface;

    /* Firstly take a look at the 802.10 SAID colour field i.e. VLAN ID */

    dot10_colour = vlan_read_tring_dot10_colour(packet_2_fastswitch);

    dot10_vlan_interface = dot10_colour_2_swidb(dot10_colour,
			   packet_2_fastswitch->if_input->hwptr);
    /*
     * In the event of receiving a 802.10 packet with a VLAN Identifier
     * (i.e. Colour) that is not supported/configured on a software IDB
     * belonging to this interface, then just junk it.
     *
     * This should be rate limited ...
     */

    if (dot10_vlan_interface == NULL) {

        if (vlan_packet_debug) {

            buginf ("\nvLAN: IEEE 802.10 packet bearing vLAN ID %d received"
                    " on physical\n      interface %s which is not configured"
		    " to route/switch ID %d.\n", dot10_colour,
		     packet_2_fastswitch->if_input->namestring, dot10_colour);
        }

        return (FALSE);

    } else

        packet_2_fastswitch->if_input = dot10_vlan_interface;


    /*
     * O.K. Classify the interior packet and then check that we're
     * configured to route that protocol on this subinterface.
     *
     * If not then consider transparently bridging it ;-(
     */

    dot10_interior_rxtype = classify_tring_dot10_packet(packet_2_fastswitch);

    if (routing_dot10_vlan_on_subif(dot10_vlan_interface,
			            dot10_interior_rxtype) == TRUE)

        /* Let's route the packet !!! */

        return(route_tring_dot10_packet(packet_2_fastswitch,
					dot10_interior_rxtype));
    /*
     * Otherwise return to the calling fast switch interrupt and let it
     * determine if we're bridging the packet.
     */

    return(FALSE);
}


