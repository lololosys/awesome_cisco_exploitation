/* $Id: clns_route.h,v 3.7.6.8 1996/09/12 00:17:02 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_route.h,v $
 *------------------------------------------------------------------
 * clns_route.h -- CLNS routing definitions
 *
 * January 1990, P. Knibbe
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_route.h,v $
 * Revision 3.7.6.8  1996/09/12  00:17:02  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.7.6.7  1996/08/11  15:12:51  dkatz
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
 * Revision 3.7.6.6  1996/08/08  14:50:33  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.7.6.5  1996/06/18  16:31:17  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Force the protocol to go to sleep long enough for all neighbor
 * adjacencies to time out when the level is changed.  While we're
 * in there, do it for NET changes as well (since these can change
 * adjacency levels).
 *
 * Revision 3.7.6.4  1996/05/26  06:31:54  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.7.6.3  1996/05/10  19:45:25  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.7.6.2  1996/05/06  06:14:51  asastry
 * CSCdi40503:  CLNS routing problems caused by discard routes (DECnet
 * conversion)
 * Branch: California_branch
 *
 * Revision 3.7.6.1  1996/03/18  19:03:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.3  1996/03/16  06:30:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.16.2  1996/03/13  01:13:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.16.1  1996/02/20  13:40:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/05  23:10:00  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.6  1996/03/01  07:31:28  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.5  1995/12/11  04:45:18  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.4  1995/12/01  02:48:17  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:52:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:52:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/09/25  08:34:42  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.10  1995/09/23  22:48:15  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.9  1995/09/23  21:37:27  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.8  1995/08/25  16:02:42  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.7  1995/08/25  14:59:36  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.6  1995/08/25  14:10:28  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.5  1995/08/25 05:39:51  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.4  1995/08/23  18:25:53  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.3  1995/08/23  18:02:56  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/08/08  16:42:49  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.1  1995/06/07 20:17:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLNS_ROUTE_H__
#define __CLNS_ROUTE_H__

#include "address.h"
#include "../clns/clns.h"
#include "../clns/clns_adj.h"
#include "isis_nlsp_proc_nums.h"
#include "bitlogic.h"

STRUCTURE_REF(isis_mesh_group_entry);

/*
 * Possible sources of routing information.
 *
 * These are used to fill in the 'flags' field in the clns_rdbtype.
 */

#define ISO_STATIC	0x00000001   /* station/area was entered manually */
#define ISO_IGRP0	0x00000002   /* station/area discovered via IGRP 
 				        process 0 */
#define ISO_IGRP1	0x00000004   /* station/area discovered via IGRP 
 				        process 1 */
#define ISO_IGRP2	0x00000008   /* station/area discovered via IGRP 
 				        process 2 */
#define ISO_IGRP3	0x00000010   /* station/area discovered via IGRP 
 				        process 3 */
#define ISO_IGRP4       0x00000020   /* station/area discovered via IGRP 
 					process 4 */
#define ISO_IGRP5       0x00000040   /* station/area discovered via IGRP 
 					process 5 */
#define ISO_IGRP6       0x00000080   /* IGRP process 6 */
#define ISO_IGRP7       0x00000100   /* IGRP process 7 */
#define ISO_IGRP8       0x00000200   /* IGRP process 8 */
#define ISO_IGRP9       0x00000400   /* IGRP process 9 */
#define ISO_IGRP10      0x00000800   /* IGRP process 10 */
#define ISO_IGRP11      0x00001000   /* IGRP process 11 */
#define ISO_IGRP12      0x00002000   /* IGRP process 12 */
#define ISO_IGRP13      0x00004000   /* IGRP process 13 */
#define ISO_IGRP14      0x00008000   /* IGRP process 14 */
#define ISO_IGRP15      0x00010000   /* IGRP process 15 */
#define ISO_IGRP16      0x00020000   /* IGRP process 16 */
#define ISO_IGRP17      0x00040000   /* IGRP process 17 */
#define ISO_IGRP18      0x00080000   /* IGRP process 18 */
#define ISO_IGRP19      0x00100000   /* IGRP process 19 */
#define ISO_IGRP20      0x00200000   /* IGRP process 20 */

