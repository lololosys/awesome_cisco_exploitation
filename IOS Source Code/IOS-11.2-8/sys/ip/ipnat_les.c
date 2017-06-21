/* $Id: ipnat_les.c,v 3.1.6.5 1996/08/13 21:27:40 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_les.c,v $
 *------------------------------------------------------------------
 * IP Network Address Translation, Low-end FS
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_les.c,v $
 * Revision 3.1.6.5  1996/08/13  21:27:40  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.4  1996/07/23  00:31:45  ahh
 * CSCdi63734:  NAT: simplify some code
 * Remove unnecessary variables.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/06/03  23:23:16  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/05/13  23:21:05  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:23  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:37  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include "types.h"
#include "ip.h"
#include "../ui/debug.h"
#include "ip_debug.h"
#include "subsys.h"
#include "fastswitch_registry.h"
#include "../iprouting/route.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "ipaccess_private.h"
#include "ipaccess2.h"
#include "../tcp/tcpinternal.h"
#include "ipnat_private.h"


/*
 * ipnat_les_udp_fixup
 *
 * Fix the UDP header.
 */

static inline void
ipnat_les_udp_fixup (udptype *udp, ipnat_entry *nat,
		     enum ipnat_lookup lookup,
		     ipaddrtype osrcadr, ipaddrtype nsrcadr,
		     ipaddrtype odstadr, ipaddrtype ndstadr)
		     
{
    ushort osport, nsport, odport, ndport;
    boolean extended;
    ushort checksum;

    /*
     * Save old ports, and figure out new port values.
     */
    osport = GETSHORT(&udp->sport);
    odport = GETSHORT(&udp->dport);
    extended = nat->flags & IPNF_EXTENDED;
    if (lookup == IPNAT_LOCAL) {
	nsport = extended ? nat->port[IPNAT_INSIDE][IPNAT_GLOBAL] : osport;
	ndport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] : odport;
    } else {
	nsport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL] : osport;
	ndport = extended ? nat->port[IPNAT_INSIDE][IPNAT_LOCAL] : odport;
    }

    /*
     * Save the old checksum temporarily.
     */
    checksum = GETSHORT(&udp->checksum);

    /*
     * Change the source and destination port values and adjust the
     * checksum if it was originally non-zero.
     */
    if (osport != nsport) {
	PUTSHORT(&udp->sport, nsport);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: UDP s=%d->%d, d=%d", osport, nsport,
		   GETSHORT(&udp->dport));
	if (checksum)
	    checksum = ip_checksum_adjust_inline(checksum, osport, nsport);
    }
    if (odport != ndport) {
	PUTSHORT(&udp->dport, ndport);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: UDP s=%d, d=%d->%d", GETSHORT(&udp->sport),
		   odport, ndport);
	if (checksum)
	    checksum = ip_checksum_adjust_inline(checksum, odport, ndport);
    }

    /*
     * Adjust the checksum for the changes in the layer 3 addresses
     * if it was originally non-zero.
     */
    if (checksum) {
	if (osrcadr != nsrcadr) {
	    checksum = ip_checksum_adjust_inline(checksum, osrcadr >> 16,
						 nsrcadr >> 16);
	    checksum = ip_checksum_adjust_inline(checksum, osrcadr & 0xffff,
						 nsrcadr & 0xffff);
	}
	if (odstadr != ndstadr) {
	    checksum = ip_checksum_adjust_inline(checksum, odstadr >> 16,
						 ndstadr >> 16);
	    checksum = ip_checksum_adjust_inline(checksum, odstadr & 0xffff,
						 ndstadr & 0xffff);
	}

	/*
	 * Put the new checksum value into the header.
	 */
	PUTSHORT(&udp->checksum, checksum);
    }
}

/*
 * ipnat_les_tcp_fixup
 *
 * Fix the TCP header.
 */

static inline void
ipnat_les_tcp_fixup (tcptype *tcp, ipnat_entry *nat,
		     enum ipnat_lookup lookup,
		     ipaddrtype osrcadr, ipaddrtype nsrcadr,
		     ipaddrtype odstadr, ipaddrtype ndstadr)
		     
