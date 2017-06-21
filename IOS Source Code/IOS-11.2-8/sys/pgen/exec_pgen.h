/* $Id: exec_pgen.h,v 3.2.60.1 1996/08/12 16:06:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pgen/exec_pgen.h,v $
 *------------------------------------------------------------------
 * E X E C _ P G E N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_pgen.h,v $
 * Revision 3.2.60.1  1996/08/12  16:06:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:56:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/***************************************************************
 * pgen
 */

EOLNS	(exec_pgen_eol, pgen_command);

KEYWORD	(exec_pgen, exec_pgen_eol, ALTERNATE,
	 "pagent", NULL /*"PAcket GENeration Tool"*/,
	 PRIV_ROOT | PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	exec_pgen
