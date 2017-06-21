/* $Id: exec_test_sse_unix.h,v 3.3.8.1 1996/08/12 16:09:05 widmer Exp $
 * $Source: /release/112/cvs/Xsys/unix/exec_test_sse_unix.h,v $
 *------------------------------------------------------------------
 * exec_test_sse_unix.h - Unix specific SSE debugging functions
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_sse_unix.h,v $
 * Revision 3.3.8.1  1996/08/12  16:09:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/29  00:52:32  hampton
 * Fix SSE parse chain so the unix images compile.  [CSCdi50303]]
 *
 * Revision 3.2  1995/11/17  17:55:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:18:10  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:27:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test sse trace <filename>
 *
 * OBJ(string,1) = <filename>
 */

EOLNS	(test_unix_sse_trace_eol, unix_ip_trace_sse);
STRING  (test_unix_sse_tracename, test_unix_sse_trace_eol,
	 no_alt, OBJ(string,1), "filename to load into cache");
KEYWORD (test_unix_sse_trace, test_unix_sse_tracename, ALTERNATE,
	 "trace", "Parse an IP trace file", PRIV_OPR);

/******************************************************************
 * test sse [<protocol> <filename>]
 *
 * OBJ(int,1) = <protocol>
 * OBJ(string,2) = <filename>
 */
EOLS	(test_unix_sse_eol, clear_sse, SSE_PROTOCOL_ANY);
SET	(test_unix_sse_noname, test_unix_sse_eol, OBJ(string,2), NULL);
STRING  (test_unix_sse_name, test_unix_sse_eol, test_unix_sse_noname,
	 OBJ(string,2), "filename to load into cache");

KEYWORD_ID (test_unix_sse_ipx, test_unix_sse_name, test_unix_sse_trace,
	    OBJ(int,1), SSE_PROTOCOL_IPX, "novell", "Novell", PRIV_OPR);
KEYWORD_ID (test_unix_sse_ip, test_unix_sse_name, test_unix_sse_ipx,
	    OBJ(int,1), SSE_PROTOCOL_IP, "ip", "IP", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	test_unix_sse_ip
