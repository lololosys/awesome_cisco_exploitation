/* $Id: exec_show_llc2.h,v 3.3.20.1 1996/08/12 16:04:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/llc2/exec_show_llc2.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ L L C 2 . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_llc2.h,v $
 * Revision 3.3.20.1  1996/08/12  16:04:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:03  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:43:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:38  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show llc2 [brief]
 */

EOLNS		(show_llc2_eol, show_llc2_command);
KEYWORD_ID      (show_llc2_brief, show_llc2_eol, show_llc2_eol,
		 OBJ(int,1), TRUE, "brief", "Brief output", PRIV_USER|PRIV_USER_HIDDEN);
KEYWORD 	(show_llc2, show_llc2_brief, ALTERNATE,
		 "llc2", "IBM LLC2 circuit information",
		 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_llc2
