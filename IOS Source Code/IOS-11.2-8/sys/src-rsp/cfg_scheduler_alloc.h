/* $Id: cfg_scheduler_alloc.h,v 3.2.62.1 1996/08/12 16:07:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/cfg_scheduler_alloc.h,v $
 *------------------------------------------------------------------
 * "scheduler allocate" parse chain
 *
 * February 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_scheduler_alloc.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:39  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:13:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * no scheduler allocate
 * scheduler allocate  [ <network> <process> ]
 *
 *	OBJ(int,1) = microseconds for network interrupt handler
 *	OBJ(int,2) = microseconds with network interrupts masked
 */

EOLNS	(sched_alloc_eol, sched_alloc_command);

NUMBER	(sched_alloc_proc, sched_alloc_eol, NONE,
	 OBJ(int,2), NETINT_MASK_MIN, NETINT_MASK_MAX,
	 "Microseconds running processes");

NUMBER	(sched_alloc_net, sched_alloc_proc, sched_alloc_eol,
	 OBJ(int,1), NETINT_MIN, NETINT_MAX,
	 "Microseconds handling network interrupts");

NOPREFIX(sched_alloc_no, sched_alloc_net, sched_alloc_eol);

NVGENNS	(sched_alloc_nv, sched_alloc_no, sched_alloc_command);

KEYWORD	(cfg_sched_alloc, sched_alloc_nv, ALTERNATE,
	 "allocate", "Guarantee CPU time for processes", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_sched_alloc
