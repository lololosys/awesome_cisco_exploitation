/* $Id: udp.c,v 3.11.4.15 1996/08/28 12:50:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/udp.c,v $
 *------------------------------------------------------------------
 * udp.c -- User Datagram Protocol (UDP) Support
 *
 * 5-October-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: udp.c,v $
 * Revision 3.11.4.15  1996/08/28  12:50:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.14  1996/08/28  05:47:11  jjohnson
 * CSCdi67022:  %SCHED-3-STUCKMTMR: Sleep w/expired mgd timer
 * Need to push the event list before a timed wait on the udp socket
 * Branch: California_branch
 *
 * Revision 3.11.4.13  1996/07/16  08:01:22  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.11.4.12  1996/07/06  05:54:14  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.11.4.11  1996/06/28  23:17:56  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.4.10  1996/06/18  01:46:20  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.11.4.9  1996/06/17  23:32:48  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.4.8  1996/05/28  17:46:53  ahh
 * CSCdi56871:  crashed in retbuffer() called from tftp_server()
 * A foreign address of 0 doesn't mean the address is illegal.
 * Branch: California_branch
 *
 * Revision 3.11.4.7  1996/05/21  09:51:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.6  1996/05/13  20:09:52  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.11.4.5  1996/05/04  01:05:04  wilber
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
 * Revision 3.11.4.4  1996/04/19  16:54:00  albright
 * CSCdi50629:  router sends arp request for ip helper address/udp forward
 * wrong int
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/04/15  21:17:02  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/03/27  21:57:20  mleelani
 * CSCdi49709:  Broadcast/multicast translation using helpers not
 * functional.
 * Branch: California_branch
 * Use helper addresses to provide the translation
 *
 * Revision 3.11.4.1  1996/03/18  20:17:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.6.4  1996/03/16  06:59:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.6.3  1996/03/14  01:06:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.7.6.2  1996/03/07  09:47:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.6.1  1996/02/20  01:00:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  22:55:55  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.10  1996/02/08  17:16:30  albright
 * CSCdi48312:  IP helper addresses malfunctioning
 *
 * Revision 3.9  1996/01/29  19:53:58  albright
 * CSCdi47639:  Accept directed broadcst no matter what the mac address is
 * set to
 *
 * Revision 3.8  1996/01/24  20:15:20  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.7  1996/01/11  02:27:01  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.6  1995/12/28  18:27:56  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.5  1995/12/01  01:42:23  bcole
 * CSCdi39902:  HSRP packet flooded out as udp broadcast
 *
 * Avoid spanning tree flooding multicast packets.
 *
 * Revision 3.4  1995/11/22  17:47:26  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/17  09:37:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:59:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:05:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/20  18:06:06  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.10  1995/09/11  23:49:42  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.9  1995/09/09 00:53:32  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.8  1995/09/05  16:37:53  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.7  1995/08/29 21:04:27  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers" to turn on/off ECHO, DISCARD,
 * and CHARGEN ports and new knob "ip bootp server" to turn on/off BOOTP
 * service.
 *
 * Revision 2.6  1995/08/23  05:31:11  gchristy
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
 * Revision 2.5  1995/08/10  15:54:10  albright
 * CSCdi38134:  IP helper-address to multicast address fails to forward
 * packet
 *
 * Revision 2.4  1995/08/03  07:42:12  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.3  1995/07/31 15:37:21  albright
 * CSCdi37183:  turning on spanning-tree udp flooding breaks directed
 * broadcasts
 *
 * Revision 2.2  1995/06/09  13:05:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:02:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "ip_registry.h"
#include "ttysrv.h"
#include "packet.h"
#include "../ui/debug.h"
#include "config.h"
#include "ip.h"
#include "udp_debug.h"
#include "../iprouting/igrp.h"
#include "../iprouting/ipigrp2.h"
#include "logger.h"
#include "access.h"
#include "ipaccess.h"
#include "../iprouting/route.h"
#include "ip_media.h"
#include "../parser/parser_defs_tcpip.h"
#include "../if/network.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_public.h"
#include "../util/random.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/service.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Forward Declarations
 */
static process udpdiscard_daemon(void);
static process udpecho_daemon(void);
static process udpchargen_daemon(void);
static ipsocktype *find_udpsocket(paktype *, udptype *, boolean *);

/*
 * Local Storage
 */
udp_traffic_t udp_traffic;
ulong udpservers_max;
ulong udpservers_current;

/*
 * External Declarations
 */

/*
 * Parse chain for UCP debug commands 
 */
#define	ALTERNATE	NONE
#include "exec_debug_udp.h"
LINK_POINT(udp_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for UDP
 */
static parser_extension_request udp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IP_CMD, &pname(udp_debug_commands) },
    { PARSE_LIST_END, NULL }
};
/*
 * udp_init
 * Set up global UDP stuff
 */
static void udp_init (subsystype *dummy)
{
    reg_add_ip_open_socket(open_ipsocket, "open_ipsocket");
    reg_add_ip_close_socket(close_ipsocket, "close_ipsocket");
    reg_add_ip_enqueue(UDP_PROT, udp_process, "udp_process");
    reg_add_ip_forward_dbcast(UDP_PROT, udp_forward_dbcast,
			      "udp_forward_dbcast");
    reg_add_ip_show_traffic(udp_show_traffic, "udp_show_traffic");
    udp_debug_init();		/* Debugging flags */
    parser_add_command_list(udp_chain_init_table, "udp");
    udpservers_max = UDP_DEFAULT_MAX_SMALL_SERVERS;
    udpservers_current = 0;
}

/*
 * enqueue_ip_socket
 * 
 * This routine is intended to be temporary.  It should goes away when
 * the queue in ipsocktype is converted to a watched_queue.
 */
void enqueue_ip_socket (ipsocktype *soc, paktype *pak)
{
    process_enqueue_pak(soc->inq, pak);
}

/*
 * udp_process
 * Routine to handle UDP input processing
 */

