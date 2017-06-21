/* $Id: tbridge_common_inline.h,v 3.2.62.3 1996/06/28 23:26:35 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_common_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_common_inline.h
 *
 * September 1995, Badri Ramaswamy
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Common inline code used across tbridge/srt directories.
 *------------------------------------------------------------------
 * $Log: tbridge_common_inline.h,v $
 * Revision 3.2.62.3  1996/06/28  23:26:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/04  05:08:54  wilber
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
 * Revision 3.2.62.1  1996/04/25  23:22:46  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.66.1  1996/03/30  02:14:17  rzagst
 * ATM Modularity: vcd should be type vcd_t not ushort.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.2  1995/11/17  18:44:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:31:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/14  16:23:03  rbadri
 * Placeholder files for Arkansas Bridged ELANs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * The following two functions are used from both process level code
 * as well as fast bridging path. Since same piece of code was duplicated
 * a common inline improves code readability.
 */


/*
 * A common inline function to create and initialize Bridge Table
 * Entries. 
 * Assumption: Interrupt protection is handled by the caller.
 */
static inline
btetype *create_bte_inline (spantype *span, uchar  *addr,idbtype *idb )
{
    int hash;
    btetype *bte;

    bte = dequeue(&bteQ);
    if (bte) {
	bte_count--;
	ieee_copy(addr, (uchar *)&bte->bte_mac_addr);
	bte->bte_action = BRIDGE_FORWARD;
	bte->bte_interface = idb;
	/*
	 * Bridge table entries are assumed to be zeroed
	 * upon creation/release so no initialization is
	 * required. 
	 */
	hash =  addr[IEEEBYTES-2] ^ addr[IEEEBYTES-1];	       
	bte->bte_next = span->bridge_tbl[hash];
	span->bridge_tbl[hash] = bte;
	bte->bte_state = BTE_READY;
	GET_TIMESTAMP(bte->bte_timer);
    }
    return (bte);
}


/*
 * Inline function to record some of the attributes of BTE
 * Assumption: Interrupt protection is handled by the caller.
 */
static inline
void tbridge_lane_learn_source_inline (btetype *bte, 
				       idbtype *idb,
				       vcd_t  vcd,
				       ushort  bte_state)
{

   bte->bte_interface = idb;
   bte->bte_proxy_addr.vc_num = vcd;
   bte->bte_state = bte_state;

   return;
}


/*
 * tbridge_get_arpa_type_inline
 *
 * Returns the ARPA type for a frame from either the usual ARPA place
 * or from a SNAP header
 */

static inline
ushort tbridge_get_arpa_type_inline (paktype *pak)

{
    snap_hdr *link = (snap_hdr *)pak->info_start;

    if (link) {
        /*
         * It's an LLC style header. If it's not SNAP, just return 0
         */
        if (GETSHORT(&link->dsap) != (SAP_SNAP | SAP_SNAP << 8))
            return(0);
        else
            return(GETSHORT(&link->type));
    } else {
        /*
         * We have to assume that it's an ARPA style frame
         */
        return(GETSHORT(&((ether_hdr *)pak->addr_start)->type_or_len));
    }
}


static inline
btetype *locate_bte_inline (spantype *span, uchar const *addr)
{
    int hash;
    btetype *bte;
    uchar *bte_addr;

    hash = addr[4] ^ addr[5];

    for (bte = span->bridge_tbl[hash]; bte; bte = bte->bte_next) {
        bte_addr = (uchar *)bte->bte_mac_addr.sword;
        if ((GETLONG(&bte_addr[2]) == GETLONG(&addr[2])) &&
            (GETSHORT(&bte_addr[0]) == GETSHORT(&addr[0])))
            break;
    }
    return(bte);
}

/*
 * tbridge_irb_pak_processing_inline
 *
 * Inline function to
 * - Record the original received bridged interface in pak
 * - Overwrite the input interface to the BVI to let routing/ARP
 *   code think this packet is coming from the BVI
 *
 * This is the processing for IRB(Integrated Routing and Bridging)
 * packet before we pass the packet to routing/ARP code from
 * bridging fast-switching code
 */

static inline
boolean tbridge_irb_pkt_processing_inline (paktype *pak)
{
    idbtype *bvi;

    /* 
     * Record the received interface
     */
    pak->irb_rcvd_input = pak->if_input;

    /*
     * Get the BVI(Bridge-group Virtual Interface) for this
     * bridge-group
     */
    bvi = pak->if_input->span_ptr->irb_bg_vidb;
    if (bvi) {
	/* Overwrite the input to bvi */
        if (!change_if_input(pak, bvi)) {

#ifdef TBRIDGE_DEBUG
            if (tbridge_debug_dis_1) {
                buginf("\nTB1: discarded IRB packet %s from %s to %e\n",
                       rxtype_string(pak->rxtype), 
		       pak->irb_rcvd_input->namestring,
		       ((ieee_addrs *)pak->addr_start)->daddr);
	    }
#endif  /* TBRIDGE_DEBUG */

            return (FALSE);
        }
        /* Need to account for packet coming from tbridge fast input on
         * original hardware interface before the rewrite. CSCdi92583 (1)
         */
        bvi->hwptr->counters.inputs_fast[ACCT_PROTO_BRIDGE]++;
        bvi->hwptr->counters.rx_cumbytes_fast[ACCT_PROTO_BRIDGE] += 
        pak->datagramsize;
    }    
    return (TRUE);
}

