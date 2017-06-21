/* $Id: route.h,v 3.11.20.19 1996/08/28 21:00:55 albright Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route.h,v $
 *------------------------------------------------------------------
 * route.h -- IP routing definitions
 *
 * 22-March-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route.h,v $
 * Revision 3.11.20.19  1996/08/28  21:00:55  albright
 * CSCdi65258:  Timers basic is not working correctly under IGRP
 * Branch: California_branch
 *
 * Revision 3.11.20.18  1996/08/27  20:20:49  bcole
 * CSCdi67283:  redistribute odr creates routing process instead of
 * deferring
 * Branch: California_branch
 *
 * Make ODR code manage a process of its own so that the IP routing parse
 * time code can work with it as expected.
 *
 * Revision 3.11.20.17  1996/08/16  17:54:12  dawnli
 * CSCdi62695:  rip not advertising networks that are passive of other
 * protocols
 * Branch: California_branch
 *
 * Do not copy NETF_PASSIVE into ndb->attrflags, since it will be
 * interpreted as NET_SUPERNET.
 * And rip version 1 does not advertise any supernets
 *
 * Revision 3.11.20.16  1996/08/06  08:28:12  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.11.20.15  1996/08/03  23:30:33  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.11.20.14  1996/07/30  06:09:11  dawnli
 * CSCdi61869:  Wrong next-hop selection if set interface defined in
 * route-map.
 * Branch: California_branch
 * Define new route lookup routine based on a given interface.
 * Call it to set the right next-hop when route-map set interface
 * defined.
 *
 * Revision 3.11.20.13  1996/06/28  02:49:13  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.11.20.12  1996/06/28  01:21:06  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.11.20.11  1996/06/24  16:05:29  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.11.20.10  1996/06/18  16:46:48  dkatz
 * CSCdi60515:  EIGRP summaries stay in topo table
 * Branch: California_branch
 * Rework the decision process for doing topo table searches for
 * the min metric under a summary.  Get rid of the metric count
 * field.  Add a hook to suppress multiple enqueued requests for
 * a min metric search.
 *
 * Revision 3.11.20.9  1996/06/03  23:24:03  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.11.20.8  1996/05/09  14:31:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.20.7  1996/05/04  01:22:21  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.11.20.6  1996/04/27  05:53:27  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.11.20.5.4.1  1996/04/27  07:05:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.11.20.5  1996/04/24  01:29:04  myeung
 * CSCdi54714:  no-redistribution does not work in NSSA
 * Branch: California_branch
 *
 * Revision 3.11.20.4  1996/04/22  05:33:08  myeung
 * CSCdi48106:  OSPF fail to install parallel ex rt with forw addr through
 * sum rt
 * Branch: California_branch
 *
 * Revision 3.10.2.3  1996/04/26  01:39:15  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.10.2.2  1996/04/17  03:06:56  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.10.2.1  1996/02/04  06:29:02  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.11.20.3  1996/04/16  19:02:13  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.11.20.2  1996/04/15  21:18:23  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.11.20.1  1996/04/02  05:37:20  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.11.36.1  1996/04/08  01:56:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.11.26.1  1996/03/22  22:52:47  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.11.10.1  1996/03/05  06:14:50  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.11.4.1  1996/02/27  21:09:27  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11  1996/02/04  06:07:48  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.10  1996/01/12  17:55:08  albright
 * CSCdi44810:  eigrp wakeup timers ignored by new scheduler
 *
 * Revision 3.9.6.1  1996/02/15  19:17:42  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.9  1995/12/10  00:42:39  pst
 * CSCdi38750:  Allow static routes with both next hop and interface
 *              also add "permanent" keyword for non-flapping static routes
 *
 * Revision 3.8  1995/12/10  00:35:13  pst
 * CSCdi45335:  Make RIPv2 less conservative about mask guesing and fix
 * default
 *
 * Revision 3.7  1995/12/07  22:29:51  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/02  10:13:04  pst
 * CSCdi44892:  RIP: do clean up pass on authentication and version
 * control code
 *
 * Revision 3.5  1995/12/02  09:05:54  rchandra
 * CSCdi43948:  ISIS-IP LSP contain non-zero metric with I/E bit set
 * - prevent the parser from accepting more than 63 as metric for
 *   redistributed routes
 * - while adding the L1 learned routes into L2, do not allow the metric
 *   to be more than 63
 *
 * Revision 3.4  1995/11/19  06:07:52  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Return indication from route_add_neighbor() as whether routing tables
 * were modified in order to avoid unnecessary route adjustment.
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.3  1995/11/17  17:36:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:10:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:06:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/23  20:59:23  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.9  1995/10/04  22:15:18  rchandra
 * CSCdi41548:  BGP : wrong nexthop could be advertised to EBGP peers
 * - while doing the last resort brute force nexthop calculation, pass the
 *   mask to iprouting_first_hop to pick the correct entry.
 *
 * Revision 2.8  1995/09/25  08:31:11  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/09/23  21:40:56  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.6  1995/09/11  23:50:46  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.5  1995/09/06 17:27:51  pst
 * CSCdi39840:  IP route cache maintenance needs more subtle invalidation
 * code
 *
 * Revision 2.4  1995/08/24  22:34:39  bcole
 * CSCdi39279:  NHRP: Aggregation needed to provide purge notification
 *
 * Provide aggregated NHRP responses.
 * Cache now consists of radix tree(s) instead of hash table.
 * Allow NHRP resolution during encapsulation of NHRP purge messages.
 * For IP, NHRP now a routing process so as to receive callbacks.
 * Allow for radix tree walk where varargs have already been pre-collected.
 *
 * Revision 2.3  1995/08/24  22:28:55  albright
 * CSCdi38766:  Static routes not redistributed into EIGRP after clear ip
 * route *
 *
 * Revision 2.2  1995/06/28  03:21:49  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:10:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "../clns/isis_linkage.h"

#define CLASS_A_BIT  0x80000000		/* off for class A, on for B, C, D */
#define CLASS_B_BIT  0x40000000		/* off for class B, on for C, D */
#define CLASS_C_BIT  0x20000000		/* off for class C, on for D  */
#define CLASS_D_BIT  0x10000000		/* off for class D */

#define CLASSA_BIT         0x80		/* off for class A, on for B, C, D */
#define CLASSB_BIT         0x40		/* off for class B, on for C, D */
#define CLASSC_BIT  	   0x20		/* off for class C, on for D  */
#define CLASSD_BIT  	   0x10		/* off for class D */

#define CLASS_A_MASK 0xFF000000		/* mask for class A net */
#define CLASS_B_MASK 0xFFFF0000		/* mask for class B net */
#define CLASS_C_MASK 0xFFFFFF00		/* mask for class C net */
#define CLASS_D_MASK 0x00000000		/* mask for class D net??? */

#define	CLASS_A_SHIFT	24		/* A: bits shift to make int */
#define	CLASS_B_SHIFT	16		/* B: bits shift to make int */
#define	CLASS_C_SHIFT	8		/* C: bits shift to make int */

#define CLASSD_AND_GREATER(addr) (((addr) & 0xe0000000) == 0xe0000000)

#define MAXROUTES 6			/* up to six multipath routes */
#define DEFMAXROUTES 4			/* but normally four */
#define MAXDEFNETWORKS 10		/* maximum default networks */

#define METRIC_DIRECT 0			/* directly connected */
#define METRIC_INACCESSIBLE 0xFFFFFFFF	/* inaccessible - 32 bits */
#define METRIC_UNKNOWN 0xFFFFFFF0	/* unknown value */
#define ISIS_MAX_LINK_METRIC 63		/* max isis link metric */

/*
 * Administrative Distance definitions
 */

#define CONNECTED_DISTANCE      0
#define STATIC_DISTANCE         1
#define IGRP2_SUMMARY_DISTANCE  5       /* should be lower than IGRP int/ext */
#define BGP_EXTERNAL_DISTANCE	20      /* must be lower than any IGP */
#define IGRP2_DISTANCE          90
#define IGRP_DISTANCE           100
#define OSPF_DISTANCE           110
#define ISIS_DISTANCE           115
#define RIP_DISTANCE            120
#define unused1_DISTANCE        130
#define EGP_DISTANCE            140
#define unused2_DISTANCE        150
#define ODR_DISTANCE            160
#define IGRP2_EX_DISTANCE       170     /* should be lower than any protocol */
#define BGP_LOCAL_DISTANCE      200	/* must be higher than any IGP */
#define BGP_INTERNAL_DISTANCE	200
#define UNKNOWN_DISTANCE        255

#define MINDISTANCE             1
#define MAXDISTANCE             255


/*
 * structure to keep track of information
 * about routes that failed to get into
 * routing table due to administative distance.
 */
typedef struct backuptype_ {
    struct backuptype_ *next;  /* ptr to next backup info */
    ulong pdbindex;            /* the pdb index which tried to add this route*/
    union {
	ulong context_info;        /* protocol specific route context info */
	isis_backuptype *isis_backup; /* The ISIS version */
    } backup_context;
    ushort route_type;         /* type of route */
} backuptype;

#define backup_context_info backup_context.context_info
#define isis_backup_context_info backup_context.isis_backup

/*
 * Subnet mask table
 */

typedef struct masktype_ {
    ipaddrtype mask;		/* A subnet mask */
    uint count;			/* Number of subnets using that mask */
} masktype;

