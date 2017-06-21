/* $Id: cfg_line_logout_warning.h,v 3.1.64.1 1996/05/17 12:15:09 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_logout_warning.h,v $
 *------------------------------------------------------------------
 *
 *------------------------------------------------------------------
 * C F G _ L I N E _ L O G O U T _ W A R N I N G . H
 *
 * Syed M Irfan Ashraf, May 1994
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 */
/***************************************************************
 *
 *	This is a line subcommand
 *           logout-warning  x
 *      where x is the number of seconds to send the warning to the
 *      tty before logging it out
 */
EOLS	(logoutwarning_eol, asm_line_commands,
	 PARSER_LINE_LOGOUT_WARNING);
NUMBER	(logoutwarning_num, logoutwarning_eol, no_alt,
	 OBJ(int,1), 0, -1, "Warning time in seconds");
NOPREFIX(logoutwarning_noprefix, logoutwarning_num, logoutwarning_eol);
NVGENS	(logoutwarning_nvgen, logoutwarning_noprefix,
	 asm_line_commands, PARSER_LINE_LOGOUT_WARNING);
KEYWORD	(line_logoutwarning, logoutwarning_nvgen, ALTERNATE,
	 "logout-warning",
	 "Set Warning countdown for absolute timeout of line",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	line_logoutwarning
