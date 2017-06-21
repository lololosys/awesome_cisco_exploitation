/* $Id: tbridge_rsp.c,v 3.12.4.6 1996/08/28 13:16:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_rsp.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_rsp.c
 *
 * October 1994, Marty Cieslak
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * RSP specific functions for transparent bridging.  These functions
 * include interface-specific functions to prepare the arguments
 * required by generic transparent bridging functions for forwarding
 * and flooding.  The RSP transparent bridging subsystem is
 * initialized here.
 *
 *------------------------------------------------------------------
 * $Log: tbridge_rsp.c,v $
 * Revision 3.12.4.6  1996/08/28  13:16:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.4.5  1996/07/23  13:29:51  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.12.4.4  1996/05/14  04:30:19  speakman
 * CSCdi57584:  CGMP packets not seen by RSP
 * Branch: California_branch
 * Calculate network_start when copying RSP packets for flooding.
 *
 * Revision 3.12.4.3  1996/05/04  05:09:18  wilber
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
 * Revision 3.12.4.2  1996/03/23  22:49:43  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.12.4.1  1996/03/18  22:12:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.3  1996/03/16  07:39:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.12.2  1996/03/07  10:52:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.1  1996/02/20  18:53:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/11  11:11:11  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.11  1996/03/07  23:33:18  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.10  1996/02/13  08:16:45  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.9  1996/02/05  03:35:15  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.8  1996/01/05  04:57:22  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.7  1995/12/14  20:59:27  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - convert
 * smds destination address to swidb to use for bridging on input packets
 *
 * Revision 3.6  1995/12/12  03:09:56  mcieslak
 * CSCdi45412:  TBR inline has grown too large
 * Use non-inline routine instead of inline.
 *
 * Revision 3.5  1995/12/01  20:45:48  mcieslak
 * CSCdi44797:  Extended Access Lists for Transparent Bridging Broken
 * Fix invalid comparisons during the determination of whether a second
 * read access to MEMD is required.
 *
 * Revision 3.4  1995/11/20  19:29:38  speakman
 * CSCdi42919:  CSCdi41837 broke lego images
 * Move SMF initialization into platform-independent code.
 *
 * Revision 3.3  1995/11/20  15:25:30  pitargue
 * CSCdi42745:  Bad SAP error received when ping over atm remote bridging.
 * remember the original datagramsize
 *
 * Revision 3.2  1995/11/17  18:45:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:31:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/26  00:30:54  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Initialize tbridge SMF chunk.
 *
 * Revision 2.4  1995/09/30  22:44:25  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Exchange the safety of data hiding for a 7% performance improvement
 * in the 64 byte pps no-drop rate.  Use an inline to do the SMF match
 * instead of a function call, and forgo the pointer safety checks in
 * ieee_equal in the bte locator to do a direct MAC address comparison.
 *
 * Revision 2.3  1995/06/28  22:34:39  mcieslak
 * Incorporate code review changes.
 * CSCdi36470:  %SYS-3-SUPNONE: Registry 6 doesnt exist while booting
 *
 * Revision 2.2  1995/06/28  20:53:05  mcieslak
 * Needed to insure that the registry has been created before we try
 * to add to it.
 * CSCdi36470:  %SYS-3-SUPNONE: Registry 6 doesnt exist while booting
 *
 * Revision 2.1  1995/06/07  23:03:29  hampton
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

#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"

#include "../ui/debug.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/atm.h"

#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"

#include "../srt/span.h"
#include "../srt/bridgeaccess.h"
#include "../srt/srt_registry.h"
#include "../srt/fddisrb.h"

#include "../if/fddi_inline.h"

#include "../hes/if_cmd.h"
#include "../hes/if_fip.h"
#include "../hes/hes_fip_inline.h"
#include "../hes/if_mci.h"

#include "../smf/smf_public.h"

#include "../smf/smf_private.h"
#include "../smf/smf_private_inline.h"

#include "tbridge_public.h"
#include "tbridge_common_inline.h"
#include "tbridge_debug.h"
#include "tbridge_sde.h"
#include "tbridge_sde_inline.h"
#include "tbridge_rsp_private.h"

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_cache.h"
#include "../src-rsp/rsp_if.h"
#include "../ui/debug.h"
#include "../if/atm_debug_flags.h"

/* For Lan Emulation */

