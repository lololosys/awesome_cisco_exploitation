/* $Id: isis.h,v 3.6.12.10 1996/09/12 00:17:13 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis.h,v $
 *------------------------------------------------------------------
 * isis.h -- ISO IS-IS definitions
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the IS-IS specific definitions and constants.
 *------------------------------------------------------------------
 * $Log: isis.h,v $
 * Revision 3.6.12.10  1996/09/12  00:17:13  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.6.12.9  1996/08/11  15:13:06  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.6.12.8  1996/08/02  02:47:50  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.6.12.7  1996/06/18  16:50:41  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.6.12.6  1996/05/26  06:45:57  dkatz
 * CSCdi58737:  ISIS/NLSP debug messages can be misleading
 * Branch: California_branch
 * Issue debug messages before transmitting packets.  Add instrumentation
 * for retransmissions.
 *
 * Revision 3.6.12.5  1996/05/26  06:32:10  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.6.12.4  1996/05/10  19:45:36  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.6.12.3  1996/05/07  17:15:53  dkatz
 * CSCdi54576:  ISIS LSP pacing rate needs to be configurable
 * Branch: California_branch
 * Provide a knob to alter the LSP pacing timer;  the timer is already
 * there.
 *
 * Revision 3.6.12.2  1996/04/21  20:47:31  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.6.12.1  1996/04/12  00:36:27  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/20  18:06:35  dkatz
 * CSCdi47916:  ISIS/NLSP LSP checksum errors
 * Improve diagnostics.  Ignore errors if the holdtime is zero
 * (workaround).
 *
 * Revision 3.5  1995/12/01  04:09:39  dkatz
 * CSCdi37675:  ISIS/NLSP can retransmit early
 *
 * Revision 3.4  1995/12/01  02:48:23  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:53:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:52:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/23  23:14:07  dkatz
 * CSCdi39776:  ISIS can mislink backup chain
 * Dont leave a backup chain dangling from the heap.
 * Make sure that we run SPF when the second level adjacency comes up
 * if L1L2.
 *
 * Revision 2.4  1995/09/23  21:37:35  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.3  1995/08/25  14:59:45  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.2  1995/08/23  18:25:59  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.1  1995/06/07  20:18:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************************

	      A feeble attempt at Internal Documentation

ISIS and NLSP (the IPX almost-version of ISIS) share a single common
code base.  They are differentiated by pdb->proc_type.  They may be
running at the same time, so static variables are almost always wrong.

ISIS (I'll include NLSP by implication from here on) consists of two processes.
The Router process demultiplexes incoming packets, and generates and
processes Hello packets.  This is done as a separate process so that the Update
process can spend "arbitrary" amounts of time calculating routes without
worrying about adjacencies being dropped.

The Update process sends and receives LSPs and calculates routes.


			  Route Calculation

The heart of route calculation is Dijkstra's Shortest Path First (SPF)
algorithm.  This is a fairly expensive calculation, so we do a number of
things to keep the cost down.

The first thing we do is to only calculate SPF across the routers in
the network--the leaf routes are not nodes in the graph.  There is one
significant side effect of this, however, which is that although the
SPF algorithm will generate routes in order by worsening metric, the
leaf routes generated may not necessarily come out in order, since
although paths to the *routers* are coming out in order, the leaf
routes are generated by adding the arbitrary last hop metric (from the
advertising router to the leaf), so they will emerge in no particular
order;  in fact, multiple paths to the same destination may appear in
any order for this reason.

Once an SPF calculation has been done, the key results of the calculation
are saved, namely, the path length from the local router to each router,
and the next hop(s) used to reach each router.  Note that an "SPF tree"
is never actually generated, but only the distance and next hop.  This
information is kept in the LSP database.  It is not necessary to do an
SPF calculation when leaf routes change, but only when information changes
that may impact the paths to the other routers (such as a change in
an inter-router adjacency, for instance).

Originally, a full SPF was done whenever *anything* changed;  as of this
writing, NLSP and ISIS-IP now no longer require a full SPF (CLNS routes
still do).  To do SPF avoidance, additional state must be kept (as we
will discuss shortly).

Route calculations are typically triggered by the installation of a new
LSP (either one we received from the outside, or one of our own that we've
installed--the algorithm does not discriminate between the two).  The
old and new LSPs are compared, and one of three actions will result:

	No action (the LSPs are identical)
	Full SPF required
	Partial route calculation required

A partial route calculation takes place when something changes in an LSP
that does not affect the inter-router topology (such as a change in a
leaf route).  When this occurs, the LSP fragment that changed is
reevaluated.  The path to the router that issued the LSP fragment is
known from the last SPF run, so essentially the LSP fragment is walked
and each leaf route is installed (using the next hop(s) to the router,
and with a metric equal to the path length to the router plus the additional
path cost to the leaf route).  Each installed route is marked with the
"version" number of the LSP fragment (which is incremented at the beginning
of the process);  after all routes are installed, any routes that have
an old version number of this LSP are deleted (since they apparently
disappeared from the LSP).

For CLNS routes, a full SPF run is always done when things change, and
an "SPF version" number is kept for each route, with the same effect
(albeit at a much greater cost).


			    Backup Routes

When SPF avoidance is done, care must be taken to keep track of worse-cost
routes.  Otherwise, if the best-cost route was lost, it would be necessary
to run a full SPF to find the next best route to the destination.

ISIS tracks this by creating a "backup route" when a route is calculated
that turns out not to be the best one.  In order to save space, rather
than keeping an entire route, a pointer back to the LSP that was responsible
for the backup route is kept;  if the best cost route is lost, a partial
route calculation is done on the LSP indicated as a backup, which will most
likely reinstall the route at a worse metric (and perhaps with a different
next hop).

In some cases there may be very many potential backup paths to a destination.
To avoid consuming large amounts of memory and CPU, only the paths with the
2nd and 3rd best metrics are tracked, and any worse paths are discarded.
If a path is discarded, a special backup entry (an "overflow" entry)
is created to mark this case;  if all other backup paths are used and the
overflow entry is encountered, a full SPF run will be done to find the
other backup paths, if any.

All backup entries created from a particular LSP are flushed when that
LSP undergoes a route calculation (and most likely some will reappear
in the course of attempting to install the routes in the LSP).


			   Data Structures

All of this takes some fairly hairy data structures.

The LSP database is kept in an AVL tree, sorted by LSP ID.  This sorting
is useful for doing a database lookup (which is a log N operation), and
is also useful for generating and processing sequence number packets,
whose contents are always sorted in LSP ID order.

When an LSP fragment is installed in the database, it is given an LSP
Index value.  This is a compact (16 bit) handle used for internal
tracking purposes.  Semantically it is identical to the LSP ID.  An
array, indexed by the LSP Index, is maintained, which contains a
pointer to the corresponding LSP database entry, as well as some other
state information.  Each array entry is known as a "lspix entry".
This allows very rapid (O(1)) access to a particular LSP.

Each lspix entry has two chains for each protocol that supports
partial route calculations (currently IP, IPX routes, and IPX services).
One chain threads through each of the routes installed from this LSP
fragment (the linkage is embedded in the native route table entry).
This chain is used to keep track of which routes were generated by the
LSP, so that when the LSP is "aged" at the end of a partial route calculation,
the routes that have been deleted will be found on this chain (with an
obsolete LSP version number) and can be easily deleted.

The other chain threads through the backup entries generated by this LSP.
This is used to allow all backup entries to be purged when the LSP is
reevaluated.

Backup entries are actually dually threaded.  The "vertical" thread links
all backup entries generated from a particular LSP, as just mentioned.
The "horizontal" thread links all backup entries pertaining to a
particular route, sorted in increasing metric order.  This allows rapid
access to the backup entries for this route;  the LSPs indicated in the
backup entry will be marked for reevaluation, which will cause the backup
routes to be installed if they are still viable.

Things are even slightly more complicated for ISIS-IP.  If the active route
is an ISIS route, the backup thread is linked directly to the NDB.  However,
if the active route is from another protocol with better administrative
distance, the backup thread is linked to the "backuptype" entry that the
IP code creates for each protocol with a worse distance that attempted to
install a route.


			    Cleanup Queue

This is a pretty ugly hack, but a necessary one.

Because of the loosely coupled nature of the implementation, it is
fairly common to have a situation where an active route is deleted,
backup entries are present, but the backup entries have not yet been
evaluated (because LSP evaluation is scheduled rather than called
directly).  In such a case there is a need to keep state for the
deleted route (so that the rest of the backup entries have somewhere
to hang from).  This is done by deleting all of the next hops, but
keeping the "null" route in the routing table.  If in fact one of the
backup entries is still viable, the "null" route is updated with the
new information and life moves on.  However, if all of the backup
routes turn out to be duds, the "null" route will be stranded, because
nothing will point at it any longer.

The way around this is that a "cleanup queue" entry is created whenever
a route is deleted but still has backup entries to be evaluated.  The
cleanup queue entry contains the route in question (in terms of address
information, rather than any kind of a pointer).  After the backup entries
have all been evaluated, the cleanup queue is examined.  A route lookup
is done on each entry in the queue;  if the route is still "null" it
is deleted.

*************************************************************************/