#define MAXMASKS	24

typedef masktype masktabletype[MAXMASKS];

/*
 *  Backup Routes
 *  -------------
 *  Backup routes occur when:
 *      o A routing protocol supplies a pdb->add_route function.
 *      o A route you currently have in the routing table is overwritten
 *        by another routing protocol.
 *      o A route you want to add to the routing table cannot be added 
 *        because the owner of the existing route has better administrative
 *        distance.
 *      o A route in the routing table has gone unreachable and there are
 *        Backup Routes in the queue or the pdb->backupmask indicates that
 *        other protocols attempted to add the route but were unsuccessful
 *        (for the above two reasons).
 *
 *  When a protocol calls network_update() to add a route and there exists
 *  one for the same destination/mask pair, a backuptype structure may be
 *  queued on the ndb. If pdb->malloc_backup is true, the structure is
 *  malloced and queued. ndb->backupmask is a procmask mask for all the 
 *  protocols that attempted to add the route and couldn't. Again, this
 *  behavior only occurs when pdb->add_route is non-NULL.
 *
 *  When a route in the routing table is going to be deleted, each protocol's
 *  pdb->add_route function will be called. The protocol's that are affected 
 *  are only ones indicated in ndb->backupmask. The protocol can take any
 *  action it sees fit. The backuptype structure is always malloced and
 *  freed by the routing table functions. The protocols need not concern
 *  themselves with this maintenance.
 *
 *  The Backup Route mechanism is applicable to ndb_zero as well.
 */

/*
 * Network Descriptor Block
 *
 * Description of various fields in the structure:
 *
 * netnumber
 *  The network portion of the IP address (IP address ANDed with network
 *  mask).
 *
 * netsmask
 *  The network mask, used to determine which bits of the network number
 *  are to be considered significant.
 *
 * attrflags
 *  A general set of flag bits for the ndb.  See description of individual
 *  bits.
 *
 * procmask
 *  The procmask field of the ndbtype structure is used inconsistently 
 *  throughout the system.  As such, I do not know of a way to simply describe 
 *  its purpose.  Instead I will classify where and how the procmask field is
 *  being set and referenced.
 *
 *  Setting of procmask:
 *  For ndbs whose pdbindex denotes them as connected or static,
 *  the procmask specifies the set of all pdbs which reference that ndb via
 *  a "network" router subcommand, plus the pdb denoted by the pdbindex.
 *  
 *  For all other ndbs, the procmask merely denotes the pdb that "owns" the
 *  ndb, ie only denotes 1 pdb, the same one which is already denoted by
 *  ndb->pdbindex.
 * 
 *  Testing of procmask:
 * 
 *  o The most common use is to check
 *  	(ndb->procmask & pdb->distflag) != 0
 *    to decide if an ndb is to be redistributed into the given routing 
 *    protocol. BGP, EGP, ISIS, IGRP2, and OSPF explicitly perform this 
 *    check, and it is also implicitly checked by all other protocols in such 
 *    places as network_redist() and validnextnetwork().
 *  
 *    These uses of ndb->procmask could fall out and be replaced with
 *    INDEX2MASK(ndb->pdbindex) if ndb->procmask only had 1 bit set.
 *  
 *  o Given an ndb pertaining to an interface, determine if that interface
 *    is one on which routing updates should be sent.  Used explicitly by IGRP,
 *    RIP.  Used implicitly by other DV protocols via pdb_majorupdate().
 *  
 *  o Determine if an ndb is to be aged by a pdb.  I believe this code really
 *    means to be checking INDEX2MASK(ndb->pdbindex) not ndb->procmask.  The
 *    two variants are equivalent in this case since ndbs with multiple bits
 *    set also have PROC_NOAGERT set.
 *  
 *  o There are also several places where the procmask is used to determine
 *    ownership of an ndb.  These checks are often buggy as they sometimes 
 *    assume that only 1 bit is set in the procmask (ISIS, IGRP2).
 *
 * advertisingmask
 *
 *  advertisingmask is a bitmask of those pdbs which have the given ndb being
 *  redistributed into them.  Advertisingmask is used by the system to select
 *  which pdbs to notify when ROUTE_DOWN/ROUTE_MODIF events occur on an ndb.
 *  
 *  For pdbs with a redist_callback routine, these pdbs are notified
 *  when this ndb has a ROUTE_UP event.  These pdbs take care of 
 *  setting/resetting their bit in the advertisingmask as part of the work 
 *  done in their callback routine.  For distance vector routing protocols 
 *  which do not have a redist_callback routine, the advertisingmask is
 *  maintained on their behalf by getnextnetwork().
 *  
 * backupmask
 *  See above description of backup routes.
 *
 * passive
 *  A mask of those pdbs which do not wish to send routing updates out
 *  interfaces denoted by the associated ndb.  Pdbs are entered into this
 *  mask if pdb->networkflags has the NETF_PASSIVE flag set for the
 *  network corresponding to this ndb.
 *
 *  As this variable is only tested by pdb_majorupdate, only some protocols 
 *  make use of this flag to limit the interfaces out which they send 
 *  routing updates.
 *
 * blindtimer
 *  A timer, which if not zero, indicates the ndb is in holddown, and
 *  specifies the period for which routing updates concerning this ndb
 *  should be ignored.  This interval is obtained from by pdb->holdtime.
 *
 * nettimer
 *  A timer used to determine how old an ndb is, relevant if the ndb is
 *  learned via a dynamic routing protocol.  Used during route aging to 
 *  determine when to expire an ndb.  The usage of this timer is protocol
 *  dependent. 
 *
 * lastgateway
 *  IP address of the gateway from which we last heard a routing update
 *  indicating reachability of the route referenced by this ndb.  For a
 *  dummy ndb, this contains 0.0.0.0.
 *
 * lastinterface
 *  The interface on which we last heard a routing update
 *  indicating reachability of the route referenced by this ndb.  For a
 *  dummy ndb, this is NULL.
 *
 * metric
 *  Protocol specific routing metric for this route as learned via the
 *  pdb which "owns" this ndb.  Connected ndb's have a metric of 0.
 *
 * distance
 *  Administrative distance of the ndb, generally taken from the pdb.
 *
 * lastindex
 *  When there are multiple rdbs on the ndb, this is the index of the
 *  particular rdb which is the next to be used for routing traffic.  Each
 *  rdb is selected in turn via round robin. 
 *
 * routecount
 *  The number of rdbs associated with this ndb.
 *
 * pdbindex
 *  The index into the ip_proctable of the pdb which is to be considered
 *  the owner of this ndb.
 *
 * attachcount
 *  The number of attached routes denoted by this ndb and the children
 *  of this ndb.  Used to determine when the NET_ATTACHED flag can be
 *  removed from ndb->attrflags.
 *
 * tag
 *  Only used by a limited set of pdbs, in protocol specific ways.
 *
 * isis_backup
 *  Points to a thread of information for ISIS backup routes, when the NDB
 *  was generated by ISIS.  If the pointer is NULL, there is no backup
 *  information.
 *
 * backup_info
 *  See above description of backup routes.
 *
 * area_id
 *       This field contains area id of the area from which this route is
 *       generated. This field will allow the route to be mapped on to an
 *       area database. This mapping is needed during forwarding address 
 *       lookups.  OSPF only.
 *
 * isis_backup
 *	 This field carries a pointer to backup information within the ISIS
 *	 database.  It is used to generate a backup route if the primary
 *	 route goes away, in order to avoid re-running SPF.  ISIS only.
 *
 * spf_instance
 *       This field contains spf instance which generated this route. This is
 *       used to overwrite the ndb if the ndb is from previous instance.
 *
 * route_type -
 *       This field contains type of ospf/IS-IS route. The following routes
 *       type are defined.
 *
 *       RTYPE_INTRA           OSPF route within an area
 *       RTYPE_INTER           OSPF route accross different areas
 *       RTYPE_EXTERN1         OSPF external route of type 1
 *       RTYPE_EXTERN2         OSPF external route of type 2
 *       RTYPE_L1              IS-IS level-1 route
 *       RTYPE_L2              IS-IS level-2 route
 *       RTYPE_OTHER           unknown route type
 *
 * forw_dist
 *       This field contains the forwarding distance from this node to the
 *       advertising boundary router. This field is used for route comparison
 *       for External Type 1 and External Type 2 routes.
 *
 * rdb
 *  A table of pointers to rdbs, representing the actual routes associated
 *  with this ndb.
 *
 * maskcount
 *  Number of valid entries found in masktable.
 *
 * masktable
 *  A possibly null pointer to a dynamically allocated array of IP network
 *  masks.  This field is only used for variably subnetted dummy ndbs, and
 *  indicates the set of netmasks which are in use by children of this ndb.
 *
 * subnets
 *  Nubmer of valid entries found in subnettable.
 *
 * subnettable
 *  A table of pointers to ndbs, indicating those ndbs which are considered
 *  children of the current ndb.  These child ndbs are often referred to
 *  as "sdb"s but that distinction is likely to go away eventually.  
 *  Currently sdbs cannot also have child ndbs of their own.
 *  
 *  When an ndb is dynamically allocated, the size of the ndb is predetermined
 *  to either include or exclude the memory necessary for child "sdb"s.
 */


#define sdbtype ndbtype

