/* $Id: dual.h,v 3.4.26.14 1996/09/10 18:09:16 sluong Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/dual.h,v $
 *------------------------------------------------------------------
 * DUAL - Diffusing update algorithm.  Definitions.
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dual.h,v $
 * Revision 3.4.26.14  1996/09/10  18:09:16  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.4.26.13  1996/09/04  23:46:30  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.4.26.12  1996/08/27  20:23:35  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.4.26.11  1996/06/28  02:49:01  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.4.26.10  1996/06/28  02:40:25  dkatz
 * CSCdi61491:  EIGRP may lose routes
 * Branch: California_branch
 * Only ignore the routes we are supposed to ignore.
 *
 * Revision 3.4.26.9  1996/06/21  16:51:35  sluong
 * CSCdi57042:  ipx floating static route kicks in after 4 min with EIGRP
 * Branch: California_branch
 *
 * Revision 3.4.26.8  1996/06/18  16:46:29  dkatz
 * CSCdi60515:  EIGRP summaries stay in topo table
 * Branch: California_branch
 * Rework the decision process for doing topo table searches for
 * the min metric under a summary.  Get rid of the metric count
 * field.  Add a hook to suppress multiple enqueued requests for
 * a min metric search.
 *
 * Revision 3.4.26.7  1996/06/18  16:42:03  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.4.26.6  1996/05/07  17:26:39  dkatz
 * CSCdi55252:  EIGRP SAP queue is not emptied when last peer is gone
 * Branch: California_branch
 * Flush the SAP queue when the last peer goes down.
 *
 * Revision 3.4.26.5  1996/04/02  07:35:12  dkatz
 * CSCdi52655:  EIGRP adjacency messages should default off
 * Branch: California_branch
 *
 * Revision 3.4.26.4  1996/04/02  07:32:35  dkatz
 * CSCdi52398:  EIGRP SAP defers too much to DUAL
 * Branch: California_branch
 * Interleave SAP and DUAL packets.  While we're in there, default SAP
 * split-horizon off again;  it's still messed up.
 *
 * Revision 3.4.26.3  1996/04/02  07:29:19  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.4.26.2  1996/04/02  07:23:33  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Fix a typo that left mondo weird stuff in the backup tables.  Get rid
 * of SAP CPUHOGs.  Turn split horizon back on by default.
 *
 * Revision 3.4.26.1  1996/04/02  05:36:52  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.4  1996/01/22  06:14:07  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/12  17:55:00  albright
 * CSCdi44810:  eigrp wakeup timers ignored by new scheduler
 *
 * Revision 3.2  1995/11/17  17:32:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:06:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/05  02:09:11  mikel
 * CSCdi40200:  EIGRP: incorrect redistributed routes in topology table
 * check to see an existing drdb already exists in the topology table.  If
 * so,  don't add overwrite the existing drdb.  This also eliminates
 * continuous updates seem when mutually redistributing
 * between different EIGRP processes
 *
 * Revision 2.4  1995/09/07  01:45:36  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.3  1995/07/13  14:28:27  albright
 * CSCdi34641:  Crash in dual_getnext after adding passive interface to
 * EIGRP
 *
 * Revision 2.2  1995/06/28  03:21:06  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:05:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DUAL_H__
#define __DUAL_H__

#include "igrp_common.h"
#include "../os/clock.h"

/*
 * EIGRP revision level
 *
 * The revision level is used to track upward compatible changes in the
 * protocol, so that newer code can know whether or not it may exploit these
 * changes.  Non-upward compatible changes should be signalled by changing
 * the "version" field in the EIGRP header (and woe be unto you).
 */
#define EIGRP_REVISION_MAJOR_LEVEL 1	/* Lube job, tires rotated */
#define EIGRP_REVISION_MINOR_LEVEL 0	/* It may be back in the shop soon */

/*
 * Define ACTIVE_METRIC_KLUDGE to force a check that makes sure all queries
 * are sent with the same metric.
 */
#define ACTIVE_METRIC_KLUDGE

/*
 * Define DUAL_SPLIT_HORIZON_QUERIES to enable split horizon on queries.
 */
#define DUAL_SPLIT_HORIZON_QUERIES

/* Define IGRP2_INDUCE_ERRORS to make it possible to induce data errors. */

/*#define IGRP2_INDUCE_ERRORS*/

#define IGRP2_HELLOTIME		(5 * ONESEC)
#define IGRP2_HOLDTIME		(IGRP2_HELLOTIME * 3)
#define IGRP2_HELLOTIME_NBMA	(60 * ONESEC)
#define IGRP2_HOLDTIME_NBMA	(IGRP2_HELLOTIME_NBMA * 3)
#define IGRP2_SLOW_VISIBLE_BANDWIDTH	1544	/* T1 */

#define IGRP2_DEFAULT_LOG_ADJ_CHANGE FALSE /* Default is to not log */

/*
 * Packetization delay.  This is the delay inserted between the time that
 * a topology table change is made, and the time that the interface packetizers
 * are invoked.  This will allow bigger packets to be built when there are
 * lots of things changing, and also means that we don't have to scan all
 * interfaces for every topo table change.
 */
#define IGRP2_PACKETIZATION_DELAY 10	/* Milliseconds */

#define IGRP2_DEF_ACTIVE_TIME (3 * ONEMIN)
#define IGRP2_ACTIVE_SCAN_FREQUENCY (ONEMIN)

#define IGRP2_JITTER_PCT	15	/* Jitter timers by 15 percent */

#define IGRP2_MAX_QDEPTH	200	/* Max number of input packets */

#define IGRP2_ACL_CHANGE_DELAY (10*ONESEC) /* Delay before tearing down nbrs */

#define IGRP2_DEFAULT_BANDWIDTH_PERCENT 50 /* Use 50% of the link by default */

#define IGRP_TO_IGRP2_SHIFT	8	/* Convert IGRP to IGRP2 metrics */

/*
 * Values for retransmission control.
 */
#define IGRP2_INIT_RTO     (2 * ONESEC)	/* Initial RTO value */
#define IGRP2_RTO_LOWER    200		/* Absolute minimum RTO */
#define IGRP2_RTO_UPPER    5*ONESEC	/* Beyond this it's silly */
#define IGRP2_RTO_SRTT_MULTIPLIER 6	/* Multiply by SRTT to get RTO */
#define IGRP2_VERSION      2
#define IGRP2_RETRY_LIMIT  16
#define IGRP2_RTO_PACING_MULTIPLIER 6	/* Bound RTO by N pacing ivls */
#define IGRP2_FLOW_SRTT_MULTIPLIER 4	/* Mcast flow timer = N * avg SRTT */
#define IGRP2_MIN_MCAST_FLOW_DELAY 50	/* Minimum multicast flow timer */
#define IGRP2_MAX_PPS 100		/* Max PPS of reliable pkts per i/f */
#define IGRP2_MIN_PACING_IVL (ONESEC/IGRP2_MAX_PPS) /* Min pacing interval */

/*
 * Values for circuit pacing
 */
#define IGRP2_NO_UNRELIABLE_DELAY_BW 1000 /* Kbps, above this no ack pacing */

/*
 * Default setting for EIGRP route holddown
 */
#define EIGRP_HOLDDOWN_DEFAULT 5*ONESEC

/*
 * TLV type definitions.  Generic (protocol-independent) TLV types are
 * defined here.  Protocol-specific ones are defined elsewhere.  IP
 * TLVs are in the 0x0100 range, ATALK in 0x0200, and IPX in 0x0300.
 */
#define IGRP2_PARAMETER    0x0001
#define IGRP2_AUTH         0x0002
#define IGRP2_SEQUENCE	   0x0003
#define IGRP2_SW_VERSION   0x0004
#define IGRP2_NEXT_MCAST_SEQ 0x0005

/*
 * SW_VERSION
 */
typedef struct igrp2_sw_version_ {
    uchar ios_majorver;			/* IOS major version */
    uchar ios_minorver;			/* IOS minor version */
    uchar eigrp_majorrev;		/* EIGRP major revision */
    uchar eigrp_minorrev;		/* EIGRP minor revision */
} igrp2_sw_version;
#define IGRP2_SW_VERSION_SIZE 4

/*
 * The following is bogus.  We should really figure out how big we can
 * make this based on the MTU of the interface and protocol.  However,
 * there's no way to deal with the situation where the size of the sequence
 * TLV is too large, other than to simply drop it, along with the multicast
 * packet that it is associated with, and unicast the packet instead.
 */
#define IGRP2_MAX_SEQ_TLV 1500		/* Max size of a Sequence TLV */

/*
 * Magic macros for handling the reply status handle arrays.  They're
 * bit arrays, one per peer.
 */
