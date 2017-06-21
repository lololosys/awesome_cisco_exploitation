/* $Id: tbridge_bvi.c,v 3.1.4.11 1996/08/15 23:45:28 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_bvi.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_bvi.c
 *
 * March, 1996 - Wilber Su
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Transparent Bridging Bridge-group Virtual Interface support
 * For Integrated Routing and Bridging(IRB) support
 *------------------------------------------------------------------
 * $Log: tbridge_bvi.c,v $
 * Revision 3.1.4.11  1996/08/15  23:45:28  wilber
 * CSCdi66123:  BVI code continues to use a packet after disposal
 * (datagram_done)
 * Fix the missing/misused returns
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/08/13  02:25:15  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.4.9  1996/08/03  23:39:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.4.8  1996/07/30  14:43:12  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.1.4.7  1996/07/17  21:39:38  wilber
 * CSCdi62840:  line protocol goes down on FE when irb is configured
 * - Check the condition that span may be NULL when we create BVI
 * - Catch and receive keepalive we sent when IRB is enabled
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/06/27  09:41:41  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.1.4.5  1996/06/01  01:32:43  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/05/21  10:05:48  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.3  1996/05/21  06:41:32  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1.4.2  1996/05/09  14:47:11  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1  1996/05/04  05:08:49  wilber
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
 * Revision 3.1  1996/03/09  03:04:09  wilber
 * Placeholders for Integrated Routing and Bridging(IRB)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "packet.h"
#include "config.h"
#include "../srt/srt_registry.h"
#include "../if/if_vidb.h"
#include "../if/network.h"
#include "../parser/parser_defs_parser.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../srt/span.h"
#include "../smf/smf_public.h"

/* For BVI ip arp setup*/
#include "../ip/ip_media.h"

/* For baddr - physical broadcast MAC address*/
#include "../h/ieee.h"

/* 
 * logger.h is needed here only because we want to include 
 * tbridge_pulbic.h, which is needed only because bvi_smf_xxx is 
 * defined in srt/bridge.c. Since this file and srt/bridge.c is 
 * within the same subsystem, we don't want to reg_invoke_bvi_smf_xxx
 * here - call them directly
 */
#include "logger.h"

#include "tbridge_public.h"
#include "tbridge_bvi.h"

/* For typedef of vcd_t, which is included in tbridge_common_inline.h */
#include "../if/atm.h"

#include "tbridge_common_inline.h"


/*
 * tbridge_bvi_getlink
 *
 * Called by raw_enqueue via the IDB dispatch routines.
 */

static long tbridge_bvi_getlink (paktype *pak)
{
    return(pak->linktype);	/* it was already set on the way out... */
}

/*
 * tbridge_bvi_oqueue
 *
 * Called by the IDB dispatch routines -- if this were a normal interface
 * we would put the packet on the appropriate output hold queue. However,
 * since this is a BVI(Bridge-group Virtual Interface), we don't need an 
 * output hold queue, instead of we should lookup the bridge table to set
 * the output interface and pass the packet to bridgeQ.
 * This is the most convenient place to steal the packet away from the 
 * normal code path.
 */

static boolean tbridge_bvi_oqueue (hwidbtype *hwidb, paktype *pak, 
				   enum HEADTAIL which)
{
    idbtype *vidb, *outputsw;
    btetype *dst_bte;
    spantype *span;
    ieee_addrs *addr_start;

    vidb = hwidb->firstsw; 
    outputsw = NULL;

    /*
     * Overwrite the input interface to the BVI before enqueuing
     * the packet to bridgeQ.
     * Reason is for bridging code, it could then figure out which
     * bridge group to bridge the packet; Otherwise, the input
     * interface is a routed interface, no bridging attributes
     * associated with it.
     */
    if (pak->if_input) {
	if (!change_if_input(pak, vidb)) {
	    datagram_done(pak);
	    return (TRUE);
	}
    } else {
	/*
	 * This is for the case that packet is generated by ourselves
	 * since change_if_input() assume we have valid pak->if_input.
	 * One example is the ARP packet which is sent out onto BVI 
	 */
	pak->if_input = vidb;
    }

