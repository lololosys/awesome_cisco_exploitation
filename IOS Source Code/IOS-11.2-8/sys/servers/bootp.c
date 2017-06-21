/* $Id: bootp.c,v 3.12.12.11 1996/08/28 13:08:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/bootp.c,v $
 *------------------------------------------------------------------
 * bootp.c  - Boot Protocol Support as per RFC 951
 *
 * 23-February-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bootp.c,v $
 * Revision 3.12.12.11  1996/08/28  13:08:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.12.10  1996/08/08  20:31:37  asb
 * CSCdi52514:  IPCP DNS negotiation returns Broadcast address
 * Branch: California_branch
 *
 * Revision 3.12.12.9  1996/08/07  09:02:17  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.12.12.8  1996/07/19  22:55:26  irfan
 * CSCdi63267:  dhcp hickups
 * Branch: California_branch
 * misc. dhcp proxy client improvements:
 *  - appear to the server as a relay agent to get unicasts back correctly.
 *  - tap packets from bootp server to see if they are responses directed
 *    to the dhcp proxy client (relay-agent).
 *  - make it work for lines that don't have a logged-in user.
 *  - reduce the sleep interval in allocation attempts.
 *  - use client ID strings beginning with a byte of 0.
 *  - rename dhcp_sock.
 *  - make allocation work if prev. request is in selecting/requesting
 *    states.
 *  - make allocation work if prev. lease entry is being purged.
 *  - should use new process primitives. also block on crashes.
 *  - add more debug information.
 *
 * Revision 3.12.12.7  1996/07/06  05:56:16  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.12.12.6  1996/06/28  23:25:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.12.12.5  1996/06/17  23:36:29  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.12.12.4  1996/05/21  10:01:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.12.12.3  1996/04/15  21:22:29  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.12.12.2  1996/04/09  14:56:27  albright
 * CSCdi52246:  BOOTP helpering from unnumbered interfaces has no giaddr
 * Branch: California_branch
 *
 * Revision 3.12.12.1  1996/03/18  21:46:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  07:28:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  10:41:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  17:09:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/02/14  19:45:48  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * making sure I don't get dinged for the lack of a space
 *
 * Revision 3.11  1996/02/14  19:36:47  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * Revision 3.10  1996/02/07  16:14:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/01  06:08:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/26  17:25:15  asb
 * CSCdi46322:  Should return async-bootp dns-server instead of routers
 * DNS server
 *
 * Revision 3.7  1996/01/24  20:19:47  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/22  06:59:03  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/11  02:30:20  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.4  1995/11/21  23:48:20  raj
 * CSCdi41168:  CSCdi38285 breaks BOOTP over VTY-async
 * Added udp checksum calls for bootp_async_reply and
 * ai_incoming_bootp_request
 *
 * Revision 3.3  1995/11/17  18:50:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:11:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:24:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/09/23  00:12:03  raj
 * CSCdi38285:  UDP checksum is not recalculated for the BOOTP reply from
 * the server
 * The small performance hit for recalculating checksums on BOOTP packets
 * is really worth it in order to avoid headaches by customers who have
 * broken BOOTP clients which will sometimes incorrectly ignore BOOTP
 * replies which have a zero checksum.
 *
 * Revision 2.13  1995/09/18  19:38:23  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.12  1995/09/10  01:46:18  shankar
 * CSCdi38875:  Crash when updating static routes at end of Autoinstall
 * Part A of commit. Eliminate a line flap instance for it causes DDR
 * to start up. VGI reports good performance with this patch.
 *
 * Revision 2.11  1995/08/30  18:37:16  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * ip_bootp_server_enable needs to be initialized to TRUE.
 *
 * Revision 2.10  1995/08/29  21:13:25  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "ip bootp server" to turn/on off BOOTP server.
 * Send ICMP port unreachable if a packet is received for TACACS port and
 * there is no TACACS server configured.
 *
 * Revision 2.9  1995/08/15  00:15:15  samuell
 * CSCdi36106:  bootp doesnt check malloc return
 *
 * Revision 2.8  1995/08/08  16:48:30  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.7  1995/08/01 04:30:38  shankar
 * CSCdi37881:  FR autoinstall does not succeed when unnumbered ports are
 * used
 *
 * Revision 2.6  1995/07/31  19:33:46  shankar
 * CSCdi37948:  Autoinstall needs a static route to bootp server for
 * special
 * cases when "unnumbered" interfaces are used at the central site. Undo
 * the
 * patch attributed to CSCdi36966.
 *
 * Revision 2.5  1995/07/24  19:03:28  shankar
 * CSCdi37097:  autoinstall on 4000/ags may not load network-confg the 1st
 * time
 *
 * Revision 2.4  1995/07/17  23:41:33  shankar
 * CSCdi36966:  Autoinstall does not need a static route to the bootp
 * server
 *
 * Revision 2.3  1995/07/12  19:45:58  shankar
 * CSCdi35964:  Frame-relay autoinstall adds unwanted static routes to
 * config
 *
 * Revision 2.2  1995/06/09  13:18:41  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:43:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/ipoptions.h"
#include "../ip/udp_debug.h"
#include "../ip/ipaddress.h"
#include "bootp.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_bootp.h"
#include "registry.h"
#include "../h/sys_registry.h"
#include "../ip/ip_registry.h"
#include "../h/media_registry.h"
#include "../h/file.h"
#include "../h/address.h"
#include "../ip/tftp.h"
#include "../os/boot.h"
#include "../iprouting/route.h"
#include "../os/hostname.h"
#include "../os/signal.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../h/cs_registry.regh"
#include "subsys.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

static bootp_spec *bootp_spec_list;
static boolean ip_bootp_server_enable = TRUE;
static pid_t bootp_pid = 0;

static void bootp_sig_handler (int signal, int dummy1, void *dummy2, 
			       char *dummy3)
{
    ipsocktype *soc;
    pid_t pid = bootp_pid;
    
    if (process_get_arg_ptr((void**)&soc))
        reg_invoke_ip_close_socket(soc);	
    bootp_pid = 0;
    process_kill(pid);
}


/*
 * bootp
 * The gateway's boot protocol server process
 */

