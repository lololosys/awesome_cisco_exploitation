/* $Id: ip_arp.c,v 3.19.6.19 1996/08/28 12:49:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_arp.c,v $
 *------------------------------------------------------------------
 * ip_arp.c -- IP specific ARP support
 *
 * 16-July-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_arp.c,v $
 * Revision 3.19.6.19  1996/08/28  12:49:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.19.6.18  1996/08/06  08:27:15  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.19.6.17  1996/07/22  15:50:08  amajumda
 * CSCdi61364:  Bridging IP over SMDS fails
 * Branch: California_branch
 * Covers the case where the router is configured as an ip host. Added an
 * additional condition to check whether bridging was in fact configured
 * on
 * an smds interface. The BRIDGING_IF macro returns true when a router is
 * configured as an ip host irrespective of whether bridging is configured
 * or
 * not.
 *
 * Revision 3.19.6.16  1996/07/19  18:52:52  dawnli
 * CSCdi62793:  incorrect arp entry over ATM when running SMDS
 * Branch: California_branch
 *
 * Revision 3.19.6.15  1996/06/29  00:11:13  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.19.6.14  1996/06/24  16:04:33  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.19.6.13  1996/06/18  01:44:50  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.19.6.12  1996/05/31  00:22:17  ahh
 * CSCdi59135:  Misaligned access in ip_arp_filter
 * Source address is on a short boundary, so don't access it directly.
 * Branch: California_branch
 *
 * Revision 3.19.6.11  1996/05/10  01:32:54  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.19.6.10  1996/05/09  14:30:49  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.19.6.9  1996/05/04  01:02:09  wilber
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
 * Revision 3.19.6.8  1996/04/29  10:02:38  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.19.6.7.2.1  1996/04/27  07:04:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.19.6.7  1996/04/25  23:13:58  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.19.6.6  1996/04/22  22:21:59  rja
 * CSCdi51047:  ARP request is ignored on transparent bridging over X25
 * Branch: California_branch.  Correct commit error from 19 Apr 1996.
 *
 * Revision 3.19.6.5  1996/04/19  17:08:39  rja
 * CSCdi51047:  ARP request is ignored on transparent bridging over X25.
 * Branch: California_branch
 *
 * Revision 3.19.6.4  1996/04/15  21:15:45  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.19.6.3  1996/04/09  14:47:59  albright
 * CSCdi51812:  IP routing over Ethernet Simplex interfaces is
 * non-functional
 * Branch: California_branch
 *
 * Revision 3.19.6.2  1996/04/07  01:15:26  speakman
 * CSCdi48117:  ARPs for local non-bridge-group IP addresses dont get
 * bridged
 * Branch: California_branch
 * Modify the ARP code to bridge ARP's for any IP address not associated
 * with an interface in the same bridge group.  This permits CRB to be
 * used in a looped-back configuration to provide de facto IRB
 * functionality.
 *
 * Revision 3.19.6.1.8.2  1996/04/08  23:21:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * While we're at it, buff and polish an unclear statement in
 * ip_arp_add_interface.
 *
 * Revision 3.19.6.1.8.1  1996/04/08  23:14:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Missed in commit of TR_LANE.
 *
 * Revision 3.19.6.1  1996/03/18  20:16:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.12.4  1996/03/16  06:57:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.12.3  1996/03/13  01:37:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.10.12.2  1996/03/07  09:45:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.12.1  1996/02/20  00:58:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.19  1996/03/01  20:20:52  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.18  1996/02/23  16:42:01  rja
 * CSCdi49596:  IP: proxy-arp replies dropped
 * Move check into correct location (SMDS case) from incorrect location
 * that patch(1) had inserted it into for CSCdi46066.
 *
 * Revision 3.17  1996/02/21  00:40:41  bcole
 * CSCdi48436: ARP: spurious access when gratuitous ARPing over SMDS
 * interface
 *
 * Don't attempt to send unsolicited ARPs out interfaces which do not
 * support ARP.  Avoid dereferencing NULL when looking for an interface's
 * SMDS address.
 *
 * Revision 3.16  1996/02/15  23:06:40  rja
 * CSCdi46066:  SMDS ARP table entry sometimes is incorrect
 *  Add checks to ip_arp_filter() to ensure that ARP Reply messages that
 * aren't of the same arp type as in use on the receiving interface get
 * dropped rather than processed as if they were correct.
 *
 * Revision 3.15  1996/02/13  19:03:47  rja
 * CSCdi43596:  ARP throttling not working
 * Replumbed IPv4 ARP Request software so that the ARP throttling
 * sanity checks always occur and aren't accidentally bypassed by
 * some callers.  Removed one now dead function in ip_arp.c
 *
 * Revision 3.14  1996/02/13  08:13:37  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.13  1996/02/07  16:12:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/02/01  06:04:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/30  17:30:03  bcole
 * CSCdi47620:  IP ARP does not support full SMDS addresses
 *
 * Fix ARP code to handle 8 byte SMDS addresses in SOME cases.
 * (Code still assumes 6 byte layer 2 addresses in other less critical
 * cases.)
 *
 * Revision 3.10.16.1  1996/03/26  02:04:38  ddecapit
 * Branch: TRlane_branch
 * Allow IP ARP over TR LANE to use LLC/SNAP
 *
 * Revision 3.10  1996/01/05  04:55:58  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.9  1995/12/28  03:22:51  bcole
 * CSCdi46070: AppleTalk AURP appears broken - no routes propogated thru
 * tunnel
 *
 * Weaken IP-ARP interrupt level check - allow incomplete ARP table entries
 * while interrupts are disabled (but still not when on interrupt stack).
 *
 * Revision 3.8  1995/12/23  10:45:13  tli
 * CSCdi46098:  ARP: more sanity checking against bogon hosts
 *
 * Revision 3.7  1995/12/18  23:10:03  bcole
 * CSCdi44821:  IP ARP attempts to allocate memory from interrupt level
 *
 * Don't attempt to create incomplete ARP table entries at interrupt
 * level.  Also fix code that was attempting to filter dup. IP addr arps
 * so that it actually does filter these instead of just printing that it
 * was.
 *
 * Revision 3.6  1995/11/30  22:58:12  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.5  1995/11/26  08:15:41  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *         "no ip address" should not be generated on subinterfaces,
 *         creates backwards compatibility problems with earlier releases.
 *
 *         Explicitly indicate cache entries that are associated with a
 *         dot10 vLAN subinterface.
 *
 * Revision 3.4  1995/11/22  17:47:06  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/19  06:05:51  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Don't send gratuitous ARP on the interface the peer is on.
 * Rename ip_send_gratuitous_arps() as ip_arp_send_gratuitous_arps().
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.2  1995/11/17  09:33:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:04:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/26  07:28:51  shaker
 * CSCdi42676: %LINK-3-BADENCAP: Interface Fddi2/0, Bad encapsulation code
 * (33). Add parenthesis instead relying on someone's memory of presidence.
 *
 * Revision 2.13  1995/10/23  20:57:04  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.12  1995/10/20  18:05:52  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.11  1995/10/09  20:59:02  pst
 * CSCdi40821:  nagative values in arp timeout configuration
 *
 * Revision 2.10  1995/09/25  08:31:54  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.9  1995/09/11  23:49:08  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.8  1995/08/25 19:43:46  gramired
 * CSCdi37963:  router coulnt build arp table for the router across rtb
 * This will take care of ether/fddi networks too.
 *
 * Revision 2.7  1995/08/24  00:47:16  gchristy
 * CSCdi37313:  IP mobility crashes router
 * - punt if ip_arp_incomplete() fails
 *
 * Revision 2.6  1995/08/23  22:51:45  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         ARP table entries for ATM should be created as ATM, not
 *         SMDS
 *
 * Revision 2.5  1995/08/17  17:27:13  gramired
 * CSCdi37963:  router coulnt build arp table for the router across rtb
 *
 * Revision 2.4  1995/08/04  06:43:29  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.3  1995/06/17 04:28:15  dkatz
 * CSCdi36033:  ARP shouldnt use Active Timers
 * Use managed timers instead.
 *
 * Revision 2.2  1995/06/09  13:05:23  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:56:54  hampton
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
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ip.h"
#include "ip_media.h"
#include "ipaddress.h"
#include "config.h"
#include "logger.h"
#include "../ip/msg_ip.c"		/* Not a typo, see logger.h */
#include "parser.h"
#include "../if/arp_debug.h"
#include "ieee.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../if/arp.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "../wan/fr_arp.h"
#include "../iprouting/route.h"
#include "parser_defs_arp.h"
#include "../iprouting/ipmobile.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "mgd_timers.h"

#include "../vlan/vlan.h"

static const char nostr[] = "no ";


/*
 * Definitions
 */
#define MAXRETRIES 4			/* maximum number of retries */
#define DISMISSTIME 1			/* seconds to dismiss */

typedef struct ip_arp_decoded_ {
    boolean reply_flag;		/* TRUE if we need to send a reply */
    idbtype *arpidb;		/* pointer to interface that received ARP */
    ipaddrtype spro;		/* source protocol address */
    ipaddrtype tpro;		/* target protocol address */
    uchar shdw[HL_SMDS_RFC];	/* source hardware address */
    uchar thdw[HL_SMDS_RFC];	/* target hardware address */
    uchar rthdw[HL_SMDS_RFC];	/* target hardware address in reply */
    int encmethod;		/* encapsulation method */
    ushort arhln;		/* Length of the hardware addresses */
    ushort opcode;
} ip_arp_decoded;

/*
 * Storage
 */

/*
 * ip_arp_forus
 * Is this ARP packet for any of our ip addresses
 * If a span pointer is provided, a matching interface
 * must also be a member of the same bridge group as
 * the interface on which the ARP was received.
 */

boolean ip_arp_forus (ipaddrtype taddr, ipaddrtype saddr,
		      ushort encap, spantype *span)
{
    idbtype *idb;
    arptype *ptr;

    /*
     * Check if the target IP address matches one of the
     * interfaces in the same bridge group as the interface
     * on which the ARP was received.
     */
    if (span != NULL) {

	ipaddr_tabletype *ipaddr_tbl;

	FOR_IPADDR_HASH_CHAIN(taddr, ipaddr_tbl) {
	    if ((ipaddr_tbl->address == taddr) &&
		(ipaddr_tbl->swidb) &&
		(ipaddr_tbl->swidb->tbridge_on_swidb) &&
		(ipaddr_tbl->swidb->span_ptr == span))
		return(TRUE);
	}

    /*
     * Check if the target IP address matches any one of our
     * interface addresses or an alias
     */
    } else if (ip_ouraddress(taddr)) {
	return(TRUE);
    }

    /* 
     * This could be arp reply announcing
     * change of hardware address or address
     * duplicate address defense packet. In
     * either case both target and source will
     * be same.
     */
    if (taddr == saddr)
	return(TRUE);

    /*
     * See if some process wants this packet
     */
    FOR_ALL_SWIDBS(idb) {
	if (reg_invoke_ip_arp_for_proto(idb, taddr))
	    return(TRUE);
    }

    /*
     * May also be a request for one of our alias ARP entries.  If
     * encapsulation was not specified, skip the check.
     */
    if (encap == ET_NULL)
	return(FALSE);
    ptr = arp_table_lookup(LINK_IP, taddr, NULL);
    if (!ptr)
	return(FALSE);
    if (ptr->flag == ARP_EF_DUP)
	return(FALSE);
    if ((ptr->mode == ARP_ALIAS) && (ptr->enctype == encap))
	return(TRUE);
    return(FALSE);
}

