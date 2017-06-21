/* $Id: tarp.h,v 3.3.62.2 1996/05/17 21:03:27 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp.h,v $
 * Revision 3.3.62.2  1996/05/17  21:03:27  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/05/17  20:55:38  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  18:44:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:43:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:33:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"

#ifndef __TARP_H__
#define __TARP_H__

#define TARP_PABUGINF if (tarp_pkt_debug) buginf
#define TARP_EVBUGINF if (tarp_event_debug) buginf

/*
 * Structure of a TARP PDU. TARP PDU's are carried as data PDU's, per
 * ISO 8473. 
 */

typedef struct tarp_pdutype_ {
    ushort lifetime;          /* TARP TTL in hops */
    ushort sequence;          /* TARP sequence number (used to detect loops) */
    uchar prot_addr_type;     /* Type of protocol that TID maps to (CLNP) */
    /* 
       Cope with compiler's quirks (while extracting bit fields.) In
       order to access these bit fields, it appears that the MIPS
       compiler extracts a short word and then masks off the relevant
       bits. Therefore, since these fields happen to fall on an  odd-byte   
       boundary, we'll see alignment errors. ;-((
     */
    union f1 {
	struct f2 {
	    uchar urc_bit:1;  /* "URC" bit. Bit is 0 means update remote cache,
				 and look at the remaining bits for type code.
				 Bit is 1 means don't update remote cache */
            uchar code_bits:7; /* type code */
	} F2;
	uchar info;
    } F1;
    uchar target_len;         /* length of TID of target (octets) */
    uchar source_len;         /* length of TID of source (octets) */
    uchar prot_addr_len;      /* length of addr in 'source_prot_addr' field */
    uchar data[1];            /* data - see format after end of struct */
} tarp_pdutype;    

#define TARP_URC_MASK 0x80    /* URC bit - see 'tarp_pdutype' above */
#define TARP_TYPE_CODE_MASK 0x7F /* type code - see 'tarp_pdutype' above */

#define TARP_GET_URC(ptr) (((ptr)->F1.info) & TARP_URC_MASK)
#define TARP_GET_CODE(ptr) (((ptr)->F1.info) & TARP_TYPE_CODE_MASK)

#define tarp_urc F1.F2.urc_bit
#define tarp_code F1.F2.code_bits

/*
  The 'data' part of the TARP PDU consists of 
     uchar target_tid[n]        (TID of target)
     uchar source_tid[n]        (TID of source)
     uchar source_prot_addr[n]  (protocol addr of source - currently an NSAP)
*/

#define TARP_PDU_HEADER_SIZE 9 /* Header size of TARP PDU */

#define TARP_MAX_TID_LEN 255   /* Max length of a host's TID */

/*
 * Macros to point to various spots in the 'data' portion of TARP PDU
 */

#define TARGET_TID(ptr) ((ptr)->data)
#define SOURCE_TID(ptr) ((ptr)->data + (ptr)->target_len)
#define SOURCE_PROT_ADDR(ptr) ((ptr)->data + (ptr)->target_len + \
			                     (ptr)->source_len)

/*
 * Macro to determine if a protocol is supported or not. Currently, this is
 * only CLNP.
 */

#define TARP_SUPPORTED_PROTOCOL(prot) (prot == TARP_NSAP_PROT_ADDR_TYPE)

/*
 * TARP type codes (for TARP PDU's). These are the codes that can appear in
 * the 7 bits following the "URC" bit in the "urc_type" field in a TARP PDU.
 *
 * TARP PDU Type                   Description
 * -------------                   -----------
 *
 *    1                Request protocol address that matches TID of target,
 *                     searching only intra-area.
 *                  
 *    2                Same as 1, but also search other areas (i.e. L2)
 *    3                Response to a TARP request
 *    4                Notification of TID/Address change
 *    5                Request TID address that matches protocol address
 *                     (currently this will only be a CLNS NET)
 */

#define TARP_RQST_TID_L1_PDU  1
#define TARP_RQST_TID_L2_PDU  2
#define TARP_RESPONSE_PDU     3
#define TARP_NOTIFY_PDU       4
#define TARP_RQST_PROT_PDU    5