process bootp (void)
{
    register ipsocktype *soc;
    register paktype *pak;
    register idbtype *idb;
    register hwidbtype *hwidb;
    hwaddrtype address;
    addrtype *proto_addr;
    ipaddrtype ip_address;
    ushort pvc;
    iphdrtype *ip;
    udptype *udp;
    bootptype *boot;

    proto_addr = NULL;
    soc = open_ipsocket(UDP_PROT, NULL, NULL, BOOTPS_PORT, 0, SOCK_ANYSRC);
    if (!soc) {
	bootp_pid = 0;
	process_kill(THIS_PROCESS);
    }

    process_set_arg_ptr(bootp_pid, soc);
    signal_oneshot(SIGTERM, bootp_sig_handler);
    while (TRUE) {
	pak = read_ipsocket(soc,TRUE,0L,&udp,&boot);
	if (!pak)
	    continue;
	idb = pak->if_input;
	ip  = (iphdrtype *)ipheadstart(pak);

	if (udp_debug)
	    buginf("\nBOOTP: opcode %d from host %i on %s, %d secs, %d hops",
			boot->bp_op, ip->srcadr, idb->namestring,
	  		n2h(boot->bp_secs), boot->bp_hops);

	/*
	 * Switch on opcode
	 */
	switch (boot->bp_op) {
	    case BOOTREQUEST:
		if (!systeminit_flag) {
		    retbuffer(pak);
		    break;
		}
		/*
		 * if the bootp request is on an async interface, we may
		 * wish to send a reply using the SLARP algorithm.
		 */
		hwidb = pak->if_input->hwptr;
		if (hwidb->status & IDB_ASYNC)
		    bootp_async_reply(pak, udp, boot);
		else if (hwidb->status & IDB_SERIAL) {
		    (*hwidb->extract_snpa)(pak, &address, SOURCE);
		    pvc = reg_invoke_autoinstall_copy_snpa(hwidb->enctype,
						&address);
		    /* 
		     * Knowing the incoming PVC, search the local tables
		     * to see if an entry exists. The map entry will tell
		     * us the IP address of the client. 
		     */
		    proto_addr = reg_invoke_autoinstall_addr_search( 
					hwidb->enctype, hwidb, pvc, pak);

		    /* 
		     * process the incoming autoinstall request.
		     */
		    if (proto_addr) {
			if (!ai_incoming_bootp_request(hwidb, pak, udp, 
						proto_addr)) {
			    if (udp_debug)
				buginf("\nAutoinstall BOOTP Server Error: Map "
					"or helper address error");
			} else {
			    continue;
			}
		    }
		}

		/*
		 * Pass this off to the UDP forwarder if we are an
		 * IP router. Note that we modify the packet before
		 * sending it for forwarding, so this should be the
		 * last thing done with the packet before possibly
		 * returning it to the pool.
		 */

		if (ip_router_running) {
		    if (idb->ip_unnumbered != NULL)
			ip_address = idb->ip_unnumbered->ip_address;
		    else
			ip_address = idb->ip_address;

		    /*
		     * If a gateway address is not known, fill in our
		     * address.
		     */

		    if (boot->bp_giaddr == 0)
		      boot->bp_giaddr = h2nl(ip_address);

		    /*
		     * If the source IP address is now known, fill in our
		     * address.
		     */

		    if (ip->srcadr == 0)
		      ip->srcadr = h2nl(ip_address);

		    /*
		     * Now attempt to forward the packet
		     */

		    if (udp_forward(pak, udp))
		      break;
		}

		retbuffer(pak);
		break;

	    case BOOTREPLY:

		/*
		 * If we're resolving our own address(es), possibly this
		 * reply is for one of our interfaces.  Process it and hope
		 * for the best. Under no circumstances should the reply be
		 * forwarded at this point.
		 */
		if (!netsareup) {
		    receive_bootreply(pak, boot);
		    break;
		}
		if (reg_invoke_bootp_check_local_delivery(pak)) {
		    break;
		}
		if (!bootp_forward_reply(pak, udp, boot))
		    retbuffer(pak);
		break;

	    default:
		retbuffer(pak);
		break;
	}
    }
}

/*
 * bootp_forward_reply
 * Attempt to forward BOOTP reply to client.
 * Returns FALSE upon failure, in which case packet still needs to be freed
 * by caller.
 */

boolean bootp_forward_reply (paktype *pak, udptype *udp, bootptype *boot)
{
    idbtype *idb;
    boolean broadcast;
    iphdrtype *ip;

    /*
     * Don't send the reply if
     *  - we're not a router.
     *  - the destination is not on a local cable
     *  - giaddr doesn't match our interface
     */
    if (!ip_router_running)
	return(FALSE);
    if (boot->bp_yiaddr)
	idb = samecable(boot->bp_yiaddr);
    else
	idb = samecable(boot->bp_giaddr);
    if (!idb)
	return(FALSE);
    if (!ip_ifaddress(idb, boot->bp_giaddr))
	return(FALSE);
    /*
     * We are reusing the incoming packet to send info on
     * so we must reset packet option cells and call
     * ipopt_inproc to properly set up for the outgoing path.
     */
    if (!ipopt_inproc(pak))
	return(FALSE);
    /*
     * Should we broadcast BOOTP reply?
     */
    if (boot->bp_flags & BOOT_FLAG_BROADCAST)
	broadcast = TRUE;
    else {
	/*
	 * Attempt to broadcast response if we don't have an IP address
	 * for the client.  (Common case for DHCPNAK packets.)
	 */
	broadcast = (boot->bp_yiaddr == 0);
    }
    ip  = (iphdrtype *)ipheadstart(pak);
    if (udp_debug)
	buginf("\nBOOTP: %sing response %i -> %i (%s)",
	       broadcast ? "Broadcast" : "Forward",
	       ip->srcadr, boot->bp_yiaddr, idb->namestring);
    /*
     * Indicate this is a reply, to be processed only by client.
     */
    udp->dport = BOOTPC_PORT;
    if (broadcast) {
	pak->desthost = ip->dstadr = -1;
	pak->if_output = idb;
	return(udp_flood(pak, udp));
    }
    /*
     * Set up arp cache entry so we can send a reply to the
     * client host.  Use boot reply information.
     */
    ip_arp_table_add(idb, boot->bp_yiaddr, boot->bp_chaddr,
		     idb->hwptr->enctype, ARP_DYNAMIC);
    ip->dstadr = boot->bp_yiaddr;
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *)ip, (ip->ihl << 2));
    /*
     * Need a new checksum?
     */
    if (udp->checksum != 0) {
	udp->checksum = 0;
	udp->checksum = CheckSum((uchar *)udp, udp->length,
				 ip->srcadr, ip->dstadr, UDP_PROT);
    }
    return(ip_forward(pak));
}

/*
 * bootp_start
 * Start the BOOTP server process if not already running
 */

void bootp_start (void)
{
    if (system_loading)
	return;
    /*
     * check ip_bootp_server_enable so that if bootp is off by
     * default, the BOOTP process won't be started here.
     */
    if ((!bootp_pid) && ip_bootp_server_enable) {
	bootp_pid = process_create(bootp, "BOOTP Server", NORMAL_STACK,
				   PRIO_NORMAL);
    }
}

