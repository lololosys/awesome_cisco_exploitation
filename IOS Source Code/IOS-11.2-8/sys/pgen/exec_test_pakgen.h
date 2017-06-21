/* $Id: exec_test_pakgen.h,v 3.3.20.1 1996/08/12 16:06:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_test_pakgen.h,v $
 *------------------------------------------------------------------
 * Test pakgen command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_pakgen.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:35  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:56:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test pakgen
 */
EOLNS	(priv_test_pakgen_eol, test_pakgen);
KEYWORD (priv_test_pakgen, priv_test_pakgen_eol, ALTERNATE,
	 "pakgen", "MAC frame packet generator", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_pakgen
