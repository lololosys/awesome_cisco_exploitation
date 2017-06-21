/* $Id: clns.h,v 3.2.60.1 1996/05/04 20:03:49 asastry Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns.h,v $
 *------------------------------------------------------------------
 * clns.h --  for ISO Connectionless Network Protocol
 *  
 * 9-October-1987 by Kirk Lougheed
 * 17-August-1988 by Jim Forster
 * 27-January-1989 by Jim Forster
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns.h,v $
 * Revision 3.2.60.1  1996/05/04  20:03:49  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:51:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/25  15:11:57  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.2  1995/08/23  18:02:47  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:16:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLNS_H__
#define __CLNS_H__

#include "packet.h"

/*
 * CLNS Headers.
 *
 * CLNS headers consist of four parts: the fixed part, address part,
 * segmentation part, and the options part.  Only the fixed part has a fixed
 * encoding.
 *
 *       +--------------------+
 *       |  Fixed Part        |
 *       +--------------------+
 *       |  Address Part      |
 *       +--------------------+
 *       |  Segmentation Part |
 *       +--------------------+
 *       |  Options Part      |
 *       +--------------------+
 *       |  Data              |
 *       +--------------------+
 *
 * There is a slightly different format for ES-IS:
 *
 *       +--------------------+
 *       |  Fixed Part        |
 *       +--------------------+
 *       |  Address Part      |
 *       +--------------------+
 *       |  Options Part      |
 *       +--------------------+
 */

/*
 * Fixed Part of CLNS Header
 */

#define CLNSFIXEDHEADER \
     uchar pid;		/* Network Layer Protocol Identifier */\
     uchar hlen;	/* header length */\
     uchar version;	/* version/protocol ID extension */\
     uchar lifetime;	/* life time remaining, in 500 msec units */\
     uchar sp:1;		/* segmentation permitted if set */\
     uchar ms:1;		/* more segments */\
     uchar er:1;		/* error report */\
     uchar type:5;	/* type code */\
     uchar LHT[2];      /* Last field differs for CLNS and ESIS */\
     /*\
      *    uchar seglen[2];	     CLNS - length of this segment \
      *    uchar holding_time[2];    ES-IS holding time for info in packet \
      */\
     uchar checksum[2];	/* header checksum */

typedef struct clnshdrtype_ {
     CLNSFIXEDHEADER	/* Actual CLNS header */
} clnshdrtype;

#define clnsheadstart(ptr)  ((ptr)->network_start)
#define clnsdatastart(ptr)  (clnsheadstart(ptr) + \
			     ((clnshdrtype *)clnsheadstart(ptr))->hlen)

/*
 * macros to access Segment Length (CLNP) and Holding Time (ES-IS)
 */
#define clns_seglen       LHT
#define esis_holding_time LHT

/*
 * values for fixedpart.pid
 */
#define NLP_INACTIVE 0x00
#define NLP_ISO8473  0x81
#define NLP_ESIS     0x82
#define NLP_ISIS     0x83
#define NLP_ISO_IGRP 0x84
#define NLP_IP       0xcc
#define NLP_IPX      0x42		/* Invented! */

/*
 * values for fixedpart.hlen
 */
#define CLNS_MINHEADERBYTES 12    /* minimum CLNS header size in bytes */
#define CLNS_MAXHEADERBYTES 254   /* max CLNS/ESIS header size */
#define ESIS_MINHEADERBYTES 9     /* minimum ES-IS header size in bytes */
#define ESIS_MINHELLOHEADERBYTES 12 /* Minimum ES-IS header size for hellos */
#define ESIS_MINRDHEADERBYTES 12 /* Minimum ES-IS header size for RDs */

/*
 * values for fixedpart.version
 */
#define CLNS_VERSION 1 
#define ESIS_VERSION 1

/*
 * values for fixedpart.type
 */
