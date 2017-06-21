/* $Id: cfg_line_dispatchchar.h,v 3.2 1995/11/17 18:53:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_dispatchchar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D I S P A T C H C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_dispatchchar.h,v $
 * Revision 3.2  1995/11/17  18:53:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:44  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * dispatch-character <num>
 * no dispatch-character
 */
EOLS    (dispatchchar_eol, asm_line_commands, PARSER_LINE_DISPATCHCHAR);

TESTVAR (dispatchchar_no_test2, no_alt, dispatchchar_eol,
	 NONE, NONE, NONE, OBJ(int,10), 0);
TESTVAR (dispatchchar_no_test1, dispatchchar_no_test2, dispatchchar_eol,
	 NONE, NONE, NONE, sense, TRUE);

pdecl(dispatchchar_char);
EVAL	(dispatchchar_char_eval, dispatchchar_char,
	 (csb->uiarray[GETOBJ(int,10)++] = GETOBJ(int,1)));
CHAR_NUMBER(dispatchchar_char, dispatchchar_char_eval, dispatchchar_no_test1,
            OBJ(int,1), "Dispatch character or its decimal equivalent");

NVGENS  (dispatchchar_nvgen, dispatchchar_char,
	 asm_line_commands, PARSER_LINE_DISPATCHCHAR);

KEYWORD (dispatchchar, dispatchchar_nvgen, ALTERNATE,
         "dispatch-character", "Define the dispatch character", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	dispatchchar
