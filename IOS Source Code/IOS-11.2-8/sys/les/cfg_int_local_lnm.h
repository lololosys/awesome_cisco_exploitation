/* $Id: cfg_int_local_lnm.h,v 3.3.28.1 1996/08/12 16:04:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_int_local_lnm.h,v $
 *------------------------------------------------------------------
 * Parser stuff for local LNM support.
 *
 * September 1994, Johnny Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_local_lnm.h,v $
 * Revision 3.3.28.1  1996/08/12  16:04:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/22  06:37:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:36:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:06:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] local_lnm
 *
 */
EOLNS	(ci_local_lnm_eol, local_lnm_enable_command);
KEYWORD	(ci_local_lnm, ci_local_lnm_eol, NONE,
	 "local-lnm", "Local LNM enable", PRIV_CONF);

TEST_IDBSTATUS(ci_local_lnm_test, ci_local_lnm, NONE, ALTERNATE, IDB_PCBUS);

#undef	ALTERNATE
#define	ALTERNATE	ci_local_lnm_test