#define CLNS_TYPE_ERROR	0x01
#define CLNS_TYPE_ERQ	0x1E	/* Echo Request */
#define CLNS_TYPE_ERP	0x1F	/* Echo Reply */
#define CLNS_TYPE_DATA	0x1C
#define ESIS_TYPE_ESH	0x02
#define ESIS_TYPE_ISH	0x04
#define ESIS_TYPE_RD	0x06
#define ESIS_TYPE_RA	0x01
#define ESIS_TYPE_AA	0x03

/*
 * ES-IS option code values.
 */
#define ESIS_SECURITY_CODE  0xc5
#define ESIS_QOS_CODE       0xc3
#define ESIS_PRIORITY_CODE  0xcd
#define ESIS_ADDR_MASK_CODE 0xe1
#define ESIS_SNPA_MASK_CODE 0xe2
#define ESIS_ESCT_CODE      0xc6
#define ESIS_SYSID_CODE	    0xe3

/*
 * Default time for address assignments
 */
#define ESIS_ADDR_ASSIGN_TIME 0xffff /* 18 hours, more or less */

/*
 * size of fixed portion of clns header (sizeof(fixedpart) is wrong)
 */
#define CLNS_FIXEDHDR_SIZE 9

/* pointer to address part */
#define PAK2ADDR(pak)  (clnsheadstart(pak) + CLNS_FIXEDHDR_SIZE)

/* pointer to the beginning of the CLNS header */
#define CLNSHDRSTART(pak) &pak->pid


/*
 * Segmentation part of header
 *	Note that segmentation part may fall on odd address, so all
 *	elements are arrays of bytes.  In fact, the segmentation part
 *      is not present if the don't fragment bit is set, indicating the
 *      non-segmenting subset is being used.
 */

typedef struct {
    uchar duid[2];	/* data unit identifier */
    uchar segoffset[2];	/* segment offset */
    uchar length[2];	/* total PDU length */
} clns_segpart_t;


/*
 * Options are encoded as type/length/value tuples.  The value is always
 * at least one octet in length.
 */

typedef struct {
    uchar type;			/* Option type */
    uchar length;		/* Option length */
    uchar value[1];		/* Option value (variable) */
} clns_option;

#define CLNS_MIN_OPTION_SIZE 3	/* Sizeof never works right! */

/*
 * Options
 */
#define CLNS_OPT_DISCARD_REASON	0xC1
#define CLNS_OPT_PADDING	0xCC
#define CLNS_OPT_PRIORITY	0xCD
#define CLNS_OPT_QOS		0xC3	
#define CLNS_QOS_GLOBAL	        0xC0	/* high bits of first param */
#define CLNS_QOS_CONGESTION	0x08    /* in first param */
#define CLNS_OPT_RROUTE		0xCB
#define CLNS_OPT_CRROUTE	0x01	/* first param */
#define CLNS_OPT_SECURITY	0xC5
#define CLNS_OPT_SROUTE		0xC8
#define CLNS_OPT_CSROUTE	0x01	/* first param */
#define CLNS_OPT_ADDRMASK	0xE1
#define CLNS_OPT_SNPAMASK	0xE2
#define CLNS_OPT_ESCT		0xC6

#define CLNS_OPT_DISCARD_REASON_LEN 2
#define CLNS_OPT_PRIORITY_LEN	    1
#define CLNS_OPT_GQOS_LEN	    1	/* Global QOS Length */
#define CLNS_OPT_GQOS_CONG       0x08   /* Congestion experienced bit */
#define CLNS_OPT_GQOS_RSVD       0x20   /* Reserved bit */
#define CLNS_OPT_ESCT_LEN	    2

#define CLNS_QOS_OPT_MASK (0xffffff00 ^ \
		      ((CLNS_OPT_GQOS_CONG | CLNS_OPT_GQOS_RSVD) << 8))
#define CLNS_QOS_OPT_VALUE ((CLNS_OPT_QOS<<24) | (1<<16) | (CLNS_QOS_GLOBAL<<8))

/*
 * Flags that denote options that may be in the CLNS packet.
 * stored in pak->clns_flags
 * clns_opt_inproc is responsible for setting them.
 */
