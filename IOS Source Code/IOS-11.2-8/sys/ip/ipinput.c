/* $Id: ipinput.c,v 3.8.4.23 1996/09/06 23:53:10 bew Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipinput.c,v $
 *------------------------------------------------------------------
 * ipinput.c -- IP input and gateway processing
 *
 * 20-August-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipinput.c,v $
 * Revision 3.8.4.23  1996/09/06  23:53:10  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Copy cryptoflags from first fragment during reassembly of fragments.
 *
 * Revision 3.8.4.22  1996/08/28  12:49:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.4.21  1996/07/29  22:45:51  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.8.4.20  1996/07/23  13:24:28  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.4.19  1996/07/13  11:39:02  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.8.4.18  1996/07/02  23:11:14  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.8.4.17  1996/06/18  01:45:38  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.8.4.16  1996/05/31  21:16:14  ahh
 * CSCdi59210:  IP: translation should happen before policy routing
 * Branch: California_branch
 *
 * Revision 3.8.4.15  1996/05/28  20:31:26  albright
 * CSCdi58660:  Router responses to network pings may have incorrect
 * source address.
 * Branch: California_branch
 *
 * Revision 3.8.4.14  1996/05/17  11:25:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.4.6  1996/05/11  03:30:30  ppearce
 * CSCdi57021:  Router crashes when RSRB FST traffic is received
 * Branch: IbuMod_Calif_branch
 *   Check for null idb before accessing fields within
 *
 * Revision 3.7.4.5  1996/05/05  23:13:03  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.4.4  1996/04/26  07:55:52  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.4.3  1996/04/03  14:42:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.4.2  1996/03/17  17:52:59  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.4.1  1996/02/28  21:24:02  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.13  1996/05/14  16:02:29  albright
 * CSCdi57188:  Debug IP Policy Output is misleading / Cosmetic Bug
 * Branch: California_branch
 *
 * Revision 3.8.4.12  1996/05/13  23:20:55  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.8.4.11  1996/05/11  03:28:58  ppearce
 * CSCdi57021:  Router crashes when RSRB FST traffic is received
 * Branch: California_branch
 *   Check for null idb before accessing fields within
 *
 * Revision 3.8.4.10  1996/05/04  01:04:29  wilber
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
 * Revision 3.8.4.9  1996/04/24  00:06:20  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.8.4.8  1996/04/19  15:42:30  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.22  1996/04/17  16:31:58  xliu
 * Branch: IosSec_branch
 * Moved crypto commands from crypto_public.h to crypto_engine_api.h.
 *
 * Revision 2.1.6.21  1996/04/11  23:15:15  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 2.1.6.20  1996/04/09  20:38:31  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.19  1996/03/30  05:39:44  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 2.1.6.18  1996/03/30  05:19:28  carrel
 * cvs fixes
 *
 * Revision 2.1.6.17  1996/03/19  08:09:36  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.16  1996/03/16  01:52:54  xliu
 * Branch: IosSec_branch
 * Add checking of null crypto vector.
 *
 * Revision 2.1.6.15  1996/03/01  01:37:38  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.14  1996/02/21  03:30:15  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.13  1996/02/13  19:18:16  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.12  1996/01/23  05:43:40  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.11  1996/01/19  06:55:33  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 2.1.6.10  1996/01/04  19:03:22  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 2.1.6.9  1995/12/17  01:40:35  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.8  1995/12/10  09:39:22  xliu
 * Branch: IosSec_branch
 * Moved encryption code to ipsendnet, after fragmentation.
 *
 * Revision 2.1.6.7  1995/11/17  17:01:17  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.6  1995/10/10  23:41:00  xliu
 * Branch: IosSec_branch
 * Fixed naming problem.
 *
 * Revision 2.1.6.5  1995/09/26  04:13:19  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.6.4  1995/09/08  08:00:29  xliu
 * Branch: IosSec_branch
 * Eliminate CRYPTO_FAST_ENCRYPT/DECRYPT cryptoflags. Replace it with the
 * switch_mode input parameters to epa encrypt/decrypt api.
 * Replace CRYPTO_DONE cryptoflag with CRYPTO_ENCRYPT_DONE and
 * CRYPTO_DECRYPT_DONE
 * flag to fix the bug caused by reused pak in ping application.
 *
 * Revision 2.1.6.3  1995/08/11  01:01:15  xliu
 * Branch: IosSec_branch
 *
 * Add encryption/decryption to low end fastswitching code (ipfast_les.c).
 *
 * Revision 2.1.6.2  1995/07/21  04:39:03  xliu
 * Branch: IosSec_branch
 * Modified the decryption code path by calling epa_decrypt from ip_enqueue
 * or ipsendnet, depending on the destination.
 *
 * Revision 2.1.6.1  1995/07/01  05:33:14  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.8.4.7  1996/04/17  13:41:55  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.8.4.6  1996/04/16  18:53:17  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.8.4.5  1996/04/15  21:16:29  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.8.4.4  1996/04/02  05:36:32  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.8.4.3  1996/03/29  02:31:40  hou
 * CSCdi51527:  vLAN ISL does not fast switch ipx ping echo return packets
 * Branch: California_branch
 * and also check swidb instead of hwidb to determine input interface for
 * 7000 platform.
 * - isl_fastswitch():
 *   set input_hwidb->fast_rcvidb to input vlan interface after vlan
 *   demultiplex.
 * - internet_enqueue():
 *   pass packet to IP queue only when the input swidb is IP enabled.
 * - nov_set_nidb_idb():
 *   assign novell idb by checking swidb instead of hwidb
 *
 * Revision 3.8.4.2  1996/03/27  21:57:06  mleelani
 * CSCdi49709:  Broadcast/multicast translation using helpers not
 * functional.
 * Branch: California_branch
 * Use helper addresses to provide the translation
 *
 * Revision 3.8.4.1  1996/03/18  20:17:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  06:59:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  01:38:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  09:46:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  01:00:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/09  22:55:49  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/02/08  17:16:28  albright
 * CSCdi48312:  IP helper addresses malfunctioning
 *
 * Revision 3.6  1996/01/19  00:57:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/11/22  17:47:15  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.4  1995/11/22  06:56:51  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.3  1995/11/17  09:35:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:57:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:05:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/20  18:05:58  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.10  1995/10/19  08:05:21  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.9  1995/09/25  08:32:18  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.8  1995/09/11  23:49:25  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.7  1995/09/09 00:53:19  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.6  1995/09/02  18:11:16  raj
 * CSCdi39164:  WFQ alignment errors on 4500
 * The IP header is not necessarily word aligned and thus when the compiler
 * outputs a load word ("lw") instruction in order to retrieve parts of the
 * iphdrtype structure, it results in alignment errors on a MIPS.  We avoid
 * this by forcing the compiler to use GETSHORT in order to retrieve the
 * structure fields instead.
 *
 * Revision 2.5  1995/08/28  21:46:17  jenny
 * CSCdi39127:  TCP header compression debugs print negative numbers
 *
 * Revision 2.4  1995/08/28  20:39:41  ahh
 * CSCdi37637:  TCP Header Compression causes ICMP TTL Expired over...
 * Do the TTL check on the right IP header when uncompression hands back a
 * new packet.
 *
 * Revision 2.3  1995/08/15  00:20:07  bcole
 * CSCdi38170:  Ping to HSRP IP address causes ICMP echo request storm
 *
 * Add notion of dynamic IP aliases, so that HSRP virtual IP address is
 * now pingable.  Eliminate packet forwarding problem for this virtual
 * IP address as a side effect.
 *
 * Revision 2.2  1995/07/12  23:02:18  bcole
 * CSCdi28695: parallel paths on the same subnet cause erroneous IP
 * tfk. forwarding
 *
 * When equal cost load balancing over multiple directly connected
 * interfaces, be sure to locally receive IP packets destined for us,
 * instead of possibly forwarding the packet back out one of the alternate
 * interfaces.
 *
 * Revision 2.1  1995/06/07  20:59:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "globs.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "globs.h"
#include "../ui/debug.h"
#include "ip.h"
#include "ip_debug.h"
#include "ip_registry.h"
#include "udp_debug.h"
#include "logger.h"
#include "access.h"
#include "ipaccess.h"
#include "ipoptions.h"
#include "../ip/ipaddress.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../iprouting/route.h"
#include "../tcp/tcpcompress.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "boxtypes.h"
#include "access.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

/*
 * Miscellaneous external definitions
 */
