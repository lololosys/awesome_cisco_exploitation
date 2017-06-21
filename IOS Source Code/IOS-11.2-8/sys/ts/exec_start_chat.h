/* $Id: exec_start_chat.h,v 3.1.40.1 1996/08/12 16:08:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_start_chat.h,v $
 *------------------------------------------------------------------
 * E X E C _ S T A R T _ C H A T . H
 *
 * May' 94  Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_start_chat.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  05:14:05  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/14  13:07:51  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:47:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:18:08  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:26:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * start-chat <chat-reg-exp-str> [ line-no [ dial-string ]]
 *
 * OBJ(string,2) = Dial string
 * OBJ(string,1) = script regexp
 * OBJ(int,1) = line to run the script on
 */

EOLNS	(exec_start_chat_eol, start_chat);

PRIV_TEST(exec_start_chat_assert, exec_start_chat_eol, NONE,
	  NONE, PRIV_ROOT);

STRING  (exec_start_chat_dstring, exec_start_chat_assert,
	 exec_start_chat_assert,
	 OBJ(string,2),
	 "Dial string to use for the Chat script");

TTYLINE (exec_start_chat_line, exec_start_chat_dstring,
	 exec_start_chat_assert,
	 OBJ(int,1), "Single TTY line number to run chat script");

STRING	(exec_start_chat_exp, exec_start_chat_line,
	 no_alt, OBJ(string,1),
	 "Chat Script name or regular expression");

KEYWORD	(exec_start_chat, exec_start_chat_exp, ALTERNATE,
	 "start-chat", "Start a chat-script on a line", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_start_chat