    /*
     * The packet is in ether format since we make the BVI a virtual-ether.
     * Process-level bridging code knows it by checking the interface type
     * is a BVI or not;
     * Fast-switch bridging code knows this since we already set 
     * tbridge_media to TBR_ETHER when we create the BVI.
     * We don't want to set PAK_TBRIDGE_IN here to notify process-level
     * code that the packet looks like ether because there is a situation
     * that protcocol routing code fast-switched the packet to the BVI,
     * which will invoke tbridge fast-switching code, and because the output
     * is span_process_force, we need to enqueue it to bridgeQ; This case
     * we didn't get the chance to set the PAK_TBRIDGE_IN.
     * Checking specifically the interface type(BVI) is better...
     */

    /* hwidb->unit represents the bridge-group number */
    span = spanarray[hwidb->unit];
    if (!span || (span->irb_bg_vidb != vidb))
	return FALSE;
	
    addr_start = (ieee_addrs *)pak->addr_start;

    /*
     * Lookup the destination.
     */
    dst_bte = locate_bte_inline(span, &addr_start->daddr[0]);

    if (dst_bte) {
	if( (dst_bte->bte_action == BRIDGE_DISCARD) ||
	   (dst_bte->bte_action == BRIDGE_RECEIVE)) {
	    datagram_done(pak);
	    return (TRUE);
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
		    datagram_done(pak);
		    return (TRUE);
		}
	    }
	    /*
	     * If output interface utilizes bridgeptr, set it
	     * to avoid flooding the packet to all attached circuits.
	     */
	    if (is_x25(outputsw->hwptr)) {
		pak->bridgeptr = dst_bte->bte_proxy_addr.x25map_ptr;
	    } else if (is_frame_relay(outputsw->hwptr)
			|| is_atm_dxi(outputsw->hwptr)) {
		pak->bridgeptr = &dst_bte->bte_proxy_addr.fr_dlci;
	    } else if (is_fddi_br_encap(outputsw->hwptr)) {
		pak->bridgeptr = &dst_bte->bte_proxy_addr.proxy_mac;
	    } else if (is_smds(outputsw->hwptr) || is_atm(outputsw->hwptr)) {
		pak->bridgeptr = (void *)dst_bte->bte_mac_addr.lword;
	    }
	}
    }
    
    /*
     * Set output interface
     * Note that if we couldn't find the entry in bridge table, we 
     * clear the output to let bridging code flood it
     */
    pak->if_output = outputsw;

    pak->linktype = LINK_BRIDGE;
    process_enqueue_pak(bridgeQ, pak);
    return(TRUE);
}

/*
 * tbridge_bvi_creation_check
 * Check if a BVI(Bridge-group Virtual Interface) can be created or
 * comping up when user configure "int BVI <bridge-group>"
 * Spit out warning message and return FALSE if we didn't pass the
 * check
 */

static boolean tbridge_bvi_creation_check (uint bridge_group)
{

    /*
     * If the system is booting up or we are executing the batch command, 
     * bybass the check.
     */
    if (!systeminit_flag || reg_invoke_access_batch_cmd(ACCESS_BATCH_CHECK,FALSE)) {
	return (TRUE);
    }

    /*
     * Only after IRB(Integrated Routing and Bridging is enabled
     * then users are allowed to configure Bridge-group Virtual
     * Interface
     */
    if (!global_irb_enable) {
	printf("\nIntegrated Routing and Bridging is not configured!");
   	return (FALSE);
    }

    if ((bridge_group < 1) || (bridge_group >= SPAN_MAXINDEX)) {
	printf(badspan);
   	return (FALSE);
    }

    if (!spanarray[bridge_group]) {
	printf("\nBridge group %d is not configured", bridge_group);
	return (FALSE);
    }
    return (TRUE);
}

