/* $Id: tbridge_rsp_inline.h,v 3.5.46.5 1996/05/19 21:14:15 hou Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_rsp_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_rsp_inline.h
 *
 * October 1994, Marty Cieslak
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * RSP specific in-line functions for transparent bridging.
 *------------------------------------------------------------------
 * $Log: tbridge_rsp_inline.h,v $
 * Revision 3.5.46.5  1996/05/19  21:14:15  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - add  bridging support for packet from different medium
 *   to ISL vlan fast ethernet on RSP.
 * - fix flooding path problem for packet coming from ISL vLAN interface.
 *   on RSP and 7000.
 *
 * Revision 3.5.46.4  1996/05/14  04:30:22  speakman
 * CSCdi57584:  CGMP packets not seen by RSP
 * Branch: California_branch
 * Calculate network_start when copying RSP packets for flooding.
 *
 * Revision 3.5.46.3  1996/05/04  05:09:20  wilber
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
 * Revision 3.5.46.2  1996/04/23  06:05:53  weismann
 * CSCdi51054:  atm 1483 spanning tree bpdus not to spec
 * Branch: California_branch
 * Make to spec.
 *
 * Revision 3.5.46.1  1996/04/11  20:43:43  slin
 * CSCdi51641:  Fix misleading comment
 * Branch: California_branch
 *
 * Revision 3.5  1995/12/14  20:59:29  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.4  1995/12/06  23:39:43  pitargue
 * CSCdi44993:  1492-byte IPX pings fail across tr to lane to tr path
 * allow 1492 byte snap packets.  the check for max ether size packets was
 * wrong.
 *
 * Revision 3.3  1995/12/01  23:25:28  pitargue
 * CSCdi44835:  Transparent bridging over serial link doesnt work
 * make sure hdlc and lex copy the transit header, while other transit
 * type media's like atm and frame relay only get the legacy lan mac
 * packet
 *
 * Revision 3.2  1995/11/17  18:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:31:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/21  23:54:36  pitargue
 * CSCdi40861:  ATM remote bridging ( IEEE bridging ) is broken for clns
 * and decnet.
 * when copying packets for flooding, use pak->mac_start instead of
 * pak->datagramstart to make sure we get the head of the mac packet
 * instead of the encaps of rfc 1483 and rfc 1490.
 *
 * Revision 2.3  1995/08/24  21:28:16  ogrady
 * CSCdi39282:  Token ring tbridge process level packets get mac addr
 * twisted.  Fix recovery inline for tr packets not handled by tbridge fs
 * code.
 *
 * Revision 2.2  1995/06/28  23:51:15  mcieslak
 * Packets being flooded were not being copied properly when they
 * came across a serial link. Also SDE packets were not being
 * copied properly either.
 * CSCdi35424:  clns neighbor fails to come up on AGS+
 *
 * Revision 2.1  1995/06/07  23:03:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TBRIDGE_RSP_INLINE_H__
#define __TBRIDGE_RSP_INLINE_H__

/*
 * tbridge_rsp_encapsulation_recovery_inline
 *
 * Puts the encapsulation back the way that the receive routines know
 * how to handle it.
 *
 * This routine may not be necessary at all depending on the handling of
 * the fastswitch code. ...ie; tring	XXX
 */
 
static inline
void tbridge_rsp_encapsulation_recovery_inline (idbtype *inputsw, 
						paktype *pak)
{
    ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;

    pak->datagramsize = tbridge_dlli.orig_datagramsize;
    switch (inputsw->tbridge_media) {

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
	}
	break;

    case TBR_CTR:
	/*
	 * Swap the addresses back in the Token Ring header so process level
	 * doesn't freak out on us
	 */
	ieee_swap(addr_start->daddr, addr_start->daddr);
	ieee_swap(addr_start->saddr, addr_start->saddr);
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
	case TBR_FDDI_TRANSIT:
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
    case TBR_FRAME_RELAY_TRANSIT:
    case TBR_SMDS_TRANSIT:
	switch (tbridge_dlli.src_media_type) {
	case TBR_FDDI:
	case TBR_TR:
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);
            break;

        default:
            break;
        }
	break;
    default:
	break;
    }
}

