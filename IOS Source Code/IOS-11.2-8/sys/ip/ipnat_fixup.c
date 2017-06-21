/* $Id: ipnat_fixup.c,v 3.1.6.11 1996/09/08 22:32:18 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_fixup.c,v $
 *------------------------------------------------------------------
 * IP Network Address Translation packet fixups
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_fixup.c,v $
 * Revision 3.1.6.11  1996/09/08  22:32:18  ahh
 * CSCdi68431:  NAT: rcp not working anymore when overloading
 * Forgot that rcp sends '\0' to rshd meaning "no stderr stream".  Doh.
 * Branch: California_branch
 *
 * Revision 3.1.6.10  1996/09/07  01:07:16  ahh
 * CSCdi68015:  NAT: multi-homing not quite right
 * Route-map-based translation needs a slightly different address
 * allocation strategy -- each inside host gets one address per
 * route-map.  Do a little cleanup.
 * Branch: California_branch
 *
 * Revision 3.1.6.9  1996/09/05  00:41:36  ahh
 * CSCdi67724:  NAT: FTP not working with rotary translation
 * Deal with the data connections properly.
 * Branch: California_branch
 *
 * Revision 3.1.6.8  1996/08/28  22:57:51  ahh
 * CSCdi65898:  NAT: rsh not working when overloading
 * rsh wants rshd to connect back to it for stderr, so need to allow this
 * flow from a random privileged port in.
 * Branch: California_branch
 *
 * Revision 3.1.6.7  1996/08/13  21:27:39  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.6  1996/07/23  13:24:31  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.6.5  1996/06/21  00:22:24  ahh
 * CSCdi60792:  NAT: traceroute not working with overload
 * Extract flow information out of the embedded transport headers.
 * Branch: California_branch
 *
 * Revision 3.1.6.4  1996/06/03  23:23:15  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/13  23:21:03  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:06:30  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:22  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:29  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include "types.h"
#include "ip.h"
#include <ciscolib.h>
#include <stdlib.h>
#include "../ui/debug.h"
#include "ip_debug.h"
#include "mgd_timers.h"
#include "ctype.h"
#include "../tcp/tcpinternal.h"		/* for GT() */
#include "ipnat_private.h"
#include "ipnat_pool.h"


/*
 * NAT fixups
 */


/*
 * ip_checksum_adjust_long
 */

static ushort
ip_checksum_adjust_long (ushort checksum, ulong old_ulong, ulong new_ulong)
{
    checksum = ip_checksum_adjust_inline(checksum, old_ulong >> 16,
					 new_ulong >> 16);
    return(ip_checksum_adjust_inline(checksum, old_ulong & 0xffff,
				     new_ulong & 0xffff));
}

/*
 * ip_checksum_adjust_short
 */

static ushort
ip_checksum_adjust_short (ushort checksum, ushort old_ushort,
			  ushort new_ushort)
{
    return(ip_checksum_adjust_inline(checksum, old_ushort, new_ushort));
}

/*
 * new_extended_entry
 *
 * Create an extended entry based on an existing simple translation entry.
 */

