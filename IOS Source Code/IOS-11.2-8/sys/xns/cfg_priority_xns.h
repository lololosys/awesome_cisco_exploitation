/* $Id: cfg_priority_xns.h,v 3.2.62.1 1996/05/30 23:48:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_priority_xns.h,v $
 *------------------------------------------------------------------
 * cfg_priority_xns.h -- Xns specific configuration options for priority
 * queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_xns.h,v $
 * Revision 3.2.62.1  1996/05/30  23:48:02  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:18  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:20  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_xns_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> xns <priority> [list <listnum>]
 *
 * OBJ(int,5) = PRIORITY_LIST
 * OBJ(int,6) = <access-list>
 */

ACCESS_LIST(cfg_prio_xns_xnslistno, cfg_prio_xns_eol, no_alt,
	 OBJ(int, 6), MINXNSACC, MAXEXNSACC, XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
	 "XNS access list");

KEYWORD_ID (cfg_prio_xns_listkwd, cfg_prio_xns_xnslistno, NONE,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_xns, cfg_prio_xns_listkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_XNS);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_xns
