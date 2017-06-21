/* $Id: cfg_ttycap.h,v 3.2.62.1 1996/08/12 16:08:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/cfg_ttycap.h,v $
 *------------------------------------------------------------------
 * C F G _ T T Y C A P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ttycap.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:50:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * ttycap <name> <ttycap>\
 *               <ttycap>\
 *               ...
 * no ttycap <name>
 */
EOLNS	(conf_ttycap_eol, termcap_command);
TEXT	(conf_ttycap_termcap, conf_ttycap_eol, no_alt,
	 OBJ(string,2), "New termcap definition");
NOPREFIX(conf_ttycap_noprefix, conf_ttycap_termcap, conf_ttycap_eol);
STRING	(conf_ttycap_name, conf_ttycap_noprefix, no_alt,
	 OBJ(string,1), "New termcap name");
NVGENNS	(conf_ttycap_nvgen, conf_ttycap_name,
	 termcap_command);
KEYWORD	(conf_ttycap, conf_ttycap_nvgen, ALTERNATE,
	 "ttycap", "Define a new termcap", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_ttycap
