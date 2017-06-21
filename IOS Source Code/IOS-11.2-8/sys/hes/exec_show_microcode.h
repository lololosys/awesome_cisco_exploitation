/* $Id: exec_show_microcode.h,v 3.1.40.2 1996/08/12 16:01:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_microcode.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ M I C R O C O D E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_microcode.h,v $
 * Revision 3.1.40.2  1996/08/12  16:01:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/06/05  18:57:12  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:09:24  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:25:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show microcode
 *
 */
EOLNS	(show_microcode_eol, show_ucode_in_bundle);
KEYWORD (show_microcode, show_microcode_eol, ALTERNATE,
	 "microcode", "Microcode in system",
	 PRIV_USER | PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_microcode
