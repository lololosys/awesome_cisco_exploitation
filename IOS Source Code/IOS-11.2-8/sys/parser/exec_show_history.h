/* $Id: exec_show_history.h,v 3.4.4.1 1996/08/12 16:06:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_history.h,v $
 *------------------------------------------------------------------
 * show history
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_history.h,v $
 * Revision 3.4.4.1  1996/08/12  16:06:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/03/09  22:56:49  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/01/29  07:09:20  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:24:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 *  show history
 *
 */
EOLNS	(show_history_eol, print_history);
KEYWORD	(show_history, show_history_eol, ALTERNATE,
	 "history", "Display the session command history",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	show_history