#define HANDLE_MALLOC_SIZE(array_size) ((array_size) * sizeof(ulong))
#define BITSIZE_ULONG	   (sizeof(ulong) * 8)
#define HANDLE_TO_CELL(h)  ((h) / BITSIZE_ULONG)
#define CELL_TO_HANDLE(c)  ((c) * BITSIZE_ULONG)
#define HANDLE_TO_BIT(h)   (1 << ((h) % BITSIZE_ULONG))
#define ULONG_HANDLE(f,h)  (f[HANDLE_TO_CELL(h)])
#define CLEAR_HANDLE(f,h)  (ULONG_HANDLE(f,h) &= ~HANDLE_TO_BIT(h))
#define SET_HANDLE(f,h)    (ULONG_HANDLE(f,h) |= HANDLE_TO_BIT(h))
#define TEST_HANDLE(f,h)   (ULONG_HANDLE(f,h) & HANDLE_TO_BIT(h))

/*
 * Work queue types.  This is used to tag work elements on the work queue
 * for processing redistributed, lost, and backup entries.
 */
typedef enum {
    IGRP2_WORK_REDIST = 0,
    IGRP2_WORK_IFDOWN,
    IGRP2_WORK_LOST,
    IGRP2_WORK_BACKUP,
    IGRP2_WORK_CONNSTATE,
    IGRP2_WORK_REDISTALL,
    IGRP2_WORK_SUMMARY,			/* ip needs this */
    IGRP2_WORK_PASSIVEINTF,		/* as well as this */
    IGRP2_WORK_UNREDIST,		/* appletalk needs this */
    IGRP2_WORK_SENDSAP,			/* ipx needs this */
    IGRP2_WORK_TABLERELOAD,
    IGRP2_WORK_INTEXTSTATE
} igrp2_eventtype;

#define DUAL_WORKQ_CHUNKSIZE 50		/* Workq chunk block size */

/*
 * Managed timer types
 */
enum {IGRP2_IIDB_PACING_TIMER, IGRP2_PEER_SEND_TIMER, IGRP2_IIDB_HELLO_TIMER,
      IGRP2_PEER_HOLDING_TIMER, IGRP2_FLOW_CONTROL_TIMER, IGRP2_ACTIVE_TIMER,
      IGRP2_PACKETIZE_TIMER, IGRP2_PDM_TIMER, IGRP2_IIDB_ACL_CHANGE_TIMER,
      IGRP2_DDB_ACL_CHANGE_TIMER};

/*
 * IGRP_OPC_UPDATE and IGRP_OPC_REQUEST are defined as 1 and 2 respectively
 * in igrp.h
 */
#define IGRP_OPC_QUERY     3
#define IGRP_OPC_REPLY     4
#define IGRP_OPC_HELLO     5
#define IGRP_OPC_IPXSAP    6
#define IGRP_OPC_PROBE	   7		/* For test purposes */
#define IGRP_OPC_ACK	   8		/* Really a kludge for debugging */
#define IGRP_NUM_OPCODES   9		/* 0 isn't used... */
#define IGRP_OPC_ILLEGAL   0xffff

#define IGRP2TLMACRO \
    ushort PACKED(type);        /* Type of data following */             \
    ushort PACKED(length);      /* Length in bytes of data following */
#define IGRP2_TLV_HDR_LENGTH 4		/* Avoid sizeof() */

#define IGRP2METRICPAKMACRO \
    uchar PACKED(delay[4]);	/* delay, in ??? of microseconds */      \
    uchar PACKED(bandwidth[4]);	/* bandwidth, in units of 1 Kbit/sec */  \
    uchar PACKED(mtu[3]); 	/* MTU, in octets */                     \
    uchar PACKED(hopcount);     /* Number of hops to destination */      \
    uchar PACKED(reliability);	/* percent packets successfully tx/rx */ \
    uchar PACKED(load);		/* percent of channel occupied */        \
    uchar PACKED(mreserved[2]); /* fill out to long word boundary */

/*
 * Unpacked (non packet) version of above
 */
typedef struct igrp2metric_ {
    ulong delay;
    ulong bandwidth;
    ulong mtu;
    ulong hopcount;
    ulong reliability;
    ulong load;
} igrp2metric;

typedef union igrp2nettype_ {
    struct ip_network {
	ipaddrtype address;
	ipaddrtype mask;
    } ip_network;
    ulong nov_network;
    struct {
	ushort start;			/* start of cable-range */
	ushort end;			/* end of cable-range */
    } atalk_network;
} igrp2nettype;

typedef union igrp2addrtype_ {
    ipaddrtype ip_address;
    struct nov_address {
	ulong net;
	uchar host[IEEEBYTES];
    } nov_address;
    ipaddrtype atalk_address;		/* not a FQA; net.node */
} igrp2addrtype;

/*
 * Anchor entry.  This data structure is used for anchoring DNDBs or DRDBs
 * on the transmit thread.  This is done as a separate structure so that
 * the DNDB can be moved while anchored (if it is, a dummy xmit thread
 * entry will be put in its place).  We need to keep the real DNDB/DRDB in
 * its new place, since it is linked to other things in a spectacular fashion.
 */
typedef struct igrp2_xmit_anchor_ {
    int xa_anchor_count;		/* Count of anchors */
    struct igrp2_xmit_thread_ *xa_thread; /* Anchored entry */
} igrp2_xmit_anchor;
#define IGRP2_MIN_ANCHOR 500		/* Block size for chunk_malloc */

/*
 * Thread entry structure.  This structure is used to thread DNDBs and DRDBs
 * together to form the threads used for maintaining the state of which
 * information is to be transmitted.
 *
 * DNDBs and DRDBs are threaded together in a single thread that hangs from
 * the DDB.  The entries are in the order in which events took place to 
 * cause the information to be transmitted.  Each entry has a serial number
 * which is always in ascending order (circularly) on the thread.  Each
 * interface has a state machine which walks down the thread.  Groups of DNDBs
 * and DRDBs which are consecutive and can be carried in the same packet
 * bundled together into packets and transmitted.  The packetization is
 * triggered by flow-control-ready events coming from the transport.
 *
 * DNDBs stay permanently threaded so that new peers can receive a full dump
 * of the topology table;  DRDBs are only threaded until the reply packet
 * that was generated from them is acknowledged.
 *
 * An entry can be "anchored" by a number of things (IIDBs, packet descriptors,
 * etc.)  This means that a pointer is being held to this entry, so it
 * cannot be freed or moved.  If the entry containing this thread element
 * changes and must be moved, it is copied first to a dummy entry.  This keeps
 * the entry intact (with its serial number) in the same place in the thread,
 * and a new entry is created and linked on the end.
 *
 * If this entry is anchored, it will have a pointer to an anchor entry.
 */
typedef struct igrp2_xmit_thread_ {
    struct igrp2_xmit_thread_ *xmit_next;  /* Next guy in chain */
    struct igrp2_xmit_thread_ **xmit_prev; /* Prev guy's next ptr (or head) */
    ulong xt_serno;			/* Serial number of this entry */
    int xt_refcount;			/* Number of active senders */
    igrp2_xmit_anchor *xt_anchor;	/* Pointer to anchor, or NULL */
    uchar xt_dummy:1;			/* TRUE if this is a dummy entry */
    uchar xt_drdb:1;			/* TRUE if this is in a DRDB */
} igrp2_xmit_thread;
#define DUAL_SERNO_MODULUS 0xff		/* Testing %% */
#define IGRP2_MIN_THREAD_DUMMIES 200	/* chunk_malloc size */


/*
 * Transmit queue types.  There is a separate transmit queue for reliable
 * and unreliable packets, both at the peer level and at the interface
 * level.
 *
 * Please don't change the order of the constants below;  some code assumes
 * that the first queue is the unreliable one.
 */
typedef enum {UNRELIABLE_QUEUE = 0, RELIABLE_QUEUE, NUM_QUEUE_TYPES}
    xmit_queue_type;

/*
 * Count of number of unreliable packets that may be sent to a peer
 * before a reliable packet is required.  This keeps the unreliable
 * packets from locking out the reliable ones.
 */
#define MAX_UNRELIABLE_PACKETS 5

/*
 * Count of number of unicasts that may be sent on an interface before a
 * multicast is required.  This keeps the unicast packets from locking
 * out the multicasts.
 */
#define MAX_UNICAST_PACKETS 5

/*
 * peertype	-- descriptor block for a EIGRP peer.
 *
 * Peers descriptor blocks are instantiated and maintained by
 * the EIGRP transport, not the DUAL routing engine.
 *
 * Peer descriptors contain all the information necessary for the
 * operation of the reliable transport.
 */
