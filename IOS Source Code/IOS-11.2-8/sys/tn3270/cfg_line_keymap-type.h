/* $Id: cfg_line_keymap-type.h,v 3.2.62.1 1996/08/12 16:08:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/cfg_line_keymap-type.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ K E Y M A P - T Y P E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_keymap-type.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:50:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:05  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:09:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * keymap-type <keymap-name>
 *
 * OBJ(int,1) = <keymap-name>
 */

EOLNS	(cfg_line_keymaptype_eol, keymaptype_command);
TEXT	(cfg_line_keymaptype_name, cfg_line_keymaptype_eol, no_alt,
	 OBJ(string,1), "Name of a keymap entry");
NOPREFIX (cfg_line_keymaptype_noprefix, cfg_line_keymaptype_name,
	  cfg_line_keymaptype_eol);
NVGENNS	(cfg_line_keymaptype_nvgen, cfg_line_keymaptype_noprefix,
	 keymaptype_command);

/*
 * Match at least 7 (keymap-) characters to keep from matching
 * the keymap global config command.
 */
KEYWORD_MM(cfg_line_keymaptype, cfg_line_keymaptype_nvgen, ALTERNATE,
	   "keymap-type", "Specify a keymap entry to use", PRIV_USER, 7);

#undef	ALTERNATE
#define	ALTERNATE	cfg_line_keymaptype
