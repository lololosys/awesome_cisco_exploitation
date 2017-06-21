/* $Id: bridge.c,v 3.12.4.17 1996/08/28 13:15:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/bridge.c,v $
 *------------------------------------------------------------------
 * bridge.c -- Device Independent Transparent Bridge Support
 *
 * 23-April-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridge.c,v $
 * Revision 3.12.4.17  1996/08/28  13:15:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.4.16  1996/08/26  20:32:13  mcieslak
 * CSCdi48388:  hybridge_input Hssi11/0: unexpected idb type - 0xC
 * Branch: California_branch
 * Packets received on transit links should have PAK_TBRIDGE_IN set if
 * they are to be passed to process level bridging.
 *
 * Revision 3.12.4.15  1996/08/14  02:36:31  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.12.4.14  1996/08/09  23:46:59  weismann
 * CSCdi65025:  Input- Output-pattern-list filters all process bridged tr
 * fddi paks. Make call for xmacaccesscheck using addr_start, not
 * datagramstart.
 * Branch: California_branch
 *
 * Revision 3.12.4.13  1996/08/06  17:36:09  rbadri
 * CSCdi65235:  LANE bridging: unused code in process level flooding path
 * Branch: California_branch
 * Pull out the code from the flooding routine.
 *
 * Revision 3.12.4.12  1996/08/03  23:37:11  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.12.4.11  1996/07/26  03:03:11  wilber
 * CSCdi63140:  Crash at _bridge_enq with no bridging configured
 * Add sanity check since bridging can get packets through the 'dump' path
 * Branch: California_branch
 *
 * Revision 3.12.4.10  1996/07/23  13:29:00  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.12.4.9  1996/07/02  23:11:29  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.12.4.8  1996/06/01  01:32:22  wilber
 * CSCdi59015:  %ALIGN-3-CORRECT, Alignment correction made at 0X6035256C
 * Use GETSHORT
 * Branch: California_branch
 *
 * Revision 3.12.4.7  1996/05/22  22:54:37  wilber
 * CSCdi56191:  buginf should be errmsg
 * - Convert the unsolicited message to errmsg
 * - Rewrite the unmeaning output string
 * Branch: California_branch
 *
 * Revision 3.12.4.6  1996/05/10  01:42:53  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.12.4.5  1996/05/09  14:46:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.4.4  1996/05/04  01:48:56  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.12.4.3  1996/04/25  23:22:33  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.12.4.2  1996/04/23  05:57:14  weismann
 * CSCdi50570:  Process bridging atm-dxi arp replies fails encapsulation
 * Branch: California_branch
 * Fix bridge table entry proxy dxi address.
 *
 * Revision 3.12.4.1  1996/03/18  22:10:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.3  1996/03/16  07:37:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.12.2  1996/03/13  02:01:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.12.1  1996/03/07  10:50:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.12  1996/03/07  23:32:25  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.11  1996/03/05  18:10:07  rbadri
 * CSCdi50469:  Transparent Bridging with ISL doesnt work
 * While writing the ISL header, restore datagramstart/mac_start to
 * the appropriate values to preserve the LLC header and data.
 *
 * Revision 3.10  1996/03/01  19:02:32  kmoberg
 * CSCdi44347:  SYS-2-SHAREDand tracebacks with DLSw ethernet. LLC2 over
 * tbridge causes a memory leak. Change retbuffer to datagram_done. Force
 * the linktype in DLSw. Don't try to retransmit a pak still in the box.
 *
 * Revision 3.9  1996/02/17  01:41:14  hou
 * CSCdi48204:  bridge takes 5 minutes to function properly with fddi
 *              encaps
 * - bridge_out()
 *   should swap bits of dst address if it isn't a bit-swapped proxy
 *   multicast address.
 *
 * Revision 3.8  1996/01/05  04:57:14  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.7  1995/12/30  00:29:03  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.6  1995/12/28  22:43:28  pitargue
 * CSCdi45879:  atm lane int configured as rfc 1483
 * always check how atm interfaces are configured.  don't assume past
 * states
 * of tbridge configurations.  also add some fields to show span and show
 * bridge
 * group to help debug this in the future.
 *
 * Revision 3.5  1995/12/17  06:10:29  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.4  1995/11/21  20:09:27  wilber
 * CSCdi42445:  Bridge table fails to receive packets for single entry
 * When station roams, should remove its fddi cam entry before overwriting
 * the input interface in bridge table; Also, recalculate permission bit
 *
 * Revision 3.3  1995/11/17  18:52:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:30:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:30:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/26  00:29:36  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert tbridge and srb to use chunk-based SMF.
 *
 * Revision 2.7  1995/08/24  21:40:47  ogrady
 * CSCdi38752:  Router crashes after entering Transparent bridging commands
 * Fix the CVS log entry.
 *
 * Revision 2.6  1995/08/24  20:58:36  ogrady
 * CSCdi38752:  Router crashes after entering Transparent bridging
 * commands.  Fix short word accesses to odd byte addresses.
 *
 * Revision 2.5  1995/08/08  16:49:04  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/07/06  00:25:07  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.3  1995/06/20  03:01:58  speakman
 * CSCdi36088:  undefined tbridge references making trunkd.test
 * Remove tbridge_smf_... functions from tbridge.c to bridge.c
 * in the interest of the lite images that build with only
 * process-level transparent bridging.
 *
 * Revision 2.2  1995/06/18  06:19:09  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 23:00:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "config.h"
#include "../if/fddi.h"
#include "span.h"
#include "access.h"
#include "../if/network.h"
#include "../if/ether_inline.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../x25/x25.h"
#include "address.h"
#include "../x25/x25_address.h"
#include "bridge.h"
#include "../srt/bridgeaccess.h"
#include "../srt/srt_registry.h"
#include "../wan/smds.h"
#include "../srt/srt_debug.h"
#include "sys_registry.h"
#include "../h/media_registry.h"

#include "../vlan/vlan.h"

#include "../h/logger.h"

#include "../smf/smf_public.h"

#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_debug.h"
#include "../tbridge/tbridge_sde.h"
#include "../tbridge/tbridge_sde_inline.h"
#include "../tbridge/tbridge_isl_inline.h"

#include "../if/atm.h"
#include "../atm/ilmi_api.h"

/* For Lan Emulation */
#include "../atm/atmsig_api.h"

#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../tbridge/tbridge_common_inline.h"
#include "../tbridge/msg_tbridge.c"



/*
 * Local Storage and Definitions
 */
boolean hybridge_started;
int bte_total;
boolean enable_80d5 = FALSE;
watched_queue *bridgeQ;
spantype *spanarray[SPAN_MAXINDEX];
boolean sap_8209_table[256]; 
boolean default_sap_8209_table[256];

/*
 * These are declared and initialized in if/network.c
 *
 * int bte_count;
 * queuetype bteQ;
 */
	    
/*
 * External references
 */
extern boolean lat_compare_groups(uchar *, uchar *);
extern void crashdump(int);

/*
 * bridge_enqueue
 * Enqueue a bridge PDU for process level input
 */

void bridge_enqueue (paktype *pak)
{
    process_enqueue_pak(bridgeQ, pak);
}

/*
 * hybridge_input
 * Handle bridge packets that slip through the fast paths.
 *
 * pak->datagramstart and pak->datagramsize are assumed to be valid. (duh)
 * if pak->flags & PAK_TBRIDGE_IN is set then the packet has been coerced to
 * look like an ethernet packet.  This is what will be set if the packet came
 * in across a transit bridge media.  Otherwise pak->input->status tells us
 * what the exact encapsulation is.
 */

