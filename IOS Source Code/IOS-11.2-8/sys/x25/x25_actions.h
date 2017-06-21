/* $Id: x25_actions.h,v 3.2 1995/11/17 18:07:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_actions.h,v $
 *------------------------------------------------------------------
 * Header file for X25 specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_actions.h,v $
 * Revision 3.2  1995/11/17  18:07:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *x121_addr_short_help(parseinfo *, uint, idbtype *);
char *x121_hwaddr_short_help(parseinfo *, uint, idbtype *);
boolean match_x121_addr(parseinfo *, char *, int *, void *,
			int, uint, idbtype *);
boolean match_x121_hwaddr(parseinfo *, char *, int *, void *,
			  int, uint, idbtype *);