/*
 * tbridge_bvi_reset
 *
 * BVI(Bridge-group Virtual Interface) reset routine
 * BVI is a virtual interface, the only reason to create this is to
 * force protocol routines to call it when those protocols want to
 * add their interesting multicast address into address filter or SMF.
 * 
 * In BVI SMF populate/delete routines, we will take care of the task 
 * to populate/depopulate bridged interfaces' SMF
 */

static void tbridge_bvi_reset (hwidbtype *hwidb)
{
    reg_invoke_media_interesting_addresses(hwidb->firstsw);
}

/*
 * tbridge_bvi_set_maxdgram()
 *
 * Description:
 * Set the max datagram size for the Bridge-group Virtual Interface.
 *
 * Parameters:
 *  idb - BVI
 *  maxdgram - the new max datagram size for the BVI
 *
 * Returns:
 *  a boolean value reflecting the success of the change
 *
 * Called by parser ONLY when users want to configure the MTU
 */

static boolean tbridge_bvi_set_maxdgram (hwidbtype *hwidb, int buffersize, 
					 int maxdgram)
{

    /*
     * Ignore the buffersize - we don't need it.
     *
     * We already set hwidb->maxmtu in parser code
     */
    hwidb->max_pak_size_def = maxdgram;
    if_maxdgram_adjust(hwidb, maxdgram);

    /*
     * We want to make sure that hwidb->maxmtu_def set to the
     * right one because the membership of the bridge group might
     * have changed, the default mtu has to be adjusted.
     */
    tbridge_bvi_mtu_select(hwidb);

    return (TRUE);
}

/*
 * tbridge_bvi_mtu_select
 *
 * Set MTU for the BVI. Get called when
 * - BVI is created
 * - BVI is coming up
 * - Memebership of bridged interfaces changed
 */

void tbridge_bvi_mtu_select (hwidbtype *hwidb)
{
    
    idbtype *port;
    int	ii;
    spantype *span;
    ushort mtu;
    long delta;
    ulong maxdgram;

    /*
     * hwidb->maxmtu	: 	The current mtu, set by configuration 
     * 			       	or reselect by router itself
     * hwidb->max_buffer_size	: 	The current max (mtu + buffers)
     * 
     * hwidb->maxmtu_def:	The default mtu, set by router itself,
     *				is the mtu of a bridged interface
     *				which has the biggest mtu in the bridge
     *				group
     */

    /*
     * Intitialize. Will be set to bigger value if we can find one
     */
    mtu = hwidb->maxmtu_def = MAXETHERSIZE;
    maxdgram = ETHER_MAXDGRAM_DEF;

    /*
     * Select the default max mtu among the bridged interfaces as the
     * default mtu for the BVI
     *
     * Note that we figure out the default mtu *before* checking if
     * user has configured mtu or not because we always want to set
     * the default mtu to the biggest mtu among the bridged interfaces,
     * so when users deconfigure mtu('no mtu'), we have the correct
     * default.
     */
    span = spanarray[hwidb->unit];

    if (!span)
	return;

    for (ii = 0; ii < span->port_count; ii++) {
        port = span->port[ii];
        if (port->hwptr->maxmtu > mtu) {
            mtu = port->hwptr->maxmtu;
	    maxdgram = port->hwptr->max_buffer_size;
            hwidb->maxmtu_def = port->hwptr->maxmtu_def;
        }
    }

    /*
     * No subinterfaces for the BVI. Since there is a user configured
     * mtu, don't overwrite maxmtu and max_buffer_size.
     */
    if (hwidb->firstsw->user_configured_mtu)
        return;

    delta = mtu - hwidb->maxmtu;
    hwidb->maxmtu = mtu;
    hwidb->max_pak_size_def = maxdgram;
    if_maxdgram_adjust(hwidb, maxdgram);

    /*
     * Notify all protocols of a MTU change.
     * (Currently, IP, CLNS, Decnet)
     */
    reg_invoke_media_change_mtu(hwidb, hwidb->firstsw, delta);

    /*
     * Invalidate the caches.
     */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
}

