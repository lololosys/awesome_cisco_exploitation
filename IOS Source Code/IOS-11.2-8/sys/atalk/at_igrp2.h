/* $Id: at_igrp2.h,v 3.2.60.3 1996/05/02 08:40:09 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_igrp2.h,v $
 *------------------------------------------------------------------
 * DUAL/AppleTalk-IGRP2 definitions
 *
 * November 1992, David S.A. Stine. Liberally borrowed from Bob's
 * 				    dual.h/igrp2.h files.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2.h,v $
 * Revision 3.2.60.3  1996/05/02  08:40:09  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.2  1996/04/02  05:35:41  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2.60.1  1996/03/23  01:26:43  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:42:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:49:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/07  01:45:14  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers se
 *
 * Revision 2.2  1995/07/13  06:49:51  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:08:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_IGRP2_H__
#define __AT_IGRP2_H__

#include "../iprouting/dual.h"
#include "../atalk/atalk.h"


typedef struct igrp2_path_ {
    struct atalk_path_	rtpath;
} igrp2_path_t;


/*
 * Constant definitions.
 */

/*
 * For right now, set the ddpType of IGRP2 packets to 88 to agree with the
 * IP implementation. This will also be the source/destination socket 
 * of the IGRP2 packets.
 */
#define ATIGRP2_DDPTYPE		88

/*
 * AppleTalk TLV values.
 */
typedef enum {
    ATIGRP2_TLV_REQUEST		= 0x0201,
    ATIGRP2_TLV_METRIC		= 0x0202,
    ATIGRP2_TLV_EXTERNAL	= 0x0203,
    ATIGRP2_TLV_CABLE_CONFIG	= 0x0204
} atalk_tlv_t;


/*
 * AppleTalk packets are woefully small, but they're all we have to
 * work with. Use DDP_MAX_DATABYTES, since we will be using DDP long
 * packets always.
 */
#define MAXATIGRP2BYTES (DDP_MAX_DATABYTES - IGRP2HEADERBYTES)

/*
 * IGRP2 packet structs.
 * Note that these don't include the DDP and (possibly) broadcast protocol
 * header...
 */
    
/*
 * Used for decoding multiple destinations in an atigrp2_metrictype
 */
#define ATIGRP2EXTPAKMACRO \
    ulong routerid;		/* Router ID of injecting router */        \
    ulong universe;		/* Originating autonomous system number */ \
    ulong tag;                  /* Arbitrary tag */                        \
    unsigned protocol:8;        /* External protocol ID */                 \
    unsigned flag:8;            /* Internal/external flag */               \
    unsigned ext_metric:16;     /* external protocol metric */

typedef struct atigrp2_mpdecode_ {
    ushort cable_lo;
    ushort cable_hi;
} atigrp2_mpdecode;

typedef struct atipgr2_requesttype_ {
    IGRP2TLMACRO
    ataddrtype nexthop;
    uchar cablerange[4];           /* actually two shorts */
} atigrp2_requesttype;

typedef struct atigrp2_metrictype_ {
    IGRP2TLMACRO
    ataddrtype nexthop;
    IGRP2METRICPAKMACRO
    atigrp2_mpdecode dest;
} atigrp2_metrictype;

typedef struct atigrp2_externtype_ {
    IGRP2TLMACRO
    ataddrtype nexthop;
    ATIGRP2EXTPAKMACRO
    IGRP2METRICPAKMACRO
    atigrp2_mpdecode dest;   /* At least one of these per tlv */
} atigrp2_externtype;

typedef struct ipigrp2_extdatatype_ {
    ATIGRP2EXTPAKMACRO
} atigrp2_extdatatype;

typedef struct atigrp2_cabletype_ {
    IGRP2TLMACRO
    ushort	cable_lo;		/* low end of cable range */
    ushort	cable_hi;		/* high end of cable range */
    ulong	routerid;		/* router ID */
} atigrp2_cabletype;

/*
 * Well, those IP guys have their PDB which has gobs and gobs of stuff
 * packed into it. And since we have a pointer for private storage off the
 * ddb, this is what will be hung off of same for AppleTalk.
 *
 * 'ipb' stands for "IGRP2 parameter block"
 */
typedef struct atalk_ipb_ {
    atalk_rdb_t	*rdb;			/* ptr to the route descriptor */

    char *name;				/* pointer to protocol name string */
    char *pname;			/* pointer to routing process name */
    watched_queue *pktQ;		/* Queue where packets will be put. */
    ulong whenchanged;			/* remember when last changed */
    boolean holdokay;			/* TRUE if holddowns permitted */
    int distance;			/* distance for this protocol */
    int def_distance;			/* default distance */
    /*
     * Timers.
     * For now, just jam the timers to be the IP defaults. Timer
     * adjustment should be left for wizards; hid the command
     * in the future.
     */
    pid_t   router_pid;		/* process ID of atigrp2_router */

    /*
     * Protocol Specific Fields
     */
    ushort asystem;		/* our AS number, if any */
    uchar edition;		/* IGRP edition number */
    ulong def_bandwidth;	/* IGRP readvertised bandwidth */
    ulong def_reliability;	/*      "	reliablity */
    ulong def_delay;		/* 	"	delay */
    ulong def_loading;		/*	"	load factor */
    ushort def_mtu;		/*	"	mtu */

    /*
     * igrp2 specific fields
     */
    void *ddb;                  /* Pointer to dual descriptor block */
    watched_queue *workQ;

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

    uchar	at_socket;
} atalk_ipb_t;

typedef struct atigrp2_worktype_ {
    struct atigrp2_worktype_ *next;
    igrp2_eventtype type;
    union {
	struct {
	    atalk_pdb_t p;	    
	} red;
	struct {
	    ushort start;
	    ushort end;
	} d;
	struct {
	    atalkidbtype *interface;
	    ushort start;
	    ushort end;
	    boolean sense;
	} ifc;
	struct {
	    atalkidbtype *interface;
	} ifd;
    } c;
} atigrp2_worktype;

    
/*
 * Externs for functions in at_igrp2.c
 */
extern void atigrp2_show_topology(parseinfo *);
extern void atigrp2_show_neighbors(parseinfo *);
extern void atigrp2_validate_route(ushort, ushort);
extern void atigrp2_config_hello(atalkidbtype *, ulong);
extern void atigrp2_config_holdtime(atalkidbtype *, ulong);
extern int  atigrp2_get_active_time(void);
extern void atigrp2_ddb_debug(parseinfo *);
extern void atigrp2_config_command(parseinfo *);
extern void atigrp2_show_event(parseinfo *);
extern void atigrp2_show_traffic(parseinfo *);
extern void atigrp2_show_globals_add(void);
extern void atigrp2_show_nbr_add(neighborentry *);
extern void atigrp2_show_tfc_add(void);
extern void atigrp2_parser_init(void);
#endif __AT_IGRP2_H__