extern ipaddrtype rsrb_fst_name;


/*
 * fair conversation generator
 *
 * return weights such that precedence gives you the ability to
 * forward more data if you have more precedence - without giving
 * away the store
 */
static const ulong ip_precedence_weight[] = {
    FAIR_QUEUE_BASE_WEIGHT/1,
    FAIR_QUEUE_BASE_WEIGHT/2,
    FAIR_QUEUE_BASE_WEIGHT/3,
    FAIR_QUEUE_BASE_WEIGHT/4,
    FAIR_QUEUE_BASE_WEIGHT/5,
    FAIR_QUEUE_BASE_WEIGHT/6,
    FAIR_QUEUE_BASE_WEIGHT/7,
    FAIR_QUEUE_BASE_WEIGHT/8
};
/*
 * the following is a short term HACK to be replaced with a registry */

short ip_fair_queue_flow_id (paktype *pak)
{
    iphdrtype *ip;
    int sum;
    uchar tos;

    if (pak->if_output->rsvp_info) {
	if ((sum = reg_invoke_ip_rsvp_classify(0, pak)))
	    return(sum);
    }

    ip = (iphdrtype *)ipheadstart(pak);
    if (IPINIP_PROT == ip->prot)
	ip = (iphdrtype *) ipdatastart_iph(ip);
    tos = ip->tos;
    pak->fair_weight = ip_precedence_weight[tos >> 5];
    sum = (tos & IP_TOS_MASK) + ip->prot + /* hash IP protocol, and TOS */
	xnethash((uchar *)&ip->srcadr, 8); /* source and destination address */
    if (!get_ip_morefragments(ip) && 0 == getfo(ip) &&
	(TCP_PROT == ip->prot || UDP_PROT == ip->prot)) {
	sum += xnethash(ipdatastart_iph(ip), 4); /* UDP/TCP Ports */
    }
    return(fair_queue_sum(pak, sum));
}
/*
 * extract precedence and return to RED
 */
short ip_red_precedence (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);
    return(ip->tos >> IP_PRECEDENCE_SHIFT);
}

/*
 * ip_dispose
 * discard an IP input packet
 */
void ip_dispose (ulong *counter, paktype *pak, char *str)
{
    char *buf;

    (*counter)++;
    
    if (iperror_debug) {
	buf = string_getnext();
	sprintf(buf, "dispose %s", str);
	ipbug_error(pak, buf);
    }

    retbuffer(pak);			/* give it back */
}

/*
 * First run some sanity checks on various fields. IP version
 * must be correct.  Total packet length must not be greater
 * than our MTU and not less than a minimum IP header.
 * The IP header length must 5 or greater.  Total packet length
 * must be less than size recorded by hardware.
 *
 * Also, do a rudimentary check here for IP Security.  If there is
 * any security in the box, we must call inproc.  This is
 * because inproc computes the definative security label used
 * everywhere else.  Inproc processes any IP options present.
 * If they weren't right for some reason drop the packet.
 * All appropriate actions have already been taken (such as
 * sending ICMP messages).
 */

static inline boolean ip_sanity_check_inline (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if ((ip->version != IPVERSION) ||
	(ip->tl > MAXDATAGRAMSIZE) ||
	(ip->tl < MINIPHEADERBYTES) ||
	(ip->ihl < btol(MINIPHEADERBYTES)) ||
	(ip->tl > (pak->datagramsize -
		    (pak->network_start - pak->datagramstart)))) {
	IP_DISPOSE(ip, formaterr, pak);
	return(FALSE);
    }
    
    /*
     * Make sure IP checksum is correct
     */
    if (ipcrc((ushort *)ip,(ip->ihl << 2))) {
	IP_DISPOSE(ip, checksumerr, pak);
	return(FALSE);
    }

    if ((ip->ihl > btol(MINIPHEADERBYTES)) || ip_secopt_bytes) {
	if (!ipopt_inproc(pak)) {
	    retbuffer(pak);
	    return(FALSE);
	}
    }

    return(TRUE);
}

/*
 * ip_sanity_check
 * For callers outside this file
 */

boolean ip_sanity_check (paktype *pak)
{
    return(ip_sanity_check_inline(pak));
}

/*
 * internet_enqueue
 * Interrupt level enqueue routine for Internet Protocol packets 
 *
 * Accept IP packets only if any software interface associated with the
 * physical input interface is enabled for IP. Also if routing on a
 * subinterface (e.g. vLANs); determine the corresponding Software IDB,
 * so that the routing protocols can install the correct routes etc.
 */

void internet_enqueue (paktype *pak)
{
    acl_headertype *acclist;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (pak->if_input->ip_enabled) {

	/*
	 * RSRB likes to get its packets at interrupt level.
	 *
	 * This check is designed to work with the vast majority of packets
	 * being filtered by the first conjunct (comparing dstadr to
	 * rsrb_fst_name).
	 */
	if (((ip->dstadr) == rsrb_fst_name) &&
	    ((ip->prot) == FST_RSRB_PROT)) {
	    if (ip_sanity_check_inline(pak)) {
		acclist = pak->if_input->ip_input_accesslist;
		if (acclist == NULL ||
		    ip_accesscheck_snpa(pak, acclist, FALSE)) {
		    reg_invoke_vrfst_input(pak);
		    return;
		}
	    } else { /* if sanity checking failed return;
			if access lists failed...fall through */
                return;
            }
	}

	process_enqueue_pak(IPpacketQ, pak);

    } else {
	protocol_discard(pak, !(BRIDGING_IF(pak->if_input, LINK_IP)));
    }
}

/*
 * ip_simple_enqueue
 * Just put the frame, sourced by us, on the internet enqueue to be forwarded.
 * Don't do the checks of 'internet_enqueue'. Used by RSRB/FST.
 */

void ip_simple_enqueue (paktype *pak)
{
    process_enqueue_pak(IPpacketQ, pak);
}

/*
 * ip_process_input
 *
 * Process any packets that are on the IP input queue.
 */
