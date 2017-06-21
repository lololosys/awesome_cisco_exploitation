/* $Id: cfg_line_width.h,v 3.2 1995/11/17 18:44:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_width.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ W I D T H . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_width.h,v $
 * Revision 3.2  1995/11/17  18:44:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:10:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * width <number>
 * no width [<number>]
 *
 * OBJ(int,1) = <number>
 */

EOLS	(cl_width_eol, line_command, TERM_WIDTH);
NUMBER	(cl_width_num, cl_width_eol, no_alt, 
	 OBJ(int,1), 0, 512, "Number of characters on a screen line");
NOPREFIX (cl_width_noprefix, cl_width_num, cl_width_eol);
NVGENS	(cl_width_nvgen, cl_width_noprefix, line_command, TERM_WIDTH);
KEYWORD	(cl_width, cl_width_nvgen, ALTERNATE,
	 "width", "Set width of the display terminal", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	cl_width