{
    ushort osport, nsport, odport, ndport;
    boolean extended;
    ushort checksum;

    /*
     * Save old ports, and figure out new port values.
     */
    osport = GETSHORT(&tcp->sourceport);
    odport = GETSHORT(&tcp->destinationport);
    extended = nat->flags & IPNF_EXTENDED;
    if (lookup == IPNAT_LOCAL) {
	nsport = extended ? nat->port[IPNAT_INSIDE][IPNAT_GLOBAL] : osport;
	ndport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] : odport;
    } else {
	nsport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL] : osport;
	ndport = extended ? nat->port[IPNAT_INSIDE][IPNAT_LOCAL] : odport;
    }

    /*
     * Save the old checksum temporarily.
     */
    checksum = GETSHORT(&tcp->checksum);

    /*
     * Change the source and destination port values
     * and adjust the checksum.
     */
    if (osport != nsport) {
	PUTSHORT(&tcp->sourceport, nsport);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: TCP s=%d->%d, d=%d", osport, nsport,
		   GETSHORT(&tcp->destinationport));
	checksum = ip_checksum_adjust_inline(checksum, osport, nsport);
    }
    if (odport != ndport) {
	PUTSHORT(&tcp->destinationport, ndport);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: TCP s=%d, d=%d->%d",
		   GETSHORT(&tcp->sourceport), odport, ndport);
	checksum = ip_checksum_adjust_inline(checksum, odport, ndport);
    }

    /*
     * Adjust the checksum for the changes in the layer 3 addresses.
     */
    if (osrcadr != nsrcadr) {
	checksum = ip_checksum_adjust_inline(checksum, osrcadr >> 16,
					     nsrcadr >> 16);
	checksum = ip_checksum_adjust_inline(checksum, osrcadr & 0xffff,
					     nsrcadr & 0xffff);
    }
    if (odstadr != ndstadr) {
	checksum = ip_checksum_adjust_inline(checksum, odstadr >> 16,
					     ndstadr >> 16);
	checksum = ip_checksum_adjust_inline(checksum, odstadr & 0xffff,
					     ndstadr & 0xffff);
    }

    /*
     * Put the new checksum value into the header.
     */
    PUTSHORT(&tcp->checksum, checksum);
}

/*
 * ipnat_les_icmp_fixup
 *
 * Fix the ICMP header.
 */

static inline void
ipnat_les_icmp_fixup (icmptype *icmp, ipnat_entry *nat,
		      enum ipnat_lookup lookup,
		      ipaddrtype osrcadr, ipaddrtype nsrcadr,
		      ipaddrtype odstadr, ipaddrtype ndstadr)
		     
{
    ushort oid, nid;
    ushort checksum;

    /*
     * When doing overload, we disambiguate pings by fiddling with the
     * identifier field in the ICMP header.  The new ID number is
     * stored in the inside port field of the translation entry.
     */
    if ((icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY) &&
	(nat->flags & IPNF_EXTENDED)) {
	oid = icmp->identifier;
	nid = nat->port[IPNAT_INSIDE][ipnat_opposite_lookup(lookup)];
	if (oid != nid) {
	    PUTSHORT(&icmp->identifier, nid);
	    if (ipnat_debug && (!ipdebug_acl ||
				ipnat_accesscheck(nat, ipdebug_acl)))
		buginf("\nNAT*: ICMP id=%d->%d", oid, nid);
	    checksum = GETSHORT(&icmp->checksum);
	    checksum = ip_checksum_adjust_inline(checksum, oid, nid);
	    PUTSHORT(&icmp->checksum, checksum);
	}
    }
}

/*
 * ipnat_les_l3_fixup
 *
 */

static boolean
ipnat_les_l3_fixup (paktype *pak, iphdrtype *ip, ushort frag, ipnat_entry *nat,
		    enum ipnat_lookup lookup)
{
    uchar *l4hdr;
    ipaddrtype osrcadr, nsrcadr, odstadr, ndstadr;
    ushort tl, ihl, checksum;
    int dglen;

