/* $Id: ipsupport.c,v 3.7.22.17 1996/09/06 23:55:11 bew Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipsupport.c,v $
 *------------------------------------------------------------------
 * ipsupport.c -- miscellaneous IP support code
 *
 * 15-September-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipsupport.c,v $
 * Revision 3.7.22.17  1996/09/06  23:55:11  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Move cryptoflags check before the mtu check in ipsendnet(). The
 * right order is to encrypt/decrypt, then fragment.
 *
 * Revision 3.7.22.16  1996/09/04  23:19:29  dawnli
 * CSCdi67411:  RIP Neighbor Statements to Subnet Broadcasts are Failing
 * Branch: California_branch
 *
 * Now, ipwrite() recognizes not only all-1s broadcast, but also
 * direct broadcast.
 *
 * Revision 3.7.22.15  1996/08/28  12:50:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.22.14  1996/08/10  00:34:03  hou
 * CSCdi60803:  ISL encapsulated IP datagrams fragmented erroneously.
 * Branch: California_branch
 * - don't frag ISL packet with less than 1500-byte ip mtu
 *
 * Revision 3.7.22.13  1996/06/20  02:15:22  ahh
 * CSCdi59646:  NAT: dest translation might be needed for local packets
 * Branch: California_branch
 *
 * Revision 3.7.22.12  1996/06/18  01:45:51  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.7.22.11  1996/06/17  22:20:01  cakyol
 * CSCdi60232:  ifdefs in LANE code should be removed
 * Branch: California_branch
 *
 * Revision 3.7.22.10  1996/06/07  00:31:19  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Added check crypto maps for locally-generated packets.
 * Also in fragment() copied the crypto_flags from the original pak
 * to the two new paks.
 *
 * Revision 3.7.22.9  1996/05/13  23:21:18  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.7.22.8  1996/05/10  23:32:26  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.7.22.7  1996/05/09  14:31:05  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.22.6  1996/05/04  01:04:47  wilber
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
 * Revision 3.7.22.5.4.1  1996/04/27  07:04:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.22.5  1996/04/19  15:42:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.7.22.4  1996/04/16  18:53:27  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.7.22.3  1996/04/15  21:16:40  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.22.2.4.1  1996/04/08  01:55:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.22.2  1996/03/28  01:23:24  jenny
 * CSCdi50806:  TCP connection terminates if outgoing interface MTU is
 * decreased
 * Branch: California_branch
 * Added a new CASE registry which IP now uses to notify of transport
 * layer if interface MTU is smaller than packet size and dont fragment
 * bit is set in the packet.
 *
 * Revision 3.7.22.1  1996/03/18  20:17:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.26.1  1996/03/25  02:19:45  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.6.1  1996/03/15  20:50:36  cakyol
 * LANE could NOT self ping an HSRP active IP address before this fix.
 * Now, the ip_sendself attaches the outgoing vc info so that lane
 * classification can be made correctly when it is raw enqueued.
 * Normally this should be the incoming vc info, but in this case,
 * we do NOT have one since packet turns around internally.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/01/24  20:15:08  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/19  00:57:41  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/13  04:21:22  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/11/22  17:47:19  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/17  09:35:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:57:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:05:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/20  18:06:00  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.8  1995/10/19  08:05:23  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.7  1995/10/03  00:09:28  lwei
 * CSCdi41263:  PAK_LOOPBACK messages received when originating multicasts.
 *           -  Put in Dino's patch to prevent locally originated mtrace
 *              messages from looping twice.
 *
 * Revision 2.6  1995/09/21  05:46:28  dino
 * CSCdi36738:  Multicast sender being designated router
 *
 * Revision 2.5  1995/09/20  17:05:09  dino
 * CSCdi36738:  Multicast sender being designated router
 *
 * Revision 2.4  1995/08/19  22:21:59  tli
 * CSCdi36983:  Need policy routing for locally generated packets
 *
 * Revision 2.3  1995/07/12  23:02:20  bcole
 * CSCdi28695: parallel paths on the same subnet cause erroneous IP
 * tfk. forwarding
 *
 * When equal cost load balancing over multiple directly connected
 * interfaces, be sure to locally receive IP packets destined for us,
 * instead of possibly forwarding the packet back out one of the alternate
 * interfaces.
 *
 * Revision 2.2  1995/06/28  23:50:19  myeung
 * CSCdi30267:  router does a tight loop in ip_sendnet when fed a weird
 * sequence
 * - Add ability to detect, drop and log information about the looping
 *   packet
 *
 * Revision 2.1  1995/06/07  21:00:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../wan/serial.h"
#include "../if/fddi.h"
#include "ip.h"
#include "ip_debug.h"
#include "ipaccount.h"
#include "../tcp/tcp.h"
#include "ipoptions.h"
#include "../if/network.h"
#include "logger.h"
#include "../iprouting/route.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../h/ttysrv.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ip/msg_ip.c"		/* Not a typo, see logger.h */
#include "../if/atm.h"
#include "../lane/lane.h"

/*
 * Forward declarations
 */
paktype *fragment(paktype *, boolean, int);

/*
 * Local storage
 */
/* The following could be a data structure, but there would be too
   much wasted space in padding out the characters. */
#define IP_MAX_PROTO_COUNTERS 10
static ulong *ip_counter_pointer[IP_MAX_PROTO_COUNTERS];
static uchar ip_counter_protocol[IP_MAX_PROTO_COUNTERS];

/*
 * ipbuild_head
 *
 * Build an IP header for a packet. This has been extracted from ipwrite, 
 * which contains documentation for this routine. Returns a packet without
 * a checksum.
 */