struct dual_peertype_ {
    struct dual_peertype_ *next;	/* Next peer at this hash location */
    struct dual_peertype_ *next_peer;	/* Next peer in global list */
    ulong last_startup_serno;		/* Serial # of end of startup stream */
    igrp2addrtype source;		/* Peer id */
    ulong router_id;			/* Peer router #, for protocols w/o */
					/* unique addresses */
    struct igrp2_idbtype_ *iidb;	/* Interface peer discovered on */
    int peer_handle;			/* Unique handle assigned to peer */
    mgd_timer holding_timer;		/* Holding timer for peer */
    ulong last_holding_time;		/* Holding time rcvd in last hello */
    sys_timestamp uptime;		/* Timestamp when peer came up */
    queuetype xmitQ[NUM_QUEUE_TYPES];	/* Reliable & unreliable xmit queues */
    ulong unrely_left_to_send;		/* Unrely left before rely sent */
    ulong retransmit_count;		/* Count of rexmissions to this peer */
    sys_timestamp pkt_first_send_time;	/* Time current packet first sent */
    sys_timestamp reinit_start;		/* Time we started reinit */
    ushort retry_count;			/* Rxmit count for packet on queue */
    uchar cr_mode:1;			/* Conditional Received Mode set */
    uchar need_init:1;			/* TRUE if next pkt should have INIT */
    uchar need_init_ack:1;		/* Need ACK of our INIT packet */
    uchar going_down:1;			/* Peer being cleared by command */
    uchar coming_up:1;			/* Peer is just coming up */
    uchar peer_deleted:1;		/* Peer has been freed */
    ulong last_seq_number;		/* Last rcvd sequence number */
    ulong srtt;				/* Smoothed round trip time */
    ulong rto;				/* Retransmission time out */
    ulong cr_sequence;			/* Expected seq # of CR packet, or 0 */
    igrp2_sw_version peer_version;	/* Software version running on nbr */
    mgd_timer peer_send_timer;		/* Timer for peer (re)transmission */
    void *protocol_data;		/* Protocol specific data about nbr */
    ulong rcv_probe_sequence;		/* Seq number of last probe pkt rcvd */
    ulong send_probe_sequence;		/* Seq number of next probe to send */
    char *down_reason;			/* Reason that neighbor went down */
};

/*
 * Topology table structures.
 */
typedef enum {				/* Routing information origins */
    IGRP2_ORG_IGRP2 = 0,		/* igrp2 destination */
    IGRP2_ORG_CONNECTED,		/* connected destination */
    IGRP2_ORG_REDISTRIBUTED,		/* redistributed destination */
    IGRP2_ORG_SUMMARY			/* summary destination */
} igrp2origintype;

/*
 * rdbtype -- route descriptor blocks.
 *
 * In DUAL, there can be multiple routes to a destination network.
 * The routing, (sometimes called "path") information is stored
 * in these route descriptor blocks.
 *
 * Routes can be introduced into the DUAL topology database via DUAL
 * (which are native routes) or by route redistribution from other
 * routing protocols; such routes are called "external" routes and
 * are tagged as such at the point in the DUAL topology where they
 * are first introduced into the topology table.
 *
 * Externally derived routes may carry external routing information
 * which is specific to the routing protocol which is the origin of the
 * route. A pointer to this is kept off the side of the rdb in the
 * 'data' field.
 */
typedef struct dual_rdbtype_ {
    struct dual_rdbtype_ *drdb_next;	/* Pointer to next rdb */
    struct dual_ndbtype_ *drdb_dndb;	/* Pointer back to owning DNDB */
    igrp2_xmit_thread drdb_thread;	/* Linkage for reply transmission */
    igrp2addrtype drdb_nexthop;		/* Possible successor */
    int drdb_handle;			/* Handle of advertising peer */
    igrp2addrtype drdb_infosource;	/* Who, if anyone, told us */
    struct igrp2_idbtype_ *drdb_iidb;	/* Interface this info came in on */
    ulong drdb_successor_metric;	/* Metric (neighbor's view) */
    ulong drdb_metric;			/* Composite metric (our view) */
    igrp2metric drdb_vecmetric;		/* Vectorized metric */
    igrp2origintype drdb_origin;	/* Origin of this information */
    ushort drdb_send_flag;		/* Send update, query, or reply */
    uchar drdb_opaque_flags;		/* Flags maintained by PDM */
    uchar drdb_keep:1;			/* Keep rdist dest during tbl reload */
    uchar drdb_external:1;		/* Is this externally derived dest? */
    uchar drdb_ignore:1;		/* Used for inbound filtering */
    uchar drdb_deleted:1;		/* DRDB has been freed */
    uchar drdb_iidb_deleted:1;		/* IIDB has been deleted */
    void *drdb_data;			/* Pointer to external info */
} dual_rdbtype;

#define NO_PEER_HANDLE (-1)		/* Handle for nonexistent peer */

/*
 * Handle structure.  This is effectively a bitmap, one for each peer.
 * One of these exists in the DDB, and one in each IIDB as well.  This
 * makes it efficient to set the reply status table in the face of
 * split horizon, etc.
 */
typedef struct igrp2_handle_type_ {
    ushort handles_used;		/* Number of bits set */
    ushort handle_array_size;		/* Size of handle array in words */
    ulong *handle_array;		/* Allocated array of handles */
} igrp2_handle_type;

/*
 * Query-origin states.  Used in paramater block and flags in topology table.
 */
typedef enum {
    DUAL_QOCLEAR=0,		/* Don't be confused; this isn't passive */
    DUAL_QOLOCAL,		/* We are the origin of this query */
    DUAL_QOMULTI,		/* Multiple computations for this dest */
    DUAL_QOSUCCS,		/* Successor is origin of this query */
    DUAL_QOCOUNT		/* Number of entries */
} dual_qo_t;

/*
 * ndbtype	-- network descriptor block.
 *
 * Used to decribe a network in the the topology table. In DUAL nomenclature,
 * a 'network' is a destination to the routing protocol.
 *
 * Networks may have more than one route by which we can reach that
 * network. These are kept off the ndb in the rdb chain; the first
 * rdb in the rdb chain is the "first next hop" (called 'successor' in
 * the DUAL nomenclature), with less optimal routes being next in the
 * rdb chain.
 */
typedef struct dual_ndbtype_ {
    struct dual_ndbtype_ *dndb_next;	/* Pointer to next ndb in has thread */
    struct dual_ndbtype_ *dndb_next_chg;/* Next dndb in change queue thread */
    igrp2_xmit_thread dndb_xmit_thread;	/* Transmit thread */
    igrp2nettype dndb_dest;		/* Destination network */
    ulong dndb_metric;			/* RD - reported metric (composite) */
    igrp2metric dndb_vecmetric;		/* RD - reported metric (vectorized) */
#ifdef ACTIVE_METRIC_KLUDGE
    igrp2metric query_metric;		/* Metric sent in queries */
#endif
    dual_qo_t query_origin;		/* Query-origin flag */
    ushort dndb_send_flag;		/* Send update, query, or reply */
    ushort dndb_successors;		/* # of successors for this dest */
    igrp2_handle_type dndb_reply_status;/* Reply status table handles */
    uchar dndb_opaque_flags_old;	/* Flags maintained by PDM */
    uchar dndb_external:1;		/* {in,ex}ternal of 1st successor */
    uchar dndb_split_horizon:1;		/* Used for split horizoning queries */
    uchar dndb_being_sent:1;		/* Set if DNDB is being sent */
    uchar dndb_tell_everyone:1;		/* Suppress split horizon */
    uchar dndb_deleted:1;		/* DNDB has been freed */
    uchar dndb_on_change_queue:1;	/* Set if on change thread */
    ulong dndb_old_metric;		/* Successors metric before active */
    dual_rdbtype *dndb_rdb;		/* Next hops.  1st one is successor */
    dual_rdbtype *dndb_succ;		/* Old successor when active.  If */
					/* NULL then rdb is old successor */
    struct igrp2_idbtype_ *dndb_query_sh_if; /* Split-horiz i/f for queries */
    sys_timestamp dndb_active_time;	/* Time when this dest went active */
} dual_ndbtype;

/*
 * Dual state definitions
 */
#define DUAL_PASSIVE  0
#define DUAL_ACTIVE   1

/*
 * Internal event indentifiers
 */
typedef enum dual_rtev_type_ {
    DUAL_RTUP,				/* New route came up */
    DUAL_RTDOWN,			/* Old route disappeared */
    DUAL_RTCHANGE,			/* Existing route changed metric */
    DUAL_RTEVCOUNT			/* Number of event types */
} dual_rtev_type;

/*
 * Mask of bits used in DUAL_QO* states (query-origin)
 */
#define DUAL_QOMASK DUAL_QOSUCCS

/*
 * send_flag definitions.  Order is important.  Reply is first,
 * followed by the remaining unicasts, followed by all multicasts.
 */
#define DUAL_SENDREPLY   0x1    /* Send a unicast reply */
#define DUAL_MULTQUERY   0x2    /* Send a multicast query */
#define DUAL_MULTUPDATE  0x4    /* Send a multicast update */

#define DUAL_MULTMASK    (DUAL_MULTUPDATE | DUAL_MULTQUERY)

