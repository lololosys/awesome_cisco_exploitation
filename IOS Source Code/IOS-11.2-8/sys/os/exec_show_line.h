/* $Id: exec_show_line.h,v 3.3.20.1 1996/08/12 16:05:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_line.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ L I N E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_line.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:05  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:46:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:24:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show line [<line>]
 * 
 * OBJ(int,1) =	<line> (-1 if no line specified)
 */
EOLNS	(show_line_eol, display_lines);
SET	(show_line_set, show_line_eol, OBJ(int,1), MAXUINT);
TTYLINE	(show_line_num, show_line_eol, show_line_set,
	OBJ(int,1), "Line number");
KEYWORD (show_line, show_line_num, ALTERNATE,
	 "line", "TTY line information", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_line
