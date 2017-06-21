/* $Id: probe.h,v 3.2.60.2 1996/06/18 01:46:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/probe.h,v $
 *------------------------------------------------------------------
 * probe.h -- Hewlett Packard 802.2 Probe Protocol Prototypes
 *
 * February 1991, David Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: probe.h,v $
 * Revision 3.2.60.2  1996/06/18  01:46:02  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/15  21:16:47  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:36:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:00:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PROBE_H__
#define __PROBE_H__

/*
 * General Probe definitions
 */
#define HPCODELEN 7
#define HPEXTEND_PAD 3			/* Padding, described below */
#define HPCODELEN_ARPA (HPCODELEN - HPEXTEND_PAD)/* Don't pad over ARPA encap*/
#define PROBE_VERSION 0
#define HPDSN	1			/* the addressing domain HPDSN */

/*
 * Probe packets can be encapsulated in Ethernet or IEEE 802.2.  In either
 * case, the packet starts with a magic SAP extension indicating that an HP
 * proprietary extension to IEEE 802.2 follows.  This consists of three
 * bytes of pad followed by the extended SAP.  See the variable hpcode in
 * probe.c, too.
 *
 * General pad and SAP {0,0,0,5,3,5,3}
 * DTC pad and SAP     {0x20, 0x20, 0x20, 9, 0x65, 9, 0x65}
 */

#define HPEXTENDEDSAP	0x5030503

/*
 * Request and reply types
 */
#define NAME_REQUEST 	0x10
#define ADDR_REQUEST 	0x11
#define WHERE_REQUEST 	0x12
#define NAME_REPLY   	0x13
#define ADDR_REPLY   	0x14
#define UNSOL_REPLY  	0x15
#define WHERE_REPLY	0x16
#define PROXY_REQUEST	0x17
#define PROXY_REPLY	0x18
#define DOWN_REPLY	0x19

/*
 * CSCdi07039
 * Bit masks of protocol types for distingushing
 * probe proxy packets from probe arp packets.
 */
#define PROBE_ARP_TYPES		(1 << NAME_REQUEST | 1 << ADDR_REQUEST | \
				 1 << WHERE_REQUEST | 1 << NAME_REPLY | \
				 1 << ADDR_REPLY | 1 << UNSOL_REPLY | \
				 1 << WHERE_REPLY | 1 << DOWN_REPLY)
#define PROBE_PROXY_TYPES	(1 << PROXY_REQUEST | 1 << PROXY_REPLY)

/*
 * Protocol IDs
 */
#define PID_ADVANCENET	255		/* aka OneNet_Services */
#define PID_TRANSPORT	254		/* aka OneNet_Transport */
#define PID_IP		8
#define PID_IEEE	7

/*
 * Capability bits
 */
#define CAPA_ALL	0xffff

/*
 * Definitions for Probe Virtual Address Request
 */
#define ADDR_REQLEN 0x10
#define REPORT_LENGTH 8 
#define DOMAIN_LENGTH 6
#define REQUEST_VERSION 0
#define DOMAIN_NUMBER 1

/*
 * Definitions for Probe Address Reply
 */
#define ADDR_REPLEN 6

#define HPHDRMACRO \
    uchar hpcode[HPCODELEN];	/* special HP code goes here */ \
    uchar version;		/* currently version zero */ \
    uchar type;			/* message type code */ \
    uchar messlen[2];		/* length of this type of message */ \
    uchar sequence[2];		/* unique identifier */

#define HPHDRLENGTH (HPCODELEN+6)	/* 13 */

#define HPPATHMACRO \
    uchar reportlen[2];		/* report length */ \
    uchar domreportlen[2];	/* domain report length */ \
    uchar vnaversion;		/* VNA version */ \
    uchar domain;		/* domain */ \
    uchar address[4];		/* IP address */ \
    uchar pathlen[2];		/* path length */ \
    uchar pid1;			/* pid (AdvanceNet) */ \
    uchar pid1len;		/* element length */ \
    uchar pid1bits[2];		/* service bits */ \
    uchar pid2;			/* another pid (Transport grp) */ \
    uchar pid2len;		/* another element length */ \
    uchar pid2bits[2];		/* service bits again */ \
    uchar pid3;		        /* IP pid */ \
    uchar pid3len;		/* another element length */ \
    uchar ipsapnum[2];		/* IP sap number */ \
    uchar pid4;  		/* IEEE 802 pid */ \
    uchar pid4len;		/* IEEE element length */ \
    uchar ieeesap[2];		/* IEEE sap number */ \
    uchar ieeeaddr[6];		/* Ethernet address */