/*
 * ip_arp_ieee_mcast
 * Check multicast bit of IEEE MAC address to determine if it is a 
 * broadcast or multicast.
 */

static inline boolean ip_arp_ieee_mcast (hwidbtype *hwidb, uchar *hdw)
{
    if ((hwidb->status & (IDB_ETHER|IDB_LEX|IDB_FDDI)) && (hdw[0] & 0x01))
	return(TRUE);
    if ((hwidb->status & IDB_TR) && (hdw[0] & 0x80))
	return(TRUE);
    return(FALSE);
}

/*
 * ip_arp_gethwidb
 * Get hwidb associtated with a particular layer 2 address, return NULL
 * if none found.
 */

static hwidbtype *ip_arp_gethwidb (uchar *target, ushort len)
{
    hwidbtype *idb;

    /*
     * Make sure that we stay within bounds.
     */
    if (len > HL_SMDS_RFC)
	return(NULL);
    FOR_ALL_HWIDBS(idb)
      if (bcmp(idb->hardware, target, len) == 0)
	return(idb);
    return(NULL);
}

/*
 * ip_arp_bridge_or_absorb
 * If the packet is not for us, and we are bridging IP,
 * then hand the packet back to the bridging code.
 *
 * If we aren't bridging IP, but we are bridging other protocols,
 * the bridging code might have handed us a packet that really isn't for
 * us.  (A unicast ARP whose destination MAC address is not ours).
 *
 * If the ARP packet really isn't for us, or we are not running IP on any
 * software interface then drop it on the floor.
 *
 * Returns TRUE to indicate packet was absorbed somehow (either by
 * being freed or passed to bridging).
 * Returns FALSE otherwise.
 */

