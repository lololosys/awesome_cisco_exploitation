/*--------------------------------------------------------------------------
 *
 * $Id: tbridge_isl_inline.h,v 3.4.6.2 1996/05/19 22:22:14 hou Exp $
 *
 * tbridge/tbridge_isl_inline.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Inline functions for Transparent Bridging of ISL vLAN encapsulated frames.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: tbridge_isl_inline.h,v $
 * Revision 3.4.6.2  1996/05/19  22:22:14  hou
 * CSCdi54416:  ISL bridging from 7000 to Cat5000 is broken
 * Branch: California_branch
 * - add code path to support packet being bridged from different medium
 *   interfaces to ISL vLAN interface on 7000.
 *
 * Revision 3.4.6.1  1996/05/19  21:14:12  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - add  bridging support for packet from different medium
 *   to ISL vlan fast ethernet on RSP.
 * - fix flooding path problem for packet coming from ISL vLAN interface.
 *   on RSP and 7000.
 *
 * Revision 3.4  1996/03/05  18:09:57  rbadri
 * CSCdi50469:  Transparent Bridging with ISL doesnt work
 * While writing the ISL header, restore datagramstart/mac_start to
 * the appropriate values to preserve the LLC header and data.
 *
 * Revision 3.3  1996/01/23  08:54:55  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


/*
 * Prepend a 26 byte ISL vLAN Header to a bridged packet egressing out
 * a Fast Ethernet interface.
 *
 * Note write the source address of the interior frame into the exterior
 * header in case the packet is processed by an intermediate switching device.
 * Also bytes 20 and 21 assume the colour assigned to the "output_swidb".
 *
 * This should be platform independent.
 */

static inline
enum TBRIDGE_TLB_RETVAL tbridge_prepend_isl_header (idbtype *output_swidb,
                                                    paktype *pak)
{


#ifdef TBRIDGE_MCI_CBUS

    mciregtype *mci_regptr = output_swidb->hwptr->mci_regptr;  

#else

    register uchar *packet_ptr;    

#endif

    pak->datagramstart -= ISL_HEADER_LENGTH;
    pak->datagramsize += ISL_HEADER_LENGTH;


#ifdef TBRIDGE_MCI_CBUS

    mci_regptr->write1long = *((ulong *)&isl_vlan_multicast[0]);
    mci_regptr->write1short = GETSHORT(&isl_vlan_multicast[4]);

    mci_regptr->write1long = *((ulong *)&pak->addr_start[0]);
    mci_regptr->write1short = GETSHORT(&pak->addr_start[4]);

    mci_regptr->write1short = (pak->datagramsize -
			      ETHER_8023_ENCAPBYTES);

    mci_regptr->write1long = *((ulong *)&isl_vlan_snap[0]);
    mci_regptr->write1short = GETSHORT(&isl_vlan_snap[4]);  

    mci_regptr->write1short = ((output_swidb->isl_vlan->colour) << 1);

    mci_regptr->write1long = 0x0;

    /*
     * Nota Bene: On the Cisco 7000 FEIP platform
     * the microcode determines on a packet by
     * packet basis whether or not to generate a
     * software CRC (ISL requires a CRC for the
     * interior packet, then an appended exterior CRC
     * covering the entire packet ... ugly!). So the
     * mechanism to indicate this is to set bit 13 in
     * the "len" field passed down to the FEIP; note
     * the actual length on Fast Ethernet can be amply
     * encoded in the first 10 bits!!!
     */

    pak->datagramsize |= ISL_DO_CRC_MASK;

#else

    packet_ptr = pak->datagramstart;

    /* Firstly drop in the ISL (i.e. Cisco) multicast header */

    ieee_copy(isl_vlan_multicast, packet_ptr);

    /*
     * In case any intermediate non-ISL speaking 802.1d device is
     * learning source addresses, rewrite the source MAC address
     * for the exterior packet.
     */

    ieee_copy (packet_ptr + ISL_HEADER_LENGTH + IEEEBYTES,
	       packet_ptr + IEEEBYTES);

    /*
     * Copy in the length, we're on an even boundary so should be kool ;-) Also
     * include the interior 4 byte CRC. The ucode will update the 802.3 length.
     */

    PUTSHORT((packet_ptr + (2 * IEEEBYTES)),
	     (pak->datagramsize - ETHER_8023_ENCAPBYTES));

    /*
     * For packet memory based platforms, mark the packet as requiring ISL
     * inner CRC generation; done by the FEIP ucode until the MICE hardware
     * assist is available at which time the "PAK_DO_ISL_CRC" becomes redundant.
     */

    pak->flags |= PAK_DO_ISL_CRC;

    /* Then 6 bytes of the SNAP header, Cisco OUI used */

    ieee_copy(isl_vlan_snap, packet_ptr + ETHER_8023_ENCAPBYTES);

    /*
     * Shift the colour field one bit position since ISL defines the LSB
     * to indicate whether or not the packet is a Spanning Tree BPDU or
     * Cisco Discovery packet!
     */

    PUTSHORT((packet_ptr + ISL_VLAN_ID_OFFSET),
	     (output_swidb->isl_vlan->colour << 1));

#endif


    output_swidb->isl_vlan->egressing_isl_packets[VLAN_TBRIDGE]++;

    /* return TBRIDGE_TLB_DONE so as to fall into the tbridge_x_inline model */

    return (TBRIDGE_TLB_DONE);
}
