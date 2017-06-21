/* $Id: decnet.h,v 3.2.60.6 1996/08/14 02:36:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet.h,v $
 *------------------------------------------------------------------
 * decnet.h -- definitions for DECnet protocol family
 *
 * June 1987, C. Hedrick
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decnet.h,v $
 * Revision 3.2.60.6  1996/08/14  02:36:09  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.2.60.5  1996/07/22  23:06:58  asastry
 * CSCdi62165:  DECnet fast-switching should be able to use DECnet default
 * route
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/05/30  23:42:46  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/05/30  01:19:25  asastry
 * CSCdi58810:  DEC 'advertise' route is wiped out by "dec routing"
 * command.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/29  19:07:51  asastry
 * CSCdi46634:  Misc DECnet code cleanup - get rid of unwanted registry
 * call.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/09  14:12:23  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.88.1  1996/04/27  06:34:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:45:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.66.1  1996/03/22  09:35:22  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/05  05:49:15  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.40.1  1996/02/27  20:40:56  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/02/13  00:17:20  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.2  1995/11/17  08:58:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/07  19:11:01  asastry
 * CSCdi33368:  Router shouldn't send DECnet Node Unreachable when dialing
 * on DDR
 *
 * Revision 2.2  1995/07/07  22:46:30  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.1  1995/06/07 20:22:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DECNET_H__
#define __DECNET_H__

#ifndef __ACCESS_H__
#include "access.h"			/* for dnacctype in prototypes */
#endif

#include "address.h"			
#include "packet.h"

#define DN_MIN_ACL 1
#define DN_MAX_ACL 100

/*
 * The DECnet addressing structure gives a max of 1024
 * nodes.  (Actually, since 0 isn't allowed, it is less,
 * but we ignore that.)  In addition, an individual machine
 * can set a maximum allowed address.
 */

#define DN_MAXNODES 1024    /* size of routing table */
#define DN_MAXAREAS 64

#define NSP_CONNECT 0x18
#define NSP_RECONNECT 0x68
#define NSP_CON_CONFIRM 0x28
#define NSP_DISC_INITIATE 0x38
#define NSP_DISC_CONFIRM 0x48

#define DN_MAC_CANONICAL	0xAA000400
#define DN_MAC_SWAPPED		0x55002000
#define DN_MAC_TR		DN_MAC_SWAPPED
#define DN_MAC_ETHER		DN_MAC_CANONICAL
#define DN_MAC_ADDRESS		DN_MAC_CANONICAL

/*
 * On the VAX, you can set maxima for total number of Ethernet
 * routers that a node will talk to and the number on any one
 * Ethernet.  Default and max is 33 for max per Ethernet.
 * Default is 32 for max the node will talk to.  I suspect
 * that real DECnet nodes use split horizon on hello messages.
 * We don't, which means that we can't have more than 33
 * total.  If somebody really needs 33 per Ethernet, you
 * will have to implement split horizon.  The following
 * parameter is set to 32, since we don't count ourselves.
 * Note that the hello packet size is overhead + 7*routers.
 * 7 * 32 = 224, so we don't have to worry about splitting
 * up hello packets.  If you increase this drastically, which
 * you can't do unless DEC changes the specs, you may have to
 * worry about splitting up hellos.
 */
#define DN_MAXROUTERS 32  /* number of routers in hello msg */

#define T2 ONESEC  /* delay between broadcasts */

/*
 * The spec defines DRDELAY as 5 * T2.  This is 5 sec.  With a normal
 * hello time of 15 sec, this means we may claim to be designated before
 * having a chance to hear from everybody else.  This is just plain
 * crazy.  I use the same criteria as expiring a hello.  So DRDELAY
 * is not actually used anywhere in this code.
 */

#define DRDELAY (5 * T2)  /* delay before declaring self designated */

