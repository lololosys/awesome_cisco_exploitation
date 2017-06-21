/* $Id: cfg_banner.h,v 3.2 1995/11/17 18:42:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_banner.h,v $
 *------------------------------------------------------------------
 * C F G _ B A N N E R . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_banner.h,v $
 * Revision 3.2  1995/11/17  18:42:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * banner { exec | incoming | login | motd } <char> <text> <char>
 * no banner { exec | incoming | login | motd }
 *
 * OBJ(int,1) = BANNER_EXEC, BANNER_INCOMING, BANNER_MOTD, BANNER_LOGIN
 * OBJ(string,1) = <char> <text> <char>
 */
EOLS	(conf_banner_eol, banner_command, CONF_BANNER);
TEXT	(conf_banner_char, conf_banner_eol, no_alt,
	 OBJ(string,1),
	 "c banner-text c, where 'c' is a delimiting character");

NOPREFIX (conf_banner_noprefix, conf_banner_char, conf_banner_eol);
NVGENS	(conf_banner_nvgen, conf_banner_noprefix, banner_command, CONF_BANNER);


TESTVAR	(conf_banner_default_nvgen, NONE, conf_banner_char,
	 NONE, NONE, NONE, nvgen, TRUE );
SET	(conf_banner_default_set, conf_banner_default_nvgen,
	 OBJ(int,1), BANNER_MOTD);
TEST_MULTIPLE_FUNCS(conf_banner_default, conf_banner_default_set, NONE, NONE);

KEYWORD_ID (conf_banner_motd, conf_banner_nvgen, conf_banner_default,
	 OBJ(int,1), BANNER_MOTD,
	 "motd", "Set Message of the Day banner", PRIV_CONF);

KEYWORD_ID (conf_banner_login, conf_banner_nvgen, conf_banner_motd,
	    OBJ(int,1), BANNER_LOGIN,
	    "login", "Set login banner", PRIV_CONF);

KEYWORD_ID (conf_banner_incoming, conf_banner_nvgen, conf_banner_login,
	 OBJ(int,1), BANNER_INCOMING,
	 "incoming", "Set incoming terminal line banner", PRIV_CONF);

KEYWORD_ID (conf_banner_conf, conf_banner_nvgen, conf_banner_incoming,
	 OBJ(int,1), BANNER_EXEC,
	 "exec", "Set EXEC process creation banner", PRIV_CONF);

/******************************************************************/
KEYWORD	(conf_banner, conf_banner_conf, ALTERNATE,
	 "banner", "Define a login banner", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_banner