void ipbuild_head (
    paktype *pak,			/* the packet to send */
    int paklen,				/* 0 if internal, else total len */
    short protocol,			/* proto to use in pak if external */
    ipaddrtype source,			/* source ip address */
    ipaddrtype destination,		/* destination ip address */
    ipoptiontype *options,		/* options to insert if internal */
    ipparamtype *param)			/* default parameters if internal */
{
    ipopt_bassectype *sec_opt;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * external (reused) packet?
     */
    if ((paklen != 0) 
	&& ((pak->ip_flags & PAKOPT_SECSTRIP)== 0)) {
	/*
	 * A truely internal packet.  Build the header then to the
	 * default security insertion dance.
	 */
	ip->version = IPVERSION;	/* IP version number */
	ip->tos = param->tos;		/* type of service */
	ip->tl = paklen;		/* length of packet */
	ip->id = param->ident;		/* packet ID */
	ip->fo = 0;			/* no fragment offset */
	ip->ipreserved = 0;		/* reserved field */
	ip->dontfragment = param->dontfragment; /* may fragment? */
	ip->morefragments = FALSE;	/* no more fragments */
	ip->prot = protocol;		/* protocol type */
	ip->ihl = btol(IPHEADERBYTES(options)); /* header length */
	if (options)
	    bcopy(options->ipopts,
		((uchar *)ipheadstart(pak)+MINIPHEADERBYTES),
		options->paddedlength);
	else if (ip_secopt_bytes) {
	    /*
	     * User is not using options but security is enabled so
	     * we have space in the header to insert security.  Because
	     * of the security kludge (IPHEADERBYTES).
	     */
	    sec_opt = (ipopt_bassectype *)((char *)ip + MINIPHEADERBYTES);
	    sec_opt->code = IPOPT_BASSEC;
	    sec_opt->length = SECURITY_SIZE;
	    sec_opt->classification = SECINSERT;
	    sec_opt->authority[0] = 0;
	}
    } else {
	/*
	 *  A reused packet.  Reverse the option list if any.
	 */
        if (ip->ihl > btol(MINIPHEADERBYTES)) /* reverse source route */
	    ipopt_reversepak(pak);		/* and others as needed */
    }
    ip->ttl = param->ttl;              /* time to live */
    ip->srcadr = source;		/* source address */
    ip->dstadr = destination;
    /*
     * A caller sets the switching flag when it wants this packet
     * to populate the fast-switching cache on the way out.  An
     * example would be DLSw/FST.
     */
    if (!param->switching)
	clear_if_input(pak);	/* ensure we are sourcing */
}

/*
 * Check locally-generated packets against a crypto map.  Encryption will be 
 * done later, but the packet will get marked for encryption now.
 */
static boolean ip_checkcryptomap (paktype *pak)
{
    if (!reg_invoke_crypto_map_check(pak, pak->if_output->crypto_map,
    				     CRYPTO_ENCRYPT, FALSE)) {
        return(FALSE);
    }
    if (crypto_deny_packet(pak)) {
        if (ippkt_debug) {
    	    ipbug(pak, "output crypto map check failed.");
        }
        return(FALSE);
    }
    return(TRUE);
}

/*
 * ipwrite
 * 
 * ipwrite is responsible for sending out internally generated packets
 * out the best interface.
 *
 * It performs the following:
 *
 * 1) If paklen is not 0, build the header, copy in options, and do default
 *    option insertion.
 * 2) Set source and destination addresses
 * 3) compute the checksum
 * 4) preroute option processing
 * 5) If the packet is not a broadcast, route the packet and send it
 * 6) If the packet is a broadcast, then for all appropriate interfaces
 *    do postroute processing and send the packet out.
 *
 * If the paklen parameter is 0, then ipwrite assumes that this is a packet
 * that is being reused.  It won't build the header or any of that stuff
 * but will copy in the passed in source and destination addresses.
 * Default security also won't be inserted because it is assumed that the
 * packet must have it already or it wouldn't be here.  A reused packet
 * will also have source routes and such things turned around before it is
 * checksumed.
 * 
 * For broadcasts (destination -1), 'interface' contains the interface
 * to use.  If it is zero than broadcast on all interfaces.
 *
 * Default security is something of a strange beast.  If there is any
 * security turned on in the system (any interface has security) then
 * the global cell ip_secopt_bytes will contain how big the needed
 * security option needs to be when inserted.  When packets are allocated
 * that do not include an intentional set of options ip_secopt_bytes
 * will automatically be added on to the header bytes.  This does not
 * happen if options are being intentionally used.  It is the caller's
 * responsibility to ensure that the correct code is inserted where
 * the security options should be (SECINSERT).  The macro IPHEADERBYTES
 * has been modified to make all this happen.
 * 
 * One of ipwrite's jobs is to see a default security field is properly
 * initialized when no options are being used.  If option's are used it is
 * the caller's responsibility to ensure that the required security field
 * is where it should be.
 *
 * Encryption policy is checked if there is a crypto map defined on the 
 * outbound interface.
 */


