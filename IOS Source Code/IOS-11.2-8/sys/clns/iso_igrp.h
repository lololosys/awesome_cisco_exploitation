/* $Id: iso_igrp.h,v 3.4 1996/01/25 22:18:59 rchandra Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/clns/iso_igrp.h,v $
 *------------------------------------------------------------------
 * iso_igrp.h -- IGRP protocol for CLNS 
 *
 * 4-Jan-1989, Paulina Knibbe
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iso_igrp.h,v $
 * Revision 3.4  1996/01/25  22:18:59  rchandra
 * CSCdi44688:  ISO-IGRP: 4k based images cannot interoperate with 68k
 * images
 * - do not super-impose structure on the byte stream while generating and
 *   parsing update
 *
 * Revision 3.3  1995/11/17  08:54:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:10:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:19:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ISO_IGRP_H__
#define __ISO_IGRP_H__

#define ISO_IGRP_VERSION 2

/*
 *
 * The timers need to play together rationally
 * In general, the hello_time and the hello_def_hold_time will determine
 *     how quickly we detect the failure of a neighbor router. We want
 *     this to be fairly quick. Right now this is set up so that we
 *     will detect a router failure in less than one minute.
 *
 * The cycletime and the routeflushtime can be longer. IP uses 90 seconds
 * but we can be more generous because we have the hello failures and
 * flash updates.
 *
 * Eventually, this should be configurable and perhaps the router-router hellos
 * could carry the info. Which takes precedence when there is a conflict
 * between neighbor routers is an open issue.
 *
 */

#define ISO_IGRP_CYCLETIME (45*ONESEC)  		/* broadcast interval*/
#define ISO_IGRP_ROUTEFLUSHTIME (3*ISO_IGRP_CYCLETIME)	/* mark invalid */
#define ISO_IGRP_BLINDTIME (3*ISO_IGRP_CYCLETIME + 10*ONESEC) /* hold down */
#define ISO_IGRP_HELLOTIME (17 * ONESEC)                 /* hello interval */

#define ISO_IGRP_HELLO_DEF_HOLD_TIME (3 * ISO_IGRP_HELLOTIME)

#define ISO_IGRP_OPCODE_STATION 1
#define ISO_IGRP_OPCODE_AREA 2
#define ISO_IGRP_OPCODE_HELLO 0x4
#define ISO_IGRP_OPCODE_QUERY 0x10

#define ISO_IGRP_LEVEL1_ONLY    1
#define ISO_IGRP_LEVEL2_ONLY    2
#define ISO_IGRP_LEVEL1_LEVEL2  3

/* 
 * values for the 'code' field of the router hello packet
 */
#define ISO_IGRP_AREALIST     1   /* list of areas this router is in */
#define ISO_IGRP_AREAROUTE    2   /* list of areas we can route to */
#define ISO_IGRP_STATIONROUTE 3   /* list of stations we can route to */
#define ISO_IGRP_PREFIXROUTE  4   /* list of prefix routes we know */
#define ISO_IGRP_PARAMS       5   /* routing parameters (timers and k?) */

/*
 * ISO-IGRP header fields
 */

/*
 * Note that the following packet type is used for both level 1 and level 2
 * routing update. The 'item' field is the only one that varies.
 */
typedef struct iso_igrp_hdrtype_ {
    uchar pid;               /* protocol identifier */
    uchar hlen;              /* header length */
    uchar version;           /* protocol version */
    uchar edition;           /* routing table edition */
    uchar opcode;
    uchar len[2];            /* total packet length */
    uchar checksum[2];       /* over whole packet */
    NSAP_T source_net;       /* NET of gateway that sent packet */    
    uchar item[0];           /* set of routing update items to exchange */
} iso_igrp_hdrtype;

#define iso_igrp_headstart(ptr) ((ptr)->network_start)

/*
 * Note that the following information may include anything which
 * routers want to tell each other
 * Currently hello packets include ISO_IGRP_AREALIST
 * Level 1 updates include ISO_IGRP_STATIONROUTE
 * Level 2 updates include ISO_IGRP_AREAROUTE and ISO_IGRP_PREFIXROUTE
 */
typedef struct iso_igrp_info_type_ {
    uchar code;                      /* which info is described here */
    uchar length;                    /* how long is this info */
    uchar value[1];                  /* array of value */
} iso_igrp_info_type;
#define ISO_IGRP_TLV_HDR_LENGTH 2    /* TLV overhead size */



/*
 * metric type is used for ISO_IGRP_STATIONROUTE and ISO_IGRP_AREAROUTE
 */
typedef struct iso_igrp_metrictype_ {
    uchar delay[3];		/* delay, in tens of microseconds */
    uchar bandwidth[3];		/* bandwidth, in units of 1 Kbit/sec */
    uchar mtu[2];		/* MTU, in octets */
    uchar reliability;		/* percent packets successfully tx/rx */
    uchar load;			/* percent of channel occupied */
    uchar hopcount;		/* hop count */
    uchar id[CLNS_MAX_NSAP_LENGTH];   /* we will adjust the length when */
 				/* allocating */
} iso_igrp_metrictype;

#define ISO_IGRP_METRIC_HOPCOUNT_LENGTH 11   

