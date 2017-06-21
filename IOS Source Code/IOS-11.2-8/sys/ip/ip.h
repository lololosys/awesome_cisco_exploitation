/* $Id: ip.h,v 3.21.4.26 1996/09/13 22:52:26 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip.h,v $
 *------------------------------------------------------------------
 * ip.h -- definitions for the Internet Protocol family
 *
 * August 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip.h,v $
 * Revision 3.21.4.26  1996/09/13  22:52:26  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.21.4.25  1996/09/10  05:31:25  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.21.4.24  1996/09/07  02:16:48  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase
 * the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.21.4.23  1996/08/20  10:24:05  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 3.21.4.22  1996/07/26  22:55:17  chwhite
 * CSCdi63685:  Correct mtrace error handling, TTL behavior
 * Branch: California_branch
 *
 * Revision 3.21.4.21  1996/07/16  08:01:19  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.21.4.20  1996/07/15  17:29:56  vandys
 * CSCdi62939:  L2F does not use IANA assigned port
 * Branch: California_branch
 *
 * Revision 3.21.4.19  1996/07/12  02:26:59  hou
 * CSCdi59825:  Fast Switching is broken in the presence of access-lists
 * Branch: California_branch
 * Use GETSHORT to avoid costly misaligned-access handling.
 *
 * Revision 3.21.4.18  1996/06/18  01:44:43  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.21.4.17  1996/06/03  22:28:36  lmetzger
 * CSCdi59001:  Traceroute is not re-entrant
 * Branch: California_branch
 * Moved all Globals to the tracetype structure.
 *
 * Revision 3.21.4.16  1996/05/23  07:37:17  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.21.4.15  1996/05/17  11:24:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.20.2.6  1996/05/14  06:19:16  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.20.2.5  1996/05/05  23:11:19  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.20.2.4  1996/04/03  14:40:51  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.20.2.3  1996/03/27  23:38:57  schiang
 * CSCdi52816:  Reserve a port number for NCIA2
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.20.2.2  1996/03/22  18:53:41  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.20.2.1  1996/03/17  17:52:04  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.21.4.14  1996/05/14  16:02:25  albright
 * CSCdi57188:  Debug IP Policy Output is misleading / Cosmetic Bug
 * Branch: California_branch
 *
 * Revision 3.21.4.13  1996/05/13  20:09:45  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.21.4.12  1996/05/11  02:30:12  tylin
 * CSCdi56413:  Multicast ping should have a ttl option
 *              Added for extended command only.
 * Branch: California_branch
 *
 * Revision 3.21.4.11  1996/05/06  16:07:40  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.21.4.10  1996/05/04  01:02:01  wilber
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
 * Revision 3.21.4.9  1996/04/27  05:51:12  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.21.4.8  1996/04/24  16:20:17  ahh
 * CSCdi55543:  NAT: modularity problems
 * Fix build failures (Doh!)
 * Branch: California_branch
 *
 * Revision 3.14.2.3  1996/04/26  01:21:00  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.14.2.2  1996/04/17  02:04:06  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.14.2.1  1996/01/31  18:34:14  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 3.21.4.7  1996/04/19  15:41:11  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.10  1996/04/09  20:35:44  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.9  1996/03/28  21:01:12  bew
 * Branch: IosSec_branch
 * Change the name from send_echo_continued() to the more standard
 * send_echo_internal().
 *
 * Revision 2.2.6.8  1996/03/19  08:08:09  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.7  1996/02/21  03:28:58  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.6  1996/01/23  05:42:30  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.5  1995/12/19  03:56:01  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.4  1995/12/17  01:39:40  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.21.4.6  1996/04/17  13:41:35  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.21.4.5  1996/04/16  18:53:04  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.21.4.4  1996/04/15  21:15:41  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.21.4.3  1996/04/04  04:54:57  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.21.4.2  1996/04/03  23:56:38  dino
 * CSCdi52903:  Add IANA assigned PIMv2 proto number and multicast address
 *              to ip.h
 * Branch: California_branch
 *
 * Revision 3.21.4.1  1996/03/18  20:16:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.15.2.3  1996/03/16  06:57:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.15.2.2  1996/03/14  01:06:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.15.2.1  1996/02/20  00:58:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.21  1996/03/11  19:25:53  ahh
 * CSCdi43020:  netbooting from not-directly connected tftp-server fails
 * Allow user to force a boot interface when spurious redirects cause
 * our automagic booting to fail.
 *
 * Revision 3.20  1996/02/26  23:55:17  dino
 * CSCdi49929:  Add constants for IANA assigned group addresses and UDP
 * port to ip.h
 *
 * Revision 3.19  1996/02/20  22:45:57  pst
 * Initial incorporation of distributed directory agent support
 *
 * Revision 3.18  1996/02/02  01:10:39  widmer
 * CSCdi48042:  Change functions in ip/iptrace.c to static
 *
 * Revision 3.17  1996/01/24  20:14:51  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.16  1996/01/23  11:55:38  mdb
 * CSCdi47339:  apply code review comments for CSCdi47065
 *
 * Revision 3.15  1996/01/19  22:29:52  ahh
 * CSCdi47207:  Move TUNNEL_PORT where it can be found
 *
 * Revision 3.14  1996/01/11  02:26:28  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.13  1996/01/10  02:22:08  raj
 * CSCdi40929:  traceroute fails, ping works. port unreachables not sent
 * Allow unreachables within the first 3 minutes and fix timer code to
 * remove ip_icmpokay variable.
 *
 * Revision 3.12  1995/12/17  06:09:43  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.11  1995/12/16  01:09:30  billw
 * CSCdi43681:  Some RADIUS code review comments not incorporated
 *
 * Revision 3.10  1995/12/13  22:57:20  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.9  1995/12/13  04:21:19  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.8  1995/12/08  00:54:22  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.7  1995/12/01  18:37:56  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.6  1995/11/30  22:58:08  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.5  1995/11/22  17:47:03  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.4  1995/11/18  03:28:50  pst
 * CSCdi43467:  unumbered interfaces dont pick up changes based upon
 * numbered change
 *
 * Revision 3.3  1995/11/17  09:33:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:55:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:04:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/20  18:05:50  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.14  1995/10/17  16:14:56  fox
 * CSCdi42286:  ip_init.c fails to compile
 * Remove (newly) redundant function declaration
 *
 * Revision 2.13  1995/10/17  06:26:11  fox
 * CSCdi42286:  ip_init.c fails to compile
 * Add declaration to ip.h
 *
 * Revision 2.12  1995/10/04  18:24:05  dino
 * CSCdi41469:  More Mtrace bugs found during EFT testing.
 *
 * Revision 2.11  1995/09/25  08:31:51  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.10  1995/09/11  23:49:04  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.9  1995/09/09 00:53:14  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.8  1995/09/03  00:11:32  raj
 * CSCdi39164:  WFQ alignment errors on 4500
 * Minor change the comments in order to more fully clarify the situation
 * where the non-word aligned IP header arises.  It should only arise in
 * the case of fastswitching.
 *
 * Revision 2.7  1995/09/02  18:11:13  raj
 * CSCdi39164:  WFQ alignment errors on 4500
 * The IP header is not necessarily word aligned and thus when the compiler
 * outputs a load word ("lw") instruction in order to retrieve parts of the
 * iphdrtype structure, it results in alignment errors on a MIPS.  We avoid
 * this by forcing the compiler to use GETSHORT in order to retrieve the
 * structure fields instead.
 *
 * Revision 2.6  1995/08/29  21:04:19  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers" to turn on/off ECHO, DISCARD,
 * and CHARGEN ports and new knob "ip bootp server" to turn on/off BOOTP
 * service.
 *
 * Revision 2.5  1995/08/09  16:20:27  albright
 * CSCdi35180:  subnet mask of next GW for static routes not checked if
 * default defi
 *
 * Revision 2.4  1995/08/03  07:42:01  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.3  1995/07/12 23:02:16  bcole
 * CSCdi28695: parallel paths on the same subnet cause erroneous IP
 * tfk. forwarding
 *
 * When equal cost load balancing over multiple directly connected
 * interfaces, be sure to locally receive IP packets destined for us,
 * instead of possibly forwarding the packet back out one of the alternate
 * interfaces.
 *
 * Revision 2.2  1995/06/11  23:29:12  dkatz
 * CSCdi35384:  ICMP timestamp packets have bad checksums
 * Calculate the checksum of reply packets over the length of the received
 * request.  Validate packet lengths according to ICMP packet type.  Get
 * rid of ugly pointer math in redirects.
 *
 * Revision 2.1  1995/06/07  20:56:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IP_H__
#define __IP_H__

#include "globs.h"
#include "address.h"
#include "packet.h"

#define IPVERSION 4		/* IP version number */
#define MINIPHEADERBYTES 20	/* minimum ip header bytes (no options) */
#define MAXIPOPTIONBYTES ((15-5)*4) /* maximum number of options allowed */
#define MAXIPHEADERBYTES 60	/* maximum bytes in an IP header */
#define IPV4_NOOPTION	0x45	/* non-optioned ipv4 packet */

