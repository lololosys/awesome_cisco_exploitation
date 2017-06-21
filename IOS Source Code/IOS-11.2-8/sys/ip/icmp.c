/* $Id: icmp.c,v 3.9.12.8 1996/07/29 22:45:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/icmp.c,v $
 *------------------------------------------------------------------
 * icmp.c -- Internet Message Control Protocol
 *
 * 15-September-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: icmp.c,v $
 * Revision 3.9.12.8  1996/07/29  22:45:34  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/07/22  19:51:40  slin
 * CSCdi63477:  Router will not send ICMP Echo Replies to an IP alias
 * Branch: California_branch
 * - Allow ICMP replies to IP aliases
 *
 * Revision 3.9.12.6  1996/06/18  01:44:38  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/05/13  23:15:19  bcole
 * CSCdi56893:  System restarted by error - a SegV exception, PC 0x60208B9C
 * Branch: California_branch
 *
 * Blow off attempts to send ICMP quench at interrupt level.  Main
 * forwarding table may be in an inconsistent state and not available for
 * consultation at interrupt level.
 *
 * Revision 3.9.12.4  1996/05/11  02:30:07  tylin
 * CSCdi56413:  Multicast ping should have a ttl option
 *              Added for extended command only.
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/04/19  15:41:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.8  1996/04/09  20:35:36  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.7  1996/03/28  21:01:06  bew
 * Branch: IosSec_branch
 * Change the name from send_echo_continued() to the more standard
 * send_echo_internal().
 *
 * Revision 2.2.6.6  1996/02/21  03:28:50  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.5  1996/01/23  05:42:24  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.4  1996/01/04  19:01:43  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 2.2.6.3  1995/12/17  01:39:36  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.9.12.2  1996/04/15  21:15:34  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.9.12.1  1996/03/18  20:15:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.3  1996/03/16  06:57:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.6.2  1996/03/07  09:45:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.6.1  1996/02/20  00:58:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/16  03:08:22  bcole
 * CSCdi49073:  Runt ICMP time exceeded packets cause packet buffer leak
 *
 * Recently added code to sanity check ICMP packet length could fail to do
 * anything with the packet buffer.  Make sure to relinguish it.
 *
 * Revision 3.8  1996/02/09  23:03:42  dbath
 * CSCdi47381:  7505 router will not netboot across FDDI
 * While we're netbooting, don't send ICMP  unreachables to our server,
 * or he'll stop feeding us.
 *
 * Revision 3.7  1996/01/24  20:14:45  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/11  02:26:22  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.5  1996/01/10  02:22:02  raj
 * CSCdi40929:  traceroute fails, ping works. port unreachables not sent
 * Allow unreachables within the first 3 minutes and fix timer code to
 * remove ip_icmpokay variable.
 *
 * Revision 3.4  1995/11/22  09:17:56  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  09:33:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:54:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/25  08:31:47  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.6  1995/09/13  00:24:16  jenny
 * CSCdi39167:  ICMP alignment errors
 * ICMP source quench message may be generated for a packet on the
 * fastswitching path. Use longword reference when accessing source
 * and dest addresses in the original IP header since packet alignment
 * is not done on that path.
 *
 * Revision 2.5  1995/09/11  23:48:59  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.4  1995/09/09 00:53:10  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.3  1995/06/28  23:50:18  myeung
 * CSCdi30267:  router does a tight loop in ip_sendnet when fed a weird
 * sequence
 * - Add ability to detect, drop and log information about the looping
 *   packet
 *
 * Revision 2.2  1995/06/11  23:29:09  dkatz
 * CSCdi35384:  ICMP timestamp packets have bad checksums
 * Calculate the checksum of reply packets over the length of the received
 * request.  Validate packet lengths according to ICMP packet type.  Get
 * rid of ugly pointer math in redirects.
 *
 * Revision 2.1  1995/06/07  20:56:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ip.h"
#include "ip_debug.h"
#include "../tcp/tcp.h"
#include "ipoptions.h"
#include "../iprouting/route.h"
#include "../iprouting/irdp.h"
#include "../iprouting/gdp.h"
#include "../if/network.h"
#include "ip_media.h"
#include "ieee.h"
#include "../ip/ipaddress.h"
#include "../os/clock.h"
#include "../util/random.h"

/*
 * Local storage
 */
icmp_traffic_t icmp_traffic;
boolean gdp_irdp_multicast;


/*
 * icmp_sendsetup
 * Common setup routines for sending an ICMP message about a datagram.
 * Do martian checking, ignore ICMP and any broadcasts.
 * Allocate a second buffer, zero ICMP header fields (first 8 bytes)
 * and copy header and 64 data bits of original message to appropriate
 * location.
 */

#define DATABYTES 8

