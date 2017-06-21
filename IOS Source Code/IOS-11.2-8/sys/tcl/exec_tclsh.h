/* $Id: exec_tclsh.h,v 3.2.62.1 1996/08/12 16:08:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcl/exec_tclsh.h,v $
 *------------------------------------------------------------------
 * exec_tclon.h
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tclsh.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:26  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:26:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(exec_tclsh_eol, tclsh_command);

KEYWORD	(exec_tclsh, exec_tclsh_eol, ALTERNATE,
	 "tclsh", "Tool Comand Language a shell", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	exec_tclsh
