/* $Id: ipaccess_inline.h,v 3.4.4.7 1996/07/20 01:13:17 bew Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_inline.h,v $
 *------------------------------------------------------------------
 * ipaccess_inline.h -- IP Access List Inline Definitions
 *
 * February 1995, Greg Christy
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_inline.h,v $
 * Revision 3.4.4.7  1996/07/20  01:13:17  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.4.4.6  1996/07/12  02:27:02  hou
 * CSCdi59825:  Fast Switching is broken in the presence of access-lists
 * Branch: California_branch
 * Use GETSHORT to avoid costly misaligned-access handling.
 *
 * Revision 3.4.4.5  1996/06/18  20:43:43  sbelair
 * CSCdi42319:  Lock&Key idle timer does not work correctly
 * Branch: California_branch
 *
 * Add a timestamp to the slowtype structure for use with dynamic access
 * lists. Record timestamp whenever there is a hit on the access list and
 * base the idle time expiration off  of  this time of last reference.
 *
 * Revision 3.4.4.4  1996/05/12  23:09:24  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/05/04  01:02:58  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.4.4.2  1996/04/19  15:41:27  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.11  1996/04/17  23:01:43  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 2.1.6.10  1996/04/09  20:36:36  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.9  1996/03/19  08:08:35  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.8  1996/02/13  19:17:56  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.7  1996/01/19  06:55:20  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 2.1.6.6  1996/01/04  19:02:17  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 2.1.6.5  1995/12/17  01:40:05  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.4  1995/11/17  17:00:30  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.3  1995/09/27  23:21:09  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.2  1995/09/27  06:09:31  carrel
 * Branch: IosSec_branch
 * Allow both Encryption and Decryption to occur to the same packet.
 *
 * Revision 2.1.6.1  1995/09/26  04:13:00  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 3.4.4.1  1996/03/18  20:16:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  06:58:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/07  09:46:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  14:25:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/07  12:46:10  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Fix alignment errors in access list checks for inbound tokenring.
 *
 * Revision 3.3  1995/11/30  04:41:42  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.2  1995/11/17  09:34:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:04:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/10  20:59:02  gchristy
 * CSCdi38534:  icmp access lists broken
 * - casting is evil
 *
 * Revision 2.2  1995/08/05  00:01:02  hcwoo
 * CSCdi38052:  High CPU utilization on 4500s
 *
 * Revision 2.1  1995/06/07  20:58:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"

/*
 * get_ipacl_inline
 * Given an access-list number, return a pointer to the list. Returns
 * NULL if list doesn't exist, or the number is out of range.
 */
static inline accessptr *
ip_getacl_inline(int num)
{

    if ((num < MINFASTACC) || (num > MAXSLOWACC))
	return(NULL);

    if (!acl_array[num]) 
	return(NULL);

    return(acl_array[num]->list);
}

/*
 * ip_slow_check_inline
 * Perform a slow access check on the given parameters.
 * Return TRUE to accept (forward), FALSE to reject.
 *
 */