#define ISO_ESIS        0x00400000   /* Station/Area Id discovered via 
 					ES/IS */
#define DECNET_PHASEIV  0x00800000   /* Station discovered by phase iv hello */
#define ISO_ISIS0       0x01000000   /* First IS-IS process */
#define ISO_ISIS1       0x02000000   /* Second IS-IS process */
#define MAX_ISIS_PROC   2            /* Maximum number of ISIS instances */
#define DECNET_DISCARD  0x04000000   /* DECnet-derived discard route */
#define ISO_NLSP	0x08000000   /* First NLSP process */
/* 32 pdbs - connected - rip - f-static - static */
#define MAX_NLSP_PROC   28
#define ISO_IGRPMASK (ISO_IGRP0+ISO_IGRP1+ISO_IGRP2+ISO_IGRP3+ISO_IGRP4+ \
                      ISO_IGRP5+ISO_IGRP6+ISO_IGRP7+ISO_IGRP8+ISO_IGRP9+ \
 		      ISO_IGRP10+ISO_IGRP11+ISO_IGRP12+ISO_IGRP13+ISO_IGRP14+ \
 		      ISO_IGRP15+ISO_IGRP16+ISO_IGRP17+ISO_IGRP18+ISO_IGRP19+\
 		      ISO_IGRP20)
#define ISO_ISISMASK (ISO_ISIS0+ISO_ISIS1)
#define ISO_NLSPMASK (ISO_NLSP)

/*
 * Administrative distance definitions for each routing protocol.
 */
#define ISO_STATIC_DIST             10
#define ISO_IGRP_DIST               100
#define ISO_ISIS_DIST               110
#define ISO_DECNET_DISCARD_DIST     120
#define ISO_ESIS_DIST               200
#define ISO_INFINITE_DIST           255

/*
 * Routing process indices.
 */

enum {
    CLNS_PDBINDEX_STATIC = 0,
    CLNS_PDBINDEX_IGRP0,
    CLNS_PDBINDEX_IGRP1,
    CLNS_PDBINDEX_IGRP2,
    CLNS_PDBINDEX_IGRP3,
    CLNS_PDBINDEX_IGRP4,
    CLNS_PDBINDEX_IGRP5,
    CLNS_PDBINDEX_IGRP6,
    CLNS_PDBINDEX_IGRP7,
    CLNS_PDBINDEX_IGRP8,
    CLNS_PDBINDEX_IGRP9,
    CLNS_PDBINDEX_IGRP10,
    CLNS_PDBINDEX_IGRP11,
    CLNS_PDBINDEX_IGRP12,
    CLNS_PDBINDEX_IGRP13,
    CLNS_PDBINDEX_IGRP14,
    CLNS_PDBINDEX_IGRP15,
    CLNS_PDBINDEX_IGRP16,
    CLNS_PDBINDEX_IGRP17,
    CLNS_PDBINDEX_IGRP18,
    CLNS_PDBINDEX_IGRP19,
    CLNS_PDBINDEX_IGRP20,
    CLNS_PDBINDEX_ISIS0,
    CLNS_PDBINDEX_ISISNLSP_MIN = CLNS_PDBINDEX_ISIS0,
    CLNS_PDBINDEX_ISIS1,
    CLNS_PDBINDEX_NLSP0,
    CLNS_PDBINDEX_NLSP1,
    CLNS_PDBINDEX_NLSP2,
    CLNS_PDBINDEX_NLSP3,
    CLNS_PDBINDEX_NLSP4,
    CLNS_PDBINDEX_NLSP5,
    CLNS_PDBINDEX_NLSP6,
    CLNS_PDBINDEX_NLSP7,
    CLNS_PDBINDEX_NLSP8,
    CLNS_PDBINDEX_NLSP9,
    CLNS_PDBINDEX_NLSP10,
    CLNS_PDBINDEX_NLSP11,
    CLNS_PDBINDEX_NLSP12,
    CLNS_PDBINDEX_NLSP13,
    CLNS_PDBINDEX_NLSP14,
    CLNS_PDBINDEX_NLSP15,
    CLNS_PDBINDEX_NLSP16,
    CLNS_PDBINDEX_NLSP17,
    CLNS_PDBINDEX_NLSP18,
    CLNS_PDBINDEX_NLSP19,
    CLNS_PDBINDEX_NLSP20,
    CLNS_PDBINDEX_NLSP21,
    CLNS_PDBINDEX_NLSP22,
    CLNS_PDBINDEX_NLSP23,
    CLNS_PDBINDEX_NLSP24,
    CLNS_PDBINDEX_NLSP25,
    CLNS_PDBINDEX_NLSP26,
    CLNS_PDBINDEX_NLSP27,
    CLNS_PDBINDEX_ISISNLSP_MAX = CLNS_PDBINDEX_NLSP27,