static ipnat_entry *
new_extended_entry (paktype *pak, iphdrtype *ip, void *transport,
		    uchar proto, ipnat_entry *nat, enum ipnat_lookup lookup)
{
    ushort inside_localport, outsideport;
    ipaddrtype outsideaddr[IPNAT_LOOKUPS];
    ipnat_mapping *mapping;
    ipnat_entry *template;
    enum ipnat_lookup opposite_lookup;
    uchar flags;
    tcptype *tcp;
    udptype *udp;

    /*
     * Figure out inside and outside ports based on the packet's
     * direction and transport protocol.
     */
    switch (proto) {
    case TCP_PROT:
	tcp = transport;
	if (lookup == IPNAT_LOCAL) {
	    inside_localport = tcp->sourceport;
	    outsideport = tcp->destinationport;
	} else {
	    inside_localport = tcp->destinationport;
	    outsideport = tcp->sourceport;
	}
	break;
    case UDP_PROT:
	udp = transport;
	if (lookup == IPNAT_LOCAL) {
	    inside_localport = udp->sport;
	    outsideport = udp->dport;
	} else {
	    inside_localport = udp->dport;
	    outsideport = udp->sport;
	}
	break;
    default:
	return(NULL);
    }

    /*
     * Determine the outside address based on the packet's direction.
     */
    if (lookup == IPNAT_LOCAL) {
	/* connection from inside */
	outsideaddr[lookup] = ip->dstadr;
    } else {
	/* connection from outside */
	outsideaddr[lookup] = ip->srcadr;
    }

    opposite_lookup = ipnat_opposite_lookup(lookup);
    flags = IPNF_EXTENDED;

    /*
     * Find the outside translation template to incorporate
     * into the translation created here.
     */
    template = ipnat_find_entry(IPADDR_ZERO, outsideaddr[lookup], 0, 0, 0,
				lookup);
    if (template) {
	outsideaddr[opposite_lookup] =
	    template->address[IPNAT_OUTSIDE][opposite_lookup];
	flags |= IPNF_OUTSIDE;
	if (ipnat_debug)
	    buginf("\nNAT: setting up outside mapping %i->%i",
		   outsideaddr[lookup], outsideaddr[opposite_lookup]);
    } else {
	outsideaddr[opposite_lookup] = outsideaddr[lookup];
    }

    /*
     * Find the mapping (if any) used to create the original translation.
     */
    mapping = ipnat_find_mapping(pak, nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				 IPNAT_LOCAL, IPNAT_INSIDE);
    if (mapping && (mapping->flags & IPNMF_OVERLOAD)) {
	/* Make a new entry from this one */
	nat = ipnat_clone_entry(nat, template, mapping, inside_localport,
				proto, outsideaddr[IPNAT_LOCAL],
				outsideport);
	/* XXX what if we get inside_globalport != inside_localport ? */
    } else {
	mapping = nat->mapping[IPNAT_INSIDE];
	nat = ipnat_create_entry(nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				 nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				 inside_localport, inside_localport,
				 outsideaddr[IPNAT_LOCAL],
				 outsideaddr[IPNAT_GLOBAL],
				 outsideport, outsideport, proto, flags);
	if (nat) {
	    nat->more_flags |= IPNF2_PORT_STOLEN;
	    if (mapping) {
		ipnat_addrpool_bump(mapping->poolname,
				    nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
		nat->mapping[IPNAT_INSIDE] = mapping;
		mapping->refcount++;
	    }
	    if (template) {
		mapping = template->mapping[IPNAT_OUTSIDE];
		nat->mapping[IPNAT_OUTSIDE] = mapping;
		if (mapping) {
		    mapping->refcount++;
		    ipnat_addrpool_bump(mapping->poolname,
					nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
		}
	    }
	}
    }

    return(nat);
}

#include "../servers/domain_private.h"
#include "../servers/domain.h"

uchar *
dns_parse_name (uchar *data, uchar *data_start, uchar *data_end,
		uchar *buffer, int buflen, int label_len, int delta)
{
    uchar len;
    ushort offset;

    while (data < data_end) {
	len = *data;
	if (len & 0xc0) {
	    /*
	     * This is an indirection to a string contained somewhere
	     * else in the record.  Process it recursively if we're
	     * actually copying the name out.
	     */
	    offset = GETSHORT(data) & 0x3fff;
	    if (delta != 0) {
		/*
		 * The IP address in the pointer recorder was translated.
		 * Update all offsets in the compressed record to reflect
		 * the change. But make sure we only update those that 
		 * point to strings placed after the reversed address label.
		 */
		if (offset > (sizeof(domaintype) + label_len)) {
		    offset += delta;
		    offset = (INDIRECT_MASK << 8) | offset;
		    PUTSHORT(data, offset);
		}
	    }
	    data += sizeof(ushort);
		
	    if (buffer)
		dns_parse_name(&data_start[offset], data_start, data_end,
			       buffer, buflen, label_len, delta);
	    return(data);
	} else {
	    data++;

	    if (len == 0) {
		if (buffer) {
		    if (buflen == 0)
			buffer--;	/* pointer was advanced past end */
		    *buffer = '\0';
		}
		return(data);	/* all done */
	    }

	    if (data >= data_end)
		break;

	    if (buffer && buflen) {
		/*
		 * Copy the label until we run out of label or buffer.
		 */
		while (len && buflen) {
		    *buffer++ = *data++;
		    len--;
		    buflen--;
		}
		if (buflen) {
		    /*
		     * Still buffer space left, mark the end of the label.
		     */
		    *buffer++ = '.';
		    buflen--;
		} else {
		    /*
		     * No buffer space for whole label, but be sure
		     * to advance the data pointer by the amount
		     * of the label not copied.
		     */
		    data += len;
		}
	    } else {
		/*
		 * Not copying the labels out, so just advance
		 * the data pointer.
		 */
		data += len;
	    }
	}
    }

    /*
     * Tie off the string and return the end pointer since
     * we fell off the end of the packet.
     */
    if (buffer) {
	if (buflen == 0)
	    buffer--;		/* pointer was advanced past end */
	*buffer = '\0';
    }
    return(data_end);
}

/*
 * ipnat_dns_fix_a_addr
 *
 * Fix the address in a DNS TYPE_A record, possibly setting up a
 * new template for future translations.  Set the ttl to 0.
 */

static boolean
ipnat_dns_fix_a_addr (paktype *pak, ipaddrtype *addressp, ulong *ttlp,
		       enum ipnat_lookup lookup)
{
    ipnat_entry *nat;
    ipaddrtype address;
    ulong ttl;
    boolean drop;

    address = GETLONG(addressp);
    ttl = GETLONG(ttlp);

    drop = FALSE;
    if (lookup == IPNAT_LOCAL) {
	/*
	 * An answer is moving from inside to outside.  Need to see if
	 * the inside_local answer has an outside_local translation.
	 * Since there might actually be several global address for
	 * this inside local address (unlikely but possible), we
	 * search for a simple/static entry, since this would indicate
	 * that they wanted this host to be known to the outside
	 * world.  If it's not found, then go create one.
	 */
	nat = ipnat_find_entry(address, IPADDR_ZERO, 0, 0, 0, IPNAT_LOCAL);
	if (nat == NULL) {
	    /* Create a new inside entry. */
	    nat = ipnat_new_inside_entry(NULL, pak, address, IPADDR_ZERO, 0, 0,
					 0, FALSE, &drop);
	}
	if (nat) {
	    if (ipnat_debug)
		buginf("\nNAT: DNS resource record %i -> %i", address,
		       nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	    PUTLONG(addressp, nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	    PUTLONG(ttlp, 0);
	}
    } else {
	/*
	 * An answer is moving from outside to inside. Need to
	 * find an existing template or static entry, or create
	 * a new template, then fix up the reply.
	 * Template must remain around for TTL.
	 */
	nat = ipnat_find_entry(IPADDR_ZERO, address, 0, 0, 0, IPNAT_GLOBAL);
	if (nat == NULL) {
	    /* Create a new outside entry. */
	    nat = ipnat_new_outside_entry(pak, address, &drop);
	}
	if (nat) {
	    if (ipnat_debug)
		buginf("\nNAT: DNS resource record %i -> %i", address,
		       nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	    PUTLONG(addressp, nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	    PUTLONG(ttlp, 0);
	}
    }
    return(!(nat == NULL && drop));
}

/*
 * ipnat_dns_fix_resource
 *
 * Fix the resource record.
 */

static uchar *
ipnat_dns_fix_resource (paktype *pak, uchar *data, uchar *data_start,
			uchar *data_end, int label_len, int delta,
			enum ipnat_lookup lookup)
{
    ushort type, class;
    ushort rdatalen;
    uchar *buffer;
    int buflen;
    ipaddrtype *addressp;
    ulong *ttlp;

    buffer = NULL;
    buflen = 0;
    data = dns_parse_name(data, data_start, data_end, buffer, buflen, 
			  label_len, delta);
    type = GETSHORT(data);
    data += sizeof(ushort);
    class = GETSHORT(data);
    data += sizeof(ushort);
    ttlp = (ulong *) data;
    data += sizeof(ulong);

    rdatalen = GETSHORT(data);
    if (data + rdatalen > data_end) {
	if (ipnat_debug)
	    buginf("\nNAT: DNS parsing trouble: rdatalen %d invalid",
		   rdatalen);
	return(data_end);
    }
    data += sizeof(ushort);

    /*
     * Now fix the resource data.
     */
    switch (type) {
    case TYPE_NS:
    case TYPE_PTR:
    case TYPE_SOA:
    case TYPE_CNAME:
	data = dns_parse_name(data, data_start, data_end, buffer, buflen,
			      label_len, delta);
	break;

    case TYPE_NB:
	/* Skip past NetBIOS flags. */
	data += sizeof(ushort);
	/*FALLTHROUGH*/
    case TYPE_A:
	addressp = (ipaddrtype *) data;
	data += sizeof(ulong);
	if (!ipnat_dns_fix_a_addr(pak, addressp, ttlp, lookup))
	    data = NULL;
	break;
    }
    return(data);
}


#define IPSTRLEN 	16	/* Max size for the ascii representation of an 
				   IP address (255.255.255.255) */
/*
 * ipnat_ascii_to_addr
 * Convert an ascii string in D.C.B.A.in-addr.arpa. from an DNS pointer 
 * record to real IP address A.B.C.D.
 */ 
static ipaddrtype 
ipnat_ascii_to_addr (uchar *buffer)
{
	
    ipaddrtype address;
    ulong addr_byte;
    int addr_cnt;
    uchar c;
    int i;

    address = 0L;
    addr_cnt = 0;
    addr_byte = 0L;

    for (i = 0; i < IPSTRLEN; i++) {
	c = *buffer++;
	if (c != '.') {
	    if ((c >= '0') && (c <= '9')) {
		/*
		 * ascii to number conversion.
		 */
		addr_byte = addr_byte * 10 + ((int)c - '0');
	    } else {
		/*
		 * This should never happen. Something other than a 
		 * digit is embedded in the ascii IP address octet. 
		 */
		return(0L);
	    }
	} else {
	    /*
	     * Parsed one byte of IP address. Reverse it's position 
	     * to get the real IP address.
	     */ 
	    address = (address >> 8 | addr_byte << 24);
	    addr_byte = 0;
	    if (++addr_cnt == 4) {
		/*
		 * We have successfully parsed a string of form D.C.B.A
		 * to an IP address A.B.C.D.
		 */
		return(address);
	    }
	}
    }
    
    /*
     * Something is wrong, we didn't get the full IP address after
     * copying IPSTRLEN characters from the ascii string.
     */
    return(0L);
}
    

/*
 * ipnat_dns_fix_ptr_addr
 * Fix the address in a DNS TYPE_PTR record. If a translation is found, 
 * replace the address with the following metrics:
 *
 *   PTR query inside -> outside:	outside local -> outside global.
 *   PTR response outside -> insdie:	outside global -> outside local.
 *   PTR query outside -> inside:	inside global -> inside local.
 *   PTR response inside -> outside:	inside local -> inside global.
 *
 */
static ipaddrtype
ipnat_dns_fix_ptr_addr (iphdrtype *ip, domaintype *dns, ipaddrtype old_addr, 
			enum ipnat_lookup lookup)
{
    ipaddrtype new_addr;
    ipnat_entry *nat = NULL;

    new_addr = 0L;
    if (lookup == IPNAT_LOCAL) {
	/*
	 * Do global address lookups.
	 */
	if (dns->qr) {
	    /* PTR response going from inside to outside. */
	    nat = ipnat_find_entry(old_addr, IPADDR_ZERO, 0, 0, 0, IPNAT_LOCAL);
	    if (nat)
		new_addr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	} else {
	    /* PTR query going from inside to outside. */
	    nat = ipnat_find_entry(IPADDR_ZERO, old_addr, 0, 0, 0, IPNAT_LOCAL);
	    if (nat)
		new_addr = nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	}
    } else {
	/*
	 * Do local address lookups.
	 */
	if (dns->qr) {
	    /* PTR response going from ouside to inside. */
	    nat = ipnat_find_entry(IPADDR_ZERO, old_addr, 0, 0, 0,
				   IPNAT_GLOBAL);
	    if (nat)
		new_addr = nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	} else {
	    /* PTR query going from outside to inside. */
	    nat = ipnat_find_entry(old_addr, IPADDR_ZERO, 0, 0, 0,
				   IPNAT_GLOBAL);
	    if (nat)
		new_addr = nat->address[IPNAT_INSIDE][IPNAT_LOCAL];
	}
    }

    if (new_addr && ipnat_debug)
	buginf("\nNAT: DNS PTR record %i -> %i", old_addr, new_addr);

    return(new_addr);
}


/*
 * ipnat_addr_to_ascii
 * Convert an IP address A.B.C.D to ascii string D.C.B.A.in-addr.arpa.
 * and returns length of the ascii string.
 */
static int 
ipnat_addr_to_ascii (ipaddrtype address, char *str)
{
    uchar *addr;
    char *cp;

    cp = str;  
    addr = (uchar *) &address;
    cp += numdotcat(addr[3],cp);
    cp += numdotcat(addr[2],cp);
    cp += numdotcat(addr[1],cp);
    cp += numdotcat(addr[0],cp);
    strcpy(cp, "in-addr.arpa.");
    return(strlen(str)); 
}
    

/*
 * pak_data_size
 * Calculate maximum number of bytes available for DNS data for a
 * packet.
 */
static int
pak_data_size (paktype *pak, iphdrtype *ip)
{
    int pak_pool_size, offset;
    int overhead, max_data_size; 

    /*
     * Maximum bytes available for DNS data in the packet is the 
     * buffer size allocated subtract the IP/UDP headers.
     */
    pak_pool_size = pak_get_pool_size(pak);
    overhead = (ip->ihl<<2) + UDPHEADERBYTES;
    max_data_size = pak_pool_size - overhead;
    /*
     * Adjust the maximum bytes available for DNS if pak->network_start
     * does not begin on (pak->data_area + ENCAPBYTES)
     */ 
    offset = pak->network_start - pak_center(pak);
    if (offset)
	max_data_size -= offset;
    return(max_data_size);
}


/*
 * ipnat_dns_shift_len
 * Compute number of bytes can be shifted for this DNS packet.
 */ 
static int
ipnat_dns_shift_len (paktype *pak, iphdrtype *ip, domaintype *dns,
		     int delta, int dns_data_len) 
{
    int pak_dns_size, max_dns_data_size;
    int truncate_len;

    /*
     * dns_data_len - current DNS data length in this packet.  
     * pak_dns_size - allocated packet buffer available for DNS 
     * 		      (including DNS header).
     * max_dns_data_size - maximum size allowed for DNS data in this 
     *                     packet.
     * truncate_len - DNS data bytes that needs to be truncated.
     */
    pak_dns_size = pak_data_size(pak, ip);
    max_dns_data_size = min(pak_dns_size, MAX_DNS_LEN) - sizeof(domaintype);
    truncate_len = (dns_data_len + delta) - max_dns_data_size;

    if (truncate_len > 0) {
	/*
	 * New DNS data length will be too big, truncat the buffer, 
	 * and set the "truncated" bit in the DNS header.
	 */
	dns_data_len -= truncate_len; 
	dns->tc = 1;
    }
    return(dns_data_len);
}


/*
 * ipnat_shift_data
 * Shift DNS data if the length of the translated address in ascii
 * format is different from the original length. 
 */
static void
ipnat_dns_shift_data (paktype *pak,iphdrtype *ip, domaintype *dns, 
		      uchar *label_start, int addr_len,
		      int dns_data_len, int delta)
{
    uchar *src, *dst;
    int copy_len;
 
    /*
     * New DNS data is going to be larger than the original.
     * We may not be able to shift the entire DNS data. Check
     * whether any data needs to be truncated.
     */
    dns_data_len = ipnat_dns_shift_len(pak, ip, dns, delta, dns_data_len);

    /*
     * Only have to shift data following the reversed address in
     * the DNS pointer record.
     */
    copy_len = dns_data_len - addr_len;
    src = label_start + addr_len;
    dst = label_start + addr_len + delta;
    /*
     * memmove() is safe for both forward and backward memory coping 
     * operations.
     */
    memmove(dst, src, copy_len);
}


/*
 * ipnat_dns_fix_ptr_record
 * Fix the DNS pointer record and translate the reversed IP address
 * embedded in the pointer record. 
 */
static uchar *
ipnat_dns_fix_ptr_record (paktype *pak, iphdrtype *ip, udptype *udp,
			  domaintype *dns, uchar *dns_data, uchar *buffer,
			  int dns_data_len, int *old_label_len, int *delta, 
			  enum ipnat_lookup lookup)
{
    ipaddrtype old_addr, new_addr;
    char *label_start;
    int new_label_len, comp_len; 
    char new_string[MAX_DNS_PTR_LEN];
    
    *old_label_len = strlen(buffer);
    old_addr = ipnat_ascii_to_addr(buffer);
    label_start = &((uchar *)dns)[sizeof(domaintype)];

    if (old_addr) {
	/*
	 * There was a valid IP address embedded in the string, check
	 * to see whether it needs to be translated.
	 */
	new_addr = ipnat_dns_fix_ptr_addr(ip, dns, old_addr, lookup);
	if (new_addr) {
	    /*
	     * The original address was translated, converted the new
	     * address back to the reversed ascii format.
	     */
	    new_label_len = ipnat_addr_to_ascii(new_addr, new_string);
	    if (new_label_len == 0) {
		return(NULL);
	    }

	    /*
	     * Shift the DNS data if the new address string length is 
	     * different from its original length.
	     */
	    *delta = new_label_len - *old_label_len;
	    if (*delta != 0) {
		ipnat_dns_shift_data(pak, ip, dns, label_start,
				     *old_label_len, dns_data_len, *delta);
		/*
		 * Make sure the pointers and UDP header are updated to 
		 * reflect the new delta in DNS data length.
		 */
		udp->length += *delta;
		dns_data += *delta;
	    }

	    /*
	     * Convert the new reversed ascii string back to the DNS 
	     * label format.
	     */
	    comp_len = comp_copy(label_start, new_string, udp->length, NULL);
	    if (comp_len == 0) {
		return(NULL);
	    }
	}
    }
    return(dns_data);
}


/*
 * ipnat_l4_udp_dns
 *
 * Do additional massaging on DNS packets.
 * May return a new packet and dispose of the original.
 * Returns NULL when fixup fails and the packet should be dropped.
 */

static paktype *
ipnat_l4_udp_dns (paktype *pak, iphdrtype *ip, udptype *udp, ipnat_entry *nat,
		  enum ipnat_lookup lookup)
{
    domaintype *dns;
    uchar *dns_data, *dns_data_end;
    int len;
    ushort count;
    uchar buffer[MAX_DNS_PTR_LEN];
    ushort type, class;
    int label_len, delta;

    /*
     * Watch for A-record reply,
     *    do source translation (lookup, then create, then translate)
     * Watch for reverse request, rewrite the request (UGLY)
     * Zone transfers?
     */

    dns = (domaintype *) udp->udpdata;
    dns_data = &((uchar *) udp->udpdata)[sizeof(domaintype)];
    len = udp->length - UDPHEADERBYTES - sizeof(domaintype);
    dns_data_end = dns_data + len;
    label_len = 0;
    delta = 0;

    count = GETSHORT(&dns->qdcount);
    while (count--) {
	/*
	 * Copy the DNS label out, just in case we need to do translation
	 * for the IP address embedded in the label.
	 */
	dns_data = dns_parse_name(dns_data, (uchar *) dns, dns_data_end,
				  buffer, MAX_DNS_PTR_LEN, label_len, delta);
	type = GETSHORT(dns_data);
	dns_data += sizeof(ushort);
	class = GETSHORT(dns_data);
	dns_data += sizeof(ushort);

	if (type == TYPE_PTR) {
	    /*
	     * This is a DNS pointer record, we may need to translate
	     * the IP addrsses embedded in the label.
	     */
	    dns_data = ipnat_dns_fix_ptr_record(pak, ip, udp, dns, dns_data,
						buffer, len, &label_len,
						&delta, lookup);
	    if (dns_data == NULL) {
		return(NULL);
	    }
	    dns_data_end += delta;
	}
    }

    count = GETSHORT(&dns->ancount);
    while (count--) {
	dns_data = ipnat_dns_fix_resource(pak, dns_data, (uchar *) dns,
					  dns_data_end, label_len, delta,
					  lookup);
	if (dns_data == NULL)
	    return(NULL);
    }

    count = GETSHORT(&dns->nscount);
    while (count--) {
	dns_data = ipnat_dns_fix_resource(pak, dns_data, (uchar *) dns,
					  dns_data_end, label_len, delta,
					  lookup);
	if (dns_data == NULL)
	    return(NULL);
    }

    count = GETSHORT(&dns->arcount);
    while (count--) {
	dns_data = ipnat_dns_fix_resource(pak, dns_data, (uchar *) dns,
					  dns_data_end, label_len, delta,
					  lookup);
	if (dns_data == NULL)
	    return(NULL);
    }
	
    udp->checksum = 0;
    udp->checksum = CheckSum((uchar *) udp, udp->length, ip->srcadr,
			     ip->dstadr, UDP_PROT);

    /*
     * The IP address in the pointer recorde has been translated and
     * the new ascii string length is different from the original
     * length. Update the total length field in IP header and do the 
     * incremental header checksum computation.
     */ 
    if (delta != 0) {
	ushort old_ip_tl;

	old_ip_tl = ip->tl;
	ip->tl += delta;
        ip->checksum = ip_checksum_adjust_short(ip->checksum, old_ip_tl,
						ip->tl);
    }
    return(pak);
}

/*
 * ipnat_l4_udp_tftp
 *
 * Handle TFTP when doing overloading.
 */

static boolean
ipnat_l4_udp_tftp (paktype *pak, iphdrtype *ip, udptype *udp, ipnat_entry *nat,
		   enum ipnat_lookup lookup)
{
    ipnat_door *door;

    if (lookup == IPNAT_LOCAL && (nat->flags & IPNF_EXTENDED)) {
	/*
	 * Create a door so that a translation can be created
	 * when the reply from the server comes back from the
	 * random port number.
	 */
	door = ipnat_find_door(UDP_PROT,
			       nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			       nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			       nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			       nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			       IPNAT_GLOBAL);
	if (door) {
	    /*
	     * It's already there, so just refresh it's timer.
	     */
	    mgd_timer_start(&door->timeout_timer, IPNAT_DOGGY_DOOR_TIMEOUT);
	    return(TRUE);
	}

	if (!ipnat_setup_door(nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			      TFTP_PORT,
			      0,
			      nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			      nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			      nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			      UDP_PROT, UDP_PROT,
			      IPNDF_OUTSIDEHOST_ANY))
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * ipnat_l4_udp_cuseeme
 *
 * Handle CU-SeeMe.
 */

/*
 * CU-SeeMe operates by having everyone send packets to a reflector
 * which then sends them to everyone else participating in the
 * conference.  There are several packet formats, and all of them
 * seem to carry the IP addresses of the conference participants
 * in them.  It makes sense that packets from the reflector carry
 * the IP addresses of the other folks who are sending you audio
 * or video, but a packet from you to the reflector can have the
 * address of another conference member.  Not sure why, maybe
 * it's for direct conferencing, bypassing the reflector.
 */

struct cuseeme_hdr {
    ushort command;
    ushort dport_1;		/* command == 1, dest port of data */
    ipaddrtype daddr_1;		/* command == 1, dest addr of data */
    ushort mystery_short;
    ushort sport;		/* all commands, source port of data */
    ipaddrtype saddr;		/* all commands, source addr of data */

    ulong mystery_longs[4];
    uchar namebuf[24];		/* sometimes this is shorter (?!) */
    ipaddrtype daddr_2;		/* command == 2, dest addr of data */
};

static void
ipnat_l4_udp_cuseeme (paktype *pak, iphdrtype *ip, udptype *udp,
		      ipnat_entry *nat, enum ipnat_lookup lookup)
{
    struct cuseeme_hdr *cuseeme;
    ipaddrtype osaddr, odaddr, nsaddr, ndaddr;
    ipaddrtype trans_saddr, trans_daddr;
    ushort osport, odport, nsport, ndport;
    ushort trans_sport, trans_dport;
    boolean extended;

    extended = nat->flags & IPNF_EXTENDED;
    /*
     * Since the protocol is secret and the packet format
     * is non-trivial, there is a bit of uncertainty in this
     * code.  The code will not translate embedded IP addresses
     * and ports that do not have the expected value.
     */
    if (lookup == IPNAT_LOCAL) {
	/*
	 * Get the new values.
	 */
	nsaddr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	nsport = nat->port[IPNAT_INSIDE][IPNAT_GLOBAL];
	ndaddr = nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	ndport = nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL];

	/*
	 * Get the expected old values.
	 */
	trans_saddr = nat->address[IPNAT_INSIDE][IPNAT_LOCAL];
	trans_sport = nat->port[IPNAT_INSIDE][IPNAT_LOCAL];
	trans_daddr = nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	trans_dport = nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL];
    } else {
	/*
	 * Get the new values.
	 */
	nsaddr = nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	nsport = nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL];
	ndaddr = nat->address[IPNAT_INSIDE][IPNAT_LOCAL];
	ndport = nat->port[IPNAT_INSIDE][IPNAT_LOCAL];

	/*
	 * Get the expected old values.
	 */
	trans_saddr = nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	trans_sport = nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	trans_daddr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	trans_dport = nat->port[IPNAT_INSIDE][IPNAT_GLOBAL];
    }

    cuseeme = (struct cuseeme_hdr *) udp->udpdata;

    /*
     * When the packet is coming from the server, the source address
     * and port embedded in the packet might be that of the server,
     * or might be that of one of the conference participants.
     *
     * If coming from the server out->in, want to do outside global lookup
     * If coming from the server in->out, want to do inside local lookup
     *
     * The problem is I don't know which is the client and which is
     * the server, so things aren't perfect here.
     */

    /*
     * Fix source port if it matches what we think it should be.
     */
    if (extended && cuseeme->sport == trans_sport) {
	osport = cuseeme->sport;
	cuseeme->sport = nsport;
    } else {
	osport = nsport;
#ifdef IPNAT_CUSEEME_DEBUG
	if (ipnat_debug)
	    buginf("\n%c: cuseeme->sport not changed from %i (%i) to %i",
		   (lookup == IPNAT_INSIDE) ? 'i' : 'o', cuseeme->sport,
		   trans_sport, nsport);
#endif
    }
    /*
     * Fix source address if it matches what we think it should be.
     */
    if (cuseeme->saddr == trans_saddr) {
	osaddr = cuseeme->saddr;
	cuseeme->saddr = nsaddr;
    } else {
	osaddr = nsaddr;
#ifdef IPNAT_CUSEEME_DEBUG
	if (ipnat_debug)
	    buginf("\n%c: cuseeme->saddr not changed from %i (%i) to %i",
		   (lookup == IPNAT_INSIDE) ? 'i' : 'o', cuseeme->saddr,
		   trans_saddr, nsaddr);
#endif
    }

    /*
     * Remainder of translation depends on what I interpret
     * to be a command.
     */
    switch (cuseeme->command) {
    case 1:
	/*
	 * Fix destination port if it matches what we think it should be.
	 */
	if (extended && cuseeme->dport_1 == trans_dport) {
	    odport = cuseeme->dport_1;
	    cuseeme->dport_1 = ndport;
	} else {
	    odport = ndport;
#ifdef IPNAT_CUSEEME_DEBUG
	    if (ipnat_debug)
		buginf("\n%c: cuseeme->dport_1 not changed from %d (%d) to %d",
		       (lookup == IPNAT_INSIDE) ? 'i' : 'o', cuseeme->dport_1,
		       trans_dport, ndport);
#endif
	}
	/*
	 * Fix destination address if it matches what we think it should be.
	 */
	if (cuseeme->daddr_1 == trans_daddr) {
	    odaddr = cuseeme->daddr_1;
	    cuseeme->daddr_1 = ndaddr;
	} else {
	    odaddr = ndaddr;
#ifdef IPNAT_CUSEEME_DEBUG
	    if (ipnat_debug)
		buginf("\n%c: cuseeme->daddr_1 not changed from %i (%i) to %i",
		       (lookup == IPNAT_INSIDE) ? 'i' : 'o', cuseeme->daddr_1,
		       trans_daddr, ndaddr);
#endif
	}
	break;
    case 2:
	/*
	 * No destination port to change.
	 */
	odport = ndport;
	/*
	 * It seems this destination address is only present
	 * when the dport_1 field of the header is 0.
	 */
	if (cuseeme->dport_1 == 0) {
	    /*
	     * Fix dest. address if it matches what we think it should be.
	     */
	    if (cuseeme->daddr_2 == trans_daddr) {
		odaddr = cuseeme->daddr_2;
		cuseeme->daddr_2 = ndaddr;
	    } else {
		odaddr = ndaddr;
#ifdef IPNAT_CUSEEME_DEBUG
		if (ipnat_debug)
		    buginf("\n%c: cuseeme->daddr_2 not changed from %i (%i) to %i",
			   (lookup == IPNAT_INSIDE) ? 'i' : 'o',
			   cuseeme->daddr_2, trans_daddr, ndaddr);
#endif
	    }
	} else {
	    odaddr = ndaddr;
	}
	break;
    default:
	/*
	 * Only embedded source address and port changes,
	 * nothing else needed.
	 */
	odaddr = ndaddr;
	odport = ndport;
	break;
    }

    if (udp->checksum) {
	if (osaddr != nsaddr)
	    udp->checksum = ip_checksum_adjust_long(udp->checksum, osaddr,
						    nsaddr);
	if (osport != nsport)
	    udp->checksum = ip_checksum_adjust_short(udp->checksum, osport,
						     nsport);
	if (odaddr != ndaddr)
	    udp->checksum = ip_checksum_adjust_long(udp->checksum, odaddr,
						    ndaddr);
	if (odport != ndport)
	    udp->checksum = ip_checksum_adjust_short(udp->checksum, odport,
						     ndport);
    }
}

/*
 * ipnat_l4_udp_nbds
 *
 * Handle NetBIOS datagram service.
 */

struct netbios_dg_hdr {
    uchar type;
    uchar flags;
    ushort id;
    ipaddrtype srcadr;
    ushort sport;
    ushort length;
    ushort offset;
};

static void
ipnat_l4_udp_nbds (paktype *pak, iphdrtype *ip, udptype *udp,
		   ipnat_entry *nat, enum ipnat_lookup lookup)
{
    struct netbios_dg_hdr *netbios_dg;
    ipaddrtype oaddr, naddr;
    ushort oport, nport;

    netbios_dg = (struct netbios_dg_hdr *) udp->udpdata;

    /*
     * Do the usual address and port translation for the address and
     * port buried in the datagram header.
     */
    oaddr = netbios_dg->srcadr;
    oport = netbios_dg->sport;
    if (lookup == IPNAT_LOCAL) {
	naddr = nat->address[IPNAT_INSIDE][IPNAT_GLOBAL];
	nport = nat->port[IPNAT_INSIDE][IPNAT_GLOBAL];
    } else {
	naddr = nat->address[IPNAT_OUTSIDE][IPNAT_LOCAL];
	nport = nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL];
    }

    /*
     * Change the fields and fix the UDP checksum.
     */
    netbios_dg->srcadr = naddr;
    if (udp->checksum && oaddr != naddr)
	udp->checksum = ip_checksum_adjust_long(udp->checksum, oaddr, naddr);
    netbios_dg->sport = nport;
    if (udp->checksum && oport != nport)
	udp->checksum = ip_checksum_adjust_short(udp->checksum, oport, nport);
}

/*
 * TCP stream editing
 */

enum ipnat_ftp_state {
    IPNAT_FTP_OPEN,
    IPNAT_FTP_PORT_ADDR,
    IPNAT_FTP_PORT_PORT,
    IPNAT_FTP_PASV_ADDR,
    IPNAT_FTP_PASV_PORT
};

enum ipnat_rsh_state {
    IPNAT_RSH_OPEN,
    IPNAT_RSH_DONE
};

enum ipnat_token {
    TOK_UNKNOWN,
    TOK_INCOMPLETE,
    TOK_PUNCT,
    TOK_EOL,
    TOK_NUMBER,
    TOK_PORT_KEYWORD,
    TOK_PASV_KEYWORD
};

/*
 * TOK_PUNCT, punctuation is in context->token_string[0]
 * TOK_NUMBER, value is in context->value
 */

static int
get_token (tcptype *tcp, int seglen, ipnat_edit_context *context, int *cnt)
{
    uchar *data;
    enum ipnat_token token;
    char ch;

    data = (uchar *) tcp + (tcp->dataoffset << 2);

    /*
     * Loop until we've run out of segment or token buffer space,
     * or until we've found a token.
     */
    for (; *cnt < seglen &&
	     context->token_end < &context->token_string[TOKEN_LEN];
	 (*cnt)++) {
	ch = data[*cnt];
	if (context->lex_state == LEX_NUMBER) {
	    /*
	     * Gathering a number -- any non-digit stops us.
	     */
	    if (!isdigit(ch)) {
		*context->token_end = '\0';
		context->token_end = context->token_string;
		context->lex_state = LEX_UNKNOWN;
		return(TOK_NUMBER);
	    }
	} else if (isspace(ch) || ispunct(ch)) {
	    if (context->token_end == context->token_string) {
		if (ispunct(ch)) {
		    context->token_string[0] = ch;
		    context->token_string[1] = '\0';
		    context->lex_state = LEX_UNKNOWN;
		    (*cnt)++;
		    return(TOK_PUNCT);
		} else {
		    if (ch == '\n') {
			context->token_string[0] = '\0';
			context->lex_state = LEX_UNKNOWN;
			(*cnt)++;
			return(TOK_EOL);
		    }
		    /* Eat leading whitespace. */
		    continue;
		}
	    }

	    /*
	     * If doing a normal keyword tokenization, white space
	     * or punctuation stops us.
	     */

	    /* Tie off the string and do token lookup */
	    *context->token_end = '\0';
	    if (strcmp(context->token_string, "PORT") == 0)
		token = TOK_PORT_KEYWORD;
	    else if (strcmp(context->token_string, "PASV") == 0)
		token = TOK_PASV_KEYWORD;
	    else
		token = TOK_UNKNOWN;
	    context->token_end = context->token_string;
	    context->lex_state = LEX_UNKNOWN;
	    return(token);
	}

	if (isdigit(ch) && context->lex_state != LEX_KEYWORD) {
	    /* See if need to start accumulating a new value. */
	    if (context->token_end == context->token_string) {
		context->lex_state = LEX_NUMBER;
		context->value = 0;
	    }
	    context->value = context->value * 10 + (ch - '0');
	} else {
	    context->lex_state = LEX_KEYWORD;
	}
	*context->token_end++ = ch;
    }

    /*
     * Hit the end of the packet.
     */
    return(TOK_INCOMPLETE);
}

/*
 * ipnat_add_new_delta
 *
 * Add a new sequence number/delta pair to the context
 * if it's not there already.
 */

static boolean
ipnat_add_new_delta (ipnat_edit_context *context, ulong seq, int delta,
		     enum ipnat_domain domain)
{
    ipnat_seq_delta_pair *p, *last, *pair;

    last = NULL;

    /*
     * See if this sequence number and delta have already been
     * recorded -- this will happen if we have retransmissions
     * of PORT commands or PASV responses.  If it's not there,
     * scan to the end of the list to find the last item in the list.
     */
    for (p = context->sd_pairs; p; p = p->next) {
	/*
	 * If this is a retransmission, we better have the same
	 * sequence number AND delta.
	 */
	if (p->seq == seq && p->delta == delta)
	    return(TRUE);
	last = p;
    }

    pair = ipnat_create_seq_delta_pair(context);
    if (pair == NULL) {
	/*
	 * If couldn't allocate, re-use the first pair.  Fold its
	 * delta into the first pair for the same domain.  If one
	 * isn't found, we're out of luck.  This is extremely
	 * unlikely however.
	 */
	if (context->sd_pairs) {
	    pair = context->sd_pairs;
	    for (p = pair->next; p; p = p->next) {
		if (p->domain == pair->domain) {
		    /*
		     * Found a pair to fold the first delta into.
		     */
		    p->delta += pair->delta;
		    /*
		     * Remove the first one now that the delta is moved.
		     */
		    context->sd_pairs = pair->next;
		    break;
		}
	    }

	    /*
	     * If no other pair for same domain, have to punt, sorry.
	     */
	    if (p == NULL)
		return(FALSE);
	} else {
	    return(FALSE);
	}
    }

    pair->seq = seq;
    pair->delta = delta;
    pair->domain = domain;
    pair->next = NULL;

    /*
     * A linear list seems like a bad idea here, but I am banking
     * on the list not needing to keep many sequence number corrections.
     * The more PORT and PASV commands, the more corrections that
     * are necessary.
     */
    if (last)
	last->next = pair;
    else
	context->sd_pairs = pair;

    return(TRUE);
}

/*
 * ipnat_find_delta
 *
 * Find the sequence number/delta pair for the given sequence number.
 */

static ipnat_seq_delta_pair *
ipnat_find_delta (ipnat_edit_context *context, ulong seq)
{
    ipnat_seq_delta_pair *p;

    for (p = context->sd_pairs; p; p = p->next) {
	if (seq == p->seq)
	    break;
    }
    return(p);
}

/*
 * ipnat_apply_seqdelta
 *
 * Apply a sequence number delta to a TCP packet.
 */

static void
ipnat_apply_seqdelta (tcptype *tcp, ipnat_edit_context *context,
		      enum ipnat_lookup lookup, enum ipnat_domain domain)
{
    ulong oldseq, newseq;
    ipnat_seq_delta_pair *p, *next;
    int seqdelta, ackdelta;

    /*
     * Collect the deltas to the sequence space up to the sequence number
     * for this packet and apply to the sequence and acknowledgement numbers
     * as needed.  Prune away old entries as they are hit.
     */
    seqdelta = ackdelta = 0;
    for (p = context->sd_pairs; p; p = next) {
	/*
	 * Save next pointer since this item might be removed.
	 */
	next = p->next;

	/*
	 * Figure out which number to compare against to see
	 * if this entry should be pruned.
	 */
	if (p->domain == IPNAT_INSIDE)
	    if (domain == IPNAT_INSIDE)
		newseq = tcp->sequencenumber;
	    else
		newseq = tcp->acknowledgementnumber;
	else
	    if (domain == IPNAT_OUTSIDE)
		newseq = tcp->sequencenumber;
	    else
		newseq = tcp->acknowledgementnumber;

	/*
	 * Prune away the entry if the entry's sequence number is too
	 * old.  Too old is defined as being at least half the
	 * sequence space away from the current number.
	 */
	if (abs(newseq - p->seq) >= 0x80000000) {
	    ipnat_remove_seq_delta_pair(p, context);
	    ipnat_destroy_seq_delta_pair(p, context);
	    continue;	/* Move on to next entry. */
	}

	/*
	 * Add this delta into the total sequence or acknowledgement
	 * delta as appropriate.
	 */
	if (p->domain == IPNAT_INSIDE) {
	    /*
	     * Delta was produced as packet moved from inside to outside.
	     */
	    if (domain == IPNAT_INSIDE) {
		/*
		 * Normal direction, add delta to sequence number.
		 */
		if (GEQ(tcp->sequencenumber, p->seq))
		    seqdelta += p->delta;
	    } else {
		/*
		 * Reverse direction, subtract delta from ack number.
		 */
		if (tcp->ack && GEQ(tcp->acknowledgementnumber,
				    p->seq + (p->delta - ackdelta)))
		    ackdelta -= p->delta;
	    }
	} else {
	    /*
	     * Delta was produced as packet moved from outside to inside.
	     */
	    if (domain == IPNAT_OUTSIDE) {
		/*
		 * Normal direction, add delta to sequence number.
		 */
		if (GEQ(tcp->sequencenumber, p->seq))
		    seqdelta += p->delta;
	    } else {
		/*
		 * Reverse direction, subtract delta from ack number.
		 */
		if (tcp->ack && GEQ(tcp->acknowledgementnumber,
				    p->seq + (p->delta - ackdelta)))
		    ackdelta -= p->delta;
	    }
	}
    }

    /*
     * Now that deltas have been accumulated, apply them to the packet.
     */
    if (seqdelta) {
	oldseq = tcp->sequencenumber;
	newseq = oldseq + seqdelta;
	tcp->sequencenumber = newseq;
	tcp->checksum = ip_checksum_adjust_long(tcp->checksum, oldseq, newseq);
    }
    if (ackdelta && tcp->ack) {
	oldseq = tcp->acknowledgementnumber;
	newseq = oldseq + ackdelta;
	tcp->acknowledgementnumber = newseq;
	tcp->checksum = ip_checksum_adjust_long(tcp->checksum, oldseq, newseq);
    }
}

/*
 * ipnat_tcp_ftp_door
 *
 * Create a door for the FTP data stream.
 */

static boolean
ipnat_tcp_ftp_door (ipnat_entry *nat, ushort insideport, ushort outsideport,
		    uchar flags)
{
    ipnat_door *door;

    door = ipnat_find_door(TCP_PROT, nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			   outsideport, insideport, IPNAT_LOCAL);
    if (door) {
	/*
	 * It's already there, so just refresh it's timer.
	 */
	mgd_timer_start(&door->timeout_timer, IPNAT_DOGGY_DOOR_TIMEOUT);
    } else {
	if (!ipnat_setup_door(nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			      nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			      outsideport,
			      insideport,
			      insideport,
			      nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			      TCP_PROT, TCP_PROT,
			      flags))
	    return(FALSE);
    }

    return(TRUE);
}

/*
 * ipnat_tcp_new_common
 *
 * Common code for writing a new FTP command into a packet
 * and fixing up context information for the control stream.
 */

static paktype *
ipnat_tcp_new_common (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		      ipnat_entry *nat, ipnat_edit_context *context,
		      enum ipnat_lookup lookup, char *fmt)
{
    int newseglen;
    uchar *data;
    int delta;
    ushort port;
    enum ipnat_domain domain;
    enum ipnat_lookup opposite_lookup;
    ipnat_entry *nat2, *template;
    ushort globalport, localport;

    if (lookup == IPNAT_LOCAL)
	domain = IPNAT_INSIDE;
    else
	domain = IPNAT_OUTSIDE;
    opposite_lookup = ipnat_opposite_lookup(lookup);
    
    /*
     * This makes the exciting assumption that even if our new PORT
     * string is longer, we actually have enough space in the packet
     * buffer to take the slop.
     * The other exciting assumption is that the original
     * PORT command started at the beginning of the packet.
     */

    data = (uchar *) tcp + (tcp->dataoffset << 2);

    port = context->port_cmd_port;

    /*
     * May need to create a translation or door for the data connection.
     */
    if (lookup == IPNAT_LOCAL && nat->mapping[IPNAT_INSIDE] &&
	((nat->mapping[IPNAT_INSIDE]->flags & IPNMF_OVERLOAD) ||
	 nat->mapping[IPNAT_INSIDE]->match_type == IPNAT_MATCH_ROUTE_MAP) &&
	context->source_type == NAT_CLIENT) {
	/*
	 * Client inside sent a PORT command and overloading
	 * is configured, so need a translation/door for the
	 * outside-initiated connection so it can get in.
	 */
	if (!ipnat_tcp_ftp_door(nat, port, FTPD_PORT,
				IPNDF_OUTSIDEHOST_ANY|IPNDF_KNOWN_PORT))
	    return(NULL);
    } else if (lookup == IPNAT_GLOBAL && (nat->flags & IPNF_DEST) &&
	       context->source_type == NAT_SERVER) {
	/*
	 * Client outside is sending a PORT command.  We need to
	 * set up a translation for the flow back so that the server
	 * appears to the client as the same address.
	 */
	globalport = localport = port;

	/*
	 * Check if it's already there first...
	 */
	nat2 = ipnat_find_entry(ip->srcadr,
				nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				globalport, FTPD_PORT, TCP_PROT, IPNAT_GLOBAL);
	if (nat2 == NULL) {
	    template = ipnat_find_entry(IPADDR_ZERO, ip->srcadr, 0, 0, 0,
					IPNAT_GLOBAL);
	    nat2 = ipnat_clone_entry(nat, template, nat->mapping[IPNAT_INSIDE],
				     FTPD_PORT, TCP_PROT, ip->srcadr,
				     globalport);
	    if (nat2 == NULL) {
		if (ipnat_debug)
		    buginf("\nNAT: couldn't create translation for FTP");
		return(NULL);
	    }
	    /* Could have gotten a new port, so make sure to use it. */
	    port = nat2->port[IPNAT_OUTSIDE][IPNAT_LOCAL];
	}
    } else if (lookup == IPNAT_LOCAL && (nat->flags & IPNF_DEST) &&
	       context->source_type == NAT_SERVER) {
	/*
	 * Server inside is sending a passive response.  We need to
	 * set up a translation for the flow coming back to the inside
	 * so that it doesn't do rotary on us.  We don't know what local
	 * port the client outside will use, though, so set up a door.
	 */
	if (!ipnat_tcp_ftp_door(nat, port, 0, IPNDF_OUTSIDEHOST_ANY))
	    return(NULL);
    }

    newseglen = sprintf(data, fmt,
			(nat->address[domain][opposite_lookup] >> 24) & 0xff,
			(nat->address[domain][opposite_lookup] >> 16) & 0xff,
			(nat->address[domain][opposite_lookup] >> 8) & 0xff,
			(nat->address[domain][opposite_lookup] >> 0) & 0xff,
			(port >> 8) & 0xff, (port >> 0) & 0xff);

    /*
     * The sequence space may change after this point,
     * so record this.
     */
    delta = newseglen - seglen;
    if (delta) {
	ip->tl += delta;
	if (!ipnat_add_new_delta(context, tcp->sequencenumber + seglen, delta,
				 domain))
	    return(NULL);
    }

    /*
     * Recalculate the checksum.  Note that the IP header
     * still holds the old address information.
     * This checksum will be incrementally updated later
     * on when the IP header is modified.
     */
    tcp->checksum = 0;
    tcp->checksum = CheckSum((uchar *) tcp, ip->tl - (ip->ihl << 2),
			     ip->srcadr, ip->dstadr, TCP_PROT);

    pak->datagramsize += delta;	/* This might not be necessary */

    return(pak);
}

/*
 * ipnat_tcp_new_portcmd
 *
 * Returns NULL or a new packet.  NULL means "use the old packet".
 * XXX Failure mode is unpleasant
 */

static paktype *
ipnat_tcp_new_portcmd (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		       ipnat_entry *nat, ipnat_edit_context *context,
		       enum ipnat_lookup lookup)
{
    return(ipnat_tcp_new_common(pak, ip, tcp, seglen, nat, context, lookup,
				"PORT %d,%d,%d,%d,%d,%d\r\n"));
}

static paktype *
ipnat_tcp_new_pasvrep (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		       ipnat_entry *nat, ipnat_edit_context *context,
		       enum ipnat_lookup lookup)
{
    return(ipnat_tcp_new_common(pak, ip, tcp, seglen, nat, context, lookup,
			"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n"));
}

/*
 * watch_ftp_port_cmd
 *
 * Client is inside talking to a server outside, watch for it sending
 * a "PORT" command.
 */

static paktype *
watch_ftp_port_cmd (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		    ipnat_entry *nat, ipnat_edit_context *context,
		    enum ipnat_lookup lookup, boolean *found)
{
    int i, parts;
    enum ipnat_token token;
    paktype *newpak;
    enum ipnat_domain domain;

    parts = 0;
    *found = FALSE;

    if (lookup == IPNAT_LOCAL)
	domain = IPNAT_INSIDE;
    else
	domain = IPNAT_OUTSIDE;

    /*
     * Packet is going outside.
     * Watch for client sending "PORT" command with local address.
     */
    i = 0;
    while ((token = get_token(tcp, seglen, context, &i)) != TOK_INCOMPLETE) {
	switch (context->state) {
	case IPNAT_FTP_OPEN:
	    if (token == TOK_PORT_KEYWORD) {
		/*
		 * Got the "PORT" keyword, get IP address next.
		 */
		*found = TRUE;
		context->state = IPNAT_FTP_PORT_ADDR;
		context->port_cmd_addr = IPADDR_ZERO;
		context->port_cmd_port = 0;
		parts = 4;
	    }
	    break;

	case IPNAT_FTP_PORT_ADDR:
	    /*
	     * Parse 4 comma separated IP address octets.
	     * This is not terrifically robust.
	     */
	    switch (token) {
	    case TOK_NUMBER:
		if (parts) {
		    context->port_cmd_addr = (context->port_cmd_addr << 8) +
			context->value;
		    parts--;
		}
		break;

	    case TOK_PUNCT:
		if (context->token_string[0] != ',') {
		    buginf("\nwanted ',' got '%c'", token,
			   context->token_string[0]);
		    /* Back to normal state. */
		    context->state = IPNAT_FTP_OPEN;
		    return(pak);
		}

		if (parts == 0) {
		    /*
		     * Got the IP address, get port next.
		     */
		    context->state = IPNAT_FTP_PORT_PORT;
		    parts = 2;
		}
		break;

	    default:
		buginf("\nbad token %d, wanted TOK_NUMBER|TOK_PUNCT", token);
		/* Back to normal state. */
		context->state = IPNAT_FTP_OPEN;
		return(pak);
		break;
	    }
	    break;

	case IPNAT_FTP_PORT_PORT:
	    /*
	     * Parse 2 comma separated port octets.
	     * This is not terrifically robust.
	     */
	    switch (token) {
	    case TOK_NUMBER:
		if (parts) {
		    context->port_cmd_port = (context->port_cmd_port << 8) +
			context->value;
		    parts--;
		}
		break;

	    case TOK_PUNCT:
		if (context->token_string[0] != ',') {
		    buginf("\nwanted ',' got '%c'", token,
			   context->token_string[0]);
		    /* Back to normal state. */
		    context->state = IPNAT_FTP_OPEN;
		    return(pak);
		}
		break;

	    case TOK_EOL:
		if (parts != 0) {
		    buginf("\npremature end-of-line");
		    /* Back to normal state. */
		    context->state = IPNAT_FTP_OPEN;
		    return(pak);
		}

		/*
		 * Got the port number, make new packet
		 * with the translated address.
		 */

		/*
		 * Back to normal state now.
		 */
		context->state = IPNAT_FTP_OPEN;
		if (context->port_cmd_addr == nat->address[domain][lookup]) {
		    newpak = ipnat_tcp_new_portcmd(pak, ip, tcp, seglen,
						   nat, context, lookup);
		    if (newpak == NULL)
			return(NULL);
		    if (newpak != pak) {
			retbuffer(pak);
			pak = newpak;
		    }
		    return(pak);
		}
		break;

	    default:
		buginf("\nbad token %d, wanted TOK_NUMBER|TOK_PUNCT", token);
		/* Back to normal state. */
		context->state = IPNAT_FTP_OPEN;
		return(pak);
		break;
	    }
	    break;

	default:
	    return(pak);
	    break;
	}
    }
    return(pak);
}

/*
 * watch_ftp_pasv_cmd
 *
 * Server is inside talking to a client outside, watch for the client
 * sending a "PASV" command so we know to start watching for the server's
 * response.
 */

static paktype *
watch_ftp_pasv_cmd (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		    ipnat_entry *nat, ipnat_edit_context *context)
{
    int i;
    enum ipnat_token token;

    /*
     * Don't look if we don't need to.
     */
    if (context->state != IPNAT_FTP_OPEN) {
	return(pak);
    }

    /*
     * Packet is coming inside.
     * Watch for client sending "PASV" command.
     */
    i = 0;
    while ((token = get_token(tcp, seglen, context, &i)) != TOK_INCOMPLETE) {
	if (token == TOK_PASV_KEYWORD) {
	    context->state = IPNAT_FTP_PASV_ADDR;
	}
    }
    return(pak);
}

/*
 * watch_ftp_pasv_rsp
 *
 * Server is inside talking to a client outside, watch for it sending
 * the response to the client's "PASV" command.
 */

static paktype *
watch_ftp_pasv_rsp (paktype *pak, iphdrtype *ip, tcptype *tcp, int seglen,
		    ipnat_entry *nat, ipnat_edit_context *context,
		    enum ipnat_lookup lookup)
{
    int i, parts;
    enum ipnat_token token;
    paktype *newpak;
    int response;
    enum ipnat_domain domain;

    if (lookup == IPNAT_LOCAL)
	domain = IPNAT_INSIDE;
    else
	domain = IPNAT_OUTSIDE;

    /*
     * Don't want to be here unless we have seen a "PASV" command
     * coming from the client outside to the server inside.
     */
    if (context->state != IPNAT_FTP_PASV_ADDR)
	return(pak);

    /*
     * Packet is going outside.
     * Watch for server replying to "PASV" with local address.
     *
     * Response looks like this:
     * 227 Entering Passive Mode (iii,iii,iii,iii,ppp,ppp)\r\n
     * cf. RFC 959 and RFC 1123
     */
    i = 0;
    response = 0;
    parts = 4;
    while ((token = get_token(tcp, seglen, context, &i)) != TOK_INCOMPLETE) {
	switch (context->state) {
	case IPNAT_FTP_PASV_ADDR:
	    /*
	     * Parse 4 comma separated IP address octets.
	     * This is not terrifically robust.
	     */
	    switch (token) {
	    case TOK_NUMBER:
		if (response) {
		    if (parts) {
			context->port_cmd_addr = (context->port_cmd_addr << 8) +
			    context->value;
			parts--;
		    }
		} else {
		    /* Get the "227" from "227 Entering Passive Mode..." */
		    response = context->value;
		}
		break;

	    case TOK_PUNCT:
		if (parts == 0) {
		    /*
		     * Got the IP address, get port next.
		     */
		    context->state = IPNAT_FTP_PASV_PORT;
		    parts = 2;
		}
		break;

	    default:
		/*
		 * We have to accept some junk until we find the
		 * IP address.
		 */
		break;
	    }
	    break;

	case IPNAT_FTP_PASV_PORT:
	    /*
	     * Parse 2 comma separated port octets.
	     * This is not terrifically robust.
	     */
	    switch (token) {
	    case TOK_NUMBER:
		if (parts) {
		    context->port_cmd_port = (context->port_cmd_port << 8) +
			context->value;
		    parts--;
		}
		break;

	    case TOK_PUNCT:
		/* XXX don't really sanity-check the punctuation, oh well */
		break;

	    case TOK_EOL:
		if (parts == 0) {
		    /*
		     * Got the port number, make new packet
		     * with the translated address.
		     */
		    /*
		     * Back to normal state now.
		     */
		    context->state = IPNAT_FTP_OPEN;
		    if (context->port_cmd_addr ==
			nat->address[domain][lookup]) {
			newpak = ipnat_tcp_new_pasvrep(pak, ip, tcp, seglen,
						       nat, context, lookup);
			if (newpak == NULL)
			    return(NULL);
			if (newpak != pak) {
			    retbuffer(pak);
			    pak = newpak;
			}
			return(pak);
		    }
		}
		break;

	    default:
		buginf("\nbad token %d, wanted TOK_NUMBER|TOK_PUNCT", token);
		/* Back to normal state. */
		context->state = IPNAT_FTP_OPEN;
		return(pak);
		break;
	    }
	    break;

	default:
	    return(pak);
	    break;
	}
    }
    return(pak);
}

/*
 * ipnat_l4_tcp_ftp
 *
 * Do additional massaging on FTP control connection packets.
 * May return a new packet and dispose of the original.  A NULL
 * return indicates that something didn't work and the packet
 * should be dropped.
 */

static paktype *
ipnat_l4_tcp_ftp (paktype *pak, iphdrtype *ip, tcptype *tcp,
		  ipnat_entry *nat, enum ipnat_lookup lookup)
{
    ipnat_edit_context *context;
    enum local_type source_type;
    int seglen;
    enum ipnat_domain domain;
    boolean found;

    if (lookup == IPNAT_LOCAL)
	domain = IPNAT_INSIDE;
    else
	domain = IPNAT_OUTSIDE;

    /*
     * Find the edit_context going on here.
     */
    context = nat->edit_context;
    if (context == NULL) {
	/*
	 * If this isn't an extended entry, need to create one
	 * to hang the editing context on.
	 */
	if (!(nat->flags & IPNF_EXTENDED)) {
	    nat = new_extended_entry(pak, ip, tcp, TCP_PROT, nat, lookup);
	    if (nat == NULL) {
		if (ipnat_debug)
		    buginf("\nNAT: couldn't create translation for FTP");
		return(NULL);
	    }
	}
	if (lookup == IPNAT_LOCAL)
	    source_type = (tcp->destinationport == FTP_PORT) ?
		NAT_CLIENT : NAT_SERVER;
	else
	    source_type = (tcp->sourceport == FTP_PORT) ?
		NAT_CLIENT : NAT_SERVER;
	context = ipnat_create_edit_context(ip, nat, source_type, lookup);
	if (context == NULL)
	    return(NULL);
	context->state = IPNAT_FTP_OPEN;
	nat->edit_context = context;
    }

    /*
     * Parse the command stream if there's anything there to parse.
     */
    seglen = ip->tl - ((ip->ihl + tcp->dataoffset) << 2);
    if (seglen > 0) {
	/*
	 * Parse for more editing opportunities.
	 */
	if (context->source_type == NAT_CLIENT) {
	    /*
	     * Client is on inside.
	     */
	    if (lookup == IPNAT_LOCAL) {
		pak = watch_ftp_port_cmd(pak, ip, tcp, seglen, nat, context,
					 lookup, &found);
		if (! found)
		    pak = watch_ftp_pasv_cmd(pak, ip, tcp, seglen, nat,
					     context);
	    } else {
		pak = watch_ftp_pasv_rsp(pak, ip, tcp, seglen, nat, context,
					 lookup);
	    }
	} else {
	    /*
	     * Client is on outside.
	     */
	    if (lookup == IPNAT_LOCAL) {
		pak = watch_ftp_pasv_rsp(pak, ip, tcp, seglen, nat, context,
					 lookup);
	    } else {
		pak = watch_ftp_port_cmd(pak, ip, tcp, seglen, nat, context,
					 lookup, &found);
		if (! found)
		    pak = watch_ftp_pasv_cmd(pak, ip, tcp, seglen, nat,
					     context);
	    }
	}

	/*
	 * Reset the header pointers if we still have a packet.
	 */
	if (pak) {
	    ip = (iphdrtype *) ipheadstart(pak);
	    tcp = (tcptype *) ipdatastart_iph(ip);
	}
    }

    /*
     * If there is a sequence-number delta to apply due to earlier
     * editing, apply it now.
     */
    if (pak && context->sd_pairs)
	ipnat_apply_seqdelta(tcp, context, lookup, domain);

    /*
     * Possibly return a new packet.
     */
    return(pak);
}

/*
 * ipnat_l4_tcp_rshell
 *
 * Handle rsh chores.  We will get here for the SYN packet and
 * create an edit context to hold the state.  The presence of
 * the editing context will force the packets out of the fast
 * switch path up to process level so this routine can be run
 * on the packet.
 */

static boolean
ipnat_l4_tcp_rshell (paktype *pak, iphdrtype *ip, tcptype *tcp,
		     ipnat_entry *nat, enum ipnat_lookup lookup)
{
    ipnat_edit_context *context;
    enum local_type source_type;
    int seglen, newseglen, delta, i;
    uint insideport[IPNAT_LOOKUPS];
    uchar *data;
    ipnat_mapping *mapping;
    ipnat_door *door;
    enum ipnat_domain domain;

    if (lookup == IPNAT_LOCAL)
	domain = IPNAT_INSIDE;
    else
	domain = IPNAT_OUTSIDE;

    /*
     * Find the edit_context going on here.
     * The context must be created before checking for the SYN
     * and bailing out of this routine.  The presence of the context
     * on the translation will keep the packets coming to process
     * level.
     */
    context = nat->edit_context;
    if (context == NULL) {
	/*
	 * If this isn't an extended entry, need to create one
	 * to hang the editing context on.
	 */
	if (!(nat->flags & IPNF_EXTENDED)) {
	    nat = new_extended_entry(pak, ip, tcp, TCP_PROT, nat, lookup);
	    if (nat == NULL) {
		if (ipnat_debug)
		    buginf("\nNAT: couldn't create translation for RSH");
		return(FALSE);
	    }
	}
	if (lookup == IPNAT_LOCAL)
	    source_type = (tcp->destinationport == RSHELL_PORT) ?
		NAT_CLIENT : NAT_SERVER;
	else
	    source_type = (tcp->sourceport == RSHELL_PORT) ?
		NAT_CLIENT : NAT_SERVER;
	context = ipnat_create_edit_context(ip, nat, source_type, lookup);
	if (context == NULL)
	    return(FALSE);
	context->state = IPNAT_RSH_OPEN;
	nat->edit_context = context;
    } else if (tcp->syn && !tcp->ack) {
	/*
	 * The context already exists for a previous incarnation
	 * of this connection, so reset the state.
	 */
	context->state = IPNAT_RSH_OPEN;
	ipnat_destroy_all_seq_delta_pairs(context);
    }

    /*
     * If there is a sequence-number delta to apply due to earlier
     * editing, apply it now.
     */
    if (context->sd_pairs)
	ipnat_apply_seqdelta(tcp, context, lookup, domain);

    if (tcp->syn || tcp->fin || tcp->rst)
	return(TRUE);

    if (!(lookup == IPNAT_LOCAL && nat->mapping[IPNAT_INSIDE] &&
	  (nat->mapping[IPNAT_INSIDE]->flags & IPNMF_OVERLOAD) &&
	  context->source_type == NAT_CLIENT))
	return(TRUE);

    /*
     * Nothing else to do if the packet has no data.
     */
    seglen = ip->tl - ((ip->ihl + tcp->dataoffset) << 2);
    if (seglen == 0)
	return(TRUE);

    /*
     * Only bail here if we think we're done and this isn't
     * a retransmission of the port string.
     */
    if (context->state == IPNAT_RSH_DONE &&
	ipnat_find_delta(context, tcp->sequencenumber) == NULL) {
	/*
	 * If there is no sequence number fixup to do, then
	 * mark the translation to indicate that fast switching
	 * can have it.
	 */
	if (context->sd_pairs == NULL)
	    nat->more_flags |= IPNF2_PROCESS_DONE;
	return(TRUE);
    }

    /*
     * Get the port number from the stream.
     */
    insideport[IPNAT_LOCAL] = 0;
    data = (uchar *) tcp + (tcp->dataoffset << 2);
    for (i = 0; i < seglen && data[i]; i++)
	insideport[IPNAT_LOCAL] = insideport[IPNAT_LOCAL] * 10 +
	    (data[i] - '0');

    /*
     * No port number means no stream coming back from the server.
     * All done.
     */
    if (insideport[IPNAT_LOCAL] == 0) {
	context->state = IPNAT_RSH_DONE;
	return(TRUE);
    }

    if (ipnat_detailed_debug)
	buginf("\nNAT: rsh sends port %d", insideport[IPNAT_LOCAL]);

    mapping = nat->mapping[IPNAT_INSIDE];

    /*
     * Create a door so that a translation can be created when the
     * reply from the server comes back from the random port number.
     */
    door = ipnat_find_door(TCP_PROT, nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			   0, insideport[IPNAT_LOCAL], IPNAT_LOCAL);
    if (door) {
	/*
	 * It's already there, so just refresh it's timer.
	 */
	mgd_timer_start(&door->timeout_timer, IPNAT_DOGGY_DOOR_TIMEOUT);
	context->state = IPNAT_RSH_DONE;
	return(TRUE);
    }

    /*
     * This port number might be in use by somebody, so get a new
     * one or allocate this one.
     */
    if (ipnat_detailed_debug)
	buginf("\nNAT: rsh: allocating new port for %d",
	       insideport[IPNAT_LOCAL]);
    insideport[IPNAT_GLOBAL] = ipnat_allocate_port(mapping, TCP_PROT,
				    nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				    nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				    insideport[IPNAT_LOCAL], IPNAT_GLOBAL);
    if (ipnat_detailed_debug)
	buginf("\nNAT: rsh: got port %d", insideport[IPNAT_GLOBAL]);

    if (insideport[IPNAT_GLOBAL] == IPNAT_NO_PORT) {
	/* Something is seriously wrong */
	if (ipnat_debug)
	    buginf("\nNAT: no global port for %i",
		   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	context->state = IPNAT_RSH_DONE;
	return(FALSE);
    }

    /*
     * If the ports are different, need to make sure to give
     * the global port number to the server.
     */
    if (insideport[IPNAT_GLOBAL] != insideport[IPNAT_LOCAL]) {
	/*
	 * Now rewrite the packet with the new port number.
	 */
	newseglen = sprintf(data, "%d", insideport[IPNAT_GLOBAL]);
	data[newseglen++] = '\0';

	/*
	 * The sequence space may change after this point,
	 * so record this.
	 */
	delta = newseglen - seglen;
	if (delta) {
	    ip->tl += delta;
	    if (!ipnat_add_new_delta(context, tcp->sequencenumber + seglen,
				     delta, domain))
		return(FALSE);
	}

	/*
	 * Recalculate the checksum.  Note that the IP header
	 * still holds the old address information.
	 * This checksum will be incrementally updated later
	 * on when the IP header is modified.
	 */
	tcp->checksum = 0;
	tcp->checksum = CheckSum((uchar *) tcp, ip->tl - (ip->ihl << 2),
				 ip->srcadr, ip->dstadr, TCP_PROT);
    }

    if (!ipnat_setup_door(nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			  nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			  RSHELL_PORT,
			  0,
			  insideport[IPNAT_LOCAL],
			  insideport[IPNAT_GLOBAL],
			  nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			  TCP_PROT, TCP_PROT,
			  IPNDF_OUTSIDEHOST_ANY | IPNDF_PORT_ALLOCATED))
	return(FALSE);

    context->state = IPNAT_RSH_DONE;
    return(TRUE);
}

/*
 * ipnat_l4_tcp_realaudio
 *
 * RealAudio opens a TCP control connection out to port 7070, and
 * tells the other side which inside UDP port to send the audio/video
 * stream in to (supposed to be in the range 6970-7170, but we don't care).
 */

struct realaudio_hdr {
    uchar junk[9];
    ushort PACKED(lport);
};

enum ipnat_realaudio_state {
    IPNAT_REALAUDIO_OPEN,
    IPNAT_REALAUDIO_DONE
};

static boolean
ipnat_l4_tcp_realaudio (paktype *pak, iphdrtype *ip, tcptype *tcp,
			ipnat_entry *nat, enum ipnat_lookup lookup)
{
    ipnat_edit_context *context;
    int seglen;
    uint insideport[IPNAT_LOOKUPS];
    struct realaudio_hdr *realaudio_hdr;
    ipnat_mapping *mapping;
    ipnat_door *door;

    if (!(nat->flags & IPNF_EXTENDED)) {
	/*
	 * Don't need to do anything if not overloading.
	 */
	return(TRUE);
    }

    /*
     * Find the edit_context going on here.
     * The context must be created before checking for the SYN
     * and bailing out of this routine.  The presence of the context
     * on the translation will keep the packets coming to process
     * level.
     */
    context = nat->edit_context;
    if (context == NULL) {
	context = ipnat_create_edit_context(ip, nat, NAT_CLIENT, lookup);
	if (context == NULL)
	    return(FALSE);
	context->state = IPNAT_REALAUDIO_OPEN;
	nat->edit_context = context;
    }

    if (tcp->syn || tcp->fin || tcp->rst)
	return(TRUE);

    /*
     * Nothing else to do if the packet has no data.
     */
    seglen = ip->tl - ((ip->ihl + tcp->dataoffset) << 2);
    if (seglen == 0)
	return(TRUE);

    /*
     * Note that that this should deal with retransmission.
     * The danger is that the segment carrying the port number
     * is retransmitted and we need to fix the port number
     * because that port number is already in use.  In that
     * case, we will only fix the first transmission attempt
     * but bail out here for all future attempts.
     */
    if (context->state == IPNAT_REALAUDIO_DONE) {
	nat->more_flags |= IPNF2_PROCESS_DONE;
	return(TRUE);
    }

    /*
     * Port is not short-aligned, so be careful.
     */
    realaudio_hdr = (struct realaudio_hdr *) ((uchar *) tcp +
					      (tcp->dataoffset << 2));
    insideport[IPNAT_LOCAL] = GETSHORT(&realaudio_hdr->lport);

    if (ipnat_detailed_debug)
	buginf("\nNAT: realaudio sends port %d", insideport[IPNAT_LOCAL]);

    mapping = nat->mapping[IPNAT_INSIDE];

    /*
     * Create a door so that a translation can be created when the
     * reply from the server comes back from the random port number.
     */
    door = ipnat_find_door(UDP_PROT, nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			   0, insideport[IPNAT_LOCAL], IPNAT_LOCAL);
    if (door) {
	/*
	 * It's already there, so just refresh it's timer.
	 */
	mgd_timer_start(&door->timeout_timer, IPNAT_DOGGY_DOOR_TIMEOUT);
	return(TRUE);
    }

    /*
     * This port number might be in use by somebody, so get a new
     * one or allocate this one.
     */
    if (ipnat_detailed_debug)
	buginf("\nNAT: realaudio: allocating new port for %d",
	       insideport[IPNAT_LOCAL]);
    insideport[IPNAT_GLOBAL] = ipnat_allocate_port(mapping, UDP_PROT,
				    nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				    nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				    insideport[IPNAT_LOCAL], IPNAT_GLOBAL);
    if (ipnat_detailed_debug)
	buginf("\nNAT: realaudio: got port %d", insideport[IPNAT_GLOBAL]);

    if (insideport[IPNAT_GLOBAL] == IPNAT_NO_PORT) {
	/* Something is seriously wrong */
	if (ipnat_debug)
	    buginf("\nNAT: no global port for %i",
		   nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	context->state = IPNAT_REALAUDIO_DONE;
	return(FALSE);
    }

    /*
     * If the ports are different, need to make sure to give
     * the global port number to the server.
     */
    if (insideport[IPNAT_GLOBAL] != insideport[IPNAT_LOCAL]) {
	PUTSHORT(&realaudio_hdr->lport, insideport[IPNAT_GLOBAL]);
	tcp->checksum = ip_checksum_adjust_short(tcp->checksum,
						 insideport[IPNAT_LOCAL],
						 insideport[IPNAT_GLOBAL]);
    }

    if (!ipnat_setup_door(nat->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			  nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			  REALAUDIO_PORT,
			  0,
			  insideport[IPNAT_LOCAL],
			  insideport[IPNAT_GLOBAL],
			  nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			  UDP_PROT, TCP_PROT,
			  IPNDF_OUTSIDEHOST_ANY))
	return(FALSE);

    context->state = IPNAT_REALAUDIO_DONE;
    return(TRUE);
}

/*
 * ipnat_icmp_udp_fixup
 *
 * Fix the UDP header inside an ICMP packet.
 */

static void
ipnat_icmp_udp_fixup (ipnat_entry *nat, icmptype *icmp, iphdrtype *ip_embed,
		      enum ipnat_lookup lookup)
{
    udptype *udp_embed;
    ushort osport, nsport, odport, ndport;
    boolean extended;

    udp_embed = (udptype *) ipdatastart_iph(ip_embed);
    osport = udp_embed->sport;
    odport = udp_embed->dport;
    extended = nat->flags & IPNF_EXTENDED;
    if (lookup == IPNAT_LOCAL) {
	nsport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] : osport;
	ndport = extended ? nat->port[IPNAT_INSIDE][IPNAT_GLOBAL] : odport;
    } else {
	nsport = extended ? nat->port[IPNAT_INSIDE][IPNAT_LOCAL] : osport;
	ndport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL] : odport;
    }
    if (osport != nsport) {
	udp_embed->sport = nsport;
	if (udp_embed->checksum)
	    udp_embed->checksum =
		ip_checksum_adjust_short(udp_embed->checksum, osport, nsport);
	else
	    icmp->checksum = ip_checksum_adjust_short(icmp->checksum,
						      osport, nsport);
    }
    if (odport != ndport) {
	udp_embed->dport = ndport;
	if (udp_embed->checksum)
	    udp_embed->checksum =
		ip_checksum_adjust_short(udp_embed->checksum, odport, ndport);
	else
	    icmp->checksum = ip_checksum_adjust_short(icmp->checksum,
						      odport, ndport);
    }
}

/*
 * ipnat_icmp_tcp_fixup
 *
 * Fix the TCP header inside an ICMP packet.
 */

static void
ipnat_icmp_tcp_fixup (ipnat_entry *nat, icmptype *icmp, iphdrtype *ip_embed,
		      enum ipnat_lookup lookup)
{
    tcptype *tcp_embed;
    ushort osport, nsport, odport, ndport;
    boolean extended;

    tcp_embed = (tcptype *) ipdatastart_iph(ip_embed);
    osport = tcp_embed->sourceport;
    odport = tcp_embed->destinationport;
    extended = nat->flags & IPNF_EXTENDED;
    if (lookup == IPNAT_LOCAL) {
	nsport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL] : osport;
	ndport = extended ? nat->port[IPNAT_INSIDE][IPNAT_GLOBAL] : odport;
    } else {
	nsport = extended ? nat->port[IPNAT_INSIDE][IPNAT_LOCAL] : osport;
	ndport = extended && (nat->flags & IPNF_OUTSIDE) ?
	    nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL] : odport;
    }
    if (osport != nsport) {
	tcp_embed->sourceport = nsport;
	/* assume TCP checksum isn't there */
	icmp->checksum = ip_checksum_adjust_short(icmp->checksum,
						  osport, nsport);
    }
    if (odport != ndport) {
	tcp_embed->destinationport = ndport;
	/* assume TCP checksum isn't there */
	icmp->checksum = ip_checksum_adjust_short(icmp->checksum,
						  odport, ndport);
    }
}

/*
 * ipnat_icmp_fixup
 *
 * Fix up the ICMP packet to cope with the address change.
 * Yes, this is a rather large inline function, but it is only
 * referenced once.
 */

static inline void
ipnat_icmp_fixup (ipnat_entry *nat, iphdrtype *ip, enum ipnat_lookup lookup,
		  enum ipnat_lookup opposite_lookup)
{
    icmptype *icmp;
    iphdrtype *ip_embed;
    ipnat_entry *template;
    ushort oid, nid;
    ipaddrtype osrcadr, nsrcadr, odstadr, ndstadr;
    ipaddrtype insideaddr, outsideaddr;
    ushort insideport, outsideport;
    boolean can_overload;
    ipaddrtype srcadr;
    uchar proto;

    icmp = (icmptype *) ipdatastart_iph(ip);

    /*
     * Handle pings.
     */
    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY) {
	if (nat->flags & IPNF_EXTENDED) {
	    oid = icmp->identifier;
	    nid = nat->port[IPNAT_INSIDE][opposite_lookup];
	    if (oid != nid) {
		icmp->identifier = nid;
		if (ipnat_debug && (!ipdebug_acl ||
				    ipnat_accesscheck(nat, ipdebug_acl)))
		    buginf("\nNAT: ICMP id=%d->%d", oid, nid);
		icmp->checksum = ip_checksum_adjust_short(icmp->checksum,
							  oid, nid);
	    }
	}
	return;
    }

    /*
     * If it doesn't have embedded headers, no need to continue.
     */
    if (!ipnat_special_icmptype(icmp->type))
	return;

    /*
     * Fix the embedded headers.
     */
    ip_embed = (iphdrtype *) icmp->data;

    if (lookup == IPNAT_LOCAL) {
	insideaddr = ip_embed->dstadr;
	outsideaddr = ip_embed->srcadr;
	ipnat_get_ports(ip_embed, &outsideport, &insideport, &proto, &srcadr,
			&can_overload);
    } else {
	insideaddr = ip_embed->srcadr;
	outsideaddr = ip_embed->dstadr;
	ipnat_get_ports(ip_embed, &insideport, &outsideport, &proto, &srcadr,
			&can_overload);
    }

    nat = ipnat_find_entry(insideaddr, outsideaddr, insideport, outsideport,
			   ip_embed->prot, lookup);
    if (nat) {
	/* Ready to translate. */
    } else {
	/*
	 * See if the source address requires translation into local space.
	 */
	template = ipnat_find_entry(IPADDR_ZERO, outsideaddr, 0, 0, 0, lookup);
	if (template == NULL)
	    outsideaddr = IPADDR_ZERO;

	/*
	 * Now try looking for a more general entry.  Static entries
	 * will get matched here.
	 */
	nat = ipnat_find_entry(insideaddr, outsideaddr, 0, 0, 0, lookup);
	if (nat) {
	    /* Ready to translate. */
	} else {
	    /* We lose. */
	    return;
	}
    }

    /*
     * Fix the transport-layer header inside the ICMP packet.
     */
    switch (ip_embed->prot) {
    case UDP_PROT:
	ipnat_icmp_udp_fixup(nat, icmp, ip_embed, lookup);
	break;
    case TCP_PROT:
	ipnat_icmp_tcp_fixup(nat, icmp, ip_embed, lookup);
	break;
    }

    /*
     * Fix the IP header inside the ICMP packet.
     */
    osrcadr = ip_embed->srcadr;
    odstadr = ip_embed->dstadr;
    if (lookup == IPNAT_LOCAL) {
	nsrcadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][opposite_lookup] : osrcadr;
	ndstadr = nat->address[IPNAT_INSIDE][opposite_lookup];
    } else {
	nsrcadr = nat->address[IPNAT_INSIDE][opposite_lookup];
	ndstadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][opposite_lookup] : odstadr;
    }
    if (osrcadr != nsrcadr) {
	ip_embed->srcadr = nsrcadr;
	ip_embed->checksum = ip_checksum_adjust_long(ip_embed->checksum,
						     osrcadr, nsrcadr);
    }
    if (odstadr != ndstadr) {
	ip_embed->dstadr = ndstadr;
	ip_embed->checksum = ip_checksum_adjust_long(ip_embed->checksum,
						     odstadr, ndstadr);
    }
}