/*
 * TARP timer values.
 *
 * Timer                   Description
 * -----                   -----------
 *
 * T1                  Waiting for response to TARP Type 1 PDU
 * T2                  Waiting for response to TARP Type 2 PDU
 * T3                  Waiting for response to TARP addr resolution request 
 * T4                  Starts when T2 expires
 *
 * LDB_FLUSH_TIMER is the timer for a cache entry in the loop
 * detection cache. Default is 5 minutes, range is 0 - 1440 minutes.
 *
 * LDB_ZERO_SEQ_TIMER is the time period a particular LDB entry is considered
 * to be untouchable by another TARP PDU with a sequence number of zero. This 
 * is done to avoid a barrage of packets in a topology with loops.
 */

/* Default */
#define TARP_DEF_TYPE1_TIMEOUT         (15*ONESEC)      /* 15 seconds */
#define TARP_DEF_TYPE2_TIMEOUT         (25*ONESEC)      /* 25 seconds */
#define TARP_DEF_ARP_RQST_TIMEOUT      (40*ONESEC)      /* 40 seconds */
#define TARP_DEF_POST_TYPE2_TIMEOUT    (15*ONESEC)      /* 15 seconds */
#define TARP_DEF_LDB_FLUSH_TIMER       (300*ONESEC)     /* 5 minutes  */
#define TARP_DEF_LDB_ZERO_SEQ_TIMER    (300*ONESEC)     /* 5 minutes  */
#define TARP_DEF_TID_CACHE_TIMER       (3600*ONESEC)    /* 60 minutes */

#define TARP_DEF_LIFETIME              100      /* default is 100 hops */
#define TARP_NSAP_PROT_ADDR_TYPE 0xFE /* Protocol type for a CLNS NSAP
					 is "FE" */
#define TARP_DEF_NSEL_TYPE 0xAF       /* default NSEL for TARP PDU's is AF */

/* Minimum */
#define TARP_MIN_TYPE1_TIMEOUT         0
#define TARP_MIN_TYPE2_TIMEOUT         0
#define TARP_MIN_ARP_RQST_TIMEOUT      0    /* (actually it's >= T1 + T2 */
#define TARP_MIN_POST_TYPE2_TIMEOUT    0
#define TARP_MIN_LDB_FLUSH_TIMER       0
#define TARP_MIN_LDB_ZERO_SEQ_TIMER    (1*ONEMIN)     /* 1 minute */
#define TARP_MIN_TID_CACHE_TIMER       (5*ONEMIN)     /* 5 minutes */
#define TARP_MIN_SEQUENCE_NUMBER       0
#define TARP_MIN_LIFETIME              0     
#define TARP_MIN_PROTOCOL_TYPE         0x0
#define TARP_MIN_NSEL_TYPE             0x0

/* Maximum */
#define TARP_MAX_TYPE1_TIMEOUT         (60*ONEMIN)      /* 60 minutes */
#define TARP_MAX_TYPE2_TIMEOUT         (60*ONEMIN)      /* 60 minutes */
#define TARP_MAX_ARP_RQST_TIMEOUT      (60*ONEMIN)      /* 60 minutes */
#define TARP_MAX_POST_TYPE2_TIMEOUT    (60*ONEMIN)      /* 60 minutes */
#define TARP_MAX_LDB_FLUSH_TIMER       (1440*ONEMIN)    /* 1440 minutes */
#define TARP_MAX_LDB_ZERO_SEQ_TIMER    (10*ONEMIN)      /* 10 minutes */
#define TARP_MAX_TID_CACHE_TIMER       (1440*ONEMIN)    /* 1440 minutes */
#define TARP_MAX_SEQUENCE_NUMBER       (unsigned)0xFFFF /* seq # is 2 Bytes */
#define TARP_MAX_LIFETIME              (unsigned)0xFFFF /* lifetime is 2
							   Bytes */
#define TARP_MAX_PROTOCOL_TYPE         (unsigned)0xFF   /* protocol type is
							   1 byte */
#define TARP_MAX_NSEL_TYPE             (unsigned)0xFF   /* N-selector is
							   1 byte */

/* TARP Statistics */

enum {TARP_INPUT, TARP_OUTPUT, TARP_HDRSYNTAX, TARP_NOMEM, TARP_BADPACKET,
      TARP_LIFETIME_EXCEEDED,

      TARP_MAXTRAFFIC};

#define TARP_PERIODIC_TIMER   (10*ONESEC)		/* 10 seconds */
#define TARP_SEND_TYPE4_TIMER (5*ONESEC)

/*
 * TARP TID-NET cache entry
 */

