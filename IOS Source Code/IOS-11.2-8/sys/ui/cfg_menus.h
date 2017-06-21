/* $Id: cfg_menus.h,v 3.1.40.2 1996/08/12 16:08:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/cfg_menus.h,v $
 *------------------------------------------------------------------
 * cfg_menu.h
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_menus.h,v $
 * Revision 3.1.40.2  1996/08/12  16:08:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/06/04  00:23:53  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * singlespace -> single-space
 *
 * Revision 3.1  1996/02/01  05:14:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1995/12/19  21:01:40  billw
 * CSCdi44869:  default behavior for a <CR> at a menu prompt should be
 * adjustable
 * Code review - add comment!
 *
 * Revision 3.3  1995/12/15  23:05:12  billw
 * CSCdi44869:  default behavior for a <CR> at a menu prompt should be
 * adjustable
 * add "menu <foo> default <n>"
 *
 * Revision 3.2  1995/11/17  18:44:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:10  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * menu <name> title d <string> d
 * menu <name> item|command <n> <string>
 * menu <name> clear-screen
 * menu <name> single-space
 * menu <name> line-mode
 * menu <name> status-line
 * menu <name> default <item>
 * no menu <name>
 *
 * OBJ(string,2) = <name>
 * OBJ(int,1) = MENU_TITLE | MENU_ITEM | MENU_COMMAND
 * OBJ(int,2) = <n>
 * OBJ(string,1) = <string>
 *
 * Note non-intuitive use of strings.  This is because title uses banner(),
 * which assumes it's building (string,1)
 */

EOLNS	(cfg_menu_eol, menu_cfgcommand);
EOLNS	(cfg_menu_clreol, menu_cfgcommand);
EOLNS	(cfg_menu_cmdeol, menu_cfgcommand);
EOLNS	(cfg_menu_txteol, menu_cfgcommand);

TXT_LINE(cfg_menu_cmdtxt, cfg_menu_cmdeol, no_alt, OBJ(string,1));

TXT_LINE(cfg_menu_txttxt, cfg_menu_txteol, no_alt, OBJ(string,1));

NUMBER (cfg_menu_cmdrest, cfg_menu_cmdtxt, no_alt,
	OBJ(int,2), 1, 18, "Menu item number");

NUMBER (cfg_menu_resttext, cfg_menu_txttxt, no_alt,
	OBJ(int,2), 1, 18, "Menu item number");

NUMBER (cfg_menu_getdef, cfg_menu_eol, no_alt,
	OBJ(int,2), 0, 18, "Menu item number");

TEXT (cfg_menu_clrtxt, cfg_menu_clreol, no_alt,
      OBJ(string,1), "delimited text of title");


KEYWORD_ID (cfg_menu_status, cfg_menu_eol, no_alt,
	    OBJ(int, 1), MENU_STATUSLINE,
	    "status-line", "Display user status at top of screen", PRIV_CONF);
KEYWORD_ID (cfg_menu_singlesp, cfg_menu_eol, cfg_menu_status,
	    OBJ(int, 1), MENU_SINGLESPACE,
	    "single-space", "single-space menu entries on display", PRIV_CONF);
KEYWORD_ID (cfg_menu_linemode, cfg_menu_eol, cfg_menu_singlesp,
	    OBJ(int, 1), MENU_LINEMODE,
	    "line-mode", "require <enter> after typing selection", PRIV_CONF);
KEYWORD_ID (cfg_menu_default, cfg_menu_getdef, cfg_menu_linemode,
	    OBJ(int, 1), MENU_DEFAULT,
	    "default", "Item number to use for RETURN", PRIV_CONF);
KEYWORD_ID (cfg_menu_clear, cfg_menu_eol, cfg_menu_default,
	    OBJ(int, 1), MENU_CLEAR,
	    "clear-screen", "Use termcap database to clear screen", PRIV_CONF);
KEYWORD_ID (cfg_menu_command, cfg_menu_cmdrest, cfg_menu_clear,
	    OBJ(int,1), MENU_COMMAND,
	    "command", "Set menu command", PRIV_CONF);
KEYWORD_ID (cfg_menu_text, cfg_menu_resttext, cfg_menu_command,
	    OBJ(int,1), MENU_TEXT,
	    "text", "Set text of menu line", PRIV_CONF);
KEYWORD_ID (cfg_menu_title, cfg_menu_clrtxt, cfg_menu_text,
	    OBJ(int,1), MENU_TITLE,
	    "title", "Set menu title", PRIV_CONF);
NOPREFIX (cfg_menu_noprefix, cfg_menu_title, cfg_menu_eol);
STRING	(cfg_menu_name, cfg_menu_noprefix, no_alt,
	 OBJ(string,2), "Name of this menu");
NVGENNS	(cfg_menu_nvgen, cfg_menu_name, menu_cfgcommand);
KEYWORD	(cfg_menu_kwd, cfg_menu_nvgen, ALTERNATE,
	 "menu", "Define a user-interface menu", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_menu_kwd
