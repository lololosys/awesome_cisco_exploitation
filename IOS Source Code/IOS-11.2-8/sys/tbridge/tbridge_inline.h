/* $Id: tbridge_inline.h,v 3.7.20.22 1996/08/22 20:29:04 hou Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_inline.h
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Generic in-line functions for transparent bridging.
 *------------------------------------------------------------------
 * $Log: tbridge_inline.h,v $
 * Revision 3.7.20.22  1996/08/22  20:29:04  hou
 * CSCdi63701:  ISL Bridging on 4500/4700 platform is not working.
 * Branch: California_branch
 *
 * Revision 3.7.20.21  1996/08/15  01:41:42  mcieslak
 * CSCdi65828:  BADSHARE while bridging SMDS over HSSI subinterfaces
 * Branch: California_branch
 * Need to alter the action variable so that datagram_done does not get
 * invoked more than once.
 *
 * Revision 3.7.20.20  1996/08/03  23:39:11  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.20.19  1996/08/03  01:23:09  rbadri
 * CSCdi62735:  memory leak in LANE interfaces
 * Branch: California_branch
 * Packets could be lost when multicast VC is zero for LANE.
 *
 * Revision 3.7.20.18  1996/07/17  21:39:44  wilber
 * CSCdi62840:  line protocol goes down on FE when irb is configured
 * - Check the condition that span may be NULL when we create BVI
 * - Catch and receive keepalive we sent when IRB is enabled
 * Branch: California_branch
 *
 * Revision 3.7.20.17  1996/07/03  22:27:34  weismann
 * CSCdi56530:  IP Bridging frame-relay<-->ppp not working. Fix conversions
 * to slow paths and fix comments that were broken for frame relay and
 * atm process level transparent bridging.
 * Branch: California_branch
 *
 * Revision 3.7.20.16  1996/06/21  00:56:34  rbadri
 * CSCdi60545:  packets bridged indefinitely if 2 atm subif belong to
 *              same bri group
 * Branch: California_branch
 * While flooding make sure that the input swidb is a LANE interface so
 * that we
 * will not send out packets on the received interface.
 *
 * Revision 3.7.20.15  1996/06/18  19:14:48  rbadri
 * CSCdi60067:  IPX rip TR->TR through bridged ELAN works intermittently.
 * Branch: California_branch
 * Set pak->addr_start properly before using it to send LE_ARP requests.
 * Clean up a little on the LEC side.
 *
 * Revision 3.7.20.14  1996/05/24  01:22:37  wilber
 * CSCdi56961:  DeMorgans revenge
 * Fix the always true expression
 * Branch: California_branch
 *
 * Revision 3.7.20.13  1996/05/23  22:18:12  wilber
 * CSCdi58505:  802.10(SDE) encaped spanning tree doesnt work
 * Should parse sde header before Software Mac-address Filter lookup
 * Branch: California_branch
 *
 * Revision 3.7.20.12  1996/05/19  22:22:11  hou
 * CSCdi54416:  ISL bridging from 7000 to Cat5000 is broken
 * Branch: California_branch
 * - add code path to support packet being bridged from different medium
 *   interfaces to ISL vLAN interface on 7000.
 *
 * Revision 3.7.20.11  1996/05/19  21:14:08  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - add  bridging support for packet from different medium
 *   to ISL vlan fast ethernet on RSP.
 * - fix flooding path problem for packet coming from ISL vLAN interface.
 *   on RSP and 7000.
 *
 * Revision 3.7.20.10  1996/05/17  12:14:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.3  1996/05/06  00:02:25  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.6.2  1996/05/02  22:07:43  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.1  1996/04/03  22:00:33  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.20.9  1996/05/09  14:47:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.20.8  1996/05/08  00:23:58  speakman
 * CSCdi56968:  CMF accounting for flooded packets is incorrect
 * Branch: California_branch
 *  - account for IP multicast rx packets in the SA's bte
 *  - show CMF rx and tx packet counts per-interface
 *  - constrain the initial IGMP report on a given interface
 *    to router ports only to prevent suppression and sequential
 *    joining behaviour
 *  - show IGMP timers only when CMF debugging is enabled
 *  - clear the multicast-router-port state when the multicast-group
 *    state is cleared
 *  - flood (unconstrained) IP multicast data packets in the absence of
 *    any multicast-router ports
 *
 * Revision 3.7.20.7  1996/05/04  05:09:02  wilber
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
 * Revision 3.7.20.6  1996/04/23  05:57:38  weismann
 * CSCdi50570:  Process bridging atm-dxi arp replies fails encapsulation
 * Branch: California_branch
 * Fix bridge table entry proxy dxi address.
 *
 * Revision 3.7.20.5  1996/04/17  13:49:36  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.7.20.4  1996/04/04  03:41:39  hou
 * CSCdi52756:  4500/4700 fddi interface may fail to bridge unicast packets
 * Branch: California_branch
 * - tbridge_forward_inline()
 *   when mac entry was removed from bridge table, the same mac was removed
 *   from fddi cam for mci and rsp only, but not for les.
 *
 * Revision 3.7.20.3  1996/03/27  09:03:33  speakman
 * CSCdi51444:  Blocked bridge ports do not respond to ARP broadcasts.
 * Branch: California_branch
 * Move the check for non-blocking interfaces to FOLLOW the onerous
 * type-specific duties.  Add receive packet debugging.
 *
 * Revision 3.7.20.2  1996/03/23  22:49:36  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.7.20.1  1996/03/18  22:12:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.2  1996/03/13  02:03:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.6.1  1996/02/20  18:52:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/05  03:35:08  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.6  1996/01/23  08:54:51  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.5  1996/01/12  05:37:44  pitargue
 * CSCdi46803:  rfc 1483 to rfc 1483 tbridging is broken
 * make sure pak->atm_enctype is set for copied packets
 *
 * Revision 3.4  1995/12/17  06:12:37  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.3  1995/11/22  19:24:48  wilber
 * CSCdi42690:  No connectivity over trans-bridging for SOME stations
 * When circuit-group is configured, don't use the output permission bit
 * pattern calculated for the learned circuit form the bridge table since
 * DLD may selec different circuit for load-distribution. Check the
 * MAC-address filter specifically instead
 *
 * Revision 3.2  1995/11/17  18:45:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  21:31:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/21  19:15:03  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * recode congestion cases in tbridge_forward_inline and
 * tbridge_flood_inline.
 * These did not adequately handle MTU oversize issues and did not
 * correctly
 * count buffers on multi-VC cases like Frame relay.
 *
 * Revision 2.11  1995/10/09  23:17:26  speakman
 * CSCdi41850:  Loopbacks do not get bridged
 * Only filter loopbacks when the DA and SA match.  Bridge any others.
 *
 * Revision 2.10  1995/09/30  22:44:22  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Exchange the safety of data hiding for a 7% performance improvement
 * in the 64 byte pps no-drop rate.  Use an inline to do the SMF match
 * instead of a function call, and forgo the pointer safety checks in
 * ieee_equal in the bte locator to do a direct MAC address comparison.
 *
 * Revision 2.9  1995/09/19  20:51:39  pitargue
 * CSCdi40560:  giants seen while tbridging over atm (1483)
 * make sure packet offsets are correct when sending rfc 1483 encap'ed
 * packets to process level
 *
 * Revision 2.8  1995/08/18  19:14:44  psandova
 * CSCdi38988:  Fix for CSCdi38322 breaks ethernet-ppp-ethernet tbridging
 *
 * Fix error in previous commit.
 *
 * Revision 2.7  1995/08/17  07:13:09  wilber
 * CSCdi38595:  Lat Compression Breaks Transparent Bridging
 * Should initialize swidb->span_latsize to -1 before checking if it is
 * a LAT packet and if we could compress it
 *
 * Revision 2.6  1995/08/12  07:42:36  fred
 * CSCdi38525:  rewrite fair queue conversation discriminator using IEEE
 *              definitions
 *
 * Revision 2.5  1995/08/10  16:41:08  psandova
 * CSCdi38322:  SR/TLB fddi-tr improperly translates functional addresses
 *
 * Swap the low-end FDDI addresses in the slow switch inline function
 * instead of the low-end specific function to catch both the unicast
 * and multicast paths.  This is a hack and it will require a lot of
 * changes to do it right.
 *
 * Revision 2.4  1995/07/06  00:25:39  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.3  1995/06/23  04:12:44  speakman
 * CSCdi36302:  IGMP fails when bridging is configured
 * Identify IP multicasts for reception in tbridge_crb_inline
 * instead of tbridge_forward_inline.
 *
 * Revision 2.2  1995/06/08  20:52:46  wmay
 * CSCdi35517: Extra field in swidb-bridge_flood_first ==> tbridge_circuitQ
 *
 * Revision 2.1  1995/06/07  23:03:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TBRIDGE_INLINE_H__
#define __TBRIDGE_INLINE_H__



/*
 * stupid process level tbridging code.  it requires packets to be in
 * ethernet format.  make it so...
 */
static inline
boolean tbridge_convert_to_ether_inline (paktype *pak)
{
    ether_hdr *ether;
    ether_hdr *dest;
    ether_hdr *src;

    switch (tbridge_dlli.src_media_type) {

    case TBR_ETHER:
	/*
	 * datagramstart, mac_start and addr_start was all adjusted
	 * in the tbridge_xxx_classify routines.
	 */
        pak->datagramstart = pak->mac_start; 
        pak->flags |= PAK_TBRIDGE_IN;
	return(TRUE);
	
    case TBR_FDDI:
	/*
	 * adjust pak->datagramsize by one for fddi.
	 * basically, the FC byte is being replaced by the
	 * length field of ethernet. Remember, at this point
         * datagramsize is size of MAC frame INCLUDING FC byte
         * for FDDI, but excluding any transit encapsulations.
         * (ie for frame relay, atm, or smds). Note TR below
         * does NOT need to adjust datagramsize because essentially
         * the ACFC fields get overwritten as a ethernet packet
         * gets created, not affecting the current datagramsize.
         * Since FDDI only has a FC field preceding the DA, we need
         * to shift the DA an extra byte (compared to TR) and account
         * for it.
	 */
	pak->datagramsize += 1;

	/*
	 * fall through
	 */
    case TBR_TR:
    case TBR_CTR:
	/*
	 * for fddi and tr frames, slide the mac addrs 2 left for
	 * ethernet length field
	 */
	src = (ether_hdr *)pak->addr_start;
        pak->addr_start -= 2;
	dest = (ether_hdr *)pak->addr_start;

	/*
	 * slide the dst and src address to the left 2 bytes.
	 * address already canonical from tbridge_xxx_classify routine 
         */
        ieee_copy(src->daddr, dest->daddr);
        ieee_copy(src->saddr, dest->saddr);
	/*
	 * add length to make it a sap/snap mac header
	 */
	ether = (ether_hdr *)pak->addr_start;
	PUTSHORT(&ether->type_or_len,
		 (pak->datagramsize - ETHER_ARPA_ENCAPBYTES));

	/*
         * adjust all the fields to reflect new transmorgofied pak
	 */
	pak->datagramstart = pak->mac_start = pak->addr_start;
	pak->flags |= PAK_TBRIDGE_IN;
	return(TRUE);

    default:
	/*
	 * Unknown type.  return false
	 */
	return(FALSE);
    }

    return(FALSE);
}

/*
 * Initialize the tbridge_dlli structure for each packet received.
 * for all legacy lans, ETHER, TR, FDDI, SERIAL, this information
 * is pretty static and this is just reset tbridge_dlli w/ the
 * static values for each media.
 *
 * there is only one tbridge_dlli, so this means that the TB code
 * is single threaded.  if the code ever changes, then this must
 * change.
 */
static inline
void tbridge_dlli_init (hwidbtype *hwidb,
			    idbtype *swidb,
			    ushort offset)
{
    tbridge_dlli.src_media_type = swidb->tbridge_media;

    /*
     * da_offset and mac_offset are currently only used
     * by the high end.
     */
    tbridge_dlli.da_offset = hwidb->span_macoffset;
    tbridge_dlli.mac_offset = offset;

    tbridge_dlli.transit_encapsize = hwidb->span_encapsize;

    /*
     * these next guys have absolutely nothing to do w/
     * non circuit switched networks
     */
    tbridge_dlli.circuit_id = 0;
    tbridge_dlli.packet_pid = 0;

    /* Reinitialize the smdssrc_ptr */
    tbridge_dlli.smdssrc_ptr = NULL;
}

static inline
fr_pvc_info_type *tbridge_get_output_fr_pvc (hwidbtype *hwidb,
					     btetype *dst_bte)
{
    return(HWIDB_TO_FR_PVC(hwidb, dst_bte->bte_proxy_addr.fr_dlci));
}


static inline
boolean tbridge_novell_check_inline (idbtype *swidb, paktype *pak)

{
    snap_hdr *link = (snap_hdr *)pak->info_start;

    /*
     * Kludge to handle Novell ISO1 packets. Take care not to
     * bridge these packets if Novell routing is enabled !!!
     */
    if (link && (link->dsap == SAPSAP_NOVELL) &&
	(swidb->hwptr->status & IDB_ETHER) && (ROUTING_IF(swidb, LINK_NOVELL)))
	return(TRUE);
    return(FALSE);
}


static inline
void tbridge_rx_accounting_inline (hwidbtype *hwidb,
				   ushort bytes)

{
    hwidb->counters.inputs_fast[ACCT_PROTO_BRIDGE]++;
    hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_BRIDGE] += bytes;

    GET_TIMESTAMP(hwidb->lastinput);
}

static inline
void tbridge_tx_accounting_inline (hwidbtype *hwidb,
				   ushort bytes)

{
    hwidb->counters.outputs_fast[ACCT_PROTO_BRIDGE]++;
    hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_BRIDGE] += bytes;
}

