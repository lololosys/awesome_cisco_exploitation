/* $Id: cfg_line_holdchar.h,v 3.2 1995/11/17 18:53:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_holdchar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ H O L D C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_holdchar.h,v $
 * Revision 3.2  1995/11/17  18:53:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:46  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * hold-character <num>
 * no hold-character
 */

EOLS	(holdchar_eol, asm_line_commands, PARSER_LINE_HOLDCHAR);
CHAR8_NUMBER(holdchar_char, holdchar_eol, no_alt,
	    OBJ(int,1), "Hold character or its decimal equivalent");

NOPREFIX(holdchar_noprefix, holdchar_char, holdchar_eol);
NVGENS	(holdchar_nvgen, holdchar_noprefix,
	 asm_line_commands, PARSER_LINE_HOLDCHAR);

KEYWORD	(holdchar, holdchar_nvgen, ALTERNATE,
	 "hold-character", "Define the hold character", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	holdchar
