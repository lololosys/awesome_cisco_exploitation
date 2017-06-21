/* $Id: exec_test_filesys.h,v 3.3.20.2 1996/08/12 16:01:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/exec_test_filesys.h,v $
 *------------------------------------------------------------------
 * Parser defines for the TEST FILESYS command (used for debug)
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_filesys.h,v $
 * Revision 3.3.20.2  1996/08/12  16:01:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/05/07  00:39:16  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:08:37  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:07:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:17  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test filesys
 */
EOLNS	(priv_test_filesys_eol, filesys_command);
KEYWORD	(priv_test_filesys, priv_test_filesys_eol, ALTERNATE,
	 "filesys", NULL /*"Flash file system debug"*/,
	 PRIV_OPR|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_filesys






