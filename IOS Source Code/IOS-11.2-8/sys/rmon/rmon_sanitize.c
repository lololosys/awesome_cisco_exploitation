/* $Id: rmon_sanitize.c,v 3.2.34.1 1996/03/18 21:45:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_sanitize.c,v $
 *------------------------------------------------------------------
 * rmon_sanitize.c: security scrub for captured packets
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_sanitize.c,v $
 * Revision 3.2.34.1  1996/03/18  21:45:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.1  1996/03/07  10:41:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1996/01/18  01:14:16  jjohnson
 * CSCdi47080:  Enable limited RMON packet capture
 *
 * Revision 3.1  1996/01/04  20:13:57  jjohnson
 * Initial Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "ieee.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "rmon_sanitize.h"

/*
 * The packet capture functionality of the RMON MIB is a hugh security
 * hole.  In order to prevent the unauthorized disclosure of information,
 * this function applies a hard-coded security posture to all packets.
 * This posture attempts to zero out the data payload while keeping all
 * headers intact.
 */
void
rmon_sanitize(paktype *pak)
{
    uchar      *endptr;
    uchar      *zeroptr;
    ulong	zerocnt;
    iphdrtype  *iphdr;
    ulong	iphdrsize;
    tcptype    *tcphdr;
    ulong	tcphdrsize;

    /*
     * keep track of the end of the datagram
     */
    endptr = pak->datagramstart + pak->datagramsize;

    /*
     * by default we will clear everything beyond the link
     * level encapsulation header
     */
    zeroptr = pak->network_start;
    zerocnt = pak->datagramsize - pak->encsize;

    /*
     * basic sanity checking
     */
    if ((zeroptr < pak->datagramstart) || ((zeroptr + zerocnt) > endptr))
	return;

    /*
     * preserve "known" network headers
     */

    if ((pak->rxtype == RXTYPE_DODIP) ||
	(pak->rxtype == RXTYPE_SNAP_DODIP)) {

	/*
	 * preserve the IP header
	 */
	iphdr = (iphdrtype *)zeroptr;
	iphdrsize = ipheadsize(iphdr);
	zeroptr += iphdrsize;
	zerocnt -= iphdrsize;

	/*
	 * different IP protocols get different treatment
	 */
	switch (iphdr->prot) {

	case (IGMP_PROT):
	case (GGP_PROT):
	case (EGP_PROT):
	case (IGRP_PROT):
	case (NHRP_PROT):
	case (HELLO_PROT):
	case (NEWIGRP_PROT):
	case (OSPF_PROT):
	case (NOSIP_PROT):
	    /*
	     * for all of the above, preserve the entire packet
	     */
	    return;

	case (ICMP_PROT):
	    /*
	     * preserve the ICMP header, zero the data
	     */
	    zeroptr += ICMPHEADERBYTES;
	    zerocnt -= ICMPHEADERBYTES;
	    break;

	case (TCP_PROT):
	    /*
	     * preserve the TCP header, zero the data
	     */
	    tcphdr = (tcptype *)zeroptr;
	    tcphdrsize = tcphdr->dataoffset << 2;
	    zeroptr += tcphdrsize;
	    zerocnt -= tcphdrsize;
	    break;

	case (UDP_PROT):
	    /*
	     * preserve the UDP header, zero the data
	     */
	    zeroptr += UDPHEADERBYTES;
	    zerocnt -= UDPHEADERBYTES;
	    break;

	default:
	    /*
	     * by default anything beyond the ip header is zeroed,
	     * zeroptr and zerocnt are already set, so just exit
	     */
	    break;
	}

    } else if (pak->rxtype == RXTYPE_RFC826_ARP) {
	/* preserve all data */
	return;
    }

    /*
     * the packet zeroing pointer and counter are positioned
     * so clear the data payload from the packet
     */
    if (zerocnt && ((zeroptr + zerocnt) <= endptr)) {
	memset(zeroptr, 0, zerocnt);
    }
}