void udp_process (paktype *pak)
{
    iphdrtype *ip;
    paktype *copy;
    udptype *udp;
    ipsocktype *soc;
    boolean discard_silently;

    ip = (iphdrtype *)ipheadstart(pak);

    udp_traffic.inputs++;
    udp = (udptype *)ipdatastart(pak);
    if (udp->checksum != 0) {
	if (udp_checksum(pak, udp)) {
	    return;
	}
    }

    if (udp_debug) {
	if (pak->linktype == LINK_IP) {
	    buginf("\nUDP: rcvd src=%i(%d), dst=%i(%d), length=%d",
		   ip->srcadr, udp->sport, ip->dstadr, 
		   udp->dport, udp->length);
	} else {
	    buginf("\nUDP: rcvd src=%n(%d), dst=%n(%d), length=%d",
		   pak->clns_srcaddr, udp->sport,
		   pak->clns_dstaddr, udp->dport, udp->length);
	}
    }

    /*
     * See if one of the special services wants this UDP datagram.
     */
    if (reg_invoke_ip_udp_input(pak, udp))
	return;

    /*
     * Scan IP socket queue for a socket.  If totally unknown, try
     * forwarding, else send an ICMP port unreachable and drop the packet.
     * If we're told to silently drop, do so.
     */
    soc = find_udpsocket(pak, udp, &discard_silently);
    if (discard_silently) {
	retbuffer(pak);
	return;
    }
    if (!soc) {
	if (pak->linktype == LINK_IP) {
	    if (udp_forward(pak, udp))
		return;
	    send_unreachable(pak, ICMP_UNRPORT);
	}
	IP_DISPOSE(udp, noport, pak);
	return;
    }

    /*
     * If packet was a broadcast, transmit a copy as well
     * as handing the packet to our socket.  Don't copy BootP
     * broadcasts since we forward them elsewhere.
     */
    if ((pak->flags & (PAK_BROADCAST|PAK_MULTICAST)) &&
	(soc->lport != BOOTPS_PORT)) {
	copy = pak_duplicate(pak);
	if (copy) {
	    if (!udp_forward(copy,(udptype *)(ipdatastart(copy))))
		retbuffer(copy);
	}
    }
    if (soc->inallocated == soc->inallowed) {
	retbuffer(pak);
	return;
    }
    soc->inallocated++;
    enqueue_ip_socket(soc, pak);
}

/*
 * match_udpsocket
 * match a UDP socket in the IPsocketQ with input address
 */
ipsocktype *match_udpsocket (ushort sport, ushort dport, 
			     addrtype *faddr, paktype *pak)
{
    ipsocktype *soc=NULL;

    for (soc = IPsocketQ.qhead; soc; soc = soc->next) {
	if (soc->protocol == UDP_PROT) {
	    if (soc->status & SOCK_ANYSRC) {
		if (dport == soc->lport) {
		    break;
		}
	    } else {
		if (sport == soc->fport && dport == soc->lport) {
		    if (pak)
		    	if ((pak->linktype == LINK_IP) && 
			    (soc->faddress.ip_addr == -1L))
			    break;
		    else
			if (soc->faddress.ip_addr == -1L)
			    break;
		    if (ip_match_address(faddr, &soc->faddress)) 
			break;
		}
	    }
	}
    }
    return(soc);
}

/*
 * find_udpsocket
 * Find a local UDP socket that matches this datagram.  Returns the socket
 * pointer, or NULL.
 *
 * Sets discard_silently to TRUE if the caller should discard the incoming
 * packet.
 */

static ipsocktype *find_udpsocket (paktype *pak, udptype *udp,
				   boolean *discard_silently)
{
    ipsocktype *soc;
    register ushort sport;
    register ushort dport;
    addrtype faddr, taddr;
    addrtype newsrc;
    iphdrtype *ip;
    pid_t pid;

    *discard_silently = FALSE;
    ip = (iphdrtype *)ipheadstart(pak);
    sport = n2h(udp->sport);
    dport = n2h(udp->dport);
    ip_extract_addr(pak, &faddr, SOURCE);
    ip_extract_addr(pak, &taddr, DESTINATION);


    /* Find any existing socket. */

    for (soc = IPsocketQ.qhead; soc; soc = soc->next) {
	if (soc->protocol == UDP_PROT) {
	    if (soc->status & SOCK_ANYSRC) {
		if (dport == soc->lport) {
		    break;
		}
	    } else {
		if (sport == soc->fport && dport == soc->lport) {
		    if (pak->linktype == LINK_IP && 
			soc->faddress.ip_addr == -1L)
			break;
		    if (ip_match_address(&faddr, &soc->faddress)) 
			break;
		}
	    }
	}
    }

    /*
     * If we're going to spawn a process, only do it if we are not at the
     * limit.
     */
    if (soc == NULL && (udpservers_current >= udpservers_max)) {
	*discard_silently = TRUE;
	return(NULL);			/* Drop silently. */
    }

    /*
     * Check list of well-known servers.  We may need to create the socket.
     */
    ip_extract_addr(pak, &newsrc, DESTINATION);
    if (pak->linktype == LINK_IP) {
	if (!(newsrc.ip_addr = ip_reply_srcadr(pak))) {
	    return(soc);
	}
    }

    switch(dport) {
    case DISCARD_PORT:
	if ((udpservers_enable) && (soc == NULL)) {
	    soc = open_ipsocket(UDP_PROT, &newsrc, &faddr, dport, sport, 0);
	    if (soc != NULL) {
		pid = process_create(udpdiscard_daemon, "UDP Discard",
				     NORMAL_STACK, PRIO_NORMAL); 
		if (pid == NO_PROCESS) {
		    close_ipsocket(soc);
		    return(NULL);
		} else {
		    process_set_arg_ptr(pid, soc);
		    udp_increment_server_count();
		}
	    }
	}
	break;
    case ECHO_PORT:
	if (udpservers_enable && soc == NULL && sport >= IPPORT_RESERVED) {
	    soc = open_ipsocket(UDP_PROT, &newsrc, &faddr, dport, sport, 0);
	    if (soc != NULL) {
		pid = process_create(udpecho_daemon, "UDP Echo", NORMAL_STACK,
				     PRIO_NORMAL);
		if (pid == NO_PROCESS) {
		    close_ipsocket(soc);
		    return(NULL);
		} else {
		    process_set_arg_ptr(pid, soc);
		    udp_increment_server_count();
		}
	    }
	}
	break;
    case CHARGEN_PORT:
	if (udpservers_enable && soc == NULL && sport >= IPPORT_RESERVED) {
	    soc = open_ipsocket(UDP_PROT, &newsrc, &faddr, dport, sport, 0);
	    if (soc != NULL) {
		pid = process_create(udpchargen_daemon, "UDP Chargen",
				     NORMAL_STACK, PRIO_LOW);
		if (pid == NO_PROCESS) {
		    close_ipsocket(soc);
		    return(NULL);
		} else {
		    process_set_arg_ptr(pid, soc);
		    udp_increment_server_count();
		}
	    }
	}
	break;
    case TFTP_PORT:
	if (soc == NULL)
	    soc = reg_invoke_ip_tftp_server_init(&newsrc, &faddr, dport, sport);
	break;
    }
    return(soc);
}

