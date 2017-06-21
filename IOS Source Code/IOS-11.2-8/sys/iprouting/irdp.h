/* $Id: irdp.h,v 3.2 1995/11/17 17:35:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/irdp.h,v $
 *------------------------------------------------------------------
 * irdp.h -- ICMP Router Discovery Protocol data structures
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: irdp.h,v $
 * Revision 3.2  1995/11/17  17:35:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __IRDP_H__
#define __IRDP_H__

/*
 * Default parameters
 */
#define IRDP_PREFERENCE	0		/* a number, any number, default=0 */
#define IRDP_MAXADVERT	600		/* send update every 5 minutes */
#define IRDP_XMAXADVERT	0		/* send update every 0 seconds */
#define IRDP_HOLDTIME	(IRDP_MAXADVERT*3) /* time to believe reports */
#define	IRDP_MAX_HOLDTIME 9000		/* as per rfc 1256, 9000 seconds */

#define	IRDP_INITIAL_ADVERT_INTERVAL	 16	/* 16 seconds */
#define	IRDP_INITIAL_ADVERTISEMENTS	 3	/* 3 advertisements */

#define IRDP_CODE_ADVERTISE	0	/* normal advertisement */
#define IRDP_CODE_MOBILEONLY	16	/* mobility, no routing */

/*
 * IRDP address information.  Follows ICMP header.
 */
#define IRDPDATABYTES(count) (8*(count))

typedef struct {
    ipaddrtype address;
    long preference;
} irdpinfo;

/*
 * A local data structure for storing proxy information
 */
typedef struct rproxy {
    struct rproxy *next;
    ipaddrtype address;
    long preference;
} rproxy;

/*
 * Extensions for Mobile IP.
 */

typedef struct mobexttype_ {
    uchar extension;			/* extension number */
    uchar length;			/* len of extension, in bytes */
    ushort seqnum;			/* sequence num of this advert */
    unsigned regreq : 1;		/* is registration required? */
    unsigned reserved : 7;		/* reserved unused bits */
} mobexttype;

#define IRDP_MOBILITY_SIZE 	5	/* don't trust sizeof() */
#define IRDP_MOBILITY_LENGTH 	3
#define IRDP_MOBILITY_EXTENSION	16

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * irdp.c
 */
extern void ip_irdp_periodic(hwidbtype *);
extern void iprouting_irdp_send_report(idbtype *, ipaddrtype);
extern void ip_irdp_goingdown(idbtype *);
extern void ip_irdp_comingup(idbtype *);
extern void ip_irdp_setup_if(idbtype *idb);

/*
 * irdp_chain.c
 */
extern void irdp_parser_init(void);

#endif __IRDP_H__
