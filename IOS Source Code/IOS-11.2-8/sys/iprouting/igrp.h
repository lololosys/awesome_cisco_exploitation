/* $Id: igrp.h,v 3.3.12.2 1996/06/18 01:47:19 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp.h,v $
 *------------------------------------------------------------------
 * igrp.h -- Interior Gateway Routing Protocol
 *
 * 3-September-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igrp.h,v $
 * Revision 3.3.12.2  1996/06/18  01:47:19  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/04/02  05:37:00  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.3  1996/02/13  08:14:19  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  17:33:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IGRP_H__
#define __IGRP_H__

#include "igrp_common.h"

#define IGRP_VERSION 1

#define IGRP_CYCLETIME (90*ONESEC)		/* broadcast interval */
#define IGRP_ROUTEFLUSHTIME (3*IGRP_CYCLETIME)	/* mark invalid */
#define IGRP_BLINDTIME (3*IGRP_CYCLETIME + 10*ONESEC) /* hold down */
#define IGRP_SUBNETFLUSHTIME (7*IGRP_CYCLETIME) /* flush time */
#define IGRP_MAXMTU 65535  /* Max MTU. This is really (short) -1 */
#define	IGRP_JITTER	20			/* jitter updates by 20% */

typedef struct igrptype_ {
    unsigned version: 4;		/* protocol version number */
    unsigned opcode: 4;			/* opcode */
    uchar edition;			/* edition number */
    ushort asystem;			/* autonymous system number */	
    ushort ninterior;			/* number of subnets in local net */
    ushort nsystem;			/* number of networks in AS */
    ushort nexterior;			/* number of networks outside AS */
    ushort checksum;			/* checksum of IGRP header and data */
} igrptype;

#define IGRPHEADERBYTES 12

typedef struct metrictype_ {
    uchar number[3];		/* 3 significant octets of IP address */
    uchar delay[3];		/* delay, in tens of microseconds */
    uchar bandwidth[3];		/* bandwidth, in units of 1 Kbit/sec */
    uchar mtu[2];		/* MTU, in octets */
    uchar reliability;		/* percent packets successfully tx/rx */
    uchar load;			/* percent of channel occupied */
    uchar hopcount;		/* hop count */
} metrictype;
   
#define METRICSIZE 14

/* 
 * Note the following constant 'MAXIGRPPACKETSIZE' is added for
 * backwards compatiblity reasons. 'BIGDATA' has been changed from
 * 1500 to 1524 and this means that we may generate large IGRP packet
 * sizes. Then things won't inter-operate...
 */
#define MAXIGRPPACKETSIZE 1500
#define MAXIGRPBYTES (((MAXIGRPPACKETSIZE-(IPHEADERBYTES(NOPT)+IGRPHEADERBYTES))/METRICSIZE)*METRICSIZE)

/*
 * IGRP metric inaccessible - fill the 24 bit composite metric with ones
 */
#define IGRP_METRIC_INACCESSIBLE 0xffffff

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

/*
 * IGRP traffic statistics
 */
typedef struct igrp_traffic_t_ {
    ulong inputs;
    ulong outputs;
    ulong checksumerr;
} igrp_traffic_t;

#endif __IGRP_H__
