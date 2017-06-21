/* $Id: vsvc_nsm.h,v 3.2 1995/11/17 17:59:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vsvc_nsm.h,v $
 *------------------------------------------------------------------
 * vsvc_nsm.h - Support for the `VINES Network and System
 *				 Management' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_nsm.h,v $
 * Revision 3.2  1995/11/17  17:59:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 *		    VINES NSM General Definitions
 *
 **********************************************************************/

/*
 * NSM Error codes.   From VINES: Network and System Management
 * Programming Interface, et. al.
 */
#define VNSM_ERR_NOTSUPPORTED	7001
#define VNSM_ERR_NOTIMPLEMENTED	7002
#define VNSM_ERR_INVPARM	7003
#define VNSM_ERR_NOMEM		7004
#define VNSM_ERR_NOTSERIAL	7005
#define VNSM_ERR_NOTLAN		7006
#define VNSM_ERR_NOTCONFIG	7007
#define VNSM_ERR_UNSUPREV	7017
#define VNSM_ERR_ALREADYREG	7023
#define VNSM_ERR_NOTREG		7024
#define VNSM_ERR_BCASTPORT	7026

/*
 * Miscellaneous NSM defines determined by inspection of Sniffer
 * Traces.
 */
#define VNSM_VERSION_1		1
#define VNSM_VERSION_2		2

#define VNSM_TAGS		1
#define VNSM_DATA		2

#define VNSM_RUNTIME		1

/*
 * The length of a MAC address.  Found many places in the API header
 * files.  For one reference, see nmlan.h: LIF_LNKADDRLEN.
 */
#define VNSM_MAX_MACLENGTH	6
#define VNSM_MAX_X25LENGTH	16

/*
 * Various string lengths used by NSM.  The documentation references
 * are listed, where they exist.
 */
/* From nmpstk.h */
#define VNSM_LEN_STACKNAME	16	/* nmpstk.h: PST_STKTAGSZ */
#define VNSM_LEN_FAMILYNAME	16
#define VNSM_LEN_PROTOCOLNAME	16	/* nmpstk.h: PST_PROTTAGSZ */
#define VNSM_LEN_TABLENAME	32	/* nmpstk.h: PST_TBLNAMESZ */
#define VNSM_LEN_PRODUCTNAME	32	/* nmserver.h */
#define VNSM_LEN_OSREVNAME	32	/* nmserver.h */
#define VNSM_LEN_OSTYPENAME	32	/* nmserver.h */
#define VNSM_LEN_DISKNAME	14	/* nmserver.h */
#define VNSM_LEN_IFNAME		64	/* nmlan.h */
#define VNSM_LEN_HELPSECTION	16
#define VNSM_LEN_HELPFILE	16
#define VNSM_LEN_LANGUAGE	16

#define VNSM_LEN_LABELMAX	64

/*
 * Statistics data types.  Gag.  These are part of a set of self
 * defining fields that Banyan uses.  They are enumerated in the
 * Banyan API file nmcommon.h.
 */
#define VNSM_NMA_COUNTER16	1
#define VNSM_NMA_COUNTER32	2
#define VNSM_NMA_RATE		3
#define VNSM_NMA_GAGUE16	4
#define VNSM_NMA_GAGUE32	5
#define VNSM_NMA_TICKS16	6
#define VNSM_NMA_TICKS32	7
#define VNSM_NMA_BOOLEAN	8
#define VNSM_NMA_BYTE		9
#define VNSM_NMA_CSTRING	10
#define VNSM_NMA_XSTRING	11
#define VNSM_NMA_BITSTRING	12
#define VNSM_NMA_STATUS16	13
#define VNSM_NMA_LOWTIDE16	14
#define VNSM_NMA_LOWTIDE32	15
#define VNSM_NMA_HIGHTIDE16	16
#define VNSM_NMA_HIGHTIDE32	17
#define VNSM_NMA_LLADDRESS	18
#define VNSM_NMA_CARDSEQ	19
#define VNSM_NMA_BYTESTRING	20
#define VNSM_NMA_IPADDRESS	21
#define VNSM_NMA_NSAP		22
#define VNSM_NMA_SNPA		23
#define VNSM_NMA_SockAddrISO	24
#define VNSM_NMA_INT16		25
#define VNSM_NMA_INT32		26
#define VNSM_NMA_ISTRING	27
#define VNSM_NMA_EMBED_INT_TBL  28
#define VNSM_NMA_EMBED_INT_STAT 29
#define VNSM_NMA_TIME_TICKS	30
#define VNSM_NMA_STAUS32	31

/*
 * See "Managing And Optimizing A VINES Network", pp 14-26,33
 */
