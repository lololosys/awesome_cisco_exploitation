/* $Id: tbridge_hes.c,v 3.8.4.6 1996/08/28 13:16:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_hes.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_hes.c
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 * MCI/cBus-specific functions for transparent bridging.  These functions
 * include interface-specific functions to prepare the arguments required
 * by generic transparent bridging functions for forwarding and flooding.
 * The high-end system (HES) transparent bridging subsystem is
 * initialized here.
 *
 *
 *------------------------------------------------------------------
 * $Log: tbridge_hes.c,v $
 * Revision 3.8.4.6  1996/08/28  13:16:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.4.5  1996/08/11  16:58:12  sluong
 * CSCdi62694:  bridged raw packets not recognized as an IPX packets
 * Branch: California_branch
 *
 * Revision 3.8.4.4  1996/07/23  13:29:44  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.4.3  1996/05/04  05:08:58  wilber
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
 * Revision 3.8.4.2  1996/03/23  22:49:32  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/03/18  22:12:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.3  1996/03/16  07:39:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.12.2  1996/03/13  02:02:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.1  1996/03/07  10:51:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8  1996/03/07  23:33:09  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.7  1996/01/05  04:57:19  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/17  06:12:32  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.5  1995/12/14  20:59:20  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.4  1995/11/30  22:55:06  hou
 * CSCdi39644:  Transparent bridging fails over MultiBus token ring and
 * MCI serial
 * - tbridge_mci_copy_rx_pak_inline():
 *   Copy the serial transit packet into pak buffer starting from
 *   datagramstart instead of mac_start.
 *
 * Revision 3.3  1995/11/20  19:29:34  speakman
 * CSCdi42919:  CSCdi41837 broke lego images
 * Move SMF initialization into platform-independent code.
 *
 * Revision 3.2  1995/11/17  18:45:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:31:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/26  00:30:40  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Initialize tbridge SMF chunk.
 *
 * Revision 2.2  1995/09/30  22:44:19  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Exchange the safety of data hiding for a 7% performance improvement
 * in the 64 byte pps no-drop rate.  Use an inline to do the SMF match
 * instead of a function call, and forgo the pointer safety checks in
 * ieee_equal in the bte locator to do a direct MAC address comparison.
 *
 * Revision 2.1  1995/06/07  23:03:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "mgd_timers.h"
#include "logger.h"
#include "../tbridge/msg_tbridge.c"	/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "access.h"
#include "address.h"
#include "config.h"

#include "../ui/debug.h"

#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/ether_inline.h"
#include "../if/atm.h"
#include "../atm/ilmi_api.h"
#include "../if/atm_debug_flags.h"

/* For Lan Emulation */
#include "../atm/atmsig_api.h"

#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../lane/lane_registry.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_fip.h"
#include "../hes/cbus_registry.h"
#include "../hes/cbus_bcopy_inline.h"
#include "../hes/if_aip.h"

#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"

#include "../srt/span.h"
#include "../srt/bridgeaccess.h"
#include "../srt/srt_registry.h"

#include "../vlan/vlan.h"

#include "../smf/smf_public.h"

#include "../smf/smf_private.h"
#include "../smf/smf_private_inline.h"

#include "tbridge_public.h"
#include "tbridge_common_inline.h"
#include "tbridge_debug.h"
#include "tbridge_hes_private.h"
#include "tbridge_sde.h"
#include "tbridge_sde_inline.h"

#include "../hes/bridge_cbus.h" /* SHOULD BE IN /tbridge */

/*
 * External data definitions:
 */

paktype *tbridge_encap_pak;
charlong *tbridge_encap;

paktype *tbridge_flood_pak;

/*
 * allocate the tbridge_dlli structure that will be used by
 * every media to switch on.  this structure will hold info
 * used to switch on and find out where the offsets lie.
 * also note that because the TB code is single threaded,
 * this works.  if TB ever is changed to handle more than one
 * packet at a time, then this needs to change to multiple
 * tbridge_dlli's.
 */
tbridge_dlli_type tbridge_dlli;

/*
 * Included code.
 */

#include "../hes/if_cmd.h"
#include "../hes/hes_fip_inline.h"

