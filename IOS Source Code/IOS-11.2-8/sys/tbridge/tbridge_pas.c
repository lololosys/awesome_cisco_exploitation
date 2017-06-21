/* $Id: tbridge_pas.c,v 3.1.36.5 1996/08/28 13:16:47 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_pas.c,v $
 *------------------------------------------------------------------
 * tbridge_pas.c - Port Adaptor based system transparent bridging
 *
 * January 1996, Michael Beesley
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tbridge_pas.c,v $
 * Revision 3.1.36.5  1996/08/28  13:16:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.36.4  1996/07/23  13:29:46  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.36.3  1996/05/04  05:09:13  wilber
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
 * Revision 3.1.36.2  1996/04/04  09:38:45  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.1.36.1  1996/03/22  00:14:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/03/18  23:36:54  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.1  1996/02/01  08:10:05  mbeesley
 * Branch: ELC_branch
 * Bring the joys of transparent bridging to the c7100.
 *
 * Revision 3.1  1996/01/30  21:54:26  mbeesley
 * Add placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "master.h"
#include "mgd_timers.h"
#include "globs.h"
#include "logger.h"
#include "../tbridge/msg_tbridge.c"	/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "access.h"
#include "address.h"
#include "config.h"

#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"

#include "../os/chunk.h"
#include "../ui/debug.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/ether_inline.h"
#include "../if/atm.h"
#include "../if/atm_debug_flags.h"

/* For Lan Emulation */

#include "../atm/ilmi_api.h"
#include "../atm/atmsig_api.h"
 
#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../lane/lane_registry.h"

#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"

#include "../srt/span.h"
#include "../srt/bridgeaccess.h"
#include "../srt/srt_registry.h"
#include "../srt/fddisrb.h"

#include "../if/fddi_inline.h"

#include "../smf/smf_public.h"
#include "../smf/smf_private.h"
#include "../smf/smf_private_inline.h"

#include "../hes/if_mci.h"

#include "tbridge_public.h"
#include "tbridge_common_inline.h"
#include "tbridge_debug.h"
#include "tbridge_sde.h"
#include "tbridge_sde_inline.h"
#include "../vlan/vlan.h"
#include "tbridge_isl_inline.h"

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
 * These defines cause all the inlines to be compiled as
 * functions for debugging purposes.  Retain them for now
 * until the code bloat that results from static inlines
 * is under control.  Until then, performance testing will
 * not yield meaningful results.  Restore them to:

#define STATIC	static
#define INLINE	inline
 */

#define STATIC static
#define INLINE inline

/*
 * This define allows a subset of the TBRIDGE_PAS code for the FDDI features.
 */
#define TBRIDGE_PAS

#include "tbridge_tlb_inline.h"
#include "tbridge_irb_inline.h"
#include "tbridge_les_inline.h"
#include "tbridge_inline.h"

/*
 * tbridge_pas_slowswitch:
 * Copy a packet from an interface for process-level bridging,
 * and translate it to ether in preparation for slow-switching it.
 * Note, in the first version of transparent bridging the packets have
 * been coalesced into contigous buffers.
 */
static void tbridge_pas_slowswitch (paktype *pak)
{
    tbridge_slowswitch_inline(pak);
}

/*
 * tbridge_pas_flood:
 * Take care of any pas-specific preparation required by
 * tbridge_flood_inline to perform platform-independent flooding.
 */
static void tbridge_pas_flood (paktype *pak)
{
    tbifd_type	*flood_list;
    ulong	flood_scope;
    idbtype     *inputsw;
    hwidbtype   *inputhw;



    inputsw = pak->if_input;
    inputhw = inputsw->hwptr;
    
    if (is_bvi(inputhw)) {
    	flood_list = spanarray[inputhw->unit]->floodQ.qhead;
    }
    else {
    	flood_list = inputsw->span_ptr->floodQ.qhead;
    }
    flood_scope = TBIFD_FLOOD;

    if (tbridge_dlli.dlli_attributes & DLLI_IPMCAST)
	flood_scope = reg_invoke_tbridge_cmf(pak, &flood_list);

    if (TBIFD_NO_FLOOD == flood_scope) {
	datagram_done(pak);
	return;
    }

    tbridge_flood_inline(pak, flood_list, flood_scope, TRUE, TBRPAK_BFR);
}

/*
 * tbridge_pas_forward:
 * Take care of any pas specific preparation required by
 * tbridge_forward_inline to perform platform-independent bridging.
 *
 * The received packet is in a network packet buffer.
 * Returns TRUE if the packet was forwarded, flooded, or filtered,
 * in which case the received packet will have been flushed.
 * Returns FALSE if the packet should be received by the bridge itself,
 * in which case the received packet will not have been flushed.
 *
 * Note: In the first version of transparent bridging, any scattered
 * packet being passed in to us here will be coalesced into a contigous
 * packet buffer before being sent thru the bridging code.
 */