    CLNS_MAXROUTERS
};
#define MAXROUTES            6
#define DEF_MAXROUTES	     4

/* 
 * Maximum # of ISO-IGRP processes is 10 
 */
#define MAX_ISO_IGRP_PROCESSES 10
#define MAX_ISISNLSP_PROCESSES (CLNS_PDBINDEX_ISISNLSP_MAX - \
				CLNS_PDBINDEX_ISISNLSP_MIN + 1)

#define CLNS_METRIC_INACCESSIBLE 0xFFFFFF /* Distinct from IP */

#define CLNS_LEVEL1   1
#define CLNS_LEVEL2   2
#define CLNS_LEVEL3   4


/*
 * Number of levels in ISIS
 */
#define ISIS_NUM_LEVELS 2  /* It's two, click, two, click, two protos in one */

typedef struct idbqueuetype_ {
    struct idbqueuetype_ *next;
    idbtype              *idb;
    char		 *pname;	/* The Level-2 process name */
} idbqueuetype;


/*
 * Route Descriptor Block
 *
 * This structure is used for each distinct path to reach a particular
 * destination. The ddb contains an array of rdb's. This information 
 * pertains to the next hop entry for a route. There also contains ISO-IGRP
 * specific fields.
 */
typedef struct clns_rdbtype_ {
    struct clns_rdbtype_ *next;
    NSAP_T     first_hop;       /* gateway or actual dest if reachable */
    idbtype    *idb;            /* interface that first_hop resides */
    ulong      protocol_source; /* static, es-is, iso-igrp, is-is */
    sys_timestamp expiration_time; /* when this route expires */
    ulong      validation_time; /* used to uniquely identify prefix routes */
    NSAP_T     info_source;     /* originator of route - for ISO-IGRP */
    uchar      load;	        /* composite loading - for ISO-IGRP */
    uchar      reliability;     /* composite reliability - for ISO-IGRP */
    uchar      hopcount;        /* hop count - for ISO-IGRP */
    ulong      metric;          /* composite metric for this route */
    ulong      delay;           /* composite delay - for ISO-IGRP */
    ulong      bandwidth;       /* composite bandwidth - for ISO-IGRP */
    ushort     route_version;   /* Validity of route - for IS-IS */
    ulong      uses;            /* Number of times this route was used */
    ulong      last_use;        /* Timestamp when this route last used */
    ushort     mtu;             /* composite MTU - for ISO-IGRP */       
} clns_rdbtype;


/*
 * Destination Descriptor Block
 *
 * One of these exists for each destination
 * that this router knows how to reach. This block will describe
 * both end stations and areas.
 */