/*
 * udp_compute_checksums
 * Compute udp and ip checksums
 */
void udp_compute_checksums (iphdrtype *ip, udptype *udp)
{
    udp->checksum = 0;
    udp->checksum = (ushort) CheckSum((uchar *)udp,udp->length,
    				      ip->srcadr, ip->dstadr, UDP_PROT);
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);
}

/*
 * udp_forward_dbcast
 * Don't let the generic ip forwarding code handle directed broadcasts.
 * We handle these in udp_forward() already.  Besides, the generic path
 * will botch the handling of udp checksums.
 */
boolean udp_forward_dbcast (void)
{
    return(FALSE);
}

/*
 * udp_forward
 * Forward UDP broadcasts in a helpful fashion.
 * Returns TRUE if datagram was forwarded, FALSE otherwise.
 * This function supports forwarding of helpered packets, udp flooded
 * packets, and directed broadcasts.
 *
 * The configuration command ip forward is used to modify this list.
 */

uint udp_forward_port[MAXUDPFORWCNT]; 
uint udp_forwcnt;

#define DEFUDPFORWCNT 9

static const uint udp_forward_defaults[DEFUDPFORWCNT] = {
    BOOTPS_PORT, BOOTPC_PORT, TFTP_PORT,
    IEN116_PORT, DOMAIN_PORT, TIME_PORT,
    NBNS_PORT, NBDS_PORT, TACACS_PORT
};

/*
 * Function returns TRUE if ttl check has to be done.
 * 
 * By default, it returns TRUE and it returns FALSE only if 
 * all the following conditions are true:
 *
 * - multicast helper maps are configured and
 * - no spanning tree and no unicast helpers are configured and
 * - the packet is not a directed broadcast.
 */
static inline boolean udp_forward_ttl_check (boolean mhelper, 
					     boolean helper,
					     boolean forward_check)
{
    if (mhelper && !(ip_spanning || helper || forward_check)) {
	return(FALSE);
    }
       
    return(TRUE);
}

