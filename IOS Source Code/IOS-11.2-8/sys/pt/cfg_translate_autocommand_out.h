/* $Id: cfg_translate_autocommand_out.h,v 3.2 1995/11/17 18:58:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/cfg_translate_autocommand_out.h,v $
 *------------------------------------------------------------------
 * Autocomand translate subcommand
 *
 * Jan. 1995, Eman Schaffer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_translate_autocommand_out.h,v $
 * Revision 3.2  1995/11/17  18:58:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:25  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:24  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:14:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_out_autocommand_return, no_alt);

/***************************************************************
 * [no] translate <inproto> <inaddress> autocommand [<text>]
 */

STRING  (trans_out_auto_text, trans_out_autocommand_return, NONE,
        OBJ(string,5), "Appropriate EXEC command to be associated with "
        "translate command");

KEYWORD_ID (trans_out_autocommand, trans_out_auto_text, ALTERNATE,
            OBJ(int,6), TRANS_PROT_AUTOCOMMAND, "autocommand",
            "Associate a command with a translation on this connections",
            PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       trans_out_autocommand
