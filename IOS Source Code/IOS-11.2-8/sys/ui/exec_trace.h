/* $Id: exec_trace.h,v 3.5.4.1 1996/08/12 16:09:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_trace.h,v $
 *------------------------------------------------------------------
 * E X E C _ T R A C E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_trace.h,v $
 * Revision 3.5.4.1  1996/08/12  16:09:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5  1996/03/09  22:58:09  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.4  1996/01/29  07:10:09  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/14  13:08:05  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  17:48:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  00:51:00  ronnie
 * CSCdi37018:  trace command should be traceroute
 *
 * Revision 2.1  1995/07/04  01:59:34  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * trace [[hint] <destination>]
 *
 * OBJ(int,1) = protocol hint (if any) or LINK_ILLEGAL
 * OBJ(string,1) = destination
 */

EOLNS	(exec_trace_eol, trace_command);

PRIV_TEST(exec_trace_assert, exec_trace_eol, NONE, no_alt,
	  PRIV_ROOT | PRIV_INTERACTIVE);

STRING	(exec_trace_destination, exec_trace_eol, exec_trace_assert,
	 OBJ(string,1), "Trace route to destination address or hostname");

TEST_MULTIPLE_FUNCS(exec_trace_test, exec_trace_destination, no_alt, NONE);

KEYWORD_ID (exec_trace_appletalk, exec_trace_destination, exec_trace_test,
	    OBJ(int,1), LINK_APPLETALK,
	    "appletalk", "AppleTalk Trace", PRIV_USER);
KEYWORD_ID (exec_trace_vines, exec_trace_destination, exec_trace_appletalk,
	    OBJ(int,1), LINK_VINES,
	    "vines", "Vines Trace (Banyan)", PRIV_USER);
KEYWORD_ID (exec_trace_vines_old, exec_trace_destination, exec_trace_vines,
	    OBJ(int,1), LINK_VINES_ECHO,
	    "oldvines", "Vines Trace (Cisco)", PRIV_USER);
KEYWORD_ID (exec_trace_clns, exec_trace_destination, exec_trace_vines_old,
	    OBJ(int,1), LINK_CLNS,
	    "clns", "ISO CLNS Trace", PRIV_USER);
KEYWORD_ID (exec_trace_ip, exec_trace_destination, exec_trace_clns,
	    OBJ(int,1), LINK_IP,
	    "ip", "IP Trace", PRIV_USER);

KEYWORD	(exec_trace, exec_trace_ip, ALTERNATE,
	 "traceroute", "Trace route to destination", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_trace