typedef struct clns_ddbtype_ {
    struct clns_ddbtype_ *next;   /* pointer to next ddbtype */
    NSAP_T       destination;     /* destination */   
    int          entry_type;      /* cache, routing table or neighbor entry */ 
    ulong        metric;          /* best of the current metrics */
    clns_rdbtype *rdb[MAXROUTES]; /* pointers to rdb's */
    ulong        flags;		  /* flag word */
    sys_timestamp blindtimer;	  /* timeout for update blindness */
    uchar        routecount;      /* count of active route blocks */
    uchar        type;            /* station or area */
    uchar        distance;        /* administrative distance */
} clns_ddbtype;

/*
 * Values for entry_type
 */
#define CLNS_NULL_ENTRY	          0  /* used when there is no nrib entry */
#define CLNS_OURNSAP_ENTRY        1  /* nsap is ours, from global clns net */
#define CLNS_OURNET_ENTRY         2  /* nsap is our NET */
#define CLNS_IS_ENTRY	          3  /* nsap is from an ISH */
#define CLNS_ES_ENTRY	          4  /* nsap is from an ESH */
#define CLNS_REDIRECT_ENTRY       5  /* nsap matches a redirect entry */
#define CLNS_ROUTE_ENTRY          6  /* nsap matches this routing entry */
#define CLNS_ROUTE_CACHE_ENTRY    7  /* cached & IS entry combination */
#define CLNS_REDIRECT_CACHE_ENTRY 8  /* cached redirect entry */
#define CLNS_DISCARD_ENTRY        9  /* bogus nsap or discard */
#define CLNS_CACHE_INVALID_ENTRY  10 /* cache entry has version	mismatch */
#define CLNS_DECNET_DISCARD_ENTRY 11 /* static discard route created by
					DECnet*/


/*
 * Protocol Descriptor Block for ISO routing protocols.
 *
 * For ISO_IGRP there is one process for each area this router is in.
 * In addition, there is one process for each domain this router is in
 * (This means that every CLNS router will have at least two ISO IGRP 
 * routing proceses.)
 *
 * For IS-IS, there is two processes per pdb. One to handle adjacency
 * related functions and another to handle LSP and SNP related processing
 * (Update process).
 *
 * The first part of the data structure must be the same for all protocols.
 */
STRUCTURE_REF(isis_fastpsnp_type);     /* clns/isis.h */

struct clns_pdbtype_ {
/*
 * WARNING!!!  The next three fields must be congruent to the same three
 * fields in pdbtype (in route.h)  This is due to the unfortunate
 * practice of coercing the PDB pointer in certain conditions.  Sad, but
 * true.
 */
    struct clns_pdbtype_ *next;	/* pointer to next pdb */
    int          family;        /* Identifies an IP or OSI pdb - always OSI */
    ulong	 ip_proctype;	/* IP protocol type flag, e.g. PROC_IGRP */
/*
 * End of warning.  We now return you to our regularly scheduled program,
 * which is already in progress.
 */
    char         *name;		/* pointer to protocol name string */
    char         *pname;	/* pointer to process name string */
    boolean      pdb_running;	/* TRUE if process already running */
    watched_boolean *watched_running;  /* Event-oriented version */
    watched_boolean *router_wakeup;    /* Set to wake up router process */
    watched_boolean *update_wakeup;    /* Set to wake up update process */
    pid_t        clns_pid;	/* process ID of running process */
    uchar        *domain;       /* pointer to domain part of nsap */
    char         domain_length; /* length of domain part of nsap */
    NSAP_T       net;           /* network entity title for this process */
    ulong        typeflag;	/* protocol type flag, e.g. ISO_IGRP0 */
    idbqueuetype *idbqueue;     /* interfaces protocol is configured over */
    queuetype    clns_inputq;	/* input queue */
    watched_queue *clns_watched_inputq; /* pointer to watched input queue */
    mgd_timer    timer;		/* our process activation timer */
    int          changed;	/* state change flag */
    sys_timestamp whenchanged;	/* remember when last changed */
    int          level;         /* level 1 or level 2 pdb */
    ulong        redistallowed; /* types of redistribution allowed */
    ulong        distance;      /* adminstrative distance for this protocol */
    uchar        *routemap[CLNS_MAXROUTERS]; /* Used for redistribution */
    route_maptype *route_map;   /* holds route-map to later get set values */