static boolean ip_arp_bridge_or_absorb (paktype *pak,
					idbtype *idb,
					ipaddrtype taddr,
					ipaddrtype saddr)
{
    hwidbtype *hwidb;
    idbtype   *swidb;
    hwaddrtype hdaddr;
    boolean    routing_ip_on_hwidb = FALSE;
    
    /*
     * There used to be another argument to this function called
     * bridge_allowed which was always TRUE, so it's toast.
     *
     * In making the decision to bridge or absorb an ARP, we want
     * to accommodate the situation in which a bridged interface
     * has been looped back to a routed interface to get de facto
     * IRB functionality using just CRB.
     *
     * What we are looking for here are ARP's that are eligible
     * for bridging.  So first we check that IP is being bridged
     * on the receiving interface.  If so, we are interested in
     * bridging any ARP received on a bridged interface that is
     * NOT for an interface in the bridge group.
     *
     * The test for ARP's destined only for the bridge group lets us
     * bridge ARP's that might be for some other interface on the box
     * back out the other bridged interfaces where they could be
     * received on a routed interface in the same box attached to the
     * same cable.
     */
    if (!global_irb_enable) {
	if (BRIDGING_IF(idb, LINK_IP)) {
	    if (!ip_arp_forus(taddr, saddr, pak->enctype, idb->span_ptr)) {
		net_bridge_enqueue(pak);
		return(TRUE);
	    }
	    if (taddr == saddr) {
		/*
		 * Gratuitous arp.
		 * Make a copy and enqueue it to the bridgeQ.
		 */
		paktype *pakcopy;
		pakcopy = pak_clip(pak, pak->datagramsize);
		if (pakcopy) {
		    pakcopy->if_input = pak->if_input;
		    net_bridge_enqueue(pakcopy);
		}
		/* 
		 * Fall through to process this gratuitous arp so
		 * arp cache can be updated if necessary.
		 */
	    }
	}
    } else {
	/*
	 * If IRB(Integrated Routing and Bridging) is enabled and
	 * the input interface is the BVI(Bridge-group Virtual
	 * Interface), we want to both bridge and receive it.
	 */
	if (is_bvi(idb->hwptr)) {
	    /*
	     * Both bridge and route IP
	     */
	    paktype *pakcopy;
	    pakcopy = pak_clip(pak, pak->datagramsize);
	    if (pakcopy) {
		/*
		 * We will bridge this copy. Restore the original
		 * input since we will enqueue it to bridgeQ and
		 * don't want to flood the packet back to the 
		 * original received interface
		 */
		if (pak->irb_rcvd_input && 
		    (pak->if_input != pak->irb_rcvd_input)) {
		    pakcopy->if_input = pak->irb_rcvd_input;
		} else {
		    pakcopy->if_input = pak->if_input;
		}
		reg_invoke_raw_enqueue(LINK_BRIDGE, pakcopy);
	    }
	    /* Process the original one */
	    return(FALSE);
	} else {
	    if (ROUTING_IF(idb, LINK_IP) ||
		ip_arp_forus(taddr, saddr, pak->enctype, 
			     BRIDGING_IF(idb, LINK_IP) ? 
			     idb->span_ptr : NULL)) {
		/*
		 * Receive it if
		 * 1. Routing IP only 
		 * OR 
		 * 2. For us:
		 * - If we are bridging, look at the bridged interfaces
		 *   of this bridge group only by passing the span_ptr
		 * - If we are not bridging, look at all interfaces
		 *
		 * Note: When IRB is enabled, bridge or route a given
		 * protocol is independent; Not routing IP doesn't 
		 * necessarily mean we are bridging IP, so we want to
		 * check if we are bridging before pass the span_ptr.
		 */
		return(FALSE);
	    } else {
		/*
		 * Not for us AND Bridging IP only
		 */
		net_bridge_enqueue(pak);
		return(TRUE);
	    }
	}
    }

    /* vLAN Routing, accept ARPs if an IP address is on any software IDB */ 

    FOR_ALL_SWIDBS_ON_HW(idb->hwptr, swidb) {

        if (swidb->ip_enabled) {

	    routing_ip_on_hwidb = TRUE;
	    break;
	}
    }

    if (routing_ip_on_hwidb == FALSE) {

        retbuffer(pak);
        return(TRUE);
    }

    /*
     * Only worry about being fed ARP packets not for us if we are
     * bridging.
     */
    if (!RUNNING_IF(LINK_BRIDGE, idb))
	return(FALSE);
    hwidb = idb->hwptr;
    if ((hwidb->status & IDB_IEEEMASK) ||
	(is_cpp(hwidb)) ||
	(is_ppp(hwidb) && hwidb->ppp_bridge_ip)) {
	 
	(*hwidb->extract_snpa)(pak, &hdaddr, DESTINATION);
	if (ip_arp_ieee_mcast(hwidb, hdaddr.addr))
	    return(FALSE);
	/*
	 * As an optimization, first check against incoming interface's
	 * MAC address.  Then, check all interface's MAC addresses.
	 */
	if (ieee_equal(hwidb->hardware, hdaddr.addr))
	    return(FALSE);
	if (ip_arp_gethwidb(hdaddr.addr, hdaddr.length) != NULL)
	    return(FALSE);
	/*
	 * Packet is a unicast, and yet, not directed at us.
	 */
	retbuffer(pak);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * ip_arp_mac_allowed
 * Check to see if hardware address is a multicast.
 * Note Ethernet and Token ring have the bit indicating multicasts 
 * in different places.
 */

static boolean ip_arp_mac_allowed (hwidbtype *hwidb, uchar *hdw)
{
    ulong functional;

    if (BRIDGING_IF(hwidb->firstsw, LINK_IP)) 
        return(TRUE);
    if ((hwidb->status & (IDB_ETHER | IDB_FDDI)) && (hdw[0] & 0x01))
	return(FALSE);
    if ((hwidb->status & IDB_TR) && (hdw[0] & TR_GROUP)) {
	/*
	 * Special case a set of functional addresses, in case this
	 * system is interoperating with HSRP.
	 */
	if ((GETSHORT(&hdw[0]) != ((TR_GROUP|TR_LOCAL) << 8)))
	    return(FALSE);
	functional = GETLONG(&hdw[2]);
	if (functional & ~(TRFUNC_STANDBY))
	    return(FALSE);
	if (functional & (TRFUNC_STANDBY))
	    return(TRUE);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ip_arp_valid_hwlen
 * Validate hwlen in ARP packet.  Return 0 if invalid.
 */

static ushort ip_arp_valid_hwlen (rfc_arp *rfcptr)
{
    ushort explen;
    
    switch (rfcptr->arhrd) {
    case HW_ETH:
    case HW_802:
	explen = HL_ETH;
	break;
    case HW_SMDS_RFC:
	explen = HL_SMDS_RFC;
	break;
    case FR_ARHRD:
	explen = FR_ARHLN;
	break;
    default:
	return(0);
    }
    if (rfcptr->arhln != explen)
	return(0);
    return(explen);
}

/*
 * ip_arp_filter
 * Perform consistency/sanity checks.  Return TRUE if okay.
 * Returns FALSE if something wrong.  Packet has been absorbed.
 */

static boolean ip_arp_filter (paktype *arppak)
{
    idbtype *idb = arppak->if_input;
    hwidbtype *hwidb = idb->hwptr;
    hwidbtype *src_hdw;
    idbtype *connected_idb;
    idbtype *first_hop;
    ipaddrtype source;
    fr_rfc_arp *fr_rfcptr;
    smds_arp *smds_rfcptr;
    rfc_arp *rfcptr;
    ushort mac_len;
    uchar *srcprop, *tprop, *thdw;

    rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);
    if (!ip_arp_mac_allowed(hwidb, rfcptr->ipshdw)) {
	retbuffer(arppak);
	return(FALSE);
    }

    /*
     * We got here because rfcptr->arpro == PR_IP.
     * Make sure protocol length is correct as well.
     */
    if (rfcptr->arpln != PL_IP) {
	retbuffer(arppak);
	return(FALSE);
    }

    mac_len = ip_arp_valid_hwlen(rfcptr);
    if (mac_len == 0) {
	retbuffer(arppak);
	return(FALSE);
    }
    /* zap arps with our hardware address sourced in them -- these can
     * get bridged back to us by cretinous spanning trees.
     *
     * CSCdi10269 - Just disallow bridging of the arp by clearing
     * bridge_allowed. Then continue to check the routing protocols.
     * This allows for self ping.
     */

    src_hdw = ip_arp_gethwidb(rfcptr->ipshdw, mac_len);

    /*
     * On bridged interfaces, complain only about ARP's sourced
     * by interfaces in the same bridge group.  This allows us
     * to bridge ARP's received on a bridged interface that is
     * looped back to a routed interface on the same box to
     * provide de facto IRB using just CRB.
     */
    if (BRIDGING_IF(idb, LINK_IP) && src_hdw) {

	if (!src_hdw->tbridge_on_hwidb) {
	    /*
	     * The source is not even bridging; don't complain.
	     */
	    src_hdw = NULL;

	} else {

	    idbtype *src_sfw;

	    FOR_ALL_SWIDBS_ON_HW(src_hdw, src_sfw)
		if ((src_sfw->tbridge_on_swidb) &&
		    (src_sfw->span_ptr == idb->span_ptr))
		    break;
	    if (NULL == src_sfw)
		/*
		 * The source is bridging but not in the same
		 * bridge group; don't complain.
		 */
		src_hdw = NULL;
	}
    }

    if (src_hdw) {
	if (arp_debug) {
	    /*
	     * Calculate proper starting addresses for ARP header fields,
	     * taking into account variable length ARP addresses.
	     */
	    srcprop = rfcptr->ipshdw + mac_len;
	    thdw = srcprop + sizeof(ipaddrtype);
	    tprop = thdw + mac_len;
	    buginf("\nIP ARP %s filtered src %i %e, dst %i %e it's our "
		   "address",
		   rfcptr->opcode == OP_REQ ? "req" : "rep",
		   GETLONG(srcprop), rfcptr->ipshdw,
		   GETLONG(tprop), thdw);
	}
	/*
	 * This arp is weird, just bitch and bail.  Either there is another
	 * node out there using our mac address (which should be corrected
	 * soonest) or this node is generating a broadcast arp looking for
	 * someone that goes out all our interfaces and it came back to us
	 * through a topology loop.  
	 */
	retbuffer(arppak);
	return(FALSE);
    }
    
    /*
     * Switch on hardware type.
     */
    switch (rfcptr->arhrd) {

      /*
       * The IEEE media.
       */
      case HW_ETH:
      case HW_802:
	if (ip_arp_bridge_or_absorb(arppak, idb, rfcptr->iptpro,
				    GETLONG(&rfcptr->ipspro)))
	    return(FALSE);

	 
	/* 
	 * If we are netbooting, don't filter proxy ARP.
	 * There are cases where first_hop fails so get out now. 
	 */
	if (!systeminit_flag || !ip_router_running)
	    return(TRUE);

	/*
	 * If it's from net directly connected to this interface, accept.
	 * Also if associated with a different subinterface of the same
	 * physical interface then write the software idb here.
	 */

	connected_idb = samecable(GETLONG(&rfcptr->ipspro));

	if ((connected_idb) && (connected_idb->hwptr == idb->hwptr)) {

	    arppak->if_input = connected_idb;
	    return(TRUE);
	}

	/*
	 * If mobile arp is enabled and it likes this address, accept.
	 * If it's from net routed through this int, accept, in case we're
	 * hearing a proxy arp from another router.
	 */
	source = GETLONG(&rfcptr->ipspro);

	if (martian(source, TRUE)) {
	    if (arp_debug)
		buginf("\nIP ARP %s filtered src %i %e, dst %i %e "
		       "martian source",
		       rfcptr->opcode == OP_REQ ? "req" : "rep",
		       GETLONG(&rfcptr->ipspro), rfcptr->ipshdw,
		       rfcptr->iptpro, rfcptr->ipthdw);
	    retbuffer(arppak);
	    return(FALSE);
	}

	if (reg_invoke_ip_mobile_arp_accept(idb, source))
	    return(TRUE);

	first_hop = reg_invoke_ip_first_hop(&source, source, IP_PATH_DEFFLAGS);
	if (first_hop == idb->output_if)
	    return(TRUE);

	if (arp_debug)
	    buginf("\nIP ARP %s filtered src %i %e, dst %i %e wrong cable, interface %s",
		   rfcptr->opcode == OP_REQ ? "req" : "rep",
		   GETLONG(&rfcptr->ipspro), rfcptr->ipshdw,
		   rfcptr->iptpro, rfcptr->ipthdw, idb->namestring);
	retbuffer(arppak);
	return(FALSE);
	break;

      /*
       * SMDS
       */
      case HW_SMDS_RFC:
	smds_rfcptr = (smds_arp *)smds_arp_headstart(arppak);
	if (ip_arp_bridge_or_absorb(arppak, idb, smds_rfcptr->iptpro,
				    smds_rfcptr->ipspro))
	    return(FALSE);
	return(TRUE);
	break;

       /* 
	* Frame Relay
	*/
      case FR_ARHRD:
	fr_rfcptr = (fr_rfc_arp *)fr_rfc_arp_headstart(arppak);

        /*
         * Make sure this ARP really came from a frame relay before
         * we make too many assumptions about the format of the
         * packet.  (CSCdj05170)
         */
        if (!idb_is_framerelay(idb)) {
            if (arp_debug)
                buginf("\nIP ARP: %s filtered - wrong ARP format for media,\n"
                    "                 src %i, dst %i, interface %s",
                fr_rfcptr->opcode == OP_REQ ? "ARP request" : "ARP reply",
                fr_rfcptr->ipspro, fr_rfcptr->iptpro, idb->namestring);
            retbuffer(arppak);
            return(FALSE);
        }

	if (ip_arp_bridge_or_absorb(arppak, idb, fr_rfcptr->iptpro,
				    fr_rfcptr->ipspro))
	    return(FALSE);
	return(TRUE);
	break;
    }

    /*
     * The unknown packet.  Discard it.
     */
    retbuffer(arppak);
    return(FALSE);
}

/*
 * ip_arp_canonicalize
 * Minimize differences between IP ARP over different media.
 */

static void ip_arp_canonicalize (paktype *arppak, ip_arp_decoded *canptr)
{
    idbtype *idb = arppak->if_input;
    rfc_arp *rfcptr;
    smds_arp *smds_rfcptr;
    fr_rfc_arp *fr_rfcptr;
    fr_idb_struct_type *fr_idb;
    ushort dlci; 

    rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);
    canptr->encmethod = arppak->enctype; /* get encapsulation method */

    if (rfcptr->opcode == OP_REQ) {
	arp_traffic.reqrcv++;
	canptr->reply_flag = TRUE;	/* reply to requests */
    } else {
	arp_traffic.reprcv++;
	canptr->reply_flag = FALSE;
    }
    canptr->arhln = rfcptr->arhln;
    canptr->arpidb = idb;		/* remember input interface */
    canptr->opcode = rfcptr->opcode;

    /*
     * Set up IP and hardware addresses in the standard forms.
     */
    switch (rfcptr->arhrd) {
      /*
       * The IEEE media.
       */
      case HW_ETH:
      case HW_802:
	canptr->tpro = rfcptr->iptpro;     /* set target protocol address */
	/*
	 * Set source protocol address.
	 */ 
	canptr->spro = GETLONG(&rfcptr->ipspro); 

	if (is_bvi(idb->hwptr) && arppak->irb_rcvd_input &&
	    is_tokenring(arppak->irb_rcvd_input->hwptr)) {
	    
	    uchar swapped_addr[IEEEBYTES];
	    
	    /*
	     * If input interface is a BVI(Bridge-group Virtual Interface)
	     * and the original received interface is a token, bit-swap the 
	     * src hardware address(ipshdw) *inside* the packet since 
	     * the BVI is a virtual-ether interface,
	     * - we want the mac in ARP cache is in canonical format
	     * - we may decide to send ARP resopnse by using this packet
	     *   later, then we copy rfcptr->ipshdw as the destination MAC.
	     *   For BVI, it should in canonical(ether) format
	     *
	     * This is for IRB(Integrated Routing and Bridging)
	     */
	    ieee_swap(rfcptr->ipshdw, swapped_addr);
	    ieee_copy(swapped_addr, rfcptr->ipshdw);
	    ieee_copy(swapped_addr, canptr->shdw); /* set src hdw addr */
	} else {
	    ieee_copy(rfcptr->ipshdw,canptr->shdw); /* set src hdw addr */
	}
	ieee_copy(idb->hwptr->hardware,canptr->thdw);
	ieee_copy(rfcptr->ipthdw,canptr->rthdw);
	break;

      /*
       * SMDS
       */
      case HW_SMDS_RFC:
	smds_rfcptr = (smds_arp *)smds_arp_headstart(arppak);
	canptr->encmethod = ET_SMDS;
	canptr->tpro = smds_rfcptr->iptpro;     /* set target proto address */
	/*
	 * Set source proto address.
	 */
	canptr->spro = GETLONG(&smds_rfcptr->ipspro);  
	bcopy(smds_rfcptr->ipshdw, canptr->shdw, HL_SMDS_RFC);
	if (canptr->arpidb->vaddr)
	    bcopy(canptr->arpidb->vaddr->addr,canptr->thdw, HL_SMDS_RFC);
	else
	    bzero(canptr->thdw, HL_SMDS_RFC);
	bcopy(smds_rfcptr->ipthdw,canptr->rthdw, HL_SMDS_RFC);
	break;

       /*
	* Frame Relay
	*/
      case FR_ARHRD:
	fr_rfcptr = (fr_rfc_arp *)fr_rfc_arp_headstart(arppak);
	canptr->encmethod = ET_FRAME_RELAY;	/* get encapsulation method */
	fr_idb = idb->hwptr->frame_relay_stuff;
	canptr->tpro = fr_rfcptr->iptpro;     /* set target protocol address */
	/*
	 * Set source protocol address.
	 */
	canptr->spro = GETLONG(&fr_rfcptr->ipspro);
	/*
	 * Since the other end didn't know its DLCI,
	 * grab it from the encapsulation
	 */
	dlci = DLCI_TO_NUM(*((ushort *)arppak->datagramstart));
	bcopy((uchar *)&dlci, canptr->shdw, fr_rfcptr->arhln);
	/* Fill in the packet source FR address */
	bcopy((uchar *)&dlci, (uchar *)&fr_rfcptr->ipshdw, fr_rfcptr->arhln); 
	ieee_zero(canptr->thdw);
	bcopy((uchar *)&fr_rfcptr->ipthdw, canptr->rthdw, fr_rfcptr->arhln);
	break;
    }
    if (arp_debug)
	buginf("\nIP ARP: rcvd %s src %i %e, dst %i %s",
	       rfcptr->opcode == OP_REQ ? "req" : "rep",
	       canptr->spro, canptr->shdw, canptr->tpro,
	       canptr->arpidb ? canptr->arpidb->namestring : "");
}

/*
 * ip_arp_assist
 * Return TRUE if we're going to reply to an ARP request.
 *
 * We always reply to an ARP if we are the target.
 *
 * If the ARP is for a host not on our subnet and we are a gateway,
 * we reply if we are the path to that host.  This allows hosts
 * that don't know about subnets to function correctly.
 */

boolean ip_arp_assist (ipaddrtype source, ipaddrtype target, idbtype
		       *input, uint protocol, int encap, uchar *thdw)
{
    arptype *ptr;
    ipaddrtype myaddr, gwaddr;
    idbtype *idb;
    int numroutes;
    boolean allow_proxy;

    ptr = arp_table_lookup(LINK_IP, target, input);
    if (ptr) {
	if (ptr->flag  == ARP_EF_DUP)
	    return(FALSE);
	/*
	 * Reply if we have a alias ARP entry for
	 * this guy, which indicates we are supposed to do ARP service for him.
	 * Or some protocol we are running is maintaining the ARP entry on
	 * our behalf.
	 */
	if (((ptr->mode == ARP_ALIAS) && (ptr->enctype == encap)) ||
	    reg_invoke_ip_arp_for_proto(input, target)) {
	    /*
	     * Return MAC address to use as source hardware address in ARP
	     * reply, since we expect to generate a proxy ARP response.
	     */
	    if (thdw)
		ieee_copy(ptr->hardware, thdw);
	    return(TRUE);
	}
    }

    /*
     * Respond if aimed at one of our interface or alias addresses
     */
    if (BRIDGING_IF(input, LINK_IP)) {
	if (ip_arp_forus(target, (ulong)0, ET_NULL, input->span_ptr))
	    return(TRUE);
    } else {
	if (ip_ifaddress(input, target) || ip_aliasaddress(target))
	    return(TRUE);
    }

    /*
     * We are doing proxy ARP from here on.
     * Make sure we're a router and that this isn't a broadcast address
     * Also make sure IP is enabled on this interface!
     */
    if (ip_broadcast(target, input))
	return(FALSE);
    if (!ip_router_running || !systeminit_flag)
	return(FALSE);
    if (!input->ip_enabled)
	return(FALSE);
    if (input->ip_proxy_disabled)
	return(FALSE);
    if (source == target)
	return(FALSE);

    /*
     * Check if we have a route to that address
     */
    gwaddr = target;
    idb = reg_invoke_ip_first_hop(&gwaddr, gwaddr, IP_PATH_DEFFLAGS);
    if (!idb)
	return(FALSE);

    /* 
     * There may be multiple routes to the gateway.  We don't want to respond
     * if any of them are on the same cable as the input.  first_hop will go
     * through the possible routes round-robin, so keep calling until we
     * get back to the original.  Count in case something wierd happens.
     */
     myaddr = gwaddr;
     for (numroutes = MAXROUTES; numroutes--; ) {
	gwaddr = target;
	idb = reg_invoke_ip_first_hop(&gwaddr, gwaddr, IP_PATH_DEFFLAGS);
	if (idb == input)
 	    break;
	if (gwaddr == myaddr)
 	    break;
    }	
    if (!idb)
	return(FALSE);
    if (idb != input) {
	/*
	 * Route is thru us.  Provide a proxy ARP response.
	 * Allow registered protocols to affect what MAC address we use as
	 * the source hardware address in the proxy ARP response.  Also
	 * allow the protocol to inhibit proxy ARP responses.
	 */
	if (thdw) {
	    if (reg_invoke_ip_proxyarp_proto_action(input, source, target,
						    &allow_proxy, thdw))
		return(allow_proxy);
	}
 	return(TRUE);
    }

    return(FALSE);
}


/*
 * ip_arp_merge
 * Merge ARP datagram's information into our data structure
 */

static void ip_arp_merge (ip_arp_decoded *canptr)
{
    int bucket;
    arptype *ptr;
    idbtype *connected_idb, *swidb, *output_if;
    boolean learn, mobile;

    /*
     * See if we've heard of this guy before.  If we have, update his
     * hardware address and the reference time.
     */
    learn = FALSE;
    FOR_ALL_SWIDBS(swidb)
	if (ROUTING_IF(swidb, LINK_IP)) {
	    learn = TRUE;
	    break;
	}
    mobile = FALSE;
    bucket = nethash(canptr->spro); /* determine hash bucket */
    for (ptr = arptable[bucket]; ptr; ptr = ptr->next) {
 	if ((ptr->type != LINK_IP) || (ptr->address != canptr->spro))
 	    continue;
	learn = FALSE;
 	break;
    }
    output_if = canptr->arpidb->output_if;
    if (ptr) {
 	switch (ptr->mode) {
	case ARP_INCOMPLETE:
	case ARP_DYNAMIC:
	case ARP_MOBILE:

	    if (ptr->mode == ARP_INCOMPLETE) {
		/*
		 * We've sent a request, and we've just learned their
		 * address. Someday, dequeue any queued packets for this
		 * host.  If we currently have an incomplete entry, then
		 * anything else is better.  Thus, skip all of the below
		 * checks. 
		 */
		ptr->mode = ARP_DYNAMIC;
	    } else {
		/*
		 * Either ARP_DYNAMIC or ARP_MOBILE.
		 * Handle the case where we hear both ARPA and SNAP
		 * from a Wellfleet.  If we have ARPA and the new
		 * information is SNAP, and the ARPA information is recent,
		 * ignore the SNAP information.
		 */
		if ((canptr->encmethod == ET_SNAP) &&
		    (canptr->arpidb->hwptr->status & IDB_ETHER) &&
		    (ptr->enctype == ET_ARPA) &&
		    (ELAPSED_TIME(ptr->time) < ONEMIN))
		    break;
		
		/*
		 * The following handles the case where we are hearing from
		 * another cisco across a transit fddi ring.  It will send
		 * or reply (to our send) with multiple arps (one native,
		 * the other transit encapsulated).  We prefer the SNAP
		 * version.
		 */
		if ((canptr->arpidb->hwptr->status & IDB_FDDI) &&
		    !bcmp(canptr->shdw, ptr->hardware, ptr->hwlen) &&
		    (ptr->idb == canptr->arpidb) &&
		    (ptr->enctype == ET_SNAP) &&
		    (canptr->encmethod == ET_ARPA))
		    break;
		
		/*
		 * If we got a duplicate ARP reply from an SMDS link, the
		 * ET_SMDS replies will always take precedence over the
		 * ET_ARPA replies. 
		 */
		if (is_smds(canptr->arpidb->hwptr) &&
		    (ptr->idb == canptr->arpidb) &&
		    (ptr->enctype == ET_SMDS) &&
		    (canptr->encmethod == ET_ARPA))
		    break;

		/*
		 * Leave the information attached to the transmit interface
		 * if this is a receive only interface.
		 */
		if (output_if != canptr->arpidb)
		    break;
	    }
	   
 	    if ((bcmp(canptr->shdw, ptr->hardware, ptr->hwlen)) ||
		(canptr->encmethod != ptr->enctype)) {
		reg_invoke_ipcache_invalidate_range(ptr->address, MASK_HOST);
                reg_invoke_ipcache_invalidate_nexthop(ptr->address);
            }
	    bcopy(canptr->shdw, ptr->hardware, canptr->arhln);
	    ptr->hwlen = canptr->arhln;		/* set the length */
 	    GET_TIMESTAMP(ptr->time);	        /* update last reference */
 	    ptr->idb = canptr->arpidb;		/* update interface pointer */
 	    ptr->enctype = canptr->encmethod;	/* update encaps method */

	    /*
	     * If it's not from net directly connected to this interface, hand
	     * the entry to mobile IP, in case it wants to add a route
	     *
	     * Or possibly entry is associated with a routing subinterface off
	     * of the same physical interface, if so let the ARP cache know.
	     */

	    connected_idb = samecable(ptr->address);
	    if (!connected_idb || connected_idb != ptr->idb) {
		if (reg_invoke_ip_mobile_arp_accept(canptr->arpidb,
						    canptr->spro)) 
		    reg_invoke_ip_mobile_arp_new_host(&ptr->mode,
						      ptr->address,
						      ptr->idb);
		if ((connected_idb != NULL) &&
		    (connected_idb->hwptr == canptr->arpidb->hwptr))

		    canptr->arpidb = connected_idb;

	    } else
		ptr->mode = ARP_DYNAMIC;
 	    break;

	  case ARP_INTERFACE:
	  case ARP_ALIAS:
 	    if (!bcmp(canptr->shdw, ptr->hardware, ptr->hwlen) ||
 		(ptr->flag == ARP_EF_DUP))
 		break;
 	    
 	    /*
 	     * Someone else is using our address 
 	     */
 	    if (canptr->reply_flag) {
		errmsg(&msgsym(DUPADDR, IP),
		       canptr->spro,canptr->arpidb->namestring,
		       canptr->shdw);
  		/*
 		 * Defend our address
		 */
		ip_arp_unsolicited((ptr->idb ? ptr->idb : canptr->arpidb),
				   canptr->spro, ptr->hardware, NULL);
 		canptr->reply_flag = FALSE;
 	    } else {
 		/* 
 		 * Some one is defending our address 
 		 * or trying to come up with our address. 
  		 */
 		if ((canptr->spro == canptr->tpro) &&
		    bcmp(canptr->shdw, canptr->thdw, canptr->arhln)) {
 		    if (is_ieee_bcast(canptr->rthdw)) {
			errmsg(&msgsym(DUPADDR, IP),
			       canptr->spro, canptr->arpidb->namestring,
			       canptr->shdw);
			ip_arp_unsolicited((ptr->idb ?
					    ptr->idb : canptr->arpidb),
					   canptr->spro, ptr->hardware, NULL);
 		    } else {
 			/*
 			 * Update our cache
 			 */
			reg_invoke_ipcache_invalidate_range(ptr->address,
							    MASK_HOST);
                        reg_invoke_ipcache_invalidate_nexthop(ptr->address);
			bcopy(canptr->shdw,ptr->hardware,canptr->arhln);
 			GET_TIMESTAMP(ptr->time);
 			ptr->idb = canptr->arpidb;
 			ptr->enctype = canptr->encmethod;
 			ptr->flag = ARP_EF_DUP;
			errmsg(&msgsym(DUPADDR, IP),
			       canptr->spro, canptr->arpidb->namestring,
			       canptr->shdw);

                        /*
                         * Action upon duplicate address detected.
                         */
                        reg_invoke_ip_dupaddr_action((ptr->idb ?
                                                      ptr->idb :
                                                      canptr->arpidb),
                                                     canptr->spro);
  		    }
  		}
  	    }
 	    break;
	  default:
 	    break;
  	}
    }

    /* 
     * Check if we're going to respond to this request.	
     */
    if (ip_arp_assist(canptr->spro, canptr->tpro, canptr->arpidb, LINK_IP,
		      canptr->encmethod, canptr->thdw) == FALSE) {
	/*
	 * It's an ARP that we're not going to proxy reply to.  Other
	 * folks might like to look at it.
	 */
	reg_invoke_ip_arp_receive(canptr->arpidb, canptr->reply_flag,
				  canptr->spro, canptr->tpro);
	canptr->reply_flag = FALSE;
	learn = FALSE;
    }


    if (canptr->reply_flag) {
	if (!ip_ifaddress(canptr->arpidb, canptr->tpro))
	    arp_traffic.proxyrep++; /* count a proxy reply */
    }

    /*
     * If it's not from a subnet directly connected to this interface,
     * hand the entry to mobile IP, in case it wants to add a route.
     */

    connected_idb = samecable(canptr->spro);

    if (!connected_idb || ((connected_idb != canptr->arpidb) &&
			   (connected_idb != output_if))) {

	if (reg_invoke_ip_mobile_arp_accept(canptr->arpidb, canptr->spro)) { 
	    mobile = TRUE;
	    learn = (ptr == NULL);
	}
    }

    if (!learn)
	return;
    /*
     * No sense in adding an entry to the arp cache for a receive only
     * interface.
     */
    if (output_if != canptr->arpidb)
	return;
    
    
    /*
     * Create an entry if we've never seen this guy before.  Check for
     * martian IP addresses.
     */
    if (martian(canptr->spro, TRUE)) {
	canptr->reply_flag = FALSE;
	return;
    }
    ptr = ip_arp_table_add(canptr->arpidb, canptr->spro, canptr->shdw,
			   canptr->encmethod, ARP_DYNAMIC);
    if (mobile)
	reg_invoke_ip_mobile_arp_new_host(&ptr->mode, ptr->address,
					  ptr->idb); 

    if (arp_debug)
	buginf("\nIP ARP: creating entry for IP address: %i, hw: %e",
	       canptr->spro, canptr->shdw);
}

/*
 * ip_arp_sendreply
 * Send a reply to the request we are servicing
 */

static void ip_arp_sendreply (paktype *arppak, ip_arp_decoded *canptr)
{
    rfc_arp *rfcptr;
    smds_arp *smds_rfcptr;
    fr_rfc_arp *fr_rfcptr;
    ipaddrtype temp;
    ulong frhard;

    rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);

    /*
     * Count a reply and change opcode.
     * Compose reply based on hardware type.
     */
    arp_traffic.repsent++;
    rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);
    rfcptr->opcode = OP_REP;

    switch (rfcptr->arhrd) {
      case HW_ETH:
      case HW_802:

	/*
	 * Set target hardware address to that of originator
	 * The target replies -- note we may be a GW doing ARP hacking
	 * so we must not use our IDB address.  Use target address instead.
	 * Set new target to address of originator.
	 * Set source hardware address to our own.
	 */
	bcopy(rfcptr->ipshdw,rfcptr->ipthdw,rfcptr->arhln);
	temp = GETLONG(&rfcptr->ipspro);
	PUTLONG(&rfcptr->ipspro, rfcptr->iptpro);
	rfcptr->iptpro = temp;
        if (is_smds(arppak->if_input->hwptr)) {
	    bcopy(default_mac_addr, rfcptr->ipshdw,canptr->arhln);
        } else {
	    bcopy(canptr->thdw, rfcptr->ipshdw,canptr->arhln);
        }
	arp_send(arppak, RBYTES, rfcptr->ipthdw, LINK_ARP,
		 arppak->if_input, arppak->enctype, LINK_IP);
	break;

      case HW_SMDS_RFC:
	/*
	 * Set target hardware address to that of originator
	 * The target replies -- note we may be a GW doing ARP hacking
	 * so we must not use our IDB address.  Use target address instead.
	 * Set new target to address of originator.
	 * Set source hardware address to our own.
	 */
	smds_rfcptr = (smds_arp *)smds_arp_headstart(arppak);
	bcopy(smds_rfcptr->ipshdw,smds_rfcptr->ipthdw,HL_SMDS_RFC);
	temp = GETLONG(&smds_rfcptr->ipspro);
	PUTLONG(&smds_rfcptr->ipspro, smds_rfcptr->iptpro);
	smds_rfcptr->iptpro = temp;
	bcopy(canptr->thdw,smds_rfcptr->ipshdw, HL_SMDS_RFC);
	arp_send(arppak, RSBYTES, smds_rfcptr->ipthdw,
		 LINK_ARP, arppak->if_input, arppak->enctype,
		 LINK_IP);
	break;

      case FR_ARHRD:
	/*
	 * Set target hardware address to that of originator
	 * The target replies -- note we may be a GW doing ARP hacking
	 * so we must not use our IDB address.  Use target address instead.
	 * Set new target to address of originator.
	 */
	fr_rfcptr = (fr_rfc_arp *)fr_rfc_arp_headstart(arppak);
	fr_rfcptr->ipthdw = fr_rfcptr->ipshdw;
	temp = GETLONG(&fr_rfcptr->ipspro);
	PUTLONG(&fr_rfcptr->ipspro, fr_rfcptr->iptpro);
	fr_rfcptr->iptpro = temp;

	/* 
	 * Fill in target hardware which will be picked up by the other end
	 * from the packet encapsulation.  send_arp() wants a pointer to
	 * the target hardware, hence the hack of copying the short value
	 * into a long and casting the long to a uchar pointer.
	 */
	frhard = fr_rfcptr->ipthdw;
	bcopy(canptr->thdw, (uchar *)&fr_rfcptr->ipshdw, fr_rfcptr->arhln);
	arp_send(arppak, RFBYTES, (uchar *)&frhard, LINK_ARP,
		 arppak->if_input,  arppak->enctype,  LINK_IP);
	break;

      default:
	/*
	 * Unknown hardware type, so just discard it.
	 */
	retbuffer(arppak);
	break;
    }
}

