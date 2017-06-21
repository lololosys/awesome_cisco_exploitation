/* $Id: tbridge_tlb_inline.h,v 3.10.4.6 1996/08/13 02:25:23 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_tlb_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_tlb_inline.h
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Generic in-line functions for translational bridging.
 *------------------------------------------------------------------
 * $Log: tbridge_tlb_inline.h,v $
 * Revision 3.10.4.6  1996/08/13  02:25:23  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.4.5  1996/06/18  19:14:54  rbadri
 * CSCdi60067:  IPX rip TR->TR through bridged ELAN works intermittently.
 * Branch: California_branch
 * Set pak->addr_start properly before using it to send LE_ARP requests.
 * Clean up a little on the LEC side.
 *
 * Revision 3.10.4.4  1996/05/04  05:09:23  wilber
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
 * Revision 3.10.4.3  1996/04/29  17:04:32  rramacha
 * CSCdi45937:  pings fail in rtbr over fsip(smds) ip proto dec bridge
 * Branch: California_branch
 *
 * Revision 3.10.4.2  1996/04/09  22:16:04  fsunaval
 * CSCdi53802:  Transparent bridging across PAs is broke again.
 * Branch: California_branch
 *
 * Revision 3.10.4.1  1996/03/18  22:12:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.4  1996/03/16  07:39:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.3  1996/03/13  02:03:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.2  1996/03/07  10:52:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  18:53:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/09  02:20:49  wilber
 * CSCdi49875:  TB PPP to FDDI truncates packets larger than 1523 bytes
 * Shouldn't get the packet size from the 'length' field of 802.3 if it is
 * zero; We use this trick to carry an ether-like packet > ETHER MTU over
 * serial
 *
 * Revision 3.9  1996/02/17  01:39:20  hou
 * CSCdi48204:  bridge takes 5 minutes to function properly with fddi
 *              encaps
 * - tbridge_to_fddi_transit_inline()
 *   shoud not swap bits of the dst address if it is a proxy multicast
 *   address.
 *
 * Revision 3.8  1996/02/16  02:08:00  pitargue
 * CSCdi49151:  ctr <-> atm <-> ctr flooding not using correct transit oui
 * use the ctl/oui of the ctr hwidb rather than the atm's, which really
 * doesn't exists.
 *
 * Revision 3.7  1996/02/12  04:52:13  amajumda
 * CSCdi48155:  problems with bridging on smds subinterfaces
 *
 * Revision 3.6  1996/02/05  10:29:28  amajumda
 * CSCdi48155:  problems with bridging on smds subinterfaces. Fix to pick
 * the
 * correct smds subinterface address swidb->vaddr->addr for the smds
 * source
 * address as opposed to spd->smds_hardware.
 *
 * Revision 3.5  1996/02/05  03:35:23  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.4  1995/12/30  00:30:09  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.3  1995/11/20  15:44:06  pitargue
 * CSCdi43833:  flooding over multiple atm vcs from fddi results in
 * bitswapped macs
 * don't bitswap original packet, but a copy for the cbus fddi flood case
 *
 * Revision 3.2  1995/11/17  18:45:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:31:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/21  19:15:11  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * recode congestion cases in tbridge_forward_inline and
 * tbridge_flood_inline.
 * These did not adequately handle MTU oversize issues and did not
 * correctly
 * count buffers on multi-VC cases like Frame relay.
 *
 * Revision 2.4  1995/08/12  07:42:40  fred
 * CSCdi38525:  rewrite fair queue conversation discriminator using IEEE
 *              definitions
 *
 * Revision 2.3  1995/08/07  04:02:42  wilber
 * CSCdi38151:  802.10 encapsulation not working, when sending packets to
 * serial i/f
 * For SDE(802.10)-encap serial, should backoff pak->datagramstart instead
 * of pak->info_start to laydown serial encap(low-end box specific)
 *
 * Revision 2.2  1995/06/10  02:26:17  wilber
 * CSCdi34297:  Trans Bridging failed from MultiBus token to cbus for SNAP
 * pkts
 * When the input is MultiBus token, we should copy data portion of the
 * packet from pak->info_start instead of pak->network_start unless we
 * want to strip the SNAP header to convert it to ARPA - otherwise we
 * drop the SNAP header
 *
 * Revision 2.1  1995/06/07  23:03:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *				OVERVIEW
 *
 * The group of functions in this file translate a packet to the
 * encapsulation required for transmission on the output interface.
 * The translation is a function of the type of output interface,
 * the type of the input interface, the type of packet memory in
 * which the original packet resides, and the type of packet memory
 * in which the translated packet will be constructed.
 *
 * The arguments to these functions are:
 *
 *  - the output interface to which the translation is to be made
 *  - the packet to be translated
 *  - a code switch to select the translation appropriate to the
 *    input and output packet memory
 *
 * If the translation was successful, the functions return TBRIDGE_TLB_DONE
 * and the caller should transmit the packet since transmit resources
 * will have been committed.
 *
 * If the translation fails, the functions return one of three
 * values indicating:
 *
 *  - there were no transmit resources
 *  - the translated packet would exceed the output interface's MTU
 *  - the translation is not supported
 *  - in the case of atm, the virtual circuit number is not known
 *
 * These functions do not transmit the packet.  They just translate it
 * into a network buffer (the low-end and RSP case), or into an MCI/cBus
 * tx window (the high-end case).  It is important to note that other
 * than in the TLB_BFR_TO_BFR case (the low-end and RSP case), the packet
 * is NOT modified.  Instead, its attributes drive the translation
 * process which results in a packet for transmission that resides in an
 * MCI/cBus tx window.  This is in contrast to the low-end and RSP case
 * in which the packet provided is translated onto itself, and two packet
 * attributes, datagramstart and datagramsize, are updated to reflect the
 * translated packet.  See tbridge_public.h for a full description of how
 * the packet should be prepared for this function.
 *
 * Well, I lied.  datagramsize is updated in all cases so that the
 * caller can do tidy tx byte count accounting.  This means that
 * the original datagramsize should be recorded by the caller and
 * restored across this function if the packet is to be re-used for
 * further translations as is the case with flooding.  This sucks
 * and a brilliant alternative will occur to me soon.
 *
 *			DETAILS, DETAILS, ...
 *
 * There is a translation function for each type of output interface,
 * and within each function, there is a case for each type of input
 * interface. Within each of these cases, there is a further set of
 * cases for translations between the different types of packet memory.
 * These sets of cases are intentionally coded without a default to
 * make it clear which cases are supported and which are not.  Some
 * cases such as he-fddi to he-fddi off-complex forwarding, while
 * supported are probably not practical.
 *
 * There are eight packet-memory cases and they are described in
 * tbridge_public.h where the enum TBRIDGE_TRANSLATE gets defined.
 *
 * LOW-END (AND RSP) TRANSPARENT/TRANSLATIONAL FORWARDING AND FLOODING
 *    "le-xxx to le-xxx fwd and fld"
 *    "le-xxx to le-yyy fwd and fld"
 * - nothing special, all done in place in the packet
 *
 * HIGH-END TRANSPARENT FORWARDING ON-COMPLEX
 *    "he-xxx to he-xxx fwd on cmplx"
 * - move the mci rx window to the mci tx window and you're done
 *
 * HIGH-END TRANSLATIONAL FORWARDING ON-COMPLEX
 *    "he-xxx to he-yyy fwd on cmplx"
 * - move the mci rx window to the mci tx window
 * - select the tx offset appropriate to the output interface
 * - write the new MAC header (may include SNAP)
 *
 * HIGH-END TRANSPARENT FORWARDING OFF-COMPLEX
 *    "he-xxx to he-xxx fwd off cmplx"
 *    "ctr to mtr fwd"
 *    "mtr to ctr fwd"
 *    "mtr to mtr fwd"
 * - reserve an mci tx window or mtr tx frame
 * - copy the COMPLETE packet from the mci rx window or mtr rx frame
 *   to the mci tx window or mtr tx frame
 * - for tring interfaces, scribble the ACFC word for good measure
 *
 * HIGH-END TRANSLATIONAL FORWARDING OFF-COMPLEX
 *    "he-xxx to he-yyy fwd off cmplx"
 *    "he-xxx to mtr fwd"
 *    "mtr to he-xxx fwd"
 * - reserve an mci tx window or mtr tx frame
 * - write the new MAC header (may include SNAP)
 * - copy the REMAINING data from the mci rx window or mtr rx frame
 *   to the mci tx window or mtr tx frame
 *
 * HIGH-END TRANSPARENT FLOODING
 *    "he-xxx to he-xxx fld"
 * - reserve an mci tx window or mtr tx frame
 * - copy the COMPLETE packet from the network buffer
 *   to the mci tx window or mtr tx frame
 *
 * HIGH-END TRANSLATIONAL FLOODING
 *    "he-xxx to he-yyy fld"
 *    "ctr to ctr fld"
 *    "ctr to mtr fld"
 *    "mtr to ctr fld"
 *    "mtr to mtr fld"
 * - reserve an mci tx window or mtr tx frame
 * - write the new MAC header (may include SNAP)
 * - copy the REMAINING data from the network buffer
 *   to the mci tx window
 * - note that tring-to-tring flooding is included here since
 *   we must rewrite the MAC header to swap the DA and SA
 *
 * The trick is to get all the offsets right.
 */

#ifndef __TBRIDGE_TLB_INLINE_H__
#define __TBRIDGE_TLB_INLINE_H__

/*
 * write RFC 1483 header
 * can't put this in tbridge_inline.h, because this file
 * is included before tbridge_inline.h
 */
static inline
void tbridge_write_1483_hdr_inline (paktype *pak, ushort vcnum, ushort pid)
{
    atm_bridge_other_hdr *atm = NULL;

    atm = (atm_bridge_other_hdr *)pak->datagramstart;

    /*
     * need this for the 4500 atmzer
     */
    pak->atm_vcid = vcnum;

    /*
     * these next 2 are not needed by the 4500.  but
     * needed for the rsp
     */
    PUTSHORT(&atm->encap.vcnum, vcnum);
    PUTSHORT(&atm->encap.enctype, (ATM_DM_PKT_AAL5 | ATM_DM_EOM_EN));

    /*
     * fill up llc and oui, but be fast about it.
     * fill up one long then a short.  the long
     * includes 1st byte of the oui
     */
    PUTLONG(&atm->llc, TBRIDGE_LLC_OUI_1ST_LONG);
    PUTSHORT(&atm->oui[1], TBRIDGE_OUI_LAST_SHORT);

    PUTSHORT(&atm->pid, pid);
    PUTSHORT(&atm->pad, 0);

    if (pid == TBRIDGE_PID_FDDI || pid == TBRIDGE_PID_8025)
	atm->pad2.pad_FC[0] = 0;
}

/*
 * Lane has only vcnum, enc_type and lecid in the header. Write them &
 * return.
 */
static inline
void tbridge_write_lane_ether_hdr_inline (paktype *pak,
					  ushort vcnum, 
					  ushort lecid)
{
    atm_aal5lane_t *atm;

    atm = (atm_aal5lane_t *)pak->datagramstart;

    /*
     * need this for the 4500 atmzer
     */
    pak->atm_vcid = vcnum;

    /*
     * these next 2 are not needed by the 4500.  but
     * needed for the rsp
     */
    PUTSHORT(&atm->encap.vcnum, vcnum);
    PUTSHORT(&atm->encap.enctype, (ATM_DM_PKT_AAL5 | ATM_DM_EOM_EN));

    /*
     * Write down the client id as part of the header
     */
    PUTSHORT(&atm->lecid,lecid );
}


/*
 * tbridge_write_1490_hdr_inline
 * Low end routine to write 1490 header
 * dlci should be fr_pvc->converted_dlci
 */
static inline
void tbridge_write_1490_hdr_inline (paktype *pak,
				    ushort dlci,
				    ushort pid)
{
    frame_relay_hdr *fr_hdr;
    snap_hdr *snap;

    fr_hdr = (frame_relay_hdr *)pak->datagramstart;
    PUTSHORT(&fr_hdr->dlci, dlci);
    PUTSHORT(&fr_hdr->type, FR_SNAP_NLPID);
    snap = (snap_hdr *)fr_hdr->snap_start;
    PUTSHORT(&snap->control, (FR_SNAP_CONTROL << 8) | 0);
    PUTSHORT(&snap->oui[1], FR_OUI23_BRIDGE);
    PUTSHORT(&snap->type, pid);
}

/*
 * tbridge_write_smds_inline
 * Write a smds header and trailer onto a packet.  The smds header
 * is best described somewhere else - look at the RFC if you're that
 * interested, or at the smdstype structure
 */
static inline
ushort tbridge_write_smds_inline (paktype *pak, hwidbtype *hwidb,
				  idbtype *swidb, uchar *smds_addr, 
				  ushort datagramsize, ushort pid)
{
    smdstype *hdr;
    smds_pdb_t *spd;
    ushort pad, basize;
    uchar *trail;
    snap_hdr *snap;

    trail = pak->datagramstart + datagramsize;

    datagramsize += SMDS_BRIDGE_PAD_SIZE;
    /* calculate the number of pad bytes needed to round the size to
     * modula 4
     */
    pad = 3 - ((datagramsize + 3) % 4);
    spd = hwidb->smds_pdb;
    pak->datagramstart -= (hwidb->encsize + SMDS_BRIDGE_PAD_SIZE);
    if (spd->dximode) {
	dxismds *ds = (dxismds *)pak->datagramstart;
	ds->dxi.addr = DXI_CMDTODSU;
	ds->dxi.control = DXI_UI;
	PUTSHORT(&ds->dxi.congest, 0);
	hdr = (smdstype *)(pak->datagramstart + SMDS_DXIBYTES);
    } else {
	PUTSHORT(pak->datagramstart, 0);
	hdr = (smdstype *)(pak->datagramstart + SMDS_CISCOBYTES);
    }
    if (pad) {
	/* add the pad bytes to the datagramsize */
	PUTLONG(trail, 0);
	trail += pad;
	datagramsize += pad;
    }
    /*
     * write the trailer at the same time as the header
     */
    *trail++ = hdr->smds_rsvd = 0;
    *trail++ = hdr->smds_betag = hwidb->smds_begin_tag++;
    /*
     * calculate the basize by adding the smds header and the snap header
     */
    basize = datagramsize + SMDS_HEADER + SNAP_HDRBYTES;
    /*
     * Continue writing the header - write the basize to the trailer at the
     * same time
     */
    PUTSHORT(&hdr->smds_basize, basize);
    PUTSHORT(trail, basize);

    /*
     * Finish up the smds header
     */
    bcopy(smds_addr, hdr->smdsdest, STATIONLEN_SMDS64);
    bcopy(swidb->vaddr ? swidb->vaddr->addr : spd->smds_hardware, hdr->smdssrc, STATIONLEN_SMDS64);
    hdr->smds_xhlpi = pad | SMDS_LLC;
    hdr->smds_xhel = XHEL_SIZE;
    PUTSHORT(&hdr->smds_x, 0);
    /* Hel field  is 12 bytes - first 3 are used, last 9 aren't */
    hdr->smds_hel[0] = SMDSHE_VERLEN;
    hdr->smds_hel[1] = SMDSHE_VERTYP;
    hdr->smds_hel[2] = SMDSHE_VERVAL;
    memset(&hdr->smds_hel[3], 0, 9);
    /*
     * Put the snap header and the pad bytes in
     */
    snap = (snap_hdr *)hdr->data;
    snap->dsap = SAP_SNAP;
    snap->ssap = SAP_SNAP;
    snap->control = LLC1_UI;
    snap->oui[0] = SMDS_BRIDGE_OUI0;
    snap->oui[1] = SMDS_BRIDGE_OUI1;
    snap->oui[2] = SMDS_BRIDGE_OUI2;
    PUTSHORT(&snap->type, pid);
    PUTSHORT(&snap->data, SMDS_BRIDGE_PAD_DATA);
    
    /* add the encapsulation size and the trailer length */
    datagramsize += hwidb->encsize + sizeof(ulong);
    return(datagramsize);
}


