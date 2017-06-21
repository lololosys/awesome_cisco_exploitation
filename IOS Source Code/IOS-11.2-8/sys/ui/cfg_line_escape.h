/* $Id: cfg_line_escape.h,v 3.2 1995/11/17 17:47:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ui/cfg_line_escape.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E S C A P E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_escape.h,v $
 * Revision 3.2  1995/11/17  17:47:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:17  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * escape-character { <decimal-number> | <char> }
 * no escape-character
 *
 * OBJ(int,1) =	<decimal-number> or the decimal equivalent of <char>
 */
EOLS	(cl_escape_eol, escape_command, TERM_ESCAPE);
CHAR8_NUMBER (cl_escape_charnum, cl_escape_eol, no_alt,
	     OBJ(int,1), "Escape character or its ASCII decimal equivalent");
EOLS	(cl_escape_dflt_eol, escape_command, TERM_ESCAPE_DEFAULT);
KEYWORD	(cl_escape_dflt, cl_escape_dflt_eol, cl_escape_charnum,
	"DEFAULT", "Use default escape character", PRIV_USER);
EOLS	(cl_escape_none_eol, escape_command, TERM_ESCAPE_NONE);
KEYWORD	(cl_escape_none, cl_escape_none_eol, cl_escape_dflt,
	"NONE", "Disable escape entirely", PRIV_USER);
EOLS	(cl_escape_break_eol, escape_command, TERM_ESCAPE_BREAK);
KEYWORD	(cl_escape_break, cl_escape_break_eol, cl_escape_none,
	"BREAK", "Cause escape on BREAK", PRIV_USER);
NOPREFIX (cl_escape_noprefix, cl_escape_break, cl_escape_eol);
NVGENS	(cl_escape_nvgen, cl_escape_noprefix, escape_command, TERM_ESCAPE);
KEYWORD	(cl_escape, cl_escape_nvgen, ALTERNATE,
	 "escape-character", "Change the current line's escape character",
	 PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	cl_escape
