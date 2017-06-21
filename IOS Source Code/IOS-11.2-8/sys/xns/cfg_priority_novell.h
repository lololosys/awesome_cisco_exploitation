/* $Id: cfg_priority_novell.h,v 3.2.62.2 1996/08/16 00:14:12 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_priority_novell.h,v $
 *------------------------------------------------------------------
 * cfg_priority_novell.h -- Novell specific configuration options for priority
 * queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_novell.h,v $
 * Revision 3.2.62.2  1996/08/16  00:14:12  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/30  23:48:00  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:16  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:18:32  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_novell_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> protocol ipx <priority> [list <listnum>]
 *
 * OBJ(int,5) = PRIORITY_LIST
 * OBJ(int,6) = <access-list>
 */

ACCESS_LIST(cfg_prio_novell_novelllistno, cfg_prio_novell_eol, no_alt,
	 OBJ(int,6), MINNOVACC, MAXENOVACC,
	 IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL, "IPX access list");

KEYWORD_ID (cfg_prio_novell_listkwd, cfg_prio_novell_novelllistno, NONE,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_novell, cfg_prio_novell_listkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_NOVELL);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_novell
