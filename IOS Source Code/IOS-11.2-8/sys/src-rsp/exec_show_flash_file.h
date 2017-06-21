/* $Id: exec_show_flash_file.h,v 3.3.56.1 1996/08/12 16:07:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/exec_show_flash_file.h,v $
 *------------------------------------------------------------------
 * Show flash command for flash file system.
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_flash_file.h,v $
 * Revision 3.3.56.1  1996/08/12  16:07:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/25  22:51:57  yanke
 * Add show flash device.
 *
 * CSCdi42314:  slave sync config command doesnt update slave config
 *
 * Revision 3.2  1995/11/17  18:46:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:40  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:23:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS (exec_show_flash_id_eol, show_flash_file_command);

STRING (exec_show_flash_id, exec_show_flash_id_eol, NONE,
	OBJ(string, 1),
	"device id");

TEST_MULTIPLE_FUNCS(exec_show_flash_test, exec_show_flash_id,
	NONE, exec_show_flash_id_eol);

KEYWORD_ID (exec_show_flash_all, exec_show_flash_test, exec_show_flash_test,
	OBJ(int, 1), SHOW_FLASH_FILE_ALL,
	"all", "Show all possible flash device information", PRIV_USER);

KEYWORD_ID (exec_show_flash_chip, exec_show_flash_test, exec_show_flash_all,
	OBJ(int, 1), SHOW_FLASH_FILE_CHIPS,
	"chips", "Show flash chip information", PRIV_USER);

KEYWORD_ID (exec_show_flash_device, exec_show_flash_test, exec_show_flash_chip,
	OBJ(int, 1), SHOW_FLASH_FILE_DEV,
	"devices", "Show all file system devices", PRIV_USER);

KEYWORD_ID (exec_show_flash_filesys, exec_show_flash_test, exec_show_flash_device,
	OBJ(int, 1), SHOW_FLASH_FILE_FILESYS,
	"filesys", "Show file system information", PRIV_USER);

KEYWORD (exec_show_flash_file, exec_show_flash_filesys, ALTERNATE,
	"flash", "Flash device information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_show_flash_file
