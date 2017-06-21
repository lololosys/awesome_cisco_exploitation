/* $Id: exec_arap.h,v 3.2.60.1 1996/08/12 16:00:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/arap/exec_arap.h,v $
 *------------------------------------------------------------------
 * ARAP Exec commands
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_arap.h,v $
 * Revision 3.2.60.1  1996/08/12  16:00:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:39:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:34:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:15:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS    (exec_arap_eol, exec_arap_command);

KEYWORD	(exec_arap, exec_arap_eol, NONE,
	 "arap", "Start Appletalk Remote Access", PRIV_USER);

ASSERT (exec_arap_test, exec_arap, ALTERNATE,
	(stdio->capabilities & MAYARAP));

#undef	ALTERNATE
#define	ALTERNATE	exec_arap_test
