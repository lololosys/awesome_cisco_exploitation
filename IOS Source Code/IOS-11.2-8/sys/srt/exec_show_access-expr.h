/* $Id: exec_show_access-expr.h,v 3.2.62.2 1996/08/12 16:07:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_show_access-expr.h,v $
 *------------------------------------------------------------------
 * Access Expression show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_access-expr.h,v $
 * Revision 3.2.62.2  1996/08/12  16:07:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/05  18:57:28  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:53:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:10  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show access-expression
 *
 */
EOLNS	(show_access_expr_expr_eol, show_access_expression);
KEYWORD (show_access_expr, show_access_expr_expr_eol, NONE,
	 "access-expression", "List access expression",
	 PRIV_USER | PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_access_expr_test, show_access_expr, NONE, ALTERNATE,
	     bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_access_expr_test
