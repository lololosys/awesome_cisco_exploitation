/* $Id: exec_show_srb.h,v 3.3.20.1 1996/08/12 16:07:55 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_show_srb.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S R B . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_srb.h,v $
 * Revision 3.3.20.1  1996/08/12  16:07:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:50  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:13  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

pdecl(show_source_bridge_eol);

LINK_TRANS(show_source_bridge_extend_here, show_source_bridge_eol);

/******************************************************************
 * show source-bridge
 *
 */
EOLNS	(show_source_bridge_eol, show_srb);

KEYWORD_ID (show_source_bridge_int, show_source_bridge_extend_here, 
	    show_source_bridge_extend_here,
	    OBJ(int,1), TRUE,
	    "interfaces", "Information about all interfaces",
	    PRIV_USER|PRIV_USER_HIDDEN);

KEYWORD_ID (show_source_bridge, show_source_bridge_int,
	    NONE,
	    OBJ(int,1), FALSE, 
	    "source-bridge", "Source-bridge parameters and statistics",
	    PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_source_bridge_test, show_source_bridge, NONE, ALTERNATE, 
	     bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_source_bridge_test
