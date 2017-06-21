/* $Id: cfg_line_autohangup.h,v 3.2 1995/11/17 18:53:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_autohangup.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ A U T O H A N G U P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_autohangup.h,v $
 * Revision 3.2  1995/11/17  18:53:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:41  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] autohangup
 */
EOLS	(line_autohangup_eol, asm_line_commands, PARSER_LINE_AUTOHANGUP);
KEYWORD	(line_autohangup, line_autohangup_eol, ALTERNATE,
	 "autohangup", "Automatically hangup when last connection closes",
	 PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_autohangup