process hybridge_input (void)
{
    spantype *span;
    tbifd_type *oif = NULL;
    paktype *pak, *new;
    idbtype *input;
    hwidbtype *inputhw;
    ulong status;
    ushort type_or_len;
    idbtype *idb;
    int pak_count, group;
    uchar *node;
    multi_media_bridge_mac_t bridge_mac;
    int llcsize;		/* Length of LLC data */
    uchar *llcstart;		/* Pointer to start of LLC data */
    boolean bte_created;
    boolean dont_swap_da;   

    bte_created = FALSE;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    process_watch_queue(bridgeQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_BRIDGE, bridge_enqueue, "bridge_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
 	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(bridgeQ);
	    if (pak == NULL) {
		break;
	    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
  	    if (tbridge_debug_pak_3) {
  		buginf("\nTB3: process bridging %s from %s to %e\n",
  		       rxtype_string(pak->rxtype),
  		       pak->if_input->namestring,
  		       ((ieee_addrs *)(pak->addr_start))->daddr);
  	    }
#endif	/* TBRIDGE_DEBUG */

	    dont_swap_da = FALSE;
	    input = pak->if_input;
	    inputhw = input->hwptr;

	    /*
	     * Special check for the BVI(Bridge-group Virtual Interface)
	     * since span_index is NULL for a BVI - BVI is a routed
	     * interface, no bridging attributes are on the BVI
	     */
	    if (is_bvi(inputhw)) {
		span = spanarray[inputhw->unit];
	    } else {
		span = spanarray[input->span_index];
	    }

            if (!span) {
		datagram_done(pak);
		continue;
	    }

            /*
             * Don't account for received packets that have valid
	     * output interface since they come from forwarding or
	     * flooding path of fast-switching and we should account 
	     * for it already.
	     * Make an exception for bridged packets coming in from
	     * SMDS since they are not counted in the fast path.
             */

            if ((!pak->if_output) || is_smds(inputhw)) {
		idb_increment_rx_proto_counts(pak, ACCT_PROTO_BRIDGE);
                pak->acct_proto = ACCT_PROTO_BRIDGE;
            }
	    
	    /*
	     * Create token ring and ethernet format MAC and LLC headers
	     * from the input packet. If the packet came from token ring,
	     * we construct an ethernet style MAC header by bitswapping
	     * the source and destination address, and then checking
	     * to see if the packet is a LLC1 packet with the magic OUI
	     * for encapsulated datagrams. If it isn't the magic OUI, we
	     * just pass the LLC data unmodified and construct an ethernet
	     * length field; if it is the magic OUI, we extract the protocol
	     * type and strip the LLC header.
	     *
	     * If the packet came from a transit bridging media it is already
	     * in ethernet format.  We just use that.  This is denoted by
	     * pak->input->status & IDB_ETHER or pak->flags & PAK_TBRIDGE_IN.
	     * pak->datagramstart points at the DA of the ether packet.
	     *
	     * The BVI(Bridge-group Virtual Interface), which is for
	     * IRB(Integrated Routing and Bridging), is a virtual-ether 
	     * too. All packets coming from the BVI are in ethernet format.
	     *
	     * If the packet came in on a non-transit FDDIT the DA and SA
	     * are already canonicalized.  We just use them.  We also
	     * have to adjust for the FC byte and for it being in 802 format.
	     */

	    /*
	     * Note that the do_snap_snap indicator on bridge_mac which
	     * is passed to bridge_out is used in that function to determine
	     * whether an ARPA encapsulated packet needs to be SNAP
	     * encapsulated on output to FDDI and TR.  It has no application
	     * on output to ETHER. --- Tony S
	     */

	    status = inputhw->status;
	    if ((status & (IDB_ETHER|IDB_TUNNEL)) ||
		(pak->flags & PAK_TBRIDGE_IN) || is_bvi(inputhw)) {

                ether_hdr *ether;

		/*
		 * Source of packet is an ether like thingy.
		 * Find pointers to ethernet style MAC header and convert
		 * it to token ring format.
		 */

		llcsize = pak->datagramsize - ETHER_ARPA_ENCAPBYTES;
		llcstart = pak->datagramstart + ETHER_ARPA_ENCAPBYTES;
		ether = (ether_hdr *)pak->datagramstart;
		
		ieee_copy(ether->daddr, bridge_mac.canonical_mac.daddr);
		ieee_copy(ether->saddr, bridge_mac.canonical_mac.saddr);
		type_or_len = GETSHORT(&ether->type_or_len);
		bridge_mac.canonical_mac.type_or_len = type_or_len;

		if (span->fddi_port_count || span->tr_port_count) {
		    /*
		     * Token Rings in the bridge group.  We need the
		     * bit swapped version.
		     */

		    /*
		     * If the input interface is a BVI(which is 
		     * virtual-ether) and is IPX packt, that means this 
		     * packet is from a IPX routed interface, where the DA
		     * of MAC header is extract from the destination MAC 
		     * inside* the IPX packet. We don't want to swap the 
		     * DA here because it is in token format. 
		     * If we swap it, it will be in canonical format on 
		     * the token, and the host won't pick it up
		     *
		     * If it is an atalk packet with Appletalk multicast
		     * DA, do the multicast translation for it since just
		     * bit-swap is NOT equal to the multicast address on
		     * token(functional address)
		     *
		     * We can't check on arpa_type for IPX packets 
		     * because the Novell-ether packet doesn't have a 
		     * ARPA type; 
		     * We can't check on rxtype for Atalk packets because
		     * the AARP packet doesn't have its own rxtype.
		     */
		    if (is_bvi(inputhw)) {
			ushort arpa_type;

			pak->rxtype =
			    ether_decode_encapsulation_inline(pak,(ether_hdr *)
							  pak->datagramstart);

			arpa_type = tbridge_get_arpa_type_inline(pak);

			if ((pak->rxtype == RXTYPE_NOVELL1) ||
			    (pak->rxtype == RXTYPE_SNAP_NOV1) ||
			    (pak->rxtype == RXTYPE_NOVELL2) ||
			    (pak->rxtype == RXTYPE_SAP_NOVELL)) {
			    dont_swap_da = TRUE;
			} else if ((arpa_type == TYPE_ETHERTALK) || 
				   (arpa_type == TYPE_AARP)) {

			    uchar apple_ether_broadcast[IEEEBYTES] = 
				{0x09, 0x00, 0x07, 0xff, 0xff, 0xff};
			    uchar apple_tr_broadcast[IEEEBYTES] =
				{0xC0, 0x00, 0x40, 0x00, 0x00, 0x00};
			    
			    if (ieee_equal(ether->daddr, 
					   apple_ether_broadcast)) {
				ieee_copy(apple_tr_broadcast, 
					  ether->daddr);
				/* In token format already */
				dont_swap_da = TRUE;
			    }
			}
		    }

		    if (dont_swap_da)
			ieee_copy(ether->daddr, bridge_mac.bs_da);
		    else
			ieee_swap(ether->daddr, bridge_mac.bs_da);

		    ieee_swap(ether->saddr, bridge_mac.bs_sa);
		}
		
		/* Look at the LLC and see if we need to create one for an
		 * ethernet format frame.  The weird hack below is to preserve
		 * the type code of 0 which shows up from some broken DEC
		 * equipment.
		 */
		if (type_or_len > MAXETHERSIZE ||
		    (type_or_len == 0 && llcsize < MAXETHERSIZE)) {
		    bridge_mac.do_snap_snap = TRUE;
		} else {
		    bridge_mac.do_snap_snap = FALSE;
		    if (llcsize > type_or_len &&
			type_or_len)	/* Length - go use it */
			llcsize = type_or_len;
		}
	    } else if (status & IDB_TR) {

		tring_hdr *trh;
		snap_hdr *snap;

		/*
		 * The packet came from a token ring. Copy
		 * the token ring packet header into the save area.
		 */
		trh = (tring_hdr *)pak->datagramstart;

		ieee_copy(trh->daddr, bridge_mac.bs_da);
		ieee_copy(trh->saddr, bridge_mac.bs_sa);

		ieee_swap(trh->daddr, bridge_mac.canonical_mac.daddr);
		ieee_swap(trh->saddr, bridge_mac.canonical_mac.saddr);

		/* Remove SNAP header from token ring SNAP frames if
		 * the frame's OUI equals to the "ethernet-transit-oui"
		 * configured on the token ring interface. 
		 * If the output interface is ethernet, the frame will become 
		 * an ARPA frame. If the output interface is token ring or 
		 * FDDI, the SNAP header will be added back later.  
		 */

                snap = (snap_hdr *)trh->data;

		if ((GETSHORT(&snap->dsap) == SNAPSNAP) &&
		    ((GETLONG(&snap->control)) == inputhw->tr_transit_oui)) {

		    /* Packet contains transit OUI - strip it and move type
		     * code into where it belongs.
		     */
		    llcsize = pak->datagramsize -
				      (TRING_ENCAPBYTES + SNAP_HDRBYTES);
		    llcstart = pak->datagramstart +
				      (TRING_ENCAPBYTES + SNAP_HDRBYTES);
		    bridge_mac.canonical_mac.type_or_len =
				      GETSHORT(&snap->type);

		    bridge_mac.do_snap_snap = TRUE;
		    
		} else {
		    /*
		     * Packet is some other kind of LLC frame; set up a
		     * length code.
		     */
		    llcsize = pak->datagramsize - TRING_ENCAPBYTES;
		    llcstart = pak->datagramstart + TRING_ENCAPBYTES;
		    bridge_mac.canonical_mac.type_or_len = 
					(llcsize > MAXETHERSIZE) ? 0 : llcsize;
		    bridge_mac.do_snap_snap = FALSE;
		}
	    } else {

		fddi_hdr *fddi;
		snap_hdr *snap;


		fddi = (fddi_hdr *)pak->datagramstart;
		switch (inputhw->type) {

		  case IDBTYPE_FDDIT:
		    ieee_copy(fddi->daddr, bridge_mac.canonical_mac.daddr);
		    ieee_copy(fddi->saddr, bridge_mac.canonical_mac.saddr);

		    if (span->tr_port_count) {
			ieee_swap(fddi->daddr, bridge_mac.bs_da);
			ieee_swap(fddi->saddr, bridge_mac.bs_sa);
		    }
		    break;

		  case IDBTYPE_BSI:
		    ieee_swap(fddi->daddr, bridge_mac.canonical_mac.daddr);
		    ieee_swap(fddi->saddr, bridge_mac.canonical_mac.saddr);

		    if (span->tr_port_count || (span->fddi_port_count > 1)) {

			ieee_copy(fddi->daddr, bridge_mac.bs_da);
			ieee_copy(fddi->saddr, bridge_mac.bs_sa);
		    }
		    break;

		  default:
		    errmsg(TBRIDGE_ERR_BAD_ENCAP, encstring(inputhw->enctype),
			   input->namestring);
		    datagram_done(pak);
		    continue;
		}

		snap = (snap_hdr *)fddi->data;

                if (GETSHORT(&snap->dsap) == SNAPSNAP) {
                  /*
                   * All the computation below assumes that the packet
                   * at least has complete SNAP header. Drop it if it
                   * doesn't.
                   */
                  if (pak->datagramsize < FDDI_SNAP_ENCAPBYTES) {
                    datagram_done(pak);
                    continue;
                  }
                }

		if (GETSHORT(&snap->dsap) == SNAPSNAP &&
		    GETLONG(&snap->control) == OLD_ETHEROUI) {
		
		    llcsize = pak->datagramsize - FDDI_SNAP_ENCAPBYTES;
		    llcstart = pak->datagramstart + FDDI_SNAP_ENCAPBYTES;
		    bridge_mac.canonical_mac.type_or_len = 
			    GETSHORT(&snap->type);
		    bridge_mac.do_snap_snap = TRUE;

		} else {
		    /*
		     * Not majic.  Use what is there.
		     */
		    llcsize = pak->datagramsize - FDDI_ENCAPBYTES;
		    llcstart = pak->datagramstart + FDDI_ENCAPBYTES;
		    bridge_mac.canonical_mac.type_or_len = 
					(llcsize > MAXETHERSIZE) ? 0 : llcsize;
		    bridge_mac.do_snap_snap = FALSE;
		}
	    }

	    /*
	     * If output interface is already known, all checks have been
	     * done.  This is the priority output/X.25 bridging case.
	     */
	    if (pak->if_output) {
		/*
		 * If it came from the virtual interface, then output checks
		 * need to be applied. Do this by very ugly jumping into
		 * the flood loop!
		 */

/***********************************************
 Unbelieveable dog shit commented out ... Tony S

		if (pak->if_input && (pak->if_input->hwptr->status & IDB_VIRTUAL)) {
		    idb = pak->if_output;
		    count = 0;
		    goto virtual_jump;
		}
 ***********************************************/

		bridge_out(pak->if_output, pak, &bridge_mac, llcstart, llcsize);
		continue;
	    }

	    /*
	     * Loop over all interfaces in this spanning tree.
	     * Do not send out the circuit upon which we received.
	     * Do not call pak_duplicate for last output interface.
	     */

	    for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {
		idb = oif->tbifd_swidb;
		if (idb->circ_group) {
		    group = idb->circ_group;
		    idb = dld_get_output_idb(span, group,
					     &pak->datagramstart[6],
					     &pak->datagramstart[0]);
		    if (!idb) {
			if (TIMER_RUNNING(span->cct_groups[group].ccg_pause_timer) &&
			    AWAKE(span->cct_groups[group].ccg_pause_timer))
			    ccg_pause_timer_expiry(span, group);
			continue;
		    }
		}
		if ((idb->if_number == input->if_number) || 
		    (input->circ_group &&
		     (idb->circ_group == input->circ_group)))
		    continue;

/***********************************************
 Unbelieveable dog shit commented out ... Tony S

	      virtual_jump:
 ***********************************************/

		/*
		 * If you jump here, have idb = pak->if_output, and count = 0
		 */
		if (idb->circ_canforward == FALSE)
		    continue;	
		if (!interface_up_simple(idb))
		    continue;

		/*
		 * Do output typecode/LSAP and address access checking
		 */
		if (idb->span_flag_out) {
		    if (!bridge_floodcheck(idb, pak, llcsize))
			continue;
		}
   		if (idb->span_address_out) {
		    if (!access_check_mac(idb->span_address_out,
					  &bridge_mac.canonical_mac.daddr[0]))
			continue;
		}
		if (idb->span_pattern_out) {
		    if (!xmacaccesscheck(idb->hwptr, idb->span_pattern_out,
					pak->addr_start, FALSE))
			continue;
		}

		/*
		 * Do output LAT service announcement filtering
		 */

		if (pak->lat_groupmask) {

		    /*
		     * Don't output this message if the message is
		     * in the deny list.
		     */

		    if (idb->span_lat_out_svc_deny[0] &&
			lat_compare_groups(pak->lat_groupmask,
					   idb->span_lat_out_svc_deny)) {
			if (bridge_lat_debug)
			  bridge_debug_lat(idb, pak, "output deny");
			continue;
		    }
		    /*
		     * Don't output this message unless it is in the
		     * permit list.
		     */

		    if (idb->span_lat_out_svc_permit[0] &&
			!lat_compare_groups(pak->lat_groupmask,
					    idb->span_lat_out_svc_permit)) {
			if (bridge_lat_debug)
			  bridge_debug_lat(idb, pak, "output permit");
			continue;
		    }

		    /*
		     * By now, we are actually going to bridge the service
		     * announcement. If they have LAT packet debugging
		     * enabled, log the fact.
		     */

		    if (bridge_latpkt_debug) {
			node = pak->lat_groupmask + *pak->lat_groupmask + 1;
			buginf("\nTBRIDGE: LAT announcement from %*s (%e) "
			       "bridged from %s to %s",
			       node[0], &node[1],
			       pak->datagramstart + IEEEBYTES,
			       pak->if_input->namestring, idb->namestring);
		    }
		}
		
		/*
		 * If we are flooding to the last interface in the list,
		 * or if we are flooding to the second last interface
		 * in the list and the last interface in the list is the
		 * input interface, we can just flood the packet in hand,
		 * avoiding the overhead of pak_duplicate.
		 */
		if ((NULL == oif->tbifd_next) ||
		    ((NULL == oif->tbifd_next->tbifd_next) &&
		     oif->tbifd_next->tbifd_swidb == pak->if_input)) {

		    bridge_out(idb, pak, &bridge_mac, llcstart, llcsize);
		    pak = NULL;
		    break;

		} else {

		    new = pak_duplicate(pak);
		    if (new)
			bridge_out(idb, new, &bridge_mac,
				   new->datagramstart +
				   (llcstart - pak->datagramstart), llcsize);
		}
	    }
	    if (pak)
		datagram_done(pak);
	}
    }
}

