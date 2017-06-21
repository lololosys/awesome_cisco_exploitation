/* $Id: exec_login.h,v 3.3.46.1 1996/08/12 16:08:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_login.h,v $
 *------------------------------------------------------------------
 * E X E C _ L O G I N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_login.h,v $
 * Revision 3.3.46.1  1996/08/12  16:08:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:23  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  17:47:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:28  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * login
 */
EOLNS	(exec_login_eol, login_exec_command);
KEYWORD	(exec_login, exec_login_eol, ALTERNATE,
	 "login", "Log in as a particular user",
	 PRIV_USER | PRIV_INTERACTIVE);


#undef	ALTERNATE
#define	ALTERNATE	exec_login
