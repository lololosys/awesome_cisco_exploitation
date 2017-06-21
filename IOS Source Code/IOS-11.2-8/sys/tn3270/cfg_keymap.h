/* $Id: cfg_keymap.h,v 3.3.28.1 1996/08/12 16:08:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/cfg_keymap.h,v $
 *------------------------------------------------------------------
 * C F G _ K E Y M A P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_keymap.h,v $
 * Revision 3.3.28.1  1996/08/12  16:08:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/17  20:18:21  ahh
 * CSCdi47049:  key and keymap commands confused
 * Use KEYWORD_MM for keymap.
 *
 * Revision 3.2  1995/11/17  18:50:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:08:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * keymap <name> <keymap>\
 *               <keymap>\
 *               ...
 * no keymap <name>
 */
EOLNS	(conf_keymap_eol, map3270keys_command);
TEXT	(conf_keymap_termcap, conf_keymap_eol, no_alt,
	 OBJ(string,2), "New keymap definition");
NOPREFIX(conf_keymap_noprefix, conf_keymap_termcap, conf_keymap_eol);
STRING	(conf_keymap_name, conf_keymap_noprefix, no_alt,
	 OBJ(string,1), "New keymap name");

TESTVAR	(conf_map3270keys_nvgen, conf_keymap_name, NONE,
	 NONE, NONE, NONE, nvgen, FALSE);
KEYWORD (conf_map3270keys, conf_map3270keys_nvgen, ALTERNATE,
	 "map3270keys", NULL, PRIV_CONF|PRIV_HIDDEN);

NVGENNS	(conf_keymap_nvgen, conf_keymap_name,
	 map3270keys_command);
KEYWORD_MM(conf_keymap, conf_keymap_nvgen, conf_map3270keys,
	 "keymap", "Define a new keymap", PRIV_CONF, 4);

#undef	ALTERNATE
#define	ALTERNATE	conf_keymap
