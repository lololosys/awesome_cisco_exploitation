/* $Id: exec_show_accounting.h,v 3.1.40.1 1996/08/12 16:05:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_accounting.h,v $
 *------------------------------------------------------------------
 * exec_show_accounting command
 *
 * April 1995, Bill Westfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_accounting.h,v $
 * Revision 3.1.40.1  1996/08/12  16:05:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show accounting
 * 
 */

EOLNS	(show_acct_eol, show_accounting);
KEYWORD (show_acct, show_acct_eol, ALTERNATE,
	 "accounting", "Accounting data for active sessions",
	 PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_acct
