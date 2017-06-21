/* $Id: cfg_line.h,v 3.2 1995/11/17 18:44:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line.h,v $
 * Revision 3.2  1995/11/17  18:44:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:08:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 */

/***************************************************************
 *	line { aux | console | tty | vty }
 * 			 <first-line> [<last-line>]
 *
 * OBJ(int,1) = <first-line>
 * OBJ(int,2) = <last-line>
 */
EOLS  (cl_line_eol, line_command, LINE_NUMBER);

TTYLINE_RANGE(cl_line_range, cl_line_eol, no_alt,
	      OBJ(int,1), OBJ(int,2), "Line number", CON_ABSOLUTE);

KEYWORD_ID(cl_line, cl_line_range, ALTERNATE,
	   leave_subcommand, TRUE,
	   "line", "Configure a terminal line", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_line