static boolean tbridge_pas_forward (paktype *fs_pak)
{
    enum TBRIDGE_IRB_RETVAL tbirb_status;
    enum TBRIDGE_FWD_RETVAL tbfwd_status;
    idbtype *inputsw;
    hwidbtype *inputhw;
    ieee_addrs *addr_start;
    paktype *pak;
    uchar *original_info_start;

    if (fs_pak->particlequeue.count != 0) {
	/*
	 * Get a buffer and copy the scattered frame into it
	 */
	pak = getbuffer(fs_pak->datagramsize);
/*
 * Possible BOGOSITY ALERT!!!  If the pak_coalesce fails but you got a
 * valid pak, it looks like pak will be lost, creating a buffer leak.
 */
	if (!pak || !pak_coalesce(fs_pak, pak)) {
	    return(FALSE);
	}
	pak->if_input = fs_pak->if_input;
    } else {
	pak = fs_pak;
    }

    /*
     * Set things up
     */
    inputsw = pak->if_input;
    inputhw = inputsw->hwptr;
    addr_start = (ieee_addrs *)pak->addr_start;
    pak->linktype = LINK_BRIDGE;	/* identify for fancy queueing oqueue */
    tbridge_dlli.orig_datagramsize = pak->datagramsize;
    original_info_start = pak->info_start;

    /*
     * Preprocess the frame
     */
    switch (inputsw->tbridge_media) {

    case TBR_ETHER:
        tbridge_dlli_init(inputhw, inputsw, 0);
        break;
 
    case TBR_TR:
        tbridge_dlli_init(inputhw, inputsw, TR_ACFC_SIZE);
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        break;
 
    case TBR_FDDI:
        tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
        break;
 
    case TBR_FDDI_TRANSIT:
        {
            snap_hdr *link;
 
            tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
            link = (snap_hdr *)pak->info_start;
 
            if ((link->dsap == SAP_SNAP) &&
                (link->type == TYPE_BRIDGE)) {
 
                /*
                 * Deencapsulate the Ethernet frame from the FDDI
                 * wrapper that surrounds it
                 */
                pak->rxtype = ether_decode_encapsulation_inline
                    (pak, (ether_hdr *)link->data);
                pak->flags |= PAK_TBRIDGE_IN;
            }
        }
        break;
 
    case TBR_LEX_TRANSIT: /* Fall through */
    case TBR_HDLC_TRANSIT:
        {
            uchar *frame;
 
            tbridge_dlli_init(inputhw, inputsw, HDLC_HEADER_SIZE);
            frame = pak->datagramstart + tbridge_dlli.transit_encapsize;
 
            /*
             * Deencapsulate the Ethernet frame from the serial wrapper
             * that surrounds it
             */
            pak->rxtype =
                ether_decode_encapsulation_inline(pak, (ether_hdr *)frame);
            pak->flags |= PAK_TBRIDGE_IN;
        }
        break;
 
    case TBR_ATM_TRANSIT:
        if (! tbridge_les_atm_media_classify(inputhw, inputsw, pak)) {
	    if (pak != fs_pak) {
		datagram_done(pak);
	    }
            return(FALSE);
	}
        break;

    case TBR_FRAME_RELAY_TRANSIT:
	if (! tbridge_les_frame_relay_classify(inputhw, inputsw, pak)) {
	    if (pak != fs_pak) {
		datagram_done(pak);
	    }
	    return(FALSE);
	}
	break;

    case TBR_LANE_ETHER:
	if (! tbridge_les_lane_ether_classify(inputhw, inputsw, pak)) {
	    if (pak != fs_pak) {
		datagram_done(pak);
	    }
	    return(FALSE);
	}
        break;

    case TBR_SMDS_TRANSIT:
	if (! tbridge_les_smds_classify(inputhw, inputsw, pak)) {
	    if (pak != fs_pak) {
		datagram_done(pak);
	    }
	    return(FALSE);
	}
	/* inputsw might have changed when we read the smds dest address */
	inputsw = pak->if_input;
	break;

    case TBR_ETHER_ISL_VLAN:

        /*
         * Received an ISL vLAN encapsulated datagram on a bridging
         * subinterface. Note, by virtue of the fact that the tbridge_media
         * flag is ISL, then we know this is subinterface bridging and
         * therefore we should bridge the interior packet as opposed to
         * transit bridging ISL datagrams received on the major IDB.
         *
         * Also the bridge_interior_isl_packet() routine will have adjusted
	 * the datagramsize and datagramstart packet attributes.
         *
         * ISL assumes the interior packet is in Ethernet format, so it's
         * handled here functionally the same as a V2 encapsulated datagram,
         * but placed at the end of this switch (inputsw->tbridge_media)
         * statement so as not to introduce an extra check before FDDI,
         * WAN bridging etc. (Martin McNealis).
         */

        tbridge_dlli_init(inputhw, inputsw, 0);
        break;

    default:
	/*
	 * inputsw->tbridge_media is not valid!
	 * Don't bridge these.
	 */
	errmsg(TBRIDGE_ERR_INVALIDMEDIA, rxtype_string(pak->rxtype),
	       inputsw->namestring);
	if (pak != fs_pak) {
	    datagram_done(pak);
	}
	return(FALSE);
	break;
    }

    tbirb_status = tbridge_irb_inline(pak, inputsw);

    switch (tbirb_status) {

    case TBRIDGE_IRB_FORWARD:
	break;

    case TBRIDGE_IRB_RECEIVE:
	/*
	 * When we clone the frame, there is no need to recover
	 * any encaps from pak as it is a copy of the fs_pak, which
	 * will be returned to the calling device driver IE subsequent 
	 * processing will occur on fs_pak.
	 */
	if (pak == fs_pak) {
          /* 
           * Put the original info_start back. The SDE case will change its 
           * value which cause driver point to the wrong field.
           */
            pak->info_start = original_info_start ;
	    tbridge_les_encapsulation_recovery_inline(inputsw, pak);
	} else {
	    datagram_done(pak);
	}
	return(FALSE);
	break;

    case TBRIDGE_IRB_DISCARD:
	if (pak != fs_pak) {
	    datagram_done(pak);
	}
	datagram_done(fs_pak);
	return(TRUE);
	break;
    }

    tbfwd_status = tbridge_forward_inline(pak, TBRPAK_BFR);

    switch (tbfwd_status) {

    case TBRIDGE_FWD_DONE:
	if (pak != fs_pak) {
	    datagram_done(fs_pak);
	}
	return(TRUE);
	break;

    case TBRIDGE_FWD_FLOOD:
	tbridge_pas_flood(pak);
	if (pak != fs_pak) {
	    datagram_done(fs_pak);
	}
	return(TRUE);
	break;

    case TBRIDGE_FWD_SLOWSWITCH:
	tbridge_pas_slowswitch(pak);
	if (pak != fs_pak) {
	    datagram_done(fs_pak);
	}
	return(TRUE);
	break;

    case TBRIDGE_FWD_RECEIVE:
	/*
	 * When we clone the frame, there is no need to recover
	 * any encaps from pak as it is a copy of the fs_pak, which
	 * will be returned to the calling device driver IE subsequent 
	 * processing will occur on fs_pak.
	 */
	if (pak == fs_pak) {
          /* 
           * Put the original info_start back. The SDE case will change its 
           * value which cause driver point to the wrong field.
           */
            pak->info_start = original_info_start ;
	    tbridge_les_encapsulation_recovery_inline(inputsw, pak);
	} else {
	    if (is_bvi(pak->if_input->hwptr)) {
		/*
		 * Input interface of 'pak'(which we pass in to the
		 * bridging code) has been overwritten to the BVI
		 * when we made bridge/route decision. Save it to
		 * fs_pak->if_input before we datagram_done() 'pak'.
		 *
		 * On 7200 platform, we do NOT set PAK_INPUTQ for
		 * pak(and fs_pak), but we have called change_if_input()
		 * in tbridge_irb_pkt_processing_inline() for irb
		 * routable packets, which assumes that we have
		 * PAK_INPUTQ set to do the right i/p and o/p
		 * hwidb->input_qcount accounting. To avoid
		 * the problem, we repair the 'damage' that
		 * set_if_input()(called by change_if_input) has
		 * done to us by calling clear_if_input(pak) to do
		 * two things here,
		 * - pak->flags &= ~PAK_INPUTQ
		 * - atomic_decrement(&bvi->hwptr->input_qcount)
		 *
		 * Do NOT use change_if_input(fs_pak, pak->if_input)
		 * here because we do not want to change/deal-with
		 * input_qcount here. By calling change_if_input(),
		 * you will unncessarily increase BVI's input_qcount.
		 */
		fs_pak->if_input = pak->if_input;
		fs_pak->irb_rcvd_input = pak->irb_rcvd_input;
		clear_if_input(pak);
	    }
	    datagram_done(pak);
	}
	return(FALSE);
	break;
    }

    /* Unreachable statement that the compiler requires. */
    return(FALSE);
}

/*
 * tbridge_pas_init:
 * Initialize the transparent bridging support.
 */
static void tbridge_pas_init (subsystype* subsys)
{
    reg_add_tbridge_forward(tbridge_pas_forward, "tbridge_pas_forward");
    reg_add_print_copyright(tbridge_print_message, "tbridge_print_message");
}

#define TBRPAS_MAJVERSION 	1
#define TBRPAS_MINVERSION 	0
#define TBRPAS_EDITVERSION 	1

SUBSYS_HEADER(tbridge_pas, TBRPAS_MAJVERSION, TBRPAS_MINVERSION,
	      TBRPAS_EDITVERSION,
              tbridge_pas_init, SUBSYS_CLASS_PROTOCOL, NULL,
              "req: fastswitch");

/* end of file */