#define CLNS_OPT_HASSEC   0x001	  /* security */
#define CLNS_OPT_HASSR    0x002	  /* Source Route */
#define CLNS_OPT_HASCSR	  0x004	  /* Source Route is complete */
#define CLNS_OPT_HASRRT	  0x008	  /* Record Route */
#define CLNS_OPT_HASCRRT  0x010	  /* Record Route is complete */
#define CLNS_OPT_HASPAD	  0x020	  /* padding */
#define CLNS_OPT_HASQOS	  0x040	  /* QOS */
#define CLNS_OPT_HASGQOS  0x080	  /* QOS is globally unique */
#define CLNS_OPT_HASPRIOR 0x100	  /* Priority */
#define CLNS_OPT_HASRFD	  0x200	  /* has reason for discard */
#define CLNS_CONVERTED 	  0x400	  /* This packet has been converted */
#define CLNS_OPT_SRNOTEND 0x10000 /* route pntr not at end of list */


/*
 * NSAP addressing
 */
#define MAX_AREA_ADDRESSES   3

#define CLNS_STATION_TYPE    1
#define CLNS_AREA_TYPE       2
#define MIN_DOMAIN_LENGTH    1
#define MAX_DOMAIN_LENGTH    11
#define STATION_LENGTH	     6
#define AREA_LENGTH          2
#define AFI_LENGTH           1
#define SELECTOR_LENGTH      1
#define SYSTEMID_LENGTH      7
#define SELECTOR_ID_OFFSET   6
#define STATION_STRING_LENGTH 14	/* xxxx.xxxx.xxxx */

#define CLNS_MIN_NSAP_LENGTH 		3
#define CLNS_MIN_REASONABLE_NSAP_LENGTH 8

/*
 * Beware--the following symbol is slightly bogus--some code uses it as if
 * it were the max length of the address itself, rather than the max length
 * of the address + length byte!  Please use CLNS_MAX_ADDR_LENGTH defined
 * below (which is *only* the length of the address itself) in the future.
 */
#define CLNS_MAX_NSAP_LENGTH 21		/* must be consistent with 
					   address.h : ADDRLEN_CLNS */

/*
 * Old NSAP data structure--archaic, don't use in new code!  It is structurally
 * equivalent to an nsap_addr below (please use the latter).
 */
typedef uchar NSAP_T[CLNS_MAX_NSAP_LENGTH];	/* first byte contains 
						   actual length */

/*
 * NSAP data structures
 *
 * There are three different data structures used to carry NSAP information:
 *   - NSAPs
 *   - NSAP prefixes
 *   - Masked NSAPs
 *
 * An NSAP is used to carry a full NSAP address.  This is what is carried in
 * OSI protocols, and what should hold full NSAP addresses, area addresses,
 * etc., where byte-aligned addresses or address chunks are used.
 *
 * An NSAP prefix looks a lot like an NSAP, but it has granularity in *bits*.
 * This should be used wherever prefixes are reasonable, particularly in the
 * prefix routing table.
 *
 * A masked NSAP is essentially an NSAP address with a mask, used primarily
 * in access lists.  It consists of two components, a lefthand side and a
 * righthand side.  Either side may be of zero length.  This allows addresses
 * to be matched on the left, the right, or both.  The righthand side starts
 * immediately after the lefthand side ("leftlength" octets into the string).
 * The "exact" flag is set if the lefthand side is to be matched exactly
 * rather than used as a prefix (this is only meaningful if the righthand
 * side is of zero length).  The left length is counted in bits, for 
 * generality--we could use access lists to filter IDRP prefixes, for instance.
 * The right hand side length is in bytes, but the left hand side
 * length can be on any arbitrary bit boundary (so we can do exact length
 * matches on prefixes).  The right hand side starts at the byte boundary
 * following the end of the left hand side (the data structure is one
 * byte longer to accommodate the wasted bits).  The "applymask" flag is
 * set if the mask must be applied (straight octet compares won't work).
 * This is only a hint--the proper mask is always present.
 */

#define MAX_CLNS_ADDR_LEN 20	/* Max length of the NSAP address */