boolean udp_forward (paktype *pak, udptype *udp)
{
    int i, good;
    paktype *pakcopy;
    udptype *udpcopy;
    idbtype *idb, *input;
    ipaddtype *ptr;
    iphdrtype *ip;
    boolean helper, mhelper;
    boolean unnumbered_check;
    ipaddrtype address;
    boolean forward_check = FALSE;
    boolean drop;
    paktype *newpak;
    idbtype *if_input;

    ip = (iphdrtype *)ipheadstart(pak);

    if (!ip_forward_udp)
	return(FALSE);

    /*
     * ip_forward_check() checks to see if pak is a directed broadcast
     * and if so it will route the packet and return TRUE.  This
     * handles directed broadcasts regardless of their
     * mac-{unicast,broadcast} disposition.
     */
    input = pak->if_input;
    if (!input)
	return(FALSE);

    helper = (input->ip_helpernetQ.qhead != NULL);
    mhelper = (input->ip_mhelpermapQ.qhead != NULL);
    unnumbered_check = (ip_spanning || helper || mhelper);
    forward_check = ip_forward_check(pak, 0L);
    
    /*
     * Do ttl check.
     */
    if (udp_forward_ttl_check(mhelper, helper, forward_check)) {
	if (ip->ttl == 0)
	    return(FALSE);
	if (!(pak->flags & PAK_MCICHECKED)) {
	    if (ip->ttl == 1)
		return(FALSE);
	}
    }
    
    if (forward_check) {
	/*
	 * Unnumbered interfaces are treated differently than numbered
	 * interfaces.  Because unnumbered interfaces share an address
	 * with another interface, they could also share the same
	 * broadcast address (assuming its own broadcast address isn't
	 * set).  If the helper address(es) or the spanning tree
	 * flooding is configured, then it will handle the forwarding.
	 * If both of these are turned off, then we will forward it as
	 * a directed broadcast.
	 */
	if ((unnumbered_check == FALSE) || (input->ip_unnumbered == NULL))
	    return(udp_flood(pak, udp));
    }

    /*
     * Clear out any turds ip_forward_check() left behind.
     */
    pak->if_output = NULL;

    /*
     * At this point, if this packet isn't a physical broadcast, then
     * just return.  The helper and spanning-tree mechanisms don't
     * forward unicast packets.
     */
    if (ip_broadcast_check(pak, unnumbered_check) == FUZ_FALSE) {

	/*
	 * Add support for helpering multicast packets also.
	 */
	if (!IPMULTICAST(ip->dstadr)) {
	    return(FALSE);
	} 
    }

    /*
     * We forward only certain UDP types.  This keeps the world
     * safe from RWHO and keeps stray RIP packets from confusing
     * gateways.  In the helpered and udp flooded cases, we
     * want to only forward udp datagrams that match the
     * udp_forward_port[] array.
     */
    for (i = 0, good = FALSE; i < udp_forwcnt; i++) {
	if (udp_forward_port[i] == udp->dport) {
	    good = TRUE;
	    break;
	}
    }
    if (!good)
	return(FALSE);

    /*
     * If we are using the simple helper address, go through a number
     * of gyrations to route the packet appropriately.  If we are using
     * the spanning tree, simply rebroadcast out every IP capable interface
     * currently in a FORWARDING state.
     */
    if (!ip_spanning) {

	/*
	 * Use helper maps for broadcast packets only.
	 */
	if (helper && (!IPMULTICAST(ip->dstadr))) {
	    for (ptr = (ipaddtype *) input->ip_helpernetQ.qhead; ptr;
		 ptr = ptr->next) {
  		/*
		 * Make a copy of the packet and send along for each
		 * helper address on the input interface.
		 */
		pakcopy = pak_duplicate(pak);
		if (!pakcopy)
		    continue;
		address = ptr->address;
		udpcopy = (udptype *)ipdatastart(pakcopy);
		if_input = pak->if_input;
		/*
		 * If it is a multicast address, then send it along to
		 * the multicast forwarding engine.  This should probably
		 * go into ip_forward_check() and udp_flood(), but that
		 * would require changes to the multicast forwarding
		 * code to allow the two functions to be seperated.
		 * Not to mention, it would slow down the spanning tree
		 * flooding case just a bit.
		 */
		if (IPMULTICAST(address)) {
		    iphdrtype *ip = (iphdrtype *)ipheadstart(pakcopy);
		    ip->dstadr = address;
		    udp_compute_checksums(ip, udpcopy);
		    if (reg_invoke_ip_mforward(pakcopy))
			continue;
		    retbuffer(pakcopy);
		    continue;
		}

		/*
		 * Don't know if we have to translate the ip helper address
		 * or not, but may need to try.
		 */
		if (if_input->ip_info && if_input->ip_info->nat_enabled) {
		    iphdrtype *ip = (iphdrtype *)ipheadstart(pakcopy);
		    ip->dstadr = address;

		    if (if_input->ip_info->nat_outside) {
			/*
			 * The UDP broadcast packet is from the outside.
			 */
			newpak = reg_invoke_translate_before_routing(LINK_IP, 
							pakcopy, &drop);
		    } else {
			/*
			 * The UDP broadcast packet is from the inside.
			 */
		        newpak = reg_invoke_translate_after_routing(LINK_IP, 
							pakcopy, &drop);
		    }

		    if (newpak) {
			/*
			 * NAT translation was successful, make sure we
		 	 * have the new packet and new helper address.
			 */ 
			pakcopy = newpak;
			udpcopy = (udptype *)ipdatastart(pakcopy);
			address = ip->dstadr; 
		    } else if (drop) {
			if (udp_debug) {
			    buginf("\nUDP: broadcast packet dropped, ");
			    buginf("src=%i, dst=%i", ip->srcadr, address);
			    buginf("\nUDP: NAT translation failed");
			}
			retbuffer(pakcopy);
			continue;
		    }
		}
		if (!ip_forward_check(pakcopy, address)) {
		    retbuffer(pakcopy);
		    continue;
		}
		if (!udp_flood(pakcopy, udpcopy))
		    retbuffer(pakcopy);
	    }
	}
 
 	/*
 	 * If multicast helper map is configured, call the
 	 * mhelper map registry.
 	 */
 	if (mhelper) {
 	    reg_invoke_ip_mhelper_forward(pak);
 	}
 
 	/*
 	 * Return false if no helpers were configured.
 	 */
 	if (helper || mhelper) {
	    retbuffer(pak);
  	    return(TRUE);
  	} else {
  	    return(FALSE);
	}
    } else {
	/*
	 * Do spanning tree flooding
	 */
	spantype *span;
	tbifd_type *oif;
 
	/*
	 * If packet is a multicast, don't spanning tree flood it.
	 */
	if ((pak->flags & PAK_MULTICAST))
	    return(FALSE);
	if (!input->circ_canforward) {
	    return(FALSE);
	}
	/*
	 * If this is a subnet broadcast for the local subnet, then
	 * don't flood it.  This causes the multiple spanning trees
	 * case to work correctly for the brokerage firms.  Note that
	 * we are not checking for secondary addresses -- this is a
	 * reasonable configuration limitation.  However, for a small
	 * performance hit the user can enable the acceptance of any
	 * broadcast.  This is checked after the initial check
	 * described above.
	 *
	 * Note:  It appears that this code will forward an
	 * ip-broadcast/mac-unicast.  Is this bad?  Changing it
	 * might be.  This code has been in the field for years.
	 * However, the directed broadcast case is intercepted
	 * above.
	 */
	if ((ip->dstadr & input->ip_nets_mask) ==
	    (input->ip_address & input->ip_nets_mask)) {
	    if (!ip_spanning_anybcast ||
		!ip_broadcast(ip->dstadr, pak->if_input))
		return(FALSE);
	}
	span = input->span_ptr;
	if (!span)
	    return(FALSE);

	/*
	 * Iterate through flood table for the spanning tree
	 * associated with the input interface until we find an interface
	 * we can forward onto.
	 */
	for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {
	    iphdrtype *pakcopy_ip;
	    uint hash, counter;
	    short group;

	    idb = oif->tbifd_swidb;
	    group = idb->circ_group;

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
		
		hash = nethash(ip->srcadr) + udp->dport;
		idb = span->cct_groups[group].idb_array[hash % counter];

		if (!idb)
		    continue;
	    }

	    if ((idb->if_number == input->if_number) ||
		(idb->circ_group && (input->circ_group == idb->circ_group))){
		continue;
	    }
	    if (idb->ip_enabled && idb->circ_canforward && interface_up(idb)) {
		pakcopy = pak_duplicate(pak);
		if (!pakcopy) {
		    continue;
		}
		pakcopy_ip = (iphdrtype *)ipheadstart(pakcopy);

		udpcopy = (udptype *)ipdatastart(pakcopy);
		pakcopy_ip->dstadr = idb->ip_broadcast;
		pakcopy->desthost = (ipaddrtype) -1L;
		pakcopy->if_output = idb;
		if (!udp_flood(pakcopy, udpcopy)) {
		    retbuffer(pakcopy);
		}
	    }
	}
	retbuffer(pak);
	return(TRUE);
    }
}

/*
 * udp_forward_init
 * Initialize the UPD forwarding database with our default values
 */

void udp_forward_init (void)
{
    int i;

    ip_forward_udp = TRUE;
    for (i = 0; i < DEFUDPFORWCNT; i++)
	udp_forward_port[i] = udp_forward_defaults[i];
    udp_forwcnt = DEFUDPFORWCNT;
}