static void ip_process_input (void)
{
    paktype *pak;
    iphdrtype *ip;
    int count;
    idbtype *idb;
    acl_headertype *acclist;
    boolean absorbed;

    /* Process only a limit number of packets per pass. */
    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	/*
	 * Dequeue an IP datagram
	 */
	pak = process_dequeue(IPpacketQ);
	if (!pak)
	    break;
	ip_traffic.inputs++;
	idb = pak->if_input;
	ip = (iphdrtype *)ipheadstart(pak);
	/*
	 * If we generated the packet, ignore decompression,
	 * input access lists and decryption.
	 */
	if (idb) {
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_IP);

	    if (pak->linktype == LINK_COMPRESSED_TCP ||
		pak->linktype == LINK_UNCOMPRESSED_TCP) {
		pak = reg_invoke_uncompress_tcpip_header(LINK_IP, pak,
							 idb->hwptr,
							 idb->hwptr->tcp_compress);
		if (!pak)
		    continue;
		/*
		 * Header compression could have returned a new packet
		 * (for those packets which cannot be uncompressed
		 * in-place), so refresh the pointer to the IP header.
		 */
		ip = (iphdrtype *)ipheadstart(pak);
	    } else {
	        /* note uncompress does a sanity check for us. */

		/*
		 * the code below makes up for a microcode flaw wherein
		 * we decrement a ttl of 0, incorrectly update the
		 * checksum (because the quick'n'dirty recalc doesn't
		 * work when you wrap) and pass up the packet.  the
		 * system should accept a packet with a ttl of zero if
		 * the packet is destined "for us".
		 */
		if ((ip->ttl == 255) && (pak->flags & PAK_MCICHECKED)) {
		    ip->ttl = 0;
		    ip->checksum = 0;
		    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);
		}
		/* Does the packet meet some simple sanity/security checks? */
		if (!ip_sanity_check_inline(pak)) 
		    continue;
	    }
	    /*
	     * Check crypto map for decryption.The decryption actually 
             * happens in ipsendnet and ip_enqueue.
	     */
	    if (idb->crypto_map && !(pak->cryptoflags & CRYPTO_DECRYPT_DONE)) {
		if (!reg_invoke_crypto_map_check(pak, idb->crypto_map,
						 CRYPTO_DECRYPT, FALSE)) {
		    continue;
		}

		if (crypto_deny_packet(pak)) {
                    if (pak->cryptoflags & CRYPTO_DENY) {
		        if (ippkt_debug)
			    ipbug(pak, "crypto map check failed.");
                    } else {
                        if (pak->cryptoflags & CRYPTO_CONN_NON_EXIST) {
                            if (ippkt_debug)
                               ipbug(pak, "crypto connection pending, packets"
                                          " dropped");
                        }
                    }
                    datagram_done(pak);
		    continue;
		}
		if (is_vip(idb->hwptr))
		    pak->cryptoflags &= ~CRYPTO_DECRYPT;
	    }

	    /*
	     * Make sure the input access list permits this packet
	     */
	    acclist = idb->ip_input_accesslist;
	    if (acclist && !ip_acc_ck_count_violations(pak, acclist)) {
		if (ippkt_debug)
		    ipbug(pak, "access denied");
		send_unreachable(pak, ICMP_UNRADMIN);
		accessfailcnt++;
		retbuffer(pak);
		continue;
	    }
	} else {
	    if (!ip_sanity_check_inline(pak)) 
		continue;
	}

	/*
	 * If we're an operational gateway, try forwarding the IP.
	 * The packet is either disposed of or it is for us.
	 */
	if (ip_router_running && systeminit_flag) {
	    /*
	     * If MCI hasn't already checked, decrement time to live,
	     * check that it isn't zero, and discard if exceeded.
	     * If packet is for us, absorb it rather than drop it.
	     */
	    if ((pak->flags & PAK_MCICHECKED) == 0) {
		if (ipttl((uchar *)ip)) {
		    if (ip_forus(pak)) {
			if (ippkt_debug)
			    ipbug(pak, "rcvd 0");
			ip_enqueue(pak);
			continue;
		    }
		    send_timeexceeded(pak,ICMP_TIMTTL);
		    IP_DISPOSE(ip, hopcount, pak);
		    continue;
		}
		pak->flags |= PAK_MCICHECKED;
	    }

	    /*
	     * Do outside-to-inside translation if required.
	     * This needs to be after input access list checking,
	     * but before routing.
	     */
	    if (idb && idb->ip_info && idb->ip_info->nat_enabled &&
		idb->ip_info->nat_outside) {
		paktype *newpak;
		boolean drop;

		newpak = reg_invoke_translate_before_routing(LINK_IP, pak,
							     &drop);
		if (newpak) {
		    pak = newpak;
		    ip = (iphdrtype *)ipheadstart(pak);
		} else if (drop) {
		    if (ipnat_debug)
			buginf("\nNAT: translation failed (B), dropping packet"
			       " s=%i d=%i", ip->srcadr, ip->dstadr);
		    send_unreachable(pak, ICMP_UNRHOST);
		    IP_DISPOSE(ip, noroute, pak);
		    continue;
		}
	    }

	    if (pak->ip_flags & PAKOPT_HASALERT) {
		/*
		 * Packet has router alert option.  Possibly allow
		 * a higher level packet to deal with the switching
		 * of this packet.
		 */
		absorbed = FALSE;
		reg_invoke_iprouting_alert(ip->prot, pak, &absorbed);
		if (absorbed)
		    continue;
	    }

	    /*
	     * Try forwarding packet.  If ip_forward() fails, then
	     * the packet is for us.
	     */
	    if (ip_forward(pak))
		continue;
	    ip_enqueue(pak);
	    continue;
	}

	/*
	 * Not an IP router, check for local destination.
	 * We could be acting as a host, a bridge, or both.
	 */
	if (ip_forus(pak)) {
	    /*
	     * If this is a broadcast packet and we're bridging IP,
	     * possibly pass a copy to the bridge flooding process.
	     * Fix up TTL field since we're bridging, not routing.
	     */
	    if (BRIDGING_IF(pak->if_input, LINK_IP) &&
		(pak->flags & (PAK_BROADCAST|PAK_MULTICAST))){
		if (pak->flags & PAK_MCICHECKED)
		    ipttl_fixup(pak);
		net_bridge_copy_broadcast(pak);
	    }
	    /*
	     * Sending it up our stack is considered a completed
	     * route.  We need to do a postroute to be complete.
	     */
	    if (pak->ip_flags &
		(PAKOPT_HASROUTE|PAKOPT_HASTSTMP|PAKOPT_HASSEC)) {
		pak->if_output = pak->if_input;
		if (!ipopt_postroute(pak, pak->if_output->ip_address)) {
		    retbuffer(pak);
		    continue;
		}
	    }
	    pak->if_output = NULL;
	    if (ippkt_debug)
		ipbug(pak, "rcvd 1");
	    ip_enqueue(pak);
	    continue;
	}

	/*
	 * We're not a router and the datagram is not for us. 
	 * If this is some sort of multicast and we're acting as
	 * a bridge, pass the packet along to the bridging flooder.
	 * Fix up TTL field since we're bridging, not routing.
	 */
	if (BRIDGING_IF(pak->if_input, LINK_IP) &&
	    physical_broadcast(pak) == FUZ_TRUE) {
	    if (pak->flags & PAK_MCICHECKED)
		ipttl_fixup(pak);
	    net_bridge_enqueue_broadcast(pak);
	    continue;
	}
	IP_DISPOSE(ip, notgateway, pak);
    }
}

/*
 * ip_input
 * The process that redistributes IP datagrams
 */

