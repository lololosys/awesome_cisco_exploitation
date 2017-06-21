/* $Id: mbone_mtrace.c,v 3.23.4.16 1996/09/11 05:07:07 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/mbone_mtrace.c,v $
 *------------------------------------------------------------------
 * mbone_mtrace.c - mbone multicast traceroute functions
 *
 * August, 1995 chwhite
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mbone_mtrace.c,v $
 * Revision 3.23.4.16  1996/09/11  05:07:07  snyder
 * CSCdi68758:  spellink errors
 *              dulplicate -> duplicate
 * Branch: California_branch
 *
 * Revision 3.23.4.15  1996/09/10  18:54:08  chwhite
 * CSCdi68628:  mtrace always responds when multicasting is not enabled
 * Branch: California_branch
 *
 * Revision 3.23.4.14  1996/09/10  18:27:57  chwhite
 * CSCdi68628:  mtrace always responds when multicasting is not enabled
 * Branch: California_branch
 *
 * Revision 3.23.4.13  1996/08/07  23:50:32  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.23.4.12  1996/08/05  21:13:56  chwhite
 * CSCdi64249:  Integrate forwarding of mtrace reponses with mforward
 * Branch: California_branch
 *
 * Revision 3.23.4.11  1996/08/03  01:20:28  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.23.4.10  1996/07/26  22:52:53  chwhite
 * CSCdi63685:  Correct mtrace error handling, TTL behavior
 * Branch: California_branch
 *
 * Revision 3.23.4.9  1996/07/10  01:01:22  mleelani
 * CSCdi61826:  input queues will up with multicast packets
 * Branch: California_branch
 * Looping mtrace packets cause IGMP process to block indefinitely.
 * Remove mrouted 3.4 mtrace interoperability.
 *
 * Revision 3.23.4.8  1996/07/03  23:51:24  chwhite
 * CSCdi61685:  mtrace will fail if incoming interface is pim-nbma mode
 * Branch: California_branch
 *
 * Revision 3.23.4.7  1996/06/19  22:31:45  chwhite
 * CSCdi58314:  mrtrace does not work on a router configured as a host
 * Branch: California_branch
 *
 * Revision 3.23.4.6  1996/06/07  16:16:04  chwhite
 * CSCdi58600:  mstat only reports incoming interface
 * Branch: California_branch
 *
 * Revision 3.23.4.5  1996/06/07  15:09:00  chwhite
 * CSCdi51969:  Jhawk wants mrinfo source-interface option
 * Branch: California_branch
 *
 * Revision 3.23.4.4  1996/05/04  01:08:36  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.23.4.3  1996/04/29  17:25:11  dino
 * CSCdi55963:  mtrace wont start from a router with negative cache state
 * Branch: California_branch
 *
 * Revision 3.23.4.2  1996/04/15  21:17:28  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.23.4.1  1996/03/18  20:18:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.16.2.3  1996/03/16  06:55:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.16.2.2  1996/03/07  09:48:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.16.2.1  1996/02/20  01:01:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.23  1996/03/09  09:08:57  dino
 * CSCdi51000:  ciscos dont forward mtrace mcast replies after fs cache
 *              populated
 *
 * Revision 3.22  1996/03/01  02:18:18  dino
 * CSCdi50401:  RP does not process unknown mtrace sources on the (*,G)
 *              tree.
 *
 * Revision 3.21  1996/02/15  10:44:55  dino
 * CSCdi49075:  Unix mtrace -g doesnt work through ciscos.
 *
 * Revision 3.20  1996/02/13  09:36:23  dino
 * CSCdi48623:  Multicast mtraces are not processed by last hop router due
 *              to f/s
 *
 * Revision 3.19  1996/02/10  07:15:59  dino
 * CSCdi48502:  mtrace returns error when initiated by RP or for 3-party
 *              mtraces.
 *
 * Revision 3.18  1996/02/08  20:11:57  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.17  1996/02/07  09:31:35  dino
 * CSCdi48313:  mtrace still doesnt return correc errors
 *
 * Revision 3.16  1996/01/22  16:09:28  chwhite
 * CSCdi47171:  mtrace does not find root of source tree
 *
 * Revision 3.15  1996/01/19  07:25:08  dino
 * CSCdi47171:  mtrace does not find root of source tree
 *
 * Revision 3.14  1996/01/19  01:22:11  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/17  07:14:02  dino
 * CSCdi46870:  mtrace needs to return more accurate error codes to user.
 *
 * Revision 3.12  1996/01/17  07:06:26  dino
 * CSCdi46870:  mtrace needs to return more accurate error codes to user.
 *
 * Revision 3.11  1996/01/11  02:28:11  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.10  1996/01/05  04:56:01  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.9  1995/12/12  02:42:08  chwhite
 * CSCdi43761:  cisco mtrace processor is too gratitious reporting error
 * responses.
 *
 * Revision 3.8  1995/12/10  18:25:11  asastry
 * CSCdi44948:  MTRACE hangs, and box crashes when aborting w/ Cntrl-Shft6.
 *
 * Revision 3.7  1995/12/01  18:38:37  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.6  1995/11/30  04:42:08  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.5  1995/11/22  06:57:21  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.4  1995/11/21  23:12:04  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/21  20:53:43  dino
 * CSCdi43017:  Fix mtrace UI bugs
 *
 * Revision 3.2  1995/11/21  19:10:28  snyder
 * CSCdi44272:  more spellink errors
 *              Stratistics -> Statistics
 *
 * Revision 3.1  1995/11/09  12:01:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/06  09:00:40  dino
 * CSCdi41679:  mtrace should return non-responding router information
 *
 * Revision 2.10  1995/10/05  04:50:47  dino
 * CSCdi41574:  mtrace doesnt return ttl threshold for outgoing interface
 *
 * Revision 2.9  1995/10/04  17:40:54  dino
 * CSCdi41469:  More Mtrace bugs found during EFT testing.
 *
 * Revision 2.8  1995/09/28  02:18:51  dino
 * CSCdi41187:  Mtrace is broken in a couple of scenarios and causes
 *              spurious errors
 *
 * Revision 2.7  1995/09/21  18:35:44  dino
 * CSCdi40833:  mrinfo with no address doesnt respond.
 *
 * Revision 2.6  1995/09/08  16:07:53  hampton
 * Clean up nits related to the timer changes.  [CSCdi39967]
 *
 * Revision 2.5  1995/08/22 21:54:25  dino
 * CSCdi39110:  DVMRP version number should be added to all packets, fix
 * style
 *
 * Revision 2.4  1995/08/17  22:44:07  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.3  1995/08/15  21:22:13  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.2  1995/08/03  17:08:15  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              expand abs() call.
 *
 * Revision 2.1  1995/08/03  01:13:00  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "name.h"
#include "ttysrv.h"
#include "access.h"
#include "../os/clock.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/tunnel.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/mbone_mtrace.h"
#include "../ui/common_strings.h"
#include "../ip/ip_registry.h"

#define MAXLEN 80
#define DEFAULT_RETRIES 3
#define QUERY_CAST       0xe0000002
#define MBONE_AUDIO      0xe0020001

/*
 * Storage
 */
static queuetype ip_mbone_mtraceQ;
queuetype ip_mrinfoQ;
static ulong ip_mbone_mtrace_id = 1;
static ulong old_query = 0;
static ulong old_multicast_query = 0;

static int nqueries = DEFAULT_RETRIES;

static ipparamtype mbonetracedefaultparams = 
    { 0, TOS_PREC_ROUTINE, TTL_DEFAULT, FRAGMENTS_ALLOWED, TRUE };


/*
 * ip_mbone_mtraceBLOCK
 * 
 * Determine if EXEC process running mbone_mtrace should wakeup.
 */
