/* $Id: cfg_chat.h,v 3.1.40.1 1996/08/12 16:08:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_chat.h,v $
 *------------------------------------------------------------------
 * C F G _ C H A T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_chat.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:59:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:42:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:02:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * chat <name> <script>
 * no chat <name>
 *
 */

EOLNS	(cfg_chat_eol, chat_script_command);
TEXT	(cfg_chat_script, cfg_chat_eol, no_alt,
	 OBJ(string,2), "Chat script in form EXPECT SEND EXPECT SEND ...");
NOPREFIX(cfg_chat_noprefix, cfg_chat_script, cfg_chat_eol);
STRING	(cfg_chat_name, cfg_chat_noprefix, no_alt,
	 OBJ(string,1), "Name of chat script");
NVGENNS	(cfg_chat_nvgen, cfg_chat_name, chat_script_command);
KEYWORD	(cfg_chat, cfg_chat_nvgen, ALTERNATE,
	 "chat-script", "Define a modem chat script", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_chat
