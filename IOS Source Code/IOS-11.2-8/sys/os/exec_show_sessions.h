/* $Id: exec_show_sessions.h,v 3.3.20.1 1996/08/12 16:05:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_sessions.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S E S S I O N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_sessions.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:09  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show sessions
 *
 */
EOLNS	(show_sessions_eol, where_command);
KEYWORD (show_sessions, show_sessions_eol, ALTERNATE,
	 "sessions", "Information about Telnet connections", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_sessions