#define MAXIPPACKETSIZE 576	/* conservative maximum */
#define	IPPACKETLIMIT	65536	/* maximum possible size of an IP packet */

#define RATIMEOUT 30000		/* thirty seconds */
                                /* size of a reassembly buffer */
#define RABUFFERSIZE	(min(MAXDATAGRAMSIZE, IPPACKETLIMIT))
#define RAMAXBUFFERS	10	/* max# paks being concurrently reassembled */

#define MAXUDPFORWCNT	200
#define IPBROADCASTLIST_CONFIGURED(idb) (idb->ip_directbcast && \
					 idb->ip_directbcast != -1)

/*
 * Flags for use by first_hop/find_path
 */
#define IP_PATH_DEFFLAGS	0x00	/* Default flag setting */
#define IP_PATH_EXACT_MATCH	0x01	/* True if only an explicit next
					   hop match allowed */
#define IP_PATH_USE_IF_GUESS	0x02	/* True to allow heuristical match */
#define IP_PATH_USE_PROXY	0x04	/* True to allow ARPing for next hop */
#define	IP_PATH_RECURSIVE	0x08	/* true if we're on recursive pass */
#define	IP_PATH_ROUNDROBIN	0x10	/* true to perform load splitting */
#define IP_PATH_FAIL_FORUS	0x20	/* make route lookup fail if forus */
#define IP_PATH_SUBDEFAULT      0x40    /* Dont allow missing subnet route to
					   follow default, but defaultflag is
					   still true */

/*
 * Special networks and addresses.
 */
#define IP_LOOPBACK_NET	0x7f000000	/* 127.0.0.0 */
#define	MASK_HOST	0xffffffff	/* 255.255.255.255 */
#define IP_LIMITED_BROADCAST 0xffffffff	/* 255.255.255.255 */
#define IP_BSD_LIMITED_BROADCAST 0	/* 0.0.0.0 */
#define IPADDR_ZERO ((ipaddrtype) 0)	/* for use as a default address */

/*
 * Multicast Address Support
 */
#define IPMULTICAST(addr) (((addr) & 0xf0000000) == 0xe0000000)
#define IPMULTICAST_RSVD(addr) ((addr) >= 0xe0000000 && (addr) <= 0xe00000ff)
#define IPMULTICAST_SCOPED(addr) (((addr) & 0xff000000) == 0xef000000)
#define ALLSYSTEMS      0xe0000001              /* All systems */
#define ALLROUTERS      0xe0000002              /* All routers */
#define DVMRPROUTERS    0xe0000004              /* All DVMRP routers */
#define ALLSPFROUTERS   0xe0000005              /* All OSPF routers */
#define ALLSPFDROUTERS  0xe0000006              /* All OSPF DRs */
#define	ALLRIPROUTERS	0xe0000009		/* All RIP v2 routers */
#define	ALLIGRPROUTERS	0xe000000a		/* All IGRP 2 routers */
#define	RSVP_ENCAP_1	0xe000000e		/* RSVP Encapsulation 1 */
#define	ALLPIMROUTERS	0xe000000d		/* All PIMv2 routers */
#define SDSYSTEMS       0xe0027fff              /* sd */
#define SDRSYSTEMS      0xe0027ffe              /* sdr */
#define MBONE_AUDIO     0xe0020001              /* mbone-audio 224.2.0.1 */
#define CISCO_RP_ANNOUNCE  0xe0000127           /* PIM - 224.0.1.39 */
#define CISCO_RP_DISCOVERY 0xe0000128           /* PIM - 224.0.1.40 */
#define MTRACE_RESPONSE    0xe0000120

#define IPMCAST_DEFAULT 0xe0000000              /* 224.0.0.0 */
#define DEFAULT_MULTICAST_MASK 0xf0000000	/* /4 mask for ip multicast */


#define ICMP_PROT	1	/* protocol number for ICMP */
#define	IGMP_PROT	2	/* protocol for group management */
#define	GGP_PROT	3	/* protocol number for GGP */
#define IPINIP_PROT	4	/* IP in IP encapsulation */
#define TCP_PROT	6	/* protocol number for TCP */
#define EGP_PROT	8	/* protocol number for EGP */
#define IGRP_PROT	9	/* protocol number for IGRP */
#define UDP_PROT	17	/* protocol number for UDP */
#define	HMP_PROT	20	/* protocol number for HMP */
#define	RDP_PROT	27	/* protocol number for RDP */
#define	RSVP_PROT	46	/* protocol number for RSVP */
#define	GRE_PROT	47	/* protocol number for GRE */
#define SDNS0_PROT      53      /* protocol number for SNDS */
#define NHRP_PROT	54	/* protocol number for NHRP */
#define SDNS1_PROT      55      /* protocol number for SDNS */
#define	HELLO_PROT	63	/* protocol number for HELLO */
#define	ND_PROT		77	/* protocol number for ND */
#define EONIP_PROT	80	/* protocol number for CLNS over IP */
#define VINES_PROT	83	/* protocol number for Banyan Vines */
#define	NEWIGRP_PROT	88	/* new protocol number for IGRP */
#define	OSPF_PROT	89	/* protocol number for OSPF */
#define	FST_RSRB_PROT	90	/* protocol number for RSRB */
#define FST_DLSW_PROT   91      /* protocol number for DLSW */
#define	NOSIP_PROT	94	/* KA9Q/NOS compatible IP over IP */
#define MAX_PROT        94      /* maximum protocol number in the
				   above list. Used in creating
				   case registry. */
#define PIM_PROT        103     /* protocol number for PIMv2 */

