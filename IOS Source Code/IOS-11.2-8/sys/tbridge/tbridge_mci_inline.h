/* $Id: tbridge_mci_inline.h,v 3.8.18.7 1996/06/04 02:03:48 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_mci_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_mci_inline.h
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * MCI/cBus-specific in-line functions for transparent bridging.
 *------------------------------------------------------------------
 * $Log: tbridge_mci_inline.h,v $
 * Revision 3.8.18.7  1996/06/04  02:03:48  wilber
 * CSCdi59204:  encapsulation bridging fails on FCI card
 * Should copy data to new paks datagramstart instead of mac_start
 * Branch: California_branch
 *
 * Revision 3.8.18.6  1996/05/22  02:22:57  hou
 * CSCdi54418:  rql on a MEC card goes down to 0.  line prot down with
 * high ignor
 * Branch: California_branch
 * - tbridge_mci_flush_datagram_inline()
 *   set cmdreg correctly to flush the buffer of input interface.
 *
 * Revision 3.8.18.5  1996/05/19  21:14:13  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - add  bridging support for packet from different medium
 *   to ISL vlan fast ethernet on RSP.
 * - fix flooding path problem for packet coming from ISL vLAN interface.
 *   on RSP and 7000.
 *
 * Revision 3.8.18.4  1996/05/04  05:09:08  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.8.18.3  1996/04/23  06:05:49  weismann
 * CSCdi51054:  atm 1483 spanning tree bpdus not to spec
 * Branch: California_branch
 * Make to spec.
 *
 * Revision 3.8.18.2  1996/03/28  02:09:38  hampton
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net.  [CSCdi52864]
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  22:12:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/13  02:03:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/02/20  18:52:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/12  04:52:16  amajumda
 * CSCdi48155:  problems with bridging on smds subinterfaces
 *
 * Revision 3.7  1996/02/05  10:29:31  amajumda
 * CSCdi48155:  problems with bridging on smds subinterfaces. Fix to pick
 * the
 * correct smds subinterface address swidb->vaddr->addr for the smds
 * source
 * address as opposed to spd->smds_hardware.
 *
 * Revision 3.6  1996/02/01  21:29:52  wmay
 * CSCdi47958:  transparent bridging over smds fails on 7000 - use
 * correct offset to get to smds source address.
 *
 * Revision 3.5  1996/01/19  22:16:20  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * remove sniff of destination address - it is done before we get here.
 *
 * Revision 3.4  1995/12/14  20:59:25  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.3  1995/11/30  22:55:09  hou
 * CSCdi39644:  Transparent bridging fails over MultiBus token ring and
 * MCI serial
 * MCI serial
 * - tbridge_mci_copy_rx_pak_inline():
 *   Copy the serial transit packet into pak buffer starting from
 *   datagramstart instead of mac_start.
 *
 * Revision 3.2  1995/11/17  18:45:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:31:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/21  19:15:06  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * recode congestion cases in tbridge_forward_inline and
 * tbridge_flood_inline.
 * These did not adequately handle MTU oversize issues and did not
 * correctly
 * count buffers on multi-VC cases like Frame relay.
 *
 * Revision 2.5  1995/09/19  20:51:47  pitargue
 * CSCdi40560:  giants seen while tbridging over atm (1483)
 * make sure packet offsets are correct when sending rfc 1483 encap'ed
 * packets to process level
 *
 * Revision 2.4  1995/08/18  18:30:31  wmay
 * CSCdi38128:  pings fail over wan t-bridging link when ppp encap is
 * configured - change HDLC_ENCAPBYTES to transit_encsize
 *
 * Revision 2.3  1995/08/12  07:42:38  fred
 * CSCdi38525:  rewrite fair queue conversation discriminator using IEEE
 *              definitions
 *
 * Revision 2.2  1995/06/10  02:26:13  wilber
 * CSCdi34297:  Trans Bridging failed from MultiBus token to cbus for SNAP
 * pkts
 * When the input is MultiBus token, we should copy data portion of the
 * packet from pak->info_start instead of pak->network_start unless we
 * want to strip the SNAP header to convert it to ARPA - otherwise we
 * drop the SNAP header
 *
 * Revision 2.1  1995/06/07  23:03:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TBRIDGE_MCI_INLINE_H__
#define __TBRIDGE_MCI_INLINE_H__


/*
 * Read a MAC header from an MCI/cBus interface into tbridge_encap
 * just as the header was received.  Packet pointers in encap_pak
 * are set to point to the beginning of the MAC addresses and to
 * the beginning of the LLC header if one is present.  You may
 * wish to read tbridge_hes_private.h for the definitions of the
 * offsets in tbridge_encap_pak.
 *
 * The hardware idb field span_macoffset always points at DA
 * in the receive window for the input interface on the MCI/cBus.
 *
 * The code switch allow the caller to optimize this function
 * call for the given media type.
 *
 * Note that the caller must guarantee that tbridge_encap is
 * longword aligned.
 *
 * The MCI/cBus reads are all longword aligned.
 *
 * EXPLICITLY SELECTS THE RX OFFSET.
 */

static inline
void tbridge_mci_read_mac_header_inline (hwidbtype *hwidb,
					 paktype *encap_pak,
					 enum LENGTH_802 code_switch)

{
    mciregtype *mci_regptr;

    mci_regptr = hwidb->mci_regptr;
    mci_regptr->argreg = tbridge_dlli.da_offset;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Read the destination and source MAC addresses.
     */
    TBE_DADADADA = mci_regptr->readlong;
    TBE_DADASASA = mci_regptr->readlong;
    TBE_SASASASA = mci_regptr->readlong;

    encap_pak->addr_start = &TBE_DA;

    /*
     * Check for FDDI-T and CTR (802.5-like) interfaces.
     */
    switch (code_switch) {

    case LENGTH_802_NOT:

	/*
	 * We have either SAP or SNAP. Use the LSAP to decide.
	 */
	TBE_LSAP_802_5 = mci_regptr->readshort;

	if (TBE_LSAP_802_5 == SNAPSNAP) {

	    /*
	     * Must be SNAP.
	     * Read CTL and the first byte of the OUI
	     */
	    TBE_CTL_OUI_802_5 = mci_regptr->readshort;

	    /* Read the remaining two OUI bytes and the SNAP TYPE. */
	    TBE_OUI_TYPE_802_5 = mci_regptr->readlong;

	} else if (TBE_LSAP_802_5 == SAPSAP_SDE) {

            TBE_SDE_CTL_SAID_0_802_5 = mci_regptr->readshort;
            TBE_SDE_SAID_1_3_SID_0_802_5 = mci_regptr->readlong;
            TBE_SDE_SID_1_4_802_5 = mci_regptr->readlong;
            TBE_SDE_SID_5_7_FLAG_802_5 = mci_regptr->readlong;
            TBE_SDE_FID_802_5 = mci_regptr->readlong;
            TBE_SDE_SNAP_LSAP_CTL_OUI_0_802_5 = mci_regptr->readlong;

            if (TBE_SDE_SNAP_LSAP_802_5 == SNAPSNAP)
                TBE_SDE_SNAP_OUI_1_2_TYPE_802_5 = mci_regptr->readlong;
        }

	/* LLC header starts at the DSAP. */
	encap_pak->info_start = &TBE_LLC_802_5;

	break;

    case LENGTH_802_YES:

	/*
	 * We have either ARPA or 802.3. Use the LENGTH to decide.
	 */
	TBE_LEN_802_3 = mci_regptr->readshort;

	if (TBE_LEN_802_3 <= MAXETHERSIZE) {

	    /*
	     * Must be 802.3.
	     * Read the DSAP and SSAP.
	     */
	    TBE_LSAP_802_3 = mci_regptr->readshort;

	    if (TBE_LSAP_802_3 == SAPSAP_SDE) {

                TBE_SDE_CTL_SAID_0_2_802_3 = mci_regptr->readlong;
                TBE_SDE_SAID_3_SID_0_2_802_3 = mci_regptr->readlong;
                TBE_SDE_SID_3_6_802_3 = mci_regptr->readlong;
                TBE_SDE_SID_7_FLAG_FID_0_1_802_3 = mci_regptr->readlong;
                TBE_SDE_FID_2_3_SNAP_LSAP_802_3 = mci_regptr->readlong;

                if (TBE_SDE_SNAP_LSAP_802_3 == SNAPSNAP) {
                    TBE_SDE_SNAP_CTL_OUI_802_3 = mci_regptr->readlong;
                    TBE_SDE_SNAP_TYPE_802_3 = mci_regptr->readshort;
                }

            } else {

		/* Read the CTL and OUI. */
		TBE_CTL_OUI_802_3 = mci_regptr->readlong;
		
		/* Read the SNAP TYPE. */
		TBE_SNAPTYPE_802_3 = mci_regptr->readshort;
	    }
		
	    /* LLC header starts at the DSAP. */
	    encap_pak->info_start = &TBE_LLC_802_3;

	} else {

	    /* Must be ARPA. */
	    encap_pak->info_start = NULL;
	}

	break;
    }
}

