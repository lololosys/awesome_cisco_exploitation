/* $Id: ipfast_c7000.c,v 3.5.6.3 1996/06/18 01:48:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/ipfast_c7000.c,v $
 *------------------------------------------------------------------
 * ipfast_c7000.c -- IP fastswitching routines that are 7000 specific.
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_c7000.c,v $
 * Revision 3.5.6.3  1996/06/18  01:48:39  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/05/04  01:47:04  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.5.6.1  1996/03/18  22:00:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  07:34:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/13  02:00:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.20.1  1996/02/20  17:57:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/05  08:53:55  wilber
 * CSCdi45659:  Turbo flooding causes loops
 * Blocking port shouldn't forward UDP broadcast; Fix a leftover check
 * from the modification of CSCdi28311
 *
 * Revision 3.4  1995/12/01  09:07:10  tli
 * CSCdi44709:  GETBUF problems in SSE fragmentation
 * If microcode passes up a bogus packet, drop it as a runt.
 *
 * Revision 3.3  1995/11/22  17:49:26  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.2  1995/11/17  18:44:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/20  18:06:19  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.2  1995/08/23  05:31:15  gchristy
 * CSCdi28311:  UDP spanning-tree flooding with DLD creates duplicate
 * packets
 *  - Nuke bridge_flood_pak. Create and initialize udp_flood_pak during FS
 *    init.
 *
 *  - Instead of iterating over all interfaces, use the spanning tree
 *    flood table associated with the input interface to get the list of
 *    interfaces participating in the bridge-group.
 *
 *  - If a potential output interface is part of a circuit-group, use a
 *    hash of the source ip address and destination udp port number to
 *    choose one of the members of the group.
 *
 * Revision 2.1  1995/06/07  22:55:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "access.h"

#include "../ip/ip.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_bcopy_inline.h"
#include "../hes/if_hes_common.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../if/tring.h"
#include "../ip/ipfast.h"
#include "../ip/ipaccess.h"
#include "../ip/ipfast_hes.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_public.h"

#include "../hes/ctrlr_cmds.h"


/*
 * udp_fast_flood
 * This routine is the combined parts of ip_input(), udp_process(),
 * udp_forward() and udp_flood().
 * It is used to flood udp datagrams at the interrupt level.
 * Maybe we'll even get a speedup in the process.
 * We will bump up to process level if:
 *  - dont have the fast (but dangerous) udp flooding enabled
 *  - not using the spanning tree for udp flooding
 *  - not forwarding udp broadcasts at all
 *  - the packet isn't for us.
 *  - the input is not in forwarding state
 *  - input interface isn't an mci/cbus style interface
 *  - mci/cbus hasn't checked the packet.
 *  - it is a bootp packet.  They should be handled elsewhere.
 *
 * Note: one must be very careful with this routine.  The checks for processes
 * listening on sockets is not made here.  It is done for performance reasons.
 * If anything needs to be bumped up to process level for processing it must
 * be done explicitly here (as in bootp).  In addition, calling
 * find_udpsocket() could call the scheduler.  We don't need those headaches
 * at interrupt level.
 */