/*
 * These defines cause all the inlines to be compiled as
 * functions for debugging purposes.  Retain them for now
 * until the code bloat that results from static inlines
 * is under control.  Until then, performance testing will
 * not yield meaningful results.  Restore them to:

#define STATIC	static
#define INLINE	inline

 */

#define STATIC
#define INLINE

/*
 * smds local variables needed for the high end
 */
ushort smds_pad, smds_basize, smds_rsvd_betag;
ulong smds_cbus_copy_last_long;

/*
 * This define exists solely to compensate for the compiler's inability
 * to eliminate code inside conditionals on constants known to the
 * compiler at compile time.  These constants are used herein as code
 * switches, the intent of which was to allow tbridge_tlb_inline.h and
 * tbridge_inline.h to be compiled for non-MCI/cBus platforms without any
 * references to functions in tbridge_mci_inline.h.  That's the theory
 * anyway.  The compiler does eliminate unreachable code framed in
 * conditionals or switch statements on compile-time constants in toy
 * files of limited complexity.  What I've done here seems to tax its
 * abilities, and so the introduction of this define and the flurry of
 * '#ifdef TBRIDGE_MCI_CBUS' statements in tbridge_tlb_inline.h and
 * tbridge_inline.h.
 */

#define TBRIDGE_MCI_CBUS

#include "tbridge_mci_inline.h"
#include "tbridge_isl_inline.h"
#include "tbridge_tlb_inline.h"
#include "tbridge_irb_inline.h"
#include "tbridge_inline.h"


/*
 * Copy a packet from an MCI/cBUs interface for process-level bridging,
 * and translate it to ether in preparation for slow-switching it.
 */

static void tbridge_mci_slowswitch (paktype *encap_pak)

{
    paktype *pak_copy;

    pak_copy = getbuffer(encap_pak->datagramsize);

    if (pak_copy) {
	if (tbridge_mci_copy_rx_pak_inline(encap_pak, pak_copy)) {
	    pak_copy->if_output = encap_pak->if_output;
	    pak_copy->bridgeptr = encap_pak->bridgeptr;
	    tbridge_slowswitch_inline(pak_copy);
	} else {
	    tbridge_mci_flush_datagram_inline(encap_pak->if_input->hwptr);
	    datagram_done(pak_copy);
	}
    }
}


/*
 * Take care of any MCI/cBus-specific preparation required by
 * tbridge_flood_inline to perform platform-independent flooding.
 */

static void tbridge_mci_flood (paktype *encap_pak)

