/* $Id: exec_erase_cmnds.h,v 3.3.4.3 1996/07/12 19:14:33 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/dev/exec_erase_cmnds.h,v $
 *------------------------------------------------------------------
 * exec_erase_cmnds.h - Erase commands. 
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_erase_cmnds.h,v $
 * Revision 3.3.4.3  1996/07/12  19:14:33  ssangiah
 * CSCdi62742:  Erase flash command missing on c7000 platform.
 * Branch: California_branch
 *
 * Revision 3.3.4.2  1996/06/05  18:57:05  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/05/07  00:37:32  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/09  22:54:39  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  09:00:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:38  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:20:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_ERASE_CMNDS_H__
#define __EXEC_ERASE_CMNDS_H__

/***************************************************************
 * erase { flash | bootflash | startup-config }
 *
 * csb->which = COPY_ERASE_FLASH,
 *		COPY_ERASE_BOOTFLASH,
 *		WRITE_ERASE
 */


EOLS	(exec_erase_startup_config_eol, write_erase, WRITE_ERASE);
KEYWORD	(exec_erase_startup_config, exec_erase_startup_config_eol,
         no_alt,
	 "startup-config", "Erase contents of configuration memory",
	 PRIV_ROOT);

EOLS    (exec_erase_flash_eol, flash_copy_command, COPY_ERASE_FLASH);
KEYWORD (exec_erase_flash, exec_erase_flash_eol, exec_erase_startup_config,
         SF, "Erase System Flash",
	 PRIV_ROOT | PRIV_INTERACTIVE);

EOLS    (exec_erase_bootflash_eol, flash_copy_command, COPY_ERASE_BOOTFLASH);
KEYWORD (exec_erase_bootflash, exec_erase_bootflash_eol, exec_erase_flash,
         BF, "Erase Bootstrap Flash",
	 PRIV_ROOT | PRIV_INTERACTIVE);
IFELSE  (ifelse_erase, exec_erase_bootflash, exec_erase_flash,
         flash_alias_match(BF));

KEYWORD (exec_erase, ifelse_erase, ALTERNATE,
         "erase", "Erase flash or configuration memory",
	 PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_erase

#endif /* __EXEC_ERASE_CMNDS_H__ */