/*
 * bridge_out
 * Send a datagram on a bridge port, adjusting encapsulation as necessary.
 */

void bridge_out (
    idbtype *output,
    paktype *pak,
    multi_media_bridge_mac_t *bridge_mac,
    uchar *llcstart,
    int llcsize)
{
    ushort *sp, *dp;
    x25map *map;
    addrtype *proto_addr;
    uchar fddi_proxy_addr[6];
    boolean use_proxy = FALSE;
    hwidbtype *outputhw = output->hwptr;

    pak->if_output = output;
    pak->linktype = LINK_BRIDGE;
    if (llcsize > llc_mtu(outputhw)) {
	output->hwptr->span_out_bigdrop++;
	datagram_done(pak);
	return;
    }

    if (outputhw->status & (IDB_ETHER|IDB_TUNNEL)) {

	if (TBR_ETHER_SDE == output->tbridge_media) {

	    /* Need to translate ARPA to SDE SNAP */
	    if (bridge_mac->do_snap_snap) {
		pak->datagramsize = ETHER_SNAP_ENCAPBYTES + SDE_ENCAPBYTES + 
		    llcsize;
		pak->datagramstart = llcstart - (ETHER_SNAP_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    } else {
		pak->datagramsize = ETHER_8023_ENCAPBYTES + SDE_ENCAPBYTES + 
		    llcsize;
		pak->datagramstart = llcstart - (ETHER_8023_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    }

	    if (pak->datagramsize < MINETHERSIZE)
		pak->datagramsize = MINETHERSIZE;

	    sp = (ushort *) pak->datagramstart;
	    dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	    PUTSHORT(sp++, *dp++);	       	/* destination */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);          	/* source */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);

	    /* length */
	    PUTSHORT(sp++, pak->datagramsize - ETHER_8023_ENCAPBYTES);

	    tbridge_bfr_sde_header_wr_inline(output, pak->datagramstart +
					     ETHER_8023_ENCAPBYTES,
					     bridge_mac->canonical_mac.saddr);

	    if (bridge_mac->do_snap_snap) {
		sp += SDE_ENCAPWORDS;
                PUTSHORT(sp++, SNAPSNAP);
                PUTSHORT(sp++, (SNAP_CTL_OUI >> 16));
                PUTSHORT(sp++, (SNAP_CTL_OUI & 0xffff));
		/* type */
                PUTSHORT(sp++, bridge_mac->canonical_mac.type_or_len);
	    }

	/*
	 * Are we egressing out an ISL vLAN interface? 
	 * Note this encapsulation is only defined on Fast Ethernet.
	 */

	} else if (output->tbridge_media == TBR_ETHER_ISL_VLAN) {

            pak->datagramsize = ETHER_ARPA_ENCAPBYTES + llcsize;

            if (pak->datagramsize < MINETHERSIZE)
                pak->datagramsize = MINETHERSIZE;

            pak->datagramstart = (llcstart - ETHER_8023_ENCAPBYTES);

	    tbridge_prepend_isl_header(output, pak);

	    /* Now write the Interior Packet's MAC header */

	    pak->datagramstart += ISL_HEADER_LENGTH;

            sp = (ushort *) pak->datagramstart;

            dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];

            PUTSHORT(sp++, *dp++);                    /* destination address */
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);                    /* source address */
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);                    /* length/type */
            
	    /* restore the datagramstart to the original place */
	    pak->datagramstart -= ISL_HEADER_LENGTH;

	} else {
	    pak->datagramsize = ETHER_ARPA_ENCAPBYTES + llcsize;
	    if (pak->datagramsize < MINETHERSIZE)
		pak->datagramsize = MINETHERSIZE;
	    pak->datagramstart = llcstart - ETHER_ARPA_ENCAPBYTES;
	    
	    sp = (ushort *) pak->datagramstart;
	    dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	    PUTSHORT(sp++, *dp++);			/* destination */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);			/* source */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);			/* len/type */
	}

    } else if (outputhw->status & IDB_SERIAL) {

	if (TBR_HDLC_TRANSIT_SDE == output->tbridge_media) {
	    /* Need to translate ARPA to SDE SNAP */
	    if (bridge_mac->do_snap_snap) {
		pak->datagramsize = ETHER_SNAP_ENCAPBYTES + SDE_ENCAPBYTES + 
		    llcsize;
		pak->datagramstart = llcstart - (ETHER_SNAP_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    } else {
		pak->datagramsize = ETHER_8023_ENCAPBYTES + SDE_ENCAPBYTES + 
		    llcsize;
		pak->datagramstart = llcstart - (ETHER_8023_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    }

	    sp = (ushort *) pak->datagramstart;
	    dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	    PUTSHORT(sp++, *dp++);			/* destination */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);			/* source */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
            /* length */
            PUTSHORT(sp++, pak->datagramsize - ETHER_8023_ENCAPBYTES);

            tbridge_bfr_sde_header_wr_inline(output, pak->datagramstart +
                                             ETHER_8023_ENCAPBYTES,
					     bridge_mac->canonical_mac.saddr);

            if (bridge_mac->do_snap_snap) {
                sp += SDE_ENCAPWORDS;
                PUTSHORT(sp++, SNAPSNAP);
                PUTSHORT(sp++, (SNAP_CTL_OUI >> 16));
                PUTSHORT(sp++, (SNAP_CTL_OUI & 0xffff));
                /* type */
                PUTSHORT(sp++, bridge_mac->canonical_mac.type_or_len);
            }

	    /*
	     * SDE encap is supported on HDLC only, so we don't have to
	     * check is_x25(outputhw), is_frame_relay(outputhw),
	     * is_smds(outputhw) and is_ppp(outputhw).
	     * outputhw->enctype should be equal to ET_HDLC.
	     * Call vencap routine to HDLC-encap this packet.
	     */
	    if (!(*outputhw->vencap)(pak, (long)pak->bridgeptr)) {
		datagram_done(pak);
		return;
	    }
	} else {
	    pak->datagramsize = llcsize + ETHER_ARPA_ENCAPBYTES;
	    pak->datagramstart = llcstart - ETHER_ARPA_ENCAPBYTES;
	    
	    sp = (ushort *) pak->datagramstart;
	    dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	    PUTSHORT(sp++, *dp++);			/* destination */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);			/* source */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);			/* len/type */
	    
	    if (is_x25(outputhw)) { 
		map = pak->bridgeptr;
		proto_addr = map ?
		    &map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr : NULL;
		if (!(*outputhw->vencap)(pak, (long)proto_addr)) {
		    datagram_done(pak);
		    return;
		}
	    } else if (is_frame_relay(outputhw) || is_smds(outputhw) || 
		       is_mlapb_only(outputhw) || 
		       is_ppp(outputhw) || (outputhw->enctype == ET_HDLC) ||
                       is_atm_dxi(outputhw) || is_cpp(outputhw)) {
		if (!(*outputhw->vencap)(pak, (long)pak->bridgeptr)) {
		    datagram_done(pak);
		    return;
		}
	    }
	}
    } else if (is_atm(outputhw)) {
	pak->datagramsize = llcsize + ETHER_ARPA_ENCAPBYTES;
	pak->datagramstart = llcstart - ETHER_ARPA_ENCAPBYTES;

	/*
	 * If this packet is from a BVI, which is a virtual ether, pad
	 * the packet to prevent any runt since some switches at the other
	 * end may assume that the interior ethernet packet is at least 
	 * MINETHERSIZE bytes.
	 */
	if ((pak->datagramsize < MINETHERSIZE) && idb_is_bvi(pak->if_input)) {
	    pak->datagramsize = MINETHERSIZE;
	}

        /*
	 * Pad the LANE packets to prevent runt */
        if (is_atm_lane(pak->if_output) && (pak->datagramsize < MINETHERSIZE)) {
	    pak->datagramsize = MINETHERSIZE;
        }

	sp = (ushort *) pak->datagramstart;
	dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	PUTSHORT(sp++, *dp++);			/* destination */
	PUTSHORT(sp++, *dp++);
	PUTSHORT(sp++, *dp++);
	
	PUTSHORT(sp++, *dp++);			/* source */
	PUTSHORT(sp++, *dp++);
	PUTSHORT(sp++, *dp++);

	PUTSHORT(sp++, *dp++);			/* len/type */
	
	if (!(*outputhw->vencap)(pak, (long)pak->bridgeptr)) {
		datagram_done(pak);
		return;
	}
    
    } else if (outputhw->status & IDB_LEX) {
	idbtype *if_input;
	idbtype *swidb;
	idbtype *serial_swidb;
	hwidbtype *serial_hwidb;
	
	/*
	 * PPP encaps has to be put on as well afterwards.
	 */
	swidb = pak->if_output;
	serial_swidb = swidb->lex_swidb;

	/*
	 * Is a serial interface bound to this lex
	 * interface.
	 */
	if (serial_swidb == NULL){ 
	    datagram_done(pak);
	    return;
	}

	pak->datagramsize = ETHER_ARPA_ENCAPBYTES + llcsize;
	if (pak->datagramsize < MINETHERSIZE)
	    pak->datagramsize = MINETHERSIZE;
	pak->datagramstart = llcstart - ETHER_ARPA_ENCAPBYTES;

	sp = (ushort *) pak->datagramstart;
	dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	PUTSHORT(sp++, *dp++);			/* destination */
	PUTSHORT(sp++, *dp++);
	PUTSHORT(sp++, *dp++);
	
	PUTSHORT(sp++, *dp++);			/* source */
	PUTSHORT(sp++, *dp++);
	PUTSHORT(sp++, *dp++);

	PUTSHORT(sp++, *dp++);			/* len/type */


	/*
	 * Save real if_input.  This must be set to NULL so that
	 * the cache line is not added.
	 */
	if_input = pak->if_input;
	pak->if_input = NULL;

	serial_hwidb = serial_swidb->hwptr;
	pak->linktype = LINK_LEX;
	pak->if_output = serial_swidb;
	
	/*
	 * Call serial hwidb's encaps.
	 */
	if (!(*serial_hwidb->vencap)(pak, (long)pak->bridgeptr)) {  
		datagram_done(pak);
		return;
	    }
	/*
	 * Restore things back.
	 */
	pak->linktype = LINK_BRIDGE;
	pak->if_input = if_input;
	pak->if_output = swidb;
	
    } else if (outputhw->status & IDB_FDDI) {
	if (outputhw->bridge_transit) {
	    /*
	     * FDDI running in Encapsulation mode.  Write an encapsulation header.
	     */
	    pak->datagramsize = ETHER_ARPA_ENCAPBYTES + llcsize + FDDI_SNAP_ENCAPBYTES;
	    pak->datagramstart = llcstart - ETHER_ARPA_ENCAPBYTES- FDDI_SNAP_ENCAPBYTES;
	    sp = (ushort *)((uchar *) pak->datagramstart - 1);
	    *sp++ = FDDI_LLC_FC_BYTE;
            /*
             * Check if a destination proxy address was passed with
             * this packet. If true, use it instead of the multicast
             * transit address.
             */
            if(pak->bridgeptr) {
              use_proxy = TRUE;
              PUTSHORT(&fddi_proxy_addr[0], *(((ushort *)pak->bridgeptr)++));
              PUTSHORT(&fddi_proxy_addr[2], *(((ushort *)pak->bridgeptr)++));
              PUTSHORT(&fddi_proxy_addr[4], *(((ushort *)pak->bridgeptr)++));
            }
	    switch (outputhw->type) {
	      case IDBTYPE_BSI:		
	      case IDBTYPE_FDDI:
              if(use_proxy) {
		  if (!ieee_equal(fddi_proxy_addr,
				  bit_swapped_transit_multicast)) { 
		      ieee_swap(fddi_proxy_addr, fddi_proxy_addr);
		  }

                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[0]));
                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[2]));
                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[4]));
              } else {
		PUTSHORT(sp++, GETSHORT(&bit_swapped_transit_multicast[0]));
		PUTSHORT(sp++, GETSHORT(&bit_swapped_transit_multicast[2]));
		PUTSHORT(sp++, GETSHORT(&bit_swapped_transit_multicast[4]));
	      }
		PUTSHORT(sp++, GETSHORT(&outputhw->bit_swapped_hardware[0]));
		PUTSHORT(sp++, GETSHORT(&outputhw->bit_swapped_hardware[2]));
		PUTSHORT(sp++, GETSHORT(&outputhw->bit_swapped_hardware[4]));
		break;
	      case IDBTYPE_FDDIT:
              if(use_proxy) {
                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[0]));
                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[2]));
                PUTSHORT(sp++, GETSHORT(&fddi_proxy_addr[4]));
              } else {
		/*
		 * fddit.  use canonical addresses.
		 */
		PUTSHORT(sp++, GETSHORT(&transit_bridge_multicast[0]));
		PUTSHORT(sp++, GETSHORT(&transit_bridge_multicast[2]));
		PUTSHORT(sp++, GETSHORT(&transit_bridge_multicast[4]));
	      }
		PUTSHORT(sp++, GETSHORT(&outputhw->hardware[0]));
		PUTSHORT(sp++, GETSHORT(&outputhw->hardware[2]));
		PUTSHORT(sp++, GETSHORT(&outputhw->hardware[4]));
		break;
	      default:
		crashdump(0);
	    }
	    PUTSHORT(sp++, 0xAAAA);
	    PUTSHORT(sp++, 0x0300);
	    PUTSHORT(sp++, 0x000C);
	    PUTSHORT(sp++, 0x6558);
	    dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
	    PUTSHORT(sp++, *dp++);		/* destination */
	    PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);
 
	    PUTSHORT(sp++, *dp++);              /* source */
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);

	    PUTSHORT(sp++, *dp++);              /* type/len */
	} else {
	    switch (outputhw->type) {
	      case IDBTYPE_FDDIT:
		/*
		 * FDDI transparent.  FDDI-T uses canonical others may use native (bs).
		 */
		dp = (ushort *) &bridge_mac->canonical_mac.daddr[0];
		break;
	      case IDBTYPE_BSI:
		dp = (ushort *) &bridge_mac->bs_da[0];
		break;
	      default:
		crashdump(0);
		dp = 0;
	    }
	    if (bridge_mac->do_snap_snap) {
		if (TBR_FDDI_SDE == output->tbridge_media) {
		    pak->datagramsize =  FDDI_SNAP_ENCAPBYTES + 
			SDE_ENCAPBYTES + llcsize;
		    pak->datagramstart = llcstart - (FDDI_SNAP_ENCAPBYTES +
						     SDE_ENCAPBYTES);
		} else {
		    pak->datagramsize =  FDDI_SNAP_ENCAPBYTES + llcsize;
		    pak->datagramstart = llcstart - FDDI_SNAP_ENCAPBYTES;
		}

		sp = (ushort *)((uchar *) pak->datagramstart - 1);
		PUTSHORT(sp++, FDDI_LLC_FC_BYTE);
		PUTSHORT(sp++, *dp++);		/* destination */
		PUTSHORT(sp++, *dp++);
		PUTSHORT(sp++, *dp++);
		
		PUTSHORT(sp++, *dp++);		/* source */
		PUTSHORT(sp++, *dp++);
		PUTSHORT(sp++, *dp++);
		
		if (TBR_FDDI_SDE == output->tbridge_media) {
		    tbridge_bfr_sde_header_wr_inline(output, 
						     pak->datagramstart +
						     FDDI_ENCAPBYTES,
						     bridge_mac->canonical_mac.saddr);
		    sp += SDE_ENCAPWORDS;
		}

		PUTSHORT(sp++, SNAPSNAP);
		PUTSHORT(sp++, (OLD_ETHEROUI >> 16));
		PUTSHORT(sp++, (OLD_ETHEROUI & 0xffff));
		PUTSHORT(sp++, bridge_mac->canonical_mac.type_or_len);
	    } else {
		if (TBR_FDDI_SDE == output->tbridge_media) {
		    pak->datagramsize = FDDI_ENCAPBYTES + SDE_ENCAPBYTES + 
			llcsize;
		    pak->datagramstart = llcstart - (FDDI_ENCAPBYTES +
						     SDE_ENCAPBYTES);
		} else {
		    pak->datagramsize = FDDI_ENCAPBYTES + llcsize;
		    pak->datagramstart = llcstart - FDDI_ENCAPBYTES;
		}

		sp = (ushort *)((uchar *) pak->datagramstart - 1);
		PUTSHORT(sp++, FDDI_LLC_FC_BYTE);
		PUTSHORT(sp++, *dp++);		/* destination */
		PUTSHORT(sp++, *dp++);
		PUTSHORT(sp++, *dp++);
		
		PUTSHORT(sp++, *dp++);		/* source */
		PUTSHORT(sp++, *dp++);
		PUTSHORT(sp++, *dp++);
		if (TBR_FDDI_SDE == output->tbridge_media)
		    tbridge_bfr_sde_header_wr_inline(output, 
						     pak->datagramstart +
						     FDDI_ENCAPBYTES,
						     bridge_mac->canonical_mac.saddr);
	    }
	}
    } else if (outputhw->status & IDB_TR) {
	if (bridge_mac->do_snap_snap) {
	    if ((TBR_CTR_SDE == output->tbridge_media)
                || (TBR_TR_SDE == output->tbridge_media)) {
		pak->datagramsize = TRING_SNAP_ENCAPBYTES + 
		    SDE_ENCAPBYTES + llcsize;
		pak->datagramstart = llcstart - (TRING_SNAP_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    } else {
		pak->datagramsize = TRING_SNAP_ENCAPBYTES + llcsize;
		pak->datagramstart = llcstart - TRING_SNAP_ENCAPBYTES;
	    }

	    sp = (ushort *)((uchar *) pak->datagramstart);
	    *sp++ = ((AC_PRI0 << 8) | FC_TYPE_LLC);
	    dp = (ushort *) &bridge_mac->bs_da[0];
	    PUTSHORT(sp++, *dp++);	       /* destination */
	    PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);
 
	    PUTSHORT(sp++, *dp++);             /* source */
            PUTSHORT(sp++, *dp++);
            PUTSHORT(sp++, *dp++);

	    if ((TBR_CTR_SDE == output->tbridge_media)
                || (TBR_TR_SDE == output->tbridge_media)) {
		tbridge_bfr_sde_header_wr_inline(output, pak->datagramstart +
						 TRING_ENCAPBYTES,
						 bridge_mac->canonical_mac.saddr);
		sp += SDE_ENCAPWORDS;
	    }

	    PUTSHORT(sp++, SNAPSNAP);
	    PUTSHORT(sp++, (outputhw->tr_transit_oui >> 16));
	    PUTSHORT(sp++, (outputhw->tr_transit_oui & 0xffff));
	    PUTSHORT(sp++, bridge_mac->canonical_mac.type_or_len);
	} else {
	    if ((TBR_CTR_SDE == output->tbridge_media)
                || (TBR_TR_SDE == output->tbridge_media)) {
		pak->datagramsize = TRING_ENCAPBYTES + SDE_ENCAPBYTES +
		    llcsize;
		pak->datagramstart = llcstart - (TRING_ENCAPBYTES +
						 SDE_ENCAPBYTES);
	    } else {
		pak->datagramsize = TRING_ENCAPBYTES + llcsize;
		pak->datagramstart = llcstart - TRING_ENCAPBYTES;
	    }

	    sp = (ushort *)((uchar *) pak->datagramstart);
	    PUTSHORT(sp++, ((AC_PRI0 << 8) | FC_TYPE_LLC));
	    dp = (ushort *) &bridge_mac->bs_da[0];
	    PUTSHORT(sp++, *dp++);		/* destination */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp++);
	    
	    PUTSHORT(sp++, *dp++);		/* source */
	    PUTSHORT(sp++, *dp++);
	    PUTSHORT(sp++, *dp);

	    if ((TBR_CTR_SDE == output->tbridge_media)
                || (TBR_TR_SDE == output->tbridge_media))
		tbridge_bfr_sde_header_wr_inline(output, pak->datagramstart +
						 TRING_ENCAPBYTES,
						 bridge_mac->canonical_mac.saddr);
	}
    }
    switch (output->tbridge_media) {
      case TBR_ETHER_SDE:
      case TBR_FDDI_SDE:
      case TBR_CTR_SDE:
      case TBR_TR_SDE:
      case TBR_HDLC_TRANSIT_SDE:
	  output->sde_said_db->egressing_dot10_packets[VLAN_TBRIDGE]++;
	  break;
      default:
	  break;
    }
    datagram_out(pak);
}