#define	ECHO_PORT	7	/* Echo service */
#define	DISCARD_PORT	9	/* Discard service */
#define	SYSTAT_PORT	11	/* Systat service */
#define	DAYTIME_PORT	13	/* Daytime service */
#define	NETSTAT_PORT	15	/* Netstat service */
#define	QUOTE_PORT	17	/* Quote service */
#define	CHARGEN_PORT	19	/* Character generator service */
#define FTPD_PORT	20	/* FTP data */
#define	FTP_PORT	21	/* FTP service */
#define TELNET_PORT	23	/* TCP/Telnet */
#define	SMTP_PORT	25	/* SMTP service */
#define TIME_PORT	37	/* UDP time service */
#define IEN116_PORT	42	/* IEN-116 name service */
#define	NICNAME_PORT	43	/* Whois service */
#define	TACACS_PORT	49	/* TACACS service */
#define DOMAIN_PORT	53	/* Domain name service port */
#define DNS_PORT	53	/* Domain name service port */
#define TUNNEL_PORT	57	/* "tunnel" port, eg Stream mode VTY */
#define	BOOTPS_PORT	67	/* BootP server port */
#define	BOOTPC_PORT	68	/* BootP client port */
#define TFTP_PORT	69	/* TFTP initial transfer port */
#define GOPHER_PORT	70	/* Gopher services */
#define	FINGER_PORT	79	/* Finger service */
#define WWW_PORT	80	/* WWW service (HTTP) */
#define	GRE_PORT	91	/* Generic Route Encapsulation */
#define	SUPDUP_PORT	95	/* Supdup service */
#define	HOSTNAME_PORT	101	/* Hostname service */
#define	POP2_PORT	109	/* POP service */
#define POP3_PORT	110	/* POP3 service */
#define SUNRPC_PORT	111	/* Sun RPC port */
#define IDENT_PORT      113     /* Ident protocol port */
#define	NNTP_PORT	119	/* NNTP service */
#define	NTP_PORT	123	/* NTP service */
#define	NBNS_PORT	137	/* NetBios Name service */
#define	NBDS_PORT	138	/* NetBios Datagram service */
#define	SGMP_PORT	153	/* SGMP requests */
#define	SGMP_TRAP_PORT	160	/* SGMP trap requests */
#define	SNMP_PORT	161	/* SNMP requests */
#define	SNMP_TRAP_PORT	162	/* SNMP trap requests */
#define XDMCP_PORT	177	/* XDM control protocol */
#define BGP_PORT	179	/* Border Gateway Protocol */
#define IRC_PORT	194	/* Internet Relay Chat */
#define DNSIX_PORT      195     /* Well-known UDP port for DNSIX */
#define DNSIX_REDI_PORT 196     /* Well-known UDP port for DNSIX */
#define	IPX_PORT	213	/* Novell Compatible IPX over UDP */
#define AURP_PORT	387	/* Apple Update-Based Routing Protocol */
#define MOBILEIP_PORT	434	/* Mobile IP registration */
#define PIM_RP_DISC     496     /* PIM RP announce and discovery messages */
#define EXEC_PORT	512	/* TCP Remote Commands */
#define BIFF_PORT	512	/* BIFF (mail notification) */
#define LOGIN_PORT	513	/* Remote login */
#define	RLOGIN_PORT	513	/* Remote login */
#define WHO_PORT	513	/* rwho service */
#define CMD_PORT	514	/* Command port */
#define RSHELL_PORT     514     /* TCP Remote Shell */
#define	SYSLOG_PORT	514	/* Syslog */
#define PRINTER_PORT	515	/* Printer service */
#define	LPD_PORT	515	/* Printer service */
#define TALK_PORT	517	/* Talk protocol rendezvous */
#define RIP_PORT        520	/* UDP port number for RIP */
#define UUCP_PORT	540	/* UUCP service */
#define KLOGIN_PORT	543	/* Kerberos login */
#define KSHELL_PORT	544	/* Kerberos shell */
#define	CAYMAN_PORT	910	/* Appletalk encapsulated in UDP */
#define IPPORT_RESERVED 1024	/* 4.3BSD (unofficial) */
#define IF_CON_PORT     1500    /* Interface console/debug port */
#define L2F_PORT	1701	/* Layer 2 forwarding port */
#define CRYPTO_KEY_XCHG	1964	/* Crypto public key exchange port */
#define NCIA2_PORT      1973    /* cisco NCIA 2 port */
#define	DIRECTOR_PORT	1974	/* remote director queries */
#define IPC_PORT        1975    /* Interprocess communcations port */
#define BSTUN_P0_PORT   1976    /* cisco BSTUN Priority 0 port  */
#define BSTUN_P1_PORT   1977    /* cisco BSTUN Priority 1 port  */
#define BSTUN_P2_PORT   1978    /* cisco BSTUN Priority 2 port  */
#define BSTUN_P3_PORT   1979    /* cisco BSTUN Priority 3 port  */
#define DLSW_P0_PORT    1980
#define DLSW_P1_PORT    1981
#define DLSW_P2_PORT    1982
#define DLSW_P3_PORT    1983
#define REMIF_PORT	1984	/* remote interfaces over TCP & UDP */
#define STANDBY_PORT	1985	/* cisco hot standby protocol */
#define LICENSE_DAEMON_PORT 1986 /* cisco license management */
#define TR_RSRB_P1_PORT	1987	/* cisco RSRB Priority 1 port */
#define TR_RSRB_P2_PORT	1988	/* cisco RSRB Priority 2 port */
#define TR_RSRB_P3_PORT	1989	/* cisco RSRB Priority 3 port */
#define STUN_P1_PORT	1990	/* cisco STUN Priority 1 port */
#define STUN_P2_PORT	1991	/* cisco STUN Priority 2 port */
#define STUN_P3_PORT	1992	/* cisco STUN Priority 3 port */
#define	SNMP_TCP_PORT	1993	/* SNMP TCP port */
#define STUN_PORT	1994	/* serial tunnel port */
#define STUN_P0_PORT	(STUN_PORT)
#define TR_RSRB_PORT	1996	/* Remote SRB port (Priority # 0) */
#define TR_RSRB_P0_PORT	(TR_RSRB_PORT)
#define GDP_PORT	1997	/* Gateway Discovery Protocol */
#define	X25_SVC_PORT	1998	/* cisco X.25 service */
#define	TCP_ID_PORT	1999	/* cisco identification port */
#define RSVP_PORT_ROUTER 1698   /* UDP port RSVP listens on */
#define RSVP_PORT_HOST  1699    /* UDP port RSVP sends on */
#define IPPORT_USER_RESERVED 5000 /* 4.3BSD (unofficial) */
#define CUSEEME_PORT	7648	/* CMU CuSeeMe */
#define REALAUDIO_PORT	7070	/* RealAudio control */
#define SDR_PORT        9875    /* UDP port sdr listens on */
#define SD_PORT         9876    /* UDP port sd listens on */

#define MAXCMDLEN	1024

/*
 * DON'T USE PORTS 2000 AND ABOVE for router-based TCP connections into the
 * router! These are reserved for various "terminals," be they real or virtual.
 * Don't make the same annoying mistake I did; avoid it at all costs!
 */

