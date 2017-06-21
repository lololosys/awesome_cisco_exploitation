/* $Id: exec_copy_file.h,v 3.1.40.2 1996/05/07 00:38:42 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/filesys/exec_copy_file.h,v $
 *------------------------------------------------------------------
 * Add 'copy file-id' commands.
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_copy_file.h,v $
 * Revision 3.1.40.2  1996/05/07  00:38:42  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/15  21:13:27  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/11/24  22:49:23  yanke
 * Take keywords in 'copy file-id' family.
 *
 * CSCdi39749:  Tftp parameters in Copy tftp xx is inconsistent for device
 * or file
 *
 * Revision 3.2  1995/11/17  18:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:16:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	copy file-id file-id
 */	
EOLS (exec_copy_file_id_id_eol, copy_file_command, COPY_FILE_ID_ID);

STRING (exec_copy_file_id_id, exec_copy_file_id_id_eol, NONE,
	OBJ(string, 2),
	"Destination device or file name");

TEST_MULTIPLE_FUNCS(exec_copy_file_id_id_test,
	exec_copy_file_id_id, NONE, no_alt);

/*
 *	copy file-id startup-config
 */
EOLS (exec_copy_file_id_sconfig_eol, copy_file_command, COPY_FILE_ID_SCONFIG);

KEYWORD (exec_copy_file_id_sconfig, exec_copy_file_id_sconfig_eol,
	exec_copy_file_id_id_test,
	"startup-config", "Copy to startup configuration", PRIV_ROOT);

/*
 *	copy file-id running-config
 */
EOLS (exec_copy_file_id_rconfig_eol, copy_file_command, COPY_FILE_ID_RCONFIG);

KEYWORD (exec_copy_file_id_rconfig, exec_copy_file_id_rconfig_eol,
	exec_copy_file_id_sconfig,
	"running-config", "Update (merge with) current system configuration",
	PRIV_ROOT);

/*
 *	copy file-id rcp
 */
EOLS (exec_copy_file_id_rcp_eol, copy_file_command, COPY_FILE_ID_RCP);

KEYWORD (exec_copy_file_id_rcp, exec_copy_file_id_rcp_eol,
	exec_copy_file_id_rconfig,
	"rcp", "Copy to an rcp server", PRIV_ROOT);

/*
 *	copy file-id tftp
 */
EOLS (exec_copy_file_id_tftp_eol, copy_file_command, COPY_FILE_ID_TFTP);

KEYWORD (exec_copy_file_id_tftp, exec_copy_file_id_tftp_eol,
	exec_copy_file_id_rcp,
	"tftp", "Copy to a TFTP server", PRIV_ROOT);

/*
 *	copy file-id flash
 */
EOLS (exec_copy_file_id_flash_eol, copy_file_command, COPY_FILE_ID_FLASH);

KEYWORD (exec_copy_file_id_flash, exec_copy_file_id_flash_eol,
	exec_copy_file_id_tftp,
	"flash", "Copy to flash devices", PRIV_ROOT);

/*
 *	copy file-id ..
 */
STRING (exec_copy_file_id, exec_copy_file_id_flash, no_alt,
	OBJ(string, 1),
	"Source file name");

TEST_MULTIPLE_FUNCS(exec_copy_file, exec_copy_file_id, NONE, no_alt);

#undef	ALTERNATE
#define	ALTERNATE	exec_copy_file