/*
 * tbridge_bvi_mac_set
 *
 * Set(reinit) the MAC address of the BVI(Bridge-group Virtual Interface)
 * We set it to the MAC portion of BridgeID of the bridge group which
 * this BVI is associated with.
 * Also do some protocol specific cleanup
 */
static void tbridge_bvi_mac_set (spantype *span, idbtype *vidb)
{
    hwidbtype *hwidb = hwidb_or_null(vidb);

    ieee_copy(&span->bridge_id[2], hwidb->hardware);
    reg_invoke_media_hwaddr_notify(vidb, hwidb->hardware, IEEEBYTES);
}

/*
 * tbridge_bvi_fastsend
 *
 * BVI specific fastsend routine
 * This is different in the sense of doing the following steps:
 * 1. change input i/f to BVI
 * 2. call the reg_invoke_tbridge_forward() do the bridging
 */

static void tbridge_bvi_fastsend (hwidbtype *hwidb, paktype *pak)
{
    
    /*
     * Overwrite the input interface to the BVI.
     * Clear the output interface to let bridging code 
     * sort it out
     */
    pak->if_output = NULL;

    if (!change_if_input(pak, pak->if_input) || 
	!reg_invoke_tbridge_forward(pak)) {
	datagram_done(pak);
    }
}

/*
 * tbridge_bvi_createidb
 *
 * Create an IDB on the fly when it is first referenced by an
 * "interface BVI <bridge-group>" command.
 *
 */

