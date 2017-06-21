/*-------------------------------------------------------------------------
 *
 * $Id: vlan_rsp.c,v 3.2.22.2 1996/03/29 19:21:00 hou Exp $
 *
 * vlan/vlan_rsp.c
 *
 * June 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Functions specific to the protocol independent routing/switching between
 * IEEE 802.10 and Inter Switch Link (ISL) Virtual LANs on the Routing and
 * Switching Processor (RSP) platform.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_rsp.c,v $
 * Revision 3.2.22.2  1996/03/29  19:21:00  hou
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
 * Revision 3.2.22.1  1996/03/18  22:33:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.4  1996/03/14  01:37:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.1.28.3  1996/03/07  11:06:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.2  1996/02/20  21:48:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1996/01/23  08:55:41  mmcneali
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
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers_inline.h"

#include "sys_registry.h"
#include "fastswitch_registry.h"

#include "../ui/debug.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"

#include "../hes/if_cmd.h"
#include "../hes/if_trip.h"

#include "../tbridge/tbridge_sde.h"

#include "../srt/span.h"
#include "../srt/srt_debug_flags.h"

#include "../cdp/cdp_debug_flags.h"

#include "../src-rsp/rsp_if.h"

#include "vlan.h"
#include "vlan_debug_flags.h"
#include "vlan_private.h"
#include "vlan_inline.h"

/*
 * Exported functions, static storage allocation and external declarations.
 */
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

    reg_add_rsp_queue_ether_for_process(
				(service_rsp_queue_ether_for_process_type)
			         rsp_queue_ether_for_process,
                                 "rsp_queue_ether_for_process");

    reg_add_rsp_queue_fddi_for_process(
			        (service_rsp_queue_fddi_for_process_type)
			         rsp_queue_fddi_for_process,
                                 "rsp_queue_fddi_for_process");

    reg_add_rsp_queue_trip_for_process(
			        (service_rsp_queue_trip_for_process_type)
			         rsp_queue_trip_for_process,
                                 "rsp_queue_trip_for_process");
}