process ip_input (void)
{
    sys_timestamp check_reassembly;
    ulong major, minor;

    TIMER_START(check_reassembly, ONEMIN);
    process_watch_timer(&check_reassembly, ENABLE);
    process_watch_queue(IPpacketQ, ENABLE, RECURRING);

    while (TRUE) {
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		ip_process_input();
		break;

	      case TIMER_EVENT:
		TIMER_UPDATE(check_reassembly, ONEMIN);
		ip_ratimeout();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * ip_forus
 * Returns TRUE if datagram is destined for one of
 *  - any interface address in the box,
 *  - an alias address,
 *  - a valid broadcast address on the input interface
 *  - a valid broadcast address on the output interface, or
 *  - a multicast address on the input interface.
 */

boolean ip_forus (paktype *pak)
{
    iphdrtype *ip; 
    idbtype *swidb;
    ipaddrtype target; 
    ipaddr_tabletype *addr;

    /*
     * If we have a source route in progress it can't be for us.
     */
    if (pak->ip_flags & PAKOPT_SRNOTEND)
	return(FALSE);

    /*
     * Optimize this lookup.  The two most common cases are the
     * primary input interface address and the preferred broadcast address.
     * The following code is redundant with the unicast hash search
     * broadcast check further below.
     */
    ip = (iphdrtype *)ipheadstart(pak);
    swidb = pak->if_input;
    target = GETLONG(&ip->dstadr);
    
    if (swidb) {
	if (swidb->ip_unnumbered)
	    swidb = swidb->ip_unnumbered;
	if (target == swidb->ip_address)
	    return(TRUE);
	if (target == swidb->ip_broadcast) {
	    ip_traffic.bcastin++;
	    pak->flags |= PAK_BROADCAST;
	    target = GETLONG(&ip->srcadr);
	    if (target == 0L)
		return(TRUE);
	    FOR_IPADDR_HASH_CHAIN(target, addr) {
		if (addr->address == target)
		    return(FALSE);
	    }
	    return(TRUE);
	}
    }

    /*
     * Quick check to see if this is a multicast address.
     * Limit check to those multicast addresses on the input interface.
     * Older code used to check for matches on all interfaces, but this is
     * much too expensive on high density routers.
     */
    if (IPMULTICAST(target)) {
	if (pak->if_input && 
	    ((pak->if_input->ip_mrouting && ip->prot == IGMP_PROT) || 
	     ip_multicast(target, pak->if_input))) {
	    ip_traffic.mcastin++;
	    pak->flags |= PAK_MULTICAST;
	    return(TRUE);
	} else
	    return(FALSE);
    }

    /*
     * Not one of the two most common addresses or multicast.
     * Search the hash chain for a unicast address
     */
    FOR_IPADDR_HASH_CHAIN(target, addr) {
	if (addr->address == target) {
	    switch (addr->type) {
	    default:
	    case IPADDR_INTERFACE:
		if (interface_up_simple(addr->swidb))
		    return(TRUE);
		break;
	    case IPADDR_ALIAS:
	    case IPADDR_DYN_ALIAS:
		pak->flags |= PAK_ALIAS;
		return(TRUE);
		break;
	    }
	}
    }

    /*
     * If we have a broadcast, count it and make
     * sure source address is not ours.
     * Bootp has zero source address -- let those through
     */
    if (swidb && ip_broadcast(target, swidb)) {
	ip_traffic.bcastin++;
	pak->flags |= PAK_BROADCAST;
	target = GETLONG(&ip->srcadr);
	if (target == 0L)
	    return(TRUE);
	FOR_IPADDR_HASH_CHAIN(target, addr) {
	    if (addr->address == target)
		return(FALSE);
	}
	return(TRUE);
    }

    /* 
     * Capture all incoming BOOTP replies for we have no 
     * clue who we are. 
     */
    if (swidb && !netsareup && !swidb->ip_address && !swidb->ip_unnumbered)
	return(TRUE);
    /*
     * Web install in progress...
     */
    if (reg_invoke_ip_webinstall_forus(ip))
	return(TRUE);

    /*
     * Check if it is a broadcast on the output interface.
     * It take care of the case when the packet address to 
     * a directed broadcast address. This check is specifically 
     * used after ip_forward_to_net() return FALSE. 
     * (In other occurrences, pak->if_output will be NULL or be
     * the same as pak->if_input so the following test will not take 
     * place at all)
     * 
     * Make sure you understand the relationship among ip_input(),
     * ip_route() and ip_forus() before making any change.
     *
     * We have to count the broadcast and make
     * sure source address is not ours.
     * Bootp has zero source address -- let those through
     */
    swidb = pak->if_output;
    target = GETLONG(&ip->dstadr);
    if (swidb && ip_broadcast(target, swidb)) {
	ip_traffic.bcastin++;
	pak->flags |= PAK_BROADCAST;
	target = GETLONG(&ip->srcadr);
	if (target == 0L)
	    return(TRUE);
	FOR_IPADDR_HASH_CHAIN(target, addr) {
	    if (addr->address == target)
		return(FALSE);
	}
	return(TRUE);
    }
    
    return(FALSE);
}

/*
 * ip_forward_dbcast
 * Indicate whether we will allow ip_forward_to_net() to forward this
 * broadcast.
 */
boolean ip_forward_dbcast (void)
{
    return(TRUE);
}

/*
 * ip_forward_to_net
 * Attempt to send a routed IP packet out an interface.  
 * If we are routing a directed broadcast to a directly connected
 * destination, it is time to convert the packet into a physical broadcast.
 * Protocols supported by 'ip forward-protocol' handle this situation
 * separately with the local copy of the packet so don't broadcast
 * such packets here - unless filtered out by the access list.
 *
 * Returns TRUE if delivery succeeded and no further processing of this
 * packet is necessary.  Return value of FALSE implies that this packet
 * may be directed at one of our interfaces and require local processing.
 */

static boolean ip_forward_to_net (paktype *pak)
{
    iphdrtype *ip, *copy_ip;
    paktype *copy;
    idbtype *output;

    ip = (iphdrtype *)ipheadstart(pak);
    output = pak->if_output;

    if ((pak->desthost != ip->dstadr) || !ip_broadcast(ip->dstadr, output)) {
	if (ippkt_debug)
	    ipbuggw(pak, "forward");
	return(ipsendnet(pak, FALSE, TRUE));
    }
    /*
     * The packet is a directed broadcast with a directly connected
     * destination.  Do we want to send it out as a physical broadcast?
     */
    if (!output->ip_directbcast)
	return(FALSE);

    /*
     * See if the upper layers handle forwarding the packets already.
     */
    if (reg_invoke_ip_forward_dbcast(ip->prot) == FALSE)
	return(FALSE);
    if (output == pak->if_input) {
        if (physical_broadcast(pak) == FUZ_TRUE)
            return(FALSE);
    }
    if (IPBROADCASTLIST_CONFIGURED(pak->if_output) &&
	!ip_old_accesscheck(pak, output->ip_directbcast)) {
	if (ippkt_debug)
	    ipbug(pak, "access denied writing broadcast");
	return(FALSE);
    }

    /*
     * Take a copy of our original frame
     */
    copy = pak_duplicate(pak);
    if (!copy) {
	return(FALSE);
    }

    copy_ip = (iphdrtype *)ipheadstart(copy);

    copy->desthost = (ipaddrtype) -1;
    if (ippkt_debug)
	ipbuggw(copy, "forward directed broadcast");

    copy_ip->dstadr = output->ip_broadcast;
    copy_ip->checksum = 0;
    copy_ip->checksum = ipcrc((ushort *)copy_ip, copy_ip->ihl << 2);
    (void) ipsendnet(copy, TRUE, TRUE);
    return(FALSE);
}

/*
 * ip_route_failure
 * We've tried to route the packet in a number of ways and all have failed.
 * Check corner conditions.
 */

static inline boolean ip_route_failure (paktype *pak)
{
    if (pak->flags & PAK_ABORT) {
	retbuffer(pak);
	return(TRUE);
    }
    if (ip_forus(pak)) {		/* failed routing, for us? */
	if (ippkt_debug)
	    ipbug(pak, "rcvd 2");
	
	/*
	 * Finish processing options prior to handing it off
	 * to our upper levels.  Only do this if we have options
	 * that need to be processed.
	 */
	if (pak->ip_flags & (PAKOPT_HASROUTE | PAKOPT_HASTSTMP
			     | PAKOPT_HASSEC)) {
	    pak->if_output = pak->if_input;	/* get through security */
	    if (!ipopt_postroute(pak, pak->if_output->ip_address)) {
		retbuffer(pak);	/* dispose of the remains */
		return(TRUE);		/* and it's been handled */
	    }
	}
	pak->if_output = NULL; /* it's local */
	
	/*
	 * Return FALSE so our caller knows it hasn't been handled yet.
	 * ip_forward's caller will enqueue it if appropriate.
	 */
	return(FALSE);
    }
    
    /*
     * No route, not for us
     */
    if (ippkt_debug)
	ipbug(pak, "unroutable");
    send_unreachable(pak,ICMP_UNRHOST);
    IP_DISPOSE(ip, noroute, pak);
    return(TRUE);
}

/*
 * ip_forward
 * Forward an IP datagram.  Returns TRUE if Datagram was passed to lower
 * levels or disposed of, FALSE if for us. For speed, gateways also attempt
 * to route packets destined for themselves. The exception cases below make
 * the code more complex, but faster. We assume that if routing succeeds and
 * the packet is for us, the output interface will be the same as the input
 * interface.
 *
 * It is assumed that ipopt_inproc has been called for this packet.  This
 * ensures that the packet option flags and control cells have been properly
 * set up.  If this assumption is violated routing will not work right in
 * all cases.
 */

boolean ip_forward (paktype *pak)
{
    iphdrtype *ip;
    boolean routed, policy_routed, drop;
    ipaddtype *ptr;
    idbtype *if_input, *if_output;
    paktype *newpak;

    ip = (iphdrtype *)ipheadstart(pak);
 
    /*
     * First, figure out how we would route it if we could.
     * Special case destination of 0.0.0.0, as it should really
     * be considered a broadcast address.
     */
    routed = ip->dstadr ? ip_route(pak, IP_PATH_FAIL_FORUS) : FALSE;
    
    if_input = pak->if_input;

    /*
     * If this packet is subject to policy routing, do that instead.
     */
    policy_routed = FALSE;
    if (if_input && if_input->ip_info && if_input->ip_info->policy_enabled) {
	if (reg_invoke_ip_policy_forward(pak)) {
	    policy_routed = TRUE;
	}
    }

    if_output = pak->if_output;

    /*
     * Do inside-to-outside translation if required.
     * This needs to be after routing but before output
     * access list checking.
     */
    if (if_input && if_output && (if_input != if_output) && 
        if_input->ip_info && if_output->ip_info && 
        if_input->ip_info->nat_enabled && if_output->ip_info->nat_enabled &&
	!if_input->ip_info->nat_outside && if_output->ip_info->nat_outside) {
	newpak = reg_invoke_translate_after_routing(LINK_IP, pak, &drop);
	if (newpak) {
	    pak = newpak;
	} else if (drop) {
	    if (ipnat_debug)
		buginf("\nNAT: translation failed (A), dropping packet"
		       " s=%i d=%i", ip->srcadr, ip->dstadr);
	    send_unreachable(pak, ICMP_UNRHOST);
	    IP_DISPOSE(ip, noroute, pak);
	    return(TRUE);
	}
    }

    if (!policy_routed) {
	/*
	 * If this is a multicast packet, use multicast forwarding algorithm.
	 * Return TRUE, when ip_mforward() will be responsible for delivering
	 * packet - both out and up the protocol stack if we are a group
	 * member. 
	 */
	if (IPMULTICAST(ip->dstadr)) {
	    pak->if_input->ip_multicast_in++;
	    if (reg_invoke_ip_mforward(pak)) {
		return(TRUE);
	    }
	}
	
	/*
	 * Try routing the packet.  If we can't, check if it is for us and
	 * if not, send an ICMP destination (host) unreachable.
	 */
	if (!routed) {
	    return(ip_route_failure(pak));
	}
    }

    if (!pak->desthost) {
	if (ippkt_debug)
	    ipbug(pak, "0.0.0.0 desthost 1");
	retbuffer(pak);
	return(TRUE);
    }
    
    /*
     * If the packet is headed for Null 0 and we have unreachables enabled,
     * then generate the unreachable now.
     */
    if ((if_output->output_if == nullidb) && 
	(nullidb->ip_unreach == TRUE)) {
	send_unreachable(pak, ICMP_UNRHOST);
	retbuffer(pak);
	return(TRUE);
    }

    /*
     * We now have a successfully routed packet.  Options have been
     * fully processed for this side.
     * 
     * Check if we should send an ICMP redirect. If the input and output
     * Interfaces are the same, first check if it is for us.  Otherwise, do
     * so if input and output interfaces are the same and if the originating
     * host is on the same net as the destination gateway.  send_redirect
     * does further checking for ICMP messages and broadcasts.  Entire
     * action controlled by redirect flag for the outgoing interface.
     *
     * If the source of the packet is us, do not send a redirect back
     * to us.
     */
    if (if_input == if_output) {
        /*
         * If it is a subnet direct broadcast, but not physical broadcast,
         * we need to route the packet back to the if_input.
         */
        if (!ip_broadcast(ip->dstadr, if_input) ||
            physical_broadcast(pak) == FUZ_TRUE) {

            if (ip_forus(pak)) {
                if (ippkt_debug)
                    ipbug(pak, "rcvd 3");
                pak->if_output = NULL;
                return(FALSE);
            }
            if (if_output->ip_redirect && !(pak->flags & PAK_NOREDIRECT)) {
                ulong mask = if_input->ip_nets_mask;

                if (ip_ouraddress(ip->srcadr)) {
                    if (ippkt_debug)
                        ipbug(pak, "rcvd local pkt");
                    retbuffer(pak);
                    return(TRUE);
                }

                if ((ip->srcadr & mask) == (pak->desthost & mask)) {
                    if (ippkt_debug)
                        ipbug(pak, "redirected");
                    send_redirect(pak, pak->desthost);
                }
            }
        }
    } else if (pak->desthost == if_output->ip_address) {
	/*
  	 * Check for routing to our own interface
  	 */
  	pak->if_output = NULL;
  	if (ippkt_debug)
	    ipbug(pak, "rcvd 4");
  	return(FALSE);
    } else if ((pak->desthost == ip->dstadr) && ip_aliasaddress(ip->dstadr)) {
  	pak->if_output = NULL;
	pak->flags |= PAK_ALIAS;
  	if (ippkt_debug)
	    ipbug(pak, "rcvd 6");
  	return(FALSE);
    } else {
	/*
	 * Check for routing to one our secondary interface
	 * addresses
	 */
	for (ptr = (ipaddtype *) if_output->ip_addressQ.qhead;
	     ptr; ptr = ptr->next)  
	    if (pak->desthost == ptr->address) {
		pak->if_output = NULL;
		if (ippkt_debug)
		    ipbug(pak, "rcvd 7");
		return(FALSE);
	    }
    }

    /*
     * Check crypto map now.  Encryption will be done later, but the 
     * packet will get marked for encryption now.
     */
    if (pak->if_output->crypto_map) {
	if (!reg_invoke_crypto_map_check(pak, pak->if_output->crypto_map,
					 CRYPTO_ENCRYPT, FALSE)) {
	    return(TRUE);
	}
        if (crypto_deny_packet(pak)) {
            if (pak->cryptoflags & CRYPTO_DENY) {
                if (ippkt_debug)
		    ipbug(pak, "crypto map check failed.");
            } else {
                if (pak->cryptoflags & CRYPTO_CONN_NON_EXIST) {
                    if (ippkt_debug)
                       ipbug(pak, "crypto connection penging, packets"
                                  " dropped");
                }
            }
            datagram_done(pak);
            return TRUE;
	}
	if (is_vip(pak->if_output->hwptr))
	    pak->cryptoflags &= ~CRYPTO_ENCRYPT;
    }

    /*
     * Check if the outgoing interface has access restrictions.
     * Send an ICMP administratively prohibited if the check fails.
     */
    if (if_output->ip_output_accesslist &&
	!ip_acc_ck_count_violations((paktype *) pak, 
			if_output->ip_output_accesslist)) {
	if (ippkt_debug)
	    ipbug(pak, "access denied");

	/*
	 * Don't send unreachables for redirected packets
	 */
	if (if_input != if_output)
	    send_unreachable(pak, ICMP_UNRADMIN);
        accessfailcnt++;
	retbuffer(pak);
	return(TRUE);
    }

    if (!ip_forward_to_net(pak)) {
	if (ip_forus(pak)) {
	    if (ippkt_debug)
		ipbug(pak, "rcvd 5");
	    pak->if_output = NULL;
	    return(FALSE);
	} else {
	    retbuffer(pak);
	    return(TRUE);
	}
    }

    ip_traffic.forwarded++;		/* count another one */
    return(TRUE);
}

/*
 * ip_broadcast_check - See if a packet is a broadcast
 *
 * This routine is used to determine if a packet is a broadcast packet.
 * On media which support broadcast, we use the physical_broadcast()
 * routine to determine if something is a broadcast. On other media,
 * we consider something a broadcast if it is sent to a valid IP broadcast
 * address for that interface.
 */

fuzzy ip_broadcast_check (paktype *pak, boolean unnumbered_check)
{
    iphdrtype *ip;
    idbtype *interface;

    ip = (iphdrtype *)ipheadstart(pak);

    if (pak->if_input->hwptr->status & IDB_BCAST) {
	return(physical_broadcast(pak));
    } else {
	interface = pak->if_input;
	if (unnumbered_check) {
	    /*
	     * Check configured broadcast address first.
	     */
	    if (ip_broadcast(ip->dstadr, interface))
		return(FUZ_TRUE);
	    /*
	     * Now check with the unnumbered interface.
	     */
	    interface = interface->ip_unnumbered;
	    if (interface == NULL)
		return(FUZ_FALSE);
	}
	return(ip_broadcast(ip->dstadr, interface) ? FUZ_TRUE : FUZ_FALSE);
    }
}

/*
 * ip_forward_check
 * This routine is used by nd_forward() and udp_forward().  It checks
 * to see if a packet should be forwarded.  If so then it stuffs "address"
 * into the destination field in "pak".  After routing the packet then
 * it checks to see if the next hop is a local broadcast address and
 * diddles the packet appropriately.  It then leaves it up to the caller
 * to actually send the packet along.  This routine returns TRUE if it
 * is routeable, FALSE if it isn't routeable or shouldn't be forwarded.
 */

boolean ip_forward_check (paktype *pak, ipaddrtype address)
{
    iphdrtype *ip;
    idbtype *idb, *unnumbered;
    ipaddrtype dstadr;

    ip = (iphdrtype *)ipheadstart(pak);
    
    /*
     * We must be an initialized router to do forwarding of IP broadcasts.
     * The packet must have a broadcast IP address on at least one
     * of our interfaces (see ip_forus()).
     */
    if (!ip_router_running || !systeminit_flag ||
	(((pak->flags & PAK_BROADCAST) == 0) &&
	 ((pak->flags & PAK_MULTICAST) == 0)))
	return(FALSE);

    /*
     * If we're a broadcast on the received network, force a new
     * destination address if we have a helper address.
     */
    idb = pak->if_input;

    /*
     * Helper address is passed in as "address".  If it is not set,
     * then check to see if this packet is a directed broadcast.  If
     * address is set, check to see if this is a physical broadcast.
     * Then, check to see if it is an IP broadcast.  If both of these
     * pass, then fill in the helper address and attempt to route it.
     * Along with broadcasts, allow multicasts also to be helpered.
     */
    if (address) {
	dstadr = ip->dstadr;
	if (!IPMULTICAST(dstadr)) {
	    if (!ip_broadcast(dstadr, idb)) {
		unnumbered = idb->ip_unnumbered;
		if (unnumbered) {
		    if (!ip_broadcast(dstadr, unnumbered))
			return(FALSE);
		}
	    }
	    if (ip_broadcast_check(pak, TRUE) == FUZ_FALSE) {
	        return(FALSE);
	    }
	}
	ip->dstadr = address;
    } else {
	if (ip_broadcast_check(pak, TRUE) == FUZ_TRUE)
	    return (FALSE);
    }

    /*
     * Handle input IP options then attempt to route the packet.
     */
    if (!ipopt_inproc(pak))
	return(FALSE);
    if (!ip_route(pak, 0))
	return(FALSE);

    /*
     * If we routed to a directly connected destination, check if that
     * destination is IP broadcast so we can generate a physical broadcast.
     */
    if (pak->desthost == ip->dstadr) {
	if (ip_broadcast(pak->desthost, pak->if_output))  {

	    /*
	     * Discard if no directed broadcasts on this interface, or
	     * the directed broadcast access list rejects the packet.
	     */
	    if (!pak->if_output->ip_directbcast)
		return(FALSE);
	    if (IPBROADCASTLIST_CONFIGURED(pak->if_output) &&
		!ip_old_accesscheck(pak, pak->if_output->ip_directbcast))
		return(FALSE);
	    ip->dstadr = pak->if_output->ip_broadcast;
	    pak->desthost = (ipaddrtype) -1;
	}
    }

    /*
     * Never send a broadcast out the input interface. Note that this does
     * allow directed broadcast to another subnet accessible through the
     * original input interface.
     */
    if ((idb == pak->if_output) && (pak->desthost == -1) &&
        physical_broadcast(pak) == FUZ_TRUE) 
	return(FALSE);
    return(TRUE);
}

/*
 * ipttl_fixup
 * Increment the TTL field, recompute IP header checksum.
 * This routine is called after IP routing has molested the packet header
 * and just before the bridging code receives an ostensibly untouched packet.
 * Someday we might replace this routine with something that doesn't recompute
 * the entire header checksum.
 */

void ipttl_fixup (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    ip->ttl++;
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);

}

/*
 * default_ip_enqueue
 * Handle an IP datagram for an unrecognized or not registered
 * protocol
 */
void default_ip_enqueue (paktype *pak)
{
    send_unreachable(pak,ICMP_UNRPROTOCOL);
    if (ippkt_debug)
	ipbug(pak, "unknown protocol");
    IP_DISPOSE(ip, noprotocol, pak);	/* protocol not supported */
}


/*
 * nd_forward
 * Handle an ND packet.
 * If ND forwarding is enabled, pass it along, otherwise drop it.
 */

void nd_forward (paktype *pak)
{
    iphdrtype *ip;
    int count;
    paktype *copy;
    ipaddtype *ptr;
    idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);
    if (ip->ttl == 0)
	return;
    if (!(pak->flags & PAK_MCICHECKED)) {
	if (ip->ttl == 1)
	    return;
    }

    idb = pak->if_input;
    if (ip_forward_nd && idb->ip_helpernetQ.qhead &&
	ip_broadcast_check(pak, TRUE) != FUZ_FALSE) {
	count = 0;
	for (ptr = (ipaddtype *) idb->ip_helpernetQ.qhead; ptr;
	                     ptr = ptr->next) {
	    if ( (++count % 20) == 0 )
		process_suspend(); /* We've hogged the processor long enough */
	    if (ptr == (ipaddtype *) idb->ip_helpernetQ.qtail)
		copy = pak;  /* At the end of the list? */
	    else {
		copy = pak_duplicate(pak);
		if (!copy) {
		    retbuffer(pak); /* our job to get rid of it */
		    return;         	       /* Give up */
		}
	    }
	    if (!ip_forward_check(copy, ptr->address)) {
	        retbuffer(copy);
		continue;
	    }

	    ip = (iphdrtype *)ipheadstart(copy);
	    
	    if (udp_debug)
		buginf("\nIP: forwarded ND pkt from %i to %i on %s",
		       ip->srcadr, ip->dstadr,
		       copy->if_output->namestring);
	    ip->checksum = 0;
	    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);
	    (void) ipsendnet(copy, TRUE, TRUE);
	}
    } else {
	IP_DISPOSE(ip, noprotocol, pak);
    }
}