/*
 * bridge_floodcheck
 * Apply output checks to a datagram going through the flooder.
 * If has a type or no LSAP checking, check the type.
 * If has an LSAP, check it.
 * If LSAP okay and is SNAP, then check the SNAP type.
 */

boolean bridge_floodcheck (idbtype *output, paktype *pak, int len)
{
    ushort typecode;
    ushort lsap;
    ulong status;
    uchar *cp, *tcp;

    status = pak->if_input->hwptr->status;

    /*
     * IDB_BVI has to be considered here since it's a virtually
     * an ethernet i/f too --- CSCdj13545
     */
    if ((status & (IDB_ETHER|IDB_TUNNEL|IDB_BVI)) || 
	(pak->flags & PAK_TBRIDGE_IN)) {
	/*
	 * Source of packet is an ether like thingy.
	 */
	cp = pak->datagramstart + (2 * IEEEBYTES);
	typecode = GETSHORT(((ushort *)cp)++);
	if (len <= MAXETHERSIZE) {
	    if ((typecode > MAXETHERSIZE) || !output->span_lsap_out) {
		if (output->span_type_out)
		    if (!access_check_type(output->span_type_out, typecode))
			return(FALSE);
 		if ((typecode == ETHER_80D5_TYPE) && (enable_80d5)) {
 		    if (output->span_lsap_out) {
 			tcp = (pak->datagramstart + ETHER_ARPA_ENCAPBYTES + ETHER_80D5_EXTRA_BYTES);
 			lsap = GETSHORT(tcp);
 			if (!access_check_type(output->span_lsap_out, lsap))
			    return(FALSE);
 			return(TRUE);
 		    }
 		}
		return(TRUE);
	    }
	}
    } else if (status & IDB_TR) {
	cp = pak->datagramstart + TRING_ENCAPBYTES;
    } else {
	switch (pak->if_input->hwptr->type) {

        /*
         *  Datagram came from an FDDI interface type
         */

	case IDBTYPE_FDDIT:
	case IDBTYPE_BSI:

            cp = pak->datagramstart + FDDI_ENCAPBYTES;
            break;

	default:

            cp = (uchar *)-1;   /* Keep compiler happy */
            crashdump(0);
        }
    }

    lsap = GETSHORT(cp);
    if (output->span_lsap_out &&
	!access_check_type(output->span_lsap_out, lsap))
      return(FALSE);

    if ((lsap == SNAPSNAP) && output->span_type_out) {
	typecode = GETSHORT(cp + 6);
	if (!access_check_type(output->span_type_out, typecode))
	  return(FALSE);
    }
    return(TRUE);
}