boolean udp_fast_flood (idbtype *swinput)
{
    mciregtype *regptr;
    iphdrtype *ip;
    hwidbtype *input;
    ushort length, broadaddr, ipsize, ipoffset, riflen;
    uint portnum;
    int itmp;
    ulong *lptr;

    if (!ip_udp_flood_it)
	return(FALSE);
    input = swinput->hwptr;
    if (!input->mci_checked)
	return(FALSE);

    regptr = input->mci_regptr;
    length = input->rxlength;

    /*
     * The code only checks the first short.  This is similar in
     * function to the check in bridge_broadcasts()
     */
    switch (input->fast_switch_type) {
      case FS_ETHER:
	/*
	 * Read the rest of the IP header.  It's aligned off by 1 word in
	 * the sniff buffer.  Also read the first long of the UDP header.
	 */
	ip = (iphdrtype *) &input->sniff[SNIFFBASE+0].d.sword[1];
	input->sniff[SNIFFBASE+3].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+4].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+6].d.sword[0] = regptr->readshort;

	regptr->rx_select = MCI_ETHER_OFFSET;
	ipoffset = MCI_OFFSETSIZE;
	broadaddr = 0xFFFF;
	ipsize = length - ETHER_ARPA_ENCAPBYTES;
	break;
    case FS_FDDI:
	/*
	 * Read the rest of the IP header.  It's aligned off by 5 shorts in
	 * the sniff buffer.  Also read the first long of the UDP header.
	 */
	ip = (iphdrtype *)&input->sniff[SNIFFBASE+2].d.sword[1];
	input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+8].d.sword[0] = regptr->readshort;

	regptr->rx_select = FDDI_RECEIVE_OFFSET + 1;
	broadaddr = 0xFFFF;
	ipsize = length - FDDI_SNAP_ENCAPBYTES;
	ipoffset = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;
	break;
    case FS_TOKEN:
	/*
	 * Read the rest of the IP header.  It's aligned off by 5 shorts in
	 * the sniff buffer.  Also read the first long of the UDP header.
	 */
	ip = (iphdrtype *)&input->sniff[SNIFFBASE+2].d.sword[1];
	input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+8].d.sword[0] = regptr->readshort;

	regptr->rx_select = CTR_RECEIVE_OFFSET + 1;
	riflen = input->rxrifwordlen;
	broadaddr = 0xFFFF;
	ipsize = length - TRING_SNAP_ENCAPBYTES - (riflen << 1);
	ipoffset = CTR_RECEIVE_OFFSET + TRING_SNAP_ENCAPWORDS +
	    riflen;
	break;
    case FS_HDLC:
	/*
	 * Sniff the IP header.
	 */
	regptr->rx_select = ipoffset = MCI_OFFSETSIZE;
	ip = (iphdrtype *)&input->sniff[SNIFFBASE+1].d.lword;
	input->sniff[SNIFFBASE+1].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+2].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+3].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+4].d.lword = regptr->readlong;
	input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
	
	/*
	 * Sniff the first word of the UDP header.
	 */
	input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
	regptr->rx_select = MCI_SERIAL_OFFSET;
	broadaddr = HDLC_BCAST;
	ipsize = length - input->encsize;
	break;
      default:
	return(FALSE);
    }
    
    {
	uint *ptr;
	
	/*
	 * If the mci passed the RX_CHECK then the header will always be
	 * MINHEADERBYTES long.  Check the UDP destination port number.
	 * There's a gross hack here to get the port number.
	 */
	portnum = ((udptype *)(ip + 1))->dport;			
	if ((portnum == BOOTPS_PORT) || (portnum == TFTP_PORT))
	    return(FALSE);
	
	/*
	 * Search the list of UDP ports which are currently being forwarded.
	 * This is coded strangely because the compiler mangles the
	 * straightforward implementation.
	 */
	itmp = udp_forwcnt;
	ptr = udp_forward_port;
	while (itmp) {
	    if (*ptr == portnum)
		break;
	    ptr++;
	    itmp--;
	}
	if (!itmp)
	    return(FALSE);
    }    

    /*
     * Check the first word of the destination MAC address to see that it's
     * a broadcast.
     */
    if (regptr->readshort != broadaddr)
	return(FALSE);
    

    /*
     * If we _would_ flood it, but this interface is blocked, then discard
     * it quickly.  This prevents the packet from floating up to process
     * level.  
     */
    if (!swinput->circ_canforward) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
    /*
     * If this is a subnet broadcast for the local subnet, then don't
     * flood it.  This causes the multiple spanning trees case to work
     * correctly for the brokerage firms.  Note that we are not
     * checking for secondary addresses -- this is a reasonable
     * configuration limitation.  However, for a small performance hit
     * the user can enable the acceptance of any broadcast.  This is
     * checked after the initial check described above.
     */
    if ((ip->dstadr & swinput->ip_nets_mask) ==
	(swinput->ip_address & swinput->ip_nets_mask)) {
	if (!ip_spanning_anybcast || !ip_broadcast(ip->dstadr, swinput))
	return(FALSE);
    }
    
    /*
     * The destination address must be a limited broadcast or an all
     * subnets broadcast.  The following code is correct, but is a
     * significant performance hit.
     */