    /* ---------------------------------------------------------------- */
    /* ----- ISO-IGRP related fields. Ignored by other protocols. ----- */
    /* ---------------------------------------------------------------- */

    int          index;		    /* routing process index (0-MAXROUTERS) */
    sys_timestamp hello_timer;      /* hello timer */
    ulong        broadcasttime;     /* broadcast interval in ticks */
    ulong        def_broadcasttime; /* default value */
    ulong        invalidtime;       /* invalid after this many ticks */
    ulong        def_invalidtime;   /* default value */
    ulong        blindtime;         /* hold-down time in ticks */
    ulong        def_blindtime;     /* default value */
    ulong        hellotime;         /* hold frequently should we send hellos */
    ulong        def_hellotime;     /* default value */
    uchar        edition;	    /* IGRP edition number */
    ulong        def_bandwidth;	    /* IGRP readvertised bandwidth */
    ulong        def_reliability;   /*  "	         reliablity */
    ulong        def_delay;	    /* 	"	         delay */
    ulong        def_loading;	    /*	"	         load factor */
    ushort       def_mtu;	    /*	"	         mtu */
    ushort       igrp_hopcount;	    /* hopcount of network_update() caller */
    
    /*
     * For each TOS specified, there needs to be different K factors.
     * For now we are assuming a single TOS.
     */
    ushort       igrp_k1;	    /* IGRP metric factor 1 */
    ushort       igrp_k1_def;	    /* IGRP metric factor 1, default */
    ushort       igrp_k2;	    /* IGRP metric factor 2 */
    ushort       igrp_k2_def;	    /* IGRP metric factor 2, default */
    ushort       igrp_k3;	    /* IGRP metric factor 3 */
    ushort       igrp_k3_def;	    /* IGRP metric factor 3, default */
    ushort       igrp_k4;	    /* IGRP metric factor 4 */
    ushort       igrp_k4_def;	    /* IGRP metric factor 4, default */
    ushort       igrp_k5;	    /* IGRP metric factor 5 */
    ushort       igrp_k5_def;	    /* IGRP metric factor 5, default */

    /*
     * Pointer to station (or area) routing database.
     * This database will be accessed by a hash value based on
     * the domain specific portion of the NET (domain).
     */
    clns_ddbtype *(dest_routing_table[CLNS_NETHASHLEN]);

    /* ------------------------------------------------------------- */
    /* ----- IS-IS related fields. Ignored by other protocols. ----- */
    /* ------------------------------------------------------------- */

