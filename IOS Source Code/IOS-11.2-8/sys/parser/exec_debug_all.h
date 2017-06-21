/* $Id: exec_debug_all.h,v 3.2.62.1 1996/08/12 16:06:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_debug_all.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A L L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_all.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:17:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug all
 *
 */
EOLNS	(debug_all_eol, debug_all_cmd);
KEYWORD (debug_all, debug_all_eol, ALTERNATE,
	 "all", "Enable all debugging", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	debug_all
