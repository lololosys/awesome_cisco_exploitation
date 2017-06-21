/* $Id: cfg_line_stopchar.h,v 3.2 1995/11/17 18:45:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_stopchar.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S T O P C H A R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_stopchar.h,v $
 * Revision 3.2  1995/11/17  18:45:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * stop-character <num>
 * no stop-character
 */

EOLS	(stopchar_eol, line_command, PARSER_LINE_STOPCHAR);
CHAR8_NUMBER(stopchar_char, stopchar_eol, no_alt,
	    OBJ(int,1), "Stop character or its decimal equivalent");

NOPREFIX(stopchar_noprefix, stopchar_char, stopchar_eol);
NVGENS	(stopchar_nvgen, stopchar_noprefix,
	 line_command, PARSER_LINE_STOPCHAR);

KEYWORD	(stopchar, stopchar_nvgen, ALTERNATE,
	 "stop-character", "Define the stop character", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	stopchar