static boolean ip_mbone_mtraceBLOCK (ulong arg)
{
   mbone_mtraceQtype *mtrace;

   mtrace = (mbone_mtraceQtype *)arg;

   /*
     * Return to EXEC process if data was received.
     */
    if (mtrace->data.count) return(FALSE);

    /*
     * Keyboard interrupt.
     */
    if (psipending2(mtrace->tty)) {
	mtrace->interrupted = TRUE;
	return(FALSE);
    }

    /*
     * We've waited long enough for Trace Responses.
     */
    if (AWAKE(mtrace->timeout)) {
	mtrace->timeout_flag = TRUE;
	return(FALSE);
    }

    /*
     * Otherwise, keep process asleep.
     */
    return(TRUE);
}

/* print_hop
 * 
 * Given an ip address print either the DNS hostname if available 
 * or the ip address
 */
static void print_hop (ipaddrtype ip)
{
    nametype *name;

    if ((name=reg_invoke_ip_address_lookup(ip))) {
	printf("%s (%i)", name->name, ip);
    } else {
	printf("%i", ip);
    }
}

/* 
 * time_diff
 *
 * Evaluates the time differential between the two passed timestamps.
 * The first argument is the query timestamp, the second the packet stamp.
 */
static int time_diff (ulong q_sent, ulong p_recv)
{

    int diff = p_recv - q_sent;
  
    return ((diff * 125) >> 13);
}

/* 
 * proto
 *
 * Returns text based on the protocol code passed.
 */
static char *proto (uchar type)
{
    static char buf[80];
  
    switch(type) {
    case PROTO_DVMRP:
	return ("DVMRP");
    case PROTO_MOSPF:
	return ("MOSPF");
    case PROTO_PIM:
	return ("PIM");
    case PROTO_CBT:
	return ("CBT");
    case PROTO_PIM_SPECIAL:
	return("PIM/DVMRP Unicast");
    case PROTO_PIM_STATIC:
	return("PIM/Static");
    case PROTO_DVMRP_STATIC:
	return("DVMRP/Static");
    case 0:
	return ("None");
    default:
	(void) sprintf(buf, "Proto %d", type);
	return(buf);
    }
}

/*
 * proto_code
 *
 * Give an RPF type (UNICAST, STATIC, DVMRP), return the mtrace code
 * which corresponds.
 */
static uchar proto_code (uchar rpf_type)
{
    switch (rpf_type) {
    case RPF_UNICAST:
	return(PROTO_PIM);
    case RPF_STATIC:
	return(PROTO_PIM_STATIC);
    case RPF_DVMRP:
        return(PROTO_PIM_SPECIAL);
    default:
	return(0);
    }
}

/* 
 * flag
 *
 * Returns text based on error flags code passed.
 */
static char *flag (uchar type)
{
    static char buf[80];
  
    switch(type) {
    case TR_NO_ERR:
	return ("");
    case TR_WRONG_IF:
	return ("Wrong interface");
    case TR_PRUNED:
	return ("Prune sent upstream");
    case TR_OPRUNED:
	return ("Output pruned");
    case TR_SCOPED:
	return ("Hit scope boundary");
    case TR_NO_RTE:
	return ("No route");
    case TR_OLD_ROUTER:
	return ("Next router no mtrace");
    case TR_NO_FWD:
	return ("Not forwarding");
    case TR_HIT_RP:
	return ("Reached RP/Core");
    case TR_RPF_IF:
	return ("RPF Interface");
    case TR_NO_MULTI:
	return ("Multicast disabled");
    case TR_ADMIN_PROHIB:
	return ("Admin. Prohibited");
    case TR_NO_SPACE:
	return ("No space in packet");
    default:
	(void) sprintf(buf, "Unknown error code %d", type);
	return(buf);
    }
}

/* 
 * output_trace
 *
 * Print the contents of an mbone traceroute packet starting at index. This
 * function assumes that index is not > 0.
 */
static boolean output_trace (mbone_mtraceQtype *mtrace,
			     mbone_mtrace_resp *resp, int repnum, int index, 
			     ipaddrtype tr_src, boolean printnum)
{

    int i;
    boolean error;
    ipaddrtype smask;

    resp = resp + index - 1;
    error = FALSE;

    for (i = index; i <= repnum; i++, resp++) {
	if (printnum) printf("\n-%3d", i);
	print_hop(resp->tr_outaddr);
	if (resp->tr_rflags) error = TRUE;
	printf (" %s  thresh^ %d  %d ms  %s", proto(resp->tr_rproto),
		resp->tr_fttl, time_diff(mtrace->time_sent, resp->tr_qarr),
		flag(resp->tr_rflags));
	if ((i > 1) && resp->tr_outaddr != (resp-1)->tr_rmtaddr) {
	    printf(" !RPF!");
	    print_hop((resp-1)->tr_rmtaddr);
	}

	if (resp->tr_rflags != TR_NO_RTE) {
	    if (resp->tr_smask <= 1) {
		printf(" [default]");
	    } else {
		smask = ip_bitcount2mask(resp->tr_smask);
		printf(" [%i/%d]", tr_src & smask, resp->tr_smask);
	    }
	}
    }
    return(error);
}

/* 
 * mbone_mtrace_command
 *
 * Process: mtrace <source> <destination> <group> <multicast ttl>
 *
 * This command mimics the mtrace command included with the mrouted
 * distribution. A trace query is passed hop by hop along the reverse
 * path to the source, starting at the last hop before the destination,
 * collecting routing information. The path should follow the path for
 * "group". The response is then returned to the recevier and output.
 */