/*
 * ip_arp_input
 * Handle ARP input processing for IP.
 */

void ip_arp_input (paktype *arppak)
{
    ip_arp_decoded decoded;
    
    if (ip_arp_filter(arppak)) {
	ip_arp_canonicalize(arppak, &decoded);
	ip_arp_merge(&decoded);
	if (decoded.reply_flag)
	    ip_arp_sendreply(arppak, &decoded);
	else
	    retbuffer(arppak);
    }
}

/*
 * ip_arp_ifcommand
 * Modify how arps are generated on a particular interface.
 * It has the following form:
 *
 * 		interface <blah de blah>
 * 		arp { arpa | probe | snap | timeout | smds }
 *
 * Note well: there is no code for "arp smds"
 */

static const char badarpcmd[] = "\nBad ARP command - %s";

void ip_arp_ifcommand (parseinfo *csb)
{

    hwidbtype *hwidb = hwidb_or_null(csb->interface);

    if (csb->nvgen) {
	idbtype *idb = csb->interface;
	switch (csb->which) {
	  case ARP_ARPA:
	    nv_write((idb->arp_arpa != idb->arp_arpa_def),
		     "%s%s", (idb->arp_arpa_def ? nostr : NULL),
		     csb->nv_command);
	    break;
	  case ARP_PROBE:
	    nv_write((idb->arp_probe != idb->arp_probe_def),
		     "%s%s", (idb->arp_probe_def ? nostr : NULL),
		     csb->nv_command);
	    break;
	  case ARP_SNAP:
	    nv_write((idb->arp_snap != idb->arp_snap_def),
		     "%s%s", (idb->arp_snap_def ? nostr : NULL),
		     csb->nv_command);
	    break;
	case ARP_FR:
	    if (is_frame_relay(idb->hwptr) )
		nv_write(!idb->fr_arp_enable, "%s%s",
			 (!idb->fr_arp_enable ? nostr : NULL),
			 csb->nv_command);
	    break;
	case ARP_ENTRY_TIME:
	    nv_write(((idb->arp_arpa || idb->arp_snap || idb->arp_smds) &&
		      (idb->arp_timeout != ARP_MAXAGE)),
		     "%s %u", csb->nv_command, idb->arp_timeout/ONESEC);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Check subcommands
     */
    switch (csb->which) {
      case ARP_ENTRY_TIME:
	if (csb->sense) {
	    /*
	     * yes. request to reset arp_timeout to user specified
	     * value. user specified value is in seconds, convert it
	     * to Milliseconds before assigning it to arp_timeout.
	     * This value applies only to the interface specified
	     * before in the configuration and it is used to timeout
	     * arp cache entries associated with this interface. If
	     * this value is 0, the entries will never be timedout. 
	     */
	    csb->interface->arp_timeout = (GETOBJ(int,1) * ONESEC);
	} else {
	    /*
	     * Request to reset arp_timeout to default value(4 hours)
	     */
	    csb->interface->arp_timeout = ARP_MAXAGE;
	}
	break;
      case ARP_SNAP:
	if (!csb->sense &&(hwidb->status & IDB_802ONLY)) {
	    /* trying to turn off snap on 802 only if */
	    printf(badarpcmd, "snap required");
	    return;
	}
	csb->interface->arp_snap = csb->sense;
	break;
      case ARP_PROBE:
	if (csb->sense && (hwidb->status & IDB_802ONLY)) {
	    /* trying to turn on arpa encaps on 802 only if */
	    printf(badarpcmd, "only snap allowed");
	    return;
	}
	csb->interface->arp_probe = csb->sense;
	(*hwidb->reset)(hwidb);
	if (hwidb->counters.resets > 0)
	    hwidb->counters.resets--;
	break;
      case ARP_ARPA:
	if (csb->sense && (hwidb->status & IDB_802ONLY)) {
	    /* trying to turn on arpa encaps on 802 only if */
	    printf(badarpcmd, "only snap allowed");
	    return;
	}
	csb->interface->arp_arpa = csb->sense;
	break;
    case ARP_FR:
	csb->interface->fr_arp_enable = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    /*
     * Since we have changed how we talk arp on this
     * cable it might be a reasonable thing to flush the
     * arp cache here.  We don't because this preserves the
     * most information about how to talk to a host.
     */
}

/*
 * ip_arp_command
 * Manually insert an entry into the ARP table.
 * Command is of the form:
 * 
 *		"arp 36.8.0.16  1234.ABCD.1F0C keyword
 * 
 * where "keyword" is one of "arpa", "sap", "snap", "smds"
 *
 */

void ip_arp_command (parseinfo *csb)
{
    int method, timeout, i;
    arptype *ptr;
    uint bucket;
    ipaddrtype address;
    idbtype *idb_configured;

    if (csb->nvgen) {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (ptr = arptable[i]; ptr; ptr = ptr->next) {
		if ((ptr->mode == ARP_STATIC) || (ptr->mode == ARP_ALIAS)) {
		    nv_write(TRUE, "%s %i %e %s%s",  csb->nv_command,
			     ptr->address, ptr->hardware, 
			     encstring(ptr->enctype),
			     (ptr->mode == ARP_ALIAS) ? " alias" : NULL);
		    if ((ptr->mode == ARP_STATIC) && ptr->idb &&
			BRIDGING_IF(ptr->idb, LINK_IP))
			nv_add(TRUE, " %s", ptr->idb->namestring);
		}
	    }
	}
	return;
    }

    csb->interface = NULL;
    idb_configured = NULL;
    address = GETOBJ(paddr,1)->ip_addr;
    if (csb->sense) {
	switch (GETOBJ(int,1)) {
	  default:
	  case ARP_ARPA: method = ET_ARPA; break;
	  case ARP_SAP:  method = ET_SAP; break;
	  case ARP_SNAP: method = ET_SNAP; break;
          case ARP_SMDS:
              {
              hwaddrtype *hwaddr = GETOBJ(hwaddr,1);
              method = ET_SMDS;
              /*
               * The ARP command only parses 48-bits.
               * SMDS requires that the unused 16-bits be filled with 0xFFFF.
               * TR-TSV-000772 defines address requirement.
               */
              hwaddr->addr[6] = hwaddr->addr[7] = 0xFF;
              }
              break;
	}
	timeout = (GETOBJ(int,4)) ? ARP_ALIAS : ARP_STATIC;

        /*
	 * When bridging IP, static ARP table entries need an indication of 
	 * what interface the MAC address is associated with.  Otherwise, 
	 * the system can't properly figure it out on its own, due to lack
	 * of IP routing info.
	 */
	if (GETOBJ(idb,1)) {
	    if (BRIDGING_IF(GETOBJ(idb,1), LINK_IP)) {
		idb_configured = GETOBJ(idb,1);
	    } else {
		printf(badarpcmd,
		       "Interface may only be specified when bridging IP");
		return;
	    }
	}

	/*
	 * Add to table, then to a lookup and ensure the mode word
	 * is set properly.  The lower level routines will not change
	 * the mode for an already existing entry.
	 */
	ip_arp_table_add(idb_configured, address, GETOBJ(hwaddr,1)->addr,
			 method, timeout);
 	ptr = arp_table_lookup(LINK_IP, address, idb_configured);
	if (ptr) {
	    ptr->mode = timeout;
	    ptr->enctype = method;
	}
    } else {	
	/*
	 * Make sure we don't try to delete an interface entry.
	 */
	bucket = nethash(address);		/* determine hash bucket */
	ptr = arptable[bucket];		/* set pointer to hash chain */
	while (ptr) {
	    if ((ptr->type == LINK_IP) && (ptr->address == address) &&
		(ptr->mode == ARP_INTERFACE)) {
		printf(badarpcmd,"Can't unset interface address");
		return;
	    }
	    ptr = ptr->next;
	}
	arp_table_remove(GETOBJ(paddr,1)->ip_addr, LINK_IP);
    }
}

/*
 * ip_arp_table_add
 * Add an IP address to the ARP cache
 */

arptype *ip_arp_table_add (idbtype *idb, ipaddrtype address, uchar *hardware,
			   int method, int flag)
{
    arptype *ptr;
    hwaddrtype hwaddr;

    if (martian(address, TRUE))
	return(NULL);
    switch(method) {
    case ET_ATM:
	if (lec_enabled(idb)) {
	    method = ET_ARPA;
	    hwaddr.type = STATION_IEEE48;
	    hwaddr.length = STATIONLEN_IEEE48;
	} else {
	    method = ET_ATM;
	    hwaddr.type = STATION_ATMVC;
	    hwaddr.length = STATIONLEN_ATMVC;
	}
	break;
    case ET_SMDS:
	method = ET_SMDS;
	hwaddr.type = STATION_SMDS64;
	hwaddr.length = STATIONLEN_SMDS64;
	break;
    case ET_FRAME_RELAY:
	method = ET_FRAME_RELAY;
	hwaddr.type = STATION_FR10;
	hwaddr.length = STATIONLEN_FR10;
	break;
    default:
    	hwaddr.type = STATION_IEEE48;
    	hwaddr.length = STATIONLEN_IEEE48;
	break;
    }
    bcopy(hardware, &hwaddr.addr, hwaddr.length);
    ptr = arp_table_lookup(LINK_IP, address, idb);
    if (ptr && (flag == ARP_DYNAMIC)) {
	switch (ptr->mode) {
	case ARP_STATIC:
	case ARP_INTERFACE:
	    /*
	     * Don't override configured entries with dynamic info.
	     */
	    if (arp_debug)
		buginf("\nIP ARP: Ignoring dynamic ARP info: %i, hw: %e",
		       address, hardware);
	    return(ptr);
	default:
	    break;
	}
    }
    if (!ptr || bcmp(hardware, ptr->hardware, ptr->hwlen) ||
	(method != ptr->enctype) ||
	(idb != ptr->idb) ||
	(flag != ptr->mode)) {
	ptr = arp_table_add(idb, address, LINK_IP, &hwaddr, method, flag);
	if (flag != ARP_INCOMPLETE) {
	    reg_invoke_ipcache_invalidate_range(address, MASK_HOST);
            reg_invoke_ipcache_invalidate_nexthop(address);
        }
    }
    return(ptr);
}

/*
 * ip_arp_invalidate
 * Given an IP address, invalidate (flush) any temporary ARP cache
 * entry associated with that address.  This is used by higher levels
 * such as TCP when a connection is starting to timeout.  We may be
 * able to recover by ARP'ing again.
 */

void ip_arp_invalidate (ipaddrtype address)
{
    arptype *ptr;

    ptr = arp_table_lookup(LINK_IP, address, NULL);
    if (ptr && (ptr->mode == ARP_DYNAMIC))
	arp_table_remove(address, LINK_IP);
}

/*
 * ip_arp_supported_idb
 * Do we support ARP out this interface?  Return TRUE if so.
 */

static boolean ip_arp_supported_idb (idbtype *swidb)
{
    if (idb_is_ieeelike(swidb))
	return(TRUE);
    if (swidb->arp_smds)
	return(TRUE);
    return(FALSE);
}

/*
 * ip_arp_add_interface
 * Quietly add an ARP entry for the specified IP address
 */

void ip_arp_add_interface (idbtype *idb, ipaddrtype address)
{
    hwidbtype *hwidb = idb->hwptr;

    if (idb_is_ieeelike(idb) || is_bvi(idb->hwptr)) {

	ip_arp_table_add(idb, address, hwidb->hardware, hwidb->enctype,
			 ARP_INTERFACE);
    /*
     * Make entry in arp table for serial interface running smds
     * arp-enabled.
     */
    } else if((hwidb->status & IDB_SERIAL) && 
	      idb->arp_smds && idb->vaddr) {
	ip_arp_table_add(idb, address, idb->vaddr->addr,
			 hwidb->enctype, ARP_INTERFACE);
    /*
     * Make an entry in ARP table for ATM interfaces running SMDS
     */
    } else if (is_atm(hwidb) && idb->vaddr) {
	ip_arp_table_add(idb, address, idb->vaddr->addr,
			 hwidb->enctype, ARP_INTERFACE);
    }
}

/* 
 * ip_arp_route 
 * Given an IP address, find an ARP entry.
 * Used when doing host style IP routing.  
 *
 * We don't call ip_arp_resolve() directly because that would
 * short-circuit around the sanity checks that throttle out
 * inappropriate ARP Requests.
 */ 

idbtype *ip_arp_route (ipaddrtype address, boolean arp_search_ok)
{
    arptype *ptr;

    /* 
     * Lookup the ARP entry.  If entry isn't found in the ARP table,
     * and other sanity checks fly, an ARP Request will be sent.
     *
     * If arp_search_ok is true, we'll wait MAXRETRIES times
     * for the ARP Reply and might retransmit the ARP Request
     * while waiting.
     */
    if (arp_search_ok)
	ptr = ip_arp_lookup_and_resolve(NULL, address);
    else 
	ptr = ip_arp_table_lookup(NULL, address);

    if (ptr)
	return(ptr->idb); 
    else
	return(NULL);
}


/*
 * ip_arp_sendrequest
 * Send an RFC826 style address resolution request.
 * A destination of NULL implies broadcast.
 */

static void ip_arp_sendrequest (ipaddrtype address, idbtype *interface,
				int method, hwaddrtype *destination)
{
    hwidbtype *hwidb = hwidb_or_null(interface);
    paktype *arppak;
    rfc_arp *rfcptr;
    smds_arp *smds_rfcptr;
    fr_rfc_arp *fr_rfcptr;
    idbtype *idb;
    arptype *ptr;
    int bytes;
    ipaddrtype saddr;
    uchar *hwdest = NULL;

    idb = (interface->ip_unnumbered) ? interface->ip_unnumbered : interface;
    /*
     * Get the best source address among the address configured in this
     * interface
     */
    saddr = ip_best_ifaddress(address, idb, TRUE);
    if (!saddr)
	saddr = idb->ip_address;

    /*
     * Think up some excuses we shouldn't do this, e.g. we have no address,
     * the target is a martian or a broadcast, or we're in a snit because
     * someone else is using our address. Notice that we are
     * taking unnumbered serial interfaces into account.
     */
    if ((saddr == 0L) || martian(address, TRUE) ||
	ip_broadcast(address, interface))
	return;
    ptr = arp_table_lookup(LINK_IP, saddr, idb);
    if (ptr && (ptr->flag == ARP_EF_DUP))
	return;

    /*
     * Get the biggest possible buffer for this ARP
     */
    bytes = max(max(RSBYTES, RFBYTES), RBYTES);
    arppak = getbuffer(bytes);
    if (!arppak)
	return;

    hwdest = (uchar *)baddr;
    if(destination) {
	hwdest = destination->addr;
    }
    rfcptr = (rfc_arp *)rfc_arp_headstart(arppak);

    /*
     * Do some beancounting, fill in the generic portion
     * of the header, and then compose the body of the datagram.
     */
    arp_traffic.reqsent++;
    rfcptr->opcode = OP_REQ;
    rfcptr->arpro = PR_IP;
    rfcptr->arpln = PL_IP;
    switch (method) {
      case ET_ATM:
	reg_invoke_arp_sendreq(method, arppak, address, interface, destination);
	break;

      case ET_SMDS:     
	smds_rfcptr = (smds_arp *)smds_arp_headstart(arppak);
	rfcptr->arhrd = HW_SMDS_RFC;
	rfcptr->arhln = HL_SMDS_RFC;
        if (!interface->vaddr) {
	    retbuffer(arppak);
	    return;
        }
	bcopy(interface->vaddr->addr, rfcptr->ipshdw, HL_SMDS_RFC);
	PUTLONG(&smds_rfcptr->ipspro, saddr);
	if (destination)
	    bcopy(destination->addr, smds_rfcptr->ipthdw, HL_SMDS_RFC);
	else {
	    memset(smds_rfcptr->ipthdw, 0, HL_SMDS_RFC);
	}
	smds_rfcptr->iptpro = address;
	arp_send(arppak, RSBYTES, hwdest, LINK_ARP,
		 interface, method, LINK_IP);
        break;

      case ET_FRAME_RELAY:
	fr_rfcptr = (fr_rfc_arp *)fr_rfc_arp_headstart(arppak);
	rfcptr->arhrd = FR_ARHRD;
	rfcptr->arhln = FR_ARHLN;
	memset(&fr_rfcptr->ipshdw, 0, ADDRLEN_FR);
	PUTLONG(&fr_rfcptr->ipspro, saddr);
	if (destination)
	    bcopy(destination->addr, (uchar *)&fr_rfcptr->ipthdw, ADDRLEN_FR);
	else 
	    memset(&fr_rfcptr->ipthdw, 0, ADDRLEN_FR);
	fr_rfcptr->iptpro = address;
	arp_send(arppak, RFBYTES, hwdest, LINK_ARP, interface, method,
		 LINK_IP);
	break;

      default:
	rfcptr->arhln = HL_ETH;
	rfcptr->arhrd = HW_ETH;
	if ((method == ET_SNAP) || (method == ET_BRIDGE_ENCAPS))
	    rfcptr->arhrd = HW_802;
	if (is_smds(hwidb)) {
	    ieee_copy(default_mac_addr, rfcptr->ipshdw);
	} else {
	    ieee_copy(hwidb->hardware, rfcptr->ipshdw);
	}
	PUTLONG(&rfcptr->ipspro, saddr);
	if (destination)
	    ieee_copy(destination->addr, rfcptr->ipthdw);
	else {
	    ieee_zero(rfcptr->ipthdw);
	}
	rfcptr->iptpro = address;
	arp_send(arppak, RBYTES, hwdest, LINK_ARP, interface, method,
		 LINK_IP);
	break;
    }
}

/*
 * ip_arp_table_lookup
 * Do address resolution for Ethernets and IEEE 802.3 networks.
 * If not in cache, send at most one ARP Request for destination. 
 * Perform sanity checks before sending any ARP Request.
 */

arptype *ip_arp_table_lookup (idbtype *interface, ipaddrtype destination)
{
    arptype *ptr;
    idbtype *idb;

    /* 
     * First we look in the ARP cache.  If it isn't there,
     * generate the appropriate resolution request(s).
     */	 
    ptr = arp_table_lookup(LINK_IP, destination, interface);
    if (ptr) {
	/*
	 * If we found something valid, return it.  
	 */
	if (ptr->mode != ARP_INCOMPLETE) 
	    return(ptr);
	
	if (CLOCK_IN_INTERVAL(ptr->time, ARP_THROTTLE_INTERVAL)) {
	    if (arp_debug)
		buginf("\nIP ARP throttled out the ARP Request for %i",
		       destination);
	    return(NULL);
	} 
	/* 
	 * We will be causing new ARP Request(s) to be sent,
	 * so set the "time of last ARP Request" equal to now. 
	 */
	GET_TIMESTAMP(ptr->time);
    } 
    
    if ((interface) && !BRIDGING_IF(interface, LINK_IP)) {

	/* 
	 * If we know which interface the target lives on and
	 * we are not bridging, then only send the ARP on the
	 * appropriate interface. 
	 */
	ip_arp_resolve(interface, destination);
    } else {

	/* 
	 * If interface is NULL or bridging is enabled, 
	 * then send ARPs out on all interfaces 
	 * because we have no clue which interface the target lives on.
	 */
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_enabled)
		ip_arp_resolve(idb, destination);
	}
    } 
    return(NULL);
}


