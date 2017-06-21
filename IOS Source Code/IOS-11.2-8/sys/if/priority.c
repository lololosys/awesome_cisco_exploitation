 /* $Id: priority.c,v 3.7.6.23 1996/09/11 23:06:56 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/priority.c,v $
 *------------------------------------------------------------------
 * priority.c -- Priority Output Queueing
 *
 * 15-July-1990, Kirk Lougheed
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: priority.c,v $
 * Revision 3.7.6.23  1996/09/11  23:06:56  snyder
 * CSCdi68880:  more const stuff
 *              120 out of image, 144 data
 * Branch: California_branch
 *
 * Revision 3.7.6.22  1996/09/11  17:39:00  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.7.6.21  1996/08/24  04:59:45  fred
 * CSCdi67070:  remove upper bound from red queues
 * Branch: California_branch
 *
 * Revision 3.7.6.20  1996/08/19  06:36:24  asastry
 * CSCdi65585:  Priority queuing not working for bridged packets.
 * Branch: California_branch
 *
 * Revision 3.7.6.19  1996/08/17  00:23:34  fred
 * CSCdi61598:  spurious memory access in fair_enqueue/dequeue
 *              caused by tty interrupt interleaving with nets interrupt;
 *              need to control interrupts entirely, so use standard
 *              routines.
 * Branch: California_branch
 *
 * Revision 3.7.6.18  1996/07/30  01:15:01  ccartee
 * CSCdi60470:  Custom-queue-list command cant be applied to channelized
 * T1 interfac
 * Branch: California_branch
 * Change name of encaps_queueing to encaps_queuing_allowed and change
 * type to RETVAL.  Insure that default behavior is to allow queuing
 * commands on interface.  Cauterize legacy references encaps_queuing to
 * remove references to passed in (ptr to) return value.
 *
 * Revision 3.7.6.17  1996/07/27  16:25:30  ccpoe
 * CSCdi64289:  Fancy queueing is not allowed on a ethernet interface
 * Branch: California_branch
 *
 * Revision 3.7.6.16  1996/07/25  23:35:08  ccartee
 * CSCdi63068:  queuing strategy cannot be recovered after reboot or
 * download
 * Branch: California_branch
 * Invoke parser operation in traversed 'plist' order, and not in fixed
 * 'command' order.
 *
 * Revision 3.7.6.15  1996/07/19  02:10:27  ccpoe
 * CSCdi53787:  Interface queueing shouldnt be allowed with VC Queueing.
 * reg_invoke_encaps_queueing() should only be called when configuring
 * fancy queueing.
 * Branch: California_branch
 *
 * Revision 3.7.6.14  1996/07/18  05:25:56  ccpoe
 * CSCdi53787:  Interface queueing shouldnt be allowed with VC Queueing
 * Branch: California_branch
 *
 * Revision 3.7.6.13  1996/07/13  11:36:28  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.7.6.12  1996/07/02  08:13:17  agt
 * CSCdi46873:  queue <n> lowest-custom <m> command not saved to NVRAM
 * Branch: California_branch
 * Fix NV generation of 'lowest-custom' command.
 *
 * Revision 3.7.6.11  1996/06/28  23:06:53  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.6.10  1996/06/21  19:17:08  fred
 * CSCdi61028:  fair-queue command on ethernet saved with truncated
 * parameters
 * Branch: California_branch
 *
 * Revision 3.7.6.9  1996/06/21  19:06:38  fred
 * CSCdi57507:  RSVP gets installed with Conversation number 0 on ethernet.
 *         Install registry callback from WFQ setup to RSVP to make sure
 *         that some reservable queues are allocated.
 * Branch: California_branch
 *
 * Revision 3.7.6.8  1996/06/07  20:46:34  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 *              interface. Save default in a variable and restore that
 *              when folks turn off RED, Priority, or Custom Queuing.
 * Branch: California_branch
 *
 * Revision 3.7.6.7  1996/06/05  21:14:57  tylin
 * CSCdi59545:  Spurious accesses with priority queuing
 * Resolve null pointer reference
 * Branch: California_branch
 *
 * Revision 3.7.6.6  1996/05/17  11:23:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.34.4  1996/05/05  23:10:02  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.34.3  1996/04/26  07:55:24  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.34.2  1996/04/03  14:38:39  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.6.5  1996/05/15  05:40:22  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.7.6.4  1996/05/13  14:47:20  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.7.6.3  1996/04/17  13:40:23  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.7.6.2  1996/03/26  20:26:05  asastry
 * CSCdi51014:  Protocol queueing may be broken - debug always shows normal
 * Branch: California_branch
 *
 * Revision 3.7.6.1  1996/03/18  20:14:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.16.4  1996/03/16  06:55:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.16.3  1996/03/13  01:35:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.16.2  1996/03/07  09:44:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.16.1  1996/01/24  22:18:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/03/04  10:49:46  fred
 * CSCdi47191:  MBRI cannot disable WFQ - screws up multilink
 *         enable WFQ use by any link that cares t
 *
 * Revision 3.6  1995/12/07  22:41:59  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/11/28  00:23:13  fred
 * CSCdi43173:  Fast-switching fails over mip
 *         If Fair Queuing already configured on MIP, deconfigure it.
 *         If Fair Queuing selected but not yet configured, select FCFS
 *         instead.
 *         MIP does not support queuing that requires backpressure
 *
 * Revision 3.4  1995/11/21  20:43:42  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.3  1995/11/17  09:30:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:51:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.24  1995/11/08  21:03:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.23  1995/10/21  19:09:51  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * Consistently enable use of fast path with priority/custom/fair queuing
 * for bridging
 *
 * Revision 2.22  1995/09/23  21:27:01  gglick
 * CSCdi40746:  X.25 and LAPB not working
 *
 * Revision 2.21  1995/09/16  01:07:28  fred
 * CSCdi40357:  Make WFQ configuration work correctly with Virtual and
 * Null IDBs.
 *
 * Revision 2.20  1995/09/07  04:32:01  fred
 * CSCdi39816:  interfaces requiring secondary queuing are broken.
 *              This includes tunnels, LAPB, X.25, Loopback, and a few
 *              others
 *
 * Revision 2.19  1995/09/06  17:45:23  raj
 * CSCdi39170:  show queueing does not use flow control
 * Actually it does but the flow control is being turned on and off in
 * subroutines so much that it's not effective.  I moved all flow control
 * to the higher level routines.
 *
 * Revision 2.18  1995/09/02  10:06:45  fred
 * CSCdi39438:  no fair does not disable WFQ
 *         implement idb->queue_algorithm to indicate the
 *         queuing algorithm deployed.
 *
 * Revision 2.17  1995/09/01  22:49:11  asastry
 * CSCdi39224:  WFQ conversation counters are inaccurate on 7000.
 *
 * Revision 2.16  1995/08/23  18:19:01  fred
 * CSCdi39010:  WFQ + ip route-cache cbus on FSIP fails
 * Code cleanups to improve user understandibility of what's happening
 *
 * Revision 2.15  1995/08/20  07:24:57  agt
 * CSCdi38970:  fair-queue congestive bandiwdth not in write term; can
 * cause reload
 *
 * Revision 2.14  1995/08/08  19:28:56  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.13  1995/08/03  21:43:38  agt
 * CSCdi37292:  Must remove priority-group configuration first msg at
 * reload
 * Also, clean up corner case of NVGen "no fair-queue" (CSCdi37619)
 *
 * Revision 2.12  1995/07/31  10:00:48  agt
 * CSCdi37619:  no fair-queue command is not written into NVRAM
 * Generate config for both FCFS and FAIR queue modes from the parse tree
 * for FAIR queuein
 *
 * Revision 2.11  1995/07/27  10:54:51  irfan
 * CSCdi37751:  Weighted Fair Queueing not configurable over Async
 * interfaces
 * fix it. Also add a cumulative counter of all packets FQ thinks it
 * ever kept around and a counter of all packets FQ thinks it gave back.
 *
 * Revision 2.10  1995/07/25  23:41:40  agt
 * CSCdi32666:  Router crashes after configuring a custom queue list
 * Check for priority or custom queue not having been allocated.
 *
 * Revision 2.9  1995/07/19  04:44:56  sberl
 * CSCdi37297:  Add debug for PAK_PRIORITIZED in priority_enqueue()
 *
 * Revision 2.8  1995/07/18  06:46:19  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.7  1995/07/10 05:39:13  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.6  1995/06/27  19:00:04  fred
 * CSCdi36454:  no priority-group did not clear idb->priority_list
 *
 * Revision 2.5  1995/06/21  21:17:28  fred
 * CSCdi35994:  LLC2 breaks fair queuing
 *         Rewrite fair_enqueue and fair_dequeue to use pak_enqueue
 *         and related services. Augment pak_enqueue services with
 *                 pak_queue_get_head(), pak_queue_get_tail(),
 *                 pak_queue_depth(), pak_queue_max_depth(),
 *                 pak_queue_set_max_depth(), and
 *                 pak_on_data_link_queue(paktype *pak)
 *         The latter is for the IBM code, which needs a way to determine
 *         whether a message is currently on a data link secondary queue
 *
 * Revision 2.4  1995/06/17  00:49:19  lbustini
 * CSCdi35984:  router hangs when ddr interface goes down
 * Don't share a single WFQ between rotary group members as this can cause
 * output_qcount to go negative.
 *
 * Revision 2.3  1995/06/15  15:18:28  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/09  03:34:52  fred
 * CSCdi35326:  %ALIGN-1-FATAL: Illegal access to a low address -
 *              fair_enqueue
 * CSCdi35163:  reported hang did not occur in this code version
 * CSCdi35342:  corrected syntax error in priority-list and queue-list
 *              command in the manner suggested in the DDTs.
 * Also - modified "show queueing fair" command output per customer
 * request
 *
 * Revision 2.1  1995/06/07  20:54:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "logger.h"
#include "network.h"
#include "config.h"
#include "../tcp/tcp.h"
#include "priority_private.h"
#include "access.h"
#include "../parser/parser_defs_priority.h"
#include "../if/traffic_shape.h"
#include "../if/network_debug.h"
#include "../ip/ip_registry.h"
#include "../xns/novell.h"
#include "../wan/ppp.h" /* CSCdi68512 */
#include "../os/element.h"
#include "../ui/parse_util.h"

/*
 * Local Storage
 */