/*
 * bootp_client
 * Process to use BootP to resolve our interface address(es)
 */

static process bootp_client (void)
{
    register ipsocktype *soc;
    register paktype *pak;
    register idbtype *idb;
    register hwidbtype *hwidb;
    hwaddrtype address;
    udptype *udp;
    bootptype *boot;
    sys_timestamp start;
    int tries;
    ushort pvc;

    /*
     * Open a client socket, die quietly if we fail.
     */
    soc = open_ipsocket(UDP_PROT, NULL, NULL, BOOTPC_PORT, BOOTPS_PORT,
			SOCK_ANYSRC);
    if (!soc)
	process_kill(THIS_PROCESS);

    /*
     * While we have unresolved interfaces, send boot requests out on
     * the remaining unresolved 10MB interfaces.
     */
    GET_TIMESTAMP(start);		/* remember starting time */
    tries = 0;				/* no retries yet */
   /*
    * The timing in this loop is critical to fast booting.  Here are
    * the relevant events: (1) it may take 1 sec or so for all the MCI's
    * to come up, because we're resetting them one by one from
    * periodic.  (2) we have at most 12 sec before manual resolver will
    * take over, and we have to wait for a long-period thing to finish
    * the reboot.  So we wait 2 sec, send a bootp, wait 7 sec for
    * responses, and then send again.
    */
    process_sleep_for(2*ONESEC);
    while (!netsareup) {

	/*
	 * If we've been trying to resolve for a while, stop bombarding
	 * the nets with broadcasts.  Sleep for a half minute between tries.
	 */
	if (++tries > MAXQUICKTRIES)
	    process_sleep_for(30*ONESEC);

	/*
	 * First we send boot requests for all unresolved interfaces.
	 */
	FOR_ALL_SWIDBS(idb) {
	    if ((idb->ip_address == 0) && (idb->hwptr->status & IDB_IEEEMASK))
		send_bootrequest(soc,idb,(int)(ELAPSED_TIME(start)/ONESEC));
	    if (udp_debug)
		buginf("\n%s : Sending Bootp request", idb->namestring);
	}

	/*
	 * Now process boot replies until we don't see one for five seconds.
	 */
	while ((pak = read_ipsocket(soc,TRUE,7*ONESEC,&udp,&boot))) {
	    if (boot->bp_op == BOOTREPLY) {
		hwidb = pak->if_input->hwptr;
		if (netsareup)
		    break;
		if (hwidb->status & IDB_SERIAL) {
		    /* 
		     * In the case that "netsareup" is false and the 
		     * incoming interface is a serial link, we 
		     * are probably getting a reply to an autoinstall
		     * related BOOTP. Lets examine the reply.
		     */
		    (*hwidb->extract_snpa)(pak, &address, SOURCE);
		    pvc = reg_invoke_autoinstall_copy_snpa(hwidb->enctype,
						&address);

 		    ai_incoming_bootp_reply(hwidb, pak, udp, pvc);
		    continue;
		}
		if (pak)
	 	receive_bootreply(pak, boot);
	    } else
		retbuffer(pak);
	}
    }
    reg_invoke_ip_close_socket(soc);		/* Done. Close socket */
    process_kill(THIS_PROCESS);			/* process termination */
}

/*
 * send_bootrequest
 * Send a boot request for the specified interface.
 * Datagram is broadcast on *all* interfaces.
 */ 

void send_bootrequest (ipsocktype *soc, idbtype *idb, int seconds)
{
    register paktype *pak, *copy;
    register bootptype *boot;
    register udptype *udp;
    idbtype *odb;
    uint offset;
    ipaddrtype ip_address;

    if (seconds < 5) {			/* I cheat. */
	seconds = 6;
    }

    pak = setup_ipsocket(soc,sizeof(bootptype),&offset);
    if (!pak) {
	return;
    }

    /*
     * Set up the BootP portion of the datagram. 
     */
    boot = (bootptype *)(pak->data_area + offset);
    udp = (udptype *) ((char *) boot - UDPHEADERBYTES);
    memset(boot, 0, sizeof(bootptype));	/* init all fields to zero */
    boot->bp_op = BOOTREQUEST;		/* a boot request */
    boot->bp_htype = 1;			/* 10MB Ethernet */
    boot->bp_hlen = IEEEBYTES;		/* 6 bytes in hardware address */
    boot->bp_secs = h2n(seconds);	/* seconds since we booted */
    /*
     * CMU bootpd needs to be told that we want the rfc standard vendor
     * info not the CMU vendor info
     */
    boot->bp_vend_cookie = BOOTP_VEND_COOKIE;
    ieee_copy((char *) idb->hwptr->hardware, (char *) boot->bp_chaddr);

    /*
     * Set up the UDP fields.
     */
    udp->sport = h2n(BOOTPC_PORT);
    udp->dport = h2n(BOOTPS_PORT);
    udp->length = h2n(UDPHEADERBYTES + sizeof(bootptype));
    udp->checksum = 0;

    /*
     * Broadcast on all interfaces a bootp request for this interface.
     * If we know the address of the output interface, use that address
     * as the giaddr field so that replies can get back to us.
     */
    FOR_ALL_SWIDBS(odb) {
	if (odb->ip_unnumbered != NULL)
	    ip_address = odb->ip_unnumbered->ip_address;
	else
	    ip_address = odb->ip_address;
	boot->bp_giaddr = h2nl(ip_address);
	copy = pak_duplicate(pak);
	if (copy)
	    ipwrite(copy,IPHEADERBYTES(NOPT)+UDPHEADERBYTES+sizeof(bootptype),
		UDP_PROT,ip_address,-1L,odb,NULL,&ipdefaultparams);
    }
    retbuffer(pak);
}

/*
 * bootp_decode_extended_reply
 * Given a bootp response with information in the vendor specific fields,
 * walk those vendor specific fields and act upon any which
 * we care to make use of.
 */

static void bootp_decode_extended_reply (idbtype *idb, bootptype *boot)
{
    uchar tag, *cp, *ep, len;
    ipaddrtype mask;

    cp = boot->bp_vend;
    ep = (uchar *)&boot[1];
    while (cp < ep) {
	tag = *cp++;
	if (tag == BOOTP_VEND_PAD)
	    continue;
	len = *cp++;
	if (&cp[len-1] >= ep)
	    return;
	switch (tag) {
	case BOOTP_VEND_END:
	    return;
	case BOOTP_VEND_MASK:
	    if (len != sizeof(mask)) {
		break;
	    }
	    bcopy(cp, (uchar *)&mask, sizeof(mask));
	    if (reg_invoke_ip_good_address(idb->ip_address, mask, TRUE) &&
		!ip_address_duplicate(idb->ip_address, mask, idb, TRUE)) {
		idb->ip_nets_mask = mask;
		class_resolve(idb);
		if (udp_debug) {
		    buginf("\nBOOTP: Resolved interface %s, "
			   "address %i mask %i",
			   idb->namestring, idb->ip_address, mask);
		}
	    } else {
		if (udp_debug) {
		    buginf("\nBOOTP: Invalid mask of %i for interface %s",
			   mask, idb->namestring);
		}
	    }
	    break;
	default:
	    break;
	}
	cp += len;
    }
}