struct ndbtype_ {
	struct ndbtype_ *next;	/* pointer to next network descriptor */
	ipaddrtype netnumber;	/* net number, possibly subnet as well */
	ipaddrtype netsmask;	/* mask of net and subnet, eg 255.255.0.0 */
	ulong attrflags;	/* attribute flag word */
	ulong pdb_overmask;	/* process mask */
	ulong advertisingmask;  /* redistributing processes */
	ulong backupmask;       /* pdbs that didn't get route in table */
	ulong passive;		/* flags for passive routing */
	sys_timestamp blindtimer;/* timeout for update blindness */
	sys_timestamp nettimer;	/* timeout for purging network entry */
	ipaddrtype lastgateway;	/* last known gateway */
	idbtype *lastinterface;	/* last known interface */
	ulong metric;		/* path metric */
	short distance;		/* administrative distance */
	short lastindex;	/* index of next route to use */
	short routecount;	/* count of active route blocks */
	short pdbindex;		/* index of owning pdb */
	ushort attachcount;	/* number of attached interfaces */
	ulong tag;		/* arbitrary tag */
	backuptype *backup_info;/* backup routes information */
	union {
	    ulong ospf_area_id;	/* area id in case of OSPF */
	    isis_backuptype *isis_backup; /* Backup ISIS routes if ISIS */
	} proto_ndb;
	ushort spf_instance;    /* spf instance that generated it-OSPF */
	ushort route_type;      /* OSPF-Intra/Inter/Extern, IS-IS L1/L2 */
	ulong forw_dist;        /* spf distance from this node to boundary */
	boolean ospf_nssa_p_bit;
	ulong forw_addr;
	struct rdbtype_ *rdb[MAXROUTES];/* pointers to rdb's */
	int maskcount;		/* count of masks */
	masktype *masktable;	/* ptr to array of masks */
	int subnets;		/* count of subnets */
	struct ndbtype_ *subnettable[1];/* array of ptrs to subnets */
};

/* Convenience defines. */

#define ndb_area_id proto_ndb.ospf_area_id
#define ndb_isis_backup proto_ndb.isis_backup


/*
 * Routing process types, used in pdb->proctype
 */
#define PROC_ILLEGAL	0x0000
#define PROC_CONNECTED	0x0001
#define PROC_STATIC	0x0002
#define PROC_BGP	0x0004
#define PROC_unused1	0x0008
#define PROC_MOBILE	0x0010
#define PROC_RIP	0x0020
#define PROC_NHRP	0x0040
#define PROC_IGRP	0x0080
#define PROC_EGP	0x0100
#define PROC_OSPF	0x0200
#define PROC_ISIS       0x0400
#define PROC_IGRP2      0x0800
#define	PROC_ISOIGRP	0x1000
#define	PROC_NLSP	0x2000
#define PROC_FSTATIC    0x4000
#define PROC_ODR	0x8000

#define	PROC_MAXPROC	PROC_ODR

#define MAX_IP_PROCESS	10
  
#define MAXREDIST	4

#define PROC_REDISTMASK (PROC_STATIC+PROC_IGRP+PROC_MOBILE+ \
			 PROC_RIP+PROC_EGP+PROC_BGP+PROC_OSPF+ \
			 PROC_ISIS+PROC_IGRP2+PROC_CONNECTED+PROC_ODR)
#define PROC_NOSPLITMASK (PROC_EGP+PROC_BGP+PROC_OSPF+PROC_ISIS)

/*
 * Processes that use the new scheduler primitives
 */
#define	PROC_NEWSCHEDULER	(PROC_RIP+PROC_IGRP+PROC_IGRP2+PROC_ODR)

/*
 * Processes that can start right away without a 'network' command.
 */
#define	PROC_NONETWORKCMD	(PROC_BGP+PROC_EGP+PROC_ODR)

/*
 * Processes that don't do supernetting at all.
 */
#define	PROC_NOSUPERNETS	(PROC_IGRP+PROC_EGP)

/*
 * Processes that don't get their routes aged out (non-periodic updates)
 */
#define PROC_NOAGERT    (PROC_CONNECTED + PROC_STATIC + PROC_OSPF + \
			 PROC_BGP + PROC_ISIS)

/*
 * Processes that care about redist callbacks concerning default ndb
 */
#define PROC_MAINTAINDEFAULT	(PROC_OSPF+PROC_ISIS)

/*
 * Processes that support for disconnected subnets
 */
#define PROC_DISCSUBNETMASK (PROC_OSPF+PROC_ISIS)

/*
 * Processes that don't send automatic periodic updates.
 */
#define PROC_NONPERIODIC (PROC_IGRP2+PROC_EGP+PROC_MOBILE+PROC_NHRP)

/*
 * Processes that don't have invalid, holddown, and flush timers.
 */
#define PROC_NOHOLDDOWN (PROC_BGP|PROC_IGRP2|PROC_MOBILE|PROC_NHRP)

#define INDEX2MASK(x)	(1<<x)
#define INDEX2PDB(x)	(ip_proctable[x])
#define NDB2PROCTYPE(x) (ip_proctable[x->pdbindex]->proctype)
#define NDBSUBNETED(ndb) (get_majornet(ndb->netnumber) != ndb->netnumber)

/*
 * Different types of OSPF routes
 */
#define OSPF_IROUTE_TYPES (RTYPE_INTRA | RTYPE_INTER)
#define OSPF_EROUTE_TYPES (RTYPE_EXTERN1 | RTYPE_EXTERN2)
#define OSPF_AROUTE_TYPES (OSPF_IROUTE_TYPES | OSPF_EROUTE_TYPES)

enum pdb_stack {
    PDB_UNKNOWN,		/* Unknown */
    PDB_IP,			/* IP */
    PDB_OSI,			/* OSI */
    PDB_ATALK,			/* AppleTalk */
    PDB_NOVELL,			/* Novell */
    PDB_ALL			/* All protocols */
    };

enum DISTTYPE {
    IP_DIST_BETTER,		/* Better admin distance */
    IP_DIST_WORSE,		/* Worse admin distance */
    IP_DIST_SAME		/* Same admin distance */
};

/*
 * Route attributes in ndb->attrflags
 */
/*
 * Many of these are used by IPX. Yes, annoying, but that's the way
 * it is right now and it will take a major effort to change this, due to
 * EIGRP/ISIS/NLSP interactions.
 */
#define	NET_SUPERNET	    0x1	 /* net is a supernet */
#define NET_FLOATING_STATIC 0x2  /* IPX floating static */
#define NET_STATIC       0x0004  /* net was entered manually */
#define NET_IGRP	 0x0008
#define NET_RIP          0x0010  /* net discovered via RIP */
#define NET_ATTACHED	 0x0020  /* net either static to if or connected */
#define NET_NLSP         0x0040  /* net discovered via NLSP */
#define NET_ISIS         0x0080  /* net discovered via integrated ISIS */
#define NET_XROUTE       0x0100  /* net is nlsp/isis external */
#define NET_CONNECTED    0x0200  /* net is connected via interface(s) */
#define	NET_PERMANENT	 0x0400  /* net is locked in place all the time */
#define NET_INTERIOR    0x01000  /* Igrp interior route */
#define NET_SYSTEM      0x02000  /* Igrp system route */
#define NET_VARIABLE    0x40000  /* net has variable length subnets */
#define NET_SUBNETTED   0x80000  /* net has subnets */
#define NET_DEXTERIOR  0x100000  /* an exterior network, set dynamically */
#define NET_SEXTERIOR  0x400000  /* an exterior network, set statically */
#define NET_NOREDIST   0x800000  /* never redistribute this network (BGP) */
#define NET_SUBNET    0x1000000  /* subnet route */
#define NET_SUMMARY   0x2000000  /* summary route */
#define	NET_TARGET    0x4000000  /* has been used for recursive routing */
#define NET_INTERNAL  0x8000000  /* Internally generated static route */ 
#define NET_PER_USER 0x10000000  /* Per user static route from tacacs+ */ 

#define NET_EXTERIOR	(NET_SEXTERIOR+NET_DEXTERIOR)
#define NET_PERMATTR	(NET_ATTACHED|NET_SUBNETTED|NET_SUBNET|NET_SEXTERIOR|\
			 NET_SUPERNET|NET_TARGET)

/*
 * Well known masks
 */
#define MASK_UNKNOWN	0x0	/* Mask unknown or not specified */

#define LONGEST_MASK(a,b) (a | b)
#define SHORTEST_MASK(a,b) (a & b)

/*
 * Route Descriptor Block
 */

#define ROBIN_DEFAULT 1		/* default round robin weight */

