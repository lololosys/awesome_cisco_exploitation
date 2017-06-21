/* $Id: cfg_line_disconnectchar.h,v 3.2 1995/11/17 18:53:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_disconnectchar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D I S C O N N E C T C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_disconnectchar.h,v $
 * Revision 3.2  1995/11/17  18:53:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:43  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * disconnect-character <num>
 * no disconnect-character
 */
EOLS	(disconnect_char_eol, asm_line_commands, PARSER_LINE_DISCONNECTCHAR);
CHAR8_NUMBER(disconnect_char_num, disconnect_char_eol, no_alt,
	    OBJ(int,1), "Disconnect character or its decimal equivalent");

NOPREFIX(disconnect_char_noprefix, disconnect_char_num, disconnect_char_eol);
NVGENS	(disconnect_char_nvgen, disconnect_char_noprefix,
	 asm_line_commands, PARSER_LINE_DISCONNECTCHAR);

KEYWORD	(disconnect_char, disconnect_char_nvgen, ALTERNATE,
	 "disconnect-character", "Define the disconnect character", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	disconnect_char