icmptype *icmp_sendsetup (paktype *pak, paktype **newpak, ulong *size)
{
    iphdrtype *ip;
    icmptype *icmp;
    paktype *newp;
    ipaddrtype source, destination;

    ip = iphdrtype_start(pak);

    if (pak->if_input == NULL)
	return(NULL);
    /*
     * icmp_sendsetup() could be called from the fastswitching path
     * when both the TX ring and hold-queue for the outgoing interface
     * are full. Make sure always use longword reference when accessing
     * the source and destination addresses in IP header since packet 
     * alignment is not performed on the fastswitching path.
     */ 
    source = GETLONG(&ip->srcadr);
    destination = GETLONG(&ip->dstadr);
    if (martian(n2hl(source),TRUE) || martian(n2hl(destination),TRUE))
	return(NULL);
    icmp = ((icmptype *)ipdatastart(pak));
    if ((ip->prot == ICMP_PROT && (icmp->type != ICMP_ECHO) && (icmp->type != ICMP_ECHOREPLY)) ||
	(pak->flags & PAK_BROADCAST) || physical_broadcast(pak) == FUZ_TRUE)
	return(NULL);
    *size = IPHEADERBYTES(NOPT) + ICMPHEADERBYTES + ipheadsize(ip) + DATABYTES;
    newp = getbuffer(*size);
    if (!newp)
	return(NULL);
    icmp = (icmptype *)(newp->network_start + IPHEADERBYTES(NOPT));
    memset(icmp, 0, ICMPHEADERBYTES);
    bcopy(ip, (((char *) icmp) + ICMPHEADERBYTES), ipheadsize(ip) + DATABYTES);
    *newpak = newp;
    return(icmp);
}

/*
 * icmp_routing_up
 * Check if we can send ICMP redirects.
 * We delay for a small amount of time as the system is initializing
 * before we send any ICMP messages relating to routing.  Note that
 * terminal servers send unreachables to (esp protocol and port
 * unreachables), so don't demand that router_enable be true.
 */

boolean icmp_routing_up (void)
{

    if (netsareup) {
        if (!TIMER_RUNNING(ip_icmptimer))
	    return(TRUE);
	if (AWAKE(ip_icmptimer)) {
	    TIMER_STOP(ip_icmptimer);
	    return(TRUE);
	}
    }
    
    return(FALSE);
}

/*
 * send_echoreply
 * Respond to an ICMP Echo with an ICMP Echo Reply
 */

static void send_echoreply (paktype *pak, icmptype *icmp, int size)
{
    iphdrtype *ip;
    ipaddrtype newsrc;

    ip  = (iphdrtype *)ipheadstart(pak);

    newsrc = ip_reply_srcadr(pak);
    if (newsrc == 0L) {
	datagram_done(pak);
	return;
    }

    /*
     * Change datagram type, recompute checksums, and route it back to sender.
     */
    icmp->type = ICMP_ECHOREPLY;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp,size);
    icmp_traffic.echorepsent++;
    if (icmppkt_debug)
	buginf("\nICMP: echo reply sent, src %i, dst %i",
	       newsrc, ip->srcadr);
    /*
     * We reuse the packet, clear the PAK_SENDSELF flag first.
     * Otherwise, ping to address of interface with IDB_HEARSELF
     * cleared, for example, fddi, loopback, etc., will fail.
     */
    pak->flags &= ~PAK_SENDSELF;
    if (ip_aliasaddress(ip->srcadr))
	pak->flags |= PAK_ALIAS; /* Allow replies to IP aliases */
    
    ipwrite(pak,0L,ICMP_PROT,newsrc,ip->srcadr,NULL,NULL,&ipdefaultparams);
}

/*
 * echoBLOCK
 * Wait for an ICMP Echo Reply or timeout.
 */

static boolean echoBLOCK (echotype *data)
{
    if (!data->active)			  /* if block flag reset, wakeup */
	return(FALSE);
    if (data->interactive &&
	psipending2(data->tty))		  /* interrupted? */
	return(FALSE);
    if (SLEEPING(data->timeout))	  /* keep blocking if timer ticking */
	return(TRUE);
    return(FALSE);
}

/*
 * send_echo
 * Send an ICMP Echo to a host and block for a reply.
 * Returns TRUE if an Echo Reply received.
 * FALSE under all other circumstances.  data->how is set based on
 * the result of the ping.
 */

boolean send_echo (ipaddrtype destination, echotype *data, int bytes,
		   ushort pattern, ipaddrtype source, boolean nofrag,
		   idbtype *idb)
{
    paktype *pak;
    iphdrtype *ip;
    icmptype *icmp;
    int databytes, i, headsize;
    ushort *dataptr;

    headsize = IPHEADERBYTES(data->options) + sizeof(icmptype);
    if (bytes < headsize)
	bytes = headsize;
    pak = getbuffer(bytes);
    if (!pak)
	return(FALSE);
    pak->datagramsize = bytes;

    ip = (iphdrtype *)ipheadstart(pak);
	
    /*
     * We initialize the packet buffer by zeroing the IP and ICMP header.
     */
    databytes = bytes - headsize;
    data->length = btow(databytes - 1) - ICMP_ROUTER_TS_WORDS;	/* Don't include timestamp */
    if (data->length < 0)		/* Too short... */
	data->length = 0;
    icmp = (icmptype *) ((char *)ip + IPHEADERBYTES(data->options));

    /*
     * Fill in the pattern data.  If we're rotating the pattern, use the
     * sequence as the initial value, and bump it once per word.
     */
    if (data->rotating_pattern)
	pattern = data->sequence;
    dataptr = (ushort *) &(icmp->data[ICMP_ROUTER_TS_LONGS]); /* The timestamp comes first. */
    for (i = 0; i < data->length; i++) {
	*dataptr++ = pattern;
	if (data->rotating_pattern)
	    pattern++;
    }
    memset(pak->network_start, 0, bytes-databytes);
    data->pattern = pattern;
    data->corruption = 0;
    data->corrupted_words = 0;

    return(send_echo_internal(destination, data, bytes, source, nofrag,
			       idb, pak, 0, FALSE));
}