/* trigger a L1 update */
#define TRIGGER_L1_ROUTES(S) { \
      if (!TIMER_RUNNING(dn_special_L1_routing)) \
          TIMER_START(dn_special_L1_routing, T2); \
      if (decnet_rt_debug && S) \
        buginf("\nDNET-RT: route update triggered by %s", S); \
    }
/* trigger a L2 update */
#define TRIGGER_L2_ROUTES(S) { \
      if (!TIMER_RUNNING(dn_special_L2_routing)) \
          TIMER_START(dn_special_L2_routing, T2); \
      if (decnet_rt_debug && S) \
        buginf("\nDNET-RT: route update triggered by %s", S); \
   }

#define TRIGGER_HELLOS(S) { \
     if (!TIMER_RUNNING(dn_special_hellos)) \
        TIMER_START(dn_special_hellos, T2); \
     if (decnet_rt_debug && S) \
       buginf("\nDNET-RT: hello update triggered by %s", S); \
   }
/*
 * Macro to check if the adjacency is up AND the adjacency count in the
 * corresponding interface is more than 0 before decrementing it.
 */
#define DECREMENT_L1_ADJ_COUNT(thisroute) { \
    if ((((thisroute)->flags & DN_RT_STATE_MASK) == DN_RT_STATE_UP) && \
	((thisroute)->interface->dn_L1_adj_count > 0)) \
	--(((thisroute)->interface)->dn_L1_adj_count); \
    }
 
/*
 * Similar macros for Phase IV+ and non-Phase IV+ adjacency counters
 */
#define DECREMENT_PHASEIV_PLUS_ADJ_COUNT(thisroute) { \
    if ((((thisroute)->flags & DN_RT_STATE_MASK) == DN_RT_STATE_UP) && \
	((thisroute)->interface->dn_phaseIV_plus_adj_count > 0)) \
	--(((thisroute)->interface)->dn_phaseIV_plus_adj_count); \
    }

#define DECREMENT_OLD_PHASEIV_ADJ_COUNT(thisroute) { \
    if ((((thisroute)->flags & DN_RT_STATE_MASK) == DN_RT_STATE_UP) && \
	((thisroute)->interface->dn_old_phaseIV_adj_count > 0)) \
	--(((thisroute)->interface)->dn_old_phaseIV_adj_count); \
    }


/*
 * DECnet header structure definition.
 *
 * Beware that fields "link" through "ettype" match the paktype and arptype
 * structures defined in network.h and arp.h.
 */

typedef struct dnhdrtype_ {
    unsigned char paksize[2];		/* size not including this, swapped */
    unsigned char pakstart[1];		/* first byte */
} dnhdrtype;

#define dnheadstart(ptr) ((ptr)->network_start)

/*
 * Size of paksize in header structure
 */
#define DN_PAKSIZE_FIELD_LEN 2

/*
 * Largest DECnet datagram size in bytes, less encapsulation
 */
#define MAXDNDATASIZE BIGDATA

/*
 * Nothing in decnet is fixed, because of the problem of padding.
 * The following can be used as overlays
 * WARNING: Because of padding, we have no guarantees as to alignment.
 * So we have to handle everything bytewise.
 */

/* flags for first byte of message */
#define DN_CTL		0x1	/* control message - hello or routing */
#define DN_DATA		0x2	/* always 1 for data message (not ctl) */
#define DN_LONG		0x4	/* has Phase V address - should always be on */
#define DN_RETREQ	0x8	/* return requested */
#define DN_RET		0x10	/* this is being returned */
#define DN_LOCAL 	0x20	/* source/dest are on same Ethernet */
#define DN_PHASE3 	0x40	/* is phase 3 - we don't handle this */
#define DN_PAD 		0x80	/* padding byte - skip it */
#define DN_FP		0x10	/* Four Prime bit when DN_CTL is set */

/*
 * control byte type field when DN_CTL is set
 */
