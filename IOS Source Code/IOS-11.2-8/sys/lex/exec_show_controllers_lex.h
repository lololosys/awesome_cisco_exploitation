/* $Id: exec_show_controllers_lex.h,v 3.2.62.1 1996/08/12 16:04:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lex/exec_show_controllers_lex.h,v $
 *------------------------------------------------------------------
 * exec_show_controllers_lex.h - LEX show controller command
 *
 * May 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_lex.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:42:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:32  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:53  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers lex [<unit number>]
 *
 */
EOLNS	(show_control_lex_eol, show_controllers);

UNIT_NUMBER(show_control_lex_unit, show_control_lex_eol, show_control_lex_eol,
	    OBJ(idb,1),  IFTYPE_LEX);

KEYWORD_ID(show_control_lex_kwd, show_control_lex_unit, NONE,
	   OBJ(int,1), IDB_LEX,
	   "lex", "Show Lex controllers", PRIV_USER);

TEST_FUNC(show_control_lex, show_control_lex_kwd, NONE, ALTERNATE,
	  is_lex_configured);

#undef	ALTERNATE
#define	ALTERNATE show_control_lex
