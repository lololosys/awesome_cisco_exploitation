/* $Id: exec_show_slip.h,v 3.3.20.1 1996/08/12 16:08:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_show_slip.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S L I P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_slip.h,v $
 * Revision 3.3.20.1  1996/08/12  16:08:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:10:02  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:54:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:59  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show slip
 * 
 */

EOLNS	(show_slip_eol, show_slip);
KEYWORD (show_slip, show_slip_eol, ALTERNATE,
	 "slip", NULL, PRIV_USER|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_slip