    osrcadr = GETLONG(&ip->srcadr);
    odstadr = GETLONG(&ip->dstadr);
    if (lookup == IPNAT_LOCAL) {
	nsrcadr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	ndstadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL] : odstadr;
    } else {
	nsrcadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL] : osrcadr;
	ndstadr = nat->address[IPNAT_INSIDE][IPNAT_LOCAL];
    }

    /*
     * Can't do layer 4 work unless this is fragment zero
     * and is large enough.
     */
    if (frag == 0) {
	l4hdr = ipdatastart_iph(ip);

	/*
	 * Get size of datagram to make sure we don't poke
	 * into headers that aren't actually there... This might
	 * be a fragment, or a truncated non-fragment.
	 */
	tl = GETSHORT(&ip->tl);
	ihl = (GETSHORT((uchar *) ip + IP_IHL_BYTE) & IHLMASK) >> IHLSHIFT;
	dglen = tl - (ihl << 2);

	switch (ip->prot) {
	case UDP_PROT:
	    if (dglen < UDPHEADERBYTES)
		break;
	    ipnat_les_udp_fixup((udptype *) l4hdr, nat, lookup,
				osrcadr, nsrcadr, odstadr, ndstadr);
	    break;

	case TCP_PROT:
	    if (dglen < TCPHEADERBYTES)
		break;
	    ipnat_les_tcp_fixup((tcptype *) l4hdr, nat, lookup,
				osrcadr, nsrcadr, odstadr, ndstadr);
	    break;

	case ICMP_PROT:
	    if (dglen < ICMPHEADERBYTES)
		break;
	    ipnat_les_icmp_fixup((icmptype *) l4hdr, nat, lookup,
				 osrcadr, nsrcadr, odstadr, ndstadr);
	    break;

	default:
	    break;
	}
    }

    if (osrcadr != nsrcadr) {
	PUTLONG(&ip->srcadr, nsrcadr);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: s=%i->%i, d=%i [%d]", osrcadr, nsrcadr,
		   GETLONG(&ip->dstadr), GETSHORT(&ip->id));
	checksum = GETSHORT(&ip->checksum);
	checksum = ip_checksum_adjust_inline(checksum, osrcadr >> 16,
					     nsrcadr >> 16);
	checksum = ip_checksum_adjust_inline(checksum, osrcadr & 0xffff,
					     nsrcadr & 0xffff);
	PUTSHORT(&ip->checksum, checksum);
    }
    if (odstadr != ndstadr) {
	PUTLONG(&ip->dstadr, ndstadr);
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT*: s=%i, d=%i->%i [%d]", GETLONG(&ip->srcadr),
		   odstadr, ndstadr, GETSHORT(&ip->id));
	checksum = GETSHORT(&ip->checksum);
	checksum = ip_checksum_adjust_inline(checksum, odstadr >> 16,
					     ndstadr >> 16);
	checksum = ip_checksum_adjust_inline(checksum, odstadr & 0xffff,
					     ndstadr & 0xffff);
	PUTSHORT(&ip->checksum, checksum);
    }

    return(FALSE);
}

/*
 * ipnat_les_translate_before_routing
 *
 * Do a global->local translation on a packet.
 * I.e., a packet is coming inside from the outside, so
 * fix up the destination address.
 *
 * Returns TRUE if this packet should be bumped to process level, FALSE
 * if fast-switching can continue.  This strange logic is necessary so that
 * we handle properly the case where this subsystem is not in the image.
 * (A STUB service point with no registered function returns 0 (FALSE)).
 */

