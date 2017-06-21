/* $Id: exec_show_flh.h,v 3.3.28.1 1996/08/12 16:04:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_flh.h,v $
 *------------------------------------------------------------------
 * Parser commands for flh
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_flh.h,v $
 * Revision 3.3.28.1  1996/08/12  16:04:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/22  06:37:24  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:37:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:18:00  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * show flh-log
 *
 */

EOLNS    (show_flh_eol, flhbuf_print);
KEYWORD (show_flh,show_flh_eol, ALTERNATE,
	 "flh-log", "Flash Load Helper log buffer", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_flh
