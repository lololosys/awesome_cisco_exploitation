/* $Id: exec_show_rsp.h,v 3.2.62.1 1996/08/12 16:07:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/exec_show_rsp.h,v $
 *------------------------------------------------------------------
 * parse chain for RSP specific show commands
 *
 * Octobert 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_rsp.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  08:21:48  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/07/04  01:57:40  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show rsp
 */

EOLNS	(show_rsp_eol, show_rsp_command);

KEYWORD_MM(show_rsp, show_rsp_eol, ALTERNATE,
	   "rsp", NULL /* NOHELP */, PRIV_NOHELP, 3);

#undef	ALTERNATE
#define	ALTERNATE	show_rsp
