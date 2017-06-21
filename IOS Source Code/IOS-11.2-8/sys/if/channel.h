/* $Id: channel.h,v 3.2 1995/11/17 09:26:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/channel.h,v $
 *------------------------------------------------------------------
 * channel.h -- IBM Channel media definitions
 *
 * March 1994, William H. Palmer 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: channel.h,v $
 * Revision 3.2  1995/11/17  09:26:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CHANNEL_H__
#define __CHANNEL_H__
/*
 * Encapsulation definitions
 */

/*
 * A very simple "encapsulation" is used for packets in MEMD.
 * This encapsulation is identical to the ATM AAL5MUX encapsulation, 
 * namely, 
 *
 *  vci vci 0x08 0x00 <IP header and data>
 */

typedef struct channel_hdr_ {
    ushort	vc;
    ushort	type;
    uchar	pad0[1];
    uchar	media;
    uchar	pad_or_data[2];
    uchar	data[0];
} channel_hdr;

#define CHANNEL_ENCAPBYTES 4
#define CHANNEL_TOKEN_ENCAPBYTES 6
#define CHANNEL_TOKEN_RIF_OFFSET (CHANNEL_RECEIVE_OFFSET+btow(sizeof(channel_hdr)+2*IEEEBYTES))
#define CHANNEL_TOKEN_ACFC_OFFSET (CHANNEL_RECEIVE_OFFSET+btow(sizeof(channel_hdr)-2))
#define CHANNEL_HOLDQ	40	/* default max output queue length */

/*
 * Extern/prototype declarations
 */
extern void channel_parser_init(void);
extern void channel_debug_init(void);
extern void channel_ipaddr_insert(ushort vc, addrtype *addr);
extern void channel_ipaddr_delete(ipaddrtype ipaddr);
#endif				
