/* $Id: exec_show_timers.h,v 3.3.20.1 1996/08/12 16:05:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_timers.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ T I M E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_timers.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:11  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:26:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show timers
 * 
 */
EOLNS	(show_timers_eol, show_timers);
KEYWORD (show_timers, show_timers_eol, ALTERNATE,
	 "timers", NULL /*"Display active and idle timers"*/,
	 PRIV_USER|PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_timers