void ip_forwardproto_command (parseinfo *csb)
{
    int i, j, target;
    boolean foundit;

    if (csb->nvgen) {
        switch (GETOBJ(int,1)) {

	  case PROTOCOL_FLOOD:
	    nv_write(ip_udp_fast_flood, csb->nv_command);
	    return;

          case PROTOCOL_ND:
	    nv_write(!ip_forward_nd, "no %s", csb->nv_command);
	    return;

	  case PROTOCOL_SDNS:
	    nv_write(ip_forward_sdns, csb->nv_command);
	    return;

          case PROTOCOL_SPANNING:
	    nv_write(ip_spanning, csb->nv_command);
	    nv_add(ip_spanning_anybcast, " any-local-broadcast");
	    return;

          case PROTOCOL_UDP:
	    if (!ip_forward_udp) {
	        nv_write(TRUE, "no %s", csb->nv_command);
	    }

	    /*
	     * First write out the defaults that have been removed.
	     */
	    for (i = 0; i < DEFUDPFORWCNT; i++) {
	        target = udp_forward_defaults[i];
	        foundit = FALSE;
	        for (j = 0; j < udp_forwcnt; j++)
		    if (udp_forward_port[j] == target) {
			foundit = TRUE;
			break;
	    	    }
		if (!foundit) {
		    nv_write(TRUE, "no %s ", csb->nv_command);
		    reg_invoke_nv_add_udp_port(target);
		}
	    }

	    /*
	     * Now write out the entire list
	     */
	    for (i = 0; i < udp_forwcnt; i++) {
		target = udp_forward_port[i];
		foundit = FALSE;
		for (j = 0; j < DEFUDPFORWCNT; j++)
		    if (udp_forward_defaults[j] == target) {
			foundit = TRUE;
			break;
		    }
		if (!foundit) {
		    nv_write(!foundit, "%s ", csb->nv_command);
		    reg_invoke_nv_add_udp_port(target);
		}
	    }
	    return;
        }
    }
    switch (GETOBJ(int,1)) {

      case PROTOCOL_FLOOD:
        ip_udp_fast_flood = csb->sense;
	break;

      case PROTOCOL_ND:
        ip_forward_nd = csb->set_to_default || csb->sense;
        break;

      case PROTOCOL_SDNS:
	ip_forward_sdns = csb->sense;
	break;

      case PROTOCOL_SPANNING:
	if (csb->sense) {
	    ip_spanning = TRUE;
	    ip_spanning_anybcast = GETOBJ(int,2);
	} else {
	    ip_spanning_anybcast = FALSE;
	    if (GETOBJ(int,2) == FALSE)
		ip_spanning = FALSE;
	}
        break;

      case PROTOCOL_UDP:
        if (GETOBJ(int,2)) {	/* A port number was given */
	    if (!int_array_op((int *)udp_forward_port, (int *)&udp_forwcnt,
			      MAXUDPFORWCNT, GETOBJ(int,3), csb->sense)) {
	        if (!csb->sense)
		    printf("\nUDP port %d not found to delete", GETOBJ(int,3));
	        else
		    printf("\nMaximum UDP forwarding ports in use.");
	    } else
		if (csb->sense)
		    ip_forward_udp = csb->sense;
        } else {
	    udp_forward_init();	/* restore defaults (according to manual) */
	    ip_forward_udp = csb->set_to_default || csb->sense;
	}
        break;
    }
    ip_udp_flood_it = (ip_spanning && ip_forward_udp && ip_udp_fast_flood);
}

/*
 * udp_flood
 * Helper routine for udp_forward()
 */ 

boolean udp_flood (paktype *pak, udptype *udp)
{
    iphdrtype *ip;
    idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_output;
    udp_compute_checksums(ip, udp);
    udp_traffic.bcastforward++;

    /*
     * Help booting cisco boxes by updateing the ARP table with the hardware
     * and software addresses from the initial TFTP load request.
     * Only do this if the IP address is on the appropriate cable (since
     * we send out tftp requests with several source addresses on each
     * interface, and don't want bogus arp entries.
     */
    if (udp->dport == TFTP_PORT) {
	ip_arp_glean(pak);
    }

    if (!ip_acc_ck_count_violations(pak, idb->ip_output_accesslist)) {
	if (udp_debug)
	  buginf("\nIP: src=%i, dst=%i, access denied (udp)",
		 ip->srcadr, ip->dstadr);
	return(FALSE);
    }
    if (udp_debug) {
	buginf("\nUDP: forwarded broadcast %d from %i to %i on %s",
	       udp->dport, ip->srcadr, ip->dstadr, idb->namestring);
    }
    (void) ipsendnet(pak,TRUE,TRUE);
    return(TRUE);
}

/*
 * open_ipsocket
 * Set up an IP socket data structure.  Local address and port will be
 * defaulted if not specified.
 */

ipsocktype *open_ipsocket (ushort protocol, addrtype *laddr, addrtype *faddr,
			   ushort lport, ushort fport, uint status)
{
    register ipsocktype *soc;
    ipaddrtype local_ip_addr, remote_ip_addr;

    if (!nnets) {				/* no nets, do nothing */
	return(NULL);
    }
    soc = malloc(sizeof(ipsocktype)); /* get some memory */
    if (!soc) {				/* return failure if no memory */
	return(NULL);
    }
    soc->inallowed = IPMAXIN;		/* set input buffering limit */
    soc->protocol = protocol;		/* initialize our data structure */
    local_ip_addr = laddr ? laddr->ip_addr : 0L;
    remote_ip_addr = faddr ? faddr->ip_addr : 0L;

    /*
     * Set our source address.  If we didn't find one and we're not doing,
     * BootP, fail rather than use an all zeros source address.
     * RIP doesn't use the local address anyhow, so ignore that too.
     */
    soc->laddress.ip_addr = local_ip_addr ?  
	local_ip_addr : ip_best_local_address(remote_ip_addr, FALSE); 
    soc->laddress.type = ADDR_IP;
    if (!soc->laddress.ip_addr && (protocol == UDP_PROT) && 
	(lport != RIP_PORT) && (lport != BOOTPS_PORT) && 
	(lport != BOOTPC_PORT) && (lport != DNS_PORT)) {
	free(soc);
	return(NULL);
    }

    /*
     * Create and initialize the input queue
     */
    soc->inq = create_watched_queue("UDP Socket", 0, 0);
    if (soc->inq == NULL) {
	free(soc);
	return(NULL);
    }

    soc->lport = lport ? lport : unique_ipport();
    soc->faddress.ip_addr = remote_ip_addr;
    soc->faddress.type = ADDR_IP;
    soc->fport = fport;
    soc->status = status;
    soc->soctty = stdio;
    soc->if_output = NULL;		/* no special interface, yet */
    soc->errflag = create_watched_boolean("UDP socket error", 0);
    enqueue(&IPsocketQ, soc);		/* add socket to its queue */
    return(soc);			/* return pointer */
}

