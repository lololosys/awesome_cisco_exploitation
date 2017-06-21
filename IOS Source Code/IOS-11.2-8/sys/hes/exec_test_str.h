/* $Id: exec_test_str.h,v 3.1.40.1 1996/08/12 16:01:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_test_str.h,v $
 *------------------------------------------------------------------
 * Test str command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_str.h,v $
 * Revision 3.1.40.1  1996/08/12  16:01:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:48:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * test str
 */
EOLNS	(priv_test_str_eol, test_all_str_interfaces);
KEYWORD	(priv_test_str, priv_test_str_eol, ALTERNATE,
	 "str", "CSC-1R/2R network interfaces", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_str
