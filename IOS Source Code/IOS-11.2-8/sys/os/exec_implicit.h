/* $Id: exec_implicit.h,v 3.3 1996/03/09 22:56:33 widmer Exp $
 * $Source: /release/111/cvs/Xsys/os/exec_implicit.h,v $
 *------------------------------------------------------------------
 * exec_implicit.h - Implicit exec commands
 *
 * March 3, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains the parse chains to handle implicit exec commands
 *------------------------------------------------------------------
 * $Log: exec_implicit.h,v $
 * Revision 3.3  1996/03/09  22:56:33  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:46:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:21:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Look for a session number to resume, in the range 1 - 99
 */


EOLS ( exec_implicit_resume_end, resume_command, 1);

DECIMAL (exec_implicit_resume, exec_implicit_resume_end, NONE,
	OBJ(int, 1), 1, 99, "Session number to resume");

/*
 * Ensure that we have some sessions to resume
 */

ASSERT (exec_implicit_resume_test, exec_implicit_resume, ALTERNATE,
	(stdio->nconns != 0) && (!(csb->flags & CONFIG_HTTP)));

#undef ALTERNATE
#define ALTERNATE exec_implicit_resume_test