#define DN_CTL_TYPE_MASK	0x0e	/* mask for the type field */
#define DN_CTL_TYPE_RT_L1	0x06	/* level 1 routing */
#define DN_CTL_TYPE_RT_L2	0x08	/* level 2 routing */
#define DN_CTL_TYPE_HELLO_RTR	0x0a	/* hello from a router */
#define DN_CTL_TYPE_HELLO_END	0x0c	/* hello from an endnode */
#define DN_CTL_TYPE_EXT		0x0e	/* extended type */

/*
 * extended types
 */
#define DN_CTL_EXTTYPE_MASK	0x70	/* mask for extended type field */
#define DN_CTL_EXTTYPE_PFPEH	0x00	/* Phase IV Prime Endnode Hello */
  
#define DN_RT_LVL1	0x7	/* level 1 routing */
#define DN_RT_LVL2	0x9	/* level 2 routing */
#define DN_HELLO_RTR	0xb 	/* hello from a router */
#define DN_HELLO_END	0xd	/* hello from an endnode */

/* overlays for types we plan to process */

typedef struct dnhellotype_ {
    uchar version,dececo,usereco; /* version info */
    uchar senderhiaddr[4];	/* AA000400 */
    uchar sender[2];		/* byte swapped DECnet address */
    uchar nodetype;		/* info about sender node */
    uchar blocksize[2];		/* byte swapped */
/* router only */
    uchar priority;		/* router priority */
    uchar area;			/* in this release, MB 0 */
    uchar hello_int[2];		/* hello int, sec., needs multipler */
    uchar odd;			/* reserved, in our sample it is 0x0f*/
    uchar count1;		/* count of rest of message */
    uchar zeros7[7];		/* must be zero */
    uchar count2;		/* count of rest of message */
    uchar adjacencies[1];	/* adjacent nodes start here */
/* endnode only */
    uchar end_hello_int[2];	/* hello int, sec., needs multipler */
} dnhellotype;

#define DN_HELLO_VERSION 2		/* all we handle */
#define DN_FUNNY_PHASEIV_HELLO_VERSION 2
                                        /* This version is used by Phase V
					 * routers. It will be accepted by
					 * Phase IV end nodes and ignored
					 * by Phase V end nodes. (Phase V
					 * routers will also send ISHes)
					 */

#define DN_PHASEIV_PLUS_VERSION 1        /* Version 2.1 hello refers
 					  * to a Phase IV+ hello
 					  */

#define DN_PHASEIV_VERSION 0             /* "Vanilla" Phase IV hello is 2.0 */

/* 
 * node types in hello.  There are some high order bits, but we don't
 * support them.  We believe it is OK to ignore those extra bits.  If
 * not, change DN_NT_MASK.
 */
#define DN_NT_LEVEL2	1		/* router */
#define DN_NT_LEVEL1	2
#define DN_NT_END	3		/* end node */
#define DN_NT_MASK	0x07		/* look at these bits only */

#define DN_MAXPACKET	1498
#define DN_MINPACKET	576

/* 
 * the following macros are used to hide byte order dependencies.  It
 * should be possible to use the code for either byte order simply by
 * changing macros
 */
#define dn_area(array) (((array)[1])>>2)
#define dn_node(array) ((array)[0] | (((array)[1] & 0x3) << 8))
/* node-id is 16 bits with area number at high order end */
#define dn_id(array) ((array)[1] << 8 | (array)[0])
/* dn_idn is id in network order */
#define dn_idn(array) ((array)[0] << 8 | (array)[1])
#define dn_addr(array,area,node) {(array)[0] = (node); (array)[1] = (area)<<2 | (((node)>>8)&0x3);}
#define get16(array) ((array)[1] << 8 | (array)[0])
#define put16(array,value) {\
				(array)[0] = (value) & 0xff;\
				    (array)[1] = ((value)>>8) & 0xff;\
			}
