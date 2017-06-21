/* $Id: exec_debug_access.h,v 3.2 1995/11/17 18:53:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/exec_debug_access.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A C C E S S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_access.h,v $
 * Revision 3.2  1995/11/17  18:53:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:03  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:47:03  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug access-expression <integer>
 *
 * OBJ(int,1) = debug level
 */
EOLS	(debug_access_expr_eol, srt_debug_command, DEBUG_ACCESS_EXPR);
NUMBER	(debug_access_expr_integer, debug_access_expr_eol,
	 debug_access_expr_eol,
	 OBJ(int,1), 0, 15, "Debugging level (0=No Debugging)");
KEYWORD_DEBUG(debug_access_expr, debug_access_expr_integer, ALTERNATE,
	      OBJ(pdb,1), srt_debug_arr,
	      "access-expression", "Boolean access expression", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_access_expr