void ipwrite (
    paktype *pak,			/* the packet to send */
    int paklen,				/* 0 if internal, else total len */
    short protocol,			/* proto to use in pak if external */
    ipaddrtype source,			/* source ip address */
    ipaddrtype destination,		/* destination ip address */
    idbtype *interface,			/* xfc to use if broadcast */
    ipoptiontype *options,		/* options to insert if internal */
    ipparamtype *param)			/* default parameters if internal */
{
    iphdrtype *ip;
    idbtype *if_output;
    paktype *newpak;
    boolean drop;
    boolean unicast, subnet_broadcast;

    ip = (iphdrtype *)ipheadstart(pak);

    if (param == (ipparamtype *)NULL)
	param = &ipdefaultparams;

    ipbuild_head(pak, paklen, protocol, source, destination, options, param);
    /*
     * Bump up the id field, so the next datagram using ipdefaultparams
     * have a different datagram id.
     */
    ipdefaultparams.ident++;

    /*
     * Process in a preroute fashion options.  This allows source
     * routes to work for broadcast destinations.
     */
    if (!ipopt_inproc(pak)) {
	datagram_done(pak);
	return;
    }

    /*
     * Perform any necessary policy routing.
     */
    if (reg_invoke_ip_policy_forward(pak)) {
	if (!pak->if_output || !pak->desthost) {
	    datagram_done(pak);
	    return;
	}
	ip->checksum = 0;		/* clear checksum field */
        ip->checksum = ipcrc((ushort *)ip, ltob(ip->ihl));
    	if (pak->if_output && pak->if_output->crypto_map) {
	    /*
	     * If there is a crypto map, check it.
	     */
            if (!ip_checkcryptomap(pak)) {
                if (pak->refcount>1) {
                    pak->cryptoflags = 0;
                    pak->crypto_enc_conn_id = 0;
                    pak->crypto_dec_conn_id = 0;
                }
	        datagram_done(pak);
	        return;
	    }
        }
        ipsendnet(pak, TRUE, param->log);
	return;
    }

    /*
     * If pre-routed use address to determine if we are broadcasting.
     * We recognize -1 or subnet broadcast as the broadcast address.
     */
    if (pak->desthost)
	destination = pak->desthost;

     if (destination == -1L || IPMULTICAST(ip->dstadr)) {
         pak->desthost = destination;
         unicast = subnet_broadcast = FALSE;
     } else {
         if (interface != NULL && ip_broadcast(destination, interface)) {
             pak->desthost = -1L;
             unicast = FALSE;
             subnet_broadcast = TRUE;
         } else {
             unicast = TRUE;
             subnet_broadcast = FALSE;
         }
     }
    /*
     * Check for directed packet.  After routing, make sure we have a valid
     * source address (this might be from an unnumbered interface) and then
     * compute checksum.
     */
     if (unicast) {
	/*
	 * Don't know if we have to translate or not, but may need to try.
	 */
	if (ip_nat_anywhere) {
	    newpak = reg_invoke_translate_before_routing(LINK_IP, pak, &drop);
	    if (newpak) {
		pak = newpak;
		ip = (iphdrtype *)ipheadstart(pak);
		/*
		 * Might not be sending to an alias anymore,
		 * so clear the flag if necessary.
		 */
		if (!ip_aliasaddress(ip->dstadr))
		    pak->flags &= ~PAK_ALIAS;
	    } else if (drop) {
		if (ipnat_debug)
		    buginf("\nNAT: translation failed (L), dropping packet"
			   " s=%i d=%i", ip->srcadr, ip->dstadr);
		datagram_done(pak);
		return;
	    }
	}
	if (!ip_route(pak, 0)) {
	    if (ippkt_debug && param->log)
		ipbug(pak, "unroutable");
	    datagram_done(pak);
	    return;
	}
	if_output = pak->if_output;
	if (ip->srcadr == 0L) {
	    if (if_output->ip_address)
		ip->srcadr = if_output->ip_address;
	    else if (pak->if_output->ip_unnumbered)
		ip->srcadr = if_output->ip_unnumbered->ip_address;
	}
	ip->checksum = 0;			/* clear checksum field */
        ip->checksum = ipcrc((ushort *)ip, ltob(ip->ihl));
	if (ippkt_debug && param->log)
	    ipbug(pak, "sending");

	/*
	 * If source address is inside and we're headed outside,
	 * we may have to translate.
	 */
	if (if_output->ip_info && if_output->ip_info->nat_enabled &&
	    if_output->ip_info->nat_outside) {
	    newpak = reg_invoke_translate_after_routing(LINK_IP, pak, &drop);
	    if (newpak) {
		pak = newpak;
	    } else if (drop) {
		if (ipnat_debug)
		    buginf("\nNAT: translation failed (L), dropping packet"
			   " s=%i d=%i", ip->srcadr, ip->dstadr);
		datagram_done(pak);
		return;
	    }
	}
	/*
	 * If there is a crypto map, check it.
	 */
	if (pak->if_output && pak->if_output->crypto_map) {
	    if (!ip_checkcryptomap(pak)) {
                if (pak->refcount>1) {
                    pak->cryptoflags = 0;
                    pak->crypto_enc_conn_id = 0;
                    pak->crypto_dec_conn_id = 0;
                }
	    	datagram_done(pak);
	    	return;
	    }
	}
        ipsendnet(pak, TRUE, param->log);
	return;
    }

    /*
     * Broadcast.
     */
    if (interface != NULL) {
	if (!interface->ip_enabled) {
	    datagram_done(pak);
	    return;
	}
	ip->srcadr = source;
	pak->if_output = interface;
	if (ip->srcadr == 0L) {
	    if (pak->if_output->ip_address)
		ip->srcadr = pak->if_output->ip_address;
	    else if (pak->if_output->ip_unnumbered)
		ip->srcadr = pak->if_output->ip_unnumbered->ip_address;
	}
	if (destination == -1L)
	    ip->dstadr = interface->ip_broadcast;
	else
	    ip->dstadr = destination;
	ip->checksum = 0;			/* clear checksum field */
	ip->checksum = ipcrc((ushort *)ipheadstart(pak),ltob(ip->ihl));
	if (!ipopt_postroute(pak, ip->srcadr)) {
	    datagram_done(pak);
	    return;
	}
	if (ippkt_debug && param->log)
	    ipbug(pak, "sending broad/multicast");

	ipsendnet(pak, TRUE, param->log);
	return;
    } else {
	paktype *copy;
	idbtype *idb = SWIDBQ_HEAD;

	while (idb) {
	    if (idb->ip_address) {
		ip->srcadr = idb->ip_address;
		ip->dstadr = destination == -1 ?
		    idb->ip_broadcast : destination;
	    } else if (idb->ip_unnumbered) {
		ip->srcadr = idb->ip_unnumbered->ip_address;
		ip->dstadr = destination == -1 ?
		    idb->ip_unnumbered->ip_broadcast : destination;
	    } else {
		ip->srcadr = source;
		ip->dstadr = destination == -1 ?
		    idb->ip_broadcast : destination;
	    }
	    ip->checksum = 0;
	    ip->checksum = ipcrc((ushort *)ip,ltob(ip->ihl));
	    pak->if_output = idb;
	    idb = IDB_NEXT(idb);
	    if (idb == NULL)		/* last one, use original */
		copy = pak;
	    else
	       	copy = pak_duplicate(pak);
	    if (copy) {
		if (!copy->if_output->ip_enabled ||
		    !ipopt_postroute(copy, ip->srcadr)) {
		    datagram_done(copy);
		    continue;
		}
		if (ippkt_debug && param->log)
		    ipbug(copy, "sending broad/multicast");
		ipsendnet(copy, TRUE, param->log);
	    }
	}
    }
}