    clns_adjtype **adjacency_db;	/* Adjacency database */
    clns_station_lookup_type stationid_lookup; /* Pointer to lookup routine */
    boolean	 name_lookup_disabled;  /* TRUE if no name lookups */
    uchar        systemId[SYSTEMID_LENGTH]; /* system id for the router */
    uchar        systemId_checksum [2];     /* checksum of system id */
    /*
     * WARNING!!  area_address actually contains full NETs.
     * active_area_address contains area addresses (prefixes).
     */
    uchar        num_areas;            /* number manual area addresses */
    NSAP_T       area_address[MAX_AREA_ADDRESSES]; /* manual area addr */
    uchar        num_active_areas;     /* number active area addresses */
    NSAP_T       active_area_address[MAX_AREA_ADDRESSES];/* active area addr */
    uchar	 old_num_active_areas; /* number of old active areas */
    NSAP_T	 old_active_area_address[MAX_AREA_ADDRESSES]; /* old set */
    int          num_L2_routes;        /* L2 routes in prefix table */
    int          redistributed_L2_routes; /* num of redist prefix routes*/
    uchar        *area_password;       /* Area auth. password */
    uchar        *domain_password;     /* Domain auth. password */
    int          nlsp_union_area_match_count[MAX_AREA_ADDRESSES*2];
    ulong	 nlsp_union_area[MAX_AREA_ADDRESSES*2]; /* NLSP area number */
    ulong	 nlsp_union_area_mask[MAX_AREA_ADDRESSES*2]; /* With mask*/
    int		 num_nlsp_union_areas;	/* Number of NLSP union areas */
    ulong	 nlsp_manual_area[MAX_AREA_ADDRESSES]; /* NLSP area number */
    ulong	 nlsp_manual_area_mask[MAX_AREA_ADDRESSES]; /* With mask*/
    int		 num_nlsp_areas;	/* Number of NLSP manual areas */
    ulong        nlsp_computed_area[MAX_AREA_ADDRESSES];
    ulong        nlsp_computed_area_mask[MAX_AREA_ADDRESSES];
    int          num_nlsp_computed_areas;
    int		 L1_adj_type;		/* ADJ_TYPE for L1 adjacencies */
    int		 L1_item_type;		/* TLV item type for L1 adjacencies */
    queuetype	 pdb_mesh_groups;	/* Queue of mesh group entries */
    isis_mesh_group_entry *mesh_group_blocked; /* Group of blocked i/f's */

    /*
     * Fields for adjacency management
     */
    ulong	 def_hello_ivl;		/* Default hello interval */
    ulong	 lan_dr_hello_divisor;	/* LAN DR hello divisor */

    /*
     * The following fields are used by the IS-IS update process.
     */
    mgd_timer    update_timer;        /* Master update timer */
    mgd_timer    lsp_timers;	      /* Parent to LSP holding timers */
    ulong	 packet_mtu;	      /* Max packet size */
    ulong	 default_packet_mtu;  /* Default value for packet_mtu */
    ulong	 lsp_lifetime;	      /* LSP lifetime, in milliseconds */
    ulong	 lsp_refresh_interval; /* LSP refresh time, in milliseconds */
    int		 packet_overhead;     /* Overhead in front of packets */
    pid_t        update_pid;          /* pid for update process */
    watched_queue *update_inputq;     /* input queue for update process */
    int		 input_drops;	      /* Drops due to full update_inputq */
    mgd_timer	 L1_spf_timer;	      /* Timer to run next level-1 SPF */
    mgd_timer	 L2_spf_timer;        /* Timer to run next level-2 SPF */
    ulong	 spf_interval[ISIS_NUM_LEVELS]; /* Min SPF ivl (msec) */
    ulong	 lsp_gen_interval[ISIS_NUM_LEVELS]; /* Min gen ivl (msec)*/
    mgd_timer	 L1_prc_timer;	      /* Partial route calculation holddown */
    ulong	 prc_interval[ISIS_NUM_LEVELS]; /* Min partial rt calc ivl */
    sys_timestamp checksum_timer;     /* validation of database frequency */
    sys_timestamp adj_holddown_timer; /* Hold down hellos until expired */
    mgd_timer    L1_lsp_build_timer;  /* Build L1 LSP when expired */
    mgd_timer    L2_lsp_build_timer;  /* Build L2 LSP when expired */
    uchar        L1_changed:1;	      /* set when L1 LSP should be built */
    uchar        L2_changed:1;        /* set when L2 LSP should be built */
    uchar        run_L1_spf:1;        /* set when level-1 change occurs */
    uchar        run_L2_spf:1;        /* set when level-2 change occurs */
    uchar        L1_routes_changed:1; /* set when Dijkstra is run for L1 */
    uchar	 some_lsps_changed:1; /* set when one or more LSPs change */
    uchar        hippity:1;           /* this IS is overloaded */
    uchar        force_hippity:1;	/* We don't want to be used in SPFs */
    uchar	 ignore_att_bit_clns:1;	/* Never try to get out of area */
    uchar	 ignore_att_bit_ip:1;	/* Never inject 0/0 in IP rt */
    uchar	 never_set_att_bit:1;	/* Never claim to be way out of area */
    uchar        run_dr_election:1;   /* We need to run DR election */
    uchar	 inject_backups:1;    /* Inject NLSP potential pseudonode */
    uchar	 ignore_cksum_errs:1; /* Ignore LSP checksum errors */
    uchar	 log_adj_changes:1;   /* Log adjacency changes */
    uchar	 long_route_display:1;/* Display detailed IP route info */
    uchar	 lsp_hash_enable:1;   /* Hash (do not pack) LSP data */
    uchar	 enable_summary:1;    /* Allow NLSP route aggregation */
    uchar	 non_pnode_summary:1; /* Use non-pseudonode lsp for agg rt */