/*
 * ip_enqueue
 * Handle a datagram aimed at one of our interfaces.
 * Reassembly happens here.
 */

void ip_enqueue (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    /* if dst is aliased... */
    if ((pak->flags & PAK_ALIAS)) {
	if (reg_invoke_ip_alias(pak)) {
	    return;
	};
    }

    /*
     * If reassembly needed, do it here.
     */
    if ((ip->morefragments) ||	(getfo(ip) != 0)) {
	pak = reassemble(pak);
	if (pak == NULL) {
	    return;			/* return now if it was a fragment */
	} else {
	    /*
	     * It is a different packet now, reset the ip pointer
	     */
	    ip = (iphdrtype *)ipheadstart(pak);
	}
    }
    /*
     * Check if the packet is set for decryption. If it is, pass the
     * packet to crypto engine. After decryption is done, the crypto engine
     * will call reg_invoke_ip_enqueue to pass the packet up the protocol
     * stack.
     */
    if ((pak->cryptoflags & CRYPTO_DECRYPT) && !is_vip(pak->if_input->hwptr)) {
        crypto_vector_t *cry_vec;
        cry_vec = (crypto_vector_t *)(pak->if_input->hwptr->crypto_vectors);
        if (cry_vec) {
            (*(cry_vec->ps_vec))(CRYPTO_DECRYPT, pak, TRUE, TRUE);
        } else {
            if (ippkt_debug) {
                ipbug(pak, "crypto_error: packet dropped.");

            }
            retbuffer(pak);
        }
        return;
    }

    /*
     * Enqueue the datagram on the correct protocol queue.
     */
    ip_traffic.localdest++;		/* count another datagram */
    reg_invoke_ip_enqueue(ip->prot, pak);
}

