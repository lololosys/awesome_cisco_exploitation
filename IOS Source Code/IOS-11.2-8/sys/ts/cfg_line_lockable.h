/* $Id: cfg_line_lockable.h,v 3.2 1995/11/17 18:53:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_lockable.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L O C K A B L E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_lockable.h,v $
 * Revision 3.2  1995/11/17  18:53:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:47  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:10:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] lockable
 */
EOLS	(line_lockable_eol, asm_line_commands, PARSER_LINE_LOCKABLE);
KEYWORD	(line_lockable, line_lockable_eol, ALTERNATE,
	 "lockable", "Allow users to lock a line", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	line_lockable