/*
 * receive_bootreply
 * Given a boot reply, see if we can resolve one of our interfaces.
 */

void receive_bootreply (paktype *pak, register bootptype *boot)
{
    iphdrtype *ip;
    register idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);

    FOR_ALL_SWIDBS(idb) {
	if ((idb->ip_address == 0) && (idb->hwptr->status & IDB_IEEEMASK)) {
	    if (ieee_equal(idb->hwptr->hardware, boot->bp_chaddr)) {
		idb->ip_address = n2hl(boot->bp_yiaddr);
		idb->ip_resolvehost = n2hl(ip->srcadr);
		idb->resolvemethod = RES_BOOTP;
		class_resolve(idb);
		idb->ip_nets_mask = MASK_HOST;
		if (boot->bp_vend_cookie == BOOTP_VEND_COOKIE)
		    bootp_decode_extended_reply(idb, boot);
		errmsg(&msgsym(BOOTP, LINK),
			idb->namestring, idb->ip_address, idb->ip_resolvehost);
		break;
	    }
	}
    }
    retbuffer(pak);			/* discard datagram */
}

/*
 * Extended bootp support
 */

/*
 * asyncbootp_command
 *
 * async-bootp TAG [:HOST] DATA ...
 * 
 * TAG is either a number or a keyword.  It is the thing being
 * specified, e.g. things like log-server, hostname.
 * 
 * Sometimes you need to specify different data for different clients
 * (e.g. if you're giving hostname or bootfile).  :HOSTNAME
 * says that this entry applies only to that host.  (Can IP address
 * or name.)
 * 
 * DATA is a quoted ASCII string (for bootfile name and host name)
 * or a list of 4-byte objects specified as either integers or
 * hostnames (name or IP addresses).  Entries are separated by
 * space.
 * 
 */
void asyncbootp_command (parseinfo *csb)
{
    bootp_spec *spec, *next, *prev, *new;
    ipaddrtype target;
    int function;
    /* 
     * max size of vendor area is 64 bytes, so this is really more than needed
     * for that.  But we need 128 bytes to handle a full size boot file name
     */
#define NUMDATA 32
#define NUMCHARSS 128
    int data[NUMDATA];
    bootp_async_formats format;
    int count = 0;
    char *dp, *sp;

    if (csb->nvgen) {
	nv_asyncbootp(csb->nv_command);
	return;
    }

    switch (GETOBJ(int,1)) {

      case BOOTP_FILE:
      case BOOTP_HOSTNAME:
	dp = (char *)data;
	sp = GETOBJ(string,1);
	for (count = 0; count < (NUMCHARSS-1); count++) {
	    if (*sp == '\0')
		break;
	    *dp++ = *sp++;
	}
	*dp = 0;
	format = BOOTP_TEXT;
	break;

      case BOOTP_TIMEOFFSET:
      case BOOTP_FILESIZE:
	data[0] = GETOBJ(int,2);
	count = 4;
	format = BOOTP_NUMERICAL;
	break;

      case BOOTP_SUBNETMASK:
      case BOOTP_GATEWAY:
      case BOOTP_TIME:
      case BOOTP_DNS:
      case BOOTP_NBNS:
      case BOOTP_LOG:
      case BOOTP_QUOTE:
      case BOOTP_LPR:
      case BOOTP_IMPRESS:
      case BOOTP_RLP:
	if (GETOBJ(paddr,2)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,2)->ip_addr;
	if (GETOBJ(paddr,3)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,3)->ip_addr;
	if (GETOBJ(paddr,4)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,4)->ip_addr;
	if (GETOBJ(paddr,5)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,5)->ip_addr;
	if (GETOBJ(paddr,6)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,6)->ip_addr;
	if (GETOBJ(paddr,7)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,7)->ip_addr;
	if (GETOBJ(paddr,8)->type == ADDR_IP)
	    data[count++] = GETOBJ(paddr,8)->ip_addr;
	count *= 4;
	format = BOOTP_IP;
	break;

      case BOOTP_DELETE:
	/* 
	* no bootp clears everything 
	*/
	for (spec = bootp_spec_list; spec; spec = next) {
	    next = spec->next;
	    free(spec);
	}
	bootp_spec_list = NULL;
	return;

      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	return;
    }

    function = GETOBJ(int,1);
    if (GETOBJ(paddr,1)->type == ADDR_IP)
	target = GETOBJ(paddr,1)->ip_addr;
    else
	target = 0xffffffff;

    if (csb->sense == FALSE) {
	/* no bootp [address] function -- kill this entry */
	for (prev = (bootp_spec *)&bootp_spec_list, spec = bootp_spec_list;
	     spec; spec = next) {
	    next = spec->next;
	    if (spec->function == function &&
		spec->address == target) {
		prev->next = next;
		free(spec);
	    } else
		prev = spec;
	}
	return;
    }

    new = malloc(sizeof(bootp_spec) + count);
    if (!new)
	return;

    new->address = target;
    new->function = function;
    new->size = count;
    new->format = format;
    bcopy(data, new->data, count);

    /* 
     * now put it in the list, in the right order 
     */
    for (prev = (bootp_spec *)&bootp_spec_list, spec = bootp_spec_list;
	 spec && ((spec->function < function) || 
		  ((spec->function == function) && (spec->address < target)));
	 spec = spec->next)
	prev = spec;
    if (! spec)
	prev->next = new;
    else if (spec->function == function && spec->address == target) {
	prev->next = new;
	new->next = spec->next;
	free(spec);
    } else {
	prev->next = new;
	new->next = spec;
    }
}


/*
 * The index specifies the tag values as specified in RFC 1084 except
 * for "bootfile" which is handled special.
 */
#define NTAGS 15
static char *const bootp_tags[NTAGS] = { "bootfile", "subnet-mask", "time-offset",
			    "gateway", "time-server", "ien116-server",
			    "dns-server", "log-server", "quote-server",
			    "lpr-server", "impress-server", "rlp-server",
			    "hostname", "bootfile-size", "nbns-server", };



/*
 * show_asyncbootp
 */