void mbone_mtrace_command (parseinfo *csb)
{
    uchar             m_ttl;
    ipaddrtype        mcast_source, mcast_dest, group, smask, responder;
    mbone_mtraceQtype *mtrace;
    mbone_mtrace_resp *resp=NULL;
    paktype           *pak=NULL;
    iphdrtype         *ip;
    igmptype          *igmp;
    int               hops, length, repnum; 
    boolean           error, weak;
    ipaddrtype        last_responder;

    m_ttl = 64;       
    responder = 0;
                    
    weak = group ? FALSE : TRUE;

    /* 
     * Parse command line and allocate queue
     */
    mtrace = ip_mbone_mtrace_parse_csb(csb, &group, &mcast_source,
				       &mcast_dest, &m_ttl, &responder);
    if (!mtrace) return;

    /* 
     * Send out request at full reverse path report length.
     */
    printf("Querying full reverse path... ");
    ip_mbone_mtrace_send_request(group, mcast_source, mcast_dest, responder,
				 m_ttl, 32, 1, mtrace);

    if (mtrace->data.count) {

	/* 
	 * We have a good full response. 
	 */
	printf("\n 0  ");
	print_hop(mcast_dest);
	pak = dequeue(&mtrace->data);
	if (pak) {
	    ip = (iphdrtype *) ipheadstart(pak);
	    igmp = (igmptype *) ipdatastart(pak);
	    resp = (mbone_mtrace_resp *) (igmp->data + QLEN);
	    length = ip->tl - (ip->ihl << 2) - IGMPHEADERBYTES;
	    repnum = (length-QLEN)/RLEN;
      
	    /* 
	     * Output the intermediate hops.
	     */
	    error = output_trace(mtrace, resp, repnum, 1,
				 mcast_source, TRUE);
	    resp += repnum - 1;

	    /* 
	     * If we encountered an older router, print its version number .
	     */
	    if (resp->tr_rflags == TR_OLD_ROUTER) {
		printf("\n-%3d", repnum + 1);
		what_kind(resp->tr_rmtaddr, TRUE);
	    } else if (!error) {

		/* 
		 * Print the source address if no error was in the response 
		 * data.
		 */
		smask = ip_bitcount2mask(resp->tr_smask);
	 
		if ((resp->tr_inaddr & smask) == (mcast_source & smask) 
		    || weak) {
		    printf("\n-%3d", repnum + 1);
		    print_hop(mcast_source);
		}
	    }
	}
    } else {	 

	/* 
	 * Switch to a hop-by-hop trace to see how far we can get.
	 */ 
	printf("switching to hop-by-hop:\n 0  ");
	print_hop(mcast_dest);
	last_responder = 0;
    
	for (hops = 1; hops <= MAXHOPS; hops++) {
	    printf("\n-%3d", hops);
	    ip_mbone_mtrace_send_request(group, mcast_source, mcast_dest,
					 responder, m_ttl, hops,
					 nqueries, mtrace);
	    if (mtrace->data.count == 0) {
		if (last_responder) what_kind(last_responder, TRUE);
		break;
	    }
	  
	    pak = dequeue(&mtrace->data);
	    if (!pak) continue;
	    ip = (iphdrtype *) ipheadstart(pak);
	    igmp = (igmptype *) ipdatastart(pak);
	    resp = (mbone_mtrace_resp *) (igmp->data + QLEN);;
	    length = ip->tl - (ip->ihl << 2) - IGMPHEADERBYTES;
	    repnum = (length-QLEN) / RLEN;
	  
	    if (repnum == hops) {
		error = output_trace(mtrace, resp, repnum, hops,
				     mcast_source, FALSE);
	    } else {
		if (resp->tr_rflags == TR_OLD_ROUTER) {
		    what_kind(resp->tr_rmtaddr, TRUE);
		    break;
		}
		if (resp->tr_rflags == TR_NO_SPACE) {
		    printf("No space left in trace packet for further hops\n");
		    break;
		}
		printf("Route must have changed...try again\n");
		error = output_trace(mtrace, resp, repnum, hops,
				     mcast_source, TRUE);
		break;
	    }
	    if (!error) {
		smask = ip_bitcount2mask(resp->tr_smask);

		if ((resp->tr_inaddr & smask) == (mcast_source & smask)) {
		    printf("\n-%3d", hops);
		    print_hop(mcast_source);
		    break;
		}
	    }
	    if (!resp->tr_rmtaddr || (resp->tr_rflags & 0x80)) break;

	    resp += (repnum - 1);
	    last_responder = resp->tr_rmtaddr;
	    datagram_done(pak);
	    pak = NULL;
	}
    }
  
    /* 
     * No response, bail.
     */
    if (!resp) {
	printf("Timed out receiving responses\n");
	printf("Perhaps no local router has a route for source, the receiver is not\n");
	printf("a member of the multicast group or the multicast ttl is too low.\n");
    }   

    if (pak) datagram_done(pak);
    unqueue(&ip_mbone_mtraceQ, mtrace);
    free(mtrace);
  
    flush();
    psioff(stdio);
}

/*
 * mrinfo_command <router> [<source address|interface>]
 * 
 * Determine version and neighbor information about a multicast
 * router via a DVMRP asknbr2 request
 */
void mrinfo_command (parseinfo *csb)
{

    mrinfoQtype *asknbr;
    ipaddrtype  router, source_addr;
    paktype     *pak;
    idbtype     *idb, *source_int;
    nametype    *name;

    source_addr = 0;
    source_int  = NULL;

    /*
     * Get the optional source interface off the command line.
     */
    source_int = GETOBJ(idb, 1);
    
    /*
     * Get the address to source the request from off the command line.
     */
    if (!source_int) {
	source_addr = GETOBJ(paddr,2)->ip_addr;
	if (!ip_ouraddress(source_addr) && source_addr) {
	    printf("\nInvalid source ip address");
	    return;	
	}	
    } else {
	
	/*
	 * Otherwise grab the source ip_address from the specified interface
	 */
	if (source_int->ip_enabled && interface_up(source_int)) {
	    source_addr = source_int->ip_address;
	    if (!source_addr) source_addr = source_int->ip_unnumbered->
		ip_address;
	};

	if(!source_addr) {
	    printf("\nInvalid source interface");
	    return;
	}
    }
	
    /*
     * Get the address of the multicast router from the command line.
     */
    router = GETOBJ(paddr, 1)->ip_addr;
    if (!router) {

	/*
	 * Sending an mrinfo request to ourselves. Find an address so the
	 * request can be looped.
	 */ 
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_enabled && interface_up(idb)) {
		router = idb->ip_address;
		if (!router) router = idb->ip_unnumbered->ip_address;
		break;
	    }
	}
	if (!router) {
	    printf("\nNo local IP addresses to send mrinfo");
	    return;
	}
    }
	
    /* 
     * Enqueue this request.
     */
    asknbr = malloc(sizeof(mrinfoQtype));
  
    if (!asknbr) return;

    enqueue(&ip_mrinfoQ, asknbr);
    psion(stdio);
    asknbr->id = router;
    TIMER_START(asknbr->timeout, MRINFO_TIMEOUT);
    asknbr->timeout_flag = FALSE;
    asknbr->tty = stdio;

    /* 
     * Send asknbr2 request to specified router.
     */
    dvmrp_send_asknbr2_request(router, source_addr); 
  
    edisms((blockproc *) ip_mrinfoBLOCK, (ulong) asknbr);
  
    /* 
     * Check if there was a response.
     */
    if (asknbr->timeout_flag) printf("\n%% Timed out receiving response");
  
    if (asknbr->data.count) {
	if ((name = reg_invoke_ip_address_lookup(router))) {
	    printf("%i (%s)", router, name->name);
	} else {
	    printf("%i", router);
	}
    }
  
    /* 
     * Continuing printing responses until there are no more.
     */
    while (asknbr->data.count) {
	pak = dequeue(&asknbr->data);
	if (!pak) continue;
	mrinfo_process_asknbr2_response(pak, router);
	datagram_done(pak);
    }
  
    unqueue(&ip_mrinfoQ, asknbr);
    free(asknbr);

    psioff(stdio);
    flush();
}

/* 
 * ip_mbone_mtrace_send_request
 *
 * Send IGMP (mrouted format) trace request towards the source in the
 * reverse path from the destination.
 * 
 * ipaddrtype group: group the route is being traced for 
 * ipaddrtype mcast_source: source address trace is aimed towards
 * ipaddrtype mcast_dest: destination address trace begins from
 * uchar ttl: ttl for multicasted requests
 * int hops: maximum number of hops request can pass through before 
 *           returning.
 * int tries: number of retries of request before giving up
 * mbone_mtraceQtype *mtrace: mtrace query/response storage queue 
 *
 */