/* 
 * ip_arp_lookup_and_resolve
 *   Attempt to lookup an ARP entry for destination on LAN media.
 *   If sane, loop MAXRETRIES times waiting for a reply.
 */
arptype *ip_arp_lookup_and_resolve (idbtype *interface, ipaddrtype destination)
{
    int retries;
    arptype *ptr;

    retries = MAXRETRIES;
    
    while (TRUE) {

	/* 
	 * Check in the ARP cache and send at most one ARP Request.
	 */
	ptr = ip_arp_table_lookup(interface, destination);

	if ((ptr) &&
	    (ptr->mode != ARP_INCOMPLETE))
	    return(ptr);

	/* 
	 * Don't bother waiting for an ARP reply if we are running a routing
	 * protocol for this packet, user has asked us to abort, or we were
	 * called from interrupt level or a high-priority process.
	 */ 
	if (ip_router_running || psipending() || get_interrupt_level() ||
	    process_is_high_priority() || blocking_disabled)
	    return(NULL);

	/* Give up after MAXRETRIES attempts */
	if (--retries == 0)
	    return(NULL);

	process_sleep_for(DISMISSTIME*ONESEC);
    }
}


/*
 * ip_arp_incomplete
 * Create an incomplete ARP table entry.  Return TRUE if successful.
 */

