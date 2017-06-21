/* $Id: at_actions.h,v 3.2 1995/11/17 08:41:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_actions.h,v $
 *------------------------------------------------------------------
 * Header file for AppleTalk specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_actions.h,v $
 * Revision 3.2  1995/11/17  08:41:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *apple_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_apple_addr(parseinfo *, char *, int *, void *,
			 int, uint, idbtype *);