typedef struct _nsap_addr {
    uchar length;		/* Length of NSAP in *octets* */
    uchar addr[MAX_CLNS_ADDR_LEN];	/* The address itself */
} nsap_addr;

typedef struct _nsap_pfx {
    uchar length;		/* Length of NSAP prefix in *bits* */
    uchar prefix[MAX_CLNS_ADDR_LEN]; /* The prefix itself */
} nsap_pfx;

typedef struct _masked_nsap {
    uchar leftlength;		/* Length of lefthand side in *bits* */
    uchar rightlength;		/* Length of righthand side in *bytes* */
    uchar applymask;		/* True if mask must be applied */
    uchar exact;		/* True if exact match is required */
    uchar addr[MAX_CLNS_ADDR_LEN+1]; /* The address */
    uchar mask[MAX_CLNS_ADDR_LEN+1]; /* The mask */
} masked_nsap;


/*
 * Values returned by get_nsap_structure
 */
#define NSAP_STRUCT_BAD 0	/* Didn't parse */
#define NSAP_STRUCT_ADDR 1	/* It's an NSAP address */
#define NSAP_STRUCT_PFX 2	/* It's an NSAP prefix */
#define NSAP_STRUCT_MASKED 3	/* It's a masked NSAP */
#define NSAP_STRUCT_NOMEM 4	/* We couldn't get memory */


/*
 * Macro for converting NSAP_Ts to nsap_addr pointers, and vice versa
 */

#define NSAP_T_to_nsap_addr(x) ((nsap_addr *)(x))
#define nsap_addr_to_NSAP_T(x) (*(NSAP_T *)(x))


/*
 * values for match type in nsap_lookup routine
 */
#define nsap_match_t uchar

#define MATCH_EXACT	0
#define MATCH_CLOSEST	1

/*
 * hashed access into CLNS cache
 */
#define CLNS_CACHE_SIZE 256

/*
 * Due to the fact that we always have the Control Byte
 * from the ISO frame stuffed in between the HDLC encapsulation
 * and the start of the Fixed CLNS header. We consider the
 * Control Byte to be part of the encapsulation in the
 * fastswitching code
 */
#define CLNS_HDLC_ENCAPBYTES (HDLC_ENCAPBYTES + 1)


/*
 * Echo Packet Format
 * echo packets (CLNS_TYPE_ECHO) are sent with this structure in the
 * data part
 */

typedef struct {
    ushort echo_id;		/* differentiate multiple pinger's  */
    ushort trace_id;            /* only used by echos generated by trace */
} clns_echo_pak_t;

#define CLNS_ECHO_REQUEST	0x22
#define CLNS_ECHO_REPLY		0x55

/*
 * structure to keep track of echo packets
 */
typedef struct tracevariant_ {
    ulong    ttl;
    NSAP_T   currentGateway;
    NSAP_T   who;
} tracevariant;

typedef struct echovariant_ {
    ushort  dummy;
} echovariant;

typedef struct clns_echo_cb_t_ {
    struct clns_echo_cb_t_ *next;
    tt_soc *tty;
    boolean active;		/* haven't seen reply yet if true */
    boolean validate;		/* True if we should validate reply */
    boolean rotating_pattern;	/* True if we should use rotating pattern */
    ushort echo_id;		/* differentiate multiple echoers */
    ushort pattern;		/* Data pattern */
    ulong corrupted_words;	/* Count of corrupted words */
    ulong corruption_offset;	/* Offset of first corrupted word */
    ushort corruption;		/* Value of first corrupted word */
    sys_timestamp timeout;
    sys_timestamp xmittime;	/* when it was sent */
    ulong deltatime;		/* how long it took to come back */
    uchar how;			/* what to print -- '!', '.', etc. */
    boolean data_packet;	/* if true send data packet else echo */
    int flags;			/* option flags */
    boolean echo_flavor;                /* echo = TRUE, trace = FALSE */
    union {
	echovariant    echo;
	tracevariant   trace;
    } v;
} clns_echo_cb_t;


/*
 * default (and maximum) parameter values
 */
#define CLNS_JITTER_PCT 25		/* Subtract 25% random jitter */