#define put16t(array,value) {register t = (value); (array)[0] = t; (array)[1] = t>>8;}
#define dn_getid(node,area) ((node) | ((area) << 10))
/*
 * adjacencies are 7 bytes: ethernet address and then one byte with
 * the routing priority and the following bit
 */
#define DN_ADJ_UP	0x80		/* adjacent node is up */
/*
 * Macro to get the decnet address as ushort from the dnroutetype structure
 */
#define GET_DECNET_ADDR(address) *(ushort *) ((dnroutetype *)(address))->gateway;


typedef struct dnmetrictype_ {
  short cost;
  short hops;
} dnmetrictype;

/*
 * MAX_ENCAP_BYTES is defined to be 6 for LANE. If someone else comes along,
 * then this needs to be changed to a higher value. In that case, an encap
 * length field in the route entry will be handy to copy only the
 * correct number of bytes, while fast switching.
 */
#define MAX_ENCAP_BYTES 6		/* Byte count in encap_info */

typedef struct dnroutetype_ {
    idbtype *interface;			/* net to get to this guy */
    sys_timestamp routetimer;		/* expires at this point */
    ushort cost;
    short numroutes;			/* number of indirect routes */
    uchar gateway[2];
    hwaddrtype snpa;
    uchar hops;			        /* metric info */
    uchar flags;
    uchar prio;
    uchar type;                         /* Phase IV or V or both? */
    dnmetrictype *vector;		/* metric vector if a router */
    dnmetrictype *areavector;
    struct dnroutetype_ *nextadj;	/* next adjacency in this bucket */
    struct dnroutetype_ *nextsplit;	/* next split route to use */
    boolean is_int_static_route;        /* TRUE if it's an interface static route */
    uchar encap_info[MAX_ENCAP_BYTES];		/* For ATM interfaces */
    ulong cache_version;
} dnroutetype;

/* 
 * We can have both routes from routing msgs and from adjacencies.
 * For an end-node, we use whichever is best.  If there's a route
 * that is lower cost than an adjacency, we keep it, but also keep
 * around the adjacency later on the list.  For a router, it is not
 * legal to use an adjacency as a route.  Again, we keep both routes
 * from routing msgs and from adjacencies on the list, with the
 * adjacency last.  We violate the protocol slightly.  We use the
 * first element on the list for routing, unless it has DN_RT_ROUTER
 * set.  In that case we have an adjacency with a router but no
 * route from it.  Don't see how that could happen, but we're not
 * allowed to route to an adjacency for a router.
 */

#define DN_RT_ROUTER 0x1	/* adjacency from a router */
#define DN_RT_ME     0x2	/* my entry */
#define DN_RT_HELLO  0x4	/* any adjacency */
#define DN_RT_WAS_STATIC 0x8	/* this adjacency superseded a static one */
#define DN_RT_NET    0x10	/* route to other net */
#define DN_RT_PRIME  0x80	/* phase iv-prime adjacency */
#define DN_RT_STATE_MASK 0x60	/* adjacent router state mask, 2 bit field */
#define DN_RT_STATE_REJ  0x00	/* adjacency rejected */
#define DN_RT_STATE_DOWN 0x20	/* adjacency down */
#define DN_RT_STATE_INIT 0x40	/* adjacency initializing */
#define DN_RT_STATE_UP   0x60	/* adjacency up */
#define DN_RT_STATE_NONE 0x01	/* no previous state, never stored */

typedef struct dnroutingmsg_ {
  uchar source[2];     /* source of this message, usual DECnet addr */
  uchar fill1;         /* must be zero */
/*
 * At this point we have a series of submessages of the form
 *   short count
 *   short first node covered
 *   repeat count times
 *     short node info
 * After all of those, there is a short checksum
 * All shorts are byte-swapped
 */
} dnroutingmsg;

/*
 * DN_MAX_RTGROUP is used to limit the number of hosts in one
 * submessage.  I have no idea why they don't just use one
 * big submessage, but VAXes never seem to use more than 32,
 * so we don't either.
 */