/*
 * msg_looppak
 * Generate error message about looping packet.
 */
static void msg_looppak (paktype *pak) {
    iphdrtype *ip = (iphdrtype *) ipheadstart(pak);
    ushort    i, optionslen, sport, dport, hl;
    uchar     *ptr, *options, buf[MAXIPOPTIONBYTES * 4];
    udptype   *udp;

    /*
     * Get IP options
     */
    hl = ip->ihl << 2;
    optionslen = hl - MINIPHEADERBYTES;	
    ptr = buf;
    options = (uchar *)ip + MINIPHEADERBYTES;
    for (i = 0; i < optionslen; i++) {
	if (i && ((i % 16) == 0))
	    ptr += sprintf(ptr, "\n        ");
	ptr += sprintf(ptr, "%02x ", *options++);
    }
    if (!optionslen)
	ptr += sprintf(ptr, "none");
    *ptr = '\0';

    /*
     * Get source and destination port for UDP and TCP
     */
    switch (ip->prot) {
        case TCP_PROT:
        case UDP_PROT:
	    udp = (udptype *)ipdatastart(pak); 
	    sport = udp->sport;
	    dport = udp->dport;
	    break;
        default:
	    sport = 0;
	    dport = 0;
    }

    errmsg(&msgsym(LOOPPAK, IP), ip->srcadr, ip->dstadr, 
	   hl, ip->tl, ip->prot, sport, dport, 
	   pak->if_input ? pak->if_input->namestring : "local",
	   pak->desthost, 
	   pak->if_output ? pak->if_output->namestring : "local",
	   buf);
}


/*
 * ip_sendself
 * Send a copy of a datagram to ourself and then dispose of it.
 * We send the copy since the datagram itself may be in a queue
 * elsewhere in the system. This routine assumes that the incoming
 * interface should be the same as the outgoing interface.
 */
void ip_sendself (paktype *pak)
{
    paktype   *copy;
    leveltype status;

    if (pak->flags & PAK_SENDSELF) {
	/*
	 * This packet loop !
	 */
	msg_looppak(pak);
	datagram_done(pak);
	return;
    }
    copy = pak_duplicate(pak);
    if (copy) {
	copy->if_input = pak->if_output;
	copy->if_output = NULL;
	copy->flags |= PAK_SENDSELF;
	if (copy->enctype == ET_BRIDGE) {

	    /*
	     * make sure that we remove the bridge code
	     * from the packet before we send it to ourself.
	     */

	    if (pak->if_output->hwptr->status & IDB_SERIAL) {
		copy->datagramstart += HDLC_ENCAPBYTES;
		copy->datagramsize -= HDLC_ENCAPBYTES;
	    } else {				/* Other choice is FDDI */
		copy->datagramstart += FDDI_SNAP_ENCAPBYTES;
		copy->datagramsize  -= FDDI_SNAP_ENCAPBYTES;
	    }
	}
	status = raise_interrupt_level(NETS_DISABLE);

	/*
	** Must set the fast_vc_info if a LANE data packet,
	** to make it look like it came from the "outside".
	** We need this information for categorization during
	** raw_enqueue.  As to the question of which vc info 
	** we should use, make it the same as the outgoing one, 
	** at least for the time being since we dont have a
	** better idea.  Even tho this is not absolutely correct,
	** it works, since we are only interested in the "type"
	** of the vc, rather than the specific vc itself.
	*/
	if (is_atm_lane(copy->if_input)) {
	    vc_info_t *vcinfo = NULL;
	    atm_db_t *atm_db = NULL;

	    atm_db = copy->if_input->hwptr->atm_db;
	    if (atm_db)
		vcinfo = atm_getvc_inline(atm_db, 
			    GETSHORT(copy->datagramstart));
	    if (vcinfo) {
		atm_db->fast_vc_info = vcinfo;
		copy->datagramstart += ATM_AAL5_LANE_ENCAPBYTES;
		copy->datagramsize -= ATM_AAL5_LANE_ENCAPBYTES;
	    }
	}

	raw_enqueue(copy->if_input->hwptr, copy);
	reset_interrupt_level(status);
    }
    datagram_done(pak);
}

/*
 * ipsendnet
 * Route and possibly fragment an IP packet.  Pass to interface driver.
 *
 * Assumes that the following have been set up:
 *
 * pak->desthost:	the internet address of the next hop.  Will be
 * 			-1 if this is a broadcast.
 * pak->if_output	the idb of the interface that this packet will go
 * 			out over.  Should never be NULL.
 *
 * Returns TRUE if successfully sent, FALSE if not (and datagram_done()
 * is called if disposeflag is TRUE).
 */

