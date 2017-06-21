/* $Id: exec_copy_flash.h,v 3.3.4.1 1996/04/15 21:13:21 bcole Exp $
 * $Source: /release/112/cvs/Xsys/dev/exec_copy_flash.h,v $
 *------------------------------------------------------------------
 * exec_copy_flash.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_copy_flash.h,v $
 * Revision 3.3.4.1  1996/04/15  21:13:21  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/09  22:54:38  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  09:00:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:38  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:16:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * copy bootflash { rcp | running-config | startup-config | tftp }
 * copy flash { rcp | running-config | startup-config | tftp }
 * copy rcp { bootflash | flash | running-config | startup-config }
 * copy running-config { bootflash | flash | rcp | startup-config | tftp }
 * copy startup-config { bootflash | flash | rcp | running-config | tftp }
 * copy tftp { bootflash | flash | running-config | startup-config }
 *
 * Obsolete but supported:
 * copy erase { bootflash | flash }
 * copy verify
 *
 * csb->which = COPY_TFTP_CONFIG, COPY_TFTP_FLASH,
 *		COPY_CONFIG_TFTP, COPY_FLASH_TFTP
 *		COPY_ERASE_FLASH, COPY_VERIFY_FLASH
 *              COPY_MOP_FLASH, COPY_RCP_FLASH, COPY_FLASH_RCP,
 *              COPY_RCP_CONFIG, COPY_CONFIG_RCP
 *
 */

/*
 * The following are obsolete -- do not extend
 */

/* copy verify flash */
EOLS    (exec_copy_verify_eol, flash_copy_command, COPY_VERIFY_FLASH);
KEYWORD (exec_copy_verify_flash, exec_copy_verify_eol, no_alt,
         SF, "Verify system flash file checksum", PRIV_USER|PRIV_HIDDEN);

/* copy verify bootflash */
EOLS    (exec_copy_verify_bflash_eol, flash_copy_command,
         COPY_VERIFY_BOOTFLASH);
KEYWORD (exec_copy_verify_bflash, exec_copy_verify_bflash_eol,
         exec_copy_verify_flash,
         BF, "Verify boot flash file checksum", PRIV_USER);
IFELSE  (ifelse_verify_flash, exec_copy_verify_bflash, exec_copy_verify_flash,
         flash_alias_match(BF));
KEYWORD (exec_copy_verify, ifelse_verify_flash, NONE,
         "verify", "Verify checksum of flash memory file",
	 PRIV_USER | PRIV_HIDDEN | PRIV_INTERACTIVE);

/* copy erase flash */
EOLS    (exec_copy_erase_flash_eol, flash_copy_command, COPY_ERASE_FLASH);
KEYWORD(exec_copy_erase_flash, exec_copy_erase_flash_eol, no_alt,
        SF, "Erase system flash", PRIV_ROOT|PRIV_HIDDEN);

/* copy erase bootflash */
EOLS    (exec_copy_erase_bflash_eol, flash_copy_command,
         COPY_ERASE_BOOTFLASH);
KEYWORD(exec_copy_erase_bflash, exec_copy_erase_bflash_eol,
        exec_copy_erase_flash,
        BF, "Erase boot flash", PRIV_ROOT);
IFELSE  (ifelse_erase_flash, exec_copy_erase_bflash, exec_copy_erase_flash,
         flash_alias_match(BF));
KEYWORD (exec_copy_erase, ifelse_erase_flash, exec_copy_verify,
        "erase", "Erase flash memory", PRIV_ROOT|PRIV_HIDDEN);

/*
 * The preceding are obsolete -- do not extend
 */

/*----------------------** copy flash commands **------------------------*/

/* extend "copy flash ..." from here */
LINK_TRANS(exec_copy_flash_file_extend_here, no_alt);
LINK_TRANS(exec_copy_flash_extend_here, exec_copy_flash_file_extend_here);
KEYWORD (exec_copy_flash, exec_copy_flash_extend_here, exec_copy_erase,
        SF, "Copy from system flash", PRIV_OPR);

/* extend "copy bootflash ..." from here */
LINK_TRANS(exec_copy_bflash_file_extend_here, no_alt);
LINK_TRANS(exec_copy_bflash_extend_here, exec_copy_bflash_file_extend_here);
KEYWORD (exec_copy_bflash, exec_copy_bflash_extend_here, exec_copy_flash,
        BF, "Copy from boot flash", PRIV_OPR);
IFELSE  (ifelse_copy_flash, exec_copy_bflash, exec_copy_flash,
         flash_alias_match(BF));

/*----------------------** copy flash commands **------------------------*/

#undef  ALTERNATE
#define ALTERNATE       ifelse_copy_flash