#define DUAL_URCOUNT     0      /* Send a unicast reply */
#define DUAL_MQCOUNT     1      /* Send a multicast query */
#define DUAL_MUCOUNT     2      /* Send a multicast update */
#define DUAL_MAXFLAG     3

/*
 * IGRP2 fixed header definition.
 */
typedef struct igrp2type_ {
    uchar version;
    uchar opcode;
    ushort checksum;
    ulong flags;
    ulong sequence;
    ulong ack;
    ulong asystem;
} igrp2type;

/*
 * Init bit definition. First unicast transmitted Update has this
 * bit set in the flags field of the fixed header. It tells the neighbor
 * to down-load his topology table.
 */
#define IGRP2_INIT_FLAG 0x01

/*
 * CR bit (Conditionally Received) definition in flags field on header. Any 
 * packets with the CR-bit set can be accepted by an IGRP2 speaker if and 
 * only if a previous Hello was received with the SEQUENCE_TYPE TLV present.
 *
 * This allows multicasts to be transmitted in order and reliably at the
 * same time as unicasts are transmitted.
 */
#define IGRP2_CR_FLAG 0x02

/*
 * Structure definition for interfaces that a respective ddb is configured 
 * for.
 *
 * All reliable packets (unicast or multicast) are initially enqueued on the
 * IIDB xmitQ.  This ensures that the packets never get out of order.
 * As packets are pulled off this queue, they are given a sequence number
 * and are either sent immediately (for multicasts) or re-queued on the
 * peer queue (for unicasts).
 *
 * If the transport tells us that the interface is multicast flow-ready,
 * we will send the next packet (or go quiescent) even though not all
 * peers may have acknowledged the packet (and in fact the packet may be
 * retransmitted.)
 */
typedef struct igrp2_idbtype_ {
    struct igrp2_idbtype_ *next;	/* For the DDB queue */
    struct igrp2_idbtype_ *next_quiescent; /* Link for DDB quiescent list */
    struct igrp2_idbtype_ **prev_quiescent; /* Back link */
    idbtype *idb;			/* Interface IGRP2 is enabled for */
    ulong total_srtt;			/* Sum of all peer SRTTs */
    ulong hello_interval;		/* Configured hello interval for idb */
    ulong holdtime;			/* Configured holdtime for idb */
    ulong holddown;			/* Configured holddown for EIGRP */
    ulong max_packet_size;	/* Packet MTU, with header, minus overhead */
    igrp2_xmit_anchor *iidb_xmit_anchor; /* Anchor for current packet */
    boolean split_horizon;		/* Perform split horison on this idb */
    boolean iidb_going_down;		/* TRUE if the IIDB is to be deleted */
    boolean clear_connected_routes;	/* TRUE if they should be removed */
    boolean use_multicast;		/* TRUE if we should use real mcast */
    boolean passive_interface;		/* TRUE if this interface is passive */
    boolean multicast_enabled;		/* TRUE if mcast filter is set up */
    mgd_timer iidb_send_timer;		/* Used to pace xmits on this i/f */
    mgd_timer iidb_peer_timer;		/* Master for all peers on this i/f */
    mgd_timer iidb_holding_timers;	/* Master for holding timers on i/f */
    mgd_timer iidb_hello_timer;		/* Fires when we need to send hello */
    mgd_timer iidb_flow_control_timer;	/* Multicast flow control timer */
    mgd_timer iidb_packetize_timer;	/* Delay before packetization */
    mgd_timer iidb_acl_change_timer;	/* Delay after access list change */
    ulong last_mcast_flow_delay;	/* Last delay for flow control timer */
    ulong igrp2_bandwidth_percentage;	/* % of bandwidth for EIGRP */
    ulong iidb_send_interval[NUM_QUEUE_TYPES]; /* Base packet gap, per queue */
    ulong iidb_last_send_interval[NUM_QUEUE_TYPES]; /* Current packet gap */
    queuetype xmitQ[NUM_QUEUE_TYPES];	/* Interface transmit queues */
    ulong unicasts_left_to_send;	/* Count of ucasts before next mcast */
    struct igrp2_pakdesc_qelm_ *last_multi_pak; /* Last mcast packet on i/f */
    ulong mcast_start_refcount;		/* Refcount on mcast when sent */
    ulong iidb_last_mcast_seq;		/* Sequence number of last multicast */
    igrp2_handle_type iidb_handles;	/* Handles for peers on this i/f */
    ulong mcasts_sent[NUM_QUEUE_TYPES];	/* Count of un/reliable mcasts */
    ulong ucasts_sent[NUM_QUEUE_TYPES];	/* Count of un/reliable ucasts */
    ulong mcast_exceptions_sent;	/* Count of mcasts sent as ucasts */
    ulong cr_packets_sent;		/* Count of mcasts sent with CR */
    ulong acks_suppressed;		/* Count of suppressed ACK packets */
    ulong retransmissions_sent;		/* Count of retransmitted packets */
    ulong out_of_sequence_rcvd;		/* Count of out-of-sequence packets */
    ulong send_probe_sequence;		/* Sequence number of probe to send */
    queuetype pdm_queue;		/* Queue for PDM purposes */
    ulong pdm_defer_count;		/* # of DUAL pkts before PDM pkt */
#ifdef IGRP2_INDUCE_ERRORS
    ulong iidb_error_rate;		/* Induced error rate (n/128) */
#endif
} igrp2_idbtype;

/*
 * Route Parameters
 *
 * This data structure is passed around when a route has been received
 * (or redistributed).  It carries all the parameters as received.
 */
typedef struct dual_newroute_ {
    igrp2nettype new_dest;		/* Destination address/mask */
    igrp2addrtype new_nexthop;		/* Next hop */
    igrp2addrtype new_infosource;	/* Info source */
    igrp2_idbtype *new_iidb;		/* Interface */
    igrp2metric new_vecmetric;		/* Vectorized metric */
    ulong new_metric;			/* Composite metric */
    ulong new_successor_metric;		/* Successor metric */
    igrp2origintype new_origin;		/* Route origin */
    boolean new_ignore;			/* Ignore route due to filtering */
    boolean new_external;		/* Route is external */
    boolean new_keep;			/* Keep dest during table reload */
    void *new_data;			/* PDM-specific data */
    uchar new_opaque_flags;		/* PDM-specific flags */
} dual_newroute;

/*
 * Packet Descriptor
 *
 * This data structure is used to abstractly represent the contents of
 * a packet.  Note that for most packets, the contents are represented
 * as a range of serial numbers and the packet is not actually generated
 * until the time of transmission.
 *
 * If the sequence number is 0 and the packet is sequenced, this indicates
 * that the packet hasn't been sent yet (not a retransmission).  A nonzero
 * sequence number indicates that the packet has been sent already.
 *
 * Note that ACKs are represented as Hellos with the ack_packet bit set.
 * The sequence number value to acknowledge in this case is carried
 * in the ack_seq_num field.  For all sequenced packets, this field carries
 * the packet sequence number once it is assigned.
 *
 * If the packet is a unicast, the "peer" variable is set to point at
 * the peer structure.
 *
 * Pregenerated packets are denoted by pregen_packet being non-NULL.  In
 * this case the packet was generated when the descriptor was enqueued,
 * and so no callback is made to the PDM to build the packet when it is
 * ready to be transmitted (since the packet has already been built).
 */
typedef struct igrp2_pakdesc_ {
    dual_peertype *peer;		/* Peer to send to (or NULL) */
    igrp2_xmit_anchor *pakdesc_anchor;	/* Anchor for start_serno */
    ulong start_serno;			/* Starting serial number */
    ulong end_serno;			/* Ending serial number */
    igrp2type *pregen_pak;		/* Pregenerated packet */
    ulong length;			/* Length of packet */
    int refcount;			/* Count of elements pointing here */
    ulong ack_seq_num;			/* Ack/Seq number of this packet */
    uchar set_init_flag:1;		/* Set if INIT flag should be sent */
    uchar sequenced:1;			/* Set if packet is sequenced */
    uchar pakdesc_multicast:1;		/* Set if sent as a multicast */
    uchar ack_packet:1;			/* Set if this is an ACK */
    uchar opcode;			/* Packet opcode */
} igrp2_pakdesc;

#define IGRP2_MIN_PAKDESC 50		/* Malloc in blocks of 50 */

/*
 * Packet Descriptor Queue Element
 *
 * This data structure is enqueued to represent the desire to transmit a
 * packet to a particular peer, or on a particular interface.  The packet
 * is represented by an igrp2_pakdesc structure.
 */
typedef struct igrp2_pakdesc_qelm_ {
    struct igrp2_pakdesc_qelm_ *next;	/* Next element in queue */
    igrp2_pakdesc *pakdesc;		/* Pointer to packet descriptor */
    sys_timestamp time_sent;		/* Time transmitted (for RTT) */
    sys_timestamp xmit_time;		/* Time to send (for rexmits) */
} igrp2_pakdesc_qelm;