/*
 * ipnat_udp_fixup
 *
 * Fix up the UDP header to cope with the address change.
 * Returns FALSE if the fixup fails and the packet should just be dropped.
 */

static inline boolean
ipnat_udp_fixup (paktype *pak, iphdrtype *ip, ipnat_entry *nat,
		 enum ipnat_lookup lookup,
		 ipaddrtype osrcadr, ipaddrtype nsrcadr,
		 ipaddrtype odstadr, ipaddrtype ndstadr)
{
    paktype *newpak;
    udptype *udp;
    int oldlen;
    ushort osport, nsport, odport, ndport;
    boolean extended;

    udp = (udptype *) ipdatastart_iph(ip);

    osport = udp->sport;
    odport = udp->dport;
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

    oldlen = ip->tl;

    /*
     * Do any required layer 4 translation.
     */
    if (udp->sport == DNS_PORT || udp->dport == DNS_PORT ||
	udp->sport == NBNS_PORT || udp->dport == NBNS_PORT) {
	newpak = ipnat_l4_udp_dns(pak, ip, udp, nat, lookup);
	if (newpak == NULL)
	    return(FALSE);
	if (newpak != pak) {
	    ip = (iphdrtype *) ipheadstart(newpak);
	    udp = (udptype *) ipdatastart_iph(ip);
	}
	pak = newpak;
    } else if (udp->sport == TFTP_PORT || udp->dport == TFTP_PORT) {
	if (!ipnat_l4_udp_tftp(pak, ip, udp, nat, lookup))
	    return(FALSE);
    } else if (udp->sport == CUSEEME_PORT || udp->dport == CUSEEME_PORT) {
	ipnat_l4_udp_cuseeme(pak, ip, udp, nat, lookup);
    } else if (udp->sport == NBDS_PORT || udp->dport == NBDS_PORT) {
	ipnat_l4_udp_nbds(pak, ip, udp, nat, lookup);
    }

    /*
     * Now update the UDP header if needed.
     */
    if (osport != nsport) {
	udp->sport = nsport;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: UDP s=%d->%d, d=%d", osport, nsport, udp->dport);
	if (udp->checksum)
	    udp->checksum = ip_checksum_adjust_short(udp->checksum, osport,
						     nsport);
    }
    if (odport != ndport) {
	udp->dport = ndport;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: UDP s=%d, d=%d->%d", udp->sport, odport, ndport);
	if (udp->checksum)
	    udp->checksum = ip_checksum_adjust_short(udp->checksum, odport,
						     ndport);
    }

    /*
     * Update the UDP checksum if one is there.
     */
    if (udp->checksum) {
	if (osrcadr != nsrcadr)
	    udp->checksum = ip_checksum_adjust_long(udp->checksum,
						    osrcadr, nsrcadr);
	if (odstadr != ndstadr)
	    udp->checksum = ip_checksum_adjust_long(udp->checksum,
						    odstadr, ndstadr);
    }
    return(TRUE);
}