boolean ipsendnet (paktype *pak, boolean disposeflag, boolean log)
{
    iphdrtype *ip;
    iphdrtype ip_tmp;
    idbtype *idb;
    hwidbtype *hwidb;
    paktype *copy;
    int mtu, i;
    paktype *orig_pak = pak;
    boolean sendself;

    ip = (iphdrtype *)ipheadstart(pak);
again:
    idb = pak->if_output;		/* set up interface pointer */

    if (!idb) {
	errmsg(&msgsym(NULLIDB, SYS), "ipsendnet");
	goto sendfailed;
    }

    if (!pak->desthost) {		/* zero address is a wham bam */
	errmsg(&msgsym(DESTHOST, IP), ip->srcadr, ip->dstadr);
	goto sendfailed;
    }

    if (!interface_up_simple(idb))
	goto sendfailed;

    sendself = FALSE;
    if (IPMULTICAST(ip->dstadr)) {
	pak->flags |= PAK_MULTICAST;
	ip_traffic.mcastout++;

	/*
	 * If we are a PIM DR on this interface, we need to loop the packet
	 * back to ourselves so the IP multicast forwarding engine sends
	 * Register messages to the RP.
	 */
	if (!pak->if_input && 
	    reg_invoke_ipmulticast_loopback(pak)) sendself = TRUE;
    } else if (pak->desthost == -1L || ip_broadcast(ip->dstadr, idb)) {
	ip_traffic.bcastout++;
	pak->flags |= PAK_BROADCAST;
    }

    /*
     * If this interface does not have an address and is not unnumbered,
     * then drop the datagram if it isn't a broadcast.  This allows BootP
     * to work, but disallows other sorts of lossage.
     */
    if (!idb->ip_address && !idb->ip_unnumbered && (pak->desthost != -1L))
	goto sendfailed;

    hwidb = idb->hwptr;
       
    /*
     * Here we check pak->cryptoflags. If the flag is set, pass
     * the packet to crypto engine. After encryption/decryption, the
     * crypto engine will negate the flag and call ipsendnet again to
     * send the packet out.
     * If the packets are coming from or going to vip interface, bypass
     * the crypto engine here since either the packets should have been
     * decrypted or to be encrypted on vip.
     */
    if ((pak->cryptoflags & CRYPTO_DECRYPT) && pak->if_input &&
		!is_vip(pak->if_input->hwptr)) {
        if (pak->if_input->hwptr->crypto_vectors) { 
    	    (*((crypto_vector_t *)pak->if_input->hwptr->crypto_vectors)->ps_vec)
    	        (CRYPTO_DECRYPT, pak, disposeflag, log);
    	    return TRUE;
    	} else {
    	    if (ippkt_debug) 
    	    	ipbug(pak, "crypto failure: no input idb->crypto_vector "
                      "assigned");
            datagram_done(pak);
    	    return TRUE;
    	}
    } else if ((pak->cryptoflags & CRYPTO_ENCRYPT) && !is_vip(hwidb)) { 
        if (hwidb->crypto_vectors) { 
    	    (*((crypto_vector_t *)hwidb->crypto_vectors)->ps_vec)
    	        (CRYPTO_ENCRYPT, pak, disposeflag, log);
    	    return TRUE;
    	} else {
    	    if (ippkt_debug) 
       	        ipbug(pak, "crypto failure: no output idb->crypto_vector "
                           "assigned");
            datagram_done(pak);
    	    return TRUE;
    	}
    } 

    /*
     * Note that the encapsulation routine may block long enough for
     * the original owner of the packet to release that packet.
     * We diddle the share count so that any datagram_done will work.
     */
    mtu = idb->ip_mtu;
    if (ip->tl <= mtu) {			/* need to fragment? */
 	pak->linktype = LINK_IP;

        /*
	 * The lower layer encapsulation may involve compression of the packet.
	 * Information from the original packet, for the purpose of accouting,
	 * should be copied before the packet is passed to the encapsulator.
	 * Using Structure copy.
	 */

        ip_tmp = *ip;

        if (pak_encap(hwidb, pak, ip->tl, (long) pak->desthost)) {
	    /*
	     * Ethernet has the problem of having two different encapsulations
	     * of different lengths on it.  Check and adjust because we
	     * assume that most hosts use ARPA encap.
	     * Here is a special check for ISL packet, which has extra
	     * 26-tye ISL header besides ARPA ethernet. Since the mtu
	     * on FEIP is 1540, we don't do fragmentation to ISL
	     * packet if its datagramsize is less than 1540 byte.
	     */
	    if (pak->datagramsize > idb->ip_mtu + ETHER_ARPA_ENCAPBYTES) {
		if ((hwidb->status & IDB_ETHER) && 
                    (pak->enctype != ET_ISL)) {	
		    mtu = idb->ip_mtu - ENCAPBYTES;     /* make small enough */
		    goto fragit;
		}
	    }

	    /*
	     * Count the packets we are generating.
	     * If IP accounting is enabled on output interface, count
	     * transit packets and bytes.
	     */	
	    if (!pak->if_input) {
		ip_traffic.outputs++;
		switch (ip_tmp.prot) {
		  case ICMP_PROT: icmp_traffic.outputs++; break;
		  case TCP_PROT: tcp_traffic.outputs++; break;
		  case UDP_PROT: udp_traffic.outputs++; break;
		  default:
		    for (i = 0; i < IP_MAX_PROTO_COUNTERS; i++) {
			if (ip_counter_protocol[i] == 0) {
			    break;
			}
			if (ip_counter_protocol[i] == ip_tmp.prot) {
			    (*ip_counter_pointer[i])++;
			    break;
			}
		    }
		}
	    } else if (idb->ip_output_acctg)
		ip_accumulate_acctg(&ip_tmp, NULL);

	    pak->acct_proto = ACCT_PROTO_IP;

	    /*
	     * If packet is for an alias, bypass the hardware.
	     * If packet is addressed to outgoing interface, set PAK_HEARSELF
	     * if interface can hearself (MCI, 3MB, Serial), otherwise
	     * bypass the hardware.  If packet is really for someone else
	     * or the hardware can hear itself transmit, pass to hardware.
	     *
	     * If packet was already looped once, don't loop again.
	     */

	    if ((pak->flags & PAK_ALIAS) || sendself)
		ip_sendself(pak);
	    else if (ip_ifaddress(idb, pak->desthost)) { 
		if (hwidb->status & IDB_HEARSELF) {
		    pak->flags |= PAK_HEARSELF;
		    datagram_out(pak);
	        } else
		    ip_sendself(pak);
	    } else
		datagram_out(pak);
	    if ((ip_tmp.morefragments == 0) && (ip_tmp.fo != 0))
		ip_traffic.fragged++;
	    /*
	     * if we've now made it through a complete fragment chain,
	     * everything's ok, so destroy the original packet
	     */
	    if (pak != orig_pak)
	        datagram_done(orig_pak);
	    return(TRUE);
	}

	/*
	 * Encapsulation failed.
	 */

	ip_traffic.encapsfailed++;	/* encapsulation failed */
	if (ippkt_debug && log)		/* for gateway tracing function */
	    ipbug(pak, "encapsulation failed");
	goto sendfailed;
    }

fragit:
    /*
     * The packet is larger than the interfaces mtu.  We must frag it.
     */
    if (ip->dontfragment == FALSE) {	/* are we allowed to fragment? */
	copy = fragment(pak, log, mtu);	/* yes, do so */
	if (copy) {
	    /*
	     * don't destroy the original packet, we may need leave it
	     * intact in case any fragment fails so that our caller
	     * can deal with it.
	     */
	    if (pak != orig_pak)
	        datagram_done(pak);
	    pak = copy;
	    ip = (iphdrtype *)ipheadstart(pak);
	    goto again;
	} else
	    ip_traffic.fragfailed++;
    } else {
	/*
	 * If this is a router generated packet, notify transport protocol
	 * of the interface MTU change.
	 */
	if (!pak->if_input)
	    reg_invoke_ip_pak_toobig(ip->prot, idb);
	else
	    send_unreachable(pak, ICMP_UNRFRAG);
	ip_traffic.fragfailed++;	/* no, count a failure */
    }

    /*
     * Come here when something went wrong.  Look at the disposeflag value
     * to determine whether we want to pass the problem up (FALSE return)
     * or whether we want to fake success (TRUE return).  Some forms of
     * encapsulation failure (e.g. directed broadcasts) need passing up
     * to the IP forwarding code.
     */
sendfailed:
    if (pak != orig_pak)		/* destroy any fragment */
        datagram_done(pak);
    if (disposeflag)
	datagram_done(orig_pak);
    return(FALSE);
}

