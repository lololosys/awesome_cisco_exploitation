/* $Id: exec_show_chunk.h,v 3.2.62.1 1996/08/12 16:06:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_chunk.h,v $
 *------------------------------------------------------------------
 * exec_show_chunk.h
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_chunk.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/03  21:48:33  hampton
 * Chunk manager should record chunk usage high water mark. [CSCdi38207]
 *
 * Revision 2.1  1995/06/07 22:22:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show chunk
 *
 */

EOLNS	   (show_chunk_eol, chunk_display);
KEYWORD_ID (show_chunk_summary, show_chunk_eol, show_chunk_eol,
	    OBJ(int,1), TRUE,
	    "summary", NULL /*"Summary Stats"*/, PRIV_OPR);

KEYWORD (show_chunk, show_chunk_summary, ALTERNATE,
	 "chunk", NULL /*"Chunk Manager Status"*/, PRIV_OPR | PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_chunk
