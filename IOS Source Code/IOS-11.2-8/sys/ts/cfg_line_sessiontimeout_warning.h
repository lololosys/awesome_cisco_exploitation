/* $Id: cfg_line_sessiontimeout_warning.h,v 3.2 1996/06/12 14:41:09 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_sessiontimeout_warning.h,v $
 *------------------------------------------------------------------
 * cfg_line_sessiontimeout_warning.h
 *
 * April 1996 Bill Thomas
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_sessiontimeout_warning.h,v $
 * Revision 3.2  1996/06/12  14:41:09  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 *
 * Revision 3.1  1996/06/12  14:39:27  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * session-disconnect-warning <minutes-warning> [message]
 *
 * OBJ(int,1) = <minutes-warning>
 * OBJ(string,1) = optional warning message
 */



EOLS	(cfg_line_sessiontimeout_warning_eol, asm_line_commands, 
	 LINE_SESSIONTIMEOUT_WARNING);

TEXT    (cfg_line_sessiontimeout_warning_msg, 
	 cfg_line_sessiontimeout_warning_eol,
	 cfg_line_sessiontimeout_warning_eol,
         OBJ(string,1), "One text line optional warning message");

KEYWORD	(cfg_line_sessiontimeout_warning_msg_opt, 
	cfg_line_sessiontimeout_warning_msg,
	cfg_line_sessiontimeout_warning_eol,
	 "message", 
	 "Optional warning message",
	 PRIV_CONF);

NUMBER	(cfg_line_sessiontimeout_warning_num, 
	 cfg_line_sessiontimeout_warning_msg_opt, no_alt,
	 OBJ(int,1), 0, -1, "Session timeout warning in seconds");
NOPREFIX (cfg_line_sessiontimeout_warning_noprefix, 
	  cfg_line_sessiontimeout_warning_num, 
	  cfg_line_sessiontimeout_warning_eol);
NVGENS	(cfg_line_sessiontimeout_warning_nvgen, 
	 cfg_line_sessiontimeout_warning_noprefix,
	 asm_line_commands, LINE_SESSIONTIMEOUT_WARNING);
KEYWORD	(cfg_line_sessiontimeout_warning, cfg_line_sessiontimeout_warning_nvgen,
	 ALTERNATE, 
	 "session-disconnect-warning", 
	 "Set warning countdown for session-timeout",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_line_sessiontimeout_warning