/*
 * ip_route
 *
 * Given a packet, find first hop IP host.  It performs the following
 * actions:
 *
 * 1) If no options computes first hop dependent on dstadr in packet.
 * 2) If options uses information left over from ipopt_inproc to compute
 *    the first hop possibly using a source route.  This info is in
 *    pak->desthost.
 * 3) if the next hop is computable (we can route it) the if_output
 *    field of the packet is set to the interface to use and pak->desthost
 *    is set to what we think the next hop destination is.
 * 4) If needed perform any post routing processing.
 *
 * ip_route will return true or false which denotes whether a route could
 * be found or not.
 *
 * ip_route assumes that the input option processor has been called if
 * needed for this packet some time earlier.  Normally this happens on
 * the way into the system (ip_input).  It is this processor that will
 * set the various option flags such as PAKOPT_HASSSR.
 *
 * If something goes wrong in option processing (such as duplicate options
 * or possibly a security failure) ip_route will return false but the
 * PAK_ABORT bit in the packet flags (pak->flags) will be set.
 *
 * The cell desthost in the packet header is modified to be the destination
 * of where the packet is headed.  This modification only happens if
 * ip_route returns TRUE.  (Note that desthost will probably have
 * been changed if ip_route returns false and there is a PAK_ABORT due
 * to a security failure).
 */

boolean ip_route (paktype *pak, ulong flags)
{
    iphdrtype *ip;
    ipaddrtype nexthop;
    idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Ask IP router, answer in pak->desthost.  This must be an if
     * on options or no options and NOT whether there is a source route.
     * If options we must do other processing regardless of whether we
     * have source routes.  We do not want to mesh the two halves of
     * the if for performance reasons.
     */
    if ((ip->ihl == btol(MINIPHEADERBYTES)) && (!ip_secopt_bytes)) {
	/*
	 * Easy case with no options and no security
	 */
	pak->desthost = ip->dstadr;	     /* set destination */
	if (pak->if_output)		       /* routing already set up? */
	    return(TRUE);		       /* yes, return now */
	flags |= IP_PATH_USE_PROXY|IP_PATH_ROUNDROBIN;
	pak->if_output = 
	    reg_invoke_ip_first_hop(&pak->desthost, pak->desthost, flags);
	if (pak->if_output)
	    return(TRUE);
	return(FALSE);			       /* no route to that address */
    } else {
	/*
	 * If we have source routing pak->desthost will have the next
	 * hop from the source route.  Otherwise it will be zero and
	 * we should just use the dstadr from the packet.
	 */
	nexthop = pak->desthost;
	if (!nexthop)			/* if 0 then not source routing */
	    pak->desthost = ip->dstadr;
	if (pak->if_output)		/* routing already set up? */
	    idb = pak->if_output;	/* already routed */
	else {				/* not routed, go find one */
	    flags |= IP_PATH_USE_PROXY|IP_PATH_ROUNDROBIN;
	    idb =
		reg_invoke_ip_first_hop(&pak->desthost, pak->desthost, flags);
	    if (!idb)
		return(FALSE);		/* if nobody home, punt */
	    if ((pak->ip_flags & PAKOPT_HASSSR) &&
		    (pak->desthost != nexthop)) { /* strict impossible */
		pak->flags |= PAK_ABORT;	  /* dump the white trash */
		send_unreachable(pak, ICMP_UNRSRFAIL);
		ip_traffic.badoption++;
		return(FALSE);
	    }
	    pak->if_output = idb;	/* set interface pointer in packet */
	}
	if ((pak->ip_flags &
	     (PAKOPT_HASROUTE|PAKOPT_HASTSTMP|PAKOPT_HASSEC)) ||
	    (ip_secopt_bytes)) {

	    /*
	     * Do any output level IP option processing  necessary
	     * (insert our address into routes, add timestamps, etc),
	     * and then recompute the checksum.  Also check outgoing
	     * security to see if it is allowed.
	     */
	    if (!ipopt_postroute(pak, idb->ip_address)) {
		pak->flags |= PAK_ABORT;       /* nail the bugger */
		return(FALSE);
	    }
	}
	return(TRUE);
    }
}

/*
 * fragment
 * Does packet fragmentation and sends the fragments to the physical
 * network.  Fragmentation is performed by breaking the packet into two
 * packets: the front packet is of maximum allowable size; the second
 * packet is iteritively fed to ipsendnet again.  The original packet is
 * not given to the driver since we have no way of locking a packet twice,
 * that is, once at process level and once here.
 */

