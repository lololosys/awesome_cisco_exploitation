/* $Id: cfg_priority_vines.h,v 3.2 1995/11/17 17:57:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/cfg_priority_vines.h,v $
 *------------------------------------------------------------------
 * cfg_priority_vines.h -- Vines specific configuration options for priority
 * queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_vines.h,v $
 * Revision 3.2  1995/11/17  17:57:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:19  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_vines_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> vines <priority> [list <listnum>]
 *
 * OBJ(int,5) = PRIORITY_LIST
 * OBJ(int,6) = <access-list>
 */

NUMBER	(cfg_prio_vines_vineslistno, cfg_prio_vines_eol, no_alt,
	 OBJ(int, 6), VINESMINACC, VINESMAXACC, "Vines access list");

KEYWORD_ID (cfg_prio_vines_listkwd, cfg_prio_vines_vineslistno, NONE,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_vines, cfg_prio_vines_listkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_VINES);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_vines
