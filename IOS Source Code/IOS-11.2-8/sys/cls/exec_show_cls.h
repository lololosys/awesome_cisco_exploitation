/* $Id: exec_show_cls.h,v 3.4.20.1 1996/08/12 16:00:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_show_cls.h,v $
 *------------------------------------------------------------------
 * EXEC_SHOW_CLS.H 
 *
 * September 8, 1994, Tzu-Ming Tsang
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_cls.h,v $
 * Revision 3.4.20.1  1996/08/12  16:00:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:08:28  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/15  06:07:46  richl
 * CSCdi46890:  show cls is too verbose, add brief option
 *
 * Revision 3.2  1995/11/17  08:56:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:25  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:22:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


EOLNS    (show_cls_eol, show_cls_command);
KEYWORD_ID (show_cls_brief, show_cls_eol, show_cls_eol,
	    OBJ(int,1), TRUE,
	    "brief", "list's a summary of CLSI connections",
	    PRIV_USER);
KEYWORD (show_cls, show_cls_brief, ALTERNATE,
         "cls", "DLC user information",
         PRIV_USER|PRIV_USER_HIDDEN);

#undef  ALTERNATE
#define ALTERNATE       show_cls

