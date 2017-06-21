/* $Id: exec_setup.h,v 3.3.4.1 1996/08/12 16:08:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_setup.h,v $
 *------------------------------------------------------------------
 * E X E C _ S E T U P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_setup.h,v $
 * Revision 3.3.4.1  1996/08/12  16:08:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/03/09  22:58:08  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  17:48:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:30  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * setup
 */
EOLNS	(exec_setup_eol, setup_command);

KEYWORD	(exec_setup, exec_setup_eol, ALTERNATE,
	 "setup", "Run the SETUP command facility",
	 PRIV_ROOT | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_setup
