/* $Id: cfg_line_activatechar.h,v 3.2 1995/11/17 18:53:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_activatechar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ A C T I V A T E C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_activatechar.h,v $
 * Revision 3.2  1995/11/17  18:53:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:39  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:08:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * activation-character <num>
 * no activation-character
 */
EOLS	(activate_char_eol, asm_line_commands, PARSER_LINE_ACTIVATECHAR);
CHAR_NUMBER(activate_char_num, activate_char_eol, no_alt,
	    OBJ(int,1), "Activation character or its decimal equivalent");

NOPREFIX(activate_char_noprefix, activate_char_num, activate_char_eol);
NVGENS	(activate_char_nvgen, activate_char_noprefix,
	 asm_line_commands, PARSER_LINE_ACTIVATECHAR);

KEYWORD	(activate_char, activate_char_nvgen, ALTERNATE,
	 "activation-character", "Define the activation character",
	 PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	activate_char
