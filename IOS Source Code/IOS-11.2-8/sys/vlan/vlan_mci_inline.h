/*-------------------------------------------------------------------------
 *
 * $Id: vlan_mci_inline.h,v 3.7.10.3 1996/08/15 00:45:05 tylin Exp $
 *
 * vlan/vlan_mci_inline.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Inline functions specific to the protocol independent routing and switching
 * between IEEE 802.10 based and Inter Switch Link (ISL) Virtual LANs on the
 * CxBus platform, i.e. Cisco 7000s.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_mci_inline.h,v $
 * Revision 3.7.10.3  1996/08/15  00:45:05  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.7.10.2  1996/07/17  18:36:35  tylin
 * CSCdi62787:  CDP does not work correctly with ISL encapsulation
 *    CDP packet should be always SNAP encapsulated.
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/06/28  21:57:38  dawnli
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
 * Revision 3.7  1996/02/25  03:52:48  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.6  1996/02/05  04:04:12  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.5  1996/01/23  08:55:38  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#ifndef __VLAN_MCI_INLINE_H__
#define __VLAN_MCI_INLINE_H__


/*
 * Platform specific exported functions and external declarations.
 */

extern  boolean    dot10_ether_vlan_fastswitch (hwidbtype *input_hwidb);
extern  boolean    dot10_fddi_vlan_fastswitch (hwidbtype *input_hwidb);
extern  boolean    dot10_tring_vlan_fastswitch (hwidbtype *input_hwidb);
extern  boolean    isl_fastswitch (hwidbtype *input_hwidb);


/*
 * Identify the type of the interior packet.
 *
 * See if it's a protocol we can route, otherwise if configured we'll bridge
 * the packet.
 * This is called in fastswitching a received ISL frame, and allows us to
 * determine if we're configured to route on this subinterface, and what fast
 * switching protocol registry to call.
 *
 * The assumption here is that the interior packet has an Ethernet format,
 * be that Version2, SNAP or 802.3.
 *
 * Nota Bene: The ucode has laid the packet down such that interior packet's
 *            MAC header starts at `rxoffset' i.e. MCI_ETHER_OFFSET.
 */

static inline enum RXTYPE classify_isl_interior_packet (hwidbtype *input_hwidb)
{

mciregtype      *mci_regptr;
register ushort ether_type;

    mci_regptr         = input_hwidb->mci_regptr;
    mci_regptr->argreg = (MCI_ETHER_OFFSET + IEEEBYTES);
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    ether_type = mci_regptr->readshort;

    /*
     * Check to see if the frame has an 802.3 or Version 2 encapsulation.
     */

    if (ether_type > (MAXETHERSIZE + ISL_HEADER_LENGTH)) {

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
        else if (ether_type == TYPE_DECNET)
	    return (RXTYPE_DECNET);
        else if (ether_type == TYPE_XNS)
	    return (RXTYPE_XNS);
        else if (ether_type == TYPE_APOLLO)
	    return (RXTYPE_APOLLO);

	else 
	    return (RXTYPE_UNKNOWN);

    /* 
     * Now classify either an 802.3/802.2 or a SNAP frame, based on LSAP value.
     * Note the read pointer should be left at the start of the LLC field so no
     * need to reselect. Only do so if determine that we need to skip over the
     * SNAP header, in which case we duplicate the above, but this is built for
     * speed, and as SNAP on Ether is much less common why check for it first!
     */

    } else {

	if (ether_type == SNAPSNAP) {

            mci_regptr->argreg = (input_hwidb->rxoffset +
				  INTERIOR_SNAP_PACKET_TYPE_OFFSET);
            mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

            ether_type = mci_regptr->readshort;

            if (ether_type == TYPE_IP10MB)
                return (RXTYPE_DODIP);
            else if (ether_type == TYPE_NOVELL1)
                return (RXTYPE_NOVELL1);
            else if ((ether_type == TYPE_ETHERTALK) || (TYPE_AARP))
                return (RXTYPE_APPLE);
            else if (ether_type == TYPE_DECNET)
                return (RXTYPE_DECNET);
            else if (ether_type == TYPE_XNS)
                return (RXTYPE_XNS);
            else if (ether_type == TYPE_APOLLO)
                return (RXTYPE_APOLLO);
            else
                return (RXTYPE_UNKNOWN);

        } else {

            ether_type = mci_regptr->readshort;

            if (ether_type == (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8))

            /*
             * Legacy support for the much deprecated "Novell Ether"
             * i.e. 802.3 format, but then just a bogus IPX header
             * checksum (0xFFFF) instead of an 802.2 header :-(
             */
                return (RXTYPE_NOVELL2);

	    else if (ether_type == TYPE_CLNS)
		return (RXTYPE_CLNS);
	    else if (ether_type == (SAP_NOVELL | SAP_NOVELL << 8))
		return (RXTYPE_SAP_NOVELL);
	    else
		return (RXTYPE_UNKNOWN);
	}
    }
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

static inline ushort vlan_read_isl_colour (hwidbtype *input_hwidb)
{

register mciregtype  *regptr;

    /*
     * Scrawl back the read pointer to the start of the exterior
     * Destination Address and then point it at the colour field. 
     */

    regptr = input_hwidb->mci_regptr;

    regptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET)
	             + ISL_COLOUR_OFFSET;
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    return ((regptr->readshort) >> 1);
}