#define IGRP2_MIN_PAKDESC_QELM 200	/* Malloc in blocks of 200 */

/*
 * Input Descriptor
 *
 * This structure is enqueued on the DDB input queue to represent an input
 * packet.  It mainly exists because we want to do a clear_if_input when
 * we first receive the packet in the Hello process and I didn't want to do
 * any cheap hacks.  So I did an expensive hack instead.  Sigh.
 */
typedef struct igrp2_input_packet_ {
    struct igrp2_input_packet_ *next;	/* For queueing, bien sur. */
    paktype *packet;			/* The packet itself */
    idbtype *if_input;			/* Input interface */
} igrp2_input_packet;

#define IGRP2_MIN_INPAK_CHUNKS 100	/* Malloc in blocks of 100 */
    
/*
 * Protocol independent MIB counters. Send counts must precede receive
 * counts and values must be ulongs, see igrp2_update_mib().
 */
typedef struct igrp2_mibtype_ {
    ulong hellos_sent;
    ulong hellos_rcvd;
    ulong queries_sent;
    ulong queries_rcvd;
    ulong updates_sent;
    ulong updates_rcvd;
    ulong replies_sent;
    ulong replies_rcvd;
    ulong acks_sent;
    ulong acks_rcvd;
} igrp2_mibtype;

/*
 * dual event log handling
 *
 * Note!!  It is assumed that the xxXMIT events are immediately followed
 * by the xxXMIT2 events!
 */
typedef enum {
    DUALEV_GENERIC = 1,
    DUALEV_UPDATE_SEND,
    DUALEV_UPDATE_SEND2,
    DUALEV_STATE,
    DUALEV_CLEARHANDLE1,
    DUALEV_CLEARHANDLE2,
    DUALEV_REPLYFREE,
    DUALEV_SENDREPLY,
    DUALEV_FPROMOTE,
    DUALEV_RTINSTALL,
    DUALEV_RTDELETE,
    DUALEV_NDBDELETE,
    DUALEV_RDBDELETE,
    DUALEV_ACTIVE,
    DUALEV_NOTACTIVE,
    DUALEV_SETREPLY,
    DUALEV_SPLIT,
    DUALEV_SEARCHFS,
    DUALEV_FCSAT1,
    DUALEV_FCSAT2,
    DUALEV_FCNOT,
    DUALEV_RCVUP1,
    DUALEV_RCVUP2,
    DUALEV_RCVUP3,
    DUALEV_RCVQ1,
    DUALEV_RCVQ2,
    DUALEV_RCVR1,
    DUALEV_RCVR2,
    DUALEV_PEERDOWN1,
    DUALEV_PEERDOWN2,
    DUALEV_PEERUP,
    DUALEV_IFDEL1,
    DUALEV_IFDEL2,
    DUALEV_IFDEL3,
    DUALEV_LOST,
    DUALEV_CONNDOWN,
    DUALEV_CONNCHANGE,
    DUALEV_REDIST1,
    DUALEV_REDIST2,
    DUALEV_SIA,
    DUALEV_SIA2,
    DUALEV_SIA3,
    DUALEV_SIA4,
    DUALEV_SIA5,
    DUALEV_NORESP,
    DUALEV_MCASTRDY,
    DUALEV_INITSTUCK,
    DUALEV_NOINIT,
    DUALEV_HOLDTIMEEXP,
    DUALEV_ACKSUPPR,
    DUALEV_UCASTSENT,
    DUALEV_MCASTSENT,
    DUALEV_PKTSENT2,
    DUALEV_PKTSUPPR,
    DUALEV_PKTNOSEND,
    DUALEV_PKTRCV,
    DUALEV_PKTRCV2,
    DUALEV_PKTRCV3,
    DUALEV_BADSEQ,
    DUALEV_REPLYACK,
    DUALEV_REPLYPACK,
    DUALEV_REPLYENQ,
    DUALEV_REPLYXMIT,
    DUALEV_REPLYXMIT2,
    DUALEV_UPDATEACK,
    DUALEV_UPDATEPACK,
    DUALEV_UPDATEENQ,
    DUALEV_UPDATEXMIT,
    DUALEV_UPDATEXMIT2,
    DUALEV_QUERYACK,
    DUALEV_QUERYPACK,
    DUALEV_QUERYENQ,
    DUALEV_QUERYXMIT,
    DUALEV_QUERYXMIT2,
    DUALEV_STARTUPACK,
    DUALEV_STARTUPPACK,
    DUALEV_STARTUPENQ,
    DUALEV_STARTUPENQ2,
    DUALEV_STARTUPXMIT,
    DUALEV_STARTUPXMIT2,
    DUALEV_LASTPEER,
    DUALEV_FIRSTPEER,
    DUALEV_QSHON,
    DUALEV_QSHOFF,
    DUALEV_SETRD,
    DUALEV_OBE,
    DUALEV_UPDATESQUASH,
    DUALEV_EMPTYCHANGEQ,
    DUALEV_REQUEUEUCAST,
    DUALEV_XMITTIME,
    DUALEV_SUMREV,
    DUALEV_SUMREV2,
    DUALEV_SUMREV3,
    DUALEV_SUMDEP,
    DUALEV_GETSUM,
    DUALEV_GETSUM2,
    DUALEV_PROCSUM,
    DUALEV_PROCSUM2,
    DUALEV_IGNORE,
    DUALEV_UNKNOWN,
    DUALEV_COUNT			/* Number of event types */
} dual_event_code;

#define DUAL_DEFAULT_MAX_EVENTS 500	/* Default event buffer size */
#define DUAL_EVENTS_BUFFER_SIZE 24
#define DUAL_LOG_ALL(ddb, event, d1, d2) \
    dual_log_all(ddb, event, &(d1), &(d2))
#define DUAL_LOG(ddb, event, target, d1, d2) \
    dual_log(ddb, event, &(target), &(d1), &(d2))

typedef enum {
    DUAL_NONE,
    DUAL_NUMBER,
    DUAL_HEXNUMBER,
    DUAL_NET,
    DUAL_ADDR,
    DUAL_STRING,
    DUAL_SWIF,
    DUAL_BOOL,
    DUAL_RTORIGIN,
    DUAL_QUERYORIGIN,
    DUAL_RTEVENT,
    DUAL_PKTTYPE,
    DUAL_ETYPE_COUNT			/* Number of types */
} dual_etype_enum;

/* Logging defaults */

#define DUAL_LOGGING_DUAL_DEFAULT TRUE
#define DUAL_LOGGING_XPORT_DEFAULT FALSE
#define DUAL_LOGGING_XMIT_DEFAULT FALSE

/*
 * Event log structure.  These are built within a circular buffer hung from
 * the DDB.  The buffer size is configurable.
 */
typedef struct dual_event_type_ {
    dual_event_code code;               /* Event Code */
    clock_epoch timestamp;		/* Event timestamp */
    uchar buf[DUAL_EVENTS_BUFFER_SIZE]; /* This should really be a union of */
                                        /* all the different combinations */
                                        /* of interesting data used */
} dual_event_type;

/*
 * When we want to display the event log, we want to copy it atomically so
 * that it will maintain consistency for the arbitrarily long time it may
 * take to format it.  Rather than copying it into one giant block (which
 * we may have trouble obtaining), we copy it into a linked list of smaller
 * blocks.
 */
#define DUAL_SHOW_EVENT_FRAG_SIZE 65536 /* Target block size, give or take */
#define DUAL_SHOW_EVENT_ENTRY_COUNT (DUAL_SHOW_EVENT_FRAG_SIZE / \
				     sizeof(dual_event_type))
typedef struct dual_show_event_block_ {
    struct dual_show_event_block_ *next_block; /* List pointer */
    dual_event_type event_entry[DUAL_SHOW_EVENT_ENTRY_COUNT]; /* The events */
} dual_show_event_block;


typedef struct dual_event_decode_ {
    uchar *message;
    dual_etype_enum data1, data2;
} dual_event_decode;

/*
 * Scratch table for figuring out interface changes.  There's one entry
 * per IIDB in the system.  It's used only by dual_routeupdate, but
 * it's used a lot, so we hang it off of the DDB to avoid thrashing.
 */
typedef struct dual_iidb_scratch_ {
    igrp2_idbtype *iidb;		/* IIDB in question */
    boolean found;			/* TRUE if we had a succ on this i/f */
} dual_iidb_scratch;

/*
 * Return values from advertise_request
 */
typedef enum advertise_request_type_ {
    SUPPRESS_ROUTE,			/* Don't advertise it */
    ADVERTISE_ROUTE,			/* Advertise it */
    POISON_ROUTE			/* Advertise it with infinity metric */
} advertise_request_type;

