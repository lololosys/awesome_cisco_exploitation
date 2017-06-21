/* $Id: if_les_ether_inline.h,v 3.6.32.3 1996/07/11 14:20:26 fbordona Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_ether_inline.h,v $
 *------------------------------------------------------------------
 * if_les_ether_inline.h  Inline functions for interrupt processing
 *
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_ether_inline.h,v $
 * Revision 3.6.32.3  1996/07/11  14:20:26  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.6.32.2  1996/05/04  01:24:42  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 * - Supress reflection and keep physical broadcast in bridging fast-path
 *   for tms380.
 *
 * Revision 3.6.32.1  1996/03/18  20:42:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.1  1996/03/13  01:49:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6  1996/01/11  22:58:55  vrawat
 * CSCdi46632:  llc2 over ethernet broken
 *
 * Revision 3.5  1995/12/13  04:22:16  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/11/20  22:26:01  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  17:40:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:36:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * les_ethernet_getbuffer_inline
 * Try to get the buffer from pool cache or from interface buffer 
 * pool or from public pools in that order.
 */
static inline paktype *les_ethernet_getbuffer_inline (hwidbtype *idb, 
                                                      ulong byte_count)
{
    paktype *pak;

    pak = pool_dequeue_cache(idb->pool);
    if (!pak)
        pak = pool_getbuffer(idb->pool);
    if (!pak)
        pak = input_getbuffer(byte_count, idb);
    else
        set_if_input_inline(pak, idb->firstsw);

    return pak;
}

/*
 * les_ethernet_RX_interrupt
 * Process the received packet
 */
static inline void les_ethernet_RX_interrupt (hwidbtype *idb, paktype *pak)
{
    ushort rxtype;
    ether_hdr *ether;
    snap_hdr *snap;
    ulong byte_count;

    /*
     * Get pointer to the start of the packet.  The hardware
     * always assumes that packets are arpa encapsulated and
     * positions them appropriately.  Thus the magic number
     * here.
     */
    rxtype = pak->rxtype =
	ether_decode_encapsulation_inline(pak,
					  (ether_hdr *)pak->datagramstart);
    
    /*
     * Is remote monitoring enabled?  If so, analyze the packet.
     * If the packet was received only because rmon has placed
     * the interface in promiscous mode, the packet is consumed.
     */
    if (idb->rmon) {
	if (reg_invoke_rmon_input(idb, pak))
	    return;
    }

    /*
     * If this interface is listening promiscuously,
     * throw the packet at the software MAC filter in
     * the tbridge code where the bridge versus route
     * decision will be made.
     */
    
#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_pak_0) {
	idbtype *swidb;
	swidb = idb->firstsw;
	buginf("\nTB0: %04x %s on %s to %e %d bytes",
	       rmd_status,
	       rxtype_string(pak->rxtype), swidb->namestring,
	       ((ieee_addrs *)(pak->addr_start))->daddr,
	       byte_count);
    }
#endif	/* TBRIDGE_DEBUG */

    if (idb->tbridge_on_hwidb) {
	if (reg_invoke_tbridge_forward(pak))
	    return;
    }

    /*
     * Attempt to fast switch the packet.
     */
    if (reg_invoke_ether_fs(rxtype, pak)) {
	return; /* switched this packet; onto next packet */
    }
    
    ether = (ether_hdr *)pak->datagramstart;
    snap = (snap_hdr *)ether->data;
    
    /*
     * check for RSRB packets or DLSw Direct encaps packets
     */
    if (ether->type_or_len <= MAXETHERSIZE) {
	if (snap->type == TYPE_DLSW) {
	    if (reg_invoke_nim_dlsw_input(idb, pak))
		return;
	} else if (snap->type == TYPE_RSRB) {
	    if (reg_invoke_nim_vring_input(idb, pak)) 
		return;
	}
    }
    if (ether->type_or_len == TYPE_IP10MB) {
	iphdrtype *ip;
	
	ip = (iphdrtype *)ether->data;
	if (ip->prot == GRE_PROT || ip->prot == IPINIP_PROT) {
	    if (reg_invoke_tunnel_ip_les_fs(pak))
		return;
	}
    }
		
    /*
     * Count input broadcasts:
     */
    if (ether->daddr[0] & 1) /* 68000 dependent */
	idb->counters.input_broad++;
    
    /*
     * Since we received a packet, the interface is up:
     */
    if (!idb->ethernet_lineup) {
	idb->ethernet_lineup = TRUE;
	idb->reason = "Keepalive OK";
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    }

    /*
     * We did not fast switch the datagram.
     * Get ready to pass it up to process level:
     */
    byte_count = pak->datagramsize;

    
    /*
     * Do we need to change datagramstart? idb->parse_packet
     * will set datagramstart appropriately for the type of
     * data packet that was received.  If this is not the
     * same as the current location of the data packet, i.e.
     * its not an arpa style frame, then the data will need
     * to be copied into a new packet.
     */
#ifdef ALIGN_DATAGRAM
    (*idb->parse_packet)(idb, pak, (uchar *)&ether->type_or_len);
#else
    (*idb->parse_packet)(idb, pak, NULL);
#endif
    if (pak->datagramstart != (uchar *)ether) {
	paktype *tmp;
        idbtype *input;

        /* 
         * Before calling clear_if_input_inline(), save pak->if_input
         * so that we can do the IRB checking below.
         */ 
        input = pak->if_input;
        clear_if_input_inline(pak);
	tmp = pak;
        pak = les_ethernet_getbuffer_inline(idb, byte_count);
	if (pak) {
	    pak->datagramsize =byte_count;
	    (*idb->parse_packet)(idb,
				 pak,
				 (uchar *)&ether->type_or_len);
	    bcopy((uchar *)ether, pak->datagramstart, (uint)byte_count);
	    /*
	     * Integrated Routing and Bridging
	     * The input interface *may* have been overwritten to the BVI
	     * (Bridge-group Virtual Interface) in tbridging fast-switching
	     * code where it makes route/bridge decision
	     */
	    if (global_irb_enable && is_bvi(input->hwptr)) {
		change_if_input(pak, input);
		pak->irb_rcvd_input = tmp->irb_rcvd_input;
	    }
	    retbuffer(tmp);
	} else {
	    retbuffer(tmp);
	    return; /* next packet */
	}
    }

    idb->counters.rx_cumbytes += byte_count;
    idb->counters.inputs++;
    if (pak->enctype == ET_LLC2) {
	llc_input(pak);
    } else
	(*(idb->iqueue)) (idb, pak);
}