void show_asyncbootp (parseinfo *csb)
{
    bootp_spec *spec;
    int i;

    if (bootp_spec_list)
	printf("The following extended data will be sent in BOOTP responses:\n\n");
    else
	printf("No extended data will be sent in BOOTP responses:\n");
    for (spec = bootp_spec_list; spec; spec = spec->next) {
	if (spec->function < NTAGS)
	    printf("%s", bootp_tags[spec->function]);
	else
	    printf("Tag %d", spec->function);
	if (spec->address != 0xffffffff)
	    printf(" (for address %i)", spec->address);
	if (spec->format == BOOTP_TEXT) {
	    printf(" \"");
	    for (i = 0; i < spec->size; i++)
		if (spec->data[i] == '"')
		    printf("\"\"");
		else
		    printf("%c", spec->data[i]);
	    printf("\"");
	} else {
	    for (i = 0; i < spec->size; i += sizeof (ipaddrtype))
		if (spec->format == BOOTP_NUMERICAL) 
		    printf(" %d", *(long *)&spec->data[i]);
		else
		    printf(" %i", *(long *)&spec->data[i]);
	}
	printf("\n");
    }
}

/*
 * nv_asyncbootp
 */
void nv_asyncbootp (char *nv_command)
{
    bootp_spec *spec;
    int i;

    for (spec = bootp_spec_list; spec; spec = spec->next) {
	if (spec->function < NTAGS)
	    nv_write(TRUE,"%s %s", nv_command, bootp_tags[spec->function]);
	else
	    nv_write(TRUE,"%s %d", nv_command, spec->function);
	nv_add(spec->address != 0xffffffff, " :%i", spec->address);
	if (spec->format == BOOTP_TEXT) {
	    nv_add(TRUE, " \"");
	    for (i = 0; i < spec->size; i++)
		if (spec->data[i] == '"')
		    nv_add(TRUE, "\"\"");
		else
		    nv_add(TRUE, "%c", spec->data[i]);
	    nv_add(TRUE, "\"");
	} else {
	    for (i = 0; i < spec->size; i += sizeof (ipaddrtype))
		if (spec->format == BOOTP_NUMERICAL) 
		    nv_add(TRUE," %d", *(long *)&spec->data[i]);
		else
		    nv_add(TRUE," %i", *(long *)&spec->data[i]);
	}
    }
}


extern boolean lat_debug;

/*
 * bootp_extended
 * Put the extended data into a bootp response
 * Default the gateway and netmask if user hasn't specified it.
 */
void bootp_extended (bootptype *boot, ipaddrtype target)
{
    bootp_spec *spec;
    boolean gateway_done = FALSE;
    boolean netmask_done = FALSE;
    boolean host_done = FALSE;
    int function, size;
    uchar *outpt = boot->bp_vend;
    uchar *endpt = &boot->bp_vend[59];  /* leave one for end byte */
    ipaddrtype gateway, subnetmask, mask;
    idbtype *idb;
    ipaddtype *ptr;

    /* magic cookie */
    boot->bp_vend_cookie = BOOTP_VEND_COOKIE;

    gateway = 0;
    /*
     * this ought to be a standard system routine.  It's like
     * samecable, but needs to return the netmask that goes with
     * the address.
     */
    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled || !idb->ip_address)
	    continue;
	mask = idb->ip_nets_mask;
	if ((idb->ip_address & mask) == (target & mask)) {
	    gateway = idb->ip_address;
	    subnetmask = mask;
	    goto done;
	}
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if ((ptr->address & ptr->mask) == (target & ptr->mask)) {
		gateway = ptr->address;
		subnetmask = ptr->mask;
		goto done;
	    }
	}
    }
 done:

    if (gateway)	 /* better than cheesy pick_defaultaddr already done */
	boot->bp_siaddr = gateway;

    /*
     * host_done indicates that we've found an entry for this
     * function and this host.  That tells us we don't need
     * to use the default (host -1)
     */

    function = -1;
    for (spec = bootp_spec_list; spec; spec = spec->next) {
	if (spec->function != function) {
	    /* new function code -- reinit host_done */
	    host_done = FALSE;
	    function = spec->function;
	}
	if ((! host_done) && 
	    (spec->address == target || spec->address == 0xffffffff)) {
	    /* copy data to packet */
	    if (spec->function == 0) {	/* bootfile is in main packet */
		size = min(spec->size, 127);
		bcopy(spec->data, boot->bp_file, size);
		boot->bp_file[size] = 0;
	    } else if (outpt + 2 + spec->size <= endpt) {
		*outpt++ = spec->function;
		*outpt++ = spec->size;
		bcopy(spec->data, outpt, spec->size);
		outpt += spec->size;
		host_done = TRUE;
		if (spec->function == 3)
		    gateway_done = TRUE;
		if (spec->function == 1)
		    netmask_done = TRUE;
	    }
	}      
    }

    /* now supply defaults if user hasn't done something */

    if (gateway && (! gateway_done) && (outpt + 2 + 4 <= endpt)) {
	*outpt++ = 3;
	*outpt++ = 4;
	bcopy(&gateway, outpt, 4);
	outpt += 4;
    }
    if (gateway && (! netmask_done) && (outpt + 2 + 4 <= endpt)) {
	*outpt++ = 1;
	*outpt++ = 4;
	bcopy(&subnetmask, outpt, 4);
	outpt += 4;
    }

    *outpt++ = 255;			/* end */
    /* spec asks us to clear the rest of the packet */
    while (outpt <= endpt)
	*outpt++ = 0;
}

/*
 * bootp_async_reply
 * See if we wish to reply to bootp requests received on async interfaces.
 * The choice is based on the same decision as SLARP - if our host part is
 * one, assign two to the remote end.
 */

void bootp_async_reply (paktype *request, udptype *udp, bootptype *boot)
{
    iphdrtype *ip;
    idbtype *idb = request->if_input;
    paktype *pak;
    ipaddrtype address = 0L;

    /* Get the remote address */
    address = reg_invoke_ip_peer_addr(idb);
    if (!address) {
        if (udp_debug)
	    buginf("\n%s: No peer address available for BootP reply",
		   idb->namestring);
	return;			/* If we don't have one, get out */
    }

    /*
     * Fill in the BootP reply.
     */
    pak = pak_duplicate(request);
    if (!pak)
	return;
    ip = (iphdrtype *)ipheadstart(pak);

    udp = (udptype *)((char *)ip + (ip->ihl << 2));
    boot = (bootptype *) udp->udpdata;
    boot->bp_op = BOOTREPLY;
    boot->bp_yiaddr = address;
    boot->bp_siaddr = ip_best_local_address(boot->bp_yiaddr, TRUE);
    boot->bp_giaddr = boot->bp_siaddr;
    bootp_extended(boot, boot->bp_yiaddr);
    udp->dport = BOOTPC_PORT;
    udp->sport = BOOTPS_PORT;
    if (udp_debug)
	buginf("\n%s: BootP responding with %i", idb->namestring,
	       boot->bp_yiaddr);
    pak->desthost = boot->bp_yiaddr;
    pak->if_output = idb;

    /*
     * Need a new checksum?
     */
    if (udp->checksum != 0) {
	udp->checksum = 0;
	udp->checksum = CheckSum((uchar *)udp, udp->length,
				 boot->bp_siaddr, boot->bp_yiaddr, UDP_PROT);
    }
    ipwrite(pak, 0, UDP_PROT, boot->bp_siaddr, boot->bp_yiaddr, NULL, NULL,
	    &ipdefaultparams);
}


