/* $Id: udp_flood_fs.c,v 3.1.8.2 1996/07/08 22:34:28 mwu Exp $
 * $Source: /release/112/cvs/Xsys/ip/udp_flood_fs.c,v $
 *------------------------------------------------------------------
 * UDP fastswitch flooding - platform independent version
 *
 * December 1995, Bob O'Grady
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: udp_flood_fs.c,v $
 * Revision 3.1.8.2  1996/07/08  22:34:28  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1.8.1  1996/06/18  01:46:24  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/12  11:10:01  ogrady
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * These routines are used to flood udp broadcast datagrams at the
 * interrupt level.
 * This is a port of the c7000 version udp_fast_flood().
 *
 * On entry:
 *    pak->network_start should == start of ip header
 *    pak->addr_start should == start of dest mac addr
 *
 * returns TRUE if pak was sent or dropped, else FALSE
 *
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

#include "master.h"
#include "logger.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "access.h"

#include "../ip/ip.h"
#include "../ip/udp_flood_fs.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../if/tring.h"
#include "../ip/ipfast.h"
#include "../ip/ipaccess.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_public.h"
#include "../h/fastswitch.h"


extern ushort fastiphdrchk(ushort *, ulong);


/*
 * return TRUE if ok to flood udp packet out given interface
 */
static inline boolean
udp_flood_output_ok (idbtype *swinput,
		     idbtype *swoutput,
		     iphdrtype *ip,
		     udptype *udp,
		     int ipsize)
{
    hwidbtype *hwoutput;

    if (swoutput == swinput)
	return(FALSE);

    if (swoutput->circ_group && (swinput->circ_group == swoutput->circ_group))
	return(FALSE);

    hwoutput = swoutput->hwptr;

    if (!swoutput->circ_canforward || !swoutput->ip_enabled ||
	(hwoutput->state != IDBS_UP))
	return(FALSE);

    /*
     * Check for correct output interface type 
     */
    if (!(hwoutput->status & (IDB_ETHER | IDB_FDDI | IDB_TR | IDB_SERIAL)))
	return(FALSE);

    if (ipsize > swoutput->ip_mtu)
	return(FALSE);

    /*
     * Check the output access list.
     */
    if (swoutput->ip_output_accesslist) {
        switch (swoutput->ip_output_accesslist->type) {
 
        case ACL_IP_SIMPLE:
        case ACL_IP_NAMED_SIMPLE:
            if (!nfast_check(swoutput->ip_output_accesslist, ip->srcadr)) {
                return(FALSE);
            }
            break;
 
        case ACL_IP_EXTENDED:
        case ACL_IP_NAMED_EXTENDED:
            if (!slow_check(swoutput->ip_output_accesslist, TRUE, swinput,
                                ip->prot, ip->srcadr, swoutput->ip_broadcast,
                                udp->sport, udp->dport, FALSE, ip->tos)) {
                return(FALSE);
            }
            break;

        default:
                return(FALSE);
        }
    }
    return(TRUE);
}


/*
 * prepend mac hdr for udp broadcast output to pak->network_start
 * adjusts datagramstart
 * returns encaps size in pak->encsize;
 */