#include "../atm/ilmi_api.h"
#include "../atm/atmsig_api.h"
 
#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../lane/lane_registry.h"

#include "../vlan/vlan.h"


/*
 * External data definitions:
 */

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
 * This define exists solely to compensate for the compiler's
 * inability to eliminate code inside conditionals on constants
 * known to the compiler at compile time.  These constants are
 * used herein as code switches, the intent of which was to allow
 * tbridge_tlb_inline.h and tbridge_inline.h to be compiled for
 * non-MCI/cBus platforms without any references to functions in
 * tbridge_mci_inline.h and tbridge_mtr_inline.h.  That's the
 * theory anyway.  The compiler does eliminate unreachable code
 * framed in conditionals or switch statements on compile-time
 * constants in toy files of limited complexity.  What I've done
 * here seems to tax its abilities, and so the introduction of
 * this define and the flurry of '#ifdef TBRIDGE_MCI_CBUS'
 * statements in tbridge_tlb_inline.h and tbridge_inline.h.
 */

#undef TBRIDGE_MCI_CBUS

/*
 * This define allows a subset of the TBRIDGE_MCI_CBUS code
 * switches above. In particular, the RSP needs the FDDI features.
 */
#define TBRIDGE_RSP_CBUS
 
#include "tbridge_isl_inline.h"
#include "tbridge_tlb_inline.h"
#include "tbridge_irb_inline.h"
#include "tbridge_rsp_inline.h"
#include "tbridge_inline.h"


/*
 * Copy a received packet from the input interface into a network
 * buffer. Note that the inline has become too large to be invoked in
 * multiple locations.
 */

boolean tbridge_rsp_copy_rx_pak (paktype *rsp_pak, paktype *pak_copy)

{
    return(tbridge_rsp_copy_rx_pak_inline(rsp_pak, pak_copy));
}


/*
 * Copy a packet from an RSP CyBus interface for process-level bridging,
 * and translate it to ether in preparation for slow-switching it.
 */

void tbridge_rsp_slowswitch (paktype *rsp_pak)

{
    paktype *pak_copy;

    pak_copy = input_getbuffer(rsp_pak->datagramsize, 
			       rsp_pak->if_input->hwptr);

    if (pak_copy) {
	if (tbridge_rsp_copy_rx_pak(rsp_pak, pak_copy)) {
	    pak_copy->if_output = rsp_pak->if_output;
	    pak_copy->bridgeptr = rsp_pak->bridgeptr;
	    tbridge_slowswitch_inline(pak_copy);
	} else {
	    datagram_done(pak_copy);
	}
    }
    datagram_done(rsp_pak);
}


/*
 * Take care of any RSP specific preparation required by
 * tbridge_flood_inline to perform platform-independent flooding.
 */

void tbridge_rsp_flood (paktype *rsp_pak)

{
    tbifd_type	*flood_list;
    ulong	flood_scope;
    idbtype     *inputsw;
    hwidbtype   *inputhw;

    /*
     * XXX - This copying is temporary for the first phase!
     *
     * At this point, we must copy the packet for flooding in the RSP.
     * There isn't any cBus flooding done on the RSP since there isn't
     * an SP, however in the future an RSP flooding software mechanism
     * will be installed.
     */
    if (!tbridge_rsp_copy_rx_pak(rsp_pak, tbridge_flood_pak)) {

	/* Account for received packets that were filtered. */
	/* This is a corner case that catches unknown media types. */

	tbridge_rx_accounting_inline(rsp_pak->if_input->hwptr,
				     tbridge_dlli.orig_datagramsize);
	datagram_done(rsp_pak);
	return;
    }

    /*
     * Flush the received packet since by this point we have copied it
     * to tbridge_flood_pak.
     */
    datagram_done(rsp_pak);

    inputsw = tbridge_flood_pak->if_input;
    inputhw = inputsw->hwptr;
    
    if (is_bvi(inputhw)) {
    	flood_list = spanarray[inputhw->unit]->floodQ.qhead;
    }
    else {
    	flood_list = inputsw->span_ptr->floodQ.qhead;
    }
    flood_scope = TBIFD_FLOOD;

    /*
     *
     */

    if (tbridge_dlli.dlli_attributes & DLLI_IPMCAST)
	flood_scope = reg_invoke_tbridge_cmf(tbridge_flood_pak, &flood_list);

    if (TBIFD_NO_FLOOD == flood_scope)
	return;

    tbridge_flood_inline(tbridge_flood_pak, flood_list, flood_scope,
			 FALSE, TBRPAK_RSP);
}


