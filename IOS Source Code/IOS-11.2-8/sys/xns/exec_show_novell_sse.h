/* $Id: exec_show_novell_sse.h,v 3.3.20.1 1996/08/16 00:14:27 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_novell_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_novell_sse.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_novell_sse.h,v $
 * Revision 3.3.20.1  1996/08/16  00:14:27  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:35:12  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:21:43  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:25:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ipx sse memory|rewrite
 * show ipx sse shadow memory [<address>]
 *
 * OBJ(int,1) = SSE_CMD_INTERNALS, SSE_CMD_MEMORY, SSE_CMD_REWRITE
 * OBJ(int,2) = <address>, -1 if no address
 */

EOLS	(show_novell_sse_eol, show_sse, SSE_PROTOCOL_IPX);

KEYWORD_ID(show_novell_sse_rewrite, show_novell_sse_eol, no_alt,
	   OBJ(int,1), SSE_CMD_REWRITE,
	   "rewrite", "SSE rewrite table", PRIV_USER); 

KEYWORD_ID(show_novell_sse_memory, show_novell_sse_eol,
	   show_novell_sse_rewrite,
	   OBJ(int,1), SSE_CMD_INTERNALS,
	   "memory", "SSE internal memory structures", PRIV_USER); 

KEYWORD (show_novell_sse, show_novell_sse_memory, ALTERNATE,
	 "sse", "SSE caches", PRIV_USER|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_novell_sse