#define DN_MAX_RTGROUP 32	/* no more than this in one group */

#define DN_DAT_DSTOFF 6	    /* offset to DECnet part of dest etherad */
#define DN_DAT_VISITOFF 17  /* offset to visit count */

typedef struct dndatapacket_ {
  uchar typecode;
  uchar dest_area;		/* currently not used */
  uchar dest_subarea;		/* not used */
  uchar dest_hi[4];		/* dest ether address */
  uchar dest[2];		/* last 2 bytes - DECnet address */
  uchar source_area;		/* not used */
  uchar source_subarea;		/* not used */
  uchar source_hi[4];		/* source ether address */
  uchar source[2];		/* last 2 bytes - DECnet address */
  uchar next_level_2;		/* ?? */
  uchar visits;			/* increment once for each gateway */
  uchar service;		/* ?? DEC20 just sets it to 0 */
  uchar protocol;		/* ditto */
  uchar databyte;		/* first byte of data */
} dndatapacket;

#define SIZEOF_DNDATAPACKET 21

/*
 * Bits in next_level_2, originally a "reserved" field.
 */
#define DN_RTFL2_CON	0x01		/* congestion encountered */

/*
 * NSP message types in msgflg byte
 */
#define SDID 0				/* source and destination id */
#define DSID 1				/* destination id */
#define RSVD 2				/* reserved */

/*
 * Network Services Protocol disconnect/template packet
 */
typedef struct dnnspdispkt_ {
    uchar msgflg;		/* flags byte */
    uchar dstaddr[2];		/* logical link destination */
    uchar srcaddr[2];		/* logical link source */
    uchar reason[2];		/* disconnect reason */
    uchar data;
} dnnspdispkt;

/*
 * Network Services Protocol connect packet
 */
typedef struct dnnspconpkt_ {
    uchar msgflg;		/* flags byte */
    uchar dstaddr[2];		/* logical link destination */
    uchar srcaddr[2];		/* logical link source */
    uchar services;		/* ??? */
    uchar version;
    uchar segsize[2];
    uchar data;             /* one of data + 18 fake connect */
} dnnspconpkt;

#define SIZEOF_DNNSPCONPKT 10

/* DECnet statistics */

#define DNST_RECEIVED		0
#define DN_FORMATERR		1
#define DN_NOTGATEWAY		2
#define DN_NOTIMP		3
#define DNST_HELLOS		4
#define DN_BADHELLO		5
#define DN_NOTLONG		6
#define DNST_DATAS		7
#define DN_BIGADDR		9
#define DN_NOROUTE		10
#define DN_NOENCAP		11
#define DNST_LVL1S		12
#define DN_BADLVL1		13
#define DN_TOOMANYVISITS	14
#define DNST_HELLOSNT		15
#define DNST_LVL1SNT		16
#define DNST_FORWARD		17
#define DN_NOMEM		18
#define DN_OTHER_AREA_HELLO	19
#define DN_OTHER_AREA_LVL1	20
#define DNST_LVL2S		21
#define DNST_LVL2SNT		22
#define DN_NOVEC		23
#define DN_OTHER_LVL2		24
#define DN_NOACCESS		25
#define DN_NOTNET    		26
#define DN_NOMAP     		27
#define DNST_LOCAL		28	/* packet had local destination */
#define DN_RETURNED  		29	/* packet was returned */
#define DN_NOTCONNECT		30	/* local destination but not connect*/
#define DNST_CONVERT_IV         31      /* Phase V converted to Phase IV */
#define DNST_CONGEST		32	/* debug only ??? can't leave in interrupt code */

#define DNST_MAX		33

#define DN_SUCCESS               0
extern int dn_stat[DNST_MAX];


/*
 * Default Configuration Parameters
 */