/*
 * prefixroute is used to advertise routing prefixes
 * The order of the field is non-obvious. All of the varying fields are
 * stored at the end. This is to make access a little easier.
 * Prefix is stored in metric.id
 */
typedef struct iso_igrp_prefixroute_ {
    struct validation_{
	ulong time;
	uchar hold_time[2];
    } validation;
    struct iso_igrp_metrictype_ metric;   /* prefix is stored in metric.id */
    NSAP_T source_NET;
} iso_igrp_prefixroute;

/*
 * Scaling/weighting parameters for scalar metric computation
 */
#define ISO_IGRP_K1_DEFAULT 1		/* unweighed inverse bandwidth */
#define ISO_IGRP_K2_DEFAULT 0		/* no loading term */
#define ISO_IGRP_K3_DEFAULT 1		/* unweighted delay */
#define ISO_IGRP_K4_DEFAULT 0		/* no reliability term */
#define ISO_IGRP_K5_DEFAULT 0		/* no reliability term */


/*
 *    ISO IGRP hello packets
 *    These packets are exchanged between Intermediate Systems (routers).
 *    The IS's use them to discover each other and learn what areas
 *    each neighbor is in.
 *    
 *    Differences from IS-IS :
 *    1) we do not have a separate packet for
 *    LAN level1 IS and LAN level2 IS (we don't have the concept of
 *    designated router for LANs (for IGRP)).
 *    2) we do checksum the packet
 *    3) In the first release, we will ignore the 'interface type' field.
 *    Later, it may prove useful to support 'level2 only' links.
 *    
 *    Note that the FORMAT of this packet enforces the restriction that
 *    a router may have only one station id. 
 */
typedef struct iso_igrp_hello_hdrtype_ {
    uchar pid;                       /* protocol identifier */
    uchar hlen;                      /* header length */
    uchar version;                   /* protocol version */
    uchar reserved;
    uchar opcode;                    /* bits 6,7,8 are reserved */
                                     /* NOTE that this may NOT conflict
				      * with the values in iso_igrp_hdrtype
				      */
    uchar len[2];                    /* total packet length */
    uchar checksum[2];               /* over whole packet */
    NSAP_T source_net;               /* NET for sending router */
    uchar intf_type;                 /* level 1, level2 or both */
    uchar holdingtime[2];            /* holding timer for info in packet */
    uchar item[0];                   /* array of info exchanged between
				      * routers
				      */
} iso_igrp_hello_hdrtype;

/*
 * Magic number, but sizeof() gives the wrong size...
 */
#define ISO_IGRP_HELLO_HDRSIZE 33
    

typedef struct iso_igrp_area_addr_type_ {
    uchar addr_length;               /* how long is this area address */
    uchar address[2];                /* actual area address (min 2) */
} iso_igrp_area_addr_type;


typedef struct iso_igrp_param_type_ {
    uchar cycletime;
    uchar routeflushtime;
    uchar blindtime;
    uchar hellotime;
    uchar k1;
    uchar k2;
    uchar k3;
    uchar k4;
    uchar k5;
} iso_igrp_param_type;
/*
		 Discussion of the various metric fields.

	Note that a value of zero for delay, bandwidth, reliability, load,
	MTU, and distance indicates that information is lacking.

    number - 3 octets of IP address.  For an interior entry, we combine the
	high byte of the source address of the datagram to form the subnet
	address in question.  For a system or exterior entry, these three
	bytes are assumed to be the high three bytes of the address.

    delay - we measure station delay in units of 10 microseconds.  By using
	24 bits, we achieve a range of 7 decimal orders of magnitude, from
	10 microseconds (the Ethernet/802.3 interframe dead time) to
	168 seconds (rather severe satellite delays!).  Each intermediate
	gateway adds its delay to this number, giving the total path delay.

	A delay of all ones indicates that the network is unreachable.

    bandwidth - this quantity is the inverse bandwidth in bits/sec scaled
	by a factor of 10E10.  The "units" are something like 10e-5 secs
	per 10e+5 bits.	Again, 24 bits gives us a range of 7 decimal orders
	of magnitude. The range is from a 1200 bps modem line to a 10 gigabit
	fiber optic.  The highest inverse bandwidth is the one recorded.

    MTU - the maximum transmission unit is measured in bytes.  The smallest
	MTU encountered in the routing path is the one recorded and passed
	along.  This measurement is useful in avoiding fragmentation showers.

    reliability - the percentage (a fraction of 255) of datagrams that were
	successfully transmitted and received on that path.  The minimum
	reliability factor is recorded and passed along.

    load - the percentage of channel bandwidth occupied.  One measure of
	congestion.  The maximum load factor is recorded and passed along.

		Some Sample Values

			Delay	        Bandwidth
	Satellite    200,000 (2 sec)	       20 (500 Mbit)
	Ethernet	 100 (1 ms)	    1,000
	1.544 Mbit	2000 (20 ms)	    6,476
	64 Kbit		2000		  156,250
	56 Kbit		2000		  178,571
	10 Kbit		2000		1,000,000
	1 Kbit		2000	       10,000,000

*/
#endif __ISO_IGRP_H__
