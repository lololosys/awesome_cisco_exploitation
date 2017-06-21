/* $Id: novell_actions.h,v 3.2 1995/11/17 19:23:00 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/novell_actions.h,v $
 *------------------------------------------------------------------
 * Header file for Novell specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_actions.h,v $
 * Revision 3.2  1995/11/17  19:23:00  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:28:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *novell_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_nov_addr(parseinfo *, char *, int *, void *,
		       int, uint, idbtype *);
void parser_novell_router(parseinfo *, const char *, char *, transition *);
