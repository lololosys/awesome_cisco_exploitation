/* $Id: snapshot_actions.h,v 3.2 1995/11/17 18:04:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/snapshot_actions.h,v $
 *------------------------------------------------------------------
 * Header file for Snapshot specific parsing functions.
 *
 * June 1994,  Joel P. Bion
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snapshot_actions.h,v $
 * Revision 3.2  1995/11/17  18:04:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _SNAPSHOT_ACTIONS_H_
#define _SNAPSHOT_ACTIONS_H_
char *snapshot_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_snapshot_addr(parseinfo *, char *, int *, void *,
			    int, uint, idbtype *);

#endif
