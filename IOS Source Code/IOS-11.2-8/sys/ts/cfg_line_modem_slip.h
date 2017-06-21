/* $Id: cfg_line_modem_slip.h,v 3.2.62.1 1996/08/12 16:08:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_modem_slip.h,v $
 *------------------------------------------------------------------
 * SLIP modem commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_modem_slip.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:48  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:10:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] modem chat-script <string>
 *
 */
EOLNS	(modem_chat_eol, chat_modem_command);
STRING	(modem_chat_regexp, modem_chat_eol, no_alt,
	 OBJ(string,1),
	 "Regular expression to select chat scripts on this line");
NOPREFIX(modem_chat_noprefix, modem_chat_regexp, modem_chat_eol);
NVGENNS	(modem_chat_nvgen, modem_chat_noprefix,
	 chat_modem_command);
KEYWORD_ID(modem_chat, modem_chat_nvgen, ALTERNATE,
	   OBJ(int,10), PARSER_MODEM_CHAT,
	   "chat-script", 
	   "Specify a regular expression for chat script selection",
	   PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	modem_chat