/*
 * isl_cdp_mci_packet()
 * CDP packet is always SNAP encap.
 */

static inline boolean isl_cdp_mci_packet (hwidbtype *input_hwidb)
{
    mciregtype *mci_regptr = input_hwidb->mci_regptr;
    mci_regptr->argreg = MCI_ETHER_OFFSET + (ISO2TYPE / 2);
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    if (mci_regptr->readshort == TYPE_CDP) {
        return (TRUE);
    } else
        return (FALSE);
}


/****************************************************/
/***                                              ***/
/*** ... Now comes the IEEE 802.10 vLAN stuff ... ***/
/***                                              ***/
/****************************************************/


/*
 * classify_dot10_interior_packet()
 *
 * Identify the interior type of a dot10 encapsulated packet.
 *
 * See if it's a protocol we can route, otherwise if configured we'll bridge
 * the packet.
 * This is called in fastswitching a received dot10 frame, and allows us to
 * determine if we're configured to route on this subinterface, and what fast
 * switching protocol registry to call.
 */

static inline
enum RXTYPE classify_dot10_interior_packet (hwidbtype *input_hwidb)
{

mciregtype      *regptr;
register ushort ether_type;

    regptr = input_hwidb->mci_regptr;

    regptr->argreg = (input_hwidb->rxoffset + SNAP_OUIBYTES);
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    if (regptr->readshort == SNAPSNAP) {

        /*
         * The packet began life in Ethernet V2 or SNAP format, so skip
	 * over the SNAP header and pick out the type. Note Ethernet V2
	 * encapsulated packets are "Snapped" before being translated to
	 * dot10.
	 */

        regptr->argreg = (input_hwidb->rxoffset + SNAP_HDRWORDS);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

        ether_type = regptr->readshort;

        if (ether_type == TYPE_IP10MB)
            return (RXTYPE_DODIP);
        else if (ether_type == TYPE_RFC826_ARP)
            return (RXTYPE_DODIP);
        else if (ether_type == TYPE_NOVELL1)
            return (RXTYPE_NOVELL1);
        else if ((ether_type == TYPE_ETHERTALK) || (ether_type == TYPE_AARP))
            return (RXTYPE_APPLE);
        else if (ether_type == TYPE_DECNET)
            return (RXTYPE_DECNET);
        else if (ether_type == TYPE_XNS)
            return (RXTYPE_XNS);
        else if (ether_type == TYPE_APOLLO)
            return (RXTYPE_APOLLO);
        else
            return (RXTYPE_UNKNOWN);

    } else {

	/* We know then that the interior packet is in 802.3 format */

        regptr->argreg = (input_hwidb->rxoffset +
			  SDE_SAID_BYTES + SDE_PT_FLAG_BYTES);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

	ether_type = (regptr->readshort << 8);
        ether_type &= (0xff00 | (regptr->readshort >> 8));  /* Fuck !!! */

        if (ether_type == TYPE_CLNS)
            return (RXTYPE_CLNS);
        else if (ether_type == (SAP_NOVELL | SAP_NOVELL << 8))
            return (RXTYPE_SAP_NOVELL);
        else if (ether_type == (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8))
            return (RXTYPE_NOVELL2);
        else
            return (RXTYPE_UNKNOWN);
    }
}