/*
 * Dual descriptor block.
 * Explanation of fields:
 *  next             Pointer to next ddb in a queue of ddbs.
 * Static entries in the ddb.
 *  pdb              Pointer to protocol descriptor block.  This is a very
 *                   IP centric data structure.  It should probably be a
 *                   generic pointer used by the protocol specific routines.
 *                   Examples of information in this structure should be
 *                   things like input and output filters, distance info,
 *                   process name, process id, jump vectors to various
 *                   hooks and handlers.
 *  cleanup          This boolean tells us whether we need to delete any
 *                   rdb's from the topology table.  This occurs when a
 *                   destination has gone passive and some (or all) of the
 *                   next hops are reporting a metric of infinity.
 * Jump vectors to protocol specific support routines.  Note: we may not
 * need the first two entries.  Time will tell.
 *  (*accept_peer)() Find out if we should accept this peer
 *  (*add_item)()    Add a route to a packet.  Called from DUAL when the
 *		     packet is being constructed.
 *  (*addrcopy)()    Copy an IGRP2 address
 *  (*addrmatch)()   Return true if the two addresses passed in are equal.
 *  (*advertise_request)()  Called so the protocol dependent components can do
 *                   filtering and aggregation on per interface basis.
 *  (*buildpacket)() Called by the driver to build a protocol-specific packet
 *		     based on information in the packet descriptor.
 *  (*compare_external()  Compare the contents of a DRDB's external information
 *		     to help decide whether the route has changed.
 *  (*enable_process_wait)()  Called by the hello process to turn on a
 *		     PDM-specific process predicate.
 *  (*exterior_check)()  Test whether the interior/exterior state of a route
 *		     has changed.
 *  (*exterior_flag)()  Find a feasible successor with the exterior bit set.
 *  (*headerptr)()   Returns pointer to beginning of IGRP2 header.
 *  (*hellohook)()   Called after the hello packet has been allocated
 *                   and filled with the initial tlvs. This vector
 *                   is optional; if left as NULL, won't be called. After
 *                   this vector is called, the hello packet is sent.
 *                   Note that if the hellohook function appends data to the
 *                   end of the hello packet after the initial tlvs, it should
 *                   note the small allocated size of the hello buffer or
 *                   allocate/copy/insert a new buffer. If this returns FALSE,
 *		     the hello should NOT be sent!
 *  (*iidb_cleanup)() Called to do any PDM-specific IIDB cleanup before the
 *		     IIDB is destroyed.
 *  (*iidb_show_detail)()  Display PDM-specific IIDB details.
 *  (*input_packet)()  Fetch the next input packet.
 *  (*interface_change)()  Called to signal a change in interface metric.
 *  (*interface_up)()  Returns a protocol-specific view of whether an
 *		     interface is up or down.
 *  (*item_size)()   Returns the byte size of a route item.
 *  (*last_peer_deleted)()  Called for PDM-specific cleanup after the last
 *		     neighbor on an interface goes down.
 *  (*localaddr)()   Returns TRUE if the specified address belongs to this
 *		     system.
 *  (*mmetric_fudge)() Returns an amount to be added to the route metric.
 *  (*ndbbucket)()   Returns the NDB hash bucket for the destination.
 *  (*ndb_done)()    Called for PDM-specific processing before an NDB is
 *		     deleted.
 *  (*ndb_new)()     Called for PDM-specific processing when an NDB is created.
 *  (*netmatch)()    Compares two destinations.
 *  (*paklen)()	     Returns the data length of a received EIGRP packet.
 *  (*pdm_timer_expiry)()  Called when the PDM timer expires.
 *  (*peerbucket)()  Returns the peer hash bucket for an address.
 *  (*peershowhook)()  Called to do PDM-specific peer detail display.
 *  (*peerstatehook)()  Called to do PDM-specific peer validation.
 *  (*printaddr)()   return pointer to sprintf a igrp2 neighbor address 
 *                   printed in the protocol's native format.
 *  (*printnet)()    return pointer to sprintf a igrp2 network number
 *                   printed in the protocol's native format.
 *  (*rtdelete)()    Delete an entry from the routing table.
 *  (*rtgetmetric)() Return the best metric for this destination.  This
 *                   routine may be scrapped as well.  We can get the best
 *                   metric used in the routing table when we do an update.
 *                   This is done by comparing the currently believed best
 *                   metric in the routing table with the metric of the
 *                   nexthop gateway we just successfully installed.
 *                   Although it may still be required for sending update
 *                   query or reply packets.
 *  (*rtmodify)()    Notify routing table that subsequent update is to be 
 *                   taken regardless of metric.  This usually boils down
 *                   to a call to _rtdelete(), but is stubbed out in where
 *                   _rtupdate() can accept any information blindly.
 *  (*rtupdate)()    Update the routing table.  If the next hop information
 *                   matches information already in the routing table and
 *                   it is the only next hop for this destination, blindly
 *                   overwrite new information with old regardless of the
 *                   offered metric.  Return true if the update
 *                   was successful.
 *  (*sendpacket)()  Called to write the specified packet to the
 *                   specified peer. Peer address will be NULL if the
 *                   packet should be broadcast.
 *  (*transport_ready)()  Called by the DUAL when a peer or interface
 *		     becomes flow-control ready.  Guaranteed to be called
 *		     only from the routing process thread.  The interface
 *		     pointer is always non-NULL.  The peer pointer is NULL
 *		     when an interface becomes flow-ready for multicast, and
 *		     is non-null when a peer becomes flow-ready for unicast.
 *		     If the pakdesc is non-null, it points to a packet that
 *		     has just been acknowledged by the peer (peer will always
 *		     be non-NULL if so).  info_pending is TRUE if DUAL has
 *		     information to send.  This callback provides a hook for
 *		     the PDM to add packets to the transmit flow, and to
 *		     clean up after packets that it added are acknowledged.
 */
struct ddbtype_ {
    struct ddbtype_ *next;      /* Pointer to next ddb */
    char *name;                 /* Name of protocol specific user of ddb */
    int index;                  /* Index into dual_ddbtable */
    enum LINK linktype;		/* Linktype for this protocol */
    ulong packet_overhead;	/* Overhead, in bytes, for this protocol */
    pid_t hello_pid;		/* Process ID of Hello process */
    watched_boolean *run_hello_process; /* Hello process parks here */
    watched_queue *ddb_inputQ;	/* Queue of packets to process */
    ulong ddb_maxQdepth;	/* High water mark for the queue */
    igrp2_xmit_thread *ddb_thread_head; /* Head of transmit DNDB thread */
    igrp2_xmit_thread **ddb_thread_tail; /* Tail of transmit DNDB thread */
    igrp2_idbtype *quiescent_iidbs; /* List of quiescent interfaces */
    ulong ddb_next_serno;	/* Next serial number to use */
    dual_ndbtype *ddb_change_queue; /* Thread of newly-changed DNDBs */
    dual_ndbtype **ddb_change_queue_tail;  /* Tail of change queue */
    ulong ddb_Qdrops;		/* Count of packet drops */
    igrp2_idbtype *nulliidb;	/* IIDB for Null0 */
    uchar k1, k2, k3, k4, k5;   /* K-values transmitted in hellos */
    uchar max_hopcount;		/* Hopcount for infinity metric */
    uchar def_split_horizon:1;  /* Do we split horizon by default? */
    uchar debug_target_set:1;   /* Is the following structure setup? */
    uchar debug_peer_set:1;	/* Same as above, but for peer filtering */
    uchar ddb_logging_enabled:1;/* Event logging is enabled */
    uchar dual_logging:1;	/* Logging DUAL events */
    uchar dual_xport_logging:1; /* Logging transport events */
    uchar dual_xmit_logging:1;  /* Logging transmission events */
    uchar kill_everyone:1;	/* Mass suicide if an SIA occurs */
    uchar ddb_going_down:1;	/* Process is dying */
    uchar log_adj_change:1;	/* Logging peer changes */
    ulong asystem;              /* AS number used to store in packets */
    ulong send_seq_number;      /* Last sequence number used to send packet */
    void *pdb;                  /* Pointer to pdb */
    queuetype iidbQ;            /* Interfaces this ddb is configured on */
    queuetype temp_iidbQ;       /* Used to hold config info before if enabled*/
    queuetype passive_iidbQ;	/* Passive interfaces for this ddb */
    ulong ddb_active_if_count;	/* # of interfaces with active peers */
    dual_peertype **peer_array; /* Array of peer pointers, indexed by handle */
    ulong peer_array_size;	/* Number of entries in peer array */
    igrp2addrtype routerid;     /* ID of this router */
    ulong scratch_table_size;	/* Number of entries in scratch table */
    dual_iidb_scratch *ddb_scratch; /* Scratchpad for dual_routeupdate */
    igrp2nettype debug_target;  /* Used for filtering debugging output */
    igrp2addrtype debug_peer;   /* Used for filtering debugging output */
    ulong active_time;          /* Maximum age of an active route */
    ulong router_id;		/* Router ID from which we heard this */
    mgd_timer ddb_master_timer;	/* Master timer for the DDB */
    mgd_timer ddb_intf_timer;	/* Parent for all interface timers */
    mgd_timer ddb_peer_timer;   /* Grandparent for all peer timers */
    mgd_timer ddb_active_timer;	/* Timer for checking SIA routes */
    mgd_timer ddb_hello_timer;	/* Parent for all hello timers */
    mgd_timer ddb_acl_change_timer; /* Delay after access list change */
    mgd_timer ddb_pdm_timer;	/* Timer for the PDM */
    chunk_type *extdata_chunk;	/* Chunks for external data */
    chunk_type *workq_chunk;	/* Chunks for the work queue */

