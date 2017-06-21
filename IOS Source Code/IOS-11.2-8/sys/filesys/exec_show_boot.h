/* $Id: exec_show_boot.h,v 3.1.40.1 1996/08/12 16:01:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/exec_show_boot.h,v $
 *------------------------------------------------------------------
 * Add 'show boot' commands.
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_boot.h,v $
 * Revision 3.1.40.1  1996/08/12  16:01:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:57:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:47:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show boot
 *
 */
EOLNS	(show_boot_eol, show_boot_command);
KEYWORD (show_boot, show_boot_eol, ALTERNATE,
	 "boot", "Boot and related environment variable", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_boot