/*
 * tbridge_to_ether_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build an Ethernet frame.
 *
 * TLB_WITHIN_MCI, TLB_MCI_TO_MCI, and TLB_BFR_TO_MCI notes:
 * - check for all conditions that prevent forwarding and
 *   return FALSE for these BEFORE the MCI/cBus select
 *   operation occurs since this commits us to transmission.
 *
 * TLB_WITHIN_MCI notes:
 * - This function is also used to convert packets to ether
 *   before transit bridging them.  When this is done on-complex,
 *   the offset at which the MAC header starts must be available
 *   to the transit translation routines to be able to pre-pend
 *   the transit header.  pak->encsize is used in these cases
 *   in this function to record this offset for use in the
 *   transit translation functions.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_ether_inline (idbtype *outputsw, paktype *pak,
			 const enum TBRIDGE_TRANSLATE code_switch)

{
    idbtype *inputsw     = pak->if_input;
    hwidbtype *inputhw   = inputsw->hwptr;
    hwidbtype *outputhw  = outputsw->hwptr;
    ushort datagramsize  = pak->datagramsize;
    ushort sde_encapwords = 0;

    ether_hdr *ether;
    snap_hdr *snap;

    switch (tbridge_dlli.src_media_type) {

    case TBR_HDLC_TRANSIT_SDE:
    case TBR_ETHER_SDE:         /* ether-sde to ether */
	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = pak->encsize = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_FDDI_TRANSIT:	/* fddi-t to ether */
    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:	/* serial-t to ether */
	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
				   leaving interior Ethernet format packet */

	/*
	 * Fall through to the ETHER case.
	 */

    case TBR_ETHER:		/* ether to ether */
        /*
         * Don't bother to check if it is SNAP encapsulated.
         * SNAP on ether won't be translated to ARPA on ether to make
         * some applications happy.
         */

	/* Not really necessary from ether to ether */
	if (datagramsize > outputhw->max_pak_size)
	    return(TBRIDGE_TLB_EXCEEDS_MTU);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* le-ether(sde) to le-ether fwd and fld */
			     /* rsp-ether(sde) to rsp-ether fwd and fld */

	    pak->datagramstart = pak->mac_start;

	    if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
		(TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
                ether = (ether_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);
                ieee_move(((ether_hdr *)pak->mac_start)->saddr, ether->saddr);
                ieee_move(((ether_hdr *)pak->mac_start)->daddr, ether->daddr);

                /*
                 *  Need to update the length.
                 */
                PUTSHORT(&ether->type_or_len, (datagramsize -
                                               ETHER_8023_ENCAPBYTES));
            }
            break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: /* he-ether(sde) to he-ether fwd on-cmplx */

	    pak->encsize = tbridge_dlli.da_offset + sde_encapwords;

	    if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   pak->encsize))
		return(TBRIDGE_TLB_NO_TXBUF);

	    if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
		(TBR_ETHER_SDE == tbridge_dlli.src_media_type))
                tbridge_mci_ether_mac_wr_inline(outputhw, pak,
                                                datagramsize -
                                                ETHER_8023_ENCAPBYTES);
	    break;

	case TLB_MCI_TO_MCI: /* he-ether(sde) to he-ether fwd off-cmplx */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);

	    if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
		(TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
                tbridge_mci_ether_mac_wr_inline(outputhw, pak,
                                                datagramsize -
                                                ETHER_8023_ENCAPBYTES);
                tbridge_mci_to_mci_inline(inputhw, outputhw,
                                          tbridge_dlli.da_offset +
                                          ETHER_8023_ENCAPWORDS +
                                          sde_encapwords,
                                          datagramsize -
                                          ETHER_8023_ENCAPBYTES);
            } else
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.da_offset,
					  datagramsize);
	    break;

	case TLB_BFR_TO_MCI: /* he-ether(sde) to he-ether fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
 
	    if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
		(TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
                tbridge_mci_ether_mac_wr_inline(outputhw, pak,
                                                datagramsize -
                                                ETHER_8023_ENCAPBYTES);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_8023_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_8023_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
	    } else {
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset,
						     datagramsize);
		} else {
		    tbridge_mci_datagram_wr_inline(outputhw, pak, 
						   datagramsize);
		}
	    }
	    break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_FDDI_SDE:

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_FDDI:		/* fddi to ether */

	/* LBJ, info_start better not be NULL! */
	snap = (snap_hdr *)pak->info_start;

        if (GETSHORT(&snap->dsap) == SNAPSNAP) {
          /*
           * All the computation below assumes that the packet at least
           * has complete SNAP header. Bail out now if it doesn't.
           */
          if (datagramsize < FDDI_SNAP_ENCAPBYTES)
            return (TBRIDGE_TLB_UNSUPPORTED);
        }

	if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
	    (GETLONG(&snap->control) == SNAP_CTL_OUI)) {

	    /*
	     * SNAP encapsulation on FDDI is always translated to
	     * ARPA encapsulation on ETHER.
	     */
	    datagramsize  -= (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* le-fddi(sde) to le-ARPA fwd and fld */
				 /* rsp-fddi(sde) to rsp-ARPA fwd and fld */

		ether = (ether_hdr *) pak->datagramstart = 
		    (pak->mac_start + FDDI_SNAP_ENCAPBYTES + 
		     (sde_encapwords * 2)) - ETHER_ARPA_ENCAPBYTES;

		/*
		 * No need to explicitly write the TYPE bytes;
		 * they are already in the proper location in the
		 * original packet.  Just copy the SA and the DA
		 * to their proper places in the ether header.
		 */
		ieee_move(((fddi_hdr *)pak->mac_start)->saddr, ether->saddr);
		ieee_move(((fddi_hdr *)pak->mac_start)->daddr, ether->daddr);

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* he-fddi(sde) to he-ARPA fwd on-cmplx */
		pak->encsize = (tbridge_dlli.mac_offset +
				FDDI_SNAP_ENCAPWORDS +
				sde_encapwords -
				ETHER_ARPA_ENCAPWORDS);

		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
		    return(TBRIDGE_TLB_NO_TXBUF);

		/*
		 * The snap->type argument it superfluous here
		 * since the type code is already in place in
		 * the original packet.
		 */
		tbridge_mci_ether_mac_wr_inline(inputhw, pak, snap->type);
		break;

	    case TLB_MCI_TO_MCI: /* he-fddi(sde) to he-ARPA fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak, snap->type);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  FDDI_SNAP_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  ETHER_ARPA_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* he-fddi(sde) to he-ARPA fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);

		tbridge_mci_ether_mac_wr_inline(outputhw, pak, snap->type);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     FDDI_SNAP_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_ARPA_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }

	} else {

	    /*
	     * The outgoing frame encapsulation is 802.3
	     */
	    datagramsize  += (ETHER_8023_ENCAPBYTES - FDDI_ENCAPBYTES);

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* le-fddi(sde) to le-802.3 fwd and fld */
				 /* rsp-fddi(sde) to rsp-802.3 fwd and fld */

		ether = (ether_hdr *) pak->datagramstart = 
		    (pak->mac_start + FDDI_ENCAPBYTES + 
		     (sde_encapwords * 2)) - ETHER_8023_ENCAPBYTES;

		ieee_move(((fddi_hdr *)pak->mac_start)->daddr, ether->daddr);
		ieee_move(((fddi_hdr *)pak->mac_start)->saddr, ether->saddr);
		PUTSHORT(&ether->type_or_len, 
                         (datagramsize - ETHER_8023_ENCAPBYTES));

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* he-fddi(sde) to he-802.3 fwd on-cmplx */
		pak->encsize = (tbridge_dlli.mac_offset +
				FDDI_ENCAPWORDS +
				sde_encapwords -
				ETHER_8023_ENCAPWORDS);

		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
		    return(TBRIDGE_TLB_NO_TXBUF);

		tbridge_mci_ether_mac_wr_inline(inputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		break;

	    case TLB_MCI_TO_MCI: /* he-fddi(sde) to he-802.3 fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  FDDI_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  ETHER_8023_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* he-fddi(sde) to he-802.3 fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
			return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     FDDI_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_8023_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
		break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	}
	break;

    case TBR_TR_SDE:

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_TR:		/* ltr to ether */

	snap = (snap_hdr *)pak->info_start;

	if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
	    (GETLONG(&snap->control) == inputhw->tr_transit_oui)) {

	    /*
	     * The outgoing frame encapsulation is ARPA
	     */
	    datagramsize  -= (TRING_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* ltr(sde) to le-ARPA fwd and fld */
				 /* ltr(sde) not supportted on rsp */

		ether = (ether_hdr *) pak->datagramstart = 
		    (pak->mac_start + TRING_SNAP_ENCAPBYTES +
		     (sde_encapwords * 2)) - ETHER_ARPA_ENCAPBYTES;

		ieee_move(((tring_hdr *)pak->mac_start)->saddr, ether->saddr);
		ieee_move(((tring_hdr *)pak->mac_start)->daddr, ether->daddr);

		break;

	    /* case NOT SUPPORTED */
	    case TLB_WITHIN_MCI:	/* ltr is not an mci interface */
	    case TLB_MCI_TO_MCI:	/* ltr is not an mci interface */
	    case TLB_BFR_TO_MCI:	/* ltr to he-ether not supported */
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }

	} else {

	    /*
	     * The outgoing frame encapsulation is 802.3
	     */

	    /*
	     * Note that datagramsize is not adjusted here since the
	     * encapsulation size for 802.5 token ring just happens
	     * to be the same as 802.3 ether.
	     */

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* ltr(sde) to le-802.3 fwd and fld */
				 /* ltr(sde) not supportted on rsp */

		ether = (ether_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);

		ieee_move(((tring_hdr *)pak->mac_start)->daddr, ether->daddr);
		ieee_move(((tring_hdr *)pak->mac_start)->saddr, ether->saddr);
		PUTSHORT(&ether->type_or_len,
                        (datagramsize - ETHER_ARPA_ENCAPBYTES));

		break;

	    /* case NOT SUPPORTED */
	    case TLB_WITHIN_MCI:	/* ltr is not an mci interface */
	    case TLB_MCI_TO_MCI:	/* ltr is not an mci interface */
	    case TLB_BFR_TO_MCI:	/* ltr to he-ether not supported */
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	}
	break;

    case TBR_CTR_SDE:

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_CTR:		/* ctr to ether */

	snap = (snap_hdr *)pak->info_start;

	if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
	    /*
	     * Old style abuse of SNAP-F8.  Should compare to SNAP_CTL_OUI.
	     */
	    (GETLONG(&snap->control) == inputhw->tr_transit_oui)) {

	    /*
	     * The outgoing frame encapsulation is ARPA
	     */
	    datagramsize  -= (TRING_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* ctr(sde) to rsp-ARPA fwd and fld */
	    			 /* ctr(sde) to le-ARPA not supported */

		ether = (ether_hdr *) pak->datagramstart = 
		    (pak->mac_start + TRING_SNAP_ENCAPBYTES + 
		     (sde_encapwords * 2)) - ETHER_ARPA_ENCAPBYTES;

		ieee_move(((tring_hdr *)pak->mac_start)->saddr, ether->saddr);
		ieee_move(((tring_hdr *)pak->mac_start)->daddr, ether->daddr);

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* ctr(sde) to he-ARPA fwd on-cmplx */
		pak->encsize = (tbridge_dlli.mac_offset +
				TRING_SNAP_ENCAPWORDS +
				sde_encapwords -
				ETHER_ARPA_ENCAPWORDS);

		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
		    return(TBRIDGE_TLB_NO_TXBUF);
		/*
		 * The snap->type argument it superfluous here
		 * since the type code is already in place in
		 * the original packet.
		 */
		tbridge_mci_ether_mac_wr_inline(inputhw, pak, snap->type);
		break;

	    case TLB_MCI_TO_MCI: /* ctr(sde) to he-ARPA fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak, snap->type);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  TRING_SNAP_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  ETHER_ARPA_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* ctr(sde) to he-ARPA fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak, snap->type);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_SNAP_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_ARPA_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }

	} else {

	    /*
	     * The outgoing frame encapsulation is 802.3
	     */

	    /*
	     * Note that datagramsize is not adjusted here since the
	     * encapsulation size for 802.5 token ring just happens
	     * to be the same as 802.3 ether.
	     */

	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* ctr(sde) to rsp-802.3 fwd and fld */
	    			 /* ctr(sde) to le-802.3 not supported */

		ether = (ether_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);

		ieee_move(((tring_hdr *)pak->mac_start)->daddr, ether->daddr);
		ieee_move(((tring_hdr *)pak->mac_start)->saddr, ether->saddr);
		PUTSHORT(&ether->type_or_len,
                        (datagramsize - ETHER_ARPA_ENCAPBYTES));

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* ctr(sde) to he-802.3 fwd on-cmplx */
		pak->encsize = (tbridge_dlli.mac_offset +
				TRING_8025_ENCAPWORDS +
				sde_encapwords -
				ETHER_8023_ENCAPWORDS);

		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
		    return(TBRIDGE_TLB_NO_TXBUF);

		tbridge_mci_ether_mac_wr_inline(inputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		break;

	    case TLB_MCI_TO_MCI: /* ctr(sde) to he-802.3 fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  TRING_8025_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  ETHER_8023_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* ctr(sde) to he-802.3 fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_ether_mac_wr_inline(outputhw, pak,
						datagramsize -
						ETHER_8023_ENCAPBYTES);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_8025_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_8023_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	}
	break;

    default:
	return(TBRIDGE_TLB_UNSUPPORTED);
    }

    if (datagramsize < MINETHERSIZE)
	/*
	 * Should we pad with zeroes to keep a "data barrier"
	 * between bridged interfaces/groups?
	 */
	datagramsize = MINETHERSIZE;

    pak->datagramsize  = datagramsize;
    
    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_fddi_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a FDDI frame.
 *
 * TLB_WITHIN_MCI, TLB_MCI_TO_MCI, and TLB_BFR_TO_MCI notes:
 * - check for all conditions that prevent forwarding and
 *   return FALSE for these BEFORE the MCI/cBus select
 *   operation occurs since this commits us to transmission.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_fddi_inline (idbtype *outputsw, paktype *pak,
			const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw    = pak->if_input;
    hwidbtype *inputhw  = inputsw->hwptr;
#endif
    hwidbtype *outputhw = outputsw->hwptr;
    ushort datagramsize = pak->datagramsize;
    ushort sde_encapwords = 0;

    fddi_hdr *fddi;
    snap_hdr *snap;

    if (TLB_BFR_TO_BFR == code_switch) {
        if (outputhw->type == IDBTYPE_BSI) {
        /*
         * Swap the destination and source addresses for translation
         * to le-fddi.  It would be more correct to keep this out of
         * the translator function, and have the low-end drivers do
         * this on transmission.  Having this here embeds it in the
         * translator, thereby diluting its generality.
         */

        ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        }
    }

    switch (tbridge_dlli.src_media_type) {
    case TBR_FDDI_SDE:          /* fddi-sde to fddi */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

    case TBR_FDDI:		/* fddi to fddi */
	/* Not really necessary from fddi to fddi */
	if (datagramsize > outputhw->max_pak_size)
	    return(TBRIDGE_TLB_EXCEEDS_MTU);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* le-fddi(sde) to le-fddi fwd and fld */
			     /* rsp-fddi(sde) to rsp-fddi fwd and fld */

	    pak->datagramstart = pak->mac_start;

            if (TBR_FDDI_SDE == tbridge_dlli.src_media_type) {
                fddi = (fddi_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);
                fddi->fc = FDDI_LLC_FC_BYTE;
                ieee_move(((fddi_hdr *)pak->mac_start)->saddr, fddi->saddr);
                ieee_move(((fddi_hdr *)pak->mac_start)->daddr, fddi->daddr);
            } else {
		/* pop start to beginning of fddi header */
		pak->datagramstart = pak->mac_start;
	    }
            break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: /* he-fddi(sde) to he-fddi fwd on-cmplx */
	    if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   tbridge_dlli.mac_offset +
						   sde_encapwords))
		    return(TBRIDGE_TLB_NO_TXBUF);
	    if ((TBR_FDDI_SDE == tbridge_dlli.src_media_type) ||
		(TBR_FDDI == tbridge_dlli.src_media_type))
		    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

	    break;

	case TLB_MCI_TO_MCI: /* he-fddi(sde) to he-fddi fwd off-cmplx */
	    if (inputsw->tbridge_media == TBR_FDDI) {
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);

		if (TBR_FDDI_SDE == tbridge_dlli.src_media_type) {
		    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

		    tbridge_mci_to_mci_inline(inputhw, outputhw,
					      tbridge_dlli.mac_offset +
					      FDDI_ENCAPWORDS +
					      sde_encapwords,
					      datagramsize - FDDI_ENCAPBYTES);
		} else
		    tbridge_mci_to_mci_inline(inputhw, outputhw, 
					      tbridge_dlli.mac_offset,
					      datagramsize);
	    } else {
		/*
		 * if it came from atm, virtual fddi, have to swap the mac
		 * addrs because the hw flips the mac addrs automagically.
		 * mac addrs have already been swapped to canonical
		 * on receive.
		 */
		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset + 
					  FDDI_ENCAPWORDS,
					  datagramsize - FDDI_ENCAPBYTES);
	    }
	    break;

	case TLB_BFR_TO_MCI: /* he-fddi(sde) to he-fddi fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);

	    /*
	     * CSCdi23653
	     * We cannot just use tbridge_mci_datagram_wr_inline to copy
	     * the packet to the output interface since datagramstart
	     * points directly at the FC byte.  We need to short-word
	     * align the FC byte, then write out the rest of the packet.
	     * So we do this in two steps.
	     */
	    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 FDDI_ENCAPWORDS +
						 sde_encapwords,
						 datagramsize -
						 FDDI_ENCAPBYTES);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
            break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_HDLC_TRANSIT_SDE:          /* hdlc-sde to fddi */
    case TBR_ETHER_SDE:

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:	/* serial-t to fddi */
    case TBR_FDDI_TRANSIT:	/* fddi-t to fddi */

	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

	/*
	 * Fall through to the ETHER case.
	 */

    case TBR_ETHER:		/* ether to fddi */
	if (pak->info_start) {
	    /*
	     * The outgoing frame encapsulation is LLC + possible SNAP.
	     * Just move the DA and SA down the packet to overwrite
	     * the length field.
	     */

            /*
             * Don't get the packet size from the "length" field if it
             * is zero.  We use this trick to carry an ether-like
             * packet > ether_mtu over serial.
             */
            if (GETSHORT( &((ether_hdr *)pak->mac_start)->type_or_len)) {
                datagramsize = 
                GETSHORT( &((ether_hdr *)pak->mac_start)->type_or_len);
            } else {
                datagramsize -= ETHER_ARPA_ENCAPBYTES;
            }

            datagramsize += FDDI_ENCAPBYTES;

	    /* Not really necessary from ether to fddi */
	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* le-802.3(sde) to le-fddi fwd and fld */
				 /* rsp-802.3(sde) to rsp-fddi fwd and fld */

		fddi = (fddi_hdr *) pak->datagramstart = 
		    (pak->mac_start + ETHER_8023_ENCAPBYTES + 
		     (sde_encapwords * 2)) - FDDI_ENCAPBYTES;

		ieee_move(((ether_hdr *)pak->mac_start)->saddr, fddi->saddr);
		ieee_move(((ether_hdr *)pak->mac_start)->daddr, fddi->daddr);
		fddi->fc = FDDI_LLC_FC_BYTE;

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* he-802.3(sde) to he-fddi fwd on-cmplx */
		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       tbridge_dlli.da_offset +
						       ETHER_8023_ENCAPWORDS +
						       sde_encapwords -
						       FDDI_ENCAPWORDS))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(inputhw, pak);
		break;

	    case TLB_MCI_TO_MCI: /* he-802.3(sde) to he-fddi fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.da_offset +
					  ETHER_8023_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  FDDI_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* he-802.3(sde) to he-fddi fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_8023_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     FDDI_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	} else {
	    /*
             * 802.10 SDE packet won't never get to here.
	     *
	     * ARPA encapsulation on ETHER is always translated to
	     * SNAP encapsulation on FDDI.
	     */
	    datagramsize  += (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);

	    /* Not really necessary from ether to fddi */
	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: {	/* le-ARPA to le-fddi fwd and fld */
					/* rsp-ARPA to rsp-fddi fwd and fld */

		fddi_hdr *fddi = (fddi_hdr *)pak->datagramstart =
		    (pak->mac_start + ETHER_ARPA_ENCAPBYTES) - 
			FDDI_SNAP_ENCAPBYTES;

		snap = (snap_hdr *)fddi->data;
		ieee_move(((ether_hdr *)pak->mac_start)->daddr, fddi->daddr);
		ieee_move(((ether_hdr *)pak->mac_start)->saddr, fddi->saddr);
		fddi->fc = FDDI_LLC_FC_BYTE;
		PUTSHORT(&snap->dsap, SNAPSNAP);
		PUTLONG(&snap->control, SNAP_CTL_OUI);

		break;
		}

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI:	/* he-ARPA to he-fddi fwd on-cmplx */
		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       tbridge_dlli.da_offset +
						       ETHER_ARPA_ENCAPWORDS -
						       FDDI_SNAP_ENCAPWORDS))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(inputhw, pak);
		tbridge_mci_snap_wr_inline(inputhw, SNAP_CTL_OUI,
					   ((ether_hdr *)
					    pak->addr_start)->type_or_len);

		break;

	    case TLB_MCI_TO_MCI:	/* he-ARPA to he-fddi fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
		tbridge_mci_snap_wr_inline(outputhw, SNAP_CTL_OUI,
					   ((ether_hdr *)
					    pak->addr_start)->type_or_len);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.da_offset +
					  ETHER_ARPA_ENCAPWORDS,
					  datagramsize -
					  FDDI_SNAP_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI:	/* he-ARPA to he-fddi fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
		tbridge_mci_snap_wr_inline(outputhw, SNAP_CTL_OUI,
					   ((ether_hdr *)
					    pak->addr_start)->type_or_len);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_ARPA_ENCAPWORDS,
						     datagramsize -
						     FDDI_SNAP_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
		break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	}
	break;

    case TBR_TR_SDE:            /* ltr-sde to le-fddi */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_TR:		/* ltr to le-fddi */

	datagramsize  -= (TRING_8025_ENCAPBYTES - FDDI_ENCAPBYTES);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* ltr(sde) to le-fddi fwd and fld */
			     /* ltr(sde) is not supported on rsp */

            fddi = (fddi_hdr *)pak->datagramstart = 
		(pak->mac_start + TRING_8025_ENCAPBYTES + 
		 (sde_encapwords * 2)) - FDDI_ENCAPBYTES;

            if (TBR_TR_SDE == tbridge_dlli.src_media_type) {
                ieee_move(((tring_hdr *)pak->mac_start)->saddr, fddi->saddr);
                ieee_move(((tring_hdr *)pak->mac_start)->daddr, fddi->daddr);
            }

            fddi->fc = FDDI_LLC_FC_BYTE;

	    break;

