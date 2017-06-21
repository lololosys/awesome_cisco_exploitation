/* $Id: exec_show_pad.h,v 3.1.2.2 1996/08/12 16:09:48 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_show_pad.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ X 2 9  . H
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_pad.h,v $
 * Revision 3.1.2.2  1996/08/12  16:09:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/30  23:47:00  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/30  20:49:18  hampton
 * Placeholder files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show x29 access-lists
 *
 * OBJ(int,1) = <list-number>
 */
EOLNS	(show_x29_access_eol, show_x29);
NUMBER  (show_x29_access_list, show_x29_access_eol, show_x29_access_eol,
	 OBJ(int,1), MINREGEXPACC, MAXREGEXPACC, "Access list number");
KEYWORD (show_x29_access, show_x29_access_list, no_alt,
	 "access-lists", "X.29 access lists", PRIV_USER);

/******************************************************************/
KEYWORD (show_x29, show_x29_access, ALTERNATE,
	 "x29", "X.29 information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_x29
