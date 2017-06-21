/* $Id: cfg_line_dispatchtimeout.h,v 3.2 1995/11/17 18:53:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_dispatchtimeout.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D I S P A T C H T I M E O U T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_dispatchtimeout.h,v $
 * Revision 3.2  1995/11/17  18:53:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:45  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 *
 */
PARAMS_KEYONLY(dispatch_timeout, ALTERNATE, "dispatch-timeout",
	       OBJ(int,1), 0, -2, 
	       asm_line_commands, PARSER_LINE_DISPATCH_TIMEOUT,
	       "Set the dispatch timer",
	       "Dispatch timer in milliseconds", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	dispatch_timeout
