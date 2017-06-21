/* $Id: cfg_line_absolute_timeout.h,v 3.1.64.1 1996/05/17 12:15:07 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_absolute_timeout.h,v $
 *------------------------------------------------------------------
 *
 *------------------------------------------------------------------
 * C F G _ L I N E _ A B S O L U T E T I M E O U T . H
 *
 * Tim Kolar, Feburary 1994
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 */
/***************************************************************
 *
 *	This is a line subcommand
 *      absolute_timeout  x
 */
EOLS	(absolutetimeout_eol, asm_line_commands,
	 PARSER_LINE_ABSOLUTE_TIMEOUT);
NUMBER	(absolutetimeout_num, absolutetimeout_eol, no_alt,
	 OBJ(int,1), 0, 10000, "Absolute timeout interval in minutes");
NOPREFIX(absolutetimeout_noprefix, absolutetimeout_num, absolutetimeout_eol);
NVGENS	(absolutetimeout_nvgen, absolutetimeout_noprefix,
	 asm_line_commands, PARSER_LINE_ABSOLUTE_TIMEOUT);
KEYWORD	(line_absolutetimeout, absolutetimeout_nvgen, ALTERNATE,
	 "absolute-timeout",
	 "Set absolute timeout for line disconnection",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	line_absolutetimeout