/*
 * Routing Descriptor Block
 *
 * This block contains a single route.  There may be multiple routes per
 * ndb.
 * 
 * gateway 
 * 	This field contains the IP address of the next hop system.  It is
 * 	zero if there is no next hop.
 *
 * infosource 
 *	This is the IP address of the router that told us the information.
 *	It is zero if we are the source of the information.
 *
 * idb 
 *	This points to an idb iff the route is known to be fixed to that
 *	interface.  This can be NULL if the route needs a recursive lookup
 *	to be done, such as for BGP or for unnumbered interfaces
 *
 * metric  
 *	The composite metric for this route.  It gets METRIC_INFINITY if
 * 	the route is ill.
 *
 * The following fields are not defined for ISIS.  They are valid only if
 * the IP_RDB_ISIS and IP_RDB_OSPF flags is clear in rdb_flags.
 *
 *   delay 
 * 	The total delay for the route, used by IGRP and EIGRP.
 *
 *   bandwidth 
 *	The minimum bandwidth for the route, used by IGRP and EIGRP.
 *
 *   mtu 
 *	The minimum mtu for the route, used by IGRP and EIGRP.
 *
 *   routetimer 
 *	This is set to msclock when the route is updated.  It is used by
 *	all of the DV protocols for aging the route.
 *
 * The following fields are *only* defined for OSPF.  They are valid only if
 * the IP_RDB_OSPF flag is set in rdb_flags.
 *
 *   lsdb
 *	Linkage to the LSA that create this RDB.
 *
 *   area
 *	Area the LSA belong to.
 *
 * The following fields are *only* defined for ISIS.  They are valid only if
 * the IP_RDB_ISIS flag is set in rdb_flags.
 *
 *   lsp_links
 *	Linkage to thread this RDB from the ISIS LSP that created it.
 *	Also contains the LSP index and the LSP version number.  Used to
 *	keep track of which routes were generated by which LSP.  If the
 *	LSP index is zero, the LSP version number is really the SPF instance
 *	instead.  This is used to track the version of routes that don't
 *	originate from LSPs, but are generated when a full SPF is done (such
 *	as the nearest-L2 default).
 *
 *   ndb
 *	Back pointer to the NDB.  Necessary for ISIS so that the NDB can be
 *	accessed from the RDB while traversing the thread.
 *
 * load 
 *	The load for the route, used by IGRP and EIGRP.  This is a
 *	percentage of available bandwidth in use, stored as an integer
 *	fraction, scaled by 255.  Thus 0 -> empty link, 255 -> saturated.
 *
 * reliability 
 *	The reliability of the route, used by IGRP and EIGRP.  Stored
 *	similarly to "load". 
 *
 * robin & robin_reset 
 *	Robin is the number of packets that remain to be sent using this
 * 	rdb before looking at the next rdb.  When robin goes to zero, it is
 *	set to robin_reset, and we increment lastindex to point to the next
 *	rdb. 
 *
 * rdb_flags
 *	Various flag bits.  See below for definitions.
 */

/* rdb_flags definitions */

#define IP_RDB_SNAPSHOT_ROUTE (0x01)    /* Route is a snapshot route */
#define IP_RDB_ISIS	      (0x02)	/* Route is from ISIS */
#define	IP_RDB_PERMANENT      (0x04)	/* Route is nailed up */
#define IP_RDB_OSPF	      (0x08)	/* Route is from OSPF */

struct rdbtype_ {
	ulong refcount;			/* (opaque!) number of users */
	ipaddrtype gateway;		/* gateway, or zero if d/c */
	ipaddrtype infosource;		/* who, if anyone, told us */
	idbtype	*idb;			/* pointer to associated IDB */
	ulong metric;			/* metric for this particular route */
	union {
	    struct {
		isis_lsp_linkage lsp_links; /* ISIS lsp thread */
		ndbtype *ndb; 		/* NDB back pointer for ISIS */
	    } isis_rdb;			/* ISIS RDB parameters */
	    struct {
		lsdbtype *lsdb;		/* Pointer to LSA */
		areatype *area;		/* Area of the LSA */
	    } ospf_rdb;			/* OSPF RDB parameters */
	    struct {
		ulong delay;		/* composite delay */
		ulong bandwidth;	/* composite bandwidth */
		ulong mtu;		/* composite MTU */
                ulong nexthop_metric;   /* the metric that next hop sees */
		sys_timestamp routetimer; /* time out for obsolescence */
	    } other_rdb;		/* non-ISIS RDB parameters */
	} proto_rdb;
	short pdbindex;			/* the pdb which owns this rdb */
	uchar load;			/* composite loading */
	uchar reliability;		/* composite reliability */
	uchar hopcount;			/* hop count (starting with R8.0) */
	uchar robin_reset;		/* reset value for round robin use */
	uchar robin;			/* current round robin value */
	uchar rdb_flags; 		/* various status bits for RDB  */
};

/* Convenience for non-ISIS protocols */

#define rdb_delay proto_rdb.other_rdb.delay
#define rdb_bandwidth proto_rdb.other_rdb.bandwidth
#define rdb_mtu proto_rdb.other_rdb.mtu
#define rdb_nexthop_metric proto_rdb.other_rdb.nexthop_metric
#define rdb_routetimer proto_rdb.other_rdb.routetimer

/* Convenience for ISIS */

#define isis_rdb_ndb proto_rdb.isis_rdb.ndb
#define isis_rdb_linkage proto_rdb.isis_rdb.lsp_links

#define	RDB_CHUNK_MAX	100

/* Convenience for OSPF */
#define ospf_rdb_lsdb proto_rdb.ospf_rdb.lsdb
#define ospf_rdb_area proto_rdb.ospf_rdb.area


/*
 * Protocol Descriptor Block for various routing protocols
 */

#define MAXPEERS 30
#define MAXNETWORKS 200

#define NETF_PASSIVE  0x01        /* network doesn't receive updates */
#define NETF_BACKDOOR 0x02        /* network is backdoor (used by BGP) */

#define MINOUTDELAY     8         /* min. output delay in ms */
#define MAXOUTDELAY     50        /* max. output delay in ms */

typedef struct peertype_ {
    struct peertype_ *next;
    ipaddrtype source;
    sys_timestamp timer;
    int distance;
    int distance2;
    acl_headertype *acl;
} peertype;


typedef struct re_metrictype {
    boolean flag;		/* true if mapping metric to static value */
    ulong tag;
    boolean tag_set;		/* true if the tag is set */
    ulong match_tag;
    boolean subnets;
    int weight;			/* redist into BGP only */
    ulong type;
    union {
	ulong scalar;
	struct vector {
	    ulong bandwidth;
	    ulong user_bandwidth;    /* user specified bandwidth for display */
	    ulong reliability;
	    ulong delay;
	    ulong loading;
	    ushort mtu;
	}vector;
    } metric;
} re_metrictype;

typedef struct re_criteria {
    long routetype;		/* mask of route types
				   e.g. 1<<PTYPE_EXTERN1 */
} re_criteria;

/*
 * Summary address data structure. Currently only used by IP-IGRP2.
 * sum_idbtype is data structure for summary->idbQ.
 */
typedef struct summarytype_ {
    struct summarytype_ *next;
    ipaddrtype address;
    ipaddrtype mask;
    queuetype  idbQ;
    ulong      min_metric;
    uchar      being_recalculated:1; /* Set if metric is to be recalculated */
} summarytype;

typedef struct sum_idbtype_ {
    struct sum_idbtype_ *next;
    idbtype             *idb;
    boolean	configured;	/* TRUE if configured, FALSE if automatic */
} sum_idbtype;

/*
 * Structure to hold offset information
 */
typedef struct offsettype_ {
    ulong metric;
    acl_headertype *acl;
}offsettype;

/*
 * IP routing related information in the idb.
 */
struct ipinfotype_ {

    /*
     * Mobile IP support.
     */
    boolean mobile_arp;			/* TRUE if mobile ARP enabled */
    boolean mobile_proxy;		/* TRUE if mobile proxy enabled */
    boolean mobile_beaconing;		/* TRUE if beaconing enabled */
    boolean mobile_foreign;		/* TRUE if foreign agent */
    boolean mobile_foreign_auth;	/* TRUE if authentication required */
    boolean mobile_home;		/* TRUE if home agent */
    boolean mobile_node;		/* TRUE if mobile node */
    boolean mobile_solicit;		/* TRUE to send solicits to FA */
    boolean mobile_regreq;		/* TRUE if registration required */
    ulong mobile_arp_holdtime;		/* holdtime */
    ulong mobile_arp_keepalive;		/* keepalive timer */
    idbtype *mobile_coa_idb;		/* care-of-address interface */

    ushort mobile_sequence_num;		/* sequence # for beaconing */

    acl_headertype *mobile_arp_permit_list;	/* access list of permits */
    acl_headertype *mobile_foreign_list;  /* access list of registrations */

    /*
     * RIP related fields
     */
    int    rip_auth_mode;		/* RIP authentication mode */
    char  *rip_auth_keychain;		/* RIP authentication key */
    ulong  rip_auth_sequence;		/* RIP authentication key sequence */
    ushort rip_send_vers;		/* RIP send version bitstring */
    ushort rip_recv_vers;		/* RIP receive version bitstring */

    /*
     * Offset-list related fields
     */
    offsettype out_offlist[MAX_IPPROCS];/* per protocol offset information */
    offsettype in_offlist[MAX_IPPROCS];

    /*
     * Policy routing
     */
    boolean policy_enabled;		/* TRUE if doing policy routing */
    char *policy_routemap;		/* Route map name */

    /*
     * NAT
     */
    tinybool nat_enabled;		/* NAT is on or off */
    tinybool nat_outside;		/* outside or inside */
};

typedef void (*majorupdatecbtype) (pdbtype *pdb, ndbtype *ndb, ipaddrtype 
				   destination, idbtype *interface, ulong
				   port, ipaddrtype source, ipaddrtype
				   sourcemask); 
typedef void (*add_routecbtype) (pdbtype *pdb, ipaddrtype address, 
				 ipaddrtype mask, backuptype *backup);
