/* $Id: cfg_line_length.h,v 3.2 1995/11/17 18:44:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_length.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L E N G T H . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_length.h,v $
 * Revision 3.2  1995/11/17  18:44:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	length <number>
 *	[no] length [<number>]
 *
 * OBJ(int,1) = <number>
 */
EOLS	(cl_length_eol, line_command, TERM_LENGTH);
NUMBER	(cl_length_num, cl_length_eol, no_alt, 
	 OBJ(int,1), 0, MAXTTYLENGTH, "Number of lines on screen (0 for no pausing)");
SET	(cl_length_default, cl_length_eol, OBJ(int,1), LENGTH_DEFAULT);
NOPREFIX (cl_length_noprefix, cl_length_num, cl_length_default);
NVGENS	(cl_length_nvgen, cl_length_noprefix, line_command, TERM_LENGTH);
KEYWORD	(cl_length, cl_length_nvgen, ALTERNATE,
	 "length", "Set number of lines on a screen", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	cl_length
