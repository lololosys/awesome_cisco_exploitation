/* $Id: exec_resume.h,v 3.3.46.1 1996/08/12 16:05:05 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_resume.h,v $
 *------------------------------------------------------------------
 * E X E C _ R E S U M E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_resume.h,v $
 * Revision 3.3.46.1  1996/08/12  16:05:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:03  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:46:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  06:40:39  billw
 * CSCdi35358:  RESUME command with no args broken (regress CSCdi27275)
 *
 * Revision 2.1  1995/07/02  10:51:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:21:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * resume [<connection-number> | <connection-name>] [<connection-options>]
 *
 * OBJ(int,1) = <connection-number>
 * OBJ(string,1) = <connection-name>
 * OBJ(string,2) = <connection-options>
 */
EOLNS	(exec_resume_eol, resume_command);

pdecl(exec_resume_opts);

TEXT	(ero_connect_args, exec_resume_eol, exec_resume_eol,
	 OBJ(string,3), "Command to be used to establish connection");

SET	(ero_opts_set, exec_resume_opts, OBJ(int,15), TRUE);
X3PAIR	(ero_set_x3pair, ero_opts_set, no_alt,
	 OBJ(int,11), OBJ(int,12), "X3 connection option pairs");
KEYWORD_ID(ero_set, ero_set_x3pair, NONE,
	   OBJ(int,10), TRUE,
	   "/set", "Set X3 connection options", PRIV_USER);
KEYWORD_ID(ero_next, ero_opts_set, ero_set,
	   OBJ(int,13), TRUE,
	   "/next", NULL /*"Step to next network connection"*/,
	   PRIV_USER|PRIV_NOHELP);
KEYWORD_ID(ero_connect, ero_connect_args, ero_next,
	   OBJ(int,14), TRUE,
	   "/connect", NULL /*"Establish a connection if necessary"*/,
	   PRIV_USER|PRIV_NOHELP);
KEYWORD_ID(ero_nostream, ero_opts_set, ero_connect,
	   OBJ(int,9), TRUE,
	   "/nostream", "Disable stream processing", PRIV_USER);
KEYWORD_ID(ero_stream, ero_opts_set, ero_nostream,
	   OBJ(int,8), TRUE,
	   "/stream", "Enable stream processing", PRIV_USER);
KEYWORD_ID(ero_noline, ero_opts_set, ero_stream,
	   OBJ(int,7), TRUE,
	   "/noline", "Disable telnet line mode", PRIV_USER);
KEYWORD_ID(ero_line, ero_opts_set, ero_noline,
	   OBJ(int,6), TRUE,
	   "/line", "Enable telnet line mode", PRIV_USER);
KEYWORD_ID(ero_noecho, ero_opts_set, ero_line,
	   OBJ(int,5), TRUE,
	   "/noecho", "Disable local echo", PRIV_USER);
KEYWORD_ID(ero_echo, ero_opts_set, ero_noecho,
	   OBJ(int,4), TRUE,
	   "/echo", "Perform local echo", PRIV_USER);
KEYWORD_ID(ero_nodebug, ero_opts_set, ero_echo,
	   OBJ(int,3), TRUE,
	   "/nodebug", "Do not print parameter changes & messages", 
	   PRIV_USER);
KEYWORD_ID(ero_debug, ero_opts_set, ero_nodebug,
	   OBJ(int,2), TRUE,
	   "/debug", "Print parameter changes & messages", PRIV_USER);
NOP	(exec_resume_opts, ero_debug, exec_resume_eol);

SET	(exec_resume_name_set, exec_resume_opts, OBJ(int,1), FALSE);
STRING	(exec_resume_name_string, exec_resume_name_set, NONE,
	 OBJ(string,1),
	 "The name of an active network connection or Connection options");

TEST_MULTIPLE_FUNCS(exec_resume_name_test, exec_resume_name_string,
	 exec_resume_eol, exec_resume_opts);

NUMBER	(exec_resume_num, exec_resume_opts, exec_resume_name_test,
	 OBJ(int,1), 1, MAX_CONNECTIONS,
	 "The number of an active network connection");
KEYWORD	(exec_resume, exec_resume_num, ALTERNATE,
	 "resume", "Resume an active network connection",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_resume