typedef void (*lost_routecbtype) (pdbtype *pdb, ndbtype *ndb);
typedef boolean (*route_agercbtype) (pdbtype *pdb, ndbtype *ndb);
typedef void (*redist_callbackcbtype) (pdbtype *pdb, ndbtype *ndb, int
				       event, int flag); 
typedef void (*networkcbtype) (pdbtype *pdb, ipaddrtype address,
			       ipaddrtype mask, uchar flags, ulong area_id,
			       int area_id_format, boolean sense);
typedef void (*redistributecbtype) (pdbtype *pdb, pdbtype *dist, boolean
				    sense); 
typedef void (*processcbtype) (pdbtype *pdb);
typedef void (*cleanupcbtype) (pdbtype *pdb);
typedef void (*neighborcbtype) (pdbtype *pdb, ipaddrtype address, parseinfo
				*csb, boolean sense);
typedef void (*areacbtype) (pdbtype *pdb, parseinfo *csb, boolean sense);
typedef boolean (*feasiblecbtype) (pdbtype *pdb, rdbtype *rdb,
                                   ulong bestmetric);
typedef boolean (*holddowncbtype) (pdbtype *pdb, ndbtype *ndb, rdbtype
				   *rdb, ulong newmetric);
typedef void (*querycbtype) (pdbtype *pdb, idbtype *idb);
typedef void (*showipprotocbtype) (pdbtype *pdb);
typedef void (*exteriorcbtype) (pdbtype *pdb, ndbtype *ndb, boolean sense);
typedef int (*distancecbtype) (pdbtype *pdb, ipaddrtype target,
			       ushort route_type, int flags,
			       int *pdb_peerindex);
typedef void (*summaryaddresscbtype) (pdbtype *pdb, boolean sense);
typedef void (*buildbackupinfocbtype) (pdbtype *pdb, ndbtype *ndb,
				       backuptype *backup, ushort route_type,
				       boolean replaced);
typedef void (*rdbcleanupcbtype) (pdbtype *pdb, rdbtype *rdb);
typedef void (*ndbcleanupcbtype) (pdbtype *pdb, ndbtype *ndb);
typedef void (*backupcleanupcbtype) (pdbtype *pdb, backuptype *backup);
typedef void (*ndbinitcbtype) (pdbtype *pdb, ndbtype *ndb, boolean new_ndb);
typedef void (*rdbinitcbtype) (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb);
typedef void (*ndbcopycbtype) (pdbtype *pdb, ndbtype *oldndb, ndbtype *newndb);
typedef void (*ndbpdbchangecbtype) (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb,
				    boolean now_yours);
typedef int (*rdbprintcbtype) (pdbtype *pdb, rdbtype *rdb, char *bp);
typedef void (*ndbprintcbtype) (pdbtype *pdb, ndbtype *ndb);
typedef enum DISTTYPE (*distancecmptype) (pdbtype *, uchar, ndbtype *,
					  ulong, ushort);
typedef void (*aclchangecbtype) (pdbtype *pdb, idbtype *idb, boolean out,
				 boolean sense);
typedef void (*hopschangecbtype) (pdbtype *pdb, uchar);
typedef void (*metriccmdcbtype) (pdbtype *pdb);
typedef void (*defmetriccmdcbtype) (pdbtype *pdb, boolean sense);
typedef void (*routeadjcbtype) (pdbtype *pdb, idbtype *idb);
typedef void (*showsumcbtype) (pdbtype *pdb);
typedef void (*autosumcmdcbtype) (pdbtype *pdb);
union aclU {		/* Allows sharing named and numeric ACLs */ 
    int listnum;
    acl_headertype *acl;
};

struct pdbtype_ {
/*
 * WARNING!!!  The next three fields must be congruent to the same three
 * fields in clns_pdbtype (in clns_route.h)  This is due to the unfortunate
 * practice of coercing the PDB pointer in certain conditions.  Sad, but
 * true.
 */
    struct pdbtype_ *next;	/* pointer to next pdb */
    int family;			/* protocol stack to use */
    ulong proctype;		/* protocol type flag, e.g. PROC_IGRP */
/*
 * End of warning.  We now return you to our regularly scheduled program,
 * which is already in progress.
 */
    char *name;			/* pointer to protocol name string */
    char *pname;		/* pointer to process name string */
    boolean running;		/* TRUE if process already running */
    int pid;			/* process ID of running process */
    int index;			/* routing process index (0 - MAX_IPPROCS) */
    ulong mask;			/* routing process mask */
    ulong distflag;		/* type flags for sending routing updates */
    ulong redistallowed;	/* types of redistribution allowed */
    int version;		/* version of routing protocol */
    add_routecbtype add_route;	/* ptr to route addition routine */
    buildbackupinfocbtype build_backupinfo; /* routine to build backup info */
    lost_routecbtype lost_route; /* inform if route is clobbered by distance */
    boolean malloc_backup;      /* used w/ add_route, want malloced backups */
				/* ptr to route ager verification routine */
    route_agercbtype route_ager;
				/* pointer to redistributing process routine */
    redist_callbackcbtype redist_callback;
    networkcbtype network_cb;	/* ptr to routine to handle network cmd */
				/* pointer to start redistribute routine */
    redistributecbtype redistribute;
    processcbtype process;	/* pointer to routing process */
    cleanupcbtype cleanup;	/* routine to flush routing process */
    neighborcbtype neighbor;	/* configure a neighbor entry */
    areacbtype area;		/* process specific area configure */
    feasiblecbtype is_feasible;	/* ptr to routine to check a potential path */
    holddowncbtype holddown;	/* ptr to predicate for going into holddown */
				/* ptr to routine to send broadcast updates */
    majorupdatecbtype majorupdate;
    querycbtype query;		/* ptr to query routine */
    showipprotocbtype showipproto;
                                /* ptr to routine for protocol specific info */
    exteriorcbtype exterior_cb;	/* ptr to routine for exterior disp. change */
    distancecbtype distance_cb;	/* ptr to routine to fetch admin distance */
                                /* ptr to routine to handle summary address */
    summaryaddresscbtype summary_address_cb;
    distancecmptype distance_cmp;/*ptr to routine to compare admin distances */
    rdbcleanupcbtype rdb_cleanup;/*ptr to routine to clean up RDBs */
    ndbcleanupcbtype ndb_cleanup;/*ptr to routine to clean up NDBs */
    backupcleanupcbtype backup_cleanup;/*ptr to routine to clean up backups */
    ndbinitcbtype ndb_init;	/* ptr to routine to init NDBs */
    rdbinitcbtype rdb_init;	/* ptr to routine to init RDBs */
    ndbcopycbtype ndb_copy;	/* ptr to routine to copy NDBs */
    ndbpdbchangecbtype ndb_pdbchange; /* Update NDB when PDB changes */
    rdbprintcbtype rdb_print;	/* ptr to routine to print RDB info */
    ndbprintcbtype ndb_print;	/* ptr to routine to print NDB info */
    aclchangecbtype acl_changed;/* ptr to routine to inform of ACL change */
    hopschangecbtype hopcount_change; /* routine to inform for maxhop change */
    metriccmdcbtype metric_cmd; /* routine to inform for metric cfg change */
    defmetriccmdcbtype defmetric_cmd; /* inform for deflt metric cfg change */
    routeadjcbtype route_adjust; /* routine to inform for route adjust */
    showsumcbtype show_summary; /* ptr to routine to print summaries */
    autosumcmdcbtype auto_sum_cmd; /* routine to inform for auto summary cmd */
    sys_timestamp timer;	/* our process activation timer */
    sys_timestamp sleep_timer;	/* sleeping timer used for hysteresis */
    ipsocktype *socket;		/* protocol socket, if any */
    queuetype *inputq;		/* pointer to input queue */
    int changed;		/* state change flag */
    sys_timestamp whenchanged;	/* remember when last changed */
    union aclU ifltr_all;	/* access list for all incoming information */
    union aclU ofltr_all;	/* access list for all outgoing information */
    union aclU ofltr_proto[MAX_IPPROCS];
    				       /* lists filtering redisted protocols */
    boolean holdokay;		/* TRUE if holddowns permitted */
    boolean validate_source;	/* TRUE if source of updates to be validated */
    int distance;		/* distance for this protocol */
    int def_distance;		/* default value */
    int distance2;		/* distance 2 for this protocol */
    int def_distance2;		/* default value */
    int distance3;		/* distance 3 for this protocol */
    int def_distance3;		/* default value */
    boolean defaultmetricflag;	/* TRUE if default metric is set */
    ulong default_metric;	/* default metric for readvertised routes */
    ulong broadcasttime;	/* broadcast interval in ticks */
    ulong def_broadcasttime;	/* default value */
    ulong invalidtime;		/* invalid after this many ticks */
    ulong def_invalidtime;	/* default value */
    ulong holdtime;		/* hold-down time in ticks */
    ulong def_holdtime;		/* default value */
    ulong flushtime;		/* flush entry after this many ticks */
    ulong def_flushtime;	/* default value */
    ulong sleeptime;            /* value to load into sleep_timer */
    short multipath;            /* maximum number of paths to remember */
    short def_multipath;	/* default value */
    sys_timestamp spf_time;	/* spf wait start time */
    ulong spf_wait_interval;    /* wait interval before running spf */
    boolean run_spf;		/* spf run flag */
    boolean atomic_update;	/* indicates spf unsed for route generation */
    ushort area_count;		/* number of areas attached to this pdb */
    ospfbackuptype *add_requests;    /* list route add requests from IP table */
    int netcount;		/* count of networks we're routing on */
    ipaddrtype networks[MAXNETWORKS];/* the table of network numbers */
    ipaddrtype network_mask[MAXNETWORKS];/* the table of network masks */
    uchar networkflags[MAXNETWORKS]; /* flags for our networks */
    ulong network_info[MAXNETWORKS]; /* other related info */
    char *network_map[MAXNETWORKS]; /* BGP: route-map to modify attributes */
    int network_weight[MAXNETWORKS]; /* BGP: weight modifier for network */
    boolean sync_disabled;	/* set if no IGP synchronization */
    int peercount;		/* count of values in peer table */
    struct peertable {		/* the peer table */
	ipaddrtype address;
	ipaddrtype mask;
	int distance;
	int distance2;
	acl_headertype *acl;
    } peers[MAXPEERS];
    peertype *peercache[NETHASHLEN]; /* the cache of peers we've talked to */
    struct neighbortype_ *neighbors; /* pointer to possible neigbors */
    struct neighbortype_ *promiscuous; /* pointer to a default neighbor
					  configuration */
    struct neighbortype_ *peergroup_list; /* list of BGP peer groups */
    short pfilter;		/* filter for accepting promiscuous
				   connections */
    boolean default_originate;	/* do we create defaults */
    boolean default_always;	/* Create default always */
    queuetype ip_address_sumq;	/* queue of summary routes */
    boolean auto_summary;	/* should we do implicit summarization? */
    uchar *default_routemap;    /* route-map reference on default-info orig */
    uint traffic_share;		/* Algorithm for computing traffic shares */
    /*
     * Arguments to network_update.  The type and meaning varies by
     * protocol. 
     */
    ulong arg1;

