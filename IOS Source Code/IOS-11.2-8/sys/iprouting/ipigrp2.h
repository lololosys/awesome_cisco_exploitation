/* $Id: ipigrp2.h,v 3.2.60.8 1996/09/10 18:09:59 sluong Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipigrp2.h,v $
 *------------------------------------------------------------------
 * DUAL definitions
 *
 * April 1992, Bob Albrightson
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipigrp2.h,v $
 * Revision 3.2.60.8  1996/09/10  18:09:59  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.2.60.7  1996/06/27  04:41:08  mikel
 * CSCdi37749:  <debug ip eigrp> help wrong
 * Branch: California_branch
 * clean up ip-eigrp AS debugging
 *
 * Revision 3.2.60.6  1996/06/18  16:46:43  dkatz
 * CSCdi60515:  EIGRP summaries stay in topo table
 * Branch: California_branch
 * Rework the decision process for doing topo table searches for
 * the min metric under a summary.  Get rid of the metric count
 * field.  Add a hook to suppress multiple enqueued requests for
 * a min metric search.
 *
 * Revision 3.2.60.5  1996/06/18  16:42:31  dkatz
 * CSCdi59446:  EIGRP doesnt advertise route matching unused summary
 * Branch: California_branch
 * Don't suppress the advertisement of a route on non-summarized
 * interfaces if the route matches the summary and there are no
 * component routes to the summary.  Add logging of summary table
 * changes.  While the hood is up, fix a problem where the peer
 * table would thrash when adjacencies are rejected.
 *
 * Revision 3.2.60.4  1996/06/18  01:47:39  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/05/27  22:45:29  dkatz
 * CSCdi21082:  IP-EIGRP: Auto-summary mishandled when configuring no ip
 * address
 * Branch: California_branch
 * Rewrite autosummary code.  Optimize a couple of unnecessary topo
 * database walks.  Fix potential multicast mishandling.
 *
 * Revision 3.2.60.2  1996/04/08  04:33:51  dkatz
 * CSCdi53903:  EIGRP stops receiving hellos after interface transition
 * Branch: California_branch
 * Don't disable multicasts when the interface goes down due to natural
 * causes.
 *
 * Revision 3.2.60.1  1996/04/02  05:37:15  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:34:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/05  02:09:36  mikel
 * CSCdi40200:  EIGRP: incorrect redistributed routes in topology table
 * check to see an existing drdb already exists in the topology table.  If
 * so,  don't add overwrite the existing drdb.  This also eliminates
 * continuous updates seem when mutually redistributing
 * between different EIGRP processes
 *
 * Revision 2.4  1995/09/23  21:40:44  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.3  1995/09/07  01:45:48  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.2  1995/08/04  03:07:56  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.1  1995/06/07  21:07:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPIGRP2_H__
#define __IPIGRP2_H__

#include "../iprouting/dual.h"
#include "../iprouting/route.h"

/*
 * Constant definitions.
 */
#define IPIGRP2_PROT      88

#define IPIGRP2_REQUEST   0x0101	/* Unused */
#define IPIGRP2_METRIC    0x0102
#define IPIGRP2_EXTERNAL  0x0103

/*
 * External TLV value definitions.
 */
enum IPIGRP2_PROT_ID {
    NULL_PROTID,
    IGRP1_PROTID,
    IGRP2_PROTID,
    STATIC_PROTID,
    RIP_PROTID,
    HELLO_PROTID,
    OSPF_PROTID	,
    ISIS_PROTID,
    EGP_PROTID,
    BGP_PROTID,
    IDRP_PROTID,
    CONNECTED_PROTID
};

/*
 * Delay for aging routes
 */
#define IPIGRP2_AGE_DELAY (ONESEC)	/* Only age routes this often */

/*
 * External TLV flag bit definitions.
 */
#define INTERNAL_FLAGS_BIT 0x01
#define CD_FLAGS_BIT       0x02

/*
 * Used for decoding multiple destinations in an ipigrp2_metrictype
 */
#define IPIGRP2EXTPAKMACRO \
    ipaddrtype PACKED(routerid); /* Router ID of injecting router */       \
    ulong PACKED(asystem);      /* Originating autonomous system number */ \
    ulong PACKED(tag);          /* Arbitrary tag */                        \
    ulong PACKED(metric);       /* External protocol metric */             \
    ushort PACKED(ereserved);	/* Fluff */                                \
    uchar PACKED(protocol);     /* External protocol ID */             	   \
    uchar PACKED(flag);         /* Internal/external flag */

#define IPIGRP2MPDECODE \
    uchar PACKED(mask);         /* Size of netmask in bits */              \
    uchar PACKED(number[1]);    /* 1 to 4 significant bytes of IP address */

typedef struct ipigrp2_mpdecode_ {
    IPIGRP2MPDECODE
} ipigrp2_mpdecode;

typedef struct ipigrp2_metrictype_ {
    IGRP2TLMACRO
    ipaddrtype PACKED(nexthop);
    IGRP2METRICPAKMACRO
    ipigrp2_mpdecode int_dest[0];	/* At least one per TLV */
} ipigrp2_metrictype;

/*
 * Kludge exterior flag bit into part of reserved field within
 * IGRP2METRICPAKMACRO
 */
#define IPIGRP2_METRICTYPEFLAGS(ptr) (((igrp2_mdecode *)ptr)->mreserved[1])

typedef struct ipigrp2_externtype_ {
    IGRP2TLMACRO
    ipaddrtype PACKED(nexthop);
    IPIGRP2EXTPAKMACRO
    IGRP2METRICPAKMACRO
    ipigrp2_mpdecode ext_dest[0];	/* At least one per TLV */
} ipigrp2_externtype;

typedef struct ipigrp2_extdatatype_ {
    IPIGRP2EXTPAKMACRO
} ipigrp2_extdatatype;

/*
 * parameter block for passing around addresses and masks
 */
typedef struct ipigrp2_address_ {
    ipaddrtype address;
    ipaddrtype mask;
} ipigrp2_address;

/*
 * IP-IGRP2 worktype for queuing redistribution route entries.
 */
typedef struct ipigrp2_worktype_ {
    struct ipigrp2_worktype_ *next;
    igrp2_eventtype type;
    union {
	struct {
	    ipigrp2_address dest;
	    int pdbindex;
	    int event;
	} red;
	struct {
	    ipigrp2_address dest;
	    idbtype *interface;
	    boolean up;
	    boolean config;
	} con;
	struct {
	    int pdbindex;
	    boolean sense;
	} rea;
	struct {
	    ipigrp2_address dest;
	} d;
	struct {
	    idbtype *interface;
	} ifd;
	struct {
	    ipigrp2_address dest;
	    boolean add;
	    igrp2metric *metric;
	} sum;
	struct {
	    idbtype *interface;
	    boolean sense;
	} pasv;
    } c;
} ipigrp2_worktype;


/*
 * Return codes from ipigrp2_summary_revise
 */
typedef enum summary_revise_result_ {
    SUMMARY_NO_CHANGE,			/* Don't update summary */
    SUMMARY_UPDATE_METRIC,		/* Update summary with new metric */
    SUMMARY_FIND_METRIC			/* Search for new metric and update */
} summary_revise_result;

/*
 * IP-IGRP2 traffic statistics
 */
typedef struct ipigrp2_traffic_t_ {
    ulong inputs;
    ulong outputs;
} ipigrp2_traffic_t;


#define IPIGRP2_DTARGET(target,dd) (ipigrp2_debug ? (dd->debug_target_set ? target == dd->debug_target.ip_network.address : TRUE) : FALSE)

#endif __IPIGRP2_H__
