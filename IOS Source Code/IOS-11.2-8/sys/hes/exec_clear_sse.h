/* $Id: exec_clear_sse.h,v 3.3 1996/02/01 23:33:13 widmer Exp $
 * $Source: /release/111/cvs/Xsys/hes/exec_clear_sse.h,v $
 *------------------------------------------------------------------
 * exec_clear_sse.h - Clear an SSE
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_sse.h,v $
 * Revision 3.3  1996/02/01  23:33:13  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:15:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:16:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear sse 
 */
EOLS	(clear_sse_eol, clear_sse, SSE_PROTOCOL_ANY);

KEYWORD_ID(clear_sse, clear_sse_eol, ALTERNATE,
	   OBJ(int, 1), LINK_ILLEGAL,
	   "sse", "Silicon Switching Engine", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_sse