    /*
     * Arguments returned from network_update.  The type and meaning
     * varies by protocol.
     */
    ulong arg2;

    /*
     * Metric modification fields
     */
    ulong mmetric_in;
    ulong mmetric_out;
    acl_headertype *mmetric_list_in;
    acl_headertype *mmetric_list_out;
    boolean exteriors_out;
    boolean exteriors_in;
    acl_headertype *exteriors_in_acl;
    acl_headertype *exteriors_out_acl;

    /*
     * Route map fields. route_map is used for parameter passing between
     * functions.
     */
    char *routemap[MAX_IPPROCS];
    route_maptype *route_map;
    route_map_nametype *table_map;
    boolean supports_snapshot; /* TRUE if pdb supports d/s */

    /*
     * Protocol Specific Fields
     */
    ushort asystem;		/* our AS number, if any */
    uchar edition;		/* IGRP edition number */
    ulong user_bandwidth;	/* user specified IGRP readvertised bw */
    ulong def_bandwidth;	/* IGRP readvertised bandwidth */
    ulong def_reliability;	/*      "	reliablity */
    ulong def_delay;		/* 	"	delay */
    ulong def_loading;		/*	"	load factor */
    ushort def_mtu;		/*	"	mtu */
    re_metrictype metric[MAX_IPPROCS]; /* metrics used for redistribution */
    re_metrictype metric_def[MAX_IPPROCS]; /* default metrics for redistribu */
    re_criteria criteria[MAX_IPPROCS]; /* criteria for redistribution */
    re_criteria criteria_def[MAX_IPPROCS]; /* default criteria for */
    ushort igrp_hopcount;	/* hopcount of network_update() caller */
    ulong igrp_maxhops;         /* maximum believable hop count */
    ulong igrp_def_maxhops;     /* default maximum hop count */
    ulong igrp_variance;	/* amount of acceptable variance */

    /*
     * IP-IGRP2 specific fields
     */
    ddbtype *ddb;		 /* Pointer to dual descriptor block */
    watched_queue *workQ;	 /* Redist, lost, and backup entries */
    queuetype summaryQ;          /* Configured aggregates */
    boolean shutdown;            /* forces direct calling of dual_linkdown */

    /*
     * For each TOS specified, there needs to be different K factors.
     * For now we are assuming a single TOS.
     */
    ushort igrp_k1;		/* IGRP metric factor 1 */
    ushort igrp_k1_def;		/* IGRP metric factor 1, default */
    ushort igrp_k2;		/* IGRP metric factor 2 */
    ushort igrp_k2_def;		/* IGRP metric factor 2, default */
    ushort igrp_k3;		/* IGRP metric factor 3 */
    ushort igrp_k3_def;		/* IGRP metric factor 3, default */
    ushort igrp_k4;		/* IGRP metric factor 4 */
    ushort igrp_k4_def;		/* IGRP metric factor 4, default */
    ushort igrp_k5;		/* IGRP metric factor 5 */
    ushort igrp_k5_def;		/* IGRP metric factor 5, default */

    ushort egp_hello;		/* EGP hello time */
    ushort egp_poll;		/* EGP poll time */

    sys_timestamp bgp_reusetime;/* time stamp when the reuse array serviced */
    ushort bgp_keeptime;	/* BGP keepalive interval */
    ushort bgp_holdtime;	/* BGP holdtime interval */
    ipaddrtype bgp_rtr_id;	/* BGP router ID */
    struct bgp_cond_adv_maptype_ *bgp_cond_adv; /* list of all conditional
						   advertising maps */

    /*
     * RIP specific
     */
    uint output_delay;          /* RIP inter-frame gap */

    int static_distance;	/* floating static route support */

    ulong *mib_table;           /* pointer to some of the mib values */
    queuetype *ospf_faddrq;	/* pointer to external forward address list */
    queuetype *ospf_exdelayq;	/* pointer to external delay queue */
    int hello_pid;		/* process ID of OSPF Hello process */
    char *hello_pname;		/* Name of Hello process */
    int ospf_scanpid;		/* Process ID of OSPF Scanner Process */
    ulong ospf_scantime;	/* OSPF Scanner interval in milsec */
    queuetype *hello_inputq;		/* pointer to input queue */
    processcbtype ospf_hello_process;	/* pointer to OSPF HELLO process */
    ulong ospf_rtr_id;		/* ospf router id */
    ushort ospf_flag;           /* ospf related flags */
    struct pdbtype_ *ospf_next;	/* pointer to next ospf pdb */
    areatype *ospf_area_list;	/* pointer to ospf area structures */
    boolean ospf_area_brtr;	/* TRUE if area border router */
    boolean ospf_as_brtr;	/* autonoumous system boundary router */
    ulong ospf_default_metric;	/* cost of default route */
    uchar ospf_default_type;	/* Type of link state to generate */
    uchar ospf_tos_option;      /* support for more than one TOS*/
    ushort ospf_nbr_ex_load;    /* is any neighbor forming adjaceny */
    ushort local_ospf_spf_instance;  /* argument to network_update */
    ushort ospf_dbase_lock;       /* are we in the process of running spf */
    sys_timestamp  ospf_run_time;  /* clock used to suspend */
    rbTree  *ospf_ex_lsdbase;	/* external link database */
    lsdbtype *ospf_ex_insert;	/* externals in routing table */ 
    lsdbtype *ospf_ex_delete;	/* externals routes to be deleted from table */
    areatype *ospf_backbone;	/* pointer to backbone neighbor */
    ulong ospf_area_id;   	/* argument to network_update */
    boolean ospf_auto_cost;	/* calculate cost based on bandwidth */
    ushort spf_instance;   	/* used by OSPF/IS-IS, network_update arg */
    ulong ospf_ex_forw_dist;   	/* argument to network_update */
    boolean ospf_nssa_p_bit;
    boolean ospf_ex_forw_addr;
    struct rtr_route_  **ospf_routes;   /* pointer to router routing table */
    clns_pdbtype *osi_pdb;      /* used by IS-IS, clns_pdbtype pointer */
    sys_timestamp ospf_maxage_delete_time; /* timestamp to decide if it is
					      time to delete MAXAGE LSA */
    lsdbtype *ospf_max_headptr;
    lsdbtype *ospf_max_tailptr;
    ushort	ospf_spf_hold_interval; /* length in second that SPF 
					   calculation will wait after
					   it has run once */
    ushort	ospf_spf_schd_delay;	/* time between a SPF is scheduled
					   and when it actually starts */
    sys_timestamp ospf_redist_summary_timer;  /* redistribute summary route
						 timer */
    sys_timestamp ospf_redist_summary_last_time;  /* timestamp of the last
						     redistribute summary 
						     route scanning */
    ospf_pdb_info *ospf_pdb;
};

/*
 * Route types
 * These are mutually exclusive, but having them as a bit field makes
 * comparisons easier.
 */

#define	RTYPE_OTHER	0x0001	/* unknown route type */

/*
 * OSPF route types
 */
#define	RTYPE_INTRA	0x0002	/* route within an area */
#define	RTYPE_INTER	0x0004	/* route accross different areas */
#define	RTYPE_EXTERN1	0x0008	/* external route of type 1 */
#define	RTYPE_EXTERN2	0x0010	/* external route of type 2 */

/*
 * IS-IS route types.
 */
#define RTYPE_L1        0x0020    /* IS-IS level-1 route */	
#define RTYPE_L2	0x0040    /* IS-IS level-2 route */