#define ALIAS_PORTMASK 0xFFFF
#define ALIAS_FLAGMASK 0xFFFF0000
#define TCPPAD_ALIASPORT 0x20000 /* pseudo-port for tcp to x25 translator */ 
#define MACIP_ALIASPORT	0x30000	 /* pseudo-port for ip->ddp translator */ 

/*
 * IP header structure definition.
 */

#ifdef BIGENDIAN
#  define FOMASK 0x1FFF
#  define IP_FRAG_INFO 3	/* number of shorts into this structure */
#  define IP_FRAG_INFO_BYTE  (IP_FRAG_INFO * sizeof(ushort))
#  define IHLMASK	0x0f00
#  define IHLSHIFT	8	/* number bits to shift to get to LSB */
#  define IP_IHL	0	/* number of shorts into this structure */
#  define IP_IHL_BYTE	(IP_IHL * sizeof(ushort))
#  define IPMACRO \
    unsigned version: 4;	/* IP version number */ \
    unsigned ihl: 4;		/* IP header length */ \
    unsigned tos: 8;		/* type of service */ \
    ushort tl;			/* total length */ \
    ushort id;			/* identifier */ \
    unsigned ipreserved: 1; \
    unsigned dontfragment: 1; \
    unsigned morefragments: 1; \
    unsigned fo: 13;		/* fragment offset */ \
    uchar ttl;			/* time to live */ \
    uchar prot;			/* protocol type */ \
    ushort checksum;		/* checksum */ \
    ipaddrtype srcadr;		/* IP source address */ \
    ipaddrtype dstadr;		/* IP destination address */ 
#else /* LITTLEENDIAN */
#  define morefragments F1.F2.morefrags
#  define dontfragment F1.F2.dontfrag
#  define ipreserved F1.F2.Reserv
#  define FOMASK 0xFF1F
#  define getfo(p) ((p)->F1.fragoff & FOMASK)
#  define fo F1.fragoff
#  define IPMACRO \
    unsigned char ihl: 4;	/* IP header length */ \
    unsigned char version: 4;	/* IP version number */ \
    unsigned tos: 8;		/* type of service */ \
    ushort tl;			/* total length */ \
    ushort id;			/* identifier */ \
    /* Stupid C compiler assigns bit orders wrong... */ \
    union f1 { 		        /* first get the bits right. */ \
	struct f2 { \
	    unsigned char dmy1:5; \
	    unsigned char morefrags:1; \
	    unsigned char dontfrag:1; \
	    unsigned char Reserv:1; \
	    unsigned char dmy2;   \
	} F2; \
    	unsigned short fragoff; \
    } F1; \
    uchar ttl;			/* time to live */ \
    uchar prot;			/* protocol type */ \
    ushort checksum;		/* checksum */ \
    ipaddrtype srcadr;		/* IP source address */ \
    ipaddrtype dstadr;		/* IP destination address */ 
#endif

typedef struct iphdrtype_ {
    IPMACRO			/* IP header */
} iphdrtype;

/*
 * Use these routines to access the "morefragments" and "fo" fields
 * of the IP header in cases where the IP header may not be word
 * aligned.  This happens sometimes when packets are traversing the
 * fastswitching path and thus pak_realign is never called.  In
 * particular, the initial reason for these routines was
 * CSCdi39164 where ip_fair_queue_flow_id was being called on output
 * via fastswitching such that the packet it was looking at was not
 * word aligned.
 *
 * (I thought about using a union inside the structure above, but that
 * would necessitate #defines of things like "morefragements" and
 * "morefragments" just happens to be a field in another structure,
 * not to mention the problem of using up namespace in general.)
 */
static inline unsigned get_ip_morefragments(iphdrtype *ip_ptr) {
    ushort *p;
    
    p = (ushort *)ip_ptr;
    return((GETSHORT(&p[IP_FRAG_INFO]) >> 13) & 1);
}

static inline unsigned getfo(iphdrtype *ip_ptr) {
    ushort *p;
    
    p = (ushort *)ip_ptr;
    return(GETSHORT(&p[IP_FRAG_INFO]) & FOMASK);
}

/*
 * IP Type of Service definitions
 */
#define	IP_TOS_PRECEDENCE	0xe0	/* mask of precedence in tos byte */
#define	IP_TOS_LOW_DELAY	8	/* values must be shifted 1 bit */
#define	IP_TOS_HIGH_TPUT	4	/* before using */
#define	IP_TOS_HIGH_RELY	2
#define IP_TOS_LOW_COST		1
#define IP_TOS_NORMAL		0
#define IP_TOS_MASK		0x1e	/* mask of tos in tos byte */

#define IP_TOS_METRIC_TYPES     8
#define IP_TOS_SHIFT		1

/*
 * IP TOS Precedence values (High order 3 bits)
 */
#define	TOS_PREC_NET_CONTROL	0xe0
#define	TOS_PREC_INET_CONTROL	0xc0
#define	TOS_PREC_CRIT_ECP	0xa0
#define	TOS_PREC_FLASH_OVER	0x80
#define	TOS_PREC_FLASH		0x60
#define	TOS_PREC_IMMEDIATE	0x40
#define	TOS_PREC_PRIORITY	0x20
#define	TOS_PREC_ROUTINE	0x00

#define IP_PRECEDENCE_SHIFT	5	/* shift value up to precedence bits */

/*
 * Fragment offset mask
 */

#define IP_FRAGOFFSET_MASK	0x1fff	/* extract fragment offset in 2nd */
					/* long */

/*
 * Define default Time to Live (TTL) value
 */
#define TTL_DEFAULT	255
#define TTL_1		1
#define TTL_2		2
#define TTL_255		255

/*
 * Values for the don't fragment field
 */
#define FRAGMENTS_ALLOWED   FALSE
#define FRAGMENTS_NOTALLOWED  TRUE

/*
 * IP bit masks.
 */

#define IP_DF_BIT_MASK		0x40	/* dont fragment */
#define IP_MF_BIT_MASK		0x20	/* more fragments */

/*
 * Useful macros for accessing start of IP fields
 */

#define ipheadsize(hdr)  ((hdr)->ihl << 2)
#define ipheadstart(pkt) ((pkt)->network_start)
#define ipdatastart(pkt) ((uchar *)ipheadstart(pkt) + \
			  ipheadsize((iphdrtype *)ipheadstart(pkt)))
#define iphdrtype_start(pkt) ((iphdrtype *)ipheadstart(pkt))
#define ipdatastart_iph(iph) ((uchar *)(iph) + ipheadsize(iph))

/*
 * Data structures for storing IP options (further specified in ipoptions.h)
 */

typedef struct ipoptiontype_ {
    int length;
    int paddedlength;
    uchar ipopts[MAXIPOPTIONBYTES];
} ipoptiontype;

/*
 * Bytes of IP header
 */
#define	NOPT (ipoptiontype *)0
#define IPHEADERBYTES(opt) (((opt) ? (opt)->paddedlength + MINIPHEADERBYTES : (MINIPHEADERBYTES + ip_secopt_bytes)))

/*
 * Descriptor for an IP fragment hole.
 * These are stored in the actual holes themselves.
 * Therefore, this structure must be less than or equal to 8 bytes
 * in order to avoid corruption when receiving out of order fragments
 * of minimum size (8 bytes).
 */

typedef struct holetype {
    ushort first,last;
    struct holetype *next;
} *holetypeptr;

#define holehdr unspecified[0]		/* used in reassembly */
#define tstamp  gp_timer		/* used in reassembly */

