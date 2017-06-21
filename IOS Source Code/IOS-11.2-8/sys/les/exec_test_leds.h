/* $Id: exec_test_leds.h,v 3.3.20.1 1996/08/12 16:04:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_test_leds.h,v $
 *------------------------------------------------------------------
 * Test leds command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_leds.h,v $
 * Revision 3.3.20.1  1996/08/12  16:04:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:58  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:37:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:27:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test leds
 */
EOLNS    (priv_test_leds_eol, test_leds);
KEYWORD (priv_test_leds, priv_test_leds_eol, ALTERNATE,
	 "leds", "LEDs", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_leds
