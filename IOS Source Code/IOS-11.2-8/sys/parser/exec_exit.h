/* $Id: exec_exit.h,v 3.3 1995/12/15 22:28:10 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/exec_exit.h,v $
 *------------------------------------------------------------------
 * E X E C _ E X I T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_exit.h,v $
 * Revision 3.3  1995/12/15  22:28:10  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:46:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:20:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * exit, quit - exit from the EXEC
 */
EOLS	(exec_exit_eol, exit_exec_command, CMD_EXIT);
KEYWORD	(exec_logout, exec_exit_eol, ALTERNATE,
	 "logout", "Exit from the EXEC", PRIV_MIN | PRIV_INTERACTIVE);
KEYWORD	(exec_quit, exec_exit_eol, exec_logout,
	 "quit", "", PRIV_MIN | PRIV_HIDDEN | PRIV_INTERACTIVE);
KEYWORD	(exec_exit, exec_exit_eol, exec_quit,
	 "exit", "Exit from the EXEC", PRIV_MIN | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_exit