/*
 * IGRP2 route types.
 */
#define RTYPE_IGRP2_INT 0x0080   /* IGRP2 derived routes. */
#define RTYPE_IGRP2_EXT 0x0100   /* IGRP2 redistributed routes. */

/*
 * BGP route types
 */
#define	RTYPE_BGP_INT	0x0200	/* BGP internal routes (iBGP) */
#define	RTYPE_BGP_EXT	0x0400	/* BGP external routes (eBGP) */
#define	RTYPE_BGP_LOC	0x0800	/* BGP local routes (aggregates) */

/*
 * OSPF NSSA external route types
 */
#define RTYPE_NSSA1	0x1000	/* OSPF NSSA external route of type 1 */
#define RTYPE_NSSA2	0x2000	/* OSPF NSSA external route of type 2 */


/* route events */
#define	ROUTE_UP	1       /* route came up */
#define	ROUTE_DOWN	2       /* route going down */
#define	ROUTE_MODIF	3       /* route modified */


/* SPF locks */
#define	DBASE_INACTIVE    0
#define	DBASE_ACTIVE      1
#define	DBASE_FREEZE      2
#define	DBASE_INVALID     3


/* redistribute callback flags */
#define DEFAULT_ROUTE	1	/* redistribute ndb as default */


/*
 *  Static descriptor block
 */ 

typedef struct ipstatictype_ {
    struct ipstatictype_ *next;
    struct ipstatictype_ *hashnext;
    struct ipstatictype_ *hashprev;
    ipaddrtype network;
    ipaddrtype gateway;
    ipaddrtype netsmask;
    idbtype *interface;
    ulong flags;
    ulong tag;
    int distance;
    hwidbtype *user_hwidb; /* used by tacacs+ for per-user static routes */
} ipstatictype;

/*
 *  Summary descriptor block
 */ 

typedef struct addrsumtype_ {
    struct addrsumtype_ *next;
    ipaddrtype network;
    ipaddrtype mask;
    struct ndbtype_ *template;
    route_map_nametype *attribute_map;
    route_map_nametype *advertise_map;
    route_map_nametype *suppress_map;
    ulong flags;
    ulong forw_addr;
    ulong  metric;
    ushort metric_type;
    ulong  tag;
    ulong  adv_tag;
    ushort best_metric_count;
} addrsumtype;

#define	ADDRSUM_VALID		0x0001		/* summary is valid */
#define	ADDRSUM_HIDDEN		0x0002		/* summary is hidden */
#define	ADDRSUM_AS_SET		0x0004		/* generate AS sets */
#define	ADDRSUM_AUTOFLTR	0x0008		/* automaticly filter */
#define ADDRSUM_NOT_ADV		0x0010		/* For OSPF, do not adv sum */
#define ADDRSUM_EXACT_MATCH	0x0020		/* For OSPF, an exact match */
#define ADDRSUM_OSPF_REDIST	0x0040		/* For OSPF, sum due to 
						   redistribution */
#define ADDRSUM_OSPF_TYPE7	0x0080		/* For OSPF, sum due to
						   type 7 */

/*
 * Structure used to maintain information about a routing process that
 * has been named, but not started.  This is used to carry information
 * over from the 'router' command to the 'network' command.
 */
typedef struct spawntype_ {
    struct spawntype_ *next;
    pdbtype *protocol;
    ipaddrtype address;
    boolean sense;
    uchar networkflags;
    ipaddrtype mask;
    ulong area_id;
    int area_id_format;
} spawntype;

#define OSPF_AREA_ID_FORMAT_IGNORE     	0
#define OSPF_AREA_ID_FORMAT_IP		1
#define OSPF_AREA_ID_FORMAT_INT		2

/*
 * Structure used by display_route_summary to summarize resource usage
 * by IP routing table.
 */

typedef struct ndbstattype_ {
    ulong nets, subnets, overhead, mem;
    ulong rtype[4];
} ndbstattype;

/*
 * Typedef for function passed to table_walk
 */
typedef ndbtype *(*table_walk_proc_type)(ndbtype *, ndbtype *, ndbtype *, 
					 void *);

/*
 * Typedef for function passed to validnextnetwork
 */

typedef boolean (*validtesttype)(ipaddrtype mask, pdbtype *pdb, ndbtype
				 *ndb);



/*
 * Some common global definitions
 */
extern const char ip_pdb_name[];
#ifndef	GROVEL
extern const char showipproto_neighborheader[];
extern boolean default_netsource;
extern boolean igrp_debug;
extern boolean subnetzero_enable;
extern boolean iprouting_classless;
extern boolean iprouting_slow_converge;
extern int default_netcount;
extern ipaddrtype default_gateway;
extern ipaddrtype default_network[];
extern ndbtype *default_ndb;
extern ndbtype *nettable[];
extern ndbtype *ndb_zero;
extern pdbtype *pdb_CONNECTED;
extern pdbtype *pdb_STATIC;
extern queuetype IPsocketQ;
extern queuetype ipstaticQ;
extern queuetype protoQ;
extern ushort egp_asystem;
extern pdbtype *ip_proctable[];
extern ndbtype *supernettable[];
extern ipstatictype *ipstatictable[];
extern masktabletype supernetmasks;
extern int supernetmaskcount;
extern boolean router_init_running;
#endif
/*
 * Prototypes for:
 *
 *    route1.c
 *    route2.c
 *    route3.c
 *    ipstatic.c
 *    ts/path.c
 */

/*
 * route1.c
 */ 

extern void net_init(void);
extern void table_init(void);
extern void table_reload(idbtype *idb);
extern boolean route_interface(idbtype *idb, boolean addflag,
			       boolean dostatics);
extern boolean route_ifaddress(idbtype *idb, ipaddrtype address,
			       ipaddrtype mask);
extern void ip_route_adjust(idbtype *idb);
extern idbtype *iprouting_first_hop(ipaddrtype *, ipaddrtype, ipaddrtype, 
				    int, ulong, ndbtype **);
extern ipaddrtype iprouting_first_hop_on_idb(ipaddrtype, idbtype *, ulong);
extern ulong ip_best_metric(ipaddrtype destination);
extern ndbtype *net_lookup(ipaddrtype address, boolean defaultflag);
extern ndbtype *subnet_lookup(ndbtype *ndb, ipaddrtype address,
			      ipaddrtype mask); 
extern ndbtype *supernet_lookup(ipaddrtype address, ipaddrtype mask,
				boolean defaultflag);
extern ndbtype *netsuper_lookup(ipaddrtype address, ipaddrtype mask,
				boolean defaultflag);
extern ndbtype *ip_rtlookup(ipaddrtype address, ipaddrtype mask);
extern ndbtype *ip_rtlookupdummy(ipaddrtype address, ipaddrtype mask);
extern idbtype *route_lookup(ndbtype *sdb, ipaddrtype *address,
			     ipaddrtype destination, int depth,
			     ulong flags, ndbtype **retndb);
extern ipaddrtype route_lookup_on_idb(ndbtype *, ipaddrtype, idbtype *,
                                      ulong);
extern void iprouting_get_netsmask(ipaddrtype, ipaddrtype *);
extern int get_nets_per_idb(pdbtype *, idbtype *);
extern int get_nets_per_address(pdbtype *, ipaddrtype);
extern ndbtype *ndb_malloc(int bytes);
extern ndbtype *network_create(pdbtype *pdb, ipaddrtype address, ipaddrtype
			       mask); 
extern void network_redist(pdbtype *pdb, ndbtype *ndb, int event);
extern void invalidate_redist(pdbtype *pdb);
extern ipaddrtype ip_ifmask(idbtype *idb, ipaddrtype address);
extern rdbtype *network_update(pdbtype *pdb, ipaddrtype address, ipaddrtype
			       mask, ipaddrtype gateway, idbtype
			       *interface, long metric, long flags,
			       ipaddrtype source, ulong tag,
			       boolean *changed_rdb,
			       ushort route_type);
extern void rdb_delete(pdbtype *pdb, ndbtype *db, int index);
extern boolean simple_variance(pdbtype *pdb, rdbtype *rdb, ulong bestmetric);
extern boolean simple_holddown(pdbtype *pdb, ndbtype *ndb, rdbtype *rdb,
			       ulong newmetric); 
extern boolean table_ager(pdbtype *pdb, ndbtype *pred, ndbtype *ndb);
extern void netarray_ager(pdbtype *pdb, ndbtype *ndb);
extern void age_all_routes(pdbtype *pdb);
extern void route_compress(ndbtype *sdb);
extern boolean ip_valid_splithorizon(ipaddrtype, ndbtype *, idbtype *);
extern boolean validnextnetwork(pdbtype *pdb, ndbtype *ndb, ndbtype *sdb,
				idbtype *interface, ipaddrtype mask,
				ipaddrtype dest, validtesttype test,
				boolean distribute_list_checkflag, 
				pdbtype **return_src_pdb);
extern ndbtype *getnextnetwork(pdbtype *pdb, ndbtype *ndb, ndbtype *sdb,
			       int *prev, idbtype *interface, ipaddrtype
			       mask, ipaddrtype dest, 
			       validtesttype test, boolean subnetflag,
			       boolean distribute_list_checkflag, 
			       pdbtype **return_src_pdb);  