/*
 * Copy the SA from a FDDI-transit header to the appropriate
 * position in the packet.
 *
 * RE-SELECTS THE RX OFFSET.
 */

static inline
void tbridge_mci_read_fddi_transit_proxy_inline (hwidbtype *hwidb,
						 paktype *encap_pak)

{
    mciregtype *mci_regptr;
    fddi_hdr *fddi = (fddi_hdr *) encap_pak->datagramstart;

    mci_regptr = hwidb->mci_regptr;
    mci_regptr->argreg = (FDDI_RECEIVE_OFFSET +
			  (FDDI_PREAMBLE / 2) +     /* skip the FC */
			  IEEEWORDS);               /* skip the DA */
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    PUTSHORT((ushort *) &fddi->saddr[0], mci_regptr->readshort);
    PUTSHORT((ushort *) &fddi->saddr[2], mci_regptr->readshort);
    PUTSHORT((ushort *) &fddi->saddr[4], mci_regptr->readshort);
}

static inline
void tbridge_mci_swap_mac_address_inline (void)
{
    tbridge_encap[0].d.byte[0] = BITSWAP(tbridge_encap[0].d.byte[0]);
    tbridge_encap[0].d.byte[1] = BITSWAP(tbridge_encap[0].d.byte[1]);
    tbridge_encap[0].d.byte[2] = BITSWAP(tbridge_encap[0].d.byte[2]);
    tbridge_encap[0].d.byte[3] = BITSWAP(tbridge_encap[0].d.byte[3]);
    tbridge_encap[1].d.byte[0] = BITSWAP(tbridge_encap[1].d.byte[0]);
    tbridge_encap[1].d.byte[1] = BITSWAP(tbridge_encap[1].d.byte[1]);
    tbridge_encap[1].d.byte[2] = BITSWAP(tbridge_encap[1].d.byte[2]);
    tbridge_encap[1].d.byte[3] = BITSWAP(tbridge_encap[1].d.byte[3]);
    tbridge_encap[2].d.byte[0] = BITSWAP(tbridge_encap[2].d.byte[0]);
    tbridge_encap[2].d.byte[1] = BITSWAP(tbridge_encap[2].d.byte[1]);
    tbridge_encap[2].d.byte[2] = BITSWAP(tbridge_encap[2].d.byte[2]);
    tbridge_encap[2].d.byte[3] = BITSWAP(tbridge_encap[2].d.byte[3]);
}

/*
 * Read the extra values from the input interface that we need to
 * do LAT compression on the output interface.  The logic here looks
 * very much like tbridge_check_lat_compression, but it does not do
 * the actual "compression".  It just reads the required values into
 * the packet.  This is less efficient than doing the "compression"
 * at the same time, but it keeps the actual "compression" logic in
 * one place.  And it's arcane enough that it merits containment.
 *
 * RE-SELECTS THE RX OFFSET.
 */

static inline
void tbridge_mci_read_lat_values_inline (hwidbtype *inputhw,
					 idbtype *outputsw,
					 paktype *encap_pak)

{
    mciregtype *mci_regptr;

    mci_regptr = inputhw->mci_regptr;

    /*
     * Don't need any further values to deal with larger packets.
     */
    if (encap_pak->datagramsize > MINETHERSIZE)
	return;

    /*
     * Check that the high 6 bits of the LAT type byte are zero,
     * indicating a normal data packet.  Then check to see how many
     * entries are in this LAT packet.
     *
     * If the etype is TYPE_LAT we need to suck in the next short
     * to get more LAT stuff.  We should still be positioned at the
     * short following the type code so just suck it in.  If you
     * are really paranoid the following code will make this bullet
     * proof:
     *
     *	mci_regptr->argreg = tbridge_dlli.da_offset + ETHER_ARPA_ENCAPBYTES/2;
     *	mci_regptr->cmdreg = RX_SELECT;
     *	input->MCI_DSAP = mci_regptr->readshort;
     */

    /*
     * Okay, I will ...
     */
    mci_regptr->argreg = tbridge_dlli.da_offset + ETHER_ARPA_ENCAPBYTES/2;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    PUTSHORT((ushort *) &encap_pak->network_start[0], mci_regptr->readshort);

    /*
     * Don't need any further values to deal with non-normal packets
     * or normal packets with other than a single entry.
     */
    if ((encap_pak->network_start[0] & 0xFC) ||
	(encap_pak->network_start[1] != 1))
	return;

    mci_regptr->argreg = tbridge_dlli.da_offset + (IEEEWORDS*2)+LAT_ONEOFFSET;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    PUTSHORT((ushort *) &encap_pak->network_start[10], mci_regptr->readshort);
}


/*
 * Move a packet in a receive window in the input interface
 * to a transmit window in an output interface.  Note that
 * we do this all through the register set associated with
 * the input interface, and will eventually initiate the
 * actual output transmission from the input register set.
 * This is on-complex forwarding without a packet copy.
 *
 * EXPLICITLY SELECTS THE TX OFFSET.
 */

static inline
boolean tbridge_mci_rx_move_select_inline (hwidbtype *inputhw,
					   hwidbtype *outputhw,
					   ushort tx_offset)

{
    mciregtype *mcireg = inputhw->mci_regptr;

    if (outputhw->output_qcount && outputhw->priority_list)
	return(FALSE);
    mcireg->argreg = outputhw->mci_index;
    mcireg->cmdreg = MCI_CMD_RX_MOVE;

    /*
     * move operation to complete before checking the status of
     * that operation.
     */
    /*
     * Doing this here puts a wrench in the accounting logic.
     * Finesse it for now and think of the right thing to do
     * here while the rx_move completes.
     */
    inputhw->counters.inputs_fast[ACCT_PROTO_BRIDGE]++;
    inputhw->counters.inputs_fast[ACCT_PROTO_BRIDGE]--;

    if (mcireg->cmdreg != MCI_RSP_OKAY)
	return(FALSE);

    mcireg->argreg = tx_offset;
    mcireg->cmdreg = MCI_CMD_TX1_SELECT;

    return(TRUE);
}

/*
 * Select the tx window in an MCI/cBus interface in preparation
 * for building and transmitting a bridged packet.  This is
 * off-complex forwarding.
 *
 * Old comment:
 * We use a word offset of 1 because of a problem in the FCIT DMA controller.
 * We have a problem if only 4 long words or fewer are in the TX FIFO.  To
 * make sure this straddles and becomes 5 words we start at a word offset of 1.
 *
 * EXPLICITLY SELECTS THE TX OFFSET.
 */

static inline
boolean tbridge_mci_tx1_select_inline (hwidbtype *outputhw)
{
    mciregtype *mcireg = outputhw->mci_regptr;

    if (outputhw->output_qcount && outputhw->priority_list)
	return(FALSE);
    mcireg->argreg = outputhw->mci_index;
    mcireg->cmdreg = MCI_CMD_SELECT;
    mcireg->argreg = outputhw->buffer_pool;
    mcireg->cmdreg = MCI_CMD_TX1_RESERVE;

    if (mcireg->cmdreg != MCI_RSP_OKAY)
        return(FALSE);

    /*
     * Don't ya love! 11! Hah, now guess where THAT number comes from ...
     mcireg->argreg = 11;        "* add re-encap area for cBus flood *"
     */

    /*
     * Leave enough room in front of the packet to pre-pend
     * a FDDI transit header.  Note that this constant, in
     * agreement with the the "old comment" above, is an
     * odd-word value.
     */
    mcireg->argreg = TBRIDGE_MCI_TX1_OFFSET;
    mcireg->cmdreg = MCI_CMD_TX1_SELECT;

    return(TRUE);
}