static inline void
udp_flood_mac_rewrite (hwidbtype *hwidb, paktype *pak)
{
    switch (hwidb->fast_switch_type) {

	case FS_ETHER:
	{
	    ether_hdr *ether;

	    pak->encsize = ETHER_ARPA_ENCAPBYTES;
	    pak->datagramstart = pak->network_start - ETHER_ARPA_ENCAPBYTES;
	    ether = (ether_hdr *) pak->datagramstart;

	    ieee_copy(baddr, ether->daddr);
	    ieee_copy(hwidb->hardware, ether->saddr);
	    PUTSHORT(&ether->type_or_len, TYPE_IP10MB);
	    break;
	}
	case FS_FDDI:
	{
	    fddi_hdr *fddi;

	    pak->encsize = FDDI_SNAP_ENCAPBYTES;
	    pak->datagramstart = pak->network_start - FDDI_SNAP_ENCAPBYTES;
	    fddi = (fddi_hdr *) pak->datagramstart;

	    fddi->fc = FDDI_LLC_FC_BYTE;
	    ieee_copy(baddr, fddi->daddr);
	    ieee_copy(hwidb->hardware, fddi->saddr);
	    /*
	     * write snap hdr - quicker done in lwords
	     */
	    PUTLONG(fddi->data, SAP_CTL_HIZER);
	    PUTLONG(&fddi->data[4], TYPE_IP10MB);
	    break;
	}
	case FS_TOKEN:
	{
	    tring_hdr *trh;

	    pak->encsize = TRING_SNAP_ENCAPBYTES;
	    pak->datagramstart = pak->network_start - TRING_SNAP_ENCAPBYTES;
	    trh = (tring_hdr *) pak->datagramstart;

	    PUTSHORT(&trh->ac, ((AC_PRI0 << 8) | FC_TYPE_LLC));
	    ieee_copy(baddr, trh->daddr);
	    ieee_copy(hwidb->hardware, trh->saddr);
	    /*
	     * write snap hdr - quicker done in lwords
	     */
	    PUTLONG(trh->data, SAP_CTL_HIZER);
	    PUTLONG(&trh->data[4], TYPE_IP10MB);
	    break;
	}
	default:
	{
	    /*
	     *  case FS_HDLC: 
	     */

	    pak->encsize = HDLC_ENCAPBYTES;
	    pak->datagramstart = pak->network_start - HDLC_ENCAPBYTES;

	    PUTLONG(pak->datagramstart, ((HDLC_BCAST << 16) | TYPE_IP10MB));

	    if (hwidb->cache_board_encap) {

	        uchar board_encaps[8];
	        int board_encsize;

	        board_encsize =
		    (*hwidb->cache_board_encap)(hwidb, board_encaps, 8);

	        if (board_encsize > 0) {
	    	    pak->encsize += board_encsize;
		    pak->datagramstart -= board_encsize;
		    bcopy(board_encaps, pak->datagramstart, board_encsize);
		}
	        /*
	         * board_encsize < 0 a bug or a legit error? 
	         */ 
	    }
	}
    }
}


/*
 * prepend mac header, output udp broadcast packet, do accounting
 */
static void
udp_flood_output (idbtype *swinput,
		  idbtype *swoutput,
		  paktype *pak,
		  int ipsize,
		  ipaddrtype ip_dstadr)
{
    hwidbtype *hwoutput;

    hwoutput = swoutput->hwptr;
    udp_flood_mac_rewrite(hwoutput, pak);

    /*
     * compare here using the input ip dst adr
     *  avoid reading the output ip header if possible - its a cache miss
     */
    if (ip_dstadr != swoutput->ip_broadcast) {

	ushort checksum;
        iphdrtype *ip;

        ip = (iphdrtype *)pak->network_start;

	/*
	 * we have to change the output IP address,
	 * recompute the checksum.
	 */
	PUTLONG(&ip->dstadr, swoutput->ip_broadcast);
	ip->checksum = 0;
	checksum = ipcrc((ushort *) ip, MINIPHEADERBYTES);
	PUTSHORT(&ip->checksum, checksum);

    }

    /*
     * send the packet
     */
    setup_fastsend_pak(pak, swinput, swoutput, pak->datagramstart,
		       ipsize+pak->encsize, LINK_IP, ACCT_PROTO_IP);

    hwoutput->fastsend(hwoutput, pak);

    udp_traffic.bcastforward++;
}


/*
 * fast flood udp packet
 * entry point from rx interrupt handlers
 */
