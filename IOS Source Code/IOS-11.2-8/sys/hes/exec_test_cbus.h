/* $Id: exec_test_cbus.h,v 3.3.20.1 1996/08/12 16:01:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_test_cbus.h,v $
 *------------------------------------------------------------------
 * Test cbus command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_cbus.h,v $
 * Revision 3.3.20.1  1996/08/12  16:01:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:42  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:16:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:27:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test cbus
 */
EOLNS	(priv_test_cbus_eol, cbus_mci_command);
KEYWORD	(priv_test_cbus, priv_test_cbus_eol, ALTERNATE,
	 "cbus", NULL /*"cbus complex interfaces"*/, PRIV_OPR|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_cbus