/*
 * ipnat_tcp_fixup
 *
 * Do any TCP stream editing and fix up the TCP header to cope with
 * the address change.  Returns NULL if the editing fails and the
 * packet should just be dropped.
 */

static inline paktype *
ipnat_tcp_fixup (paktype *pak, iphdrtype *ip, ipnat_entry *nat,
		 enum ipnat_lookup lookup,
		 ipaddrtype osrcadr, ipaddrtype nsrcadr,
		 ipaddrtype odstadr, ipaddrtype ndstadr)
{
    paktype *newpak;
    tcptype *tcp;
    int oldlen;
    ushort osport, nsport, odport, ndport;
    boolean extended;

    tcp = (tcptype *) ipdatastart_iph(ip);

    osport = tcp->sourceport;
    odport = tcp->destinationport;
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

    oldlen = ip->tl;

    /*
     * If this packet is part of an FTP control stream, do
     * any translation required there.
     */
    if (tcp->sourceport == FTP_PORT || tcp->destinationport == FTP_PORT) {
	newpak = ipnat_l4_tcp_ftp(pak, ip, tcp, nat, lookup);
	if (newpak == NULL) {
	    /* Something failed, must drop the packet. */
	    return(NULL);
	}
	if (newpak != pak) {
	    ip = (iphdrtype *) ipheadstart(newpak);
	    tcp = (tcptype *) ipdatastart_iph(ip);
	}
	pak = newpak;
    } else if (tcp->sourceport == RSHELL_PORT ||
	       tcp->destinationport == RSHELL_PORT) {
	if (!ipnat_l4_tcp_rshell(pak, ip, tcp, nat, lookup)) {
	    /* Something failed, must drop the packet. */
	    return(NULL);
	}
    } else if (tcp->sourceport == REALAUDIO_PORT ||
	       tcp->destinationport == REALAUDIO_PORT) {
	if (!ipnat_l4_tcp_realaudio(pak, ip, tcp, nat, lookup)) {
	    /* Something failed, must drop the packet. */
	    return(NULL);
	}
    }

    /*
     * Now update the TCP header if needed.
     */
    if (osport != nsport) {
	tcp->sourceport = nsport;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: TCP s=%d->%d, d=%d", osport, nsport,
		   tcp->destinationport);
	tcp->checksum = ip_checksum_adjust_short(tcp->checksum, osport,
						 nsport);
    }
    if (odport != ndport) {
	tcp->destinationport = ndport;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: TCP s=%d, d=%d->%d", tcp->sourceport,
		   odport, ndport);
	tcp->checksum = ip_checksum_adjust_short(tcp->checksum, odport,
						 ndport);
    }

    /*
     * Update the TCP checksum.
     */
    if (osrcadr != nsrcadr)
	tcp->checksum = ip_checksum_adjust_long(tcp->checksum, osrcadr,
						nsrcadr);
    if (odstadr != ndstadr)
	tcp->checksum = ip_checksum_adjust_long(tcp->checksum, odstadr,
						ndstadr);

    /*
     * If the packet length changed because of any editing,
     * fix up the checksum again.
     */
    if (ip->tl != oldlen)
	ip->checksum = ip_checksum_adjust_short(ip->checksum, oldlen, ip->tl);

    /*
     * Watch for FINs and RSTs so we can throw stuff away.
     */
    if (!(nat->flags & IPNF_STATIC) && extended) {
	/*
	 * Remember any FIN.
	 */
	if (tcp->fin) {
	    if (lookup == IPNAT_LOCAL)
		nat->more_flags |= IPNF2_FIN_OUT;
	    else
		nat->more_flags |= IPNF2_FIN_IN;
	}

	/*
	 * Reset the inactivity timer if needed.
	 */
	if (tcp->syn) {
	    /*
	     * Reset the inactivity timer to a shorter interval if we
	     * see a SYN to make sure that the translation goes away
	     * in case the connection never gets established.  the
	     * timer will be set to its normal (long) interval when a
	     * non-SYN packet comes through, indicating that the
	     * connection is established.
	     */
	    if (ipnat_timeouts[IPNAT_SYN_TIMEOUT].enabled)
		mgd_timer_start(&nat->inactivity_timer,
				ipnat_timeouts[IPNAT_SYN_TIMEOUT].timeout);
	} else if (tcp->rst || (tcp->fin &&
				IPNAT_BOTH_FINS(nat->more_flags))) {
	    /*
	     * Reset the inactivity timer to a shorter interval if
	     * we see a RST or two FINs to make sure the translation
	     * goes away promptly.
	     * XXX should really check that last FIN has been acked.
	     */
	    nat->flags |= IPNF_TIMING_OUT;
	    if (ipnat_timeouts[IPNAT_FINRST_TIMEOUT].enabled)
		mgd_timer_start(&nat->inactivity_timer,
				ipnat_timeouts[IPNAT_FINRST_TIMEOUT].timeout);
	}
    }

    return(pak);
}

