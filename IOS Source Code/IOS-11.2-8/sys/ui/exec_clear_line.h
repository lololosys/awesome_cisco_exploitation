/* $Id: exec_clear_line.h,v 3.2.62.1 1996/08/12 16:08:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_clear_line.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ L I N E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_line.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:47:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:23  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:16:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear line <line-number>
 *
 * OBJ(int,1) = <line-number>
 */
EOLNS	(clear_line_eol, zap_line);
TTYLINE	(clear_line_num, clear_line_eol, no_alt,
	 OBJ(int,1), "Line number");
KEYWORD (clear_line, clear_line_num, ALTERNATE,
	 "line", "Reset a terminal line", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_line