/*
 * unique_ipport
 * Create a unique 16-bit local socket number.
 */

ushort unique_ipport (void)
{
    register ipsocktype *soc;
    register uint lport;

    lport = random_gen() & 0xFFFF;
    while (TRUE) {
	for (soc = (ipsocktype *)IPsocketQ.qhead; soc; soc = soc->next) {
	   if (soc->lport == lport)
		break;
	}
	if (soc == NULL)
	    return(lport);

	lport++;
    }
}

/*
 * close_ipsocket
 * Release storage associated with an IP socket.
 */

void close_ipsocket (register ipsocktype *soc)
{
    register paktype *pak;

    if (!soc)				/* ignore nil pointers */
	return;			
    unqueue(&IPsocketQ, soc);		/* remove socket from its queue */
    while (!process_is_queue_empty(soc->inq)) { /* flush input queue */
	pak = process_dequeue(soc->inq);
	retbuffer(pak);
    }
    delete_watched_queue(&soc->inq);
    delete_watched_boolean(&soc->errflag);
    while (soc->outq.qhead) {		/* flush output queue */
	pak = dequeue(&soc->outq);
	retbuffer(pak);
    }
    free(soc);				/* release storage */
}

/*
 * read_ipsocket
 * Get next datagram from an IP socket.
 * If wait is FALSE, return immediately
 *            TRUE and time is 0, wait until a datagram arrives
 *	      TRUE and time is non-zero, wait at most time tics for datagram.
 */

paktype *read_ipsocket (register ipsocktype *soc, int wait, ulong time,
		       void *head2, void *data2)
{
    register paktype *pak;
    uchar **head, **data;
    boolean events_pushed;

    head = (uchar **)head2;
    data = (uchar **)data2;
    while (TRUE) {
	if (!process_is_queue_empty(soc->inq)) {
	    pak = process_dequeue(soc->inq);
	    --soc->inallocated;
	    *head = (uchar *)ipdatastart(pak); 
	    switch (soc->protocol) {
	      case UDP_PROT:
		*data = *head + UDPHEADERBYTES;
		break;
	      case IGRP_PROT:
		*data = *head + IGRPHEADERBYTES;
		break;
	      case IPIGRP2_PROT:
		*data = *head + IGRP2HEADERBYTES;
		break;
	      case PIM_PROT:
		*data = *head + PIMHEADERBYTES;
		break;
	      default:
		errmsg(&msgsym(SOCKUNKN, SYS), soc->protocol);
		break;
	    }
	    return(pak);
        }
	if ((soc->status & SOCK_NETWORK_ERROR) &&
	    (process_get_boolean(soc->errflag))) {
	    process_set_boolean(soc->errflag, FALSE);
		return(NULL);
	}
        if (wait == FALSE)		/* return NULL if no waiting */
	    return(NULL);
	events_pushed = process_push_event_list(NULL);
	process_watch_queue(soc->inq, ENABLE, ONE_SHOT);
	process_watch_boolean(soc->errflag, ENABLE, ONE_SHOT);
	process_wait_for_event_timed(time);
	if (events_pushed) {
	    /*
	     * Disable and pop only if the push was successful
	     * in creating a clean slate.
	     */
	    process_watch_queue(soc->inq, DISABLE, ONE_SHOT);
	    process_watch_boolean(soc->errflag, DISABLE, ONE_SHOT);
	    process_pop_event_list(NULL);
	}
	wait = FALSE;			/* don't wait again */
    }
}

/*
 * setup_ipsocket
 * Allocate a buffer for an IP write operation.  "bytes" is the maximum
 * number of bytes in the packet, "offset" is a pointer to the count of
 * bytes associated with o/s and protocol overhead.  "(char *) pak + offset"
 * is the start of the IP protocol (UDP, TCP) data portion.
 */

paktype *setup_ipsocket (register ipsocktype *soc, register uint bytes,
			uint *offset)
{
    register int len;
    register paktype *pak;

    switch (soc->protocol) {		/* figure out protocol overhead */
	case UDP_PROT:
	    len = UDPHEADERBYTES;
	    break;
	case IGRP_PROT:
	    len = IGRPHEADERBYTES;
	    break;
	default:
	    return(NULL);
    }
    len += IPHEADERBYTES(NOPT);

    bytes += len;
    pak = getbuffer(bytes);
    *offset = ENCAPBYTES + len;
    if (pak)
	pak->linktype = LINK_IP;
    return(pak);
}

/*
 * send_ip_udp
 * take a packet that already has data in it, add ip and udp headers, and
 * send it.  this is intended for use with code that uses the ip_udp_input
 * registry to get its received packets, and allows UDP packets to be sent
 * without having to open a socket.
 *  On entry, UDP data is assumed to start at datagramstart+udpheaderbytes,
 * and will be moved if the header in unexpectedly large.  datagramsize
 * contains the length of the data portion of the packet (not including IP
 * or UDP headers...)
 *  Tuba is dead, it is not supported!
 */