/*
 * pak->datagramsize is the number of raw bytes received
 *                   usually supplied by the driver
 *
 * pak->datagramstart points to the beginning of the received packet
 *                    INCLUDING any transit encapsulation and preamble
 *                    to the DA
 *
 *  - the DA for ether packets
 *  - the FC byte for fddi packets
 *  - the AC byte for tring packets
 *  - the HDLC station address (0x0F00) for serial HDLC packets
 *
 * pak->mac_start points to the beginning of the MAC header
 *                of the packet to be bridged EXCLUDING any
 *                transit encapsulation
 *
 * For non-transit bridging (ETHER, TRING, FDDI), datagramstart == mac_start.
 * 
 * For transit bridging (PPP, HDLC-transit, FDDI-transit),
 * datagramstart points to the beginning of the transit encapsulation
 * while mac_start points to the DA of the encapsulated packet.
 *
 * pak->addr_start points to the DA (NOT the transit DA)
 *
 * pak->info_start points to the LLC header if one is present
 *
 * The code switch must indicate the type of packet memory of the input packet.
 */

static
enum TBRIDGE_FWD_RETVAL
tbridge_forward_inline (paktype *pak,
			const enum TBRIDGE_PAKMEM code_switch)

{
    idbtype *inputsw, *outputsw, *bvi;
    hwidbtype *inputhw, *outputhw;

    spantype *span;
    btetype *src_bte, *dst_bte;
    ieee_addrs *addr_start;
    paktype *half_fast_pak, *pakcopy;
    ulong dgsize_rx;
    ushort arpa_type;

    boolean on_complex;
    enum TBRIDGE_TLB_RETVAL action;

#if defined(TBRIDGE_MCI_CBUS) || defined(TBRIDGE_RSP_CBUS) || defined(TBRIDGE_PAS)
    ulong count ;
#endif

    /* CSCdj03137 */
    uchar canonical_hw_addr[IEEEBYTES];


    inputsw = pak->if_input;
    inputhw = inputsw->hwptr;

    addr_start = (ieee_addrs *)pak->addr_start;

    /*
     * If the input interface is a BVI, we don't have valid span_ptr
     * since the BVI is not a bridged interface. Don't use
     * inputsw->span_ptr below to reference bridge-group, use local
     * variable "span" instead. outputsw->span_ptr is OK since output
     * won't never be a BVI which is a routed interface.
     */
    if (is_bvi(inputhw)) {
	span = spanarray[inputhw->unit];
    } else {
	span = inputsw->span_ptr;
    }

    /*
     * We have some onerous type-specific duties to take care
     * of that could well be done in a general way by IP's and
     * drivers, but with which we're stuck.  Note that these
     * packets are NOT pointed at us; the SMF will have nabbed
     * those already.
     *
     * CSCdi51444 - take care of these duties BEFORE verifying
     * that the receiving interface is non-blocking.  So beware.
     * Only receive or flush packets in these checks.  Don't
     * forward or flood any until AFTER the check for circ_canlearn
     * below.
     */
    arpa_type = tbridge_get_arpa_type_inline(pak);

    if (arpa_type == TYPE_RFC826_ARP || arpa_type == TYPE_REVERSE_ARP) {
#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
	if (tbridge_debug_rcv_1) {
	    buginf("\nTB1: received [0x%x] packet %s from %s to %e\n",
		   arpa_type,
		   rxtype_string(pak->rxtype),
		   inputsw->namestring, addr_start->daddr);
	}
#endif	/* TBRIDGE_DEBUG */

        /*
	 * We want to catch all ARP packets which have not been
	 * received yet here.
	 *
         * Unicast ARP pointed at us is already received in
         * tbridge_irb_inline() when we lookup SMF.
         *
	 * Physical broadcast ARP when 
	 * - IRB is enabled
	 * - IRBing(both route and bridge) IP 
	 * has been received in tbridge_irb_inline().
	 *
         * We need to receiv two kinds of ARP packets here,
         * (1) Physical broadcast ARP when 
	 *   	- IRB is not enabled OR Not IRBing IP.
	 *	We don't need to overwrite the input interface,
	 *	just return.
         * (2) Unicast ARP not pointed at this box. 
	 *	IF:
	 * 	- IRB is enabled 
	 * 	- BVI(Bridge-group Virtual Interface) is configured
	 * 	- IRBing(both route and bridge) IP
	 * Mark in dlli_attributes to indicate that this is
	 * an irb packet because this is the ARP packet of the 
	 * protocol which we are both bridge and route;
	 * We want to record the original received bridged
	 * interface in pak and overwrite the input interface
	 * to the BVI after we learn the source mac and check filter
	 */

        /*
         * If the interface is not config as a bridge, then the span could be
         * NULL.  Then we can't use span->irb_bg_vidb.
         * An example of this is if the sub i/f is configured as bridge 
         * (i.e. vlan), and the main interface is not.  If we received an 
         * ARP packet is not a vlan, then the packet could from main interface
         * which do not have any bridge associated.
         */
        if (span) {
	    bvi = span->irb_bg_vidb;
	} else {
	    bvi = NULL;
	}

	if (global_irb_enable && bvi && ROUTING_IF(inputsw, LINK_IP) && 
	    BRIDGING_IF(inputsw, LINK_IP)) {
	    tbridge_dlli.dlli_attributes |= DLLI_IRBPKT;
	    /* Fall through to learn the src mac and check filter */
	} else {
	    return(TBRIDGE_FWD_RECEIVE);
	}
    } else if (arpa_type == TYPE_LOOP) {

	/* 
	 * Keepalives pointed at this interface CAN get to here 
	 * when, 
	 * - Bridging is not configured on the major interface 
	 *   (hwidb->firstsw) and since keepalive is hwidb based, 
	 *   we couldn't demultiplex it to the bridged subinterface, 
	 *   we failed to catch it when lookup first swidb's NULL 
	 *   smf.
	 * - IRB is enabled, which will bypass the station bit 
	 *   check on the high-end box because IRB needs to 
	 *   overwrite the input interface to the BVI for those 
	 *   routable packets, thus all packets will fall through
	 *   bridgding code.
	 * 
	 * So if this keepalive is pointed at this interface, 
	 * receive it. 
	 */

        /*
         * CSCdj03137 - If the packet arrived on a token-ring interface,
         * compare keepalive daddr to a canonically swapped form of the hw 
         * addr, since the packet's addresses have already been canonically 
         * swapped to ethernet format.
         */

        ieee_copy(inputhw->hardware, (uchar *)canonical_hw_addr);

        if (inputhw->status & IDB_TR) {
            ieee_swap((uchar *)canonical_hw_addr, (uchar *)canonical_hw_addr);
        }
        
        if (ieee_equal(addr_start->daddr, (uchar *)canonical_hw_addr)) {
            return(TBRIDGE_FWD_RECEIVE);
        }

	/*
	 * Keepalives NOT pointed at this interface's MAC address
	 * just clutter up the bridge table.  Get rid of them here.
	 * Note that keepalives are loopbacks in which the DA and
	 * SA are the same.  There are other applications of loopbacks
	 * with differing DA and SA that must be bridged (CSCdi41850).
	 */

	if ((GETLONG(&(addr_start->daddr[2])) ==
	     GETLONG(&(addr_start->saddr[2]))) &&
	    (GETSHORT(&(addr_start->daddr[0])) ==
	     GETSHORT(&(addr_start->saddr[0])))) {

	    goto flush_datagram;
	}
    }

    /*
     * CSCdi22308 - check that the input interface is enabled
     * for learning BEFORE de-referencing the span_ptr.  This
     * protects us from bridged packets that arrive AFTER the
     * the span_ptr has been zeroed as a part of disabling
     * bridging.  These packets are those committed to the host
     * queue by the SP while the RP is in the process of disabling
     * bridging.
     */

    /*
     * If this interface is not even learning,
     * receive only transit-bridged datagrams addressed to the
     * input interface.  Not sure what for.
     * Flush all others.
     */
    if (!inputsw->circ_canlearn && !is_bvi(inputhw)) {
	if ((inputhw->bridge_transit) &&
	    (ieee_equal(addr_start->daddr, &inputhw->hardware[0])))
	    return(TBRIDGE_FWD_RECEIVE);
	goto flush_datagram;
    }

    /*
     * Check the source type/lsap against input bridge filters
     */
    if ((inputsw->span_flag_in) &&
	(!tbridge_type_sap_filter(pak, inputsw->span_lsap_in,
				  inputsw->span_type_in)))
	goto flush_datagram;
    
    /*
     * If checking arbitrary field, do it now.
     */
    if (inputsw->span_pattern_in &&
	!xmacaccesscheck(inputhw, inputsw->span_pattern_in, addr_start->daddr,
			 (code_switch != TBRPAK_BFR)))
	goto flush_datagram;

    /*
     * With Integrated Routing and Briding, we may get packets from
     * routing fast-switched code which makes up the src MAC address of
     * Bridge-group Virtual interface, thus we don't want to learn these
     * packets coming from BVI.
     */
    if (is_bvi(inputhw)) {
	src_bte = NULL;
	goto skip_learning;
    }
    
    /*
     * Lookup source address in bridging cache
     */
    src_bte = locate_bte_inline(span, &addr_start->saddr[0]);

    if (src_bte) {

	/*
	 * Check for a source roaming to a new input interface.
	 */
	if ((!src_bte->bte_attributes) && (src_bte->bte_interface) &&
	    (src_bte->bte_interface != inputsw)) {

	    /*
	     * Remove any hardware address filters.
	     */
	    if (src_bte->bte_interface->tbridge_media == TBR_FDDI) {
		reg_invoke_media_delete_hardware_address_filter
		    (src_bte->bte_interface->hwptr,
		     (uchar *) &src_bte->bte_mac_addr.sword[0], inputhw);
	    }
	    
	    /*
	     * Recalculate the input permissions.
	     */
	    if (inputsw->span_address_in || inputsw->span_address_out)
		bridge_address_check(src_bte);

#ifdef TBRIDGE_MCI_CBUS
	    /*
	     * Rewrite the input interface and the permissions in
	     * any autonomous bridge cache entry.
	     */
	    if (src_bte->bte_interface->hwptr->cbus_bridge_enable &&
		src_bte->bte_bce_addr) {
		reg_invoke_cbus_bce_update_ifa(src_bte);
		reg_invoke_cbus_bce_update_permissions(src_bte);
	    }
#endif
	}

#if defined(TBRIDGE_MCI_CBUS) || defined(TBRIDGE_RSP_CBUS) || defined(TBRIDGE_PAS)
    /*
     * Populate FDDI CAM if source is chatty. The CAM ucode protects ourselves
     * against the outside world that may be throwing weird junk at us.
     * We don't ever insert our own address or a multicast/packet with RII in.
     *
     * Slave reads to CAM to find out whether an address exists or not are very
     * costly from performance point of view. For FDDI-PAs (mif68840), we need
     * to populate CAM the first three times and then every 16k packets if the
     * CAM is still not rejecting packets based on Src-Dest addr match. This is
     * not needed on FIP as we get an indication FDDIT_SA_FND. So we don't have
     * to go through this exercise on FIP.
     * We give three chances for an incoming packet go through populate cam pat
     * Note:  the arp packet does not go through this path.
     * Do every 16k afterwards to avoid the cam flush out by other entries.
     */

    if (inputsw->tbridge_media == TBR_FDDI) {
        count = src_bte->bte_rxcount + src_bte->bte_filtered_rxcount;
        if ((count & FDDI_CAM_REFRESH_CHECK) == FDDI_CAM_REFRESH_CHECK) {
            if ((count <= FDDI_CAM_INITIAL_REFRESH_PERIOD) ||
                ((count & FDDI_CAM_REFRESH_PERIOD) == FDDI_CAM_REFRESH_PERIOD))
            {
                /*
                 * Input is a FDDIT running transparently, the SA is not in the
                 * CAM, and we are hearing alot from this station.  Put them
                 * into the CAM.  Note this won't catch a station that is being
                 * talked to alot from some other stn but isn't talking much.
                 */
                    add_fddi_hardware_address_filter_inline(inputhw,
                             (uchar *)&src_bte->bte_mac_addr.sword[0]);
            }
        }
    }

#endif

	/*
	 * Now record the new input interface, only if it is not
	 * a LANE interface. tbridge_record_proxy_src_addr() will
	 * correctly learn for LANE.
	 * Change the state in src_bte to ready, for all interfaces other 
	 * than lane. It helps learning easy and maintain
	 * consistency in states.
	 */
	if (!is_atm_lane(inputsw)) {
	    src_bte->bte_interface = inputsw;
	    src_bte->bte_state = BTE_READY;
	}

	if (inputhw->bridge_transit) {
	    tbridge_record_proxy_src_addr(pak, src_bte, inputhw, FALSE);
	}

	if (src_bte->bte_action == BRIDGE_DISCARD)
	    goto flush_datagram;

        if ((inputsw->span_address_in) &&
	    (!(inputsw->number_bit_mask &
	       src_bte->bte_permit_in[inputsw->bit_mask_word])))
	    goto flush_datagram;

	/*
	 * Source bte located in the bridge table and passed all filters.
	 */
	goto skip_learning;
    }
    
    /*
     * Source bte NOT located in the bridge table.
     * Attempt to create one.
     */

    /*
     * Forward datagrams with multicast source address only if so configured
     * and we are not filtering the source MAC address.
     */
    if (ISMULTICAST(addr_start->saddr)) {
	if (!span->mcast_source) {
	    goto flush_datagram;
	} else {
	    if ((inputsw->span_address_in) &&
		!(access_check_mac(inputsw->span_address_in,
				   addr_start->saddr)))
		goto flush_datagram;
	}

	/*
	 * We have a datagram with a multicast source address,
	 * and we are configured to forward these.
	 */
	goto skip_learning;
    }
    
    /*
     * Source address is a unicast address to be learned.
     */

    src_bte = create_bte_inline(span, addr_start->saddr, inputsw);
    
    if (!src_bte) {

	/*
	 * Couldn't get a station block.  Check the source MAC address
	 * against input address filters
	 */
	span->learned_entry_discards++;
	if ((inputsw->span_address_in) &&
	    !(access_check_mac(inputsw->span_address_in,
			       addr_start->saddr)))
	    goto flush_datagram;

	/*
	 * skip learning
	 */
	
    } else {


	if (inputsw->span_address_in || inputsw->span_address_out)
	    bridge_address_check(src_bte);

	/*
	 * If source learning is disabled, dump this packet here
	 * rather than flooding it.
	 */
	if (span->acquire == FALSE) {
	    src_bte->bte_action = BRIDGE_DISCARD;
	    goto flush_datagram;
	}

	/*
	 * For LANE, if the BTE is created new, then 
	 * tbridge_record_proxy_src_addr() will learn properly.
	 */
	if (inputhw->bridge_transit)
	    tbridge_record_proxy_src_addr(pak, src_bte, inputhw, TRUE);

	if ((inputsw->span_address_in) &&
	    !(access_check_mac(inputsw->span_address_in,
			       addr_start->saddr)))
	    goto flush_datagram;
    }

    /*
     * Found  or created source address in the bridging database.
     */
    
 skip_learning:
    
    if (!inputsw->circ_canforward && !is_bvi(inputhw))
	goto flush_datagram;

    /*
     * This check creates new BCEs as well as re-creating them
     * if they have been lost due to cBus cache anihilation.
     */
    if (inputhw->cbus_bridge_enable && src_bte && !src_bte->bte_bce_addr)
	reg_invoke_srt_bce_add(span, src_bte, inputhw);

    /*
     * Process the IRB packet and return from here before the 
     * DA lookup.
     */
    if (tbridge_dlli.dlli_attributes & DLLI_IRBPKT) {
	if (src_bte)
	    src_bte->bte_rxcount++;
        if (tbridge_irb_pkt_processing_inline(pak)) {
            return (TBRIDGE_FWD_RECEIVE);
        } else {
            goto flush_datagram;
        }
    }

    outputhw = NULL;
    outputsw = NULL;
    dst_bte = NULL;

    /*
     * Lookup the destination.  Skip this for IP multicasts.
     */

    if (!(tbridge_dlli.dlli_attributes & DLLI_IPMCAST))
	dst_bte = locate_bte_inline(span, &addr_start->daddr[0]);

    if (dst_bte) {

	if (dst_bte->bte_action == BRIDGE_DISCARD)
	    goto flush_datagram;

	if (dst_bte->bte_action == BRIDGE_RECEIVE)
	    return(TBRIDGE_FWD_RECEIVE);

	/*
	 * For LANE interfaces, we don't want to flood/forward, if the
	 * BTE says the state is flushing. If a generic use of bte_state is
         * needed, then BTE_LANE_FLUSH should not overlap with others.
	 */
	if (dst_bte->bte_state == BTE_LANE_FLUSH) {
            goto flush_datagram;
	} 

	outputsw = dst_bte->bte_interface;

	if (outputsw) {
	    /*
	     * If this interface is in a circuit group,
	     * determine the output interface.
	     */
	    if (outputsw->circ_group) {
		int ccg = outputsw->circ_group;
		outputsw = dld_get_output_idb(span, ccg, 
					      &addr_start->saddr[0],
					      &addr_start->daddr[0]);
		if (!outputsw) {	
		    if (TIMER_RUNNING(span->cct_groups[ccg].ccg_pause_timer) &&
			AWAKE(span->cct_groups[ccg].ccg_pause_timer))
			ccg_pause_timer_expiry(span, ccg);
		    goto flush_datagram;
		}
	    }
	    outputhw = outputsw->hwptr;

	}

    }

    /*
     * If output interface unknown, flood this datagram.
     */
    if (!outputsw ) {

	if (src_bte)
	    src_bte->bte_rxcount++;

	/*
	 * If we are here to flood this packet because it is a broadcast
	 * or multicast count it as an input broadcast.
	 */
	if (ISMULTICAST(addr_start->daddr))
	    inputhw->counters.input_broad++;

	if ((span->port_count == 1) && !is_bvi(inputhw))
	    /*
	     * If the input interface is BVI, we could have only
	     *  one bridged interface.
	     */
            goto flush_datagram;

	return(TBRIDGE_FWD_FLOOD);
    }
    

    /*
     * If going out the same interface -- flush datagram.
     *
     * dst_bte is guaranteed to be valid since outputsw is derived
     * from it. src_bte could be NULL when we are configured to
     * forward datagrams with multicast source address.
     */
    if ((outputsw->if_number == inputsw->if_number) ||
        (inputsw->circ_group &&
	 (outputsw->circ_group == inputsw->circ_group))) {
	dst_bte->bte_filtered_txcount++;
	if (src_bte)
	    src_bte->bte_filtered_rxcount++;
        goto flush_datagram;
    }

    /*
     * If forwarding disallowed -- flush datagram.
     */
    if (!outputsw->circ_canforward)
	goto flush_datagram;

    if (outputsw->span_address_out) {
        /*
         * The output circuit selected by DLD may be different from
         * the circuit we learned on bridge table for this MAC address.
         * If circuit-group is configured, don't check the output
         * permission calculated for the learned circuit.
         */
        if (outputsw->circ_group) {
            if (!access_check_mac(outputsw->span_address_out,
                                  addr_start->daddr))
                goto flush_datagram;
        } else {
            if (!(outputsw->number_bit_mask &
                  dst_bte->bte_permit_out[outputsw->bit_mask_word]))
                goto flush_datagram;
        }
    }

    /*
     * If checking type or LSAP on output -- do so now.
     */
    if ((outputsw->span_flag_out) &&
	(!tbridge_type_sap_filter(pak, outputsw->span_lsap_out,
				  outputsw->span_type_out)))
	goto flush_datagram;

    /*
     * If checking arbitrary pattern on output -- do so now.
     */
    if ((outputsw->span_pattern_out) &&
	(!xmacaccesscheck(inputhw, outputsw->span_pattern_out, 
			  addr_start->daddr, (code_switch != TBRPAK_BFR))))
	goto flush_datagram;

    /*
     * If LAT compression enabled -- do so now.
     */
    if ((ushort)outputsw->span_latcompress) {
	outputsw->span_latsize = -1;
	if (tbridge_get_arpa_type_inline(pak) == TYPE_LAT) {
#ifdef TBRIDGE_MCI_CBUS
	    if (TBRPAK_MCI == code_switch)
		tbridge_mci_read_lat_values_inline(inputhw, outputsw, pak);
#endif
	    tbridge_check_lat_compression(outputsw, pak);
	}
    }

    
    if (outputhw->span_bridge_process_force) {

	pak->if_output = NULL;
	pak->bridgeptr = NULL;

	/*
	 * There should be no doubt that we have both a src and dst
	 * bte, as well as input and output interfaces by this point.
	 */
	if (src_bte)
	    src_bte->bte_rxcount++;

	if (dst_bte) {
	    dst_bte->bte_txcount++;

	    pak->if_output = outputsw;

	    if (outputhw->x25_bridge)
		pak->bridgeptr = dst_bte->bte_proxy_addr.x25map_ptr;

	    if (is_frame_relay(outputhw))
		if (outputhw->frame_relay_stuff->fr_bridge)
		    pak->bridgeptr = &dst_bte->bte_proxy_addr.fr_dlci;

            if (is_atm_dxi(outputhw))
                pak->bridgeptr = &dst_bte->bte_proxy_addr.fr_dlci;

	    if (is_fddi_br_encap(outputhw))
		pak->bridgeptr = &dst_bte->bte_proxy_addr.proxy_mac;

	    if (outputhw->smds_bridge)
		pak->bridgeptr = (void *)dst_bte->bte_mac_addr.lword;
	}

	/* Account for received packets that were process switched. 
         * CSCdi92583- Do not account for BVI inputs to bridging
         */
        if (!is_bvi(inputhw)) {	
            tbridge_rx_accounting_inline(inputhw, 
                                         tbridge_dlli.orig_datagramsize);
        }        
        
	return(TBRIDGE_FWD_SLOWSWITCH);
    }
    
    /*
     * For cbus-bridged interfaces, add a BCE corresponding to this BTE
     * if one does not exist.  The BTE would have been created with one
     * but it may have been lost due to cbus cache anihilation.  This
     * check restores it.  Doing this for the destination MAC is just
     * an optimization that assists the restoration of AB to destinations
     * before they are heard from.
     */
    if (outputhw->cbus_bridge_enable && !dst_bte->bte_bce_addr)
	reg_invoke_srt_bce_add(span, dst_bte, outputhw);

    /*
     * Check for packets that need to be forwarded to an
     * interface on a different MCI/cBus complex than the
     * input interface.
     */

    on_complex = TRUE;
    half_fast_pak = NULL;

#ifdef TBRIDGE_MCI_CBUS
    if (TBRPAK_MCI == code_switch) {
	if (inputhw->mci_regptr != outputhw->mci_regptr)
	    on_complex = FALSE;
	}

#endif
    

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_2)
	buginf("\nTB2: forwarding %s from %s to %s"
	       "\n     %e to %e",
	       rxtype_string(pak->rxtype), pak->if_input->namestring,
	       outputsw->namestring,
	       addr_start->saddr, addr_start->daddr);
