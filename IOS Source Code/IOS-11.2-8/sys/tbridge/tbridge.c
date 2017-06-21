/* $Id: tbridge.c,v 3.4.44.4 1996/08/28 13:16:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge.c
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Generic functions for transparent bridging.
 *------------------------------------------------------------------
 * $Log: tbridge.c,v $
 * Revision 3.4.44.4  1996/08/28  13:16:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.3  1996/05/09  14:47:09  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.44.2  1996/05/04  05:08:48  wilber
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
 * Revision 3.4.44.1.2.1  1996/04/27  07:26:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.44.1  1996/04/25  23:22:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.66.1  1996/04/08  14:49:50  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.64.1  1996/04/08  07:20:08  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.4  1996/01/05  04:57:17  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/08  07:31:25  mcieslak
 * CSCdi43513:  Bus Error Crash at tbridge_log_giant
 * Fix unaligned access to MEMD by using the GETLONG macro.
 *
 * Revision 3.2  1995/11/17  18:44:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:31:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/17  07:13:06  wilber
 * CSCdi38595:  Lat Compression Breaks Transparent Bridging
 * Should initialize swidb->span_latsize to -1 before checking if it is
 * a LAT packet and if we could compress it
 *
 * Revision 2.2  1995/06/20  03:02:03  speakman
 * CSCdi36088:  undefined tbridge references making trunkd.test
 * Remove tbridge_smf_... functions from tbridge.c to bridge.c
 * in the interest of the lite images that build with only
 * process-level transparent bridging.
 *
 * Revision 2.1  1995/06/07  23:03:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "mgd_timers.h"
#include "logger.h"
#include "../tbridge/msg_tbridge.c"	/* Not a typo, see logger.h */
#include "packet.h"
#include "config.h"

#include "interface_private.h"
#include "access.h"
#include "ieee.h"

#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../wan/smds.h"

#include "../srt/span.h"

#include "../if/atm.h"
#include "../atm/ilmi_api.h"

/* For Lan Emulation */
#include "../atm/atmsig_api.h"
 
#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"

#include "tbridge_public.h"
#include "tbridge_common_inline.h"


/*
 * tbridge_type_sap_filter
 *
 * Check packet type and lsap (where applicable) against our outstanding
 * filters. Return TRUE if the packet is cool, else return FALSE to
 * indicate that it can be ditched
 */

boolean
tbridge_type_sap_filter (paktype *pak, int lsap, int type)

{
    snap_hdr *link = (snap_hdr *)pak->info_start;

    if (link) {
	/*
	 * We've got a packet with an LLC header. Check the LSAP
	 * if a filter is enabled for it.
	 */
	if (lsap && !access_check_type(lsap, GETSHORT(&link->dsap)))
	    return(FALSE);

	/*
	 * If we've got an LLC header and the DSAP isn't SAP_SNAP
	 * then don't bother checking the type
	 */
	if (link->dsap != SAP_SNAP)
	    return(TRUE);
    }

    /*
     * We've got a type to check
     */
    if (type) {
	ushort packet_type = 0;

	/*
	 * Either pull the type field from the SNAP header or from
	 * the politically incorrect ARPA location.
	 */
	if (link)
	    packet_type = link->type;
	else
	    packet_type =
		GETSHORT(&((ether_hdr *)pak->addr_start)->type_or_len);

	if (!access_check_type(type, packet_type))
	    return(FALSE);
    }

    /*
     * The packet showed no discernable filter matches. It can pass.
     */
    return(TRUE);
}

/*
 * Function to stuff proxy table with mac address.
 * The parameter bte_created == TRUE, if BTE was newly created
 *                           == FALSE, otherwise.

 */