#ifndef __ISIS_H__
#define __ISIS_H__

#include "../clns/clns_adj.h"
#include "../clns/isis_nlsp_debug.h"

#define ISIS_MAX_QUEUE_DEPTH       200      /* Depth of update process queue */
#define ISIS_PACKET_LIMITER        20
#define ISIS_TIMER_LIMITER         (ISIS_PACKET_LIMITER/2)
#define ISIS_JITTER_PERCENT        25       /* Jitter timers by this much */
#define ISIS_HT_MULTIPLIER         3
#define ISIS_DEF_HELLO_IVL         10*ONESEC
#define ISIS_LAN_DR_HELLO_DIVISOR  3
#define ISIS_DEF_CSNP_TIMER        10*ONESEC
#define ISIS_DEF_RETRANS_TIMER     5*ONESEC
#define ISIS_AGE_INTERVAL          10*ONESEC	/* Interval for ager */
#define ISIS_NLSP_SPF_INITIAL_WAIT ((5*ONESEC)+(ONESEC/2)) /* Wait before first SPF */
#define ISIS_MAX_INTERFACES	   255	     /* Max # IDB's for ISIS or NLSP */
#define ISIS_P2P_CSNP_WAIT	   5*ONESEC  /* Wait for LSPs on LAN first */
#define ISIS_P2P_LSP_WAIT	   3*ONESEC  /* Wait for CSNPs to go first */
#define ISIS_LSP_BUILD_DELAY	2*ONESEC /* Wait before building LSPs */
#define ISIS_LSP_REFRESH_OFFSET (30*ONESEC) /* Amount less than lifetime */
#define NLSP_LAN_DR_HELLO_DIVISOR  2

/*
 * The default designated router priority and metric. 
 */
#define ISIS_DEF_DR_PRIO 64
#define NLSP_DR_PRIO_RAISE 20		/* DR raises its priority this much */
#define ISIS_DEF_METRIC  10

/*
 * Maximum values allowed.
 */
#define ISIS_MAX_PRIO        0x7f
#define ISIS_MAX_METRIC      0x3f
#define ISIS_MAX_PATH_METRIC 1024	/* Actually max + 1 */
#define ISIS_MAX_QOS         4
#define ISIS_MAX_PATHS       MAXROUTES

/*
 * Duplicate system ID detection values.  We measure how often we regenerate
 * our own non-pseudonode LSPs (either level).  If we do it more than
 * ISIS_DUPID_COUNT times in ISIS_DUPID_INTERVAL, we declare there to be
 * a duplicate.
 */
#define ISIS_DUPID_COUNT 6		/* Max count per interval */
#define ISIS_DUPID_INTERVAL (5 * ONEMIN)  /* Interval time */

/*
 * The possible values for the 'code' field in the packets.
 */
#define ISIS_AREA_ADDRESSES   1
#define ISIS_IS_NEIGHBORS     2
#define ISIS_ES_NEIGHBORS     3
#define ISIS_L2_PART_DR       4
#define ISIS_PREFIX_NEIGHBORS 5
#define ISIS_IS_NEIGHBORS_IIH 6
#define ISIS_PAD              8
#define ISIS_SNP              9
#define ISIS_AUTHENTICATION   10
#define ISIS_NLPID            129
#define ISIS_AUTH_PW_TYPE     1

/*
 * IP specific values for the 'code' field.
 */
#define ISIS_IP_INTERNAL_ADDR 128
#define ISIS_IP_EXTERNAL_ADDR 130
#define ISIS_IDR_INFO         131
#define ISIS_IP_IF_ADDRESSES  132

/*
 * IPX/NLSP specific values for the 'code' field.
 */
#define ISIS_NLSP_AREA_ADDRS  0xC0
#define ISIS_NLSP_MGMT_INFO   0xC1
#define ISIS_NLSP_LINK_INFO   0xC2
#define ISIS_NLSP_SVCS_INFO   0xC3
#define ISIS_NLSP_EXT_ROUTES  0xC4
#define ISIS_NLSP_LOCAL_MTU   0xC5
#define ISIS_NLSP_SUMMARY_ROUTES 0xC6

/*
 * Protocol-independent extensions for the 'code' field.
 */
#define ISIS_SERIAL_CIRC_STATE 0xF0
#define ISIS_SERIAL_CIRC_STATE_LENGTH 1	/* A single byte of state */

#define ISIS_NUM_CODES_POSSIBLE 256	/* Number of TLV codes */

#define ISIS_VERSION        1
#define NLSP_VERSION        2
#define ISIS_VERSION_LSP    1
#define ISIS_VERSION_HELLO  1

/*
 * Used for IS-type and circuit-type in PDUs as well as for
 * internal variables.
 *
 * Can be treated as bitfields;  L1L2 == L1 | L2.
 */
#define ISIS_CIRCUIT_NONE   0
#define ISIS_CIRCUIT_L1     CLNS_LEVEL1              /* Level 1 only */
#define ISIS_CIRCUIT_L2     CLNS_LEVEL2              /* Level 2 only */
#define ISIS_CIRCUIT_L1L2   CLNS_LEVEL1+CLNS_LEVEL2  /* Level 1 and Level 2 */
#define ISIS_NUM_CIRC_TYPES 4		     	     /* Number of types */

/* 
 * Codes for the various isis packet type fields.
 */
#define ISIS_LAN_L1_HELLO 15
#define ISIS_LAN_L2_HELLO 16
#define ISIS_PT_HELLO     17
#define ISIS_L1_LSP       18
#define ISIS_L2_LSP       20
#define ISIS_L1_CSNP      24
#define ISIS_L2_CSNP      25
#define ISIS_L1_PSNP      26
#define ISIS_L2_PSNP      27

/*
 * Miscellaneous defines.
 */
#define ISIS_LSPBUFFERSIZE	1492

#define ISIS_METRIC_UNSUPPORTED 128  /* high bit set */

#define ISIS_MINBCASTLSPGENINTERVAL 33
#define ISIS_DEF_PACING_INTERVAL    33
#define ISIS_CSNPINTERVAL           10*ONESEC    /* 10 seconds */

#define ISIS_ZEROAGE_LIFETIME       ONEMIN	 /* 1 minute */   
#define ISIS_SEQUENCE_MODULUS       (unsigned)0xffffffff

#define ISIS_PSEUDO_ID		    6            /* Offset to pseudo ID */

#define ISIS_LSP_CHECKSUM_LENGTH    2

#define	ISIS_INVALID_IP_NEXTHOP	0xfffffffe	 /* No usable IP addr in IIH */

/* IPX/NLSP Media types */