/*
 * send_echo_internal
 * Common routine used by send_echo and crypto_ip_send_echo.
 */
boolean send_echo_internal (ipaddrtype destination, echotype *data, int bytes,
			     ipaddrtype source, boolean nofrag, idbtype *idb,
			     paktype *pak, uchar code, boolean crypto)
{
    iphdrtype *ip;
    icmptype *icmp;
    ipparamtype params;
    sys_timestamp now;


    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *) ((char *)ip + IPHEADERBYTES(data->options));

    /*
     * Fill in the ICMP and IP portion of the datagram
     */
    icmp->type = ICMP_ECHO;
    icmp->code = code;
    icmp->checksum = 0;
    icmp->identifier = h2n(data->id);
    icmp->sequence = h2n(data->sequence);
    if (!crypto) {
	GET_TIMESTAMP(now);
	PAK_PUT_TS(icmp->data, now);
    }
    icmp->checksum=ipcrc((ushort *)icmp,(bytes-IPHEADERBYTES(data->options)));
    if (ip_aliasaddress(destination))
	pak->flags |= PAK_ALIAS;
    params = ipdefaultparams;           /* struct copy */
    params.tos = data->tos;
    params.ttl = data->ttl;
    params.dontfragment = nofrag;
    params.ident = (short)icmp_traffic.echosent;	/* use for unique id */
    ipwrite(pak,bytes,ICMP_PROT,source,destination,idb,data->options,&params);
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    enqueue(&echoQ, data);
    icmp_traffic.echosent++;
    edisms((blockproc *)echoBLOCK, (ulong)data);
    if ((data->active) || (crypto))
	unqueue(&echoQ, data);

    return(data->active ? FALSE : TRUE);
}

/*
 * rcv_echoreply
 * Handle an Echo Reply, possibly signal upper levels
 */

static void rcv_echoreply (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;
    echotype *ep, *child, *current_ep;
    sys_timestamp time_sent;
    int wordindex;
    ushort *dataptr;
    
    ip = (iphdrtype *)ipheadstart(pak);

    if (icmppkt_debug)
	buginf("\nICMP: echo reply rcvd, src %i, dst %i",
	       ip->srcadr, ip->dstadr);

    for (ep = (echotype *) echoQ.qhead; ep; ep = ep->next) {
	if (n2h(icmp->identifier) == ep->id &&
	    n2h(icmp->sequence) == ep->sequence && ep->active) {
 	    current_ep = ep;
 	    if (ep->broadcast && (ep->how == '!' || ep->how == 'C')) {
		child = malloc(sizeof(echotype));
		if (child) {
		    child->child = ep->child;
		    ep->child = child;
 		    current_ep = child;
		}
	    } else {
		if (ep->options)
		    extract_ipoptions(ip, ep->options);
 	    }
 	    current_ep->how = '!';
 	    current_ep->who = ip->srcadr;
	    PAK_GET_TS(icmp->data, &time_sent);
 	    current_ep->deltatime = ELAPSED_TIME(time_sent);
 
 	    /*
 	     * If requested, validate the received packet.  If a rotating
 	     * pattern is in use, duplicate it here.
 	     */
 	    if (ep->validate) {
 		dataptr = (ushort *) &(icmp->data[ICMP_ROUTER_TS_LONGS]); /* Skip the timestamp */
 		if (ep->rotating_pattern)
 		    ep->pattern = ep->sequence;
 		for (wordindex = 0; wordindex < ep->length; wordindex++) {
 		    if (*dataptr != ep->pattern) {
 			if (current_ep->corrupted_words == 0) {
 			    current_ep->corruption = *dataptr;
 			    current_ep->how = 'C';
 			    current_ep->corruption_offset =
 				(uchar *) dataptr - (uchar *) ip;
 			}
 			current_ep->corrupted_words++;
 		    }
 		    dataptr++;
 		    if (ep->rotating_pattern)
 			ep->pattern++;
 		}
 		if (icmppkt_debug && current_ep->corrupted_words) {
 		    buginf(", corrupted, pattern %-0#6x, offset %d, count %d",
 			   current_ep->corruption,
 			   current_ep->corruption_offset,
 			   current_ep->corrupted_words);
 		}
	    }
	    if (!ep->broadcast) {
		ep->active = FALSE;
		unqueue(&echoQ, ep);
	    }
	    break;
	}
    }
    retbuffer(pak);
}

/*
 * icmp_quench
 * What to do when ICMP protocol gets a source quench.
 * Take action only if returned packet was ECHO.
 */

static void icmp_quench (iphdrtype *iph, ipaddrtype source)
{
    icmptype *icmp;
    echotype *ep;

    icmp = (icmptype *)ipdatastart_iph(iph);
    if (icmp->type != ICMP_ECHO)
	return;
    for (ep = (echotype *) echoQ.qhead; ep; ep = ep->next) {
	if (icmp->identifier == ep->id &&
	    icmp->sequence == ep->sequence && ep->active) {
	    ep->how = 'Q';
	    ep->who = source;
	    ep->active = FALSE;
	    unqueue(&echoQ, ep);
	    break;
	}
    }
}