boolean ip_udp_send (paktype *pak, ipaddrtype laddr, ipaddrtype faddr,
		     ushort lport, ushort fport)
{
    int len;
    iphdrtype *ip; udptype *udp;

    if (!pak)
	return(FALSE);

    pak->linktype = LINK_IP;	/* It's an IP packet! */
    ip = (iphdrtype*) ipheadstart(pak);
    len =  UDPHEADERBYTES + pak->datagramsize;
    /*
     * Revise our source IP address so that hopefully it will match the
     * interface the packet is sent out on.  The final interface is not
     * chosen until later, sigh.
     */
    if (laddr == 0)
	laddr = ip_best_local_address(faddr, FALSE);

    udp = (udptype *) (pak->network_start + IPHEADERBYTES(NOPT));
    udp->sport = h2n(lport);
    udp->dport = h2n(fport);
    udp->length = h2n(len);
    udp->checksum = 0;

    /*
     * VPDN/L2F do not want checksums on
     */
    if ((faddr != -1L) && !(pak->flags & PAK_VPN_USED))
	udp->checksum = CheckSum((uchar *)udp, udp->length,
				 laddr,faddr, UDP_PROT);

    /*
     * Write IP datagram to the net.  Possibly route it.
     */
    len += IPHEADERBYTES(NOPT);
    if (udp_debug && fport != SYSLOG_PORT)
	buginf("\nUDP: sent src=%i(%d), dst=%i(%d), length=%d",
	       laddr, lport, faddr, fport, len);
    ipwrite(pak, len, UDP_PROT, laddr, faddr, NULL, NULL, NULL);
    return(TRUE);
}

/*
 * ip_write_sock_multi
 * Send a datagram set up by setup_ipsocket to the network.
 *
 * We may or may not send this packet out multiple interfaces.
 * Specifically if we are:
 *	 acting as an IP host instead of an IP router and
 * 	 handling a unicast packet with no transmit interface specified and
 * 	 cannot figure out the next hop via find_path
 * then we transmit out all IP capable interfaces and via the
 * default_gateway too.
 *
 * As we may change soc->laddress, callers should have configured 
 * their socket with SOCK_ANYSRC.
 */

void ip_write_sock_multi (ipsocktype *soc, paktype *pak, uint bytes)
{
    idbtype *idb;
    ipaddrtype laddress, orig_laddress, dest;
    paktype *pak_copy;

    if ((!ip_router_running || !systeminit_flag) &&
	(soc->if_output == NULL) &&
	((dest = soc->faddress.ip_addr) != -1) &&
	!IPMULTICAST(dest)) {
	idb = find_path(&dest, IP_PATH_DEFFLAGS);
	if (!idb) {
	    orig_laddress = soc->laddress.ip_addr;
	    FOR_ALL_SWIDBS(idb) {
		if (!idb->ip_enabled || !interface_up(idb))
		    continue;
		pak_copy = pak_duplicate(pak);
		if (!pak_copy)
		    continue;
		laddress = idb->ip_address;
		if (!laddress && idb->ip_unnumbered)
		    laddress = idb->ip_unnumbered->ip_address;
		soc->laddress.ip_addr = laddress;
		soc->if_output = idb;
		write_ipsocket(soc, pak_copy, bytes);
	    }
	    soc->if_output = NULL;
	    if (default_gateway) {
		/*
		 * Try to compute a source IP address
		 * which makes sense for use with the default_gateway
		 */
		soc->laddress.ip_addr =
		    ip_best_local_address(default_gateway, FALSE);
		write_ipsocket(soc, pak, bytes);
	    } else {
		retbuffer((paktype *)pak);
	    }
	    /*
	     * Restore socket values
	     */
	    soc->laddress.ip_addr = orig_laddress;
	    return;
	} else {
	    /*
	     * Recompute source IP address in case interface has flapped
	     */
	    soc->laddress.ip_addr =
		ip_best_local_address(soc->faddress.ip_addr, FALSE);
	}
    }
    write_ipsocket(soc, pak, bytes);
}

/*
 * write_ipsocket
 * Send a datagram set up by setup_ipsocket to the network.
 */

void write_ipsocket (register ipsocktype *soc, register paktype *pak,
		     register uint bytes)
{
    register udptype *udp;
    register int size;
    ipparamtype param;

    /*
     * Get size of network layer header.
     */
    size = IPHEADERBYTES(NOPT);
    /*
     * Revise our source IP address so that hopefully it will match the
     * interface the packet is sent out on.  The final interface is not
     * chosen until later, sigh.
     */
    if (soc->status & SOCK_SRC_DYN) {
	soc->laddress.ip_addr = 
	    ip_best_local_address(soc->faddress.ip_addr, FALSE);
    }
    
    switch (soc->protocol) {
    case UDP_PROT:
	udp = (udptype *) (pak->network_start + size);
	udp->sport = h2n(soc->lport);
	udp->dport = h2n(soc->fport);
	udp->length = h2n(UDPHEADERBYTES + bytes);
	size += UDPHEADERBYTES + bytes;
	udp->checksum = 0;

	/*
	 * We see if we are sending to a broadcast address. If not
	 * we can perform a checksum since we believe that the
	 * pseudo-header won't change out from underneath us. Broadcasts
	 * unfortunately cannot have a checksum here since the destination
	 * address may change at driver level.
	 *
	 * NOTE: this code could generate UDP packets with bad checksums.
	 */
	if (soc->faddress.ip_addr != -1L) {
	    udp->checksum = CheckSum((uchar *)udp, udp->length,
            soc->laddress.ip_addr, soc->faddress.ip_addr, UDP_PROT);
	}
	break;

    default:
	errmsg(&msgsym(SOCKUNKN, SYS), soc->protocol);
	datagram_done(pak);
	return;
    }

    /*
     * If the socket has a preferred output interface, set it.
     * This is used to choose the correct interface on which to
     * reply if we have multiple, unsubnetted interfaces onto a
     * single IP network.  This state usually exists at system
     * startup, before TFTP has loaded the configuration files.
     */
    pak->if_output = soc->if_output;

    /*
     * Write IP datagram to the net.  Possibly route it.
     */
    param.ident = soc->ident++;
    param.tos = ipdefaultparams.tos;
    param.ttl = ipdefaultparams.ttl;
    param.dontfragment = ipdefaultparams.dontfragment;
    param.log = (soc->fport != SYSLOG_PORT);
    if (ip_aliasaddress(soc->faddress.ip_addr))
    	pak->flags |= PAK_ALIAS;
    if (udp_debug && soc->fport != SYSLOG_PORT)
    	buginf("\nUDP: sent src=%i(%d), dst=%i(%d), length=%d",
    n2hl(soc->laddress.ip_addr), soc->lport, 
    n2hl(soc->faddress.ip_addr), soc->fport, size);
    ipwrite(pak, size, soc->protocol, soc->laddress.ip_addr,
    soc->faddress.ip_addr, NULL, NULL, &param);
}

