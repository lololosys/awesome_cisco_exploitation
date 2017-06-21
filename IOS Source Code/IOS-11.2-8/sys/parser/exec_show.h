/* $Id: exec_show.h,v 3.4.46.1 1996/08/12 16:06:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show.h,v $
 * Revision 3.4.46.1  1996/08/12  16:06:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1995/12/14  13:07:48  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.3  1995/11/28  19:50:51  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  18:46:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:21:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************/
EOLNS	(exec_show_help_eol, show_help_command);
PRIV_TEST(exec_show_help, exec_show_help_eol, NONE,
	  exec_show_commands, PRIV_MIN | PRIV_HIDDEN);

KEYWORD	(exec_show, exec_show_help, ALTERNATE,
	 "show", "Show running system information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_show