{
    idbtype *inputsw = encap_pak->if_input;
    hwidbtype *inputhw = inputsw->hwptr;
    paktype *flood_pak;

    tbifd_type	*flood_list;
    ulong	flood_scope;

    boolean cbus_flood_done = FALSE;
    boolean copy_the_packet = FALSE;

    inputsw = encap_pak->if_input;
    inputhw = inputsw->hwptr;
    
    if (is_bvi(inputhw)) {
    	flood_list = spanarray[inputhw->unit]->floodQ.qhead;
    }
    else {
    	flood_list = inputsw->span_ptr->floodQ.qhead;
    }
    flood_scope = TBIFD_FLOOD;

    /*
     * See if we can flood this packet entirely on the cBus.
     */
    if (cbus_flood_all(inputsw, flood_list, &copy_the_packet) && 
	cbus_flood_oncomplex(inputhw)) {
	/* Account for received packets that were flooded. */
	tbridge_rx_accounting_inline(inputhw, tbridge_dlli.orig_datagramsize);
	return;
    }

    /*
     * If cbus copy assistance for flooding is enabled and no output
     * interface's span_bridge_process_force is set, we don't need 
     * to copy the packet to network buffer(tbridge_flood_pak), just
     * pass encap_pak with datalink-layer header to tbridge_flood_inline()
     * for the translational job. Also we don't need to worry about 
     * Autonomous Bridging since AB is not compatible with cbus copy due 
     * to the fact that AB will flush the packet after it's done with the
     * assumption that packet has been copied to network buffer.
     * The boolean, "cbus_copy_capable" should have been cleared/set
     * accordingly.
     */
    if (inputsw->span_ptr->cbus_copy_capable) {
	if (copy_the_packet) {
	    flood_pak = tbridge_flood_pak;
	    if (!tbridge_mci_copy_rx_pak_inline(encap_pak, flood_pak)) {
		tbridge_rx_accounting_inline(inputhw, 
					     tbridge_dlli.orig_datagramsize);
		tbridge_mci_flush_datagram_inline(inputhw);
		return;
	    }
	} else {
	    mciregtype *regptr;
	    ushort trail_offset;

	    /*
	     * This is gross, but if we are not copying the packet,
	     * we must read the last long for smds, so it can pad 0's
	     * and add the trailer on
	     */
	    flood_pak = encap_pak;
	    regptr = inputhw->mci_regptr;
	    if (tbridge_dlli.src_media_type == TBR_FDDI)
		trail_offset = tbridge_dlli.mac_offset - 1 +
		    (((encap_pak->datagramsize +
		       SMDS_BRIDGE_PAD_FDDI_SIZE) & ~0x3) >> 1);
	    else
		trail_offset = tbridge_dlli.mac_offset - 1 +
		    (((encap_pak->datagramsize +
		       SMDS_BRIDGE_PAD_SIZE) & ~0x3) >> 1);
	    regptr->argreg = trail_offset;
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    smds_cbus_copy_last_long = regptr->readlong;
	}
    } else {
	flood_pak = tbridge_flood_pak;

	/*
	 * At this point, we must copy the packet for flooding in the RP.
	 * There may be some interfaces eligible for cBus flooding
	 * but some others require RP flooding.
	 */
	if (!tbridge_mci_copy_rx_pak_inline(encap_pak, flood_pak)) {
	    /*
	     * Account for received packets that were filtered.
	     * This is a corner case that catches unknown media types
	     * and giants.
	     */
	    tbridge_rx_accounting_inline(inputhw,
					 tbridge_dlli.orig_datagramsize);
	    tbridge_mci_flush_datagram_inline(inputhw);
	    return;
	}

	if (inputhw->cbus_bridge_enable &&
	    inputsw->span_ptr->auto_flood_count) {

	    /*
	     * Now that we have copied the packet for flooding, we can initiate
	     * cBus flooding if the input interface itself has cBus bridging
	     * enabled.
	     */
	    if (cbus_flood_oncomplex(inputhw))
		cbus_flood_done = TRUE;
	    
	} else {
	    
	    /*
	     * Flush the received packet since by this point we have copied it
	     * to tbridge_flood_pak, and any on-complex cBus flooding has been
	     * done.
	     */
	    tbridge_mci_flush_datagram_inline(inputhw);
	}
    }
    flood_pak->if_input = inputsw;
    tbridge_flood_inline(flood_pak, flood_list, flood_scope,
			 cbus_flood_done, TBRPAK_MCI);
}

/*
 * Take care of any MCI/cBus-specific preparation required by
 * tbridge_forward_inline to perform platform-independent bridging.
 *
 * Transparently bridge a packet from an MCI/cBus interface to another
 * MCI/cBus.
 * Obsoletes hes_bridge_receive and mbus_tr_mci_bridge_input.
 *
 * The received packet is in the interface's receive window.
 * Returns TRUE if the packet was forwarded, flooded, or filtered,
 * in which case the received packet will have been flushed.
 * Returns FALSE if the packet should be received by the bridge itself,
 * in which case the received packet will not have been flushed.
 */