#define NLSP_TYPE_GENERIC_LAN	0x0000	/* Generic LAN */
#define NLSP_TYPE_GENERIC_WAN	0x8000	/* Generic WAN */
#define NLSP_TYPE_LOCALTALK	0x0001	/* Localtalk */
#define NLSP_TYPE_ETHERNET	0x0002	/* DIX Ethernet II */
#define NLSP_TYPE_802_3_LLC	0x0003	/* 802.3 + LLC, no SNAP */
#define NLSP_TYPE_802_3_RAW	0x0005	/* 802.3, no LLC */
#define NLSP_TYPE_802_3_SNAP	0x000A	/* 802.3 + LLC + SNAP */
#define NLSP_TYPE_802_5_LLC	0x0004	/* 802.5 + LLC, no SNAP */
#define NLSP_TYPE_802_5_SNAP	0x000B	/* 802.5 + LLC + SNAP */
#define NLSP_TYPE_ISDN		0x800D	/* ISDN */
#define NLSP_TYPE_IP_TUNNEL	0x0013	/* IP Tunnel */
#define NLSP_TYPE_IP_RELAY	0x8013	/* IP Relay */
#define NLSP_TYPE_FDDI_LLC	0x0014	/* FDDI + LLC, no SNAP */
#define NLSP_TYPE_FDDI_SNAP	0x0017	/* FDDI + LLC + SNAP */
#define NLSP_TYPE_PPP		0x801C	/* PPP */
#define NLSP_TYPE_X_25		0x801E	/* X.25 */
#define NLSP_TYPE_FRAME_RELAY	0x801F	/* Frame Relay */
#define NLSP_TYPE_WAN_MASK	0x8000	/* Set for WAN media */

/* NLSP costs to reach the ephemeral static pseudonode */

#define NLSP_STATIC_MTU 1500		/* Just like ethernet */
#define NLSP_STATIC_BANDWIDTH 10000000
#define NLSP_STATIC_DELAY 1000
#define NLSP_STATIC_COST 20

/*
 * LSP ID
 *
 * The standard eight byte LSP ID.  Note that the system_id field is sometimes
 * overloaded as a seven byte value (with selector/pseudo_id field).  This
 * is harmless.
 */
typedef struct isis_lspid_ {
    uchar system_id[STATION_LENGTH];	/* Station ID */
    uchar pseudo_id;			/* Pseudonode number */
    uchar lsp_number;			/* Fragment number */
} isis_lspid;
#define ISIS_LSPID_LENGTH 8		/* Size of this beast */
#define ISIS_MAX_LSP_FRAGMENT 0xff	/* Max value of fragment field */


/*
 * Generic IS IS packet type.
 *
 *    NOTE: this structure is only for the header fields that are in
 *    common with ALL ISIS packet types. The specific type should be
 *    used wherever possible.
 */
#define ISIS_PDU_HEADER_FIELDS \
    uchar pid;\
    uchar hlen;\
    uchar version_pid;\
    uchar id_length;\
    uchar non_routing:1;		/* NLSP only */\
    uchar reserved1:2;\
    uchar type:5;\
    uchar version_pdu;\
    uchar reserved2;\
    uchar max_area_addresses;		/* Reserved in NLSP */

typedef struct isishdrtype_ {
    ISIS_PDU_HEADER_FIELDS
} isishdrtype;
#define ISIS_MINIMAL_HEADER_SIZE   8

#define isisheadstart(ptr) ((ptr)->network_start)


/*
 * Serial IIH packet type.
 */
typedef struct isis_pt_IIH_hdrtype_ {
    ISIS_PDU_HEADER_FIELDS
    uchar reserved:4;
    uchar state:2;	      /* NLSP adjacency state (0 for ISIS) */
    uchar circuit_type:2;     /* Level 1, Level2 or both */
    uchar source_id[STATION_LENGTH];  /* PDU originator */
    uchar ht[2];              /* Number of seconds to remember hello */
    uchar pdu_length[2];      /* Length of entire pdu */
    uchar local_circuit_id;   /* Unique identifier for this interface */
    uchar item[1];            /* Data */
} isis_pt_IIH_hdrtype;
#define ISIS_PT_HELLO_HEADER_SIZE  20

/* Values for serial IIH state field taken from CLNS_ADJ_xx_STATE */

/*
 * LAN IIH packet type.
 */
typedef struct isis_lan_IIH_hdrtype_ {
    ISIS_PDU_HEADER_FIELDS
    uchar reserved:3;
    uchar no_multicast:1;      /* Set if system not multicast capable (NLSP) */
    uchar reserved_again:2;
    uchar circuit_type:2;      /* Level 1, Level2 or both */
    uchar source_id[STATION_LENGTH];  /* PDU originator */
    uchar ht[2];               /* number of seconds to remember hello */
    uchar pdu_length[2];       /* length of entire pdu */
    uchar prio;                /* lan designated router priority */
    uchar lan_id[SYSTEMID_LENGTH];
    uchar item[1];             /* Data */
} isis_lan_IIH_hdrtype;
#define ISIS_LAN_HELLO_HEADER_SIZE 27


/*
 * LSP core structure.  This is the data common to the LSP database
 * and the link state packet transmitted on the wire.
 *
 * There are assumptions in the code that the sequence_number field
 * starts at a fullword offset into the structure, beware!
 */
#define ISIS_LSP_CORE_STRUCT \
    isis_lspid PACKED(lspid);\
    ulong PACKED(sequence_number);    /* Do not align seq. num in struct */\
    uchar checksum[ISIS_LSP_CHECKSUM_LENGTH];\
    uchar p:1;                        /* Partition Repair supported */\
    uchar att:4;                      /* attached with which metrics */\
    uchar hippity:1;                  /* cost for router (0 or infinite) */\
    uchar is_type:2;                  /* level1 or level2 IS */\
    uchar item[0];                    /* start of variable length part */
#define ISIS_LSP_CORE_SIZE 15	      /* Size, excluding one byte of item */
#define ISIS_LSP_L2_ATTACHED 1	      /* Bit in "att" field to check */

/*
 * isis_lsp_core
 *
 * Core data structure for LSP database and packet, expressed as a
 * separate struct.  Be forewarned that inclusion of this struct ends
 * up aligned only because the one byte of "item" pads to a fullword
 * boundary!
 */
typedef struct isis_lsp_core_ {
ISIS_LSP_CORE_STRUCT
} isis_lsp_core;

/*
 * Link State PDU.  This is the packet header
 */
typedef struct isis_LSP_hdrtype_ {
    ISIS_PDU_HEADER_FIELDS
    uchar pdu_length[2];      /* entire pdu length (including header) */
    uchar ht[2];              /* number of seconds before LSP expires */
    ISIS_LSP_CORE_STRUCT
} isis_LSP_hdrtype;
#define isis_LSP_hdr_copy_start lspid	/* Place to copy to from LSP DB */
#define ISIS_LSP_HEADER_SIZE       27


/*
 * Complete sequence number PDU (csnp).
 */
typedef struct isis_CSNP_hdrtype_ {
    ISIS_PDU_HEADER_FIELDS
    uchar pdu_length[2];
    uchar source_id[SYSTEMID_LENGTH];
    isis_lspid PACKED(start_lsp);
    isis_lspid PACKED(end_lsp);
    uchar item[0];
} isis_CSNP_hdrtype;
#define ISIS_CSNP_HEADER_SIZE      33

typedef struct isis_snp_entry_ {
    uchar lifetime[2];
    isis_lspid PACKED(lspid);
    ulong PACKED(sequence_number);
    uchar checksum[2];
} isis_snp_entry;
#define ISIS_SNP_ENTRY_LENGTH 16


/*
 * Partial sequence number pdu (psnp).
 */
typedef struct isis_PSNP_hdrtype_ {
    ISIS_PDU_HEADER_FIELDS
    uchar pdu_length[2];
    uchar source_id[SYSTEMID_LENGTH];
    uchar item[0];
} isis_PSNP_hdrtype;
#define ISIS_PSNP_HEADER_SIZE      17


/*
 * Parent entry
 *
 * Each of these keeps track of our position in the world relative
 * our parent.  There are multiple of these per LSP db entry, corresponding
 * to each path.
 *
 * For example, the hop count kept for each path is the hop count as 
 * measured through each parent.
 *
 * The path cost is not kept here, since it is the same for all paths.
 */