/*
 * Parameter structure for ipwrite and ipbuild_head routines
 */

typedef struct ipparamtype_ {
    short ident;			/* packet id */
    uchar tos;				/* type of service */
    uchar ttl;				/* time to live value */
    tinybool dontfragment;		/* fragment packet */
    tinybool log;			/* log output packet */
    tinybool switching;			/* packet data is switched */
} ipparamtype;

/*
 * IP traffic counting
 */
typedef struct ip_traffic_t_ {
    ulong inputs;
    ulong forwarded;
    ulong outputs;
    ulong formaterr;
    ulong checksumerr;
    ulong hopcount;
    ulong noprotocol;
    ulong rainputs;
    ulong reassembled;
    ulong ratimeout;
    ulong rafailed;
    ulong bcastin;
    ulong mcastin;
    ulong noroute;
    ulong localdest;
    ulong notgateway;
    ulong bcastout;
    ulong mcastout;
    ulong fragged;
    ulong fragcount;
    ulong fragfailed;
    ulong encapsfailed;
    ulong badsecure;
    ulong badoption;
    ulong options;

    ulong opt_end;
    ulong opt_nop;
    ulong opt_bassec;
    ulong opt_lsr;
    ulong opt_tstmp;
    ulong opt_extsec;
    ulong opt_cipso;
    ulong opt_rrt;
    ulong opt_sid;
    ulong opt_ssr;
    ulong opt_unk;
    ulong opt_alert;

    ulong gre_inputs;
    ulong ipip_inputs;
    ulong eonip_inputs;
} ip_traffic_t;

/*
 * ICMP definitions
 */

#define ICMP_DELAYTIME	(3*ONEMIN)

#define ICMP_ECHOREPLY 0		/* echo reply */
#define ICMP_ECHOREPLYSIZE ICMPHEADERBYTES + 4  /* 4 bytes of data */

#define ICMP_UNREACHABLE 3		/* destination unreachable */ 
#define ICMP_UNREACHABLESIZE ICMPHEADERBYTES + MINIPHEADERBYTES /* w/IP hdr */

#define ICMP_UNRNET 0				/* net unreachable code */
#define ICMP_UNRHOST 1				/* host unreachable code */
#define ICMP_UNRPROTOCOL 2			/* protocol unreachable code*/
#define ICMP_UNRPORT 3				/* port unreachable code */
#define ICMP_UNRFRAG 4				/* couldn't fragment code */
#define	ICMP_UNRSRFAIL 5			/* source route failed code */
#define ICMP_UNRNONET 6				/* no such network */  
#define ICMP_UNRNOHOST 7			/* no such host */
#define ICMP_UNRHOSTISOLATED 8			/* host isolated */
#define ICMP_UNRNETPROHIB 9			/* dod net admin prohib */
#define ICMP_UNRPROHIB 10			/* dod host admin. prohibit */
#define ICMP_UNRNETTOS 11			/* net and tos unreachable */
#define ICMP_UNRHOSTTOS 12			/* host and tos unreachable */
#define ICMP_UNRADMIN 13			/* administratively prohib. */
#define ICMP_UNRHOSTPREC 14			/* host precedence */
#define ICMP_UNRPRECEDENCE 15			/* precedence cutoff */

#define ICMP_QUENCH 4			/* source quench */
#define ICMP_QUENCHSIZE ICMPHEADERBYTES + MINIPHEADERBYTES /* w/IP hdr */

#define ICMP_REDIRECT 5			/* redirect */
#define ICMP_REDIRECTSIZE ICMPHEADERBYTES + MINIPHEADERBYTES /* w/IP hdr */
#define ICMP_REDNET 0				/* net redirect code */
#define ICMP_REDHOST 1				/* host redirect code */
#define ICMP_REDSERVNET 2			/* TOS and net code */
#define ICMP_REDSERVHOST 3			/* TOS and host code */

#define ICMP_ALTADDRESS 6		/* Alternate address */

#define ICMP_ECHO 8

#define	ICMP_RDPADVERT 9		/* RDP advertisements */

#define	ICMP_RDPSOLICIT 10		/* RDP solicitations */

#define ICMP_TIMEEXCEEDED 11		/* time exceeded */
#define ICMP_TIMEEXCEEDEDSIZE ICMPHEADERBYTES + MINIPHEADERBYTES + UDPHEADERBYTES
#define ICMP_TIMTTL 0				/* ttl in transit code */ 
#define ICMP_TIMFRAG 1				/* ra timeout exceeded code */

#define ICMP_PARAMETER 12		/* parameter problem */
#define ICMP_PARAMPROB 0			/* general problem */
#define ICMP_PARAMREQUIRED 1			/* option is missing */
#define ICMP_PARAMREQNOROOM 2			/* option missing, no room to add */

#define ICMP_TIMESTAMP 13
#define ICMP_TIMESTAMPSIZE ICMPHEADERBYTES + 12	/* Three longwords of data */
#define ICMP_TIMESTAMPINVALID 0x80000000        /* "Nonstandard" bit */

#define ICMP_TIMESTAMPREPLY 14
#define ICMP_INFOREQUEST 15
#define ICMP_INFOREPLY 16
#define ICMP_MASKREQUEST 17
#define ICMP_MASKREQUESTSIZE ICMPHEADERBYTES + 4 /* One word for the mask */
#define ICMP_MASKREPLY 18
#define ICMP_MASKREPLYSIZE ICMPHEADERBYTES + 4 /* One word for the mask */
#define ICMP_TRACEROUTE 30
#define ICMP_CONVERSION 31
#define ICMP_MOBILEREDIRECT 32

/*
 * Each ICMP type is different after the first 32 bits.
 * This struct is intended for Echo and Address Mask ICMP types
 */

#define ICMPHEADERBYTES 8		/* Standard header length */
#define ICMPMINHEADERBYTES 4		/* Minimum necessary to even parse */
#define icmp_embedded_iph(pak) ((iphdrtype *)(ipdatastart(pak) +\
					      ICMPHEADERBYTES))

typedef struct icmptype_ {
	uchar type;
	uchar code;
	ushort checksum;
	ushort identifier;
	ushort sequence;
	ulong data[0];

#define	next_hop_mtu sequence		/* for path MTU discovery */
#define redir_gw identifier		/* For redirects */
} icmptype;

/*
 * Find the start of the original IP datagram inside the ICMP message.
 */
#define icmporiginalstart(pkt) (ipdatastart(pkt) + ICMPHEADERBYTES)

#define ICMP_ROUTER_TS_LONGS	sizeof(sys_timestamp)/sizeof(ulong)
#define ICMP_ROUTER_TS_WORDS	sizeof(sys_timestamp)/sizeof(ushort)

/*
 * Find the start of the original data inside the IP datagram inside the
 * ICMP message.
 */
static inline uchar *icmpdatastart (paktype *pak)
{
    iphdrtype *iphead;

    iphead = (iphdrtype *)icmporiginalstart(pak);
    return((uchar *)iphead + (iphead->ihl << 2));
}

/*
 * This struct is intended for RDP ICMP types
 */

typedef	struct icmprdptype_ {
	uchar type;
	uchar code;
	ushort checksum;
	uchar  num_addrs;
	uchar  entry_size;
	ushort lifetime;
	ulong data[1];
} icmprdptype;

