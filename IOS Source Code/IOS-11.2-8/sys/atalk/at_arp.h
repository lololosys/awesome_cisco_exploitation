/* $Id: at_arp.h,v 3.2.58.1 1996/03/23 01:25:43 slin Exp $
 * $Source: 
 *------------------------------------------------------------------
 * at_arp.h -- definitions for Appletalk ARP protocol
 *
 * April 1992, David S.A. Stine
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_arp.h,v $
 * Revision 3.2.58.1  1996/03/23  01:25:43  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:41:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:07:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __AT_ARP_H__
#define __AT_ARP_H__

#include "../h/address.h"

/*
 * AARP hardware types. Note that FDDI is the same as Ethernet, since
 * the FDDITalk proposal says that this is necessary.
 */
typedef enum {
    AARP_HWUNK     = 0,
    AARP_HWETHER   = 1,
    AARP_HWTR      = 2,
    AARP_HWFDDI   = AARP_HWETHER,
    AARP_HWOLDFDDI = 3,
    AARP_HWSMDS    = 4,
    AARP_HWFR      = 5,
    AARP_HWX25     = 6,
    AARP_HWSMDSTALK = 14
} aarp_hw_t;

/*
 * AppleTalk ARP private structures.
 */


/*
 * AARP "ops" structure;
 *
 * This works like the nbpOps structure; the underlying idea is the
 * same.
 * When we receive a an AARP packet in from the outside world, we
 * decode it into one of these structures to reduce the amount of
 * logic we have to carry around with us to break out the variable-length
 * hardware addresses.
 *
 * A copy of this information is kept in the AARP tree.
 */
typedef struct _AARPOps {
    paktype	*pak;			/* pointer to the pak we got it fm */
    ushort	opcode;			/* AARP function code */
    ushort	hw_type;		/* hardware type typecode */
    ushort	protoType;		/* protocol type ($809B typically) */
    uchar	ptAddrLen;		/* protocol address len (usually 4) */
    uchar	hwAddrLen;		/* hardware address len (usually 6) */
    uchar	srcHwAddr[24];		/* source hw address */
    uchar	dstHwAddr[24];		/* destination hw address */
    ataddrtype	srcPtAddr;		/* source 24-bit AT address */
    ataddrtype	dstPtAddr;		/* dest/target 24-bit AT address */
    ushort	srcPtAddrNet;		/* source AT addr, net number */
    ushort	dstPtAddrNet;		/* dest. AT addr, net number */
    uchar	srcPtAddrNode;		/* source AT addr, node number */
    uchar	dstPtAddrNode;		/* source AT addr, node number */
} aarpOps_t;


typedef enum {
    AARP_NULL,				/* undefined - error */
    AARP_DYNAMIC,			/* we learned this entry */
    AARP_STATIC,			/* static entry, eg, X.25 mapped */
    AARP_INTERFACE,			/* interface entry */
    AARP_ALIAS,				/* static, we're fronting this host */
    AARP_WAITING,			/* We are waiting for ARP response */
    AARP_TEST,				/* Test AARP entry */
    AARP_ENTRY_TYPE_MAX			/* don't index on this;used to alloc */
} aarp_entry_t;

/*
 * AARP tree entries are keyed by their AppleTalk addresses. The MSB of the
 * key might be left for later double-keying. (perhaps the IDB numbers,
 * ala the neighbor entry tree?
 */
typedef struct aarpentry_ {
    struct treeLink_ link;		/* tree link structure */
    ataddrtype        proto_addr;	/* protocol address */
    encapstype        enctype;		/* how this entry got here */
    atalkidbtype           *idb;		/* interface for this entry */
    aarp_entry_t      type;		/* class of this entry */	
    ushort            arp_retries;	/* number of arp retries done */
    sys_timestamp     time;	        /* time of last update */
#define ARP_MAX_RETRIES		10
    paktype           *unarped_pak;	/* link to un-arp'd packet */
    uchar	      protoType[2];	/* protocol type, typically $80F3 */
    short             hw_len;		/* hw addr len, -1 means  'invalid' */
    uchar             hw_addr[STATIONLEN_MAX]; /* hardware address */
} aarpentry_t;