/*
 * icmp_unreachable
 * What to do when ICMP protocol gets an unreachable response.
 * Take action only if returned packet was ECHO.  This routine also
 * handles a code 4 unreachable (frag needed and DF set).  If ipmtu == 0
 * then it was a normal unreachable.
 */

static void icmp_unreachable (iphdrtype *iph, ipaddrtype source, ushort ipmtu)
{
    icmptype *icmp;
    echotype *ep;
  
    icmp = (icmptype *)ipdatastart_iph(iph);
    if (icmp->type != ICMP_ECHO)
        return;
    for (ep = (echotype *) echoQ.qhead; ep; ep = ep->next) {
	if (n2h(icmp->identifier) == ep->id &&
	      n2h(icmp->sequence) == ep->sequence && ep->active) {
	    if ( ipmtu ) {
		ep->how = 'M';
		ep->mtu = ipmtu;
	    } else
		ep->how = 'U';
	    ep->who = source;
	    ep->active = FALSE;
	    unqueue(&echoQ, ep);
	    break;
	}
    }
}

/*
 * send_maskreply
 * Send a Address Mask Reply in response to an Address Mask Request.
 * Do this only if we're a gateway.  NOTE that we are reusing a
 * packet that may have options that we need to keep.  Do not get
 * confused by the hidden security option kludge.
 *
 * Assumption: if_input is not NULL.  This means that this routine
 * is never called internally except in the normal processing of
 * inbound icmp mask request messages which will always have an
 * input inteface.
 */

static void send_maskreply (paktype *pak, icmptype *icmp, int size)
{
    iphdrtype *ip;
    idbtype *idb;
    ipaddrtype newdest;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Ignore if we're not sending replies or if we're not a router,
     * or input interface isn't ready to answer.
     */
    idb = pak->if_input;
    if (!idb->ip_maskreply || !ip_router_running ||
	!idb->ip_enabled || idb->ip_address == 0L) {
	retbuffer(pak);
	return;
    }
    newdest = n2hl(ip->srcadr);
    if (newdest == 0L)
	newdest = -1L;
    icmp->data[0] = h2nl(idb->ip_nets_mask);
    icmp->type = ICMP_MASKREPLY;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size);
    if (icmppkt_debug)
	buginf("\nICMP: sending mask reply (%i) to %i via %s",
		icmp->data[0], newdest, idb->namestring);
    icmp_traffic.maskrepsent++;
    ipwrite(pak,0L,ICMP_PROT,idb->ip_address, newdest,idb,NULL,&ipdefaultparams);
}

/*
 * rcv_maskreply
 * Utilize the information in an Address Mask Reply that we originated.
 */

static const char maskmesg[] = "\nICMP: %smask reply %#i from %i";

static void rcv_maskreply (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;
    ulong *ptr;
    idbtype *idb;
    ulong code;
    ipaddrtype mask;

    ip  = (iphdrtype *)ipheadstart(pak);

    ptr = (ulong *) icmp;	/* point to first 32bits of ICMP message */
    ptr++;			/* first long is ICMP header */
    code = *ptr++;		/* second should be the interface id */
    mask = n2hl(*ptr);		/* third should be mask */
    idb = pak->if_input;	/* input interface */
    if (idb->ip_maskid != 0L && code == idb->ip_maskid) {  /* from us? */
	if (icmppkt_debug)
	    buginf(maskmesg, "", mask, n2hl(ip->srcadr));
	if(!mask || icmp->code || !good_address(n2hl(ip->srcadr),mask,TRUE)){
	   retbuffer(pak); /* flush the luser */
	   return;
	}
	idb->ip_nets_mask = mask;
	if (idb->ip_address)
	    class_resolve(idb);
    } else if (icmppkt_debug)
	buginf(maskmesg, "unexpected ", mask, n2hl(ip->srcadr));
    retbuffer(pak);	/* flush datagram */
}

/*
 * send_redirect
 * Send an ICMP redirect.  This routine does some checks for the
 * appropriateness of the redirect.  If the source is a martian, ICMP (non
 * echo), or broadcast punt.  It is assumed that our caller has checked for
 * other redirect blow offs.
 *
 * 'myself' is to handle "internal" redirects generated by option processing.
 *
 * Send a network redirect if we are routing according to an ndb, a host
 * redirect otherwise.
 */

void send_redirect (paktype *pak, ipaddrtype dhost)
{
    iphdrtype *ip;
    paktype *red;
    register icmptype *icmp;
    ipaddrtype myself;
    ulong size;

    ip = (iphdrtype *)ipheadstart(pak);

    if ((pak->flags & PAK_NOREDIRECT) || martian(dhost,TRUE))
	return;
    if (!icmp_routing_up())
	return;
    icmp = icmp_sendsetup(pak, &red, &size);
    if (!icmp)
	return;
    icmp->type = ICMP_REDIRECT;
    /*
     * Should be able to just unconditionally send a host redirect, but
     * MILNET relies upon network redirects where applicable.
     */
    if (reg_invoke_iprouting_via_nonsubnet(ip->dstadr))
	icmp->code = ICMP_REDNET;
    else 
	icmp->code = ICMP_REDHOST;
    dhost = h2nl(dhost);
    bcopy((char *) & dhost, ((char *) icmp) + 4, sizeof(ipaddrtype));
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    icmp_traffic.redirectsent++;
    myself = pak->if_input ? pak->if_input->ip_address :
	     ip_best_local_address(ip->srcadr, TRUE);
    if (icmppkt_debug)
	buginf("\nICMP: redirect sent to %i for dest %i, use gw %i",
		ip->srcadr, ip->dstadr, n2hl(dhost));
    ipwrite(red,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, NULL, &ipdefaultparams);
}