void ip_mbone_mtrace_send_request (ipaddrtype group, ipaddrtype mcast_source,
				   ipaddrtype mcast_dest, 
				   ipaddrtype mcast_responder, uchar ttl,
				   int hops, int tries, 
				   mbone_mtraceQtype *mtrace)
{

    igmptype           *igmp;
    mbone_mtrace_query *query;
    paktype            *pak, *dup;
    ulong              length;
    int                i;
    clock_epoch        time_sent;
    ipaddrtype         lasthop;
    boolean            weak;

    length = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + QLEN;
    pak = getbuffer(length);
    if (!pak) return;

    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_MTRACE_TYPE;
    igmp->checksum = 0;
    igmp->address = group;
    igmp->code = hops;
    query = (mbone_mtrace_query *) igmp->data;
    query->tr_src = mcast_source;
    query->tr_dst = mcast_dest;
    weak = group ? FALSE : TRUE;
    
    if (mcast_responder) {
	query->tr_raddr = mcast_responder;
    } else {
	query->tr_raddr = ip_best_local_address(mcast_source, 0);
    }

    /*
     * If the destination is on a local net, use ourself as the lasthop,
     * Otherwise use the trace group unless this is a weak trace, in which 
     * case we default to the destination address given.
     */
    if (weak) { 
	lasthop = mcast_dest;
    } else {
	if (!samecable(mcast_dest)) {
	    lasthop = group; 
	} else {

	    /* 
	     * We are the lasthop, so forward the request to ourselves! 
	     */ 
	    lasthop = ip_best_local_address(mcast_dest, 0);
	    if (!lasthop) {
	       IGMP_BUGINF("IGMP: Mtrace connected destination not reachable");
		datagram_done(pak);
		return;
	    }
	}
    }	
    query->ttl_qid = ttl ? ttl << 24 : IPMULTICAST(query->tr_raddr) ?
	MBONE_MTRACE_DEFAULT_TTL << 24 : 255 << 24;
  
    for (i = tries; i > 0; --i) {
	if (tries == nqueries) {
	    if (i == ((nqueries + 1) >> 1)) {
	      
	      /* 
	       * Try a unicast to the destination in the hopes that it is a 
	       * multicast router. 
               */
	      lasthop = mcast_dest;
	      ttl = TTL_DEFAULT;
	    }
	    if (i <= ((nqueries + 3) >> 2) && ttl == 0) {
	        
	        /* 
		 * If that fails, send to with the return address as 
		 * response cast in case mcast gets through an administrative
	         * boundary that unicast will not. Continue with increasing
                 * multicast ttl.
                 */
	        lasthop = group;
	        query->tr_raddr = MTRACE_RESPONSE;
		query->ttl_qid += 32 << 24;
		if (query->ttl_qid >> 24 > MBONE_MTRACE_MAX_TTL) {
		    query->ttl_qid = MBONE_MTRACE_MAX_TTL << 24;
		}
	    }
        }
	mtrace->id = ip_mbone_mtrace_id;
	query->ttl_qid &= 0xff000000;
	query->ttl_qid += mtrace->id;
	ip_mbone_mtrace_id++;
    
	/* 
	 * Timestamp this request.
	 */
	clock_get_time(&time_sent);
	mtrace->time_sent = (time_sent.epoch_secs << 16) + 
	    ((time_sent.epoch_frac >> 16) & 0xffff);
	TIMER_START(mtrace->timeout, MBONE_MTRACE_TIMEOUT);
      
	igmp->checksum = ipcrc((ushort *) igmp, length - IPHEADERBYTES(NOPT));
      
	dup = pak_duplicate(pak);
	if (!dup) {
	    retbuffer(pak);
	    return;
	}
    
	IGMP_BUGINF("\nIGMP: Send Mtrace request to %i, nhops %d, ttl %d",
		    lasthop, igmp->code, ttl);

	mbonetracedefaultparams.ttl = ttl;
	mbonetracedefaultparams.ident = ipdefaultparams.ident;
	ipwrite(pak, length, IGMP_PROT, IPADDR_ZERO, lasthop, NULL, NULL,
		&mbonetracedefaultparams);
    	
	pak = dup;
	igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
	igmp->checksum = 0;
	query = (mbone_mtrace_query *) igmp->data;
    
	edisms((blockproc *) ip_mbone_mtraceBLOCK, (ulong) mtrace);
	if (mtrace->data.count == 0 && !mtrace->interrupted) {
	    printf("* ");
	    flush();
	    continue;
	} else {
	    break;
	}
    }
    retbuffer(pak);
}

/* 
 * ip_mbone_mtrace_receive_response 
 *
 * Receives incoming mbone_mtrace (code 0x1e) packet and funnels it
 * into appropriate mtrace queue.
 */
void ip_mbone_mtrace_receive_response (igmptype *igmp, paktype *pak, 
				       int length)
{
    iphdrtype          *ip;
    mbone_mtraceQtype  *mqueue;
    mbone_mtrace_query *query;

    ip = (iphdrtype *) ipheadstart(pak);

    IGMP_BUGINF("\nIGMP: Received Mtrace response from %i (%s)",
		ip->srcadr, pak->if_input->namestring);

    if (length < sizeof(igmptype)) {
	IGMP_BUGINF("\nIGMP: Mtrace packet too short");
	datagram_done(pak);
	return;
    }

    query = (mbone_mtrace_query *) igmp->data;
  
    for (mqueue = (mbone_mtraceQtype *) ip_mbone_mtraceQ.qhead; mqueue;
	 mqueue = mqueue->next)	{
	if ((query->ttl_qid & 0x00ffffff) != mqueue->id) continue;
	enqueue(&mqueue->data, pak);
	GET_TIMESTAMP(pak->gp_timer);
	break;
    }

    /* 
     * This response is not for us.
     */
    if (!mqueue) datagram_done(pak);	
}
 
/* 
 * what_kind
 * 
 * Determines the type and version of the router specified by sending a 
 * DVMRP_ASKNBR2 request and processing the response.
 */
ulong what_kind (ipaddrtype ip, boolean display)
{
    mrinfoQtype *asknbr;
    paktype     *pak;
    igmptype    *igmp;
    ulong         version;

    version = 0;
    asknbr = malloc(sizeof(mrinfoQtype));
    if (!asknbr) return(version);

    enqueue(&ip_mrinfoQ, asknbr);
    asknbr->id = ip;
  
    TIMER_START(asknbr->timeout, MRINFO_TIMEOUT);
    asknbr->timeout_flag = FALSE;
    asknbr->tty = stdio;

    dvmrp_send_asknbr2_request (ip, 0);

    if (display) print_hop(ip);

    edisms((blockproc *) ip_mrinfoBLOCK, (ulong) asknbr);
  
    if (asknbr->data.count) {
	pak = dequeue(&asknbr->data);
	igmp = (igmptype *) ipdatastart(pak);
	version = igmp->address;
	if (display) {
	    dvmrp_print_version(version, FALSE);
	    printf(" didn't respond\n");
	}
	datagram_done(pak);
    }
    unqueue(&ip_mrinfoQ, asknbr);
    free(asknbr);
    return(version);
}

/* 
 * fixup_stats
 *
 * Corrects packet counts in trace response for counter roll-overs.
 */
static void fixup_stats (mbone_mtrace_resp *base, mbone_mtrace_resp *new, 
			 int reps)
{
    int rno=reps;
    mbone_mtrace_resp *b=base+reps, *n=new+reps;
  
    while (--rno >= 0) {
	if( (--n)->tr_pktcnt < (--b)->tr_pktcnt) break;
    }
    if (rno < 0) return;
  
    rno = reps;
    b = base+rno;
    n = new+rno;

    while(--rno >= 0) {
	(--b)->tr_pktcnt = (--n)->tr_pktcnt;
    }
}

/* 
 * scale_time
 *
 * scales a time value and returns the appropriate suffix
 * "ms" or "s".
 */
static char *scale_time (int *time)
{
    if (*time > -1000 && *time < 10000) return("ms ");

    *time /= 1000;
    return("s ");
}

/* 
 * stat_line 
 *
 * Produce one line of packet loss and statistcs for output based on the two
 * queries given.
 */
static void stat_line (mbone_mtrace_resp *r, mbone_mtrace_resp *s, int next)
{
  
    ulong timediff = (s->tr_qarr - r->tr_qarr) >> 16;
    ulong ilost, glost;
    int   ipct, gpct;
    ulong iout = s->tr_ifout - r->tr_ifout;
    ulong gout = s->tr_pktcnt - r->tr_pktcnt;
    int   ipps, gpps;
    char  istr[8], gstr[8];
    uchar have = NEITHER;

    if (timediff == 0) timediff = 1;
    ipps = iout / timediff;
    gpps = gout / timediff;

    if (iout || s->tr_ifout != 0xFFFFFFFF) have |= OUTS;

    if (next) {
	--r; 
	--s;
	if (s->tr_ifin != 0xFFFFFFFF || r->tr_ifin != 0xFFFFFFFF) have |= INS;
    }
  
    switch (have) {
    case INOUT:
	ilost = iout - (s->tr_ifin - r->tr_ifin);
	if (iout) {
	    ipct = (ilost * 100 + (iout >> 1)) / iout;
	} else {
	    ipct = 0;
	}
	if ((-100 < ipct) && (ipct < 101) && (iout > 10)) {
	    sprintf(istr, " %d", ipct);
	} else {
	    memcpy(istr, " --", 4);
	}
    
	glost = gout - (s->tr_pktcnt - r->tr_pktcnt);
	if (gout) {
	    gpct = (glost * 100 + (gout >>1))/gout;
	} else {
	    gpct = 0;
	}
	if ((-100 < gpct) && (gpct < 101) && (gout > 10)) {
	    sprintf(gstr, " %d", gpct);
	} else {
	    memcpy(gstr, " --", 4);
	}
    
	printf("   %d/%d =%s%%      %d pps    %d/%d =%s%%  %d pps\n",
	       ilost, iout, istr, ipps, glost, gout, gstr, gpps);
	break;
    
    case INS:
	iout = s->tr_ifin - r->tr_ifin;
	gout = s->tr_pktcnt - r->tr_pktcnt;
	ipps = iout / timediff;
	gpps = gout/ timediff;
    
    case OUTS:
	printf ("       %d         %d pps           %d    %d pps\n",
		iout, ipps, gout, gpps);
	break;

    case NEITHER:
	printf("\n");
	break;
    }
}

