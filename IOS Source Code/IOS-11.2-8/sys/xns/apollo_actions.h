/* $Id: apollo_actions.h,v 3.2 1995/11/17 19:20:31 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/apollo_actions.h,v $
 *------------------------------------------------------------------
 * Header file for Apollo specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: apollo_actions.h,v $
 * Revision 3.2  1995/11/17  19:20:31  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *apollo_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_apollo_addr(parseinfo *, char *, int *, void *,
			  int, uint, idbtype *);