/*
 * tbridge_rsp_copy_inline
 *
 * Copies the data starting from the specified "src" point in an RSP MEMD
 * buffer (using cache line accesses) to the specified "dst" point in DRAM.
 * The cache lines are invalidated once the copy is complete to avoid
 * possible coherency problems.
 *
 * The bcopy should really be replaced with a 'bcopy-source-invalidate',
 * or some such, to invalidate the source from the data cache as it is
 * copied. It could also do some whizzy 'create-dirty-exclusive' cache
 * magic to prevent us from doing a 'read/modify/write' on the
 * destination in DRAM, to save time. XXX
 */
 
static inline
void tbridge_rsp_copy_inline (uchar *src,
			      uchar *dst,
			      int size)
{
    bcopy(src, dst, size);
    writeback_inval_data_inline(src, size);
}


/*
 * Copy a received packet from the input interface into a network
 * buffer.
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
boolean tbridge_rsp_copy_rx_pak_inline (paktype *rsp_pak,
					paktype *pak_copy)

{
    ushort dgsize, llcsize;

    hwidbtype *inputhw = rsp_pak->if_input->hwptr;

    pak_copy->if_input = rsp_pak->if_input;
    pak_copy->mac_start = pak_center(pak_copy);
    pak_copy->network_start = pak_center(pak_copy);
    pak_copy->datagramsize = dgsize = rsp_pak->datagramsize;
    pak_copy->rxtype = rsp_pak->rxtype;

    switch (tbridge_dlli.src_media_type) {

    case TBR_ETHER:
    case TBR_ETHER_ISL_VLAN:
	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on ETHER.  We actually treat SNAP
	 * just like 802.3 in this case.
	 */
	pak_copy->datagramstart = pak_copy->mac_start -= ETHER_ARPA_ENCAPBYTES;
	llcsize = dgsize - ETHER_ARPA_ENCAPBYTES;
	pak_copy->addr_start = pak_copy->mac_start;
	if (rsp_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXETHERSIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXETHERSIZE, rsp_pak);
	    return(FALSE);
	}

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

	break;

    case TBR_ETHER_SDE:
	/*
	 * This copy works only because we are making the assumption
	 * that ARPA and 802.3 encapsulations are the same length, and
	 * we do not expect SNAP on ETHER.  We actually treat SNAP
	 * just like 802.3 in this case.
	 *
	 * LBJ, rsp_pak->info_start better not be NULL!
	 */
        pak_copy->datagramstart = pak_copy->mac_start -= 
	    (ETHER_ARPA_ENCAPBYTES + SDE_ENCAPBYTES);

	pak_copy->info_start = 
	    ((uchar *)&((ether_hdr *) pak_copy->mac_start)->data) + 
		SDE_ENCAPBYTES;

        pak_copy->addr_start = pak_copy->mac_start;

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

        break;

    case TBR_FDDI: {

	snap_hdr *link = (snap_hdr *)rsp_pak->info_start;

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
	if (rsp_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((fddi_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXFDDISIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXFDDISIZE, rsp_pak);
	    return(FALSE);
	}

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

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

	break;
    }

    case TBR_FDDI_SDE: {

	/*
	 * LBJ, rsp_pak->info_start better not be NULL!
	 */

        snap_hdr *link = (snap_hdr *)rsp_pak->info_start;

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

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

        break;
    }

	/*
	 * the media type was really set to TBR_TR, since we want
	 * to take the low end path.
	 */
    case TBR_TR: { 

	snap_hdr *link = (snap_hdr *)rsp_pak->info_start;

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
	if (rsp_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((tring_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXTRSIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXTRSIZE, rsp_pak);
	    return(FALSE);
	}

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

	break;
    }

    case TBR_CTR_SDE: {

	/*
	 * LBJ, rsp_pak->info_start better not be NULL!
	 */

        snap_hdr *link = (snap_hdr *)rsp_pak->info_start;

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

	tbridge_rsp_copy_inline(rsp_pak->mac_start, pak_copy->datagramstart,
				dgsize);

        break;
    }

    case TBR_FDDI_TRANSIT:
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
	if (rsp_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > MAXFDDISIZE) {
	    tbridge_log_giant(inputhw, llcsize, MAXFDDISIZE, rsp_pak);
	    return(FALSE);
	}

	tbridge_rsp_copy_inline(rsp_pak->datagramstart,
				pak_copy->datagramstart, dgsize);

        break;

    case TBR_HDLC_TRANSIT:
    case TBR_LEX_TRANSIT:
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
	if (rsp_pak->info_start)
	    pak_copy->info_start = (uchar *)
		&((ether_hdr *) pak_copy->mac_start)->data;
	else
	    pak_copy->info_start = NULL;

	if (llcsize > inputhw->maxmtu) {
	    tbridge_log_giant(inputhw, llcsize, inputhw->maxmtu, rsp_pak);
	    return(FALSE);
	}

	tbridge_rsp_copy_inline(rsp_pak->datagramstart,
				pak_copy->datagramstart, dgsize);

	break;

    case TBR_HDLC_TRANSIT_SDE:

        /*
         * This copy works only because we are making the assumption
         * that ARPA and 802.3 encapsulations are the same length, and
         * we do not expect SNAP on HDLC-transit.  We actually treat SNAP
         * just like 802.3 in this case.
	 *
	 * LBJ, rsp_pak->info_start better not be NULL!
         */


        pak_copy->mac_start -= (ETHER_ARPA_ENCAPBYTES + SDE_ENCAPBYTES);
        pak_copy->datagramstart = pak_copy->mac_start -
            tbridge_dlli.transit_encapsize;

	pak_copy->info_start = 
	    ((uchar *)&((ether_hdr *) pak_copy->mac_start)->data) +
		SDE_ENCAPBYTES;

        pak_copy->addr_start = pak_copy->mac_start;

	tbridge_rsp_copy_inline(rsp_pak->datagramstart,
				pak_copy->datagramstart, dgsize);

        break;

    default:
	/*
	 * "This should never happen."
	 */
	return(FALSE);

	break;
    }

    /*
     * Adjust network_start to reflect the presence
     * of an LLC (SAP or SNAP) header.
     */
    if (pak_copy->info_start != NULL)
	pak_copy->network_start = pak_copy->info_start +
	    (rsp_pak->network_start - rsp_pak->info_start);

    return(TRUE);
}