    boolean (*accept_peer)(struct ddbtype_ *, igrp2addrtype *,
			   igrp2_idbtype *);
    ulong (*add_item)(struct ddbtype_ *, igrp2_idbtype *, dual_ndbtype *,
		      void *, ulong, boolean);
    ulong (*addrcopy)(igrp2addrtype *, uchar *);
    boolean (*addrmatch)(igrp2addrtype *, igrp2addrtype *);
    advertise_request_type (*advertise_request)(struct ddbtype_ *,
						igrp2nettype *, idbtype *);
    igrp2type *(*buildpacket)(struct ddbtype_ *, dual_peertype *,
			      igrp2_pakdesc *, boolean *);
    boolean (*compare_external)(void *, void *);
    void (*enable_process_wait)(struct ddbtype_ *);
    void (*exterior_check)(struct ddbtype_ *, dual_ndbtype *, uchar *);
    uchar (*exterior_flag)(dual_ndbtype *);
    igrp2type *(*headerptr)(paktype *);
    boolean (*hellohook)(struct ddbtype_ *, igrp2type *, int *, idbtype *,
			 uchar *);
    void (*iidb_cleanup)(struct ddbtype_ *, igrp2_idbtype *);
    void (*iidb_show_detail)(struct ddbtype_ *, igrp2_idbtype *);
    paktype *(*input_packet)(struct ddbtype_ *, igrp2addrtype *);
    void (*interface_change)(ddbtype *, igrp2_idbtype *);
    boolean (*interface_up)(idbtype *);
    ulong (*item_size)(dual_ndbtype *);
    void (*last_peer_deleted)(struct ddbtype_ *, igrp2_idbtype *);
    boolean (*localaddr)(struct ddbtype_ *, igrp2addrtype *, idbtype *);
    ulong (*mmetric_fudge)(struct ddbtype_ *, igrp2nettype *, idbtype *, int);
    int (*ndbbucket)(igrp2nettype *);
    void (*ndb_done)(dual_ndbtype *);
    void (*ndb_new)(dual_ndbtype *, idbtype *);
    boolean (*netmatch)(igrp2nettype *, igrp2nettype *);
    ulong (*paklen)(paktype *);
    void (*pdm_timer_expiry)(struct ddbtype_ *, mgd_timer *);
    int (*peerbucket)(igrp2addrtype *);
    void (*peershowhook)(struct ddbtype_ *, dual_peertype *);
    void (*peerstatehook)(struct ddbtype_ *, dual_peertype *, boolean);
    char *(*printaddr)(igrp2addrtype *);
    char *(*printnet)(igrp2nettype *);
    void (*rtdelete)(struct ddbtype_ *, dual_ndbtype *, dual_rdbtype *);
    ulong (*rtgetmetric)(igrp2nettype *, struct ddbtype_ *);
    void (*rtmodify)(struct ddbtype_ *, dual_ndbtype *, dual_rdbtype *);
    boolean (*rtupdate)(struct ddbtype_ *, dual_ndbtype *, dual_rdbtype *,
			boolean *);
    void (*sendpacket)(igrp2type *, int, dual_peertype *, igrp2_idbtype *,
		       boolean);
    void (*transport_ready)(struct ddbtype_ *, igrp2_idbtype *,
			    igrp2_pakdesc *, dual_peertype *, boolean);
    void (*set_mtu)(igrp2_idbtype *);
    ulong (*extract_tag)(dual_rdbtype *); /* Get admin tag value */
    dual_peertype *peercache[NETHASHLEN]; /* Hash into peers we've heard */
    igrp2_handle_type ddb_handles;	/* Array of peer handle bits */
    dual_ndbtype *topology[NETHASHLEN]; /* Hash into topology table */
    igrp2_mibtype mib;                  /* MIB variables */
    ulong max_events;			/* Size of event buffer */
    dual_peertype *peer_list;		/* Full list of all peers */
    dual_event_type *events;		/* DUAL event log */
    dual_event_type *event_curptr;	/* Next entry for event log */
    dual_event_type *sia_events;	/* DUAL event log from last SIA */
    dual_event_type *sia_eventptr;	/* Pointer to event log from SIA */
    boolean log_events;			/* TRUE if log from evt Q per-event */
    ulong event_count;			/* serial # of event */
};

#define DUAL_EXTDATA_CHUNKSIZE 50	/* Extdata chunk block size */

/*
 * Dual event types.
 */
#define DUAL_UPDATE	0	/* Update packet */
#define DUAL_QUERY	1	/* Query packet */
#define DUAL_REPLY	2      	/* Reply packet */
#define DUAL_RTABLE	3       /* Routing table change (redist. case) */
#define DUAL_LINKDOWN	4       /* Link to neighbor has gone down */

/*
 * IDB linkage
 *
 * An thread of these is tied to the IDB (actually it's in the ipinfo field).
 * This provides a quick way to find the IIDB given the IDB.
 */
struct igrp2_idblink_ {
    struct igrp2_idblink_ *link_next;	/* Next one in thread */
    ddbtype *link_ddb;			/* DDB of owning process */
    igrp2_idbtype *link_iidb;		/* IIDB associated with this IDB */
};

/*
 * Generic TLV type used for packet decoding.
 */
typedef struct igrp2_gentlvtype_ {
    IGRP2TLMACRO
} igrp2_gentlvtype;

/*
 * flag field definitions
 */
#define IGRP2_EXTERNAL   0x01   /* Consider this from an external source */
#define IGRP2_CD         0x02   /* Candidate default route */

typedef struct igrp2_paramtype_ {
    IGRP2TLMACRO
    uchar k1;                   /* Bandwidth factor */
    uchar k2;                   /* Load factor */
    uchar k3;                   /* Delay factor */
    uchar k4;                   /* Reliability bias */
    uchar k5;                   /* Reliability factor */
    uchar pad;                  /* Pad, set to zero, ignored on receipt */
    ushort holdtime;            /* How long we ignore lost hello's */
} igrp2_paramtype;
#define IGRP2_PARAM_TLV_LENGTH (IGRP2_TLV_HDR_LENGTH + 8)

typedef struct igrp2_versiontype_ {
    IGRP2TLMACRO
    igrp2_sw_version version;
} igrp2_versiontype;
#define IGRP2_VERSION_TLV_LENGTH (IGRP2_TLV_HDR_LENGTH + IGRP2_SW_VERSION_SIZE)

/*
 * TLV for multicast sequence number to go with sequence TLV.
 *
 * This was added to the hello containing the sequence TLV so that the
 * hello packet could be more tightly bound to the multicast packet bearing
 * the CR bit that follows it.  The sequence number of the impending multicast
 * is carried herein.
 */
typedef struct igrp2_next_multi_seq_type_ {
    IGRP2TLMACRO
    ulong seq_num;		/* Sequence number of associated packet */
} igrp2_next_multi_seq_type;
#define IGRP2_MULTISEQ_TLV_LENGTH (IGRP2_TLV_HDR_LENGTH + 4)


/*
 * The folowing typedef is for decoding the igrp2 metric from a 
 * packet.
 */
typedef struct igrp2_mdecode_ {
    IGRP2METRICPAKMACRO
} igrp2_mdecode;

/*
 * Probe packet data format.  No TLVs for simplicity.
 */
typedef struct igrp2_probe_type_ {
    ulong probe_sequence;		/* Sequence number */
} igrp2_probe_type;
#define IGRP2_PROBE_SIZE 4		/* Length of data */


#define IGRP2HEADERBYTES sizeof(igrp2type)
#define MAXIGRP2BYTES    1500
#define IGRP2HELLOHDRBYTES sizeof(igrp2_paramtype) + sizeof(igrp2_versiontype)
/*
 * Kludge space for AT-EIGRP cablerange TLV, pending reworking of how the
 * hello tlvs are constructed.
 */
#define IGRP2HELLOHOOKBYTEKLUDGE (sizeof(igrp2_gentlvtype) + 2*sizeof(ulong))
#define IGRP2_KBYTES 5			/* Length of k value list in packets */

/*
 * Maximum # of oos packets we will tolerate from a neighbor while in init
 * state.  If we receive more than these, the neighbor must wrongly
 * think we are not in init state.
 */
