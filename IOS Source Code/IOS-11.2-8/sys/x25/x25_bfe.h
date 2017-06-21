/* $Id: x25_bfe.h,v 3.2.62.1 1996/03/18 22:50:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/x25/x25_bfe.h,v $
 *------------------------------------------------------------------
 * x25_bfe.h -- X25 Level 3 BFE emergency support
 *
 * February 1992, Babak Nader
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_bfe.h,v $
 * Revision 3.2.62.1  1996/03/18  22:50:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:19:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:55:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:07:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/04  22:02:28  gglick
 * CSCdi37951:  X.25/BFE X.121 address generation incorrect
 *
 * Revision 2.1  1995/06/07  23:22:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __X25_BFE_H__
#define __X25_BFE_H__

#include <ctype.h>
#include "../iprouting/route.h"

/*
 * This used to be in imp.h until we ditched our 1822 support
 */
#define	IMP_MTU	1006		/* largest outgoing IP through IMP */

/*
 *  Red Virtual Network (RVN) information
 */
STRUCTURE_REF(rvntype);
struct rvntype_ {
    uchar net;		/* assumes/requires a class A IP address */
    ushort port;	/* type of device */
    ushort domain;	/* domain ID */
    ushort host;	/* BFE ID */
};

/*
 * BFE Emergency hash struct
 */
struct bfetype_ {
    bfetype	*bfe_next;
    hwidbtype	*idb;
    ipaddrtype	host_ipaddr;
    ipaddrtype	rbfe_ipaddr;
    rvntype	rbfe_rvn;
};

/*
 *  X.121 address nominal lengths
 */
#define X25_DDN_X121_ADDR_LEN 12
#define X25_BFE_X121_ADDR_LEN 12

/*
 * BFE emergency mode types
 */
#define BFE_NEVER       0
#define BFE_ALWAYS      1
#define BFE_DECISION    2

/*
 * BFE emergency status
 */
#define BFE_EMERG_MODE  1
#define BFE_EMW_OPEN 	2


/*
 * BFE decision setting 
 */
#define BFE_DECISION_NO  0
#define BFE_DECISION_YES 1
#define BFE_DECISION_ASK 2

/* BFE facility length 
 *	Code (1 byte) +
 *	LEN field (1 byte) +
 *	ATIF (4 bytes) = 6 bytes total.
 */
#define BFE_FACIL_SIZE	6 

/* Length field value */
#define BFE_FACIL_LEN	4 


#define	X25_BFE_HASH_SIZE	64
#define	X25_BFE_HASH(h_address, len) \
    (xnethash((uchar *)h_address, len) & (X25_BFE_HASH_SIZE - 1))

/*
 *  The BFE certification suite occasionally requires that the 1980 X.25
 *  Recommendation be violated--this macro is used to mark these problem areas
 */
#define BFE_BROKEN(idb)		ISBFE(idb)

/*
 *  define values and functions to convert BFE IP and X.121 addresses
 */

/*
 *  decimal fields of a 12-digit X.121 address:
 *
 *  0000   0   P   DDD   HHH
 *  \__/       |   \_/   \_/
 *  DNIC     port domain host
 */
#define X25_BFE_DNIC		"0000"
#define X25_BFE_MAX_PORT	7	/* per page 2-5 */
#define X25_BFE_MAX_DOMAIN	999	/* per page 2-5 */
#define X25_BFE_MAX_HOST	999	/* per page 2-5 */
#define BFE_DNIC_DIGITS		4
#define BFE_MBZ_DIGITS		1
#define BFE_PORT_DIGITS		1
#define BFE_DOMAIN_DIGITS	3
#define BFE_HOST_DIGITS		3

/*
 *  binary fields of an IP address:
 *
 *  MSB					LSB
 *  nnnn nnnn 0ppp dddd dddd ddhh hhhh hhhh
 *   \______/  \_/ \__________/\__________/
 *    network  port   domain      host
 */
#define BFE_NET_BITS	 8
#define BFE_NET_M	0xff000000
#define BFE_MBZ_BITS	 1
#define BFE_MBZ_M	0x00800000
#define BFE_PORT_BITS	 3
#define BFE_PORT_M	0x00700000
#define BFE_DOMAIN_BITS	10
#define BFE_DOMAIN_M	0x000ffc00
#define BFE_HOST_BITS	10
#define BFE_HOST_M	0x000003ff