#define CLNS_DEF_CONFIG_TIMER (1*ONEMIN)	/* 1 minute */
#define CLNS_MAX_CONFIG_TIMER (unsigned)0xffff  /* 16 bit value */

#define CLNS_DEF_HOLDING_TIMER (5*ONEMIN)	/* 5 minutes */
#define CLNS_MAX_HOLDING_TIMER (unsigned)0xffff /* 16 bit value */

#define ESIS_PERIODIC_TIMER (10*ONESEC)		/* 10 seconds */
#define CLNS_ROUTE_CACHE_HOLDING_TIMER (5*ONEMIN)  /* 5 minutes */

#define CLNS_DEF_ERPDU_INTERVAL 10		/* 10 msec */
#define CLNS_DEF_RDPDU_INTERVAL 100		/* 100 msec */

#define CLNS_DEF_LIFETIME 64
#define CLNS_MAX_LIFETIME 255

#define CLNS_MINMTU 512		/* min mtu, as per 8473 Sec. 8.3 */
#define CLNS_DEF_CONGESTION 4			/* conservative value */

#define CLNS_NETHASHLEN NETHASHLEN

#define CLNS_MINECHOSIZE 63		/* Min size with big addrs, qos, pad */
#define CLNS_MAXECHOSIZE 16384

#define CLNS_CACHE_SEARCH TRUE

#define CLNS_MAX_TAG_LENGTH 50



/* #define DEBUG */

