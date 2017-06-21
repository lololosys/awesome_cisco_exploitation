/* $Id: exec_show_async.h,v 3.3.20.1 1996/08/12 16:08:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_show_async.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A S Y N C . H
 *
 * March 18, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_async.h,v $
 * Revision 3.3.20.1  1996/08/12  16:08:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:10:00  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:54:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:57  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:22:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * show async status
 */

EOLNS (show_async_status_eol, show_slip);
KEYWORD (show_async_status, show_async_status_eol, no_alt,
	 "status", "Terminal line status and summary counters",
	 PRIV_USER);

/******************************************************************
 * show async bootp
 * 
 */

EOLNS	(show_async_bootp_eol, show_asyncbootp);
KEYWORD (show_async_bootp, show_async_bootp_eol, show_async_status,
	 "bootp", "Extended BOOTP Information", PRIV_USER);

KEYWORD (show_async, show_async_bootp, ALTERNATE,
	 "async", "Information on terminal lines used as router interfaces",
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_async