boolean tbridge_mci_forward (idbtype *inputsw)
{
    enum TBRIDGE_IRB_RETVAL tbirb_status;
    enum TBRIDGE_FWD_RETVAL tbfwd_status;

    hwidbtype *inputhw = inputsw->hwptr;

    /*
     * MCI/cBus transparent bridging is driven from a copy of the
     * received packet's MAC header in tbridge_encap_pak.  Dress
     * up this copy with all the important attributes of the real
     * thing.  Copy only the MAC header from the rx window to the
     * copy.
     */
    tbridge_encap_pak->if_input = inputsw;
    tbridge_encap_pak->rxtype = inputhw->rxtype;
    tbridge_dlli.orig_datagramsize = 
	tbridge_encap_pak->datagramsize = inputhw->rxlength;

    /*
     * Note, normally "inputsw" is the first software IDB. However in the
     * case of subinterface bridging, such as dot10 or ISL vLAN bridging,
     * it should already point to the appropriate subinterface/swidb.
     * To be able to bridge on subinterfaces without also bridging on the
     * the major interface, media drivers must enable bridging on any
     * interface for which at least one subinterface is configured for
     * bridging.
     *
     * Note also that since dot10 vLAN (SDE encapsulated) bridging is
     * not supported on the first swidb, we do not expect TBR_xxx_SDE
     * as a value for tbridge_media in this switch.
     */

    switch (inputsw->tbridge_media) {
 
    case TBR_ETHER:
        tbridge_dlli_init(inputhw, inputsw, inputhw->span_macoffset);
        tbridge_encap_pak->mac_start = (uchar *) tbridge_encap;
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start;
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_YES);
	break;

    case TBR_FDDI:
        tbridge_dlli_init(inputhw, inputsw, FDDI_RECEIVE_OFFSET);
        tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) - FDDI_FC;
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start;
        /* Note that we do not copy the FC. */
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_NOT);
        /* MAC addresses are canonical from the driver. */

	/* 
	 * It is extremely unpleasant that we have to add the following 
	 * classification check because FIP ucode on the 7000/ags does not
	 * classify novell-fddi encapsulation and I cannot get the ucode
	 * folks to make the change since the cbus ucode has come to an end.
	 * rxtype was set to unknown for physical broadcast packets and it
	 * causes those packets being bridged instead of routed even when 
	 * IPX routing is turned on.
	 */

	{
	    sap_hdr *llc = (sap_hdr *)tbridge_encap_pak->info_start;
	    
	    if ((tbridge_encap_pak->rxtype == RXTYPE_UNKNOWN) &&
		(GETSHORT(&llc->dsap) == 
		 (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8))) {
		tbridge_encap_pak->rxtype = RXTYPE_NOVELL2;
	    }
	}
	break;
 
    case TBR_CTR:
        tbridge_dlli_init(inputhw, inputsw, CTR_RECEIVE_OFFSET);
        tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap) -
            TR_ACFC_SIZE;
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start;
        /* Note that we do not copy the ACFC. */
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_NOT);
        /* MAC addresses are non-canonical from the driver. */
        tbridge_mci_swap_mac_address_inline();
        break;
 
    case TBR_FDDI_TRANSIT:
        /*
         * copy the transit mac header first
         */
        tbridge_dlli_init(inputhw, inputsw, FDDI_RECEIVE_OFFSET);
        tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap);
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
            FDDI_SNAP_ENCAPBYTES;
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_YES);
 
        /* Copy up the proxy source address as well */
        tbridge_mci_read_fddi_transit_proxy_inline(inputhw,
                                                   tbridge_encap_pak);
        break;
 
    case TBR_LEX_TRANSIT: /* Fall through */
    case TBR_HDLC_TRANSIT:
        tbridge_dlli_init(inputhw, inputsw, inputhw->span_macoffset);
        tbridge_encap_pak->mac_start = ((uchar *) tbridge_encap);
        /*
         * Use tbridge_dlli.transit_encapsize here since different
         * hdlc-transit interfaces
         * have different sizes of transit header, specifically MIP
         * interfaces have an extra two bytes.
         */
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start -
            tbridge_dlli.transit_encapsize;
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_YES);
        break;
 
    case TBR_ATM_TRANSIT:
        /*
         * RFC1483 header info w/ ATM header looks like this.
         *
         * atm header   - vc_t (ushort)    vcnum
         *                ushort           enctype
         * rfc 1483 hdr - uchar[3]         LLC (always AA AA 03)
         *                uchar[3]         OUI (always 00 80 C2)
         *                ushort           pid (encapsulated media type)
         *                ushort           pad
         *  tr/fddi only  uchar            pad
         *                uchar            FC byte
         *
         * fill  tbridge_dlli w/ the appropriate values depending on
         * packet type
         *
         * if we receive an unknown rfc1483 media type (must be
         * proprietary then) then bail...
         */
 
        if (! tbridge_mci_atm_media_classify(inputhw, inputsw))
            return(FALSE);
 
        break;
    case TBR_LANE_ETHER:
        /*
         * LANE encapsulation header looks like this:
	 *
         * atm header   - vc_t (ushort)    vcnum
         *                ushort           enctype 

         * LEC_ID       - ushort           Lane client id
         *
         * MAC Frame    - Ethernet or Token ring format
	 */

	if (! tbridge_mci_lane_ether_classify(inputhw, inputsw))
            return(FALSE);
	break;
                
    case TBR_FRAME_RELAY_TRANSIT:
	/*
	 * We can have either cisco frame relay or frame relay ietf
	 * headers.  See RFC 1490 for the IETF header format
         */	 
	if (! tbridge_mci_frame_relay_classify(inputsw))
	    return(FALSE);
	break;
	
    case TBR_SMDS_TRANSIT:
	if (! tbridge_mci_smds_classify(inputsw))
	    return(FALSE);
	/* Inputsw might have changed when we read the smds dest. address */
	inputsw = tbridge_encap_pak->if_input;
	break;

    case TBR_ETHER_ISL_VLAN:

	/*
	 * Received an ISL vLAN encapsulated datagram on a bridging
	 * subinterface. Note, by virtue of the fact that the tbridge_media
	 * flag is ISL, then we know this is subinterface bridging and
	 * therefore we should bridge the interior packet as opposed to
	 * transit bridging ISL datagrams received on the major IDB.
	 *
	 * Also the isl_fastswitch() routine will have adjusted the
	 * inappropriately named span_macoffset!
	 *
	 * ISL assumes the interior packet is in Ethernet format, so it's
	 * handled here functionally the same as a V2 encapsulated datagram,
	 * but placed at the end of this switch (inputsw->tbridge_media)
	 * statement so as not to introduce an extra check before FDDI,
	 * WAN bridging etc. (Martin McNealis).
	 */

        tbridge_dlli_init(inputhw, inputsw, inputhw->span_macoffset);
        tbridge_encap_pak->mac_start = (uchar *) tbridge_encap;
        tbridge_encap_pak->datagramstart = tbridge_encap_pak->mac_start;
        tbridge_mci_read_mac_header_inline(inputhw, tbridge_encap_pak,
                                           LENGTH_802_YES);
        break;

    default:
	/*
	 * inputsw->tbridge_media is not valid!
	 * Don't bridge these.
	 */

	errmsg(TBRIDGE_ERR_INVALIDMEDIA,
	       rxtype_string(tbridge_encap_pak->rxtype), inputsw->namestring);
	return(FALSE);
	break;
    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_1) {
	ieee_addrs *addr_start = (ieee_addrs *)(tbridge_encap_pak->addr_start);
	buginf("\nTB1: read %s from %s to %e",
	       rxtype_string(tbridge_encap_pak->rxtype),
	       tbridge_encap_pak->if_input->namestring,
	       addr_start->daddr);
    }