/*
 * print_stats
 *
 * Output the results of statistics returned through a multicast traceroute.
 * Uses the results of two separate traces to acquire packet loss, etc.
 *
*/
static void print_stats (mbone_mtrace_resp *base, mbone_mtrace_resp *new, 
			 mbone_mtrace_resp *prev, igmptype *igmp, 
			 ulong ts, ulong tr, int reps)
{
    int rtt, hop, ttl;
    char  *ms, *flagstr;
    ulong smask, resptime, qarrtime;
    int   rno=reps-1;
    mbone_mtrace_resp *b, *p, *n;
    mbone_mtrace_query *query;
    nametype *name;

  
    b = base + rno;
    p = prev + rno;
    n = new + rno;
    query = (mbone_mtrace_query *) igmp->data;
  
    resptime = tr;
    qarrtime = new->tr_qarr;
    ttl = n->tr_fttl;
    smask = ip_bitcount2mask(b->tr_smask);
 
    printf("  Source        Response Dest");
    printf("    Packet Statistics For     Only For Traffic\n");
    printf("%i    %i    All Multicast Traffic     From %i\n",
	   ((b->tr_inaddr & smask) == (query->tr_src & smask)) ? 
	   query->tr_src : 0, query->tr_raddr, query->tr_src);

    rtt = time_diff(ts, resptime);
    ms  = scale_time(&rtt);

    printf("     |       __/  rtt %5d%s  Lost/Sent = Pct  Rate     To %i\n",
	   rtt, ms, igmp->address);

    hop = time_diff(qarrtime, resptime);
    ms  = scale_time(&hop);

    printf("     v      /     hop %5d%s", hop, ms);
    printf("  ---------------------     --------------------\n");
  
    while(TRUE)  {
	if((n->tr_inaddr != b->tr_inaddr) || (n->tr_inaddr != b->tr_inaddr)) {
	    printf("Route changed, start again.\n");
	    return;
	}
	
	flagstr = flag(n->tr_rflags);
	
	if (n->tr_inaddr != n->tr_outaddr)
	   printf("%16i\n", n->tr_inaddr);

	name = reg_invoke_ip_address_lookup(n->tr_outaddr);
	printf("%16i%s %s\n", n->tr_outaddr, (name) ? name->name : "?", 
	       flagstr);

	if (rno-- < 1) break;
    
	printf("     |     ^      ttl   %d   ", ttl);
	if (prev == new) {
	    printf("\n");
	} else {
	    stat_line(p, n, TRUE);
	}
	resptime = qarrtime;
	qarrtime = ((n-1)->tr_qarr);
	hop = time_diff(qarrtime, resptime);
	ms = scale_time(&hop);
    
	printf("     v     |      hop %5d%s", hop, ms);
	stat_line(b, n, TRUE);
    
	--b, --p, --n;
	if (ttl < n->tr_fttl) {
	    ttl = n->tr_fttl;
	} else {
	    ++ttl;
	}
    }

    printf("     |      \\__   ttl   %d   ", ttl);
    if (prev == new) {
	printf("\n");
    } else {
	stat_line(p, n, FALSE);
    }
    hop = time_diff(ts, qarrtime);
    ms = scale_time(&hop);
  
    printf("     v         \\  hop %5d%s", hop, ms);
    stat_line(b, n, FALSE);
    printf("%16i%16i\n", query->tr_dst, query->tr_raddr);
    printf("  Receiver      Query Source\n");
    return;
}

/* 
 * mbone_mstat_command 
 *
 * mstat <multicast source> <multicast destination> <group> [interval]
 * 
 * mstat produces and displays packet loss and rate statistics on a hop-by-hop
 * basis through a single branch of a multicast path (source->destination via 
 * group). The mechanism is identical to that of mtrace: a reverse path trace. 
 *
 */ 
void mbone_mstat_command (parseinfo *csb)
{

    mbone_mtraceQtype  *mtrace;
    ipaddrtype         mcast_source, mcast_dest, group, responder;
    mbone_mtrace_resp  *base, *prev = NULL, *new;
    paktype            *base_pak = NULL, *new_pak = NULL, *prev_pak = NULL;
    iphdrtype          *ip;
    igmptype           *igmp;
    int                length, base_rnum, new_rnum, interval, numstats=1;
    ulong              base_ts, new_ts, new_tr;
    uchar              m_ttl;
    clock_epoch        rtime;

    /*
     * Parse CL and allocate a new req/resp queue
     */
    interval = 10;
    m_ttl = 64;
    mtrace = ip_mbone_mtrace_parse_csb(csb, &group, &mcast_source,
				       &mcast_dest, &m_ttl, &responder);
    if (!mtrace) return;

    printf("Waiting to accumulate statistics....");
  
    /*
     * Send base request at full reverse path report length, give three
     * chances for success to allow alternate path methods (increasing ttl,
     * multicast responses, etc. to be tried).
     */
    ip_mbone_mtrace_send_request(group, mcast_source, mcast_dest, responder,
				 m_ttl, 32, 3, mtrace);
  
    if (mtrace->data.count) {

	/* 
	 * Good first response.	
	 */
	printf (".");

	base_pak = dequeue(&mtrace->data);
	ip = (iphdrtype *) ipheadstart(base_pak);
	igmp = (igmptype *) ipdatastart(base_pak);
	base = (mbone_mtrace_resp *) (igmp->data + QLEN);
	length = ip->tl - (ip->ihl << 2) - IGMPHEADERBYTES;
	base_rnum = (length-QLEN)/RLEN;
	
       	base_ts = mtrace->time_sent;
		
	while (numstats--) {
	    flush();
	
	    /* 
	     * Wait interval before sending next trace.
	     */
	    process_sleep_for(interval*1000);

	    /* 
	     * Send the next trace.
	     */
	    ip_mbone_mtrace_send_request(group,mcast_source,mcast_dest
					 ,responder,m_ttl,base_rnum, 3,
					 mtrace);

	    if (mtrace->data.count == 0) {
		printf("Second trace timed out.\n");
		break;
	    } else
		printf(".\n");
	  
	    new_pak = dequeue(&mtrace->data);
	    ip = (iphdrtype *) ipheadstart(new_pak);
	    igmp = (igmptype *) ipdatastart(new_pak);
	    new = (mbone_mtrace_resp *) (igmp->data + QLEN);
	    length = ip->tl - (ip->ihl << 2) - IGMPHEADERBYTES;
	    new_rnum = (length-QLEN) / RLEN;
	    new_ts = mtrace->time_sent;

	    clock_get_time(&rtime);
	    new_tr = (rtime.epoch_secs << 16) + 
		      ((rtime.epoch_frac >> 16) & 0xffff);
	    
	    if (base_rnum != new_rnum) {
		printf("Trace length does not match.\n");
		break;
	    }
	  
	    printf("Results after %d seconds:\n\n", ((new_ts-base_ts) >> 16));
	 
	    /* 
	     * Make adjustments for counter roll-over.	
	     */
	    fixup_stats(base, new, base_rnum);
	  
	    if (!prev)
		print_stats(base, new, new, igmp, new_ts, new_tr, base_rnum);
	    else {
		print_stats(base, new, prev, igmp, new_ts, new_tr,base_rnum);
		datagram_done(prev_pak);
	    }
	  
	    prev_pak = new_pak;
	    prev = new;
	}
      
    } else {
	printf("\nTimeout on first trace.\n");
    }
  
    if (base_pak) datagram_done(base_pak);
    if (new_pak) datagram_done(new_pak);

    unqueue(&ip_mbone_mtraceQ, mtrace);
    free(mtrace);

    flush();
    psioff(stdio);
}