typedef struct isis_lsp_parent_ {
    clns_adjtype *next_hop;		/* Next hop adjacency */
    ulong throughput;			/* Throughput in BPS (NLSP) */
    ulong delay;			/* Delay in usec (NLSP) */
    ulong mtu;				/* MTU in bytes (NLSP) */
    ulong hops;				/* Hop count (NLSP) */
    ulong ticks;			/* Ticks in jiffies (NLSP) */
} isis_lsp_parent;


/*
 * LSP database entry structure
 *
 * db_length is the size of the LSP, including this header
 * max_db_length is the max size of the LSP, including this header
 *
 * Note that the "LSP core" data structure is defined as an array of
 * zero elements at the end.  This is done so that sizing calculations
 * work out correctly.  A field in the core structure can be referenced
 * via lspdb->data->foo.
 *
 */
struct isis_lspdbtype_ {
    struct isis_lspdbtype_ *next_lsp;	/* Link word for miscellany */
    ulong        db_length;		/* Size of this LSP */
    ulong 	 max_db_length;		/* As big as this LSP can get */

    mgd_timer    lsp_timer;		/* Timer for holdtime and purge time */
    mgd_timer    *lsp_refresh_timer;	/* Pointer to refresh timer */

    struct isis_lspdbtype_ *next_tent;	/* Link in TENT list */
    struct isis_lspdbtype_ **prev_tent;	/* Back pointer in TENT list */

    /*
     * The following area is copied from an old LSP to a new LSP when that
     * new LSP is replacing the old in the LSP database.
     */
    uchar	 copy_start[0];		/* Start of area copied to new lsp */

    sys_timestamp next_gen;             /* Time stamp when next gen allowed */
    int          root_distance;		/* Distance from root in this node */
    int		 parent_count;		/* Number of parent entries */
    isis_lsp_parent parents[ISIS_MAX_PATHS]; /* Next hops to reach this node */
    ushort	 lsp_index;		/* LSP Index number */
    ushort	 parent_index;		/* LSP index of our parent */
    uchar	 db_on_paths:1;		/* TRUE if on the PATHS list */
    uchar	 db_on_tent:1;		/* TRUE if on the TENT list */

    uchar	 copy_end[0];		/* End of copy area */

    mgd_timer	 gen_limit_timer;	/* Timer for controlling gen limit */
    dynamic_bitfield SRM;		/* Send routing msg - bit per i/f */
    dynamic_bitfield SSN;		/* Send seq num msg - bit per i/f */
    dynamic_bitfield on_rexmit_queue;	/* On rexmit Q; bit per i/f */
    uchar	 level;			/* LSP level */
    uchar	 non_routing:1;		/* True if NLSP non-routing server */
    uchar	 lsp_active:1;		/* True if LSP not purged */
    uchar	 lsp_freed:1;		/* True if LSP has been released */
    uchar	 lsp_suppress:1;	/* Allow suppress if NO_CHANGE */
    uchar	 lsp_force_insert:1;	/* Force insert irregardless */

    isis_lsp_core data[0];		/* LSP core and data */
};

/*
 * Standard TLV structure
 */
typedef struct {
    uchar code;                       /* which info is described here */
    uchar length;                     /* how long is this info */
    uchar value[1];                   /* array of value */
} isis_info_type;

#define ISIS_INFO_TYPE_OVERHEAD 2     /* Overhead of above structure */
#define ISIS_INFO_TYPE_MAXDATA 0xff   /* Max size */

/*
 * Amount of overhead used by LSP DB entry, relative to the LSP packet
 * size.  The "sizeof" may give us extra overhead; so be it.
 */
#define ISIS_LSPDB_OVERHEAD (isis_lspdb_private_length() + \
	ISIS_LSP_CORE_SIZE - ISIS_LSP_HEADER_SIZE)

/*
 * ADVANCE_ITEM - advance the item pointer to the next TLV item.
 *
 * If the current length is zero, the pointer is not advanced unless
 * the "advance_if_zero" flag is set.  Note that the tests optimize
 * out if "advance_if_zero" is TRUE.
 */
#define ADVANCE_ITEM(itemptr, advance_if_zero) {\
    if ((advance_if_zero) || (itemptr)->length) {\
	(itemptr) = (isis_info_type *) ((uchar *) (itemptr) +\
					(itemptr)->length +\
					       ISIS_INFO_TYPE_OVERHEAD);\
    }\
}

/*
 * isis_item_length - calculate length of current item based on start and
 * end pointers
 *
 * Assumes that the TLV header is included in the block of information
 * (but according to spec is excluded from the length).
 */
static inline uchar isis_item_length (isis_info_type *item, void *end)
{
    return ((uchar *) end - (uchar *) item - ISIS_INFO_TYPE_OVERHEAD);
}

/*
 * isis_overall_item_length - calculate length of current item
 * (including overhead) based on item header.
 *
 * assumes that zero-length items are to be deleted!
 */
static inline ulong isis_overall_item_length (isis_info_type *item)
{
    if (item->length == 0)
	return(0);
    return(item->length + ISIS_INFO_TYPE_OVERHEAD);
}


#define ISIS_MIN_IS_AD_SIZE     12  
#define ISIS_MIN_ES_AD_SIZE     10
#define ISIS_MIN_PREFIX_AD_SIZE 27  /* largest prefix plus overhead */
#define ISIS_ONE_ES_AD_SIZE     6
#define ISIS_ONE_IS_AD_SIZE     11

/*
 * Structure of an ISIS metric advertisement in the packet.
 */
typedef struct isis_metric_type_ {
    uchar default_metric;
    uchar delay_metric;
    uchar expense_metric;
    uchar error_metric;
} isis_metric_type;

typedef struct isis_is_nbr_ad_ {
    isis_metric_type metric;
    uchar            neighbor_id[SYSTEMID_LENGTH];
} isis_is_nbr_ad;

/*
 * ES neighbor advertisement
 */
typedef struct isis_es_nbr_ad_ {
    isis_metric_type metric;
    uchar            neighbor_id[STATION_LENGTH];    /* neighbor_id repeats */
} isis_es_nbr_ad;

/*
 * Prefix neighbor advertisement
 */
typedef struct isis_prefix_nbr_ad_ {
    isis_metric_type metric;
    NSAP_T           prefix;    /* prefix repeats */
} isis_prefix_nbr_ad;

/*
 * IP Reachability Information
 */
typedef struct isis_ip_nbr_ad_ {
    isis_metric_type metric;
    ipaddrtype       ip_address;
    ipaddrtype       mask;
} isis_ip_nbr_ad;

/*
 * Inter-Domain Routing (IDR) Protocol Information
 */
typedef struct isis_idr_info_ {
    uchar idr_type;
    uchar info [1];    /* Variable length - length in isis_info_type */
} isis_idr_info;

/*
 * IPX/NLSP Area Address Information
 *
 * The "value" field contains an array of one to three of the following:
 */
typedef struct isis_nlsp_area_addrs_ {
    ulong	PACKED(net_number);	/* IPX Network Number */
    ulong	PACKED(net_mask);	/* Netmask */
} isis_nlsp_area_addrs;

#define ISIS_NLSP_MAX_AREA_ADDRS 3	/* No more than three */
#define ISIS_NLSP_AREA_ADDRS_LEN 8


/*
 * IPX/NLSP Management Information
 */
typedef struct isis_nlsp_mgmt_info_ {
    ulong	PACKED(net_number);	/* IPX Network Number */
    uchar	node_number[IEEEBYTES];	/* IPX Node Number */
    uchar	ipx_version_number;	/* Version number */
    uchar	router_name_length;	/* Length of router name */
    uchar	router_name[0];		/* Name ( <= ISIS_NLSP_SVC_MAXLEN) */
} isis_nlsp_mgmt_info;

#define ISIS_NLSP_MGMT_INFO_LEN 12	/* Plus length of name */
#define NLSP_IPX_VERSION_NUMBER 1	/* For IPX version 1 */

