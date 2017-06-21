/* $Id: ipv6.h,v 3.2 1995/11/17 17:37:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipv6/ipv6.h,v $
 *------------------------------------------------------------------
 * ipv6.h -- Definitions for IPv6
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipv6.h,v $
 * Revision 3.2  1995/11/17  17:37:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:11:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:11:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef long long ipv6addrtype;

typedef struct ipv6hdrtype_ {
    unsigned version: 4;		/* version number */
    unsigned flow: 24;			/* flow label */
    ushort length;			/* payload lenth */
    uchar next_header;			/* next header type */
    uchar hop;				/* hop limit */
    ipv6addrtype srcaddr;		/* source address */
    ipv6addrtype dstaddr;		/* destination address */
} ipv6hdrtype;

#define IPV6VERSION		6	/* version 6 */
#define IPV6HEADERBYTES 	(sizeof(ipv6hdrtype))
#define IPV6_DEFAULT_HOPS	255	/* default hop count */
#define IPV6PACKETLIMIT		65535	/* max size packet */

enum IPV6_OPTIONS {
    IPV6_OPTION_PAD1 = 0,		/* single byte pad */
    IPV6_OPTION_PAD2 = 1,		/* multi-byte pad */
};

enum IPV6_HEADERS {
    IPV6_HEADER_HOP = 0,		/* hop by hop options */
    IPV6_HEADER_ROUTING = 43,		/* routing header */
    IPV6_HEADER_FRAGMENT = 44,		/* fragment header */
    IPV6_HEADER_AUTHENTICATION = 51,	/* authentication info */
    IPV6_HEADER_END,			/* end to end options, value tbd */
    IPV6_HEADER_IPV6 = 41,		/* tunneling -- strange */
};
