/* $Id: smds_actions.h,v 3.2 1995/11/17 18:04:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/smds_actions.h,v $
 *------------------------------------------------------------------
 * Header file for SMDS specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: smds_actions.h,v $
 * Revision 3.2  1995/11/17  18:04:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:19:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *smds_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_smds_addr(parseinfo *, char *, int *, void *,
			int, uint, idbtype *);