#endif

    tbirb_status = tbridge_irb_inline(tbridge_encap_pak, inputsw);

    switch (tbirb_status) {

    case TBRIDGE_IRB_FORWARD:
	break;

    case TBRIDGE_IRB_RECEIVE:
	return(FALSE);
	break;

    case TBRIDGE_IRB_DISCARD:
	tbridge_mci_flush_datagram_inline(inputhw);
	return(TRUE);
	break;
    }

    tbfwd_status = tbridge_forward_inline(tbridge_encap_pak, TBRPAK_MCI);

    switch (tbfwd_status) {

    case TBRIDGE_FWD_DONE:
	return(TRUE);
	break;

    case TBRIDGE_FWD_FLOOD:
	tbridge_mci_flood(tbridge_encap_pak);
	return(TRUE);
	break;

    case TBRIDGE_FWD_SLOWSWITCH:
	tbridge_mci_slowswitch(tbridge_encap_pak);
	tbridge_mci_flush_datagram_inline(inputhw);
	return(TRUE);
	break;

    case TBRIDGE_FWD_RECEIVE:
	return(FALSE);
	break;
    }

    /* Unreachable statement that the compiler requires. */
    return(FALSE);
}


static boolean tbridge_hes_vbridge_forward (paktype *pak)
{
    idbtype *inputsw = pak->if_input;
    enum TBRIDGE_FWD_RETVAL tbfwd_status;

    tbifd_type	*flood_list;
    ulong	flood_scope;

    tbridge_dlli_init(inputsw->hwptr, inputsw, TR_ACFC_SIZE);

    /* The dlli needs the real input interface. */

    tbridge_dlli.src_media_type = TBR_CTR;

    tbfwd_status = tbridge_forward_inline(pak, TBRPAK_VBR);

    switch (tbfwd_status) {

    case TBRIDGE_FWD_DONE:
        datagram_done(pak);
	break;

    case TBRIDGE_FWD_FLOOD:

	flood_list = pak->if_input->span_ptr->floodQ.qhead;
	flood_scope = TBIFD_FLOOD;

	tbridge_flood_inline(pak, flood_list, flood_scope, FALSE, TBRPAK_MCI);
        datagram_done(pak);
	break;

    case TBRIDGE_FWD_SLOWSWITCH:
    case TBRIDGE_FWD_RECEIVE:
        datagram_done(pak);
	break;
    }

    return(TRUE);
}


