/* $Id: egp.h,v 3.2.60.1 1996/06/18 01:47:11 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/egp.h,v $
 *------------------------------------------------------------------
 * egp.h -- EGP definitions, RFC904
 *
 * 22-April-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: egp.h,v $
 * Revision 3.2.60.1  1996/06/18  01:47:11  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:32:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EGP_H__
#define __EGP_H__

/*
 * Random useful things
 */
#define MAXEGPDATASIZE MAXDATAGRAMSIZE
#define THIRDDATASIZE 2000
#define egptype struct egptype_
#define	EGP_VERSION 2
#define EGPSIZE	(sizeof(egptype) + IPHEADERBYTES(NOPT))
#define ip2egp(pak) (pak->network_start + IPHEADERBYTES(NOPT))
#define EGPROUTEDISMS 500

/*
 * Network entry purging times
 * The invalid and flush times are double what they should be because of a
 * old bug...  Maybe we shouldn't fix them...?
 */
#define	EGP_CYCLETIME		(3*ONEMIN)  	/* broadcast interval */
#define EGP_ROUTEFLUSHTIME	(2*3*EGP_CYCLETIME) /* mark invalid */
#define	EGP_BLINDTIME		0		/* hold down (none) */
#define EGP_NETFLUSHTIME	(2*10*ONEMIN)  	/* flush time */


/*
 * Define the EGP packet type
 */

egptype {
    uchar   version;
    uchar   type;
    uchar   code;
    uchar   status;
    ushort  checksum;
    ushort  asystem;
    ushort  sequence;
    union   ttype {
	/* 
	 *  Neighbor Acquisition Message.
	 */
	struct /*na*/ {
	    ushort   PACKED(helloint);
	    ushort   PACKED(pollint);
	} PACKED(na);

#define	EGP_TYPE_NA	3

#define	EGP_NA_REQUEST	0
#define	EGP_NA_CONFIRM	1
#define	EGP_NA_REFUSE	2
#define	EGP_NA_CEASE	3
#define	EGP_NA_CEASEACK	4

#define	EGP_NAS_UNSPEC	0	/* unspecified */
#define	EGP_NAS_ACTIVE	1	/* active mode */
#define	EGP_NAS_PASSIVE	2	/* passive mode */
#define	EGP_NAS_INSUFF	3	/* insufficient resources */
#define	EGP_NAS_ADMIN   4	/* administratively prohibited */
#define	EGP_NAS_HALTING 5	/* system going down */
#define	EGP_NAS_PARAMS  6	/* parameter problem */
#define	EGP_NAS_PROTO   7	/* protocol violation */

	/* 
	 *  Neighbor Reachability Messages.
	 */

#define	EGP_TYPE_NR	5

#define	EGP_NR_HELLO	0
#define	EGP_NR_IHU	1

	/*  For the status field  */

#define	EGP_NRS_UNKNOWN		0	/* indeterminate status */
#define	EGP_NRS_UP		1	/* up */
#define	EGP_NRS_DOWN		2	/* down */

	/* 
	 *  EGP POLL/REPLY Messages
	 */

	struct /*pl*/ {
	    uchar  PACKED(nintgw);
	    uchar  PACKED(nextgw);
	    ulong  PACKED(net);
	    uchar  PACKED(data[2]);
	} PACKED(pl);

#define	EGP_TYPE_UPDATE		1
#define	EGP_TYPE_POLL		2

#define EGP_POLL_CODE 		0		/* always zero */

		/* for the status field */

#define EGP_POLLS_INDET		0
#define EGP_POLLS_UP		1
#define EGP_POLLS_DOWN		2
#define EGP_POLLS_UNSOLICITED	128

	/* 
	 *  EGP Error
	 */

	struct /*err*/ {
	    ushort  PACKED(reason);
	    char    PACKED(copy[2]);
	} PACKED(err);

#define	EGP_TYPE_ERROR	8

#define	EGP_ERROR_UNSPEC	0
#define	EGP_ERROR_HEADER	1
#define	EGP_ERROR_DATA		2
#define	EGP_ERROR_NONR		3
#define	EGP_ERROR_EXCESSPOLL	4
#define	EGP_ERROR_NORESPONSE	5

    } PACKED(t);
};


/*
 * Define the neighbor data structure, EGP state flags, and timeouts
 */

/* #define neighbortype struct neighbortype_ */



/*
 * neighbortype flag bits
 */
#define EN_KEEP		0x1	/* permanent entry */
#define EN_DEAD		0x2	/* dead, flush when possible */
#define EN_PASSIVE	0x4	/* passive mode */
#define EN_PRIMARY	0x8	/* primary router */
#define EN_NOHELLO	0x10	/* suppress next HELLO */
#define EN_STARTED	0x20	/* EN_PRIMARY and running */
#define EN_TEMP		0x40	/* promiscuous entry */

/*
 * Non-permanent flags
 */
#define EN_FLAGMASK (EN_DEAD+EN_PASSIVE+EN_NOHELLO+EN_STARTED+EN_TEMP)

/*
 * EGP neighbor states
 */
#define IDLE_STATE		0
#define ACQUISITION_STATE	1
#define DOWN_STATE		2
#define UP_STATE		3
#define CEASE_STATE		4

/*
 * Time parameters in seconds (after RFC904)
 * Changes per BBN core maintainers as of 18-May-1987:
 *   Hello interval 30 -> 60, Poll interval 120 -> 180
 */
#define P1	60		/* minimum interval between Hello commands */
#define P2     180		/* minimum interval between Poll commands */
#define P3      30		/* interval between Request or Cease rexmit */
#define P4    3600		/* max inactivity for a Up or Down entry */
#define P5     120		/* max inact for a Acquiring or Ceased entry*/

/*
 * Thresholds
 */
#define UP_ACT_THRESHOLD	3	/* j */
#define UP_PASS_THRESHOLD	1	/* j */
#define DOWN_ACT_THRESHOLD	1	/* k */
#define DOWN_PASS_THRESHOLD	0	/* k */

/*
 * EGP traffic statistics
 */
typedef struct egp_traffic_t_ {
    ulong inputs;
    ulong outputs;
    ulong formaterr;
    ulong checksumerr;
    ulong noport;
} egp_traffic_t;

extern egp_traffic_t egp_traffic;
extern neighbortype *egp_neighbors[NETHASHLEN];

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * egp_chain.c
 */
extern void egp_parser_init(void);

/*
 * egp.c
 */
extern void egp_timers_command(parseinfo *);
extern void autonomous_command(parseinfo *);
extern void egp_show_neighbors(parseinfo *);

#endif __EGP_H__