#define IGRP2_OOS_MAX 3

/*
 * Maximum time to delay sending a hello after sending an update with
 * init bit set.
 */

#define IGRP2_INIT_HELLO_DELAY (2 * ONESEC)

/*
 * Extern defintions for global variables.
 */
extern queuetype ddbQ;
extern ulong igrp2_packet_debug_flags;
extern ulong igrp2_xmit_debug_flags;
extern chunk_type *igrp2_anchor_chunks;
extern chunk_type *igrp2_dummy_chunks;
extern chunk_type *igrp2_inpak_chunks;

typedef void  (*igrp2_peerfunction)(struct ddbtype_ *, dual_peertype *,
				    void *);

/*
 * Externs for functions in dual.c
 */
extern void dual_rcvpkt(ddbtype *, dual_peertype *, dual_newroute *, ulong);
extern void dual_peerdown(ddbtype *, dual_peertype *);
extern void dual_conn_rtchange(ddbtype *, dual_newroute *, dual_rtev_type);
extern void dual_rtchange(ddbtype *, dual_newroute *, dual_rtev_type);
extern void dual_lostroute(ddbtype *, igrp2nettype *);
extern void dual_sendupdate(ddbtype *, dual_ndbtype *, uchar *);
extern void dual_validate_route(ddbtype *, igrp2nettype *);
extern void dual_reload_proto_table(ddbtype *ddb);
extern boolean dual_getnext(ddbtype *, int *, ulong *, boolean *, boolean);
extern dual_ndbtype *dual_ndblookup(ddbtype *, igrp2nettype *);
extern dual_ndbtype *dual_table_walk(ddbtype *, dual_ndbtype *, int *);
extern char *dual_query_origin_str(dual_qo_t qo);
extern void dual_cleanup(ddbtype *);
extern void dual_scan_active(ddbtype *);
extern dual_rdbtype *dual_routelookup(ddbtype *, igrp2nettype *,
				      igrp2_idbtype *, igrp2addrtype *);
extern void dual_allocate_free_lists(void);
extern boolean dual_init_ddb(ddbtype *, char *, ulong, ulong);
extern void dual_log_event(ddbtype *ddb, dual_event_code, void *, void *);
extern void dual_show_events(ddbtype *, int, ulong, ulong);
extern void dual_xmit_continue(ddbtype *, igrp2_idbtype *, igrp2_pakdesc *,
			       dual_peertype *);
extern void dual_new_peer(ddbtype *, dual_peertype *);
extern igrp2type *dual_buildpacket(ddbtype *, igrp2_idbtype *, dual_peertype *,
				   igrp2_pakdesc *, boolean *);
extern void dual_last_peer_deleted(ddbtype *, igrp2_idbtype *);
extern void dual_first_peer_added(ddbtype *, igrp2_idbtype *);
extern void dual_unanchor_entry(ddbtype *, igrp2_xmit_thread *);
extern boolean dual_init_events (ddbtype *, ulong, boolean);
extern void dual_clear_event_logging(ddbtype *);
extern void dual_clear_log(ddbtype *);
extern void dual_adjust_scratchpad(ddbtype *);
extern void dual_sia_copy_log(ddbtype *);
extern void dual_remove_iidb_from_quiescent_list(ddbtype *, igrp2_idbtype *);
extern void dual_finish_update_send_guts(ddbtype *);
extern void dual_ifdelete(ddbtype *, igrp2_idbtype *);
extern void dual_configure_logging(parseinfo *, ddbtype *, boolean, boolean,
				   boolean);

/*
 * Externs for functions in igrp2.c
 */
extern paktype *igrp2_getpak(int);
extern igrp2type *igrp2_getbuf(ushort);
extern int igrp2_findhandle(ddbtype *, igrp2addrtype *, idbtype *);
extern ulong igrp2_cdelay(ulong);
extern void igrp2_metric_decode(igrp2metric *, igrp2_mdecode *);
extern void igrp2_metric_encode(ddbtype *, dual_ndbtype *, igrp2_idbtype *, 
				igrp2_mdecode *, boolean, boolean);
extern ulong igrp2_compute_metric(ddbtype *, igrp2metric *, igrp2nettype *,
				  igrp2_idbtype *, ulong *);
extern char *igrp2_otype(int);
extern int igrp2_dual2opc(int);
extern dual_peertype *igrp2_receive(ddbtype *, paktype *, igrp2addrtype *, 
				    idbtype *);
extern dual_peertype *igrp2_find_peer(ddbtype *, igrp2addrtype *, idbtype *);
extern igrp2_pakdesc *igrp2_enqueue(ddbtype *, dual_peertype *,
				    igrp2_idbtype *, boolean);
extern void igrp2_show_neighbors(ddbtype *, idbtype *, boolean, boolean);
extern void igrp2_show_interface(ddbtype *, idbtype *, boolean);
extern void igrp2_show_topology(ddbtype *, int);
extern void igrp2_show_timers(ddbtype *);
extern igrp2_idbtype *igrp2_find_iidb(ddbtype *, idbtype *);
extern igrp2_idbtype *igrp2_find_temp_idb(ddbtype *, idbtype *);
extern void igrp2_free_ddb(ddbtype *);
extern boolean igrp2_allocate_iidb(ddbtype *, idbtype *, boolean);
extern void igrp2_deallocate_iidb(ddbtype *, igrp2_idbtype *);
extern igrp2_idbtype *igrp2_create_iidb(ddbtype *, idbtype *);
extern void igrp2_take_nbrs_down(ddbtype *, idbtype *, boolean, char *);
extern void igrp2_take_all_nbrs_down(ddbtype *, char *);
extern void igrp2_take_peer_down(ddbtype *, dual_peertype *, boolean, char *);
extern void igrp2_destroy_peer(ddbtype *, dual_peertype *);
extern void igrp2_flush_peer_packets(ddbtype *, dual_peertype *);
extern void igrp2_set_bandwidth_percent(ddbtype *, idbtype *, ulong);
extern void igrp2_set_hello_interval(ddbtype *, idbtype *, ulong);
extern void igrp2_set_holdtime(ddbtype *, idbtype *, ulong);
extern void igrp2_set_holddown(ddbtype *, idbtype *, ulong);
extern void igrp2_set_split_horizon(ddbtype *, idbtype *, boolean);
extern void igrp2_update_mib(ddbtype *, igrp2type *, boolean);
extern void igrp2_show_traffic(ddbtype *);
extern void igrp2_for_each_peer(ddbtype *, igrp2_peerfunction, void *);
extern void igrp2_debug_command(parseinfo *);
extern boolean igrp2_process_managed_timers(ddbtype *);
extern boolean igrp2_default_timers(igrp2_idbtype *);
extern void igrp2_probe_peer(ddbtype *, dual_peertype *, boolean);
extern void igrp2_probe_interface(ddbtype *, igrp2_idbtype *, boolean);
extern boolean igrp2_init_hello_process (ddbtype *, char *);
extern boolean igrp2_peer_is_flow_ready(dual_peertype *);
extern boolean igrp2_interface_is_flow_ready(igrp2_idbtype *);
extern dual_peertype *igrp2_handle_to_peer(ddbtype *, int);
extern void igrp2_flush_iidb_xmit_queues(ddbtype *, igrp2_idbtype *);
extern boolean igrp2_xmit_debug_ok(ulong);
extern void igrp2_clear_handle(ddbtype *ddb, igrp2_handle_type *, int);
extern boolean igrp2_test_handle(ddbtype *, igrp2_handle_type *, int);
extern void igrp2_freehandle(ddbtype *, dual_peertype *peer);
extern void igrp2_bugpak(dual_peertype *, igrp2type *, igrp2_pakdesc *,
			 char *, ddbtype *, idbtype *, igrp2_idbtype *,
			 igrp2addrtype *, ushort);
extern void igrp2_passive_interface(ddbtype *, igrp2_idbtype *, boolean);
extern void igrp2_set_debug_ddb(ddbtype *);
extern void igrp2_process_starting(ddbtype *);
extern void igrp2_start_acl_timer(ddbtype *, idbtype *);

/*
 * igrp2_debug.c
 */
extern void igrp2_debug_init(void);

/*
 * igrp2_chain.c
 */
extern void igrp2_parser_init(void);

/*
 * Debug macro definitions.
 */
#define IGRP2_FBUGINF  if (igrp2_fsm_debug) buginf
#define IGRP2_FTARGET(target) (igrp2_fsm_debug ? (ddb->debug_target_set ? (*ddb->netmatch)((target), &ddb->debug_target) : TRUE) : FALSE)
#define IGRP2_FABUGINF(target) if (IGRP2_FTARGET(target)) buginf
#define IGRP2_XBUGINF(flags, parms...) \
    if (igrp2_xmit_debug && igrp2_xmit_debug_ok(flags)) buginf( ## parms)


#endif __DUAL_H__
