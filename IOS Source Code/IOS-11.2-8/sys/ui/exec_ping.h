/* $Id: exec_ping.h,v 3.5.4.1 1996/08/12 16:08:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_ping.h,v $
 *------------------------------------------------------------------
 * E X E C _ P I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_ping.h,v $
 * Revision 3.5.4.1  1996/08/12  16:08:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5  1996/03/09  22:58:06  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.4  1995/12/15  22:28:24  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.3  1995/12/14  13:08:04  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  17:48:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:29  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/15  15:19:19  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:21:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * ping [[hint] <destination>]
 *
 * OBJ(string,1) = destination
 * OBJ(int,1) = protocol hint (if any)
 */
EOLNS	(exec_ping_eol, ping_command);

PRIV_TEST(exec_ping_assert, exec_ping_eol, no_alt, NONE,
	  PRIV_ROOT | PRIV_INTERACTIVE);

STRING	(exec_ping_destination, exec_ping_eol, exec_ping_assert,
	 OBJ(string,1), "Ping destination address or hostname");
LINK_TRANS(exec_ping_return_here, exec_ping_destination);

TEST_MULTIPLE_FUNCS(exec_ping_test, exec_ping_destination, no_alt, NONE);

LINK_TRANS(exec_ping_extend_here, exec_ping_test);

KEYWORD_ID (exec_ping_ip, exec_ping_destination, exec_ping_extend_here,
	    OBJ(int,1), LINK_IP, "ip", "IP echo", PRIV_USER);

KEYWORD	(exec_ping, exec_ping_ip, ALTERNATE,
	 "ping", "Send echo messages", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	exec_ping