/*
 * ai_incoming_bootp_request
 * Routine to process an incoming autoinstall request on a serial interface.
 * This routine formulates a reply back to the client.
 */

boolean ai_incoming_bootp_request (
	hwidbtype *idb, 
	paktype *pak, 
	udptype *udph, 
	addrtype *proto_addr)
{
    bootptype *bootph;
    ipaddrtype *server_proto_addr, temp;
    ipaddtype *ptr;
    long blankaddr;

    bootph = (bootptype *) udph->udpdata;
    ptr = NULL;
    server_proto_addr = &pak->if_input->ip_address;
    blankaddr = 0;

	/* 
	 * Server IP addr, entered by server, siaddr
	 */
	if (bcmp((uchar *)&pak->if_input->ip_address, (uchar *)&blankaddr,
						ADDRLEN_IP) == 0) {
	    if (pak->if_input->ip_unnumbered) {
		temp = pak->if_input->ip_unnumbered->ip_address;
		if (udp_debug)
		    buginf("\nReply to autoinstall request, unnumbered address is %i",
							temp);
	    } else {
		if (udp_debug) {
		    buginf("\nReply to incoming autoinstall request: Bootp server error");
		    buginf("\nSerial address not configured, neither is helper address");
		}
		return(FALSE);
	    }

	    bcopy((uchar *)&temp, (uchar *)&bootph->bp_siaddr,ADDRLEN_IP);
	} else {
	    bcopy((uchar *)server_proto_addr, (uchar *)(&bootph->bp_siaddr),
					ADDRLEN_IP);
	    if (udp_debug) {
		buginf("\nReply to autoinstall request, server serial address is %i",
							*server_proto_addr);
	    }
	}
	/* 
	 * Client IP addr, entered by server, yiaddr
	 */
	bcopy((uchar *)&proto_addr->addr, (uchar *)(&bootph->bp_yiaddr),
					ADDRLEN_IP);
	if (udp_debug)
	    buginf("\nReply to autoinstall request, Client address is %i",
						bootph->bp_yiaddr);

	/* 
	 * Client address mask, Use the same as on the BOOTP server
	 */
	if (bcmp((uchar *)&pak->if_input->ip_nets_mask, (uchar *)&blankaddr,
					ADDRLEN_IP) == 0) {
	    if (pak->if_input->ip_unnumbered) {
		temp = pak->if_input->ip_unnumbered->ip_nets_mask;
		if (udp_debug)
		    buginf("\nReply to autoinstall request, unnumbered mask is %i",
							temp);
	    } else {
		if (udp_debug) {
		    buginf("\nReply to incoming autoinstall request: Bootp server error");
		    buginf("\nSerial address/mask not configured, neither is helper address/mask");
		}
		return(FALSE);
	    }

	    bcopy((uchar *)&temp, (uchar *)&bootph->bp_ciaddr,ADDRLEN_IP);
	} else {
	    bcopy((uchar *)&pak->if_input->ip_nets_mask,
			(uchar *)&bootph->bp_ciaddr,ADDRLEN_IP);
	    if (udp_debug)
		buginf("\nReply to autoinstall request, Client serial mask is %i",
						pak->if_input->ip_nets_mask);
	}

	/* Formulate the reply */
	bootph->bp_op = BOOTREPLY;
	udph->sport = BOOTPS_PORT;
	udph->dport = BOOTPC_PORT;

	/* 
	 * Use the user configured "ip helper" address to point
	 * to the configuration server. 
	 * The "autoinstall" user must configure this on the router 
	 * that acts as the BOOTP server.
	 */
	if (pak->if_input->ip_helpernetQ.qhead) {
	    ptr = (ipaddtype *) pak->if_input->ip_helpernetQ.qhead;
	    bcopy((uchar *)&ptr->address,
			(uchar *)&bootph->bp_giaddr, ADDRLEN_IP);
	    if (udp_debug)
		buginf("\nReply to autoinstall request, helper address is %i",
						ptr->address);
	} else {
	    if (udp_debug) {
		buginf("\nReply to incoming autoinstall request: Bootp server error");
		buginf("\nReply to autoinstall request, helper address not configured");
	    }
	    return(FALSE);
	}

	if (martian(bootph->bp_yiaddr, TRUE)) {
	    if (udp_debug) {
		buginf("\nReply to incoming autoinstall request: Bootp server error");
		buginf("\nReply to autoinstall request, Illegal client address is %i", 
						bootph->bp_yiaddr);
	    }
	    return(FALSE);
	}

	/* 
	 * Set things up so that the response can be delivered 
	 * back to the client. 
	 */
    	pak->desthost = bootph->bp_yiaddr;
    	pak->if_output = pak->if_input;

        /*
	 * Need a new checksum?
	 */
        if (udph->checksum != 0) {
	    udph->checksum = 0;
	    udph->checksum = CheckSum((uchar *)udph, udph->length,
				      bootph->bp_siaddr,
				      bootph->bp_yiaddr, UDP_PROT);
	}
    	ipwrite(pak, 0, UDP_PROT, bootph->bp_siaddr, bootph->bp_yiaddr, 
				pak->if_output, NULL, &ipdefaultparams);

return(TRUE);
}
/*
 * ai_incoming_bootp_reply
 * We happen to be the client and we received a BOOTP REPLY.
 *
 * Autoinstall routine that looks at the guts of incoming bootp replies
 * and deciphers an address for the serial interface 0. Then 
 * a default network path is configured. Subsequently, the configuration
 * file is downloaded through the serial interface. 
 */