/*
 * Worker function for extended bridging access lists. Read
 * up to 4 bytes from the specified offset in an MCI/cBus
 * rx window and return it to the caller. 
 *
 * N.B.: Function assumes that *size* <= 4.
 */

static ulong tbridge_mci_read_bytes(hwidbtype *hwidb,
			     uchar *base,
			     int offset,
			     ushort size)

{
    mciregtype *mci_regptr;
    register uchar *p;
    register ulong value;
    union {
	uchar  byte[8];
	ushort sword[4];
	ulong  dword[2];
    } readbuf;
    
    mci_regptr = hwidb->mci_regptr;
    mci_regptr->argreg = tbridge_dlli.da_offset + ((ushort)offset >> 1);
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    readbuf.dword[0] = mci_regptr->readlong;
    
    /*
     * See if we have to read another word out of the MCI/cBus.
     */
    if ((offset & 1) && (size > 3))
	readbuf.sword[2] = mci_regptr->readshort;
    
    /*
     * Although it appears as if this routine is slow, it was carefully
     * hand profiled and optimized -- please check the generated code.
     */
    p = &readbuf.byte[offset & 1];
    value = *p++;
    while (--size)
	value = (value << 8) | *p++;
    
    return(value);
}


/*
 * tbridge_fddi_transit_receive
 * Special code to receive a bridged fddi datagram -- screwy encapsulation.
 * The datagram is either addressed to one of our MAC level addresses or
 * a ARP request (MAC broadcast) for one of our IP addresses.
 * Return TRUE if we are done with the packet.
 * Return FALSE if we need to throttle the interface due to the lack of
 * pak buffer.
 */
