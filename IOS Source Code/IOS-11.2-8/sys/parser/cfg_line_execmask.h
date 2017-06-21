/* $Id: cfg_line_execmask.h,v 3.2 1995/11/17 18:44:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_execmask.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E X E C M A S K . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_execmask.h,v $
 * Revision 3.2  1995/11/17  18:44:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * exec-character-bits <num>
 * no exec-character-bits
 */
EOLS	(cl_execmask_eol, line_command, PARSER_LINE_EXEC_MASK );
NUMBER	(cl_execmask_num, cl_execmask_eol, no_alt,
	 OBJ(int,1), 7, 8, "Bits per character" );
NOPREFIX(cl_execmask_noprefix, cl_execmask_num, cl_execmask_eol );
NVGENS	(cl_execmask_nvgen, cl_execmask_noprefix,
	 line_command, PARSER_LINE_EXEC_MASK );
KEYWORD_MM(cl_execmask, cl_execmask_nvgen, ALTERNATE,
	   "exec-character-bits", 
	   "Size of characters to the command exec", PRIV_USER, 6 );

#undef	ALTERNATE
#define	ALTERNATE	cl_execmask