/*
 * Select the tx window with arbitrary offset in an MCI/cBus interface in
 * preparation for building and transmitting a bridged packet.  This is
 * for off-complex or on-complex forwarding.
 *
 * ASSUME transmit buffer HAS been reserved.
 *
 * EXPLICITLY SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_tx1_select_offset_inline (hwidbtype *hwidb,
                                           ushort tx_offset)
{
    mciregtype *mcireg = hwidb->mci_regptr;

    mcireg->argreg = tx_offset;
    mcireg->cmdreg = MCI_CMD_TX1_SELECT;

    return;
}

/*
 * tbridge_mci_enqueue_holdq_inline()
 *
 * enqueue data in hold queue and then make sure we get an interrupt 
 * from the MCI/CBUS interface by attempting to transmit the message.
 *
 * note that we are only worried about the interrupt when we have
 * enqueued the FIRST message: once one message is in queue, the MCI/CBUS
 * has refused a reservation and knows that it needs to interrupt.
 */
static inline
enum TBRIDGE_TLB_RETVAL
tbridge_mci_enqueue_holdq_inline (hwidbtype *outputhw, paktype *pak)
{    
    holdq_enqueue_fast(outputhw, pak);
    if (1 == outputhw->output_qcount)
	outputhw->soutput(outputhw);
    return TBRIDGE_TLB_DEFERRED;
}
/*
 *    tbridge_duplicate_buffer
 *
 * Duplicate a buffer that contains the entire message, so that it can
 * be saved in a holding queue
 */
static inline
paktype * tbridge_copy_pak (paktype *pak, idbtype *outputsw)
{    
    paktype *pakcopy;
    pakcopy = getbuffer(pak->datagramsize);
    if (pakcopy) {
	pak_copy(pak, pakcopy, pak->datagramsize);
	pakcopy->if_output = outputsw;
	pakcopy->linktype = LINK_BRIDGE;
	pakcopy->acct_proto = ACCT_PROTO_BRIDGE;
	pakcopy->bridgeptr = NULL;
    }
    return(pakcopy);
}

/*
 * tbridge_mci_copy_rx_data_inline()
 *
 * Copy spcific-length data from MCI/cBus starting from an offset to
 * a network buffer.
 */
static inline
void tbridge_mci_copy_rx_data_inline (hwidbtype *inputhw,
				      ushort rx_offset,
				      uchar *dst,
				      ushort length)
{    
    mciregtype *mci_regptr = inputhw->mci_regptr;
    
    mci_regptr->argreg = rx_offset;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    mci2buffer(&mci_regptr->readlong, dst, length);
}

static inline
void tbridge_mci_read_select_inline (hwidbtype *hwidb,
				     ushort mci_read_offset)

{
    mciregtype *mci_regptr;

    mci_regptr = hwidb->mci_regptr;

    mci_regptr->argreg = mci_read_offset;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
}

/*
 * Pre-pend a transit ATM header to a packet in a tx window.
 * The tx_offset is assumed to be the offset to the beginning
 * of the packet's own MAC header.  The transmit pointer is
 * backed up from there by enough words to accommodate  the
 * transit header.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_atm_transit_hdr_wr_inline (hwidbtype *hwidb,
					    ushort vcnum,
					    ushort type)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    /*
     * add aip_encap_t info
     * vcnum and encapsulation type
     */
    mci_regptr->write1short = vcnum;
    mci_regptr->write1short = (AIP_DM_PKT_AAL5 | AIP_DM_EOM_EN);

    /*
     * add rfc1483 header
     */
    mci_regptr->write1long  = TBRIDGE_LLC_OUI_1ST_LONG;
    mci_regptr->write1short = TBRIDGE_OUI_LAST_SHORT;
    mci_regptr->write1short = type;
    mci_regptr->write1short = TBRIDGE_ATM_PAD;
}



/*
 * Pre-pend a  ATM lane header to a packet in a tx window.
 * The tx_offset is assumed to be the offset to the beginning
 * of the packet's own MAC header.  The transmit pointer is
 * backed up from there by enough words to accommodate  the
 * transit header.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_lane_ether_hdr_wr_inline (hwidbtype *hwidb,
					   ushort offset,
					   ushort vcnum,
					   ushort lecid)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    
    mci_regptr->argreg = offset - ATM_AAL5_LANE_ENCAPWORDS;
    mci_regptr->cmdreg = MCI_CMD_TX1_SELECT;
    /*
     * add aip_encap_t info
     * vcnum and encapsulation type
     */
    mci_regptr->write1short = vcnum;
    mci_regptr->write1short = (AIP_DM_PKT_AAL5 | AIP_DM_EOM_EN);

    /*
     * add lane client id
     */
    mci_regptr->write1short  = lecid;

}


/*
 * just rewrite the vc number for a rfc1483 style packet.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_atm2atm_transit_hdr_wr_inline (hwidbtype *hwidb,
						ushort vcnum)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    /*
     * rewrite the vc number
     */
    mci_regptr->write1short = vcnum;                /* vc number */
}

/*
 * Add the frame relay 1490 header to a packet in a tx window.
 * Tx offset is assumed to be point to the beginning of the
 * packet's own mac header.
 * Does not reselect the TX-offset.
 * dlci should be fr_pvc->converted_dlci
 */
static inline
void tbridge_mci_fr_ietf_hdr_wr_inline (hwidbtype *hwidb,
					ushort dlci,
					ushort type)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    ushort temp;
    if (hwidb->cache_board_encap){
	(*hwidb->cache_board_encap)(hwidb, &temp, sizeof(ushort));
	mci_regptr->write1short = temp;
    }

    /*
     * dlci number
     */
    mci_regptr->write1short = dlci;

    /*
     * add rfc1483 header
     */
    mci_regptr->write1long  = (FR_SNAP_NLPID << 16) | (FR_SNAP_CONTROL << 8);
    mci_regptr->write1short = FR_OUI23_BRIDGE;
    mci_regptr->write1short = type;
}

/*
 * Slap a cisco frame relay header on the beginning of the output
 * packet
 * Reselects the Tx-offset
 */
static inline
void tbridge_mci_fr_cisco_hdr_wr_inline (hwidbtype *hwidb,
					 hwidbtype *outputhw,
					 ushort dlci,
					 ushort tx_offset)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    ushort temp;

    mci_regptr->argreg = tx_offset - btow(outputhw->span_encapsize);
    mci_regptr->cmdreg = MCI_CMD_TX1_SELECT;
    if (outputhw->cache_board_encap){
	(*outputhw->cache_board_encap)(outputhw, &temp, sizeof(ushort));
	mci_regptr->write1short = temp;
    }

    mci_regptr->write1short = dlci;
    mci_regptr->write1short = TYPE_BRIDGE;
}

/*
 * Write an 802.10 SDE header to an MCI/cBUs interface.
 *
 * The parameter 'hwidb' is either inputhw(on-complex case) or
 * outputhw(the other cases) in order to access the appropriate
 * mci buffer.
 *
 * The parameter 'outputsw' is the output swidb for picking up the
 * SAID configured on that output sub-interface. Don't count on
 * pak->if_output because fast-swithcing code doesn't set it, in stead
 * it always passes 'outputsw' to funtions explicitly.
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_sde_header_wr_inline (hwidbtype *hwidb,
				       idbtype *outputsw,
				       uchar *src_addr)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    ulong said;

    /*
     * Will pick up configured SAID from outputsw.
     */
    
    if (!outputsw->sde_said_db)
        return;

    said = outputsw->sde_said_db->sdb_said;

    mci_regptr->write1short = SAPSAP_SDE;
    mci_regptr->write1short = (LLC1_UI << 8 | (uchar)(said >> 24));
    mci_regptr->write1short = (ushort)(said >> 8);
    mci_regptr->write1short = (((uchar)(said & 0xFF)) << 8 | *src_addr);
    mci_regptr->write1long = GETLONG(&src_addr[1]);

    /* Flag is zero for now - no fragmentation support */
    mci_regptr->write1long = (*((uchar *)(&src_addr[5]))) << 24;
}