typedef struct tarp_tid_cache_type_ {
    struct tarp_tid_cache_type_ *next; /* pointer to next entry */
    uchar prot_addr_type;              /* Type of protocol(most likely CLNP) */
    NSAP_T net;                        /* this entry's NET */
    uchar *tid;                         /* this entry's TID */
    uchar flags;                       /* entry flags */
    sys_timestamp expiration_time;     /* when we forget this entry */
} tarp_tid_cache_type;

/*
 * TID cache entry flags
 */
#define TARP_STATIC_FLAG  0x01
#define TARP_MY_ENTRY_FLAG 0x02

/*
 * Search keys on TARP TID-NET cache
 */
enum TARP_TID_NET_KEY {TARP_TID_KEY, TARP_NET_KEY};

/*
 * Source/destination enumeration type.
 */
enum TARP_SRC_DST {TARP_SOURCE, TARP_DEST};

/*
 * Hash table to store entries
 */
tarp_tid_cache_type *(tarp_tid_table[NETHASHLEN]);

/*
 * TARP loop detection cache entry.
 */

typedef struct tarp_ldb_type_ {
    struct tarp_ldb_type_ *next;
    ushort sequence;                     /* this entry's sequence # */
    uchar sys_id[STATION_LENGTH];         /* this entry's 6 byte system ID */
    sys_timestamp expiration_time;       /* when we forget this entry */
    sys_timestamp zero_seq_time;         /* ignore zero-seq packets if
					    this timer is running */
} tarp_ldb_type;

/*
 * TARP loop detection cache.
 */

tarp_ldb_type *(tarp_ldb_table[NETHASHLEN]);

enum TARP_TABLE_TYPE {TARP_TID_CACHE, TARP_LDB_CACHE};

/*
 * List of all statically configured adjacencies. Elements
 * can be of different types:
 *
 * TARP_BLACKLIST - NET of adjacency to whom we will not propagate TARP PDU's.
 * TARP_STATIC_MAP - statically configured NET<->TID mapping. The 'tid' for
 *                   such entries will be non-NULL.
 * TARP_STATIC_ADJ - statically configured TARP adjacencies. This will be
 *                   used to propagate TARP PDU's to a host that does not
 *                   have a CLNS adjacency to this host. This is useful when
 *                   two TARP hosts are separated by a non-TARP host.
 */
typedef struct tarp_static_adjQ_type_ {
    struct tarp_static_adjQ_type_ *next;
    NSAP_T nsap;
    uchar *tid;          /* TID - applicable only to TARP_STATIC_MAP entries */
    uchar type;          /* Type of entry - see list following this  struct */
} tarp_static_adjQ_type;

enum TARP_ADJ_ENTRY_TYPE {TARP_BLACKLIST, TARP_STATIC_MAP, TARP_STATIC_ADJ,
			  TARP_MAX_QUEUE};

/*
 * Queue of all outstanding requests. Queue elements can be of two types:
 *
 * TARP TID request (L1 or L2) - Used when we need an NET for a given TID.
 *                   If we'd sent a Type 1 or 2 PDU, we enqueue an entry 
 *                   with a TID, and dequeue it when a response (with an NET)
 *                   is received. If we get a response for which there 
 *                   is no matching entry in this queue, we discard the 
 *                   response as a duplicate.
 *
 * TARP NET request - Used when we need a TID for a given NET.
 *                   When a Type 5 PDU is sent, we enqueue an entry with an
 *                   NET, and dequeue it when a response (with a TID) is
 *                   received. If we get a response for which there is no
 *                   matching entry in this queue, we discard the response
 *                   as a duplicate.
 */

enum TARP_RESPONSE_TYPE {TARP_RESP_FAILURE,
			 TARP_RESP_SUCCESS, TARP_RESP_TIMEOUT, 
			 TARP_RESP_INTERRUPTED};

typedef struct tarp_rqstQ_type_ {
    struct tarp_rqstQ_type_ *next;
    uchar *tid_or_net;             /* TID (PDU type 1 or 2) or NET (type 5)*/
    uchar type;
    tt_soc *tty;                    
    sys_timestamp expiration_time;
    enum TARP_RESPONSE_TYPE status;
} tarp_rqstQ_type;

/*
 * Is this host an ES or an IS?? 
 */
enum TARP_HOST_TYPE {TARP_NULL_HOST_TYPE, TARP_IS_TYPE, TARP_ES_TYPE};

#endif __TARP_H__


