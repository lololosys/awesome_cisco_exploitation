/* $Id: nbfcp.h,v 3.2.62.1 1996/08/18 09:08:19 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/nbfcp.h,v $
 *------------------------------------------------------------------
 * nbfcp.h - NBF control protocol definitions for PPP
 *
 * August 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nbfcp.h,v $
 * Revision 3.2.62.1  1996/08/18  09:08:19  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.2  1995/11/17  18:03:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  23:06:41  syiu
 * ?
 * Initial placeholder for NBFCP/PPP
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define MAX_CI_NBFARRAY 30

struct nbfcp_options_ {
    int neg_name_projection: 1;  /* Negotiate NetBios Name projection? */
    int neg_peer_info: 1;        /* Negotiate Peer Information? */
    int neg_multicast_filter: 1; /* Negotiate Multicast Filtering? */
    int neg_ieee_mac_addr: 1;    /* Negotiate IEEE Mac Address header? */
    uchar remotepeername[MAX_CI_NBFARRAY]; /* Remote Peer name */
    ushort remotepeerclass;       /* Remote Peer Class */
    ulong remotepeerversion;    /* Remote Peer  Version */
    uchar peername[MAX_CI_NBFARRAY]; /* This Peer's Name */
    ushort mcast_fwd_period;     /* Multicast Forward Period */
    uchar mcast_priority;        /* Multicast Priority */
};

typedef struct nbfcp_options_ nbfcp_options;

#define NBF_PEER_CLASS 4
#define NBF_VERSION_MAJOR 0x03
#define NBF_VERSION_MINOR 0x00
#define NBF_MCAST_FWD_PERIOD_DEF 5
#define NBF_MCAST_FWD_PERIOD_NONE 0
#define NBF_MCAST_FWD_PERIOD_NEG 0xFF
#define NBF_MCAST_FWD_PERIOD_MAX 60
#define NBF_MCAST_PRIO_DIRECTED 0
#define NBF_MCAST_PRIO_MCAST 1 

/*
 * Options.
 */
#define CI_NBF_NAME_PROJECTION  1
#define CI_NBF_PEER_INFO        2
#define CI_NBF_MCAST_FILTER     3
#define CI_NBF_IEEE_MAC_HDR     4

/*
 * Name Projection Added Flag
 */
#define NBFCP_NAME_PROJ_UNIQUE_NAME 1
#define NBFCP_NAME_PROJ_GROUP_NAME 2
 
extern void nbfcp_init(fsm *);

#define LENCINAMEPROJ(neg) (neg ? (2+16+1) : 0)
#define LENCIPEERINFO(neg, len) (neg ? len+8 : 0)
#define LENCIMCASTFLTR(neg) (neg ? 5 : 0)
#define LENCIIEEEMAC(neg) (neg ? 2 :0)

/* not used currently */
#define ADDCINAMEPROJ(opt, neg, append, orig_ucp, insert_ucp, value1, value2) \;

/* not used currently */
#define ACKCINAMEPROJ(opt, neg, value1) \
;

/* not used currently */
#define NAKCINAMEPROJ(opt, neg, code) \;

/* not used currently */
#define REJCINAMEPROJ(opt, neg, value1) \
;

/* not used currently */
#define ADDCIPEERINFO(opt, neg, value1, value2, value3, value4, value4_len) \
;

/* not used currently */
#define ACKCIPEERINFO(opt, neg, value1) \
;

/* not used currently */
#define NAKCIPEERINFO(opt, neg, code) \
;

/* not used currently */
#define REJCIPEERINFO(opt, neg, value1) \
;

#define ADDCIMCASTFLTR(opt, neg, value1, value2) \
    if (neg) { \
       PUTCHAR(opt, ucp); \
       PUTCHAR(LENCIMCASTFLTR(TRUE), ucp); \
       PPP_PUTSHORT(value1, ucp); \
       PUTCHAR(value2, ucp); \
       if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: sending CONFREQ, type = %d (%s), value1 = %x, value2 =%x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIMCASTFLTR(opt, neg, value1, value2) \
    if (neg) { \
       if ((len -= LENCIMCASTFLTR(TRUE)) < 0) \
	    goto bad; \
       GETCHAR(citype, p); \
       GETCHAR(cilen, p); \
       if (cilen != LENCIMCASTFLTR(TRUE)) \
	    goto bad; \
       PPP_GETSHORT(cishort, p); \
       GETCHAR(cichar, p); \
       if ((cishort != (value1)) || (cichar != (value2))) \
          goto bad; \
       if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config ACK received, type = %d (%s), value1 = %x, value2 = %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIMCASTFLTR(opt, neg, code) \
    if (neg && \
	(len >= LENCIMCASTFLTR(TRUE)) && \
	(p[1] == LENCIMCASTFLTR(TRUE)) && \
	(p[0] == opt)) { \
	len -= LENCIMCASTFLTR(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
        GETCHAR(cichar, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config NAK received, type = %d (%s), value1 = %x value2 = %x", \
		 opt, #opt, cishort, cichar); \
	code \
    }


#define REJCIMCASTFLTR(opt, neg, value1, value2) \
    if (neg && \
	len >= LENCIMCASTFLTR(TRUE) && \
	p[1] == LENCIMCASTFLTR(TRUE) && \
	p[0] == opt) { \
	len -= LENCIMCASTFLTR(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
        GETCHAR(cichar, p); \
	/* Check rejected value. */ \
	if ((cishort != (value1)) || (cichar != (value2))) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config REJ received, type = %d (%s), value1 = %x value2=%x", \
		 opt, #opt, value1, value2 ); \
	neg = 0; \
    }


#define ADDCIIEEEMAC(opt, neg) \
     if (neg) { \
       PUTCHAR(opt, ucp); \
       PUTCHAR(LENCIIEEEMAC(TRUE), ucp); \
       if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: sending CONFREQ, type = %d (%s)", \
		 opt, #opt); \
     }


#define ACKCIIEEEMAC(opt, neg) \
    if (neg) { \
       if ((len -= LENCIIEEEMAC(TRUE)) < 0) \
	    goto bad; \
       GETCHAR(citype, p); \
       GETCHAR(cilen, p); \
       if (cilen != LENCIIEEEMAC(TRUE)) \
	    goto bad; \
       if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config ACK received, type = %d (%s)", \
		 opt, #opt); \
}

#define NAKCIIEEEMAC(opt, neg, code) \
    if (neg && \
	(len >= LENCIIEEEMAC(TRUE)) && \
	(p[1] == LENCIIEEEMAC(TRUE)) && \
	(p[0] == opt)) { \
	len -= LENCIIEEEMAC(TRUE); \
	if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config NAK received, type = %d (%s)", \
		 opt, #opt); \
	code \
    }

#define REJCIIEEEMAC(opt, neg) \
    if (neg && \
	(len >= LENCIIEEEMAC(TRUE)) && \
	(p[1] == LENCIIEEEMAC(TRUE)) && \
	(p[0] == opt)) { \
	len -= LENCIIEEEMAC(TRUE); \
	INCPTR(2, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nnbfcp: config REJ received, type = %d (%s)", \
		 opt, #opt); \
	neg = 0; \
    }
