/* $Id: exec_show_tcp.h,v 3.3.20.1 1996/08/12 16:08:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_show_tcp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ T C P . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_tcp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:08:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:55  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:26:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show tcp [<line>]
 * show tcp brief [all]
 * show tcp tcb <TCB address>
 *
 * OBJ(int,1) = <line>
 *		-1 if no line number.
 * OBJ(int,2) = TRUE if asked to see brief list
 * OBJ(int,3) = TRUE if "all" keyword seen
 * OBJ(int,4) = <TCB address> 
 */
EOLNS	(show_tcp_eol, show_tcpstatus);
OPT_HEXNUM (show_tcp_tcb_addr, show_tcp_eol, no_alt, 
             OBJ(int, 4), "TCB address");
KEYWORD (show_tcp_tcb, show_tcp_tcb_addr, show_tcp_eol, 
         "tcb", "TCB address", PRIV_USER);
KEYWORD_ID (show_tcp_all, show_tcp_eol, show_tcp_eol,
	OBJ(int,3), TRUE,
	"all", "All end-points (even listeners)", PRIV_USER);
KEYWORD_ID (show_tcp_brief, show_tcp_all, show_tcp_tcb,
	OBJ(int,2), TRUE,
	"brief", "Brief display", PRIV_USER);
SET	(show_tcp_set, show_tcp_brief, OBJ(int,1), -1);
TTYLINE (show_tcp_line, show_tcp_eol, show_tcp_set,
         OBJ(int,1), "Line number");
KEYWORD (show_tcp, show_tcp_line, ALTERNATE,
	 "tcp", "Status of TCP connections", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_tcp