boolean
udp_flood_fs (idbtype *swinput, paktype *pak)
{
    paktype *newpak;
    idbtype *swoutput, *first_swoutput;
    hwidbtype *input;
    iphdrtype *ip;
    udptype *udp;
    spantype *span;
    tbifd_type *oif;
    uint *forward_port;
    uint portnum, hash, counter;
    int i;
    ipaddrtype ip_dstadr;
    ushort ipsize;
    short group;


    if (!ip_udp_flood_it)
	return (FALSE);

    input = swinput->hwptr;

    ip = (iphdrtype *) pak->network_start;

    ipsize = pak->datagramsize - (pak->network_start - pak->datagramstart);


    /*
     * Check the first word of the destination MAC address to see that it's
     * a broadcast. The code only checks the first short.  This is similar in
     * function to the check in bridge_broadcasts()
     *
     * (xxx should put bcast addr into idb)
     */
    if (input->fast_switch_type != FS_HDLC) {
	/*
	 * ether, fddi, tr
	 */
        if (GETSHORT(pak->addr_start) != 0xFFFF)
	    return (FALSE);
    } else {
        if (GETSHORT(pak->addr_start) != HDLC_BCAST)
	    return (FALSE);
    }

    /*
     * check IP hdr is valid and MINHEADERBYTES long, decr ttl, etc
     */
    ip_dstadr = GETLONG(&ip->dstadr);
    if (!(pak->flags & PAK_MCICHECKED)) {
        if (fastiphdrchk((ushort *)ip, ip_dstadr))
	    return(FALSE);
        pak->flags |= PAK_MCICHECKED;
    }

    /*
     * Check the UDP destination port number.
     */
    udp = (udptype *)(ip + 1);
    portnum = udp->dport;
    if ((portnum == BOOTPS_PORT) || (portnum == TFTP_PORT))
	return (FALSE);

    /*
     * Search the list of UDP ports which are currently being forwarded.
     * This is coded strangely because the compiler mangles the
     * straightforward implementation.
     */
    i = udp_forwcnt;
    forward_port = udp_forward_port;
    while (i) {
	if (portnum == *forward_port)
	    break;
	forward_port++;
	i--;
    }

    if (!i)
	return (FALSE);

    /*
     * If we _would_ flood it, but this interface is blocked, then discard
     * it quickly.  This prevents the packet from floating up to process
     * level.  
     */
    if (!swinput->circ_canforward) {
	datagram_done(pak);
	return (TRUE);
    }

    /*
     * If this is a subnet broadcast for the local subnet, then don't
     * flood it.  This causes the multiple spanning trees case to work
     * correctly for the brokerage firms.  Note that we are not
     * checking for secondary addresses -- this is a reasonable
     * configuration limitation.
     */
    if ((ip_dstadr & swinput->ip_nets_mask) ==
	    (swinput->ip_address & swinput->ip_nets_mask)) {
	if (!ip_spanning_anybcast || !ip_broadcast(ip_dstadr, swinput))
	    return (FALSE);
    }

    /*
     * Do NOT recompute the UDP checksum.  Brokerage firms such as Solomon
     * Brothers use UDP forwarding very heavily and are concerned about
     * performance.  Recomputing the checksum is too expensive for this
     * application.  Step on the checksum in the original packet.
     */
    udp->checksum = 0;

    ip_traffic.bcastin++;
    ip_traffic.inputs++;
    udp_traffic.inputs++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += pak->datagramsize;
    input->counters.inputs_fast[ACCT_PROTO_IP]++;
    GET_TIMESTAMP(input->lastinput);
    input->counters.input_broad++;

#ifdef notdef
    if (udp_debug)
	buginf("\nUDP: rcvd src=%i(%d), dst=%i(%d), length=%d, turbo",
	       ip->srcadr,
	       ((udptype *) (ip + 1))->sport,
	       ip_dstadr,
	       ((udptype *) (ip + 1))->dport,
	       ipsize);
#endif

    swoutput = NULL;
    first_swoutput = NULL;

    span = swinput->span_ptr;
    if (!span)
	return (FALSE);

    /*
     * Iterate through flood table for the spanning tree
     * associated with the input interface until we find an interface
     * we can forward onto.
     */
    for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {
 
        swoutput = oif->tbifd_swidb;
	group = swoutput->circ_group;

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
	    swoutput = span->cct_groups[group].idb_array[hash % counter];

	    if (!swoutput)
		continue;
	}

	if (!udp_flood_output_ok(swinput, swoutput, ip, udp, ipsize))
		continue;


	/*
	 * we have an interface to output on
	 * save first one for outputting the input pak later.
	 * (this test inside the loop is probably no worse than pulling
	 *  the code into the I cache twice).
	 */
	if (!first_swoutput) {
	    first_swoutput = swoutput;
	} else {

	    /*
	     * for the rest: get pak for fast output
	     * copy ip hdr and data to new pak and output
	     */
	    if (!(newpak = platform_fastsend_getbuffer(ipsize))) {
		break;	/* can't do more copies - send out orig */
	    }
	    platform_fs_pak_data_copy(pak, newpak, (uchar *)ip,
				      newpak->network_start, ipsize);

	    udp_flood_output(swinput, swoutput, newpak, ipsize, ip_dstadr);
	}
    }

    if (first_swoutput) {
	/*
	 * use input packet for final output to save making a copy
	 */
	udp_flood_output(swinput, first_swoutput, pak, ipsize, ip_dstadr);
    } else {

	/*
	 * no interfaces to forward packet to?
	 */
	datagram_done(pak);
    }
    return(TRUE);
}