/*
 * Write an ether MAC header to an MCI/cBUs interface.
 * The "bytes" argument may be either the ether type for
 * ARPA encap, or the length for 802.3 encap.
 * 
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_ether_mac_wr_inline (hwidbtype *hwidb,
				      paktype *pak,
				      ushort bytes)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1long = *((ulong *)&pak->addr_start[0]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[4]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[8]);
    mci_regptr->write1short = bytes;
}

/*
 * Write a token ring MAC header to an MCI/cBus interface.
 * 
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_tr_mac_wr_inline (hwidbtype *hwidb,
				   paktype *pak)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1short = FC_TYPE_LLC;
    mci_regptr->write1long = *((ulong *)&pak->addr_start[0]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[4]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[8]);
}
/*
 * Write a fddi MAC header to an MCI/cBus interface.
 * 
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_fddi_mac_wr_inline (hwidbtype *hwidb,
				     paktype *pak)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1short = FDDI_LLC_FC_BYTE;
    mci_regptr->write1long = *((ulong *)&pak->addr_start[0]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[4]);
    mci_regptr->write1long = *((ulong *)&pak->addr_start[8]);
}

/*
 * Write a token ring MAC header to an MCI/cBus interface.
 * The MAC addresses provided are assumed to be in canonical
 * order, and are bit-swapped in this function when the
 * header is written.
 * 
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_tring_mac_wr_inline (hwidbtype *hwidb,
				      ieee_addrs *addr_start)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    tring_hdr tring_copy, *tring;	/* translated MAC header */

    tring = &tring_copy;

    ieee_move(addr_start->daddr, tring->daddr);
    ieee_move(addr_start->saddr, tring->saddr);
    ieee_swap(tring->daddr, tring->daddr);
    ieee_swap(tring->saddr, tring->saddr);

    mci_regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
    mci_regptr->write1long = *((ulong *)&tring->daddr[0]);
    mci_regptr->write1long = *((ulong *)&tring->daddr[4]);
    mci_regptr->write1long = *((ulong *)&tring->saddr[2]);
}


/*
 * Write a SNAP header to an MCI/cBus interface.
 * This function assumes the appropriate MAC
 * header (fddi or token ring) has been written.
 * 
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_snap_wr_inline (hwidbtype *hwidb,
				 ulong ctl_oui,
				 ushort type)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1short = SNAPSNAP;
    mci_regptr->write1long  = ctl_oui;
    mci_regptr->write1short = type;
}

/*
 * Pre-pend a transit fddi MAC header to a packet in a tx window.
 * The tx_offset is assumed to be the offset to the beginning
 * of the packet's own MAC header.  The transmit pointer is
 * backed up from there by enough words to accommodate  the
 * transit header.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_fddi_transit_hdr_wr_inline (hwidbtype *hwidb,
					     uchar *dst_addr,
					     uchar *src_addr,
					     ushort tx_offset)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->argreg = tx_offset - FDDI_SNAP_ENCAPWORDS;
    mci_regptr->cmdreg = MCI_CMD_TX1_SELECT;

    mci_regptr->write1short = FDDI_LLC_FC_BYTE;
    mci_regptr->write1long = *((ulong *)&dst_addr[0]);
    mci_regptr->write1short = GETSHORT(&dst_addr[4]);

    mci_regptr->write1long = *((ulong *)&src_addr[0]);
    mci_regptr->write1short = GETSHORT(&src_addr[4]);

    mci_regptr->write1short = SNAPSNAP;
    mci_regptr->write1long = CISCOOUI;
    mci_regptr->write1short = TYPE_BRIDGE;
}

/*
 * Pre-pend a transit HDLC header to a packet in a tx window.
 * The tx_offset is assumed to be the offset to the beginning
 * of the packet's own MAC header.  The transmit pointer is
 * backed up from there by enough words to accommodate  the
 * transit header.
 *
 * The first argument is the idb for which transmission will
 * be initiated. For on-complex bridging, this is NOT the idb
 * associated with the output interface.  The second argument
 * is the idb associated with the output interface and we use
 * its attributes to build the appropriate transit header.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_hdlc_transit_hdr_wr_inline (hwidbtype *hwidb,
					     hwidbtype *outputhw,
					     ushort tx_offset)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    ushort temp;

    mci_regptr->argreg = tx_offset - btow(outputhw->span_encapsize);
    mci_regptr->cmdreg = MCI_CMD_TX1_SELECT;
    if (outputhw->cache_board_encap){
	(*outputhw->cache_board_encap)(outputhw, &temp, sizeof(ushort));
	mci_regptr->write1short = temp;
    }

    mci_regptr->write1long = HDLC_BRIDGECODE;
}

/*
 * Pre-pend a transit LEX header to a packet in a tx window.
 * The tx_offset is assumed to be the offset to the beginning
 * of the packet's own MAC header.  The transmit pointer is
 * backed up from there by enough words to accommodate  the
 * transit header.
 *
 * The first argument is the idb for which transmission will
 * be initiated. For on-complex bridging, this is NOT the idb
 * associated with the output interface.  The second argument
 * is the idb associated with the output interface and we use
 * its attributes to build the appropriate transit header.
 *
 * RE-SELECTS THE TX OFFSET.
 */

static inline
void tbridge_mci_lex_transit_hdr_wr_inline (hwidbtype *hwidb,
					     hwidbtype *outputhw,
					     ushort tx_offset)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;
    ushort *src = outputhw->lex_bridge_header_cache;
    short enclen = outputhw->span_encapsize;

    mci_regptr->argreg = tx_offset - btow(enclen);
    mci_regptr->cmdreg = MCI_CMD_TX1_SELECT;

    /*
     * The assumption is that outputhw->span_encapsize will always be of even
     * lenght. The place where this is set in lex_bind(). If it is not even
     * this has to change.
     */
    while (enclen > 0) {
	mci_regptr->write1short = *src++;
	enclen -= sizeof(short);
    }
}

/*
 * tbridge_mci_smds_setup
 * This routine sets up global variables for the pad, betag and basize
 * fields in the smds header.  datagramsize should be the size of the
 * entire ether/token/fddi packet to encapsulate
 */
static inline
ushort tbridge_mci_smds_setup (ushort datagramsize,
			     hwidbtype *outputhw)
{
    datagramsize += SMDS_BRIDGE_PAD_SIZE;
    smds_pad = 3 - ((datagramsize + 3) % 4);
    smds_rsvd_betag = outputhw->smds_begin_tag++;
    smds_basize = datagramsize + smds_pad + SMDS_HEADER + SNAP_HDRBYTES;
    return(datagramsize);
}

/*
 * tbridge_mci_smds_trailer_wr_inline
 * writes the smds trailer
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */
static inline
void tbridge_mci_smds_trailer_wr_inline (mciregtype *regptr, ulong last_long)
{
    switch (smds_pad) {
    case 3: last_long &= 0xff000000; break;
    case 2: last_long &= 0xffff0000; break;
    case 1: last_long &= 0xffffff00; break;
    default: break;
    }
    if (smds_pad)
	regptr->write1long = last_long;
    regptr->write1short = smds_rsvd_betag;
    regptr->write1short = smds_basize;
}

/*
 * tbridge_mci_smds_hdr_wr_inline
 * writes the header
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */
static inline
void tbridge_mci_smds_hdr_wr_inline (hwidbtype *outputhw, idbtype *swidb,
				       mciregtype *regptr,
				       uchar *smds_addr,
				       ushort pid)

{
    smds_pdb_t *spd;
    ulong *hw;

    if (outputhw->cache_board_encap){
	ushort temp;
	(*outputhw->cache_board_encap)(outputhw, &temp, sizeof(ushort));
	regptr->write1short = temp;
    }
    spd = outputhw->smds_pdb;
    if (spd->dximode) {
	regptr->write1long = (DXI_CMDTODSU << 24) | (DXI_UI << 16);
    } else {
	regptr->write1short = 0;
    }
    regptr->write1short = smds_rsvd_betag;
    regptr->write1short = smds_basize;
    regptr->write1long = GETLONG(smds_addr);
    regptr->write1long = GETLONG((smds_addr + sizeof(ulong)));
    hw = (ulong *)(swidb->vaddr ? swidb->vaddr->addr : spd->smds_hardware);
    regptr->write1long = GETLONG(hw++);
    regptr->write1long = GETLONG(hw); 
    regptr->write1short = ((smds_pad | SMDS_LLC) << 8) | XHEL_SIZE;
    regptr->write1short = 0;
    regptr->write1long = (SMDSHE_VERLEN << 24) | (SMDSHE_VERTYP << 16) |
	(SMDSHE_VERVAL << 8);
    regptr->write1long = 0;
    regptr->write1long = 0;
    regptr->write1long = (SAP_SNAP << 24) | (SAP_SNAP << 16) | (LLC1_UI << 8) |
	SMDS_BRIDGE_OUI0;
    regptr->write1long = (SMDS_BRIDGE_OUI1 << 24) | (SMDS_BRIDGE_OUI2 << 16) |
	pid;
    regptr->write1short = SMDS_BRIDGE_PAD_DATA;
}