/*
 * tbridge_ager
 * Called once a minute to check if station entries have aged out.
 */

void tbridge_ager (void)
{
    if (hybridge_started)
	gc_stations(NULL, FALSE);
}

/* 
 * bridge_address_check
 * This function is called to check the mac level address against the
 * address access list(s) -- both input and output. It is called from
 * interrupt level immediately after a new station address has been learned.
 */

void bridge_address_check (btetype *mac)
{
    macaddracctype *item;
    int access_list;
    ulong ul_mask, bit_number, word_number;
    ushort us_mask;
    idbtype *idb = mac->bte_interface;
    
    /*
     * Check input access list first.
     */
    bit_number = idb->number_bit_mask;
    word_number = idb->bit_mask_word;
    access_list = idb->span_address_in;
    if (access_list != 0) {
	item = mac_get_std_acl(access_list);
	if (item == NULL) 
	    mac->bte_permit_in[word_number] |= bit_number;
	else {
	    while (item) {
		us_mask = ~item->mask.u.sword[2];
		if ((mac->bte_mac_addr.sword[2] & us_mask) == item->addr.u.sword[2]) {
		    ul_mask = ~item->mask.u.lword[0];
		    if ((mac->bte_mac_addr.lword[0] & ul_mask) == 
			item->addr.u.lword[0]) {
			if (item->grant)
			    mac->bte_permit_in[word_number] |= bit_number;
			else
			    mac->bte_permit_in[word_number] &= ~bit_number;
			goto outlists;
		    }
		}
		item = item->next;
	    }
	    mac->bte_permit_in[word_number] &= ~bit_number;
	} 
    }	
    
    /*
     * Now check against output access list(s).
     */
outlists:
    access_list = idb->span_address_out;
    if (access_list != 0) {
	item = mac_get_std_acl(access_list);
	if (item == NULL)
	    mac->bte_permit_out[word_number] |= bit_number;
	else {
	    while (item) {
		us_mask = ~item->mask.u.sword[2];
		if ((mac->bte_mac_addr.sword[2] & us_mask) == item->addr.u.sword[2]) {
		    ul_mask = ~item->mask.u.lword[0];
		    if ((mac->bte_mac_addr.lword[0] & ul_mask) == 
			item->addr.u.lword[0]) {
			if (item->grant)
			    mac->bte_permit_out[word_number] |= bit_number;
			else
			    mac->bte_permit_out[word_number] &= ~bit_number;
			return;
		    }
		}
		item = item->next;
	    }
	    mac->bte_permit_out[word_number] &= ~bit_number;
	}
    }
}

/*
 * bridge_enq
 * Pass a received packet buffer to one of the bridging processes.
 * These are usually protocol broadcasts received when we are not routing.
 * "makeowncopy" is TRUE if the caller wants to keep the original for
 * other protocol processing.  IP and ARP make use of this functionality.
 * Returns TRUE if we passed packet or a copy to the bridging process.
 *
 * If the packet has the PAK_SRB_IN flag set, we pass it
 * to source route bridging, otherwise transparent bridging.
 */

