/* $Id: exec_file.h,v 3.1.40.2 1996/08/12 16:01:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/exec_file.h,v $
 *------------------------------------------------------------------
 * File system user commands
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_file.h,v $
 * Revision 3.1.40.2  1996/08/12  16:01:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/05/07  00:39:06  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/20  01:18:27  thille
 * CSCdi37375:  undelete makes un abbreviation for undebug ambiguous
 * Fix it.
 *
 * Revision 2.2  1995/06/21  18:17:43  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:20:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*-------------------------------------------------------------------------
*
*	pwd command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_pwd_eol, pwd_command);

KEYWORD (exec_pwd, exec_pwd_eol, ALTERNATE,
	"pwd", "Display current device", PRIV_USER);


/*-------------------------------------------------------------------------
*
*	cd command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_cd_eol, cd_command);

STRING (exec_cd_dev, exec_cd_eol, exec_cd_eol,
	OBJ(string, 1),
	"Device name");

KEYWORD (exec_cd, exec_cd_dev, exec_pwd,
	"cd", "Change current device", PRIV_USER);


/*-------------------------------------------------------------------------
*
*	Format command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_format_eol, format_command);

STRING (exec_format_dev_monlib, exec_format_eol, exec_format_eol,
	OBJ(string, 2),
	"Name of the device that contains monlib");

TEST_MULTIPLE_FUNCS(exec_format_dev_monlib_test,
	exec_format_dev_monlib, NONE, NONE);

STRING (exec_format_dev_id, exec_format_dev_monlib_test, no_alt,
	OBJ(string, 1),
	"Name of device to be formatted");

NUMBER  (exec_format_dev_spare, exec_format_dev_id, no_alt,
         OBJ(int, 2), 0, 16, "Spare sector number");

KEYWORD_ID(exec_format_dev, exec_format_dev_spare, exec_format_dev_id,
           OBJ(int, 1), TRUE,
           "spare", "Number of reserved spare sectors",
           PRIV_ROOT);

KEYWORD (exec_format, exec_format_dev, exec_cd,
	"format", "Format a device", PRIV_ROOT);


/*-------------------------------------------------------------------------
*
*	Squeeze command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_squeeze_eol, squeeze_command);

STRING (exec_squeeze_id, exec_squeeze_eol, no_alt,
	OBJ(string, 1),
	"Name of device to be squeezed");

KEYWORD (exec_squeeze, exec_squeeze_id, exec_format,
	"squeeze", "Squeeze a device", PRIV_ROOT);


/*-------------------------------------------------------------------------
*
*	Undelete command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_undelete_eol, undelete_command);

STRING (exec_undelete_dev_id, exec_undelete_eol, exec_undelete_eol,
	OBJ(string, 1), "Device name");

NUMBER (exec_undelete_index, exec_undelete_dev_id, no_alt,
	OBJ(int, 1), 0, 700000, "File index");

KEYWORD_MM (exec_undelete, exec_undelete_index, exec_squeeze,
	    "undelete", "Undelete a file", PRIV_ROOT, 5);


/*-------------------------------------------------------------------------
*
*	Delete command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_delete_eol, delete_command);

STRING (exec_delete_id, exec_delete_eol, no_alt,
	OBJ(string, 1),
	"Name of file to be deleted");

KEYWORD (exec_delete, exec_delete_id, exec_undelete,
	"delete", "Delete a file", PRIV_ROOT);


/*-------------------------------------------------------------------------
*
*	Dir command.
*
*-------------------------------------------------------------------------*/
EOLNS (exec_dir_eol, dir_command);

STRING (exec_dir_id, exec_dir_eol, exec_dir_eol,
	OBJ(string, 1),
	"File name");

TEST_MULTIPLE_FUNCS(exec_dir_test2, exec_dir_id, NONE, NONE);

KEYWORD_ID (exec_dir_all, exec_dir_test2, exec_dir_test2,
	OBJ(int, 1), DIR_ALL,
	"/all", "List all files on given device", PRIV_ROOT);

KEYWORD_ID (exec_dir_deleted, exec_dir_test2, exec_dir_all,
	OBJ(int, 1), DIR_DELETED,
	"/deleted", "List only deleted files on given device", PRIV_ROOT);

KEYWORD_ID (exec_dir_long, exec_dir_test2, exec_dir_deleted,
	OBJ(int, 1), DIR_LONG,
	"/long", "List files in long format on given device", PRIV_ROOT);

KEYWORD (exec_dir, exec_dir_long, exec_delete,
	"dir", "List files on given device", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_dir
