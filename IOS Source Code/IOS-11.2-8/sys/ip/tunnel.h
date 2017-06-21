/* $Id: tunnel.h,v 3.3.60.3 1996/05/08 20:55:55 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ip/tunnel.h,v $
 *------------------------------------------------------------------
 * Generic Tunneling Information (non private data structures)
 *
 * April, 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: tunnel.h,v $
 * Revision 3.3.60.3  1996/05/08  20:55:55  lwei
 * CSCdi57099:  rsp-boot-m image can not be built due to the fix to
 * CSCdi56122
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/05/08  01:47:04  lwei
 * CSCdi56122:  Tunnel interface default MTU should not show up in nvgen
 * output
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/23  01:33:30  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3  1995/11/17  09:37:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:59:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/25  03:24:24  myeung
 * CSCdi38715:  OSPF route change brings down tunnel line protocol
 * - Add blind timer to tolerate transient failure
 *
 * Revision 2.1  1995/06/07  21:01:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TUNNEL_H__
#define __TUNNEL_H__

/* Bandwidth and delay for display and IGRP */

#define VISIBLE_TUNNEL_BANDWIDTH 9	/* Nominally 9.6K */
#define TUNNEL_DELAY 50000		/* 500 msec */

/*
 * Timer values...
 * Time between IP header calculations.
 * Time to shutdown if we detect a recursive route (don't X the streams).
 * Time to ignore "down" routes before really going down.
 */

#define	TUNNEL_HEADCALCTIME	(2  * ONESEC)
#define	TUNNEL_STOMPTIME	(60 * ONESEC)
#define	TUNNEL_BLINDTIME	(5  * ONESEC)

typedef	enum tunnel_mode_ {
    UNINITIALIZED_TUN_MODE,		/* must be first */
    TUN_MODE_GRE_IP,
    TUN_MODE_MGRE_IP,
    TUN_MODE_NOS_IP,
    TUN_MODE_IP_IP,
    TUN_MODE_DVMRP_IP,			/* braindead abortion */
    TUN_MODE_EON_IP,
    TUN_MODE_CAYMAN_UDP,
    TUN_MODE_AURP_UDP,
    TUN_MODE_IPTALK_UDP,
    END_TUN_MODE			/* must be last! */
} tunnel_mode;

#define	TUN_MODE_DEFAULT TUN_MODE_GRE_IP

typedef enum tunnel_transport_ {
    TUN_TRANS_IP,
    TUN_TRANS_IP_MULTI
} tunnel_transport;

typedef paktype * (*encaps_t)(tunnel_info *, paktype *); /*encapsulate pkt */
typedef void (*resync_t)(tunnel_info *);                 /*sequencing resync*/
typedef void (*cstate_encaps_t)(idbtype *, int);         /*encaps state chg*/

struct tunnel_info_ {
        struct tunnel_info_ *next;	/* next tunnel in tunnel queue */
        hwidbtype *hwidb;		/* parent idb pointer */
        encaps_t encaps;		/* encapsulate pkt */
	resync_t resync;		/* sequencing resync */
	cstate_encaps_t cstate;		/* encaps state chg */
	tunnel_mode mode;		/* encapsulation mode */
	tunnel_transport transport;	/* transport mode */
	const char *modestring;		/* pointer to text mode */
        ipaddrtype source;		/* address to source encapsulated
					   packets */
	idbtype   *source_idb;		/* --or pointer to IDB whose address
					   we use */
	ipaddrtype destination;		/* address of the other side of the
					   tunnel */
	int protocol;			/* layer 3 protocol info */
	boolean checksum;		/* should we do end-to-end checksum? */
	boolean checksum_default;	/* default checksum value */

	boolean fast_capable;  		 /* Whether fast tunnelling is
					    possible */ 
	boolean fast_enabled;  		 /* Whether fast tunnelling is
					    enabled */ 
	sys_timestamp out_timer;   	/* guard for route flapping */
 	sys_timestamp blind_timer;	/* sort of an "anti-holddown" check */
	paktype	  *header;		/* IP header to bcopy into packet */
	idbtype   *if_output;           /* Current idb of next hop */
	ushort	   ident;		/* IP identification number */

	/* GRE specific fields */
	boolean	carry_ipso;		/* acquire IP SO or CIPSO from parent */
	boolean	enable_key;		/* provide a tunnel key */
	ulong   key;			/* the actual key */
	boolean	enable_seq;		/* provide sequencing for ordering */
	ulong	t_sequence;		/* sequence field of last sent packet */
	ulong	r_sequence;		/* sequence field of last rcvd packet */
	ulong	srclen;			/* source route in bytes (0..255) */
	ipaddrtype source_route[64];	/* source route */

	/* AURP specific fields */
	boolean aurp_enabled;
	struct at_aurp_info_ *aurp;
};

/*
 * Macro to traverse tunnel queue
 */
#define FOR_ALL_TUNNELS(t) \
    for ((t) = (tunnel_info *) tunnelQ->qhead; (t); (t) = (t)->next)

/*
 * Local address for tunnel
 */
#define TUNNEL_MYADDR(tinfo) \
    (tinfo->source_idb ? tinfo->source_idb->ip_address : \
        tinfo->source)

/*
 * tunnel.c
 */ 
extern queuetype *tunnelQ;	/* head of list of tunnels */
extern boolean	  tunnel_vencap(paktype *, long);
extern boolean	  tunnel_debug;

/*
 * tunnel_chain.c
 */
extern void tunnel_parser_init(void);
#endif