#define DN_HELLO_TIME 15	/* time between hellos */
#define DN_ROUTE_TIME 40	/* time between routing updates */
#define DN_MAX_ACTIVATION 10	/* activate router at least this often */
/*
 * DN_LONGTIME is an "infinity" used to initialize timers.  If I really
 * believed in the timer macros, I'd use 0x80000000, but I don't.  If
 * there are no hellos or routing updates scheduled, we'll get an
 * activiation of the router after this time, but even 60 sec is probably
 * good enough, since activiations are that expensive.
 */
#define DN_LONGTIME 70000	/* arbitrary large number */
/*
 * Normally DECnet uses 8*hello to expire adjacencies, and when
 * an adjacency expires, removes its routes.  We don't have that
 * complex a data structure.  This seems good enough
 */
#define DN_ROUTE_EXPIRE 130	/* route expiration */
#define DN_HELLO_EXPIRE 120	/* adjacency expiration */
#define DN_MAX_COST 1022	/* ignore routes with greater cost */
#define DN_MAX_HOPS 30		/* ignore routes with greater hop count */
#define DN_MAX_AREA_COST 1022	/* ditto for area routes */
#define DN_MAX_AREA_HOPS 30
#define DN_MAX_BCST_ROUTERS 65535 /* maximum routers on a segment */
#define DN_MAX_SPLITS 31	/* max split paths */
#define DN_MAX_VISITS 63	/* throw away packets after this */
#define DN_MAX_PRIO 127		/* highest routing priority */
#define DN_MAX_AREA 63		/* largest legal area */
#define DN_MAX_NODE 1023	/* largest legal node number */

#define DN_DEF_NODE 1023	/* default largest legal node number */
#define DN_DEF_PRIO 64		/* default routing priority */
#define DN_DEF_CATHRESH 1	/* default congestion threshold */

/*
 *  Per-network DECnet variables.
 */
#define DN_MAXNETS 4		/* max number of DECnet networks */
				/* This could be set bigger */

/*
 * for hashing a decnet address into the map table
 */
#define dnhash(val) (((val) ^ (val>>8)) & 0xFF)
#define DNNHASH 256		/* size of mapping hash table */

/*
 * The following value must be set to the value of NDECNETCMDS found in
 * decnet.cmd.
 */
#define NDECNETCMDS_H 24

/*  
 *  Map a DECnet address to another network
 */
typedef struct _dnmap {
    ushort fromaddr;			/* from address */
    ushort toaddr;			/* address to map to */
    ushort tonetn;			/* network to map to */
    char cost;				/* administrative cost */
    char hops;				/* administrative hops */
    long use;				/* use count */
    struct _dnmap *next;		/* next map in list */
    struct _dnmap *reverse;		/* next map in reverse map */
} dnmap;

/*
 * Structure to hold information for 'dec advertise' routes.
 */
typedef struct _dn_adv_info {
    ushort cost;
    uchar  hops;
} dn_adv_info;
/*
 * The most important data structure is dnareas/dnroutes.  This contains
 * all the routes, and also all the adjacencies.  It is indexed by host
 * number or area number.  For a local system (i.e. one that we keep track
 * of with hello), what is there is a list of adjacencies.  There can be
 * more than one adjacency even for a single host, because we may be
 * hearing them via more than one interface.  For an area, there is an
 * adjacency for every area router we are hearing hellos from.
 */
