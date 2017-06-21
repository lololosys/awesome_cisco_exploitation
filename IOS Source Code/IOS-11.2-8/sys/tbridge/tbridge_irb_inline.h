/* $Id: tbridge_irb_inline.h,v 3.1.4.4 1996/09/10 02:38:24 hou Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_irb_inline.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_irb_inline.h
 *
 * March, 1996 - Wilber Su
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Generic inline functions for Integrated Routing and Bridging (IRB)
 *------------------------------------------------------------------
 * $Log: tbridge_irb_inline.h,v $
 * Revision 3.1.4.4  1996/09/10  02:38:24  hou
 * CSCdi63727:  IP RIP routing doent work with 802.10 over FDDI
 * Branch: California_branch
 * - distinguish 802.10 vLAN routing packet from bridging packet so that
 *   the  multicast routing pkts can go to 802.10 vLAN routing stack.
 *
 * Revision 3.1.4.3  1996/08/27  08:28:11  wilber
 * CSCdi67271:  atalk bridging failed when irb is enabled
 * Should explicitly clear pakcopy->if_output
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/23  22:18:15  wilber
 * CSCdi58505:  802.10(SDE) encaped spanning tree doesnt work
 * Should parse sde header before Software Mac-address Filter lookup
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/04  05:09:04  wilber
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
 * Revision 3.1  1996/03/09  03:04:11  wilber
 * Placeholders for Integrated Routing and Bridging(IRB)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TBRIDGE_IRB_INLINE_H__
#define __TBRIDGE_IRB_INLINE_H__


/*
 * Given a packet with at least a DA, an rxtype, and an input interface,
 * determine whether to bridge or route this packet.  Packets to be
 * routed will be returned with TBRIDGE_IRB_RECEIVE.  Packets to be
 * bridged will be returned with TBRIDGE_IRB_FORWARD.  Packets to be
 * discarded for any reason will be returned with TBRIDGE_IRB_DISCARD.
 *
 * The action of this function depends critically on the contents of the
 * Software MAC Address Filter (SMF), and on the contents of rxtype_flags.
 * Both of these are per-interface data structures associated with the
 * first swidb of an interface.  See smf/smf.c and all the xxx_irb_update()
 * functions where xxx is apollo, atalk, clns, decnet, ip, novell, vines
 * and xns, as well as reg_invoke_irb_update().
 */

STATIC INLINE
enum TBRIDGE_IRB_RETVAL
tbridge_irb_inline (paktype *pak, idbtype *inputsw)