pritype *parray[PRIORITY_MAXLIST];
pritype *cust_parray[PRIORITY_MAXLIST];
pritype *fair_parray[PRIORITY_MAXLIST];
static ushort priority_changed_option[PRIORITY_MAXLIST];
static priority_callback *priority_callbacks;
static sys_timestamp priority_entry_changed_time;
static boolean priority_callbacks_registered = FALSE;
static const char priority_list_name[] = "Priority List";
static const char priority_item_name[] = "Priority List Item";
static const char priority_callback_name[] = "Priority Callback";
static const char fair_queue_name[] = "Fair Queueing";

/*
 * Forward references
 */
static void priority_perform_change_callbacks(void);
static void priority_mark_list_changed(ulong list, ushort option);



/*
 * identify type of queuing
 */
ushort priority_type (hwidbtype *hwidb) {
    return(hwidb->queue_algorithm);
}

/*
 * diallst_getlink
 * Return protocol corresponding to passed access list.
 *
 */
uint diallst_getlink (int list)
{
    uint linktype;
 
    if ((list >= MINFASTACC) && (list <= MAXSLOWACC))
	linktype = LINK_IP;
    else if (((list >= MINTYPEACC) && (list <= MAXTYPEACC)) ||
	     ((list >= MINMACADDRACC) && (list <= MAXMACADDRACC)) ||
	     ((list >= MINXMACACC) && (list <= MAXXMACACC)))
	linktype = LINK_BRIDGE;
    else if ((list >= MINDECACC) && (list <= MAXDECACC))
	linktype = LINK_DECNET;
    else if ((list >= MINXNSACC) && (list <= MAXEXNSACC))
	linktype = LINK_XNS;
    else if ((list >= MINATALKACC) && (list <= MAXATALKACC))
	linktype = LINK_APPLETALK;
    else if ((list >= MINNOVACC) && (list <= MAXNOVSAPACC))
	linktype = LINK_NOVELL;
    else
	linktype = LINK_ILLEGAL;
 
    return(linktype);
}

/*
 * priority_allocate_list
 * Setup a priority list data structure
 */

pritype *priority_allocate_list (boolean custom, int number)
{
    pritype *p;
    int i;

    p = malloc_named(sizeof(*p), priority_list_name);
    if (p == NULL) {
	printf(nomemory);
	return(NULL);
    }
    p->plist = NULL;
    p->pdefault = custom ? 1 : PRIORITY_NORMAL;
    p->lowest_custom = 1;
    p->custom = custom;
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	if (custom) {
	    p->bytecount[i] = CUSTOM_QUEUE_PACKET_SIZE;
	    p->qsize[i] = CUSTOM_QUEUE_SIZE;
	    p->qsize_def[i] = CUSTOM_QUEUE_SIZE;
	} else {
	    p->qsize[i] = PRIORITY_QLIMIT*(i+1);
	    p->qsize_def[i] = PRIORITY_QLIMIT*(i+1);
	}
    }
    if (custom) {
	cust_parray[number] = p;
    } else {
	parray[number] = p;
    }
    return(p);
}

/*
 * priority_classify
 * Return priority level for a given datagram and output interface.
 */

