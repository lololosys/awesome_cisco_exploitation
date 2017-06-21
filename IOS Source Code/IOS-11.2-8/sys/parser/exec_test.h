/* $Id: exec_test.h,v 3.3 1996/03/09 22:56:51 widmer Exp $
 * $Source: /release/111/cvs/Xsys/parser/exec_test.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E S T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_test.h,v $
 * Revision 3.3  1996/03/09  22:56:51  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:47:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:26:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************/
KEYWORD_TRANS(exec_test, exec_test_extend_here, no_alt, ALTERNATE,
	      "test", "Test subsystems, memory, and interfaces",
	      PRIV_OPR | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_test