/* ip_mbone_mtrace_parse_csb
 *
 * Parse the command line for mtrace and mstat calls. Returns an mtrace queue 
 * for this query.
 */
mbone_mtraceQtype *ip_mbone_mtrace_parse_csb (parseinfo *csb, 
		      ipaddrtype *group, ipaddrtype *mcast_source,
		      ipaddrtype *mcast_dest, uchar *ttl,
		      ipaddrtype *mtrace_resp)
{
  
    mbone_mtraceQtype *mtrace;
    nametype          *name1, *name2;
    boolean           prompt;
    char              buf[MAXLEN];
  
    prompt = FALSE;

    /* 
     * Get multicast source address: NOTE THIS IS NOT THE INTERFACE SOURCE
     * ADDRESS BUT THE SOURCE TO TRACE THE ROUTE FROM.
     */
    *mcast_source = GETOBJ(paddr, 1)->ip_addr;
    if (!*mcast_source) {
	prompt = TRUE;
	printf("Source address or name: ");
	if (rdtty(buf, MAXLEN)) {
	    parse_hostname(buf, mcast_source);
	}
    }
    if (martian(*mcast_source, TRUE) || IPMULTICAST(*mcast_source)) {
	printf("\n%% Bad source address");
	return(NULL);
    }
  
    /*
     * Get destination address for traceroute. If not interactive input and
     * not specified use this system's best address.
     */
    *mcast_dest = GETOBJ(paddr,2)->ip_addr;
    if (IPMULTICAST(*mcast_dest)) {
	*group = *mcast_dest;
	*mcast_dest = 0;
    } else {
	*group = 0;
    }
    if (!prompt && !*mcast_dest) {
	*mcast_dest = ip_best_local_address(*mcast_source, TRUE);
    }

    if (!*mcast_dest) {
	prompt = TRUE;
	printf("Destination address or name: ");
	if (rdtty(buf,MAXLEN)) {
	    parse_hostname(buf,mcast_dest);
	}
	if (!*mcast_dest || IPMULTICAST(*mcast_dest)) {
	    printf("\n%% Bad destination address");
	    return(NULL);
	}
    }
  
    /*
     * Get group address to trace route via. If not interactive input and
     * not specified use mbone-audio as group address.
     */
    if (!*group) *group = GETOBJ(paddr, 3)->ip_addr;
    if (!prompt && !*group) *group = 0;

    if (prompt) {
	printf("Group address or name: ");
	if (rdtty(buf,MAXLEN)) {
	    parse_hostname(buf,group);
	}
    }
    if ((!IPMULTICAST(*group) || IPMULTICAST_RSVD(*group)) && *group) {
	printf("\n%% Bad IP group address");
	return(NULL);
    }
  
    /*
     * Get Ttl.
     */
    if (ttl && prompt) {
	if (!GETOBJ(int,1)) {
	   *ttl = get_num_in_range("Multicast request TTL", 10, 1 , 255, *ttl);
	} else {
	    *ttl = GETOBJ(int,1);
	}
    }
  
    if (prompt) {
	printf("Response address for mtrace: ");
	if (rdtty(buf, MAXLEN)) {
	    parse_hostname(buf,mtrace_resp);
	}
    }
    
    /*
     *  Allocate data structure.
     */
    mtrace = malloc(sizeof(mbone_mtraceQtype));
    if (!mtrace) return (NULL);
  
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf("\nMtrace from %i to %i ", *mcast_source, *mcast_dest);
    if (*group) {
	printf("via group %i", *group);
    } else {
	printf("via RPF");
    }

    name1 = reg_invoke_ip_address_lookup(*mcast_source);
    name2 = reg_invoke_ip_address_lookup(*mcast_dest);
    printf("\nFrom source (%s) to destination (%s)\n", 
	   (name1) ? name1->name : "?", (name2) ? name2->name : "?");
    flush();
  
    enqueue(&ip_mbone_mtraceQ, mtrace);
    mtrace->id = ip_mbone_mtrace_id;
    mtrace->tty = stdio;

    return(mtrace);
}

/*
 * mtrace_get_if_addr
 *
 * Returns the proper ip address for the interface accounting for
 * DVMRP tunnels and ip unnumbered interfaces.
 */
static ipaddrtype mtrace_get_if_addr(idbtype *idb)
{
    if (!idb) return(0);
    if (DVMRP_TUNNEL(idb)) {
	return(dvmrp_get_tunnel_source(idb));
    } else {
	if(!idb->ip_address && idb->ip_unnumbered) {
	    return(idb->ip_unnumbered->ip_address);
	} else {
	    if (idb->ip_address) return(idb->ip_address);
	}
    }
    return(0);
}

/*
 * ip_mbone_mtrace_process_request
 *
 * Process an incoming traceroute request according to mrouted traceroute.
 */