static idbtype * tbridge_bvi_createidb (uint bridge_group)
{
    idbtype *vidb;
    hwidbtype *hwidb;
    spantype *span;
    uchar swapped_addr[IEEEBYTES];

    /*
     * Check if bridge-group number and span are valid
     */
    if (!tbridge_bvi_creation_check(bridge_group))
	return (NULL);

    vidb = vidb_malloc();
    if (!vidb) {
	printf(nomemory);
	return (NULL);
    }

    /*
     * Record the pointer of this BVI for that bridge group
     */
    span = spanarray[bridge_group];

    /*
     * Note that we may not have valid 'span' yet when we are booting
     * up since we could have no interface configured for bridging (so
     * we do not initialize span yet) but have bvi configured(which
     * required a valid span ptr). 
     * If this is true, bypass it now and tbridge_bvi_swif_comingup() 
     * which will be called later will take care of this case.
     */
    if (span) {
	span->irb_bg_vidb = vidb;
    }

    hwidb = vidb->hwptr;
    hwidb->fastsend = tbridge_bvi_fastsend;
    hwidb->status  	|= IDB_BVI;
    hwidb->type		= IDBTYPE_BVI;
    hwidb->state    	= IDBS_UP;
    hwidb->name     	= IFNAME_BVI;
    hwidb->typestring = "BVI";

    /*
     * Though BVI is a vitual interface, we need a hardware address to
     * pass the packet to the bridging code. and to let hosts/routers 
     * on bridged segments to be able to point at this MAC address.
     *
     * We need an unique MAC address for each BVI in the world, not just
     * unique to the box. The best way is to borrow the MAC address of 
     * bridgeID of that bridge group which this BVI is associated with. 
     */
    if (span) {
	tbridge_bvi_mac_set(span, vidb);
    }

    /*
     * Populate the bridged interface SMF(Software Macaddress Filter)
     * with BVI's borrowed mac addr 
     */
    bvi_smf_update(hwidb->hardware, vidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_BRIDGE);

    /*
     * If the bridged interface is token-ring, populate its SMF
     * with the bit-swapped version of BVI's mac address as well.
     * Reasons are,
     * - When BVI generats ARP response/request, the src mac of
     *   BVI *inside* ARP packet is in canonical format since BVI
     *   is virtual-ether; Bridging code will bit-swap the mac
     *   header but NOT the payload, so stations/routers on bridging
     *   token side will use this canonical mac as the destination
     *   mac to send to the BVI. Bridging code bit-swap the mac header
     *   from token before we get to SMF lookup, so we need to
     *   listen to this bit-swapped mac(in token format) for BVI as well.
     * - OR we could implement a flag/field in ARP cache to indicate
     *   the original received bridged interface is token, so when
     *   generating ARP request/response from the BVI, we know we have 
     * 	 to swap BVI's hardware address before inserting it into ARP
     *   packet. But couldn't achieve it without seriously touching 
     *   the ARP code and implementing kludy checks all over the places. 
     *   Furthemore, different protocols has different but similar
     *   ARPing mechanism. Don't want to touch all of them.
     */
    ieee_swap(hwidb->hardware, swapped_addr);
    bvi_smf_update(swapped_addr, vidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_TR);

    /*
     * Note that we don't need to call 
     * 	reg_invoke_media_interesting_addresses(vidb);
     * here to populate registered protocol interesting address
     * since we are just creating the BVI and hasn't read in the 
     * protocol attributes configured on the BVI
     */

    /*
     * Record what bridge-group this BVI is associated with
     */
    hwidb->unit     = bridge_group;
    hwidb->slotunit = bridge_group;
    hwidb->vc       = NO_VC;

    /* Virtual ether */
    set_default_interface_bandwidth(vidb, VISIBLE_ETHER_BANDWIDTH);

    idb_init_names(vidb, TRUE);

    /*
     * Set the fastswitch flavor
     */
    hwidb->fast_switch_type = FS_BVI;

    hwidb->iqueue   = raw_enqueue;

    /* 
     * This is intended for outbound(output to the BVI); 
     * Inbound traffic shouldn't use bvi->linktype to parse the 
     * link type
     */
    hwidb->getlink = tbridge_bvi_getlink;

    /*
     * Before we pass the packet with network-layer data to the
     * bridging code, we need to construct a frame with MAC-layer
     * header. Let's make it "virtaul ether", that is to construct
     * the ehter-like frame.
     * These routines also take care of Destination MAC resolution -
     * that is, lookup ARP cache, generate ARP request if necessary...
     */
    hwidb->vencap = ether_vencap;

    hwidb->deencap  = NULL;

    /* 
     * This is the piece where we hand the packet to bridging code to
     * lookup the bridge table and bridge it.
     */
    hwidb->oqueue   = tbridge_bvi_oqueue;
    hwidb->oqueue_dequeue = (oqueue_dequeue_t)return_null;

    /*
     * Since we never really transmit a packet out from a BVI
     * (we pass the packet to bridgingQ when we are supposed to
     * enqueue it to BVI's ouputQ), we don't need a soutput routine.
     */
    hwidb->soutput  = (soutput_t) return_nothing;

    hwidb->extract_hwaddr = (extract_hwaddr_t)return_false;
    hwidb->extract_snpa = (extract_snpa_t)return_false;

    /*
     * No link level broadcasts
     */
    hwidb->broadcast= (broadcast_t) return_false;

    hwidb->reset    = tbridge_bvi_reset;
    hwidb->periodic = (periodic_t) return_nothing;

    /*
     * Number it for SNMP 
     */
    hwidb->hw_if_index = idb_get_next_hw_index();
    hwidb->snmp_if_index = reg_invoke_ifmib_register_hwidb(hwidb);

    /*
     * Set up how ARP is done.
     */
    hwidb->enctype = ET_ARPA;
    vidb->arp_arpa = vidb->arp_arpa_def = TRUE;
    vidb->arp_probe = vidb->arp_probe_def = FALSE;
    vidb->arp_snap = vidb->arp_snap_def = FALSE;
    vidb->arp_timeout = ARP_MAXAGE;

    /*
     * For IP
     */
    vidb->ip_enctype = ET_ARPA;

    /* Virtual ether */
    vidb->tbridge_media = TBR_ETHER;

    /*
     * Interface now exists, put it on the idb queue.
     * Do this last so idb->tunnel is always valid.
     */
    idb_enqueue(hwidb);
    nnets++;			/* increment total number of interfaces */

    /*
     * For parser - User configure MTU for the BVI
     */
    hwidb->set_maxdgram = tbridge_bvi_set_maxdgram;

    /* 
     * Set default MTU
     */
    tbridge_bvi_mtu_select(hwidb);

    /*
     * Setup fast-switching
     * This is for protocols which initiate/setup fast-switching for
     * all swidbs when routing is configured. Because "xxx routing" is
     * read in before BVI is created when we readin the configuration, 
     * we need to reinitiate fast-switching flag for BVI.
     * IPX is one example
     */
    reg_invoke_fast_setup(hwidb);

    return(vidb);
}

