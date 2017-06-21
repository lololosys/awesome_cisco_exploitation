/* $Id: exec_show_srb_sse.h,v 3.3 1996/02/01 23:33:18 widmer Exp $
 * $Source: /release/111/cvs/Xsys/hes/exec_show_srb_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_source_srb_sse.h
 *
 * Feb 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_srb_sse.h,v $
 * Revision 3.3  1996/02/01  23:33:18  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:16:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:18:07  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:26:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show source-bridge sse
 *
 */

EOLS	(show_source_bridge_sse_eol, show_sse, SSE_PROTOCOL_SRB);

KEYWORD_ID(show_source_bridge_sse, show_source_bridge_sse_eol, ALTERNATE,
	   OBJ(int,1), SSE_CMD_INTERNALS,
	   "sse", "SSE information", PRIV_USER|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_source_bridge_sse
