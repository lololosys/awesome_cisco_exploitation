/* $Id: exec_show_keymap.h,v 3.4.20.1 1996/08/12 16:08:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/exec_show_keymap.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ K E Y M A P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_keymap.h,v $
 * Revision 3.4.20.1  1996/08/12  16:08:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:34:39  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:09:56  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:50:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:24:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show keymap [{ all | <keymap-name> }]
 * 
 * OBJ(int,1) =	TRUE = Keyword 'all' was parsed
 *		FALSE = No keyword 'all' was seen
 * OBJ(int,2) =	TRUE = A keymap-name was given
 * 		FALSE = No keymap-name was given
 * OBJ(string,1) = <keymap-name>
 */
EOLNS	(show_keymap_eol, show_keymap);
SET	(show_keymap_set, show_keymap_eol, OBJ(int,2), TRUE);
STRING	(show_keymap_name, show_keymap_set, NONE,
	OBJ(string,1), "Keymap name to display");
TEST_MULTIPLE_FUNCS(show_keymap_test, show_keymap_name, NONE, show_keymap_eol);
KEYWORD_ID (show_keymap_all, show_keymap_eol, show_keymap_test,
	OBJ(int,1), TRUE,
	"all", "All keymaps", PRIV_USER);
KEYWORD (show_keymap, show_keymap_all, ALTERNATE,
	 "keymap", "Terminal keyboard mappings", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_keymap
