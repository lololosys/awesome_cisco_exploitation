/*-------------------------------------------------------------------------
 *
 * $Id: tbridge_les_inline.h,v 3.5.28.2 1996/05/04 05:09:06 wilber Exp $
 *
 * tbridge/tbridge_les_inline.h
 *
 * October 1994, Steve Southam
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 *
 * All rights reserved.
 * 
 * Low-End platform specific inline functions for Transparent Bridging.
 *
 *-------------------------------------------------------------------------
 *
 * $Log: tbridge_les_inline.h,v $
 * Revision 3.5.28.2  1996/05/04  05:09:06  wilber
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
 * Revision 3.5.28.1  1996/04/23  06:05:45  weismann
 * CSCdi51054:  atm 1483 spanning tree bpdus not to spec
 * Branch: California_branch
 * Make to spec.
 *
 * Revision 3.5  1996/01/19  22:16:17  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * remove sniff of destination address - it is done before we get here.
 *
 * Revision 3.4  1995/12/14  20:59:24  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.3  1995/12/06  23:39:45  pitargue
 * CSCdi44993:  1492-byte IPX pings fail across tr to lane to tr path
 * allow 1492 byte snap packets.  the check for max ether size packets was
 * wrong.
 *
 * Revision 3.2  1995/11/17  18:45:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:31:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/10  17:52:43  pitargue
 * CSCdi35638:  packets going to process level from the 4500 atm nim
 * crashes router
 * make sure packets to be processed at process level is unmunged.
 * fix comments.
 *
 * Revision 2.1  1995/06/07  23:03:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-------------------------------------------------------------------------
 * $Endlog$
 *
 */


/*
 * tbridge_media_adjust_inline()
 *
 * The following nonsense is required in order to prepare frames for
 * being passed to process level. However, some paths to process level
 * require that encapsulated frames remain encapsulated, and some don't.
 * That's why there's a mask passed in for media types that need to
 * be adjusted.
 *
 * I'd have fixed this but it required dorking deeper into the high-end
 * bridging code than I'd have liked. This bullshit *has* to be fixed
 * by the bridge restructuring
 */

static inline void tbridge_les_encapsulation_recovery_inline (idbtype *input,
							      paktype *pak)
{
    ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;

    pak->datagramsize = tbridge_dlli.orig_datagramsize;
    switch (input->tbridge_media) {
    case TBR_FDDI:
    case TBR_FDDI_TRANSIT:
	if (pak->flags & PAK_TBRIDGE_IN) {
	    /*
	     * If PAK_TBRIDGE_IN is set, the frame really was encapsulated
	     * and we make it look like an ethernet frame for going to
	     * process level. If ethernet is vanilla, then this is cheezy
	     * mocha raspberry flavor...
	     */
	    pak->datagramstart += tbridge_dlli.transit_encapsize;
	    pak->datagramsize -= tbridge_dlli.transit_encapsize;
	} else {
	    /*
	     * The frame is a standard vanilla FDDI frame that we picked
	     * up. Swap the addresses back (grrr) so process level doesn't
	     * freak out on us
	     */
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);
	}
	break;

    case TBR_TR:
	/*
	 * Swap the addresses back in the Token Ring header so process
	 * level doesn't freak out on us
	 */
	ieee_swap(addr_start->daddr, addr_start->daddr);
	ieee_swap(addr_start->saddr, addr_start->saddr);
	break;

    case TBR_HDLC_TRANSIT:
    case TBR_LEX_TRANSIT:
 	break;
	
    case TBR_FRAME_RELAY_TRANSIT:
    case TBR_SMDS_TRANSIT:
	/* add back the encapsulation that we subtracted */
	switch (tbridge_dlli.src_media_type) {
	case TBR_FDDI:
	case TBR_TR:
	    /*
	     * Swap the addresses back (grrr) so process level doesn't
	     * freak out on us
	     */
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);
	    break;

	default:
	    break;
	}
	break;
    case TBR_ATM_TRANSIT:
	/*
	 * make sure these packets are destined for atm_input()
	 * and nothing else.  no quick shortcuts to hybridge_input()
	 * because 1) hwidb->status | ATM is not defined and we
	 * still have all the 1483 header info that we need.
	 */
	switch (tbridge_dlli.src_media_type) {
	case TBR_FDDI:
	case TBR_TR:
	    /*
	     * up. Swap the addresses back (grrr) so process level doesn't
	     * freak out on us
	     */
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);

	    break;

	default:
	    break;
	}
        break;
    
    case TBR_LANE_ETHER:
	pak->datagramsize += ATM_AAL5_LANE_ENCAPBYTES;
	break;

    default:
	break;
    }
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
 *
 * for the low/mid range systems, we don't need the da_offset and
 * mac_offset.  we don't have to deal w/ the cbus.  we're lucky.
 * we have the entire packet in memory.
 */