boolean bridge_enq (paktype *pak, boolean makeowncopy)
{
    idbtype *idb;
    uchar bucket;
    ushort typecode = 0;
    ushort lsap;
    btetype *dest_mac;
    btetype *mac;
    leveltype level;
    spantype *span;
    uchar *svc, *tcp;
    lcitype *lci;
    fr_idb_struct_type *fr_idb;
    dxi_idb_struct_type *dxi_idb;
    sap_hdr *sap;
    snap_hdr *snap;
    ether_hdr *ether;
    tring_hdr *trh;
    fddi_hdr *fddi;
    boolean checktype = FALSE, checklsap = TRUE;
    ether_hdr ethermac;
    int offset;
    hwidbtype *hwidb;
    idbtype *macidb;

    idb = pak->if_input;
    hwidb = idb->hwptr;

    /*
     * Pass it off to the source route bridge
     */
    if (pak->flags & PAK_SRB_IN)
	return(reg_invoke_srt_srb_multicast(idb, pak, makeowncopy));
    /*
     * Attempt to transparent bridge the packet.
     * First make sure interface is part of some spanning tree
     */
    if (!idb->span_index || !spanarray[idb->span_index])
	return(FALSE);
    
    /*
     * Make sure that the port is at least in learning state
     */

    if (!idb->circ_canlearn)
	return(FALSE);

    /*
     * Create an ethernet format packet header to use in the subsequent
     * checking.
     * 
     * If we have type, set 'checktype' to TRUE(default is FALSE)
     * If we don't have llc, set 'checklsap' to FALSE(default is TRUE)
     */
    if (hwidb->bridge_transit || (hwidb->status & IDB_ETHER)) {
	/*
	 * CSCdi48388 - Packets received on transit links should have 
	 * PAK_TBRIDGE_IN set. A packet could have come in on a serial
	 * link that is also doing transit bridging... but may not be a
	 * transit packet itself. Throw it away.
	 */
	if (hwidb->bridge_transit && !(pak->flags & PAK_TBRIDGE_IN))
	    return(FALSE);

	ether = (ether_hdr *)pak->datagramstart;
	ieee_copy(ether->daddr, ethermac.daddr);
	ieee_copy(ether->saddr, ethermac.saddr);
        pak->addr_start = ether->daddr;

	sap = (sap_hdr *)ether->data;
	typecode = GETSHORT(&ether->type_or_len);

	if (typecode > MAXETHERSIZE) {
	    checktype = TRUE;
	    checklsap = FALSE;
	} else {
	    /* CSCdi15997: input-type-list does not permit bridging of
	     * broadcast packets.
	     * if packet is a snap packet, should update the typecode
	     * for later calling access_check_type(list, typecode).
	     */
	    lsap = GETSHORT((ushort *)&sap->dsap);
	    if (lsap == SNAPSNAP) {
		snap = (snap_hdr *)ether->data;
		typecode = GETSHORT(&snap->type);
		checktype = TRUE;
	    }
	}
	offset = 0;
    } else if (hwidb->status & IDB_TR) {
	trh = (tring_hdr *) pak->datagramstart;
	if (trh->saddr[0] & TR_RII)		/* no RIFs allowed */
	    return(FALSE);
	ieee_swap(trh->daddr, ethermac.daddr);
	ieee_swap(trh->saddr, ethermac.saddr);
        pak->addr_start = trh->daddr;

	sap = (sap_hdr *)trh->data;
	if (sap->dsap == SAP_SNAP) {
	    checktype = TRUE;
	    snap = (snap_hdr *)trh->data;
	    typecode = GETSHORT(&snap->type);
	}
	/* Offset and mask code for multicast source address */
	offset = 2;
    } else if (is_atm(hwidb)) {			/* ATM type device */
	ether = (ether_hdr *)pak->datagramstart;
	ieee_copy(ether->daddr, ethermac.daddr);
	ieee_copy(ether->saddr, ethermac.saddr);
        pak->addr_start = ether->daddr;

	sap = (sap_hdr *)ether->data;
	typecode = GETSHORT(&ether->type_or_len);
	checktype = (typecode > MAXETHERSIZE) || !idb->span_lsap_in;
	offset = 0;

        /* CSCdi15997: input-type-list does not permit bridging of
         * broadcast packets.
         * if packet is a snap packet, should update the typecode
         * for later calling access_check_type(list, typecode).
         */
        lsap = GETSHORT((ushort *)&sap->dsap);
        if ((typecode < MAXETHERSIZE) && (lsap == SNAPSNAP)) {
            snap = (snap_hdr *)ether->data;
            typecode = GETSHORT(&snap->type);
        }
    } else /* if (hwidb->status & IDB_FDDI) */ {
	fddi = (fddi_hdr *) pak->datagramstart;
	if (hwidb->type == IDBTYPE_BSI) {
	    ieee_swap(fddi->daddr, ethermac.daddr);
	    ieee_swap(fddi->saddr, ethermac.saddr);
            pak->addr_start = fddi->daddr;
        } else {
	    /*
	     * must be a FDDIT, running transparently.  Note FDDIT is
	     * canonical.
	     */
	    ieee_copy(fddi->daddr, ethermac.daddr);
	    ieee_copy(fddi->saddr, ethermac.saddr);
            pak->addr_start = fddi->daddr;	
        }
	if (ethermac.saddr[0] & 0x01)		/* no RIFs allowed */
	    return(FALSE);
	sap = (sap_hdr *)fddi->data;
	if (sap->dsap == SAP_SNAP) {
	    checktype = TRUE;
	    snap = (snap_hdr *)fddi->data;
	    typecode = GETSHORT(&snap->type);
	}
	offset = 1;
    }

    /*
     * Check if destination should be discarded
     */
    pak->bridgeptr = NULL;
    bucket = ethermac.daddr[4] ^ ethermac.daddr[5];
    span = idb->span_ptr;
    for (dest_mac = span->bridge_tbl[bucket]; dest_mac; dest_mac = dest_mac->bte_next) {
	if (ieee_equal(ethermac.daddr, (uchar *)&dest_mac->bte_mac_addr.sword[0])) {
	    if (dest_mac->bte_action == BRIDGE_DISCARD) {
		if (!makeowncopy)
		  datagram_done(pak);
		return(TRUE);
	    }
	    macidb = dest_mac->bte_interface;
	    if (macidb) {
		if (macidb->span_address_out &&
		    !(dest_mac->bte_permit_out[macidb->bit_mask_word] & 
		      macidb->number_bit_mask)) {
		    if (!makeowncopy)
		      datagram_done(pak);
		    return(TRUE);
		}
		pak->if_output = macidb;
		if (is_x25(macidb->hwptr)) {
		    pak->bridgeptr = dest_mac->bte_proxy_addr.x25map_ptr;
		}
		if (is_frame_relay(macidb->hwptr) || is_atm_dxi(macidb->hwptr)) {
		    pak->bridgeptr = &dest_mac->bte_proxy_addr.fr_dlci;
		}

                /*
                 * Check for encapsulation bridging over a fddi interface.
                 * If it is, pass destination proxy mac address with pak.
                 */
                if (is_fddi_br_encap(macidb->hwptr)) {
                  pak->bridgeptr = &dest_mac->bte_proxy_addr.proxy_mac;
                }
		if (is_smds(macidb->hwptr)) {
		    pak->bridgeptr = (void *)dest_mac->bte_mac_addr.lword;
		}
		if (is_atm(macidb->hwptr)) {
		    pak->bridgeptr = (void *)dest_mac->bte_mac_addr.lword;
		}
		/*
		 * For LANE, this is a place to meake sure that the entry is
		 * not flushing, if so toss!
		 */
	    }
	    break;
	}
    }	    

    /*
     * See if the MAC-level source address is in the bridge database.
     */
    bucket = ethermac.saddr[4] ^ ethermac.saddr[5];
    for (mac = span->bridge_tbl[bucket]; mac; mac = mac->bte_next) {
	if (ieee_equal(&ethermac.saddr[0], (uchar *)&mac->bte_mac_addr.sword[0])) {
	    if (mac->bte_action == BRIDGE_DISCARD) {
		mac->bte_rxcount++;	/* count'em to keep the station entry current */
		if (!makeowncopy)
		    datagram_done(pak);
		return(TRUE);
	    }
		    
	    /*
	     * In general, all firmware is supposed to ensure that a packet
	     * that we transmit is not passed back to us as a received frame.
	     * This either happens because hearing our own frame is impossible
	     * on this media, or because (like the SBE-16 card) the card should
	     * be smart enough to eat the frames it generates.
	     *
	     * Just to be safe, if a station appears to move within
	     * 5 seconds of its last receive, drop the packet.
	     *
	     * This is more than being safe; this is required as the card often
	     * breaks down on this task.
	     */
	    if (!mac->bte_attributes && mac->bte_interface &&
		(mac->bte_interface != idb)) {
		
		if (!mac->bte_interface->circ_group &&
		    (((mac->bte_rxcount + mac->bte_filtered_rxcount) !=
		      mac->bte_prev_rxcount) ||
		     (CLOCK_IN_INTERVAL(mac->bte_timer, 5 * ONESEC)))) {
		    if (!makeowncopy)
			datagram_done(pak);
		    return(TRUE);
		}
		
		/*
		 * Remove any hardware address filters.
		 */
		if (mac->bte_interface->tbridge_media == TBR_FDDI) {
		    reg_invoke_media_delete_hardware_address_filter
			(mac->bte_interface->hwptr,
			 (uchar *)&mac->bte_mac_addr.sword[0], NULL);
		}
		
		mac->bte_interface = idb;
		
		/*
		 * Recaculate the input permissions.
		 */
		if (idb->span_address_in || idb->span_address_out)
		    bridge_address_check(mac);

		/* 
		 * The return value of this registry call should not affect the
		 * handling of the frame being bridged. (CSCdi14578)
		 */
		reg_invoke_srt_learn_source(idb, pak, mac);
 	    }

            if (is_atm_lane(idb)) {
		/*
		 * For LANE, Learn the VC only on direct data. Otherwise, don't
		 * learn this source. 
		 */
		vc_info_t *vc;

                vc  =  pak->atm_vc;

		if (vc && vc->user_vc_type == LANE_LEC_DATA){
		    tbridge_lane_learn_source_inline(mac, 
						     idb, 
					             pak->atm_vcid,
						     BTE_READY);
	            reg_invoke_srt_learn_source(idb, pak, mac);
		} 

	    }
	    goto src_found;
	}
    }

    /*
     * Before adding an entry, check if multicast source.
     */
    if (ethermac.saddr[0] & 0x01) {
  	if (!span->mcast_source) {
 	    if (!makeowncopy)
  		datagram_done(pak);
  	    return(TRUE);
  	}
 	mac = NULL;
 	goto src_found;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    mac = create_bte_inline(span, &ethermac.saddr[0], idb);
    if (mac) {
	bridge_address_check(mac);		/* Initialize access lists */

	reg_invoke_srt_bce_add(span, mac, hwidb);

	/*
	 * If this came in on FDDI, set the transit multicast as
	 * the previous hop. Future return traffic will set a better
	 * path.
	 */

	if ((hwidb->status & IDB_FDDI) && (hwidb->fddi_options & FDDI_ENCAPSULATE)) {
	    if (hwidb->type == IDBTYPE_FDDIT)
		ieee_copy(transit_bridge_multicast, mac->bte_proxy_addr.proxy_mac);
	    else
		ieee_copy(bit_swapped_transit_multicast, mac->bte_proxy_addr.proxy_mac);
	} else if (is_x25(hwidb)) {
  	    lci = pak->x25_cb;
	    /*because of the edisms() in this path, x25_lci_delete() can
	     *delete the lci pointed to by pak->cb */
	    if (lci && validmem (lci) && validmem(lci->lci_map))
		    mac->bte_proxy_addr.x25map_ptr = lci->lci_map;
	    else 
		    mac->bte_proxy_addr.x25map_ptr = NULL;
	} else if (is_frame_relay(hwidb)) {
	    fr_idb = hwidb->frame_relay_stuff;
	    mac->bte_proxy_addr.fr_dlci = 
		DLCI_TO_NUM(GETSHORT(pak->datagramstart - FR_TRANSIT_ENCSIZE(pak)));
	} else if (is_atm_dxi(hwidb)) {
	    dxi_idb = hwidb->atm_dxi_stuff;
	    mac->bte_proxy_addr.fr_dlci = BR_DFA_TO_NUM(GETSHORT(pak->datagramstart
			- ATM_DXI_BRIDGE_ENCAPBYTES),dxi_idb);
	} else if (is_smds(hwidb)) {
	    smdstype *smh = (smdstype *)(pak->datagramstart 
	      - (SMDS_SNAP_ENCAPBYTES + SMDS_BRIDGE_PAD_SIZE));
	    bcopy(smh->smdssrc, mac->bte_proxy_addr.proxy_mac, 
		STATIONLEN_SMDS64);
	} else if (is_atm_lane(idb)) {
	    /*
             * For LANE, Learn the BUS VC. This will force things to go on
             * BUS and trigger an LE_ARP for the unknown destination.
             */

	    vcd_t vcd;
		
	    vcd = idb->lane_info->lec_info->mcast_send.vcd;             
	    tbridge_lane_learn_source_inline(mac, 
					     idb, 
					     vcd,
					     BTE_LANE_FLOOD);
	} else if (is_atm(hwidb)) {
	    memset(mac->bte_proxy_addr.proxy_mac, 0, STATIONLEN_SMDS64);
	    if (pak->encsize < ATM_SMDS_ENCAPBYTES) {
                /*
                 * Do NOT assume that atm header preceding
                 * pak->datagramstart is still there because
                 * the pak may have been realigned. Typical
                 * example is in raw_enqueue().
                 * Since we have no way of knowing if a pak has
                 * been realigned, we never know the atm header
                 * is valid or not. Get the vc from atm_db.
                 * If we can't get vc from it, don't learn the
                 * src mac here.
                 */
                if (hwidb->atm_db && hwidb->atm_db->fast_vc_info) {
                    mac->bte_proxy_addr.vc_num =
                        hwidb->atm_db->fast_vc_info->vc;
                } else {
                    reg_invoke_srt_bce_remove(span, mac);
                    bridge_entry_global_remove(&ethermac.saddr[0]);
                    mac = NULL;
                }
	    } else {
		smdstype *smh = (smdstype *)(pak->datagramstart 
					     - pak->encsize + 
					     ATM_ENCAP_HDRBYTES);
		bcopy(smh->smdssrc, mac->bte_proxy_addr.proxy_mac, 
		      STATIONLEN_SMDS64);
	    }
	}
    }
    else
	span->learned_entry_discards++;

    reset_interrupt_level(level);

    /*
     * Fix for CSCdi07934:
     * - set action to discard if source learning on the bridge is disabled
     * - count this first one received, but dump it here instead of flooding
     * - do this outside of the level adjustment since we return from here
     */
    if ((mac) && idb->span_ptr->acquire == FALSE) {
	mac->bte_action = BRIDGE_DISCARD;
	mac->bte_rxcount++;
	if (!makeowncopy)
	    datagram_done(pak);
	return(TRUE);
    }

 src_found:

    if (mac)
	mac->bte_rxcount++;

    /*
     * Check that the input port is in a forwarding state.
     */

    if (!idb->circ_canforward) {
	if (!makeowncopy)
	    datagram_done(pak);
	return(TRUE);
    }

    /*
     * Make sure we are not simply sending the frame back from where it came
     * (loopback...)
     */
    if (pak->if_output) {
	if ((pak->if_output->if_number == pak->if_input->if_number) ||
	    (pak->if_input->circ_group &&
	     (pak->if_output->circ_group == pak->if_input->circ_group))) {
	    if (!makeowncopy)
		datagram_done(pak);
	    return(TRUE);
	}
    }


    /*
     * Do input LAT group code filtering. Check to see if this
     * is a service announcement, and that that group code
     */

    if (span->lat_svc_filter) {
	if (checktype && typecode == TYPE_LAT) {
	    svc = pak->datagramstart + ETHER_ARPA_ENCAPBYTES + offset;
	    if (svc[0] == LAT_SERVICE && svc[12] <= 32) {
		pak->lat_groupmask = &svc[12];

		/*
		 * This is a service announcement. Check to see if
		 * there is an input deny list on this interface,
		 * and if so, don't bridge this packet if there
		 * is a group code overlap 
		 */

		if (idb->span_lat_in_svc_deny[0] &&
		    lat_compare_groups(pak->lat_groupmask,
				       idb->span_lat_in_svc_deny)) {
		    if (bridge_lat_debug)
		      bridge_debug_lat(idb, pak, "input deny");
		    if (!makeowncopy)
		      datagram_done(pak);
		    return(TRUE);
		}

		/*
		 * Check to see if there is an input permit list,
		 * and if so, get out if this message does not match
		 * the list.
		 */

		if (idb->span_lat_in_svc_permit[0] &&
		    !lat_compare_groups(pak->lat_groupmask,
					idb->span_lat_in_svc_permit)) {
		    if (bridge_lat_debug)
		      bridge_debug_lat(idb, pak, "input permit");
		    if (!makeowncopy)
		      datagram_done(pak);
		    return(TRUE);
		}
	    }
	}
    }

    /*
     * Do input type/lsap access checking.
     *  - If type filter is configured and has type, check the type.
     *  - If lsap filter is configured and has lsap, check the lsap.
     */

    if (idb->span_flag_in) {
	if (idb->span_type_in && checktype) {
	    if (!access_check_type(idb->span_type_in, typecode)) {
		if (!makeowncopy) 
		    datagram_done(pak);
		return(TRUE);
	    }
	}

	if (idb->span_lsap_in && checklsap) {
            if ((typecode == ETHER_80D5_TYPE) && enable_80d5) {
                tcp = (pak->datagramstart + ETHER_ARPA_ENCAPBYTES + offset +
                       ETHER_80D5_EXTRA_BYTES);
                lsap = GETSHORT(tcp);
	    } else {
		lsap = GETSHORT(&sap->dsap);
	    }

	    if (!access_check_type(idb->span_lsap_in, lsap)) {
		if (!makeowncopy) 
		    datagram_done(pak);
		return(TRUE);
	    }
	}
    }
    
    /*
     * Check against the input address access list.
     */
    if (idb->span_address_in) {
	if (!access_check_mac(idb->span_address_in, &ethermac.saddr[0])) {
	    if (!makeowncopy)
		datagram_done(pak);
	    return(TRUE);
	}
    }

    /*
     * Check against the input pattern address list.
     */

    if (idb->span_pattern_in) {
	if (!xmacaccesscheck(hwidb, idb->span_pattern_in, 
			     pak->addr_start, FALSE)) {
	    if (!makeowncopy)
		datagram_done(pak);
	    return(TRUE);
	}
    }	
    
    /*
     * Should not be discarded.
     * Enqueue a copy or the original, depending on caller's desires.
     */
    pak = makeowncopy ? pak_duplicate(pak) : pak;
    if (pak) {
	if (dest_mac)
	    dest_mac->bte_txcount++;
	pak->linktype = LINK_BRIDGE;
	process_enqueue_pak(bridgeQ, pak);
    }
    return(TRUE);
}

/*
 * bridge_debug_lat - Output a LAT debugging message
 */

void bridge_debug_lat (idbtype *idb, paktype *pak, char *reason)
{
    uchar *node;

    node = pak->lat_groupmask + *pak->lat_groupmask + 1;

    buginf("\nTBRIDGE: LAT announcement from %*s (%e) dropped due "
	   "to %s %s mask",
	   node[0], &node[1], pak->datagramstart + IEEEBYTES,
	   idb->namestring, reason);
}

/*
 * Locate the station entry for a given MAC address in the
 * given forwarding table and return a pointer to it.
 */

btetype *bridge_get_stn (uchar *mac, btetype *table[])
{
    uchar index;
    btetype *entry;
    leveltype level;

    /*
     * Calculate the hash table index.
     */
    index = mac[IEEEBYTES-2] ^ mac[IEEEBYTES-1];

    /*
     * Linear search the list of bridge table entries for matching MAC addres.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    for (entry = table[index]; entry; entry = entry->bte_next)
	if (ieee_equal(&mac[0], (uchar *)&entry->bte_mac_addr.sword[0]))
	    break;
    reset_interrupt_level(level);

    return(entry);
}

/*
 * Locate the lexically next station entry to a given MAC address in the
 * given forwarding table and return a pointer to it.
 */

btetype *bridge_get_next_stn (uchar *mac, btetype *table[])
{
    int i, j, k;
    btetype *entry, *found_entry = NULL;
    uchar *next_mac, *found_mac = NULL;
    leveltype level;

    /* Search all lists in the table */
    for (i=0; i < NETHASHLEN; i++) {

	level = raise_interrupt_level(NETS_DISABLE);

	/* Search all entries in a list */
	for (entry = table[i]; entry; entry = entry->bte_next) {

	    next_mac = (uchar *)(&entry->bte_mac_addr.sword[0]);

	    /* Check for greater than the given MAC */
	    for (j = 0; j < IEEEBYTES; j++) {
		if (next_mac[j] < mac[j])
		    break;
		else if ((next_mac[j] > mac[j]) ||
                         ((j == (IEEEBYTES - 1)) && (next_mac[j] == mac[j]))) {
		    /* Initialize the candidate with first found. */
		    if (!found_entry) {
			found_entry = entry;
			found_mac = next_mac;
		    } else {

			/* Check for less than the current candidate */
			for (k = 0; k < IEEEBYTES; k++) {
			    if (next_mac[k] > found_mac[k])
				break;
			    else if (next_mac[k] < found_mac[k]) {
				found_entry = entry;
				found_mac = next_mac;
				break;
			    }
			}
			break;
		    }
		}
	    }
	}

	reset_interrupt_level(level);
    }

    return(found_entry);
}


/*
 * bridge_is_broadcast
 * Ensure that this is an appropriate broadcast message for the
 * medium on which we are bridging broadcasts.
 *
 * For the three media on which we support bridging of received multicasts,
 * token ring, FCIT, and ethernet, we check the appropriate media-specific
 * place for the broadcast bit. If this is not a broadcast, or not from
 * the appropriate media, we will not pass the message on to the bridging
 * process.  It can only be a multicast of somekind that makes it here.
 * Unicasts or packets that don't match one of our multicast filter bits
 * are either dealt with by bridge_receive or are dumped.
 *
 * If this packet came in off a transit bridging media, PAK_TBRIDGE_IN
 * should be set and the packet is assumed to look exactly like an
 * ethernet packet.  That is pak->datagramstart is assumed to point
 * at the first byte of DA.
 *
 * If the packet came in off a transparent bridging media than we check
 * for a multicast DA.  If not we drop it.  The assumption is that we
 * have already been through bridge_receive and it failed so no point
 * in doing it again.
 * 
 * On token ring, the first byte of DA follows two bytes of AC/FC.  On
 * FDDI, it follows one byte of FC.  All other media is assumed to start
 * with byte 0.  FDDI has same bit order as TR (bit 7 of first DA byte).
 * FDDI-T has the same order as ETher (bit 0 of first DA byte).
 */

static boolean bridge_is_unicast (paktype *pak)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_input;
    if (!idb)
	return(TRUE);
    hwidb = idb->hwptr;
    if (hwidb->bridge_transit) {
	if (!(pak->flags & PAK_TBRIDGE_IN))
	    return(TRUE);
	if (!(pak->datagramstart[0] & 0x01))
	    return(TRUE);
    } else {
	if (hwidb->type == IDBTYPE_FDDIT) {
	    if (!(pak->datagramstart[1] & 0x01))
		return(TRUE);
	} else if (hwidb->status & IDB_TR) {		/* Try token ring */
	    if (!(pak->datagramstart[2] & 0x80))
		return(TRUE);
	} else if (hwidb->type == IDBTYPE_BSI) {
	    if (!(pak->datagramstart[1] & 0x80))
		return(TRUE);
	} else {
	    if (!(pak->datagramstart[0] & 0x01))
		return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * bridge_entry_global_remove
 *
 * Remove specific-mac-address entry from all bridge tables.
 *
 * Called by SERVICE_BRIDGE_ENTRY_GLOBAL_REMOVE
 */

void bridge_entry_global_remove (uchar const *addr)
{
    btetype *mac = NULL, *last = NULL;
    int index;
    spantype *span;

    for (index = 0; index < SPAN_MAXINDEX; index++) {
        span = spanarray[index];
        if (!span)
            continue;

        mac = bridge_entry_locate(span, addr, &last);
        if (mac)
            bridge_entry_remove(span, mac, last);
    }
}

/*
 * tbridge_bte_entry_locate
 *
 * Wrapper for bridge_entry_locate. Checks for Non-null MAC addr and span
 * pointer before calling bridge_entry_locate().
 *
 */
btetype *tbridge_bte_entry_locate (spantype *span, uchar const *addr,
				   btetype **lastp)
{
   if (!span || !addr) {
       return (NULL);
   }
   return (bridge_entry_locate (span, addr, lastp));
}

/*
 * bridge_entry_locate
 *
 * Locate the bridge table entry for a given MAC address
 * in a given bridge table.
 *
 * Input Parameters:
 *  span, addr
 * Output Parameters:
 *  lastp
 * Return
 *  pointer of the entry. NULL if couldn't locate it.
 */

btetype *bridge_entry_locate (spantype *span, uchar const *addr,
				  btetype **lastp)
{
    int hash;
    leveltype level;
    btetype *mac, *last = NULL;

    *lastp = NULL;
    hash = addr[IEEEBYTES-2] ^ addr[IEEEBYTES-1];
    level = raise_interrupt_level(NETS_DISABLE);
    for (mac = span->bridge_tbl[hash]; mac; mac = mac->bte_next) {
        if (ieee_equal((uchar *)&mac->bte_mac_addr.sword[0], addr)) {
	    *lastp = last;
	    break;
        } else {
	    last = mac;
	}
    }
    reset_interrupt_level(level);
    return(mac);
}

/*
 * tbridge_bte_entry_remove
 *
 * Wrapper function for bridge_entry_remove callable across subsystems
 * through a registry call. 
 *
 */
void tbridge_bte_entry_remove (spantype *span, btetype *mac,
				  btetype *last)
{
   bridge_entry_remove(span, mac, last);
}

 

/*
 * bridge_entry_remove
 *
 * Remove a given bridge table entry from a given bridge table.
 * Remove any associated bridge cache entry or fddi cam entry.
 *
 * Parameters:
 *  span
 *  mac   : pointer to the bridge table entry we want to remove
 *  last  : pointer to the preceeding bridge table entry
 */

void bridge_entry_remove (spantype *span, btetype *mac, btetype *last)
{
    leveltype level;
    int hash = 0;
    uchar *macaddr;

    if (!span || !mac)
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Make sure that this entry has not been removed
     * since we located it and its predecessor.  This
     * check anticipates the day when bte removal might
     * occur at interrupt level.
     */
    if (last) {
	if (last->bte_next != mac) {
	    reset_interrupt_level(level);
	    return;
	}
    } else {
        macaddr = (uchar *)&(mac->bte_mac_addr.sword[0]);
        hash = macaddr[IEEEBYTES-2] ^ macaddr[IEEEBYTES-1];
	if (span->bridge_tbl[hash] != mac) {
	    reset_interrupt_level(level);
	    return;
	}
    }

    /*
     * See if there is a bridge cache entry to remove:
     */
    reg_invoke_srt_bce_remove(span, mac);

    /*
     * See if there is a hardware address filter to remove:
     * xx_bridge_media and mci_bridge_media will eventually
     * be replaced by a single variable - bridge_media.
     * This will be done in 10.1/NAC1, the reasoning being
     * that we do not want to risk destabilizing the code
     * now, and 10.1 will get a lot wider range of testing.
     */
    if (mac->bte_interface &&
        (mac->bte_interface->tbridge_media  == TBR_FDDI))
	reg_invoke_media_delete_hardware_address_filter
	    (mac->bte_interface->hwptr,
	     (uchar *) &mac->bte_mac_addr.sword[0], NULL);

    /*
     * Remove the bridge table entry itself:
     */
    if (last) {
        last->bte_next = mac->bte_next;
    } else
        span->bridge_tbl[hash] = mac->bte_next;

    /*
     * Zero out the struct for this entry
     */
    memset(mac, 0, sizeof(btetype));

    enqueue(&bteQ, mac);
    bte_count++;
    reset_interrupt_level(level);
}

/*
 * bridge_routefailed
 * Common routine to pass an unwanted packet to the bridging code.
 * You probably should think about using datagram_done() instead.
 */

void bridge_routefailed (paktype *pak, boolean copyflag, boolean unicast_ok)
{
    if (unicast_ok || !bridge_is_unicast(pak)) {
	if (bridge_enq(pak, copyflag))
	    return;
    }
    /*
     * Eat the packet
     */
    if (copyflag)
	return;
    idb_increment_rx_proto_counts(pak, ACCT_PROTO_OTHER);
    datagram_done(pak);
}

/*
 * dld_get_output_idb
 *
 * Return a pointer to a circuit group idb based on a function
 * of the (source, destination) or source address.
 *
 * Return NULL if we can't get valid ccg_idb_count - it means
 * all circuit group interfaces are unavailable or we are suspending
 * the transmission.
 */

idbtype *dld_get_output_idb (spantype *span, short group, 
			     uchar *srcmac, uchar *dstmac)
{
    ushort hash;
    uint counter;

    if (!span)
	return(NULL);

    counter = span->cct_groups[group].ccg_idb_count;

    /*
     * Possible cases for counter is zero:
     *  (1)All Circuit group interfaces are unavailable
     *  (2)Still in pause interval
     */
    if (!counter)
	return(NULL);

    if (span->cct_groups[group].srcmac_based)
	hash = srcmac[IEEEBYTES-1] ^ srcmac[IEEEBYTES-2];
    else
	hash = (srcmac[IEEEBYTES-1] ^ dstmac[IEEEBYTES-1]) ^
	    (srcmac[IEEEBYTES-2] ^ dstmac[IEEEBYTES-2]);

    return(span->cct_groups[group].idb_array[hash % counter]);
}

/*
 * ccg_pause_timer_expiry
 *
 * When circuit group pause timer expires, we need to 
 *	(1)Stop the pause timer
 * 	(2)Restore the correct number for active circuit group idb number
 */

void ccg_pause_timer_expiry (spantype *span, int group)
{

    leveltype level;

    if (!span || !group)
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    TIMER_STOP(span->cct_groups[group].ccg_pause_timer);
    span->cct_groups[group].ccg_idb_count =
	span->cct_groups[group].ccg_avail_idb_count;

    reset_interrupt_level(level);
}

/*
 * show_bridge_circuit_group
 *
 * Display circuit-group information
 */

void show_bridge_circuit_group (parseinfo *csb)
{
    spantype *span;
    int brgroup = 0, circgroup = 0;
    int spanindex, group, index, count, avail_count, unavail_count;
    idbtype *idb = NULL;
    boolean mac_option = FALSE;
    mac_address_type dstaddr, srcaddr;

    if (!bridge_enable)
	return;

    brgroup = GETOBJ(int,1);            /* 0 means all bridge groups */
    circgroup = GETOBJ(int,2);  	/* 0 means all circuit groups */

    /*
     * See if the expanded format of the command has been entered.
     * show bridge <bridge-group> circuit-group  <circuit-group> 
     *			<src-mac-addr> <dst-mac-addr>
     */
    if (GETOBJ(hwaddr,1)->length) {
	mac_option = TRUE;
	ieee_copy(GETOBJ(hwaddr,1)->addr, srcaddr.u.byte);
    }
    if (GETOBJ(hwaddr,2)->length)
	ieee_copy(GETOBJ(hwaddr,2)->addr, dstaddr.u.byte);

    automore_enable(NULL);
    for (spanindex = 1; spanindex < SPAN_MAXINDEX; spanindex++) {
	if (brgroup && (brgroup != spanindex))
	    continue;
	span = spanarray[spanindex];
	if (!span)
	    continue;
	/*
	 * Now display each circuit group
	 */
	for (group = 1; group < CCG_MAXGROUPS; group++) {
	    if (circgroup && (circgroup != group))
		continue;
	    if (mac_option) {
		/* 
		 * Parser won't allow circgroup == 0 if expanded format 
		 * has been entered.
		 */
		if (!GETOBJ(hwaddr,2)->length && 
		    !span->cct_groups[group].srcmac_based) {
		    printf("\n%%Destination MAC address required");
		    return;
		}
		if (span->cct_groups[group].srcmac_based &&
		    GETOBJ(hwaddr,2)->length) {
		    printf("\n%%Please enter source MAC address only");
		    return;
		}
		idb = dld_get_output_idb(span, group, srcaddr.u.byte,
					 dstaddr.u.byte);
		if (idb)
		    printf("\n Output circuit group interface is %s", 
			   idb->namestring);
		else 
		    printf("\n No output circuit group interface is available");
		continue;
	    }

	    count = span->cct_groups[group].ccg_idb_count;
	    avail_count = span->cct_groups[group].ccg_avail_idb_count;
	    unavail_count = span->cct_groups[group].ccg_unavail_idb_count;

	    if (avail_count || unavail_count) {
		printf("\n Bridge group %d Circuit group %d:", spanindex, group);
		if (span->cct_groups[group].ccg_pause_interval !=
		    CCG_DEFAULT_PAUSE_INTERVAL)
		    printf("\n   Transmission pause interval is %ums", 
			   span->cct_groups[group].ccg_pause_interval);
		if (span->cct_groups[group].srcmac_based)
		    printf("\n   Output interface selection is source-based");
	    }

	    /*
	     * Show available interfaces if the transmission is paused.
	     */
	    if (!count && 
		TIMER_RUNNING(span->cct_groups[group].ccg_pause_timer) &&
		SLEEPING(span->cct_groups[group].ccg_pause_timer)) {
		printf("\n   Transmission is paused");
		for (index = 0; index < avail_count; index++) {
		    idb = span->cct_groups[group].idb_array[index];
		    printf("\n   Interface %s: %sinserted, %slearning, %sforwarding ",
			   idb->namestring, 
			   idb->circ_inserted ? NULL : "not ",
			   idb->circ_canlearn ? NULL : "not ",
			   idb->circ_canforward ? NULL : "not ");
		}
	    }

	    /*
	     * Show active interfaces.
	     */
	    for (index = 0; index < count; index++) {
		idb = span->cct_groups[group].idb_array[index];
		printf("\n   Interface %s: %sinserted, %slearning, %sforwarding ",
		       idb->namestring,
		       idb->circ_inserted ? NULL : "not ",
		       idb->circ_canlearn ? NULL : "not ",
		       idb->circ_canforward ? NULL : "not ");
	    }

	    /*
	     * Show unavailable interfaces.
	     */
	    for (index = 0; index < unavail_count; index++) {
		idb = span->cct_groups[group].idb_array[CCG_MAXCIRCUITS - 1 - index];
		printf("\n   Interface %s is unavailable", idb->namestring);
	    }
	}
    }
    automore_disable();
}

/*
 * ccg_get_next_idb
 *
 * Get the next circuit-group idb in idb_array[group]
 */

idbtype *ccg_get_next_idb (idbtype *idb)
{    
    spantype *span;
    int group, i, count;

    span = idb->span_ptr;
    group = idb->circ_group;
    
    if (!idb->tbridge_on_swidb || !group)
	return (NULL);

    count = span->cct_groups[group].ccg_avail_idb_count - 1;
    for (i = 0; i < count; i++) {
	if (span->cct_groups[group].idb_array[i] == idb)
	    return(span->cct_groups[group].idb_array[i + 1]);
    }
    return (NULL);
}

/*
 * These functions really belong in tbridge/tbridge.c where
 * they started out life, but some lite images build with
 * only process-level tbridging (which can't be too useful),
 * so here they be.
 */

boolean
tbridge_smf_update (uchar *mac_addr, idbtype *swidb, ulong result)
{
    if (swidb->tbridge_on_swidb)
	return(smf_insert_address(mac_addr, swidb, result));
    else
	return(smf_delete_address(mac_addr, swidb, result));
}

boolean
tbridge_smf_delete (uchar *mac_addr, idbtype *swidb, ulong result)
{
    return(smf_delete_address(mac_addr, swidb, result));
}

boolean
tbridge_smf_delete_by_result (idbtype *swidb, ulong result)
{
    return(smf_delete_by_result(swidb, result));
}

/*
 * bvi_smf_update
 *
 * bvi(Bridge-group Virtual Interface) represents the bridge group. 
 * It is a routed interface(bridging is NOT supported on the BVI) and 
 * hence no smf itself.
 *
 * We will populate bridged interfaces with this mac_addr. Normally this
 * applies to all bridged interface, so the "flag" should be IDB_BRIDGE.
 * Sometimes the mac_addr is meant for specific type of interface, e.g.
 * fddi, token..., then the flag will be set accordingly.
 * 
 * We don't assume the mac_addr we pass in for BVI is in canonical
 * format or not. If caller want to populate token format address in 
 * token bridging interface, caller has to bitswapp the address and set
 * the flag accordingly.
 *
 * Should be called ONLY when idb is a BVI.
 */

boolean
bvi_smf_update (uchar *mac_addr, idbtype *bvi, ulong result, ulong flag)
{

    spantype *span;
    int ii;
    idbtype *port;

    span = spanarray[bvi->hwptr->unit];
    if (!span)
	return (FALSE);

    for (ii = 0; ii < span->port_count; ii++) {
	port = span->port[ii];
	if (port->hwptr->status & flag)
	    if (!smf_insert_address(mac_addr, port, result))
		return (FALSE);
    }
    return (TRUE);
}

/*
 * bvi_smf_delete
 *
 * bvi is Bridge-group Virtual Interface.
 */
boolean
bvi_smf_delete (uchar *mac_addr, idbtype *bvi, ulong result, ulong flag)
{
    spantype *span;
    int jj;
    idbtype *port;
    
    span = spanarray[bvi->hwptr->unit];
    if (!span)
	return (FALSE);
    
    for (jj = 0; jj < span->port_count; jj++) {
	port = span->port[jj];
	if (port->hwptr->status & flag)
	    if (!smf_delete_address(mac_addr, port, result))
		return (FALSE);
    }
    return (TRUE);
}

/*
 * bvi_smf_delete_by_result
 *
 * bvi is Bridge-group Virtual Interface.
 */
boolean
bvi_smf_delete_by_result (idbtype *bvi, ulong result)
{

    spantype *span;
    int kk;
    idbtype *port;
    
    span = spanarray[bvi->hwptr->unit];
    if (!span)
	return (FALSE);
    
    for (kk = 0; kk < span->port_count; kk++) {
	port = span->port[kk];
	if (!smf_delete_by_result(port, result))
	    return (FALSE);
    }
    return (TRUE);
}

void
tbridge_smf_show (idbtype *swidb)
{
    smf_show_filter(swidb);
}

char *tbridge_encap_string(idbtype *swidb)
{
    switch (swidb->tbridge_media) {
    case TBR_ATM_TRANSIT:	  return("RFC 1483");
    case TBR_LANE_ETHER:          return("LANE Ethernet");
    case TBR_FDDI_TRANSIT:        return("Transit");
    case TBR_SMDS_TRANSIT:        return("802.6");
    case TBR_FRAME_RELAY_TRANSIT: return("Frame Relay");
    case TBR_ETHER_SDE:
    case TBR_FDDI_SDE:
    case TBR_CTR_SDE:
    case TBR_TR_SDE:              return("SDE");
    case TBR_HDLC_TRANSIT_SDE:    return("HDLC SDE");
    case TBR_ETHER_ISL_VLAN:      return("ISL");
    default:                      return(NULL);
    }
}

void tbridge_clear_cache (int index)
{
    spantype *span;
 
    span = spanarray[index];
    gc_stations(span, TRUE);
}