#ifdef TBRIDGE_MCI_CBUS
	/* case NOT SUPPORTED */
	case TLB_WITHIN_MCI:	/* ltr is not an mci interface */
	case TLB_MCI_TO_MCI:	/* ltr is not an mci interface */
	case TLB_BFR_TO_MCI:	/* ltr to he-fddi not supported */
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_CTR_SDE:           /* ctr-sde to he-fddi */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_CTR:		/* ctr to he-fddi */

	datagramsize  -= (TRING_8025_ENCAPBYTES - FDDI_ENCAPBYTES);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* ctr(sde) to rsp-fddi fwd and fld */
			     /* ctr(sde) is not supported on le */

            fddi = (fddi_hdr *)pak->datagramstart = 
		(pak->mac_start + TRING_8025_ENCAPBYTES + 
		 (sde_encapwords * 2)) - FDDI_ENCAPBYTES;

            /*
             * Driver will take care of fddi->ac.
             */
            if (TBR_CTR_SDE == tbridge_dlli.src_media_type) {
                ieee_move(((tring_hdr *)pak->mac_start)->saddr, fddi->saddr);
                ieee_move(((tring_hdr *)pak->mac_start)->daddr, fddi->daddr);
            }

            fddi->fc = FDDI_LLC_FC_BYTE;

	    break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: /* ctr(sde) to he-fddi fwd on-cmplx */
	    if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   tbridge_dlli.mac_offset +
						   TRING_8025_ENCAPWORDS +
						   sde_encapwords -
						   FDDI_ENCAPWORDS))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_fddi_mac_wr_inline(inputhw, pak);
	    break;

	case TLB_MCI_TO_MCI: /* ctr(sde) to he-fddi fwd off-cmplx */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
	    tbridge_mci_to_mci_inline(inputhw, outputhw,
				      tbridge_dlli.mac_offset +
				      TRING_8025_ENCAPWORDS +
				      sde_encapwords,
				      datagramsize -
				      FDDI_ENCAPBYTES);
	    break;

	case TLB_BFR_TO_MCI: /* ctr(sde) to he-fddi fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

	    snap = (snap_hdr *)pak->info_start;
	    if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
		/*
		 * Old style abuse of SNAP-F8.  Should compare to SNAP_CTL_OUI.
		 */
		(GETLONG(&snap->control) == inputhw->tr_transit_oui)) {
		tbridge_mci_snap_wr_inline(outputhw, SNAP_CTL_OUI,
					   snap->type);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_SNAP_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     FDDI_SNAP_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
           } else {
	       if (inputsw->span_ptr->cbus_copy_capable) {
                   tbridge_cbus_copy_rx_data_inline(outputhw,
						    tbridge_dlli.mac_offset +
                                                    TRING_8025_ENCAPWORDS +
                                                    sde_encapwords,
                                                    datagramsize -
                                                    FDDI_ENCAPBYTES);
	       } else {
                   tbridge_mci_llcdata_wr_inline(outputhw, pak);
               }
           }
            break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    default:
	return(TBRIDGE_TLB_UNSUPPORTED);
    }

    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_tring_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a Token Ring (TR, CTR) frame.
 *
 * TLB_WITHIN_MCI, TLB_MCI_TO_MCI, and TLB_BFR_TO_MCI notes:
 * - check for all conditions that prevent forwarding and
 *   return FALSE for these BEFORE the MCI/cBus select
 *   operation occurs since this commits us to transmission.
 */

static const uchar apple_ether_broadcast[IEEEBYTES] = 
		    {0x09, 0x00, 0x07, 0xff, 0xff, 0xff};
static const uchar apple_tr_broadcast[IEEEBYTES] =
		    {0xC0, 0x00, 0x40, 0x00, 0x00, 0x00};

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_tring_inline (idbtype *outputsw, paktype *pak,
			 const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw    = pak->if_input;
    hwidbtype *inputhw  = inputsw->hwptr;
#endif
    hwidbtype *outputhw = outputsw->hwptr;
    ushort datagramsize = pak->datagramsize;
    ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;
    ushort sde_encapwords = 0;

    tring_hdr *tring;
    snap_hdr *snap;
    boolean dont_swap_da;

    dont_swap_da = FALSE;

    if (TLB_BFR_TO_BFR == code_switch) {
	/*
	 * Swap the destination and source addresses for translation
	 * to ltr.  It would be more correct to keep this out of
	 * the translator function, and have the low-end drivers do
	 * this on transmission.  Having this here embeds it in the
	 * translator, thereby diluting its generality.
	 */

	/*
	 * If the input interface is a BVI(which is virtual-ether) and
	 * it is an IPX packt, that means this packet is from an IPX
	 * routed interface, where the DA of MAC header is extract from
	 * the destination MAC *inside* the IPX packet. We don't want 
	 * to swap the DA here because it should be in token format. 
	 * If we swap it, it will be in canonical format on the token, 
	 * and the host won't pick it up
	 *
	 * If it is an atalk packet with Appletalk multicast DA, do the
	 * multicast translation for it since just bit-swap is NOT
	 * equal to the multicast address on token(functional address)
	 *
	 * We can't check on arpa_type for IPX packets because the
	 * Novell-ether packet doesn't have a ARPA type;
	 * We can't check on rxtype for Atalk packets because the
	 * AARP packet doesn't have its own rxtype.
	 */
	if (is_bvi(pak->if_input->hwptr)) {
	    ushort arpa_type;

            pak->rxtype =
                ether_decode_encapsulation_inline(pak, (ether_hdr *)
						  pak->datagramstart);

	    arpa_type = tbridge_get_arpa_type_inline(pak);

	    if ((pak->rxtype == RXTYPE_NOVELL1) ||
		(pak->rxtype == RXTYPE_SNAP_NOV1) ||
		(pak->rxtype == RXTYPE_NOVELL2) ||
		(pak->rxtype == RXTYPE_SAP_NOVELL)) {
		dont_swap_da = TRUE;
	    } else if ((arpa_type == TYPE_ETHERTALK) || 
		       (arpa_type == TYPE_AARP)) {
		
		if (ieee_equal(addr_start->daddr, apple_ether_broadcast)) {
		    ieee_copy(apple_tr_broadcast, addr_start->daddr);
		    /* In token format already */
		    dont_swap_da = TRUE;
		}
	    }
	}

	if (!dont_swap_da)
	    ieee_swap(addr_start->daddr, addr_start->daddr);

	ieee_swap(addr_start->saddr, addr_start->saddr);
    }

    switch (tbridge_dlli.src_media_type) {

    case TBR_TR_SDE:            /* ltr-sde to ltr */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_TR:		/* ltr to ltr */
	switch (code_switch) {

	case TLB_BFR_TO_BFR:	/* ltr(sde) to ltr fwd and fld */
				/* ltr(sde) is not supported on rsp */

	    pak->datagramstart = pak->mac_start;

            if (TBR_TR_SDE == tbridge_dlli.src_media_type) {
                tring = (tring_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);

                ieee_move(((tring_hdr *)pak->mac_start)->saddr, tring->saddr);
                ieee_move(((tring_hdr *)pak->mac_start)->daddr, tring->daddr);
            } else {
		/* Pop header to beginning of header */
		pak->datagramstart = pak->mac_start;
		tring = (tring_hdr *) pak->datagramstart;
	    }

	    /*
	     * CSCdi78734: 
	     * If we are forwarding packet to a token 
	     * ring, we always need to reset the AC
	     * and FC to be safe. The ucode will set 
	     * the correct AC FC bits per context of 
	     * the output ring
	     */
	    tring->ac = AC_PRI0;
	    tring->fc = FC_TYPE_LLC;
	    break;

#ifdef TBRIDGE_MCI_CBUS
	/* case NOT SUPPORTED */
	case TLB_WITHIN_MCI:	/* ltr is not an mci interface */
	case TLB_MCI_TO_MCI:	/* ltr is not an mci interface */
	case TLB_BFR_TO_MCI:	/* ltr to ctr not supported */
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_CTR_SDE:           /* ctr-sde to tring */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */


    case TBR_CTR:		/* ctr to tring */
	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* ctr(sde) to ctr fwd and fld */
			     /* ctr(sde) is not supported on le */

	    pak->datagramstart = pak->mac_start;

            if (TBR_CTR_SDE == tbridge_dlli.src_media_type) {
                tring = (tring_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);

                ieee_move(((tring_hdr *)pak->mac_start)->saddr, tring->saddr);
                ieee_move(((tring_hdr *)pak->mac_start)->daddr, tring->daddr);
            } else {
                tring = (tring_hdr *) pak->datagramstart;
	    }

	    /*
	     * RSP requires tring->ac.
	     */
	    tring->ac = AC_PRI0;
	    tring->fc = FC_TYPE_LLC;

	    break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: /* ctr(sde) to ctr fwd on-cmplx*/
	    if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   tbridge_dlli.mac_offset +
						   sde_encapwords))
		return(TBRIDGE_TLB_NO_TXBUF);

            if (TBR_CTR_SDE == tbridge_dlli.src_media_type)
                tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
            else
		/*
		 * Note that the rx move has left the first byte
		 * of the datagram selected so we can just overwrite
		 * it with the desired ACFC.
		 */
		inputhw->mci_regptr->write1short =
		    (AC_PRI0 << 8) | FC_TYPE_LLC;
	    break;

	case TLB_MCI_TO_MCI: /* ctr(sde) to ctr fwd off-cmplx */
	    /*
	     * This case was not previously supported.
	     */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);

            if (TBR_CTR_SDE == tbridge_dlli.src_media_type) {
                tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
                tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
                                          TRING_8025_ENCAPWORDS +
                                          sde_encapwords,
					  datagramsize -
					  TRING_8025_ENCAPBYTES);
            } else {
		/*
		 * Note that the tx select has left the first byte
		 * of the datagram selected so we can just write
		 * the desired ACFC before copying the rest of the
		 * datagram.
		 */
		outputhw->mci_regptr->write1short =
		    (AC_PRI0 << 8) | FC_TYPE_LLC;
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  (TR_ACFC_SIZE / 2),
					  datagramsize - TR_ACFC_SIZE);
	    }
	    break;

	case TLB_BFR_TO_MCI: /* ctr(sde) to ctr fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
	        return(TBRIDGE_TLB_NO_TXBUF);
	    /*
	     * We cannot just copy the datagram in the buffer to the
	     * transmit window since the MAC addresses in the buffer
	     * are in canonical order.  We must write a new MAC header,
	     * and then copy the rest of the datagram.
	     */
	    tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
	    
	    if (inputsw->span_ptr->cbus_copy_capable) {
	        tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 TRING_8025_ENCAPWORDS +
						 sde_encapwords,
						 datagramsize -
						 TRING_8025_ENCAPBYTES);
	    } else {
	        tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
	    break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_FDDI_SDE:          /* ctr-fddi to tring */

	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_FDDI:		/* fddi to tring */
	datagramsize  += (TRING_8025_ENCAPBYTES - FDDI_ENCAPBYTES);

	/* Not really necessary from fddi to tring */
	if (datagramsize > outputhw->max_pak_size)
	    return(TBRIDGE_TLB_EXCEEDS_MTU);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* le-fddi(sde) to le-tring fwd and fld */
			     /* rsp-fddi(sde) to ctr fwd and fld */

	    tring = (tring_hdr *)pak->datagramstart = 
		(pak->mac_start + FDDI_ENCAPBYTES + (sde_encapwords * 2)) - 
		    TRING_8025_ENCAPBYTES;

            if (TBR_FDDI_SDE == tbridge_dlli.src_media_type) {
                ieee_move(((fddi_hdr *)pak->mac_start)->saddr, tring->saddr);
                ieee_move(((fddi_hdr *)pak->mac_start)->daddr, tring->daddr);
            }

            /*
             * Driver will take care of tring->ac on le, but RSP requires it.
             */
	    tring->ac = AC_PRI0;
	    tring->fc = FC_TYPE_LLC;

	    break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: /* he-fddi(sde) to ctr fwd on-cmplx */
	    if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   tbridge_dlli.mac_offset +
						   FDDI_ENCAPWORDS +
						   sde_encapwords -
						   TRING_8025_ENCAPWORDS))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_tring_mac_wr_inline(inputhw, addr_start);
	    break;

	case TLB_MCI_TO_MCI: /* he-fddi(sde) to ctr fwd off-cmplx */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
	    tbridge_mci_to_mci_inline(inputhw, outputhw,
				      tbridge_dlli.mac_offset +
				      FDDI_ENCAPWORDS +
				      sde_encapwords,
				      datagramsize -
				      TRING_8025_ENCAPBYTES);
	    break;

	case TLB_BFR_TO_MCI: /* he-fddi(sde) to ctr fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);

	    snap = (snap_hdr *)pak->info_start;
	    if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
		(GETLONG(&snap->control) == SNAP_CTL_OUI)) {
		/*
		 * Old style abuse of SNAP-F8.  Should write SNAP_CTL_OUI.
		 */
		tbridge_mci_snap_wr_inline(outputhw, outputhw->tr_transit_oui,
					   snap->type);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     FDDI_SNAP_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     TRING_SNAP_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
	    } else {
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     FDDI_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     TRING_8025_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
	    }
            break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_HDLC_TRANSIT_SDE:  /* serial-t-sde to tring */

        /* Fall through */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:	/* serial-t to tring */
    case TBR_FDDI_TRANSIT:	/* fddi-t to tring */

	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

	/*
	 * Fall through to the ETHER case.
	 */

    case TBR_ETHER_SDE:         /* ether-sde to tring */
    case TBR_ETHER:		/* ether to tring */
	snap = (snap_hdr *)pak->info_start;

	if (snap) {
	    /*
	     * The outgoing frame encapsulation is LLC + possible SNAP
	     * first get the length of the packet from the length
	     * field since it could be a padded to meet the minimum
	     * length requirement of ethernet.  this is because of
	     * cmns (cons) or sna.  wonderful how all this intermedia
	     * bullshit can really make this a bloody mess.  remember
	     * that the length is only the size of the llc.  it does
	     * not include the da, sa and length.
	     */

            /*
             * Don't get the packet size from the "length" field if it
             * is zero.  We use this trick to carry an ether-like
             * packet > ether_mtu over serial.
             */
            if (GETSHORT( &((ether_hdr *)pak->mac_start)->type_or_len)) {
                datagramsize = 
                GETSHORT( &((ether_hdr *)pak->mac_start)->type_or_len);
            } else {
                datagramsize -= ETHER_ARPA_ENCAPBYTES;
            }

	    datagramsize  += TRING_8025_ENCAPBYTES;

            if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
		(TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
		datagramsize -= SDE_ENCAPBYTES;
		sde_encapwords = SDE_ENCAPWORDS;
	    }

	    /* Not really necessary from ether to tring */
	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: /* le-802.3(sde) to ltr */
				 /* rsp-802.3(sde) to ctr */

		tring = (tring_hdr *)pak->datagramstart = 
		    (pak->mac_start + ETHER_8023_ENCAPBYTES + 
		     (sde_encapwords * 2)) - TRING_8025_ENCAPBYTES;

		ieee_move(((ether_hdr *)pak->mac_start)->saddr, tring->saddr);
		ieee_move(((ether_hdr *)pak->mac_start)->daddr, tring->daddr);

		tring->ac = AC_PRI0;
		tring->fc = FC_TYPE_LLC;

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI: /* he-802.3(sde) to ctr fwd on-cmplx */
		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       tbridge_dlli.da_offset+
						       ETHER_8023_ENCAPWORDS +
						       sde_encapwords -
						       TRING_8025_ENCAPWORDS))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_tring_mac_wr_inline(inputhw, addr_start);
		break;

	    case TLB_MCI_TO_MCI: /* he-802.3(sde) to ctr fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.da_offset +
					  ETHER_8023_ENCAPWORDS +
					  sde_encapwords,
					  datagramsize -
					  TRING_8025_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI: /* he-802.3(sde) to ctr fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_8023_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     TRING_8025_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }

	} else {

            /*
             * 802.10 SDE packet won't never get to here.
	     *
	     * ARPA encapsulation on ETHER is always translated to
	     * SNAP encapsulation on TRING.
	     */
	    datagramsize  += (TRING_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);

	    /* Not really necessary from ether to tring */
	    if (datagramsize > outputhw->max_pak_size)
		return(TBRIDGE_TLB_EXCEEDS_MTU);

	    switch (code_switch) {

	    case TLB_BFR_TO_BFR: 	/* le-ARPA to ltr fwd and fld */
					/* rsp-ARPA to ctr fwd and fld */

		tring = (tring_hdr *)pak->datagramstart = 
		    (pak->mac_start + ETHER_ARPA_ENCAPBYTES) -
			TRING_SNAP_ENCAPBYTES;
		snap  = (snap_hdr *)tring->data;

		ieee_move(((ether_hdr *)pak->mac_start)->daddr, tring->daddr);
		ieee_move(((ether_hdr *)pak->mac_start)->saddr, tring->saddr);
		PUTSHORT(&snap->dsap, SNAPSNAP);
		PUTLONG(&snap->control, outputhw->tr_transit_oui);
		tring->ac = AC_PRI0;
		tring->fc = FC_TYPE_LLC;

		break;

#ifdef TBRIDGE_MCI_CBUS
	    case TLB_WITHIN_MCI:	/* he-ARPA to ctr fwd on-cmplx */
		if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       tbridge_dlli.da_offset +
						       ETHER_ARPA_ENCAPWORDS -
						       TRING_SNAP_ENCAPWORDS))
		    return(TBRIDGE_TLB_NO_TXBUF);

		tbridge_mci_tring_mac_wr_inline(inputhw, addr_start);
		/*
		 * Old style abuse of SNAP-F8.  Should write SNAP_CTL_OUI.
		 */
		tbridge_mci_snap_wr_inline(inputhw, outputhw->tr_transit_oui,
					   ((ether_hdr *)
					    addr_start)->type_or_len);
		break;

	    case TLB_MCI_TO_MCI:	/* he-ARPA to ctr fwd off-cmplx */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
		/*
		 * Old style abuse of SNAP-F8.  Should write SNAP_CTL_OUI.
		 */
		tbridge_mci_snap_wr_inline(outputhw, outputhw->tr_transit_oui,
					   ((ether_hdr *)
					    addr_start)->type_or_len);
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.da_offset +
					  ETHER_ARPA_ENCAPWORDS,
					  datagramsize -
					  TRING_SNAP_ENCAPBYTES);
		break;

	    case TLB_BFR_TO_MCI:	/* he-ARPA to ctr fld */
		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);
		tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
		/*
		 * Old style abuse of SNAP-F8.  Should write SNAP_CTL_OUI.
		 */
		tbridge_mci_snap_wr_inline(outputhw, outputhw->tr_transit_oui,
					   ((ether_hdr *)
					    addr_start)->type_or_len);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_ARPA_ENCAPWORDS,
						     datagramsize -
						     TRING_SNAP_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
                break;
