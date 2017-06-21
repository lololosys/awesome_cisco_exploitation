/* $Id: exec_debug_sse.h,v 3.2 1995/11/17 09:15:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/exec_debug_sse.h,v $
 *------------------------------------------------------------------
 * exec_debug_sse.h - Debugging for the SSE
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_sse.h,v $
 * Revision 3.2  1995/11/17  09:15:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:49:06  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug sse
 *
 */
EOLS	(debug_sse_eol, debug_command, DEBUG_SSE);
KEYWORD_DEBUG(debug_sse, debug_sse_eol, ALTERNATE,
	      OBJ(pdb,1), sse_debug_arr,
	      "sse", "Silicon Switching Engine information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_sse