void ai_incoming_bootp_reply (
	hwidbtype *idb, 
	paktype *pak, 
	udptype *udph, 
	ushort pvc)
{
    char *filename, *unix_filename, *dos_filename, *old_hostname;
    bootptype *bootph;
    int count, hostlen;

    addrtype client_ip_addr;			/* Client Address */
    addrtype config_server_ip_addr;		/* Config Server */
    addrtype client_ip_mask;			/* Address Mask */
    addrtype bootp_server_addr;			/* FR Bootp server */

    boolean tftp_success, hostname_resolved, server_is_DOS;
    parseinfo *csb;
    ipaddrtype network;

    memset(&client_ip_addr, 0, sizeof(addrtype)); 	/* Client Address */
    memset(&client_ip_mask, 0, sizeof(addrtype));	/* Client Mask */
    memset(&config_server_ip_addr, 0, sizeof(addrtype)); /* Config Server */
    memset(&bootp_server_addr, 0, sizeof(addrtype));	/* FR Bootp server */

    /* Initialize all addresses */
    client_ip_addr.type = ADDR_IP;
    client_ip_addr.length = ADDRLEN_IP;
    config_server_ip_addr.type = ADDR_IP;
    config_server_ip_addr.length = ADDRLEN_IP;
    client_ip_mask.type = ADDR_IP;
    client_ip_mask.length = ADDRLEN_IP;
    bootp_server_addr.type = ADDR_IP;
    bootp_server_addr.length = ADDRLEN_IP;

    bootph = (bootptype *) udph->udpdata;
    tftp_success = FALSE;
    hostname_resolved = FALSE;
    server_is_DOS = FALSE;
    count = AUTOINSTALL_LOAD_COUNT;

	/* Retrieve the address mask */
	bcopy((uchar *)(&bootph->bp_ciaddr), &client_ip_mask.addr, 
					ADDRLEN_IP);
	/* Retrieve client address*/
	bcopy((uchar *)(&bootph->bp_yiaddr), &client_ip_addr.addr, 
					ADDRLEN_IP);
	/* Retrieve address of config server*/
	bcopy((uchar *)(&bootph->bp_giaddr), &config_server_ip_addr.addr, 
					ADDRLEN_IP);
	/* Retrieve address of bootp server*/
	bcopy((uchar *)(&bootph->bp_siaddr), 
		(uchar *)&bootp_server_addr.addr, ADDRLEN_IP);

	/* 
	 * Deal with replies as long as the interface doesn't have 
	 * an IP address. BOOTP replies are processed in sequence. 
	 * This allows us to deal with multiple BOOTP replies
	 * If one good reply has been received, others are not processed.
	 */
	if (!reg_invoke_ip_good_address(idb->firstsw->ip_address,
			idb->firstsw->ip_nets_mask,
			(idb->firstsw->ip_nets_mask ? TRUE : FALSE))) {
	    /* 
	     * We try to make use of existing functions to accomplish 
	     * the goal of writing in the IP address and a default route.
	     * The easiest way to do this is to access the functions 
	     * through the existing parser interface. 
	     */
	    csb = malloc(sizeof(parseinfo));
	    if (!csb) {
		datagram_done(pak);
		return;
	    }
	    csb->interface = idb->firstsw;   
	    csb->nvgen = FALSE; 
	    csb->sense = TRUE; 
	    csb->addr1 = client_ip_addr; 
	    csb->addr2 = client_ip_mask;
	    csb->interface->hwptr = idb;
	    csb->interface->ip_address = 0;

	    /* 
	     * Start off by configuring Serial0 with the address 
	     * and the mask returned by the bootp server.
	     */
	    reg_invoke_ip_address_command (csb); 

	    /* 
	     * We assume a linktype of LINK_IP and configure 
	     * a static map on Serial0 that points back to the 
	     * bootp server. This effectively establishes the 
	     * PVC.
	     */
	    reg_invoke_autoinstall_static_map(idb->enctype, idb, 
					&bootp_server_addr, pvc);

	    /* 
	     * setup the default network path so that all outgoing 
	     * traffic can be directed through the serial interface. 
	     * First, set the static route to the bootp server. 
	     * Next, set the static route to the config server through
	     * the bootp server. This is helpful in cases where the 
	     * config server is on a different net than the bootp
	     * server. 
	     */
	    csb->addr1 = bootp_server_addr;
	    reg_invoke_ip_defaultnetwork_command(csb);
	    csb->addr1 = config_server_ip_addr;
	    reg_invoke_ip_defaultnetwork_command(csb);
	    reg_invoke_ip_add_static_route(
			bootph->bp_giaddr, MASK_HOST, 
			bootph->bp_siaddr, idb->firstsw, 0,
			NET_SEXTERIOR, STATIC_DISTANCE, NULL);

	    netsareup = TRUE;
	    process_set_boolean(watched_netsareup, TRUE);
    	    process_sleep_for(10*ONESEC);

	    while ((--count > 0) && (!tftp_success)) {
	    	if (!hostname_resolved) {
	    	    filename = "network-confg";
	    	    tftp_success = reg_invoke_noninteractive_config_read(
				FILE_ACCESS_TFTP,
				filename, &config_server_ip_addr, FALSE);
		    if (!tftp_success) {
	    	    	filename = "cisconet.cfg";
	    	    	tftp_success = reg_invoke_noninteractive_config_read(
				FILE_ACCESS_TFTP,
				filename, &config_server_ip_addr, FALSE);
			if (tftp_success)
			    server_is_DOS = TRUE;
		    }
	    	}
		/*
		 * To resolve a line flap problem caused by LMI starvation
		 * between loading network-confg and hostname-confg,
		 * dismiss for a while so that LMI can get through.
		 */
		if (tftp_success)
		    process_sleep_for(10*ONESEC);

		if (whoami()) {
		    hostname_resolved = TRUE;
		    hostlen = strlen(hostname);
		    if (!server_is_DOS) {
		    	unix_filename = malloc(hostlen + sizeof("-confg"));
		    	if (unix_filename != NULL) {
			    concat(hostname, "-confg", unix_filename);
			    lowercase(unix_filename);
	    	    	    tftp_success = 
				reg_invoke_noninteractive_config_read(
				FILE_ACCESS_TFTP,
				unix_filename, &config_server_ip_addr, FALSE);
		    	    free(unix_filename);
		    	}
		    } else {
		    	dos_filename = malloc(sizeof("hostname.cfg"));
			if (dos_filename != NULL) {
			    /* Max length for DOS filename is 8 */
			    sstrncpy(dos_filename, hostname, 9);
			    old_hostname = dos_filename;
			    concat(old_hostname, ".cfg", dos_filename);
			    lowercase(dos_filename);
	    	    	    tftp_success = 
			    	    reg_invoke_noninteractive_config_read(
				    FILE_ACCESS_TFTP,
				    dos_filename, &config_server_ip_addr, FALSE);
			    free(dos_filename);
			}
		    }
		}
		if (!tftp_success && !hostname_resolved) {
	    	    filename = "router-confg";
	    	    tftp_success = reg_invoke_noninteractive_config_read(
				FILE_ACCESS_TFTP,
				filename, &config_server_ip_addr, FALSE);
		    if (!tftp_success) {
	    	    	filename = "ciscortr.cfg";
	    	    	tftp_success = reg_invoke_noninteractive_config_read(
				FILE_ACCESS_TFTP,
				filename, &config_server_ip_addr, FALSE);
		    }
		}
		if (!tftp_success)
		    process_sleep_for(2 * ONESEC); 
	    }

	    csb->sense = FALSE; 
	    csb->addr1 = bootp_server_addr;
	    reg_invoke_ip_defaultnetwork_command(csb);
	    network = GETOBJ(paddr,1)->ip_addr;
	    reg_invoke_ip_delete_static_route(get_majornet(network), 
				get_majormask(network), NULL);
	    csb->addr1 = config_server_ip_addr;
	    reg_invoke_ip_defaultnetwork_command(csb);
	    network = GETOBJ(paddr,1)->ip_addr;
	    reg_invoke_ip_delete_static_route(get_majornet(network), 
				get_majormask(network), NULL);
	    reg_invoke_ip_delete_static_route(bootph->bp_giaddr, 
				MASK_HOST, NULL);
				
	    free(csb);

	    /* don't need the packet any longer */
	    datagram_done(pak);

	} else {
	    datagram_done(pak);
	}
	reg_invoke_autoinstall_cleanup(idb->enctype, idb);
	reg_invoke_autoinstall_complete(idb->enctype, idb);
}