static boolean
ipnat_les_translate_before_routing (void *hwidborpakptr)
{
    paktype *pak;
    ipnat_entry *nat;
    iphdrtype *ip;
    tcptype *tcp;
    udptype *udp;
    icmptype *icmp;
    ipaddrtype srcadr, dstadr;
    ushort sport, dport;
    ushort tcpflags;
    ipnat_mapping *mapping;

    pak = hwidborpakptr;
    ip = (iphdrtype *) ipheadstart(pak);

    dstadr = GETLONG(&ip->dstadr);
    if (dstadr == IP_LIMITED_BROADCAST || dstadr == IP_BSD_LIMITED_BROADCAST)
	return(FALSE);

    if (get_ip_morefragments(ip) || getfo(ip)) {
	/* Would be nice to know if overloading or not, but oh well. */
	return(TRUE);
    } else {
	switch (ip->prot) {
	case TCP_PROT:
	    tcp = (tcptype *) ipdatastart_iph(ip);
	    sport = GETSHORT(&tcp->sourceport);
	    dport = GETSHORT(&tcp->destinationport);
	    if (ipnat_special_tcpport(sport) || ipnat_special_tcpport(dport))
		return(TRUE);	/* Do these at process level. */
	    tcpflags = GETSHORT((uchar *) tcp + TCP_DOFF_FLAGS_OFFSET);
	    if (tcpflags & (TCP_SYN_MASK | TCP_FIN_MASK | TCP_RST_MASK))
		return(TRUE);	/* Do this at process level. */
	    break;

	case UDP_PROT:
	    udp = (udptype *) ipdatastart_iph(ip);
	    sport = GETSHORT(&udp->sport);
	    dport = GETSHORT(&udp->dport);
	    if (ipnat_special_udpport(sport) || ipnat_special_udpport(dport))
		return(TRUE);	/* Do these at process level. */
	    break;

	case ICMP_PROT:
	    icmp = (icmptype *) ipdatastart_iph(ip);
	    if (ipnat_special_icmptype(icmp->type))
		return(TRUE);	/* Do these at process level. */
	    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY)
		sport = dport = GETSHORT(&icmp->identifier);
	    else
		sport = dport = 0;
	    break;

	default:
	    sport = dport = 0;
	    break;
	}
    }

    srcadr = GETLONG(&ip->srcadr);

    nat = ipnat_find_entry(dstadr, srcadr, dport, sport, ip->prot,
			   IPNAT_GLOBAL);
    if (nat) {
	/*
	 * If there is an editing context and the special processing
	 * isn't done yet, then process level needs to work on this
	 * flow, so send this packet to process level.
	 */
	if (nat->edit_context && !(nat->more_flags & IPNF2_PROCESS_DONE))
	    return(TRUE);

	/* Ready to translate. */
	ipnat_stats.hits++;
    } else {
	/* Try looking more generally. */

	/*
	 * See if the source address requires translation into local space.
	 */
	if (ipnat_find_entry(IPADDR_ZERO, srcadr, 0, 0, 0, IPNAT_GLOBAL)) {
	    /* Too much work, bail. */
	    return(TRUE);
	}

	nat = ipnat_find_entry(dstadr, IPADDR_ZERO, 0, 0, 0, IPNAT_GLOBAL);
	if (nat && !(nat->flags & IPNF_EXTENDABLE)) {
	    /* Ready to translate. */
	    ipnat_stats.hits++;
	} else {
	    /*
	     * See if we have a mapping for this destination address.
	     * I need a faster way of doing this!
	     */
	    mapping = ipnat_find_mapping(pak, dstadr, IPNAT_GLOBAL,
					 IPNAT_OUTSIDE);
	    if (!mapping) {
		/*
		 * No mapping, so no translation will be created
		 * at process level, so switch the packet now.
		 */
		return(FALSE);
	    }

	    /* No creation at interrupt level. */
	    return(TRUE);
	}
    }

    if (ipnat_detailed_debug) {
	char *protstring;

	protstring = lookup_id_string(proto_items, ip->prot);
	buginf("\nNAT*: o: %s (%i, %d) -> (%i, %d) [%d]", protstring ?
	       protstring : "?", srcadr, sport, dstadr, dport,
	       GETSHORT(&ip->id));
    }

    return(ipnat_les_l3_fixup(pak, ip, 0, nat, IPNAT_GLOBAL));
}

/*
 * ipnat_les_translate_after_routing
 *
 * Do a local->global translation on a packet.
 * I.e., a packet is going outside, so fix up the source address.
 *
 * Returns TRUE if this packet should be bumped to process level,
 * FALSE if fast-switching can continue.  This strange logic is
 * necessary so that the case where this subsystem is not in the image
 * is handled properly.  A missing STUB returns 0 (FALSE).
 */

