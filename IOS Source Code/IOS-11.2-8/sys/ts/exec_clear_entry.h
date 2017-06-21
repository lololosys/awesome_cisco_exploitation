/* $Id: exec_clear_entry.h,v 3.3.20.1 1996/08/12 16:08:29 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_clear_entry.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ E N T R Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_entry.h,v $
 * Revision 3.3.20.1  1996/08/12  16:08:29  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:59  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:54:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:54  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:15:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear entry <entry-number>
 *
 * OBJ(int,1) = <entry-number>
 */
EOLNS	(clear_entry_eol, delete_termqueue);
NUMBER	(clear_entry_num, clear_entry_eol, no_alt,
	 OBJ(int,1), 0, 256, "Queue entry number");
KEYWORD (clear_entry_kwd, clear_entry_num, NONE,
	 "entry", "Clear a terminal queue entry", PRIV_OPR);

ASSERT	(clear_entry, clear_entry_kwd, ALTERNATE,
	 termqueue_queuehead);

#undef	ALTERNATE
#define	ALTERNATE	clear_entry
