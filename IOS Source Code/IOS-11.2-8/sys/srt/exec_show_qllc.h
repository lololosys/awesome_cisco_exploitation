/* $Id: exec_show_qllc.h,v 3.3.20.1 1996/08/12 16:07:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_show_qllc.h,v $
 *------------------------------------------------------------------
 * QLLC: show command syntax
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_qllc.h,v $
 * Revision 3.3.20.1  1996/08/12  16:07:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:47  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:11  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* show qllc */

EOLNS      (show_qllc_eol, show_lnx);
INTERFACE (show_qllc_get_interface, show_qllc_eol,
           show_qllc_eol, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD   (show_lnx, show_qllc_get_interface, NONE,
	   "qllc", "Display qllc-llc2 and qllc-sdlc conversion information",
	   PRIV_USER|PRIV_USER_HIDDEN);
TEST_BOOLEAN(show_qllc_test, show_lnx, NONE, ALTERNATE, bridge_enable);

#undef    ALTERNATE
#define   ALTERNATE     show_qllc_test