static inline boolean
ip_slow_check_inline (acl_headertype *acl, uint interrupt_context,
		      idbtype *input, hwaddrtype *src_snpa,
		      uint protocol,
		      ipaddrtype source, ipaddrtype destination,
		      ulong sport, ulong dport,
		      boolean ackrst, uchar tosbyte, int *cryptoflags)
{
    slowtype *item, *inclitem, *next;
    uchar tos, precedence;

    /*
     * Find the access list.  Return TRUE (pass the packet)
     * if the list doesn't exist or is illegal.
     */
    if (!acl || !acl->list)
	return(TRUE);

    if ((acl->type != ACL_IP_EXTENDED) &&
	(acl->type != ACL_IP_NAMED_EXTENDED)) 
	return(TRUE);

    item = (slowtype *) acl->list;

    ackrst = ackrst && (protocol == TCP_PROT);
    tos = tosbyte & IP_TOS_MASK;
    precedence = tosbyte & IP_TOS_PRECEDENCE;
    
    /*
     * Linear scan through list, look for a match.  Doing a continue here
     * implies that there's no match, so we should go to the next item in
     * the list.
     */
    next = NULL;
    inclitem = NULL;
    for (; item; item = next) {
	next = item->next;
	/*
	 * De-NEST from a nested Dynamic list.
	 */
	if (inclitem && next == NULL) {
	    next = inclitem->next;
	    inclitem = NULL;
	}
	/*
	 * Nest down if necessary.  Check to see if we stumbled into a
	 * dynamic list with temporary entries.
	 */
	if (item->dynamic) {
	    if (item->dynacl_p->item) {
		inclitem = item;
		item = item->dynacl_p->item;
		GET_TIMESTAMP(item->lastref); 
		if (item->next) {
		    next = item->next;
		} else {
		    /*
		     * Dynamic list has only 1 entry 
		     */
		    next = inclitem->next;
		    inclitem = NULL;
		}
	    } else {
		continue;	/* Dynamic list is empty */
	    }
	}
	if (item->source != (source & ~item->srcmask))
	    continue;
	if (item->destination != (destination & ~item->dstmask))
	    continue;

	/*
	 * The next check is an optimization for benchmarking.  It allows
	 * us to kick out pure source and destination filters more quickly.
	 */
	if (item->protocol == IP_PROT) {
	    if (!item->fancy)
		goto match;
	} else if (item->protocol != protocol)
	    continue;
	if (item->established && !ackrst)
	    continue;

	switch (item->dstopr) {
	case OPC_LT:
	    if (!(dport < item->dport))
		continue;
	    break;
	case OPC_GT:
	    if (!(dport > item->dport))
		continue;
	    break;
	case OPC_EQ:
	    if (!(dport == item->dport))
		continue;
	    break;
	case OPC_NEQ:
	    if (!(dport != item->dport))
		continue;
	    break;
	case OPC_RANGE:
	    if (!((dport >= item->dport) &&
		  (dport <= item->dport2)))
		continue;
	    break;
	case OPC_ONEBYTE:
	    /*
	     * This case only happens for ICMP and IGMP.
	     */
	    if (!(sport == item->dport))
		continue;
	    break;
	case OPC_TWOBYTES:
	    /*
	     * This case only happens for ICMP.
	     */
	    if (!((sport == item->dport) &&
		  (dport == item->dport2)))
		continue;
	    break;
	case OPC_NONE:
	default:
	    break;
	}

	if (!item->fancy)
	    goto match;

	if (item->precedence &&
	    (item->precedence_value != precedence))
	    continue;

	if (item->tos &&
	    (item->tos_value != tos))
	    continue;
	
	switch (item->srcopr) {
	case OPC_LT:
	    if (!(sport < item->sport))
		continue;
	    break;
	case OPC_GT:
	    if (!(sport > item->sport))
		continue;
	    break;
	case OPC_EQ:
	    if (!(sport == item->sport))
		continue;
	    break;
	case OPC_NEQ:
	    if (!(sport != item->sport))
		continue;
	    break;
	case OPC_RANGE:
	    if (!((sport >= item->sport) &&
		  (sport <= item->sport2)))
		continue;
	    break;
	case OPC_NONE:
	default:
	    break;
	}

      match:
	if (item->log) {
	    if (interrupt_context) {
                if (cryptoflags) 
		    *cryptoflags |= CRYPTO_ACL_LOG;
		return(FALSE);	/* drop down to process level */
            }
	    if (item->log == IP_ACCESS_LOG_INPUT) /* Log input info */
		accesslog_cache(acl, item->grant, input, src_snpa,
				source, destination, protocol, sport,
				dport); 
	    else		/* Normal logging */
		accesslog_cache(acl, item->grant, NULL, NULL,
				source, destination, protocol, sport,
				dport); 
	}
	item->hits++;
	return(item->grant);

    }
    return(FALSE);
}

/*
 * Perform an access check on a routed, outgoing IP datagram.
 * Returns TRUE to send datagram, FALSE otherwise.
 *
 * In HIGH-END systems, a mirror image of this function exists for IP
 * fastswitching, if changes are made to this function the
 * corresponding changes must be made to *mci_ip_fastcheck* or
 * *mci_ip_slowcheck_inline*.
 */
