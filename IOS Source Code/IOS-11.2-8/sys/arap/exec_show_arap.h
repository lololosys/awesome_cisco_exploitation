/* $Id: exec_show_arap.h,v 3.3.20.1 1996/08/12 16:00:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/arap/exec_show_arap.h,v $
 *------------------------------------------------------------------
 * ARAP Show Commands
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_arap.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:08  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:39:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:54:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:34:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:22:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show arap
 * 
 */

EOLNS	(show_arap_eol, show_arap);
SET	(show_arap_set, show_arap_eol, OBJ(int,1), MAXUINT);
TTYLINE (show_arap_line, show_arap_eol, show_arap_set,
	OBJ(int,1), "Line number");
KEYWORD (show_arap, show_arap_line, ALTERNATE,
	 "arap", "Show Appletalk Remote Access statistics", 
						PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_arap

