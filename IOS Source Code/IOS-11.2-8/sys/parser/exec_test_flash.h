/* $Id: exec_test_flash.h,v 3.3.20.1 1996/08/12 16:06:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_test_flash.h,v $
 *------------------------------------------------------------------
 * Test flash command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_flash.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:32  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:48:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * test flash
 */
EOLNS	(priv_test_flash_eol, test_flash);
KEYWORD	(priv_test_flash, priv_test_flash_eol, ALTERNATE,
	 "flash", "Mci and envm flash eprom interfaces", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_flash