static inline
boolean tbridge_les_atm_media_classify (hwidbtype *hwidb,
					idbtype *swidb,
					paktype *pak)
{
    atm_bridge_ether_hdr *atm_hdr =
	(atm_bridge_ether_hdr *)pak->datagramstart;

    tbridge_dlli.circuit_id = pak->atm_vcid;
    tbridge_dlli.packet_pid = atm_hdr->pid;
    /*
     * move pak->mac_start and friends over the atm encaps.
     * this makes everything fall in line.
     */
    switch (atm_hdr->pid) {
    case TBRIDGE_PID_8023FCS:
	pak->datagramsize -= FCS8023_SIZE;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_8023:
	{
	    ether_hdr *ether;

	    tbridge_dlli.src_media_type = TBR_ETHER;
	    tbridge_dlli.transit_encapsize = ATM_BRIDGE_ETHER_ENCAPBYTES;
	    pak->mac_start      = pak->datagramstart +
		ATM_BRIDGE_ETHER_ENCAPBYTES;;
	    pak->addr_start     = pak->mac_start;    /* DA start */

	    /*
	     * check if it's arpa or 802.3.  if it's 802.3, set info_start
	     */
	    ether = (ether_hdr *)pak->mac_start;
	    if (ether->type_or_len <= MAXETHERSIZE)
		pak->info_start = pak->mac_start + ETHER_ARPA_ENCAPBYTES;
	    else
		pak->info_start = NULL;

	    pak->datagramsize  -= ATM_BRIDGE_ETHER_ENCAPBYTES;

	    /*
	     * Now pak->rxtype is RXTYPE_UNKNOWN because we don't
	     * decode the interior packet in 
	     *  atm_decode_encapsulation_inline().
	     * If IRB is enabled, find out the real rxtype for the
	     * interior packet in order to make bridge/route decision
	     * correctly later.
	     */
	    if (global_irb_enable) {
		pak->rxtype =
		    ether_decode_encapsulation_inline(pak, ether);
	    }

	    break;
	}

    case TBRIDGE_PID_8025FCS:
	pak->datagramsize -= TR_FCS_SIZE;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_8025:
	{
	    ieee_addrs *addr_start;

	    tbridge_dlli.src_media_type = TBR_TR;
	    tbridge_dlli.transit_encapsize = ATM_BRIDGE_OTHER_ENCAPBYTES;
	    pak->mac_start      = pak->datagramstart +
		ATM_BRIDGE_ETHER_ENCAPBYTES;
	    pak->addr_start     = pak->mac_start + TR_ACFC_SIZE;
	    pak->info_start     = pak->mac_start + TRING_ENCAPBYTES;
	    pak->datagramsize  -= ATM_BRIDGE_ETHER_ENCAPBYTES;

	    /*
	     * now flip the da and sa to canonical order
	     */
	    addr_start = (ieee_addrs *)pak->addr_start;
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);
	    break;
	}

    case TBRIDGE_PID_FDDIFCS:
	pak->datagramsize -= FDDI_FCS;
	/*
	 * fall through
	 */
    case TBRIDGE_PID_FDDI:
	{
	    ieee_addrs *addr_start;

	    tbridge_dlli.src_media_type = TBR_FDDI;
	    tbridge_dlli.transit_encapsize = ATM_BRIDGE_OTHER_ENCAPBYTES;

	    if (global_irb_enable) {
		fddi_hdr *fddi;
		/* We have a pad byte for FDDI */
		fddi = (fddi_hdr *)(pak->datagramstart + 
				    ATM_BRIDGE_ETHER_ENCAPBYTES + 1);
		/*
		 * fddi_decode_encapsulation will set mac_start, addr_start,
		 * info_start
		 */
		pak->rxtype = fddi_decode_encapsulation_inline(pak, fddi,
                	(hwidb->type == IDBTYPE_FDDIT ? FDDIT_RII : FDDI_RII));
	    } else {
		pak->mac_start      = pak->datagramstart +
		    ATM_BRIDGE_ETHER_ENCAPBYTES + 1;
		pak->addr_start     = pak->mac_start + FDDI_FC;
		pak->info_start     = pak->mac_start + FDDI_ENCAPBYTES;
	    }

	    pak->datagramsize  -= (ATM_BRIDGE_ETHER_ENCAPBYTES + 1);

	    /*
	     * now flip the da and sa to canonical order
	     */
	    addr_start = (ieee_addrs *)pak->addr_start;
	    ieee_swap(addr_start->daddr, addr_start->daddr);
	    ieee_swap(addr_start->saddr, addr_start->saddr);
	    break;
	}
    case TBRIDGE_PID_BPDU:
        return (FALSE);     /* This kicks spanning bpdu to process level */

    default:
	/*
	 * unknown type
	 */
	if (atm_errors_debug)
	    buginf("\nATM(%s): encap type %d not supported",
		   hwidb->hw_namestring, atm_hdr->pid);
	return(FALSE);
    }

    return(TRUE);
}

