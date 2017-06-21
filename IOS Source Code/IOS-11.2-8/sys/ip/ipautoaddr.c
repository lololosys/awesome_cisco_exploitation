/* $Id: ipautoaddr.c,v 3.2.4.4 1996/08/28 12:49:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipautoaddr.c,v $
 *------------------------------------------------------------------
 * Support for auto-discovery of router IP addresses.
 *
 * December 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipautoaddr.c,v $
 * Revision 3.2.4.4  1996/08/28  12:49:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.4.3  1996/07/18  21:52:46  bcole
 * CSCdi57941:  Windows95 reports duplicate IP address on network
 * Branch: California_branch
 *
 * Place interface in promiscuous mode while using ARP for address
 * discovery.  This way we can completely & transparently spoof the web
 * client.
 *
 * Revision 3.2.4.2  1996/04/15  21:16:17  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2.4.1  1996/03/18  20:16:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  06:58:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  22:55:45  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:35  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "sys_registry.h"
#include "ip_registry.h"
#include "../iprouting/route.h"
#include "ip.h"
#include "ip_media.h"
#include "ieee.h"
#include "../ui/setup.h"
#include "../servers/domain.h"
#include "../servers/domain_private.h"
#include "logger.h"
#include "../ip/msg_ip.c"		/* Not a typo, see logger.h */
#include "ipautoaddr.h"
#include "config.h"
#include "media_registry.h"

/*
 * Definitions
 */
#define IP_AUTOADDR_ARP_RETRIES 5
#define IP_WEBINSTALL_NAME "new-router.cisco.com"
#define RESPFIXEDBYTES 14
#define MAXMATCHSIZE 50

/*
 * Local Storage
 */
static ipaddrtype ip_autoaddr_arp_seed = 0; /* For IP address discovery */
static pid_t dns_pid = 0;		/* Process ID for DNS snoop process */
static ipsocktype *dns_soc = NULL;	/* Socket for DNS snoop process */

/*
 * ip_autoaddr_can_use
 * Go ARP for an address a few times to see if we can use it.  If nobody
 * responds, assume it is OK for us to use it.  Return TRUE if so.
 */

static boolean ip_autoaddr_can_use (ipaddrtype seed, idbtype *swidb,
				    ipaddrtype addr, uchar *seed_mac)
{
    paktype *arppak;
    rfc_arp *rfcptr;
    int i;
    arptype *ptr;

    for (i = 0; i < IP_AUTOADDR_ARP_RETRIES; i++) {
	ptr = arp_table_lookup(LINK_IP, addr, swidb);
	if (!ptr)
	    ip_arp_incomplete(swidb, addr);
	else if (ptr->mode != ARP_INCOMPLETE)
	    return(FALSE);
	arppak = getbuffer(RBYTES);
	if (!arppak)
	    return(FALSE);
	rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);
	rfcptr->opcode = OP_REQ;
	rfcptr->arpro = PR_IP;
	rfcptr->arpln = PL_IP;
	rfcptr->arhln = HL_ETH;
	rfcptr->arhrd = HW_ETH;
	ieee_copy(seed_mac, rfcptr->ipshdw);
	PUTLONG(&rfcptr->ipspro, seed);
	ieee_zero(rfcptr->ipthdw);
	rfcptr->iptpro = addr;
	arp_send(arppak, RBYTES, (uchar *)baddr, LINK_ARP, swidb, ET_ARPA,
		 LINK_IP);
	process_sleep_for(ONESEC/2);
    }
    return(TRUE);
}

/*
 * ip_autoaddr_pick_ouraddr
 * Attempt to use ARP to auto discover an available IP address, given a seed
 * which is suppoesd to represent an IP address that is on the local cable.
 */