/*
 * IPX/NLSP Link Information
 *
 * It may get longer someday;  don't blow up if it comes in longer
 * than this.
 */
typedef struct isis_nlsp_link_info_ {
    uchar	S1:1;			/* S1 bit, must be zero */
    uchar	external:1;		/* Internal/External (must be zero) */
    uchar	cost:6;			/* Link cost */
    uchar	reserved[3];		/* Reserved, must be zero */
    uchar	neighbor_id[SYSTEMID_LENGTH]; /* Neighbor ID */
    ulong	PACKED(mtu_size);	/* Link MTU */
    ulong	PACKED(delay);		/* Delay in usecs */
    ulong	PACKED(throughput);	/* Throughput in BPS */
    ushort	PACKED(media_type);	/* Media Type */
} isis_nlsp_link_info;

#define ISIS_NLSP_LINK_INFO_LEN 25

/*
 * IPX/NLSP Services Information
 *
 * Only one per TLV.
 */
typedef struct isis_nlsp_svcs_info_ {
    uchar 	hops;			/* Hop count to server */
    ulong	PACKED(net_number);	/* Network number */
    uchar	node_number[IEEEBYTES];	/* Node number */
    ushort	PACKED(socket);		/* Service socket number */
    ushort	PACKED(type);		/* Service type */
    uchar	name[0];		/* Service name */
} isis_nlsp_svcs_info;

#define ISIS_NLSP_SVCS_INFO_LEN 15	/* Plus length of name */
#define ISIS_NLSP_SVCS_INFO_KEY_OFFSET 1 /* We don't key on the hops */
#define ISIS_NLSP_SVC_MAXLEN  47	/* Max length of a service name */

/*
 * IPX/NLSP External Routes
 */
typedef struct isis_nlsp_ext_routes_ {
    uchar	hops;			/* Hop count to network */
    ulong	PACKED(net_number);	/* Network number */
    ushort	PACKED(ticks);		/* Tick count (delay) to network */
} isis_nlsp_ext_routes;

#define ISIS_NLSP_EXT_ROUTES_LEN 7
#define ISIS_NLSP_EXT_ROUTES_KEY_OFFSET 1 /* We don't key on the hops */

/*
 * IPX/NLSP Route Summary
 */
typedef struct isis_nlsp_summary_routes_ {
    uchar       masklen;                /* length of mask in address */
    ulong       PACKED(address);        /* Network address */
    ushort      PACKED(ticks);          /* Tick count (delay) to network */
    uchar       area_count;             /* number of areas */
} isis_nlsp_summary_routes;

#define ISIS_NLSP_SUMMARY_ROUTES_LEN 8
#define ISIS_NLSP_SUMMARY_ROUTES_KEY_OFFSET 1

/*
 * IPX/NLSP Local MTU
 */
typedef struct isis_nlsp_local_mtu_ {
    ulong	PACKED(mtu);			/* Link MTU */
} isis_nlsp_local_mtu;

#define ISIS_NLSP_LOCAL_MTU_LEN 4


/*
 * isis_walk_lsp
 */
typedef union isis_generic_ad_ {
    uchar          es_neighbor[STATION_LENGTH];  /* six bytes of neighbor id */
    isis_is_nbr_ad is_neighbor;       /* metric plus IS neighbor id */
    uchar          prefix_neighbor[CLNS_MAX_NSAP_LENGTH];  /* NSAP Prefix */
    uchar          area_address[CLNS_MAX_NSAP_LENGTH]; /* Area address */
    uchar          nlpid;             /* NLPID byte */
    ipaddrtype     ip_address;        /* IP interface address */
    isis_ip_nbr_ad ip_info;           /* IP reachability info plus metric */
    isis_idr_info  idr_info;          /* Inter-Domain information */
    isis_nlsp_area_addrs ipx_areas;   /* IPX/NLSP area addresses */
    isis_nlsp_mgmt_info ipx_mgmt_info;/* IPX/NLSP management info */
    isis_nlsp_link_info ipx_link_info;/* IPX/NLSP link info */
    isis_nlsp_svcs_info ipx_svcs_info;/* IPX/NLSP services info */
    isis_nlsp_ext_routes ipx_ext_rts; /* IPX/NLSP external routes */
    isis_nlsp_summary_routes ipx_sum_rts; /* IPX/NLSP summary route */
} isis_generic_ad;


/*
 * isis_rexmit_entry
 *
 * This block contains a retransmission timer and the identity of an
 * LSP to retransmit (for point-to-point links).
 */
typedef struct isis_rexmit_entry_ {
    struct isis_rexmit_entry_ *next;
    sys_timestamp rexmit_timer;		/* Retransmit timestamp */
    idbtype *idb;			/* Interface on which we're sending */
    ulong lsp_seq;			/* Sequence number of LSP to send */
    isis_lspid timer_id;		/* LSP ID of LSP to send */
    char level;				/* ISIS Level */
} isis_rexmit_entry;


/*
 * This structure is used to hold the LSPs in a balanced tree
 *
 */
struct isis_tree_type_ {
    int                    balance;             /* May need to after insert. */
    struct isis_tree_type_ *llink;              /* Left link in tree. */
    struct isis_tree_type_ *rlink;              /* Right link in tree. */
    isis_lspdbtype         *lsp;                /* LSP for this node. */
    isis_lspid		   lspid;		/* LSPID at this node. */
};


/*
 * Router Descriptor Block definition (next hop information).	
 */
typedef struct isis_rdbtype_ {
    uchar      system_id [SYSTEMID_LENGTH]; /* Next hop system id. */
    hwaddrtype snpa;                         /* Physical address of adjacency.*/
    idbtype    *idb;                         /* Interface it is attached to. */
} isis_rdbtype;


/*
 * IS-IS Level-1 route table data structure definition. The Level-2
 * route table is maintained by CLNS.
 */
struct isis_ddbtype_ {
    struct isis_ddbtype_ *next;                /* Next entry at this bucket. */
    uchar        destination [STATION_LENGTH]; /* Dest system id. */
    uchar        route_count;                  /* Number of next hops. */
    ulong        metric;                       /* Cost to reach destination. */
    ushort       route_version;                /* Determines if valid. */
    isis_rdbtype *next_hops [ISIS_MAX_PATHS];  /* Next hop info. */
};

/*
 *  IS-IS aggregation
 *  -----------------
 *  Integrated IS-IS does aggregation in two cases. When summarizing IP routes
 *  from a level-1 area into the level-2 subdomain and when redistributing
 *  IP routes into level-1 or level-2. The summary-address router subcommand
 *  configures the type of aggregation.
 *
 *  [no] summary-address <ip-address> <ip-mask> [level-1|level-2|level1-2]
 *
 *  IS-IS advertises the configured aggregates with a metric equal to the
 *  minimum of all the more specific routes of the aggregate. If there are
 *  no more specific routes that exist, the aggregate is not advertised in
 *  an LSP. When the more specific route with the lowest metric has changed or
 *  gone unreachable, the aggregate metric changes (provided there are no other
 *  more specific routes with the same metric). Any other route changes to the 
 *  more specific routes do not cause any changes to the aggregates.
 * 
 *  When there are no summary addresses configured, all level-1 routes are
 *  advertised into level-2.
 *
 *  Aggregation can also be performed when redistributing routes from another 
 *  routing protocol into an IS-IS level-1 area. This capability is similar
 *  to the OSPF NSSA feature.
 */

/*
 * IP address summary entries.
 * 
 * The metric field is used to store the metric of the summaries generated from
 * the redistributed routes.  This field lives only during the generation of
 * the LSP at each level.
 *
 * The L2_metric field is used to store the metric of summaries generated
 * from the set of level-1 routes, plus the ISIS interface routes.
 * This field is long-lived, since it is used when partial route recalculations
 * are done to decide whether or not to invest in a level 2 LSP regeneration
 * (which has the side effect of bringing all of these up to date).
 */
