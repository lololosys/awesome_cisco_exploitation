/* $Id: cfg_line_terminaltype.h,v 3.2 1995/11/17 18:44:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_terminaltype.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ T E R M I N A L T Y P E . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_terminaltype.h,v $
 * Revision 3.2  1995/11/17  18:44:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:10:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * terminal-type <str>
 * no terminal-type
 */
EOLS	(terminal_type_eol, line_command, PARSER_LINE_TERM_TYPE);
STRING	(terminal_type_str, terminal_type_eol, no_alt,
	 OBJ(string,1), "Terminal type");
NOPREFIX(terminal_type_noprefix, terminal_type_str,
	 terminal_type_eol); 
NVGENS	(terminal_type_nvgen, terminal_type_noprefix,
	 line_command, PARSER_LINE_TERM_TYPE);
KEYWORD (terminal_type, terminal_type_nvgen, ALTERNATE,
	 "terminal-type", "Set the terminal type", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	terminal_type
