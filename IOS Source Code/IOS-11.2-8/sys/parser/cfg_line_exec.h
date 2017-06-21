/* $Id: cfg_line_exec.h,v 3.2 1995/11/17 18:44:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_exec.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E X E C . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_exec.h,v $
 * Revision 3.2  1995/11/17  18:44:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exec
 *
 */

EOLS	(line_exec_eol, line_command, PARSER_LINE_EXEC );

KEYWORD	(line_exec, line_exec_eol, ALTERNATE,
	 "exec", "Start an EXEC process", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	line_exec