/*
 * tbridge_smds_adjust_datagramsize
 * This will calculate the real datagramsize for the packet.
 * datagramsize is the size of the ether/token/fddi packet encapsulated
 */
static inline
ushort tbridge_smds_adjust_datagramsize (ushort datagramsize,
					hwidbtype *hwidb)
{
    return(datagramsize + hwidb->encsize + sizeof(ulong) + smds_pad);
}


/*
 * Copy a packet in a receive window in the input interface
 * to a transmit window in an output interface.  This is
 * how off-complex bridging is done.
 *
 *
 * RE-SELECTS THE RX OFFSET.
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_to_mci_inline (hwidbtype *inputhw,
				hwidbtype *outputhw,
				ushort rx_offset,
				ushort bytes)

{
    mciregtype *srcreg = inputhw->mci_regptr;
    mciregtype *dstreg = outputhw->mci_regptr;

    /*
     * Note that for robustness, we explicitly select the rx offset
     * rather than making any assumptions about where it was left.
     * The caller is responsible for selecting the tx offset.
     */

    srcreg->argreg = rx_offset;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    mci2mci(&srcreg->readlong, &dstreg->write1long, bytes);
}

/*
 * Copy a packet from a network buffer to a transmit
 * window in an output interface.  This is flooding
 * between like-media when the presence or content
 * of an LLC header is of no consequence to the
 * translation.
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_datagram_wr_inline (hwidbtype *hwidb,
				     paktype *pak,
				     ushort datagramsize)

{
    mciregtype *mcireg = hwidb->mci_regptr;

    buffer2mci(pak->mac_start, &mcireg->write1long, datagramsize);
}

/*
 * Copy the LLC and network-layer portion of a packet
 * from a network buffer to a transmit window in an
 * output interface.  This is flooding between like
 * media when we have had to rewrite the MAC header
 * for some reason and need to continue the translation
 * with the LLC header.
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_llcdata_wr_inline (hwidbtype *hwidb,
				    paktype *pak)

{
    mciregtype *mcireg = hwidb->mci_regptr;

    buffer2mci(pak->info_start, &mcireg->write1long, pak->datagramsize -
	       (pak->info_start - pak->mac_start));
}

/*
 * Copy just the network-layer portion of a packet from
 * a network buffer to a transmit window in an output
 * interface.  This is flooding between unlike media.

 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_netdata_wr_inline (hwidbtype *hwidb,
				    paktype *pak)

{
    mciregtype *mcireg = hwidb->mci_regptr;

    buffer2mci(pak->network_start, &mcireg->write1long, pak->datagramsize -
	       (pak->network_start - pak->mac_start));
}



/*
 * Transmit a packet on the given interface.
 *
 * ASSUMES THE TX OFFSET HAS BEEN PRE-SELECTED.
 */

static inline
void tbridge_mci_tx1_inline (hwidbtype *hwidb,
			     ushort dgsize)

{
    mciregtype *mcireg = hwidb->mci_regptr;

    mcireg->argreg = dgsize;
    mcireg->cmdreg = MCI_CMD_TX1_START;
}

/*
 * Initiate cBus-assisted flooding to those interfaces
 * configured for autonomous bridging.
 */

static inline
void tbridge_mci_auto_flood_inline (hwidbtype *hwidb,
				    ushort dgsize)

{
    mciregtype *mcireg = hwidb->mci_regptr;
    int response;

    mcireg->argreg = dgsize;
    mcireg->cmdreg = FCI_BRIDGE_AUTO_FLOOD;

    response = mcireg->cmdreg;
    if (response != MCI_RSP_OKAY) {
	buginf("\ntbridge_mci_auto_flood_inline failed to flood: %x",
	       response);
    }
}

/*
 * Dump a received packet.
 */

static inline
void tbridge_mci_flush_datagram_inline (hwidbtype *hwidb)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    /* make sure we SELECT this i/f */
    mci_regptr->argreg = hwidb->mci_index;
    mci_regptr->cmdreg = MCI_CMD_SELECT;

    mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
}


/*
 * Copy a received packet from the input interface into a network
 * buffer.  Note that we are NOT copying encap_pak; it contains
 * only the MAC header of the incoming packet.  We will use a
 * couple of attributes of encap_pak (datagramsize and info_start)
 * and some of the contents (LSAP and CTL_OUI) to guide the copying,
 * but the contents of encap_pak themselves are not moved.
 *
 * For all encapsulations except SNAP, we align the copy so that
 * the MAC header ends at the end of the encapsulation area
 * (pak_center a.k.a network_start) where the LLC or data begin.
 * For SNAP encapsulations, we align the copy so that the entire
 * SNAP header, including LLC and type, ends at the end of the
 * encapsulation area where the data begins.  There's a very good
 * reason for this arrangement.
 *
 * When we translate this packet for different media, we will
 * translate everything up to network_start into a new encapsulation,
 * but beyond that, nothing will change.  Translational bridging
 * can be restricted to creating a new encapsulation from the old
 * one, and then appending everything from network_start onwards
 * in a uniform way.  The number of bytes beyond network_start will
 * always be (datagramsize - (network_start - mac_start)).
 *
 * For each type of input media, we set mac_start so that
 * the copy will align as described above, and then we read in the
 * whole enchilada.
 *
 */

