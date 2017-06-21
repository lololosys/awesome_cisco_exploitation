/* $Id: exec_copy_sse.h,v 3.3.20.1 1996/08/12 16:01:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_copy_sse.h,v $
 *------------------------------------------------------------------
 * exec_copy_sse.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_copy_sse.h,v $
 * Revision 3.3.20.1  1996/08/12  16:01:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:14  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:15:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:17:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * copy sse memory tftp
 * copy sse rewrite tftp
 * OBJ(int,1) = SSE_CMD_MEMORY | SSE_CMD_REWRITE ;
 */

EOLNS    (exec_copy_sse_eol, copy_sse_command);
KEYWORD (exec_copy_sse_tftp, exec_copy_sse_eol, no_alt,
	 "tftp", "Copy to a tftp server", PRIV_ROOT);

KEYWORD_ID(exec_copy_sse_rewrite, exec_copy_sse_tftp, no_alt,
	   OBJ(int,1), SSE_CMD_REWRITE,
	   "rewrite", "Copy SSE rewrite memory", PRIV_ROOT);

KEYWORD_ID(exec_copy_sse_mem, exec_copy_sse_tftp, exec_copy_sse_rewrite,
	   OBJ(int,1), SSE_CMD_MEMORY,
	   "memory", "Copy SSE memory", PRIV_ROOT);

KEYWORD (exec_copy_sse, exec_copy_sse_mem, ALTERNATE,
	"sse", "Copy SSE data structures", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_copy_sse

