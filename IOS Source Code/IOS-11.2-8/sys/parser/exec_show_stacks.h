/* $Id: exec_show_stacks.h,v 3.5.20.1 1996/08/12 16:06:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_stacks.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S T A C K . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_stacks.h,v $
 * Revision 3.5.20.1  1996/08/12  16:06:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5  1996/02/01  23:34:19  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.4  1996/01/29  07:09:27  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/14  13:07:50  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:47:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:26:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show stacks [<frame-count>]
 *
 * OBJ(int,1) =	<frame-count>
 *		0 = no <frame-count>
 */
EOLNS	(show_stacks_eol, show_stacks);
NUMBER	(show_stacks_frames, show_stacks_eol, NONE,
	 OBJ(int,1), 1, 8192,
	 "Process to show stack detail on");
PRIV_TEST(show_stacks_priv, show_stacks_frames, NONE,
	  show_stacks_eol, PRIV_ROOT);
KEYWORD (show_stacks_kwd, show_stacks_priv, ALTERNATE,
	 "stacks", "Process stack utilization", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_stacks_kwd
