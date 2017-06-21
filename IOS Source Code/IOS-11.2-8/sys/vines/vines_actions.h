/* $Id: vines_actions.h,v 3.2 1995/11/17 17:57:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vines_actions.h,v $
 *------------------------------------------------------------------
 * Header file for Vines specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vines_actions.h,v $
 * Revision 3.2  1995/11/17  17:57:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *vines_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_vines_addr(parseinfo *, char *, int *, void *,
			 int, uint, idbtype *);