boolean tbridge_fddi_transit_receive (hwidbtype *idb)
{
    mciregtype *regptr;
    ushort bytes;
    paktype *pak;
    ulong ultemp;
    spantype *span;
    btetype *bte;
    /* NB--not subinterface ready! */
    idbtype *swidb = idb->firstsw;
    ether_hdr *ether;

    regptr = idb->mci_regptr;
    if (!swidb->span_ptr || !(idb->fddi_options & FDDI_ENCAPSULATE)) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }

    /*
     * Subtract the bridge encapsulation overhead from the datagram size.
     */
    bytes = idb->rxlength;
    bytes -= FDDI_SNAP_ENCAPBYTES;
    pak = input_getbuffer(bytes, idb);
    if (!pak)
	return(FALSE);

    /* Interior packet is virtual-ether */
    ether = (ether_hdr *)tbridge_encap_pak->mac_start;

    /*
     * The following code assumes that we are looking up the destination
     * address first -- we need to add the source address into our bridge
     * data  base. The reason being, our reply needs to know whether the
     * request came from on net (a station on the fddi ring) or via a
     * bridged request.
     */
    ultemp = ether->saddr[MAC_HASH1];
    ultemp ^= ether->saddr[MAC_HASH2];
    span = swidb->span_ptr;
    for (bte = span->bridge_tbl[ultemp]; bte; bte = bte->bte_next) {
	if (!ieee_equal((uchar *)bte->bte_mac_addr.sword, &ether->saddr[0]))
            continue;
	if (!bte->bte_attributes)
            if ((bte->bte_interface) && (bte->bte_interface != swidb)) {

                /*
                 * A station's MAC address has reappeared behind a different
                 * interface from that on which we learned it. i.e. the station
                 * has been connected on a different subnet/interface.
                 * Therefore, in addition to reflecting this change in the
                 * Bridge Table, check that if we're autonomously bridging, we 
                 * update the SP Cache aswell. This is done in two parts, 
                 * firstly the IFA is rewritten, then the BCE access filter 
                 * permissions.
                 */
                bte->bte_interface = swidb;

                if (bte->bte_interface->hwptr->cbus_bridge_enable
                    && bte->bte_bce_addr) {
                    reg_invoke_cbus_bce_update_ifa(bte);
                    reg_invoke_cbus_bce_update_permissions(bte);
                }

            } else {

                bte->bte_interface = swidb;
            }

	bte->bte_rxcount++;
	goto src_found;
    }

    /*
     * The source of this datagram has not been found -- add it to the
     * bridging database if not a multicast.
     */
    if (ether->saddr[0] & 0x01)
	goto src_found;

    bte = dequeue(&bteQ);
    if (bte) {
	bte_count--;
	bte->bte_mac_addr.sword[0] = *((ushort *)(&ether->saddr[0]));
	bte->bte_mac_addr.straddle.two = *((ulong *)(&ether->saddr[2]));
	bte->bte_next = (swidb->span_ptr)->bridge_tbl[ultemp];
	(idb->firstsw->span_ptr)->bridge_tbl[ultemp] = bte;
	bte->bte_attributes = 0;
	bte->bte_action = BRIDGE_FORWARD;
	bte->bte_interface = swidb;
	bte->bte_txcount = 0;
	bte->bte_prev_rxcount = 0;
	bte->bte_rxcount = 1;

	/*
	 * Store the FDDI sending station address in the bridge table.
	 */
	regptr->argreg = FDDI_RECEIVE_OFFSET + IEEEWORDS + 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	PUTSHORT(&bte->bte_proxy_addr.proxy_mac[0], regptr->readshort);
	PUTSHORT(&bte->bte_proxy_addr.proxy_mac[2], regptr->readshort);
	PUTSHORT(&bte->bte_proxy_addr.proxy_mac[4], regptr->readshort);
    }
    else
	span->learned_entry_discards++;

 src_found:
    /*
     * Determine the ethernet encapsulation style.
     */
    pak->datagramsize = bytes;
    ether_parse_packet(idb, pak, (uchar *)&ether->type_or_len);

    /*
     * Set pointer to start of packet and read it into packet buffer.
     * Hand to raw input process and flush datagram in MCI RX buffer.
     */
    regptr->argreg = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);
    regptr->cmdreg = MCI_CMD_RX_FLUSH;
    idb->counters.rx_cumbytes += bytes;
    idb->counters.inputs++;
    pak->flags |= PAK_TBRIDGE_IN;
    (void)raw_enqueue(idb, pak);
    return(TRUE);
}


/*
 * In the beginning ...
 *
 * The world of MCI/cBus transparent/translational bridging begins
 * with the functions registered at the end of this function which
 * attempt to forward or flood a received packet.
 * 
 * The general outline of what follows is:
 *
 * - a driver interrupt routine (mci_interrupt, or cbus_interrupt)
 *   invokes one of the forwarding functions defined here *   through
 *   a registry
 *
 * - these forwarding functions do a bit of interface-specific preparation
 *   before calling a completely generic forwarding inline to perform
 *   platform-independent bridging
 *
 * - based on the result of the generic forwarding inline, the forwarding
 *   functions registered here do one of four things:
 *
 *   - if the packet was forwarded, they return to the driver
 * 
 *   - if the packet is to be flooded, they call a flooding function
 *     to do a bit of interface-specific preparation before calling
 *     a completely generic flooding inline to perform platform-independent
 *     flooding, and return to the driver 
 *
 *   - if the packet is to be slow switched, they call a slow switching
 *     funtion to do interface-specific preparation and queueing of a
 *     packet for process-level bridging
 * 
 *   - if the packet is to be received, they return to the driver with
 *     an indication that the packet needs to be received by the bridge
 *     and queued for process-level handling
 *
 * The key to the generic inlines is the code switch.  It is set by the
 * caller to indicate whether the packet is directly accessible in a network
 * buffer (as is the case on low-end and IRSP platforms), or only
 * indirectly accessible in an MCI/cBus receive window (as is the case
 * on high-end platforms).  This switch is used to select platform-specific
 * operations where required in the generic bridging code. 
 *
 * That's all there is to it.
 *
 * In this function we allocate the network buffers used throughout the
 * hes tbridge code to forward and flood packets.  Forwarding is usually
 * done without reading much more than the MAC header of a received packet.
 * tbridge_encap_pak is pre-allocated for this purpose, and tbridge_encap is
 * set to point to the data area associated with this packet.  This packet
 * is only ever used to record packet attributes as determined from the
 * link-layer header in the packet's data area.  It has no other contents to
 * speak of.
 *
 * Flooding is done by reading the entire received packet into a network
 * buffer, flushing the received packet, and then translating the packet
 * for each outbound interface in the flood list. tbridge_flood_pak is
 * pre-allocated for this purpose.
 * 
 */