/*
 * function called via registry for getting DNS addresses for PPP
 * BOOTP clients [Microsoft Windows '95, for example]
 * RFC 1877
 */
static 
void	
bootp_ns_addr_dns (boolean primary, void *addr)
{
    reg_invoke_bootp_ns_addr_dns(primary, (ipaddrtype *)addr);
    return;
}

/*
 * cut code duplication for following two functions
 */
inline	
static	
boolean	
bootp_ns_addr_xxx_bootp (int type, boolean primary, ipaddrtype *addr)
{
    bootp_spec	*spec;
    int		ix;

    /* set index to 0 if primary, 1 otherwise */
    ix = primary ? 0 : 1;   /* should optimize to ix=!primary */

    /* ix is the offset into spec->data */
    ix *= sizeof *addr;

    for (spec = bootp_spec_list ; spec ; spec = spec->next) {
	if ((spec->function == type) && (spec->format == BOOTP_IP)) {
	    if (spec->size > ix) {
		*addr = *(ipaddrtype *)&spec->data[ix];
		if (*addr == -1) {
		    /* never ever return -1 */
		    *addr = 0;
		    return FALSE;
		}
		return TRUE;
	    }
	}
    }
    *addr = 0;	/* failure */
    return FALSE;
}

/*
 * extract DNS server information from the async-bootp server
 */
static	
boolean	
bootp_ns_addr_dns_bootp (boolean primary, ipaddrtype *addr)
{
    return bootp_ns_addr_xxx_bootp(BOOTP_DNS, primary, addr);
}

/*
 * function called via registry for getting NBNS addresses for PPP
 * BOOTP clients [Microsoft Windows '95, for example]
 * RFC 1877
 */
static 
void	
bootp_ns_addr_nbns (boolean primary, void *addr)
{
    reg_invoke_bootp_ns_addr_nbns(primary, (ipaddrtype *)addr);
    return;
}

/*
 * extract NBNS server information from the async-bootp server
 */
static	
boolean	
bootp_ns_addr_nbns_bootp (boolean primary, ipaddrtype *addr)
{
    return bootp_ns_addr_xxx_bootp(BOOTP_NBNS, primary, addr);
}

/*
 * Initialize the registry for exported bootp dns functions in this file.
 * This should be called before the system DNS versions of same.
 */

static void bootp_init_registry (void)
{
    reg_add_bootp_ns_addr(DNS_PORT, bootp_ns_addr_dns,
			   "bootp_ns_addr_dns");
    reg_add_bootp_ns_addr_dns(bootp_ns_addr_dns_bootp,
			      "bootp_ns_addr_dns_bootp");
    reg_add_bootp_ns_addr(NBNS_PORT, bootp_ns_addr_nbns,
			   "bootp_ns_addr_nbns");
    reg_add_bootp_ns_addr_nbns(bootp_ns_addr_nbns_bootp,
				"bootp_ns_addr_nbns_bootp");
}

/*
 * bootp_resolver
 */

static void bootp_resolver (void)
{
    /*
     * Spawn the BootP client process
     */
    process_create(bootp_client, "BootP Resolver", LARGE_STACK, PRIO_NORMAL);
}

/*
 * bootp_client
 * Used by autoinstall over serial ports.
 */

int fork_bootp_client (char *name)
{
    return((int)process_create(bootp_client, name, NORMAL_STACK, PRIO_NORMAL));
}

/*
 * bootp_server_command
 * Parse the 'ip bootp server' command.
 */

void bootp_server_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(ip_bootp_server_enable == FALSE, "no %s", csb->nv_command);
	return;
    }
    ip_bootp_server_enable = csb->set_to_default || csb->sense;
    if (ip_bootp_server_enable) {
	if (router_enable && !system_loading) {
	    bootp_start();
	}
    } else {
	if (bootp_pid)
	    signal_send(bootp_pid, SIGTERM);	
    }
}

/*
 * Parse chain registration array for BOOTP
 */

#define ALTERNATE       NONE
#include "../ip/cfg_async-bootp.h"
LINK_POINT(bootp_async_commands, ALTERNATE);
#undef	ALTERNATE

#define ALTERNATE       NONE
#include "cfg_ip_bootp.h"
LINK_POINT(bootp_ip_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request bootp_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(bootp_async_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(bootp_ip_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * bootp_init
 * Initialize the BOOTP support
 */

static void bootp_init (subsystype *dummy)
{
    reg_add_ip_autoaddr_load(bootp_resolver, "bootp_resolver");
    reg_add_fork_bootp_client(fork_bootp_client, "fork_bootp_client");
    parser_add_command_list(bootp_chain_init_table, "bootp");
    bootp_init_registry();
    bootp_start();
}

/*
 * BOOTP subsystem header
 */

#define BOOTP_MAJVERSION 1
#define BOOTP_MINVERSION 0
#define BOOTP_EDITVERSION  1

SUBSYS_HEADER(bootp, BOOTP_MAJVERSION, BOOTP_MINVERSION, BOOTP_EDITVERSION,
	      bootp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
