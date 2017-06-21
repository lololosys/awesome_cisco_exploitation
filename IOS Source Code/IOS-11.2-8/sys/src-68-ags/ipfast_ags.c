/* $Id: ipfast_ags.c,v 3.4.50.5 1996/08/28 13:11:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-ags/ipfast_ags.c,v $
 *------------------------------------------------------------------
 * ipfast_ags.c -- Fastswitching routines specific to the AGS+.
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_ags.c,v $
 * Revision 3.4.50.5  1996/08/28  13:11:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.50.4  1996/07/23  13:28:16  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4.50.3  1996/06/18  01:48:33  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.50.2  1996/05/04  01:46:38  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Revision 3.4.50.1  1996/03/18  21:58:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/13  02:00:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.22.1  1996/02/20  17:54:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/22  17:49:20  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/17  18:42:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:39  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:24:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/20  18:06:17  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.2  1995/08/23  05:31:13  gchristy
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
 * Revision 2.1  1995/06/07  22:54:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "access.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "../ip/ip.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../if/tring.h"
#include "../ip/ipfast.h"
#include "../ip/ipaccess.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_public.h"

static paktype *udp_flood_pak;

extern int nfddinets, nTRs, nhssinets;


/*
 * udp_fast_flood_int
 * Workhorse routine for udp_fast_flood().  This is a seperate routine
 * because we were running out of registers in udp_fast_flood().
 */
void udp_fast_flood_int (
     idbtype *input,
     ulong ipsize)
{
    idbtype *swidb;
    hwidbtype *idb;
    ulong output_interface;
    mciregtype *dstreg;
    ulong length;
    iphdrtype *ip;
    udptype *udp;
    spantype *span = input->span_ptr;
    tbifd_type *oif;
    short group;
    
    if (!span)
	return;
    

    ip = (iphdrtype *)ipheadstart(udp_flood_pak);
    udp = (udptype *)ipdatastart(udp_flood_pak);

    /*
     * Do NOT recompute UDP checksum.  Brokerage firms such as Solomon
     * Brothers use UDP forwarding very heavily and are concerned about
     * performance.  Recomputing the checksum is too expensive for this
     * application.
     */
    udp->checksum = 0; 

    /*
     * Iterate through flood table for the spanning tree
     * associated with the input interface until we find an interface
     * we can forward onto.
     */
    for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {

	swidb = oif->tbifd_swidb;
	group = swidb->circ_group;

	if (group) {
	    uint hash, counter;
	
	    /*
	     * This interface is part of a DLD circuit group.  Use
	     * the IP source address and destination UDP port 
	     * number of the input packet to select which member
	     * of a circuit group to forward to.
	     */
	    counter = span->cct_groups[group].ccg_idb_count;

	    if (!counter)
		continue;
		
	    hash = nethash(ip->srcadr) + udp->dport;
	    swidb = span->cct_groups[group].idb_array[hash % counter];

	    if (!swidb)
		continue;
	}

	idb = swidb->hwptr;

	if (swidb == input)
	    continue;

	if (swidb->circ_group &&
	    (input->circ_group == swidb->circ_group))
	    continue;

	if (!swidb->circ_canforward || !swidb->ip_enabled ||
	    (idb->state != IDBS_UP))
	    continue;

	output_interface = idb->fast_switch_type;

	/*
	 * If not an ether or Fddi interface, joke 'em.  They shouldn't
	 * have turned on the feature to begin with.  The packet just
	 * wont make it out this particular interface.
	 */
	if (idb->status & (IDB_ETHER|IDB_FDDI|IDB_TR|IDB_SERIAL)) {

	    if (ipsize > swidb->ip_mtu)
		continue;

	    ip->dstadr = swidb->ip_broadcast;

	    ip->checksum = 0;
	    ip->checksum = ipcrc((ushort *)ip, MINIPHEADERBYTES);

	    /*
	     * ip_output_accesslist is already checked in ip_accesscheck(),
	     * but maybe this will save us a subroutine call.
	     */
	    if ((swidb->ip_output_accesslist > 0) &&
		!ip_accesscheck(udp_flood_pak,
				swidb->ip_output_accesslist))
		continue;

	    dstreg = idb->mci_regptr;
	    dstreg->argreg = idb->mci_index;
	    dstreg->cmdreg = MCI_CMD_SELECT;

	    dstreg->argreg = idb->buffer_pool;
	    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;

	    if (dstreg->cmdreg != MCI_RSP_OKAY) {
		idb->counters.output_total_drops++;
		continue;
	    }
	    
	    dstreg->argreg = 0;
	    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

	    switch (output_interface) {
	      case FS_ETHER:
		dstreg->write1long = *((ulong *)&baddr[0]);
		dstreg->write1short = GETSHORT(&baddr[4]);
		dstreg->write1long = *((ulong *)&idb->hardware[0]);
		dstreg->write1short = GETSHORT(&idb->hardware[4]);
		dstreg->write1short = TYPE_IP10MB;
		length = ipsize + ETHER_ARPA_ENCAPBYTES;
		if (length < MINETHERSIZE)
		    length = MINETHERSIZE;
		break;
	      case FS_FDDI:
		dstreg->write1short = FDDI_LLC_FC_BYTE;
		dstreg->write1long = *((ulong *)&baddr[0]);
		dstreg->write1short = GETSHORT(&baddr[4]);
		if (idb->type == IDBTYPE_FDDIT) {
		    dstreg->write1long = *((ulong *)&idb->hardware[0]);
		    dstreg->write1short = GETSHORT(&idb->hardware[4]);
		} else {
		    dstreg->write1long = *((ulong *)&idb->bit_swapped_hardware[0]);
		    dstreg->write1short = GETSHORT(&idb->bit_swapped_hardware[4]);
		}
		dstreg->write1long = SAP_CTL_HIZER;
		dstreg->write1long = TYPE_IP10MB & 0xFFFF;
		length = (ipsize + FDDI_SNAP_ENCAPBYTES) | MCI_TX_ODDALIGN;
		break;
	      case FS_TOKEN:
		dstreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
		dstreg->write1long = *((ulong *)&baddr[0]);
		dstreg->write1short = GETSHORT(&baddr[4]);
		dstreg->write1long = *((ulong *)&idb->hardware[0]);
		dstreg->write1short = GETSHORT(&idb->hardware[4]);
		dstreg->write1long = SAP_CTL_HIZER;
		dstreg->write1long = TYPE_IP10MB & 0xFFFF;
		length = ipsize + TRING_SNAP_ENCAPBYTES;
		break;
	      default : /* case FS_HDLC: */
		if (idb->cache_board_encap){
		    ushort encapbytes;
		    (*idb->cache_board_encap)(idb,&encapbytes,sizeof(ushort));
		    dstreg->write1short = encapbytes;
		}
		dstreg->write1long = (HDLC_BCAST << 16) | TYPE_IP10MB;
		length = ipsize + idb->encsize;
		break;
	    }

	    buffer2mci(ip, &dstreg->write1long, ipsize);

	    dstreg->argreg = length;
	    dstreg->cmdreg = MCI_CMD_TX1_START;
	    idb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] +=
		(length & ~MCI_TX_ODDALIGN);
	    idb->counters.outputs_fast[ACCT_PROTO_IP]++;
	    udp_traffic.bcastforward++;
	}
    }
}

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