#endif /* TBRIDGE_DEBUG */

    /*
     * Save this for accounting purposes since it will be overwritten
     * in the translation process.  Don't do rx accounting until we
     * have returned successfully from translation.  This lets us
     * keep filtered, forwarded, and flooded receive counts in
     * agreement.
     */
    dgsize_rx = pak->datagramsize;
    pak->acct_proto = ACCT_PROTO_BRIDGE;
    pak->if_output = outputsw;

    action = TBRIDGE_TLB_UNSUPPORTED;
    switch (outputsw->tbridge_media) {

    case TBR_ETHER:			/* forward to ether */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-ether */
	case TBRPAK_BFR:		/* le-xxx forward to le-ether */
	    action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-ether */
	    if (on_complex) {
		action = tbridge_to_ether_inline(outputsw, pak, TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_ether_inline(outputsw, pak, TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
	    		action = tbridge_to_ether_inline(outputsw,
						half_fast_pak, TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
      case TBRPAK_VBR:                  /* vbridge forward to he-ether */
          action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
          if (action == TBRIDGE_TLB_DONE)
              tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
          break;
#else
	default:
	    break;
#endif
	}
	break;
	
    case TBR_FDDI:			/* forward to fddi */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-fddi */
	case TBRPAK_BFR:		/* le-xxx forward to le-fddi */
	    action = tbridge_to_fddi_inline(outputsw, pak, TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-fddi */
	    if (on_complex) {
		action = tbridge_to_fddi_inline(outputsw, pak, TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_fddi_inline(outputsw, pak, TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize |
				       MCI_TX_ODDALIGN);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
	    		action = tbridge_to_fddi_inline(outputsw,
					half_fast_pak, TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
      case TBRPAK_VBR:        /* vbridge forward to he-fddi */
          action = tbridge_to_fddi_inline(outputsw, pak, TLB_BFR_TO_MCI);
          if (action == TBRIDGE_TLB_DONE)
              tbridge_mci_tx1_inline(outputhw,
                                     pak->datagramsize | MCI_TX_ODDALIGN);
          break;
#else
	default:
	    break;
#endif
	}
        break;

    case TBR_CTR:			/* forward to ctr */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to ctr */
	    action = tbridge_to_tring_inline(outputsw, pak, TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
	case TBRPAK_BFR:		/* le-xxx forward to ctr */
	    action = TBRIDGE_TLB_UNSUPPORTED;
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to ctr */
	    if (on_complex) {
		action = tbridge_to_tring_inline(outputsw, pak, TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_tring_inline(outputsw, pak, TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
	    		action = tbridge_to_tring_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
      case TBRPAK_VBR:        /* vbridge forward to ctr */
          action = tbridge_to_tring_inline(outputsw, pak, TLB_BFR_TO_MCI);
          if (action == TBRIDGE_TLB_DONE)
              tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
          break;
#else
	default:
	    break;
#endif
	}
	break;
	
    case TBR_TR:			/* forward to ltr */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to ltr */
	    action = TBRIDGE_TLB_UNSUPPORTED;
	    break;
	case TBRPAK_BFR:		/* le-xxx forward to ltr */
	    action = tbridge_to_tring_inline(outputsw, pak, TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to ltr */
        case TBRPAK_VBR:                /* vbridge mci forward to ltr */
	    action = TBRIDGE_TLB_UNSUPPORTED;
	    break;
#else
	default:
	    break;
#endif
	}
	break;
	
    case TBR_FDDI_TRANSIT:		/* forward to fddi-t */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-fddi-t */
	case TBRPAK_BFR:		/* le-xxx forward to le-fddi-t */
	    action = tbridge_to_fddi_transit_inline(outputsw, pak,
						    dst_bte->bte_proxy_addr.proxy_mac,
						    TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-fddi-t */
	    if (on_complex) {
		action = tbridge_to_fddi_transit_inline(outputsw, pak,
							dst_bte->bte_proxy_addr.proxy_mac,
							TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_fddi_transit_inline(outputsw, pak,
							dst_bte->bte_proxy_addr.proxy_mac,
							TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize |
				       MCI_TX_ODDALIGN);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_fddi_transit_inline(outputsw, half_fast_pak,
								dst_bte->bte_proxy_addr.proxy_mac,
								TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
        case TBRPAK_VBR:        /* vbridge forward to he-fddi-t */
            action = tbridge_to_fddi_transit_inline(outputsw, pak,
                                            dst_bte->bte_proxy_addr.proxy_mac,
                                            TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE)
                tbridge_mci_tx1_inline(outputhw,
                                       pak->datagramsize | MCI_TX_ODDALIGN);
            break;
#else
	default:
	    break;
#endif
	}
        break;

    case TBR_ATM_TRANSIT: {
	/*
	 * get the VC number associated w/ this DA.  if the VC returned
	 * is zero, then there no VC w/ associated w/ this DA.
	 */
	if (dst_bte->bte_proxy_addr.vc_num) {
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx forward to le-atm */
	    case TBRPAK_BFR:		/* le-xxx forward to le-atm */
		pak->atm_enctype = ATM_DM_PKT_AAL5;
		action = tbridge_to_atm_transit_inline(outputsw, pak,
						       dst_bte->bte_proxy_addr.vc_num,
						       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx forward to he-atm */
		if (on_complex) {
		    action = tbridge_to_atm_transit_inline(outputsw, pak,
							   dst_bte->bte_proxy_addr.vc_num,
							   TLB_WITHIN_MCI);
		} else {
		    action = tbridge_to_atm_transit_inline(outputsw, pak,
							   dst_bte->bte_proxy_addr.vc_num,
							   TLB_MCI_TO_MCI);
		}
		if (action == TBRIDGE_TLB_DONE)
		    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		else if (action == TBRIDGE_TLB_NO_TXBUF &&
			 outputhw->priority_list) {
		    half_fast_pak = getbuffer(pak->datagramsize);
		    if (half_fast_pak) {
			half_fast_pak->if_output = outputsw;
			if (!tbridge_mci_copy_rx_pak_inline
			    (pak, half_fast_pak)) {
			    datagram_done(half_fast_pak);
			} else {
			    half_fast_pak->atm_enctype = ATM_DM_PKT_AAL5;
			    action = tbridge_to_atm_transit_inline(outputsw, half_fast_pak,
								   dst_bte->bte_proxy_addr.vc_num,
								   TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}

		break;
            case TBRPAK_VBR:                /* vbridge forward to he-atm */
                action = tbridge_to_atm_transit_inline(outputsw,
                                                       pak, dst_bte->bte_proxy_addr.vc_num,
                                                       TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE)
                    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
                break;
#else
	    default:
		break;
#endif
	    }
	} else {
	    /*
	     * this should never happen.  why?  well because if we learn
	     * the DA, then we should of learned the VC number of the
	     * packet that came in on the ATM interface.  but CYA and
	     * put this here.
	     * so output an error message and flush this puppy...
	     */
	    errmsg(TBRIDGE_ERR_NOVCDROP);
	    action = TBRIDGE_TLB_UNSUPPORTED;
	}
    }
	break;
    case TBR_LANE_ETHER: {
	/*
	 * get the VC number associated w/ this DA.  if the VC returned
	 * is zero, then there is no  VC w/ associated w/ this DA.
	 */
        ushort vc;
	vc = dst_bte->bte_proxy_addr.vc_num;

	if (vc) {

	    switch (code_switch) {
	    case TBRPAK_RSP:	/* rsp-xxx forward to le-lane-ether */
	    case TBRPAK_BFR:	/* le-xxx forward to le-lane-ether */
		pak->atm_enctype =  ATM_DM_PKT_AAL5;
		action = tbridge_to_lane_ether_inline(outputsw, 
						      pak,
						      vc,
						      TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    (*outputhw->fastsend)(outputhw, pak);
		}
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:	/* he-xxx forward to lane-ether */
                if (on_complex) {
		    action = tbridge_to_lane_ether_inline(outputsw, 
							  pak,
							  vc,
							  TLB_WITHIN_MCI);
		} else {
		    action = tbridge_to_lane_ether_inline(outputsw,
							  pak,
							  vc,
							  TLB_MCI_TO_MCI);
		}
		if (action == TBRIDGE_TLB_DONE)
		    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		else if (action == TBRIDGE_TLB_NO_TXBUF &&
			 outputhw->priority_list) {
		    half_fast_pak = getbuffer(pak->datagramsize);
		    if (half_fast_pak) {
			half_fast_pak->if_output = outputsw;
			if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
			    datagram_done(half_fast_pak);
			} else {
			    action = tbridge_to_lane_ether_inline(outputsw,
				           half_fast_pak, vc, TLB_BFR_TO_MCI);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}

		break;
            case TBRPAK_VBR:        /* vbridge forward to he-serial-t */
                action = tbridge_to_lane_ether_inline(outputsw, 
                                                      pak,
						      vc,
                                                      TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE)
                    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
                break;
#else
	    default:
		break;
#endif
	    }
	    if (dst_bte->bte_state == BTE_LANE_FLOOD) {
		/*
		 * If the BTE is in the Flooding state, then send out
		 * an LE_ARP indication
		 */
		reg_invoke_lec_bridge_enqueue(pak->addr_start,
					      outputsw);

	    }
	} else {
	    /*
             * This shouldn't happen for 2 reasons:
             * 1. If the VC was taken down, then BTE would have disappeared.
             * 2. If swidb !=0, VC better be Non-Zero as well. 
	     */
	    errmsg(TBRIDGE_ERR_NOVCDROP);
	    action = TBRIDGE_TLB_UNSUPPORTED;
	}
    }
	break;

    case TBR_HDLC_TRANSIT:		/* forward to serial-t */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-serial-t */
	case TBRPAK_BFR:		/* le-xxx forward to le-serial-t */
	    action = tbridge_to_hdlc_transit_inline(outputsw, pak,
						    TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-serial-t */
	    if (on_complex) {
		action = tbridge_to_hdlc_transit_inline(outputsw, pak,
							TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_hdlc_transit_inline(outputsw, pak,
							TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_hdlc_transit_inline(outputsw,
						half_fast_pak, TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
      case TBRPAK_VBR:        /* vbridge forward to he-serial-t */
          action = tbridge_to_hdlc_transit_inline(outputsw, pak,
                                                  TLB_BFR_TO_MCI);
          if (action == TBRIDGE_TLB_DONE)
              tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    break;
#else
	default:
	    break;
#endif
	}
        break;
    case TBR_FRAME_RELAY_TRANSIT: {
	fr_pvc_info_type *fr_pvc;

	/*
	 * get the PVC structure associated w/ this DA.  if it is NULL
	 * then there no PVC w/ associated w/ this DA.
	 */
	fr_pvc = tbridge_get_output_fr_pvc(outputhw, dst_bte);
	if (fr_pvc) {
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx forward to le-atm */
	    case TBRPAK_BFR:		/* le-xxx forward to le-atm */
		action = tbridge_to_frame_relay_inline(outputsw, outputhw,
						       pak, fr_pvc,
						       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx forward to he-atm */
		if (on_complex) {
		    action = tbridge_to_frame_relay_inline(outputsw,
							   outputhw, pak,
							   fr_pvc,
							   TLB_WITHIN_MCI);
		} else {
		    action = tbridge_to_frame_relay_inline(outputsw,
							   outputhw, pak,
							   fr_pvc,
							   TLB_MCI_TO_MCI);
		}
		if (action == TBRIDGE_TLB_DONE)
		    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		else if (action == TBRIDGE_TLB_NO_TXBUF &&
			 outputhw->priority_list) {
		    half_fast_pak = getbuffer(pak->datagramsize);
		    if (half_fast_pak) {
			half_fast_pak->if_output = outputsw;
			if (!tbridge_mci_copy_rx_pak_inline
			    (pak, half_fast_pak)) {
			    datagram_done(half_fast_pak);
			} else {
			    action = tbridge_to_frame_relay_inline(outputsw, outputhw,
								   half_fast_pak, fr_pvc,
								   TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}

		break;
          case TBRPAK_VBR:                /* vbridge forward to he-atm */
              action = tbridge_to_frame_relay_inline(outputsw, outputhw,
                                                     pak,
                                                     fr_pvc,
                                                     TLB_BFR_TO_MCI);
              if (action == TBRIDGE_TLB_DONE)
                  tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
              break;
#else
	    default:
		break;
#endif
	    }
	} else
	    /*
	     * this should never happen.  why?  well because if we learn
	     * the DA, then we should have learned the PVC number of the
	     * packet that came in.  but CYA and put this here.
	     */
	    return(TBRIDGE_FWD_FLOOD);

    }
	break;
    case TBR_SMDS_TRANSIT:
	switch (code_switch) {
	case TBRPAK_RSP:                /* rsp-xxx forward to le-smds */
	case TBRPAK_BFR:                /* le-xxx forward to le-smds */
	    action = tbridge_to_smds_inline(outputsw, outputhw,pak,
					    dst_bte->bte_proxy_addr.proxy_mac,
					    TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:                /* he-xxx forward to he-smds */
	    if (on_complex) {
		action =
		    tbridge_to_smds_inline(outputsw, outputhw, pak,
					   dst_bte->bte_proxy_addr.proxy_mac,
					   TLB_WITHIN_MCI);
	    } else {
		action =
		    tbridge_to_smds_inline(outputsw, outputhw, pak,
					   dst_bte->bte_proxy_addr.proxy_mac,
					   TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
		     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action =
			    tbridge_to_smds_inline(outputsw, outputhw,
					   half_fast_pak,
					   dst_bte->bte_proxy_addr.proxy_mac,
					   TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE)
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
	case TBRPAK_VBR:                /* vbridge forward to he-smds */
	    action = tbridge_to_smds_inline(outputsw, outputhw, pak,
				       dst_bte->bte_proxy_addr.proxy_mac,
				       TLB_BFR_TO_MCI);
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
#else
	default:
	    break;
#endif
	}
	break;

    case TBR_ETHER_SDE:         /* forward to ether-sde */
        switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-ether */
        case TBRPAK_BFR:		/* le-xxx forward to le-ether */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action == TBRIDGE_TLB_DONE) {
		action = tbridge_to_ether_sde_inline(outputsw, pak,
						     TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
	    }
            break;
#ifdef TBRIDGE_MCI_CBUS
        case TBRPAK_MCI:		/* he-xxx forward to he-ether */
            if (on_complex) {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_WITHIN_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_ether_sde_inline(outputsw, pak,
							 TLB_WITHIN_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
		}
            } else {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_MCI_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_ether_sde_inline(outputsw, pak,
							 TLB_MCI_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		}
            }
	    if (action == TBRIDGE_TLB_NO_TXBUF &&
		outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline
	    		(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_llc_inline(outputsw, half_fast_pak,
						       TLB_BFR_TO_BFR);
			if (action != TBRIDGE_TLB_DONE)
			    datagram_done(half_fast_pak);
			else {
			    action = tbridge_to_ether_sde_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
			    if (action != TBRIDGE_TLB_DONE)
				datagram_done(half_fast_pak);
			    else
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}
	    }
            break;
        case TBRPAK_VBR:        /* vbridge forward to he-ether */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE) {
              action = tbridge_to_ether_sde_inline(outputsw, pak,
                                                   TLB_BFR_TO_MCI);
              if (action == TBRIDGE_TLB_DONE)
                  tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
            }
            break;

#else
	default:
	    break;
#endif
        }
        break;
	
    case TBR_FDDI_SDE:			/* forward to  fddi-sde */
        switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-fddi */
        case TBRPAK_BFR:		/* le-xxx forward to le-fddi */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action == TBRIDGE_TLB_DONE) {
		action = tbridge_to_fddi_sde_inline(outputsw, pak,
						    TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
	    }
            break;
#ifdef TBRIDGE_MCI_CBUS
        case TBRPAK_MCI:		/* he-xxx forward to he-fddi */
            if (on_complex) {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_WITHIN_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_fddi_sde_inline(outputsw, pak,
							TLB_WITHIN_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(inputhw, pak->datagramsize |
					       MCI_TX_ODDALIGN);
		}
            } else {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_MCI_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_fddi_sde_inline(outputsw, pak,
							TLB_MCI_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(outputhw, pak->datagramsize |
					       MCI_TX_ODDALIGN);
		}
            }
	    if (action == TBRIDGE_TLB_NO_TXBUF &&
		outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline
	    		(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_llc_inline(outputsw, half_fast_pak,
						       TLB_BFR_TO_BFR);
			if (action != TBRIDGE_TLB_DONE)
			    datagram_done(half_fast_pak);
			else {
			    action = tbridge_to_fddi_sde_inline(outputsw,
						half_fast_pak, TLB_BFR_TO_BFR);
			    if (action != TBRIDGE_TLB_DONE)
				datagram_done(half_fast_pak);
			    else
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}
	    }
            break;
        case TBRPAK_VBR:        /* vbridge forward to he-fddi */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE) {
                action = tbridge_to_fddi_sde_inline(outputsw, pak,
                                                    TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE)
                    tbridge_mci_tx1_inline(outputhw,
                                           pak->datagramsize |
                                           MCI_TX_ODDALIGN);
            }
            break;
#else
	default:
	    break;
#endif
        }
        break;
	
    case TBR_CTR_SDE:			/* forward to ctr-sde */
        switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to ctr */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action == TBRIDGE_TLB_DONE) {
		action = tbridge_to_tring_sde_inline(outputsw, pak,
						     TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
	    }
            break;
        case TBRPAK_BFR:		/* le-xxx forward to ctr */
            action = TBRIDGE_TLB_UNSUPPORTED;
            break;
#ifdef TBRIDGE_MCI_CBUS
        case TBRPAK_MCI:		/* he-xxx forward to ctr */
            if (on_complex) {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_WITHIN_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_tring_sde_inline(outputsw, pak,
							 TLB_WITHIN_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
		}
            } else {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_MCI_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_tring_sde_inline(outputsw, pak,
							 TLB_MCI_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		}
            }
	    if (action == TBRIDGE_TLB_NO_TXBUF &&
		outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_llc_inline(outputsw, half_fast_pak,
						       TLB_BFR_TO_BFR);
			if (action != TBRIDGE_TLB_DONE)
			    datagram_done(half_fast_pak);
			else {
			    action = tbridge_to_tring_sde_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
			    if (action != TBRIDGE_TLB_DONE)
				datagram_done(half_fast_pak);
			    else
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}
	    }
            break;
        case TBRPAK_VBR:        /* vbridge forward to ctr */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE) {
                action = tbridge_to_tring_sde_inline(outputsw, pak,
                                                     TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE)
                    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
            }
            break;
#else
	default:
	    break;
#endif
        }
        break;
		
     case TBR_TR_SDE:           /* forward to tr-sde */
         switch (code_switch) {
           case TBRPAK_RSP:        /* rsp-xxx forward to tr */
             action = TBRIDGE_TLB_UNSUPPORTED;
             break;
           case TBRPAK_BFR:        /* le-xxx forward to tr */
             action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_BFR);
             if (action == TBRIDGE_TLB_DONE) {
                 action = tbridge_to_tring_sde_inline(outputsw, pak,
                                                      TLB_BFR_TO_BFR);
                 if (action == TBRIDGE_TLB_DONE)
                     (*outputhw->fastsend)(outputhw, pak);
             }

             break;
           default:
             break;
         }
         break;

    case TBR_HDLC_TRANSIT_SDE:		/* forward to serial-t-sde */
        switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-serial-t */
        case TBRPAK_BFR:		/* le-xxx forward to le-serial-t */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action == TBRIDGE_TLB_DONE) {
		action = tbridge_to_hdlc_transit_sde_inline(outputsw, pak,
							    TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pak);
	    }
            break;
#ifdef TBRIDGE_MCI_CBUS
        case TBRPAK_MCI:		/* he-xxx forward to he-serial-t */
            if (on_complex) {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_WITHIN_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_hdlc_transit_sde_inline(outputsw, pak,
								TLB_WITHIN_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
		}
            } else {
                action = tbridge_to_llc_inline(outputsw, pak, TLB_MCI_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_hdlc_transit_sde_inline(outputsw, pak,
								TLB_MCI_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
		}
            }
	    if (action == TBRIDGE_TLB_NO_TXBUF &&
		outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_llc_inline(outputsw, half_fast_pak,
						       TLB_BFR_TO_BFR);
			if (action != TBRIDGE_TLB_DONE)
			    datagram_done(half_fast_pak);
			else {
			    action = tbridge_to_hdlc_transit_sde_inline(outputsw,
						half_fast_pak, TLB_BFR_TO_BFR);
			    if (action != TBRIDGE_TLB_DONE)
				datagram_done(half_fast_pak);
			    else
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
			}
		    }
		}
	    }
            break;
        case TBRPAK_VBR:        /* vbridge forward to he-serial-t */
            action = tbridge_to_llc_inline(outputsw, pak, TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE) {
                action = tbridge_to_hdlc_transit_sde_inline(outputsw, pak,
                                                            TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE)
                    tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
              }
              break;
#else
	default:
	    break;
#endif
        }
        break;
	
    case TBR_LEX_TRANSIT:		/* forward to lex-t */
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-lex-t */
	case TBRPAK_BFR:		/* le-xxx forward to le-lex-t */
	    action = tbridge_to_lex_transit_inline(outputsw, pak,
						   TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-lex-t */
	    if (on_complex) {
		action = tbridge_to_lex_transit_inline(outputsw, pak,
						       TLB_WITHIN_MCI);
	    } else {
		action = tbridge_to_lex_transit_inline(outputsw, pak,
						       TLB_MCI_TO_MCI);
	    }
	    if (action == TBRIDGE_TLB_DONE)
		tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
	    	    } else {
			action = tbridge_to_lex_transit_inline(outputsw,
					      half_fast_pak, TLB_BFR_TO_BFR);
	    		if (action == TBRIDGE_TLB_DONE)
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }
	    break;
        case TBRPAK_VBR:        /* vbridge forward to he-lex-t */
            action = tbridge_to_lex_transit_inline(outputsw, pak,
                                                   TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE)
                tbridge_mci_tx1_inline(outputhw, pak->datagramsize);
            break;
#else
	default:
	    break;
#endif
	}
        break;

    /*
     * The following case is bridging out to an Inter Switch Link (ISL) 
     * encapsulating vLAN interface. This is only supported on Fast
     * Ethernet (C7xxx/C4xxx) interfaces.
     */
     
    case TBR_ETHER_ISL_VLAN:         /* Forward to Fast Ethernet ISL encap */
                                     /* Just prepend 26 byte ISL header here */
        switch (code_switch) {

            case TBRPAK_RSP:         /* rsp-xxx forward to */
	                             /* rsp-fast-ether-isl */
            case TBRPAK_BFR:         /* les-xxx forward to */
                                      /* les-fast-ether-isl */

	        /*
		 * Call this function to adjust the packet's encapsulation
		 * when translating beween different media, eg fddi->ether,
		 * token->ether.
		 */
	        action = tbridge_to_ether_inline(outputsw, pak,
						 TLB_BFR_TO_BFR);

		if (action == TBRIDGE_TLB_DONE) {
  		    tbridge_prepend_isl_header(outputsw, pak);
		    (*outputhw->fastsend)(outputhw, pak);
		}

                break;

#ifdef TBRIDGE_MCI_CBUS

            case TBRPAK_MCI:        /* he-xxx forward to he-fast-ether-isl */
	      
		if (on_complex) {

		    action = tbridge_to_ether_inline(outputsw, pak,
						     TLB_WITHIN_MCI);
		    if (action == TBRIDGE_TLB_DONE) {

			pak->encsize -= ISL_INTERIOR_PACKET;
			tbridge_mci_tx1_select_offset_inline(inputhw,
							     pak->encsize); 
		        tbridge_prepend_isl_header(outputsw, pak);
			tbridge_mci_tx1_inline(inputhw,
					       pak->datagramsize);
		    }
		} else {
		    action = tbridge_to_ether_inline(outputsw, pak,
						     TLB_MCI_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE) {

			pak->encsize = TBRIDGE_MCI_TX1_OFFSET;
			pak->encsize -= ISL_INTERIOR_PACKET;
			tbridge_mci_tx1_select_offset_inline(outputhw,
							     pak->encsize);
			tbridge_prepend_isl_header(outputsw, pak);
			tbridge_mci_tx1_inline(outputhw,
                                               pak->datagramsize);
		    }
		    
		}
                break;

	    /* Compiler needs to see all enumerations */

            case TBRPAK_VBR:       /* vbridge forward to he-fast-ether-isl */
                action = tbridge_prepend_isl_header(outputsw, pak);
                tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
                break;
#else
            default:
                break;
#endif
        }
        break;

#ifdef notdef
    case TBR_CIP_VLAN:
	switch (code_switch) {
	case TBRPAK_RSP:		/* rsp-xxx forward to rsp-ether */
	    action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_BFR);
	    if (action == TBRIDGE_TLB_DONE)
		(*outputhw->fastsend)(outputhw, pak);
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:		/* he-xxx forward to he-ether */
	    /* always on complex */
	    action = tbridge_to_ether_inline(outputsw, pak,
					     TLB_WITHIN_MCI);
	    if (action == TBRIDGE_TLB_DONE) 
		tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
	    else if (action == TBRIDGE_TLB_NO_TXBUF &&
	    	     outputhw->priority_list) {
		half_fast_pak = getbuffer(pak->datagramsize);
		if (half_fast_pak) {
		    half_fast_pak->if_output = outputsw;
	    	    if (!tbridge_mci_copy_rx_pak_inline(pak, half_fast_pak)) {
	    		datagram_done(half_fast_pak);
		    } else {
			action = tbridge_to_ether_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
	    		    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
	    	    }
		}
	    }

	    break;
        case TBRPAK_VBR:                /* vbridge forward to he-ether */
            action = tbridge_to_ether_inline(outputsw, pak, TLB_BFR_TO_MCI);
            if (action == TBRIDGE_TLB_DONE)
                tbridge_mci_tx1_inline(inputhw, pak->datagramsize);
            break;
#else
	default:
	    break;
#endif
	}
#endif
    case TBR_VBRIDGE:         /* forward to virtual bridge */
	
        /*
         * Translate packet to token ring and vector to vbridge handler.
         * If handler returns false then send the packet to srb_forward
         * for process switching else to forward explorers.
         */
        pak->linktype = LINK_BRIDGE;

        switch (code_switch) {
        case TBRPAK_RSP:        /* rsp-xxx forward to virtual bridge */
            action = tbridge_to_tring_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action != TBRIDGE_TLB_DONE)
               goto flush_datagram;
            if ((*outputhw->vbridge_forward)(pak, outputhw))
                break;

            /* Copy a packet from an RSP CyBus interface to buffer. */
            pakcopy = pak_clip(pak, pak->datagramsize);
            if (!pakcopy) {
                outputhw->counters.output_nobuffers++;
                goto flush_datagram;
            }
            datagram_done(pak);
            reg_invoke_srt_srb_forward(pakcopy, outputhw, pakcopy->riflen);
            break;

        case TBRPAK_BFR:        /* le-xxx  forward to virtual bridge */
            action = tbridge_to_tring_inline(outputsw, pak, TLB_BFR_TO_BFR);
            if (action != TBRIDGE_TLB_DONE)
               goto flush_datagram;
            if ((*outputhw->vbridge_forward)(pak, outputhw))
                break;
            reg_invoke_srt_srb_forward(pak, outputhw, pak->riflen);
            break;

#ifdef TBRIDGE_MCI_CBUS
        case TBRPAK_MCI:  /* he-mci forward to virtual bridge */

	    on_complex = FALSE;

            /* 
             * Platform is CBUS/SP.  Pull packet into a buffer.
             */
            pakcopy = getbuffer(dgsize_rx);
            if (!pakcopy) {
                outputhw->counters.output_nobuffers++;
                goto flush_datagram;
            }
            if (!tbridge_mci_copy_rx_pak_inline(pak, pakcopy)) {
                datagram_done(pakcopy);
                goto flush_datagram;
            }
            pakcopy->if_output = outputsw;
            action = tbridge_to_tring_inline(outputsw, pakcopy,
					     TLB_BFR_TO_BFR);
            if (action != TBRIDGE_TLB_DONE) {
                datagram_done(pakcopy);
                goto flush_datagram;
	    }
            if ((*outputhw->vbridge_forward)(pakcopy, outputhw))
                break;
            reg_invoke_srt_srb_forward(pakcopy, outputhw, pakcopy->riflen);
            break;

        case TBRPAK_VBR:  /* vbridge to vbridge not supported */
            goto flush_datagram;
#else
        default:
            goto flush_datagram;
#endif
      }
      break;

    default:
	action = TBRIDGE_TLB_UNSUPPORTED;
	break;

    }    /* end of switch on (outputsw->tbridge_media) */


    switch (action) {
    case TBRIDGE_TLB_DONE:
	if (src_bte)
	    src_bte->bte_rxcount++;

	if (dst_bte)
	    dst_bte->bte_txcount++;

	/* Account for received packets that were forwarded. 
         * CSCdi92583- Do not account for BVI inputs to bridging
         */
        if (!is_bvi(inputhw)) {	
            tbridge_rx_accounting_inline(inputhw, 
                                         tbridge_dlli.orig_datagramsize);
        }
	switch (outputsw->tbridge_media) {
	  case TBR_ETHER_SDE:
	  case TBR_FDDI_SDE:
	  case TBR_CTR_SDE:
	  case TBR_TR_SDE:
	  case TBR_HDLC_TRANSIT_SDE:
	      outputsw->sde_said_db->egressing_dot10_packets[VLAN_TBRIDGE]++;
	      break;
	  default:
	      break;	  
	  }

#ifdef TBRIDGE_MCI_CBUS
	/* Account for transmitted packets that were forwarded. */
	tbridge_tx_accounting_inline(outputhw, pak->datagramsize);

	if (!on_complex)
	    tbridge_mci_flush_datagram_inline(inputhw);
#endif

	return(TBRIDGE_FWD_DONE);

    case TBRIDGE_TLB_DEFERRED:

	if (src_bte)
	    src_bte->bte_rxcount++;

	if (dst_bte)
	    dst_bte->bte_txcount++;

	/* Account for received packets that were forwarded. 
         * CSCdi92583- Do not account for BVI inputs to bridging       
         */    
        if (!is_bvi(inputhw)) {
            tbridge_rx_accounting_inline(inputhw, dgsize_rx);
        }
#ifdef TBRIDGE_MCI_CBUS
	tbridge_mci_flush_datagram_inline(inputhw);
#endif

	return(TBRIDGE_FWD_DONE);

    case TBRIDGE_TLB_NO_TXBUF:
	outputhw->counters.output_total_drops++;
	break;

    case TBRIDGE_TLB_EXCEEDS_MTU:
	outputhw->span_out_bigdrop++;
	break;

    case TBRIDGE_TLB_UNSUPPORTED:
	break;
    }
    /* FALL THROUGH TO FLUSH DATAGRAM */

 flush_datagram:

    /* Account for received packets that were filtered. */
    /*
     * This does not distinguish between filtering on
     * input or output, let alone the specific reason
     * for filtering, but at least it accounts for these
     * packets separately from forwarded and flooded ones.
     * We do not account at all for flooded packets that
     * were filtered on output.
     */

    /* CSCdi92583- Do not account for BVI inputs to bridging */ 
    if (!is_bvi(inputhw)) {
        tbridge_rx_accounting_inline(inputhw, tbridge_dlli.orig_datagramsize);
    }

    switch (code_switch) {

    case TBRPAK_RSP:
    case TBRPAK_BFR:
	datagram_done(pak);
	break;

#ifdef TBRIDGE_MCI_CBUS
    case TBRPAK_MCI:
	tbridge_mci_flush_datagram_inline(inputhw);
	break;

    case TBRPAK_VBR:
        break;
#else
    default:
	break;
#endif
    }

    return(TBRIDGE_FWD_DONE);
}

/*
 * Set the packet copy attributes expected by process-level
 * bridging and queue the packet copy for process-level bridging.
 * No matter the encaps, it has got to look like ethernet by the
 * end of this routine to keep hybridge_input happy.
 */

static inline
void tbridge_slowswitch_inline (paktype *pak)

{
    hwidbtype *inputhw = pak->if_input->hwptr;
    idbtype *outputsw = pak->if_output;

    /*
     * All this is copied from the original which has no merit
     * other than that it least disturbs the status quo.
     * Process-level bridging is its own mess.
     */

    if (!inputhw->sinput)
	/*
	 * Original comment:
	 * That means we don't have "sinput" routine which should
	 * take care of "input" counter
	 */
	inputhw->counters.inputs++;	/* slow packets */

    GET_TIMESTAMP(inputhw->lastinput);

     /* SDE encaps need to be handled properly for bridging to process
      * level.  Note sde_enqueue routine is for handling locally processed
      * packets (ie broadcast, BPDU, ARP). Please note switch below implies
      * there ARE sde encapsulations (where tbridge media is NOT TBR_XXX_SDE)
      * that will slowswitch/bridge on major interface with sde encap NOT
      * extracted. This is by VLAN design when a packet traverses an interface
      * with bridging, yet particular color (SAID) for packet is not configured
      * on incoming interface. In these cases, switch below hits default
      * case only.
      */
    switch (pak->if_input->tbridge_media) {
        case TBR_ETHER_SDE:
        case TBR_FDDI_SDE:
        case TBR_CTR_SDE:
        case TBR_TR_SDE:
        case TBR_HDLC_TRANSIT_SDE:
             sde_extract_header_inline(pak);
        default:
             break;
    }

    if (outputsw &&
	outputsw->span_latcompress && (outputsw->span_latsize != -1))
	pak->datagramsize = outputsw->span_latsize;

    pak->enctype = ET_ARPA;
    pak->linktype = LINK_BRIDGE;
    pak->acct_proto = ACCT_PROTO_BRIDGE;

    /*
     * since we can get tr and fddi encap'ed packets, make them look
     * like ethernet packets.  standards based protocols encap fddi
     * and tr packets.  so first check for standards based protocols
     * based soley on circuit id (for frame relay and atm) or 
     * smds multcast address pointer (for smds).  
     *
     * for hdlc and fddi transit, the transit header and the
     * datagramsize needs to be adjusted since it's already in
     * ethernet form.  we just have to strip off the header.
     * for all standards based transit encapsulations
     * (currently atm, frame relay, and smds), do magical
     * things in tbridge_convert_to_ether_inline().
     *
     * remember, standards based transit encaps are really virtual
     * ethernet, tr or fddi packets...
     */
    if ((0 != tbridge_dlli.circuit_id) || (NULL != tbridge_dlli.smdssrc_ptr)) {
	/*
	 * if false is returned, it's of an unknown packet type
	 */
	if (FALSE == tbridge_convert_to_ether_inline(pak)) {
	    datagram_done(pak);
	    return;
	}
    } else if (inputhw->bridge_transit) {
	pak->flags |= PAK_TBRIDGE_IN;
	pak->datagramstart += tbridge_dlli.transit_encapsize;
	pak->datagramsize -= tbridge_dlli.transit_encapsize;
    }

    /*
     * If interface is TR or low-end FDDI then swap the address back to
     * non-canonical because the scheduled process expects the addresses
     * to be in token ring format.
     *
     * HACK ALERT!!  The test for low-end FDDI is a gross hack !!  The real
     * fix is to leave addresses in canonical format and remove references
     * to low-end platform type IDBTYPE_FDDIT from the "hybridge_input()"
     * function.  This would require the SRB, Routing and Input-Queuing
     * paths of low-end input drivers to canonicalize the FDDI addresses.
     * On the high-end platforms the FDDI addresses are always canonical.
     */

    if ((inputhw->status & IDB_TR) || (inputhw->type == IDBTYPE_BSI)) {
        ieee_addrs *addr_start = (ieee_addrs *)pak->addr_start;
        ieee_swap(addr_start->daddr, addr_start->daddr);
        ieee_swap(addr_start->saddr, addr_start->saddr);
    }

    process_enqueue_pak(bridgeQ, pak);
}


/*
 * Flood a received packet on all interfaces in the flood list.
 * The packet resides in network buffer prepared by the caller.
 * From low-end interfaces, this is the received packet itself.
 * From MCI/cBUS and RSP interfaces, this is a copy of the
 * received packet specifically for the purposes of flooding.
 *
 * For low-end and RSP interfaces, the original packet is
 * duplicated for each output interface, and the original packet
 * is freed upon flood termination.
 *
 * For MCI/cBus interfaces, the flood is translated into a tx window or
 * frame, and transmitted from there.  The copy of the packet is not
 * modified and is left in place upon flood termination.
 *
 * The flood list is a list of outgoing-interface descriptors
 * that associate an idb with a variety of attributes that can
 * be used to determine whether the interface should be included
 * in the flood.
 */

STATIC INLINE
void tbridge_flood_inline (paktype *pak,
			   tbifd_type *oif_list,
			   ulong flood_scope,
			   boolean cbus_flood_done,
			   const enum TBRIDGE_PAKMEM code_switch)
{
    idbtype	*inputsw = pak->if_input;
    hwidbtype	*inputhw = inputsw->hwptr;
    idbtype	*outputsw = NULL;
    hwidbtype	*outputhw = NULL;
    void        *subidb = NULL;

    paktype	*pakcopy = NULL;
#ifdef TBRIDGE_MCI_CBUS
    paktype	*half_fast_pak;
#endif
    ushort	datagramsize_rx = pak->datagramsize;
    ieee_addrs	*addr_start = (ieee_addrs *)pak->addr_start;
    spantype	*span;

    tbifd_type	*oif;			/* outgoing interface descriptor */
    int		ccg;			/* circuit group for dld */
    enum TBRIDGE_TLB_RETVAL action;

    /*
     * If the input interface is BVI, we don't have valid span_ptr
     * since BVI is not a bridging interface. Don't use
     * inputsw->span_ptr below to reference bridge-group, use local
     * variable "span" instead. outputsw->span_ptr is OK since output
     * won't never be a BVI which is a routing interface.
     */
    if (is_bvi(inputhw)) {
	span = spanarray[inputhw->unit];
    } else {
	span = inputsw->span_ptr;
    }

    /* Account for received packets that were flooded. 
     * CSCdi92583- Do not account for BVI inputs to bridging
     */
    if (!is_bvi(inputhw)) {
        tbridge_rx_accounting_inline(inputhw, tbridge_dlli.orig_datagramsize);
    } 
#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_2 && (!ISMULTICAST(addr_start->daddr)))
	buginf("\nTB2: flooding %s received on %s"
	       "\n     %e to %e",
	       rxtype_string(pak->rxtype), pak->if_input->namestring,
	       addr_start->saddr, addr_start->daddr);

    if (tbridge_debug_mcast_2 && ISMULTICAST(addr_start->daddr))
	buginf("\nTB2: flooding %s received on %s"
	       "\n     %e to %e",
	       rxtype_string(pak->rxtype), pak->if_input->namestring,
	       addr_start->saddr, addr_start->daddr);
#endif	/* TBRIDGE_DEBUG */

    for (oif = oif_list; oif; oif = oif->tbifd_next) {
	outputsw = oif->tbifd_swidb;

        /*
         * If this interface is in a circuit group,
         * determine the output interface.
         */
        if (outputsw->circ_group) {
            ccg = outputsw->circ_group;
            outputsw = dld_get_output_idb(span, ccg,
					  &addr_start->saddr[0],
					  &addr_start->daddr[0]);
            if (!outputsw) {
                if (TIMER_RUNNING(span->cct_groups[ccg].ccg_pause_timer) &&
                    AWAKE(span->cct_groups[ccg].ccg_pause_timer))
                    ccg_pause_timer_expiry(span, ccg);
                continue;
            }
        }

	/* 
         * If this interface is participating in VTP pruning, make
 	 * sure that the output interface is not in 'pruned' state.
         * Otherwise, floods are disabled.
         */

	subidb = idb_get_swsb(outputsw, SWIDB_SB_VTP_SUB);
	if ((subidb) && (reg_invoke_vtp_port_is_pruned(subidb)))
	  continue;
	  
	/*
	 * Output interface is on the input net:
         * Don't propogate flood packet in this case. Standards
         * based wan clouds included (ie ATM FRAME_RELAY) as these
         * clouds specify fully meshed topology for bridge support
         * (see 1490 spec related to bridging) and therefore do
         * not require broadcast, multicast bridge forwarding beyond
         * single bridge hop. For partial mesh, using unique
         * vc per subinterface handles situation.
         */

	if ((outputsw->if_number == inputsw->if_number) ||
            (inputsw->circ_group && 
	    (outputsw->circ_group == inputsw->circ_group))) {
	        oif->tbifd_rx_flood_packets++;
		continue;
        }

	/*
	 * Output interface is not forwarding:
	 */
	if (!outputsw->circ_canforward)
	    continue;

	outputhw = outputsw->hwptr;

  	if (outputhw->state != IDBS_UP)
  	    continue;

	if (!(oif->tbifd_attributes & flood_scope)) {

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
	    if (tbridge_debug_ipm)
		if (tbridge_dlli.dlli_attributes & DLLI_IPMCAST)
		    buginf("\nTB: %i skipping %s (attr %04x scope %04x)",
			   GETLONG((ipaddrtype *)(pak->network_start + 16)),
			   oif->tbifd_swidb->namestring,
			   oif->tbifd_attributes, flood_scope);
#endif /* TBRIDGE_DEBUG */

	    continue;
	}

	/*
	 * Skip over cBus bridging interfaces if the cBus flood has been done.
	 */
	if ((outputhw->cbus_bridge_enable) &&
	    (oif->tbifd_attributes & TBIFD_AUTO_FLOOD) &&
	    (cbus_flood_done))
	    continue;

	/*
	 * Perform output filter checking
	 */
	if ((outputsw->span_address_out) &&
	    (!access_check_mac(outputsw->span_address_out, addr_start->daddr)))
	    continue;

	if ((outputsw->span_flag_out) &&
	    (!tbridge_type_sap_filter(pak, outputsw->span_lsap_out,
				      outputsw->span_type_out)))
	    continue;

	if ((outputsw->span_pattern_out) &&
	    (!xmacaccesscheck(outputhw, outputsw->span_pattern_out,
			      addr_start->daddr, FALSE)))
	    continue;

	/*
	 * Account for packets actually flooded on this interface.
	 */
	oif->tbifd_tx_flood_packets++;

	switch (code_switch) {

	case TBRPAK_RSP:
	case TBRPAK_BFR:
	    {
		short dgsize = tbridge_dlli.orig_datagramsize;

		pakcopy = getbuffer(dgsize);
		if (!pakcopy) {
		    outputhw->counters.output_nobuffers++;
		    continue;
		}
		/*
		 * for ATM (and frame relay) we need to copy the
		 * transit encapsulation as well.  pak->datagramsize
		 * reflects the size w/o the encapsize.
		 */
		pak_copy(pak, pakcopy, dgsize);
                pakcopy->if_input   = inputsw;
		pakcopy->if_output = outputsw;
		pakcopy->linktype = LINK_BRIDGE;
		pakcopy->acct_proto = ACCT_PROTO_BRIDGE;
		pakcopy->bridgeptr = NULL;
	    }
	    break;

	case TBRPAK_MCI:
        case TBRPAK_VBR:
	    /*
	     * Just use the flood pak to drive the translation.
	     * Restore the original datagramsize between translations
	     * since it gets written in the process.
	     */
	    pakcopy = pak;
	    pakcopy->datagramsize = datagramsize_rx;
	    break;
	}

	/*
	 * If LAT compression enabled, handle the compression now.
	 */
	if (outputsw->span_latcompress) {
	    outputsw->span_latsize = -1;
	    if (tbridge_get_arpa_type_inline(pak) == TYPE_LAT) {
		tbridge_check_lat_compression(outputsw, pakcopy);
	    }
	}

	/*
	 * Check to see if we're forcing things to punt to process level
	 */ 
	if (outputhw->span_bridge_process_force) {

	    /*
	     * Check to see if this packet is too big for the output interface.
	     */
	    if (pakcopy->datagramsize -
		(pakcopy->network_start - pakcopy->mac_start) >
		llc_mtu(outputhw)) {
		if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
		    datagram_done(pakcopy);
		outputhw->span_out_bigdrop++;
		continue;
	    }

	    /*
	     * For high-end interfaces, we need to make a copy of
	     * the packet in tbridge_flood_pak.
	     */
	    switch (code_switch) {

	    case TBRPAK_MCI:
		{
		    short dgsize = tbridge_dlli.orig_datagramsize;

		    /*
		     * CSCdi31501
		     * Don't use pak_duplicate.  It will allocate buffers
		     * from the same pool as tbridge_flood_pak which could
		     * be huge.  Instead, get a buffer of the right size
		     * using the original size of the rx packet, and then
		     * copy the thing.
		     */
		    pakcopy = getbuffer(dgsize);
		    if (!pakcopy) {
			outputhw->counters.output_nobuffers++;
			continue;
		    }
		    pak_copy(pak, pakcopy, dgsize);
		    pakcopy->if_output = outputsw;
		    pakcopy->bridgeptr = NULL;
		}
		break;

	    case TBRPAK_RSP:
	    case TBRPAK_BFR:
            case TBRPAK_VBR:
		break;
	    }

	    tbridge_slowswitch_inline(pakcopy);
	    continue;
	}
	action = TBRIDGE_TLB_UNSUPPORTED;

	switch (outputsw->tbridge_media) {

	case TBR_ETHER:			/* flood to ether */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-ether*/
	    case TBRPAK_BFR:		/* le-xxx flood to le-ether*/
		action = tbridge_to_ether_inline(outputsw, pakcopy,
						 TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to he-ether */
		action = tbridge_to_ether_inline(outputsw, pakcopy,
						 TLB_BFR_TO_MCI);
		if ((action == TBRIDGE_TLB_DONE) &&
		    !outputhw->cbus_bridge_enable) 
		    tbridge_mci_tx1_inline(outputhw, pakcopy->datagramsize);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_ether_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_FDDI:			/* flood to fddi */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-fddi */
	    case TBRPAK_BFR:		/* le-xxx flood to le-fddi */
		action = tbridge_to_fddi_inline(outputsw, pakcopy,
						TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to he-fddi */
		action = tbridge_to_fddi_inline(outputsw, pakcopy,
						TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE &&
		    !outputhw->cbus_bridge_enable)
		    tbridge_mci_tx1_inline(outputhw,
					   pakcopy->datagramsize |
					   MCI_TX_ODDALIGN);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
			action = tbridge_to_fddi_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_CTR:			/* flood to ctr */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to ctr */
		action = tbridge_to_tring_inline(outputsw, pakcopy,
						 TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
	    case TBRPAK_BFR:		/* le-xxx flood to ctr */
		action = TBRIDGE_TLB_UNSUPPORTED;
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to ctr */
		action = tbridge_to_tring_inline(outputsw, pakcopy,
						 TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) 
		    tbridge_mci_tx1_inline(outputhw, pakcopy->datagramsize);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_tring_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_TR:			/* flood to ltr */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to ltr */
		action = TBRIDGE_TLB_UNSUPPORTED;
		break;
	    case TBRPAK_BFR:		/* le-xxx flood to ltr */
		action = tbridge_to_tring_inline(outputsw, pakcopy,
						 TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to ltr */
            case TBRPAK_VBR:            /* flood to virtual bridge */
		action = TBRIDGE_TLB_UNSUPPORTED;
		break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_FDDI_TRANSIT:		/* flood to fddi-t */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-fddi-t */
	    case TBRPAK_BFR:		/* le-xxx flood to le-fddi-t */
		action = tbridge_to_fddi_transit_inline(outputsw, pakcopy,
							NULL, TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to he-fddi-t */
		action = tbridge_to_fddi_transit_inline(outputsw, pakcopy,
							NULL, TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE &&
		    !outputhw->cbus_bridge_enable)
		    tbridge_mci_tx1_inline(outputhw,
					   pakcopy->datagramsize |
					   MCI_TX_ODDALIGN);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_fddi_transit_inline(outputsw,
				half_fast_pak, NULL, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_ATM_TRANSIT: {
	    vc_info_t *vc;
	    paktype *p_copy = NULL;

	    /*
	     * flood this packet out to all the vc's associated w/ this
	     * SOFTWARE idb.  cycle through the linked list of vc's off
	     * this sw idb.
	     *
	     * MPP try to make this into a registry.  however it's harder
	     * than one is lead to believe.  (to much context
	     * associated w/ the TB code.)  however because
	     * tbridge_circuitQ is just a queuetype ptr, this actually
	     * isn't bad.
	     */
	    vc = (vc_info_t *)outputsw->tbridge_circuitQ.qhead;
	    if (vc == NULL) {
		errmsg(TBRIDGE_ERR_NOVCFLOOD, outputsw->namestring);
		action = TBRIDGE_TLB_DONE;
                if ((code_switch==TBRPAK_RSP) || (code_switch==TBRPAK_BFR)) 
                    datagram_done(pakcopy);
	    }

	    for (;vc; vc = vc->next_vc_in_bridge_group) {

		/*
		 * make sure VC is up
		 */
		if (!ISVCACTIVE(vc))
		    continue;

		/*
		 * don't transmit on the rx'ed VC
		 */
		if (tbridge_dlli.circuit_id == vc->vc)
		    continue;

		switch (code_switch) {
		case TBRPAK_RSP:
		case TBRPAK_BFR: {	/* le-xxx flood to le-atm */
		    /*
		     * for the low end & rsp, make a duplicate copy
		     * of the packet.  but for the last vc, don't
		     * make a copy, but send the original packet.
		     */
		    if (vc->next_vc_in_bridge_group != NULL) {
			p_copy = getbuffer(tbridge_dlli.orig_datagramsize);
			if (!p_copy) {
			    outputhw->counters.output_nobuffers++;
			    continue;
			}
			pak_copy(pak, p_copy, tbridge_dlli.orig_datagramsize);
			p_copy->if_output = outputsw;
			p_copy->linktype = LINK_BRIDGE;
			p_copy->acct_proto = ACCT_PROTO_BRIDGE;
			p_copy->bridgeptr = NULL;
			p_copy->atm_enctype = ATM_DM_PKT_AAL5;

			action = tbridge_to_atm_transit_inline(outputsw,
							       p_copy, vc->vc,
							       TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    (*outputhw->fastsend)(outputhw, p_copy);
			else
			    datagram_done(p_copy);
		    } else {
			action = tbridge_to_atm_transit_inline(outputsw,
							       pakcopy, vc->vc,
							       TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) {
			    pakcopy->atm_enctype = ATM_DM_PKT_AAL5;
			    (*outputhw->fastsend)(outputhw, pakcopy);
			} else {
                            datagram_done(pakcopy);
                        }
		    }
		}
		    break;
#ifdef TBRIDGE_MCI_CBUS
		case TBRPAK_MCI:	/* he-xxx flood to he-atm */
		    pakcopy = pak;
		    pakcopy->datagramsize = datagramsize_rx;
		    action = tbridge_to_atm_transit_inline(outputsw,
							   pakcopy, vc->vc,
							   TLB_BFR_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE &&
			!outputhw->cbus_bridge_enable)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
	            else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		             (outputhw->priority_list)) {
		        /*
		         * for the low end & rsp, make a duplicate copy
		         * of the packet.  but for the last vc, don't
		         * make a copy, but send the original packet.
		         */
		        if (vc->next_vc_in_bridge_group != NULL) {
			    p_copy = getbuffer(tbridge_dlli.orig_datagramsize);
			    if (!p_copy) {
			        outputhw->counters.output_nobuffers++;
			        continue;
			    }
			    pak_copy(pak, p_copy,
				     tbridge_dlli.orig_datagramsize);
			    p_copy->if_output = outputsw;
			    p_copy->linktype = LINK_BRIDGE;
			    p_copy->acct_proto = ACCT_PROTO_BRIDGE;
			    p_copy->bridgeptr = NULL;

			    action = tbridge_to_atm_transit_inline(outputsw,
								   p_copy, vc->vc,
								   TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
		                action = tbridge_mci_enqueue_holdq_inline(outputhw, p_copy);
			    else
			        datagram_done(p_copy);
		        } else {
			    p_copy = pakcopy;

			    action = tbridge_to_atm_transit_inline(outputsw,
								   p_copy, vc->vc,
								   TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
		                action = tbridge_mci_enqueue_holdq_inline(outputhw, p_copy);
		        }
		    }
		    break;
                case TBRPAK_VBR:  /* flood to virtual bridge not supported */
	            action = TBRIDGE_TLB_UNSUPPORTED;
                    break;
#else
		default:
		    break;
#endif
		}

		/*
		 * this is here to try to make accouting a little
		 * saner.  however, it's based on an swidb, vs
		 * the VC, which it really should be.  someone
		 * needs to fix this...
		 */
		switch (action) {

		case TBRIDGE_TLB_DEFERRED:
		case TBRIDGE_TLB_DONE:
		    break;

		case TBRIDGE_TLB_NO_TXBUF:
		    outputhw->counters.output_total_drops++;
		    if ((TBRPAK_RSP == code_switch) ||
			(TBRPAK_BFR == code_switch))
			datagram_done(p_copy);
		    continue;
		    break;

		case TBRIDGE_TLB_EXCEEDS_MTU:
		    outputhw->span_out_bigdrop++;
		    if ((TBRPAK_RSP == code_switch) ||
			(TBRPAK_BFR == code_switch))
			datagram_done(p_copy);
		    continue;
		    break;

		case TBRIDGE_TLB_UNSUPPORTED:
		    if ((TBRPAK_RSP == code_switch) ||
			(TBRPAK_BFR == code_switch))
			datagram_done(p_copy);
		    continue;
		    break;
		}
	    }
	} 
	    break;
 	case TBR_LANE_ETHER: {
 	    ushort          vc;
 	    
	    /*
 	     * We have to get the BUS vc from lec_info directly as this could
	     * be a broadcast ARP and so there is no BTE.
 	     */
 	    vc =  is_atm_lane(outputsw) ? (outputsw->lane_info->lec_info->
 					   mcast_send.vcd) : 0;
 	    if (!vc) {
 		action = TBRIDGE_TLB_DONE;
		if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
		    datagram_done(pakcopy);
		continue;
		break;
 	    }
	
 	    switch (code_switch) {
 	    case TBRPAK_RSP:
 	    case TBRPAK_BFR: {		/* le-xxx flood to le-lane-ether */
 
		pakcopy->atm_enctype =  ATM_DM_PKT_AAL5;
 		action = tbridge_to_lane_ether_inline(outputsw,
 						      pakcopy,
 						      vc,
 						      TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
 		    (*outputhw->fastsend)(outputhw, pakcopy);
 		}
 	    }
 		break;
#ifdef TBRIDGE_MCI_CBUS
 	    case TBRPAK_MCI:		/* he-xxx flood to he-lane-ether */
 		action = tbridge_to_lane_ether_inline(outputsw,
 						      pakcopy,
 						      vc,
 						      TLB_BFR_TO_MCI);
 		if (action == TBRIDGE_TLB_DONE &&
 		    !outputhw->cbus_bridge_enable)
 		    tbridge_mci_tx1_inline(outputhw,
 					   pakcopy->datagramsize);
		else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
			 (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
 		        action =  tbridge_to_lane_ether_inline(outputsw,
 							       half_fast_pak,
 							       vc,
 							       TLB_BFR_TO_BFR);
 		        if (action == TBRIDGE_TLB_DONE)
 			    tbridge_mci_enqueue_holdq_inline(outputhw,
							     half_fast_pak);
                    }
		}
 		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
 	    default:
 		break;
#endif
 	    } 
	    /* Accounting code similar to TBR_ATM_TRANSIT case */
	    switch (action) {
 
	    case TBRIDGE_TLB_DEFERRED:
	    case TBRIDGE_TLB_DONE:
		break;
 
	    case TBRIDGE_TLB_NO_TXBUF:
		outputhw->counters.output_total_drops++;
		if ((TBRPAK_RSP == code_switch) ||
		    (TBRPAK_BFR == code_switch))
		    datagram_done(pakcopy);
		continue;
		break;
 
	    case TBRIDGE_TLB_EXCEEDS_MTU:
		outputhw->span_out_bigdrop++;
		if ((TBRPAK_RSP == code_switch) ||
		    (TBRPAK_BFR == code_switch))
		    datagram_done(pakcopy);
		continue;
		break;
 
	    case TBRIDGE_TLB_UNSUPPORTED:
		if ((TBRPAK_RSP == code_switch) ||
		    (TBRPAK_BFR == code_switch))
		    datagram_done(pakcopy);
		continue;
		break;
	    }

	}
 	    break;
	    
	case TBR_HDLC_TRANSIT:	/* flood to serial-t */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-serial-t */
	    case TBRPAK_BFR:		/* le-xxx flood to le-serial-t */
		action = tbridge_to_hdlc_transit_inline(outputsw, pakcopy,
							TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to he-serial-t */
		action = tbridge_to_hdlc_transit_inline(outputsw, pakcopy,
							TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE &&
		    !outputhw->cbus_bridge_enable)
		    tbridge_mci_tx1_inline(outputhw, pakcopy->datagramsize);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_hdlc_transit_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

	case TBR_FRAME_RELAY_TRANSIT: {
	    fr_pvc_info_type *fr_pvc, *fr_pvc_input;
	    boolean not_first;

	    if (inputhw == outputhw) {
		fr_pvc_input = HWIDB_TO_FR_PVC(inputhw, 
					       tbridge_dlli.circuit_id);
	    } else {
		fr_pvc_input = NULL;
	    }
	    
	    /*
	     * flood this packet out to all the vc's associated w/ this
	     * SOFTWARE idb.  cycle through the linked list of vc's off
	     * this sw idb.
	     */
	    not_first = FALSE;
	    for (fr_pvc = outputsw->tbridge_circuitQ.qhead;
		 fr_pvc != NULL;
		 fr_pvc = fr_pvc->bridge_flood_next) {
		/*
		 * make sure VC is up
		 */
		if (!(DLCI_ACTIVE(fr_pvc->pvc_status)))
		    continue;

		if (fr_pvc == fr_pvc_input)
		    continue;

		switch (code_switch) {
		case TBRPAK_RSP:
		case TBRPAK_BFR:	/* le-xxx flood to le-atm */
		    if (not_first) {
			/* Account for transmitted packets that were
			   flooded. on the previous send */
			if (ISMULTICAST(addr_start->daddr))
			    outputhw->counters.output_broad++;
			pakcopy = getbuffer(tbridge_dlli.orig_datagramsize);
			if (!pakcopy) {
			    outputhw->counters.output_nobuffers++;
			    continue;
			}
			/* Perhaps use pak_duplicate here ? */
			pak_copy(pak, pakcopy, tbridge_dlli.orig_datagramsize);
			pakcopy->if_output = outputsw;
			pakcopy->linktype = LINK_BRIDGE;
			pakcopy->acct_proto = ACCT_PROTO_BRIDGE;
			pakcopy->bridgeptr = NULL;
		    } else
			not_first = TRUE;
		    action = tbridge_to_frame_relay_inline(outputsw,
							   outputhw,
							   pakcopy,
							   fr_pvc,
							   TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		    break;
#ifdef TBRIDGE_MCI_CBUS
		case TBRPAK_MCI:	/* he-xxx flood to he-atm */
		    action = tbridge_to_frame_relay_inline(outputsw,
							   outputhw,
							   pakcopy, fr_pvc,
							   TLB_BFR_TO_MCI);
		    /*
		     * Account for transmitted packets that were
		     * flooded. on the previous send
		     */
		    if (ISMULTICAST(addr_start->daddr))
			outputhw->counters.output_broad++;
		    if (action == TBRIDGE_TLB_DONE &&
			!outputhw->cbus_bridge_enable)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
	            else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		             (outputhw->priority_list)) {
		        half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		        if (half_fast_pak) {
			    action = tbridge_to_frame_relay_inline(outputsw,
								   outputhw,
								   half_fast_pak,
								   fr_pvc,
								   TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE) 
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
		        }
		    }
		    break;
                case TBRPAK_VBR: /* flood to virtual bridge not supported */
	            action = TBRIDGE_TLB_UNSUPPORTED;
                    break;
#else
		default:
		    break;
#endif
		}
	    }
	}
	    break;
	case TBR_SMDS_TRANSIT:	{/* flood to serial-t */
	    smdsaddr *smds_addr;
	    /*
	     * We now flood to the smds bridge multicast address.
	     * If it's not available, don't flood
	     */
	    if (outputsw->tbridge_circuitQ.qhead == NULL) {
	        if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
                    datagram_done(pakcopy);
		action = TBRIDGE_TLB_DONE;
	        break;
	    }
	    smds_addr = (smdsaddr *)outputsw->tbridge_circuitQ.qhead;
	    switch (code_switch) {
	    case TBRPAK_RSP:	/* rsp-xxx flood to rsp-serial-t */
	    case TBRPAK_BFR:	/* le-xxx flood to le-serial-t */
		action = tbridge_to_smds_inline(outputsw, outputhw, pakcopy,
						smds_addr->addr,
						TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:	/* he-xxx flood to he-serial-t */
		action = tbridge_to_smds_inline(outputsw, outputhw, pakcopy,
						smds_addr->addr,
						TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE &&
		    !outputhw->cbus_bridge_enable)
		    tbridge_mci_tx1_inline(outputhw, pakcopy->datagramsize);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_smds_inline(outputsw, outputhw,
							half_fast_pak,
							smds_addr->addr,
							TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}

		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	}
	    break;

        case TBR_ETHER_SDE:		/* flood to ether-sde */
            switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-ether-sde */
            case TBRPAK_BFR:		/* le-xxx flood to le-ether-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_ether_sde_inline(outputsw, pakcopy,
							 TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		}
                break;
#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:		/* he-xxx flood to he-ether-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_ether_sde_inline(outputsw, pakcopy,
							 TLB_BFR_TO_MCI);
		    if ((action == TBRIDGE_TLB_DONE) &&
			!outputhw->cbus_bridge_enable)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
		}
	        if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		    (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_llc_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) {
		            action = tbridge_to_ether_sde_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE) 
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
		        }
		    }
		}
                break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
            }
            break;

        case TBR_FDDI_SDE:		/* flood to fddi-sde */
            switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-fddi-sde */
            case TBRPAK_BFR:		/* le-xxx flood to le-fddi-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_fddi_sde_inline(outputsw, pakcopy,
							TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		}
                break;
#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:		/* he-xxx flood to he-fddi-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_fddi_sde_inline(outputsw, pakcopy,
							TLB_BFR_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE &&
			!outputhw->cbus_bridge_enable)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize |
					       MCI_TX_ODDALIGN);
		}
	        if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		    (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_llc_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) {
		            action = tbridge_to_fddi_sde_inline(outputsw,
						half_fast_pak, TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
		        }
		    }
		}
                break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
            }
            break;

        case TBR_CTR_SDE:		/* flood to ctr-sde */
            switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to ctr-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_tring_sde_inline(outputsw, pakcopy,
							 TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		}
                break;
            case TBRPAK_BFR:		/* le-xxx flood to ctr-sde */
                action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:		/* he-xxx flood to ctr-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_tring_sde_inline(outputsw, pakcopy,
							 TLB_BFR_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
		}
	        if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		    (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_llc_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) {
		            action = tbridge_to_tring_sde_inline(outputsw,
						 half_fast_pak, TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
		        }
		    }
		}
                break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
            }
            break;

        case TBR_TR_SDE:		/* flood to ltr-sde */
            switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to ltr-sde */
                action = TBRIDGE_TLB_UNSUPPORTED;
                break;
            case TBRPAK_BFR:		/* le-xxx flood to ltr-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_tring_sde_inline(outputsw, pakcopy,
							 TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		}
                break;
#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:		/* he-xxx flood to ltr-sde */
            case TBRPAK_VBR:            /* flood to virtual bridge */
                action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
            }
            break;

        case TBR_HDLC_TRANSIT_SDE:      /* flood to serial-t-sde */
            switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-serial-t-sde */
            case TBRPAK_BFR:		/* le-xxx flood to le-serial-t-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_hdlc_transit_sde_inline(outputsw,
								pakcopy,
								TLB_BFR_TO_BFR);
		    if (action == TBRIDGE_TLB_DONE)
			(*outputhw->fastsend)(outputhw, pakcopy);
		}
                break;
#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:		/* he-xxx flood to he-serial-t-sde */
                action = tbridge_to_llc_inline(outputsw, pakcopy, 
					       TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE) {
		    action = tbridge_to_hdlc_transit_sde_inline(outputsw,
								pakcopy,
								TLB_BFR_TO_MCI);
		    if (action == TBRIDGE_TLB_DONE &&
			!outputhw->cbus_bridge_enable)
			tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
		}
	        if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		    (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_llc_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) {
		            action = tbridge_to_hdlc_transit_sde_inline
				(outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			    if (action == TBRIDGE_TLB_DONE)
				action = tbridge_mci_enqueue_holdq_inline
				    (outputhw, half_fast_pak);
		        }
		    }
		}
                break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
            }
            break;

	case TBR_LEX_TRANSIT:		/* flood to lex-t */
	    switch (code_switch) {
	    case TBRPAK_RSP:		/* rsp-xxx flood to rsp-lex-t */
	    case TBRPAK_BFR:		/* le-xxx flood to le-lex-t */
		action = tbridge_to_lex_transit_inline(outputsw, pakcopy,
						       TLB_BFR_TO_BFR);
		if (action == TBRIDGE_TLB_DONE)
		    (*outputhw->fastsend)(outputhw, pakcopy);
		break;
#ifdef TBRIDGE_MCI_CBUS
	    case TBRPAK_MCI:		/* he-xxx flood to he-lex-t */
		action = tbridge_to_lex_transit_inline(outputsw, pakcopy,
						       TLB_BFR_TO_MCI);
		if (action == TBRIDGE_TLB_DONE &&
		    !outputhw->cbus_bridge_enable)
		    tbridge_mci_tx1_inline(outputhw, pakcopy->datagramsize);
	        else if ((action == TBRIDGE_TLB_NO_TXBUF) &&
		         (outputhw->priority_list)) {
		    half_fast_pak = tbridge_copy_pak(pakcopy, outputsw);
		    if (half_fast_pak) {
		        action = tbridge_to_lex_transit_inline
			    (outputsw, half_fast_pak, TLB_BFR_TO_BFR);
			if (action == TBRIDGE_TLB_DONE) 
			    action = tbridge_mci_enqueue_holdq_inline
				(outputhw, half_fast_pak);
		    }
		}
		break;
            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
	        action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
	    default:
		break;
#endif
	    }
	    break;

    /*
     * The following case is bridging out to an Inter Switch Link (ISL)
     * encapsulating vLAN interface. This is only supported on highend
     * Fast Ethernet (CxBus/RSP/LES) interfaces.
     */

        case TBR_ETHER_ISL_VLAN:     /* Flood to Fast Ethernet ISL encap */
                                     /* Just prepend 26 byte ISL header here */
            switch (code_switch) {

                case TBRPAK_RSP:      /* rsp-xxx flood to */
		                       /* rsp-fastether-isl */
                case TBRPAK_BFR:      /* les-xxx flood to */
                                       /* les-fast-ether-isl */


                    /*
                     * Call this function to adjust the packet's encapsulation
                     * when translating beween different media, eg fddi->ether,
                     * token->ether.
                     */
                    action = tbridge_to_ether_inline(outputsw, pakcopy,
                                                 TLB_BFR_TO_BFR);
 
                    if (action == TBRIDGE_TLB_DONE) {
                        tbridge_prepend_isl_header(outputsw, pakcopy);
                        (*outputhw->fastsend)(outputhw, pakcopy);
                    }

                break;

#ifdef TBRIDGE_MCI_CBUS

                case TBRPAK_MCI:        /* he-xxx flood to he-fastether-isl */
                action = tbridge_to_ether_inline(outputsw, pakcopy,
                                                     TLB_BFR_TO_MCI);
                if (action == TBRIDGE_TLB_DONE) {
 
                    pakcopy->encsize = TBRIDGE_MCI_TX1_OFFSET;
                    pakcopy->encsize -= ISL_INTERIOR_PACKET;
                    tbridge_mci_tx1_select_offset_inline(outputhw,
                                                         pakcopy->encsize);
                    tbridge_prepend_isl_header(outputsw, pakcopy);

		    if (action == TBRIDGE_TLB_DONE &&
			!outputhw->cbus_bridge_enable) {
		      
		        tbridge_mci_tx1_inline(outputhw,
					       pakcopy->datagramsize);
		    }

		} else
                    action = TBRIDGE_TLB_NO_TXBUF;

                break;

            /* Compiler needs to see all enumerations */

            case TBRPAK_VBR:   /* flood to virtual bridge not supported */
                action = TBRIDGE_TLB_UNSUPPORTED;
                break;
#else
            default:
                break;
#endif
            }
            break;
            case TBR_VBRIDGE: /* flood across virtual bridge. */
              switch (code_switch) {
              case TBRPAK_RSP:    /* rsp-xxx flood to rsp-ether*/
              case TBRPAK_BFR:    /* le-xxx flood to le-ether*/
                 break;

#ifdef TBRIDGE_MCI_CBUS
            case TBRPAK_MCI:    /* he-xxx flood to he-ether */
                pakcopy = getbuffer(datagramsize_rx);
                if (!pakcopy) {
                    outputhw->counters.output_nobuffers++;
                    continue;
                }
                pak_copy(pak, pakcopy, pak->datagramsize);
                pakcopy->if_input   = inputsw;
                pakcopy->if_output  = outputsw;
                pakcopy->acct_proto = ACCT_PROTO_BRIDGE;
                pakcopy->linktype   = LINK_BRIDGE;
                break;

            case TBRPAK_VBR:    /* flood to virtual bridge not supported */
              action = TBRIDGE_TLB_UNSUPPORTED;
                continue;
#else
            default:
                continue;
#endif
            }

            /*
             * Convert packet and try to fastswitch it.  If translator returns
             * false then send the packet to srb_forward for explorer handling
             * or process switching.
             */
            action = tbridge_to_tring_inline(outputsw,
                                               pakcopy,TLB_BFR_TO_BFR);
            if (action == TBRIDGE_TLB_DONE)
                if (!((*outputhw->vbridge_forward)(pakcopy, outputhw)))
                    reg_invoke_srt_srb_forward(pakcopy, outputhw,
                                               pakcopy->riflen);
            break;
	default:
	    action = TBRIDGE_TLB_UNSUPPORTED;
	    break;

	} /* end of switch on outputsw->tbridge_media */

	switch (action) {

	case TBRIDGE_TLB_DONE:
	    switch (outputsw->tbridge_media) {
	      case TBR_ETHER_SDE:
	      case TBR_FDDI_SDE:
	      case TBR_CTR_SDE:
	      case TBR_TR_SDE:
	      case TBR_HDLC_TRANSIT_SDE:
		  outputsw->sde_said_db->egressing_dot10_packets[VLAN_TBRIDGE]++;
		  break;
	      default:
		  break;	  
	    }
	    break;
	case TBRIDGE_TLB_DEFERRED:
	    break;

	case TBRIDGE_TLB_NO_TXBUF:
	    outputhw->counters.output_total_drops++;
	    if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
		datagram_done(pakcopy);
	    continue;
	    break;

	case TBRIDGE_TLB_EXCEEDS_MTU:
	    outputhw->span_out_bigdrop++;
	    if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
		datagram_done(pakcopy);
	    continue;
	    break;

	case TBRIDGE_TLB_UNSUPPORTED:
	    if ((TBRPAK_RSP == code_switch) || (TBRPAK_BFR == code_switch))
		datagram_done(pakcopy);
	    continue;
	    break;
	}

	switch (code_switch) {
	case TBRPAK_RSP:
	case TBRPAK_BFR:
        case TBRPAK_VBR:
	    break;
#ifdef TBRIDGE_MCI_CBUS
	case TBRPAK_MCI:

	    if (outputhw->cbus_bridge_enable) {
		/*
		 * Upon encountering the first AB interface in the flood 
		 * list, initiate an off-complex flood.  The fact that it
		 * is off-complex is indicate by a non-zero value in the 
		 * argreg.This floods the packet on all AB interfaces in 
		 * the flood list, so we flip a boolean to indicate this 
		 * has been done, and any further AB interfaces in the
		 * flood list can be skipped.
		 */
		if (oif->tbifd_attributes & TBIFD_AUTO_FLOOD) {

		    if (outputhw->status & IDB_FDDI)
			tbridge_mci_auto_flood_inline(outputhw,
						      pakcopy->datagramsize |
						      MCI_TX_ODDALIGN);
		    else
			tbridge_mci_auto_flood_inline(outputhw,
						      pakcopy->datagramsize);
		    cbus_flood_done = TRUE;

		    if (tbridge_debug_cbus_flood)
			buginf("\n cBus off-complex flood initiated from %s",
			       inputhw->hw_namestring);
		}
	    }
	    break;
#else
	default:
	    break;
#endif
	}

#ifdef TBRIDGE_MCI_CBUS
	/* Account for transmitted packets that were flooded. */
	/* only on high end - low end fastsend routines do it for us */
	if (action != TBRIDGE_TLB_DEFERRED) 
	    /* if deferred, counted as process switched */
	    tbridge_tx_accounting_inline(outputhw, pakcopy->datagramsize);
#endif

        if (ISMULTICAST(addr_start->daddr))
            outputhw->counters.output_broad++;
    }

    /*
     * Didn't flush the packet if we did cbus copy assistance for
     * flooding. Flush it now since we are done.
     */

#ifdef TBRIDGE_MCI_CBUS
    if (span->cbus_copy_capable)
	tbridge_mci_flush_datagram_inline(inputhw);
#endif

    switch (code_switch) {
    case TBRPAK_RSP:
	break;				/* is this break needed?  will the packet get
					   freed? */
    case TBRPAK_BFR:
	datagram_done(pak);
	break;
#ifdef TBRIDGE_MCI_CBUS
    case TBRPAK_MCI:
    case TBRPAK_VBR:
	break;
#else
    default:
	break;
#endif
    }
}

#endif /* __TBRIDGE_INLINE_H__ */