#ifdef notdef
    if (ip->dstadr != IP_LIMITED_BROADCAST) {
	if (ip->dstadr != IP_BSD_LIMITED_BROADCAST) {
	    ipaddrtype majormask;

	    majormask = getmajormask(swinput->ip_address);
	    if ((ip->dstadr != (swinput->ip_address & majormask)) &&
		(ip->dstadr != (swinput->ip_address | ~majormask)))
		return(FALSE);
	}
    }
#endif

    /*
     * Do NOT recompute the UDP checksum.  Brokerage firms such as Solomon
     * Brothers use UDP forwarding very heavily and are concerned about
     * performance.  Recomputing the checksum is too expensive for this
     * application.  Step on the checksum in the original packet.
     */
    regptr->rxw_select = ipoffset + btow(MINIPHEADERBYTES) +
	btow(UDPHEADERBYTES) - 1;
    regptr->write1short = 0;

    ip_traffic.bcastin++;
    ip_traffic.inputs++;
    udp_traffic.inputs++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += length;
    input->counters.inputs_fast[ACCT_PROTO_IP]++;
    GET_TIMESTAMP(input->lastinput);
    input->counters.input_broad++;
    itmp = ip->dstadr;

#ifdef notdef
    if (udp_debug)
	buginf("\nUDP: rcvd src=%i(%d), dst=%i(%d), length=%d, turbo",
	       ip->srcadr,
	       ((udptype *)(ip + 1))->sport,
	       ip->dstadr,
	       ((udptype *)(ip + 1))->dport,
	       ipsize);