typedef struct isis_addrsumtype_ {
    struct isis_addrsumtype_ *next;
    ipaddrtype sum_address;		/* IP address configured. */
    ipaddrtype sum_mask;		/* IP mask configured. */
    int sum_level;			/* Configured level to summarize. */ 
    ulong sum_metric;			/* min metric of redist sum */
    ulong sum_L2_metric;		/* min metric computed via Dijkstra*/
} isis_addrsumtype;

/*
 * LSP context block
 *
 * Used to hold the context while building LSPs.
 *
 * lsp and lsp_data are guaranteed to always be consistent (lsp_data
 * points within lsp).
 *
 * lsp_item is NULL when the LSP is new and has no items yet.  It points
 * to the current item thereafter.
 *
 * usable_length is the max usable portion of this LSP, according to
 * the "10% free" rule.  This includes the database header (isis_lspdbtype),
 * and thus is in the same units as max_db_length.  For the moment we
 * are not actually saving the 10% free, so usable_length is the same
 * as max_db_length, but we might do this again some day so I won't touch
 * the code.
 *
 * lsp_end always points to the end of the LSP.  Note that when a new
 * has been created with a zero length, lsp_end does not point at the
 * data area of the item, but at the item header (since a zero-length
 * item doesn't really exist).
 */

typedef struct isis_lspcontext_ {
    isis_lspdbtype *lsp;		/* Pointer to LSP structure */
    int usable_length;			/* max_db_length - 10% */
    clns_pdbtype *pdb;			/* Owning PDB */
    isis_info_type *lsp_item;		/* Current item being built or NULL */
    uchar *lsp_end;			/* End of LSP data */
    idbtype *lsp_idb;			/* IDB pointer if pseudonode */
    int level;				/* ISIS_CIRCUIT_L1 / L2 */
    isis_lspid lspid;			/* LSP ID */
    uchar item_type;			/* Item type being built */
} isis_lspcontext;

/*
 * Structure returned by isis_valid_packet.
 */
typedef struct isis_TLV_parse_ {
    isis_info_type *TLV_array[ISIS_NUM_CODES_POSSIBLE];
} isis_TLV_parse;

/*
 * CSNP control structure.  One per level per interface (in the ISIS IDB).
 */
typedef struct csnp_control_ {
    ulong csnp_interval;		/* Interval to send CSNPs */
    mgd_timer isis_csnp_timer;		/* send next complete seq # packet */
    boolean building_packets;		/* TRUE if building packets */
    isis_lspid next_snp_id;		/* LSPID for next packet */
} csnp_control;


/*
 * SPF trigger types.  If you change this list, be sure to change the
 * text constants in isis_update2.c.
 */
enum {
    ISIS_SPF_TRIG_PERIODIC,		/* Periodic SPF */
    ISIS_SPF_TRIG_NEWSYSID,		/* New system ID */
    ISIS_SPF_TRIG_NEWAREA,		/* New area */
    ISIS_SPF_TRIG_NEWLEVEL,		/* Changed router level */
    ISIS_SPF_TRIG_RTCLEAR,		/* Cleared route table */
    ISIS_SPF_TRIG_NEWMETRIC,		/* Changed link metric */
    ISIS_SPF_TRIG_IPBACKUP,		/* IP backup callback */
    ISIS_SPF_TRIG_IPQUERY,		/* IP query callback */
    ISIS_SPF_TRIG_IPXBACKUP,		/* IPX backup callback */
    ISIS_SPF_TRIG_IPXQUERY,		/* IPX query callback */
    ISIS_SPF_TRIG_ATTACH,		/* L2 attach bit changed */
    ISIS_SPF_TRIG_DISTANCE,		/* Admin distance change */
    ISIS_SPF_TRIG_AREASET,		/* Area set changed */
    ISIS_SPF_TRIG_NEWADJ,		/* New adjacency */
    ISIS_SPF_TRIG_DBCHANGED,		/* Database carpet pulled out */
    ISIS_SPF_TRIG_OVERFLOWHIT,		/* Hit backup overflow entry */
/*
 * All triggers larger or equal then ISIS_SPF_TRIG_NEWLSP0 are supposed
 * to specify an lspid in the spf_log. So don't change the order of
 * these types randomly.
 */
    ISIS_SPF_TRIG_NEWLSP0,		/* New LSP 0 */
    ISIS_SPF_TRIG_LSPEXPIRY,		/* LSP expired */
    ISIS_SPF_TRIG_FIELDS,		/* Important fields changed */
    ISIS_SPF_TRIG_CODE,			/* TLV code mismatch */
    ISIS_SPF_TRIG_TLVCHG,		/* TLV change */
    ISIS_SPF_TRIG_NUMTYPES		/* Number of trigger types */
};
#define ISIS_SPF_TRIG_SAVES_LSPID (1 << ISIS_SPF_TRIG_NEWLSP0)

/*
 * SPF trigger log structure
 */
#define ISIS_SPF_LOG_SIZE 20		/* Number of entries in each log */

typedef struct isis_spf_log_entry_ {
    ulong trigger_type;			/* Bit vector of triggers */
    ulong trigger_count;		/* Number of triggers on this run */
    sys_timestamp run_time;		/* Time the SPF finally ran */
    ulong spf_duration;			/* Length of the run */
    ulong node_count;			/* Number of nodes on the tree */
    isis_lspid lspid;			/* Last LSP causing a change */
} isis_spf_log_entry;

struct isis_spf_log_ {
    ulong log_index;			/* Current log index */
    isis_spf_log_entry log_entry[ISIS_SPF_LOG_SIZE];  /* The log entries */
};


/*
 * Mesh group display structure
 *
 * Used to build a list of interfaces on a mesh group, for display
 * purposes.
 */
typedef struct isis_mesh_group_display_ {
    struct isis_mesh_group_display_ *next; /* Next in the list */
    char *intf_name;			/* Interface name */
} isis_mesh_group_display;


/*
 * Mesh group structure
 *
 * A queue of these structures is hung off of the PDB for ISIS and
 * NLSP if mesh groups are in use.
 */
struct isis_mesh_group_entry_ {
    struct isis_mesh_group_entry_ *next; /* Next in the list */
    ulong mesh_group_number;		/* List number */
    dynamic_bitfield mesh_group_bits;	/* Bitvector of interfaces */
    ulong mesh_group_intf_count;	/* Number of interfaces in the group */
    isis_mesh_group_display *display_list; /* List of interface names */
};


/*
 * IS-IS specific interface structures
 *
 * The IDB has an array of pointers to this structure, one for IS-IS and
 * one for NLSP, indexed by process type number.
 */

struct isisidbtype_ {

    /*
     * Hello timers.
     */
    mgd_timer isis_hello_timer;		/* when to send IIH next */
    sys_timestamp isis_last_hello;	/* msclock of last IIH sent */
    
    mgd_timer isis_hello2_timer;	/* when to send next level2 LAN IIH */
    sys_timestamp isis_last2_hello;	/* ms of last level2 LAN IIH sent*/

    /*
     * Various other transmission timers.
     */
    mgd_timer isis_psnp_timer;		/* send next partial seq # packet */

    /*
     * Retransmission info.
     */
    queuetype isis_rexmit_queue;	/* Queue of rexmit_entries */
    mgd_timer isis_rexmit_timer;	/* Kicks when it's time to rexmit */
    ulong isis_min_rexmit_interval;	/* Min ivl between rexmits on i/f */
    ulong isis_retrans_interval;	/* Ivl between rexmits per LSP */
    
    /*
     * LSP transmission pacing controls.
     */
    ulong pacing_interval;		/* Interval between transmissions */
    ulong cfg_pacing_interval;		/* Interval if configured */
    mgd_timer isis_lsp_pacing_timer;	/* Time to send next LSP */
    isis_lspid last_lsp_sent;		/* LSP ID of last LSP sent */
    int last_lsp_level_sent;		/* Level of last LSP sent */

    /*
     * Mesh group info.
     */
    isis_mesh_group_entry *isis_mesh_group;  /* Mesh group membership */