#else
	    default:
#endif
		return(TBRIDGE_TLB_UNSUPPORTED);
		break;
	    }
	}
	break;

      default:
	return(TBRIDGE_TLB_UNSUPPORTED);
    }

    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_fddi_transit_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a FDDI-transit frame.
 *
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_fddi_transit_inline (idbtype *outputsw, paktype *pak,
				uchar *proxy_addr,
				const enum TBRIDGE_TRANSLATE code_switch)
{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;

    uchar *dst_addr        = transit_bridge_multicast;
    uchar *src_addr        = outputhw->hardware;

    enum TBRIDGE_TLB_RETVAL action;
    fddi_hdr *fddi;
    snap_hdr *snap;

    /*
     * Determine (to within a few bytes) whether the resulting
     * transit encapsulated packet would exceed the FDDI MTU
     * before trying to translate it to ether for a more exact
     * size.
     */
    if ((ulong)((pak->datagramsize - tbridge_dlli.transit_encapsize) +
	FDDI_SNAP_ENCAPBYTES) > outputhw->max_pak_size)
	return(TBRIDGE_TLB_EXCEEDS_MTU);

    if (outputhw->type != IDBTYPE_FDDIT) {
	dst_addr = bit_swapped_transit_multicast;
	src_addr = outputhw->bit_swapped_hardware;
    }

    switch (code_switch) {

    case TLB_BFR_TO_BFR:	/* le-xxx to le-fddi-t fwd and fld */
				/* rsp-xxx to rsp-fddi-t fwd and fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);
	/*
	 * Find the start and size of our new encapsulation.
	 * Adjust our pointers for FDDI ISO2 encapsulation.
	 * Write the FDDI header out in front of our encapsulated frame.
	 */
	pak->datagramstart -= FDDI_SNAP_ENCAPBYTES;
	fddi = (fddi_hdr *) pak->datagramstart;
	snap = (snap_hdr *)fddi->data;

	if (proxy_addr && (!ieee_equal(proxy_addr,
                                       bit_swapped_transit_multicast)))
	    if (outputhw->type == IDBTYPE_FDDIT)
		ieee_copy(proxy_addr, fddi->daddr);
	    else
		ieee_swap(proxy_addr, fddi->daddr);
	else
	    ieee_copy(dst_addr, fddi->daddr);
	ieee_copy(src_addr, fddi->saddr);
	fddi->fc = FDDI_LLC_FC_BYTE;
	PUTSHORT(&snap->dsap, SNAPSNAP);
	PUTLONG(&snap->control, CISCOOUI);
	PUTSHORT(&snap->type, TYPE_BRIDGE);

        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:	/* he-xxx to he-fddi-t fwd on cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fddi_transit_hdr_wr_inline(inputhw, proxy_addr, src_addr,
					       pak->encsize);
	break;

    case TLB_MCI_TO_MCI:	/* he-xxx to he-fddi-t fwd off cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fddi_transit_hdr_wr_inline(outputhw, proxy_addr, src_addr,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;

    case TLB_BFR_TO_MCI:	/* he-xxx to he-fddi-t fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fddi_transit_hdr_wr_inline(outputhw, dst_addr, src_addr,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;
#else
    default:
#endif
	action = TBRIDGE_TLB_UNSUPPORTED;
	return(action);
	break;
    }

    pak->datagramsize += FDDI_SNAP_ENCAPBYTES;
    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_hdlc_transit_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build an HDLC-transit frame.
 *
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_hdlc_transit_inline (idbtype *outputsw, paktype *pak,
				const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;

    enum TBRIDGE_TLB_RETVAL action;

    /*
     * Determine (to within a few bytes) whether the resulting
     * transit encapsulated packet would exceed the ETHER MTU
     * before trying to translate it to ether for a more exact
     * size.
     */
    if ((ulong)((pak->datagramsize - tbridge_dlli.transit_encapsize) +
	outputhw->span_encapsize) > outputhw->max_pak_size)
	return(TBRIDGE_TLB_EXCEEDS_MTU);

    switch (code_switch) {

    case TLB_BFR_TO_BFR:	/* le-xxx to le-serial-t fwd and fld */
				/* rsp-xxx to rsp-serial-t fwd and fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);
	/*
	 * If we're using LAT compression on this frame,
	 * copy in the compressed size
	 */
	if (outputsw->span_latcompress && (outputsw->span_latsize != -1))
	    pak->datagramsize = outputsw->span_latsize;
	/*
	 * Adjust our pointers for HDLC encapsulation
	 */
	pak->datagramstart -= outputhw->span_encapsize;
	/*
	 * Write the 4 byte HDLC bridge header
	 */
	PUTLONG(pak->datagramstart, HDLC_BRIDGECODE);
        /*
         * Add board level (ie. MIP) encapsulation if necessary for RSP
         */
	if (outputhw->board_encap) {
	    if((*outputhw->board_encap)(pak, outputhw) == FALSE) {
		return(TBRIDGE_TLB_UNSUPPORTED);
	    }
	}
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:	/* he-xxx to he-serial-t fwd on cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_hdlc_transit_hdr_wr_inline(inputhw, outputhw,
					       pak->encsize);
	break;

    case TLB_MCI_TO_MCI:	/* he-xxx to he-serial-t fwd off cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_hdlc_transit_hdr_wr_inline(outputhw, outputhw,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;

    case TLB_BFR_TO_MCI:	/* he-xxx to he-serial-t fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_hdlc_transit_hdr_wr_inline(outputhw, outputhw,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;
#else
    default:
#endif
	action = TBRIDGE_TLB_UNSUPPORTED;
	return(action);
	break;
    }

    pak->datagramsize += outputhw->span_encapsize;
    return(TBRIDGE_TLB_DONE);
}

/*
 * tbridge_to_atm_transit_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a ATM frame.
 *
 * NOTE:  this is not quite like cisco's form of transit bridging
 * in that frames going across a transit medium is not converted
 * to ethernet form.  this is an important distinction!
 *
 * TLB_WITHIN_MCI, TLB_MCI_TO_MCI, and TLB_BFR_TO_MCI notes:
 * - check for all conditions that prevent forwarding and
 *   return FALSE for these BEFORE the MCI/cBus select
 *   operation occurs since this commits us to transmission.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_atm_transit_inline (idbtype *outputsw, paktype *pak, short vcnum,
			const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
    snap_hdr *snap;
    ushort atm_frame_start;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;
    ushort datagramsize    = pak->datagramsize;
    ieee_addrs *addr_start = NULL;
    ushort sde_encapwords = 0;

    switch (tbridge_dlli.src_media_type) {
    /*
     * note:  we don't take off the hdlc transit header or the fddi
     * transit header.  just encapsulate it inside an atm transit
     * bridge header.  i don't think the max size is going to be
     * reached because the max mtu for atm is large, but should
     * be a concern.
     * also, taking off the transit header means that the rx side
     * doesn't know how to reencapsulate it into a cisco specific
     * transit header.  similar to the original atalk traversing
     * a fddi backbone.
     */

    case TBR_HDLC_TRANSIT_SDE:
    case TBR_ETHER_SDE:         /* ether-sde to atm */
	/*
	 * for SDE, take off the SDE encaps...
	 */
	datagramsize -= SDE_ENCAPBYTES;
	sde_encapwords = SDE_ENCAPWORDS;
 
        /* Fall through */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:      /* serial-t to atm */
    case TBR_FDDI_TRANSIT:      /* fddi-t to atm */

	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

        /*
         * Fall through to the ETHER case.
         */

    case TBR_ETHER:             /* ether to atm */

	/*
	 * datagramsize does NOT include the atm header yet. 
	 * Upate it at the end of the switch.
	 */
	
        /* Not really necessary from ether to atm */
        if ((datagramsize + ATM_BRIDGE_ETHER_ENCAPBYTES) >
	    outputhw->max_pak_size)
            return(TBRIDGE_TLB_EXCEEDS_MTU);

        switch (code_switch) {

        case TLB_BFR_TO_BFR: {  /* le-802.3 to le-atm fwd and fld */
	    ether_hdr *ether;

            if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
                (TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
                ether = (ether_hdr *) pak->datagramstart = pak->mac_start +
                    (sde_encapwords * 2);
                ieee_move(((ether_hdr *)pak->mac_start)->saddr, ether->saddr);
                ieee_move(((ether_hdr *)pak->mac_start)->daddr, ether->daddr);
 
                /*
                 * Need to update the length.
                 */
                PUTSHORT(&ether->type_or_len, (datagramsize -
                                               ETHER_8023_ENCAPBYTES));
	    }

            pak->datagramstart = pak->mac_start - ATM_BRIDGE_ETHER_ENCAPBYTES;
	    tbridge_write_1483_hdr_inline(pak, vcnum, TBRIDGE_PID_8023);

	}
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI:    /* he-802.3 to he-atm fwd on-cmplx */

	    atm_frame_start = tbridge_dlli.da_offset + sde_encapwords -
		ATM_BRIDGE_ETHER_ENCAPWORDS;

            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   atm_frame_start))
		return(TBRIDGE_TLB_NO_TXBUF);

	    /*
	     * write the atm rfc 1483 header
	     */
            tbridge_mci_atm_transit_hdr_wr_inline(inputhw, vcnum,
                                                  TBRIDGE_PID_8023);
	    /*
	     * if SDE, take off the SDE header
	     */
            if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
                (TBR_ETHER_SDE == tbridge_dlli.src_media_type))
                tbridge_mci_ether_mac_wr_inline(outputhw, pak,
                                                datagramsize -
                                                ETHER_8023_ENCAPBYTES);
            break;

        case TLB_BFR_TO_MCI:    /* he-802.3 to he-atm fld */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);
	  
            tbridge_mci_atm_transit_hdr_wr_inline(outputhw, vcnum,
                                                  TBRIDGE_PID_8023);

            if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
                (TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
                tbridge_mci_ether_mac_wr_inline(outputhw, pak,
                                                datagramsize -
                                                ETHER_8023_ENCAPBYTES);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_8023_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     ETHER_8023_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
            } else {
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset,
						     datagramsize);
		} else {
		    tbridge_mci_datagram_wr_inline(outputhw, pak, datagramsize);
		}
	    }
	    break;
	    
	    /* case NOT SUPPORTED */
        case TLB_MCI_TO_MCI:    /* he-802.3 to he-atm fwd off-cmplx */
