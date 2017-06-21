/* $Id: rip.h,v 3.3 1995/12/02 10:13:01 pst Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/rip.h,v $
 *------------------------------------------------------------------
 * rip.h -- Definitions for Routing Information Protocol, aka "routed"
 *
 * 11-December-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rip.h,v $
 * Revision 3.3  1995/12/02  10:13:01  pst
 * CSCdi44892:  RIP: do clean up pass on authentication and version
 * control code
 *
 * Revision 3.2  1995/11/17  17:36:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:10:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RIP_H__
#define __RIP_H__

#define	RIP_MIN_VERSION	1	/* we support RIP v1 and RIP v2 for now */
#define	RIP_MAX_VERSION 2

#define UNSPECIFIED_FAMILY 0	/* protocol family unspecified */
#define IN_FAMILY 2		/* Internet address family */
#define	RIP_MAXMETRIC 16	/* same max hop count as PUP */
#define RIP_MAXPACKETSIZE 512 	/* largest broadcast packet size */
#define RIPHEADERBYTES 4	/* size of header in bytes */
#define MAXRIPBYTES 500		/* max data portion, 25 20-byte entries */

#define	RIPCMD_REQUEST	1	/* want information */
#define	RIPCMD_REPLY	2	/* updated information */
#define	RIPCMD_TRACEON	3	/* turn debugging on */
#define	RIPCMD_TRACEOFF	4	/* turn debugging off */

#define RIP_UPDATETIME (30*ONESEC)	/* wakeup, send updates this often */
#define RIP_INVALIDTIME (180*ONESEC)	/* invalidate entries this old */
#define RIP_GCTIME (240*ONESEC)		/* flush invalid entries this old */

#define	RIP_JITTER	15		/* jitter updates up to 15 percent */

/*
 * Command identifiers to be passed in to ip_rip_int_command
 */
enum ip_int_rip_cmdtypes {
	RIP_SEND_VERS_CMD,
	RIP_RECV_VERS_CMD,
	RIP_AUTH_MODE_CMD,
	RIP_AUTH_KEY_CMD
};

/*
 * The packet definitions
 */
typedef struct ripentrytype_ {
    ushort family;
    ushort tag;
    ulong address;
    ulong mask;
    ulong nexthop;
    ulong metric;
} ripentrytype;

typedef struct riptype_ {
    uchar opcode;
    uchar version;
    ushort reserved;
    ripentrytype entry[0];
} riptype;

/*
 * This is the exact same packet, but now we've found out we've got
 * authentication information (groan).
 */
#define	RIP_AUTH_ID		0xFFFF		/* can you say overload? */
#define	RIP_AUTH_TYPE_TEXT	2		/* defined in RFC1723 */
#define	RIP_AUTH_TYPE_MD	3		/* keyed message digest */

/*
 * Size of an initial RIP authentication header (this takes up what would
 * have been the first RIP route entry.
 */
#define	RIP_AUTH_HEADER_LEN	(sizeof(ripentrytype))

#define	RIP_AUTH_TEXT_DATA_LEN	16		/* length of text field */
#define	RIP_AUTH_MD5_LEN	16		/* length of actual MD5 MD */

typedef struct ripauthtype_ {
    uchar opcode;
    uchar version;
    ushort reserved;
    ushort authid;		/* if this is 0xFFFF, we've got auth info */
    ushort authtype;
    union {
	uchar authtext[RIP_AUTH_TEXT_DATA_LEN];
	struct {
		ushort pkt_length;
		uchar  key_id;
		uchar  auth_len;
		ulong  sequence;
		ulong  reserved1;
		ulong  reserved2;
	} md;
    } u;
    ripentrytype entry[0];
} ripauthtype;

/*
 * RIP MD5 suffix appended to end of message when keyed MD5 is being used.
 */
typedef struct ripmdsuffixtype_ {
    ushort magic1;
    ushort magic2;
    uchar  digest[RIP_AUTH_MD5_LEN];
} ripmdsuffixtype;

typedef struct ripsendtype_ {
    struct ripsendtype_ *next;
    paktype             *pak;
    int                 bytes;
    ipaddrtype          source;
    ipaddrtype          destination;
    uchar               ttl;
} ripsendtype;

#define RIP_SENDQ_THRESHOLD     16


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern void ip_rip_parser_init(void);
#endif __RIP_H__
