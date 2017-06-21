/* $Id: ip_localpool.h,v 3.3 1996/01/03 20:33:12 vandys Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ip/ip_localpool.h,v $
 *------------------------------------------------------------------
 * IP local address pool handling definitions
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_localpool.h,v $
 * Revision 3.3  1996/01/03  20:33:12  vandys
 * CSCdi43813:  IP local pools too small for max VTY-Async configs
 *
 * Revision 3.2  1995/11/17  09:33:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:35:46  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/06/20  18:49:13  irfan
 * CSCdi34969:  IP pooling gives addresses that are already in use.
 * Flag a member address in in-use state, as to-be-deleted, when the
 * parent scope is killed. Delete such orphan addresses when released
 * by the user code. Perform additional sanity checks while creating
 * new scopes.
 *
 * Revision 2.1  1995/06/07  20:57:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IP_LOCALP_H__
#define __IP_LOCALP_H__

/*
 * Sanity bound constant
 */
#define MAX_POOL_ADDRS (1024)

/*
 * routines available to others
 */
extern void ip_localpool_command(parseinfo *);


#endif __IP_LOCALP_H__