static inline
boolean tbridge_mci_copy_rx_pak_inline (paktype *encap_pak,
					paktype *pak_copy)
{
    mciregtype *mcireg;
    ushort dgsize, llcsize;

    hwidbtype *inputhw = encap_pak->if_input->hwptr;

    pak_copy->if_input = encap_pak->if_input;
    pak_copy->mac_start = pak_copy->network_start = pak_center(pak_copy);

    pak_copy->datagramsize = dgsize = encap_pak->datagramsize;
    pak_copy->linktype = LINK_BRIDGE;

    switch (tbridge_dlli.src_media_type) {

    case TBR_ETHER:
    case TBR_ETHER_ISL_VLAN:
	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;


	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on ETHER.  We actually treat SNAP
	 * just like 802.3 in this case.
	 */
	pak_copy->datagramstart = pak_copy->mac_start -= ETHER_ARPA_ENCAPBYTES;
	llcsize = dgsize - ETHER_ARPA_ENCAPBYTES;
	pak_copy->addr_start = pak_copy->mac_start;
	if (encap_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXETHERSIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXETHERSIZE, encap_pak);
	    return(FALSE);
	}
	mci2buffer(&mcireg->readlong, pak_copy->mac_start, dgsize);
	break;

    case TBR_ETHER_SDE:
	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on ETHER.  We actually treat SNAP
	 * just like 802.3 in this case.
	 *
	 * LBJ, encap_pak->info_start better not be NULL!
	 */
        pak_copy->datagramstart = pak_copy->mac_start -= 
	    (ETHER_ARPA_ENCAPBYTES + SDE_ENCAPBYTES);

	pak_copy->info_start = 
	    ((uchar *)&((ether_hdr *) pak_copy->mac_start)->data) + 
		SDE_ENCAPBYTES;

        pak_copy->addr_start = pak_copy->mac_start;

        mci2buffer(&mcireg->readlong, pak_copy->mac_start, dgsize);
        break;

    case TBR_FDDI: {
	snap_hdr *link = (snap_hdr *)encap_pak->info_start;

	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

	if ((link) &&
	    (GETSHORT(&link->dsap) == SNAPSNAP) &&
            (GETLONG(&link->control) == SNAP_CTL_OUI)) {
	    pak_copy->datagramstart = pak_copy->mac_start -=
		FDDI_SNAP_ENCAPBYTES;
	    llcsize = dgsize - FDDI_SNAP_ENCAPBYTES;
	} else {
	    pak_copy->datagramstart = pak_copy->mac_start -= FDDI_ENCAPBYTES;
	    llcsize = dgsize - FDDI_ENCAPBYTES;
	}

	pak_copy->addr_start = pak_copy->mac_start + FDDI_FC;
	if (encap_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((fddi_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXFDDISIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXFDDISIZE, encap_pak);
	    return(FALSE);
	}
	/* An extra byte before FC is included to maintain word alignment */
	mci2buffer(&mcireg->readlong, pak_copy->mac_start - 1, dgsize + 1);

	/*
	 * if this fddi packet is coming from a virtual fddi, i.e.,
	 * atm or frame relay, we need to flip the mac addrs since
	 * it came over the link in non-canonical order.  however we
	 * deal we all fddi frames assuming the mac addrs are in
	 * canonical order.  remember, the fddi hw on the high end
	 * does this neat flipping for us, so we don't worry about
	 * it in thoses cases.  these virtual fddi links, we do...
	 *
	 * for native fddi interfaces, tbridge_dlli.circuit_id is
	 * always 0.  hopefully, frame relay sets this as well.
	 * are you listening, wmay?
	 */
	if (tbridge_dlli.circuit_id) {
	    fddi_hdr *fddi;

	    fddi = (fddi_hdr *) pak_copy->mac_start;
	    ieee_swap(fddi->daddr, fddi->daddr);
	    ieee_swap(fddi->saddr, fddi->saddr);
	}

	break;
    }

    case TBR_FDDI_SDE: {
	/*
	 * LBJ, encap_pak->info_start better not be NULL!
	 */
        snap_hdr *link = (snap_hdr *)encap_pak->info_start;

	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

        if ((GETSHORT(&link->dsap) == SNAPSNAP) &&
            (GETLONG(&link->control) == SNAP_CTL_OUI)) {
            pak_copy->datagramstart = pak_copy->mac_start -=
                (FDDI_SNAP_ENCAPBYTES + SDE_ENCAPBYTES);
        } else {
            pak_copy->datagramstart = pak_copy->mac_start -= 
		(FDDI_ENCAPBYTES + SDE_ENCAPBYTES);
        }

        pak_copy->addr_start = pak_copy->mac_start + FDDI_FC;

	pak_copy->info_start = 
	    ((uchar *)&((fddi_hdr *) pak_copy->mac_start)->data) +
		SDE_ENCAPBYTES;

        /* An extra byte before FC is included to maintain word alignment */
        mci2buffer(&mcireg->readlong, pak_copy->mac_start - 1, dgsize + 1);
        break;
    }

    case TBR_CTR: {
	tring_hdr *tring;
	snap_hdr *link = (snap_hdr *)encap_pak->info_start;

	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

	if ((link) &&
	    (GETSHORT(&link->dsap) == SNAPSNAP) &&
/*
 * Old style abuse of SNAP-F8.  Should compare to SNAP_CTL_OUI.
 */
            (GETLONG(&link->control) == inputhw->tr_transit_oui)) {
	    pak_copy->datagramstart = pak_copy->mac_start -=
		TRING_SNAP_ENCAPBYTES;
	    llcsize = dgsize - TRING_SNAP_ENCAPBYTES;
	} else {
	    pak_copy->datagramstart = pak_copy->mac_start -= TRING_ENCAPBYTES;
	    llcsize = dgsize - TRING_ENCAPBYTES;
	}

	pak_copy->addr_start = pak_copy->mac_start + TR_ACFC_SIZE;
	if (encap_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((tring_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXTRSIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXTRSIZE, encap_pak);
	    return(FALSE);
	}
	mci2buffer(&mcireg->readlong, pak_copy->mac_start, dgsize);

	tring = (tring_hdr *) pak_copy->mac_start;
	ieee_swap(tring->daddr, tring->daddr);
	ieee_swap(tring->saddr, tring->saddr);

	break;
    }

    case TBR_CTR_SDE: {
	/*
	 * LBJ, encap_pak->info_start better not be NULL!
	 */
        tring_hdr *tring;
        snap_hdr *link = (snap_hdr *)encap_pak->info_start;

	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

        if ((GETSHORT(&link->dsap) == SNAPSNAP) &&
/*
 * Old style abuse of SNAP-F8.  Should compare to SNAP_CTL_OUI.
 */
            (GETLONG(&link->control) == inputhw->tr_transit_oui)) {
            pak_copy->datagramstart = pak_copy->mac_start -=
                (TRING_SNAP_ENCAPBYTES + SDE_ENCAPBYTES);
        } else {
            pak_copy->datagramstart = pak_copy->mac_start -= 
		(TRING_ENCAPBYTES + SDE_ENCAPBYTES);
        }

        pak_copy->addr_start = pak_copy->mac_start + TR_ACFC_SIZE;

	pak_copy->info_start = 
	    ((uchar *)&((tring_hdr *) pak_copy->mac_start)->data) +
		SDE_ENCAPBYTES;

        mci2buffer(&mcireg->readlong, pak_copy->mac_start, dgsize);

        tring = (tring_hdr *) pak_copy->mac_start;
        ieee_swap(tring->daddr, tring->daddr);
        ieee_swap(tring->saddr, tring->saddr);

        break;
    }

    case TBR_FDDI_TRANSIT:
	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on FDDI-transit.  We actually treat SNAP
	 * just like 802.3 in this case.
	 */
	pak_copy->mac_start -= ETHER_ARPA_ENCAPBYTES;
	pak_copy->datagramstart = pak_copy->mac_start - FDDI_SNAP_ENCAPBYTES;
	llcsize = dgsize - (ETHER_ARPA_ENCAPBYTES + FDDI_SNAP_ENCAPBYTES);
	pak_copy->addr_start = pak_copy->mac_start;
	if (encap_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXFDDISIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXFDDISIZE, encap_pak);
	    return(FALSE);
	}
	/* An extra byte before FC is included to maintain word alignment */
	mci2buffer(&mcireg->readlong, pak_copy->datagramstart - 1, 
		   dgsize + 1);
	break;

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:
	/*
	 * Select the beginning of the received packet in the input interface:
	 * mac_offset here doesn't include the damn hdlc header.  i dunno
	 * why for lex and hdlc, the copy needs this stupid hdlc header.
	 * for this stupid reason, selecting the rx window can't be outside
	 * of this switch statement.
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset -
	    tbridge_dlli.transit_encapsize/2;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on HDLC-transit.  We actually treat SNAP
	 * just like 802.3 in this case.
	 */
	pak_copy->mac_start -= ETHER_ARPA_ENCAPBYTES;
	pak_copy->datagramstart = pak_copy->mac_start -
	    tbridge_dlli.transit_encapsize;
	llcsize = dgsize - (ETHER_ARPA_ENCAPBYTES +
			    tbridge_dlli.transit_encapsize);
	pak_copy->addr_start = pak_copy->mac_start;
	if (encap_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > inputhw->maxmtu) {
	    tbridge_log_giant(inputhw, llcsize, inputhw->maxmtu, encap_pak);
	    return(FALSE);
	}
	mci2buffer(&mcireg->readlong, pak_copy->datagramstart, dgsize);
	break;

    case TBR_HDLC_TRANSIT_SDE:
	/*
	 * Select the beginning of the received packet in the input interface:
	 */
	mcireg = inputhw->mci_regptr;
	mcireg->argreg = tbridge_dlli.mac_offset;
	mcireg->cmdreg = MCI_CMD_RX_SELECT;

        /*
         * This copy works only because we are making the assumption
         * that ARPA and 802.3 encapsulations are the same length, and
         * we do not expect SNAP on HDLC-transit.  We actually treat SNAP
         * just like 802.3 in this case.
	 *
	 * LBJ, encap_pak->info_start better not be NULL!
         */

        pak_copy->mac_start -= (ETHER_ARPA_ENCAPBYTES + SDE_ENCAPBYTES);
        pak_copy->datagramstart = pak_copy->mac_start -
            tbridge_dlli.transit_encapsize;

	pak_copy->info_start = 
	    ((uchar *)&((ether_hdr *) pak_copy->mac_start)->data) +
		SDE_ENCAPBYTES;

        pak_copy->addr_start = pak_copy->mac_start;

        mci2buffer(&mcireg->readlong, pak_copy->datagramstart, dgsize);
        break;

    default:
	/*
	 * "This should never happen."
	 */
	return(FALSE);
	break;
    }

    return(TRUE);
}


/*
 * tbridge_cbus_copy_rx_data_inline()
 *
 * Copy spcific-length data from MCI/cBus RX pointer to TX pointer,
 * starting from an offset.
 * This is on-complex, cbus_bcopy() assistance copy.
 */

static inline
void tbridge_cbus_copy_rx_data_inline (hwidbtype *hwidb,
					ushort rx_offset,
					ushort length)

{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->rx_select = rx_offset;
    cbus_bcopy(mci_regptr, length);
}


