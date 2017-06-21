/* $Id: exec_lock.h,v 3.3.46.1 1996/08/12 16:06:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_lock.h,v $
 *------------------------------------------------------------------
 * E X E C _ L O C K . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_lock.h,v $
 * Revision 3.3.46.1  1996/08/12  16:06:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:12  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:46:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:21:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * lock
 */
EOLNS	(exec_lock_eol, lock_command);
KEYWORD	(exec_lock, exec_lock_eol, ALTERNATE,
	 "lock", "Lock the terminal", PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_lock
