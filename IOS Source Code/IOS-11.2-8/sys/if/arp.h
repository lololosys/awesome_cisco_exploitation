/* $Id: arp.h,v 3.4.6.3 1996/06/18 01:44:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/arp.h,v $
 *------------------------------------------------------------------
 * arp.h -- RFC826 style ARP
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arp.h,v $
 * Revision 3.4.6.3  1996/06/18  01:44:09  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/05/22  20:54:50  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.4.6.1  1996/03/18  20:12:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:54:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/02/20  00:55:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/01  20:20:43  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.3  1996/02/13  08:12:42  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2.26.1  1996/01/24  22:17:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:25:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ARP_H__
#define __ARP_H__


/*
 * Generic ARP definitions
 */

#define HW_ETH 1			/* hardware is 10MB Ethernet */
#define HW_802 6			/* hardware is 802.2 network */
#define HW_SMDS_RFC 14
#define SMDSBYTES 8
#define HL_ETH 6			/* hardware addr length is 6 bytes */
#define HL_802 6			/* hardware addr length is 6 bytes */
#define HL_SMDS_RFC 8
#define HL_X25	20			/* Also known as X25_ADDRESS_LENGTH */
#define OP_REQ 1			/* ARP request opcode */
#define OP_REP 2			/* ARP reply opcode */
#define OP_REV_REQ 3			/* RARP request opcode */
#define OP_REV_REP 4 			/* RARP reply opcode */

/*
 * IP ARP over IEEE media
 */

#define PR_IP 0x800			/* protocol is 10MB IP */
#define PL_IP 4				/* IP protocol length is 4 bytes */
#define RBYTES 28			/* datagram length in bytes */

typedef struct rfc_arp {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    uchar ipshdw[IEEEBYTES];		/* 48 bits of source hdw address */
    ipaddrtype PACKED(ipspro);		/* 32 bits of source protocol */
    uchar ipthdw[IEEEBYTES];		/* 48 bits of target hdw address */
    ipaddrtype iptpro;			/* 32 bits of target protocol */
} rfc_arp;

#define rfc_arp_headstart(ptr)    ((ptr)->network_start)

/*
 * IP ARP over SMDS
 */

#define RSBYTES 32			/* datagram length in bytes */

typedef struct smds_arp_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    uchar ipshdw[HL_SMDS_RFC];		/* 64 bits of source hdw address */
    ipaddrtype PACKED(ipspro);		/* 32 bits of source protocol */
    uchar ipthdw[HL_SMDS_RFC];		/* 64 bits of target hdw address */
    ipaddrtype iptpro;			/* 32 bits of target protocol */
} smds_arp;

#define smds_arp_headstart(ptr)    ((ptr)->network_start)

/*
 * Apollo ARP parameters
 */
#define PR_APOLLO 0x8019		/* protocol is Apollo Domain */
#define PL_APOLLO 4			/* protocol length field */
#define APOLLO_RBYTES 28		/* datagram length in bytes */

/*
 * Hash table entry
 */

#define ARP_NULL	0		/* undefined - error */
#define ARP_DYNAMIC	1		/* we learned this entry */
#define ARP_STATIC	2		/* static entry */
#define ARP_INTERFACE	3		/* interface entry */
#define ARP_ALIAS	4		/* static, we're fronting this host */
#define ARP_MOBILE	5		/* learned this entry from mobile
					   host */
#define ARP_INCOMPLETE	6		/* we've requested, but have no
					   reply yet */

/*
 * ARP throttling rate
 */
#define ARP_THROTTLE_INTERVAL	2*ONESEC /* in ms */

/*
 * Flags for arp entry 
 */
#define ARP_EF_DUP	  0x1		/* entry contains duplicate address */

/*
 * Maximum number of retries
 */
#define ARP_MAX_RETRIES	   10

/*
 * ARP entry default maximum age
 */
#define	ARP_MAXAGE	(4*ONEHOUR)

struct arptype_ {
    struct arptype_ *next;		/* pointer to next record */
    ipaddrtype address;			/* protocol address */
    int enctype;			/* how this entry got here */
    idbtype *idb;			/* interface for this entry */
    ushort type;			/* protocol type code */
    uchar  mode;			/* class of this entry */	
    uchar flag;				/* flag word */
    uchar hwtype;			/* Type of HW address(if/adress.h) */
    uchar hwlen;			/* Length of HW address */
    uchar hardware[8];			/* hardware address */
    sys_timestamp time;			/* time of last update */
};

/*
 * ARP Traffic statistics
 */
typedef struct arp_traffic_t_ {
    ulong reqrcv;
    ulong reprcv;
    ulong revrcv;
    ulong otherrcv;
    ulong reqsent;
    ulong repsent;
    ulong proxyrep;
    ulong revsent;
} arp_traffic_t;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern arptype *arptable[];
extern arp_traffic_t arp_traffic;

/*
 * arp.c
 */
extern arptype *arp_table_lookup(long, long, idbtype *);
addrtype *arp_hwaddr_lookup (uchar *srcaddr, paktype *pak);
extern void arp_table_remove(long, long);
extern arptype *arp_table_add(idbtype *, long, long, hwaddrtype *, int, int);
extern void arp_table_reload(void);
extern void arp_display(int, void *, idbtype *, long, hwaddrtype *);
extern void arp_send(paktype *, int, uchar *, long, idbtype *, int, long);
extern void arp_show_traffic(void);
extern void arp_flush_cache(idbtype *idb);
extern void arp_clear_command(parseinfo *csb);

/*
 * arp_chain.c
 */
extern void arp_parser_init(void);

#endif __ARP_H__