#define esis_pbuginf if (esis_packet_debug) buginf
#define clns_pbuginf if (clns_packet_debug) buginf
#define clns_ebuginf if (clns_event_debug || clns_packet_debug) buginf
#define clns_rbuginf if (clns_routing_debug) buginf
#define clns_ibuginf if (clns_igrp_debug) buginf
#define clns_fbuginf if (clns_filter_debug) buginf
#define adj_ebuginf if (clns_event_debug) buginf
#define adj_pbuginf(pdb, str, parms...) \
 if (isis_nlsp_adj_packet_debug[(pdb)->proc_type])\
    buginf("\n%s-Adj: "str, (pdb)->errmsg_name , ## parms)
#define update_pbuginf(pdb, str, parms...) \
 if (isis_nlsp_update_packet_debug[(pdb)->proc_type])\
    buginf("\n%s-Update: "str, (pdb)->errmsg_name , ## parms)
#define spf_ebuginf(pdb, is_verbose, str, parms...) \
 if (isis_nlsp_spf_event_debug[(pdb)->proc_type] && \
     !((is_verbose) && isis_nlsp_terse_spf_event_debug[(pdb)->proc_type])) \
    buginf("\n%s-SPF: "str, (pdb)->errmsg_name , ## parms)
#define spf_tbuginf(pdb, str, parms...) \
 if (isis_nlsp_spf_trigger_debug[(pdb)->proc_type])\
    buginf("\n%s-SPF-TRIG: "str, (pdb)->errmsg_name , ## parms)
#define snp_buginf(pdb, str, parms...) \
 if (isis_nlsp_snp_debug[(pdb)->proc_type])\
    buginf("\n%s-SNP: "str, (pdb)->errmsg_name , ## parms)
#define locupdate_buginf(pdb, str, parms...) \
 if (isis_nlsp_local_update_debug[(pdb)->proc_type])\
    buginf("\n%s-LocUpdate: "str, (pdb)->errmsg_name , ## parms)

/*
 * CLNS traffic statistics
 */
enum {CLNS_INPUT, CLNS_OUTPUT, CLNS_ERSRCVD, CLNS_ERSSENT, CLNS_LOCAL, 
	  CLNS_NOT_AN_IS, CLNS_FORWARD, CLNS_HDRSYNTAX, CLNS_CHECKSUM, 
	  CLNS_LIFETIME, CLNS_OUTPUTCNGSTN_DROPPED, 
	  CLNS_OUTPUTCNGSTN_EXPRNCED, CLNS_NLPUNKNOWN, CLNS_BCASTIN, 
	  CLNS_BCASTOUT, CLNS_NOROUTE, CLNS_UNREACHDST, CLNS_ROUTEDISCARD, 
	  CLNS_SEGMENTED, CLNS_SEGFAILED, CLNS_ENCAPSFAILED, 
	  CLNS_OPTIONS_PACKETS, CLNS_OPTIONS_GQOS, CLNS_OPTIONS_TOTAL, 
	  CLNS_BADOPTION, CLNS_ECHOREPIN, CLNS_ECHOREPOUT, CLNS_ECHOREQIN, 
	  CLNS_ECHOREQOUT,

	  ESIS_ESHSRCVD, ESIS_ISHSRCVD, ESIS_ADDRCONFLICTSRCVD, 
	  ESIS_BADOPTION, ESIS_ESHSSENT, ESIS_ISHSSENT, ESIS_RDUSRCVD, 
	  ESIS_RDUSSENT, ESIS_QCFRCVD, ESIS_QCFSENT, ESIS_RASRCVD, 
	  ESIS_AASSENT,

	  ISO_IGRP_IN, ISO_IGRP_OUT, ISO_IGRP_QOUT, ISO_IGRP_QIN, 
	  ISO_IGRP_H_IN, ISO_IGRP_H_OUT, ISO_IGRP_SYNTAX_ERROR,

	  CLNS_MAXTRAFFIC};

/*
 * Reason for Discard parameters
 */

/*
 * general errors		0x0x	 
 */
#define CLNS_DISC_UNSPEC	0x00	/* unspecified reason */
#define CLNS_DISC_PROTOCOL	0x01	/* protocol procedure error */
#define CLNS_DISC_CHECKSUM	0x02	/* bad checksum */
#define CLNS_DISC_CONGEST	0x03	/* dropped because of congestion */
#define CLNS_DISC_HDRSYNTAX	0x04	/* header syntax error */
#define CLNS_DISC_SEGMENT	0x05	/* segmentation was forbidden */
#define CLNS_DISC_INCOMPL	0x06	/* incomplete PDU received */
#define CLNS_DISC_DUPLICATE	0x07	/* duplicate option */

/*
 * address errors		0x8x
 */
#define CLNS_DISC_UNREACHDST	0x80	/* destination unreachable */
#define CLNS_DISC_UNKNOWNDST	0x81	/* destination unknown */

/*
 * source routing errors	0x9x
 */
#define CLNS_DISC_SROUTEUNSPEC	0x90	/* unspecified */
#define CLNS_DISC_SRSYNTAX	0x91	/* syntax error */
#define CLNS_DISC_UNKNOWNADDR	0x92	/* unknown address */
#define CLNS_DISC_BADPATH	0x93	/* unacceptable path */

/*
 * lifetime exceeded errors	0xAx
 */
#define CLNS_DISC_HOPS		0xA0	/* in transit */
#define CLNS_DISC_REASSEM	0xA1	/* while reassembling */

/*
 * discard errors		0xBx
 */
#define CLNS_DISC_OPTION	0xB0	/* unsupported option */
#define CLNS_DISC_VERSION	0xB1	/* unsupported protocol version */
#define CLNS_DISC_SECURITY	0xB2	/* unsupported security option */
#define CLNS_DISC_SROUTE	0xB3	/* unsupported source route option */
#define CLNS_DISC_RROUTE	0xB4	/* unsupported record route option */

/*
 * reassembly errors		0xCx
 */
#define CLNS_DISC_INTERF	0xC0	/* reassembly interference */


/*
 * This macro builds an NSAP address from an adjacency database entry.
 * It always uses the first area address to build the high-order part
 * of the NSAP address. An N-selector of 0x00 is always appended.
 */
#define BUILD_NSAP(Adj, Nsap)                           \
    build_nsap (Adj->area_address [0][0],               \
	&Adj->area_address [0][1], NULL,       \
	&Adj->key [1], "\0", &Nsap);

/*
 * Check the length of an NSAP address. Must be between
 * 3 and 20 octets, inclusive.
 */
#define NSAP_LENGTH_CHECK(Nsap)				\
    (Nsap [0] >= CLNS_MIN_NSAP_LENGTH && Nsap [0] < CLNS_MAX_NSAP_LENGTH)

/* The CLNS header as seen in the fast switching routines */

typedef struct clns_fast_header_ {
    uchar fudge[3];		/* bytes of encaps garbage */
    CLNSFIXEDHEADER
    NSAP_T dest;		/* destination NSAP */
} clns_fast_header;

/*
 * Check if the AFI is in the range 0 - 99.
 */
#define AFI_OK(Nsap) \
    (Nsap[0] && ((uchar) Nsap[1]) <= 99)

/*
 * Do a modulo 255 operation on a longword.  A "% 255" generates a
 * divide instruction!
 */
#ifdef mc68000
#define MOD255(x, z) {\
    ulong w;\
    ulong s;\
    w = x;\
    asm volatile ( "\
        swap %1\n\
        addw %1,%0\n\
        addxw %4,%0" \
                  : "=d" ((x)), "=d" ((w))\
                  : "0" ((x)), "1" ((w)), "d" ((z)));\
    s = x >> 8;\
    asm volatile ( "\
        addb %1,%0\n\
        addxb %4,%0" \
		  : "=d" ((x)), "=r" ((s)) \
		  : "0" ((x)), "1" ((s)), "d" ((z)));\
    x &= 0xff;\
    if (x == 0xff)\
        x = 0;\
}
#else
#define MOD255(x, z) {\
    x %= 255;\
    if (x == 0xff) x = 0;\
}
#endif

/*
 * Add a constant to a byte, mod 255.  (i.e. add the constant, and add any
 * generated "carry" back into the result.)  Note that the result can never
 * be zero, since the carry bit will be set (and the end-around add will 
 * bump it to 1).  In the "C" version, the carry is manually detected and
 * added.
 */
#ifdef mc68000
#define ADD_MOD255(x,k,z) {\
    asm volatile ( "\
        addqb %2,%0\n\
        addxb %3,%0" \
		  : "=d" ((x)) \
		  : "0" ((x)), "I" ((k)), "d" ((z)));\
    }
#else
/*
 * The compiler does a nice job of optimizing this in C...
 */
#define ADD_MOD255(x,k,z) {\
        if (x + k > 255) x = x + k + 1;\
        else x = x + k;\
    }
#endif

/*
 * Subtract a constant from a byte, mod 255, and set the result to 255 if it
 * was 0. (i.e. subtract the constant from the byte, and subtract any 
 * generated "borrow" from the result.)  In the "C" version, the borrow
 * and negative numbers are handled manually.
 */
#ifdef mc68000
#define SUB_MOD255(x,k,z) {\
    asm volatile ( "\
        subqb %2,%0\n\
        subxb %3,%0" \
		  : "=d" ((x)) \
		  : "0" ((x)), "I" ((k)), "d" ((z)));\
    if (x == 0)\
        x = 0xff;\
    }
#else
/*
 * The compiler does a nice job of optimizing this in C...
 */
#define SUB_MOD255(x,k,z) {\
        if (x <= k) x = 255 - k + x;\
        else x = x - k;\
   }
#endif

/* Timer types for ES-IS */

enum {
    ESIS_AGER_TIMER,			/* Ager, context = NULL */
    ESIS_HELLO_TIMER			/* Hello timer, context = IDB */
};

/*
 * A type for a station ID lookup routine.  NLSP and ISIS have different
 * versions.
 */
typedef char * (*clns_station_lookup_type) (clns_pdbtype *, uchar *);

/* Default values for CLNS cache invalidation */

#define CLNS_CACHE_MIN_INVAL_DEFAULT (2*ONESEC)	/* 2 seconds minimum */
#define CLNS_CACHE_MAX_INVAL_DEFAULT (5*ONESEC)	/* 5 seconds maximum */
#define CLNS_CACHE_QUIET_IVL_DEFAULT 3		/* 3 seconds of quiet time */
#define CLNS_CACHE_QUIET_THRESH_DEFAULT 0       /* 0 requests in that time */

#endif __CLNS_H__