static inline void bfe_ip_form (rvntype *rvn, ipaddrtype *ip)
{
    *ip = ((rvn->net << (BFE_HOST_BITS + BFE_DOMAIN_BITS + BFE_PORT_BITS +
			 BFE_MBZ_BITS)) & BFE_NET_M) |
	  ((rvn->port << (BFE_HOST_BITS + BFE_DOMAIN_BITS)) & BFE_PORT_M) |
	  ((rvn->domain << BFE_HOST_BITS) & BFE_DOMAIN_M) |
	  (rvn->host & BFE_HOST_M);
}
static inline boolean bfe_ip_parse (ipaddrtype ip, rvntype *rvn)
{
    rvn->host = (ip & BFE_HOST_M);
    rvn->domain = (ip & BFE_DOMAIN_M) >> BFE_HOST_BITS;
    rvn->port = (ip & BFE_PORT_M) >> (BFE_HOST_BITS + BFE_DOMAIN_BITS);
    rvn->net = (ip & BFE_NET_M) >>
		(BFE_HOST_BITS + BFE_DOMAIN_BITS + BFE_PORT_BITS +
		 BFE_MBZ_BITS);

    /*
     *  validate IP address
     *  NB: the port range spans the binary encoding, so it needn't be checked
     */
    return(((ip & CLASS_A_BIT) == 0) &&
	   ((ip & BFE_MBZ_M) == 0) &&
	   (rvn->domain <= X25_BFE_MAX_DOMAIN) &&
	   (rvn->host <= X25_BFE_MAX_HOST));
}
static inline void bfe_x121_form (rvntype *rvn, addrtype *x121)
{
    x121->type = ADDR_X121;
    x121->length = X25_BFE_X121_ADDR_LEN;
    (void) sprintf(x121->x121_addr, "%4s0%1u%-03u%-03u",
		   X25_BFE_DNIC, rvn->port, rvn->domain, rvn->host);
}

static inline boolean bfe_x121_parse (
    addrtype *x121,
    rvntype *rvn,
    ipaddrtype net_ip)
{
    uint dnic;
    uint inx;

    /*
     *  preliminary validation of X.121 address
     */
    if ((x121->type != ADDR_X121) ||
	(x121->length != X25_BFE_X121_ADDR_LEN)) {
	return(FALSE);
    }

    /*
     *  initialize the values
     */
    dnic = 0;
    rvn->port = 0;
    rvn->domain = 0;
    rvn->host = 0;

    rvn->net = (net_ip & BFE_NET_M) >>
		(BFE_HOST_BITS + BFE_DOMAIN_BITS + BFE_PORT_BITS +
		 BFE_MBZ_BITS);		/* network number isn't explicit */
    for (inx = 0; inx < BFE_DNIC_DIGITS; inx++) {
	if (! isdigit(x121->x121_addr[inx])) {
	    return(FALSE);
	}
	dnic = (dnic * 10) + (x121->x121_addr[inx] - '0');
    }
    for ( ; inx < (BFE_DNIC_DIGITS + BFE_MBZ_DIGITS); inx++) {
	if (x121->x121_addr[inx] != '0') {
	    return(FALSE);
	}
    }
    for ( ; inx < (BFE_DNIC_DIGITS + BFE_MBZ_DIGITS + BFE_PORT_DIGITS); inx++) {
	if (! isdigit(x121->x121_addr[inx])) {
	    return(FALSE);
	}
	rvn->port = (rvn->port * 10) + (x121->x121_addr[inx] - '0');
    }
    for ( ; inx < (BFE_DNIC_DIGITS + BFE_MBZ_DIGITS +
		   BFE_PORT_DIGITS + BFE_DOMAIN_DIGITS); inx++) {
	if (! isdigit(x121->x121_addr[inx])) {
	    return(FALSE);
	}
	rvn->domain = (rvn->domain * 10) + (x121->x121_addr[inx] - '0');
    }
    for ( ; inx < (BFE_DNIC_DIGITS + BFE_MBZ_DIGITS +
		   BFE_PORT_DIGITS + BFE_DOMAIN_DIGITS +
		   BFE_HOST_DIGITS); inx++) {
	if (! isdigit(x121->x121_addr[inx])) {
	    return(FALSE);
	}
	rvn->host = (rvn->host * 10) + (x121->x121_addr[inx] - '0');
    }

    /*
     *  final validation
     *  NB: the domain and host ranges span the decimal encoding, so they
     *      needn't be checked
     */
    return((dnic == 0) &&
	   (rvn->port <= X25_BFE_MAX_PORT));
}

extern bfetype *x25_bfe_hash_table[X25_BFE_HASH_SIZE];


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * x25_bfe.c
 */
extern bfetype *x25_bfe_enter(hwidbtype *, ipaddrtype *, ipaddrtype *);
extern bfetype *x25_bfe_lookup(hwidbtype *, ipaddrtype *);
extern void show_x25bfe(void);
extern void x25_bfe_nvgen(char *nv_command, hwidbtype *hwidb);
extern void x25_bfe_node(boolean add, hwidbtype *, ipaddrtype *, ipaddrtype *);
extern void x25_bfe_delete(hwidbtype *, ipaddrtype *);
extern void x25_bfenode_delete(hwidbtype *, bfetype *);
extern void x25_bfe_hash_init(void);
extern void x25_bfe_diag(hwidbtype *, paktype *);
extern boolean x25_process_atif(hwidbtype *, lcitype *, uchar, uchar, boolean);

#endif __X25_BFE_H__