static ipaddrtype ip_autoaddr_pick_ouraddr (ipaddrtype seed, idbtype *swidb,
					    uchar *seed_mac)
{
    uchar host;
    ipaddrtype ouraddr;
    int i;

    host = seed & ~CLASS_C_MASK;
    for (i = host - 1; i > 0; i--) {
	ouraddr = (seed & CLASS_C_MASK) + i;
	if (ip_autoaddr_can_use(seed, swidb, ouraddr, seed_mac))
	    return(ouraddr);
    }
    for (i = host + 1; i < 256; i++) {
	ouraddr = (seed & CLASS_C_MASK) + i;
	if (ip_autoaddr_can_use(seed, swidb, ouraddr, seed_mac))
	    return(ouraddr);
    }
    return(0);
}

/*
 * ip_webinstall_forus
 * Is this a DNS packet for which we may want to spoof a reply?
 * If so, accept the packet, even if it doesn't seem like the packet is
 * for us.
 */

static boolean ip_webinstall_forus (void *iphdr)
{
    iphdrtype *ip;
    udptype *udp;

    ip = iphdr;
    if (!ip_autoaddr_arp_seed)
	return(FALSE);
    if (ip->prot != UDP_PROT)
	return(FALSE);
    udp = (udptype *)ipdatastart_iph(ip);
    if (udp->dport != DNS_PORT)
	return(FALSE);
    return(TRUE);
}

/*
 * ip_webinstall_addr
 * Routine to determine if the passed in IP address is to be considered
 * "ours", where ours really means the IP address that we chose during
 * IP address auto discovery.
 */

static boolean ip_webinstall_addr (idbtype *swidb, ipaddrtype addr)
{
    if (!ip_autoaddr_arp_seed)
	return(FALSE);
    return(addr == ip_autoaddr_arp_seed);
}

/*
 * domain_same
 * Compare DNS names in compressed format.
 * Return FALSE if names are different, TRUE if names are same.
 */

