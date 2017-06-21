/* $Id: caps.h,v 3.2 1995/11/17 07:40:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/caps.h,v $
 *------------------------------------------------------------------
 * caps.h -- Definitions for Capability list EMT calls
 *
 * August 1989, Chris Shaker
 *
 * Copyright (c) 1989-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: caps.h,v $
 * Revision 3.2  1995/11/17  07:40:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Current length of data[]:
 */
#define SYSCAP_LENGTH	2

struct capabilities {
    u_short version;		 /* version number for caps.h */
    u_short length;		 /* length of data[] (# of shorts) */
    u_short data[SYSCAP_LENGTH]; /* capabilities list */
};

/*
 * Current version number for capabilities bit list. Bump this number
 * whenever the meaning of a bit (or the format) changes.
 */
#define SYSCAP_VERSION	1

/*
 * Capability bits for data[0]. Note that the name of each bit includes
 * the version number of the capability list in which it first appeared:
 */
#define SYSCAP_1_BRIDGE	0x0001
#define SYSCAP_1_DDN	0x0002
#define SYSCAP_1_X25	0x0004
#define SYSCAP_1_SRBX	0x0008		/* Don't use anymore ! */
#define SYSCAP_1_LAT	0x0010
#define SYSCAP_1_PROTO  0x0020