/*
 * Take care of any RSP specific preparation required by
 * tbridge_forward_inline to perform platform-independent bridging.
 *
 * Transparently bridge a packet from an RSP CyBus interface to another
 * RSP CyBus interface.
 *
 * The received packet is in the MEMD buffer written by the receiving
 * interface processor.
 *
 * The following fields of the rsp_pak should be set up by the caller
 * prior to the invokation of reg_invoke_tbridge_forward():
 *		if_input
 *		datagramsize
 *		datagramstart
 *		mac_start
 *		addr_start
 *		info_start
 *              rxtype
 *
 * Returns TRUE if the packet was forwarded, flooded, or filtered,
 * in which case the received packet will have been flushed.
 * Returns FALSE if the packet should be received by the bridge itself,
 * in which case the received packet will not have been flushed.
 */
boolean tbridge_rsp_forward (paktype *rsp_pak)
{
    enum TBRIDGE_IRB_RETVAL tbirb_status;
    enum TBRIDGE_FWD_RETVAL tbfwd_status;

    idbtype *inputsw = rsp_pak->if_input;
    hwidbtype *inputhw = inputsw->hwptr;
    uchar *original_info_start;

    ieee_addrs *addr_start  = (ieee_addrs *)rsp_pak->addr_start;
    rsp_pak->linktype = LINK_BRIDGE;	/* identify for fancy queueing oqueue */
    original_info_start = rsp_pak->info_start;

    /*
     * RSP transparent bridging is driven from the received packet as
     * it exists in MEMD. At this point the pak structure referenced is
     * the special one generated as the packet was taken off the raw Q.
     * This pak structure can only be used to manipulate the packet at
     * interrupt level.
     *
     * Note that inputsw is expected to be the first swidb and that
     * we are switching on a general attribute of the interface.
     * This is straight forward for interface bridging, but in the
     * case of sub-interface bridging remember that we have yet to
     * identify the true input software idb.  To be able to bridge
     * on sub-interfaces without also bridging on the interface,
     * media drivers must enable bridging on any interface for which
     * at least one sub-interface is configured for bridging.
     *
     * Note also that since SDE-encapsulated bridging is not supported
     * on the first swidb, we do not expect TBR_xxx_SDE as a value for
     * tbridge_media in this switch.
     */

    tbridge_dlli.orig_datagramsize = rsp_pak->datagramsize;

    switch (inputsw->tbridge_media) {

    case TBR_ETHER:
	tbridge_dlli_init(inputhw, inputsw, 0);
        break;
 
    case TBR_CTR:
        tbridge_dlli_init(inputhw, inputsw, TR_ACFC_SIZE);

	/*
	 * reset media_type to TBR_TR, since it's really
	 * more like the low end, and want to take the
	 * TBR_BFR_TO_BFR path...
	 */
	tbridge_dlli.src_media_type = TBR_TR;

        /*
	 * MAC addresses are non-canonical from the driver.
	 */
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
        break;
 
    case TBR_VBRIDGE:
        tbridge_dlli_init(inputhw, inputsw, TR_ACFC_SIZE);
	tbridge_dlli.src_media_type = TBR_TR;
        break;
 
    case TBR_FDDI:
        tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
        break;
 
    case TBR_FDDI_TRANSIT:
        {
            snap_hdr *link;
 
            tbridge_dlli_init(inputhw, inputsw, FDDI_FC);
            link = (snap_hdr *)rsp_pak->info_start;
 
            if ((link->dsap == SAP_SNAP) &&
                (link->type == TYPE_BRIDGE)) {
 
                /*
                 * Deencapsulate the Ethernet frame from the FDDI
                 * wrapper that surrounds it
                 */
                rsp_pak->rxtype = ether_decode_encapsulation_inline
                    (rsp_pak, (ether_hdr *)link->data);
 
#ifdef TBRIDGE_DEBUG
                addr_start = (ieee_addrs *)rsp_pak->addr_start;
#endif
                rsp_pak->flags |= PAK_TBRIDGE_IN;
            }
        }
        break;
 
    case TBR_LEX_TRANSIT: /* Fall through */
    case TBR_HDLC_TRANSIT:
        {
            uchar *frame;
 
            tbridge_dlli_init(inputhw, inputsw, HDLC_HEADER_SIZE);
            frame = rsp_pak->datagramstart + tbridge_dlli.transit_encapsize;
 
            /*
             * Deencapsulate the Ethernet frame from the serial wrapper
             * that surrounds it
             */
            rsp_pak->rxtype =
                ether_decode_encapsulation_inline(rsp_pak, (ether_hdr *)frame);
 
#ifdef TBRIDGE_DEBUG
            addr_start = (ieee_addrs *)rsp_pak->addr_start;
#endif
            rsp_pak->flags |= PAK_TBRIDGE_IN;
        }
        break;
 
    case TBR_ATM_TRANSIT:
        if (! tbridge_rsp_atm_media_classify(inputhw, inputsw, rsp_pak))
            return(FALSE);
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)rsp_pak->addr_start;
#endif
        break;

    case TBR_LANE_ETHER:
	if (! tbridge_rsp_lane_ether_classify(inputhw, inputsw, rsp_pak))
	    return FALSE;
	break;

    case TBR_FRAME_RELAY_TRANSIT:
	if (! tbridge_rsp_frame_relay_classify(inputhw, inputsw, rsp_pak))
	    return(FALSE);
#ifdef TBRIDGE_DEBUG
	addr_start = (ieee_addrs *)rsp_pak->addr_start;
#endif
	break;

    case TBR_SMDS_TRANSIT:
	if (! tbridge_rsp_smds_classify(inputhw, inputsw, rsp_pak))
	    return(FALSE);
	inputsw = rsp_pak->if_input;
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
         * here, For packet goes to process level on the high end
         * system we can still trigger a lot of meaningless errmsg 
         * just by attempting bridging it here. 
         * 
         * When FDDI calls the fastswitch first, it in turn calls the 
         * this routine.  Then the driver calls this routine again with
         * the if_input point to the swidb.  
         * 
         * If we have fixed the consisitence problem for the if_input,
         * this code may change.
         */
	return (FALSE);
	break;
    default:

        /*
         * inputsw->tbridge_media is not valid!
	 * Don't bridge these.
         */
	errmsg(TBRIDGE_ERR_INVALIDMEDIA, rxtype_string(rsp_pak->rxtype),
	       inputsw->namestring);
	return(FALSE);
	break;
    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_0) {
	buginf("\nTB0: %s on %s from %e to %e %d bytes",
	       rxtype_string(rsp_pak->rxtype), inputsw->namestring,
	       addr_start->saddr, addr_start->daddr,
	       rsp_pak->datagramsize);
    }