/*
 * reassemble
 * Does reassembly of packets.  If a packet has been completely
 * reassembled then a ptr to it is returned; otherwise NULL is returned.
 * The reassembly algorithm used is described in the "Internet Protocol
 * Implementation Guide".
 */

paktype *reassemble (paktype *pak)
{
    iphdrtype *ip;
    paktype *frag;
    iphdrtype *frag_ip;
    int data_size;

    ip = (iphdrtype *)ipheadstart(pak);

    if (ippkt_debug)
	buginf("\nIP: recv fragment from %i offset %d bytes",
	       ip->srcadr,
	       getfo(ip) << 3);

    /*
     * Make sure received fragments are a multiple of 8 bytes
     */

    if (ip->morefragments) {
	data_size = (int) (ip->tl - (ip->ihl << 2));
	if (data_size < 8 || data_size & 0x7) {
	    if (ippkt_debug)
		buginf("\nIP: received fragment from %i, improper format.",
		       ip->srcadr);
	    IP_DISPOSE(ip, rafailed, pak);
	    return NULL;
	}
    }

    /* 
     * Scan list for a matching fragmented datagram.
     */
    ip_traffic.rainputs++;
    frag = (paktype *)reassemblyQ.qhead;
    while (frag) {
	frag_ip = (iphdrtype *)ipheadstart(frag);

	if ((frag_ip->srcadr == ip->srcadr) &&
	    (frag_ip->dstadr == ip->dstadr) &&
	    (frag_ip->id == ip->id) && 
	    (frag_ip->prot == ip->prot))
	    break;			/* it's the same packet! */
	frag = frag->next;
    }
    /*
     * If not in list, start reassembly.
     */
    if (frag == NULL) {
	if (QUEUEFULL(&reassemblyQ)) {
	    frag = dequeue(&reassemblyQ);
	    if (frag)
		IP_DISPOSE(ip, ratimeout, frag);
	}
	frag = getbuffer(RABUFFERSIZE);/* get maximum size buffer */
	if (frag == NULL) {
	    retbuffer(pak);	/* no storage, release buffer */
	    return(NULL);
	}
	frag_ip = (iphdrtype *)ipheadstart(frag);
	frag->if_input = pak->if_input;	/* remember interface ptr */
	frag->cryptoflags = pak->cryptoflags; /* remember cryptoflags */
	frag->crypto_enc_conn_id = pak->crypto_enc_conn_id;
	frag->crypto_dec_conn_id = pak->crypto_dec_conn_id;
	TIMER_START(frag->tstamp, RATIMEOUT);
	frag->linktype = pak->linktype;
	bcopy(ip, frag_ip, ip->ihl << 2);
	frag_ip->tl = 0;
	frag_ip->morefragments = 0;
	frag_ip->fo = 0;
	enqueue(&reassemblyQ, frag);
	frag->holehdr = 0;
	makehole(frag, (holetypeptr)ipdatastart(frag), 0, RABUFFERSIZE);
    }
    /* 
     * Add the current fragment to the packet being reassembled.
     */
    frag = addnew(pak, frag);
    return(frag);
}

