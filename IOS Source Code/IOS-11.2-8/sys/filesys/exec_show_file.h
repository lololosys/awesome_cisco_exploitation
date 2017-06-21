/* $Id: exec_show_file.h,v 3.1.40.2 1996/08/12 16:01:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/exec_show_file.h,v $
 *------------------------------------------------------------------
 * Parser header file for 'show file' command.
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_file.h,v $
 * Revision 3.1.40.2  1996/08/12  16:01:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/05/07  00:39:11  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:47:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:59  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS (exec_show_file_eol, show_file_command);

TEXT	(exec_show_file_id, exec_show_file_eol, no_alt,
	 OBJ(string, 1), "Name of the configuration file");

KEYWORD (exec_show_file, exec_show_file_id, ALTERNATE,
	 "file", "Show a configuration file", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_show_file