    /*
     * CSNP transmission pacing controls.
     */
    csnp_control csnp_ctl[ISIS_NUM_LEVELS]; /* One for each level */

    /*
     * Flags.
     */
    uchar isis_running:1;		/* ISIS/NLSP running on this i/f */
    uchar isis_interface_up:1;		/* Track interface state */
    uchar isis_compute_L1_dr:1;		/* re-compute the level 1 DR */
    uchar isis_compute_L2_dr:1;		/* re-compute the level 2 DR */
    uchar isis_L1_dr_changed:1;		/* some info in L1 DR LSP changed */
    uchar isis_L2_dr_changed:1;		/* some info in L2 DR LSP changed */
    uchar match_all_areas:1;		/* TRUE if all area addrs must match */
    uchar isis_lsp_send_requested:1;	/* TRUE if we should send our LSPs */
    uchar isis_sent_an_lsp:1;		/* TRUE if LSP sent since we started */
    uchar isis_ipx_xroutes_changed:1;	/* TRUE if xroutes changed */
    uchar nlsp_explicit_cost:1;		/* TRUE if NLSP link cost hand-set */
    
    /*
     * The following fields are used to determine the
     * designated level 1 and level 2 routers (DR) on LANs.
     */
    long    isis_if_number;		/* ordinal of IS-IS idb, 0..N */
					/* or -1 if not configured */
    ulong   isis_local_ckt_number;	/* Circuit number of this i/f */
    int     isis_circuit_type;		/* Configured circuit type */
    uchar   isis_L1_dr_id[SYSTEMID_LENGTH]; /* L1 DR on this circuit */
    uchar   isis_L1_circuit_id[SYSTEMID_LENGTH]; /* unique ID for this i/f */
					/* (serial i/fs and level 1 LAN) */ 
    uchar   isis_L2_dr_id[SYSTEMID_LENGTH]; /* L2 DR on this circuit */
    uchar   isis_L2_circuit_id[SYSTEMID_LENGTH]; /* level2 LAN id */
    uchar   isis_L1_prio;		/* level 1 designated router prio */
    uchar   isis_L1_cur_prio;		/* L1 current DR prio (NLSP changes) */
    uchar   isis_L2_prio;		/* level 2 designated router prio */
    uchar   isis_if_state;		/* State to report in P2P Hello */
    mgd_timer isis_dr_timer;		/* dr holddown timer */
    
    /*
     * The following fields are used to determined the ISIS metric
     * for this interface.
     */
    uchar   isis_L1_metric;		/* level 1 metric (6 bits) */
    uchar   isis_L2_metric;		/* level 2 metric (6 bits) */
    
    /*
     * Current authentication mechanism is a simple password. 
     */
    uchar   *isis_L1_pw;		/* Level 1 password */
    uchar   *isis_L2_pw;		/* Level 2 password */
    
    /*
     * Filter for adjacencies
     */
    ulong isis_adj_filter;		/* Index of filter, or 0 */
    
    /* 
     * The following fields are IS-IS configured timer intervals.
     */
    ulong isis_L1_hello_interval;	/* L1 IIH interval */
    ulong isis_L1_hello_multiplier;	/* L1 hello multiplier */
    ulong isis_L1_dr_hello_interval;	/* L1 IIH interval for DRs */
    ulong isis_L2_hello_interval;	/* L2 IIH interval */
    ulong isis_L2_hello_multiplier;	/* L2 hello multiplier */
    ulong isis_L2_dr_hello_interval;	/* L2 IIH interval for DRs */
    
    /*
     * Count of the number of active adjacencies (level-1 and level-2)
     * on the interface.
     */
    ulong isis_adj_count[ISIS_NUM_LEVELS];
    
    /*
     * Per interface traffic counters
     */
    enum {ISISIDB_L1_HELLO_OUT, ISISIDB_L1_HELLO_IN, ISISIDB_L2_HELLO_OUT,
          ISISIDB_L2_HELLO_IN, ISISIDB_PTP_HELLO_OUT, ISISIDB_PTP_HELLO_IN,
          ISISIDB_L1_LSP_OUT, ISISIDB_L1_LSP_IN, ISISIDB_L2_LSP_OUT,
	  ISISIDB_L2_LSP_IN,
          ISISIDB_L1_CSNP_OUT, ISISIDB_L1_CSNP_IN, ISISIDB_L2_CSNP_OUT,
          ISISIDB_L2_CSNP_IN, ISISIDB_L1_PSNP_OUT, ISISIDB_L1_PSNP_IN,
          ISISIDB_L2_PSNP_OUT, ISISIDB_L2_PSNP_IN, ISISIDB_L1_DR_ELECTS,
          ISISIDB_L2_DR_ELECTS, ISISIDB_L1DRCHANGES, 
	  ISISIDB_CIRCNEIGHCHANGE, ISISIDB_CIRCREJNEIGHBORS,
          ISISIDB_MAX_TRAFFIC};

    ulong isisidb_traffic[ISISIDB_MAX_TRAFFIC];
};

/*
 * Minimum pacing interval for LSPs and CSNPs.  At least this much time
 * needs to pass between the transmission of LSPs on an interface.
 */
#define NLSP_MINIMUM_PACING_INTERVAL 55	/* We can drive 55! */

/*
 * Limiters and such for dealing with >255 interfaces.  See the comments
 * at the beginning of isis.c for more info.
 */
#define ISIS_P2P_CKT_MASK 0xff		/* Mask for ckt # for P2P interfaces */
#define ISIS_P2P_CKT_OFFSET 0x100	/* Starting ckt # for P2P interfaces */
#define ISIS_LAN_CKT_COUNT 0x100	/* Number of LAN ckts possible */

/*
 * IS-IS managed timer types
 */
enum {ISIS_L1_HELLO_TIMER, ISIS_L2_HELLO_TIMER, ISIS_DR_TIMER,
	  ISIS_ADJ_TIMER, ISIS_PSNP_TIMER, ISIS_L1_CSNP_TIMER,
	  ISIS_L2_CSNP_TIMER, ISIS_AGE_TIMER, ISIS_REXMIT_TIMER,
	  ISIS_LSP_PACING_TIMER, ISIS_LSP_GEN_TIMER, ISIS_LSP_TIMER,
	  ISIS_LSP_REFRESH_TIMER, ISIS_L1_LSP_BUILD_TIMER,
	  ISIS_L2_LSP_BUILD_TIMER, ISIS_L1_SPF_TIMER, ISIS_L2_SPF_TIMER,
          ISIS_L1_PRC_TIMER, ISIS_DUPID_TIMER};

/*
 * LSP index table
 *
 * Each LSP in the database has an LSP index.  This index corresponds to
 * a table entry below.  This table is used to associate the contents of
 * an LSP with the associated protocol-specific forwarding table entries,
 * etc., allowing relatively efficient updating of routes without having
 * to run full Dijkstra calculations, etc.
 *
 * Note that the LSP index array leaves the 0th entry unused, so that
 * ix=0 can be used for end conditions.
 *
 * Note that this array may be reallocated at will, since it grows and
 * shrinks dynamically.  This of course involves changing the prev
 * pointers on the first item in each chain (thus the prev pointers should
 * be "**" pointers).  The index number is also changable at will;
 * it should be considered an opaque value by everything except ISIS/NLSP.
 */

typedef enum {ISIS_ROUTE_IPX, 
		  ISIS_ROUTE_IPX_SVC,
		  ISIS_ROUTE_IP,
		  NUM_ISIS_ROUTE_TYPES}
isis_route_type;

struct isis_lspix_type_ {
    isis_lspdbtype *lsp;		/* Pointer to LSP */
    ushort lsp_version;			/* Current version number */
    uchar lsp_changed:1;		/* LSP changed */
    uchar lspix_in_use:1;		/* This index entry is in use */
    int level;				/* ISIS level of this LSP */
    isis_lsp_linkage *chains[NUM_ISIS_ROUTE_TYPES]; /* Chain heads */
    isis_backuptype *backup_chains[NUM_ISIS_ROUTE_TYPES]; /* Backup info */
};

