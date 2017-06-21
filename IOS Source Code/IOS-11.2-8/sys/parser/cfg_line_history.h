/* $Id: cfg_line_history.h,v 3.2 1995/11/17 18:44:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_history.h,v $
 *------------------------------------------------------------------
 * cfg_line_history.h
 *    Terminal history commands
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_history.h,v $
 * Revision 3.2  1995/11/17  18:44:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************
 * history			; turn history on
 * no history			; turn history off
 */
EOLS	(cl_history_eol, line_command, PARSER_LINE_HISTORY);

/****************************************************************
 * history size <num>		; set history size to <num>
 * no history size		; set history size to default
 */
PARAMS_KEYONLY(cl_history_size, cl_history_eol, "size",
	       OBJ(int,1), 0, MAX_HISTORY_SIZE,
	       line_command, PARSER_LINE_HISTORY_SIZE,
	       "Set history buffer size",
	       "Size of history buffer", PRIV_USER);

KEYWORD	(cl_history, cl_history_size, ALTERNATE,
	 "history", "Enable and control the command history function",
	 PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	cl_history
