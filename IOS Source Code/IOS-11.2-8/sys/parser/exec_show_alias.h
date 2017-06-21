/* $Id: exec_show_alias.h,v 3.2.62.1 1996/08/12 16:06:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_alias.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A L I A S . H
 *
 * Display alias commands
 *
 * October 1994, Robert Widmer
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_alias.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************************
 * show aliases
 *
 */

EOLNS	(show_alias_eol, show_alias_command);
MODE	(show_alias_mode, show_alias_eol, show_alias_eol,
	 pmp, PARSER_MODE_ALIASES);
KEYWORD	(show_alias, show_alias_mode, ALTERNATE,
	 "aliases", "Display alias commands", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_alias