/*
 * figure out what kind of media is encapsulated in the atm
 * packet and return TRUE if a recognized packet type...
 *
 * atm encaps (after the vc number & enctype) is rfc1483 header
 * before the mac packet.  it looks like this...
 *
 *       AA AA 03
 *       00 80 c2
 *       00 0X
 * where X is:
 *
 *  1 - ethernet w/ fcs included
 *  7 - ethernet w/o fcs included
 *  3 - token ring w/ fcs included
 *  9 - token ring w/o fcs included
 *  4 - fddi w/ fcs included
 *  A - fddi w/o fcs included
 *
 *  next line is either
 *       00 00    for ethernet or
 *       00 00 ?? for token ring and fddi
 */
static inline
boolean tbridge_mci_atm_media_classify (hwidbtype *hwidb,
					idbtype *swidb)
{
    /*
     * MCI/cBus transparent bridging is driven from a copy of the
     * received packet's MAC header in tbridge_encap_pak.  Dress
     * up this copy with all the important attributes of the real
     * thing.  Copy only the MAC header from the rx window to the
     * copy.
     */
    tbridge_encap_pak->if_input = swidb;

    tbridge_dlli.circuit_id = hwidb->sniff[SNIFFBASE+0].d.sword[0];
    tbridge_dlli.packet_pid = hwidb->sniff[SNIFFBASE+2].d.sword[1];

    /*
     * the type of lan is already sniffed into the sniff buffer
     * we can always read it again but let's just get it from
     * memory.
     */
    switch (tbridge_dlli.packet_pid) {
    case TBRIDGE_PID_8023FCS:
	tbridge_encap_pak->datagramsize -= FCS8023_SIZE;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_8023:
	tbridge_dlli.src_media_type = TBR_ETHER;
	tbridge_dlli.da_offset = hwidb->rxoffset + ATM_BRIDGE_ETHER_ENCAPWORDS;
	tbridge_dlli.mac_offset = tbridge_dlli.da_offset;
	tbridge_dlli.transit_encapsize = ATM_BRIDGE_ETHER_ENCAPBYTES;

	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap);
	tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
	    ATM_BRIDGE_ETHER_ENCAPBYTES;

	tbridge_encap_pak->datagramsize -= ATM_BRIDGE_ETHER_ENCAPBYTES;
	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_YES);

	break;

    case TBRIDGE_PID_8025FCS:
	tbridge_encap_pak->datagramsize -= TR_FCS_SIZE;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_8025:
	tbridge_dlli.src_media_type = TBR_CTR;
	tbridge_dlli.da_offset = hwidb->rxoffset + ATM_BRIDGE_OTHER_ENCAPWORDS;
	tbridge_dlli.mac_offset = hwidb->rxoffset + ATM_BRIDGE_ETHER_ENCAPWORDS;
	tbridge_dlli.transit_encapsize = ATM_BRIDGE_OTHER_ENCAPBYTES;

	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) -
	    TR_ACFC_SIZE;
	tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
	    ATM_BRIDGE_ETHER_ENCAPBYTES;

	tbridge_encap_pak->datagramsize -= ATM_BRIDGE_ETHER_ENCAPBYTES;
	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_NOT);
	/*
	 * MAC addresses are assumed to be in non-canonical order.
	 * it's just like getting a packet from a token ring.
	 */
	tbridge_mci_swap_mac_address_inline();

	break;

    case TBRIDGE_PID_FDDIFCS:
	tbridge_encap_pak->datagramsize -= FDDI_FCS;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_FDDI:
	tbridge_dlli.src_media_type = TBR_FDDI;
	tbridge_dlli.da_offset = hwidb->rxoffset + ATM_BRIDGE_OTHER_ENCAPWORDS;
	tbridge_dlli.mac_offset = hwidb->rxoffset + ATM_BRIDGE_ETHER_ENCAPWORDS;
	tbridge_dlli.transit_encapsize = ATM_BRIDGE_OTHER_ENCAPBYTES;

	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) - FDDI_FC;
	tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
	    ATM_BRIDGE_OTHER_ENCAPBYTES - FDDI_FC;

	tbridge_encap_pak->datagramsize -= (ATM_BRIDGE_ETHER_ENCAPBYTES + 1);
	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_NOT);

	/*
	 * MAC addresses are assumed to be in non-canonical order.
	 * it's just like getting a packet from a token ring.
	 */
	tbridge_mci_swap_mac_address_inline();

	break;
    case TBRIDGE_PID_BPDU:
        return (FALSE);     /* This kicks spanning bpdu to process level */

    default:
	/*
	 * unknown type
	 */
	if (atm_errors_debug)
	    buginf("\nATM(%s): encap type %d not supported",
		   hwidb->hw_namestring, hwidb->sniff[SNIFFBASE+2].d.sword[1]);
	return (FALSE);
    }

    return(TRUE);
}

/*
 * When packet is received on the lane interface, set up the tbridge_dlli
 * structure for later use. Closely follows what is done above for 1483.
 * Key diff is that the incoming packet can only be of type Ether 
 * for LANE. 
 */
static inline
boolean tbridge_mci_lane_ether_classify (hwidbtype *hwidb,
					idbtype *swidb)
{
    /*
     * MCI/cBus transparent bridging is driven from a copy of the
     * received packet's MAC header in tbridge_encap_pak.  Dress
     * up this copy with all the important attributes of the real
     * thing.  Copy only the MAC header from the rx window to the
     * copy.
     */

    tbridge_dlli.circuit_id = hwidb->sniff[SNIFFBASE+0].d.sword[0];

    tbridge_dlli.packet_pid = 0;

    /* 
     * Set the media type based on what type of LAn we are emulating
     * on this interface (ethernet or Tokenring.
     */
    tbridge_dlli.src_media_type = TBR_ETHER;

    tbridge_dlli.transit_encapsize =  ATM_AAL5_LANE_ENCAPBYTES;

    tbridge_dlli.da_offset = ( hwidb->rxoffset +
                               ATM_AAL5_LANE_ENCAPWORDS);
			      
    tbridge_dlli.mac_offset = tbridge_dlli.da_offset;

    tbridge_encap_pak->mac_start = (uchar *) tbridge_encap;
    tbridge_encap_pak->datagramstart = ( tbridge_encap_pak->mac_start -
	                                tbridge_dlli.transit_encapsize ); 

    /* Datagramsize excludes the ATM LANE encapsulation */
    tbridge_encap_pak->datagramsize -= tbridge_dlli.transit_encapsize;
    tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
				       LENGTH_802_YES);
    return(TRUE);
}

