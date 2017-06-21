/* $Id: cfg_line_autocommand.h,v 3.2 1995/11/17 18:45:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_autocommand.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ A U T O C O M M A N D . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_autocommand.h,v $
 * Revision 3.2  1995/11/17  18:45:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:09:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * autocommand <text>
 * no autocommand
 */
EOLS	(line_autocommand_eol, line_command, PARSER_LINE_AUTOCOMMAND);
TEXT	(line_autocommand_text, line_autocommand_eol, no_alt,
	 OBJ(string,1), "Appropriate EXEC command");
NOPREFIX(line_autocommand_noprefix, line_autocommand_text,
	 line_autocommand_eol);
NVGENS	(line_autocommand_nvgen, line_autocommand_noprefix,
	 line_command, PARSER_LINE_AUTOCOMMAND);
KEYWORD	(line_autocommand, line_autocommand_nvgen, ALTERNATE,
	 "autocommand", "Automatically execute an EXEC command", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	line_autocommand
