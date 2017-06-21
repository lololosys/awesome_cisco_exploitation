/* $Id: exec_copy_flash_modem.h,v 3.1.14.2 1996/07/14 18:29:40 elem Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_copy_flash_modem.h,v $
 *------------------------------------------------------------------
 * exec_copy_flash_modem.h Parser for copy flash modem command.
 *
 * May 96, Eddy Lem
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_copy_flash_modem.h,v $
 * Revision 3.1.14.2  1996/07/14  18:29:40  elem
 * CSCdi58543:  duplicate copy tftp/flash/rcp commands in parser
 * Branch: California_branch
 * Use PRIV_DUPLICATE to remove duplication.
 *
 * Revision 3.1.14.1  1996/06/16  21:14:09  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/19  20:07:03  vnguyen
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* copy flash modem */

EOLS    (exec_copy_flash_modem_eol, mai_command_parser, COPY_FLASH_MODEM);
KEYWORD (exec_copy_flash_modem, exec_copy_flash_modem_eol,
        NONE,
        "modem", "Download modem firmware to modem", PRIV_OPR);
KEYWORD (exec_copy_flash_modem_command, exec_copy_flash_modem,
        NONE,
        "flash", "", PRIV_DUPLICATE | PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       exec_copy_flash_modem_command

