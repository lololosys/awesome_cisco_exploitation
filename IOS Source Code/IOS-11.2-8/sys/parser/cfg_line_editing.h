/* $Id: cfg_line_editing.h,v 3.2 1995/11/17 18:44:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_editing.h,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_editing.h,v $
 * Revision 3.2  1995/11/17  18:44:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 *
 */

EOLS	(cl_editing_eol, line_command, PARSER_LINE_EDITING);
KEYWORD	(cl_editing, cl_editing_eol, ALTERNATE,
	 "editing", "Enable command line editing", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	cl_editing