/*
 * ipnat_l3_fixup
 *
 * Do any higher layer fixups (inside out), then fixup the
 * the IP header and checksum.
 */

paktype *
ipnat_l3_fixup (paktype *pak, iphdrtype *ip, ipnat_entry *nat,
		enum ipnat_lookup lookup)
{
    paktype *newpak;
    ipaddrtype osrcadr, nsrcadr, odstadr, ndstadr;
    enum ipnat_lookup opposite_lookup;
    int dglen;

    opposite_lookup = ipnat_opposite_lookup(lookup);

    /*
     * Save away old and new information.
     */
    osrcadr = ip->srcadr;
    odstadr = ip->dstadr;
    if (lookup == IPNAT_LOCAL) {
	nsrcadr = nat->address[IPNAT_INSIDE][opposite_lookup];
	ndstadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][opposite_lookup] : odstadr;
    } else {
	nsrcadr = (nat->flags & IPNF_OUTSIDE) ?
	    nat->address[IPNAT_OUTSIDE][opposite_lookup] : osrcadr;
	ndstadr = nat->address[IPNAT_INSIDE][opposite_lookup];
    }

    /*
     * Can't do layer 4 work unless this is fragment zero
     * and is large enough.
     */
    if (ip->fo == 0) {
	/*
	 * Get size of datagram to make sure we don't poke
	 * into headers that aren't actually there... This might
	 * be a fragment, or a truncated non-fragment.
	 */
	dglen = ip->tl - (ip->ihl << 2);
	switch (ip->prot) {
	case ICMP_PROT:
	    if (dglen < ICMPHEADERBYTES)
		break;
	    ipnat_icmp_fixup(nat, ip, lookup, opposite_lookup);
	    break;

	case UDP_PROT:
	    if (dglen < UDPHEADERBYTES)
		break;
	    if (!ipnat_udp_fixup(pak, ip, nat, lookup, osrcadr, nsrcadr,
				 odstadr, ndstadr))
		pak = NULL;
	    break;

	case TCP_PROT:
	    if (dglen < TCPHEADERBYTES)
		break;
	    newpak = ipnat_tcp_fixup(pak, ip, nat, lookup, osrcadr, nsrcadr,
				     odstadr, ndstadr);
	    if (newpak && newpak != pak)
		ip = (iphdrtype *) ipheadstart(newpak);
	    pak = newpak;
	    break;

	default:
	    break;
	}
    }

    /*
     * Now fix the IP header.
     */

    if (osrcadr != nsrcadr) {
	ip->srcadr = nsrcadr;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: s=%i->%i, d=%i [%d]", osrcadr, nsrcadr, ip->dstadr,
		   ip->id);
	ip->checksum = ip_checksum_adjust_long(ip->checksum, osrcadr, nsrcadr);
    }
    if (odstadr != ndstadr) {
	ip->dstadr = ndstadr;
	if (ipnat_debug && (!ipdebug_acl ||
			    ipnat_accesscheck(nat, ipdebug_acl)))
	    buginf("\nNAT: s=%i, d=%i->%i [%d]", ip->srcadr, odstadr, ndstadr,
		   ip->id);
	ip->checksum = ip_checksum_adjust_long(ip->checksum, odstadr, ndstadr);
    }

    return(pak);
}
