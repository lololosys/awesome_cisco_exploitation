/* $Id: cfg_termqueue.h,v 3.2.62.1 1996/08/12 16:08:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_termqueue.h,v $
 *------------------------------------------------------------------
 * CFG_TERMQUEUE.H
 *
 * Terminal Queue Command Support
 *
 * March 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_termqueue.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:54:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:53  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * terminal-queue entry-retry-interval <num>
 * no terminal-queue entry-retry-interval
 */
PARAMS_KEYONLY(termqueue_interval, no_alt, "entry-retry-interval",
	       OBJ(int,1), 0, 255,
	       termqueue_interval_command, 0,
	       "Set the terminal queue polling interval",
	       "Interval in seconds", PRIV_CONF);
KEYWORD (cfg_termqueue, termqueue_interval, ALTERNATE, "terminal-queue",
	 "Terminal queue commands", PRIV_CONF);


#undef  ALTERNATE
#define ALTERNATE       cfg_termqueue