static void tbridge_hes_init (subsystype* subsys)

{
    int flood_pak_size;

    /*
     * Allocate the permanent network buffers we will be using
     * to forward, flood, and process-bridge packets.
     *
     * Note that getbuffer adds ENCAPBYTES to the size and sets
     *  - pak->datagramstart = pak->data_area + ENCAPBYTES
     *  - pak->network_start = pak->data_area + ENCAPBYTES
     */

    /*
     * tbridge_encap_pak is used to assemble just the MAC header
     * of a packet from an MCI/cBus interface.
     */
    tbridge_encap_pak = getbuffer(SMALLDATA);
    if (tbridge_encap_pak == NULL) {
	return;
    }

    tbridge_encap_pak->enctype = ET_NULL;
    tbridge_encap_pak->linktype = LINK_BRIDGE;

    /*
     * Since this encap area will be used primarily to assemble
     * a fragment of an incoming packet header beginning with
     * the DA and SA, assign it to a long word aligned position
     * within the packet's data area that leaves enough room
     * for a full ETHER SNAP header to be read in.
     */

    tbridge_encap = (charlong *) (tbridge_encap_pak->datagramstart -
				  ((ETHER_SNAP_ENCAPBYTES + 3) & ~0x3));
    /* xaccess longword will be read beyond pak_center */

    /*
     * Get a buffer to use for flooding at interrupt level
     */

    flood_pak_size = MAXETHERSIZE;
    if (nTRs)
	flood_pak_size = max(flood_pak_size, MAXTRSIZE);
    if (nfddinets)
	flood_pak_size = max(flood_pak_size, MAXFDDISIZE);
    if (nhssinets)
	flood_pak_size = max(flood_pak_size, MAXDS3SIZE);

    /*
     * Allocate 3 extra bytes so that when mci2buffer is used to
     * to do maximum-length longword copies to this buffer, there
     * will be enough room for any potential overwrite.
     * Why is this not caught by TRAILBYTES?
     */
    tbridge_flood_pak = getbuffer(flood_pak_size + 3);
    if (!tbridge_flood_pak) {
	return;
    }

    tbridge_flood_pak->enctype = ET_NULL;
    tbridge_flood_pak->linktype = LINK_BRIDGE;

    /*
     * Open for business ...
     */
    reg_add_tbridge_mci_forward(tbridge_mci_forward, "tbridge_mci_forward");
    reg_add_vbridge_forward(tbridge_hes_vbridge_forward, 
			       "tbridge_hes_vbridge_forward");
    reg_add_tbridge_read_bytes(tbridge_mci_read_bytes,
			       "tbridge_mci_read_bytes");
    reg_add_tbridge_fddi_transit_receive(tbridge_fddi_transit_receive,
					 "tbridge_fddi_transit_receive");
    reg_add_print_copyright(tbridge_print_message, "tbridge_print_message");
}

#define TBRHES_MAJVERSION 1
#define TBRHES_MINVERSION 0
#define TBRHES_EDITVERSION 1

SUBSYS_HEADER(tbridge_hes, TBRHES_MAJVERSION, TBRHES_MINVERSION,
	      TBRHES_EDITVERSION,
              tbridge_hes_init, SUBSYS_CLASS_PROTOCOL, 
	      "seq: tbridge",
              "req: fastswitch");