/*
 * rcv_redirect
 * Receive an ICMP redirect, update redirect table
 * Make sure we are sender of the original packet.
 * Ignore any physical broadcasts or non-host redirects.
 */

static void rcv_redirect (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;
    ipaddrtype address, gateway;
    ipaddr_tabletype *addr;
    idbtype *idb;
    iphdrtype *iph;
  
    iph = icmp_embedded_iph(pak);
    ip  = (iphdrtype *)ipheadstart(pak);
    if ((icmp->code == ICMP_REDHOST || icmp->code == ICMP_REDNET) &&
	physical_broadcast(pak) != FUZ_TRUE) {
	bcopy(&iph->srcadr, &address, sizeof(ipaddrtype));
	idb = NULL;
	FOR_IPADDR_HASH_CHAIN(address, addr) {
	    if (addr->address == address) {
		idb = addr->swidb;
		break;
	    }
	}
	if (idb) {
	    bcopy(&icmp->redir_gw, &gateway, sizeof(ipaddrtype));
	    bcopy(&iph->dstadr, &address, sizeof(ipaddrtype));
	    gateway = n2hl(gateway);
	    address = n2hl(address);
	    if (icmppkt_debug)
	        buginf("\nICMP: redirect rcvd from %i -- for %i use gw %i",
			ip->srcadr, address, gateway);
	    path_update(address, gateway, pak->if_input);
	}
    }
    retbuffer(pak);
}

/*
 * unreach2string
 * Given an ICMP unreachable code, return a descriptive string
 */

char *unreach2string (uchar code)
{
    switch (code) {
    case ICMP_UNRNET:		return("net");
    case ICMP_UNRHOST:		return("host");
    case ICMP_UNRPROTOCOL:	return("protocol");
    case ICMP_UNRPORT:		return("port");
    case ICMP_UNRFRAG:		return("frag. needed and DF set");
    case ICMP_UNRSRFAIL:	return("source route failed");
    case ICMP_UNRNONET:		return("no such network");
    case ICMP_UNRNOHOST:	return("no such host");
    case ICMP_UNRHOSTISOLATED:	return("host isolated");
    case ICMP_UNRNETPROHIB:	return("network prohibited");
    case ICMP_UNRPROHIB:	return("prohibited");
    case ICMP_UNRNETTOS:	return("net and TOS");
    case ICMP_UNRHOSTTOS:	return("host and TOS");
    case ICMP_UNRADMIN:		return("administratively prohibited");
    case ICMP_UNRHOSTPREC:	return("host precedence");
    case ICMP_UNRPRECEDENCE:	return("precedence cutoff");
    }
    return("UNKNOWN");
}

/*
 * send_unreachable
 * Send an ICMP destination unreachable message
 * We always send a host unreachable message
 * Ignore ICMP's and broadcasts 
 */

void send_unreachable (paktype *pak, uchar code)
{
    iphdrtype *ip;
    paktype *newp;
    icmptype *icmp;
    ipaddrtype myself;
    idbtype *idb;
    ulong size;

    ip  = (iphdrtype *)ipheadstart(pak);

    /*
     * Don't send unreachables:
     *   - faster than once a second per interface.
     *   - if we're netbooting
     *   - if interface has unreachables disabled.
     */ 
    idb = pak->if_input;
    if (!idb ||
	CLOCK_IN_INTERVAL(idb->ip_unreach_timer, idb->ip_unreach_interval))
	return;
    if (!netsareup || !idb->ip_unreach || system_loading)
	return;
    myself = pak->if_input->ip_address;
    icmp = icmp_sendsetup(pak, &newp, &size);
    if (!icmp)
	return;
    icmp->type = ICMP_UNREACHABLE;
    icmp->code = code;
    icmp->identifier = 0;
    if ( code == ICMP_UNRFRAG ) { /* maybe we should call h2ns() here? */
	icmp->next_hop_mtu = pak->if_output->ip_mtu - 
	    ((ulong)ip - (ulong) pak->datagramstart);
    } else {
	icmp->sequence = 0;
    }
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    icmp_traffic.unreachsent++;
    if (icmppkt_debug)
	buginf("\nICMP: dst (%i) %s unreachable sent to %i",
	    n2hl(ip->dstadr), unreach2string(code), n2hl(ip->srcadr));
    ipwrite(newp,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, NULL, &ipdefaultparams);
    GET_TIMESTAMP(idb->ip_unreach_timer);
}

/*
 * rcv_unreachable
 * Someone sent an ICMP destination unreachable to us.
 */

static void rcv_unreachable (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;
    iphdrtype *ip_embed;

    ip       = (iphdrtype *)ipheadstart(pak);
    ip_embed = (iphdrtype *)icmp_embedded_iph(pak);

    if (icmppkt_debug)
	buginf("\nICMP: dst (%i) %s unreachable rcv from %i",
		ip->dstadr, unreach2string(icmp->code), ip->srcadr);
    switch (ip_embed->prot) {
    case ICMP_PROT:
	icmp_unreachable(ip_embed, ip->srcadr,
			 icmp->code == ICMP_UNRFRAG ? icmp->next_hop_mtu : 0);
	break;
    default:
	reg_invoke_ip_icmp_rcvunreach(ip_embed->prot, ip_embed, ip->srcadr,
				      icmp);
	break;
    }
    retbuffer(pak);		/* flush datagram */
}