struct echotype_ {
    struct echotype_ *next;
    struct echotype_ *child;
    tt_soc *tty;
    ulong active;
    sys_timestamp timeout;
    ushort id;
    ushort sequence;
    ulong deltatime;
    uchar tos;
    uchar how;
    ushort mtu;    /* for handling frag needed and DF set */
    ipaddrtype who;
    ipoptiontype *options;
    boolean broadcast;
    boolean interactive;
    boolean validate;			/* TRUE if validating return data */
    boolean rotating_pattern;		/* TRUE if using a rotating pattern */
    ushort pattern;			/* Data pattern */
    short length;			/* Data pattern length (words) */
    ushort corruption;			/* Corruption pattern */
    ushort corrupted_words;		/* Count of corrupted words */
    ushort corruption_offset;		/* Offset to first corruption */
    ushort ttl;				/* TTL setting for multicast packets */
};

struct tracetype_ {
    struct tracetype_ *next;             /* used for queueing */
    tt_soc *tty;                         /* tty used by this instance */
    conntype *conntop;                   /* used to detect process death */
    ulong active;                        /* flag to say this packet active */
    ushort ttl;                          /* time to live */
    sys_timestamp timeout;               /* maximum time to wait */
    sys_timestamp timesent;
    ushort dport;                        /* destination port */
    ushort sport;                        /* source port */
    uchar how;                           /* type of termination */
    ipoptiontype *options;               /* incoming options */
    ulong whichoptions;			 /* which options are set? */
    ipaddrtype who;                      /* pass back to caller */
    boolean interactive;                 /* support non-interactive */
    ipaddrtype currentGateway;           /* output control */
    ulong deltatime;                     /* output value */
};

/*
 * ICMP traffic statistics
 */
typedef struct icmp_traffic_t_ {
    ulong inputs;
    ulong outputs;
    ulong checksumerr;
    ulong formaterr;
    ulong echoreprcv;
    ulong unreachrcv;
    ulong quenchrcv;
    ulong redirectrcv;
    ulong echorcv;
    ulong ttlrcv;
    ulong paramrcv;
    ulong timestrcv;
    ulong timestreprcv;
    ulong inforeqrcv;
    ulong inforeprcv;
    ulong maskreqrcv;
    ulong maskreprcv;
    ulong otherrcv;
    ulong echorepsent;
    ulong unreachsent;
    ulong quenchsent;
    ulong redirectsent;
    ulong echosent;
    ulong ttlsent;
    ulong paramsent;
    ulong timestsent;
    ulong timestrepsent;
    ulong inforeqsent;
    ulong inforepsent;
    ulong maskreqsent;
    ulong maskrepsent;
    ulong rdpsolsent;
    ulong rdpsolrcv;
    ulong rdpadvsent;
    ulong rdpadvrcv;
} icmp_traffic_t;


/*
 * UDP definitions
 */

struct udptype_ {
	ushort sport;			/* source port */
	ushort dport;			/* destination port */
	ushort length;			/* bytes in data area */
	ushort checksum;		/* checksum */
	ulong udpdata[0];		/* start of UDP data bytes */
};

/*
 * Minimum UDP datagram length
 */
#define UDPHEADERBYTES 8

/*
 * PIM header size
 */
#define PIMHEADERBYTES 4

/*
 * max # of entry to be processed in NVGEN before suspend 
 * (to be used in "ip route ...", "ip host ...") 
 */
#define MAX_ENTRY_PER_NVGEN 300 


/* Default max number of UDP small servers processes. */

#define UDP_DEFAULT_MAX_SMALL_SERVERS 10 /* Default process limiter */

/*
 * UDP Traffic Statistics
 */
typedef struct udp_traffic_t_ {
    ulong inputs;
    ulong outputs;
    ulong checksumerr;
    ulong noport;
    ulong bcastforward;
} udp_traffic_t;

/*
 * TCP definitions (see also tcp.h)
 */

struct tcptype_ {
    ushort sourceport;
    ushort destinationport;
    ulong sequencenumber;
    ulong acknowledgementnumber;
#ifdef BIGENDIAN
    unsigned dataoffset: 4;
    unsigned reserved: 6;
    unsigned urg: 1;
    unsigned ack: 1;
    unsigned psh: 1;
    unsigned rst: 1;
    unsigned syn: 1;
    unsigned fin: 1;
#endif
#ifdef LITTLEENDIAN
    unsigned char reserved1: 4;
    unsigned char dataoffset: 4;
    unsigned char fin: 1;
    unsigned char syn: 1;
    unsigned char rst: 1;
    unsigned char psh: 1;
    unsigned char ack: 1;
    unsigned char urg: 1;
    unsigned char reserved2: 2;
#endif
    ushort window;
    ushort checksum;
    ushort urgentpointer;
    uchar data[4];
};
#define	TCP_FLAGS_FRAGMENT	1	/* ACKRST bits occur in fragment 1 */
#define TCP_DOFF_FLAGS_OFFSET	12	/* byte offset from start of TCP
					 * header to word containing
					 * data offset and flags.
					 */
#define TCP_URG_MASK		0x0020	/* Mask for urg */
#define TCP_ACK_MASK		0x0010	/* Mask for ack */
#define TCP_PSH_MASK		0x0008	/* Mask for psh */
#define TCP_RST_MASK		0x0004	/* Mask for rst */
#define TCP_SYN_MASK		0x0002	/* Mask for syn */
#define TCP_FIN_MASK		0x0001	/* Mask for fin */
#define TCP_ACKRST_MASK		(TCP_ACK_MASK | TCP_RST_MASK)

/*
 * The IP socket data structure.
 * For use with UDP protocol family.
 */

struct ipsocktype_ {
	struct ipsocktype_ *next;	/* pointer to next socket */
	watched_queue *inq;		/* input queue */
	queuetype outq;			/* output queue */
	int inallocated;		/* packets in input queue */	
	int inallowed;			/* max packets in input queue */
	ushort status;			/* various stuff for this sock */
	uint protocol;			/* protocol type, e.g. UDP, TCP */
	addrtype laddress;
	addrtype faddress;
	ushort lport;
	ushort fport;
	ushort ident;			/* used for IP identifier field */
	tt_soc* soctty;			/* pointer to stdio of creator */
	idbtype *if_output;		/* output interface, if any */
	watched_boolean *errflag;	/* error event indication */
	uchar error;			/* type of error */
};

/*
 * Flags in the status field of the ipsocktype structure
 */
#define SOCK_ANYSRC             0x0001  /* accept any source port */
#define SOCK_INTERR             0x0002  /* PSI interrupts break read waits */
#define SOCK_NETWORK_ERROR      0x0004  /* Subscribe to err notificatication */
#define SOCK_SRC_DYN            0x0008  /* Dynamically maintain source */
                                        /* with each write */ 
#define SOCK_SRC_SPECIFIED      0x0010  /* Maintain specified source */
                                        /* IP address from application */

#define IPMAXIN 50			/* buffer this many IP datagrams */

/*
 * IP address data structure
 * Used for creating a linked list of IP addresses so an interface
 * can have multiple addresses.
 */

struct ipaddtype_ {
    struct ipaddtype_ *next;
    ipaddrtype address;
    ipaddrtype mask;
};

/*
 * Version of ipaddtype used in linked lists of multicast addresses.
 * This structure adds a refcount.
 */