#endif	/* TBRIDGE_DEBUG */

    tbirb_status = tbridge_irb_inline(rsp_pak, inputsw);

    switch (tbirb_status) {

    case TBRIDGE_IRB_FORWARD:
	break;

    case TBRIDGE_IRB_RECEIVE:
        /*
         * Put the original info_start back. The SDE case will change its
         * value which cause driver point to the wrong field.
         */
        rsp_pak->info_start = original_info_start ;
	tbridge_rsp_encapsulation_recovery_inline(inputsw, rsp_pak);
	return(FALSE);
	break;

    case TBRIDGE_IRB_DISCARD:
	datagram_done(rsp_pak);
	return(TRUE);
	break;
    }

    tbfwd_status = tbridge_forward_inline(rsp_pak, TBRPAK_RSP);

    switch (tbfwd_status) {

    case TBRIDGE_FWD_DONE:
	return(TRUE);
	break;

    case TBRIDGE_FWD_FLOOD:
	tbridge_rsp_flood(rsp_pak);
	return(TRUE);
	break;

    case TBRIDGE_FWD_SLOWSWITCH:
	tbridge_rsp_slowswitch(rsp_pak);
	return(TRUE);
	break;

    case TBRIDGE_FWD_RECEIVE:
        /*
         * Put the original info_start back. The SDE case will change its
         * value which cause driver point to the wrong field.
         */
        rsp_pak->info_start = original_info_start ;
	tbridge_rsp_encapsulation_recovery_inline(inputsw, rsp_pak);
	return(FALSE);
	break;
    }

    /* Unreachable statement that the compiler requires. */
    return(FALSE);
}


