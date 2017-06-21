/* $Id: cfg_line_help.h,v 3.2 1995/11/17 18:44:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_help.h,v $
 *------------------------------------------------------------------
 * Terminal help command
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_help.h,v $
 * Revision 3.2  1995/11/17  18:44:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(ct_help_eol, line_command, PARSER_LINE_HELP);
KEYWORD	(ct_help, ct_help_eol, ALTERNATE,
	 "full-help", "Provide help to unprivileged user", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	ct_help
