/* $Id: exec_show_smf.h,v 3.3.20.1 1996/08/12 16:07:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/smf/exec_show_smf.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S M F . H
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_smf.h,v $
 * Revision 3.3.20.1  1996/08/12  16:07:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:39  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:52:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:54  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/16  23:39:26  widmer
 * CSCdi35982:  show ? displays some wrong info
 *
 * Revision 2.1  1995/06/07  22:25:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show smf [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */

EOLNS	(show_smf_eol, smf_show_command);

INTERFACE (show_smf_interface, show_smf_eol, show_smf_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD  (show_smf, show_smf_interface, ALTERNATE,
	  "smf", "Software MAC filter", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       show_smf
