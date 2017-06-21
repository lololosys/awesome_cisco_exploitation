/* $Id: cfg_alias.h,v 3.2.62.2 1996/08/12 16:05:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_alias.h,v $
 *------------------------------------------------------------------
 * C F G _ A L I A S . H
 *
 * Alias configuration command
 *
 * October 1994, Robert Widmer
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_alias.h,v $
 * Revision 3.2.62.2  1996/08/12  16:05:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/17  23:35:35  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:42:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************************
 * alias <mode> <name> <alias>
 * no alias <mode> [<name>]
 *
 */
EOLNS	(cfg_alias_eol, parser_alias_command);
TEXT	(cfg_alias_alias, cfg_alias_eol, no_alt,
	 OBJ(string,2), "New alias");
NOPREFIX(cfg_alias_noprefix, cfg_alias_alias, cfg_alias_eol);
STRING	(cfg_alias_name, cfg_alias_noprefix, no_alt,
	 OBJ(string,1), "Alias name");
TESTVAR	(cfg_alias_test, cfg_alias_eol, NONE,
	 NONE, NONE, cfg_alias_name, sense, FALSE);
NVGENNS	(cfg_alias_nvgen, cfg_alias_test, parser_alias_command);
MODE	(cfg_alias_mode, cfg_alias_nvgen, no_alt,
	 pmp, PARSER_MODE_ALIASES);
KEYWORD	(cfg_alias, cfg_alias_mode, ALTERNATE,
	 "alias", "Create command alias", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	cfg_alias