struct dnnet_ {
    /* dnareas and dnroutes must stay in this order */
    dnroutetype *dnareas[DN_MAXAREAS];	/* area routing table */
    dnroutetype *dnroutes[DN_MAXNODES];	/* level 1 routing table */
    dnroutetype **dn_maxroute;		/* last in dnroutes that is non NULL */
    int my_dn_area;
    int my_dn_node;
    ushort my_dn_address;		/* my address in network order */
    ushort netn;			/* this network number */
    uchar decnetaddr[6];
    int *vars[NDECNETCMDS_H];		/* pointers to command variables */
    int dn_max_cost;			/* ignore routes with greater cost */
    int dn_max_hops;			/* ignore routes with greater count */
    int dn_max_area_cost;		/* ditto for area routes */
    int dn_max_area_hops;
    int dn_max_visits;			/* throw away packets after this */
    int dn_prio;			/* our routing priority (default) */
    int dn_max_node;			/* max node included in routing msgs */
    int dn_max_area;			/* max area included in routing msgs */
    int dn_max_paths;			/* max split paths */
    int doareas;			/* on to act as area router */
    boolean attached;                   /* TRUE, if we have links to
					   other areas */
    int dn_interim_mode;		/* TRUE if interim path splitting */
    boolean iv_prime;			/* phase-iv prime flag */
    dnmap *nethash[DNNHASH];		/* net mapping hash table */
    dnmap *reverse[DNNHASH];		/* reverse index hash table */
    dn_adv_info dn_adv_area[DN_MAXAREAS]; /* info about 'dec advertised'
					     routes */
    boolean propagate_static;           /* if TRUE, include static routes in
					  routing updates */
};

/*
 * The following constants are used to mark each adjacency
 *    Adjacencies can be 
 *        a) Phase IV only
 *        b) Phase V only
 *        c) Phase IV or Phase V
 *        d) OSI_INJECTED, which is one created by the 'dec advertise' cmd
 *        e) PHASEIV_STATIC, which is one created by the 'dec route' cmd
 *
 * End systems adjacencies will be marked 'PhaseIV only' or PhaseV only'
 * Routers will be marked according to which packet types they will 
 * accept. (NOTE that Phase V routers may accept either packet format).
 */
#define PHASEIV 0x1
#define PHASEV  0x2
#define OSI_INJECTED 0x4
#define PHASEV_PHASEIV (PHASEIV | PHASEV)
#define PHASEIV_PLUS 0x8
#define PHASEIV_STATIC 0x10



typedef struct dn_tr_multi_t_ {
    ushort upper;
    ulong PACKED(lower);
} dn_tr_multi_t;
    
typedef struct dn_info_ {
   dn_tr_multi_t dn_tr_iv_routers, /* dn all routers functional */
 	         dn_tr_iv_prime_routers, /* dn all prime routers fnc */
 	         dn_tr_iv_endnodes; /* dn all endnodes functional */
} dn_info;



/* #define DEBUG */
#define dnet_pabuginf if (decnet_debug) buginf
#define dnet_cobuginf if (decnet_conn_debug) buginf
#define dnet_robuginf if (decnet_rt_debug) buginf
#define dnet_pktbuginf if (decnet_pkt_debug) buginf
#define dnet_adjbuginf if (decnet_adj_debug) buginf

/*
 * Structure used for decnet_staticQ.
 */
typedef struct decnet_staticQtype_ {
    struct decnet_staticQtype_ *next;
    ushort                   dn_addr;
    ushort                   next_hop_dn_addr;
    ushort                   cost;
    ushort                   hops;
    idbtype                  *idb;
    hwaddrtype               snpa;
    boolean                  is_int_static_route;   /* TRUE if it's an interface
						       static route */
} decnet_staticQtype;

/*
 * Structure used for decnet_advQ. All DEC advertised routes are maintained
 * in a special Q, so that they can be reinstated when DECnet routing is
 * restarted.
 */
typedef struct decnet_advQtype_ {
    struct decnet_advQtype_ *next;
    uchar                    area;
    ushort                   cost;
    ushort                   hops;
} decnet_advQtype;


/*
 * DECnet access lists
 */
static inline dnacctype *dn_get_acl (int number)
{
    return((dnacctype *)accessarray[number]);
}

static inline void dn_set_acl (int number, dnacctype *acl)
{
    accessarray[number] = (accessptr *)acl;
}

#endif __DECNET_H__
