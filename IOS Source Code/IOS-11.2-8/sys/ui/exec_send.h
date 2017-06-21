/* $Id: exec_send.h,v 3.3.4.1 1996/08/12 16:08:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_send.h,v $
 *------------------------------------------------------------------
 * E X E C _ S E N D . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_send.h,v $
 * Revision 3.3.4.1  1996/08/12  16:08:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/03/09  22:58:07  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  17:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:30  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * send { <line-number> | * }
 *
 * OBJ(int,1) = <line-number>
 *		MAXUINT for all lines
 */
EOLNS	(exec_send_eol, send_command);
KEYWORD_ID(exec_send_anyline, exec_send_eol, no_alt,
	   OBJ(int,1), MAXUINT,
	   "*", "All tty lines", PRIV_USER);
TTYLINE (exec_send_line, exec_send_eol, exec_send_anyline,
	 OBJ(int,1), "Send a message to a specific line");
KEYWORD	(exec_send, exec_send_line, ALTERNATE,
	 "send", "Send a message to other tty lines",
	 PRIV_OPR | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_send