/*
 * send_timeexceeded
 * Send an ICMP time exceeded message
 */

void send_timeexceeded (paktype *pak, long code)
{
    iphdrtype *ip;
    icmptype *icmp;
    ipaddrtype myself;
    paktype *newp;
    ulong size;

    ip = (iphdrtype *)ipheadstart(pak);

    icmp = icmp_sendsetup(pak, &newp, &size);
    if (!icmp)
	return;
    icmp->type = ICMP_TIMEEXCEEDED;
    icmp->code = (uchar) code;
    icmp->identifier = 0;
    icmp->sequence = 0;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    icmp_traffic.ttlsent++;
    myself = pak->if_input ? pak->if_input->ip_address :
	     ip_best_local_address(ip->srcadr, FALSE);
    if (icmppkt_debug)
	buginf("\nICMP: time exceeded (%s) sent to %i (dest was %i)",
	code == ICMP_TIMTTL ? "time to live" : "reassembly",
	n2hl(ip->srcadr), n2hl(ip->dstadr));
    ipwrite(newp,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, NULL, &ipdefaultparams);
}

/*
 * send_parameter
 * Send an ICMP parameter problem message
 */

void send_parameter (paktype *pak, int code, int offset)
{
    iphdrtype *ip;
    icmptype *icmp;
    ipaddrtype myself;
    paktype *newp;
    ulong size;

    ip = (iphdrtype *)ipheadstart(pak);

    icmp = icmp_sendsetup(pak, &newp, &size);
    if (!icmp)
	return;
    icmp->type = ICMP_PARAMETER;
    icmp->code = code;
    icmp->identifier = 0;
    icmp->sequence = 0;
    *(((uchar *) icmp) + 4) = offset;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    myself = pak->if_input ? pak->if_input->ip_address :
	     ip_best_local_address(ip->srcadr, FALSE);
    icmp_traffic.paramsent++;
    if (icmppkt_debug)
	buginf("\nICMP: parameter problem sent to %i (dest was %i)",
	    n2hl(ip->srcadr), n2hl(ip->dstadr));
    ipwrite(newp,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, NULL, &ipdefaultparams);
}

/*
 * rcv_parameter
 * Process a received parameter problem message.  Eventually, this may
 * be passed up to higher levels, but for now it just buginfs and discards
 * the packet.
 */

static void rcv_parameter (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (icmppkt_debug)
	buginf("\nICMP: parameter problem rcvd %i",
		n2hl(ip->srcadr));
    retbuffer(pak);		/* flush datagram */
}

/*
 * rcv_quench
 * Process a received quench problem message.
 */

static void rcv_quench (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip, *embed_ip;

    ip       = (iphdrtype *)ipheadstart(pak);
    embed_ip = icmp_embedded_iph(pak);

    if (icmppkt_debug)
	buginf("\nICMP: source quench rcvd %i", n2hl(ip->srcadr));
    switch (embed_ip->prot) {
    case ICMP_PROT:
	icmp_quench(embed_ip, ip->srcadr);
	break;
    default:
	reg_invoke_ip_icmp_rcvquench(embed_ip->prot, embed_ip, ip->srcadr);
	break;
    }
    retbuffer(pak);		/* flush datagram */
}

/*
 * send_quench
 * Send an ICMP Source Quench message
 */

void send_quench (paktype *pak)
{
    iphdrtype *ip;
    icmptype *icmp;
    idbtype *idb;
    paktype *newp;
    ulong size;
    ipaddrtype source, destination;

    /*
     * Don't want to attempt to encapsulate a new IP packet at interrupt
     * level (due to need to look at main IP routing table which might not
     * be in a consistent state).
     */
    if (onintstack())
	return;
    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Don't send quenchs more than once a second per interface.
     */ 
    idb = pak->if_input;
    if (!idb || CLOCK_IN_INTERVAL(idb->ip_quench_timer, idb->ip_quench_interval))
	return;
    icmp = icmp_sendsetup(pak, &newp, &size);
    if (!icmp)
	return;
    /*
     * send_quench() could be called from the fastswitching path when
     * both the TX ring and hold-queue for the outgoing interface
     * are full. Make sure always use longword reference when accessing
     * the source and destination addresses in IP header since packet
     * alignment is not performed on the fastswitching path.
     */ 
    source = GETLONG(&ip->srcadr);
    destination = GETLONG(&ip->dstadr);
    icmp->type = ICMP_QUENCH;
    icmp->identifier = 0;
    icmp->sequence = 0;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    icmp_traffic.quenchsent++;
    if (icmppkt_debug)
	buginf("\nICMP: source quench sent to %i (dest was %i)",
				 n2hl(source), n2hl(destination));
    ipwrite(newp,size,ICMP_PROT,pak->if_input->ip_address,n2hl(source),
	NULL, NULL, &ipdefaultparams);
    GET_TIMESTAMP(idb->ip_quench_timer);
}