boolean ip_mbone_mtrace_process_request (igmptype *igmp, paktype *pak,
					 int length)
{

    iphdrtype          *ip;
    gdbtype            *gdb;
    midbtype           *midb;
    mdbtype            *mdb;
    mbone_mtrace_query *mq;
    mbone_mtrace_resp  *mr;
    paktype            *newpak; 
    idbtype            *if_input; 
    idbtype            *idb;
    igmptype           *new_igmp;
    ipaddrtype         src, smask, source, next_hop, dest;
    ulong              newlength, iphdr_len;
    clock_epoch        qtime;
    int                errtype, datalen, repnum, rcount;
    boolean            lasthop, looped, multicasted, we_rp = FALSE, weak;
    uchar              rpf_type;
  
    clock_get_time(&qtime);
   
    ip = (iphdrtype *) ipheadstart(pak);
    src = ip->srcadr;
    if_input = pak->if_input;
    iphdr_len = ip->ihl << 2;
    mq = (mbone_mtrace_query *) igmp->data;
    datalen = length - IGMPHEADERBYTES;
    mdb = NULL;
    gdb = NULL;
    idb = NULL;

    errtype = TR_NO_ERR;
    weak = igmp->address ? FALSE : TRUE;

    IGMP_BUGINF("\nIGMP: Received Mtrace request from %i (%s) to %i",
		ip->srcadr, if_input->namestring, ip->dstadr);
    IGMP_BUGINF("\n      Mtrace source %i, dest %i, qid %x", mq->tr_src, 
		mq->tr_dst, mq->ttl_qid & 0x00ffffff);
    IGMP_BUGINF("\n      Mtrace group %i, responder %i, rttl %d",
		igmp->address, mq->tr_raddr, (mq->ttl_qid & 0xff000000) >> 24);
    IGMP_BUGINF(", nhops %d", igmp->code);   
    
    /*
     * Ignore duplicates. We dont check for 3.4 mrouted responses
     * anymore. We have to special case multicast queries because, we may
     * want to hear the same qid again as a unicast query.
     */
    if (old_query == (mq->ttl_qid & 0x00ffffff)) {
	IGMP_BUGINF("\n      Ignoring duplicate Mtrace packet, id: %x", 
		    mq->ttl_qid & 0x00ffffff);
	return(TRUE);
    } else if ((IPMULTICAST(ip->dstadr)) && 
	       (!IPMULTICAST_RSVD(ip->dstadr)) &&
	       (old_multicast_query == (mq->ttl_qid & 0x00ffffff))) {
	/*
         * If we received a multicast query which we received before,
	 * drop it.
	 */
	IGMP_BUGINF("\n     Ignoring duplicate Mtrace packet, id: %x",
		    mq->ttl_qid & 0x00ffffff);
	return(TRUE);
    } else {
	old_query = mq->ttl_qid & 0x00ffffff;
	if (IPMULTICAST(ip->dstadr)) {
	    old_multicast_query = old_query;
	}	
    }

    /* 
     * Is packet at report max? Drop it. 
     */
    repnum = igmp->code;
    rcount = (datalen - QLEN) /	RLEN;

    /* 
     * Determine if packet was multicasted and if we are the last hop to
     * the destination.
     */
    multicasted = (IPMULTICAST(ip->dstadr) && !(ip->dstadr == ALLROUTERS));
    lasthop = ((boolean) samecable(mq->tr_dst) && !rcount);

    if (rcount == repnum) {
	IGMP_BUGINF("\n      Mtrace request is full");
	errtype = TR_NO_SPACE;
    }

    /*
     * Check to see if the request was originated by this system
     * and this system is the last hop. If so, accept it.
     */
    looped = (ip->srcadr == ip->dstadr && ip_ouraddress(ip->srcadr));

    /* 
     * Make sure the incoming interface supports multicast. Then use
     * ip_mroute() to grab RPF neighbor for src and group. Make exception
     * for requests originated by this system for this system (looped).
     */
    if (!ip_mrouter_running) {
	if (!looped || !ip_router_running) {
	    IGMP_BUGINF("\n      IP multicast not enabled on router");
	 
	    /*
	     * Drop the request if it was multicasted, there may be other
	     * multicast routers that would like to respond.
	     */
	    if ((multicasted || ip->dstadr == ALLROUTERS)
		&& datalen == QLEN) return(TRUE);
	   
	    if (!errtype) { 
		errtype = TR_NO_MULTI;
	    }
	}
    }	
	
    /* 
     * Allow initial queries to come in on non-multicast interfaces
     * as long as the mrouter is enabled.
     */
    if (!(if_input->ip_mrouting || lasthop)) {
	IGMP_BUGINF("\n      IP multicast not enabled on interface %s",
		    if_input->namestring);

	/*
	 * Drop multicasted requests.
	 */ 
	if ((multicasted || ip->dstadr == ALLROUTERS) &&
	    datalen == QLEN) return (TRUE);

	if (!errtype) {
	    errtype = TR_NO_MULTI;
	}
    }
      
    /* 
     * If this is a query and it has been multicast, are we the last
     * hop router to the destination? If so then start the trace
     * packet upstream, otherwise forward the packet if it can be
     * and return.
     */
    if (datalen == QLEN && !lasthop && !errtype) {
	if (!multicasted) return(TRUE);
	  
	/* 
	 * Not for us, but it is multicast so forward it.
	 */
	mdb = ip_mroute(pak, ip);
	if (!mdb) {
	    IGMP_BUGINF("\n      Can't forward request, RPF failure");
	    return(TRUE);
	}
	  
	/* 
	 * Did this request come from a router other than 
	 * ourselves? 
	 */
	if (ip_ouraddress(ip->srcadr)) {
	    IGMP_BUGINF("\n      Packet looping");
	    return(TRUE);
	}

	IGMP_BUGINF("\n      Forwarding request");

	mdb->fastswitch = FALSE;
	ip_mroute_replicate(mdb, pak);
	    
	/* 
	 * We'll need to be able to hear this one again possibly, but
         * we won't forward the multicast query again.
	 */
	old_query = 0;
	    
	/* 
	 * Tell igmp_process() to not free the pak.
	 */
	return(FALSE);
    }

    /* 
     * Any errors prior to this will be fatal and we don't need to do any
     * more processing. If this is a weak mtrace, proceed directly to an
     * rpf check.
     */
    if (!errtype && !weak) {

	/*
	 * Do route lookup on requested group and source. Both addresses are
	 * obtained from the contents of the mtrace packet.
	 */
	gdb = ip_get_gdb(igmp->address);
	if (gdb) {
	    we_rp = ip_ouraddress(gdb->rp.address);

	    /*
	     * This informational message will get overwritten if
	     * there are more serious errors below.
	     */
	    if (we_rp) errtype = TR_HIT_RP;
	    
	    mdb = ip_get_best_mdb(gdb, mq->tr_src);
	    if ((gdb->mdb.if_input || we_rp) && IS_SPARSE_GROUP(gdb)) {
		if (!mdb || !(mdb->flags & SPT_FLAG)) mdb = &gdb->mdb;
	    }
	}

	if (!mdb) {
	    
	    /* 
	     *  There was no multicast state for this (S,G), go down
	     *  to weak mtrace to get an rpf neighbor if there is one.
	     */
	     weak = TRUE;
	     IGMP_BUGINF("\n      No state found, switching to weak mtrace");
	
	} else {
	    
	    /*
	     * If there was state, make sure that there is an RPF
	     * neighbor or an incoming interface.
	     */
	    if ((mdb->if_input == nullidb) ||
		(!mdb->rpf_nbr && !mdb->if_input && mdb != &gdb->mdb)) { 
	    IGMP_BUGINF("\n      No %s for (%i, %i)", (mdb) ? "RPF neighbor" : 
			"route", mq->tr_src, igmp->address);

	    /* 
	     * Don't send an error if this was a multicasted request
	     */
	    if ((multicasted || ip->dstadr == ALLROUTERS) && datalen == QLEN) {
		return(TRUE);
	    }
	    errtype = TR_NO_RTE;
	    } else {
	
		IGMP_BUGINF("\n      Mtrace RPF neighbor %i (%s)", 
			    mdb->rpf_nbr, (mdb->if_input) ? 
			    mdb->if_input->namestring : "Null");

		/*
		 * Is this a negative cache entry? Exclude check in RP
		 * so we can mtrace from RP back to source when there
		 * are only members on the (S,G) incoming interface.
		 */
		if (MDB_OUTPUTQ_NULL(mdb) && !we_rp && !looped) {
		    IGMP_BUGINF("\n      Negative cache entry");
		    errtype = TR_PRUNED;
		} else {

		    /*
		     * Make sure that the query destination is on the 
		     * forwarding path for this (S,G) on this router.
		     */
		    if (lasthop) {
			idb = samecable(mq->tr_dst);
		    } else {
			idb = if_input;
		    }
		    midb = ip_get_midb(&mdb->if_outputQ, idb, src, 
				       IS_SPARSE_GROUP(gdb));
 		    if (ipmulticast_test_boundary(idb, mdb->group) ||
			ipmulticast_test_boundary(mdb->if_input, mdb->group)) {
			errtype = TR_SCOPED;
			IGMP_BUGINF("\n      Mtrace hit scoped boundary");
		    } else {
			if (midb) {
			    if (midb->state == MIDB_PRUNE_STATE) {
				errtype = TR_OPRUNED;
			    } else {
				if (rcount && mdb->if_input == idb)
				    errtype = TR_RPF_IF;
			    }
			    if (errtype) IGMP_BUGINF(
				"\n      Request came in on %s interface (%s)",
				(errtype == TR_RPF_IF) ? "RPF" : "pruned",
				idb->namestring);
			}	
		    }
		}	
	    }
	}
    }

    /* 
     * Fill in the packet fields, stats, etc. 
     */
    newlength = (length + RLEN + IPHEADERBYTES(NOPT));
  
    newpak = getbuffer(newlength);
    if (!newpak) {
	return(TRUE);
    }

    /*
     * Copy just the IGMP portion of the original packet. This way
     * we avoid inadvertently including ip options.
     */ 
    new_igmp = (igmptype *) (ipheadstart(newpak) + IPHEADERBYTES(NOPT));
    bcopy(igmp, new_igmp, length);

    pak->flags &= ~PAK_SENDSELF;
    newpak->flags &= ~PAK_SENDSELF;
  
    mq = (mbone_mtrace_query *) new_igmp->data;
    mr = (mbone_mtrace_resp *) (((char *) new_igmp->data + datalen));
  
    memset(mr, 0, sizeof(mbone_mtrace_resp));
  
    mr->tr_qarr = (qtime.epoch_secs << 16) + 
	((qtime.epoch_frac >> 16) & 0xffff);
  
    if (mdb) {
	mr->tr_rmtaddr = mdb->rpf_nbr;

	/*
	 * Record routing protocol type. Really what routing table we
	 * RPF'd from.
	 */
	mr->tr_rproto = proto_code(mdb->rpf_type);

	/*
	 * Supply IP address of interface. 
	 */
	mr->tr_inaddr = mtrace_get_if_addr(mdb->if_input);

	/* 
	 * Use idb total multicast counters to track input and output flow.
	 */
	mr->tr_ifout = if_input->ip_multicast_out;
	mr->tr_ifin = (mdb->if_input) ? mdb->if_input->ip_multicast_in : 0;
	mr->tr_pktcnt = mdb->uses;
    }
  
    /* If we are doing a weak mtrace (group 0.0.0.0), then fill in the 
     * RPF neighbor towards the "source" and the limited information we can
     * set everything else to zero.
     */

    if (weak) {
	next_hop = 0;

	IGMP_BUGINF("\nWeak mtrace request, ");
	idb = ip_rpf_lookup(mq->tr_src, MASK_HOST, &source, &smask, &next_hop,
			    &rpf_type, ROUTINGDEPTH);
	if (!idb || idb == nullidb) {
	    IGMP_BUGINF("\n Failed RPF lookup");
	    errtype = TR_NO_RTE;
	} else {
	    IGMP_BUGINF("RPF Neighbor to %i is %i", mq->tr_src, next_hop);
	    
	    mr->tr_inaddr  = mtrace_get_if_addr(idb);
	    mr->tr_rmtaddr = next_hop;
	    mr->tr_ifout   = if_input->ip_multicast_out;
	    mr->tr_ifin    = idb->ip_multicast_in;
	    mr->tr_pktcnt  = 0;
	    mr->tr_rproto  = proto_code(rpf_type);
	    mr->tr_smask   = ip_bitsinmask(smask);	
	}
    }	
    /* 
     * Outgoing interface address to get back to destination. Consider
     * unnumbered interfaces.
     */
    mr->tr_fttl = if_input->ip_multicast_threshold;
    mr->tr_outaddr = if_input->ip_address;
    if (DVMRP_TUNNEL(if_input)) {
	mr->tr_outaddr = dvmrp_get_tunnel_source(if_input);
    } else {
	if (!mr->tr_outaddr && if_input->ip_unnumbered) {
	    mr->tr_outaddr = if_input->ip_unnumbered->ip_address;
	}
    }
  
    /* 
     * Are we oversized? 
     */
    if ((char *) mr + RLEN > (char *) ipheadstart(newpak) + MAXIPPACKETSIZE) {
	errtype = TR_NO_SPACE;
    
	/* 
	 * Put error condition in previous hop's report.
	 */
	mr = (mbone_mtrace_resp *)mr - 1;
    }
    mr->tr_rflags = errtype;
    
    /* 
     * Have we got a fatal error condition? If so, bail out and send back a 
     * response with that condition.
     */
    if ((errtype & 0x80) || (errtype == TR_NO_RTE) || 
	(errtype == TR_WRONG_IF)) {
	if (!(multicasted && lasthop) || ip_ouraddress(mq->tr_dst)) {
	    new_igmp->type = IGMP_MTRACE_RESP;
	    new_igmp->checksum = 0;
	    new_igmp->checksum = ipcrc((ushort *) new_igmp, newlength - 
				   IPHEADERBYTES(NOPT));

	    if (IPMULTICAST(mq->tr_raddr)) {
		mbonetracedefaultparams.ttl = (mq->ttl_qid & 0xff000000) >> 24;
	    } else {
		mbonetracedefaultparams.ttl = TTL_DEFAULT;
	    }
	    mbonetracedefaultparams.ident = ipdefaultparams.ident;

	    ipwrite(newpak, newlength, IGMP_PROT, mr->tr_outaddr, 
		    mq->tr_raddr, NULL, NULL, &mbonetracedefaultparams);
	    
	    IGMP_BUGINF("\nIGMP: Reply with Mtrace error %d (%s) to %i", 
			errtype, flag(errtype), mq->tr_raddr);
	}
	return(TRUE);
    }

    /* 
     * Count the number of 1-bits in the source mask. 
     */ 
    if (!weak) {
	if (ip_rpf_lookup(mq->tr_src, MASK_HOST, &source, &smask, &next_hop, 
			  &rpf_type, ROUTINGDEPTH)) {
	    mr->tr_smask = ip_bitsinmask(smask);
	}
    }
    /* 
     * At the end of the line? Forward response back to destination. Otherwise,
     * send it on to the next hop (mdb->rpf_nbr).
     */
    if ((rcount+1 == repnum) || (!weak && (!mdb || !mdb->rpf_nbr)) ||
	(weak && (!next_hop || ip_ouraddress(mq->tr_src)))) {
       	new_igmp->type = IGMP_MTRACE_RESP;
       	dest = mq->tr_raddr;
	
	if (IPMULTICAST(dest)) {
	    mbonetracedefaultparams.ttl = (mq->ttl_qid & 0xff000000) >> 24;
	} else {
	    mbonetracedefaultparams.ttl = TTL_DEFAULT;
	}
	idb = NULL;
    } else {
	new_igmp->type = IGMP_MTRACE_TYPE;
	
	if (weak) {
	    dest = next_hop;
	} else {
	    dest = mdb->rpf_nbr;
	    idb = mdb->if_input;
	}

	if (idb && idb->hwptr->tunnel && 
	    idb->hwptr->tunnel->mode == TUN_MODE_GRE_IP) dest = ALLROUTERS;
    }

    IGMP_BUGINF("\nIGMP: %s Mtrace %s to %i", 
	(new_igmp->type == IGMP_MTRACE_RESP) ? "Reply with" : "Forwarding",
	(new_igmp->type == IGMP_MTRACE_RESP) ? "response" : "request", dest);
    if (idb) IGMP_BUGINF(" on %s", idb->namestring);

    /*
     * Checksum and send packet to IP.
     */
    new_igmp->checksum = 0;
    new_igmp->checksum = ipcrc((ushort *) new_igmp,
			       newlength - IPHEADERBYTES(NOPT));
    mbonetracedefaultparams.ident = ipdefaultparams.ident;
    
    ipwrite(newpak, newlength, IGMP_PROT, mr->tr_inaddr, dest, idb, NULL, 
	    &mbonetracedefaultparams);
    
    return(TRUE);
}

/* 
 * ip_mrinfoBLOCK
 *
 * Determine if EXEC process running mrinfo should wakeup.
 */
boolean ip_mrinfoBLOCK (mrinfoQtype *mrinfo)
{

    /*
     * Return to EXEC if data was received.
     */
    if (mrinfo->data.count) return(FALSE);

    /*
     * Keyboard interrupt.
     */
    if (psipending2(mrinfo->tty)) {
	mrinfo->interrupted = TRUE;
	return(FALSE);
    }

    /*
     * Has the request timed out?
     */
    if (AWAKE(mrinfo->timeout)) {
	mrinfo->timeout_flag = TRUE;
	return(FALSE);
    }

    /*
     * Keep process asleep, if not.
     */
    return(TRUE);
}