/*
 * makehole
 * Make a hole descriptor at the indicated location in pkt.
 * All quantities are in terms of bytes and may not exceed the size of
 * a ushort (IPPACKETLIMIT).  They are passed in as uint's for performance
 * reasons ONLY.
 */

void makehole (paktype *pak, holetypeptr ptr, uint first, uint last)
{
    ptr->first = (ushort) first;
    ptr->last = (ushort) last;
    ptr->next = (holetypeptr) pak->holehdr;
    pak->holehdr = (unsigned long) ptr;
}

/*
 * sdns_forward
 * Handle an SDNS packet.
 * If SDNS forwarding is enabled, pass it along, otherwise drop it.
 */

void sdns_forward (paktype *pak)
{
    iphdrtype *ip;
    paktype *copy;
    ipaddtype *ptr;
    idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);
    if (ip->ttl == 0)
	return;
    if (!(pak->flags & PAK_MCICHECKED)) {
	if (ip->ttl == 1)
	    return;
    }

    idb = pak->if_input;
    if (ip_forward_sdns && idb->ip_helpernetQ.qhead &&
	ip_broadcast_check(pak, TRUE) != FUZ_FALSE) {
	for (ptr = (ipaddtype *) idb->ip_helpernetQ.qhead; ptr;
	                     ptr = ptr->next) {

	    /*
	     * Make a copy of the packet and send along for each
	     * helper address on the input interface.
	     */
	    copy = pak_duplicate(pak);
	    if (!copy)
		continue;

	    ip = (iphdrtype *)ipheadstart(copy);

	    if (!ip_forward_check(copy, ptr->address)) {
	        retbuffer(copy);
		continue;
	    }
	    if (udp_debug)
		buginf("\nIP: forwarded SDNS(%d) pkt from %i to %i on %s",
		       ip->prot, ip->srcadr, ip->dstadr,
		       copy->if_output->namestring);
	    ip->checksum = 0;
	    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);
	    (void) ipsendnet(copy, TRUE, TRUE);
	}
	retbuffer(pak);
    } else		
	IP_DISPOSE(ip, noprotocol, pak);
}

/*
 * addnew
 * Updates the hole descriptor list for pkt. and copies the fragment
 * data into the reassembly buffer.
 * Returns pointer to packet if it has been completely reassembled.
 */

paktype *addnew (paktype *fragpkt, paktype *pkt)
{
    iphdrtype *fragpkt_ip;
    iphdrtype *pkt_ip;
    holetypeptr ptr, ptrprev, ptrnext;
    ulong linkhdr_len;
    uchar *fdgramstart;                 /* fragpkt datagramstart */
    uchar *dgramstart;                  /* pkt datagramstart */
    ulong fragfirst, fraglast;		/* these are ulongs for performance
					   reasons only, they may not exceed
					   64k */

    fragpkt_ip = (iphdrtype *)ipheadstart(fragpkt);
    pkt_ip     = (iphdrtype *)ipheadstart(pkt);

    ptr = (holetypeptr) pkt->holehdr;
    ptrprev = NULL;
    fragfirst = getfo(fragpkt_ip) << 3;
    fraglast = fragfirst + fragpkt_ip->tl - (fragpkt_ip->ihl << 2) - 1;

    /* 
     * Check if the new fragment will make the reassembled packet too large.
     */
    if (fraglast > (RABUFFERSIZE - (pkt_ip->ihl << 2))) {

	/* 
	 * Deallocate reassembly resources.
	 */
	unqueue(&reassemblyQ, pkt);
	retbuffer(pkt);
	retbuffer(fragpkt);
	ip_traffic.rafailed++;
	return(NULL);
    }
    /* 
     * Traverse list of hole descriptors to see where fragment lies.
     */
    while (ptr != NULL) {
	ptrnext = ptr->next;
	if ((ptr->first > fraglast) || (ptr->last < fragfirst)) {
	    ptrprev = ptr;
	    ptr = ptrnext;		/* no overlap with this hole. */
	    continue;
	}

	/* 
	 * Fragment overlaps the hole.  Delete the hole descriptor.
	 */
	if (ptrprev == NULL)
	    pkt->holehdr = (unsigned long) ptrnext;
	else
	    ptrprev->next = ptrnext;
	/* 
	 * Add hole descriptors for any holes left in front or back of the
	 * fragment.
	 */
	if (fragfirst > ptr->first)
	    makehole(pkt, ptr, ptr->first, fragfirst - 1);
	if ((fraglast < ptr->last) && (fragpkt_ip->morefragments == 1))
	    makehole(pkt, (holetypeptr)((uchar *)ipdatastart(pkt)+fraglast+1),
		     fraglast+1,ptr->last);
	ptr = ptrnext;
    }

    /*
     * If this is the first fragment, copy the link layer
     * header and set datagramstart to the link layer header in the
     * reassembly buffer and copy over flags.
     */
    if (fragfirst == 0) {
	fdgramstart = fragpkt->datagramstart;
	linkhdr_len = ipheadstart(fragpkt) - fdgramstart;
	dgramstart = ipheadstart(pkt) - linkhdr_len;
	bcopy(fdgramstart, dgramstart, linkhdr_len);
	pkt->datagramstart = dgramstart;
	/* note: this is an unofficial fix waiting for CSCdi30204 */
	pkt->flags = fragpkt->flags & ~PAK_INPUTQ;
    }

    /* 
     * Move the fragment data into the reassembly buffer and discard the
     * fragment buffer.
     */
    bcopy(ipdatastart(fragpkt),(ipdatastart(pkt) + (getfo(fragpkt_ip) << 3)),
	    (fragpkt_ip->tl - (fragpkt_ip->ihl << 2)));
    retbuffer(fragpkt);
    /* 
     * If reassembly is complete, then return the packet, else NULL.
     */
    pkt_ip->tl += (fraglast - fragfirst + 1);
    if (pkt->holehdr == 0) {
	pkt_ip->tl += h2n(pkt_ip->ihl << 2);
	unqueue(&reassemblyQ, pkt);
	ip_traffic.reassembled++;
	pkt->datagramsize = pkt_ip->tl;
	return(pkt);
    }
    return(NULL);
}