boolean ip_arp_incomplete (idbtype *interface, ipaddrtype destination)
{
    arptype *ptr;

    if (onintstack()) {
	/*
	 * We may be encapsulating an IP packet at interupt level...
	 * Example: sending an ICMP quench.
	 */
	ptr = NULL;
    } else {
	ptr = ip_arp_table_add(interface, destination, (uchar *) zeromac,
			       interface->ip_enctype, ARP_INCOMPLETE);
    }
    if (arp_debug)
	buginf("\nIP ARP: %s incomplete entry for IP address: %i",
	       (ptr ? "creating" : "failed to create"), destination);

    if (ptr)
	return(TRUE);

    return(FALSE);
}

/*
 * ip_arp_resolve
 * Do address resolution for IP protocols on a single interface.
 * Checks to see if particular AR method should be used
 */

void ip_arp_resolve (idbtype *interface, ipaddrtype destination)
{
    hwidbtype *hwidb;
    arptype *ptr;

    /*
     * Check to insure that there isn't an entry.  If there isn't one, then
     * create an incomplete one.
     */
    ptr = arp_table_lookup(LINK_IP, destination, NULL);
    if (!ptr) {

	if (!ip_arp_incomplete(interface, destination))
	    return;
	/*
	 * Other folks might care that we're starting an incomplete entry.
	 */
	reg_invoke_ip_arp_add_incomplete_entry(interface, destination);
    }

    if (interface->arp_probe) 
	reg_invoke_arp_sendreq(ET_SAP, NULL, destination, interface, NULL);
    if (interface->arp_snap) 
	ip_arp_sendrequest(destination, interface, ET_SNAP, NULL);
    if (interface->arp_arpa ||
	(is_ppp(interface->hwptr) && interface->hwptr->ppp_bridge_ip))
	ip_arp_sendrequest(destination, interface, ET_ARPA, NULL);
    if (interface->arp_smds) 
	ip_arp_sendrequest(destination, interface, ET_SMDS, NULL);
    if (interface->fr_arp_enable) 
	ip_arp_sendrequest(destination, interface, ET_FRAME_RELAY, NULL);
    if (is_atm(interface->hwptr))
	ip_arp_sendrequest(destination, interface, ET_ATM, NULL);

    /*
     * ARPing via a vLAN subinterface, just use the default media encapsulator
     * and have the vLAN specific encapsulation applied later.
     */

    if (IS_VLAN_CONFIGURED_SUBINTERFACE(interface))

	ip_arp_sendrequest(destination, interface,
			   interface->hwptr->enctype_def, NULL);

    if (BRIDGING_IF(interface, LINK_IP)) {
	hwidb = interface->hwptr;
	if (hwidb->status & IDB_FDDI)
	    ip_arp_sendrequest(destination, interface, ET_BRIDGE_ENCAPS,
			       NULL);
	if (hwidb->status & IDB_SERIAL) {
	    if (is_smds(hwidb))
	      {
		if (interface->tbridge_on_swidb)
		  ip_arp_sendrequest(destination, interface,
				   ET_BRIDGE_ENCAPS, NULL);
	      }
	    else if (is_ppp(hwidb))
	       ip_arp_sendrequest(destination, interface, ET_ARPA, NULL);
	    else if (is_frame_relay(hwidb) || is_atm_dxi(hwidb))
	       ip_arp_sendrequest(destination, interface, ET_ARPA, NULL);
	    else
		ip_arp_sendrequest(destination, interface, ET_HDLC, NULL);
	}
	if (is_atm(hwidb)) {
	    ip_arp_sendrequest(destination, interface, ET_BRIDGE_ENCAPS,
			       NULL);
	}
    }
}


