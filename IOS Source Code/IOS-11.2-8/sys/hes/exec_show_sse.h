/* $Id: exec_show_sse.h,v 3.3.20.2 1996/06/05 18:57:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_sse.h
 *
 * Oct., 1992 Tony Li
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_sse.h,v $
 * Revision 3.3.20.2  1996/06/05  18:57:14  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/21  09:47:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/01  23:33:19  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:16:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:26:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show sse interface
 * show sse match
 * show sse memory [<address>]
 * show sse memory free
 * show sse rewrite [<address>]
 * show sse summary
 *
 * OBJ(int,1) = SSE_CMD_SUMMARY, SSE_CMD_MEMORY, SSE_CMD_REWRITE,
 * 		SSE_CMD_MEMFREE, SSE_CMD_MATCH
 * OBJ(int,2) = <address>, -1 if no address
 */

EOLS	(show_sse_eol, show_sse, SSE_PROTOCOL_ANY);

KEYWORD_ID(show_sse_summary, show_sse_eol, no_alt,
	   OBJ(int,1), SSE_CMD_SUMMARY,
	   "summary", "SSE summary statistics", PRIV_USER); 

SET     (show_sse_rewrite_noaddr, show_sse_eol, OBJ(int,2), -1);
NUMBER  (show_sse_rewrite_addr, show_sse_eol, show_sse_rewrite_noaddr,
	 OBJ(int,2), 0, SSE_MAX_ADDRESS, "Dump memory starting at <address>");
KEYWORD_ID(show_sse_rewrite, show_sse_rewrite_addr, show_sse_summary,
	   OBJ(int,1), SSE_CMD_REWRITE,
	   "rewrite", "SSE board rewrite memory", PRIV_USER); 

SET     (show_sse_memory_noaddr, show_sse_eol, OBJ(int,2), -1);

NUMBER  (show_sse_memory_addr, show_sse_eol, show_sse_memory_noaddr,
	 OBJ(int,2), 0, SSE_MAX_ADDRESS, "Dump memory starting at <address>");

KEYWORD_ID(show_sse_memory_free, show_sse_eol, show_sse_memory_addr,
	   OBJ(int,1), SSE_CMD_MEMFREE,
	   "free", "Show memory allocation map", PRIV_USER);

KEYWORD_ID(show_sse_memory, show_sse_memory_free, show_sse_rewrite,
	   OBJ(int,1), SSE_CMD_MEMORY,
	   "memory", "SSE board memory", PRIV_USER); 

KEYWORD_ID (show_sse_match, show_sse_eol, show_sse_memory,
	    OBJ(int, 1), SSE_CMD_MATCH,
	    "match", "SSE match node cache", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID(show_sse_interface, show_sse_eol, show_sse_match,
	   OBJ(int,1), SSE_CMD_INTERFACE,
	   common_str_interface, "SSE interface memory", PRIV_USER|PRIV_HIDDEN);

KEYWORD (show_sse, show_sse_interface, ALTERNATE,
	 "sse", "Silicon Switching Engine", PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_sse
