/* $Id: ip_debug.h,v 3.2.60.2 1996/05/22 20:54:58 raj Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_debug.h,v $
 *------------------------------------------------------------------
 * ip_debug.h -- Debugging declarations for General IP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_debug.h,v $
 * Revision 3.2.60.2  1996/05/22  20:54:58  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.2.60.1  1996/04/16  18:53:06  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:33:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:35:37  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  20:57:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up IP debugging */
extern void ip_debug_init(void);

/* Parser function for IP debugging commands */
extern void ip_debug_command(parseinfo *csb);

/* IP packet debugging can be restricted by an access list */
extern int ipdebug_acl;

/* Detailed IP packet debugging */
extern boolean ipdebug_detail;
extern boolean ipdebug_dump;

/* IP error debugging can be restricted by an access list */
extern int iperror_acl;

/* Detailed IP error debugging */
extern boolean iperror_detail;
extern boolean iperror_dump;

/* IP packet debugging can be restricted by an access list */
extern int ipmdebug_acl;

/* Detailed IP packet debugging */
extern boolean ipmdebug_detail;

/*
 * Used for selective debugging of multicast packets.
 */
extern ipaddrtype ipmpkt_group_debug;

/* Debug IP peer address pooling */
extern boolean peer_debug;

/*
 * The actual debugging flags are defined in ip_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ip/ip_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ip/ip_debug_flags.h"