/*
 * ip_arp_refresh
 * Given an arp entry that's about to expire, send the destination a
 * unicast ARP so that they reply and update our ARP cache.
 *
 */

void ip_arp_refresh (arptype *ptr)
{
    idbtype *interface = ptr->idb;
    hwaddrtype hwaddr;

    if (!interface)
	return;
    switch (ptr->enctype) {
        case ET_ARPA :
            if (!interface->arp_arpa) 
                return;
            break;
        case ET_SNAP :
            if (!interface->arp_snap)
                return; 
            break;
        case ET_FRAME_RELAY :
            if (!interface->fr_arp_enable)
                return; 
            break;
        case ET_SMDS :
            if (!interface->arp_smds)
                return; 
            break;
    }
    hwaddr.type = ptr->hwtype;
    hwaddr.length = ptr->hwlen;
    bcopy(ptr->hardware, hwaddr.addr, ptr->hwlen);
    ip_arp_sendrequest(ptr->address, interface, ptr->enctype, &hwaddr);
}

/*
 * ip_arp_fscache_invaldiate
 * Also invalidate the fast switching cache for this entry.
 * We never need to set blow_whole_cache to true because we're smart.
 */
boolean ip_arp_fscache_invalidate (arptype *ptr)
{
    if (ptr->idb) {
	reg_invoke_ipcache_invalidate_range(ptr->address, MASK_HOST);
        reg_invoke_ipcache_invalidate_nexthop(ptr->address);
    }

    return (FALSE);
}

/*
 * ip_arp_string
 * Protocol specific string and address to be printed in "show ip arp".
 */

static int ip_arp_string (arptype *ptr)
{			  
    return(printf("\nInternet  %i", ptr->address));
}

/*
 * ip_arp_show_command
 * Show IP part of the ARP table.
 */

void ip_arp_show_command (parseinfo *csb)
{
    hwaddrtype *hwaddr;
    idbtype *idb;
    ipaddrtype ipaddress;
    
    hwaddr = NULL;
    idb = NULL;
    ipaddress = 0;

    switch (csb->which) {
    case SHOW_ARP_INTERFACE:
	idb = GETOBJ(idb, 1);
	break;
    case SHOW_ARP_ADDR:
	ipaddress = GETOBJ(paddr,1)->ip_addr;
	break;
    case SHOW_ARP_MAC:
	hwaddr = GETOBJ(hwaddr, 1);
	break;
    case SHOW_ARP:
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    arp_display(LINK_IP, ip_arp_string, idb, ipaddress, hwaddr);
}

/*
 * ip_arp_glean
 * Make or update an ARP entry based on an actual IP packet that
 * we have received.  Checks for validity.
 */

void ip_arp_glean (paktype *arppak)
{
    iphdrtype *ip;
    hwaddrtype hdaddr;
    hwidbtype *hwidb = arppak->if_input->hwptr;
    ether_hdr *ether;

    (*hwidb->extract_snpa)(arppak, &hdaddr, SOURCE);
    ip = (iphdrtype *)ipheadstart(arppak);

    /*
     * Watch out for multicast source addresses
     * If target_type was set appropriately, these checks would be unnecessary!
     */
    if (ip_arp_ieee_mcast(hwidb, hdaddr.addr))
	return;
    if (hwidb->status & IDB_SERIAL) {
	if  ((hdaddr.type == STATION_SMDS64) && SMDS_ISGROUP(hdaddr.addr[0]))
	    return;
	if (hdaddr.target_type != TARGET_UNICAST)
	    return;
    }
    /*
     * If the media doesn't support ARP, don't add an ARP table entry
     */
    if (!ip_arp_supported_idb(arppak->if_input)) {
	/*
	 * If the packet is from a serial line, and we are resorting to
	 * host-based routing, then we are desperate, so allow the ARP
	 * entry to be added.  If we are bridging IP, we are already possibly
	 * adding ARP entries for serial lines.
	 */
	if (((hwidb->status & IDB_SERIAL) == 0) ||
	    (systeminit_flag && ip_router_running))
	    return;
	/*
	 * The interface's encapsulation does not normally support ARP.
	 * However, if we are bridging IP, IP traffic may be bridge
	 * encapsulated, thus supporting ARP.  In this case the packet will
	 * contain a legitimate MAC address, yet extract_snpa did not
	 * provide us with one.  Extract the MAC address from the packet
	 * header.
	 * This applies to PPP, HDLC, and Frame Relay encapsulations.
	 */
	if (BRIDGING_IF(arppak->if_input, LINK_IP) &&
	    RUNNING_IF(LINK_BRIDGE, arppak->if_input)) {
	    switch (arppak->enctype) {
	    case ET_ARPA:
	    case ET_SNAP:
	    case ET_SAP:
		ether = (ether_hdr *)arppak->mac_start;
		ieee_copy(ether->saddr, hdaddr.addr);
		hdaddr.length = STATIONLEN_IEEE48;
		break;
	    }
	}
	if (hdaddr.length == 0) {
	    /*
	     * Make sure a real MAC address is used for arp table entry.
	     */
	    ieee_copy(hwidb->hardware, hdaddr.addr);
	}
    }
    if (systeminit_flag && ip_router_running &&
	(arppak->if_input != samecable(ip->srcadr)))
	return;
    if (arp_debug)
	buginf("\nIP ARP: Gleaning entry for %i, %e", ip->srcadr,
	       hdaddr.addr);
    ip_arp_table_add(arppak->if_input, ip->srcadr, hdaddr.addr,
		     arppak->enctype, ARP_DYNAMIC);
}

/*
 * ip_arp_reload
 * Reload the ARP cache with our IP interface addresses
 * Called by SERVICE_ARP_RELOAD
 */

void ip_arp_reload (idbtype *idb)
{
    ipaddtype *ptr;
    hwidbtype *hwidb;

    hwidb = idb->hwptr;
    if (!idb->ip_enabled || !idb->ip_address)
	return;
    if (!ip_arp_supported_idb(idb))
	return;
    if ((hwidb->status & IDB_TR) && (hwidb->state != IDBS_UP))
	return;
    if (hwidb->state == IDBS_ADMINDOWN)
	return;
    ip_arp_add_interface(idb, idb->ip_address);
    ip_arp_unsolicited(idb, idb->ip_address, NULL, NULL);
    for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next){
	ip_arp_add_interface(idb, ptr->address);
	ip_arp_unsolicited(idb, ptr->address, NULL, NULL);
    }
}