static boolean domain_same (uchar *name1, uchar *name2, int bytes)
{
    uchar len1, len2;

    while (bytes > 0) {
	len1 = *name1++;
	len2 = *name2++;
	if (len1 != len2)
	    return(FALSE);
	if (len1 == 0)
	    return(TRUE);
	bytes -= len1 + 1;
	while (len1--) {
	    if (toupper(*name1++) != toupper(*name2++))
		return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * domain_is_query
 * Does this DNS packet represent a query for the passed in name?
 * Return TRUE if so.
 */

static boolean domain_is_query (void *data, uchar *name)
{
    domaintype *dns;
    uchar comp_name[MAXMATCHSIZE];
    int qname_bytes;

    dns = data;
    if (dns->qr != 0)
	return(FALSE);
    if (dns->opcode != OPCODE_QUERY)
	return(FALSE);
    if (dns->qdcount != 1)
	return(FALSE);
    qname_bytes = comp_copy(comp_name, name, MAXMATCHSIZE, NULL);
    if (domain_same((uchar *)&dns[1], comp_name, qname_bytes) == TRUE)
	return(TRUE);
    return(FALSE);
}

/*
 * ip_dns_response
 * Build a DNS response packet.
 */

static void ip_dns_response (idbtype *if_out, udptype *udp_src, void *data,
			     ipaddrtype src, ipaddrtype dst, uchar *name)
		      
{
    paktype *pak;
    domaintype *dns_src, *dns;
    udptype *udp;
    int bytes, dns_bytes, query_bytes, qname_bytes;
    uchar *dns_stream;
    uchar comp_name[MAXMATCHSIZE];

    qname_bytes = comp_copy(comp_name, name, MAXMATCHSIZE, NULL);
    dns_src = data;
    query_bytes = sizeof(domaintype) + qname_bytes + QUERYFIXEDBYTES;
    dns_bytes =  query_bytes + qname_bytes + RESPFIXEDBYTES;
    bytes = IPHEADERBYTES(NOPT) + UDPHEADERBYTES + dns_bytes;
    pak = getbuffer(bytes);
    if (!pak)
	return;
    udp = (udptype *)(ipheadstart(pak) + IPHEADERBYTES(NOPT));
    dns = (domaintype *)((uchar *)udp + UDPHEADERBYTES);
    udp->sport = DNS_PORT;
    udp->dport = udp_src->sport;
    udp->length = UDPHEADERBYTES + dns_bytes;
    bcopy(dns_src, dns, query_bytes);
    dns->qr = 1;
    dns->aa = 1;
    dns->ra = 0;
    dns->reserved = 0;
    dns->rcode = RCODE_OKAY;
    dns->ancount = 1;
    dns->nscount = dns->arcount = 0;
    dns_stream = (uchar *)dns + query_bytes;
    /*
     * Now construct response section
     */
    bcopy(comp_name, dns_stream, qname_bytes);
    dns_stream += qname_bytes;
    PUTSHORT(dns_stream, TYPE_A);
    dns_stream += 2;
    PUTSHORT(dns_stream, CLASS_IN);
    dns_stream += 2;
    /*
     * 30 seconds TTL
     */
    PUTLONG(dns_stream, 30);
    dns_stream += 4;
    PUTSHORT(dns_stream, sizeof(ipaddrtype));
    dns_stream += 2;
    PUTLONG(dns_stream, src);

    udp->checksum = 0;
    udp->checksum = CheckSum((uchar *)udp, udp->length, src, dst, UDP_PROT);
    ipwrite(pak, bytes, UDP_PROT, src, dst, if_out, NULL, NULL);
}

/*
 * ip_webinstall_supported
 * Determine if the interface is one on which we support this 'clickstart'
 * feature.
 */

boolean ip_webinstall_supported (hwidbtype *hwidb)
{
    switch (hwidb->type) {
    case IDBTYPE_LANCE:
    case IDBTYPE_QUICC_ETHER:
	return(TRUE);
    default:
	return(FALSE);
    }
}

/*
 * ip_webinstall_nopromisc_if
 * Take interface out of promiscuous mode.
 */

static void ip_webinstall_nopromisc_if (hwidbtype *hwidb)
{
    if (!ip_webinstall_supported(hwidb))
	return;
    /*
     * Take interface out of promiscuous mode
     */
    (*hwidb->reset)(hwidb);
}

/*
 * ip_webinstall_nopromisc
 * Take all interfaces out of promiscuous mode.
 */

static void ip_webinstall_nopromisc (void)
{
    hwidbtype *hwidb;

    reg_delete_media_if_should_use_promiscuous();
    FOR_ALL_HWIDBS(hwidb) {
	ip_webinstall_nopromisc_if(hwidb);
    }
}

/*
 * ip_webinstall_promisc
 * Put interface into promiscuous mode.
 */

static void ip_webinstall_promisc (hwidbtype *hwidb)
{
    if (!ip_webinstall_supported(hwidb))
	return;
    reg_add_media_if_should_use_promiscuous(ip_webinstall_supported,
					    "ip_webinstall_supported");
    (*hwidb->reset)(hwidb);
}

/*
 * ip_dns_killproc
 * Disable IP address auto discovery.  Kill DNS snoop process.  Cleanup.
 */

void ip_dns_killproc (boolean silent)
{
    if (!dns_pid)
	return;
    ip_webinstall_nopromisc();
    ip_autoaddr_arp_seed = 0;
    /*
     * Should be able to call reg_delete_..., but this is a stub registry,
     * so use the long form...
     */
    registry_delete_list(REG_IP, SERVICE_IP_ARP_FOR_PROTO, ip_webinstall_addr);
    reg_delete_ip_webinstall_forus();
    if (dns_soc) {
	close_ipsocket(dns_soc);
	dns_soc = NULL;
    }
    if (!silent)
	errmsg(&msgsym(WEBINST_KILL, IP));
    process_kill(dns_pid);
    dns_pid = 0;
}

/*
 * ip_dns_setup_end
 * Registry wrapper routine to stop attempting IP address auto-discovery,
 * and the DNS snoop process.
 */

static void ip_dns_setup_end (setup_struct *sp)
{
    ip_dns_killproc(FALSE);
}

/*
 * ip_dns_snoop
 * Process to listen for DNS requests that fly by on the wire.  Use these
 * requests to originate IP address auto-discovery.  Spoof a reply to
 * the DNS request, so that a web browser can locate us.
 */

static process ip_dns_snoop (void)
{
    paktype *pak;
    udptype *udp;
    void *data;
    ipaddrtype seed;
    ipaddrtype ouraddr;
    hwaddrtype hdaddr;
    idbtype *swidb;
    iphdrtype *ip;

    dns_soc = open_ipsocket(UDP_PROT, 0, 0, DNS_PORT, DNS_PORT, SOCK_ANYSRC);
    if (!dns_soc) {
	ip_dns_killproc(FALSE);
	return;
    }
    seed = 0;
    pak = NULL;
    swidb = NULL;
    while (!netsareup && !seed) {
	while ((pak = read_ipsocket(dns_soc, TRUE, 5*ONESEC, &udp, &data))) {
	    swidb = pak->if_input;
	    /*
	     * Only consider LAN media
	     */
	    if ((swidb->hwptr->status & IDB_IEEEMASK) == 0) {
		retbuffer(pak);
		continue;
	    }
	    if (domain_is_query(data, IP_WEBINSTALL_NAME)) {
		seed = iphdrtype_start(pak)->srcadr;
		break;
	    }
	    retbuffer(pak);
	}
    }
    if (!seed) {
	ip_dns_killproc(FALSE);
	return;
    }
    errmsg(&msgsym(WEBINST_START, IP), seed);
    /*
     * Now we know the IP address of the station which is trying to configure
     * us.
     */
    ip_autoaddr_arp_seed  = seed;
    (*swidb->hwptr->extract_snpa)(pak, &hdaddr, SOURCE);
    /*
     * Put the interface into promiscuous mode, but only for the duration
     * of the address discovery.
     */
    ip_webinstall_promisc(swidb->hwptr);
    ouraddr = ip_autoaddr_pick_ouraddr(seed, swidb, hdaddr.addr);
    reg_delete_media_if_should_use_promiscuous();
    ip_webinstall_nopromisc_if(swidb->hwptr);
    if (ouraddr)
	errmsg(&msgsym(WEBINST_COMP, IP), ouraddr);
    if (!swidb->ip_address) {
	swidb->ip_address = ouraddr;
	swidb->resolvemethod = RES_HEURISTIC;
	class_resolve(swidb);
	ipisresolved();
	setup_abort();
    }
    retbuffer(pak);
    reg_add_setup_end(ip_dns_setup_end, "ip_dns_setup_end");
    /*
     * Now prepare to answer the next DNS request with our newly
     * acquired IP address
     */
    for (;;) {
	while ((pak = read_ipsocket(dns_soc, TRUE, 5*ONESEC, &udp, &data))) {
	    swidb = pak->if_input;
	    /*
	     * Only consider LAN media
	     */
	    if ((swidb->hwptr->status & IDB_IEEEMASK) == 0) {
		retbuffer(pak);
		continue;
	    }
	    if (domain_is_query(data, IP_WEBINSTALL_NAME)) {
		ip = iphdrtype_start(pak);
		seed = ip->srcadr;
		(*swidb->hwptr->extract_snpa)(pak, &hdaddr, SOURCE);
		errmsg(&msgsym(WEBINST_RESP, IP), seed, ip->dstadr);
		ip_arp_table_add(swidb, seed, hdaddr.addr, ET_ARPA,
				 ARP_DYNAMIC);
		ip_dns_response(swidb, udp, data, ouraddr, seed,
				IP_WEBINSTALL_NAME);
	    }
	    retbuffer(pak);
	}
    }
    ip_dns_killproc(FALSE);
}

/*
 * ip_webinstall
 * Prepare to attempt auto-discovery of our IP address, if this image supports
 * configuration of the router via HTTP.
 */

void ip_webinstall (void)
{
    reg_add_ip_arp_for_proto(ip_webinstall_addr, "ip_webinstall_addr");
    reg_add_ip_webinstall_forus(ip_webinstall_forus, "ip_webinstall_forus");
    dns_pid = process_create(ip_dns_snoop, "DNS Snoop", NORMAL_STACK,
			     PRIO_NORMAL);
}
