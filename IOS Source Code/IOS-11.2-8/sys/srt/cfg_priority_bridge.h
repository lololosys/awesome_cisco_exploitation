/* $Id: cfg_priority_bridge.h,v 3.2.62.1 1996/05/30 23:45:44 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_priority_bridge.h,v $
 *------------------------------------------------------------------
 * cfg_priority_bridge.h -- Bridge specific configuration options for priority
 * queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_bridge.h,v $
 * Revision 3.2.62.1  1996/05/30  23:45:44  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:53:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:59  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:14  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_bridge_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> bridge <priority> [list <listnum>]
 *
 * OBJ(int,5) = PRIORITY_LIST
 * OBJ(int,6) = <access-list>
 */

ACCESS_LIST(cfg_prio_bridge_bridgelistno, cfg_prio_bridge_eol, no_alt,
	 OBJ(int,6), MINTYPEACC, MAXTYPEACC, TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	 "Ethernet-typecode access list");

KEYWORD_ID (cfg_prio_bridge_listkwd, cfg_prio_bridge_bridgelistno, NONE,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_bridge, cfg_prio_bridge_listkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_BRIDGE);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_bridge
