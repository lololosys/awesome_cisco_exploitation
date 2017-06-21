/* $Id: cfg_line_flush_activate.h,v 3.1.2.1 1996/07/04 00:29:12 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_flush_activate.h,v $
 *------------------------------------------------------------------
 * Parse chains for "flush-at-activation" line subcommand
 *
 * July 1996, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_flush_activate.h,v $
 * Revision 3.1.2.1  1996/07/04  00:29:12  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.1  1996/07/03  23:14:37  irfan
 * File needed for "flush-at-activation" line subcommand.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 * [no] flush-at-activation
 *
 */

EOLS	(cfg_line_flush_activate_eol, asm_line_commands,
	 PARSER_LINE_FLUSH_ACTIVATE);

KEYWORD	(cfg_line_flush_activate, cfg_line_flush_activate_eol, ALTERNATE,
	 "flush-at-activation", "Clear input stream at activation",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_line_flush_activate