static boolean
ipnat_les_translate_after_routing (void *hwidborpakptr)
{
    paktype *pak;
    ipnat_entry *nat;
    iphdrtype *ip;
    tcptype *tcp;
    udptype *udp;
    icmptype *icmp;
    ipaddrtype srcadr, dstadr;
    ushort sport, dport;
    ushort tcpflags;
    boolean can_overload;

    pak = hwidborpakptr;
    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * Find the inside->outside translation.
     */

    if (get_ip_morefragments(ip) || getfo(ip)) {
	/* Would be nice to know if overloading or not, but oh well. */
	return(TRUE);
    } else {
	switch (ip->prot) {
	case TCP_PROT:
	    tcp = (tcptype *) ipdatastart_iph(ip);
	    sport = GETSHORT(&tcp->sourceport);
	    dport = GETSHORT(&tcp->destinationport);
	    if (ipnat_special_tcpport(sport) || ipnat_special_tcpport(dport))
		return(TRUE);	/* Do these at process level. */
	    tcpflags = GETSHORT((uchar *) tcp + TCP_DOFF_FLAGS_OFFSET);
	    if (tcpflags & (TCP_SYN_MASK | TCP_FIN_MASK | TCP_RST_MASK))
		return(TRUE);	/* Do this at process level. */
	    can_overload = TRUE;
	    break;

	case UDP_PROT:
	    udp = (udptype *) ipdatastart_iph(ip);
	    sport = GETSHORT(&udp->sport);
	    dport = GETSHORT(&udp->dport);
	    if (ipnat_special_udpport(sport) || ipnat_special_udpport(dport))
		return(TRUE);	/* Do these at process level. */
	    can_overload = TRUE;
	    break;

	case ICMP_PROT:
	    icmp = (icmptype *) ipdatastart_iph(ip);
	    if (ipnat_special_icmptype(icmp->type)) 
		return(TRUE);	/* Do these at process level. */
	    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY) {
		sport = dport = GETSHORT(&icmp->identifier);
		can_overload = TRUE;
	    } else {
		sport = dport = 0;
		can_overload = FALSE;
	    }
	    can_overload = FALSE;
	    break;

	default:
	    sport = dport = 0;
	    can_overload = FALSE;
	    break;
	}
    }

    srcadr = GETLONG(&ip->srcadr);
    dstadr = GETLONG(&ip->dstadr);

    nat = ipnat_find_entry(srcadr, dstadr, sport, dport, ip->prot,
			   IPNAT_LOCAL);
    if (nat) {
	/*
	 * If there is an editing context and the special processing
	 * isn't done yet, then process level needs to work on this
	 * flow, so send this packet to process level.
	 */
	if (nat->edit_context && !(nat->more_flags & IPNF2_PROCESS_DONE))
	    return(TRUE);

	/* Ready to translate. */
	ipnat_stats.hits++;
    } else {
	ipnat_mapping *mapping;

	/* Try looking more generally. */

	/*
	 * See if the destination address requires translation
	 * into global space.
	 */
	if (ipnat_find_entry(IPADDR_ZERO, dstadr, 0, 0, 0, IPNAT_LOCAL)) {
	    /* Too much work, bail. */
	    return(TRUE);
	}

	nat = ipnat_find_entry(srcadr, IPADDR_ZERO, 0, 0, 0, IPNAT_LOCAL);
	if (nat && !(nat->flags & IPNF_EXTENDABLE)) {
	    if (can_overload && !(nat->flags & IPNF_EXTENDED)) {
		mapping = ipnat_find_mapping(pak,
				nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				IPNAT_LOCAL, IPNAT_INSIDE);
		if (mapping && (mapping->flags & IPNMF_OVERLOAD)) {
		    /* Make a new entry from this one at process level. */
		    return(TRUE);
		}
	    }
	    /* Ready to translate. */
	    ipnat_stats.hits++;
	} else {
	    /*
	     * See if we have a mapping for this source address.
	     * I need a faster way of doing this!
	     */
	    mapping = ipnat_find_mapping(pak, srcadr, IPNAT_LOCAL,
					 IPNAT_INSIDE);
	    if (!mapping) {
		/*
		 * No mapping, so no translation will be created
		 * at process level, so switch the packet now.
		 */
		return(FALSE);
	    }

	    /* Create a new entry at process level. */
	    return(TRUE);
	}
    }

    if (ipnat_detailed_debug) {
	char *protstring;

	protstring = lookup_id_string(proto_items, ip->prot);
	buginf("\nNAT*: i: %s (%i, %d) -> (%i, %d) [%d]", protstring ?
	       protstring : "?", srcadr, sport, dstadr, dport,
	       GETSHORT(&ip->id));
    }

    return(ipnat_les_l3_fixup(pak, ip, 0, nat, IPNAT_LOCAL));
}

/*
 * ipnat_les_init
 * Initialize NAT FS support 
 */

static void
ipnat_les_init (subsystype *subsys)
{
    reg_add_fast_translate_after_routing(RXTYPE_DODIP,
					 ipnat_les_translate_after_routing,
					 NULL);
    reg_add_fast_translate_before_routing(RXTYPE_DODIP,
					  ipnat_les_translate_before_routing,
					  NULL);
}

/*
 * NAT FS subsystem header
 */

#define IPNAT_LESFS_MAJVERSION	1
#define IPNAT_LESFS_MINVERSION	0
#define IPNAT_LESFS_EDITVERSION	1

SUBSYS_HEADER(ipnat_les_fs, IPNAT_LESFS_MAJVERSION, IPNAT_LESFS_MINVERSION,
	      IPNAT_LESFS_EDITVERSION, ipnat_les_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch");
