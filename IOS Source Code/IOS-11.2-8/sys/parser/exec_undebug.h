/* $Id: exec_undebug.h,v 3.3 1996/03/09 22:56:53 widmer Exp $
 * $Source: /release/111/cvs/Xsys/parser/exec_undebug.h,v $
 *------------------------------------------------------------------
 * E X E C _ U N D E B U G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_undebug.h,v $
 * Revision 3.3  1996/03/09  22:56:53  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:48:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************/
KEYWORD_ID(exec_undebug, exec_debug_commands, ALTERNATE,
	   sense, FALSE,
	   "undebug", "Disable debugging functions (see also 'debug')",
	   PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_undebug
