/* $Id: cfg_line_autoselect.h,v 3.3 1996/02/01 23:34:44 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ts/cfg_line_autoselect.h,v $
 *------------------------------------------------------------------
 * Autoselect
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_autoselect.h,v $
 * Revision 3.3  1996/02/01  23:34:44  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:53:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:42  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * autoselect
 *
 */



EOLS	(autoselect_default_eol, asm_line_commands, PARSER_LINE_AUTOSELECTDEF);

EOLS	(autoselect_eol, asm_line_commands, PARSER_LINE_AUTOSELECT);

KEYWORD_ID(autoselect_ppp, autoselect_eol, autoselect_default_eol,
	   OBJ(int,1), PARSER_AUTOSELECT_PPP,
	   "ppp", "Set line to allow PPP autoselection", PRIV_CONF);

KEYWORD_ID(autoselect_slip, autoselect_eol, autoselect_ppp,
	   OBJ(int,1), PARSER_AUTOSELECT_SLIP,
	   "slip", "Set line to allow SLIP autoselection", PRIV_CONF);

KEYWORD_ID(autoselect_duringlogin, autoselect_eol, autoselect_slip,
	   OBJ(int,1), PARSER_AUTOSELECT_DURING_LOGIN,
	   "during-login", "Do autoselect at the Username/Password prompt",
	   PRIV_CONF);

KEYWORD_ID(autoselect_arap, autoselect_eol, autoselect_duringlogin,
	   OBJ(int,1), PARSER_AUTOSELECT_ARAP,
	   "arap", "Set line to allow ARAP autoselection", PRIV_CONF);

KEYWORD (autoselect, autoselect_arap, ALTERNATE,
	 "autoselect", "Set line to autoselect", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	autoselect