static inline
boolean tbridge_rsp_atm_media_classify(hwidbtype *hwidb,
				       idbtype *swidb,
				       paktype *pak)
{
    ieee_addrs *addr_start;
    atm_bridge_ether_hdr *atm_hdr =
	(atm_bridge_ether_hdr *)pak->datagramstart;

    tbridge_dlli.circuit_id = atm_hdr->encap.vcnum;
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
		ATM_BRIDGE_ETHER_ENCAPBYTES;
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

    case TBRIDGE_PID_FDDIFCS:
       	pak->datagramsize -= FDDI_FCS;
        /*
         * fall through
         */
    case TBRIDGE_PID_FDDI:
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
 * Inline function to classify packets on LANE interface emulating
 * ethernet. These packets have VCD+Enc_Type+LECID as the transit
 * encapsulation
 */
static inline
boolean tbridge_rsp_lane_ether_classify(hwidbtype *hwidb,
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
boolean tbridge_rsp_frame_relay_classify (hwidbtype *hwidb,
					  idbtype *swidb,
					  paktype *pak)
{
    /*
     * move pak->datagramstart and friends over the frame-relay encaps.
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
	    pak->rxtype = fddi_decode_encapsulation_inline(pak, fddi,
		        (hwidb->type == IDBTYPE_FDDIT ? FDDIT_RII : FDDI_RII));

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
boolean tbridge_rsp_smds_classify (hwidbtype *hwidb,
				   idbtype *swidb,
				   paktype *pak)
{
    snap_hdr *snap;
    smdstype *smds;

    /* these are set up in smds_decode_encapsulation_inline */
    smds = (smdstype *)pak->mac_start;
    /*
     * We need to find out the real swidb - we could have subinterfaces here
     * Pass the smds destination address into the smds code to find it
     */
    swidb = reg_invoke_media_addr_to_swidb(ET_SMDS, smds->smdsdest, hwidb);
    if (swidb == NULL)
	return(FALSE);

    snap = (snap_hdr *)pak->info_start;
    tbridge_dlli.packet_pid = GETSHORT(&snap->type);
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
	    fddi_decode_encapsulation_inline(pak, fddi,
		        (hwidb->type == IDBTYPE_FDDIT ? FDDIT_RII : FDDI_RII));

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
add_fddi_hardware_address_filter_inline (hwidbtype *inputhw,  uchar *mac_addr)
{
   if ((inputhw->fddi_cam_sa_miss == FDDIT_CAM_VALID) || (inputhw->status & IDB_VIP))
       reg_invoke_media_add_hardware_address_filter(inputhw, mac_addr );
}

#endif /* __TBRIDGE_RSP_INLINE_H__ */
