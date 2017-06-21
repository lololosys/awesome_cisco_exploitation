/* $Id: stun_actions.h,v 3.2 1995/11/17 09:24:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/stun_actions.h,v $
 *------------------------------------------------------------------
 *
 * April 1993, Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: stun_actions.h,v $
 * Revision 3.2  1995/11/17  09:24:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *stun_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_stun_addr(parseinfo *, char *, int *, void *,
			int, uint, idbtype *);