/*
 * tbridge_bvi_parser_createidb
 *
 * Create an IDB on the fly when it is first referenced by an
 * "interface BVI <bridge-group>" command.
 *
 */

boolean tbridge_bvi_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_BVI) {
	csb->createidb = tbridge_bvi_createidb;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * tbridge_bvi_stay_shutdown
 *
 * For Bridge-group Virtual Interface only.
 * Called to see if the virtual interface should stay in the shutdown 
 * state. Return TRUE if the inteface should stay down.
 */

boolean tbridge_bvi_stay_shutdown (idbtype *swidb)
{
    if (!is_bvi(swidb->hwptr)) {
	/* Let it pass */
	return (FALSE);
    }

    if (!tbridge_bvi_creation_check(swidb->hwptr->unit)) {
	/*
	 * Didn't pass the check - stay shutdown.
	 */
	return (TRUE);
    } else {
	return (FALSE);
    }
}


/*
 * tbridge_bvi_swif_erase
 *
 * For Bridge-group Virtual Interface only.
 */

void tbridge_bvi_swif_erase (idbtype *swidb)
{
    spantype *span;
    hwidbtype *hwidb;
    uchar swapped_addr[IEEEBYTES];

    hwidb = hwidb_or_null(swidb);

    if (!is_bvi(hwidb)) {
	return;
    }

    span = spanarray[hwidb->unit];
    if (!span) {
	return;
    }

    /*
     * Decouple the BVI and the corresponding bridge group since
     * we are removing the BVI
     */
    span->irb_bg_vidb = NULL;

    /*
     * We shouldn't be worry about taking out protocol-related 
     * attributes from the BVI like ip address since protocols should 
     * already register their own swif_erase() routine to remove those 
     * attributes.
     */

    /*
     * Depopulate bridged interfaces' SMF with the BVI's borrowed MAC 
     * when the BVI is removed(in fact, shut-downed and moved to 
     * unused queue)
     * We didn't null all the attribute of BVI since it's unnecessary.
     * Save us some effort to reinitialize when it is comping back up
     */
    bvi_smf_delete(hwidb->hardware, swidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
			      IDB_BRIDGE);

    ieee_swap(hwidb->hardware, swapped_addr);
    bvi_smf_delete(swapped_addr, swidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_TR);

    /*
     * Null the BVI's MAC address
     */
    bzero(hwidb->hardware, IEEEBYTES);

    /*
     * Update registered protocol interesting address
     */
    reg_invoke_media_interesting_addresses(swidb);
}

/*
 * tbridge_bvi_swif_comingup
 *
 * For Bridge-group Virtual Interface only.
 */

void tbridge_bvi_swif_comingup (idbtype *swidb)
{
    tbridge_bvi_initialize(swidb);
}

/*
 * Function to (re)initialzed BVI. This 
 * function will be called at three places
 * when a BVI is coming up or when spanning
 * tree is generating a new bridge id or
 * when the mac manual override on bvi is
 * taking place
 */
void tbridge_bvi_initialize (idbtype *swidb)
{
    spantype *span;
    hwidbtype *hwidb;
    uchar swapped_addr[IEEEBYTES];
    
    hwidb = hwidb_or_null(swidb);

    if (!is_bvi(hwidb)) {
	return;
    }

    span = spanarray[hwidb->unit];
    if (!span) {
	return;
    }

    /*
     * If operator manual overriding has taken place,
     * then don't sync with the spanning tree ID
     * --- CSCdj10454  
     * the first clause tells us if there is no
     * hardware address for BVI, we should always use 
     * the spanning tree ID
     * the second clause tells us if a manual override
     * has taken place. If operator has given a forced-mac,
     * the forced_mac will always be the same as the hardware
     */
    if (is_ieee_zero(hwidb->hardware) ||
        ieee_compare(hwidb->hardware, hwidb->forced_mac) != 0) {
	/*
	 * Reinitialize BVI's MAC address according to span
	 */
    	tbridge_bvi_mac_set(span, swidb);
    } 

    /*
     * Record the pointer of the BVI for that bridge group again 
     * otherwise we only set irb_bg_vidb when we created the BVI 
     * and we may decoupled it already when we removed the 
     * bridge-group
     */
    (spanarray[hwidb->unit])->irb_bg_vidb = swidb;

    /*
     * Before reinitialize BVI's MAC address, remove the current one
     * from SMF if any
     */
    reg_invoke_bvi_smf_delete_by_result(swidb,
					SMF_MACTYP_BVI |
					SMF_ACTION_RECEIVE);
    
    /* 
     * Populate bridged interface's SMF with BVI's borrowed MAC
     */
    bvi_smf_update(hwidb->hardware, swidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_BRIDGE);

    /*
     * Populate the bit-swap version(in token format)
     */
    ieee_swap(hwidb->hardware, swapped_addr);
    bvi_smf_update(swapped_addr, swidb, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_TR);
    /*
     * Update registered protocol interesting address
     */
    reg_invoke_media_interesting_addresses(swidb);
}

/*
 * tbridge_bvi_macaddrs_update
 *
 * Called (via reg_invoke_media_interesting_addresses) when a
 * change in the listening state of an interface has taken place.
 * The interface is listening either selectively or promiscuously.
 *
 * When Integrated Routing and Bridged is configured and a BVI(Bridge-
 * group Virtual interface) exists, bridged interface needs to listen
 * to BVI's borrowed MAC address. Also if any protocol is (de)configured
 * on the BVI, we want to (de)populate bridged interface's SMF with
 * protocol's interesting multicast addresses.
 * 
 * Update the SMF so that BVI(Bridge Virtual Interface) borrowed MAC
 * addresses and protocol interesting multicasts are either inserted 
 * (the interface is listening promiscuously) or deleted (the interface
 * is listening selectively).
 *
 * This is for bridged interface, not for BVI.
 */
void tbridge_bvi_macaddrs_update (idbtype *idb)
{

    idbtype *bvi;
    uchar swapped_addr[IEEEBYTES];

    /*
     * If the idb is a BVI, we will return here since BVI is not
     * a bridged interface.
     */
    if (!global_irb_enable || !idb->span_ptr || 
	!idb->span_ptr->irb_bg_vidb)
	return;

    bvi = idb->span_ptr->irb_bg_vidb;

    bvi_smf_update(bvi->hwptr->hardware, bvi,
		   SMF_MACTYP_BVI | SMF_ACTION_RECEIVE, IDB_BRIDGE);

    ieee_swap(bvi->hwptr->hardware, swapped_addr);

    bvi_smf_update(swapped_addr, bvi, SMF_MACTYP_BVI | SMF_ACTION_RECEIVE,
		   IDB_TR);

    /*
     * Update registered protocol interesting address
     */
    reg_invoke_media_interesting_addresses(bvi);
}