static inline boolean
ip_accesscheck_main_inline (paktype *pak, acl_headertype *acl,
			    hwaddrtype *src_snpa,
			    uint interrupt_context, boolean reverse)
{
    ushort dport, sport, frag;
    boolean ackrst;
    idbtype *input;
    iphdrtype *ip;
    uchar protocol, tosbyte;
    ipaddrtype source, destination;
    tcptype *tcp;
    udptype *udp;
    icmptype *icmp;
    igmptype *igmp;

    ip = (iphdrtype *)ipheadstart(pak);
    
    if (!acl)			/* Punt if undefined list */
	return(TRUE);

    switch (acl->type) {
    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	if (reverse)
	    return(nfast_check(acl, GETLONG(&ip->dstadr)));
	else
	    return(nfast_check(acl, GETLONG(&ip->srcadr)));
    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:
	break;

    default :
	return(TRUE);
    }    

    protocol = ip->prot;
    dport = 0;
    sport = 0;
    ackrst = FALSE;
    if (acl->tp_header_required) {
	/*
	 * Packet doesn't have TCP/UDP header, but we don't need it
	 * anyways.  However, there's a trick that can be done with
	 * fragmentation where you can overlay the ackrst bits and
	 * spoof the established processing, so don't permit non-zero
	 * fragments that start inside the TCP header.
	 * Don't reference ip->fo directly, since this may be unaligned
	 * for some encapsulations (e.g., tokenring with rif, ISL vLAN).
	 */
	frag = GETSHORT((uchar *)ip + IP_FRAG_INFO_BYTE) & FOMASK;
	if (frag)
	    return (frag > TCP_FLAGS_FRAGMENT) || (protocol != TCP_PROT);

	/*
	 * Grab info outside of IP header.
	 */
	if (protocol == TCP_PROT) {
	    tcp = (tcptype *) ipdatastart(pak);
	    sport = GETSHORT(&tcp->sourceport);
	    dport = GETSHORT(&tcp->destinationport);

	    /* 
	     * Don't reference tcp->ack and tcp->rst directly, 
	     * since this may be unaligned for some encapsulations 
	     * (e.g., tokenring with rif, ISL vLAN, etc). 
	     * Note: we assume BIGENDIAN is defined, if the
	     * fastswitch code is ported to LITTLEENDIAN platform,
	     * you might need to change this line.
	     */
	    ackrst = GETSHORT((uchar *)tcp + TCP_DOFF_FLAGS_OFFSET) &
		TCP_ACKRST_MASK;
	} else if (protocol == UDP_PROT) {
	    udp = (udptype *) ipdatastart(pak);
	    sport = GETSHORT(&udp->sport);
	    dport = GETSHORT(&udp->dport);
	} else if (protocol == ICMP_PROT) {
	    icmp = (icmptype *) ipdatastart(pak);
	    sport = icmp->type;
	    dport = icmp->code;
	} else if (protocol == IGMP_PROT) {
	    igmp = (igmptype *) ipdatastart(pak);
	    sport = igmp->type;
	}
    }
    
    source = GETLONG(&ip->srcadr);
    destination = GETLONG(&ip->dstadr);
    tosbyte = ip->tos;

    input = pak->if_input;

    if (reverse) {
	if ((protocol == TCP_PROT) || (protocol == UDP_PROT)) {
	    ushort tport;
	    tport = sport;
	    sport = dport;
	    dport = tport;
	}
	return(ip_slow_check_inline(acl, interrupt_context, input,
				    src_snpa, protocol,
				    destination, source, sport, dport,
				    ackrst, tosbyte, &pak->cryptoflags));
    } else {
	return(ip_slow_check_inline(acl, interrupt_context, input,
				    src_snpa, protocol,
				    source, destination, sport, dport,
				    ackrst, tosbyte, &pak->cryptoflags));
    }
}

static inline boolean
ip_accesscheck_inline (paktype *pak, acl_headertype *acl,
		       hwaddrtype *src_snpa, uint interrupt_context) 
{
    return(ip_accesscheck_main_inline(pak, acl, src_snpa, interrupt_context,
				      FALSE));  
}
static inline boolean
ip_accesscheck_reverse_inline (paktype *pak, acl_headertype *acl,
			       hwaddrtype *src_snpa, uint interrupt_context) 
{
    return(ip_accesscheck_main_inline(pak, acl, src_snpa, interrupt_context,
				      TRUE)); 
}

