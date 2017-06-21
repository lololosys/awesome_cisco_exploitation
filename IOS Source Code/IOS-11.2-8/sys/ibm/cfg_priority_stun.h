/* $Id: cfg_priority_stun.h,v 3.2 1995/11/17 09:22:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_priority_stun.h,v $
 *------------------------------------------------------------------
 * cfg_priority_stun.c -- Priority queueing options for STUN
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_stun.h,v $
 * Revision 3.2  1995/11/17  09:22:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:17:18  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_stun_eol, prioritylist_command, FALSE);

/**********************************************************************
 * Args for 'priority-list <n> protocol stun <priority>
 *					[ address <stun-group> <stun-addr> ]
 * The args are parsed here because, unlike the majority of the other
 * variations of this command, the args are after the priority level.
 */

/* [ address <stun-group> <stun-addr> ] */
STUNADDR (cfg_prio_stun_addr, cfg_prio_stun_eol, no_alt,
	  OBJ(paddr,1), "A STUN address");
EVAL	(cfg_prio_stun_group_eval, cfg_prio_stun_addr,
	 csb->stungroup = GETOBJ(int,6));
NUMBER	(cfg_prio_stun_group, cfg_prio_stun_group_eval, no_alt,
	 OBJ(int, 6), 1, 255, "A valid STUN group number");
KEYWORD_ID (cfg_prio_stun_addrkwd, cfg_prio_stun_group, NONE,
	    OBJ(int, 5), PRIORITY_ADDRESS, "address",
	    "Allow specification of a STUN group & address", 
	 PRIV_CONF);

TESTVAR	(cfg_priority_stun, cfg_prio_stun_addrkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_STUN);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_stun