#else
        default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	datagramsize += ATM_BRIDGE_ETHER_ENCAPBYTES;
	break;

    case TBR_FDDI_SDE:          /* fddi-sde to atm */
 	/*
	 * for SDE, take off the SDE encaps...
	 */
        datagramsize -= SDE_ENCAPBYTES;
        sde_encapwords = SDE_ENCAPWORDS;

    case TBR_FDDI:              /* fddi to atm */

	/*
	 * datagramsize does NOT include the atm header yet.
	 * Upate it at the end of the switch
	 */
        if ((datagramsize + (ATM_BRIDGE_OTHER_ENCAPBYTES - 1)) > 
	    outputhw->max_pak_size)
            return(TBRIDGE_TLB_EXCEEDS_MTU);

        switch (code_switch) {

        case TLB_BFR_TO_BFR: {   /* le-fddi to le-atm fwd and fld */
	    fddi_hdr *fddi;

	    /*
	     * if fddi SDE, take off SDE header
	     */
            if (TBR_FDDI_SDE == tbridge_dlli.src_media_type) {
                fddi = (fddi_hdr *) pak->mac_start = pak->mac_start +
                    (sde_encapwords * 2);
                ieee_move(((fddi_hdr *)pak->mac_start)->saddr, fddi->saddr);
                ieee_move(((fddi_hdr *)pak->mac_start)->daddr, fddi->daddr);
            }

            addr_start = (ieee_addrs *)pak->addr_start;
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

            pak->datagramstart = pak->mac_start -
		(ATM_BRIDGE_OTHER_ENCAPBYTES - 1);
	    tbridge_write_1483_hdr_inline(pak, vcnum, TBRIDGE_PID_FDDI);

	}
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI:    /* he-fddi to he-atm fwd on-cmplx */
            addr_start = (ieee_addrs *)pak->addr_start;

	    atm_frame_start = tbridge_dlli.mac_offset + sde_encapwords -
		ATM_BRIDGE_ETHER_ENCAPWORDS;

            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
                                                   atm_frame_start))
                return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_atm_transit_hdr_wr_inline(inputhw, vcnum,
                                                  TBRIDGE_PID_FDDI);
            /*
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1483 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

	    tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
            break;

        case TLB_BFR_TO_MCI: {   /* he-fddi to he-atm fld */
	    ieee_addrs addr_copy;
            mciregtype *mci_regptr = outputhw->mci_regptr;

	    addr_start = (ieee_addrs *)pak->addr_start;

            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_atm_transit_hdr_wr_inline(outputhw, vcnum,
                                                  TBRIDGE_PID_FDDI);
            /*
             * CSCdi23653
             * We cannot just use tbridge_mci_datagram_wr_inline to copy
             * the packet to the output interface since datagramstart
             * points directly at the FC byte.  We need to short-word
             * align the FC byte, then write out the rest of the packet.
             * So we do this in two steps.
             *
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1483 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_copy.daddr);
            ieee_swap(addr_start->saddr, addr_copy.saddr);

	    mci_regptr->write1short = FDDI_LLC_FC_BYTE;
            mci_regptr->write1long = *((ulong *)&addr_copy.daddr[0]);
            mci_regptr->write1long = *((ulong *)&addr_copy.daddr[4]);
            mci_regptr->write1long = *((ulong *)&addr_copy.saddr[2]);

	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 FDDI_ENCAPWORDS +
						 sde_encapwords,
						 datagramsize -
						 FDDI_ENCAPBYTES);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
	}
            break;

        /* case NOT SUPPORTED */
        case TLB_MCI_TO_MCI:    /* he-fddi to he-atm fwd off-cmplx */
#else
        default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	datagramsize += (ATM_BRIDGE_OTHER_ENCAPBYTES - 1);
	break;

    case TBR_TR_SDE:            /* ltr-sde to le-atm */
        datagramsize -= SDE_ENCAPBYTES;
        sde_encapwords = SDE_ENCAPWORDS;
 
        /* Fall through */
    case TBR_TR:   {             /* ltr to le-atm */
	tring_hdr *tring;
	if (TBR_TR_SDE == tbridge_dlli.src_media_type) {
	    tring = (tring_hdr *) pak->mac_start = pak->mac_start +
		(sde_encapwords * 2);
 
	    /*
	     * forget the AC byte.  meaningless on other lans anyway...
	     */
	    tring->fc = FC_TYPE_LLC;
	    ieee_move(((tring_hdr *)pak->mac_start)->saddr, tring->saddr);
	    ieee_move(((tring_hdr *)pak->mac_start)->daddr, tring->daddr);
	}

	addr_start = (ieee_addrs *)pak->addr_start;
        datagramsize += ATM_BRIDGE_ETHER_ENCAPBYTES;
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* ltr to le-atm fwd and fld */
            pak->datagramstart = pak->mac_start - ATM_BRIDGE_ETHER_ENCAPBYTES;
	    tbridge_write_1483_hdr_inline(pak, vcnum, TBRIDGE_PID_8025);

            break;

        /* case NOT SUPPORTED */
        case TLB_WITHIN_MCI:    /* ltr is not an mci interface */
        case TLB_MCI_TO_MCI:    /* ltr is not an mci interface */
        case TLB_BFR_TO_MCI:    /* ltr to he-atm not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
    }
    break;

#ifdef TBRIDGE_MCI_CBUS
    case TBR_CTR_SDE:           /* ctr-sde to tring */
	datagramsize -= SDE_ENCAPBYTES;
        sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */
    case TBR_CTR:               /* ctr to he-atm */

	/*
	 * datagramsize does NOT include the atm header yet.
	 * Upate it at the end of the switch.
	 */
	addr_start = (ieee_addrs *)pak->addr_start;

        switch (code_switch) {

        case TLB_WITHIN_MCI:    /* ctr to he-fddi fwd on-cmplx */
	    atm_frame_start = tbridge_dlli.mac_offset + sde_encapwords -
		ATM_BRIDGE_ETHER_ENCAPWORDS;

            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
                                                   atm_frame_start))
		return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_atm_transit_hdr_wr_inline(inputhw, vcnum,
                                                  TBRIDGE_PID_8025);

            if (TBR_CTR_SDE == tbridge_dlli.src_media_type)
                tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
            break;

        case TLB_BFR_TO_MCI:    /* ctr to he-atm fld */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

	    /*
	     * when copied to the buffer, the address are in
	     * canonical order.  so...
	     * flip the addresses back to non-canonical order
	     * this is to give the illusion on the other side
	     * of this atm virtual circuit that the packet
	     * came off a token ring.
	     */
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_atm_transit_hdr_wr_inline(outputhw, vcnum,
                                                  TBRIDGE_PID_8025);

            tbridge_mci_tr_mac_wr_inline(outputhw, pak);

            /*
             * We cannot just use tbridge_mci_llcdata_wr_inline to copy
             * the rest of the packet to the output interface since in
             * the SNAP case, we must respect the configured transit oui
             * (grrrr). So we do this in two steps.
             */
            snap = (snap_hdr *)pak->info_start;
            if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
                /*
                 * Old style abuse of SNAP-F8.  Should compare to SNAP_CTL_OUI.
                 */
                (GETLONG(&snap->control) == inputhw->tr_transit_oui)) {
                /*
                 * Old style abuse of SNAP-F8.  Should write SNAP_CTL_OUI.
                 */
                tbridge_mci_snap_wr_inline(outputhw, inputhw->tr_transit_oui,
                                           snap->type);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_SNAP_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     TRING_SNAP_ENCAPBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
            } else {
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_8025_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize -
						     TRING_8025_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
	    }
            break;

        /* case NOT SUPPORTED */
        case TLB_MCI_TO_MCI:    /* ctr to he-atm fwd off-cmplx */
        case TLB_BFR_TO_BFR:    /* ctr to le-atm not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	datagramsize += ATM_BRIDGE_ETHER_ENCAPBYTES;
	break;
#endif

    default:
        return(TBRIDGE_TLB_UNSUPPORTED);
        break;

    }

    pak->datagramsize = datagramsize;
    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_lane_ether_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a ATM lane frame.
 *
 * The  frames going across LANE interface is assumed to be in
 * to ethernet form.  In future,it could be a TR frame.
 *
 * TLB_WITHIN_MCI, TLB_MCI_TO_MCI, and TLB_BFR_TO_MCI notes:
 * - check for all conditions that prevent forwarding and
 *   return FALSE for these BEFORE the MCI/cBus select
 *   operation occurs since this commits us to transmission.
 */

STATIC INLINE 
enum TBRIDGE_TLB_RETVAL
tbridge_to_lane_ether_inline (idbtype *outputsw, paktype *pak, ushort vcnum,
			      const enum TBRIDGE_TRANSLATE code_switch)