paktype *fragment (paktype *old, boolean log, int mtu)
{
    iphdrtype *old_ip;
    paktype *new1, *new2;
    iphdrtype *new1_ip, *new2_ip;
    int len, hdrlen, nfb;

    old_ip = (iphdrtype *)ipheadstart(old);

    /* 
     * Compute header size in bytes and determine number of fragment blocks
     * we can fit into a maximum size packet.
     * See if we have enough storage to do fragmenting.
     */
    hdrlen = ipheadsize(old_ip);       		/* count of header bytes */
    nfb = (mtu - hdrlen) / 8;			/* frag blocks in packet */
    len = hdrlen + nfb * 8;			/* length of first frag */

    new1 = getbuffer(len);		        /* buffer for first frag */
    if (!new1)
	return(NULL);
    new1_ip = (iphdrtype *)ipheadstart(new1);

    new2 = getbuffer(old_ip->tl - nfb*8);
    if (!new2) {
	retbuffer(new1);
	return(NULL);
    }
    new2_ip = (iphdrtype *)ipheadstart(new2);

    /*
     * This overcounts the number of packets fragmented due to the
     * recursive algorithm used below.
     */
    ip_traffic.fragcount += 2;			/* count fragments */
    ip_traffic.fragged++;			/* count packets */

    /*
     * Copy first part of original into new1 buffer.
     * Send first new buffer to network driver.
     */
    bcopy((char *)old_ip, (char *)new1_ip, len);
    new1_ip->morefragments = 1;
    new1_ip->tl = h2n(len);
    new1_ip->checksum = 0;
    new1_ip->checksum = ipcrc((ushort *)new1_ip,hdrlen);
    new1->flags |= (old->flags & ~PAK_INPUTQ); /* Copy old flags, except
						  for input queue flag,
						  since we did not use
						  input_getbuffer() */
    new1->cryptoflags = old->cryptoflags;       /* Copy old cryptoflags */
    new1->crypto_enc_conn_id = old->crypto_enc_conn_id; /* and conn_id's */
    new1->crypto_dec_conn_id = old->crypto_dec_conn_id;
    new1->if_input = old->if_input;
    new1->if_output = old->if_output;		/* copy idb pointer */
    new1->desthost = old->desthost;
    new1->linktype = old->linktype;
    if (ippkt_debug && log)
	ipbug(new1, "sending fragment");
    if (!ipsendnet(new1, TRUE, log)) {
	retbuffer(new2);
	return(NULL);				/* fragment failed, so fail */
    }

    /*
     * Copy header into new2 buffer.  Update various options.
     */
    bcopy((char *)old_ip, (char *)new2_ip, hdrlen);  /* copy header */
    new2_ip->tl = h2n(old_ip->tl - nfb * 8);	  /* set new2 length */
    new2_ip->fo = h2n(old_ip->fo + nfb);		  /* set frag. offset */
    new2_ip->morefragments = old_ip->morefragments;  /* copy frag flag */
    if (hdrlen > MINIPHEADERBYTES)		  /* delete any IP options not*/
	ipopt_fixfrag(new2,hdrlen);		  /* supposed to be in frags */
    new2_ip->checksum = 0;			  /* clear checksum */
    new2_ip->checksum = ipcrc((ushort *)new2_ip, hdrlen);

    /*
     * Copy remaining data into new2 buffer.  Copy relevant data fields.
     * Recursively handle back fragment.
     */
    bcopy(((char *)old_ip + len),
	  ((char *)new2_ip + hdrlen),
	  new2_ip->tl - hdrlen);
    new2->flags |= (old->flags & ~PAK_INPUTQ); /* Copy old flags, except
	        				  for input queue flag,
						  since we did not use
						  input_getbuffer() */
    new2->cryptoflags = old->cryptoflags;       /* Copy old cryptoflags */
    new2->crypto_enc_conn_id = old->crypto_enc_conn_id; /* and conn id's */
    new2->crypto_dec_conn_id = old->crypto_dec_conn_id;
    new2->if_input = old->if_input;
    new2->if_output = old->if_output;		/* copy idb pointer */
    new2->desthost = old->desthost;
    new2->linktype = old->linktype;
    return(new2);
}

#if 0
/*
 * Example code.  We use assembly language routines on most platforms
 */

/*
 * ipcrc
 * Return CRC checksum for the specified chunk of data
 */

ushort ipcrc (ushort *p, int count)
{
    uchar *ptr = (uchar *) p;		/* damn prototypes */
    int crc;
    ushort i, *data;

    count = (count+1) >> 1;
    data = (ushort *) ptr;
    crc = 0;
    for (i = 0; i < count; i++)
        crc += *data++;		/* 2's complement add the next header word*/
    /* Add once to get first overflow */
    crc = (crc & 0xFFFF) + (crc >> 16);
    /* Add again to catch overflows caused by above */
    crc += crc >> 16;
    i = (short) crc;
    return (~i);
}
#endif


#if defined(UNIX)

/*
 * ipttl
 * Decrement IP TTL field and fixup checksum.
 * Returns TRUE if TTL exceeded, FALSE otherwise
 */

#define TTL_OFFSET  8
#define IHL_OFFSET  0
#define CKS_OFFSET  10

boolean ipttl (uchar *head)
{
    uchar ttl;
    uint temp;
    ushort *temp_csum;

    ttl = head[TTL_OFFSET];
    if (ttl <= 1)
	return(TRUE);
    ttl--;
    head[TTL_OFFSET] = ttl;
    temp_csum = (ushort *)&head[CKS_OFFSET];
    temp = *temp_csum;
    temp += 0x100;
    temp = (temp & 0xFFFF) + (temp >> 16);
    temp += temp >> 16;
    *temp_csum =(ushort)temp;

    return(FALSE);
}

#endif /* defined(UNIX) */


#if 0
/*
 * ip_print
 * Print IP header information on standard output.  For debugging purposes.
 */