/*
 * Worker function for extended bridging access lists. Read
 * up to 4 bytes from the specified offset in the RSP's uncached
 * MEMD space and return it to the caller. 
 *
 * N.B.: Function assumes that *size* <= 4.
 */

ulong tbridge_rsp_read_bytes (hwidbtype *hwidb,
			      uchar *base,
			      int offset,
			      ushort size)

{
    /*
     * Use uncached address space to access the MEMD. Minimize the number
     * accesses made to MEMD to reduce the stall time. We try to get the
     * data with a single read of one 4 byte word and do a second read
     * only if necessary.
     * 
     * Note: The RSP1 can do the odd sized accesses; ie. 3, 5, 7 bytes,
     *       but the RSP2 will not be able to for accesses to the peer
     *       RSP2's MEMD.
     */
    #define MEMD_WORD_MASK (0x3)
    ulong value;
    ulong MEMD_uncached;
    long delta;
    long shift;
    ulong mask;
    
    MEMD_uncached = (ulong) ADDR_MEMD_TO_UNCACHED(base + offset);
    delta = (MEMD_uncached & MEMD_WORD_MASK) + size - 4;
    shift = delta * 8;
    mask = 0xFFFFFFFF >> ((4 - size) * 8);

    value = *(ulong *)(MEMD_uncached &= ~MEMD_WORD_MASK);
    if (delta > 0) {
	/*
	 * Data crosses the word boundary, do a second read and
	 * combine it with the data from the first read.
	 */
	value = (value << shift) |
	    (*(ulong *)(MEMD_uncached + 4) >> (32 - shift));
    } else if (delta < 0) {
	/* Data is not aligned to the right side of the word so align it. */
	value >>= -shift;
    }
    value &= mask;
    return(value);
}


/*
 * In the beginning ...
 *
 * The world of RSP transparent/translational bridging begins with the
 * functions registered at the end of this function which attempt to forward
 * or flood a received packet.
 * 
 * The general outline of what follows is:
 *
 * - a driver interrupt routine (rsp_process_event) eventually invokes one
 *   of the forwarding functions defined here through a registry
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
 * indirectly accessible in an MCI/cBus receive window (as is the case on
 * high-end platforms).  This switch is used to select platform-specific
 * operations where required in the generic bridging code.  
 *
 * That's all there is to it.
 *
 * In this function we allocate the network buffers used throughout the
 * RSP tbridge code to forward and flood packets.  Forwarding is done
 * directly out of the MEMD buffer where possible. Special access outside of
 * the cached MAC header of a received packet may need to be made if extended
 * access lists are configured. The RSP's special fastswitch paktype
 * structure is used during the bridging. It cannot be used to preserve
 * state across function invokations.
 *
 * Flooding is done by reading the entire received packet into a network
 * buffer, flushing the received packet, and then translating the packet
 * for each outbound interface in the flood list. tbridge_flood_pak is
 * pre-allocated for this purpose.
 * 
 */

void tbridge_rsp_init (subsystype* subsys)

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
     * XXX - Allocate extra bytes so that when the RSP's special MEMD copy
     * routine is used to to do maximum-length copies to this buffer, there
     * will be enough room for any potential overwrite.
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
    reg_add_tbridge_forward(tbridge_rsp_forward, "tbridge_rsp_forward");
    reg_add_vbridge_forward(tbridge_rsp_forward, "tbridge_rsp_forward");
    reg_add_tbridge_read_bytes(tbridge_rsp_read_bytes,
                               "tbridge_rsp_read_bytes");
    reg_add_print_copyright(tbridge_print_message, "tbridge_print_message");

}

#define TBRRSP_MAJVERSION 1
#define TBRRSP_MINVERSION 0
#define TBRRSP_EDITVERSION 1

SUBSYS_HEADER(tbridge_rsp, TBRRSP_MAJVERSION, TBRRSP_MINVERSION,
	      TBRRSP_EDITVERSION,
              tbridge_rsp_init, SUBSYS_CLASS_PROTOCOL,
              "seq: tbridge",
              "req: fastswitch tbridge");
