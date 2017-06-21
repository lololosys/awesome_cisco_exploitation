/* $Id: cfg_line_exit.h,v 3.2 1995/11/17 18:44:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_exit.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E X I T . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_exit.h,v $
 * Revision 3.2  1995/11/17  18:44:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 *	exit - exit from line configuration mode
 */
EOLS	(cl_exit_eol, line_command, LINE_EXIT_CONFIG);
KEYWORD	(cl_exit_kwd, cl_exit_eol, NONE,
	 "exit", "Exit from line configuration mode", PRIV_MIN);
TESTVAR	(cl_exit, cl_exit_kwd, NONE, NONE, NONE, ALTERNATE, sense, TRUE);

#undef	ALTERNATE
#define	ALTERNATE	cl_exit