typedef struct ipmultiaddtype_ {
    struct ipmultiaddtype_ *next;
    ipaddrtype address;
    ipaddrtype mask;
    int refcount;
} ipmultiaddtype;

/*
 * DNSIX NAT Record identifier definitions. 
*/

#define	DNSIX_AUDIT_REDI_RCV    9
#define	DNSIX_SECERROR_SENDR    211
#define	DNSIX_SECERROR_RECV     212
#define DNSIX_REDI_IGNORED	223


/*
 * IP host information in the idb.
 */

struct iphostinfotype_ {
};

enum IP_NETMASK_FORMATS {
    MASK_BITCOUNT,
    MASK_DOTTED_DECIMAL,
    MASK_HEXADECIMAL,
};

/*
 * This code assists in the interface between the IP subsystem and the
 * radix module (which is protocol independant).
 */
#define	IPRADIXKEYLEN	(sizeof(ipaddrtype)+1)	/* length + address */
#define	IPRADIXDATAOFFSET 8			/* 8 bits before data */

typedef struct ipradixkeytype_ {
    uchar pad[3];
    uchar length;
    ipaddrtype value;
} ipradixkeytype;

/*
 * ip_addr2radixkey
 *
 * Move IP address into a special format for the radix code.
 * The first three (ignored) bytes are a padding.
 * The next byte is the length of the length byte + key
 * followed by the key.
 *
 * Return a pointer to the length byte of this data structure.
 */
static inline uchar *
ipaddr2radixkey (ipaddrtype source, ipradixkeytype *key)
{
    key->length = IPRADIXKEYLEN;
    key->value  = source;
    return(&key->length);
}

/*
 * ip_bitsinmask
 * Given a netmask, return the number of significant bits in the mask.
 */
static inline int ip_bitsinmask (ipaddrtype mask)
{
    int prefix = 0;

    while (mask & 0x80000000) {
      mask = mask << 1;
      prefix++;
    }
    return (prefix);
}

/*
 * ip_copy_address
 *
 * Copy an IP address from a ptr of ipaddrtype into a ptr of addrtype.
 */
static inline void ip_copy_address (ipaddrtype from, addrtype *to)
{
    to->type = ADDR_IP;
    to->ip_addr = from;
}

/*
 * ip_checksum_adjust_inline
 * Adjust the checksum to account for a change from old to new.
 * From RFC 1624.
 */

static inline ushort
ip_checksum_adjust_inline (ushort checksum, ushort old_ushort,
			   ushort new_ushort)
{
    ulong tlong;

    tlong = checksum - (~old_ushort & 0xffff);
    tlong = ((tlong & 0xffff) + (tlong >> 16)) & 0xffff;
    tlong = tlong - new_ushort;
    return((tlong & 0xffff) + (tlong >> 16));
}

/*
 * This macro puts together a string based on the specified traffic
 * counter so we can find out why a packet ended up in ip_dispose
 * when debugging is enabled.
 */
