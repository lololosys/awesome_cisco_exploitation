/* $Id: exec_tclquit.h,v 3.2.62.1 1996/08/12 16:08:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcl/exec_tclquit.h,v $
 *------------------------------------------------------------------
 * exec_tclquit.h
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tclquit.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:26  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:26:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(exec_tclquit_eol, tclquit_command);

KEYWORD	(exec_tclquit, exec_tclquit_eol, ALTERNATE,
	 "tclquit", "Quit Tool Comand Language  shell", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	exec_tclquit
