/*-------------------------------------------------------------------------
 *
 * $Id: tbridge_les.c,v 3.8.4.4 1996/08/28 13:16:44 thille Exp $
 *
 * tbridge/tbridge_les.c
 *
 * October 1994, Steve Southam
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 *
 * All rights reserved.
 * 
 * Low end functions for transparent bridging.
 * These functions include interface-specific functions to prepare the
 * arguments required by generic transparent bridging functions for 
 * forwarding and flooding. The low-end system (LES) transparent bridging
 * subsystem is initialized here.
 *
 * Based on tbridge/tbridge_hes.c
 *
 *-------------------------------------------------------------------------
 * $Log: tbridge_les.c,v $
 * Revision 3.8.4.4  1996/08/28  13:16:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.4.3  1996/05/04  05:09:05  wilber
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
 * Revision 3.8.4.2  1996/03/23  22:49:39  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/03/18  22:12:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/16  07:39:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.12.2  1996/03/07  10:52:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  18:52:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/07  23:33:12  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.7  1996/02/13  08:16:43  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.6  1996/01/05  04:57:21  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/17  06:12:42  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.4  1995/12/14  20:59:22  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.3  1995/11/20  19:29:36  speakman
 * CSCdi42919:  CSCdi41837 broke lego images
 * Move SMF initialization into platform-independent code.
 *
 * Revision 3.2  1995/11/17  18:45:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:31:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/26  00:30:45  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Initialize tbridge SMF chunk.
 *
 * Revision 2.4  1995/09/30  22:44:24  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Exchange the safety of data hiding for a 7% performance improvement
 * in the 64 byte pps no-drop rate.  Use an inline to do the SMF match
 * instead of a function call, and forgo the pointer safety checks in
 * ieee_equal in the bte locator to do a direct MAC address comparison.
 *
 * Revision 2.3  1995/08/10  16:41:11  psandova
 * CSCdi38322:  SR/TLB fddi-tr improperly translates functional addresses
 *
 * Swap the low-end FDDI addresses in the slow switch inline function
 * instead of the low-end specific function to catch both the unicast
 * and multicast paths.  This is a hack and it will require a lot of
 * changes to do it right.
 *
 * Revision 2.2  1995/06/10  17:52:42  pitargue
 * CSCdi35638:  packets going to process level from the 4500 atm nim
 * crashes router
 * make sure packets to be processed at process level is unmunged.
 * fix comments.
 *
 * Revision 2.1  1995/06/07  23:03:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-------------------------------------------------------------------------
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

#include "tbridge_les_private.h"

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

#define STATIC
#define INLINE

#include "tbridge_tlb_inline.h"
#include "tbridge_irb_inline.h"
#include "tbridge_les_inline.h"
#include "tbridge_inline.h"



/*
 * Copy a packet from an les interface for process-level bridging,
 * and translate it to ether in preparation for slow-switching it.
 */

void tbridge_les_slowswitch (paktype *pak)
{
    tbridge_slowswitch_inline(pak);
}


/*
 * Take care of any les-specific preparation required by
 * tbridge_flood_inline to perform platform-independent flooding.
 */

void tbridge_les_flood (paktype *pak)
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
 * Take care of any low-end specific preparation required by
 * tbridge_forward_inline to perform platform-independent bridging.
 *
 * The received packet is in a network packet buffer.
 * Returns TRUE if the packet was forwarded, flooded, or filtered,
 * in which case the received packet will have been flushed.
 * Returns FALSE if the packet should be received by the bridge itself,
 * in which case the received packet will not have been flushed.
 */