{

    lec_id_t lecid;
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
    ushort   offset;
#endif
    enum TBRIDGE_TLB_RETVAL action;

    
    /* Check to insure that LANE is configured on this interface */   
    if (is_atm_lane(outputsw))
        lecid = outputsw->lane_info->lec_info->lecid;
    else
        return(TBRIDGE_TLB_UNSUPPORTED);

    switch (code_switch) {

    case TLB_BFR_TO_BFR: 
	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	if (action != TBRIDGE_TLB_DONE)
	    return (action);
	pak->addr_start = pak->datagramstart;
	/* Write the LANE header in front */
	pak->datagramstart -=  ATM_AAL5_LANE_ENCAPBYTES;
	tbridge_write_lane_ether_hdr_inline(pak, vcnum, lecid);

	break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:    /* he-802.3 to he-atm fwd on-cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	offset = pak->encsize;
	tbridge_mci_lane_ether_hdr_wr_inline(inputhw, offset, vcnum, lecid);
 
	break;

    case TLB_MCI_TO_MCI:    /* he-802.3 to he-atm fwd off-cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	offset = TBRIDGE_MCI_TX1_OFFSET;
	tbridge_mci_lane_ether_hdr_wr_inline(inputhw, offset, vcnum, lecid);
	break;

    case TLB_BFR_TO_MCI:    /* he-802.3 to he-atm fld */
	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	offset = TBRIDGE_MCI_TX1_OFFSET;
	tbridge_mci_lane_ether_hdr_wr_inline(inputhw, offset, vcnum, lecid);
	
	break;
#else
    default:
#endif
	return(TBRIDGE_TLB_UNSUPPORTED);
	break;
    }
    pak->datagramsize += ATM_AAL5_LANE_ENCAPBYTES;
    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_ether_sde_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build an 802.3, SDE(802.10) Ethernet frame.
 *
 * pak->encsize is used in MCI cases in this function to record the 
 * offset for use in the transit translation functions to prepend the
 * transit header.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_ether_sde_inline (idbtype *outputsw, paktype *pak,
                             const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw     = pak->if_input;
    hwidbtype *inputhw   = inputsw->hwptr;
#endif
    hwidbtype *outputhw  = outputsw->hwptr;
    ushort datagramsize  = pak->datagramsize;

    ether_hdr *ether;

    datagramsize  += (ETHER_8023_ENCAPBYTES + SDE_ENCAPBYTES);

    if (datagramsize > outputhw->max_pak_size)
        return(TBRIDGE_TLB_EXCEEDS_MTU);

    /*
     * Adjust pak->encsize to record where the wr ptr is.
     */
    switch (code_switch) {

    case TLB_BFR_TO_BFR: /* le-xxx to le-802.3-sde fwd and fld */
			 /* rsp-xxx to rsp-802.3-sde fwd and fld */
        ether = (ether_hdr *) pak->datagramstart = pak->info_start -
            (ETHER_8023_ENCAPBYTES + SDE_ENCAPBYTES);
        ieee_move(((ether_hdr *)pak->addr_start)->saddr, ether->saddr);
        ieee_move(((ether_hdr *)pak->addr_start)->daddr, ether->daddr);
        PUTSHORT(&ether->type_or_len, (datagramsize -
                                               ETHER_8023_ENCAPBYTES));
        tbridge_bfr_sde_header_wr_inline(outputsw, ether->data, ether->saddr);
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI: /* he-xxx to he-802.3-sde fwd on-cmplx */
	pak->encsize -= (ETHER_8023_ENCAPWORDS + SDE_ENCAPWORDS);

        tbridge_mci_tx1_select_offset_inline(inputhw, pak->encsize);
        tbridge_mci_ether_mac_wr_inline(inputhw, pak, datagramsize -
                                        ETHER_8023_ENCAPBYTES);
        tbridge_mci_sde_header_wr_inline(inputhw, outputsw, 
					 &pak->addr_start[IEEEBYTES]);
        break;

    case TLB_MCI_TO_MCI: /* he-xxx to he-802.3-sde fwd off-cmplx */
    case TLB_BFR_TO_MCI: /* he-xxx to he-802.3-sde fld */
	pak->encsize -= (ETHER_8023_ENCAPWORDS + SDE_ENCAPWORDS);

        tbridge_mci_tx1_select_offset_inline(outputhw, pak->encsize);
        tbridge_mci_ether_mac_wr_inline(outputhw, pak, datagramsize -
                                        ETHER_8023_ENCAPBYTES);
        tbridge_mci_sde_header_wr_inline(outputhw, outputsw,
					 &pak->addr_start[IEEEBYTES]);
        break;
#else
    default:
#endif
        return(TBRIDGE_TLB_UNSUPPORTED);
        break;
    }

    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_fddi_sde_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a FDDI-SDE(802.10) frame.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_fddi_sde_inline (idbtype *outputsw, paktype *pak,
                        const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;
    ushort datagramsize    = pak->datagramsize;

    fddi_hdr *fddi;

    /*
     * Adjust datagramsize beforehand and back-up write-ptr preparing
     * for pre-pending header.
     */
    datagramsize  += (FDDI_ENCAPBYTES + SDE_ENCAPBYTES);
    if (datagramsize > outputhw->max_pak_size)
        return(TBRIDGE_TLB_EXCEEDS_MTU);

    if (TLB_BFR_TO_BFR == code_switch) {
        if (outputhw->type == IDBTYPE_BSI) {

        /*
         * Swap the destination and source addresses for translation
         * to le-fddi.  It would be more correct to keep this out of
         * the translator function, and have the low-end drivers do
         * this on transmission.  Having this here embeds it in the
         * translator, thereby diluting its generality.
         */
        ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        }
    }

    switch (code_switch) {
    case TLB_BFR_TO_BFR:        /* le-xxx to le-fddi-sde fwd and fld */
				/* rsp-xxx to rsp-fddi-sde fwd and fld */

        fddi = (fddi_hdr *) pak->datagramstart = (pak->info_start -
                                                  (FDDI_ENCAPBYTES +
						   SDE_ENCAPBYTES));
        ieee_move(((ether_hdr *)pak->addr_start)->daddr, fddi->daddr);
        ieee_move(((ether_hdr *)pak->addr_start)->saddr, fddi->saddr);
        fddi->fc = FDDI_LLC_FC_BYTE;
        tbridge_bfr_sde_header_wr_inline(outputsw, fddi->data, fddi->saddr);
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:        /* he-xxx to he-fddi-sde fwd on-cmplx */
        tbridge_mci_tx1_select_offset_inline(inputhw, pak->encsize -
                                             (SDE_ENCAPWORDS +
                                              FDDI_ENCAPWORDS));
        tbridge_mci_fddi_mac_wr_inline(inputhw, pak);
        tbridge_mci_sde_header_wr_inline(inputhw, outputsw,
					 &pak->addr_start[IEEEBYTES]);
        break;

    case TLB_MCI_TO_MCI:        /* he-xxx to he-fddi-sde fwd off-cmplx */
    case TLB_BFR_TO_MCI:        /* he-xxx to he-fddi-sde fld */
        tbridge_mci_tx1_select_offset_inline(outputhw, pak->encsize -
                                             (SDE_ENCAPWORDS +
                                              FDDI_ENCAPWORDS));
        tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
        tbridge_mci_sde_header_wr_inline(outputhw, outputsw,
					 &pak->addr_start[IEEEBYTES]);
        break;
#else
    default:
#endif
        return(TBRIDGE_TLB_UNSUPPORTED);
        break;
    }

    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_tring_sde_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build a Token Ring CTR-sde frame.
 *
 * For now, we support 802.10 SDE on CTR only.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_tring_sde_inline (idbtype *outputsw, paktype *pak,
                         const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;
    hwidbtype *outputhw    = outputsw->hwptr;
    ushort datagramsize    = pak->datagramsize;
    tring_hdr *tring;
    uchar canonical_saddr[IEEEBYTES];


    datagramsize += (TRING_8025_ENCAPBYTES + SDE_ENCAPBYTES);

    if (datagramsize > outputhw->max_pak_size)
        return(TBRIDGE_TLB_EXCEEDS_MTU);

    switch (code_switch) {
    case TLB_BFR_TO_BFR: /* rsp-xxx to ctr-sde fwd and fld */
			 /* le-xxx to ltr-sde supported */

        ieee_copy(addr_start->saddr, canonical_saddr);
	ieee_swap(addr_start->daddr, addr_start->daddr);
	ieee_swap(addr_start->saddr, addr_start->saddr);

	tring = (tring_hdr *)pak->datagramstart = 
	    (pak->info_start - (TRING_8025_ENCAPBYTES + SDE_ENCAPBYTES));

	ieee_move(((ether_hdr *)pak->addr_start)->daddr, tring->daddr);
	ieee_move(((ether_hdr *)pak->addr_start)->saddr, tring->saddr);
	tring->ac = AC_PRI0;
	tring->fc = FC_TYPE_LLC;
        tbridge_bfr_sde_header_wr_inline(outputsw, tring->data, 
                                        canonical_saddr);
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:        /* he-xxx to ctr-sde fwd on-cmplx*/
        tbridge_mci_tx1_select_offset_inline(inputhw, pak->encsize -
                                             (TRING_8025_ENCAPWORDS +
                                              SDE_ENCAPWORDS));
        tbridge_mci_tring_mac_wr_inline(inputhw, addr_start);
        tbridge_mci_sde_header_wr_inline(inputhw, outputsw,
					 &pak->addr_start[IEEEBYTES]);
        break;

    case TLB_MCI_TO_MCI:        /* ctr to ctr-sde fwd off-cmplx */
        /*
         * This case was not previously supported.
         */
    case TLB_BFR_TO_MCI:        /* ctr to ctr-sde fld */
        tbridge_mci_tx1_select_offset_inline(outputhw, pak->encsize -
                                             (TRING_8025_ENCAPWORDS +
                                              SDE_ENCAPWORDS));
        tbridge_mci_tring_mac_wr_inline(outputhw, addr_start);
        tbridge_mci_sde_header_wr_inline(outputhw, outputsw,
					 &pak->addr_start[IEEEBYTES]);
        break;
#else
    default:
#endif
        return(TBRIDGE_TLB_UNSUPPORTED);
	break;
    }

    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_hdlc_transit_sde_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build an HDLC-transit-sde frame.
 *
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_hdlc_transit_sde_inline (idbtype *outputsw, paktype *pak,
                                const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;
    enum TBRIDGE_TLB_RETVAL action;

    /*
     * Packet comes in with LLC portion. Call
     * tbridge_to_ether_sde_inline() to pre-pend SDE hdr and
     * MAC hdr.
     */
    switch (code_switch) {

    case TLB_BFR_TO_BFR: /* le-xxx to le-serial-t-sde fwd and fld */
			 /* rsp-xxx to rsp-serial-t-sde fwd and fld */

        action = tbridge_to_ether_sde_inline(outputsw, pak, TLB_BFR_TO_BFR);
        if (action != TBRIDGE_TLB_DONE)
            return(action);
        /*
         * If we're using LAT compression on this frame,
         * copy in the compressed size
         */
        if (outputsw->span_latcompress && (outputsw->span_latsize != -1))
            pak->datagramsize = outputsw->span_latsize;
        /*
         * Adjust our pointers for HDLC encapsulation
         */
	pak->datagramstart -= outputhw->span_encapsize;
        /*
         * Write the 4 byte HDLC bridge header
         */
        PUTLONG(pak->datagramstart, HDLC_BRIDGECODE);
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI: /* he-xxx to he-serial-t-sde fwd on cmplx */
        action = tbridge_to_ether_sde_inline(outputsw, pak, TLB_WITHIN_MCI);
        if (action != TBRIDGE_TLB_DONE)
            return(action);

        tbridge_mci_hdlc_transit_hdr_wr_inline(inputhw, outputhw,
                                               pak->encsize);
        break;

    case TLB_MCI_TO_MCI: /* he-xxx to he-serial-t-sde fwd off cmplx */

        action = tbridge_to_ether_sde_inline(outputsw, pak, TLB_MCI_TO_MCI);
        if (action != TBRIDGE_TLB_DONE)
            return(action);

        tbridge_mci_hdlc_transit_hdr_wr_inline(outputhw, outputhw,
                                               pak->encsize);
        break;

    case TLB_BFR_TO_MCI: /* he-xxx to he-serial-t-sde fld */

        action = tbridge_to_ether_sde_inline(outputsw, pak, TLB_BFR_TO_MCI);
        if (action != TBRIDGE_TLB_DONE)
            return(action);

        tbridge_mci_hdlc_transit_hdr_wr_inline(outputhw, outputhw,
                                               pak->encsize);
        break;
#else
    default:
#endif
        action = TBRIDGE_TLB_UNSUPPORTED;
        return(action);
        break;
    }
    pak->datagramsize += outputhw->span_encapsize;
    return(TBRIDGE_TLB_DONE);
}

/*
 * tbridge_to_fr_ietf_inline
 * Given an input frame from any interface, switch to a frame relay
 * ietf (RFC 1490)
 */
STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_fr_ietf_inline (idbtype *outputsw, hwidbtype *outputhw,
			   paktype *pak, fr_pvc_info_type *pvc,
			   const enum TBRIDGE_TRANSLATE code_switch)
{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    ushort datagramsize    = pak->datagramsize;
    ieee_addrs *addr_start = NULL;
    ushort dlci = pvc->convert_dlci;
#ifdef TBRIDGE_MCI_CBUS
    ushort board_encap;

   /* board encap is on high ends. Note implementation difference
    * between 7000 vs. rsp (rsp follows low end memory path) 
    */
    board_encap = outputhw->encsize - FRAME_RELAY_ENCAPBYTES;
    datagramsize += board_encap;
    board_encap = btow(board_encap);
#endif
    
    switch (tbridge_dlli.src_media_type) {
    /*
     * taking off the transit header means that the rx side
     * doesn't know how to reencapsulate it into a cisco specific
     * transit header.  similar to the original atalk traversing
     * a fddi backbone.
     */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:      /* serial-t to frame relay */
    case TBR_FDDI_TRANSIT:      /* fddi-t to frame relay */

	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

        /*
         * Fall through to the ETHER case.
         */

    case TBR_ETHER:             /* ether to frame-relay */

	/*
	 * datagramsize does NOT include the FR header yet. 
	 * Upate it at the end of the switch.
	 */
	
	/* Not really necessary from ether to fr ietf */
	if ((ulong)(datagramsize + FRAME_RELAY_SNAPENCAPBYTES) >
	    outputhw->max_pak_size)
	    return(TBRIDGE_TLB_EXCEEDS_MTU);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* le-802.3 to le-fr ietf fwd and fld */
	    pak->datagramstart = pak->mac_start - FRAME_RELAY_SNAPENCAPBYTES;
	    tbridge_write_1490_hdr_inline(pak, dlci, FR1490_ETHER);
	    if (outputhw->board_encap) {
	
               /* CSCdi86940:
                * First sync the two values, because pak->datagramsize
                * can change within board_encap function.
                */

                pak->datagramsize = datagramsize;
                (*outputhw->board_encap)(pak, outputhw);
	   
               /*
                * Then guarantee the final write back does the right
                * thing.
                */

                datagramsize = pak->datagramsize;
            }
	    break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: { /* he-802.3 to he-fr ietf fwd on-cmplx */
	    ushort offset;
	    
	    offset = tbridge_dlli.da_offset - board_encap - 
		btow(FRAME_RELAY_SNAPENCAPBYTES); 
	    if (!tbridge_mci_rx_move_select_inline(inputhw,
						   outputhw,
						   offset))
		return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_ETHER);
	    break;
	}	
	case TLB_MCI_TO_MCI: /* he-802.3 to he-fr ietf fwd off-cmplx */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci,
					      FR1490_ETHER);

	    tbridge_mci_to_mci_inline(inputhw, outputhw,
				      tbridge_dlli.da_offset,
				      datagramsize);
	    break;

	case TLB_BFR_TO_MCI: /* he-802.3 to he-fr ietf fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_ETHER);
	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset,
						 datagramsize);
	    } else {
		tbridge_mci_datagram_wr_inline(outputhw, pak, 
					       datagramsize);
	    }
	    break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	datagramsize += FRAME_RELAY_SNAPENCAPBYTES;
	break;

    case TBR_FDDI:              /* fddi to fr ietf */

	/*
	 * datagramsize does NOT include the FR header yet. 
	 * Upate it at the end of the switch.
	 */

        if ((ulong)(datagramsize + FRAME_RELAY_SNAPENCAPBYTES + FR1490_FDDI_PADBYTES) 
	    > outputhw->max_pak_size)
            return(TBRIDGE_TLB_EXCEEDS_MTU);

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* le-fddi to le-fr_ietf fwd and fld */
            addr_start = (ieee_addrs *)pak->addr_start;

            pak->datagramstart = pak->mac_start -
		FRAME_RELAY_SNAPENCAPBYTES - FR1490_FDDI_PADBYTES;

	    tbridge_write_1490_hdr_inline(pak, dlci, FR1490_FDDI);

            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

	    if (outputhw->board_encap) {
	
               /* CSCdi86940:
                * First sync the two values, because pak->datagramsize
                * can change within board_encap function.
                */

                pak->datagramsize = datagramsize;
                (*outputhw->board_encap)(pak, outputhw);
	   
               /*
                * Then guarantee the final write back does the right
                * thing.
                */

                datagramsize = pak->datagramsize;
            }
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI: {    /* he-fddi to he-fr ietf fwd on-cmplx */
	    ushort offset;
            addr_start = (ieee_addrs *)pak->addr_start;
	    offset = tbridge_dlli.mac_offset -
		btow(FRAME_RELAY_SNAPENCAPBYTES) - board_encap;
            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw, offset))
                return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_FDDI);
            /*
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1490 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

	    tbridge_mci_read_select_inline(inputhw, tbridge_dlli.mac_offset +
					   FDDI_FC + (IEEEWORDS * 2));
	}
            break;

        case TLB_MCI_TO_MCI:    /* he-fddi to he-fr ietf fwd off-cmplx */
            addr_start = (ieee_addrs *)pak->addr_start;

            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);
            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_FDDI);
            /*
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1490 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

            tbridge_mci_to_mci_inline(inputhw, outputhw,
                                      tbridge_dlli.mac_offset + FDDI_FC +
				      (IEEEWORDS * 2),
				      datagramsize - FDDI_ENCAPBYTES);
            break;

        case TLB_BFR_TO_MCI: {   /* he-fddi to he-fr ietf fld */
	    ieee_addrs addr_copy;
            mciregtype *mci_regptr = outputhw->mci_regptr;

	    addr_start = (ieee_addrs *)pak->addr_start;

            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_FDDI);
            /*
             * CSCdi23653
             * We cannot just use tbridge_mci_datagram_wr_inline to copy
             * the packet to the output interface since datagramstart
             * points directly at the FC byte.  We need to short-word
             * align the FC byte, then write out the rest of the packet.
             * So we do this in two steps.
             *
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1490 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_copy.daddr);
            ieee_swap(addr_start->saddr, addr_copy.saddr);

	    mci_regptr->write1short = FDDI_LLC_FC_BYTE;
            mci_regptr->write1long = *((ulong *)&addr_copy.daddr[0]);
            mci_regptr->write1long = *((ulong *)&addr_copy.daddr[4]);
            mci_regptr->write1long = *((ulong *)&addr_copy.saddr[2]);

	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 FDDI_ENCAPWORDS,
						 datagramsize -
						 FDDI_ENCAPBYTES);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
	}
            break;
#else
        default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	/*
	 * add an extra byte for padding here - it's part of rfc 1490
	 */
        datagramsize += FRAME_RELAY_SNAPENCAPBYTES + FR1490_FDDI_PADBYTES;
	break;

    case TBR_TR:                /* ltr to le-fr ietf */
	addr_start = (ieee_addrs *)pak->addr_start;

        datagramsize += FRAME_RELAY_SNAPENCAPBYTES;

        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* ltr to le-fr ietf fwd and fld */

            pak->datagramstart = pak->mac_start - FRAME_RELAY_SNAPENCAPBYTES;

	    tbridge_write_1490_hdr_inline(pak, dlci, FR1490_802_5);
	    if (outputhw->board_encap) {
	
               /* CSCdi86940:
                * First sync the two values, because pak->datagramsize
                * can change within board_encap function.
                */

                pak->datagramsize = datagramsize;
                (*outputhw->board_encap)(pak, outputhw);
	   
               /*
                * Then guarantee the final write back does the right
                * thing.
                */

                datagramsize = pak->datagramsize;
            }
            break;

        /* case NOT SUPPORTED */
        case TLB_WITHIN_MCI:    /* ltr is not an mci interface */
        case TLB_MCI_TO_MCI:    /* ltr is not an mci interface */
        case TLB_BFR_TO_MCI:    /* ltr to he-fr ietf not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
    break;

#ifdef TBRIDGE_MCI_CBUS
    case TBR_CTR:               /* ctr to he-fr ietf */

	/*
	 * datagramsize does NOT include the FR header yet. 
	 * Upate it at the end of the switch.
	 */

        switch (code_switch) {

        case TLB_WITHIN_MCI: {    /* ctr to he-fddi fwd on-cmplx */
	    ushort offset;
	    offset = tbridge_dlli.mac_offset -
		btow(FRAME_RELAY_SNAPENCAPBYTES) - board_encap;
            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw, offset))
		return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_802_5);
	}
            break;

        case TLB_MCI_TO_MCI:    /* ctr to he-fr ietf fwd off-cmplx */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_802_5);

            tbridge_mci_to_mci_inline(inputhw,
				      outputhw,
                                      tbridge_dlli.mac_offset,
				      datagramsize);

            break;

        case TLB_BFR_TO_MCI:    /* ctr to he-fr ietf fld */
	    if (!tbridge_mci_tx1_select_inline(outputhw))
	        return(TBRIDGE_TLB_NO_TXBUF);

	    /*
	     * when copied to the buffer, the address are in
	     * canonical order.  so...
	     * flip the addresses back to non-canonical order
	     * this is to give the illusion on the other side
	     * of this fr ietf virtual circuit that the packet
	     * came off a token ring.
	     */
	    addr_start = (ieee_addrs *)pak->addr_start;
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_fr_ietf_hdr_wr_inline(outputhw, dlci, FR1490_802_5);

            tbridge_mci_tr_mac_wr_inline(outputhw, pak);
	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 TRING_8025_ENCAPWORDS,
						 datagramsize -
						 TRING_8025_ENCAPBYTES);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
            break;

        /* case NOT SUPPORTED */
        case TLB_BFR_TO_BFR:    /* ctr to le-fr ietf not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
        datagramsize += FRAME_RELAY_SNAPENCAPBYTES;
	break;
#endif
    default:
        return(TBRIDGE_TLB_UNSUPPORTED);
        break;

    }

    pak->datagramsize = datagramsize;
    /*
     * Handle pvc accounting here
     */
    INC_PVC_FAST_OUTPUT(pvc, pak->datagramsize);
    return(TBRIDGE_TLB_DONE);
}

/*
 * tbridge_to_fr_cisco_inline
 * Write a cisco frame relay header onto a bridged frame
 */
STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_fr_cisco_inline (idbtype *outputsw, hwidbtype *outputhw,
			    paktype *pak, fr_pvc_info_type *pvc,
			    const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    fr_idb_struct_type *fr_idb;
    ushort dlci = pvc->convert_dlci;

    enum TBRIDGE_TLB_RETVAL action;

    /*
     * Same basic idea as hdlc - convert everything to ether, and
     * slap on the encapsulation.
     *
     * Determine (to within a few bytes) whether the resulting
     * transit encapsulated packet would exceed the ETHER MTU
     * before trying to translate it to ether for a more exact
     * size.
     */
    if ((ulong)((pak->datagramsize - tbridge_dlli.transit_encapsize) +
	outputhw->span_encapsize) > outputhw->max_pak_size)
	return(TBRIDGE_TLB_EXCEEDS_MTU);

    fr_idb = outputhw->frame_relay_stuff;
    switch (code_switch) {

    case TLB_BFR_TO_BFR: {	/* le-xxx to le-frame relay fwd and fld */
	frame_relay_hdr *hdr;
	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);
	/*
	 * Adjust our pointers for Frame relay encapsulation
	 */
	pak->datagramstart -= FRAME_RELAY_ENCAPBYTES;
	hdr = (frame_relay_hdr *)pak->datagramstart;
	/*
	 * Write the 4 byte frame relay bridge header
	 */
	PUTSHORT(&hdr->dlci, dlci);
	PUTSHORT(&hdr->type, TYPE_BRIDGE);
	if (outputhw->board_encap)
	    (*outputhw->board_encap)(pak, outputhw);
	}
        break;

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:	/* he-xxx to he-frame relay-t fwd on cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fr_cisco_hdr_wr_inline(inputhw, outputhw,
					   dlci, pak->encsize);
	break;

    case TLB_MCI_TO_MCI:	/* he-xxx to he-frame relay-t fwd off cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fr_cisco_hdr_wr_inline(outputhw, outputhw,
					   dlci, TBRIDGE_MCI_TX1_OFFSET);
	break;

    case TLB_BFR_TO_MCI:	/* he-xxx to he-serial-t fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_fr_cisco_hdr_wr_inline(outputhw, outputhw,
					   dlci, TBRIDGE_MCI_TX1_OFFSET);
	break;
#else
    default:
#endif
	action = TBRIDGE_TLB_UNSUPPORTED;
	return(action);
	break;
    }

    pak->datagramsize += outputhw->span_encapsize;
    INC_PVC_FAST_OUTPUT(pvc, pak->datagramsize);
    return(TBRIDGE_TLB_DONE);
}


STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_frame_relay_inline (idbtype *outputsw,
			       hwidbtype *outputhw,
			       paktype *pak,
			       fr_pvc_info_type *pvc,
			       const enum TBRIDGE_TRANSLATE code_switch)

{
    /*
     * Call the correct output routine, based on whether this PVC
     * is a cisco style or ietf style
     */
    if (pvc->bridge_encap == FR_ENCAPS_IETF) {
	return(tbridge_to_fr_ietf_inline(outputsw, outputhw, pak,
					 pvc, code_switch));
    } 
    return(tbridge_to_fr_cisco_inline(outputsw, outputhw, pak, 
				      pvc, code_switch));
}

/*
 * tbridge_to_smds_inline
 * Given an input frame from any interface, switch to a smds interface
 */
STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_smds_inline (idbtype *outputsw, hwidbtype *outputhw,
			paktype *pak, uchar *smds_addr,
			const enum TBRIDGE_TRANSLATE code_switch)
{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    ushort datagramsize    = pak->datagramsize;
    ieee_addrs *addr_start = NULL;
    
    switch (tbridge_dlli.src_media_type) {
    /*
     * taking off the transit header means that the rx side
     * doesn't know how to reencapsulate it into a cisco specific
     * transit header.  similar to the original atalk traversing
     * a fddi backbone.
     */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:      /* serial-t to smds */
    case TBR_FDDI_TRANSIT:      /* fddi-t to smds */

	datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

        /*
         * Fall through to the ETHER case.
         */

    case TBR_ETHER:             /* ether to frame-relay */

	/*
	 * datagramsize does NOT include the header yet. 
	 * Update it at the end of the switch.
	 */
	
	/* Not really necessary from ether to smds */
	if ((ulong)((datagramsize + outputhw->encsize +
	     SMDS_BRIDGE_PAD_SIZE + SNAP_HDRBYTES)) >
	    outputhw->max_pak_size)
	    return(TBRIDGE_TLB_EXCEEDS_MTU);

	switch (code_switch) {

	case TLB_BFR_TO_BFR: /* le-802.3 to le-smds fwd and fld */
	    pak->datagramstart = pak->mac_start;
	    datagramsize = tbridge_write_smds_inline(pak, outputhw, outputsw, 
						     smds_addr, datagramsize,
						     SMDS_BRIDGE_PID_8023);
	    if (outputhw->board_encap) {
		/*
		 * first sync these two values, since the value
		 * of pak->datagramstart might get changed within
		 */
		pak->datagramsize = datagramsize;
		(*outputhw->board_encap)(pak, outputhw);
		/*
		 * guarantee the final write back does the right
		 * thing
		 */
		datagramsize = pak->datagramsize;
	    }
	    break;

#ifdef TBRIDGE_MCI_CBUS
	case TLB_WITHIN_MCI: { /* he-802.3 to he-smds fwd on-cmplx */
	    ushort offset, trail_offset;
	    ulong last_long;
	    mciregtype *regptr;
	    
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    trail_offset = tbridge_dlli.da_offset - 1 +
		((datagramsize & ~0x3) >> 1);
	    regptr = outputhw->mci_regptr;
	    regptr->argreg = trail_offset;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    last_long = regptr->readlong;
	    if (!tbridge_mci_rx_move_select_inline(inputhw,
						   outputhw,
						   trail_offset))
		return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_smds_trailer_wr_inline(regptr,last_long);
	    offset = tbridge_dlli.da_offset - btow(outputhw->encsize) -
		btow(SMDS_BRIDGE_PAD_SIZE);
	    regptr->argreg = offset;
	    regptr->cmdreg = MCI_CMD_TX1_SELECT;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr, 
					   smds_addr, SMDS_BRIDGE_PID_8023);
	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	    break;
	}	
	case TLB_MCI_TO_MCI: { /* he-802.3 to he-smds fwd off-cmplx */
	    ushort copy_size;
	    mciregtype *srcreg, *dstreg;
	    ulong last_long;
	    
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    copy_size = (datagramsize & (~0x03)) - SMDS_BRIDGE_PAD_SIZE;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, 
					   outputhw->mci_regptr, smds_addr, 
					   SMDS_BRIDGE_PID_8023);

	    srcreg = inputhw->mci_regptr;
	    if (copy_size & 0x2) {
		/* Means we have an extra word - we want to be
		 * exact to the last_long - mci2mci does longs
		 */
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset,
					  copy_size - 2);
		dstreg = outputhw->mci_regptr;
		dstreg->write1short = srcreg->readshort;
	    } else {
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset,
					  copy_size);
	    }
	    last_long = srcreg->readlong;
	    tbridge_mci_smds_trailer_wr_inline(outputhw->mci_regptr,
					       last_long);
	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	}
	    break;

	case TLB_BFR_TO_MCI:
	    { /* he-802.3 to he-smds fld */
		ulong last_long;
		ushort copy_size;
		mciregtype *regptr;

		datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
		copy_size = (datagramsize & (~0x03)) - SMDS_BRIDGE_PAD_SIZE;
	    
		if (inputsw->span_ptr->cbus_copy_capable) {
		    last_long = smds_cbus_copy_last_long;
		} else {
		    last_long = GETLONG(pak->mac_start + copy_size);
		}

		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);

		regptr = outputhw->mci_regptr;
		tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr,
					       smds_addr,
					       SMDS_BRIDGE_PID_8023);

		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset,
						     copy_size);
		} else {
		    tbridge_mci_datagram_wr_inline(outputhw, pak, 
						   copy_size);
		}
		tbridge_mci_smds_trailer_wr_inline(regptr, last_long);
		datagramsize = 
		    tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	    }
	    break;
#else
	default:
#endif
	    return(TBRIDGE_TLB_UNSUPPORTED);
	    break;
	}
	break;

    case TBR_FDDI:              /* fddi to smds */

	/*
	 * datagramsize does NOT include the smds header yet. 
	 * Upate it at the end of the switch.
	 */

        if ((ulong)(datagramsize + outputhw->encsize + SMDS_BRIDGE_PAD_FDDI_SIZE)
	    > outputhw->max_pak_size)
            return(TBRIDGE_TLB_EXCEEDS_MTU);

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* le-fddi to le-smds fwd and fld */
            addr_start = (ieee_addrs *)pak->addr_start;

	    /* extra fddi pad byte */
	    pak->datagramstart = pak->mac_start - SMDS_BRIDGE_PAD_FDDI_EXTRA;
	    *pak->datagramstart = SMDS_BRIDGE_PAD_DATA;
	    datagramsize++;
	    datagramsize = tbridge_write_smds_inline(pak, outputhw, outputsw, 
						     smds_addr, datagramsize,
						     SMDS_BRIDGE_PID_FDDI);
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

	    if (outputhw->board_encap) {
                /*
                 * first sync these two values, since the value
                 * of pak->datagramstart might get changed within
                 */
                pak->datagramsize = datagramsize;
                (*outputhw->board_encap)(pak, outputhw);
	   
                /*
                 * guarantee the final write back does the right
                 * thing
                 */
                datagramsize = pak->datagramsize;
            }
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI: {    /* he-fddi to he-smds fwd on-cmplx */
	    ushort offset, trail_offset;
	    ulong last_long;
	    mciregtype *regptr;

	    /* Fddi pad is 3 bytes, not 2 */
	    datagramsize++;
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    
	    trail_offset = tbridge_dlli.mac_offset - 1 +
		((datagramsize & ~0x3) >> 1);
	    regptr = outputhw->mci_regptr;
	    regptr->argreg = trail_offset;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    last_long = regptr->readlong;
	    if (!tbridge_mci_rx_move_select_inline(inputhw,
						   outputhw,
						   trail_offset))
		return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_smds_trailer_wr_inline(regptr,last_long);
	    offset = tbridge_dlli.mac_offset - btow(outputhw->encsize) -
		btow(SMDS_BRIDGE_PAD_SIZE);
	    regptr->argreg = offset;
	    regptr->cmdreg = MCI_CMD_TX1_SELECT;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr, 
					   smds_addr,
					   SMDS_BRIDGE_PID_FDDI);
            /*
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1490 packets non canonical
	     */
	    addr_start = (ieee_addrs *)pak->addr_start;
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_fddi_mac_wr_inline(outputhw, pak);

	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	}
            break;

        case TLB_MCI_TO_MCI: {   /* he-fddi to he-smds fwd off-cmplx */
	    ushort copy_size;
	    mciregtype *srcreg, *dstreg;
	    ulong last_long;
            addr_start = (ieee_addrs *)pak->addr_start;
	    
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    copy_size = ((datagramsize - FDDI_ENCAPBYTES) & (~0x03)) -
		SMDS_BRIDGE_PAD_SIZE;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, 
					   outputhw->mci_regptr, smds_addr, 
					   SMDS_BRIDGE_PID_FDDI);

            /*
	     * swap mac addrs to make the mac address of fddi
	     * packets encap'ed in rfc 1490 packets non canonical
	     */
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);

            tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
	    srcreg = inputhw->mci_regptr;
	    copy_size -= FDDI_ENCAPBYTES;
	    if (copy_size & 0x2) {
		/* Means we have an extra word - we want to be
		 * exact to the last_long - mci2mci does longs
		 */
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  btow(FDDI_ENCAPBYTES),
					  copy_size - 2);
		dstreg = outputhw->mci_regptr;
		dstreg->write1short = srcreg->readshort;
	    } else {
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset +
					  btow(FDDI_ENCAPBYTES),
					  copy_size);
	    }
	    last_long = srcreg->readlong;
	    tbridge_mci_smds_trailer_wr_inline(outputhw->mci_regptr,
					       last_long);
	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	}
            break;

        case TLB_BFR_TO_MCI:    /* he-fddi to he-smds fld */
	    { /* he-802.3 to he-smds fld */
		ulong last_long;
		ushort copy_size/* , trail_offset */;
		mciregtype *regptr;

		/* fddi header is 3 bytes */
		datagramsize++;
		datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
		copy_size = (datagramsize & (~0x03)) -
		    SMDS_BRIDGE_PAD_SIZE;
	    
		if (inputsw->span_ptr->cbus_copy_capable) {
		    last_long = smds_cbus_copy_last_long;
		} else {
		    last_long = GETLONG(pak->mac_start + copy_size);
		}

		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);

		regptr = outputhw->mci_regptr;
		tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr,
					       smds_addr,
					       SMDS_BRIDGE_PID_FDDI);

		/*
		 * CSCdi23653
		 * We cannot just use tbridge_mci_datagram_wr_inline to copy
		 * the packet to the output interface since datagramstart
		 * points directly at the FC byte.  We need to short-word
		 * align the FC byte, then write out the rest of the packet.
		 * So we do this in two steps.
		 *
		 * swap mac addrs to make the mac address of fddi
		 * packets encap'ed in rfc 1490 packets non canonical
		 */
		addr_start = (ieee_addrs *)pak->addr_start;
		ieee_swap(addr_start->daddr, addr_start->daddr);
		ieee_swap(addr_start->saddr, addr_start->saddr);

		tbridge_mci_fddi_mac_wr_inline(outputhw, pak);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     FDDI_ENCAPWORDS,
						     copy_size -
						     (FDDI_ENCAPBYTES + 1));
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
		tbridge_mci_smds_trailer_wr_inline(regptr, last_long);
		datagramsize = 
		    tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	    }
            break;
#else
        default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	break;

    case TBR_TR:                /* ltr to le-smds */
	addr_start = (ieee_addrs *)pak->addr_start;

        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* ltr to le-smds fwd and fld */
	    pak->datagramstart = pak->mac_start;
	    datagramsize = tbridge_write_smds_inline(pak, outputhw, outputsw, 
						     smds_addr, datagramsize,
						     SMDS_BRIDGE_PID_8025);
	    if (outputhw->board_encap) {
                /*
                 * first sync these two values, since the value
                 * of pak->datagramstart might get changed within
                 */
                pak->datagramsize = datagramsize;
                (*outputhw->board_encap)(pak, outputhw);
                /*
                 * guarantee the final write back does the right
                 * thing
                 */
                datagramsize = pak->datagramsize;
            }
            break;

        /* case NOT SUPPORTED */
        case TLB_WITHIN_MCI:    /* ltr is not an mci interface */
        case TLB_MCI_TO_MCI:    /* ltr is not an mci interface */
        case TLB_BFR_TO_MCI:    /* ltr to he-smds not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
    break;

#ifdef TBRIDGE_MCI_CBUS
    case TBR_CTR:               /* ctr to he-smds */
        switch (code_switch) {
	case TLB_WITHIN_MCI: { /* he-802.5 to he-smds fwd on-cmplx */
	    ushort offset, trail_offset;
	    ulong last_long;
	    mciregtype *regptr;
	    
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    trail_offset = tbridge_dlli.mac_offset - 1 +
		((datagramsize & ~0x3) >> 1);
	    regptr = outputhw->mci_regptr;
	    regptr->argreg = trail_offset;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    last_long = regptr->readlong;
	    if (!tbridge_mci_rx_move_select_inline(inputhw,
						   outputhw,
						   trail_offset))
		return(TBRIDGE_TLB_NO_TXBUF);

	    tbridge_mci_smds_trailer_wr_inline(regptr,last_long);
	    offset = tbridge_dlli.mac_offset - btow(outputhw->encsize) -
		btow(SMDS_BRIDGE_PAD_SIZE);
	    regptr->argreg = offset;
	    regptr->cmdreg = MCI_CMD_TX1_SELECT;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr, 
					   smds_addr,
					   SMDS_BRIDGE_PID_8025);
	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	}	
	    break;
	    
	case TLB_MCI_TO_MCI: { /* he-802.5 to he-smds fwd off-cmplx */
	    ushort copy_size;
	    mciregtype *srcreg, *dstreg;
	    ulong last_long;
	    
	    if (!tbridge_mci_tx1_select_inline(outputhw))
		return(TBRIDGE_TLB_NO_TXBUF);
	    datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
	    copy_size = (datagramsize & (~0x03)) - SMDS_BRIDGE_PAD_SIZE;
	    tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, 
					   outputhw->mci_regptr, smds_addr, 
					   SMDS_BRIDGE_PID_8025);

	    srcreg = inputhw->mci_regptr;
	    if (copy_size & 0x2) {
		/* Means we have an extra word - we want to be
		 * exact to the last_long - mci2mci does longs
		 */
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset,
					  copy_size - 2);
		dstreg = outputhw->mci_regptr;
		dstreg->write1short = srcreg->readshort;
	    } else {
		tbridge_mci_to_mci_inline(inputhw, outputhw,
					  tbridge_dlli.mac_offset,
					  copy_size);
	    }
	    last_long = srcreg->readlong;
	    tbridge_mci_smds_trailer_wr_inline(outputhw->mci_regptr,
					       last_long);
	    datagramsize =
		tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	}

            break;

        case TLB_BFR_TO_MCI:    /* ctr to he-smds fld */
	    { /* he-802.3 to he-smds fld */
		ulong last_long;
		ushort copy_size/* , trail_offset */;
		mciregtype *regptr;

		datagramsize = tbridge_mci_smds_setup(datagramsize, outputhw);
		copy_size = (datagramsize & (~0x03)) - SMDS_BRIDGE_PAD_SIZE;
	    
		if (inputsw->span_ptr->cbus_copy_capable) {
		    last_long = smds_cbus_copy_last_long;
		} else {
		    last_long = GETLONG(pak->mac_start + copy_size);
		}

		if (!tbridge_mci_tx1_select_inline(outputhw))
		    return(TBRIDGE_TLB_NO_TXBUF);

		regptr = outputhw->mci_regptr;
		tbridge_mci_smds_hdr_wr_inline(outputhw, outputsw, regptr,
					       smds_addr,
					       SMDS_BRIDGE_PID_8025);

		/*
		 * CSCdi23653
		 * We cannot just use tbridge_mci_datagram_wr_inline to copy
		 * the packet to the output interface since datagramstart
		 * points directly at the FC byte.  We need to short-word
		 * align the FC byte, then write out the rest of the packet.
		 * So we do this in two steps.
		 *
		 * swap mac addrs to make the mac address of fddi
		 * packets encap'ed in smds packets non canonical
		 */
		addr_start = (ieee_addrs *)pak->addr_start;
		ieee_swap(addr_start->daddr, addr_start->daddr);
		ieee_swap(addr_start->saddr, addr_start->saddr);


		tbridge_mci_tr_mac_wr_inline(outputhw, pak);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.mac_offset +
						     TRING_8025_ENCAPWORDS,
						     copy_size -
						     TRING_8025_ENCAPBYTES);
		} else {
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
		tbridge_mci_smds_trailer_wr_inline(regptr, last_long);
		datagramsize = 
		    tbridge_smds_adjust_datagramsize(datagramsize, outputhw);
	    }
            break;

        /* case NOT SUPPORTED */
        case TLB_BFR_TO_BFR:    /* ctr to le-smds not supported */
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
	break;