void
tbridge_record_proxy_src_addr (paktype *pak,
			       btetype *entry,
			       hwidbtype *inputhw,
			       boolean  bte_created)
{
    switch(pak->if_input->tbridge_media) {
    case TBR_FDDI_TRANSIT: {

	fddi_hdr *fddi = (fddi_hdr *) pak->datagramstart;

	/*
	 * Store the wire order address of the sending FDDI bridge
	 * in the proxy address table
	 */
	ieee_copy(fddi->saddr, (uchar *) &entry->bte_proxy_addr.proxy_mac);
    }
	break;

    case TBR_SMDS_TRANSIT: {
	/*
	 * Store the SMDS source address in the proxy address table
	 */
	PUTLONG(entry->bte_proxy_addr.proxy_mac,
		GETLONG(tbridge_dlli.smdssrc_ptr));
	PUTLONG(&entry->bte_proxy_addr.proxy_mac[4],
		GETLONG(&tbridge_dlli.smdssrc_ptr[4]));
    }
	break;
	
    case TBR_HDLC_TRANSIT:
	break;

    case TBR_FRAME_RELAY_TRANSIT:
	entry->bte_proxy_addr.fr_dlci = tbridge_dlli.circuit_id;
	break;

    case TBR_ATM_TRANSIT:
	entry->bte_proxy_addr.vc_num = tbridge_dlli.circuit_id;
	break;

    case TBR_LANE_ETHER: {

        vc_info_t *vc;
        vc = inputhw->atm_db->fast_vc_info;
	/*
         * If the entry is new learn the BUS VC.
         * This way, le-arp can be initiated and flush can be sent, as
         * we don't know at this time, if we are using the BUS path or not.
         */

	if (bte_created) {
	    vcd_t vcd;
	    
	    vcd = pak->if_input->lane_info->lec_info->mcast_send.vcd;
	    tbridge_lane_learn_source_inline(entry,
					     pak->if_input,
					     vcd,
					     BTE_LANE_FLOOD);
        } else {
	    /*
	     * If the BTE already exists, then learn the new interface only
	     * if the data came on a direct VC. If the entry is not in ready 
	     * state, just reset the interface and go on!
	     */
	    if (entry->bte_state == BTE_READY ) {
		if (vc->user_vc_type == LANE_LEC_DATA) {
		    tbridge_lane_learn_source_inline(entry,
						     pak->if_input,
						     tbridge_dlli.circuit_id,
						     BTE_READY);
		} 
	    }
	}
	break;
    }
    default:
	break;
    }
}


/*
 * Check to see if the incoming frame will allow LAT compression
 * Caller should have initialized outputsw->span_latsize to -1
 */

void
tbridge_check_lat_compression (idbtype *outputsw,
			       paktype *pak)

{
    hwidbtype *outputhw = outputsw->hwptr;

    /*
     * Compress only small packets.	
     */
    if (pak->datagramsize > MINETHERSIZE)
	return;
    
    /*
     * Check that the high 6 bits of the LAT type byte are zero,
     * indicating a normal data packet.  Then check to see how many
     * entries are in this LAT packet.
     */
    if (pak->network_start[0] & 0xFC)
	return;
    
    /*
     * If zero entries in this LAT packet, set new length now.
     * These are acknowledgements and are very common.
     */
    if (pak->network_start[1] == 0) {
	outputsw->span_latsize = LAT_ZEROSIZE + outputhw->span_encapsize;
	return;
    }
    
    /*
     * We are assuming (have not verified) that a short packet with one
     * entry is the next most common case and that we don't get much
     * benefit from compressing packets with more entries.
     */
    if (pak->network_start[1] != 1)
	return;
    outputsw->span_latsize = LAT_ZEROSIZE + outputhw->span_encapsize +
	                   LAT_ENTRYOVERHEAD + pak->network_start[10];
}


/*
 * Account for and log a packet too big for copying.
 */

void tbridge_log_giant (hwidbtype *idb, int length, int mtu, paktype *giant)
{
    ieee_addrs *hdr = (ieee_addrs *) giant->addr_start;

    idb->span_in_bigdrop++;
    errmsg(TBRIDGE_ERR_GIANT, idb->hw_namestring, length, mtu,
	   hdr->daddr, hdr->saddr,
	   /* the first 4 bytes following the DA/SA pair */
	   GETLONG(hdr + 1));
}

/*
 * tbridge_print_message - print the bridging message for Print_Hardware()
 */

void tbridge_print_message ()
{
    if (bridge_enable)
	printf("Bridging software.\n");
}

btetype *
tbridge_locate_bte (spantype *span, uchar const *addr)
{
    int hash;
    btetype *bte;
    uchar *bte_addr;
    leveltype level;

    hash = addr[MAC_HASH1] ^ addr[MAC_HASH2];

    level = raise_interrupt_level(NETS_DISABLE);

    for (bte = span->bridge_tbl[hash]; bte; bte = bte->bte_next) {
	bte_addr = (uchar *)bte->bte_mac_addr.sword;
	if ((GETLONG(&bte_addr[2]) == GETLONG(&addr[2])) &&
	    (GETSHORT(&bte_addr[0]) == GETSHORT(&addr[0])))
	    break;
    }
    reset_interrupt_level(level);
    return(bte);
}
