/* $Id: cfg_boot_helper.h,v 3.1.40.1 1996/08/12 16:01:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/cfg_boot_helper.h,v $
 *------------------------------------------------------------------
 * Add boot helper command. 
 *
 * March 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_boot_helper.h,v $
 * Revision 3.1.40.1  1996/08/12  16:01:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:57:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:42:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:16:22  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:01:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS (conf_boot_helper_file_eol, boot_helper_file_command);

STRING (conf_boot_helper_file, conf_boot_helper_file_eol, no_alt,
	OBJ(string, 1),
	"file id of the Boot helper image");

TESTVAR	(conf_boot_helper_noprefix, conf_boot_helper_file_eol, NONE,
	NONE, NONE, conf_boot_helper_file, sense, FALSE);

NVGENS	(conf_boot_helper_nvgen, conf_boot_helper_noprefix,
	boot_helper_file_command, 0);

KEYWORD (conf_boot_helper_kwd, conf_boot_helper_nvgen, ALTERNATE,
	 "bootldr", "Boot loader (RxBoot) image name", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_boot_helper_kwd