#endif
    default:
        return(TBRIDGE_TLB_UNSUPPORTED);
        break;

    }

    pak->datagramsize = datagramsize;
    /*
     * Handle pvc accounting here
     */
    return(TBRIDGE_TLB_DONE);
}


/*
 * tbridge_to_llc_inline
 *
 * Prepare for pre-pending SDE header.
 * Take care of LLC-layer data.
 * Use pak->info_start(low-end and rsp) or pak->encsize(high-end) to record
 * where the LLC data begins. Caller then use this pointer to prepend
 * headers.
 *
 * ARPA from ETHER needs to be encapsulated into SNAP for any kind of
 * SDE media.
 *
 * case TLB_BFR_TO_BFR: use pak->addr_start to record where the
 * MAC addresses in case we have to move it before prepending
 * SNAP hdr.
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_llc_inline (idbtype *outputsw, paktype *pak,
                        const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;
    ushort datagramsize    = pak->datagramsize;
    ushort sde_encapwords = pak->encsize = 0;

    ether_hdr *ether;
    snap_hdr *snap;
    ulong snap_ctl_oui = SNAP_CTL_OUI;

    switch (tbridge_dlli.src_media_type) {

    case TBR_HDLC_TRANSIT_SDE:  /* serial-t-sde to sde */

        /* Fall through */

    case TBR_LEX_TRANSIT:
    case TBR_HDLC_TRANSIT:      /* serial-t to sde */
    case TBR_FDDI_TRANSIT:      /* fddi-t to sde */

        datagramsize -= tbridge_dlli.transit_encapsize;

    case TBR_ETHER_ISL_VLAN:    /* ISL vLAN header removed at this stage,
                                   leaving interior Ethernet format packet */

        /*
         * Fall through to the ETHER case.
         */

    case TBR_ETHER_SDE:         /* ether-sde to sde */

	if ((TBR_HDLC_TRANSIT_SDE == tbridge_dlli.src_media_type) ||
	    (TBR_ETHER_SDE == tbridge_dlli.src_media_type)) {
            datagramsize -= SDE_ENCAPBYTES;
            sde_encapwords = pak->encsize = SDE_ENCAPWORDS;
	}

        /* Fall through */

    case TBR_ETHER:             /* ether to sde */

        if (pak->info_start) {
            /*
             * The outgoing frame encapsulation is LLC + possible SNAP.
             */
            datagramsize  -= ETHER_8023_ENCAPBYTES;

            switch (code_switch) {

            case TLB_BFR_TO_BFR:        /* le-802.3 to le-sde fwd and fld */
					/* rsp-802.3 to rsp-sde fwd and fld */
                pak->info_start = (pak->mac_start + ETHER_8023_ENCAPBYTES +
                                   (sde_encapwords * 2));
                break;

#ifdef TBRIDGE_MCI_CBUS
            case TLB_WITHIN_MCI:        /* he-802.3 to he-sde fwd on-cmplx */
                pak->encsize = (tbridge_dlli.da_offset +
                                ETHER_8023_ENCAPWORDS +
                                sde_encapwords);

                if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
                    return(TBRIDGE_TLB_NO_TXBUF);
                break;

            case TLB_MCI_TO_MCI:        /* he-802.3 to he-sde fwd off-cmplx */
                if (!tbridge_mci_tx1_select_inline(outputhw))
                    return(TBRIDGE_TLB_NO_TXBUF);
                tbridge_mci_to_mci_inline(inputhw, outputhw,
                                          tbridge_dlli.da_offset +
                                          ETHER_8023_ENCAPWORDS +
                                          sde_encapwords,
                                          datagramsize);
                pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
                break;

            case TLB_BFR_TO_MCI:        /* he-802.3 to he-sde fld */
                if (!tbridge_mci_tx1_select_inline(outputhw))
                    return(TBRIDGE_TLB_NO_TXBUF);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_8023_ENCAPWORDS +
						     sde_encapwords,
						     datagramsize);
		} else {
		    /*
		     * pak->info_start already points to native LLC for
		     * normal or SDE-encap pkt.
		     */
		    tbridge_mci_llcdata_wr_inline(outputhw, pak);
		}
		pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
		break;
#else
	    default:
#endif
                return(TBRIDGE_TLB_UNSUPPORTED);
                break;
            }
        } else {
            /*
             * ARPA encapsulation on ETHER is always translated to
             * SNAP encapsulation on SDE.
             *
             * SDE-encap packet won't never get to here since
             * pak->info_start is set.
             */
            datagramsize  += (SNAP_HDRBYTES - ETHER_ARPA_ENCAPBYTES);

	    /*
	     * Old style abuse of SNAP-F8. Should use SNAP_CTL_OUI.
	     */
	    if (outputhw->status & IDB_TR)
		snap_ctl_oui = outputhw->tr_transit_oui;

            switch (code_switch) {

            case TLB_BFR_TO_BFR:        /* le-ARPA to le-sde fwd and fld */
					/* rsp-ARPA to rsp-sde fwd and fld */

                pak->info_start = (pak->mac_start + ETHER_ARPA_ENCAPBYTES -
				   SNAP_HDRBYTES);
                /*
                 * Use pak->addr_start to record where the new MAC address are.
                 * Don't bother to move the 'length' since we have this
                 * data with us already(datagramsize).
                 */
                ether = (ether_hdr *)pak->addr_start =
                    pak->mac_start + ETHER_ARPA_ENCAPBYTES -
			(SNAP_HDRBYTES + ETHER_8023_ENCAPBYTES);
		
                ieee_move(((ether_hdr *)pak->mac_start)->daddr, ether->daddr);
                ieee_move(((ether_hdr *)pak->mac_start)->saddr, ether->saddr);

                /*
                 * No need to explicitly write the TYPE bytes;
                 * they are already in the proper location in the
                 * original packet.
                 */
                snap = (snap_hdr *)ether->data;
                PUTSHORT(&snap->dsap, SNAPSNAP);
		/* Low-end code doesn't use hwidb->tr_transit_oui */
                PUTLONG(&snap->control, snap_ctl_oui);
                break;

#ifdef TBRIDGE_MCI_CBUS
            case TLB_WITHIN_MCI:        /* he-ARPA to he-sde fwd on-cmplx */
                pak->encsize = (tbridge_dlli.da_offset +
                                ETHER_ARPA_ENCAPWORDS -
                                (SNAP_HDRBYTES /2));

                if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						       pak->encsize))
                    return(TBRIDGE_TLB_NO_TXBUF);

                tbridge_mci_snap_wr_inline(inputhw, snap_ctl_oui,
                                           ((ether_hdr *)
                                            pak->addr_start)->type_or_len);
                break;

            case TLB_MCI_TO_MCI:        /* he-ARPA to he-sde fwd off-cmplx */
                if (!tbridge_mci_tx1_select_inline(outputhw))
                    return(TBRIDGE_TLB_NO_TXBUF);

                tbridge_mci_snap_wr_inline(outputhw, snap_ctl_oui,
                                           ((ether_hdr *)
                                            pak->addr_start)->type_or_len);
                tbridge_mci_to_mci_inline(inputhw, outputhw,
                                          tbridge_dlli.da_offset +
                                          ETHER_ARPA_ENCAPWORDS,
                                          datagramsize - SNAP_HDRBYTES);
                pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
                break;

            case TLB_BFR_TO_MCI:        /* he-ARPA to he-sde fld */
                if (!tbridge_mci_tx1_select_inline(outputhw))
                    return(TBRIDGE_TLB_NO_TXBUF);

                tbridge_mci_snap_wr_inline(outputhw, snap_ctl_oui,
                                           ((ether_hdr *)
                                            pak->addr_start)->type_or_len);
		if (inputsw->span_ptr->cbus_copy_capable) {
		    tbridge_cbus_copy_rx_data_inline(outputhw,
						     tbridge_dlli.da_offset +
						     ETHER_ARPA_ENCAPWORDS,
						     datagramsize -
						     SNAP_HDRBYTES);
		} else {
		    tbridge_mci_netdata_wr_inline(outputhw, pak);
		}
#else
	    default:
#endif
                return(TBRIDGE_TLB_UNSUPPORTED);
                break;
            }
        }
        break;

    case TBR_FDDI_SDE:         /* fddi-sde to sde */

            datagramsize -= SDE_ENCAPBYTES;
            sde_encapwords = pak->encsize = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_FDDI:              /* fddi to sde */

        datagramsize -= FDDI_ENCAPBYTES;

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* le-fddi to le-sde fwd and fld */
				/* rsp-fddi to rsp-sde fwd and fld */
            pak->info_start = (pak->mac_start + FDDI_ENCAPBYTES +
                               (sde_encapwords * 2));
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI:    /* he-fddi to he-sde fwd on-cmplx */
            pak->encsize = tbridge_dlli.mac_offset + FDDI_ENCAPWORDS + 
		sde_encapwords;

            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   pak->encsize))
                return(TBRIDGE_TLB_NO_TXBUF);
            break;

        case TLB_MCI_TO_MCI:    /* he-fddi to he-sde fwd off-cmplx */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);
            tbridge_mci_to_mci_inline(inputhw, outputhw,
                                      tbridge_dlli.mac_offset +
				      FDDI_ENCAPWORDS + sde_encapwords,
                                      datagramsize);
            pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
            break;

        case TLB_BFR_TO_MCI:    /* he-fddi to he-sde fld */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);
	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 FDDI_ENCAPWORDS +
						 sde_encapwords,
						 datagramsize);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }
	    pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
            break;
#else
	default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
        break;

    case TBR_TR_SDE:         /* ltr-sde to sde */

            datagramsize -= SDE_ENCAPBYTES;
            sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_TR:                /* ltr to le-sde */
        datagramsize  -= TRING_8025_ENCAPBYTES;

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* ltr to le-sde fwd and fld */
				/* ltr not supported on rsp */
            pak->info_start = (pak->mac_start + TRING_8025_ENCAPBYTES +
                               (sde_encapwords * 2));
            break;

#ifdef TBRIDGE_MCI_CBUS
        /* case NOT SUPPORTED */
        case TLB_WITHIN_MCI:    /* ltr is not an mci interface */
        case TLB_MCI_TO_MCI:    /* ltr is not an mci interface */
        case TLB_BFR_TO_MCI:    /* ltr to he-fddi not supported */
#else
	default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
        break;

    case TBR_CTR_SDE:         /* ctr-sde to sde */

            datagramsize -= SDE_ENCAPBYTES;
            sde_encapwords = SDE_ENCAPWORDS;

        /* Fall through */

    case TBR_CTR:               /* ctr to he-sde */

        datagramsize  -= TRING_8025_ENCAPBYTES;

        switch (code_switch) {

        case TLB_BFR_TO_BFR:    /* ctr to rsp-sde fwd and fld */
				/* ctr not supported on le */
            pak->info_start = (pak->mac_start + TRING_8025_ENCAPBYTES +
                               (sde_encapwords * 2));
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TLB_WITHIN_MCI:    /* ctr to he-sde fwd on-cmplx */
            pak->encsize = (tbridge_dlli.mac_offset + TRING_8025_ENCAPWORDS +
                            sde_encapwords);

            if (!tbridge_mci_rx_move_select_inline(inputhw, outputhw,
						   pak->encsize))
                return(TBRIDGE_TLB_NO_TXBUF);

            break;

        case TLB_MCI_TO_MCI:    /* ctr to he-sde fwd off-cmplx */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

            tbridge_mci_to_mci_inline(inputhw, outputhw,
                                      tbridge_dlli.mac_offset +
                                      TRING_8025_ENCAPWORDS +
                                      sde_encapwords,
                                      datagramsize);
            pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
            break;

        case TLB_BFR_TO_MCI:    /* ctr to he-sde fld */
            if (!tbridge_mci_tx1_select_inline(outputhw))
                return(TBRIDGE_TLB_NO_TXBUF);

            pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
	    if (inputsw->span_ptr->cbus_copy_capable) {
		tbridge_cbus_copy_rx_data_inline(outputhw,
						 tbridge_dlli.mac_offset +
						 TRING_8025_ENCAPWORDS +
						 sde_encapwords,
						 datagramsize);
	    } else {
		tbridge_mci_llcdata_wr_inline(outputhw, pak);
	    }		
            break;
#else
	default:
#endif
            return(TBRIDGE_TLB_UNSUPPORTED);
            break;
        }
        break;

    default:
        return(TBRIDGE_TLB_UNSUPPORTED);
    }

    /*
     * LLC(possible SNAP) size.
     */
    pak->datagramsize  = datagramsize;

    return(TBRIDGE_TLB_DONE);
}

/*
 * tbridge_to_lex_transit_inline
 *
 * Given an input frame from any interface on which we support bridging,
 * build an lex-transit frame.
 *
 */

STATIC INLINE
enum TBRIDGE_TLB_RETVAL
tbridge_to_lex_transit_inline (idbtype *outputsw, paktype *pak,
				const enum TBRIDGE_TRANSLATE code_switch)

{
#ifdef TBRIDGE_MCI_CBUS
    idbtype *inputsw       = pak->if_input;
    hwidbtype *inputhw     = inputsw->hwptr;
#endif
    hwidbtype *outputhw    = outputsw->hwptr;

    enum TBRIDGE_TLB_RETVAL action;

    /*
     * Determine (to within a few bytes) whether the resulting
     * transit encapsulated packet would exceed the ETHER MTU
     * before trying to translate it to ether for a more exact
     * size.
     */
    if ((ulong)((pak->datagramsize - tbridge_dlli.transit_encapsize) +
	outputhw->span_encapsize) > outputhw->max_pak_size)
	return(TBRIDGE_TLB_EXCEEDS_MTU);

    switch (code_switch) {

    case TLB_BFR_TO_BFR:	/* le-xxx to le-lex-t fwd and fld */ {
				/* rsp-xxx to rsp-lex-t fwd and fld */

	short enclen = outputhw->span_encapsize;
	ushort *src = outputhw->lex_bridge_header_cache;
	uchar *datagramstart;

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);
	/*
	 * If we're using LAT compression on this frame,
	 * copy in the compressed size
	 */
	if (outputsw->span_latcompress && (outputsw->span_latsize != -1))
	    pak->datagramsize = outputsw->span_latsize;
	
	/*
	 * Adjust our pointers for Lex
	 */
	pak->datagramstart -= outputhw->span_encapsize;
	datagramstart = pak->datagramstart;
	/*
	 * Write the  Lex header which currently is of size 6 an even number.
	 * The assumption is that outputhw->span_encapsize will always be of
	 * even length. The place where this is set in lex_bind(). If it is
	 * not even this has to change.
	 */
	bcopy(src, datagramstart, enclen);
        break;
    }

#ifdef TBRIDGE_MCI_CBUS
    case TLB_WITHIN_MCI:	/* he-xxx to he-lex-t fwd on cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_lex_transit_hdr_wr_inline(inputhw, outputhw,
					       pak->encsize);
	break;

    case TLB_MCI_TO_MCI:	/* he-xxx to he-lex-t fwd off cmplx */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_lex_transit_hdr_wr_inline(outputhw, outputhw,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;

    case TLB_BFR_TO_MCI:	/* he-xxx to he-lex-t fld */

	action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
	if (action != TBRIDGE_TLB_DONE)
	    return(action);

	tbridge_mci_lex_transit_hdr_wr_inline(outputhw, outputhw,
					       TBRIDGE_MCI_TX1_OFFSET);
	break;
#else
    default:
#endif
	return(TBRIDGE_TLB_UNSUPPORTED);
	break;
    }

    pak->datagramsize += outputhw->span_encapsize;
    return(TBRIDGE_TLB_DONE);
}

#endif /* __TBRIDGE_TLB_INLINE_H__ */
