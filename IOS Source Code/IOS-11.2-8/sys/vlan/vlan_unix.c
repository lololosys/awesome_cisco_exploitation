/* $Id: vlan_unix.c,v 3.1 1996/02/29 02:31:44 hampton Exp $
 * $Source: /release/111/cvs/Xsys/vlan/vlan_unix.c,v $
 *------------------------------------------------------------------
 * Functions specific to the protocol independent routing between IEEE
 * 802.10 and Inter Switch Link (ISL) Virtual LANs on the UNIX router.
 *
 * June 1995, Martin McNealis.
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vlan_unix.c,v $
 * Revision 3.1  1996/02/29  02:31:44  hampton
 * Add UNIX (non)support for VLAN. [CSCdi50330]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "types.h"
#include "interface_private.h"
#include "fastswitch_registry.h"


/*
 * ISL encapsulation not supported on the unix images.
 */
boolean write_isl_encapsulation (idbtype *output_swidb, paktype *pak)
{
    return(FALSE);
}


/*
 * Exported functions, static storage allocation and external declarations.
 */


void vlan_register_fastswitching (void)
{
    /*
     * IEEE 802.10 frames for routing/switching can appear on all LAN media
     * and all platforms whereas the ISL vLAN trunking protocol is only
     * supported on the Fast Ethernet interface.
     */

    reg_add_ether_fs(RXTYPE_IEEE_802_10,
		     (service_ether_fs_type) return_false, "return_false");

    reg_add_fddi_fs(RXTYPE_IEEE_802_10,
                    (service_fddi_fs_type) return_false, "return_false");

    reg_add_token_fs(RXTYPE_IEEE_802_10,
                    (service_token_fs_type) return_false, "return_false");

    reg_add_ether_fs(RXTYPE_ISL,
		     (service_ether_fs_type) return_false, "return_false");

    reg_add_rsp_queue_ether_for_process(
				 (service_rsp_queue_ether_for_process_type)
                                 return_false, "return_false");

    reg_add_rsp_queue_fddi_for_process(
				(service_rsp_queue_fddi_for_process_type)
                                 return_false, "return_false");

    reg_add_rsp_queue_trip_for_process(
				(service_rsp_queue_trip_for_process_type)
                                 return_false, "return_false");
}