/*
 * The following goop was extracted from ../if/arp.h
 */

#define PL_AARP 4			/* protocol address len is 4 bytes */
#define AARP_NETBYTES 2			/* AARP AT Protocol net length */

/*
 * Packet format and field definitions for AppleTalk ARP
 *
 * Note that in fact, when dealing with the variable-length addresses
 * that AARP now supports, we won't be using the fixed fields beyond
 * the first h/w address in the packet. A pointer is set to point to the
 * first h/w address in the packet and the rest of the information is
 * trowled into the packet according to the MAC-level address size.
 */

typedef struct aarp_hdr_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    uchar ipshdw[IEEEBYTES];		/* 48 bits of source hdw address */
#define AARP_PADBYTES 1			/* pad bytes in proto ad., must be 0 */
    uchar ipspro_pad[AARP_PADBYTES];    /* 8 bits of pad, must be 0 */
    uchar ipspro_net[AARP_NETBYTES];    /* 16 bits of source protocol, AT Net */
    uchar ipspro;			/* 8 bits of source protocol */
    uchar ipthdw[IEEEBYTES];		/* 48 bits of target hdw address */
    uchar iptpro_pad[AARP_PADBYTES]; 	/* 8 bits of pad, must be 0 */
    uchar iptpro_net[AARP_NETBYTES];    /* 16 bits of target protocol, AT Net */
    uchar iptpro;			/* 8 bits of target protocol address */
} aarp_hdr;

extern inline aarp_hdr *
aarpheadstart (paktype *pak) 
{
    return (aarp_hdr *) pak->network_start;
}

/*
 * The way we deal with a variable-length h/w address is to set up a struct
 * which contains pointers to all the interesting portions of an AARP
 * packet from the beginning of the source h/w address on down.
 */
typedef struct aarp_ptrs_ {
    uchar	*src_hw_addr;		/* ptr to start of iphdw */
    uchar	*src_pt_addr;		/* ptr to start of 4-byte pt addr */
    uchar	*src_pt_net;		/* ptr to 2-byte network number */
    uchar	*src_pt_node;		/* ptr to 1-byte nide number */
    uchar	*dst_hw_addr;		/* ptr to start of ipthdw */
    uchar	*dst_pt_addr;		/* ptr to start of 4-byte pt addr */
    uchar	*dst_pt_net;		/* ptr to 2-byte network number */
    uchar	*dst_pt_node;		/* ptr to 1-byte nide number */
} aarp_ptrs_t;

#define OP_AARP_REQUEST 1
#define OP_AARP_REPLY 	2
#define OP_AARP_PROBE   3

/*
 * AARP_HEADER_SIZE accounts for the size of an AARP packet without
 * the two hardware addresses in the packet. So to get the
 * true packet size, you want AARP_HEADER_SIZE + 2*aarp_hwlen(idb)
 */
#define AARP_HEADER_SIZE	(2+2+1+1+2 + 4 + 4)

#define AARP_PROBE_INTERVAL  (ONESEC/5) /* reXmit at 1/5 sec. intervals */
#define AARP_PROBE_RETRANSMIT 10 	/* reXmit this many times */

#define AARP_REQUEST_INTERVAL  (ONESEC)	/* reXmit at 1 sec. intervals */
#define AARP_REQUEST_RETRANSMIT 5 	/* reXmit this many times */
#define AARP_ENTRY_TIMEOUT	(4*ONEHOUR)
#define AARP_GC_LIMIT		50

extern pid_t aarp_pid;

struct aarpentry_ *	
aarp_insert(atalkidbtype*, ataddrtype, uchar const *, ushort, encapstype,
	    aarp_entry_t);

#endif __AT_ARP_H__
