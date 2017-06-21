/* $Id: exec_debug_chat.h,v 3.1 1996/02/01 04:59:36 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ts/exec_debug_chat.h,v $
 *------------------------------------------------------------------
 * Chat scripts debugging support comamnds
 *
 * May' 94, Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * 
 *------------------------------------------------------------------
 * $ Log: $
 *------------------------------------------------------------------
 * $EndLog$
 */

/******************************************************************
 * debug chat
 *
 */
EOLS	(debug_chat_eol, chat_debug_command, DEBUG_CHAT);

TTYLINE (debug_chat_get_line, debug_chat_eol, no_alt,
	OBJ(int,1), "Single TTY line number to debug chat scripts");

KEYWORD (debug_chat_line, debug_chat_get_line, debug_chat_eol,
	 "line", "Single TTY line to debug scripts", PRIV_OPR);

KEYWORD_DEBUG(debug_chat_cmds, debug_chat_line, ALTERNATE,
	      OBJ(pdb,1), chat_debug_arr,
	      "chat", "Chat scripts activity", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_chat_cmds
