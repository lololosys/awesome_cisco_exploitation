/* $Id: exec_bootflash_cmnds.h,v 3.2.62.1 1996/06/05 18:57:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_bootflash_cmnds.h,v $
 *------------------------------------------------------------------
 * exec_bootflash_cmnds.h - Commands specific to bootstrap flash.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_bootflash_cmnds.h,v $
 * Revision 3.2.62.1  1996/06/05  18:57:22  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:45:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:25  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:15:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_BOOTFLASH_CMNDS_H__
#define __EXEC_BOOTFLASH_CMNDS_H__

/***************************************************************
 * bootflash show [all | err]
 * bootflash backup
 * bootflash update
 * bootflash verify
 * bootflash erase
 *
 * csb->which = COPY_TFTP_BOOTFLASH,
 *		COPY_BOOTFLASH_TFTP,
 *		COPY_ERASE_BOOTFLASH, COPY_BOOTFLASH_VERIFY,
 *              SHOW_BOOTFLASH
 */

#define BF "bootflash"

EOLS    (exec_bootflash_show_eol, show_bootflash, SHOW_BOOTFLASH);
KEYWORD_ID(exec_bootflash_show_err, exec_bootflash_show_eol, 
           exec_bootflash_show_eol, OBJ(int,3), TRUE,
           "err", "show boot flash chip erase and write retries", 
	   PRIV_USER);
KEYWORD_ID(exec_bootflash_show_all, exec_bootflash_show_eol, 
           exec_bootflash_show_err, OBJ(int,2), TRUE,
           "all", "show info on each boot flash chip", PRIV_USER);
KEYWORD (exec_bootflash_show, exec_bootflash_show_all, no_alt,
         "show", "Bootstrap flash information", PRIV_USER|PRIV_USER_HIDDEN);

EOLS	(exec_bflash_erase_eol, bootflash_copy_command, COPY_ERASE_BOOTFLASH);
KEYWORD	(exec_bootflash_erase, exec_bflash_erase_eol, exec_bootflash_show,
	 "erase", "Erase bootstrap flash",
	 PRIV_ROOT);

EOLS	(exec_bflash_verify_eol, bootflash_copy_command, COPY_VERIFY_BOOTFLASH);
KEYWORD	(exec_bootflash_verify, exec_bflash_verify_eol, exec_bootflash_erase,
	 "verify", "Verify checksum of file in bootstrap flash",
	 PRIV_ROOT);

EOLS	(exec_bflash_update_eol, bootflash_copy_command, COPY_TFTP_BOOTFLASH);
KEYWORD	(exec_bootflash_update, exec_bflash_update_eol, exec_bootflash_verify,
	 "update", "Copy from a tftp server to bootstrap flash",
	 PRIV_ROOT);

EOLS	(exec_bflash_backup_eol, bootflash_copy_command, COPY_BOOTFLASH_TFTP);
KEYWORD	(exec_bootflash_backup, exec_bflash_backup_eol, exec_bootflash_update,
	 "backup", "Copy from bootstrap flash to a tftp server",
	 PRIV_ROOT);

KEYWORD	(exec_bootflash, exec_bootflash_backup, ALTERNATE,
	 BF, "Bootstrap Flash functions", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_bootflash

#endif /* __EXEC_BOOTFLASH_CMNDS_H__ */