/*
 * send_timestampreply
 * Send a Time Stamp Reply.  Use real UTC time unless the clock isn't set.
 * Note that we are reusing a packet that came in.  It may have
 * options that we wish to preserve.  Beware of the security
 * hidden option kludge (IPHEADERBYTES).
 */

static void send_timestampreply (paktype *pak, icmptype *icmp, int size)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    icmp->type = ICMP_TIMESTAMPREPLY;
    icmp->data[1] = clock_icmp_time();	/* receive timestamp */
    icmp->data[2] = icmp->data[1];	/* transmit timestamp */
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size);
    if (icmppkt_debug)
	buginf("\nICMP: sending time stamp reply to %i", n2hl(ip->srcadr));
    icmp_traffic.timestrepsent++;
    ipwrite(pak, 0, ICMP_PROT,ip->dstadr,n2hl(ip->srcadr),
	NULL, NULL, &ipdefaultparams);
}

/*
 * send_inforeply
 * Send an Information Reply.
 *
 * We are reusing an input packet.  It contains options and other
 * things that we wish to keep around.  Beware of the hidden
 * security option kludge.  Ipwrite takes care of it.
 */

static void send_inforeply (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;
    ipaddrtype target;
    idbtype *idb;
    hwidbtype *hwidb;
    uchar *source;
    ieee_addrs *addrs;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    hwidb = idb->hwptr;
    target = (idb->ip_address & idb->ip_nets_mask) |
                 (n2hl(ip->srcadr) & ~idb->ip_nets_mask);

    /*
     * If we don't already have an arp entry for this guy, use the hardware
     * address on the incoming packet -- the remote machine can't arp until
     * we tell him what his IP address is!  We only do this for the interfaces
     * and encapsulation types we know use ARP for IP support.  Feel free to
     * add your own definitions later.
     */

    if (!arp_table_lookup(LINK_IP, target, idb)) {
	if (hwidb->status & IDB_IEEEMASK) {
	    addrs = (ieee_addrs *)pak->addr_start;
	    source = addrs->saddr;
	} else
	    source = NULL;
	if (source)
	    ip_arp_table_add(idb, target, source, hwidb->enctype, ARP_DYNAMIC);
    }

    icmp->type = ICMP_INFOREPLY;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp,ICMPHEADERBYTES);
    if (icmppkt_debug)
	buginf("\nICMP: sending info reply to %i", n2hl(ip->srcadr));
    icmp_traffic.inforepsent++;
    ipwrite(pak,0,ICMP_PROT,idb->ip_address,target,NULL, NULL, &ipdefaultparams);
}

/*
 * rcv_irdpadvert
 * Process an IRDP advertisement (just queue it up for gdp client module)
 */

static void rcv_irdpadvert (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (icmppkt_debug)
	buginf("\nICMP: rdp advert rcvd type %d, code %d, from %i",
	       icmp->type, icmp->code, ip->srcadr);
    reg_invoke_ip_gdpclient_enqueue(pak);
}

/*
 * rcv_irdpsolicit
 * Process an IRDP solicitation (send a report if it is valid)
 */

static void rcv_irdpsolicit (paktype *pak, icmptype *icmp)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (icmppkt_debug)
        buginf("\nICMP: rdp solicit rcvd type %d, code %d, from %i",
	       icmp->type, icmp->code, n2hl(ip->srcadr));
    reg_invoke_iprouting_irdp_send_report(pak->if_input, ip->srcadr);
    retbuffer(pak);
}

/*
 * send_irdpsolicit
 * Send a solicitation message out the given interface for the
 * given destination
 */

void send_irdpsolicit (idbtype *idb)
{
    iphdrtype *ip;
    paktype *pak;
    icmptype *icmp;
    ipaddrtype dummy, destination;
    int bytes;

    if (gdp_irdp_multicast)
	destination = ALLROUTERS;
    else
	destination = -1L;
    icmp_traffic.rdpsolsent++;

    /*
     * Build an ICMP packet from scratch.  Do not use IP sockets.
     */
    bytes = IPHEADERBYTES(NOPT) + ICMPHEADERBYTES;
    pak = getbuffer(bytes);
    if (!pak)
	return;

    ip = (iphdrtype *)ipheadstart(pak);
	
    icmp = (icmptype *) ((char *)ip + IPHEADERBYTES(NOPT));
    icmp->type = ICMP_RDPSOLICIT;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->identifier = 0;
    icmp->sequence = 0;
    
    icmp->checksum = ipcrc((ushort *)icmp, ICMPHEADERBYTES);

    if (!idb) {
	dummy = destination;
	idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_USE_PROXY);
	if (!idb) {
	    (void) retbuffer(pak);
	    return;
	}
    }

    if (icmppkt_debug)
      buginf("\nICMP: src=%i, dst=%i, irdp solicitation sent",
	     idb->ip_address, destination);

    ipwrite(pak, bytes, ICMP_PROT, idb->ip_address, destination, idb, NULL,
		&ipdefaultparams);
}

/* 
 * icmp_show_traffic
 * ICMP statistics
 */

