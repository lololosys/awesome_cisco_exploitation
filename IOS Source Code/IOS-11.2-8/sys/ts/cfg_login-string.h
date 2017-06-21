/* $Id: cfg_login-string.h,v 3.2.62.1 1996/08/12 16:08:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_login-string.h,v $
 *------------------------------------------------------------------
 * C F G _ L O G I N - S T R I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_login-string.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:51  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:11:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * login-string <hostname> <char> <text> <char>
 * no login-string <hostname> [<char> <text> <char>]
 * 
 * OBJ(string,1) = <char> <text> <char>
 * OBJ(string,2) = <hostname>
 */

EOLNS	(cfg_loginstr_eol, set_loginstring);
TEXT	(cfg_loginstr_char, cfg_loginstr_eol, no_alt,
	 OBJ(string,1), "c login-string c, where 'c' is a delimiting character");

NOPREFIX (cfg_loginstr_noprefix, cfg_loginstr_char, cfg_loginstr_eol);

STRING	(cfg_loginstr_host, cfg_loginstr_noprefix, no_alt,
	 OBJ(string,2), "Name of the host for this login string");
NVGENNS	(cfg_loginstr_nvgen, cfg_loginstr_host, set_loginstring);

/******************************************************************/
KEYWORD	(cfg_loginstr, cfg_loginstr_nvgen, ALTERNATE,
	 "login-string", "Define a host-specific login string", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_loginstr
