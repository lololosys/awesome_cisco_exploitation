/* $Id: cfg_line_session-limit.h,v 3.2 1995/11/17 18:53:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_session-limit.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S E S S I O N - L I M I T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_session-limit.h,v $
 * Revision 3.2  1995/11/17  18:53:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:49  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:10:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * session-limit <session-number>
 *
 * OBJ(int,1) = <session-number>
 */

EOLS	(cfg_line_slimit_eol, asm_line_commands, LINE_SESSION_LIMIT);
NUMBER	(cfg_line_slimit_num, cfg_line_slimit_eol, no_alt,
	 OBJ(int,1), 0, -1, "Maximum number of sessions");
NOPREFIX (cfg_line_slimit_noprefix, cfg_line_slimit_num, cfg_line_slimit_eol);
NVGENS	(cfg_line_slimit_nvgen, cfg_line_slimit_noprefix,
	 asm_line_commands, LINE_SESSION_LIMIT);
KEYWORD	(cfg_line_slimit, cfg_line_slimit_nvgen, ALTERNATE, 
	 "session-limit", "Set maximum number of sessions", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_line_slimit
