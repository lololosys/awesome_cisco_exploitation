/* $Id: exec_show_sum.h,v 3.3.20.1 1996/08/12 16:06:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_sum.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S U M . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_sum.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:29  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:26:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show sum
 * 
 */
EOLNS	(show_sum_eol, show_sum);
KEYWORD (show_sum, show_sum_eol, ALTERNATE,
	 "sum", NULL, PRIV_USER | PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_sum
