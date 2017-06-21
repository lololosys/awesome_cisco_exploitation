/* $Id: cfg_line_startchar.h,v 3.2 1995/11/17 18:45:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_startchar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S T A R T C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_startchar.h,v $
 * Revision 3.2  1995/11/17  18:45:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * start-character <num>
 * no start-character
 */
EOLS	(startchar_eol, line_command, PARSER_LINE_STARTCHAR);
CHAR8_NUMBER(startchar_char, startchar_eol, no_alt,
	    OBJ(int,1), "Start character or its decimal equivalent");

NOPREFIX(startchar_noprefix, startchar_char, startchar_eol);
NVGENS	(startchar_nvgen, startchar_noprefix,
	 line_command, PARSER_LINE_STARTCHAR);

KEYWORD	(startchar, startchar_nvgen, ALTERNATE,
	 "start-character", "Define the start character", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	startchar
