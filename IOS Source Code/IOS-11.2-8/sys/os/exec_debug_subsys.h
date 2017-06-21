/* $Id: exec_debug_subsys.h,v 3.1 1996/02/16 00:43:13 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/exec_debug_subsys.h,v $
 *------------------------------------------------------------------
 * exec_debug_subsys.h - subsystem parser chain debugging
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_subsys.h,v $
 * Revision 3.1  1996/02/16  00:43:13  hampton
 * Migrate more files out of the parser directory.  [CSCdi49139]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:49:11  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug subsys
 *
 */
EOLS	(debug_subsys_eol, debug_command, DEBUG_SUBSYS);
KEYWORD_DEBUG(debug_subsys, debug_subsys_eol, ALTERNATE,
	      OBJ(pdb,1), os_debug_arr,
	      "subsystem", "", PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_subsys