boolean tbridge_les_forward (paktype *pak)
{
    enum TBRIDGE_IRB_RETVAL tbirb_status;
    enum TBRIDGE_FWD_RETVAL tbfwd_status;

    idbtype *inputsw = pak->if_input;
    hwidbtype *inputhw = inputsw->hwptr;

    ieee_addrs *addr_start  = (ieee_addrs *)pak->addr_start;

    pak->linktype = LINK_BRIDGE;	/* identify for fancy queueing oqueue */

    tbridge_dlli.orig_datagramsize = pak->datagramsize;

    switch (inputsw->tbridge_media) {

    case TBR_ETHER:
        tbridge_dlli_init(inputhw, inputsw, 0);
        break;
 
    case TBR_TR:
        tbridge_dlli_init(inputhw, inputsw, TR_ACFC_SIZE);
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        break;
 
    case TBR_VBRIDGE:
        tbridge_dlli_init(inputhw, inputsw, TR_ACFC_SIZE);
        tbridge_dlli.src_media_type = TBR_TR;
        break;
 
    case TBR_FDDI:
        tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        break;
 
    case TBR_FDDI_TRANSIT:
        {
            snap_hdr *link;
 
            tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
            link = (snap_hdr *)pak->info_start;
 
            ieee_swap(addr_start->daddr, addr_start->daddr);
            ieee_swap(addr_start->saddr, addr_start->saddr);
 
            if ((link->dsap == SAP_SNAP) &&
                (link->type == TYPE_BRIDGE)) {
 
                /*
                 * Deencapsulate the Ethernet frame from the FDDI
                 * wrapper that surrounds it
                 */
                pak->rxtype = ether_decode_encapsulation_inline
                    (pak, (ether_hdr *)link->data);
 
#ifdef TBRIDGE_DEBUG
                addr_start = (ieee_addrs *)pak->addr_start;
#endif
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
 
#ifdef TBRIDGE_DEBUG
            addr_start = (ieee_addrs *)pak->addr_start;
#endif
            pak->flags |= PAK_TBRIDGE_IN;
        }
        break;
 
    case TBR_ATM_TRANSIT:
        if (! tbridge_les_atm_media_classify(inputhw, inputsw, pak))
            return(FALSE);
 
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)pak->addr_start;
#endif
        break;

    case TBR_FRAME_RELAY_TRANSIT:
	if (! tbridge_les_frame_relay_classify(inputhw, inputsw, pak))
	    return(FALSE);
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)pak->addr_start;
#endif
	break;
    case TBR_LANE_ETHER: {
	if (! tbridge_les_lane_ether_classify(inputhw, inputsw, pak))
	    return(FALSE);
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)pak->addr_start;
#endif
    }
        break;

    case TBR_SMDS_TRANSIT:
	if (! tbridge_les_smds_classify(inputhw, inputsw, pak))
	    return(FALSE);
	/* inputsw might have changed when we read the smds dest address */
	inputsw = pak->if_input;
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)pak->addr_start;
#endif
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

    case TBR_FDDI_SDE:
	/*
	 * Even thougth we never bridge for media type TBR_FDDI_SDE
	 * here, For packet goes to process level on the low end
	 * system we can still trigger a lot of meaningless errmsg 
	 * just by attempting bridging it here. 
	 * Note: For LES reg_invoke_rsp_queue_fddi_for_process() 
	 * called by fs path is actually an noop
	 */
	return (FALSE);
	break;

    default:
	/*
	 * inputsw->tbridge_media is not valid!
	 * Don't bridge these.
	 */
	errmsg(TBRIDGE_ERR_INVALIDMEDIA, rxtype_string(pak->rxtype),
	       inputsw->namestring);
	return(FALSE);
	break;
    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_1) {
	buginf("\nTB1: read %s from %s"
	       "\n     %e to %e",
	       rxtype_string(pak->rxtype), pak->if_input->namestring,
	       addr_start->saddr, addr_start->daddr);
    }
#endif	/* TBRIDGE_DEBUG */

    tbirb_status = tbridge_irb_inline(pak, inputsw);

    switch (tbirb_status) {

    case TBRIDGE_IRB_FORWARD:
	break;

    case TBRIDGE_IRB_RECEIVE:
        tbridge_les_encapsulation_recovery_inline(inputsw, pak);
	return(FALSE);
	break;

    case TBRIDGE_IRB_DISCARD:
	datagram_done(pak);
	return(TRUE);
	break;
    }

    tbfwd_status = tbridge_forward_inline(pak, TBRPAK_BFR);

    switch (tbfwd_status) {

    case TBRIDGE_FWD_DONE:
	return(TRUE);
	break;

    case TBRIDGE_FWD_FLOOD:
	tbridge_les_flood(pak);
	return(TRUE);
	break;

    case TBRIDGE_FWD_SLOWSWITCH:
	tbridge_les_slowswitch(pak);
	return(TRUE);
	break;

    case TBRIDGE_FWD_RECEIVE:
        tbridge_les_encapsulation_recovery_inline(inputsw, pak);
	return(FALSE);
	break;
    }

    /* Unreachable statement that the compiler requires. */
    return(FALSE);
}


void tbridge_les_init (subsystype* subsys)

{
    /*
     * Open for business ...
     */
    reg_add_tbridge_forward(tbridge_les_forward, "tbridge_les_forward");
    reg_add_vbridge_forward(tbridge_les_forward, "tbridge_les_forward");
    reg_add_print_copyright(tbridge_print_message, "tbridge_print_message");

}

#define TBRLES_MAJVERSION 1
#define TBRLES_MINVERSION 0
#define TBRLES_EDITVERSION 1

SUBSYS_HEADER(tbridge_les, TBRLES_MAJVERSION, TBRLES_MINVERSION,
	      TBRLES_EDITVERSION,
              tbridge_les_init, SUBSYS_CLASS_PROTOCOL, NULL,
              "req: fastswitch");