    /*
     * Special IS-IS partition avoidance kludge.  If the "partition-avoidance"
     * router subcommand has been issued, and there are no L1 or ES 
     * adjacencies, we will stop sending any IIHs.
     */
    uchar	 partition_avoid_on:1; /* Partition avoidance enabled */
    uchar	 partition_avoid_ok:1; /* No L1 or ES adjacencies present */
    dynamic_bitfield SRM_mask;	      /* one bit per IS-IS interface */
    isis_tree_type *level1_LSPs;      /* head of level 1 LSP AVL tree */
    isis_tree_type *level2_LSPs;      /* head of level 2 LSP AVL tree */
    isis_lspix_type *lspix_table;     /* LSPIX array */
    ushort       lspix_count;	      /* Size of LSPIX array */
    mgd_timer    isis_age_timer;      /* Ages LSPs and routes */
    isis_ddbtype *route_table[CLNS_NETHASHLEN]; /* Level 1 routing table. */
    ushort	 current_L1_version;  /* Used to invalidate route table. */
    ushort	 current_L2_version;  /* Used to invalidate route table. */
    isis_spf_log *L1_spf_log;	      /* L1 SPF log */
    isis_spf_log *L2_spf_log;	      /* L1 SPF log */
    chunk_type	 *backup_chunks;      /* Chunk header for backup structures */
    isis_cleanup_queue *cleanup_queue;/* Cleanup queue */
    isis_fastpsnp_type *fastpsnp_cache;/* Most recently received LSPs */

    /* Duplicate System ID detection hack. */

    mgd_timer	 systemid_dup_timer;  /* Measure the time */
    ulong	 regen_count;	      /* Measure the count */

    /*
     * The following fields are related to running integrated IS-IS.
     */
    uchar        isis_mode;        /* Bits for OSI, IP, IPX */
    queuetype    osi_idbq;         /* Interfaces enabled for IS-IS for IP. */
    queuetype    ip_idbq;          /* Interfaces enabled for IS-IS for OSI. */
    queuetype	 ipx_idbq;	   /* Interfaces enabled for IS-IS for IPX. */

    /*
     * The following fields are related to running IS-IS for IPX nets.
     */
    novpdbtype   *ipx_pdb;         /* IPX pdb, NULL means no integrated IPX. */
    isis_proc_type proc_type;	   /* IS-IS or NLSP (used as an index!) */
    char	 *errmsg_name;	   /* name for use in error messages */
    uchar        lsp_rt_frags;     /* Fragment count for (hashed) routes */
    uchar        lsp_svc_frags;    /* Fragment count for (hashed) svcs */
    uchar        lsp_agg_frag;     /* Fragment for first summary if hashing */
    sys_timestamp ipx_redist_maxhold;/* Timer for holddown limiting */

    /*
     * The following fields are related to running IS-IS for TCP/IP nets.
     */
    pdbtype      *ip_pdb;            /* IP pdb, NULL means OSI-only mode. */
    queuetype    ip_address_sumq;    /* IP addr summary entries configured. */