{
    ulong smf_result, sde_result;
    ushort arpa_type;
    paktype *pakcopy;
    ieee_addrs *addr_start;

    /*
     * Parse 802.10 SDE encapsulated packets to establish
     * pak->if_input and pak->info_start for these packets.
     */

    if (pak->info_start) {
	sap_hdr *llc = (sap_hdr *)pak->info_start;
	
        if (GETSHORT(&llc->dsap) == SAPSAP_SDE) {
	    sde_result = tbridge_parse_sde_header_inline(pak);
            switch(sde_result) {
              case SDE_SUBIF_BRIDGE:
		  pak->if_input->sde_said_db->ingressing_dot10_packets[VLAN_TBRIDGE]++;
              case SDE_MAINIF_BRIDGE:
                  break;
              case SDE_SUBIF_ROUTE:
                  return (TBRIDGE_IRB_RECEIVE);
                  break;
              case SDE_DISCARD:
                  return (TBRIDGE_IRB_DISCARD);
                  break;
            }

	    /*
	     * Overwrite inputsw
	     * Overwrite tbridge_dlli.src_media_type to true colors.
	     */
	    inputsw = pak->if_input;
	    tbridge_dlli.src_media_type = pak->if_input->tbridge_media;
	}
    }

    /*
     * The DA is assumed to be canonical, but this is a hold
     * over from the time when these addresses were matched
     * in the bridge table.  With a per-interface SMF, this
     * convention could be relaxed, reducing the overhead
     * of swapping and unswapping non-canonical addresses
     * at least until we actually decide what to with a given
     * packet.
     */
    addr_start = (ieee_addrs *)(pak->addr_start);

    tbridge_dlli.dlli_attributes &= (~(DLLI_IRBPKT | DLLI_IPMCAST));

    /*
     * This is a function call to keep the caller insulated
     * from the nitty gritty of the SMF.  The search is typically
     * short due to the small number of addresses in the SMF
     * and a choice of a hash function designed to result in
     * chains of unit length.
     */
    smf_result = smf_match_address_inline(addr_start->daddr, inputsw);

    /*
     * We want to deal with misses most efficiently, so enter
     * the switch only with match results.
     */
    if (smf_result) {
	switch (smf_result & SMF_ACTION_MASK) {

	case SMF_ACTION_RECEIVE:

            /*
             * This packet is pointed at our unicast, an interesting
             * multicast, or the broadcast address.
             */
            if (!global_irb_enable) {
                /*
                 * Receive the unicasts, any interesting multicasts, but
                 * only those broadcasts that are in a protocol we are
                 * also routing on this interface.  This logic is intended
                 * to keep as much broadcast bridging as possible in the
                 * fast path.
                 */

                if ((!(smf_result & SMF_MACTYP_PHYS_BCAST)) ||
                    ROUTING_RXTYPE(inputsw, pak->rxtype)) {
		    
#ifdef TBRIDGE_DEBUG    /* see the monument in tbridge_debug_flags.h !!! */
		    if (tbridge_debug_rcv_1) {
			buginf("\nTB1: SMF received %s from %s to %e\n",
			       rxtype_string(pak->rxtype),
			       inputsw->namestring, addr_start->daddr);
		    }
#endif  /* TBRIDGE_DEBUG */

                    /*
                     * This packet is:
                     *  - pointed at the interface's unicast or
                     *  - pointed at an interesting multicast or
                     *  - pointed at the broadcast address AND we are
                     *    routing the protocol
                     * Receive it.
                     */
                    return(TBRIDGE_IRB_RECEIVE);
		}
                /*
                 * This packet is pointed at the broadcast address BUT
                 * we are NOT routing the protocol on this interface.
                 * Bridge it.
                 */
                return(TBRIDGE_IRB_FORWARD);

            } else {

                /*
                 * For Integrated Routing and Bridging(IRB).
		 *
		 * IRB is supported on packet-memory platforms only.
		 * It is NOT supported on AGS+ and 7000. Rely on parser
		 * to enforce it, we don't check platform here.
                 */

#ifdef TBRIDGE_DEBUG    /* see the monument in tbridge_debug_flags.h !!! */
                if (tbridge_debug_rcv_1) {
                    buginf("\nTB1: SMF received %s from %s to %e\n",
                           rxtype_string(pak->rxtype),
                           inputsw->namestring, addr_start->daddr);
		}
#endif  /* TBRIDGE_DEBUG */

		/*
		 ************************************************
		 * - IRB is enabled
		 * - Points at this box(a matched entry in SMF)
		 ************************************************
		 */

		/*
		 * If we are not routing this protocol, we should
		 * self-receive this packet without overwriting
		 * the input interface to the BVI. Examples are
		 * loopback and STP BPDUs.
		 *
		 * Exceptions are ARP; We never route ARP but we 
		 * want to overwrite the input interface to the BVI 
		 * if we are IRBing this protocol. Need special check
		 * for ARP.
		 */
		arpa_type = tbridge_get_arpa_type_inline(pak);

		if (!ROUTING_RXTYPE(inputsw, pak->rxtype)) {
		    if ((arpa_type != TYPE_RFC826_ARP) &&
			(arpa_type != TYPE_AARP)) {
			/*
			 * - IRB is enabled
			 * - Points at this box(a matched entry in SMF)
			 * - Not routing this rxtype
			 * - Not (ip, atalk) ARP
			 */
			if (smf_result & SMF_MACTYP_PHYS_BCAST) {
			    /* 
			     * - Physical broadcast 
			     */
			    if (BRIDGING_RXTYPE(inputsw, pak->rxtype)) {
				/* 
				 * - Bridging this rxtype 
				 */
				return (TBRIDGE_IRB_FORWARD);
			    } else {
				/* 
				 * - Not bridging this rxtype 
				 */

#ifdef TBRIDGE_DEBUG    
				if (tbridge_debug_dis_1) {
				    buginf("\nTB1: discarded bridged packet"
					   " %s from %s to %e\n",
					   rxtype_string(pak->rxtype),
					   inputsw->namestring, 
					   addr_start->daddr);
				}
#endif  /* TBRIDGE_DEBUG */

				return (TBRIDGE_IRB_DISCARD);
			    }
			} else {
			    /* 
			     * - Unicast or interesting multicast 
			     */
			    return (TBRIDGE_IRB_RECEIVE);
			}
		    }
		}

		/*
		 ************************************************
		 * - IRB is enabled
		 * - Points at this box(a matched entry in SMF)
		 * - Routing this rxtype OR (ip, atalk) ARP 
		 ************************************************
		 */

		if (!BRIDGING_RXTYPE(inputsw, pak->rxtype)) {
		    /*
		     * - IRB is enabled
		     * - Points at this box(a matched entry in SMF)
		     * - Routing this rxtype OR (ip, atalk) ARP 
		     * - Not bridging this rxtype
		     *
		     * => Just route it without overwriting the
		     * input interface to the BVI 
		     *
		     * We are NOT IRBing this rxtype; We are just 
		     * normal routing it; All network attributes
		     * associated with this rxtype are on received
		     * interface(which is a bridged interfaces but
		     * we are not bridging this rxtype) rather than
		     * on the BVI.
		     */
		    return (TBRIDGE_IRB_RECEIVE);
		}

		/*
		 * We bridge and do not route ARP in any cases.
		 *
		 * Special check for ARP to decide overwrite
		 * the input interface to the BVI or not.
		 */
		if (arpa_type == TYPE_RFC826_ARP) {
		    if ((!ROUTING_IF(inputsw, LINK_IP)) || 
			(!BRIDGING_IF(inputsw, LINK_IP))) {
			/*
			 * If we are not routing OR not bridging the 
			 * protocol, we don't want to overwrite the 
			 * input interface.
			 *
			 * Only when we are IRBing(both bridge and route)
			 * IP or Atalk, we overwrite ARP's input interface 
			 * to the BVI.
			 *
			 * Four cases(IP)
			 *
			 * Route Bridge
			 * ----- -------
			 *   x	   x	: Overwrite to BVI
			 *   -	   x	: Not
			 *   x	   -	: Not
			 *   -	   -	: Not
			 *
			 */
			return (TBRIDGE_IRB_RECEIVE);
		    }
		} else if (arpa_type == TYPE_AARP) {
		    if ((!(inputsw->tbridge_route_linktypes & 
			  LINK_APPLETALK_RBI)) ||
			(inputsw->tbridge_bridge_linktypes & 
			 LINK_APPLETALK_RBI)) {
			return (TBRIDGE_IRB_RECEIVE);
		    }
		}

		/*
		 ************************************************
		 * - IRB is enabled
		 * - Points at this box(a matched entry in SMF)
		 * - Routing this rxtype OR (ip, atalk) ARP 
		 * - Bridging this rxtype
		 ************************************************
		 */

		if (!(smf_result & (SMF_MACTYP_OWN_UNICAST |
				    SMF_MACTYP_BVI))) {
		    /*
		     * - IRB is enabled
		     * - Points at this box(a matched entry in SMF)
		     * - Routing this rxtyep OR (ip, atalk) ARP 
		     * - Bridging this rxtype
		     * - Interesting multicast OR physical broadcast
		     *
		     * =>	Make a copy and bridge the copy in 
		     * 	process-level
		     *  
		     * Examples: routing update(e.g. EIGRP) or 
		     * Atalk ARP with multicast DA; Or IP ARP 
		     * with physical bradcast DA
		     */
		    short dgsize = pak->datagramsize +
			tbridge_dlli.transit_encapsize;
		    
		    pakcopy = pak_clip(pak, dgsize);
		    if (pakcopy) {
			pakcopy->if_input = inputsw;
			pakcopy->linktype = LINK_BRIDGE;
			pakcopy->acct_proto = ACCT_PROTO_BRIDGE;
			pakcopy->bridgeptr = NULL;
			pakcopy->if_output = NULL;

			/*
			 * Bridge this copy in process level
			 */
			reg_invoke_bridge_enqueue(pakcopy, FALSE,
						  FALSE);
		    }
		}

		/*
		 **************************************************
		 * - IRB is enabled
		 * - Points at this box(a matched entry in SMF)
		 * - Routing this rxtype OR (ip, atalk) ARP 
		 * - Bridging this rxtype
		 * - Have enqueued the copy to bridgeQ for 
		 *	multicast/broadcast
		 **************************************************
		 */

		/*
                 * Mark in dlli_attributes to indicate that this is
                 * an irb packet because
                 * - we are both bridge and route this protocol
                 * OR
                 * - ARP packet of the protocol which we are both 
		 * 	bridge and route
                 *
                 * We want to record the original received bridged
                 * interface in pak and overwrite the input interface
		 * to the BVI after we learn the source mac
                 */
                tbridge_dlli.dlli_attributes |= DLLI_IRBPKT;

		/*
		 ************************************************
		 * - IRB is enabled
		 * - Points at this box(a matched entry in SMF)
		 * - Routing this rxtype OR (ip, atalk) ARP 
		 * - Bridging this rxtype
		 * - Have enqueued the copy to bridgeQ
		 * - Have marked dlli irb packet flag
		 ************************************************
		 */

		/* 
		 * Fall through tbridge forwarding code to learn
		 * the source mac and check filter/access-list
		 */
		return (TBRIDGE_IRB_FORWARD);
	    }
            break;


        /*
         * The SMF could someday be used to provide generalized
         * source and destination MAC address filtering as currently
         * but brokenly provided by the bte permission bits.
         * These two cases are in anticipation of that day, my gift
         * to the next generation.
         */
	case SMF_ACTION_FORWARD:
	    return(TBRIDGE_IRB_FORWARD);
	    break;

	case SMF_ACTION_DISCARD:
	    return(TBRIDGE_IRB_DISCARD);
	    break;

	default:
	    /*
	     * The SMF facility does not permit invalid actions,
	     * but just in case, discard these packets.
	     */
	    errmsg(TBRIDGE_ERR_SMF_ACTION, addr_start->daddr,
		   inputsw->namestring, smf_result);
	    return(TBRIDGE_IRB_DISCARD);
	    break;
	}
    }

    /*
     * Someday, the SMF should do partial matches.  In the meantime ...
     */

    if (ISMULTICAST(addr_start->daddr)) {
	if ((GETLONG(addr_start->daddr) & OUI_MASK) == IPMULTICAST_EP) {
	    tbridge_dlli.dlli_attributes |= DLLI_IPMCAST;
	} else if (ieee_equal(addr_start->daddr, cgmp_address)) {
	    tbridge_dlli.dlli_attributes |= DLLI_IPMCAST;
	}
	/*
	 * branch off another packet for m'cast code when packet 
	 * is incoming from a non-bvi i/f
	 * o global_irb_enable  : We only duplicate packet
	 * 			  in IRB mode since it's
	 * 			  the only situation we might bridge
	 *			  and route packet for a given
	 *			  protocol
	 * o !is_bvi	        : This packet is actually from a 
	 *			  routing i/f. Don't need to send
	 *			  it back again
	 * o DLLI_IPMCAST	: Only enqueue IP packet to ip queue
	 * o ROUTING_RXTYPE	: Only send it when we are routing
	 *			  this packet
	 */
	if (global_irb_enable &&
	    !is_bvi(inputsw->hwptr) && 
	    (tbridge_dlli.dlli_attributes & DLLI_IPMCAST) &&
	    ROUTING_RXTYPE(inputsw, pak->rxtype)) {

	    /*
	     * make a copy. enqueue the new packet into the IP 
	     * queue. Let the old flow through code. So
	     * this packet can be bridged out such that other
	     * m'cast router can see it.
	     */ 
	    short dgsize = pak->datagramsize +
		tbridge_dlli.transit_encapsize;
	    short network_offset = pak->network_start - pak->datagramstart;
		    
	    pakcopy = pak_clip(pak, dgsize);

	    if (pakcopy) {

		pakcopy->if_input = inputsw;
		pakcopy->network_start = pakcopy->datagramstart + 
				network_offset;
		pakcopy->enctype = pak->enctype;
		pakcopy->encsize = pak->encsize;
		pakcopy->linktype = LINK_IP;
		pakcopy->acct_proto = ACCT_PROTO_IP;
		pakcopy->bridgeptr = NULL;
		pakcopy->if_output = NULL;

		if (!tbridge_irb_pkt_processing_inline(pakcopy)) {
		    datagram_done(pakcopy);
		}
		else {
		    /*
		     * Send to IP queue and let IP dispatch this
		     * packet
		     */
		    reg_invoke_raw_enqueue(LINK_IP, pakcopy);
		}
	    }
	}
    }

    /*
     * We get here with most packets: bridgeable unicasts
     * that missed the filter:
     */

    /*
     * Shed any unicasts or multicasts in protocols we are
     * routing on this interface but that are not pointed at us.
     * This shedding is probably one of the first things to
     * handle differently to support integrated routing and
     * bridging (IRB).
     *
     * If IRB is enabled, bridge or route a unicast or multicast
     * which is NOT pointed at us of a given protocl is independent.
     *
     * Four cases,
     *
     * Route	Bridge
     * ------	-------
     *   x	   x	: Bridge it
     *   -	   x	: Bridge it
     *   x	   -	: Discard it
     *   -	   -	: Discard it
     *
     * So as long as we are bridging this protocol, we don't want
     * to discard this; As long as we are NOT bridging this protcol,
     * we want to discard it regardless we are routing this protcol
     * on this interface or not. We always check if we are bridging 
     * this protcol on this interface or not regardless if IRB is
     * enabled or not.
     */
    if (!global_irb_enable && ROUTING_RXTYPE(inputsw, pak->rxtype)) {

        if (inputsw->ip_mrouting) {
	    if (tbridge_dlli.dlli_attributes & DLLI_IPMCAST) {

#ifdef TBRIDGE_DEBUG    /* see the monument in tbridge_debug_flags.h !!! */
                if (tbridge_debug_rcv_1) {
                    buginf("\nTB1: IPM received %s from %s to %e\n",
                           rxtype_string(pak->rxtype),
                           inputsw->namestring, addr_start->daddr);
                }
#endif  /* TBRIDGE_DEBUG */

                return(TBRIDGE_IRB_RECEIVE);
            }
        }


#ifdef TBRIDGE_DEBUG    /* see the monument in tbridge_debug_flags.h !!! */
        if (tbridge_debug_dis_1) {
            buginf("\nTB1: discarded routed packet %s from %s to %e\n",
                   rxtype_string(pak->rxtype),
                   inputsw->namestring, addr_start->daddr);
        }
#endif  /* TBRIDGE_DEBUG */

    	/*
     	 *  If the major input I/F is not doing tbridging and we are 
     	 *  routing on this interface (as checked by the outer if). 
     	 *  Then we return TBRIDGE_CRB_RECEIVE for more process instead 
     	 *  of discarding this packet.
     	 *  The reason why routable packets coming from non-bridged main
     	 *  I/F can get to here when:
     	 *  -- IPSTAT_BRIDGING_MASK bit is turned on or tbridge_on_hwidb
     	 *     is set and,
     	 *  -- no SMF entries for b'cast and IP m'cast are created in 
     	 *     major I/F thus the smf_match_address_inline() fails.
     	 */
     	if (!inputsw->tbridge_on_swidb)
    		return(TBRIDGE_IRB_RECEIVE);

        return(TBRIDGE_IRB_DISCARD);
    }

    /*
     * The fate of most packets: bridgeable unicasts and multicasts:
     *
     * If the input interface is a Birdge-group Virtual Interface which
     * doesn't support bridging, the packet is routed to this bridge
     * group by routing code - relax this check.
     * Maybe we need to check it on outbound as well???
     */
    if (BRIDGING_RXTYPE(inputsw, pak->rxtype) || is_bvi(inputsw->hwptr)) {
	return(TBRIDGE_IRB_FORWARD);
    } else {

#ifdef TBRIDGE_DEBUG    /* see the monument in tbridge_debug_flags.h !!! */
        if (tbridge_debug_dis_1) {
            buginf("\nTB1: discarded bridged packet %s from %s to %e\n",
                   rxtype_string(pak->rxtype),
                   inputsw->namestring, addr_start->daddr);
        }
#endif  /* TBRIDGE_DEBUG */

    	/*
    	 * We reach here with the following T/F table value:
    	 * BRIDGEING_RXTYPE is FALSE AND is_bvi is FALSE AND
	 * (IRB is enabled OR ROUTING_RXTYPE is FALSE)
    	 * before we decide to discard this packet.
	 */
	if(ROUTING_RXTYPE(inputsw, pak->rxtype)) {
     	    if (!inputsw->tbridge_on_swidb)
    		return(TBRIDGE_IRB_RECEIVE);
	}

	return(TBRIDGE_IRB_DISCARD);
    }
}

#endif /* __TBRIDGE_IRB_INLINE_H__ */