void ip_print (paktype *pak)
{
    iphdrtype *ip;
    udptype *udp;
    tcptype *tp;

    ip = (iphdrtype *)ipheadstart(pak);

    buginf("\nIP:  Ver=%d, IHL=%d, TOS=%d, len=%d, ID=%d, res=%d, dont=%d, more=%d",
		ip->version, ip->ihl, ip->tos, ip->tl, ip->id,
		ip->ipreserved, ip->dontfragment, ip->morefragments);
    buginf("\n     FO=%d, TTL=%d, prot=%d, check=%d, src=%i, dst=%i",
		getfo(pak), ip->ttl, ip->prot,
		ip->checksum, ip->srcadr, ip->dstadr);

    switch (pak->prot) {
	case UDP_PROT:
	    udp = (udptype *) ipdatastart(pak);
	    buginf("\nUDP: sport=%d, dport=%d, check=%d, len=%d, data0=%u",
			udp->sport, udp->dport, udp->checksum, udp->length,
			udp->udpdata[0]);
	    break;
	case TCP_PROT:
	    tp = (tcptype *) ipdatastart(pak);
	    buginf ("\nTCP: sport=%d, dport=%d, seq=%lu ack=%lu, bits=",
		    tp->sourceport, tp->destinationport,
		    tp->sequencenumber, tp->acknowledgementnumber);
	    if (tp->urg)
		buginf("URG ");
	    if (tp->ack)
		buginf("ACK ");
	    if (tp->psh)
		buginf("PSH ");
	    if (tp->rst)
		buginf("RST ");
	    if (tp->syn)
		buginf("SYN ");
	    if (tp->fin)
		buginf("FIN ");
	    break;
	default:
	   break;
    }
}
#endif

/*
 * ip_printf
 * I/i prints a dotted Internet address.  Takes an integer argument.
 */

int ip_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
	       boolean rjust, int width, int size, int hash, int precision)
{
    ulong n;
    int length, d, charcount;
    uchar digits[16];

    charcount = 0;
    n = va_arg(*argp, ipaddrtype);
    length = sprintf(digits, "%d.%d.%d.%d",
		     (n >> 24), ((n & 077600000) >> 16),
		     ((n & 0177400) >> 8), (n & 0377));

    if (rjust && width != -1 && (length < width))
	for (d = 0; d < (width - length); d++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	    charcount++;
	}

    if (*cp) {
	strcpy(*cp, digits);
	*cp += length;
    } else
	_ttyprintf(tty, "%s", digits);
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/*
 * ip_mask_printf
 * M/m Prints an IP netmask in user selected format.  Takes an integer
 * argument.  Normal usage printf("%i%m", address, mask).  Use %#m if the
 * mask is not adjacent to an address.  DO NOT USE THIS IN NV GENERATION
 * YET.
 */

int ip_mask_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
		    boolean rjust, int width, int size, int hash,
		    int precision)
{
    ulong n;
    int length, d, charcount, format;
    uchar digits[17];

    charcount = 0;
    n = va_arg(*argp, ipaddrtype);

    length = 0;
    if (tty && tty != NULLTTY && tty != CONTTY)
	format = tty->tty_ip_netmask_format;
    else
	format = console->tty_ip_netmask_format;
    switch (format) {
    case MASK_DOTTED_DECIMAL:
	length = sprintf(digits, " %d.%d.%d.%d",
			 (n >> 24), ((n & 0x00ff0000) >> 16),
			 ((n & 0xff00) >> 8), (n & 0xff));
	break;
    case MASK_HEXADECIMAL:
	length = sprintf(digits, " %#0x", n);
	break;
    case MASK_BITCOUNT:
	if (mask_contiguous(n)) 
	    length = sprintf(digits, hash ? " /%d" : "/%d", ip_bitsinmask(n));
	else
	    length = sprintf(digits, " %#0x", n);
	break;
    }

    if (rjust && width != -1 && (length < width))
	for (d = 0; d < (width - length); d++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	    charcount++;
	}

    if (*cp) {
	strcpy(*cp, digits);
	*cp += length;
    } else
	_ttyprintf(tty, "%s", digits);
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/*
 * ip_reply_srcadr
 * Determine a reasonable IP source address. If we can't then
 * return NULL
 */

ipaddrtype ip_reply_srcadr (paktype *pak)
{
    iphdrtype *ip;
    register ipaddrtype newsrc;
    register idbtype *idb;
    uchar aliases_checked = 0;

    ip = iphdrtype_start(pak);

    /*
     * Swap IP source and destination with care.  If a broadcast was
     * received, use our input interface address as new source.
     * Otherwise, use old destination address as new source address.
     * Do not reply if the interface corresponding the new source address
     * is not in an up state.  This allows others to check the state of
     * a gateway's interfaces without explicitly transiting the gateway.
     */
    idb = pak->if_input;
    if (idb == NULL)
	return(ip->dstadr);
    newsrc = 0L;
    do {
	if (!idb->ip_enabled)
	    goto next;
	/*
	 * This code will allow a strangeness to happen.  If we receive
	 * a packet on one interface with a dstadr of the broadcast
	 * address (as set in the idb) we will use that new interfaces
	 * ip_address as the source of the reply rather than the interfaces
	 * that the packet originally came in on.  Who cares.
	 */
	if (interface_up(idb)) {
	    if (ip_broadcast(h2nl(ip->dstadr), idb) ||
		ip_multicast(h2nl(ip->dstadr), idb)) {
		newsrc = h2nl(pak->if_input->ip_address);
		break;
	    }
	    if (ip_ifaddress(idb, ip->dstadr)) {
		newsrc = ip->dstadr;
		break;
	    }
	}
	/*
	 * Check if destined toward an IP alias.  We used to assume
	 * that the interface is up and running but this is now
	 * explicitly checked for later.  idb will be pak->if_input
	 * if we have an alias.
	 */
	if (aliases_checked == 0) {
	    if (ip_aliasaddress(ip->dstadr)) {
		newsrc = ip->dstadr;
		break;
	    }
	    aliases_checked = 1;
	}
next:
	idb = IDB_NEXT(idb);
	if (idb == NULL)
	    idb = SWIDBQ_HEAD;
    } while (idb != pak->if_input);
    return(newsrc);
}

/*
 * ip_set_proto_outcounter
 *
 * Add a protocol/counter pairing for counting output packets.  This is
 * used by the ipsendnet() routine, which is also called from ipwrite().
 */
void ip_set_proto_outcounter (uchar protocol, ulong *counter)
{
    int i;

    for (i = 0; i < IP_MAX_PROTO_COUNTERS; i++) {
	if (ip_counter_protocol[i] == protocol)
	    return;
	if (ip_counter_protocol[i] == 0) {
	    ip_counter_protocol[i] = protocol;
	    ip_counter_pointer[i] = counter;
	    return;
	}
    }
    errmsg(&msgsym(CNTRFULL, IP), protocol);
}