/*
 * If LANE is configured on the interface, then the encapsulation would 
 * include the Client ID apart from the VC and the Enc_Type. That's what 
 * this function takes care of in the header
 */
static inline
boolean tbridge_les_lane_ether_classify (hwidbtype *hwidb,
					 idbtype *swidb,
					 paktype *pak)
{
    ether_hdr *ether;

    tbridge_dlli.circuit_id = pak->atm_vcid;
    tbridge_dlli.packet_pid = 0;

    tbridge_dlli.src_media_type = TBR_ETHER;
    tbridge_dlli.transit_encapsize = ATM_AAL5_LANE_ENCAPBYTES; 
    pak->mac_start  = pak->datagramstart + ATM_AAL5_LANE_ENCAPBYTES;
    pak->addr_start = pak->mac_start;	/* DA start */

    /*
     * check if it's arpa or 802.3.  if it's 802.3, set info_start
     */
    ether = (ether_hdr *)pak->mac_start;
    if (ether->type_or_len <= MAXETHERSIZE)
	pak->info_start = pak->mac_start + ETHER_ARPA_ENCAPBYTES;
    else
	pak->info_start = NULL;

    pak->datagramsize  -= ATM_AAL5_LANE_ENCAPBYTES;

    return(TRUE);
}

/*
 * Low end routine to classify frame relay packets
 * Set up is done in fr_fast_pvc_switch_or_fs
 * pak->info_start is set if we are IETF.
 * We pass the dlci number in the sniff buffer (Ugly, but no decode needed
 * in this routine)
 */
