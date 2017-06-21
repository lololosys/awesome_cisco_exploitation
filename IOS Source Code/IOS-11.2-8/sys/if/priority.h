/* $Id: priority.h,v 3.5.18.6 1996/07/25 23:35:11 ccartee Exp $
 * $Source: /release/112/cvs/Xsys/if/priority.h,v $
 *------------------------------------------------------------------
 * Prototypes for if/priority.c (consistent with interface_generic.h)
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions for priority and custom queueing
 *------------------------------------------------------------------
 * $Log: priority.h,v $
 * Revision 3.5.18.6  1996/07/25  23:35:11  ccartee
 * CSCdi63068:  queuing strategy cannot be recovered after reboot or
 * download
 * Branch: California_branch
 * Invoke parser operation in traversed 'plist' order, and not in fixed
 * 'command' order.
 *
 * Revision 3.5.18.5  1996/05/17  11:23:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.3  1996/05/05  23:10:04  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.4.2  1996/04/26  07:55:26  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.1  1996/04/03  14:38:41  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.18.4  1996/05/15  05:40:24  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/05/04  00:46:49  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore the input to the Bridge-group Virtual Interface if necessary
 * - Support user settable mtu for the BVI.
 * - Introduce global_irb_enable, SET_BRIDGING_RXTYPE and "show idb irb"
 *
 * Revision 3.5.18.2  1996/04/17  13:40:27  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  20:14:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/02/20  00:57:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/09  04:02:00  fred
 * CSCdi46763:  Max queue size for fair queues needs to be greater than 512
 *         Voila! Was 512, now 4096
 *
 * Revision 3.4.14.1  1996/01/24  22:18:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/12  23:27:18  fred
 * CSCdi42501:  Weighted Fair Queuing (WFQ) uses too much memory.
 *         Remove RSVP queues and hash the link layer protocol queues.
 *         Long term fix in Californi
 *
 * Revision 3.3  1995/11/17  09:30:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/10/21  19:17:20  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 * Enable use of PQ/CQ/WFQ with Flow and Optimum Switching
 *
 * Revision 2.12  1995/09/23  21:27:04  gglick
 * CSCdi40746:  X.25 and LAPB not working
 *
 * Revision 2.11  1995/09/16  01:07:30  fred
 * CSCdi40357:  Make WFQ configuration work correctly with Virtual and
 * Null IDBs.
 *
 * Revision 2.10  1995/09/07  04:32:02  fred
 * CSCdi39816:  interfaces requiring secondary queuing are broken.
 *              This includes tunnels, LAPB, X.25, Loopback, and a few
 *              others
 *
 * Revision 2.9  1995/09/01  22:49:15  asastry
 * CSCdi39224:  WFQ conversation counters are inaccurate on 7000.
 *
 * Revision 2.8  1995/08/23  18:19:03  fred
 * CSCdi39010:  WFQ + ip route-cache cbus on FSIP fails
 * Code cleanups to improve user understandibility of what's happening
 *
 * Revision 2.7  1995/08/08  19:28:58  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.6  1995/07/27  10:54:52  irfan
 * CSCdi37751:  Weighted Fair Queueing not configurable over Async
 * interfaces
 * fix it. Also add a cumulative counter of all packets FQ thinks it
 * ever kept around and a counter of all packets FQ thinks it gave back.
 *
 * Revision 2.5  1995/07/10  05:39:14  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.4  1995/06/21  21:17:29  fred
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
 * Revision 2.3  1995/06/17  00:49:23  lbustini
 * CSCdi35984:  router hangs when ddr interface goes down
 * Don't share a single WFQ between rotary group members as this can cause
 * output_qcount to go negative.
 *
 * Revision 2.2  1995/06/09  03:34:56  fred
 * CSCdi35326:  %ALIGN-1-FATAL: Illegal access to a low address -
 *              fair_enqueue
 * CSCdi35163:  reported hang did not occur in this code version
 * CSCdi35342:  corrected syntax error in priority-list and queue-list
 *              command in the manner suggested in the DDTs.
 * Also - modified "show queueing fair" command output per customer
 * request
 *
 * Revision 2.1  1995/06/07  20:54:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PRIORITY_H__
#define __PRIORITY_H__

/* Much of the original contents of this has been split out to 
 * priority_private.h so that what remains here does not require 
 * interface_private.h, and only requires interface_generic.h
 * All in the name of decoupling, aka modularity  ... gcox.
 */

#define PRIORITY_MAXLIST  17

#define BSC_MAX_ADDR_LENGTH   1
#define SDLC_MAX_ADDR_LENGTH  8

/*
 * An item in a priority list.
 */
struct plisttype_ {
    struct plisttype_ *next;
    uchar linkcode;
    uchar priority;
    ushort testlist;
    long lt_size;
    long gt_size;
    ushort tcp_port;
    ushort udp_port;
    boolean ip_fragments;
    hwidbtype *interface;
    uchar stun_group;
    uchar sdlc_addr[SDLC_MAX_ADDR_LENGTH];
    uchar bstun_group;
    uchar bsc_addr[BSC_MAX_ADDR_LENGTH];
};

typedef enum plist_entrytype_ {
    PL_ILLEGAL = 0,		/* catch unitialized values */
    PL_INTERFACE,
    PL_PROTOCOL,
    PL_GT_SIZE,
    PL_LT_SIZE,
    PL_TCP_PORT,
    PL_UDP_PORT,
    PL_TEST_LIST,
    PL_IP_FRAG,
    PL_STUN,
    PL_BSTUN
} plist_entrytype;    


struct pritype_ {
    plisttype *plist;
    ushort qsize[PRIORITY_MAXTYPE];
    ushort qsize_def[PRIORITY_MAXTYPE];
    ulong bytecount[PRIORITY_MAXTYPE];
    ushort pdefault;
    boolean custom;
    ushort lowest_custom;
};

extern pritype *parray[];
extern pritype *cust_parray[];
extern pritype *fair_parray[];

static inline pritype *
PRIORITY_ARRAY (hwidbtype * idb)
{
    fair_queue_head *fair_queue_head = idb->fair_queue_head;
    if (NULL != fair_queue_head)
        return fair_parray[idb->priority_list];
    if (idb->custom_queuenum)
        return cust_parray[idb->priority_list];
    return parray[idb->priority_list];
}

#define FAIR_QUEUE_BASE_WEIGHT  4096    /* weight of a standard conversation */

extern char *priority2name(int);
extern void priority_init(void);
/* here for stun.c */
extern char *queue2name(int, boolean);
/* for stun_priority.c */
extern char *idb_queue2name(hwidbtype *, int);
extern void delete_fair_queue(hwidbtype *idb);

extern short linktype_is_fq_conversation(paktype *);
extern ushort priority_type(hwidbtype *hwidb);
#endif    /* __PRIORITY_H__     */