#define IP_DISPOSE(class,type,pak)\
ip_dispose(& ## class ## _traffic. ## type, pak, #class ## "." ## #type);
					      

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern ipparamtype ipdefaultparams;
extern ipparamtype iprouteparams;
extern queuetype echoQ;
extern sys_timestamp ip_icmptimer;
extern ip_traffic_t ip_traffic;
extern icmp_traffic_t icmp_traffic;
extern udp_traffic_t udp_traffic;

extern boolean ip_router_running;
extern boolean ip_mrouter_running;

extern uint udp_forwcnt;
extern uint udp_forward_port[];
extern ulong udpservers_max;
extern ulong udpservers_current;
extern int ip_background_pid;

/*
 * More inlines that had to come after the extern declarations.
 */

/*
 * udp_increment_server_count
 *
 * Bump the count of active small UDP servers
 */
static inline void udp_increment_server_count (void)
{
    udpservers_current++;
}

/*
 * udp_decrement_server_count
 *
 * Drop the count of active small UDP servers
 */
static inline void udp_decrement_server_count (void)
{
    if (udpservers_current)
	udpservers_current--;
}

/*
 * Miscellaneous function declarations
 */
extern void clear_ip_accounting_func( boolean checkpoint  );

/*
 * icmp.c
 */
extern boolean icmp_routing_up(void);
extern boolean send_echo(ipaddrtype, echotype *, int, ushort, ipaddrtype,
			 boolean, idbtype *);
extern boolean send_echo_internal (ipaddrtype, echotype *, int, ipaddrtype,
				    boolean, idbtype *, paktype *, uchar,
				    boolean);
extern char *unreach2string(uchar);
extern void icmp_process(paktype *);
extern icmptype *icmp_sendsetup(paktype *, paktype **, ulong *);
extern void icmp_show_traffic(void);
extern void send_parameter(paktype *, int, int);
extern void send_quench(paktype *);
extern void send_redirect(paktype *, ipaddrtype);
extern void send_timeexceeded(paktype *, long);
extern void send_unreachable(paktype *, uchar);
extern void send_irdpsolicit(idbtype *);

/*
 * icmpping.c
 */
extern void register_snmp_ip_echo(void);
extern void ip_ping(char *);
extern boolean ip_ping_parse(char *, int, int *,char **, boolean *);

/*
 * ipaccount.c
 */
extern void ip_acct_fixup(boolean);
extern void ip_acct_init(void);
extern void ip_accumulate_acctg(iphdrtype *ip, acl_headertype *violation);

/*
 * ipaddress.c
 */ 
extern void ip_setaddresses(void);
extern boolean address_probe(int count, ulong seconds);
extern boolean address_boot(void);
extern boolean ipisresolved(void);
extern void ip_address_change(idbtype *, ipaddrtype, ipaddrtype, boolean, 
			      boolean);
extern void class_resolve(idbtype *idb);
extern boolean mask_contiguous(ipaddrtype mask);
extern int ip_bytesinmask(ipaddrtype netmask);
extern int ip_bitsinmask(ipaddrtype netmask);
extern ipaddrtype ip_bitcount2mask(int bitcount);
extern boolean good_address(ipaddrtype address, ipaddrtype mask, boolean
			    maskcheck); 
extern boolean good_route_mask(ipaddrtype address, ipaddrtype mask,
			       boolean subnetzero_allowed);
extern ipaddrtype get_majornet(ipaddrtype address);
extern ipaddrtype get_majormask(ipaddrtype address);
extern boolean parse_ip_address(char *ptr, ipaddrtype *result);
extern boolean parse_hostname(char *ptr, ipaddrtype *result);
extern boolean ip_parse_hostname(char *ptr, addrtype *result);
extern boolean ip_address_duplicate(ipaddrtype address, ipaddrtype mask,
				    idbtype *ignore_idb, boolean primary); 
extern idbtype *pick_defaultidb(ipaddrtype network);
extern ipaddrtype ip_best_local_address(ipaddrtype target, boolean secondok);
extern ipaddrtype ip_best_ifaddress(ipaddrtype target, idbtype *idb,
				    boolean secondok); 
extern boolean martian(ipaddrtype address, boolean hostflag);
extern boolean ip_martian(addrtype *address, boolean hostflag);
extern boolean ip_broadcast(ipaddrtype target, idbtype *idb);
extern boolean ip_multicast(ipaddrtype target, idbtype *idb);
extern boolean ip_ifnetwork(idbtype *idb, ipaddrtype target);
extern boolean ip_ifaddress(idbtype *idb, ipaddrtype target);
extern boolean ip_ouraddress(ipaddrtype target);
extern idbtype *ip_ouraddress_interface(ipaddrtype target);
extern boolean ip_aliasaddress(ipaddrtype target);
extern boolean ip_up_ouraddress(ipaddrtype target);
extern boolean ip_ifbroadcast(ipaddrtype target, ipaddrtype address,
			      ipaddrtype mask); 
extern char *ip_resolvestr(idbtype *idb);
extern void ip_multicast_ctl(idbtype *idb, ipaddrtype group, boolean join);
extern void ip_slarp_resolve(paktype *);
extern void ip_slarp_sendrequest(idbtype *);
extern idbtype *samecable(ipaddrtype);
extern boolean ip_address_on_idb(ipaddrtype, idbtype *);
extern boolean ip_match_connected_subnet(ipaddrtype address, ipaddrtype
					 *subnet, ipaddrtype *subnet_mask);
extern boolean check_slip_addr(ipaddrtype);
extern void ip_address_command(parseinfo *);
extern void ipaddr_table_rebuild(idbtype *);
extern void fixup_ipsockets(void);
extern void ip_extract_addr(paktype *, addrtype *, enum SRCDST);
extern boolean ip_match_address (addrtype *, addrtype *);

/*
 * ipinput.c
 */
extern void ip_rxtypes_update(idbtype *);
extern boolean ip_forus(paktype *);
extern boolean ip_forward(paktype *);
extern boolean ip_forward_check(paktype *, ipaddrtype);
extern process ip_input(void);
extern paktype *addnew(paktype *, paktype *);
extern paktype *reassemble(paktype *);
extern boolean ip_sanity_check(paktype *);
extern void internet_enqueue(paktype *);
extern void ip_enqueue(paktype *);
extern void ip_init(subsystype *);
extern void ip_ratimeout(void);
extern void ip_show_traffic(void);
extern void ipbug(paktype *, char *);
extern void ipbug_error(paktype *, char *);
extern void ipbug_no_outputif(paktype *, char *);
extern void ipbuggw(paktype *, char *);
extern void ipttl_fixup(paktype *);
extern void makehole(paktype *, holetypeptr, uint, uint);
extern void nd_forward(paktype *);
extern void sdns_forward(paktype *);
extern fuzzy ip_broadcast_check(paktype *, boolean);
extern void default_ip_enqueue(paktype *);
extern void ip_simple_enqueue(paktype *);
extern boolean ip_forward_dbcast(void);
extern boolean udp_forward_dbcast(void);
extern short ip_red_precedence(paktype *);
extern short ip_fair_queue_flow_id(paktype *);
extern void ip_dispose(ulong *, paktype *, char *);

/*
 * ipname.c
 */
extern boolean numeric_lookup(uchar, char *, addrtype *);

/*
 * ipparse.c
 */
extern void ip_show_interface_brief(idbtype *);
extern void ip_routing(boolean, boolean);
extern void recompute_idbsec(idbtype *);
extern void recompute_syssec(void);
extern void show_ip(char *);
extern void show_iptcp(char *);
extern void ip_split_horizon_command(parseinfo *);
extern void ip_glob_sec_command(parseinfo *);
extern void ip_print_network_header(paktype *, ushort **);
extern void ip_set_per_user_acl(idbtype *, int, uint);
/*
 * ipsupport.c
 */
/* extern ushort ipcrc(uchar *, int); */
extern boolean ip_route(paktype *, ulong);
extern boolean ipsendnet(paktype *, boolean, boolean);
extern paktype *fragment(paktype *, boolean, int);
extern void ip_print(paktype *);
extern void ip_sendself(paktype *);
extern void ipwrite(paktype *pak, int, short, ipaddrtype, ipaddrtype, idbtype *, ipoptiontype *, ipparamtype *);
extern void ipbuild_head(paktype *pak, int, short, ipaddrtype, ipaddrtype, ipoptiontype *, ipparamtype *);
extern int ip_printf(const char **, char **, tt_soc *, va_list *, int, int, int, int, int);
extern int ip_mask_printf(const char **, char **, tt_soc *, va_list *, int, int, int, int, int);
extern ipaddrtype ip_reply_srcadr(paktype *);
extern void ip_set_proto_outcounter(uchar, ulong *);

/*
 * iptrace.c
 */
extern void ip_trace(char *);
extern void ip_trace_sendprobe(ipaddrtype, ipaddrtype, tracetype *);

/*
 * udp.c
 */
extern boolean udp_flood(paktype *, udptype *);
extern boolean udp_forward(paktype *, udptype *);
extern ipsocktype *open_ipsocket(ushort, addrtype *, addrtype *,
				 ushort, ushort, uint);
extern paktype *read_ipsocket(ipsocktype *, int, ulong, void *, void *);
extern ipsocktype *match_udpsocket(ushort, ushort, addrtype *, paktype *);
extern paktype *setup_ipsocket(ipsocktype *, uint, uint *);
extern ushort CheckSum(uchar *, ushort, ipaddrtype, ipaddrtype, ushort);
extern boolean udp_checksum(paktype *, udptype *);
extern ushort unique_ipport(void);
extern void close_ipsocket(ipsocktype *);
extern void udp_forward_init(void);
extern void udp_process(paktype *);
extern void udp_show_traffic(void);
extern void write_ipsocket(ipsocktype *, paktype *, uint);
extern void ip_write_sock_multi(ipsocktype *, paktype *, uint);
extern void enqueue_ip_socket(ipsocktype *, paktype *);
extern boolean ip_udp_send(paktype *, ipaddrtype, ipaddrtype, ushort, ushort);
extern void udp_compute_checksums(iphdrtype *, udptype *);

/*
 *
 * tcpip/ip_media.c
 */
extern void ip_media_registry(void);

/*
 * ip/ip_chain.c
 */
extern void ip_parser_init(void);

/*
 * ip/ipalias_chain.c
 */
extern void ipalias_parser_init(void);

/*
 * ip/ipaccess_chain.c
 */
extern void ipacclist_parser_init(void);

/*
 * ip/ipaccount_chain.c
 */
extern void ipaccount_parser_init(void);

/*
 * ip/ipfast_chain.c
 */
extern void ipfast_parser_init(void);

/*
 * ip/ip_init.c
 */
extern queuetype IPsocketQ;
extern watched_queue *IPpacketQ;
extern queuetype reassemblyQ;
extern ulong accessfailcnt;
extern ulong ip_checkpointvalue;
extern boolean ip_forward_nd;
extern boolean ip_forward_sdns;
extern boolean ip_udp_fast_flood;
extern boolean ip_udp_flood_it;
extern boolean ip_forward_udp;
extern boolean ip_spanning;
extern boolean ip_spanning_anybcast;
extern boolean netsareup;		/* TRUE if all interfaces active */
extern watched_boolean *watched_netsareup;
extern boolean ip_nat_anywhere;
extern void process_wait_on_netsareup(ulong delta_time);

/*
 * ip/tftp.c
 */
extern boolean tftp_serving;
extern idbtype *ip_tftp_boot_idb;

/*
 * ip/ip_setup.c
 */
extern void ip_setup_int(setup_struct *);

/*
 * ip/ip_test.c
 */
extern void ip_test_init(void);
#endif  __IP_H__