static inline
boolean tbridge_les_frame_relay_classify (hwidbtype *hwidb,
					  idbtype *swidb,
					  paktype *pak)
{
    /*
     * move pak->mac_start and friends over the frame-relay encaps.
     * this makes everything fall in line.
     */
    tbridge_dlli.circuit_id = hwidb->sniff[SNIFFBASE+0].d.sword[0];
    if (pak->info_start == NULL) {
	/*
	 * We have a normal cisco frame-relay packet - that means that
	 * we have ethernet right after the encapsulation
	 */
	tbridge_dlli.src_media_type = TBR_ETHER;
	tbridge_dlli.transit_encapsize = FRAME_RELAY_ENCAPBYTES;
	pak->mac_start = pak->datagramstart + FRAME_RELAY_ENCAPBYTES;
	pak->datagramsize -= FRAME_RELAY_ENCAPBYTES;
	pak->rxtype =
	    ether_decode_encapsulation_inline(pak,
					      (ether_hdr *)pak->mac_start);
    } else {
	/*
	 * have a 1490 header
	 * Note: use pak->encsize because of the dumb snap0pad stuff.
	 * pak->info_start is set to the snap header in the calling routine
	 */
	snap_hdr *snap = (snap_hdr *)pak->info_start;
	tbridge_dlli.packet_pid = GETSHORT(&snap->type);
	switch (tbridge_dlli.packet_pid) {
	case FR1490_ETHER_FCS:
	    pak->datagramsize -= FCS8023_SIZE;
	    /*
	     * fall through
	     */
	case FR1490_ETHER:
	    tbridge_dlli.src_media_type = TBR_ETHER;
	    tbridge_dlli.transit_encapsize = pak->encsize;

	    pak->datagramsize -= pak->encsize;
	    /*
	     * ether_decode_encapsulation_inline will set up the
	     * mac_start and addr_start, info_start if its an
	     * llc frame
	     */
	    pak->rxtype =
		ether_decode_encapsulation_inline(pak,
						  (ether_hdr *)snap->data);
	    break;
	case FR1490_802_5_FCS:
	    pak->datagramsize -= TR_FCS_SIZE;
	    /*
	     * fall through
	     */
	case FR1490_802_5:
	    {
		tring_hdr *tring;

		tbridge_dlli.src_media_type = TBR_TR;
		tbridge_dlli.transit_encapsize = pak->encsize;
		pak->datagramsize  -= pak->encsize;
		pak->mac_start  = pak->datagramstart + pak->encsize;
		tring = (tring_hdr *)pak->mac_start;
		pak->addr_start = tring->daddr;
		pak->info_start = tring->data;

		/*
		 * now flip the da and sa to canonical order
		 */
		ieee_swap(tring->daddr, tring->daddr);
		ieee_swap(tring->saddr, tring->saddr);
		/* should do some type of encapsulation decode here
		 * but there's no mechanism for it
		 */
	    }
	    break;

	case FR1490_FDDI_FCS:
	    pak->datagramsize -= FDDI_FCS;
	    /*
	     * fall through
	     */
	case FR1490_FDDI: {
	    fddi_hdr *fddi;
	    ushort encsize;

	    /* We have a pad byte for FDDI */
	    encsize = pak->encsize + FR1490_FDDI_PADBYTES;
	    tbridge_dlli.src_media_type = TBR_FDDI;
	    tbridge_dlli.transit_encapsize = encsize;
	    pak->datagramsize  -= encsize;
	    fddi = (fddi_hdr *)(pak->datagramstart + encsize);
	    /*
	     * fddi_decode_encapsulation will set mac_start, addr_start,
	     * info_start
	     */
	    pak->rxtype = fddi_decode_encapsulation_inline(pak,
							   fddi, FDDI_RII);

	    /*
	     * now flip the da and sa to canonical order
	     */
	    ieee_swap(fddi->daddr, fddi->daddr);
	    ieee_swap(fddi->saddr, fddi->saddr);
	    break;
	}
	case FR1490_SPANNING:
	default:
	    /*
	     * unknown type - boost to process switching - 
	     */
	    return(FALSE);
	}
    }
    return(TRUE);
}
static inline
boolean tbridge_les_smds_classify (hwidbtype *hwidb,
				   idbtype *swidb,
				   paktype *pak)
{
    snap_hdr *snap;
    smdstype *smds;

    /* these are set up in smds_decode_encapsulation_inline */
    smds = (smdstype *)pak->mac_start;
    snap = (snap_hdr *)pak->info_start;
    tbridge_dlli.packet_pid = GETSHORT(&snap->type);
    /*
     * pak->if_input should be set with the correct subinterface -
     * we no longer need to sniff this.
     */
    tbridge_dlli.smdssrc_ptr = smds->smdssrc;

    /* The transit_encapsize accounts for the variable smds dxi header
     * (depending on cisco mode or not) PLUS the SMDS bridge snap header
     * and the SMDS bridge PAD encap bytes. After the pad starts the
     * MAC frame of the smds packet. PAD gets accounted for based on
     * packet_pid, and by end of routine tbridge_dlli.transit_encapsize
     * is properly adjusted to include appropriate PAD bytes for given
     * media being bridged over SMDS, and therefore includes ALL wan
     * smds encapsulation (including PAD) up to but NOT including
     * the MAC portion of the smds bridge frame.
     *
     * Please note pak->network_start is set by smds decode routines
     * associated with interrupt level input driver routines and does
     * NOT point to network data of bridged packet, it points to END
     * of smds SNAP bridge encapsulation, just before PAD bytes of smds
     * bridge encapsulation and before the MAC of smds transit bridged
     * packet. Using network_start to calculate mac_start and
     * transit_encapsize
     * is faster than calculating these entities with encsize for smds
     * because of pain of dealing with variable smds dxi header (too
     * much dereferencing to get dxi header length value here), even
     * though it would be nicer as in bridging code we do not want to
     * make habit of using network_start as this pointer gets shuffled
     * around alot in context of IOS code. So we use network_start in
     * this interrupt level smds routine based on fact of how it is set
     * in smds_decode routines at input driver. May want to change this
     * eventually and use datagramstart and encsize packet attributes, with
     * smds dxi header size attributes, but may cost us some speed.
     */

    tbridge_dlli.transit_encapsize = pak->network_start - pak->datagramstart;
    
    switch (tbridge_dlli.packet_pid) {
    case SMDS_BRIDGE_PID_8023FCS:
	pak->datagramsize -= FCS8023_SIZE;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_8023:
        /* Account for smds bridge pad bytes in transit_encapsize */
        tbridge_dlli.transit_encapsize += SMDS_BRIDGE_PAD_SIZE; 
        tbridge_dlli.src_media_type = TBR_ETHER;
        
	/*
	 * ether_decode_encapsulation_inline will set up the
	 * mac_start and addr_start, info_start if its an
	 * llc frame
	 */
	pak->rxtype =
	    ether_decode_encapsulation_inline(pak,(ether_hdr *)
					      (snap->data +
					       SMDS_BRIDGE_PAD_SIZE));
	break;
    case SMDS_BRIDGE_PID_8025FCS:
	pak->datagramsize -= TR_FCS_SIZE;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_8025:
	{
	    tring_hdr *tring;
          
           /* Account for smds bridge pad bytes in transit_encapsize */        
            tbridge_dlli.transit_encapsize += SMDS_BRIDGE_PAD_SIZE; 
	    tbridge_dlli.src_media_type = TBR_TR;
	    pak->mac_start  = pak->network_start + SMDS_BRIDGE_PAD_SIZE;
	    tring = (tring_hdr *)pak->mac_start;
	    pak->addr_start = tring->daddr;
	    pak->info_start = tring->data;

	    /*
	     * now flip the da and sa to canonical order
	     */
	    ieee_swap(tring->daddr, tring->daddr);
	    ieee_swap(tring->saddr, tring->saddr);
	    /* should do some type of encapsulation decode here
	     * but there's no mechanism for it
	     */
	}
	break;

    case SMDS_BRIDGE_PID_FDDIFCS:
	pak->datagramsize -= FDDI_FCS;
	/*
	 * fall through
	 */
    case SMDS_BRIDGE_PID_FDDI: {
	fddi_hdr *fddi;

        /* Account for smds bridge pad bytes in transit_encapsize */
        tbridge_dlli.transit_encapsize += SMDS_BRIDGE_PAD_FDDI_SIZE;
	tbridge_dlli.src_media_type = TBR_FDDI;
	fddi = (fddi_hdr *)(pak->network_start + SMDS_BRIDGE_PAD_FDDI_SIZE);
	/*
	 * fddi_decode_encapsulation will set mac_start, addr_start,
	 * info_start
	 */
	pak->rxtype =
	    fddi_decode_encapsulation_inline(pak, fddi, FDDI_RII);

	/*
	 * now flip the da and sa to canonical order
	 */
	ieee_swap(fddi->daddr, fddi->daddr);
	ieee_swap(fddi->saddr, fddi->saddr);
	break;
    }
    case SMDS_BRIDGE_PID_BPDU:
    default:
	/*
	 * unknown type - boost to process switching - 
	 */
	return(FALSE);
    }
    /* get rid of pad and encapsulation */
    pak->datagramsize -= tbridge_dlli.transit_encapsize;
    pak->datagramsize -= ((smds->smds_xhlpi & SMDS_XHLPI_PL) + sizeof(ulong));
			  
    return(TRUE);
}

static inline
void
add_fddi_hardware_address_filter_inline (hwidbtype *inputhw, uchar *mac_addr)
{
   reg_invoke_media_add_hardware_address_filter(inputhw, mac_addr);
}

