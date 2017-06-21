/* $Id: cfg_privilege.h,v 3.2.62.1 1996/08/12 16:05:50 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_privilege.h,v $
 *------------------------------------------------------------------
 * C F G _ P R I V I L E G E . H
 *
 * Privilege global configuration command.
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_privilege.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:44:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:11:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************************
 * priv <mode> level <level> <command>
 * no priv <mode> <command>
 *
 * csb->pmp = mode
 * OBJ(int,1) = <level>
 * OBJ(string,1) = <command>
 */
EOLNS	(conf_priv_eol, priv_command);
TEXT	(conf_priv_cmd, conf_priv_eol, no_alt,
	 OBJ(string,1), "Initial keywords of the command to modify");
TEST_MULTIPLE_FUNCS(conf_priv_test, conf_priv_cmd, conf_priv_eol, NONE);

KEYWORD_ID(config_priv_reset, conf_priv_cmd, NONE,
	   OBJ(int,2), TRUE,
	   "reset", "Reset privilege level of command", PRIV_ROOT);
TESTVAR	(config_priv_notest, conf_priv_test, config_priv_reset, NONE, NONE,
	 no_alt, sense, FALSE);
NUMBER	(conf_priv_level, conf_priv_cmd, no_alt,
	 OBJ(int,1), PRIV_MIN, PRIV_MAX, "Privilege level");
KEYWORD	(conf_priv_level_kw, conf_priv_level, config_priv_notest,
	 "level", "Set privilege level of command", PRIV_ROOT);
NVGENNS	(conf_priv_nvgen, conf_priv_level_kw, priv_command);
MODE	(conf_priv_mode, conf_priv_nvgen, no_alt,
	 pmp, PARSER_MODE_PRIVILEGE);

/*************************************************************************/
KEYWORD	(configure_priv, conf_priv_mode, ALTERNATE,
	 "privilege", "Command privilege parameters", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_priv
