/* $Id: exec_copy_tftp_modem.h,v 3.1.14.2 1996/07/14 18:29:43 elem Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_copy_tftp_modem.h,v $
 *------------------------------------------------------------------
 * exec_copy_tftp_modem.h Parser for copy tftp modem command.
 *
 * May 96, Eddy Lem
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_copy_tftp_modem.h,v $
 * Revision 3.1.14.2  1996/07/14  18:29:43  elem
 * CSCdi58543:  duplicate copy tftp/flash/rcp commands in parser
 * Branch: California_branch
 * Use PRIV_DUPLICATE to remove duplication.
 *
 * Revision 3.1.14.1  1996/06/16  21:14:12  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/19  20:07:07  vnguyen
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* copy tftp modem */

EOLS    (exec_copy_tftp_modem_eol, mai_command_parser, COPY_TFTP_MODEM);
KEYWORD (exec_copy_tftp_modem, exec_copy_tftp_modem_eol,
        NONE,
        "modem", "Download modem firmware to modem", PRIV_OPR);
KEYWORD (exec_copy_tftp_modem_command, exec_copy_tftp_modem,
        NONE,
        "tftp", "", PRIV_DUPLICATE | PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       exec_copy_tftp_modem_command