/*
 * ip_ratimeout;
 * Handles packet reassembly timeouts.
 * Deallocates the resources being used to reassemble pkt.
 */

void ip_ratimeout (void)
{
    paktype *pkt, *next;

    if (QUEUEEMPTY(&reassemblyQ))
	return;
    pkt = (paktype *) reassemblyQ.qhead;
    while (pkt) {
	next = pkt->next;
	if (!SLEEPING(pkt->tstamp)) {
	    unqueue(&reassemblyQ, pkt);
	    send_timeexceeded(pkt,ICMP_TIMFRAG);
	    IP_DISPOSE(ip, ratimeout, pkt);
	}
	pkt = next;
    }
}

/*
 * ipbug_detail--print detailed IP packet debugging info
 */
static void ipbug_detail(iphdrtype *ip)
{
    udptype *udp;
    tcptype *tcp;
    icmptype *icmp;
    
    switch (ip->prot) {
    case ICMP_PROT:
	icmp = (icmptype *) ipdatastart_iph(ip);
	buginf("\n    ICMP type=%d, code=%d", icmp->type, icmp->code);
	break;
    case TCP_PROT:
	tcp = (tcptype *) ipdatastart_iph(ip);
	buginf("\n    TCP src=%d, dst=%d, seq=%u, ack=%u, win=%d",
	       tcp->sourceport, tcp->destinationport,
	       tcp->sequencenumber, tcp->acknowledgementnumber,
	       tcp->window);
	if (tcp->urg)
	    buginf(" URG");
	if (tcp->ack)
	    buginf(" ACK");
	if (tcp->psh)
	    buginf(" PSH");
	if (tcp->rst)
	    buginf(" RST");
	if (tcp->syn)
	    buginf(" SYN");
	if (tcp->fin)
	    buginf(" FIN");
	break;
    case UDP_PROT:
	udp = (udptype *) ipdatastart_iph(ip);
	buginf("\n    UDP src=%d, dst=%d", udp->sport, udp->dport);
	break;
    default:
	buginf(", proto=%d", ip->prot);
	break;
    }
}

/*
 * ippkt_check
 * See if this packet would be discarded by an IP access list or
 * if the source address is that of one of our logging ttys.
 * Returns FALSE to suppress logging activity.
 */

boolean ippkt_check (paktype *pak, boolean logger, uint acl)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (ipdebug_acl != 0 && !ip_accesscheck(pak, acl_array[acl]))
	return(FALSE);
    if (logger && logger_vtyaddress(ip->srcadr))
	return(FALSE);
    return(TRUE);
}

/*
 * ipbug_inline
 * Print a pretty buginf about an IP packet or IP packet being forwarded
 * outputif - if set to true will print the output interface if the packet
 *            has been routed.
 * gw       - if set to true will print the next hop GW.
 */

static inline void ipbug_inline (paktype *pak, char *string,
				 boolean outputif, boolean gw,
				 uint acl, boolean detail, boolean dump) 
{
    iphdrtype *ip;
    uchar *start, *end;

    ip = (iphdrtype *)ipheadstart(pak);

    if (!ippkt_check(pak, TRUE, acl))
	return;

    buginf("\nIP: s=%i (%s), d=%i", ip->srcadr,
	   pak->if_input ? pak->if_input->namestring : "local", ip->dstadr);
    if (outputif && pak->if_output)
	buginf(" (%s)", pak->if_output->namestring);
    if (gw)
	buginf(", g=%i", pak->desthost);
    buginf(", len %d, %s", ip->tl, string);

    if (detail) {
	ipbug_detail(ip);
    }

    if (dump) {
	start = pak->datagramstart;
	end   = start + pak->datagramsize;
	memory_dump(start, end, FALSE,
		    MEMORY_DUMP_DATA_TYPE_ASCII,
		    (memory_dump_output_t)buginf);
    }
}

/*
 * ipbug
 * Call ipbug_inline showing the output interface
 */

void ipbug (paktype *pak, char *string)
{
    ipbug_inline(pak, string, TRUE, FALSE, ipdebug_acl,
		 ipdebug_detail, ipdebug_dump);
}

/*
 * ipbug_error
 * Call ipbug_inline using the error ACL
 */

void ipbug_error (paktype *pak, char *string)
{
    ipbug_inline(pak, string, TRUE, FALSE, iperror_acl,
		 iperror_detail, iperror_dump);
}

/*
 * ipbug_no_outputif
 * Call ipbug_inline without showing the output interface
 */

void ipbug_no_outputif (paktype *pak, char *string)
{
    ipbug_inline(pak, string, FALSE, FALSE, ipdebug_acl,
		 ipdebug_detail, ipdebug_dump);
}

/*
 * ipbuggw
 * Call ipbug_inline showing the output interface and nexthop gateway
 */

void ipbuggw (paktype *pak, char *string)
{
    ipbug_inline(pak, string, TRUE, TRUE, ipdebug_acl, ipdebug_detail,
		 ipdebug_dump);
}

/*
 * ip_show_traffic
 * IP statistics
 */

void ip_show_traffic (void)
{
    printf("\nIP statistics:");
    printf("\n  Rcvd:  %lu total, %lu local destination",
		ip_traffic.inputs, ip_traffic.localdest);
    printf("\n         %lu format errors, %lu checksum errors, "
	   "%lu bad hop count",
	   ip_traffic.formaterr, ip_traffic.checksumerr,
	   ip_traffic.hopcount);
    printf("\n         %lu unknown protocol, %lu not a gateway",
		ip_traffic.noprotocol,ip_traffic.notgateway);
    printf("\n         %lu security failures, %lu bad options, "
	   "%lu with options", ip_traffic.badsecure,
	   ip_traffic.badoption, ip_traffic.options);

    printf("\n  Opts:  %lu end, %lu nop, %lu basic security, "
	   "%lu loose source route", ip_traffic.opt_end,
	   ip_traffic.opt_nop, ip_traffic.opt_bassec, ip_traffic.opt_lsr);
    printf("\n         %lu timestamp, %lu extended security, "
	   "%lu record route", ip_traffic.opt_tstmp,
	   ip_traffic.opt_extsec, ip_traffic.opt_rrt);
    printf("\n         %lu stream ID, %lu strict source route, %lu alert,"
	   " %lu cipso",
	   ip_traffic.opt_sid, ip_traffic.opt_ssr, ip_traffic.opt_alert,
	   ip_traffic.opt_cipso);
    printf("\n         %lu other", ip_traffic.opt_unk);
    printf("\n  Frags: %lu reassembled, %lu timeouts, %lu couldn't reassemble",
	   ip_traffic.reassembled, ip_traffic.ratimeout,
	   ip_traffic.rafailed);
    printf("\n         %lu fragmented, %lu couldn't fragment",
    		ip_traffic.fragged,ip_traffic.fragfailed);
    printf("\n  Bcast: %lu received, %lu sent",
		ip_traffic.bcastin,ip_traffic.bcastout);
    printf("\n  Mcast: %lu received, %lu sent",
		ip_traffic.mcastin,ip_traffic.mcastout);
    printf("\n  Sent:  %lu generated, %lu forwarded",
		ip_traffic.outputs, ip_traffic.forwarded);
    printf("\n         %lu encapsulation failed, %lu no route",
		 ip_traffic.encapsfailed, ip_traffic.noroute);
}
