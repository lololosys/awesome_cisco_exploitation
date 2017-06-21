/* $Id: exec_verify_cmnds.h,v 3.3.4.1 1996/05/07 00:37:36 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/dev/exec_verify_cmnds.h,v $
 *------------------------------------------------------------------
 * exec_verify_cmnds.h - Verify commands. 
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_verify_cmnds.h,v $
 * Revision 3.3.4.1  1996/05/07  00:37:36  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/09  22:54:40  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  09:00:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:42  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_VERIFY_CMNDS_H__
#define __EXEC_VERIFY_CMNDS_H__

/***************************************************************
 * verify flash
 * verify bootflash
 *
 * csb->which = COPY_VERIFY_FLASH,
 *		COPY_VERIFY_BOOTFLASH
 */


/* extend "verify file_id" from here */
LINK_TRANS(exec_verify_file_extend_here, no_alt);

EOLS    (exec_verify_flash_eol, flash_copy_command, COPY_VERIFY_FLASH);
KEYWORD (exec_verify_flash, exec_verify_flash_eol,
         exec_verify_file_extend_here,
         SF, "Verify System Flash file", PRIV_OPR);

EOLS    (exec_verify_bootflash_eol, flash_copy_command, COPY_VERIFY_BOOTFLASH);
KEYWORD (exec_verify_bootflash, exec_verify_bootflash_eol, NONE,
         BF, "Verify Bootstrap Flash file", PRIV_OPR);
ASSERT	(ifelse_verify, exec_verify_bootflash, exec_verify_flash,
	 flash_alias_match(BF));

/*
 * The new flash file system introduced with the RSP expects a file name
 * as the argument to the verify command.
 */
IFELSE  (ifelse_verify_test, ifelse_verify, exec_verify_file_extend_here,
         !reg_invoke_rsp_flash_file_system());

KEYWORD (exec_verify, ifelse_verify_test, ALTERNATE,
         "verify", "Verify checksum of a Flash file",
	 PRIV_OPR | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_verify

#endif /* __EXEC_VERIFY_CMNDS_H__ */