extern ndbtype *find_valid_subnet(pdbtype *pdb, ndbtype *sdb, ndbtype *tdb,
			       int *subprev, idbtype *interface, ipaddrtype
			       mask, ipaddrtype dest, 
			       validtesttype test, boolean subnetflag,
			       boolean distribute_list_checkflag,
			       pdbtype **return_src_pdb);
extern boolean ndb_is_up(ndbtype *ndb);
extern void ip_route_new_default(void);
extern boolean ip_secondary(idbtype *idb, boolean addflag, ipaddrtype address,
			    ipaddrtype mask); 
extern void table_walk(table_walk_proc_type, void *param1); 
extern ndbtype *prune_ndb_idb(ndbtype *pred, ndbtype *db, ndbtype *ndb,
			      void *param);
extern void table_prune_idb(idbtype *idb);
extern ndbtype *prune_ndb_pdb(ndbtype *pred, ndbtype *db, ndbtype *ndb,
			      void *param);
extern void table_prune_pdb(pdbtype *pdb);
extern ndbtype *ip_ndb_subnet(ndbtype *ndb);
extern boolean ip_ndb_variable(ndbtype *ndb);
extern void ip_ndb_addmask(ndbtype *ndb, ipaddrtype mask);
extern void ip_ndb_removemask(ndbtype *ndb, ipaddrtype mask);
extern void ip_supernet_addmask(ipaddrtype mask);
extern void ip_supernet_removemask(ipaddrtype mask);
extern backuptype *check_backup_info(pdbtype *pdb, ndbtype *ndb);
extern backuptype *build_backup_info(pdbtype *pdb, ndbtype *ndb, 
				     ushort route_type, boolean replaced);
extern ndbtype *geteachroute(ndbtype **ndbptr, int *ndb_bucket, 
			     ndbtype **sdbptr, int *sdb_bucket, 
			     boolean);
extern boolean route_add_neighbor(idbtype *, ipaddrtype);
extern void callbackup_routes(ndbtype *ndb);
extern void ip_route_delete(pdbtype *pdb, ipaddrtype address, ipaddrtype
			    mask, ipaddrtype gateway, ipaddrtype source,
			    idbtype *interface, ushort route_type);
extern void ip_robin_setup(pdbtype *pdb, ndbtype *ndb);
extern void ip_route_start_aging(idbtype *idb, paktype *pak);
extern boolean iprouting_cache_mask(ipaddrtype, ipaddrtype *, ipaddrtype *);
extern ulong offset_metric(pdbtype *, idbtype *, ipaddrtype, int);
extern boolean iprouting_recursive(ipaddrtype);
extern boolean iprouting_destination_valid(ipaddrtype, ipaddrtype, idbtype *);
extern void iprouting_shutdown(void);
extern void iprouting_delete_conn_route(ipaddrtype, ipaddrtype);
extern void iprouting_delete_conn_route_int(ipaddrtype, ipaddrtype, idbtype *);
extern boolean iprouting_delete_route(ipaddrtype, ipaddrtype);
extern void iprouting_gdp_default_ndb_prio(ushort *);
extern boolean iprouting_via_nonsubnet(ipaddrtype);
extern boolean iprouting_snmp_rtlookup_exact(ipaddrtype, ndbtype **,
					     ulong *, boolean *);
boolean iprouting_snmp_rtlookup_lexnext(ipaddrtype, ndbtype *,
					ulong *, boolean *);
boolean iprouting_conn_plimit_exceed(ulong);
extern enum DISTTYPE iprouting_distcmp_generic(pdbtype *pdb, uchar,
					       ndbtype *, ulong, ushort);
extern void iprouting_background_onoff(boolean);
extern int ip_find_rdb_index(ndbtype *, rdbtype *);
extern void ndb_free(ndbtype *, boolean);
extern watched_boolean *ip_statics_unsynced;

/*
 * route2.c
 */ 
extern int route_preamble(void);
extern boolean print_subnets(ndbtype *ndb, ulong pdbmask, ipaddrtype address,
			     ipaddrtype mask, boolean prefix);
extern boolean print_this_ndb(ndbtype *ndb, ulong pdbmask, ipaddrtype address,
			      ipaddrtype mask, boolean prefix);
extern char nettype(ndbtype *ndb);
extern char *route_type(ndbtype *ndb);
extern boolean route_out(ndbtype *ndb, int len);
extern pdbtype *ip_get_src_pdb(pdbtype *, ndbtype *, boolean);
extern void default_showipproto(pdbtype *pdb);
extern void showipproto_redistribute(pdbtype *pdb);
extern addrsumtype *ip_get_summary (pdbtype *pdb, ulong address);


/*
 * route3.c
 */ 
extern pdbtype *router_parse(parseinfo *csb, boolean nullokay, int *pdb_type);
extern void ip_router_erase(pdbtype *pdbtype);
extern process router_init(void);
extern void network_finish(pdbtype *pdb, ipaddrtype address, uchar
			   networkflags, ipaddrtype mask, ulong area_id,
			   int area_id_format, boolean sense);
extern void set_ndb_flags(ndbtype *ndb, pdbtype *pdb, ulong attrflags,
			  boolean sense); 
extern void set_ndb_range_flags(ndbtype *ndb, pdbtype *pdb, ipaddrtype address,
				ipaddrtype mask, ulong attrflags,
				boolean sense); 
extern void network_revise(ipaddrtype network);
extern int pdb_distance(pdbtype *pdb, ipaddrtype source, ipaddrtype target,
			ipaddrtype mask, int flags, ushort route_type);
extern void pdb_flushcache(pdbtype *pdb);
extern boolean pdb_sawmajor(idbtype *idb, ipaddrtype address);
extern void pdb_majorupdate(pdbtype *pdb);
extern void pdb_topology(idbtype *idb);
extern boolean pdb_outfilter(pdbtype *pdb, idbtype *interface,
			     pdbtype *src_pdb, ipaddrtype address); 
extern boolean pdb_routemap(pdbtype *, pdbtype *, ndbtype *);
extern boolean pdb_routemap_for_default(pdbtype *);
extern boolean pdb_interface_for_default(pdbtype *, idbtype *);
extern void pdb_killp(pdbtype *, int);
extern boolean is_id_used(ulong address);
extern ipaddrtype allocate_rtr_id(boolean need_unique);
extern int allocate_pdbindex(pdbtype *pdb);
extern void free_pdbindex(pdbtype *pdb);
extern void print_redistribute_command(pdbtype *pdb);
extern void clear_routecommand_func(ipaddrtype, ipaddrtype);
extern ulong ip_get_def_metric_scalar(pdbtype *, int);
extern void ip_get_def_metric_vector(pdbtype *, int, struct vector *);
extern void auto_summary_command(parseinfo *);
extern void iprouting_validate_command(parseinfo *);
extern void ip_defaultnetwork_command(parseinfo *);
extern pdbtype *iprouting_pdb_init_default(ushort);

/*
 * ip_radix.c
 */
extern rntype *ip_radix_nextnode(ipaddrtype, rntype *);
extern rntype *ip_radix_successor(rntype *, caddr_t);
extern rntype *ip_radix_ver_successor(rntype *, ulong version, caddr_t);

/*
 * ipstatic.c
 */ 
extern boolean ip_static_update(ipstatictype *sre);
extern void ip_addstatic(ipaddrtype network, ipaddrtype mask, 
			 ipaddrtype gateway, idbtype *idb, ulong tag, 
			 long flags, long distance, hwidbtype *hdwidb);
extern void ip_deletestatic(ipaddrtype network, ipaddrtype mask, idbtype *idb);
extern void ip_delete_per_user_route(hwidbtype *idb);
extern void ip_static_deleteidb(idbtype *idb);
extern void adjust_static_routes(pdbtype *pdb, idbtype *idb);
extern void ip_static_adjust(void);
extern void ipstatic_callbackup_route(pdbtype *, ipaddrtype address, 
				      ipaddrtype mask, backuptype *backup);

/*
 * ts/path.c  (for lack of a better place)
 */
extern idbtype *find_path(ipaddrtype *, ulong);
extern idbtype *first_hop_no_routing(ipaddrtype *, ipaddrtype, ulong);
extern void ip_redirect_age_cache(void);
extern void ip_redirect_flush_cache(idbtype *);
extern void path_init(void);
extern void path_update(ulong, ulong, idbtype *);
extern void clear_redirect_cache(parseinfo *);
extern boolean ip_host_routing;

/*
 * iprouting/iprouting_init.c
 */
extern void iprouting_init(subsystype *);

/*
 * iprouting/iprouting_setup.c
 */
extern void iprouting_setup_init(setup_struct *);
extern void iprouting_setup_global(setup_struct *);
extern void iprouting_setup_end(setup_struct *);

/*
 * ip/ipfast.c (needs to be moved to iprouting)
 */
extern void ipcache_invalidate(hwidbtype *, idbtype *, boolean);
extern void ipcache_invalidate_range(ipaddrtype, ipaddrtype);
extern void ipcache_invalidate_subrange(ipaddrtype, ipaddrtype, ipaddrtype);
extern void ipcache_invalidate_target(ipaddrtype, ipaddrtype, ipaddrtype);
extern void ipcache_setup_interface(hwidbtype *);
extern boolean ipcache_acl_debug(ipaddrtype prefix, ipaddrtype mask);

#endif __ROUTE_H__

/*
Local Variables:
comment-column: 32
End:
*/
