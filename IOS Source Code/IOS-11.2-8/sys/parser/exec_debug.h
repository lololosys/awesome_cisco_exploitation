/* $Id: exec_debug.h,v 3.4.46.1 1996/08/12 16:06:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_debug.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug.h,v $
 * Revision 3.4.46.1  1996/08/12  16:06:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1995/12/14  13:07:45  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.3  1995/11/28  19:50:49  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  18:45:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:47:00  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************/
EOLNS	(exec_debug_help_eol, debug_help_command);
PRIV_TEST(exec_debug_help, exec_debug_help_eol, NONE,
	  exec_debug_commands, PRIV_MIN | PRIV_HIDDEN);

KEYWORD_ID(exec_debug_false, exec_debug_help, no_alt,
	   sense, FALSE,
	   "debug", "Disable debugging functions (see also 'undebug')",
	   PRIV_OPR);
KEYWORD	(exec_debug_no, exec_debug_false, ALTERNATE,
	 "no", "Disable debugging functions", PRIV_OPR);

KEYWORD_ID(exec_debug_true, exec_debug_help, exec_debug_no,
	   sense, TRUE,
	   "debug", "Debugging functions (see also 'undebug')", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_debug_true
