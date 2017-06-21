/* $Id: clns_actions.h,v 3.2 1995/11/17 08:51:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_actions.h,v $
 *------------------------------------------------------------------
 * Header file for CLNS specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clns_actions.h,v $
 * Revision 3.2  1995/11/17  08:51:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:16:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *clns_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_clns_addr(parseinfo *, char *, int *, void *,
			int, uint, idbtype *);
void parser_clns_router(parseinfo *, const char *, char *, transition *);
