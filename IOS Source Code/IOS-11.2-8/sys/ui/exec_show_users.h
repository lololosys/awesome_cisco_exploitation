/* $Id: exec_show_users.h,v 3.4.20.1 1996/08/12 16:08:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_show_users.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ U S E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_users.h,v $
 * Revision 3.4.20.1  1996/08/12  16:08:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:34:49  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:10:06  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:48:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:32  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:26:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show users [all]
 *
 * OBJ(int,1) = TRUE if all users
 */
EOLNS	(show_users_eol, show_systat);
KEYWORD_ID(show_users_all, show_users_eol, show_users_eol,
	   OBJ(int,1), TRUE,
	   "all", "Include information about inactive ports", PRIV_USER);
KEYWORD (show_users, show_users_all, ALTERNATE,
	 "users", "Display information about terminal lines", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_users