#define HPPATHLENGTH 34

typedef struct probetype_ {
    HPHDRMACRO;
    uchar reportlen[2];		/* report length */
    uchar domlen[2];		/* domain length */
    uchar reqversion;		/* request version number */
    uchar domain;		/* domain number  */
    uchar address[4];		/* IP address */
} probetype;

#define PROBETYPELENGTH (HPHDRLENGTH+10) /* 23 */

typedef struct probenametype_ {
    HPHDRMACRO;
    uchar nametype;		/* name type */
    uchar namelen;		/* name length */
    uchar name[50];		/* name */
} probenametype;
#define PROBENAMETYPELENGTH (HPHDRLENGTH+52) /* 65 */

typedef struct probenamereptype_ {
    HPHDRMACRO;
    HPPATHMACRO;
} probenamereptype;
#define PROBENAMEREPTYPELENGTH (HPHDRLENGTH+HPPATHLENGTH) /* 47 */

typedef struct probepath_ {
    HPPATHMACRO;
} probepath;

typedef struct probewhereis_ {
    HPHDRMACRO;
    uchar replen1[2];		/* report length 1 */
    uchar domreplen1[2];	/* domain report length 1 */
    uchar vers1;		/* version 1 */
    uchar domain1;		/* domain 1 */
    uchar destination[4]; 	/* destination address */
    uchar replen2[2];		/* report length 2 */
    uchar domreplen2[2];	/* domain report length 2 */
    uchar vers2;		/* version 2 */
    uchar domain2;		/* domain 2 */
    uchar source[4];		/* source address */
} probewhereis;

typedef struct probewhereisreply_ {
    HPHDRMACRO;
    uchar replen[2];		/* report length */
    uchar domreplen[2];		/* domain report length */
    uchar vers;			/* version */
    uchar domain;		/* domain */
    uchar gateway[4];		/* gateway address */
    uchar pathlen[2];		/* path length */
    uchar pid;			/* IEEE/ETHER */
    uchar elemlen;		/* element length */
    uchar sap[2];		/* sap number */
    uchar ieeeaddr[6];		/* Ethernet address */
    uchar hopcount[2];		/* hop count */
} probewhereisreply;

#define PROBEWIRLENGTH (HPHDRLENGTH+24) /* 37 */

/*
 * requests is an array of probe requests we have made.
 */

#define MAXPROBES 20
#define DECAYCOUNT 7

typedef struct probesenttype_ {
    uint address;
    ushort sequence;
    ushort decay;
} probesenttype;


/*
 * PROBE traffic statistics
 */
typedef struct probe_traffic_t_ {
    ulong reqrcv;
    ulong replyrcv;
    ulong otherrcv;
    ulong replysent;
    ulong replyproxy;
    ulong reqsent;
    ulong namercv;
    ulong namesent;
    ulong wherercv;
    ulong wheresent;
} probe_traffic_t;


extern process probe_input(void);
extern int lookup_address(uint);
extern int lookup_sequence(uint);
extern paktype *hp_proxy_request(paktype *, idbtype *, probetype *, uchar *);
extern paktype *hp_whereis_request(paktype *pak, idbtype *idb,
				   probewhereis *probe, uint linktype,
				   uchar *hardware);
extern void probe_sendreq(paktype *, ipaddrtype, idbtype *, hwaddrtype *);
extern void probe_enqueue(paktype *);
extern void probe_show_traffic(void);
extern void probe_arp_update(idbtype *, ipaddrtype, long, uchar *, int, int);

extern void probe_parser_init(void);

#endif __PROBE_H__