/*
 * udp_show_traffic
 * UDP statistics
 */

void udp_show_traffic (void)
{
    printf("\n\nUDP statistics:");
    printf("\n  Rcvd: %lu total, %lu checksum errors, %lu no port",
	   udp_traffic.inputs, udp_traffic.checksumerr,
	   udp_traffic.noport);
    printf("\n  Sent: %lu total, %lu forwarded broadcasts",
	   udp_traffic.outputs, udp_traffic.bcastforward);
}

/*
 * CheckSum
 * Computes the checksum for designated TCP or UDP packet.
 *
 * The checksum consists of the one's complement of the one's 
 * complement sum of the 16-bit words in the tcp packet with
 * the checksum field set to zero.  This sum also includes the
 * pseudo header of the tcp packet which consists of the packet's
 * source and destination address, its length, and its type.
 *
 * Parameters:
 * ptr       - ptr to packet we are checksumming
 * paklength - length of packet in bytes
 * srcadr    - source address of packet
 * dstadr    - destination address of packet
 * typecode  - protocol type of packet
 */

ushort CheckSum (uchar *ptr, ushort paklength, ipaddrtype srcadr,
		 ipaddrtype dstadr, ushort typecode)
{
    register uint sum;
    register ushort *w1, *w2;

    /*
     * Checksum the data portion using the standard routine
     */
    sum = ipcrc((ushort *)ptr, paklength);
    sum = (~sum) & 0xFFFF;

    /*
     * Add in the pseudo header
     */
    w1 = (ushort *) &srcadr;
    w2 = (ushort *) &dstadr;
    sum += paklength + typecode + (*w1++) + (*w2++);
    sum += (*w1) + (*w2);

    /*
     * Add in the one's complement carry and return result
     */
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = (sum + (sum >> 16));
    return((ushort) ~sum & 0xFFFF);
}

/*
 * udp_checksum
 * Returns TRUE if checksum is bad. Packet is dispose of properly depending
 * on network layer that UDP runs over.
 */

boolean udp_checksum (paktype *pak, udptype *udp)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * This is only a sanity check so we dont go running off the
     * end of memory checksumming the packet.
     */
    if (n2h(udp->length) < pak->datagramsize) {
	if (pak->linktype == LINK_IP) {
	    if (CheckSum((uchar *)udp, n2h(udp->length), ip->srcadr,
			 ip->dstadr, UDP_PROT)) {
		IP_DISPOSE(udp, checksumerr, pak);
		return(TRUE);
	    }
	} 
    } else {
	if (pak->linktype == LINK_IP) {
	    IP_DISPOSE(udp, checksumerr, pak);
	    return(TRUE);
	} else {
	    retbuffer(pak);
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * udpecho_daemon
 * perform udp echo service
 */

#define	IDLE_TIMEOUT	(5 * ONESEC)	/* No need to stick around... */

static process udpecho_daemon (void)
{
    paktype *pak;
    udptype *udp;
    char *offset;
    ipsocktype *soc;

    if (process_get_arg_ptr((void**)&soc)) {
	while (TRUE) {
	    pak = read_ipsocket(soc, TRUE, IDLE_TIMEOUT, &udp, &offset);
	    if (pak == (paktype *)NULL) {
		break;
	    }
	    
	    /*
	     * IP and UDP header fields "swapped" when socket was created.
	     */
	    write_ipsocket(soc, pak, n2h(udp->length) - UDPHEADERBYTES);
	}
	reg_invoke_ip_close_socket(soc);
    }
    udp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * udpdiscard_daemon
 * perform udp discard service
 */

static process udpdiscard_daemon (void)
{
    paktype *pak;
    udptype *udp;
    char *offset;
    ipsocktype *soc;

    if (process_get_arg_ptr((void**)&soc)) {
	while (TRUE) {
	    pak = read_ipsocket(soc, TRUE, IDLE_TIMEOUT, &udp, &offset);
	    if (pak == (paktype *)NULL) {
		break;
	    }
	    retbuffer(pak);
	}
	reg_invoke_ip_close_socket(soc);
    }
    udp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * udpchargen_daemon
 * perform udp chargen service
 */

#define NEXT_ASCII_WRAP(ch)	((((ch) - ' ' + 1) % ('~' + 1 - ' ')) + ' ')

#define CHARGEN_COLS		72

static process udpchargen_daemon (void)
{
    paktype *pak;
    udptype *udp;
    uint offset;
    uchar *data;
    static uchar s_char = ' ';
    uchar ch;
    int col;
    ipsocktype *soc;

    if (process_get_arg_ptr((void**)&soc)) {
	while (TRUE) {
	    /* Read whatever was sent and throw it away. */
	    pak = read_ipsocket(soc, TRUE, IDLE_TIMEOUT, &udp, &offset);
	    if (pak == NULL)
		break;
	    retbuffer(pak);
	    
	    /* Set up a new packet and buffer (including space for CRLF). */
	    pak = setup_ipsocket(soc, CHARGEN_COLS + 2, &offset);
	    if (pak == NULL)
		break;
	    
	    /* Spit out a line. */
	    col = 0;
	    data = pak->data_area + offset;
	    for (ch = s_char; col < CHARGEN_COLS; ch = NEXT_ASCII_WRAP(ch))
		data[col++] = ch;
	    data[col++] = RETURN;
	    data[col++] = LINEFEED;
	    write_ipsocket(soc, pak, col);
	    
	    s_char = NEXT_ASCII_WRAP(s_char);
	}

	reg_invoke_ip_close_socket(soc);
    }
    udp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * UDP subsystem header
 */

#define UDP_MAJVERSION 1
#define UDP_MINVERSION 0
#define UDP_EDITVERSION  1

SUBSYS_HEADER(udp, UDP_MAJVERSION, UDP_MINVERSION, UDP_EDITVERSION,
	      udp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
