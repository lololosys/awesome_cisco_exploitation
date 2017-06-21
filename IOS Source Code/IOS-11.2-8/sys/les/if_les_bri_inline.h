/* $Id: if_les_bri_inline.h,v 3.3.12.3 1996/06/01 01:32:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri_inline.h,v $
 *------------------------------------------------------------------
 * if_les_bri_inline.h -- Definitions for BRI inline routines. 
 *
 * November 1994, Greg Stovall
 *	
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_bri_inline.h,v $
 * Revision 3.3.12.3  1996/06/01  01:32:02  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.3.12.2  1996/04/22  21:51:45  gtaylor
 * CSCdi54554:  Bridging over isdn leased-line broken for HDLC
 * encapsulation.
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/03/18  20:42:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:12:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/13  01:49:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:41:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/15  18:02:31  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.2  1995/11/17  17:40:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:32:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/10  20:27:44  agh
 * CSCdi38551:  Adding BRI fastswitching to sapphire
 *
 * Revision 2.1  1995/06/07  21:35:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * =====================================================================
 * bri_RX_interrupt()
 *
 * Description:
 * Take a bri packet and pass it up to process level. If the packet
 * is for a B channel, set the encapsulation type in the pak struct.
 * Bridge the packet if it is a transit packet.
 * The bri interrupt routines are called from the HW specific interrupt
 * handler. The calling interrupt handler handles all ring and buffering
 * issues. The this interrupt handler need only handle bri related issues.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * pak - pointer to the received packet
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static inline void bri_RX_interrupt (hwidbtype *idb, paktype *pak)
{
    idbtype *dchan_swidb;
    ushort rxtype;

    echotype *ep;
    
    /*
     * See if this bri interface is under system loopback(internal) or plug 
     * loopback test(external). If so, queue the packet in loopback queue. 
     */
    if (idb->isdn_info->loopback_mode) {
	ep = (echotype *) test_bri_echoQ.qhead; 
	ep->active = FALSE;
	pak_enqueue(&test_bri_loopbackQ, pak);
	idb->counters.rx_cumbytes += pak->datagramsize;
	idb->counters.inputs++;
	return;
    }

    if(ISBRILEASED(idb))
        dchan_swidb = idb->firstsw;
    else
        dchan_swidb = idb->isdn_info->d_chan_idb->firstsw;

    if (idb->isdn_info->chantype == ISDN_Dchan
		&& (!ISBRILEASED(idb))) {
        /*
         * D channel packet, send it straight to process level
         */
        serial_queue_for_process(idb, pak);
    } else { /* B channel packet */
        switch (idb->enctype) {
        case ET_HDLC:
            rxtype = hdlc_decode_encapsulation_inline(pak, 
                                          (vhdlctype *)(pak->datagramstart));
	    if (reg_invoke_hdlc_fs(rxtype, pak))
		return;
            break;
        case ET_PPP:
            rxtype = ppp_decode_encapsulation_inline(pak,
                                          (ppp_hdr *)(pak->datagramstart));
	    if (reg_invoke_ppp_fs(rxtype, pak))
                return;
            break;
        default:
            rxtype = RXTYPE_UNKNOWN;
	    break;
        }

        if ((rxtype == RXTYPE_TRANSIT) && RUNNING_IF(LINK_BRIDGE, dchan_swidb)) {
            pak->enctype = ET_BRIDGE;
            if (!change_if_input(pak, dchan_swidb)) {
                bri_drop_pak(idb, pak);
		return;
            }
            if (reg_invoke_tbridge_forward(pak))
		return;
        }
        serial_queue_for_process(idb, pak);
    }
}