void icmp_show_traffic (void)
{
    printf("\n\nICMP statistics:");
    printf("\n  Rcvd: %lu format errors, %lu checksum errors, "
	        "%lu redirects, %lu unreachable",
	        icmp_traffic.formaterr,   icmp_traffic.checksumerr,
	        icmp_traffic.redirectrcv, icmp_traffic.unreachrcv);
    printf("\n        %lu echo, %lu echo reply, %lu mask requests, "
	        "%lu mask replies, %lu quench",
 		icmp_traffic.echorcv,    icmp_traffic.echoreprcv,
	        icmp_traffic.maskreqrcv, icmp_traffic.maskreprcv,
	        icmp_traffic.quenchrcv);
    printf("\n        %lu parameter, %lu timestamp, %lu info request, %lu other",
		icmp_traffic.paramrcv,   icmp_traffic.timestrcv,
		icmp_traffic.inforeqrcv, icmp_traffic.otherrcv);
    printf("\n        %lu irdp solicitations, %lu irdp advertisements",
	        icmp_traffic.rdpsolrcv, icmp_traffic.rdpadvrcv);
    printf("\n  Sent: %lu redirects, %lu unreachable, %lu echo, %lu echo reply",
		icmp_traffic.redirectsent, icmp_traffic.unreachsent,
 		icmp_traffic.echosent,     icmp_traffic.echorepsent);
    printf("\n        %lu mask requests, %lu mask replies, %lu quench, %lu timestamp",
		icmp_traffic.maskreqsent, icmp_traffic.maskrepsent,
		icmp_traffic.quenchsent,  icmp_traffic.timestrepsent);
    printf("\n        %lu info reply, %lu time exceeded, %lu parameter problem",
		icmp_traffic.inforepsent, icmp_traffic.ttlsent,
		icmp_traffic.paramsent);
    printf("\n        %lu irdp solicitations, %lu irdp advertisements",
	        icmp_traffic.rdpsolsent, icmp_traffic.rdpadvsent);
}

/*
 * icmp_process
 * Routine to handle ICMP input processing
 */

void icmp_process (paktype *pak)

{
    iphdrtype *ip;
    icmptype *icmp;
    int size;

    ip = iphdrtype_start(pak);

    icmp_traffic.inputs++;
    size = ipheadsize(ip);		/* IP header length in bytes */
    icmp = (icmptype *) ipdatastart(pak);
    size = n2h(ip->tl) - size;		/* ICMP length in bytes */
    if (size < ICMPMINHEADERBYTES) {	/* Too small to even consider */
	IP_DISPOSE(icmp, formaterr, pak);/* It'll have to do */
	return;
    }
    if (ipcrc((ushort *)icmp,size)) {
	IP_DISPOSE(icmp, checksumerr, pak);
	return;
    }
    pak->flags &= ~PAK_ALIAS;	/* dont care about dest address */
    switch (icmp->type) {
      case ICMP_ECHOREPLY:
	if (size < ICMP_ECHOREPLYSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.echoreprcv++;
	rcv_echoreply(pak,icmp);
	break;
      case ICMP_UNREACHABLE:
	if (size < ICMP_UNREACHABLESIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.unreachrcv++;
	rcv_unreachable(pak,icmp);
	break;
      case ICMP_QUENCH:
	if (size < ICMP_QUENCHSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.quenchrcv++;
	rcv_quench(pak, icmp);
	break;
      case ICMP_REDIRECT:
	if (size < ICMP_REDIRECTSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.redirectrcv++;
	rcv_redirect(pak,icmp);
	break;
      case ICMP_ECHO:
	icmp_traffic.echorcv++;
	send_echoreply(pak,icmp,size);
	break;
      case ICMP_RDPADVERT:
	icmp_traffic.rdpadvrcv++;
	rcv_irdpadvert(pak, icmp);
	break;
      case ICMP_RDPSOLICIT:
	icmp_traffic.rdpsolrcv++;
	rcv_irdpsolicit(pak, icmp);
	break;
      case ICMP_TIMEEXCEEDED:
	if (size < ICMP_TIMEEXCEEDEDSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.ttlrcv++;
	reg_invoke_ip_trace_rcvexceed(pak, icmp);
	break;
      case ICMP_PARAMETER:
	icmp_traffic.paramrcv++;
	rcv_parameter(pak, icmp);
	break;
      case ICMP_TIMESTAMP:
	if (size < ICMP_TIMESTAMPSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.timestrcv++;
	send_timestampreply(pak, icmp, size);
	break;
      case ICMP_TIMESTAMPREPLY:
	IP_DISPOSE(icmp, timestreprcv, pak);
	break;
      case ICMP_INFOREQUEST:
	icmp_traffic.inforeqrcv++;
	send_inforeply(pak, icmp);
	break;
      case ICMP_INFOREPLY:
	IP_DISPOSE(icmp, inforeprcv, pak);
	break;
      case ICMP_MASKREQUEST:
	if (size < ICMP_MASKREQUESTSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.maskreqrcv++;
	send_maskreply(pak, icmp, size);
	break;
      case ICMP_MASKREPLY:
	if (size < ICMP_MASKREPLYSIZE) {
	    IP_DISPOSE(icmp, formaterr, pak);
	    return;
	}
	icmp_traffic.maskreprcv++;
	rcv_maskreply(pak,icmp);
	break;
      default:
	if (icmppkt_debug)
	    buginf("\nICMP: rcvd type %d, code %d, from %i",
		   icmp->type, icmp->code, n2hl(ip->srcadr));
	IP_DISPOSE(icmp, otherrcv, pak);
	break;
    }
}
