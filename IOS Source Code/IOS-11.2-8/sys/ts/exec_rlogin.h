/* $Id: exec_rlogin.h,v 3.4 1995/12/15 22:28:18 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ts/exec_rlogin.h,v $
 *------------------------------------------------------------------
 * E X E C _ R L O G I N . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_rlogin.h,v $
 * Revision 3.4  1995/12/15  22:28:18  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.3  1995/12/14  13:08:02  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:54:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/01  23:10:01  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.1  1995/07/04  01:58:57  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/15  15:19:22  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:21:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * rlogin <host> [debug]
 *  OBJ(int,3) = TRUE if debug specified
 *  OBJ(int,10) = TRUE if username OBJ(string,2) specified
 *
 */
LINK_TRANS(exec_rlogin_port_exit, no_alt);
LINK_TRANS(exec_rlogin_eol_exit, no_alt);
PRIV_TEST(exec_rlogin_hidden, exec_rlogin_port_exit, NONE,
	  exec_rlogin_eol_exit, PRIV_HIDDEN);

KEYWORD_ID(exec_rlogin_debug, exec_rlogin_hidden, exec_rlogin_hidden,
	   OBJ(int,3), TRUE,
	   "debug", "Enable rlogin debugging output", PRIV_USER);

TEST_MULTIPLE_FUNCS(exec_rlogin_test, exec_rlogin_debug,
		    no_alt, NONE);

STRING  (exec_rlogin_uname, exec_rlogin_debug, no_alt,
         OBJ(string,2), "Remote user name");
KEYWORD_ID (exec_rlogin_l, exec_rlogin_uname, exec_rlogin_test,
         OBJ(int, 10), TRUE,
         "-l", "Specify remote username", PRIV_USER);
KEYWORD_ID (exec_rlogin_user, exec_rlogin_uname, exec_rlogin_l,
         OBJ(int, 10), TRUE,
         "/user", "Specify remote username", PRIV_USER);


STRING	(exec_rlogin_host, exec_rlogin_user, no_alt,
	 OBJ(string,1), "IP address or hostname of a remote system");
SET	(exec_rlogin_port_set, exec_rlogin_host,
	 OBJ(int,1), RLOGIN_PORT);
KEYWORD_ID(exec_rlogin, exec_rlogin_port_set, ALTERNATE,
	   OBJ(int,9), CMD_CONNECT_RLOGIN,
	   "rlogin", "Open an rlogin connection",
	   PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_rlogin