static inline
boolean tbridge_mci_frame_relay_classify (idbtype *swidb)
{
    hwidbtype *hwidb;
    ushort length;

    hwidb = swidb->hwptr;
    length = hwidb->rxlength;

    /*
     * MCI/cBus transparent bridging is driven from a copy of the
     * received packet's MAC header in tbridge_encap_pak.  Dress
     * up this copy with all the important attributes of the real
     * thing.  Copy only the MAC header from the rx window to the
     * copy.
     */
    tbridge_encap_pak->if_input = swidb;
    /*
     * Saved this in fr_fast_pvc_switch_or_fs
     */
    tbridge_dlli.circuit_id = hwidb->sniff[SNIFFBASE+3].d.sword[0];
    tbridge_dlli.packet_pid = hwidb->sniff[SNIFFBASE+2].d.sword[0];
    /*
     * the type of lan is already sniffed into the sniff buffer
     * we can always read it again but let's just get it from
     * memory.
     */
    if (hwidb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_BRIDGE) {
	/* cisco frame relay bridging */
	tbridge_dlli.src_media_type = TBR_ETHER;
	tbridge_dlli.da_offset = tbridge_dlli.mac_offset =
	    hwidb->rxoffset + btow(hwidb->encsize);
	tbridge_dlli.transit_encapsize = hwidb->encsize;

	tbridge_encap_pak->mac_start = ((uchar *)tbridge_encap);
	tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
	    tbridge_dlli.transit_encapsize;
	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_YES);

	tbridge_encap_pak->datagramsize = length -
	    tbridge_dlli.transit_encapsize;
    } else {
	/* IETF style bridging */
	tbridge_dlli.mac_offset = hwidb->rxoffset +
	    btow(FRAME_RELAY_SNAPENCAPBYTES - FRAME_RELAY_ENCAPBYTES) +
		btow(hwidb->encsize);
	tbridge_dlli.transit_encapsize = FRAME_RELAY_SNAPENCAPBYTES -
	    FRAME_RELAY_ENCAPBYTES + hwidb->encsize;
	
	switch (tbridge_dlli.packet_pid) {
	case FR1490_ETHER_FCS:
	    length -= FCS8023_SIZE;
	    /*
	     * fall through
	     */
	case FR1490_ETHER:
	    tbridge_dlli.src_media_type = TBR_ETHER;
	    tbridge_dlli.da_offset = tbridge_dlli.mac_offset;
	
	    tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap);
	    tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
		tbridge_dlli.transit_encapsize;

	    tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					       LENGTH_802_YES);
	    
	    tbridge_encap_pak->datagramsize = length -
		tbridge_dlli.transit_encapsize;
	    break;

	case FR1490_802_5_FCS:
	    length -= TR_FCS_SIZE;
	    /*
	     * fall through
	     */
	case FR1490_802_5:
	    tbridge_dlli.src_media_type = TBR_CTR;

	    /* 1 more word for the da for token ring */
	    tbridge_dlli.da_offset = tbridge_dlli.mac_offset +
		btow(TR_ACFC_SIZE);

	    /*
	     * the AC byte really doesn't count when it comes to rfc
	     * 1490.  we just care about the FC byte.  so just use
	     * FDDI_FC because we don't have such a thing defined for TR.
	     */
	    tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) -
		TR_ACFC_SIZE;
	    tbridge_encap_pak->datagramstart =
		tbridge_encap_pak->mac_start - tbridge_dlli.transit_encapsize;

	    tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					       LENGTH_802_NOT);
	    /*
	     * MAC addresses are assumed to be in non-canonical order.
	     * it's just like getting a packet from a token ring.
	     */
	    tbridge_mci_swap_mac_address_inline();
	    tbridge_encap_pak->datagramsize = length -
		tbridge_dlli.transit_encapsize;
	    break;

	case FR1490_FDDI_FCS:
	    length -= FDDI_FCS;
	    /*
	     * fall through
	     */
	case FR1490_FDDI:
	    tbridge_dlli.src_media_type = TBR_FDDI;
	    /* add 1 word in */
	    tbridge_dlli.da_offset = tbridge_dlli.mac_offset + 1;
	    /* add extra byte for FDDI PAD */
	    tbridge_dlli.transit_encapsize += FR1490_FDDI_PADBYTES;

	    tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) - FDDI_FC;
	    tbridge_encap_pak->datagramstart =
		tbridge_encap_pak->mac_start -
		    (FRAME_RELAY_SNAPENCAPBYTES + FR1490_FDDI_PADBYTES);

	    tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					       LENGTH_802_NOT);

	    /*
	     * MAC addresses are assumed to be in non-canonical order.
	     * it's just like getting a packet from a token ring.
	     */
	    tbridge_mci_swap_mac_address_inline();
	    tbridge_encap_pak->datagramsize = length -
		tbridge_dlli.transit_encapsize;
	    break;

	default:
	    /*
	     * unknown type (spanning tree)
	     */
	    return (FALSE);
	}
    }

    return(TRUE);
}

static inline
boolean tbridge_mci_smds_classify (idbtype *swidb)
{
    hwidbtype *hwidb;
    ushort length;
    mciregtype *regptr;
    ushort offset;

    hwidb = swidb->hwptr;
    length = hwidb->rxlength;
    /*
     * MCI/cBus transparent bridging is driven from a copy of the
     * received packet's MAC header in tbridge_encap_pak.  Dress
     * up this copy with all the important attributes of the real
     * thing.  Copy only the MAC header from the rx window to the
     * copy.
     */
    /*
     * Saved this in the cbus interrupt routine
     */
    tbridge_dlli.packet_pid = hwidb->sniff[SNIFFBASE+0].d.sword[0];
    tbridge_dlli.smdssrc_ptr = tbridge_dlli.smdssrc;
    /*
     * the type of lan is already sniffed into the sniff buffer
     * we can always read it again but let's just get it from
     * memory.
     */
    tbridge_dlli.mac_offset = hwidb->rxoffset + btow(hwidb->encsize) +
	btow(SMDS_BRIDGE_PAD_SIZE);
    tbridge_dlli.transit_encapsize = hwidb->encsize + SMDS_BRIDGE_PAD_SIZE;
    regptr = hwidb->mci_regptr;
    /*
     * we want to point at the smds dest address, so we can read in that,
     * the smds source, and the length of the pad
     */
    offset = hwidb->rxoffset + btow(hwidb->smds_pdb->fast_input_modelen) +
	btow(sizeof(ulong) + STATIONLEN_SMDS64);
    regptr->argreg = offset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    /*
     * read in the source address and the pad size
     */
    *(ulong *)tbridge_dlli.smdssrc = regptr->readlong;
    *(ulong *)&tbridge_dlli.smdssrc[4] = regptr->readlong;
    offset = regptr->readshort;
    /*
     * Figure out the real swidb
     */
    tbridge_encap_pak->if_input = hwidb->fast_rcvidb;
    if (tbridge_encap_pak->if_input == NULL)
	return(FALSE);
    /*
     * compute the datagramsize of the bridged packet.  This is
     * total size - encap size - pad - trailer - bridge_pad size
     */
    length -= hwidb->encsize + ((offset >> 8) & SMDS_XHLPI_PL) +
	sizeof(ulong) + SMDS_BRIDGE_PAD_SIZE;
    switch (tbridge_dlli.packet_pid) {
    case SMDS_BRIDGE_PID_8023FCS:
	length -= FCS8023_SIZE;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_8023:
	tbridge_dlli.src_media_type = TBR_ETHER;
	tbridge_dlli.da_offset = tbridge_dlli.mac_offset;
	
	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap);
	tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
	    tbridge_dlli.transit_encapsize;

	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_YES);
	    
	tbridge_encap_pak->datagramsize = length;
	break;

    case SMDS_BRIDGE_PID_8025FCS:
	length -= TR_FCS_SIZE;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_8025:
	tbridge_dlli.src_media_type = TBR_CTR;

	/* 1 more word for the da for token ring */
	tbridge_dlli.da_offset = tbridge_dlli.mac_offset +
	    btow(TR_ACFC_SIZE);

	/*
	 * the AC byte really doesn't count when it comes to rfc
	 * 1490.  we just care about the FC byte.  so just use
	 * FDDI_FC because we don't have such a thing defined for TR.
	 */
	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) -
	    TR_ACFC_SIZE;
	tbridge_encap_pak->datagramstart =
	    tbridge_encap_pak->mac_start - tbridge_dlli.transit_encapsize;

	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_NOT);
	/*
	 * MAC addresses are assumed to be in non-canonical order.
	 * it's just like getting a packet from a token ring.
	 */
	tbridge_mci_swap_mac_address_inline();
	tbridge_encap_pak->datagramsize = length;
	break;

    case SMDS_BRIDGE_PID_FDDIFCS:
	length -= FDDI_FCS;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_FDDI:
	tbridge_dlli.src_media_type = TBR_FDDI;
	/* add 1 word in */
	tbridge_dlli.da_offset = tbridge_dlli.mac_offset + 1;
	/* extra byte for FDDI PAD */
	length -= SMDS_BRIDGE_PAD_FDDI_EXTRA;
	tbridge_dlli.transit_encapsize += SMDS_BRIDGE_PAD_FDDI_EXTRA;

	tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) - FDDI_FC;
	tbridge_encap_pak->datagramstart =
	    tbridge_encap_pak->mac_start - tbridge_dlli.transit_encapsize;

	tbridge_mci_read_mac_header_inline(hwidb, tbridge_encap_pak,
					   LENGTH_802_NOT);

	/*
	 * MAC addresses are assumed to be in non-canonical order.
	 * it's just like getting a packet from a token ring.
	 */
	tbridge_mci_swap_mac_address_inline();
	tbridge_encap_pak->datagramsize = length;
	break;

    default:
	/*
	 * unknown type (spanning tree)
	 */
	return (FALSE);
    }
 
    return(TRUE);
}
static inline
void
add_fddi_hardware_address_filter_inline (hwidbtype *inputhw, uchar *mac_addr)
{
   if ((inputhw->fddi_cam_sa_miss == FDDIT_CAM_VALID) || (inputhw->status & IDB_VIP))
    reg_invoke_media_add_hardware_address_filter(inputhw, mac_addr );
}

#endif /* __TBRIDGE_MCI_INLINE_H__ */