int priority_classify (hwidbtype *output, paktype *pak)
{
    ushort typecode, sport, dport;
    plisttype *plist;
    char source[ADDRLEN_MAX*3], destination[ADDRLEN_MAX*3];
    int queue;
    iphdrtype *ippak;

    if (PRIORITY_ARRAY(output) == NULL) {
	queue = DEFAULT_QUEUE(output);
	if (priority_debug) {
	    buginf("\nPQ: %s: %s -> %s", output->hw_namestring,
		   print_linktype(pak->linktype),
		   idb_queue2name(output, queue));
	}
	return(queue);
    }

    for (plist = PRIORITY_ARRAY(output)->plist; plist; plist = plist->next) {

	if (plist->interface) {
	    if (!pak->if_input || plist->interface != pak->if_input->hwptr)
		continue;
	    if (priority_debug) {
		buginf("\nPQ: %s: %s -> %s", output->hw_namestring,
		       print_linktype(pak->linktype),
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}

	if (plist->linkcode != pak->linktype) {
	    continue;
	}

	if (plist->stun_group) {
	    if (reg_invoke_stun_prioritization(plist, pak, output)) {
		return(plist->priority);
	    }
	    continue;
	}

	if (plist->bstun_group) {
	    if (reg_invoke_bstun_prioritization(plist, pak, output)) {
		return(plist->priority);
	    }
	    continue;
	}

	if (plist->gt_size) {
	    if (pak->datagramsize <= plist->gt_size) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s: %s (%d bytes) -> %s", output->hw_namestring,
		       print_linktype(pak->linktype), pak->datagramsize,
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->lt_size) {
	    if (pak->datagramsize >= plist->lt_size) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s: %s (%d bytes) -> %s", output->hw_namestring,
		       print_linktype(pak->linktype), pak->datagramsize,
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->tcp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != TCP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (getfo(ippak))
		continue;
	    sport = TcpPkt(pak)->sourceport;
	    dport = TcpPkt(pak)->destinationport;
	    if ((sport != plist->tcp_port) && (dport != plist->tcp_port)) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s: %s (tcp %d) -> %s", output->hw_namestring,
		       print_linktype(pak->linktype), plist->tcp_port,
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->udp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != UDP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (getfo(ippak))
		continue;
	    sport = ((udptype *)ipdatastart(pak))->sport;
	    dport = ((udptype *)ipdatastart(pak))->dport;
	    if ((sport != plist->udp_port) && (dport != plist->udp_port)) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s: %s (udp %d) -> %s", output->hw_namestring,
		       print_linktype(pak->linktype), plist->udp_port,
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->ip_fragments) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (!getfo(ippak))
		continue;
	    if (priority_debug) {
		buginf("\nPQ: %s: %s fragment -> %s", output->hw_namestring,
		       print_linktype(pak->linktype),
		       idb_queue2name(output, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->testlist) {
	    if (!reg_invoke_access_check(plist->linkcode,
					 pak, (int) plist->testlist)) {
		continue;
	    }
	    if (priority_debug) {
		if (plist->linkcode == LINK_BRIDGE) {
		    typecode = reg_invoke_extract_typecode(pak);
		    buginf("\nPQ: %s: %s (0x%-04x) -> %s",
			   output->hw_namestring,
			   print_linktype(pak->linktype), typecode,
			   idb_queue2name(output, plist->priority));
		} else {
		    reg_invoke_sprintf_src_dst(
				  plist->linkcode, source, pak, SOURCE);
		    reg_invoke_sprintf_src_dst(
				  plist->linkcode, destination, pak,
				  DESTINATION);
		    buginf("\nPQ: %s: %s (s=%s, d=%s) -> %s",
			   output->hw_namestring,
			   print_linktype(pak->linktype), source, destination,
			   idb_queue2name(output, plist->priority));
		}
	    }
	    return(plist->priority);
	}
	if (priority_debug) {
	    buginf("\nPQ: %s: %s -> %s", output->hw_namestring,
		   print_linktype(pak->linktype),
		   idb_queue2name(output, plist->priority));
	}
	return(plist->priority);
    }
    if (priority_debug) {
	buginf("\nPQ: %s: %s (defaulting) -> %s", output->hw_namestring,
	       print_linktype(pak->linktype),
	       idb_queue2name(output, PRIORITY_ARRAY(output)->pdefault));
    }
    return(PRIORITY_ARRAY(output)->pdefault);
}

/*
 * priority_enqueue
 *
 * Enqueue a packet for output according to priority.
 * We are assuming an output interface is set in pak->if_output
 */
boolean priority_enqueue (
    hwidbtype *output,
    paktype *pak,
    enum HEADTAIL which)
{
    int value;

    /*
     * Figure out this datagram's priority code:
     *  - if not prioritizing, use normal priority queue.
     *  - Packets with PAK_PRIORITY set always have high priority.
     */
    if (which == HEAD) {
	/*
	 * Packet is returning to interface output queue after already
	 * having been classified.  Reuse classification that was stored
	 * away in packet header after packet was last dequeued.
	 */
	value = pak->oqnumber;
    } else if (pak->flags & PAK_PRIORITIZED) {
	/*
	 * Packet was classified prior to arriving here, so just take the queue
	 * number out of the packet.
	 */
	value = pak->oqnumber;
	if (priority_debug) {
	    buginf("\nPQ: PAK_PRIORITIZED %s: %s -> %s",
		   pak->if_output->hwptr->hw_namestring,
		   print_linktype(pak->linktype),
		   idb_queue2name(pak->if_output->hwptr, pak->oqnumber));
	}
    } else if (pak->flags & PAK_PRIORITY) {
	value = PRIORITY_HIGH;
	which = HEAD;
    } else {
	value = priority_classify(output, pak);
    }

    /*
     * Hang datagram on appropriate output queue
     */
    if (which == TAIL) {
	if (pak_enqueue(&(output->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    } else {
	if (pak_requeue(&(output->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    }
    if (priority_debug) {
	buginf(" -- congestion drop");
    }

    return(holdq_enqueue_fail(output, pak, which, value));
}

/*
 * priority_dequeue
 *
 * Dequeue the next packet from one of the hold queues
 */
static paktype *priority_dequeue (hwidbtype *idb)
{
    paktype *pak = NULL;
    int i;
    ushort datagramsize_old;

    /*
     * Handle priority queueing.
     * Get next packet in highest queue.
     */
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	if (idb->outputq[i].qhead) {
	    pak = pak_dequeue(&(idb->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		idb->output_qcount--;
		idb->counters.output_dequeued[i]++;
		pak->oqnumber = i;
		break;
	    }
	}
    }

    if (!pak)
	return(NULL);

    /*
     * Do any compression on output
     * Store pre-compression size for potential credit
     * The test below should be more generic.
     */
    datagramsize_old = pak->datagramsize;
    if (idb->tcp_compress ||
 		((pak->nif_output != NULL) &&
 		 (pak->nif_output->compression_negotiated_enabled))) {
	pak = reg_invoke_compress_header(pak->linktype, pak);
    }
    if (idb->comp_db) {
	pak = reg_invoke_encaps_compress(idb->enctype, pak);
    }
    /*
     * Credit traffic shaping statistics for overcharging which
     * results from last-minute knowledge of compression savings.
     * Put check beyond all compression routines to insure all
     * bytes savings are accounted for...
     */
    traffic_shape_credit(pak, datagramsize_old);

    return(pak);
}


/*
 * fair_enqueue
 *
 * Enqueue a packet for output according to it session number found in
 * the packet descriptor (paktype).
 */
boolean fair_enqueue (hwidbtype * output, paktype * pak, enum HEADTAIL which)
{
    ushort conversation;
    fair_queue_head *fq;
    fair_queue *this_queue;
    fair_queue *previous_queue;
    fair_queue *next_queue;
    paktype *tail;
    int comparison;
    ulong sequence;
    ushort datagramsize;

    fq = output->fair_queue_head;
    if (HEAD == which) {
	/* 
	 * requeuing a message that we just pulled out.
	 *
	 * the challenge is to put all the pointers and all the counters
	 * back as though the thing had never been dequeued in the first place.
	 *
	 * one could argue for a target of opportunity here - the queue is
	 * deeper than allowed (burst of interactive conversation when nearly
	 * full already) and this is from a multi-pak queue, we could discard
	 * the requeued message as a "deterministic early discard". Interesting
	 * idea to play with, but I'm not sure what the effect on throughput 
	 * rates would be.
	 */
	this_queue = fq->fq_last;
	if (!pak_requeue(&this_queue->q, pak)) {
	    datagram_done(pak);
	    this_queue->fq_discards++;
	    return(FALSE);
	} else if (1 == pak_queue_depth(&this_queue->q)) {
	    /* queue was empty; simple prepend */
	    fq->fq_active_conversations++;
	    this_queue->fq_next = next_queue = fq->fq_first;
	    this_queue->fq_fore = NULL;
	    if (next_queue)
		next_queue->fq_fore = this_queue;
	} else {
	    /* queue was not empty */
	    previous_queue = this_queue->fq_fore;
	    if (previous_queue) {
		/* it was middle or last, pull it out */
		next_queue = this_queue->fq_next;
		if (next_queue)
		    next_queue->fq_fore = previous_queue;
		previous_queue->fq_next = next_queue;

		/* now requeue the queue at the head */
		this_queue->fq_next = next_queue = fq->fq_first;
		this_queue->fq_fore = NULL;
		/*
		 * no need to check for NULL, as there was something in
		 * queue
		 */
		next_queue->fq_fore = this_queue;
	    }
	}
	/* back up the sequence number */
	pak->flags |= PAK_DLQ;
	pak->fair_sequence = fq->fq_sequence;
	fq->fq_sequence = fq->fq_resequence;
	fq->fq_first = this_queue;
	output->output_qcount++;
	return(TRUE);
    }

    /* normal case: tail queue 
     * Priority packets are hashed into one of FAIR_LINK_TYPE_HASH
     * number of 'private' conversations.  Allows for faster delivery,
     * because these packets are not delivered in sequence with data
     * packets of same conversation.
     *
     * Set hash mask, which is required parameter in conversation
     * classification functions.
     */
    pak->fq_hash_mask = fq->fq_hash_mask;
    if (pak->flags & PAK_PRIORITIZED) {
	conversation = pak->oqnumber;
    } else {
	if (pak->flags & PAK_PRIORITY) {
	    conversation = linktype_is_fq_conversation(pak);
	} else {
	    conversation = reg_invoke_determine_fair_queue_flow_id
		(pak->linktype, pak);
	}
	pak->oqnumber = conversation;
    }

    if (conversation < fq->fq_queue_count) {
	this_queue = fair_queue_address(fq, conversation);
	if (!pak_queue_depth(&this_queue->q)) {

	    /*
	     * first message in queue for this_queue conversation
	     */
	    if (!pak_enqueue(&this_queue->q, pak)) {

		/* 
		 * pak->refcount <= 0, 
		 * pak_queue_depth(q) > maximum (how?)
		 * or could not get an element 
		 */
		this_queue->fq_discards++;
		goto tail_drop;
	    }
	    pak->flags |= PAK_DLQ;
	    fq->fq_active_conversations++;
	    if (fq->fq_max_active_conversations < fq->fq_active_conversations)
		fq->fq_max_active_conversations = fq->fq_active_conversations;

	    datagramsize = pak->datagramsize;
	    sequence = fq->fq_sequence + pak->fair_weight * datagramsize;
	    comparison = sequence - fq->fq_high_sequence;
	    if (0 < comparison)	/* new queue tail? */
		fq->fq_high_sequence = sequence;
	    pak->fair_sequence = sequence;
	    previous_queue = fq->fq_first;
	    output->output_qcount++;
	    if (!previous_queue) {

		/* 
		 * first in entire queue - there were no active conversations
		 */
		fq->fq_first = this_queue;
		this_queue->fq_fore = NULL;
		this_queue->fq_next = NULL;
		return(TRUE);
	    }

	    /* 
	     * not first conversation queue; 
	     * sort against other conversations 
	     */
	    pak = pak_queue_get_head(&previous_queue->q);
	    comparison = sequence - pak->fair_sequence;
	    if (comparison < 0) {

		/* 
		 * conversation goes to head of the list 
		 */
		this_queue->fq_fore = NULL;
		this_queue->fq_next = previous_queue;
		previous_queue->fq_fore = this_queue;
		fq->fq_first = this_queue;
		return(TRUE);
	    }

	    /*
	     * conversation belongs later in the list, maybe the tail
	     */
	    next_queue = previous_queue->fq_next;
	    while (next_queue) {
		pak = pak_queue_get_head(&next_queue->q);
		/* check for NULL should be unnecessary, as this queue 
		   has waiting data */
		comparison = sequence - pak->fair_sequence;
		if (comparison < 0)
		    break;
		previous_queue = next_queue;
		next_queue = previous_queue->fq_next;
	    }
	    this_queue->fq_next = next_queue;
	    previous_queue->fq_next = this_queue;
	    this_queue->fq_fore = previous_queue;
	    if (next_queue)
		next_queue->fq_fore = this_queue;
	    return(TRUE);
	} /* end empty conversation queue case */

	/*
	 * pak is enqueued where there is already data
	 * we will enqueue to the tail of the conversation queue
	 *
	 * we want to discard when
	 *
	 *	the queue as a whole is very deep, >= discard threshold
	 * AND  this is not a routing or control message (PAK_PRIORITY)
	 * AND  the sequence number of this message says it is the last
	 *      in the queue
	 */
	tail = pak_queue_get_tail(&this_queue->q);
	datagramsize = pak->datagramsize;
	sequence = tail->fair_sequence + pak->fair_weight * datagramsize;
	pak->fair_sequence = sequence;

        comparison = sequence - fq->fq_high_sequence;
	if ((pak->flags & PAK_PRIORITY) ||
	    (comparison < 0 || output->output_qcount < fq->fq_max_enqueue)) {
	    if (pak_enqueue(&this_queue->q, pak)) {

		/*
		 * not first in conversation - 
		 * schedule and enqueue at tail of conversation queue
		 */
		pak->flags |= PAK_DLQ;
		output->output_qcount++;
	        if (0 < comparison)	/* new queue tail? */
		    fq->fq_high_sequence = sequence;
		return(TRUE);
	    }
	}

	/* 
	 * note that it this point, one could argue for "deterministic early 
	 * discard", similar to the "Random Early Discard" algorithm.
	 *
	 * the Random Early Discard algorithm suggests that one take an early
	 * frame from what might be the overburdening conversation and pitch
	 * it. Here, we do a tail drop from the overburdening conversation.
	 * We could, if we chose, add the tail but drop the head.
	 *
	 * I didn't do that, because I was concerned about benchmarking (would
	 * it make the overburdening conversation in a traffic generator test
	 * simply drop out of the mix or become unpredictable?) and because we
	 * would now have to sort the conversation into place. Here, we simply
	 * count a discard against the overburdening conversation.
	 */
	if (fair_debug)
	    buginf(" -- congestion drop");
	this_queue->fq_discards++;
    } else {
	if (fair_debug)
	    buginf(" -- bad conversation id drop");
    }
  tail_drop:
    output->counters.output_drops[PRIORITY_NORMAL]++;
    output->counters.output_total_drops++;	/* CSCdi39125 */
    datagram_done(pak);
    return(FALSE);
}

/*
 * fair_dequeue
 *
 * Dequeue the next packet from one of the hold queues
 */

static paktype *fair_dequeue (hwidbtype * idb)
{
    paktype *pak;
    paktype *next_message;
    fair_queue *this_queue;
    fair_queue *previous_queue;
    fair_queue *next_queue;
    int comparison;
    ulong sequence;
    ushort datagramsize_old;

    fair_queue_head *fq = idb->fair_queue_head;
    this_queue = fq->fq_first;
    if (!this_queue)
	return(NULL);
    if (!pak_queue_get_head(&this_queue->q) ||
	0 == pak_queue_depth(&this_queue->q)) {
	  holdq_reset(idb);
	  return(NULL);
	  if (fair_debug)
	      buginf("\nWFQ: Queue reset in fair_dequeue()");
    }
    
	    

    pak = pak_dequeue(&this_queue->q);
    if (!pak) /* this should never happen */
	  return NULL;

    pak->flags &= ~PAK_DLQ;
    fq->fq_last = this_queue;
    idb->output_qcount--;
    fq->fq_resequence = fq->fq_sequence;
    fq->fq_sequence = pak->fair_sequence;
    previous_queue = this_queue->fq_next;
    next_message = pak_queue_get_head(&this_queue->q);

    if (!next_message) {

	/*
	 * conversation depleted; remove from the list
	 */
	fq->fq_active_conversations--;
	fq->fq_first = previous_queue;
	if (previous_queue)
	    previous_queue->fq_fore = NULL;
	this_queue->fq_next = NULL;
	goto compress;
    }
    if (!previous_queue) {

	/*
	 * conversation not depleted, but no other conversations
	 */
	goto compress;
    }

    /* 
     * sort against other conversations 
     */
    sequence = next_message->fair_sequence;
    next_message = pak_queue_get_head(&previous_queue->q);
    comparison = sequence - next_message->fair_sequence;
    if (comparison <= 0) {
	/* conversation belongs at head of the list, where it already is */
	goto compress;
    }
    /*
     * conversation belongs later in the list, maybe the tail
     */
    fq->fq_first = previous_queue;
    previous_queue->fq_fore = NULL;
    this_queue->fq_next = NULL;
    next_queue = previous_queue->fq_next;
    while (next_queue) {
	next_message = pak_queue_get_head(&next_queue->q);
	comparison = sequence - next_message->fair_sequence;
	if (comparison < 0)
	    break;
	previous_queue = next_queue;
	next_queue = previous_queue->fq_next;
    }
    this_queue->fq_next = next_queue;
    if (next_queue)
	next_queue->fq_fore = this_queue;
    previous_queue->fq_next = this_queue;
    this_queue->fq_fore = previous_queue;

    /*
     * Do any compression on output
     * Store pre-compression size for potential credit
     * The test of "idb->tcp_compress" should be more generic.
     */
  compress:
    datagramsize_old = pak->datagramsize;
    if (idb->tcp_compress) {
	pak = reg_invoke_compress_header(pak->linktype, pak);
    }
    if (idb->comp_db) {
	pak = reg_invoke_encaps_compress(idb->enctype, pak);
    }
    /*
     * Credit traffic shaping statistics for overcharging which
     * results from last-minute knowledge of compression savings.
     * Put check beyond all compression routines to insure all
     * bytes savings are accounted for...
     */
    traffic_shape_credit(pak, datagramsize_old);

    return(pak);
}

/*
 * build_fair_queue_head
 *
 * build and initialize the data structure used by fair queuing
 * make it appear that a previous message was extracted from one of the queues
 */ 
void build_fair_queue_head (hwidbtype * idb,
			    ushort congestion_threshold,
			    ushort number_hashed_queue,
			    ushort number_reserved_queue)
{
    int i;
    fair_queue_head *fq;
    fair_queue *link_queue;
    fair_queue *hashed_queue = NULL;
    fair_queue *reserved_queue = NULL;
    int status;
 
    fq = idb->fair_queue_head;
    if (!fq) {
	/* This will be over written in prioritygroup_command() by
	 * QUEUE_FAIR
	 */
	idb->queue_algorithm = QUEUE_FAIR_DEF;
	fq = malloc_named (sizeof (fair_queue_head), fair_queue_name);
	hashed_queue = malloc_named(number_hashed_queue*sizeof(fair_queue),
				    fair_queue_name);
	if (number_reserved_queue)
	    reserved_queue =
		malloc_named(number_reserved_queue*sizeof(fair_queue),
			     fair_queue_name);
	if (fq && hashed_queue &&
	    (!number_reserved_queue || reserved_queue)) {
	    status = raise_interrupt_level(NETS_DISABLE);
	    idb->oqueue = fair_enqueue;
	    idb->oqueue_dequeue = fair_dequeue;
	    idb->custom_queuenum = 0;
	    bzero (fq, sizeof (fair_queue_head));
	    fq->fq_hash_mask = number_hashed_queue - 1; /* power of 2 */
	    fq->fq_reserved_queue_start =
		number_hashed_queue + 1 + FAIR_LINK_TYPE_HASH;
	    fq->fq_queue_count =
		fq->fq_reserved_queue_start + number_reserved_queue;
	    fq->fq_usage_count = 1;
	    fq->fq_max_enqueue = congestion_threshold;
	    fq->fq_last = fq->fq_link_queue;
	    idb->fair_queue_head = fq;
	    idb->priority_list = congestion_threshold;
	    fq->fq_hashed_queue = hashed_queue;
	    for (i = number_hashed_queue; --i >= 0; hashed_queue++) {
		queue_init(&hashed_queue->q, fq->fq_max_enqueue);
	    }
	    link_queue = fq->fq_link_queue;
	    for (i = 1 + FAIR_LINK_TYPE_HASH; --i >= 0; link_queue++) {
		queue_init(&link_queue->q, fq->fq_max_enqueue);
	    }
	    fq->fq_reserved_queue = reserved_queue;
	    for (i = number_reserved_queue; --i >= 0; reserved_queue++) {
		queue_init(&reserved_queue->q, 0);
	    }
	    reg_add_clear_counters(fair_clear_counters, "fair_clear_counters");
	    reset_interrupt_level(status);
	    reg_invoke_rsvp_fixup_wfq(idb);	/* see if RSVP has a comment */
	} else {

	    /*
	     * if memory allocation fails, use FIFO queuing until
	     * some subsequent configuration attempt finds the memory
	     */
	    idb->oqueue = holdq_enqueue;
	    idb->oqueue_dequeue = holdq_dequeue;
	    idb->priority_list = 0;
	}
    }
}

/*
 * delete_fair_queue
 * Remove a fair queue from an interface.
 */
void delete_fair_queue (hwidbtype *idb)
{
    fair_queue_head *fq;
    int status;

    fq = idb->fair_queue_head;

    if (fq) {
	status = raise_interrupt_level(NETS_DISABLE);
	fq->fq_usage_count--;
	if (0 == fq->fq_usage_count) {
	    holdq_reset(idb);
	    free(fq->fq_hashed_queue);
	    if (fq->fq_reserved_queue)
		free(fq->fq_reserved_queue);
	    free(fq);
	}
	idb->queue_algorithm = QUEUE_FCFS;
	idb->oqueue = holdq_enqueue;
	idb->oqueue_dequeue = holdq_dequeue;
	idb->priority_list = 0;
	idb->fair_queue_head = NULL;
	reset_interrupt_level(status);
    } else if ((QUEUE_FAIR | QUEUE_FAIR_DEF) & idb->queue_algorithm)
	idb->queue_algorithm = QUEUE_FCFS;
}

/*
 * custom_dequeue
 */
static paktype *custom_dequeue (hwidbtype *idb)
{
    paktype *pak = NULL;
    int i;
    pritype *p;
    ushort datagramsize_old;

    /*
     * Handle custom queuing
     * Special case high priority queue for keepalives and such
     * Treat this and all queues below "lowest_custom" as Priority Qs
     */
    p = PRIORITY_ARRAY(idb);
    for (i = 0; i < p->lowest_custom; i++) {
	if (idb->outputq[i].qhead) {
	    pak = pak_dequeue(&(idb->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		idb->output_qcount--;
		idb->counters.output_dequeued[i]++;
		pak->oqnumber = i;
		/*
		 * Do any compression on output
  		 * Store pre-compression size for potential credit
  		 * The test below should be more generic.
		 */
 		datagramsize_old = pak->datagramsize;
 		if (idb->tcp_compress ||
		     ((pak->nif_output != NULL) &&
 		      (pak->nif_output->compression_negotiated_enabled))) {
		    pak = reg_invoke_compress_header(pak->linktype, pak);
		}
		if (idb->comp_db) {
		    pak = reg_invoke_encaps_compress(idb->enctype, pak);
		}
 		/*
 		 * Credit traffic shaping statistics for overcharging which
 		 * results from last-minute knowledge of compression savings.
 		 * Put check beyond all compression routines to insure all
 		 * bytes savings are accounted for...
 		 */
 		traffic_shape_credit(pak, datagramsize_old);

		return(pak);
	    }
	}
    }

    /*
     * Now run the custom queues looking to see what should be transmitted.
     */
    i = idb->custom_queuenum;
    do {
	if (idb->outputq[i].qhead) {
	    pak = pak_dequeue(&(idb->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		idb->output_qcount--;
		idb->counters.output_dequeued[i]++;
		pak->oqnumber = i;
	    }
	    break;
	}
	i = PRIORITY_QUEUE_INC(i);
    } while (i != idb->custom_queuenum);

    if (!pak) {
	idb->custqremain[i] = custom_qbytecnt(idb,i);
	return(NULL);
    }

    /*
     * Decrement byte count for current queue.  If we've transmitted too
     * much from the current queue, reset byte count and move queue
     * pointer to next queue.
     */
    if (idb->custqremain[i] > pak->datagramsize) {
	idb->custqremain[i] -= pak->datagramsize;
	if (custom_debug) {
	    buginf("\nPQ: (Pk size: %d) Q # was %d now %d",
		   pak->datagramsize, idb->custom_queuenum, i);
	}
    } else {
	idb->custqremain[i] = custom_qbytecnt(idb, i);
	i = PRIORITY_QUEUE_INC(i);
	if (custom_debug) {
	    buginf("\nPQ: (Pk size: %d) Inc. Q # to %d", pak->datagramsize, i);
	}
    }
    /*
     * Update pointer to queue array, reset byte count for former current
     * queue.
     */
    if (idb->custom_queuenum != i) {
	idb->custqremain[idb->custom_queuenum] =
	    custom_qbytecnt(idb, idb->custom_queuenum);
	idb->custom_queuenum = i;
    }

    /*
     * Do any compression on output
     * Store pre-compression size for potential credit
     * The test below should be more generic.
     */
    datagramsize_old = pak->datagramsize;
    if (idb->tcp_compress ||
 	 ((pak->nif_output != NULL) &&
 	  (pak->nif_output->compression_negotiated_enabled))) {
	pak = reg_invoke_compress_header(pak->linktype, pak);
    }
    if (idb->comp_db) {
      pak = reg_invoke_encaps_compress(idb->enctype, pak);
    }
    /*
     * Credit traffic shaping statistics for overcharging which
     * results from last-minute knowledge of compression savings.
     * Put check beyond all compression routines to insure all
     * bytes savings are accounted for...
     */    
    traffic_shape_credit(pak, datagramsize_old);
 
    return(pak);
}

/*
 * priority_qsize
 * Return desired size of an interface's output queue
 */

int priority_qsize (hwidbtype *idb, int index)
{
    pritype *p;

    if (idb->priority_list) {
	p = PRIORITY_ARRAY(idb);
	if (p)
	    return(p->qsize[index]);
	if (idb->red_parameters)
	    return 0;
	if (idb->custom_queuenum)
	    return(CUSTOM_QUEUE_SIZE);
	else
	    return(PRIORITY_QLIMIT*(index+1));
    }
    return(idb->output_defqcount[index]);
}

/*
 * custom_qbytecnt
 * Return desired packet byte count for an interface's custom output
 * queue.
 */

ulong custom_qbytecnt (hwidbtype *idb, int index)
{
    pritype *p;

    p = PRIORITY_ARRAY(idb);
    return(p ? p->bytecount[index] : CUSTOM_QUEUE_PACKET_SIZE);
}

static struct priority_prioritylist_ {
    uchar priority;
    char *name;
} priority_prioritylist[PRIORITY_MAXTYPE] = {
    {PRIORITY_HIGH,   "high"},
    {PRIORITY_MEDIUM, "medium"},
    {PRIORITY_NORMAL, "normal"},
    {PRIORITY_LOW,    "low"}
};

/*
 * prioritylist_command
 * Set up an item in a priority list
 */

void prioritylist_command (parseinfo *csb)
{
    int number, pri_cmd;
    pritype *p;
    plisttype *plist;
    boolean custom;
    plist_entrytype pl_type;
    
    custom = (QUEUE_CUSTOM == GETOBJ(int,16));
    if (csb->nvgen) {
	for (number = 0; number < PRIORITY_MAXLIST; number++) {
	    p = custom ? cust_parray[number] : parray[number];
	    if (!p)		/* no queue list defined, go to next queue  */
		continue;
	    /*
	     * Parse down plisttype elements to preserve relative
	     * element priority while writing configuration.
	     * Preservation of element order allows customer to fine
	     * tune queuing strategy, as output of priority_classify()
	     * is sensitive to element order.
	     *
	     * Currently only PRICMD_PROTOCOL and PRICMD_INTERFACE
	     * -type commands need to preserve order.  The other
	     * PRICMD_* commands operate on pritype_ (and not
	     * individual plist_entrytype(s)) and are not sensitive to
	     * order.
	     */
	    for (plist = p->plist; plist; plist = plist->next) {
		  pl_type = pl_get_entrytype(plist);
		  pri_cmd = pl_map_pltype2pricmd(pl_type);
		  reg_invoke_priority_parse(pri_cmd, csb, number, p, plist);
	    } /* endFor plist */

	    /*
	     * Write out commands that operate on pritype_ after finishing
	     * with order sensitive commands
	     */
	    for (pri_cmd = PRICMD_START_SUBFN;
		 pri_cmd <= PRICMD_FINISH_SUBFN; pri_cmd++) {
		    if ((pri_cmd == PRICMD_PROTOCOL) ||
			(pri_cmd == PRICMD_INTERFACE))
			continue;
		    reg_invoke_priority_parse(pri_cmd, csb, number, p, NULL);
	    } /* endFor pri_cmd */

	} /* endFor number */
    } else {
	number = GETOBJ(int,1);
	p = custom ? cust_parray[number] : parray[number];
	/* Guard against unallocated priority lists */
	if (p == NULL) {
	    p = priority_allocate_list(custom, number);
	    if (p == NULL) {
		return;
	    }
	}
	reg_invoke_priority_parse(GETOBJ(int,2), csb, number, p, NULL);
    }
}

/*
 * prilist_lowest_custom
 * Set the lowest queue number to be treated as custom for this list
 *  (below that are "priority")
 */

static void prilist_lowest_custom (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    if (csb->nvgen) {
	nv_write(p->lowest_custom != 1,
		 "%s %d lowest-custom %s", csb->nv_command, listnum,
		 queue2name(p->lowest_custom, p->custom));
	return;
    }

    if (!csb->sense) {
	p->lowest_custom = 1;
    } else {
	p->lowest_custom = GETOBJ(int,4);
    }
}

/*
 * prilist_default
 * Set the default priority for this list
 */

static void prilist_default (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    if (csb->nvgen) {
	nv_write(p->pdefault != (p->custom ? 1 : PRIORITY_NORMAL),
		 "%s %d default %s", csb->nv_command, listnum,
		 queue2name(p->pdefault, p->custom));
	return;
    }

    if (!csb->sense) {
	p->pdefault = (p->custom ? 1 : PRIORITY_NORMAL);
    } else {
	p->pdefault = GETOBJ(int,4);
    }
}

/*
 * prilist_interface
 * Set the priority for traffic received on a particular interface
 */

static void prilist_interface (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    plisttype *pred;
    hwidbtype *interface;
    int priority;


    if (csb->nvgen) {
	  /*
	   * During nvgen, only process 'current passed in element'.
	   * Caller is responsible for walking plist chain to insure
	   * relative order of different 'plist' types.  Since parsing
	   * order is dependent on nvgen order, no change is required
	   * to parsing code logic.
	   */
	  if (plist->interface) {
	    nv_write(TRUE,
		     "%s %d interface %s %s", csb->nv_command, listnum,
		     plist->interface->hw_namestring,
		     queue2name(plist->priority, p->custom));
	  }
	  return;
    }

    interface = hwidb_or_null(GETOBJ(idb,1));
    priority = GETOBJ(int,4);

    /*
     * Search the priority array for an item that matches on interface
     * and priority. If we find an item, possibly delete it, then return.
     * "pred" is the pointer to the last item in the list.
     */
    pred = NULL;
    for (plist = p->plist; plist;
	 pred = plist, plist = plist->next) {
	if (plist->interface && (plist->interface == interface)) {
	    if (!csb->sense) {
		if (pred) {
		    pred->next = plist->next;
		} else {
		    p->plist = plist->next;
		}
		free(plist);
	    } else {
		if (plist->priority != priority) {
		    plist->priority = priority;
		}
	    }
	    return;
	}
    }

    /*
     * Item not in list.  Create it at end of list.
     */
    if (!csb->sense) {
	return;
    }
    plist = malloc_named(sizeof(plisttype), priority_item_name);
    if (!plist) {
	printf(nomemory);
	return;
    }
    plist->interface = interface;
    plist->priority = priority;
    if (pred) {
	pred->next = plist;
    } else {
	p->plist = plist;
    }
}

/*
 * prilist_protocol
 * Handle the priority-list protocol keyword
 */

static void prilist_protocol (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    plisttype *next, *pred, ptmp;

    if (csb->nvgen) {
	  /*
	   * During nvgen, only process 'current passed in element'.
	   * Caller is responsible for walking plist chain to insure
	   * relative order of different 'plist' types.  Since parsing
	   * order is dependent on nvgen order, no change is required
	   * to parsing code logic.
	   */
	  if (plist->linkcode) {
	    nv_write(TRUE, "%s %d protocol %s %s", csb->nv_command, listnum,
		     print_protocol(plist->linkcode),
		     queue2name(plist->priority, p->custom));
	    nv_add(plist->gt_size, " gt %d", plist->gt_size);
	    nv_add(plist->lt_size, " lt %d", plist->lt_size);
	    nv_add(plist->testlist, " list %d", plist->testlist);
	    if (plist->tcp_port) {
		nv_add(TRUE, " tcp ");
		reg_invoke_nv_add_tcp_port(plist->tcp_port);
	    }
	    if (plist->udp_port) {
		nv_add(TRUE, " udp ");
		reg_invoke_nv_add_udp_port(plist->udp_port);
	    }
	    nv_add(plist->ip_fragments, " fragments");
	    reg_invoke_priority_protocol_nv_add(plist->linkcode, plist);
	}
	return;
    }

    /*
     * Register the one-minute priority list change checker
     */
    if (priority_callbacks_registered == FALSE) {
	priority_callbacks_registered = TRUE;
	reg_add_onemin(priority_perform_change_callbacks,
		       "priority_perform_change_callbacks");
    }

    /*
     * Get the protocol string.
     * If no protocol string and a "no" was specified, delete the entire list.
     */
    memset(&ptmp, 0, sizeof(plisttype));
    ptmp.linkcode = GETOBJ(int,3);
    if ((ptmp.linkcode == LINK_ILLEGAL) && !csb->sense) {
	plist = p->plist;
	p->plist = NULL;
	while (plist) {
	    next = plist->next;
	    free(plist);
	    plist = next;
	}
	priority_mark_list_changed(GETOBJ(int,1), PRICMD_PROTOCOL);
	reg_invoke_priority_list_changed(GETOBJ(int,1), ptmp.linkcode, csb->sense);
	return;
    }

    /*
     * Get priority type
     */
    ptmp.priority = GETOBJ(int,4);

    /*
     * Parse further conditions:
     *  - "list <list-number>"
     *  - "gt <size>"
     *  - "lt <size>"
     *  - "tcp" <port>"
     *  - "udp"  <port>"
     *  - "fragments"
     */
    switch (GETOBJ(int,5)) {
    case PRIORITY_LIST:
	ptmp.testlist = GETOBJ(int,6);

	/*
	 * Verify protocol and list range.
	 */
	if (ptmp.testlist &&
	    !reg_invoke_valid_access_list(ptmp.linkcode, ptmp.testlist)) {
	    printf("bad list number");
	    return;
	}
	break;
    case PRIORITY_LT:
	ptmp.lt_size = GETOBJ(int,6);
	break;
    case PRIORITY_GT:
	ptmp.gt_size = GETOBJ(int,6);
	break;
    case PRIORITY_TCP:
	if (ptmp.linkcode != LINK_IP) {
	    printf("tcp arg valid only for IP");
	    return;
	}
	ptmp.tcp_port = GETOBJ(int,6);
	break;
    case PRIORITY_UDP:
	if (ptmp.linkcode != LINK_IP) {
	    printf("udp arg valid only for IP");
	    return;
	}
	ptmp.udp_port = GETOBJ(int,6);
	break;
    case PRIORITY_FRAGS:
	ptmp.ip_fragments = TRUE;
	break;
    case PRIORITY_UNSPEC:
	/*
	 * No further conditions.
	 */
	break;
    case PRIORITY_ADDRESS:
    default:
	if (!reg_invoke_priority_protocol_parse(ptmp.linkcode, csb, &ptmp))
	    return;
	break;
    }

    /*
     * Search the priority array for an item that matches on link and priority.
     * If we find an item, update or delete it and return.
     * "pred" is the pointer to the last item in the list.
     */
    pred = NULL;
    for (plist = p->plist; plist;
	 pred = plist, plist = plist->next) {
	if (plist->linkcode == ptmp.linkcode) {
	    if (plist->testlist != ptmp.testlist) {
		continue;
	    }
	    if ((plist->gt_size != ptmp.gt_size) ||
		(plist->lt_size != ptmp.lt_size)) {
		continue;
	    }
	    if ((plist->tcp_port != ptmp.tcp_port) ||
		(plist->udp_port != ptmp.udp_port)) {
		continue;
	    }
	    if (plist->ip_fragments != ptmp.ip_fragments)
		continue;
	    if (!reg_invoke_priority_protocol_compare(ptmp.linkcode, plist,
						      &ptmp))
		continue;

	    ptmp.next = plist->next;
	    *plist = ptmp;		/* structure copy */
	    ptmp.next = NULL;		/* just in case */
	    if (!csb->sense) {
		if (pred) {
		    pred->next = plist->next;
		} else {
		    p->plist = plist->next;
		}
		free(plist);
	    }
	    priority_mark_list_changed(GETOBJ(int,1), PRICMD_PROTOCOL);
	    reg_invoke_priority_list_changed(GETOBJ(int,1), ptmp.linkcode, csb->sense);
	    return;
	}
    }

    /*
     * Item not in list.  Create it at end of list.
     */
    if (!csb->sense) {
	return;
    }
    plist = malloc_named(sizeof(plisttype), priority_item_name);
    if (!plist) {
	printf(nomemory);
	return;
    }
    ptmp.next = plist->next;
    *plist = ptmp;			/* structure copy */
    if (pred) {
	pred->next = plist;
    } else {
	p->plist = plist;
    }
    reg_invoke_priority_list_changed(GETOBJ(int,1), ptmp.linkcode, csb->sense);
    priority_mark_list_changed(GETOBJ(int,1), PRICMD_PROTOCOL);
}

/*
 * prilist_true
 * Compare two priority list items and always return true.
 */

static boolean prilist_true (plisttype *a, plisttype *b)
{
    return(TRUE);
}

/*
 * d e f a u l t _ q u e u i n g _ a l l o w e d
 *
 * registered as default case.  Default case is that queuing
 * configuration will be allowed...
 */
static boolean default_encaps_queuing_allowed (hwidbtype *idb, ulong queue_type) {
      return(TRUE);
}      

/*
 * priority_holdq_reset
 * A priority list queue limit has been changed.
 * Make interface queue limits match the desired queue limits.
 */

static void priority_holdq_reset (
    int list_no)
{
    hwidbtype *idb;
    int status;

    reg_invoke_queue_reset(list_no);
    status = raise_interrupt_level(NETS_DISABLE);
    FOR_ALL_HWIDBS(idb) {
        /*
         * Resetting the holdq ONLY for interfaces configured
         * by the priority list, then call the hardware reset routine
         * to possibly adjust hardware output buffering to match
         * the priority queueing status.
         * NOTE: holdq_reset will not reset the interface when
         *       the argument passed to it is not NULL.
         */
        if (idb->priority_list == list_no) {
            holdq_reset( idb);
            (*idb->reset)(idb);
            if (idb->counters.resets > 0)
                idb->counters.resets--;
        }
    }
    reset_interrupt_level(status);
}


/*
 * prilist_qlimit
 * Handle the queue-limit keyword
 */

static void prilist_qlimit (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    int j;
    
    if (p->custom)
	return;
    if (csb->nvgen) {
	nv_write(((p->qsize[PRIORITY_HIGH] !=
		   p->qsize_def[PRIORITY_HIGH]) ||
		  (p->qsize[PRIORITY_MEDIUM] !=
		   p->qsize_def[PRIORITY_MEDIUM]) ||
		  (p->qsize[PRIORITY_NORMAL] !=
		   p->qsize_def[PRIORITY_NORMAL]) ||
		  (p->qsize[PRIORITY_LOW] !=
		   p->qsize_def[PRIORITY_LOW])),
		 "%s %d queue-limit %d %d %d %d", csb->nv_command, listnum,
		 p->qsize[PRIORITY_HIGH],
		 p->qsize[PRIORITY_MEDIUM],
		 p->qsize[PRIORITY_NORMAL],
		 p->qsize[PRIORITY_LOW]);
	return;
    }

    if (csb->sense) {
	if ((p->qsize[PRIORITY_HIGH] == GETOBJ(int,3)) &&
	    (p->qsize[PRIORITY_MEDIUM] == GETOBJ(int,4)) &&
	    (p->qsize[PRIORITY_NORMAL] == GETOBJ(int,5)) &&
	    (p->qsize[PRIORITY_LOW] == GETOBJ(int,6))) {
	    /* Nothing changed */
	    return;
	}
	p->qsize[PRIORITY_HIGH] = GETOBJ(int,3);
	p->qsize[PRIORITY_MEDIUM] = GETOBJ(int,4);
	p->qsize[PRIORITY_NORMAL] = GETOBJ(int,5);
	p->qsize[PRIORITY_LOW] = GETOBJ(int,6);
    } else {
	boolean nothing_changed = TRUE;

	for (j = 0; j < PRIORITY_MAXTYPE; j++) {
	    if (p->qsize[j] != p->qsize_def[j]) {
		p->qsize[j] = p->qsize_def[j];
		nothing_changed = FALSE;
	    }
	}
	if (nothing_changed) {
	    return;
	}
    }
    priority_holdq_reset( GETOBJ(int,1));
}

static void custom_qcount (parseinfo *csb, int listnum, pritype *p, plisttype *plist)
{
    boolean parsed_bcount, parsed_qlen, custom;
    int qnum;

    custom = (QUEUE_CUSTOM == GETOBJ(int,16));
    if (!custom)
	return;
    if (csb->nvgen) {
	for (qnum = 0; qnum < PRIORITY_MAXLIST; qnum++) {
	    if ((p->bytecount[qnum] != CUSTOM_QUEUE_PACKET_SIZE) ||
		(p->qsize[qnum] != p->qsize_def[qnum])) {
		nv_write(TRUE, "%s %d queue %d", csb->nv_command, listnum,
			 qnum);
		nv_add((p->bytecount[qnum] != CUSTOM_QUEUE_PACKET_SIZE),
		       " byte-count %u", p->bytecount[qnum]);
		nv_add((p->qsize[qnum] != p->qsize_def[qnum]),
		       " limit %d", p->qsize[qnum]);
	    }
	}
	return;
    }
    qnum = GETOBJ(int,3);
    parsed_bcount = GETOBJ(int,4);
    parsed_qlen = GETOBJ(int,6);
    if (parsed_bcount) {
	if (csb->sense) {
	    p->bytecount[qnum] = GETOBJ(int,5);
	} else {
	    p->bytecount[qnum] = CUSTOM_QUEUE_PACKET_SIZE;
	}
    }
    if (parsed_qlen) {
	if (!csb->sense) {
	    SETOBJ(int,7) = p->qsize_def[qnum];
	}
	if (p->qsize[qnum] != GETOBJ(int,7)) {
	    p->qsize[qnum] = GETOBJ(int,7);
            priority_holdq_reset( GETOBJ(int,1));
	}
    }
}

/*
 * prioritygroup_command
 *
 * Set up a priority list, custom queue list, or weighted fair queue
 * for an interface.
 *
 * Use of these are mutually exclusive
 */

void prioritygroup_command (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *idb;
    int i;
    fair_queue_head *fq;
    red_parameters *redp;
    int status;
    pritype *p;	
    ushort default_queue_mode;
    ushort interface_queue_mode;

    idb = csb->interface->hwptr;
    interface_queue_mode = priority_type(idb);
    if (fair_queue_bandwidth(idb) && fair_queue_supported(idb) &&
	queuing_can_be_modifed(idb)) {
	default_queue_mode = QUEUE_FAIR_DEF;
    } else {
	default_queue_mode = QUEUE_FCFS;
    }

    fq = idb->fair_queue_head;
    if (csb->nvgen) {			/* generate commands for queueing? */
	switch (csb->which) {
	default: return;
	case QUEUE_FAIR:
	    /*
	     * Must generate config for both FCFS and FAIR interfaces here
	     */
	    if ((QUEUE_FAIR_DEF | QUEUE_FAIR) & interface_queue_mode) {
		if (QUEUE_FAIR_DEF == default_queue_mode &&
		    idb->priority_list == FAIR_QUEUE_DEFAULT_QDEPTH &&
		    fq->fq_hash_mask == FAIR_QUEUE_HASH_QUEUES_DEFAULT - 1 &&
		    fq->fq_queue_count == 
		    fq->fq_reserved_queue_start + FAIR_RSVP_QUEUES_DEFAULT)

		    /* default configuration, do not save the command */
		    return;
		nv_write(TRUE, "%s %d %d %d", csb->nv_command, 
			 idb->priority_list,
			 fq->fq_hash_mask + 1,
			 fq->fq_queue_count - fq->fq_reserved_queue_start);
		return;
	    } else if (QUEUE_FCFS == interface_queue_mode) {
		if (QUEUE_FCFS == default_queue_mode) {
		    return;
		} else {
		    /* so the change of default must turn it off... */
		    nv_write(TRUE, "no %s", csb->nv_command);
		    return;
		}		
	    }
	    return;
	case QUEUE_RED:
	case QUEUE_CUSTOM:
	case QUEUE_PRIORITY:
	    if (csb->which != interface_queue_mode)
	        return;
	    break;
	}
	nv_write(TRUE, "%s %d", csb->nv_command, idb->priority_list);
	return;
    }
    
    /*
     * If "no priority-group" command and none defined, return quietly, else
     * set the default queue back to its default length. If the value of 
     * priority_list changes reset the interface to allow the tx ring to 
     * be setup properly.
     *
     * if "no custom" or "no fair" command and none defined,
     * out-a-here. Otherwise, re-initialize the interface so that the
     * queues are history.
     */
    if (!csb->sense) {
	if (csb->which != priority_type(idb)) {
	    switch (csb->which) {
	    default:	
		return;
	    case QUEUE_FAIR:
		if (priority_type(idb) != QUEUE_FAIR_DEF)
		    return;
	    }
	}
	status = raise_interrupt_level(NETS_DISABLE);
	holdq_reset(idb);
	idb->priority_list = 0;
	idb->queue_algorithm = default_queue_mode;
	switch (csb->which) {
	default:
	    break;
	case QUEUE_FAIR:
	    delete_fair_queue(idb);
	    break;
	case QUEUE_RED:
	    if ((redp = idb->red_parameters)) {
		idb->red_parameters = NULL;
	        idb->outputq[PRIORITY_NORMAL].maximum = redp->saved_threshold;
	        idb->output_defqcount[PRIORITY_NORMAL] = redp->saved_threshold;
	        free(redp);
	    }
	case QUEUE_CUSTOM:
	    idb->custom_queuenum = 0;
	case QUEUE_PRIORITY:
	    set_default_queueing(idb);
	    break;
	}
	
        idb->outputq[DEFAULT_QUEUE(idb)].maximum = 
 	     hwidb_holdqueue_size_or_default(idb);
        reg_invoke_interface_cfg_modified(idb);
	reset_interrupt_level(status);
    } else {
	/*
	 * Traffic shaping over link layer may not allow fancy
	 * queueing.  If not allowed, do nothing gracefully...
	 * Registered function must print out any appropriate messages
	 * to inform user if queuing command is not allowed for this
	 * interface/encap combination.
	 */
	if (!reg_invoke_encaps_queuing_allowed(idb->enctype, idb, csb->which))
	    return;
	/*
	 * Until ppp code is re-worked to interoperate with WFQ,
	 * disallow WFQ if multilink is enabled. CSCdi68512
	 */
	if (is_mlp_enabled(idb)) {
	       printf("Must disable ppp multilink configuration first.");
	       return;
	}
	if (!priority_ip_routecache(idb)) {
	       printf("Must disable IP %s Route Cache first.",
		      (idb->ip_routecache & IP_FAST_ROUTECACHE)? "Fast":
		      (idb->ip_routecache & IP_CBUS_ROUTECACHE)? "CBUS":
		      (idb->ip_routecache & IP_SSE_ROUTECACHE)? "SSE":
		      (idb->ip_routecache & IP_FLOW_ROUTECACHE)? "Flow":
		      (idb->ip_routecache & IP_OPTIMUM_ROUTECACHE)? "Optimal":
		      (idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)? 
		      "Distributed":
		      ""
		      );
	       return;
	  }
	if (idb->priority_list) {
	       switch (csb->which) {
	         case QUEUE_FAIR:
		      if (fq) 
			   ;			/* but it IS fair queueing */
		      else if (idb->custom_queuenum) {
			     printf("Must remove custom-queue configuration first.");
			     return;
		      } else if (QUEUE_RED == interface_queue_mode) {
			     printf("Must remove RED configuration first.");
			     return;
		      } else {
			     printf("Must remove priority-group configuration first.");
			     return;
		        }
		      break;
	         case QUEUE_RED:
		 case QUEUE_CUSTOM:
		 case QUEUE_PRIORITY:
		      if (QUEUE_RED != csb->which && QUEUE_RED == interface_queue_mode) {
			     printf("Must remove RED configuration first.");
			     return;
			}
		      if (QUEUE_PRIORITY != csb->which && QUEUE_PRIORITY == interface_queue_mode) {
			     printf("Must remove priority-group configuration first.");
			     return;
			}
		      if (QUEUE_CUSTOM != csb->which && QUEUE_CUSTOM == interface_queue_mode) {
			     printf("Must remove custom-queue configuration first.");
			     return;
			}
		      break;
		 }
	  }
	if (QUEUE_FAIR == csb->which) {

	    /* validity check on number of hashed queues */
	    if (GETOBJ(int,2) != (GETOBJ(int,2) & -GETOBJ(int,2))) {
		/* the number is not a power of 2 */
		printf("Number of dynamic queues must be a power of 2"
		       " (16, 32, 64, 128, 256, 512, 1024)");
	        return;
	    }
	} else if (idb->priority_list == GETOBJ(int,1)) {
	    /* Nothing changed */
	    return;
	}
	
	/* if it defaulted to fair queueing or fair queueing was otherwise 
	 * enabled, pitch the old data structure before creating a new one.
	 */
	status = raise_interrupt_level(NETS_DISABLE);

	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (swidb->traffic_shape) {
		swidb->traffic_shape->shape_reset(swidb->traffic_shape);
	    }
	}
	holdq_reset(idb);
	delete_fair_queue(idb);
	idb->priority_list = GETOBJ(int,1);
	idb->queue_algorithm = csb->which;
	switch (csb->which) {
	case QUEUE_FAIR:
	    /*
	     * GETOBJ(int,2) is the number of hashed queues;
	     * GETOBJ(int,3) is the number of reserved queues
	     */
	    build_fair_queue_head(idb, idb->priority_list,
				  GETOBJ(int,2), GETOBJ(int,3));
	    idb->queue_algorithm = QUEUE_FAIR;
	    if (!idb->fair_queue_head) {
		printf("allocation of fair_queue_head data structure failed");
	    }
	    break;
	case QUEUE_CUSTOM:
	    idb->oqueue = priority_enqueue;
	    idb->oqueue_dequeue = custom_dequeue;
	    idb->custom_queuenum = 1;
	    for (i = 0; i < PRIORITY_MAXLIST; i++) {
		idb->custqremain[i] = custom_qbytecnt(idb, i);
	    }
	    /* Guard against unallocated priority lists */
	    p = cust_parray[idb->priority_list];
	    if (p == NULL) {
		p = priority_allocate_list(TRUE, idb->priority_list);
	    }
	    break;
	case QUEUE_PRIORITY:
	    idb->oqueue = priority_enqueue;
	    idb->oqueue_dequeue = priority_dequeue;
	    /* Guard against unallocated priority lists */
	    p = parray[idb->priority_list];
	    if (p == NULL) {
		p = priority_allocate_list(FALSE, idb->priority_list);
	    }
	    break;
	case QUEUE_RED:
	    redp = idb->red_parameters;
	    if (!redp) {
		idb->red_parameters = redp = malloc(sizeof(*redp));
		if (!redp)
		    break;
		redp->saved_threshold = idb->outputq[PRIORITY_NORMAL].maximum;
		idb->outputq[PRIORITY_NORMAL].maximum = 0;
	    }
	    redp->red_shift = GETOBJ(int,1);
	    red_set_precedence_levels(idb,
				      redp->saved_threshold/2);
	    redp->mean_queue_depth = 0;
	    idb->oqueue = red_enqueue;
	    idb->oqueue_dequeue = red_dequeue;
	    reg_add_clear_counters(red_clear_counters, "red_clear_counters");
	    break;
	}
	reset_interrupt_level(status);
    }
    reg_invoke_interface_cfg_modified(idb);
    holdq_reset(idb);
    
    /*
     * Fix up fast switching for various routed protocols
     */
    reg_invoke_fast_setup(idb);
}

/*
 * queue2name
 * Given a queue # and whether or not queue # is a custom queue,
 * return queue name
 */

char *queue2name (int priority, boolean custom)
{
    static char namenum[8];

    if (custom) {
	sprintf(namenum, "%d", priority);
	return(namenum);
    }
    return(priority2name(priority));
}

/*
 * idb_queue2name
 *
 * Given an idb and a queue number, return queue name.
 */

char *idb_queue2name (
    hwidbtype *output,
    int queue)
{
    static char namenum[8];

    if (output->custom_queuenum) {
	sprintf(namenum, "%d", queue);
	return(namenum);
    }
    return(priority2name(queue));
}

/*
 * priority2name
 * Given a priority, return keyword
 */

char *priority2name (int priority)
{
    int i;

    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	if (priority_prioritylist[i].priority == priority) {
	    return(priority_prioritylist[i].name);
	}
    }
    return(NULL);
}

/*
 * show_queue_counts
 *
 * Display interface specific counter information concerning output queues.
 */
void show_queue_counts (parseinfo *csb)
{
    hwidbtype *idb;
    int i;
    boolean custom;
    idb_si_counters_t	*sic;

    idb = GETOBJ(idb, 1)->hwptr;
    printf("\nInterface %s queueing strategy: ", idb->hw_namestring);

    if ((QUEUE_FAIR | QUEUE_FAIR_DEF) & priority_type(idb)) {
	printf("%s", "fair");
	show_fair_queue_command (idb);
	return;
    }
    if (priority_type(idb) == QUEUE_RED) {
	printf("%s", "RED");
	holdq_display(idb);
	return;
    }

    if (!idb->priority_list) {
	printf("none");
	return;
    }
    custom = idb->custom_queuenum;
    sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
    printf("%s", custom ? "custom" : "priority");
    printf("\n\nOutput queue utilization (queue/count)\n\t");
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	printf("%s/%d ", queue2name(i, custom),
	       idb->counters.output_dequeued[i] - sic->output_dequeued[i]);
	if (!custom && (i == PRIORITY_LOW)) {
	    break;
	}
	if (i == ((PRIORITY_MAXTYPE - 1) / 2)) {
	    printf("\n\t");
	}
    }
}


/*
 * show_queueing
 * Show the priority lists and associated information
 */

static const char queue_header[] = "\nList   Queue  Args";

void show_queueing (int type, int list_no)
{
    boolean custom = type & QUEUE_CUSTOM;        /* flag */
    int i, j, qnum;
    plisttype *plist;
    pritype *p;
    boolean shown_header = FALSE;

    for (i = 0; i < PRIORITY_MAXLIST; i++) {
	if (list_no != (-1)) {
	    if (list_no != i) {
		continue;
	    }
	}
	switch (type) {
	case QUEUE_FAIR:
	case QUEUE_FAIR_DEF:    
	    p = fair_parray[i];
	    break;
	case QUEUE_CUSTOM:
	    p = cust_parray[i];
	    break;
	case QUEUE_RED:
	case QUEUE_PRIORITY:
	    p = parray[i];
	    break;
	default:
	    /* how would this happen? */
	    continue;
	}
	if (!p) {
	    continue;
	}
	mem_lock(p);

	if (!shown_header) {
	    shown_header = TRUE;
	    automore_header(queue_header);
	}

	if (custom && (p->lowest_custom != 1)) {
	    printf("\n%5d  %6s lowest custom queue", i, 
		   queue2name(p->lowest_custom, custom));
	}
	if (p->pdefault != (custom ? 1 : PRIORITY_NORMAL)) {
	    printf("\n%5d  %6s default", i, queue2name(p->pdefault, custom));
	}
	for (j = 0; (plist = queuepick(&p->plist, j)); j++) {
	    if (!validmem(plist)) {
		break;
	    }
	    mem_lock(plist);
	    if (reg_invoke_priority_show(plist, custom, i)) {
		free(plist);
		continue;
	    }
	    if (plist->interface) {
		printf("\n%5d  %6s interface %11s", i,
		       queue2name(plist->priority, custom),
		       plist->interface->hw_namestring);
		free(plist);
		continue;
	    }
	    printf("\n%5d  %6s protocol %11s", i,
		   queue2name(plist->priority, custom),
		   print_linktype(plist->linkcode));
	    if (plist->gt_size)
		printf(" gt %d", plist->gt_size);
	    else if (plist->lt_size)
		printf(" lt %d", plist->lt_size);
	    else if (plist->tcp_port) {
		printf(" tcp port ");
		reg_invoke_printf_tcp_port(plist->tcp_port);
	    } else if (plist->udp_port) {
		printf(" udp port ");
		reg_invoke_printf_udp_port(plist->udp_port);
	    } else if (plist->testlist)
		printf(" list %d", plist->testlist);
	    else if (plist->ip_fragments)
		printf(" fragments");
	    free(plist);
	}
	for (qnum = 0; qnum < PRIORITY_MAXLIST; qnum++) {
	    if ((custom && (p->bytecount[qnum] != CUSTOM_QUEUE_PACKET_SIZE)) ||
		(p->qsize[qnum] != p->qsize_def[qnum])) {
		printf("\n%5d  %6s ", i, queue2name(qnum, custom));
		if (custom &&
		    (p->bytecount[qnum] != CUSTOM_QUEUE_PACKET_SIZE)) {
		    printf("byte-count %u ", p->bytecount[qnum]);
		}
		if (p->qsize[qnum] != p->qsize_def[qnum]) {
		    printf("limit %d", p->qsize[qnum]);
		}
	    }
	    if (!custom && (qnum == PRIORITY_LOW)) {
		break;
	    }
	}
	free(p);
    }
}


/*
 * show_queueing_command
 */

static const char fair_queue_hdr[] =
           "\n  Interface    Discard       Dynamic        Reserved\n"
             "               threshold     queue count    queue count\n";

void show_queueing_command (parseinfo *csb)
{
    int type;
    hwidbtype *idb;
    fair_queue_head *fq;

    type = GETOBJ(int,1);
    automore_enable(NULL);

    if ((QUEUE_FAIR | QUEUE_FAIR_DEF) & type) {
        boolean print_title = TRUE;
	printf("\nCurrent fair queue configuration:");
	FOR_ALL_HWIDBS(idb) {
	    if (priority_type(idb) & (QUEUE_FAIR | QUEUE_FAIR_DEF)) {
		if (print_title) {
		    print_title = FALSE;
		    automore_header(fair_queue_hdr);
		}
		fq = idb->fair_queue_head;
		printf("  %13s", idb->hw_namestring);
		printf("%14d", 
		       fq->fq_max_enqueue);
		printf("%15d",
		       fq->fq_hash_mask + 1);
		printf("%5d\n",
		       fq->fq_queue_count - fq->fq_reserved_queue_start);
	    }
	}
    }

    if (type & QUEUE_PRIORITY) {
	printf("\nCurrent priority queue configuration:");
	show_queueing(QUEUE_PRIORITY, -1);
    }

    if (type & QUEUE_CUSTOM) {
	printf("\nCurrent custom queue configuration:");
	show_queueing(QUEUE_CUSTOM, -1);
    }
    if (type & QUEUE_RED) {
	printf("\nCurrent RED queue configuration:");
        FOR_ALL_HWIDBS(idb) {
            if (priority_type(idb) == QUEUE_RED) {
                printf("\n  Interface %s", idb->hw_namestring);
                printf("  exponential weighting constant is %d", 
                       idb->red_parameters->red_shift);
              }
        }
    }
    automore_disable();
}

/*
 * Support for priority list callbacks. Will callback functions defined at
 * runtime if an priority list is changed. Will be checked once a minute by
 * net_background.
 */


/*
 * priority_create_callback
 *
 * Create an priority callback function
 */
boolean priority_create_callback (priority_callbackfunction callback_function,
				  ulong low, ulong high)
{
    priority_callback *new;

    if ((new = malloc_named(sizeof(priority_callback),
			    priority_callback_name)) == NULL)
        return(FALSE);
    new->callback_function = callback_function;
    new->low = low;
    new->high = high;
    new->next = priority_callbacks;
    priority_callbacks = new;
    return(TRUE);
}

/*
 * priority_destroy_callback
 *
 * Destroy an priority callback function
 */
boolean priority_destroy_callback (void *func)
{
    priority_callback *prev, *current;

    for (prev = (priority_callback *) &priority_callbacks,
	 current = prev->next;
         current != NULL; prev = current, current = current->next) {
        if ((current->callback_function == func)) {
            prev->next = current->next;
            free(current);
            return(TRUE);
        }
    }
    return(FALSE);
}

/*
 * priority_perform_change_callbacks
 *
 * Periodically see if priority lists have changed, and call callbacks
 */
static void priority_perform_change_callbacks (void)
{
    priority_callback *current;
    ulong i;

    /* If nothing changed, just return */
    if (!TIMER_RUNNING(priority_entry_changed_time))
        return;

    TIMER_STOP(priority_entry_changed_time);

    for (current = priority_callbacks; current != NULL;
         current = current->next) {
        for (i = current->low; i <= current->high; i++) {
            if (priority_changed_option[i]) {
                (*current->callback_function)(i, priority_changed_option[i]);
            }
        }
    }
    for (i = 0; i < PRIORITY_MAXLIST; i++) {
        priority_changed_option[i] = 0;
    }
}

/*
 * priority_mark_list_changed
 *
 * Mark an priority list as changed.
 */
static void priority_mark_list_changed (ulong list, ushort option)
{
    if ((list > 0) && (list < PRIORITY_MAXLIST)) {
        priority_changed_option[list] = option;
    }
    GET_TIMESTAMP(priority_entry_changed_time);
}


/*
 * priority_init
 * Initialize the priority data structure
 */

void priority_init (void)
{
    int i;

    for (i = 0; i < PRIORITY_MAXLIST; i++) {
	parray[i] = NULL;
	cust_parray[i] = NULL;
	fair_parray[i] = NULL;
	priority_changed_option[i] = 0;
    }
    priority_callbacks = NULL;
    TIMER_STOP(priority_entry_changed_time);

    /*
     * Initialize priority parser support
     */
    priority_parser_init();
    reg_add_priority_parse(PRICMD_LOWEST, prilist_lowest_custom, 
			   "prilist_lowest_custom");
    reg_add_priority_parse(PRICMD_DEFAULT, prilist_default, "prilist_default");
    reg_add_priority_parse(PRICMD_INTERFACE, prilist_interface,
			   "prilist_interface");
    reg_add_priority_parse(PRICMD_PROTOCOL, prilist_protocol,
			   "prilist_protocol");
    reg_add_priority_parse(PRICMD_QLIMIT, prilist_qlimit, "prilist_qlimit");
    reg_add_priority_parse(PRICMD_QCUSTOM, custom_qcount, "custom_qcount");
    reg_add_default_priority_protocol_compare(prilist_true, "prilist_true");
    reg_add_default_encaps_queuing_allowed(default_encaps_queuing_allowed,
					   "default_encaps_queuing_allowed");
}
/*
 *        fair_reserve_conversation
 *
 *        get a conversation queue number to assign to a data flow
 *        returns 0 if no conversations available, or a ushort integer
 */
ushort
fair_reserve_conversation (hwidbtype * idb)
{
    fair_queue_head *fq;
    fair_queue *queue;
    int conversation_number;
    int upper_bound;

    if (!idb)
	return(0);
    fq = idb->fair_queue_head;
    if (!fq)
	return(0);
    queue = fq->fq_reserved_queue;
    if (!queue)
	return(0);

    conversation_number = fq->fq_reserved_queue_start;
    upper_bound = fq->fq_queue_count;

    for (; conversation_number < upper_bound; queue++, conversation_number++) {
	if (!queue->q.maximum) {

	    /*
	     * hack alert!
	     *
	     * in order to preserve the size of the fair_queue_ data
	     * structure at 32 bytes (quick indexing), we overload the
	     * q.maximum field. If the queue is not currently reserved,
	     * q.maximum is zero. If it *is* currently reserved, it
	     * is the queueing subsystem's maximum queue depth, in order
	     * to drop when we need to.
	     */

	    fq->fq_allocated_conversations++;
	    if (fq->fq_max_allocated_conversations <
		fq->fq_allocated_conversations)
		fq->fq_max_allocated_conversations++;
	    pak_queue_set_max_depth(&queue->q, fq->fq_max_enqueue);
	    return(conversation_number);
	}
    }
    return(0);		  /* return "no conversation available" */
}
/*
 *	fair_release_conversation
 *
 *	release a previously assigned conversation number
 */
void
fair_release_conversation (hwidbtype * idb, ushort conversation_number)
{
    fair_queue_head *fq;
    fair_queue *queue;

    if (!idb)
	return;
    fq = idb->fair_queue_head;
    if (!fq)
	return;
    if (!(fq->fq_reserved_queue_start <= conversation_number &&
	  conversation_number < fq->fq_queue_count))
	return;
    queue = fq->fq_reserved_queue + 
		   (conversation_number - fq->fq_reserved_queue_start);
    if (!queue)
	return;
    if (queue->q.maximum) {
	pak_queue_set_max_depth(&queue->q, 0);
	fq->fq_allocated_conversations--;
    }
}

/*
 * Given a plist entry, determine type.  Type is defined as which
 * member of priority_parse() case registry is responsible for setting
 * this variable.
 */
plist_entrytype pl_get_entrytype (plisttype *plist)
{
    plist_entrytype return_code = PL_ILLEGAL;
    
    if (plist->interface)
	return_code = PL_INTERFACE;
    else if ((plist->linkcode) || (plist->gt_size)  || (plist->lt_size)  ||
	     (plist->tcp_port) || (plist->udp_port) || (plist->testlist) ||
	     (plist->ip_fragments))
	return_code = PL_PROTOCOL;	
    else if (plist->stun_group)
	return_code = PL_STUN;
    else if (plist->bstun_group)
	return_code = PL_BSTUN;

    return(return_code);
}    

int pl_map_pltype2pricmd(plist_entrytype pl_type) {
    int return_code = (int) NULL;
    
    switch (pl_type) {
      case PL_INTERFACE:
	  return_code = PRICMD_INTERFACE;
	  break;

      case PL_PROTOCOL:		/* intentional fall-through */
      case PL_GT_SIZE:		/* all of these keywords processed by */
      case PL_LT_SIZE:		
      case PL_TCP_PORT:		/* prilist_protocol() */
      case PL_UDP_PORT:
      case PL_TEST_LIST:
      case PL_IP_FRAG:
	  return_code = PRICMD_PROTOCOL;
	  break;

	  /* 
	   * these cases are not processed in the
	   * reg_invoke_priority_parse() hierarchy
	   *
	   */
      case PL_STUN:
      case PL_BSTUN:
      case PL_ILLEGAL:
      default:
	  break;
    }

    return(return_code);
}

/*
 * return maximum queue depth in messages
 */
ulong maximum_interface_queue_depth(hwidbtype *hwidb)
{
    if ((QUEUE_FAIR | QUEUE_FAIR_DEF) & hwidb->queue_algorithm)
	return hwidb->mci_txcount + hwidb->fair_queue_head->fq_max_enqueue;
    else
	return hwidb->mci_txcount + hwidb->holdqueue_out;
}
