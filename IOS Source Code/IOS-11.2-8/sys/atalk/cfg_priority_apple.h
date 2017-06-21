/* $Id: cfg_priority_apple.h,v 3.2.60.1 1996/05/30 23:42:11 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cfg_priority_apple.h,v $
 *------------------------------------------------------------------
 * cfg_priority_apple.h -- AppleTalk specific configuration options for
 * priority queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_apple.h,v $
 * Revision 3.2.60.1  1996/05/30  23:42:11  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:44:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:13  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_apple_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> apple <priority> [list <listnum>]
 *
 * OBJ(int,5) = PRIORITY_LIST
 * OBJ(int,6) = <access-list>
 */

ACCESS_LIST(cfg_prio_apple_applelistno, cfg_prio_apple_eol, no_alt,
	 OBJ(int, 6), MINATALKACC, MAXATALKACC,
	 AT_MIN_ACL, AT_MAX_ACL, "AppleTalk access list");

KEYWORD_ID (cfg_prio_apple_listkwd, cfg_prio_apple_applelistno, NONE,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_apple, cfg_prio_apple_listkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_APPLETALK);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_apple
