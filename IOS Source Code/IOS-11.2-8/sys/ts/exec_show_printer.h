/* $Id: exec_show_printer.h,v 3.1.40.1 1996/08/12 16:08:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_show_printer.h,v $
 *------------------------------------------------------------------
 * "Show printers" command for lpd printers
 *
 * October 1994, Bill Westfield
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_printer.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  05:14:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:47:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:25:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * show printers
 * 
 */

EOLNS	(show_lpt_eol, show_printers);
KEYWORD (show_lpt, show_lpt_eol, ALTERNATE,
	 "printers", "Show LPD printer information", 
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_lpt
