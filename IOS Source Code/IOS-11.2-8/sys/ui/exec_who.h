/* $Id: exec_who.h,v 3.4.20.1 1996/08/12 16:09:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_who.h,v $
 *------------------------------------------------------------------
 * E X E C _ W H O . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_who.h,v $
 * Revision 3.4.20.1  1996/08/12  16:09:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:34:50  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:10:10  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:48:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:35  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:28:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * who [all]
 *
 * OBJ(int,1) = TRUE if all users
 */
EOLNS	(exec_who_eol, show_systat);
KEYWORD_ID(exec_who_all, exec_who_eol, exec_who_eol,
	   OBJ(int,1), TRUE,
	   "all", NULL /*"Include information about inactive ports"*/,
	   PRIV_USER);
KEYWORD (exec_who, exec_who_all, ALTERNATE,
	 "who", NULL, PRIV_USER | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	exec_who
