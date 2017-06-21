/* $Id: bootp.h,v 3.2.62.1 1996/04/15 21:22:31 bcole Exp $
 * $Source: /release/112/cvs/Xsys/servers/bootp.h,v $
 *------------------------------------------------------------------
 * bootp.h -- Bootstrap Protocol (BOOTP).  RFC 951.
 *  
 * 23-February-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bootp.h,v $
 * Revision 3.2.62.1  1996/04/15  21:22:31  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:50:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/29  21:13:28  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "ip bootp server" to turn/on off BOOTP server.
 * Send ICMP port unreachable if a packet is received for TACACS port and
 * there is no TACACS server configured.
 *
 * Revision 2.1  1995/06/07  22:43:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BOOTP_H__
#define __BOOTP_H__

#define AUTOINSTALL_LOAD_COUNT 	4	/* Attempts to load files */
#define MAXQUICKTRIES 12

struct bootptype_ {
	uchar	bp_op;		/* packet opcode type */
#define	BOOTREQUEST	1
#define	BOOTREPLY	2
	uchar	bp_htype;	/* hardware addr type */
	uchar	bp_hlen;	/* hardware addr length */
	uchar	bp_hops;	/* gateway hops */
	ulong	bp_xid;		/* transaction ID */
	ushort	bp_secs;	/* seconds since boot began */	
	ushort	bp_flags;	/* New flags field, per rfc1542 */
	ipaddrtype bp_ciaddr;	/* client IP address */
	ipaddrtype bp_yiaddr;	/* 'your' IP address */
	ipaddrtype bp_siaddr;	/* server IP address */
	ipaddrtype bp_giaddr;	/* gateway IP address */
	uchar	bp_chaddr[16];	/* client hardware address */
	uchar	bp_sname[64];	/* server host name */
	uchar	bp_file[128];	/* boot file name */
	ulong	bp_vend_cookie;	/* cookie indicating vendor specific data */
	uchar	bp_vend[60];	/* vendor-specific area */
};

/*
 * Flags field bits
 */
#define BOOT_FLAG_BROADCAST 0x8000

/*
 * BOOTP vendor information, as per rfc1395
 */
#define BOOTP_VEND_COOKIE 0x63825363

#define BOOTP_VEND_END 255
#define BOOTP_VEND_PAD 0
#define BOOTP_VEND_MASK 1

/*
 * async bootp data formats
 */
typedef enum bootp_async_formats_ {
    BOOTP_NUMERICAL,
    BOOTP_IP,
    BOOTP_TEXT
} bootp_async_formats;

/*
 * Extended bootp
 *
 * for any given function, we can specify different values for
 * different clients.  All specific clients come first.  The
 * default comes at the end, and has IP address of -1.
 *
 * for the moment we assume all fields are 4 bytes (which isn't
 * true for bootfilesize -- sorry)
 *
 * we need to save the format the user typed it in to allow for
 * clean displaying
 */

typedef struct bootp_spec_ {
    struct bootp_spec_ *next;		/* must be first element */
    ipaddrtype address;
    int function;
    bootp_async_formats format;
    int size;
    char data[0];
} bootp_spec;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern process bootp(void);
extern void bootp_start(void);
extern void send_bootrequest(ipsocktype *soc, idbtype *idb, int seconds);
extern void receive_bootreply(paktype *pak, bootptype *boot);
extern void nv_asyncbootp(char *);
extern void bootp_extended(bootptype *boot, ipaddrtype target);
extern void bootp_async_reply(paktype *request, udptype *udp, bootptype *boot);
extern boolean bootp_forward_reply(paktype *pak, udptype *udp, bootptype *boot);
extern boolean ai_incoming_bootp_request(hwidbtype *, paktype *, udptype *, 
					addrtype *);
extern void ai_incoming_bootp_reply(hwidbtype *, paktype *, udptype *, ushort);
#endif __BOOTP_H__
