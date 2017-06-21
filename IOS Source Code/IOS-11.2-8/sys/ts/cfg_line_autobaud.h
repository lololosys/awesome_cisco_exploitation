/* $Id: cfg_line_autobaud.h,v 3.4 1996/02/15 07:26:35 irfan Exp $
 * $Source: /release/111/cvs/Xsys/ts/cfg_line_autobaud.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ A U T O B A U D . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_autobaud.h,v $
 * Revision 3.4  1996/02/15  07:26:35  irfan
 * CSCdi48971:  autobaud command not nvgen-ed
 * ensure OBJ(int,1) is 0.
 *
 * Revision 3.3  1996/02/01  23:34:42  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:53:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:40  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * autobaud
 *
 */
EOLS	(autobaud_eol, asm_line_commands, PARSER_LINE_AUTOBAUD);

KEYWORD_ID(autobaud_fast, autobaud_eol, autobaud_eol,
	   OBJ(int,1), TRUE,
	   "fast", NULL, PRIV_CONF|PRIV_HIDDEN);

KEYWORD_ID (autobaud_kwd, autobaud_fast, NONE,
	    OBJ(int,1), FALSE,
	    "autobaud", "Set line to autobaud", PRIV_CONF);

TESTVAR	(autobaud, NONE, autobaud_kwd, NONE, NONE, ALTERNATE,
	 line1, 0);


#undef	ALTERNATE
#define	ALTERNATE	autobaud
