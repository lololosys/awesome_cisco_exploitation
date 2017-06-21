/* $Id: cfg_hub_exit.h,v 3.2.62.1 1996/08/12 16:04:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_hub_exit.h,v $
 *------------------------------------------------------------------
 * Hub exit command
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub_exit.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:36:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * exit
 */

EOLNS	(hub_exit_eol, hub_exit_command);
KEYWORD	(hub_exit, hub_exit_eol, NONE,
	 "exit", "Exit from hub configuration mode", PRIV_USER);
TESTVAR	(hub_exit_test, hub_exit, NONE,
	 NONE, NONE, ALTERNATE, sense, TRUE);

#undef	ALTERNATE
#define	ALTERNATE	hub_exit_test
