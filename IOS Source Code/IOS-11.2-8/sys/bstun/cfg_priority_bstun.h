/* $Id: cfg_priority_bstun.h,v 3.2 1995/11/17 08:49:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/cfg_priority_bstun.h,v $
 *------------------------------------------------------------------
 * cfg_priority_bstun.h -- Priority queueing options for BSTUN
 *
 * March 1995, Paul Pearce
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_bstun.h,v $
 * Revision 3.2  1995/11/17  08:49:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/21  18:17:15  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_bstun_eol, prioritylist_command, FALSE);

/**********************************************************************
 * Args for 'priority-list <n> protocol bstun <priority>
 *					[ address <bstun-group> <bsc-addr> ]
 * The args are parsed here because, unlike the majority of the other
 * variations of this command, the args are after the priority level.
 */

/* [ address <bstun-group> <bsc-addr> ] */
BSTUNADDR (cfg_prio_bstun_addr, cfg_prio_bstun_eol, no_alt,
	  OBJ(paddr,1), "BSC address");
EVAL	(cfg_prio_bstun_group_eval, cfg_prio_bstun_addr,
	 csb->bstungroup = GETOBJ(int,6));
NUMBER	(cfg_prio_bstun_group, cfg_prio_bstun_group_eval, no_alt,
	 OBJ(int, 6), 1, 255, "Valid BSTUN group number");
KEYWORD_ID (cfg_prio_bstun_addrkwd, cfg_prio_bstun_group, NONE,
	    OBJ(int, 5), PRIORITY_ADDRESS, "address",
	    "Allow specification of a BSTUN group & BSC address", 
	 PRIV_CONF);

TESTVAR	(cfg_priority_bstun, cfg_prio_bstun_addrkwd, NONE, NONE, NONE,
	 ALTERNATE, OBJ(int,3), LINK_BSTUN);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_bstun
