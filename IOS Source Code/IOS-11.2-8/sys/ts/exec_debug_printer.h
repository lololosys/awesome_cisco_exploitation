/* $Id: exec_debug_printer.h,v 3.1.40.1 1996/08/12 16:08:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_debug_printer.h,v $
 *------------------------------------------------------------------
 * exec_debug_printer.h  Debugging for LPD printer support
 *
 * October 1994, Bill Westfield
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_printer.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  05:14:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:48:41  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug printer
 *
 */
EOLNS	(debug_printer_eol, printer_debug_cmd);
KEYWORD (debug_printer, debug_printer_eol, ALTERNATE,
	 "printer", "LPD printer protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_printer