#define ISIS_LSPIX_INCREMENT 10		/* Add 10 more when we grow */


/*
 * SNP-building context
 *
 * This data structure is used to maintain context while building a set
 * of SNPs.
 */
typedef struct isis_snp_build_context_ {
    paktype *pak;			/* The current packet */
    isis_info_type *item;		/* Current item pointer */
    ulong type;				/* The packet type (ISIS_L1_CSNP...) */
    ulong current_length;		/* Current packet length */
    boolean packet_full;		/* TRUE if rw exited with full pkt */
    hwaddrtype *snpa;			/* SNPA to send on */
} isis_snp_build_context;

/*
 * CSNP-parsing context
 *
 * This data structure is used to maintain context while parsing an CSNP.
 */
typedef struct isis_csnp_parse_context_ {
    idbtype *idb;			/* IDB */
    isisidbtype *isisidb;		/* ISIS IDB */
    isis_CSNP_hdrtype *csnp_pkt;	/* Current CSNP header */
    boolean serial;			/* TRUE if a serial link */
    int level;				/* L1/L2 */
    isis_info_type *item;		/* Current item being parsed */
    isis_info_type *item_end;		/* End of current item */
    isis_snp_entry *current_entry;	/* Entry being considered */
    uchar *packet_end;			/* End of the packet */
} isis_csnp_parse_context;

/*
 * IS-IS backup information
 *
 * This structure is hung off of the NDB to indicate the possible presence
 * of a backup route.  If the primary route is being deleted, this will
 * tell ISIS either which LSP to recompute from, or give it the sad
 * information that it must recompute SPF.
 *
 * There are two threads for this data structure.  The NDB thread connects
 * all backup routes for a particular destination network.  The LSP thread
 * connects all backup routes on a particular LSP.  The NDB thread is
 * sorted in ascending metric order.  The LSP thread is not ordered.
 *
 * Note that the NDB thread may be hanging off of an NDB backup entry,
 * rather than the NDB itself, if there is a route in the forwarding table
 * with a better administrative distance than ISIS.
 *
 * Note that if multiple backup next hops are defined at a particular
 * metric, they will be contiguous on the NDB thread.
 *
 * If the LSP index is zero, it means that there are more backup routes
 * but their location in the database is unknown.  Note that this entry
 * will not be on an LSP thread!  The metric in this entry is set to the
 * metric of the entry that was truncated;  this allows entries with a better
 * metric to be safely inserted in front of this entry (since it is known
 * that there was no entry with a better metric that was truncated).
 *
 * Note that the high-order bit of the metric is set if the LSP is level-2;
 * this gives level-1 routes precedence regardless of metric.
 */
struct isis_backuptype_ { 
   struct isis_backuptype_ *ndb_next;	/* Forward NDB thread */
   struct isis_backuptype_ **ndb_prev;	/* Backward NDB thread  */
   struct isis_backuptype_ *lsp_next;	/* Forward LSP thread */
   struct isis_backuptype_ **lsp_prev;	/* Backward LSP thread */
   ulong metric;			/* Path metric */
   ushort lsp_index;			/* LSP index of backup */
};

#define ISIS_BACKUP_MAX_METRIC_DEPTH 2	/* 2 worse metrics allowed */
#define ISIS_BACKUP_L2_METRIC_BIT (1<<31) /* Set if L2 route */
#define ISIS_BACKUP_BASE_METRIC(metric) (metric & (~ISIS_BACKUP_L2_METRIC_BIT))
#define ISIS_BACKUP_LEVEL(metric) (((metric & ISIS_BACKUP_L2_METRIC_BIT) != 0)\
				   + 1)

/*
 * ISIS Cleanup Queue entry
 *
 * The Cleanup Queue is a mechanism used to avoid having to search the
 * entire route table in order to clean up after a series of partial
 * route calculations.
 *
 * The Cleanup Queue is used when the LSP ager deletes the final next
 * hop from a route, but notices that there are backup routes present.
 * When this occurs, the LSPs corresponding to the backup routes are
 * flagged for recalculation and the route entry is not deleted (because
 * there may be still more backup routes present).  After all the
 * recalculations are complete, each entry in the Cleanup Queue is examined,
 * and if the corresponding route still has no next-hop entries present,
 * it will be deleted at that time.
 */
struct isis_cleanup_queue_ {
    struct isis_cleanup_queue_ *next_cleanup;
    isis_route_type route_type;
    union {
	struct {
	    ipaddrtype address;		/* IP address */
	    ipaddrtype mask;		/* Net mask */
	} ip_cleanup;
    } proto_cleanup;			/* Protocol-specific fields */
};

#define cleanup_ip_addr proto_cleanup.ip_cleanup.address
#define cleanup_ip_mask proto_cleanup.ip_cleanup.mask

/*
 * Fast PSNP cache structure. We access all information via the lspix in
 * the real LSP, just to make sure the LSP is still there.
 */
struct isis_fastpsnp_type_ {
    ushort lspix;
};

#define ISIS_FASTPSNP_CACHESIZE (MAXUCHAR + 1)
#define ISIS_FASTPSNP_ID_SIZE \
        (ISIS_LSPID_LENGTH + sizeof(ulong) + ISIS_LSP_CHECKSUM_LENGTH)
	/* LSP id, LSP sequencenumber and LSP checksum */


/*
 * IS-IS defined macros.
 */
#define ISIS_L1_HASH(addr) (0xff & nsap_hash (STATION_LENGTH, addr))

#define ISIS_LSPID_IN_RANGE(pak, lspid) \
    (isis_match_lspid (lspid, pak->start_lsp) >= 0 && \
     isis_match_lspid (pak->end_lsp, lspid) >=0)

/*
 * ADVANCE_POINTER - add a byte offset to an opaque pointer
 */
#define ADVANCE_POINTER(pointer, amount) \
    (pointer) = (void *)(((uchar *) (pointer)) + (amount))


/*
 * ISIS/NLSP traffic statistics
 */
enum {ISIS_L1_HELLO_OUT, ISIS_L1_HELLO_IN, ISIS_L2_HELLO_OUT, 
	  ISIS_L2_HELLO_IN, ISIS_PTP_HELLO_OUT, ISIS_PTP_HELLO_IN, 
	  ISIS_L1_LSP_OUT, ISIS_L1_LSP_IN, ISIS_L2_LSP_OUT, ISIS_L2_LSP_IN, 
	  ISIS_L1_CSNP_OUT, ISIS_L1_CSNP_IN, ISIS_L2_CSNP_OUT, 
	  ISIS_L2_CSNP_IN, ISIS_L1_PSNP_OUT, ISIS_L1_PSNP_IN, 
	  ISIS_L2_PSNP_OUT, ISIS_L2_PSNP_IN, ISIS_L1_DR_ELECTS, 
	  ISIS_L2_DR_ELECTS, ISIS_L1_SPF_DONE, ISIS_L2_SPF_DONE,
	  ISIS_L1_PARTIALS, ISIS_L2_PARTIALS, ISIS_LSP_CORR,
	  ISIS_L1_OVR_LOAD, ISIS_L1_DBASE_OVR_LOAD, ISIS_MAXSEQNUMS,
          ISIS_SEQNUMSKIPS, ISIS_LSP_PURGE, ISIS_VERSION_ERR,
	  ISIS_INCORRECT_PACKETS, ISIS_RESOURCE_FAIL, ISIS_CKSUM_ERR,
	  ISIS_CKSUM_ERR_HT0, ISIS_RETRANSMITS, ISIS_FASTPSNP_CACHEHITS,
	  ISIS_L1_LSP_BUILD, ISIS_L2_LSP_BUILD,
	  ISIS_L1_LSP_REFRESHED, ISIS_L2_LSP_REFRESHED,
	  ISIS_MAX_TRAFFIC};

extern ulong isis_traffic[ISIS_NUM_PROC_TYPE][ISIS_MAX_TRAFFIC];
extern const char isis_data_str[];

#endif __ISIS_H__
