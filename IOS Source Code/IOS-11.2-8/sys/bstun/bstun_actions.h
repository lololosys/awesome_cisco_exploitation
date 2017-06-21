/* $Id: bstun_actions.h,v 3.2 1995/11/17 08:48:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bstun_actions.h,v $
 *------------------------------------------------------------------
 * bstun_actions.h - BSTUN address matching prototypes
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_actions.h,v $
 * Revision 3.2  1995/11/17  08:48:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _BSTUN_ACTIONS_H_
#define _BSTUN_ACTIONS_H_

extern char *bstun_addr_short_help(parseinfo *, uint, idbtype *);
extern boolean match_bstun_addr(parseinfo *, char *, int *, void *,
			int, uint, idbtype *);

#endif