/*
 * ip_arp_unsolicited
 * Send an unsolicited ARP on an interface, possibly using the supplied
 * unicast address.  If no supplied address, use the broadcast address.
 * This routine is used to notify others that we may have changed our
 * hardware address.
 */

void ip_arp_unsolicited (idbtype *idb, ipaddrtype ipaddress, uchar *thdw,
			 uchar *shdw)
{
    paktype *arppak;
    rfc_arp *rfcarp;
    smds_arp *smds_rfcarp;
    fr_rfc_arp *fr_rfcptr; 
    hwidbtype *hwidb = idb->hwptr;
    int bytes;
    
    /*
     * Get the biggest possible buffer for this ARP
     */
    bytes = max(max(RSBYTES, RFBYTES), RBYTES);
    arppak = getbuffer(bytes);
    if (!arppak)
	return;

    switch (hwidb->enctype) {
arp_smds_over_atm:
      case ET_SMDS:
	smds_rfcarp = (smds_arp *)smds_arp_headstart(arppak);
	arp_traffic.repsent++;
	if (!shdw) {
	    if (!idb->vaddr) {
		retbuffer(arppak);
		return;
	    }
	    bcopy(idb->vaddr->addr, smds_rfcarp->ipshdw, HL_SMDS_RFC);
	} else
	    bcopy(shdw, smds_rfcarp->ipshdw, HL_SMDS_RFC);
	if (!thdw)
	    bcopy(baddr, smds_rfcarp->ipthdw, HL_SMDS_RFC);
	else
	    bcopy(thdw, smds_rfcarp->ipthdw, HL_SMDS_RFC);
	smds_rfcarp->arhrd = HW_SMDS_RFC;
	smds_rfcarp->arpro = PR_IP;
	smds_rfcarp->arhln = HL_SMDS_RFC;
	smds_rfcarp->arpln = PL_IP;
	smds_rfcarp->opcode = OP_REP;
	PUTLONG(&smds_rfcarp->ipspro, ipaddress);
	smds_rfcarp->iptpro = ipaddress;
	arp_send(arppak, RSBYTES, (uchar *)baddr, LINK_ARP,
		 idb, hwidb->enctype, LINK_IP);
	break;

      case ET_FRAME_RELAY:
	fr_rfcptr = (fr_rfc_arp *)fr_rfc_arp_headstart(arppak);
	arp_traffic.repsent++;
	fr_rfcptr->ipshdw = 0;
	if (!thdw)
	    bcopy(baddr, (uchar *)&fr_rfcptr->ipthdw, FR_ARHLN);
	else
	    bcopy(thdw, (uchar *)&fr_rfcptr->ipthdw, FR_ARHLN);
	fr_rfcptr->arhrd = FR_ARHRD;
	fr_rfcptr->arpro = PR_IP;
	fr_rfcptr->arhln = FR_ARHLN;
	fr_rfcptr->arpln = PL_IP;
	fr_rfcptr->opcode = OP_REP;
	PUTLONG(&fr_rfcptr->ipspro, ipaddress);
	fr_rfcptr->iptpro = ipaddress;
	arp_send(arppak, RFBYTES, (uchar *) 0xFFFF,
		 LINK_ARP, idb, hwidb->enctype, LINK_IP);
	break;

      case ET_ATM:
        if (idb->arp_smds)
            goto arp_smds_over_atm;
        /*
         * else FALL THROUGH
         */
      case ET_ARPA:
      case ET_SNAP:
      default:
        rfcarp = (rfc_arp *)rfc_arp_headstart(arppak);
	arp_traffic.repsent++;
	if (!shdw)
	    ieee_copy(hwidb->hardware, rfcarp->ipshdw);
	else
	    ieee_copy(shdw, rfcarp->ipshdw);
	if (!thdw)
	    ieee_copy(baddr, rfcarp->ipthdw);
	else
	    ieee_copy(thdw, rfcarp->ipthdw);
	rfcarp->arhrd = (hwidb->enctype == ET_SNAP) ? HW_802 : HW_ETH;
	rfcarp->arpro = PR_IP;
	rfcarp->arhln = HL_ETH;
	rfcarp->arpln = PL_IP;
	rfcarp->opcode = OP_REP;
	PUTLONG(&rfcarp->ipspro, ipaddress);
	rfcarp->iptpro = ipaddress;
	arp_send(arppak, RBYTES, (uchar *)baddr, LINK_ARP, idb,
		 hwidb->enctype, LINK_IP);
	break;
    }
}

/*
 * ip_gratuitous_arp_process
 *
 * Process to handle the timer expirations for the various interfaces on
 * which we need to send gratuitous ARPs.
 *
 * There is a managed timer per interface running when this process
 * gets control, linked to a master managed timer that is passed as a
 * parameter to the process.  As the IDB timers expire, we send the ARPs
 * on those interfaces, and then free the timers (they were malloc'ed when
 * created).
 *
 * When there are no longer any running timers subordinate to the master,
 * everything is done, so we free the master timer and destroy the process.
 */
static forktype ip_gratuitous_arp_process (void)
{
    mgd_timer *master;
    mgd_timer *idb_timer;
    idbtype *idb;

    /* Get the master timer pointer. */
    
    if (!process_get_arg_ptr((void**)&master)) { /* Uhoh */
	process_kill(THIS_PROCESS);
	return;
    }

    /* Now sleep on the timer. */

    process_watch_mgd_timer(master, ENABLE);

    while (mgd_timer_first_child(master)) { /* As long as there are children */
	process_wait_for_event();

	/* The only events are timer expirations. */

	while ((idb_timer = mgd_timer_first_expired(master))) {
	    idb = mgd_timer_context(idb_timer);

	    /* Got one.  Send the ARP if appropriate. */
	    if (idb->ip_enabled && idb->ip_address && interface_up(idb) &&
		ip_arp_supported_idb(idb)) {
		ip_arp_unsolicited(idb, idb->ip_address, NULL, NULL);
	    }

	    /* Stop the timer and free it. */

	    mgd_timer_stop(idb_timer);
	    free(idb_timer);
	}
    }

    /* All the children have left home.  Retire to Florida. */

    free(master);
    process_kill(THIS_PROCESS);
}

/* 
 * ip_arp_announcement
 * Annouce us to the world, by sending unsolicited ARPs on all interfaces.
 *
 * If there are down interfaces that aren't quite up yet, we spawn a process
 * to send the ARPs later.
 */

void ip_arp_announcement (void)
{
    idbtype *idb;
    mgd_timer *master;
    mgd_timer *idb_timer;
    pid_t pid;

    master = NULL;

    /* Check all interfaces. */

    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled || !idb->ip_address)
	    continue;
	if (ip_arp_supported_idb(idb)) {
	    /* If the interface is up, send the ARP immediately. */

	    if (interface_up(idb)) {
		ip_arp_unsolicited(idb, idb->ip_address, NULL, NULL);
            } else {

		/*
		 * Interface is down.  If it's FDDI or token ring and it's
		 * not administratively down, fire off a timer to give it
		 * time to come up, and then give it a shot.
		 */
		if ((idb->hwptr->status & (IDB_FDDI | IDB_TR))
		    && (!interface_admin_down(idb))){

		    /*
		     * If this is the first, create the process and
		     * master timer.
		     */
		    if (!master) {
			pid = process_create(ip_gratuitous_arp_process,
					     "IP Gratuitous ARP",
					     SMALL_STACK,
					     PRIO_LOW);
			if (pid == NO_PROCESS)
			    return;	/* Punt... */
			master = malloc(sizeof(mgd_timer));
			if (!master) {
			    process_kill(pid);
			    return;	/* Punt--we're in deep doodoo. */
			}
			mgd_timer_init_parent(master, NULL);
			process_set_arg_ptr(pid, master);
		    }

		    /* Create a timer for the interface. */

		    idb_timer = malloc(sizeof(mgd_timer));
		    if (idb_timer) {
			mgd_timer_init_leaf(idb_timer, master, 0, idb, FALSE);
			mgd_timer_start(idb_timer, idb->arp_gra_time * ONESEC);
		    }
		}
	    }
	}
    }
}

/*
 * ip_arp_send_gratuitous_arps
 * Send gratuitous ARP reply out all relevent enabled IP interfaces
 * regarding a P2P neighbor address.
 * Don't send the arps on the interface that the neighbor is on.
 */

void ip_arp_send_gratuitous_arps (idbtype *p2p_idb, ipaddrtype addr)
{
    idbtype *idb;

    if (!addr || !ip_gratuitous_arps)
	return;
    FOR_ALL_SWIDBS(idb) {
	if (p2p_idb == idb)
	    continue;
	if (!interface_up(idb) || !idb->ip_enabled)
	    continue;
	if (!ip_arp_supported_idb(idb))
	    continue;
	ip_arp_unsolicited(idb, addr, NULL, NULL);
    }
}
