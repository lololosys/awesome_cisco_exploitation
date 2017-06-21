/* $Id: exec_show_dlcsw.h,v 3.3.20.1 1996/08/12 16:00:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_show_dlcsw.h,v $
 *------------------------------------------------------------------
 * DLC-SW Show Command Parser Definitions
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_dlcsw.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:28  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:56:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  22:37:27  thille
 * CSCdi37357:  CSCdi28835 broke fr abbreviation for frame-relay
 * Changing "cfrad" to "fras" broke "fr" as an abbreviation for
 * "frame-relay."  Since "fras" will require all four characters to match
 * anyway, there is no reason not to use keyword_mm instead of keyword.
 *
 * Revision 2.1  1995/07/04  01:52:25  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:23:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS    (show_dlcsw_eol, show_dlcsw_command);
KEYWORD_MM (show_dlcsw, show_dlcsw_eol, ALTERNATE,
	    "fras", "FRAS Information",
	    PRIV_USER|PRIV_USER_HIDDEN, 4);

#undef  ALTERNATE
#define ALTERNATE       show_dlcsw