#define VNSM_FLAG_USABLE	0x0001
#define VNSM_FLAG_SERVER	0x0002
#define VNSM_FLAG_NEIGHBOR	0x0004	/* used only in neighbor table */
#define VNSM_FLAG_DELETE_ME	0x0008
#define VNSM_FLAG_FOR_DOS	0x0010
#define VNSM_FLAG_WHO_KNOWS	0x0020	/* not visible via NSM */
#define VNSM_FLAG_MODIFIED	0x0040
#define VNSM_FLAG_WHO_KNOWS2	0x0080	/* not visible via NSM */
#define VNSM_FLAG_NON_VINES	0x0100
#define VNSM_FLAG_REDIRECT_C	0x0200	/* created by redirect */
#define VNSM_FLAG_SUPPRESSION1	0x0400
#define VNSM_FLAG_SUPPRESSION2	0x0800
#define VNSM_FLAG_WAN		0x1000
#define VNSM_FLAG_LAN		0x2000
#define VNSM_FLAG_REDIRECT_M	0x4000	/* modified by redirect */
#define VNSM_FLAG_PRE_550	0x8000


/**********************************************************************
 *
 *	       VINES NSM Procedure specific definitions
 *
 **********************************************************************/

/**********
 *
 * Get Server Info (1)
 *
 **********/
#define VNSM_SVR_OSINFO		1	/* sniffer traces */
#define VNSM_SVR_FSINFO		2	/* sniffer traces */
#define VNSM_SVR_COMMINFO	3	/* sniffer traces */
#define VNSM_SVR_CPUINFO	4	/* sniffer traces */

#define VNSM_SVR_OS_UNIX	1	/* nmserver.h */

#define VNSM_NUM_STACKS		10
#define VNSM_NUM_PROTOCOLS	10

/**********
 *
 * Get Server Stats (4)
 *
 **********/
#define VNSM_SVR_SUMMSTAT	1	/* sniffer traces */
#define VNSM_SVR_FSSTAT		2	/* sniffer traces */
#define VNSM_SVR_DISKSTAT	3	/* sniffer traces */

#define VNSM_MAX_DISKS		5	/* not that we have any */

/**********
 *
 * Get LAN Information (7)
 *
 **********/
#define VNSM_LIF_SUPP_VINES	0x01	/* nmlan.h */
#define VNSM_LIF_SUPP_IP	0x02	/* nmlan.h */
#define VNSM_LIF_SUPP_ATALK	0x04	/* nmlan.h */
#define VNSM_LIF_SUPP_ISO	0x08	/* nmlan.h */

/**********
 *
 * Get LAN IF Info Summary (11)
 *
 **********/
#define VNSM_LANIF_COMMSTATS	1	/* sniffer traces */
#define VNSM_LANIF_IFSTATS	4	/* sniffer traces */

#define VNSM_NUM_INTERFACES	20

/**********
 *
 * Get Protocol Stack Table Information (14)
 *
 **********/

/* From nmcommon.h */
#define VNSM_FAMILY_VINES	1
#define VNSM_FAMILY_DARPA	2
#define VNSM_FAMILY_ATALK	3
#define VNSM_FAMILY_ISO		5
#define VNSM_FAMILY_IPX		6

/* From nmpstk.h */
#define VNSM_PROT_VINESARP	1
#define VNSM_PROT_VINESIP	2
#define VNSM_PROT_VINESRTP	3
#define VNSM_PROT_VINESSPP	5

/* From nmpstk.h */
#define VNSM_TBL_VRTP_NBRS	1	/* Neighbors */
#define VNSM_TBL_VRTP_ROUTES	2	/* Routes */

/**********
 *
 * Get Protocol Stack Information (15)
 *
 **********/

/*
 * Cisco local data structure.  Remembers where the router was in walking
 * its tables when the last message was filled.  Allows it to continue
 * from that point when the next request arrives.
 */
typedef struct vnsm_pstk_pass_info_ {
    dqueue_t	p;
    ulong	req_net;
    ushort	req_host;
    ushort	family;
    ushort	protocol;
    ushort	table;
    ulong	iteration;
    ulong	last_net;
    ushort	last_host;
} vnsm_pstk_pass_info;

#define VNSM_RTP_MAXTTL		   5	/* counts 5 -> 0 in 90 second units */
#define VNSM_RTP_NOTAGED       65535	/* instead of one of the above numbers */
#define VNSM_DATA_LIMIT		1300	/* arbitrary limit to make data fit */

/**********
 *
 * Start Send (22)
 *
 **********/
#define VNSM_PERIOD	(ONEMIN)	/* Interval to sent to MNET client */

/*
 * Cisco local data structure.  Remembers who should be getting
 * periodic summary status (NSM call 0) messages.
 */
typedef struct vnsm_send_addr_ {
    dqueue_t       q;
    vinesaddress   address;
} vnsm_send_addr;

/**********
 *
 * Trace (28)
 *
 **********/
#define VNSM_TRACE_GNET_VALID	0x0100	/* sniffer traec */
#define VNSM_TRACE_PNET_VALID	0x0001	/* sniffer traec */


/***********************************************************************
 *
 *			     PROTOTYPES
 *
 ***********************************************************************/
extern ushort vnsm_trace_send_call(ulong, ulong);
