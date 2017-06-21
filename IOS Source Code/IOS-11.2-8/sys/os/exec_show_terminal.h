/* $Id: exec_show_terminal.h,v 3.3.20.1 1996/08/12 16:05:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_terminal.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ T E R M I N A L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_terminal.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:10  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:26:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show terminal
 *
 */
EOLNS	(show_terminal_eol, show_terminal);
KEYWORD (show_terminal, show_terminal_eol, ALTERNATE,
	 "terminal", "Display terminal configuration parameters", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_terminal