#endif

    {
	idbtype *swidb = NULL, *nextswidb = NULL;
	hwidbtype *idb = NULL, *nextidb = NULL;
	spantype *span = swinput->span_ptr;
	tbifd_type *oif, *next_oif;
	uint hash, counter;
	short group;

	if (!span)
	    return(FALSE);


	/*
	 * Iterate through flood table for the spanning tree
	 * associated with the input interface until we find an interface
	 * we can forward onto.
	 */
	for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {

	    swidb = oif->tbifd_swidb;
	    group = swidb->circ_group;

	    if (group) {
		/*
		 * This interface is part of a DLD circuit group.  Use
		 * the IP source address and destination UDP port 
		 * number of the input packet to select which member
		 * of a circuit group to forward to.
		 */
		counter = span->cct_groups[group].ccg_idb_count;

		if (!counter)
		    continue;
		
		hash = nethash(ip->srcadr) + portnum;
		swidb = span->cct_groups[group].idb_array[hash % counter];

		if (!swidb)
		    continue;
	    }

	    if (swidb == swinput)
		continue;

	    idb = swidb->hwptr;

	    if (swidb->circ_group &&
		(swinput->circ_group == swidb->circ_group))
		continue;

	    if (!swidb->circ_canforward || !swidb->ip_enabled ||
		(idb->state != IDBS_UP))
		continue;
	    
	    /*
	     * If not an ether, Fddi, or TR interface, joke 'em.  They
	     * shouldn't have turned on the feature to begin with.
	     * The packet just won't make it out this particular interface.
	     */
	    if (!(idb->status & (IDB_ETHER|IDB_FDDI|IDB_TR|IDB_SERIAL)))
		continue;
	    
	    if (ipsize > swidb->ip_mtu)
		continue;
	    
	    /*
	     * Check the output access list.
	     */
	    if (swidb->ip_output_accesslist) {
		switch (swidb->ip_output_accesslist->type) {

		case ACL_IP_SIMPLE:
		case ACL_IP_NAMED_SIMPLE:
		    if (!nfast_check(swidb->ip_output_accesslist,
				    ip->srcadr))
			continue;
		    break;

		case ACL_IP_EXTENDED:
		case ACL_IP_NAMED_EXTENDED:
		    if (!slow_check(swidb->ip_output_accesslist, TRUE,
				    swinput, ip->prot, ip->srcadr,
				    swidb->ip_broadcast,
				    ((udptype *)(ip + 1))->sport,
				    ((udptype *)(ip + 1))->dport,
				    FALSE, ip->tos))
			continue;
		    break;

		default:
		    break;
		}
	    }
	    break;
	}
	if (NULL == oif) {
	    /*
	     * Now get rid of the input packet.
	     */
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}

	/*
	 * Find the next idb to copy to, if any.  If there is one, then
	 * send make a copy for the _previous_ interface.  Confused yet?
	 */
	for (next_oif = oif->tbifd_next; next_oif;
	     next_oif = next_oif->tbifd_next) {

	    nextswidb = next_oif->tbifd_swidb;
	    group = nextswidb->circ_group;

	    if (group) {
		/*
		 * Use the IP source address and destination UDP port
		 * number of the input packet to select which member
		 * of a circuit group to forward to.
		 */
		counter = span->cct_groups[group].ccg_idb_count;

		if (!counter)
		    continue;
		
		hash = nethash(ip->srcadr) + portnum;
		nextswidb = span->cct_groups[group].idb_array[hash % counter];

		if (!nextswidb)
		    continue;
	    }

	    if (nextswidb == swinput)
		continue;

	    if (nextswidb->circ_group &&
		(swinput->circ_group == nextswidb->circ_group))
		continue;

	    nextidb = nextswidb->hwptr;

	    if (!nextswidb->circ_canforward || !nextswidb->ip_enabled ||
		(nextidb->state != IDBS_UP))
		continue;
	    
	    /*
	     * If not an ether, Fddi, or TR interface, joke 'em.  They
	     * shouldn't have turned on the feature to begin with.
	     * The packet just won't make it out this particular interface.
	     */
	    if (!(nextidb->status & (IDB_ETHER|IDB_FDDI|IDB_TR|IDB_SERIAL)))
		continue;
	    
	    if (ipsize > nextswidb->ip_mtu)
		continue;
	    
	    /*
	     * Check the output access list.
	     */
	    if (nextswidb->ip_output_accesslist) {

		switch (nextswidb->ip_output_accesslist->type) {
		case ACL_IP_SIMPLE:
		case ACL_IP_NAMED_SIMPLE:
		    if (!nfast_check(nextswidb->ip_output_accesslist,
				     ip->srcadr))
			continue;
		    break;

		case ACL_IP_EXTENDED:
		case ACL_IP_NAMED_EXTENDED:
		    if (!slow_check(nextswidb->ip_output_accesslist, TRUE,
				    swinput, ip->prot, ip->srcadr,
				    nextswidb->ip_broadcast,
				    ((udptype *)(ip + 1))->sport,
				    ((udptype *)(ip + 1))->dport,
				    FALSE, ip->tos))
			continue;
		    break;

		default:
		    break;
		}
	    }
	    
	    /*
	     * If we made it here, there are at least two interfaces we should
	     * burp onto.  Go ahead and make a copy for the first one (swidb,
	     * idb). 
	     */
	    regptr->argreg = idb->mci_index;
	    regptr->cmdreg = MCI_CMD_SELECT;
	    
	    regptr->argreg = idb->buffer_pool;
	    regptr->cmdreg = MCI_CMD_TX1_RESERVE;
	    
	    if (regptr->cmdreg != MCI_RSP_OKAY) {
		idb->counters.output_total_drops++;
		continue;
	    }
	    
	    switch (idb->fast_switch_type) {
	    case FS_ETHER:
		regptr->write1long = *((ulong *)&baddr[0]);
		regptr->write1short = GETSHORT(&baddr[4]);
		regptr->write1long = *((ulong *)&idb->hardware[0]);
		regptr->write1short = GETSHORT(&idb->hardware[4]);
		regptr->write1short = TYPE_IP10MB;
		length = ipsize + ETHER_ARPA_ENCAPBYTES;
		break;
	    case FS_FDDI:
		regptr->write1short = FDDI_LLC_FC_BYTE;
		regptr->write1long = *((ulong *)&baddr[0]);
		regptr->write1short = GETSHORT(&baddr[4]);
		regptr->write1long = *((ulong *)&idb->hardware[0]);
		regptr->write1short = GETSHORT(&idb->hardware[4]);
		regptr->write1long = SAP_CTL_HIZER;
		regptr->write1long = TYPE_IP10MB & 0xFFFF;
		length = (ipsize + FDDI_SNAP_ENCAPBYTES) | MCI_TX_ODDALIGN;
		break;
	    case FS_TOKEN:
		regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
		regptr->write1long = *((ulong *)&baddr[0]);
		regptr->write1short = GETSHORT(&baddr[4]);
		regptr->write1long = *((ulong *)&idb->hardware[0]);
		regptr->write1short = GETSHORT(&idb->hardware[4]);
		regptr->write1long = SAP_CTL_HIZER;
		regptr->write1long = TYPE_IP10MB & 0xFFFF;
		length = ipsize + TRING_SNAP_ENCAPBYTES;
		break;
	    default:
		/* case FS_HDLC: */
		if (idb->cache_board_encap){
		    ushort encapbytes;
		    (*idb->cache_board_encap)(idb,&encapbytes,sizeof(ushort));
		    regptr->write1short = encapbytes;
		}
		regptr->write1long = (HDLC_BCAST << 16) | TYPE_IP10MB;
		length = ipsize + idb->encsize;
		break;
	    }
	    
	    /*
	     * Datalink header is in place, copy the packet.
	     */
	    
	    if (itmp == swidb->ip_broadcast) {
		/*
		 * Copy the rest of the packet.
		 */
		regptr->rx_select = ipoffset;
		cbus_bcopy(regptr, ipsize);
	    } else {
		/*
		 * If we have to change the output IP address, then we
		 * recompute the checksum and lay the IP header down by hand.
		 * Note the gross hack here with the pointer.
		 */
		ip->dstadr = swidb->ip_broadcast;
		ip->checksum = 0;
		ip->checksum = 
		    ipcrc((ushort *)ip, MINIPHEADERBYTES);
		lptr = (ulong *)ip;
		regptr->write1long = *lptr++;
		regptr->write1long = *lptr++;
		regptr->write1long = *lptr++;
		regptr->write1long = *lptr++;
		regptr->write1long = *lptr;
		
		/*
		 * Copy the rest of the packet.
		 */
		regptr->rx_select = ipoffset + btow(MINIPHEADERBYTES);
		cbus_bcopy(regptr, ipsize - btow(MINIPHEADERBYTES));
	    }
	    
	    idb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] +=
		(length & ~MCI_TX_ODDALIGN);
	    idb->counters.outputs_fast[ACCT_PROTO_IP]++;
	    udp_traffic.bcastforward++;
	    
	    /*
	     * Ok, now go the next interface.
	     */
	    swidb = nextswidb;
	    idb = nextidb;

	    regptr->argreg = length;
	    regptr->cmdreg = MCI_CMD_TX1_START;
	}
	
	/*
	 * This is the last interface that needs attention.  Use an RX_MOVE
	 * instead of doing the copy.
	 */
	regptr->argreg = idb->mci_index;
	regptr->cmdreg = MCI_CMD_RX_MOVE;

	if (regptr->cmdreg != MCI_RSP_OKAY) {
	    /*
	     * Move failed, flush the input packet.
	     */
	    idb->counters.output_total_drops++;
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}
	
	switch (idb->fast_switch_type) {
	case FS_ETHER:
	    regptr->tx1_select = ipoffset - btow(ETHER_ARPA_ENCAPBYTES);
	    regptr->write1long = *((ulong *)&baddr[0]);
	    regptr->write1short = GETSHORT(&baddr[4]);
	    regptr->write1long = *((ulong *)&idb->hardware[0]);
	    regptr->write1short = GETSHORT(&idb->hardware[4]);
	    regptr->write1short = TYPE_IP10MB;
	    length = ipsize + ETHER_ARPA_ENCAPBYTES;
	    break;
	case FS_FDDI:
	    regptr->tx1_select = ipoffset - FDDI_SNAP_ENCAPWORDS;
	    regptr->write1short = FDDI_LLC_FC_BYTE;
	    regptr->write1long = *((ulong *)&baddr[0]);
	    regptr->write1short = GETSHORT(&baddr[4]);
	    regptr->write1long = *((ulong *)&idb->hardware[0]);
	    regptr->write1short = GETSHORT(&idb->hardware[4]);
	    regptr->write1long = SAP_CTL_HIZER;
	    regptr->write1long = TYPE_IP10MB & 0xFFFF;
	    length = (ipsize + FDDI_SNAP_ENCAPBYTES) | MCI_TX_ODDALIGN;
	    break;
	case FS_TOKEN:
	    regptr->tx1_select = ipoffset - TRING_SNAP_ENCAPWORDS;
	    regptr->write1short =  (AC_PRI0 << 8) | FC_TYPE_LLC;
	    regptr->write1long = *((ulong *)&baddr[0]);
	    regptr->write1short = GETSHORT(&baddr[4]);
	    regptr->write1long = *((ulong *)&idb->hardware[0]);
	    regptr->write1short = GETSHORT(&idb->hardware[4]);
	    regptr->write1long = SAP_CTL_HIZER;
	    regptr->write1long = TYPE_IP10MB & 0xFFFF;
	    length = ipsize + TRING_SNAP_ENCAPBYTES;
	    break;
	default:
	    /* case FS_HDLC: */
	    regptr->tx1_select = ipoffset - btow(idb->encsize);
	    if (idb->cache_board_encap){
		ushort encapbytes;
		
		(*idb->cache_board_encap)(idb,&encapbytes,sizeof(ushort));
		regptr->write1short = encapbytes;
	    } 
	    regptr->write1long = (HDLC_BCAST << 16) | TYPE_IP10MB;
	    length = ipsize + idb->encsize;
	    break;
	}
	
	if (itmp != swidb->ip_broadcast) {
	    /*
	     * If we have to change the output IP address, then we
	     * recompute the checksum and lay the IP header down by hand.
	     * Note the gross hack here with the pointer.
	     */
	    ip->dstadr = swidb->ip_broadcast;
	    ip->checksum = 0;
	    ip->checksum = 
		ipcrc((ushort *)ip, MINIPHEADERBYTES);
	    lptr = (ulong *)ip;
	    regptr->write1long = *lptr++;
	    regptr->write1long = *lptr++;
	    regptr->write1long = *lptr++;
	    regptr->write1long = *lptr++;
	    regptr->write1long = *lptr;
	}
	
	regptr->argreg = length;
	regptr->cmdreg = MCI_CMD_TX1_START;
	idb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] +=
	    (length & ~MCI_TX_ODDALIGN);
	idb->counters.outputs_fast[ACCT_PROTO_IP]++;
	udp_traffic.bcastforward++;
	return(TRUE);
    }
}


