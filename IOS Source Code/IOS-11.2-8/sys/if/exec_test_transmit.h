/* $Id: exec_test_transmit.h,v 3.1.40.1 1996/08/12 16:02:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_test_transmit.h,v $
 *------------------------------------------------------------------
 * Test transmit command
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *
 *------------------------------------------------------------------
 * $Log: exec_test_transmit.h,v $
 * Revision 3.1.40.1  1996/08/12  16:02:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:09:33  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:48:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * test transmit
 */
EOLNS    (priv_test_transmit_eol, transmit_frames);
KEYWORD (priv_test_transmit, priv_test_transmit_eol, ALTERNATE,
         "transmit", NULL /*"transmit frames"*/, PRIV_OPR|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_transmit