    /*
     * Scratchpad fields for calculating SPF.
     */
    isis_lspdbtype **isis_tent;		/* Pointer to TENT list */
    int isis_tent_count;		/* Number of entries on TENT */
    int min_tent;			/* Minimum distance in use */
    int max_tent;			/* Maximum distance in use */
    int suspend_count;			/* Count of suspends in SPF */
    ushort isis_nr_best_L2s;		/* How many equal cost ways out */
    ushort isis_best_L2s[MAXROUTES];	/* Nearest L2 routers by lsp index */
    int isis_best_L2_cost;		/* Cost to get to closest L2 routers */
    isis_ddbtype *isis_best_L2;		/* Routing table entry for CLNS */
};

/*
 * Integrated IS-IS modes. A bit string is used.
 */
#define ISIS_NULL_MODE 0x00
#define ISIS_OSI_MODE  0x01
#define ISIS_IP_MODE   0x02
#define ISIS_IPX_MODE  0x04
#define ISIS_AT_MODE   0x08

#define ISIS_BACKUP_CHUNK_INCR 100	/* Backup entry chunk size */

#define ISIS_IP_LSP_BUILD_DELAY ONESEC	/* Delay LSP builds this much */

/*
 * The following address formats are used to distinguish between different
 * protocol families.
 */
enum {ISIS_NULL_FORMAT, ISIS_SYSTEMID_FORMAT, ISIS_STATIONID_FORMAT,
	  ISIS_NSAP_FORMAT, ISIS_IP_FORMAT, ISIS_IPX_FORMAT, 
	  ISIS_IPX_EXT_FORMAT, ISIS_IPX_SVC_FORMAT, ISIS_AT_FORMAT,
	  ISIS_IPX_SUMMARY_FORMAT};


/*
 * CLNS route cache flags (idb->clns_routecache). Really bit positions.
 * Moved here from h/interface_private.h.
 */
# define CLNS_FAST_ROUTECACHE		0x00000001
# define CLNS_SSE_ROUTECACHE		0x00000002

/*
 * clns_cachetype
 *
 * Each entry in the clns cache has a 'next' pointer for the
 * hash value collision list and a pointer to a route descriptor block.
 * Note that the route descriptor block is still on its original
 * queue (either the conforming or non-conforming database).
 *
 * Each cache entry consists of a linked list of equal cost routes
 * for this destination. This list wraps. The pointer into the list
 * will change each time the entry is used to switch a packet.
 */

#define MAXMACSTRING  	52		/* TR ISO2 frame + full length RIF */

typedef struct clns_cache_adjtype_ {
    struct clns_cache_adjtype_ *next;
    ulong        uses;
    sys_timestamp last_used;		/* Time last used */
    uchar	 dst_snpa[STATIONLEN_MAX];/* MAC addr for dst, native form */
    clns_adjtype *adj;
    ushort	 length;
    uchar	 rewrite[MAXMACSTRING];
} clns_cache_adjtype;

typedef struct clns_cachetype_ {
    struct clns_cachetype_ *next;
    ulong              cache_version;
    sys_timestamp      cache_expiration;
    NSAP_T             destination;
    clns_cache_adjtype *cache_adj;
} clns_cachetype;

/*
 * Structure used for clns_staticQ.
 */
typedef struct clns_staticQtype_ {
    struct clns_staticQtype_ *next;
    NSAP_T                   prefix;
    NSAP_T                   next_hop;
    idbtype                  *idb;
    hwaddrtype               snpa;
} clns_staticQtype;

/*
 * Structure used for storing locally configured NETs.
 */
typedef struct clns_local_nettype_ {
    struct clns_local_nettype_ *next;
    NSAP_T net;
} clns_local_nettype;

extern void clns_add_cache_rewrite (paktype *pak, ulong dest_addr);

#define L2PREFIX_LENGTH	7 	/* length of the string _DOMAIN prefixed to 
				   the domain names */

#endif __CLNS_ROUTE_H__