/*
 * ip_fast_fragmentation
 * Perform fast fragmentation by fragmenting from one interface directly to
 * another.  Assumes the packet has already been encapsulated correctly
 * within the input buffer.
 */

boolean ip_fast_fragmentation (hwidbtype *inidb, hwidbtype *outidb)
{
    mciregtype *regptr;
    idbtype *idb;
    uchar outmac, outmacwds;
    ushort macoffset;
    ushort utmp1, utmp2, rxoffset;
    ushort frag_offset, outbytes, len, origfo;
    int i;
    paktype *pak;
    volatile ulong *writeptr;

    regptr = inidb->mci_regptr;

    /*
     * Get the input and output mac lengths.
     */
    utmp1 = regptr->rxsrbinfo;
    outmac = utmp1 & 0xff;
    utmp1 >>= 8;

    /*
     * Select to start of output mac.  Read the outmac into our local
     * buffer.  
     * NB: Kludge here to get to the correct start of the output mac
     * header. 
     */
    outmacwds = btow(outmac);
    rxoffset = inidb->rxoffset + btow(utmp1);
    macoffset = rxoffset - outmacwds;
    regptr->rx_select = rxoffset;

    /*
     * Now at start of IP header.  Read into sniff buffer.  We know it
     * fits because an IP header that would have been switched could only
     * be 5 longwords.  Check the DF bit.  Leave the pointer at the start
     * of user data.
     */
    inidb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;
    inidb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;
    if (inidb->sniff[SNIFFBASE+1].d.byte[2] & IP_DF_BIT_MASK)
	goto receive;
    inidb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;
    inidb->sniff[SNIFFBASE+3].d.lword = regptr->readlong;
    inidb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;

    /*
     * Stop pointing at the total length, with writing enabled.  Later,
     * we'll want to point to the end of the IP header.
     */
    regptr->rxw_select = rxoffset + MCI_IP_LENGTH_OFFSET;

    rxoffset += btow(MINIPHEADERBYTES);

    /*
     * Get the output MTU.  If someone is strange enough to use an odd MTU,
     * just pretend it's one byte shorter.
     * Calculate the length to tell the output MCI.  It's really too bad
     * that we have to caluculate this at runtime.  It would be much nicer
     * to do so at config time.
     */
    utmp2 = outidb->firstsw->ip_mtu;
    FOR_ALL_SWIDBS_ON_HW(outidb, idb) 
	utmp2 = min(utmp2, idb->ip_mtu);
    if (utmp2 & 1)
	utmp2--;

    /*
     * IP fragments must be a multiple of 8 bytes long.
     * Len is the number of data bytes that we have to
     * fragment, utmp1 is the size of the full fragments that we'll be
     * sending. 
     */
    outbytes = utmp2 - MINIPHEADERBYTES;
    outbytes &= ~7;
    len = inidb->rxlength - utmp1 - MINIPHEADERBYTES;
    outmac += MINIPHEADERBYTES;
    utmp1 = outbytes + outmac;
    if (outmac & 1)
	utmp1 |= MCI_TX_ODDALIGN;

    /*
     * Mung the header to say that it has more fragments, and that the
     * packet will fill the mtu.  Save the current fragment offset in
     * origfo.  Decrement TTL.  Write the length field in the real header.
     */
    utmp2 = outbytes + MINIPHEADERBYTES;
    inidb->sniff[SNIFFBASE+0].d.sword[1] = utmp2;
    regptr->write1short = utmp2;
    inidb->sniff[SNIFFBASE+2].d.byte[0]--;
    origfo = inidb->sniff[SNIFFBASE+1].d.sword[1];
    frag_offset = origfo | (IP_MF_BIT_MASK << 8);
    regptr->rx_select = rxoffset;
    fci_select_if(outidb, regptr);
    
    while (len > outbytes) {
	/*
	 * Grab an output buffer.
	 */
	regptr->argreg = outidb->buffer_pool;
	regptr->cmdreg = MCI_CMD_TX1_RESERVE;
	if (regptr->cmdreg != MCI_RSP_OKAY) 
	    goto txdrop;

	/*
	 * Lay down the mac header and IP header.
	 */
	regptr->rx_select = macoffset;
	cbus_bcopy(regptr, wtob(outmacwds) + MINIPHEADERBYTES);

	/*
	 * Set the fragment offset field and zero the checksum field.
	 * Checksum the IP header in the sniff buffer and then lay that
	 * down.  
	 */
	inidb->sniff[SNIFFBASE+1].d.sword[1] = frag_offset;
	regptr->rx_select = rxoffset;
	inidb->sniff[SNIFFBASE+2].d.sword[1] = 0;
	regptr->tx1_select = outmacwds + MCI_IP_FRAG_OFFSET;
	inidb->sniff[SNIFFBASE+2].d.sword[1] =
	    ipcrc((ushort *)&inidb->sniff[SNIFFBASE+0], MINIPHEADERBYTES);
	regptr->write1short = frag_offset;
	regptr->write1long = inidb->sniff[SNIFFBASE+2].d.lword;
	regptr->tx1_select = outmacwds + btow(MINIPHEADERBYTES);

	/*
	 * Move the data.
	 */
	cbus_bcopy(regptr, outbytes);

	/*
	 * Update where we are in the world.  Fragment offsets are in
	 * multiples of 8 bytes.  We do this now because the bcopy can run
	 * in parallel.
	 */
	rxoffset += (outbytes >> 1);
	frag_offset += (outbytes >> 3);
	len -= outbytes;
	outidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP]
	    += (utmp1 & ~MCI_TX_ODDALIGN);
	outidb->counters.outputs_fast[ACCT_PROTO_IP]++;
	ip_traffic.fragcount++;

	/*
	 * Send it.  Be sure to select back to 0 before
	 * sending as that sets the transmission offset.
	 */
	regptr->tx1_select = 0;
	regptr->argreg = utmp1;
	regptr->cmdreg = MCI_CMD_TX1_START;
    }

    /*
     * Now, do the trailing portion.
     * Grab an output buffer.
     */
    regptr->argreg = outidb->buffer_pool;
    regptr->cmdreg = MCI_CMD_TX1_RESERVE;
    if (regptr->cmdreg != MCI_RSP_OKAY) 
	goto txdrop;
    
    /*
     * Lay down the mac header.
     */
    regptr->rx_select = macoffset;
    cbus_bcopy(regptr, wtob(outmacwds) + MINIPHEADERBYTES);
    
    /*
     * Set the fragment offset field.  Checksum the IP header in the
     * sniff buffer and then lay that down. 
     */
    inidb->sniff[SNIFFBASE+0].d.sword[1] = len + MINIPHEADERBYTES;
    frag_offset &= ~(IP_MF_BIT_MASK << 8);
    frag_offset |= (origfo & (IP_MF_BIT_MASK << 8));
    regptr->rx_select = rxoffset;
    inidb->sniff[SNIFFBASE+1].d.sword[1] = frag_offset;
    inidb->sniff[SNIFFBASE+2].d.sword[1] = 0;
    regptr->tx1_select = outmacwds;
    inidb->sniff[SNIFFBASE+2].d.sword[1] =
	ipcrc((ushort *)&inidb->sniff[SNIFFBASE+0], MINIPHEADERBYTES);
    writeptr = &regptr->write1long;
    *writeptr = inidb->sniff[SNIFFBASE+0].d.lword;
    *writeptr = inidb->sniff[SNIFFBASE+1].d.lword;
    *writeptr = inidb->sniff[SNIFFBASE+2].d.lword;
    regptr->tx1_select = outmacwds + btow(MINIPHEADERBYTES);
    
    /*
     * Move the data and do the accounting.
     */
    cbus_bcopy(regptr, len);
    
    utmp2 = len + outmac;
    if (outmac & 1)
	utmp2 |= MCI_TX_ODDALIGN;
    outidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP]
	+= (utmp2 & ~MCI_TX_ODDALIGN);
    outidb->counters.outputs_fast[ACCT_PROTO_IP]++;
    ip_traffic.fragcount++;
    ip_traffic.fragged++;

    /*
     * Send it.
     */
    regptr->tx1_select = 0;
    regptr->argreg = utmp2;
    regptr->cmdreg = MCI_CMD_TX1_START;

    return(FALSE);

 txdrop:
    outidb->counters.output_total_drops++;
    return(FALSE);

 receive:
    /*
     * Move to the start of the IP header.
     * utmp1 = input mac length
     */
    regptr->rx_select = inidb->rxoffset + btow(utmp1);
    i = inidb->rxlength - utmp1;
    if (-1 == i) {

	/*
	 * This is a cover-up for some obscure problem, possibly with
	 * ucode, that passes up some kind of runt.
	 */
	inidb->counters.runts++;
	return(FALSE);
    }
    pak = input_getbuffer(i, inidb);
    if (!pak) {
	cbus_mci_idb_throttle(inidb);
	return(FALSE);
    }
    pak->datagramsize = i;
    pak->datagramstart = pak->data_area + ENCAPBYTES;
    pak->mac_start = pak->datagramstart;
    pak->addr_start = pak->mac_start;
    pak->rif_start = NULL;
    pak->riflen = 0;
    pak->info_start = pak->mac_start;
    pak->network_start = pak->mac_start;
    mci2buffer(&regptr->readlong, pak->datagramstart, i);		
    inidb->counters.inputs++;
    internet_enqueue(pak);
    return(FALSE);
}

/*
 * udp_fast_flood_init
 * Initialize udp fast flooding.
 *
 * This is a noop for the 7000.
 */

void udp_fast_flood_init ()
{
}
