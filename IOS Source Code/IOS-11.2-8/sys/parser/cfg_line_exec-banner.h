/* $Id: cfg_line_exec-banner.h,v 3.2 1995/11/17 18:44:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_exec-banner.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E X E C - B A N N E R . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_exec-banner.h,v $
 * Revision 3.2  1995/11/17  18:44:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * exec-banner
 * no exec-banner
 */
EOLS	(cl_execbanner_eol, line_command, LINE_BANNER);
KEYWORD	(cl_execbanner_kwd, cl_execbanner_eol, NONE,
	 "exec-banner", "Enable the display of the EXEC banner", PRIV_CONF);
TEST_MULTIPLE_FUNCS(cl_execbanner, cl_execbanner_kwd, NONE, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	cl_execbanner
