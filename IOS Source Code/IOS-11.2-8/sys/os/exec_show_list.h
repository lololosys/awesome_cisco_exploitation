/* $Id: exec_show_list.h,v 3.4.20.1 1996/08/12 16:05:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_list.h,v $
 *------------------------------------------------------------------
 * exec_show_list.h
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_list.h,v $
 * Revision 3.4.20.1  1996/08/12  16:05:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:34:13  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:09:06  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:46:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:24:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show list
 *
 */

EOLNS	(show_list_summary_eol, list_display);
NUMBER	(show_list_number, show_list_summary_eol, show_list_summary_eol,
	 OBJ(int,1), 1, 0xFFFF, NULL /*"List ID"*/);
KEYWORD (show_list, show_list_number, ALTERNATE,
	 "list", NULL /*"List Manager Status"*/, PRIV_OPR | PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_list

