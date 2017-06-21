/* $Id: cfg_line_databits.h,v 3.2 1995/11/17 18:45:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_databits.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D A T A B I T S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_databits.h,v $
 * Revision 3.2  1995/11/17  18:45:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:09:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * databits { 5 | 6 | 7 | 8 }
 *
 */
EOLS	(databits_eol, line_command, PARSER_LINE_DATABITS);
KEYWORD_ID(databits8, databits_eol, no_alt,
	   OBJ(int,1), 8, "8", "", PRIV_USER);
KEYWORD_ID(databits7, databits_eol, databits8,
	   OBJ(int,1), 7, "7", "", PRIV_USER);
KEYWORD_ID(databits6, databits_eol, databits7,
	   OBJ(int,1), 6, "6", "", PRIV_USER);
KEYWORD_ID(databits5, databits_eol, databits6,
	   OBJ(int,1), 5, "5", "Number of data bits", PRIV_USER);

SET	(databits_noprefix_set, databits_eol, OBJ(int,1), 8);
NOPREFIX(databits_noprefix, databits5, databits_noprefix_set);
NVGENS	(databits_nvgens, databits_noprefix,
	 line_command, PARSER_LINE_DATABITS);
KEYWORD	(line_databits, databits_nvgens, ALTERNATE,
	 "databits", "Set number of data bits per character", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_databits