/*
 * vlan_mciread_dot10_colour ()
 *
 * Extract the 4 byte SAID (Virtual LAN Identifier) from a received IEEE
 * 802.10 packet on Ethernet.
 *
 * Note that this assumes that the rxoffset has been placed 14 bytes back 
 * from the start of the network header, to expedite Fast Switch routing !!!
 * For the microcode this means it needs to lay the packet down (on an even
 * boundary) position dependent for dot10 and dot10 SNAP.
 *
 * i.e. Our received packet off the wire looks like the following where the
 *      AC/FC or FC prepend is present for Token Ring and FDDI respectively.
 *
 * -------------------------------------------------------------
 * |  |  |    |      |        |    |Station|Frag|Original |    |
 * |AC|FC|Dest|Source|0A 0A 03|SAID|       |    |         |Data|
 * |  |  |    |      |        |    |  ID   |Flag|802.2 LLC|    |
 * -------------------------------------------------------------
 *                   <-------- 16 bytes -------->
 *
 * or in the SNAP case (note this encapsulation is much more common on FDDI
 * and Token Ring.
 *
 *--------------------------------------------------------------------------
 *|  |  |    |      |      |    |          |Frag|SNAP Header |Original|    |
 *|AC|FC|Dest|Source|0A0A03|SAID|Station ID|    |            |        |Data|
 *|  |  |    |      |      |    |          |Flag|AA AA 03 OUI| Type   |    |
 *--------------------------------------------------------------------------
 *                  <--------- 16 bytes -------->
 *
 * As an optimization, maybe get the ucode boys to classify SNAP and non-SNAP
 * dot10 packets, coz you're taking a performance hit here, particularly in 
 * reading from an uneven boundary ;-(
 */

static inline ulong vlan_mciread_dot10_colour (hwidbtype *input_hwidb)
{

register mciregtype  *regptr;
register ulong        said;

    /*
     * Check firstly in case we have SNAP.
     * Scrawl back the read pointer to the location of the 802.10 colour field.
     */

    regptr = input_hwidb->mci_regptr;

    regptr->argreg = (MCI_OFFSETSIZE - SNAP_HDRWORDS);
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    if (regptr->readshort == SNAPSNAP) {

	/* 
	 * The packet began life in SNAP format, so go back abit further
	 * (21 bytes) and shuffle around the even boundary read constraint.
	 */

	regptr->argreg = (MCI_OFFSETSIZE -
			  ((SDE_SAID_BYTES + SDE_PT_SID_BYTES) / 2) -
			  SDE_PT_FLAG_BYTES - SNAP_HDRWORDS);
        regptr->cmdreg = MCI_CMD_RX_SELECT;

        said = (regptr->readshort << 8);
        said &= (0xff000000 | (regptr->readlong >> 8));  /* Fuck !!! */

        return(said);
    }

    else {

        regptr->argreg = (input_hwidb->rxoffset - SDE_PT_FLAG_BYTES);
        regptr->cmdreg = MCI_CMD_RX_SELECT;
	return (regptr->readlong);
    }
}


#endif /* __VLAN_MCI_INLINE_H__ */