boolean udp_fast_flood (
     idbtype *swinput)
{
    ulong length;
    ulong ipsize;
    mciregtype *srcreg;
    int i;
    boolean good;
    ulong input_interface;
    ushort broadaddr, riflen;
    hwidbtype *input = swinput->hwptr;

    if (!ip_udp_flood_it)
	return(FALSE);
    if (!input->mci_checked)
	return(FALSE);

    input_interface = input->fast_switch_type;
    srcreg = input->mci_regptr;

    /*
     * Short read is ip checksum.  Long reads are SA, DA, and first
     * word of udp header (which contains sport and dport).  We only
     * need the udp dport and the ip DA.  Reading all this extra baggage
     * is probably faster than selecting past it.
     * Note: this code relies on the rx pointer to be sitting at the IP
     * checksum.
     */
    input->sniff[SNIFFBASE+3].d.sword[1] = srcreg->readshort;
    input->sniff[SNIFFBASE+4].d.lword = srcreg->readlong;
    input->sniff[SNIFFBASE+5].d.lword = srcreg->readlong;
    input->sniff[SNIFFBASE+6].d.lword = srcreg->readlong;

    /*
     * If the mci passed the RX_CHECK then the header will always be
     * MINHEADERBYTES long.
     */
    if ((input->sniff[SNIFFBASE+6].d.sword[1] == BOOTPS_PORT) ||
	(input->sniff[SNIFFBASE+6].d.sword[1] == TFTP_PORT))
	return(FALSE);

    length = input->rxlength;

    /*
     * The code only checks the first short.  This is similar in
     * function to the check in bridge_broadcasts()
     */
    switch (input_interface) {
      case FS_ETHER:
	srcreg->argreg = MCI_ETHER_OFFSET;
	udp_flood_pak->datagramstart = udp_flood_pak->network_start -
					  ETHER_ARPA_ENCAPBYTES;
	broadaddr = 0xFFFF;
	ipsize = length - ETHER_ARPA_ENCAPBYTES;
	break;
      case FS_FDDI:
	srcreg->argreg = FDDI_RECEIVE_OFFSET + 1;
	udp_flood_pak->datagramstart = udp_flood_pak->network_start -
					  (FDDI_SNAP_ENCAPBYTES + 1);
	broadaddr = 0xFFFF;
	ipsize = length - FDDI_SNAP_ENCAPBYTES;
	break;
      case FS_TOKEN:
	riflen = input->rxrifwordlen << 1;
	srcreg->argreg = CTR_RECEIVE_OFFSET + 1;
	udp_flood_pak->datagramstart = udp_flood_pak->network_start -
					  TRING_SNAP_ENCAPBYTES - riflen;
	broadaddr = 0xFFFF;
	ipsize = length - TRING_SNAP_ENCAPBYTES - riflen;
	break;
      case FS_HDLC:
	srcreg->argreg = MCI_SERIAL_OFFSET;
	udp_flood_pak->datagramstart = udp_flood_pak->network_start -
					  input->encsize;
	broadaddr = HDLC_BCAST;
	ipsize = length - input->encsize;
	break;
      default:
	return(FALSE);
    }
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    
    if (srcreg->readshort == broadaddr) {
	for (i = 0, good = FALSE; i < udp_forwcnt; i++) {
	    if (udp_forward_port[i] == input->sniff[SNIFFBASE+6].d.sword[1]) {
		good = TRUE;
		break;
	    }
	}
	if (!good)
	    return(FALSE);
    } else
	return(FALSE);
    /*
     * If we _would_ flood it, but this interface is blocked, then discard
     * it quickly.  This prevents the packet from floating up to process
     * level.  
     */

    if (!swinput->circ_canforward) {
	srcreg->cmdreg = MCI_CMD_RX_FLUSH;
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
    if ((input->sniff[SNIFFBASE+5].d.lword & swinput->ip_nets_mask) ==
	(swinput->ip_address & swinput->ip_nets_mask)) {
	if (!ip_spanning_anybcast ||
	    !ip_broadcast(input->sniff[SNIFFBASE+5].d.lword, swinput))
	return(FALSE);
    }
    
    udp_flood_pak->datagramsize = length;
    udp_flood_pak->if_input = swinput;
    srcreg->argreg = input->rxoffset;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    mci2buffer(&srcreg->readlong, udp_flood_pak->datagramstart, length);
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;

    ip_traffic.bcastin++;
    ip_traffic.inputs++;
    udp_traffic.inputs++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += length;
    input->counters.inputs_fast[ACCT_PROTO_IP]++;
    GET_TIMESTAMP(input->lastinput);

#ifdef notdef
    if (ippkt_debug)
	ipbug(pak, "rcvd 6");

    if (udp_debug)
	buginf("\nUDP: rcvd src=%i(%d), dst=%i(%d), length=%d",
	       n2hl(pak->srcadr), udp->sport, n2hl(pak->dstadr), udp->dport,
	       n2h(udp->length));
#endif notdef

    udp_fast_flood_int(swinput, ipsize);
    return(TRUE);
}


/*
 * ip_fast_fragmentation
 * This is one of those ungodly stubs.  I hate doing this, but there's no
 * clean subsystem to register the 7000 copy today, and I'm not going to
 * introduce one in the middle of a maintenance release.
 */

boolean ip_fast_fragmentation (hwidbtype *inidb, hwidbtype *outidb)
{
    return(FALSE);
}


/*
 * udp_fast_flood_init
 * Initialize udp fast flooding.
 *
 * Get a buffer for the datagram.
 */

void udp_fast_flood_init (void)
{
    int bufsize;

    if (!udp_flood_pak) {

	bufsize = MAXETHERSIZE;

	if (nTRs)
	    bufsize = max(bufsize, MAXTRSIZE);

	if (nfddinets)
	    bufsize = max(bufsize, MAXFDDISIZE);

	if (nhssinets)
	    bufsize = max(bufsize, MAXDS3SIZE);

	/*
	 * Allocate 3 extra bytes so that when mci2buffer is used to
	 * to do maximum-length longword copies to this buffer, there
	 * will be enough room for any potential overwrite.
	 */

	udp_flood_pak = getbuffer(bufsize + 3);

	if (!udp_flood_pak) {
	    crashdump(1);
	}
	    
    }
}
